/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxBrgFdbUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxBrgFdb.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/bridge/cpssExMxBrgFdb.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* the number of entries in the "mac range filter" table    */
/* defined in cpssExMxBrgFdb.c                              */
/* Be carefull to keep this values synchronized!            */
#define MAC_RANGE_FILTER_TABLE_SIZE_CNS 8


/* Out of range value for virtual port id  - PRV_CPSS_MAX_PP_PORTS_NUM_CNS */

/* Valid value for physical port    */
#define BRG_FDB_VALID_PHY_PORT_CNS           0

/* Valid value for virtual port     */
#define BRG_FDB_VALID_VIRT_PORT_CNS          0

/* Valid value for vlan id          */
#define BRG_FDB_TESTED_VLAN_ID_CNS           100

/* Valid value for vlan mask        */
#define BRG_FDB_TESTED_VLAN_MASK_CNS         0xFFF

/* Invalid enum */
#define BRG_FDB_INVALID_ENUM_CNS             0x5AAAAAA5

/* Valid value for MAC address      */
#define BRG_FDB_TESTED_MAC_ADDR_CNS          {{0x00, 0x1A, 0xFF, 0xFF, 0xFF, 0xFF}}

/* Valid value for MAC mask         */
#define BRG_FDB_TESTED_MAC_MASK_CNS          {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}

/*  Family sets constants */
#define UTF_TWISTD_TIGER_FAMILY_SET_CNS            (UTF_CPSS_PP_FAMILY_TWISTD_CNS | \
                                                   UTF_CPSS_PP_FAMILY_TIGER_CNS)

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbAgingTimeoutSet
(
    IN GT_U8  dev,
    IN GT_U32 timeout
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbAgingTimeoutSet)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U32  timeout;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with valid timeout parameter [timeout = 300]. Expected: GT_OK. */
        timeout = 300;
        st = cpssExMxBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);

        /* 1.2. Call function with out of range timeout parameter. Expected: NON GT_OK. */
        /* 1.2.1. Check bottom boundary [timeout = 0]. */
        timeout = 0;
        st = cpssExMxBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);

        /* 1.2.2. Check upper boundary [timeout = 640]. */
        timeout = 640;
        st = cpssExMxBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, timeout);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM.*/

    timeout = 256;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbAgingTimeoutSet(dev, timeout);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, timeout);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* timeout == 256 */

    st = cpssExMxBrgFdbAgingTimeoutSet(dev, timeout);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, timeout);
}

/*----------------------------------------------------------------------------*/
/*GT_STATUS cpssExMxBrgFdbMacAddrEntrySet
(
    IN GT_U8                dev,
    IN CPSS_MAC_ENTRY_STC  *entryPtr
)*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacAddrEntrySet)
{
/*
ITERATE_DEVICES
1.1. Call function with valid entryPtr. Expected: GT_OK.
1.2. #if 0 (the caller need to implement get AU message)
Call cpssExMxBrgFdbQaSend with non-null entryPtr with macAddr [00:1A:FF:FF:FF:FF] and vlanId [100]. Expected: GT_OK and entryWasFound [GT_TRUE] macEntry the same as input was:
check next fields of macEntry: isStatic, vlanId, srcTc, dstTc, daCommand, saCommand, saClass, daClass, saCib, daCib, daRoute, dstInterface.type and
if entry.dstInterface.type= CPSS_INTERFACE_PORT_E check dstInterface.devPort.devNum, dstInterface.devPort.portNum;
if entry.dstInterface.type= CPSS_INTERFACE_TRUNK_E check dstInterface.trunkId;
if entry.dstInterface.type= CPSS_INTERFACE_VIDX_E check dstInterface.vidx;
if entry.dstInterface.type= CPSS_INTERFACE_VID_E check dstInterface.vid.
#endif
1.3. Call function with entryPtr = NULL. Expected: GT_BAD_PTR.
1.4. Call function with entry.dstInterface.type = CPSS_INTERFACE_VID_E. Expected: NON GT_OK (because for mac entry dstInterface - destination interface : port/trunk/vidx).
1.5. Call function with of range entryPtr->dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS],entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E]. Expected: NON GT_OK.
1.6. Call function with of range entryPtr->dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E]. Expected: NON GT_OK.
1.7. Call function with of range entryPtr->dstInterface.trunkId [128],entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E]. Expected: NON GT_OK.
1.8. Call function with of range for entryPtr->dstInterface.vidx [0xFFFF],entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E]. Expected: NON GT_OK.
1.9. Call function with of range for entryPtr->srcTc  [8]. Expected: NON GT_OK.
1.10. Call function with of range for entryPtr->dstTc  [8]. Expected: NON GT_OK.
1.11. Call function with of range for entryPtr->daCommand [0x5AAAAAA5]. Expected: NON GT_OK.
1.12. Call function with not supported for ExMx entryPtr->daCommand [GT_MAC_TABLE_MIRROR_TO_CPU/ CPSS_MAC_TABLE_SOFT_DROP_E]. Expected: NON GT_OK.
1.13. Call function with of range for entryPtr->saCommand [0x5AAAAAA5]. Expected: NON GT_OK.
1.14. Call function with not supported for ExMx entryPtr->saCommand [GT_MAC_TABLE_MIRROR_TO_CPU/ CPSS_MAC_TABLE_SOFT_DROP_E]. Expected: NON GT_OK.
*/

    GT_STATUS st = GT_OK;

    GT_U8               dev;
    CPSS_MAC_ENTRY_STC  entry;

    GT_ETHERADDR        validMac = BRG_FDB_TESTED_MAC_ADDR_CNS;

    cpssOsBzero((GT_VOID*)&entry, sizeof(entry));
    cpssOsMemCpy((GT_VOID*)&(entry.macAddr), (const GT_VOID*)&validMac, sizeof(validMac));
    entry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    entry.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
    entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
    entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
    entry.dstInterface.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
    /* entry.dstInterface.devPort.devNum == 0       */
    /* entry.dstInterface.devPort.portNum == 0      */
    /* entry.dstInterface.trunkId == 0              */
    /* entry.dstInterface.vidx == 0                 */
    /* entry.isStatic == GT_FALSE                   */
    /* entry.srcTc == 0                             */
    /* entry.dstTc == 0                             */
    /* entry.daCib == GT_FALSE                      */
    /* entry.daClass == GT_FALSE                    */
    /* entry.saCib == GT_FALSE                      */
    /* entry.saClass == GT_FALSE                    */
    /* entry.daRoute == GT_FALSE                    */
    /* entry.cosIpv4En == GT_FALSE                  */
    /* entry.mirrorToRxAnalyzerPortEn == GT_FALSE   */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with valid entryPtr. Expected: GT_OK. */
        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

#if 0  /* the caller need to implement get AU message */

        /* 1.2. Call cpssExMxBrgFdbQaSend with non-null      */
        /* entryPtr with macAddr [00:1A:FF:FF:FF:FF] and vlanId [100].  */
        /* Expected: GT_OK and entryWasFound [GT_TRUE]                  */
        /* macEntry the same as input was.                              */

        CPSS_MAC_UPDATE_MSG_STC     retEntryMsg;
        GT_BOOL                     cmpRes;
        CPSS_EXMX_ADDR_UP_MSG_STC   auMsgs[1];
        GT_U32                      numOfAuMsgs;


        /* retEntryMsg.macEntry.vlanId = entry.vlanId; */

        st = cpssExMxBrgFdbQaSend(dev,entry.vlanId, &entry.macAddr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssExMxBrgFdbQaSend: %d",
                                     dev);
        if (GT_OK == st)
        {
            numOfAuMsgs = 1;

            st = cpssExMxBrgFdbAuMsgBlockGet(dev, &numOfAuMsgs,auMsgs);
            if((GT_OK == st) && (1 == numOfAuMsgs))
            {
                retEntryMsg = auMsgs[0].messageData.macUpdate;

                /* check if entryWasFound = GT_TRUE */
                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, retEntryMsg.entryWasFound,
                                            "cpssExMxBrgFdbMacAddrHwEntryGet: entry wasn't found [dev = %d]",
                                            dev);

                /* check if returned entry equal to set */
                /* cmpRes = cpssOsMemCmp((const GT_VOID*)&entry.macAddr,
                                      (const GT_VOID*)&(retEntryMsg.macEntry.macAddr),
                                       sizeof(entry.macAddr)); */
                cmpRes = GT_TRUE;

                if(retEntryMsg.macEntry.dstInterface.type != entry.dstInterface.type)
                    cmpRes = GT_FALSE;

                switch(entry.dstInterface.type)
                {
                case CPSS_INTERFACE_PORT_E:
                    if((retEntryMsg.macEntry.dstInterface.devPort.devNum !=
                                               entry.dstInterface.devPort.devNum) &&
                       (retEntryMsg.macEntry.dstInterface.devPort.portNum !=
                                               entry.dstInterface.devPort.portNum))
                        cmpRes = GT_FALSE;
                    break;
                case CPSS_INTERFACE_TRUNK_E:
                    if(retEntryMsg.macEntry.dstInterface.trunkId !=
                                               entry.dstInterface.trunkId)
                        cmpRes = GT_FALSE;
                    break;
                case CPSS_INTERFACE_VIDX_E:
                    if(retEntryMsg.macEntry.dstInterface.vidx !=
                                               entry.dstInterface.vidx)
                        cmpRes = GT_FALSE;
                    break;
                default:
                    cmpRes = GT_FALSE;
                }

                if(retEntryMsg.macEntry.isStatic != entry.isStatic)
                    cmpRes = GT_FALSE;
                if(retEntryMsg.macEntry.vlanId != entry.vlanId)
                    cmpRes = GT_FALSE;
                if(retEntryMsg.macEntry.srcTc != entry.srcTc)
                    cmpRes = GT_FALSE;
                if(retEntryMsg.macEntry.dstTc != entry.dstTc)
                    cmpRes = GT_FALSE;
                if(retEntryMsg.macEntry.daCommand != entry.daCommand)
                    cmpRes = GT_FALSE;
                if(retEntryMsg.macEntry.saCommand != entry.saCommand)
                    cmpRes = GT_FALSE;
                if(retEntryMsg.macEntry.saClass != entry.saClass)
                    cmpRes = GT_FALSE;
                if(retEntryMsg.macEntry.daClass != entry.daClass)
                    cmpRes = GT_FALSE;
                if(retEntryMsg.macEntry.saCib != entry.saCib)
                    cmpRes = GT_FALSE;
                if(retEntryMsg.macEntry.daCib != entry.daCib)
                    cmpRes = GT_FALSE;
                if(retEntryMsg.macEntry.daRoute != entry.daRoute)
                    cmpRes = GT_FALSE;
                /* these parameters are not relevant in EX */
                /*
                GT_BOOL                 cosIpv4En;
                GT_BOOL                 mirrorToRxAnalyzerPortEn */

                UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, cmpRes,
                                            "cpssExMxBrgFdbMacAddrHwEntryGet: get another macEntry than was written [dev = %d]",
                                            dev);

            }
        }
