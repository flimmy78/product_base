/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPhySmiUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxChPhySmi.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
/* #include <cpss/generic/cpssTypes.h> */
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <cpss/dxCh/dxChxGen/phy/private/prvCpssDxChPhySmi.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define PHY_SMI_VALID_PHY_PORT_CNS  0

#define IS_PHY_ADDR_SUPPORTED(devNum, portNum)                     \
    ((PRV_CPSS_PORT_XG_E > portType) || \
        (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].isFlexLink == GT_TRUE))

#define IS_PHY_SMI_IF_SUPPORTED(devNum, portNum)    \
            (PRV_CPSS_PORT_XG_E > portType)

/* ch1,2,3,xcat ports 24..27 are XG ports, xcat2 has no XG ports    
in Lion all ports are XG */
#define IS_XG_PORT_SUPPORT_MAC(dev,port)      \
    (((port) >= 24) ? 1 :    \
    (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 1 : 0)

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyPortAddrSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U8   phyAddr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyPortAddrSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with phyAddr [12].
    Expected: GT_OK for non 10Gbps port;
              GT_NOT_SUPPORTED for 10Gbps port.
    1.1.2. Check for out of range phy address. For non
           10Gbps port call function with phyAddr [32].
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   phyAddr = 0;
    GT_U8   phyAddFromHwPtr = 0;
    GT_U8   phyAddFromDbPtr = 0;

    CPSS_PP_FAMILY_TYPE_ENT     devFamily;
    PRV_CPSS_PORT_TYPE_ENT      portType;

    /* prepare iterator for go over all active devices, ALL families*/
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* get port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            /* 1.1.1. Call function with phyAddr [12]. Expected:        */
            /* GT_OK for non 10Gbps port;                               */
            /* GT_NOT_SUPPORTED for  10Gbps port.                       */
            phyAddr = 12;

            st = cpssDxChPhyPortAddrSet(dev, port, phyAddr);

            if (IS_PHY_ADDR_SUPPORTED(dev,port))
            {/* non 10Gbps or Flex link ports */
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                       "Cheetah device, non 10Gbps port: %d, %d, %d",
                                                dev, port, phyAddr);

                st = cpssDxChPhyPortAddrGet(dev, port, &phyAddFromHwPtr,
                                            &phyAddFromDbPtr);

                UTF_VERIFY_EQUAL2_STRING_MAC(phyAddr, phyAddFromHwPtr ,
                     "get another <phyAddr> value than was set: %d, %d", dev, port);
            }
            else
            {/* 10Gbps */
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_NOT_SUPPORTED, st,
                        "Cheetah device, 10Gbps port: %d, %d", dev, port);
            }

            /*
               1.1.2. Check for out of range phy address.
               For non 10Gbps port call function with phyAddr [BIT_5].
               Expected: GT_OUT_OF_RANGE.
            */
            if (IS_PHY_ADDR_SUPPORTED(dev,port))
            {
                phyAddr = BIT_5;

                st = cpssDxChPhyPortAddrSet(dev, port, phyAddr);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, port, phyAddr);
            }
        }

        phyAddr = 12;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhyPortAddrSet(dev, port, phyAddr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhyPortAddrSet(dev, port, phyAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    phyAddr = 12;
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator - iterate ONLY non-active devices! Because  */
    /* for non-Ch & 10G port we expect "non GT_OK", but not GT_BAD_PARAM   */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyPortAddrSet(dev, port, phyAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */
    /* phyAddr ==12 */

    st = cpssDxChPhyPortAddrSet(dev, port, phyAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyPortAddrGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U8   *phyAddFromHwPtr,
    OUT GT_U8   *phyAddFromDbPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyPortAddrGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with not null pointers.
    Expected: GT_OK for non 10Gbps port;
              GT_NOT_SUPPORTED for 10Gbps port.
    1.1.2. Call with wrong phyAddFromHwPtr [NULL].
    Expected: GT_BAD_PTR.
    1.1.3. Call with wrong phyAddFromDbPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   phyAddFromHwPtr = 0;
    GT_U8   phyAddFromDbPtr = 0;

    CPSS_PP_FAMILY_TYPE_ENT     devFamily;
    PRV_CPSS_PORT_TYPE_ENT      portType;

    /* prepare iterator for go over all active devices, ALL families*/
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* get port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            /*
                1.1.1. Call function with not null pointers.
                Expected: GT_OK for non 10Gbps port;
                          GT_NOT_SUPPORTED for 10Gbps port.
            */

            st = cpssDxChPhyPortAddrGet(dev, port, &phyAddFromHwPtr, &phyAddFromDbPtr);

            if (IS_PHY_ADDR_SUPPORTED(dev,port))
            {/* non 10Gbps or flex ports */
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                      "Cheetah device, non 10Gbps port: %d, %d", dev, port);
            }
            else
            {/* 10Gbps */
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_NOT_SUPPORTED, st,
                          "Cheetah device, 10Gbps port: %d, %d", dev, port);
            }

            /*
                1.1.2. Call with wrong phyAddFromHwPtr [NULL].
                Expected: GT_BAD_PTR.
            */

            st = cpssDxChPhyPortAddrGet(dev, port, NULL, &phyAddFromDbPtr);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                  "Cheetah device, non 10Gbps port: %d, %d", dev, port);

            /*
                1.1.3. Call with wrong phyAddFromDbPtr [NULL].
                Expected: GT_BAD_PTR.
            */

            st = cpssDxChPhyPortAddrGet(dev, port, &phyAddFromHwPtr, NULL);

            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                  "Cheetah device, non 10Gbps port: %d, %d", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhyPortAddrGet(dev, port, &phyAddFromHwPtr, &phyAddFromDbPtr);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhyPortAddrGet(dev, port, &phyAddFromHwPtr, &phyAddFromDbPtr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator - iterate ONLY non-active devices! Because  */
    /* for non-Ch & 10G port we expect "non GT_OK", but not GT_BAD_PARAM   */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyPortAddrGet(dev, port, &phyAddFromHwPtr, &phyAddFromDbPtr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */

    st = cpssDxChPhyPortAddrGet(dev, port, &phyAddFromHwPtr, &phyAddFromDbPtr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyPortSmiRegisterRead
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyReg,
    OUT GT_U16    *dataPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyPortSmiRegisterRead)
{
/*
ITERATE_DEVICES_PHY_PORTS
1.1.1. Call function with non-NULL dataPtr and phyReg [12].
Expected: GT_OK for non 10Gbps port;
          GT_NOT_SUPPORTED for 10Gbps port.
1.1.2. Check for out of range SMI register. For non 10Gbps port call
       function with phyReg [32] and non-NULL dataPtr.
Expected: NON GT_OK.
1.1.3. Check for NULL pointer support. For non 10Gbps port call function
       with phyRegr [12] and dataPtr [NULL].
Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   phyReg;
    GT_U16  data;

    CPSS_PP_FAMILY_TYPE_ENT     devFamily;
    PRV_CPSS_PORT_TYPE_ENT      portType;

    /* this function is highly dependent on ASIC family, so lets run if for xcat only */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E  | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* get port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            /* 1.1.1. Call function with non-NULL dataPtr and phyReg [12].  */
            /* Expected: GT_OK for non 10Gbps port;     */
            /* GT_NOT_SUPPORTED for 10Gbps port and;     */
            phyReg = 12;

            st = cpssDxChPhyPortSmiRegisterRead(dev, port, phyReg, &data);

            if (IS_PHY_SMI_IF_SUPPORTED(dev,port))
            {/* non 10Gbps */
                if (0 == IS_XG_PORT_SUPPORT_MAC(dev,port))
                {
                    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                 "Cheetah device, non 10Gbps port: %d, %d, %d",
                                 dev, port, phyReg);
                }
                else
                {
                    UTF_VERIFY_EQUAL3_STRING_MAC(GT_NOT_INITIALIZED, st,
                                 "Cheetah device, non 10Gbps port: %d, %d, %d",
                                 dev, port, phyReg);
                }
            }
            else
            {/* 10Gbps */
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_NOT_SUPPORTED, st,
                                             "Cheetah device, 10Gbps port: %d, %d",
                                             dev, port);
            }

            if (IS_PHY_SMI_IF_SUPPORTED(dev,port))
            {
                /* 1.1.2. Check for out of range SMI register.                   */
                /* For non 10Gbps port call function with                        */
                /* phyReg [32] and non-NULL dataPtr. Expected: NON GT_OK.        */
                phyReg = 32;

                st = cpssDxChPhyPortSmiRegisterRead(dev, port, phyReg, &data);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyReg);

                /* 1.1.3. Check for NULL pointer support. For                    */
                /* non 10Gbps port call function with phyRegr [12] and           */
                /* dataPtr [NULL]. Expected: GT_BAD_PTR.                         */
                phyReg = 12;

                st = cpssDxChPhyPortSmiRegisterRead(dev, port, phyReg, NULL);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL",
                                             dev, port, phyReg);
            }
        }

        phyReg = 12;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhyPortSmiRegisterRead(dev, port, phyReg, &data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhyPortSmiRegisterRead(dev, port, phyReg, &data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    phyReg = 12;
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator*/

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyPortSmiRegisterRead(dev, port, phyReg, &data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */
    /* phyReg == 12 */

    st = cpssDxChPhyPortSmiRegisterRead(dev, port, phyReg, &data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyPortSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyReg,
    IN  GT_U16    data
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyPortSmiRegisterWrite)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with phyReg [22] and data [0xA].
    Expected: GT_OK for non 10Gbps port;
              GT_NOT_SUPPORTED for 10Gbps port.
    1.1.2. Call cpssDxChPhyPortSmiRegisterRead function with non-NULL
           dataPtr and phyReg [22].
    Expected: GT_OK and dataPtr [0xA] for non 10Gbps port;
              GT_NOT_SUPPORTED for 10Gbps port.
    1.1.3. Check for out of range SMI register. For non 10Gbps port call
           function with phyAddr [32] and data [0xA].
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   phyReg;
    GT_U16  data;

    GT_U16  retData;

    CPSS_PP_FAMILY_TYPE_ENT     devFamily;
    PRV_CPSS_PORT_TYPE_ENT      portType;
    
    GT_BOOL currPhyPollStatus;

    data = 0xA;

    /* this function is highly dependent on ASIC family, so lets run if for xcat only */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E  | UTF_LION_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* get port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            if (IS_PHY_SMI_IF_SUPPORTED(dev,port))
            {/* non 10Gbps */
                if (0 == IS_XG_PORT_SUPPORT_MAC(dev,port))
                {
                    st = cpssDxChPhyAutonegSmiGet(dev, port, &currPhyPollStatus);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                             "cpssDxChPhyAutonegSmiGet: %d, %d", dev, port);
        
                    /* swtich off phy polling by ASIC to prevent phy page change */
                    st = cpssDxChPhyAutonegSmiSet(dev, port, GT_FALSE);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                                 "cpssDxChPhyAutonegSmiSet: %d, %d, GT_FALSE", dev, port);
                }
            }

            /* 1.1.1. Call function with data [0xA] and phyReg [22 dec].    */
            /* Expected: GT_OK for non 10Gbps port;                         */
            /* GT_NOT_SUPPORTED for 10Gbps port and;                        */
            phyReg = 22; /* page address register exists on every page in all Marvell PHY's AFAIK */

            st = cpssDxChPhyPortSmiRegisterWrite(dev, port, phyReg, data);

            if (IS_PHY_SMI_IF_SUPPORTED(dev,port))
            {/* non 10Gbps */
                if (0 == IS_XG_PORT_SUPPORT_MAC(dev,port))
                {
                    UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                                 "Cheetah device, non 10Gbps port: %d, %d, %d, %d",
                                                 dev, port, phyReg, data);
                }
                else
                {
                    UTF_VERIFY_EQUAL4_STRING_MAC(GT_NOT_INITIALIZED, st,
                                                 "Cheetah device, non 10Gbps port: %d, %d, %d, %d",
                                                 dev, port, phyReg, data);
                }
            }
            else
            {/* 10Gbps */
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_NOT_SUPPORTED, st,
                                             "Cheetah device, 10Gbps port: %d, %d",
                                             dev, port);
            }

            /* 1.1.2. Call cpssDxChPhyPortSmiRegisterRead function with */
            /* non-NULL dataPtr and phyReg [12]. Expected: GT_OK and    */
            /* dataPtr [234] for non 10Gbps port;                       */
            /* GT_NOT_SUPPORTED for 10Gbps port.                        */

            st = cpssDxChPhyPortSmiRegisterRead(dev, port, phyReg, &retData);

