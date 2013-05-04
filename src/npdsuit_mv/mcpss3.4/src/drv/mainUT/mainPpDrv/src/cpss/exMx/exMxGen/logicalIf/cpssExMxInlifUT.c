/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxInlifUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxInlif.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>

#include <cpss/exMx/exMxGen/logicalIf/cpssExMxInlif.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Tests use this value as default valid value for port */
#define INLIF_VALID_VIRT_PORT_CNS      0

/* Invalid enum */
#define INLIF_INVALID_ENUM_CNS         0x5AAAAAA5

/* Macro to define is device from Twist or Samba family         */
/* devFamily  - family of device (CPSS_PP_FAMILY_TYPE_ENT type) */
#define IS_TWIST_OR_SAMBA_MAC(devFamily) ((CPSS_PP_FAMILY_TIGER_E != (devFamily)) ? GT_TRUE : GT_FALSE)


/* Internal functions forward declaration */
static GT_STATUS prvUtfMaxNumOfLifsGet(IN GT_U8 dev,
                                       OUT GT_U32 *maxNumOfLifsPtr);
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxInlifPortEntryGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_U32      *inlifHwDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxInlifPortEntryGet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with non-NULL inlifHwDataPtr.
    Expected: GT_OK.
1.1.2. Call function with NULL inlifHwDataPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U32      inlifHwData[4];


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with non-NULL inlifHwDataPtr.   */
            /* Expected: GT_OK.                                     */

            st = cpssExMxInlifPortEntryGet(dev, port, inlifHwData);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call function with inlifHwDataPtr [NULL].    */
            /* Expected: GT_BAD_PTR.                               */

            st = cpssExMxInlifPortEntryGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL",
                                         dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxInlifPortEntryGet(dev, port, inlifHwData);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxInlifPortEntryGet(dev, port, inlifHwData);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = INLIF_VALID_VIRT_PORT_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssExMxInlifPortEntryGet(dev, port, inlifHwData);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxInlifPortEntryGet(dev, port, inlifHwData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxInlifPortEntrySet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    IN  GT_U32      *inlifHwDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxInlifPortEntrySet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with inlifHwDataPtr [{1234, 0, 0, 5678}].
    Expected: GT_OK.
1.1.2. Call function cpssExMxInlifPortEntryGet
    with non-NULL inlifHwDataPtr.
    Expected: GT_OK and inlifHwData [{1234, 0, 0, 5678}].
1.1.3. Call function with inlifHwDataPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_U32      inlifHwData[4];

    GT_U32      retInlifHwData[4];
    GT_BOOL     failureWas;

    cpssOsBzero((GT_VOID*)inlifHwData, sizeof(inlifHwData[0])*4);
    inlifHwData[0] = 1234;
    inlifHwData[3] = 5678;
    /* inlifHwData == {1234, 0, 0, 5678} */

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with inlifHwDataPtr [{1234, 0, 0, 5678}]. */
            /* Expected: GT_OK.                                               */
            cpssOsBzero((GT_VOID*)inlifHwData, sizeof(inlifHwData[0])*4);
            inlifHwData[0] = 1234;
            inlifHwData[3] = 5678;
            /* inlifHwData == {1234, 0, 0, 5678} */

            st = cpssExMxInlifPortEntrySet(dev, port, inlifHwData);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* restore inlifHwData in case function has overwritten it */
            cpssOsBzero((GT_VOID*)inlifHwData, sizeof(inlifHwData[0])*4);
            inlifHwData[0] = 1234;
            inlifHwData[3] = 5678;
            /* inlifHwData == {1234, 0, 0, 5678} */

            /* 1.1.2. Call function cpssExMxInlifPortEntryGet        */
            /* with non-NULL inlifHwDataPtr.                         */
            /* Expected: GT_OK and inlifHwData [{1234, 0, 0, 5678}]. */

            st = cpssExMxInlifPortEntryGet(dev, port, retInlifHwData);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "cpssExMxInlifPortEntryGet: %d, %d",
                                         dev, port);
            if (GT_OK == st)
            {

                failureWas = (0 == cpssOsMemCmp((const GT_VOID*)inlifHwData,
                                                (const GT_VOID*)retInlifHwData, sizeof(inlifHwData[0])*4)) ? GT_FALSE : GT_TRUE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                                             "get another inlifHwData than has been set [dev = %d, port = %d]",
                                             dev, port);
            }

            /* 1.1.3. Call function with inlifHwDataPtr [NULL].    */
            /* Expected: GT_BAD_PTR.                               */

            st = cpssExMxInlifPortEntrySet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL",
                                         dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxInlifPortEntrySet(dev, port, inlifHwData);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxInlifPortEntrySet(dev, port, inlifHwData);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    port = INLIF_VALID_VIRT_PORT_CNS;

    /*2. Go over all non active devices. */

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssExMxInlifPortEntrySet(dev, port, inlifHwData);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxInlifPortEntrySet(dev, port, inlifHwData);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxInlifPortLookupModeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    IN  CPSS_EXMX_INLIF_PORT_MODE_ENT   inlifMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxInlifPortLookupModeSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with inlifMode
    [CPSS_EXMX_SINGLE_INLIF_E]. Expected: GT_OK for non Ex1x6
    devices and NON GT_OK for Tiger (Ex1x6) devices.
1.1.2. For non-Ex1x6 devices call function 
    cpssExMxInlifPortLookupModeGet with non-NULL
    inlifModePtr. Expected: GT_OK and inlifMode 
    [CPSS_EXMX_SINGLE_INLIF_E].
1.1.3. Call function with inlifMode [CPSS_EXMX_HYBRID_INLIF_E].
    Expected: GT_OK for Ex1x6 devices and NON GT_OK for non Ex1x6 devices.
1.1.4. For Ex1x6 devices call function 
    cpssExMxInlifPortLookupModeGet with non-NULL inlifModePtr.
    Expected: GT_OK and inlifMode [CPSS_EXMX_HYBRID_INLIF_E].
1.1.5. Call function with out of range inlifMode [0x5AAAAAA5]
    Expected: GT_BAD_PARAM.
1.3.1. Call function for non active port with inlifMode 
    [CPSS_EXMX_SINGLE_INLIF_E] for non-Ex1x6 devices 
    and inlifMode [CPSS_EXMX_HYBRID_INLIF_E] for Ex1x6 devices
    Expected: GT_BAD_PARAM.
1.4. Call with out of range for virtual port and inlifMode 
    [CPSS_EXMX_SINGLE_INLIF_E] for non-Ex1x6 devices and 
    inlifMode [CPSS_EXMX_HYBRID_INLIF_E] for Ex1x6 devices.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                           dev;
    GT_U8                           port;
    CPSS_EXMX_INLIF_PORT_MODE_ENT   inlifMode;

    CPSS_EXMX_INLIF_PORT_MODE_ENT   retInlifMode;
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

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with inlifMode                          */
            /* [CPSS_EXMX_SINGLE_INLIF_E]. Expected: GT_OK for non Ex1x6    */
            /* devices and NON GT_OK for Ex1x6 devices.                     */
            inlifMode = CPSS_EXMX_SINGLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);

            if (CPSS_PP_FAMILY_TIGER_E == devFamily)
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                                "for Tiger device: %d, %d, %d",
                                                dev, port, inlifMode);
            }
            else
            { /* non-Ex1x6 devices */
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                            "for non-Tiger device: %d, %d, %d",
                                            dev, port, inlifMode);

                /* 1.1.2. For non-Ex1x6 devices call function                 */  
                /* cpssExMxInlifPortLookupModeGet with non-NULL inlifModePtr. */
                /* Expected: GT_OK and inlifMode [CPSS_EXMX_SINGLE_INLIF_E].  */

                st = cpssExMxInlifPortLookupModeGet(dev, port, &retInlifMode);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                            "cpssExMxInlifPortLookupModeGet: for non-Tiger device: %d, %d",
                                            dev, port);
                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(inlifMode, retInlifMode,
                                                "get another InLIF Lookup Mode than has been set: %d, %d",
                                                dev, port);
                }
            }

            /* 1.1.3. Call function with inlifMode                          */
            /* [CPSS_EXMX_HYBRID_INLIF_E]. Expected: GT_OK for Ex1x6        */
            /* devices and NON GT_OK for non-Ex1x6 devices.                 */
            inlifMode = CPSS_EXMX_HYBRID_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);

            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                            "for non-Tiger device: %d, %d, %d",
                                            dev, port, inlifMode);
            }
            else
            { /* Ex1x6 devices */
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                            "for Tiger device: %d, %d, %d",
                                            dev, port, inlifMode);

                /* 1.1.4. For Ex1x6 devices call function                     */  
                /* cpssExMxInlifPortLookupModeGet with non-NULL inlifModePtr. */
                /* Expected: GT_OK and inlifMode [CPSS_EXMX_HYBRID_INLIF_E].  */

                st = cpssExMxInlifPortLookupModeGet(dev, port, &retInlifMode);

                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                            "cpssExMxInlifPortLookupModeGet: for Tiger device: %d, %d",
                                            dev, port);
                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(inlifMode, retInlifMode,
                                                "get another InLIF Lookup Mode than has been set: %d, %d",
                                                dev, port);
                }
            }

            /* 1.1.5. Call function with out of range inlifMode [0x5AAAAAA5].*/
            /* Expected: GT_BAD_PARAM.                                       */
            inlifMode = 0x5AAAAAA5;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, inlifMode);
        }

        /* set valid inlif mode for the current device type */
        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            inlifMode = CPSS_EXMX_HYBRID_INLIF_E;
        }
        else
        {
            inlifMode = CPSS_EXMX_SINGLE_INLIF_E;
        }

        /* 1.3. Go over all non active virtual ports and active device id. */
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. Call function for non active port with inlifMode  */
            /* [CPSS_EXMX_SINGLE_INLIF_E] for non-Ex1x6 devices and     */
            /* inlifMode [CPSS_EXMX_HYBRID_INLIF_E] for Ex1x6 devices.  */
            /* Expected: GT_BAD_PARAM.                                  */
            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port and inlifMode   */
        /* [CPSS_EXMX_SINGLE_INLIF_E] for non-Ex1x6 devices and         */
        /* inlifMode [CPSS_EXMX_HYBRID_INLIF_E] for Ex1x6 devices.      */
        /* Expected: GT_BAD_PARAM.                                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = INLIF_VALID_VIRT_PORT_CNS;
    inlifMode = CPSS_EXMX_SINGLE_INLIF_E; /* its value doesn't matter in next cases */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxInlifPortLookupModeGet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            portNum,
    OUT CPSS_EXMX_INLIF_PORT_MODE_ENT    *inlifModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxInlifPortLookupModeGet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function with non-NULL inlifMode.
    Expected: GT_OK.
