/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxBrgVlanUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxBrgVlan.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgVlan.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */
/*  This constant is copied from cpssExMxBrgStp.c               */
/* Be careful to keep them synchronized!                        */
#define MAX_NUMBER_OF_STG_CNS                           256


/* Tests use this vlan id for testing VLAN functions            */
#define BRG_VLAN_TESTED_VLAN_ID_CNS                     100

/* Tests use this value as default valid value for port         */
#define BRG_VLAN_VALID_VIRT_PORT_CNS                    0

/* Tests use this value for testing functions which have        */
/* entry number parameter.                                      */
#define BRG_VLAN_TESTED_ENTRY_NUMBER_CNS                0

/* Tests use this value for testing out of range entry number.  */
#define BRG_VLAN_MAX_ENTRY_NUMBER_CNS                   0xFF

/* Tests use this value to check invalid enumeration            */
#define BRG_VLAN_INVALID_ENUM_CNS                       0x5AAAAAA5

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanMaxVidSet
(
    IN  GT_U8   devNum,
    IN GT_U16   maxVid
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanMaxVidSet)
{
/*
ITERATE_DEVICES
1.1. For each active device set maxVid [100]. Expected: GT_OK.
1.2. For each active device set out of range maxVid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      maxVid;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For each active device set maxVid [100].    */
        /* Expected: GT_OK.                                 */
        maxVid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanMaxVidSet(dev, maxVid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, maxVid);

        /* 1.2. For each active device set out of range maxVid [PRV_CPSS_MAX_NUM_VLANS_CNS].    */
        /* Expected: GT_BAD_PARAM.                                                              */
        maxVid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgVlanMaxVidSet(dev, maxVid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, maxVid);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    maxVid = BRG_VLAN_TESTED_VLAN_ID_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanMaxVidSet(dev, maxVid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanMaxVidSet(dev, maxVid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanEntryWrite
(
    IN GT_U8              devNum,
    IN GT_U16             vid,
    IN CPSS_PORTS_BMP_STC *portsMembersPtr,
    IN CPSS_PORTS_BMP_STC *portsTaggingPtr,
    IN CPSS_VLAN_INFO_STC *vlanInfoPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanEntryWrite)
{
/*
ITERATE_DEVICES
1.1. Call function for vlan Id [100], portsMemberPtr [0, 16], portsTaggingPtr [0, 16], vlanInfoPtr [zeroed, stpId = 12]. Expected: GT_OK.
1.2. Call corresponding "get" function cpssExMxBrgVlanEntryRead to make sure that writing has been done correct: returned vlanInfo should be the same as written.
1.3. Call with out of range Vlan Id [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
1.4. Call with Vlan id [100] and portsMembersPtr [NULL]. Expected: GT_BAD_PTR.
1.5. Call with Vlan id [100] and portsTaggingPtr [NULL]. Expected: GT_BAD_PTR.
1.6. Call with Vlan id [100] and vlanInfoPtr [NULL]. Expected: GT_BAD_PTR.
1.7. Call with invalid value for field unregMcFilterCmd [0xFF] of vlanInfoPtr. Expected: NON GT_OK.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_U16              vid;
    CPSS_PORTS_BMP_STC  portsMembers;
    CPSS_PORTS_BMP_STC  portsTagging;
    CPSS_VLAN_INFO_STC  vlanInfo;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        CPSS_PORTS_BMP_STC  portsMembersWritten;
        CPSS_PORTS_BMP_STC  portsTaggingWritten;
        CPSS_VLAN_INFO_STC  vlanInfoWritten;
        GT_BOOL             isValid;
        GT_32               cmpRes;

        /* 1.1. Call function for vlan Id [100], portsMemberPtr [0, 16], portsTaggingPtr [0, 16],   */
        /* vlanInfoPtr [zeroed, stpId = 12].                                                        */
        /* Expected: GT_OK.                                                                         */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        portsMembers.ports[0] = 16;
        portsMembers.ports[1] = 0;

        portsTagging.ports[0] = 16;
        portsTagging.ports[1] = 0;

        cpssOsBzero((GT_VOID*)&vlanInfo, sizeof(vlanInfo));
        vlanInfo.stpId = 12;

        st = cpssExMxBrgVlanEntryWrite(dev, vid, &portsMembers, &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* 1.2. Call corresponding "get" function cpssExMxBrgVlanEntryRead                                      */
        /* to check that writing has been done correct: returned vlanInfo should be the same as written.        */
        st = cpssExMxBrgVlanEntryRead(dev, vid, &portsMembersWritten, &portsTaggingWritten, &vlanInfoWritten, &isValid);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxBrgVlanEntryRead: %d, %d", dev, vid);

        /* check that ports bitmap is the same as written */
        cmpRes = cpssOsMemCmp((const GT_VOID*)&portsMembersWritten, (const GT_VOID*)&portsMembers, sizeof(portsMembers));
        UTF_VERIFY_EQUAL0_STRING_MAC(0, cmpRes, "ports bitmap isn't the same as written");

        /* check that ports tagging bitmap is the same as written */
        cmpRes = cpssOsMemCmp((const GT_VOID*)&portsTaggingWritten, (const GT_VOID*)&portsTagging, sizeof(portsTagging));
        UTF_VERIFY_EQUAL0_STRING_MAC(0, cmpRes, "ports tagging bitmap isn't the same as written");

        /* check that vlan info is the same as written */
        cmpRes = cpssOsMemCmp((const GT_VOID*)&vlanInfoWritten, (const GT_VOID*)&vlanInfo, sizeof(vlanInfo));
        UTF_VERIFY_EQUAL0_STRING_MAC(0, cmpRes, "vlan info isn't the same as written");

        /* check that written vlan is valid */
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValid, "vlan [%d] isn't valid as expected", vid);

        /* 1.3. Call with out of range Vlan Id [PRV_CPSS_MAX_NUM_VLANS_CNS].    */
        /* Expected: GT_BAD_PARAM.                                              */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgVlanEntryWrite(dev, vid, &portsMembers, &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vid);

        /* 1.4. Call with Vlan id [100] and portsMembersPtr [NULL].             */
        /* Expected: GT_BAD_PTR.                                                */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanEntryWrite(dev, vid, NULL, &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, NULL", dev, vid);

        /* 1.5. Call with Vlan id [100] and portsTaggingPtr [NULL].             */
        /* Expected: GT_BAD_PTR.                                                */
        st = cpssExMxBrgVlanEntryWrite(dev, vid, &portsMembers, NULL, &vlanInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, <members>, NULL", dev, vid);

        /* 1.6. Call with Vlan id [100] and vlanInfoPtr [NULL].                 */
        /* Expected: GT_BAD_PTR.                                                */
        st = cpssExMxBrgVlanEntryWrite(dev, vid, &portsMembers, &portsTagging, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, <members>, <tagging>, NULL", dev, vid);

        /* 1.7. Call with invalid value for field unregMcFilterCmd [0xFF] of vlanInfoPtr.   */
        /* Expected: NON GT_OK.                                                             */
        vlanInfo.unregMcFilterCmd = 0xFF;

        st = cpssExMxBrgVlanEntryWrite(dev, vid, &portsMembers, &portsTagging, &vlanInfo);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                                         "%d, %d, <members>, <tagging>, unregMcFilterCmd [%d]",
                                         dev, vid, vlanInfo.unregMcFilterCmd);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

    portsMembers.ports[0] = 16;
    portsMembers.ports[1] = 0;

    portsTagging.ports[0] = 16;
    portsTagging.ports[1] = 0;

    cpssOsBzero((GT_VOID*)&vlanInfo, sizeof(vlanInfo));
    vlanInfo.stpId = 12;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>.     */
        /* Expected: GT_BAD_PARAM.                                              */
        st = cpssExMxBrgVlanEntryWrite(dev, vid, &portsMembers, &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.        */
    /* Expected: GT_BAD_PARAM.                              */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanEntryWrite(dev, vid, &portsMembers, &portsTagging, &vlanInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanEntryRead
(
    IN  GT_U8              devNum,
    IN  GT_U16             vid,
    OUT CPSS_PORTS_BMP_STC *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC *portsTaggingPtr,
    OUT CPSS_VLAN_INFO_STC *vlanInfoPtr,
    OUT GT_BOOL            *isValidPtr
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanEntryRead)
{
/*
ITERATE_DEVICES
1.1. Call function for vlan Id [100], not null portsMemberPtr, not null portsTaggingPtr, not null vlanInfoPtr, not null isValidPtr. Expected: GT_OK.
1.2. Call with out of range Vlan Id [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
1.3. Call with Vlan id [100] and portsMembersPtr [NULL]. Expected: GT_BAD_PTR.
1.4. Call with Vlan id [100] and portsTaggingPtr [NULL]. Expected: GT_BAD_PTR.
1.5. Call with Vlan id [100] and vlanInfoPtr [NULL]. Expected: GT_BAD_PTR.
1.6. Call with Vlan id [100] and isValidPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_U16              vid;
    CPSS_PORTS_BMP_STC  portsMembers;
    CPSS_PORTS_BMP_STC  portsTagging;
    CPSS_VLAN_INFO_STC  vlanInfo;
    GT_BOOL             isValid;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for vlan Id [100], not null portsMemberPtr,           */
        /* not null portsTaggingPtr, not null vlanInfoPtr, not null isValidPtr.     */
        /* Expected: GT_OK.                                                         */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanEntryRead(dev, vid, &portsMembers, &portsTagging, &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* 1.2. Call with out of range Vlan Id [PRV_CPSS_MAX_NUM_VLANS_CNS].        */
        /* Expected: GT_BAD_PARAM.                                                  */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgVlanEntryRead(dev, vid, &portsMembers, &portsTagging, &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vid);

        /* 1.3. Call with Vlan id [100] and portsMembersPtr [NULL].                 */
        /* Expected: GT_BAD_PTR.                                                    */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanEntryRead(dev, vid, NULL, &portsTagging, &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, vid);

        /* 1.4. Call with Vlan id [100] and portsTaggingPtr [NULL].                 */
        /* Expected: GT_BAD_PTR.                                                    */
        st = cpssExMxBrgVlanEntryRead(dev, vid, &portsMembers, NULL, &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, vid);

        /* 1.5. Call with Vlan id [100] and vlanInfoPtr [NULL].                     */
        /* Expected: GT_BAD_PTR.                                                    */
        st = cpssExMxBrgVlanEntryRead(dev, vid, &portsMembers, &portsTagging, NULL, &isValid);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, vid);

        /* 1.6. Call with Vlan id [100] and isValidPtr [NULL].                      */
        /* Expected: GT_BAD_PTR.                                                    */
        st = cpssExMxBrgVlanEntryRead(dev, vid, &portsMembers, &portsTagging, &vlanInfo, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, vid);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>.     */
        /* Expected: GT_BAD_PARAM.                                              */
        st = cpssExMxBrgVlanEntryRead(dev, vid, &portsMembers, &portsTagging, &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.        */
    /* Expected: GT_BAD_PARAM.                              */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanEntryRead(dev, vid, &portsMembers, &portsTagging, &vlanInfo, &isValid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanMemberAdd
(
    IN GT_U8   devNum,
    IN GT_U16  vid,
    IN GT_U8   portNum,
    IN GT_BOOL isTagged
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanMemberAdd)
{
/*
ITERATE_DEVICES_VIRT_CPU_PORT
1.1.1. Call with vid [100], isTagged [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.1.2. Call with out range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vid;
    GT_U8       port;
    GT_BOOL     isTagged;

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
            /* 1.1.1. Call with vid [100], isTagged [GT_TRUE and GT_FALSE].     */
            /* Expected: GT_OK.                                                 */
            vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
            isTagged = GT_TRUE;

            st = cpssExMxBrgVlanMemberAdd(dev, vid, port, isTagged);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vid, port, isTagged);

            isTagged = GT_FALSE;

            st = cpssExMxBrgVlanMemberAdd(dev, vid, port, isTagged);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vid, port, isTagged);

            /* 1.1.2. Call with out range vid [PRV_CPSS_MAX_NUM_VLANS_CNS].     */
            /* Expected: GT_BAD_PARAM.                                          */
            vid = PRV_CPSS_MAX_NUM_VLANS_CNS;
            isTagged = GT_TRUE;

            st = cpssExMxBrgVlanMemberAdd(dev, vid, port, isTagged);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, port, isTagged);
        }

        /* 1.2. Check that function handles CPU virtual port. */
        port = CPSS_CPU_PORT_NUM_CNS;
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
        isTagged = GT_TRUE;

        /* 1.2.1. <Check other parameters for each active device id and CPU virtual port>. */
        st = cpssExMxBrgVlanMemberAdd(dev, vid, port, isTagged);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vid, port, isTagged);

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
        isTagged = GT_TRUE;

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxBrgVlanMemberAdd(dev, vid, port, isTagged);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, port, isTagged);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
        isTagged = GT_TRUE;

        st = cpssExMxBrgVlanMemberAdd(dev, vid, port, isTagged);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, port, isTagged);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;
    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
    isTagged = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanMemberAdd(dev, vid, port, isTagged);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanMemberAdd(dev, vid, port, isTagged);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanPortDelete
(
    IN GT_U8          devNum,
    IN GT_U16         vid,
    IN GT_U8          port
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanPortDelete)
{
/*
ITERATE_DEVICES_VIRT_CPU_PORT
1.1.1. Call with vid [100]. Expected: GT_OK.
1.1.2. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vid;
    GT_U8       port;

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
            /* 1.1.1. Call with vid [100].      */
            /* Expected: GT_OK.                 */
            vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

            st = cpssExMxBrgVlanPortDelete(dev, vid, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, port);

            /* 1.1.2. Call with out range vid [PRV_CPSS_MAX_NUM_VLANS_CNS].     */
            /* Expected: GT_BAD_PARAM.                                          */
            vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssExMxBrgVlanPortDelete(dev, vid, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, port);
        }

        /* 1.2. Check that function handles CPU virtual port. */
        port = CPSS_CPU_PORT_NUM_CNS;
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /* 1.2.1. <Check other parameters for each active device id and CPU virtual port>. */
        st = cpssExMxBrgVlanPortDelete(dev, vid, port);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, port);

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxBrgVlanPortDelete(dev, vid, port);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, port);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanPortDelete(dev, vid, port);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;
    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanPortDelete(dev, vid, port);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanPortDelete(dev, vid, port);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanHasUplinksSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vid,
    IN  GT_BOOL hasUplinks
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanHasUplinksSet)
{
/*
ITERATE_DEVICES
1.1. Call with vid [100], hasUplinks [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.2. For each active device call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vid;
    GT_BOOL     hasUplinks;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with vid [100], hasUplinks [GT_TRUE and GT_FALSE]. */
        /* Expected: GT_OK.                                             */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
        hasUplinks = GT_TRUE;

        st = cpssExMxBrgVlanHasUplinksSet(dev, vid, hasUplinks);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, hasUplinks);

        hasUplinks = GT_FALSE;

        st = cpssExMxBrgVlanHasUplinksSet(dev, vid, hasUplinks);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, hasUplinks);

        /* 1.2. For each active device call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS].     */
        /* Expected: GT_BAD_PARAM.                                                                  */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;
        hasUplinks = GT_TRUE;

        st = cpssExMxBrgVlanHasUplinksSet(dev, vid, hasUplinks);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, hasUplinks);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
    hasUplinks = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanHasUplinksSet(dev, vid, hasUplinks);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanHasUplinksSet(dev, vid, hasUplinks);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanHasLocalsSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  vid,
    IN  GT_BOOL hasLocals
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanHasLocalsSet)
{
/*
ITERATE_DEVICES
1.1. For each active device call with vid [100] and hasLocals [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.2. For each active device call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vid;
    GT_BOOL     hasLocals;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with vid [100], hasUplinks [GT_TRUE and GT_FALSE]. */
        /* Expected: GT_OK.                                             */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
        hasLocals = GT_TRUE;

        st = cpssExMxBrgVlanHasLocalsSet(dev, vid, hasLocals);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, hasLocals);

        hasLocals = GT_FALSE;

        st = cpssExMxBrgVlanHasLocalsSet(dev, vid, hasLocals);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, hasLocals);

        /* 1.2. For each active device call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS].     */
        /* Expected: GT_BAD_PARAM.                                                                  */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;
        hasLocals = GT_TRUE;

        st = cpssExMxBrgVlanHasLocalsSet(dev, vid, hasLocals);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, hasLocals);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
    hasLocals = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanHasLocalsSet(dev, vid, hasLocals);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanHasLocalsSet(dev, vid, hasLocals);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanPortPvidGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_U16  *vid
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanPortPvidGet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call with non-null vid. Expected: GT_OK.
1.1.2. Call with vid [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vid;
    GT_U8       port;

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
            /* 1.1.1. Call with non-null vid.   */
            /* Expected: GT_OK.                 */
            vid = 0;

            st = cpssExMxBrgVlanPortPvidGet(dev, port, &vid);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);

            /* 1.1.2. Call with vid [NULL].     */
            /* Expected: GT_BAD_PTR.            */
            st = cpssExMxBrgVlanPortPvidGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            vid = 0;

            st = cpssExMxBrgVlanPortPvidGet(dev, port, &vid);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, vid);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        vid = 0;

        st = cpssExMxBrgVlanPortPvidGet(dev, port, &vid);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, vid);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        vid = 0;

        st = cpssExMxBrgVlanPortPvidGet(dev, port, &vid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    vid = 0;

    st = cpssExMxBrgVlanPortPvidGet(dev, port, &vid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanPortPvidSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    IN  GT_U16  vid
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanPortPvidSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call with vid [100]. Expected: GT_OK.
1.1.2. Call cpssExMxBrgVlanPortPvidGet Expected: GT_OK and the same vid.
1.1.3. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vid;
    GT_U8       port;

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
            GT_U16 vidWritten;

            /* 1.1.1. Call with vid [100].          */
            /* Expected: GT_OK.                     */
            vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

            st = cpssExMxBrgVlanPortPvidSet(dev, port, vid);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vid);

            /* 1.1.2. Call cpssExMxBrgVlanPortPvidGet       */
            /* Expected: GT_OK and the same vid.            */
            st = cpssExMxBrgVlanPortPvidGet(dev, port, &vidWritten);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxBrgVlanPortPvidGet: %d, %d, %d", dev, port, vid);
            UTF_VERIFY_EQUAL1_STRING_MAC(vid, vidWritten, "failed write vid [%d]", vid);

            /* 1.1.3. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS].  */
            /* Expected: GT_BAD_PARAM.                                          */
            vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssExMxBrgVlanPortPvidSet(dev, port, vid);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, vid);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

            st = cpssExMxBrgVlanPortPvidSet(dev, port, vid);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, vid);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanPortPvidSet(dev, port, vid);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, vid);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;
    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanPortPvidSet(dev, port, vid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanPortPvidSet(dev, port, vid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanPortIngFltEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL value
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanPortIngFltEnable)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call with value [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     value;

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
            /* 1.1.1. Call with value [GT_TRUE and GT_FALSE].   */
            /* Expected: GT_OK.                                 */
            value = GT_FALSE;

            st = cpssExMxBrgVlanPortIngFltEnable(dev, port, value);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, value);

            value = GT_TRUE;
            st = cpssExMxBrgVlanPortIngFltEnable(dev, port, value);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, value);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            value = GT_TRUE;

            st = cpssExMxBrgVlanPortIngFltEnable(dev, port, value);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, value);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        value = GT_TRUE;

        st = cpssExMxBrgVlanPortIngFltEnable(dev, port, value);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, value);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;
    value = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssExMxBrgVlanPortIngFltEnable(dev, port, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanPortIngFltEnable(dev, port, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanIpClassEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL value
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanIpClassEnable)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call with value [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     value;

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
            /* 1.1.1. Call with value [GT_TRUE and GT_FALSE].   */
            /* Expected: GT_OK.                                 */
            value = GT_FALSE;

            st = cpssExMxBrgVlanIpClassEnable(dev, port, value);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, value);

            value = GT_TRUE;
            st = cpssExMxBrgVlanIpClassEnable(dev, port, value);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, value);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            value = GT_TRUE;

            st = cpssExMxBrgVlanIpClassEnable(dev, port, value);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, value);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        value = GT_TRUE;

        st = cpssExMxBrgVlanIpClassEnable(dev, port, value);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, value);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;
    value = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssExMxBrgVlanIpClassEnable(dev, port, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanIpClassEnable(dev, port, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanSubnetClassEntrySet
(
    IN GT_U8        devNum,
    IN GT_U8        entryNum,
    IN GT_IPADDR    sip,
    IN GT_U8        ipPrefix,
    IN GT_BOOL      addDel
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanSubnetClassEntrySet)
{
/*
ITERATE_DEVICES
1.1. For each active device call with entryNum [0], sip [192.168.0.1], and ipPrefix [24], addDel [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.2. Call with out of range value for entryNum [0xFF]. Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           entryNum;
    IN GT_IPADDR    sip;
    IN GT_U8        ipPrefix;
    IN GT_BOOL      addDel;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* NETWORK ORDER */
    sip.arIP[0] = 1;
    sip.arIP[1] = 0;
    sip.arIP[2] = 168;
    sip.arIP[3] = 192;

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For each active device call with entryNum [0],                  */
        /* sip [192.168.0.1], and ipPrefix [24], addDel [GT_TRUE and GT_FALSE]. */
        /* Expected: GT_OK.                                                     */
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;

        ipPrefix = 24;
        addDel = GT_TRUE;

        st = cpssExMxBrgVlanSubnetClassEntrySet(dev, entryNum, sip, ipPrefix, addDel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);

        addDel = GT_FALSE;

        st = cpssExMxBrgVlanSubnetClassEntrySet(dev, entryNum, sip, ipPrefix, addDel);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);

        /* 1.2. Call with out of range value for entryNum [0xFF].   */
        /* Expected: NON GT_OK.                                     */
        entryNum = BRG_VLAN_MAX_ENTRY_NUMBER_CNS;

        st = cpssExMxBrgVlanSubnetClassEntrySet(dev, entryNum, sip, ipPrefix, addDel);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
    ipPrefix = 24;
    addDel = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanSubnetClassEntrySet(dev, entryNum, sip, ipPrefix, addDel);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanSubnetClassEntrySet(dev, entryNum, sip, ipPrefix, addDel);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanSubnetClassVidSet
(
    IN GT_U8     devNum,
    IN GT_U8     entryNum,
    IN GT_U16    vlanId
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanSubnetClassVidSet)
{
/*
ITERATE_DEVICES
1.1. For each active device call with entryNum [0], vlanId [100]. Expected: GT_OK.
1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
1.3. Call with out of range entryNum [0xFF]. Expected: NON GT_OK
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           entryNum;
    GT_U16          vlanId;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For each active device call with entryNum [0], vlanId [100].    */
        /* Expected: GT_OK.                                                     */
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanSubnetClassVidSet(dev, entryNum, vlanId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, vlanId);

        /* 1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].     */
        /* Expected: GT_BAD_PARAM.                                              */
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgVlanSubnetClassVidSet(dev, entryNum, vlanId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, entryNum, vlanId);

        /* 1.3. Call with out of range entryNum [0xFF].                         */
        /* Expected: NON GT_OK                                                  */
        entryNum = BRG_VLAN_MAX_ENTRY_NUMBER_CNS;
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanSubnetClassVidSet(dev, entryNum, vlanId);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, vlanId);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanSubnetClassVidSet(dev, entryNum, vlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanSubnetClassVidSet(dev, entryNum, vlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanSubnetClassTcSet
(
    IN GT_U8     devNum,
    IN GT_U8     entryNum,
    IN GT_U8     trafClass
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanSubnetClassTcSet)
{
/*
ITERATE_DEVICES
1.1. For each active device call with entryNum [0], trafClass [0]. Expected: GT_OK.
1.2. Call with out of range trafClass [CPSS_TC_RANGE_CNS]. Expected: NON GT_OK.
1.3. Call with out of range entryNum [0xFF]. Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           entryNum;
    GT_U8           trafClass;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For each active device call with entryNum [0], trafClass [0].   */
        /* Expected: GT_OK.                                                     */
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        trafClass = 0;

        st = cpssExMxBrgVlanSubnetClassTcSet(dev, entryNum, trafClass);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, trafClass);

        /* 1.2. Call with out of range trafClass [CPSS_TC_RANGE_CNS]      */
        /* Expected: NON GT_OK.                                           */
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        trafClass = CPSS_TC_RANGE_CNS;

        st = cpssExMxBrgVlanSubnetClassTcSet(dev, entryNum, trafClass);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, trafClass);

        /* 1.3. Call with out of range entryNum [0xFF].                         */
        /* Expected: NON GT_OK.                                                 */
        entryNum = BRG_VLAN_MAX_ENTRY_NUMBER_CNS;
        trafClass = 0;

        st = cpssExMxBrgVlanSubnetClassTcSet(dev, entryNum, trafClass);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, trafClass);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
    trafClass = 0;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanSubnetClassTcSet(dev, entryNum, trafClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanSubnetClassTcSet(dev, entryNum, trafClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanSubnetClassEntryEnable
(
    IN GT_U8   devNum,
    IN GT_U8   entryNum,
    IN GT_BOOL value
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanSubnetClassEntryEnable)
{
/*
ITERATE_DEVICES
1.1. For each active device call with entryNum [0], value [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.2. Call with out of range entryNum [0xFF]. Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           entryNum;
    GT_BOOL         value;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For each active device call with entryNum [0], value [GT_TRUE and GT_FALSE].    */
        /* Expected: GT_OK.                                                                     */
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        value = GT_FALSE;

        st = cpssExMxBrgVlanSubnetClassEntryEnable(dev, entryNum, value);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, value);

        value = GT_TRUE;

        st = cpssExMxBrgVlanSubnetClassEntryEnable(dev, entryNum, value);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, value);

        /* 1.2. Call with out of range entryNum [0xFF].                                         */
        /* Expected: NON GT_OK.                                                                 */
        entryNum = BRG_VLAN_MAX_ENTRY_NUMBER_CNS;
        value = GT_TRUE;

        st = cpssExMxBrgVlanSubnetClassEntryEnable(dev, entryNum, value);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, value);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
    value = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanSubnetClassEntryEnable(dev, entryNum, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanSubnetClassEntryEnable(dev, entryNum, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanProtoClassEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL value
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanProtoClassEnable)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. For each active device call with value [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     value;

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
            /* 1.1.1. Call with value [GT_TRUE and GT_FALSE].   */
            /* Expected: GT_OK.                                 */
            value = GT_FALSE;

            st = cpssExMxBrgVlanProtoClassEnable(dev, port, value);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, value);

            value = GT_TRUE;

            st = cpssExMxBrgVlanProtoClassEnable(dev, port, value);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, value);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            value = GT_TRUE;

            st = cpssExMxBrgVlanProtoClassEnable(dev, port, value);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, value);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        value = GT_TRUE;

        st = cpssExMxBrgVlanProtoClassEnable(dev, port, value);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, value);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;
    value = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */

        st = cpssExMxBrgVlanProtoClassEnable(dev, port, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanProtoClassEnable(dev, port, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanProtoClassGet
(
    IN GT_U8                        devNum,
    IN GT_32                        entryNum,
    OUT GT_U16                      *etherType,
    OUT CPSS_PROT_CLASS_ENCAP_STC   *encList
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanProtoClassGet)
{
/*
ITERATE_DEVICES
1.1. For each active device call with entryNum [0], non-null etherType, and non-null encList [GT_TRUE, GT_FALSE, GT_FALSE]. Expected: GT_OK.
1.2. Call with out of range entryNum [0xFF]. Expected: NON GT_OK.
1.3. Call with etherType [NULL]. Expected: GT_BAD_PTR.
1.4. Call with encList [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U8                       entryNum;
    GT_U16                      etherType;
    CPSS_PROT_CLASS_ENCAP_STC   encList;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For each active device call with entryNum [0], non-null etherType,  */
        /* and non-null encList [GT_TRUE, GT_FALSE, GT_FALSE].                      */
        /* Expected: GT_OK.                                                         */
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        etherType = 0;

        encList.ethV2 = GT_TRUE;
        encList.llcSnap = GT_FALSE;
        encList.nonLlcSnap = GT_FALSE;

        st = cpssExMxBrgVlanProtoClassGet(dev, entryNum, &etherType, &encList);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, etherType);

        /* 1.2. Call with out of range entryNum [0xFF].     */
        /* Expected: NON GT_OK.                             */
        entryNum = BRG_VLAN_MAX_ENTRY_NUMBER_CNS;
        etherType = 0;

        st = cpssExMxBrgVlanProtoClassGet(dev, entryNum, &etherType, &encList);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, etherType);

        /* 1.3. Call with etherType [NULL]. */
        /* Expected: GT_BAD_PTR.            */
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        etherType = 0;

        st = cpssExMxBrgVlanProtoClassGet(dev, entryNum, NULL, &encList);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, entryNum);

        /* 1.4. Call with encList [NULL].   */
        /* Expected: GT_BAD_PTR.            */

        st = cpssExMxBrgVlanProtoClassGet(dev, entryNum, &etherType, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, entryNum, etherType);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;

    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        etherType = 0;

        st = cpssExMxBrgVlanProtoClassGet(dev, entryNum, &etherType, &encList);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    etherType = 0;

    st = cpssExMxBrgVlanProtoClassGet(dev, entryNum, &etherType, &encList);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanProtoClassSet
(
    IN GT_U8                        devNum,
    IN GT_32                        entryNum,
    IN GT_U16                       etherType,
    IN CPSS_PROT_CLASS_ENCAP_STC    *encList
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanProtoClassSet)
{
/*
ITERATE_DEVICES
1.1. For each active device call with entryNum [0], etherType [1], and non-null encList [GT_TRUE, GT_FALSE, GT_FALSE]. Expected: GT_OK.
1.2. Call cpssExMxBrgVlanProtoClassGet. Expected: GT_OK and the same etherType and encList.
1.3. Call with out of range entryNum [0xFF]. Expected: NON GT_OK.
1.4. Call with encList [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U8                       entryNum;
    GT_U16                      etherType;
    CPSS_PROT_CLASS_ENCAP_STC   encList;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_U16 etherTypeWritten;

        /* 1.1. For each active device call with entryNum [0], etherType [1],   */
        /* and non-null encList [GT_TRUE, GT_FALSE, GT_FALSE].                  */
        /* Expected: GT_OK.                                                     */
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        etherType = 1;

        encList.ethV2 = GT_TRUE;
        encList.llcSnap = GT_FALSE;
        encList.nonLlcSnap = GT_FALSE;

        st = cpssExMxBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, etherType);

        /* 1.2. Call cpssExMxBrgVlanProtoClassGet.                              */
        /* Expected: GT_OK and the same etherType and encList.                  */
        st = cpssExMxBrgVlanProtoClassGet(dev, entryNum, &etherTypeWritten, &encList);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxBrgVlanProtoClassGet: %d, %d, %d", dev, entryNum, etherTypeWritten);
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeWritten, "failed write etherType [%d]", etherType);

        /* 1.3. Call with out of range entryNum [0xFF].                         */
        /* Expected: NON GT_OK.                                                 */
        entryNum = BRG_VLAN_MAX_ENTRY_NUMBER_CNS;

        st = cpssExMxBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, etherType);

        /* 1.4. Call with encList [NULL].                                       */
        /* Expected: GT_BAD_PTR.                                                */
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;

        st = cpssExMxBrgVlanProtoClassSet(dev, entryNum, etherType, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, entryNum, etherType);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
    etherType = 0;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanVidProtoSet
(
    IN GT_U8           devNum,
    IN GT_U8           port,
    IN GT_32           entryNum,
    IN GT_U16          vlanId
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanVidProtoSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call with entryNum [0], vlanId [100]. Expected: GT_OK.
1.1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
1.1.3. Call with out of range entryNum [0xFF]. Expected: NON GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vid;
    GT_U8       port;
    GT_32       entryNum;

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
            /* 1.1.1. Call with entryNum [0], vlanId [100]. */
            /* Expected: GT_OK.                             */
            entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
            vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

            st = cpssExMxBrgVlanVidProtoSet(dev, port, entryNum, vid);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, entryNum, vid);

            /* 1.1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].   */
            /* Expected: GT_BAD_PARAM.                                              */
            vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssExMxBrgVlanVidProtoSet(dev, port, entryNum, vid);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, entryNum, vid);

            /* 1.1.3. Call with out of range entryNum [0xFF].                       */
            /* Expected: NON GT_OK.                                                 */
            entryNum = BRG_VLAN_MAX_ENTRY_NUMBER_CNS;
            vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

            st = cpssExMxBrgVlanVidProtoSet(dev, port, entryNum, vid);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, entryNum, vid);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxBrgVlanVidProtoSet(dev, port, entryNum, vid);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, entryNum, vid);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanVidProtoSet(dev, port, entryNum, vid);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, entryNum, vid);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;
    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
    entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanVidProtoSet(dev, port, entryNum, vid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanVidProtoSet(dev, port, entryNum, vid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanTcProtoSet
(
    IN GT_U8           devNum,
    IN GT_U8           port,
    IN GT_32           entryNum,
    IN GT_U8           trafClass
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanTcProtoSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call with entryNum [0], trafClass [5]. Expected: GT_OK.
1.1.2. Call with out of range trafClass [CPSS_TC_RANGE_CNS]. Expected: NON GT_OK.
1.1.3. Call with out of range entryNum [0xFF]. Expected: NON GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U8       trafClass;
    GT_U8       port;
    GT_32       entryNum;

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
            /* 1.1.1. Call with entryNum [0], trafClass [5] .   */
            /* Expected: GT_OK.                                 */
            entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
            trafClass = 5;

            st = cpssExMxBrgVlanTcProtoSet(dev, port, entryNum, trafClass);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, entryNum, trafClass);

            /* 1.1.2. Call with out of range trafClass [CPSS_TC_RANGE_CNS]    */
            /* Expected: NON GT_OK.                                           */
            trafClass = CPSS_TC_RANGE_CNS;

            st = cpssExMxBrgVlanTcProtoSet(dev, port, entryNum, trafClass);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, entryNum, trafClass);

            /* 1.1.3. Call with out of range entryNum [0xFF].                       */
            /* Expected: NON GT_OK.                                                 */
            entryNum = BRG_VLAN_MAX_ENTRY_NUMBER_CNS;
            trafClass = 5;

            st = cpssExMxBrgVlanTcProtoSet(dev, port, entryNum, trafClass);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, entryNum, trafClass);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        trafClass = 5;

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            st = cpssExMxBrgVlanTcProtoSet(dev, port, entryNum, trafClass);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, entryNum, trafClass);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
        trafClass = 5;

        st = cpssExMxBrgVlanTcProtoSet(dev, port, entryNum, trafClass);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, entryNum, trafClass);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;
    entryNum = BRG_VLAN_TESTED_ENTRY_NUMBER_CNS;
    trafClass = 5;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanTcProtoSet(dev, port, entryNum, trafClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanTcProtoSet(dev, port, entryNum, trafClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanLearningStateSet
(
    IN GT_U8    devNum,
    IN GT_U16   vid,
    IN GT_BOOL  status
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanLearningStateSet)
{
/*
ITERATE_DEVICES
1.1. Call with vid [100], status [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.2. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U16          vid;
    GT_BOOL         status;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with vid [100], status [GT_TRUE and GT_FALSE].         */
        /* Expected: GT_OK.                                                 */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
        status = GT_TRUE;

        st = cpssExMxBrgVlanLearningStateSet(dev, vid, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, status);

        status = GT_FALSE;

        st = cpssExMxBrgVlanLearningStateSet(dev, vid, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, status);

        /* 1.2. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS].    */
        /* Expected: GT_BAD_PARAM.                                          */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgVlanLearningStateSet(dev, vid, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, status);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
    status = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanLearningStateSet(dev, vid, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanLearningStateSet(dev, vid, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanIngressFilteringStatusGet
(
    IN    GT_U8     devNum,
    OUT   GT_U16    *causeVid,
    OUT   GT_U8     *causePort
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanIngressFilteringStatusGet)
{
/*
ITERATE_DEVICES
1.1. For each active devices call with non-null causeVid, and non-null causePort. Expected: GT_OK.
1.2. Call with causeVid [NULL]. Expected: GT_BAD_PTR.
1.3. Call with causePort [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      causeVid;
    GT_U8       causePort;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For each active devices call with non-null causeVid, and non-null causePort.    */
        /* Expected: GT_OK.                                                                     */
        causeVid = 0;
        causePort= 0;

        st = cpssExMxBrgVlanIngressFilteringStatusGet(dev, &causeVid, &causePort);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, causeVid, causePort);

        /* 1.2. Call with causeVid [NULL].  */
        /* Expected: GT_BAD_PTR.            */
        causePort= 0;

        st = cpssExMxBrgVlanIngressFilteringStatusGet(dev, NULL, &causePort);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, NULL, %d", dev, causePort);

        /* 1.3. Call with causePort [NULL]. */
        /* Expected: GT_BAD_PTR.            */
        causeVid = 0;

        st = cpssExMxBrgVlanIngressFilteringStatusGet(dev, &causeVid, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, causeVid);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        causeVid = 0;
        causePort= 0;

        st = cpssExMxBrgVlanIngressFilteringStatusGet(dev, &causeVid, &causePort);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    causeVid = 0;
    causePort= 0;

    st = cpssExMxBrgVlanIngressFilteringStatusGet(dev, &causeVid, &causePort);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_BOOL cpssExMxBrgVlanIsDevMember
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanIsDevMember)
{
/*
ITERATE_DEVICES
1.1. Call cpssExMxBrgVlanEntryWrite to "create" vlanId[100] with port [0] for active device. Expected: GT_OK.
1.2. Call with vlanId [100]. Expected: GT_TRUE.
1.3. Call cpssExMxBrgVlanEntryInvalidate to clear vlanId [100] entry. Expected: GT_OK.
1.4. Call with vlanId [100]. Expected: GT_FALSE.
1.5. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_FALSE.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U16          vid;
    GT_BOOL         res;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        CPSS_PORTS_BMP_STC portsMembers;
        CPSS_PORTS_BMP_STC portsTagging;
        CPSS_VLAN_INFO_STC vlanInfo;

        portsMembers.ports[0] = 1;/*only port 0*/
        portsMembers.ports[1] = 0;

        portsTagging.ports[0] = 1;/*port 0 as tagged*/
        portsTagging.ports[1] = 0;

        vlanInfo.hasLocals = GT_TRUE;/* port 0 is local */
        vlanInfo.hasUplinks = GT_TRUE;/* support multi-device system */
        vlanInfo.isCpuMember = GT_FALSE;
        vlanInfo.learnEnable = GT_TRUE;
        vlanInfo.unregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;
        vlanInfo.perVlanSaMacEnabled = GT_FALSE;
        vlanInfo.saMacSuffix = 0xFf;/*don't care*/
        vlanInfo.stpId = 5;/* just a number */

        /* 1.1. Call cpssExMxBrgVlanEntryWrite to "create" vlanId[100] with port [0] for active device  */
        /* Expected: GT_OK.                                                                             */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanEntryWrite(dev, vid, &portsMembers , &portsTagging , &vlanInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxBrgVlanEntryWrite: %d, %d", dev, vid);

        /* 1.2. Call with vlanId [100].     */
        /* Expected: GT_TRUE.               */
        res = cpssExMxBrgVlanIsDevMember(dev, vid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_TRUE, res, dev, vid);

        /* 1.3. Call cpssExMxBrgVlanEntryInvalidate to clear vlanId [100] entry.    */
        /* Expected: GT_OK.                                                         */
        st = cpssExMxBrgVlanEntryInvalidate(dev, vid);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxBrgVlanEntryInvalidate: %d, %d", dev, vid);

        /* 1.4. Call with vlanId [100].     */
        /* Expected: GT_FALSE.              */
        res = cpssExMxBrgVlanIsDevMember(dev, vid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_FALSE, res, dev, vid);

        /* 1.5. Call with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS].            */
        /* Expected: GT_BAD_PARAM.                                                  */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        res = cpssExMxBrgVlanIsDevMember(dev, vid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, res, dev, vid);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_FALSE.                                              */
        res = cpssExMxBrgVlanIsDevMember(dev, vid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_FALSE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_FALSE.                              */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    res = cpssExMxBrgVlanIsDevMember(dev, vid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_FALSE, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlan2StpIdBind
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   stpId
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlan2StpIdBind)
{
/*
ITERATE_DEVICES
1.1. Call with vlanId [100], stpId [10]. Expected: GT_OK.
1.2. Call cpssExMxBrgVlanStpIdGet with vlandId [100]. Expected: GT_OK and the same stpId [10].
1.3. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
1.4. Call with out of range stpId [MAX_NUMBER_OF_STG_CNS]. Expected: NON GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vlanId;
    GT_U16      stpId;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_U16 stpIdWritten;

        /* 1.1. Call with vlanId [100], stpId [10].                             */
        /* Expected: GT_OK.                                                     */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        stpId = 10;

        st = cpssExMxBrgVlan2StpIdBind(dev, vlanId, stpId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, stpId);

        /* 1.2. Call cpssExMxBrgVlanStpIdGet with vlandId [100].                */
        /* Expected: GT_OK and the same stpId [10].                             */
        st = cpssExMxBrgVlanStpIdGet(dev, vlanId, &stpIdWritten);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxBrgVlanStpIdGet: %d, %d, %d", dev, vlanId, stpIdWritten);
        UTF_VERIFY_EQUAL1_STRING_MAC(stpId, stpIdWritten, "failed write stpId [%d]", stpId);

        /* 1.3. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS].    */
        /* Expected: GT_BAD_PARAM.                                              */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgVlan2StpIdBind(dev, vlanId, stpId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, stpId);

        /* 1.4. Call with out of range stpId [MAX_NUMBER_OF_STG_CNS].           */
        /* Expected: NON GT_OK.                                                 */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        stpId = MAX_NUMBER_OF_STG_CNS;

        st = cpssExMxBrgVlan2StpIdBind(dev, vlanId, stpId);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, stpId);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    stpId = 10;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlan2StpIdBind(dev, vlanId, stpId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlan2StpIdBind(dev, vlanId, stpId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanStpIdGet
(
    IN  GT_U8    devNum,
    IN  GT_U16   vlanId,
    OUT GT_U16   *stpId
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanStpIdGet)
{
/*
ITERATE_DEVICES
1.1. Call with vlanId [100], non-null stpId. Expected: GT_OK.
1.2. Call with stpId [NULL]. Expected: GT_BAD_PTR.
1.3. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vlanId;
    GT_U16      stpId;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with vlanId [100], non-null stpId.                         */
        /* Expected: GT_OK.                                                     */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        stpId = 0;

        st = cpssExMxBrgVlanStpIdGet(dev, vlanId, &stpId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, stpId);

        /* 1.2. Call with stpId [NULL].                                         */
        /* Expected: GT_BAD_PTR.                                                */
        st = cpssExMxBrgVlanStpIdGet(dev, vlanId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, vlanId);

        /* 1.3. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS].    */
        /* Expected: GT_BAD_PARAM.                                              */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        stpId = 0;

        st = cpssExMxBrgVlanStpIdGet(dev, vlanId, &stpId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, stpId);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /*2. Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        stpId = 0;

        st = cpssExMxBrgVlanStpIdGet(dev, vlanId, &stpId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    stpId = 0;

    st = cpssExMxBrgVlanStpIdGet(dev, vlanId, &stpId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanEtherTypeSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    IN  GT_U16                  etherType,
    IN  GT_U16                  vidRange
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanEtherTypeSet)
{
/*
ITERATE_DEVICES
1.1. Call with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E and CPSS_VLAN_ETHERTYPE1_E], etherType [0], and vidRange [100]. Expected: GT_OK.
1.2. Call with out of range vidRange [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
1.3. Call with invalid enumeration etherTypeMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_ETHER_MODE_ENT     etherTypeMode;
    GT_U16                  etherType;
    GT_U16                  vidRange;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E and CPSS_VLAN_ETHERTYPE1_E],    */
        /* etherType [0], and vidRange [100].                                                   */
        /* Expected: GT_OK.                                                                     */
        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
        etherType = 0;
        vidRange = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType, vidRange);

        etherTypeMode = CPSS_VLAN_ETHERTYPE1_E;

        st = cpssExMxBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType, vidRange);

        /* 1.2. Call with out of range vidRange [PRV_CPSS_MAX_NUM_VLANS_CNS].   */
        /* Expected: GT_BAD_PARAM.                                              */
        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
        etherType = 0;
        vidRange = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, etherTypeMode, etherType, vidRange);

        /* 1.3. Call with invalid enumeration etherTypeMode [0x5AAAAAA5].       */
        /* Expected: GT_BAD_PARAM.                                              */
        vidRange = BRG_VLAN_TESTED_VLAN_ID_CNS;
        etherTypeMode = BRG_VLAN_INVALID_ENUM_CNS;

        st = cpssExMxBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, etherTypeMode, etherType, vidRange);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    etherType = 0;
    vidRange = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanPortTagModeSet
(
    GT_U8   devNum,
    GT_U16  vlanId,
    GT_U8   portNum,
    GT_BOOL tagged
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanPortTagModeSet)
{
/*
ITERATE_DEVICES_VIRT_PORTS
1.1.1. Call with vlanId [100], tagged [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vid;
    GT_U8       port;
    GT_BOOL     tagged;

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
            /* 1.1.1. Call with vlanId [100], tagged [GT_TRUE and GT_FALSE].        */
            /* Expected: GT_OK.                                                     */
            vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
            tagged = GT_TRUE;

            st = cpssExMxBrgVlanPortTagModeSet(dev, vid, port, tagged);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vid, port, tagged);

            tagged = GT_FALSE;

            st = cpssExMxBrgVlanPortTagModeSet(dev, vid, port, tagged);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vid, port, tagged);

            /* 1.1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].   */
            /* Expected: GT_BAD_PARAM.                                              */
            vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssExMxBrgVlanPortTagModeSet(dev, vid, port, tagged);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, port, tagged);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.3. Go over all non active virtual ports and active device id. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.3.1. <Call function for non active port and valid parameters>.     */
            /* Expected: GT_BAD_PARAM.                                              */
            vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
            tagged = GT_TRUE;

            st = cpssExMxBrgVlanPortTagModeSet(dev, vid, port, tagged);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, port, tagged);
        }

        /* 1.4. Call with out of range for virtual port.                            */
        /* Expected: GT_BAD_PARAM.                                                  */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
        tagged = GT_TRUE;

        st = cpssExMxBrgVlanPortTagModeSet(dev, vid, port, tagged);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, port, tagged);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;
    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
    tagged = GT_TRUE;

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanPortTagModeSet(dev, vid, port, tagged);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanPortTagModeSet(dev, vid, port, tagged);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanTableInvalidate
(
    IN GT_U8 devNum
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanTableInvalidate)
{
/*
ITERATE_DEVICES
1.1. For each active device call function. Expected: GT_OK.
1.2. Call cpssExMxBrgVlanIsDevMember with vlanId [100]. Expected: GT_FALSE.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_U16      vid;
        GT_BOOL     res;

        /* 1.1. For each active device call function.   */
        /* Expected: GT_OK.                             */
        st = cpssExMxBrgVlanTableInvalidate(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call cpssExMxBrgVlanIsDevMember with vlanId [100].  */
        /* Expected: GT_FALSE.                                      */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        res = cpssExMxBrgVlanIsDevMember(dev, vid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_FALSE, res, dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanTableInvalidate(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanTableInvalidate(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanEntryInvalidate
(
    IN GT_U8    devNum,
    IN GT_U16   vid
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanEntryInvalidate)
{
/*
ITERATE_DEVICES
1.1. For each active device call with vid [100]. Expected: GT_OK.
1.2. Call cpssExMxBrgVlanIsDevMember with vlanId [100]. Expected: GT_FALSE.
1.3. Call with out of range for vid [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vid;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_BOOL     res;

        /* 1.1. For each active device call with vid [100].                     */
        /* Expected: GT_OK.                                                     */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgVlanEntryInvalidate(dev, vid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* 1.2. Call cpssExMxBrgVlanIsDevMember with vlanId [100].              */
        /* Expected: GT_FALSE.                                                  */
        res = cpssExMxBrgVlanIsDevMember(dev, vid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_FALSE, res, dev, vid);

        /* 1.3. Call with out of range for vid [PRV_CPSS_MAX_NUM_VLANS_CNS].    */
        /* Expected: GT_BAD_PARAM.                                              */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgVlanEntryInvalidate(dev, vid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vid);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanEntryInvalidate(dev, vid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanEntryInvalidate(dev, vid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanMacPerVlanSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_U8   saMac,
    IN GT_BOOL saMacEn
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanMacPerVlanSet)
{
/*
ITERATE_DEVICES
1.1. For each active device call with vlanId [100], saMac [0], and saMacEn [GT_TRUE and GT_FALSE]. Expected: GT_OK.
1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_U16      vid;
    GT_U8       saMac;
    GT_BOOL     saMacEn;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. For each active device call with vlanId [100], saMac [0], and saMacEn [GT_TRUE and GT_FALSE].   */
        /* Expected: GT_OK.                                                                                     */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
        saMac = 0;
        saMacEn = GT_TRUE;

        st = cpssExMxBrgVlanMacPerVlanSet(dev, vid, saMac, saMacEn);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vid, saMac, saMacEn);

        saMacEn = GT_FALSE;

        st = cpssExMxBrgVlanMacPerVlanSet(dev, vid, saMac, saMacEn);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vid, saMac, saMacEn);

        /* 1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].     */
        /* Expected: GT_BAD_PARAM.                                              */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgVlanMacPerVlanSet(dev, vid, saMac, saMacEn);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, saMac, saMacEn);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
    saMac = 98;
    saMacEn = GT_FALSE;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanMacPerVlanSet(dev, vid, saMac, saMacEn);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanMacPerVlanSet(dev, vid, saMac, saMacEn);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanUnregMcFilterSet
(
    IN GT_U8                                    devNum,
    IN GT_U16                                   vlanId,
    IN CPSS_EXMX_UNREG_MC_EGR_FILTER_CMD_ENT    unregMcFilterCmd
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanUnregMcFilterSet)
{
/*
ITERATE_DEVICES
1.1. Call with vlanId [100], unregMcFilterCmd [CPSS_UNREG_MC_VLAN_FRWD_E]. Expected: GT_OK.
1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS]. Expected: GT_BAD_PARAM.
1.3. Call with invalid enumeration unregMcFilterCmd [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                          st = GT_OK;
    GT_U8                              dev;
    GT_U16                             vid;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT   unregMcFilterCmd;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with vlanId [100], unregMcFilterCmd [CPSS_UNREG_MC_VLAN_FRWD_E].  */
        /* Expected: GT_OK.                                                                 */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
        unregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;

        st = cpssExMxBrgVlanUnregMcFilterSet(dev, vid, unregMcFilterCmd);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vid, unregMcFilterCmd);

        /* 1.2. Call with out of range vlandId [PRV_CPSS_MAX_NUM_VLANS_CNS].    */
        /* Expected: GT_BAD_PARAM.                                              */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgVlanUnregMcFilterSet(dev, vid, unregMcFilterCmd);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, unregMcFilterCmd);

        /* 1.3. Call with invalid enumeration unregMcFilterCmd [0x5AAAAAA5].    */
        /* Expected: GT_BAD_PARAM.                                              */
        vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
        unregMcFilterCmd = BRG_VLAN_INVALID_ENUM_CNS;

        st = cpssExMxBrgVlanUnregMcFilterSet(dev, vid, unregMcFilterCmd);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vid, unregMcFilterCmd);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    vid = BRG_VLAN_TESTED_VLAN_ID_CNS;
    unregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanUnregMcFilterSet(dev, vid, unregMcFilterCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanUnregMcFilterSet(dev, vid, unregMcFilterCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgVlanUnregMcEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  unregMcFilterPerVlanEnable
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgVlanUnregMcEnable)
{
/*
ITERATE_DEVICES
1.1. Call with unregMcFilterPerVlanEnable [GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     unregMcFilterPerVlanEnable;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with unregMcFilterPerVlanEnable [GT_TRUE and GT_FALSE].    */
        /* Expected: GT_OK.                                                     */
        unregMcFilterPerVlanEnable = GT_TRUE;

        st = cpssExMxBrgVlanUnregMcEnable(dev, unregMcFilterPerVlanEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, unregMcFilterPerVlanEnable);

        unregMcFilterPerVlanEnable = GT_FALSE;

        st = cpssExMxBrgVlanUnregMcEnable(dev, unregMcFilterPerVlanEnable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, unregMcFilterPerVlanEnable);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    unregMcFilterPerVlanEnable = GT_FALSE;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxBrgVlanUnregMcEnable(dev, unregMcFilterPerVlanEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgVlanUnregMcEnable(dev, unregMcFilterPerVlanEnable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cppExMxBrgVlan suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxBrgVlan)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanMaxVidSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanMemberAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanPortDelete)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanHasUplinksSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanHasLocalsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanPortPvidGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanPortPvidSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanPortIngFltEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanIpClassEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanSubnetClassEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanSubnetClassVidSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanSubnetClassTcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanSubnetClassEntryEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanProtoClassEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanProtoClassGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanProtoClassSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanVidProtoSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanTcProtoSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanLearningStateSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanIngressFilteringStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanIsDevMember)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlan2StpIdBind)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanStpIdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanPortTagModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanTableInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanMacPerVlanSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanUnregMcFilterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgVlanUnregMcEnable)
UTF_SUIT_END_TESTS_MAC(cpssExMxBrgVlan)


