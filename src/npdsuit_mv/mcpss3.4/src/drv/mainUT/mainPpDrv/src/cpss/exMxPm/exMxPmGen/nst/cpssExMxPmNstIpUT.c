/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmNstIpUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmNstIp, that provides
*       CPSS EXMXPM NST IP support.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/nst/cpssExMxPmNstIp.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Invalid enum */
#define NST_IP_INVALID_ENUM_CNS    0x5AAAAAA5


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstIpProfileTableEntrySet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          profileIndex,
    IN  CPSS_EXMXPM_NST_IP_PROFILE_STC  *profilePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstIpProfileTableEntrySet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileIndex [0 / 15]
                   and profilePtr {enableIpv4uRpfForBridgedTraffic [GT_FALSE / GT_TRUE],
                                   enableIpv4HeaderCheckForBridgedTraffic [GT_FALSE / GT_TRUE],
                                   enableIpv4SipSaCheckForBridgedTraffic [GT_FALSE / GT_TRUE],
                                   enableIpv4SipFilterForBridgedTraffic [GT_FALSE / GT_TRUE],
                                   enableIpv4uRpfForRoutedTraffic [GT_FALSE / GT_TRUE],
                                   enableIpv4SipSaCheckForRoutedTraffic [GT_FALSE / GT_TRUE],
                                   enableIpv4SipFilterForRoutedTraffic [GT_FALSE / GT_TRUE],
                                   enableIpv4SipLookupForAccessMatrix [GT_FALSE / GT_TRUE],
                                   ipv4DefaultSipAccessLevel [GT_FALSE / GT_TRUE],
                                   enableArpuRpf [GT_FALSE / GT_TRUE],
                                   enableArpSipSaCheck [GT_FALSE / GT_TRUE],
                                   enableArpSipFilter [GT_FALSE / GT_TRUE]}.
    Expected: GT_OK.
    1.2. Call cpssExMxPmNstIpProfileTableEntryGet with the same profileIndex and non-NULL profilePtr.
    Expected: GT_OK and the same profile
    1.3. Call with out of range profileIndex [16]
                   and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.4. Call with profilePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                          profileIndex = 0;
    CPSS_EXMXPM_NST_IP_PROFILE_STC  profile;
    CPSS_EXMXPM_NST_IP_PROFILE_STC  profileGet;

    cpssOsBzero((GT_VOID*) &profile, sizeof(profile));
    cpssOsBzero((GT_VOID*) &profileGet, sizeof(profileGet));

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with profileIndex [0 / 15]
                           and profilePtr {enableIpv4uRpfForBridgedTraffic [GT_FALSE / GT_TRUE],
                                           enableIpv4HeaderCheckForBridgedTraffic [GT_FALSE / GT_TRUE],
                                           enableIpv4SipSaCheckForBridgedTraffic [GT_FALSE / GT_TRUE],
                                           enableIpv4SipFilterForBridgedTraffic [GT_FALSE / GT_TRUE],
                                           enableIpv4uRpfForRoutedTraffic [GT_FALSE / GT_TRUE],
                                           enableIpv4SipSaCheckForRoutedTraffic [GT_FALSE / GT_TRUE],
                                           enableIpv4SipFilterForRoutedTraffic [GT_FALSE / GT_TRUE],
                                           enableIpv4SipLookupForAccessMatrix [GT_FALSE / GT_TRUE],
                                           ipv4DefaultSipAccessLevel [GT_FALSE / GT_TRUE],
                                           enableArpuRpf [GT_FALSE / GT_TRUE],
                                           enableArpSipSaCheck [GT_FALSE / GT_TRUE],
                                           enableArpSipFilter [GT_FALSE / GT_TRUE]}.
            Expected: GT_OK.
        */

        /* Call with profileIndex [0] */
        profileIndex = 0;

        profile.enableIpv4uRpfForBridgedTraffic        = GT_FALSE;
        profile.enableIpv4HeaderCheckForBridgedTraffic = GT_FALSE;
        profile.enableIpv4SipSaCheckForBridgedTraffic  = GT_FALSE;
        profile.enableIpv4SipFilterForBridgedTraffic   = GT_FALSE;
        profile.enableIpv4uRpfForRoutedTraffic         = GT_FALSE;

        profile.enableIpv4SipSaCheckForRoutedTraffic   = GT_FALSE;
        profile.enableIpv4SipFilterForRoutedTraffic    = GT_FALSE;
        profile.enableIpv4SipLookupForAccessMatrix     = GT_FALSE;
        profile.ipv4DefaultSipAccessLevel              = GT_FALSE;
        profile.enableArpuRpf                          = GT_FALSE;
        profile.enableArpSipSaCheck                    = GT_FALSE;
        profile.enableArpSipFilter                     = GT_FALSE;
        
        st = cpssExMxPmNstIpProfileTableEntrySet(dev, profileIndex, &profile);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        /*
            1.2. Call cpssExMxPmNstIpProfileTableEntryGet with the same profileIndex and non-NULL profilePtr.
            Expected: GT_OK and the same profile
        */
        st = cpssExMxPmNstIpProfileTableEntryGet(dev, profileIndex, &profileGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstIpProfileTableEntryGet: %d, %d", dev, profileIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4uRpfForBridgedTraffic, profileGet.enableIpv4uRpfForBridgedTraffic,
                       "get another profilePtr->enableIpv4uRpfForBridgedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4HeaderCheckForBridgedTraffic, profileGet.enableIpv4HeaderCheckForBridgedTraffic,
                       "get another profilePtr->enableIpv4HeaderCheckForBridgedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipSaCheckForBridgedTraffic, profileGet.enableIpv4SipSaCheckForBridgedTraffic,
                       "get another profilePtr->enableIpv4SipSaCheckForBridgedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipFilterForBridgedTraffic, profileGet.enableIpv4SipFilterForBridgedTraffic,
                       "get another profilePtr->enableIpv4SipFilterForBridgedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4uRpfForRoutedTraffic, profileGet.enableIpv4uRpfForRoutedTraffic,
                       "get another profilePtr->enableIpv4uRpfForRoutedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipSaCheckForRoutedTraffic, profileGet.enableIpv4SipSaCheckForRoutedTraffic,
                       "get another profilePtr->enableIpv4SipSaCheckForRoutedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipFilterForRoutedTraffic, profileGet.enableIpv4SipFilterForRoutedTraffic,
                       "get another profilePtr->enableIpv4SipFilterForRoutedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipLookupForAccessMatrix, profileGet.enableIpv4SipLookupForAccessMatrix,
                       "get another profilePtr->enableIpv4SipLookupForAccessMatrix than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.ipv4DefaultSipAccessLevel, profileGet.ipv4DefaultSipAccessLevel,
                       "get another profilePtr->ipv4DefaultSipAccessLevel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableArpuRpf, profileGet.enableArpuRpf,
                       "get another profilePtr->enableArpuRpf than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableArpSipSaCheck, profileGet.enableArpSipSaCheck,
                       "get another profilePtr->enableArpSipSaCheck than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableArpSipFilter, profileGet.enableArpSipFilter,
                       "get another profilePtr->enableArpSipFilter than was set: %d", dev);

        /* Call with profileIndex [15] */
        profileIndex = 15;

        profile.enableIpv4uRpfForBridgedTraffic        = GT_TRUE;
        profile.enableIpv4HeaderCheckForBridgedTraffic = GT_TRUE;
        profile.enableIpv4SipSaCheckForBridgedTraffic  = GT_TRUE;
        profile.enableIpv4SipFilterForBridgedTraffic   = GT_TRUE;
        profile.enableIpv4uRpfForRoutedTraffic         = GT_TRUE;

        profile.enableIpv4SipSaCheckForRoutedTraffic   = GT_TRUE;
        profile.enableIpv4SipFilterForRoutedTraffic    = GT_TRUE;
        profile.enableIpv4SipLookupForAccessMatrix     = GT_TRUE;
        profile.ipv4DefaultSipAccessLevel              = GT_TRUE;
        profile.enableArpuRpf                          = GT_TRUE;
        profile.enableArpSipSaCheck                    = GT_TRUE;
        profile.enableArpSipFilter                     = GT_TRUE;
        
        st = cpssExMxPmNstIpProfileTableEntrySet(dev, profileIndex, &profile);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        /*
            1.2. Call cpssExMxPmNstIpProfileTableEntryGet with the same profileIndex and non-NULL profilePtr.
            Expected: GT_OK and the same profile
        */
        st = cpssExMxPmNstIpProfileTableEntryGet(dev, profileIndex, &profileGet);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstIpProfileTableEntryGet: %d, %d", dev, profileIndex);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4uRpfForBridgedTraffic, profileGet.enableIpv4uRpfForBridgedTraffic,
                       "get another profilePtr->enableIpv4uRpfForBridgedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4HeaderCheckForBridgedTraffic, profileGet.enableIpv4HeaderCheckForBridgedTraffic,
                       "get another profilePtr->enableIpv4HeaderCheckForBridgedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipSaCheckForBridgedTraffic, profileGet.enableIpv4SipSaCheckForBridgedTraffic,
                       "get another profilePtr->enableIpv4SipSaCheckForBridgedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipFilterForBridgedTraffic, profileGet.enableIpv4SipFilterForBridgedTraffic,
                       "get another profilePtr->enableIpv4SipFilterForBridgedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4uRpfForRoutedTraffic, profileGet.enableIpv4uRpfForRoutedTraffic,
                       "get another profilePtr->enableIpv4uRpfForRoutedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipSaCheckForRoutedTraffic, profileGet.enableIpv4SipSaCheckForRoutedTraffic,
                       "get another profilePtr->enableIpv4SipSaCheckForRoutedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipFilterForRoutedTraffic, profileGet.enableIpv4SipFilterForRoutedTraffic,
                       "get another profilePtr->enableIpv4SipFilterForRoutedTraffic than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipLookupForAccessMatrix, profileGet.enableIpv4SipLookupForAccessMatrix,
                       "get another profilePtr->enableIpv4SipLookupForAccessMatrix than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.ipv4DefaultSipAccessLevel, profileGet.ipv4DefaultSipAccessLevel,
                       "get another profilePtr->ipv4DefaultSipAccessLevel than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableArpuRpf, profileGet.enableArpuRpf,
                       "get another profilePtr->enableArpuRpf than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableArpSipSaCheck, profileGet.enableArpSipSaCheck,
                       "get another profilePtr->enableArpSipSaCheck than was set: %d", dev);
        UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableArpSipFilter, profileGet.enableArpSipFilter,
                       "get another profilePtr->enableArpSipFilter than was set: %d", dev);

        /*
            1.3. Call with out of range profileIndex [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        profileIndex = 16;

        st = cpssExMxPmNstIpProfileTableEntrySet(dev, profileIndex, &profile);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        profileIndex = 0;

        /*
            1.4. Call with profilePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNstIpProfileTableEntrySet(dev, profileIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, profilePtr = NULL", dev);
    }

    profileIndex = 0;

    profile.enableIpv4uRpfForBridgedTraffic        = GT_TRUE;
    profile.enableIpv4HeaderCheckForBridgedTraffic = GT_TRUE;
    profile.enableIpv4SipSaCheckForBridgedTraffic  = GT_TRUE;
    profile.enableIpv4SipFilterForBridgedTraffic   = GT_TRUE;
    profile.enableIpv4uRpfForRoutedTraffic         = GT_TRUE;

    profile.enableIpv4SipSaCheckForRoutedTraffic   = GT_TRUE;
    profile.enableIpv4SipFilterForRoutedTraffic    = GT_TRUE;
    profile.enableIpv4SipLookupForAccessMatrix     = GT_TRUE;
    profile.ipv4DefaultSipAccessLevel              = GT_TRUE;
    profile.enableArpuRpf                          = GT_TRUE;
    profile.enableArpSipSaCheck                    = GT_TRUE;
    profile.enableArpSipFilter                     = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstIpProfileTableEntrySet(dev, profileIndex, &profile);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstIpProfileTableEntrySet(dev, profileIndex, &profile);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstIpProfileTableEntryGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          profileIndex,
    OUT CPSS_EXMXPM_NST_IP_PROFILE_STC  *profilePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstIpProfileTableEntryGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with profileIndex [0 / 15]
                   and non-NULL profilePtr.
    Expected: GT_OK.
    1.2. Call with out of range profileIndex [16] and other parameters from 1.1.
    Expected: NOT GT_OK.
    1.3. Call with profilePtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                          profileIndex = 0;
    CPSS_EXMXPM_NST_IP_PROFILE_STC  profile;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with profileIndex [0 / 15]
                           and non-NULL profilePtr.
            Expected: GT_OK.
        */

        /* Call with profileIndex [0] */
        profileIndex = 0;

        st = cpssExMxPmNstIpProfileTableEntryGet(dev, profileIndex, &profile);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        /* Call with profileIndex [15] */
        profileIndex = 15;

        st = cpssExMxPmNstIpProfileTableEntryGet(dev, profileIndex, &profile);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        /*
            1.2. Call with out of range profileIndex [16]
                           and other parameters from 1.1.
            Expected: NOT GT_OK.
        */
        profileIndex = 16;

        st = cpssExMxPmNstIpProfileTableEntryGet(dev, profileIndex, &profile);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, profileIndex);

        profileIndex = 0;

        /*
            1.3. Call with profilePtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNstIpProfileTableEntryGet(dev, profileIndex, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, profilePtr = NULL", dev);
    }

    profileIndex = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstIpProfileTableEntryGet(dev, profileIndex, &profile);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstIpProfileTableEntryGet(dev, profileIndex, &profile);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstIpRouterAccessMatrixCommandSet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  sipAccessLevel,
    IN  GT_U32                  dipAccessLevel,
    IN  CPSS_PACKET_CMD_ENT     command
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstIpRouterAccessMatrixCommandSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with sipAccessLevel [0 / 7],
                   dipAccessLevel [5 / 7]
                   and command [CPSS_PACKET_CMD_ROUTE_E /
                                CPSS_PACKET_CMD_DROP_HARD_E /
                                CPSS_PACKET_CMD_DROP_SOFT_E].
    Expected: GT_OK.
    1.2. Call cpssExMxPmNstIpRouterAccessMatrixCommandGet with the same sipAccessLevel, dipAccessLevel and non-NULL commandPtr.
    Expected: GT_OK and the same command.
    1.3. Call with out of range sipAccessLevel [8]
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.4. Call with out of range dipAccessLevel [8]
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.5. Call with command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                            CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                            CPSS_PACKET_CMD_FORWARD_E] (not supported)
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.6. Call with out of range command [0x5AAAAAA5]
                   and other parameters from 1.1.
    Expected: GT_BAD_PARAM.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                  sipAccessLevel = 0;
    GT_U32                  dipAccessLevel = 0;
    CPSS_PACKET_CMD_ENT     command        = CPSS_PACKET_CMD_ROUTE_E;
    CPSS_PACKET_CMD_ENT     commandGet     = CPSS_PACKET_CMD_ROUTE_E;


    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with sipAccessLevel [0 / 3 / 7],
                           dipAccessLevel [5 / 6 / 7]
                           and command [CPSS_PACKET_CMD_ROUTE_E /
                                        CPSS_PACKET_CMD_DROP_HARD_E /
                                        CPSS_PACKET_CMD_DROP_SOFT_E].
            Expected: GT_OK.
        */

        /* Call with command [CPSS_PACKET_CMD_ROUTE_E] */
        sipAccessLevel = 0;
        dipAccessLevel = 5;
        command        = CPSS_PACKET_CMD_ROUTE_E;
        
        st = cpssExMxPmNstIpRouterAccessMatrixCommandSet(dev, sipAccessLevel, dipAccessLevel, command);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, sipAccessLevel, dipAccessLevel, command);

        /*
            1.2. Call cpssExMxPmNstIpRouterAccessMatrixCommandGet with the same sipAccessLevel, dipAccessLevel and non-NULL commandPtr.
            Expected: GT_OK and the same command.
        */
        st = cpssExMxPmNstIpRouterAccessMatrixCommandGet(dev, sipAccessLevel, dipAccessLevel, &commandGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstIpRouterAccessMatrixCommandGet: %d, %d, %d", dev, sipAccessLevel, dipAccessLevel);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet,
                       "get another command than was set: %d", dev);

        /* Call with command [CPSS_PACKET_CMD_DROP_HARD_E] */
        sipAccessLevel = 3;
        dipAccessLevel = 6;
        command        = CPSS_PACKET_CMD_DROP_HARD_E;
        
        st = cpssExMxPmNstIpRouterAccessMatrixCommandSet(dev, sipAccessLevel, dipAccessLevel, command);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, sipAccessLevel, dipAccessLevel, command);

        /*
            1.2. Call cpssExMxPmNstIpRouterAccessMatrixCommandGet with the same sipAccessLevel, dipAccessLevel and non-NULL commandPtr.
            Expected: GT_OK and the same command.
        */
        st = cpssExMxPmNstIpRouterAccessMatrixCommandGet(dev, sipAccessLevel, dipAccessLevel, &commandGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstIpRouterAccessMatrixCommandGet: %d, %d, %d", dev, sipAccessLevel, dipAccessLevel);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet,
                       "get another command than was set: %d", dev);

        /* Call with command [CPSS_PACKET_CMD_DROP_SOFT_E] */
        sipAccessLevel = 7;
        dipAccessLevel = 7;
        command        = CPSS_PACKET_CMD_DROP_SOFT_E;
        
        st = cpssExMxPmNstIpRouterAccessMatrixCommandSet(dev, sipAccessLevel, dipAccessLevel, command);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, sipAccessLevel, dipAccessLevel, command);

        /*
            1.2. Call cpssExMxPmNstIpRouterAccessMatrixCommandGet with the same sipAccessLevel, dipAccessLevel and non-NULL commandPtr.
            Expected: GT_OK and the same command.
        */
        st = cpssExMxPmNstIpRouterAccessMatrixCommandGet(dev, sipAccessLevel, dipAccessLevel, &commandGet);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstIpRouterAccessMatrixCommandGet: %d, %d, %d", dev, sipAccessLevel, dipAccessLevel);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(command, commandGet,
                       "get another command than was set: %d", dev);

        /*
            1.3. Call with out of range sipAccessLevel [8]
                   and other parameters from 1.1.
            Expected: NON GT_OK.
        */
        sipAccessLevel = 8;

        st = cpssExMxPmNstIpRouterAccessMatrixCommandSet(dev, sipAccessLevel, dipAccessLevel, command);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, sipAccessLevel);

        sipAccessLevel = 0;

        /*
            1.4. Call with out of range dipAccessLevel [8]
                           and other parameters from 1.1.
            Expected: NON GT_OK.
        */
        dipAccessLevel = 8;

        st = cpssExMxPmNstIpRouterAccessMatrixCommandSet(dev, sipAccessLevel, dipAccessLevel, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dipAccessLevel = %d", dev, dipAccessLevel);

        dipAccessLevel = 0;

        /*
            1.5. Call with command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E /
                                    CPSS_PACKET_CMD_TRAP_TO_CPU_E /
                                    CPSS_PACKET_CMD_FORWARD_E] (not supported)
                           and other parameters from 1.1.
            Expected: NON GT_OK.
        */

        /* Call with command [CPSS_PACKET_CMD_MIRROR_TO_CPU_E] */
        command = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;

        st = cpssExMxPmNstIpRouterAccessMatrixCommandSet(dev, sipAccessLevel, dipAccessLevel, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, command = %d", dev, command);

        /* Call with command [CPSS_PACKET_CMD_TRAP_TO_CPU_E] */
        command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;

        st = cpssExMxPmNstIpRouterAccessMatrixCommandSet(dev, sipAccessLevel, dipAccessLevel, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, command = %d", dev, command);

        /* Call with command [CPSS_PACKET_CMD_FORWARD_E] */
        command = CPSS_PACKET_CMD_FORWARD_E;

        st = cpssExMxPmNstIpRouterAccessMatrixCommandSet(dev, sipAccessLevel, dipAccessLevel, command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, command = %d", dev, command);

        /*
            1.6. Call with out of range command [0x5AAAAAA5]
                           and other parameters from 1.1.
            Expected: GT_BAD_PARAM.
        */
        command = NST_IP_INVALID_ENUM_CNS;

        st = cpssExMxPmNstIpRouterAccessMatrixCommandSet(dev, sipAccessLevel, dipAccessLevel, command);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, command = %d", dev, command);
    }

    sipAccessLevel = 0;
    dipAccessLevel = 5;
    command        = CPSS_PACKET_CMD_ROUTE_E;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstIpRouterAccessMatrixCommandSet(dev, sipAccessLevel, dipAccessLevel, command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstIpRouterAccessMatrixCommandSet(dev, sipAccessLevel, dipAccessLevel, command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstIpRouterAccessMatrixCommandGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  sipAccessLevel,
    IN  GT_U32                  dipAccessLevel,
    OUT CPSS_PACKET_CMD_ENT     *commandPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstIpRouterAccessMatrixCommandGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with sipAccessLevel [7],
                   dipAccessLevel [0]
                   and non-null commandPtr.
    Expected: GT_OK.
    1.2. Call with out of range sipAccessLevel [8]
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.3. Call with out of range dipAccessLevel [8]
                   and other parameters from 1.1.
    Expected: NON GT_OK.
    1.4. Call with commandPtr [NULL]
                   and other parameters from 1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32                  sipAccessLevel = 0;
    GT_U32                  dipAccessLevel = 0;
    CPSS_PACKET_CMD_ENT     command        = CPSS_PACKET_CMD_ROUTE_E;
    

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with sipAccessLevel [7],
                           dipAccessLevel [0]
                           and non-null commandPtr.
            Expected: GT_OK.
        */
        sipAccessLevel = 7;
        dipAccessLevel = 0;
        
        st = cpssExMxPmNstIpRouterAccessMatrixCommandGet(dev, sipAccessLevel, dipAccessLevel, &command);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, sipAccessLevel, dipAccessLevel);

        /*
            1.2. Call with out of range sipAccessLevel [8]
                   and other parameters from 1.1.
            Expected: NON GT_OK.
        */
        sipAccessLevel = 8;

        st = cpssExMxPmNstIpRouterAccessMatrixCommandGet(dev, sipAccessLevel, dipAccessLevel, &command);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, sipAccessLevel);

        sipAccessLevel = 0;

        /*
            1.3. Call with out of range dipAccessLevel [8]
                           and other parameters from 1.1.
            Expected: NON GT_OK.
        */
        dipAccessLevel = 8;

        st = cpssExMxPmNstIpRouterAccessMatrixCommandGet(dev, sipAccessLevel, dipAccessLevel, &command);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, dipAccessLevel = %d", dev, dipAccessLevel);

        dipAccessLevel = 0;

        /*
            1.4. Call with commandPtr [NULL]
                           and other parameters from 1.1.
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNstIpRouterAccessMatrixCommandGet(dev, sipAccessLevel, dipAccessLevel, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, commandPtr = NULL", dev);
    }

    sipAccessLevel = 0;
    dipAccessLevel = 5;
    
    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstIpRouterAccessMatrixCommandGet(dev, sipAccessLevel, dipAccessLevel, &command);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstIpRouterAccessMatrixCommandGet(dev, sipAccessLevel, dipAccessLevel, &command);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstIpBridgedPktExceptionsCounterSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      value
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstIpBridgedPktExceptionsCounterSet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with value [0 / 15 / 0xFFFFFFFF].
    Expected: GT_OK.
    1.2. Call cpssExMxPmNstIpBridgedPktExceptionsCounterGet with non-NULL valuePtr.
    Expected: GT_OK and the same value.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      value    = 0;
    GT_U32      valueGet = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with value [0 / 15 / 0xFFFFFFFF].
            Expected: GT_OK.
        */

        /* Call with value [0] */
        value = 0;

        st = cpssExMxPmNstIpBridgedPktExceptionsCounterSet(dev, value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, value);

        /*
            1.2. Call cpssExMxPmNstIpBridgedPktExceptionsCounterGet with non-NULL valuePtr.
            Expected: GT_OK and the same value.
        */
        st = cpssExMxPmNstIpBridgedPktExceptionsCounterGet(dev, &valueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstIpBridgedPktExceptionsCounterGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(value, valueGet,
                       "get another value than was set: %d", dev);

        /* Call with value [15] */
        value = 15;

        st = cpssExMxPmNstIpBridgedPktExceptionsCounterSet(dev, value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, value);

        /*
            1.2. Call cpssExMxPmNstIpBridgedPktExceptionsCounterGet with non-NULL valuePtr.
            Expected: GT_OK and the same value.
        */
        st = cpssExMxPmNstIpBridgedPktExceptionsCounterGet(dev, &valueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstIpBridgedPktExceptionsCounterGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(value, valueGet,
                       "get another value than was set: %d", dev);

        /* Call with value [0xFFFFFFFF] */
        value = 0xFFFFFFFF;

        st = cpssExMxPmNstIpBridgedPktExceptionsCounterSet(dev, value);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, value);

        /*
            1.2. Call cpssExMxPmNstIpBridgedPktExceptionsCounterGet with non-NULL valuePtr.
            Expected: GT_OK and the same value.
        */
        st = cpssExMxPmNstIpBridgedPktExceptionsCounterGet(dev, &valueGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmNstIpBridgedPktExceptionsCounterGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(value, valueGet,
                       "get another value than was set: %d", dev);
    }

    value = 0;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstIpBridgedPktExceptionsCounterSet(dev, value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstIpBridgedPktExceptionsCounterSet(dev, value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmNstIpBridgedPktExceptionsCounterGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *valuePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstIpBridgedPktExceptionsCounterGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-null valuePtr. 
    Expected: GT_OK.
    1.2. Call with valuePtr [NULL]. 
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      value = 0;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 
            1.1. Call with non-null valuePtr.
            Expected: GT_OK.
        */
        st = cpssExMxPmNstIpBridgedPktExceptionsCounterGet(dev, &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null valuePtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmNstIpBridgedPktExceptionsCounterGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, valuePtr = NULL", dev);
    }

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmNstIpBridgedPktExceptionsCounterGet(dev, &value);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmNstIpBridgedPktExceptionsCounterGet(dev, &value);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
    Test function to Fill IpProfile table.
*/
UTF_TEST_CASE_MAC(cpssExMxPmNstIpFillIpProfileTable)
{
/*
    ITERATE_DEVICE (ExMxPm)
    1.1. Get table Size.
    Expected: GT_OK.
    1.2. Fill all entries in IpProfile table.
         Call cpssExMxPmNstIpProfileTableEntrySet with profileIndex [0..numEntries - 1]
                                                       and profilePtr {enableIpv4uRpfForBridgedTraffic [GT_TRUE],
                                                                       enableIpv4HeaderCheckForBridgedTraffic [GT_FALSE],
                                                                       enableIpv4SipSaCheckForBridgedTraffic [GT_TRUE],
                                                                       enableIpv4SipFilterForBridgedTraffic [GT_FALSE],
                                                                       enableIpv4uRpfForRoutedTraffic [GT_TRUE],
                                                                       enableIpv4SipSaCheckForRoutedTraffic [GT_FALSE],
                                                                       enableIpv4SipFilterForRoutedTraffic [GT_TRUE],
                                                                       enableIpv4SipLookupForAccessMatrix [GT_FALSE],
                                                                       ipv4DefaultSipAccessLevel [GT_TRUE],
                                                                       enableArpuRpf [GT_FALSE],
                                                                       enableArpSipSaCheck [GT_TRUE],
                                                                       enableArpSipFilter [GT_FALSE]}.
    Expected: GT_OK.
    1.3. Try to write entry with index out of range.
         Call cpssExMxPmNstIpProfileTableEntrySet with profileIndex [numEntries] and other params from 1.2.
    Expected: NOT GT_OK.
    1.4. Read all entries in IpProfile table and compare with original.
         Call cpssExMxPmNstIpProfileTableEntryGet with the same profileIndex and non-NULL profilePtr.
    Expected: GT_OK and the same profile.
    1.5. Try to read entry with index out of range.
         Call cpssExMxPmNstIpProfileTableEntryGet with profileIndex [numEntries] and non-NULL profilePtr.
    Expected: NOT GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_U32      numEntries = 0;
    GT_U32      iTemp      = 0;

    CPSS_EXMXPM_NST_IP_PROFILE_STC  profile;
    CPSS_EXMXPM_NST_IP_PROFILE_STC  profileGet;


    cpssOsBzero((GT_VOID*) &profile, sizeof(profile));
    cpssOsBzero((GT_VOID*) &profileGet, sizeof(profileGet));

    /* Fill the entry for IpProfile table */
    profile.enableIpv4uRpfForBridgedTraffic        = GT_FALSE;
    profile.enableIpv4HeaderCheckForBridgedTraffic = GT_TRUE;
    profile.enableIpv4SipSaCheckForBridgedTraffic  = GT_FALSE;
    profile.enableIpv4SipFilterForBridgedTraffic   = GT_TRUE;
    profile.enableIpv4uRpfForRoutedTraffic         = GT_FALSE;

    profile.enableIpv4SipSaCheckForRoutedTraffic   = GT_TRUE;
    profile.enableIpv4SipFilterForRoutedTraffic    = GT_FALSE;
    profile.enableIpv4SipLookupForAccessMatrix     = GT_TRUE;
    profile.ipv4DefaultSipAccessLevel              = GT_FALSE;
    profile.enableArpuRpf                          = GT_TRUE;
    profile.enableArpSipSaCheck                    = GT_FALSE;
    profile.enableArpSipFilter                     = GT_TRUE;
        
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Get table Size */
        numEntries = 0x10;

        /* 1.2. Fill all entries in IpProfile table */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmNstIpProfileTableEntrySet(dev, iTemp, &profile);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmNstIpProfileTableEntrySet: %d, %d", dev, iTemp);
        }

        /* 1.3. Try to write entry with index out of range. */
        st = cpssExMxPmNstIpProfileTableEntrySet(dev, numEntries, &profile);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmNstIpProfileTableEntrySet: %d, %d", dev, iTemp);

        /* 1.4. Read all entries in IpProfile table and compare with original */
        for(iTemp = 0; iTemp < numEntries; ++iTemp)
        {
            st = cpssExMxPmNstIpProfileTableEntryGet(dev, iTemp, &profileGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmNstIpProfileTableEntryGet: %d, %d", dev, iTemp);
    
            /* Verifying values */
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4uRpfForBridgedTraffic, profileGet.enableIpv4uRpfForBridgedTraffic,
                           "get another profilePtr->enableIpv4uRpfForBridgedTraffic than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4HeaderCheckForBridgedTraffic, profileGet.enableIpv4HeaderCheckForBridgedTraffic,
                           "get another profilePtr->enableIpv4HeaderCheckForBridgedTraffic than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipSaCheckForBridgedTraffic, profileGet.enableIpv4SipSaCheckForBridgedTraffic,
                           "get another profilePtr->enableIpv4SipSaCheckForBridgedTraffic than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipFilterForBridgedTraffic, profileGet.enableIpv4SipFilterForBridgedTraffic,
                           "get another profilePtr->enableIpv4SipFilterForBridgedTraffic than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4uRpfForRoutedTraffic, profileGet.enableIpv4uRpfForRoutedTraffic,
                           "get another profilePtr->enableIpv4uRpfForRoutedTraffic than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipSaCheckForRoutedTraffic, profileGet.enableIpv4SipSaCheckForRoutedTraffic,
                           "get another profilePtr->enableIpv4SipSaCheckForRoutedTraffic than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipFilterForRoutedTraffic, profileGet.enableIpv4SipFilterForRoutedTraffic,
                           "get another profilePtr->enableIpv4SipFilterForRoutedTraffic than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableIpv4SipLookupForAccessMatrix, profileGet.enableIpv4SipLookupForAccessMatrix,
                           "get another profilePtr->enableIpv4SipLookupForAccessMatrix than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.ipv4DefaultSipAccessLevel, profileGet.ipv4DefaultSipAccessLevel,
                           "get another profilePtr->ipv4DefaultSipAccessLevel than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableArpuRpf, profileGet.enableArpuRpf,
                           "get another profilePtr->enableArpuRpf than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableArpSipSaCheck, profileGet.enableArpSipSaCheck,
                           "get another profilePtr->enableArpSipSaCheck than was set: %d", dev);
            UTF_VERIFY_EQUAL1_STRING_MAC(profile.enableArpSipFilter, profileGet.enableArpSipFilter,
                           "get another profilePtr->enableArpSipFilter than was set: %d", dev);
        }

        /* 1.5. Try to read entry with index out of range. */
        st = cpssExMxPmNstIpProfileTableEntryGet(dev, numEntries, &profileGet);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxPmNstIpProfileTableEntryGet: %d, %d", dev, iTemp);
    }
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmNstIp suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmNstIp)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstIpProfileTableEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstIpProfileTableEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstIpRouterAccessMatrixCommandSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstIpRouterAccessMatrixCommandGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstIpBridgedPktExceptionsCounterSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstIpBridgedPktExceptionsCounterGet)
    /* Tests for Tables */
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmNstIpFillIpProfileTable)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmNstIp)