1.1.2. Call function with inlifMode [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;

    GT_U8                           dev;
    GT_U8                           port;
    CPSS_EXMX_INLIF_PORT_MODE_ENT   inlifMode;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function with non-NULL inlifMode.    */
            /* Expected: GT_OK.                                 */

            st = cpssExMxInlifPortLookupModeGet(dev, port, &inlifMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.2. Call function with inlifHwDataPtr [NULL].    */
            /* Expected: GT_BAD_PTR.                               */

            st = cpssExMxInlifPortLookupModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL",
                                         dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxInlifPortLookupModeGet(dev, port, &inlifMode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxInlifPortLookupModeGet(dev, port, &inlifMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = INLIF_VALID_VIRT_PORT_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssExMxInlifPortLookupModeGet(dev, port, &inlifMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxInlifPortLookupModeGet(dev, port, &inlifMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssExMxInlifVlanEntryGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    IN  GT_U16      vid,
    OUT GT_U32      *inlifHwDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxInlifVlanEntryGet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_SINGLE_INLIF_E]. Expected: GT_OK for Samba/Twist (non-Tiger) devices, NON GT_OK for Tiger devices.
1.1.2. Call function with non-NULL inlifHwDataPtr, vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_OK for Twist and Samba and NON GT_OK otherwise.
1.1.3. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E]. Expected: GT_OK for Samba/Twist (non-Tiger) devices, NON GT_OK for Tiger devices.
1.1.4. Call function cpssExMxInlifVlanConfigGet with non-NULL vlanBaseAddPtr, portInLifMinVidPtr, portInLifMaxVidPtr. Expected: GT_OK for devices of Twist \Samba family (non-Tiger) and NON GT_OK for devices from other families.
1.1.5. Call function with non-NULL inlifHwDataPtr, vid [portInLifMinVidPtr]. Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.
1.1.6. For Twist and Samba device call function with non-NULL inlifHwDataPtr, vid [portInLifMinVidPtr -1]. Expected:  NON GT_OK.
1.1.7. For Twist and Samba device call function with non-NULL inlifHwDataPtr, vid [portInLifMaxVidPtr +1]. Expected: NON GT_OK.
1.1.8. For Twist and Samba device call function with non-NULL inlifHwDataPtr, vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
1.1.9. For Twist and Samba device call function with inlifHwDataPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_U16      vid;
    GT_U32      inlifHwData[4];


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* APPLICABLE DEVICES: Twist/Samba devices                          */
        /* UT assumes that vid can have any value if inlifMode is SINGLE.   */
        CPSS_PP_FAMILY_TYPE_ENT         devFamily;
        CPSS_EXMX_INLIF_PORT_MODE_ENT   inlifMode;

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            GT_U32      vlanBaseAdd;
            GT_U16      portInLifMinVid;
            GT_U16      portInLifMaxVid;

            /* 1.1.1. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_SINGLE_INLIF_E].   */
            /* Expected: GT_OK for non Ex1x6 devices and NON GT_OK for Ex1x6 devices.                           */
            inlifMode = CPSS_EXMX_SINGLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                             dev, port, inlifMode);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                                 dev, port, inlifMode);
            }

            /* 1.1.2. Call function with non-NULL inlifHwDataPtr, vid [PRV_CPSS_MAX_NUM_VLANS_CNS].             */
            /* Expected: GT_OK.for Twist and Samba and NON GT_OK otherwise                                      */
            vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwData[0]);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }

            /* 1.1.3. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E]. */
            /* Expected: GT_OK for Samba/Twist (non-Tiger) devices, NON GT_OK for Tiger devices.                */
            inlifMode = CPSS_EXMX_MULTIPLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                             dev, port, inlifMode);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                                 dev, port, inlifMode);
            }

            /* 1.1.4. Call function cpssExMxInlifVlanConfigGet with non-NULL vlanBaseAddPtr,                    */
            /* portInLifMinVidPtr, portInLifMaxVidPtr to get Vids range for multiple mode.                      */
            /* Expected: GT_OK for devices of Twist \Samba family (non-Tiger) and                               */
            /* NON GT_OK for devices from other families.                                                       */
            st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAdd, &portInLifMinVid, &portInLifMaxVid);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanConfigGet: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanConfigGet: %d, %d", dev, port);
            }

            /* 1.1.5. Call function with non-NULL inlifHwDataPtr, vid [portInLifMinVidPtr].                     */
            /* Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.                             */
            vid = portInLifMinVid;

            st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwData[0]);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }

            /* 1.1.6. For Twist and Samba device call function with non-NULL inlifHwDataPtr,                    */
            /* vid [portInLifMinVidPtr -1].                                                                     */
            /* Expected:  NON GT_OK.                                                                            */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                vid = portInLifMinVid - 1;

                st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwData[0]);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }

            /* 1.1.7. For Twist and Samba device call function with non-NULL inlifHwDataPtr,                    */
            /* vid [portInLifMaxVidPtr +1].                                                                     */
            /* Expected: NON GT_OK.                                                                             */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                vid = portInLifMaxVid + 1;

                st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwData[0]);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }

            /* 1.1.8. For Twist and Samba device call function with non-NULL inlifHwDataPtr,                    */
            /* vid [PRV_CPSS_MAX_NUM_VLANS_CNS].                                                                */
            /* Expected: GT_BAD_PARAM.                                                                          */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

                st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwData[0]);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, vid);
            }

            /* 1.1.9. For Twist and Samba device call function with inlifHwDataPtr [NULL].                      */
            /* Expected: GT_BAD_PTR.                                                                            */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                vid = portInLifMinVid;

                st = cpssExMxInlifVlanEntryGet(dev, port, vid, NULL);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, port, vid);
            }
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        vid = 100;

        /* 1.3. Go over all non active virtual ports and active device id. */
        if (IS_TWIST_OR_SAMBA_MAC(devFamily))
        {
            while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
            {
                /* 1.3.1. <Call function for non active port and valid parameters>.     */
                /* Expected: GT_BAD_PARAM.                                              */
                st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwData[0]);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        if (IS_TWIST_OR_SAMBA_MAC(devFamily))
        {
            port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwData[0]);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = INLIF_VALID_VIRT_PORT_CNS;
    vid = 100;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwData[0]);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwData[0]);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssExMxInlifVlanEntrySet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    IN  GT_U16  vid,
    IN  GT_U32  *inlifHwDataPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxInlifVlanEntrySet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E]. Expected: GT_OK for non Ex1x6 devices and NON GT_OK for Ex1x6 devices.
