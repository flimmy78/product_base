/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxBrgCountUT.c
*
* DESCRIPTION:
*       Unit test for cpssExMxBrgCount.c that provides Ingress Bridge Counters
*       facility cpss Ex/Mx implementation.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgCount.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum                     */
#define BRG_CNT_INVALID_ENUM_CNS             0x5AAAAAA5

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgCntMacDaSaSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *saAddrPtr,
    IN  GT_ETHERADDR    *daAddrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgCntMacDaSaSet)
{
/*
ITERATE_DEVICES
1.1. Call function with non-NULL saAddrPtr [AB:CD:EF:00:00:01]
    and daAddrPtr [AB:CD:EF:00:00:02].  Expected: GT_OK.
1.2. Call function with non-NULL daAddrPtr [AB:CD:EF:00:00:02] 
    and saAddrPtr [NULL]. Expected: GT_BAD_PTR.
1.3. Call function with non-NULL saAddrPtr [AB:CD:EF:00:00:01] 
    and daAddrPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8           dev;
    GT_ETHERADDR    saAddr = {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x01}};
    GT_ETHERADDR    daAddr = {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x02}};


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL saAddrPtr [AB:CD:EF:00:00:01]   */
        /* and daAddrPtr [AB:CD:EF:00:00:02].  Expected: GT_OK.             */
        st = cpssExMxBrgCntMacDaSaSet(dev, &saAddr, &daAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with non-NULL daAddrPtr [AB:CD:EF:00:00:02]   */
        /* and saAddrPtr [NULL]. Expected: GT_BAD_PTR.                      */
        st = cpssExMxBrgCntMacDaSaSet(dev, NULL, &daAddr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);

        /* 1.3. Call function with non-NULL saAddrPtr [AB:CD:EF:00:00:01]   */
        /* and daAddrPtr [NULL]. Expected: GT_BAD_PTR.                      */
        st = cpssExMxBrgCntMacDaSaSet(dev, NULL, &daAddr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, <saAddrPtr>, NULL", dev);
    }

    /* saAddr == {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x01}} */
    /* daAddr == {{0xAB, 0xCD, 0xEF, 0x00, 0x00, 0x02}} */

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgCntMacDaSaSet(dev, &saAddr, &daAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgCntMacDaSaSet(dev, &saAddr, &daAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgCntHostGroupCntrsGet
(
    IN   GT_U8                              devNum,
    OUT  CPSS_EXMX_BRIDGE_HOST_CNTR_STC     *hostGroupPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgCntHostGroupCntrsGet)
{
/*
ITERATE_DEVICES
1.1. Call function with non-NULL hostGroupPtr. Expected: GT_OK.
1.2. Call function with hostGroupPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                               dev;
    CPSS_EXMX_BRIDGE_HOST_CNTR_STC      hostGroup;

    /* zero out the structure */
    cpssOsBzero((GT_VOID*)&hostGroup, sizeof(hostGroup));

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL hostGroupPtr. Expected: GT_OK. */
        st = cpssExMxBrgCntHostGroupCntrsGet(dev, &hostGroup);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with hostGroupPtr [NULL]. Expected: GT_BAD_PTR.*/
        st = cpssExMxBrgCntHostGroupCntrsGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgCntHostGroupCntrsGet(dev, &hostGroup);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgCntHostGroupCntrsGet(dev, &hostGroup);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/* 
GT_STATUS cpssExMxBrgCntMatrixGroupCntrsGet
(
    IN   GT_U8                                  devNum,
    OUT  CPSS_EXMX_BRIDGE_MATRIX_CNTR_STC       *matrixGroupPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgCntMatrixGroupCntrsGet)
{
/*
ITERATE_DEVICES
1.1. Call function with non-NULL matrixGroupPtr. Expected: GT_OK.
1.2. Call function with matrixGroupPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                               dev;
    CPSS_EXMX_BRIDGE_MATRIX_CNTR_STC    matrixGroup;

    /* zero out the structure */
    cpssOsBzero((GT_VOID*)&matrixGroup, sizeof(matrixGroup));

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL matrixGroupPtr. Expected: GT_OK. */
        st = cpssExMxBrgCntMatrixGroupCntrsGet(dev, &matrixGroup);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with matrixGroupPtr [NULL]. Expected: GT_BAD_PTR.*/
        st = cpssExMxBrgCntMatrixGroupCntrsGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgCntMatrixGroupCntrsGet(dev, &matrixGroup);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgCntMatrixGroupCntrsGet(dev, &matrixGroup);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgCntBridgeIngressCntrModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_EXMX_BRIDGE_CNTR_SET_ID_ENT        cntrSetId,
    IN  CPSS_EXMX_BRIDGE_INGR_CNTR_MODES_ENT    setMode,
    IN  GT_U8                                   port,
    IN  GT_U16                                  vlan
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgCntBridgeIngressCntrModeSet)
{
/*
ITERATE_DEVICES
1.1. Call function with cntrSetId [CPSS_EXMX_BRG_CNT_SET_ID_2_E], setMode [CPSS_BRG_CNT_MODE_3_E], vlan [100], port [0]. Expected: GT_OK.
1.2. Call function with cntrSetId [CPSS_EXMX_BRG_CNT_SET_ID_1_E], setMode [CPSS_BRG_CNT_MODE_0_E], vlan [100], port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (port value is important only for 0 and 1 modes). Expected: GT_OK.
1.3. Call function with cntrSetId [CPSS_EXMX_BRG_CNT_SET_ID_0_E], setMode [CPSS_BRG_CNT_MODE_2_E], vlan [PRV_CPSS_MAX_NUM_VLANS_CNS] (vlan value is important only for 1 and 3 modes), port [0]. Expected: GT_OK.
1.4. Call function with out of range cntrSetId [0x5AAAAAA5], other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.5. Call function with out of range setMode [0x5AAAAAA5], other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.6. Call function with out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS], other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.7. Call function with out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                               dev;
    CPSS_EXMX_BRIDGE_CNTR_SET_ID_ENT    cntrSetId;
    CPSS_BRIDGE_INGR_CNTR_MODES_ENT     setMode;
    GT_U8                               port;
    GT_U16                              vlan;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with cntrSetId [CPSS_EXMX_BRG_CNT_SET_ID_2_E],
        setMode [CPSS_BRG_CNT_MODE_3_E], vlan [100], port [0].
        Expected: GT_OK. */
        cntrSetId = CPSS_EXMX_BRG_CNT_SET_ID_2_E;
        setMode = CPSS_BRG_CNT_MODE_3_E;
        vlan = 100;
        port = 0;

        st = cpssExMxBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, 
                                    dev, cntrSetId, setMode, port, vlan);

        /* 1.2. Call function with cntrSetId [CPSS_EXMX_BRG_CNT_SET_ID_1_E],
        setMode [CPSS_BRG_CNT_MODE_0_E], vlan [100],
        port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (port value is important only for 1 and 3 modes).
        Expected: GT_OK. */
        cntrSetId = CPSS_EXMX_BRG_CNT_SET_ID_1_E;
        setMode = CPSS_BRG_CNT_MODE_0_E;
        vlan = 100;
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, 
                                    dev, cntrSetId, setMode, port, vlan);

        /* 1.3. Call function with cntrSetId [CPSS_EXMX_BRG_CNT_SET_ID_0_E],
        setMode [CPSS_BRG_CNT_MODE_2_E], vlan [PRV_CPSS_MAX_NUM_VLANS_CNS]
        (vlan value is important only for 0 and 1 modes), port [0]. Expected: GT_OK. */
        cntrSetId = CPSS_EXMX_BRG_CNT_SET_ID_0_E;
        setMode = CPSS_BRG_CNT_MODE_1_E;
        vlan = PRV_CPSS_MAX_NUM_VLANS_CNS;
        port = 0;

        st = cpssExMxBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, 
                                    dev, cntrSetId, setMode, port, vlan);

        /* 1.4. Call function with out of range cntrSetId [0x5AAAAAA5],
        other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */
        cntrSetId = BRG_CNT_INVALID_ENUM_CNS;
        setMode = CPSS_BRG_CNT_MODE_3_E;
        vlan = 100;
        port = 0;

        st = cpssExMxBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, 
                                    dev, cntrSetId, setMode, port, vlan);
        /* restore */
        cntrSetId = CPSS_EXMX_BRG_CNT_SET_ID_2_E;

        /* 1.5. Call function with out of range setMode [0x5AAAAAA5],
        other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */
        setMode = BRG_CNT_INVALID_ENUM_CNS;

        st = cpssExMxBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, 
                                    dev, cntrSetId, setMode, port, vlan);
        /* restore */
        setMode = CPSS_BRG_CNT_MODE_3_E;

        /* 1.6. Call function with out of range vlan [PRV_CPSS_MAX_NUM_VLANS_CNS],
        other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */
        vlan = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st, 
                                    dev, cntrSetId, setMode, port, vlan);
        /* restore */
        vlan = 100;

        /* 1.7. Call function with out of range port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
        other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OUT_OF_RANGE, st, 
                                    dev, cntrSetId, setMode, port, vlan);
        /* restore */
        port = 0;
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    cntrSetId = CPSS_EXMX_BRG_CNT_SET_ID_2_E;
    setMode = CPSS_BRG_CNT_MODE_3_E;
    vlan = 100;
    port = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgCntBridgeIngressCntrModeSet(dev, cntrSetId, setMode, port, vlan);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgCntBridgeIngressCntrsGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_EXMX_BRIDGE_CNTR_SET_ID_ENT    cntrSetId,
    OUT CPSS_EXMX_BRIDGE_INGRESS_CNTR_STC   *ingrCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgCntBridgeIngressCntrsGet)
{
/*
ITERATE_DEVICES
1.1. Call function with cntrSetId [CPSS_EXMX_BRG_CNT_SET_ID_1_E]
    and non-NULL ingCntrPtr. Expected: GT_OK.
1.2. Call function with out of range cntrSetId [0x5AAAAAA5]. 
    Expected: GT_BAD_PARAM.
1.3. Call function with in range cntrSetId [CPSS_EXMX_BRG_CNT_SET_ID_0_E] 
    and ingrCntrPtr [NULL].  Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                               dev;
    CPSS_EXMX_BRIDGE_CNTR_SET_ID_ENT    cntrSetId;
    CPSS_BRIDGE_INGRESS_CNTR_STC        ingrCntr;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with cntrSetId [CPSS_EXMX_BRG_CNT_SET_ID_1_E] */
        /* and non-NULL ingrCntrPtr. Expected: GT_OK.                        */
        cntrSetId = CPSS_EXMX_BRG_CNT_SET_ID_1_E;

        st = cpssExMxBrgCntBridgeIngressCntrsGet(dev, cntrSetId, &ingrCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetId);

        /* 1.2. Call function with out of range cntrSetId [0x5AAAAAA5]. */
        /* Expected: GT_BAD_PARAM.                                      */
        cntrSetId = BRG_CNT_INVALID_ENUM_CNS;

        st = cpssExMxBrgCntBridgeIngressCntrsGet(dev, cntrSetId, &ingrCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetId);

        /* 1.3. Call function with in range cntrSetId               */
        /* [CPSS_EXMX_BRG_CNT_SET_ID_0_E] and ingrCntrPtr [NULL].   */
        /* Expected: GT_BAD_PTR.                                    */
        cntrSetId = CPSS_EXMX_BRG_CNT_SET_ID_0_E;

        st = cpssExMxBrgCntBridgeIngressCntrsGet(dev, cntrSetId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, cntrSetId);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    cntrSetId = CPSS_EXMX_BRG_CNT_SET_ID_1_E;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgCntBridgeIngressCntrsGet(dev, cntrSetId, &ingrCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgCntBridgeIngressCntrsGet(dev, cntrSetId, &ingrCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgCntLearnedEntryDiscGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *valPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgCntLearnedEntryDiscGet)
{
/*
ITERATE_DEVICES
1.1. Call function with non-NULL valPtr. Expected: GT_OK.
1.2. Call function with valPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8           dev;
    GT_U32          val;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL valPtr. Expected: GT_OK. */
        st = cpssExMxBrgCntLearnedEntryDiscGet(dev, &val);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with valPtr [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxBrgCntLearnedEntryDiscGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, NULL",
                                     dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgCntLearnedEntryDiscGet(dev, &val);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgCntLearnedEntryDiscGet(dev, &val);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cppExMxBrgCount suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxBrgCount)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgCntMacDaSaSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgCntHostGroupCntrsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgCntMatrixGroupCntrsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgCntBridgeIngressCntrModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgCntBridgeIngressCntrsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgCntLearnedEntryDiscGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxBrgCount)
