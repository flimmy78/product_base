/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmPhySmiUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmPhySmi, that provides
*        API implementation for port Core Serial Management Interface facility.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/phy/cpssExMxPmPhySmi.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define PHY_SMI_VALID_PHY_PORT_CNS  0

/* Invalid enum */
#define PHY_SMI_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPhyPortAddrSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U8   phyAddr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPhyPortAddrSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with phyAddr [0 / 16 / 31].
    Expected: GT_OK for non-XG ports and GT_NOT_SUPPORTED for XG ports.
    1.1.2. For non-XG call with out of range phyAddr [32].
    Expected: NOT GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PHY_SMI_VALID_PHY_PORT_CNS;

    GT_U8                  phyAddr  = 0;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;


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
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "prvUtfPortTypeGet: %d, %d", dev, port);

            /* 
                1.1.1. Call with phyAddr [0 / 16 / 31].
                Expected: GT_OK for non-XG ports and GT_NOT_SUPPORTED for XG ports.
            */

            /* Call with phyAddr [0] */
            phyAddr = 0;

            st = cpssExMxPmPhyPortAddrSet(dev, port, phyAddr);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, phyAddr);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyAddr);
            }

            /* Call with phyAddr [16] */
            phyAddr = 16;

            st = cpssExMxPmPhyPortAddrSet(dev, port, phyAddr);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, phyAddr);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyAddr);
            }

            /* Call with phyAddr [31] */
            phyAddr = 31;

            st = cpssExMxPmPhyPortAddrSet(dev, port, phyAddr);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, phyAddr);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyAddr);
            }

            /* 
                1.1.2. For non-XG call with out of range phyAddr [32].
                Expected: NOT GT_OK.
            */
            phyAddr = 32;

            if (PRV_CPSS_PORT_XG_E != portType)
            {
                st = cpssExMxPmPhyPortAddrSet(dev, port, phyAddr);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyAddr);
            }
        }

        phyAddr = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmPhyPortAddrSet(dev, port, phyAddr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);
        
        st = cpssExMxPmPhyPortAddrSet(dev, port, phyAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPhyPortAddrSet(dev, port, phyAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    phyAddr = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPhyPortAddrSet(dev, port, phyAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPhyPortAddrSet(dev, port, phyAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPhyPortSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyReg,
    IN  GT_U16    data
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPhyPortSmiRegisterWrite)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with phyReg [0 / 16 / 31],
                     data [0 / 100 / 0xFF].
    Expected: GT_OK for non-XG ports and GT_NOT_SUPPORTED for XG ports.
    1.1.2. For non-XG ports call cpssExMxPmPhyPortSmiRegisterRead with the same phyReg and non-NULL dataPtr.
    Expected: GT_OK and the same data. 
    1.1.3. For non-XG ports call with out of range phyReg [32],
                                      data [0xFF].
    Expected: NOT GT_OK.
    1.1.4. For non-XG ports call with phyReg [31],
                                      data [0xFFFF] (no any constraints).
    Expected: GT_OK.
*/

    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PHY_SMI_VALID_PHY_PORT_CNS;

    GT_U8                  phyReg   = 0;
    GT_U16                 data     = 0;
    GT_U16                 dataGet  = 0;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;


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
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "prvUtfPortTypeGet: %d, %d", dev, port);

            /* 
                1.1.1. Call with phyReg [0 / 16 / 31],
                                 data [0 / 100 / 0xFF].
                Expected: GT_OK for non-XG ports and GT_NOT_SUPPORTED for XG ports.
            */

            /* Call with phyReg [0], data [0] */
            phyReg = 0;
            data   = 0;

            st = cpssExMxPmPhyPortSmiRegisterWrite(dev, port, phyReg, data);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, phyReg, data);
            }
            else
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, phyReg, data);

                /* 
                    1.1.2. For non-XG ports call cpssExMxPmPhyPortSmiRegisterRead with the same phyReg and non-NULL dataPtr.
                    Expected: GT_OK and the same data. 
                */
                st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &dataGet);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPhyPortSmiRegisterRead: %d, %d, %d", dev, port, phyReg);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(data, dataGet,
                           "get another data than was set: %d, %d", dev, port);
            }

            /* Call with phyReg [16], data [100] */
            phyReg = 16;
            data   = 100;

            st = cpssExMxPmPhyPortSmiRegisterWrite(dev, port, phyReg, data);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, phyReg, data);
            }
            else
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, phyReg, data);

                /* 
                    1.1.2. For non-XG ports call cpssExMxPmPhyPortSmiRegisterRead with the same phyReg and non-NULL dataPtr.
                    Expected: GT_OK and the same data. 
                */
                st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &dataGet);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPhyPortSmiRegisterRead: %d, %d, %d", dev, port, phyReg);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(data, dataGet,
                           "get another data than was set: %d, %d", dev, port);
            }

            /* Call with phyReg [31], data [0xFF] */
            phyReg = 31;
            data   = 0xFF;

            st = cpssExMxPmPhyPortSmiRegisterWrite(dev, port, phyReg, data);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, phyReg, data);
            }
            else
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, phyReg, data);

                /* 
                    1.1.2. For non-XG ports call cpssExMxPmPhyPortSmiRegisterRead with the same phyReg and non-NULL dataPtr.
                    Expected: GT_OK and the same data. 
                */
                st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &dataGet);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPhyPortSmiRegisterRead: %d, %d, %d", dev, port, phyReg);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(data, dataGet,
                           "get another data than was set: %d, %d", dev, port);
            }

            /* 
                1.1.3. For non-XG ports call with out of range phyReg [32],
                                                  data [0xFF].
                Expected: NOT GT_OK.
           */
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                phyReg = 32;

                st = cpssExMxPmPhyPortSmiRegisterWrite(dev, port, phyReg, data);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyReg);

                phyReg = 0;
            }

            /* 
                1.1.4. For non-XG ports call with phyReg [31],
                                                  data [0xFFFF] (no any constraints).
                Expected: GT_OK.
            */
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                data = 0xFFFF;

                st = cpssExMxPmPhyPortSmiRegisterWrite(dev, port, phyReg, data);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, data = %d", dev, port, data);
            }
        }

        phyReg = 0;
        data   = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssExMxPmPhyPortSmiRegisterWrite(dev, port, phyReg, data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPhyPortSmiRegisterWrite(dev, port, phyReg, data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPhyPortSmiRegisterWrite(dev, port, phyReg, data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    phyReg = 0;
    data   = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPhyPortSmiRegisterWrite(dev, port, phyReg, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPhyPortSmiRegisterWrite(dev, port, phyReg, data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPhyPortSmiRegisterRead
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyReg,
    OUT GT_U16    *dataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPhyPortSmiRegisterRead)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with phyReg [0 / 16 / 31],
                     non-null dataPtr. 
    Expected: GT_OK for non-XG ports and GT_NOT_SUPPORTED for XG ports.
    1.1.2. For non-XG ports call with out of range phyReg [32],
                                      non-null dataPtr. 
    Expected: NOT GT_OK.
    1.1.3. For non-XG ports call with phyReg [31],
                                      dataPtr [NULL].
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = PHY_SMI_VALID_PHY_PORT_CNS;

    GT_U8                  phyReg   = 0;
    GT_U16                 data     = 0;
    PRV_CPSS_PORT_TYPE_ENT portType = PRV_CPSS_PORT_NOT_EXISTS_E;

    
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
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "prvUtfPortTypeGet: %d, %d", dev, port);

            /* 
                1.1.1. Call with phyReg [0 / 16 / 31], non-null dataPtr. 
                Expected: GT_OK for non-XG ports and GT_NOT_SUPPORTED for XG ports.
            */

            /* Call with phyReg [0], non-null dataPtr */
            phyReg = 0;

            st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &data);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, phyReg);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyReg);
            }

            /* Call with phyReg [16], non-null dataPtr */
            phyReg = 16;

            st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &data);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, phyReg);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyReg);
            }

            /* Call with phyReg [31], non-null dataPtr */
            phyReg = 31;

            st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &data);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, port, phyReg);
            }
            else
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyReg);
            }

            /* 
                1.1.2. For non-XG ports call with out of range phyReg [32],
                                                  non-null dataPtr. 
                Expected: NOT GT_OK.
            */
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                phyReg = 32;

                st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &data);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyReg);

                phyReg = 0;
            }

            /* 
                1.1.3. For non-XG ports call with phyReg [31], dataPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            if (PRV_CPSS_PORT_XG_E != portType)
            {
                st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, dataPtr = NULL", dev, port);
            }
        }

        phyReg = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    phyReg = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPhyPortSmiRegisterRead(dev, port, phyReg, &data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPhyPort10GSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_U8     phyId,
    IN  GT_U16    phyReg,
    IN  GT_U8     phyDev,
    IN  GT_U16    data
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPhyPort10GSmiRegisterWrite)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with phyId [0 / 31], 
                     useExternalPhy [GT_FALSE and GT_TRUE],
                     phyReg [0 / 0xFF],
                     phyDev [0 / 31],
                     data [0 / 0xFF].
    Expected: GT_OK for XG ports and GT_NOT_SUPPORTED for non-XG ports.
    1.1.2. For XG ports call cpssExMxPmPhyPort10GSmiRegisterRead with the same phyID, useExternalPhy, phyReg, phyDev and non-NULL dataPtr.
    Expected: GT_OK and the same data.
    1.1.3. For XG ports call with out of range phyId [32], 
                                               useExternalPhy [GT_TRUE] 
                                               and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.1.4. For XG ports call with useExternalPhy [GT_FALSE], 
                                  out of range phyId [32] (not relevant), 
                                  and other parameters from 1.1.
    Expected: GT_OK.
    1.1.5. For XG ports call with phyReg [0xFFFF] (no any constraints), 
                                  and other parameters from 1.1.
    Expected: GT_OK.
    1.1.6. For XG ports call with out of range phyDev [32]
                                               and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.1.7. For XG ports call with data [0xFFFF] (no any constraints) 
                                  and other parameters from 1.1.
    Expected: GT_OK.
*/

    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PHY_SMI_VALID_PHY_PORT_CNS;

    GT_U8                  phyId          = 0;
    GT_U16                 phyReg         = 0;
    GT_U8                  phyDev         = 0;
    GT_U16                 data           = 0;
    GT_U16                 dataGet        = 0;
    PRV_CPSS_PORT_TYPE_ENT portType       = PRV_CPSS_PORT_NOT_EXISTS_E;

    
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
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "prvUtfPortTypeGet: %d, %d", dev, port);

            /* 
                1.1.1. Call with phyId [0 / 31],
                                 useExternalPhy [GT_FALSE and GT_TRUE],
                                 phyReg [0 / 0xFF],
                                 phyDev [0 / 31],
                                 data [0 / 0xFF].
                Expected: GT_OK for XG ports and GT_NOT_SUPPORTED for non-XG ports.
            */

            /* Call with phyId [0] */
            phyId          = 0;
            phyReg         = 0;
            phyDev         = 0;
            data           = 0;

            st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, phyId, phyReg, phyDev, data);

                /*  
                    1.1.2. For XG ports call cpssExMxPmPhyPort10GSmiRegisterRead with the same phyID,
                                                                                      phyReg,
                                                                                      phyDev
                                                                                      and non-NULL dataPtr.
                    Expected: GT_OK and the same data.
                */
                st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataGet);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPhyPort10GSmiRegisterRead: %d, %d, %d, %d", dev, phyId, phyReg, phyDev);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(data, dataGet,
                           "get another data than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, phyId, phyReg, phyDev, data);
            }

            /* Call with phyId [31] */
            phyId          = 31;
            phyReg         = 0xFF;
            phyDev         = 31;
            data           = 0xFF;

            st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, phyId, phyReg, phyDev, data);

                /*  
                    1.1.2. For XG ports call cpssExMxPmPhyPort10GSmiRegisterRead with the same phyID,
                                                                                      phyReg,
                                                                                      phyDev
                                                                                      and non-NULL dataPtr.
                    Expected: GT_OK and the same data.
                */
                st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataGet);
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, 
                           "cpssExMxPmPhyPort10GSmiRegisterRead: %d, %d, %d, %d", dev, phyId, phyReg, phyDev);

                /* Verifying values */
                UTF_VERIFY_EQUAL2_STRING_MAC(data, dataGet,
                           "get another data than was set: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, phyId, phyReg, phyDev, data);
            }

            if (PRV_CPSS_PORT_XG_E == portType)
            {
                /*
                    1.1.3. For XG ports call with out of range phyId [32],
                                                  useExternalPhy [GT_TRUE]
                                                  ant other parameters from 1.1.
                    Expected: NOT GT_OK.
                */
                phyId          = 32;
                
                st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, phyId);

                /*
                    1.1.4. For XG ports call with useExternalPhy [GT_FALSE],
                                                  out of range phyId [32] (not relevant),
                                                  and other parameters from 1.1.
                    Expected: GT_OK.
                */
                
                st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, phyId);

                phyId = 0;
            
                /*
                    1.1.5. For XG ports call with phyReg [0xFFFF] (no any constraints),
                                                  and other parameters from 1.1.
                    Expected: GT_OK.
                */
                phyReg = 0xFFFF;
            
                st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, phyReg = %d", dev, phyReg);

                phyReg = 0;
            
                /*
                    1.1.6. For XG ports call with out of range phyDev [32]
                                                  and other parameters from 1.1.
                    Expected: NOT GT_OK.
                */
                phyDev = PRV_CPSS_MAX_PP_DEVICES_CNS;
            
                st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, phyDev = %d", dev, phyDev);

                phyDev = dev;
            
                /*
                    1.1.7. For XG ports call with data [0xFFFF] (no any constraints)
                                                  and other parameters from 1.1.
                    Expected: GT_OK.
                */
                data = 0xFFFF;

                st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, data = %d", dev, data);
            }
        }

        phyId          = 0;
        phyReg         = 0;
        phyDev         = 0;
        data           = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    phyId          = 0;
    phyReg         = 0;
    phyDev         = 0;
    data           = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPhyPort10GSmiRegisterWrite(dev, phyId, phyReg, phyDev, data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPhyPort10GSmiRegisterRead
(
    IN  GT_U8     devNum,
    IN  GT_U8     phyId,
    IN  GT_U16    phyReg,
    IN  GT_U8     phyDev,
    OUT GT_U16    *dataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPhyPort10GSmiRegisterRead)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with phyId [0 / 31],
                     useExternalPhy [GT_FALSE and GT_TRUE],
                     phyReg [0 / 0xFF],
                     phyDev [0 / 31],
                     non-null dataPtr.
    Expected: GT_OK for XG ports and GT_NOT_SUPPORTED for non-XG ports.
    1.1.2. For XG ports call with out of range phyId [32],
                                  useExternalPhy [GT_TRUE]
                                  and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.1.3. For XG ports call with useExternalPhy [GT_FALSE],
                                  out of range phyId [32] (not relevant)
                                  and other parameters from 1.1.
    Expected: GT_OK.
    1.1.4. For XG ports call with phyReg [0xFFFF] (no any constraints)
                                  and other parameters from 1.1.
    Expected: GT_OK.
    1.1.5. For XG ports call with out of range phyDev [32]
                                  and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.1.6. For XG ports call with data [NULL]
                                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/

    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PHY_SMI_VALID_PHY_PORT_CNS;

    GT_U8                  phyId          = 0;
    GT_U16                 phyReg         = 0;
    GT_U8                  phyDev         = 0;
    GT_U16                 dataPtr        = 0;
    PRV_CPSS_PORT_TYPE_ENT portType       = PRV_CPSS_PORT_NOT_EXISTS_E;


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
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call with phyId [0 / 31],
                                 useExternalPhy [GT_FALSE and GT_TRUE],
                                 phyReg [0 / 0xFF],
                                 phyDev [0 / 31],
                                 non-null dataPtr.
                Expected: GT_OK for XG ports and GT_NOT_SUPPORTED for non-XG ports.
            */

            /* Call with phyId [0] */
            phyId          = 0;
            phyReg         = 0;
            phyDev         = 0;

            st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataPtr);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, phyId, phyReg, phyDev);
            }
            else
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, phyId, phyReg, phyDev);
            }

            /* Call with phyId [31] */
            phyId          = 31;
            phyReg         = 0xFF;
            phyDev         = 31;

            st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataPtr);
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, phyId, phyReg, phyDev);
            }
            else
            {
                UTF_VERIFY_EQUAL4_PARAM_MAC(GT_NOT_SUPPORTED, st, dev, phyId, phyReg, phyDev);
            }

            /*
                1.1.2. For XG ports call with out of range phyId [32],
                                              useExternalPhy [GT_TRUE]
                                              and other parameters from 1.1.
                Expected: NOT GT_OK.
            */
            if (PRV_CPSS_PORT_XG_E == portType)
            {
                phyId = 32;
            
                st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataPtr);
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, phyId);
            
                /*
                    1.1.3. For XG ports call with useExternalPhy [GT_FALSE],
                                                  out of range phyId [32] (not relevant)
                                                  and other parameters from 1.1.
                    Expected: GT_OK.
                */
                
                st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataPtr);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, phyId);

                phyId = 32;

                /*
                    1.1.4. For XG ports call with phyReg [0xFFFF] (no any constraints)
                                                  and other parameters from 1.1.
                    Expected: GT_OK.
                */
                phyReg = 0xFFFF;
                
                st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataPtr);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, phyReg = %d", dev, phyReg);
            
                phyReg = 0;

                /*
                    1.1.5. For XG ports call with out of range phyDev [32]
                                                  and other parameters from 1.1.
                    Expected: NOT GT_OK.
                */
                phyDev = PRV_CPSS_MAX_PP_DEVICES_CNS;

                st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataPtr);
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, phyDev = %d", dev, phyDev);

                phyDev = dev;

                /*
                    1.1.6. For XG ports call with data [NULL]
                                                  and other parameters from 1.1.
                    Expected: GT_BAD_PTR.
                */
                st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, NULL);
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dataPtr = NULL", dev);
            }
        }

        phyId          = 0;
        phyReg         = 0;
        phyDev         = 0;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataPtr);
            UTF_VERIFY_NOT_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    phyId          = 0;
    phyReg         = 0;
    phyDev         = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPhyPort10GSmiRegisterRead(dev, phyId, phyReg, phyDev, &dataPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPhyAutonegSmiEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPhyAutonegSmiEnableSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPhyAutonegSmiEnableGet with non-NULL enabledPtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PHY_SMI_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


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
                1.1.1. Call with enable [GT_FALSE and GT_TRUE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPhyAutonegSmiEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPhyAutonegSmiEnableGet with non-NULL enabledPtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPhyAutonegSmiEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPhyAutonegSmiEnableGet: %d, %d", dev, port);
                                                                                   
            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPhyAutonegSmiEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPhyAutonegSmiEnableGet with non-NULL enabledPtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPhyAutonegSmiEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmPhyAutonegSmiEnableGet: %d, %d", dev, port);
                                                                                   
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
            st = cpssExMxPmPhyAutonegSmiEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPhyAutonegSmiEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
     
        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPhyAutonegSmiEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPhyAutonegSmiEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPhyAutonegSmiEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port); 
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPhyAutonegSmiEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enabledPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPhyAutonegSmiEnableGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enabledPtr. 
    Expected: GT_OK.
    1.1.2. Call with enabledPtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PHY_SMI_VALID_PHY_PORT_CNS;

    GT_BOOL     enabled = GT_FALSE;


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
                1.1.1. Call with non-null enabledPtr. 
                Expected: GT_OK.
            */
            st = cpssExMxPmPhyAutonegSmiEnableGet(dev, port, &enabled);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enabledPtr [NULL]. 
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPhyAutonegSmiEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enabledPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssExMxPmPhyAutonegSmiEnableGet(dev, port, &enabled);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPhyAutonegSmiEnableGet(dev, port, &enabled);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPhyAutonegSmiEnableGet(dev, port, &enabled);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPhyAutonegSmiEnableGet(dev, port, &enabled);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPhyAutonegSmiEnableGet(dev, port, &enabled);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPhySmiAutoMediaSelectSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPhySmiAutoMediaSelectSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmPhySmiAutoMediaSelectGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PHY_SMI_VALID_PHY_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;


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
                1.1.1. Call with enable [GT_FALSE and GT_TRUE]. Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmPhySmiAutoMediaSelectSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPhySmiAutoMediaSelectGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPhySmiAutoMediaSelectGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmPhySmiAutoMediaSelectGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmPhySmiAutoMediaSelectSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmPhySmiAutoMediaSelectGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmPhySmiAutoMediaSelectGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmPhySmiAutoMediaSelectGet: %d, %d", dev, port);

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
            st = cpssExMxPmPhySmiAutoMediaSelectSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPhySmiAutoMediaSelectSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPhySmiAutoMediaSelectSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPhySmiAutoMediaSelectSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPhySmiAutoMediaSelectSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmPhySmiAutoMediaSelectGet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmPhySmiAutoMediaSelectGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS (ExMxPm)
    1.1.1. Call with non-null enablePtr. 
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = PHY_SMI_VALID_PHY_PORT_CNS;

    GT_BOOL     enable = GT_FALSE;
    
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
            st = cpssExMxPmPhySmiAutoMediaSelectGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmPhySmiAutoMediaSelectGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            st = cpssExMxPmPhySmiAutoMediaSelectGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmPhySmiAutoMediaSelectGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmPhySmiAutoMediaSelectGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmPhySmiAutoMediaSelectGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmPhySmiAutoMediaSelectGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmPhySmi suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmPhySmi)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPhyPortAddrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPhyPortSmiRegisterWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPhyPortSmiRegisterRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPhyPort10GSmiRegisterWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPhyPort10GSmiRegisterRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPhyAutonegSmiEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPhyAutonegSmiEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPhySmiAutoMediaSelectSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmPhySmiAutoMediaSelectGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmPhySmi)