1.1.2. Call function cpssExMxInlifVlanConfigGet with non-NULL vlanBaseAddPtr, portInLifMinVidPtr, portInLifMaxVidPtr to get Vids range for multiple mode. Expected: GT_OK for devices of Twist \Samba family (non-Tiger) and NON GT_OK for devices from other families.
1.1.3. Call cpssExMxInlifVlanEntryGet with vid [portInLifMinVid] to get current vlan InLIF entry and save it. Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.
1.1.4. Call function with inlifHwDataPtr [{8765, 0, 0, 4321}], vid [portInLifMinVid]. Expected: GT_OK for Twist and Samba and NON GT_OK otherwise.
1.1.5. Call function cpssExMxInlifVlanEntryGet with non-NULL inlifHwDataPtr, vid [portInLifMinVid]. Expected: GT_OK and inlifHwData [{8765, 0, 0, 4321}] for Twist and Samba and NON GT_OK otherwise.
1.1.6. For Twist and Samba devices call function with inlifHwDataPtr [{8765, 0, 0, 4321}], vid [portInLifMinVidPtr -1]. Expected: NON GT_OK
1.1.7. For Twist and Samba devices call function with inlifHwDataPtr [{8765, 0, 0, 4321}], vid [portInLifMaxVidPtr +1]. Expected: NON GT_OK.
1.1.8. For Twist and Samba devices call function with inlifHwDataPtr [{8765, 0, 0, 4321}], vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
1.1.9. For Twist and Samba devices call function with inlifHwDataPtr [NULL]. Expected: GT_BAD_PTR.
1.1.10. Call cpssExMxInlifVlanEntrySet to restore vlan InLIF entry for multiple mode. Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.
1.1.11. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_SINGLE_INLIF_E]. Expected: GT_OK for Samba/Twist (non-Tiger) devices, NON GT_OK for Tiger devices.
1.1.12. Call cpssExMxInlifVlanEntryGet with vid [PRV_CPSS_MAX_NUM_VLANS_CNS] to get current vlan InLIF entry and save it, vid parameter should be ignored. Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.
1.1.13. Call function with inlifHwDataPtr [{1234, 0, 0, 5678}], vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.
1.1.14. Call function cpssExMxInlifVlanEntryGet with non-NULL inlifHwDataPtr, vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_OK and inlifHwData [{1234, 0, 0, 5678}] for Twist and Samba devices and NON GT_OK otherwise.
1.1.15. Call cpssExMxInlifVlanEntrySet to restore vlan InLIF entry for single mode. Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_U16      vid;
    GT_U32      inlifHwData[4];


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    inlifHwData[0] = 8765;
    inlifHwData[1] = 0;
    inlifHwData[2] = 0;
    inlifHwData[3] = 4321;

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* APPLICABLE DEVICES: Twist/Samba devices                          */
        /* UT assumes that vid can have any value if inlifMode is SINGLE.   */
        CPSS_PP_FAMILY_TYPE_ENT         devFamily;
        CPSS_EXMX_INLIF_PORT_MODE_ENT   inlifMode;

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            GT_U32      inlifHwDataOrig[4];
            GT_U32      inlifHwDataWritten[4];
            GT_U32      vlanBaseAdd;
            GT_U16      portInLifMinVid;
            GT_U16      portInLifMaxVid;

            /* 1.1.1. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E]. */
            /* Expected: GT_OK for non Ex1x6 devices and NON GT_OK for Ex1x6 devices.                           */
            inlifMode = CPSS_EXMX_MULTIPLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                             dev, port, inlifMode);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                                 dev, port, inlifMode);
            }

            /* 1.1.2. Call function cpssExMxInlifVlanConfigGet with non-NULL vlanBaseAddPtr,                    */
            /* portInLifMinVidPtr, portInLifMaxVidPtr to get Vids range for multiple mode.                      */
            /* Expected: GT_OK for devices of Twist \Samba family (non-Tiger) and                               */
            /* NON GT_OK for devices from other families.                                                       */
            st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAdd, &portInLifMinVid, &portInLifMaxVid);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanConfigGet: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanConfigGet: %d, %d", dev, port);
            }

            /* 1.1.3. Call cpssExMxInlifVlanEntryGet with vid [portInLifMinVid] to                              */
            /* get current vlan InLIF entry and save it.                                                        */
            /* Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.                             */
            vid = portInLifMinVid;

            st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwDataOrig[0]);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanEntryGet: %d, %d, %d", dev, port, vid);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanEntryGet: %d, %d, %d", dev, port, vid);
            }

            /* 1.1.4. Call function with inlifHwDataPtr [{8765, 0, 0, 4321}], vid [portInLifMinVid].            */
            /* Expected: GT_OK for Twist and Samba and NON GT_OK otherwise.                                     */
            st = cpssExMxInlifVlanEntrySet(dev, port, vid, &inlifHwData[0]);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }

            /* 1.1.5. Call function cpssExMxInlifVlanEntryGet with non-NULL inlifHwDataPtr,                     */
            /* vid [portInLifMinVid].                                                                           */
            /* Expected: GT_OK and inlifHwData [{8765, 0, 0, 4321}] for Twist and Samba and NON GT_OK otherwise.*/
            st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwDataWritten[0]);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
                UTF_VERIFY_EQUAL3_PARAM_MAC(inlifHwData[0], inlifHwDataWritten[0], dev, port, vid);
                UTF_VERIFY_EQUAL3_PARAM_MAC(inlifHwData[1], inlifHwDataWritten[1], dev, port, vid);
                UTF_VERIFY_EQUAL3_PARAM_MAC(inlifHwData[2], inlifHwDataWritten[2], dev, port, vid);
                UTF_VERIFY_EQUAL3_PARAM_MAC(inlifHwData[3], inlifHwDataWritten[3], dev, port, vid);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }

            /* 1.1.6. For Twist and Samba devices call function with inlifHwDataPtr [{8765, 0, 0, 4321}],       */
            /* vid [portInLifMinVidPtr -1].                                                                     */
            /* Expected: NON GT_OK                                                                              */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                vid = portInLifMinVid - 1;

                st = cpssExMxInlifVlanEntrySet(dev, port, vid, &inlifHwData[0]);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }

            /* 1.1.7. For Twist and Samba devices call function with inlifHwDataPtr [{8765, 0, 0, 4321}],       */
            /* vid [portInLifMaxVidPtr +1].                                                                     */
            /* Expected: NON GT_OK.                                                                             */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                vid = portInLifMaxVid + 1;

                st = cpssExMxInlifVlanEntrySet(dev, port, vid, &inlifHwData[0]);
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }

            /* 1.1.8. For Twist and Samba devices call function with inlifHwDataPtr [{8765, 0, 0, 4321}],       */
            /* vid [PRV_CPSS_MAX_NUM_VLANS_CNS].                                                                */
            /* Expected: GT_BAD_PARAM.                                                                          */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

                st = cpssExMxInlifVlanEntrySet(dev, port, vid, &inlifHwData[0]);
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, vid);
            }

            /* 1.1.9. For Twist and Samba devices call function with inlifHwDataPtr [NULL].                     */
            /* Expected: GT_BAD_PTR.                                                                            */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                vid = portInLifMinVid;

                st = cpssExMxInlifVlanEntrySet(dev, port, vid, NULL);
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, port, vid);
            }

            /* 1.1.10. Call cpssExMxInlifVlanEntrySet to restore vlan InLIF entry for multiple mode.            */
            /* Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.                             */
            vid = portInLifMinVid;

            st = cpssExMxInlifVlanEntrySet(dev, port, vid, &inlifHwDataOrig[0]);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }

            /* Now we are going to test this function in the single mode with vid is equal to max vid */

            /* 1.1.11. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_SINGLE_INLIF_E].  */
            /* Expected: GT_OK for Samba/Twist (non-Tiger) devices, NON GT_OK for Tiger devices.                */
            inlifMode = CPSS_EXMX_SINGLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                             dev, port, inlifMode);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                                 dev, port, inlifMode);
            }

            /* 1.1.12. Call cpssExMxInlifVlanEntryGet with vid [PRV_CPSS_MAX_NUM_VLANS_CNS] to                  */
            /* get current vlan InLIF entry and save it, vid parameter should be ignored.                       */
            /* Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.                             */
            vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwDataOrig[0]);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanEntryGet: %d, %d, %d", dev, port, vid);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanEntryGet: %d, %d, %d", dev, port, vid);
            }

            /* 1.1.13. Call function with inlifHwDataPtr [{1234, 0, 0, 5678}],                                  */
            /* vid [PRV_CPSS_MAX_NUM_VLANS_CNS].                                                                */
            /* Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.                             */
            st = cpssExMxInlifVlanEntrySet(dev, port, vid, &inlifHwData[0]);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }

            /* 1.1.14. Call function cpssExMxInlifVlanEntryGet with non-NULL inlifHwDataPtr,                    */
            /* vid [PRV_CPSS_MAX_NUM_VLANS_CNS].                                                                */
            /* Expected: GT_OK and inlifHwData [{1234, 0, 0, 5678}] for Twist and Samba devices and             */
            /* NON GT_OK otherwise.                                                                             */
            st = cpssExMxInlifVlanEntryGet(dev, port, vid, &inlifHwDataWritten[0]);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
                UTF_VERIFY_EQUAL3_PARAM_MAC(inlifHwData[0], inlifHwDataWritten[0], dev, port, vid);
                UTF_VERIFY_EQUAL3_PARAM_MAC(inlifHwData[1], inlifHwDataWritten[1], dev, port, vid);
                UTF_VERIFY_EQUAL3_PARAM_MAC(inlifHwData[2], inlifHwDataWritten[2], dev, port, vid);
                UTF_VERIFY_EQUAL3_PARAM_MAC(inlifHwData[3], inlifHwDataWritten[3], dev, port, vid);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }

            /* 1.1.15. Call cpssExMxInlifVlanEntrySet to restore vlan InLIF entry for single mode.              */
            /* Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.                             */
            st = cpssExMxInlifVlanEntrySet(dev, port, vid, &inlifHwDataOrig[0]);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);
            }
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        vid = 100;

        /* 1.3. Go over all non active virtual ports and active device id. */
        if (IS_TWIST_OR_SAMBA_MAC(devFamily))
        {
            while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
            {
                /* 1.3.1. <Call function for non active port and valid parameters>.     */
                /* Expected: GT_BAD_PARAM.                                              */
                st = cpssExMxInlifVlanEntrySet(dev, port, vid, &inlifHwData[0]);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        if (IS_TWIST_OR_SAMBA_MAC(devFamily))
        {
            port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxInlifVlanEntrySet(dev, port, vid, &inlifHwData[0]);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = INLIF_VALID_VIRT_PORT_CNS;
    vid = 100;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxInlifVlanEntrySet(dev, port, vid, &inlifHwData[0]);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxInlifVlanEntrySet(dev, port, vid, &inlifHwData[0]);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssExMxInlifVlanConfigSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U32      vlanBaseAdd,
    IN  GT_U16      portInLifMinVid,
    IN  GT_U16      portInLifMaxVid
)
*/
UTF_TEST_CASE_MAC(cpssExMxInlifVlanConfigSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E]. Expected: GT_OK for non-Tiger devices and NON GT_OK for Tiger (Ex1x6) devices.
1.1.2. Call cpssExMxInlifVlanConfigGet to get current vlan base address and and vlan range save it. Expected: GT_OK for Twist and Samba (non-Tiger) devices and NON GT_OK otherwise.
1.1.3. Call function with vlanBaseAdd [0], portInLifMinVid [12], portInLifMaxVid [285]. Expected: GT_OK for devices of Twist or Samba (non-Tiger) family and NON GT_OK for devices from other families.
1.1.4. Call function cpssExMxInlifVlanConfigGet with non-NULL vlanBaseAddPtr, portInLifMinVidPtr, portInLifMaxVidPtr. Expected: GT_OK and vlanBaseAdd [0], portInLifMinVid [12], portInLifMaxVid [285] for devices of Twist or Samba (non-Tiger) family and NON GT_OK for devices from other families.
1.1.5. For Twist and Samba devices call function with out of range portInLifMinVid [PRV_CPSS_MAX_NUM_VLANS_CNS] ( portInLifMaxVid [285], vlanBaseAdd [0]). Expected: NON GT_OK.
1.1.6. For Twist and Samba devices call function with out of range portInLifMaxVid [PRV_CPSS_MAX_NUM_VLANS_CNS] ( portInLifMinVid [12], vlanBaseAdd [0]). Expected: NON GT_OK.
1.1.7. For Twist and Samba devices call function with portInLifMinVid [285], portInLifMaxVid [12], vlanBaseAdd [0]. Expected: NON GT_OK.
1.1.8. For Twist and Samba devices call function with out of range vlanBaseAddr [MaxInlifsNum(dev)] ( portInLifMinVid [12], portInLifMaxVid [285]). Expected: NON GT_OK.
1.1.9. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_SINGLE_INLIF_E]. Expected: GT_OK for non Ex1x6 devices and NON GT_OK for Ex1x6 devices.
1.1.10. Call function with vlanBaseAdd [0], portInLifMinVid [12], portInLifMaxVid [285]. Expected: NON GT_OK for devices of Twist or Samba family (because of inlif mode value) and NON GT_OK for devices from other families (because of device type).
1.1.11. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E]. Expected: GT_OK for non -Tiger devices and NON GT_OK for Tiger devices.
1.1.12. Call function cpssExMxInlifVlanConfigSet to restore original vlan base address and vlan range. Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_U32      vlanBaseAdd;
    GT_U16      portInLifMinVid;
    GT_U16      portInLifMaxVid;

    GT_U32      vlanBaseAddOrig;
    GT_U16      portInLifMinVidOrig;
    GT_U16      portInLifMaxVidOrig;

    GT_U32      vlanBaseAddWritten;
    GT_U16      portInLifMinVidWritten;
    GT_U16      portInLifMaxVidWritten;

    CPSS_PP_FAMILY_TYPE_ENT         devFamily;
    CPSS_EXMX_INLIF_PORT_MODE_ENT   inlifMode;
    GT_U32                          maxNumOfLifs;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* APPLICABLE DEVICES:  Twist/Samba device.                         */
        /* This function can be used only when the inlif mode is multiple.  */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfMaxNumOfLifsGet(dev, &maxNumOfLifs);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMaxNumOfLifsGet: %d", dev);

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E].     */
            /* Expected: GT_OK for non Ex1x6 devices and NON GT_OK for Ex1x6 devices.                               */
            inlifMode = CPSS_EXMX_MULTIPLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                             dev, port, inlifMode);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                                 dev, port, inlifMode);
            }

            /* 1.1.2. Call cpssExMxInlifVlanConfigGet to get current vlan base address and and vlan range save it.  */
            /* Expected: GT_OK for Twist and Samba (non-Tiger) devices and NON GT_OK otherwise.                     */
            st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAddOrig, &portInLifMinVidOrig, &portInLifMaxVidOrig);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanConfigGet: %d, %d", dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanConfigGet: %d, %d", dev, port);
            }

            /* 1.1.3. Call function with vlanBaseAdd [0], portInLifMinVid [12], portInLifMaxVid [285].             */
            /* Expected: GT_OK for devices of Twist or Samba (non-Tiger) family and                                 */
            /* NON GT_OK for devices from other families.                                                           */
            vlanBaseAdd = 0;
            portInLifMinVid = 12;
            portInLifMaxVid = 285;

            st = cpssExMxInlifVlanConfigSet(dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
            }

            /* 1.1.4. Call function cpssExMxInlifVlanConfigGet with non-NULL vlanBaseAddPtr,                        */
            /* portInLifMinVidPtr, portInLifMaxVidPtr.                                                              */
            /* Expected: GT_OK and vlanBaseAdd [0], portInLifMinVid [12], portInLifMaxVid [285] for                */
             /* devices of Twist or Samba (non-Tiger) family and NON GT_OK for devices from other families.         */

            st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAddWritten,
                                            &portInLifMinVidWritten, &portInLifMaxVidWritten);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanConfigGet: %d, %d", dev, port);
                if (GT_OK == st)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanBaseAdd, vlanBaseAddWritten,
                                                "get another vlanBaseAdd than was set: %d, %d",
                                                dev, port);
                    UTF_VERIFY_EQUAL2_STRING_MAC(portInLifMinVid, portInLifMinVidWritten,
                                                "get another portInLifMinVid than was set: %d, %d",
                                                dev, port);
                    UTF_VERIFY_EQUAL2_STRING_MAC(portInLifMaxVid, portInLifMaxVidWritten,
                                                "get another portInLifMaxVid than was set: %d, %d",
                                                dev, port);
                }
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxInlifVlanConfigGet: %d, %d", dev, port);
            }

            /* 1.1.5. For Twist and Samba devices call function with out of range                                   */
            /* portInLifMinVid [PRV_CPSS_MAX_NUM_VLANS_CNS] ( portInLifMaxVid [285], vlanBaseAdd [0])              */
            /* Expected: NON GT_OK.                                                                                 */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                vlanBaseAdd         = 0;
                portInLifMinVid     = PRV_CPSS_MAX_NUM_VLANS_CNS;
                portInLifMaxVid     = 285;

                st = cpssExMxInlifVlanConfigSet(dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
                UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
            }

            /* 1.1.6. For Twist and Samba devices call function with out of range                                   */
            /* portInLifMaxVid [PRV_CPSS_MAX_NUM_VLANS_CNS] ( portInLifMinVid [12], vlanBaseAdd [0]).              */
            /* Expected: NON GT_OK                                                                                  */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {

                vlanBaseAdd         = 0;
                portInLifMinVid     = 12;
                portInLifMaxVid     = PRV_CPSS_MAX_NUM_VLANS_CNS;

                st = cpssExMxInlifVlanConfigSet(dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
                UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
            }

            /* 1.1.7. For Twist and Samba devices call function with portInLifMinVid [285],                         */
            /* portInLifMaxVid [12], vlanBaseAdd [0].                                                              */
            /* Expected: NON GT_OK                                                                                  */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                vlanBaseAdd         = 0;
                portInLifMinVid     = 285;
                portInLifMaxVid     = 12;

                st = cpssExMxInlifVlanConfigSet(dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
                UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
            }

            /* 1.1.8. For Twist and Samba devices call function with out of     */
            /* range vlanBaseAddr [MaxInlifsNum(dev)] ( portInLifMinVid [12],   */
            /* portInLifMaxVid [285]). Expected: NON GT_OK.                     */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                vlanBaseAdd = maxNumOfLifs;
                portInLifMinVid     = 12;
                portInLifMaxVid     = 285;

                st = cpssExMxInlifVlanConfigSet(dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, %d == maxNumOfLifs",
                                                 dev, port, vlanBaseAdd);
            }

            /* 1.1.9. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_SINGLE_INLIF_E].       */
            /* Expected: GT_OK for non Ex1x6 devices and NON GT_OK for Ex1x6 devices.                               */
            inlifMode = CPSS_EXMX_SINGLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                             dev, port, inlifMode);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                                 dev, port, inlifMode);
            }

            /* 1.1.10. Call function with vlanBaseAdd [0], portInLifMinVid [12], portInLifMaxVid [285].             */
            /* Expected: NON GT_OK for devices of Twist or Samba family (because of inlif mode value) and           */
            /* NON GT_OK for devices from other families (because of device type).                                  */
            vlanBaseAdd         = 0;
            portInLifMinVid     = 12;
            portInLifMaxVid     = 285;

            st = cpssExMxInlifVlanConfigSet(dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
            UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);

            /* 1.1.11. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E].    */
            /* Expected: GT_OK for non -Tiger devices and NON GT_OK for Tiger devices.                              */
            inlifMode = CPSS_EXMX_MULTIPLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                             dev, port, inlifMode);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                                 dev, port, inlifMode);
            }

            /* 1.1.12. Call function cpssExMxInlifVlanConfigSet to                                                  */
            /* restore original vlan base address and vlan range.                                                   */
            /* Expected: GT_OK for Twist and Samba devices and NON GT_OK otherwise.                                 */
            st = cpssExMxInlifVlanConfigSet(dev, port, vlanBaseAddOrig, portInLifMinVidOrig, portInLifMaxVidOrig);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, port,
                                            vlanBaseAddOrig, portInLifMinVidOrig, portInLifMaxVidOrig);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL5_PARAM_MAC(GT_OK, st, dev, port,
                                                vlanBaseAddOrig, portInLifMinVidOrig, portInLifMaxVidOrig);
            }
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        vlanBaseAdd         = 0;
        portInLifMinVid     = 12;
        portInLifMaxVid     = 285;

        /* 1.3. Go over all non active virtual ports and active device id. */
        if (IS_TWIST_OR_SAMBA_MAC(devFamily))
        {
            while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
            {
                /* 1.3.1. <Call function for non active port and valid parameters>.     */
                /* Expected: GT_BAD_PARAM.                                              */
                st = cpssExMxInlifVlanConfigSet(dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        if (IS_TWIST_OR_SAMBA_MAC(devFamily))
        {
            port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxInlifVlanConfigSet(dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = INLIF_VALID_VIRT_PORT_CNS;

    vlanBaseAdd         = 0;
    portInLifMinVid     = 12;
    portInLifMaxVid     = 285;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxInlifVlanConfigSet(dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxInlifVlanConfigSet(dev, port, vlanBaseAdd, portInLifMinVid, portInLifMaxVid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssExMxInlifVlanConfigGet
(
    IN GT_U8        devNum,
    IN GT_U8        portNum,
    OUT GT_U32      *vlanBaseAddPtr,
    OUT GT_U16      *portInLifMinVidPtr,
    OUT GT_U16      *portInLifMaxVidPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxInlifVlanConfigGet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_SINGLE_INLIF_E]. Expected: GT_OK for non Ex1x6 devices and NON GT_OK for Ex1x6 devices.
1.1.2. Call function with non-NULL vlanBaseAddPtr, portInLifMinVidPtr, portInLifMaxVidPtr. Expected: NON GT_OK for devices of Twist or Samba family (because of inlif mode value) and NON GT_OK for devices from other families (because of device type).
1.1.3. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E]. Expected: GT_OK for non Ex1x6 devices and NON GT_OK for Ex1x6 devices.
1.1.4. Call function with non-NULL vlanBaseAddPtr, portInLifMinVidPtr, portInLifMaxVidPtr. Expected: GT_OK for devices of Twist or Samba (non-Tiger) family and NON GT_OK for devices from other families.
1.1.5. For Twist and Samba devices with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E] call function with vlanBaseAddPtr [NULL]. Expected: GT_BAD_PTR. 
1.1.6. For Twist and Samba devices with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E] call function with portInLifMinVidPtr [NULL]. Expected: GT_BAD_PTR.
1.1.7. For Twist and Samba devices with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E] call function with portInLifMaxVidPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_U32      vlanBaseAdd;
    GT_U16      portInLifMinVid;
    GT_U16      portInLifMaxVid;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* APPLICABLE DEVICES:  Twist/Samba device.                         */
        /* This function can be used only when the inlif mode is multiple.  */
        CPSS_PP_FAMILY_TYPE_ENT         devFamily;
        CPSS_EXMX_INLIF_PORT_MODE_ENT   inlifMode;

        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_SINGLE_INLIF_E].   */
            /* Expected: GT_OK for non Ex1x6 devices and NON GT_OK for Ex1x6 devices.                           */
            inlifMode = CPSS_EXMX_SINGLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                             dev, port, inlifMode);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                                 dev, port, inlifMode);
            }

            /* 1.1.2. Call function with non-NULL vlanBaseAddPtr, portInLifMinVidPtr, portInLifMaxVidPtr.       */
            /* Expected: NON GT_OK for devices of Twist or Samba family (because of inlif mode value) and       */
            /* NON GT_OK for devices from other families (because of device type).                              */
            st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAdd, &portInLifMinVid, &portInLifMaxVid);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.1.3. Call function cpssExMxInlifPortLookupModeSet with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E]. */
            /* Expected: GT_OK for non Ex1x6 devices and NON GT_OK for Ex1x6 devices.                           */
            inlifMode = CPSS_EXMX_MULTIPLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                             dev, port, inlifMode);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: %d, %d, %d",
                                                 dev, port, inlifMode);
            }

            /* 1.1.4. Call function with non-NULL vlanBaseAddPtr, portInLifMinVidPtr, portInLifMaxVidPtr.       */
            /* Expected: GT_OK for devices of Twist or Samba (non-Tiger) family and                             */
            /* NON GT_OK for devices from other families.                                                       */
            st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAdd, &portInLifMinVid, &portInLifMaxVid);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            }

            /* 1.1.5. For Twist and Samba devices with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E]                   */
            /* call function with vlanBaseAddPtr [NULL].                                                        */
            /* Expected: GT_BAD_PTR.                                                                            */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                st = cpssExMxInlifVlanConfigGet(dev, port, NULL, &portInLifMinVid, &portInLifMaxVid);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
            }

            /* 1.1.6. For Twist and Samba devices with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E]                   */
            /* call function with portInLifMinVidPtr [NULL].                                                    */
            /* Expected: GT_BAD_PTR.                                                                            */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAdd, NULL, &portInLifMaxVid);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
            }

            /* 1.1.7. For Twist and Samba devices with inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E]                   */
            /* call function with portInLifMaxVidPtr [NULL].                                                    */
            /* Expected: GT_BAD_PTR.                                                                            */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAdd, &portInLifMinVid, NULL);
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
            }
        }

        /* 1.3. Go over all non active virtual ports and active device id. */
        if (IS_TWIST_OR_SAMBA_MAC(devFamily))
        {
            st = prvUtfNextVirtPortReset(&port, dev);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
            while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
            {
                /* 1.3.1. <Call function for non active port and valid parameters>.     */
                /* Expected: GT_BAD_PARAM.                                              */
                st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAdd, &portInLifMinVid, &portInLifMaxVid);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }

            /* 1.4. Call with out of range for virtual port.                            */
            /* Expected: GT_BAD_PARAM.                                                  */
            port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAdd, &portInLifMinVid, &portInLifMaxVid);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = INLIF_VALID_VIRT_PORT_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAdd, &portInLifMinVid, &portInLifMaxVid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAdd, &portInLifMinVid, &portInLifMaxVid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxInlifDefaultEntrySet
(
    IN  GT_U8                            devNum,
    IN  CPSS_EXMX_INLIF_FIELD_TYPE_ENT   *fieldTypePtr,
    IN  CPSS_EXMX_INLIF_FIELD_VALUE_UNT  *fieldValuePtr,
    IN  GT_U32                           numOfFields
)
*/
UTF_TEST_CASE_MAC(cpssExMxInlifDefaultEntrySet)
{
/*
ITERATE_DEVICES
1.1. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_DROP_MC_LOCAL_E}]
    and fieldValue [{GT_TRUE}], numOfFields [1]
    Expected: GT_OK for Tiger devices for vlan inlif entry - regular, GT_OK for non-Tiger.
1.2. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}]
    and fieldValue [{GT_TRUE}], numOfFields [1]
    Expected: GT_OK for Samba/Twist (non-Tiger) devices, NON GT_OK for Tiger devices.
1.3. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_PCL_MAX_HOP_E}]
    and fieldValue [{3}], numOfFields [1]
    Expected: GT_OK for Twist-D  devices, NON GT_OK for others.
1.4. Call function with fieldTypePtr 
    [{CPSS_EXMX_INLIF_DIS_BRG_E; CPSS_EXMX_INLIF_EN_IPV4_E}]
    and fieldValue [{GT_TRUE; GT_TRUE}], numOfFields [2]. Expected: GT_OK.
1.5. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_MIRROR_2_CPU_E}]
    and fieldValue [{GT_TRUE}], numOfFields [1]. Expected: GT_OK.
1.6. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E}] (not supported)
    and fieldValue [{1}], numOfFields [1]. Expected: GT_BAD_PARAM.
1.7. Call function with out of range fieldType [{0x5AAAAAA5}]
    and fieldValue [{GT_TRUE}], numOfFields [1]. Expected: NON GT_OK.
1.8. Call function with fieldTypePtr [NULL] and fieldValue [{1}],
    numOfFields [1]. Expected: GT_BAD_PTR.
1.9. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}]
    and fieldValue [NULL], numOfFields [1]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    CPSS_EXMX_INLIF_FIELD_TYPE_ENT      fieldTypeArr[2];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT     fieldValueArr[2];
    GT_U32                              numOfFields;

    CPSS_PP_FAMILY_TYPE_ENT             devFamily;

    cpssOsBzero((GT_VOID*)fieldTypeArr, sizeof(fieldTypeArr[0])*2);
    cpssOsBzero((GT_VOID*)fieldValueArr, sizeof(fieldValueArr[0])*2);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {

        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        /* 1.1. Call function with fieldTypePtr                     */
        /* [{CPSS_EXMX_INLIF_DROP_MC_LOCAL_E}]                      */
        /* and fieldValue [{GT_TRUE}], numOfFields [1]              */
        /* Expected: GT_OK for Tiger devices for vlan inlif entry   */
        /*- non-reduced, GT_OK for non-Tiger.                         */
        fieldTypeArr[0] = CPSS_EXMX_INLIF_DROP_MC_LOCAL_E;
        fieldValueArr[0].boolField = GT_TRUE;
        numOfFields = 1;

        st = cpssExMxInlifDefaultEntrySet(dev, fieldTypeArr, fieldValueArr, numOfFields);

        if (CPSS_PP_FAMILY_TIGER_E == devFamily)
        {
            if (PRV_CPSS_EXMX_PP_MAC(dev)->moduleCfg.logicalIfCfg.vlanInlifEntryType
                != CPSS_EXMX_VLAN_INLIF_ENTRY_TYPE_REDUCED_E)
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                             "for Tiger device, non-reduced vlan inlif entry: %d, %d, %d, %d",
                                             dev, fieldTypeArr[0],
                                             fieldValueArr[0].boolField, numOfFields);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                             "for Tiger device, reduced vlan inlif entry: %d, %d, %d, %d",
                                             dev, fieldTypeArr[0],
                                             fieldValueArr[0].boolField, numOfFields);
            }
        }
        else
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                         "for non-Tiger device: %d, %d, %d, %d",
                                         dev, fieldTypeArr[0],
                                         fieldValueArr[0].boolField, numOfFields);
        }

        /* 1.2. Call function with fieldTypePtr                         */
        /* [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}] and fieldValue [{GT_TRUE}], */
        /* numOfFields [1] Expected: GT_OK for Samba/Twist (non-Tiger)  */
        /* devices, NON GT_OK for Tiger devices.                        */
        fieldTypeArr[0] = CPSS_EXMX_INLIF_EN_MPLS_MC_E;
        fieldValueArr[0].boolField = GT_TRUE;
        numOfFields = 1;

        st = cpssExMxInlifDefaultEntrySet(dev, fieldTypeArr, fieldValueArr, numOfFields);

        if (IS_TWIST_OR_SAMBA_MAC(devFamily))
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                        "for Twist/Samba device: %d, %d, %d, %d",
                                         dev, fieldTypeArr[0],
                                         fieldValueArr[0].boolField, numOfFields);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                         "for non Twist/Samba device: %d, %d, %d, %d",
                                         dev, fieldTypeArr[0],
                                         fieldValueArr[0].boolField, numOfFields);
        }

        /* 1.3. Call function with fieldTypePtr                     */
        /* [{CPSS_EXMX_INLIF_PCL_MAX_HOP_E}] and fieldValue [{3}],  */
        /* numOfFields [1] Expected: GT_OK for Twist-D  devices,    */
        /* NON GT_OK for others.                                    */
        fieldTypeArr[0] = CPSS_EXMX_INLIF_PCL_MAX_HOP_E;
        fieldValueArr[0].u32Field = 3;
        numOfFields = 1;

        st = cpssExMxInlifDefaultEntrySet(dev, fieldTypeArr, fieldValueArr, numOfFields);

        if (CPSS_PP_FAMILY_TWISTD_E == devFamily)
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                        "for Twist-D device: %d, %d, %d, %d",
                                         dev, fieldTypeArr[0],
                                         fieldValueArr[0].boolField, numOfFields);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                         "for non Twist-D device: %d, %d, %d, %d",
                                         dev, fieldTypeArr[0],
                                         fieldValueArr[0].boolField, numOfFields);
        }

        /* 1.4. Call function with fieldTypePtr                     */
        /* [{CPSS_EXMX_INLIF_DIS_BRG_E; CPSS_EXMX_INLIF_EN_IPV4_E}] */
        /* and fieldValue [{GT_TRUE; GT_TRUE}], numOfFields [2].    */
        /* Expected: GT_OK.                                         */
        fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
        fieldValueArr[0].boolField = GT_TRUE;
        fieldTypeArr[1] = CPSS_EXMX_INLIF_EN_IPV4_E;
        fieldValueArr[1].boolField = GT_TRUE;
        numOfFields = 2;

        st = cpssExMxInlifDefaultEntrySet(dev, fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                                     "%d, type[0] = %d, val[0] = %d, type[1] = %d, val[1] = %d",
                                     dev, fieldTypeArr[0],fieldValueArr[0].boolField,
                                     fieldTypeArr[1],fieldValueArr[1].boolField);

        /* 1.5. Call function with fieldTypePtr                             */
        /* [{CPSS_EXMX_INLIF_MIRROR_2_CPU_E}] and fieldValue [{GT_TRUE}],   */
        /* numOfFields [1]. Expected: GT_OK.                                */
        fieldTypeArr[0] = CPSS_EXMX_INLIF_MIRROR_2_CPU_E;
        fieldValueArr[0].boolField = GT_TRUE;
        numOfFields = 1;

        st = cpssExMxInlifDefaultEntrySet(dev, fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, fieldTypeArr[0],
                                     fieldValueArr[0].boolField, numOfFields);

        /* 1.6. Call function with fieldTypePtr                         */ 
        /* [{CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E}] and fieldValue [{1}], */
        /* numOfFields [1]. Expected: GT_BAD_PARAM.                     */
        fieldTypeArr[0] = CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E;
        fieldValueArr[0].u32Field = 1;
        numOfFields = 1;

        st = cpssExMxInlifDefaultEntrySet(dev, fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, fieldTypeArr[0],
                                     fieldValueArr[0].boolField, numOfFields);

        /* 1.7. Call function with out of range fieldType [{0x5AAAAAA5}]     */
        /* and fieldValue [{GT_TRUE}], numOfFields [1]. Expected: NON GT_OK. */
        fieldTypeArr[0] = INLIF_INVALID_ENUM_CNS;
        numOfFields = 1;

        st = cpssExMxInlifDefaultEntrySet(dev, fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, fieldTypeArr[0]);

        /* 1.8. Call function with fieldTypePtr [NULL] and fieldValue [{1}], */
        /* numOfFields [1]. Expected: GT_BAD_PTR.                            */
        numOfFields = 1;

        st = cpssExMxInlifDefaultEntrySet(dev, NULL, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);

        /* 1.9. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}] */
        /* and fieldValue [NULL], numOfFields [1]. Expected: GT_BAD_PTR.      */
        fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
        numOfFields = 1;

        st = cpssExMxInlifDefaultEntrySet(dev, NULL, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, <fieldTypePtr>, NULL", dev);
    }

    fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
    fieldValueArr[0].boolField = GT_TRUE;
    numOfFields = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxInlifDefaultEntrySet(dev, fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxInlifDefaultEntrySet(dev, fieldTypeArr, fieldValueArr, numOfFields);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxInlifFieldsSet
(
    IN  GT_U8                            devNum,
    IN  GT_U8                            port,
    IN  GT_U16                           vid,
    IN  CPSS_EXMX_INLIF_FIELD_TYPE_ENT   *fieldTypePtr,
    IN  CPSS_EXMX_INLIF_FIELD_VALUE_UNT  *fieldValuePtr,
    IN  GT_U32                           numOfFields
)
*/
UTF_TEST_CASE_MAC(cpssExMxInlifFieldsSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call function cpssExMxInlifPortLookupModeSet with
    inlifMode [CPSS_EXMX_SINGLE_INLIF_E]. Expected: GT_OK for Samba/Twist
    (non-Tiger) devices, NON GT_OK for Tiger devices.
1.1.2. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}]
    and fieldValue [{GT_TRUE}], numOfFields [1], vid [PRV_CPSS_MAX_NUM_VLANS_CNS]
    (vid isn't used in SINGLE mode). Expected: GT_OK for Samba/Twist
    (non-Tiger) devices, NON GT_OK for Tiger devices.
1.1.3. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_PCL_MAX_HOP_E}]
    and fieldValue [{3}], numOfFields [1], vid [1]. Expected: GT_OK for Twist-D
    devices, NON GT_OK for others.
