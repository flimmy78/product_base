/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPcl.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPcl.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>

#include <cpss/exMx/exMxGen/pcl/cpssExMxPcl.h>
#include <cpss/exMx/exMxTg/pcl/cpssExMxTgPcl.h>
#include <cpss/exMx/exMxGen/policy/cpssExMxPolicy.h>


#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define PCL_INVALID_ENUM_CNS                0x5AAAAAA5


/*  Internal function. Is used for filling PCL action structure     */
/*  with default values which are used for most of all tests.       */
static void pclActionDefaultSet(IN CPSS_EXMX_POLICY_ACTION_STC   *actionPtr);

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclTcamFlush
(
    IN GT_U8                           devNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclTcamFlush)
{
/*
ITERATE_DEVICES
1.1. Call with all active device ids. Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8           dev;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with all active device ids. Expected: GT_OK.   */
        /* Expected: GT_OK.                                         */

        st = cpssExMxPclTcamFlush(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclTcamFlush(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclTcamFlush(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclTemplateSet
(
    IN GT_U8                                devNum,
    IN GT_U8                                profileId,
    IN CPSS_EXMX_PCL_TEMPLATE_ENT           keyTemplate,
    IN GT_U32                               udbAmount,
    IN CPSS_EXMX_PCL_UDB_CFG_STC            udbCfg[],
    IN CPSS_EXMX_PCL_INCLUDE_VLAN_INLIF_ENT includeVlanOrInLifNum,
    IN GT_BOOL                              includeProfileId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclTemplateSet)
{
/*
ITERATE_DEVICES
1.1. For 98EX1x6 devices call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E] to set 8 templates per profile mode. Expected: GT_OK.
1.2. Call with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], udbAmount [2], udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0], [CPSS_EXMX_PCL_OFFSET_L4_E, 0]], includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_NONE_E], includeProfileId [GT_FALSE]. Expected: GT_OK.
1.3. Call cpssExMxPclTemplateGet with non-NULL udbCfg, includeVlanOrInLifNumPtr, includeProfileIdPtr, other params same as in 1.2. Expected: GT_OK and same values as written.
1.4. Check for out of range for profileId. Call with profileId [2], other parameters is the same as in 1.2. Expected: NON GT_OK.
1.5. Check for invalid keyTemplate for 8-2 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], other parameters is the same as in 1.2. Expected: NON GT_OK.
1.6. Check for invalid UDB config entry. Call with udbAmout [1], udbCfg [0x5AAAAAA5, 0], other parameters is the same as in 1.2. Expected: GT_BAD_PARAM.
1.7. Check for offsets in the udb entry which aren't supported by all device types. Call with udbAmount [1], udbCfg [CPSS_EXMX_PCL_OFFSET_L5_E/CPSS_EXMX_PCL_OFFSET_MPLS_E/ CPSS_EXMX_PCL_OFFSET_START_E, 0], other parameters is the same as in 1.2. Expected: GT_OK for non 98EX1x6 devices and NON GT_OK for 98EX1x6 devices.
1.8. Check if include VID isn't supported by 98EX1x6 devices. Call with includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_VLAN_E], other parameters is the same as in 1.2. Expected: GT_OK for non 98EX1x6 and NON GT_OK for 98EX1x6 devices.
1.9. Check if include profile id isn't supported by 98EX1x5 devices. Call with includeProfileId [GT_TRUE], other parameters is the same as in 1.2. Expected: GT_OK for non 98EX1x5 and NON GT_OK for 98EX1x5 devices.
1.10. Check for invalid enum value for includeVlanOrInLifNum. Call with includeVlanOrInLifNum [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
1.11. For 98EX1x6 devices set profile size 4-4, four templates per four profiles per one device. Call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E] to set 4 templates per profile mode. Expected: GT_OK.
1.12. For 98EX1x6 devices call with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], udbAmount [2], udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0], [CPSS_EXMX_PCL_OFFSET_L4_E, 0]], includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_NONE_E], includeProfileId [GT_FALSE]. Expected: GT_OK.
1.13. For 98EX1x6 devices call cpssExMxPclTemplateGet with non-NULL udbCfg, includeVlanOrInLifNumPtr, includeProfileIdPtr, other params same as in 1.12. Expected: GT_OK and same values as written.
1.14. For 98EX1x6 devices check for out of range for profileId. Call with profileId [5], other parameters is the same as in 1.12. Expected: NON GT_OK.
1.15. For 98EX1x6 devices check for invalid keyTemplate for 4-4 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], other parameters is the same as in 1.12. Expected: NON GT_OK.
1.16. For 98EX1x6 devices set profile size 1-16, one template per sixsteen profiles per one device. Call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E] to set one template per profile mode. Expected: GT_OK.
1.17. For 98EX1x6 devices call with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_ANY_E], udbAmount [2], udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0], [CPSS_EXMX_PCL_OFFSET_L4_E, 0]], includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_NONE_E], includeProfileId [GT_FALSE]. Expected: GT_OK.
1.18. For 98EX1x6 devices call cpssExMxPclTemplateGet with non-NULL udbCfg, includeVlanOrInLifNumPtr, includeProfileIdPtr, other params same as in 1.17. Expected: GT_OK and same values as written.
1.19. For 98EX1x6 devices check for out of range for profileId. Call with profileId [16], other parameters is the same as in 1.17. Expected: NON GT_OK.
1.20. For 98EX1x6 devices check for invalid keyTemplate for 1-16 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], other parameters is the same as in 1.17. Expected: NON GT_OK.
1.21. Call cpssExMxPclTcamFlush to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                   dev;
    GT_U8                                   profileId;
    CPSS_EXMX_PCL_TEMPLATE_ENT              keyTemplate;
    GT_U32                                  udbAmount;
    CPSS_EXMX_PCL_UDB_CFG_STC               udbCfg[2];
    CPSS_EXMX_PCL_INCLUDE_VLAN_INLIF_ENT    includeVlanOrInLifNum;
    GT_BOOL                                 includeProfileId;

    GT_BOOL                                 failureWas;
    CPSS_EXMX_PCL_UDB_CFG_STC               retUdbCfg[2];
    CPSS_EXMX_PCL_INCLUDE_VLAN_INLIF_ENT    retIncludeVlanOrInLifNum;
    GT_BOOL                                 retIncludeProfileId;


    CPSS_EXMX_TG_PCL_PROFILE_SIZE_ENT       profileSize;
    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        /* 1.1.For 98EX1x6 devices call function cpssExMxTgPclProfileSizeSet*/
        /* with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E]   */
        /* to set 8 templates per profile mode. Expected: GT_OK.            */
        profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E;

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                         "cpssExMxTgPclProfileSizeSet: %d, %d",
                                         dev, profileSize);
        }

        /* 1.2. Call with profileId [0], keyTemplate                        */
        /* [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], udbAmount [2],              */
        /* udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0],                          */
        /* [CPSS_EXMX_PCL_OFFSET_L4_E, 0]], includeVlanOrInLifNum           */
        /* [CPSS_EXMX_PCL_INCLUDE_NONE_E], includeProfileId [GT_FALSE].     */
        /* Expected: GT_OK.                                                 */
        profileId = 0;
        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;
        udbAmount = 2;
        udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_L2_E;
        udbCfg[0].offset = 0;
        udbCfg[1].offsetType = CPSS_EXMX_PCL_OFFSET_L4_E;
        udbCfg[1].offset = 0;
        includeVlanOrInLifNum = CPSS_EXMX_PCL_INCLUDE_NONE_E;
        includeProfileId = GT_FALSE;
        
        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "%d, %d, %d, <udbAmount>, <udbCfg>, %d, 0; profileSize = %d",
                                     dev, profileId, keyTemplate,
                                     includeVlanOrInLifNum, profileSize);

        /* 1.3. Call cpssExMxPclTemplateGet with non-NULL udbCfg,       */
        /* includeVlanOrInLifNumPtr, includeProfileIdPtr, other params  */
        /* same as in 1.2. Expected: GT_OK and same values as written.  */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)retUdbCfg, 0, 
                     udbAmount*sizeof(CPSS_EXMX_PCL_UDB_CFG_STC));

        st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                    retUdbCfg, &retIncludeVlanOrInLifNum, &retIncludeProfileId);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateGet: %d, %d, %d, %d",
                                     dev, profileId, keyTemplate, udbAmount);
        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)udbCfg,
                                            (const GT_VOID*)retUdbCfg, udbAmount*sizeof(CPSS_EXMX_PCL_UDB_CFG_STC))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                         "cpssExMxPclTemplateGet: get another udbCfg than was set: %d, %d, %d, %d",
                                         dev, profileId, keyTemplate, udbAmount);

            failureWas = (includeVlanOrInLifNum != retIncludeVlanOrInLifNum);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                         "cpssExMxPclTemplateGet: get another includeVlanOrInLifNum than was set: %d, %d, %d, %d",
                                         dev, profileId, keyTemplate, udbAmount);

            failureWas = (includeProfileId != retIncludeProfileId);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                         "cpssExMxPclTemplateGet: get another includeProfileId than was set: %d, %d, %d, %d",
                                         dev, profileId, keyTemplate, udbAmount);
        }

        /* 1.4. Check for out of range for profileId. Call with     */
        /* profileId [2], other parameters is the same as in 1.2.   */
        /* Expected: NON GT_OK.                                     */
        profileId = 2;

        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileId);

        profileId = 0; /* restore correct value */

        /* 1.5. Check for invalid keyTemplate for 8-2 mode. Call with       */
        /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], other parameters    */
        /* is the same as in 1.2. Expected: NON GT_OK.                      */
        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E;

        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                        "%d, %d, %d, profileSize = %d",
                                        dev, profileId, keyTemplate, profileSize);

        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E; /* restore correct value */

        /* 1.6. Check for invalid UDB config entry. Call with udbAmout [1], */
        /* udbCfg [0x5AAAAAA5, 0], other parameters is the same as in 1.2.  */
        /* Expected: GT_BAD_PARAM.                                          */
        udbAmount = 1;
        udbCfg[0].offsetType = PCL_INVALID_ENUM_CNS;
        udbCfg[0].offset = 0;

        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, udbAmount = %d, udbCfg[0].offsetType = %d",
                                     dev, udbAmount, udbCfg[0].offsetType);

        /* 1.7. Check for offsets in the udb entry which aren't supported   */
        /* by all device types. Call with udbAmount [1],                    */
        /* udbCfg [CPSS_EXMX_PCL_OFFSET_L5_E/CPSS_EXMX_PCL_OFFSET_MPLS_E/        */
        /* CPSS_EXMX_PCL_OFFSET_START_E, 0], other parameters is the same   */
        /* as in 1.2. Expected: GT_OK for non 98EX1x6 devices               */
        /* and NON GT_OK for 98EX1x6 devices.                               */
        udbAmount = 1;
        udbCfg[0].offset = 0;

        udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_L5_E;

        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        if (CPSS_PP_FAMILY_TIGER_E != devFamily)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "non-Tiger device: %d, udbCfg[0].offsetType = %d",
                                         dev, udbCfg[0].offsetType);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, udbCfg[0].offsetType = %d",
                                             dev, udbCfg[0].offsetType);
        }

        udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_MPLS_E;

        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        if (CPSS_PP_FAMILY_TIGER_E != devFamily)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "non-Tiger device: %d, udbCfg[0].offsetType = %d",
                                         dev, udbCfg[0].offsetType);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, udbCfg[0].offsetType = %d",
                                             dev, udbCfg[0].offsetType);
        }

        udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_START_E;

        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        if (CPSS_PP_FAMILY_TIGER_E != devFamily)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "non-Tiger device: %d, udbCfg[0].offsetType = %d",
                                         dev, udbCfg[0].offsetType);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, udbCfg[0].offsetType = %d",
                                             dev, udbCfg[0].offsetType);
        }

        /* restore correct values */
        udbAmount = 2;
        udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_L2_E;
        udbCfg[0].offset = 0;

        /* 1.8. Check if include VID isn't supported by 98EX1x6 devices.    */
        /* Call with includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_VLAN_E],  */
        /* other parameters is the same as in 1.2. Expected: GT_OK for      */
        /* non 98EX1x6 and NON GT_OK for 98EX1x6 devices.                   */
        includeVlanOrInLifNum = CPSS_EXMX_PCL_INCLUDE_VLAN_E;

        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        if (CPSS_PP_FAMILY_TIGER_E != devFamily)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "non-Tiger device: %d, includeVlanOrInLifNum = %d",
                                         dev, includeVlanOrInLifNum);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, includeVlanOrInLifNum = %d",
                                             dev, includeVlanOrInLifNum);
        }

        includeVlanOrInLifNum = CPSS_EXMX_PCL_INCLUDE_NONE_E; /* restore correct value */

        /* 1.9. Check if include profile id isn't supported by 98EX1x5      */
        /* devices. Call with includeProfileId [GT_TRUE], other parameters  */
        /* is the same as in 1.2. Expected: GT_OK for non 98EX1x5 and       */
        /* NON GT_OK for 98EX1x5 devices.                                   */
        includeProfileId = GT_TRUE;

        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        if (CPSS_PP_FAMILY_TWISTD_E != devFamily)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "non Twist-D device: %d, includeProfileId = %d",
                                         dev, includeProfileId);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                             "Twist-D device: %d, includeProfileId = %d",
                                             dev, includeProfileId);
        }

        includeProfileId = GT_FALSE; /* restore correct value */

        /* 1.10. Check for invalid enum value for includeVlanOrInLifNum.     */
        /* Call with includeVlanOrInLifNum [0x5AAAAAA5].                    */
        /* Expected: GT_BAD_PARAM.                                          */
        includeVlanOrInLifNum = PCL_INVALID_ENUM_CNS;

        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, includeVlanOrInLifNum = %d",
                                     dev, includeVlanOrInLifNum);

        includeVlanOrInLifNum = CPSS_EXMX_PCL_INCLUDE_NONE_E; /* restore correct value */

        /* Tests for Tiger devices (1.11 - 1.19.)*/

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {

            /* 1.11. Set profile size 4-4, four templates per four profiles     */
            /* per one device. For 98EX1x6 devices call function                */
            /* cpssExMxTgPclProfileSizeSet                                      */
            /* with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E]   */
            /* to set 4 templates per profile mode. Expected: GT_OK.            */
            profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E;

            st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                         "cpssExMxTgPclProfileSizeSet: %d, %d",
                                         dev, profileSize);

            /* 1.12. For 98EX1x6 devices call with profileId [0], keyTemplate   */
            /* [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], udbAmount [2],                  */
            /* udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0],                          */
            /* [CPSS_EXMX_PCL_OFFSET_L4_E, 0]],                                 */
            /* includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_NONE_E],            */
            /* includeProfileId [GT_FALSE]. Expected: GT_OK for 98EX1x6 devices */
            /* and NON GT_OK for 98EX1x5, 98MX6x5 and 98MX6x8 devices.          */
            profileId = 0;
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E;
            udbAmount = 2;
            udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_L2_E;
            udbCfg[0].offset = 0;
            udbCfg[1].offsetType = CPSS_EXMX_PCL_OFFSET_L4_E;
            udbCfg[1].offset = 0;
            includeVlanOrInLifNum = CPSS_EXMX_PCL_INCLUDE_NONE_E;
            includeProfileId = GT_FALSE;

            st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, includeVlanOrInLifNum, includeProfileId);
            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                                         "Tiger device: %d, %d, %d, <udbAmount>, <udbCfg>, %d, 0; profileSize = %d",
                                         dev, profileId, keyTemplate,
                                         includeVlanOrInLifNum, profileSize);

            /* 1.13. For 98EX1x6 devices call cpssExMxPclTemplateGet with    */
            /* non-NULL udbCfg, includeVlanOrInLifNumPtr,                   */
            /* includeProfileIdPtr, other params same as in 1.12.           */
            /* Expected: GT_OK and same values as written.                  */

            /* Fill returned structures with zero */
            cpssOsMemSet((GT_VOID*)retUdbCfg, 0, 
                         udbAmount*sizeof(CPSS_EXMX_PCL_UDB_CFG_STC));

            st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                        retUdbCfg, &retIncludeVlanOrInLifNum, &retIncludeProfileId);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateGet: %d, %d, %d, %d",
                                         dev, profileId, keyTemplate, udbAmount);
            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)udbCfg,
                                                (const GT_VOID*)retUdbCfg, udbAmount*sizeof(CPSS_EXMX_PCL_UDB_CFG_STC))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclTemplateGet: get another udbCfg than was set: %d, %d, %d, %d",
                                             dev, profileId, keyTemplate, udbAmount);

                failureWas = (includeVlanOrInLifNum != retIncludeVlanOrInLifNum);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclTemplateGet: get another includeVlanOrInLifNum than was set: %d, %d, %d, %d",
                                             dev, profileId, keyTemplate, udbAmount);

                failureWas = (includeProfileId != retIncludeProfileId);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclTemplateGet: get another includeProfileId than was set: %d, %d, %d, %d",
                                             dev, profileId, keyTemplate, udbAmount);
            }

            /* 1.14. For 98EX1x6 devices check for out of range for profileId.  */
            /* Call with profileId [5], other parameters is the same as in 1.12.*/
            /* Expected: NON GT_OK.                                             */
            profileId = 5;

            st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, includeVlanOrInLifNum, includeProfileId);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, profileId = %d",
                                             dev, profileId);

           profileId = 0; /* restore correct value */

            /* 1.15. For 98EX1x6 devices check for invalid keyTemplate for 4-4  */
            /* mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], */
            /* other parameters is the same as in 1.12. Expected: NON GT_OK.    */
           keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;

           st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                       udbCfg, includeVlanOrInLifNum, includeProfileId);
           UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                            "Tiger device: %d, keyTemplate = %d, profileSize = %d",
                                            dev, keyTemplate, profileSize);

           keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E; /* restore correct value */

            /* 1.16. Set profile size 1-16, one template per sixsteen profiles  */
            /* per one device. For 98EX1x6 devices call function                */
            /* cpssExMxTgPclProfileSizeSet with                                 */
            /* profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E]        */
            /* to set one template per profile mode. Expected: GT_OK.           */
           profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E;

           st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
           UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                        "cpssExMxTgPclProfileSizeSet: %d, %d",
                                        dev, profileSize);

            /* 1.17. For 98EX1x6 devices call with profileId [0], keyTemplate   */
            /* [CPSS_EXMX_PCL_TEMPLATE_ANY_E], udbAmount [2],                   */
            /* udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0],                          */
            /* [CPSS_EXMX_PCL_OFFSET_L4_E, 0]],                                 */
            /* includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_NONE_E],            */
            /* includeProfileId [GT_FALSE]. Expected: GT_OK for 98EX1x6 devices */
            /* and NON GT_OK for 98EX1x5, 98MX6x5 and 98MX6x8 devices.          */
            profileId = 0;
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_ANY_E;
            udbAmount = 2;
            udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_L2_E;
            udbCfg[0].offset = 0;
            udbCfg[1].offsetType = CPSS_EXMX_PCL_OFFSET_L4_E;
            udbCfg[1].offset = 0;
            includeVlanOrInLifNum = CPSS_EXMX_PCL_INCLUDE_NONE_E;
            includeProfileId = GT_FALSE;

            st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, includeVlanOrInLifNum, includeProfileId);
            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                                         "Tiger device: %d, %d, %d, <udbAmount>, <udbCfg>, %d, 0; profileSize = %d",
                                         dev, profileId, keyTemplate,
                                         includeVlanOrInLifNum, profileSize);

            /* 1.18. For 98EX1x6 devices call cpssExMxPclTemplateGet with   */
            /* non-NULL udbCfg, includeVlanOrInLifNumPtr,                   */
            /* includeProfileIdPtr, other params same as in 1.17.           */
            /* Expected: GT_OK and same values as written.                  */

            /* Fill returned structures with zero */
            cpssOsMemSet((GT_VOID*)retUdbCfg, 0, 
                         udbAmount*sizeof(CPSS_EXMX_PCL_UDB_CFG_STC));

            st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                        retUdbCfg, &retIncludeVlanOrInLifNum, &retIncludeProfileId);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateGet: %d, %d, %d, %d",
                                         dev, profileId, keyTemplate, udbAmount);
            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)udbCfg,
                                                (const GT_VOID*)retUdbCfg, udbAmount*sizeof(CPSS_EXMX_PCL_UDB_CFG_STC))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclTemplateGet: get another udbCfg than was set: %d, %d, %d, %d",
                                             dev, profileId, keyTemplate, udbAmount);

                failureWas = (includeVlanOrInLifNum != retIncludeVlanOrInLifNum);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclTemplateGet: get another includeVlanOrInLifNum than was set: %d, %d, %d, %d",
                                             dev, profileId, keyTemplate, udbAmount);

                failureWas = (includeProfileId != retIncludeProfileId);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclTemplateGet: get another includeProfileId than was set: %d, %d, %d, %d",
                                             dev, profileId, keyTemplate, udbAmount);
            }

            /* 1.19. For 98EX1x6 devices check for out of range for profileId.   */
            /* Call with profileId [16], other parameters is the same as in 1.17.*/ 
            /* Expected: NON GT_OK.                                              */
            profileId = 16;

            st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, includeVlanOrInLifNum, includeProfileId);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, profileId = %d",
                                             dev, profileId);

            profileId = 0; /* restore correct value */

            /* 1.20. For 98EX1x6 devices check for invalid keyTemplate for 1-16 */
            /* mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], */
            /* other parameters is the same as in 1.17. Expected: NON GT_OK.    */
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;

            st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, includeVlanOrInLifNum, includeProfileId);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, keyTemplate = %d, profileSize = %d",
                                             dev, keyTemplate, profileSize);

            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_ANY_E; /* restore correct value */
        }

        /* 1.21. Call cpssExMxPclTcamFlush to cleanup after testing.    */
        /* Expected: GT_OK.                                             */
        cpssExMxPclTcamFlush(dev);
    }

    /* set correct values for all the parameters (except of device number) */
    profileId = 0;
    keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;
    udbAmount = 2;
    udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_L2_E;
    udbCfg[0].offset = 0;
    udbCfg[1].offsetType = CPSS_EXMX_PCL_OFFSET_L4_E;
    udbCfg[1].offset = 0;
    includeVlanOrInLifNum = CPSS_EXMX_PCL_INCLUDE_NONE_E;
    includeProfileId = GT_FALSE;


    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                udbCfg, includeVlanOrInLifNum, includeProfileId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclTemplateDefaultActionSet
