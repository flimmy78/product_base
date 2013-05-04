/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtHyperGlinkUT.c
*
* DESCRIPTION:
*       Unit tests for gtHyperGlink, that provides
*       Prestera API definitions for HyperG.Link fabric ports.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpssXbar/generic/xbarControl/gtHyperGlink.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define XBAR_HYPER_GLINK_INVALID_ENUM_CNS   0x5AAAAAA5

#define XBAR_HYPER_GLINK_VALID_FPORT_CNS    0


/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgSetInitParams
(
    IN   GT_U8                  devNum,
    IN   GT_U8                  hgParamsLen,
    IN   GT_HG_INIT_PARAMS      *hgParams
)
*/
UTF_TEST_CASE_MAC(gtHgSetInitParams)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with hgParamsLen[1],
                   hgParams {fport [0 / 2], 
                             enable [GT_FALSE / GT_TRUE],
                             swapLaneOrder [GT_FALSE / GT_TRUE], 
                             numHglinkLanes [GT_HG_4_LANES / GT_HG_6_LANES], 
                             preEmphasis [GT_HG_DRIVER_PRE_EMPHASIS_0_PC / GT_HG_DRIVER_PRE_EMPHASIS_28_PC], 
                             clockSpeed [GT_HG_CLOCK_2_GHZ / GT_HG_CLOCK_3_125_GHZ], 
                             repeaterMode [GT_FALSE / GT_TRUE], 
                             vcm [GT_HG_17V_VCM / GT_HG_14V_VCM]}.
    Expected: GT_OK.
    1.2. Call gtHgGetInitParams with the same hgParamsLen 
                                and non-NULL hgParams.
    Expected: GT_OK and the same hgParams.
    1.3. Call with out of range hgParams->fport [maxFportNum(dev) + 1] 
                   and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range hgParams->numHglinkLanes [0x5AAAAAA5] 
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range hgParams->preEmphasis [0x5AAAAAA5] 
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range hgParams->clockSpeed [0x5AAAAAA5] 
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with out of range hgParams->vcm [0x5AAAAAA5] 
                   and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with hgParams [NULL] 
                    and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_U8               hgParamsLen = 0;
    GT_HG_INIT_PARAMS   hgParams;
    GT_HG_INIT_PARAMS   hgParamsGet;


    cpssOsBzero((GT_VOID*) &hgParams, sizeof(hgParams));
    cpssOsBzero((GT_VOID*) &hgParamsGet, sizeof(hgParamsGet));

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with hgParamsLen[1],
                           hgParams {fport [0 / 2], 
                                     enable [GT_FALSE / GT_TRUE],
                                     swapLaneOrder [GT_FALSE / GT_TRUE], 
                                     numHglinkLanes [GT_HG_4_LANES / GT_HG_6_LANES], 
                                     preEmphasis [GT_HG_DRIVER_PRE_EMPHASIS_0_PC / GT_HG_DRIVER_PRE_EMPHASIS_28_PC], 
                                     clockSpeed [GT_HG_CLOCK_2_GHZ / GT_HG_CLOCK_3_125_GHZ], 
                                     repeaterMode [GT_FALSE / GT_TRUE], 
                                     vcm [GT_HG_17V_VCM / GT_HG_14V_VCM]}.
            Expected: GT_OK.
        */
        /* Call with hgParams.fport = 0 */
        hgParamsLen                         = 1;
        hgParams.fport                      = 0;
        hgParams.enable                     = GT_FALSE;
        hgParams.swapLaneOrder              = GT_FALSE;
        hgParams.numHglinkLanes             = GT_HG_4_LANES;
        hgParams.preEmphasis                = GT_HG_DRIVER_PRE_EMPHASIS_0_PC;
        hgParams.clockSpeed                 = GT_HG_CLOCK_2_GHZ;
        hgParams.repeaterMode               = GT_FALSE;
        hgParams.vcm                        = GT_HG_17V_VCM;

        st = gtHgSetInitParams(dev, hgParamsLen, &hgParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hgParamsLen);

        /*
            1.2. Call gtHgGetInitParams with the same hgParamsLen 
                                        and non-NULL hgParams.
            Expected: GT_OK and the same hgParams.
        */
        st = gtHgGetInitParams(dev, &hgParamsLen, &hgParamsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtHgGetInitParams: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.fport, hgParamsGet.fport,
                   "get another hgParams->fport than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.enable, hgParamsGet.enable,
                   "get another hgParams->enable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.swapLaneOrder, hgParamsGet.swapLaneOrder,
                   "get another hgParams->swapLaneOrder than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.numHglinkLanes, hgParamsGet.numHglinkLanes,
                   "get another hgParams->numHglinkLanes than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.preEmphasis, hgParamsGet.preEmphasis,
                   "get another hgParams->preEmphasis than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.clockSpeed, hgParamsGet.clockSpeed,
                   "get another hgParams->clockSpeed than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.repeaterMode, hgParamsGet.repeaterMode,
                   "get another hgParams->repeaterMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.vcm, hgParamsGet.vcm,
                   "get another hgParams->vcm than was set: %d", dev);
        
        /* Call with hgParams.fport = 2 */
        hgParams.fport                     = 2;
        hgParams.enable                    = GT_TRUE;
        hgParams.swapLaneOrder             = GT_TRUE;
        hgParams.numHglinkLanes            = GT_HG_6_LANES;
        hgParams.preEmphasis               = GT_HG_DRIVER_PRE_EMPHASIS_28_PC;
        hgParams.clockSpeed                = GT_HG_CLOCK_3_125_GHZ;
        hgParams.repeaterMode              = GT_TRUE;
        hgParams.vcm                       = GT_HG_14V_VCM;

        st = gtHgSetInitParams(dev, hgParamsLen, &hgParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hgParamsLen);

        /*
            1.2. Call gtHgGetInitParams with the same hgParamsLen 
                                        and non-NULL hgParams.
            Expected: GT_OK and the same hgParams.
        */
        st = gtHgGetInitParams(dev, &hgParamsLen, &hgParamsGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "gtHgGetInitParams: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.fport, hgParamsGet.fport,
                   "get another hgParams->fport than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.enable, hgParamsGet.enable,
                   "get another hgParams->enable than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.swapLaneOrder, hgParamsGet.swapLaneOrder,
                   "get another hgParams->swapLaneOrder than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.numHglinkLanes, hgParamsGet.numHglinkLanes,
                   "get another hgParams->numHglinkLanes than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.preEmphasis, hgParamsGet.preEmphasis,
                   "get another hgParams->preEmphasis than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.clockSpeed, hgParamsGet.clockSpeed,
                   "get another hgParams->clockSpeed than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.repeaterMode, hgParamsGet.repeaterMode,
                   "get another hgParams->repeaterMode than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(hgParams.vcm, hgParamsGet.vcm,
                   "get another hgParams->vcm than was set: %d", dev);
       
        /*
            1.3. Call with out of range hgParams->fport [maxFportNum(dev) + 1] 
                           and other params from 1.1.
            Expected: NOT GT_OK.
        */
        hgParams.fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev) + 1;

        st = gtHgSetInitParams(dev, hgParamsLen, &hgParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, hgParams->fport = %d", dev, hgParams.fport);

        hgParams.fport = 0;

        /*
            1.4. Call with out of range hgParams->numHglinkLanes [0x5AAAAAA5]
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        hgParams.numHglinkLanes = XBAR_HYPER_GLINK_INVALID_ENUM_CNS;

        st = gtHgSetInitParams(dev, hgParamsLen, &hgParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, hgParams->numHglinkLanes = %d", dev, hgParams.numHglinkLanes);

        hgParams.numHglinkLanes = GT_HG_6_LANES;

        /*
            1.5. Call with out of range hgParams->preEmphasis [0x5AAAAAA5] 
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        hgParams.preEmphasis = XBAR_HYPER_GLINK_INVALID_ENUM_CNS;

        st = gtHgSetInitParams(dev, hgParamsLen, &hgParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, hgParams->preEmphasis = %d", dev, hgParams.preEmphasis);

        hgParams.preEmphasis = GT_HG_DRIVER_PRE_EMPHASIS_28_PC;

        /*
            1.6. Call with out of range hgParams->clockSpeed [0x5AAAAAA5] 
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        hgParams.clockSpeed = XBAR_HYPER_GLINK_INVALID_ENUM_CNS;

        st = gtHgSetInitParams(dev, hgParamsLen, &hgParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, hgParams->clockSpeed = %d", dev, hgParams.clockSpeed);

        hgParams.clockSpeed = GT_HG_CLOCK_3_125_GHZ;

        /*
            1.7. Call with out of range hgParams->vcm [0x5AAAAAA5] 
                           and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        hgParams.vcm = XBAR_HYPER_GLINK_INVALID_ENUM_CNS;

        st = gtHgSetInitParams(dev, hgParamsLen, &hgParams);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, hgParams->vcm = %d", dev, hgParams.vcm);

        hgParams.vcm = GT_HG_14V_VCM;

        /*
            1.8. Call with hgParams [NULL] 
                            and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtHgSetInitParams(dev, hgParamsLen, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, hgParams = NULL", dev);
    }

    hgParamsLen                         = 1;
    hgParams.fport                      = 0;
    hgParams.enable                     = GT_FALSE;
    hgParams.swapLaneOrder              = GT_FALSE;
    hgParams.numHglinkLanes             = GT_HG_4_LANES;
    hgParams.preEmphasis                = GT_HG_DRIVER_PRE_EMPHASIS_0_PC;
    hgParams.clockSpeed                 = GT_HG_CLOCK_2_GHZ;
    hgParams.repeaterMode               = GT_FALSE;
    hgParams.vcm                        = GT_HG_17V_VCM;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgSetInitParams(dev, hgParamsLen, &hgParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgSetInitParams(dev, hgParamsLen, &hgParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgGetInitParams
(
    IN      GT_U8               devNum,
    INOUT   GT_U8               *hgParamsLen,
    OUT     GT_HG_INIT_PARAMS   *hgParams
)
*/
UTF_TEST_CASE_MAC(gtHgGetInitParams)
{
/*
    ITERATE_XBAR_DEVICES
    1.1. Call with hgParamsLen[maxFportNum(dev)] 
                   and non-NULL  hgParams.
    Expected: GT_OK.
    1.2. Call with out of range hgParamsLen[0 / maxFportNum(dev) + 1] 
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with hgParamsLen [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with hgParams [NULL] 
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;    
    GT_U8       dev;

    GT_U8               hgParamsLen = 0;
    GT_HG_INIT_PARAMS   hgParams[256];


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with hgParamsLen[maxFportNum(dev)] 
                           and non-NULL  hgParams.
            Expected: GT_OK.
        */
        hgParamsLen = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgGetInitParams(dev, &hgParamsLen, hgParams);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, hgParamsLen);
        
        /*
            1.2. Call with out of range hgParamsLen[0 / maxFportNum(dev) + 1] 
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        /* Call with hgParamsLen = 0 */
        hgParamsLen = 0;

        st = gtHgGetInitParams(dev, &hgParamsLen, hgParams);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, hgParamsLen);

        /* Call with hgParamsLen = maxFportNum(dev) + 1 */
        hgParamsLen = UTF_GET_MAX_FPORTS_NUM_MAC(dev) + 1;

        st = gtHgGetInitParams(dev, &hgParamsLen, hgParams);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, hgParamsLen);

        hgParamsLen = 1;

        /*
            1.3. Call with hgParamsLen [NULL] 
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtHgGetInitParams(dev, NULL, hgParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, hgParamsLen = NULL", dev);

        /*
            1.4. Call with hgParams [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = gtHgGetInitParams(dev, &hgParamsLen, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, hgParams = NULL", dev);
    }

    hgParamsLen = 1;

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgGetInitParams(dev, &hgParamsLen, hgParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgGetInitParams(dev, &hgParamsLen, hgParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgPcsTxResetEnable
(
    IN   GT_U8     devNum,
    IN   GT_U8     fPort,
    IN   GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(gtHgPcsTxResetEnable)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* Call with enable = GT_FALSE */
            enable = GT_FALSE;

            st = gtHgPcsTxResetEnable(dev, fport, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, enable);

            /* Call with enable = GT_TRUE */
            enable = GT_TRUE;

            st = gtHgPcsTxResetEnable(dev, fport, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, enable);
        }

        enable = GT_FALSE;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgPcsTxResetEnable(dev, fport, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgPcsTxResetEnable(dev, fport, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    enable = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgPcsTxResetEnable(dev, fport, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgPcsTxResetEnable(dev, fport, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgPcsRxLaneResetEnable
(
    IN   GT_U8     devNum,
    IN   GT_U8     fPort,
    IN   GT_U8     lane,
    IN   GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(gtHgPcsRxLaneResetEnable)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with lane [0 / 5], 
                     enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call with out of range lane [6] 
                     and other parameters from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_U8       lane   = 0;
    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with lane [0 / 5], 
                                 enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* Call with lane = 0 */
            lane   = 0;
            enable = GT_FALSE;

            st = gtHgPcsRxLaneResetEnable(dev, fport, lane, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, lane, enable);

            /* Call with lane = 5 */
            lane   = 5;
            enable = GT_TRUE;

            st = gtHgPcsRxLaneResetEnable(dev, fport, lane, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fport, lane, enable);

            /*
                1.1.2. Call with out of range lane [6] 
                                 and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            lane = 6;

            st = gtHgPcsRxLaneResetEnable(dev, fport, lane, enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, lane);
        }

        lane   = 0;
        enable = GT_FALSE;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgPcsRxLaneResetEnable(dev, fport, lane, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgPcsRxLaneResetEnable(dev, fport, lane, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    lane   = 0;
    enable = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgPcsRxLaneResetEnable(dev, fport, lane, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgPcsRxLaneResetEnable(dev, fport, lane, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgGetPcsRxStatus
(
    IN   GT_U8     devNum,
    IN   GT_U8     fPort,
    OUT  GT_U8    *laneSyncUpBitmap,
    OUT  GT_BOOL  *alignmentLock
)
*/
UTF_TEST_CASE_MAC(gtHgGetPcsRxStatus)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with non-NULL laneSyncUpBitmap 
                     and non-NULL alignmentLock.
    Expected: GT_OK.
    1.1.2. Call with laneSyncUpBitmap [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.3. Call with alignmentLock [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_U8       laneSyncUpBitmap = 0;
    GT_BOOL     alignmentLock    = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL laneSyncUpBitmap 
                                 and non-NULL alignmentLock.
                Expected: GT_OK.
            */
            st = gtHgGetPcsRxStatus(dev, fport, &laneSyncUpBitmap, &alignmentLock);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

            /*
                1.1.2. Call with laneSyncUpBitmap [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtHgGetPcsRxStatus(dev, fport, NULL, &alignmentLock);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, laneSyncUpBitmap = NULL", dev, fport);

            /*
                1.1.3. Call with alignmentLock [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtHgGetPcsRxStatus(dev, fport, &laneSyncUpBitmap, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, alignmentLock = NULL", dev, fport);
        }

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgGetPcsRxStatus(dev, fport, &laneSyncUpBitmap, &alignmentLock);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgGetPcsRxStatus(dev, fport, &laneSyncUpBitmap, &alignmentLock);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgGetPcsRxStatus(dev, fport, &laneSyncUpBitmap, &alignmentLock);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgGetPcsRxStatus(dev, fport, &laneSyncUpBitmap, &alignmentLock);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgFlowControlCounters
(
    IN   GT_U8     devNum,
    IN   GT_U8     fPort,
    OUT  GT_U32   *rxFcCounter,
    OUT  GT_U32   *txFcCounter
)
*/
UTF_TEST_CASE_MAC(gtHgFlowControlCounters)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with non-NULL rxFcCounter 
                     and non-NULL txFcCounter.
    Expected: GT_OK.
    1.1.2. Call with rxFcCounter [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.3. Call with txFcCounter [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_U32      rxFcCounter = 0;
    GT_U32      txFcCounter = 0;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL rxFcCounter 
                                 and non-NULL txFcCounter.
                Expected: GT_OK.
            */
            st = gtHgFlowControlCounters(dev, fport, &rxFcCounter, &txFcCounter);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

            /*
                1.1.2. Call with rxFcCounter [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtHgFlowControlCounters(dev, fport, NULL, &txFcCounter);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, rxFcCounter = NULL", dev, fport);

            /*
                1.1.3. Call with txFcCounter [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtHgFlowControlCounters(dev, fport, &rxFcCounter, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, txFcCounter = NULL", dev, fport);
        }

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgFlowControlCounters(dev, fport, &rxFcCounter, &txFcCounter);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgFlowControlCounters(dev, fport, &rxFcCounter, &txFcCounter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgFlowControlCounters(dev, fport, &rxFcCounter, &txFcCounter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgFlowControlCounters(dev, fport, &rxFcCounter, &txFcCounter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgCellRxDropCounters
(
    IN   GT_U8     devNum,
    IN   GT_U8     fPort,
    OUT  GT_U8    *badCellHdrDropCount,
    OUT  GT_U8    *badCellLenDropCount,
    OUT  GT_U8    *pcsToMacErrorDropCount
)
*/
UTF_TEST_CASE_MAC(gtHgCellRxDropCounters)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with non-NULL badCellHdrDropCount, 
                     non-NULL badCellLenDropCount 
                     and non-NULL pcsToMacErrorDropCount.
    Expected: GT_OK.
    1.1.2. Call with badCellHdrDropCount [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.3. Call with badCellLenDropCount [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.4. Call with pcsToMacErrorDropCount [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_U8       badCellHdrDropCount    = 0;
    GT_U8       badCellLenDropCount    = 0;
    GT_U8       pcsToMacErrorDropCount = 0;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL badCellHdrDropCount, 
                                 non-NULL badCellLenDropCount 
                                 and non-NULL pcsToMacErrorDropCount.
                Expected: GT_OK.
            */
            st = gtHgCellRxDropCounters(dev, fport, &badCellHdrDropCount, &badCellLenDropCount, &pcsToMacErrorDropCount);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

            /*
                1.1.2. Call with badCellHdrDropCount [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtHgCellRxDropCounters(dev, fport, NULL, &badCellLenDropCount, &pcsToMacErrorDropCount);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, badCellHdrDropCount = NULL", dev, fport);

            /*
                1.1.3. Call with badCellLenDropCount [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtHgCellRxDropCounters(dev, fport, &badCellHdrDropCount, NULL, &pcsToMacErrorDropCount);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, badCellLenDropCount = NULL", dev, fport);

            /*
                1.1.4. Call with pcsToMacErrorDropCount [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtHgCellRxDropCounters(dev, fport, &badCellHdrDropCount, &badCellLenDropCount, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, pcsToMacErrorDropCount = NULL", dev, fport);
        }

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgCellRxDropCounters(dev, fport, &badCellHdrDropCount, &badCellLenDropCount, &pcsToMacErrorDropCount);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgCellRxDropCounters(dev, fport, &badCellHdrDropCount, &badCellLenDropCount, &pcsToMacErrorDropCount);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgCellRxDropCounters(dev, fport, &badCellHdrDropCount, &badCellLenDropCount, &pcsToMacErrorDropCount);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgCellRxDropCounters(dev, fport, &badCellHdrDropCount, &badCellLenDropCount, &pcsToMacErrorDropCount);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgSendPcsPing
(
    IN   GT_U8     devNum,
    IN   GT_U8     fPort,
    IN   GT_U8     data[3]
)
*/
UTF_TEST_CASE_MAC(gtHgSendPcsPing)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with data [0,0,0 / 255,255,255].
    Expected: GT_OK.
    1.1.2. Call gtHgRxPcsPingGet with non-NULL pingData.
    Expected: GT_OK and the same pingData.
    1.1.3. Call with data [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_U8       data[3];
    GT_U8       dataGet[3];
    GT_BOOL     isEqual = GT_FALSE;


    cpssOsBzero(data, sizeof(data[0])*3);
    cpssOsBzero(dataGet, sizeof(dataGet[0])*3);

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with data [0,0,0 / 255,255,255].
                Expected: GT_OK.
            */
            /* Call with data[0,0,0] */
            data[0] = 0;
            data[1] = 0;
            data[2] = 0;

            st = gtHgSendPcsPing(dev, fport, data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

            /*
                1.1.2. Call gtHgRxPcsPingGet with non-NULL pingData.
                Expected: GT_OK and the same pingData.
            */
            st = gtHgRxPcsPingGet(dev, fport, dataGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "gtHgRxPcsPingGet: %d, %d", dev, fport);

            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) data, (GT_VOID*) dataGet, sizeof(data[0])*3)) ? GT_TRUE : GT_FALSE; 
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "get another data then was set: %d, %d", dev, fport);

            /* Call with data[255,255,255] */
            data[0] = 255;
            data[1] = 255;
            data[2] = 255;

            st = gtHgSendPcsPing(dev, fport, data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

            /*
                1.1.2. Call gtHgRxPcsPingGet with non-NULL pingData.
                Expected: GT_OK and the same pingData.
            */
            st = gtHgRxPcsPingGet(dev, fport, dataGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "gtHgRxPcsPingGet: %d, %d", dev, fport);

            /* Verifying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) data, (GT_VOID*) dataGet, sizeof(data[0])*3)) ? GT_TRUE : GT_FALSE; 
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "get another data than was set: %d, %d", dev, fport);

            /*
                1.1.3. Call with data [NULL].
                Expected: GT_BAD_PTR.
            */
            st = gtHgSendPcsPing(dev, fport, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, data = NULL", dev, fport);
        }

        data[0] = 0;
        data[1] = 0;
        data[2] = 0;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgSendPcsPing(dev, fport, data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgSendPcsPing(dev, fport, data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    data[0] = 0;
    data[1] = 0;
    data[2] = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgSendPcsPing(dev, fport, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgSendPcsPing(dev, fport, data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgRxPcsPingGet
(
    IN  GT_U8           xbarId,
    IN  GT_U8           fport,
    OUT GT_U8           pingData[3]
)
*/
UTF_TEST_CASE_MAC(gtHgRxPcsPingGet)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with non-NULL pingData.
    Expected: GT_OK.
    1.1.2. Call with pingData [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_U8       pingData[3];


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL pingData.
                Expected: GT_OK.
            */
            st = gtHgRxPcsPingGet(dev, fport, pingData);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

            /*
                1.1.2. Call with pingData [NULL].
                Expected: GT_BAD_PTR.
            */
            st = gtHgRxPcsPingGet(dev, fport, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, pingData = NULL", dev, fport);
        }

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgRxPcsPingGet(dev, fport, pingData);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgRxPcsPingGet(dev, fport, pingData);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgRxPcsPingGet(dev, fport, pingData);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgRxPcsPingGet(dev, fport, pingData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgSerdesStatus
(
    IN   GT_U8     devNum,
    IN   GT_U8     fPort,
    OUT  GT_U8     *tbgLockedBitmap,
    OUT  GT_U8     *signalDetectBitmap
)
*/
UTF_TEST_CASE_MAC(gtHgSerdesStatus)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with non-NULL tbgLockedBitmap 
                     and non-NULL signalDetectBitmap.
    Expected: GT_OK.
    1.1.2. Call with tbgLockedBitmap [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.3. Call with signalDetectBitmap [NULL] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_U8       tbgLockedBitmap    = 0;
    GT_U8       signalDetectBitmap = 0;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL tbgLockedBitmap 
                                 and non-NULL signalDetectBitmap.
                Expected: GT_OK.
            */
            st = gtHgSerdesStatus(dev, fport, &tbgLockedBitmap, &signalDetectBitmap);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

            /*
                1.1.2. Call with tbgLockedBitmap [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtHgSerdesStatus(dev, fport, NULL, &signalDetectBitmap);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, tbgLockedBitmap = NULL", dev, fport);

            /*
                1.1.3. Call with signalDetectBitmap [NULL] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = gtHgSerdesStatus(dev, fport, &tbgLockedBitmap, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, signalDetectBitmap = NULL", dev, fport);
        }

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgSerdesStatus(dev, fport, &tbgLockedBitmap, &signalDetectBitmap);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgSerdesStatus(dev, fport, &tbgLockedBitmap, &signalDetectBitmap);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgSerdesStatus(dev, fport, &tbgLockedBitmap, &signalDetectBitmap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgSerdesStatus(dev, fport, &tbgLockedBitmap, &signalDetectBitmap);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgSerdesUnitShutdown
(
    IN   GT_U8     devNum,
    IN   GT_U8     fPort,
    IN   GT_BOOL   enable
)
*/
UTF_TEST_CASE_MAC(gtHgSerdesUnitShutdown)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_BOOL     enable = GT_FALSE;
    
    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* Call with enable = GT_FALSE */
            enable = GT_FALSE;

            st = gtHgSerdesUnitShutdown(dev, fport, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, enable);

            /* Call with enable = GT_TRUE */
            enable = GT_TRUE;

            st = gtHgSerdesUnitShutdown(dev, fport, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, enable);
        }

        enable = GT_FALSE;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgSerdesUnitShutdown(dev, fport, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgSerdesUnitShutdown(dev, fport, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    enable = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgSerdesUnitShutdown(dev, fport, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgSerdesUnitShutdown(dev, fport, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS gtHgSetSerdesPowerEn
(
    IN   GT_U8                devNum,
    IN   GT_U8                fPort,
    IN   GT_U8                laneBitMap,
    IN   GT_SERDES_POWER_TYPE powerType,
    IN   GT_BOOL              enable
)
*/
UTF_TEST_CASE_MAC(gtHgSetSerdesPowerEn)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with laneBitMap[0 / 0x3F], 
                     powerType [GT_SERDES_PLL_POWER / GT_SERDES_DRIVER_AND_RECEIVER_POWER],
                     enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call with out of range powerType [0x5AAAAAA5] 
                     and other parameters from 1.1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_U8                laneBitMap = 0;
    GT_SERDES_POWER_TYPE powerType  = GT_SERDES_PLL_POWER;
    GT_BOOL              enable     = GT_FALSE;
    
    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with laneBitMap[0 / 0x3F], 
                                 powerType [GT_SERDES_PLL_POWER / GT_SERDES_DRIVER_AND_RECEIVER_POWER],
                                 enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* Call with laneBitMap = 0 */
            laneBitMap = 0;
            powerType  = GT_SERDES_PLL_POWER;
            enable     = GT_FALSE;

            st = gtHgSetSerdesPowerEn(dev, fport, laneBitMap, powerType, enable);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, fport, laneBitMap, powerType, enable);
            
            /* Call with laneBitMap = 0x3F */
            laneBitMap = 0x3F;
            powerType  = GT_SERDES_DRIVER_AND_RECEIVER_POWER;
            enable     = GT_TRUE;

            st = gtHgSetSerdesPowerEn(dev, fport, laneBitMap, powerType, enable);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, fport, laneBitMap, powerType, enable);

            /*
                1.1.2. Call with out of range powerType [0x5AAAAAA5] 
                                 and other parameters from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            powerType = XBAR_HYPER_GLINK_INVALID_ENUM_CNS;

            st = gtHgSetSerdesPowerEn(dev, fport, laneBitMap, powerType, enable);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, powerType = %d", dev, fport, powerType);
        }

        laneBitMap = 0;
        powerType  = GT_SERDES_PLL_POWER;
        enable     = GT_FALSE;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgSetSerdesPowerEn(dev, fport, laneBitMap, powerType, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgSetSerdesPowerEn(dev, fport, laneBitMap, powerType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    laneBitMap = 0;
    powerType  = GT_SERDES_PLL_POWER;
    enable     = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgSetSerdesPowerEn(dev, fport, laneBitMap, powerType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgSetSerdesPowerEn(dev, fport, laneBitMap, powerType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS  gtHgFlowControlTCAware
(
    IN  GT_U8       devNum,     
    IN  GT_U8       fPort,
    IN  GT_BOOL     tcEnable
)
*/
UTF_TEST_CASE_MAC(gtHgFlowControlTCAware)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with tcEnable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call gtHgGetFlowControlTCAware with non-NULL tcEnablePtr.
    Expected: GT_OK and the same tcEnable.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_BOOL     tcEnable    = GT_FALSE;
    GT_BOOL     tcEnableGet = GT_FALSE;
    
    
    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with tcEnable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* Call with tcEnable = GT_FASLE */
            tcEnable = GT_FALSE;

            st = gtHgFlowControlTCAware(dev, fport, tcEnable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tcEnable);

            /*
                1.1.2. Call gtHgGetFlowControlTCAware with non-NULL tcEnablePtr.
                Expected: GT_OK and the same tcEnable.
            */
            st = gtHgGetFlowControlTCAware(dev, fport, &tcEnableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "gtHgGetFlowControlTCAware: %d, %d", dev, fport);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(tcEnable, tcEnableGet,
                       "get another tcEnable then was set: %d, %d", dev, fport);

            /* Call with tcEnable = GT_TRUE */
            tcEnable = GT_TRUE;

            st = gtHgFlowControlTCAware(dev, fport, tcEnable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, fport, tcEnable);

            /*
                1.1.2. Call gtHgGetFlowControlTCAware with non-NULL tcEnablePtr.
                Expected: GT_OK and the same tcEnable.
            */
            st = gtHgGetFlowControlTCAware(dev, fport,&tcEnableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "gtHgGetFlowControlTCAware: %d, %d", dev, fport);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(tcEnable, tcEnableGet,
                       "get another tcEnable then was set: %d, %d", dev, fport);
        }

        tcEnable = GT_FALSE;

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgFlowControlTCAware(dev, fport, tcEnable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgFlowControlTCAware(dev, fport, tcEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    tcEnable = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgFlowControlTCAware(dev, fport, tcEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgFlowControlTCAware(dev, fport, tcEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS  gtHgGetFlowControlTCAware
(
    IN  GT_U8       devNum,     
    IN  GT_U8       fPort,
    OUT GT_BOOL     *tcEnablePtr
)
*/
UTF_TEST_CASE_MAC(gtHgGetFlowControlTCAware)
{
/*
    ITERATE_XBAR_DEVICES_FA_PORTS
    1.1.1. Call with non-NULL tcEnable.
    Expected: GT_OK.
    1.1.2. Call with tcEnablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st    = GT_OK;    
    GT_U8       dev;
    GT_U8       fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    GT_BOOL     tcEnable = GT_FALSE;


    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_TRUE))
    {
        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL tcEnable.
                Expected: GT_OK.
            */
            st = gtHgGetFlowControlTCAware(dev, fport, &tcEnable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, fport);

            /*
                1.1.2. Call with tcEnablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = gtHgGetFlowControlTCAware(dev, fport, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, tcEnablePtr = NULL", dev, fport);
        }

        st = prvUtfNextXbarFaPortReset(&fport, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available fabric ports. */
        while (GT_OK == prvUtfNextXbarFaPortGet(&fport, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = gtHgGetFlowControlTCAware(dev, fport, &tcEnable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        fport = UTF_GET_MAX_FPORTS_NUM_MAC(dev);

        st = gtHgGetFlowControlTCAware(dev, fport, &tcEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, fport);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    fport = XBAR_HYPER_GLINK_VALID_FPORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextXbarReset(&dev, UTF_ALL_XBAR_TYPES_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextXbarGet(&dev, GT_FALSE))
    {
        st = gtHgGetFlowControlTCAware(dev, fport, &tcEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = gtHgGetFlowControlTCAware(dev, fport, &tcEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of gtHyperGlink suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(gtHyperGlink)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgSetInitParams)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgGetInitParams)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgPcsTxResetEnable)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgPcsRxLaneResetEnable)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgGetPcsRxStatus)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgFlowControlCounters)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgCellRxDropCounters)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgSendPcsPing)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgRxPcsPingGet)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgSerdesStatus)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgSerdesUnitShutdown)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgSetSerdesPowerEn)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgFlowControlTCAware)
    UTF_SUIT_DECLARE_TEST_MAC(gtHgGetFlowControlTCAware)
UTF_SUIT_END_TESTS_MAC(gtHyperGlink)