1.1.4. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E;
    CPSS_EXMX_INLIF_EN_IPV4_E}] and fieldValue [{GT_TRUE; GT_TRUE}],
    numOfFields [2]. Expected: GT_OK for Samba/Twist (non-Tiger)
    devices, NON GT_OK for Tiger devices.
1.1.5. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_MIRROR_2_CPU_E}]
    and fieldValue [{GT_TRUE}], numOfFields [1]. 
    Expected: GT_OK for Samba/Twist (non-Tiger) devices,
    NON GT_OK for Tiger devices.
1.1.6. Call function with fieldTypePtr [{CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E}]
    and fieldValue [{1}], numOfFields [1].
    Expected: GT_OK for Samba/Twist (non-Tiger) devices,
    NON GT_OK for Tiger devices.
1.1.7. For Twist and Samba devices call function with out of range fieldType [{0x5AAAAAA5}]
    and fieldValue [{GT_TRUE}], numOfFields [1]. Expected: NON GT_OK.
1.1.8. For Twist and Samba devices call function with fieldTypePtr [NULL] and fieldValue [{1}],
    numOfFields [1]. Expected: GT_BAD_PTR.
1.1.9. For Twist and Samba devices call function with fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}]
    and fieldValue [NULL], numOfFields [1]. Expected: GT_BAD_PTR.