(
    IN GT_U8                           devNum,
    IN GT_U8                           profileId,
    IN CPSS_EXMX_PCL_TEMPLATE_ENT      keyTemplate,
    IN CPSS_EXMX_POLICY_ACTION_STC        *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclTemplateDefaultActionSet)
{
/*
ITERATE_DEVICES
1.1. For 98EX1x6 devices call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E] to set 8 templates per profile mode. Expected: GT_OK.
1.2. Call with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], non NULL actionPtr [[CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE}, {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0}, {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}]. Expected: GT_OK.
1.3. Call cpssExMxPclTemplateDefaultActionGet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], non NULL actionPtr. Expected: GT_OK and same values as written.

1.4. Check for policer mode which isn't supported by 98EX1x5 devices. Call with policer mode [CPSS_EXMX_POLICY_ACTION_POLICER_COUNTER_ONLY_E] in the actionPtr and the same other parameters as in 1.2. Expected: GT_OK for non 98EX1x5 and NON GT_OK for 98EX1x5 device.
1.5. Check for NULL handling. Call with actionPtr [NULL]. Expected: GT_BAD_PTR.
1.6. Check for out of range for profileId. Call with profileId [2], other parameters is the same as in 1.2. Expected: NON GT_OK.
1.7. Check for invalid keyTemplate for 8-2 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], other parameters is the same as in 1.2. Expected: NON GT_OK.
1.8. For 98EX1x6 devices set profile size 4-4, four templates per four profiles per one device. Call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E] to set 4 templates per profile mode. Expected: GT_OK.
1.9. For 98EX1x6 devices call with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], and the same actionPtr as in 1.2. Expected: GT_OK.
1.10. Call cpssExMxPclTemplateDefaultActionGet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], non NULL actionPtr. Expected: GT_OK and same values as written.

1.11. For 98EX1x6 devices check for out of range for profileId. Call with profileId [5], other parameters is the same as in 1.9. Expected: NON GT_OK.
1.12. For 98EX1x6 devices check for invalid keyTemplate for 4-4 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], other parameters is the same as in 1.9. Expected: NON GT_OK.
1.13. For 98EX1x6 devices set profile size 1-16, one template per sixsteen profiles per one device. Call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E] to set one template per profile mode. Expected: GT_OK.
1.14. For 98EX1x6 devices call with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_ANY_E], and the same actionPtr as in 1.2. Expected: GT_OK.
1.15. Call cpssExMxPclTemplateDefaultActionGet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_ANY_E], non NULL actionPtr. Expected: GT_OK and same values as written.

1.16. For 98EX1x6 devices check for out of range for profileId. Call with profileId [16], other parameters is the same as in 1.14. Expected: NON GT_OK.
1.17. For 98EX1x6 devices check for invalid keyTemplate for 1-16 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], other parameters is the same as in 1.14. Expected: NON GT_OK.
1.18. Call cpssExMxPclTcamFlush to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                           dev;
    GT_U8                           profileId;
    CPSS_EXMX_PCL_TEMPLATE_ENT      keyTemplate;
    CPSS_EXMX_POLICY_ACTION_STC        action;
    CPSS_EXMX_POLICY_ACTION_STC        retAction;
    GT_BOOL                         failureWas;

    CPSS_EXMX_TG_PCL_PROFILE_SIZE_ENT       profileSize;
    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);
        /* 1.1. For 98EX1x6 devices call function                           */
        /* cpssExMxTgPclProfileSizeSet with                                 */
        /* profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E] to set */
        /* 8 templates per profile mode. Expected: GT_OK.                   */
        profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E;

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                         "cpssExMxTgPclProfileSizeSet: %d, %d",
                                         dev, profileSize);
        }

        /* 1.2. Call with profileId [0], keyTemplate                        */
        /* [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], non NULL actionPtr          */
        /* [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE},       */
        /* {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …},                           */
        /* {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …},                   */
        /* {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0},  */
        /* {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}].      */
        /* Expected: GT_OK.                                                 */
        profileId = 0;
        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;
        pclActionDefaultSet(&action);

        st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileId, keyTemplate);


        /* 1.3. Call cpssExMxPclTemplateDefaultActionGet with profileId [0], */
        /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], non NULL         */
        /* actionPtr. Expected: GT_OK and same values as written.            */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)&retAction, 0,
                     sizeof(CPSS_EXMX_POLICY_ACTION_STC));

        st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &retAction);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateDefaultActionGet: %d, %d, %d",
                                     dev, profileId, keyTemplate);
        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action,
                                            (const GT_VOID*)&retAction, sizeof(action))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                         "cpssExMxPclTemplateDefaultActionGet: get another action than was set: %d, %d, %d",
                                         dev, profileId, keyTemplate);
        }


        /* 1.4. Check for policer mode which isn't supported by 98EX1x5     */ 
        /* devices. Call with policer mode                                  */
        /* [CPSS_EXMX_POLICY_ACTION_POLICER_COUNTER_ONLY_E] in the actionPtr   */
        /* and the same other parameters as in 1.2. Expected: GT_OK for non */
        /* 98EX1x5 and NON GT_OK for 98EX1x5 device.                        */
        action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_COUNTER_ONLY_E;

        st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
        if (CPSS_PP_FAMILY_TWISTD_E != devFamily)
        {
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileId, keyTemplate);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                             "Twist-D device: %d, %d, %d, action.policer.policerMode = %d",
                                             dev, profileId, keyTemplate,
                                             action.policer.policerMode);
        }

        /* restore valid value */
        action.policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;

        /* 1.5. Check for NULL handling. Call with actionPtr [NULL].        */
        /* Expected: GT_BAD_PTR.                                            */

        st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, %d, NULL",
                                     dev, profileId, keyTemplate);

        /* 1.6. Check for out of range for profileId. Call with             */
        /* profileId [2], other parameters is the same as in 1.2.           */
        /* Expected: NON GT_OK.                                             */
        profileId = 2;

        st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileId);

        profileId = 0; /* restore valid value */

        /* 1.7. Check for invalid keyTemplate for 8-2 mode. Call with       */
        /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], other parameters is */ 
        /* the same as in 1.2. Expected: NON GT_OK.                         */
        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E;

        st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                        "%d, %d, %d, profileSize = %d",
                                        dev, profileId, keyTemplate, profileSize);

        /* restore valid value */
        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;

        /* Tests for Tiger devices (1.7. - 1.14.) */

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            /* 1.8. For 98EX1x6 devices set profile size 4-4, four templates    */
            /* per four profiles per one device. Call function                  */
            /* cpssExMxTgPclProfileSizeSet with                                 */
            /* profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E] to set */
            /* 4 templates per profile mode. Expected: GT_OK.                   */
            profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E;

            st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                         "cpssExMxTgPclProfileSizeSet: %d, %d",
                                         dev, profileSize);

            /* 1.9. For 98EX1x6 devices call with profileId [0],                */
            /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], and the same        */
            /* actionPtr as in 1.2. Expected: GT_OK for 98EX1x6 devices and     */
            /* NON GT_OK for 98EX1x5, 98MX6x5 and 98MX6x8 devices.              */
            profileId = 0;
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E;

            st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                        "Tiger device: %d, %d, %d, profileSize = %d",
                                        dev, profileId, keyTemplate, profileSize);

            /* 1.10. Call cpssExMxPclTemplateDefaultActionGet with profileId [0],*/
            /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], non NULL actionPtr. */
            /* Expected: GT_OK and same values as written.                      */

            /* Fill returned structures with zero */
            cpssOsMemSet((GT_VOID*)&retAction, 0,
                         sizeof(CPSS_EXMX_POLICY_ACTION_STC));

            st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &retAction);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateDefaultActionGet: %d, %d, %d",
                                         dev, profileId, keyTemplate);
            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action,
                                                (const GT_VOID*)&retAction, sizeof(action))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclTemplateDefaultActionGet: get another action than was set: %d, %d, %d",
                                             dev, profileId, keyTemplate);
            }


            /* 1.11. For 98EX1x6 devices check for out of range for profileId.   */
            /* Call with profileId [5], other parameters is the same as in 1.9. */
            /* Expected: NON GT_OK.                                             */
            profileId = 5;

            st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileId);

            profileId = 0; /* restore valid value */

            /* 1.12. For 98EX1x6 devices check for invalid keyTemplate for 4-4  */
            /* mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], */
            /* other parameters is the same as in 1.9. Expected: NON GT_OK.     */
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;

            st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, %d, %d, profileSize = %d",
                                             dev, profileId, keyTemplate, profileSize);

            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E; /* restore valid value */

            /* 1.13. For 98EX1x6 devices set profile size 1-16, one template    */
            /* per sixsteen profiles per one device. Call function              */
            /* cpssExMxTgPclProfileSizeSet with                                 */
            /* profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E]        */
            /* to set one template per profile mode. Expected: GT_OK.           */
            profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E;

            st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                         "cpssExMxTgPclProfileSizeSet: %d, %d",
                                         dev, profileSize);

            /* 1.14. For 98EX1x6 devices call with profileId [0],               */
            /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_ANY_E], and the same         */
            /* actionPtr as in 1.2. Expected: GT_OK for 98EX1x6 devices         */
            /* and NON GT_OK for 98EX1x5, 98MX6x5 and 98MX6x8 devices.          */
            profileId = 0;
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_ANY_E;

            st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                        "Tiger device: %d, %d, %d, profileSize = %d",
                                        dev, profileId, keyTemplate, profileSize);

            /* 1.15. Call cpssExMxPclTemplateDefaultActionGet with profileId [0],*/
            /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_ANY_E], non NULL actionPtr.  */
            /* Expected: GT_OK and same values as written.                      */

            /* Fill returned structures with zero */
            cpssOsMemSet((GT_VOID*)&retAction, 0,
                         sizeof(CPSS_EXMX_POLICY_ACTION_STC));

            st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &retAction);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateDefaultActionGet: %d, %d, %d",
                                         dev, profileId, keyTemplate);
            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action,
                                                (const GT_VOID*)&retAction, sizeof(action))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclTemplateDefaultActionGet: get another action than was set: %d, %d, %d",
                                             dev, profileId, keyTemplate);
            }


            /* 1.16. For 98EX1x6 devices check for out of range for profileId.  */
            /* Call with profileId [16], other parameters is the same           */
            /* as in 1.14. Expected: NON GT_OK.                                 */
            profileId = 16;

            st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileId);

            profileId = 0; /* restore valid value */

            /* 1.17. For 98EX1x6 devices check for invalid keyTemplate for 1-16 */
            /* mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], */
            /* other parameters is the same as in 1.14. Expected: NON GT_OK.    */
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;

            st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, %d, %d, profileSize = %d",
                                             dev, profileId, keyTemplate, profileSize);

            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_ANY_E; /* restore valid value */
        }

        /* 1.18. Call cpssExMxPclTcamFlush to cleanup after testing.    */
        /* Expected: GT_OK.                                             */
        cpssExMxPclTcamFlush(dev);

    }

    /* Set valid values for all the parameters (except device id) */
    profileId = 0;
    keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;
    pclActionDefaultSet(&action);

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclRuleSet
(
    IN GT_U8                              devNum,
    IN CPSS_EXMX_PCL_RULE_FORMAT_ENT      ruleFormat,
    IN GT_U32                             ruleIndex,
    IN CPSS_EXMX_PCL_RULE_FIELDS_UNT      *maskPtr,
    IN CPSS_EXMX_PCL_RULE_FIELDS_UNT      *patternPtr,
    IN CPSS_EXMX_POLICY_ACTION_STC           *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclRuleSet)
{
/*
ITERATE_DEVICES
1.1. Check standard rule for 98EX1x6 devices with legal parameters. Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …], patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …], actionPtr [[CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE}, {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0}, {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}]. Expected: GT_OK for 98EX1x6 and NON GT_OK otherwise.
1.2. For 98X1x6 call cpssExMxPclRuleGet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512], non-NULL maskPtr, patternPtr, actionPtr. Expected: GT_OK and same values as written.
1.3. For 98X1x6 devices check for out of range ruleIndex for standard rule format. Call with ruleIndex [2048], other parameters is the same as in 1.1. Expected: NON GT_OK.
1.4. For 98X1x6 devices check for NULL pointers. Call with maskPtr [NULL], other parameters is the same as in 1.1. Expected: GT_BAD_PTR.
1.5. For 98X1x6 devices check for NULL pointers. Call with patternPtr [NULL], other parameters is the same as in 1.1. Expected: GT_BAD_PTR.
1.6. For 98X1x6 devices check for NULL pointers. Call with actionPtr [NULL], other parameters is the same as in 1.1. Expected: GT_BAD_PTR.
1.7. For 98X1x6 devices Check for invalid ruleFormat. Call with ruleFormat [0x5AAAAAA5], other parameters is the same as in 1.1. Expected: GT_BAD_PARAM.
1.8. For 98X1x6 devices call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512] to cleanup after testing. Expected: GT_OK.
1.9. Check extended rule. Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E], ruleIndex [50], maskPtr-> tgExtMac.stdRule is the same as in 1.1., other fileds are zeroed, patternPtr->tgExtMac.stdRule is the same as in 1.1., other fields are zeroed, actionPtr is the same as in 1.1. Expected: GT_OK for 98EX1x6 and NON GT_OK otherwise.
1.10. For 98X1x6 call cpssExMxPclRuleGet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E], ruleIndex [50], non-NULL maskPtr, patternPtr, actionPtr. Expected: GT_OK and same values as written.
1.11. For 98X1x6 devices check for out of range ruleIndex for extended rule format. Call with ruleIndex [1024], other parameters is the same as 1.9. Expected: NON GT_OK.
1.12. For 98X1x6 set standard rule with ruleIndex which is not allowed because of extended rule consumes two spaces of standard rule. Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [50+512], other parameters is the same as in 1.1. Expected: NON GT_OK.
1.13. For 98X1x6 devices call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [50] to cleanup after testing. Expected: GT_OK.
1.14. Check rule format for 98EX1x5 devices. Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0], maskPtr->tdRule [0xFF, 0x00, …], patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …], actionPtr is the same as in 1.1. Expected: GT_OK.
1.15. For 98X1x5 call cpssExMxPclRuleGet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0], non-NULL maskPtr, patternPtr, actionPtr. Expected: GT_OK and same values as written.
1.16. For 98EX1x5 devices check for out of range ruleIndex. Call with ruleIndex [1024], other parameters is the same as in 1.14. Expected: NON GT_OK.
1.17. For 98EX1x5 devices call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    CPSS_EXMX_PCL_RULE_FORMAT_ENT       ruleFormat;
    GT_U32                              ruleIndex;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT       mask;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT       retMask;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT       pattern;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT       retPattern;
    CPSS_EXMX_POLICY_ACTION_STC         action;
    CPSS_EXMX_POLICY_ACTION_STC         retAction;
    GT_BOOL                             failureWas;

    CPSS_PP_FAMILY_TYPE_ENT             devFamily;
    CPSS_EXMX_PCL_RULE_SIZE_ENT         ruleSize;

    pclActionDefaultSet(&action);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        /* 1.1.Check standard rule for 98EX1x6 devices with legal parameters.*/
        /* Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E],        */
        /* ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …],      */
        /* patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …],  */
        /* actionPtr [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E,{GT_FALSE, GT_FALSE}*/
        /* {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …},                            */
        /* {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …},                    */
        /* {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0},                     */
        /* {GT_FALSE, 0, 0},                                                 */
        /* {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}].       */
        /* Expected: GT_OK for 98EX1x6 and NON GT_OK otherwise.              */
        ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
        ruleIndex = 512;

        cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
        mask.tgStd.pclId = 0xFF;
        mask.tgStd.pktType = 0xFF;
        mask.tgStd.ipHdrOk = 0xFF;

        cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
        pattern.tgStd.pclId = 0;
        pattern.tgStd.pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;
        pattern.tgStd.ipHdrOk = 1;

        st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                &action);
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "Tiger device: %d, %d, %d",
                                        dev, ruleFormat, ruleIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "non-Tiger device: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);
        }

        /* Tests for Tiger devices */

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            /* 1.2. For 98X1x6 call cpssExMxPclRuleGet with ruleFormat  */
            /* [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512],   */
            /* non-NULL maskPtr, patternPtr, actionPtr. Expected: GT_OK */
            /* and same values as written.                              */

            /* Fill returned structures with zero */
            cpssOsMemSet((GT_VOID*)&retMask, 0,
                         sizeof(CPSS_EXMX_PCL_RULE_FIELDS_UNT));
            cpssOsMemSet((GT_VOID*)&retPattern, 0,
                         sizeof(CPSS_EXMX_PCL_RULE_FIELDS_UNT));
            cpssOsMemSet((GT_VOID*)&retAction, 0,
                         sizeof(CPSS_EXMX_POLICY_ACTION_STC));

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &retMask, &retPattern,
                                &retAction);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleGet: %d, %d, %d",
                                         dev, ruleFormat, ruleIndex);
            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&mask,
                                                (const GT_VOID*)&retMask, sizeof(mask))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclRuleGet: get another mask than was set: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);

                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&pattern,
                                                (const GT_VOID*)&retPattern, sizeof(pattern))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclRuleGet: get another pattern than was set: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);

                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action,
                                                (const GT_VOID*)&retAction, sizeof(action))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclRuleGet: get another action than was set: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);
            }


            /* 1.3. For 98X1x6 devices check for out of range ruleIndex for      */
            /* standard rule format. Call with ruleIndex [2048], other parameters*/
            /* is the same as in 1.1. Expected: NON GT_OK.                       */
            ruleIndex = 2048;

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

            ruleIndex = 512; /* restore valid value */

            /* 1.4. For 98X1x6 devices check for NULL pointers.                  */
            /* Call with maskPtr [NULL], other parameters is the same as in 1.1. */
            /* Expected: GT_BAD_PTR.                                             */

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, NULL, &pattern,
                                    &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, %d, maskPtr = NULL",
                                         dev, ruleFormat, ruleIndex);

            /* 1.5. For 98X1x6 devices check for NULL pointers.                  */
            /* Call with patternPtr [NULL], other parameters is the same         */
            /* as in 1.1. Expected: GT_BAD_PTR.                                  */
            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, NULL,
                                    &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, %d, patternPtr = NULL",
                                         dev, ruleFormat, ruleIndex);

            /* 1.6. For 98X1x6 devices check for NULL pointers.                  */
            /* Call with actionPtr [NULL], other parameters is the same as in 1.1*/
            /* Expected: GT_BAD_PTR.                                             */
            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, %d, actionPtr = NULL",
                                         dev, ruleFormat, ruleIndex);

            /* 1.7. For 98X1x6 devices Check for invalid ruleFormat.             */
            /* Call with ruleFormat [0x5AAAAAA5], other parameters is the same   */
            /* as in 1.1.Expected: GT_BAD_PARAM.                                 */
            ruleFormat = PCL_INVALID_ENUM_CNS;

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleFormat);

            /* restore valid value */
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E; 

            /* 1.8. For 98X1x6 devices call cpssExMxPclRuleInvalidate with       */
            /* ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512]         */
            /* to cleanup after testing. Expected: GT_OK.                        */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                         "cpssExMxPclRuleInvalidate: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
        }

        /* 1.9. Check extended rule. Call with                               */
        /* ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E],              */
        /* ruleIndex [50], maskPtr-> tgExtMac.stdRule is the same as in 1.1. */
        /* other fileds are zeroed, patternPtr->tgExtMac.stdRule is the same */
        /* as in 1.1., other fields are zeroed, actionPtr is the same        */
        /* as in 1.1. Expected: GT_OK for 98EX1x6 and NON GT_OK otherwise.   */
        ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E;
        ruleIndex = 50;

        cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
        mask.tgExtMac.stdRule.pclId = 0xFF;
        mask.tgExtMac.stdRule.pktType = 0xFF;
        mask.tgExtMac.stdRule.ipHdrOk = 0xFF;

        cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
        pattern.tgExtMac.stdRule.pclId = 0;
        pattern.tgExtMac.stdRule.pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;
        pattern.tgExtMac.stdRule.ipHdrOk = 1;

        st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                &action);
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "Tiger device: %d, %d, %d",
                                        dev, ruleFormat, ruleIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "non-Tiger device: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);
        }

        /* Tests for Tiger devices */

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            /* 1.10. For 98X1x6 call cpssExMxPclRuleGet with ruleFormat  */
            /* [CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E], ruleIndex [50],*/
            /* non-NULL maskPtr, patternPtr, actionPtr. Expected: GT_OK */
            /* and same values as written.                              */

            /* Fill returned structures with zero */
            cpssOsMemSet((GT_VOID*)&retMask, 0,
                         sizeof(CPSS_EXMX_PCL_RULE_FIELDS_UNT));
            cpssOsMemSet((GT_VOID*)&retPattern, 0,
                         sizeof(CPSS_EXMX_PCL_RULE_FIELDS_UNT));
            cpssOsMemSet((GT_VOID*)&retAction, 0,
                         sizeof(CPSS_EXMX_POLICY_ACTION_STC));

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &retMask, &retPattern,
                                &retAction);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleGet: %d, %d, %d",
                                         dev, ruleFormat, ruleIndex);
            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&mask,
                                                (const GT_VOID*)&retMask, sizeof(mask))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclRuleGet: get another mask than was set: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);

                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&pattern,
                                                (const GT_VOID*)&retPattern, sizeof(pattern))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclRuleGet: get another pattern than was set: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);

                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action,
                                                (const GT_VOID*)&retAction, sizeof(action))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclRuleGet: get another action than was set: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);
            }


            /* 1.11. For 98X1x6 devices check for out of range ruleIndex          */
            /* for extended rule format. Call with ruleIndex [1024],             */
            /* other parameters is the same as 1.9. Expected: NON GT_OK.         */
            ruleIndex = 1024;

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

            ruleIndex = 50; /* restore valid value */

            /* 1.12. For 98X1x6 set standard rule with ruleIndex                    */
            /* which is not allowed because of extended rule consumes               */
            /* two spaces of standard rule. Call with                               */
            /* ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [50+512], */
            /* other parameters is the same as in 1.1. Expected: NON GT_OK.         */
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
            ruleIndex = 50 + 512;

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

            /* restore valid values */
            ruleIndex = 50;
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E;

            /* 1.13. For 98X1x6 devices call cpssExMxPclRuleInvalidate with      */
            /* ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [50]          */
            /* to cleanup after testing. Expected: GT_OK.                        */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                         "cpssExMxPclRuleInvalidate: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
        }

        /* 1.14. Check rule format for 98EX1x5 devices.                      */
        /* Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E],            */
        /* ruleIndex [0], maskPtr->tdRule [0xFF, 0x00, …],                   */
        /* patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …],       */
        /* actionPtr is the same as in 1.1. Expected: GT_OK                  */
        /* for 98EX1x5 devices and NON GT_OK otherwise.                      */
        ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TD_E;
        ruleIndex = 0;

        cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
        mask.tdRule.pktType = 0xFF;

        cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
        pattern.tdRule.pktType = CPSS_EXMX_PCL_PKT_IPV4_UDP_E;

        st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                &action);
        if (CPSS_PP_FAMILY_TWISTD_E == devFamily)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "Twist-D device: %d, %d, %d",
                                        dev, ruleFormat, ruleIndex);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "non Twist-D device: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);
        }

        /* Tests for Twist-D devices */

        if (CPSS_PP_FAMILY_TWISTD_E == devFamily)
        {
            /* 1.15. For 98X1x5 call cpssExMxPclRuleGet with ruleFormat      */
            /* [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0], non-NULL    */
            /* maskPtr, patternPtr, actionPtr. Expected: GT_OK and same     */
            /* values as written.                                           */

            /* Fill returned structures with zero */
            cpssOsMemSet((GT_VOID*)&retMask, 0,
                         sizeof(CPSS_EXMX_PCL_RULE_FIELDS_UNT));
            cpssOsMemSet((GT_VOID*)&retPattern, 0,
                         sizeof(CPSS_EXMX_PCL_RULE_FIELDS_UNT));
            cpssOsMemSet((GT_VOID*)&retAction, 0,
                         sizeof(CPSS_EXMX_POLICY_ACTION_STC));

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &retMask, &retPattern,
                                &retAction);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleGet: %d, %d, %d",
                                         dev, ruleFormat, ruleIndex);
            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&mask,
                                                (const GT_VOID*)&retMask, sizeof(mask))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclRuleGet: get another mask than was set: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);

                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&pattern,
                                                (const GT_VOID*)&retPattern, sizeof(pattern))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclRuleGet: get another pattern than was set: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);

                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action,
                                                (const GT_VOID*)&retAction, sizeof(action))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclRuleGet: get another action than was set: %d, %d, %d",
                                             dev, ruleFormat, ruleIndex);
            }


            /* 1.16. For 98EX1x5 devices check for out of range ruleIndex.       */
            /* Call with ruleIndex [1024], other parameters is the same          */
            /* as in 1.14. Expected: NON GT_OK.                                  */
            ruleIndex = 1024;

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

            ruleIndex = 0; /* restore valid value */

            /* 1.17. For 98EX1x5 devices call cpssExMxPclRuleInvalidate with     */
            /* ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0]           */
            /* to cleanup after testing. Expected: GT_OK.                        */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                         "cpssExMxPclRuleInvalidate: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
        }
    }

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
    ruleIndex = 512;
    cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
    cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                            &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclRuleActionUpdate
(
    IN GT_U8                              devNum,
    IN CPSS_EXMX_PCL_RULE_SIZE_ENT        ruleSize,
    IN GT_U32                             ruleIndex,
    IN CPSS_EXMX_POLICY_ACTION_STC           *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclRuleActionUpdate)
{
/*
ITERATE_DEVICES
1.1. For 98X1x6 devices set rule for testing. Call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …], patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …], actionPtr [[CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE}, {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0}, {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}]. Expected: GT_OK.
1.2. For 98X1x6 update action with legal parameters. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512], actionPtr is the same as in 1.1. Expected: GT_OK.
1.3. For 98X1x6 call cpssExMxPclRuleActionGet with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512], non-NULL actionPtr. Expected: GT_OK and same action as written.
1.4. For 98X1x6 check with rule size for other type of rule. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [512], actionPtr is the same as in 1.1. Expected: NON GT_OK.
1.5. For 98X1x6 check invalid rule size value. Call with ruleSize [0x5AAAAA5], ruleIndex [512], actionPtr is the same as in 1.1. Expected: GT_BAD_PARAM.
1.6. For 98X1x6 check out of range ruleIndex. Call with ruleIndex [2048], other parameters is the same as in 1.1. Expected: NON GT_OK.
1.7. For 98X1x6 check for NULL pointer. Call with actionPtr [NULL]. Expected: GT_BAD_PTR.
1.8. For 98X1x6 devices call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512] to cleanup after testing. Expected: GT_OK.
1.9. For 98EX1x5 devices set rule for testing. Call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0], maskPtr->tdRule [0xFF, 0x00, …], patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …], actionPtr is the same as in 1.1. Expected: GT_OK.
1.10. For 98EX1x5 update action with legal parameters. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0], actionPtr is the same as in 1.1. Expected: GT_OK.
1.11. For 98X1x5 call cpssExMxPclRuleActionGet with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0], non-NULL actionPtr. Expected: GT_OK and same action as written.
1.12. For 98EX1x5 check with unsupported rule size. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [0], actionPtr is the same as in 1.1. Expected: NON GT_OK.
1.13. For 98EX1x5 check invalid rule size value. Call with ruleSize [0x5AAAAA5], ruleIndex [0], actionPtr is the same as in 1.1. Expected: GT_BAD_PARAM.
1.14. For 98EX1x5 check out of range ruleIndex. Call with ruleIndex [1024], other parameters is the same as in 1.1. Expected: NON GT_OK.
1.15. For 98EX1x5 check for NULL pointer. Call with actionPtr [NULL]. Expected: GT_BAD_PTR.
1.16. For 98EX1x5 devices call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    CPSS_EXMX_PCL_RULE_SIZE_ENT         ruleSize;
    GT_U32                              ruleIndex;
    CPSS_EXMX_POLICY_ACTION_STC         action;
    CPSS_EXMX_POLICY_ACTION_STC         retAction;
    GT_BOOL                             failureWas;

    CPSS_EXMX_PCL_RULE_FORMAT_ENT       ruleFormat;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT       mask;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT       pattern;
    CPSS_PP_FAMILY_TYPE_ENT             devFamily;

    /* prepare <action> (it shouldn't change during the test) */
    pclActionDefaultSet(&action);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            /* 1.1. For 98X1x6 devices set rule for testing. Call               */
            /* cpssExMxPclRuleSet with ruleFormat                               */
            /* [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512],           */
            /* maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …],                      */
            /* patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …], */
            /* actionPtr [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E,                   */
            /* {GT_FALSE, GT_FALSE}, {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …},     */
            /* {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …},                   */
            /* {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0},  */
            /* {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}].      */
            /* Expected: GT_OK.                                                 */
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
            ruleIndex = 512;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            mask.tgStd.pclId = 0xFF;
            mask.tgStd.pktType = 0xFF;
            mask.tgStd.ipHdrOk = 0xFF;

            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.tgStd.pclId = 0;
            pattern.tgStd.pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;
            pattern.tgStd.ipHdrOk = 1;

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "cpssExMxPclRuleSet: Tiger device: %d, %d, %d",
                                        dev, ruleFormat, ruleIndex);

            /* 1.2. For 98X1x6 update action with legal parameters. Call with   */
            /* ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512],       */
            /* actionPtr is the same as in 1.1. Expected: GT_OK.                */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

            st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "Tiger device: %d, %d, %d",
                                        dev, ruleSize, ruleIndex);

            /* 1.3. For 98X1x6 call cpssExMxPclRuleActionGet with ruleSize      */
            /* [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512], non-NULL       */
            /* actionPtr. Expected: GT_OK and same action as written.           */

            /* Fill retAction with non-zero */
            cpssOsMemSet((GT_VOID*)&retAction, 0xFF,
                         sizeof(CPSS_EXMX_POLICY_ACTION_STC));

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &retAction);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleActionGet: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action,
                                                (const GT_VOID*)&retAction, sizeof(action))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclRuleActionGet: get another action than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);
            }

            /* 1.4. For 98X1x6 check with rule size for other type of rule.     */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E],              */
            /* ruleIndex [512], actionPtr is the same as in 1.1.                */
            /* Expected: NON GT_OK.                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;

            st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E; /* restore valid values */

            /* 1.5. For 98X1x6 check invalid rule size value. Call with         */
            /* ruleSize [0x5AAAAA5], ruleIndex [512], actionPtr is the same     */
            /* as in 1.1. Expected: GT_BAD_PARAM.                               */
            ruleSize = PCL_INVALID_ENUM_CNS;

            st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleIndex);

            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E; /* restore valid values */

            /* 1.6. For 98X1x6 check out of range ruleIndex. Call with          */
            /* ruleIndex [2048], other parameters is the same as in 1.1.        */
            /* Expected: NON GT_OK.                                             */
            ruleIndex = 2048;

            st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, 
                                             "Tiger device: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

            ruleIndex = 512; /* restore valid value */

            /* 1.7. For 98X1x6 check for NULL pointer. Call with                */
            /* actionPtr [NULL]. Expected: GT_BAD_PTR.                          */

            st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL",
                                         dev, ruleSize, ruleIndex);

            /* 1.8. For 98X1x6 devices call cpssExMxPclRuleInvalidate with      */
            /* ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512]        */
            /* to cleanup after testing. Expected: GT_OK.                       */

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                         "cpssExMxPclRuleInvalidate: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
        }

        if (CPSS_PP_FAMILY_TWISTD_E == devFamily)
        {
            /* 1.9. For 98EX1x5 devices set rule for testing.                   */
            /* Call cpssExMxPclRuleSet with                                     */
            /* ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0],      */
            /* maskPtr->tdRule [0xFF, 0x00, …],                                 */
            /* patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …],      */
            /* actionPtr is the same as in 1.1. Expected: GT_OK.                */
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TD_E;
            ruleIndex = 0;
            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            mask.tdRule.pktType = 0xFF;
            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.tdRule.pktType = CPSS_EXMX_PCL_PKT_IPV4_UDP_E;

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "cpssExMxPclRuleSet: Twist-D device: %d, %d, %d",
                                        dev, ruleFormat, ruleIndex);

            /* 1.10. For 98EX1x5 update action with legal parameters.           */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E],              */
            /* ruleIndex [0], actionPtr is the same as in 1.1.                  */
            /* Expected: GT_OK.                                                 */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

            st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "Twist-D device: %d, %d, %d",
                                        dev, ruleSize, ruleIndex);

            /* 1.11. For 98X1x5 call cpssExMxPclRuleActionGet with ruleSize     */
            /* [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0], non-NULL         */
            /* actionPtr. Expected: GT_OK and same action as written.           */

            /* Fill retAction with non-zero */
            cpssOsMemSet((GT_VOID*)&retAction, 0xFF,
                         sizeof(CPSS_EXMX_POLICY_ACTION_STC));

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &retAction);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleActionGet: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
            if (GT_OK == st)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&action,
                                                (const GT_VOID*)&retAction, sizeof(action))) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                                             "cpssExMxPclRuleActionGet: get another action than was set: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);
            }

            /* 1.12. For 98EX1x5 check with unsupported rule size.              */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [0]*/
            /* actionPtr is the same as in 1.1. Expected: NON GT_OK.            */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;

            st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "Twist-D device: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E; /* restore valid value */

            /* 1.13. For 98EX1x5 check invalid rule size value. Call with       */
            /* ruleSize [0x5AAAAA5], ruleIndex [0], actionPtr is the same       */
            /* as in 1.1. Expected: GT_BAD_PARAM.                               */
            ruleSize = PCL_INVALID_ENUM_CNS;

            st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleIndex);

            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E; /* restore valid value */

            /* 1.14. For 98EX1x5 check out of range ruleIndex.                  */
            /* Call with ruleIndex [1024], other parameters is the same         */
            /* as in 1.1. Expected: NON GT_OK.                                  */
            ruleIndex = 1024;

            st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "Twist-D device: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

            ruleIndex = 0; /* restore valid value */

            /* 1.15. For 98EX1x5 check for NULL pointer. Call with              */
            /* actionPtr [NULL]. Expected: GT_BAD_PTR.                          */

            st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL",
                                         dev, ruleSize, ruleIndex);

            /* 1.16. For 98EX1x5 devices call cpssExMxPclRuleInvalidate         */
            /* with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0]     */
            /* to cleanup after testing. Expected: GT_OK.                       */

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                         "cpssExMxPclRuleInvalidate: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
        }
    }

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
    ruleIndex = 512;

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclRuleActionUpdate(dev, ruleSize, ruleIndex, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclRuleInvalidate
(
    IN GT_U8                              devNum,
    IN CPSS_EXMX_PCL_RULE_SIZE_ENT        ruleSize,
    IN GT_U32                             ruleIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclRuleInvalidate)
{
/*
ITERATE_DEVICES
1.1. For 98X1x6 devices set rule for testing. Call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …], patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …], actionPtr [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE}, {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0}, {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}]. Expected: GT_OK.
1.2. For 98X1x6 devices check for invalid ruleSize for rule which was written as standard rule. Call with invalid ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [512]. Expected: NON GT_OK.
1.3. For 98X1x6 devices invalidate written rule with legal parameters. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512]. Expected: GT_OK.
1.4. For 98X1x6 devices check for invalid enum value for ruleSize. Call with ruleSize [0x5AAAAAA5], ruleIndex [512]. Expected: GT_BAD_PARAM.
1.5. For 98X1x6 devices check for out of range ruleIndex. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [2048]. Expected: NON GT_OK.
1.6. For 98EX1x5 devices set rule for testing. Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0], maskPtr->tdRule [0xFF, 0x00, …], patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …], actionPtr is the same as in 1.1. Expected: GT_OK.
1.7. For 98EX1x5 invalidate rule with unsuppored ruleSize. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [0]. Expected: NON GT_OK.
1.8. For 98EX1x5 invalidate written rule with legal parameters. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0]. Expected: GT_OK.
1.9. For 98EX1x5 check invalid rule size value. Call with ruleSize [0x5AAAAA5], ruleIndex [0]. Expected: GT_BAD_PARAM.
1.10. For 98EX1x5 check out of range ruleIndex. Call with ruleIndex [1024]. Expected: NON GT_OK.
*/
    GT_STATUS                       st = GT_OK;
    GT_U8                           dev;
    GT_U32                          ruleIndex;
    CPSS_EXMX_PCL_RULE_SIZE_ENT     ruleSize;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        CPSS_PP_FAMILY_TYPE_ENT             devFamily;
        CPSS_EXMX_PCL_RULE_FORMAT_ENT       ruleFormat;
        CPSS_EXMX_PCL_RULE_FIELDS_UNT       mask;
        CPSS_EXMX_PCL_RULE_FIELDS_UNT       pattern;
        CPSS_EXMX_POLICY_ACTION_STC            action;

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For 98X1x6 devices set rule for testing.                                                */
        /* Call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E],                */
        /* ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …],                                 */
        /* patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …],                             */
        /* actionPtr [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE},                         */
        /* {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …},        */
        /* {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0},                              */
        /* {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}].                                  */
        /* Expected: GT_OK.                                                                             */
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
            ruleIndex = 512;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            mask.tgStd.pclId = 0xFF;
            mask.tgStd.pktType = 0xFF;
            mask.tgStd.ipHdrOk = 0xFF;

            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.tgStd.pclId = 0;
            pattern.tgStd.pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;
            pattern.tgStd.ipHdrOk = 1;

            pclActionDefaultSet(&action);

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleSet: %d, %d, %d", dev, ruleFormat, ruleIndex);

            /* 1.2. For 98X1x6 devices check for invalid ruleSize for rule which was                        */
            /* written as standard rule. Call with invalid ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E],        */
            /* ruleIndex [512].                                                                             */
            /* Expected: NON GT_OK.                                                                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;
            ruleIndex = 512;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

            /* 1.3. For 98X1x6 devices invalidate written rule with legal parameters.                       */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512].                         */
            /* Expected: GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 512;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

            /* 1.4. For 98X1x6 devices check for invalid enum value for ruleSize.                           */
            /* Call with ruleSize [0x5AAAAAA5], ruleIndex [512].                                            */
            /* Expected: GT_BAD_PARAM.                                                                      */
            ruleSize = PCL_INVALID_ENUM_CNS;
            ruleIndex = 512;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleIndex);

            /* 1.5. For 98X1x6 devices check for out of range ruleIndex.                                    */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [2048].                        */
            /* Expected: NON GT_OK.                                                                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 2048;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);
        }

        /* 1.6. For 98EX1x5 devices set rule for testing.                                               */
        /* Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0],                        */
        /* maskPtr->tdRule [0xFF, 0x00, …], patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …], */
        /* actionPtr is the same as in 1.1.                                                             */
         /* Expected: GT_OK.                                                                            */
        if (CPSS_PP_FAMILY_TWISTD_E == devFamily)
        {
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TD_E;
            ruleIndex = 0;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            pattern.tdRule.pktType = 0xFF;

            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.tdRule.pktType = CPSS_EXMX_PCL_PKT_IPV4_UDP_E;

            pclActionDefaultSet(&action);

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleSet: %d, %d, %d", dev, ruleFormat, ruleIndex);

            /* 1.7. For 98EX1x5 invalidate rule with unsuppored ruleSize.                                   */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [0].                           */
            /* Expected: NON GT_OK.                                                                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;
            ruleIndex = 0;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

            /* 1.8. For 98EX1x5 invalidate written rule with legal parameters.                              */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0].                           */
            /* Expected: GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 0;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);

            /* 1.9. For 98EX1x5 check invalid rule size value.                                              */
            /* Call with ruleSize [0x5AAAAA5], ruleIndex [0].                                               */
            /* Expected: GT_BAD_PARAM.                                                                      */
            ruleSize = PCL_INVALID_ENUM_CNS;
            ruleIndex = 0;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleIndex);

            /* 1.10. For 98EX1x5 check out of range ruleIndex. Call with ruleIndex [1024].                  */
            /* Expected: NON GT_OK.                                                                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 1024;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex);
        }
    }

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
    ruleIndex = 0;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclRuleCopy
(
    IN GT_U8                              devNum,
    IN CPSS_EXMX_PCL_RULE_SIZE_ENT        ruleSize,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclRuleCopy)
{
/*
ITERATE_DEVICES
1.1. For 98X1x6 devices set rule for testing. Call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …], patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …], actionPtr [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE}, {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0}, {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}]. Expected: GT_OK.
1.2. For 98X1x6 devices try to copy with ruleSize which isn't applicable for standard rule. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleSrcIndex [512], ruleDstIndex [0]. Expected: NON GT_OK.
1.3. For 98X1x6 devices try to copy with invalid enum value for ruleSize. Call with ruleSize [0x5AAAAAA5], ruleSrcIndex [512], ruleDstIndex [0]. Expected: GT_BAD_PARAM.
1.4. For 98X1x6 devices try to copy with out of range source rule index. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleSrcIndex [2048], ruleDstIndex [0]. Expected: NON GT_OK.
1.5. For 98X1x6 devices try to copy with out of range destination rule index. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleStrIndex [512], ruleDstIndex [2048]. Expected: NON GT_OK.
1.6. For 98X1x6 devices try to copy with legal parameters. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleSrcIndex [512], ruleDstIndex [0]. Expected: GT_OK.
1.7. For 98X1x6 devices invalidate source rule. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512]. Expected: GT_OK.
1.8. For 98X1x6 devices invalidate destination rule. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0]. Expected: GT_OK.
1.9. For 98EX1x5 devices set rule for testing. Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0], maskPtr->tdRule [0xFF, 0x00, …], patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …], actionPtr is the same as in 1.1. Expected: GT_OK.
1.10. For 98EX1x5 devices try to copy with unsupported ruleSize. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleSrcIndex [0], ruleDstIndex [512]. Expected: NON GT_OK.
1.11. For 98EX1x5 devices try to copy with invalid enum value for ruleSize. Call with ruleSize [0x5AAAAAA5], ruleSrcIndex [0], ruleDstIndex [512]. Expected: GT_BAD_PARAM.
1.12. For 98EX1x5 devices try to copy with out of range source rule index. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleSrcIndex [1024], ruleDstIndex [512]. Expected: NON GT_OK.
1.13. For 98EX1x5 devices try to copy with out of range destination rule index. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleStrIndex [0], ruleDstIndex [1024]. Expected: NON GT_OK.
1.14. For 98EX1x5 devices try to copy with legal parameters. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleSrcIndex [0], ruleDstIndex [512]. Expected: GT_OK.
1.15. For 98EX1x5 devices invalidate source rule. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0]. Expected: GT_OK.
1.16. For 98EX1x5 devices invalidate destination rule. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512]. Expected: GT_OK.
*/
    GT_STATUS                       st = GT_OK;
    GT_U8                           dev;
    GT_U32                          ruleSrcIndex;
    GT_U32                          ruleDstIndex;
    CPSS_EXMX_PCL_RULE_SIZE_ENT     ruleSize;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_U32                              ruleIndex;
        CPSS_PP_FAMILY_TYPE_ENT             devFamily;
        CPSS_EXMX_PCL_RULE_FORMAT_ENT       ruleFormat;
        CPSS_EXMX_PCL_RULE_FIELDS_UNT       mask;
        CPSS_EXMX_PCL_RULE_FIELDS_UNT       pattern;
        CPSS_EXMX_POLICY_ACTION_STC            action;

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For 98X1x6 devices set rule for testing.                                                    */
        /* Call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E],                    */
        /* ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …],                                     */
        /* patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …],                                 */
        /* actionPtr [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE},                             */
        /* {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …},            */
        /* {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0},                                  */
        /*{CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}].                                       */
        /* Expected: GT_OK.                                                                                 */
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
            ruleIndex = 512;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            mask.tgStd.pclId = 0xFF;
            mask.tgStd.pktType = 0xFF;
            mask.tgStd.ipHdrOk = 0xFF;

            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.tgStd.pclId = 0;
            pattern.tgStd.pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;
            pattern.tgStd.ipHdrOk = 1;

            pclActionDefaultSet(&action);

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleSet: %d, %d, %d", dev, ruleFormat, ruleIndex);

            /* 1.2. For 98X1x6 devices try to copy with ruleSize which isn't applicable for standard rule.      */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleSrcIndex [512], ruleDstIndex [0].        */
            /* Expected: NON GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;
            ruleSrcIndex = 512;
            ruleDstIndex = 0;

            st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleSrcIndex, ruleDstIndex);

            /* 1.3. For 98X1x6 devices try to copy with invalid enum value for ruleSize.                        */
            /* Call with ruleSize [0x5AAAAAA5], ruleSrcIndex [512], ruleDstIndex [0].                           */
            /* Expected: GT_BAD_PARAM.                                                                          */
            ruleSize = PCL_INVALID_ENUM_CNS;
            ruleSrcIndex = 512;
            ruleDstIndex = 0;

            st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleSrcIndex, ruleDstIndex);

            /* 1.4. For 98X1x6 devices try to copy with out of range source rule index.                         */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleSrcIndex [2048], ruleDstIndex [0].       */
            /* Expected: NON GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleSrcIndex = 2048;
            ruleDstIndex = 0;

            st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleSrcIndex, ruleDstIndex);

            /* 1.5. For 98X1x6 devices try to copy with out of range destination rule index.                    */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleStrIndex [512], ruleDstIndex [2048].     */
            /* Expected: NON GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleSrcIndex = 512;
            ruleDstIndex = 2048;

            st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleSrcIndex, ruleDstIndex);

            /* 1.6. For 98X1x6 devices try to copy with legal parameters.                                       */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleSrcIndex [512], ruleDstIndex [0].        */
            /* Expected: GT_OK.                                                                                 */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleSrcIndex = 512;
            ruleDstIndex = 0;

            st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleSrcIndex, ruleDstIndex);

            /* 1.7. For 98X1x6 devices invalidate source rule.                                                  */
            /* Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512].   */
            /* Expected: GT_OK.                                                                                 */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 512;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);

            /* 1.8. For 98X1x6 devices invalidate destination rule.                                             */
            /* Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0].     */
            /* Expected: GT_OK.                                                                                 */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 0;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
        }

        /* 1.9. For 98EX1x5 devices set rule for testing.                                                   */
        /* Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0],                            */
        /* maskPtr->tdRule [0xFF, 0x00, …], patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …],     */
        /* actionPtr is the same as in 1.1.                                                                 */
        /* Expected: GT_OK.                                                                                 */
        if (CPSS_PP_FAMILY_TWISTD_E == devFamily)
        {
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TD_E;
            ruleIndex = 0;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            pattern.tdRule.pktType = 0xFF;

            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.tdRule.pktType = CPSS_EXMX_PCL_PKT_IPV4_UDP_E;

            pclActionDefaultSet(&action);

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleSet: %d, %d, %d", dev, ruleFormat, ruleIndex);

            /* 1.10. For 98EX1x5 devices try to copy with unsupported ruleSize.                                 */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleSrcIndex [0], ruleDstIndex [512].        */
            /* Expected: NON GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;
            ruleSrcIndex = 0;
            ruleDstIndex = 512;

            st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleSrcIndex, ruleDstIndex);

            /* 1.11. For 98EX1x5 devices try to copy with invalid enum value for ruleSize.                      */
            /* Call with ruleSize [0x5AAAAAA5], ruleSrcIndex [0], ruleDstIndex [512].                           */
            /* Expected: GT_BAD_PARAM.                                                                          */
            ruleSize = PCL_INVALID_ENUM_CNS;
            ruleSrcIndex = 0;
            ruleDstIndex = 512;

            st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleSrcIndex, ruleDstIndex);

            /* 1.12. For 98EX1x5 devices try to copy with out of range source rule index.                       */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleSrcIndex [1024], ruleDstIndex [512].     */
            /* Expected: NON GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleSrcIndex = 1024;
            ruleDstIndex = 512;

            st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleSrcIndex, ruleDstIndex);

            /* 1.13. For 98EX1x5 devices try to copy with out of range destination rule index.                  */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleStrIndex [0], ruleDstIndex [1024].       */
            /* Expected: NON GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleSrcIndex = 0;
            ruleDstIndex = 1024;

            st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleSrcIndex, ruleDstIndex);

            /* 1.14. For 98EX1x5 devices try to copy with legal parameters.                                     */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleSrcIndex [0], ruleDstIndex [512].        */
            /* Expected: GT_OK.                                                                                 */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleSrcIndex = 0;
            ruleDstIndex = 512;

            st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleSrcIndex, ruleDstIndex);

            /* 1.15. For 98EX1x5 devices invalidate source rule.                                                */
            /* Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0].     */
            /* Expected: GT_OK.                                                                                 */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 0;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);

            /* 1.16. For 98EX1x5 devices invalidate destination rule.                                           */
            /* Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512].   */
            /* Expected: GT_OK.                                                                                 */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 512;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
        }
    }

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
    ruleSrcIndex = 0;
    ruleDstIndex = 512;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclRuleCopy(dev, ruleSize, ruleSrcIndex, ruleDstIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclEngineModeSet
(
    IN GT_U8                         devNum,
    IN CPSS_EXMX_PCL_ENGINE_MODE_ENT pclEngineMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclEngineModeSet)
{
/*
ITERATE_DEVICES
1.1. Call with pclEngineMode [CPSS_EXMX_PCL_ENGINE_MODE_ACTIVE_E]. Expected: GT_OK for Tiger, Samba/Rumba and non GT_OK for others.
1.2. For Tiger, Samba/Rumba family check for invalid enum value. Call with pclEngineMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                           dev;
    CPSS_EXMX_PCL_ENGINE_MODE_ENT   pclEngineMode;

    CPSS_PP_FAMILY_TYPE_ENT         devFamily;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        /* 1.1. Call with pclEngineMode [CPSS_EXMX_PCL_ENGINE_MODE_ACTIVE_E].*/
        /* Expected: GT_OK for Tiger, Samba/Rumba and non GT_OK for others.  */

        pclEngineMode = CPSS_EXMX_PCL_ENGINE_MODE_ACTIVE_E;

        st = cpssExMxPclEngineModeSet(dev, pclEngineMode);

        if ((CPSS_PP_FAMILY_TIGER_E == devFamily) || (CPSS_PP_FAMILY_SAMBA_E == devFamily))
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,"Tiger, Samba/Rumba device: %d, %d",
                                         dev, pclEngineMode);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,"non - (Tiger or Samba/Rumba) device: %d, %d",
                                         dev, pclEngineMode);
        }

        /* 1.2. For Tiger, Samba/Rumba family check for invalid enum value. */
        /* Call with pclEngineMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.    */

        if ((CPSS_PP_FAMILY_TIGER_E == devFamily) || (CPSS_PP_FAMILY_SAMBA_E == devFamily))
        {
            pclEngineMode = PCL_INVALID_ENUM_CNS;

            st = cpssExMxPclEngineModeSet(dev, pclEngineMode);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,"Tiger, Samba/Rumba device: %d, %d",
                                         dev, pclEngineMode);
        }

    }

    pclEngineMode = CPSS_EXMX_PCL_ENGINE_MODE_ACTIVE_E;

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclEngineModeSet(dev, pclEngineMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclEngineModeSet(dev, pclEngineMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclTwoLookupsActionSelectSet
(
    IN GT_U8                                           devNum,
    IN CPSS_EXMX_PCL_TWO_LOOKUPS_ACTION_SELECTION_STC  *actionSelectionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclTwoLookupsActionSelectSet)
{
/*
ITERATE_DEVICES
1.1. Call with non-NULL actionSelectionPtr [CPSS_EXMX_PCL_ACTION_FROM_LOOKUP0_E, …]. Expected: GT_OK for Tiger, Samba/Rumba and non GT_OK for others.
1.2. For Tiger, Samba/Rumba family call with actionSelectionPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                           dev;
    CPSS_EXMX_PCL_TWO_LOOKUPS_ACTION_SELECTION_STC  actionSelection;

    CPSS_PP_FAMILY_TYPE_ENT         devFamily;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    cpssOsBzero((GT_VOID*)&actionSelection, sizeof(actionSelection));

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        /* 1.1. Call with non-NULL actionSelectionPtr                       */
        /* [CPSS_EXMX_PCL_ACTION_FROM_LOOKUP0_E, …]. Expected: GT_OK for    */
        /* Tiger, Samba/Rumba and non GT_OK for others.                     */
        
        actionSelection.packetCmd = CPSS_EXMX_PCL_ACTION_FROM_LOOKUP0_E;

        st = cpssExMxPclTwoLookupsActionSelectSet(dev, &actionSelection);

        if ((CPSS_PP_FAMILY_TIGER_E == devFamily) || (CPSS_PP_FAMILY_SAMBA_E == devFamily))
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "Tiger or Samba/Rumba device: %d, actionSelection.packetCmd = %d",
                                         dev, actionSelection.packetCmd);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                         "non - (Tiger or Samba/Rumba) device: %d, actionSelection.packetCmd = %d",
                                         dev, actionSelection.packetCmd);
        }

        /* 1.2. For Tiger, Samba/Rumba family call with actionSelectionPtr  */
        /* [NULL]. Expected: GT_BAD_PTR.                                    */

        if ((CPSS_PP_FAMILY_TIGER_E == devFamily) || (CPSS_PP_FAMILY_SAMBA_E == devFamily))
        {
            st = cpssExMxPclTwoLookupsActionSelectSet(dev, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
        }
    }

    actionSelection.packetCmd = CPSS_EXMX_PCL_ACTION_FROM_LOOKUP0_E;

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclTwoLookupsActionSelectSet(dev, &actionSelection);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclTwoLookupsActionSelectSet(dev, &actionSelection);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclRulePolicerActionUpdate
(
    IN GT_U8                                 devNum,
    IN CPSS_EXMX_PCL_RULE_SIZE_ENT           ruleSize,
    IN GT_U32                                ruleIndex,
    IN CPSS_EXMX_POLICY_ACTION_POLICER_MODE_ENT policerMode,
    IN GT_U32                                policerId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclRulePolicerActionUpdate)
{
/*
ITERATE_DEVICES
1.1. For 98X1x6 devices set rule for testing. Call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …], patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …], actionPtr [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE}, {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0}, {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}]. Expected: GT_OK.
1.2. For 98X1x6 devices call with ruleSize which isn't applicable for standard rule. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [512], policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0]. Expected: NON GT_OK.
1.3. For 98X1x6 devices call with invalid ruleSize enum value. Call with ruleSize [0x5AAAAAA5], ruleIndex [512], policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0]. Expected: GT_BAD_PARAM.
1.4. For 98X1x6 devices call with out of range ruleIndex. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [2048], policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0]. Expected: NON GT_OK.
1.5. For 98X1x6 devices call with invalid enum value for policerMode. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512], policerMode [0x5AAAAAA5], policerId [0]. Expected: GT_BAD_PARAM.
1.6. For 98X1x6 devices call with out of range policerId. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512], policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0xEFFFFFF5]. Expected: NON GT_OK.
1.7. For 98X1x6 devices call with legal parameters. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512], policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0]. Expected: GT_OK.
1.8. For 98X1x6 devices invalidate tested rule. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512]. Expected: GT_OK.
1.9. For 98EX1x5 devices set rule for testing. Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0], maskPtr->tdRule [0xFF, 0x00, …], patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …], actionPtr is the same as in 1.1. Expected: GT_OK.
1.10. For 98EX1x5 devices call with unsupported ruleSize. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [0], policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0]. Expected: NON GT_OK.
1.11. For 98EX1x5 devices call with invalid ruleSize enum value. Call with ruleSize [0x5AAAAAA5], ruleIndex [0], policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0]. Expected: GT_BAD_PARAM.
1.12. For 98EX1x5 devices call with out of range ruleIndex. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [1024], policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0]. Expected: NON GT_OK.
1.13. For 98EX1x5 devices call with unsupported value for policerMode. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0], policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_METER_ONLY_E], policerId [0]. Expected: NON GT_OK.
1.14. For 98EX1x5 devices call with invalid enum value for policerMode. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0], policerMode [0x5AAAAAA5], policerId [0]. Expected: GT_BAD_PARAM.
1.15. For 98EX1x5 devices call with out of range policerId. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0], policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0xEFFFFFF5]. Expected: NON GT_OK.
1.16. For 98EX1x5 devices call with legal parameters. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0], policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0]. Expected: GT_OK.
1.17. For 98EX1x5 devices invalidate tested rule. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0]. Expected: GT_OK.
*/
    GT_STATUS                               st = GT_OK;
    GT_U8                                   dev;
    CPSS_EXMX_PCL_RULE_SIZE_ENT             ruleSize;
    GT_U32                                  ruleIndex;
    CPSS_EXMX_POLICY_ACTION_POLICER_MODE_ENT   policerMode;
    GT_U32                                  policerId;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        CPSS_PP_FAMILY_TYPE_ENT             devFamily;
        CPSS_EXMX_PCL_RULE_FORMAT_ENT       ruleFormat;
        CPSS_EXMX_PCL_RULE_FIELDS_UNT       mask;
        CPSS_EXMX_PCL_RULE_FIELDS_UNT       pattern;
        CPSS_EXMX_POLICY_ACTION_STC            action;

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For 98X1x6 devices set rule for testing.                                                */
        /* Call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E],                */
        /* ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …],                                 */
        /* patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …],                             */
        /* actionPtr [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE},                         */
        /* {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …},        */
        /* {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0},                              */
        /* {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}].                                  */
        /* Expected: GT_OK.                                                                             */
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
            ruleIndex = 512;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            mask.tgStd.pclId = 0xFF;
            mask.tgStd.pktType = 0xFF;
            mask.tgStd.ipHdrOk = 0xFF;

            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.tgStd.pclId = 0;
            pattern.tgStd.pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;
            pattern.tgStd.ipHdrOk = 1;

            pclActionDefaultSet(&action);

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleSet: %d, %d, %d", dev, ruleFormat, ruleIndex);

            /* 1.2. For 98X1x6 devices call with ruleSize which isn't applicable for standard rule.         */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [512],                         */
            /* policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0].                        */
            /* Expected: NON GT_OK.                                                                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;
            ruleIndex = 512;
            policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
            policerId = 0;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.3. For 98X1x6 devices call with invalid ruleSize enum value.                               */
            /* Call with ruleSize [0x5AAAAAA5], ruleIndex [512],                                            */
            /* policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0].                        */
            /* Expected: GT_BAD_PARAM.                                                                      */
            ruleSize = PCL_INVALID_ENUM_CNS;
            ruleIndex = 512;
            policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
            policerId = 0;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.4. For 98X1x6 devices call with out of range ruleIndex.                                    */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [2048],                        */
            /* policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0].                        */
            /* Expected: NON GT_OK.                                                                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 2048;
            policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
            policerId = 0;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.5. For 98X1x6 devices call with invalid enum value for policerMode.                        */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512],                         */
            /* policerMode [0x5AAAAAA5], policerId [0].                                                     */
            /* Expected: GT_BAD_PARAM.                                                                      */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 512;
            policerMode = PCL_INVALID_ENUM_CNS;
            policerId = 0;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.6. For 98X1x6 devices call with out of range policerId.                                    */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512],                         */
            /* policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0xEFFFFFF5].               */
            /* Expected: NON GT_OK.                                                                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 512;
            policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
            policerId = 0xEFFFFFF5;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.7. For 98X1x6 devices call with legal parameters.                                          */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512],                         */
            /* policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0].                        */
            /* Expected: GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 512;
            policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
            policerId = 0;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.8. For 98X1x6 devices invalidate tested rule.                                              */
            /* Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E],                */
            /* ruleIndex [512].                                                                             */
            /* Expected: GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 512;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
        }

        /* 1.9. For 98EX1x5 devices set rule for testing.                                               */
        /* Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0],                        */
        /* maskPtr->tdRule [0xFF, 0x00, …], patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …], */
        /* actionPtr is the same as in 1.1.                                                             */
        /* Expected: GT_OK.                                                                             */
        if (CPSS_PP_FAMILY_TWISTD_E == devFamily)
        {
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TD_E;
            ruleIndex = 0;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            pattern.tdRule.pktType = 0xFF;

            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.tdRule.pktType = CPSS_EXMX_PCL_PKT_IPV4_UDP_E;

            pclActionDefaultSet(&action);

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleSet: %d, %d, %d", dev, ruleFormat, ruleIndex);

            /* 1.10. For 98EX1x5 devices call with unsupported ruleSize.                                    */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [0],                           */
            /* policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0].                        */
            /* Expected: NON GT_OK.                                                                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;
            ruleIndex = 0;
            policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
            policerId = 0;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.11. For 98EX1x5 devices call with invalid ruleSize enum value.                             */
            /* Call with ruleSize [0x5AAAAAA5], ruleIndex [0],                                              */
            /* policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0].                        */
            /* Expected: GT_BAD_PARAM.                                                                      */
            ruleSize = PCL_INVALID_ENUM_CNS;
            ruleIndex = 0;
            policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
            policerId = 0;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.12. For 98EX1x5 devices call with out of range ruleIndex.                                  */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [1024],                        */
            /* policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0].                        */
            /* Expected: NON GT_OK.                                                                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 1024;
            policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
            policerId = 0;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.13. For 98EX1x5 devices call with unsupported value for policerMode.                       */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0],                           */
            /* policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_METER_ONLY_E], policerId [0].                      */
            /* Expected: NON GT_OK.                                                                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 0;
            policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_METER_ONLY_E;
            policerId = 0;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.14. For 98EX1x5 devices call with invalid enum value for policerMode.                      */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0],                           */
            /* policerMode [0x5AAAAAA5], policerId [0].                                                     */
            /* Expected: GT_BAD_PARAM.                                                                      */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 0;
            policerMode = PCL_INVALID_ENUM_CNS;
            policerId = 0;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.15. For 98EX1x5 devices call with out of range policerId.                                  */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0],                           */
            /* policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0xEFFFFFF5].               */
            /* Expected: NON GT_OK.                                                                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 0;
            policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
            policerId = 0xEFFFFFF5;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.16. For 98EX1x5 devices call with legal parameters.                                        */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0],                           */
            /* policerMode [CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E], policerId [0].                        */
            /* Expected: GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 0;
            policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
            policerId = 0;

            st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, ruleSize, ruleIndex, policerMode, policerId);

            /* 1.17. For 98EX1x5 devices invalidate tested rule.                                            */
            /* Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0]. */
            /* Expected: GT_OK.                                                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
            ruleIndex = 0;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
        }
    }

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
    ruleIndex =512;
    policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
    policerId = 0;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclRulePolicerActionUpdate(dev, ruleSize, ruleIndex, policerMode, policerId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_EXMX_PCL_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                             ruleIndex,
    OUT CPSS_EXMX_POLICY_ACTION_STC        *actionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclRuleActionGet)
{
/*
ITERATE_DEVICES
1.1. For 98X1x6 devices set rule for testing. Call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …], patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …], actionPtr [[CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE}, {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0}, {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}]. Expected: GT_OK.
1.2. For 98X1x6 GET action with legal parameters. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512], non-NULL actionPtr. Expected: GT_OK.
1.3. For 98X1x6 check with rule size for other type of rule. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [512], non-NULL actionPtr. Expected: NON GT_OK.
1.4. For 98X1x6 check invalid rule size value. Call with ruleSize [0x5AAAAA5], ruleIndex [512], non-NULL actionPtr. Expected: GT_BAD_PARAM.
1.5. For 98X1x6 check out of range ruleIndex. Call with ruleIndex [2048], non-NULL actionPtr, other parameters is the same as in 1.1. Expected: NON GT_OK.
1.6. For 98X1x6 check for NULL pointer. Call with actionPtr [NULL]. Expected: GT_BAD_PTR.
1.7. For 98X1x6 devices call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512] to cleanup after testing. Expected: GT_OK.
1.8. For 98EX1x5 devices set rule for testing. Call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0], maskPtr->tdRule [0xFF, 0x00, …], patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …], actionPtr is the same as in 1.1. Expected: GT_OK.
1.9. For 98EX1x5 GET action with legal parameters. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0], non-NULL actionPtr. Expected: GT_OK.
1.10. For 98EX1x5 check with unsupported rule size. Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [0], non-NULL actionPtr. Expected: NON GT_OK.
1.11. For 98EX1x5 check invalid rule size value. Call with ruleSize [0x5AAAAA5], ruleIndex [0], non-NULL actionPtr. Expected: GT_BAD_PARAM.
1.12. For 98EX1x5 check out of range ruleIndex. Call with ruleIndex [1024], non-NULL actionPtr, other parameters is the same as in 1.1. Expected: NON GT_OK.
1.13. For 98EX1x5 check for NULL pointer. Call with actionPtr [NULL]. Expected: GT_BAD_PTR.
1.14. For 98EX1x5 devices call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    CPSS_EXMX_PCL_RULE_SIZE_ENT         ruleSize;
    GT_U32                              ruleIndex;
    CPSS_EXMX_POLICY_ACTION_STC            action;

    CPSS_EXMX_PCL_RULE_FORMAT_ENT       ruleFormat;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT       mask;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT       pattern;
    CPSS_PP_FAMILY_TYPE_ENT             devFamily;

    /* prepare <action> (it shouldn't change during the test) */
    pclActionDefaultSet(&action);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            /* 1.1. For 98X1x6 devices set rule for testing. Call               */
            /* cpssExMxPclRuleSet with ruleFormat                               */
            /* [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512],           */
            /* maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …],                      */
            /* patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …], */
            /* actionPtr [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E,                   */
            /* {GT_FALSE, GT_FALSE}, {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …},     */
            /* {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …},                   */
            /* {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0},  */
            /* {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}].      */
            /* Expected: GT_OK.                                                 */
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
            ruleIndex = 512;

            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            mask.tgStd.pclId = 0xFF;
            mask.tgStd.pktType = 0xFF;
            mask.tgStd.ipHdrOk = 0xFF;

            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.tgStd.pclId = 0;
            pattern.tgStd.pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;
            pattern.tgStd.ipHdrOk = 1;

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "cpssExMxPclRuleSet: Tiger device: %d, %d, %d",
                                        dev, ruleFormat, ruleIndex);

            /* 1.2. For 98X1x6 GET action with legal parameters. Call with      */
            /* ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512],       */
            /* non-NULL actionPtr. Expected: GT_OK.                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "Tiger device: %d, %d, %d",
                                        dev, ruleSize, ruleIndex);

            /* 1.3. For 98X1x6 check with rule size for other type of rule.     */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E],              */
            /* ruleIndex [512], non-NULL actionPtr.                             */
            /* Expected: NON GT_OK.                                             */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E; /* restore valid values */

            /* 1.4. For 98X1x6 check invalid rule size value. Call with         */
            /* ruleSize [0x5AAAAA5], ruleIndex [512], non-NULL actionPtr.       */
            /* Expected: GT_BAD_PARAM.                                          */
            ruleSize = PCL_INVALID_ENUM_CNS;

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleIndex);

            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E; /* restore valid values */

            /* 1.5. For 98X1x6 check out of range ruleIndex. Call with          */
            /* ruleIndex [2048], non-NULL actionPtr, other parameters is the    */
            /* same as in 1.1. Expected: NON GT_OK.                             */
            ruleIndex = 2048;

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, 
                                             "Tiger device: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

            ruleIndex = 512; /* restore valid value */

            /* 1.6. For 98X1x6 check for NULL pointer. Call with                */
            /* actionPtr [NULL]. Expected: GT_BAD_PTR.                          */

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL",
                                         dev, ruleSize, ruleIndex);

            /* 1.7. For 98X1x6 devices call cpssExMxPclRuleInvalidate with      */
            /* ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512]        */
            /* to cleanup after testing. Expected: GT_OK.                       */

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                         "cpssExMxPclRuleInvalidate: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
        }

        if (CPSS_PP_FAMILY_TWISTD_E == devFamily)
        {
            /* 1.8. For 98EX1x5 devices set rule for testing.                   */
            /* Call cpssExMxPclRuleSet with                                     */
            /* ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0],      */
            /* maskPtr->tdRule [0xFF, 0x00, …],                                 */
            /* patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …],      */
            /* actionPtr is the same as in 1.1. Expected: GT_OK.                */
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TD_E;
            ruleIndex = 0;
            cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
            mask.tdRule.pktType = 0xFF;
            cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
            pattern.tdRule.pktType = CPSS_EXMX_PCL_PKT_IPV4_UDP_E;

            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "cpssExMxPclRuleSet: Twist-D device: %d, %d, %d",
                                        dev, ruleFormat, ruleIndex);

            /* 1.9. For 98EX1x5 GET action with legal parameters.               */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E],              */
            /* ruleIndex [0], non-NULL actionPtr.                               */
            /* Expected: GT_OK.                                                 */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "Twist-D device: %d, %d, %d",
                                        dev, ruleSize, ruleIndex);

            /* 1.10. For 98EX1x5 check with unsupported rule size.              */
            /* Call with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [0]*/
            /* non-NULL actionPtr. Expected: NON GT_OK.                         */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "Twist-D device: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E; /* restore valid value */

            /* 1.11. For 98EX1x5 check invalid rule size value. Call with       */
            /* ruleSize [0x5AAAAA5], ruleIndex [0], non-NULL actionPtr.         */
            /* Expected: GT_BAD_PARAM.                                          */
            ruleSize = PCL_INVALID_ENUM_CNS;

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleSize, ruleIndex);

            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E; /* restore valid value */

            /* 1.12. For 98EX1x5 check out of range ruleIndex.                  */
            /* Call with ruleIndex [1024], non-NULL actionPtr, other parameters */
            /* is the same as in 1.1. Expected: NON GT_OK.                      */
            ruleIndex = 1024;

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &action);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "Twist-D device: %d, %d, %d",
                                             dev, ruleSize, ruleIndex);

            ruleIndex = 0; /* restore valid value */

            /* 1.13. For 98EX1x5 check for NULL pointer. Call with              */
            /* actionPtr [NULL]. Expected: GT_BAD_PTR.                          */

            st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL",
                                         dev, ruleSize, ruleIndex);

            /* 1.14. For 98EX1x5 devices call cpssExMxPclRuleInvalidate         */
            /* with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0]     */
            /* to cleanup after testing. Expected: GT_OK.                       */

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                         "cpssExMxPclRuleInvalidate: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
        }
    }

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
    ruleIndex = 512;

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclRuleActionGet(dev, ruleSize, ruleIndex, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclRuleGet
(
    IN  GT_U8                              devNum,
    IN  CPSS_EXMX_PCL_RULE_FORMAT_ENT      ruleFormat,
    IN  GT_U32                             ruleIndex,
    OUT CPSS_EXMX_PCL_RULE_FIELDS_UNT      *maskPtr,
    OUT CPSS_EXMX_PCL_RULE_FIELDS_UNT      *patternPtr,
    OUT CPSS_EXMX_POLICY_ACTION_STC        *pclActionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclRuleGet)
{
/*
ITERATE_DEVICES
1.1. Set standard rule for testing. For 98EX1x6 devices call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …], patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …], actionPtr [[CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE}, {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0}, {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}]. Expected: GT_OK.
1.2. For 98X1x6 call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512], non-NULL maskPtr, patternPtr, actionPtr. Expected: GT_OK.
1.3. For 98X1x6 devices check for out of range ruleIndex for standard rule format. Call with ruleIndex [2048], other parameters is the same as in 1.2. Expected: NON GT_OK.
1.4. For 98X1x6 devices check for NULL pointers. Call with maskPtr [NULL], other parameters is the same as in 1.2. Expected: GT_BAD_PTR.
1.5. For 98X1x6 devices check for NULL pointers. Call with patternPtr [NULL], other parameters is the same as in 1.2. Expected: GT_BAD_PTR.
1.6. For 98X1x6 devices check for NULL pointers. Call with actionPtr [NULL], other parameters is the same as in 1.2. Expected: GT_BAD_PTR.
1.7. For 98X1x6 devices Check for invalid ruleFormat. Call with ruleFormat [0x5AAAAAA5], other parameters is the same as in 1.2. Expected: GT_BAD_PARAM.
1.8. For 98X1x6 devices call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512] to cleanup after testing. Expected: GT_OK.
1.9. Set extended rule for testing. For 98EX1x6 call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E], ruleIndex [50], maskPtr-> tgExtMac.stdRule is the same as in 1.1., other fileds are zeroed, patternPtr->tgExtMac.stdRule is the same as in 1.1., other fields are zeroed, actionPtr is the same as in 1.1. Expected: GT_OK.
1.10. For 98X1x6 call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E], ruleIndex [50], non-NULL maskPtr, patternPtr, actionPtr. Expected: GT_OK.
1.11. For 98X1x6 devices check for out of range ruleIndex for extended rule format. Call with ruleIndex [1024], other parameters is the same as 1.10. Expected: NON GT_OK.
1.12. For 98X1x6 get standard rule with ruleIndex which is not allowed because of extended rule consumes two spaces of standard rule. Call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [50+512], other parameters is the same as in 1.2. Expected: NON GT_OK.
1.13. For 98X1x6 devices call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [50] to cleanup after testing. Expected: GT_OK.
1.14. Set rule for testing. For 98EX1x5 devices call cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0], maskPtr->tdRule [0xFF, 0x00, …], patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …], actionPtr is the same as in 1.1. Expected: GT_OK.
1.15. For 98X1x5 call with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0], non-NULL maskPtr, patternPtr, actionPtr. Expected: GT_OK.
1.16. For 98EX1x5 devices check for out of range ruleIndex. Call with ruleIndex [1024], other parameters is the same as in 1.15. Expected: NON GT_OK.
1.17. For 98EX1x5 devices call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0] to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    CPSS_EXMX_PCL_RULE_FORMAT_ENT       ruleFormat;
    GT_U32                              ruleIndex;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT       mask;
    CPSS_EXMX_PCL_RULE_FIELDS_UNT       pattern;
    CPSS_EXMX_POLICY_ACTION_STC         action;

    CPSS_PP_FAMILY_TYPE_ENT             devFamily;
    CPSS_EXMX_PCL_RULE_SIZE_ENT         ruleSize;

    pclActionDefaultSet(&action);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        /* 1.1. Set standard rule for testing. For 98EX1x6 devices call     */
        /* cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E],*/
        /* ruleIndex [512], maskPtr->tgStd [0xFF, 0xFF, 0xFF, 0x00, …],      */
        /* patternPtr->tgStd [0, CPSS_EXMX_PCL_PKT_IPV4_TCP_E, 1, 0x00, …],  */
        /* actionPtr [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E,{GT_FALSE, GT_FALSE}*/
        /* {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …},                            */
        /* {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …},                    */
        /* {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0},                     */
        /* {GT_FALSE, 0, 0},                                                 */
        /* {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}].       */
        /* Expected: GT_OK.              */
        ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
        ruleIndex = 512;

        cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
        mask.tgStd.pclId = 0xFF;
        mask.tgStd.pktType = 0xFF;
        mask.tgStd.ipHdrOk = 0xFF;

        cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
        pattern.tgStd.pclId = 0;
        pattern.tgStd.pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;
        pattern.tgStd.ipHdrOk = 1;

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "Tiger device: %d, %d, %d",
                                        dev, ruleFormat, ruleIndex);
        }

        /* Tests for Tiger devices */

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            /* 1.2. For 98X1x6 call with ruleFormat                     */
            /* [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [512],   */
            /* non-NULL maskPtr, patternPtr, actionPtr. Expected: GT_OK.*/

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleGet: %d, %d, %d",
                                         dev, ruleFormat, ruleIndex);

            /* 1.3. For 98X1x6 devices check for out of range ruleIndex for      */
            /* standard rule format. Call with ruleIndex [2048], other parameters*/
            /* is the same as in 1.2. Expected: NON GT_OK.                       */
            ruleIndex = 2048;

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

            ruleIndex = 512; /* restore valid value */

            /* 1.4. For 98X1x6 devices check for NULL pointers.                  */
            /* Call with maskPtr [NULL], other parameters is the same as in 1.2. */
            /* Expected: GT_BAD_PTR.                                             */

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, NULL, &pattern,
                                    &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, %d, maskPtr = NULL",
                                         dev, ruleFormat, ruleIndex);

            /* 1.5. For 98X1x6 devices check for NULL pointers.                  */
            /* Call with patternPtr [NULL], other parameters is the same         */
            /* as in 1.2. Expected: GT_BAD_PTR.                                  */
            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, NULL,
                                    &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, %d, patternPtr = NULL",
                                         dev, ruleFormat, ruleIndex);

            /* 1.6. For 98X1x6 devices check for NULL pointers.                  */
            /* Call with actionPtr [NULL], other parameters is the same as in 1.2*/
            /* Expected: GT_BAD_PTR.                                             */
            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, %d, actionPtr = NULL",
                                         dev, ruleFormat, ruleIndex);

            /* 1.7. For 98X1x6 devices Check for invalid ruleFormat.             */
            /* Call with ruleFormat [0x5AAAAAA5], other parameters is the same   */
            /* as in 1.2.Expected: GT_BAD_PARAM.                                 */
            ruleFormat = PCL_INVALID_ENUM_CNS;

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, ruleFormat);

            /* restore valid value */
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E; 

            /* 1.8. For 98X1x6 devices call cpssExMxPclRuleInvalidate with       */
            /* ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [512]         */
            /* to cleanup after testing. Expected: GT_OK.                        */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                         "cpssExMxPclRuleInvalidate: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
        }

        /* 1.9. Set extended rule for testing. For 98EX1x6 call cpssExMxPclRuleSet*/
        /* with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E],         */
        /* ruleIndex [50], maskPtr-> tgExtMac.stdRule is the same as in 1.1. */
        /* other fileds are zeroed, patternPtr->tgExtMac.stdRule is the same */
        /* as in 1.1., other fields are zeroed, actionPtr is the same        */
        /* as in 1.1. Expected: GT_OK.   */
        ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E;
        ruleIndex = 50;

        cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
        mask.tgExtMac.stdRule.pclId = 0xFF;
        mask.tgExtMac.stdRule.pktType = 0xFF;
        mask.tgExtMac.stdRule.ipHdrOk = 0xFF;

        cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
        pattern.tgExtMac.stdRule.pclId = 0;
        pattern.tgExtMac.stdRule.pktType = CPSS_EXMX_PCL_PKT_IPV4_TCP_E;
        pattern.tgExtMac.stdRule.ipHdrOk = 1;

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "Tiger device: %d, %d, %d",
                                        dev, ruleFormat, ruleIndex);
        }

        /* Tests for Tiger devices */

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            /* 1.10. For 98X1x6 call with ruleFormat                    */
            /* [CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E], ruleIndex [50],*/
            /* non-NULL maskPtr, patternPtr, actionPtr. Expected: GT_OK */

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleGet: %d, %d, %d",
                                         dev, ruleFormat, ruleIndex);


            /* 1.11. For 98X1x6 devices check for out of range ruleIndex         */
            /* for extended rule format. Call with ruleIndex [1024],             */
            /* other parameters is the same as 1.10. Expected: NON GT_OK.        */
            ruleIndex = 1024;

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

            ruleIndex = 50; /* restore valid value */

            /* 1.12. For 98X1x6 get standard rule with ruleIndex                    */
            /* which is not allowed because of extended rule consumes               */
            /* two spaces of standard rule. Call with                               */
            /* ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E], ruleIndex [50+512], */
            /* other parameters is the same as in 1.2. Expected: NON GT_OK.         */
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
            ruleIndex = 50 + 512;

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

            /* restore valid values */
            ruleIndex = 50;
            ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_EXT_MAC_E;

            /* 1.13. For 98X1x6 devices call cpssExMxPclRuleInvalidate with      */
            /* ruleSize [CPSS_EXMX_PCL_RULE_SIZE_EXT_E], ruleIndex [50]          */
            /* to cleanup after testing. Expected: GT_OK.                        */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_EXT_E;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                         "cpssExMxPclRuleInvalidate: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
        }

        /* 1.14. Set rule for testing. For 98EX1x5 devices call                 */
        /* cpssExMxPclRuleSet with ruleFormat [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], */
        /* ruleIndex [0], maskPtr->tdRule [0xFF, 0x00, …],                   */
        /* patternPtr->tdRule [CPSS_EXMX_PCL_PKT_IPV4_UDP_E, 0x00, …],       */
        /* actionPtr is the same as in 1.1. Expected: GT_OK                  */
        ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TD_E;
        ruleIndex = 0;

        cpssOsBzero((GT_VOID*)&mask, sizeof(mask));
        mask.tdRule.pktType = 0xFF;

        cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern));
        pattern.tdRule.pktType = CPSS_EXMX_PCL_PKT_IPV4_UDP_E;

        if (CPSS_PP_FAMILY_TWISTD_E == devFamily)
        {
            st = cpssExMxPclRuleSet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                        "Twist-D device: %d, %d, %d",
                                        dev, ruleFormat, ruleIndex);
        }

        /* Tests for Twist-D devices */

        if (CPSS_PP_FAMILY_TWISTD_E == devFamily)
        {
            /* 1.15. For 98X1x5 call with ruleFormat                        */
            /* [CPSS_EXMX_PCL_RULE_FORMAT_TD_E], ruleIndex [0], non-NULL    */
            /* maskPtr, patternPtr, actionPtr. Expected: GT_OK              */

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                &action);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleGet: %d, %d, %d",
                                         dev, ruleFormat, ruleIndex);


            /* 1.16. For 98EX1x5 devices check for out of range ruleIndex.       */
            /* Call with ruleIndex [1024], other parameters is the same          */
            /* as in 1.15. Expected: NON GT_OK.                                  */
            ruleIndex = 1024;

            st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                    &action);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleFormat, ruleIndex);

            ruleIndex = 0; /* restore valid value */

            /* 1.17. For 98EX1x5 devices call cpssExMxPclRuleInvalidate with     */
            /* ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E], ruleIndex [0]           */
            /* to cleanup after testing. Expected: GT_OK.                        */
            ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

            st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                         "cpssExMxPclRuleInvalidate: %d, %d, %d",
                                         dev, ruleSize, ruleIndex);
        }
    }

    /* Set valid parameters. As soon as family of device can't be determined */
    /* - values valid for any family                                         */
    ruleFormat = CPSS_EXMX_PCL_RULE_FORMAT_TG_STD_E;
    ruleIndex = 512;
    /* cpssOsBzero((GT_VOID*)&mask, sizeof(mask)); */
    /* cpssOsBzero((GT_VOID*)&pattern, sizeof(pattern)); */

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                                &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclRuleGet(dev, ruleFormat, ruleIndex, &mask, &pattern,
                            &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclTemplateDefaultActionGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           profileId,
    IN  CPSS_EXMX_PCL_TEMPLATE_ENT      keyTemplate,
    OUT CPSS_EXMX_POLICY_ACTION_STC     *pclActionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclTemplateDefaultActionGet)
{
/*
ITERATE_DEVICES
1.1. For 98EX1x6 devices call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E] to set 8 templates per profile mode. Expected: GT_OK.
1.2. Call cpssExMxPclTemplateDefaultActionSet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], non NULL actionPtr [[CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE}, {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …}, {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0}, {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}]. Expected: GT_OK.
1.3. Call cpssExMxPclTemplateDefaultActionGet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], non NULL actionPtr. Expected: GT_OK.

1.4. Check for NULL handling. Call with actionPtr [NULL]. Expected: GT_BAD_PTR.
1.5. Check for out of range for profileId. Call with profileId [2], other parameters is the same as in 1.2. Expected: NON GT_OK.
1.6. Check for invalid keyTemplate for 8-2 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], other parameters is the same as in 1.2. Expected: NON GT_OK.
1.7. For 98EX1x6 devices set profile size 4-4, four templates per four profiles per one device. Call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E] to set 4 templates per profile mode. Expected: GT_OK.
1.8. For 98EX1x6 devices call cpssExMxPclTemplateDefaultActionSet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], and the same actionPtr as in 1.2. Expected: GT_OK.
1.9. Call cpssExMxPclTemplateDefaultActionGet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], non NULL actionPtr. Expected: GT_OK.

1.10. For 98EX1x6 devices check for out of range for profileId. Call with profileId [5], other parameters is the same as in 1.8. Expected: NON GT_OK.
1.11. For 98EX1x6 devices check for invalid keyTemplate for 4-4 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], other parameters is the same as in 1.8. Expected: NON GT_OK.
1.12. For 98EX1x6 devices set profile size 1-16, one template per sixsteen profiles per one device. Call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E] to set one template per profile mode. Expected: GT_OK.
1.13. For 98EX1x6 devices call cpssExMxPclTemplateDefaultActionSet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_ANY_E], and the same actionPtr as in 1.2. Expected: GT_OK.
1.14. Call cpssExMxPclTemplateDefaultActionGet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_ANY_E], non NULL actionPtr. Expected: GT_OK.

1.15. For 98EX1x6 devices check for out of range for profileId. Call with profileId [16], other parameters is the same as in 1.13. Expected: NON GT_OK.
1.16. For 98EX1x6 devices check for invalid keyTemplate for 1-16 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], other parameters is the same as in 1.13. Expected: NON GT_OK.
1.17. Call cpssExMxPclTcamFlush to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                           dev;
    GT_U8                           profileId;
    CPSS_EXMX_PCL_TEMPLATE_ENT      keyTemplate;
    CPSS_EXMX_POLICY_ACTION_STC        action;

    CPSS_EXMX_TG_PCL_PROFILE_SIZE_ENT       profileSize;
    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);
        /* 1.1. For 98EX1x6 devices call function                           */
        /* cpssExMxTgPclProfileSizeSet with                                 */
        /* profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E] to set */
        /* 8 templates per profile mode. Expected: GT_OK.                   */
        profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E;

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                         "cpssExMxTgPclProfileSizeSet: %d, %d",
                                         dev, profileSize);
        }

        /* 1.2. Call cpssExMxPclTemplateDefaultActionSet with profileId [0], keyTemplate */
        /* [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], non NULL actionPtr          */
        /* [CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E, {GT_FALSE, GT_FALSE},       */
        /* {CPSS_EXMX_POLICY_ACTION_MARK_NONE_E, …},                           */
        /* {CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E, …},                   */
        /* {CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E, 0}, {GT_FALSE, 0, 0},  */
        /* {CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E, GT_FALSE, 0}].      */
        /* Expected: GT_OK.                                                 */
        profileId = 0;
        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;
        pclActionDefaultSet(&action);

        st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, profileId, keyTemplate);


        /* 1.3. Call cpssExMxPclTemplateDefaultActionGet with profileId [0], */
        /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], non NULL         */
        /* actionPtr. Expected: GT_OK                                        */

        st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &action);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateDefaultActionGet: %d, %d, %d",
                                     dev, profileId, keyTemplate);

        /* 1.4. Check for NULL handling. Call with actionPtr [NULL].        */
        /* Expected: GT_BAD_PTR.                                            */

        st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, %d, NULL",
                                     dev, profileId, keyTemplate);

        /* 1.5. Check for out of range for profileId. Call with             */
        /* profileId [2], other parameters is the same as in 1.2.           */
        /* Expected: NON GT_OK.                                             */
        profileId = 2;

        st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &action);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileId);

        profileId = 0; /* restore valid value */

        /* 1.6. Check for invalid keyTemplate for 8-2 mode. Call with       */
        /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], other parameters is */ 
        /* the same as in 1.2. Expected: NON GT_OK.                         */
        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E;

        st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &action);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                        "%d, %d, %d, profileSize = %d",
                                        dev, profileId, keyTemplate, profileSize);

        /* restore valid value */
        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;

        /* Tests for Tiger devices */

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            /* 1.7. For 98EX1x6 devices set profile size 4-4, four templates    */
            /* per four profiles per one device. Call function                  */
            /* cpssExMxTgPclProfileSizeSet with                                 */
            /* profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E] to set */
            /* 4 templates per profile mode. Expected: GT_OK.                   */
            profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E;

            st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                         "cpssExMxTgPclProfileSizeSet: %d, %d",
                                         dev, profileSize);

            /* 1.8. For 98EX1x6 devices call cpssExMxPclTemplateDefaultActionSet*/
            /* with profileId [0],                                              */
            /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], and the same        */
            /* actionPtr as in 1.2. Expected: GT_OK for 98EX1x6 devices and     */
            /* NON GT_OK for 98EX1x5, 98MX6x5 and 98MX6x8 devices.              */
            profileId = 0;
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E;

            st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                        "Tiger device: %d, %d, %d, profileSize = %d",
                                        dev, profileId, keyTemplate, profileSize);

            /* 1.9. Call cpssExMxPclTemplateDefaultActionGet with profileId [0],*/
            /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], non NULL actionPtr. */
            /* Expected: GT_OK                                                  */

            st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &action);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateDefaultActionGet: %d, %d, %d",
                                         dev, profileId, keyTemplate);


            /* 1.10. For 98EX1x6 devices check for out of range for profileId.  */
            /* Call with profileId [5], other parameters is the same as in 1.8. */
            /* Expected: NON GT_OK.                                             */
            profileId = 5;

            st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileId);

            profileId = 0; /* restore valid value */

            /* 1.11. For 98EX1x6 devices check for invalid keyTemplate for 4-4  */
            /* mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], */
            /* other parameters is the same as in 1.8. Expected: NON GT_OK.     */
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;

            st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, %d, %d, profileSize = %d",
                                             dev, profileId, keyTemplate, profileSize);

            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E; /* restore valid value */

            /* 1.12. For 98EX1x6 devices set profile size 1-16, one template    */
            /* per sixsteen profiles per one device. Call function              */
            /* cpssExMxTgPclProfileSizeSet with                                 */
            /* profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E]        */
            /* to set one template per profile mode. Expected: GT_OK.           */
            profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E;

            st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                         "cpssExMxTgPclProfileSizeSet: %d, %d",
                                         dev, profileSize);

            /* 1.13.For 98EX1x6 devices call cpssExMxPclTemplateDefaultActionSet*/
            /* with profileId [0],                                              */
            /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_ANY_E], and the same         */
            /* actionPtr as in 1.2. Expected: GT_OK for 98EX1x6 devices         */
            /* and NON GT_OK for 98EX1x5, 98MX6x5 and 98MX6x8 devices.          */
            profileId = 0;
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_ANY_E;

            st = cpssExMxPclTemplateDefaultActionSet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                        "Tiger device: %d, %d, %d, profileSize = %d",
                                        dev, profileId, keyTemplate, profileSize);

            /* 1.14. Call cpssExMxPclTemplateDefaultActionGet with profileId [0],*/
            /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_ANY_E], non NULL actionPtr.  */
            /* Expected: GT_OK.                                                 */

            st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &action);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateDefaultActionGet: %d, %d, %d",
                                         dev, profileId, keyTemplate);


            /* 1.15. For 98EX1x6 devices check for out of range for profileId.  */
            /* Call with profileId [16], other parameters is the same           */
            /* as in 1.13. Expected: NON GT_OK.                                 */
            profileId = 16;

            st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileId);

            profileId = 0; /* restore valid value */

            /* 1.16. For 98EX1x6 devices check for invalid keyTemplate for 1-16 */
            /* mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], */
            /* other parameters is the same as in 1.13. Expected: NON GT_OK.    */
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;

            st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &action);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, %d, %d, profileSize = %d",
                                             dev, profileId, keyTemplate, profileSize);

            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_ANY_E; /* restore valid value */
        }

        /* 1.17. Call cpssExMxPclTcamFlush to cleanup after testing.    */
        /* Expected: GT_OK.                                             */
        cpssExMxPclTcamFlush(dev);

    }

    /* Set valid values for all the parameters (except device id) */
    profileId = 0;
    keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;
    pclActionDefaultSet(&action);

    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &action);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclTemplateDefaultActionGet(dev, profileId, keyTemplate, &action);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPclTemplateGet
(
    IN  GT_U8                                devNum,
    IN  GT_U8                                profileId,
    IN  CPSS_EXMX_PCL_TEMPLATE_ENT           keyTemplate,
    IN  GT_U32                               udbAmount,
    OUT CPSS_EXMX_PCL_UDB_CFG_STC            udbCfg[],
    OUT CPSS_EXMX_PCL_INCLUDE_VLAN_INLIF_ENT *includeVlanOrInLifNumPtr,
    OUT GT_BOOL                              *includeProfileIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPclTemplateGet)
{
/*
ITERATE_DEVICES
1.1. For 98EX1x6 devices call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E] to set 8 templates per profile mode. Expected: GT_OK.
1.2. Call cpssExMxPclTemplateSet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], udbAmount [2], udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0], [CPSS_EXMX_PCL_OFFSET_L4_E, 0]], includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_NONE_E], includeProfileId [GT_FALSE]. Expected: GT_OK.
1.3. Call cpssExMxPclTemplateGet with non-NULL udbCfg, includeVlanOrInLifNumPtr, includeProfileIdPtr, other params same as in 1.2. Expected: GT_OK.
1.4. Check for out of range for profileId. Call with profileId [2], other parameters is the same as in 1.3. Expected: NON GT_OK.
1.5. Check for invalid keyTemplate for 8-2 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], other parameters is the same as in 1.3. Expected: NON GT_OK.
1.6. For 98EX1x6 devices set profile size 4-4, four templates per four profiles per one device. Call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E] to set 4 templates per profile mode. Expected: GT_OK.
1.7. For 98EX1x6 devices call cpssExMxPclTemplateSet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], udbAmount [2], udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0], [CPSS_EXMX_PCL_OFFSET_L4_E, 0]], includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_NONE_E], includeProfileId [GT_FALSE]. Expected: GT_OK.
1.8. For 98EX1x6 devices call cpssExMxPclTemplateGet with non-NULL udbCfg, includeVlanOrInLifNumPtr, includeProfileIdPtr, other params same as in 1.7. Expected: GT_OK and same values as written.
1.9. For 98EX1x6 devices check for out of range for profileId. Call with profileId [5], other parameters is the same as in 1.8. Expected: NON GT_OK.
1.10. For 98EX1x6 devices check for invalid keyTemplate for 4-4 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], other parameters is the same as in 1.8. Expected: NON GT_OK.
1.11. For 98EX1x6 devices set profile size 1-16, one template per sixsteen profiles per one device. Call function cpssExMxTgPclProfileSizeSet with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E] to set one template per profile mode. Expected: GT_OK.
1.12. For 98EX1x6 devices call cpssExMxPclTemplateSet with profileId [0], keyTemplate [CPSS_EXMX_PCL_TEMPLATE_ANY_E], udbAmount [2], udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0], [CPSS_EXMX_PCL_OFFSET_L4_E, 0]], includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_NONE_E], includeProfileId [GT_FALSE]. Expected: GT_OK.
1.13. For 98EX1x6 devices call cpssExMxPclTemplateGet with non-NULL udbCfg, includeVlanOrInLifNumPtr, includeProfileIdPtr, other params same as in 1.12. Expected: GT_OK and same values as written.
1.14. For 98EX1x6 devices check for out of range for profileId. Call with profileId [16], other parameters is the same as in 1.13. Expected: NON GT_OK.
1.15. For 98EX1x6 devices check for invalid keyTemplate for 1-16 mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], other parameters is the same as in 1.13. Expected: NON GT_OK.
1.16. Call cpssExMxPclTcamFlush to cleanup after testing. Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                   dev;
    GT_U8                                   profileId;
    CPSS_EXMX_PCL_TEMPLATE_ENT              keyTemplate;
    GT_U32                                  udbAmount;
    CPSS_EXMX_PCL_UDB_CFG_STC               udbCfg[2];
    CPSS_EXMX_PCL_INCLUDE_VLAN_INLIF_ENT    includeVlanOrInLifNum;
    GT_BOOL                                 includeProfileId;

    CPSS_EXMX_TG_PCL_PROFILE_SIZE_ENT       profileSize;
    CPSS_PP_FAMILY_TYPE_ENT                 devFamily;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        /* 1.1.For 98EX1x6 devices call function cpssExMxTgPclProfileSizeSet*/
        /* with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E]   */
        /* to set 8 templates per profile mode. Expected: GT_OK.            */
        profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_8_TEMPLATES_E;

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                         "cpssExMxTgPclProfileSizeSet: %d, %d",
                                         dev, profileSize);
        }

        /* 1.2. Call cpssExMxPclTemplateSet with profileId [0], keyTemplate                        */
        /* [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], udbAmount [2],              */
        /* udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0],                          */
        /* [CPSS_EXMX_PCL_OFFSET_L4_E, 0]], includeVlanOrInLifNum           */
        /* [CPSS_EXMX_PCL_INCLUDE_NONE_E], includeProfileId [GT_FALSE].     */
        /* Expected: GT_OK.                                                 */
        profileId = 0;
        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;
        udbAmount = 2;
        udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_L2_E;
        udbCfg[0].offset = 0;
        udbCfg[1].offsetType = CPSS_EXMX_PCL_OFFSET_L4_E;
        udbCfg[1].offset = 0;
        includeVlanOrInLifNum = CPSS_EXMX_PCL_INCLUDE_NONE_E;
        includeProfileId = GT_FALSE;
        
        st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, includeVlanOrInLifNum, includeProfileId);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "%d, %d, %d, <udbAmount>, <udbCfg>, %d, 0; profileSize = %d",
                                     dev, profileId, keyTemplate,
                                     includeVlanOrInLifNum, profileSize);

        /* 1.3. Call cpssExMxPclTemplateGet with non-NULL udbCfg,       */
        /* includeVlanOrInLifNumPtr, includeProfileIdPtr, other params  */
        /* same as in 1.2. Expected: GT_OK.                             */

        st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, &includeVlanOrInLifNum, &includeProfileId);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateGet: %d, %d, %d, %d",
                                     dev, profileId, keyTemplate, udbAmount);

        /* 1.4. Check for out of range for profileId. Call with     */
        /* profileId [2], other parameters is the same as in 1.3.   */
        /* Expected: NON GT_OK.                                     */
        profileId = 2;

        st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, &includeVlanOrInLifNum, &includeProfileId);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileId);

        profileId = 0; /* restore correct value */

        /* 1.5. Check for invalid keyTemplate for 8-2 mode. Call with       */
        /* keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], other parameters    */
        /* is the same as in 1.3. Expected: NON GT_OK.                      */
        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E;

        st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, &includeVlanOrInLifNum, &includeProfileId);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                        "%d, %d, %d, profileSize = %d",
                                        dev, profileId, keyTemplate, profileSize);

        keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E; /* restore correct value */

        /* Tests for Tiger devices */

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {

            /* 1.6. Set profile size 4-4, four templates per four profiles     */
            /* per one device. For 98EX1x6 devices call function                */
            /* cpssExMxTgPclProfileSizeSet                                      */
            /* with profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E]   */
            /* to set 4 templates per profile mode. Expected: GT_OK.            */
            profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_4_TEMPLATES_E;

            st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                         "cpssExMxTgPclProfileSizeSet: %d, %d",
                                         dev, profileSize);

            /* 1.7. For 98EX1x6 devices call cpssExMxPclTemplateSet with        */
            /* profileId [0], keyTemplate                                       */
            /* [CPSS_EXMX_PCL_TEMPLATE_IPV4_E], udbAmount [2],                  */
            /* udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0],                          */
            /* [CPSS_EXMX_PCL_OFFSET_L4_E, 0]],                                 */
            /* includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_NONE_E],            */
            /* includeProfileId [GT_FALSE]. Expected: GT_OK for 98EX1x6 devices */
            /* and NON GT_OK for 98EX1x5, 98MX6x5 and 98MX6x8 devices.          */
            profileId = 0;
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E;
            udbAmount = 2;
            udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_L2_E;
            udbCfg[0].offset = 0;
            udbCfg[1].offsetType = CPSS_EXMX_PCL_OFFSET_L4_E;
            udbCfg[1].offset = 0;
            includeVlanOrInLifNum = CPSS_EXMX_PCL_INCLUDE_NONE_E;
            includeProfileId = GT_FALSE;

            st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, includeVlanOrInLifNum, includeProfileId);
            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                                         "Tiger device: %d, %d, %d, <udbAmount>, <udbCfg>, %d, 0; profileSize = %d",
                                         dev, profileId, keyTemplate,
                                         includeVlanOrInLifNum, profileSize);

            /* 1.8. For 98EX1x6 devices call cpssExMxPclTemplateGet with    */
            /* non-NULL udbCfg, includeVlanOrInLifNumPtr,                   */
            /* includeProfileIdPtr, other params same as in 1.7.           */
            /* Expected: GT_OK.                                             */

            st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, &includeVlanOrInLifNum, &includeProfileId);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateGet: %d, %d, %d, %d",
                                         dev, profileId, keyTemplate, udbAmount);

            /* 1.9. For 98EX1x6 devices check for out of range for profileId.  */
            /* Call with profileId [5], other parameters is the same as in 1.8.*/
            /* Expected: NON GT_OK.                                             */
            profileId = 5;

            st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, &includeVlanOrInLifNum, &includeProfileId);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, profileId = %d",
                                             dev, profileId);

           profileId = 0; /* restore correct value */

            /* 1.10. For 98EX1x6 devices check for invalid keyTemplate for 4-4  */
            /* mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], */
            /* other parameters is the same as in 1.8. Expected: NON GT_OK.    */
           keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;

           st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                       udbCfg, &includeVlanOrInLifNum, &includeProfileId);
           UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                            "Tiger device: %d, keyTemplate = %d, profileSize = %d",
                                            dev, keyTemplate, profileSize);

           keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_E; /* restore correct value */

            /* 1.11. Set profile size 1-16, one template per sixsteen profiles  */
            /* per one device. For 98EX1x6 devices call function                */
            /* cpssExMxTgPclProfileSizeSet with                                 */
            /* profileSize [CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E]        */
            /* to set one template per profile mode. Expected: GT_OK.           */
           profileSize = CPSS_EXMX_TG_PCL_PROFILE_SIZE_1_TEMPLATES_E;

           st = cpssExMxTgPclProfileSizeSet(dev, profileSize);
           UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                        "cpssExMxTgPclProfileSizeSet: %d, %d",
                                        dev, profileSize);

            /* 1.12. For 98EX1x6 devices call cpssExMxPclTemplateSet with       */
            /* profileId [0], keyTemplate                                       */
            /* [CPSS_EXMX_PCL_TEMPLATE_ANY_E], udbAmount [2],                   */
            /* udbCfg [[CPSS_EXMX_PCL_OFFSET_L2_E, 0],                          */
            /* [CPSS_EXMX_PCL_OFFSET_L4_E, 0]],                                 */
            /* includeVlanOrInLifNum [CPSS_EXMX_PCL_INCLUDE_NONE_E],            */
            /* includeProfileId [GT_FALSE]. Expected: GT_OK for 98EX1x6 devices */
            /* and NON GT_OK for 98EX1x5, 98MX6x5 and 98MX6x8 devices.          */
            profileId = 0;
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_ANY_E;
            udbAmount = 2;
            udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_L2_E;
            udbCfg[0].offset = 0;
            udbCfg[1].offsetType = CPSS_EXMX_PCL_OFFSET_L4_E;
            udbCfg[1].offset = 0;
            includeVlanOrInLifNum = CPSS_EXMX_PCL_INCLUDE_NONE_E;
            includeProfileId = GT_FALSE;

            st = cpssExMxPclTemplateSet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, includeVlanOrInLifNum, includeProfileId);
            UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                                         "Tiger device: %d, %d, %d, <udbAmount>, <udbCfg>, %d, 0; profileSize = %d",
                                         dev, profileId, keyTemplate,
                                         includeVlanOrInLifNum, profileSize);

            /* 1.13. For 98EX1x6 devices call cpssExMxPclTemplateGet with   */
            /* non-NULL udbCfg, includeVlanOrInLifNumPtr,                   */
            /* includeProfileIdPtr, other params same as in 1.12.           */
            /* Expected: GT_OK.                                             */

            st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, &includeVlanOrInLifNum, &includeProfileId);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPclTemplateGet: %d, %d, %d, %d",
                                         dev, profileId, keyTemplate, udbAmount);

            /* 1.14. For 98EX1x6 devices check for out of range for profileId.   */
            /* Call with profileId [16], other parameters is the same as in 1.13.*/ 
            /* Expected: NON GT_OK.                                              */
            profileId = 16;

            st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, &includeVlanOrInLifNum, &includeProfileId);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, profileId = %d",
                                             dev, profileId);

            profileId = 0; /* restore correct value */

            /* 1.15. For 98EX1x6 devices check for invalid keyTemplate for 1-16 */
            /* mode. Call with keyTemplate [CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E], */
            /* other parameters is the same as in 1.13. Expected: NON GT_OK.    */
            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;

            st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                        udbCfg, &includeVlanOrInLifNum, &includeProfileId);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, keyTemplate = %d, profileSize = %d",
                                             dev, keyTemplate, profileSize);

            keyTemplate = CPSS_EXMX_PCL_TEMPLATE_ANY_E; /* restore correct value */
        }

        /* 1.16. Call cpssExMxPclTcamFlush to cleanup after testing.    */
        /* Expected: GT_OK.                                             */
        cpssExMxPclTcamFlush(dev);
    }

    /* set correct values for all the parameters (except of device number) */
    profileId = 0;
    keyTemplate = CPSS_EXMX_PCL_TEMPLATE_IPV4_UDP_E;
    udbAmount = 2;
    udbCfg[0].offsetType = CPSS_EXMX_PCL_OFFSET_L2_E;
    udbCfg[0].offset = 0;
    udbCfg[1].offsetType = CPSS_EXMX_PCL_OFFSET_L4_E;
    udbCfg[1].offset = 0;
    includeVlanOrInLifNum = CPSS_EXMX_PCL_INCLUDE_NONE_E;
    includeProfileId = GT_FALSE;


    /*2. Go over all non active devices. */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                    udbCfg, &includeVlanOrInLifNum, &includeProfileId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPclTemplateGet(dev, profileId, keyTemplate, udbAmount,
                                udbCfg, &includeVlanOrInLifNum, &includeProfileId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cppExMxPcl suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPcl)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclTcamFlush)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclTemplateSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclTemplateDefaultActionSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclRuleSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclRuleActionUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclRuleInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclRuleCopy)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclEngineModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclTwoLookupsActionSelectSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclRulePolicerActionUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclRuleActionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclRuleGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclTemplateDefaultActionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPclTemplateGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPcl)


