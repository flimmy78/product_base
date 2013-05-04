/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxTgIpUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxTgIp, that provides
*       the CPSS EXMX Tiger Ip HW structures APIs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */
#include <cpss/exMx/exMxTg/ip/cpssExMxTgIp.h>

#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define TG_IP_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define TG_IP_TESTED_VLAN_ID_CNS  100

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpMcRuleSet
(
    IN GT_U8                        dev,
    IN GT_U32                       ruleIndex,
    IN GT_BOOL                      isValid,
    IN CPSS_EXMX_PCL_IP_MC_KEY_STC  *maskPtr,
    IN CPSS_EXMX_PCL_IP_MC_KEY_STC  *patternPtr,
    IN CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC  *ipMcActionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpMcRuleSet)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call with ruleIndex[0 / 2047],
                   isValid [GT_TRUE],
                   maskPtr [0xFF, …, 0xFF],
                   patternPtr{pclId [0],
                              fwdEnable [GT_TRUE],
                              mngEnable [GT_TRUE],
                              portLifNum [0],
                              dip [A5:24:72:D3:2C:80:DD:02:00:29:EC:7A:00:2B:EA:73],
                              vid [100]},
                   ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E],
                                  destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E],
                                  dest {ssmNode {type [CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E],
                                                 lpmIndex [0] } } }.
    Expected: GT_OK.
    1.2. Call with ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_ASM_E],
                                  destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_LINK_LOCAL_E],
                                  dest {asmRoute {routeEntryBaseMemAddr [0],
                                                  blockSize [1],
                                                  routeEntryMethod [CPSS_EXMX_IP_DEF_ROUTE_ENTRY_GROUP_E]} } }
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.3. Call cpssExMxTgIpMcRuleGet with same ruleIndex.
    Expected: GT_OK and the same values.
    1.4. Call with out of range patternPtr->vid [4096]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.5. Call with out of range ipMcActionPtr->command [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range ipMcActionPtr->destScope [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with out of range ipMcActionPtr->ssmNode.
                   Type [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with out of range ipMcActionPtr->ssmNode.
                   lpmIndex [0xFFFFFFFF] (no any constraints)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.9. Call with out of range ipMcActionPtr->asmRoute.
                   routeEntryMethod [0x5AAAAAA5]
                   and other parameters from 1.2.
    Expected: GT_BAD_PARAM.
    1.10. Call with isValid [GT_FALSE],
                    maskPtr [NULL] (not relevant when isValid = GT_FALSE)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.11. Call with isValid [GT_FALSE],
                    patternPtr [NULL] (not relevant when isValid = GT_FALSE)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.12. Call with isValid [GT_FALSE],
                    ipMcActionPtr [NULL] (not relevant when isValid = GT_FALSE)
                    and other parameters from 1.1.
    Expected: GT_OK.
    1.13. Call with out of range ruleIndex [2048]
                    and other parameters from 1.1.
    Expected: NON GT_OK.
    1.14. Call with isValid [GT_TRUE],
                    maskPtr [NULL] (relevant when isValid = GT_ TRUE)
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.15. Call with isValid [GT_ TRUE],
                    patternPtr [NULL] (relevant when isValid = GT_ TRUE)
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.16. Call with isValid [GT_ TRUE],
                    ipMcActionPtr [NULL] (relevant when isValid = GT_ TRUE)
                    and other parameters from 1.1.
    Expected: GT_BAD_PTR.
    1.17. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E],
                                              ruleIndex [0 / 2047] to cleanup after testing.
    Expected: GT_OK.
*/
    GT_STATUS                         st = GT_OK;

    GT_U8                             dev;
    GT_U32                            ruleIndex  = 0;
    GT_BOOL                           isValid    = GT_FALSE;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       mask;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       pattern;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcAction;
    GT_BOOL                           isValidGet = GT_FALSE;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       maskGet;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       patternGet;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcActionGet;
    CPSS_EXMX_PCL_RULE_SIZE_ENT       ruleSize   = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
    GT_BOOL                           isEqual    = GT_FALSE;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with ruleIndex[0 / 2047],
                           isValid [GT_TRUE],
                           maskPtr [0xFF, …, 0xFF],
                           patternPtr{pclId [0],
                                      fwdEnable [GT_TRUE],
                                      mngEnable [GT_TRUE],
                                      portLifNum [0],
                                      dip [A5:24:72:D3:2C:80:DD:02:00:29:EC:7A:00:2B:EA:73],
                                      vid [100]},
                           ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E],
                                          destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E],
                                          dest {ssmNode {type [CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E],
                                                         lpmIndex [0] } } }.
            Expected: GT_OK.
        */
        isValid   = GT_TRUE;

        /*cpssOsBzero((GT_VOID*) &mask, sizeof(mask));*/
        cpssOsMemSet((GT_VOID*) &mask, 0xFF, sizeof(mask));
        
        cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));

        pattern.pclId      = 0;
        pattern.fwdEnable  = GT_TRUE;
        pattern.mngEnable  = GT_TRUE;
        pattern.portLifNum = 0;
        pattern.vid        = TG_IP_TESTED_VLAN_ID_CNS;

        pattern.dip[0]  = 0xA5;
        pattern.dip[1]  = 0x24;
        pattern.dip[2]  = 0x72;
        pattern.dip[3]  = 0xD3;
        pattern.dip[4]  = 0x2C;
        pattern.dip[5]  = 0x80;
        pattern.dip[6]  = 0xDD;
        pattern.dip[7]  = 0x2;
        pattern.dip[8]  = 0;
        pattern.dip[9]  = 0x29;
        pattern.dip[10] = 0xEC;
        pattern.dip[11] = 0x7A;
        pattern.dip[12] = 0;
        pattern.dip[13] = 0x2B;
        pattern.dip[14] = 0xEA;
        pattern.dip[15] = 0x73;

        cpssOsBzero((GT_VOID*) &ipMcAction, sizeof(ipMcAction));

        ipMcAction.command   = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E;
        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E;

        ipMcAction.dest.ssmNode.type     = CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E;
        ipMcAction.dest.ssmNode.lpmIndex = 0;

        /* Call with ruleIndex [0] */
        ruleIndex = 0;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, isValid);

        /*
            1.3. Call cpssExMxTgIpMcRuleGet with same ruleIndex.
            Expected: GT_OK and the same values.
        */
        st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &ipMcActionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxTgIpMcRuleGet: %d, %d", dev, ruleIndex);

        if (GT_OK == st)
        {
            /* Verifying isValid value */
            UTF_VERIFY_EQUAL1_STRING_MAC(isValid, isValidGet,
                       "get another isValid than was set: %d", dev);

            /* Verifying maskPtr value */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask,
                                         (GT_VOID*) &maskGet,
                                         sizeof(mask))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another maskPtr than was set: %d", dev);

            /* Verifying patternPtr value */
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.pclId, patternGet.pclId,
                       "get another patternPtr->pclId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.fwdEnable, patternGet.fwdEnable,
                       "get another patternPtr->fwdEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.mngEnable, patternGet.mngEnable,
                       "get another patternPtr->mngEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.portLifNum, patternGet.portLifNum,
                       "get another patternPtr->portLifNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.vid, patternGet.vid,
                       "get another patternPtr->vid than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.dip,
                                         (GT_VOID*) &patternGet.dip,
                                         sizeof(pattern.dip))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another patternPtr->dip than was set: %d", dev);

            /* Verifying ipMcActionPtr value */
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.command, ipMcActionGet.command,
                       "get another ipMcActionPtr->command than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.destScope, ipMcActionGet.destScope,
                       "get another ipMcActionPtr->destScope than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.dest.ssmNode.type, ipMcActionGet.dest.ssmNode.type,
                       "get another ipMcActionPtr->dest.ssmNode.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.dest.ssmNode.lpmIndex, ipMcActionGet.dest.ssmNode.lpmIndex,
                       "get another ipMcActionPtr->dest.ssmNode.lpmIndex than was set: %d", dev);
        }

        /* Call with ruleIndex [2047] */
        ruleIndex = 2047;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, isValid);

        /*
            1.3. Call cpssExMxTgIpMcRuleGet with same ruleIndex.
            Expected: GT_OK and the same values.
        */
        st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &ipMcActionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxTgIpMcRuleGet: %d, %d", dev, ruleIndex);

        if (GT_OK == st)
        {
            /* Verifying isValid value */
            UTF_VERIFY_EQUAL1_STRING_MAC(isValid, isValidGet,
                       "get another isValid than was set: %d", dev);

            /* Verifying maskPtr value */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask,
                                         (GT_VOID*) &maskGet,
                                         sizeof(mask))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another maskPtr than was set: %d", dev);

            /* Verifying patternPtr value */
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.pclId, patternGet.pclId,
                       "get another patternPtr->pclId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.fwdEnable, patternGet.fwdEnable,
                       "get another patternPtr->fwdEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.mngEnable, patternGet.mngEnable,
                       "get another patternPtr->mngEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.portLifNum, patternGet.portLifNum,
                       "get another patternPtr->portLifNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.vid, patternGet.vid,
                       "get another patternPtr->vid than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.dip,
                                         (GT_VOID*) &patternGet.dip,
                                         sizeof(pattern.dip))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another patternPtr->dip than was set: %d", dev);

            /* Verifying ipMcActionPtr value */
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.command, ipMcActionGet.command,
                       "get another ipMcActionPtr->command than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.destScope, ipMcActionGet.destScope,
                       "get another ipMcActionPtr->destScope than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.dest.ssmNode.type, ipMcActionGet.dest.ssmNode.type,
                       "get another ipMcActionPtr->dest.ssmNode.type than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.dest.ssmNode.lpmIndex, ipMcActionGet.dest.ssmNode.lpmIndex,
                       "get another ipMcActionPtr->dest.ssmNode.lpmIndex than was set: %d", dev);
        }

        /*
            1.2. Call with ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_ASM_E],
                                          destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_LINK_LOCAL_E],
                                          dest {asmRoute {routeEntryBaseMemAddr [0],
                                                          blockSize [1],
                                                          routeEntryMethod [CPSS_EXMX_IP_DEF_ROUTE_ENTRY_GROUP_E]} } }
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        ipMcAction.command   = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_ASM_E;
        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_LINK_LOCAL_E;

        ipMcAction.dest.asmRoute.routeEntryBaseMemAddr = 0;
        ipMcAction.dest.asmRoute.blockSize             = 1;
        ipMcAction.dest.asmRoute.routeEntryMethod      = CPSS_EXMX_IP_DEF_ROUTE_ENTRY_GROUP_E;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, ruleIndex, isValid);

        /*
            1.3. Call cpssExMxTgIpMcRuleGet with same ruleIndex.
            Expected: GT_OK and the same values.
        */
        st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &ipMcActionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxTgIpMcRuleGet: %d, %d", dev, ruleIndex);

        if (GT_OK == st)
        {
            /* Verifying isValid value */
            UTF_VERIFY_EQUAL1_STRING_MAC(isValid, isValidGet,
                       "get another isValid than was set: %d", dev);

            /* Verifying maskPtr value */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &mask,
                                         (GT_VOID*) &maskGet,
                                         sizeof(mask))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another maskPtr than was set: %d", dev);

            /* Verifying patternPtr value */
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.pclId, patternGet.pclId,
                       "get another patternPtr->pclId than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.fwdEnable, patternGet.fwdEnable,
                       "get another patternPtr->fwdEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.mngEnable, patternGet.mngEnable,
                       "get another patternPtr->mngEnable than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.portLifNum, patternGet.portLifNum,
                       "get another patternPtr->portLifNum than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(pattern.vid, patternGet.vid,
                       "get another patternPtr->vid than was set: %d", dev);

            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &pattern.dip,
                                         (GT_VOID*) &patternGet.dip,
                                         sizeof(pattern.dip))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isEqual,
                       "get another patternPtr->dip than was set: %d", dev);

            /* Verifying ipMcActionPtr value */
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.command, ipMcActionGet.command,
                       "get another ipMcActionPtr->command than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.destScope, ipMcActionGet.destScope,
                       "get another ipMcActionPtr->destScope than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.dest.asmRoute.routeEntryBaseMemAddr, ipMcActionGet.dest.asmRoute.routeEntryBaseMemAddr,
                       "get another ipMcActionPtr->dest.asmRoute.routeEntryBaseMemAddr than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.dest.asmRoute.blockSize, ipMcActionGet.dest.asmRoute.blockSize,
                       "get another ipMcActionPtr->dest.asmRoute.blockSize than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.dest.asmRoute.routeEntryMethod, ipMcActionGet.dest.asmRoute.routeEntryMethod,
                       "get another ipMcActionPtr->dest.asmRoute.routeEntryMethod than was set: %d", dev);
        }

        /*
            1.4. Call with out of range patternPtr->vid [4096]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ruleIndex = 0;

        ipMcAction.command   = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E;
        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E;

        ipMcAction.dest.ssmNode.type     = CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E;
        ipMcAction.dest.ssmNode.lpmIndex = 0;

        pattern.vid = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, patternPtr->vid = %d", dev, ruleIndex, pattern.vid);

        pattern.vid = TG_IP_TESTED_VLAN_ID_CNS;

        /*
            1.5. Call with out of range ipMcActionPtr->command [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ipMcAction.command = TG_IP_INVALID_ENUM_CNS;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, ipMcActionPtr->command = %d",
                                     dev, ruleIndex, ipMcAction.command);

        ipMcAction.command = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E;

        /*
            1.6. Call with out of range ipMcActionPtr->destScope [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ipMcAction.destScope= TG_IP_INVALID_ENUM_CNS;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, ipMcActionPtr->destScope = %d",
                                     dev, ruleIndex, ipMcAction.destScope);

        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E;

        /*
            1.7. Call with out of range ipMcActionPtr->ssmNode.
                           Type [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ipMcAction.dest.ssmNode.type = TG_IP_INVALID_ENUM_CNS;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, ipMcActionPtr->dest.ssmNode.type = %d, ipMcActionPtr->command",
                                     dev, ruleIndex, ipMcAction.dest.ssmNode.type, ipMcAction.command);

        ipMcAction.dest.ssmNode.type = CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E;

        /*
            1.8. Call with out of range ipMcActionPtr->ssmNode.
                        lpmIndex [0xFFFFFFFF] (no any constraints)
                        and other parameters from 1.1.
            Expected: GT_OK.
        */
        ipMcAction.dest.ssmNode.lpmIndex = 0xFFFFFFFF;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, ipMcActionPtr->dest.ssmNode.lpmIndex = %d, ipMcActionPtr->command",
                                     dev, ruleIndex, ipMcAction.dest.ssmNode.lpmIndex, ipMcAction.command);

        ipMcAction.dest.ssmNode.lpmIndex = 0;

        /*
            1.9. Call with out of range ipMcActionPtr->asmRoute.
                           routeEntryMethod [0x5AAAAAA5]
                           and other parameters from 1.2.
            Expected: GT_BAD_PARAM.
        */
        ipMcAction.command   = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_ASM_E;
        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_LINK_LOCAL_E;

        ipMcAction.dest.asmRoute.routeEntryBaseMemAddr = 0;
        ipMcAction.dest.asmRoute.blockSize             = 1;
        ipMcAction.dest.asmRoute.routeEntryMethod      = TG_IP_INVALID_ENUM_CNS;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, ipMcActionPtr->dest.asmRoute.routeEntryMethod = %d, ipMcActionPtr->command",
                                     dev, ruleIndex, ipMcAction.dest.asmRoute.routeEntryMethod, ipMcAction.command);

        ipMcAction.dest.asmRoute.routeEntryMethod = CPSS_EXMX_IP_DEF_ROUTE_ENTRY_GROUP_E;

        /*
            1.10. Call with isValid [GT_FALSE],
                            maskPtr [NULL] (not relevant when isValid = GT_FALSE)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        ipMcAction.command   = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E;
        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E;

        ipMcAction.dest.ssmNode.type     = CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E;
        ipMcAction.dest.ssmNode.lpmIndex = 0;

        isValid = GT_FALSE;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, NULL, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, isValid = %d, maskPtr = NULL", dev, isValid);

        /*
            1.11. Call with isValid [GT_FALSE],
                            patternPtr [NULL] (not relevant when isValid = GT_FALSE)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, NULL, &ipMcAction);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, isValid = %d, patternPtr = NULL", dev, isValid);

        /*
            1.12. Call with isValid [GT_FALSE],
                            ipMcActionPtr [NULL] (not relevant when isValid = GT_FALSE)
                            and other parameters from 1.1.
            Expected: GT_OK.
        */
        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, isValid = %d, ipMcActionPtr = NULL", dev, isValid);

        /*
            1.13. Call with out of range ruleIndex [2048]
                            and other parameters from 1.1.
            Expected: NON GT_OK.
        */
        ruleIndex = 2048;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.14. Call with isValid [GT_TRUE],
                            maskPtr [NULL] (relevant when isValid = GT_ TRUE)
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        isValid = GT_TRUE;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, NULL, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, isValid = %d, maskPtr = NULL", dev, isValid);

        /*
            1.15. Call with isValid [GT_ TRUE],
                            patternPtr [NULL] (relevant when isValid = GT_ TRUE)
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, NULL, &ipMcAction);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, isValid = %d, patternPtr = NULL", dev, isValid);

        /*
            1.16. Call with isValid [GT_ TRUE],
                            ipMcActionPtr [NULL] (relevant when isValid = GT_ TRUE)
                            and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, isValid = %d, ipMcActionPtr = NULL", dev, isValid);

        /*
            1.17. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E],
                                                      ruleIndex [0 / 2047] to cleanup after testing.
            Expected: GT_OK.
        */
        ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

        /* Call with ruleIndex [0] */
        ruleIndex = 0;

        st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);

        /* Call with ruleIndex [2047] */
        ruleIndex = 2047;

        st = cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
    }

    isValid   = GT_TRUE;
    ruleIndex = 0;

    cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
    cpssOsMemSet((GT_VOID*) &(mask), 0xFF, sizeof(mask));
            
    cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));
    
    pattern.pclId      = 0;
    pattern.fwdEnable  = GT_TRUE;
    pattern.mngEnable  = GT_TRUE;
    pattern.portLifNum = 0;
    pattern.vid        = TG_IP_TESTED_VLAN_ID_CNS;
    
    pattern.dip[0]  = 0xA5;
    pattern.dip[1]  = 0x24;
    pattern.dip[2]  = 0x72;
    pattern.dip[3]  = 0xD3;
    pattern.dip[4]  = 0x2C;
    pattern.dip[5]  = 0x80;
    pattern.dip[6]  = 0xDD;
    pattern.dip[7]  = 0x2;
    pattern.dip[8]  = 0;
    pattern.dip[9]  = 0x29;
    pattern.dip[10] = 0xEC;
    pattern.dip[11] = 0x7A;
    pattern.dip[12] = 0;
    pattern.dip[13] = 0x2B;
    pattern.dip[14] = 0xEA;
    pattern.dip[15] = 0x73;

    cpssOsBzero((GT_VOID*) &ipMcAction, sizeof(ipMcAction));
    
    ipMcAction.command   = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E;
    ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E;
    
    ipMcAction.dest.ssmNode.type     = CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E;
    ipMcAction.dest.ssmNode.lpmIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpMcRuleActionUpdate
(
    IN GT_U8                              dev,
    IN GT_U32                             ruleIndex,
    IN CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC  *ipMcActionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpMcRuleActionUpdate)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call cpssExMxTgIpMcRuleSet with ruleIndex[0 / 2047],
                                         isValid [GT_TRUE],
                                         maskPtr [0xFF, …, 0xFF],
                                         patternPtr{pclId [0],
                                                    fwdEnable [GT_TRUE],
                                                    mngEnable [GT_TRUE],
                                                    portLifNum [0],
                                                    dip [A5:24:72:D3:2C:80:DD:02:00:29:EC:7A:00:2B:EA:73],
                                                    vid [100]},
                                         ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E],
                                                        destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E],
                                                        dest {ssmNode {type [CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E],
                                                                       lpmIndex [0] } } } to set IP MC Rule Action.
    Expected: GT_OK.
    1.2. Call with ruleIndex [0 / 2047]
                   and ipMcActionPtr the same as in 1.1.
    Expected: GT_OK.
    1.3. Call with ruleIndex [0]
                   and with ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_ASM_E],
                                           destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_LINK_LOCAL_E],
                                           dest {asmRoute {routeEntryBaseMemAddr [0],
                                                           blockSize [1],
                                                           routeEntryMethod [CPSS_EXMX_IP_DEF_ROUTE_ENTRY_GROUP_E]} } }.
    Expected: GT_OK.
    1.4. Call cpssExMxTgIpMcRuleActionGet with same ruleIndex.
    Expected: GT_OK and the updated values.
    1.5. Call with out of range ipMcActionPtr->command [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.6. Call with out of range ipMcActionPtr->destScope [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call with out of range ipMcActionPtr->ssmNode.Type [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call with out of range ipMcActionPtr->ssmNode.lpmIndex [0xFFFFFFFF] (no any constraints)
                   and other parameters from 1.1.
    Expected: GT_OK.
    1.9. Call with out of range ipMcActionPtr->asmRoute.routeEntryMethod [0x5AAAAAA5]
                   and other parameters from 1.3.
    Expected: GT_BAD_PARAM.
    1.10. Call with out of range ruleIndex [2048]
                    and other parameters from 1.2.
    Expected: NON GT_OK.
    1.11. Call with ruleIndex[0],
                    ipMcActionPtr [NULL].
    Expected: GT_BAD_PTR. 
    1.12. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E],
                                              ruleIndex [0 / 2047] to cleanup after testing.
    Expected: GT_OK.
*/
    GT_STATUS                         st = GT_OK;

    GT_U8                             dev;
    GT_U32                            ruleIndex  = 0;
    GT_BOOL                           isValid    = GT_FALSE;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       mask;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       pattern;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcAction;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcActionGet;
    CPSS_EXMX_PCL_RULE_SIZE_ENT       ruleSize   = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxTgIpMcRuleSet with ruleIndex[0 / 2047],
                                                 isValid [GT_TRUE],
                                                 maskPtr [0xFF, …, 0xFF],
                                                 patternPtr{pclId [0],
                                                            fwdEnable [GT_TRUE],
                                                            mngEnable [GT_TRUE],
                                                            portLifNum [0],
                                                            dip [A5:24:72:D3:2C:80:DD:02:00:29:EC:7A:00:2B:EA:73],
                                                            vid [100]},
                                                 ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E],
                                                                destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E],
                                                                dest {ssmNode {type [CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E],
                                                                               lpmIndex [0] } } } to set IP MC Rule Action.
            Expected: GT_OK.
        */
        isValid   = GT_TRUE;

        cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
        cpssOsMemSet((GT_VOID*) &(mask), 0xFF, sizeof(mask));
        
        cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));

        pattern.pclId      = 0;
        pattern.fwdEnable  = GT_TRUE;
        pattern.mngEnable  = GT_TRUE;
        pattern.portLifNum = 0;
        pattern.vid        = TG_IP_TESTED_VLAN_ID_CNS;

        pattern.dip[0]  = 0xA5;
        pattern.dip[1]  = 0x24;
        pattern.dip[2]  = 0x72;
        pattern.dip[3]  = 0xD3;
        pattern.dip[4]  = 0x2C;
        pattern.dip[5]  = 0x80;
        pattern.dip[6]  = 0xDD;
        pattern.dip[7]  = 0x2;
        pattern.dip[8]  = 0;
        pattern.dip[9]  = 0x29;
        pattern.dip[10] = 0xEC;
        pattern.dip[11] = 0x7A;
        pattern.dip[12] = 0;
        pattern.dip[13] = 0x2B;
        pattern.dip[14] = 0xEA;
        pattern.dip[15] = 0x73;

        cpssOsBzero((GT_VOID*) &ipMcAction, sizeof(ipMcAction));

        ipMcAction.command   = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E;
        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E;

        ipMcAction.dest.ssmNode.type     = CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E;
        ipMcAction.dest.ssmNode.lpmIndex = 0;

        /* Call with ruleIndex [0] */
        ruleIndex = 0;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxTgIpMcRuleSet: %d, %d, %d",dev, ruleIndex, isValid);

        /*
            1.2. Call with ruleIndex [0 / 2047]
                           and ipMcActionPtr the same as in 1.1.
            Expected: GT_OK.
        */

        /* Call with ruleIndex [0] */
        st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcAction);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /* Call with ruleIndex [2047] */
        ruleIndex = 2047;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxTgIpMcRuleSet: %d, %d, %d",dev, ruleIndex, isValid);

        /* Call with ruleIndex [2047] */
        st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcAction);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /*
            1.3. Call with ruleIndex [0]
                           and with ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_ASM_E],
                                                   destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_LINK_LOCAL_E],
                                                   dest {asmRoute {routeEntryBaseMemAddr [0],
                                                                   blockSize [1],
                                                                   routeEntryMethod [CPSS_EXMX_IP_DEF_ROUTE_ENTRY_GROUP_E]} } }.
            Expected: GT_OK.
        */
        ruleIndex = 0;

        ipMcAction.command = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_ASM_E;
        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_LINK_LOCAL_E;

        ipMcAction.dest.asmRoute.routeEntryBaseMemAddr = 0;
        ipMcAction.dest.asmRoute.blockSize             = 1;
        ipMcAction.dest.asmRoute.routeEntryMethod      = CPSS_EXMX_IP_DEF_ROUTE_ENTRY_GROUP_E;

        st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcAction);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /*
            1.4. Call cpssExMxTgIpMcRuleActionGet with same ruleIndex.
            Expected: GT_OK and the updated values.
        */
        st = cpssExMxTgIpMcRuleActionGet(dev, ruleIndex, &ipMcActionGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxTgIpMcRuleActionGet: %d, %d", dev, ruleIndex);

        /* Verifying ipMcActionPtr value */
        UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.command, ipMcActionGet.command,
                   "get another ipMcActionPtr->command than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.destScope, ipMcActionGet.destScope,
                   "get another ipMcActionPtr->destScope than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.dest.asmRoute.routeEntryBaseMemAddr, ipMcActionGet.dest.asmRoute.routeEntryBaseMemAddr,
                   "get another ipMcActionPtr->dest.asmRoute.routeEntryBaseMemAddr than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.dest.asmRoute.blockSize, ipMcActionGet.dest.asmRoute.blockSize,
                   "get another ipMcActionPtr->dest.asmRoute.blockSize than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(ipMcAction.dest.asmRoute.routeEntryMethod, ipMcActionGet.dest.asmRoute.routeEntryMethod,
                   "get another ipMcActionPtr->dest.asmRoute.routeEntryMethod than was set: %d", dev);

        /*
            1.5. Call with out of range ipMcActionPtr->command [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ipMcAction.command   = TG_IP_INVALID_ENUM_CNS;
        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E;

        ipMcAction.dest.ssmNode.type     = CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E;
        ipMcAction.dest.ssmNode.lpmIndex = 0;

        st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, ipMcActionPtr->command = %d",
                                     dev, ruleIndex, ipMcAction.command);

        ipMcAction.command = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E;

        /*
            1.6. Call with out of range ipMcActionPtr->destScope [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ipMcAction.destScope = TG_IP_INVALID_ENUM_CNS;

        st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, ipMcActionPtr->destScope = %d",
                                     dev, ruleIndex, ipMcAction.destScope);

        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E;

        /*
            1.7. Call with out of range ipMcActionPtr->ssmNode.Type [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        ipMcAction.dest.ssmNode.type = TG_IP_INVALID_ENUM_CNS;

        st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, ipMcActionPtr->dest.ssmNode.type = %d",
                                     dev, ruleIndex, ipMcAction.dest.ssmNode.type);

        ipMcAction.dest.ssmNode.type = CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E;

        /*
            1.8. Call with out of range ipMcActionPtr->ssmNode.lpmIndex [0xFFFFFFFF] (no any constraints)
                           and other parameters from 1.1.
            Expected: GT_OK.
        */
        ipMcAction.dest.ssmNode.lpmIndex = 0xFFFFFFFF;

        st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, ipMcActionPtr->dest.ssmNode.lpmIndex = %d",
                                     dev, ruleIndex, ipMcAction.dest.ssmNode.lpmIndex);

        ipMcAction.dest.ssmNode.lpmIndex = 0;

        /*
            1.9. Call with out of range ipMcActionPtr->asmRoute.routeEntryMethod [0x5AAAAAA5]
                           and other parameters from 1.3.
            Expected: GT_BAD_PARAM.
        */
        ipMcAction.command = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_ASM_E;
        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_LINK_LOCAL_E;

        ipMcAction.dest.asmRoute.routeEntryBaseMemAddr = 0;
        ipMcAction.dest.asmRoute.blockSize             = 1;
        ipMcAction.dest.asmRoute.routeEntryMethod      = TG_IP_INVALID_ENUM_CNS;

        st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, ipMcActionPtr->dest.asmRoute.routeEntryMethod = %d",
                                     dev, ruleIndex, ipMcAction.dest.asmRoute.routeEntryMethod);

        ipMcAction.dest.asmRoute.routeEntryMethod = CPSS_EXMX_IP_DEF_ROUTE_ENTRY_GROUP_E;

        /*
            1.10. Call with out of range ruleIndex [2048]
                            and other parameters from 1.2.
            Expected: NON GT_OK.
        */
        ruleIndex = 2048;

        st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcAction);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.11. Call with ruleIndex[0],
                            ipMcActionPtr [NULL].
            Expected: GT_BAD_PTR. 
        */
        st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipMcActionPtr = NULL", dev);

        /*
            1.12. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E],
                                                      ruleIndex [0 / 2047] to cleanup after testing.
            Expected: GT_OK.
        */

        ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

        /* Call with ruleIndex [0] */
        ruleIndex = 0;

        cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);

        /* Call with ruleIndex [2047] */
        ruleIndex = 2047;

        cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
    }

    ruleIndex = 0;

    cpssOsBzero((GT_VOID*) &ipMcAction, sizeof(ipMcAction));

    ipMcAction.command   = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E;
    ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E;

    ipMcAction.dest.ssmNode.type     = CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E;
    ipMcAction.dest.ssmNode.lpmIndex = 0;

    st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxTgIpMcRuleSet: %d, %d, %d",dev, ruleIndex, isValid);

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcAction);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpMcRuleActionUpdate(dev, ruleIndex, &ipMcAction);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpv6McPclIdSet
(
    IN GT_U8                              dev,
    IN GT_U32                             pclId
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpv6McPclIdSet)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call with pclId [0].
    Expected: GT_OK.
    1.2. Call with pclId [0xFFFFFFFF] (no any constraints).
    Expected: GT_OK.
