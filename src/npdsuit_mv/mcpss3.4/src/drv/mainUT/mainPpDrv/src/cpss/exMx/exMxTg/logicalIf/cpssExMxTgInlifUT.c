/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxTgInlifUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxTgInlif.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>

#include <cpss/exMx/exMxTg/logicalIf/cpssExMxTgInlif.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define TG_INLIF_INVALID_ENUM_CNS    0x5AAAAAA5

#if 0 /* in functions contract constant range specified - no need to get this value */
/* Internal functions forward declaration */
static GT_STATUS prvUtfMaxIpv4TunnelTermEntriesGet(IN GT_U8 dev, 
                                                   OUT GT_U32 *maxIpv4TunnelTermEntriesPtr);
#endif

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifPortFieldsSet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            port,
    IN  CPSS_EXMX_INLIF_FIELD_TYPE_ENT   *fieldTypePtr,
    IN  CPSS_EXMX_INLIF_FIELD_VALUE_UNT  *fieldValuePtr,
    IN  GT_U32                           numOfFields
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifPortFieldsSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E; CPSS_EXMX_INLIF_EN_IPV4_E}] and fieldValue [{GT_TRUE; GT_TRUE}], numOfFields [2]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.1.2. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E; CPSS_EXMX_INLIF_DSCP_IPV6_TC_E}] and fieldValue [{GT_TRUE; 1}], numOfFields [2]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.1.3. Check for non-applicable for Tiger field type. For Tiger device call function with fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_E}] and fieldValue [{GT_TRUE}], numOfFields [1]. Expected: NON GT_OK.
1.1.4. For Tiger device call function with out of range fieldType [{0x5AAAAAA5}] and fieldValue [{GT_TRUE}], numOfFields [1]. Expected: NON GT_OK.
1.1.5. For Tiger device call function with fieldTypePtr [NULL] and fieldValue [{1}], numOfFields [1]. Expected: GT_BAD_PTR.
1.1.6. For Tiger device call function with fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}] and fieldValue [NULL], numOfFields [1]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                               dev;
    GT_U8                               port;
    CPSS_EXMX_INLIF_FIELD_TYPE_ENT      fieldTypeArr[2];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT     fieldValueArr[2];
    GT_U32                              numOfFields;

    cpssOsBzero((GT_VOID*)fieldTypeArr, sizeof(fieldTypeArr));
    cpssOsBzero((GT_VOID*)fieldValueArr, sizeof(fieldValueArr));

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with fieldTypePtr                        */
            /* [{CPSS_EXMX_INLIF_DIS_BRG_E; CPSS_EXMX_INLIF_EN_IPV4_E}]      */
            /* and fieldValue [{GT_TRUE; GT_TRUE}], numOfFields [2].         */
            /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.*/
            fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
            fieldValueArr[0].boolField = GT_TRUE;
            fieldTypeArr[1] = CPSS_EXMX_INLIF_EN_IPV4_E;
            fieldValueArr[1].boolField = GT_TRUE;
            numOfFields = 2;

            st = cpssExMxTgInlifPortFieldsSet(dev, port, fieldTypeArr, fieldValueArr, numOfFields);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                         "Tiger device: %d, %d, fieldType[0] = %d, fieldType[1] = %d",
                                         dev, port, fieldTypeArr[0], fieldTypeArr[1]);

            /* 1.1.2. Call function with fieldTypePtr                        */
            /* [{CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E;                         */
            /* CPSS_EXMX_INLIF_DSCP_IPV6_TC_E}] and fieldValue [{GT_TRUE; 1}]*/
            /* numOfFields [2]. Expected: GT_OK for Tiger devices            */
            /* and GT_BAD_PARAM for others.                                  */
            fieldTypeArr[0] = CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E;
            fieldValueArr[0].boolField = GT_TRUE;
            fieldTypeArr[1] = CPSS_EXMX_INLIF_DSCP_IPV6_TC_E;
            fieldValueArr[1].u32Field = 1;
            numOfFields = 2;

            st = cpssExMxTgInlifPortFieldsSet(dev, port, fieldTypeArr, fieldValueArr, numOfFields);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                         "Tiger device: %d, %d, fieldType[0] = %d, fieldType[1] = %d",
                                         dev, port, fieldTypeArr[0], fieldTypeArr[1]);

            /* Tests for Tiger devices only */
            /* 1.1.3. Check for non-applicable for Tiger field type.         */
            /* For Tiger device call function with fieldTypePtr              */
            /* [{CPSS_EXMX_INLIF_EN_MPLS_E}] and fieldValue [{GT_TRUE}],     */
            /* numOfFields [1]. Expected: NON GT_OK.                         */
            fieldTypeArr[0] = CPSS_EXMX_INLIF_EN_MPLS_E;
            fieldValueArr[0].boolField = GT_TRUE;
            numOfFields = 1;

            st = cpssExMxTgInlifPortFieldsSet(dev, port, fieldTypeArr, fieldValueArr, numOfFields);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "Tiger device: %d, %d, fieldType[0] = %d",
                                             dev, port, fieldTypeArr[0]);

            /* 1.1.4. For Tiger device call function with out of range       */
            /* fieldType [{0x5AAAAAA5}] and fieldValue [{GT_TRUE}],          */
            /* numOfFields [1]. Expected: NON GT_OK.                         */
            fieldTypeArr[0] = TG_INLIF_INVALID_ENUM_CNS;
            numOfFields = 1;

            st = cpssExMxTgInlifPortFieldsSet(dev, port, fieldTypeArr, fieldValueArr, numOfFields);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "%d, %d, fieldType[0] = %d",
                                             dev, port, fieldTypeArr[0]);

            /* 1.1.5. For Tiger device call function with fieldTypePtr [NULL]*/
            /*  and fieldValue [{1}], numOfFields [1]. Expected: GT_BAD_PTR. */
            numOfFields = 1;

            st = cpssExMxTgInlifPortFieldsSet(dev, port, NULL, fieldValueArr, numOfFields);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, NULL",
                                         dev, port);

            /* 1.1.6. For Tiger device call function with fieldTypePtr       */
            /* [{CPSS_EXMX_INLIF_DIS_BRG_E}] and fieldValue [NULL],          */
            /* numOfFields [1]. Expected: GT_BAD_PTR.                        */
            fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
            numOfFields = 1;

            st = cpssExMxTgInlifPortFieldsSet(dev, port, fieldTypeArr, NULL, numOfFields);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                         "%d, %d, <fieldValuePtr> = NULL",
                                         dev, port);
        }

        /* Tests for Tiger devices only */

        fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
        fieldValueArr[0].boolField = GT_TRUE;
        numOfFields = 1;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxTgInlifPortFieldsSet(dev, port, fieldTypeArr, fieldValueArr, numOfFields);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxTgInlifPortFieldsSet(dev, port, fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /*2. Go over all non active devices. */
    port = 0;
    fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
    fieldValueArr[0].boolField = GT_TRUE;
    numOfFields = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifPortFieldsSet(dev, port, fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifPortFieldsSet(dev, port, fieldTypeArr, fieldValueArr, numOfFields);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifExpeditedTunnelFieldsSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           expeditedTunnelNum,
    IN  CPSS_EXMX_INLIF_FIELD_TYPE_ENT  *fieldTypePtr,
    IN  CPSS_EXMX_INLIF_FIELD_VALUE_UNT *fieldValuePtr,
    IN  GT_U32                          numOfFields

)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifExpeditedTunnelFieldsSet)
{
/*
ITERATE_DEVICES
1.1. Call function with expeditedTunnelNum [7], fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E; CPSS_EXMX_INLIF_EN_IPV4_E}] and fieldValue [{GT_TRUE; GT_TRUE}], numOfFields [2]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Call function with expeditedTunnelNum [0], fieldTypePtr [{CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E; CPSS_EXMX_INLIF_DSCP_IPV6_TC_E}] and fieldValue [{GT_TRUE; 1}], numOfFields [2]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.3. Check for non-applicable for Tiger field type. For Tiger device call function with expeditedTunnelNum [0], fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_E}] and fieldValue [{GT_TRUE}], numOfFields [1]. Expected: NON GT_OK.
1.4. For Tiger device call function with out of range fieldType [{0x5AAAAAA5}] and fieldValue [{GT_TRUE}], expeditedTunnelNum [0], numOfFields [1]. Expected: NON GT_OK.
1.5. For Tiger device call function with fieldTypePtr [NULL] and fieldValue [{1}], expeditedTunnelNum [0], numOfFields [1]. Expected: GT_BAD_PTR.
1.6. For Tiger device call function with fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}] and fieldValue [NULL], expeditedTunnelNum [0], numOfFields [1]. Expected: GT_BAD_PTR.
1.7. Check for non-valid expeditedTunnelNum. For Tiger device call function with expeditedTunnelNum [7 + 1] and fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}] and fieldValue [NULL], numOfFields [1]. Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    GT_U8                               expeditedTunnelNum;
    CPSS_EXMX_INLIF_FIELD_TYPE_ENT      fieldTypeArr [2];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT     fieldValueArr [2];
    GT_U32                              numOfFields;

    GT_U32                              maxIpv4TunnelTermEntries = 0;

    cpssOsBzero((GT_VOID*)fieldTypeArr, sizeof(fieldTypeArr));
    cpssOsBzero((GT_VOID*)fieldValueArr, sizeof(fieldValueArr));

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        maxIpv4TunnelTermEntries = 7;

        /* st = prvUtfMaxIpv4TunnelTermEntriesGet(dev, &maxIpv4TunnelTermEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMaxIpv4TunnelTermEntriesGet: %d",
                                     dev);
        if (GT_OK != st)
        {
            maxIpv4TunnelTermEntries = 0;
        }
        */

        /* 1.1. Call function with expeditedTunnelNum                       */
        /* [7],                                 */
        /* fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E;                        */
        /* CPSS_EXMX_INLIF_EN_IPV4_E}] and fieldValue [{GT_TRUE; GT_TRUE}], */
        /* numOfFields [2]. Expected: GT_OK for Tiger devices               */
        /* and GT_BAD_PARAM for others.                                     */
        expeditedTunnelNum = (GT_U8) maxIpv4TunnelTermEntries;
        fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
        fieldValueArr[0].boolField = GT_TRUE;
        fieldTypeArr[1] = CPSS_EXMX_INLIF_EN_IPV4_E;
        fieldValueArr[1].boolField = GT_TRUE;
        numOfFields = 2;

        st = cpssExMxTgInlifExpeditedTunnelFieldsSet(dev, expeditedTunnelNum,
                                                     fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                     "Tiger device: %d, %d, fieldType[0] = %d, fieldType[1] = %d",
                                     dev, expeditedTunnelNum, fieldTypeArr[0], fieldTypeArr[1]);

        /* 1.2. Call function with expeditedTunnelNum [0],                  */
        /* fieldTypePtr [{CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E;               */
        /* CPSS_EXMX_INLIF_DSCP_IPV6_TC_E}] and fieldValue [{GT_TRUE; 1}],  */
        /* numOfFields [2]. Expected: GT_OK for Tiger devices               */
        /* and GT_BAD_PARAM for others.                                     */
        expeditedTunnelNum = 0;
        fieldTypeArr[0] = CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E;
        fieldValueArr[0].boolField = GT_TRUE;
        fieldTypeArr[1] = CPSS_EXMX_INLIF_DSCP_IPV6_TC_E;
        fieldValueArr[1].u32Field = 1;
        numOfFields = 2;

        st = cpssExMxTgInlifExpeditedTunnelFieldsSet(dev, expeditedTunnelNum,
                                                     fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                     "Tiger device: %d, %d, fieldType[0] = %d, fieldType[1] = %d",
                                     dev, expeditedTunnelNum, fieldTypeArr[0], fieldTypeArr[1]);

        /* Tests for Tiger devices only */
        /* 1.3. Check for non-applicable for Tiger field type.              */
        /* For Tiger device call function with expeditedTunnelNum [0],      */
        /* fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_E}] and                   */
        /* fieldValue [{GT_TRUE}], numOfFields [1]. Expected: NON GT_OK.    */
        expeditedTunnelNum = 0;
        fieldTypeArr[0] = CPSS_EXMX_INLIF_EN_MPLS_E;
        fieldValueArr[0].boolField = GT_TRUE;
        numOfFields = 1;

        st = cpssExMxTgInlifExpeditedTunnelFieldsSet(dev, expeditedTunnelNum,
                                                     fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                         "Tiger device: %d, %d, fieldType[0] = %d",
                                         dev, expeditedTunnelNum, fieldTypeArr[0]);

        /* 1.4. For Tiger device call function with out of range            */
        /* fieldType [{0x5AAAAAA5}] and fieldValue [{GT_TRUE}],             */
        /* expeditedTunnelNum [0], numOfFields [1]. Expected: NON GT_OK.    */
        fieldTypeArr[0] = TG_INLIF_INVALID_ENUM_CNS;
        numOfFields = 1;
        expeditedTunnelNum = 0;

        st = cpssExMxTgInlifExpeditedTunnelFieldsSet(dev, expeditedTunnelNum,
                                                     fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                     "%d, %d, fieldType[0] = %d",
                                     dev, expeditedTunnelNum, fieldTypeArr[0]);

        /* 1.5. For Tiger device call function with fieldTypePtr [NULL]     */
        /* and fieldValue [{1}], expeditedTunnelNum [0], numOfFields [1].   */
        /* Expected: GT_BAD_PTR.                                            */
        expeditedTunnelNum = 0;
        numOfFields = 1;

        st = cpssExMxTgInlifExpeditedTunnelFieldsSet(dev, expeditedTunnelNum,
                                                     NULL, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, NULL",
                                     dev, expeditedTunnelNum);

        /* 1.6. For Tiger device call function with                         */
        /* fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}] and fieldValue [NULL],*/
        /* expeditedTunnelNum [0], numOfFields [1]. Expected: GT_BAD_PTR.   */
        expeditedTunnelNum = 0;
        fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
        numOfFields = 1;

        st = cpssExMxTgInlifExpeditedTunnelFieldsSet(dev, expeditedTunnelNum,
                                                     fieldTypeArr, NULL, numOfFields);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, fieldValuePtr = NULL",
                                     dev, expeditedTunnelNum);

        /* 1.7. Check for non-valid expeditedTunnelNum.                     */
        /* For Tiger device call function with                              */
        /* expeditedTunnelNum [7 + 1]           */
        /* and fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}] and               */
        /* fieldValue [{GT_TRUE}], numOfFields [1]. Expected: NON GT_OK.    */
        expeditedTunnelNum = (GT_U8) (maxIpv4TunnelTermEntries + 1);
        fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
        fieldValueArr[0].boolField = GT_TRUE;
        numOfFields = 1;

        st = cpssExMxTgInlifExpeditedTunnelFieldsSet(dev, expeditedTunnelNum,
                                                     fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                         "%d, %d", dev, expeditedTunnelNum);
   }

    expeditedTunnelNum = 0;
    fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
    fieldValueArr[0].boolField = GT_TRUE;
    numOfFields = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifExpeditedTunnelFieldsSet(dev, expeditedTunnelNum,
                                                     fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifExpeditedTunnelFieldsSet(dev, expeditedTunnelNum,
                                                 fieldTypeArr, fieldValueArr, numOfFields);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifExpeditedTunnelEntryGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       expeditedTunnelNum,
    OUT GT_U32      *inlifHwDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifExpeditedTunnelEntryGet)
{
/*
ITERATE_DEVICES
1.1. Call function with expeditedTunnelNum [7] and non-NULL inlifHwDataPtr. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Check for out of range expeditedTunnelNum. For Tiger device call function with expeditedTunnelNum [7 + 1] and non-NULL inlifHwDataPtr. Expected: NON GT_OK.
1.3. For Tiger device call function with expeditedTunnelNum [0] inlifHwDataPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    GT_U8                               expeditedTunnelNum;
    GT_U32                              inlifHwDataArr[4];

    GT_U32                              maxIpv4TunnelTermEntries = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        maxIpv4TunnelTermEntries = 7;

        /* st = prvUtfMaxIpv4TunnelTermEntriesGet(dev, &maxIpv4TunnelTermEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMaxIpv4TunnelTermEntriesGet: %d",
                                     dev);
        if (GT_OK != st)
        {
            maxIpv4TunnelTermEntries = 0;
        }
        */

        /* 1.1. Call function with expeditedTunnelNum                       */
        /* [7] and non-NULL inlifHwDataPtr.     */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.   */
        expeditedTunnelNum = (GT_U8) maxIpv4TunnelTermEntries;

        st = cpssExMxTgInlifExpeditedTunnelEntryGet(dev, expeditedTunnelNum, inlifHwDataArr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, expeditedTunnelNum);

        /* Tests for Tiger devices only */
        /* 1.2. Check for out of range expeditedTunnelNum.                  */
        /* For Tiger device call function with expeditedTunnelNum           */
        /* [7 + 1] and non-NULL inlifHwDataPtr. */
        /* Expected: NON GT_OK.                                             */
        expeditedTunnelNum = (GT_U8) (maxIpv4TunnelTermEntries + 1);

        st = cpssExMxTgInlifExpeditedTunnelEntryGet(dev, expeditedTunnelNum, inlifHwDataArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, expeditedTunnelNum);

        /* 1.3. For Tiger device call function with expeditedTunnelNum [0]  */
        /* inlifHwDataPtr [NULL]. Expected: GT_BAD_PTR.                     */
        expeditedTunnelNum = 0;

        st = cpssExMxTgInlifExpeditedTunnelEntryGet(dev, expeditedTunnelNum, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL",
                                     dev, expeditedTunnelNum);
   }

    expeditedTunnelNum = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifExpeditedTunnelEntryGet(dev, expeditedTunnelNum, inlifHwDataArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifExpeditedTunnelEntryGet(dev, expeditedTunnelNum, inlifHwDataArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifExpeditedTunnelEntrySet
(
    IN  GT_U8       devNum,
    IN  GT_U8       expeditedTunnelNum,
    IN  GT_U32      *inlifHwDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifExpeditedTunnelEntrySet)
{
/*
ITERATE_DEVICES
1.1. Call function with expeditedTunnelNum [7], inlifHwDataPtr [{1234, 0, 0, 5678}]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Call function cpssExMxTgInlifExpeditedTunnelEntryGet with expeditedTunnelNum [7] and non-NULL inlifHwDataPtr. Expected: For Tiger device GT_OK and inlifHwData [{1234, 0, 0, 5678}] and GT_BAD_PARAM for other devices.
1.3. Check for out of range expeditedTunnelNum. For Tiger device call function with expeditedTunnelNum [7 + 1] and non-NULL inlifHwDataPtr. Expected: NON GT_OK.
1.4. For Tiger device call function with inlifHwDataPtr [NULL] and expeditedTunnelNum [0]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    GT_U8                               expeditedTunnelNum;
    GT_U32                              inlifHwDataArr[4];

    GT_U32                              maxIpv4TunnelTermEntries = 0;
    GT_U32                              retInlifHwDataArr[4];
    GT_BOOL                             failureWas;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        maxIpv4TunnelTermEntries = 7;

        /* st = prvUtfMaxIpv4TunnelTermEntriesGet(dev, &maxIpv4TunnelTermEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMaxIpv4TunnelTermEntriesGet: %d",
                                     dev);
        if (GT_OK != st)
        {
            maxIpv4TunnelTermEntries = 0;
        }
        */

        /* 1.1. Call function with expeditedTunnelNum                       */
        /* [7] and                              */
        /* inlifHwDataPtr [{1234, 0, 0, 5678}].                             */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.   */
        expeditedTunnelNum = (GT_U8) maxIpv4TunnelTermEntries;

        cpssOsBzero((GT_VOID*)inlifHwDataArr, sizeof(inlifHwDataArr));
        inlifHwDataArr[0] = 1234;
        inlifHwDataArr[3] = 5678;

        st = cpssExMxTgInlifExpeditedTunnelEntrySet(dev, expeditedTunnelNum, inlifHwDataArr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, expeditedTunnelNum);

        /* restore inlifHwData in case function has changed it */
        cpssOsBzero((GT_VOID*)inlifHwDataArr, sizeof(inlifHwDataArr));
        inlifHwDataArr[0] = 1234;
        inlifHwDataArr[3] = 5678;

        /* 1.2. Call function cpssExMxTgInlifExpeditedTunnelEntryGet        */
        /* with expeditedTunnelNum [7] and      */
        /* non-NULL inlifHwDataPtr. Expected: For Tiger device GT_OK and    */
        /* inlifHwData [{1234, 0, 0, 5678}] and GT_BAD_PARAM for other      */
        /* devices.                                                         */

        st = cpssExMxTgInlifExpeditedTunnelEntryGet(dev, expeditedTunnelNum, retInlifHwDataArr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                     "cpssExMxTgInlifExpeditedTunnelEntryGet: Tiger device: %d, %d",
                                     dev, expeditedTunnelNum);
        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)inlifHwDataArr,
                                            (const GT_VOID*)retInlifHwDataArr, sizeof(inlifHwDataArr))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                                         "get another InLIF entry than has been written: %d, %d",
                                         dev, expeditedTunnelNum);
        }

        /* Tests for Tiger devices only */
        /* 1.3. Check for out of range expeditedTunnelNum.                  */
        /* For Tiger device call function with expeditedTunnelNum           */
        /* [7 + 1] and non-NULL inlifHwDataPtr. */
        /* Expected: NON GT_OK.                                             */
        expeditedTunnelNum = (GT_U8) (maxIpv4TunnelTermEntries + 1);

        st = cpssExMxTgInlifExpeditedTunnelEntrySet(dev, expeditedTunnelNum, inlifHwDataArr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, expeditedTunnelNum);

        /* 1.4. For Tiger device call function with expeditedTunnelNum [0]  */
        /* inlifHwDataPtr [NULL]. Expected: GT_BAD_PTR.                     */
        expeditedTunnelNum = 0;

        st = cpssExMxTgInlifExpeditedTunnelEntrySet(dev, expeditedTunnelNum, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL",
                                     dev, expeditedTunnelNum);
   }

    expeditedTunnelNum = 0;
    cpssOsBzero((GT_VOID*)inlifHwDataArr, sizeof(inlifHwDataArr));

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifExpeditedTunnelEntrySet(dev, expeditedTunnelNum, inlifHwDataArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifExpeditedTunnelEntrySet(dev, expeditedTunnelNum, inlifHwDataArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifVlanFieldsSet
(
    IN  GT_U8                            devNum,
    IN  GT_U16                           vlan,
    IN  CPSS_EXMX_INLIF_FIELD_TYPE_ENT   *fieldTypePtr,
    IN  CPSS_EXMX_INLIF_FIELD_VALUE_UNT  *fieldValuePtr,
    IN  GT_U32                           numOfFields
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifVlanFieldsSet)
{
/*
ITERATE_DEVICES
1.1. Call function with vlan [100], fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E; CPSS_EXMX_INLIF_EN_IPV4_E}] and fieldValue [{GT_TRUE; GT_TRUE}], numOfFields [2]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. For Tiger device call function with valn [99], fieldTypePtr [{CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E (not supported); CPSS_EXMX_INLIF_DSCP_IPV6_TC_E}] and fieldValue [{GT_TRUE; 1}], numOfFields [2]. Expected: GT_BAD_PARAM.
1.3. Check for field type invalid for reduced inlif entry. For Tiger devices call function with vlan [100], fieldTypePtr [CPSS_EXMX_INLIF_DROP_RES_SIP_E], fieldValuePtr [GT_TRUE], numOfFields [1]. Expected: GT_OK for Tiger devices with non-reduced inlif entry and NON GT_OK for Tiger devices with reduced inlif entry.
1.4. Check for non-applicable for Tiger field type. For Tiger device call function with valn [100], fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_E}] and fieldValue [{GT_TRUE}], numOfFields [1]. Expected: NON GT_OK.
1.5. For Tiger device call function with out of range fieldType [{0x5AAAAAA5}] and fieldValue [{GT_TRUE}], vlan [100], numOfFields [1]. Expected: NON GT_OK.
1.6. For Tiger device call function with fieldTypePtr [NULL] and fieldValue [{1}], vlan [100], numOfFields [1]. Expected: GT_BAD_PTR.
1.7. For Tiger device call function with fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}] and fieldValue [NULL], vlan [100], numOfFields [1]. Expected: GT_BAD_PTR.
1.8. Check for non-valid vlan. For Tiger device call function with vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] and fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}] and fieldValue [{GT_TRUE}], numOfFields [1]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    GT_U16                              vlan;
    CPSS_EXMX_INLIF_FIELD_TYPE_ENT      fieldTypeArr [2];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT     fieldValueArr [2];
    GT_U32                              numOfFields;

    cpssOsBzero((GT_VOID*)fieldTypeArr, sizeof(fieldTypeArr));
    cpssOsBzero((GT_VOID*)fieldValueArr, sizeof(fieldValueArr));

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with vlan [100]                               */
        /* fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E;                        */
        /* CPSS_EXMX_INLIF_EN_IPV4_E}] and fieldValue [{GT_TRUE; GT_TRUE}], */
        /* numOfFields [2]. Expected: GT_OK for Tiger devices               */
        /* and GT_BAD_PARAM for others.                                     */
        vlan = 100;
        fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
        fieldValueArr[0].boolField = GT_TRUE;
        fieldTypeArr[1] = CPSS_EXMX_INLIF_EN_IPV4_E;
        fieldValueArr[1].boolField = GT_TRUE;
        numOfFields = 2;

        st = cpssExMxTgInlifVlanFieldsSet(dev, vlan, fieldTypeArr,
                                          fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                     "Tiger device: %d, %d, fieldType[0] = %d, fieldType[1] = %d",
                                     dev, vlan, fieldTypeArr[0], fieldTypeArr[1]);

        /* 1.2. For Tiger call function with vlan [99]                          */
        /* fieldTypePtr [{CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E (not supported);   */
        /* CPSS_EXMX_INLIF_DSCP_IPV6_TC_E}] and fieldValue [{GT_TRUE; 1}],      */
        /* numOfFields [2]. Expected: GT_BAD_PARAM                              */
        vlan = 99;
        fieldTypeArr[0] = CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E;
        fieldValueArr[0].boolField = GT_TRUE;
        fieldTypeArr[1] = CPSS_EXMX_INLIF_DSCP_IPV6_TC_E;
        fieldValueArr[1].u32Field = 1;
        numOfFields = 2;

        st = cpssExMxTgInlifVlanFieldsSet(dev, vlan, fieldTypeArr,
                                          fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st,
                                     "All device: %d, %d, fieldType[0] = %d, fieldType[1] = %d",
                                     dev, vlan, fieldTypeArr[0], fieldTypeArr[1]);
/*
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                         "Tiger device: %d, %d, fieldType[0] = %d, fieldType[1] = %d",
                                         dev, vlan, fieldTypeArr[0], fieldTypeArr[1]);
        }
        else
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PARAM, st,
                                         "non-Tiger device: %d", dev);
        }
*/
        /* Tests for Tiger devices only */
        /* 1.3. Check for field type invalid for reduced inlif entry.       */
        /* For Tiger devices call function with vlan [100],                 */
        /* fieldTypePtr [CPSS_EXMX_INLIF_DROP_RES_SIP_E],                   */
        /* fieldValuePtr [GT_TRUE], numOfFields [1].                        */
        /* Expected: GT_OK for Tiger devices with non-reduced inlif entry   */
        /* and NON GT_OK for Tiger devices with reduced inlif entry.        */
        vlan = 100;
        fieldTypeArr[0] = CPSS_EXMX_INLIF_DROP_RES_SIP_E;
        fieldValueArr[0].boolField = GT_TRUE;
        numOfFields = 1;

        st = cpssExMxTgInlifVlanFieldsSet(dev, vlan, fieldTypeArr,
                                          fieldValueArr, numOfFields);
        if (PRV_CPSS_EXMX_PP_MAC(dev)->moduleCfg.logicalIfCfg.vlanInlifEntryType
            != CPSS_EXMX_VLAN_INLIF_ENTRY_TYPE_REDUCED_E)
        {
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                         "for Tiger device, non-reduced vlan inlif entry: %d, %d, fieldType[0] = %d",
                                         dev, vlan, fieldTypeArr[0]);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "for Tiger device, reduced vlan inlif entry: %d, %d, fieldType[0] = %d",
                                             dev, vlan, fieldTypeArr[0]);
        }

        /* 1.4. Check for non-applicable for Tiger field type.              */
        /* For Tiger device call function with vlan [100],                  */
        /* fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_E}] and                   */
        /* fieldValue [{GT_TRUE}], numOfFields [1]. Expected: NON GT_OK.    */
        vlan = 100;
        fieldTypeArr[0] = CPSS_EXMX_INLIF_EN_MPLS_E;
        fieldValueArr[0].boolField = GT_TRUE;
        numOfFields = 1;

        st = cpssExMxTgInlifVlanFieldsSet(dev, vlan, fieldTypeArr,
                                          fieldValueArr, numOfFields);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                         "Tiger device: %d, %d, fieldType[0] = %d",
                                         dev, vlan, fieldTypeArr[0]);

        /* 1.5. For Tiger device call function with out of range    */
        /* fieldType [{0x5AAAAAA5}] and fieldValue [{GT_TRUE}],     */
        /* vlan [100], numOfFields [1]. Expected: NON GT_OK.        */
        fieldTypeArr[0] = TG_INLIF_INVALID_ENUM_CNS;
        numOfFields = 1;
        vlan = 100;

        st = cpssExMxTgInlifVlanFieldsSet(dev, vlan, fieldTypeArr,
                                          fieldValueArr, numOfFields);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                     "%d, %d, fieldType[0] = %d",
                                     dev, vlan, fieldTypeArr[0]);

        /* 1.6. For Tiger device call function with fieldTypePtr [NULL]     */
        /* and fieldValue [{1}], vlan [100], numOfFields [1].               */
        /* Expected: GT_BAD_PTR.                                            */
        vlan = 100;
        numOfFields = 1;

        st = cpssExMxTgInlifVlanFieldsSet(dev, vlan, NULL, fieldValueArr,
                                          numOfFields);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, vlan);

        /* 1.7. For Tiger device call function with                         */
        /* fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}] and fieldValue [NULL],*/
        /* vlan [100], numOfFields [1]. Expected: GT_BAD_PTR.               */
        vlan = 100;
        fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
        numOfFields = 1;

        st = cpssExMxTgInlifVlanFieldsSet(dev, vlan, fieldTypeArr, NULL,
                                          numOfFields);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, fieldValuePtr = NULL",
                                     dev, vlan);

        /* 1.8. Check for non-valid vlan.                                   */
        /* For Tiger device call function with                              */
        /* vlan [PRV_CPSS_MAX_NUM_VLANS_CNS]                                */
        /* and fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}] and               */
        /* fieldValue [{GT_TRUE}], numOfFields [1]. Expected: GT_BAD_PARAM. */
        vlan = PRV_CPSS_MAX_NUM_VLANS_CNS;
        fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
        fieldValueArr[0].boolField = GT_TRUE;
        numOfFields = 1;

        st = cpssExMxTgInlifVlanFieldsSet(dev, vlan, fieldTypeArr,
                                          fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlan);
   }

    vlan = 100;
    fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
    fieldValueArr[0].boolField = GT_TRUE;
    numOfFields = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifVlanFieldsSet(dev, vlan, fieldTypeArr,
                                          fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifVlanFieldsSet(dev, vlan, fieldTypeArr, fieldValueArr,
                                      numOfFields);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifVlanEntryGet
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlan,
    OUT GT_U32                      *inlifHwDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifVlanEntryGet)
{
/*
ITERATE_DEVICES
1.1. Call function with vlan [100] and non-NULL inlifHwDataPtr. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Check for out of range vlan. For Tiger device call function with vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] and non-NULL inlifHwDataPtr. Expected: GT_BAD_PARAM.
1.3. For Tiger device call function with vlan [100] inlifHwDataPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    GT_U16                              vlan;
    GT_U32                              inlifHwDataArr[4];

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with vlan [100] and non-NULL inlifHwDataPtr.  */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.   */
        vlan = 100;

        st = cpssExMxTgInlifVlanEntryGet(dev, vlan, inlifHwDataArr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, vlan);

        /* 1.2. Check for out of range vlan.                                */
        /* For Tiger device call function with                              */
        /* vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] and non-NULL inlifHwDataPtr.   */
        /* Expected: GT_BAD_PARAM.                                          */
        vlan = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxTgInlifVlanEntryGet(dev, vlan, inlifHwDataArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlan);

        /* 1.3. For Tiger device call function with vlan [100]  */
        /* inlifHwDataPtr [NULL]. Expected: GT_BAD_PTR.         */
        vlan = 100;

        st = cpssExMxTgInlifVlanEntryGet(dev, vlan, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL",
                                     dev, vlan);
    }

    vlan = 100;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifVlanEntryGet(dev, vlan, inlifHwDataArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifVlanEntryGet(dev, vlan, inlifHwDataArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifVlanEntrySet
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlan,
    IN  GT_U32                      *inlifHwDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifVlanEntrySet)
{
/*
ITERATE_DEVICES
1.1. Call function with vlan [100], inlifHwDataPtr [{1234, 0, 0, 5678}]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Call function cpssExMxTgInlifVlanEntryGet with vlan [100] and non-NULL inlifHwDataPtr. Expected: For Tiger device GT_OK and inlifHwData [{1234, 0, 0, 5678}] - for non-reduced inlif entry and inlifHwData [{1234, 0}] - for reduced one; GT_BAD_PARAM for non-Tiger devices.
1.3. Call function with vlan [1], inlifHwDataPtr [{0, 1234, 5678, 0}]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.4. Call function cpssExMxTgInlifVlanEntryGet with vlan [1] and non-NULL inlifHwDataPtr. Expected: For Tiger device GT_OK and inlifHwData [{1234, 0, 0, 5678}] - for non-reduced inlif entry and inlifHwData [{1234, 0}] - for reduced one; GT_BAD_PARAM for non-Tiger devices.
1.5. Check for out of range vlan. For Tiger device call function with vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] and non-NULL inlifHwDataPtr. Expected: GT_BAD_PARAM.
1.6. For Tiger device call function with inlifHwDataPtr [NULL] and vlan [100]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                      dev;
    GT_U16                     vlan;
    GT_U32                     inlifHwDataArr[4];

    GT_U32                     retInlifHwDataArr[4];
    GT_BOOL                    failureWas;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with vlan [100], inlifHwDataPtr                */
        /* [{1234, 0, 0, 5678}]. Expected: GT_OK for Tiger devices           */
        /* and GT_BAD_PARAM for others.                                      */
        vlan = 100;

        cpssOsBzero((GT_VOID*)inlifHwDataArr, sizeof(inlifHwDataArr));
        inlifHwDataArr[0] = 1234;
        inlifHwDataArr[3] = 5678;

        st = cpssExMxTgInlifVlanEntrySet(dev, vlan, inlifHwDataArr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, vlan);

        /* restore inlifHwData in case function has changed it */
        cpssOsBzero((GT_VOID*)inlifHwDataArr, sizeof(inlifHwDataArr));
        inlifHwDataArr[0] = 1234;
        inlifHwDataArr[3] = 5678;

        /* 1.2. Call function cpssExMxTgInlifVlanEntryGet with vlan [100]    */
        /* and non-NULL inlifHwDataPtr. Expected: For Tiger device GT_OK     */
        /*and inlifHwData [{1234, 0, 0, 5678}] - for non-reduced inlif entry */
        /* and inlifHwData [{1234, 0}] - for reduced one;                    */
        /* GT_BAD_PARAM for non-Tiger devices.                               */

        st = cpssExMxTgInlifVlanEntryGet(dev, vlan, retInlifHwDataArr);
        /* Tiger device */
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                     "cpssExMxTgInlifVlanEntryGet: Tiger device: %d, %d",
                                     dev, vlan);
        if (GT_OK == st)
        {
            if (PRV_CPSS_EXMX_PP_MAC(dev)->moduleCfg.logicalIfCfg.vlanInlifEntryType
                != CPSS_EXMX_VLAN_INLIF_ENTRY_TYPE_REDUCED_E)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)inlifHwDataArr,
                                                (const GT_VOID*)retInlifHwDataArr, sizeof(inlifHwDataArr[0])*4)) ? GT_FALSE : GT_TRUE;
            }
            else
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)inlifHwDataArr,
                                                (const GT_VOID*)retInlifHwDataArr, sizeof(inlifHwDataArr[0])*2)) ? GT_FALSE : GT_TRUE;
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                                         "get another InLIF entry than has been written: %d, %d",
                                         dev, vlan);
        }

        /* 1.3. Call function with vlan [1], inlifHwDataPtr                  */
        /* [{0, 1234, 5678, 0}]. Expected: GT_OK for Tiger devices           */
        /* and GT_BAD_PARAM for others.                                      */
        vlan = 1;

        cpssOsBzero((GT_VOID*)inlifHwDataArr, sizeof(inlifHwDataArr));
        inlifHwDataArr[1] = 1234;
        inlifHwDataArr[2] = 5678;

        st = cpssExMxTgInlifVlanEntrySet(dev, vlan, inlifHwDataArr);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, vlan);

        /* restore inlifHwData in case function has changed it */
        cpssOsBzero((GT_VOID*)inlifHwDataArr, sizeof(inlifHwDataArr[0])*4);
        inlifHwDataArr[1] = 1234;
        inlifHwDataArr[2] = 5678;

        /* 1.4. Call function cpssExMxTgInlifVlanEntryGet with vlan [1]      */
        /* and non-NULL inlifHwDataPtr. Expected: For Tiger device GT_OK     */
        /* and inlifHwData [{0, 1234, 5678, 0}] - for non-reduced inlif entry*/
        /* and inlifHwData [{0, 1234}] - for reduced one;                    */
        /* GT_BAD_PARAM for non-Tiger devices.                               */

        st = cpssExMxTgInlifVlanEntryGet(dev, vlan, retInlifHwDataArr);

        /* Tiger device */
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                     "cpssExMxTgInlifVlanEntryGet: Tiger device: %d, %d",
                                     dev, vlan);
        if (GT_OK == st)
        {
            if (PRV_CPSS_EXMX_PP_MAC(dev)->moduleCfg.logicalIfCfg.vlanInlifEntryType
                != CPSS_EXMX_VLAN_INLIF_ENTRY_TYPE_REDUCED_E)
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)inlifHwDataArr,
                                                (const GT_VOID*)retInlifHwDataArr, sizeof(inlifHwDataArr[0])*4)) ? GT_FALSE : GT_TRUE;
            }
            else
            {
                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)inlifHwDataArr,
                                                (const GT_VOID*)retInlifHwDataArr, sizeof(inlifHwDataArr[0])*2)) ? GT_FALSE : GT_TRUE;
            }

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                                         "get another InLIF entry than has been written: %d, %d",
                                         dev, vlan);
        }

        /* Tests for Tiger devices only */
        /* 1.5. Check for out of range vlan. For Tiger device call function  */
        /* with vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] and non-NULL inlifHwDataPtr*/
        /* Expected: GT_BAD_PARAM.                                           */
        vlan = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxTgInlifVlanEntrySet(dev, vlan, inlifHwDataArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlan);

        /* 1.6. For Tiger device call function with inlifHwDataPtr [NULL]    */
        /* and vlan [100]. Expected: GT_BAD_PTR.                             */
        vlan = 100;

        st = cpssExMxTgInlifVlanEntrySet(dev, vlan, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL",
                                     dev, vlan);
   }

    vlan = 100;
    cpssOsBzero((GT_VOID*)inlifHwDataArr, sizeof(inlifHwDataArr));

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifVlanEntrySet(dev, vlan, inlifHwDataArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifVlanEntrySet(dev, vlan, inlifHwDataArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifVlanMappingSet
(
    IN    GT_U8               devNum,
    IN    GT_U16              vlan,
    IN    GT_U32              index
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifVlanMappingSet)
{
/*
ITERATE_DEVICES
1.1. Call function with vlan [100] and index [100]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Check for out of range vlan. For Tiger devices call function with vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] and index[100]. Expected: GT_BAD_PARAM.
1.3. Check for out range index. For Tiger devices call function with index [4096] and vlan[100]. Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    GT_U16                              vlan;
    GT_U32                              index;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with vlan [100] and index [100].             */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.  */
        vlan = 100;
        index = 100;

        st = cpssExMxTgInlifVlanMappingSet(dev, vlan, index);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d",
                                     dev, vlan, index);

        /* Tests for Tiger devices only */
        /* 1.2. Check for out of range vlan. For Tiger devices call         */
        /* function with vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] and index[100].  */
        /* Expected: GT_BAD_PARAM.                                          */
        vlan = PRV_CPSS_MAX_NUM_VLANS_CNS;
        index = 100;

        st = cpssExMxTgInlifVlanMappingSet(dev, vlan, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlan);

        /* 1.3. Check for out range index. For Tiger devices call           */
        /* function with index [4096] and vlan[100]. Expected: NON GT_OK.   */
        vlan = 100;
        index = 4096;

        st = cpssExMxTgInlifVlanMappingSet(dev, vlan, index);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlan, index);
   }

    vlan = 100;
    index = 100;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifVlanMappingSet(dev, vlan, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifVlanMappingSet(dev, vlan, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifExpeditedTunnelLookupModeSet
(
    IN  GT_U8                         devNum,
    IN  GT_U8                         expeditedTunnelNum,
    IN  CPSS_EXMX_INLIF_PORT_MODE_ENT inlifmode
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifExpeditedTunnelLookupModeSet)
{
/*
ITERATE_DEVICES
1.1. Call function with expeditedTunnelNum [7] and ilifMode [CPSS_EXMX_HYBRID_INLIF_E]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
1.2. Call function cpssExMxTgInlifExpeditedTunnelLookupModeGet with expeditedTunnelNum [7] and non-NULL ilifModePtr. Expected: GT_OK and inlifModePtr [CPSS_EXMX_HYBRID_INLIF_E] for Tiger devices and NON GT_OK for others.
1.3. Check for out of range expeditedTunnelNum. For Tiger devices call function with expeditedTunnelNum [7 + 1] and inlifMode [CPSS_EXMX_HYBRID_INLIF_E]. Expected: NON GT_OK.
1.4. Check for out range inlifMode. For Tiger devices call function with inlifMode [0x5AAAAAA5] and expeditedTunnelNum [7]. Expected: GT_BAD_PARAM.
1.5. Check for invalid for Tiger devices inlifMode. For Tiger devices call function with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E/ CPSS_EXMX_SINGLE_INLIF_E] and expeditedTunnelNum [7]. Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    GT_U8                               expeditedTunnelNum;
    CPSS_EXMX_INLIF_PORT_MODE_ENT       inlifmode;

    GT_U32                              maxIpv4TunnelTermEntries = 0;
    CPSS_EXMX_INLIF_PORT_MODE_ENT       retInlifmode;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        maxIpv4TunnelTermEntries = 7;

        /* st = prvUtfMaxIpv4TunnelTermEntriesGet(dev, &maxIpv4TunnelTermEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMaxIpv4TunnelTermEntriesGet: %d",
                                     dev);
        if (GT_OK != st)
        {
            maxIpv4TunnelTermEntries = 0;
        }
        */

        /* 1.1. Call function with expeditedTunnelNum                       */
        /* [7] and ilifMode                     */
        /* [CPSS_EXMX_HYBRID_INLIF_E]. Expected: GT_OK for Tiger devices    */
        /* and GT_BAD_PARAM for others.                                     */
        expeditedTunnelNum = (GT_U8) maxIpv4TunnelTermEntries;
        inlifmode = CPSS_EXMX_HYBRID_INLIF_E;

        st = cpssExMxTgInlifExpeditedTunnelLookupModeSet(dev, expeditedTunnelNum, inlifmode);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d",
                                     dev, expeditedTunnelNum, inlifmode);

        /* 1.2. Call function cpssExMxTgInlifExpeditedTunnelLookupModeGet   */
        /* with expeditedTunnelNum [7]          */
        /* and non-NULL ilifModePtr. Expected: GT_OK and inlifModePtr       */
        /* [CPSS_EXMX_HYBRID_INLIF_E] for Tiger devices                     */
        /* and GT_BAD_PARAM for others.                                     */

        st = cpssExMxTgInlifExpeditedTunnelLookupModeGet(dev, expeditedTunnelNum, &retInlifmode);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                                     "cpssExMxTgInlifExpeditedTunnelLookupModeGet: Tiger device: %d, %d, %d",
                                     dev, expeditedTunnelNum, inlifmode);
        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(inlifmode, retInlifmode, 
                                         "Tiger device: get another InLIf Lookup mode than has been set: %d, %d",
                                         dev, expeditedTunnelNum);
        }

        /* 1.3. Check for out of range expeditedTunnelNum. For Tiger devices*/
        /* call function with expeditedTunnelNum                            */
        /* [7 + 1] and inlifMode                */
        /* [CPSS_EXMX_HYBRID_INLIF_E]. Expected: NON GT_OK.                 */
        expeditedTunnelNum = (GT_U8) (maxIpv4TunnelTermEntries + 1);
        inlifmode = CPSS_EXMX_HYBRID_INLIF_E;

        st = cpssExMxTgInlifExpeditedTunnelLookupModeSet(dev, expeditedTunnelNum, inlifmode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, expeditedTunnelNum);

        /* 1.4. Check for out range inlifMode. For Tiger devices    */
        /* call function with inlifMode [0x5AAAAAA5] and            */
        /* expeditedTunnelNum [0]. Expected: GT_BAD_PARAM.          */
        expeditedTunnelNum = 0;
        inlifmode = TG_INLIF_INVALID_ENUM_CNS;

        st = cpssExMxTgInlifExpeditedTunnelLookupModeSet(dev, expeditedTunnelNum, inlifmode);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, expeditedTunnelNum, inlifmode);

        /* 1.5. Check for invalid for Tiger devices inlifMode.          */
        /* For Tiger devices call function with inlifMode               */
        /* [CPSS_EXMX_MULTIPLE_INLIF_E/ CPSS_EXMX_SINGLE_INLIF_E] and   */
        /* expeditedTunnelNum [7].          */
        /* Expected: NON GT_OK.                                         */
        expeditedTunnelNum = 0;

        inlifmode = CPSS_EXMX_MULTIPLE_INLIF_E;

        st = cpssExMxTgInlifExpeditedTunnelLookupModeSet(dev, expeditedTunnelNum, inlifmode);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d",
                                         dev, expeditedTunnelNum, inlifmode);

        inlifmode = CPSS_EXMX_SINGLE_INLIF_E;

        st = cpssExMxTgInlifExpeditedTunnelLookupModeSet(dev, expeditedTunnelNum, inlifmode);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d",
                                         dev, expeditedTunnelNum, inlifmode);
   }

    expeditedTunnelNum = 0;
    inlifmode = CPSS_EXMX_HYBRID_INLIF_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifExpeditedTunnelLookupModeSet(dev, expeditedTunnelNum, inlifmode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifExpeditedTunnelLookupModeSet(dev, expeditedTunnelNum, inlifmode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifExpeditedTunnelLookupModeGet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            expeditedTunnelNum,
    OUT CPSS_EXMX_INLIF_PORT_MODE_ENT    *inlifModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifExpeditedTunnelLookupModeGet)
{
/*
ITERATE_DEVICES
1.1. Call function with expeditedTunnelNum [7] and non-NULL ilifModePtr. Expected: GT_OK for Tiger devices and NON GT_OK for others.
1.2. Check for out of range expeditedTunnelNum. For Tiger devices call function with expeditedTunnelNum [7 + 1] and non-NULL inlifModePtr. Expected: NON GT_OK.
1.3. Check for NULL pointer. For Tiger devices call function with inlifModePtr [NULL] and expeditedTunnelNum [7]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    GT_U8                               expeditedTunnelNum;
    CPSS_EXMX_INLIF_PORT_MODE_ENT       inlifmode;

    GT_U32                              maxIpv4TunnelTermEntries = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        maxIpv4TunnelTermEntries = 7;

        /* st = prvUtfMaxIpv4TunnelTermEntriesGet(dev, &maxIpv4TunnelTermEntries);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMaxIpv4TunnelTermEntriesGet: %d",
                                     dev);
        if (GT_OK != st)
        {
            maxIpv4TunnelTermEntries = 0;
        }
        */

        /* 1.1. Call function with expeditedTunnelNum                       */
        /* [7] and non-NULL ilifMode            */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.   */
        expeditedTunnelNum = (GT_U8) maxIpv4TunnelTermEntries;

        st = cpssExMxTgInlifExpeditedTunnelLookupModeGet(dev, expeditedTunnelNum, &inlifmode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, expeditedTunnelNum);

        /* Tests for Tiger devices only */
        /* 1.2. Check for out of range expeditedTunnelNum. For Tiger        */
        /* devices call function with expeditedTunnelNum                    */
        /* [7 + 1] and non-NULL inlifMode       */
        /* Expected: NON GT_OK.                                             */
        expeditedTunnelNum = (GT_U8) (maxIpv4TunnelTermEntries + 1);

        st = cpssExMxTgInlifExpeditedTunnelLookupModeGet(dev, expeditedTunnelNum, &inlifmode);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, expeditedTunnelNum);

        /* 1.3. Check for NULL pointer. For Tiger devices call function */
        /* with inlifModePtr [NULL] and expeditedTunnelNum [0].         */
        /* Expected: GT_BAD_PTR.                                        */
        expeditedTunnelNum = 0;

        st = cpssExMxTgInlifExpeditedTunnelLookupModeGet(dev, expeditedTunnelNum, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "Tiger device: %d, %d, NULL",
                                     dev, expeditedTunnelNum);
   }

    expeditedTunnelNum = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifExpeditedTunnelLookupModeGet(dev, expeditedTunnelNum, &inlifmode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifExpeditedTunnelLookupModeGet(dev, expeditedTunnelNum, &inlifmode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable
(
    IN  GT_U8         devNum,
    IN  GT_BOOL       enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable)
{
/*
ITERATE_DEVICES
1.1. Call function with enable [GT_TRUE and GT_FALSE]. Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                       dev;
    GT_BOOL                     enable;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with enable [GT_TRUE and GT_FALSE].           */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others.   */ 
        enable = GT_TRUE;

        st = cpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, enable);

        enable = GT_FALSE;

        st = cpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, enable);
   }

    enable = GT_FALSE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgInlifIgmpCommandSet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMX_INLIF_COMMAND_ENT         command
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgInlifIgmpCommandSet)
{
/*
ITERATE_DEVICES
1.1. Call function with command [CPSS_EXMX_INLIF_IGMP_FORWARDING_E\ CPSS_EXMX_INLIF_IGMP_TRAP_TO_CPU_E\ CPSS_EXMX_INLIF_IGMP_MIRROR_TO_CPU_E]. Expected: GT_OK for Tiger devices and NON GT_OK for others.
1.2. For Tiger devices call function with out of range command [0x5AAAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                           dev;
    CPSS_EXMX_INLIF_COMMAND_ENT     command;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with command                                */
        /* [CPSS_EXMX_INLIF_IGMP_FORWARDING_E\                            */
        /* CPSS_EXMX_INLIF_IGMP_TRAP_TO_CPU_E\                            */
        /* CPSS_EXMX_INLIF_IGMP_MIRROR_TO_CPU_E].                         */
        /* Expected: GT_OK for Tiger devices and GT_BAD_PARAM for others. */
        command = CPSS_EXMX_INLIF_IGMP_FORWARDING_E;

        st = cpssExMxTgInlifIgmpCommandSet(dev, command);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, command);

        command = CPSS_EXMX_INLIF_IGMP_TRAP_TO_CPU_E;

        st = cpssExMxTgInlifIgmpCommandSet(dev, command);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, command);

        command = CPSS_EXMX_INLIF_IGMP_MIRROR_TO_CPU_E;

        st = cpssExMxTgInlifIgmpCommandSet(dev, command);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, command);

        /* 1.2. For Tiger devices call function with out of range   */
        /* command [0x5AAAAAAA5]. Expected: GT_BAD_PARAM.           */
        command = TG_INLIF_INVALID_ENUM_CNS;

        st = cpssExMxTgInlifIgmpCommandSet(dev, command);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, command);
   }

    command = GT_FALSE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTgInlifIgmpCommandSet(dev, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgInlifIgmpCommandSet(dev, command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxTgInlif suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxTgInlif)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifPortFieldsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifExpeditedTunnelFieldsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifExpeditedTunnelEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifExpeditedTunnelEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifVlanFieldsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifVlanEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifVlanEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifVlanMappingSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifExpeditedTunnelLookupModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifExpeditedTunnelLookupModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgInlifIgmpCommandSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxTgInlif)


#if 0 /* in functions contract constant range specified - no need to get this value */

/*******************************************************************************
* prvUtfMaxIpv4TunnelTermEntriesGet
*
* DESCRIPTION:
*       This routine returns maximum number of ipv4 tunnel termination entries per device.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       maxMcGroupsPtr     -  max number of ipv4 tunnel termination entries
*
*       GT_OK           -   Get max number of ipv4 tunnel termination entries was OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfMaxIpv4TunnelTermEntriesGet
(
    IN  GT_U8       dev,
    OUT GT_U32      *maxIpv4TunnelTermEntriesPtr
)
{
    PRV_CPSS_EXMX_PP_CONFIG_STC  *exMxDevPtr;

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(maxIpv4TunnelTermEntriesPtr);

    /* check if dev active and from Tiger family */
    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(dev);
    exMxDevPtr = PRV_CPSS_EXMX_PP_MAC(dev);

    *maxIpv4TunnelTermEntriesPtr = exMxDevPtr->moduleCfg.tunnelCfg.maxIpv4TunnelTermEntries;

    return GT_OK;
}
#endif
