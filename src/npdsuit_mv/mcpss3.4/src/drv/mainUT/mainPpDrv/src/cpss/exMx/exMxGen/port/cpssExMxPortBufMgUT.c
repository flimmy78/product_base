/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPortBufMgUT.c
*
* DESCRIPTION:
*       Unit Tests for configuring Port Buffer manager facility
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortBufMg.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define PORT_BUFMG_VALID_PHY_PORT_CNS   0

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortXonLimitSet
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_U16   xonLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPortXonLimitSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with xonLimit [1024]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U16  xonLimit;

    xonLimit = 1024;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with xonLimit [1024]. Expected: GT_OK. */
            st = cpssExMxPortXonLimitSet(dev, port, xonLimit);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, xonLimit);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each invalid port */
            st = cpssExMxPortXonLimitSet(dev, port, xonLimit);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, xonLimit);
        }

        /* 1.3. For active device check that function returns non GT_OK */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        /* xonLimit == 1024 */

        st = cpssExMxPortXonLimitSet(dev, port, xonLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, xonLimit);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_BUFMG_VALID_PHY_PORT_CNS;
    xonLimit = 104;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortXonLimitSet(dev, port, xonLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, xonLimit);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == PRV_PORT_TESTED_CNS  */
    /* xonLimit == 104              */

    st = cpssExMxPortXonLimitSet(dev, port, xonLimit);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev ,port, xonLimit);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortXoffLimitSet
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_U16   xoffLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPortXoffLimitSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with xoffLimit [512]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U16  xoffLimit;

    xoffLimit = 512;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with xoffLimit [512]. Expected: GT_OK. */
            st = cpssExMxPortXoffLimitSet(dev, port, xoffLimit);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, xoffLimit);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each invalid port */
            st = cpssExMxPortXoffLimitSet(dev, port, xoffLimit);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, xoffLimit);
        }

        /* 1.3. For active device check that function returns non GT_OK */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        /* xoffLimit == 512 */

        st = cpssExMxPortXoffLimitSet(dev, port, xoffLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, xoffLimit);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_BUFMG_VALID_PHY_PORT_CNS;
    xoffLimit = 10;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortXoffLimitSet(dev, port, xoffLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, xoffLimit);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == PRV_PORT_TESTED_CNS  */
    /* xoffLimit == 10              */

    st = cpssExMxPortXoffLimitSet(dev, port, xoffLimit);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev ,port, xoffLimit);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortRxBufLimitSet
(
    IN GT_U8    dev,
    IN GT_U8    port,
    IN GT_U16   rxBufLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPortRxBufLimitSet)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call with rxBufLimit [9216]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U16  rxBufLimit;

    rxBufLimit = 9216;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with rxBufLimit [9216]. Expected: GT_OK. */
            st = cpssExMxPortRxBufLimitSet(dev, port, rxBufLimit);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, rxBufLimit);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each invalid port */
            st = cpssExMxPortRxBufLimitSet(dev, port, rxBufLimit);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, rxBufLimit);
        }

        /* 1.3. For active device check that function returns non GT_OK */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        /* rxBufLimit == 9216 */

        st = cpssExMxPortRxBufLimitSet(dev, port, rxBufLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, rxBufLimit);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PORT_BUFMG_VALID_PHY_PORT_CNS;
    rxBufLimit = 145;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortRxBufLimitSet(dev, port, rxBufLimit);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, rxBufLimit);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == PRV_PORT_TESTED_CNS  */
    /* rxBufLimit == 145            */

    st = cpssExMxPortRxBufLimitSet(dev, port, rxBufLimit);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev ,port, rxBufLimit);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortBufMngMaxBufSet
(
    IN GT_U8    dev,
    IN GT_U16   maxBuffers
)
*/
UTF_TEST_CASE_MAC(cpssExMxPortBufMngMaxBufSet)
{
/*
ITERATE_DEVICES
1.1. Call with maxBuffers [128]. Expected: GT_OK.
1.2. Call function with maxBuff [4096]. Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U16  maxBuffers;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with maxBuffers [128]. Expected: GT_OK. */
        maxBuffers = 128;

        st = cpssExMxPortBufMngMaxBufSet(dev, maxBuffers);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, maxBuffers);

        /* 1.2. Call function with maxBuff [4096]. Expected: NON GT_OK. */
        maxBuffers = 4096;

        st = cpssExMxPortBufMngMaxBufSet(dev, maxBuffers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, maxBuffers);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    maxBuffers = 128;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortBufMngMaxBufSet(dev, maxBuffers);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, maxBuffers);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortBufMngMaxBufSet(dev, maxBuffers);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, maxBuffers);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortBufMngLinesLimitSet
(
    IN GT_U8    dev,
    IN GT_U16   linesLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPortBufMngLinesLimitSet)
{
/*
ITERATE_DEVICES
1.1. Call with linesLimit [102]. Expected: GT_OK.
1.2. Call function with linesLimit [512]. Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U16  linesLimit;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with linesLimit [102]. Expected: GT_OK. */
        linesLimit = 102;

        st = cpssExMxPortBufMngLinesLimitSet(dev, linesLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, linesLimit);

        /* 1.2. Call function with linesLimit [512]. Expected: NON GT_OK. */
        linesLimit = 512;

        st = cpssExMxPortBufMngLinesLimitSet(dev, linesLimit);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, linesLimit);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    linesLimit = 102;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortBufMngLinesLimitSet(dev, linesLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, linesLimit);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortBufMngLinesLimitSet(dev, linesLimit);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, linesLimit);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortCpuRxBufLimitSet
(
    IN GT_U8    dev,
    IN GT_U16   rxBufLimit
)
*/
UTF_TEST_CASE_MAC(cpssExMxPortCpuRxBufLimitSet)
{
/*
ITERATE_DEVICES
1.1. Call with rxBufLimit [512]. Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U16  rxBufLimit;

    rxBufLimit = 512;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with rxBufLimit [512]. Expected: GT_OK. */
        st = cpssExMxPortCpuRxBufLimitSet(dev, rxBufLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, rxBufLimit);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    rxBufLimit = 100;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortCpuRxBufLimitSet(dev, rxBufLimit);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, rxBufLimit);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* rxBufLimit == 100            */

    st = cpssExMxPortCpuRxBufLimitSet(dev, rxBufLimit);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, rxBufLimit);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortBufModeGet
(
    IN  GT_U8                 dev,
    OUT CPSS_BUF_MODE_ENT     *bufModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPortBufModeGet)
{
/*
ITERATE_DEVICES
1.1. Call with non-null bufModePtr. Expected: GT_OK.
1.2. Call with bufModePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8               dev;
    CPSS_BUF_MODE_ENT   bufMode;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-NULL bufModePtr. Expected: GT_OK. */
        st = cpssExMxPortBufModeGet(dev, &bufMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call with bufModePtr [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxPortBufModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortBufModeGet(dev, &bufMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortBufModeGet(dev, &bufMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPortCpuRxBufCountGet
(
    IN  GT_U8   dev,
    OUT GT_U16  *cpuRxBufCntPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPortCpuRxBufCountGet)
{
/*
ITERATE_DEVICES
1.1. Call with non-null cpuRxBufCntPtr. Expected: GT_OK.
1.2. Call with cpuRxBufCntPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U16  cpuRxBufCnt;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with non-NULL cpuRxBufCntPtr. Expected: GT_OK. */
        st = cpssExMxPortCpuRxBufCountGet(dev, &cpuRxBufCnt);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call with cpuRxBufCntPtr [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxPortCpuRxBufCountGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPortCpuRxBufCountGet(dev, &cpuRxBufCnt);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPortCpuRxBufCountGet(dev, &cpuRxBufCnt);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxPortBufMg suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPortBufMg)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortXonLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortXoffLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortRxBufLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortBufMngMaxBufSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortBufMngLinesLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortCpuRxBufLimitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortBufModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPortCpuRxBufCountGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPortBufMg)
