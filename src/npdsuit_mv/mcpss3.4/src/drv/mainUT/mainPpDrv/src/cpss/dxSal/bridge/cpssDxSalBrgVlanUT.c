/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
*  cpssDxSalBrgVlanUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxSalBrgVlan, that provides 
*            VLAN facility DxSal cpss implementation
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
/* includes */
#include <cpss/dxSal/bridge/cpssDxSalBrgVlan.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/generic/cos/cpssCosTypes.h>


#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */
#define BRG_VLAN_INVALID_ENUM_CNS    0x5AAAAAA5

/* the following definition should be synchronized with cpssDxSalBrgVlan.c 
   because it is taken from it : */

/* number of Protocols */
#define PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS  4

/* the following definition should be synchronized with cpssDxSalBrgVlan.c 
   because it is taken from it : */

/* maximal number of entries in the Internal Vlan Table */
#define PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS  256

/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanEtherTypeSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    IN  GT_U16                  etherType,
    IN  GT_U16                  vidRange
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanEtherTypeSet)
{
/*
ITERATE_DEVICES(DxSal)
1.1. Call function with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E / 
CPSS_VLAN_ETHERTYPE1_E], etherType [0x8100 / 0xAAAA] and vidRange [50 / 100]. 
Expected: GT_OK.
1.2. Call function with out of range etherTypeMode [0x5AAAAAA5] and other 
parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.3. Call function with etherType [0xFFFF] (no constraints in the contract) 
and other parameters the same as in 1.1. Expected: GT_OK.
1.4. Call function with out of range vidRange [PRV_CPSS_MAX_NUM_VLANS_CNS] 
and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_ETHER_MODE_ENT     etherTypeMode;
    GT_U16                  etherType;
    GT_U16                  vidRange;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E / 
        CPSS_VLAN_ETHERTYPE1_E], etherType [0x8100 / 0xAAAA] and vidRange [50 / 100]. 
        Expected: GT_OK. */
        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
        etherType = 0x8100;
        vidRange = 50;

        st = cpssDxSalBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType, vidRange);

        etherTypeMode = CPSS_VLAN_ETHERTYPE1_E;
        etherType = 0xAAAA;
        vidRange = 100;

        st = cpssDxSalBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType, vidRange);

        /* 1.2. Call function with out of range etherTypeMode [0x5AAAAAA5] and other 
        parameters the same as in 1.1. Expected: GT_BAD_PARAM. */
        etherTypeMode = BRG_VLAN_INVALID_ENUM_CNS;

        st = cpssDxSalBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, etherTypeMode = %d",
                                     dev, etherTypeMode);

        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E; /* restore */

        /* 1.3. Call function with etherType [0xFFFF] (no constraints in the contract) 
        and other parameters the same as in 1.1. Expected: GT_OK. */
        etherType = 0xFFFF;

        st = cpssDxSalBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, etherType = %d",
                                     dev, etherType);

        /* 1.4. Call function with out of range vidRange [PRV_CPSS_MAX_NUM_VLANS_CNS] 
        and other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */
        vidRange = PRV_CPSS_MAX_NUM_VLANS_CNS;
        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
        etherType = 0xAAAA;

        st = cpssDxSalBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, vidRange = %d",
                                     dev, vidRange);
    }
    etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    etherType = 0x8100;
    vidRange = 50;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanProtoClassEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanProtoClassEnable)
{
/*
ITERATE_DEVICES_PHY_PORT (DxSal)
1.1. Call function with enable [GT_TRUE / GT_FALSE]. Expected: GT_OK. 
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           port;
    GT_BOOL         enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1. Call function with enable [GT_TRUE / GT_FALSE]. Expected: GT_OK.  */
            enable = GT_TRUE;

            st = cpssDxSalBrgVlanProtoClassEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            enable = GT_FALSE;

            st = cpssDxSalBrgVlanProtoClassEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
        }
        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalBrgVlanProtoClassEnable(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalBrgVlanProtoClassEnable(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgVlanProtoClassEnable(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }
    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanProtoClassEnable(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanProtoClassEnable(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanProtoClassSet
(
    IN GT_U8                        devNum,
    IN GT_U32                       entryNum,
    IN GT_U16                       etherType,
    IN CPSS_PROT_CLASS_ENCAP_STC    *encListPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanProtoClassSet)
{
/*
ITERATE_DEVICES(DxSal)
1.1. Call function with entryNum [1 / 2], etherType [10 / 2000] and non-NULL 
encListPtr {ethV2 [GT_TRUE / GT_FALSE]; nonLlcSnap [GT_TRUE / GT_FALSE]; 
llcSnap [GT_TRUE / GT_FALSE]}. Expected: GT_OK.
1.2. Call cpssDxSalBrgVlanProtoClassGet with the same parameters as in 1.1. 
Expected: GT_OK and the same etherType and encListPtr.
1.3. Call function with out of range entryNum 
[PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS] and other parameters the same 
as in 1.1. Expected: NON GT_OK.
1.4. Call function with etherType [0xFFFF] (no constraints in the contract) and
 other parameters the same as in 1.1. Expected: GT_OK.
1.5. Call function with encListPtr [NULL] and other parameters the same as in 1.1. 
Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U32                      entryNum;
    GT_U16                      etherType;
    GT_U16                      retEtherType;
    CPSS_PROT_CLASS_ENCAP_STC   encList;
    CPSS_PROT_CLASS_ENCAP_STC   retEncList;
    GT_BOOL                     isEqual;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with entryNum [1 / 2], etherType [10 / 2000] and non-NULL 
        encListPtr {ethV2 [GT_TRUE / GT_FALSE]; nonLlcSnap [GT_TRUE / GT_FALSE]; 
        llcSnap [GT_TRUE / GT_FALSE]}. Expected: GT_OK.
        1.2. Call cpssDxSalBrgVlanProtoClassGet with the same parameters as in 1.1. 
        Expected: GT_OK and the same etherType and encListPtr. */
        entryNum = 1;
        etherType = 10;
        encList.ethV2 = GT_TRUE;
        encList.nonLlcSnap = GT_TRUE;
        encList.llcSnap = GT_TRUE;

        st = cpssDxSalBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, etherType);

        st = cpssDxSalBrgVlanProtoClassGet(dev, entryNum, &retEtherType, &retEncList);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgVlanProtoClassGet: %d, %d", dev, entryNum);
        UTF_VERIFY_EQUAL2_STRING_MAC(etherType, retEtherType, 
                                     "got another etherType than was set: %d, %d",
                                     dev, entryNum);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&encList, (GT_VOID*)&retEncList, 
                                     sizeof(encList)))
                   ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
                                     "got another encList than was set: %d, %d",
                                     dev, entryNum);

        entryNum = 2;
        etherType = 2000;
        encList.ethV2 = GT_FALSE;
        encList.nonLlcSnap = GT_FALSE;
        encList.llcSnap = GT_FALSE;

        st = cpssDxSalBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, etherType);

        st = cpssDxSalBrgVlanProtoClassGet(dev, entryNum, &retEtherType, &retEncList);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgVlanProtoClassGet: %d, %d", dev, entryNum);
        UTF_VERIFY_EQUAL2_STRING_MAC(etherType, retEtherType, 
                                     "got another etherType than was set: %d, %d",
                                     dev, entryNum);
        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&encList, (GT_VOID*)&retEncList, 
                                     sizeof(encList)))
                   ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
                                     "got another encList than was set: %d, %d",
                                     dev, entryNum);

        /* 1.3. Call function with out of range entryNum 
        [PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS] and other parameters the same 
        as in 1.1. Expected: NON GT_OK. */
        entryNum = PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS;

        st = cpssDxSalBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);

        /* 1.4. Call function with etherType [0xFFFF] (no constraints in the contract) and
        other parameters the same as in 1.1. Expected: GT_OK. */
        etherType = 0xFFFF;
        entryNum = 2;

        st = cpssDxSalBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryNum, etherType);

        /* 1.5. Call function with encListPtr [NULL] and other parameters the same as in 1.1. 
        Expected: GT_BAD_PTR. */
        etherType = 10;
        st = cpssDxSalBrgVlanProtoClassSet(dev, entryNum, etherType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, encListPtr = NULL", dev);
    }
    entryNum = 1;
    etherType = 10;
    encList.ethV2 = GT_TRUE;
    encList.nonLlcSnap = GT_TRUE;
    encList.llcSnap = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanProtoClassSet(dev, entryNum, etherType, &encList);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanProtoClassGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      entryNum,
    OUT GT_U16                      *etherTypePtr,
    OUT CPSS_PROT_CLASS_ENCAP_STC   *encListPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanProtoClassGet)
{
/*
ITERATE_DEVICES(DxSal)
1.1. Call function with entryNum [1 / 2], non-NULL etherTypePtr and non-NULL 
encListPtr. Expected: GT_OK.
1.2. Call function with out of range entryNum 
[PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS] and other parameters the same 
as in 1.1. Expected: NON GT_OK.
1.3. Call function with etherTypePtr [NULL] and other parameters the same as 
in 1.1. Expected: GT_BAD_PTR.
1.4. Call function with encListPtr [NULL] ] and other parameters the same as 
in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;
    GT_U32                      entryNum;
    GT_U16                      retEtherType = 0;
    CPSS_PROT_CLASS_ENCAP_STC   retEncList = {GT_FALSE, GT_FALSE, GT_FALSE};

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with entryNum [1 / 2], non-NULL etherTypePtr and non-NULL 
        encListPtr. Expected: GT_OK. */
        entryNum = 1;

        st = cpssDxSalBrgVlanProtoClassGet(dev, entryNum, &retEtherType, &retEncList);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);

        entryNum = 2;

        st = cpssDxSalBrgVlanProtoClassGet(dev, entryNum, &retEtherType, &retEncList);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);

        /* 1.2. Call function with out of range entryNum 
        [PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS] and other parameters the same 
        as in 1.1. Expected: NON GT_OK. */
        entryNum = PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS;

        st = cpssDxSalBrgVlanProtoClassGet(dev, entryNum, &retEtherType, &retEncList);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryNum);

        /* 1.3. Call function with etherTypePtr [NULL] and other parameters the same as 
        in 1.1. Expected: GT_BAD_PTR. */
        entryNum = 1;

        st = cpssDxSalBrgVlanProtoClassGet(dev, entryNum, NULL, &retEncList);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, retEtherType=NULL",
                                     dev);

        /* 1.4. Call function with encListPtr [NULL] ] and other parameters the same as 
        in 1.1. Expected: GT_BAD_PTR. */

        st = cpssDxSalBrgVlanProtoClassGet(dev, entryNum, &retEtherType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, retEncList=NULL",
                                     dev);
    }
    entryNum = 1;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanProtoClassGet(dev, entryNum, &retEtherType, &retEncList);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanProtoClassGet(dev, entryNum, &retEtherType, &retEncList);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanPortProtoVidSet
(
    IN GT_U8           devNum,
    IN GT_U8           port,
    IN GT_U32          entryNum,
    IN GT_U16          vlanId
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanPortProtoVidSet)
{
/*
ITERATE_DEVICES_PHY_PORT (DxSal)
1.1. Call function with entryNum [1 / 2] and vlanId [100 / 4095]. 
Expected: GT_OK.
1.2. Call function with out of range entryNum 
[PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS] and vlanId the same as in 1.1. 
Expected: NON GT_OK.
1.3. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] and 
entryNum the same as in 1.1. Expected: GT_BAD_PARAM. 
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           port;
    GT_U32          entryNum;
    GT_U16          vlanId;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1. Call function with entryNum [1 / 2] and vlanId [100 / 4095].
            Expected: GT_OK. */
            entryNum = 1;
            vlanId = 100;

            st = cpssDxSalBrgVlanPortProtoVidSet(dev, port, entryNum, vlanId);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, entryNum, vlanId);

            entryNum = 2;
            vlanId = 4095;

            st = cpssDxSalBrgVlanPortProtoVidSet(dev, port, entryNum, vlanId);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, entryNum, vlanId);

            /* 1.2. Call function with out of range entryNum 
            [PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS] and vlanId the same as in 1.1. 
            Expected: NON GT_OK. */
            entryNum = PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS;

            st = cpssDxSalBrgVlanPortProtoVidSet(dev, port, entryNum, vlanId);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryNum = %d", dev, entryNum);

            entryNum = 1;   /* restore */

            /* 1.3. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] and 
            entryNum the same as in 1.1. Expected: GT_BAD_PARAM.  */
            vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxSalBrgVlanPortProtoVidSet(dev, port, entryNum, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, vlanId = %d", dev, vlanId);
        }
        entryNum = 1;
        vlanId = 100;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalBrgVlanPortProtoVidSet(dev, port, entryNum, vlanId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalBrgVlanPortProtoVidSet(dev, port, entryNum, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgVlanPortProtoVidSet(dev, port, entryNum, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }
    entryNum = 1;
    vlanId = 100;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanPortProtoVidSet(dev, port, entryNum, vlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanPortProtoVidSet(dev, port, entryNum, vlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanPortProtoTcSet
(
    IN GT_U8           devNum,
    IN GT_U8           port,
    IN GT_U32          entryNum,
    IN GT_U8           trafClass
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanPortProtoTcSet)
{
/*
ITERATE_DEVICES_PHY_PORT (DxSal)
1.1. Call function with entryNum [1 / 2] and trafClass [1 / 2]. Expected: GT_OK.
1.2. Call function with out of range entryNum 
[PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS] and trafClass [3]. Expected: 
NON GT_OK.
1.3. Call function with out of range trafClass [CPSS_4_TC_RANGE_CNS] and 
entryNum [1]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           port;
    GT_U32          entryNum;
    GT_U8           trafClass;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1. Call function with entryNum [1 / 2] and trafClass [1 / 2]. 
            Expected: GT_OK. */
            entryNum = 1;
            trafClass = 1;

            st = cpssDxSalBrgVlanPortProtoTcSet(dev, port, entryNum, trafClass);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, entryNum, trafClass);

            entryNum = 2;
            trafClass = 2;

            st = cpssDxSalBrgVlanPortProtoTcSet(dev, port, entryNum, trafClass);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, entryNum, trafClass);

            /* 1.2. Call function with out of range entryNum 
            [PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS] and trafClass [3]. Expected: 
            NON GT_OK. */
            entryNum = PRV_CPSS_DXSAL_BRG_PORT_PROT_ENTRY_NUM_MAX_CNS;
            trafClass = 3;

            st = cpssDxSalBrgVlanPortProtoTcSet(dev, port, entryNum, trafClass);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryNum = %d",
                                             dev, entryNum);
            entryNum = 1;   /* restore */

            /* 1.3. Call function with out of range trafClass [CPSS_4_TC_RANGE_CNS] and 
            entryNum [1]. Expected: GT_BAD_PARAM. */
            trafClass = CPSS_4_TC_RANGE_CNS;

            st = cpssDxSalBrgVlanPortProtoTcSet(dev, port, entryNum, trafClass);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, trafClass = %d",
                                         dev, trafClass);
        }
        entryNum = 1;
        trafClass = 1;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalBrgVlanPortProtoTcSet(dev, port, entryNum, trafClass);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalBrgVlanPortProtoTcSet(dev, port, entryNum, trafClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgVlanPortProtoTcSet(dev, port, entryNum, trafClass);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }
    entryNum = 1;
    trafClass = 1;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanPortProtoTcSet(dev, port, entryNum, trafClass);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanPortProtoTcSet(dev, port, entryNum, trafClass);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanLearningStateSet
(
    IN GT_U8    devNum,
    IN GT_U32   entryIdx,
    IN GT_BOOL  status
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanLearningStateSet)
{
/*
ITERATE_DEVICES(DxSal)
1.1. Call function with entryIdx [10 / 100] and status [GT_TRUE / GT_FALSE]. 
Expected: GT_OK. 
1.2. Call function with out of range entryIdx 
[PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS] and status [GT_TRUE]. 
Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                  entryIdx;
    GT_BOOL                 status;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with entryIdx [10 / 100] and status [GT_TRUE / GT_FALSE]. 
        Expected: GT_OK.  */
        entryIdx = 10;
        status = GT_TRUE;

        st = cpssDxSalBrgVlanLearningStateSet(dev, entryIdx, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryIdx, status);

        entryIdx = 100;
        status = GT_FALSE;

        st = cpssDxSalBrgVlanLearningStateSet(dev, entryIdx, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, entryIdx, status);

        /* 1.2. Call function with out of range entryIdx 
        [PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS] and status [GT_TRUE]. 
        Expected: NON GT_OK. */
        entryIdx = PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS;
        status = GT_TRUE;

        st = cpssDxSalBrgVlanLearningStateSet(dev, entryIdx, status);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryIdx = %d",
                                         dev, entryIdx);
    }
    entryIdx = 10;
    status = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanLearningStateSet(dev, entryIdx, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanLearningStateSet(dev, entryIdx, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanPortIngFltEnable
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanPortIngFltEnable)
{
/*
ITERATE_DEVICES_PHY_PORT (DxSal)
1.1. Call function with enable [GT_TRUE / GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           port;
    GT_BOOL         enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1. Call function with enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK. */
            enable = GT_TRUE;

            st = cpssDxSalBrgVlanPortIngFltEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            enable = GT_FALSE;

            st = cpssDxSalBrgVlanPortIngFltEnable(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);
        }
        enable = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalBrgVlanPortIngFltEnable(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalBrgVlanPortIngFltEnable(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgVlanPortIngFltEnable(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }
    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanPortIngFltEnable(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanPortIngFltEnable(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanPortVidGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_U16  *vlanIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanPortVidGet)
{
/*
ITERATE_DEVICES_PHY_PORT (DxSal)
1.1. Call function with non-NULL vlanIdPtr. Expected: GT_OK.
1.2. Call function with vlanIdPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           port;
    GT_U16          retVlanId = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1. Call function with non-NULL vlanIdPtr. Expected: GT_OK. */
            st = cpssDxSalBrgVlanPortVidGet(dev, port, &retVlanId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /* 1.2. Call function with vlanIdPtr [NULL]. Expected: GT_BAD_PTR. */
            st = cpssDxSalBrgVlanPortVidGet(dev, port, NULL);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vlanIdPtr = NULL", dev);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalBrgVlanPortVidGet(dev, port, &retVlanId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalBrgVlanPortVidGet(dev, port, &retVlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgVlanPortVidGet(dev, port, &retVlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanPortVidGet(dev, port, &retVlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanPortVidGet(dev, port, &retVlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanPortVidSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    IN  GT_U16  vlanId
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanPortVidSet)
{
/*
ITERATE_DEVICES_PHY_PORT (DxSal)
1.1. Call function with vlanId [100 / 4095]. Expected: GT_OK.
1.2. Call cpssDxSalBrgVlanPortVidGet. Expected: GT_OK and the same vlanId.
1.3. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           port;
    GT_U16          vlanId;
    GT_U16          retVlanId = 100;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.1. For all active devices go over all available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1. Call function with vlanId [100 / 4095]. Expected: GT_OK.
            1.2. Call cpssDxSalBrgVlanPortVidGet. Expected: GT_OK and the same vlanId. */
            vlanId = 100;

            st = cpssDxSalBrgVlanPortVidSet(dev, port, vlanId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vlanId);

            st = cpssDxSalBrgVlanPortVidGet(dev, port, &retVlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgVlanPortVidGet: %d, %d",
                                         dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanId, retVlanId, "got another vlanId than was set: %d, %d",
                                         dev, port);

            vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS - 1;

            st = cpssDxSalBrgVlanPortVidSet(dev, port, vlanId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vlanId);

            st = cpssDxSalBrgVlanPortVidGet(dev, port, &retVlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgVlanPortVidGet: %d, %d",
                                         dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanId, retVlanId, "got another vlanId than was set: %d, %d",
                                         dev, port);

            /* 1.3. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_BAD_PARAM. */
            vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssDxSalBrgVlanPortVidSet(dev, port, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, vlanId=%d", dev, vlanId);
        }
        vlanId = 100;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. For all active devices go over all non available physical ports. */
        while(GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalBrgVlanPortVidSet(dev, port, vlanId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }
        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalBrgVlanPortVidSet(dev, port, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgVlanPortVidSet(dev, port, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }
    vlanId = 100;

    /* 2. For not active devices check that function returns non GT_OK.*/
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanPortVidSet(dev, port, vlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanPortVidSet(dev, port, vlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanTranslationTableEntriesRangeSet
(
    IN  GT_U8   devNum,
    IN  GT_U16  firstVid,
    IN  GT_U32  numberOfVids,
    IN  GT_U32  internalVlanTblIdx
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanTranslationTableEntriesRangeSet)
{
/*
ITERATE_DEVICES(DxSal)
1.1. Call function with firstVid [50 / 2000], numberOfVids [100 / 2095] and 
internalVlanTblIdx [0 / 255]. Expected: GT_OK.
1.2. Call cpssDxSalBrgVlanTranslationTableEntryGet for each Vid in the specified
range. Expected: GT_OK and the same internalVlanTblIdx.
1.3. Call function with out of range firstVid [PRV_CPSS_MAX_NUM_VLANS_CNS] and
other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.4. Call function with out of range numberOfVids [PRV_CPSS_MAX_NUM_VLANS_CNS]
and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.5. Call function with firstVid [3000], numberOfVids [3000] and 
internalVlanTblIdx [0]. Expected: GT_BAD_PARAM.
1.6. Call function with out of range internalVlanTblIdx 
[PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS] and other parameters 
the same as in 1.1. Expected: NON GT_OK.
1.7. Call cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate. 
Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U16                  firstVid;
    GT_U32                  numberOfVids;
    GT_U32                  internalVlanTblIdx;
    GT_U32                  retInternalVlanTblIdx = 0;
    GT_U32                  i;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with firstVid [50 / 2000], numberOfVids [100 / 2095] and 
        internalVlanTblIdx [0 / 255]. Expected: GT_OK.
        1.2. Call cpssDxSalBrgVlanTranslationTableEntryGet for each Vid in the specified
        range. Expected: GT_OK and the same internalVlanTblIdx. 
        1.7. Call cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate. 
        Expected: GT_OK. */
        firstVid = 50;
        numberOfVids = 100;
        internalVlanTblIdx = 0;

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeSet(dev, 
                  firstVid, numberOfVids, internalVlanTblIdx);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, firstVid, numberOfVids, internalVlanTblIdx);

        for (i = firstVid; i < (firstVid + numberOfVids); i++)
        {
            st = cpssDxSalBrgVlanTranslationTableEntryGet(dev, (GT_U16)i, &retInternalVlanTblIdx);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                "cpssDxSalBrgVlanTranslationTableEntryGet: %d, %d", dev, i);
            UTF_VERIFY_EQUAL2_STRING_MAC(internalVlanTblIdx, retInternalVlanTblIdx,
                "got another internalVlanTblIdx than was set: %d, %d", dev, i);
        }

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid, numberOfVids);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate: %d, %d, %d", dev, firstVid, numberOfVids);

        firstVid = 2000;
        numberOfVids = 2095;
        internalVlanTblIdx = 255;

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeSet(dev, 
                  firstVid, numberOfVids, internalVlanTblIdx);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, firstVid, numberOfVids, internalVlanTblIdx);

        for (i = firstVid; i < (firstVid + numberOfVids); i++)
        {
            st = cpssDxSalBrgVlanTranslationTableEntryGet(dev, (GT_U16)i, &retInternalVlanTblIdx);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                "cpssDxSalBrgVlanTranslationTableEntryGet: %d, %d", dev, i);
            UTF_VERIFY_EQUAL2_STRING_MAC(internalVlanTblIdx, retInternalVlanTblIdx,
                "got another internalVlanTblIdx than was set: %d, %d", dev, i);
        }

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid, numberOfVids);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate: %d, %d, %d", dev, firstVid, numberOfVids);

        /* 1.3. Call function with out of range firstVid [PRV_CPSS_MAX_NUM_VLANS_CNS] and
        other parameters the same as in 1.1. Expected: GT_BAD_PARAM. 
        1.7. Call cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate. 
        Expected: GT_OK. */

        firstVid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeSet(dev, 
                  firstVid, numberOfVids, internalVlanTblIdx);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, firstVid = %d", dev, firstVid);

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid, numberOfVids);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate: %d, %d, %d", dev, firstVid, numberOfVids);

        /* restore */
        firstVid = 50;

        /* 1.4. Call function with out of range numberOfVids [PRV_CPSS_MAX_NUM_VLANS_CNS]
        and other parameters the same as in 1.1. Expected: GT_BAD_PARAM. 
        1.7. Call cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate. 
        Expected: GT_OK. */

        numberOfVids = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeSet(dev, 
                  firstVid, numberOfVids, internalVlanTblIdx);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, numberOfVids = %d",
                                     dev, numberOfVids);

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid, numberOfVids);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate: %d, %d, %d", dev, firstVid, numberOfVids);

        /* 1.5. Call function with firstVid [3000], numberOfVids [3000] and 
        internalVlanTblIdx [0]. Expected: GT_BAD_PARAM. 
        1.7. Call cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate. 
        Expected: GT_OK. */

        firstVid = 3000;
        numberOfVids = 3000;
        internalVlanTblIdx = 0;

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeSet(dev, 
                  firstVid, numberOfVids, internalVlanTblIdx);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, firstVid=%d, numberOfVids=%d",
                                     dev, firstVid, numberOfVids);

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid, numberOfVids);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate: %d, %d, %d", dev, firstVid, numberOfVids);

        /* restore */
        firstVid = 50;
        numberOfVids = 100;

        /* 1.6. Call function with out of range internalVlanTblIdx 
        [PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS] and other parameters 
        the same as in 1.1. Expected: NON GT_OK. 
        1.7. Call cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate. 
        Expected: GT_OK. */

        internalVlanTblIdx = PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS;

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeSet(dev, 
                  firstVid, numberOfVids, internalVlanTblIdx);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, internalVlanTblIdx = %d",
                                         dev, internalVlanTblIdx);

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid, numberOfVids);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate: %d, %d, %d", dev, firstVid, numberOfVids);
    }
    firstVid = 50;
    numberOfVids = 100;
    internalVlanTblIdx = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanTranslationTableEntriesRangeSet(dev, 
                  firstVid, numberOfVids, internalVlanTblIdx);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanTranslationTableEntriesRangeSet(dev, 
              firstVid, numberOfVids, internalVlanTblIdx);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanTranslationTableEntryGet
(
    IN  GT_U8                  devNum,
    IN  GT_U16                 vlanId,
    OUT GT_U32            *internalVlanTblIdxPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanTranslationTableEntryGet)
{
/*
ITERATE_DEVICES(DxSal)
1.1. Call function with vlanId [100 / 4095] and non-NULL internalVlanTblIdxPtr.
Expected: GT_OK.
1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] and 
non-NULL internalVlanTblIdxPtr. Expected: GT_BAD_PARAM.
1.3. Call function with internalVlanTblIdxPtr [NULL] and vlanId [100].
Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U16                  vlanId;
    GT_U32                  retInternalVlanTblIdx = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with vlanId [100 / 4095] and non-NULL internalVlanTblIdxPtr.
        Expected: GT_OK. */
        vlanId = 100;

        st = cpssDxSalBrgVlanTranslationTableEntryGet(dev, vlanId, &retInternalVlanTblIdx);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS-1;

        st = cpssDxSalBrgVlanTranslationTableEntryGet(dev, vlanId, &retInternalVlanTblIdx);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /* 1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] and 
        non-NULL internalVlanTblIdxPtr. Expected: GT_BAD_PARAM. */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxSalBrgVlanTranslationTableEntryGet(dev, vlanId, &retInternalVlanTblIdx);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        /* restore */
        vlanId = 100;

        /* 1.3. Call function with internalVlanTblIdxPtr [NULL] and vlanId [100].
        Expected: GT_BAD_PTR. */
        st = cpssDxSalBrgVlanTranslationTableEntryGet(dev, vlanId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, internalVlanTblIdxPtr=NULL",dev);
    }
    vlanId = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanTranslationTableEntryGet(dev, vlanId, &retInternalVlanTblIdx);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanTranslationTableEntryGet(dev, vlanId, &retInternalVlanTblIdx);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate
(
    IN  GT_U8   devNum,
    IN  GT_U16  firstVid,
    IN  GT_U32  numberOfVids
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate)
{
/*
ITERATE_DEVICES(DxSal)
1.1. Call function with firstVid [100 / 2095] and numberOfVids [50 / 2000].
Expected: GT_OK.
1.2. Call function with out of range firstVid [PRV_CPSS_MAX_NUM_VLANS_CNS]
and numberOfVids the same as in 1.1. Expected: GT_BAD_PARAM.
1.3. Call function with out of range numberOfVids [PRV_CPSS_MAX_NUM_VLANS_CNS]
and firstVid the same as in 1.1. Expected: GT_BAD_PARAM.
1.4. Call function with firstVid [3000] and numberOfVids [3000].
Expected: GT_BAD_PARAM.

*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U16                  firstVid;
    GT_U32                  numberOfVids;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with firstVid [100 / 2095] and numberOfVids [50 / 2000].
        Expected: GT_OK. */
        firstVid = 100;
        numberOfVids = 50;

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid,
                                                                    numberOfVids);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, firstVid, numberOfVids);

        firstVid = 2095;
        numberOfVids = 2000; /* total 4095 */

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid,
                                                                    numberOfVids);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, firstVid, numberOfVids);

        /* 1.2. Call function with out of range firstVid [PRV_CPSS_MAX_NUM_VLANS_CNS]
        and numberOfVids the same as in 1.1. Expected: GT_BAD_PARAM. */
        firstVid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid,
                                                                    numberOfVids);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, firstVid=%d", dev, firstVid);

        /* restore */
        firstVid = 100;

        /* 1.3. Call function with out of range numberOfVids [PRV_CPSS_MAX_NUM_VLANS_CNS]
        and firstVid the same as in 1.1. Expected: GT_BAD_PARAM. */
        numberOfVids = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid,
                                                                    numberOfVids);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, numberOfVids=%d",
                                     dev, numberOfVids);

        /* 1.4. Call function with firstVid [3000] and numberOfVids [3000].
        Expected: GT_BAD_PARAM. */
        firstVid = 3000;
        numberOfVids = 3000;

        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid,
                                                                    numberOfVids);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, firstVid, numberOfVids);
    }
    firstVid = 100;
    numberOfVids = 50;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid,
                                                                    numberOfVids);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate(dev, firstVid,
                                                                numberOfVids);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