*/
    GT_STATUS  st    = GT_OK;

    GT_U8      dev;
    GT_U32     pclId = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with pclId [0].
            Expected: GT_OK.
        */
        pclId = 0;

        st = cpssExMxTgIpv6McPclIdSet(dev, pclId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, pclId);

        /*
            1.2. Call with pclId [0xFFFFFFFF] (no any constraints).
            Expected: GT_OK.
        */
        pclId = 0xFFFFFFFF;

        st = cpssExMxTgIpv6McPclIdSet(dev, pclId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, pclId);
    }

    pclId = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpv6McPclIdSet(dev, pclId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpv6McPclIdSet(dev, pclId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpv6McDefaultActionIndexSet
(
    IN GT_U8                              dev,
    IN GT_U32                             actionIndex
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpv6McDefaultActionIndexSet)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call with actionIndex [0].
    Expected: GT_OK.
    1.2. Call with actionIndex [0xFFFFFFFF] (no any constraints).
    Expected: GT_OK.
*/
    GT_STATUS       st          = GT_OK;

    GT_U8           dev;
    GT_U32          actionIndex = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call with actionIndex [0].
            Expected: GT_OK.
        */
        actionIndex = 0;

        st = cpssExMxTgIpv6McDefaultActionIndexSet(dev, actionIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, actionIndex);

        /*
            1.2. Call with actionIndex [0xFFFFFFFF] (no any constraints).
            Expected: GT_OK.
        */
        actionIndex = 0xFFFFFFFF;

        st = cpssExMxTgIpv6McDefaultActionIndexSet(dev, actionIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, actionIndex);
    }

    actionIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpv6McDefaultActionIndexSet(dev, actionIndex);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpv6McDefaultActionIndexSet(dev, actionIndex);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpMcRuleActionGet
(
    IN  GT_U8                             dev,
    IN  GT_U32                            ruleIndex,
    OUT CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC *ipMcActionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpMcRuleActionGet)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call cpssExMxTgIpMcRuleSet with ruleIndex[0 / 2047],
                                         isValid [GT_TRUE],
                                         maskPtr [0xFF, …, 0xFF],
                                         patternPtr{pclId [0],
                                                    fwdEnable [GT_TRUE],
                                                    mngEnable [GT_TRUE],
                                                    portLifNum [0],
                                                    dip [A5:24:72:D3:2C:80:DD:02:00:29:EC:7A:00:2B:EA:73],
                                                    vid [100]},
                                         ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E],
                                                        destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E],
                                                        dest{ssmNode{type[CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E],
                                                                     lpmIndex [0] } } } to set IP MC Rule Action.
    Expected: GT_OK.
    1.2. Call with ruleIndex [0 / 2047]
                   and non-null ipMcActionPtr.
    Expected: GT_OK.
    1.3. Call with out of range ruleIndex [2048]
                   and other parameters from 1.2.
    Expected: NON GT_OK.
    1.4. Call with ruleIndex[0],
                   ipMcActionPtr [NULL].
    Expected: GT_BAD_PTR. 
    1.5. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E],
                                             ruleIndex [0 / 2047] to cleanup after testing.
    Expected: GT_OK.
