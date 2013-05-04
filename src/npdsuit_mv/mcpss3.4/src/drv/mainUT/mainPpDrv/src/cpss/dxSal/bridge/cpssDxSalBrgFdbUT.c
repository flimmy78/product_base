/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxSalBrgFdbUT.c
*
* DESCRIPTION:
*       Unit tests for cpssDxSalBrgFdb, that provides
*       MAC Address table facility DXSAL CPSS declarations.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/dxSal/bridge/cpssDxSalBrgFdb.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxSal/config/private/prvCpssDxSalInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define BRG_FDB_INVALID_ENUM_CNS        0x5AAAAAA5

/* Default valid value for physical port id */
#define BRG_FDB_VALID_PHY_PORT_CNS      0

/* Maximum value for trunk id. The max value from all searched sources where taken   */
#define MAX_TRUNK_ID_CNS                128

/* valid vlan value */
#define PRV_CPSS_VALID_VLANS_CNS        100

/* the number of entries in the "mac range filter" table*/
#define MAC_RANGE_FILTER_TABLE_SIZE_CNS 8

/* Internal functions forward declaration */
static GT_STATUS prvUtfMacTableSize(IN GT_U8 dev, OUT GT_U32* tableSizePtr);


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMacAddrEntrySet
(
    IN GT_U8                dev,
    IN CPSS_MAC_ENTRY_STC  *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMacAddrEntrySet)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with non-NULL entryPtr {macAddr [{0xAA,0xBB,0,0,0,0x01}]; dstInterface {type [CPSS_INTERFACE_PORT_E / CPSS_INTERFACE_TRUNK_E / CPSS_INTERFACE_VIDX_E]; devPort {devNum [dev]; portNum[0]}; trunkId [2]; vidx [100]}; isStatic [GT_TRUE / GT_FALSE]; vlanId [100]; srcTc [0 / 1 / 3]; dstTc [0 / 1 / 3]; daCommand [CPSS_MAC_TABLE_FRWRD_E / CPSS_MAC_TABLE_DROP_E / CPSS_MAC_TABLE_INTERV_E]; saCommand [CPSS_MAC_TABLE_FRWRD_E / CPSS_MAC_TABLE_DROP_E / CPSS_MAC_TABLE_INTERV_E]; saClass [GT_TRUE]; daClass [GT_TRUE]; saCib [GT_TRUE]; daCib [GT_TRUE]; daRoute [GT_TRUE];  cosIpv4En [GT_FALSE];  mirrorToRxAnalyzerPortEn [GT_TRUE]}.
    Expected: GT_OK. 
    1.2. Call function with entryPtr->daCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E / CPSS_MAC_TABLE_SOFT_DROP_E] (not supported) and other parameters the same as in 1.1.
    Expected: NOT GT_OK. 
    1.3. Call function with entryPtr->saCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E / CPSS_MAC_TABLE_SOFT_DROP_E] (not supported) and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.4. Call function with out of range entryPtr->dstInterface.type [0x5AAAAAA5] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E], out of range entryPtr-> dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.6. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E], out of range entryPtr-> dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.7. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E], out of range entryPtr -> dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.8. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E], out of range entryPtr -> dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant) and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.9. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E], out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] 
    and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.10. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E], out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] (not relevant) and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.11. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E], out of range entryPtr->dstInterface.vidx [0xFFFF] and other parameters the same as in 1.1.
    Expected: NON GT_OK 
    1.12. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E], out of range entryPtr->dstInterface.vidx [0xFFFF] (not relevant) and other parameters the same as in 1.1.
    Expected: GT_OK 
    1.13. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VID_E] (not supported by function's contract) and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.14. Call function with with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E], out of range entryPtr->dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant) and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.15. Call function with out of range entryPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.16. Call function with out of range entryPtr->srcTc [CPSS_4_TC_RANGE_CNS] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.17. Call function with out of range entryPtr->dstTc [CPSS_4_TC_RANGE_CNS] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.18. Call function with out of range entryPtr->saCommand [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
    1.19. Call function with out of range entryPtr->daCommand [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
    1.20. Call function with entryPtr [NULL].
    Expected: GT_BAD_PTR. 
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    CPSS_MAC_ENTRY_STC entry;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL entryPtr {macAddr [{0xAA,0xBB,0,0,0,0x01}];
                                                       dstInterface {type [CPSS_INTERFACE_PORT_E /
                                                                           CPSS_INTERFACE_TRUNK_E /
                                                                           CPSS_INTERFACE_VIDX_E];
                                                                     devPort {devNum [dev];
                                                                              portNum[0]};
                                                                     trunkId [2];
                                                                     vidx [100]};
                                                       isStatic [GT_TRUE];
                                                       vlanId [100];
                                                       srcTc [0 / 1 / 3];
                                                       dstTc [0 / 1 / 3];
                                                       daCommand [CPSS_MAC_TABLE_FRWRD_E /
                                                                  CPSS_MAC_TABLE_DROP_E /
                                                                  CPSS_MAC_TABLE_INTERV_E];
                                                       saCommand [CPSS_MAC_TABLE_FRWRD_E /
                                                                  CPSS_MAC_TABLE_DROP_E /
                                                                  CPSS_MAC_TABLE_INTERV_E];
                                                       saClass [GT_TRUE];
                                                       daClass [GT_TRUE];
                                                       saCib [GT_TRUE];
                                                       daCib [GT_TRUE];
                                                       daRoute [GT_TRUE];
                                                       cosIpv4En [GT_FALSE];
                                                       mirrorToRxAnalyzerPortEn [GT_TRUE]}.
            Expected: GT_OK. 
        */
        entry.macAddr.arEther[0] = 0xAA;
        entry.macAddr.arEther[1] = 0xBB;
        entry.macAddr.arEther[2] = 0;
        entry.macAddr.arEther[3] = 0;
        entry.macAddr.arEther[4] = 0;
        entry.macAddr.arEther[5] = 1;

        entry.isStatic  = GT_TRUE;
        entry.vlanId    = PRV_CPSS_VALID_VLANS_CNS;
        entry.saClass   = GT_TRUE;
        entry.daClass   = GT_TRUE;
        entry.saCib     = GT_TRUE;
        entry.daCib     = GT_TRUE;
        entry.daRoute   = GT_TRUE;
        entry.cosIpv4En = GT_FALSE;
        entry.mirrorToRxAnalyzerPortEn = GT_TRUE;

        /* Call with type [CPSS_INTERFACE_PORT_E], srcTc [0], dstTc [0],         */
        /* daCommand [CPSS_MAC_TABLE_FRWRD_E], saCommand [CPSS_MAC_TABLE_FRWRD_E */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.devPort.devNum  = dev;
        entry.dstInterface.devPort.portNum = 0;

        entry.srcTc = 0;
        entry.dstTc = 0;

        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with type [CPSS_INTERFACE_TRUNK_E], srcTc [1], dstTc [1],      */
        /* daCommand [CPSS_MAC_TABLE_DROP_E], saCommand [CPSS_MAC_TABLE_DROP_E */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        entry.dstInterface.trunkId = 2;

        entry.srcTc = 1;
        entry.dstTc = 1;

        entry.daCommand = CPSS_MAC_TABLE_DROP_E;
        entry.saCommand = CPSS_MAC_TABLE_DROP_E;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with type [CPSS_INTERFACE_VIDX_E], srcTc [3], dstTc [3],           */
        /* daCommand [CPSS_MAC_TABLE_INTERV_E], saCommand [CPSS_MAC_TABLE_INTERV_E */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        entry.srcTc = 3;
        entry.dstTc = 3;

        entry.daCommand = CPSS_MAC_TABLE_INTERV_E;
        entry.saCommand = CPSS_MAC_TABLE_INTERV_E;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with entryPtr->daCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E /
                                                         CPSS_MAC_TABLE_SOFT_DROP_E] (not supported)
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK. 
        */
        entry.daCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryPtr->daCommand = %d", dev, entry.daCommand);

        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.3. Call function with entryPtr->saCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E /
                                                         CPSS_MAC_TABLE_SOFT_DROP_E] (not supported)
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK. 
        */
        entry.saCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryPtr->saCommand = %d", dev, entry.saCommand);

        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.4. Call function with out of range entryPtr->dstInterface.type [0x5AAAAAA5]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.dstInterface.type = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->dstInterface = %d", dev, entry.dstInterface.type);

        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.5. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E],
                                    out of range entryPtr->dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.devPort.devNum = %d",
                                         dev, entry.dstInterface.type, entry.dstInterface.devPort.devNum);

        entry.dstInterface.devPort.devNum = dev;

        /*
            1.6. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E],
                                    out of range entryPtr->dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        entry.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.devPort.devNum = %d",
                                    dev, entry.dstInterface.type, entry.dstInterface.devPort.devNum);

        entry.dstInterface.devPort.devNum = dev;

        /*
            1.7. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E],
                                    out of range entryPtr->dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.devPort.portNum = %d",
                                        dev, entry.dstInterface.type, entry.dstInterface.devPort.portNum);

        entry.dstInterface.devPort.portNum = 0;

        /*
            1.8. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E],
                                    out of range entryPtr->dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (not relevant)
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        entry.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.devPort.portNum = %d",
                                    dev, entry.dstInterface.type, entry.dstInterface.devPort.portNum);

        entry.dstInterface.devPort.portNum = 0;

        /*
            1.9. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E],
                                    out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] 
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        entry.dstInterface.trunkId = MAX_TRUNK_ID_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.trunkId = %d",
                                        dev, entry.dstInterface.type, entry.dstInterface.trunkId);

        entry.dstInterface.trunkId = 2;

        /*
            1.10. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E],
                                     out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] (not relevant)
                                     and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.trunkId = MAX_TRUNK_ID_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.trunkId = %d",
                                    dev, entry.dstInterface.type, entry.dstInterface.trunkId);

        entry.dstInterface.trunkId = 2;

        /*
            1.11. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E],
                                     out of range entryPtr->dstInterface.vidx [0xFFFF]
                                     and other parameters the same as in 1.1.
            Expected: NON GT_OK 
        */
        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        entry.dstInterface.vidx = 0xFFFF;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.vidx = %d",
                                        dev, entry.dstInterface.type, entry.dstInterface.vidx);

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.12. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E],
                                     out of range entryPtr->dstInterface.vidx [0xFFFF] (not relevant)
                                     and other parameters the same as in 1.1.
            Expected: GT_OK 
        */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        entry.dstInterface.vidx = 0xFFFF;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.vidx = %d",
                                    dev, entry.dstInterface.type, entry.dstInterface.vidx);

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.13. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VID_E] (not supported by function's contract)
                                     and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_VID_E;

        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d", dev, entry.dstInterface.type);

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.14. Call function with with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E],
                                     out of range entryPtr->dstInterface.vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)
                                     and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        entry.dstInterface.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.vlanId = %d",
                                    dev, entry.dstInterface.type, entry.dstInterface.vlanId);

        /*
            1.15. Call function with out of range entryPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->vlanId = %d", dev, entry.vlanId);

        entry.vlanId = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.16. Call function with out of range entryPtr->srcTc [CPSS_4_TC_RANGE_CNS]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.srcTc = CPSS_4_TC_RANGE_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->srcTc = %d", dev, entry.srcTc);

        entry.srcTc = 0;

        /*
            1.17. Call function with out of range entryPtr->dstTc [CPSS_4_TC_RANGE_CNS]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.dstTc = CPSS_4_TC_RANGE_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->dstTc = %d", dev, entry.dstTc);

        entry.dstTc = 0;

        /*
            1.18. Call function with out of range entryPtr->saCommand [0x5AAAAAA5]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.saCommand = BRG_FDB_INVALID_ENUM_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->saCommand = %d", dev, entry.saCommand);

        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.19. Call function with out of range entryPtr->daCommand [0x5AAAAAA5]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.daCommand = BRG_FDB_INVALID_ENUM_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->daCommand = %d", dev, entry.daCommand);

        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.20. Call function with entryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryPtr = NULL", dev);
    }

    entry.macAddr.arEther[0] = 0xAA;
    entry.macAddr.arEther[1] = 0xBB;
    entry.macAddr.arEther[2] = 0;
    entry.macAddr.arEther[3] = 0;
    entry.macAddr.arEther[4] = 0;
    entry.macAddr.arEther[5] = 1;

    entry.isStatic  = GT_TRUE;
    entry.vlanId    = PRV_CPSS_VALID_VLANS_CNS;
    entry.saClass   = GT_TRUE;
    entry.daClass   = GT_TRUE;
    entry.saCib     = GT_TRUE;
    entry.daCib     = GT_TRUE;
    entry.daRoute   = GT_TRUE;
    entry.cosIpv4En = GT_FALSE;
    entry.mirrorToRxAnalyzerPortEn = GT_TRUE;

    entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

    entry.dstInterface.devPort.devNum  = dev;
    entry.dstInterface.devPort.portNum = 0;

    entry.srcTc = 0;
    entry.dstTc = 0;

    entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
    entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalFdbBrgCosIpv4EnWABitSet
(
    IN GT_U8                dev,
    IN GT_U32               macIndex
)
*/
UTF_TEST_CASE_MAC(cpssDxSalFdbBrgCosIpv4EnWABitSet)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with macIndex [ 0 / macTableSize(dev) - 1].
    Expected: GT_OK.
    1.2. Call function with out of range macIndex [macTableSize(dev)].
    Expected: NON GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U32    macIndex    = 0;
    GT_U32    maxMacIndex = 0;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting mac table size */
        st = prvUtfMacTableSize(dev, &maxMacIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMacTableSize: %d", dev);

        /* 
            1.1. Call function with macIndex [ 0 / macTableSize(dev) - 1].
            Expected: GT_OK.
        */

        /* Call with macIndex[0] */
        macIndex = 0;
        
        st = cpssDxSalFdbBrgCosIpv4EnWABitSet(dev, macIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, macIndex);

        /* Call with macIndex[macTableSize(dev) - 1] */
        macIndex = maxMacIndex - 1;
        
        st = cpssDxSalFdbBrgCosIpv4EnWABitSet(dev, macIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, macIndex);

        /*
            1.2. Call function with out of range macIndex [macTableSize(dev)].
            Expected: NON GT_OK.
        */
        macIndex = maxMacIndex;
        
        st = cpssDxSalFdbBrgCosIpv4EnWABitSet(dev, macIndex);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, macIndex);

        macIndex = 0;
    }

    macIndex = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalFdbBrgCosIpv4EnWABitSet(dev, macIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalFdbBrgCosIpv4EnWABitSet(dev, macIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMacAddrEntryDel
(
    IN GT_U8                dev,
    IN CPSS_MAC_ENTRY_STC  *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMacAddrEntryDel)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function cpssDxSalBrgFdbMacAddrEntrySet with non-NULL entryPtr {macAddr [{0xAA,0xBB,0,0,0,0x01}]; dstInterface {type [CPSS_INTERFACE_PORT_E / CPSS_INTERFACE_TRUNK_E / CPSS_INTERFACE_VIDX_E]; devPort {devNum [dev]; portNum[0]}; trunkId [2]; vidx [100]}; isStatic [GT_TRUE]; vlanId [100]; srcTc [0 / 1 / 3]; dstTc [0 / 1 / 3]; daCommand [CPSS_MAC_TABLE_FRWRD_E / CPSS_MAC_TABLE_DROP_E / CPSS_MAC_TABLE_INTERV_E / CPSS_MAC_TABLE_CNTL_E]; saCommand [CPSS_MAC_TABLE_FRWRD_E / CPSS_MAC_TABLE_DROP_E / CPSS_MAC_TABLE_INTERV_E / CPSS_MAC_TABLE_CNTL_E]; saClass [GT_TRUE]; daClass [GT_TRUE]; saCib [GT_TRUE]; daCib [GT_TRUE]; daRoute [GT_TRUE];  cosIpv4En [GT_FALSE];  mirrorToRxAnalyzerPortEn [GT_TRUE]} to add entry.
    Expected: GT_OK. 
    1.2. Call function with out of range entryPtr->dstInterface.type [0x5AAAAAA5] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E], out of range entryPtr-> dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.4. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E], out of range entryPtr-> dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant) and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.5. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E], out of range entryPtr -> dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.6. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E], out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] (not relevant)
    and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.7. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E], out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] 
    and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.8. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E], out of range entryPtr->dstInterface.vidx [0xFFFF] (not relevant) and other parameters the same as in 1.1.
    Expected: GT_OK 
    1.9. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E], out of range entryPtr->dstInterface.vidx [0xFFFF] and other parameters the same as in 1.1.
    Expected: NOT GT_OK 
    1.10. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VID_E] (not supported) and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.11. Call function with out of range entryPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.12. Call function with out of range entryPtr->srcTc [CPSS_4_TC_RANGE_CNS] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.13. Call function with out of range entryPtr->dstTc [CPSS_4_TC_RANGE_CNS] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.14. Call function with entryPtr->daCommand [CPSS_MAC_TABLE_SOFT_DROP_E ] and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.15. Call function with out of range entryPtr->daCommand [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
    1.16. Call function with entryPtr->saCommand [CPSS_MAC_TABLE_SOFT_DROP_E ] and other parameters the same as in 1.1. Expected: NON GT_OK.
    1.17. Call function with out of range entryPtr->saCommand [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
    1.18. Call function with entryPtr [NULL]. Expected: GT_BAD_PTR. 
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    CPSS_MAC_ENTRY_STC entry;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function cpssDxSalBrgFdbMacAddrEntrySet with non-NULL entryPtr {macAddr [{0xAA,0xBB,0,0,0,0x01}];
                                                       dstInterface {type [CPSS_INTERFACE_PORT_E /
                                                                           CPSS_INTERFACE_TRUNK_E /
                                                                           CPSS_INTERFACE_VIDX_E];
                                                                     devPort {devNum [dev];
                                                                              portNum[0]};
                                                                     trunkId [2];
                                                                     vidx [100]};
                                                       isStatic [GT_TRUE];
                                                       vlanId [100];
                                                       srcTc [0 / 1 / 3];
                                                       dstTc [0 / 1 / 3];
                                                       daCommand [CPSS_MAC_TABLE_FRWRD_E /
                                                                  CPSS_MAC_TABLE_DROP_E /
                                                                  CPSS_MAC_TABLE_INTERV_E];
                                                       saCommand [CPSS_MAC_TABLE_FRWRD_E /
                                                                  CPSS_MAC_TABLE_DROP_E /
                                                                  CPSS_MAC_TABLE_INTERV_E];
                                                       saClass [GT_TRUE];
                                                       daClass [GT_TRUE];
                                                       saCib [GT_TRUE];
                                                       daCib [GT_TRUE];
                                                       daRoute [GT_TRUE];
                                                       cosIpv4En [GT_FALSE];
                                                       mirrorToRxAnalyzerPortEn [GT_TRUE]}.
            Expected: GT_OK. 
        */
        entry.macAddr.arEther[0] = 0xAA;
        entry.macAddr.arEther[1] = 0xBB;
        entry.macAddr.arEther[2] = 0;
        entry.macAddr.arEther[3] = 0;
        entry.macAddr.arEther[4] = 0;
        entry.macAddr.arEther[5] = 1;

        entry.isStatic  = GT_TRUE;
        entry.vlanId    = PRV_CPSS_VALID_VLANS_CNS;
        entry.saClass   = GT_TRUE;
        entry.daClass   = GT_TRUE;
        entry.saCib     = GT_TRUE;
        entry.daCib     = GT_TRUE;
        entry.daRoute   = GT_TRUE;
        entry.cosIpv4En = GT_FALSE;
        entry.mirrorToRxAnalyzerPortEn = GT_TRUE;

        /* Call with type [CPSS_INTERFACE_PORT_E], srcTc [0], dstTc [0],         */
        /* daCommand [CPSS_MAC_TABLE_FRWRD_E], saCommand [CPSS_MAC_TABLE_FRWRD_E */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.devPort.devNum  = dev;
        entry.dstInterface.devPort.portNum = 0;

        entry.srcTc = 0;
        entry.dstTc = 0;

        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with type [CPSS_INTERFACE_TRUNK_E], srcTc [1], dstTc [1],      */
        /* daCommand [CPSS_MAC_TABLE_DROP_E], saCommand [CPSS_MAC_TABLE_DROP_E */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        entry.dstInterface.trunkId = 2;

        entry.srcTc = 1;
        entry.dstTc = 1;

        entry.daCommand = CPSS_MAC_TABLE_DROP_E;
        entry.saCommand = CPSS_MAC_TABLE_DROP_E;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /* Call with type [CPSS_INTERFACE_VIDX_E], srcTc [3], dstTc [3],           */
        /* daCommand [CPSS_MAC_TABLE_INTERV_E], saCommand [CPSS_MAC_TABLE_INTERV_E */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        entry.srcTc = 3;
        entry.dstTc = 3;

        entry.daCommand = CPSS_MAC_TABLE_INTERV_E;
        entry.saCommand = CPSS_MAC_TABLE_INTERV_E;
        
        st = cpssDxSalBrgFdbMacAddrEntrySet(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with out of range entryPtr->dstInterface.type [0x5AAAAAA5]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.dstInterface.type = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->dstInterface = %d", dev, entry.dstInterface.type);

        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.3. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E],
                                    out of range entryPtr->dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.devPort.devNum = %d",
                                        dev, entry.dstInterface.type, entry.dstInterface.devPort.devNum);

        entry.dstInterface.devPort.devNum = dev;

        /*
            1.4. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E],
                                    out of range entryPtr->dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        entry.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.devPort.devNum = %d",
                                    dev, entry.dstInterface.type, entry.dstInterface.devPort.devNum);

        entry.dstInterface.devPort.devNum = dev;

        /*
            1.5. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E],
                                    out of range entryPtr->dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.devPort.portNum = %d",
                                        dev, entry.dstInterface.type, entry.dstInterface.devPort.portNum);

        entry.dstInterface.devPort.portNum = 0;

        /*
            1.6. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E],
                                     out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] (not relevant)
                                     and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.trunkId = MAX_TRUNK_ID_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.trunkId = %d",
                                    dev, entry.dstInterface.type, entry.dstInterface.trunkId);

        entry.dstInterface.trunkId = 2;

        /*
            1.7. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E],
                                    out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] 
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        entry.dstInterface.trunkId = MAX_TRUNK_ID_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.trunkId = %d",
                                        dev, entry.dstInterface.type, entry.dstInterface.trunkId);

        entry.dstInterface.trunkId = 2;

        /*
            1.8. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E],
                                     out of range entryPtr->dstInterface.vidx [0xFFFF] (not relevant)
                                     and other parameters the same as in 1.1.
            Expected: GT_OK 
        */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        entry.dstInterface.vidx = 0xFFFF;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.vidx = %d",
                                    dev, entry.dstInterface.type, entry.dstInterface.vidx);

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.9. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E],
                                     out of range entryPtr->dstInterface.vidx [0xFFFF]
                                     and other parameters the same as in 1.1.
            Expected: NON GT_OK 
        */
        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        entry.dstInterface.vidx = 0xFFFF;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.vidx = %d",
                                        dev, entry.dstInterface.type, entry.dstInterface.vidx);

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.10. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VID_E] (not supported by function's contract)
                                     and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_VID_E;

        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d", dev, entry.dstInterface.type);

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.11. Call function with out of range entryPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->vlanId = %d", dev, entry.vlanId);

        entry.vlanId = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.12. Call function with out of range entryPtr->srcTc [CPSS_4_TC_RANGE_CNS]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.srcTc = CPSS_4_TC_RANGE_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->srcTc = %d", dev, entry.srcTc);

        entry.srcTc = 0;

        /*
            1.13. Call function with out of range entryPtr->dstTc [CPSS_4_TC_RANGE_CNS]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.dstTc = CPSS_4_TC_RANGE_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->dstTc = %d", dev, entry.dstTc);

        entry.dstTc = 0;

        /*
            1.14. Call function with entryPtr->daCommand [CPSS_MAC_TABLE_SOFT_DROP_E] (not supported)
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK. 
        */
        entry.daCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryPtr->daCommand = %d", dev, entry.daCommand);

        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.15. Call function with out of range entryPtr->daCommand [0x5AAAAAA5]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.daCommand = BRG_FDB_INVALID_ENUM_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->daCommand = %d", dev, entry.daCommand);

        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.16. Call function with entryPtr->saCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E /
                                                         CPSS_MAC_TABLE_SOFT_DROP_E] (not supported)
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK. 
        */
        entry.saCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryPtr->saCommand = %d", dev, entry.saCommand);

        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.17. Call function with out of range entryPtr->saCommand [0x5AAAAAA5]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.saCommand = BRG_FDB_INVALID_ENUM_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->saCommand = %d", dev, entry.saCommand);

        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.20. Call function with entryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryPtr = NULL", dev);
    }

    entry.macAddr.arEther[0] = 0xAA;
    entry.macAddr.arEther[1] = 0xBB;
    entry.macAddr.arEther[2] = 0;
    entry.macAddr.arEther[3] = 0;
    entry.macAddr.arEther[4] = 0;
    entry.macAddr.arEther[5] = 1;

    entry.isStatic  = GT_TRUE;
    entry.vlanId    = PRV_CPSS_VALID_VLANS_CNS;
    entry.saClass   = GT_TRUE;
    entry.daClass   = GT_TRUE;
    entry.saCib     = GT_TRUE;
    entry.daCib     = GT_TRUE;
    entry.daRoute   = GT_TRUE;
    entry.cosIpv4En = GT_FALSE;
    entry.mirrorToRxAnalyzerPortEn = GT_TRUE;

    entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

    entry.dstInterface.devPort.devNum  = dev;
    entry.dstInterface.devPort.portNum = 0;

    entry.srcTc = 0;
    entry.dstTc = 0;

    entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
    entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbMacAddrEntryDel(dev, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMacVlanLookupModeSet
(
    IN GT_U8                dev,
    IN CPSS_MAC_VL_ENT      mode
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMacVlanLookupModeSet)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with mode [CPSS_IVL_E / CPSS_SVL_E].
    Expected: GT_OK.
    1.2. Call function with out of range mode [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    CPSS_MAC_VL_ENT mode = CPSS_IVL_E;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with mode [CPSS_IVL_E / CPSS_SVL_E].
            Expected: GT_OK.
        */

        /* Call with mode [CPSS_IVL_E] */
        mode = CPSS_IVL_E;
        
        st = cpssDxSalBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /* Call with mode [CPSS_SVL_E] */
        mode = CPSS_SVL_E;
        
        st = cpssDxSalBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, mode);

        /*
            1.2. Call function with out of range mode [0x5AAAAAA5].
            Expected: GT_BAD_PARAM. 
        */
        mode = BRG_FDB_INVALID_ENUM_CNS;
        
        st = cpssDxSalBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, mode);

        mode = CPSS_IVL_E;
    }

    mode = CPSS_IVL_E;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMacVlanLookupModeSet(dev, mode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbMacVlanLookupModeSet(dev, mode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbPortLearnSet
(
    IN GT_U8                    dev,
    IN GT_U8                    port,
    IN GT_BOOL                  status,
    IN CPSS_PORT_LOCK_CMD_ENT   cmd
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbPortLearnSet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxSal)
    1.1. Call function with status [GT_TRUE / GT_FALSE]
                            and cmd [CPSS_LOCK_FRWRD_E /
                                     CPSS_LOCK_DROP_E /
                                     CPSS_LOCK_TRAP_E /
                                     CPSS_LOCK_MIRROR_E]
                       when status = GT_FALSE.
    Expected: GT_OK.
    1.2. Call cpssDxSalBrgFdbPortLearnStatusGet with non-null statusPtr to get current status.
    Expected: GT_OK and same status.
    1.3. Call function with satus [GT_FALSE]
                            and cmd [CPSS_MAC_TABLE_SOFT_DROP_E] (not supported).
    Expected: NON GT_OK.
    1.4. Call function with satus [GT_FALSE]
                            and out of range cmd [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
    1.5. Call function with satus [GT_TRUE]
                            and out of range cmd [0x5AAAAAA5] (not relevant with this status).
    Expected: GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev; 
    GT_U8     port = BRG_FDB_VALID_PHY_PORT_CNS;

    GT_BOOL                status    = GT_FALSE;
    CPSS_PORT_LOCK_CMD_ENT cmd       = CPSS_LOCK_FRWRD_E;
    GT_BOOL                statusGet = GT_FALSE;
    
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
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
                1.1. Call function with status [GT_TRUE / GT_FALSE]
                                        and cmd [CPSS_LOCK_FRWRD_E /
                                                 CPSS_LOCK_DROP_E /
                                                 CPSS_LOCK_TRAP_E /
                                                 CPSS_LOCK_MIRROR_E]
                                   when status = GT_FALSE.
                Expected: GT_OK.
            */

            /* Call with status [GT_FALSE] and cmd [CPSS_LOCK_FRWRD_E] */
            status = GT_FALSE;
            cmd    = CPSS_LOCK_FRWRD_E;

            st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, status, cmd);

            /*
                1.2. Call cpssDxSalBrgFdbPortLearnStatusGet with non-null statusPtr to get current status.
                Expected: GT_OK and same status.
            */
            st = cpssDxSalBrgFdbPortLearnStatusGet(dev, port, &statusGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbPortLearnStatusGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(status, statusGet,
                       "get another trfClass than was set: %d, %d", dev, port);

            /* Call with status [GT_FALSE] and cmd [CPSS_LOCK_DROP_E] */
            cmd = CPSS_LOCK_DROP_E;

            st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, status, cmd);

            /*
                1.2. Call cpssDxSalBrgFdbPortLearnStatusGet with non-null statusPtr to get current status.
                Expected: GT_OK and same status.
            */
            st = cpssDxSalBrgFdbPortLearnStatusGet(dev, port, &statusGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbPortLearnStatusGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(status, statusGet,
                       "get another trfClass than was set: %d, %d", dev, port);

            /* Call with status [GT_TRUE] and cmd [CPSS_LOCK_TRAP_E] */
            status = GT_TRUE;
            cmd    = CPSS_LOCK_TRAP_E;

            st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, status, cmd);

            /*
                1.2. Call cpssDxSalBrgFdbPortLearnStatusGet with non-null statusPtr to get current status.
                Expected: GT_OK and same status.
            */
            st = cpssDxSalBrgFdbPortLearnStatusGet(dev, port, &statusGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbPortLearnStatusGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(status, statusGet,
                       "get another trfClass than was set: %d, %d", dev, port);

            /* Call with status [GT_TRUE] and cmd [CPSS_LOCK_MIRROR_E] */
            cmd = CPSS_LOCK_MIRROR_E;

            st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, status, cmd);

            /*
                1.2. Call cpssDxSalBrgFdbPortLearnStatusGet with non-null statusPtr to get current status.
                Expected: GT_OK and same status.
            */
            st = cpssDxSalBrgFdbPortLearnStatusGet(dev, port, &statusGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbPortLearnStatusGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(status, statusGet,
                       "get another trfClass than was set: %d, %d", dev, port);
            
            /*
                1.3. Call function with satus [GT_FALSE]
                                        and cmd [CPSS_MAC_TABLE_SOFT_DROP_E] (not supported).
                Expected: NON GT_OK.
            */
            status = GT_FALSE;
            cmd    = CPSS_MAC_TABLE_SOFT_DROP_E;

            st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, port, status, cmd);

            cmd = CPSS_LOCK_FRWRD_E;

            /*
                1.4. Call function with satus [GT_FALSE]
                                        and out of range cmd [0x5AAAAAA5].
                Expected: GT_BAD_PARAM.
            */

            status = GT_FALSE;
            cmd    = BRG_FDB_VALID_PHY_PORT_CNS;

            st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status, cmd);

            cmd = CPSS_LOCK_FRWRD_E;

            /*
                1.5. Call function with satus [GT_TRUE]
                                        and out of range cmd [0x5AAAAAA5] (not relevant with this status).
                Expected: GT_OK.    
            */
            status = GT_TRUE;
            cmd    = BRG_FDB_VALID_PHY_PORT_CNS;

            st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, port, status, cmd);

            cmd = CPSS_LOCK_FRWRD_E;
        }

        status = GT_TRUE;
        cmd    = CPSS_LOCK_FRWRD_E;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    status = GT_TRUE;
    cmd    = CPSS_LOCK_FRWRD_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalBrgFdbPortLearnSet(dev, port, status, cmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbPortLearnStatusGet
(
    IN  GT_U8   dev,
    IN  GT_U8   port,
    OUT GT_BOOL *statusPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbPortLearnStatusGet)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxSal)
    1.1. Call function with non-NULL statusPtr.
    Expected: GT_OK.
    1.2. Call function with statusPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev; 
    GT_U8     port = BRG_FDB_VALID_PHY_PORT_CNS;

    GT_BOOL   status = GT_FALSE;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
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
                1.1. Call function with non-NULL statusPtr.
                Expected: GT_OK.
            */
            st = cpssDxSalBrgFdbPortLearnStatusGet(dev, port, &status);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.2. Call function with statusPtr [NULL].
                Expected: GT_BAD_PTR.   
            */
            st = cpssDxSalBrgFdbPortLearnStatusGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, statusPtr = NULL", dev, port);
        }

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalBrgFdbPortLearnStatusGet(dev, port, &status);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalBrgFdbPortLearnStatusGet(dev, port, &status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgFdbPortLearnStatusGet(dev, port, &status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbPortLearnStatusGet(dev, port, &status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalBrgFdbPortLearnStatusGet(dev, port, &status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMacFilterPortEnable
(
    IN GT_U8   dev,
    IN GT_U8   port,
    IN GT_BOOL status
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMacFilterPortEnable)
{
/*
    ITERATE_DEVICES_PHY_PORT (DxSal)
    1.1. Call function with status [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev; 
    GT_U8     port = BRG_FDB_VALID_PHY_PORT_CNS;

    GT_BOOL   status = GT_FALSE;
    
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
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
                1.1. Call function with status [GT_TRUE / GT_FALSE].
                Expected: GT_OK.    
            */

            /* Call with status [GT_FALSE] */
            status = GT_FALSE;

            st = cpssDxSalBrgFdbMacFilterPortEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, status);

            /* Call with status [GT_TRUE] */
            status = GT_TRUE;

            st = cpssDxSalBrgFdbMacFilterPortEnable(dev, port, status);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, status);
        }

        status = GT_TRUE;

        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available physical ports. */
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssDxSalBrgFdbMacFilterPortEnable(dev, port, status);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssDxSalBrgFdbMacFilterPortEnable(dev, port, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssDxSalBrgFdbMacFilterPortEnable(dev, port, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    status = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_FDB_VALID_PHY_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMacFilterPortEnable(dev, port, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssDxSalBrgFdbMacFilterPortEnable(dev, port, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMacFilterEntrySet
(
    IN GT_U8                dev,
    IN GT_32                entryInd,
    IN GT_ETHERADDR         *macAddrPtr,
    IN GT_ETHERADDR         *macMaskPtr,
    IN CPSS_MAC_FLT_CMD_ENT daCommand,
    IN CPSS_MAC_FLT_CMD_ENT saCommand
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMacFilterEntrySet)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with entryInd [0 / MAC_RANGE_FILTER_TABLE_SIZE_CNS -  1],
                            non-NULL macAddrPtr [{0xAA,0xBB,0,0,0,0x01}],
                            non-NULL macMaskPtr [{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}],
                            daCommand [CPSS_MAC_FLT_FRWRD_E /
                                       CPSS_MAC_FLT_DROP_E /
                                       CPSS_MAC_FLT_CNTL_E /
                                       CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E],
                            saCommand [CPSS_MAC_FLT_FRWRD_E /
                                       CPSS_MAC_FLT_DROP_E /
                                       CPSS_MAC_FLT_CNTL_E /
                                       CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E].
    Expected: GT_OK.
    1.2. Call cpssDxSalBrgFdbMacFilterEntryGet with the same parameters as in 1.1.
    Expected: GT_OK and the same macAddrPtr, macMaskPtr, daCommand and saCommand.
    1.3. Call function with out of range entryInd [MAC_RANGE_FILTER_TABLE_SIZE_CNS]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.4. Call function with macAddrPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call function with macMaskPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call function with out of range daCommand [0x5AAAAAA5]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call function with out of range saCommand [0x5AAAAAA5]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_32                entryInd  = 0;
    GT_ETHERADDR         macAddr;
    GT_ETHERADDR         macMask;
    CPSS_MAC_FLT_CMD_ENT daCommand = CPSS_MAC_FLT_FRWRD_E;
    CPSS_MAC_FLT_CMD_ENT saCommand = CPSS_MAC_FLT_FRWRD_E;
    GT_ETHERADDR         macAddrGet;
    GT_ETHERADDR         macMaskGet;
    CPSS_MAC_FLT_CMD_ENT daCmdGet  = CPSS_MAC_FLT_FRWRD_E;
    CPSS_MAC_FLT_CMD_ENT saCmdGet  = CPSS_MAC_FLT_FRWRD_E;
    GT_BOOL              isEqual   = GT_FALSE;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with entryInd [0 / MAC_RANGE_FILTER_TABLE_SIZE_CNS -  1],
                                    non-NULL macAddrPtr [{0xAA,0xBB,0,0,0,0x01}],
                                    non-NULL macMaskPtr [{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}],
                                    daCommand [CPSS_MAC_FLT_FRWRD_E /
                                               CPSS_MAC_FLT_DROP_E /
                                               CPSS_MAC_FLT_CNTL_E /
                                               CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E],
                                    saCommand [CPSS_MAC_FLT_FRWRD_E /
                                               CPSS_MAC_FLT_DROP_E /
                                               CPSS_MAC_FLT_CNTL_E /
                                               CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E].
            Expected: GT_OK.
        */
        macAddr.arEther[0] = 0xAA;
        macAddr.arEther[1] = 0xBB;
        macAddr.arEther[2] = 0;
        macAddr.arEther[3] = 0;
        macAddr.arEther[4] = 0;
        macAddr.arEther[5] = 1;

        macMask.arEther[0] = 0xFF;
        macMask.arEther[1] = 0xFF;
        macMask.arEther[2] = 0xFF;
        macMask.arEther[3] = 0xFF;
        macMask.arEther[4] = 0xFF;
        macMask.arEther[5] = 0xFF;

        /* Call with entryInd [0], daCommand [CPSS_MAC_FLT_FRWRD_E], saCommand [CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E] */
        entryInd  = 0;
        daCommand = CPSS_MAC_FLT_FRWRD_E;
        saCommand = CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E;
        
        st = cpssDxSalBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask, daCommand, saCommand);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, daCommand = %d, saCommand = %d",
                                     dev, entryInd, daCommand, saCommand);

        /*
            1.2. Call cpssDxSalBrgFdbMacFilterEntryGet with the same parameters as in 1.1.
            Expected: GT_OK and the same macAddrPtr, macMaskPtr, daCommand and saCommand.
        */
        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddrGet, &macMaskGet, &daCmdGet, &saCmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacFilterEntryGet: %d, %d", dev, entryInd);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macAddr, (GT_VOID*) &macAddrGet, sizeof (macAddr)))? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                     "get another macAddr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macMask, (GT_VOID*) &macMaskGet, sizeof (macMask)))? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                     "get another macMask than was set: %d", dev);

        UTF_VERIFY_EQUAL2_STRING_MAC(daCommand, daCmdGet,
                       "get another daCommand than was set: %d, %d", dev, entryInd);
        UTF_VERIFY_EQUAL2_STRING_MAC(saCommand, saCmdGet,
                       "get another saCommand than was set: %d, %d", dev, entryInd);

        /* Call with entryInd [0], daCommand [CPSS_MAC_FLT_DROP_E], saCommand [CPSS_MAC_FLT_CNTL_E] */
        daCommand = CPSS_MAC_FLT_DROP_E;
        saCommand = CPSS_MAC_FLT_CNTL_E;
        
        st = cpssDxSalBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask, daCommand, saCommand);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, daCommand = %d, saCommand = %d",
                                     dev, entryInd, daCommand, saCommand);

        /*
            1.2. Call cpssDxSalBrgFdbMacFilterEntryGet with the same parameters as in 1.1.
            Expected: GT_OK and the same macAddrPtr, macMaskPtr, daCommand and saCommand.
        */
        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddrGet, &macMaskGet, &daCmdGet, &saCmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacFilterEntryGet: %d, %d", dev, entryInd);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macAddr, (GT_VOID*) &macAddrGet, sizeof (macAddr)))? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                     "get another macAddr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macMask, (GT_VOID*) &macMaskGet, sizeof (macMask)))? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                     "get another macMask than was set: %d", dev);

        UTF_VERIFY_EQUAL2_STRING_MAC(daCommand, daCmdGet,
                       "get another daCommand than was set: %d, %d", dev, entryInd);
        UTF_VERIFY_EQUAL2_STRING_MAC(saCommand, saCmdGet,
                       "get another saCommand than was set: %d, %d", dev, entryInd);

        /* Call with entryInd [MAC_RANGE_FILTER_TABLE_SIZE_CNS-1], daCommand [CPSS_MAC_FLT_CNTL_E], saCommand [CPSS_MAC_FLT_DROP_E] */
        entryInd  = MAC_RANGE_FILTER_TABLE_SIZE_CNS - 1;
        daCommand = CPSS_MAC_FLT_CNTL_E;
        saCommand = CPSS_MAC_FLT_DROP_E;
        
        st = cpssDxSalBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask, daCommand, saCommand);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, daCommand = %d, saCommand = %d",
                                     dev, entryInd, daCommand, saCommand);

        /*
            1.2. Call cpssDxSalBrgFdbMacFilterEntryGet with the same parameters as in 1.1.
            Expected: GT_OK and the same macAddrPtr, macMaskPtr, daCommand and saCommand.
        */
        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddrGet, &macMaskGet, &daCmdGet, &saCmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacFilterEntryGet: %d, %d", dev, entryInd);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macAddr, (GT_VOID*) &macAddrGet, sizeof (macAddr)))? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                     "get another macAddr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macMask, (GT_VOID*) &macMaskGet, sizeof (macMask)))? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                     "get another macMask than was set: %d", dev);

        UTF_VERIFY_EQUAL2_STRING_MAC(daCommand, daCmdGet,
                       "get another daCommand than was set: %d, %d", dev, entryInd);
        UTF_VERIFY_EQUAL2_STRING_MAC(saCommand, saCmdGet,
                       "get another saCommand than was set: %d, %d", dev, entryInd);

        /* Call with entryInd [MAC_RANGE_FILTER_TABLE_SIZE_CNS-1], daCommand [CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E], saCommand [CPSS_MAC_FLT_FRWRD_E] */
        daCommand = CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E;
        saCommand = CPSS_MAC_FLT_FRWRD_E;
        
        st = cpssDxSalBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask, daCommand, saCommand);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, daCommand = %d, saCommand = %d",
                                     dev, entryInd, daCommand, saCommand);

        /*
            1.2. Call cpssDxSalBrgFdbMacFilterEntryGet with the same parameters as in 1.1.
            Expected: GT_OK and the same macAddrPtr, macMaskPtr, daCommand and saCommand.
        */
        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddrGet, &macMaskGet, &daCmdGet, &saCmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacFilterEntryGet: %d, %d", dev, entryInd);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macAddr, (GT_VOID*) &macAddrGet, sizeof (macAddr)))? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                     "get another macAddr than was set: %d", dev);

        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &macMask, (GT_VOID*) &macMaskGet, sizeof (macMask)))? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                                     "get another macMask than was set: %d", dev);

        UTF_VERIFY_EQUAL2_STRING_MAC(daCommand, daCmdGet,
                       "get another daCommand than was set: %d, %d", dev, entryInd);
        UTF_VERIFY_EQUAL2_STRING_MAC(saCommand, saCmdGet,
                       "get another saCommand than was set: %d, %d", dev, entryInd);

        /*
            1.3. Call function with out of range entryInd [MAC_RANGE_FILTER_TABLE_SIZE_CNS]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        entryInd = MAC_RANGE_FILTER_TABLE_SIZE_CNS;

        st = cpssDxSalBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask, daCommand, saCommand);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryInd);

        entryInd = 0;

        /*
            1.4. Call function with macAddrPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacFilterEntrySet(dev, entryInd, NULL, &macMask, daCommand, saCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, macAddrPtr = NULL", dev, entryInd);

        /*
            1.5. Call function with macMaskPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, NULL, daCommand, saCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, macMaskPtr = NULL", dev, entryInd);

        /*
            1.6. Call function with out of range daCommand [0x5AAAAAA5]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        daCommand = BRG_FDB_INVALID_ENUM_CNS;
        
        st = cpssDxSalBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask, daCommand, saCommand);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, daCommand = %d", dev, entryInd, daCommand);

        daCommand = CPSS_MAC_FLT_FRWRD_E;

        /*
            1.7. Call function with out of range saCommand [0x5AAAAAA5]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        saCommand = BRG_FDB_INVALID_ENUM_CNS;
        
        st = cpssDxSalBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask, daCommand, saCommand);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, saCommand = %d", dev, entryInd, saCommand);

        saCommand = CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E;
    }

    macAddr.arEther[0] = 0xAA;
    macAddr.arEther[1] = 0xBB;
    macAddr.arEther[2] = 0;
    macAddr.arEther[3] = 0;
    macAddr.arEther[4] = 0;
    macAddr.arEther[5] = 1;

    macMask.arEther[0] = 0xFF;
    macMask.arEther[1] = 0xFF;
    macMask.arEther[2] = 0xFF;
    macMask.arEther[3] = 0xFF;
    macMask.arEther[4] = 0xFF;
    macMask.arEther[5] = 0xFF;

    entryInd  = 0;
    daCommand = CPSS_MAC_FLT_FRWRD_E;
    saCommand = CPSS_MAC_FLT_FRWRD_MIRROR_TO_CPU_E;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask, daCommand, saCommand);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbMacFilterEntrySet(dev, entryInd, &macAddr, &macMask, daCommand, saCommand);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMacFilterEntryGet
(
    IN GT_U8                dev,
    IN GT_32                entryInd,
    OUT GT_ETHERADDR         *macAddrPtr,
    OUT GT_ETHERADDR         *macMaskPtr,
    OUT CPSS_MAC_FLT_CMD_ENT *daCommandPtr,
    OUT CPSS_MAC_FLT_CMD_ENT *saCommandPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMacFilterEntryGet)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with [0 / MAC_RANGE_FILTER_TABLE_SIZE_CNS -  1],
                       non-NULL macAddrPtr,macMaskPtr, daCommandPtr and saCommandPtr.
    Expected: GT_OK.
    1.2. Call function with out of range entryInd [MAC_RANGE_FILTER_TABLE_SIZE_CNS]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call function with with macAddrPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call function with macMaskPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call function with daCommandPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call function with saCommandPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_32                entryInd  = 0;
    GT_ETHERADDR         macAddr;
    GT_ETHERADDR         macMask;
    CPSS_MAC_FLT_CMD_ENT daCommand = CPSS_MAC_FLT_FRWRD_E;
    CPSS_MAC_FLT_CMD_ENT saCommand = CPSS_MAC_FLT_FRWRD_E;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with [0 / MAC_RANGE_FILTER_TABLE_SIZE_CNS -  1],
                               non-NULL macAddrPtr,macMaskPtr, daCommandPtr and saCommandPtr.
            Expected: GT_OK.
        */

        /* Call with entryInd [0] */
        entryInd  = 0;
        
        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask, &daCommand, &saCommand);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryInd);

        /* Call with entryInd [MAC_RANGE_FILTER_TABLE_SIZE_CNS-1] */
        entryInd  = MAC_RANGE_FILTER_TABLE_SIZE_CNS - 1;
        
        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask, &daCommand, &saCommand);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryInd);

        /*
            1.2. Call function with out of range entryInd [MAC_RANGE_FILTER_TABLE_SIZE_CNS]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        entryInd = MAC_RANGE_FILTER_TABLE_SIZE_CNS;

        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask, &daCommand, &saCommand);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryInd);

        entryInd = 0;

        /*
            1.3. Call function with macAddrPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, NULL, &macMask, &daCommand, &saCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, macAddrPtr = NULL", dev, entryInd);

        /*
            1.4. Call function with macMaskPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, NULL, &daCommand, &saCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, macMaskPtr = NULL", dev, entryInd);

        /*
            1.5. Call function with daCommandPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask, NULL, &saCommand);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, daCommandPtr = NULL", dev, entryInd);

        /*
            1.6. Call function with saCommandPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask, &daCommand, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, saCommandPtr = NULL", dev, entryInd);
    }

    entryInd  = 0;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask, &daCommand, &saCommand);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbMacFilterEntryGet(dev, entryInd, &macAddr, &macMask, &daCommand, &saCommand);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbNaToCpuEnable
(
    IN GT_U8   dev,
    IN GT_BOOL status
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbNaToCpuEnable)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with status [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_BOOL   status = GT_FALSE;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with status [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with entryInd status [GT_FALSE] */
        status = GT_FALSE;
        
        st = cpssDxSalBrgFdbNaToCpuEnable(dev, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, status);

        /* Call with entryInd status [GT_TRUE] */
        status = GT_TRUE;
        
        st = cpssDxSalBrgFdbNaToCpuEnable(dev, status);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, status);
    }

    status = GT_TRUE;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbNaToCpuEnable(dev, status);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbNaToCpuEnable(dev, status);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbAuToCpuSet
