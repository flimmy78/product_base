/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmVlanUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgVlan, that provides
*       CPSS ExMxPm VLAN facility API
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

/*#include <cpss/generic/cos/cpssCosTypes.h>*/
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgVlan.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#include <gtOs/gtOsMem.h>

/* defines */

/* Default valid value for port id */
#define BRG_VLAN_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define BRG_VLAN_INVALID_ENUM_CNS    0x5AAAAAA5

/* Tests use this vlan id for testing VLAN functions */
#define BRG_VLAN_TESTED_VLAN_ID_CNS  100

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanEntryWrite
(
    IN GT_U8                            devNum,
    IN GT_U16                           vlanId,
    IN CPSS_PORTS_BMP_STC               *portsMembersPtr,
    IN CPSS_PORTS_BMP_STC               *portsTaggingPtr,
    IN CPSS_EXMXPM_BRG_VLAN_INFO_STC    *vlanInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanEntryWrite)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with vlanId [100 / 4095],
                            non-NULL portsMembersPtr [{5, 10}],
                            non-NULL portsTaggingPtr [{10, 5}]
                            and non-NULL vlanInfoPtr {cpuMember [GT_TRUE / GT_FALSE];
                                                      mirrToRxAnalyzerEn [GT_TRUE / GT_FALSE];
                                                      ipv4IpmBrgMode [CPSS_BRG_IPM_SGV_E / CPSS_BRG_IPM_GV_E] (relevant when ipv4IpmBrgEn = GT_TRUE);
                                                      ipv6IpmBrgMode [CPSS_BRG_IPM_SGV_E / CPSS_BRG_IPM_GV_E] (relevant when ipv6IpmBrgEn = GT_TRUE);
                                                      ipv4IpmBrgEn [GT_TRUE / GT_FALSE];
                                                      ipv6IpmBrgEn [GT_TRUE / GT_FALSE];
                                                      stgId [0 / 255];
                                                      egressUnregMcFilterCmd [CPSS_UNREG_MC_VLAN_FRWD_E / CPSS_UNREG_MC_VLAN_CPU_FRWD_E]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgVlanEntryRead with parameters the same as in 1.1.
                                              and non-NULL isValidPtr.
    Expected: GT_OK, isValidPtr=GT_TRUE and the same portsMembersPtr, portsTaggingPtr and vlanInfoPtr.
    1.3. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call function with portsMembersPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call function with portsTaggingPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call function with out of range vlanInfoPtr->ipv4IpmBrgMode [0x5AAAAAA5],
                            vlanInfoPtr->ipv4IpmBrgEn [GT_TRUE / GT_FALSE]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.7. Call function with out of range vlanInfoPtr->ipv6IpmBrgMode [0x5AAAAAA5],
                            vlanInfoPtr->ipv6IpmBrgEn [GT_TRUE / GT_FALSE]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.8. Call function with out of range vlanInfoPtr->stgId [256]
                             and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.10. Call with out of range portsMembersPtr [{56}]
                    and other parameters the same as in 1.1.
    Expected: GT_OUT_OF_RANGE.
    1.11. Call with out of range portsTaggingPtr [{56}]
                    and other parameters the same as in 1.1.
    Expected: GT_OUT_OF_RANGE.
    1.12. Call with vlanInfoPtr->egressUnregMcFilterCmd [0x5AAAAAA5] (out of range)
                    and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.13. Call function with vlanInfoPtr [NULL]
                             and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.14. Call cpssExMxPmBrgVlanEntryInvalidate with vlanId the same as in 1.1.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16                        vlanId = 0;
    CPSS_PORTS_BMP_STC            portsMembers;
    CPSS_PORTS_BMP_STC            portsTagging;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC vlanInfo;
    CPSS_PORTS_BMP_STC            portsMembersGet;
    CPSS_PORTS_BMP_STC            portsTaggingGet;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC vlanInfoGet;
    GT_BOOL                       isValid = GT_FALSE;
    GT_BOOL                       isEqual = GT_FALSE;
    GT_U32                        portsBitmap = 0;
    GT_U32                        portsMembersVal = 10; /* ports 33 & 35 (assign to bitmap 1)*/
    GT_U32                        portsTaggingVal = 5;  /* ports 32 & 34 (assign to bitmap 1)*/
    GT_U32                        mask = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with vlanId [100 / 4095],
                                    non-NULL portsMembersPtr [{5, 10}],
                                    non-NULL portsTaggingPtr [{10, 5}]
                                    and non-NULL vlanInfoPtr {cpuMember [GT_TRUE / GT_FALSE];
                                                              mirrToRxAnalyzerEn [GT_TRUE / GT_FALSE];
                                                              ipv4IpmBrgMode [CPSS_BRG_IPM_SGV_E / CPSS_BRG_IPM_GV_E] (relevant when ipv4IpmBrgEn = GT_TRUE);
                                                              ipv6IpmBrgMode [CPSS_BRG_IPM_SGV_E / CPSS_BRG_IPM_GV_E] (relevant when ipv6IpmBrgEn = GT_TRUE);
                                                              ipv4IpmBrgEn [GT_TRUE / GT_FALSE];
                                                              ipv6IpmBrgEn [GT_TRUE / GT_FALSE];
                                                              stgId [0 / 255];
                                                              egressUnregMcFilterCmd [CPSS_UNREG_MC_VLAN_FRWD_E / CPSS_UNREG_MC_VLAN_CPU_FRWD_E]}.
            Expected: GT_OK.
        */       
        /* bitmap 1 is relevant only if more then 32 ports are defined */
        if (PRV_CPSS_PP_MAC(dev)->numOfVirtPorts > 32) 
        {
            mask = 1 << (PRV_CPSS_PP_MAC(dev)->numOfVirtPorts - 32);
            portsBitmap |= (mask - 1);
        }                

        /* check that the ports in the test are defined in the system */
        if ((portsBitmap < portsMembersVal) | (portsBitmap < portsTaggingVal))
        {           
            portsMembersVal = 0;
            portsTaggingVal = 0;
        }

        portsMembers.ports[0] = 5;                /* ports 0 & 2 */  
        portsMembers.ports[1] = portsMembersVal;  /* ports 33 & 35 */
        portsTagging.ports[0] = 10;               /* ports 1 & 3 */
        portsTagging.ports[1] = portsTaggingVal;  /* ports 32 & 34 */

        /* Call with vlanId = 100 */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        
        vlanInfo.cpuMember              = GT_FALSE;
        vlanInfo.mirrorToTxAnalyzerEn   = GT_FALSE;
        vlanInfo.ipv4IpmBrgMode         = CPSS_BRG_IPM_SGV_E;
        vlanInfo.ipv6IpmBrgMode         = CPSS_BRG_IPM_SGV_E;
        vlanInfo.ipv4IpmBrgEn           = GT_FALSE;
        vlanInfo.ipv6IpmBrgEn           = GT_FALSE;
        vlanInfo.stgId                  = 0;
        vlanInfo.egressUnregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;
        
        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /*
            1.2. Call cpssExMxPmBrgVlanEntryRead with parameters the same as in 1.1.
                                              and non-NULL isValidPtr.
            Expected: GT_OK, isValidPtr=GT_TRUE and the same portsMembersPtr, portsTaggingPtr and vlanInfoPtr.
        */
        st = cpssExMxPmBrgVlanEntryRead(dev, vlanId, &portsMembersGet,
                                        &portsTaggingGet, &vlanInfoGet, &isValid);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanEntryRead: %d, %d", dev, vlanId);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isValid, "Vlan entry is NOT valid: %d, %d", dev, vlanId);

        if (GT_TRUE == isValid)
        {
            /* Verifying portsMembersPtr */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsMembers,
                                         (GT_VOID*) &portsMembersGet,
                                         sizeof(portsMembers))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                       "get another portsMembersPtr than was set: %d, %d", dev, vlanId);

            /* Verifying portsTaggingPtr */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsTagging,
                                         (GT_VOID*) &portsTaggingGet,
                                         sizeof(portsTagging))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                       "get another portsTaggingPtr than was set: %d, %d", dev, vlanId);

            /* Verifying vlanInfoPtr fields */
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.cpuMember,
                                         vlanInfoGet.cpuMember,
                                         "get another vlanInfoPtr->cpuMember than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.mirrorToTxAnalyzerEn,
                                         vlanInfoGet.mirrorToTxAnalyzerEn,
                                         "get another vlanInfoPtr->mirrorToTxAnalyzerEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgEn,
                                         vlanInfoGet.ipv4IpmBrgEn,
                                         "get another vlanInfoPtr->ipv4IpmBrgEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgEn,
                                         vlanInfoGet.ipv6IpmBrgEn,
                                         "get another vlanInfoPtr->ipv6IpmBrgEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.stgId,
                                         vlanInfoGet.stgId,
                                         "get another vlanInfoPtr->stgId than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.egressUnregMcFilterCmd,
                                         vlanInfoGet.egressUnregMcFilterCmd,
                                         "get another vlanInfoPtr->egressUnregMcFilterCmd than was set: %d, %d", dev, vlanId);

            if (GT_TRUE == vlanInfo.ipv4IpmBrgEn)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgMode,
                                             vlanInfoGet.ipv4IpmBrgMode,
                                             "get another vlanInfoPtr->ipv4IpmBrgMode than was set: %d, %d", dev, vlanId);
            }

            if (GT_TRUE == vlanInfo.ipv6IpmBrgEn)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgMode,
                                             vlanInfoGet.ipv6IpmBrgMode,
                                             "get another vlanInfoPtr->ipv6IpmBrgMode than was set: %d, %d", dev, vlanId);
            }
        }

        /* Call with vlanId = 4095 */
        vlanId = 4095;
        
        vlanInfo.cpuMember              = GT_TRUE;
        vlanInfo.mirrorToTxAnalyzerEn   = GT_TRUE;
        vlanInfo.ipv4IpmBrgMode         = CPSS_BRG_IPM_GV_E;
        vlanInfo.ipv6IpmBrgMode         = CPSS_BRG_IPM_GV_E;
        vlanInfo.ipv4IpmBrgEn           = GT_TRUE;
        vlanInfo.ipv6IpmBrgEn           = GT_TRUE;
        vlanInfo.stgId                  = 255;
        vlanInfo.egressUnregMcFilterCmd = CPSS_UNREG_MC_VLAN_CPU_FRWD_E;
        
        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /*
            1.2. Call cpssExMxPmBrgVlanEntryRead with parameters the same as in 1.1.
                                              and non-NULL isValidPtr.
            Expected: GT_OK, isValidPtr=GT_TRUE and the same portsMembersPtr, portsTaggingPtr and vlanInfoPtr.
        */
        st = cpssExMxPmBrgVlanEntryRead(dev, vlanId, &portsMembersGet,
                                        &portsTaggingGet, &vlanInfoGet, &isValid);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanEntryRead: %d, %d", dev, vlanId);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isValid, "Vlan entry is NOT valid: %d, %d", dev, vlanId);

        if (GT_TRUE == isValid)
        {
            /* Verifying portsMembersPtr */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsMembers,
                                         (GT_VOID*) &portsMembersGet,
                                         sizeof(portsMembers))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                       "get another portsMembersPtr than was set: %d, %d", dev, vlanId);

            /* Verifying portsTaggingPtr */
            isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsTagging,
                                         (GT_VOID*) &portsTaggingGet,
                                         sizeof(portsTagging))) ? GT_TRUE : GT_FALSE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                       "get another portsTaggingPtr than was set: %d, %d", dev, vlanId);

            /* Verifying vlanInfoPtr fields */
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.cpuMember,
                                         vlanInfoGet.cpuMember,
                                         "get another vlanInfoPtr->cpuMember than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.mirrorToTxAnalyzerEn,
                                         vlanInfoGet.mirrorToTxAnalyzerEn,
                                         "get another vlanInfoPtr->mirrorToTxAnalyzerEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgEn,
                                         vlanInfoGet.ipv4IpmBrgEn,
                                         "get another vlanInfoPtr->ipv4IpmBrgEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgEn,
                                         vlanInfoGet.ipv6IpmBrgEn,
                                         "get another vlanInfoPtr->ipv6IpmBrgEn than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.stgId,
                                         vlanInfoGet.stgId,
                                         "get another vlanInfoPtr->stgId than was set: %d, %d", dev, vlanId);
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.egressUnregMcFilterCmd,
                                         vlanInfoGet.egressUnregMcFilterCmd,
                                         "get another vlanInfoPtr->egressUnregMcFilterCmd than was set: %d, %d", dev, vlanId);

            if (GT_TRUE == vlanInfo.ipv4IpmBrgEn)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgMode,
                                             vlanInfoGet.ipv4IpmBrgMode,
                                             "get another vlanInfoPtr->ipv4IpmBrgMode than was set: %d, %d", dev, vlanId);
            }

            if (GT_TRUE == vlanInfo.ipv6IpmBrgEn)
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgMode,
                                             vlanInfoGet.ipv6IpmBrgMode,
                                             "get another vlanInfoPtr->ipv6IpmBrgMode than was set: %d, %d", dev, vlanId);
            }
        }

        /*
            1.3. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.4. Call function with portsMembersPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, NULL,
                                         &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsMembersPtr = NULL", dev);

        /*
            1.5. Call function with portsTaggingPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         NULL, &vlanInfo);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsTaggingPtr = NULL", dev);

        /*
            1.6. Call function with out of range vlanInfoPtr->ipv4IpmBrgMode [0x5AAAAAA5],
                                    vlanInfoPtr->ipv4IpmBrgEn [GT_TRUE / GT_FALSE]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanInfo.ipv4IpmBrgMode = BRG_VLAN_INVALID_ENUM_CNS;

        /* Call with vlanInfoPtr->ipv4IpmBrgEn [GT_TRUE] */
        vlanInfo.ipv4IpmBrgEn = GT_TRUE;

        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, vlanInfoPtr->ipv4IpmBrgMode = %d, vlanInfoPtr->ipv4IpmBrgEn = %d",
                                     dev, vlanInfo.ipv4IpmBrgMode, vlanInfo.ipv4IpmBrgEn);

        /* Call with vlanInfoPtr->ipv4IpmBrgEn [GT_FALSE] */
        vlanInfo.ipv4IpmBrgEn = GT_FALSE;

        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, vlanInfoPtr->ipv4IpmBrgMode = %d, vlanInfoPtr->ipv4IpmBrgEn = %d",
                                     dev, vlanInfo.ipv4IpmBrgMode, vlanInfo.ipv4IpmBrgEn);

        vlanInfo.ipv4IpmBrgMode = CPSS_BRG_IPM_SGV_E;

        /*
            1.7. Call function with out of range vlanInfoPtr->ipv6IpmBrgMode [0x5AAAAAA5],
                                    vlanInfoPtr->ipv6IpmBrgEn [GT_TRUE / GT_FALSE]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanInfo.ipv6IpmBrgMode = BRG_VLAN_INVALID_ENUM_CNS;

        /* Call with vlanInfoPtr->ipv6IpmBrgEn [GT_TRUE] */
        vlanInfo.ipv6IpmBrgEn = GT_TRUE;

        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, vlanInfoPtr->ipv6IpmBrgMode = %d, vlanInfoPtr->ipv6IpmBrgEn = %d",
                                     dev, vlanInfo.ipv6IpmBrgMode, vlanInfo.ipv6IpmBrgEn);

        /* Call with vlanInfoPtr->ipv6IpmBrgEn [GT_FALSE] */
        vlanInfo.ipv6IpmBrgEn = GT_FALSE;

        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, vlanInfoPtr->ipv6IpmBrgMode = %d, vlanInfoPtr->ipv6IpmBrgEn = %d",
                                     dev, vlanInfo.ipv6IpmBrgMode, vlanInfo.ipv6IpmBrgEn);

        vlanInfo.ipv6IpmBrgMode = CPSS_BRG_IPM_GV_E;

        /*
            1.8. Call function with out of range vlanInfoPtr->stgId [256]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanInfo.stgId = 256;

        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, vlanInfoPtr->stgId = %d", dev, vlanInfo.stgId);

        vlanInfo.stgId = 0;

        /*
            1.10. Call with out of range portsMembersPtr [{32}]
                            and other parameters the same as in 1.1.
            Expected: GT_OUT_OF_RANGE.
        */

        /* Call with portsMembersPtr->ports[0] = {32} */
        if (PRV_CPSS_PP_MAC(dev)->numOfVirtPorts < 32 )
        {
            portsMembers.ports[0] = (GT_U32)(1 << 31); /* port 32 */

            st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, &vlanInfo);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, portsMembersPtr->ports[0] = %d", dev, portsMembers.ports[0]);
        }
       
        portsMembers.ports[0] = 0;

        /* Call with portsMembersPtr->ports[1] = {56} */
        if ((PRV_CPSS_PP_MAC(dev)->numOfVirtPorts > 32) & (PRV_CPSS_PP_MAC(dev)->numOfVirtPorts < 64))
        {
            portsMembers.ports[1] = 1 << 24; /* port 56 */

            st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                             &portsTagging, &vlanInfo);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, portsMembersPtr->ports[1] = %d", dev, portsMembers.ports[1]);

            portsMembers.ports[1] = 0;
        }
        /*
            1.11. Call with out of range portsTaggingPtr [{32}]
                            and other parameters the same as in 1.1.
            Expected: GT_OUT_OF_RANGE.
        */
        /* Call with portsTaggingPtr->ports[0] = {32} */
        if (PRV_CPSS_PP_MAC(dev)->numOfVirtPorts < 32 )
        {
            portsTagging.ports[0] = (GT_U32)(1 << 31); /* port 32 */

            st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                             &portsTagging, &vlanInfo);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, portsTaggingPtr->ports[0] = %d", dev, portsTagging.ports[0]);
        }

        portsTagging.ports[0] = 0;

        /* Call with portsTaggingPtr->ports[1] = {56} */
        if (PRV_CPSS_PP_MAC(dev)->numOfVirtPorts > 32)
        {
            portsTagging.ports[1] = 1 << 24; /* port 56 */

            st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                             &portsTagging, &vlanInfo);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OUT_OF_RANGE, st, "%d, portsTaggingPtr->ports[1] = %d", dev, portsTagging.ports[1]);

            portsTagging.ports[1] = 0;
        }

        /*
            1.12. Call with vlanInfoPtr->egressUnregMcFilterCmd [0x5AAAAAA5] (out of range)
                            and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanInfo.egressUnregMcFilterCmd = BRG_VLAN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers, &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, vlanInfoPtr->egressUnregMcFilterCmd = %d", 
                                     dev, vlanInfo.egressUnregMcFilterCmd);

        vlanInfo.egressUnregMcFilterCmd = CPSS_UNREG_MC_VLAN_CPU_FRWD_E;

        /*
            1.13. Call function with vlanInfoPtr [NULL]
                                     and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vlanInfoPtr = NULL", dev);

        /* 
            1.14. Call cpssExMxPmBrgVlanEntryInvalidate with vlanId the same as in 1.1.
            Expected: GT_OK.
        */

        /* Call with vlanId = 100 */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxPmBrgVlanEntryInvalidate(dev, vlanId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmBrgVlanEntryInvalidate: %d, %d", dev, vlanId);

        /* Call with vlanId = 4095 */
        vlanId = 4095;

        st = cpssExMxPmBrgVlanEntryInvalidate(dev, vlanId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmBrgVlanEntryInvalidate: %d, %d", dev, vlanId);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

    portsMembers.ports[0] = 5;
    portsMembers.ports[1] = portsMembersVal;
    portsTagging.ports[0] = 10;
    portsTagging.ports[1] = portsTaggingVal;

    vlanInfo.cpuMember              = GT_FALSE;
    vlanInfo.mirrorToTxAnalyzerEn   = GT_FALSE;
    vlanInfo.ipv4IpmBrgMode         = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv6IpmBrgMode         = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv4IpmBrgEn           = GT_FALSE;
    vlanInfo.ipv6IpmBrgEn           = GT_FALSE;
    vlanInfo.stgId                  = 0;
    vlanInfo.egressUnregMcFilterCmd = CPSS_UNREG_MC_VLAN_CPU_FRWD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, &vlanInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmBrgVlanEntryWrite(dev, vlanId, &portsMembers,
                                         &portsTagging, &vlanInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanEntryRead
(
    IN  GT_U8                           devNum,
    IN  GT_U16                          vlanId,
    OUT CPSS_PORTS_BMP_STC              *portsMembersPtr,
    OUT CPSS_PORTS_BMP_STC              *portsTaggingPtr,
    OUT CPSS_EXMXPM_BRG_VLAN_INFO_STC   *vlanInfoPtr,
    OUT GT_BOOL                         *isValidPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanEntryRead)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with vlanId [100 / 4095],
                            non-NULL portsMembersPtr,
                            non-NULL portsTaggingPtr,
                            non-NULL vlanInfoPtr
                            and non-NULL isValidPtr.
    Expected: GT_OK.
    1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call function with portsMembersPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call function with portsTaggingPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call function with vlanInfoPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call function with isValidPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16                        vlanId = 0;
    CPSS_PORTS_BMP_STC            portsMembers;
    CPSS_PORTS_BMP_STC            portsTagging;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC vlanInfo;
    GT_BOOL                       isValid = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with vlanId [100 / 4095],
                                    non-NULL portsMembersPtr,
                                    non-NULL portsTaggingPtr,
                                    non-NULL vlanInfoPtr
                                    and non-NULL isValidPtr.
            Expected: GT_OK.
        */

        /* Call with vlanId = 100 */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxPmBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                        &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /* Call with vlanId = 4095 */
        vlanId = 4095;

        st = cpssExMxPmBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                        &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /*
            1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                        &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.3. Call function with portsMembersPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanEntryRead(dev, vlanId, NULL, &portsTagging,
                                        &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsMembersPtr = NULL", dev);

        /*
            1.4. Call function with portsTaggingPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanEntryRead(dev, vlanId, &portsMembers, NULL,
                                        &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portsTaggingPtr = NULL", dev);

        /*
            1.5. Call function with vlanInfoPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                        NULL, &isValid);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vlanInfoPtr = NULL", dev);

        /*
            1.6. Call function with isValidPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                        &vlanInfo, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, isValidPtr = NULL", dev);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                        &vlanInfo, &isValid);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanEntryRead(dev, vlanId, &portsMembers, &portsTagging,
                                        &vlanInfo, &isValid);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanEntryInvalidate
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanEntryInvalidate)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with vlanId [100 / 4095].
    Expected: GT_OK.
    1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16 vlanId = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with vlanId [100 / 4095].
            Expected: GT_OK.
        */

        /* Call with vlanId = 100 */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxPmBrgVlanEntryInvalidate(dev, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /* Call with vlanId = 4095 */
        vlanId = 4095;

        st = cpssExMxPmBrgVlanEntryInvalidate(dev, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /*
            1.2. Call with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmBrgVlanEntryInvalidate(dev, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanEntryInvalidate(dev, vlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanEntryInvalidate(dev, vlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanMemberSet
(
    IN GT_U8   devNum,
    IN GT_U16  vlanId,
    IN GT_U8   portNum,
    IN GT_BOOL isMember,
    IN GT_BOOL isTagged
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanMemberSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with vlanId [100 / 4095],
                              isMember [GT_TRUE / GT_FALSE]
                              and isTagged [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                              and other parameters the same as in 1.1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_U16  vlanId   = 0;
    GT_BOOL isMember = GT_FALSE;
    GT_BOOL isTagged = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with vlanId [100 / 4095],
                                          isMember [GT_TRUE / GT_FALSE]
                                          and isTagged [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call function with vlanId [100], isMember [GT_FALSE], isTagged [GT_FALSE] */
            vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
            isMember = GT_FALSE;
            isTagged = GT_FALSE;
            
            st = cpssExMxPmBrgVlanMemberSet(dev, vlanId, port, isMember, isTagged);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, vlanId, port, isMember, isTagged);

            /* Call function with vlanId [100], isMember [GT_TRUE], isTagged [GT_TRUE] */
            vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
            isMember = GT_TRUE;
            isTagged = GT_TRUE;
            
            st = cpssExMxPmBrgVlanMemberSet(dev, vlanId, port, isMember, isTagged);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, vlanId, port, isMember, isTagged);

            /* Call function with vlanId [4095], isMember [GT_FALSE], isTagged [GT_FALSE] */
            vlanId   = 4095;
            isMember = GT_FALSE;
            isTagged = GT_FALSE;
            
            st = cpssExMxPmBrgVlanMemberSet(dev, vlanId, port, isMember, isTagged);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, vlanId, port, isMember, isTagged);

            /* Call function with vlanId [4095], isMember [GT_TRUE], isTagged [GT_TRUE] */
            vlanId   = 4095;
            isMember = GT_TRUE;
            isTagged = GT_TRUE;
            
            st = cpssExMxPmBrgVlanMemberSet(dev, vlanId, port, isMember, isTagged);
            UTF_VERIFY_EQUAL5_PARAM_MAC(GT_OK, st, dev, vlanId, port, isMember, isTagged);

            /*
                1.1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                          and other parameters the same as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssExMxPmBrgVlanMemberSet(dev, vlanId, port, isMember, isTagged);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, vlanId = %d, port = %d",
                                         dev, vlanId, port);
        }

        vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
        isMember = GT_FALSE;
        isTagged = GT_FALSE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanMemberSet(dev, vlanId, port, isMember, isTagged);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanMemberSet(dev, vlanId, port, isMember, isTagged);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanMemberSet(dev, vlanId, port, isMember, isTagged);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
    isMember = GT_FALSE;
    isTagged = GT_FALSE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanMemberSet(dev, vlanId, port, isMember, isTagged);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanMemberSet(dev, vlanId, port, isMember, isTagged);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanCpuMemberSet
(
    IN GT_U8     devNum,
    IN GT_U16    vlanId,
    IN GT_BOOL   isMember
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanCpuMemberSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with vlanId [100 / 4095]
                            and isMember [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                 and isMember [GT_TRUE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16  vlanId   = 0;
    GT_BOOL isMember = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with vlanId [100 / 4095]
                                    and isMember [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with vlanId = 100, isMember [GT_FALSE] */
        vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
        isMember = GT_FALSE;

        st = cpssExMxPmBrgVlanCpuMemberSet(dev, vlanId, isMember);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, isMember);

        /* Call with vlanId = 100, isMember [GT_TRUE] */
        isMember = GT_TRUE;

        st = cpssExMxPmBrgVlanCpuMemberSet(dev, vlanId, isMember);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, isMember);

        /* Call with vlanId = 4095, isMember [GT_FALSE] */
        vlanId   = 4095;
        isMember = GT_FALSE;

        st = cpssExMxPmBrgVlanCpuMemberSet(dev, vlanId, isMember);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, isMember);

        /* Call with vlanId = 4095, isMember [GT_TRUE] */
        isMember = GT_TRUE;

        st = cpssExMxPmBrgVlanCpuMemberSet(dev, vlanId, isMember);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, isMember);

        /*
            1.2. Call Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                         and isMember [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmBrgVlanCpuMemberSet(dev, vlanId, isMember);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);
    }

    vlanId   = BRG_VLAN_TESTED_VLAN_ID_CNS;
    isMember = GT_TRUE;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanCpuMemberSet(dev, vlanId, isMember);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanCpuMemberSet(dev, vlanId, isMember);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortAcceptFrameTypeSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    IN  CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     frameType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortAcceptFrameTypeSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call function with frameType [CPSS_PORT_ACCEPT_FRAME_TAGGED_E /
                                         CPSS_PORT_ACCEPT_FRAME_ALL_E /
                                         CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgVlanPortAcceptFrameTypeGet with non-NULL frameTypePtr.
    Expected: GT_OK and the same frameType.
    1.1.3. Call function with frameType [CPSS_PORT_ACCEPT_FRAME_NONE_E] (invalid).
    Expected: GT_BAD_PARAM.
    1.1.4. Call function with out of range frameType [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    CPSS_PORT_ACCEPT_FRAME_TYPE_ENT frameType    = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;
    CPSS_PORT_ACCEPT_FRAME_TYPE_ENT frameTypeGet = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with frameType [CPSS_PORT_ACCEPT_FRAME_TAGGED_E /
                                                     CPSS_PORT_ACCEPT_FRAME_ALL_E /
                                                     CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E].
                Expected: GT_OK.
            */

            /* Call with frameType [CPSS_PORT_ACCEPT_FRAME_TAGGED_E] */
            frameType = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;

            st = cpssExMxPmBrgVlanPortAcceptFrameTypeSet(dev, port, frameType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, frameType);

            /* Call with frameType [CPSS_PORT_ACCEPT_FRAME_ALL_E] */
            frameType = CPSS_PORT_ACCEPT_FRAME_ALL_E;

            st = cpssExMxPmBrgVlanPortAcceptFrameTypeSet(dev, port, frameType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, frameType);

            /* Call with frameType [CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E] */
            frameType = CPSS_PORT_ACCEPT_FRAME_UNTAGGED_E;

            st = cpssExMxPmBrgVlanPortAcceptFrameTypeSet(dev, port, frameType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, frameType);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortAcceptFrameTypeGet with non-NULL frameTypePtr.
                Expected: GT_OK and the same frameType.
            */
            st = cpssExMxPmBrgVlanPortAcceptFrameTypeGet(dev, port, &frameTypeGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortAcceptFrameTypeGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(frameType, frameTypeGet,
                       "get another frameType than was set: %d, %d", dev, port);

            /*
                1.1.3. Call function with frameType [CPSS_PORT_ACCEPT_FRAME_NONE_E] (invalid).
                Expected: GT_BAD_PARAM.
            */
            frameType = CPSS_PORT_ACCEPT_FRAME_NONE_E;

            st = cpssExMxPmBrgVlanPortAcceptFrameTypeSet(dev, port, frameType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, frameType);

            /*
                1.1.4. Call function with out of range frameType [0x5AAAAAA5].
                Expected: GT_BAD_PARAM.
            */

            frameType = BRG_VLAN_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgVlanPortAcceptFrameTypeSet(dev, port, frameType);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, frameType);
        }

        frameType = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortAcceptFrameTypeSet(dev, port, frameType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortAcceptFrameTypeSet(dev, port, frameType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortAcceptFrameTypeSet(dev, port, frameType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    frameType = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortAcceptFrameTypeSet(dev, port, frameType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortAcceptFrameTypeSet(dev, port, frameType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortAcceptFrameTypeGet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    OUT CPSS_PORT_ACCEPT_FRAME_TYPE_ENT     *frameTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortAcceptFrameTypeGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call function with non-NULL frameTypePtr.
    Expected: GT_OK.
    1.1.2. Call function with frameTypePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    CPSS_PORT_ACCEPT_FRAME_TYPE_ENT frameType = CPSS_PORT_ACCEPT_FRAME_TAGGED_E;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with non-NULL frameTypePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgVlanPortAcceptFrameTypeGet(dev, port, &frameType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
            /*
                1.1.2. Call function with frameTypePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgVlanPortAcceptFrameTypeGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, frameTypePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortAcceptFrameTypeGet(dev, port, &frameType);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortAcceptFrameTypeGet(dev, port, &frameType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortAcceptFrameTypeGet(dev, port, &frameType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortAcceptFrameTypeGet(dev, port, &frameType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortAcceptFrameTypeGet(dev, port, &frameType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortPvidSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    IN  GT_U16  vlanId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortPvidSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with vlanId [100 / 4095].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgVlanPortPvidGet with non-NULL vidPtr.
    Expected: GT_OK and the same vlanId.
    1.1.3. Call function with out of range range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_U16 vlanId    = 0;
    GT_U16 vlanIdGet = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with vlanId [100 / 4095].
                Expected: GT_OK.
            */

            /* Call with vlanId [100] */
            vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

            st = cpssExMxPmBrgVlanPortPvidSet(dev, port, vlanId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vlanId);

            /* Call with vlanId [4095] */
            vlanId = 4095;

            st = cpssExMxPmBrgVlanPortPvidSet(dev, port, vlanId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, vlanId);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortPvidGet with non-NULL vidPtr.
                Expected: GT_OK and the same vlanId.
            */
            st = cpssExMxPmBrgVlanPortPvidGet(dev, port, &vlanIdGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortPvidGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(vlanId, vlanIdGet,
                       "get another vlanId than was set: %d, %d", dev, port);

            /*
                1.1.3. Call function with out of range range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS].
                Expected: GT_BAD_PARAM.
            */
            vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssExMxPmBrgVlanPortPvidSet(dev, port, vlanId);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, vlanId);
        }

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortPvidSet(dev, port, vlanId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortPvidSet(dev, port, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortPvidSet(dev, port, vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortPvidSet(dev, port, vlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortPvidSet(dev, port, vlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortPvidGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_U16  *vidPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortPvidGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with non-NULL vidPtr.
    Expected: GT_OK.
    1.1.2. Call function with vidPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_U16 vlanId = 0;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with non-NULL vidPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgVlanPortPvidGet(dev, port, &vlanId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function with vidPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgVlanPortPvidGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, vidPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortPvidGet(dev, port, &vlanId);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortPvidGet(dev, port, &vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortPvidGet(dev, port, &vlanId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortPvidGet(dev, port, &vlanId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortPvidGet(dev, port, &vlanId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortPvidPrecedenceSet
(
    IN GT_U8                                        devNum,
    IN GT_U8                                        port,
    IN CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  precedence
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortPvidPrecedenceSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with precedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                          CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgVlanPortPvidPrecedenceGet with non-NULL precedencePtr.
    Expected: GT_OK and the same precedence. 
    1.1.3. Call function with out of range precedence [0x5AAAAAA5].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT precedence    = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT precedenceGet = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with precedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                             CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E].
                Expected: GT_OK.
            */

            /* Call function with precedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E] */
            precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

            st = cpssExMxPmBrgVlanPortPvidPrecedenceSet(dev, port, precedence);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, precedence);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortPvidPrecedenceGet with non-NULL precedencePtr.
                Expected: GT_OK and the same precedence. 
            */
            st = cpssExMxPmBrgVlanPortPvidPrecedenceGet(dev, port, &precedenceGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortPvidPrecedenceGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(precedence, precedenceGet,
                       "get another precedence than was set: %d, %d", dev, port);

            /* Call function with precedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E] */
            precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

            st = cpssExMxPmBrgVlanPortPvidPrecedenceSet(dev, port, precedence);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, precedence);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortPvidPrecedenceGet with non-NULL precedencePtr.
                Expected: GT_OK and the same precedence. 
            */
            st = cpssExMxPmBrgVlanPortPvidPrecedenceGet(dev, port, &precedenceGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortPvidPrecedenceGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(precedence, precedenceGet,
                       "get another precedence than was set: %d, %d", dev, port);

            /*
                1.1.3. Call function with out of range precedence [0x5AAAAAA5].
                Expected: GT_BAD_PARAM.
            */
            precedence = BRG_VLAN_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgVlanPortPvidPrecedenceSet(dev, port, precedence);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, precedence);
        }

        precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortPvidPrecedenceSet(dev, port, precedence);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortPvidPrecedenceSet(dev, port, precedence);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortPvidPrecedenceSet(dev, port, precedence);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortPvidPrecedenceSet(dev, port, precedence);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortPvidPrecedenceSet(dev, port, precedence);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortPvidPrecedenceGet
(
    IN  GT_U8                                        devNum,
    IN  GT_U8                                        port,
    OUT CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  *precedencePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortPvidPrecedenceGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with non-NULL precedencePtr.
    Expected: GT_OK.
    1.1.2. Call function with precedencePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT precedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with non-NULL precedencePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgVlanPortPvidPrecedenceGet(dev, port, &precedence);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function with precedencePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgVlanPortPvidPrecedenceGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, precedencePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortPvidPrecedenceGet(dev, port, &precedence);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortPvidPrecedenceGet(dev, port, &precedence);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortPvidPrecedenceGet(dev, port, &precedence);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortPvidPrecedenceGet(dev, port, &precedence);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortPvidPrecedenceGet(dev, port, &precedence);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortForcePvidEnableSet
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortForcePvidEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgVlanPortForcePvidEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL state    = GT_FALSE;
    GT_BOOL stateGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with enable [GT_TRUE and GT_FALSE].
                Expected: GT_OK.
            */

            /* Call function with enable [GT_FALSE] */
            state = GT_FALSE;

            st = cpssExMxPmBrgVlanPortForcePvidEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortForcePvidEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgVlanPortForcePvidEnableGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortForcePvidEnableGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call function with enable [GT_TRUE] */
            state = GT_TRUE;

            st = cpssExMxPmBrgVlanPortForcePvidEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortForcePvidEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgVlanPortForcePvidEnableGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortForcePvidEnableGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        state = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortForcePvidEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortForcePvidEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortForcePvidEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortForcePvidEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortForcePvidEnableSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortForcePvidEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortForcePvidEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgVlanPortForcePvidEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgVlanPortForcePvidEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,"%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortForcePvidEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortForcePvidEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortForcePvidEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortForcePvidEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortForcePvidEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanProtocolEntrySet
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    IN GT_U16                           etherType,
    IN CPSS_PROT_CLASS_ENCAP_STC        *encListPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanProtocolEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with index [0 / 11],
                            etherType [0 / 1000]
                            and non-NULL encListPtr {ethV2 [GT_TRUE / GT_FALSE];
                                                     nonLlcSnap [GT_TRUE / GT_FALSE];
                                                     llcSnap [GT_TRUE / GT_FALSE]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgVlanProtocolEntryGet with non-NULL etherTypePtr,
                                                     non-NULL encListPtr and non-NULL validEntryPtr.
    Expected: GT_OK, validEntryPtr=GT_TRUE and the same etherType and encListPtr.
    1.3. Call function with out of range index [12]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call function with etherType [0xFFFF] (no constraints in the contract)
                            and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.5. Call function with encListPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.6. Call cpssExMxPmBrgVlanProtocolEntryInvalidate.
    Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    index        = 0;
    GT_U16                    etherType    = 0;
    GT_U16                    etherTypeGet = 0;
    CPSS_PROT_CLASS_ENCAP_STC encList;
    CPSS_PROT_CLASS_ENCAP_STC encListGet;
    GT_BOOL                   isValidEntry = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with index [0 / 11],
                                    etherType [0 / 1000]
                                    and non-NULL encListPtr {ethV2 [GT_TRUE / GT_FALSE];
                                                             nonLlcSnap [GT_TRUE / GT_FALSE];
                                                             llcSnap [GT_TRUE / GT_FALSE]}.
            Expected: GT_OK.
        */

        /* Call with index [0], etherType [0] */
        index  = 0;
        etherType = 1;

        encList.ethV2      = GT_FALSE;
        encList.nonLlcSnap = GT_FALSE;
        encList.llcSnap    = GT_FALSE;

        st = cpssExMxPmBrgVlanProtocolEntrySet(dev, index, etherType, &encList);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, etherType);

        /*
            1.2. Call cpssExMxPmBrgVlanProtocolEntryGet with non-NULL etherTypePtr,
                                                             non-NULL encListPtr and non-NULL validEntryPtr.
            Expected: GT_OK, validEntryPtr=GT_TRUE and the same etherType and encListPtr.
        */
        st = cpssExMxPmBrgVlanProtocolEntryGet(dev, index, &etherTypeGet,
                                               &encListGet, &isValidEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                     "cpssExMxPmBrgVlanProtocolEntryGet: %d, %d", dev, index);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValidEntry,
                                     "Entry is not valid: %d", dev);

        if (GT_TRUE == isValidEntry)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet,
                                         "get another etherType than was set: %d", dev);

            /* Verifying struct fields */
            UTF_VERIFY_EQUAL1_STRING_MAC(encList.ethV2, encListGet.ethV2,
                                         "get another encListPtr->ethV2 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(encList.nonLlcSnap, encListGet.nonLlcSnap,
                                         "get another encListPtr->nonLlcSnap than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(encList.llcSnap, encListGet.llcSnap,
                                         "get another encListPtr->llcSnap than was set: %d", dev);
        }

        /* Call with index [11], etherType [1000] */
        index  = 11;
        etherType = 1000;

        encList.ethV2      = GT_FALSE;
        encList.nonLlcSnap = GT_FALSE;
        encList.llcSnap    = GT_FALSE;

        st = cpssExMxPmBrgVlanProtocolEntrySet(dev, index, etherType, &encList);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, etherType);

        /*
            1.2. Call cpssExMxPmBrgVlanProtocolEntryGet with non-NULL etherTypePtr,
                                                             non-NULL encListPtr and non-NULL validEntryPtr.
            Expected: GT_OK, validEntryPtr=GT_TRUE and the same etherType and encListPtr.
        */
        st = cpssExMxPmBrgVlanProtocolEntryGet(dev, index, &etherTypeGet,
                                               &encListGet, &isValidEntry);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                                     "cpssExMxPmBrgVlanProtocolEntryGet: %d, %d", dev, index);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValidEntry,
                                     "Entry is not valid: %d", dev);

        if (GT_TRUE == isValidEntry)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet,
                                         "get another etherType than was set: %d", dev);

            /* Verifying struct fields */
            UTF_VERIFY_EQUAL1_STRING_MAC(encList.ethV2, encListGet.ethV2,
                                         "get another encListPtr->ethV2 than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(encList.nonLlcSnap, encListGet.nonLlcSnap,
                                         "get another encListPtr->nonLlcSnap than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(encList.llcSnap, encListGet.llcSnap,
                                         "get another encListPtr->llcSnap than was set: %d", dev);
        }

        /*
            1.3. Call function with out of range index [12]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        index = 12;

        st = cpssExMxPmBrgVlanProtocolEntrySet(dev, index, etherType, &encList);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, index);

        index = 0;

        /*
            1.4. Call function with etherType [0xFFFF] (no constraints in the contract)
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        etherType = 0xFFFF;

        st = cpssExMxPmBrgVlanProtocolEntrySet(dev, index, etherType, &encList);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, etherType);

        etherType = 0;

        /*
            1.5. Call function with encListPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanProtocolEntrySet(dev, index, etherType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, encListPtr = NULL", dev);

        /*
            1.6. Call cpssExMxPmBrgVlanProtocolEntryInvalidate.
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;

        st = cpssExMxPmBrgVlanProtocolEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmBrgVlanProtocolEntryInvalidate: %d, %d", dev, index);

        /* Call with index [11] */
        index = 11;

        st = cpssExMxPmBrgVlanProtocolEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                   "cpssExMxPmBrgVlanProtocolEntryInvalidate: %d, %d", dev, index);
    }

    index     = 0;
    etherType = 0;

    encList.ethV2      = GT_TRUE;
    encList.nonLlcSnap = GT_TRUE;
    encList.llcSnap    = GT_TRUE;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanProtocolEntrySet(dev, index, etherType, &encList);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanProtocolEntrySet(dev, index, etherType, &encList);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanProtocolEntryGet
(
    IN GT_U8                            devNum,
    IN GT_U32                           index,
    OUT GT_U16                          *etherTypePtr,
    OUT CPSS_PROT_CLASS_ENCAP_STC       *encListPtr,
    OUT GT_BOOL                         *validEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanProtocolEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with index [0 / 11],
                            non-NULL etherTypePtr,
                            non-NULL encListPtr
                            and non-NULL validEntryPtr.
    Expected: GT_OK.
    1.2. Call function with out of range index [12]
                            and other parameters the same as in 1.1.
    Expected: NON GT_OK.
    1.3. Call function with etherTypePtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call function with encListPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.5. Call function with validEntryPtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                    index      = 0;
    GT_U16                    etherType  = 0;
    CPSS_PROT_CLASS_ENCAP_STC encList;
    GT_BOOL                   validEntry = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with index [0 / 11],
                                    non-NULL etherTypePtr,
                                    non-NULL encListPtr
                                    and non-NULL validEntryPtr.
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;
        
        st = cpssExMxPmBrgVlanProtocolEntryGet(dev, index, &etherType,
                                               &encList, &validEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index [11] */
        index = 11;
        
        st = cpssExMxPmBrgVlanProtocolEntryGet(dev, index, &etherType,
                                               &encList, &validEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*  
            1.2. Call function with out of range index [12]
                                    and other parameters the same as in 1.1.
            Expected: NON GT_OK.
        */
        index = 12;

        st = cpssExMxPmBrgVlanProtocolEntryGet(dev, index, &etherType,
                                               &encList, &validEntry);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        index = 0;

        /*
            1.3. Call function with etherTypePtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanProtocolEntryGet(dev, index, NULL,
                                               &encList, &validEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, etherTypePtr = NULL", dev);

        /*
            1.4. Call function with encListPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanProtocolEntryGet(dev, index, &etherType,
                                               NULL, &validEntry);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, encListPtr = NULL", dev);

        /*
            1.5. Call function with validEntryPtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanProtocolEntryGet(dev, index, &etherType,
                                               &encList, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, validEntryPtr = NULL", dev);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanProtocolEntryGet(dev, index, &etherType,
                                               &encList, &validEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanProtocolEntryGet(dev, index, &etherType,
                                           &encList, &validEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanProtocolEntryInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           index
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanProtocolEntryInvalidate)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with index [0 / 11].
    Expected: GT_OK.
    1.2. Call function with out of range index [12].
    Expected: NON GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32  index = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with index [0 / 11].
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;

        st = cpssExMxPmBrgVlanProtocolEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index [11] */
        index = 11;

        st = cpssExMxPmBrgVlanProtocolEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*  
            1.2. Call function with out of range index [12].
            Expected: NON GT_OK.
        */
        index = 12;

        st = cpssExMxPmBrgVlanProtocolEntryInvalidate(dev, index);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanProtocolEntryInvalidate(dev, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanProtocolEntryInvalidate(dev, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortProtocolVidEnableSet
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortProtocolVidEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgVlanPortProtocolVidEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL state    = GT_FALSE;
    GT_BOOL stateGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with enable [GT_TRUE and GT_FALSE].
                Expected: GT_OK.
            */

            /* Call function with enable [GT_FALSE] */
            state = GT_FALSE;

            st = cpssExMxPmBrgVlanPortProtocolVidEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortProtocolVidEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgVlanPortProtocolVidEnableGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortProtocolVidEnableGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call function with enable [GT_TRUE] */
            state = GT_TRUE;

            st = cpssExMxPmBrgVlanPortProtocolVidEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortProtocolVidEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgVlanPortProtocolVidEnableGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortProtocolVidEnableGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        state = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortProtocolVidEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortProtocolVidEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortProtocolVidEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortProtocolVidEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortProtocolVidEnableSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortProtocolVidEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortProtocolVidEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgVlanPortProtocolVidEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgVlanPortProtocolVidEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,"%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortProtocolVidEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortProtocolVidEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortProtocolVidEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortProtocolVidEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortProtocolVidEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortProtocolQosEnableSet
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortProtocolQosEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgVlanPortProtocolQosEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL state    = GT_FALSE;
    GT_BOOL stateGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with enable [GT_TRUE and GT_FALSE].
                Expected: GT_OK.
            */

            /* Call function with enable [GT_FALSE] */
            state = GT_FALSE;

            st = cpssExMxPmBrgVlanPortProtocolQosEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortProtocolQosEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgVlanPortProtocolQosEnableGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortProtocolQosEnableGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call function with enable [GT_TRUE] */
            state = GT_TRUE;

            st = cpssExMxPmBrgVlanPortProtocolQosEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortProtocolQosEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgVlanPortProtocolQosEnableGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortProtocolQosEnableGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        state = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortProtocolQosEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortProtocolQosEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortProtocolQosEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortProtocolQosEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortProtocolQosEnableSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortProtocolQosEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortProtocolQosEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgVlanPortProtocolQosEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgVlanPortProtocolQosEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,"%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortProtocolQosEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortProtocolQosEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortProtocolQosEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortProtocolQosEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortProtocolQosEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortProtocolVidQosEntrySet
(
    IN GT_U8                                        devNum,
    IN GT_U8                                        port,
    IN GT_U32                                       index,
    IN CPSS_EXMXPM_BRG_VLAN_PROT_CLASS_CFG_STC      *vlanCfgPtr,
    IN CPSS_EXMXPM_QOS_ENTRY_STC                    *qosCfgPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortProtocolVidQosEntrySet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with index [0 / 11],
                              non-NULL vlanCfgPtr {vlanId [100 / 4095];
                                                   vlanIdAssignCmd [CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E /
                                                                    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E];
                                                   vlanIdAssignPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                                                           CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E}
                              and qosCfgPtr [NULL].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgVlanPortProtocolVidQosEntryGet with non-NULL vlanCfgPtr,
                                                                 non-NULL validEntryPtr
                                                                 and other parameters the same as in 1.1.1.
    Expected: GT_OK, validEntryPtr=GT_TRUE and the same vlanCfgPtr. 
    1.1.3. Call function with index [2 / 10],
                              vlanCfgPtr [NULL]
                              and non-NULL qosCfgPtr {qosParamsModify {modifyTc [GT_TRUE / GT_FALSE];
                                                                       modifyUp [GT_TRUE / GT_FALSE];
                                                                       modifyDp [GT_TRUE / GT_FALSE];
                                                                       modifyDscp [GT_TRUE / GT_FALSE];
                                                                       modifyExp [GT_TRUE / GT_FALSE]};
                                                      qosParams {tc [0 / 7];
                                                                 dp [CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E];
                                                                 up [0 / 7];
                                                                 dscp [0 / 63];
                                                                 exp [0 / 7]};
                                                      qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                                                     CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E]}.
    Expected: GT_OK.
    1.1.4. Call cpssExMxPmBrgVlanPortProtocolVidQosEntryGet with non-NULL qosCfgPtr,
                                                                 non-NULL validEntryPtr
                                                                 and other parameters the same as in 1.1.3.
    Expected: GT_OK, validEntryPtr=GT_TRUE and the same qosCfgPtr.
    1.1.5. Call function with index [3],
                              vlanCfgPtr the same as on 1.1.1.
                              and qosCfgPtr the same as in 1.1.3.
    Expected: GT_OK.
    1.1.6. Call cpssExMxPmBrgVlanPortProtocolVidQosEntryGet with non-NULL vlanCfgPtr,
                                                                 non-NULL qosCfgPtr,
                                                                 non-NULL validEntryPtr
                                                                 and index the same as in 1.1.5.
    Expected: Expected: GT_OK, validEntryPtr=GT_TRUE and the same vlanCfgPtr and qosCfgPtr.
    1.1.7. Call function with out of range index [12]
                              and other parameters the same as in 1.1.1.
    Expected: NON GT_OK.
    1.1.8. Call function with index [0],
                              out of range vlanCfgPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                              and other parameters the same as in 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.9. Call function with index [0],
                              out of range vlanCfgPtr->vlanIdAssignCmd [0x5AAAAAA5]
                              and other parameters the same as in 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.10. Call function with index [0],
                               out of range vlanCfgPtr->vlanIdAssignPrecedence [0x5AAAAAA5]
                               and other parameters the same as in 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.11. Call function with index [0],
                               qosCfgPtr->qosParamsModify.modifyTc [GT_TRUE],
                               out of range qosCfgPtr->qosParams.tc [CPSS_TC_RANGE_CNS=8]
                               and other parameters the same as in 1.1.3.
    Expected: GT_BAD_PARAM.
    1.1.12. Call function with index [0],
                               qosCfgPtr->qosParamsModify.modifyTc [GT_FALSE],
                               out of range qosCfgPtr->qosParams.tc [CPSS_TC_RANGE_CNS=8] (not relevant)
                               and other parameters the same as in 1.1.3.
    Expected: GT_OK.
    1.1.13. Call function with index [0],
                               qosCfgPtr->qosParamsModify.modifyDp [GT_TRUE],
                               out of range qosCfgPtr->qosParams.dp [0x5AAAAAA5]
                               and other parameters the same as in 1.1.3.
    Expected: GT_BAD_PARAM.
    1.1.14. Call function with index [0],
                               qosCfgPtr->qosParamsModify.modifyDp [GT_FALSE],
                               out of range qosCfgPtr->qosParams.dp [0x5AAAAAA5] (not relevant)
                               and other parameters the same as in 1.1.3.
    Expected: GT_OK.
    1.1.15. Call function with index [0],
                               qosCfgPtr->qosParamsModify.modifyUp [GT_TRUE],
                               out of range qosCfgPtr->qosParams.up [CPSS_USER_PRIORITY_RANGE_CNS=8]
                               and other parameters the same as in 1.1.3.
    Expected: NON GT_OK.
    1.1.16. Call function with index [0],
                               qosCfgPtr->qosParamsModify.modifyUp [GT_FALSE],
                               out of range qosCfgPtr->qosParams.up [CPSS_USER_PRIORITY_RANGE_CNS=8] (not relevant)
                               and other parameters the same as in 1.1.3.
    Expected: GT_OK.
    1.1.17. Call function with index [0],
                               qosCfgPtr->qosParamsModify.modifyDscp [GT_TRUE],
                               out of range qosCfgPtr->qosParams.dscp [CPSS_DSCP_RANGE_CNS=64]
                               and other parameters the same as in 1.1.3.
    Expected: NON GT_OK.
    1.1.18. Call function with index [0],
                               qosCfgPtr->qosParamsModify.modifyDscp [GT_FALSE],
                               out of range qosCfgPtr->qosParams.dscp [CPSS_DSCP_RANGE_CNS=64] (not relevant)
                               and other parameters the same as in 1.1.3.
    Expected: GT_OK.
    1.1.19. Call function with index [0],
                               qosCfgPtr->qosParamsModify.modifyExp [GT_TRUE],
                               out of range qosCfgPtr->qosParams.exp [CPSS_EXP_RANGE_CNS=8]
                               and other parameters the same as in 1.1.3.
    Expected: NON GT_OK.
    1.1.20. Call function with index [0],
                               qosCfgPtr->qosParamsModify.modifyExp [GT_FALSE],
                               out of range qosCfgPtr->qosParams.exp [CPSS_EXP_RANGE_CNS=8] (not relevant)
                               and other parameters the same as in 1.1.3.
    Expected: GT_OK.
    1.1.21. Call function with index [0],
                               out of range qosCfgPtr->qosPrecedence [0x5AAAAAA5]
                               and other parameters the same as in 1.1.3.
    Expected: GT_BAD_PARAM.
    1.1.22. Call function with index [0],
                               vlanCfgPtr [NULL]
                               and qosCfgPtr [NULL].
    Expected: NON GT_OK. 
    1.1.23. Call cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate with the same index.
    Expected: GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_U32                                  index   = 0;
    CPSS_EXMXPM_BRG_VLAN_PROT_CLASS_CFG_STC vlanCfg;
    CPSS_EXMXPM_QOS_ENTRY_STC               qosCfg;
    CPSS_EXMXPM_BRG_VLAN_PROT_CLASS_CFG_STC vlanCfgGet;
    CPSS_EXMXPM_QOS_ENTRY_STC               qosCfgGet;
    GT_BOOL                                 isValid = GT_FALSE;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with index [0 / 11],
                                          non-NULL vlanCfgPtr {vlanId [100 / 4095];
                                                               vlanIdAssignCmd [CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E /
                                                                                CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E];
                                                               vlanIdAssignPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                                                                       CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E}
                                          and qosCfgPtr [NULL].
                Expected: GT_OK.
            */

            /* Call with index [0] */
            index = 0;

            vlanCfg.vlanId                 = BRG_VLAN_TESTED_VLAN_ID_CNS;
            vlanCfg.vlanIdAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
            vlanCfg.vlanIdAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, NULL);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortProtocolVidQosEntryGet with non-NULL vlanCfgPtr,
                                                                             non-NULL validEntryPtr
                                                                             and other parameters the same as in 1.1.1.
                Expected: GT_OK, validEntryPtr=GT_TRUE and the same vlanCfgPtr. 
            */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfgGet, &qosCfgGet, &isValid);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgVlanPortProtocolVidQosEntryGet: %d, %d, %d", dev, port, index);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_TRUE, isValid, "Entry is NOT valid: %d, %d, %d", dev, port, index);

            if (GT_TRUE == isValid)
            {
                /* Verifying vlanCfgPtr */
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanId, vlanCfgGet.vlanId,
                           "get another vlanCfgPtr->vlanId than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanIdAssignCmd, vlanCfgGet.vlanIdAssignCmd,
                           "get another vlanCfgPtr->vlanIdAssignCmd than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanIdAssignPrecedence, vlanCfgGet.vlanIdAssignPrecedence,
                           "get another vlanCfgPtr->vlanIdAssignPrecedence than was set: %d, %d", dev, port);
            }

            /* Call with index [11] */
            index = 11;

            vlanCfg.vlanId                 = 4095;
            vlanCfg.vlanIdAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
            vlanCfg.vlanIdAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, NULL);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortProtocolVidQosEntryGet with non-NULL vlanCfgPtr,
                                                                             non-NULL validEntryPtr
                                                                             and other parameters the same as in 1.1.1.
                Expected: GT_OK, validEntryPtr=GT_TRUE and the same vlanCfgPtr. 
            */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfgGet, &qosCfgGet, &isValid);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgVlanPortProtocolVidQosEntryGet: %d, %d, %d", dev, port, index);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_TRUE, isValid, "Entry is NOT valid: %d, %d, %d", dev, port, index);

            if (GT_TRUE == isValid)
            {
                /* Verifying vlanCfgPtr */
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanId, vlanCfgGet.vlanId,
                           "get another vlanCfgPtr->vlanId than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanIdAssignCmd, vlanCfgGet.vlanIdAssignCmd,
                           "get another vlanCfgPtr->vlanIdAssignCmd than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanIdAssignPrecedence, vlanCfgGet.vlanIdAssignPrecedence,
                           "get another vlanCfgPtr->vlanIdAssignPrecedence than was set: %d, %d", dev, port);
            }

            /*
                1.1.3. Call function with index [2 / 10],
                                          vlanCfgPtr [NULL]
                                          and non-NULL qosCfgPtr {qosParamsModify {modifyTc [GT_TRUE / GT_FALSE];
                                                                                   modifyUp [GT_TRUE / GT_FALSE];
                                                                                   modifyDp [GT_TRUE / GT_FALSE];
                                                                                   modifyDscp [GT_TRUE / GT_FALSE];
                                                                                   modifyExp [GT_TRUE / GT_FALSE]};
                                                                  qosParams {tc [0 / 7];
                                                                             dp [CPSS_DP_GREEN_E / CPSS_DP_YELLOW_E];
                                                                             up [0 / 7];
                                                                             dscp [0 / 63];
                                                                             exp [0 / 7]};
                                                                  qosPrecedence [CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E /
                                                                                 CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E]}.
                Expected: GT_OK.
            */

            /* Call with index [2] */
            index = 2;

            qosCfg.qosParamsModify.modifyTc   = GT_FALSE;
            qosCfg.qosParamsModify.modifyUp   = GT_FALSE;
            qosCfg.qosParamsModify.modifyDp   = GT_FALSE;
            qosCfg.qosParamsModify.modifyDscp = GT_FALSE;
            qosCfg.qosParamsModify.modifyExp  = GT_FALSE;

            qosCfg.qosParams.tc   = 0;
            qosCfg.qosParams.dp   = CPSS_DP_GREEN_E;
            qosCfg.qosParams.up   = 0;
            qosCfg.qosParams.dscp = 0;
            qosCfg.qosParams.exp  = 0;

            qosCfg.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.4. Call cpssExMxPmBrgVlanPortProtocolVidQosEntryGet with non-NULL qosCfgPtr,
                                                                             non-NULL validEntryPtr
                                                                             and other parameters the same as in 1.1.3.
                Expected: GT_OK, validEntryPtr=GT_TRUE and the same qosCfgPtr.
            */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfgGet, &qosCfgGet, &isValid);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgVlanPortProtocolVidQosEntryGet: %d, %d, %d", dev, port, index);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_TRUE, isValid, "Entry is NOT valid: %d, %d, %d", dev, port, index);

            if (GT_TRUE == isValid)
            {
                /* Verifying qosCfgPtr */
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyTc, qosCfgGet.qosParamsModify.modifyTc,
                           "get another qosCfgPtr->qosParamsModify.modifyTc than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyUp, qosCfgGet.qosParamsModify.modifyUp,
                           "get another qosCfgPtr->qosParamsModify.modifyUp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyDp, qosCfgGet.qosParamsModify.modifyDp,
                           "get another qosCfgPtr->qosParamsModify.modifyDp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyDscp, qosCfgGet.qosParamsModify.modifyDscp,
                           "get another qosCfgPtr->qosParamsModify.modifyDscp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyExp, qosCfgGet.qosParamsModify.modifyExp,
                           "get another qosCfgPtr->qosParamsModify.modifyExp than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.tc, qosCfgGet.qosParams.tc,
                           "get another qosCfgPtr->qosParams.tc than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.up, qosCfgGet.qosParams.up,
                           "get another qosCfgPtr->qosParams.up than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.dp, qosCfgGet.qosParams.dp,
                           "get another qosCfgPtr->qosParams.dp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.dscp, qosCfgGet.qosParams.dscp,
                           "get another qosCfgPtr->qosParams.dscp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.exp, qosCfgGet.qosParams.exp,
                           "get another qosCfgPtr->qosParams.exp than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosPrecedence, qosCfgGet.qosPrecedence,
                           "get another qosCfgPtr->qosPrecedence than was set: %d, %d", dev, port);
            }

            /* Call with index [10] */
            index = 10;

            qosCfg.qosParamsModify.modifyTc   = GT_TRUE;
            qosCfg.qosParamsModify.modifyUp   = GT_TRUE;
            qosCfg.qosParamsModify.modifyDp   = GT_TRUE;
            qosCfg.qosParamsModify.modifyDscp = GT_TRUE;
            qosCfg.qosParamsModify.modifyExp  = GT_TRUE;

            qosCfg.qosParams.tc   = 7;
            qosCfg.qosParams.dp   = CPSS_DP_YELLOW_E;
            qosCfg.qosParams.up   = 7;
            qosCfg.qosParams.dscp = 63;
            qosCfg.qosParams.exp  = 7;

            qosCfg.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.4. Call cpssExMxPmBrgVlanPortProtocolVidQosEntryGet with non-NULL qosCfgPtr,
                                                                             non-NULL validEntryPtr
                                                                             and other parameters the same as in 1.1.3.
                Expected: GT_OK, validEntryPtr=GT_TRUE and the same qosCfgPtr.
            */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfgGet, &qosCfgGet, &isValid);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgVlanPortProtocolVidQosEntryGet: %d, %d, %d", dev, port, index);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_TRUE, isValid, "Entry is NOT valid: %d, %d, %d", dev, port, index);

            if (GT_TRUE == isValid)
            {
                /* Verifying qosCfgPtr */
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyTc, qosCfgGet.qosParamsModify.modifyTc,
                           "get another qosCfgPtr->qosParamsModify.modifyTc than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyUp, qosCfgGet.qosParamsModify.modifyUp,
                           "get another qosCfgPtr->qosParamsModify.modifyUp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyDp, qosCfgGet.qosParamsModify.modifyDp,
                           "get another qosCfgPtr->qosParamsModify.modifyDp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyDscp, qosCfgGet.qosParamsModify.modifyDscp,
                           "get another qosCfgPtr->qosParamsModify.modifyDscp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyExp, qosCfgGet.qosParamsModify.modifyExp,
                           "get another qosCfgPtr->qosParamsModify.modifyExp than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.tc, qosCfgGet.qosParams.tc,
                           "get another qosCfgPtr->qosParams.tc than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.up, qosCfgGet.qosParams.up,
                           "get another qosCfgPtr->qosParams.up than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.dp, qosCfgGet.qosParams.dp,
                           "get another qosCfgPtr->qosParams.dp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.dscp, qosCfgGet.qosParams.dscp,
                           "get another qosCfgPtr->qosParams.dscp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.exp, qosCfgGet.qosParams.exp,
                           "get another qosCfgPtr->qosParams.exp than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosPrecedence, qosCfgGet.qosPrecedence,
                           "get another qosCfgPtr->qosPrecedence than was set: %d, %d", dev, port);
            }

            /*
                1.1.5. Call function with index [3],
                                          vlanCfgPtr the same as on 1.1.1.
                                          and qosCfgPtr the same as in 1.1.3.
                Expected: GT_OK.
            */
            index = 3;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, &qosCfg);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.6. Call cpssExMxPmBrgVlanPortProtocolVidQosEntryGet with non-NULL vlanCfgPtr,
                                                                             non-NULL qosCfgPtr,
                                                                             non-NULL validEntryPtr
                                                                             and index the same as in 1.1.5.
                Expected: Expected: GT_OK, validEntryPtr=GT_TRUE and the same vlanCfgPtr and qosCfgPtr.
            */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfgGet, &qosCfgGet, &isValid);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgVlanPortProtocolVidQosEntryGet: %d, %d, %d", dev, port, index);

            UTF_VERIFY_EQUAL3_STRING_MAC(GT_TRUE, isValid, "Entry is NOT valid: %d, %d, %d", dev, port, index);

            if (GT_TRUE == isValid)
            {
                /* Verifying vlanCfgPtr */
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanId, vlanCfgGet.vlanId,
                           "get another vlanCfgPtr->vlanId than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanIdAssignCmd, vlanCfgGet.vlanIdAssignCmd,
                           "get another vlanCfgPtr->vlanIdAssignCmd than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanCfg.vlanIdAssignPrecedence, vlanCfgGet.vlanIdAssignPrecedence,
                           "get another vlanCfgPtr->vlanIdAssignPrecedence than was set: %d, %d", dev, port);

                /* Verifying qosCfgPtr */
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyTc, qosCfgGet.qosParamsModify.modifyTc,
                           "get another qosCfgPtr->qosParamsModify.modifyTc than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyUp, qosCfgGet.qosParamsModify.modifyUp,
                           "get another qosCfgPtr->qosParamsModify.modifyUp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyDp, qosCfgGet.qosParamsModify.modifyDp,
                           "get another qosCfgPtr->qosParamsModify.modifyDp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyDscp, qosCfgGet.qosParamsModify.modifyDscp,
                           "get another qosCfgPtr->qosParamsModify.modifyDscp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParamsModify.modifyExp, qosCfgGet.qosParamsModify.modifyExp,
                           "get another qosCfgPtr->qosParamsModify.modifyExp than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.tc, qosCfgGet.qosParams.tc,
                           "get another qosCfgPtr->qosParams.tc than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.up, qosCfgGet.qosParams.up,
                           "get another qosCfgPtr->qosParams.up than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.dp, qosCfgGet.qosParams.dp,
                           "get another qosCfgPtr->qosParams.dp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.dscp, qosCfgGet.qosParams.dscp,
                           "get another qosCfgPtr->qosParams.dscp than was set: %d, %d", dev, port);
                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosParams.exp, qosCfgGet.qosParams.exp,
                           "get another qosCfgPtr->qosParams.exp than was set: %d, %d", dev, port);

                UTF_VERIFY_EQUAL2_STRING_MAC(qosCfg.qosPrecedence, qosCfgGet.qosPrecedence,
                           "get another qosCfgPtr->qosPrecedence than was set: %d, %d", dev, port);
            }

            /*
                1.1.7. Call function with out of range index [12]
                                          and other parameters the same as in 1.1.1.
                Expected: NON GT_OK.
            */
            index = 12;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, NULL);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            index = 0;

            /*
                1.1.8. Call function with index [0],
                                          out of range vlanCfgPtr->vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                          and other parameters the same as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            vlanCfg.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, vlanCfgPtr->vlanId = %d", dev, port, vlanCfg.vlanId);

            vlanCfg.vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

            /*
                1.1.9. Call function with index [0],
                                          out of range vlanCfgPtr->vlanIdAssignCmd [0x5AAAAAA5]
                                          and other parameters the same as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            vlanCfg.vlanIdAssignCmd = BRG_VLAN_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, vlanCfgPtr->vlanIdAssignCmd = %d",
                                         dev, port, vlanCfg.vlanIdAssignCmd);

            vlanCfg.vlanIdAssignCmd = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;

            /*
                1.1.10. Call function with index [0],
                                           out of range vlanCfgPtr->vlanIdAssignPrecedence [0x5AAAAAA5]
                                           and other parameters the same as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            vlanCfg.vlanIdAssignPrecedence = BRG_VLAN_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, vlanCfgPtr->vlanIdAssignPrecedence = %d",
                                         dev, port, vlanCfg.vlanIdAssignPrecedence);

            vlanCfg.vlanIdAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

            /*
                1.1.11. Call function with index [0],
                                           qosCfgPtr->qosParamsModify.modifyTc [GT_TRUE],
                                           out of range qosCfgPtr->qosParams.tc [CPSS_TC_RANGE_CNS=8]
                                           and other parameters the same as in 1.1.3.
                Expected: GT_BAD_PARAM.
            */
            qosCfg.qosParamsModify.modifyTc = GT_TRUE;

            qosCfg.qosParams.tc = CPSS_TC_RANGE_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, qosCfg->qosParamsModify.modifyTc = %d, qosCfg->qosParams.tc = %d",
                                         dev, port, qosCfg.qosParamsModify.modifyTc, qosCfg.qosParams.tc);

            qosCfg.qosParams.tc = 0;

            /*
                1.1.12. Call function with index [0],
                                           qosCfgPtr->qosParamsModify.modifyTc [GT_FALSE],
                                           out of range qosCfgPtr->qosParams.tc [CPSS_TC_RANGE_CNS=8] (not relevant)
                                           and other parameters the same as in 1.1.3.
                Expected: GT_OK.
            */
            qosCfg.qosParamsModify.modifyTc = GT_FALSE;

            qosCfg.qosParams.tc = CPSS_TC_RANGE_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, qosCfg->qosParamsModify.modifyTc = %d, qosCfg->qosParams.tc = %d",
                                         dev, port, qosCfg.qosParamsModify.modifyTc, qosCfg.qosParams.tc);

            qosCfg.qosParams.tc = 0;

            /*
                1.1.13. Call function with index [0],
                                           qosCfgPtr->qosParamsModify.modifyDp [GT_TRUE],
                                           out of range qosCfgPtr->qosParams.dp [0x5AAAAAA5]
                                           and other parameters the same as in 1.1.3.
                Expected: GT_BAD_PARAM.
            */
            qosCfg.qosParamsModify.modifyDp = GT_TRUE;

            qosCfg.qosParams.dp = BRG_VLAN_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, qosCfg->qosParamsModify.modifyDp = %d, qosCfg->qosParams.dp = %d",
                                         dev, port, qosCfg.qosParamsModify.modifyDp, qosCfg.qosParams.dp);

            qosCfg.qosParams.dp = CPSS_DP_GREEN_E;

            /*
                1.1.14. Call function with index [0],
                                           qosCfgPtr->qosParamsModify.modifyDp [GT_FALSE],
                                           out of range qosCfgPtr->qosParams.dp [0x5AAAAAA5] (not relevant)
                                           and other parameters the same as in 1.1.3.
                Expected: GT_OK.
            */
            qosCfg.qosParamsModify.modifyDp = GT_FALSE;

            qosCfg.qosParams.dp = BRG_VLAN_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, qosCfg->qosParamsModify.modifyDp = %d, qosCfg->qosParams.dp = %d",
                                         dev, port, qosCfg.qosParamsModify.modifyDp, qosCfg.qosParams.dp);

            qosCfg.qosParams.dp = CPSS_DP_GREEN_E;

            /*
                1.1.15. Call function with index [0],
                                           qosCfgPtr->qosParamsModify.modifyUp [GT_TRUE],
                                           out of range qosCfgPtr->qosParams.up [CPSS_USER_PRIORITY_RANGE_CNS=8]
                                           and other parameters the same as in 1.1.3.
                Expected: NON GT_OK.
            */
            qosCfg.qosParamsModify.modifyUp = GT_TRUE;

            qosCfg.qosParams.up = CPSS_USER_PRIORITY_RANGE_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, qosCfg->qosParamsModify.modifyUp = %d, qosCfg->qosParams.up = %d",
                                             dev, port, qosCfg.qosParamsModify.modifyUp, qosCfg.qosParams.up);

            qosCfg.qosParams.up = 0;

            /*
                1.1.16. Call function with index [0],
                                           qosCfgPtr->qosParamsModify.modifyUp [GT_FALSE],
                                           out of range qosCfgPtr->qosParams.up [CPSS_USER_PRIORITY_RANGE_CNS=8] (not relevant)
                                           and other parameters the same as in 1.1.3.
                Expected: GT_OK.
            */
            qosCfg.qosParamsModify.modifyUp = GT_FALSE;

            qosCfg.qosParams.up = CPSS_USER_PRIORITY_RANGE_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, qosCfg->qosParamsModify.modifyUp = %d, qosCfg->qosParams.up = %d",
                                         dev, port, qosCfg.qosParamsModify.modifyUp, qosCfg.qosParams.up);

            qosCfg.qosParams.up = 0;

            /*
                1.1.17. Call function with index [0],
                                           qosCfgPtr->qosParamsModify.modifyDscp [GT_TRUE],
                                           out of range qosCfgPtr->qosParams.dscp [CPSS_DSCP_RANGE_CNS=64]
                                           and other parameters the same as in 1.1.3.
                Expected: NON GT_OK.
            */
            qosCfg.qosParamsModify.modifyDscp = GT_TRUE;

            qosCfg.qosParams.dscp = CPSS_DSCP_RANGE_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, qosCfg->qosParamsModify.modifyDscp = %d, qosCfg->qosParams.dscp = %d",
                                             dev, port, qosCfg.qosParamsModify.modifyDscp, qosCfg.qosParams.dscp);

            qosCfg.qosParams.dscp = 0;

            /*
                1.1.18. Call function with index [0],
                                           qosCfgPtr->qosParamsModify.modifyDscp [GT_FALSE],
                                           out of range qosCfgPtr->qosParams.dscp [CPSS_DSCP_RANGE_CNS=64] (not relevant)
                                           and other parameters the same as in 1.1.3.
                Expected: GT_OK.
            */
            qosCfg.qosParamsModify.modifyDscp = GT_FALSE;

            qosCfg.qosParams.dscp = CPSS_DSCP_RANGE_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, qosCfg->qosParamsModify.modifyDscp = %d, qosCfg->qosParams.dscp = %d",
                                         dev, port, qosCfg.qosParamsModify.modifyDscp, qosCfg.qosParams.dscp);

            qosCfg.qosParams.dscp = 0;

            /*
                1.1.19. Call function with index [0],
                                           qosCfgPtr->qosParamsModify.modifyExp [GT_TRUE],
                                           out of range qosCfgPtr->qosParams.exp [CPSS_EXP_RANGE_CNS=8]
                                           and other parameters the same as in 1.1.3.
                Expected: NON GT_OK.
            */
            qosCfg.qosParamsModify.modifyExp = GT_TRUE;

            qosCfg.qosParams.exp = CPSS_EXP_RANGE_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, qosCfg->qosParamsModify.modifyExp = %d, qosCfg->qosParams.exp = %d",
                                             dev, port, qosCfg.qosParamsModify.modifyExp, qosCfg.qosParams.exp);

            qosCfg.qosParams.exp = 0;

            /*
                1.1.20. Call function with index [0],
                                           qosCfgPtr->qosParamsModify.modifyExp [GT_FALSE],
                                           out of range qosCfgPtr->qosParams.exp [CPSS_EXP_RANGE_CNS=8] (not relevant)
                                           and other parameters the same as in 1.1.3.
                Expected: GT_OK.
            */
            qosCfg.qosParamsModify.modifyExp = GT_FALSE;

            qosCfg.qosParams.exp = CPSS_EXP_RANGE_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, qosCfg->qosParamsModify.modifyExp = %d, qosCfg->qosParams.exp = %d",
                                         dev, port, qosCfg.qosParamsModify.modifyExp, qosCfg.qosParams.exp);

            qosCfg.qosParams.exp = 0;

            /*
                1.1.21. Call function with index [0],
                                           out of range qosCfgPtr->qosPrecedence [0x5AAAAAA5]
                                           and other parameters the same as in 1.1.3.
                Expected: GT_BAD_PARAM.
            */
            qosCfg.qosPrecedence = BRG_VLAN_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, &qosCfg);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, qosCfg->qosPrecedence = %d",
                                         dev, port, qosCfg.qosPrecedence);

            qosCfg.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

            /*
                1.1.22. Call function with index [0],
                                           vlanCfgPtr [NULL]
                                           and qosCfgPtr [NULL].
                Expected: NON GT_OK.
            */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, NULL, NULL);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d, vlanCfgPtr = NULL, qosCfgPtr = NULL",
                                             dev, port);

            /*
                1.1.23. Call cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate with the same index.
                Expected: GT_OK.
            */

            /* Call with index = 0 */
            index = 0;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgVlanEntryInvalidate: %d, %d, %d", dev, port, index);

            /* Call with index = 11 */
            index = 11;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgVlanEntryInvalidate: %d, %d, %d", dev, port, index);

            /* Call with index = 2 */
            index = 2;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgVlanEntryInvalidate: %d, %d, %d", dev, port, index);

            /* Call with index = 10 */
            index = 10;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgVlanEntryInvalidate: %d, %d, %d", dev, port, index);

            /* Call with index = 3 */
            index = 3;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgVlanEntryInvalidate: %d, %d, %d", dev, port, index);
        }

        index = 0;

        vlanCfg.vlanId                 = BRG_VLAN_TESTED_VLAN_ID_CNS;
        vlanCfg.vlanIdAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
        vlanCfg.vlanIdAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        qosCfg.qosParamsModify.modifyTc   = GT_FALSE;
        qosCfg.qosParamsModify.modifyUp   = GT_FALSE;
        qosCfg.qosParamsModify.modifyDp   = GT_FALSE;
        qosCfg.qosParamsModify.modifyDscp = GT_FALSE;
        qosCfg.qosParamsModify.modifyExp  = GT_FALSE;

        qosCfg.qosParams.tc   = 0;
        qosCfg.qosParams.dp   = CPSS_DP_GREEN_E;
        qosCfg.qosParams.up   = 0;
        qosCfg.qosParams.dscp = 0;
        qosCfg.qosParams.exp  = 0;

        qosCfg.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, &qosCfg);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, &qosCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, &qosCfg);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    index = 0;

    vlanCfg.vlanId                 = BRG_VLAN_TESTED_VLAN_ID_CNS;
    vlanCfg.vlanIdAssignCmd        = CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
    vlanCfg.vlanIdAssignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    
    qosCfg.qosParamsModify.modifyTc   = GT_FALSE;
    qosCfg.qosParamsModify.modifyUp   = GT_FALSE;
    qosCfg.qosParamsModify.modifyDp   = GT_FALSE;
    qosCfg.qosParamsModify.modifyDscp = GT_FALSE;
    qosCfg.qosParamsModify.modifyExp  = GT_FALSE;
    
    qosCfg.qosParams.tc   = 0;
    qosCfg.qosParams.dp   = CPSS_DP_GREEN_E;
    qosCfg.qosParams.up   = 0;
    qosCfg.qosParams.dscp = 0;
    qosCfg.qosParams.exp  = 0;
    
    qosCfg.qosPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, &qosCfg);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortProtocolVidQosEntrySet(dev, port, index, &vlanCfg, &qosCfg);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortProtocolVidQosEntryGet
(
    IN GT_U8                                     devNum,
    IN GT_U8                                     port,
    IN GT_U32                                    index,
    OUT CPSS_EXMXPM_BRG_VLAN_PROT_CLASS_CFG_STC  *vlanCfgPtr,
    OUT CPSS_EXMXPM_QOS_ENTRY_STC                *qosCfgPtr,
    OUT GT_BOOL                                  *validEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortProtocolVidQosEntryGet)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with index [0],
                              non-NULL vlanCfgPtr,
                              qosCfgPtr [NULL]
                              and non-NULL validEntryPtr.
    Expected: GT_OK.
    1.1.2. Call function with index [5],
                              vlanCfgPtr [NULL],
                              non-NULL qosCfgPtr
                              and non-NULL validEntryPtr.
    Expected: GT_OK.
    1.1.3. Call function with index [11],
                              non-NULL vlanCfgPtr,
                              non-NULL qosCfgPtr
                              and non-NULL validEntryPtr.
    Expected: GT_OK.
    1.1.4. Call function with out of range index [12],
                              non-NULL vlanCfgPtr,
                              qosCfgPtr [NULL]
                              and non-NULL validEntryPtr.
    Expected: NON GT_OK.
    1.1.5. Call function with index [0],
                              non-NULL vlanCfgPtr,
                              non-NULL qosCfgPtr
                              and validEntryPtr [NULL].
    Expected: GT_BAD_PTR.
    1.1.6. Call function with index [0],
                              vlanCfgPtr [NULL],
                              non-NULL qosCfgPtr
                              and non-NULL validEntryPtr.
    Expected: GT_BAD_PTR.
    1.1.7. Call function with index [0],
                              non-NULL vlanCfgPtr,
                              qosCfgPtr [NULL]
                              and non-NULL validEntryPtr.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_U32                                  index      = 0;
    CPSS_EXMXPM_BRG_VLAN_PROT_CLASS_CFG_STC vlanCfg;
    CPSS_EXMXPM_QOS_ENTRY_STC               qosCfg;
    GT_BOOL                                 validEntry = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with index [0],
                                          non-NULL vlanCfgPtr,
                                          qosCfgPtr [NULL]
                                          and non-NULL validEntryPtr.
                Expected: GT_OK.
            */
            index = 0;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfg, &qosCfg, &validEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, %d, qosCfgPtr = NULL", dev, port, index);

            /*
                1.1.2. Call function with index [5],
                                          vlanCfgPtr [NULL],
                                          non-NULL qosCfgPtr
                                          and non-NULL validEntryPtr.
                Expected: GT_OK.
            */
            index = 5;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfg, &qosCfg, &validEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, %d, vlanCfgPtr = NULL", dev, port, index);

            /*
                1.1.3. Call function with index [11],
                                          non-NULL vlanCfgPtr,
                                          non-NULL qosCfgPtr
                                          and non-NULL validEntryPtr.
                Expected: GT_OK.
            */
            index = 11;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfg, &qosCfg, &validEntry);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, %d, vlanCfgPtr = NULL, qosCfgPtr = NULL",
                                         dev, port, index);

            /*
                1.1.4. Call function with out of range index [12],
                                          non-NULL vlanCfgPtr,
                                          qosCfgPtr [NULL]
                                          and non-NULL validEntryPtr.
                Expected: NON GT_OK.
            */
            index = 12;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfg, &qosCfg, &validEntry);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            index = 0;

            /*
                1.1.5. Call function with index [0],
                                          non-NULL vlanCfgPtr,
                                          non-NULL qosCfgPtr
                                          and validEntryPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfg, &qosCfg, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, validEntry = NULL", dev, port);

            /*
                1.1.6. Call function with index [0],
                                          vlanCfgPtr [NULL],
                                          non-NULL qosCfgPtr
                                          and non-NULL validEntryPtr.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             NULL, &qosCfg, &validEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, validEntry = NULL", dev, port);

            /*
                1.1.7. Call function with index [0],
                                          non-NULL vlanCfgPtr,
                                          qosCfgPtr [NULL]
                                          and non-NULL validEntryPtr.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfg, NULL, &validEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, validEntry = NULL", dev, port);
        }

        index = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                             &vlanCfg, &qosCfg, &validEntry);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                         &vlanCfg, &qosCfg, &validEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                         &vlanCfg, &qosCfg, &validEntry);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    index = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                         &vlanCfg, &qosCfg, &validEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortProtocolVidQosEntryGet(dev, port, index,
                                                     &vlanCfg, &qosCfg, &validEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_U32   index
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate)
{
/*
    ITERATE_DEVICES_VIRT_PORT (ExMxPm)
    1.1.1. Call function with index [0 / 11].
    Expected: GT_OK.
    1.1.2. Call function with out of range index [12].
    Expected: NON GT_OK.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_U32 index = 0;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with index [0 / 11].
                Expected: GT_OK.
            */

            /* Call with index [0] */
            index = 0;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /* Call with index [11] */
            index = 11;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.1.2. Call function with out of range index [12].
                Expected: NON GT_OK.
            */
            index = 12;

            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
            UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);
        }

        index = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_BAD_PARAM */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
    }

    index = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate(dev, port, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortIngressFilterEnableSet
(
    IN GT_U8   devNum,
    IN GT_U8   port,
    IN GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortIngressFilterEnableSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgVlanPortIngressFilterEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL state    = GT_FALSE;
    GT_BOOL stateGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with enable [GT_TRUE and GT_FALSE].
                Expected: GT_OK.
            */

            /* Call function with enable [GT_FALSE] */
            state = GT_FALSE;

            st = cpssExMxPmBrgVlanPortIngressFilterEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortIngressFilterEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgVlanPortIngressFilterEnableGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortIngressFilterEnableGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);

            /* Call function with enable [GT_TRUE] */
            state = GT_TRUE;

            st = cpssExMxPmBrgVlanPortIngressFilterEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, state);

            /*
                1.1.2. Call cpssExMxPmBrgVlanPortIngressFilterEnableGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgVlanPortIngressFilterEnableGet(dev, port, &stateGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortIngressFilterEnableGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(state, stateGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        state = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortIngressFilterEnableSet(dev, port, state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortIngressFilterEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortIngressFilterEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    state = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortIngressFilterEnableSet(dev, port, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortIngressFilterEnableSet(dev, port, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortIngressFilterEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortIngressFilterEnableGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL state = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call function with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgVlanPortIngressFilterEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call function with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgVlanPortIngressFilterEnableGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st,"%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortIngressFilterEnableGet(dev, port, &state);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortIngressFilterEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortIngressFilterEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortIngressFilterEnableGet(dev, port, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortIngressFilterEnableGet(dev, port, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanInvalidFilterEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanInvalidFilterEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgVlanInvalidFilterEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL state    = GT_FALSE;
    GT_BOOL stateGet = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1.1. Call with enable [GT_TRUE and GT_FALSE].
            Expected: GT_OK.
        */

        /* Call function with enable [GT_FALSE] */
        state = GT_FALSE;

        st = cpssExMxPmBrgVlanInvalidFilterEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.1.2. Call cpssExMxPmBrgVlanInvalidFilterEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgVlanInvalidFilterEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanInvalidFilterEnableGet: %d", dev);

        /* Verifying frameType */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                   "get another enable than was set: %d, %d", dev);

        /* Call function with enable [GT_TRUE] */
        state = GT_TRUE;

        st = cpssExMxPmBrgVlanInvalidFilterEnableSet(dev, state);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, state);

        /*
            1.1.2. Call cpssExMxPmBrgVlanInvalidFilterEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgVlanInvalidFilterEnableGet(dev, &stateGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanInvalidFilterEnableGet: %d", dev);

        /* Verifying frameType */
        UTF_VERIFY_EQUAL1_STRING_MAC(state, stateGet,
                   "get another enable than was set: %d, %d", dev);
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
        st = cpssExMxPmBrgVlanInvalidFilterEnableSet(dev, state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanInvalidFilterEnableSet(dev, state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanInvalidFilterEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanInvalidFilterEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call function with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL state = GT_FALSE;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with non-NULL enablePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmBrgVlanInvalidFilterEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call function with enablePtr [NULL].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanInvalidFilterEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgVlanInvalidFilterEnableGet(dev, &state);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanInvalidFilterEnableGet(dev, &state);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanIngressEtherTypeSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    IN  GT_U16                  etherType,
    IN  GT_U16                  maxVidRange
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanIngressEtherTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E /
                                           CPSS_VLAN_ETHERTYPE1_E],
                            etherType [0 / 0x8100]
                            and vidRange [50 / 100].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgVlanIngressEtherTypeGet with the same etherTypeMode,
                                                        non-NULL etherTypePtr
                                                        and non-NULL vidRangePtr.
    Expected: GT_OK and the same etherType and vidRange.
    1.3. Call function with out of range etherTypeMode [0x5AAAAAA5]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.4. Call function with etherType [0xFFFF]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.5. Call function with out of range vidRange [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_ETHER_MODE_ENT etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    GT_U16              etherType     = 0;
    GT_U16              vidRange      = 0;
    GT_U16              etherTypeGet  = 0;
    GT_U16              vidRangeGet   = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E /
                                                   CPSS_VLAN_ETHERTYPE1_E],
                                    etherType [0 / 0x8100]
                                    and vidRange [50 / 100].
            Expected: GT_OK.
        */

        /* Call with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E] */
        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
        etherType     = 0;
        vidRange      = 50;

        st = cpssExMxPmBrgVlanIngressEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType, vidRange);

        /*
            1.2. Call cpssExMxPmBrgVlanIngressEtherTypeGet with the same etherTypeMode,
                                                                non-NULL etherTypePtr
                                                                and non-NULL vidRangePtr.
            Expected: GT_OK and the same etherType and vidRange.
        */
        st = cpssExMxPmBrgVlanIngressEtherTypeGet(dev, etherTypeMode, &etherTypeGet, &vidRangeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanIngressEtherTypeGet: %d, %d", dev, etherTypeMode);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet,
                   "get another etherType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidRange, vidRangeGet,
                   "get another vidRange than was set: %d", dev);

        /* Call with etherTypeMode [CPSS_VLAN_ETHERTYPE1_E] */
        etherTypeMode = CPSS_VLAN_ETHERTYPE1_E;
        etherType     = 0x8100;
        vidRange      = BRG_VLAN_TESTED_VLAN_ID_CNS;

        st = cpssExMxPmBrgVlanIngressEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType, vidRange);

        /*
            1.2. Call cpssExMxPmBrgVlanIngressEtherTypeGet with the same etherTypeMode,
                                                                non-NULL etherTypePtr
                                                                and non-NULL vidRangePtr.
            Expected: GT_OK and the same etherType and vidRange.
        */
        st = cpssExMxPmBrgVlanIngressEtherTypeGet(dev, etherTypeMode, &etherTypeGet, &vidRangeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanIngressEtherTypeGet: %d, %d", dev, etherTypeMode);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet,
                   "get another etherType than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(vidRange, vidRangeGet,
                   "get another vidRange than was set: %d", dev);

        /*
            1.3. Call function with out of range etherTypeMode [0x5AAAAAA5]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        etherTypeMode = BRG_VLAN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgVlanIngressEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, etherTypeMode);

        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;

        /*
            1.4. Call function with etherType [0xFFFF]
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        etherType = 0xFFFF;

        st = cpssExMxPmBrgVlanIngressEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType, vidRange);

        etherType = 0;

        /*
            1.5. Call function with out of range vidRange [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        vidRange = PRV_CPSS_MAX_NUM_VLANS_CNS;

        st = cpssExMxPmBrgVlanIngressEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, vidRange = %d", dev, vidRange);
    }

    etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    etherType     = 0;
    vidRange      = BRG_VLAN_TESTED_VLAN_ID_CNS;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanIngressEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanIngressEtherTypeSet(dev, etherTypeMode, etherType, vidRange);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanIngressEtherTypeGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    OUT GT_U16                  *etherTypePtr,
    OUT GT_U16                  *maxVidRangePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanIngressEtherTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E /
                                           CPSS_VLAN_ETHERTYPE1_E],
                            non-NULL etherTypePtr
                            and non-NULL vidRangePtr.
    Expected: GT_OK.
    1.2. Call function with out of range etherTypeMode [0x5AAAAAA5]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call function with etherTypePtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
    1.4. Call function with vidRangePtr [NULL]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_ETHER_MODE_ENT etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    GT_U16              etherType     = 0;
    GT_U16              vidRange      = 0;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E /
                                                   CPSS_VLAN_ETHERTYPE1_E],
                                    non-NULL etherTypePtr
                                    and non-NULL vidRangePtr.
            Expected: GT_OK.
        */

        /* Call with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E] */
        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
        
        st = cpssExMxPmBrgVlanIngressEtherTypeGet(dev, etherTypeMode, &etherType, &vidRange);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, etherTypeMode);

        /* Call with etherTypeMode [CPSS_VLAN_ETHERTYPE1_E] */
        etherTypeMode = CPSS_VLAN_ETHERTYPE1_E;
        
        st = cpssExMxPmBrgVlanIngressEtherTypeGet(dev, etherTypeMode, &etherType, &vidRange);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, etherTypeMode);

        /*
            1.2. Call function with out of range etherTypeMode [0x5AAAAAA5]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        etherTypeMode = BRG_VLAN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgVlanIngressEtherTypeGet(dev, etherTypeMode, &etherType, &vidRange);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, etherTypeMode);

        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;

        /*
            1.3. Call function with etherTypePtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanIngressEtherTypeGet(dev, etherTypeMode, NULL, &vidRange);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, etherTypePtr = NULL", dev);

        /*
            1.4. Call function with vidRangePtr [NULL]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanIngressEtherTypeGet(dev, etherTypeMode, &etherType, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, vidRangePtr = NULL", dev);
    }

    etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanIngressEtherTypeGet(dev, etherTypeMode, &etherType, &vidRange);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanIngressEtherTypeGet(dev, etherTypeMode, &etherType, &vidRange);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanEgressEtherTypeSet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    IN  GT_U16                  etherType
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanEgressEtherTypeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E / CPSS_VLAN_ETHERTYPE1_E]
                            and etherType [0 / 0x8100].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgVlanEgressEtherTypeGet with the same etherTypeMode and non-NULL etherTypePtr.
    Expected: GT_OK and the same etherType. 
    1.3. Call function with etherType [0xFFFF]
                            and etherTypeMode [CPSS_VLAN_ETHERTYPE0_E].
    Expected: GT_OK. 
    1.4. Call function with out of range etherTypeMode [0x5AAAAAA5]
                            and and etherType [0xAAAA].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_ETHER_MODE_ENT etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    GT_U16              etherType     = 0;
    GT_U16              etherTypeGet  = 0;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E / CPSS_VLAN_ETHERTYPE1_E]
                                    and etherType [0 / 0x8100].
            Expected: GT_OK.
        */

        /* Call with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E] */
        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
        etherType     = 0;
        
        st = cpssExMxPmBrgVlanEgressEtherTypeSet(dev, etherTypeMode, etherType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType);

        /*
            1.2. Call cpssExMxPmBrgVlanEgressEtherTypeGet with the same etherTypeMode and non-NULL etherTypePtr.
            Expected: GT_OK and the same etherType. 
        */
        st = cpssExMxPmBrgVlanEgressEtherTypeGet(dev, etherTypeMode, &etherTypeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanEgressEtherTypeGet: %d, %d", dev, etherTypeMode);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet,
                   "get another etherType than was set: %d", dev);
        
        /* Call with etherTypeMode [CPSS_VLAN_ETHERTYPE1_E] */
        etherTypeMode = CPSS_VLAN_ETHERTYPE1_E;
        etherType     = 0;
        
        st = cpssExMxPmBrgVlanEgressEtherTypeSet(dev, etherTypeMode, etherType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType);

        /*
            1.2. Call cpssExMxPmBrgVlanEgressEtherTypeGet with the same etherTypeMode and non-NULL etherTypePtr.
            Expected: GT_OK and the same etherType. 
        */
        st = cpssExMxPmBrgVlanEgressEtherTypeGet(dev, etherTypeMode, &etherTypeGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanEgressEtherTypeGet: %d, %d", dev, etherTypeMode);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet,
                   "get another etherType than was set: %d", dev);

        /*
            1.3. Call function with etherType [0xFFFF]
                                    and etherTypeMode [CPSS_VLAN_ETHERTYPE0_E].
            Expected: GT_OK. 
        */
        etherType = 0xFFFF;

        st = cpssExMxPmBrgVlanEgressEtherTypeSet(dev, etherTypeMode, etherType);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, etherTypeMode, etherType);

        etherType = 0;

        /*
            1.4. Call function with out of range etherTypeMode [0x5AAAAAA5]
                                    and and etherType [0xAAAA].
            Expected: GT_BAD_PARAM.
        */
        etherTypeMode = BRG_VLAN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgVlanEgressEtherTypeSet(dev, etherTypeMode, etherType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, etherTypeMode);
    }

    etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    etherType     = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanEgressEtherTypeSet(dev, etherTypeMode, etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanEgressEtherTypeSet(dev, etherTypeMode, etherType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanEgressEtherTypeGet
(
    IN  GT_U8                   devNum,
    IN  CPSS_ETHER_MODE_ENT     etherTypeMode,
    OUT GT_U16                  *etherTypePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanEgressEtherTypeGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E / CPSS_VLAN_ETHERTYPE1_E]
                            and non-NULL etherTypePtr.
    Expected: GT_OK.
    1.2. Call function with out of range etherTypeMode [0x5AAAAAA5]
                            and non-NULL etherTypePtr.
    Expected: GT_BAD_PARAM.
    1.3. Call function with etherTypePtr [NULL]
                            and etherTypeMode [CPSS_VLAN_ETHERTYPE0_E].
    Expected: GT_BAD_PTR.  
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    CPSS_ETHER_MODE_ENT etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    GT_U16              etherType     = 0;
    
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E / CPSS_VLAN_ETHERTYPE1_E]
                                    and non-NULL etherTypePtr.
            Expected: GT_OK.
        */

        /* Call with etherTypeMode [CPSS_VLAN_ETHERTYPE0_E] */
        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
        
        st = cpssExMxPmBrgVlanEgressEtherTypeGet(dev, etherTypeMode, &etherType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, etherTypeMode);

        /* Call with etherTypeMode [CPSS_VLAN_ETHERTYPE1_E] */
        etherTypeMode = CPSS_VLAN_ETHERTYPE1_E;
        
        st = cpssExMxPmBrgVlanEgressEtherTypeGet(dev, etherTypeMode, &etherType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, etherTypeMode);

        /*
            1.2. Call function with out of range etherTypeMode [0x5AAAAAA5]
                                    and non-NULL etherTypePtr.
            Expected: GT_BAD_PARAM.
        */
        etherTypeMode = BRG_VLAN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgVlanEgressEtherTypeGet(dev, etherTypeMode, &etherType);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, etherTypeMode);

        etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;

        /*
            1.3. Call function with etherTypePtr [NULL]
                                    and etherTypeMode [CPSS_VLAN_ETHERTYPE0_E].
            Expected: GT_BAD_PTR.  
        */
        st = cpssExMxPmBrgVlanEgressEtherTypeGet(dev, etherTypeMode, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, etherTypePtr = NULL", dev);
    }

    etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanEgressEtherTypeGet(dev, etherTypeMode, &etherType);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanEgressEtherTypeGet(dev, etherTypeMode, &etherType);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanStgIdBind
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   stgId
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanStgIdBind)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with vlanId [100 / 4095]
                            and stgId [0 / 100].
    Expected: GT_OK.
    1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and stgId [0].
    Expected: GT_BAD_PARAM.
    1.3. Call function with out of range stgId [256]
                            and vlanId [100].
    Expected: NON GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16  vlanId = 0;
    GT_U16  stgId  = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with vlanId [100 / 4095]
                                    and stpId [0 / 100].
            Expected: GT_OK.
        */

        /* Call with vlanId [100], stgId [0] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        stgId  = 0;

        st = cpssExMxPmBrgVlanStgIdBind(dev, vlanId, stgId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, stgId);

        /* Call with vlanId [4095], stgId [100] */
        vlanId = 4095;
        stgId  = 0;

        st = cpssExMxPmBrgVlanStgIdBind(dev, vlanId, stgId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, stgId);

        /*
            1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                    and stgId [0].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        stgId  = 0;

        st = cpssExMxPmBrgVlanStgIdBind(dev, vlanId, stgId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.3. Call function with out of range stgId [256]
                                    and vlanId [100].
            Expected: NON GT_OK.
        */
        stgId  = 256;

        st = cpssExMxPmBrgVlanStgIdBind(dev, vlanId, stgId);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, stgId = %d", dev, stgId);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    stgId  = 0;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanStgIdBind(dev, vlanId, stgId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanStgIdBind(dev, vlanId, stgId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanIpmBridgingEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  GT_BOOL                     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanIpmBridgingEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with vlanId [100 / 4095],
                            ipVer [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E]
                            and enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
    1.3. Call function with ipVer [CPSS_IP_PROTOCOL_IPV4V6_E]
                            and other parameters the same as in 1.1.
    Expected: GT_OK.
    1.4. Call function with out of range ipVer [0x5AAAAAA5]
                            and other parameters the same as in 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16                     vlanId = 0;
    CPSS_IP_PROTOCOL_STACK_ENT ipVer  = CPSS_IP_PROTOCOL_IPV4_E;
    GT_BOOL                    enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with vlanId [100 / 4095],
                                    ipVer [CPSS_IP_PROTOCOL_IPV4_E / CPSS_IP_PROTOCOL_IPV6_E]
                                    and enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with vlanId [100], ipVer [CPSS_IP_PROTOCOL_IPV4_E], enable [GT_FALSE] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        ipVer  = CPSS_IP_PROTOCOL_IPV4_E;
        enable = GT_FALSE;

        st = cpssExMxPmBrgVlanIpmBridgingEnableSet(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, enable);

        /* Call with vlanId [4095], ipVer [CPSS_IP_PROTOCOL_IPV6_E], enable [GT_TRUE] */
        vlanId = 4095;
        ipVer  = CPSS_IP_PROTOCOL_IPV6_E;
        enable = GT_TRUE;

        st = cpssExMxPmBrgVlanIpmBridgingEnableSet(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, enable);

        /*
            1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        
        st = cpssExMxPmBrgVlanIpmBridgingEnableSet(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.3. Call function with ipVer [CPSS_IP_PROTOCOL_IPV4V6_E] (invalid)
                                    and other parameters the same as in 1.1.
            Expected: GT_OK.
        */
        ipVer = CPSS_IP_PROTOCOL_IPV4V6_E;
        
        st = cpssExMxPmBrgVlanIpmBridgingEnableSet(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipVer = %d", dev, ipVer);

        ipVer = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.4. Call function with out of range ipVer [0x5AAAAAA5]
                                    and other parameters the same as in 1.1.
            Expected: GT_BAD_PARAM.
        */
        ipVer = BRG_VLAN_INVALID_ENUM_CNS;
        
        st = cpssExMxPmBrgVlanIpmBridgingEnableSet(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ipVer = %d", dev, ipVer);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    ipVer  = CPSS_IP_PROTOCOL_IPV4_E;
    enable = GT_TRUE;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanIpmBridgingEnableSet(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanIpmBridgingEnableSet(dev, vlanId, ipVer, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanIpmBridgingModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      vlanId,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipVer,
    IN  CPSS_BRG_IPM_MODE_ENT       ipmMode
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanIpmBridgingModeSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call cpssExMxPmBrgVlanIpmBridgingEnableSet with vlanId [100 / 4095],
                                                         ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                                                CPSS_IP_PROTOCOL_IPV6_E]
                                                         and enable [GT_TRUE].
    Expected: GT_OK.
    1.2. Call function with vlanId [100 / 4095],
                            ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                   CPSS_IP_PROTOCOL_IPV6_E] (according to the cpssExMxPmBrgVlanIpmBridgingEnableSet parameters)
                            and ipmMode [CPSS_BRG_IPM_SGV_E / CPSS_BRG_IPM_GV_E].
    Expected: GT_OK.
    1.3. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and other parameters the same as in 1.2.
    Expected: GT_BAD_PARAM.
    1.4. Call function with ipVer [CPSS_IP_PROTOCOL_IPV4V6_E]
                            and other parameters the same as in 1.2.
    Expected: GT_OK.
    1.5. Call function with out of range ipVer [0x5AAAAAA5]
                            and other parameters the same as in 1.2.
    Expected: GT_BAD_PARAM.
    1.6. Call function with out of range ipmMode [0x5AAAAAA5]
                            and other parameters the same as in 1.2.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16                     vlanId  = 0;
    CPSS_IP_PROTOCOL_STACK_ENT ipVer   = CPSS_IP_PROTOCOL_IPV4_E;
    CPSS_BRG_IPM_MODE_ENT      ipmMode = CPSS_BRG_IPM_SGV_E;
    GT_BOOL                    enable  = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call cpssExMxPmBrgVlanIpmBridgingEnableSet with vlanId [100 / 4095],
                                                                 ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                                                        CPSS_IP_PROTOCOL_IPV6_E]
                                                                 and enable [GT_TRUE].
            Expected: GT_OK.
        */
        enable = GT_TRUE;

        /* Call with vlanId [100]*/
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        ipVer  = CPSS_IP_PROTOCOL_IPV4_E;

        st = cpssExMxPmBrgVlanIpmBridgingEnableSet(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanIpmBridgingEnableSet: %d, %d, %d, %d",
                                     dev, vlanId, ipmMode, enable);

        /*
            1.2. Call function with vlanId [100 / 4095],
                                    ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                           CPSS_IP_PROTOCOL_IPV6_E] (according to the cpssExMxPmBrgVlanIpmBridgingEnableSet parameters)
                                    and ipmMode [CPSS_BRG_IPM_SGV_E / CPSS_BRG_IPM_GV_E].
            Expected: GT_OK.
        */
        ipmMode = CPSS_BRG_IPM_SGV_E;

        st = cpssExMxPmBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, ipmMode);

        /* Call with vlanId [4095]*/
        vlanId = 4095;
        ipVer  = CPSS_IP_PROTOCOL_IPV6_E;

        st = cpssExMxPmBrgVlanIpmBridgingEnableSet(dev, vlanId, ipVer, enable);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanIpmBridgingEnableSet: %d, %d, %d, %d",
                                     dev, vlanId, ipmMode, enable);

        /*
            1.2. Call function with vlanId [100 / 4095],
                                    ipVer [CPSS_IP_PROTOCOL_IPV4_E /
                                           CPSS_IP_PROTOCOL_IPV6_E] (according to the cpssExMxPmBrgVlanIpmBridgingEnableSet parameters)
                                    and ipmMode [CPSS_BRG_IPM_SGV_E / CPSS_BRG_IPM_GV_E].
            Expected: GT_OK.
        */
        ipmMode = CPSS_BRG_IPM_GV_E;

        st = cpssExMxPmBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, vlanId, ipVer, ipmMode);

        /*
            1.3. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                    and other parameters the same as in 1.2.
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        
        st = cpssExMxPmBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;

        /*
            1.4. Call function with ipVer [CPSS_IP_PROTOCOL_IPV4V6_E]
                                    and other parameters the same as in 1.2.
            Expected: GT_OK.
        */
        ipVer = CPSS_IP_PROTOCOL_IPV4V6_E;
        
        st = cpssExMxPmBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, ipVer = %d", dev, ipVer);

        ipVer = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.5. Call function with out of range ipVer [0x5AAAAAA5]
                                    and other parameters the same as in 1.2.
            Expected: GT_BAD_PARAM.
        */
        ipVer = BRG_VLAN_INVALID_ENUM_CNS;
        
        st = cpssExMxPmBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ipVer = %d", dev, ipVer);

        ipVer = CPSS_IP_PROTOCOL_IPV4_E;

        /*
            1.6. Call function with out of range ipmMode [0x5AAAAAA5]
                                    and other parameters the same as in 1.2.
            Expected: GT_BAD_PARAM.
        */
        ipmMode = BRG_VLAN_INVALID_ENUM_CNS;
        
        st = cpssExMxPmBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, ipmMode = %d", dev, ipmMode);
    }

    vlanId  = BRG_VLAN_TESTED_VLAN_ID_CNS;
    ipVer   = CPSS_IP_PROTOCOL_IPV4_E;
    ipmMode = CPSS_BRG_IPM_SGV_E;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanIpmBridgingModeSet(dev, vlanId, ipVer, ipmMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    IN GT_U32    index
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1. Call function with index [0 / 7].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet with non-NULL indexPtr.
    Expected: GT_OK and the same index.
    1.3. Call function with out of range index [8].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_U32 index    = 0;
    GT_U32 indexGet = 0;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1. Call function with index [0 / 7].
                Expected: GT_OK.
            */

            /* Call with index [0] */
            index = 0;

            st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet(dev, port, index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.2. Call cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet with non-NULL indexPtr.
                Expected: GT_OK and the same index.
            */
            st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet(dev, port, &indexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(index, indexGet,
                       "get another index than was set: %d, %d", dev, port);

            /* Call with index [7] */
            index = 7;

            st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet(dev, port, index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.2. Call cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet with non-NULL indexPtr.
                Expected: GT_OK and the same index.
            */
            st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet(dev, port, &indexGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                       "cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet: %d, %d", dev, port);

            /* Verifying frameType */
            UTF_VERIFY_EQUAL2_STRING_MAC(index, indexGet,
                       "get another index than was set: %d, %d", dev, port);

            /*
                1.3. Call function with out of range index [8].
                Expected: GT_BAD_PARAM.
            */
            index = 8;

            st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet(dev, port, index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_BAD_PARAM, st, dev, port, index);
        }

        index = 0;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet(dev, port, index);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet(dev, port, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet(dev, port, index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    index = 0;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet(dev, port, index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet(dev, port, index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet
(
    IN  GT_U8     devNum,
    IN  GT_U8      portNum,
    OUT GT_U32    *indexPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1. Call function with non-NULL indexPtr.
    Expected: GT_OK.
    1.2. Call function with indexPtr [NULL].
    Expected: GT_BAD_PTR. 
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev;
    GT_U8       port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    GT_U32 index = 0;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1. Call function with non-NULL indexPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet(dev, port, &index);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, index);

            /*
                1.2. Call function with indexPtr [NULL].
                Expected: GT_BAD_PTR. 
            */
            st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, indexPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while(GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet(dev, port, &index);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */

        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet(dev, port, &index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet(dev, port, &index);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet(dev, port, &index);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet(dev, port, &index);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanUntaggedMruProfileValueSet
(
    IN GT_U8     devNum,
    IN GT_U32    index,
    IN GT_U32    mru
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanUntaggedMruProfileValueSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with index [0 / 7]
                            and mru [100 / 0x2840].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgVlanUntaggedMruProfileValueGet with non-NULL mruPtr
                                                          and index the same as in 1.1.
    Expected: GT_OK and the same mru.
    1.3. Call function with out of range index [8]
                            and mru [100].
    Expected: GT_BAD_PARAM.
    1.4. Call function with out of range mru [0xFFFF]
                            and index [0].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32 index  = 0;
    GT_U32 mru    = 0;
    GT_U32 mruGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with index [0 / 7]
                                    and mru [100 / 0x2840].
            Expected: GT_OK.
        */

        /* Call with index [0], mru [100] */
        index = 0;
        mru   = 100;

        st = cpssExMxPmBrgVlanUntaggedMruProfileValueSet(dev, index, mru);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, mru);

        /*
            1.2. Call cpssExMxPmBrgVlanUntaggedMruProfileValueGet with non-NULL mruPtr
                                                                  and index the same as in 1.1.
            Expected: GT_OK and the same mru.
        */
        st = cpssExMxPmBrgVlanUntaggedMruProfileValueGet(dev, index, &mruGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanUntaggedMruProfileValueGet: %d, %d", dev, index);

        /* Verifying mru */
        UTF_VERIFY_EQUAL1_STRING_MAC(mru, mruGet,
                   "get another mru than was set: %d", dev);

        /* Call with index [7], mru [0x2840] */
        index = 7;
        mru   = 0x2840;

        st = cpssExMxPmBrgVlanUntaggedMruProfileValueSet(dev, index, mru);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, index, mru);

        /*
            1.2. Call cpssExMxPmBrgVlanUntaggedMruProfileValueGet with non-NULL mruPtr
                                                                  and index the same as in 1.1.
            Expected: GT_OK and the same mru.
        */
        st = cpssExMxPmBrgVlanUntaggedMruProfileValueGet(dev, index, &mruGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanUntaggedMruProfileValueGet: %d, %d", dev, index);

        /* Verifying mru */
        UTF_VERIFY_EQUAL1_STRING_MAC(mru, mruGet,
                   "get another mru than was set: %d", dev);

        /*  
            1.3. Call function with out of range index [8]
                                    and mru [100].
            Expected: GT_BAD_PARAM.
        */
        index = 8;
        mru   = 100;

        st = cpssExMxPmBrgVlanUntaggedMruProfileValueSet(dev, index, mru);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, index);

        index = 0;

        /*
            1.4. Call function with out of range mru [0xFFFF]
                                    and index [0].
            Expected: GT_BAD_PARAM.
        */
        mru = 0xFFFF;

        st = cpssExMxPmBrgVlanUntaggedMruProfileValueSet(dev, index, mru);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, mru = %d", dev, mru);
    }

    index = 0;
    mru   = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanUntaggedMruProfileValueSet(dev, index, mru);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanUntaggedMruProfileValueSet(dev, index, mru);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanUntaggedMruProfileValueGet
(
    IN  GT_U8     devNum,
    IN  GT_U32    index,
    OUT GT_U32    *mruPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanUntaggedMruProfileValueGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with index [0 / 7]
                            and non-NULL mruPtr.
    Expected: GT_OK.
    1.2. Call Call function with index [8]
                                 and non-NULL mruPtr.
    Expected: GT_BAD_PARAM.
    1.3. Call function with mruPtr [NULL]
                            and index [0].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32 index = 0;
    GT_U32 mru   = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with index [0 / 7]
                                    and non-NULL mruPtr.
            Expected: GT_OK.
        */

        /* Call with index [0] */
        index = 0;
        
        st = cpssExMxPmBrgVlanUntaggedMruProfileValueGet(dev, index, &mru);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /* Call with index [7] */
        index = 7;
        
        st = cpssExMxPmBrgVlanUntaggedMruProfileValueGet(dev, index, &mru);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, index);

        /*  
            1.2. Call Call function with index [8]
                                         and non-NULL mruPtr.
            Expected: GT_BAD_PARAM.
        */
        index = 8;
        
        st = cpssExMxPmBrgVlanUntaggedMruProfileValueGet(dev, index, &mru);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, index);

        index = 0;

        /*
            1.3. Call function with mruPtr [NULL]
                                    and index [0].
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanUntaggedMruProfileValueGet(dev, index, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, mruPtr = NULL", dev);
    }

    index = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanUntaggedMruProfileValueGet(dev, index, &mru);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/

    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanUntaggedMruProfileValueGet(dev, index, &mru);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet
(
    IN GT_U8        devNum,
    IN GT_U16       vlanId,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with vlanId [100 / 4095]
                            and enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                            and enable [GT_TRUE].
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16  vlanId = 0;
    GT_BOOL enable = GT_FALSE;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call function with vlanId [100 / 4095]
                                    and enable [GT_TRUE / GT_FALSE].
            Expected: GT_OK.
        */

        /* Call with vlanId [100], enable [GT_FALSE] */
        vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
        enable = GT_FALSE;

        st = cpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet(dev, vlanId, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, enable);

        /* Call with vlanId [4095], enable [GT_TRUE] */
        vlanId = 4095;
        enable = GT_TRUE;

        st = cpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet(dev, vlanId, enable);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, enable);

        /*
            1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
                                    and enable [GT_TRUE].
            Expected: GT_BAD_PARAM.
        */
        vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;
        enable = GT_TRUE;

        st = cpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet(dev, vlanId, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);
    }

    vlanId = BRG_VLAN_TESTED_VLAN_ID_CNS;
    enable = GT_TRUE;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet(dev, vlanId, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet(dev, vlanId, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vlanId,
    IN  CPSS_UNREG_MC_EGR_FILTER_CMD_ENT    egressUnregMcFilterCmd
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with vlanId [100 / 4095]
                            and egressUnregMcFilterCmd [CPSS_UNREG_MC_VLAN_FRWD_E / 
                                                        CPSS_UNREG_MC_VLAN_CPU_FRWD_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet with non-NULL egressUnregMcFilterCmdPtr
                                                                  and vlanId the same as in 1.1.
    Expected: GT_OK and the same egressUnregMcFilterCmd.
    1.3. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
    Expected: GT_BAD_PARAM.
    1.4. Call function with out of range egressUnregMcFilterCmd [BRG_VLAN_INVALID_ENUM_CNS]
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st  = GT_OK;
    GT_U8       dev;

    GT_U16                           vlanId = 0;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT egressUnregMcFilterCmd    = CPSS_UNREG_MC_CPU_FRWD_E;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT egressUnregMcFilterCmdGet = CPSS_UNREG_MC_CPU_FRWD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with vlanId [100 / 4095]
                                    and egressUnregMcFilterCmd [CPSS_UNREG_MC_VLAN_FRWD_E / 
                                                                CPSS_UNREG_MC_VLAN_CPU_FRWD_E].
            Expected: GT_OK.
        */
        /* iterate with vlanId = 100 */
        vlanId = 100;

        egressUnregMcFilterCmd = CPSS_UNREG_MC_CPU_FRWD_E;

        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet(dev, vlanId, egressUnregMcFilterCmd);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, egressUnregMcFilterCmd);

        /*
            1.2. Call cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet with non-NULL egressUnregMcFilterCmdPtr
                                                                          and vlanId the same as in 1.1.
            Expected: GT_OK and the same egressUnregMcFilterCmd.
        */
        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet(dev, vlanId, &egressUnregMcFilterCmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet: %d, %d", dev, vlanId);

        /* Verifying mru */
        UTF_VERIFY_EQUAL1_STRING_MAC(egressUnregMcFilterCmd, egressUnregMcFilterCmdGet,
                   "get another egressUnregMcFilterCmd than was set: %d", dev);

        /* iterate with vlanId = 4095 */
        vlanId = 4095;

        egressUnregMcFilterCmd = CPSS_UNREG_MC_VLAN_CPU_FRWD_E;

        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet(dev, vlanId, egressUnregMcFilterCmd);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, vlanId, egressUnregMcFilterCmd);

        /*
            1.2. Call cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet with non-NULL egressUnregMcFilterCmdPtr
                                                                          and vlanId the same as in 1.1.
            Expected: GT_OK and the same egressUnregMcFilterCmd.
        */
        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet(dev, vlanId, &egressUnregMcFilterCmdGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                   "cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet: %d, %d", dev, vlanId);

        /* Verifying mru */
        UTF_VERIFY_EQUAL1_STRING_MAC(egressUnregMcFilterCmd, egressUnregMcFilterCmdGet,
                   "get another egressUnregMcFilterCmd than was set: %d", dev);

        /*
            1.3. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
            Expected: GT_BAD_PARAM.
        */
        vlanId = 4096;

        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet(dev, vlanId, egressUnregMcFilterCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = 100;

        /*
            1.4. Call function with out of range egressUnregMcFilterCmd [BRG_VLAN_INVALID_ENUM_CNS]
            Expected: GT_BAD_PARAM.
        */
        egressUnregMcFilterCmd = BRG_VLAN_INVALID_ENUM_CNS;

        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet(dev, vlanId, egressUnregMcFilterCmd);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, egressUnregMcFilterCmd = %d", dev, egressUnregMcFilterCmd);
    }

    vlanId = 100;

    egressUnregMcFilterCmd = CPSS_UNREG_MC_CPU_FRWD_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet(dev, vlanId, egressUnregMcFilterCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet(dev, vlanId, egressUnregMcFilterCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vlanId,
    OUT CPSS_UNREG_MC_EGR_FILTER_CMD_ENT    *egressUnregMcFilterCmdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call function with vlanId [100 / 4095]
                            and not NULL egressUnregMcFilterCmdPtr.
    Expected: GT_OK.
    1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
    Expected: GT_BAD_PARAM.
    1.3. Call function with egressUnregMcFilterCmdPtr [NULL]
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U16                              vlanId = 0;
    CPSS_UNREG_MC_EGR_FILTER_CMD_ENT    egressUnregMcFilterCmd = CPSS_UNREG_MC_CPU_FRWD_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /*
            1.1. Call function with vlanId [100 / 4095]
                                    and not NULL egressUnregMcFilterCmdPtr.
            Expected: GT_OK.
        */
        /* iterate with vlanId = 100 */
        vlanId = 100;

        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet(dev, vlanId, &egressUnregMcFilterCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /* iterate with vlanId = 4095 */
        vlanId = 4095;

        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet(dev, vlanId, &egressUnregMcFilterCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, vlanId);

        /*
            1.2. Call function with out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS]
            Expected: GT_BAD_PARAM.
        */
        vlanId = 4096;

        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet(dev, vlanId, &egressUnregMcFilterCmd);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, vlanId);

        vlanId = 100;

        /*
            1.3. Call function with egressUnregMcFilterCmdPtr [NULL]
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet(dev, vlanId, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, egressUnregMcFilterCmdPtr = NULL", dev);
    }

    vlanId = 100;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet(dev, vlanId, &egressUnregMcFilterCmd);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet(dev, vlanId, &egressUnregMcFilterCmd);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill vlan table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanFillVlanTable)
{
/*
    1. Call osMemStartHeapAllocationCounter set the current value of 
                        heap allocated bytes to the allocation counter
    ITERATE_DEVICE (ExMxPm)
    2.1. Get table Size.
    Expected: GT_OK.
    2.2. Fill all entries in vlan table.
         Call cpssExMxPmBrgVlanEntryWrite with vlanId [0..numEntries - 1],
                                    non-NULL portsMembersPtr [{5, 10}],
                                    non-NULL portsTaggingPtr [{10, 5}]
                                    and non-NULL vlanInfoPtr {cpuMember [GT_TRUE];
                                                              mirrToRxAnalyzerEn [GT_TRUE];
                                                              ipv4IpmBrgMode [CPSS_BRG_IPM_SGV_E] (relevant when ipv4IpmBrgEn = GT_TRUE);
                                                              ipv6IpmBrgMode [CPSS_BRG_IPM_SGV_E] (relevant when ipv6IpmBrgEn = GT_TRUE);
                                                              ipv4IpmBrgEn [GT_TRUE];
                                                              ipv6IpmBrgEn [GT_TRUE];
                                                              stgId [0];
                                                              egressUnregMcFilterCmd [CPSS_UNREG_MC_VLAN_FRWD_E]}.
    Expected: GT_OK.
    2.3. Try to write entry with index out of range.
         Call cpssExMxPmBrgVlanEntryWrite with vlanId [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    2.4. Read all entries in vlan table and compare with original.
         Call cpssExMxPmBrgVlanEntryRead with parameters the same as in 1.2.
                                                      and non-NULL isValidPtr.
    Expected: GT_OK, isValidPtr=GT_TRUE and the same portsMembersPtr, portsTaggingPtr and vlanInfoPtr.
    2.5. Try to read entry with index out of range.
         Call cpssExMxPmBrgVlanEntryRead with vlanId [numEntries] and other params from 1.4.
    Expected: GT_OK, isValidPtr = GT_FALSE.
    2.6. Delete all entries in vlan table.
         Call cpssExMxPmBrgVlanEntryInvalidate with vlanId the same as in 1.2.
    Expected: GT_OK.
    2.7. Try to delete entry with index out of range.
         Call cpssExMxPmBrgVlanEntryInvalidate with vlanId [numEntries].
    Expected: NOT GT_OK.
    3. Call osMemGetHeapAllocationCounter to returns the delta of current allocated 
                            bytes number and the value of allocation counter.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     isEqual         = GT_FALSE;
    GT_U16      numEntries      = 0;
    GT_U16      iTemp           = 0;
    GT_BOOL     isValid         = GT_FALSE;
    GT_U32      portsBitmap     = 0;
    GT_U32      portsMembersVal = 10; /* ports 33 & 35 (assign to bitmap 1)*/
    GT_U32      portsTaggingVal = 5;  /* ports 32 & 34 (assign to bitmap 1)*/
    GT_U32      mask            = 0;
    GT_U32      memDif          = 0;


    CPSS_PORTS_BMP_STC            portsMembers;
    CPSS_PORTS_BMP_STC            portsTagging;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC vlanInfo;
    CPSS_PORTS_BMP_STC            portsMembersGet;
    CPSS_PORTS_BMP_STC            portsTaggingGet;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC vlanInfoGet;


    cpssOsBzero((GT_VOID*) &portsMembers, sizeof(portsMembers));
    cpssOsBzero((GT_VOID*) &portsTagging, sizeof(portsTagging));
    cpssOsBzero((GT_VOID*) &vlanInfo, sizeof(vlanInfo));
    cpssOsBzero((GT_VOID*) &portsMembersGet, sizeof(portsMembersGet));
    cpssOsBzero((GT_VOID*) &portsTaggingGet, sizeof(portsTaggingGet));
    cpssOsBzero((GT_VOID*) &vlanInfoGet, sizeof(vlanInfoGet));
    
    /* Fill the entry for vlan table */
    /* check that the ports in the test are defined in the system */
    if ((portsBitmap < portsMembersVal) | (portsBitmap < portsTaggingVal))
    {           
        portsMembersVal = 0;
        portsTaggingVal = 0;
    }

    portsMembers.ports[0] = 5;                /* ports 0 & 2 */  
    portsMembers.ports[1] = portsMembersVal;  /* ports 33 & 35 */
    portsTagging.ports[0] = 10;               /* ports 1 & 3 */
    portsTagging.ports[1] = portsTaggingVal;  /* ports 32 & 34 */

    vlanInfo.cpuMember              = GT_FALSE;
    vlanInfo.mirrorToTxAnalyzerEn   = GT_FALSE;
    vlanInfo.ipv4IpmBrgMode         = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv6IpmBrgMode         = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv4IpmBrgEn           = GT_FALSE;
    vlanInfo.ipv6IpmBrgEn           = GT_FALSE;
    vlanInfo.stgId                  = 0;
    vlanInfo.egressUnregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;

    /*
        1. Call osMemStartHeapAllocationCounter set the current value of 
                            heap allocated bytes to the allocation counter
    */
    osMemStartHeapAllocationCounter();

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 2. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 2.1. Get table Size */
        numEntries = PRV_CPSS_MAX_NUM_VLANS_CNS;

        /* bitmap 1 is relevant only if more then 32 ports are defined */
        if (PRV_CPSS_PP_MAC(dev)->numOfVirtPorts > 32) 
        {
            mask = 1 << (PRV_CPSS_PP_MAC(dev)->numOfVirtPorts - 32);
            portsBitmap |= (mask - 1);
        }

        /* 2.2. Fill all entries in vlan table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmBrgVlanEntryWrite(dev, iTemp, &portsMembers, &portsTagging, &vlanInfo);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanEntryWrite: %d, %d", dev, iTemp);
        }

        /* 2.3. Try to write entry with index out of range. */
        st = cpssExMxPmBrgVlanEntryWrite(dev, numEntries, &portsMembers, &portsTagging, &vlanInfo);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanEntryWrite: %d, %d", dev, iTemp);

        /* 2.4. Read all entries in vlan table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmBrgVlanEntryRead(dev, iTemp, &portsMembersGet, &portsTaggingGet, &vlanInfoGet, &isValid);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanEntryRead: %d, %d", dev, iTemp);
        
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isValid, "Vlan entry is NOT valid: %d, %d", dev, iTemp);
        
            if (GT_TRUE == isValid)
            {
                /* Verifying portsMembersPtr */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsMembers,
                                             (GT_VOID*) &portsMembersGet,
                                             sizeof(portsMembers))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                           "get another portsMembersPtr than was set: %d, %d", dev, iTemp);
        
                /* Verifying portsTaggingPtr */
                isEqual = (0 == cpssOsMemCmp((GT_VOID*) &portsTagging,
                                             (GT_VOID*) &portsTaggingGet,
                                             sizeof(portsTagging))) ? GT_TRUE : GT_FALSE;
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_TRUE, isEqual,
                           "get another portsTaggingPtr than was set: %d, %d", dev, iTemp);
        
                /* Verifying vlanInfoPtr fields */
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.cpuMember,
                                             vlanInfoGet.cpuMember,
                                             "get another vlanInfoPtr->cpuMember than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.mirrorToTxAnalyzerEn,
                                             vlanInfoGet.mirrorToTxAnalyzerEn,
                                             "get another vlanInfoPtr->mirrorToTxAnalyzerEn than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgEn,
                                             vlanInfoGet.ipv4IpmBrgEn,
                                             "get another vlanInfoPtr->ipv4IpmBrgEn than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgEn,
                                             vlanInfoGet.ipv6IpmBrgEn,
                                             "get another vlanInfoPtr->ipv6IpmBrgEn than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.stgId,
                                             vlanInfoGet.stgId,
                                             "get another vlanInfoPtr->stgId than was set: %d, %d", dev, iTemp);
                UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.egressUnregMcFilterCmd,
                                             vlanInfoGet.egressUnregMcFilterCmd,
                                             "get another vlanInfoPtr->egressUnregMcFilterCmd than was set: %d, %d", dev, iTemp);
        
                if (GT_TRUE == vlanInfo.ipv4IpmBrgEn)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv4IpmBrgMode,
                                                 vlanInfoGet.ipv4IpmBrgMode,
                                                 "get another vlanInfoPtr->ipv4IpmBrgMode than was set: %d, %d", dev, iTemp);
                }
        
                if (GT_TRUE == vlanInfo.ipv6IpmBrgEn)
                {
                    UTF_VERIFY_EQUAL2_STRING_MAC(vlanInfo.ipv6IpmBrgMode,
                                                 vlanInfoGet.ipv6IpmBrgMode,
                                                 "get another vlanInfoPtr->ipv6IpmBrgMode than was set: %d, %d", dev, iTemp);
                }
            }
        }

        /* 2.5. Try to read entry with index out of range. */
        st = cpssExMxPmBrgVlanEntryRead(dev, numEntries, &portsMembersGet, &portsTaggingGet, &vlanInfoGet, &isValid);
        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, isValid, "Vlan entry is NOT valid: %d, %d", dev, iTemp);
        }
        
        /* 2.6. Delete all entries in vlan table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmBrgVlanEntryInvalidate(dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanEntryInvalidate: %d, %d", dev, iTemp);
        }

        /* 2.7. Try to delete entry with index out of range. */
        st = cpssExMxPmBrgVlanEntryInvalidate(dev, numEntries);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanEntryInvalidate: %d, %d", dev, iTemp);
    }

    /*
        3. Call osMemGetHeapAllocationCounter to returns the delta of current allocated 
                                bytes number and the value of allocation counter.
    */
    memDif = osMemGetHeapAllocationCounter();
    UTF_VERIFY_EQUAL0_STRING_MAC(0, memDif, "size of heap changed (heap_bytes_allocated)");
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill VlanProtocol table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgVlanFillVlanProtocolTable)
{
/*
    1. Call osMemStartHeapAllocationCounter set the current value of 
                        heap allocated bytes to the allocation counter
    ITERATE_DEVICE (ExMxPm)
    2.1. Get table Size.
    Expected: GT_OK.
    2.2. Fill all entries in VlanProtocol table.
         Call cpssExMxPmBrgVlanProtocolEntrySet with index [0..numEntries - 1],
                                            etherType [0]
                                            and non-NULL encListPtr {ethV2 [GT_TRUE];
                                                                     nonLlcSnap [GT_TRUE];
                                                                     llcSnap [GT_TRUE]}.
    Expected: GT_OK.
    2.3. Try to write entry with index out of range.
         Call cpssExMxPmBrgVlanProtocolEntrySet with index [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    2.4. Read all entries in VlanProtocol table and compare with original.
         Call cpssExMxPmBrgVlanProtocolEntryGet with non-NULL etherTypePtr,
                                                     non-NULL encListPtr and non-NULL validEntryPtr.
    Expected: GT_OK, validEntryPtr=GT_TRUE and the same etherType and encListPtr.
    2.5. Try to read entry with index out of range.
         Call cpssExMxPmBrgVlanProtocolEntryGet with index [numEntries] and other params from 1.4.
    Expected: GT_OK, validEntryPtr = GT_FALSE.
    2.6. Delete all entries in VlanProtocol table.
         Call cpssExMxPmBrgVlanProtocolEntryInvalidate.
    Expected: GT_OK.
    2.7. Try to delete entry with index out of range.
         Call cpssExMxPmBrgVlanProtocolEntryInvalidate with index [numEntries].
    Expected: NOT GT_OK.
    3. Call osMemGetHeapAllocationCounter to returns the delta of current allocated 
                            bytes number and the value of allocation counter.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries   = 0;
    GT_U32      iTemp        = 0;
    GT_U16      etherType    = 0;
    GT_U16      etherTypeGet = 0;
    GT_BOOL     isValidEntry = GT_FALSE;

    CPSS_PROT_CLASS_ENCAP_STC encList;
    CPSS_PROT_CLASS_ENCAP_STC encListGet;


    cpssOsBzero((GT_VOID*) &encList, sizeof(encList));
    cpssOsBzero((GT_VOID*) &encListGet, sizeof(encListGet));

    /* Fill the entry for VlanProtocol table */
    etherType = 1;

    encList.ethV2      = GT_FALSE;
    encList.nonLlcSnap = GT_FALSE;
    encList.llcSnap    = GT_FALSE;

    /*
        1. Call osMemStartHeapAllocationCounter set the current value of 
                            heap allocated bytes to the allocation counter
    */
    osMemStartHeapAllocationCounter();

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 2. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 2.1. Get table Size */
        numEntries = 12;

        /* 2.2. Fill all entries in VlanProtocol table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmBrgVlanProtocolEntrySet(dev, iTemp, etherType, &encList);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanProtocolEntrySet: %d, %d, %d", dev, iTemp, etherType);
        }

        /* 2.3. Try to write entry with index out of range. */
        st = cpssExMxPmBrgVlanProtocolEntrySet(dev, numEntries, etherType, &encList);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanProtocolEntrySet: %d, %d, %d", dev, iTemp, etherType);

        /* 2.4. Read all entries in VlanProtocol table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmBrgVlanProtocolEntryGet(dev, iTemp, &etherTypeGet, &encListGet, &isValidEntry);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanProtocolEntryGet: %d, %d", dev, iTemp);
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_TRUE, isValidEntry, "Entry is not valid: %d", dev);
        
            if (GT_TRUE == isValidEntry)
            {
                UTF_VERIFY_EQUAL1_STRING_MAC(etherType, etherTypeGet,
                                             "get another etherType than was set: %d", dev);
        
                /* Verifying struct fields */
                UTF_VERIFY_EQUAL1_STRING_MAC(encList.ethV2, encListGet.ethV2,
                                             "get another encListPtr->ethV2 than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(encList.nonLlcSnap, encListGet.nonLlcSnap,
                                             "get another encListPtr->nonLlcSnap than was set: %d", dev);
                UTF_VERIFY_EQUAL1_STRING_MAC(encList.llcSnap, encListGet.llcSnap,
                                             "get another encListPtr->llcSnap than was set: %d", dev);
            }
        }

        /* 2.5. Try to read entry with index out of range. */
        st = cpssExMxPmBrgVlanProtocolEntryGet(dev, numEntries, &etherTypeGet, &encListGet, &isValidEntry);
        if(st == GT_OK)
        {
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_FALSE, isValidEntry, "Entry is not valid: %d", dev);
        }

        /* 2.6. Delete all entries in VlanProtocol table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmBrgVlanProtocolEntryInvalidate(dev, iTemp);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanProtocolEntryInvalidate: %d, %d", dev, iTemp);
        }

        /* 2.7. Try to delete entry with index out of range. */
        st = cpssExMxPmBrgVlanProtocolEntryInvalidate(dev, numEntries);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmBrgVlanProtocolEntryInvalidate: %d, %d", dev, iTemp);
    }

    /*
        3. Call osMemGetHeapAllocationCounter to returns the delta of current allocated 
                                bytes number and the value of allocation counter.
    */
    iTemp = osMemGetHeapAllocationCounter();
    UTF_VERIFY_EQUAL0_STRING_MAC(0, iTemp, "size of heap changed (heap_bytes_allocated)");
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmBrgVlan suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmBrgVlan)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanMemberSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanCpuMemberSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortAcceptFrameTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortAcceptFrameTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortPvidSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortPvidGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortPvidPrecedenceSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortPvidPrecedenceGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortForcePvidEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortForcePvidEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanProtocolEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanProtocolEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanProtocolEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortProtocolVidEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortProtocolVidEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortProtocolQosEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortProtocolQosEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortProtocolVidQosEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortProtocolVidQosEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortProtocolVidQosEntryInvalidate)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortIngressFilterEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortIngressFilterEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanInvalidFilterEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanInvalidFilterEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanIngressEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanIngressEtherTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanEgressEtherTypeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanEgressEtherTypeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanStgIdBind)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanIpmBridgingEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanIpmBridgingModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortUntaggedMruProfileIndexSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanPortUntaggedMruProfileIndexGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanUntaggedMruProfileValueSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanUntaggedMruProfileValueGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanMirrorToTxAnalyzerEnableSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanEgressUnregMcFilterCmdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanEgressUnregMcFilterCmdGet)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanFillVlanTable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgVlanFillVlanProtocolTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmBrgVlan)

