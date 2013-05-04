/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxSalPortStatUT.c
*
* DESCRIPTION:
*       Unit tests for core port statistics and
*       egress counters facility.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxSal/port/cpssDxSalPortStat.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* Invalid enum */
#define CPSS_UT_INVALID_ENUM_CNS        0x5AAAAAA5

/* Tests use this value as out-of-range value for vlanId */
#define PRV_CPSS_MAX_NUM_VLANS_CNS  4096


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalMacCounterGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalMacCounterGet)
{
/*
ITERATE_DEVICES_PHY_PORTS (DxSal)
1.1.1. Call with cntrName [CPSS_GOOD_OCTETS_RCV_E, CPSS_BAD_OCTETS_RCV_E, CPSS_MAC_TRANSMIT_ERR_E] and non-null cntrValuePtr.
Expected: GT_OK.
1.1.2. Call with out of range ctrlName [0x5AAAAAA5] and non-null cntrValuePtr.
Expected: GT_BAD_PARAM.
1.1.3. Call with cntrName [CPSS_GOOD_OCTETS_RCV_E] and cntrValuePtr [NULL]
Expected: GT_BAD_PTR.
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    CPSS_PORT_MAC_COUNTERS_ENT  cntrName;
    GT_U64                      cntrValue;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with cntrName [CPSS_GOOD_OCTETS_RCV_E,
               CPSS_BAD_OCTETS_RCV_E, CPSS_MAC_TRANSMIT_ERR_E] and
               non-null cntrValuePtr. Expected: GT_OK.  */

            cntrName = CPSS_GOOD_OCTETS_RCV_E;

            st = cpssDxSalMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);

            cntrName = CPSS_BAD_OCTETS_RCV_E;

            st = cpssDxSalMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);

            cntrName = CPSS_MAC_TRANSMIT_ERR_E;

            st = cpssDxSalMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);

            /* 1.1.2. Call with out of range ctrlName [0x5AAAAAA5] and
               non-null cntrValuePtr. Expected: GT_BAD_PARAM.   */

            cntrName = CPSS_UT_INVALID_ENUM_CNS;

            st = cpssDxSalMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                              "%d, cntrName = %d", dev, cntrName);

            cntrName = CPSS_GOOD_OCTETS_RCV_E;

            /* 1.1.3. Call with cntrName [CPSS_GOOD_OCTETS_RCV_E]
               and cntrValuePtr [NULL]. Expected: GT_BAD_PTR.   */

            st = cpssDxSalMacCounterGet(dev, port, cntrName, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                              "%d, cntrValuePtr = NULL", dev);
        }

        cntrName = CPSS_GOOD_OCTETS_RCV_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalMacCounterGet(dev, port, cntrName, &cntrValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalMacCounterGet(dev, port, cntrName, &cntrValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;
    cntrName = CPSS_GOOD_OCTETS_RCV_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalMacCounterGet(dev, port, cntrName, &cntrValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalMacCounterGet(dev, port, cntrName, &cntrValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS   cpssDxSalPortMacCountersOnPortGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArray
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortMacCountersOnPortGet)
{
/*
ITERATE_DEVICES_PHY_PORTS (DxSal)
1.1.1. Call with non-null portMacCounterSetArray. Expected: GT_OK.
1.1.2. Call with portMacCounterSetArray [NULL]. Expected: GT_BAD_PTR
*/
    GT_STATUS   st       = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    CPSS_PORT_MAC_COUNTER_SET_STC   portMacCounterSetArray;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null portMacCounterSetArray.
               Expected: GT_OK. */

            st = cpssDxSalPortMacCountersOnPortGet(dev, port,
                                                   &portMacCounterSetArray);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with portMacCounterSetArray [NULL].
               Expected: GT_BAD_PTR */

            st = cpssDxSalPortMacCountersOnPortGet(dev, port, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, 
                "%d, portMacCounterSetArray = NULL", dev);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalPortMacCountersOnPortGet(dev, port,
                                                   &portMacCounterSetArray);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortMacCountersOnPortGet(dev, port,
                                               &portMacCounterSetArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalPortMacCountersOnPortGet(dev, port,
                                               &portMacCounterSetArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortMacCountersOnPortGet(dev, port,
                                               &portMacCounterSetArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortMacCountersOnPortGet(dev, port,
                                           &portMacCounterSetArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortEgressCntrModeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           cntrSetNum,
    IN  CPSS_PORT_EGRESS_CNT_MODE_ENT   setModeBmp,
    IN  GT_U8                           portNum,
    IN  GT_U16                          vlanId,
    IN  GT_U8                           tc
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortEgressCntrModeSet)
{
/*
ITERATE_DEVICES_PHY_CPU_PORTS (DxSal)
1.1.1. Call with cntrSetNum[0 / 1], setModeBmp [CPSS_EGRESS_CNT_PORT_E / CPSS_EGRESS_CNT_VLAN_E / CPSS_EGRESS_CNT_TC_E], vlanId[100] and tc[0, 1, 2].
Expected: GT_OK.
1.1.2. Call with cntrSetNum[10] (not supported) and other valid parameters from 1.1.1.
Expected: NOT GT_OK.
1.1.3. Call with out of range setModeBmp [0x5AAAAAA5] and other valid parameters from 1.1.1.
Expected: GT_BAD_PARAM.
1.1.4. Call with out of range  vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] = 4096 and other valid parameters from 1.1.1.
Expected: GT_BAD_PARAM.
1.1.5. Call with out of range tc [CPSS_4_TC_RANGE_CNS] = 4 and other valid parameters from 1.1.1.
Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U8                           cntrSetNum;
    CPSS_PORT_EGRESS_CNT_MODE_ENT   setModeBmp;
    GT_U16                          vlanId;
    GT_U8                           tc;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with cntrSetNum[0 / 1], setModeBmp
               [CPSS_EGRESS_CNT_PORT_E / CPSS_EGRESS_CNT_VLAN_E /
               CPSS_EGRESS_CNT_TC_E], vlanId[100] and tc[0, 1, 2].
               Expected: GT_OK. */

            cntrSetNum = 0;
            setModeBmp = CPSS_EGRESS_CNT_PORT_E;
            vlanId = 100;
            tc = 0;

            st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                                port, vlanId, tc);
            UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, cntrSetNum, setModeBmp,
                                                port, vlanId, tc);

            cntrSetNum = 1;
            setModeBmp = CPSS_EGRESS_CNT_VLAN_E;
            vlanId = 100;
            tc = 1;

            st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                                port, vlanId, tc);
            UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, cntrSetNum, setModeBmp,
                                                port, vlanId, tc);

            cntrSetNum = 0;
            setModeBmp = CPSS_EGRESS_CNT_TC_E;
            vlanId = 100;
            tc = 2;

            st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                                port, vlanId, tc);
            UTF_VERIFY_EQUAL6_PARAM_MAC(GT_OK, st, dev, cntrSetNum, setModeBmp,
                                                port, vlanId, tc);

            /* 1.1.2. Call with cntrSetNum[10] (not supported) and other
               valid parameters from 1.1.1. Expected: NOT GT_OK. */

            cntrSetNum = 10;
            setModeBmp = CPSS_EGRESS_CNT_PORT_E;
            vlanId = 100;
            tc = 0;

            st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                                port, vlanId, tc);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, cntrSetNum = %d",
                                             dev, cntrSetNum);
            cntrSetNum = 0;

            /* 1.1.3. Call with out of range setModeBmp [0x5AAAAAA5] and
               other valid parameters from 1.1.1. Expected: GT_BAD_PARAM.*/

            setModeBmp = CPSS_UT_INVALID_ENUM_CNS;

            st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                                port, vlanId, tc);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, setModeBmp = %d",
                                         dev, setModeBmp);
            setModeBmp = CPSS_EGRESS_CNT_PORT_E;

            /* 1.1.4. Call with out of range  vlanId
               [PRV_CPSS_MAX_NUM_VLANS_CNS] = 4096 and other valid
               parameters from 1.1.1. Expected: GT_BAD_PARAM.   */

            vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                                port, vlanId, tc);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, vlanId = %d",
                                         dev, vlanId);
            vlanId = 100;

            /* 1.1.5. Call with out of range tc [CPSS_4_TC_RANGE_CNS] = 4
               and other valid parameters from 1.1.1. Expected: GT_BAD_PARAM.*/

            tc = CPSS_4_TC_RANGE_CNS;

            st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                                port, vlanId, tc);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, tc = %d",
                                         dev, tc);
            tc = 0;
        }

        /* 1.2. Check that function handles CPU port    */
        port = CPSS_CPU_PORT_NUM_CNS;
        cntrSetNum = 0;
        setModeBmp = CPSS_EGRESS_CNT_PORT_E;
        vlanId = 100;
        tc = 0;

        /* 1.2.1. <Check other parameters for each active device id and CPU port>. */
        st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                            port, vlanId, tc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.3. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                                port, vlanId, tc);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                            port, vlanId, tc);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = 0;
    cntrSetNum = 0;
    setModeBmp = CPSS_EGRESS_CNT_PORT_E;
    vlanId = 100;
    tc = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                            port, vlanId, tc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp,
                                        port, vlanId, tc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalPortEgressCntrsGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       cntrSetNum,
    OUT CPSS_PORT_EGRESS_CNTR_STC   *egrCntrPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalPortEgressCntrsGet)
{
/*
ITERATE_DEVICES (DxSal)
1.1. Call with cntrSetNum [0 / 1] and non-null egrCntrPtr. Expected: GT_OK.
1.2. Call with cntrSetNum [100] and non-null egrCntrPtr. Expected: NON GT_OK.
1.3. Call with with cntrSetNum [0] and egrCntrPtr [NULL]. Expected: GT_BAD_PTR
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U8                       cntrSetNum;
    CPSS_PORT_EGRESS_CNTR_STC   egrCntr;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with cntrSetNum [0 / 1] and non-null egrCntrPtr.
           Expected: GT_OK. */

        cntrSetNum = 0;

        st = cpssDxSalPortEgressCntrsGet(dev, cntrSetNum, &egrCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetNum);

        cntrSetNum = 1;

        st = cpssDxSalPortEgressCntrsGet(dev, cntrSetNum, &egrCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetNum);

        /* 1.2. Call with cntrSetNum [100] and
           non-null egrCntrPtr. Expected: NON GT_OK.    */

        cntrSetNum = 100;

        st = cpssDxSalPortEgressCntrsGet(dev, cntrSetNum, &egrCntr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetNum);

        /* 1.3. Call with with cntrSetNum [0] and egrCntrPtr
           [NULL]. Expected: GT_BAD_PTR */

        cntrSetNum = 0;

        st = cpssDxSalPortEgressCntrsGet(dev, cntrSetNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, egrCntrPtr = NULL", dev);
    }

    cntrSetNum = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalPortEgressCntrsGet(dev, cntrSetNum, &egrCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalPortEgressCntrsGet(dev, cntrSetNum, &egrCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssDxSalPortStat suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalPortStat)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalMacCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortMacCountersOnPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortEgressCntrModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalPortEgressCntrsGet)
UTF_SUIT_END_TESTS_MAC(cpssDxSalPortStat)