(
    IN GT_U8                        dev,
    IN CPSS_MAC_AU2CPU_TYPE_ENT     auMsgType,
    IN GT_BOOL                      enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbAuToCpuSet)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with auMsgType [CPSS_MAC_AU_MESSAGE_SA_LEARNING_E /
                                       CPSS_MAC_AU_MESSAGE_SA_SECURITY_E /
                                       CPSS_MAC_AU_MESSAGE_ALL_E /
                                       CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E]
                            and enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssDxSalBrgFdbAuToCpuGet with auMsgType [CPSS_MAC_AU_MESSAGE_SA_LEARNING_E /
                                                        CPSS_MAC_AU_MESSAGE_SA_SECURITY_E /
                                                        CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E]
                                             and non-NULL enablePtr.
    Expected: GT_OK and the same enable.
    1.3. Call function with auMsgType [CPSS_MAC_AU_MESSAGE_TA_AA_E /
                                       CPSS_MAC_AU_MESSAGE_QA_E /
                                       CPSS_MAC_AU_MESSAGE_QA_FROM_DEVICE_TO_OTHER_DEVICES_AND_TO_CPU_E] (not supported)
                            and enable the same as in 1.1.
    Expected: NON GT_OK.
    1.4. Call function with out of range auMsgType [0x5AAAAAA5]
                            and enable the same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    CPSS_MAC_AU2CPU_TYPE_ENT auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;
    GT_BOOL                  enable    = GT_FALSE;
    GT_BOOL                  enableGet = GT_FALSE;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with auMsgType [CPSS_MAC_AU_MESSAGE_SA_LEARNING_E /
                                               CPSS_MAC_AU_MESSAGE_SA_SECURITY_E /
                                               CPSS_MAC_AU_MESSAGE_ALL_E /
                                               CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E]
                                    and enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_SA_LEARNING_E], enable [GT_FALSE] */
        enable    = GT_FALSE;
        auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;
        
        st = cpssDxSalBrgFdbAuToCpuSet(dev, auMsgType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, auMsgType, enable);

        /*
            1.2. Call cpssDxSalBrgFdbAuToCpuGet with auMsgType [CPSS_MAC_AU_MESSAGE_SA_LEARNING_E /
                                                                CPSS_MAC_AU_MESSAGE_SA_SECURITY_E /
                                                                CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E]
                                                     and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbAuToCpuGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_SA_SECURITY_E], enable [GT_FALSE] */
        auMsgType = CPSS_MAC_AU_MESSAGE_SA_SECURITY_E;
        
        st = cpssDxSalBrgFdbAuToCpuSet(dev, auMsgType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, auMsgType, enable);

        /*
            1.2. Call cpssDxSalBrgFdbAuToCpuGet with auMsgType [CPSS_MAC_AU_MESSAGE_SA_LEARNING_E /
                                                                CPSS_MAC_AU_MESSAGE_SA_SECURITY_E /
                                                                CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E]
                                                     and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbAuToCpuGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_ALL_E], enable [GT_TRUE] */
        enable    = GT_TRUE;
        auMsgType = CPSS_MAC_AU_MESSAGE_ALL_E;
        
        st = cpssDxSalBrgFdbAuToCpuSet(dev, auMsgType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, auMsgType, enable);

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E], enable [GT_TRUE] */
        auMsgType = CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E;
        
        st = cpssDxSalBrgFdbAuToCpuSet(dev, auMsgType, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, auMsgType, enable);

        /*
            1.2. Call cpssDxSalBrgFdbAuToCpuGet with auMsgType [CPSS_MAC_AU_MESSAGE_SA_LEARNING_E /
                                                                CPSS_MAC_AU_MESSAGE_SA_SECURITY_E /
                                                                CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E]
                                                     and non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbAuToCpuGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet, "get another enable than was set: %d", dev);

        /*
            1.3. Call function with auMsgType [CPSS_MAC_AU_MESSAGE_TA_AA_E /
                                               CPSS_MAC_AU_MESSAGE_QA_E /
                                               CPSS_MAC_AU_MESSAGE_QA_FROM_DEVICE_TO_OTHER_DEVICES_AND_TO_CPU_E] (not supported)
                                    and enable the same as in 1.1.
            Expected: NON GT_OK.
        */

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_TA_AA_E], enable [GT_TRUE] */
        enable    = GT_TRUE;
        auMsgType = CPSS_MAC_AU_MESSAGE_TA_AA_E;
        
        st = cpssDxSalBrgFdbAuToCpuSet(dev, auMsgType, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, auMsgType, enable);

        auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_QA_E], enable [GT_TRUE] */
        enable    = GT_TRUE;
        auMsgType = CPSS_MAC_AU_MESSAGE_QA_E;
        
        st = cpssDxSalBrgFdbAuToCpuSet(dev, auMsgType, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, auMsgType, enable);

        auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_QA_FROM_DEVICE_TO_OTHER_DEVICES_AND_TO_CPU_E], enable [GT_TRUE] */
        enable    = GT_TRUE;
        auMsgType = CPSS_MAC_AU_MESSAGE_QA_FROM_DEVICE_TO_OTHER_DEVICES_AND_TO_CPU_E;
        
        st = cpssDxSalBrgFdbAuToCpuSet(dev, auMsgType, enable);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, auMsgType, enable);

        auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;

        /*
            1.4. Call function with out of range auMsgType [0x5AAAAAA5]
                                    and enable the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        enable    = GT_TRUE;
        auMsgType = BRG_FDB_INVALID_ENUM_CNS;
        
        st = cpssDxSalBrgFdbAuToCpuSet(dev, auMsgType, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, auMsgType);

        auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;
    }

    enable    = GT_TRUE;
    auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbAuToCpuSet(dev, auMsgType, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbAuToCpuSet(dev, auMsgType, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbAuToCpuGet
(
    IN GT_U8                        dev,
    IN CPSS_MAC_AU2CPU_TYPE_ENT     auMsgType,
    OUT GT_BOOL                     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbAuToCpuGet)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with auMsgType [CPSS_MAC_AU_MESSAGE_SA_LEARNING_E /
                                       CPSS_MAC_AU_MESSAGE_SA_SECURITY_E /
                                       CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E]
                            and non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call function with auMsgType [CPSS_MAC_AU_MESSAGE_TA_AA_E /
                                       CPSS_MAC_AU_MESSAGE_QA_E /
                                       CPSS_MAC_AU_MESSAGE_ALL_E /
                                       CPSS_MAC_AU_MESSAGE_QA_FROM_DEVICE_TO_OTHER_DEVICES_AND_TO_CPU_E] (not supported)
                            and non-NULL enablePtr.
    Expected: NON GT_OK.
    1.3. Call function with out of range auMsgType [0x5AAAAAA5]
                            and non-NULL enablePtr.
    Expected: GT_BAD_PARAM.
    1.4. Call function with auMsgType[CPSS_MAC_AU_MESSAGE_SA_LEARNING_E]
                            and enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    CPSS_MAC_AU2CPU_TYPE_ENT auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;
    GT_BOOL                  enable    = GT_FALSE;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with auMsgType [CPSS_MAC_AU_MESSAGE_SA_LEARNING_E /
                                               CPSS_MAC_AU_MESSAGE_SA_SECURITY_E /
                                               CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E]
                                    and non-NULL enablePtr.
            Expected: GT_OK.
        */

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_SA_LEARNING_E] */
        auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;
        
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, auMsgType);

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_SA_SECURITY_E] */
        auMsgType = CPSS_MAC_AU_MESSAGE_SA_SECURITY_E;
        
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, auMsgType);

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E] */
        auMsgType = CPSS_MAC_AU_MESSAGE_ALL_EXCLUDE_QA_NA_E;
        
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, auMsgType);

        /*
            1.2. Call function with auMsgType [CPSS_MAC_AU_MESSAGE_TA_AA_E /
                                               CPSS_MAC_AU_MESSAGE_QA_E /
                                               CPSS_MAC_AU_MESSAGE_ALL_E /
                                               CPSS_MAC_AU_MESSAGE_QA_FROM_DEVICE_TO_OTHER_DEVICES_AND_TO_CPU_E] (not supported)
                                    and non-NULL enablePtr.
            Expected: NON GT_OK.
        */

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_TA_AA_E] */
        auMsgType = CPSS_MAC_AU_MESSAGE_TA_AA_E;
        
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, auMsgType);

        auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_QA_E] */
        auMsgType = CPSS_MAC_AU_MESSAGE_QA_E;
        
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, auMsgType);

        auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_ALL_E] */
        auMsgType = CPSS_MAC_AU_MESSAGE_ALL_E;
        
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, auMsgType);

        auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;

        /* Call with auMsgType [CPSS_MAC_AU_MESSAGE_QA_FROM_DEVICE_TO_OTHER_DEVICES_AND_TO_CPU_E] */
        auMsgType = CPSS_MAC_AU_MESSAGE_QA_FROM_DEVICE_TO_OTHER_DEVICES_AND_TO_CPU_E;
        
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enable);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, auMsgType);

        auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;

        /*
            1.3. Call function with out of range auMsgType [0x5AAAAAA5]
                                    and non-NULL enablePtr.
            Expected: GT_BAD_PARAM.
        */
        auMsgType = BRG_FDB_INVALID_ENUM_CNS;
        
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, auMsgType);

        auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;

        /*
            1.4. Call function with auMsgType[CPSS_MAC_AU_MESSAGE_SA_LEARNING_E]
                                    and enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, enablePtr = NULL", dev, auMsgType);
    }

    auMsgType = CPSS_MAC_AU_MESSAGE_SA_LEARNING_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbAuToCpuGet(dev, auMsgType, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbQaSend
(
    IN  GT_U8               dev,
    IN  GT_U16              vid,
    IN  GT_ETHERADDR        *macAddrPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbQaSend)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with vid [100 / 4095]
                            and non-NULL macAddrPtr [{0xAA,0xBB,0,0,0,0x01}].
    Expected: GT_OK.
    1.2. Call function with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and non-NULL macAddrPtr [{0xAA,0xBB,0,0,0,0x01}].
    Expected: GT_BAD_PARAM.
    1.3. Call function with macAddrPtr [NULL]
                            and vid [100].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U16       vid = 0;
    GT_ETHERADDR macAddr;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with vid [100 / 4095]
                                    and non-NULL macAddrPtr [{0xAA,0xBB,0,0,0,0x01}].
            Expected: GT_OK.
        */

        /* Call with vid [100], macAddrPtr [{0xAA,0xBB,0,0,0,0x01}] */
        vid = PRV_CPSS_VALID_VLANS_CNS;

        macAddr.arEther[0] = 0xAA;
        macAddr.arEther[1] = 0xBB;
        macAddr.arEther[2] = 0;
        macAddr.arEther[3] = 0;
        macAddr.arEther[4] = 0;
        macAddr.arEther[5] = 1;
        
        st = cpssDxSalBrgFdbQaSend(dev, vid, &macAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* Call with vid [4095], macAddrPtr [{0xAA,0xBB,0,0,0,0x01}] */
        vid = 4095;
        
        st = cpssDxSalBrgFdbQaSend(dev, vid, &macAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /*
            1.2. Call function with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                    and non-NULL macAddrPtr [{0xAA,0xBB,0,0,0,0x01}].
            Expected: GT_BAD_PARAM.
        */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;
        
        st = cpssDxSalBrgFdbQaSend(dev, vid, &macAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vid);

        vid = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.3. Call function with macAddrPtr [NULL]
                                    and vid [100].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbQaSend(dev, vid, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macAddrPtr = NULL", dev);
    }

    vid = PRV_CPSS_VALID_VLANS_CNS;

    macAddr.arEther[0] = 0xAA;
    macAddr.arEther[1] = 0xBB;
    macAddr.arEther[2] = 0;
    macAddr.arEther[3] = 0;
    macAddr.arEther[4] = 0;
    macAddr.arEther[5] = 1;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbQaSend(dev, vid, &macAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbQaSend(dev, vid, &macAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbQiSend
(
    IN  GT_U8               dev,
    IN  GT_U16              vid,
    IN  GT_ETHERADDR        *macAddrPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbQiSend)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with vid [100 / 4095]
                            and non-NULL macAddrPtr [{0xAA,0xBB,0,0,0,0x01}].
    Expected: GT_OK.
    1.2. Call function with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and non-NULL macAddrPtr [{0xAA,0xBB,0,0,0,0x01}].
    Expected: GT_BAD_PARAM.
    1.3. Call function with macAddrPtr [NULL] and vid [100].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U16       vid = 0;
    GT_ETHERADDR macAddr;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with vid [100 / 4095]
                                    and non-NULL macAddrPtr [{0xAA,0xBB,0,0,0,0x01}].
            Expected: GT_OK.
        */

        /* Call with vid [100], macAddrPtr [{0xAA,0xBB,0,0,0,0x01}] */
        vid = PRV_CPSS_VALID_VLANS_CNS;

        macAddr.arEther[0] = 0xAA;
        macAddr.arEther[1] = 0xBB;
        macAddr.arEther[2] = 0;
        macAddr.arEther[3] = 0;
        macAddr.arEther[4] = 0;
        macAddr.arEther[5] = 1;
        
        st = cpssDxSalBrgFdbQiSend(dev, vid, &macAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /* Call with vid [4095], macAddrPtr [{0xAA,0xBB,0,0,0,0x01}] */
        vid = 4095;
        
        st = cpssDxSalBrgFdbQiSend(dev, vid, &macAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vid);

        /*
            1.2. Call function with out of range vid [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                    and non-NULL macAddrPtr [{0xAA,0xBB,0,0,0,0x01}].
            Expected: GT_BAD_PARAM.
        */
        vid = PRV_CPSS_MAX_NUM_VLANS_CNS;
        
        st = cpssDxSalBrgFdbQiSend(dev, vid, &macAddr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vid);

        vid = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.3. Call function with macAddrPtr [NULL]
                                    and vid [100].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbQiSend(dev, vid, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, macAddrPtr = NULL", dev);
    }

    vid = PRV_CPSS_VALID_VLANS_CNS;

    macAddr.arEther[0] = 0xAA;
    macAddr.arEther[1] = 0xBB;
    macAddr.arEther[2] = 0;
    macAddr.arEther[3] = 0;
    macAddr.arEther[4] = 0;
    macAddr.arEther[5] = 1;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbQiSend(dev, vid, &macAddr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbQiSend(dev, vid, &macAddr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMaxLookupLenSet
(
    IN GT_U8    dev,
    IN GT_U8    lookupLen
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMaxLookupLenSet)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with even lookupLen [2 / 4 / 6 / 8 / 10 / 12 / 14 / 16 ].
    Expected: GT_OK.
    1.2. Call function with odd lookupLen [11 / 17].
    Expected: NON GT_OK.
    1.3. Call function with out of range lookupLen [0 / 18].
    Expected: NON GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U8     lookupLen = 0;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with even lookupLen [2 / 4 / 6 / 8 / 10 / 12 / 14 / 16 ].
            Expected: GT_OK.
        */

        /* Call with lookupLen [2] */
        lookupLen = 2;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        /* Call with lookupLen [4] */
        lookupLen = 4;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        /* Call with lookupLen [6] */
        lookupLen = 6;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        /* Call with lookupLen [8] */
        lookupLen = 8;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        /* Call with lookupLen [10] */
        lookupLen = 10;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        /* Call with lookupLen [12] */
        lookupLen = 12;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        /* Call with lookupLen [14] */
        lookupLen = 14;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        /* Call with lookupLen [16] */
        lookupLen = 16;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        /*
            1.2. Call function with odd lookupLen [11 / 17].
            Expected: NON GT_OK.
        */

        /* Call with lookupLen [11] */
        lookupLen = 11;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        lookupLen = 2;

        /* Call with lookupLen [17] */
        lookupLen = 17;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        lookupLen = 2;

        /*
            1.3. Call function with out of range lookupLen [0 / 18].
            Expected: NON GT_OK.
        */

        /* Call with lookupLen [0] */
        lookupLen = 0;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        lookupLen = 2;

        /* Call with lookupLen [18] */
        lookupLen = 18;
        
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, lookupLen);

        lookupLen = 2;
    }

    lookupLen = 2;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbMaxLookupLenSet(dev, lookupLen);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMacAddrEntryWrite
(
    IN GT_U8                dev,
    IN GT_U32               index,
    IN GT_BOOL              skip,
    IN CPSS_MAC_ENTRY_STC   *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMacAddrEntryWrite)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function cpssDxSalBrgFdbL2IngrProcEnable to disable the Layer 2 Ingress processor.
    Expected: GT_OK.
    1.2. Call function with index [0 / 2 / macTableSize(dev) - 1] , skip [GT_FALSE] and non-NULL entryPtr {macAddr [{0xAA,0xBB,0,0,0,0x01}]; dstInterface {type [CPSS_INTERFACE_PORT_E / CPSS_INTERFACE_TRUNK_E / CPSS_INTERFACE_VIDX_E]; devPort [devNum [dev]; portNum[0]]; trunkId [2]; vidx [100]}; isStatic [GT_TRUE]; vlanId [100]; srcTc [0 / 1 / 3]; dstTc [0 / 1 / 3]; daCommand [CPSS_MAC_TABLE_FRWRD_E / CPSS_MAC_TABLE_DROP_E / CPSS_MAC_TABLE_INTERV_E / CPSS_MAC_TABLE_CNTL_E]; saCommand [CPSS_MAC_TABLE_FRWRD_E / CPSS_MAC_TABLE_DROP_E / CPSS_MAC_TABLE_INTERV_E / CPSS_MAC_TABLE_CNTL_E]; saClass [GT_TRUE]; daClass [GT_TRUE]; saCib [GT_TRUE]; daCib [GT_TRUE]; daRoute [GT_TRUE];  cosIpv4En [GT_FALSE];  mirrorToRxAnalyzerPortEn [GT_FALSE]}.
    Expected: GT_OK. 
    1.3. Call cpssDxSalBrgFdbMacAddrEntryRead with the same index.
    Expected: GT_OK and the same skip and entryPtr.
    1.4. Call function with out of range index [macTableSize(dev)] and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.5. Call function with out of range entryPtr->dstInterface.type [0x5AAAAAA5] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call function with skip [GT_TRUE], out of range entryPtr->dstInterface.type [0x5AAAAAA5] (entry will be skipped) and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.7. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E], out of range entryPtr-> dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.8. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E], out of range entryPtr-> dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (not relevant) and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.9. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E], out of range entryPtr -> dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.10. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E], out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] (not relevant)
    and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.11. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E], out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] 
    and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.12. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E], out of range entryPtr->dstInterface.vidx [0xFFFF] (not relevant) and other parameters the same as in 1.1.
    Expected: GT_OK 
    1.13. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E], out of range entryPtr->dstInterface.vidx [0xFFFF] and other parameters the same as in 1.1.
    Expected: NOT GT_OK 
    1.14. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VID_E] (not supported) and other parameters the same as in 1.1.
    Expected: NOT GT_OK.
    1.15. Call function with out of range entryPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.16. Call function with out of range entryPtr->srcTc [CPSS_4_TC_RANGE_CNS] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.17. Call function with out of range entryPtr->dstTc [CPSS_4_TC_RANGE_CNS] and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.18. Call function with entryPtr->daCommand [CPSS_MAC_TABLE_SOFT_DROP_E ] and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.19. Call function with out of range entryPtr->daCommand [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
    1.20. Call function with entryPtr->saCommand [CPSS_MAC_TABLE_SOFT_DROP_E ] and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.21. Call function with out of range entryPtr->saCommand [0x5AAAAAA5] and other parameters the same as in 1.1. Expected: GT_BAD_PARAM.
    1.22. Call function with entryPtr [NULL]. Expected: GT_BAD_PTR.
    1.23. Call cpssDxSalBrgFdbMacAddrEntryInvalidate
    with the index [0 / macTableSize(dev) - 1] to invalidate changes.
    Expected: GT_OK.
    1.24. Call function cpssDxSalBrgFdbL2IngrProcEnable to enable the Layer 2 Ingress processor.
    Expected: GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U32             index   = 0;
    GT_BOOL            skip    = GT_FALSE;
    GT_BOOL            enable  = GT_FALSE;
    GT_BOOL            valid   = GT_FALSE;
    GT_BOOL            skipGet = GT_FALSE;
    GT_BOOL            aged    = GT_FALSE;
    GT_BOOL            isEqual = GT_FALSE;
    CPSS_MAC_ENTRY_STC entry;
    CPSS_MAC_ENTRY_STC entryGet;
    GT_U32             maxMacIndex = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting mac table size */
        st = prvUtfMacTableSize(dev, &maxMacIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMacTableSize: %d", dev);

        /*
            1.1. Call function cpssDxSalBrgFdbL2IngrProcEnable to disable the Layer 2 Ingress processor.
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbL2IngrProcEnable: %d, %d", dev, enable);

        /* 
            1.2. Call function with index [0 / 2 / macTableSize(dev) - 1],
                                    skip [GT_FALSE],
                                    non-NULL entryPtr {macAddr [{0xAA,0xBB,0,0,0,0x01}];
                                                       dstInterface {type [CPSS_INTERFACE_PORT_E /
                                                                           CPSS_INTERFACE_TRUNK_E /
                                                                           CPSS_INTERFACE_VIDX_E];
                                                                     devPort {devNum [dev];
                                                                              portNum[0]};
                                                                     trunkId [2];
                                                                     vidx [100]};
                                                       isStatic [GT_TRUE];
                                                       vlanId [100];
                                                       srcTc [0 / 1 / 3];
                                                       dstTc [0 / 1 / 3];
                                                       daCommand [CPSS_MAC_TABLE_FRWRD_E /
                                                                  CPSS_MAC_TABLE_DROP_E /
                                                                  CPSS_MAC_TABLE_INTERV_E];
                                                       saCommand [CPSS_MAC_TABLE_FRWRD_E /
                                                                  CPSS_MAC_TABLE_DROP_E /
                                                                  CPSS_MAC_TABLE_INTERV_E];
                                                       saClass [GT_TRUE];
                                                       daClass [GT_TRUE];
                                                       saCib [GT_TRUE];
                                                       daCib [GT_TRUE];
                                                       daRoute [GT_TRUE];
                                                       cosIpv4En [GT_FALSE];
                                                       mirrorToRxAnalyzerPortEn [GT_TRUE]}.
            Expected: GT_OK. 
        */
        skip = GT_FALSE;

        entry.macAddr.arEther[0] = 0xAA;
        entry.macAddr.arEther[1] = 0xBB;
        entry.macAddr.arEther[2] = 0;
        entry.macAddr.arEther[3] = 0;
        entry.macAddr.arEther[4] = 0;
        entry.macAddr.arEther[5] = 1;

        entry.isStatic  = GT_TRUE;
        entry.vlanId    = PRV_CPSS_VALID_VLANS_CNS;
        entry.saClass   = GT_TRUE;
        entry.daClass   = GT_TRUE;
        entry.saCib     = GT_TRUE;
        entry.daCib     = GT_TRUE;
        entry.daRoute   = GT_TRUE;
        entry.cosIpv4En = GT_FALSE;
        entry.mirrorToRxAnalyzerPortEn = GT_TRUE;

        /* Call with type [CPSS_INTERFACE_PORT_E], srcTc [0], dstTc [0],         */
        /* daCommand [CPSS_MAC_TABLE_FRWRD_E], saCommand [CPSS_MAC_TABLE_FRWRD_E */
        index = 0;

        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.devPort.devNum  = dev;
        entry.dstInterface.devPort.portNum = 0;

        entry.srcTc = 0;
        entry.dstTc = 0;

        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /* Call with type [CPSS_INTERFACE_TRUNK_E], srcTc [1], dstTc [1],      */
        /* daCommand [CPSS_MAC_TABLE_DROP_E], saCommand [CPSS_MAC_TABLE_DROP_E */
        index = 2;

        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        entry.dstInterface.trunkId = 2;

        entry.srcTc = 1;
        entry.dstTc = 1;

        entry.daCommand = CPSS_MAC_TABLE_DROP_E;
        entry.saCommand = CPSS_MAC_TABLE_DROP_E;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /* Call with type [CPSS_INTERFACE_VIDX_E], srcTc [3], dstTc [3],           */
        /* daCommand [CPSS_MAC_TABLE_INTERV_E], saCommand [CPSS_MAC_TABLE_INTERV_E */
        index = maxMacIndex - 1;

        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        entry.srcTc = 3;
        entry.dstTc = 3;

        entry.daCommand = CPSS_MAC_TABLE_INTERV_E;
        entry.saCommand = CPSS_MAC_TABLE_INTERV_E;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /*
            1.3. Call cpssDxSalBrgFdbMacAddrEntryRead with the same index.
            Expected: GT_OK and the same skip and entryPtr.
        */
        st = cpssDxSalBrgFdbMacAddrEntryRead(dev, index, &valid, &skipGet, &aged, &entryGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacAddrEntryRead: %d, %d", dev, index);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, valid, "get invalid entry: %d, %d", dev, index);

        if (GT_TRUE == valid)
        {
            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(skip, skipGet, "get another skip than was set: %d, %d", dev, index);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &entry.macAddr, (GT_VOID*) &entryGet.macAddr, sizeof (entry.macAddr)))? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                                     "get another entryPtr->macAddr than was set: %d, %d", dev, index);

            UTF_VERIFY_EQUAL2_STRING_MAC(entry.dstInterface.type, entryGet.dstInterface.type,
                                     "get another entryPtr->dstInterface.type than was set: %d, %d", dev, index);

            UTF_VERIFY_EQUAL2_STRING_MAC(entry.isStatic, entryGet.isStatic,
                                     "get another entryPtr->isStatic than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.vlanId, entryGet.vlanId,
                                     "get another entryPtr->vlanId than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.srcTc, entryGet.srcTc,
                                     "get another entryPtr->srcTc than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.dstTc, entryGet.dstTc,
                                     "get another entryPtr->dstTc than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.daCommand, entryGet.daCommand,
                                     "get another entryPtr->daCommand than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.saCommand, entryGet.saCommand,
                                     "get another entryPtr->saCommand than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.saClass, entryGet.saClass,
                                     "get another entryPtr->saClass than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.daClass, entryGet.daClass,
                                     "get another entryPtr->daClass than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.saCib, entryGet.saCib,
                                     "get another entryPtr->saCib than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.daCib, entryGet.daCib,
                                     "get another entryPtr->daCib than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.daRoute, entryGet.daRoute,
                                     "get another entryPtr->daRoute than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.cosIpv4En, entryGet.cosIpv4En,
                                     "get another entryPtr->cosIpv4En than was set: %d, %d", dev, index);
            UTF_VERIFY_EQUAL2_STRING_MAC(entry.mirrorToRxAnalyzerPortEn, entryGet.mirrorToRxAnalyzerPortEn,
                                     "get another entryPtr->mirrorToRxAnalyzerPortEn than was set: %d, %d", dev, index);
        }

        /*
            1.4. Call function with out of range index [macTableSize(dev)]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        index = maxMacIndex;

        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.5. Call function with out of range entryPtr->dstInterface.type [0x5AAAAAA5]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.dstInterface.type = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->dstInterface = %d", dev, entry.dstInterface.type);

        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        /*
            1.6. Call function with skip [GT_TRUE], out of range entryPtr->dstInterface.type [0x5AAAAAA5] (entry will be skipped) and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        skip = GT_TRUE;

        entry.dstInterface.type = BRG_FDB_INVALID_ENUM_CNS;

        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, skip = %d, entryPtr->dstInterface = %d", dev, entry.dstInterface.type, skip);

        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        skip = GT_FALSE;

        /*
            1.7. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E],
                                    out of range entryPtr->dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.devPort.devNum = %d",
                                        dev, entry.dstInterface.type, entry.dstInterface.devPort.devNum);

        entry.dstInterface.devPort.devNum = dev;

        /*
            1.8. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E],
                                    out of range entryPtr->dstInterface.devPort.devNum [PRV_CPSS_MAX_PP_DEVICES_CNS]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        entry.dstInterface.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.devPort.devNum = %d",
                                    dev, entry.dstInterface.type, entry.dstInterface.devPort.devNum);

        entry.dstInterface.devPort.devNum = dev;

        /*
            1.9. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E],
                                    out of range entryPtr->dstInterface.devPort.portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.devPort.portNum = %d",
                                        dev, entry.dstInterface.type, entry.dstInterface.devPort.portNum);

        entry.dstInterface.devPort.portNum = 0;

        /*
            1.10. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_PORT_E],
                                     out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] (not relevant)
                                     and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

        entry.dstInterface.trunkId = MAX_TRUNK_ID_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.trunkId = %d",
                                    dev, entry.dstInterface.type, entry.dstInterface.trunkId);

        entry.dstInterface.trunkId = 2;

        /*
            1.11. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E],
                                    out of range entryPtr->dstInterface.trunkId [MAX_TRUNK_ID_CNS] 
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        entry.dstInterface.trunkId = MAX_TRUNK_ID_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.trunkId = %d",
                                        dev, entry.dstInterface.type, entry.dstInterface.trunkId);

        entry.dstInterface.trunkId = 2;

        /*
            1.12. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_TRUNK_E],
                                     out of range entryPtr->dstInterface.vidx [0xFFFF] (not relevant)
                                     and other parameters the same as in 1.1.
            Expected: GT_OK 
        */
        entry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;

        entry.dstInterface.vidx = 0xFFFF;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.vidx = %d",
                                    dev, entry.dstInterface.type, entry.dstInterface.vidx);

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.13. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VIDX_E],
                                     out of range entryPtr->dstInterface.vidx [0xFFFF]
                                     and other parameters the same as in 1.1.
            Expected: NON GT_OK 
        */
        entry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

        entry.dstInterface.vidx = 0xFFFF;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d, ->dstInterface.vidx = %d",
                                        dev, entry.dstInterface.type, entry.dstInterface.vidx);

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.14. Call function with entryPtr->dstInterface.type [CPSS_INTERFACE_VID_E] (not supported by function's contract)
                                     and other parameters the same as in 1.1.
            Expected: NOT GT_OK.
        */
        entry.dstInterface.type = CPSS_INTERFACE_VID_E;

        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, ->dstInterface.type = %d", dev, entry.dstInterface.type);

        entry.dstInterface.vidx = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.15. Call function with out of range entryPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->vlanId = %d", dev, entry.vlanId);

        entry.vlanId = PRV_CPSS_VALID_VLANS_CNS;

        /*
            1.16. Call function with out of range entryPtr->srcTc [CPSS_4_TC_RANGE_CNS]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.srcTc = CPSS_4_TC_RANGE_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->srcTc = %d", dev, entry.srcTc);

        entry.srcTc = 0;

        /*
            1.17. Call function with out of range entryPtr->dstTc [CPSS_4_TC_RANGE_CNS]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.dstTc = CPSS_4_TC_RANGE_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->dstTc = %d", dev, entry.dstTc);

        entry.dstTc = 0;

        /*
            1.18. Call function with entryPtr->daCommand [CPSS_MAC_TABLE_SOFT_DROP_E] (not supported)
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK. 
        */
        entry.daCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryPtr->daCommand = %d", dev, entry.daCommand);

        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.19. Call function with out of range entryPtr->daCommand [0x5AAAAAA5]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.daCommand = BRG_FDB_INVALID_ENUM_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->daCommand = %d", dev, entry.daCommand);

        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.20. Call function with entryPtr->saCommand [CPSS_MAC_TABLE_MIRROR_TO_CPU_E /
                                                         CPSS_MAC_TABLE_SOFT_DROP_E] (not supported)
                                    and other parameters the same as in 1.1.
            Expected: NOT GT_OK. 
        */
        entry.saCommand = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, entryPtr->saCommand = %d", dev, entry.saCommand);

        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.21. Call function with out of range entryPtr->saCommand [0x5AAAAAA5]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        entry.saCommand = BRG_FDB_INVALID_ENUM_CNS;
        
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, entryPtr->saCommand = %d", dev, entry.saCommand);

        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;

        /*
            1.22. Call function with entryPtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryPtr = NULL", dev);

        /*
            1.23. Call cpssDxSalBrgFdbMacAddrEntryInvalidate
            with the index [0 / macTableSize(dev) - 1] to invalidate changes.
            Expected: GT_OK.
        */

        /* Call with index = 0 */
        index = 0;

        st = cpssDxSalBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacAddrEntryInvalidate: %d, %d", dev, index);

        /* Call with index = 2 */
        index = 2;

        st = cpssDxSalBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacAddrEntryInvalidate: %d, %d", dev, index);

        /* Call with index = macTableSize(dev) - 1 */
        index = maxMacIndex - 1;

        st = cpssDxSalBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacAddrEntryInvalidate: %d, %d", dev, index);

        /*
            1.24. Call function cpssDxSalBrgFdbL2IngrProcEnable to enable the Layer 2 Ingress processor.
            Expected: GT_OK.
        */

        enable = GT_TRUE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbL2IngrProcEnable: %d, %d", dev, enable);
    }

    index = 0;
    skip  = GT_FALSE;

    entry.macAddr.arEther[0] = 0xAA;
    entry.macAddr.arEther[1] = 0xBB;
    entry.macAddr.arEther[2] = 0;
    entry.macAddr.arEther[3] = 0;
    entry.macAddr.arEther[4] = 0;
    entry.macAddr.arEther[5] = 1;

    entry.isStatic  = GT_TRUE;
    entry.vlanId    = PRV_CPSS_VALID_VLANS_CNS;
    entry.saClass   = GT_TRUE;
    entry.daClass   = GT_TRUE;
    entry.saCib     = GT_TRUE;
    entry.daCib     = GT_TRUE;
    entry.daRoute   = GT_TRUE;
    entry.cosIpv4En = GT_FALSE;
    entry.mirrorToRxAnalyzerPortEn = GT_TRUE;

    entry.dstInterface.type = CPSS_INTERFACE_PORT_E;

    entry.dstInterface.devPort.devNum  = dev;
    entry.dstInterface.devPort.portNum = 0;

    entry.srcTc = 0;
    entry.dstTc = 0;

    entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
    entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMacAddrEntryRead
(
    IN GT_U8         dev,
    IN GT_U32        index,
    OUT GT_BOOL      *validPtr,
    OUT GT_BOOL      *skipPtr,
    OUT GT_BOOL      *agedPtr,
    OUT CPSS_MAC_ENTRY_STC  *entryPtr
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMacAddrEntryRead)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function cpssDxSalBrgFdbL2IngrProcEnable to disable the Layer 2 Ingress processor.
    Expected: GT_OK.
    1.2. Call function cpssDxSalBrgFdbMacAddrEntryWrite
    with index [0 / macTableSize(dev) - 1] , skip [GT_FALSE] and non-NULL entryPtr {macAddr [{0xAA,0xBB,0,0,0,0x01}]; dstInterface {type [CPSS_INTERFACE_TRUNK_E]; trunkId [2]}; isStatic [GT_TRUE]; vlanId [100]; srcTc [0]; dstTc [0]; daCommand [CPSS_MAC_TABLE_FRWRD_E]; saCommand [CPSS_MAC_TABLE_FRWRD_E]; saClass [GT_TRUE]; daClass [GT_TRUE]; saCib [GT_TRUE]; daCib [GT_TRUE]; daRoute [GT_TRUE];  cosIpv4En [GT_FALSE];  mirrorToRxAnalyzerPortEn [GT_FALSE]}.
    Expected: GT_OK.
    1.3. Call function with with index [0 / macTableSize(dev) - 1], non-NULL validPtr, non-NULL skipPtr, non-NULL agedPtr and entryPtr.
    Expected: GT_OK.
    1.4. Call function with with out of range index [macTableSize(dev)] and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.5. Call function with validPtr [NULL] and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call function with skipPtr [NULL] and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.7. Call function with agedPtr [NULL] and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.8. Call function with entryPtr [NULL] and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.9. Call cpssDxSalBrgFdbMacAddrEntryInvalidate
    with the index [0 / macTableSize(dev) - 1] to invalidate changes.
    Expected: GT_OK.
    1.10. Call function cpssDxSalBrgFdbL2IngrProcEnable to enable the Layer 2 Ingress processor.
    Expected: GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U32             index   = 0;
    GT_BOOL            valid   = GT_FALSE;
    GT_BOOL            skip    = GT_FALSE;
    GT_BOOL            aged    = GT_FALSE;
    GT_BOOL            enable  = GT_FALSE;
    CPSS_MAC_ENTRY_STC entry;
    GT_U32             maxMacIndex = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting mac table size */
        st = prvUtfMacTableSize(dev, &maxMacIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMacTableSize: %d", dev);

        /*
            1.1. Call function cpssDxSalBrgFdbL2IngrProcEnable to disable the Layer 2 Ingress processor.
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbL2IngrProcEnable: %d, %d", dev, enable);

        /* 
            1.2. Call function cpssDxSalBrgFdbMacAddrEntryWrite with
                                    index [0 / macTableSize(dev) - 1] ,
                                    skip [GT_FALSE]
                                    and non-NULL entryPtr {macAddr [{0xAA,0xBB,0,0,0,0x01}];
                                                           dstInterface {type [CPSS_INTERFACE_TRUNK_E]; trunkId [2]};
                                                           isStatic [GT_TRUE];
                                                           vlanId [100];
                                                           srcTc [0];
                                                           dstTc [0];
                                                           daCommand [CPSS_MAC_TABLE_FRWRD_E];
                                                           saCommand [CPSS_MAC_TABLE_FRWRD_E];
                                                           saClass [GT_TRUE];
                                                           daClass [GT_TRUE];
                                                           saCib [GT_TRUE];
                                                           daCib [GT_TRUE];
                                                           daRoute [GT_TRUE];
                                                           cosIpv4En [GT_FALSE];
                                                           mirrorToRxAnalyzerPortEn [GT_FALSE]}.
        */
        skip = GT_FALSE;

        entry.macAddr.arEther[0] = 0xAA;
        entry.macAddr.arEther[1] = 0xBB;
        entry.macAddr.arEther[2] = 0;
        entry.macAddr.arEther[3] = 0;
        entry.macAddr.arEther[4] = 0;
        entry.macAddr.arEther[5] = 1;

        entry.dstInterface.type    = CPSS_INTERFACE_TRUNK_E;
        entry.dstInterface.trunkId = 2;
        
        entry.isStatic  = GT_TRUE;
        entry.vlanId    = PRV_CPSS_VALID_VLANS_CNS;
        entry.srcTc     = 0;
        entry.dstTc     = 0;
        entry.daCommand = CPSS_MAC_TABLE_FRWRD_E;
        entry.saCommand = CPSS_MAC_TABLE_FRWRD_E;
        entry.saClass   = GT_TRUE;
        entry.daClass   = GT_TRUE;
        entry.saCib     = GT_TRUE;
        entry.daCib     = GT_TRUE;
        entry.daRoute   = GT_TRUE;
        entry.cosIpv4En = GT_FALSE;
        entry.mirrorToRxAnalyzerPortEn = GT_TRUE;

        /* Call with index = 0 */
        index = 0;
                
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        /* Call with index = macTableSize(dev) - 1 */
        index = maxMacIndex - 1;
                
        st = cpssDxSalBrgFdbMacAddrEntryWrite(dev, index, skip, &entry);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, skip);

        cpssOsBzero((GT_VOID*) &entry, sizeof(entry));

        /*
            1.3. Call function with with index [0 / macTableSize(dev) - 1],
                                         non-NULL validPtr,
                                         non-NULL skipPtr,
                                         non-NULL agedPtr and entryPtr.
            Expected: GT_OK.
        */
        /* Call with index = 0 */
        index = 0;
                
        st = cpssDxSalBrgFdbMacAddrEntryRead(dev, index, &valid, &skip, &aged, &entry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index = macTableSize(dev) - 1 */
        index = maxMacIndex - 1;
                
        st = cpssDxSalBrgFdbMacAddrEntryRead(dev, index, &valid, &skip, &aged, &entry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.4. Call function with out of range index [macTableSize(dev)]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        index = maxMacIndex;

        st = cpssDxSalBrgFdbMacAddrEntryRead(dev, index, &valid, &skip, &aged, &entry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.5. Call function with validPtr [NULL] and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacAddrEntryRead(dev, index, NULL, &skip, &aged, &entry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validPtr = NULL", dev);

        /*
            1.6. Call function with skipPtr [NULL] and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacAddrEntryRead(dev, index, &valid, NULL, &aged, &entry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, skipPtr = NULL", dev);

        /*
            1.7. Call function with agedPtr [NULL] and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacAddrEntryRead(dev, index, &valid, &skip, NULL, &entry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, agedPtr = NULL", dev);

        /*
            1.8. Call function with entryPtr [NULL] and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacAddrEntryRead(dev, index, &valid, &skip, &aged, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, entryPtr = NULL", dev);

        /*
            1.9. Call cpssDxSalBrgFdbMacAddrEntryInvalidate
            with the index [0 / macTableSize(dev) - 1] to invalidate changes.
            Expected: GT_OK.
        */

        /* Call with index = 0 */
        index = 0;

        st = cpssDxSalBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacAddrEntryInvalidate: %d, %d", dev, index);

        /* Call with index = macTableSize(dev) - 1 */
        index = maxMacIndex - 1;

        st = cpssDxSalBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacAddrEntryInvalidate: %d, %d", dev, index);

        /*
            1.10. Call function cpssDxSalBrgFdbL2IngrProcEnable to enable the Layer 2 Ingress processor.
            Expected: GT_OK.
        */

        enable = GT_TRUE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbL2IngrProcEnable: %d, %d", dev, enable);
    }

    index = 0;
        
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMacAddrEntryRead(dev, index, &valid, &skip, &aged, &entry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbMacAddrEntryRead(dev, index, &valid, &skip, &aged, &entry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMacAddrEntryInvalidate
(
    IN GT_U8         dev,
    IN GT_U32        index
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMacAddrEntryInvalidate)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function cpssDxSalBrgFdbL2IngrProcEnable to disable the Layer 2 Ingress processor.
    Expected: GT_OK.
    1.2. Call function with index [0 / macTableSize(dev) - 1].
    Expected: GT_OK.
    1.3. Call function with out of range index [macTableSize(dev)].
    Expected: NON GT_OK.
    1.4. Call function cpssDxSalBrgFdbL2IngrProcEnable to enable the Layer 2 Ingress processor.
    Expected: GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U32    index       = 0;
    GT_BOOL   enable      = GT_FALSE;
    GT_U32    maxMacIndex = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting mac table size */
        st = prvUtfMacTableSize(dev, &maxMacIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMacTableSize: %d", dev);

        /*
            1.1. Call function cpssDxSalBrgFdbL2IngrProcEnable to disable the Layer 2 Ingress processor.
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbL2IngrProcEnable: %d, %d", dev, enable);

        /* 
            1.2. Call function with index [0 / macTableSize(dev) - 1].
            Expected: GT_OK.
        */

        /* Call with index = 0 */
        index = 0;

        st = cpssDxSalBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index = macTableSize(dev) - 1 */
        index = maxMacIndex - 1;

        st = cpssDxSalBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.4. Call function with out of range index [macTableSize(dev)]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        index = maxMacIndex;

        st = cpssDxSalBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.5. Call function cpssDxSalBrgFdbL2IngrProcEnable to enable the Layer 2 Ingress processor.
            Expected: GT_OK.
        */

        enable = GT_TRUE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbL2IngrProcEnable: %d, %d", dev, enable);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMacAddrEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbMacAddrEntryInvalidate(dev, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbL2IngrProcEnable
(
    IN GT_U8         dev,
    IN GT_BOOL       enable
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbL2IngrProcEnable)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_BOOL   enable = GT_FALSE;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            Call function with enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with enable = GT_FALSE */
        enable = GT_FALSE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /* Call with enable = GT_TRUE */
        enable = GT_TRUE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMacAddrRawEntryRead
(
    IN GT_U8         devNum,
    IN GT_U32        index,
    OUT GT_U32       entryArray[CPSS_DXSAL_FDB_RAW_ENTRY_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMacAddrRawEntryRead)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function cpssDxSalBrgFdbL2IngrProcEnable to disable the Layer 2 Ingress processor.
    Expected: GT_OK.
    1.2. Call function with index [0 / macTableSize(dev) - 1]  and non-NULL entryArray.
    Expected: GT_OK.
    1.3. Call function with out of range index [MacTableSize(dev) and non-NULL entryArray.
    Expected: NON GT_OK.
    1.4. Call function with with index [0] and entryArray [NULL].
    Expected: GT_BAD_PTR.
    1.5. Call function cpssDxSalBrgFdbL2IngrProcEnable to enable the Layer 2 Ingress processor.
    Expected: GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U32    index       = 0; 
    GT_BOOL   enable      = GT_FALSE;
    GT_U32    entryArray[CPSS_DXSAL_FDB_RAW_ENTRY_SIZE_CNS];
    GT_U32    maxMacIndex = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting mac table size */
        st = prvUtfMacTableSize(dev, &maxMacIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMacTableSize: %d", dev);

        /*
            1.1. Call function cpssDxSalBrgFdbL2IngrProcEnable to disable the Layer 2 Ingress processor.
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbL2IngrProcEnable: %d, %d", dev, enable);

        /* 
            1.2. Call function with index [0 / macTableSize(dev) - 1]  and non-NULL entryArray.
            Expected: GT_OK.
        */

        /* Call with index = 0 */
        index = 0;

        st = cpssDxSalBrgFdbMacAddrRawEntryRead(dev, index, entryArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index = macTableSize(dev) - 1 */
        index = maxMacIndex - 1;

        st = cpssDxSalBrgFdbMacAddrRawEntryRead(dev, index, entryArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.3. Call function with out of range index [macTableSize(dev)]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        index = maxMacIndex;

        st = cpssDxSalBrgFdbMacAddrRawEntryRead(dev, index, entryArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.4. Call function with with index [0] and entryArray [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacAddrRawEntryRead(dev, index, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, entryArray = NULL", dev, index);

        /*
            1.5. Call function cpssDxSalBrgFdbL2IngrProcEnable to enable the Layer 2 Ingress processor.
            Expected: GT_OK.
        */

        enable = GT_TRUE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbL2IngrProcEnable: %d, %d", dev, enable);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMacAddrRawEntryRead(dev, index, entryArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbMacAddrRawEntryRead(dev, index, entryArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssDxSalBrgFdbMacAddrRawEntryWrite
(
    IN GT_U8         devNum,
    IN GT_U32        index,
    IN GT_U32        entryArray[CPSS_DXSAL_FDB_RAW_ENTRY_SIZE_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssDxSalBrgFdbMacAddrRawEntryWrite)
{
/*
    ITERATE_DEVICES(DxSal)
    1.1. Call function cpssDxSalBrgFdbL2IngrProcEnable to disable the Layer 2 Ingress processor.
    Expected: GT_OK.
    1.2. Call function with with index [0 / macTableSize(dev) - 1]  and non-NULL entryArray [{123,124,1,3}].
    Expected: GT_OK.
    1.3. Call cpssDxSalBrgFdbMacAddrRawEntryRead
    With the same index.
    Expected: GT_OK and the same entryArray.
    1.4. Call function with out of range index [MacTableSize(dev)] and non-NULL entryArray [{123,124,1,3}].
    Expected: NON GT_OK.
    1.5. Call function with entryArray [NULL].
    Expected: GT_BAD_PTR.
    1.6. Call function cpssDxSalBrgFdbL2IngrProcEnable to enable the Layer 2 Ingress processor.
    Expected: GT_OK.
*/
    GT_STATUS st   = GT_OK;
    GT_U8     dev;

    GT_U32    index       = 0;
    GT_BOOL   enable      = GT_FALSE;
    GT_U32    maxMacIndex = 0;
    GT_BOOL   isEqual     = GT_FALSE;
    GT_U32    entryArr[4];
    GT_U32    entryArrGet[4];

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* Getting mac table size */
        st = prvUtfMacTableSize(dev, &maxMacIndex);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfMacTableSize: %d", dev);

        /*
            1.1. Call function cpssDxSalBrgFdbL2IngrProcEnable to disable the Layer 2 Ingress processor.
            Expected: GT_OK.
        */
        enable = GT_FALSE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbL2IngrProcEnable: %d, %d", dev, enable);

        /* 
            1.2. Call function with with index [0 / macTableSize(dev) - 1]
                                         and non-NULL entryArray [{123,124,1,3}].
            Expected: GT_OK.
        */
        entryArr[0] = 123;
        entryArr[0] = 124;
        entryArr[0] = 1;
        entryArr[0] = 3;

        /* Call with index = 0 */
        index = 0;

        st = cpssDxSalBrgFdbMacAddrRawEntryWrite(dev, index, entryArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.3. Call cpssDxSalBrgFdbMacAddrRawEntryRead with the same index.
            Expected: GT_OK and the same entryArray.
        */
        st = cpssDxSalBrgFdbMacAddrRawEntryRead(dev, index, entryArrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacAddrRawEntryRead: %d, %d", dev, index);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &entryArr, (GT_VOID*) &entryArrGet, sizeof (entryArr)))? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                                     "get another entryArray than was set: %d, %d", dev, index);

        /* Call with index = macTableSize(dev) - 1 */
        index = maxMacIndex - 1;

        st = cpssDxSalBrgFdbMacAddrRawEntryWrite(dev, index, entryArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*
            1.3. Call cpssDxSalBrgFdbMacAddrRawEntryRead with the same index.
            Expected: GT_OK and the same entryArray.
        */
        st = cpssDxSalBrgFdbMacAddrRawEntryRead(dev, index, entryArrGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbMacAddrRawEntryRead: %d, %d", dev, index);

        /* Verifying values */
        isEqual = (0 == cpssOsMemCmp((GT_VOID*) &entryArr, (GT_VOID*) &entryArrGet, sizeof (entryArr)))? GT_TRUE : GT_FALSE;
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                                     "get another entryArray than was set: %d, %d", dev, index);

        /*
            1.4. Call function with out of range index [macTableSize(dev)]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        index = maxMacIndex;

        st = cpssDxSalBrgFdbMacAddrRawEntryWrite(dev, index, entryArr);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.5. Call function with with index [0] and entryArray [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssDxSalBrgFdbMacAddrRawEntryWrite(dev, index, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, entryArray = NULL", dev, index);

        /*
            1.6. Call function cpssDxSalBrgFdbL2IngrProcEnable to enable the Layer 2 Ingress processor.
            Expected: GT_OK.
        */

        enable = GT_TRUE;

        st = cpssDxSalBrgFdbL2IngrProcEnable(dev, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssDxSalBrgFdbL2IngrProcEnable: %d, %d", dev, enable);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_DXSAL_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssDxSalBrgFdbMacAddrRawEntryWrite(dev, index, entryArr);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssDxSalBrgFdbMacAddrRawEntryWrite(dev, index, entryArr);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssDxSalBrgFdb suit
*/
UTF_SUIT_BEGIN_TESTS_MAC(cpssDxSalBrgFdb)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMacAddrEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalFdbBrgCosIpv4EnWABitSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMacAddrEntryDel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMacVlanLookupModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbPortLearnSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbPortLearnStatusGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMacFilterPortEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMacFilterEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMacFilterEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbNaToCpuEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbAuToCpuSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbAuToCpuGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbQaSend)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbQiSend)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMaxLookupLenSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMacAddrEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMacAddrEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMacAddrEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbL2IngrProcEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMacAddrRawEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssDxSalBrgFdbMacAddrRawEntryWrite)
UTF_SUIT_END_TESTS_MAC(cpssDxSalBrgFdb)

/*******************************************************************************
* prvUtfMacTableSize
*
* DESCRIPTION:
*       This routine returns mac table size per device.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       tableSizePtr    - (pointer to) mac table size
*
*       GT_OK           - Get mac table size was OK
*       GT_BAD_PARAM    - Invalid device id
*       GT_BAD_PTR      - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS prvUtfMacTableSize
(
    IN  GT_U8       dev,
    OUT GT_U32      *tableSizePtr
)
{
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(tableSizePtr);

    /* check if dev active and from DxSal family */
    PRV_CPSS_DXSAL_DEV_CHECK_MAC(dev);

    *tableSizePtr = PRV_CPSS_DXSAL_PP_MAC(dev)->moduleCfg.bridgeCfg.macTableSize;

    return(GT_OK);
}