cpssDxSalBrgVlanEntryWrite
(
    IN GT_U8              devNum,
    IN GT_U32             entryIdx,
    IN CPSS_PORTS_BMP_STC *portsMembersPtr,
    IN CPSS_PORTS_BMP_STC *portsTaggingPtr,
    IN CPSS_VLAN_INFO_STC *vlanInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanEntryWrite)
{
/*
ITERATE_DEVICES(DxSal)
1.1. Call function with entryIdx [10 / 255], non-NULL portsMembersPtr 
[{1, 0}], non-NULL portsTaggingPtr [{3, 0}] and non-NULL vlanInfoPtr 
{hasLocals [GT_TRUE / GT_FALSE]; hasUplinks [GT_TRUE / GT_FALSE]; isCpuMember
 [GT_TRUE / GT_FALSE]; learnEnable [GT_TRUE / GT_FALSE]; unregMcFilterCmd 
 [CPSS_UNREG_MC_VLAN_FRWD_E /  CPSS_UNREG_MC_VLAN_CPU_FRWD_E /  
 CPSS_UNREG_MC_CPU_FRWD_E /  CPSS_UNREG_MC_NOT_CPU_FRWD_E]; 
 perVlanSaMacEnabled[GT_TRUE / GT_FALSE]; saMacSuffix[1] (if 
 perVlanSaMacEnabled = GT_TRUE); stpId[0 / 11]}. Expected: GT_OK.
1.2. Call cpssDxSalBrgVlanEntryRead with the same entryIdx as in 1.1. 
Expected: GT_OK, isValidPtr == GT_TRUE and the same portsMembersPtr, 
portsTaggingPtr and vlanInfoPtr. 
1.3. Call function with out of range entryIdx 
[PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS] and other parameters 
the same as in 1.1. Expected: NON GT_OK.
1.4. Call function with portsMembersPtr [NULL] and other parameters the 
same as in 1.1. Expected: GT_BAD_PTR.
1.5. Call function with portsTaggingPtr [NULL] and other parameters the same 
as in 1.1. Expected: GT_BAD_PTR.
1.6. Call function with out of range vlanInfoPtr->unregMcFilterCmd [0x5AAAAAA5] 
and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
1.7. Call function with vlanInfoPtr-> perVlanSaMacEnabled [GT_TRUE], 
vlanInfoPtr->saMacSuffix [255] (no constraints in the contract) and other 
parameters the same as in 1.1. Expected: GT_OK.
1.8. Call function with out of range vlanInfoPtr ->stpId [12 / 32] and other 
parameters the same as in 1.1. Expected: NON GT_OK.
1.9. Call function with vlanInfoPtr [NULL] and other parameters the same as 
in 1.1. Expected: GT_BAD_PTR.
1.10. Call function with portsMembersPtr [0xFF..F] and other parameters the same as 
in 1.1. Expected: NON GT_OK.
1.11. Call function with portsTaggingPtr [0xFF..F] and other parameters the same as 
in 1.1. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                  entryIdx;
    CPSS_PORTS_BMP_STC      portsMembers;
    CPSS_PORTS_BMP_STC      retPortsMembers;
    CPSS_PORTS_BMP_STC      portsTagging;
    CPSS_PORTS_BMP_STC      retPortsTagging;
    CPSS_VLAN_INFO_STC      vlanInfo;
    CPSS_VLAN_INFO_STC      retVlanInfo;
    GT_BOOL                 isEqual;
    GT_BOOL                 isValid;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with entryIdx [10 / 255], non-NULL portsMembersPtr 
        [{1, 0}], non-NULL portsTaggingPtr [{3, 0}] and non-NULL vlanInfoPtr 
        {hasLocals [GT_TRUE / GT_FALSE]; hasUplinks [GT_TRUE / GT_FALSE]; isCpuMember
        [GT_TRUE / GT_FALSE]; learnEnable [GT_TRUE / GT_FALSE]; unregMcFilterCmd 
        [CPSS_UNREG_MC_VLAN_FRWD_E /  CPSS_UNREG_MC_VLAN_CPU_FRWD_E /  
        CPSS_UNREG_MC_CPU_FRWD_E /  CPSS_UNREG_MC_NOT_CPU_FRWD_E]; 
        perVlanSaMacEnabled[GT_TRUE / GT_FALSE]; saMacSuffix[1] (if 
        perVlanSaMacEnabled = GT_TRUE); stpId[0 / 11]}. Expected: GT_OK.
        1.2. Call cpssDxSalBrgVlanEntryRead with the same entryIdx as in 1.1. 
        Expected: GT_OK, isValidPtr == GT_TRUE and the same portsMembersPtr, 
        portsTaggingPtr and vlanInfoPtr.  */
        entryIdx = 10;
        portsMembers.ports[0] = 1;
        portsMembers.ports[1] = 0;
        portsTagging.ports[0] = 3;
        portsTagging.ports[1] = 0;
        vlanInfo.hasLocals = GT_TRUE;
        vlanInfo.hasUplinks = GT_TRUE;
        vlanInfo.isCpuMember = GT_TRUE;
        vlanInfo.learnEnable = GT_TRUE;
        vlanInfo.unregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;
        vlanInfo.perVlanSaMacEnabled = GT_TRUE;
        vlanInfo.saMacSuffix = 1;
        vlanInfo.stpId = 0;

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers, 
                                        &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &retPortsMembers,
                                       &retPortsTagging, &retVlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgVlanEntryRead: %d, %d",
                                     dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&portsMembers, (GT_VOID*)&retPortsMembers, 
                                     sizeof(portsMembers))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another portsMembers than was set: %d, %d", dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&portsTagging, (GT_VOID*)&retPortsTagging, 
                                     sizeof(portsTagging))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another portsTagging than was set: %d, %d", dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vlanInfo, (GT_VOID*)&retVlanInfo, 
                                     sizeof(vlanInfo))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another vlanInfo than was set: %d, %d", dev, entryIdx);

        /* 1.1., 1.2. for entryIdx [255], hasLocals [GT_FALSE], hasUplinks[GT_FALSE], isCpuMember[GT_FALSE],
        learnEnable[GT_FALSE], unregMcFilterCmd [CPSS_UNREG_MC_VLAN_CPU_FRWD_E],
        perVlanSaMacEnabled [GT_FALSE], stpId [11] */

        entryIdx = 255;
        vlanInfo.hasLocals = GT_FALSE;
        vlanInfo.hasUplinks = GT_FALSE;
        vlanInfo.isCpuMember = GT_FALSE;
        vlanInfo.learnEnable = GT_FALSE;
        vlanInfo.unregMcFilterCmd = CPSS_UNREG_MC_VLAN_CPU_FRWD_E;
        vlanInfo.perVlanSaMacEnabled = GT_FALSE;
        vlanInfo.stpId = 11;

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers, 
                                        &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &retPortsMembers,
                                       &retPortsTagging, &retVlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgVlanEntryRead: %d, %d",
                                     dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&portsMembers, (GT_VOID*)&retPortsMembers, 
                                     sizeof(portsMembers))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another portsMembers than was set: %d, %d", dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&portsTagging, (GT_VOID*)&retPortsTagging, 
                                     sizeof(portsTagging))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another portsTagging than was set: %d, %d", dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vlanInfo, (GT_VOID*)&retVlanInfo, 
                                     sizeof(vlanInfo))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another vlanInfo than was set: %d, %d", dev, entryIdx);

        /* 1.1., 1.2. for unregMcFilterCmd [CPSS_UNREG_MC_CPU_FRWD_E] */
        vlanInfo.unregMcFilterCmd = CPSS_UNREG_MC_CPU_FRWD_E;

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers, 
                                        &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &retPortsMembers,
                                       &retPortsTagging, &retVlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgVlanEntryRead: %d, %d",
                                     dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&portsMembers, (GT_VOID*)&retPortsMembers, 
                                     sizeof(portsMembers))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another portsMembers than was set: %d, %d", dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&portsTagging, (GT_VOID*)&retPortsTagging, 
                                     sizeof(portsTagging))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another portsTagging than was set: %d, %d", dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vlanInfo, (GT_VOID*)&retVlanInfo, 
                                     sizeof(vlanInfo))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another vlanInfo than was set: %d, %d", dev, entryIdx);

        /* 1.1., 1.2. for unregMcFilterCmd [CPSS_UNREG_MC_NOT_CPU_FRWD_E] */
        vlanInfo.unregMcFilterCmd = CPSS_UNREG_MC_NOT_CPU_FRWD_E;

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers, 
                                        &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &retPortsMembers,
                                       &retPortsTagging, &retVlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgVlanEntryRead: %d, %d",
                                     dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&portsMembers, (GT_VOID*)&retPortsMembers, 
                                     sizeof(portsMembers))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another portsMembers than was set: %d, %d", dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&portsTagging, (GT_VOID*)&retPortsTagging, 
                                     sizeof(portsTagging))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another portsTagging than was set: %d, %d", dev, entryIdx);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*)&vlanInfo, (GT_VOID*)&retVlanInfo, 
                                     sizeof(vlanInfo))) ? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual, 
            "got another vlanInfo than was set: %d, %d", dev, entryIdx);

        /* 1.3. Call function with out of range entryIdx 
        [PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS] and other parameters 
        the same as in 1.1. Expected: NON GT_OK. */
        entryIdx = PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS;

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        /* restore */
        entryIdx = 0;

        /* 1.4. Call function with portsMembersPtr [NULL] and other parameters the 
        same as in 1.1. Expected: GT_BAD_PTR. */
        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, NULL,
                                       &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsMembers=NULL", dev);

        /* 1.5. Call function with portsTaggingPtr [NULL] and other parameters the same 
        as in 1.1. Expected: GT_BAD_PTR. */
        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers,
                                       NULL, &vlanInfo);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsTagging=NULL", dev);

        /* 1.6. Call function with out of range vlanInfoPtr->unregMcFilterCmd [0x5AAAAAA5] 
        and other parameters the same as in 1.1. Expected: GT_BAD_PARAM. */
        vlanInfo.unregMcFilterCmd = BRG_VLAN_INVALID_ENUM_CNS;

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
               "%d, vlanInfo.unregMcFilterCmd = %d", dev, vlanInfo.unregMcFilterCmd);

        /* restore */
        vlanInfo.unregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;

        /* 1.7. Call function with vlanInfoPtr-> perVlanSaMacEnabled [GT_TRUE], 
        vlanInfoPtr->saMacSuffix [255] (no constraints in the contract) and other 
        parameters the same as in 1.1. Expected: GT_OK. */
        vlanInfo.perVlanSaMacEnabled = GT_TRUE;
        vlanInfo.saMacSuffix = 255;

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfo.saMacSuffix = %d",
                                     dev, vlanInfo.saMacSuffix);

        /* 1.8. Call function with out of range vlanInfoPtr ->stpId [12 / 32] and other 
        parameters the same as in 1.1. Expected: NON GT_OK. */
        vlanInfo.stpId = 12;

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfo.stpId = %d",
                                         dev, vlanInfo.stpId);

        vlanInfo.stpId = 32;

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, vlanInfo.stpId = %d",
                                         dev, vlanInfo.stpId);

        /* restore */
        vlanInfo.stpId = 0;

        /* 1.9. Call function with vlanInfoPtr [NULL] and other parameters the same as 
        in 1.1. Expected: GT_BAD_PTR. */

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers,
                                       &portsTagging, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vlanInfo=NULL", dev);

        /* 1.10. Call function with portsMembersPtr [0xFF..F] and other parameters the same as 
        in 1.1. Expected: NON GT_OK. */
        portsMembers.ports[0] = 0xFFFFFFFF;
        portsMembers.ports[1] = 0xFFFFFFFF;

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, portMembers{ports[0] = %d, ports[1] = %d}",
                                         dev, portsMembers.ports[0], portsMembers.ports[1]);
        /*restore*/
        portsMembers.ports[0] = 1;
        portsMembers.ports[1] = 0;

        /* 1.11. Call function with portsTaggingPtr [0xFF..F] and other parameters the same as 
        in 1.1. Expected: NON GT_OK. */
        portsTagging.ports[0] = 0xFFFFFFFF;
        portsTagging.ports[1] = 0xFFFFFFFF;

        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, portsTagging{ports[0] = %d, ports[1] = %d}",
                                         dev, portsTagging.ports[0], portsTagging.ports[1]);
        /*restore*/
        portsTagging.ports[0] = 3;
        portsTagging.ports[1] = 0;

    }
    entryIdx = 10;
    portsMembers.ports[0] = 1;
    portsMembers.ports[1] = 0;
    portsTagging.ports[0] = 3;
    portsTagging.ports[1] = 0;
    vlanInfo.hasLocals = GT_TRUE;
    vlanInfo.hasUplinks = GT_TRUE;
    vlanInfo.isCpuMember = GT_TRUE;
    vlanInfo.learnEnable = GT_TRUE;
    vlanInfo.unregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;
    vlanInfo.perVlanSaMacEnabled = GT_TRUE;
    vlanInfo.saMacSuffix = 1;
    vlanInfo.stpId = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanEntryWrite(dev, entryIdx, &portsMembers,
                                   &portsTagging, &vlanInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgVlanEntryRead
(
    IN  GT_U8              devNum,
    IN  GT_U32             entryIdx,
    OUT CPSS_PORTS_BMP_STC *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC *portsTaggingPtr,
    OUT CPSS_VLAN_INFO_STC *vlanInfoPtr,
    OUT GT_BOOL            *isValidPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgVlanEntryRead)
{
/*
ITERATE_DEVICES(DxSal)
1.1. Call function with entryIdx [10 / PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS - 1], non-NULL portsMembersPtr, non-NULL portsTaggingPtr, non-NULL vlanInfoPtr and non-NULL isValidPtr. Expected: GT_OK.
1.2. Call function with out of range entryIdx [PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS] and other parameters the same as in 1.1. Expected: NON GT_OK.
1.3. Call function with portsMembersPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.4. Call function with portsTaggingPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.5. Call function with vlanInfoPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.6. Call function with isValidPtr [NULL] and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U32                  entryIdx;
    CPSS_PORTS_BMP_STC      portsMembers;
    CPSS_PORTS_BMP_STC      portsTagging;
    CPSS_VLAN_INFO_STC      vlanInfo;
    GT_BOOL                 isValid = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with entryIdx [10 /
           PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS - 1],
           non-NULL portsMembersPtr, non-NULL portsTaggingPtr, non-NULL
           vlanInfoPtr and non-NULL isValidPtr. Expected: GT_OK.    */

        entryIdx = 10;

        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);


        entryIdx = PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS - 1;

        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        /* 1.2. Call function with out of range entryIdx
           [PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS]
           and other parameters the same as in 1.1. Expected: NON GT_OK. */

        entryIdx = PRV_CPSS_DXSAL_BRG_INTERNAL_VLAN_ENTRY_NUM_MAX_CNS;

        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo, &isValid);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIdx);

        entryIdx = 10;  /* restore */

        /* 1.3. Call function with portsMembersPtr [NULL] and other
           parameters the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, NULL,
                                       &portsTagging, &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, portsMembersPtr = NULL ", dev);

        /* 1.4. Call function with portsTaggingPtr [NULL] and other
           parameters the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &portsMembers,
                                       NULL, &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, portsTaggingPtr = NULL ", dev);

        /* 1.5. Call function with vlanInfoPtr [NULL] and other
           parameters the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &portsMembers,
                                       &portsTagging, NULL, &isValid);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, vlanInfoPtr = NULL ", dev);

        /* 1.6. Call function with isValidPtr [NULL] and other
           parameters the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, isValidPtr = NULL ", dev);
    }

    entryIdx = 10;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &portsMembers,
                                       &portsTagging, &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgVlanEntryRead(dev, entryIdx, &portsMembers,
                                   &portsTagging, &vlanInfo, &isValid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssDxSalBrgVlan suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalBrgVlan)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanProtoClassEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanProtoClassSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanProtoClassGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanPortProtoVidSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanPortProtoTcSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanLearningStateSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanPortIngFltEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanPortVidGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanPortVidSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanTranslationTableEntriesRangeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanTranslationTableEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanTranslationTableEntriesRangeInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgVlanEntryRead)
UTF_SUIT_END_TESTS_MAC(cpssDxSalBrgVlan)
