/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPolicerUT.c
*
* DESCRIPTION:
*       Unit Tests for Ingress Policing Engine functions
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>

#include <cpss/generic/cos/cpssCosTypes.h>

#include <cpss/exMx/exMxGen/policer/cpssExMxPolicer.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Constant to define Tiger + Samba/Rumba (98MX6x8, 98MX6x5, 98EX1x6) device family set */
#define UTF_TIGER_SAMBA_FAMILY_SET_CNS     (UTF_CPSS_PP_FAMILY_TIGER_CNS | \
                                            UTF_CPSS_PP_FAMILY_SAMBA_CNS)

/* Invalid enumeration value used for testing */
#define POLICER_INVALID_ENUM_CNS       0x5AAAAAA5

/* Internal functions declaration */

/*  Internal function. Is used for filling Policer meterEntryCnfg structure */
/*  with default values which are used for most of all tests.               */
static void policerMeterEntryCnfgDefaultSet(IN
            CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC *meterEntryCnfgPtr);

/*  Internal function. Is used for filling Policer countEntryCnfg structure */
/*  with default values which are used for most of all tests.               */
static void policerCountEntryCnfgDefaultSet(IN
            CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC *countEntryCnfgPtr);

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerCountingEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerCountingEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function with enable [GT_FALSE and GT_TRUE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_FALSE and GT_TRUE].       */
        /* Expected: GT_OK.                                             */
        enable = GT_FALSE;

        st = cpssExMxPolicerCountingEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_TRUE;

        st = cpssExMxPolicerCountingEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_TRUE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerCountingEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerCountingEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerMeteringEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerMeteringEnable)
{
/*
ITERATE_DEVICES 
1.1. Call function with enable [GT_FALSE and GT_TRUE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_FALSE and GT_TRUE].       */
        /* Expected: GT_OK.                                             */
        enable = GT_FALSE;

        st = cpssExMxPolicerMeteringEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_TRUE;

        st = cpssExMxPolicerMeteringEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_TRUE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerMeteringEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerMeteringEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerPacketSizeModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_POLICER_PACKET_SIZE_MODE_ENT    lengthType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerPacketSizeModeSet)
{
/*
ITERATE_DEVICES 
1.1. Call function with lengthType [CPSS_POLICER_PACKET_SIZE_L3_ONLY_E / CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E / CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E]. Expected: GT_OK.
1.2. Call function with lengthType [CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E]. Expected: NON GT_OK.
1.3. Check out-of-range enum. Call with lengthType[0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                           st = GT_OK;
    GT_U8                               dev;
    CPSS_POLICER_PACKET_SIZE_MODE_ENT   lengthType;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with lengthType                           */
        /* [CPSS_POLICER_PACKET_SIZE_L3_ONLY_E /                        */
        /*  CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E /                     */
        /*  CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E].                     */
        /* Expected: GT_OK.                                             */
        lengthType = CPSS_POLICER_PACKET_SIZE_L3_ONLY_E;

        st = cpssExMxPolicerPacketSizeModeSet(dev, lengthType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lengthType);

        lengthType = CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E;

        st = cpssExMxPolicerPacketSizeModeSet(dev, lengthType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lengthType);

        lengthType = CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E;

        st = cpssExMxPolicerPacketSizeModeSet(dev, lengthType);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lengthType);

        /* 1.2. Call function with lengthType                           */
        /* [CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E]                */
        /* Expected: NON GT_OK.                                         */
        lengthType = CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E;

        st = cpssExMxPolicerPacketSizeModeSet(dev, lengthType);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lengthType);

        /* 1.3. Check out-of-range enum. Call with lengthType   */
        /* [0x5AAAAAA5]. Expected: GT_BAD_PARAM.                */
        lengthType = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerPacketSizeModeSet(dev, lengthType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, lengthType);
    }

    lengthType = CPSS_POLICER_PACKET_SIZE_L3_ONLY_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerPacketSizeModeSet(dev, lengthType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerPacketSizeModeSet(dev, lengthType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerMeterDuRatioSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMX_POLICER_METER_DU_RATIO_ENT    duRatio
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerMeterDuRatioSet)
{
/*
ITERATE_DEVICES 
1.1. Call function with duRatio [CPSS_EXMX_POLICER_METER_DU_MODE_RATIO_64_E / CPSS_EXMX_POLICER_METER_DU_MODE_RATIO_16_E]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.  
1.2. Check out-of-range enum. For Tiger devices call with duRatio [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                               st = GT_OK;
    GT_U8                                   dev;
    CPSS_EXMX_POLICER_METER_DU_RATIO_ENT    duRatio;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with duRatio                                  */
        /* [CPSS_EXMX_POLICER_METER_DU_MODE_RATIO_64_E /                    */
        /*  CPSS_EXMX_POLICER_METER_DU_MODE_RATIO_16_E].                    */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.   */  

        duRatio = CPSS_EXMX_POLICER_METER_DU_MODE_RATIO_64_E;

        st = cpssExMxPolicerMeterDuRatioSet(dev, duRatio);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "Tiger device: %d", dev, duRatio);

        duRatio = CPSS_EXMX_POLICER_METER_DU_MODE_RATIO_16_E;

        st = cpssExMxPolicerMeterDuRatioSet(dev, duRatio);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "Tiger device: %d", dev, duRatio);

        /* 1.2. Check out-of-range enum. For Tiger devices call */
        /* with duRatio [0x5AAAAAA5]. Expected: GT_BAD_PARAM.   */
        duRatio = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerMeterDuRatioSet(dev, duRatio);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, duRatio);
    }

    duRatio = CPSS_EXMX_POLICER_METER_DU_MODE_RATIO_64_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerMeterDuRatioSet(dev, duRatio);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerMeterDuRatioSet(dev, duRatio);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerDefaultDscpSet
(
    IN GT_U8    devNum,
    IN GT_U8    dscp
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerDefaultDscpSet)
{
/*
ITERATE_DEVICES 
1.1. Call function with dscp [CPSS_DSCP_RANGE_CNS-1]. Expected: GT_OK.
1.2. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS]. Expected: NON GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       dscp;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with dscp [CPSS_DSCP_RANGE_CNS-1]. Expected: GT_OK.*/
        dscp = CPSS_DSCP_RANGE_CNS-1;

        st = cpssExMxPolicerDefaultDscpSet(dev, dscp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);

        /* 1.2. Call function with out of range dscp [CPSS_DSCP_RANGE_CNS]. */
        /* Expected: NON GT_OK.                                             */
        dscp = CPSS_DSCP_RANGE_CNS;

        st = cpssExMxPolicerDefaultDscpSet(dev, dscp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, dscp);
    }

    dscp = CPSS_DSCP_RANGE_CNS-1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerDefaultDscpSet(dev, dscp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerDefaultDscpSet(dev, dscp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerL2RemarkModeSet
(
    IN GT_U8                                    devNum,
    IN CPSS_EXMX_POLICER_L2_REMARK_MODE_ENT     mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerL2RemarkModeSet)
{
/*
ITERATE_DEVICES 
1.1. Call function with mode [CPSS_EXMX_POLICER_L2_REMARK_TC_E / CPSS_EXMX_POLICER_L2_REMARK_UP_E]. Expected: GT_OK for Tiger and Samba/Rumba devices and GT_BAD_PARAM for others.
1.2. Check out-of-range enum. For Tiger and Samba/Rumba devices call with mode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                               st = GT_OK;
    GT_U8                                   dev;
    CPSS_EXMX_POLICER_L2_REMARK_MODE_ENT    mode;

    st = prvUtfNextDeviceReset(&dev, UTF_TIGER_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with mode [CPSS_EXMX_POLICER_L2_REMARK_TC_E   */
        /* / CPSS_EXMX_POLICER_L2_REMARK_UP_E]. Expected: GT_OK for Tiger   */
        /* and Samba/Rumba devices and GT_BAD_PARAM for others.             */
        mode = CPSS_EXMX_POLICER_L2_REMARK_TC_E;

        st = cpssExMxPolicerL2RemarkModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        mode = CPSS_EXMX_POLICER_L2_REMARK_UP_E;

        st = cpssExMxPolicerL2RemarkModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /* 1.2. Check out-of-range enum. For Tiger and Samba/Rumba devices */
        /* call with mode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.            */
        mode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerL2RemarkModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    mode = CPSS_EXMX_POLICER_L2_REMARK_TC_E;

    st = prvUtfNextDeviceReset(&dev, UTF_TIGER_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerL2RemarkModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerL2RemarkModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerModifyUpPacketTypeSet
(
    IN GT_U8                                        devNum,
    IN CPSS_EXMX_POLICER_UP_REMARK_PACKET_TYPE_ENT  packetType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerModifyUpPacketTypeSet)
{
/*
ITERATE_DEVICES 
1.1. Call function with packetType [CPSS_EXMX_POLICER_UP_REMARK_IP_PACKETS_ONLY_E / CPSS_EXMX_POLICER_UP_REMARK_BOTH_E / CPSS_EXMX_POLICER_UP_REMARK_DISABLED_E]. Expected: GT_OK for Tiger and Samba/Rumba devices and GT_BAD_PARAM for others..
1.2. Check out-of-range enum. For Tiger and Samba/Rumba devices call with packetType [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                                   st = GT_OK;
    GT_U8                                       dev;
    CPSS_EXMX_POLICER_UP_REMARK_PACKET_TYPE_ENT packetType;

    st = prvUtfNextDeviceReset(&dev, UTF_TIGER_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with packetType                           */
        /* [CPSS_EXMX_POLICER_UP_REMARK_IP_PACKETS_ONLY_E /             */
        /* CPSS_EXMX_POLICER_UP_REMARK_BOTH_E /                         */
        /* CPSS_EXMX_POLICER_UP_REMARK_DISABLED_E]. Expected: GT_OK for */
        /* Tiger and Samba/Rumba devices and GT_BAD_PARAM for others.   */
        packetType = CPSS_EXMX_POLICER_UP_REMARK_IP_PACKETS_ONLY_E;

        st = cpssExMxPolicerModifyUpPacketTypeSet(dev, packetType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, packetType);

        packetType = CPSS_EXMX_POLICER_UP_REMARK_BOTH_E;

        st = cpssExMxPolicerModifyUpPacketTypeSet(dev, packetType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, packetType);

        packetType = CPSS_EXMX_POLICER_UP_REMARK_DISABLED_E;

        st = cpssExMxPolicerModifyUpPacketTypeSet(dev, packetType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, packetType);


        /* 1.2. Check out-of-range enum. For Tiger and Samba/Rumba devices  */
        /* call with packetType [0x5AAAAAA5]. Expected: GT_BAD_PARAM.       */
        packetType = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerModifyUpPacketTypeSet(dev, packetType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, packetType);
    }

    packetType = CPSS_EXMX_POLICER_UP_REMARK_IP_PACKETS_ONLY_E;

    st = prvUtfNextDeviceReset(&dev, UTF_TIGER_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerModifyUpPacketTypeSet(dev, packetType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerModifyUpPacketTypeSet(dev, packetType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntrySet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  policerEntryIndex,
    IN  CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   *meterEntryCnfgPtr,
    IN  CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   *countEntryCnfgPtr,
    OUT CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   *lbParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntrySet)
{
/*
ITERATE_DEVICES 
1.1. Check valid params. Call with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Call cpssExMxPolicerEntryGet with policerEntryIndex [maxNumOfPolicerEntries-1], non NULL meterEntryCnfgPtr and countEntryCnfgPtr. Expected: GT_OK and the same values as written - check by fields.
1.3. Check out-of-range policerEntryIndex. Call with policerEntryIndex [maxNumOfPolicerEntries], other parameters same as in 1.1. Expected: non GT_OK.
1.4. Call with meterEntryCnfgPtr [NULL], other parameters same as in 1.1. Expected: GT_OK.
1.5. Call with countEntryCnfgPtr [NULL], other parameters same as in 1.1. Expected: GT_OK.
1.6. Call with lbParamsPtr [NULL] and meterEntryCnfgPtr [NULL] and other parameters same as in 1.1. Expected: GT_OK.
1.7. Call with countEntryCnfgPtr [NULL], meterEntryCnfgPtr [ NULL] and , other parameters same as in 1.1. Expected: NON GT_OK.
1.8. Check for NULL pointers. Call with lbParamsPtr [NULL], other parameters same as in 1.1. Expected: NON GT_OK.

meterEntryCnfg fields check:
1.9. Call function with meterEntryCnfgPtr -> modifyTcEnable [GT_TRUE], and other parameters the same as in 1.5. Expected: GT_OK for Samba/Rumba and Tiger and NON GT_OK for others (not supported).
1.10. Call function with meterEntryCnfgPtr -> modifyExternalCosEnable [GT_TRUE], and other parameters the same as in 1.5. Expected: GT_OK for Samba/Rumba and Tiger and NON GT_OK for others (not supported).
1.11. Call function with meterEntryCnfgPtr -> remarkDp [GT_TRUE], and other parameters the same as in 1.5. Expected: GT_OK (it's ignored by Samba/Rumba and Tiger, but not unsupported).
1.12. Call function with meterEntryCnfgPtr -> meterColorMode [0x5AAAAAA5], and other parameters the same as in 1.5. Expected: GT_BAD_PARAM.
1.13. Call function with meterEntryCnfgPtr -> meterMode [0x5AAAAAA5], and other parameters the same as in 1.5. Expected: GT_BAD_PARAM.
1.14. Call function with meterEntryCnfgPtr -> mngCounterSet [0x5AAAAAA5], and other parameters the same as in 1.5. Expected: GT_BAD_PARAM.

countEntryCnfg fields check:
1.15. Call function with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [NULL], lbParamsPtr [NULL], countEntryCnfgPtr [doCounting = GT_TRUE; counterDuMode = CPSS_POLICER_EXMX_COUNT_DU16_E; counterMode = CPSS_EXMX_POLICER_COUNT_HYBRID_MODE_E]. Expected: GT_OK.
1.16. Call function with countEntryCnfgPtr -> counterDuMode [0x5AAAAAA5], and other parameters the same as in 1.15. Expected: GT_BAD_PARAM.
1.17. Call function with countEntryCnfgPtr -> counterMode [0x5AAAAA5], and other parameters the same as in 1.5. Expected: NON GT_OK for Samba/Rumba and Tiger and GT_OK for others (this parameter is not used be other devices).

1.18. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS                               st = GT_OK;
    GT_U8                                   dev;

    GT_U32                                  policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    GT_U32                                  maxNumOfPolicerEntries = 0;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   retMeterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   retCountEntryCnfg;

    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;
    GT_BOOL                                 failureWas;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;

            st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                    &meterEntryCnfg, &countEntryCnfg, &lbParams);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);

            continue;
        }

        /* 1.1. Check valid params. Call with policerEntryIndex             */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_POLICER_COLOR_BLIND_E, meterMode=           */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /* CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */

        policerEntryIndex = maxNumOfPolicerEntries - 1;
        /* policerMeterEntryCnfgDefaultSet(&meterEntryCnfg); */

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);

        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.2. Call cpssExMxPolicerEntryGet with policerEntryIndex         */
        /* [maxNumOfPolicerEntries-1], non NULL meterEntryCnfgPtr and       */
        /* countEntryCnfgPtr.Expected: GT_OK and the same values as written.*/

        /* erase values before read to make them initially different from set values */
        cpssOsMemSet((GT_VOID*)&retMeterEntryCnfg, 0xFF,
                     sizeof(CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC));
        cpssOsMemSet((GT_VOID*)&retCountEntryCnfg, 0xFF,
                     sizeof(CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC));

        st = cpssExMxPolicerEntryGet(dev, policerEntryIndex, &retMeterEntryCnfg,
                                     &retCountEntryCnfg);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryGet: %d, %d",
                                     dev, policerEntryIndex);

        if (GT_OK == st)
        {
            failureWas = GT_FALSE;

            if ((meterEntryCnfg.doMetering != retMeterEntryCnfg.doMetering) ||
                (meterEntryCnfg.remarkBasedOnL3Cos != retMeterEntryCnfg.remarkBasedOnL3Cos) ||
                (meterEntryCnfg.remarkBasedOnL2Cos != retMeterEntryCnfg.remarkBasedOnL2Cos) ||
                (meterEntryCnfg.dropRed != retMeterEntryCnfg.dropRed) ||
                (meterEntryCnfg.meterColorMode != retMeterEntryCnfg.meterColorMode) ||
                (meterEntryCnfg.meterMode != retMeterEntryCnfg.meterMode) ||
                (meterEntryCnfg.mngCounterSet != retMeterEntryCnfg.mngCounterSet))
            {
                failureWas = GT_TRUE;
            }

            if ((CPSS_PP_FAMILY_TIGER_E == devFamily) || (CPSS_PP_FAMILY_SAMBA_E == devFamily))
            {
                if ((meterEntryCnfg.modifyTcEnable != retMeterEntryCnfg.modifyTcEnable) ||
                    (meterEntryCnfg.modifyExternalCosEnable != retMeterEntryCnfg.modifyExternalCosEnable))
                {
                    failureWas = GT_TRUE;
                }
            }
            else
            {/* non - Tiger/Samba device */
                if ((GT_FALSE == meterEntryCnfg.remarkBasedOnL3Cos) &&
                    (meterEntryCnfg.remarkDp != retMeterEntryCnfg.remarkDp))
                {
                    failureWas = GT_TRUE;
                }
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another meterEntryCnfg than has been written [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);

            failureWas = GT_FALSE;

            if (countEntryCnfg.doCounting != retCountEntryCnfg.doCounting)
            {
                failureWas = GT_TRUE;
            }

            if ((GT_TRUE == countEntryCnfg.doCounting) &&
                (countEntryCnfg.counterDuMode != retCountEntryCnfg.counterDuMode))
            {
                failureWas = GT_TRUE;
            }

            if (((CPSS_PP_FAMILY_TIGER_E == devFamily) ||
                 (CPSS_PP_FAMILY_SAMBA_E == devFamily)) &&
                (countEntryCnfg.counterMode != retCountEntryCnfg.counterMode))
            {
                failureWas = GT_TRUE;
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another countEntryCnfg than has been written [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);
        }

        /* 1.3. Check out-of-range policerEntryIndex. Call with         */
        /* policerEntryIndex [maxNumOfPolicerEntries], other parameters */
        /* same as in 1.1. Expected: non GT_OK.                         */
        policerEntryIndex = maxNumOfPolicerEntries;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* Restore valid value */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        /* 1.4. Call with meterEntryCnfgPtr [NULL],             */
        /* other parameters same as in 1.1. Expected: GT_OK.    */

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                NULL, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                                     "%d, meterEntryCnfgPtr = NULL", dev);

        /* 1.5. Call with countEntryCnfgPtr [NULL],             */
        /* other parameters same as in 1.1. Expected: GT_OK.    */

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, NULL, &lbParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                                     "%d, countEntryCnfgPtr = NULL", dev);

        /* 1.6. Call with lbParamsPtr [NULL] and meterEntryCnfgPtr [NULL]
        and other parameters same as in 1.1. Expected: GT_OK.*/
        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                NULL, &countEntryCnfg, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
            "%d, lbParamsPtr = NULL, meterEntryCnfgPtr = NULL", dev);

        /* 1.7. Call with countEntryCnfgPtr [NULL], meterEntryCnfgPtr [NULL]
        and , other parameters same as in 1.1. Expected: NON GT_OK. */
        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                NULL, NULL, &lbParams);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, 
            "%d, countEntryCnfgPtr = NULL, meterEntryCnfgPtr = NULL", dev);

        /* 1.8. Check for NULL pointers. Call with lbParamsPtr [NULL],  */
        /* other parameters same as in 1.1. Expected: NON GT_OK.        */
        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, NULL);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, 
            "%d, lbParamsPtr = NULL, meterEntryCnfg != NULL", dev);

        /* 1.9. Call function with meterEntryCnfgPtr -> modifyTcEnable [GT_TRUE],
        and other parameters the same as in 1.5.
        Expected: GT_OK for Samba/Rumba and Tiger and
        NON GT_OK for others (not supported). */
        meterEntryCnfg.modifyTcEnable = GT_TRUE;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, NULL, &lbParams);
        if ((CPSS_PP_FAMILY_TIGER_E == devFamily) ||
            (CPSS_PP_FAMILY_SAMBA_E == devFamily))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "non Tiger or Samba/Rumba device: %d, %d, meterEntryCnfgPtr->modifyTcEnable = %d",
                dev, policerEntryIndex, meterEntryCnfg.modifyTcEnable);
        }

        meterEntryCnfg.modifyTcEnable = GT_FALSE; /* restore */

        /* 1.10. Call function with
        meterEntryCnfgPtr -> modifyExternalCosEnable [GT_TRUE],
        and other parameters the same as in 1.5.
        Expected: GT_OK for Samba/Rumba and Tiger and
        NON GT_OK for others (not supported). */
        meterEntryCnfg.modifyExternalCosEnable = GT_TRUE;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, NULL, &lbParams);
        if ((CPSS_PP_FAMILY_TIGER_E == devFamily) ||
            (CPSS_PP_FAMILY_SAMBA_E == devFamily))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "non Tiger or Samba/Rumba device: %d, %d, meterEntryCnfgPtr->modifyExternalCosEnable = %d",
                dev, policerEntryIndex, meterEntryCnfg.modifyExternalCosEnable);
        }

        meterEntryCnfg.modifyExternalCosEnable = GT_FALSE; /* restore */

        /* 1.11. Call function with meterEntryCnfgPtr -> remarkDp [GT_TRUE],
        and other parameters the same as in 1.5.
        Expected: GT_OK (it's ignored by Samba/Rumba and Tiger,
        but not unsupported). */
        meterEntryCnfg.remarkDp = GT_TRUE;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, NULL, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, %d, meterEntryCnfgPtr->remarkDp = %d",
            dev, policerEntryIndex, meterEntryCnfg.remarkDp);

        meterEntryCnfg.remarkDp = GT_FALSE; /* restore */

        /* 1.12. Call function with
        meterEntryCnfgPtr -> meterColorMode [0x5AAAAAA5],
        and other parameters the same as in 1.5. Expected: GT_BAD_PARAM. */
        meterEntryCnfg.meterColorMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, NULL, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
            "%d, %d, meterEntryCnfgPtr->meterColorMode = %d",
            dev, policerEntryIndex, meterEntryCnfg.meterColorMode);

        meterEntryCnfg.meterColorMode = CPSS_POLICER_COLOR_BLIND_E; /* restore */

        /* 1.13. Call function with meterEntryCnfgPtr -> meterMode [0x5AAAAAA5],
        and other parameters the same as in 1.5. Expected: GT_BAD_PARAM. */
        meterEntryCnfg.meterMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, NULL, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
            "%d, %d, meterEntryCnfgPtr->meterMode = %d",
            dev, policerEntryIndex, meterEntryCnfg.meterMode);

        meterEntryCnfg.meterMode = CPSS_EXMX_POLICER_METER_SR_TCM_E; /* restore */

        /* 1.14. Call function with meterEntryCnfgPtr -> mngCounterSet [0x5AAAAAA5],
        and other parameters the same as in 1.5. Expected: GT_BAD_PARAM. */
        meterEntryCnfg.mngCounterSet = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, NULL, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
            "%d, %d, meterEntryCnfgPtr->mngCounterSet = %d",
            dev, policerEntryIndex, meterEntryCnfg.mngCounterSet);

        meterEntryCnfg.mngCounterSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E; /* restore */

        /* 1.15. Call function with policerEntryIndex [maxNumOfPolicerEntries-1],
        meterEntryCnfgPtr [NULL], lbParamsPtr [NULL],
        countEntryCnfgPtr [doCounting = GT_TRUE;
        counterDuMode = CPSS_POLICER_EXMX_COUNT_DU16_E;
        counterMode = CPSS_EXMX_POLICER_COUNT_HYBRID_MODE_E].
        Expected: GT_OK. */
        countEntryCnfg.doCounting = GT_TRUE;
        countEntryCnfg.counterDuMode = CPSS_POLICER_EXMX_COUNT_DU16_E;
        countEntryCnfg.counterMode = CPSS_EXMX_POLICER_COUNT_HYBRID_MODE_E;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                NULL, &countEntryCnfg, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.16. Call function with
        countEntryCnfgPtr -> counterDuMode [0x5AAAAAA5],
        and other parameters the same as in 1.15. Expected: NON GT_OK
        for doCounting == GT_TRUE and GT_OK otherwise (if doCounting == GT_FALSE,
        counterDuMode is non-valid). */
        countEntryCnfg.counterDuMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                NULL, &countEntryCnfg, NULL);
        if (GT_TRUE == countEntryCnfg.doCounting)
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                "%d, %d, countEntryCnfgPtr [doCounting = %d, counterDuMode = %d]",
                dev, policerEntryIndex, countEntryCnfg.doCounting, countEntryCnfg.counterDuMode);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                "%d, %d, countEntryCnfgPtr [doCounting = %d, counterDuMode = %d]",
                dev, policerEntryIndex, countEntryCnfg.doCounting, countEntryCnfg.counterDuMode);
        }

        countEntryCnfg.counterDuMode = CPSS_POLICER_EXMX_COUNT_DU16_E;

        /* 1.17. Call function with
        countEntryCnfgPtr -> counterMode [0x5AAAAA5],
        and other parameters the same as in 1.5.
        Expected: NON GT_OK for Samba/Rumba and Tiger and
        GT_OK for others (this parameter is not used by other devices). */
        countEntryCnfg.counterMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                NULL, &countEntryCnfg, NULL);
        if ((devFamily == CPSS_PP_FAMILY_TIGER_E) ||
            (devFamily == CPSS_PP_FAMILY_SAMBA_E))
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "Tiger or Samba/Rumba device: %d, %d, countEntryCnfgPtr->counterDuMode = %d",
                dev, policerEntryIndex, countEntryCnfg.counterDuMode);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
        }

        policerCountEntryCnfgDefaultSet(&countEntryCnfg); /* fully restore counterEntry */

        /* 1.18. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex  */
        /* [maxNumOfPolicerEntries-1] to cleanup after testing.             */
        /* Expected: GT_OK.                                                 */
        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);
    }

    /* set correct params */
    policerEntryIndex = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
            &meterEntryCnfg, &countEntryCnfg, &lbParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryCountingUpdate
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  policerEntryIndex,
    IN  CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   *countEntryCnfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryCountingUpdate)
{
/*
ITERATE_DEVICES 
1.1. Write policer entry for testing. Call cpssExMxPolicerEntrySet with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Make counting update (change first and last fields in countEntryCnfgPtr compared to 1.1). Call with policerEntryIndex [maxNumOfPolicerEntries-1], countEntryCnfgPtr[ doCounting=GT_TRUE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_CDU_ONLY_E ]. Expected: GT_OK.
1.3. Call cpssExMxPolicerEntryGet with policerEntryIndex [maxNumOfPolicerEntries-1], non NULL meterEntryCnfgPtr and countEntryCnfgPtr. Expected: GT_OK, for meterEntryCnfgPtr - same values as written at 1.1; for countEntryCnfgPtr - same values as written at 1.2 - check by fields.
1.4. Check out-of-range policerEntryIndex. Call with policerEntryIndex [maxNumOfPolicerEntries], other parameters same as in 1.2. Expected: non GT_OK.
1.5. Check for NULL pointers. Call with countEntryCnfgPtr [NULL], other parameters same as in 1.2. Expected: GT_BAD_PTR.

countEntryCnfg fields check:
1.6. Call function with countEntryCnfgPtr -> counterDuMode [0x5AAAAAA5], and other parameters the same as in 1.2. Expected: GT_BAD_PARAM.
1.7. Call function with countEntryCnfgPtr -> counterMode [0x5AAAAA5], and other parameters the same as in 1.2. Expected: NON GT_OK for Samba/Rumba and Tiger and GT_OK for others (this parameter is not used be other devices).

1.8. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS                               st = GT_OK;
    GT_U8                                   dev;

    GT_U32                                  policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   UpdateCountEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    GT_U32                                  maxNumOfPolicerEntries = 0;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   retMeterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   retCountEntryCnfg;

    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;
    GT_BOOL                                 failureWas;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);

    policerCountEntryCnfgDefaultSet(&UpdateCountEntryCnfg);
    UpdateCountEntryCnfg.doCounting = GT_TRUE;
    UpdateCountEntryCnfg.counterMode = CPSS_EXMX_POLICER_COUNT_CDU_ONLY_E;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;

            st = cpssExMxPolicerEntryCountingUpdate(dev, policerEntryIndex,
                                                    &UpdateCountEntryCnfg);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);

            continue;
        }

        /* 1.1. Write policer entry for testing. Call                       */
        /* cpssExMxPolicerEntrySet  with policerEntryIndex                  */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=      */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /* CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntrySet: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.2. Make counting update (change first and last fields in        */
        /* countEntryCnfgPtr compared to 1.1). Call with policerEntryIndex   */
        /* [maxNumOfPolicerEntries-1], countEntryCnfgPtr[ doCounting=GT_TRUE,*/
        /* counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =      */
        /* CPSS_EXMX_POLICER_COUNT_CDU_ONLY_E ]. Expected: GT_OK.            */

        st = cpssExMxPolicerEntryCountingUpdate(dev, policerEntryIndex,
                                                &UpdateCountEntryCnfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.3. Call cpssExMxPolicerEntryGet with policerEntryIndex 
           [maxNumOfPolicerEntries-1], non NULL meterEntryCnfgPtr and
           countEntryCnfgPtr. Expected: GT_OK, for meterEntryCnfgPtr -
           same values as written at 1.1; for countEntryCnfgPtr - same
           values as written at 1.2.    */

        /* erase values before read to make them initially different from set values */
        cpssOsMemSet((GT_VOID*)&retMeterEntryCnfg, 0xFF,
                     sizeof(CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC));
        cpssOsMemSet((GT_VOID*)&retCountEntryCnfg, 0xFF,
                     sizeof(CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC));

        st = cpssExMxPolicerEntryGet(dev, policerEntryIndex, &retMeterEntryCnfg,
                                     &retCountEntryCnfg);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryGet: %d, %d",
                                     dev, policerEntryIndex);
        if (GT_OK == st)
        {
            failureWas = GT_FALSE;

            if ((meterEntryCnfg.doMetering != retMeterEntryCnfg.doMetering) ||
                (meterEntryCnfg.remarkBasedOnL3Cos != retMeterEntryCnfg.remarkBasedOnL3Cos) ||
                (meterEntryCnfg.remarkBasedOnL2Cos != retMeterEntryCnfg.remarkBasedOnL2Cos) ||
                (meterEntryCnfg.dropRed != retMeterEntryCnfg.dropRed) ||
                (meterEntryCnfg.meterColorMode != retMeterEntryCnfg.meterColorMode) ||
                (meterEntryCnfg.meterMode != retMeterEntryCnfg.meterMode) ||
                (meterEntryCnfg.mngCounterSet != retMeterEntryCnfg.mngCounterSet))
            {
                failureWas = GT_TRUE;
            }

            if ((CPSS_PP_FAMILY_TIGER_E == devFamily) || (CPSS_PP_FAMILY_SAMBA_E == devFamily))
            {
                if ((meterEntryCnfg.modifyTcEnable != retMeterEntryCnfg.modifyTcEnable) ||
                    (meterEntryCnfg.modifyExternalCosEnable != retMeterEntryCnfg.modifyExternalCosEnable))
                {
                    failureWas = GT_TRUE;
                }
            }
            else
            {/* non - Tiger/Samba device */
                if ((GT_FALSE == meterEntryCnfg.remarkBasedOnL3Cos) &&
                    (meterEntryCnfg.remarkDp != retMeterEntryCnfg.remarkDp))
                {
                    failureWas = GT_TRUE;
                }
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another meterEntryCnfg than has been written [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);

            failureWas = GT_FALSE;

            if (UpdateCountEntryCnfg.doCounting != retCountEntryCnfg.doCounting)
            {
                failureWas = GT_TRUE;
            }

            if ((GT_TRUE == UpdateCountEntryCnfg.doCounting) &&
                (UpdateCountEntryCnfg.counterDuMode != retCountEntryCnfg.counterDuMode))
            {
                failureWas = GT_TRUE;
            }

            if (((CPSS_PP_FAMILY_TIGER_E == devFamily) ||
                 (CPSS_PP_FAMILY_SAMBA_E == devFamily)) &&
                (UpdateCountEntryCnfg.counterMode != retCountEntryCnfg.counterMode))
            {
                failureWas = GT_TRUE;
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another countEntryCnfg than has been written [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);
        }

        /* 1.4. Check out-of-range policerEntryIndex. Call with
           policerEntryIndex [maxNumOfPolicerEntries], other parameters
           same as in 1.2. Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;

        st = cpssExMxPolicerEntryCountingUpdate(dev, policerEntryIndex,
                                                &UpdateCountEntryCnfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* Restore valid value */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        /* 1.5. Check for NULL pointers. Call with countEntryCnfgPtr [NULL],*/
        /* other parameters same as in 1.2. Expected: GT_BAD_PTR.           */

        st = cpssExMxPolicerEntryCountingUpdate(dev, policerEntryIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, 
                                     "%d, countEntryCnfgPtr = NULL", dev);

        /* 1.6. Call function with
        countEntryCnfgPtr -> counterDuMode [0x5AAAAAA5],
        and other parameters the same as in 1.2. Expected: NON GT_OK
        for doCounting == GT_TRUE and GT_OK otherwise (if doCounting == GT_FALSE,
        counterDuMode is non-valid). */
        UpdateCountEntryCnfg.counterDuMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntryCountingUpdate(dev, policerEntryIndex,
                                                &UpdateCountEntryCnfg);
        if (GT_TRUE == UpdateCountEntryCnfg.doCounting)
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                "%d, %d, countEntryCnfgPtr [doCounting = %d, counterDuMode = %d]",
                dev, policerEntryIndex, UpdateCountEntryCnfg.doCounting, UpdateCountEntryCnfg.counterDuMode);
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                "%d, %d, countEntryCnfgPtr [doCounting = %d, counterDuMode = %d]",
                dev, policerEntryIndex, UpdateCountEntryCnfg.doCounting, UpdateCountEntryCnfg.counterDuMode);
        }

        UpdateCountEntryCnfg.counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E;

        /* 1.7. Call function with
        countEntryCnfgPtr -> counterMode [0x5AAAAA5],
        and other parameters the same as in 1.5.
        Expected: NON GT_OK for Samba/Rumba and Tiger and
        GT_OK for others (this parameter is not used by other devices). */
        UpdateCountEntryCnfg.counterMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntryCountingUpdate(dev, policerEntryIndex,
                                                &UpdateCountEntryCnfg);
        if ((devFamily == CPSS_PP_FAMILY_TIGER_E) ||
            (devFamily == CPSS_PP_FAMILY_SAMBA_E))
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "Tiger or Samba/Rumba device: %d, %d, countEntryCnfgPtr->counterDuMode = %d",
                dev, policerEntryIndex, UpdateCountEntryCnfg.counterDuMode);
        }
        else
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
        }

        UpdateCountEntryCnfg.counterMode = CPSS_EXMX_POLICER_COUNT_CDU_ONLY_E; /* restore */

        /* 1.8. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex  */
        /* [maxNumOfPolicerEntries-1] to cleanup after testing.             */
        /* Expected: GT_OK.                                                 */
        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);
    }

    /* set correct params */
    policerEntryIndex = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryCountingUpdate(dev, policerEntryIndex,
                                                &UpdateCountEntryCnfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryCountingUpdate(dev, policerEntryIndex,
                                            &UpdateCountEntryCnfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryMeteringUpdate
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  policerEntryIndex,
    IN  CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   *meterEntryCnfgPtr,
    OUT CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   *lbParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryMeteringUpdate)
{
/*
ITERATE_DEVICES 
1.1. Write policer entry for testing. Call cpssExMxPolicerEntrySet with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Make metering update (change first and last fields in meterEntryCnfgPtr compared to 1.1). Call with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_POLICER_COLOR_AWARE_E, meterMode= CPSS_EXMX_POLICER_METER_TR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [trTcmParams {cir=10000, cbs=65535, pir = 65000, pbs=128000}] ], non NULL lbParamsPtr. Expected: GT_OK.
1.3. Call cpssExMxPolicerEntryGet with policerEntryIndex [maxNumOfPolicerEntries-1], non NULL meterEntryCnfgPtr and countEntryCnfgPtr. Expected: GT_OK, for meterEntryCnfgPtr - same values as written at 1.2; for countEntryCnfgPtr - same values as written at 1.1 - check by fields.
1.4. Check out-of-range policerEntryIndex. Call with policerEntryIndex [maxNumOfPolicerEntries], other parameters same as in 1.2. Expected: non GT_OK.
1.5. Check for NULL pointers. Call with meterEntryCnfgPtr [NULL], other parameters same as in 1.2. Expected: GT_BAD_PTR.
1.6. Check for NULL pointers. Call with lbParamsPtr [NULL], other parameters same as in 1.2. Expected: GT_BAD_PTR.

meterEntryCnfg fields check:
1.7. Call function with meterEntryCnfgPtr -> modifyTcEnable [GT_TRUE], and other parameters the same as in 1.2. Expected: GT_OK for Samba/Rumba and Tiger and NON GT_OK for others (not supported).
1.8. Call function with meterEntryCnfgPtr -> modifyExternalCosEnable [GT_TRUE], and other parameters the same as in 1.5. Expected: GT_OK for Samba/Rumba and Tiger and NON GT_OK for others (not supported).
1.9. Call function with meterEntryCnfgPtr -> remarkDp [GT_TRUE], and other parameters the same as in 1.5. Expected: GT_OK (it's ignored by Samba/Rumba and Tiger, but not unsupported).
1.10. Call function with meterEntryCnfgPtr -> meterColorMode [0x5AAAAAA5], and other parameters the same as in 1.5. Expected: GT_BAD_PARAM.
1.11. Call function with meterEntryCnfgPtr -> meterMode [0x5AAAAAA5], and other parameters the same as in 1.5. Expected: GT_BAD_PARAM.
1.12. Call function with meterEntryCnfgPtr -> mngCounterSet [0x5AAAAAA5], and other parameters the same as in 1.5. Expected: GT_BAD_PARAM.

1.13. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS                               st = GT_OK;
    GT_U8                                   dev;

    GT_U32                                  policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   UpdateMeterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    GT_U32                                  maxNumOfPolicerEntries = 0;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   retMeterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   retCountEntryCnfg;

    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;
    GT_BOOL                                 failureWas;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);

    policerMeterEntryCnfgDefaultSet(&UpdateMeterEntryCnfg);
    /* changes from defaults */
    UpdateMeterEntryCnfg.meterColorMode = CPSS_POLICER_COLOR_AWARE_E;
    UpdateMeterEntryCnfg.meterMode = CPSS_EXMX_POLICER_METER_TR_TCM_E;
    UpdateMeterEntryCnfg.lbParams.trTcmParams.cbs = 65535;
    UpdateMeterEntryCnfg.lbParams.trTcmParams.cir = 10000;
    UpdateMeterEntryCnfg.lbParams.trTcmParams.pbs = 128000;
    UpdateMeterEntryCnfg.lbParams.trTcmParams.pir = 65000;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev,
                                             &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;

            st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                    &UpdateMeterEntryCnfg, &lbParams);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);

            continue;
        }

        /* 1.1. Write policer entry for testing. Call                       */
        /* cpssExMxPolicerEntrySet  with policerEntryIndex                  */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=      */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /* CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntrySet: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.2. Make metering update (change first and last fields in
           meterEntryCnfgPtr compared to 1.1). Call with policerEntryIndex
           [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_FALSE,
           remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=
           GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,
           meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=
           CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE,
           mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams
           [srTcmParams {cir=10000, cbs=65535, ebs=128000}] ], non NULL
           lbParamsPtr. Expected: GT_OK.    */
        st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.3. Call cpssExMxPolicerEntryGet with policerEntryIndex
           [maxNumOfPolicerEntries-1], non NULL meterEntryCnfgPtr and
           countEntryCnfgPtr. Expected: GT_OK, for meterEntryCnfgPtr -
           same values as written at 1.2; for countEntryCnfgPtr - same
           values as written at 1.1.    */

        /* erase values before read to make them initially different from set values */
        cpssOsMemSet((GT_VOID*)&retMeterEntryCnfg, 0xFF,
                     sizeof(CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC));
        cpssOsMemSet((GT_VOID*)&retCountEntryCnfg, 0xFF,
                     sizeof(CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC));

        st = cpssExMxPolicerEntryGet(dev, policerEntryIndex, &retMeterEntryCnfg,
                                     &retCountEntryCnfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryGet: %d, %d",
                                     dev, policerEntryIndex);
        if (GT_OK == st)
        {
            failureWas = GT_FALSE;

            if ((UpdateMeterEntryCnfg.doMetering != retMeterEntryCnfg.doMetering) ||
                (UpdateMeterEntryCnfg.remarkBasedOnL3Cos != retMeterEntryCnfg.remarkBasedOnL3Cos) ||
                (UpdateMeterEntryCnfg.remarkBasedOnL2Cos != retMeterEntryCnfg.remarkBasedOnL2Cos) ||
                (UpdateMeterEntryCnfg.dropRed != retMeterEntryCnfg.dropRed) ||
                (UpdateMeterEntryCnfg.meterColorMode != retMeterEntryCnfg.meterColorMode) ||
                (UpdateMeterEntryCnfg.meterMode != retMeterEntryCnfg.meterMode) ||
                (UpdateMeterEntryCnfg.mngCounterSet != retMeterEntryCnfg.mngCounterSet))
            {
                failureWas = GT_TRUE;
            }

            if ((CPSS_PP_FAMILY_TIGER_E == devFamily) || (CPSS_PP_FAMILY_SAMBA_E == devFamily))
            {
                if ((UpdateMeterEntryCnfg.modifyTcEnable != retMeterEntryCnfg.modifyTcEnable) ||
                    (UpdateMeterEntryCnfg.modifyExternalCosEnable != retMeterEntryCnfg.modifyExternalCosEnable))
                {
                    failureWas = GT_TRUE;
                }
            }
            else
            {/* non - Tiger/Samba device */
                if ((GT_FALSE == UpdateMeterEntryCnfg.remarkBasedOnL3Cos) &&
                    (UpdateMeterEntryCnfg.remarkDp != retMeterEntryCnfg.remarkDp))
                {
                    failureWas = GT_TRUE;
                }
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another meterEntryCnfg than has been written [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);

            failureWas = GT_FALSE;

            if (countEntryCnfg.doCounting != retCountEntryCnfg.doCounting)
            {
                failureWas = GT_TRUE;
            }

            if ((GT_TRUE == countEntryCnfg.doCounting) &&
                (countEntryCnfg.counterDuMode != retCountEntryCnfg.counterDuMode))
            {
                failureWas = GT_TRUE;
            }

            if (((CPSS_PP_FAMILY_TIGER_E == devFamily) ||
                 (CPSS_PP_FAMILY_SAMBA_E == devFamily)) &&
                (countEntryCnfg.counterMode != retCountEntryCnfg.counterMode))
            {
                failureWas = GT_TRUE;
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another countEntryCnfg than has been written [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);
        }

        /* 1.4. Check out-of-range policerEntryIndex. Call with
           policerEntryIndex [maxNumOfPolicerEntries], other parameters
           same as in 1.2. Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;

        st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* Restore valid value */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        /* 1.5. Check for NULL pointers. Call with meterEntryCnfgPtr [NULL],*/
        /* other parameters same as in 1.2. Expected: GT_BAD_PTR.           */
        st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                NULL, &lbParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, 
                                     "%d, meterEntryCnfgPtr = NULL", dev);

        /* 1.6. Check for NULL pointers. Call with lbParams [NULL],*/
        /* other parameters same as in 1.2. Expected: GT_BAD_PTR.  */
        st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, 
                                     "%d, lbParams = NULL", dev);

        /* 1.7. Call function with meterEntryCnfgPtr -> modifyTcEnable [GT_TRUE],
        and other parameters the same as in 1.2.
        Expected: GT_OK for Samba/Rumba and Tiger and
        NON GT_OK for others (not supported). */
        UpdateMeterEntryCnfg.modifyTcEnable = GT_TRUE;

        st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        if ((CPSS_PP_FAMILY_TIGER_E == devFamily) ||
            (CPSS_PP_FAMILY_SAMBA_E == devFamily))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "non Tiger or Samba/Rumba device: %d, %d, meterEntryCnfgPtr->modifyTcEnable = %d",
                dev, policerEntryIndex, UpdateMeterEntryCnfg.modifyTcEnable);
        }

        UpdateMeterEntryCnfg.modifyTcEnable = GT_FALSE; /* restore */

        /* 1.8. Call function with
        meterEntryCnfgPtr -> modifyExternalCosEnable [GT_TRUE],
        and other parameters the same as in 1.2.
        Expected: GT_OK for Samba/Rumba and Tiger and
        NON GT_OK for others (not supported). */
        UpdateMeterEntryCnfg.modifyExternalCosEnable = GT_TRUE;

        st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        if ((CPSS_PP_FAMILY_TIGER_E == devFamily) ||
            (CPSS_PP_FAMILY_SAMBA_E == devFamily))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "non Tiger or Samba/Rumba device: %d, %d, meterEntryCnfgPtr->modifyExternalCosEnable = %d",
                dev, policerEntryIndex, UpdateMeterEntryCnfg.modifyExternalCosEnable);
        }

        UpdateMeterEntryCnfg.modifyExternalCosEnable = GT_FALSE; /* restore */

        /* 1.9. Call function with meterEntryCnfgPtr -> remarkDp [GT_TRUE],
        and other parameters the same as in 1.2.
        Expected: GT_OK (it's ignored by Samba/Rumba and Tiger,
        but not unsupported). */
        UpdateMeterEntryCnfg.remarkDp = GT_TRUE;

        st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, %d, meterEntryCnfgPtr->remarkDp = %d",
            dev, policerEntryIndex, UpdateMeterEntryCnfg.remarkDp);

        UpdateMeterEntryCnfg.remarkDp = GT_FALSE; /* restore */

        /* 1.10. Call function with
        meterEntryCnfgPtr -> meterColorMode [0x5AAAAAA5],
        and other parameters the same as in 1.2. Expected: GT_BAD_PARAM. */
        UpdateMeterEntryCnfg.meterColorMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
            "%d, %d, meterEntryCnfgPtr->meterColorMode = %d",
            dev, policerEntryIndex, UpdateMeterEntryCnfg.meterColorMode);

        UpdateMeterEntryCnfg.meterColorMode = CPSS_POLICER_COLOR_AWARE_E; /* restore */

        /* 1.11. Call function with meterEntryCnfgPtr -> meterMode [0x5AAAAAA5],
        and other parameters the same as in 1.2. Expected: GT_BAD_PARAM. */
        UpdateMeterEntryCnfg.meterMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
            "%d, %d, meterEntryCnfgPtr->meterMode = %d",
            dev, policerEntryIndex, UpdateMeterEntryCnfg.meterMode);

        UpdateMeterEntryCnfg.meterMode = CPSS_EXMX_POLICER_METER_TR_TCM_E; /* restore */

        /* 1.12. Call function with meterEntryCnfgPtr -> mngCounterSet [0x5AAAAAA5],
        and other parameters the same as in 1.5. Expected: GT_BAD_PARAM. */
        UpdateMeterEntryCnfg.mngCounterSet = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
            "%d, %d, meterEntryCnfgPtr->mngCounterSet = %d",
            dev, policerEntryIndex, UpdateMeterEntryCnfg.mngCounterSet);

        UpdateMeterEntryCnfg.mngCounterSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E; /* restore */

        /* 1.13. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex  */
        /* [maxNumOfPolicerEntries-1] to cleanup after testing.             */
        /* Expected: GT_OK.                                                 */
        st = cpssExMxPolicerEntryInvalidate(dev,policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);
    }

    /* set correct params */
    policerEntryIndex = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryMeteringUpdate(dev, policerEntryIndex,
                                            &UpdateMeterEntryCnfg, &lbParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryMeteringUpdateWithBypassWa
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  policerEntryIndex,
    IN  CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   *meterEntryCnfgPtr,
    OUT CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   *lbParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryMeteringUpdateWithBypassWa)
{
/*
ITERATE_DEVICES 
1.1. Write policer entry for testing. Call cpssExMxPolicerEntrySet with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Make metering update (change first and last fields in meterEntryCnfgPtr compared to 1.1). Call with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_POLICER_COLOR_AWARE_E, meterMode= CPSS_EXMX_POLICER_METER_TR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [trTcmParams {cir=10000, cbs=65535, pir = 65000, pbs=128000}] ], non NULL lbParamsPtr. Expected: GT_OK.
1.3. Call cpssExMxPolicerEntryGet with policerEntryIndex [maxNumOfPolicerEntries-1], non NULL meterEntryCnfgPtr and countEntryCnfgPtr. Expected: GT_OK, for meterEntryCnfgPtr - same values as written at 1.2; for countEntryCnfgPtr - same values as written at 1.1 - check by fields.
1.4. Check out-of-range policerEntryIndex. Call with policerEntryIndex [maxNumOfPolicerEntries], other parameters same as in 1.2. Expected: non GT_OK.
1.5. Check for NULL pointers. Call with meterEntryCnfgPtr [NULL], other parameters same as in 1.2. Expected: GT_BAD_PTR.
1.6. Check for NULL pointers. Call with lbParamsPtr [NULL], other parameters same as in 1.2. Expected: GT_BAD_PTR.

meterEntryCnfg fields check:
1.7. Call function with meterEntryCnfgPtr -> modifyTcEnable [GT_TRUE], and other parameters the same as in 1.2. Expected: GT_OK for Samba/Rumba and Tiger and NON GT_OK for others (not supported).
1.8. Call function with meterEntryCnfgPtr -> modifyExternalCosEnable [GT_TRUE], and other parameters the same as in 1.5. Expected: GT_OK for Samba/Rumba and Tiger and NON GT_OK for others (not supported).
1.9. Call function with meterEntryCnfgPtr -> remarkDp [GT_TRUE], and other parameters the same as in 1.5. Expected: GT_OK (it's ignored by Samba/Rumba and Tiger, but not unsupported).
1.10. Call function with meterEntryCnfgPtr -> meterColorMode [0x5AAAAAA5], and other parameters the same as in 1.5. Expected: GT_BAD_PARAM.
1.11. Call function with meterEntryCnfgPtr -> meterMode [0x5AAAAAA5], and other parameters the same as in 1.5. Expected: GT_BAD_PARAM.
1.12. Call function with meterEntryCnfgPtr -> mngCounterSet [0x5AAAAAA5], and other parameters the same as in 1.5. Expected: GT_BAD_PARAM.

1.13. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS                               st = GT_OK;
    GT_U8                                   dev;

    GT_U32                                  policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   UpdateMeterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    GT_U32                                  maxNumOfPolicerEntries = 0;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   retMeterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   retCountEntryCnfg;

    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;
    GT_BOOL                                 failureWas;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);

    policerMeterEntryCnfgDefaultSet(&UpdateMeterEntryCnfg);
    /* changes from defaults */
    UpdateMeterEntryCnfg.meterColorMode = CPSS_POLICER_COLOR_AWARE_E;
    UpdateMeterEntryCnfg.meterMode = CPSS_EXMX_POLICER_METER_TR_TCM_E;
    UpdateMeterEntryCnfg.lbParams.trTcmParams.cbs = 65535;
    UpdateMeterEntryCnfg.lbParams.trTcmParams.cir = 10000;
    UpdateMeterEntryCnfg.lbParams.trTcmParams.pbs = 128000;
    UpdateMeterEntryCnfg.lbParams.trTcmParams.pir = 65000;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev,
                                             &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;

            st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                    &UpdateMeterEntryCnfg, &lbParams);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);

            continue;
        }

        /* 1.1. Write policer entry for testing. Call                       */
        /* cpssExMxPolicerEntrySet  with policerEntryIndex                  */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=      */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /* CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntrySet: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.2. Make metering update (change first and last fields in
           meterEntryCnfgPtr compared to 1.1). Call with policerEntryIndex
           [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_FALSE,
           remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=
           GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,
           meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=
           CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE,
           mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams
           [srTcmParams {cir=10000, cbs=65535, ebs=128000}] ], non NULL
           lbParamsPtr. Expected: GT_OK.    */
        st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.3. Call cpssExMxPolicerEntryGet with policerEntryIndex
           [maxNumOfPolicerEntries-1], non NULL meterEntryCnfgPtr and
           countEntryCnfgPtr. Expected: GT_OK, for meterEntryCnfgPtr -
           same values as written at 1.2; for countEntryCnfgPtr - same
           values as written at 1.1.    */

        /* erase values before read to make them initially different from set values */
        cpssOsMemSet((GT_VOID*)&retMeterEntryCnfg, 0xFF,
                     sizeof(CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC));
        cpssOsMemSet((GT_VOID*)&retCountEntryCnfg, 0xFF,
                     sizeof(CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC));

        st = cpssExMxPolicerEntryGet(dev, policerEntryIndex, &retMeterEntryCnfg,
                                     &retCountEntryCnfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryGet: %d, %d",
                                     dev, policerEntryIndex);
        if (GT_OK == st)
        {
            failureWas = GT_FALSE;

            if ((UpdateMeterEntryCnfg.doMetering != retMeterEntryCnfg.doMetering) ||
                (UpdateMeterEntryCnfg.remarkBasedOnL3Cos != retMeterEntryCnfg.remarkBasedOnL3Cos) ||
                (UpdateMeterEntryCnfg.remarkBasedOnL2Cos != retMeterEntryCnfg.remarkBasedOnL2Cos) ||
                (UpdateMeterEntryCnfg.dropRed != retMeterEntryCnfg.dropRed) ||
                (UpdateMeterEntryCnfg.meterColorMode != retMeterEntryCnfg.meterColorMode) ||
                (UpdateMeterEntryCnfg.meterMode != retMeterEntryCnfg.meterMode) ||
                (UpdateMeterEntryCnfg.mngCounterSet != retMeterEntryCnfg.mngCounterSet))
            {
                failureWas = GT_TRUE;
            }

            if ((CPSS_PP_FAMILY_TIGER_E == devFamily) || (CPSS_PP_FAMILY_SAMBA_E == devFamily))
            {
                if ((UpdateMeterEntryCnfg.modifyTcEnable != retMeterEntryCnfg.modifyTcEnable) ||
                    (UpdateMeterEntryCnfg.modifyExternalCosEnable != retMeterEntryCnfg.modifyExternalCosEnable))
                {
                    failureWas = GT_TRUE;
                }
            }
            else
            {/* non - Tiger/Samba device */
                if ((GT_FALSE == UpdateMeterEntryCnfg.remarkBasedOnL3Cos) &&
                    (UpdateMeterEntryCnfg.remarkDp != retMeterEntryCnfg.remarkDp))
                {
                    failureWas = GT_TRUE;
                }
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another meterEntryCnfg than has been written [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);

            failureWas = GT_FALSE;

            if (countEntryCnfg.doCounting != retCountEntryCnfg.doCounting)
            {
                failureWas = GT_TRUE;
            }

            if ((GT_TRUE == countEntryCnfg.doCounting) &&
                (countEntryCnfg.counterDuMode != retCountEntryCnfg.counterDuMode))
            {
                failureWas = GT_TRUE;
            }

            if (((CPSS_PP_FAMILY_TIGER_E == devFamily) ||
                 (CPSS_PP_FAMILY_SAMBA_E == devFamily)) &&
                (countEntryCnfg.counterMode != retCountEntryCnfg.counterMode))
            {
                failureWas = GT_TRUE;
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another countEntryCnfg than has been written [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);
        }

        /* 1.4. Check out-of-range policerEntryIndex. Call with
           policerEntryIndex [maxNumOfPolicerEntries], other parameters
           same as in 1.2. Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;

        st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* Restore valid value */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        /* 1.5. Check for NULL pointers. Call with meterEntryCnfgPtr [NULL],*/
        /* other parameters same as in 1.2. Expected: GT_BAD_PTR.           */
        st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                NULL, &lbParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, 
                                     "%d, meterEntryCnfgPtr = NULL", dev);

        /* 1.6. Check for NULL pointers. Call with lbParams [NULL],*/
        /* other parameters same as in 1.2. Expected: GT_BAD_PTR.  */
        st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, 
                                     "%d, lbParams = NULL", dev);

        /* 1.7. Call function with meterEntryCnfgPtr -> modifyTcEnable [GT_TRUE],
        and other parameters the same as in 1.2.
        Expected: GT_OK for Samba/Rumba and Tiger and
        NON GT_OK for others (not supported). */
        UpdateMeterEntryCnfg.modifyTcEnable = GT_TRUE;

        st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        if ((CPSS_PP_FAMILY_TIGER_E == devFamily) ||
            (CPSS_PP_FAMILY_SAMBA_E == devFamily))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "non Tiger or Samba/Rumba device: %d, %d, meterEntryCnfgPtr->modifyTcEnable = %d",
                dev, policerEntryIndex, UpdateMeterEntryCnfg.modifyTcEnable);
        }

        UpdateMeterEntryCnfg.modifyTcEnable = GT_FALSE; /* restore */

        /* 1.8. Call function with
        meterEntryCnfgPtr -> modifyExternalCosEnable [GT_TRUE],
        and other parameters the same as in 1.2.
        Expected: GT_OK for Samba/Rumba and Tiger and
        NON GT_OK for others (not supported). */
        UpdateMeterEntryCnfg.modifyExternalCosEnable = GT_TRUE;

        st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        if ((CPSS_PP_FAMILY_TIGER_E == devFamily) ||
            (CPSS_PP_FAMILY_SAMBA_E == devFamily))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "non Tiger or Samba/Rumba device: %d, %d, meterEntryCnfgPtr->modifyExternalCosEnable = %d",
                dev, policerEntryIndex, UpdateMeterEntryCnfg.modifyExternalCosEnable);
        }

        UpdateMeterEntryCnfg.modifyExternalCosEnable = GT_FALSE; /* restore */

        /* 1.9. Call function with meterEntryCnfgPtr -> remarkDp [GT_TRUE],
        and other parameters the same as in 1.2.
        Expected: GT_OK (it's ignored by Samba/Rumba and Tiger,
        but not unsupported). */
        UpdateMeterEntryCnfg.remarkDp = GT_TRUE;

        st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, %d, meterEntryCnfgPtr->remarkDp = %d",
            dev, policerEntryIndex, UpdateMeterEntryCnfg.remarkDp);

        UpdateMeterEntryCnfg.remarkDp = GT_FALSE; /* restore */

        /* 1.10. Call function with
        meterEntryCnfgPtr -> meterColorMode [0x5AAAAAA5],
        and other parameters the same as in 1.2. Expected: GT_BAD_PARAM. */
        UpdateMeterEntryCnfg.meterColorMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
            "%d, %d, meterEntryCnfgPtr->meterColorMode = %d",
            dev, policerEntryIndex, UpdateMeterEntryCnfg.meterColorMode);

        UpdateMeterEntryCnfg.meterColorMode = CPSS_POLICER_COLOR_AWARE_E; /* restore */

        /* 1.11. Call function with meterEntryCnfgPtr -> meterMode [0x5AAAAAA5],
        and other parameters the same as in 1.2. Expected: GT_BAD_PARAM. */
        UpdateMeterEntryCnfg.meterMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
            "%d, %d, meterEntryCnfgPtr->meterMode = %d",
            dev, policerEntryIndex, UpdateMeterEntryCnfg.meterMode);

        UpdateMeterEntryCnfg.meterMode = CPSS_EXMX_POLICER_METER_TR_TCM_E; /* restore */

        /* 1.12. Call function with meterEntryCnfgPtr -> mngCounterSet [0x5AAAAAA5],
        and other parameters the same as in 1.5. Expected: GT_BAD_PARAM. */
        UpdateMeterEntryCnfg.mngCounterSet = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
            "%d, %d, meterEntryCnfgPtr->mngCounterSet = %d",
            dev, policerEntryIndex, UpdateMeterEntryCnfg.mngCounterSet);

        UpdateMeterEntryCnfg.mngCounterSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E; /* restore */

        /* 1.13. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex  */
        /* [maxNumOfPolicerEntries-1] to cleanup after testing.             */
        /* Expected: GT_OK.                                                 */
        st = cpssExMxPolicerEntryInvalidate(dev,policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);
    }

    /* set correct params */
    policerEntryIndex = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                                &UpdateMeterEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(dev, policerEntryIndex,
                                            &UpdateMeterEntryCnfg, &lbParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryCountingEnable
(
    IN GT_U8    devNum,
    IN GT_U32   policerEntryIndex,
    IN GT_BOOL  enableCounting
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryCountingEnable)
{
/*
ITERATE_DEVICES 
1.1. Write policer entry for testing. Call cpssExMxPolicerEntrySet with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Call with policerEntryIndex [maxNumOfPolicerEntries-1], enableCounting [GT_FALSE / GT_TRUE]. Expected: GT_OK.
1.3. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
1.4. Check with empty policer entry. Call with policerEntryIndex [0], enableCounting [GT_FALSE / GT_TRUE]. Expected: GT_OK.
1.5. Check out-of-range policerEntryIndex. Call with policerEntryIndex [maxNumOfPolicerEntries], enableCounting [GT_TRUE]. Expected: non GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable;
    GT_U32      policerEntryIndex;

    GT_U32                                  maxNumOfPolicerEntries = 0;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;
            enable = GT_FALSE;

            st = cpssExMxPolicerEntryCountingEnable(dev, policerEntryIndex, enable);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);

            continue;
        }

        /* 1.1. Write policer entry for testing. Call                       */
        /* cpssExMxPolicerEntrySet  with policerEntryIndex                  */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=      */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /* CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntrySet: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.2. Call with policerEntryIndex [maxNumOfPolicerEntries-1],
        enableCounting [GT_FALSE / GT_TRUE]. Expected: GT_OK. */
        enable = GT_FALSE;

        st = cpssExMxPolicerEntryCountingEnable(dev, policerEntryIndex, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerEntryIndex, enable);

        enable = GT_TRUE;

        st = cpssExMxPolicerEntryCountingEnable(dev, policerEntryIndex, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerEntryIndex, enable);

        /* 1.3. Call cpssExMxPolicerEntryInvalidate with
        policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing.
        Expected: GT_OK. */
        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.4. Check with empty policer entry. Call with policerEntryIndex[0],
        enableCounting [GT_FALSE / GT_TRUE]. Expected: GT_OK. */
        policerEntryIndex = 0;

        enable = GT_FALSE;

        st = cpssExMxPolicerEntryCountingEnable(dev, policerEntryIndex, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerEntryIndex, enable);

        enable = GT_TRUE;

        st = cpssExMxPolicerEntryCountingEnable(dev, policerEntryIndex, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerEntryIndex, enable);

        /* 1.5. Check out-of-range policerEntryIndex.
        Call with policerEntryIndex [maxNumOfPolicerEntries],
        enableCounting [GT_TRUE]. Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;
        enable = GT_TRUE;

        st = cpssExMxPolicerEntryCountingEnable(dev, policerEntryIndex, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
    }

    policerEntryIndex = 0;
    enable = GT_TRUE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryCountingEnable(dev, policerEntryIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryCountingEnable(dev, policerEntryIndex, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryMeteringEnable
(
    IN GT_U8    devNum,
    IN GT_U32   policerEntryIndex,
    IN GT_BOOL  enableMetering
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryMeteringEnable)
{
/*
ITERATE_DEVICES 
1.1. Write policer entry for testing. Call cpssExMxPolicerEntrySet with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Call with policerEntryIndex [maxNumOfPolicerEntries-1], enableMetering [GT_FALSE / GT_TRUE]. Expected: GT_OK.
1.3. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
1.4. Check with empty policer entry. Call with policerEntryIndex [0], enableMetering [GT_TRUE]. Expected: NON GT_OK (accordingly to function's contract this function may be applied only for already configured metering Policer Entry).
1.5. Check out-of-range policerEntryIndex. Call with policerEntryIndex [maxNumOfPolicerEntries], enableMetering [GT_TRUE]. Expected: non GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enable;
    GT_U32      policerEntryIndex;

    GT_U32                                  maxNumOfPolicerEntries = 0;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;
            enable = GT_FALSE;

            st = cpssExMxPolicerEntryMeteringEnable(dev, policerEntryIndex, enable);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);

            continue;
        }

        /* 1.1. Write policer entry for testing. Call                       */
        /* cpssExMxPolicerEntrySet  with policerEntryIndex                  */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=      */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /* CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntrySet: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.2. Call with policerEntryIndex [maxNumOfPolicerEntries-1],
        enableMetering [GT_FALSE / GT_TRUE]. Expected: GT_OK. */
        enable = GT_FALSE;

        st = cpssExMxPolicerEntryMeteringEnable(dev, policerEntryIndex, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerEntryIndex, enable);

        enable = GT_TRUE;

        st = cpssExMxPolicerEntryMeteringEnable(dev, policerEntryIndex, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerEntryIndex, enable);

        /* 1.3. Call cpssExMxPolicerEntryInvalidate with
        policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing.
        Expected: GT_OK. */
        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.4. Check with empty policer entry. Call with policerEntryIndex [0],
        enableMetering [GT_TRUE]. Expected: NON GT_OK
        (accordingly to function's contract this function may be applied only
        for already configured metering Policer Entry). */
        policerEntryIndex = 0;
        enable = GT_TRUE;

        st = cpssExMxPolicerEntryMeteringEnable(dev, policerEntryIndex, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, policerEntryIndex, enable);

        /* 1.5. Check out-of-range policerEntryIndex.
        Call with policerEntryIndex [maxNumOfPolicerEntries],
        enableMetering [GT_TRUE]. Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;
        enable = GT_TRUE;

        st = cpssExMxPolicerEntryMeteringEnable(dev, policerEntryIndex, enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
    }

    policerEntryIndex = 0;
    enable = GT_TRUE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryMeteringEnable(dev, policerEntryIndex, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryMeteringEnable(dev, policerEntryIndex, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  policerEntryIndex,
    OUT CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   *meterEntryCnfgPtr,
    OUT CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   *countEntryCnfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryGet)
{
/*
ITERATE_DEVICES 
1.1. Write policer entry for testing. Call cpssExMxPolicerEntrySet with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Call with policerEntryIndex [maxNumOfPolicerEntries-1], non NULL meterEntryCnfgPtr and countEntryCnfgPtr. Expected: GT_OK.
1.3. Check with empty policer entry. Call with policerEntryIndex [0], non NULL meterEntryCnfgPtr and countEntryCnfgPtr. Expected: GT_OK.
1.4. Check out-of-range policerEntryIndex. Call with policerEntryIndex [maxNumOfPolicerEntries], non NULL meterEntryCnfgPtr and countEntryCnfgPtr. Expected: non GT_OK.
1.5. Call with meterEntryCnfgPtr [NULL], other parameters same as in 1.2. Expected: GT_OK.
1.6. Call with countEntryCnfgPtr [NULL], other parameters same as in 1.2. Expected: GT_OK.
1.7. Check for NULL pointers. Call with countEntryCnfgPtr [NULL] and meterEntryCnfgPtr [NULL], other parameters same as in 1.2. Expected: NON GT_OK.
1.8. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                                   dev;
    GT_U32                                  policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   retCountEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   retMeterEntryCnfg;

    GT_U32                                  maxNumOfPolicerEntries = 0;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;

            st = cpssExMxPolicerEntryGet(dev, policerEntryIndex,
                                         &retMeterEntryCnfg, &retCountEntryCnfg);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);

            continue;
        }

        /* 1.1. Write policer entry for testing. Call                       */
        /* cpssExMxPolicerEntrySet  with policerEntryIndex                  */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=      */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /* CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntrySet: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.2. Call with policerEntryIndex [maxNumOfPolicerEntries-1],
        non NULL meterEntryCnfgPtr and countEntryCnfgPtr. Expected: GT_OK. */
        st = cpssExMxPolicerEntryGet(dev, policerEntryIndex,
                                     &retMeterEntryCnfg, &retCountEntryCnfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.3. Check with empty policer entry. Call with policerEntryIndex[0],
        non NULL meterEntryCnfgPtr and countEntryCnfgPtr. Expected: GT_OK. */
        policerEntryIndex = 0;

        st = cpssExMxPolicerEntryGet(dev, policerEntryIndex,
                                     &retMeterEntryCnfg, &retCountEntryCnfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.4. Check out-of-range policerEntryIndex. Call with
        policerEntryIndex [maxNumOfPolicerEntries], non NULL meterEntryCnfgPtr
        and countEntryCnfgPtr. Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;

        st = cpssExMxPolicerEntryGet(dev, policerEntryIndex,
                                     &retMeterEntryCnfg, &retCountEntryCnfg);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        policerEntryIndex = maxNumOfPolicerEntries - 1; /* restore */

        /* 1.5. Call with meterEntryCnfgPtr [NULL], other parameters
        same as in 1.2. Expected: GT_OK. */
        st = cpssExMxPolicerEntryGet(dev, policerEntryIndex,
                                     NULL, &retCountEntryCnfg);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, meterEntryCnfgPtr = NULL",
                                     dev, policerEntryIndex);

        /* 1.6. Call with countEntryCnfgPtr [NULL], other parameters
        same as in 1.2. Expected: GT_OK. */

        st = cpssExMxPolicerEntryGet(dev, policerEntryIndex,
                                     &retMeterEntryCnfg, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, countEntryCnfgPtr = NULL",
                                     dev, policerEntryIndex);

        /* 1.7. Check for NULL pointers. Call with countEntryCnfgPtr [NULL]
        and meterEntryCnfgPtr [NULL], other parameters same as in 1.2.
        Expected: NON GT_OK. */
        st = cpssExMxPolicerEntryGet(dev, policerEntryIndex, NULL, NULL);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, NULL, NULL",
                                         dev, policerEntryIndex);

        /* 1.8. Call cpssExMxPolicerEntryInvalidate with
        policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing.
        Expected: GT_OK. */
        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);
    }

    policerEntryIndex = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryGet(dev, policerEntryIndex,
                                     &retMeterEntryCnfg, &retCountEntryCnfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryGet(dev, policerEntryIndex,
                                 &retMeterEntryCnfg, &retCountEntryCnfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryInvalidate
(
    IN GT_U8    devNum,
    IN GT_U32   policerEntryIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryInvalidate)
{
/*
ITERATE_DEVICES 
1.1. Call with policerEntryIndex [0 / maxNumOfPolicerEntries-1]. Expected: GT_OK.
1.2. Check out-of-range policerEntryIndex. Call with policerEntryIndex [maxNumOfPolicerEntries]. Expected: non GT_OK.
*/
    GT_STATUS   st = GT_OK;

    GT_U8    dev;
    GT_U32   policerEntryIndex;

    GT_U32   maxNumOfPolicerEntries = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;

            st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);

            continue;
        }

        /* 1.1. Call with policerEntryIndex [0 / maxNumOfPolicerEntries-1].
        Expected: GT_OK. */
        policerEntryIndex = 0;

        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.2. Check out-of-range policerEntryIndex.
        Call with policerEntryIndex [maxNumOfPolicerEntries].
        Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;

        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
    }

    policerEntryIndex = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryMeterParamsCalculate
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMX_POLICER_METER_MODE_ENT        meterMode,
    IN  CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   *lbInParamsPtr,
    OUT CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   *lbOutParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryMeterParamsCalculate)
{
/*
ITERATE_DEVICES 
1.1. Call with meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, lbInParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], non NULL lbOutParamsPtr. Expected: GT_OK.
1.2. Check for NULL pointers. Call with lbInParamsPtr [NULL], non NULL lbParamsPtr. Expected: GT_BAD_PTR.
1.3. Check for NULL pointers. Call with lbOutParamsPtr [NULL], other parameters same as in 1.1. Expected: GT_BAD_PTR.
1.4. Call function with meterMode [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                               st = GT_OK;

    GT_U8                                   dev;
    CPSS_EXMX_POLICER_METER_MODE_ENT        meterMode;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbInParams;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbOutParams;

    meterMode = CPSS_EXMX_POLICER_METER_SR_TCM_E;
    lbInParams.srTcmParams.cir = 10000;
    lbInParams.srTcmParams.cbs = 65535;
    lbInParams.srTcmParams.ebs = 256000;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E,
        lbInParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],
        non NULL lbOutParamsPtr. Expected: GT_OK. */
        st = cpssExMxPolicerEntryMeterParamsCalculate(dev, meterMode,
                                                      &lbInParams,&lbOutParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, meterMode);

        /* 1.2. Check for NULL pointers. Call with lbInParamsPtr [NULL],
        non NULL lbParamsPtr. Expected: GT_BAD_PTR. */
        st = cpssExMxPolicerEntryMeterParamsCalculate(dev, meterMode,
                                                      NULL, &lbOutParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lbInParamsPtr = NULL", dev);

        /* 1.3. Check for NULL pointers. Call with lbOutParamsPtr [NULL],
        other parameters same as in 1.1. Expected: GT_BAD_PTR.  */
        st = cpssExMxPolicerEntryMeterParamsCalculate(dev, meterMode,
                                                      &lbInParams, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, lbOutParamsPtr = NULL", dev);

        /* 1.4. Call function with meterMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM. */
        meterMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntryMeterParamsCalculate(dev, meterMode,
                                                      &lbInParams,&lbOutParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, meterMode);

        meterMode = CPSS_EXMX_POLICER_METER_SR_TCM_E;
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryMeterParamsCalculate(dev, meterMode,
                                                      &lbInParams,&lbOutParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryMeterParamsCalculate(dev, meterMode,
                                                  &lbInParams,&lbOutParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryMeterRefresh
(
    IN  GT_U8   devNum,
    IN GT_U32   policerEntryIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryMeterRefresh)
{
/*
ITERATE_DEVICES 
1.1. Write policer entry for testing. Call cpssExMxPolicerEntrySet with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Check with non-empty policer entry. Call with policerEntryIndex [maxNumOfPolicerEntries-1]. Expected: GT_OK for Tiger and GT_BAD_PARAM for others.
1.3. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
1.4. Check with empty policer entry. For Tiger device call with policerEntryIndex [maxNumOfPolicerEntries-1]. Expected: GT_OK.
1.5. Check out-of-range policerEntryIndex. For Tiger device call with policerEntryIndex [maxNumOfPolicerEntries]. Expected: non GT_OK.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                                   dev;
    GT_U32                                  policerEntryIndex;

    GT_U32                                  maxNumOfPolicerEntries = 0;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;

            st = cpssExMxPolicerEntryMeterRefresh(dev, policerEntryIndex);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);

            continue;
        }

        /* 1.1. Write policer entry for testing. Call                       */
        /* cpssExMxPolicerEntrySet  with policerEntryIndex                  */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=      */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /* CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntrySet: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.2. Check with non-empty policer entry.
        Call with policerEntryIndex [maxNumOfPolicerEntries-1].
        Expected: GT_OK. */

        st = cpssExMxPolicerEntryMeterRefresh(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.3. Call cpssExMxPolicerEntryInvalidate with
        policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing.
        Expected: GT_OK. */
        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.4. Check with empty policer entry.
        Call with policerEntryIndex [maxNumOfPolicerEntries-1].
        Expected: GT_OK. */

        st = cpssExMxPolicerEntryMeterRefresh(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.5. Check out-of-range policerEntryIndex.
        Call with policerEntryIndex [maxNumOfPolicerEntries].
        Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;

        st = cpssExMxPolicerEntryMeterRefresh(dev, policerEntryIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);
    }

    policerEntryIndex = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryMeterRefresh(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryMeterRefresh(dev, policerEntryIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/* 
GT_STATUS cpssExMxPolicerGlobalCountersGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_EXMX_POLICER_GLOBAL_CNTR_STC   *countersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerGlobalCountersGet)
{
/*
ITERATE_DEVICES 
1.1. Call with non-NULL countersPtr. Expected: GT_OK.
1.2. Check for NULL pointers. Call with countersPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                               dev;
    CPSS_EXMX_POLICER_GLOBAL_CNTR_STC   counters;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-NULL countersPtr. Expected: GT_OK. */
        st = cpssExMxPolicerGlobalCountersGet(dev, &counters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Check for NULL pointers. Call with countersPtr [NULL].
        Expected: GT_BAD_PTR. */
        st = cpssExMxPolicerGlobalCountersGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerGlobalCountersGet(dev, &counters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerGlobalCountersGet(dev, &counters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerSetOfManagementCountersGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMX_POLICER_MNG_CONF_CNTR_SET_ENT     confCntrSet,
    OUT CPSS_EXMX_POLICER_MNG_CONF_CNTR_STC         *countersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerSetOfManagementCountersGet)
{
/*
ITERATE_DEVICES 
1.1. Call with confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E / CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET3_E] and non-NULL countersPtr. Expected: GT_OK.
1.2. Check with not allowed enum value. Call with confCntrSet [CPSS_EXMX_POLICER_MNG_NO_COUNT_E] and non-NULL countersPtr. Expected: non GT_OK.
1.3. Check out-of-range enum. Call with confCntrSet [0x5AAAAAA5] and non-NULL countersPtr. Expected: GT_BAD_PARAM.
1.4. Check for NULL pointers. Call with confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E] and countersPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                                       dev;
    CPSS_EXMX_POLICER_MNG_CONF_CNTR_SET_ENT     confCntrSet;
    CPSS_EXMX_POLICER_MNG_CONF_CNTR_STC         counters;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E
        / CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET3_E] and non-NULL countersPtr.
        Expected: GT_OK. */
        confCntrSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E;

        st = cpssExMxPolicerSetOfManagementCountersGet(dev, confCntrSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, confCntrSet);

        confCntrSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET3_E;

        st = cpssExMxPolicerSetOfManagementCountersGet(dev, confCntrSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, confCntrSet);

        /* 1.2. Check with not allowed enum value. Call with confCntrSet
        [CPSS_EXMX_POLICER_MNG_NO_COUNT_E] and non-NULL countersPtr.
        Expected: non GT_OK. */
        confCntrSet = CPSS_EXMX_POLICER_MNG_NO_COUNT_E;

        st = cpssExMxPolicerSetOfManagementCountersGet(dev, confCntrSet, &counters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, confCntrSet);

        /* 1.3. Check out-of-range enum. Call with confCntrSet [0x5AAAAAA5]
        and non-NULL countersPtr. Expected: GT_BAD_PARAM. */
        confCntrSet = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerSetOfManagementCountersGet(dev, confCntrSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, confCntrSet);

        /* 1.4. Check for NULL pointers. Call with
        confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E] and
        countersPtr [NULL]. Expected: GT_BAD_PTR. */
        confCntrSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E;

        st = cpssExMxPolicerSetOfManagementCountersGet(dev, confCntrSet, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, confCntrSet);
    }

    confCntrSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerSetOfManagementCountersGet(dev, confCntrSet, &counters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerSetOfManagementCountersGet(dev, confCntrSet, &counters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerSetOfManagementCountersSet
(
    IN GT_U8                                    devNum,
    IN CPSS_EXMX_POLICER_MNG_CONF_CNTR_SET_ENT  confCntrSet,
    IN CPSS_EXMX_POLICER_MNG_CONF_CNTR_STC      *countersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerSetOfManagementCountersSet)
{
/*
ITERATE_DEVICES 
1.1. Call with confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E], countersPtr[1,2,3,4,5,6,7,8]. Expected: GT_OK.
1.2. Call cpssExMxPolicerSetOfManagementCountersGet with confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E] and non-NULL countersPtr. Expected: GT_OK and same values as written in 1.1.
1.3. Call with confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET3_E], countersPtr[8,7,6,5,4,3,2,1]. Expected: GT_OK.
1.4. Call cpssExMxPolicerSetOfManagementCountersGet with confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET3_E] and non-NULL countersPtr. Expected: GT_OK and same values as written in 1.3.
1.5. Check with not allowed enum value. Call with confCntrSet [CPSS_EXMX_POLICER_MNG_NO_COUNT_E] and  countersPtr [1,2,3,4,5,6,7,8]. Expected: non GT_OK.
1.6. Check out-of-range enum. Call with confCntrSet [0x5AAAAAA5], countersPtr [1,2,3,4,5,6,7,8]. Expected: GT_BAD_PARAM.
1.7. Check for NULL pointers. Call with confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E] and countersPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                                       dev;
    CPSS_EXMX_POLICER_MNG_CONF_CNTR_SET_ENT     confCntrSet;
    CPSS_EXMX_POLICER_MNG_CONF_CNTR_STC         counters;

    GT_BOOL                                     failureWas;
    CPSS_EXMX_POLICER_MNG_CONF_CNTR_STC         retCounters;

    cpssOsBzero((GT_VOID*)&counters, sizeof(counters));

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E],
        countersPtr[1,2,3,4,5,6,7,8]. Expected: GT_OK. */
        counters.greenBytes.l[0] = 1;
        counters.greenPkts = 2;
        counters.yellowBytes.l[0] = 3;
        counters.yellowPkts = 4;
        counters.redBytes.l[0] = 5;
        counters.redPkts = 6;
        counters.dropBytes.l[0] = 7;
        counters.dropPkts = 8;

        confCntrSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E;

        st = cpssExMxPolicerSetOfManagementCountersSet(dev, confCntrSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, confCntrSet);

        /* 1.2. Call cpssExMxPolicerSetOfManagementCountersGet with
        confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E] and
        non-NULL countersPtr. Expected: GT_OK and same values as written in 1.1. */
        st = cpssExMxPolicerSetOfManagementCountersGet(dev, confCntrSet, &retCounters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
            "cpssExMxPolicerSetOfManagementCountersGet: %d, %d", dev, confCntrSet);
        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&counters,
                                            (const GT_VOID*)&retCounters, sizeof(counters))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another counters than has been written [dev = %d, confCntrSet = %d]",
                dev, confCntrSet);
        }

        /* 1.3. Call with confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET3_E],
        countersPtr[8,7,6,5,4,3,2,1]. Expected: GT_OK. */
        counters.greenBytes.l[0] = 8;
        counters.greenPkts = 7;
        counters.yellowBytes.l[0] = 6;
        counters.yellowPkts = 5;
        counters.redBytes.l[0] = 4;
        counters.redPkts = 3;
        counters.dropBytes.l[0] = 2;
        counters.dropPkts = 1;

        confCntrSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET3_E;

        st = cpssExMxPolicerSetOfManagementCountersSet(dev, confCntrSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, confCntrSet);

        /* 1.4. Call cpssExMxPolicerSetOfManagementCountersGet with
        confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET3_E] and
        non-NULL countersPtr. Expected: GT_OK and same values as written in 1.3. */
        st = cpssExMxPolicerSetOfManagementCountersGet(dev, confCntrSet, &retCounters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
            "cpssExMxPolicerSetOfManagementCountersGet: %d, %d", dev, confCntrSet);
        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&counters,
                                            (const GT_VOID*)&retCounters, sizeof(counters))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another counters than has been written [dev = %d, confCntrSet = %d]",
                dev, confCntrSet);

        }

        /* 1.5. Check with not allowed enum value. Call with
        confCntrSet [CPSS_EXMX_POLICER_MNG_NO_COUNT_E] and 
        countersPtr [1,2,3,4,5,6,7,8]. Expected: non GT_OK. */
        confCntrSet = CPSS_EXMX_POLICER_MNG_NO_COUNT_E;

        st = cpssExMxPolicerSetOfManagementCountersSet(dev, confCntrSet, &counters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, confCntrSet);

        /* 1.6. Check out-of-range enum. Call with confCntrSet [0x5AAAAAA5],
        countersPtr [1,2,3,4,5,6,7,8]. Expected: GT_BAD_PARAM. */
        confCntrSet = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerSetOfManagementCountersSet(dev, confCntrSet, &counters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, confCntrSet);

        /* 1.7. Check for NULL pointers. Call with
        confCntrSet [CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E] and
        countersPtr [NULL]. Expected: GT_BAD_PTR. */
        confCntrSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E;

        st = cpssExMxPolicerSetOfManagementCountersSet(dev, confCntrSet, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, confCntrSet);
    }

    confCntrSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E;
    counters.greenBytes.l[0] = 1;
    counters.greenPkts = 2;
    counters.yellowBytes.l[0] = 3;
    counters.yellowPkts = 4;
    counters.redBytes.l[0] = 5;
    counters.redPkts = 6;
    counters.dropBytes.l[0] = 7;
    counters.dropPkts = 8;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerSetOfManagementCountersSet(dev, confCntrSet, &counters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerSetOfManagementCountersSet(dev, confCntrSet, &counters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryCountersGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  policerEntryIndex,
    OUT CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  
                                                *pcktDuCountersPtr,
    OUT CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC   
                                                *confCountersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryCountersGet)
{
/*
ITERATE_DEVICES 
1.1. Write policer entry for testing. Call cpssExMxPolicerEntrySet with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Check with non-empty policer entry. Call with policerEntryIndex [maxNumOfPolicerEntries-1], non-NULL pcktDuCountersPtr and confCountersPtr. Expected: GT_OK.
1.3. Check with empty policer entry. Call with policerEntryIndex [0], non-NULL pcktDuCountersPtr and confCountersPtr. Expected: GT_OK.
1.4. Check out-of-range policerEntryIndex. Call with policerEntryIndex [maxNumOfPolicerEntries], non-NULL pcktDuCountersPtr and confCountersPtr. Expected: non GT_OK.
1.5. Check for only one null pointer of two. Call with policerEntryIndex [maxNumOfPolicerEntries-1], non-NULL pcktDuCountersPtr, confCountersPtr[NULL]. Expected: GT_OK.
1.6. Check for only one null pointer of two. Call with policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL], non-NULL confCountersPtr. Expected: GT_OK.
1.7. Check for two null pointers. Call with policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL], confCountersPtr[NULL]. Expected: GT_BAD_PTR.
1.8. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS                               st = GT_OK;

    GT_U8                                           dev;
    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  pcktDuCounters;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC       confCounters;
    GT_U32                                          policerEntryIndex;

    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    GT_U32                                  maxNumOfPolicerEntries = 0;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;

            st = cpssExMxPolicerEntryCountersGet(dev, policerEntryIndex,
                                                 &pcktDuCounters, &confCounters);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);

            continue;
        }

        /* 1.1. Write policer entry for testing. Call                       */
        /* cpssExMxPolicerEntrySet  with policerEntryIndex                  */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=      */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /* CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntrySet: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.2. Check with non-empty policer entry. Call with
        policerEntryIndex [maxNumOfPolicerEntries-1], non-NULL pcktDuCountersPtr
        and confCountersPtr. Expected: GT_OK. */
        st = cpssExMxPolicerEntryCountersGet(dev, policerEntryIndex,
                                             &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.3. Check with empty policer entry. Call with policerEntryIndex [0],
        non-NULL pcktDuCountersPtr and confCountersPtr. Expected: GT_OK. */
        policerEntryIndex = 0;

        st = cpssExMxPolicerEntryCountersGet(dev, policerEntryIndex,
                                             &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.4. Check out-of-range policerEntryIndex. Call with
        policerEntryIndex [maxNumOfPolicerEntries], non-NULL pcktDuCountersPtr
        and confCountersPtr. Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;

        st = cpssExMxPolicerEntryCountersGet(dev, policerEntryIndex,
                                             &pcktDuCounters, &confCounters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.5. Check for only one null pointer of two. Call with
        policerEntryIndex [maxNumOfPolicerEntries-1], non-NULL pcktDuCountersPtr,
        confCountersPtr[NULL]. Expected: GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntryCountersGet(dev, policerEntryIndex,
                                             &pcktDuCounters, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, confCountersPtr = NULL",
                                     dev, policerEntryIndex);

        /* 1.6. Check for only one null pointer of two. Call with
        policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL],
        non-NULL confCountersPtr. Expected: GT_OK. */
        st = cpssExMxPolicerEntryCountersGet(dev, policerEntryIndex,
                                             NULL, &confCounters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, pcktDuCountersPtr = NULL",
                                     dev, policerEntryIndex);

        /* 1.7. Check for two null pointers. Call with
        policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL],
        confCountersPtr[NULL]. Expected: NON GT_OK. */
        st = cpssExMxPolicerEntryCountersGet(dev, policerEntryIndex, NULL, NULL);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, NULL, NULL",
                                     dev, policerEntryIndex);

        /* 1.8. Call cpssExMxPolicerEntryInvalidate with
        policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing.
        Expected: GT_OK. */
        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);
    }

    /* set correct params */
    policerEntryIndex = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryCountersGet(dev, policerEntryIndex,
                                             &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryCountersGet(dev, policerEntryIndex,
                                         &pcktDuCounters, &confCounters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/* 
GT_STATUS cpssExMxPolicerEntryCountersGetAndReset
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  policerEntryIndex,
    IN  CPSS_EXMX_POLICER_COUNT_RESET_MODE_ENT  resetMode,
    OUT CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  
                                                *pcktDuCountersPtr,
    OUT CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC   
                                                *confCountersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryCountersGetAndReset)
{
/*
ITERATE_DEVICES 
1.1. Write policer entry for testing. Call cpssExMxPolicerEntrySet with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Check with non-empty policer entry. Call with policerEntryIndex [maxNumOfPolicerEntries-1], resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E / CPSS_EXMX_POLICER_COUNT_RESET_ALL_E], non-NULL pcktDuCountersPtr and confCountersPtr. Expected: GT_OK for Samba/Rumba and Tiger devices and GT_BAD_PARAM for others.
1.3. Check with empty policer entry. Call with policerEntryIndex [0], resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E / CPSS_EXMX_POLICER_COUNT_RESET_ALL_E], non-NULL pcktDuCountersPtr and confCountersPtr. Expected: GT_OK for Samba/Rumba and Tiger devices and GT_BAD_PARAM for others.
1.4. Check out-of-range policerEntryIndex. For Samba/Rumba and Tiger devices call with policerEntryIndex [maxNumOfPolicerEntries], resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E], non-NULL pcktDuCountersPtr and confCountersPtr. Expected: non GT_OK.
1.5. Check out-of-range enum. For Samba/Rumba and Tiger devices call with policerEntryIndex [maxNumOfPolicerEntries-1], resetMode[0x5AAAAAA5], non-NULL pcktDuCountersPtr and confCountersPtr. Expected: GT_BAD_PARAM.
1.6. Check for only one null pointer of two. For Samba/Rumba and Tiger devices call with policerEntryIndex [maxNumOfPolicerEntries-1], resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E],  non-NULL pcktDuCountersPtr, confCountersPtr[NULL]. Expected: GT_OK.
1.7. Check for only one null pointer of two. For Samba/Rumba and Tiger devices call with policerEntryIndex [maxNumOfPolicerEntries-1], resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E], pcktDuCountersPtr[NULL], non-NULL confCountersPtr. Expected: GT_OK.
1.8. Check for two null pointers. For Samba/Rumba and Tiger devices call with policerEntryIndex [maxNumOfPolicerEntries-1], resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E], pcktDuCountersPtr[NULL], confCountersPtr[NULL]. Expected: GT_OK.
1.9. For Samba/Rumba and Tiger devices call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS                               st = GT_OK;

    GT_U8                                           dev;
    CPSS_EXMX_POLICER_COUNT_RESET_MODE_ENT          resetMode;
    GT_U32                                          policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  pcktDuCounters;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC       confCounters;

    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    GT_U32                                  maxNumOfPolicerEntries = 0;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);

    st = prvUtfNextDeviceReset(&dev, UTF_TIGER_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;
            resetMode = CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E;

            st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                        resetMode, &pcktDuCounters, &confCounters);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);

            continue;
        }

        /* 1.1. Write policer entry for testing. Call                       */
        /* cpssExMxPolicerEntrySet  with policerEntryIndex                  */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=      */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /* CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntrySet: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.2. Check with non-empty policer entry. Call with
        policerEntryIndex [maxNumOfPolicerEntries-1],
        resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E /
        CPSS_EXMX_POLICER_COUNT_RESET_ALL_E], non-NULL pcktDuCountersPtr and
        confCountersPtr. Expected: GT_OK for Samba/Rumba and Tiger devices and
        GT_BAD_PARAM for others. */
        resetMode = CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E;

        st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                    resetMode, &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                     "Tiger or Samba device: %d, %d, %d",
                                     dev, policerEntryIndex, resetMode);

        resetMode = CPSS_EXMX_POLICER_COUNT_RESET_ALL_E;

        st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                    resetMode, &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                     "Tiger or Samba device: %d, %d, %d",
                                     dev, policerEntryIndex, resetMode);

        /* 1.3. Check with empty policer entry. Call with policerEntryIndex[0],
        resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E /
        CPSS_EXMX_POLICER_COUNT_RESET_ALL_E], non-NULL pcktDuCountersPtr and
        confCountersPtr. Expected: GT_OK for Samba/Rumba and Tiger devices and
        GT_BAD_PARAM for others. */
        policerEntryIndex = 0;

        resetMode = CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E;

        st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                    resetMode, &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                     "Tiger or Samba device: %d, %d, %d",
                                     dev, policerEntryIndex, resetMode);

        resetMode = CPSS_EXMX_POLICER_COUNT_RESET_ALL_E;

        st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                    resetMode, &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                     "Tiger or Samba device: %d, %d, %d",
                                     dev, policerEntryIndex, resetMode);

        /* 1.4. Check out-of-range policerEntryIndex. For Samba/Rumba and Tiger
        devices call with policerEntryIndex [maxNumOfPolicerEntries],
        resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E], non-NULL
        pcktDuCountersPtr and confCountersPtr. Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;
        resetMode = CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E;

        st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                    resetMode, &pcktDuCounters, &confCounters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.5. Check out-of-range enum. For Samba/Rumba and Tiger devices
        call with policerEntryIndex [maxNumOfPolicerEntries-1],
        resetMode[0x5AAAAAA5], non-NULL pcktDuCountersPtr and
        confCountersPtr. Expected: GT_BAD_PARAM. */
        policerEntryIndex = maxNumOfPolicerEntries - 1;
        resetMode = POLICER_INVALID_ENUM_CNS;

        st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                    resetMode, &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st,
                                    dev, policerEntryIndex, resetMode);

        /* 1.6. Check for only one null pointer of two. For Samba/Rumba
        and Tiger devices call with policerEntryIndex [maxNumOfPolicerEntries-1],
        resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E],
        non-NULL pcktDuCountersPtr, confCountersPtr[NULL]. Expected: GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries - 1;
        resetMode = CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E;

        st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                    resetMode, &pcktDuCounters, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, %d, %d, confCountersPtr =NULL", dev, policerEntryIndex, resetMode);

        /* 1.7. Check for only one null pointer of two. For Samba/Rumba and
        Tiger devices call with policerEntryIndex [maxNumOfPolicerEntries-1],
        resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E],
        pcktDuCountersPtr[NULL], non-NULL confCountersPtr. Expected: GT_OK. */
        st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                    resetMode, NULL, &confCounters);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, %d, %d, pcktDuCountersPtr =NULL", dev, policerEntryIndex, resetMode);

        /* 1.8. Check for two null pointers. For Samba/Rumba and Tiger devices
        call with policerEntryIndex [maxNumOfPolicerEntries-1],
        resetMode[CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E],
        pcktDuCountersPtr[NULL], confCountersPtr[NULL]. Expected: GT_OK. */
        st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                    resetMode, NULL, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, %d, %d, NULL, NULL", dev, policerEntryIndex, resetMode);

        /* 1.9. For Samba/Rumba and Tiger devices call
        cpssExMxPolicerEntryInvalidate with
        policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing.
        Expected: GT_OK. */
        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);
    }

    /* set correct params */
    policerEntryIndex = 0;
    resetMode = CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E;

    st = prvUtfNextDeviceReset(&dev, UTF_TIGER_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                    resetMode, &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryCountersGetAndReset(dev, policerEntryIndex,
                                resetMode, &pcktDuCounters, &confCounters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryCountersSet
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          policerEntryIndex,
    IN  CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  *pcktDuCountersPtr,
    IN  CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC       *confCountersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryCountersSet)
{
/*
ITERATE_DEVICES 
1.1. Write policer entry for testing. Call cpssExMxPolicerEntrySet with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Call with policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[1,2], confCountersPtr[3,4,5]. Expected: GT_OK.
1.3. Call cpssExMxPolicerEntryCountersGet with policerEntryIndex [maxNumOfPolicerEntries-1], non-NULL pcktDuCountersPtr and confCountersPtr. Expected: GT_OK and same values as written.
1.4. Check out-of-range policerEntryIndex. Call with policerEntryIndex [maxNumOfPolicerEntries], pcktDuCountersPtr[1,2], confCountersPtr[3,4,5]. Expected: non GT_OK.
1.5. Check for only one null pointer of two. Call with policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[1,2], confCountersPtr[NULL]. Expected: GT_OK.
1.6. Check for only one null pointer of two. Call with policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL], confCountersPtr[3,4,5]. Expected: GT_OK.
1.7. Check for two null pointers. Call with policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL], confCountersPtr[NULL]. Expected: GT_BAD_PTR.
1.8. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS                               st = GT_OK;

    GT_U8                                           dev;
    GT_U32                                          policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  pcktDuCounters;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC       confCounters;

    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  retPcktDuCounters;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC       retConfCounters;
    GT_BOOL                                         failureWas;

    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    GT_U32                                  maxNumOfPolicerEntries = 0;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);
    countEntryCnfg.counterMode = CPSS_EXMX_POLICER_COUNT_HYBRID_MODE_E;
    
    cpssOsBzero((GT_VOID*)&pcktDuCounters, sizeof(pcktDuCounters));
    cpssOsBzero((GT_VOID*)&confCounters, sizeof(confCounters));

    pcktDuCounters.cduCounter.l[0] = 1;
    pcktDuCounters.pktCounter.l[0] = 2;
    confCounters.greenDuCounter.l[0] = 3;
    confCounters.yellowDuCounter.l[0] = 4;
    confCounters.redDuCounter.l[0] = 5;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;

            st = cpssExMxPolicerEntryCountersSet(dev, policerEntryIndex,
                                                 &pcktDuCounters, &confCounters);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);
            continue;
        }

        /* 1.1. Write policer entry for testing. Call                       */
        /* cpssExMxPolicerEntrySet  with policerEntryIndex                  */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=      */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /*CPSS_EXMX_POLICER_COUNT_HYBRID_MODE_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntrySet: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.2. Call with policerEntryIndex [maxNumOfPolicerEntries-1],
        pcktDuCountersPtr[1,2], confCountersPtr[3,4,5]. Expected: GT_OK. */
        st = cpssExMxPolicerEntryCountersSet(dev, policerEntryIndex,
                                             &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.3. Call cpssExMxPolicerEntryCountersGet with
        policerEntryIndex [maxNumOfPolicerEntries-1], non-NULL pcktDuCountersPtr
        and confCountersPtr. Expected: GT_OK and same values as written. */
        st = cpssExMxPolicerEntryCountersGet(dev, policerEntryIndex,
                                             &retPcktDuCounters, &retConfCounters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
            "cpssExMxPolicerEntryCountersGet: %d, %d", dev, policerEntryIndex);

        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&pcktDuCounters,
                                            (const GT_VOID*)&retPcktDuCounters, sizeof(pcktDuCounters))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another pcktDuCounters than has been set [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);

            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&confCounters,
                                            (const GT_VOID*)&retConfCounters, sizeof(confCounters))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another confCounters than has been set [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);
        }

        /* 1.4. Check out-of-range policerEntryIndex. Call with
        policerEntryIndex [maxNumOfPolicerEntries], pcktDuCountersPtr[1,2],
        confCountersPtr[3,4,5]. Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;

        st = cpssExMxPolicerEntryCountersSet(dev, policerEntryIndex,
                                             &pcktDuCounters, &confCounters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.5. Check for only one null pointer of two. Call with
        policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[1,2],
        confCountersPtr[NULL]. Expected: GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntryCountersSet(dev, policerEntryIndex,
                                             &pcktDuCounters, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, confCountersPtr = NULL",
                                     dev, policerEntryIndex);

        /* 1.6. Check for only one null pointer of two. Call with
        policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL],
        confCountersPtr[3,4,5]. Expected: GT_OK. */
        st = cpssExMxPolicerEntryCountersSet(dev, policerEntryIndex,
                                             NULL, &confCounters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, pcktDuCountersPtr = NULL",
                                     dev, policerEntryIndex);

        /* 1.7. Check for two null pointers. Call with
        policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL],
        confCountersPtr[NULL]. Expected: NON GT_OK. */
        st = cpssExMxPolicerEntryCountersSet(dev, policerEntryIndex, NULL, NULL);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, NULL, NULL",
                                     dev, policerEntryIndex);

        /* 1.8. Call cpssExMxPolicerEntryInvalidate with
        policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing.
        Expected: GT_OK. */
        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);
    }

    /* set correct params */
    policerEntryIndex = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryCountersSet(dev, policerEntryIndex,
                                             &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryCountersSet(dev, policerEntryIndex,
                                         &pcktDuCounters, &confCounters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryCountersSetWithBypassWa
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          policerEntryIndex,
    IN  CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  *pcktDuCountersPtr,
    IN  CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC       *confCountersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryCountersSetWithBypassWa)
{
/*
ITERATE_DEVICES 
1.1. Write policer entry for testing. Call cpssExMxPolicerEntrySet with policerEntryIndex [maxNumOfPolicerEntries-1], meterEntryCnfgPtr [doMetering=GT_TRUE, remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE, meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode= CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=GT_FALSE, mngCounterSet= CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E, lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ], countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E ], non-NULL lbParamsPtr. Expected: GT_OK.
1.2. Call with policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[1,2], confCountersPtr[3,4,5]. Expected: GT_OK.
1.3. Call cpssExMxPolicerEntryCountersGet with policerEntryIndex [maxNumOfPolicerEntries-1], non-NULL pcktDuCountersPtr and confCountersPtr. Expected: GT_OK and same values as written.
1.4. Check out-of-range policerEntryIndex. Call with policerEntryIndex [maxNumOfPolicerEntries], pcktDuCountersPtr[1,2], confCountersPtr[3,4,5]. Expected: non GT_OK.
1.5. Check for only one null pointer of two. Call with policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[1,2], confCountersPtr[NULL]. Expected: GT_OK.
1.6. Check for only one null pointer of two. Call with policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL], confCountersPtr[3,4,5]. Expected: GT_OK.
1.7. Check for two null pointers. Call with policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL], confCountersPtr[NULL]. Expected: NON GT_OK.
1.8. Call cpssExMxPolicerEntryInvalidate with policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS                               st = GT_OK;

    GT_U8                                           dev;
    GT_U32                                          policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  pcktDuCounters;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC       confCounters;

    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  retPcktDuCounters;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC       retConfCounters;
    GT_BOOL                                         failureWas;

    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfg;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfg;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;

    GT_U32                                  maxNumOfPolicerEntries = 0;

    policerMeterEntryCnfgDefaultSet(&meterEntryCnfg);
    policerCountEntryCnfgDefaultSet(&countEntryCnfg);
    countEntryCnfg.counterMode = CPSS_EXMX_POLICER_COUNT_HYBRID_MODE_E;

    cpssOsBzero((GT_VOID*)&pcktDuCounters, sizeof(pcktDuCounters));
    cpssOsBzero((GT_VOID*)&confCounters, sizeof(confCounters));

    pcktDuCounters.cduCounter.l[0] = 1;
    pcktDuCounters.pktCounter.l[0] = 2;
    confCounters.greenDuCounter.l[0] = 3;
    confCounters.yellowDuCounter.l[0] = 4;
    confCounters.redDuCounter.l[0] = 5;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*  Get maxNumOfPolicerEntries */
        st = prvUtfExMxMaxNumOfPolicerEntriesGet(dev, &maxNumOfPolicerEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "prvUtfExMxMaxNumOfPolicerEntriesGet: %d", dev);

        if (0 == maxNumOfPolicerEntries)
        {
            policerEntryIndex = 0;

            st = cpssExMxPolicerEntryCountersSetWithBypassWa(dev, policerEntryIndex,
                                                 &pcktDuCounters, &confCounters);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, maxNumOfPolicerEntries = %d",
                                            dev, policerEntryIndex, maxNumOfPolicerEntries);
            continue;
        }

        /* 1.1. Write policer entry for testing. Call                       */
        /* cpssExMxPolicerEntrySet  with policerEntryIndex                  */
        /* [maxNumOfPolicerEntries-1], meterEntryCnfgPtr[doMetering=GT_TRUE,*/
        /* remarkBasedOnL3Cos=GT_TRUE, remarkDp=GT_FALSE, modifyTcEnable=   */
        /* GT_FALSE, remarkBasedOnL2Cos=GT_FALSE, dropRed=GT_FALSE,         */
        /* meterColorMode= CPSS_EXMX_POLICER_COLOR_BLIND_E, meterMode=      */
        /* CPSS_EXMX_POLICER_METER_SR_TCM_E, modifyExternalCosEnable=       */
        /* GT_FALSE,mngCounterSet=CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E,*/
        /* lbParams [srTcmParams {cir=10000, cbs=65535, ebs=256000}] ],     */
        /* countEntryCnfgPtr[ doCounting=GT_FALSE, counterDuMode =          */
        /* CPSS_POLICER_EXMX_COUNT_DU1_E, counterMode =                     */
        /* CPSS_EXMX_POLICER_COUNT_HYBRID_MODE_E ], non-NULL lbParamsPtr.  */
        /* Expected: GT_OK.                                                 */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntrySet(dev, policerEntryIndex,
                &meterEntryCnfg, &countEntryCnfg, &lbParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntrySet: %d, %d",
                                     dev, policerEntryIndex);

        /* 1.2. Call with policerEntryIndex [maxNumOfPolicerEntries-1],
        pcktDuCountersPtr[1,2], confCountersPtr[3,4,5]. Expected: GT_OK. */
        st = cpssExMxPolicerEntryCountersSetWithBypassWa(dev, policerEntryIndex,
                                             &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.3. Call cpssExMxPolicerEntryCountersGet with
        policerEntryIndex [maxNumOfPolicerEntries-1], non-NULL pcktDuCountersPtr
        and confCountersPtr. Expected: GT_OK and same values as written. */
        st = cpssExMxPolicerEntryCountersGet(dev, policerEntryIndex,
                                             &retPcktDuCounters, &retConfCounters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
            "cpssExMxPolicerEntryCountersGet: %d, %d", dev, policerEntryIndex);

        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&pcktDuCounters,
                                            (const GT_VOID*)&retPcktDuCounters, sizeof(pcktDuCounters))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another pcktDuCounters than has been set [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);

            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&confCounters,
                                            (const GT_VOID*)&retConfCounters, sizeof(confCounters))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "get another confCounters than has been set [dev = %d, policerEntryIndex = %d]",
                dev, policerEntryIndex);
        }

        /* 1.4. Check out-of-range policerEntryIndex. Call with
        policerEntryIndex [maxNumOfPolicerEntries], pcktDuCountersPtr[1,2],
        confCountersPtr[3,4,5]. Expected: non GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries;

        st = cpssExMxPolicerEntryCountersSetWithBypassWa(dev, policerEntryIndex,
                                             &pcktDuCounters, &confCounters);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, policerEntryIndex);

        /* 1.5. Check for only one null pointer of two. Call with
        policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[1,2],
        confCountersPtr[NULL]. Expected: GT_OK. */
        policerEntryIndex = maxNumOfPolicerEntries - 1;

        st = cpssExMxPolicerEntryCountersSetWithBypassWa(dev, policerEntryIndex,
                                             &pcktDuCounters, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, confCountersPtr = NULL",
                                     dev, policerEntryIndex);

        /* 1.6. Check for only one null pointer of two. Call with
        policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL],
        confCountersPtr[3,4,5]. Expected: GT_OK. */
        st = cpssExMxPolicerEntryCountersSetWithBypassWa(dev, policerEntryIndex,
                                             NULL, &confCounters);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, pcktDuCountersPtr = NULL",
                                     dev, policerEntryIndex);

        /* 1.7. Check for two null pointers. Call with
        policerEntryIndex [maxNumOfPolicerEntries-1], pcktDuCountersPtr[NULL],
        confCountersPtr[NULL]. Expected: NON GT_OK. */
        st = cpssExMxPolicerEntryCountersSetWithBypassWa(dev, policerEntryIndex, NULL, NULL);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, NULL, NULL",
                                     dev, policerEntryIndex);

        /* 1.8. Call cpssExMxPolicerEntryInvalidate with
        policerEntryIndex [maxNumOfPolicerEntries-1] to cleanup after testing.
        Expected: GT_OK. */
        st = cpssExMxPolicerEntryInvalidate(dev, policerEntryIndex);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPolicerEntryInvalidate: %d, %d",
                                     dev, policerEntryIndex);
    }

    /* set correct params */
    policerEntryIndex = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryCountersSetWithBypassWa(dev, policerEntryIndex,
                                             &pcktDuCounters, &confCounters);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryCountersSetWithBypassWa(dev, policerEntryIndex,
                                         &pcktDuCounters, &confCounters);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerCountersAlarmLimitSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMX_POLICER_ALARM_LIMIT_STC   *countersAlarmLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerCountersAlarmLimitSet)
{
/*
ITERATE_DEVICES
1.1. Call with countersAlarmLimit[packetCountLimit= 0x1fffffff; cduCountLimit=10,000,000; conformanceCountLimit= 0x4ffffffffff]. Expected: GT_OK.
1.2. Check for null pointers. Call with countersAlarmLimit [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                               dev;
    CPSS_EXMX_POLICER_ALARM_LIMIT_STC   countersAlarmLimit;

    countersAlarmLimit.packetCountLimit = 0xfffffff;
    countersAlarmLimit.cduCountLimit = 10000000;
    countersAlarmLimit.conformanceCountLimit.l[0] = 0xffffffff;
    countersAlarmLimit.conformanceCountLimit.l[1] = 0x3ff;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with countersAlarmLimit[packetCountLimit= 0x1fffffff;
        cduCountLimit=10,000,000; conformanceCountLimit= 0x4ffffffffff].
        Expected: GT_OK. */
        
        st = cpssExMxPolicerCountersAlarmLimitSet(dev, &countersAlarmLimit);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, packetCountLimit= %d, cduCountLimit = %d, conformanceCountLimit.l[0] = %d",
            dev, countersAlarmLimit.packetCountLimit, countersAlarmLimit.cduCountLimit, countersAlarmLimit.conformanceCountLimit.l[0]);

        /* 1.2. Check for null pointers. Call with countersAlarmLimit [NULL].
        Expected: GT_BAD_PTR. */
        st = cpssExMxPolicerCountersAlarmLimitSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerCountersAlarmLimitSet(dev, &countersAlarmLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerCountersAlarmLimitSet(dev, &countersAlarmLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerLargeCountersAlarmLimitSet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMX_POLICER_ALARM_LIMIT_LARGE_STC     *largeCountersAlarmLimit

)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerLargeCountersAlarmLimitSet)
{
/*
ITERATE_DEVICES
1.1. Call with largeCountersAlarmLimit [packetCountLimit =10,000,000; cduCountLimit =10,000,000]. Expected: GT_OK for Samba/Rumba, Tiger devices and GT_BAD_PARAM for others.
1.2. Check for null pointers. For Samba/Rumba, Tiger devices call with largeCountersAlarmLimit [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                                       dev;
    CPSS_EXMX_POLICER_ALARM_LIMIT_LARGE_STC     largeCountersAlarmLimit;

    cpssOsBzero((GT_VOID*)&largeCountersAlarmLimit, sizeof(largeCountersAlarmLimit));

    largeCountersAlarmLimit.packetCountLimit.l[0] = 10000000;
    largeCountersAlarmLimit.cduCountLimit.l[0] = 10000000;

    st = prvUtfNextDeviceReset(&dev, UTF_TIGER_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with largeCountersAlarmLimit [packetCountLimit =10,000,000;
        cduCountLimit =10,000,000].
        Expected: GT_OK for Samba/Rumba, Tiger devices and GT_BAD_PARAM for others. */

        st = cpssExMxPolicerLargeCountersAlarmLimitSet(dev, &largeCountersAlarmLimit);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "Tiger or Samba device: %d, packetCountLimit = %d, cduCountLimit = %d",
            dev, largeCountersAlarmLimit.packetCountLimit.l[0], largeCountersAlarmLimit.cduCountLimit.l[0]);

        /* 1.2. Check for null pointers. For Samba/Rumba, Tiger devices
        call with largeCountersAlarmLimit [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxPolicerLargeCountersAlarmLimitSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_TIGER_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerLargeCountersAlarmLimitSet(dev, &largeCountersAlarmLimit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerLargeCountersAlarmLimitSet(dev, &largeCountersAlarmLimit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *cduCountExceedPtr,
    OUT GT_BOOL     *pcktCountExceedPtr,
    OUT GT_U32      *policerEntryIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet)
{
/*
ITERATE_DEVICES
1.1. Call with non-NULL cduCountExceedPtr, pcktCountExceedPtr, policerEntryIndexPtr. Expected: GT_OK.
1.2. Check for null pointers. Call with cduCountExceedPtr [NULL], others not NULL. Expected: GT_BAD_PTR.
1.3. Check for null pointers. Call with pcktCountExceedPtr [NULL], others not NULL. Expected: GT_BAD_PTR.
1.4. Check for null pointers. Call with policerEntryIndexPtr [NULL], others not NULL. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8     dev;
    GT_BOOL   cduCountExceed;
    GT_BOOL   pcktCountExceed;
    GT_U32    policerEntryIndex;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-NULL cduCountExceedPtr, pcktCountExceedPtr,
        policerEntryIndexPtr. Expected: GT_OK. */
        st = cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet(dev,
                        &cduCountExceed, &pcktCountExceed, &policerEntryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Check for null pointers. Call with cduCountExceedPtr [NULL],
        others not NULL. Expected: GT_BAD_PTR. */
        st = cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet(dev,
                        NULL, &pcktCountExceed, &policerEntryIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cduCountExceedPtr = NULL", dev);

        /* 1.3. Check for null pointers. Call with pcktCountExceedPtr [NULL],
        others not NULL. Expected: GT_BAD_PTR. */
        st = cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet(dev,
                        &cduCountExceed, NULL, &policerEntryIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, pcktCountExceedPtr = NULL", dev);

        /* 1.4. Check for null pointers. Call with policerEntryIndexPtr [NULL],
        others not NULL. Expected: GT_BAD_PTR. */
        st = cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet(dev,
                        &cduCountExceed, &pcktCountExceed, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, policerEntryIndexPtr = NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet(dev,
                        &cduCountExceed, &pcktCountExceed, &policerEntryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet(dev,
                    &cduCountExceed, &pcktCountExceed, &policerEntryIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPolicerEntryConfCountAlarmInfoGet
(
    IN  GT_U8       devNum,
    OUT GT_BOOL     *greenExceedPtr,
    OUT GT_BOOL     *yellowExceedPtr,
    OUT GT_BOOL     *redExceedPtr,
    OUT GT_U32      *policerEntryIndexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPolicerEntryConfCountAlarmInfoGet)
{
/*
ITERATE_DEVICES
1.1. Call with non-NULL greenExceedPtr, yellowExceedPtr, redExceedPtr, policerEntryIndexPtr. Expected: GT_OK.
1.2. Check for null pointers. Call with greenExceedPtr [NULL], others not NULL. Expected: GT_BAD_PTR.
1.3. Check for null pointers. Call with yellowExceedPtr [NULL], others not NULL. Expected: GT_BAD_PTR.
1.4. Check for null pointers. Call with redExceedPtr [NULL], others not NULL. Expected: GT_BAD_PTR.
1.5. Check for null pointers. Call with policerEntryIndexPtr [NULL], others not NULL. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8     dev;
    GT_BOOL   greenExceed;
    GT_BOOL   yellowExceed;
    GT_BOOL   redExceed;
    GT_U32    policerEntryIndex;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-NULL greenExceedPtr, yellowExceedPtr,
        redExceedPtr, policerEntryIndexPtr. Expected: GT_OK. */
        st = cpssExMxPolicerEntryConfCountAlarmInfoGet(dev, &greenExceed,
                                &yellowExceed, &redExceed, &policerEntryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Check for null pointers. Call with greenExceedPtr [NULL],
        others not NULL. Expected: GT_BAD_PTR. */
        st = cpssExMxPolicerEntryConfCountAlarmInfoGet(dev, NULL,
                                &yellowExceed, &redExceed, &policerEntryIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, greenExceedPtr = NULL", dev);

        /* 1.3. Check for null pointers. Call with yellowExceedPtr [NULL],
        others not NULL. Expected: GT_BAD_PTR. */
        st = cpssExMxPolicerEntryConfCountAlarmInfoGet(dev, &greenExceed,
                                NULL, &redExceed, &policerEntryIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, yellowExceedPtr = NULL", dev);

        /* 1.4. Check for null pointers. Call with redExceedPtr [NULL],
        others not NULL. Expected: GT_BAD_PTR. */
        st = cpssExMxPolicerEntryConfCountAlarmInfoGet(dev, &greenExceed,
                                &yellowExceed, NULL, &policerEntryIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, redExceedPtr = NULL", dev);

        /* 1.5. Check for null pointers. Call with policerEntryIndexPtr [NULL],
        others not NULL. Expected: GT_BAD_PTR. */
        st = cpssExMxPolicerEntryConfCountAlarmInfoGet(dev, &greenExceed,
                                &yellowExceed, &redExceed, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, policerEntryIndexPtr = NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPolicerEntryConfCountAlarmInfoGet(dev, &greenExceed,
                                &yellowExceed, &redExceed, &policerEntryIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPolicerEntryConfCountAlarmInfoGet(dev, &greenExceed,
                            &yellowExceed, &redExceed, &policerEntryIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxPolicer suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPolicer)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerCountingEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerMeteringEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerPacketSizeModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerMeterDuRatioSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerDefaultDscpSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerL2RemarkModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerModifyUpPacketTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryCountingUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryMeteringUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryMeteringUpdateWithBypassWa)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryCountingEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryMeteringEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryMeterParamsCalculate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryMeterRefresh)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerGlobalCountersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerSetOfManagementCountersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerSetOfManagementCountersSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryCountersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryCountersGetAndReset)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryCountersSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryCountersSetWithBypassWa)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerCountersAlarmLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerLargeCountersAlarmLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPolicerEntryConfCountAlarmInfoGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPolicer)

/*----------------------------------------------------------------------------*/

/*  Internal function. Is used for filling Policer meterEntryCnfg structure */
/*  with default values which are used for most of all tests.               */
static void policerMeterEntryCnfgDefaultSet
(
    IN CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC     *meterEntryCnfgPtr
)
{
/*
    GT_BOOL                                 doMetering;             GT_TRUE
    GT_BOOL                                 remarkBasedOnL3Cos;     GT_TRUE
    GT_BOOL                                 remarkDp;               GT_FALSE                           
    GT_BOOL                                 modifyTcEnable;         GT_FALSE
    GT_BOOL                                 remarkBasedOnL2Cos;     GT_FALSE
    GT_BOOL                                 dropRed;                GT_FALSE
    CPSS_EXMX_POLICER_COLOR_MODE_ENT        meterColorMode;         CPSS_POLICER_COLOR_BLIND_E
    CPSS_EXMX_POLICER_METER_MODE_ENT        meterMode;              CPSS_EXMX_POLICER_METER_SR_TCM_E
    GT_BOOL                                 modifyExternalCosEnable;        GT_FALSE
    CPSS_EXMX_POLICER_MNG_CONF_CNTR_SET_ENT mngCounterSet;          CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParams;               srTcmParams {cir=10000, cbs=65535, ebs=256000}
*/
    if (NULL == meterEntryCnfgPtr)
    {
        return;
    }

    cpssOsBzero((GT_VOID*)meterEntryCnfgPtr, sizeof(CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC));

    meterEntryCnfgPtr->doMetering = GT_TRUE;
    meterEntryCnfgPtr->remarkBasedOnL3Cos = GT_TRUE;
    meterEntryCnfgPtr->remarkDp = GT_FALSE;
    meterEntryCnfgPtr->modifyTcEnable = GT_FALSE;
    meterEntryCnfgPtr->remarkBasedOnL2Cos = GT_FALSE;
    meterEntryCnfgPtr->dropRed = GT_FALSE;

    meterEntryCnfgPtr->meterColorMode = CPSS_POLICER_COLOR_BLIND_E;
    meterEntryCnfgPtr->meterMode = CPSS_EXMX_POLICER_METER_SR_TCM_E;

    meterEntryCnfgPtr->modifyExternalCosEnable = GT_FALSE;

    meterEntryCnfgPtr->mngCounterSet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E;

    meterEntryCnfgPtr->lbParams.srTcmParams.cir = 10000;
    meterEntryCnfgPtr->lbParams.srTcmParams.cbs = 65535;
    meterEntryCnfgPtr->lbParams.srTcmParams.ebs = 256000;
}

/*----------------------------------------------------------------------------*/

/*  Internal function. Is used for filling Policer countEntryCnfg structure */
/*  with default values which are used for most of all tests.               */
static void policerCountEntryCnfgDefaultSet
(
    IN CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC     *countEntryCnfgPtr
)
{
/*
    GT_BOOL                                 doCounting;     GT_FALSE
    CPSS_EXMX_POLICER_COUNT_DU_MODE_ENT     counterDuMode;  CPSS_POLICER_EXMX_COUNT_DU1_E
    CPSS_EXMX_POLICER_COUNT_MODE_ENT        counterMode;    CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E
*/
    if (NULL == countEntryCnfgPtr)
    {
        return;
    }

    cpssOsBzero((GT_VOID*)countEntryCnfgPtr, sizeof(CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC));
    countEntryCnfgPtr->doCounting = GT_FALSE;
    countEntryCnfgPtr->counterDuMode = CPSS_POLICER_EXMX_COUNT_DU1_E;
    countEntryCnfgPtr->counterMode = CPSS_EXMX_POLICER_COUNT_PACKETS_ONLY_E;
}