1.1.10. Call function cpssExMxInlifPortLookupModeSet with
    inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E].
    Expected: GT_OK for Samba/Twist (non-Tiger) devices,
    NON GT_OK for Tiger devices.
1.1.11. Call function cpssExMxInlifVlanConfigGet with non-NULL vlanBaseAddPtr,
    portInLifMinVidPtr, portInLifMaxVidPtr.
    Expected: GT_OK for devices of Twist \Samba family (non-Tiger)
    and NON GT_OK for devices from other families.
1.1.12. Call function  with fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}]
    and fieldValue [{GT_TRUE}], numOfFields [1], vid [portInLifMinVidPtr].
    Expected: GT_OK for Samba/Twist (non-Tiger) devices, NON GT_OK for Tiger devices.
1.1.13. For Twist and Samba devices call function  with fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}]
    and fieldValue [{GT_TRUE}], numOfFields [1], vid [portInLifMaxVidPtr + 1].
    Expected: NON GT_OK.
1.1.14. For Twist and Samba devices call function  with fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}]
    and fieldValue [{GT_TRUE}], numOfFields [1], vid [portInLifMinVidPtr - 1].
    Expected: NON GT_OK.
1.1.15. For Twist and Samba devices call function  with fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}]
    and fieldValue [{GT_TRUE}], numOfFields [1], vid [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                               dev;
    GT_U8                               port;
    GT_U16                              vid;
    CPSS_EXMX_INLIF_FIELD_TYPE_ENT      fieldTypeArr[2];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT     fieldValueArr[2];
    GT_U32                              numOfFields;

    CPSS_EXMX_INLIF_PORT_MODE_ENT       inlifMode;
    CPSS_PP_FAMILY_TYPE_ENT             devFamily;
    GT_U32                              vlanBaseAdd;
    GT_U16                              portInLifMinVid = 1;
    GT_U16                              portInLifMaxVid = 1;

    cpssOsBzero((GT_VOID*)fieldTypeArr, sizeof(fieldTypeArr[0])*2);
    cpssOsBzero((GT_VOID*)fieldValueArr, sizeof(fieldValueArr[0])*2);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device family */
        st = prvUtfDeviceFamilyGet(dev, &devFamily);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceFamilyGet: %d",
                                     dev);

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For each active device id go over all active virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function cpssExMxInlifPortLookupModeSet with      */
            /* inlifMode [CPSS_EXMX_SINGLE_INLIF_E]. Expected: GT_OK for     */
            /* Samba/Twist (non-Tiger) devices, NON GT_OK for Tiger devices. */
            inlifMode = CPSS_EXMX_SINGLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: non-Tiger device: %d, %d, %d",
                                             dev, port, inlifMode);
            }
            else
            {/* Tiger */
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: Tiger device: %d, %d, %d",
                                             dev, port, inlifMode);
            }

            /* 1.1.2. Call function with fieldTypePtr                        */
            /* [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}] and fieldValue [{GT_TRUE}],  */
            /* numOfFields [1], vid [PRV_CPSS_MAX_NUM_VLANS_CNS]             */
            /* (vid isn't used in SINGLE mode). Expected: GT_OK for          */
            /* Samba/Twist (non-Tiger) devices, NON GT_OK for Tiger devices. */
            fieldTypeArr[0] = CPSS_EXMX_INLIF_EN_MPLS_MC_E;
            fieldValueArr[0].boolField = GT_TRUE;
            numOfFields = 1;
            vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);

            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                                             "for Twist/Samba device: %d, %d, %d, %d, %d",
                                             dev, port, vid, fieldTypeArr[0],
                                             fieldValueArr[0].boolField);
            }
            else
            {/* Tiger */
                UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
                                             "for non Twist/Samba device: %d, %d, %d, %d, %d",
                                             dev, port, vid, fieldTypeArr[0],
                                             fieldValueArr[0].boolField);
            }

            /* 1.1.3. Call function with fieldTypePtr                   */
            /* [{CPSS_EXMX_INLIF_PCL_MAX_HOP_E}] and fieldValue [{3}],  */
            /* numOfFields [1], vid [1]. Expected: GT_OK for Twist-D    */
            /* devices, NON GT_OK for others.                           */
            fieldTypeArr[0] = CPSS_EXMX_INLIF_PCL_MAX_HOP_E;
            fieldValueArr[0].u32Field = 3;
            numOfFields = 1;
            vid = 1;

            st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);

            if (CPSS_PP_FAMILY_TWISTD_E == devFamily)
            {
                UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                                             "for Twist-D device: %d, %d, %d, %d, %d",
                                             dev, port, vid, fieldTypeArr[0],
                                             fieldValueArr[0].boolField);
            }
            else
            {/* non Twist-D */
                UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
                                             "for non Twist-D device: %d, %d, %d, %d, %d",
                                             dev, port, vid, fieldTypeArr[0],
                                             fieldValueArr[0].boolField);
            }

            /* 1.1.4. Call function with fieldTypePtr                        */
            /* [{CPSS_EXMX_INLIF_DIS_BRG_E; CPSS_EXMX_INLIF_EN_IPV4_E}]      */
            /* and fieldValue [{GT_TRUE; GT_TRUE}], numOfFields [2], vid [1].*/
            /* Expected: GT_OK for Samba/Twist (non-Tiger) devices,          */
            /* NON GT_OK for Tiger devices.                                  */
            fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
            fieldValueArr[0].boolField = GT_TRUE;
            fieldTypeArr[1] = CPSS_EXMX_INLIF_EN_IPV4_E;
            fieldValueArr[1].boolField = GT_TRUE;
            numOfFields = 2;
            vid = 1;

            st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);

            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                                             "for Twist/Samba device: %d, %d, %d, type[0] = %d, type[1] = %d",
                                             dev, port, vid, fieldTypeArr[0], fieldTypeArr[1]);
            }
            else
            {/* Tiger */
                UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
                                                 "for non Twist/Samba device: %d, %d, %d, type[0] = %d, type[1] = %d",
                                                 dev, port, vid, fieldTypeArr[0], fieldTypeArr[1]);
            }

            /* 1.1.5. Call function with fieldTypePtr                        */
            /* [{CPSS_EXMX_INLIF_MIRROR_2_CPU_E}] and fieldValue [{GT_TRUE}],*/
            /* numOfFields [1], vid [1]. Expected: GT_OK for Samba/Twist     */
            /* (non-Tiger) devices, NON GT_OK for Tiger devices.             */
            fieldTypeArr[0] = CPSS_EXMX_INLIF_MIRROR_2_CPU_E;
            fieldValueArr[0].boolField = GT_TRUE;
            numOfFields = 1;
            vid = 1;

            st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);

            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                                             "for Twist/Samba device: %d, %d, %d, %d, %d",
                                             dev, port, vid, fieldTypeArr[0],
                                             fieldValueArr[0].boolField);
            }
            else
            {/* Tiger */
                UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
                                             "for non Twist/Samba device: %d, %d, %d, %d, %d",
                                             dev, port, vid, fieldTypeArr[0],
                                             fieldValueArr[0].boolField);
            }

            /* 1.1.6. Call function with fieldTypePtr                       */
            /* [{CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E}] and fieldValue [{1}], */
            /* numOfFields [1]. Expected: GT_OK for Samba/Twist (non-Tiger) */
            /* devices, NON GT_OK for Tiger devices.                        */
            fieldTypeArr[0] = CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E;
            fieldValueArr[0].u32Field = 1;
            numOfFields = 1;
            vid = 1;

            st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);

            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                                             "for Twist/Samba device: %d, %d, %d, %d, %d",
                                             dev, port, vid, fieldTypeArr[0],
                                             fieldValueArr[0].boolField);
            }
            else
            {/* Tiger */
                UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
                                             "for non Twist/Samba device: %d, %d, %d, %d, %d",
                                             dev, port, vid, fieldTypeArr[0],
                                             fieldValueArr[0].boolField);
            }

            /* 1.1.7. For Twist and Samba devices call function with out of */
            /* range fieldType [{0x5AAAAAA5}] and fieldValue [{GT_TRUE}],   */
            /* numOfFields [1]. Expected: NON GT_OK.                        */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                fieldTypeArr[0] = INLIF_INVALID_ENUM_CNS;
                numOfFields = 1;
                vid = 1;

                st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);

                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                                 "for Twist/Samba device: %d, %d, %d, %d",
                                                 dev, port, vid, fieldTypeArr[0]);
            }

            /* 1.1.8. For Twist and Samba devices call function with        */
            /* fieldTypePtr [NULL] and fieldValue [{1}], numOfFields [1].   */
            /* Expected: GT_BAD_PTR                                         */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                numOfFields = 1;
                vid = 1;

                st = cpssExMxInlifFieldsSet(dev, port, vid, NULL, fieldValueArr, numOfFields);

                UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                                             "for Twist/Samba device: %d, %d, %d, NULL",
                                             dev, port, vid);
            }

            /* 1.1.9. For Twist and Samba devices call function with    */
            /* fieldTypePtr [{CPSS_EXMX_INLIF_DIS_BRG_E}] and           */
            /* fieldValue [NULL], numOfFields [1]. Expected: GT_BAD_PTR */  
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
                numOfFields = 1;
                vid = 1;

                st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, NULL, numOfFields);

                UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st,
                                             "for Twist/Samba device: %d, %d, %d, <fieldTypePtr>, NULL",
                                             dev, port, vid);
            }

            /* 1.1.10. Call function cpssExMxInlifPortLookupModeSet with    */
            /* inlifMode [CPSS_EXMX_MULTIPLE_INLIF_E].                      */
            /* Expected: GT_OK for Samba/Twist (non-Tiger) devices,         */
            /* NON GT_OK for Tiger devices.                                 */
            inlifMode = CPSS_EXMX_MULTIPLE_INLIF_E;

            st = cpssExMxInlifPortLookupModeSet(dev, port, inlifMode);
            if (CPSS_PP_FAMILY_TIGER_E != devFamily)
            {
                UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: non-Tiger device: %d, %d, %d",
                                             dev, port, inlifMode);
            }
            else
            {/* Tiger */
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxInlifPortLookupModeSet: Tiger device: %d, %d, %d",
                                             dev, port, inlifMode);
            }

            /* 1.1.11. Call function cpssExMxInlifVlanConfigGet with    */
            /* non-NULL vlanBaseAddPtr, portInLifMinVidPtr,             */
            /* portInLifMaxVidPtr. Expected: GT_OK for devices of       */
            /* Twist \Samba family (non-Tiger) and NON GT_OK for        */
            /* devices from other families.                             */
            st = cpssExMxInlifVlanConfigGet(dev, port, &vlanBaseAdd, &portInLifMinVid, &portInLifMaxVid);

            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                             "cpssExMxInlifVlanConfigGet: Twist/Samba device: %d, %d",
                                             dev, port);
            }
            else
            {/* Tiger */
                portInLifMinVid = 1;
                portInLifMaxVid = 1;
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, 
                                                 "cpssExMxInlifVlanConfigGet: non Twist/Samba device: %d, %d",
                                                 dev, port);
            }

            /* 1.1.12. Call function  with fieldTypePtr                     */
            /* [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}] and fieldValue [{GT_TRUE}], */
            /* numOfFields [1], vid [portInLifMinVidPtr].                   */
            /* Expected: GT_OK for Samba/Twist (non-Tiger) devices,         */
            /* NON GT_OK for Tiger devices.                                 */
            fieldTypeArr[0] = CPSS_EXMX_INLIF_EN_MPLS_MC_E;
            fieldValueArr[0].boolField = GT_TRUE;
            numOfFields = 1;
            vid = portInLifMinVid;

            st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st,
                                             "for Twist/Samba device: %d, %d, %d, %d, %d",
                                             dev, port, vid, fieldTypeArr[0],
                                             fieldValueArr[0].boolField);
            }
            else
            {/* Tiger */
                UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
                                             "for non Twist/Samba device: %d, %d, %d, %d, %d",
                                             dev, port, vid, fieldTypeArr[0],
                                             fieldValueArr[0].boolField);
            }

            /* 1.1.13. For Twist and Samba devices call function  with  */
            /* fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}] and        */
            /* fieldValue [{GT_TRUE}], numOfFields [1],                 */
            /* vid [portInLifMaxVidPtr + 1]. Expected: NON GT_OK.       */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                fieldTypeArr[0] = CPSS_EXMX_INLIF_EN_MPLS_MC_E;
                fieldValueArr[0].boolField = GT_TRUE;
                numOfFields = 1;
                vid = portInLifMaxVid + 1;

                st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);

                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                                 "for Twist/Samba device: %d, %d, %d",
                                                 dev, port, vid);
            }

            /* 1.1.14. For Twist and Samba devices call function    */
            /* with fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}]   */
            /* and fieldValue [{GT_TRUE}], numOfFields [1],         */
            /* vid [portInLifMinVidPtr - 1]. Expected: NON GT_OK.   */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                fieldTypeArr[0] = CPSS_EXMX_INLIF_EN_MPLS_MC_E;
                fieldValueArr[0].boolField = GT_TRUE;
                numOfFields = 1;
                vid = portInLifMinVid - 1;

                st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);

                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                                "for Twist/Samba device: %d, %d, %d",
                                                dev, port, vid);
            }

            /* 1.1.15. For Twist and Samba devices call function        */
            /* with fieldTypePtr [{CPSS_EXMX_INLIF_EN_MPLS_MC_E}]       */
            /* and fieldValue [{GT_TRUE}], numOfFields [1],             */
            /* vid [PRV_CPSS_MAX_NUM_VLANS_CNS].Expected: NON GT_OK.    */
            if (IS_TWIST_OR_SAMBA_MAC(devFamily))
            {
                fieldTypeArr[0] = CPSS_EXMX_INLIF_EN_MPLS_MC_E;
                fieldValueArr[0].boolField = GT_TRUE;
                numOfFields = 1;
                vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

                st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);

                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                                 "for Twist/Samba device: %d, %d, %d",
                                                 dev, port, vid);
            }
        }

        if (IS_TWIST_OR_SAMBA_MAC(devFamily))
        {
            fieldTypeArr[0] = CPSS_EXMX_INLIF_EN_MPLS_MC_E;
            fieldValueArr[0].boolField = GT_TRUE;
            numOfFields = 1;
            vid = 1;

            st = prvUtfNextVirtPortReset(&port, dev);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

            /* 1.3. Go over all non active virtual ports and active device id. */
            while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
            {
                /* 1.3.1. <Call function for non active port and valid parameters>.     */
                /* Expected: GT_BAD_PARAM.                                              */
                st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
            }

            /* 1.4. Call with out of range for virtual port.     */
            /* Expected: GT_BAD_PARAM.                           */
            port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
    }

    fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
    fieldValueArr[0].boolField = GT_TRUE;
    numOfFields = 1;
    vid = 1;

    port = INLIF_VALID_VIRT_PORT_CNS;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxInlifFieldsSet(dev, port, vid, fieldTypeArr, fieldValueArr, numOfFields);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxInlif suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxInlif)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxInlifPortEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxInlifPortEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxInlifPortLookupModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxInlifPortLookupModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxInlifDefaultEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxInlifFieldsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxInlifVlanEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxInlifVlanEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxInlifVlanConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxInlifVlanConfigGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxInlif)

/*******************************************************************************
* prvUtfMaxNumOfLifsGet
*
* DESCRIPTION:
*       This routine returns max number of interfaces need to be supported per device.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*        maxNumOfLifsPtr     -  max number of interfaces need to be supported
*
*       GT_OK           -   Get max number of interfaces need to be supported was OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfMaxNumOfLifsGet
(
    IN  GT_U8       dev,
    OUT GT_U32      *maxNumOfLifsPtr
)
{
    PRV_CPSS_EXMX_PP_CONFIG_STC  *exMxDevPtr;

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(maxNumOfLifsPtr);

    /* check if dev active */
    PRV_CPSS_EXMX_DEV_CHECK_MAC(dev);
    exMxDevPtr = PRV_CPSS_EXMX_PP_MAC(dev);

    *maxNumOfLifsPtr = exMxDevPtr->moduleCfg.logicalIfCfg.maxNumOfLifs;

    return GT_OK;
}
