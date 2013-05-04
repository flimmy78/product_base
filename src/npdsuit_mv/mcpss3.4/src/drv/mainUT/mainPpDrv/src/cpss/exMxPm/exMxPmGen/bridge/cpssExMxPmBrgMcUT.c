/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmBrgGenUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgGen, that provides
*       CPSS ExMxPm Multicast Group facility API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgMc.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define BRG_MC_VALID_VIRT_PORT_CNS 0


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgMcIpv6BytesSelectSet
(
    IN GT_U8  devNum,
    IN GT_U8  dipBytesSelectMap[4],
    IN GT_U8  sipBytesSelectMap[4]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgMcIpv6BytesSelectSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with dipBytesSelectMap [{0,2,3,5}]
                   and sipBytesSelectMap [{6, 7, 8, 15}]
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgMcIpv6BytesSelectGet.
    Expected: GT_OK and the same dipBytesSelectMap and sipBytesSelectMap.
    1.3. Call with dipBytesSelectMap [{0,2,3,15}]
                   and sipBytesSelectMap [{3, 7, 8, 15}]
    Expected: GT_OK.
    1.4. Call with equal values of bytes numbers dipBytesSelectMap[{0, 0, 1, 1}]
                   and other valid parameters from 1.1.
    Expected: GT_OK.
    1.5. Call with equal values of bytes numbers sipBytesSelectMap[{0, 0, 1, 1}]
                   and other valid parameters from 1.1.
    Expected: GT_OK.
    1.6. Call with out of range dipBytesSelectMap[{16, 0, 1, 2}]
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK.
    1.7. Call with out of range sipBytesSelectMap[{0, 1, 2, 16}]
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK.
    1.8. Call with null dipBytesSelectMap [NULL]
                    and other valid parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.9. Call with null sipBytesSelectMap[NULL]
                    and other valid parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       dipBytesMap[4];
    GT_U8       sipBytesMap[4];
    GT_U8       dipBytesMapGet[4];
    GT_U8       sipBytesMapGet[4];
    GT_BOOL     isEqual = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with dipBytesSelectMap [{0,2,3,5}]
                           and sipBytesSelectMap [{6, 7, 8, 15}]
            Expected: GT_OK.
        */
        dipBytesMap[0] = 0;
        dipBytesMap[1] = 2;
        dipBytesMap[2] = 3;
        dipBytesMap[3] = 5;

        sipBytesMap[0] = 6;
        sipBytesMap[1] = 7;
        sipBytesMap[2] = 8;
        sipBytesMap[3] = 15;

        st = cpssExMxPmBrgMcIpv6BytesSelectSet(dev, dipBytesMap, sipBytesMap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call cpssExMxPmBrgMcIpv6BytesSelectGet.
            Expected: GT_OK and the same dipBytesSelectMap and sipBytesSelectMap.
        */
        st = cpssExMxPmBrgMcIpv6BytesSelectGet(dev, dipBytesMapGet, sipBytesMapGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgMcIpv6BytesSelectGet: %d", dev);

        /* Verifying dipBytesSelectMap */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) dipBytesMap,
                                     (GT_VOID*) dipBytesMapGet,
                                     sizeof(dipBytesMap))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another dipBytesSelectMap than was set: %d", dev);

        /* Verifying sipBytesSelectMap */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) sipBytesMap,
                                     (GT_VOID*) sipBytesMapGet,
                                     sizeof(sipBytesMap))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                   "get another sipBytesSelectMap than was set: %d", dev);

        /*
            1.3. Call with dipBytesSelectMap [{0,2,3,15}]
                           and sipBytesSelectMap [{3, 7, 8, 15}]
            Expected: GT_OK.
        */
        dipBytesMap[0] = 0;
        dipBytesMap[1] = 2;
        dipBytesMap[2] = 3;
        dipBytesMap[3] = 15;

        sipBytesMap[0] = 3;
        sipBytesMap[1] = 7;
        sipBytesMap[2] = 8;
        sipBytesMap[3] = 15;

        st = cpssExMxPmBrgMcIpv6BytesSelectSet(dev, dipBytesMap, sipBytesMap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.4. Call with equal values of bytes numbers dipBytesSelectMap[{0, 0, 1, 1}]
                           and other valid parameters from 1.1.
            Expected: GT_OK.
        */
        dipBytesMap[0] = 0;
        dipBytesMap[1] = 0;
        dipBytesMap[2] = 1;
        dipBytesMap[3] = 1;

        st = cpssExMxPmBrgMcIpv6BytesSelectSet(dev, dipBytesMap, sipBytesMap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        dipBytesMap[0] = 0;
        dipBytesMap[1] = 2;
        dipBytesMap[2] = 3;
        dipBytesMap[3] = 5;

        /*
            1.5. Call with equal values of bytes numbers sipBytesSelectMap[{0, 0, 1, 1}]
                           and other valid parameters from 1.1.
            Expected: GT_OK.
        */
        sipBytesMap[0] = 0;
        sipBytesMap[1] = 0;
        sipBytesMap[2] = 1;
        sipBytesMap[3] = 1;

        st = cpssExMxPmBrgMcIpv6BytesSelectSet(dev, dipBytesMap, sipBytesMap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        sipBytesMap[0] = 6;
        sipBytesMap[1] = 7;
        sipBytesMap[2] = 8;
        sipBytesMap[3] = 15;

        /*
            1.8. Call with out of range dipBytesSelectMap[{16, 0, 1, 2}]
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK.
        */
        dipBytesMap[0] = 16;
        dipBytesMap[1] = 0;
        dipBytesMap[2] = 1;
        dipBytesMap[3] = 2;

        st = cpssExMxPmBrgMcIpv6BytesSelectSet(dev, dipBytesMap, sipBytesMap);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dipBytesSelectMap[0] = %d", dev, dipBytesMap[0]);

        dipBytesMap[0] = 0;
        dipBytesMap[1] = 2;
        dipBytesMap[2] = 3;
        dipBytesMap[3] = 5;

        /*
            1.9. Call with out of range sipBytesSelectMap[{0, 1, 2, 16}]
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK.
        */
        sipBytesMap[0] = 0;
        sipBytesMap[1] = 1;
        sipBytesMap[2] = 2;
        sipBytesMap[3] = 16;

        st = cpssExMxPmBrgMcIpv6BytesSelectSet(dev, dipBytesMap, sipBytesMap);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, sipBytesSelectMap[0] = %d", dev, sipBytesMap[0]);

        sipBytesMap[0] = 6;
        sipBytesMap[1] = 7;
        sipBytesMap[2] = 8;
        sipBytesMap[3] = 15;

        /*
            1.10. Call with null dipBytesSelectMap [NULL]
                            and other valid parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgMcIpv6BytesSelectSet(dev, NULL, sipBytesMap);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dipBytesSelectMap = NULL", dev);

        /*
            1.11. Call with null sipBytesSelectMap[NULL]
                            and other valid parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgMcIpv6BytesSelectSet(dev, dipBytesMap, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, sipBytesSelectMap = NULL", dev);
    }

    dipBytesMap[0] = 0;
    dipBytesMap[1] = 2;
    dipBytesMap[2] = 3;
    dipBytesMap[3] = 5;

    sipBytesMap[0] = 6;
    sipBytesMap[1] = 7;
    sipBytesMap[2] = 8;
    sipBytesMap[3] = 15;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgMcIpv6BytesSelectSet(dev, dipBytesMap, sipBytesMap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgMcIpv6BytesSelectSet(dev, dipBytesMap, sipBytesMap);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgMcIpv6BytesSelectGet
(
    IN  GT_U8  devNum,
    OUT GT_U8  dipBytesSelectMap[4],
    OUT GT_U8  sipBytesSelectMap[4]
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgMcIpv6BytesSelectGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null dipBytesSelectMap
                   and non-null sipBytesSelectMap.
    Expected: GT_OK.
    1.2. Call with null dipBytesSelectMap[NULL]
                   and non-null sipBytesSelectMap.
    Expected: GT_BAD_PTR.
    1.3. Call with non-null dipBytesSelectMap
                   and null sipBytesSelectMap [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U8       dipBytesMap[4];
    GT_U8       sipBytesMap[4];


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null dipBytesSelectMap
                           and non-null sipBytesSelectMap.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgMcIpv6BytesSelectGet(dev, dipBytesMap, sipBytesMap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null dipBytesSelectMap [NULL]
                           and other valid parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgMcIpv6BytesSelectGet(dev, NULL, sipBytesMap);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, dipBytesSelectMap = NULL", dev);

        /*
            1.3. Call with null sipBytesSelectMap[NULL]
                           and other valid parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgMcIpv6BytesSelectGet(dev, dipBytesMap, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, sipBytesSelectMap = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgMcIpv6BytesSelectGet(dev, dipBytesMap, sipBytesMap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgMcIpv6BytesSelectGet(dev, dipBytesMap, sipBytesMap);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgMcEntryWrite
(
    IN GT_U8                devNum,
    IN GT_U16               vidx,
    IN GT_BOOL              isCpuMember,
    IN CPSS_PORTS_BMP_STC   *portBitmapPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgMcEntryWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with vidx [100/ 1023],
                   isCpuMember [GT_FALSE and GT_TRUE]
                   and portBitmapPtr->ports[{3, 0} / {12, 0}].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgMcEntryRead with the same vidx, non-NULL portBitmapPtr and non-NULLisCpuMemberPtr.
    Expected: GT_OK and the same portBitmapPtr and isCpuMember.
    1.3. Call with out of range vidx [PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS]
                   and other valid parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with out of range portBitmapPtr [{56}]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.5. Call with null portBitmapPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16               vidx = 0;
    GT_BOOL              isCpuMember = GT_FALSE;
    GT_BOOL              isCpuMemGet = GT_FALSE;
    CPSS_PORTS_BMP_STC   portBitmap;
    CPSS_PORTS_BMP_STC   portBitmapGet;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vidx [100/ 1023],
                           isCpuMember [GT_FALSE and GT_TRUE]
                           and portBitmapPtr->ports[{3, 0} / {12, 0}].
            Expected: GT_OK.
        */

        /* Call with vidx [100] */
        vidx        = 100;
        isCpuMember = GT_FALSE;

        portBitmap.ports[0] = 3;
        portBitmap.ports[1] = 0;

        st = cpssExMxPmBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, isCpuMember);

        /*
            1.2. Call cpssExMxPmBrgMcEntryRead with the same vidx, non-NULL portBitmapPtr and non-NULLisCpuMemberPtr.
            Expected: GT_OK and the same portBitmapPtr and isCpuMember.
        */
        st = cpssExMxPmBrgMcEntryRead(dev, vidx, &isCpuMemGet, &portBitmapGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgMcEntryRead: %d, %d", dev, vidx);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(isCpuMember, isCpuMemGet,
                       "get another isCpuMember than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(portBitmap.ports[0], portBitmapGet.ports[0],
                       "get another portBitmapPtr->ports[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(portBitmap.ports[1], portBitmapGet.ports[1],
                       "get another portBitmapPtr->ports[1] than was set: %d", dev);

        /* Call with vidx [1023] */
        vidx        = 1023;
        isCpuMember = GT_TRUE;

        portBitmap.ports[0] = 12;
        portBitmap.ports[1] = 0;

        st = cpssExMxPmBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, isCpuMember);

        /*
            1.2. Call cpssExMxPmBrgMcEntryRead with the same vidx, non-NULL portBitmapPtr and non-NULLisCpuMemberPtr.
            Expected: GT_OK and the same portBitmapPtr and isCpuMember.
        */
        st = cpssExMxPmBrgMcEntryRead(dev, vidx, &isCpuMemGet, &portBitmapGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgMcEntryRead: %d, %d", dev, vidx);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(isCpuMember, isCpuMemGet,
                       "get another isCpuMember than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(portBitmap.ports[0], portBitmapGet.ports[0],
                       "get another portBitmapPtr->ports[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(portBitmap.ports[1], portBitmapGet.ports[1],
                       "get another portBitmapPtr->ports[1] than was set: %d", dev);

        /*
            1.3. Call with out of range vidx [PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS]
                           and other valid parameters from 1.1.
            Expected: NOT GT_OK.
        */
        vidx = PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS;

        st = cpssExMxPmBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);

        vidx = 100;

        /*
            1.4. Call with out of range portBitmapPtr [{56}]
                           and other parameters the same as in 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        portBitmap.ports[1] = 0x1000000;

        st = cpssExMxPmBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, portBitmapPtr->ports[1] = %d",
                                         dev, portBitmap.ports[1]);

        portBitmap.ports[0] = 3;
        portBitmap.ports[0] = 0;

        /*
            1.5. Call with portBitmapPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgMcEntryWrite(dev, vidx, isCpuMember, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portBitmapPtr = NULL", dev);
    }

    vidx = 100;

    portBitmap.ports[0] = 3;
    portBitmap.ports[1] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgMcEntryRead
(
    IN  GT_U8                devNum,
    IN  GT_U16               vidx,
    OUT GT_BOOL              *isCpuMemberPtr,
    OUT CPSS_PORTS_BMP_STC   *portBitmapPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgMcEntryRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with vidx [100 / 1023]
                   non-null portBitmapPtr,
                   and non-NULL isCpuMemberPtr.
    Expected: GT_OK.
    1.2. Call with out of range vidx [PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS]
                   and non-null portBitmapPtr.
    Expected: NOT GT_OK.
    1.3. Call with null isCpuMemberPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with null portBitmapPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16               vidx        = 0;
    GT_BOOL              isCpuMember = GT_FALSE;
    CPSS_PORTS_BMP_STC   portBitmap;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vidx [100 / 1023]
                           non-null portBitmapPtr,
                           and non-NULL isCpuMemberPtr.
            Expected: GT_OK.
        */

        /* Call with vidx [100] */
        vidx = 100;

        st = cpssExMxPmBrgMcEntryRead(dev, vidx, &isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);

        /* Call with vidx [1023] */
        vidx = 1023;

        st = cpssExMxPmBrgMcEntryRead(dev, vidx, &isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);

        /*
            1.2. Call with out of range vidx [PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS]
                           and non-null portBitmapPtr.
            Expected: NOT GT_OK.
        */
        vidx = PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS;

        st = cpssExMxPmBrgMcEntryRead(dev, vidx, &isCpuMember, &portBitmap);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);

        vidx = 100;

        /*
            1.3. Call with null isCpuMemberPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgMcEntryRead(dev, vidx, NULL, &portBitmap);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, isCpuMemberPtr = NULL", dev);

        /*
            1.4. Call with portBitmapPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgMcEntryRead(dev, vidx, &isCpuMember, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portBitmapPtr = NULL", dev);
    }

    vidx = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgMcEntryRead(dev, vidx, &isCpuMember, &portBitmap);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgMcEntryRead(dev, vidx, &isCpuMember, &portBitmap);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgMcMemberAdd
(
    IN GT_U8   devNum,
    IN GT_U16  vidx,
    IN GT_U8   portNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgMcMemberAdd)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with vidx [100 / 1023]
                   and portNum [0 / 16].
    Expected: GT_OK.
    1.2. Call with out of range portNum [56]
                   and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call with out of range vidx [PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS = 1024]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16      vidx    = 0;
    GT_U8       portNum = BRG_MC_VALID_VIRT_PORT_CNS;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vidx [100 / 1023]
                           and portNum [0 / 16].
            Expected: GT_OK.
        */

        /* Call with vidx [100] */
        vidx    = 100;
        portNum = BRG_MC_VALID_VIRT_PORT_CNS;

        st = cpssExMxPmBrgMcMemberAdd(dev, vidx, portNum);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, portNum);

        /* Call with vidx [1023] */
        vidx    = 1023;
        portNum = 16;

        st = cpssExMxPmBrgMcMemberAdd(dev, vidx, portNum);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, portNum);

        /*
            1.2. Call with out of range portNum [56]
                           and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        portNum = 56;

        st = cpssExMxPmBrgMcMemberAdd(dev, vidx, portNum);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, portNum = %d", dev, portNum);

        portNum = BRG_MC_VALID_VIRT_PORT_CNS;

        /*
            1.3. Call with out of range vidx [PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        vidx = PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS;

        st = cpssExMxPmBrgMcMemberAdd(dev, vidx, portNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);
    }

    vidx    = 100;
    portNum = BRG_MC_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgMcMemberAdd(dev, vidx, portNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgMcMemberAdd(dev, vidx, portNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgMcMemberDelete
(
    IN GT_U8          devNum,
    IN GT_U16         vidx,
    IN GT_U8          portNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgMcMemberDelete)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with vidx [100 / 1023]
                   and portNum [0 / 16].
    Expected: GT_OK.
    1.2. Call with out of range portNum [56]
                   and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call with out of range vidx [PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS]
                   and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16      vidx    = 0;
    GT_U8       portNum = BRG_MC_VALID_VIRT_PORT_CNS;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with vidx [100 / 1023]
                           and portNum [0 / 16].
            Expected: GT_OK.
        */

        /* Call with vidx [100] */
        vidx    = 100;
        portNum = BRG_MC_VALID_VIRT_PORT_CNS;

        st = cpssExMxPmBrgMcMemberDelete(dev, vidx, portNum);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, portNum);

        /* Call with vidx [1023] */
        vidx    = 1023;
        portNum = 16;

        st = cpssExMxPmBrgMcMemberDelete(dev, vidx, portNum);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vidx, portNum);

        /*
            1.2. Call with out of range portNum [56]
                           and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        portNum = 56;

        st = cpssExMxPmBrgMcMemberDelete(dev, vidx, portNum);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, portNum = %d", dev, portNum);

        portNum = BRG_MC_VALID_VIRT_PORT_CNS;

        /*
            1.3. Call with out of range vidx [PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS]
                           and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        vidx = PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS;

        st = cpssExMxPmBrgMcMemberDelete(dev, vidx, portNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, vidx);
    }

    vidx    = 100;
    portNum = BRG_MC_VALID_VIRT_PORT_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgMcMemberDelete(dev, vidx, portNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgMcMemberDelete(dev, vidx, portNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Multicast Group table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgMcFillMulticastGroupTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in Multicast Group table.
         Call cpssExMxPmBrgMcEntryWrite with vidx [0 ... numEntries-1],
                                           isCpuMember [GT_FALSE]
                                           and portBitmapPtr->ports[{3, 0}].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmBrgMcEntryWrite with vidx [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in Multicast Group table and compare with original.
         Call cpssExMxPmBrgMcEntryRead with the same vidx, non-NULL portBitmapPtr and non-NULL isCpuMemberPtr.
    Expected: GT_OK and the same portBitmapPtr and isCpuMember.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmBrgMcEntryRead with vidx [numEntries] and other params from 1.4.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16      numEntries  = 0;
    GT_U16      iTemp       = 0;
    GT_BOOL     isCpuMember = GT_FALSE;
    GT_BOOL     isCpuMemGet = GT_FALSE;

    CPSS_PORTS_BMP_STC   portBitmap;
    CPSS_PORTS_BMP_STC   portBitmapGet;


    cpssOsBzero((GT_VOID*) &portBitmap, sizeof(portBitmap));
    cpssOsBzero((GT_VOID*) &portBitmapGet, sizeof(portBitmapGet));

    /* Fill the entry for Multicast Group table */
    isCpuMember = GT_FALSE;

    portBitmap.ports[0] = 3;
    portBitmap.ports[1] = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS;

        /* 1.2. Fill all entries in Multicast Group table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            portBitmap.ports[0] = iTemp % ((GT_U32) (1 << 31));
            portBitmap.ports[1] = iTemp % ((GT_U32) (1 << 22));

            st = cpssExMxPmBrgMcEntryWrite(dev, iTemp, isCpuMember, &portBitmap);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcEntryWrite: %d, %d, %d", dev, iTemp, isCpuMember);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmBrgMcEntryWrite(dev, numEntries, isCpuMember, &portBitmap);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcEntryWrite: %d, %d, %d", dev, iTemp, isCpuMember);

        /* 1.4. Read all entries in Multicast Group table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            portBitmap.ports[0] = iTemp % ((GT_U32) (1 << 31));
            portBitmap.ports[1] = iTemp % ((GT_U32) (1 << 22));

            st = cpssExMxPmBrgMcEntryRead(dev, iTemp, &isCpuMemGet, &portBitmapGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmBrgMcEntryRead: %d, %d", dev, iTemp);

            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(isCpuMember, isCpuMemGet,
                           "get another isCpuMember than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(portBitmap.ports[0], portBitmapGet.ports[0],
                           "get another portBitmapPtr->ports[0] than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(portBitmap.ports[1], portBitmapGet.ports[1],
                           "get another portBitmapPtr->ports[1] than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmBrgMcEntryRead(dev, numEntries, &isCpuMemGet, &portBitmapGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcEntryRead: %d, %d", dev, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test for Functionality
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgMcFunctionality)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Call cpssExMxPmBrgMcEntryWrite with vidx [0 .. 4095],
                   isCpuMember [GT_TRUE]
                   and portBitmapPtr->ports[{0, 0}].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgMcEntryWrite with vidx [4096] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call cpssExMxPmBrgMcMemberAdd with vidx [0 .. 4095]
                                       and portNum [0 .. 63].
    Expected: GT_OK.
    1.4 Call cpssExMxPmBrgMcMemberAdd with portNum [64] (out of range) and other params from 1.3.
    Expected: NOT GT_OK.
    1.5 Call cpssExMxPmBrgMcMemberAdd with vidx [4096] (out of range) and other params from 1.3.
    Expected: NOT GT_OK.
    1.6. Call cpssExMxPmBrgMcEntryRead with vidx [0 .. 4095], non-NULL portBitmapPtr and non-NULLisCpuMemberPtr.
    Expected: GT_OK and portBitmapPtr->ports [{0x1FFFFFFF, 0x0}] and isCpuMember [TRUE].
    1.7. Call cpssExMxPmBrgMcEntryRead with vidx [4096] (out of range) and other params from 1.6.
    Expected: NOT GT_OK.
    1.8. Call cpssExMxPmBrgMcMemberDelete with vidx [0 .. 4095]
                                          and portNum [0 .. 63].
    Expected: GT_OK.
    1.9 Call cpssExMxPmBrgMcMemberDelete with portNum [64] (out of range) and other params from 1.8.
    Expected: NOT GT_OK.
    1.10 Call cpssExMxPmBrgMcMemberDelete with vidx [4096] (out of range) and other params from 1.8.
    Expected: NOT GT_OK.
    1.11. Call cpssExMxPmBrgMcEntryRead with vidx [0 .. 4095], non-NULL portBitmapPtr and non-NULLisCpuMemberPtr.
    Expected: GT_OK and portBitmapPtr->ports [{0, 0}] and isCpuMember [TRUE].
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32               iTemp = 0;
    GT_U32               jTemp = 0;
    GT_BOOL              isCpuMember = GT_FALSE;
    GT_BOOL              isCpuMemberGet = GT_FALSE;
    CPSS_PORTS_BMP_STC   portBitmap;
    CPSS_PORTS_BMP_STC   portBitmapGet;
    GT_U8                portNum = BRG_MC_VALID_VIRT_PORT_CNS;
    GT_U16               vidx    = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmBrgMcEntryWrite with vidx [0 .. 4095],
                           isCpuMember [GT_TRUE]
                           and portBitmapPtr->ports[{0, 0}].
            Expected: GT_OK.
        */
        isCpuMember = GT_TRUE;

        portBitmap.ports[0] = 0;
        portBitmap.ports[1] = 0;

        for(iTemp=0; iTemp<PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS; ++iTemp)
        {
            vidx = (GT_U16)iTemp;
   
            st = cpssExMxPmBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcEntryWrite: %d, %d, %d", dev, vidx, isCpuMember);
        }

        /*
            1.2. Call cpssExMxPmBrgMcEntryWrite with vidx [4096] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        vidx = PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS;

        st = cpssExMxPmBrgMcEntryWrite(dev, vidx, isCpuMember, &portBitmap);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcEntryWrite: %d, %d", dev, vidx);

        /*
            1.3. Call cpssExMxPmBrgMcMemberAdd with vidx [0 .. 4095]
                                               and portNum [0 .. 63].
            Expected: GT_OK.
        */
        for(iTemp=0; iTemp<PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS; ++iTemp)
        {
                vidx = (GT_U16)iTemp;
                for(jTemp=0; jTemp<UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev); ++jTemp)
                {
                    portNum = (GT_U8)jTemp;
        
                    st = cpssExMxPmBrgMcMemberAdd(dev, vidx, portNum);
                    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcMemberAdd: %d, %d, %d", dev, vidx, portNum);
                }

                /*
                    1.4 Call cpssExMxPmBrgMcMemberAdd with portNum [64] (out of range) and other params from 1.3.
                    Expected: NOT GT_OK.
                */
                portNum = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);
    
                st = cpssExMxPmBrgMcMemberAdd(dev, vidx, portNum);
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcMemberAdd: %d, %d, %d", dev, vidx, portNum);
        }

        /*
            1.5 Call cpssExMxPmBrgMcMemberAdd with vidx [4096] (out of range) and other params from 1.3.
            Expected: NOT GT_OK.
        */
        vidx = PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS;

        st = cpssExMxPmBrgMcMemberAdd(dev, vidx, portNum);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcMemberAdd: %d, %d", dev, vidx);

        /*
            1.6. Call cpssExMxPmBrgMcEntryRead with vidx [0 .. 4095], non-NULL portBitmapPtr and non-NULLisCpuMemberPtr.
            Expected: GT_OK and portBitmapPtr->ports [{0x1FFFFFFF, 0x0}] and isCpuMember [TRUE].
        */
        for(iTemp=0; iTemp<PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS; ++iTemp)
        {
            vidx = (GT_U16)iTemp;

            st = cpssExMxPmBrgMcEntryRead(dev, vidx, &isCpuMemberGet, &portBitmapGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcEntryRead: %d, %d", dev, vidx);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isCpuMemberGet,
                           "get another isCpuMember than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(0xFFFFFFFF, portBitmapGet.ports[0],
                           "get another portBitmapPtr->ports[0] than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(0x3FFFFF, portBitmapGet.ports[1],
                           "get another portBitmapPtr->ports[1] than was set: %d", dev);
        }

        /*
            1.7. Call cpssExMxPmBrgMcEntryRead with vidx [4096] (out of range) and other params from 1.6.
            Expected: NOT GT_OK.
        */
        vidx = PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS;

        st = cpssExMxPmBrgMcEntryRead(dev, vidx, &isCpuMemberGet, &portBitmapGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcEntryRead: %d, %d", dev, vidx);
        
        /*
            1.8. Call cpssExMxPmBrgMcMemberDelete with vidx [0 .. 4095]
                                                  and portNum [0 .. 63].
            Expected: GT_OK.
        */
        for(iTemp=0; iTemp<PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS; ++iTemp)
        {
                vidx = (GT_U16)iTemp;
                for(jTemp=0; jTemp<UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev); ++jTemp)
                {
                    portNum = (GT_U8)jTemp;
        
                    st = cpssExMxPmBrgMcMemberDelete(dev, vidx, portNum);
                    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcMemberDelete: %d, %d, %d", dev, vidx, portNum);
                }

                /*
                    1.9 Call cpssExMxPmBrgMcMemberDelete with portNum [64] (out of range) and other params from 1.8.
                    Expected: NOT GT_OK.
                */
                portNum = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);
    
                st = cpssExMxPmBrgMcMemberDelete(dev, vidx, portNum);
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcMemberDelete: %d, %d, %d", dev, vidx, portNum);
        }

        /*
            1.10 Call cpssExMxPmBrgMcMemberDelete with vidx [4096] (out of range) and other params from 1.8.
            Expected: NOT GT_OK.
        */
        vidx = PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS;

        st = cpssExMxPmBrgMcMemberDelete(dev, vidx, portNum);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcMemberDelete: %d, %d", dev, vidx);

        /*
            1.11. Call cpssExMxPmBrgMcEntryRead with vidx [0 .. 4095], non-NULL portBitmapPtr and non-NULLisCpuMemberPtr.
            Expected: GT_OK and portBitmapPtr->ports [{0, 0}] and isCpuMember [TRUE].
        */
        for(iTemp=0; iTemp<PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS; ++iTemp)
        {
            vidx = (GT_U16)iTemp;

            st = cpssExMxPmBrgMcEntryRead(dev, vidx, &isCpuMemberGet, &portBitmapGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgMcEntryRead: %d, %d", dev, vidx);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isCpuMemberGet,
                           "get another isCpuMember than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(0, portBitmapGet.ports[0],
                           "get another portBitmapPtr->ports[0] than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(0, portBitmapGet.ports[1],
                           "get another portBitmapPtr->ports[1] than was set: %d", dev);
        }

    }

}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmBrgMc suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmBrgMc)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgMcIpv6BytesSelectSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgMcIpv6BytesSelectGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgMcEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgMcEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgMcMemberAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgMcMemberDelete)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgMcFillMulticastGroupTable)
    /* Test for Functionality */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgMcFunctionality)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmBrgMc)

