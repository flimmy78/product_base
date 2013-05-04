/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmPortStatUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmPortStat, that provides
*       ExMxPm CPSS implementation for core port statistics and
*       egress counters facility.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortStat.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define PORT_STAT_VALID_PHY_PORT_CNS  0

/* Invalid enum */
#define PORT_STAT_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortSpecificMacCounterGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_MAC_COUNTERS_ENT  cntrName,
    OUT GT_U64                      *cntrValuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortSpecificMacCounterGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with ctrlName [CPSS_GOOD_OCTETS_RCV_E /
                               CPSS_BAD_OCTETS_RCV_E /
                               CPSS_MAC_TRANSMIT_ERR_E]
                     and non-null cntrValuePtr.
    Expected: GT_OK.
    1.1.2. Call with ctrlName [CPSS_BadFC_RCV_E] (not relevant)
                     and non-null cntrValuePtr.
    Expected: NOT GT_OK.
    1.1.3. Call with ctrlName [CPSS_EXCESSIVE_COLLISIONS_E /
                               CPSS_LATE_COLLISIONS_E /
                               CPSS_COLLISIONS_E /
                               CPSS_MULTIPLE_PKTS_SENT_E /
                               CPSS_DEFERRED_PKTS_SENT_E] (not support for XG ports)
                     and non-null cntrValuePtr.
    Expected: GT_OK for non-XG ports and NOT GT_OK for XG ports.
    1.1.4. Call with out of range ctrlName [0x5AAAAAA5]
                     and non-null cntrValuePtr.
    Expected: GT_BAD_PARAM.
    1.1.5. Call with ctrlName [CPSS_BAD_PKTS_RCV_E,
                               CPSS_UNRECOG_MAC_CNTR_RCV_E, 
                               CPSS_BadFC_RCV_E, 
                               CPSS_GOOD_PKTS_RCV_E, 
                               CPSS_GOOD_PKTS_SENT_E]
                     and non-null cntrValuePtr.
    Expected: NOT GT_OK.
    1.1.6. Call with ctrlName [CPSS_GOOD_OCTETS_RCV_E]
                     and cntrValuePtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    CPSS_PORT_MAC_COUNTERS_ENT  cntrName  = CPSS_GOOD_OCTETS_RCV_E;
    PRV_CPSS_PORT_TYPE_ENT      portType  = PRV_CPSS_PORT_NOT_EXISTS_E;
    GT_U64                      cntrValue;

    cpssOsBzero((GT_VOID*) &cntrValue, sizeof(cntrValue));


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* Getting port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "prvUtfPortTypeGet: %d, %d", dev, port);

            /* 
                1.1.1. Call with ctrlName [CPSS_GOOD_OCTETS_RCV_E /
                                           CPSS_BAD_OCTETS_RCV_E /
                                           CPSS_MAC_TRANSMIT_ERR_E]
                                 and non-null cntrValuePtr.
                Expected: GT_OK.
            */

            /* Call with ctrlName [CPSS_GOOD_OCTETS_RCV_E] */
            cntrName = CPSS_GOOD_OCTETS_RCV_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            if(port == 27)
            {
                /* MIB counters are not supported for port 27 */
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }

            /* Call with ctrlName [CPSS_BAD_OCTETS_RCV_E] */
            cntrName = CPSS_BAD_OCTETS_RCV_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            if(port == 27)
            {
                /* MIB counters are not supported for port 27 */
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }

            /* Call with ctrlName [CPSS_MAC_TRANSMIT_ERR_E] */
            cntrName = CPSS_MAC_TRANSMIT_ERR_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            if(port == 27)
            {
                /* MIB counters are not supported for port 27 */
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }

            /*
                1.1.2. Call with ctrlName [CPSS_BadFC_RCV_E] (not relevant)
                                 and non-null cntrValuePtr.
                Expected: NOT GT_OK.
            */
            cntrName = CPSS_BadFC_RCV_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);

            cntrName = CPSS_GOOD_OCTETS_RCV_E;

            /*
                1.1.3. Call with ctrlName [CPSS_EXCESSIVE_COLLISIONS_E /
                                           CPSS_LATE_COLLISIONS_E /
                                           CPSS_COLLISIONS_E /
                                           CPSS_MULTIPLE_PKTS_SENT_E /
                                           CPSS_DEFERRED_PKTS_SENT_E] (not support for XG ports)
                                 and non-null cntrValuePtr.
                Expected: GT_OK for non-XG ports and NOT GT_OK for XG ports.
            */

            /* Call with ctrlName [CPSS_EXCESSIVE_COLLISIONS_E] */
            cntrName = CPSS_EXCESSIVE_COLLISIONS_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            if((PRV_CPSS_PORT_XG_E == portType)||(port == 27))
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }

            /* Call with ctrlName [CPSS_LATE_COLLISIONS_E] */
            cntrName = CPSS_LATE_COLLISIONS_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            if((PRV_CPSS_PORT_XG_E == portType)||(port == 27))
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }

            /* Call with ctrlName [CPSS_COLLISIONS_E] */
            cntrName = CPSS_COLLISIONS_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            if((PRV_CPSS_PORT_XG_E == portType)||(port == 27))
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }

            /* Call with ctrlName [CPSS_MULTIPLE_PKTS_SENT_E] */
            cntrName = CPSS_MULTIPLE_PKTS_SENT_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            if((PRV_CPSS_PORT_XG_E == portType)||(port == 27))
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }

            /* Call with ctrlName [CPSS_DEFERRED_PKTS_SENT_E] */
            cntrName = CPSS_DEFERRED_PKTS_SENT_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            if((PRV_CPSS_PORT_XG_E == portType)||(port == 27))
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);
            }

            /*
                1.1.4. Call with out of range ctrlName [0x5AAAAAA5]
                                 and non-null cntrValuePtr.
                Expected: GT_BAD_PARAM.
            */
            cntrName = PORT_STAT_INVALID_ENUM_CNS;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, cntrName);

            cntrName = CPSS_GOOD_OCTETS_RCV_E;

            /*
                1.1.5. Call with ctrlName [CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E, CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E]
                                 and non-null cntrValuePtr.
                Expected: NOT GT_OK.
            */
            cntrName = CPSS_BAD_PKTS_RCV_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);

            cntrName = CPSS_UNRECOG_MAC_CNTR_RCV_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);

            cntrName = CPSS_BadFC_RCV_E;            

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);

            cntrName = CPSS_GOOD_PKTS_RCV_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);

            cntrName = CPSS_GOOD_PKTS_SENT_E;

            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, cntrName);

            /*
                1.1.6. Call with ctrlName [CPSS_GOOD_OCTETS_RCV_E]
                                 and cntrValuePtr [NULL]
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, cntrValuePtr = NULL", dev, port);
        }

        cntrName = CPSS_GOOD_OCTETS_RCV_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

#ifdef PUMA_2
        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
#endif
    }

    cntrName = CPSS_GOOD_OCTETS_RCV_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortSpecificMacCounterGet(dev, port, cntrName, &cntrValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS   cpssExMxPmPortAllMacCountersGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArray
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortAllMacCountersGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null portMacCounterSetArray. 
    Expected: GT_OK (not for port 27).
    1.1.2. Call with portMacCounterSetArray [NULL]. 
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    CPSS_PORT_MAC_COUNTER_SET_STC  portMacCounterSetArray;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with non-null portMacCounterSetArray. 
                Expected: GT_OK.
            */
            st = cpssExMxPmPortAllMacCountersGet(dev, port, &portMacCounterSetArray);
            if(port != 27)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                /* MIB counters are  not supported for port 27 */
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /*
                1.1.2. Call with portMacCounterSetArray [NULL]. 
                Expected: GT_BAD_PTR
            */
            st = cpssExMxPmPortAllMacCountersGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, portMacCounterSetArray = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortAllMacCountersGet(dev, port, &portMacCounterSetArray);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortAllMacCountersGet(dev, port, &portMacCounterSetArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

#ifdef PUMA_2
        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortAllMacCountersGet(dev, port, &portMacCounterSetArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
#endif
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortAllMacCountersGet(dev, port, &portMacCounterSetArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortAllMacCountersGet(dev, port, &portMacCounterSetArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacCountersEnableSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacCountersEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortMacCountersEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPortMacCountersEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortMacCountersEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmPortMacCountersEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmPortMacCountersEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPortMacCountersEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPortMacCountersEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same value as written.
            */
            st = cpssExMxPmPortMacCountersEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmPortMacCountersEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacCountersEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacCountersEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
#ifdef PUMA_1
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacCountersEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
#endif
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacCountersEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortMacCountersEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacCountersEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacCountersEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with null enablePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortMacCountersEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortMacCountersEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacCountersEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacCountersEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
#ifdef PUMA_1
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacCountersEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
#endif
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacCountersEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortMacCountersEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacCountersClearOnReadEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacCountersClearOnReadEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortMacCountersClearOnReadEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            if(port != 27) /* not supported */
            {
                /* 
                    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                    Expected: GT_OK.
                */
    
                /* Call with enable [GT_FALSE] */
                enable = GT_FALSE;
    
                st = cpssExMxPmPortMacCountersClearOnReadEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
    
                /*
                    1.1.2. Call cpssExMxPmPortMacCountersClearOnReadEnableGet with non-NULL enablePtr.
                    Expected: GT_OK and the same value as written.
                */
                st = cpssExMxPmPortMacCountersClearOnReadEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPortMacCountersClearOnReadEnableGet: %d, %d", dev, port);
    
                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
                
                /* Call with enable [GT_TRUE] */
                enable = GT_TRUE;
    
                st = cpssExMxPmPortMacCountersClearOnReadEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
    
                /*
                    1.1.2. Call cpssExMxPmPortMacCountersClearOnReadEnableGet with non-NULL enablePtr.
                    Expected: GT_OK and the same value as written.
                */
                st = cpssExMxPmPortMacCountersClearOnReadEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPortMacCountersClearOnReadEnableGet: %d, %d", dev, port);
    
                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacCountersClearOnReadEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacCountersClearOnReadEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacCountersClearOnReadEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacCountersClearOnReadEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortMacCountersClearOnReadEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacCountersClearOnReadEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacCountersClearOnReadEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with null enablePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortMacCountersClearOnReadEnableGet(dev, port, &enable);
            if(port == 27)
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortMacCountersClearOnReadEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacCountersClearOnReadEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacCountersClearOnReadEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacCountersClearOnReadEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacCountersClearOnReadEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortMacCountersClearOnReadEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacCountersRxHistogramEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacCountersRxHistogramEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortMacCountersRxHistogramEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            if(port != 27)
            {
                /* 
                    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                    Expected: GT_OK.
                */
    
                /* Call with enable [GT_FALSE] */
                enable = GT_FALSE;
    
                st = cpssExMxPmPortMacCountersRxHistogramEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
    
                /*
                    1.1.2. Call cpssExMxPmPortMacCountersRxHistogramEnableGet with non-NULL enablePtr.
                    Expected: GT_OK and the same value as written.
                */
                st = cpssExMxPmPortMacCountersRxHistogramEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPortMacCountersRxHistogramEnableGet: %d, %d", dev, port);
    
                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
                
                /* Call with enable [GT_TRUE] */
                enable = GT_TRUE;
    
                st = cpssExMxPmPortMacCountersRxHistogramEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
    
                /*
                    1.1.2. Call cpssExMxPmPortMacCountersRxHistogramEnableGet with non-NULL enablePtr.
                    Expected: GT_OK and the same value as written.
                */
                st = cpssExMxPmPortMacCountersRxHistogramEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPortMacCountersRxHistogramEnableGet: %d, %d", dev, port);
    
                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacCountersRxHistogramEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacCountersRxHistogramEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacCountersRxHistogramEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacCountersRxHistogramEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortMacCountersRxHistogramEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacCountersRxHistogramEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacCountersRxHistogramEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with null enablePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortMacCountersRxHistogramEnableGet(dev, port, &enable);
            if(port == 27)
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortMacCountersRxHistogramEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacCountersRxHistogramEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacCountersRxHistogramEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacCountersRxHistogramEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacCountersRxHistogramEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortMacCountersRxHistogramEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacCountersTxHistogramEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacCountersTxHistogramEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call function with enable [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortMacCountersTxHistogramEnableGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            if(port != 27)
            {
                /* 
                    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                    Expected: GT_OK.
                */
    
                /* Call with enable [GT_FALSE] */
                enable = GT_FALSE;
    
                st = cpssExMxPmPortMacCountersTxHistogramEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
    
                /*
                    1.1.2. Call cpssExMxPmPortMacCountersTxHistogramEnableGet with non-NULL enablePtr.
                    Expected: GT_OK and the same value as written.
                */
                st = cpssExMxPmPortMacCountersTxHistogramEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPortMacCountersTxHistogramEnableGet: %d, %d", dev, port);
    
                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
                
                /* Call with enable [GT_TRUE] */
                enable = GT_TRUE;
    
                st = cpssExMxPmPortMacCountersTxHistogramEnableSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
    
                /*
                    1.1.2. Call cpssExMxPmPortMacCountersTxHistogramEnableGet with non-NULL enablePtr.
                    Expected: GT_OK and the same value as written.
                */
                st = cpssExMxPmPortMacCountersTxHistogramEnableGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPortMacCountersTxHistogramEnableGet: %d, %d", dev, port);
    
                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacCountersTxHistogramEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacCountersTxHistogramEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacCountersTxHistogramEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacCountersTxHistogramEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortMacCountersTxHistogramEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacCountersTxHistogramEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacCountersTxHistogramEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with null enablePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortMacCountersTxHistogramEnableGet(dev, port, &enable);
            if(port == 27)
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortMacCountersTxHistogramEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacCountersTxHistogramEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacCountersTxHistogramEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacCountersTxHistogramEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacCountersTxHistogramEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortMacCountersTxHistogramEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacCountersCaptureTriggerSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  captureTrigger
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacCountersCaptureTriggerSet)
{
/*
    ITERATE_DEVICES_PORTS (ExMxPm)
    1.1.1. Call function with captureTrigger [GT_FALSE / GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPortMacCountersCaptureTriggerGet
    Expected: GT_OK and the same enablePtr.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    GT_BOOL   enable    = GT_FALSE;
    GT_BOOL   enableGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            if(port != 27)
            {
                /* 
                    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                    Expected: GT_OK.
                */
    
                /* Call with enable [GT_FALSE] */
                enable = GT_FALSE;
    
                st = cpssExMxPmPortMacCountersCaptureTriggerSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
    
                /*
                    1.1.2. Call cpssExMxPmPortMacCountersCaptureTriggerGet with non-NULL enablePtr.
                    Expected: GT_OK and the same value as written.
                */
                st = cpssExMxPmPortMacCountersCaptureTriggerGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPortMacCountersCaptureTriggerGet: %d, %d", dev, port);
    
                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
                
                /* Call with enable [GT_TRUE] */
                enable = GT_TRUE;
    
                st = cpssExMxPmPortMacCountersCaptureTriggerSet(dev, port, enable);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
    
                /*
                    1.1.2. Call cpssExMxPmPortMacCountersCaptureTriggerGet with non-NULL enablePtr.
                    Expected: GT_OK and the same value as written.
                */
                st = cpssExMxPmPortMacCountersCaptureTriggerGet(dev, port, &enableGet);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPortMacCountersCaptureTriggerGet: %d, %d", dev, port);
    
                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                           "get another enable than was set: %d, %d", dev, port);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacCountersCaptureTriggerSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacCountersCaptureTriggerSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacCountersCaptureTriggerSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacCountersCaptureTriggerSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortMacCountersCaptureTriggerSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPortMacCountersCaptureTriggerGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortMacCountersCaptureTriggerGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr.
    Expected: GT_OK.
    1.1.2. Call with null enablePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    GT_BOOL   enable = GT_FALSE;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmPortMacCountersCaptureTriggerGet(dev, port, &enable);
            if(port == 27)
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortMacCountersCaptureTriggerGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortMacCountersCaptureTriggerGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortMacCountersCaptureTriggerGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortMacCountersCaptureTriggerGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortMacCountersCaptureTriggerGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortMacCountersCaptureTriggerGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS   cpssExMxPmPortCapturedMacCountersGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArray
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPortCapturedMacCountersGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null portMacCounterSetArray. 
    Expected: GT_OK.
    1.1.2. Call with portMacCounterSetArray [NULL]. 
    Expected: GT_BAD_PTR
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PORT_STAT_VALID_PHY_PORT_CNS;

    CPSS_PORT_MAC_COUNTER_SET_STC   portMacCounterSetArray;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with non-null portMacCounterSetArray. 
                Expected: GT_OK.
            */
            st = cpssExMxPmPortCapturedMacCountersGet(dev, port, &portMacCounterSetArray);
            if(port == 27)
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /*
                1.1.2. Call with portMacCounterSetArray [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPortCapturedMacCountersGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, portMacCounterSetArray = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPortCapturedMacCountersGet(dev, port, &portMacCounterSetArray);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPortCapturedMacCountersGet(dev, port, &portMacCounterSetArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

#ifdef PUMA_2
        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPortCapturedMacCountersGet(dev, port, &portMacCounterSetArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
#endif
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PORT_STAT_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPortCapturedMacCountersGet(dev, port, &portMacCounterSetArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPortCapturedMacCountersGet(dev, port, &portMacCounterSetArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmPortStat suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmPortStat)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortSpecificMacCounterGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortAllMacCountersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacCountersEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacCountersEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacCountersClearOnReadEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacCountersClearOnReadEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacCountersRxHistogramEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacCountersRxHistogramEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacCountersTxHistogramEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacCountersTxHistogramEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacCountersCaptureTriggerSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortMacCountersCaptureTriggerGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPortCapturedMacCountersGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmPortStat)