/*  Internal function. Is used for filling PCL action structure     */
/*  with default values which are used for most of all tests.       */
static void pclActionDefaultSet
(
    IN CPSS_EXMX_POLICY_ACTION_STC  *actionPtr
)
{
    cpssOsBzero((GT_VOID*)actionPtr, sizeof(CPSS_EXMX_POLICY_ACTION_STC));
    actionPtr->pktCmd = CPSS_EXMX_POLICY_ACTION_PKT_FORWARD_E;
    actionPtr->mirror.mirrorToCpu = GT_FALSE;
    actionPtr->mirror.mirrorToRxAnalyzerPort = GT_FALSE;
    actionPtr->qos.markCmd = CPSS_EXMX_POLICY_ACTION_MARK_NONE_E;
    actionPtr->redirect.redirectCmd = CPSS_EXMX_POLICY_ACTION_REDIRECT_CMD_NONE_E;
    actionPtr->policer.policerMode = CPSS_EXMX_POLICY_ACTION_POLICER_DISABLED_E;
    actionPtr->policer.policerId = 0;
    actionPtr->fa.setLbi = GT_FALSE;
    actionPtr->fa.lbi = 0;
    actionPtr->fa.flowId = 0;
    actionPtr->vlan.modifyVlan = CPSS_EXMX_POLICY_ACTION_VLAN_CMD_NOT_MODIFY_E;
    actionPtr->vlan.nestedVlan = GT_FALSE;
    actionPtr->vlan.vlanId = 0;
}