/* Next is valid for ExMx, but is it valid for DxCh?
#ifdef ASIC_SIMULATION
simulation not support the "read" actions to return values other then 0
            retData = data;
#endif  ASIC_SIMULATION*/

#ifdef ASIC_SIMULATION
            retData = data;
#endif  /*ASIC_SIMULATION*/

            if (IS_PHY_SMI_IF_SUPPORTED(dev,port))
            {/* non 10Gbps */
                if (0 == IS_XG_PORT_SUPPORT_MAC(dev,port))
                {
                    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                     "cpssDxChPhyPortSmiRegisterRead: non 10Gbps port: %d, %d, %d",
                                                 dev, port, phyReg);
                }
                else
                {
                    UTF_VERIFY_EQUAL3_STRING_MAC(GT_NOT_INITIALIZED, st,
                     "cpssDxChPhyPortSmiRegisterRead: non 10Gbps port: %d, %d, %d",
                                                 dev, port, phyReg);
                }
                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL3_STRING_MAC(data, retData,
                                 "read another data than was written: %d, %d, %d",
                                                 dev, port, phyReg);
                }
            }
            else
            {/* 10Gbps */
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_NOT_SUPPORTED, st,
                 "cpssDxChPhyPortSmiRegisterRead: 10Gbps port: %d, %d", dev, port);
            }

            if (IS_PHY_SMI_IF_SUPPORTED(dev,port))
            {
                /* 1.1.3. Check for out of range SMI register.               */
                /* For non 10Gbps port call function with                    */
                /* phyReg  [32] and data [234]. Expected: NON GT_OK.         */
                phyReg = 32;

                st = cpssDxChPhyPortSmiRegisterWrite(dev, port, phyReg, data);
                if (0 == IS_XG_PORT_SUPPORT_MAC(dev,port))
                {
                    UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyReg);

                    /* restore phy polling mode for port */
                    st = cpssDxChPhyAutonegSmiSet(dev, port, currPhyPollStatus);
                    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                                 "cpssDxChPhyAutonegSmiSet: %d, %d", dev, port);
                }
                else
                {
                    UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_NOT_INITIALIZED, st, dev, port, phyReg);
                }
            }
        }
        phyReg = 22;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhyPortSmiRegisterWrite(dev, port, phyReg, data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhyPortSmiRegisterWrite(dev, port, phyReg, data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    phyReg = 22;
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator - iterate ONLY non-active devices! Because  */
    /* for non-Ch & 10G port we expect "non GT_OK", but not GT_BAD_PARAM   */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyPortSmiRegisterWrite(dev, port, phyReg, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */
    /* phyReg == 22 */

    st = cpssDxChPhyPortSmiRegisterWrite(dev, port, phyReg, data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyPort10GSmiRegisterRead
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyId,
    IN  GT_BOOL   useExternalPhy,
    IN  GT_U16    phyReg,
    IN  GT_U8     phyDev,
    OUT GT_U16    *dataPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyPort10GSmiRegisterRead)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with non-NULL dataPtr and phyId [12],
           useExternalPhy [GT_TRUE and GT_FALSE], phyReg [270], phyDev [20].
    Expected: GT_OK for 10Gbps port;
              GT_NOT_SUPPORTED for non 10Gbps port.
    1.1.2. Check for out of range ID of external 10G PHY.
           For 10Gbps port call function with phyId [32],
           useExternalPhy [GT_TRUE], phyDev [20], phyReg [2],
           and non-NULL dataPtr.
    Expected: NON GT_OK.
    1.1.3. Check for out of range PHY device id. For 10Gbps port call
           function with phyId [12], useExternalPhy [GT_TRUE],
           phyDev [32], phyReg [2], and non-NULL dataPtr.
    Expected: NON GT_OK.
    1.1.4. Check for NULL pointer support. For 10Gbps port call
           function with phyId [12], useExternalPhy [GT_TRUE],
           phyDev [20], phyReg [2], and dataPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   phyId;
    GT_BOOL useExternalPhy;
    GT_U16  phyReg;
    GT_U8   phyDev;
    GT_U16  data;

    CPSS_PP_FAMILY_TYPE_ENT     devFamily;
    PRV_CPSS_PORT_TYPE_ENT      portType;

    /* this function is highly dependent on ASIC family, so lets run if for CH3 only */
    /* TBD: fix useExternalPhy to false for CH3 */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_XCAT_E  | UTF_LION_E | UTF_XCAT2_E);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* get port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            /* 1.1.1. Call function with non-NULL dataPtr and phyId [12],   */
            /* useExternalPhy [GT_TRUE and GT_FALSE], phyDev [20],          */
            /* phyReg [270].                                                */
            /* Expected: GT_OK for 10Gbps port;                             */
            /* GT_NOT_SUPPORTED for non 10Gbps port.                        */
            phyId = 12;
            phyDev = 20;
            phyReg = 270;
            useExternalPhy = GT_TRUE;

            st = cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy,
                                                   phyReg, phyDev, &data);

            UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st,
                                         "Cheetah device, 10Gbps port: %d, %d, %d, %d, %d, %d",
                                         dev, port, phyId, useExternalPhy, phyReg, phyDev);

            useExternalPhy = GT_FALSE;

            st = cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy,
                                                   phyReg, phyDev, &data);

            if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_CHEETAH3_E)
            {
                UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st,
                                             "Cheetah device, 10Gbps port: %d, %d, %d, %d, %d, %d",
                                             dev, port, phyId, useExternalPhy, phyReg, phyDev);
            }
            else
            {
                /* ch3 and above */
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_APPLICABLE_DEVICE, st,
                                             "Cheetah3 and above device , useExternalPhy = GT_FALSE: %d",
                                             dev);
            }

            if (PRV_CPSS_PORT_XG_E == portType)
            {
                /* 1.1.2. Check for out of range ID of external 10G PHY.  */
                /* For 10Gbps port call function with                     */
                /* phyId [32], useExternalPhy [GT_TRUE], phyDev [20],     */
                /* phyReg [2], and non-NULL dataPtr. Expected: NON GT_OK. */
                phyId = 32;
                useExternalPhy = GT_TRUE;
                phyDev = 20;
                phyReg = 2;

                st = cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy,
                                                       phyReg, phyDev, &data);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyId);

                /* 1.1.3. Check for out of range PHY device id.           */
                /* For 10Gbps port call function with                     */
                /* phyId [12], useExternalPhy [GT_TRUE], phyDev [32],     */
                /* phyReg [2], and non-NULL dataPtr. Expected: NON GT_OK. */
                phyId = 12;
                useExternalPhy = GT_TRUE;
                phyDev = 32;
                phyReg = 2;

                st = cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy,
                                                       phyReg, phyDev, &data);
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                                 "%d, %d, phyDev = %d", dev, port, phyDev);

                /* 1.1.4. Check for NULL pointer support.                   */
                /* For 10Gbps port call function with                       */
                /* phyId [12], useExternalPhy [GT_TRUE], phyDev [20],       */
                /* phyReg [2], and dataPtr [NULL]. Expected: GT_BAD_PTR.    */
                phyId = 12;
                useExternalPhy = GT_TRUE;
                phyDev = 20;
                phyReg = 2;

                st = cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy,
                                                       phyReg, phyDev, NULL);
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                                                 "%d, %d, dataPtr = NULL", dev, port);
            }
        }

        phyId = 12;
        useExternalPhy = GT_TRUE;
        phyDev = 20;
        phyReg = 2;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy,
                                                   phyReg, phyDev, &data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy,
                                               phyReg, phyDev, &data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    phyId = 12;
    useExternalPhy = GT_TRUE;
    phyDev = 20;
    phyReg = 2;
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy,
                                               phyReg, phyDev, &data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */

    st = cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy,
                                           phyReg, phyDev, &data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyPort10GSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyId,
    IN  GT_BOOL   useExternalPhy,
    IN  GT_U16    phyReg,
    IN  GT_U8     phyDev,
    IN  GT_U16    data
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyPort10GSmiRegisterWrite)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with dataPtr [56] and phyId [12],
           useExternalPhy [GT_TRUE and GT_FALSE], phyDev [20], phyReg [270].
    Expected: GT_OK for Cheetah devices and 10Gbps port;
              GT_NOT_SUPPORTED for Cheetah device and non 10Gbps port.
    1.1.2. Call function cpssDxChPhyPort10GSmiRegisterRead with non-NULL dataPtr
           and phyId [12], useExternalPhy [GT_TRUE and GT_FALSE],
           phyDev [20], phyReg [270].
    Expected: GT_OK  and dataPtr [56] for 10Gbps port;
              GT_NOT_SUPPORTED for non 10Gbps port.
    1.1.3. Check for out of range ID of external 10Gbps PHY. For 10Gbps
           port call function with phyId [32], useExternalPhy [GT_TRUE],
           phyDev [20], phyReg [2], and non-NULL dataPtr.
    Expected: NON GT_OK.
    1.1.4. Check for out of range PHY device id. For 10Gbps port call
           function with phyId [12], useExternalPhy [GT_TRUE], phyDev [32],
           phyReg [2], and non-NULL dataPtr.
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   port;
    GT_U8   phyId;
    GT_BOOL useExternalPhy;
    GT_U16  phyReg;
    GT_U8   phyDev;
    GT_U16  data;

    GT_U16  retData;

    CPSS_PP_FAMILY_TYPE_ENT     devFamily;
    PRV_CPSS_PORT_TYPE_ENT      portType;

    data = 0x800a;

    /* this function is highly dependent on ASIC family, so lets run if for CH1-2 only */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_DIAMOND_E | UTF_CH3_E | UTF_XCAT_E  | UTF_LION_E | UTF_XCAT2_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d", dev);

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* get port type */
            st = prvUtfPortTypeGet(dev, port, &portType);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "prvUtfPortTypeGet: %d, %d", dev, port);

            /* no sence to write to not existing registers */
            if (PRV_CPSS_PORT_XG_E != portType)
                continue;

            /* 1.1.1. Call function with dataPtr [56] and phyId [12],       */
            /* useExternalPhy [GT_TRUE and GT_FALSE], phyDev [20],          */
            /* phyReg [270]. Expected:                                      */
            /* GT_OK for 10Gbps port;                                       */
            /* GT_NOT_SUPPORTED for Cheetah device and non 10Gbps port and; */
            /* GT_BAD_PARAM for 10Gbps port;                                */

            /* 1.1.2. Call function cpssDxChPhyPort10GSmiRegisterRead       */
            /* with non-NULL dataPtr and phyId [12],                        */
            /* useExternalPhy [GT_TRUE and GT_FALSE], phyDev [20],          */
            /* phyReg [270]. Expected:                                      */
            /* GT_OK  and dataPtr [56] for 10Gbps port;                     */
            /* GT_NOT_SUPPORTED for non 10Gbps port                         */
            phyId = 0;
            phyDev = 5;
            phyReg = 0x8000;

            /* it's impossible to build generic test for any type of external PHY */
            useExternalPhy = GT_FALSE; 

            st = cpssDxChPhyPort10GSmiRegisterWrite(dev, port, phyId, useExternalPhy,
                                                   phyReg, phyDev, data);

            UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st,
                        "Cheetah device, 10Gbps port: %d, %d, %d, %d, %d, %d",
                        dev, port, phyId, useExternalPhy, phyReg, phyDev);

            /* 1.1.2. for useExternalPhy == GT_TRUE */
            st = cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy,
                                                   phyReg, phyDev, &retData);