*/
    GT_STATUS                         st = GT_OK;

    GT_U8                             dev;
    GT_U32                            ruleIndex  = 0;
    GT_BOOL                           isValid    = GT_FALSE;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       mask;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       pattern;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcAction;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcActionGet;
    CPSS_EXMX_PCL_RULE_SIZE_ENT       ruleSize   = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxTgIpMcRuleSet with ruleIndex[0 / 2047],
                                                 isValid [GT_TRUE],
                                                 maskPtr [0xFF, …, 0xFF],
                                                 patternPtr{pclId [0],
                                                            fwdEnable [GT_TRUE],
                                                            mngEnable [GT_TRUE],
                                                            portLifNum [0],
                                                            dip [A5:24:72:D3:2C:80:DD:02:00:29:EC:7A:00:2B:EA:73],
                                                            vid [100]},
                                                 ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E],
                                                                destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E],
                                                                dest{ssmNode{type[CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E],
                                                                             lpmIndex [0] } } } to set IP MC Rule Action.
            Expected: GT_OK.
        */
        isValid = GT_TRUE;

        cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
        cpssOsMemSet((GT_VOID*) &(mask), 0xFF, sizeof(mask));
        
        cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));

        pattern.pclId      = 0;
        pattern.fwdEnable  = GT_TRUE;
        pattern.mngEnable  = GT_TRUE;
        pattern.portLifNum = 0;
        pattern.vid        = TG_IP_TESTED_VLAN_ID_CNS;

        pattern.dip[0]  = 0xA5;
        pattern.dip[1]  = 0x24;
        pattern.dip[2]  = 0x72;
        pattern.dip[3]  = 0xD3;
        pattern.dip[4]  = 0x2C;
        pattern.dip[5]  = 0x80;
        pattern.dip[6]  = 0xDD;
        pattern.dip[7]  = 0x2;
        pattern.dip[8]  = 0;
        pattern.dip[9]  = 0x29;
        pattern.dip[10] = 0xEC;
        pattern.dip[11] = 0x7A;
        pattern.dip[12] = 0;
        pattern.dip[13] = 0x2B;
        pattern.dip[14] = 0xEA;
        pattern.dip[15] = 0x73;

        cpssOsBzero((GT_VOID*) &ipMcAction, sizeof(ipMcAction));

        ipMcAction.command   = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E;
        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E;

        ipMcAction.dest.ssmNode.type     = CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E;
        ipMcAction.dest.ssmNode.lpmIndex = 0;

        /* Call with ruleIndex [0] */
        ruleIndex = 0;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,"cpssExMxTgIpMcRuleSet: %d, %d, %d", dev, ruleIndex, isValid);

        /*
            1.2. Call with ruleIndex [0 / 2047]
                           and non-null ipMcActionPtr.
            Expected: GT_OK.
        */

        /* Call with ruleIndex [0] */
        st = cpssExMxTgIpMcRuleActionGet(dev, ruleIndex, &ipMcActionGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /* Call with ruleIndex [2047] */
        ruleIndex = 2047;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st,"cpssExMxTgIpMcRuleSet: %d, %d, %d", dev, ruleIndex, isValid);

        /* Call with ruleIndex [2047] */
        st = cpssExMxTgIpMcRuleActionGet(dev, ruleIndex, &ipMcActionGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /*
            1.3. Call with out of range ruleIndex [2048]
                           and other parameters from 1.2.
            Expected: NON GT_OK.
        */
        ruleIndex = 2048;

        st = cpssExMxTgIpMcRuleActionGet(dev, ruleIndex, &ipMcActionGet);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.4. Call with ruleIndex[0],
                           ipMcActionPtr [NULL].
            Expected: GT_BAD_PTR. 
        */
        st = cpssExMxTgIpMcRuleActionGet(dev, ruleIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipMcActionPtr = NULL", dev);

        /*
            1.5. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E],
                                                     ruleIndex [0 / 2047] to cleanup after testing.
            Expected: GT_OK.
        */
        ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

        /* Call with ruleIndex [0] */
        ruleIndex = 0;

        cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);

        /* Call with ruleIndex [2047] */
        ruleIndex = 2047;

        cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
    }

    ruleIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpMcRuleActionGet(dev, ruleIndex, &ipMcActionGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpMcRuleActionGet(dev, ruleIndex, &ipMcActionGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxTgIpMcRuleGet
(
    IN  GT_U8                              dev,
    IN  GT_U32                             ruleIndex,
    OUT GT_BOOL                            *isValidPtr,
    OUT CPSS_EXMX_PCL_IP_MC_KEY_STC        *maskPtr,
    OUT CPSS_EXMX_PCL_IP_MC_KEY_STC        *patternPtr,
    OUT CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC  *ipMcActionPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTgIpMcRuleGet)
{
/*
    ITERATE_DEVICES (ExMxTg)
    1.1. Call cpssExMxTgIpMcRuleSet with ruleIndex[0 / 2047],
                                         isValid [GT_TRUE],
                                         maskPtr [0xFF, …, 0xFF],
                                         patternPtr{pclId [0],
                                                    fwdEnable [GT_TRUE],
                                                    mngEnable [GT_TRUE],
                                                    portLifNum [0],
                                                    dip [A5:24:72:D3:2C:80:DD:02:00:29:EC:7A:00:2B:EA:73],
                                                    vid [100]},
                                                    ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E],
                                                                   destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E],
                                                                   dest {ssmNode {type [CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E],
                                                                                  lpmIndex [0] } } } to set IP MC Rule Action.
    Expected: GT_OK.
    1.2. Call with ruleIndex [0 / 2047]
                   and non-null isValidPtr,
                   non-null maskPtr,
                   non-null patternPtr
                   and non-null ipMcActionPtr.
    Expected: GT_OK.
    1.3. Call with out of range ruleIndex [2048]
                   and other parameters from 1.2.
    Expected: NON GT_OK.
    1.4. Call with ruleIndex[0],
                   isValidPtr [NULL]
                   and other non-null pointers.
    Expected: GT_BAD_PTR. 
    1.5. Call with ruleIndex[0],
                   maskPtr [NULL]
                   and other non-null pointers.
    Expected: GT_BAD_PTR.
    1.6. Call with ruleIndex[0],
                   patternPtr [NULL]
                   and other non-null pointers.
    Expected: GT_BAD_PTR.
    1.7. Call with ruleIndex[0],
                   ipMcActionPtr [NULL]
                   and other non-null pointers.
    Expected: GT_BAD_PTR.
    1.8. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E],
                                             ruleIndex [0 / 2047] to cleanup after testing.
    Expected: GT_OK.
*/
    GT_STATUS                         st = GT_OK;

    GT_U8                             dev;
    GT_U32                            ruleIndex  = 0;
    GT_BOOL                           isValid    = GT_FALSE;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       mask;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       pattern;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcAction;
    GT_BOOL                           isValidGet = GT_FALSE;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       maskGet;
    CPSS_EXMX_PCL_IP_MC_KEY_STC       patternGet;
    CPSS_EXMX_TG_PCL_IP_MC_ACTION_STC ipMcActionGet;
    CPSS_EXMX_PCL_RULE_SIZE_ENT       ruleSize   = CPSS_EXMX_PCL_RULE_SIZE_STD_E;
    

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxTgIpMcRuleSet with ruleIndex[0 / 2047],
                                                 isValid [GT_TRUE],
                                                 maskPtr [0xFF, …, 0xFF],
                                                 patternPtr{pclId [0],
                                                            fwdEnable [GT_TRUE],
                                                            mngEnable [GT_TRUE],
                                                            portLifNum [0],
                                                            dip [A5:24:72:D3:2C:80:DD:02:00:29:EC:7A:00:2B:EA:73],
                                                            vid [100]},
                                                            ipMcActionPtr {command [CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E],
                                                                           destScope [CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E],
                                                                           dest {ssmNode {type [CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E],
                                                                                          lpmIndex [0] } } } to set IP MC Rule Action.
            Expected: GT_OK.
        */
        isValid   = GT_TRUE;

        cpssOsBzero((GT_VOID*) &mask, sizeof(mask));
        cpssOsMemSet((GT_VOID*) &(mask), 0xFF, sizeof(mask));
        
        cpssOsBzero((GT_VOID*) &pattern, sizeof(pattern));

        pattern.pclId      = 0;
        pattern.fwdEnable  = GT_TRUE;
        pattern.mngEnable  = GT_TRUE;
        pattern.portLifNum = 0;
        pattern.vid        = TG_IP_TESTED_VLAN_ID_CNS;

        pattern.dip[0]  = 0xA5;
        pattern.dip[1]  = 0x24;
        pattern.dip[2]  = 0x72;
        pattern.dip[3]  = 0xD3;
        pattern.dip[4]  = 0x2C;
        pattern.dip[5]  = 0x80;
        pattern.dip[6]  = 0xDD;
        pattern.dip[7]  = 0x2;
        pattern.dip[8]  = 0;
        pattern.dip[9]  = 0x29;
        pattern.dip[10] = 0xEC;
        pattern.dip[11] = 0x7A;
        pattern.dip[12] = 0;
        pattern.dip[13] = 0x2B;
        pattern.dip[14] = 0xEA;
        pattern.dip[15] = 0x73;

        cpssOsBzero((GT_VOID*) &ipMcAction, sizeof(ipMcAction));

        ipMcAction.command   = CPSS_EXMX_TG_PCL_IP_MC_ACTION_CMD_SSM_E;
        ipMcAction.destScope = CPSS_EXMX_TG_PCL_IP_MC_ACTION_DST_GLOBAL_E;

        ipMcAction.dest.ssmNode.type     = CPSS_EXMX_TG_PCL_IP_MC_ACTION_NODE_REGULAR_E;
        ipMcAction.dest.ssmNode.lpmIndex = 0;

        /* Call with ruleIndex [0] */
        ruleIndex = 0;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxTgIpMcRuleSet: %d, %d, %d", dev, ruleIndex, isValid);

        /*
            1.2. Call with ruleIndex [0 / 2047]
                           and non-null isValidPtr,
                           non-null maskPtr,
                           non-null patternPtr
                           and non-null ipMcActionPtr.
            Expected: GT_OK.
        */

        /* Call with ruleIndex [0] */
        st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &ipMcActionGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /* Call with ruleIndex [2047] */
        ruleIndex = 2047;

        st = cpssExMxTgIpMcRuleSet(dev, ruleIndex, isValid, &mask, &pattern, &ipMcAction);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxTgIpMcRuleSet: %d, %d, %d", dev, ruleIndex, isValid);

        /* Call with ruleIndex [2047] */
        st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &ipMcActionGet);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        /*
            1.3. Call with out of range ruleIndex [2048]
                           and other parameters from 1.2.
            Expected: NON GT_OK.
        */
        ruleIndex = 2048;

        st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &ipMcActionGet);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, ruleIndex);

        ruleIndex = 0;

        /*
            1.4. Call with ruleIndex[0],
                           isValidPtr [NULL]
                           and other non-null pointers.
            Expected: GT_BAD_PTR. 
        */
        st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, NULL, &maskGet, &patternGet, &ipMcActionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, isValidPtr = NULL", dev);

        /*
            1.5. Call with ruleIndex[0],
                           maskPtr [NULL]
                           and other non-null pointers.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, &isValidGet, NULL, &patternGet, &ipMcActionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, maskPtr = NULL", dev);

        /*
            1.6. Call with ruleIndex[0],
                           patternPtr [NULL]
                           and other non-null pointers.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, &isValidGet, &maskGet, NULL, &ipMcActionGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, patternPtr = NULL", dev);

        /*
            1.7. Call with ruleIndex[0],
                           ipMcActionPtr [NULL]
                           and other non-null pointers.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, ipMcActionPtr = NULL", dev);

        /*
            1.17. Call cpssExMxPclRuleInvalidate with ruleSize [CPSS_EXMX_PCL_RULE_SIZE_STD_E],
                                                      ruleIndex [0 / 2047] to cleanup after testing.
            Expected: GT_OK.
        */
        ruleSize = CPSS_EXMX_PCL_RULE_SIZE_STD_E;

        /* Call with ruleIndex [0] */
        ruleIndex = 0;

        cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);

        /* Call with ruleIndex [2047] */
        ruleIndex = 2047;

        cpssExMxPclRuleInvalidate(dev, ruleSize, ruleIndex);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPclRuleInvalidate: %d, %d, %d", dev, ruleSize, ruleIndex);
    }

    ruleIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &ipMcActionGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTgIpMcRuleGet(dev, ruleIndex, &isValidGet, &maskGet, &patternGet, &ipMcActionGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxTgIp suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxTgIp)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpMcRuleSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpMcRuleActionUpdate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpv6McPclIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpv6McDefaultActionIndexSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpMcRuleActionGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTgIpMcRuleGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxTgIp)

