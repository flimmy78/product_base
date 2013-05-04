 /*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCncUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChCnc, that provides
*       CPSS DxCh Centralized Counters (CNC) API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <appDemo/utils/appDemoFdbUpdateLock.h>

/* defines */

/* Default valid value for port id */
#define CNC_VALID_PHY_PORT_CNS  0

#define CNC_BLOCKS_NUM(_dev) \
    PRV_CPSS_DXCH_PP_MAC(_dev)->fineTuning.tableSize.cncBlocks

#define CNC_BLOCK_ENTRIES_NUM(_dev) \
    PRV_CPSS_DXCH_PP_MAC(_dev)->fineTuning.tableSize.cncBlockNumEntries

#define PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(_dev, _st, _normalSt)  \
    if ((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(_dev) == 0) ||                  \
      (PRV_CPSS_DXCH_PP_MAC(_dev)->errata.                                 \
       info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.                   \
        enabled == GT_TRUE))                                               \
    {                                                                      \
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_SUPPORTED, _st, _dev);          \
    }                                                                      \
    else                                                                   \
    {                                                                      \
        UTF_VERIFY_EQUAL1_PARAM_MAC(_normalSt, _st, _dev);                 \
    }

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncBlockClientEnableSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E],
                            updateEnable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call function cpssDxChCncBlockClientEnableGet with not NULL updateEnablePtr
                                                       and other params from 1.1.
    Expected: GT_OK and the same updateEnable as was set.
    1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                       and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call function with wrong enum values client and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    blockNum        = 0;
    CPSS_DXCH_CNC_CLIENT_ENT  client          = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    GT_BOOL                   updateEnable    = GT_FALSE;
    GT_BOOL                   updateEnableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                    client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                            CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E],
                                    updateEnable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with blockNum = 0 */
        blockNum     = 0;
        client       = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
        updateEnable = GT_FALSE;

        st = cpssDxChCncBlockClientEnableSet(dev, blockNum, client, updateEnable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, blockNum, client, updateEnable);

        /*
            1.2. Call function cpssDxChCncBlockClientEnableGet with not NULL updateEnablePtr
                                                               and other params from 1.1.
            Expected: GT_OK and the same updateEnable as was set.
        */
        st = cpssDxChCncBlockClientEnableGet(dev, blockNum, client, &updateEnableGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChCncBlockClientEnableGet: %d, %d, %d", dev, blockNum, client);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(updateEnable, updateEnableGet,
                   "got another updateEnable then was set: %d", dev);

        /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
        blockNum     = (CNC_BLOCKS_NUM(dev) - 1);
        client       = CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E;
        updateEnable = GT_TRUE;

        st = cpssDxChCncBlockClientEnableSet(dev, blockNum, client, updateEnable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, blockNum, client, updateEnable);

        /*
            1.2. Call function cpssDxChCncBlockClientEnableGet with not NULL updateEnablePtr
                                                               and other params from 1.1.
            Expected: GT_OK and the same updateEnable as was set.
        */
        st = cpssDxChCncBlockClientEnableGet(dev, blockNum, client, &updateEnableGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChCncBlockClientEnableGet: %d, %d, %d", dev, blockNum, client);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(updateEnable, updateEnableGet,
                   "got another updateEnable then was set: %d", dev);

        /*
            1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                               and other params from 1.1.
            Expected: NOT GT_OK.
        */
        blockNum = CNC_BLOCKS_NUM(dev);

        st = cpssDxChCncBlockClientEnableSet(dev, blockNum, client, updateEnable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

        blockNum = 0;

        /*
            1.4. Call function with wrong enum values client and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCncBlockClientEnableSet
                           (dev, blockNum, client, updateEnable),
                           client);
    }

    blockNum     = 0;
    client       = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    updateEnable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncBlockClientEnableSet(dev, blockNum, client, updateEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncBlockClientEnableSet(dev, blockNum, client, updateEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncBlockClientEnableGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E]
                            and not NULL updateEnablePtr.
    Expected: GT_OK.
    1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call function with wrong enum values client
                            and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call function with updateEnablePtr [NULL]
                            and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    blockNum     = 0;
    CPSS_DXCH_CNC_CLIENT_ENT  client       = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    GT_BOOL                   updateEnable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                    client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                            CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E]
                                    and not NULL updateEnablePtr.
            Expected: GT_OK.
        */
        /* iterate with blockNum = 0 */
        blockNum = 0;
        client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

        st = cpssDxChCncBlockClientEnableGet(dev, blockNum, client, &updateEnable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, client);

        /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
        blockNum = (CNC_BLOCKS_NUM(dev) - 1);
        client = CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E;

        st = cpssDxChCncBlockClientEnableGet(dev, blockNum, client, &updateEnable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, client);

        /*
            1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                    and other params from 1.1.
            Expected: NOT GT_OK.
        */
        blockNum = CNC_BLOCKS_NUM(dev);

        st = cpssDxChCncBlockClientEnableGet(dev, blockNum, client, &updateEnable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

        blockNum = 0;

        /*
            1.3. Call function with wrong enum values client
                                    and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCncBlockClientEnableGet
                           (dev, blockNum, client, &updateEnable),
                           client);

        /*
            1.4. Call function with updateEnablePtr [NULL]
                                    and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncBlockClientEnableGet(dev, blockNum, client, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, updateEnablePtr = NULL", dev);
    }

    blockNum = 0;
    client   = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncBlockClientEnableGet(dev, blockNum, client, &updateEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncBlockClientEnableGet(dev, blockNum, client, &updateEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U64                      indexRangesBmp
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncBlockClientRangesSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E],
                            indexRangesBmp [0 / 255].
    Expected: GT_OK.
    1.2. Call function cpssDxChCncBlockClientRangesGet with not NULL indexRangesBmpPtr
                                                            and other params from 1.1.
    Expected: GT_OK and the same indexRangesBmp as was set.
    1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call function with wrong enum values client
                            and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call function with out of range indexRangesBmp [256]
                            and other params from 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    blockNum          = 0;
    CPSS_DXCH_CNC_CLIENT_ENT  client            = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    GT_U64                    indexRangesBmp;
    GT_U64                    indexRangesBmpGet;

    indexRangesBmp.l[0]    = 0;
    indexRangesBmp.l[1]    = 0;
    indexRangesBmpGet.l[0] = 0;
    indexRangesBmpGet.l[1] = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                    client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                            CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E],
                                    indexRangesBmp [0 / 255].
            Expected: GT_OK.
        */
        /* iterate with blockNum = 0 */
        blockNum       = 0;
        client         = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
        indexRangesBmp.l[0] = 0;

        st = cpssDxChCncBlockClientRangesSet(dev, blockNum, client, indexRangesBmp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, blockNum, client, indexRangesBmp.l[0]);
        /*
            1.2. Call function cpssDxChCncBlockClientRangesGet with not NULL indexRangesBmpPtr
                                                                    and other params from 1.1.
            Expected: GT_OK and the same indexRangesBmp as was set.
        */
        st = cpssDxChCncBlockClientRangesGet(dev, blockNum, client, &indexRangesBmpGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChCncBlockClientRangesGet: %d, %d, %d", dev, blockNum, client);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(indexRangesBmp.l[0], indexRangesBmpGet.l[0],
                   "got another indexRangesBmp then was set: %d", dev);

        /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
        blockNum       = (CNC_BLOCKS_NUM(dev) - 1);
        client         = CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E;
        indexRangesBmp.l[0] = 255;

        st = cpssDxChCncBlockClientRangesSet(dev, blockNum, client, indexRangesBmp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, blockNum, client, indexRangesBmp.l[0]);

        /*
            1.2. Call function cpssDxChCncBlockClientRangesGet with not NULL indexRangesBmpPtr
                                                                    and other params from 1.1.
            Expected: GT_OK and the same indexRangesBmp as was set.
        */
        st = cpssDxChCncBlockClientRangesGet(dev, blockNum, client, &indexRangesBmpGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChCncBlockClientRangesGet: %d, %d, %d", dev, blockNum, client);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(indexRangesBmp.l[0], indexRangesBmpGet.l[0],
                   "got another indexRangesBmp then was set: %d", dev);

        /*
            1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                    and other params from 1.1.
            Expected: NOT GT_OK.
        */
        blockNum = CNC_BLOCKS_NUM(dev);

        st = cpssDxChCncBlockClientRangesSet(dev, blockNum, client, indexRangesBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

        blockNum = 0;

        /*
            1.4. Call function with wrong enum values client
                                    and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCncBlockClientRangesSet
                            (dev, blockNum, client, indexRangesBmp),
                            client);

        /*
            1.5. Call function with out of range indexRangesBmp [256]
                                    and other params from 1.1.
            Expected: NOT GT_OK.
        */
        indexRangesBmp.l[0] = 256;

        st = cpssDxChCncBlockClientRangesSet(
            dev, blockNum, client, indexRangesBmp);

        if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(dev))
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(
                GT_OK, st, dev, indexRangesBmp.l[0]);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(
                GT_OK, st, dev, indexRangesBmp.l[0]);
        }
    }

    blockNum       = 0;
    client         = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    indexRangesBmp.l[0] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncBlockClientRangesSet(dev, blockNum, client, indexRangesBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncBlockClientRangesSet(dev, blockNum, client, indexRangesBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncBlockClientRangesGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_U32                    *indexRangesBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncBlockClientRangesGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E]
                            and not NULL indexRangesBmpPtr.
    Expected: GT_OK.
    1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call function with wrong enum values client
                            and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call function with indexRangesBmpPtr [NULL]
                            and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    blockNum       = 0;
    CPSS_DXCH_CNC_CLIENT_ENT  client         = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    GT_U64                    indexRangesBmp;

    indexRangesBmp.l[0]    = 0;
    indexRangesBmp.l[1]    = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                    client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                            CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E]
                                    and not NULL indexRangesBmpPtr.
            Expected: GT_OK.
        */
        /* iterate with blockNum = 0 */
        blockNum = 0;
        client   = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

        st = cpssDxChCncBlockClientRangesGet(dev, blockNum, client, &indexRangesBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, client);

        /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
        blockNum = (CNC_BLOCKS_NUM(dev) - 1);
        client   = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

        st = cpssDxChCncBlockClientRangesGet(dev, blockNum, client, &indexRangesBmp);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, client);

        /*
            1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                    and other params from 1.1.
            Expected: NOT GT_OK
        */
        blockNum = CNC_BLOCKS_NUM(dev);

        st = cpssDxChCncBlockClientRangesGet(dev, blockNum, client, &indexRangesBmp);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

        blockNum = 0;

        /*
            1.3. Call function with wrong enum values client
                                    and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCncBlockClientRangesGet
                            (dev, blockNum, client, &indexRangesBmp),
                            client);

        /*
            1.4. Call function with indexRangesBmpPtr [NULL]
                                    and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncBlockClientRangesGet(dev, blockNum, client, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, indexRangesBmpPtr = NULL", dev);
    }

    blockNum = 0;
    client   = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncBlockClientRangesGet(dev, blockNum, client, &indexRangesBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncBlockClientRangesGet(dev, blockNum, client, &indexRangesBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_U8                     portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortClientEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh3 and above)
    1.1.1. Call function with client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E]
                              and enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call function cpssDxChCncPortClientEnableGet with not NULL enablePtr
                                                        and other params from 1.1.1.
    Expected: GT_OK and the same enable.
    1.1.3. Call function with client [CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E] (only L2/L3 Ingress Vlan client supported)
                              and other params from 1.1.
    Expected: NOT GT_OK.
    1.1.4. Call function with wrong enum values client
                              and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CNC_VALID_PHY_PORT_CNS;

    CPSS_DXCH_CNC_CLIENT_ENT  client    = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    GT_BOOL                   enable    = GT_FALSE;
    GT_BOOL                   enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E]
                                          and enable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
            /* iterate with enable = GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxChCncPortClientEnableSet(dev, port, client, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, client, enable);

            /*
                1.1.2. Call function cpssDxChCncPortClientEnableGet with not NULL enablePtr
                                                                    and other params from 1.1.1.
                Expected: GT_OK and the same enable.
            */
            st = cpssDxChCncPortClientEnableGet(dev, port, client, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChCncPortClientEnableGet: %d, %d, %d", dev, port, client);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "got another enable then was set: %d, %d", dev, port);

            /* iterate with enable = GT_TRUE */
            enable = GT_TRUE;

            st = cpssDxChCncPortClientEnableSet(dev, port, client, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, client, enable);

            /*
                1.1.2. Call function cpssDxChCncPortClientEnableGet with not NULL enablePtr
                                                                    and other params from 1.1.1.
                Expected: GT_OK and the same enable.
            */
            st = cpssDxChCncPortClientEnableGet(dev, port, client, &enableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChCncPortClientEnableGet: %d, %d, %d", dev, port, client);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "got another enable then was set: %d, %d", dev, port);

            /*
                1.1.3. Call function with client [CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E] (only L2/L3 Ingress Vlan client supported)
                                          and other params from 1.1.
                Expected: NOT GT_OK.
            */
            client = CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E;

            st = cpssDxChCncPortClientEnableSet(dev, port, client, enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, client);

            /*
                1.1.4. Call function with wrong enum values client and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCncPortClientEnableSet
                                (dev, port, client, enable),
                                client);
        }

        client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCncPortClientEnableSet(dev, port, client, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCncPortClientEnableSet(dev, port, client, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCncPortClientEnableSet(dev, port, client, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    port = CNC_VALID_PHY_PORT_CNS;
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortClientEnableSet(dev, port, client, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortClientEnableSet(dev, port, client, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_U8                     portNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortClientEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh3 and above)
    1.1.1. Call function with client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E]
                              and not NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call function with client [CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E] (only L2/L3 Ingress Vlan client supported)
                              and other params from 1.1.
    Expected: NOT GT_OK.
    1.1.3. Call function with wrong enum values client
                              and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call function with enablePtr [NULL]
                              and other params from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CNC_VALID_PHY_PORT_CNS;

    CPSS_DXCH_CNC_CLIENT_ENT  client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    GT_BOOL                   enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E]
                                          and not NULL enablePtr.
                Expected: GT_OK.
            */
            client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

            st = cpssDxChCncPortClientEnableGet(dev, port, client, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, client);

            /*
                1.1.2. Call function with client [CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E] (only L2/L3 Ingress Vlan client supported)
                                          and other params from 1.1.
                Expected: NOT GT_OK.
            */
            client = CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E;

            st = cpssDxChCncPortClientEnableGet(dev, port, client, &enable);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, client);

            /*
                1.1.3. Call function with wrong enum values client
                                          and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCncPortClientEnableGet
                                (dev, port, client, &enable),
                                client);

            /*
                1.1.4. Call function with enablePtr [NULL]
                                          and other params from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCncPortClientEnableGet(dev, port, client, NULL);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCncPortClientEnableGet(dev, port, client, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCncPortClientEnableGet(dev, port, client, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCncPortClientEnableGet(dev, port, client, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    port = CNC_VALID_PHY_PORT_CNS;

    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortClientEnableGet(dev, port, client, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortClientEnableGet(dev, port, client, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call function cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
                       with not NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL enable    = GT_FALSE;
    GT_BOOL enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
                               with not NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
                               with not NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with not NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncClientByteCountModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    IN  CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT countMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncClientByteCountModeSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E],
                            countMode [CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E /
                                       CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E].
    Expected: GT_OK.
    1.2. Call function cpssDxChCncClientByteCountModeGet with not NULL countModePtr
                                                         and other params from 1.1.
    Expected: GT_OK and the same countMode as was set.
    1.3. Call function with wrong enum values client
                            and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call function with wrong enum values countMode
                            and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CNC_CLIENT_ENT          client       = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT countMode    = CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E;
    CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT countModeGet = CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                            CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E],
                                    countMode [CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E /
                                               CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E].
            Expected: GT_OK.
        */
        /* iterate with client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E */
        client    = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
        countMode = CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E;

        st = cpssDxChCncClientByteCountModeSet(dev, client, countMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, client, countMode);

        /*
            1.2. Call function cpssDxChCncClientByteCountModeGet with not NULL countModePtr
                                                                 and other params from 1.1.
            Expected: GT_OK and the same countMode as was set.
        */
        st = cpssDxChCncClientByteCountModeGet(dev, client, &countModeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChCncClientByteCountModeGet: %d, %d", dev, client);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(countMode, countModeGet,
                   "got another countMode then was set: %d", dev);

        /* iterate with client = CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E */
        client    = CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E;
        countMode = CPSS_DXCH_CNC_BYTE_COUNT_MODE_L3_E;

        st = cpssDxChCncClientByteCountModeSet(dev, client, countMode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, client, countMode);

        /*
            1.2. Call function cpssDxChCncClientByteCountModeGet with not NULL countModePtr
                                                                 and other params from 1.1.
            Expected: GT_OK and the same countMode as was set.
        */
        st = cpssDxChCncClientByteCountModeGet(dev, client, &countModeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChCncClientByteCountModeGet: %d, %d", dev, client);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(countMode, countModeGet,
                   "got another countMode then was set: %d", dev);

        /*
            1.3. Call function with wrong enum values client
                                    and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCncClientByteCountModeSet
                            (dev, client, countMode),
                            client);

        /*
            1.4. Call function with wrong enum values countMode
                                    and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCncClientByteCountModeSet
                            (dev, client, countMode),
                            countMode);
    }

    client    = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    countMode = CPSS_DXCH_CNC_BYTE_COUNT_MODE_L2_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncClientByteCountModeSet(dev, client, countMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncClientByteCountModeSet(dev, client, countMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncClientByteCountModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT          client,
    OUT CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT *countModePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncClientByteCountModeGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E]
                            and not NULL countModePtr.
    Expected: GT_OK.
    1.2. Call function with wrong enum values client
                            and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call function with countModePtr [NULL]
                            and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CNC_CLIENT_ENT          client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT countMode;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                            CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E]
                                    and not NULL countModePtr.
            Expected: GT_OK.
        */
        /* iterate with client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E */
        client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

        st = cpssDxChCncClientByteCountModeGet(dev, client, &countMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, client);

        /* iterate with client = CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E */
        client = CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E;

        st = cpssDxChCncClientByteCountModeGet(dev, client, &countMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, client);

        /*
            1.2. Call function with wrong enum values client and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCncClientByteCountModeGet
                            (dev, client, &countMode),
                            client);

        /*
            1.2. Call function with countModePtr [NULL]
                                    and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncClientByteCountModeGet(dev, client, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, countModePtr = NULL", dev);
    }

    client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncClientByteCountModeGet(dev, client, &countMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncClientByteCountModeGet(dev, client, &countMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncEgressVlanDropCountModeSet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncEgressVlanDropCountModeSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with mode [CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E /
                                  CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E].
    Expected: GT_OK.
    1.2. Call function cpssDxChCncEgressVlanDropCountModeGet with not NULL modePtr.
    Expected: GT_OK and the same mode as was set.
    1.3. Call function with wrong enum values mode .
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT mode    = CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E;
    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT modeGet = CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E /
                                          CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E].
            Expected: GT_OK.
        */
        /* iterate with mode = CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E */
        mode = CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E;

        st = cpssDxChCncEgressVlanDropCountModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call function cpssDxChCncEgressVlanDropCountModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssDxChCncEgressVlanDropCountModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncEgressVlanDropCountModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "got another mode then was set: %d", dev);

        /* iterate with mode = CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E */
        mode = CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_TAIL_DROP_ONLY_E;

        st = cpssDxChCncEgressVlanDropCountModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call function cpssDxChCncEgressVlanDropCountModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssDxChCncEgressVlanDropCountModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncEgressVlanDropCountModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "got another mode then was set: %d", dev);

        /*
            1.3. Call function with wrong enum values mode .
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCncEgressVlanDropCountModeSet
                            (dev, mode),
                            mode);
    }

    mode = CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncEgressVlanDropCountModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncEgressVlanDropCountModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncEgressVlanDropCountModeGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncEgressVlanDropCountModeGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with not NULL modePtr.
    Expected: GT_OK.
    1.2. Call function with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT mode = CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_EGRESS_FILTER_AND_TAIL_DROP_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with not NULL modePtr.
            Expected: GT_OK.
        */
        st = cpssDxChCncEgressVlanDropCountModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncEgressVlanDropCountModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncEgressVlanDropCountModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncEgressVlanDropCountModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCounterClearByReadEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCounterClearByReadEnableSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call function cpssDxChCncCounterClearByReadEnableGet with not NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssDxChCncCounterClearByReadEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChCncCounterClearByReadEnableGet with not NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChCncCounterClearByReadEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCounterClearByReadEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChCncCounterClearByReadEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChCncCounterClearByReadEnableGet with not NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChCncCounterClearByReadEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCounterClearByReadEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCounterClearByReadEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCounterClearByReadEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCounterClearByReadEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCounterClearByReadEnableGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with not NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChCncCounterClearByReadEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncCounterClearByReadEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCounterClearByReadEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCounterClearByReadEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCounterClearByReadValueSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_CNC_COUNTER_STC        *counterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCounterClearByReadValueSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with counterPtr { byteCount [0 / 0x0007 FFFF FFFF],
                                         packetCount [0 / 0x1FFF FFFF] }.
    Expected: GT_OK.
    1.2. Call function cpssDxChCncCounterClearByReadValueGet with not NULL counterGet.
    Expected: GT_OK and the same counter as was set.
    1.3. Call function with counterPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CNC_COUNTER_STC counter;
    CPSS_DXCH_CNC_COUNTER_STC counterGet;
    GT_BOOL                   isEqual = GT_FALSE;


    cpssOsBzero((GT_VOID*) &counter, sizeof(counter));
    cpssOsBzero((GT_VOID*) &counterGet, sizeof(counterGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with counterPtr { byteCount [0 / 0x0007 FFFF FFFF],
                                                 packetCount [0 / 0x1FFF FFFF] }.
            Expected: GT_OK.
        */
        /* iterate with counter.byteCount = 0 */
        counter.byteCount.l[0] = 0x0;
        counter.byteCount.l[1] = 0x0;
        counter.packetCount.l[0] = 0x0;
        counter.packetCount.l[1] = 0x0;

        st = cpssDxChCncCounterClearByReadValueSet(
            dev, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function cpssDxChCncCounterClearByReadValueGet with not NULL counterGet.
            Expected: GT_OK and the same counter as was set.
        */
        st = cpssDxChCncCounterClearByReadValueGet(
            dev, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counterGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCounterClearByReadValueGet: %d", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &counter,
                                     (GT_VOID*) &counterGet,
                                     sizeof(counter))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another counter then was set: %d", dev);

        /* iterate with counter.byteCount = 0x0007 FFFF FFFF */
        counter.byteCount.l[0] = 0xFFFFFFFF;
        counter.byteCount.l[1] = 0x07;
        counter.packetCount.l[0]    = 0x1FFFFFFF;
        counter.packetCount.l[1]    = 0;

        st = cpssDxChCncCounterClearByReadValueSet(
            dev, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function cpssDxChCncCounterClearByReadValueGet with not NULL counterGet.
            Expected: GT_OK and the same counter as was set.
        */
        st = cpssDxChCncCounterClearByReadValueGet(
            dev, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counterGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCounterClearByReadValueGet: %d", dev);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &counter,
                                     (GT_VOID*) &counterGet,
                                     sizeof(counter))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got another counter then was set: %d", dev);

        /*
            1.3. Call function with counterPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncCounterClearByReadValueGet(
            dev, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterPtr = NULL", dev);
    }

    counter.byteCount.l[0] = 0x0;
    counter.byteCount.l[1] = 0x0;
    counter.packetCount.l[0] = 0;
    counter.packetCount.l[1] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCounterClearByReadValueSet(
            dev, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCounterClearByReadValueSet(
        dev, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCounterClearByReadValueGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_CNC_COUNTER_STC       *counterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCounterClearByReadValueGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with not NULL counterPtr.
    Expected: GT_OK.
    1.2. Call function with counterPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CNC_COUNTER_STC counter;


    cpssOsBzero((GT_VOID*) &counter, sizeof(counter));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with not NULL counterPtr.
            Expected: GT_OK.
        */
        st = cpssDxChCncCounterClearByReadValueGet(
            dev, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with counterPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncCounterClearByReadValueGet(
            dev, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCounterClearByReadValueGet(
            dev, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCounterClearByReadValueGet(
        dev, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCounterWraparoundEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCounterWraparoundEnableSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call function cpssDxChCncCounterWraparoundEnableGet with not NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL enable    = GT_FALSE;
    GT_BOOL enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */
        /* iterate with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssDxChCncCounterWraparoundEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChCncCounterWraparoundEnableGet with not NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChCncCounterWraparoundEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCounterWraparoundEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChCncCounterWraparoundEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChCncCounterWraparoundEnableGet with not NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChCncCounterWraparoundEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCounterWraparoundEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCounterWraparoundEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCounterWraparoundEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCounterWraparoundEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCounterWraparoundEnableGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with not NULL enablePtr.
    Expected: GT_OK.
    1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with not NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChCncCounterWraparoundEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncCounterWraparoundEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCounterWraparoundEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCounterWraparoundEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCounterWraparoundIndexesGet
(
    IN    GT_U8    devNum,
    IN    GT_U32   blockNum,
    INOUT GT_U32   *indexNumPtr,
    OUT   GT_U32   indexesArr[]
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCounterWraparoundIndexesGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            indexNumPtr [0 / 5]
                            and not NULL indexesArr.
    Expected GT_OK.
    1.2. Call function with out of range blockNum [(CNC_BLOCKS_NUM - 1)]
                            and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call function with indexNumPtr [NULL]
                            and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call function with indexesArr [NULL]
                        and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32   blockNum  = 0;
    GT_U32   indexNum  = 0;
    GT_U32   indexesArr[255];


    cpssOsBzero((GT_VOID*) indexesArr, sizeof(indexesArr[0])*255);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                    indexNumPtr [0 / 5]
                                    and not NULL indexesArr.
            Expected GT_OK.
        */
        /* iterate with blockNum = 0 */
        blockNum = 0;
        indexNum = 0;

        st = cpssDxChCncCounterWraparoundIndexesGet(dev, blockNum, &indexNum, indexesArr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, indexNum);

        /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
        blockNum = (CNC_BLOCKS_NUM(dev) - 1);
        indexNum = 5;

        st = cpssDxChCncCounterWraparoundIndexesGet(dev, blockNum, &indexNum, indexesArr);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, indexNum);

        /*
            1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                    and other params from 1.1.
            Expected: NOT GT_OK.
        */
        blockNum = CNC_BLOCKS_NUM(dev);

        st = cpssDxChCncCounterWraparoundIndexesGet(dev, blockNum, &indexNum, indexesArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

        blockNum = 0;

        /*
            1.3. Call function with indexNumPtr [NULL]
                                    and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncCounterWraparoundIndexesGet(dev, blockNum, NULL, indexesArr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, indexNumPtr = NULL", dev);

        /*
            1.4. Call function with indexesArr [NULL]
                                    and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncCounterWraparoundIndexesGet(dev, blockNum, &indexNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, indexesArr = NULL", dev);
    }

    blockNum = 0;
    indexNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCounterWraparoundIndexesGet(dev, blockNum, &indexNum, indexesArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCounterWraparoundIndexesGet(dev, blockNum, &indexNum, indexesArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCounterSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          blockNum,
    IN  GT_U32                          index,
    IN  CPSS_DXCH_CNC_COUNTER_STC       *counterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCounterSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            index [0 / 2047],
                            counterPtr { byteCount [0 / 0x0007 FFFF FFFF],
                                         packetCount [0 / 0x1FFF FFFF] }.
    Expected: GT_OK.
    1.2. Call function cpssDxChCncCounterGet with not NULL counterPtr
                                             and other params from 1.1.
    Expected: GT_OK and the same counter as was set.
    1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call function with out of range index [2048]
                            and other params from 1.1.
    Expected: NOT GT_OK.
    1.5. Call function with counterPtr [NULL]
                            and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    blockNum = 0;
    GT_U32                    index    = 0;
    CPSS_DXCH_CNC_COUNTER_STC counter;
    CPSS_DXCH_CNC_COUNTER_STC counterGet;
    GT_BOOL                   isEqual  = GT_FALSE;


    cpssOsBzero((GT_VOID*) &counter, sizeof(counter));
    cpssOsBzero((GT_VOID*) &counterGet, sizeof(counterGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                    index [0 / 2047],
                                    counterPtr { byteCount [0 / 0x0007 FFFF FFFF],
                                                 packetCount [0 / 0x1FFF FFFF] }.
            Expected: GT_OK.
        */
        /* iterate with blockNum = 0 */
        blockNum = 0;
        index    = 0;

        counter.byteCount.l[0] = 0x0;
        counter.byteCount.l[1] = 0x0;
        counter.packetCount.l[0]    = 0;
        counter.packetCount.l[1]    = 0;

        st = cpssDxChCncCounterSet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, index);

        /*
            1.2. Call function cpssDxChCncCounterGet with not NULL counterPtr
                                                     and other params from 1.1.
            Expected: GT_OK and the same counter as was set.
        */
        st = cpssDxChCncCounterGet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counterGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCounterGet: %d, %d, &d", dev, blockNum, index);

        /* Veryfying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &counter,
                                     (GT_VOID*) &counterGet,
                                     sizeof(counter))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got  another counter then was set: %d", dev);

        /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
        blockNum = (CNC_BLOCKS_NUM(dev) - 1);
        index    = CNC_BLOCK_ENTRIES_NUM(dev) - 1;

        counter.byteCount.l[0] = 0xFFFFFFFF;
        counter.byteCount.l[1] = 0x07;
        counter.packetCount.l[0]    = 0x1FFFFFFF;
        counter.packetCount.l[1]    = 0;

        st = cpssDxChCncCounterSet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, index);

        /*
            1.2. Call function cpssDxChCncCounterGet with not NULL counterPtr
                                                     and other params from 1.1.
            Expected: GT_OK and the same counter as was set.
        */
        st = cpssDxChCncCounterGet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counterGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCounterGet: %d, %d, &d", dev, blockNum, index);

        /* Veryfying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &counter,
                                     (GT_VOID*) &counterGet,
                                     sizeof(counter))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "got  another counter then was set: %d", dev);

        /*
            1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                    and other params from 1.1.
            Expected: NOT GT_OK.
        */
        blockNum = CNC_BLOCKS_NUM(dev);

        st = cpssDxChCncCounterSet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

        blockNum = 0;

        /*
            1.4. Call function with out of range index [2048]
                                    and other params from 1.1.
            Expected: NOT GT_OK.
        */
        index = CNC_BLOCK_ENTRIES_NUM(dev);

        st = cpssDxChCncCounterSet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, index = %d", dev, index);

        index = 0;

        /*
            1.5. Call function with counterPtr [NULL]
                                    and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncCounterSet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterPtr = NULL", dev);
    }

    blockNum = 0;
    index    = 0;

    counter.byteCount.l[0] = 0x00;
    counter.byteCount.l[1] = 0x00;
    counter.packetCount.l[0]    = 0;
    counter.packetCount.l[1]    = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCounterSet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCounterSet(
        dev, blockNum, index,
        CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCounterGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          blockNum,
    IN  GT_U32                          index,
    OUT CPSS_DXCH_CNC_COUNTER_STC       *counterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCounterGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            index [0 / 2047]
                            and not NULL counterPtr.
    Expected: GT_OK.
    1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call function with out of range index [2048]
                            and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call function with counterPtr [NULL]
                            and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      blockNum = 0;
    GT_U32                      index    = 0;
    CPSS_DXCH_CNC_COUNTER_STC   counter;


    cpssOsBzero((GT_VOID*) &counter, sizeof(counter));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                    index [0 / 2047]
                                    and not NULL counterPtr.
            Expected: GT_OK.
        */
        /* iterate with blockNum = 0 */
        blockNum = 0;
        index    = 0;

        st = cpssDxChCncCounterGet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, index);

        /* iterate with blockNum = 0 */
        blockNum = (CNC_BLOCKS_NUM(dev) - 1);
        index    = CNC_BLOCK_ENTRIES_NUM(dev) - 1;

        st = cpssDxChCncCounterGet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, index);

        /*
            1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                    and other params from 1.1.
            Expected: NOT GT_OK.
        */
        blockNum = CNC_BLOCKS_NUM(dev);

        st = cpssDxChCncCounterGet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

        blockNum = 0;

        /*
            1.3. Call function with out of range index [2048]
                                    and other params from 1.1.
            Expected: NOT GT_OK.
        */
        index = CNC_BLOCK_ENTRIES_NUM(dev);

        st = cpssDxChCncCounterGet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, index = %d", dev, index);

        index = 0;

        /*
            1.4. Call function with counterPtr [NULL]
                                    and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncCounterGet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterPtr = NULL", dev);
    }

    blockNum = 0;
    index    = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCounterGet(
            dev, blockNum, index,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCounterGet(
        dev, blockNum, index,
        CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

static GT_VOID clearMemoAfterBlockUploadTrigger
(
    IN GT_U8  dev,
    IN GT_U32 portGroupsBmp
)
{
    GT_STATUS   st = GT_OK;
    GT_STATUS   blockGetSt = GT_OK;
    GT_U32      uploadInProcess; /* block bitmask */
    GT_U32      waitCount;
    GT_U32      numOfMsgs;
    CPSS_MAC_UPDATE_MSG_EXT_STC fuMessage;
    CPSS_DXCH_CNC_COUNTER_STC   counterValues;

    /* The chunk is a part of que from upload beginning to the    */
    /* end of uploaded data of to the end of the queue if reached.*/
    /* Wait for upload completion, but not more then 100 chunks.  */
    uploadInProcess = 0;
    for (waitCount = 0; (waitCount < 100); waitCount++)
    {
        st = cpssDxChCncPortGroupBlockUploadInProcessGet(
            dev, portGroupsBmp, &uploadInProcess);
        PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(dev, st, GT_OK);

        if (st != GT_OK)
        {
            /* for devices that not support upload */
            return;
        }

        if (uploadInProcess != 0)
        {
            /* wait 100 millisec => enough for chunk upload */
            cpssOsTimerWkAfter(100);
        }

        /* clear the CNC counter messages queue */
        while (1)
        {
            numOfMsgs = 1;
            blockGetSt = cpssDxChCncPortGroupUploadedBlockGet(
                dev, portGroupsBmp, &numOfMsgs,
                CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, &counterValues);
            if (blockGetSt == GT_NO_MORE)
            {
                break;
            }

            PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(
                dev, blockGetSt, GT_OK);
        }

        if ((blockGetSt == GT_NO_MORE) && (uploadInProcess == 0))
        {
            /* the cleared state */
            break;
        }
    }

    UTF_VERIFY_EQUAL2_PARAM_MAC(0, uploadInProcess, dev, uploadInProcess);

    /* clear the FU message queue */
    do
    {
        numOfMsgs = 1;
        APPDEMO_FDB_UPDATE_LOCK();
        st = cpssDxChBrgFdbFuMsgBlockGet(dev, &numOfMsgs, &fuMessage);
        APPDEMO_FDB_UPDATE_UNLOCK();
        if( st != GT_OK )
        {
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_NO_MORE, st, dev, numOfMsgs);
        }
    }
    while( st == GT_OK );

}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncBlockUploadTrigger)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)].
    Expected: GT_OK.
    1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32      blockNum = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)].
            Expected: GT_OK.
        */

        /* clear the FU message queue */
        clearMemoAfterBlockUploadTrigger(
            dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS);

        /* iterate with blockNum = 0 */
        blockNum = 0;

        st = cpssDxChCncBlockUploadTrigger(dev, blockNum);
        PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(dev, st, GT_OK);

        cpssOsTimerWkAfter(1);

        /* clear the FU message queue */
        clearMemoAfterBlockUploadTrigger(
            dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS);

        /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
        blockNum = (CNC_BLOCKS_NUM(dev) - 1);

        st = cpssDxChCncBlockUploadTrigger(dev, blockNum);
        PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(dev, st, GT_OK);

        cpssOsTimerWkAfter(1);

        /* clear the FU message queue */
        clearMemoAfterBlockUploadTrigger(
            dev, CPSS_PORT_GROUP_UNAWARE_MODE_CNS);

        /*
            1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM].
            Expected: NOT GT_OK.
        */
        blockNum = CNC_BLOCKS_NUM(dev);

        st = cpssDxChCncBlockUploadTrigger(dev, blockNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

    }

    blockNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncBlockUploadTrigger(dev, blockNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncBlockUploadTrigger(dev, blockNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                     *inProcessBlocksBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncBlockUploadInProcessGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with not NULL inProcessBlocksBmpPtr.
    Expected: GT_OK.
    1.2. Call function with inProcessBlocksBmpPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32      inProcessBlocksBmp = 0;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with not NULL inProcessBlocksBmpPtr.
            Expected: GT_OK.
        */
        st = cpssDxChCncBlockUploadInProcessGet(dev, &inProcessBlocksBmp);
        PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(dev, st, GT_OK);

        /*
            1.2. Call function with inProcessBlocksBmpPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncBlockUploadInProcessGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, inProcessBlocksBmpPtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncBlockUploadInProcessGet(dev, &inProcessBlocksBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncBlockUploadInProcessGet(dev, &inProcessBlocksBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncUploadedBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfCounterValuesPtr,
    OUT    CPSS_DXCH_CNC_COUNTER_STC   *counterValuesPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncUploadedBlockGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call function with numOfCounterValuesPtr [0 / 5]
                            and not NULL counterValuesPtr.
    Expected: GT_OK.
    1.2. Call function with numOfCounterValuesPtr [NULL]
                            and other param from 1.1.
    Expected: GT_BAD_PTR.
    1.3. Call function with counterValuesPtr [NULL]
                            and other param from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      numOfCounterValues = 0;
    CPSS_DXCH_CNC_COUNTER_STC   counterValues[10];

    cpssOsBzero((GT_VOID*) counterValues, sizeof(counterValues));


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with numOfCounterValuesPtr [0]
                                    and not NULL counterValuesPtr.
            Expected: GT_OK.
        */

        /* Call with numOfCounterValues [0] */
        numOfCounterValues = 0;

        st = cpssDxChCncUploadedBlockGet(
            dev, &numOfCounterValues,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, counterValues);
        if (GT_NO_MORE != st)
        {
            PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(dev, st, GT_OK);
        }

        /* Call with numOfCounterValues [5] */
        numOfCounterValues = 5;

        st = cpssDxChCncUploadedBlockGet(
            dev, &numOfCounterValues,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, counterValues);
        if (GT_NO_MORE != st)
        {
            PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(dev, st, GT_OK);
        }

        /*
            1.2. Call function with numOfCounterValuesPtr [NULL]
                                    and other param from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncUploadedBlockGet(
            dev, NULL, CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, counterValues);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfCounterValuesPtr = NULL", dev);

        /*
            1.3. Call function with counterValuesPtr [NULL]
                                    and other param from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncUploadedBlockGet(
            dev, &numOfCounterValues,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterValuesPtr = NULL", dev);
    }

    numOfCounterValues = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncUploadedBlockGet(
            dev, &numOfCounterValues,
            CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, counterValues);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncUploadedBlockGet(
        dev, &numOfCounterValues,
        CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E, counterValues);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCountingEnableSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    IN  GT_BOOL                                 enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCountingEnableSet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with cncUnit [CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E /
                            CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E]
                and enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.2. Call function cpssDxChCncCountingEnableGet
                       with not NULL enablePtr.
    Expected: GT_OK and the same enable as was set.
    1.3. Call  with wrong enum values cncUnit .
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit = 0;
    GT_BOOL enable    = GT_TRUE;
    GT_BOOL enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cncUnit [CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E /
                                    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E]
                        and enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* iterate with enable [GT_FALSE],
        cncUnit [CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E] */

        enable = GT_FALSE;
        cncUnit = CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E;

        st = cpssDxChCncCountingEnableSet(dev, cncUnit, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChCncCountingEnableGet
                               with not NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChCncCountingEnableGet(
            dev, cncUnit, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCountingEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChCncCountingEnableSet(dev, cncUnit, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* iterate with enable [GT_FALSE],
        cncUnit [CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E] */

        enable = GT_FALSE;
        cncUnit = CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E;

        st = cpssDxChCncCountingEnableSet(dev, cncUnit, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChCncCountingEnableGet
                               with not NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChCncCountingEnableGet(dev, cncUnit, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCountingEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);


        /* iterate with enable [GT_TRUE],
        cncUnit [CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E] */

        enable = GT_TRUE;
        cncUnit = CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E;

        st = cpssDxChCncCountingEnableSet(dev, cncUnit, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChCncCountingEnableGet
                               with not NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChCncCountingEnableGet(dev, cncUnit, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCountingEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);

        /* iterate with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxChCncCountingEnableSet(dev, cncUnit, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* iterate with enable [GT_TRUE],
        cncUnit [CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E] */

        enable = GT_TRUE;
        cncUnit = CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E;

        st = cpssDxChCncCountingEnableSet(dev, cncUnit, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call function cpssDxChCncCountingEnableGet
                               with not NULL enablePtr.
            Expected: GT_OK and the same enable as was set.
        */
        st = cpssDxChCncCountingEnableGet(dev, cncUnit, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCountingEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "got another enable then was set: %d", dev);

        /*
            1.3. Call  with wrong enum values cncUnit.
            Expected: GT_BAD_PARAM.
        */
        enable = GT_FALSE;

        UTF_ENUMS_CHECK_MAC(cpssDxChCncCountingEnableSet
                            (dev, cncUnit, enable),
                            cncUnit);
    }

    enable = GT_TRUE;
    cncUnit = CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCountingEnableSet(dev, cncUnit, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCountingEnableSet(dev, cncUnit, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCountingEnableGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit,
    OUT GT_BOOL                                 *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCountingEnableGet)
{
/*
    ITERATE_DEVICES (xCat and above)
    1.1. Call with cncUnit [CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E /
                            CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E].
    Expected: GT_OK.
    1.2. Call  with wrong enum values cncUnit .
    Expected: GT_BAD_PARAM.
    1.3. Call  with wrong enable [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit = 0;
    GT_BOOL enable;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with cncUnit [CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E /
                                    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E]
            Expected: GT_OK.
        */

        /*call with cncUnit [CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E] */

        cncUnit = CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_TTI_E;

        st = cpssDxChCncCountingEnableGet(dev, cncUnit, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*call with cncUnit [CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E] */

        cncUnit = CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E;

        st = cpssDxChCncCountingEnableGet(dev, cncUnit, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call  with wrong enum values cncUnit.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCncCountingEnableGet
                            (dev, cncUnit, &enable),
                            cncUnit);

        /*
            1.3. Call  with wrong enable [NULL].
            Expected: GT_BAD_PTR.
        */

        st = cpssDxChCncCountingEnableGet(dev, cncUnit, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, enable);
    }

    enable = GT_TRUE;
    cncUnit = CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_PCL_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCountingEnableGet(dev, cncUnit, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCountingEnableGet(dev, cncUnit, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCounterFormatSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCounterFormatSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
            and format [CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                        CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E /
                        CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E ]
    Expected: GT_OK.
    1.2. Call function cpssDxChCncCounterFormatGet with not NULL formatPtr
         and other params from 1.1.
    Expected: GT_OK and the same format as was set.
    1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
         and other params from 1.1.
    Expected: NOT GT_OK.
    1.4. Call function with wrong enum values format and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      blockNum = 0;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT format    = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT formatGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                    and format [CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                                CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E /
                                CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E].
            Expected: GT_OK.
        */
        /* iterate with blockNum = 0 */
        blockNum = 0;
        format   = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;

        st = cpssDxChCncCounterFormatSet(dev, blockNum, format);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, format);

        /*
            1.2. Call function cpssDxChCncCounterFormatGet with
                not NULL formatPtr and other params from 1.1.
            Expected: GT_OK and the same format as was set.
        */
        st = cpssDxChCncCounterFormatGet(dev, blockNum, &formatGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCounterFormatGet: %d, %d, %d", dev, blockNum, format);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(format, formatGet,
                   "got another format then was set: %d", dev);

        /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
        blockNum     = (CNC_BLOCKS_NUM(dev) - 1);
        format       = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E;

        st = cpssDxChCncCounterFormatSet(dev, blockNum, format);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, format);

        /*
            1.2. Call function cpssDxChCncCounterFormatGet
                with not NULL formatPtr and other params from 1.1.
            Expected: GT_OK and the same format as was set.
        */
        st = cpssDxChCncCounterFormatGet(dev, blockNum, &formatGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCounterFormatGet: %d, %d, %d", dev, blockNum, format);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(format, formatGet,
                   "got another format then was set: %d", dev);


        /* iterate with format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E;*/
        format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E;

        st = cpssDxChCncCounterFormatSet(dev, blockNum, format);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, format);

        /*
            1.2. Call function cpssDxChCncCounterFormatGet
                with not NULL formatPtr and other params from 1.1.
            Expected: GT_OK and the same format as was set.
        */
        st = cpssDxChCncCounterFormatGet(dev, blockNum, &formatGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChCncCounterFormatGet: %d, %d, %d", dev, blockNum, format);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(format, formatGet,
                   "got another format then was set: %d", dev);

        /*
            1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                 and other params from 1.1.
            Expected: NOT GT_OK.
        */
        blockNum = CNC_BLOCKS_NUM(dev);

        st = cpssDxChCncCounterFormatSet(dev, blockNum, format);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

        blockNum = 0;

        /*
            1.4. Call function with wrong enum values format and other params from 1.1.
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCncCounterFormatSet
                           (dev, blockNum, format),
                           format);
    }

    blockNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCounterFormatSet(dev, blockNum, format);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCounterFormatSet(dev, blockNum, format);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCounterFormatGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  *formatPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCounterFormatGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            and not NULL formatPtr.
    Expected: GT_OK.
    1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call function with formatPtr [NULL]
                            and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      blockNum = 0;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                    and not NULL formatPtr.
            Expected: GT_OK.
        */
        /* iterate with blockNum = 0 */
        blockNum = 0;

        st = cpssDxChCncCounterFormatGet(dev, blockNum, &format);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, format);

        /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
        blockNum = (CNC_BLOCKS_NUM(dev) - 1);

        st = cpssDxChCncCounterFormatGet(dev, blockNum, &format);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, format);

        /*
            1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                    and other params from 1.1.
            Expected: NOT GT_OK.
        */
        blockNum = CNC_BLOCKS_NUM(dev);

        st = cpssDxChCncCounterFormatGet(dev, blockNum, &format);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

        blockNum = 0;

        /*
            1.3. Call function with formatPtr [NULL]
                                    and other params from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncCounterFormatGet(dev, blockNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, formatPtr = NULL", dev);
    }

    blockNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCounterFormatGet(dev, blockNum, &format);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCounterFormatGet(dev, blockNum, &format);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCpuAccessStrictPriorityEnableSet
(
    IN  GT_U8   dev,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCpuAccessStrictPriorityEnableSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxChCncCpuAccessStrictPriorityEnableGet.
    Expected: GT_OK and the same enable than was set.
*/
    GT_STATUS   st       = GT_OK;
    GT_U8       dev;
    GT_BOOL     state    = GT_TRUE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* call with state  = GT_FALSE; */
        state  = GT_FALSE;

        st = cpssDxChCncCpuAccessStrictPriorityEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChCncCpuAccessStrictPriorityEnableGet.
            Expected: GT_OK and the same enable than was set.
        */
        st = cpssDxChCncCpuAccessStrictPriorityEnableGet(dev, &stateGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
             "cpssDxChCncCpuAccessStrictPriorityEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                               "get another state than was set: %d", dev);

        /* call with state  = GT_TRUE; */
        state  = GT_TRUE;

        st = cpssDxChCncCpuAccessStrictPriorityEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssDxChCncCpuAccessStrictPriorityEnableGet.
            Expected: GT_OK and the same enable than was set.
        */
        st = cpssDxChCncCpuAccessStrictPriorityEnableGet(dev, &stateGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                  "cpssDxChCncCpuAccessStrictPriorityEnableGet: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                               "get another state than was set: %d", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* call with state  = GT_TRUE; */
    state  = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCpuAccessStrictPriorityEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCpuAccessStrictPriorityEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncCpuAccessStrictPriorityEnableGet
(
    IN  GT_U8   dev,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncCpuAccessStrictPriorityEnableGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with not null enablePtr.
    Expected: GT_OK.
    1.2. Call with incorrect enablePtr[NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st     = GT_OK;
    GT_U8       dev;
    GT_BOOL     state;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChCncCpuAccessStrictPriorityEnableGet(dev, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call with incorrect enablePtr[NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncCpuAccessStrictPriorityEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, state);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncCpuAccessStrictPriorityEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncCpuAccessStrictPriorityEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncEgressQueueClientModeSet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncEgressQueueClientModeSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call function with mode [CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E /
                                  CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E].
    Expected: GT_OK.
    1.2. Call function cpssDxChCncEgressQueueClientModeGet with not NULL modePtr.
    Expected: GT_OK and the same mode as was set.
    1.3. Call function with wrong enum values mode.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT mode    = CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E;
    CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT modeGet;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with mode [CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E /
                                          CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E].
            Expected: GT_OK.
        */
        /* iterate with mode = CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E */
        mode = CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_CN_E;

        st = cpssDxChCncEgressQueueClientModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call function cpssDxChCncEgressQueueClientModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */

        st = cpssDxChCncEgressQueueClientModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncEgressQueueClientModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "got another mode then was set: %d", dev);

        /* iterate with mode = CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E */
        mode = CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E;

        st = cpssDxChCncEgressQueueClientModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call function cpssDxChCncEgressQueueClientModeGet with not NULL modePtr.
            Expected: GT_OK and the same mode as was set.
        */
        st = cpssDxChCncEgressQueueClientModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCncEgressQueueClientModeGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "got another mode then was set: %d", dev);

        /*
            1.3. Call function with wrong enum values mode .
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCncEgressQueueClientModeSet
                            (dev, mode),
                            mode);
    }

    mode = CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncEgressQueueClientModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncEgressQueueClientModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncEgressQueueClientModeGet
(
    IN  GT_U8                                    devNum,
    OUT CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncEgressQueueClientModeGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call function with not NULL modePtr.
    Expected: GT_OK.
    1.2. Call function with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT mode = CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_TAIL_DROP_E;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with not NULL modePtr.
            Expected: GT_OK.
        */
        st = cpssDxChCncEgressQueueClientModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with modePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCncEgressQueueClientModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E| UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncEgressQueueClientModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncEgressQueueClientModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupBlockClientEnableSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh3 and above)
    1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E],
                            updateEnable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call function cpssDxChCncPortGroupBlockClientEnableGet
         with not NULL updateEnablePtr and other params from 1.1.
    Expected: GT_OK and the same updateEnable as was set.
    1.1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
         and other params from 1.1.
    Expected: NOT GT_OK.
    1.1.4. Call function with wrong enum values client and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    blockNum        = 0;
    CPSS_DXCH_CNC_CLIENT_ENT  client          = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    GT_BOOL                   updateEnable    = GT_FALSE;
    GT_BOOL                   updateEnableGet = GT_FALSE;

    GT_PORT_GROUPS_BMP        portGroupsBmp = 1;
    GT_U32                    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                        client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                                CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E],
                                        updateEnable [GT_FALSE / GT_TRUE].
                Expected: GT_OK.
            */
            /* iterate with blockNum = 0 */
            blockNum     = 0;
            client       = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
            updateEnable = GT_FALSE;

            st = cpssDxChCncPortGroupBlockClientEnableSet(dev,
                        portGroupsBmp, blockNum, client, updateEnable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, blockNum, client, updateEnable);

            /*
                1.1.2. Call function cpssDxChCncPortGroupBlockClientEnableGet with not NULL updateEnablePtr
                                                                   and other params from 1.1.
                Expected: GT_OK and the same updateEnable as was set.
            */
            st = cpssDxChCncPortGroupBlockClientEnableGet(dev,
                        portGroupsBmp, blockNum, client, &updateEnableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                "cpssDxChCncPortGroupBlockClientEnableGet: %d, %d, %d", dev, blockNum, client);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(updateEnable, updateEnableGet,
                       "got another updateEnable then was set: %d", dev);

            /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
            blockNum     = (CNC_BLOCKS_NUM(dev) - 1);
            client       = CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E;
            updateEnable = GT_TRUE;

            st = cpssDxChCncPortGroupBlockClientEnableSet(dev,
                        portGroupsBmp, blockNum, client, updateEnable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, blockNum, client, updateEnable);

            /*
                1.1.2. Call function cpssDxChCncPortGroupBlockClientEnableGet with not NULL updateEnablePtr
                                                                   and other params from 1.1.
                Expected: GT_OK and the same updateEnable as was set.
            */
            st = cpssDxChCncPortGroupBlockClientEnableGet(dev,
                        portGroupsBmp, blockNum, client, &updateEnableGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
              "cpssDxChCncPortGroupBlockClientEnableGet: %d, %d, %d", dev, blockNum, client);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(updateEnable, updateEnableGet,
                       "got another updateEnable then was set: %d", dev);

            /*
                1.1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                   and other params from 1.1.
                Expected: NOT GT_OK.
            */
            blockNum = CNC_BLOCKS_NUM(dev);

            st = cpssDxChCncPortGroupBlockClientEnableSet(dev,
                        portGroupsBmp, blockNum, client, updateEnable);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

            blockNum = 0;

            /*
                1.1.4. Call function with wrong enum values client and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCncPortGroupBlockClientEnableSet
                               (dev, portGroupsBmp, blockNum, client, updateEnable),
                               client);

        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChCncPortGroupBlockClientEnableSet(dev,
                        portGroupsBmp, blockNum, client, updateEnable);

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

        st = cpssDxChCncPortGroupBlockClientEnableSet(dev,
                        portGroupsBmp, blockNum, client, updateEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;
    blockNum      = 0;
    client        = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    updateEnable  = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupBlockClientEnableSet(dev,
                    portGroupsBmp, blockNum, client, updateEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupBlockClientEnableSet(dev,
                     portGroupsBmp, blockNum, client, updateEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupBlockClientEnableGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh3 and above)
    1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E]
                            and not NULL updateEnablePtr.
    Expected: GT_OK.
    1.1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call function with wrong enum values client
                            and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call function with updateEnablePtr [NULL]
                            and other params from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    blockNum     = 0;
    CPSS_DXCH_CNC_CLIENT_ENT  client       = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    GT_BOOL                   updateEnable = GT_FALSE;
    GT_PORT_GROUPS_BMP        portGroupsBmp = 1;
    GT_U32                    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                        client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                                CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E]
                                        and not NULL updateEnablePtr.
                Expected: GT_OK.
            */
            /* iterate with blockNum = 0 */
            blockNum = 0;
            client = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

            st = cpssDxChCncPortGroupBlockClientEnableGet(dev,
                            portGroupsBmp, blockNum, client, &updateEnable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, client);

            /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
            blockNum = (CNC_BLOCKS_NUM(dev) - 1);
            client = CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E;

            st = cpssDxChCncPortGroupBlockClientEnableGet(dev,
                            portGroupsBmp, blockNum, client, &updateEnable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, client);

            /*
                1.1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                        and other params from 1.1.
                Expected: NOT GT_OK.
            */
            blockNum = CNC_BLOCKS_NUM(dev);

            st = cpssDxChCncPortGroupBlockClientEnableGet(dev,
                            portGroupsBmp, blockNum, client, &updateEnable);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

            blockNum = 0;

            /*
                1.1.3. Call function with wrong enum values client
                                        and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCncPortGroupBlockClientEnableGet
                               (dev, portGroupsBmp, blockNum, client, &updateEnable),
                               client);

            /*
                1.1.4. Call function with updateEnablePtr [NULL]
                                        and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCncPortGroupBlockClientEnableGet(dev,
                                            portGroupsBmp, blockNum, client, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, updateEnablePtr = NULL", dev);

        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChCncPortGroupBlockClientEnableGet(dev,
                                    portGroupsBmp, blockNum, client, &updateEnable);

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

        st = cpssDxChCncPortGroupBlockClientEnableGet(dev,
                                    portGroupsBmp, blockNum, client, &updateEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;
    blockNum      = 0;
    client        = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupBlockClientEnableGet(dev,
                            portGroupsBmp, blockNum, client, &updateEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupBlockClientEnableGet(dev,
                            portGroupsBmp, blockNum, client, &updateEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U64                      indexRangesBmp
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupBlockClientRangesSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh3 and above)
    1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E],
                            indexRangesBmp [0 / 255].
    Expected: GT_OK.
    1.1.2. Call function cpssDxChCncPortGroupBlockClientRangesGet with not NULL indexRangesBmpPtr
                                                            and other params from 1.1.
    Expected: GT_OK and the same indexRangesBmp as was set.
    1.1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.
    Expected: NOT GT_OK.
    1.1.4. Call function with wrong enum values client
                            and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.5. Call function with out of range indexRangesBmp [256]
                            and other params from 1.1.
    Expected: GT_OK for lion B0 and above and NOT GT_OK for others .
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    blockNum          = 0;
    CPSS_DXCH_CNC_CLIENT_ENT  client            = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    GT_U64                    indexRangesBmp    = {{0,0}};
    GT_U64                    indexRangesBmpGet = {{0,0}};

    GT_PORT_GROUPS_BMP        portGroupsBmp = 1;
    GT_U32                    portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                        client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E],
                        indexRangesBmp [0 / 255].
                Expected: GT_OK.
            */
            /* iterate with blockNum = 0 */
            blockNum       = 0;
            client         = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
            indexRangesBmp.l[0] = 0;

            st = cpssDxChCncPortGroupBlockClientRangesSet(dev,
                            portGroupsBmp, blockNum, client, indexRangesBmp);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, blockNum, client, indexRangesBmp.l[0]);

            if(GT_OK == st)
            {
                /*
                    1.1.2. Call function cpssDxChCncPortGroupBlockClientRangesGet
                    with not NULL indexRangesBmpPtr and other params from 1.1.1.
                    Expected: GT_OK and the same indexRangesBmp as was set.
                */
                st = cpssDxChCncPortGroupBlockClientRangesGet(dev,
                                    portGroupsBmp, blockNum, client, &indexRangesBmpGet);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                   "cpssDxChCncPortGroupBlockClientRangesGet: %d, %d, %d", dev, blockNum, client);

                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(indexRangesBmp.l[0], indexRangesBmpGet.l[0],
                           "got another indexRangesBmp then was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(indexRangesBmp.l[1], indexRangesBmpGet.l[1],
                           "got another indexRangesBmp then was set: %d", dev);
            }

            /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
            blockNum       = (CNC_BLOCKS_NUM(dev) - 1);
            client         = CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E;
            indexRangesBmp.l[0] = 255;

            st = cpssDxChCncPortGroupBlockClientRangesSet(dev,
                                        portGroupsBmp, blockNum, client, indexRangesBmp);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, blockNum, client, indexRangesBmp.l[0]);

            if(GT_OK == st)
            {
                /*
                    1.1.2. Call function cpssDxChCncPortGroupBlockClientRangesGet
                        with not NULL indexRangesBmpPtr and other params from 1.1.1.
                    Expected: GT_OK and the same indexRangesBmp as was set.
                */
                st = cpssDxChCncPortGroupBlockClientRangesGet(dev,
                                    portGroupsBmp, blockNum, client, &indexRangesBmpGet);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                  "cpssDxChCncPortGroupBlockClientRangesGet: %d, %d, %d", dev, blockNum, client);

                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(indexRangesBmp.l[0], indexRangesBmpGet.l[0],
                           "got another indexRangesBmp then was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(indexRangesBmp.l[1], indexRangesBmpGet.l[1],
                           "got another indexRangesBmp then was set: %d", dev);
            }

            /*
                1.1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                        and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            blockNum = CNC_BLOCKS_NUM(dev);

            st = cpssDxChCncPortGroupBlockClientRangesSet(dev,
                                    portGroupsBmp, blockNum, client, indexRangesBmp);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

            blockNum = 0;

            /*
                1.1.4. Call function with wrong enum values client
                                        and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCncPortGroupBlockClientRangesSet
                                (dev, portGroupsBmp, blockNum, client, indexRangesBmp),
                                client);

            /*
                1.1.5. Call function with out of range indexRangesBmp [256]
                                        and other params from 1.1.1.
                Expected: GT_OK for lion B0 and above and NOT GT_OK for others .
            */
            indexRangesBmp.l[0] = 256;

            st = cpssDxChCncPortGroupBlockClientRangesSet(dev, portGroupsBmp,
                                                blockNum, client, indexRangesBmp);

            if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(dev))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, indexRangesBmp.l[0]);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, indexRangesBmp.l[0]);
            }

        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        indexRangesBmp.l[0] = 0x0F;

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChCncPortGroupBlockClientRangesSet(dev, portGroupsBmp,
                                                blockNum, client, indexRangesBmp);

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

        st = cpssDxChCncPortGroupBlockClientRangesSet(dev, portGroupsBmp,
                                                blockNum, client, indexRangesBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp  = 1;
    blockNum       = 0;
    client         = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    indexRangesBmp.l[0] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupBlockClientRangesSet(dev,
                        portGroupsBmp, blockNum, client, indexRangesBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupBlockClientRangesSet(dev,
                        portGroupsBmp, blockNum, client, indexRangesBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupBlockClientRangesGet
(
    IN  GT_U8                     devNum,
    IN  GT_PORT_GROUPS_BMP        portGroupsBmp,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_U32                    *indexRangesBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupBlockClientRangesGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh3 and above)
    1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                    CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E]
                            and not NULL indexRangesBmpPtr.
    Expected: GT_OK.
    1.1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call function with wrong enum values client
                            and other params from 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call function with indexRangesBmpPtr [NULL]
                            and other params from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    blockNum       = 0;
    CPSS_DXCH_CNC_CLIENT_ENT  client         = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;
    GT_U64                    indexRangesBmp = {{0,0}};

    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                        client [CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E /
                                CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E]
                        and not NULL indexRangesBmpPtr.
                Expected: GT_OK.
            */
            /* iterate with blockNum = 0 */
            blockNum = 0;
            client   = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

            st = cpssDxChCncPortGroupBlockClientRangesGet(dev,
                            portGroupsBmp, blockNum, client, &indexRangesBmp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, client);

            /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
            blockNum = (CNC_BLOCKS_NUM(dev) - 1);
            client   = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

            st = cpssDxChCncPortGroupBlockClientRangesGet(dev,
                                portGroupsBmp, blockNum, client, &indexRangesBmp);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, client);

            /*
                1.1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                        and other params from 1.1.1.
                Expected: NOT GT_OK
            */
            blockNum = CNC_BLOCKS_NUM(dev);

            st = cpssDxChCncPortGroupBlockClientRangesGet(dev, portGroupsBmp,
                    blockNum, client, &indexRangesBmp);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

            blockNum = 0;

            /*
                1.1.3. Call function with wrong enum values client
                                        and other params from 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCncPortGroupBlockClientRangesGet
                                (dev, portGroupsBmp, blockNum, client, &indexRangesBmp),
                                client);

            /*
                1.1.4. Call function with indexRangesBmpPtr [NULL]
                                        and other params from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCncPortGroupBlockClientRangesGet(dev, portGroupsBmp,
                                                    blockNum, client, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, indexRangesBmpPtr = NULL", dev);

        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChCncPortGroupBlockClientRangesGet(dev, portGroupsBmp,
                                                    blockNum, client, &indexRangesBmp);

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

        st = cpssDxChCncPortGroupBlockClientRangesGet(dev, portGroupsBmp,
                                                    blockNum, client, &indexRangesBmp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;
    blockNum = 0;
    client   = CPSS_DXCH_CNC_CLIENT_L2L3_INGRESS_VLAN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupBlockClientRangesGet(dev, portGroupsBmp,
                            blockNum, client, &indexRangesBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupBlockClientRangesGet(dev, portGroupsBmp, blockNum,
                        client, &indexRangesBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    OUT GT_U32                     *inProcessBlocksBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupBlockUploadInProcessGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh3 and above)
    1.1.1. Call function with not NULL inProcessBlocksBmpPtr.
    Expected: GT_OK.
    1.1.2. Call function with inProcessBlocksBmpPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U32              inProcessBlocksBmp = 0;
    GT_PORT_GROUPS_BMP  portGroupsBmp = 1;
    GT_U32              portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with not NULL inProcessBlocksBmpPtr.
                Expected: GT_OK.
            */
            st = cpssDxChCncPortGroupBlockUploadInProcessGet(
                dev, portGroupsBmp, &inProcessBlocksBmp);
            PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(dev, st, GT_OK);

            /*
                1.1.2. Call function with inProcessBlocksBmpPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCncPortGroupBlockUploadInProcessGet(
                dev, portGroupsBmp, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(
                GT_BAD_PTR, st,
                "%d, inProcessBlocksBmpPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChCncPortGroupBlockUploadInProcessGet(
                dev, portGroupsBmp, &inProcessBlocksBmp);

            if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(
                    dev, st, GT_BAD_PARAM);
            }
            else
            {
                if (st != GT_NO_MORE)
                {
                    PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(
                        dev, st, GT_OK);
                }
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChCncPortGroupBlockUploadInProcessGet(
            dev, portGroupsBmp, &inProcessBlocksBmp);
        PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(dev, st, GT_OK);
    }

    portGroupsBmp = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupBlockUploadInProcessGet(dev,
                                        portGroupsBmp, &inProcessBlocksBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupBlockUploadInProcessGet(dev,
                                        portGroupsBmp, &inProcessBlocksBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      blockNum
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupBlockUploadTrigger)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh3 and above)
    1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)].
    Expected: GT_OK.
    1.1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U32      blockNum = 0;

    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next active port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)].
                Expected: GT_OK.
            */

            /* clear the FU message queue */
            clearMemoAfterBlockUploadTrigger(dev, portGroupsBmp);

            /* iterate with blockNum = 0 */
            blockNum = 0;

            st = cpssDxChCncPortGroupBlockUploadTrigger(dev, portGroupsBmp, blockNum);
            PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(dev, st, GT_OK);

            cpssOsTimerWkAfter(1);

            /* clear the FU message queue */
            clearMemoAfterBlockUploadTrigger(dev, portGroupsBmp);

            /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
            blockNum = (CNC_BLOCKS_NUM(dev) - 1);

            st = cpssDxChCncPortGroupBlockUploadTrigger(
                dev, portGroupsBmp, blockNum);
            PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(dev, st, GT_OK);

            cpssOsTimerWkAfter(1);

            /* clear the FU message queue */
            clearMemoAfterBlockUploadTrigger(dev, portGroupsBmp);

            /*
                1.1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM].
                Expected: NOT GT_OK.
            */
            blockNum = CNC_BLOCKS_NUM(dev);

            st = cpssDxChCncPortGroupBlockUploadTrigger(dev, portGroupsBmp, blockNum);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        blockNum = 0;

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next port */
            portGroupsBmp = (1 << portGroupId);

            if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                st = cpssDxChCncPortGroupBlockUploadTrigger(
                    dev, portGroupsBmp, blockNum);

                PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(
                    dev, st, GT_BAD_PARAM);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* wait 1 milliseconds */
        cpssOsTimerWkAfter(1);

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        /* clear the FU message queue */
        clearMemoAfterBlockUploadTrigger(dev, portGroupsBmp);

        st = cpssDxChCncPortGroupBlockUploadTrigger(
            dev, portGroupsBmp, blockNum);
        PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(dev, st, GT_OK);

        cpssOsTimerWkAfter(1);

        /* clear the FU message queue */
        clearMemoAfterBlockUploadTrigger(dev, portGroupsBmp);
    }

    portGroupsBmp = 1;
    blockNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupBlockUploadTrigger(dev, portGroupsBmp, blockNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupBlockUploadTrigger(dev, portGroupsBmp, blockNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupCounterFormatSet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupCounterFormatSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (Lion and above)
    1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
            and format [CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                        CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E /
                        CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E ]
    Expected: GT_OK.
    1.1.2. Call function cpssDxChCncPortGroupCounterFormatGet with not NULL formatPtr
         and other params from 1.1.1.
    Expected: GT_OK and the same format as was set.
    1.1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
         and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.4. Call function with wrong enum values format and other params from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      blockNum = 0;

    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT format    = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT formatGet;

    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                        and format [CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                                    CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E /
                                    CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E].
                Expected: GT_OK.
            */
            /* iterate with blockNum = 0 */
            blockNum = 0;
            format   = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;

            st = cpssDxChCncPortGroupCounterFormatSet(dev, portGroupsBmp, blockNum, format);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, format);

            /*
                1.1.2. Call function cpssDxChCncPortGroupCounterFormatGet with
                    not NULL formatPtr and other params from 1.1.1.
                Expected: GT_OK and the same format as was set.
            */
            st = cpssDxChCncPortGroupCounterFormatGet(dev, portGroupsBmp, blockNum, &formatGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChCncPortGroupCounterFormatGet: %d, %d, %d", dev, blockNum, format);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(format, formatGet,
                       "got another format then was set: %d", dev);

            /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
            blockNum     = (CNC_BLOCKS_NUM(dev) - 1);
            format       = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E;

            st = cpssDxChCncPortGroupCounterFormatSet(dev, portGroupsBmp, blockNum, format);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, format);

            /*
                1.1.2. Call function cpssDxChCncPortGroupCounterFormatGet
                    with not NULL formatPtr and other params from 1.1.1.
                Expected: GT_OK and the same format as was set.
            */
            st = cpssDxChCncPortGroupCounterFormatGet(dev, portGroupsBmp, blockNum, &formatGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChCncPortGroupCounterFormatGet: %d, %d, %d", dev, blockNum, format);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(format, formatGet,
                       "got another format then was set: %d", dev);


            /* iterate with format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E;*/
            format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E;

            st = cpssDxChCncPortGroupCounterFormatSet(dev, portGroupsBmp, blockNum, format);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, format);

            /*
                1.1.2. Call function cpssDxChCncPortGroupCounterFormatGet
                    with not NULL formatPtr and other params from 1.1.1.
                Expected: GT_OK and the same format as was set.
            */
            st = cpssDxChCncPortGroupCounterFormatGet(dev, portGroupsBmp, blockNum, &formatGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChCncPortGroupCounterFormatGet: %d, %d, %d", dev, blockNum, format);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(format, formatGet,
                       "got another format then was set: %d", dev);

            /*
                1.1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                     and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            blockNum = CNC_BLOCKS_NUM(dev);

            st = cpssDxChCncPortGroupCounterFormatSet(dev, portGroupsBmp, blockNum, format);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

            blockNum = 0;

            /*
                1.1.4. Call function with wrong enum values format and other params from 1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCncPortGroupCounterFormatSet
                               (dev, portGroupsBmp, blockNum, format),
                               format);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChCncPortGroupCounterFormatSet(dev, portGroupsBmp, blockNum, format);

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

        st = cpssDxChCncPortGroupCounterFormatSet(dev, portGroupsBmp, blockNum, format);

        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;
    blockNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupCounterFormatSet(dev, portGroupsBmp, blockNum, format);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupCounterFormatSet(dev, portGroupsBmp, blockNum, format);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupCounterFormatGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  *formatPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupCounterFormatGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (Lion and above)
    1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            and not NULL formatPtr.
    Expected: GT_OK.
    1.1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call function with formatPtr [NULL]
                            and other params from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      blockNum = 0;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format;

    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                        and not NULL formatPtr.
                Expected: GT_OK.
            */
            /* iterate with blockNum = 0 */
            blockNum = 0;

            st = cpssDxChCncPortGroupCounterFormatGet(dev, portGroupsBmp, blockNum, &format);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, format);

            /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
            blockNum = (CNC_BLOCKS_NUM(dev) - 1);

            st = cpssDxChCncPortGroupCounterFormatGet(dev, portGroupsBmp, blockNum, &format);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, format);

            /*
                1.1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                        and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            blockNum = CNC_BLOCKS_NUM(dev);

            st = cpssDxChCncPortGroupCounterFormatGet(dev, portGroupsBmp, blockNum, &format);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

            blockNum = 0;

            /*
                1.1.3. Call function with formatPtr [NULL]
                                        and other params from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCncPortGroupCounterFormatGet(dev, portGroupsBmp, blockNum, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, formatPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChCncPortGroupCounterFormatGet(dev, portGroupsBmp, blockNum, &format);

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

        st = cpssDxChCncPortGroupCounterFormatGet(dev, portGroupsBmp, blockNum, &format);

        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;
    blockNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupCounterFormatGet(dev, portGroupsBmp, blockNum, &format);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupCounterFormatGet(dev, portGroupsBmp, blockNum, &format);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupCounterSet
(
    IN  GT_U8                            devNum,
    IN  GT_PORT_GROUPS_BMP               portGroupsBmp,
    IN  GT_U32                           blockNum,
    IN  GT_U32                           index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT format,
    IN  CPSS_DXCH_CNC_COUNTER_STC       *counterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupCounterSet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh3 and above)
    1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            index [0 / 2047],
                            format [CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                                    CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                                    CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E],
                            counterPtr { byteCount [0 / 0x0007 FFFF FFFF],
                                         packetCount [0 / 0x1FFF FFFF] }.
    Expected: GT_OK.
    1.1.2. Call function cpssDxChCncPortGroupCounterGet
        with not NULL counterPtr and other params from 1.1.1.
    Expected: GT_OK and the same counter as was set.
    1.1.2. Call function cpssDxChCncPortGroupCounterGet
        with not NULL counterPtr and other params from 1.1.1.
    Expected: GT_OK and the same counter as was set.
    1.1.2. Call function cpssDxChCncPortGroupCounterGet
        with not NULL counterPtr and other params from 1.1.1.
    Expected: GT_OK and the same counter as was set.
    1.1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.4. Call function with out of range index [2048] and other params from 1.1.
    Expected: NOT GT_OK.
    1.1.5. Call function with wrong enum value format and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.6. Call function with counterPtr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    blockNum = 0;
    GT_U32                    index    = 0;

    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;

    CPSS_DXCH_CNC_COUNTER_STC counter;
    CPSS_DXCH_CNC_COUNTER_STC counterGet;
    GT_BOOL                   isEqual  = GT_FALSE;

    GT_PORT_GROUPS_BMP        portGroupsBmp = 1;
    GT_U32                    portGroupId;

    cpssOsBzero((GT_VOID*) &counter, sizeof(counter));
    cpssOsBzero((GT_VOID*) &counterGet, sizeof(counterGet));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                        index [0 / 2047],
                                        format [CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                                                CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                                                CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E],
                                        counterPtr { byteCount [0 / 0x0007 FFFF FFFF],
                                                     packetCount [0 / 0x1FFF FFFF] }.
                Expected: GT_OK.
            */
            /* iterate with blockNum = 0 */
            blockNum = 0;
            index    = 0;

            counter.byteCount.l[0] = 0xFFFFFFFF;
            counter.byteCount.l[1] = 0x07;
            counter.packetCount.l[0] = 0x1FFFFFFF;
            counter.packetCount.l[1] = 0;

            format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;

            st = cpssDxChCncPortGroupCounterSet(dev, portGroupsBmp,
                                blockNum, index, format, &counter);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, index);

            /*
                1.1.2. Call function cpssDxChCncPortGroupCounterGet
                    with not NULL counterPtr and other params from 1.1.1.
                Expected: GT_OK and the same counter as was set.
            */
            st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                            blockNum, index, format, &counterGet);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "cpssDxChCncPortGroupCounterGet: %d, %d, &d", dev, blockNum, index);

            /* Veryfying values */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &counter,
                                         (GT_VOID*) &counterGet,
                                         sizeof(counter))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "got  another counter then was set: %d", dev);


            /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) and
               format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E; */
            blockNum = (CNC_BLOCKS_NUM(dev) - 1);
            index    = CNC_BLOCK_ENTRIES_NUM(dev) - 1;

            if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(dev) != 0)
            {
                counter.byteCount.l[0] = 0xFFFFFFFF;
                counter.byteCount.l[1] = 0x0000001F;
                counter.packetCount.l[0] = 0x07FFFFFF;
                counter.packetCount.l[1] = 0;

                format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E;

                st = cpssDxChCncPortGroupCounterSet(dev, portGroupsBmp,
                                                   blockNum, index, format, &counter);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, index);

                /*
                    1.1.2. Call function cpssDxChCncPortGroupCounterGet
                        with not NULL counterPtr and other params from 1.1.1.
                    Expected: GT_OK and the same counter as was set.
                */
                st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                                blockNum, index, format, &counterGet);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                     "cpssDxChCncPortGroupCounterGet: %d, %d, &d", dev, blockNum, index);

                /* Veryfying values */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &counter,
                                             (GT_VOID*) &counterGet,
                                             sizeof(counter))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                           "got  another counter then was set: %d", dev);


                /* iterate with format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E; */
                blockNum = (CNC_BLOCKS_NUM(dev) - 1);
                index    = CNC_BLOCK_ENTRIES_NUM(dev) - 1;

                counter.byteCount.l[0]   = 0x07FFFFFF;
                counter.byteCount.l[1]   = 0;
                counter.packetCount.l[0] = 0xFFFFFFFF;
                counter.packetCount.l[1] = 0x0000001F;

                format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E;

                st = cpssDxChCncPortGroupCounterSet(dev, portGroupsBmp,
                                                   blockNum, index, format, &counter);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, index);

                /*
                    1.1.2. Call function cpssDxChCncPortGroupCounterGet
                        with not NULL counterPtr and other params from 1.1.1.
                    Expected: GT_OK and the same counter as was set.
                */
                st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                                blockNum, index, format, &counterGet);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                     "cpssDxChCncPortGroupCounterGet: %d, %d, &d", dev, blockNum, index);

                /* Veryfying values */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &counter,
                                             (GT_VOID*) &counterGet,
                                             sizeof(counter))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                           "got  another counter then was set: %d", dev);

            }

            /*
                1.1.3. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                        and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            blockNum = CNC_BLOCKS_NUM(dev);

            st = cpssDxChCncPortGroupCounterSet(dev, portGroupsBmp,
                                               blockNum, index, format, &counter);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

            blockNum = 0;

            /*
                1.1.4. Call function with out of range index [2048] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            index = CNC_BLOCK_ENTRIES_NUM(dev);

            st = cpssDxChCncPortGroupCounterSet(dev, portGroupsBmp,
                                               blockNum, index, format, &counter);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, index = %d", dev, index);

            index = 0;

            if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(dev) != 0)
            {
                /* The test relevant for Lion only */
                /*
                    1.1.5. Call function with wrong enum value format and other params from 1.1.
                    Expected: GT_BAD_PARAM.
                */
                UTF_ENUMS_CHECK_MAC(
                    cpssDxChCncPortGroupCounterSet
                    (dev, portGroupsBmp, blockNum, index, format, &counter),
                    format);
            }

            /*
                1.1.6. Call function with counterPtr [NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCncPortGroupCounterSet(dev, portGroupsBmp,
                                               blockNum, index, format, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChCncPortGroupCounterSet(dev, portGroupsBmp,
                                               blockNum, index, format, &counter);

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

        st = cpssDxChCncPortGroupCounterSet(dev, portGroupsBmp,
                                               blockNum, index, format, &counter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;
    blockNum = 0;
    index    = 0;

    counter.byteCount.l[0] = 0x00;
    counter.byteCount.l[1] = 0x00;
    counter.packetCount.l[0] = 0;
    counter.packetCount.l[1] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupCounterSet(dev, portGroupsBmp,
                           blockNum, index, format, &counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupCounterSet(dev, portGroupsBmp,
                           blockNum, index, format, &counter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupCounterGet
(
    IN  GT_U8                             devNum,
    IN  GT_PORT_GROUPS_BMP                portGroupsBmp,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupCounterGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh3 and above)
    1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                            index [0 / 2047] and not NULL counterPtr.
    Expected: GT_OK.
    1.1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                            and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call function with out of range index [2048] and other params from 1.1.
    Expected: NOT GT_OK.
    1.1.4. Call function with counterPtr [NULL]
                            and other params from 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                      blockNum = 0;
    GT_U32                      index    = 0;

    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;

    CPSS_DXCH_CNC_COUNTER_STC   counter;
    GT_PORT_GROUPS_BMP          portGroupsBmp = 1;
    GT_U32                      portGroupId;

    cpssOsBzero((GT_VOID*) &counter, sizeof(counter));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                                        index [0 / 2047] and not NULL counterPtr.
                Expected: GT_OK.
            */
            /* iterate with blockNum = 0 */
            blockNum = 0;
            index    = 0;
            format   = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;

            st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                            blockNum, index, format, &counter);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, index);

            /* iterate with blockNum = CNC_BLOCKS_NUM - 1 */
            blockNum = (CNC_BLOCKS_NUM(dev) - 1);
            index    = CNC_BLOCK_ENTRIES_NUM(dev) - 1;
            format   = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E;

            st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                            blockNum, index, format, &counter);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, index);

            /*
                1.1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                        and other params from 1.1.1.
                Expected: NOT GT_OK.
            */
            blockNum = CNC_BLOCKS_NUM(dev);

            st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                            blockNum, index, format, &counter);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

            blockNum = 0;

            /*
                1.1.3. Call function with out of range index [2048] and other params from 1.1.
                Expected: NOT GT_OK.
            */
            index = CNC_BLOCK_ENTRIES_NUM(dev);

            st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                            blockNum, index, format, &counter);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, index = %d", dev, index);

            index = 0;

            if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(dev) != 0)
            {
                /* The test relevant for Lion only */
                /*
                    1.1.4. Call function with wrong enum value format and other params from 1.1.
                    Expected: GT_BAD_PARAM.
                */
                UTF_ENUMS_CHECK_MAC(
                    cpssDxChCncPortGroupCounterGet
                    (dev, portGroupsBmp, blockNum, index, format, &counter),
                    format);
            }

            /*
                1.1.5. Call function with counterPtr [NULL]
                                        and other params from 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                            blockNum, index, format, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                            blockNum, index, format, &counter);

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

        st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                            blockNum, index, format, &counter);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;
    blockNum = 0;
    index    = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                        blockNum, index, format, &counter);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupCounterGet(dev, portGroupsBmp,
                                    blockNum, index, format, &counter);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupCounterWraparoundIndexesGet
(
    IN    GT_U8                  devNum,
    IN    GT_PORT_GROUPS_BMP     portGroupsBmp,
    IN    GT_U32                 blockNum,
    INOUT GT_U32                *indexNumPtr,
    OUT   GT_U32                 portGroupIdArr[],
    OUT   GT_U32                 indexesArr[]
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupCounterWraparoundIndexesGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh3 and above)
    1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
               indexNumPtr [0 / 8] and not NULL indexesArr.
    Expected GT_OK.
    1.1.2. Call function with out of range blockNum [(CNC_BLOCKS_NUM - 1)]
                            and other params from 1.1.1.
    Expected: NOT GT_OK.
    1.1.3. Call function with indexNumPtr [NULL] and other params from 1.1.
    Expected: GT_OK (NULL pointer supported).
    1.1.4. Call function with portGroupIdArr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
    1.1.5. Call function with indexesArr [NULL] and other params from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32   blockNum  = 0;
    GT_U32   indexNum  = 0;
    GT_U32   portGroupIdArr[255];
    GT_U32   indexesArr[255];

    GT_PORT_GROUPS_BMP   portGroupsBmp = 1;
    GT_U32               portGroupId;

    cpssOsBzero((GT_VOID*) portGroupIdArr, sizeof(portGroupIdArr[0])*255);
    cpssOsBzero((GT_VOID*) indexesArr, sizeof(indexesArr[0])*255);

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with blockNum [0 / (CNC_BLOCKS_NUM - 1)],
                              indexNumPtr [0 / 8] and not NULL indexesArr.
                Expected GT_OK.
            */
            /* iterate with blockNum = 0 */
            blockNum = 0;
            indexNum = 0;

            st = cpssDxChCncPortGroupCounterWraparoundIndexesGet(dev,
                    portGroupsBmp, blockNum, &indexNum, portGroupIdArr, indexesArr);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, indexNum);

            /* iterate with blockNum = (CNC_BLOCKS_NUM - 1) */
            blockNum = (CNC_BLOCKS_NUM(dev) - 1);
            indexNum = 8;

            st = cpssDxChCncPortGroupCounterWraparoundIndexesGet(dev,
                    portGroupsBmp, blockNum, &indexNum, portGroupIdArr, indexesArr);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, blockNum, indexNum);

            /*
                1.1.2. Call function with out of range blockNum [CNC_BLOCKS_NUM]
                                        and other params from 1.1.
                Expected: NOT GT_OK.
            */
            blockNum = CNC_BLOCKS_NUM(dev);

            st = cpssDxChCncPortGroupCounterWraparoundIndexesGet(dev,
                    portGroupsBmp, blockNum, &indexNum, portGroupIdArr, indexesArr);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, blockNum);

            blockNum = 0;

            /*
                1.1.3. Call function with indexNumPtr [NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCncPortGroupCounterWraparoundIndexesGet(dev,
                    portGroupsBmp, blockNum, NULL, portGroupIdArr, indexesArr);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, indexNumPtr = NULL", dev);

            /*
                1.1.4. Call function with portGroupIdArr [NULL] and other params from 1.1.
                Expected: GT_OK (NULL pointer supported).
            */
            st = cpssDxChCncPortGroupCounterWraparoundIndexesGet(dev,
                    portGroupsBmp, blockNum, &indexNum, NULL, indexesArr);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "%d, portGroupIdArr = NULL", dev);

            /*
                1.1.5. Call function with indexesArr [NULL] and other params from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCncPortGroupCounterWraparoundIndexesGet(dev,
                                portGroupsBmp, blockNum, &indexNum, portGroupIdArr, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, indexesArr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next non-active port */
            portGroupsBmp = (1 << portGroupId);

            st = cpssDxChCncPortGroupCounterWraparoundIndexesGet(dev,
                    portGroupsBmp, blockNum, &indexNum, portGroupIdArr, indexesArr);

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

        st = cpssDxChCncPortGroupCounterWraparoundIndexesGet(dev,
                    portGroupsBmp, blockNum, &indexNum, portGroupIdArr, indexesArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portGroupsBmp);
    }

    portGroupsBmp = 1;
    blockNum = 0;
    indexNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupCounterWraparoundIndexesGet(dev, portGroupsBmp,
                            blockNum, &indexNum, portGroupIdArr, indexesArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupCounterWraparoundIndexesGet(dev, portGroupsBmp,
                        blockNum, &indexNum, portGroupIdArr, indexesArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCncPortGroupUploadedBlockGet
(
    IN     GT_U8                             devNum,
    IN     GT_PORT_GROUPS_BMP                portGroupsBmp,
    INOUT  GT_U32                           *numOfCounterValuesPtr,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT    CPSS_DXCH_CNC_COUNTER_STC        *counterValuesPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCncPortGroupUploadedBlockGet)
{
/*
    ITERATE_DEVICES_PORT_GROUPS (DxCh3 and above)
    1.1.1. Call function with numOfCounterValuesPtr [0 / 5 / 7],
            format [CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                    CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                    CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E],
         and not NULL counterValuesPtr.
    Expected: GT_OK.
    1.1.2. Call function with numOfCounterValuesPtr [NULL] and other param from 1.1.
    Expected: GT_BAD_PTR.
    1.1.3. Call function with wrong enum value format and other params from 1.1.
    Expected: GT_BAD_PARAM.
    1.1.4. Call function with counterValuesPtr [NULL] and other param from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                           numOfCounterValues = 0;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;
    CPSS_DXCH_CNC_COUNTER_STC        counterValues[10];

    GT_PORT_GROUPS_BMP      portGroupsBmp = 1;
    GT_U32                  portGroupId;

    cpssOsBzero((GT_VOID*) counterValues, sizeof(counterValues));

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Go over all active port groups. */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev, portGroupId)
        {
            /* set next port */
            portGroupsBmp = (1 << portGroupId);
            /*
                1.1.1. Call function with numOfCounterValuesPtr [0 / 5 / 7],
                        format [CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                                CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E /
                                CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E],
                     and not NULL counterValuesPtr.
                Expected: GT_OK.
            */
            /* Call with numOfCounterValues [0]
               and format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;*/

            numOfCounterValues = 0;
            format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;

            st = cpssDxChCncPortGroupUploadedBlockGet(
                dev, portGroupsBmp,
                &numOfCounterValues, format, counterValues);
            if (st != GT_NO_MORE)
            {
                PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(
                    dev, st, GT_OK);
            }

            /* Call with numOfCounterValues [5]
               and format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E;*/

            numOfCounterValues = 5;
            format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_1_E;

            st = cpssDxChCncPortGroupUploadedBlockGet(dev, portGroupsBmp,
                                &numOfCounterValues, format, counterValues);
            if (st != GT_NO_MORE)
            {
                PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(
                    dev, st, GT_OK);
            }

            /* Call with numOfCounterValues [7]
               and format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E;*/

            numOfCounterValues = 7;
            format = CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_2_E;

            st = cpssDxChCncPortGroupUploadedBlockGet(dev, portGroupsBmp,
                                &numOfCounterValues, format, counterValues);
            if (st != GT_NO_MORE)
            {
                PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(
                    dev, st, GT_OK);
            }

            /*
                1.1.2. Call function with numOfCounterValuesPtr [NULL] and other param from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCncPortGroupUploadedBlockGet(dev, portGroupsBmp,
                                            NULL, format, counterValues);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                    "%d, numOfCounterValuesPtr = NULL", dev);

            if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(dev) != 0)
            {
                /* The test relevant for Lion only */
                /*
                    1.1.3. Call function with wrong enum value format and other params from 1.1.
                    Expected: GT_BAD_PARAM.
                */
                UTF_ENUMS_CHECK_MAC(
                    cpssDxChCncPortGroupUploadedBlockGet
                    (dev, portGroupsBmp, &numOfCounterValues, format, counterValues),
                    format);
            }

            /*
                1.1.4. Call function with counterValuesPtr [NULL] and other param from 1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCncPortGroupUploadedBlockGet(dev, portGroupsBmp,
                                &numOfCounterValues, format, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, counterValuesPtr = NULL", dev);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.2. For not-active port groups check that function returns GT_BAD_PARAM. */
        PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)
        {
            /* set next port */
            portGroupsBmp = (1 << portGroupId);

            if (PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(dev))
            {
                st = cpssDxChCncPortGroupUploadedBlockGet(
                    dev, portGroupsBmp,
                    &numOfCounterValues, format, counterValues);
                UTF_VERIFY_EQUAL2_PARAM_MAC(
                    GT_BAD_PARAM, st, dev, portGroupsBmp);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(dev,portGroupId)

        /* 1.3. For unaware port groups check that function returns GT_OK. */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

        st = cpssDxChCncPortGroupUploadedBlockGet(
            dev, portGroupsBmp,
            &numOfCounterValues, format, counterValues);
        if (st != GT_NO_MORE)
        {
            PRV_CPSS_DXCH_UPLOAD_RELATED_FUNC_CHECK_MAC(
                dev, st, GT_OK);
        }
    }

    portGroupsBmp = 1;
    numOfCounterValues = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCncPortGroupUploadedBlockGet(dev, portGroupsBmp,
                            &numOfCounterValues, format, counterValues);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCncPortGroupUploadedBlockGet(dev, portGroupsBmp,
                        &numOfCounterValues, format, counterValues);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChCnc suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChCnc)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncBlockClientEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncBlockClientEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncBlockClientRangesSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncBlockClientRangesGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortClientEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortClientEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncClientByteCountModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncClientByteCountModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncEgressVlanDropCountModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncEgressVlanDropCountModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCounterClearByReadEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCounterClearByReadEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCounterClearByReadValueSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCounterClearByReadValueGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCounterWraparoundEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCounterWraparoundEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCounterWraparoundIndexesGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCounterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncBlockUploadTrigger)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncBlockUploadInProcessGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncUploadedBlockGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCountingEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCountingEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCounterFormatSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCounterFormatGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCpuAccessStrictPriorityEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncCpuAccessStrictPriorityEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncEgressQueueClientModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncEgressQueueClientModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupBlockClientEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupBlockClientEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupBlockClientRangesSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupBlockClientRangesGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupBlockUploadInProcessGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupBlockUploadTrigger)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupCounterFormatSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupCounterFormatGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupCounterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupCounterWraparoundIndexesGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCncPortGroupUploadedBlockGet)
UTF_SUIT_END_TESTS_MAC(cpssDxChCnc)
