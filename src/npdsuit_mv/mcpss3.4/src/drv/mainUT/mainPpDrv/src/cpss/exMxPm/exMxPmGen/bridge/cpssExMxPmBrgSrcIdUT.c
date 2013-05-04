/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmBrgSrcIdUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgSrcId, that provides
*       CPSS ExMxPm Source Id facility API
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgSrcId.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define BRG_SRC_ID_VALID_PHY_PORT_CNS  0

/* Default valid value for port id */
#define BRG_SRC_ID_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define BRG_SRC_ID_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdGroupPortAdd
(
    IN GT_U8   devNum,
    IN GT_U32  sourceId,
    IN GT_U8   portNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdGroupPortAdd)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with sourceId [0 / 31].
    Expected: GT_OK.
    1.1.2. Call with out of range sourceId [32].
    Expected: NON GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_SRC_ID_VALID_VIRT_PORT_CNS;

    GT_U32      sourceId = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtsical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with sourceId [0 / 31].
                Expected: GT_OK.
            */

            /* Call with sourceId [0] */
            sourceId = 0;

            st = cpssExMxPmBrgSrcIdGroupPortAdd(dev, sourceId, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, sourceId, port);

            /* Call with sourceId [31] */
            sourceId = 31;

            st = cpssExMxPmBrgSrcIdGroupPortAdd(dev, sourceId, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, sourceId, port);

            /*
                1.1.2. Call with out of range sourceId [32].
                Expected: NON GT_OK.
            */
            sourceId = 32;

            st = cpssExMxPmBrgSrcIdGroupPortAdd(dev, sourceId, port);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, sourceId, port);
        }

        sourceId = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtsical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgSrcIdGroupPortAdd(dev, sourceId, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgSrcIdGroupPortAdd(dev, sourceId, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgSrcIdGroupPortAdd(dev, sourceId, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    sourceId = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_SRC_ID_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdGroupPortAdd(dev, sourceId, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdGroupPortAdd(dev, sourceId, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdGroupPortDelete
(
    IN GT_U8   devNum,
    IN GT_U32  sourceId,
    IN GT_U8   portNum
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdGroupPortDelete)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with sourceId [0 / 31].
    Expected: GT_OK.
    1.1.2. Call with out of range sourceId [32].
    Expected: NON GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_SRC_ID_VALID_VIRT_PORT_CNS;

    GT_U32      sourceId = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtsical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with sourceId [0 / 31].
                Expected: GT_OK.
            */

            /* Call with sourceId [0] */
            sourceId = 0;

            st = cpssExMxPmBrgSrcIdGroupPortDelete(dev, sourceId, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, sourceId, port);

            /* Call with sourceId [31] */
            sourceId = 31;

            st = cpssExMxPmBrgSrcIdGroupPortDelete(dev, sourceId, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, sourceId, port);

            /*
                1.1.2. Call with out of range sourceId [32].
                Expected: NON GT_OK.
            */
            sourceId = 32;

            st = cpssExMxPmBrgSrcIdGroupPortDelete(dev, sourceId, port);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, sourceId, port);
        }

        sourceId = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtsical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgSrcIdGroupPortDelete(dev, sourceId, port);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgSrcIdGroupPortDelete(dev, sourceId, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgSrcIdGroupPortDelete(dev, sourceId, port);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    sourceId = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_SRC_ID_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdGroupPortDelete(dev, sourceId, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdGroupPortDelete(dev, sourceId, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdGroupEntrySet
(
    IN GT_U8               devNum,
    IN GT_U32              sourceId,
    IN GT_BOOL             cpuSrcIdMember,
    IN CPSS_PORTS_BMP_STC  *portsMembersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdGroupEntrySet)
{
/*
    ITERATE_DEVICES(ExMxPm)
    1.1. Call with sourceId [0 / 31],
                   cpuSrcIdMember[GT_TRUE / GT_FALSE]
                   and portsMembersPtr->ports[{3, 0} / {12, 0}].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgSrcIdGroupEntryGet with the same sourceId and non-NULL cpuSrcIdMemberPtr and non-NULL portsMembersPtr.
    Expected: GT_OK and the same cpuSrcIdMember and  portsMembersPtr.
    1.3. Call with out of range sourceId [32]
                   and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.4. Call with portsMembersPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32              sourceId          = 0;
    GT_BOOL             cpuSrcIdMember    = GT_FALSE;
    GT_BOOL             cpuSrcIdMemberGet = GT_FALSE;
    CPSS_PORTS_BMP_STC  portsMembers;
    CPSS_PORTS_BMP_STC  portsMembersGet;

    cpssOsMemSet(&portsMembersGet,0,sizeof(portsMembersGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with sourceId [0 / 31],
                           cpuSrcIdMember[GT_TRUE / GT_FALSE]
                           and portsMembersPtr->ports[{3, 0} / {12, 0}].
            Expected: GT_OK.
        */

        /* Call with sourceId [0] */
        sourceId       = 0;
        cpuSrcIdMember = GT_FALSE;

        portsMembers.ports[0] = 3;
        portsMembers.ports[1] = 0;

        st = cpssExMxPmBrgSrcIdGroupEntrySet(dev, sourceId, cpuSrcIdMember, &portsMembers);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, sourceId, cpuSrcIdMember);

        /*
            1.2. Call cpssExMxPmBrgSrcIdGroupEntryGet with the same sourceId and non-NULL cpuSrcIdMemberPtr and non-NULL portsMembersPtr.
            Expected: GT_OK and the same cpuSrcIdMember and  portsMembersPtr.
        */
        st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, &cpuSrcIdMemberGet, &portsMembersGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgSrcIdGroupEntryGet: %d, %d", dev, sourceId);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuSrcIdMember, cpuSrcIdMemberGet,
                       "get another cpuSrcIdMember than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(portsMembers.ports[0], portsMembersGet.ports[0],
                       "get another portsMembersPtr->ports[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(portsMembers.ports[1], portsMembersGet.ports[1],
                       "get another portsMembersPtr->ports[1] than was set: %d", dev);

        /* Call with sourceId [31] */
        sourceId       = 31;
        cpuSrcIdMember = GT_TRUE;

        portsMembers.ports[0] = 12;
        portsMembers.ports[1] = 0;

        st = cpssExMxPmBrgSrcIdGroupEntrySet(dev, sourceId, cpuSrcIdMember, &portsMembers);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, sourceId, cpuSrcIdMember);

        /*
            1.2. Call cpssExMxPmBrgSrcIdGroupEntryGet with the same sourceId and non-NULL cpuSrcIdMemberPtr and non-NULL portsMembersPtr.
            Expected: GT_OK and the same cpuSrcIdMember and  portsMembersPtr.
        */
        st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, &cpuSrcIdMemberGet, &portsMembersGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgSrcIdGroupEntryGet: %d, %d", dev, sourceId);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(cpuSrcIdMember, cpuSrcIdMemberGet,
                       "get another cpuSrcIdMember than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(portsMembers.ports[0], portsMembersGet.ports[0],
                       "get another portsMembersPtr->ports[0] than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(portsMembers.ports[1], portsMembersGet.ports[1],
                       "get another portsMembersPtr->ports[1] than was set: %d", dev);

        /*
            1.3. Call with out of range sourceId [32]
                           and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        sourceId = 32;

        st = cpssExMxPmBrgSrcIdGroupEntrySet(dev, sourceId, cpuSrcIdMember, &portsMembers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, sourceId);

        sourceId = 0;

        /*
            1.4. Call with portsMembersPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgSrcIdGroupEntrySet(dev, sourceId, cpuSrcIdMember, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsMembersPtr = NULL", dev);
    }

    sourceId       = 0;
    cpuSrcIdMember = GT_FALSE;

    portsMembers.ports[0] = 3;
    portsMembers.ports[1] = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdGroupEntrySet(dev, sourceId, cpuSrcIdMember, &portsMembers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdGroupEntrySet(dev, sourceId, cpuSrcIdMember, &portsMembers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdGroupEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              sourceId,
    OUT GT_BOOL             *cpuSrcIdMemberPtr,
    OUT CPSS_PORTS_BMP_STC  *portsMembersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdGroupEntryGet)
{
/*
    ITERATE_DEVICES(ExMxPm)
    1.1. Call with sourceId [0 / 31],
                   non-NULL cpuSrcIdMemberPtr
                   and non-NULL portsMembersPtr.
    Expected: GT_OK.
    1.2. Call with out of range sourceId [32]
                   and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call with cpuSrcIdMemberPtr [NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call with portsMembersPtr[NULL]
                   and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32              sourceId       = 0;
    GT_BOOL             cpuSrcIdMember = GT_FALSE;
    CPSS_PORTS_BMP_STC  portsMembers;

    cpssOsMemSet(&portsMembers,0,sizeof(portsMembers));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with sourceId [0 / 31],
                           non-NULL cpuSrcIdMemberPtr
                           and non-NULL portsMembersPtr.
            Expected: GT_OK.
        */

        /* Call with sourceId [0] */
        sourceId = 0;

        st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, &cpuSrcIdMember, &portsMembers);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, sourceId);

        /* Call with sourceId [31] */
        sourceId = 31;

        st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, &cpuSrcIdMember, &portsMembers);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, sourceId);

        /*
            1.2. Call with out of range sourceId [32]
                           and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        sourceId = 32;

        st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, &cpuSrcIdMember, &portsMembers);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, sourceId);

        sourceId = 0;

        /*
            1.3. Call with cpuSrcIdMemberPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, NULL, &portsMembers);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cpuSrcIdMemberPtr = NULL", dev);

        /*
            1.5. Call with portsMembersPtr [NULL]
                           and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, &cpuSrcIdMember, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsMembersPtr = NULL", dev);
    }

    sourceId = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, &cpuSrcIdMember, &portsMembers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, &cpuSrcIdMember, &portsMembers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdPortDefaultSrcIdSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN GT_U32  srcId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdPortDefaultSrcIdSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with srcId [0 / 31].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgSrcIdPortDefaultSrcIdGet with non-NULL srcIdPtr.
    Expected: GT_OK and the same srcId.
    1.1.3. Call with out of range srcId [32].
    Expected: NON GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_SRC_ID_VALID_VIRT_PORT_CNS;

    GT_U32      srcId    = 0;
    GT_U32      srcIdGet = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with srcId [0 / 31].
                Expected: GT_OK.
            */

            /* Call with srcId [0] */
            srcId = 0;

            st = cpssExMxPmBrgSrcIdPortDefaultSrcIdSet(dev, port, srcId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, srcId);

            /*
                1.1.2. Call cpssExMxPmBrgSrcIdPortDefaultSrcIdGet with non-NULL srcIdPtr.
                Expected: GT_OK and the same srcId.
            */
            st = cpssExMxPmBrgSrcIdPortDefaultSrcIdGet(dev, port, &srcIdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmBrgSrcIdPortDefaultSrcIdGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(srcId, srcIdGet,
                           "get another srcId than was set: %d, %d", dev, port);

            /* Call with srcId [31] */
            srcId = 31;

            st = cpssExMxPmBrgSrcIdPortDefaultSrcIdSet(dev, port, srcId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, srcId);

            /*
                1.1.2. Call cpssExMxPmBrgSrcIdPortDefaultSrcIdGet with non-NULL srcIdPtr.
                Expected: GT_OK and the same srcId.
            */
            st = cpssExMxPmBrgSrcIdPortDefaultSrcIdGet(dev, port, &srcIdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmBrgSrcIdPortDefaultSrcIdGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(srcId, srcIdGet,
                           "get another srcId than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range srcId [32].
                Expected: NON GT_OK.
            */
            srcId = 32;

            st = cpssExMxPmBrgSrcIdPortDefaultSrcIdSet(dev, port, srcId);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, srcId);
        }

        srcId = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgSrcIdPortDefaultSrcIdSet(dev, port, srcId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgSrcIdPortDefaultSrcIdSet(dev, port, srcId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgSrcIdPortDefaultSrcIdSet(dev, port, srcId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    srcId = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_SRC_ID_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdPortDefaultSrcIdSet(dev, port, srcId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdPortDefaultSrcIdSet(dev, port, srcId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdPortDefaultSrcIdGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_U32  *srcIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdPortDefaultSrcIdGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with non-NULL srcIdPtr.
    Expected: GT_OK.
    1.1.2. Call function with srcIdPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_SRC_ID_VALID_VIRT_PORT_CNS;

    GT_U32      srcId    = 0;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /*
                1.1.1. Call with non-NULL srcIdPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgSrcIdPortDefaultSrcIdGet(dev, port, &srcId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function with srcIdPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgSrcIdPortDefaultSrcIdGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, srcIdPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available logical ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgSrcIdPortDefaultSrcIdGet(dev, port, &srcId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgSrcIdPortDefaultSrcIdGet(dev, port, &srcId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgSrcIdPortDefaultSrcIdGet(dev, port, &srcId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_SRC_ID_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdPortDefaultSrcIdGet(dev, port, &srcId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdPortDefaultSrcIdGet(dev, port, &srcId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdUcEgressFilterEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdUcEgressFilterEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_FALSE and GT_TRUE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgSrcIdUcEgressFilterEnableGet with non-NULL enablePtr.
    Expected:  GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state    = GT_FALSE;
    GT_BOOL     stateGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with enable [GT_FALSE / GT_TRUE].
            Expected: GT_OK.
        */

        /* Call with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssExMxPmBrgSrcIdUcEgressFilterEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgSrcIdUcEgressFilterEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgSrcIdUcEgressFilterEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgSrcIdUcEgressFilterEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgSrcIdUcEgressFilterEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.2. Call cpssExMxPmBrgSrcIdUcEgressFilterEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgSrcIdUcEgressFilterEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgSrcIdUcEgressFilterEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                       "get another enablePtr than was set: %d", dev);
    }

    state = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdUcEgressFilterEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdUcEgressFilterEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdUcEgressFilterEnableGet
(
    IN GT_U8     devNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdUcEgressFilterEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected:  GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     state = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with non-null enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgSrcIdUcEgressFilterEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgSrcIdUcEgressFilterEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdUcEgressFilterEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdUcEgressFilterEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (ExMxPm)
    1.1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet
    with non-NULL enablePtr.
    Expected: GT_OK and the same value as written.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_SRC_ID_VALID_PHY_PORT_CNS;

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
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet: %d, %d", dev, port);

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
            st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_SRC_ID_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (ExMxPm)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_SRC_ID_VALID_PHY_PORT_CNS;

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
            st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_SRC_ID_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdPortAssignModeSet
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdPortAssignModeSet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (ExMxPm)
    1.1.1. Call with mode [CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E /
                           CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgSrcIdPortAssignModeGet with non-NULL modePtr.
    Expected: GT_OK and the same mode.
    1.1.3. Call with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_SRC_ID_VALID_PHY_PORT_CNS;

    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode    = CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;
    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  modeGet = CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;

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
                1.1.1. Call with mode [CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E /
                                       CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E].
                Expected: GT_OK.
            */

            /* Call with mode [CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E] */
            mode = CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;

            st = cpssExMxPmBrgSrcIdPortAssignModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);

            /*
                1.1.2. Call cpssExMxPmBrgSrcIdPortAssignModeGet with non-NULL modePtr.
                Expected: GT_OK and the same mode.
            */
            st = cpssExMxPmBrgSrcIdPortAssignModeGet(dev, port, &modeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgSrcIdPortAssignModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d, %d", dev, port);

            /* Call with mode [CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E] */
            mode = CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E;

            st = cpssExMxPmBrgSrcIdPortAssignModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, mode);

            /*
                1.1.2. Call cpssExMxPmBrgSrcIdPortAssignModeGet with non-NULL modePtr.
                Expected: GT_OK and the same mode.
            */
            st = cpssExMxPmBrgSrcIdPortAssignModeGet(dev, port, &modeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgSrcIdPortAssignModeGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(mode, modeGet,
                       "get another mode than was set: %d, %d", dev, port);

            /*
                1.1.3. Call with out of range mode [0x5AAAAAA5].
                Expected: GT_BAD_PARAM.
            */
            mode = BRG_SRC_ID_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgSrcIdPortAssignModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, mode);
        }

        mode = CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgSrcIdPortAssignModeSet(dev, port, mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgSrcIdPortAssignModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgSrcIdPortAssignModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    mode = CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_SRC_ID_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdPortAssignModeSet(dev, port, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdPortAssignModeSet(dev, port, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgSrcIdPortAssignModeGet
(
    IN GT_U8                            devNum,
    IN GT_U8                            portNum,
    IN CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  *modePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdPortAssignModeGet)
{
/*
    ITERATE_DEVICES_PHY_CPU_PORT (ExMxPm)
    1.1.1. Call with non-NULL modePtr.
    Expected: GT_OK.
    1.1.2. Call with modePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_SRC_ID_VALID_PHY_PORT_CNS;

    CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT  mode = CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E;

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
                1.1.1. Call with non-NULL modePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgSrcIdPortAssignModeGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with modePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgSrcIdPortAssignModeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, modePtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgSrcIdPortAssignModeGet(dev, port, &mode);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_PHY_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgSrcIdPortAssignModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgSrcIdPortAssignModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_SRC_ID_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgSrcIdPortAssignModeGet(dev, port, &mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgSrcIdPortAssignModeGet(dev, port, &mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill Source ID Egress Filtering tAble.
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdFillGroupTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in Source ID Egress Filtering table.
         Call cpssExMxPmBrgSrcIdGroupEntrySet with sourceId [0..numEntries - 1],
                   cpuSrcIdMember[GT_TRUE]
                   and portsMembersPtr->ports[{3, 0}].
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmBrgSrcIdGroupEntrySet with sourceId [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in Source ID Egress Filtering table and compare with original.
         Call cpssExMxPmBrgSrcIdGroupEntryGet with the same sourceId and 
                                                non-NULL cpuSrcIdMemberPtr 
                                                and non-NULL portsMembersPtr.
    Expected: GT_OK and the same cpuSrcIdMember and  portsMembersPtr.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmBrgSrcIdGroupEntryGet with sourceId [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries        = 0;
    GT_U32      iTemp             = 0;
    GT_BOOL     cpuSrcIdMember    = GT_FALSE;
    GT_BOOL     cpuSrcIdMemberGet = GT_FALSE;

    CPSS_PORTS_BMP_STC  portsMembers;
    CPSS_PORTS_BMP_STC  portsMembersGet;


    cpssOsBzero((GT_VOID*) &portsMembers, sizeof(portsMembers));
    cpssOsBzero((GT_VOID*) &portsMembersGet, sizeof(portsMembersGet));

    /* Fill the entry for Source ID Egress Filtering table */
    cpuSrcIdMember = GT_FALSE;

    portsMembers.ports[0] = 0;
    portsMembers.ports[1] = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 32;

        /* 1.2. Fill all entries in Source ID Egress Filtering table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* make every entry unique */
            portsMembers.ports[0] = iTemp % ((GT_U32) (1 << 31));
            portsMembers.ports[1] = iTemp % ((GT_U32) (1 << 22));

            st = cpssExMxPmBrgSrcIdGroupEntrySet(dev, iTemp, cpuSrcIdMember, &portsMembers);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupEntrySet: %d, %d, %d", dev, iTemp, cpuSrcIdMember);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmBrgSrcIdGroupEntrySet(dev, numEntries, cpuSrcIdMember, &portsMembers);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupEntrySet: %d, %d, %d", dev, iTemp, cpuSrcIdMember);

        /* 1.4. Read all entries in Source ID Egress Filtering table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            /* restore unique entry before compare */
            portsMembers.ports[0] = iTemp % ((GT_U32) (1 << 31));
            portsMembers.ports[1] = iTemp % ((GT_U32) (1 << 22));

            st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, iTemp, &cpuSrcIdMemberGet, &portsMembersGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                           "cpssExMxPmBrgSrcIdGroupEntryGet: %d, %d", dev, iTemp);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(cpuSrcIdMember, cpuSrcIdMemberGet,
                           "get another cpuSrcIdMember than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(portsMembers.ports[0], portsMembersGet.ports[0],
                           "get another portsMembersPtr->ports[0] than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(portsMembers.ports[1], portsMembersGet.ports[1],
                           "get another portsMembersPtr->ports[1] than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, numEntries, &cpuSrcIdMemberGet, &portsMembersGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupEntryGet: %d, %d", dev, iTemp);
    }
}

/*----------------------------------------------------------------------------*/
/*
    Test for Functionality
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgSrcIdFunctionality)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Call cpssExMxPmBrgSrcIdGroupEntrySet with sourceId [0 .. 31],
                   cpuSrcIdMember[GT_TRUE / GT_FALSE]
                   and portsMembersPtr->ports[{0, 0}].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgSrcIdGroupEntrySet with sourceId [32] (out of range) and other params from 1.1.
    Expected: NOT GT_OK.
    1.3. Call cpssExMxPmBrgSrcIdGroupPortAdd with sourceId [0 .. 31]
                                             and portNum [0 .. 63].
    Expected: GT_OK.
    1.4 Call cpssExMxPmBrgSrcIdGroupPortAdd with portNum [64] (out of range) and other params from 1.3.
    Expected: NOT GT_OK.
    1.5 Call cpssExMxPmBrgSrcIdGroupPortAdd with sourceId [32] (out of range) and other params from 1.3.
    Expected: NOT GT_OK.
    1.6. Call cpssExMxPmBrgSrcIdGroupEntryGet with sourceId [0 .. 31], non-NULL portBitmapPtr and non-NULLisCpuMemberPtr.
    Expected: GT_OK and portBitmapPtr->ports [{0xFFFFFFFF, 0xFFFFFFFF}] and cpuSrcIdMember [TRUE].
    1.7. Call cpssExMxPmBrgSrcIdGroupEntryGet with sourceId [32] (out of range) and other params from 1.6.
    Expected: NOT GT_OK.
    1.8. Call cpssExMxPmBrgSrcIdGroupPortDelete with sourceId [0 .. 31]
                                          and portNum [0 .. 63].
    Expected: GT_OK.
    1.9 Call cpssExMxPmBrgSrcIdGroupPortDelete with portNum [64] (out of range) and other params from 1.8.
    Expected: NOT GT_OK.
    1.10 Call cpssExMxPmBrgSrcIdGroupPortDelete with sourceId [32] (out of range) and other params from 1.8.
    Expected: NOT GT_OK.
    1.11. Call cpssExMxPmBrgSrcIdGroupEntryGet with sourceId [0 .. 31], non-NULL portBitmapPtr and non-NULLisCpuMemberPtr.
    Expected: GT_OK and portBitmapPtr->ports [{0, 0}] and cpuSrcIdMember [TRUE].
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32               iTemp = 0;
    GT_U32               jTemp = 0;
    GT_BOOL              cpuSrcIdMember = GT_FALSE;
    CPSS_PORTS_BMP_STC   portBitmap;
    CPSS_PORTS_BMP_STC   portBitmapGet;
    GT_U8                portNum  = 0;
    GT_U32               sourceId = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmBrgSrcIdGroupEntrySet with sourceId [0 .. 31],
                           cpuSrcIdMember[GT_TRUE / GT_FALSE]
                           and portsMembersPtr->ports[{0, 0}].
            Expected: GT_OK.
        */
        cpuSrcIdMember = GT_TRUE;

        portBitmap.ports[0] = 0;
        portBitmap.ports[1] = 0;

        for(iTemp=0; iTemp<32; ++iTemp)
        {
            sourceId = iTemp;

            st = cpssExMxPmBrgSrcIdGroupEntrySet(dev, sourceId, cpuSrcIdMember, &portBitmap);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupEntrySet: %d, %d, %d", dev, sourceId, cpuSrcIdMember);
        }

        /*
            1.2. Call cpssExMxPmBrgSrcIdGroupEntrySet with sourceId [32] (out of range) and other params from 1.1.
            Expected: NOT GT_OK.
        */
        sourceId = 32;

        st = cpssExMxPmBrgSrcIdGroupEntrySet(dev, sourceId, cpuSrcIdMember, &portBitmap);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupEntrySet: %d, %d", dev, sourceId);        

        /*
            1.3. Call cpssExMxPmBrgSrcIdGroupPortAdd with sourceId [0 .. 31]
                                                     and portNum [0 .. 63].
            Expected: GT_OK.
        */
        for(iTemp=0; iTemp<32; ++iTemp)
        {
                sourceId = iTemp;
                for(jTemp=0; jTemp<UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev); ++jTemp)
                {
                    portNum = (GT_U8)jTemp;

                    st = cpssExMxPmBrgSrcIdGroupPortAdd(dev, sourceId, portNum);
                    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupPortAdd: %d, %d, %d", dev, sourceId, portNum);
                }

                /*
                    1.4 Call cpssExMxPmBrgSrcIdGroupPortAdd with portNum [64] (out of range) and other params from 1.3.
                    Expected: NOT GT_OK.
                */
                portNum = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

                st = cpssExMxPmBrgSrcIdGroupPortAdd(dev, sourceId, portNum);
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupPortAdd: %d, %d, %d", dev, sourceId, portNum);
        }
        
        /*
            1.5 Call cpssExMxPmBrgSrcIdGroupPortAdd with sourceId [32] (out of range) and other params from 1.3.
            Expected: NOT GT_OK.
        */
        sourceId = 32;

        st = cpssExMxPmBrgSrcIdGroupPortAdd(dev, sourceId, portNum);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupPortAdd: %d, %d", dev, sourceId);

        /*
            1.6. Call cpssExMxPmBrgSrcIdGroupEntryGet with sourceId [0 .. 31], non-NULL portBitmapPtr and non-NULLisCpuMemberPtr.
            Expected: GT_OK and portBitmapPtr->ports [{0xFFFFFFFF, 0xFFFFFFFF}] and cpuSrcIdMember [TRUE].
        */
        for(iTemp=0; iTemp<32; ++iTemp)
        {
            sourceId = iTemp;

            st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, &cpuSrcIdMember, &portBitmapGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupEntryGet: %d, %d", dev, sourceId);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, cpuSrcIdMember,
                           "get another cpuSrcIdMember than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(0xFFFFFFFF, portBitmapGet.ports[0],
                           "get another portBitmapPtr->ports[0] than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(0x3FFFFF, portBitmapGet.ports[1],
                           "get another portBitmapPtr->ports[1] than was set: %d", dev);
        }

        /*
            1.7. Call cpssExMxPmBrgSrcIdGroupEntryGet with sourceId [32] (out of range) and other params from 1.6.
            Expected: NOT GT_OK.
        */
        sourceId = 32;

        st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, &cpuSrcIdMember, &portBitmapGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupEntryGet: %d, %d", dev, sourceId);

        /*
            1.8. Call cpssExMxPmBrgSrcIdGroupPortDelete with sourceId [0 .. 31]
                                                  and portNum [0 .. 63].
            Expected: GT_OK.
        */
        for(iTemp=0; iTemp<32; ++iTemp)
        {
                sourceId = iTemp;
                for(jTemp=0; jTemp<UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev); ++jTemp)
                {
                    portNum = (GT_U8)jTemp;

                    st = cpssExMxPmBrgSrcIdGroupPortDelete(dev, sourceId, portNum);
                    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupPortDelete: %d, %d, %d", dev, sourceId, portNum);
                }

                /*
                    1.9 Call cpssExMxPmBrgSrcIdGroupPortDelete with portNum [64] (out of range) and other params from 1.8.
                    Expected: NOT GT_OK.
                */
                portNum = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

                st = cpssExMxPmBrgSrcIdGroupPortDelete(dev, sourceId, portNum);
                UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupPortDelete: %d, %d, %d", dev, sourceId, portNum);
        }
       
        /*
            1.10 Call cpssExMxPmBrgSrcIdGroupPortDelete with sourceId [32] (out of range) and other params from 1.8.
            Expected: NOT GT_OK.
        */
        sourceId = 32;

        st = cpssExMxPmBrgSrcIdGroupPortDelete(dev, sourceId, portNum);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupPortDelete: %d, %d, %d", dev, sourceId, portNum);

        /*
            1.11. Call cpssExMxPmBrgSrcIdGroupEntryGet with sourceId [0 .. 31], non-NULL portBitmapPtr and non-NULLisCpuMemberPtr.
            Expected: GT_OK and portBitmapPtr->ports [{0, 0}] and isCpuMember [TRUE].
        */
        for(iTemp=0; iTemp<32; ++iTemp)
        {
            sourceId = iTemp;

            st = cpssExMxPmBrgSrcIdGroupEntryGet(dev, sourceId, &cpuSrcIdMember, &portBitmapGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgSrcIdGroupEntryGet: %d, %d", dev, sourceId);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, cpuSrcIdMember,
                           "get another cpuSrcIdMember than was set: %d", dev);
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
 * Configuration of cpssExMxPmBrgSrcId suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmBrgSrcId)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdGroupPortAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdGroupPortDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdGroupEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdGroupEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdPortDefaultSrcIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdPortDefaultSrcIdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdUcEgressFilterEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdUcEgressFilterEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdPortUcEgressFilterEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdPortUcEgressFilterEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdPortAssignModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdPortAssignModeGet)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdFillGroupTable)
    /* Test for Functionality */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgSrcIdFunctionality)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmBrgSrcId)