#endif /* the caller need to implement get AU message */

        /* 1.3. Call function with entryPtr = NULL. Expected: GT_BAD_PTR. */
        st = cpssExMxBrgFdbMacAddrEntrySet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);

        /* 1.4. Call function with entry.dstInterface.type = CPSS_INTERFACE_VID_E.
        Expected: NON GT_OK. */
        entry.dstInterface.type = CPSS_INTERFACE_VID_E;
        entry.dstInterface.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, entry.dstInterface.type = %d", dev, entry.dstInterface.type);

        entry.dstInterface.vlanId = 0; /* restore */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E; /* restore */

        /* 1.5. Call function with of range for vlanId as part of entryPtr. */
        /* Expected: GT_BAD_PARAM.                                             */
        entry.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
            "%d, entry.vlanId = %d", dev, entry.vlanId);

        entry.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS; /* restore */

        /* 1.6. Call function with of range
        entryPtr->dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
        entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E].
        Expected: NON GT_OK. */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;
        entry.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, entry.dstInterface [type = %d, devPort.devNum = %d]",
            dev, entry.dstInterface.type, entry.dstInterface.devPort.devNum);

        entry.dstInterface.devPort.devNum = 0; /* restore */

        /* 1.7. Call function with of range
        entryPtr->dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
        entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E].
        Expected: NON GT_OK. */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;
        entry.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, entry.dstInterface [type = %d, devPort.portNum = %d]",
            dev, entry.dstInterface.type, entry.dstInterface.devPort.portNum);

        entry.dstInterface.devPort.portNum = 0; /* restore */

        /* 1.8. Call function with of range
        entryPtr->dstInterface.trunkId [128],
        entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E].
        Expected: NON GT_OK. */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;
        entry.dstInterface.trunkId = 128;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, entry.dstInterface [type = %d, trunkId = %d]",
            dev, entry.dstInterface.type, entry.dstInterface.trunkId);

        entry.dstInterface.trunkId = 0; /* restore */

        /* 1.9. Call function with of range for
        entryPtr->dstInterface.vidx [0xFFFF],
        entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E].
        Expected: NON GT_OK. */
        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        entry.dstInterface.vidx = 0xFFFF;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, entry.dstInterface [type = %d, vidx = %d]",
            dev, entry.dstInterface.type, entry.dstInterface.vidx);

        entry.dstInterface.vidx = 0; /* restore */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E; /* restore */

        /* 1.10. Call function with of range for entryPtr->srcTc [8].
        Expected: NON GT_OK. */
        entry.srcTc = CPSS_TC_RANGE_CNS;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, entry.srcTc = %d", dev, entry.srcTc);

        entry.srcTc = 0; /* restore */

        /* 1.11. Call function with of range for entryPtr->dstTc [8].
        Expected: NON GT_OK. */
        entry.dstTc = CPSS_TC_RANGE_CNS;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, entry.dstTc = %d", dev, entry.dstTc);

        entry.dstTc = 0; /* restore */

        /* 1.12. Call function with of range for entryPtr->daCommand [0x5AAAAAA5].
        Expected: NON GT_OK. */
        entry.daCommand = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, entry.daCommand = %d", dev, entry.daCommand);

        /* 1.13. Call function with not supported for ExMx
        entryPtr->daCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E/ CPSS_MAC_TABLE_SOFT_DROP_E].
        Expected: NON GT_OK. */
        entry.daCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, entry.daCommand = %d", dev, entry.daCommand);

        entry.daCommand = CPSS_MAC_TABLE_SOFT_DROP_E;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, entry.daCommand = %d", dev, entry.daCommand);

        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E; /* restore */

        /* 1.14. Call function with of range for
        entryPtr->saCommand [0x5AAAAAA5]. Expected: NON GT_OK. */
        entry.saCommand = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, entry.saCommand = %d", dev, entry.saCommand);

        /* 1.15. Call function with not supported for ExMx
        entryPtr->saCommand [GT_MAC_TABLE_MIRROR_TO_CPU/ CPSS_MAC_TABLE_SOFT_DROP_E].
        Expected: NON GT_OK. */
        entry.saCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, entry.saCommand = %d", dev, entry.saCommand);

        entry.saCommand = CPSS_MAC_TABLE_SOFT_DROP_E;

        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, entry.saCommand = %d", dev, entry.saCommand);

        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E; /* restore */
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMacAddrEntrySet(dev, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacAddrEntryDel
(
    IN GT_U8                dev,
    IN CPSS_MAC_ENTRY_STC  *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacAddrEntryDel)
{
    GT_STATUS st = GT_OK;

    GT_U8               dev;
    CPSS_MAC_ENTRY_STC  entry;

    GT_ETHERADDR        validMac = BRG_FDB_TESTED_MAC_ADDR_CNS;

    /* zero out the entry */
    cpssOsBzero((GT_VOID*)&entry, sizeof(entry));
    /* set valid mac */
    cpssOsMemCpy((GT_VOID*)&(entry.macAddr), (const GT_VOID*)&validMac, sizeof(validMac));
    entry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    entry.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
    entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
    entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
    /* entry.dstInterface.devPort.devNum = 0;
    entry.dstInterface.devPort.portNum = 0;
    entry.dstInterface.trunkId = 0;
    entry.dstInterface.vidx = 0;
    entry.isStatic = GT_FALSE;
    entry.srcTc = 0;
    entry.dstTc = 0;
    entry.daCib = GT_FALSE;
    entry.daClass = GT_FALSE;
    entry.saCib = GT_FALSE;
    entry.saClass = GT_FALSE;
    entry.daRoute = GT_FALSE;
    entry.cosIpv4En = GT_FALSE;
    entry.mirrorToRxAnalyzerPortEn = GT_FALSE;
    */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with valid entryPtr. Expected: GT_OK. */
        entry.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with entryPtr = NULL. Expected: GT_BAD_PTR. */
        st = cpssExMxBrgFdbMacAddrEntryDel(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);

        /* 1.3. Call function with invalid values for fields of entryPtr. */
        /* 1.3.1. Check entryPtr->vlanId out of range. Expected: GT_BAD_PARAM.*/
        entry.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, <entryPtr->vlanId> = %d", dev, entry.vlanId);
    }

    cpssOsBzero((GT_VOID*)&entry, sizeof(entry));
    cpssOsMemCpy((GT_VOID*)&(entry.macAddr), (const GT_VOID*)&validMac, sizeof(validMac));
    entry.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMacAddrEntryDel(dev, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*GT_STATUS cpssExMxBrgFdbMacTriggerModeSet
(
    IN GT_U8                dev,
    IN CPSS_MAC_ACTION_MODE_ENT   mode,
    IN GT_U16               vlanId,
    IN GT_U16               vlanMask
);
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacTriggerModeSet)
{
    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    CPSS_MAC_ACTION_MODE_ENT    mode;
    GT_U16                      vlanId;
    GT_U16                      vlanMask;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with valid values for vlanId                   */
        /* [BRG_FDB_TESTED_VLAN_ID_CNS]                                      */
        /* and vlanMask[BRG_FDB_TESTED_VLAN_MASK_CNS] for both cases of mode */
        vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
        vlanMask = BRG_FDB_TESTED_VLAN_MASK_CNS;
        mode = CPSS_ACT_AUTO_E;

        st = cpssExMxBrgFdbMacTriggerModeSet(dev, mode, vlanId, vlanMask);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, mode, vlanId, vlanMask);

        /* vlanId == BRG_FDB_TESTED_VLAN_ID_CNS          */
        /* vlanMask == BRG_FDB_TESTED_VLAN_MASK_CNS      */
        mode = CPSS_ACT_TRIG_E;
        st = cpssExMxBrgFdbMacTriggerModeSet(dev, mode, vlanId, vlanMask);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, mode, vlanId, vlanMask);

        /* 1.2. Call function with out of range vlanId. Expected: GT_BAD_PARAM. */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        /* vlanMask == BRG_FDB_TESTED_VLAN_MASK_CNS  */
        /* mode = CPSS_ACT_TRIG_E                    */
        st = cpssExMxBrgFdbMacTriggerModeSet(dev, mode, vlanId, vlanMask);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, mode, vlanId, vlanMask);

        /* 1.3 Check for invalid mode [BRG_FDB_INVALID_ENUM_CNS]. Expected: GT_BAD_PARAM. */
        vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
        /* vlanMask == BRG_FDB_TESTED_VLAN_MASK_CNS       */

        st = cpssExMxBrgFdbMacTriggerModeSet(dev, BRG_FDB_INVALID_ENUM_CNS, vlanId, vlanMask);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st,
                                    dev, BRG_FDB_INVALID_ENUM_CNS, vlanId, vlanMask);
     }

    /* 2. For not active devices check that function returns GT_BAD_PARAM.*/

    vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
    vlanMask = BRG_FDB_TESTED_VLAN_MASK_CNS;
    mode = CPSS_ACT_AUTO_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacTriggerModeSet(dev, mode, vlanId, vlanMask);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, mode, vlanId, vlanMask);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /*  vlanId == BRG_FDB_TESTED_VLAN_ID_CNS         */
    /* vlanMask == BRG_FDB_TESTED_VLAN_MASK_CNS      */
    /* mode == CPSS_ACT_AUTO_E                       */

    st = cpssExMxBrgFdbMacTriggerModeSet(dev, mode, vlanId, vlanMask);
    UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, mode, vlanId, vlanMask);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacTriggerToggle
(
    IN GT_U8  dev
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacTriggerToggle)
{
    GT_STATUS st = GT_OK;

    GT_U8 dev;

    CPSS_MAC_ACTION_MODE_ENT    mode = CPSS_ACT_TRIG_E;
    GT_U16                      vlanId = 0;
    GT_U16                      vlanMask = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function cpssExMxBrgFdbMacTriggerModeSet
        vlanId [0] and vlanMask [0] mode [CPSS_ACT_TRIG_E].
        Expect: GT_OK. */
        st = cpssExMxBrgFdbMacTriggerModeSet(dev, mode, vlanId, vlanMask);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
            "cpssExMxBrgFdbMacTriggerModeSet: %d, %d, %d, %d", dev, mode, vlanId, vlanMask);

        /* 1.2. Call function. Must return GT_OK. */
        st = cpssExMxBrgFdbMacTriggerToggle(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacTriggerToggle(dev);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMacTriggerToggle(dev);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbStaticTransEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbStaticTransEnable)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                           */
        enable = GT_TRUE;
        st = cpssExMxBrgFdbStaticTransEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;
        st = cpssExMxBrgFdbStaticTransEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbStaticTransEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    enable = GT_FALSE;

    st = cpssExMxBrgFdbStaticTransEnable(dev, enable);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbStaticDelEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbStaticDelEnable)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                           */
        enable = GT_TRUE;
        st = cpssExMxBrgFdbStaticDelEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;
        st = cpssExMxBrgFdbStaticDelEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbStaticDelEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    enable = GT_FALSE;

    st = cpssExMxBrgFdbStaticDelEnable(dev, enable);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbAgeWithoutRemovalEnable
(
    IN GT_U8    dev,
    IN GT_BOOL  enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbAgeWithoutRemovalEnable)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                           */
        enable = GT_TRUE;
        st = cpssExMxBrgFdbAgeWithoutRemovalEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;
        st = cpssExMxBrgFdbAgeWithoutRemovalEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbAgeWithoutRemovalEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    enable = GT_FALSE;

    st = cpssExMxBrgFdbAgeWithoutRemovalEnable(dev, enable);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacVlanLookupModeSet
(
    IN GT_U8                dev,
    IN CPSS_MAC_VL_ENT      mode (CPSS_IVL_E = 1, CPSS_SVL_E)
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacVlanLookupModeSet)
{
    GT_STATUS st = GT_OK;

    GT_U8           dev;
    CPSS_MAC_VL_ENT mode;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    mode = CPSS_IVL_E;
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with both valid values of mode.*/
        /* mode == CPSS_IVL_E */
        st = cpssExMxBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /* 1.2. Check for invalid mode. Expected: GT_BAD_PARAM. */
        st = cpssExMxBrgFdbMacVlanLookupModeSet(dev, BRG_FDB_INVALID_ENUM_CNS);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st,
                                    dev, BRG_FDB_INVALID_ENUM_CNS);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    mode = CPSS_SVL_E;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
    }

    /* 3. Call function with out of bound value for device id. */

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* mode == CPSS_SVL_E */

    st = cpssExMxBrgFdbMacVlanLookupModeSet(dev, mode);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacAddrLookupModeSet
(
    IN  GT_U8                 dev,
    IN CPSS_ADDR_LOOKUP_MODE_ENT   addrMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacAddrLookupModeSet)
{
    GT_STATUS st = GT_OK;

    GT_U8           dev;
    CPSS_MAC_VL_ENT addrMode;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */

    addrMode = CPSS_MAC_SQN_VLAN_SQN_E;

    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with valid value for addrMode.    */
        /* Must return GT_OK.                                       */
        /* addrMode == CPSS_MAC_SQN_VLAN_SQN_E */
        st = cpssExMxBrgFdbMacAddrLookupModeSet(dev, addrMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, addrMode);

        /*1.2.  Check for invalid addrMode. Expected: GT_BAD_PARAM.*/
        st = cpssExMxBrgFdbMacAddrLookupModeSet(dev,BRG_FDB_INVALID_ENUM_CNS);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st,
                                    dev, BRG_FDB_INVALID_ENUM_CNS);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    addrMode = CPSS_MAC_RND_VLAN_SQN_E;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacAddrLookupModeSet(dev, addrMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, addrMode);
    }

    /* 3. Call function with out of bound value for device id. */

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* addrMode == CPSS_MAC_RND_VLAN_SQN_E */

    st = cpssExMxBrgFdbMacAddrLookupModeSet(dev, addrMode);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, addrMode);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbPortLearnSet
(
    IN GT_U8            dev,
    IN GT_U8            port,
    IN GT_BOOL          status,
    IN CPSS_PORT_LOCK_CMD_ENT cmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbPortLearnSet)
{
    GT_STATUS   st = GT_OK;

    GT_U8                   dev;
    GT_U8                   port;
    GT_BOOL                 status;
    CPSS_PORT_LOCK_CMD_ENT  cmd;
    GT_BOOL                 retStatus;
    CPSS_PORT_LOCK_CMD_ENT  retCmd;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available Physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function for each port               */
            /* with status GT_TRUE and GT_FALSE                 */
            /* 1.1.2. Call cpssExMxBrgFdbPortLearnGet and check             */
            /* if returned values of status and cmd equal to set values.    */
            cmd = CPSS_LOCK_FRWRD_E;
            status = GT_TRUE;

            st = cpssExMxBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, status, cmd);

            st = cpssExMxBrgFdbPortLearnGet(dev, port, &retStatus, &retCmd);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "cpssExMxBrgFdbPortLearnGet [dev = %d, port = %d]",
                                         dev, port);
            if (GT_OK == st)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(status, retStatus,
                    "get another status than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(cmd, retCmd,
                    "get another cmd than was set: %d, %d", dev, port);
            }

            cmd = CPSS_LOCK_TRAP_E;
            status = GT_FALSE;

            st = cpssExMxBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st,
                                        dev, port, status, cmd);
            st = cpssExMxBrgFdbPortLearnGet(dev, port, &retStatus, &retCmd);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                         "cpssExMxBrgFdbPortLearnGet: %d, %d",
                                         dev, port);
            if (GT_OK == st)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(status, retStatus,
                    "get another status than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(cmd, retCmd,
                    "get another cmd than was set: %d, %d", dev, port);
            }
        }

        cmd = CPSS_LOCK_DROP_E;

        /* prepare Physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Check for status = GT_TRUE and status = GT_FALSE.    */
            status = GT_TRUE;
            st = cpssExMxBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st,
                                        dev, port, status, cmd);

            status = GT_FALSE;
            st = cpssExMxBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st,
                                        dev, port, status, cmd);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        status = GT_TRUE;

        /* cmd == CPSS_LOCK_DROP_E */
        st = cpssExMxBrgFdbPortLearnSet(dev, port, status, cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status, cmd);

        /* 1.4. Call function for valid port with cmd = CPSS_LOCK_MIRROR_E  */
        /* [status= GT_FALSE]. Must return NON GT_OK.                       */
        /* CPSS_LOCK_MIRROR_E (Forward and Mirror to CPU)                   */
        /* Applicable to Dx devices. Since we are working with ExMx         */
        /* NON GT_OK expected.                                              */
        cmd = CPSS_LOCK_MIRROR_E;
        port = BRG_FDB_VALID_PHY_PORT_CNS;
        status = GT_FALSE;
        st = cpssExMxBrgFdbPortLearnSet(dev, port, status, cmd);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, status, cmd);

        /* 1.5. Check for invalid cmd [BRG_FDB_INVALID_ENUM_CNS].   */
        /* Expected: GT_BAD_PARAM.                                  */
        port = BRG_FDB_VALID_PHY_PORT_CNS;
        status = GT_FALSE;
        cmd = BRG_FDB_INVALID_ENUM_CNS;
        st = cpssExMxBrgFdbPortLearnSet(dev, port, status, cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st,
                                    dev, port, status, cmd);

        /* 1.6. Call with cmd [CPSS_LOCK_SOFT_DROP_E] (applicable for DxCh only) */
        /* and status [GT_FALSE]. Expected: NON GT_OK.                           */
        port = BRG_FDB_VALID_PHY_PORT_CNS;
        cmd = CPSS_LOCK_SOFT_DROP_E;
        status = GT_FALSE;
        st = cpssExMxBrgFdbPortLearnSet(dev, port, status, cmd);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, status, cmd);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_FDB_VALID_PHY_PORT_CNS;
    status = GT_TRUE;
    cmd = CPSS_LOCK_FRWRD_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbPortLearnSet(dev, port, status, cmd);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status, cmd);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == BRG_FDB_VALID_PHY_PORT_CNS   */
    /* status == GT_TRUE                     */
    /* cmd == CPSS_LOCK_FRWRD_E              */

    st = cpssExMxBrgFdbPortLearnSet(dev, port, status, cmd);
    UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev ,port, status, cmd);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbPortLearnGet
(
    IN  GT_U8   dev,
    IN  GT_U8   port,
    OUT GT_BOOL *statusPtr,
    OUT CPSS_PORT_LOCK_CMD_ENT *cmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbPortLearnGet)
{
    GT_STATUS   st = GT_OK;

    GT_U8                   dev;
    GT_U8                   port;
    GT_BOOL                 status;
    CPSS_PORT_LOCK_CMD_ENT  cmd;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare Physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available Physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function for each port with valid    */
            /* statusPtr and valid cmdPtr. Must return GT_OK.   */
            st = cpssExMxBrgFdbPortLearnGet(dev, port, &status, &cmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                         "[dev = %d, port =%d, status = %d, cmd =%d]",
                                         dev, port, status, cmd);

            /* 1.1.2. Call function for each port with statusPtr = NULL. */
            /* Must return GT_BAD_PTR.                                   */
            st = cpssExMxBrgFdbPortLearnGet(dev, port, NULL, &cmd);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                         "[&status = NULL, dev = %d, port =%d]",
                                         dev, port);


            /* 1.1.3. Call function for each port with cmdPtr = NULL. */
            /* Must return GT_BAD_PTR.                                */
            st = cpssExMxBrgFdbPortLearnGet(dev, port, &status, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                         "[&cmd = NULL, dev = %d, port =%d]",
                                         dev, port);
        }

        /* 1.2. Go over non configured ports */

        /* prepare Physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            st = cpssExMxBrgFdbPortLearnGet(dev, port, &status, &cmd);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st,
                                         "[dev = %d, port =%d, status = %d, cmd =%d]",
                                         dev, port, status, cmd);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        st = cpssExMxBrgFdbPortLearnGet(dev, port, &status, &cmd);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st,
                                     "[dev = %d, port =%d, status = %d, cmd =%d]",
                                     dev, port, status, cmd);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbPortLearnGet(dev, port, &status, &cmd);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st,
                                     "[dev = %d, port =%d, status = %d, cmd =%d]",
                                     dev, port, status, cmd);

    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == BRG_FDB_VALID_PHY_PORT_CNS  */
    st = cpssExMxBrgFdbPortLearnGet(dev, port, &status, &cmd);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st,
                                 "[dev = %d, port =%d, status = %d, cmd =%d]",
                                 dev, port, status, cmd);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacFilterPortEnable
(
    IN GT_U8   dev,
    IN GT_U8   port,
    IN GT_BOOL status
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacFilterPortEnable)
{
    GT_STATUS   st = GT_OK;

    GT_U8       dev;
    GT_U8       port;
    GT_BOOL     status;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare Physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available Physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function for each port               */
            /* with status GT_TRUE and GT_FALSE                 */
            status = GT_TRUE;
            st = cpssExMxBrgFdbMacFilterPortEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, status);

            status = GT_FALSE;
            st = cpssExMxBrgFdbMacFilterPortEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, status);
        }

        /* prepare Physical port iterator */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            /* Check for status = GT_TRUE and status = GT_FALSE.    */
            status = GT_TRUE;
            st = cpssExMxBrgFdbMacFilterPortEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);

            status = GT_FALSE;
            st = cpssExMxBrgFdbMacFilterPortEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        status = GT_TRUE;
        st = cpssExMxBrgFdbMacFilterPortEnable(dev, port, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = BRG_FDB_VALID_PHY_PORT_CNS;
    status = GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacFilterPortEnable(dev, port, status);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == BRG_FDB_VALID_PHY_PORT_CNS   */
    /* status == GT_TRUE                     */

    st = cpssExMxBrgFdbMacFilterPortEnable(dev, port, status);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev ,port, status);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbNaToCpuEnable
(
    IN GT_U8   dev,
    IN GT_BOOL status
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbNaToCpuEnable)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL status;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with status = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                           */
        status = GT_TRUE;
        st = cpssExMxBrgFdbNaToCpuEnable(dev, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, status);

        status = GT_FALSE;
        st = cpssExMxBrgFdbNaToCpuEnable(dev, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, status);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    status = GT_TRUE;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbNaToCpuEnable(dev, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, status);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    status = GT_FALSE;

    st = cpssExMxBrgFdbNaToCpuEnable(dev, status);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, status);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacUpd2CpuEnable
(
    IN GT_U8   dev,
    IN GT_BOOL status
)
*/

UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacUpd2CpuEnable)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL status;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with status = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                           */
        status = GT_TRUE;
        st = cpssExMxBrgFdbMacUpd2CpuEnable(dev, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, status);

        status = GT_FALSE;
        st = cpssExMxBrgFdbMacUpd2CpuEnable(dev, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, status);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    status = GT_TRUE;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacUpd2CpuEnable(dev, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, status);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    status = GT_FALSE;

    st = cpssExMxBrgFdbMacUpd2CpuEnable(dev, status);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, status);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbAuToCpuSet
(
    IN GT_U8                dev,
    IN CPSS_MAC_AU2CPU_TYPE_ENT   auMsgType,
    IN GT_BOOL              enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbAuToCpuSet)
{
    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    CPSS_MAC_AU2CPU_TYPE_ENT    auMsgType;
    GT_BOOL                     enable;

    GT_BOOL                     retEnable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);


    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                           */

        /* 1.2. Call cpssExMxBrgFdbAuToCpuGet.                          */
        /* Expected: GT_OK and the same output parameters as input was. */

        auMsgType = CPSS_MAC_AU_MESSAGE_QA_E;
        enable = GT_TRUE;

        st = cpssExMxBrgFdbAuToCpuSet(dev, auMsgType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, auMsgType, enable);

        /* no meaning in "get all" status !! */
        st = cpssExMxBrgFdbAuToCpuGet(dev,auMsgType,&retEnable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "cpssExMxBrgFdbAuToCpuGet: %d, %d",
                                     dev, auMsgType);
        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(enable, retEnable,
                                         "set enable = %d, but get %d [dev = %d, auMsgType =%d]",
                                         enable, retEnable, dev, auMsgType);
        }

        auMsgType = CPSS_MAC_AU_MESSAGE_TA_AA_E;/*parameter that valid to all ExMx device*/
        enable = GT_FALSE;

        st = cpssExMxBrgFdbAuToCpuSet(dev, auMsgType, enable );
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, auMsgType, enable);

        st = cpssExMxBrgFdbAuToCpuGet(dev,auMsgType,&retEnable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "cpssExMxBrgFdbAuToCpuGet: %d, %d",
                                     dev, auMsgType);
        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(enable, retEnable,
                                         "set enable = %d, but get %d [dev = %d, auMsgType =%d]",
                                         enable, retEnable, dev, auMsgType);
        }

        /* 1.3. Check for invalid auMsgType [0x5AAAAAA5].   */
        /* Expected: GT_BAD_PARAM.                          */
        /* enable == GT_FALSE */
        st = cpssExMxBrgFdbAuToCpuSet(dev, BRG_FDB_INVALID_ENUM_CNS, enable );
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st,
                                    dev, BRG_FDB_INVALID_ENUM_CNS, enable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    auMsgType = CPSS_MAC_AU_MESSAGE_TA_AA_E;
    enable = GT_TRUE;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbAuToCpuSet(dev, auMsgType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, auMsgType, enable);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* auMsgType == CPSS_MAC_AU_MESSAGE_TA_AA_E */
    /* enable = GT_TRUE                         */

    st = cpssExMxBrgFdbAuToCpuSet(dev, auMsgType, enable);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, auMsgType, enable);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbAuToCpuGet
(
    IN GT_U8                dev,
    IN CPSS_MAC_AU2CPU_TYPE_ENT   auMsgType,
    OUT GT_BOOL              *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbAuToCpuGet)
{
    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    CPSS_MAC_AU2CPU_TYPE_ENT    auMsgType;
    GT_BOOL                     enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    auMsgType = CPSS_MAC_AU_MESSAGE_TA_AA_E;

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with valid auMsgType [CPSS_MAC_AU_MESSAGE_TA_AA_E]   */
        /* and non-null enablePtr. Expected: GT_OK                        */

        /* auMsgType == CPSS_MAC_AU_MESSAGE_TA_AA_E */
        st = cpssExMxBrgFdbAuToCpuGet(dev, auMsgType, &enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d", dev, auMsgType);

        /* 1.2 Call with enablePtr = NULL. Expected: GT_BAD_PTR. */
        st = cpssExMxBrgFdbAuToCpuGet(dev, auMsgType, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, auMsgType);


        /* 1.3. Check for invalid auMsgType [0x5AAAAAA5].   */
        /* Expected: GT_BAD_PARAM.                          */
        st = cpssExMxBrgFdbAuToCpuGet(dev, BRG_FDB_INVALID_ENUM_CNS, &enable );
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, %d", dev, auMsgType);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbAuToCpuGet(dev, auMsgType, &enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, %d", dev, auMsgType);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* auMsgType == CPSS_MAC_AU_MESSAGE_SA_LEARNING_E */

    st = cpssExMxBrgFdbAuToCpuGet(dev, auMsgType, &enable);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, %d", dev, auMsgType);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbFlushPortEmulate
(
    IN GT_U8    dev,
    IN GT_U8    portDev,
    IN GT_U8    port,
    IN GT_U16   vlanId,
    IN GT_U16   vlanMask
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbFlushPortEmulate)
{
    GT_STATUS st = GT_OK;

    GT_U8    dev;
    GT_U8    portDev;
    GT_U8    port;
    GT_U16   vlanId;
    GT_U16   vlanMask = BRG_FDB_TESTED_VLAN_MASK_CNS;

    CPSS_MAC_AU2CPU_TYPE_ENT    auMsgType = CPSS_MAC_AU_MESSAGE_TA_AA_E;
    GT_BOOL                     auMsgEnable;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        auMsgEnable = GT_FALSE;
        st = cpssExMxBrgFdbAuToCpuSet(dev, auMsgType, auMsgEnable);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                    "cpssExMxBrgFdbAuToCpuSet: %d, %d, %d",
                                    dev, auMsgType, auMsgEnable);

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call with portDev [0], vlanId [100],  */
            /* and vlanMask [0xFFF]. Expected: GT_OK.       */
            portDev = 0;
            vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;

            st = cpssExMxBrgFdbFlushPortEmulate(dev, portDev, port, vlanId, vlanMask);

            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st,
                                        dev, portDev, port, vlanId, vlanMask);

            /* 1.1.2. Call with out of range for vlanId. Expected: GT_BAD_PARAM. */
            vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
            /* portDev == 0 */
            st = cpssExMxBrgFdbFlushPortEmulate(dev, portDev, port, vlanId, vlanMask);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st,
                                        dev, portDev, port, vlanId, vlanMask);
            /*1.1.3. Call with out of range for portDev. Expected: GT_OUT_OF_RANGE. */
            portDev = PRV_CPSS_MAX_PP_DEVICES_CNS;
            vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
            st = cpssExMxBrgFdbFlushPortEmulate(dev, portDev, port, vlanId, vlanMask);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OUT_OF_RANGE, st,
                                        dev, portDev, port, vlanId, vlanMask);
        }

        portDev = 0;
        vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each invalid port */
            st = cpssExMxBrgFdbFlushPortEmulate(dev, portDev, port, vlanId, vlanMask);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st,
                                        dev, portDev, port, vlanId, vlanMask);
        }

        /* 1.3. For active device check that function returns GT_OUT_OF_RANGE */
        /* for out of bound value for port number.                            */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        /* portDev == 0                          */
        /* vlanId == BRG_FDB_TESTED_VLAN_ID_CNS  */

        st = cpssExMxBrgFdbFlushPortEmulate(dev, portDev, port, vlanId, vlanMask);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OUT_OF_RANGE, st,
                                    dev, portDev, port, vlanId, vlanMask);

        auMsgEnable = GT_TRUE;
        st = cpssExMxBrgFdbAuToCpuSet(dev, auMsgType, auMsgEnable);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                    "cpssExMxBrgFdbAuToCpuSet: %d, %d, %d",
                                    dev, auMsgType, auMsgEnable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    port = BRG_FDB_VALID_PHY_PORT_CNS;
    portDev = 0;
    vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbFlushPortEmulate(dev, portDev, port, vlanId, vlanMask);
        UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st,
                                    dev, portDev, port, vlanId, vlanMask);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == dev, portDev, port, vlanId, vlanMask */

    st = cpssExMxBrgFdbFlushPortEmulate(dev, portDev, port, vlanId, vlanMask);
    UTF_VERIFY_EQUAL5_PARAM_MAC(GT_BAD_PARAM, st,
                                dev, portDev, port, vlanId, vlanMask);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbFlush
(
    IN GT_U8    dev,
    IN GT_U16   vlanId,
    IN GT_U16   vlanMask
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbFlush)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U16  vlanId;
    GT_U16  vlanMask = BRG_FDB_TESTED_VLAN_MASK_CNS;

    CPSS_MAC_AU2CPU_TYPE_ENT    auMsgType = CPSS_MAC_AU_MESSAGE_TA_AA_E;
    GT_BOOL                     auMsgEnable;

        /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        auMsgEnable = GT_FALSE;
        st = cpssExMxBrgFdbAuToCpuSet(dev, auMsgType, auMsgEnable);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                    "cpssExMxBrgFdbAuToCpuSet: %d, %d, %d",
                                    dev, auMsgType, auMsgEnable);

        /*1.1. Call with vlanId [100] and vlanMask [0xFFF]. Expected: GT_OK. */
        vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
        st = cpssExMxBrgFdbFlush(dev, vlanId, vlanMask);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, vlanMask);

        /* 1.2. Call with out range for vlandId. Expected: GT_BAD_PARAM. */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        st = cpssExMxBrgFdbFlush(dev, vlanId, vlanMask);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, vlanMask);

        auMsgEnable = GT_TRUE;
        st = cpssExMxBrgFdbAuToCpuSet(dev, auMsgType, auMsgEnable);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                    "cpssExMxBrgFdbAuToCpuSet: %d, %d, %d",
                                    dev, auMsgType, auMsgEnable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbFlush(dev, vlanId, vlanMask);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, vlanMask);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* vlanId = BRG_FDB_TESTED_VLAN_ID_CNS */

    st = cpssExMxBrgFdbFlush(dev, vlanId, vlanMask);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId, vlanMask);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMaxLookupLenSet
(
    IN GT_U8    dev,
    IN GT_U8    lookupLen
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMaxLookupLenSet)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U8   lookupLen;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with lookupLen [2, 4, 6, 8, 10, 12, 14, and 16].   */
        /* Expected: GT_OK.                                             */
        lookupLen = 2;
        do
        {
            st = cpssExMxBrgFdbMaxLookupLenSet(dev, lookupLen);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);
            lookupLen = lookupLen + 2;
        }
        while (lookupLen < 17);

        /* 1.2. Call with lookupLen [0,1,3,5,7,9,11,13,15,17].  */
        /* Expected: NON GT_OK.                                 */
        lookupLen = 0;
        st = cpssExMxBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        lookupLen = 1;
        do
        {
            st = cpssExMxBrgFdbMaxLookupLenSet(dev, lookupLen);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);
            lookupLen = lookupLen + 2;
        }
        while (lookupLen < 18);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    lookupLen = 2;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, lookupLen);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMaxLookupLenSet(dev, lookupLen);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, lookupLen);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacAddrEntryInvalidate
(
    IN GT_U8         dev,
    IN GT_U32        index
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacAddrEntryInvalidate)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U32  index;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with index [0]. Expected: GT_OK.*/
        index = 0;
        st = cpssExMxBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* 1.2. Call with out of range index [0xFFFFFFFF].  */
        /* Expected: NON GT_OK.                             */
        index = 0xFFFFFFFF;
        st = cpssExMxBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    index = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, index);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* index == 0 */

    st = cpssExMxBrgFdbMacAddrEntryInvalidate(dev, index);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, index);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbL2IngrProcEnable
(
    IN GT_U8         dev,
    IN GT_BOOL       enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbL2IngrProcEnable)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function for with enable = GT_TRUE and GT_FALSE.   */
        /* Must return GT_OK.                                           */
        enable = GT_TRUE;
        st = cpssExMxBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        enable = GT_FALSE;
        st = cpssExMxBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enable = GT_TRUE;

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    enable = GT_FALSE;

    st = cpssExMxBrgFdbL2IngrProcEnable(dev, enable);
    UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, enable);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbPortAuToCpuSet
(
    IN GT_U8                dev,
    IN GT_U8                portNum,
    IN CPSS_MAC_AU2CPU_TYPE_ENT   auMsgType,
    IN GT_BOOL              enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbPortAuToCpuSet)
{
    GT_STATUS   st = GT_OK;

    GT_U8                       dev;
    GT_U8                       port;
    CPSS_MAC_AU2CPU_TYPE_ENT    auMsgType;
    GT_BOOL                     enable;

   /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_TWISTD_TIGER_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    auMsgType = CPSS_MAC_AU_MESSAGE_ALL_E;
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* prepare virtual port iterator */
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 1.1.1. Call function for each port               */
            /* with auMsgType [CPSS_MAC_AU_MESSAGE_ALL_E] and   */
            /* enable [GT_TRUE and GT_FALSE]. Expected: GT_OK.  */
            enable = GT_TRUE;

            st = cpssExMxBrgFdbPortAuToCpuSet(dev, port, auMsgType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st,
                                        dev, port, auMsgType, enable);

            enable = GT_FALSE;

            st = cpssExMxBrgFdbPortAuToCpuSet(dev, port, auMsgType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st,
                                        dev, port, auMsgType, enable);

            /* 1.1.2. Check for invalid auMsgType [0x5AAAAAA5]. */
            /* Expected: GT_BAD_PARAM.                          */
            st = cpssExMxBrgFdbPortAuToCpuSet(dev, port, BRG_FDB_INVALID_ENUM_CNS, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st,
                                        dev, port, BRG_FDB_INVALID_ENUM_CNS, enable);
        }

        /* prepare virtual port iterator */
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        enable = GT_TRUE;

        /* 1.2. Go over non configured ports */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non configured port.   */
            st = cpssExMxBrgFdbPortAuToCpuSet(dev, port, auMsgType, enable);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st,
                                        dev, port, auMsgType, enable);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                      */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        st = cpssExMxBrgFdbPortAuToCpuSet(dev, port, auMsgType, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, auMsgType, enable);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/

    port = BRG_FDB_VALID_VIRT_PORT_CNS;
    auMsgType = CPSS_MAC_AU_MESSAGE_QA_E;
    enable =GT_TRUE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_TWISTD_TIGER_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbPortAuToCpuSet(dev, port, auMsgType, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, auMsgType, enable);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    /* port == BRG_FDB_VALID_VIRT_PORT_CNS       */
    /* auMsgType == CPSS_MAC_AU_MESSAGE_QA_E     */
    /* enable =GT_TRUE                           */

    st = cpssExMxBrgFdbPortAuToCpuSet(dev, port, auMsgType, enable);
    UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev ,port, auMsgType, enable);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacFilterEntrySet
(
    IN GT_U8                dev,
    IN GT_32                entryInd,
    IN GT_ETHERADDR         *macAddrPtr,
    IN GT_ETHERADDR         *macMaskPtr,
    IN CPSS_MAC_FLT_CMD_ENT daCommand,
    IN CPSS_MAC_FLT_CMD_ENT saCommand
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacFilterEntrySet)
{
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    GT_32                   entryInd;
    GT_ETHERADDR            macAddr = BRG_FDB_TESTED_MAC_ADDR_CNS;
    GT_ETHERADDR            macMask = BRG_FDB_TESTED_MAC_MASK_CNS;
    CPSS_MAC_FLT_CMD_ENT    daCommand;
    CPSS_MAC_FLT_CMD_ENT    saCommand;

    GT_ETHERADDR            retMacAddr;
    GT_ETHERADDR            retMacMask;
    CPSS_MAC_FLT_CMD_ENT    retDaCommand;
    CPSS_MAC_FLT_CMD_ENT    retSaCommand;

    GT_BOOL                 failureWas = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with entryInd [0], macAddr [00:1A:FF:FF:FF:FF] */
        /* and macMask [FF:FF:FF:FF:FF:FF], daCommand [CPSS_MAC_FLT_CNTL_E] */
        /* and saCommand [CPSS_MAC_FLT_CNTL_E]. Expected: GT_OK. */
        entryInd = 0;
        /* macAddr = {0x00, 0x1A, 0xFF, 0xFF, 0xFF, 0xFF}; */
        /* macMask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; */
        daCommand = CPSS_MAC_FLT_CNTL_E;
        saCommand = CPSS_MAC_FLT_CNTL_E;

        st = cpssExMxBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask,
                                             daCommand, saCommand);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                                     "%d, %d, <macAddrPtr>, <macMaskPtr>, %d, %d",
                                     dev, entryInd, daCommand, saCommand);

        /* 1.2. Call cpssExMxBrgFdbMacFilterEntryGet. */
        /* Expected: GT_OK and the same output parameters as input was. */
        st = cpssExMxBrgFdbMacFilterEntryGet(dev, entryInd, &retMacAddr, &retMacMask,
                                             &retDaCommand, &retSaCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "cpssExMxBrgFdbMacFilterEntryGet: %d, %d",
                                     dev, entryInd);
        if (GT_OK == st)
        {
            /* check if macAddr returned by Get the same as was Set */
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&macAddr,
                                            (const GT_VOID*)&retMacAddr, sizeof(macAddr))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                                         "get another macAddr than was set [dev = %d, entryInd = %d]",
                                         dev, entryInd);
            /* [TODO]: output of set and got macAddr */

            /* check if macMask returned by Get the same as was Set */
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&macMask,
                                            (const GT_VOID*)&retMacMask, sizeof(macMask))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                                         "get another macMask than was set [dev = %d, entryInd = %d]",
                                         dev, entryInd);
            /* [TODO]: output of set and got macMask */

            /* check if daCommand returned by Get the same as was Set */
            UTF_VERIFY_EQUAL4_STRING_MAC(daCommand, retDaCommand,
                                         "was set daCommand = %d but get %d [dev = %d, entryInd = %d]",
                                         daCommand, retDaCommand, dev, entryInd);

            /* check if saCommand returned by Get the same as was Set */
            UTF_VERIFY_EQUAL4_STRING_MAC(saCommand, retSaCommand,
                                         "was set saCommand = %d but get %d [dev = %d, entryInd = %d]",
                                         saCommand, retSaCommand, dev, entryInd);
        }
        /* 1.3. Check for invalid daCommand. Expected: GT_BAD_PARAM. */
        /* entryInd == 0                                    */
        /* macAddr == {0x00, 0x1A, 0xFF, 0xFF, 0xFF, 0xFF}  */
        /* macMask == {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}  */
        /* saCommand == CPSS_MAC_FLT_CNTL_E                 */
        daCommand = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask,
                                             daCommand, saCommand);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, %d, <macAddrPtr>, <macMaskPtr>, %d, %d",
                                     dev, entryInd, daCommand, saCommand);

        /* 1.4. Check for invalid saCommand. Expected: GT_BAD_PARAM. */
        /* entryInd == 0                                    */
        /* macAddr == {0x00, 0x1A, 0xFF, 0xFF, 0xFF, 0xFF}  */
        /* macMask == {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}  */
        saCommand = BRG_FDB_INVALID_ENUM_CNS;
        daCommand = CPSS_MAC_FLT_CNTL_E;

        st = cpssExMxBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask,
                                             daCommand, saCommand);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, %d, <macAddrPtr>, <macMaskPtr>, %d, %d",
                                     dev, entryInd, daCommand, saCommand);

        /* 1.5. Call with out of range entryInd. Expected: NON GT_OK. */
        entryInd = MAC_RANGE_FILTER_TABLE_SIZE_CNS;
        saCommand = CPSS_MAC_FLT_FRWRD_E;
        /* macAddr == {0x00, 0x1A, 0xFF, 0xFF, 0xFF, 0xFF}  */
        /* macMask == {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}  */
        /* daCommand == CPSS_MAC_FLT_CNTL_E                 */

        st = cpssExMxBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask,
                                             daCommand, saCommand);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                                     "%d, %d, <macAddrPtr>, <macMaskPtr>, %d, %d",
                                     dev, entryInd, daCommand, saCommand);

        /* 1.6. Call with macAddrPtr [NULL]. Expected: GT_BAD_PTR. */
        entryInd = 0;
        /* macMask == {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}  */
        /* daCommand == CPSS_MAC_FLT_CNTL_E                 */
        /* saCommand == CPSS_MAC_FLT_FRWRD_E                */

        st = cpssExMxBrgFdbMacFilterEntrySet(dev, entryInd, NULL, &macMask,
                                             daCommand, saCommand);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, NULL, <macMaskPtr>, %d, %d",
                                     dev, entryInd, daCommand, saCommand);

        /* 1.7. Call with macMaskPtr [NULL]. Expected: GT_BAD_PTR. */
        /* entryInd == 0                                    */
        /* macAddr == {0x00, 0x1A, 0xFF, 0xFF, 0xFF, 0xFF}  */
        /* daCommand == CPSS_MAC_FLT_CNTL_E                 */
        /* saCommand == CPSS_MAC_FLT_FRWRD_E                */

        st = cpssExMxBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, NULL,
                                             daCommand, saCommand);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, <macAddrPtr>, NULL, %d, %d",
                                     dev, entryInd, daCommand, saCommand);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* entryInd == 0                                    */
    /* macAddr == {0x00, 0x1A, 0xFF, 0xFF, 0xFF, 0xFF}  */
    /* macMask == {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}  */
    /* daCommand == CPSS_MAC_FLT_CNTL_E                 */
    /* saCommand == CPSS_MAC_FLT_FRWRD_E                */
    entryInd = 0;
    daCommand = CPSS_MAC_FLT_CNTL_E;
    saCommand = CPSS_MAC_FLT_FRWRD_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask,
                                             daCommand, saCommand);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, %d, <macAddrPtr>, <macMaskPtr>, %d, %d",
                                     dev, entryInd, daCommand, saCommand);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask,
                                         daCommand, saCommand);
    UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st,
                                 "%d, %d, <macAddrPtr>, <macMaskPtr>, %d, %d",
                                 dev, entryInd, daCommand, saCommand);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacFilterEntryGet
(
    IN GT_U8                dev,
    IN GT_32                entryInd,
    OUT GT_ETHERADDR         *macAddrPtr,
    OUT GT_ETHERADDR         *macMaskPtr,
    OUT CPSS_MAC_FLT_CMD_ENT *daCommandPtr,
    OUT CPSS_MAC_FLT_CMD_ENT *saCommandPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacFilterEntryGet)
{
    GT_STATUS st = GT_OK;

    GT_U8                   dev;
    GT_32                   entryInd;
    GT_ETHERADDR            macAddr;
    GT_ETHERADDR            macMask;
    CPSS_MAC_FLT_CMD_ENT    daCommand;
    CPSS_MAC_FLT_CMD_ENT    saCommand;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with entryInd [0] and valid pointers. Expected: GT_OK. */
        entryInd = 0;

        st = cpssExMxBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask,
                                             &daCommand, &saCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d", dev, entryInd);

        /* 1.2. Check for &daCommand = NULL. Expected: GT_BAD_PTR. */
        /* entryInd == 0 */
        st = cpssExMxBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask,
                                             NULL, &saCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, <macAddrPtr>, <macMaskPtr>, NULL",
                                     dev, entryInd);

        /* 1.3. Check for &saCommand = NULL. Expected: GT_BAD_PTR. */
        /* entryInd == 0 */
        st = cpssExMxBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask,
                                             &daCommand, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, <macAddrPtr>, <macMaskPtr>, <daCommandPtr>, NULL",
                                     dev, entryInd);

        /* 1.4. Call with out of range entryInd. Expected: NON GT_OK. */
        entryInd = MAC_RANGE_FILTER_TABLE_SIZE_CNS;

        st = cpssExMxBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask,
                                             &daCommand, &saCommand);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d", dev, entryInd);

        /* 1.5. Call with macAddrPtr [NULL]. Expected: GT_BAD_PTR. */
        entryInd = 0;

        st = cpssExMxBrgFdbMacFilterEntryGet(dev, entryInd, NULL, &macMask,
                                             &daCommand, &saCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, NULL, <macMaskPtr>", dev, entryInd);

        /* 1.6. Call with macMaskPtr [NULL]. Expected: GT_BAD_PTR. */
        /* entryInd == 0                                    */
        st = cpssExMxBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, NULL,
                                             &daCommand, &saCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, %d, <macAddrPtr>, NULL", dev, entryInd);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */
    entryInd = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask,
                                             &daCommand, &saCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, %d", dev, entryInd);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask,
                                         &daCommand, &saCommand);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, %d", dev, entryInd);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbDeviceTableSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  devTable[CPSS_EXMX_FDB_DEV_TABLE_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbDeviceTableSet)
{
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_U32  devTable[CPSS_EXMX_FDB_DEV_TABLE_SIZE_CNS];


    /* zero out the entry */
    cpssOsBzero((GT_VOID*)&devTable, sizeof(devTable[0]) * CPSS_EXMX_FDB_DEV_TABLE_SIZE_CNS );

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with zeroed devTable.  Expected: GT_OK. */
        st = cpssExMxBrgFdbDeviceTableSet(dev, devTable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbDeviceTableSet(dev, devTable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbDeviceTableSet(dev, devTable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbTransplant
(
    IN GT_U8    dev,
    IN GT_U8    devNew,
    IN GT_U8    newPort,
    IN GT_U8    devOld,
    IN GT_U8    oldPort,
    IN GT_U16   vlanId,
    IN GT_U16   vlanMask
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbTransplant)
{
    GT_STATUS st = GT_OK;

    GT_U8    dev;
    GT_U8    devNew;
    GT_U8    newPort;
    GT_U8    devOld;
    GT_U8    oldPort;
    GT_U16   vlanId;
    GT_U16   vlanMask;

    CPSS_MAC_AU2CPU_TYPE_ENT    auMsgType = CPSS_MAC_AU_MESSAGE_TA_AA_E;
    GT_BOOL                     auMsgEnable;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        auMsgEnable = GT_FALSE;
        st = cpssExMxBrgFdbAuToCpuSet(dev, auMsgType, auMsgEnable);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                    "cpssExMxBrgFdbAuToCpuSet: %d, %d, %d",
                                    dev, auMsgType, auMsgEnable);

        /* 1.1. Call with devNew [0], newPort [0], devOld [0],               */
        /* oldPort [0], vlanId [100], and vlanMask [0xFFF]. Expected: GT_OK. */
        devNew = 0;
        newPort = 0;
        devOld = 0;
        oldPort = 0;
        vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
        vlanMask = BRG_FDB_TESTED_VLAN_MASK_CNS;

        st = cpssExMxBrgFdbTransplant(dev, devNew, newPort, devOld, oldPort,
                                      vlanId, vlanMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call with out of range devNew. Expected: GT_OUT_OF_RANGE. */
        devNew = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxBrgFdbTransplant(dev, devNew, newPort, devOld, oldPort,
                                      vlanId, vlanMask);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OUT_OF_RANGE, st, dev, devNew);

        /* 1.3. Call with out of range newPort. Expected: GT_OUT_OF_RANGE. */
        devNew = 0;
        newPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS; /*Physical port?!*/

        st = cpssExMxBrgFdbTransplant(dev, devNew, newPort, devOld, oldPort,
                                      vlanId, vlanMask);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, <devNew>, %d", dev, newPort);

        /* 1.4. Call with out of range devOld. Expected: GT_OUT_OF_RANGE. */
        newPort = 0;
        devOld = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxBrgFdbTransplant(dev, devNew, newPort, devOld, oldPort,
                                      vlanId, vlanMask);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, <devNew>, <newPort>, %d", dev, devOld);

        /* 1.5. Call with out of range oldPort. Expected: GT_OUT_OF_RANGE. */
        devOld = 0;
        oldPort = PRV_CPSS_MAX_PP_PORTS_NUM_CNS; /*Physical port?!*/

        st = cpssExMxBrgFdbTransplant(dev, devNew, newPort, devOld, oldPort,
                                      vlanId, vlanMask);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, <devNew>, <newPort>, <devOld>, %d", dev, oldPort);

        /* 1.6. Call function with out of range vlanId. Expected: GT_BAD_PARAM. */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        oldPort = 0;
        st = cpssExMxBrgFdbTransplant(dev, devNew, newPort, devOld, oldPort,
                                      vlanId, vlanMask);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, <devNew>, <newPort>, <devOld>, <oldPort>, %d", dev, vlanId);

        auMsgEnable = GT_TRUE;
        st = cpssExMxBrgFdbAuToCpuSet(dev, auMsgType, auMsgEnable);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                    "cpssExMxBrgFdbAuToCpuSet: %d, %d, %d",
                                    dev, auMsgType, auMsgEnable);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM.*/
    vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
    vlanMask = BRG_FDB_TESTED_VLAN_MASK_CNS;
    devNew = 0;
    newPort = 0;
    devOld = 0;
    oldPort = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbTransplant(dev, devNew, newPort, devOld, oldPort,
                                      vlanId, vlanMask);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbTransplant(dev, devNew, newPort, devOld, oldPort,
                                  vlanId, vlanMask);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacAddrEntryWrite
(
    IN GT_U8         dev,
    IN GT_U32        index,
    IN GT_BOOL       skip,
    IN CPSS_MAC_ENTRY_STC  *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacAddrEntryWrite)
{
/*
ITERATE_DEVICES
1.1. Call with skip [GT_FALSE], index [0], macAddr [00:1A:FF:FF:FF:FF], vlanId [100]. Expected: GT_OK.
1.2. #if 0 (the caller need to implement get AU message)
Call cpssExMxBrgFdbQaSend with non-null entryPtr with macAddr [00:1A:FF:FF:FF:FF] and vlanId [100]. Expected: GT_OK and entryWasFound [GT_TRUE], macEntry the same as input was:
check next fields of macEntry: isStatic, vlanId, srcTc, dstTc, daCommand, saCommand, saClass, daClass, saCib, daCib, daRoute, dstInterface.type and
if entry.dstInterface.type= CPSS_INTERFACE_PORT_E check dstInterface.devPort.devNum, dstInterface.devPort.portNum;
if entry.dstInterface.type= CPSS_INTERFACE_TRUNK_E check dstInterface.trunkId;
if entry.dstInterface.type= CPSS_INTERFACE_VIDX_E check dstInterface.vidx;
if entry.dstInterface.type= CPSS_INTERFACE_VID_E check dstInterface.vid.
#endif
1.3. Call cpssExMxBrgFdbMacAddrEntryRead
 with non-null pointers with index [0]. Expected: GT_OK and entryPtr the same as was written and skipPtr [GT_FALSE].
1.4. Call function with out of range for index [0xFFFFFFFF]. Expected: NON GT_OK.
1.5. Call function with of range for vlanId as part of entryPtr. Expected: GT_BAD_PARAM.
1.6. Call function with entryPtr [NULL]. Expected: GT_BAD_PTR.
1.7. Call function with of range entryPtr->dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS],entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E]. Expected: NON GT_OK.
1.8. Call function with of range entryPtr->dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E]. Expected: NON GT_OK.
1.9. Call function with of range entryPtr->dstInterface.trunkId [128],entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E]. Expected: NON GT_OK.
1.10. Call function with of range for entryPtr->dstInterface.vidx [0xFFFF],entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E]. Expected: NON GT_OK.
1.11. Call function with of range for entryPtr->srcTc  [8]. Expected: NON GT_OK.
1.12. Call function with of range for entryPtr->dstTc  [8]. Expected: NON GT_OK.
1.13. Call function with of range for entryPtr->daCommand [0x5AAAAAA5]. Expected: NON GT_OK.
1.14. Call function with not supported for ExMx entryPtr->daCommand [GT_MAC_TABLE_MIRROR_TO_CPU/ CPSS_MAC_TABLE_SOFT_DROP_E]. Expected: NON GT_OK.
1.15. Call function with of range for entryPtr->saCommand [0x5AAAAAA5]. Expected: NON GT_OK.
1.16. Call function with not supported for ExMx entryPtr->saCommand [GT_MAC_TABLE_MIRROR_TO_CPU/ CPSS_MAC_TABLE_SOFT_DROP_E]. Expected: NON GT_OK.
1.17. Call function with not applicable entryPtr->dstInterface. type [CPSS_INTERFACE_VID_E], entryPtr->dstInterface.vlanId [100]. Expected: NON GT_OK.
*/

    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    GT_U32                      index;
    GT_BOOL                     skip;
    CPSS_MAC_ENTRY_STC          entry;

/*    CPSS_MAC_UPDATE_MSG_STC     retEntryMsg;*/

    GT_BOOL             retValid;
    GT_BOOL             retSkip;
    GT_BOOL             retAged;
    GT_U8               retAssociatedDevNum;
    CPSS_MAC_ENTRY_STC  retEntry;

    GT_BOOL                     CmpRes;
    GT_ETHERADDR                validMac = BRG_FDB_TESTED_MAC_ADDR_CNS;


    /* zero out the entry */
    cpssOsBzero((GT_VOID*)&entry, sizeof(entry));

    /* set valid mac */
    cpssOsMemCpy((GT_VOID*)&(entry.macAddr), (const GT_VOID*)&validMac, sizeof(validMac));

    /* entry.vlan - will be set later               */
    entry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
    entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
    /* entry.dstInterface.devPort.devNum == 0       */
    /* entry.dstInterface.devPort.portNum == 0      */
    /* entry.dstInterface.trunkId == 0              */
    /* entry.dstInterface.vidx == 0                 */
    /* entry.isStatic == GT_FALSE                   */
    /* entry.srcTc == 0                             */
    /* entry.dstTc == 0                             */
    /* entry.daCib == GT_FALSE                      */
    /* entry.daClass == GT_FALSE                    */
    /* entry.saCib == GT_FALSE                      */
    /* entry.saClass == GT_FALSE                    */
    /* entry.daRoute == GT_FALSE                    */
    /* entry.cosIpv4En == GT_FALSE                  */
    /* entry.mirrorToRxAnalyzerPortEn == GT_FALSE   */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with skip [GT_FALSE], index [0]           */
        /* macAddr [00:1A:FF:FF:FF:FF], vlanId [100]. Expected: GT_OK.  */

        /* set valid vlan*/
        entry.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
        index = 0;
        skip = GT_FALSE;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

#if 0  /* the caller need to implement get AU message */
        /* 1.2. Call cpssExMxBrgFdbMacAddrHwEntryGet with non-null      */
        /* entryPtr with macAddr [00:1A:FF:FF:FF:FF] and vlanId [100].  */
        /* Expected: GT_OK and entryWasFound [GT_TRUE]                  */
        /* macEntry the same as input was.                              */
        st = cpssExMxBrgFdbQaSend(dev,entry.vlanId, &entry.macAddr);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssExMxBrgFdbMacAddrHwEntryGet: %d",
                                     dev);
        if (GT_OK == st)
        {
            /* check if entryWasFound = GT_TRUE */
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, retEntryMsg.entryWasFound,
                                        "cpssExMxBrgFdbMacAddrHwEntryGet: entry wasn't found [dev = %d]",
                                        dev);
            /* check if returned entry equal to set */
            /* CmpRes = cpssOsMemCmp((const GT_VOID*)&entry.macAddr,
                                  (const GT_VOID*)&(retEntryMsg.macEntry.macAddr),
                                   sizeof(entry.macAddr)); */
            CmpRes = GT_TRUE;

            if(retEntryMsg.macEntry.dstInterface.type != entry.dstInterface.type)
                CmpRes = GT_FALSE;

            switch(entry.dstInterface.type)
            {
            case CPSS_INTERFACE_PORT_E:
                if((retEntryMsg.macEntry.dstInterface.devPort.devNum !=
                                           entry.dstInterface.devPort.devNum) &&
                   (retEntryMsg.macEntry.dstInterface.devPort.portNum !=
                                           entry.dstInterface.devPort.portNum))
                    CmpRes = GT_FALSE;
                break;
            case CPSS_INTERFACE_TRUNK_E:
                if(retEntryMsg.macEntry.dstInterface.trunkId !=
                                           entry.dstInterface.trunkId)
                    CmpRes = GT_FALSE;
                break;
            case CPSS_INTERFACE_VIDX_E:
                if(retEntryMsg.macEntry.dstInterface.vidx !=
                                           entry.dstInterface.vidx)
                    CmpRes = GT_FALSE;
                break;
            case CPSS_INTERFACE_VID_E:
                if(retEntryMsg.macEntry.dstInterface.vlanId !=
                                           entry.dstInterface.vlanId)
                    CmpRes = GT_FALSE;
                break;
            default:
                CmpRes = GT_FALSE;
            }

            if(retEntryMsg.macEntry.isStatic != entry.isStatic)
                CmpRes = GT_FALSE;
            if(retEntryMsg.macEntry.vlanId != entry.vlanId)
                CmpRes = GT_FALSE;
            if(retEntryMsg.macEntry.srcTc != entry.srcTc)
                CmpRes = GT_FALSE;
            if(retEntryMsg.macEntry.dstTc != entry.dstTc)
                CmpRes = GT_FALSE;
            if(retEntryMsg.macEntry.daCommand != entry.daCommand)
                CmpRes = GT_FALSE;
            if(retEntryMsg.macEntry.saCommand != entry.saCommand)
                CmpRes = GT_FALSE;
            if(retEntryMsg.macEntry.saClass != entry.saClass)
                CmpRes = GT_FALSE;
            if(retEntryMsg.macEntry.daClass != entry.daClass)
                CmpRes = GT_FALSE;
            if(retEntryMsg.macEntry.saCib != entry.saCib)
                CmpRes = GT_FALSE;
            if(retEntryMsg.macEntry.daCib != entry.daCib)
                CmpRes = GT_FALSE;
            if(retEntryMsg.macEntry.daRoute != entry.daRoute)
                CmpRes = GT_FALSE;
            /* these parameters are not relevant in EX */
            /*
            GT_BOOL                 cosIpv4En;
            GT_BOOL                 mirrorToRxAnalyzerPortEn*/

            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, CmpRes,
                                        "cpssExMxBrgFdbMacAddrHwEntryGet: get another macEntry than was written [dev = %d]",
                                        dev);
        }

#endif  /* the caller need to implement get AU message */

        /* 1.3. Call cpssExMxBrgFdbMacAddrEntryRead with non-null pointers  */
        /* with index [0]. Expected: GT_OK and entryPtr the same as was     */
        /* written and skipPtr [GT_FALSE].                                  */
        st = cpssExMxBrgFdbMacAddrEntryRead(dev, index, &retValid, &retSkip,
                                  &retAged, &retAssociatedDevNum, &retEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                                     "cpssExMxBrgFdbMacAddrEntryRead: %d, %d",
                                     dev, index);
        if (GT_OK == st)
        {
            /* check if returned entry equal to set */
            CmpRes = (cpssOsMemCmp((const GT_VOID*)&entry, (const GT_VOID*)&retEntry, sizeof(entry)) == 0)
            ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, CmpRes,
                "cpssExMxBrgFdbMacAddrEntryRead: get another macEntry than was written: %d, %d",
                dev, index);

            /* check if returned skip value equal to set */
            UTF_VERIFY_EQUAL2_STRING_MAC(skip, retSkip,
                "cpssExMxBrgFdbMacAddrEntryRead: get another skip than was written: %d, %d",
                dev, index);
        }

        /* 1.4. Call function with out of range for index [0xFFFFFFFF]. */
        /* Expected: NON GT_OK.                                         */
        index = 0xFFFFFFFF;
        /* skip == GT_FALSE */

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        index = 0; /* restore */

        /* 1.5. Call function with entryPtr [NULL]. Expected: GT_BAD_PTR. */
        /* skip == GT_FALSE */
        /* index == 0 */

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, NULL);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, NULL", dev, index, skip);

        /* 1.6. Call function with of range for vlanId as part of entryPtr. */
        /* Expected: GT_BAD_PARAM.                                             */
        entry.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        /* index == 0       */
        /* skip == GT_FALSE */

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, %d, %d, <entry.vlanId> = %d",
                                     dev, index, skip, entry.vlanId);

        entry.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS; /* restore */

        /* 1.7. Call function with of range
        entryPtr->dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
        entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E].
        Expected: NON GT_OK. */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;
        entry.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.dstInterface [type = %d, devPort.devNum = %d]",
            dev, index, skip, entry.dstInterface.type, entry.dstInterface.devPort.devNum);

        entry.dstInterface.devPort.devNum = 0; /* restore */

        /* 1.8. Call function with of range
        entryPtr->dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS],
        entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E].
        Expected: NON GT_OK. */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;
        entry.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.dstInterface [type = %d, devPort.portNum = %d]",
            dev, index, skip, entry.dstInterface.type, entry.dstInterface.devPort.portNum);

        entry.dstInterface.devPort.portNum = 0; /* restore */

        /* 1.9. Call function with of range
        entryPtr->dstInterface.trunkId [128],
        entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E].
        Expected: NON GT_OK. */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;
        entry.dstInterface.trunkId = 128;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.dstInterface [type = %d, trunkId = %d]",
            dev, index, skip, entry.dstInterface.type, entry.dstInterface.trunkId);

        entry.dstInterface.trunkId = 0; /* restore */

        /* 1.10. Call function with of range for
        entryPtr->dstInterface.vidx [0xFFFF],
        entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E].
        Expected: NON GT_OK. */
        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;
        entry.dstInterface.vidx = 0xFFFF;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL5_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.dstInterface [type = %d, vidx = %d]",
            dev, index, skip, entry.dstInterface.type, entry.dstInterface.vidx);

        entry.dstInterface.vidx = 0; /* restore */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E; /* restore */

        /* 1.11. Call function with of range for entryPtr->srcTc [8].
        Expected: NON GT_OK. */
        entry.srcTc = CPSS_TC_RANGE_CNS;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.srcTc = %d", dev, index, skip, entry.srcTc);

        entry.srcTc = 0; /* restore */

        /* 1.12. Call function with of range for entryPtr->dstTc [8].
        Expected: NON GT_OK. */
        entry.dstTc = CPSS_TC_RANGE_CNS;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.dstTc = %d", dev, index, skip, entry.dstTc);

        entry.dstTc = 0; /* restore */

        /* 1.13. Call function with of range for entryPtr->daCommand [0x5AAAAAA5].
        Expected: NON GT_OK. */
        entry.daCommand = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.daCommand = %d", dev, index, skip, entry.daCommand);

        /* 1.14. Call function with not supported for ExMx
        entryPtr->daCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E/ CPSS_MAC_TABLE_SOFT_DROP_E].
        Expected: NON GT_OK. */
        entry.daCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.daCommand = %d", dev, index, skip, entry.daCommand);

        entry.daCommand = CPSS_MAC_TABLE_SOFT_DROP_E;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.daCommand = %d", dev, index, skip, entry.daCommand);

        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E; /* restore */

        /* 1.15. Call function with of range for
        entryPtr->saCommand [0x5AAAAAA5]. Expected: NON GT_OK. */
        entry.saCommand = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.saCommand = %d", dev, index, skip, entry.saCommand);

        /* 1.16. Call function with not supported for ExMx
        entryPtr->saCommand [GT_MAC_TABLE_MIRROR_TO_CPU/ CPSS_MAC_TABLE_SOFT_DROP_E].
        Expected: NON GT_OK. */
        entry.saCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.saCommand = %d", dev, index, skip, entry.saCommand);

        entry.saCommand = CPSS_MAC_TABLE_SOFT_DROP_E;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.saCommand = %d", dev, index, skip, entry.saCommand);

        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E; /* restore */

        /* 1.17. Call function with not applicable
        entryPtr->dstInterface.type [CPSS_INTERFACE_VID_E],
        entryPtr->dstInterface.vlanId [100]. Expected: NON GT_OK. */
        entry.dstInterface.type = CPSS_INTERFACE_VID_E;
        entry.dstInterface.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
            "%d, %d, %d, entry.dstInterface.type = %d",
            dev, index, skip, entry.dstInterface.type);

        entry.dstInterface.vlanId = 0; /* restore */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E; /* restore */
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */
    cpssOsBzero((GT_VOID*)&entry, sizeof(entry));
    cpssOsMemCpy((GT_VOID*)&(entry.macAddr), (const GT_VOID*)&validMac, sizeof(validMac));
    entry.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
    index = 0;
    skip = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, index, skip);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
    UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, index, skip);
}