/* Next is valid for ExMx, but is it valid for DxCh? */
#ifdef ASIC_SIMULATION
/* simulation not support the "read" actions to return values other then 0 */
            retData = data;
#endif  /*ASIC_SIMULATION*/

            UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st,
            "cpssDxChPhyPort10GSmiRegisterRead: Cheetah device, 10Gbps port: %d, %d, %d, %d, %d, %d",
                      dev, port, phyId, useExternalPhy, phyReg, phyDev);
            if (GT_OK == st)
            {
                UTF_VERIFY_EQUAL6_STRING_MAC(data, retData,
                "read another data than was written: %d, %d, %d, %d, %d, %d",
                           dev, port, phyId, useExternalPhy, phyReg, phyDev);
            }

            /* 1.1.1 for useExternalPhy == GT_FALSE */
            useExternalPhy = GT_FALSE;

            st = cpssDxChPhyPort10GSmiRegisterWrite(dev, port, phyId, useExternalPhy,
                                                   phyReg, phyDev, data);

            if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_CHEETAH3_E)
            {
                UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st,
                     "Cheetah device, 10Gbps port: %d, %d, %d, %d, %d, %d",
                         dev, port, phyId, useExternalPhy, phyReg, phyDev);
            }
            else
            {
                /* ch3 and above */
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_APPLICABLE_DEVICE, st,
                                             "Cheetah3 and above device , useExternalPhy = GT_FALSE: %d",
                                             dev);
            }

            /* 1.1.2. for useExternalPhy == GT_FALSE */
            st = cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy,
                                                   phyReg, phyDev, &retData);
