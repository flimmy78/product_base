/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPortStatUT.c
*
* DESCRIPTION:
*       Unit Tests for port statistics and egress counters
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
******************************************************************************/

#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortStat.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>


/* Default valid value for port id */
#define PORT_STAT_VALID_PHY_PORT_CNS    0

/* Invalid enumeration value used for testing */
#define PORT_STAT_INVALID_ENUM_CNS      0x5AAAAAA5

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortMacCounterGet
(
    IN  GT_U8                       dev,
    IN  GT_U8                       port,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortMacCounterGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with cntrName [CPSS_MAC_TRANSMIT_ERR_E] and non-null cntrValuePtr. Expected: GT_OK.
1.1.2. Call with cntrName [CPSS_DROP_EVENTS_E] and non-null cntrValuePtr. Expected: GT_OK.
1.1.3. Call with cntrName [CPSS_BadFC_RCV_E] and non-null cntrValuePtr. Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.
1.1.4. Call with cntrName [CPSS_MAC_TRANSMIT_ERR_E] and cntrValuePtr [NULL]. Expected: GT_BAD_PTR.
1.1.5. Call with bound value for cntrName [CPSS_LAST_MAC_COUNTER_NUM_E] and non-null cntrValuePtr. Expected: NON GT_BAD_PARAM.
1.1.6. Call with cntrName [0x5AAAAAA5] and non-null cntrValuePtr. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    GT_U8                       port;
    CPSS_PORT_MAC_COUNTERS_ENT  cntrName;
    GT_U64                      cntrValue;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            PRV_CPSS_PORT_TYPE_ENT  portType;

            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.1.1. Call with cntrName [CPSS_MAC_TRANSMIT_ERR_E] and non-null cntrValuePtr.   */
            /* Expected: GT_OK.                                                                 */
            cntrName = CPSS_MAC_TRANSMIT_ERR_E;

            st = cpssExMxPortMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);

            /* 1.1.2. Call with cntrName [CPSS_DROP_EVENTS_E] and non-null cntrValuePtr.        */
            /* Expected: GT_OK                                                                  */
            cntrName = CPSS_DROP_EVENTS_E;

            st = cpssExMxPortMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);

            /* 1.1.3. Call with cntrName [CPSS_BadFC_RCV_E] and non-null cntrValuePtr.          */
            /* Expected: GT_OK for 10Gbps port and GT_NOT_SUPPORTED for non 10Gbps port.        */
            cntrName = CPSS_BadFC_RCV_E;

            st = cpssExMxPortMacCounterGet(dev, port, cntrName, &cntrValue);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(cntrValue.l[0], 0, dev, port, cntrName);
                UTF_VERIFY_EQUAL3_PARAM_MAC(cntrValue.l[1], 0, dev, port, cntrName);
            }

            /* 1.1.4. Call with cntrName [CPSS_MAC_TRANSMIT_ERR_E] and cntrValuePtr [NULL].     */
            /* Expected: GT_BAD_PTR.                                                            */
            cntrName = CPSS_MAC_TRANSMIT_ERR_E;

            st = cpssExMxPortMacCounterGet(dev, port, cntrName, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, port, cntrName);

            /* 1.1.5. Call with bound value for cntrName [CPSS_LAST_MAC_COUNTER_NUM_E] and non-null cntrValuePtr.   */
            /* Expected: NON GT_BAD_PARAM.                                                                          */
            cntrName = CPSS_LAST_MAC_COUNTER_NUM_E;

            st = cpssExMxPortMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, cntrName);

            /* 1.1.6. Call with cntrName [0x5AAAAAA5] and non-null cntrValuePtr.                */
            /* Expected: GT_BAD_PARAM.                                                          */
            cntrName = PORT_STAT_INVALID_ENUM_CNS;

            st = cpssExMxPortMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, cntrName);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        cntrName = CPSS_MAC_TRANSMIT_ERR_E;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortMacCounterGet(dev, port, cntrName, &cntrValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_STAT_VALID_PHY_PORT_CNS;
    cntrName = CPSS_MAC_TRANSMIT_ERR_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortMacCounterGet(dev, port, cntrName, &cntrValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortMacCounterGet(dev, port, cntrName, &cntrValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortMacCountersOnPortGet
(
    IN  GT_U8   dev,
    IN  GT_U8   port,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC *portMacCounterSetPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortMacCountersOnPortGet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with non-null portMacCounterSetPtr. Expected: GT_OK.
1.1.2. Call with portMacCounterSetPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                           dev;
    GT_U8                           port;
    CPSS_PORT_MAC_COUNTER_SET_STC   portMacCounterSet;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with non-null portMacCounterSetPtr.  */
            /* Expected: GT_OK.                                 */
            st = cpssExMxPortMacCountersOnPortGet(dev, port, &portMacCounterSet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call with portMacCounterSetPtr [NULL].    */
            /* Expected: GT_BAD_PTR.                            */
            st = cpssExMxPortMacCountersOnPortGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Expected: GT_BAD_PARAM                               */
            st = cpssExMxPortMacCountersOnPortGet(dev, port, &portMacCounterSet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM  */
        /* for out of bound value for port number.                          */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxPortMacCountersOnPortGet(dev, port, &portMacCounterSet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. Go over not active devices    */
    /* Expected: GT_BAD_PARAM           */
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortMacCountersOnPortGet(dev, port, &portMacCounterSet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id    */
    /* Expected: GT_BAD_PARAM                                   */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortMacCountersOnPortGet(dev, port, &portMacCounterSet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortEgressCntrModeSet
(
    IN  GT_U8                          dev,
    IN  GT_U8                          cntrSetNum,
    IN  CPSS_PORT_EGRESS_CNT_MODE_ENT  setModeBmp,
    IN  GT_U8                          port,
    IN  GT_U16                         vlan,
    IN  GT_U8                          tc,
    IN  CPSS_DP_LEVEL_ENT              dpLevel
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortEgressCntrModeSet)
{
/*
ITERATE_DEVICES
1.1. Call with cntrSetNum [1], setModeBmp [CPSS_EGRESS_CNT_PORT_E + CPSS_EGRESS_CNT_VLAN_E + CPSS_EGRESS_CNT_TC_E + CPSS_EGRESS_CNT_DP_E] port [0], vlan [100], tc [0], dpLevel [CPSS_DP_GREEN_E]. Expected: GT_OK.
1.2. Call with cntrSetNum [0], setModeBmp [CPSS_EGRESS_CNT_PORT_E], port [0], vlan [100], tc [0], dpLevel [CPSS_DP_GREEN_E]. Expected: GT_OK.
1.3. Call with cntrSetNum [2], setModeBmp [CPSS_EGRESS_CNT_PORT_E], port [0], vlan [100], tc [0], dpLevel [CPSS_DP_GREEN_E]. Expected: NON GT_OK.
1.4. Call with cntrSetNum [1], setModeBmp [0x5AAAAAA5], port [0], vlan [100], tc [0], dpLevel [CPSS_DP_GREEN_E]. Expected: GT_BAD_PARAM.
1.5. Call with cntrSetNum [1], setModeBmp [CPSS_EGRESS_CNT_PORT_E], port [CPSS_CPU_PORT_NUM_CNS], vlan [100], tc [0], dpLevel [CPSS_DP_GREEN_E]. Expected: GT_OK.
1.6. Call with cntrSetNum [1], setModeBmp [CPSS_EGRESS_CNT_PORT_E], port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], vlan [100], dp [CPSS_DP_GREEN_E]. Expected: GT_BAD_PARAM.
1.7. Call with cntrSetNum [1], setModeBmp [CPSS_EGRESS_CNT_VLAN_E], port [0], vlan [PRV_CPSS_MAX_NUM_VLANS_CNS], tc [0], dpLevel [CPSS_DP_GREEN_E]. Expected: GT_BAD_PARAM.
1.8. Call with cntrSetNum [1], setModeBmp [CPSS_EGRESS_CNT_TC_E], port [0], vlan [100], tc [CPSS_TC_RANGE_CNS], dpLevel [CPSS_DP_GREEN_E]. Expected: GT_BAD_PARAM.
1.9. Call with cntrSetNum [1], setModeBmp [CPSS_EGRESS_CNT_DP_E], port [0], vlan [100], tc [0], dpLevel [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                           dev;
    GT_U8                           cntrSetNum;
    CPSS_PORT_EGRESS_CNT_MODE_ENT   setModeBmp;
    GT_U8                           port;
    GT_U16                          vlan;
    GT_U8                           tc;
    CPSS_DP_LEVEL_ENT               dpLevel;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with cntrSetNum [1],                                                                               */
        /* setModeBmp [CPSS_EGRESS_CNT_PORT_E + CPSS_EGRESS_CNT_VLAN_E + CPSS_EGRESS_CNT_TC_E + CPSS_EGRESS_CNT_DP_E],  */
        /* port [0], vlan [100], tc [0], dpLevel [CPSS_DP_GREEN_E].                                                     */
        /* Expected: GT_OK.                                                                                             */
        cntrSetNum = 1;
        setModeBmp = CPSS_EGRESS_CNT_PORT_E | CPSS_EGRESS_CNT_VLAN_E | CPSS_EGRESS_CNT_TC_E | CPSS_EGRESS_CNT_DP_E;
        port = 0;
        vlan = 100;
        tc = 0;
        dpLevel = CPSS_DP_GREEN_E;

        st = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntrSetNum, setModeBmp);

        /* 1.2. Call with cntrSetNum [0], setModeBmp [CPSS_EGRESS_CNT_PORT_E],              */
        /* port [0], vlan [100], tc [0], dpLevel [CPSS_DP_GREEN_E].                         */
        /* Expected: GT_OK.                                                                 */
        cntrSetNum = 0;
        setModeBmp = CPSS_EGRESS_CNT_PORT_E;
        port = 0;
        vlan = 100;
        tc = 0;
        dpLevel = CPSS_DP_GREEN_E;

        st = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntrSetNum, setModeBmp);

        /* 1.3. Call with cntrSetNum [2], setModeBmp [CPSS_EGRESS_CNT_PORT_E],              */
        /* port [0], vlan [100], tc [0], dpLevel [CPSS_DP_GREEN_E].                         */
        /* Expected: NON GT_OK.                                                             */
        cntrSetNum = 2;
        setModeBmp = CPSS_EGRESS_CNT_PORT_E;
        port = 0;
        vlan = 100;
        tc = 0;
        dpLevel = CPSS_DP_GREEN_E;

        st = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntrSetNum, setModeBmp);

        /* 1.4. Call with cntrSetNum [1], setModeBmp [0x5AAAAAA5],                          */
        /* port [0], vlan [100], tc [0], dpLevel [CPSS_DP_GREEN_E].                         */
        /* Expected: GT_BAD_PARAM.                                                          */
        cntrSetNum = 1;
        setModeBmp = PORT_STAT_INVALID_ENUM_CNS;
        port = 0;
        vlan = 100;
        tc = 0;
        dpLevel = CPSS_DP_GREEN_E;

        st = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetNum, setModeBmp);

        /* 1.5. Call with cntrSetNum [1], setModeBmp [CPSS_EGRESS_CNT_PORT_E],              */
        /* port [CPSS_CPU_PORT_NUM_CNS], vlan [100], tc [0], dpLevel [CPSS_DP_GREEN_E].     */
        /* Expected: GT_OK.                                                                 */
        cntrSetNum = 1;
        setModeBmp = CPSS_EGRESS_CNT_PORT_E;
        port = CPSS_CPU_PORT_NUM_CNS;
        vlan = 100;
        tc = 0;
        dpLevel = CPSS_DP_GREEN_E;

        st = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, cntrSetNum, setModeBmp);

        /* 1.6. Call with cntrSetNum [1], setModeBmp [CPSS_EGRESS_CNT_PORT_E],              */
        /* port [PRV_CPSS_MAX_PP_PORTS_NUM_CNS], vlan [100], dp [CPSS_DP_GREEN_E].          */
        /* Expected: GT_BAD_PARAM.                                                          */
        cntrSetNum = 1;
        setModeBmp = CPSS_EGRESS_CNT_PORT_E;
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        vlan = 100;
        tc = 0;
        dpLevel = CPSS_DP_GREEN_E;

        st = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetNum, setModeBmp);

        /* 1.7. Call with cntrSetNum [1], setModeBmp [CPSS_EGRESS_CNT_VLAN_E],              */
        /* port [0], vlan [PRV_CPSS_MAX_NUM_VLANS_CNS], tc [0], dpLevel [CPSS_DP_GREEN_E].  */
        /* Expected: GT_BAD_PARAM.                                                          */
        cntrSetNum = 1;
        setModeBmp = CPSS_EGRESS_CNT_VLAN_E;
        port = 0;
        vlan = PRV_CPSS_MAX_NUM_VLANS_CNS;
        tc = 0;
        dpLevel = CPSS_DP_GREEN_E;

        st = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetNum, setModeBmp);

        /* 1.8. Call with cntrSetNum [1], setModeBmp [CPSS_EGRESS_CNT_TC_E],                */
        /* port [0], vlan [100], tc [CPSS_TC_RANGE_CNS], dpLevel [CPSS_DP_GREEN_E].         */
        /* Expected: GT_BAD_PARAM.                                                          */
        cntrSetNum = 1;
        setModeBmp = CPSS_EGRESS_CNT_TC_E;
        port = 0;
        vlan = 100;
        tc = CPSS_TC_RANGE_CNS;
        dpLevel = CPSS_DP_GREEN_E;

        st = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetNum, setModeBmp);


        /* 1.9. Call with cntrSetNum [1], setModeBmp [CPSS_EGRESS_CNT_DP_E],                */
        /* port [0], vlan [100], tc [0], dpLevel [0x5AAAAAA5].                              */
        /* Expected: GT_BAD_PARAM.                                                          */
        cntrSetNum = 1;
        setModeBmp = CPSS_EGRESS_CNT_DP_E;
        port = 0;
        vlan = 100;
        tc = 0;
        dpLevel = PORT_STAT_INVALID_ENUM_CNS;

        st = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, cntrSetNum, setModeBmp);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    cntrSetNum = 1;
    setModeBmp = CPSS_EGRESS_CNT_PORT_E;
    port = 0;
    vlan = 100;
    tc = 0;
    dpLevel = CPSS_DP_GREEN_E;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortEgressCntrsGet
(
    IN  GT_U8                          dev,
    IN  GT_U8                          cntrSetNum,
    OUT CPSS_PORT_EGRESS_CNTR_STC      *egrCntrPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxPortEgressCntrsGet)
{
/*
ITERATE_DEVICES
1.1. Call with cntrSetNum [1], non-null egrCntrPtr. Expected: GT_OK.
1.2. Call with cntrSetNum [0], non-null egrCntrPtr. Expected: GT_OK.
1.3. Call with cntrSetNum [2], non-null egrCntrPtr. Expected: NON GT_OK.
1.4. Call with cntrSetNum [1], egrCntrPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                       dev;
    GT_U8                       cntrSetNum;
    CPSS_PORT_EGRESS_CNTR_STC   egrCntr;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with cntrSetNum [1], non-null egrCntrPtr.  */
        /* Expected: GT_OK.                                     */
        cntrSetNum = 1;

        st = cpssExMxPortEgressCntrsGet(dev, cntrSetNum, &egrCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetNum);

        /* 1.2. Call with cntrSetNum [0], non-null egrCntrPtr.  */
        /* Expected: GT_OK.                                     */
        cntrSetNum = 0;

        st = cpssExMxPortEgressCntrsGet(dev, cntrSetNum, &egrCntr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetNum);

        /* 1.3. Call with cntrSetNum [2], non-null egrCntrPtr.  */
        /* Expected: NON GT_OK.                                 */
        cntrSetNum = 2;

        st = cpssExMxPortEgressCntrsGet(dev, cntrSetNum, &egrCntr);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, cntrSetNum);

        /* 1.4. Call with cntrSetNum [1], egrCntrPtr [NULL].    */
        /* Expected: GT_BAD_PTR.                                */
       cntrSetNum = 1;

        st = cpssExMxPortEgressCntrsGet(dev, cntrSetNum, NULL);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, NULL", dev, cntrSetNum);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    cntrSetNum = 1;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxPortEgressCntrsGet(dev, cntrSetNum, &egrCntr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortEgressCntrsGet(dev, cntrSetNum, &egrCntr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxPortStat suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPortStat)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortMacCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortMacCountersOnPortGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortEgressCntrModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortEgressCntrsGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPortStat)