/* #if 0 the caller need to implement get AU message */
/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbQaSend
(
    IN  GT_U8               dev,
    IN  GT_U16              vid,
    IN  GT_ETHERADDR        *macAddrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbQaSend)
{
/*
ITERATE_DEVICES
1.1. Call cpssExMxBrgFdbMacAddrEntryWrite with skip [GT_FALSE], index [0], macAddr [00:1A:FF:FF:FF:FF], vlanId [100]. Expected: GT_OK.
1.2. Call function with macAddr [00:1A:FF:FF:FF:FF] and vlanId [100]. Expected: GT_OK.
1.3. Call function with of range for vlanId. Expected: GT_BAD_PARAM.
1.4. Call function with macAddrPtrPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    GT_U16                      vid;

    CPSS_MAC_ENTRY_STC          entry;
    GT_ETHERADDR                validMac = BRG_FDB_TESTED_MAC_ADDR_CNS;


    cpssOsBzero((GT_VOID*)&entry, sizeof(entry));
    cpssOsMemCpy((GT_VOID*)&(entry.macAddr), (const GT_VOID*)&validMac, sizeof(validMac));
    entry.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
    entry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
    entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
    /* entry.dstInterface.devPort.devNum == 0       */
    /* entry.dstInterface.devPort.portNum == 0      */
    /* entry.dstInterface.trunkId == 0              */
    /* entry.dstInterface.vidx == 0                 */
    /* entry.isStatic == GT_FALSE                   */
    /* entry.srcTc == 0                             */
    /* entry.dstTc == 0                             */
    /* entry.daCib == GT_FALSE                      */
    /* entry.daClass == GT_FALSE                    */
    /* entry.saCib == GT_FALSE                      */
    /* entry.saClass == GT_FALSE                    */
    /* entry.daRoute == GT_FALSE                    */
    /* entry.cosIpv4En == GT_FALSE                  */
    /* entry.mirrorToRxAnalyzerPortEn == GT_FALSE   */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call cpssExMxBrgFdbMacAddrEntryWrite                        */
        /* with skip [GT_FALSE], index [0], macAddr [00:1A:FF:FF:FF:FF],    */
        /* vlanId [100]. Expected: GT_OK.                                   */

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, 0, GT_FALSE, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                     "cpssExMxBrgFdbMacAddrEntryWrite: %d, %d, %d",
                                     dev, 0, GT_FALSE);

        /* 1.2. Call function with macAddr [00:1A:FF:FF:FF:FF]
        and vlanId [100]. Expected: GT_OK. */
        vid = entry.vlanId;

        st = cpssExMxBrgFdbQaSend(dev, vid, &(entry.macAddr));
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* 1.3. Call function with of range for vlanId.
        Expected: GT_BAD_PARAM. */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgFdbQaSend(dev, vid, &(entry.macAddr));
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vid);

        vid = entry.vlanId; /* restore */

        /* 1.4. Call function with macAddrPtrPtr [NULL].
        Expected: GT_BAD_PTR. */

        st = cpssExMxBrgFdbQaSend(dev, vid, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, vid);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbQaSend(dev, entry.vlanId, &(entry.macAddr));
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbQaSend(dev, entry.vlanId, &(entry.macAddr));
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbQiSend
(
    IN  GT_U8               dev,
    IN  GT_U16              vid,
    IN  GT_ETHERADDR        *macAddrPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbQiSend)
{
/*
ITERATE_DEVICES
1.1. Call cpssExMxBrgFdbMacAddrEntryWrite with skip [GT_FALSE], index [0], macAddr [00:1A:FF:FF:FF:FF], vlanId [100]. Expected: GT_OK.
1.2. Call function with non-null macAddrPtr [00:1A:FF:FF:FF:FF], vid [100]. Expected: GT_OK.
1.3. Call function with of range for vid. Expected: GT_BAD_PARAM.
1.4. Call function with macAddrPtr [NULL]. Expected: GT_BAD_PTR.
*/

    GT_STATUS st = GT_OK;

    GT_U8               dev;
    GT_U16              vid;

    CPSS_MAC_ENTRY_STC  entry;
    GT_ETHERADDR        validMac = BRG_FDB_TESTED_MAC_ADDR_CNS;


    cpssOsBzero((GT_VOID*)&entry, sizeof(entry));
    cpssOsMemCpy((GT_VOID*)&(entry.macAddr), (const GT_VOID*)&validMac, sizeof(validMac));
    entry.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
    entry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
    entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
    /* entry.dstInterface.devPort.devNum == 0       */
    /* entry.dstInterface.devPort.portNum == 0      */
    /* entry.dstInterface.trunkId == 0              */
    /* entry.dstInterface.vidx == 0                 */
    /* entry.isStatic == GT_FALSE                   */
    /* entry.srcTc == 0                             */
    /* entry.dstTc == 0                             */
    /* entry.daCib == GT_FALSE                      */
    /* entry.daClass == GT_FALSE                    */
    /* entry.saCib == GT_FALSE                      */
    /* entry.saClass == GT_FALSE                    */
    /* entry.daRoute == GT_FALSE                    */
    /* entry.cosIpv4En == GT_FALSE                  */
    /* entry.mirrorToRxAnalyzerPortEn == GT_FALSE   */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call cpssExMxBrgFdbMacAddrEntryWrite                        */
        /* with skip [GT_FALSE], index [0], macAddr [00:1A:FF:FF:FF:FF],    */
        /* vlanId [100]. Expected: GT_OK.                                   */

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, 0, GT_FALSE, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
                                     "cpssExMxBrgFdbMacAddrEntryWrite: %d, %d, %d",
                                     dev, 0, GT_FALSE);

        /* 1.2. Call function with macAddr [00:1A:FF:FF:FF:FF]
        and vlanId [100]. Expected: GT_OK. */
        vid = entry.vlanId;

        st = cpssExMxBrgFdbQiSend(dev, vid, &(entry.macAddr));
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* 1.3. Call function with of range for vlanId.
        Expected: GT_BAD_PARAM. */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxBrgFdbQiSend(dev, vid, &(entry.macAddr));
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vid);

        vid = entry.vlanId; /* restore */

        /* 1.4. Call function with macAddrPtrPtr [NULL].
        Expected: GT_BAD_PTR. */

        st = cpssExMxBrgFdbQiSend(dev, vid, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, vid);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbQiSend(dev, entry.vlanId, &(entry.macAddr));
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbQiSend(dev, entry.vlanId, &(entry.macAddr));
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}
/* #endif the caller need to implement get AU message */


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacAddrEntryRead
(
    IN GT_U8         dev,
    IN GT_U32        index,
    OUT GT_BOOL      *validPtr,
    OUT GT_BOOL      *skipPtr,
    OUT GT_BOOL      *agedPtr,
    OUT GT_U8        *associatedDevNumPtr,
    OUT CPSS_MAC_ENTRY_STC  *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacAddrEntryRead)
{
/*
ITERATE_DEVICES
1.1. Call with cpssExMxBrgFdbMacAddrEntryWrite skip [GT_FALSE], index [0], macAddr [00:1A:FF:FF:FF:FF], vlanId [100]. Expected: GT_OK.
1.2. Call function with index [0] and non-NULL validPtr, skipPtr, agedPtr, AssociatedDevNumPtr and entryPtr. Expected: GT_OK and entryPtr the same as was written and skipPtr [GT_FALSE].
1.3. Call function with validPtr [NULL] and other parametes the same as in 1.2. Expected: GT_BAD_PTR.
1.4. Call function with skipPtr [NULL] and other parametes the same as in 1.2. Expected: GT_BAD_PTR.
1.5. Call function with agedPtr [NULL] and other parametes the same as in 1.2. Expected: GT_BAD_PTR.
1.6. Call function with associatedDevNumPtr [NULL] and other parametes the same as in 1.2. Expected: GT_BAD_PTR.
1.7. Call function with entryPtr [NULL] and other parametes the same as in 1.2. Expected: GT_BAD_PTR.
1.8. Call with out of range index [0xFFFFFFFF]. Expected: NON GT_OK.
*/

    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    GT_U32                      index;
    GT_BOOL                     skip;
    CPSS_MAC_ENTRY_STC          entry;

    GT_BOOL             retValid;
    GT_BOOL             retSkip;
    GT_BOOL             retAged;
    GT_U8               retAssociatedDevNum;
    CPSS_MAC_ENTRY_STC  retEntry;

    GT_32                       CmpRes;
    GT_ETHERADDR                validMac = BRG_FDB_TESTED_MAC_ADDR_CNS;


    /* zero out the entry */
    cpssOsBzero((GT_VOID*)&entry, sizeof(entry));
    /* set valid mac */
    cpssOsMemCpy((GT_VOID*)&(entry.macAddr), (const GT_VOID*)&validMac, sizeof(validMac));
    entry.vlanId = BRG_FDB_TESTED_VLAN_ID_CNS;
    entry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
    entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
    /* entry.dstInterface.devPort.devNum == 0       */
    /* entry.dstInterface.devPort.portNum == 0      */
    /* entry.dstInterface.trunkId == 0              */
    /* entry.dstInterface.vidx == 0                 */
    /* entry.isStatic == GT_FALSE                   */
    /* entry.srcTc == 0                             */
    /* entry.dstTc == 0                             */
    /* entry.daCib == GT_FALSE                      */
    /* entry.daClass == GT_FALSE                    */
    /* entry.saCib == GT_FALSE                      */
    /* entry.saClass == GT_FALSE                    */
    /* entry.daRoute == GT_FALSE                    */
    /* entry.cosIpv4En == GT_FALSE                  */
    /* entry.mirrorToRxAnalyzerPortEn == GT_FALSE   */
    
    skip = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with cpssExMxBrgFdbMacAddrEntryWrite skip [GT_FALSE],
        index [0], macAddr [00:1A:FF:FF:FF:FF], vlanId [100]. Expected: GT_OK. */
        index = 0;

        st = cpssExMxBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,
            "cpssExMxBrgFdbMacAddrEntryWrite: %d, %d, %d", dev, index, skip);

        /* 1.2. Call function with index [0] and non-NULL validPtr, skipPtr,
        agedPtr, AssociatedDevNumPtr and entryPtr.
        Expected: GT_OK and entryPtr the same as was written and
        skipPtr [GT_FALSE]. */
        st = cpssExMxBrgFdbMacAddrEntryRead(dev, index, &retValid, &retSkip,
                                  &retAged, &retAssociatedDevNum, &retEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
        if (GT_OK == st)
        {
            /* check if returned entry equal to set */
            CmpRes = cpssOsMemCmp((const GT_VOID*)&entry, (const GT_VOID*)&retEntry, sizeof(entry));
            UTF_VERIFY_EQUAL2_STRING_MAC(0, CmpRes,
                "get another macEntry than was written by cpssExMxBrgFdbMacAddrEntryWrite: %d, %d",
                dev, index);

            /* check if returned skip value equal to set */
            UTF_VERIFY_EQUAL2_STRING_MAC(skip, retSkip,
                "get another skip than was written by cpssExMxBrgFdbMacAddrEntryWrite: %d, %d",
                dev, index);
        }

        /* 1.3. Call function with validPtr [NULL] and other parametes
        the same as in 1.2. Expected: GT_BAD_PTR. */
        st = cpssExMxBrgFdbMacAddrEntryRead(dev, index, NULL, &retSkip,
                                  &retAged, &retAssociatedDevNum, &retEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /* 1.4. Call function with skipPtr [NULL] and other parametes
        the same as in 1.2. Expected: GT_BAD_PTR. */
        st = cpssExMxBrgFdbMacAddrEntryRead(dev, index, &retValid, NULL,
                                  &retAged, &retAssociatedDevNum, &retEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, skipPtr = NULL", dev);

        /* 1.5. Call function with agedPtr [NULL] and other parametes
        the same as in 1.2. Expected: GT_BAD_PTR. */
        st = cpssExMxBrgFdbMacAddrEntryRead(dev, index, &retValid, &retSkip,
                                  NULL, &retAssociatedDevNum, &retEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, agedPtr = NULL", dev);

        /* 1.6. Call function with associatedDevNumPtr [NULL] and
        other parametes the same as in 1.2. Expected: GT_BAD_PTR. */
        st = cpssExMxBrgFdbMacAddrEntryRead(dev, index, &retValid, &retSkip,
                                  &retAged, NULL, &retEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, associatedDevNumPtr = NULL", dev);

        /* 1.7. Call function with entryPtr [NULL] and other parametes
        the same as in 1.2. Expected: GT_BAD_PTR. */
        st = cpssExMxBrgFdbMacAddrEntryRead(dev, index, &retValid, &retSkip,
                                  &retAged, &retAssociatedDevNum, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryPtr = NULL", dev);

        /* 1.8. Call with out of range index [0xFFFFFFFF].
        Expected: NON GT_OK. */
        index = 0xFFFFFFFF;

        st = cpssExMxBrgFdbMacAddrEntryRead(dev, index, &retValid, &retSkip,
                                  &retAged, &retAssociatedDevNum, &retEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }


    index = 0;

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacAddrEntryRead(dev, index, &retValid, &retSkip,
                                  &retAged, &retAssociatedDevNum, &retEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id. */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMacAddrEntryRead(dev, index, &retValid, &retSkip,
                              &retAged, &retAssociatedDevNum, &retEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbTrunkAgingModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_AGE_TRUNK_MODE_ENT  trunkAgingMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbTrunkAgingModeSet)
{
/*
ITERATE_DEVICES
1.1.Call function with trunkAgingMode [CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E/ CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E]. Expected: GT_OK.
1.2. Call function with invalid enum value for trunkAgingMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS st = GT_OK;

    GT_U8                           dev;
    CPSS_FDB_AGE_TRUNK_MODE_ENT     trunkAgingMode;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.Call function with trunkAgingMode
        [CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E/
        CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E]. Expected: GT_OK. */
        trunkAgingMode = CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E;

        st = cpssExMxBrgFdbTrunkAgingModeSet(dev, trunkAgingMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkAgingMode);

        trunkAgingMode = CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E;

        st = cpssExMxBrgFdbTrunkAgingModeSet(dev, trunkAgingMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkAgingMode);

        /* 1.2. Call function with invalid enum value for
        trunkAgingMode [0x5AAAAAA5]. Expected: GT_BAD_PARAM. */
        trunkAgingMode = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssExMxBrgFdbTrunkAgingModeSet(dev, trunkAgingMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, trunkAgingMode);
    }

    trunkAgingMode = CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E;

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbTrunkAgingModeSet(dev, trunkAgingMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbTrunkAgingModeSet(dev, trunkAgingMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacAddrRawEntryRead
(
    IN GT_U8         devNum,
    IN GT_U32        index,
    OUT GT_U32       entryArray[CPSS_EXMX_FDB_RAW_ENTRY_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacAddrRawEntryRead)
{
/*
ITERATE_DEVICES
1.1.Call function with index [0]. Expected: GT_OK.
1.2. Call with out of range index [0xFFFFFFFF]. Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8    dev;
    GT_U32   index;
    GT_U32   entryArray[CPSS_EXMX_FDB_RAW_ENTRY_SIZE_CNS];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1.Call function with index [0]. Expected: GT_OK. */
        index = 0;

        st = cpssExMxBrgFdbMacAddrRawEntryRead(dev, index, entryArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* 1.2. Call with out of range index [0xFFFFFFFF].
        Expected: NON GT_OK. */
        index = 0xFFFFFFFF;

        st = cpssExMxBrgFdbMacAddrRawEntryRead(dev, index, entryArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }

    index = 0;

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacAddrRawEntryRead(dev, index, entryArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMacAddrRawEntryRead(dev, index, entryArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacAddrRawEntryWrite
(
    IN GT_U8         devNum,
    IN GT_U32        index,
    IN GT_U32        entryArray[CPSS_EXMX_FDB_RAW_ENTRY_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacAddrRawEntryWrite)
{
/*
ITERATE_DEVICES
1.1. Call function with index [0] and entryArray [12, 0, 0, 345]. Expected: GT_OK.
1.2. Call function cpssExMxBrgFdbMacAddrRawEntryRead with index [0]. Expected: GT_OK and.entryArray [12, 0, 0, 345].
1.3. Call with out of range index [0xFFFFFFFF]. Expected: NON GT_OK.
*/
    GT_STATUS st = GT_OK;

    GT_U8    dev;
    GT_U32   index;
    GT_U32   entryArray[CPSS_EXMX_FDB_RAW_ENTRY_SIZE_CNS];

    GT_U32   retEntryArray[CPSS_EXMX_FDB_RAW_ENTRY_SIZE_CNS];
    GT_32    CmpRes;

    /* zero out the entry */
    cpssOsBzero((GT_VOID*)&entryArray, sizeof(entryArray));
    entryArray[0] = 12;
    entryArray[CPSS_EXMX_FDB_RAW_ENTRY_SIZE_CNS - 1] = 345;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with index [0] and entryArray [12, 0, 0, 345].
        Expected: GT_OK. */
        index = 0;

        st = cpssExMxBrgFdbMacAddrRawEntryWrite(dev, index, entryArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* 1.2. Call function cpssExMxBrgFdbMacAddrRawEntryRead with index [0].
        Expected: GT_OK and.entryArray [12, 0, 0, 345]. */
        st = cpssExMxBrgFdbMacAddrRawEntryRead(dev, index, retEntryArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxBrgFdbMacAddrRawEntryRead: %d, %d",
                                     dev, index);
        if (GT_OK == st)
        {
            /* check if returned entry equal to set */
            CmpRes = cpssOsMemCmp((const GT_VOID*)&entryArray, (const GT_VOID*)&(retEntryArray), sizeof(entryArray));
            UTF_VERIFY_EQUAL2_STRING_MAC(0, CmpRes,
                "cpssExMxBrgFdbMacAddrRawEntryRead: get another entryArray than was written: %d, %d",
                dev, index);
        }

        /* 1.3. Call with out of range index [0xFFFFFFFF].
        Expected: NON GT_OK. */
        index = 0xFFFFFFFF;

        st = cpssExMxBrgFdbMacAddrRawEntryWrite(dev, index, entryArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }

    index = 0;

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacAddrRawEntryWrite(dev, index, entryArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMacAddrRawEntryWrite(dev, index, entryArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacActionsEnableGet
(
    IN GT_U8         devNum,
    OUT GT_BOOL      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacActionsEnableGet)
{
/*
ITERATE_DEVICES
1.1. Call function non-NULL enablePtr. Expected: GT_OK.
1.2. Call function enablePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL enable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function non-NULL enablePtr. Expected: GT_OK. */
        st = cpssExMxBrgFdbMacActionsEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function enablePtr [NULL]. Expected: GT_BAD_PTR. */
        st = cpssExMxBrgFdbMacActionsEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacActionsEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMacActionsEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbMacActionsEnableSet
(
    IN GT_U8         devNum,
    IN GT_BOOL       enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbMacActionsEnableSet)
{
/*
ITERATE_DEVICES
1.1. Call function enable [GT_FALSE/ GT_TRUE]. Expected: GT_OK.
1.2. Call function cpssExMxBrgFdbMacActionsEnableGet with non-NULL enablePtr. Expected: GT_OK and enablePtr the same as just written.
*/
    GT_STATUS st = GT_OK;

    GT_U8   dev;
    GT_BOOL enable;

    GT_BOOL retEnable;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function enable [GT_FALSE/ GT_TRUE]. Expected: GT_OK. */

        /* 1.2. Call function cpssExMxBrgFdbMacActionsEnableGet with       */
        /* non-NULL enablePtr. Expected: GT_OK and enablePtr the same as   */
        /* just written.                                                   */
        enable = GT_FALSE;

        st = cpssExMxBrgFdbMacActionsEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* 1.2. for enable == GT_FALSE*/
        st = cpssExMxBrgFdbMacActionsEnableGet(dev, &retEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, retEnable,
                "cpssExMxBrgFdbMacActionsEnableGet: get another <enable> than was set: %d", dev);
        }

        /* 1.1. for enable == GT_TRUE */
        enable = GT_TRUE;

        st = cpssExMxBrgFdbMacActionsEnableSet(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* 1.2. for enable == GT_TRUE*/
        st = cpssExMxBrgFdbMacActionsEnableGet(dev, &retEnable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);
        if (GT_OK == st)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(enable, retEnable,
                "cpssExMxBrgFdbMacActionsEnableGet: get another <enable> than was set: %d", dev);
        }
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbMacActionsEnableSet(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbMacActionsEnableSet(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbAuMsgBlockGet
(
    IN     GT_U8                     devNum,
    INOUT  GT_U32                    *numOfAuPtr,
    OUT    CPSS_EXMX_ADDR_UP_MSG_STC *auMessagesPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbAuMsgBlockGet)
{
/*
ITERATE_DEVICES
1.1. Call function with numOfAuPtr [1] and auMessagesPtr size enough for one entry. Expected: GT_OK.
1.2. Call function with numOfAuPtr [NULL] and non-NULL auMessagesPtr. Expected: GT_BAD_PTR.
1.3. Call function with numOfAuPtr [1] and auMessagesPtr [NULL]. Expected: GT_BAD_PTR.
*/

    GT_STATUS st = GT_OK;

    GT_U8                       dev;
    GT_U32                      numOfAu;
    CPSS_EXMX_ADDR_UP_MSG_STC   auMessages[1];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with numOfAuPtr [1] and auMessagesPtr size */
        /* enough for one entry. Expected: GT_OK.                        */
        numOfAu = 1;

        st = cpssExMxBrgFdbAuMsgBlockGet(dev, &numOfAu, auMessages);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with numOfAuPtr [NULL] and non-NULL   */
        /* auMessagesPtr. Expected: GT_BAD_PTR.                     */
        st = cpssExMxBrgFdbAuMsgBlockGet(dev, NULL, auMessages);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, numOfAuPtr = NULL",  dev);

        /* 1.3. Call function with numOfAuPtr [1] and auMessagesPtr [NULL]. */
        /* Expected: GT_BAD_PTR.                                            */
        numOfAu = 1;

        st = cpssExMxBrgFdbAuMsgBlockGet(dev, &numOfAu, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, auMessagesPtr = NULL",  dev);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */
    numOfAu = 1;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbAuMsgBlockGet(dev, &numOfAu, auMessages);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbAuMsgBlockGet(dev, &numOfAu, auMessages);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbHashOptionsSet
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_HASH_OPTION_STC     *hashOptParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbHashOptionsSet)
{
/*
ITERATE_DEVICES 
1.1. Call function cpssExMxBrgFdbHashOptionsGet with non-NULL hashOptParamsPtr  and store current configuration.
Expected: GT_OK.
1.2. Call function with hashOptParamsPtr [{macLookupMask [0xFFFFFF000000], vidLookupMask [0x0F00],macShiftLeft [5],vidShiftLeft [2]}].
Expected: GT_OK.
1.3. Call function cpssExMxBrgFdbHashOptionsGet with non-NULL hashOptParamsPtr.
Expected: GT_OK and FDB hash options parameters the same as just written.
1.4. Call function with out of range vidLookupMask [0xFF00] and other parameters the same as in 1.2.
Expected: NON GT_OK.
1.5. Call function with out of range macShiftLeft [6] and other parameters the same as in 1.2.
Expected: NON GT_OK.
1.6. Call function with out of range vidShiftLeft [3] and other parameters the same as in 1.2.
Expected: NON GT_OK.
1.7. Restore FDB hash options parameters. Call function with preserved in 1.1. values.
Expected: GT_OK.
1.8. Call function with hashOptParamsPtr [NULL].
Expected: GT_BAD_PTR.
*/

    GT_STATUS st = GT_OK;

    GT_U8                           dev;
    CPSS_FDB_HASH_OPTION_STC        hashOptParams;
    CPSS_FDB_HASH_OPTION_STC        retHashOptParams;
    CPSS_FDB_HASH_OPTION_STC        origHashOptParams;

    GT_BOOL                         failureWas;

    cpssOsBzero((GT_VOID*)&hashOptParams, sizeof(hashOptParams));
    hashOptParams.macLookupMask.arEther[5] = 0xFF;
    hashOptParams.macLookupMask.arEther[4] = 0xFF;
    hashOptParams.macLookupMask.arEther[3] = 0xFF;
    hashOptParams.macShiftLeft = 5;
    hashOptParams.vidLookupMask = 0x0F00;
    hashOptParams.vidShiftLeft = 2;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function cpssExMxBrgFdbHashOptionsGet with non-NULL
        hashOptParamsPtr  and store current configuration. Expected: GT_OK. */
        st = cpssExMxBrgFdbHashOptionsGet(dev, &origHashOptParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "cpssExMxBrgFdbHashOptionsGet: %d. Cannot restore original FDB hash options.", dev);

        /* 1.2. Call function with hashOptParamsPtr [{macLookupMask
        [0xFFFFFF000000], vidLookupMask [0x0F00],macShiftLeft [5],vidShiftLeft [2]}].
        Expected: GT_OK. */
        st = cpssExMxBrgFdbHashOptionsSet(dev, &hashOptParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.3. Call function cpssExMxBrgFdbHashOptionsGet with non-NULL
        hashOptParamsPtr. Expected: GT_OK and FDB hash options parameters the same
        as just written. */
        st = cpssExMxBrgFdbHashOptionsGet(dev, &retHashOptParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                                     "cpssExMxBrgFdbHashOptionsGet: %d", dev);
        if(GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&hashOptParams,
                                            (const GT_VOID*)&retHashOptParams, sizeof(hashOptParams))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, failureWas,
                "get another FDB Hash Options than has been written [dev = %d]", dev);
        }

        /* 1.4. Call function with out of range vidLookupMask [0xFF00] and
        other parameters the same as in 1.2. Expected: NON GT_OK. */
        hashOptParams.vidLookupMask = 0xFF00;

        st = cpssExMxBrgFdbHashOptionsSet(dev, &hashOptParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, hashOptParams.vidLookupMask = %d",
                                        dev, hashOptParams.vidLookupMask);

        hashOptParams.vidLookupMask = 0x0F00; /* restore */

        /* 1.5. Call function with out of range macShiftLeft [6] and
        other parameters the same as in 1.2. Expected: NON GT_OK. */
        hashOptParams.macShiftLeft = 6;

        st = cpssExMxBrgFdbHashOptionsSet(dev, &hashOptParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, hashOptParams.macShiftLeft = %d",
                                        dev, hashOptParams.macShiftLeft);

        hashOptParams.macShiftLeft = 5; /* restore */

        /* 1.6. Call function with out of range vidShiftLeft [3] and
        other parameters the same as in 1.2. Expected: NON GT_OK. */
        hashOptParams.vidShiftLeft = 3;

        st = cpssExMxBrgFdbHashOptionsSet(dev, &hashOptParams);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, hashOptParams.vidShiftLeft = %d",
                                        dev, hashOptParams.vidShiftLeft);

        hashOptParams.vidShiftLeft = 2; /* restore */

        /* 1.7. Restore FDB hash options parameters. Call function with
        preserved in 1.1. values. Expected: GT_OK. */
        st = cpssExMxBrgFdbHashOptionsSet(dev, &origHashOptParams);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
            "%d. Could not restore original FDB hash options.", dev);

        /* 1.8. Call function with hashOptParamsPtr [NULL].
        Expected: GT_BAD_PTR. */
        st = cpssExMxBrgFdbHashOptionsSet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbHashOptionsSet(dev, &hashOptParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbHashOptionsSet(dev, &hashOptParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxBrgFdbHashOptionsGet
(
    IN  GT_U8                       devNum,
    OUT CPSS_FDB_HASH_OPTION_STC    *hashOptParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxBrgFdbHashOptionsGet)
{
/*
ITERATE_DEVICES 
1.1. Call function with non-NULL hashOptParamsPtr.
Expected: GT_OK.
1.2. Call function with hashOptParamsPtr [NULL].
Expected: GT_BAD_PTR.
*/

    GT_STATUS st = GT_OK;

    GT_U8                           dev;
    CPSS_FDB_HASH_OPTION_STC        retHashOptParams;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL hashOptParamsPtr. Expected: GT_OK. */
        st = cpssExMxBrgFdbHashOptionsGet(dev, &retHashOptParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* 1.2. Call function with hashOptParamsPtr [NULL].
        Expected: GT_BAD_PTR. */
        st = cpssExMxBrgFdbHashOptionsGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    /* 2. For not active devices check that function returns GT_BAD_PARAM. */
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxBrgFdbHashOptionsGet(dev, &retHashOptParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxBrgFdbHashOptionsGet(dev, &retHashOptParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cppExMxBrgFdb suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxBrgFdb)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbAgingTimeoutSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacAddrEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacAddrEntryDel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacTriggerModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacTriggerToggle)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbStaticTransEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbStaticDelEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbAgeWithoutRemovalEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacVlanLookupModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacAddrLookupModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbPortLearnSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbPortLearnGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacFilterPortEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbNaToCpuEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacUpd2CpuEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbAuToCpuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbAuToCpuGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbFlushPortEmulate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbFlush)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMaxLookupLenSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacAddrEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbL2IngrProcEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbPortAuToCpuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacFilterEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacFilterEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbDeviceTableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbTransplant)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacAddrEntryWrite)
/* #if 0 the caller need to implement get AU message */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbQaSend)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbQiSend)
/* #endif the caller need to implement get AU message */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacAddrEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbTrunkAgingModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacAddrRawEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacAddrRawEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacActionsEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbMacActionsEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbAuMsgBlockGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbHashOptionsSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxBrgFdbHashOptionsGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxBrgFdb)