/* Next is valid for ExMx, but is it valid for DxCh? */
#ifdef ASIC_SIMULATION
/* simulation not support the "read" actions to return values other then 0 */
            retData = data;
#endif    /*ASIC_SIMULATION*/

            if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_CHEETAH3_E)
            {
                UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st,
                "cpssDxChPhyPort10GSmiRegisterRead: Cheetah device, 10Gbps port: %d, %d, %d, %d, %d, %d",
                           dev, port, phyId, useExternalPhy, phyReg, phyDev);
                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL6_STRING_MAC(data, retData,
                    "read another data than was written: %d, %d, %d, %d, %d, %d",
                            dev, port, phyId, useExternalPhy, phyReg, phyDev);
                }
            }
            else
            {
                /* ch3 and above */
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_NOT_APPLICABLE_DEVICE, st,
                                             "Cheetah3 and above device , useExternalPhy = GT_FALSE: %d",
                                             dev);
            }

            /* 1.1.2. Check for out of range ID of external 10G PHY.  */
            /* For 10Gbps port call function with                     */
            /* phyId [32], useExternalPhy [GT_TRUE], phyDev [20],     */
            /* phyReg [2], and data [56]. Expected: NON GT_OK.        */
            phyId = 32;
            useExternalPhy = GT_TRUE;
            phyDev = 5;
            phyReg = 0x8000;

            st = cpssDxChPhyPort10GSmiRegisterWrite(dev, port, phyId, useExternalPhy,
                                                   phyReg, phyDev, data);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, phyId);

            /* 1.1.3. Check for out of range PHY device id.           */
            /* For Cheetah device and 10Gbps port call function with  */
            /* phyId [12], useExternalPhy [GT_TRUE], phyDev [32],     */
            /* phyReg [2], and data [56]. Expected: NON GT_OK.        */
            phyId = 0;
            useExternalPhy = GT_FALSE;
            phyDev = 32;
            phyReg = 0x8000;

            st = cpssDxChPhyPort10GSmiRegisterWrite(dev, port, phyId, useExternalPhy,
                                                   phyReg, phyDev, data);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                             "%d, %d, phyDev = %d", dev, port, phyDev);
        }

        phyId = 0;
        useExternalPhy = GT_FALSE;
        phyDev = 5;
        phyReg = 0x8000;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhyPort10GSmiRegisterWrite(dev, port, phyId, useExternalPhy,
                                                   phyReg, phyDev, data);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhyPort10GSmiRegisterWrite(dev, port, phyId, useExternalPhy,
                                               phyReg, phyDev, data);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    phyId = 0;
    useExternalPhy = GT_FALSE;
    phyDev = 5;
    phyReg = 0x8000;
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator - iterate ONLY non-active devices! Because  */
    /* for non-Ch & 10G port we expect "non GT_OK", but not GT_BAD_PARAM   */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyPort10GSmiRegisterWrite(dev, port, phyId, useExternalPhy,
                                               phyReg, phyDev, data);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */

    st = cpssDxChPhyPort10GSmiRegisterWrite(dev, port, phyId, useExternalPhy,
                                           phyReg, phyDev, data);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyAutonegSmiGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enabledPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyAutonegSmiGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Check for NULL pointer support.  Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;

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
            /* 1.1.1. Call function with non-NULL enablePtr. */
            /* Expected: GT_OK                               */
            st = cpssDxChPhyAutonegSmiGet(dev, port, &enable);

            if (0 == IS_XG_PORT_SUPPORT_MAC(dev,port))
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "non 10Gbps port: %d, %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                        "10Gbps port: %d, %d", dev, port);
            }

            /* 1.1.2. Check for NULL pointer support.                     */
            /* call function with enablePtr [NULL]. Expected: GT_BAD_PTR. */
            st = cpssDxChPhyAutonegSmiGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhyAutonegSmiGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhyAutonegSmiGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyAutonegSmiGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */

    st = cpssDxChPhyAutonegSmiGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyAutonegSmiSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL enabled
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyAutonegSmiSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK for Cheetah devices.
    1.1.2. Call function cpssDxChPhyAutonegSmiGet with non-NULL enablePtr.
    Expected: GT_OK and enablePtr the same as was set.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     enable;

    GT_BOOL     retEnable;

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
            /* 1.1.1. Call function with enable [GT_TRUE; GT_FALSE]. */
            /* Expected: GT_OK for Cheetah devices;                  */
            /* GT_BAD_PARAM for non-Cheetah device.                  */

            /* 1.1.2. Call function cpssDxChPhyAutonegSmiGet            */
            /* with non-NULL enablePtr. Expected: GT_OK and enablePtr   */
            /* the same as was set for Cheetah devices;                 */
            /* GT_BAD_PARAM for non-Cheetah device.                     */

            enable = GT_TRUE;

            st = cpssDxChPhyAutonegSmiSet(dev, port, enable);

            if (0 == IS_XG_PORT_SUPPORT_MAC(dev,port))
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                        "device: %d, %d, %d", dev, port, enable);

                /* 1.1.2. for enable == GT_TRUE */
                st = cpssDxChPhyAutonegSmiGet(dev, port, &retEnable);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "cpssDxChPhyAutonegSmiGet: Cheetah device: %d, %d",
                                             dev, port);
                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(enable, retEnable,
                         "get another <enable> value than was set: %d, %d",
                                                 dev, port);
                }
            }
            else
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                        "10Gbps port: %d, %d", dev, port);
            }

            /*1.1.1. with enable == GT_FALSE */
            enable = GT_FALSE;

            st = cpssDxChPhyAutonegSmiSet(dev, port, enable);

            if (0 == IS_XG_PORT_SUPPORT_MAC(dev,port))
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                        "Cheetah device: %d, %d, %d", dev, port, enable);

                /* 1.1.2. for enable == GT_FALSE */
                st = cpssDxChPhyAutonegSmiGet(dev, port, &retEnable);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                        "cpssDxChPhyAutonegSmiGet: Cheetah device: %d, %d",
                                             dev, port);
                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(enable, retEnable,
                        "get another <enable> value than was set: %d, %d",
                                                 dev, port);
                }
            }
            else
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                        "10Gbps port: %d, %d", dev, port);
            }
        }

        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhyAutonegSmiSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhyAutonegSmiSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    enable = GT_TRUE;
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyAutonegSmiSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */

    st = cpssDxChPhyAutonegSmiSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhySmiAutoMediaSelectSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   autoMediaSelect

)
*/
UTF_TEST_CASE_MAC(cpssDxChPhySmiAutoMediaSelectSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with autoMediaSelect [0x21].
    Expected: GT_OK.
    1.1.2. Call function cpssDxChPhySmiAutoMediaSelectGet
           with non-NULL autoMediaSelectPtr.
    Expected: GT_OK and autoMediaSelectPtr [0x21].
    1.1.3. Check for out of range autoMediaSelect. Call
           function with autoMediaSelect [0x41].
    Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U32      autoMediaSelect;

    GT_U32      retAutoMediaSelect;

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
            /* 1.1.1. Call function with autoMediaSelect [0x21]. */
            /* Expected: GT_OK */
            autoMediaSelect = 0x21;

            st = cpssDxChPhySmiAutoMediaSelectSet(dev, port, autoMediaSelect);

            if (0 == IS_XG_PORT_SUPPORT_MAC(dev,port))
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "Cheetah device: %d, %d, %d",
                                             dev, port, autoMediaSelect);

                /* 1.1.2. Call function cpssDxChPhySmiAutoMediaSelectGet    */
                /* with non-NULL autoMediaSelectPtr. Expected:              */
                /* GT_OK and autoMediaSelectPtr [0x21] */
                st = cpssDxChPhySmiAutoMediaSelectGet(dev, port, &retAutoMediaSelect);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssDxChPhySmiAutoMediaSelectGet: Cheetah device: %d, %d", dev, port);
                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(autoMediaSelect, retAutoMediaSelect,
                    "get another <autoMediaSelect> value than was set: %d, %d", dev, port);
                }
            }
            else
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                        "10Gbps port: %d, %d", dev, port);
            }

            /* 1.1.3. Check for out of range autoMediaSelect.    */
            /* Call function with autoMediaSelect [0x41].        */
            /* Expected: NON GT_OK.                              */
            autoMediaSelect = 0x41;

            st = cpssDxChPhySmiAutoMediaSelectSet(dev, port, autoMediaSelect);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, autoMediaSelect);
        }

        autoMediaSelect = 0x21;

        /* 1.2. For all active devices go over all non available physical ports. */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhySmiAutoMediaSelectSet(dev, port, autoMediaSelect);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhySmiAutoMediaSelectSet(dev, port, autoMediaSelect);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    autoMediaSelect = 0x21;
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhySmiAutoMediaSelectSet(dev, port, autoMediaSelect);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */

    st = cpssDxChPhySmiAutoMediaSelectSet(dev, port, autoMediaSelect);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhySmiAutoMediaSelectGet
(

    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   *autoMediaSelectPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhySmiAutoMediaSelectGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with non-NULL autoMediaSelectPtr.
    Expected: GT_OK.
    1.1.2. Check for NULL pointer support. Call function
           with autoMediaSelectPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U32      autoMediaSelect;

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
            /* 1.1.1. Call function with non-NULL autoMediaSelectPtr.   */
            /* Expected: GT_OK */
            st = cpssDxChPhySmiAutoMediaSelectGet(dev, port, &autoMediaSelect);

            if (0 == IS_XG_PORT_SUPPORT_MAC(dev,port))
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Cheetah device: %d, %d",
                                             dev, port);

                /* 1.1.2. Check for NULL pointer support.                    */
                /* Call function with autoMediaSelectPtr [NULL].             */
                /* Expected: GT_BAD_PTR.                                     */
                st = cpssDxChPhySmiAutoMediaSelectGet(dev, port, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                             "%d, %d, NULL", dev, port);
            }
            else
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                        "10Gbps port: %d, %d", dev, port);
            }
        }

        /* 1.2. For all active devices go over all non available physical ports. */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhySmiAutoMediaSelectGet(dev, port, &autoMediaSelect);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhySmiAutoMediaSelectGet(dev, port, &autoMediaSelect);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhySmiAutoMediaSelectGet(dev, port, &autoMediaSelect);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */

    st = cpssDxChPhySmiAutoMediaSelectGet(dev, port, &autoMediaSelect);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyPortSmiInit
(
    IN GT_U8    devNum,
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyPortSmiInit)
{
/*
    ITERATE_DEVICES_PHY_PORTS
    1.1.1. Call function with non-NULL autoMediaSelectPtr.
    Expected: GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.1. Call function with non-NULL autoMediaSelectPtr.   */
        /* Expected: GT_OK */
        st = cpssDxChPhyPortSmiInit(dev);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_NONE_FAMILY_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyPortSmiInit(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPhyPortSmiInit(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyPortSmiInterfaceSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  CPSS_PHY_SMI_INTERFACE_ENT  smiInterface
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyPortSmiInterfaceSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS(xCat and above)
    1.1.1. Call function with smiInterface [CPSS_PHY_SMI_INTERFACE_0_E /
                                            CPSS_PHY_SMI_INTERFACE_1_E].
    Expected: GT_OK and GT_NOT_SUPPORTED - for non flex ports
    1.1.2. Call get function with the same params.
    Expected: GT_OK and the same smiInterface.
    1.1.3. Call with wrong enum values smiInterface.
    Expected: GT_BAD_PARAM and GT_NOT_SUPPORTED - for non flex ports.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    CPSS_PHY_SMI_INTERFACE_ENT  smiInterface = 0;
    CPSS_PHY_SMI_INTERFACE_ENT  smiInterfaceGet = 2;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with smiInterface [CPSS_PHY_SMI_INTERFACE_0_E /
                                                        CPSS_PHY_SMI_INTERFACE_1_E].
                Expected: GT_OK and GT_NOT_SUPPORTED - for non flex ports.
            */

            /*call with  smiInterface = CPSS_PHY_SMI_INTERFACE_0_E; */
            smiInterface = CPSS_PHY_SMI_INTERFACE_0_E;

            st = cpssDxChPhyPortSmiInterfaceSet(dev, port, smiInterface);

            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call get function with the same params.
                Expected: GT_OK and the same smiInterface.
            */

            st = cpssDxChPhyPortSmiInterfaceGet(dev, port, &smiInterfaceGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(smiInterface, smiInterfaceGet,
                 "get another <smiInterface> value than was set: %d, %d", dev, port);


            /*call with  smiInterface = CPSS_PHY_SMI_INTERFACE_1_E; */

            smiInterface = CPSS_PHY_SMI_INTERFACE_1_E;

            st = cpssDxChPhyPortSmiInterfaceSet(dev, port, smiInterface);

            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call get function with the same params.
                Expected: GT_OK and the same smiInterface.
            */

            st = cpssDxChPhyPortSmiInterfaceGet(dev, port, &smiInterfaceGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            UTF_VERIFY_EQUAL2_STRING_MAC(smiInterface, smiInterfaceGet,
                 "get another <smiInterface> value than was set: %d, %d", dev, port);


            /*
                1.1.3. Call with wrong enum values smiInterface.
                Expected: GT_BAD_PARAM
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPhyPortSmiInterfaceSet
                                (dev, port, smiInterface),
                                smiInterface);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhyPortSmiInterfaceSet(dev, port, smiInterface);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhyPortSmiInterfaceSet(dev, port, smiInterface);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyPortSmiInterfaceSet(dev, port, smiInterface);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */

    st = cpssDxChPhyPortSmiInterfaceSet(dev, port, smiInterface);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyPortSmiInterfaceGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT CPSS_PHY_SMI_INTERFACE_ENT  *smiInterfacePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyPortSmiInterfaceGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS(xCat and above)
    1.1.1. Call function with  not null smiInterface.
    Expected: GT_OK and GT_NOT_SUPPORTED - for non flex ports
    1.1.2. Call with wrong smiInterface [NULL].
    Expected: GT_BAD_PTR and GT_NOT_SUPPORTED - for non flex ports.
*/
    GT_STATUS st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    CPSS_PHY_SMI_INTERFACE_ENT  smiInterface = 0;
    CPSS_PHY_SMI_INTERFACE_ENT  smiInterfaceGet = 2;

    /* prepare iterator for go over all active devices */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call function with  not null smiInterface.
                Expected: GT_OK and GT_NOT_SUPPORTED - for non flex ports.
            */

            st = cpssDxChPhyPortSmiInterfaceSet(dev, port, smiInterface);

            st = cpssDxChPhyPortSmiInterfaceGet(dev, port, &smiInterfaceGet);

            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with wrong smiInterface [NULL].
                Expected: GT_BAD_PTR and GT_NOT_SUPPORTED - for non flex ports.
            */

            st = cpssDxChPhyPortSmiInterfaceGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PTR, st, dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxChPhyPortSmiInterfaceGet(dev, port, &smiInterfaceGet);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxChPhyPortSmiInterfaceGet(dev, port, &smiInterfaceGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = PHY_SMI_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyPortSmiInterfaceGet(dev, port, &smiInterfaceGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == 0    */

    st = cpssDxChPhyPortSmiInterfaceGet(dev, port, &smiInterfaceGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhySmiMdcDivisionFactorGet
(
    IN  GT_U8                                 dev,
    IN  GT_U8                                 portNum,
    OUT CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT  *divisionFactorPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhySmiMdcDivisionFactorGet)
{
/*
    ITERATE_DEVICES_PHY_PORTS(DxChXcat and above)
    1.1. Call with not null divisionFactor pointer.
    Expected: GT_OK.
    1.2. Call api with wrong divisionFactorPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                                 dev;
    GT_U8                                 portNum = 0;
    CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT  divisionFactor;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&portNum, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&portNum, GT_TRUE))
        {
            /*
                1.1. Call with not null divisionFactor pointer.
                Expected: GT_OK.
            */

            st = cpssDxChPhySmiMdcDivisionFactorGet(dev, portNum, &divisionFactor);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call api with wrong divisionFactorPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssDxChPhySmiMdcDivisionFactorGet(dev, portNum, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, 
                                       "%d, divisionFactorPtr = NULL", dev);
        }

        st = prvUtfNextPhyPortReset(&portNum, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        if (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
        {/* in Lion it's global parameter - no check for portNum */
            /* 1.2. For all active devices go over all non available physical ports. */
            while (GT_OK == prvUtfNextPhyPortGet(&portNum, GT_FALSE))
            {
                /* 1.2.1. Call function for each non-active portNum */
                st = cpssDxChPhySmiMdcDivisionFactorGet(dev, portNum, &divisionFactor);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
            }

            /* 1.3. For active device check that function returns GT_BAD_PARAM */
            /* for out of bound value for portNum number.                         */
            portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
    
            st = cpssDxChPhySmiMdcDivisionFactorGet(dev, portNum, &divisionFactor);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
        }
    }

    /* restore correct values */
    portNum = 0;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhySmiMdcDivisionFactorGet(dev, portNum, &divisionFactor);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPhySmiMdcDivisionFactorGet(dev, portNum, &divisionFactor);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhySmiMdcDivisionFactorSet
(
    IN  GT_U8                                dev,
    IN  GT_U8                                portNum,
    IN  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactor
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhySmiMdcDivisionFactorSet)
{
/*
    ITERATE_DEVICES_PHY_PORTS(DxChXcat and above)
    1.1. Call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E /
                                  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E /
                                  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E /
                                  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E /
                                  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E /
                                  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E],
    Expected: GT_OK.
    1.2. Call cpssDxChPhySmiMdcDivisionFactorGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong divisionFactor [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                                dev;
    GT_U8                                portNum = 0;
    CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E;
    CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactorGet = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&portNum, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&portNum, GT_TRUE))
        {
            /*
                1.1. Call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E /
                                              CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E /
                                              CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E /
                                              CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E /
                                              CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E /
                                              CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E],
                Expected: GT_OK.
            */

            /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E] */
            divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E;

            st = cpssDxChPhySmiMdcDivisionFactorSet(dev, portNum, divisionFactor);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, portNum);

            /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E] */
            divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E;

            st = cpssDxChPhySmiMdcDivisionFactorSet(dev, portNum, divisionFactor);
            if ((PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
                || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portNum);

                /*
                    1.2. Call cpssDxChPhySmiMdcDivisionFactorGet
                           with the same parameters.
                    Expected: GT_OK and the same values than was set.
                */
                st = cpssDxChPhySmiMdcDivisionFactorGet(dev, portNum, &divisionFactorGet);
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                          "cpssDxChPhySmiMdcDivisionFactorGet: %d ", dev);

                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(divisionFactor, divisionFactorGet, 
                               "got another divisionFactor then was set: %d", dev);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, portNum);
            }

            /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E] */
            divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E;

            st = cpssDxChPhySmiMdcDivisionFactorSet(dev, portNum, divisionFactor);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portNum);

            /*
                1.2. Call cpssDxChPhySmiMdcDivisionFactorGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChPhySmiMdcDivisionFactorGet(dev, portNum, &divisionFactorGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPhySmiMdcDivisionFactorGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(divisionFactor, divisionFactorGet, 
                           "got another divisionFactor then was set: %d", dev);

            /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E] */
            divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E;

            st = cpssDxChPhySmiMdcDivisionFactorSet(dev, portNum, divisionFactor);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portNum);

            /*
                1.2. Call cpssDxChPhySmiMdcDivisionFactorGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChPhySmiMdcDivisionFactorGet(dev, portNum, &divisionFactorGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPhySmiMdcDivisionFactorGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(divisionFactor, divisionFactorGet, 
                           "got another divisionFactor then was set: %d", dev);

            /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E] */
            divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E;

            st = cpssDxChPhySmiMdcDivisionFactorSet(dev, portNum, divisionFactor);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portNum);

            /*
                1.2. Call cpssDxChPhySmiMdcDivisionFactorGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChPhySmiMdcDivisionFactorGet(dev, portNum, &divisionFactorGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPhySmiMdcDivisionFactorGet: %d ", dev);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(divisionFactor, divisionFactorGet, 
                           "got another divisionFactor then was set: %d", dev);

            /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E] */
            divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E;

            st = cpssDxChPhySmiMdcDivisionFactorSet(dev, portNum, divisionFactor);
            if (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portNum);

                /*
                    1.2. Call cpssDxChPhySmiMdcDivisionFactorGet
                           with the same parameters.
                    Expected: GT_OK and the same values than was set.
                */
                st = cpssDxChPhySmiMdcDivisionFactorGet(dev, portNum, &divisionFactorGet);
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                          "cpssDxChPhySmiMdcDivisionFactorGet: %d ", dev);

                /* Verifying values */
                UTF_VERIFY_EQUAL1_STRING_MAC(divisionFactor, divisionFactorGet, 
                               "got another divisionFactor then was set: %d", dev);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, portNum);
            }

            /*
                1.3. Call api with wrong divisionFactor [wrong enum values].
                Expected: GT_BAD_PARAM.
            */
            UTF_ENUMS_CHECK_MAC(cpssDxChPhySmiMdcDivisionFactorSet
                                (dev, portNum, divisionFactor),
                                divisionFactor);
        }

        st = prvUtfNextPhyPortReset(&portNum, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* 1.2. For all active devices go over all non available physical ports. */
            while (GT_OK == prvUtfNextPhyPortGet(&portNum, GT_FALSE))
            {
                /* 1.2.1. Call function for each non-active portNum */
                st = cpssDxChPhySmiMdcDivisionFactorSet(dev, portNum, divisionFactor);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
            }
    
            /* 1.3. For active device check that function returns GT_BAD_PARAM */
            /* for out of bound value for portNum number.                         */
            portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
    
            st = cpssDxChPhySmiMdcDivisionFactorSet(dev, portNum, divisionFactor);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portNum);
        }
    }

    /* restore correct values */
    divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E;
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhySmiMdcDivisionFactorSet(dev, portNum, divisionFactor);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPhySmiMdcDivisionFactorSet(dev, portNum, divisionFactor);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyXsmiMdcDivisionFactorGet
(
    IN  GT_U8                                 dev,
    OUT CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT  *divisionFactorPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyXsmiMdcDivisionFactorGet)
{
/*
    ITERATE_DEVICES(DxChXcat and above)
    1.1. Call with not null divisionFactorPtr.
    Expected: GT_OK.
    1.2. Call api with wrong divisionFactorPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                                 dev;
    CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT  divisionFactor;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with not null divisionFactorPtr.
            Expected: GT_OK.
        */
        st = cpssDxChPhyXsmiMdcDivisionFactorGet(dev, &divisionFactor);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call api with wrong divisionFactorPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxChPhyXsmiMdcDivisionFactorGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, 
                                   "%d, divisionFactorPtr = NULL", dev);
    }
     
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyXsmiMdcDivisionFactorGet(dev, &divisionFactor);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPhyXsmiMdcDivisionFactorGet(dev, &divisionFactor);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxChPhyXsmiMdcDivisionFactorSet
(
    IN  GT_U8                                dev,
    IN  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactor
)
*/
UTF_TEST_CASE_MAC(cpssDxChPhyXsmiMdcDivisionFactorSet)
{
/*
    ITERATE_DEVICES(DxChXcat and above)
    1.1. Call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E /
                                  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E /
                                  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E /
                                  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E /
                                  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E /
                                  CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E],
    Expected: GT_OK.
    1.2. Call cpssDxChPhyXsmiMdcDivisionFactorGet
           with the same parameters.
    Expected: GT_OK and the same values than was set.
    1.3. Call api with wrong divisionFactor [wrong enum values].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                                dev;
    CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E;
    CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactorGet = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);
    
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E /
                                          CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E /
                                          CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E /
                                          CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E /
                                          CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E /
                                          CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E],
            Expected: GT_OK.
        */

        /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E] */
        divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E;

        st = cpssDxChPhyXsmiMdcDivisionFactorSet(dev, divisionFactor);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
            || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /*
                1.2. Call cpssDxChPhyXsmiMdcDivisionFactorGet
                       with the same parameters.
                Expected: GT_OK and the same values than was set.
            */
            st = cpssDxChPhyXsmiMdcDivisionFactorGet(dev, &divisionFactorGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPhyXsmiMdcDivisionFactorGet: %d ", dev);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(divisionFactor, divisionFactorGet, 
                           "got another divisionFactor then was set: %d", dev);
        }

        /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E] */
        divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E;

        st = cpssDxChPhyXsmiMdcDivisionFactorSet(dev, divisionFactor);
        if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call get func again, 1.2. */
            st = cpssDxChPhyXsmiMdcDivisionFactorGet(dev, &divisionFactorGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPhyXsmiMdcDivisionFactorGet: %d ", dev);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(divisionFactor, divisionFactorGet, 
                           "got another divisionFactor then was set: %d", dev);
        }
        
        /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E] */
        divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E;

        st = cpssDxChPhyXsmiMdcDivisionFactorSet(dev, divisionFactor);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
            || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call get func again, 1.2. */
            st = cpssDxChPhyXsmiMdcDivisionFactorGet(dev, &divisionFactorGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPhyXsmiMdcDivisionFactorGet: %d ", dev);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(divisionFactor, divisionFactorGet, 
                           "got another divisionFactor then was set: %d", dev);
        }

        /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E] */
        divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E;

        st = cpssDxChPhyXsmiMdcDivisionFactorSet(dev, divisionFactor);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
            || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call get func again, 1.2. */
            st = cpssDxChPhyXsmiMdcDivisionFactorGet(dev, &divisionFactorGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPhyXsmiMdcDivisionFactorGet: %d ", dev);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(divisionFactor, divisionFactorGet, 
                           "got another divisionFactor then was set: %d", dev);
        }

        /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E] */
        divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E;

        st = cpssDxChPhyXsmiMdcDivisionFactorSet(dev, divisionFactor);
        if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call get func again, 1.2. */
            st = cpssDxChPhyXsmiMdcDivisionFactorGet(dev, &divisionFactorGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPhyXsmiMdcDivisionFactorGet: %d ", dev);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(divisionFactor, divisionFactorGet, 
                           "got another divisionFactor then was set: %d", dev);
        }

        /* call with divisionFactor[CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E] */
        divisionFactor = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E;

        st = cpssDxChPhyXsmiMdcDivisionFactorSet(dev, divisionFactor);
        if ((PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
            || (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
        }
        else
        {
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

            /* call get func again, 1.2. */
            st = cpssDxChPhyXsmiMdcDivisionFactorGet(dev, &divisionFactorGet);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                      "cpssDxChPhyXsmiMdcDivisionFactorGet: %d ", dev);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(divisionFactor, divisionFactorGet, 
                           "got another divisionFactor then was set: %d", dev);
        }

        /*
            1.3. Call api with wrong divisionFactor [wrong enum values].
            Expected: GT_BAD_PARAM.
        */
        UTF_ENUMS_CHECK_MAC(cpssDxChPhyXsmiMdcDivisionFactorSet
                            (dev, divisionFactor),
                            divisionFactor);
    }
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&dev, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxChPhyXsmiMdcDivisionFactorSet(dev, divisionFactor);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }
     
    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxChPhyXsmiMdcDivisionFactorSet(dev, divisionFactor);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
 * Configuration of cppDxChPhySmi suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxChPhySmi)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyPortAddrSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyPortAddrGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyPortSmiRegisterRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyPortSmiRegisterWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyPort10GSmiRegisterRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyPort10GSmiRegisterWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyAutonegSmiGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyAutonegSmiSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhySmiAutoMediaSelectSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhySmiAutoMediaSelectGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyPortSmiInit)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyPortSmiInterfaceSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyPortSmiInterfaceGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhySmiMdcDivisionFactorGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhySmiMdcDivisionFactorSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyXsmiMdcDivisionFactorGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxChPhyXsmiMdcDivisionFactorSet)
UTF_SUIT_END_TESTS_MAC(cpssDxChPhySmi)


