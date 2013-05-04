/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCscdUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChCscd, that provides
*       Cascading definitions -- Cheetah.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Defines */

/* Default valid value for port id */
#define CSCD_VALID_PHY_PORT_CNS  0

/* Invalid  tc  - Traffic Class of the packet on the source port (0..7)*/
#define CSCD_INVALID_TC_CNS      8

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdPortTypeSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        portNum,
    IN CPSS_CSCD_PORT_TYPE_ENT      portType
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdPortTypeSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxChx)
    1.1.1. Call with portType [CPSS_CSCD_PORT_DSA_MODE_REGULAR_E /
                               CPSS_CSCD_PORT_DSA_MODE_EXTEND_E /
                               CPSS_CSCD_PORT_NETWORK_E].
    Expected: GT_OK.
    1.1.2. Call cpssDxChCscdPortTypeGet.
    Expected: GT_OK and the same value.
    1.1.3. Call with wrong enum values portType .
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st          = GT_OK;

    GT_U8                   dev;
    GT_U8                   port        = CSCD_VALID_PHY_PORT_CNS;
    CPSS_CSCD_PORT_TYPE_ENT portType    = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;

    CPSS_CSCD_PORT_TYPE_ENT portTypeGet = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with portType [CPSS_CSCD_PORT_DSA_MODE_REGULAR_E /
                                           CPSS_CSCD_PORT_DSA_MODE_EXTEND_E /
                                           CPSS_CSCD_PORT_NETWORK_E].
                Expected: GT_OK.
            */

            /* Call function with portType [CPSS_CSCD_PORT_DSA_MODE_EXTEND_E] */
            portType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;

            st = cpssDxChCscdPortTypeSet(dev, port, portType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, portType);

            /* Call function with portType [CPSS_CSCD_PORT_DSA_MODE_REGULAR_E] */
            portType = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E;

            st = cpssDxChCscdPortTypeSet(dev, port, portType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, portType);

            /* Call function with portType [CPSS_CSCD_PORT_NETWORK_E] */
            portType = CPSS_CSCD_PORT_NETWORK_E;

            st = cpssDxChCscdPortTypeSet(dev, port, portType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, portType);

            /*
                1.1.2. Call cpssDxChCscdPortTypeGet.
                Expected: GT_OK and the same value.
            */
            st = cpssDxChCscdPortTypeGet(dev, port, &portTypeGet);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, portType);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(portType, portTypeGet,
                       "get another portType than was set: %d", dev);

            /*
                1.1.3. Call with wrong enum values portType .
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCscdPortTypeSet
                                (dev, port, portType),
                                portType);
        }

        portType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdPortTypeSet(dev, port, portType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdPortTypeSet(dev, port, portType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdPortTypeSet(dev, port, portType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    portType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdPortTypeSet(dev, port, portType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdPortTypeSet(dev, port, portType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdPortTypeGet
(
    IN  GT_U8                        devNum,
    IN  GT_U8                        portNum,
    OUT CPSS_CSCD_PORT_TYPE_ENT     *portTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdPortTypeGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxChx)
    1.1.1. Call with not null portType.
    Expected: GT_OK.
    1.1.2. Call with wrong portType [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;

    GT_U8                   dev;
    GT_U8                   port = CSCD_VALID_PHY_PORT_CNS;
    CPSS_CSCD_PORT_TYPE_ENT portType;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with not null portType.
                Expected: GT_OK.
            */
            st = cpssDxChCscdPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, portType);

            /*
                1.1.2. Call with wrong portType [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCscdPortTypeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PTR, st, dev, port, portType);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdPortTypeGet(dev, port, &portType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                     */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdPortTypeGet(dev, port, &portType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    portType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdPortTypeGet(dev, port, &portType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdPortTypeGet(dev, port, &portType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdDevMapTableSet
(
    IN GT_U8                        devNum,
    IN GT_U8                        targetDevNum,
    IN CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    IN CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT srcPortTrunkHashEn
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdDevMapTableSet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with targetDevNum [1] and valid cascadeLinkPtr {1, CPSS_CSCD_LINK_TYPE_PORT_E}, srcPortTrunkHashEn [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E].
Expected: GT_OK.
1.2. Call cpssDxChCscdDevMapTableGet with targetDevNum [1] and non-NULL pointers.
Expected: GT_OK and cascadeLink the same as written (srcPortTrunkHashEn is not relevant in this case).
1.3. Call function with targetDevNum [1] and valid cascadeLinkPtr {2, CPSS_CSCD_LINK_TYPE_TRUNK_E}, srcPortTrunkHashEn [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E/ CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E].
Expected: GT_OK.
1.4. Call cpssDxChCscdDevMapTableGet with targetDevNum [1] and non-NULL pointers.
Expected: GT_OK and cascadeLink  the same as written; srcPortTrunkHashEn should be the same as written for Cheetah2,3 devices.
1.5. Call function with out of range targetDevNum [32], and valid cascadeLinkPtr {50, CPSS_CSCD_LINK_TYPE_TRUNK_E}, srcPortTrunkHashEn [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E/ CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E].
Expected: NOT GT_OK.
1.6. Call function with targetDevNum [16], null cascadeLinkPtr [NULL], srcPorttrunkHashEn [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E].
Expected: GT_BAD_PTR.
1.7. Call function with targetDevNum [15] and out of range cascadeLinkPtr->linkNum (for trunk) cascadeLinkPtr {128, CPSS_CSCD_LINK_TYPE_TRUNK_E}, srcPorttrunkHashEn [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E].
Expected: NOT GT_OK.
1.8. Call function with targetDevNum [15] and out of range cascadeLinkPtr->linkNum (for port) cascadeLinkPtr {64, CPSS_CSCD_LINK_TYPE_PORT_E}, srcPorttrunkHashEn [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E].
Expected: NOT GT_OK.
1.9. Call function with targetDevNum [5] and out of range cascadeLinkPtr->linkType cascadeLinkPtr {1, wrong enum values}, srcPorttrunkHashEn [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E].
Expected: GT_BAD_PARAM.
1.10. Call function with targetDevNum [5] and cascadeLinkPtr {1, CPSS_CSCD_LINK_TYPE_PORT_E}, srcPorttrunkHashEn [wrong enum values] (important only for ascadeLinkPtr->linkType == CPSS_CSCD_LINK_TYPE_TRUNK_E).
Expected: GT_OK.
1.11. Call function with targetDevNum [5] and cascadeLinkPtr {1, CPSS_CSCD_LINK_TYPE_TRUNK_E}, srcPorttrunkHashEn [wrong enum values].
Expected: GT_BAD_PARAM for Cheetah2,3 and GT_OK for others (his parameter is relevant only to DXCH2,3 devices).
*/
    GT_STATUS   st     = GT_OK;
    GT_U32      index  = 0;

    GT_U8                               dev;
    GT_U8                               devNum = 0;
    CPSS_CSCD_LINK_TYPE_STC             cascadeLink;
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT  srcPortTrunkHashEn;

    CPSS_CSCD_LINK_TYPE_STC             cascadeLinkGet;
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT  srcPortTrunkHashEnGet;
    GT_BOOL                             isEqual;

    CPSS_PP_FAMILY_TYPE_ENT             devFamily;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /*
            1.1. Call function with targetDevNum [1]
                            and valid cascadeLinkPtr {1, CPSS_CSCD_LINK_TYPE_PORT_E}.
            Expected: GT_OK.
        */
        devNum               = 1;
        cascadeLink.linkNum  = 1;
        cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_PORT_E;
        /* for this link_type srcPortTrunkHashEn value is not important*/
        srcPortTrunkHashEn = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;

        st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "%d, %d, cascadeLinkPtr {linkNum = %d, linkType = %d}, %d",
                                     dev, devNum, cascadeLink.linkNum, cascadeLink.linkType, srcPortTrunkHashEn);

        /* 1.2. Call cpssDxChCscdDevMapTableGet with targetDevNum [1] and non-NULL pointers.
        Expected: GT_OK and cascadeLink the same as written
        (srcPortTrunkHashEn is not relevant in this case).*/
        st = cpssDxChCscdDevMapTableGet(dev, devNum, 0, &cascadeLinkGet, &srcPortTrunkHashEnGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChCscdDevMapTableGet: %d, %d", dev, devNum);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&cascadeLink, (GT_VOID*)&cascadeLinkGet, sizeof(cascadeLink)))
                  ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "get another cascadeLink than was set: %d, %d", dev, devNum);

        /*
            1.3. Call function with targetDevNum [1]
            and valid cascadeLinkPtr {2, CPSS_CSCD_LINK_TYPE_TRUNK_E},
            srcPortTrunkHashEn [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E/
                                CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E].
            Expected: GT_OK.
        */

        /*  1.4. Call cpssDxChCscdDevMapTableGet with targetDevNum [1] and non-NULL pointers.
            Expected: GT_OK and cascadeLink  the same as written; srcPortTrunkHashEn should be
            the same as written for Cheetah2,3 devices.
        */

        /*  1.3. for srcPortTrunkHashEn == CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E */
        cascadeLink.linkNum  = 2;
        cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;
        /* srcPortTrunkHashEn = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E*/

        st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "%d, %d, cascadeLinkPtr {linkNum = %d, linkType = %d}, %d",
                                     dev, devNum, cascadeLink.linkNum, cascadeLink.linkType, srcPortTrunkHashEn);

        /*  1.4. for srcPortTrunkHashEn == CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E */
        st = cpssDxChCscdDevMapTableGet(dev, devNum, 0, &cascadeLinkGet, &srcPortTrunkHashEnGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChCscdDevMapTableGet: %d, %d", dev, devNum);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&cascadeLink, (GT_VOID*)&cascadeLinkGet, sizeof(cascadeLink)))
                  ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "get another cascadeLink than was set: %d, %d", dev, devNum);


        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(srcPortTrunkHashEn, srcPortTrunkHashEnGet,
                                         "get another srcPortTrunkHashEn than was set: %d, %d", dev, devNum);
        }

        /*  1.3. for srcPortTrunkHashEn == CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E */
        srcPortTrunkHashEn = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E;

        st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "%d, %d, cascadeLinkPtr {linkNum = %d, linkType = %d}, %d",
                                     dev, devNum, cascadeLink.linkNum, cascadeLink.linkType, srcPortTrunkHashEn);

        /*  1.4. for srcPortTrunkHashEn == CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E */
        st = cpssDxChCscdDevMapTableGet(dev, devNum, 0, &cascadeLinkGet, &srcPortTrunkHashEnGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxChCscdDevMapTableGet: %d, %d", dev, devNum);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&cascadeLink, (GT_VOID*)&cascadeLinkGet, sizeof(cascadeLink)))
                  ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, "get another cascadeLink than was set: %d, %d", dev, devNum);


        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(srcPortTrunkHashEn, srcPortTrunkHashEnGet,
                                         "get another srcPortTrunkHashEn than was set: %d, %d", dev, devNum);
        }


       /* for further tests 1.5-1.9 in This func the value of srcPortTrunkHashEn have
           no real impact*/

        /*
            1.5. Call function with out of range targetDevNum [32],
                            and valid cascadeLinkPtr {50, CPSS_CSCD_LINK_TYPE_TRUNK_E},
            srcPortTrunkHashEn [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E/
                                CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E].
            Expected: NOT GT_OK.
        */
        devNum               = 32;
        cascadeLink.linkNum  = 50;
        cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;
        /* srcPortTrunkHashEn = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E */

        st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, devNum);

        srcPortTrunkHashEn = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;

        st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, &cascadeLink,srcPortTrunkHashEn);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, devNum);

        /*
            1.6. Call function with targetDevNum [16],
                            null cascadeLinkPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        devNum = 16;

        st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, NULL, srcPortTrunkHashEn);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cascadeLinkPtr = NULL", dev);

        /*
            1.7. Call function with targetDevNum [15]
                            and out of range cascadeLinkPtr->linkNum (for trunk)
                            cascadeLinkPtr {128, CPSS_CSCD_LINK_TYPE_TRUNK_E}.
            Expected: NOT GT_OK.
        */
        devNum               = 15;
        cascadeLink.linkNum  = 128;
        cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;

        st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, cascadeLinkPtr->linkNum = %d, "
                                                    "cascadeLinkPtr->linkType = %d",
                                         dev, cascadeLink.linkNum, cascadeLink.linkType);

        /*
            1.8. Call function with targetDevNum [15]
                             and out of range cascadeLinkPtr->linkNum (for port)
                             cascadeLinkPtr {64, CPSS_CSCD_LINK_TYPE_PORT_E}.
            Expected: NOT GT_OK.
        */
        cascadeLink.linkNum  = 64;
        cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_PORT_E;

        st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, cascadeLinkPtr->linkNum = %d, "
                                                    "cascadeLinkPtr->linkType = %d",
                                         dev, cascadeLink.linkNum, cascadeLink.linkType);

        /*
            1.9. Call function with targetDevNum [5]
                            and out of range cascadeLinkPtr->linkType
                            cascadeLinkPtr {1, wrong enum values}.
            Expected: GT_BAD_PARAM.
        */
        devNum               = 5;
        cascadeLink.linkNum  = 1;

        UTF_ENUMS_CHECK_MAC(cpssDxChCscdDevMapTableSet
                            (dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn),
                            cascadeLink.linkType);

        /*
            1.10. Call function with targetDevNum [5] 
                  and cascadeLinkPtr {1, CPSS_CSCD_LINK_TYPE_PORT_E},
                  srcPorttrunkHashEn [wrong enum values] 
                  (important only for ascadeLinkPtr->linkType == CPSS_CSCD_LINK_TYPE_TRUNK_E).
            Expected: GT_OK.
        */
        devNum               = 5;
        cascadeLink.linkNum  = 1;
        cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_PORT_E;
        
        for(index = 0; index < utfInvalidEnumArrSize; index++)
        {
            srcPortTrunkHashEn = utfInvalidEnumArr[index];

            st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                            "%d, cascadeLinkPtr->linkType = %d, srcPortTrunkHashEn = %d",
                                         dev, cascadeLink.linkType, srcPortTrunkHashEn);
        }

        /* 
            1.11. Call function with targetDevNum [5] and 
                  cascadeLinkPtr {1, CPSS_CSCD_LINK_TYPE_TRUNK_E}, 
                  wrong enum values srcPortTrunkHashEn.
            Expected: GT_BAD_PARAM for Cheetah2 and GT_OK for others
                        (this parameter is relevant only to DXCH2,3 devices). 
        */
        cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;

        if (devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            UTF_ENUMS_CHECK_MAC(cpssDxChCscdDevMapTableSet
                                (dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn),
                                srcPortTrunkHashEn);
        }
        else
        {
            for(index = 0; index < utfInvalidEnumArrSize; index++)
            {
                srcPortTrunkHashEn = utfInvalidEnumArr[index];
                st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn);

                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                        "non Cheetah2: %d, cascadeLinkPtr->linkType = %d, srcPortTrunkHashEn = %d",
                                            dev, cascadeLink.linkType, srcPortTrunkHashEn);
            }
        }
    }

    devNum               = 1;
    cascadeLink.linkNum  = 1;
    cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_PORT_E;
    srcPortTrunkHashEn = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdDevMapTableSet(dev, devNum, 0, &cascadeLink, srcPortTrunkHashEn);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdDevMapTableGet
(
    IN GT_U8                        devNum,
    IN GT_U8                        targetDevNum,
    OUT CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    OUT CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT *srcPortTrunkHashEnPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdDevMapTableGet)
{
/*
ITERATE_DEVICES (DxChx)
1.1. Call function with targetDevNum [1] and non-NULL pointers.
Expected: GT_OK.
1.2. Call function with out of range targetDevNum [32], and non-NULL pointers.
Expected: NOT GT_OK.
1.3. Call function with targetDevNum [1], cascadeLinkPtr [NULL] and non-NULL srcPortTrunkHashEnPtr.
Expected: GT_BAD_PTR.
1.4. Call function with targetDevNum [1], non-NULL cascadeLinkPtr and srcPortTrunkHashEnPtr [NULL].
Expected: GT_BAD_PTR.
*/
    GT_STATUS               st     = GT_OK;

    GT_U8                               dev;
    GT_U8                               devNum = 0;
    CPSS_CSCD_LINK_TYPE_STC             cascadeLinkGet;
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT  srcPortTrunkHashEnGet;

    CPSS_PP_FAMILY_TYPE_ENT             devFamily;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        /* 1.1. Call function with targetDevNum [1] and non-NULL pointers.
            Expected: GT_OK. */
        devNum = 1;

        st = cpssDxChCscdDevMapTableGet(dev, devNum, 0, &cascadeLinkGet, &srcPortTrunkHashEnGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, devNum);

        /* 1.2. Call function with out of range targetDevNum [32], and non-NULL pointers.
            Expected: NOT GT_OK. */
        devNum = 32;

        st = cpssDxChCscdDevMapTableGet(dev, devNum, 0, &cascadeLinkGet, &srcPortTrunkHashEnGet);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, devNum);

        devNum = 1; /* restore */

        /* 1.3. Call function with targetDevNum [1], cascadeLinkPtr [NULL] and non-NULL srcPortTrunkHashEnPtr.
            Expected: GT_BAD_PTR. */
        st = cpssDxChCscdDevMapTableGet(dev, devNum, 0, NULL, &srcPortTrunkHashEnGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, devNum);

        /* 1.4. Call function with targetDevNum [1], non-NULL cascadeLinkPtr and srcPortTrunkHashEnPtr [NULL].
            Expected: GT_BAD_PTR for Ch2 and GT_OK for Ch (this parameter is not relevant for Ch). */
        st = cpssDxChCscdDevMapTableGet(dev, devNum, 0, &cascadeLinkGet, NULL);

        if(CPSS_PP_FAMILY_CHEETAH_E != devFamily)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, srcPortTrunkHashEnGetPtr = NULL", dev, devNum);
        }
        else
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Cheetah: %d, %d, srcPortTrunkHashEnGetPtr = NULL", dev, devNum);
        }
    }

    devNum = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdDevMapTableGet(dev, devNum, 0, &cascadeLinkGet, &srcPortTrunkHashEnGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdDevMapTableGet(dev, devNum, 0, &cascadeLinkGet, &srcPortTrunkHashEnGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdRemapQosModeSet
(
   IN GT_U8                             devNum,
   IN GT_U8                             portNum,
   IN CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT remapType
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdRemapQosModeSet)
{
/*
    ITERATE_DEVICES_PHY_ PORTS (DxChx)
    1.1.1. Call with remapType [CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E /
                                CPSS_DXCH_CSCD_QOS_REMAP_CNTRL_ONLY_E /
                                CPSS_DXCH_CSCD_QOS_REMAP_DATA_ONLY_E /
                                CPSS_DXCH_CSCD_QOS_REMAP_ALL_E].
    Expected: GT_OK.
    1.1.2. Call with wrong enum values remapType .
    Expected: GT_BAD_PARAM.
    1.2. Call with port [CPSS_CPU_PORT_NUM_CNS = 63]
                   and valid other parameters.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                         st        = GT_OK;

    GT_U8                             dev;
    GT_U8                             port      = CSCD_VALID_PHY_PORT_CNS;
    CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT remapType = CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with remapType [CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E /
                                CPSS_DXCH_CSCD_QOS_REMAP_CNTRL_ONLY_E /
                                CPSS_DXCH_CSCD_QOS_REMAP_DATA_ONLY_E /
                                CPSS_DXCH_CSCD_QOS_REMAP_ALL_E].
                Expected: GT_OK.
            */

            /* Call function with remapType [CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E] */
            remapType = CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E;

            st = cpssDxChCscdRemapQosModeSet(dev, port, remapType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, remapType);

            /* Call function with remapType [CPSS_DXCH_CSCD_QOS_REMAP_CNTRL_ONLY_E] */
            remapType = CPSS_DXCH_CSCD_QOS_REMAP_CNTRL_ONLY_E;

            st = cpssDxChCscdRemapQosModeSet(dev, port, remapType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, remapType);

            /* Call function with remapType [CPSS_DXCH_CSCD_QOS_REMAP_DATA_ONLY_E] */
            remapType = CPSS_DXCH_CSCD_QOS_REMAP_DATA_ONLY_E;

            st = cpssDxChCscdRemapQosModeSet(dev, port, remapType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, remapType);

            /* Call function with remapType [CPSS_DXCH_CSCD_QOS_REMAP_ALL_E] */
            remapType = CPSS_DXCH_CSCD_QOS_REMAP_ALL_E;

            st = cpssDxChCscdRemapQosModeSet(dev, port, remapType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, remapType);

            /*
                1.1.2. Call with wrong enum values remapType.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCscdRemapQosModeSet
                                (dev, port, remapType),
                                remapType);
        }

        remapType = CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdRemapQosModeSet(dev, port, remapType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdRemapQosModeSet(dev, port, remapType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdRemapQosModeSet(dev, port, remapType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    remapType = CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdRemapQosModeSet(dev, port, remapType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdRemapQosModeSet(dev, port, remapType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdCtrlQosSet
(
   IN GT_U8             devNum,
   IN GT_U8             ctrlTc,
   IN CPSS_DP_LEVEL_ENT ctrlDp,
   IN CPSS_DP_LEVEL_ENT cpuToCpuDp
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdCtrlQosSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with ctrlTc [1, 2],
                   ctrlTc [CPSS_DP_GREEN_E /
                           CPSS_DP_RED_E]
                   and cpuToCpuDp [CPSS_DP_GREEN_E /
                                   CPSS_DP_RED_E].
    Expected: GT_OK.
    1.2. Call with out of range ctrlTc [CPSS_TC_RANGE_CNS = 8],
                   ctrlTc [CPSS_DP_GREEN_E]
                   and cpuToCpuDp [CPSS_DP_RED_E].
    Expected: GT_BAD_PARAM.
    1.3. Call with tc [1],
                   wrong enum values ctrlTc 
                   and cpuToCpuDp [CPSS_DP_GREEN_E].
    Expected: GT_BAD_PARAM.
    1.4. Call with tc [2],
                   ctrlTc [CPSS_DP_GREEN_E]
                   and wrong enum values cpuToCpuDp .
    Expected: GT_BAD_PARAM.
    1.5. Call with ctrlTc [3],
                   ctrlTc [CPSS_DP_YELLOW_E]
                   and cpuToCpuDp [CPSS_DP_GREEN_E].
    Expected: NOT GT_OK for DxCh.
    1.6. Call with ctrlTc [4],
                   ctrlTc [CPSS_DP_GREEN_E]
                   and cpuToCpuDp [CPSS_DP_YELLOW_E].
    Expected: NOT GT_OK for DxCh.
*/
    GT_STATUS               st         = GT_OK;

    GT_U8                   dev;
    GT_U8                   ctrlTc     = 0;
    CPSS_DP_LEVEL_ENT       ctrlDp     = CPSS_DP_GREEN_E;
    CPSS_DP_LEVEL_ENT       cpuToCpuDp = CPSS_DP_GREEN_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ctrlTc [1, 2],
                           ctrlDp [CPSS_DP_GREEN_E /
                                   CPSS_DP_RED_E]
                            and cpuToCpuDp [CPSS_DP_GREEN_E /
                                            CPSS_DP_RED_E].
            Expected: GT_OK.
        */

        /* Call with ctrlTc [1], ctrlDp [CPSS_DP_GREEN_E], cpuToCpuDp [CPSS_DP_GREEN_E] */
        ctrlTc     = 1;
        ctrlDp     = CPSS_DP_GREEN_E;
        cpuToCpuDp = CPSS_DP_GREEN_E;

        st = cpssDxChCscdCtrlQosSet(dev, ctrlTc, ctrlDp, cpuToCpuDp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ctrlTc, ctrlDp, cpuToCpuDp);

        /* Call with ctrlTc [2], ctrlDp [CPSS_DP_RED_E], cpuToCpuDp [CPSS_DP_RED_E] */
        ctrlTc     = 2;
        ctrlDp     = CPSS_DP_RED_E;
        cpuToCpuDp = CPSS_DP_RED_E;

        st = cpssDxChCscdCtrlQosSet(dev, ctrlTc, ctrlDp, cpuToCpuDp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ctrlTc, ctrlDp, cpuToCpuDp);

        /*
            1.2. Call with out of range ctrlTc [CPSS_TC_RANGE_CNS = 8],
                   ctrlDp [CPSS_DP_GREEN_E]
                   and cpuToCpuDp [CPSS_DP_RED_E].
            Expected: GT_BAD_PARAM.
        */
        ctrlTc     = CPSS_TC_RANGE_CNS;
        ctrlDp     = CPSS_DP_GREEN_E;

        st = cpssDxChCscdCtrlQosSet(dev, ctrlTc, ctrlDp, cpuToCpuDp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, ctrlTc);

        /*
            1.3. Call with tc [1], wrong enum values ctrlTc 
                           and cpuToCpuDp [CPSS_DP_GREEN_E].
            Expected: GT_BAD_PARAM.
        */
        ctrlTc     = 1;
        cpuToCpuDp = CPSS_DP_GREEN_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChCscdCtrlQosSet
                            (dev, ctrlTc, ctrlDp, cpuToCpuDp),
                            ctrlDp);

        /*
            1.4. Call with tc [2], ctrlTc [CPSS_DP_GREEN_E]
                           and wrong enum values cpuToCpuDp.
            Expected: GT_BAD_PARAM.
        */
        ctrlTc     = 2;
        ctrlDp     = CPSS_DP_GREEN_E;

        UTF_ENUMS_CHECK_MAC(cpssDxChCscdCtrlQosSet
                            (dev, ctrlTc, ctrlDp, cpuToCpuDp),
                            cpuToCpuDp);

        /*
            1.5. Call with ctrlTc [3],
                   ctrlTc [CPSS_DP_YELLOW_E]
                   and cpuToCpuDp [CPSS_DP_GREEN_E].
            Expected: NOT GT_OK for DxCh.
        */
        ctrlTc     = 3;
        ctrlDp     = CPSS_DP_YELLOW_E;
        cpuToCpuDp = CPSS_DP_GREEN_E;

        st = cpssDxChCscdCtrlQosSet(dev, ctrlTc, ctrlDp, cpuToCpuDp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ctrlDp = %d", dev, ctrlDp);

        /*
            1.6. Call with ctrlTc [4],
                   ctrlTc [CPSS_DP_GREEN_E]
                   and cpuToCpuDp [CPSS_DP_YELLOW_E].
            Expected: NOT GT_OK for DxCh.
        */
        ctrlTc     = 4;
        ctrlDp     = CPSS_DP_GREEN_E;
        cpuToCpuDp = CPSS_DP_YELLOW_E;

        st = cpssDxChCscdCtrlQosSet(dev, ctrlTc, ctrlDp, cpuToCpuDp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cpuToCpuDp = %d", dev, cpuToCpuDp);
    }

    ctrlTc     = 1;
    ctrlDp     = CPSS_DP_GREEN_E;
    cpuToCpuDp = CPSS_DP_GREEN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdCtrlQosSet(dev, ctrlTc, ctrlDp, cpuToCpuDp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdCtrlQosSet(dev, ctrlTc, ctrlDp, cpuToCpuDp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdRemapDataQosTblSet
(
   IN GT_U8              devNum,
   IN GT_U8              tc,
   IN CPSS_DP_LEVEL_ENT  dp,
   IN GT_U8              remapTc,
   IN CPSS_DP_LEVEL_ENT  remapDp
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdRemapDataQosTblSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with tc [1, 2],
                   dp [CPSS_DP_GREEN_E /
                       CPSS_DP_RED_E]
                   remapTc [3, 4]
                   and remapDp [CPSS_DP_GREEN_E /
                                CPSS_DP_RED_E].
    Expected: GT_OK.
    1.2. Call with out of range tc [CPSS_TC_RANGE_CNS = 8],
                   dp [CPSS_DP_GREEN_E],
                   remapTc [5]
                   and remapDp [CPSS_DP_RED_E].
    Expected: GT_BAD_PARAM.
    1.3. Call with tc [1],
                   dp [CPSS_DP_RED_E],
                   out of range remapTc [CPSS_TC_RANGE_CNS = 8]
                   and remapDp [CPSS_DP_GREEN_E].
    Expected: GT_BAD_PARAM.
    1.4. Call with tc [1],
                   wrong enum values dp 
                   remapTc[2]
                   and remapDp [CPSS_DP_GREEN_E].
    Expected: GT_BAD_PARAM.
    1.5. Call with tc [2],
                   dp [CPSS_DP_GREEN_E]
                   remapTc[4]
                   and wrong enum values remapDp .
    Expected: GT_BAD_PARAM.
    1.6. Call with tc [3],
                   dp [CPSS_DP_YELLOW_E]
                   remapTc [5]
                   and remapDp [CPSS_DP_GREEN_E].
    Expected: NOT GT_OK for DxCh.
    1.7. Call with tc [4],
                   dp [CPSS_DP_GREEN_E]
                   remapTc [6]
                   and remapDp [CPSS_DP_YELLOW_E].
    Expected: NOT GT_OK for DxCh.
*/
    GT_STATUS               st         = GT_OK;

    GT_U8                   dev;
    GT_U8                   tc      = 0;
    CPSS_DP_LEVEL_ENT       dp      = CPSS_DP_GREEN_E;
    GT_U8                   remapTc = 0;
    CPSS_DP_LEVEL_ENT       remapDp = CPSS_DP_GREEN_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with tc [1, 2],
                           dp [CPSS_DP_GREEN_E /
                               CPSS_DP_RED_E]
                            remapTc [3, 4]
                            and remapDp [CPSS_DP_GREEN_E /
                                         CPSS_DP_RED_E].
            Expected: GT_OK.
        */

        /* Call with tc [1], dp [CPSS_DP_GREEN_E] */
        /* remapTc [3], remapDp [CPSS_DP_GREEN_E] */
        tc      = 1;
        dp      = CPSS_DP_GREEN_E;
        remapTc = 3;
        remapDp = CPSS_DP_GREEN_E;

        st = cpssDxChCscdRemapDataQosTblSet(dev, tc, dp, remapTc, remapDp);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, tc, dp, remapTc, remapDp);

        /* Call with tc [2], dp [CPSS_DP_RED_E] */
        /* remapTc [4], remapDp [CPSS_DP_RED_E] */
        tc      = 2;
        dp      = CPSS_DP_RED_E;
        remapTc = 4;
        remapDp = CPSS_DP_RED_E;

        st = cpssDxChCscdRemapDataQosTblSet(dev, tc, dp, remapTc, remapDp);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, tc, dp, remapTc, remapDp);

        /*
            1.2. Call with out of range tc [CPSS_TC_RANGE_CNS = 8],
                           dp [CPSS_DP_GREEN_E],
                           remapTc [5]
                           and remapDp [CPSS_DP_RED_E].
            Expected: GT_BAD_PARAM.
        */
        tc      = CPSS_TC_RANGE_CNS;
        dp      = CPSS_DP_GREEN_E;
        remapTc = 5;

        st = cpssDxChCscdRemapDataQosTblSet(dev, tc, dp, remapTc, remapDp);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, tc);

        /*
            1.3. Call with tc [1],
                           dp [CPSS_DP_RED_E],
                           out of range remapTc [CPSS_TC_RANGE_CNS = 8]
                           and remapDp [CPSS_DP_GREEN_E].
            Expected: GT_BAD_PARAM.
        */
        tc      = 1;
        dp      = CPSS_DP_RED_E;
        remapTc = CPSS_TC_RANGE_CNS;
        remapDp = CPSS_DP_GREEN_E;

        st = cpssDxChCscdRemapDataQosTblSet(dev, tc, dp, remapTc, remapDp);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, remapTc = %d", dev, remapTc);

        /*
            1.4. Call with tc [1], wrong enum values dp 
                           remapTc[2] and remapDp [CPSS_DP_GREEN_E].
            Expected: GT_BAD_PARAM.
        */
        remapTc = 2;

        UTF_ENUMS_CHECK_MAC(cpssDxChCscdRemapDataQosTblSet
                            (dev, tc, dp, remapTc, remapDp),
                            remapDp);

        /*
            1.5. Call with tc [2], dp [CPSS_DP_GREEN_E], remapTc[4]
                   and wrong enum values remapDp.
            Expected: GT_BAD_PARAM.
        */
        tc      = 2;
        dp      = CPSS_DP_GREEN_E;
        remapTc = 4;

        UTF_ENUMS_CHECK_MAC(cpssDxChCscdRemapDataQosTblSet
                            (dev, tc, dp, remapTc, remapDp),
                            remapDp);

        /*
            1.6. Call with tc [3], dp [CPSS_DP_YELLOW_E], remapTc [5]
                           and remapDp [CPSS_DP_GREEN_E].
            Expected: NOT GT_OK for DxCh.
        */
        tc      = 3;
        dp      = CPSS_DP_YELLOW_E;
        remapTc = 5;
        remapDp = CPSS_DP_GREEN_E;

        st = cpssDxChCscdRemapDataQosTblSet(dev, tc, dp, remapTc, remapDp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dp = %d", dev, dp);

        /*
            1.7. Call with tc [4],
                           dp [CPSS_DP_GREEN_E]
                           remapTc [6]
                           and remapDp [CPSS_DP_YELLOW_E].
            Expected: NOT GT_OK for DxCh.
        */
        tc      = 4;
        dp      = CPSS_DP_GREEN_E;
        remapTc = 6;
        remapDp = CPSS_DP_YELLOW_E;

        st = cpssDxChCscdRemapDataQosTblSet(dev, tc, dp, remapTc, remapDp);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, remapDp = %d", dev, remapDp);
    }

    tc      = 1;
    dp      = CPSS_DP_GREEN_E;
    remapTc = 3;
    remapDp = CPSS_DP_GREEN_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdRemapDataQosTblSet(dev, tc, dp, remapTc, remapDp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdRemapDataQosTblSet(dev, tc, dp, remapTc, remapDp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdDsaSrcDevFilterSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enableOwnDevFltr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdDsaSrcDevFilterSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with enableOwnDevFltr [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
*/
    GT_STATUS   st     = GT_OK;

    GT_U8       dev;
    GT_BOOL     state = GT_TRUE;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enableOwnDevFltr [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call function with enableOwnDevFltr [GT_FALSE] */
        state = GT_FALSE;

        st = cpssDxChCscdDsaSrcDevFilterSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /* Call function with enableOwnDevFltr [GT_TRUE] */
        state = GT_TRUE;

        st = cpssDxChCscdDsaSrcDevFilterSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdDsaSrcDevFilterSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdDsaSrcDevFilterSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdHyperGInternalPortModeSet
(
    IN  GT_U8                                devNum,
    IN  GT_U8                                portNum,
    IN  CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdHyperGInternalPortModeSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh1 DxCh2)
    1.1.1. Call with mode [CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E /
                           CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E].
    Expected: GT_OK for XG ports and GT_NOT_SUPPORTED for others.
    1.1.2. Call with wrong enum values mode .
    Expected: GT_BAD_PARAM.
    1.2. Call with port [CPSS_CPU_PORT_NUM_CNS = 63] and valid other parameters.
    Expected: NOT GT_OK.
*/
    GT_STATUS                            st   = GT_OK;

    GT_U8                                dev;
    GT_U8                                port = CSCD_VALID_PHY_PORT_CNS;
    CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT mode = CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E;
    PRV_CPSS_PORT_TYPE_ENT               portType;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d",
                                         dev, port);

            /*
                1.1.1. Call with mode [CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E /
                                       CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E].
                Expected: GT_OK for XG ports and GT_NOT_SUPPORTED for others.
            */

            /* Call with mode [CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E] */
            mode = CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E;

            st = cpssDxChCscdHyperGInternalPortModeSet(dev, port, mode);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, mode);
            }

            /* Call with mode [CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E] */
            mode = CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E;

            st = cpssDxChCscdHyperGInternalPortModeSet(dev, port, mode);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, mode);
            }

            /*
                1.1.2. Call with wrong enum values mode .
                Expected: GT_BAD_PARAM.
            */
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_ENUMS_CHECK_MAC(cpssDxChCscdHyperGInternalPortModeSet
                                    (dev, port, mode),
                                    mode);
            }
        }

        mode = CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E;

        /* 1.2. For all active devices go over all non available
           physical ports.
        */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdHyperGInternalPortModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdHyperGInternalPortModeSet(dev, port, mode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* 1.4. For active device check that function returns NOT GT_OK */
        /* for CPU port number.                                         */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdHyperGInternalPortModeSet(dev, port, mode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    mode = CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E;
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdHyperGInternalPortModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdHyperGInternalPortModeSet(dev, port, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

#if 0 /*function is deleted in the new version - headers were sent by Radlan 19.06.06 */

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdUcRouteTrunkHashTypeSet
(
    IN GT_U8                              devNum,
    IN GT_U8                              targetDevNum,
    IN CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT hashType
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdUcRouteTrunkHashTypeSet)
{
/*
    ITERATE_DEVICES (DxCh2)
    1.1. Call with targetDevNum [1]
                   and hashType [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E /
                                 CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E].
    Expected: GT_OK.
    1.2. Call with targetDevNum [32] (no any constraints in function's contract)
                   and hashType [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E].
    Expected: GT_OK.
    1.3. Call with targetDevNum [2]
                   and wrong enum values hashType .
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                          st    = GT_OK;

    GT_U8                              dev;
    GT_U8                              devNum = 0;
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT hashType;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with targetDevNum [1]
                   and hashType [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E /
                                 CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E].
            Expected: GT_OK.
        */

        /* Call with targetDevNum [1]                                    */
        /* hashType [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E] */
        devNum   = 1;
        hashType = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;

        st = cpssDxChCscdUcRouteTrunkHashTypeSet(dev, devNum, hashType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, devNum, hashType);

        /* Call with targetDevNum [1]                              */
        /* hashType [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E] */
        hashType = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E;

        st = cpssDxChCscdUcRouteTrunkHashTypeSet(dev, devNum, hashType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, devNum, hashType);

        /*
            1.2. Call with targetDevNum [32] (no any constraints in function's contract)
                           and hashType [CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E].
            Expected: GT_OK.
        */
        devNum   = 32;
        hashType = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;

        st = cpssDxChCscdUcRouteTrunkHashTypeSet(dev, devNum, hashType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, devNum, hashType);

        /*
            1.3. Call with targetDevNum [2]
                   and wrong enum values hashType .
            Expected: GT_BAD_PARAM.
        */
        devNum   = 2;
        hashType = CSCD_INVALID_ENUM_CNS;

        st = cpssDxChCscdUcRouteTrunkHashTypeSet(dev, devNum, hashType);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, hashType = %d",
                                     dev, hashType);
    }

    devNum   = 1;
    hashType = CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH3_E | UTF_XCAT_E | UTF_LION_E | UTF_XCAT2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdUcRouteTrunkHashTypeSet(dev, devNum, hashType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdUcRouteTrunkHashTypeSet(dev, devNum, hashType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
#endif

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdHyperGPortCrcModeSet
(
    IN GT_U8                             devNum,
    IN GT_U8                             portNum,
    IN CPSS_PORT_DIRECTION_ENT           portDirection,
    IN CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT  crcMode
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdHyperGPortCrcModeSet)
{
/*
    ITERATE_DEVICES_PHY_ PORTS (DxCh2 and above)
    1.1.1. Call with portDirection [CPSS_PORT_DIRECTION_RX_E /
                                    CPSS_PORT_DIRECTION_TX_E /
                                    CPSS_PORT_DIRECTION_BOTH_E]
                     and crcMode [CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E /
                                  CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E].
    Expected: GT_OK.
    1.1.2. Call with wrong enum values portDirection 
                     and crcMode [CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E].
    Expected: GT_BAD_PARAM.
    1.1.3. Call with portDirection [CPSS_PORT_DIRECTION_RX_E]
                     and wrong enum values crcMode .
    Expected: GT_BAD_PARAM.
    1.2. Call with port [CPSS_CPU_PORT_NUM_CNS = 63] and valid other parameters.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS                        st        = GT_OK;

    GT_U8                            dev;
    GT_U8                            port      = CSCD_VALID_PHY_PORT_CNS;
    CPSS_PORT_DIRECTION_ENT          direction = CPSS_PORT_DIRECTION_RX_E;
    CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT mode      = CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with portDirection [CPSS_PORT_DIRECTION_RX_E /
                                                CPSS_PORT_DIRECTION_TX_E /
                                                CPSS_PORT_DIRECTION_BOTH_E]
                                 and crcMode [CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E /
                                              CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E].
                Expected: GT_OK.
            */

            /* Call with portDirection [CPSS_PORT_DIRECTION_RX_E] */
            /* crcMode [CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E]       */
            direction = CPSS_PORT_DIRECTION_RX_E;
            mode      = CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E;

            st = cpssDxChCscdHyperGPortCrcModeSet(dev, port, direction, mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, mode);

            /* Call with portDirection [CPSS_PORT_DIRECTION_TX_E] */
            /* crcMode [CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E]       */
            direction = CPSS_PORT_DIRECTION_TX_E;
            mode      = CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E;

            st = cpssDxChCscdHyperGPortCrcModeSet(dev, port, direction, mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, mode);

            /* Call with portDirection [CPSS_PORT_DIRECTION_BOTH_E] */
            /* crcMode [CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E]       */
            direction = CPSS_PORT_DIRECTION_BOTH_E;
            mode      = CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E;

            st = cpssDxChCscdHyperGPortCrcModeSet(dev, port, direction, mode);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, mode);

            /*
                1.1.2. Call with wrong enum values portDirection 
                                 and crcMode [CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCscdHyperGPortCrcModeSet
                                (dev, port, direction, mode),
                                direction);

            /*
                1.1.3. Call with portDirection [CPSS_PORT_DIRECTION_RX_E]
                     and wrong enum values crcMode.
                Expected: GT_BAD_PARAM.
            */
            direction = CPSS_PORT_DIRECTION_RX_E;

            UTF_ENUMS_CHECK_MAC(cpssDxChCscdHyperGPortCrcModeSet
                                (dev, port, direction, mode),
                                mode);
        }

        direction = CPSS_PORT_DIRECTION_RX_E;
        mode      = CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdHyperGPortCrcModeSet(dev, port, direction, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdHyperGPortCrcModeSet(dev, port, direction, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdHyperGPortCrcModeSet(dev, port, direction, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    direction = CPSS_PORT_DIRECTION_RX_E;
    mode      = CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E;
    port      = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdHyperGPortCrcModeSet(dev, port, direction, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdHyperGPortCrcModeSet(dev, port, direction, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdFastFailoverFastStackRecoveryEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdFastFailoverFastStackRecoveryEnableSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChCscdFastFailoverFastStackRecoveryEnableGet with non-null enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssDxChCscdFastFailoverFastStackRecoveryEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChCscdFastFailoverFastStackRecoveryEnableGet with non-null enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChCscdFastFailoverFastStackRecoveryEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdFastFailoverFastStackRecoveryEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssDxChCscdFastFailoverFastStackRecoveryEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChCscdFastFailoverFastStackRecoveryEnableGet with non-null enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChCscdFastFailoverFastStackRecoveryEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdFastFailoverFastStackRecoveryEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdFastFailoverFastStackRecoveryEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdFastFailoverFastStackRecoveryEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdFastFailoverFastStackRecoveryEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdFastFailoverFastStackRecoveryEnableGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChCscdFastFailoverFastStackRecoveryEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCscdFastFailoverFastStackRecoveryEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdFastFailoverFastStackRecoveryEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdFastFailoverFastStackRecoveryEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdFastFailoverSecondaryTargetPortMapSet
(
    IN GT_U8        devNum,
    IN GT_U8        portNum,
    IN GT_U8        secondaryTargetPort
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdFastFailoverSecondaryTargetPortMapSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh3 and above)
    1.1.1. Call with secondaryTargetPort [0 / 31].
    Expected: GT_OK.
    1.1.2. Call cpssDxChCscdFastFailoverSecondaryTargetPortMapGet with non-null secondaryTargetPortPtr.
    Expected: GT_OK and the same secondaryTargetPort.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CSCD_VALID_PHY_PORT_CNS;

    GT_U8   secondaryTargetPort    = 0;
    GT_U8   secondaryTargetPortGet = 0;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with secondaryTargetPort [0 or "0" in the local port group for lion
                                                      / 31 or "15" in the local port group for lion].
                Expected: GT_OK.
            */

            /* Call with secondaryTargetPort [0 or "0" in the local port group for lion] */
            if( PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E )
            {
                secondaryTargetPort = (GT_U8) (port & 0xF0);
            }
            else
            {
                secondaryTargetPort = 0;
            }

            st = cpssDxChCscdFastFailoverSecondaryTargetPortMapSet(dev, port, secondaryTargetPort);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, secondaryTargetPort);

            /*
                1.1.2. Call cpssDxChCscdFastFailoverSecondaryTargetPortMapGet with non-null secondaryTargetPortPtr.
                Expected: GT_OK and the same secondaryTargetPort.
            */
            st = cpssDxChCscdFastFailoverSecondaryTargetPortMapGet(dev, port, &secondaryTargetPortGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdFastFailoverSecondaryTargetPortMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(secondaryTargetPort, secondaryTargetPortGet,
                       "get another secondaryTargetPort than was set: %d", dev, port);

            /* Call with secondaryTargetPort [31 or or "15" in the local port group for lion] */
            if( PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E )
            {
                secondaryTargetPort = (GT_U8) (port | 0xF);
                /* NULL port and above is not a valid one */
                if( secondaryTargetPort >= CPSS_NULL_PORT_NUM_CNS)
                    secondaryTargetPort = CPSS_NULL_PORT_NUM_CNS - 1;
            }
            else
            {
                secondaryTargetPort = 31;
            }

            st = cpssDxChCscdFastFailoverSecondaryTargetPortMapSet(dev, port, secondaryTargetPort);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, secondaryTargetPort);

            /*
                1.1.2. Call cpssDxChCscdFastFailoverSecondaryTargetPortMapGet with non-null secondaryTargetPortPtr.
                Expected: GT_OK and the same secondaryTargetPort.
            */
            st = cpssDxChCscdFastFailoverSecondaryTargetPortMapGet(dev, port, &secondaryTargetPortGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdFastFailoverSecondaryTargetPortMapGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(secondaryTargetPort, secondaryTargetPortGet,
                       "get another secondaryTargetPort than was set: %d", dev, port);
        }

        secondaryTargetPort = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdFastFailoverSecondaryTargetPortMapSet(dev, port, secondaryTargetPort);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdFastFailoverSecondaryTargetPortMapSet(dev, port, secondaryTargetPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdFastFailoverSecondaryTargetPortMapSet(dev, port, secondaryTargetPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    secondaryTargetPort = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdFastFailoverSecondaryTargetPortMapSet(dev, port, secondaryTargetPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdFastFailoverSecondaryTargetPortMapSet(dev, port, secondaryTargetPort);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdFastFailoverSecondaryTargetPortMapGet
(
    IN  GT_U8        devNum,
    IN  GT_U8        portNum,
    OUT GT_U8        *secondaryTargetPortPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdFastFailoverSecondaryTargetPortMapGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh3 and above)
    1.1.1. Call with non-NULL secondaryTargetPortPtr.
    Expected: GT_OK.
    1.1.2. Call with secondaryTargetPortPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CSCD_VALID_PHY_PORT_CNS;

    GT_U8   secondaryTargetPort = 0;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL secondaryTargetPortPtr.
                Expected: GT_OK.
            */
            st = cpssDxChCscdFastFailoverSecondaryTargetPortMapGet(dev, port, &secondaryTargetPort);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with secondaryTargetPortPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCscdFastFailoverSecondaryTargetPortMapGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, secondaryTargetPortPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdFastFailoverSecondaryTargetPortMapGet(dev, port, &secondaryTargetPort);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdFastFailoverSecondaryTargetPortMapGet(dev, port, &secondaryTargetPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdFastFailoverSecondaryTargetPortMapGet(dev, port, &secondaryTargetPort);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdFastFailoverSecondaryTargetPortMapGet(dev, port, &secondaryTargetPort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdFastFailoverSecondaryTargetPortMapGet(dev, port, &secondaryTargetPort);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet with non-null enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet with non-null enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet with non-null enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet)
{
/*
    ITERATE_DEVICES (DxCh3 and above)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdFastFailoverPortIsLoopedSet
(
  IN  GT_U8      devNum,
  IN  GT_U8      portNum,
  IN GT_BOOL     isLooped
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdFastFailoverPortIsLoopedSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh3 and above)
    1.1.1. Call with isLooped [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChCscdFastFailoverPortIsLoopedGet with non-null isLoopedPtr.
    Expected: GT_OK and the same isLooped.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CSCD_VALID_PHY_PORT_CNS;

    GT_BOOL   isLooped    = GT_FALSE;
    GT_BOOL   isLoopedGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with isLooped [GT_FALSE and GT_TRUE].
                Expected: GT_OK.
            */

            /* Call with isLooped [GT_FALSE] */
            isLooped = GT_FALSE;

            st = cpssDxChCscdFastFailoverPortIsLoopedSet(dev, port, isLooped);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, isLooped);

            /*
                1.1.2. Call cpssDxChCscdFastFailoverPortIsLoopedGet with non-null isLoopedPtr.
                Expected: GT_OK and the same isLooped.
            */
            st = cpssDxChCscdFastFailoverPortIsLoopedGet(dev, port, &isLoopedGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdFastFailoverPortIsLoopedGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(isLooped, isLoopedGet,
                       "get another isLooped than was set: %d", dev, port);

            /* Call with isLooped [GT_TRUE] */
            isLooped = GT_TRUE;

            st = cpssDxChCscdFastFailoverPortIsLoopedSet(dev, port, isLooped);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, isLooped);

            /*
                1.1.2. Call cpssDxChCscdFastFailoverPortIsLoopedGet with non-null isLoopedPtr.
                Expected: GT_OK and the same isLooped.
            */
            st = cpssDxChCscdFastFailoverPortIsLoopedGet(dev, port, &isLoopedGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdFastFailoverPortIsLoopedGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(isLooped, isLoopedGet,
                       "get another isLooped than was set: %d", dev, port);
        }

        isLooped = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdFastFailoverPortIsLoopedSet(dev, port, isLooped);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdFastFailoverPortIsLoopedSet(dev, port, isLooped);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdFastFailoverPortIsLoopedSet(dev, port, isLooped);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    isLooped = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdFastFailoverPortIsLoopedSet(dev, port, isLooped);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdFastFailoverPortIsLoopedSet(dev, port, isLooped);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdFastFailoverPortIsLoopedGet
(
  IN GT_U8      devNum,
  IN GT_U8      portNum,
  OUT GT_BOOL    *isLoopedPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdFastFailoverPortIsLoopedGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh3 and above)
    1.1.1. Call with non-NULL isLoopedPtr.
    Expected: GT_OK.
    1.1.2. Call with isLoopedPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CSCD_VALID_PHY_PORT_CNS;

    GT_BOOL   isLooped = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL isLoopedPtr.
                Expected: GT_OK.
            */
            st = cpssDxChCscdFastFailoverPortIsLoopedGet(dev, port, &isLooped);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with isLoopedPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCscdFastFailoverPortIsLoopedGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, isLoopedPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdFastFailoverPortIsLoopedGet(dev, port, &isLooped);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdFastFailoverPortIsLoopedGet(dev, port, &isLooped);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdFastFailoverPortIsLoopedGet(dev, port, &isLooped);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdFastFailoverPortIsLoopedGet(dev, port, &isLooped);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdFastFailoverPortIsLoopedGet(dev, port, &isLooped);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdPortBridgeBypassEnableSet
(
  IN  GT_U8      devNum,
  IN  GT_U8      portNum,
  IN  GT_BOOL    enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdPortBridgeBypassEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxChx)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChCscdPortBridgeBypassEnableGet with non-null enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CSCD_VALID_PHY_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE and GT_TRUE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssDxChCscdPortBridgeBypassEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssDxChCscdPortBridgeBypassEnableGet with non-null enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssDxChCscdPortBridgeBypassEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdPortBridgeBypassEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssDxChCscdPortBridgeBypassEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssDxChCscdPortBridgeBypassEnableGet with non-null enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssDxChCscdPortBridgeBypassEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdPortBridgeBypassEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev, port);
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdPortBridgeBypassEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdPortBridgeBypassEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdPortBridgeBypassEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdPortBridgeBypassEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdPortBridgeBypassEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdPortBridgeBypassEnableGet
(
  IN GT_U8      devNum,
  IN GT_U8      portNum,
  OUT GT_BOOL    *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdPortBridgeBypassEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxChx)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CSCD_VALID_PHY_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxChCscdPortBridgeBypassEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCscdPortBridgeBypassEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdPortBridgeBypassEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdPortBridgeBypassEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdPortBridgeBypassEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdPortBridgeBypassEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdPortBridgeBypassEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdQosPortTcRemapEnableSet
(
  IN  GT_U8      devNum,
  IN  GT_U8      portNum,
  IN GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdQosPortTcRemapEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh3 and above)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssDxChCscdQosPortTcRemapEnableGet with non-null enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CSCD_VALID_PHY_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with enable [GT_FALSE and GT_TRUE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssDxChCscdQosPortTcRemapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssDxChCscdQosPortTcRemapEnableGet with non-null enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssDxChCscdQosPortTcRemapEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdQosPortTcRemapEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssDxChCscdQosPortTcRemapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssDxChCscdQosPortTcRemapEnableGet with non-null enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssDxChCscdQosPortTcRemapEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdQosPortTcRemapEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev, port);
        }

        enable = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdQosPortTcRemapEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdQosPortTcRemapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdQosPortTcRemapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdQosPortTcRemapEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdQosPortTcRemapEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdQosPortTcRemapEnableGet
(
  IN GT_U8      devNum,
  IN GT_U8      portNum,
  OUT GT_BOOL    *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdQosPortTcRemapEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (DxCh3 and above)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CSCD_VALID_PHY_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssDxChCscdQosPortTcRemapEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCscdQosPortTcRemapEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdQosPortTcRemapEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdQosPortTcRemapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdQosPortTcRemapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdQosPortTcRemapEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdQosPortTcRemapEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdQosTcRemapTableSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              tc,
    IN  CPSS_DXCH_CSCD_QOS_TC_REMAP_STC    *tcMappingsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdQosTcRemapTableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh3 and above)
    1.1.1. Call with not null tcMappings.
    Expected: GT_OK.
    1.1.2. Call cpssDxChCscdQosTcRemapTableGet with non-null tcMappingsPtr.
    Expected: GT_OK and the same tcMappings.
    1.1.3. Call  with out of range tc [ CSCD_INVALID_TC_CNS ].
    Expected: GT_BAD_PARAM.
    1.1.4. Call with null tcMappings [NULL].
    Expected: GT_BAD_PTR.
    1.1.5. Call  with out of range tcMappings structure members
           [CSCD_INVALID_TC_CNS].
    Expected: GT_BAD_PARAM for all members.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port;

    GT_U32                            tc = 0;
    CPSS_DXCH_CSCD_QOS_TC_REMAP_STC   tcMappings;
    CPSS_DXCH_CSCD_QOS_TC_REMAP_STC   tcMappingsGet;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with tcMappings [0 / 31].
                Expected: GT_OK.
            */

            /* Call with tcMappings [0] */
            tc = 0;

            tcMappings.forwardLocalTc    = 1;
            tcMappings.forwardStackTc    = 1;
            tcMappings.toAnalyzerLocalTc = 1;
            tcMappings.toAnalyzerStackTc = 1;
            tcMappings.toCpuLocalTc      = 1;
            tcMappings.toCpuStackTc      = 1;
            tcMappings.fromCpuLocalTc    = 1;
            tcMappings.fromCpuStackTc    = 1;

            st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);

            /*
                1.1.2. Call cpssDxChCscdQosTcRemapTableGet with non-null tcMappingsPtr.
                Expected: GT_OK and the same tcMappings.
            */
            st = cpssDxChCscdQosTcRemapTableGet(dev, tc, &tcMappingsGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdQosTcRemapTableGet: %d, %d", dev, tc);

            /*
                1.1.3. Call  with out of range tc [ CSCD_INVALID_TC_CNS ].
                Expected: GT_BAD_PARAM.
            */
            tc = CSCD_INVALID_TC_CNS;

            st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, tc);

            tc = 0;

            /*
                1.1.4. Call with null tcMappings [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCscdQosTcRemapTableSet(dev, tc, NULL);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, tc);

            /*
                1.1.5. Call  with out of range tcMappings structure members
                       [CSCD_INVALID_TC_CNS].
                Expected: GT_BAD_PARAM for all members.
            */

            tcMappings.forwardLocalTc    = CSCD_INVALID_TC_CNS;
            st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);
            tcMappings.forwardLocalTc    = 1;

            tcMappings.forwardStackTc    = CSCD_INVALID_TC_CNS;
            st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);
            tcMappings.forwardStackTc    = 1;

            tcMappings.toAnalyzerLocalTc = CSCD_INVALID_TC_CNS;
            st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);
            tcMappings.toAnalyzerLocalTc = 1;

            tcMappings.toAnalyzerStackTc = CSCD_INVALID_TC_CNS;
            st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);
            tcMappings.toAnalyzerStackTc = 1;

            tcMappings.toCpuLocalTc      = CSCD_INVALID_TC_CNS;
            st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);
            tcMappings.toCpuLocalTc      = 1;

            tcMappings.toCpuStackTc      = CSCD_INVALID_TC_CNS;
            st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);
            tcMappings.toCpuStackTc      = 1;

            tcMappings.fromCpuLocalTc    = CSCD_INVALID_TC_CNS;
            st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);
            tcMappings.fromCpuLocalTc    = 1;

            tcMappings.fromCpuStackTc    = CSCD_INVALID_TC_CNS;
            st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, tc);
            tcMappings.fromCpuStackTc    = 1;
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdQosTcRemapTableSet(dev, tc, &tcMappings);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdQosTcRemapTableGet
(
    IN   GT_U8                              devNum,
    IN   GT_U32                             tc,
    OUT  CPSS_DXCH_CSCD_QOS_TC_REMAP_STC   *tcMappingsPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdQosTcRemapTableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (DxCh3 and above)
    1.1.1. Call with non-NULL tcMappingsPtr.
    Expected: GT_OK.
    1.1.2. Call with wrong tc [CSCD_INVALID_TC_CNS].
    Expected: GT_BAD_PARAM.
    1.1.3. Call with tcMappingsPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS                       st   = GT_OK;
    GT_U8                           dev;
    GT_U8                           port = CSCD_VALID_PHY_PORT_CNS;

    GT_U32                          tc = 0;
    CPSS_DXCH_CSCD_QOS_TC_REMAP_STC tcMappings;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL tcMappingsPtr.
                Expected: GT_OK.
            */
            st = cpssDxChCscdQosTcRemapTableGet(dev, tc, &tcMappings);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with wrong tc [CSCD_INVALID_TC_CNS].
                Expected: GT_BAD_PARAM.
            */
            tc = CSCD_INVALID_TC_CNS;

            st = cpssDxChCscdQosTcRemapTableGet(dev, tc, &tcMappings);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

            tc = 0;

            /*
                1.1.2. Call with tcMappingsPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChCscdQosTcRemapTableGet(dev, tc, NULL);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                    "%d, %d, tcMappingsPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdQosTcRemapTableGet(dev, tc, &tcMappings);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdQosTcRemapTableGet(dev, tc, &tcMappings);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdOrigSrcPortFilterEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdOrigSrcPortFilterEnableSet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssDxChCscdOrigSrcPortFilterEnableGet with non-null enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with enable [GT_FALSE and GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        enable = GT_FALSE;

        st = cpssDxChCscdOrigSrcPortFilterEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChCscdOrigSrcPortFilterEnableGet
                 with non-null enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChCscdOrigSrcPortFilterEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdOrigSrcPortFilterEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssDxChCscdOrigSrcPortFilterEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssDxChCscdOrigSrcPortFilterEnableGet with
                 non-null enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxChCscdOrigSrcPortFilterEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdOrigSrcPortFilterEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);
    }

    enable = GT_FALSE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdOrigSrcPortFilterEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdOrigSrcPortFilterEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxChCscdOrigSrcPortFilterEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdOrigSrcPortFilterEnableGet)
{
/*
    ITERATE_DEVICES (DxChx)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;


    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssDxChCscdOrigSrcPortFilterEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCscdOrigSrcPortFilterEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdOrigSrcPortFilterEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdOrigSrcPortFilterEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdDevMapLookupModeSet
(
    IN GT_U8                               devNum,
    IN CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT   mode
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdDevMapLookupModeSet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with mode [CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E /
                         CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E]
    Expected: GT_OK.
    1.2. Call cpssDxChCscdDevMapLookupModeGet with non-NULL modePtr
    Expected: GT_OK and same value as written.
    1.3. Call with out of range mode [wrong enum values]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT   mode    = CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E;
    CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT   modeGet = CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with mode [CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E /
                                 CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E]
            Expected: GT_OK.
        */
        mode = CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E;

        st = cpssDxChCscdDevMapLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChCscdDevMapLookupModeGet with non-NULL modePtr
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChCscdDevMapLookupModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdDevMapLookupModeGet: %d", dev);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "got another mode then was set: %d", dev);

        /*
            1.1. Call with mode [CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E /
                                 CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E]
            Expected: GT_OK.
        */
        mode = CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E;

        st = cpssDxChCscdDevMapLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call cpssDxChCscdDevMapLookupModeGet with non-NULL modePtr
            Expected: GT_OK and same value as written.
        */
        st = cpssDxChCscdDevMapLookupModeGet(dev, &modeGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdDevMapLookupModeGet: %d", dev);

        /* verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(mode, modeGet,
                   "got another mode then was set: %d", dev);

        /*
            1.3. Call with out of range mode [wrong enum values]
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChCscdDevMapLookupModeSet
                            (dev, mode),
                            mode);
    }

    mode = CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdDevMapLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdDevMapLookupModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdDevMapLookupModeGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT   *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdDevMapLookupModeGet)
{
/*
    ITERATE_DEVICES (Lion and above)
    1.1. Call with non-NULL modePtr
    Expected: GT_OK.
    1.2. Call with NULL modePtr.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT   mode = CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E;
    

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-NULL modePtr
            Expected: GT_OK.
        */
        st = cpssDxChCscdDevMapLookupModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with NULL modePtr.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChCscdDevMapLookupModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, modePtr = NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdDevMapLookupModeGet(dev, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdDevMapLookupModeGet(dev, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdPortLocalDevMapLookupEnableSet
(
    IN GT_U8                devNum,
    IN GT_U8                portNum,
    IN CPSS_DIRECTION_ENT   portDirection,   
    IN GT_BOOL              enable
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdPortLocalDevMapLookupEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (Lion and above)
    1.1.1. Call with portDirection [CPSS_DIRECTION_INGRESS_E /
                                    CPSS_DIRECTION_EGRESS_E /
                                    CPSS_DIRECTION_BOTH_E].
                     enable [GT_FALSE / GT_TRUE]
    Expected: GT_OK.
    1.1.2. Call cpssDxChCscdPortLocalDevMapLookupEnableGet with non-NULL pointers,
                                                           other params same as in 1.1.1.
    Expected: GT_OK and the same values.
    1.1.3. Call with out of range portDirection [wrong enum values],
                     other params same as in 1.1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CSCD_VALID_PHY_PORT_CNS;

    CPSS_DIRECTION_ENT   direction = CPSS_DIRECTION_INGRESS_E;
    GT_BOOL              enable    = GT_FALSE;
    GT_BOOL              enableGet = GT_FALSE;


    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with portDirection [CPSS_DIRECTION_INGRESS_E /
                                                CPSS_DIRECTION_EGRESS_E /
                                                CPSS_DIRECTION_BOTH_E].
                                 enable [GT_FALSE / GT_TRUE]
                Expected: GT_OK.
            */
            direction = CPSS_DIRECTION_INGRESS_E;
            enable    = GT_FALSE;

            st = cpssDxChCscdPortLocalDevMapLookupEnableSet(dev, port, direction, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, enable);

            /*
                1.1.2. Call cpssDxChCscdPortLocalDevMapLookupEnableGet with non-NULL pointers,
                                                                       other params same as in 1.1.1.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdPortLocalDevMapLookupEnableGet: %d",  dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

            /*
                1.1.1. Call with portDirection [CPSS_DIRECTION_INGRESS_E /
                                                CPSS_DIRECTION_EGRESS_E /
                                                CPSS_DIRECTION_BOTH_E].
                                 enable [GT_FALSE / GT_TRUE]
                Expected: GT_OK.
            */
            direction = CPSS_DIRECTION_EGRESS_E;
            enable    = GT_TRUE;

            st = cpssDxChCscdPortLocalDevMapLookupEnableSet(dev, port, direction, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, enable);

            /*
                1.1.2. Call cpssDxChCscdPortLocalDevMapLookupEnableGet with non-NULL pointers,
                                                                       other params same as in 1.1.1.
                Expected: GT_OK and the same values.
            */
            st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdPortLocalDevMapLookupEnableGet: %d",  dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

            /*
                1.1.1. Call with portDirection [CPSS_DIRECTION_INGRESS_E /
                                                CPSS_DIRECTION_EGRESS_E /
                                                CPSS_DIRECTION_BOTH_E].
                                 enable [GT_FALSE / GT_TRUE]
                Expected: GT_OK.
            */
            direction = CPSS_DIRECTION_BOTH_E;
            
            st = cpssDxChCscdPortLocalDevMapLookupEnableSet(dev, port, direction, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, direction, enable);

            /*
                1.1.2. Call cpssDxChCscdPortLocalDevMapLookupEnableGet with non-NULL pointers,
                                                                       other params same as in 1.1.1.
                Expected: GT_BAD_PARAM, use Egress or Ingress to get the same values.
            */
            st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st,
                       "cpssDxChCscdPortLocalDevMapLookupEnableGet: %d",  dev);

            direction = CPSS_DIRECTION_EGRESS_E;
            st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdPortLocalDevMapLookupEnableGet: %d",  dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

            direction = CPSS_DIRECTION_INGRESS_E;
            st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enableGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssDxChCscdPortLocalDevMapLookupEnableGet: %d",  dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d", dev);

            /*
                1.1.3. Call with out of range portDirection [wrong enum values],
                                 other params same as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCscdPortLocalDevMapLookupEnableSet
                                (dev, port, direction, enable),
                                direction);
        }

        direction = CPSS_DIRECTION_INGRESS_E;
        enable    = GT_FALSE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdPortLocalDevMapLookupEnableSet(dev, port, direction, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdPortLocalDevMapLookupEnableSet(dev, port, direction, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;
        direction = CPSS_DIRECTION_INGRESS_E;
        enable    = GT_TRUE;

        st = cpssDxChCscdPortLocalDevMapLookupEnableSet(dev, port, direction, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdPortLocalDevMapLookupEnableGet: %d",  dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        direction = CPSS_DIRECTION_INGRESS_E;
        enable    = GT_FALSE;

        st = cpssDxChCscdPortLocalDevMapLookupEnableSet(dev, port, direction, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdPortLocalDevMapLookupEnableGet: %d",  dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        direction = CPSS_DIRECTION_EGRESS_E;
        enable    = GT_TRUE;

        st = cpssDxChCscdPortLocalDevMapLookupEnableSet(dev, port, direction, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdPortLocalDevMapLookupEnableGet: %d",  dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

        direction = CPSS_DIRECTION_EGRESS_E;
        enable    = GT_FALSE;

        st = cpssDxChCscdPortLocalDevMapLookupEnableSet(dev, port, direction, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssDxChCscdPortLocalDevMapLookupEnableGet: %d",  dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                   "get another enable than was set: %d", dev);

    }

    direction = CPSS_DIRECTION_INGRESS_E;
    enable    = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdPortLocalDevMapLookupEnableSet(dev, port, direction, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdPortLocalDevMapLookupEnableSet(dev, port, direction, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChCscdPortLocalDevMapLookupEnableGet
(
    IN  GT_U8               devNum,
    IN  GT_U8               portNum,
    IN CPSS_DIRECTION_ENT   portDirection,   
    OUT GT_BOOL             *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChCscdPortLocalDevMapLookupEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (Lion and above)
    1.1.1. Call with portDirection [CPSS_DIRECTION_INGRESS_E /
                                    CPSS_DIRECTION_EGRESS_E /
                                    CPSS_DIRECTION_BOTH_E].
                     non NULL enablePtr
    Expected: GT_OK.
    1.1.2. Call with out of range portDirection [wrong enum values],
                     other params same as in 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.3. Call with enablePtr [NULL],
                     other params same as in 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = CSCD_VALID_PHY_PORT_CNS;

    CPSS_DIRECTION_ENT   direction = CPSS_DIRECTION_INGRESS_E;
    GT_BOOL              enable    = GT_FALSE;
    

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with portDirection [CPSS_DIRECTION_INGRESS_E /
                                                CPSS_DIRECTION_EGRESS_E /
                                                CPSS_DIRECTION_BOTH_E].
                                 non NULL enablePtr
                Expected: GT_OK.
            */

            /*call with direction = CPSS_DIRECTION_INGRESS_E */
            direction = CPSS_DIRECTION_INGRESS_E;
            
            st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, direction);

            /*call with direction = CPSS_DIRECTION_EGRESS_E */
            direction = CPSS_DIRECTION_EGRESS_E;
            
            st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, direction);

            /*call with direction = CPSS_DIRECTION_BOTH_E */
            direction = CPSS_DIRECTION_BOTH_E;
            
            st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, direction);

            /*
                1.1.2. Call with out of range portDirection [wrong enum values],
                                 other params same as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChCscdPortLocalDevMapLookupEnableGet
                                (dev, port, direction, &enable),
                                direction);

            /*
                1.1.3. Call with enablePtr [NULL],
                                 other params same as in 1.1.1.
                Expected: GT_BAD_PTR.
            */
            direction = CPSS_DIRECTION_EGRESS_E;
            st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, NULL);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
        }

        direction = CPSS_DIRECTION_INGRESS_E;
        
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    direction = CPSS_DIRECTION_INGRESS_E;
    
    /* 2. For not active devices check that function returns non GT_OK.*/
    port = CSCD_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChCscdPortLocalDevMapLookupEnableGet(dev, port, direction, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxChCscd suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChCscd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdPortTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdPortTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdDevMapTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdDevMapTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdRemapQosModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdCtrlQosSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdRemapDataQosTblSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdDsaSrcDevFilterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdHyperGInternalPortModeSet)
/*    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdUcRouteTrunkHashTypeSet) */
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdHyperGPortCrcModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdFastFailoverFastStackRecoveryEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdFastFailoverFastStackRecoveryEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdFastFailoverSecondaryTargetPortMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdFastFailoverSecondaryTargetPortMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdFastFailoverPortIsLoopedSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdFastFailoverPortIsLoopedGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdPortBridgeBypassEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdPortBridgeBypassEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdQosPortTcRemapEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdQosPortTcRemapEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdQosTcRemapTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdQosTcRemapTableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdOrigSrcPortFilterEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdOrigSrcPortFilterEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdDevMapLookupModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdDevMapLookupModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdPortLocalDevMapLookupEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChCscdPortLocalDevMapLookupEnableGet)
UTF_SUIT_END_TESTS_MAC(cpssDxChCscd)
