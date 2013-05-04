/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxPmBrgPrvEdgeVlanUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxPmBrgPrvEdgeVlan, that provides
*       cpss ExMxPM implementation for Prestera Private Edge VLANs.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

/* includes */
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgPrvEdgeVlan.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* Default valid value for port id */
#define BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS 0

/* Invalid enum */
#define BRG_EDGE_VLAN_INVALID_ENUM_CNS    0x5AAAAAA5

/* Maximum value for trunk id. */
#define MAX_TRUNK_ID_CNS                  256


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgPrvEdgeVlanEnable
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgPrvEdgeVlanEnable)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.2. Call cpssExMxPmBrgPrvEdgeVlanEnableGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

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
        enable = GT_FALSE;

        st = cpssExMxPmBrgPrvEdgeVlanEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmBrgPrvEdgeVlanEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgPrvEdgeVlanEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgPrvEdgeVlanEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);

        /* Call with enable [GT_TRUE] */
        enable = GT_TRUE;

        st = cpssExMxPmBrgPrvEdgeVlanEnable(dev, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enable);

        /*
            1.2. Call cpssExMxPmBrgPrvEdgeVlanEnableGet with non-NULL enablePtr.
            Expected: GT_OK and the same enable.
        */
        st = cpssExMxPmBrgPrvEdgeVlanEnableGet(dev, &enableGet);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgPrvEdgeVlanEnableGet: %d", dev);

        /* Verifying values */
        UTF_VERIFY_EQUAL1_STRING_MAC(enable, enableGet,
                       "get another enablePtr than was set: %d", dev);
    }

    enable = GT_TRUE;

    /* 2. For not-active devices and devices from non-applicable family */
    /* check that function returns GT_BAD_PARAM.                        */

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* go over all non active devices */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgPrvEdgeVlanEnable(dev, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgPrvEdgeVlanEnable(dev, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgPrvEdgeVlanEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgPrvEdgeVlanEnableGet)
{
/*
    ITERATE_DEVICES (ExMxPm)
    1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    GT_BOOL     enable = GT_FALSE;

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
        st = cpssExMxPmBrgPrvEdgeVlanEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, dev);

        /*
            1.2. Call with null enablePtr [NULL]. 
            Expected: GT_BAD_PTR.
        */
        st = cpssExMxPmBrgPrvEdgeVlanEnableGet(dev, NULL);
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
        st = cpssExMxPmBrgPrvEdgeVlanEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of bound value for device id.*/
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxPmBrgPrvEdgeVlanEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgPrvEdgeVlanPortConfigSet
(
    IN GT_U8                    devNum,
    IN GT_U8                    portNum,
    IN GT_BOOL                  enable,
    IN CPSS_INTERFACE_INFO_STC  *pveIfPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgPrvEdgeVlanPortConfigSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with enable [GT_TRUE]
                     and pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                   devPort {devNum [devNum], portNum [0]}}.
    Expected: GT_OK.
    1.1.2. Call with enable [GT_TRUE]
                     and pveIfPtr {type [CPSS_INTERFACE_TRUNK_E],
                                   trunkId [2]}.
    Expected: GT_OK.
    1.1.3. Call cpssExMxPmBrgPrvEdgeVlanPortConfigGet with non-NULL enablePtr and non-NULL pveIfPtr.
    Expected: GT_OK and the same enable and pveIfPtr.
    1.1.4. Call with pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                               devPort {out of range devNum [PRV_CPSS_MAX_PP_DEVICES_CNS], portNum [0]}}
                     and other parameters the same as in 1.1.1.
    Expected: NOT GT_OK.
    1.1.5. Call with pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                               devPort {devNum [devNum], out of range portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]}}
                     and other parameters the same as in 1.1.1.
    Expected: NOT GT_OK.
    1.1.6. Call with pveIfPtr {type [CPSS_INTERFACE_TRUNK_E],
                               devPort {out of range devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (ignored), portNum [0]},
                               trunkId [2]}
                     and other parameters the same as in 1.1.2.
    Expected: GT_OK.
    1.1.7. Call with pveIfPtr {type [CPSS_INTERFACE_TRUNK_E],
                               devPort {devNum [devNum], out of range portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (ignored)},
                               trunkId [2]}
                     and other parameters the same as in 1.1.2.
    Expected: GT_OK.
    1.1.8. Call with enable [GT_FALSE]
                     and pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                    devPort {out of range devNum [PRV_CPSS_MAX_PP_DEVICES_CNS], portNum [0]}}
                                    (pveIfPtr is ignored).
    Expected: GT_OK.
    1.1.9. Call with enable [GT_FALSE]
                     and pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                   devPort {devNum [devNum], out of range portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]}}
                                   (pveIfPtr is ignored).
    Expected: GT_OK.
    1.1.10. Call with pveIfPtr {type [CPSS_INTERFACE_TRUNK_E],
                                out of range trunkId [MAX_TRUNK_ID_CNS = 256]}
                      and other parameters the same as in 1.1.2.
    Expected: NOT GT_OK.
    1.1.11. Call with pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                devPort {devNum [devNum], portNum [0]},
                                out of range trunkId [MAX_TRUNK_ID_CNS = 256] (ignored)}
                      and other parameters the same as in 1.1.1.
    Expected: GT_OK.
    1.1.12. Call with enable [GT_FALSE]
                      and pveIfPtr {type [CPSS_INTERFACE_TRUNK_E],
                                    out of range trunkId [MAX_TRUNK_ID_CNS = 256]}
                                    (pveIfPtr is ignored).
    Expected: GT_OK.
    1.1.13. Call with pveIfPtr {type [CPSS_INTERFACE_VIDX_E /
                                      CPSS_INTERFACE_VID_E] (not relevant)
                      and other parameters the same as in 1.1.1.
    Expected: NOT GT_OK.
    1.1.14. Call with pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                out of range vidx[PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS = 1024] (not relevant)}
                      and other parameters the same as in 1.1.1.
    Expected: GT_OK.
    1.1.15. Call with pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)}
                      and other parameters the same as in 1.1.1.
    Expected: GT_OK.
    1.1.16. Call with out of range pveIfPtr->type [0x5AAAAAA5]
                      and other parameters the same as in 1.1.1.
    Expected: GT_BAD_PARAM.
    1.1.17. Call with enable [GT_FALSE]
                      and pveIfPtr [NULL] (pveIfPtr is ignored).
    Expected: GT_OK.
    1.1.18. Call with enable [GT_TRUE]
                      and pveIfPtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL                 enable    = GT_FALSE;
    GT_BOOL                 enableGet = GT_FALSE;
    CPSS_INTERFACE_INFO_STC pveIf;
    CPSS_INTERFACE_INFO_STC pveIfGet;


    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with enable [GT_TRUE]
                                 and pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                               devPort {devNum [devNum], portNum [0]}}.
                Expected: GT_OK.
            */
            enable = GT_TRUE;

            pveIf.type = CPSS_INTERFACE_PORT_E;

            pveIf.devPort.devNum  = dev;
            pveIf.devPort.portNum = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.3. Call cpssExMxPmBrgPrvEdgeVlanPortConfigGet with non-NULL enablePtr and non-NULL pveIfPtr.
                Expected: GT_OK and the same enable and pveIfPtr.
            */
            st = cpssExMxPmBrgPrvEdgeVlanPortConfigGet(dev, port, &enableGet, &pveIfGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgPrvEdgeVlanPortConfigGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(pveIf.type, pveIfGet.type,
                       "get another pveIfPtr->type than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(pveIf.devPort.devNum, pveIfGet.devPort.devNum,
                       "get another pveIfPtr->devPort.devNum than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(pveIf.devPort.portNum, pveIfGet.devPort.portNum,
                       "get another pveIfPtr->devPort.portNum than was set: %d, %d", dev, port);
            
            /*
                1.1.2. Call with enable [GT_TRUE]
                                 and pveIfPtr {type [CPSS_INTERFACE_TRUNK_E],
                                               trunkId [2]}.
                Expected: GT_OK.
            */
            enable = GT_TRUE;

            pveIf.type    = CPSS_INTERFACE_TRUNK_E;
            pveIf.trunkId = 2;
            
            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.3. Call cpssExMxPmBrgPrvEdgeVlanPortConfigGet with non-NULL enablePtr and non-NULL pveIfPtr.
                Expected: GT_OK and the same enable and pveIfPtr.
            */
            st = cpssExMxPmBrgPrvEdgeVlanPortConfigGet(dev, port, &enableGet, &pveIfGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgPrvEdgeVlanPortConfigGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(pveIf.type, pveIfGet.type,
                       "get another pveIfPtr->type than was set: %d, %d", dev, port);
            UTF_VERIFY_EQUAL2_STRING_MAC(pveIf.trunkId, pveIfGet.trunkId,
                       "get another pveIfPtr->trunkId than was set: %d, %d", dev, port);

            /*
                1.1.4. Call with pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                           devPort {out of range devNum [PRV_CPSS_MAX_PP_DEVICES_CNS], portNum [0]}}
                                 and other parameters the same as in 1.1.1.
                Expected: NOT GT_OK.
            */
            pveIf.type = CPSS_INTERFACE_PORT_E;

            pveIf.devPort.devNum  = PRV_CPSS_MAX_PP_DEVICES_CNS;
            pveIf.devPort.portNum = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, pveIfPtr->devPort.devNum = %d",
                                             dev, port, enable, pveIf.devPort.devNum);

            pveIf.devPort.devNum = dev;

            /*
                1.1.5. Call with pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                           devPort {devNum [devNum], out of range portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]}}
                                 and other parameters the same as in 1.1.1.
                Expected: NOT GT_OK.
            */
            pveIf.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, pveIfPtr->devPort.portNum = %d",
                                             dev, port, enable, pveIf.devPort.portNum);

            pveIf.devPort.portNum = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

            /*
                1.1.6. Call with pveIfPtr {type [CPSS_INTERFACE_TRUNK_E],
                                           devPort {out of range devNum [PRV_CPSS_MAX_PP_DEVICES_CNS] (ignored), portNum [0]},
                                           trunkId [2]}
                                 and other parameters the same as in 1.1.2.
                Expected: GT_OK.
            */
            pveIf.type = CPSS_INTERFACE_TRUNK_E;

            pveIf.devPort.devNum = PRV_CPSS_MAX_PP_DEVICES_CNS;
            
            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, pveIfPtr->type = %d, pveIfPtr->devPort.devNum = %d",
                                         dev, port, pveIf.type, pveIf.devPort.devNum);

            pveIf.devPort.devNum = dev;

            /*
                1.1.7. Call with pveIfPtr {type [CPSS_INTERFACE_TRUNK_E],
                                           devPort {devNum [devNum], out of range portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS] (ignored)},
                                           trunkId [2]}
                                 and other parameters the same as in 1.1.2.
                Expected: GT_OK.
            */
            pveIf.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;
            
            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, pveIfPtr->type = %d, pveIfPtr->devPort.portNum = %d",
                                         dev, port, pveIf.type, pveIf.devPort.portNum);

            pveIf.devPort.portNum = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

            /*
                1.1.8. Call with enable [GT_FALSE]
                                 and pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                                devPort {out of range devNum [PRV_CPSS_MAX_PP_DEVICES_CNS], portNum [0]}}
                                                (pveIfPtr is ignored).
                Expected: GT_OK.
            */
            enable = GT_FALSE;

            pveIf.type = CPSS_INTERFACE_PORT_E;

            pveIf.devPort.devNum  = PRV_CPSS_MAX_PP_DEVICES_CNS;
            pveIf.devPort.portNum = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, pveIfPtr->devPort.devNum = %d",
                                         dev, port, enable, pveIf.devPort.devNum);

            pveIf.devPort.devNum = dev;

            /*
                1.1.9. Call with enable [GT_FALSE]
                                 and pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                               devPort {devNum [devNum], out of range portNum [PRV_CPSS_MAX_PP_PORTS_NUM_CNS]}}
                                               (pveIfPtr is ignored).
                Expected: GT_OK.
            */
            pveIf.devPort.portNum = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, pveIfPtr->devPort.portNum = %d",
                                         dev, port, enable, pveIf.devPort.portNum);

            pveIf.devPort.portNum = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

            /*
                1.1.10. Call with pveIfPtr {type [CPSS_INTERFACE_TRUNK_E],
                                            out of range trunkId [MAX_TRUNK_ID_CNS = 256]}
                                  and other parameters the same as in 1.1.2.
                Expected: NOT GT_OK.
            */
            enable = GT_TRUE;

            pveIf.type    = CPSS_INTERFACE_TRUNK_E;
            pveIf.trunkId = MAX_TRUNK_ID_CNS;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, pveIfPtr->trunkId = %d",
                                             dev, port, enable, pveIf.trunkId);

            /*
                1.1.11. Call with pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                            devPort {devNum [devNum], portNum [0]},
                                            out of range trunkId [MAX_TRUNK_ID_CNS = 256] (ignored)}
                                  and other parameters the same as in 1.1.1.
                Expected: GT_OK.
            */
            pveIf.type = CPSS_INTERFACE_PORT_E;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, pveIfPtr->type = %d, pveIfPtr->trunkId = %d",
                                         dev, port, pveIf.type, pveIf.trunkId);

            /*
                1.1.12. Call with enable [GT_FALSE]
                                  and pveIfPtr {type [CPSS_INTERFACE_TRUNK_E],
                                                out of range trunkId [MAX_TRUNK_ID_CNS = 256]}
                                                (pveIfPtr is ignored).
                Expected: GT_OK.
            */
            enable = GT_FALSE;

            pveIf.type = CPSS_INTERFACE_TRUNK_E;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, pveIfPtr->trunkId = %d",
                                         dev, port, enable, pveIf.trunkId);

            pveIf.trunkId = 2;

            /*
                1.1.13. Call with pveIfPtr {type [CPSS_INTERFACE_VIDX_E /
                                                  CPSS_INTERFACE_VID_E] (not relevant)
                                  and other parameters the same as in 1.1.1.
                Expected: NOT GT_OK.
            */
            enable = GT_TRUE;

            /* Call with pveIfPtr->type [CPSS_INTERFACE_VIDX_E] */
            pveIf.type = CPSS_INTERFACE_VIDX_E;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, pveIfPtr->type = %d",
                                             dev, port, enable, pveIf.type);

            /* Call with pveIfPtr->type [CPSS_INTERFACE_VID_E] */
            pveIf.type = CPSS_INTERFACE_VID_E;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, pveIfPtr->type = %d",
                                             dev, port, enable, pveIf.type);

            /*
                1.1.14. Call with pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                            out of range vidx[PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS = 1024] (not relevant)}
                                  and other parameters the same as in 1.1.1.
                Expected: GT_OK.
            */
            pveIf.type = CPSS_INTERFACE_PORT_E;

            pveIf.vidx = PRV_CPSS_EXMXPM_VIDX_MAX_NUM_CNS;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, pveIfPtr->vidx = %d",
                                         dev, port, enable, pveIf.vidx);

            pveIf.vidx = 0;

            /*
                1.1.15. Call with pveIfPtr {type [CPSS_INTERFACE_PORT_E],
                                            out of range vlanId [PRV_CPSS_MAX_NUM_VLANS_CNS] (not relevant)}
                                  and other parameters the same as in 1.1.1.
                Expected: GT_OK.
            */
            pveIf.vlanId = PRV_CPSS_MAX_NUM_VLANS_CNS;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "%d, %d, %d, pveIfPtr->vlanId = %d",
                                         dev, port, enable, pveIf.vlanId);

            pveIf.vlanId = 100;

            /*
                1.1.16. Call with out of range pveIfPtr->type [0x5AAAAAA5]
                                  and other parameters the same as in 1.1.1.
                Expected: GT_BAD_PARAM.
            */
            pveIf.type = BRG_EDGE_VLAN_INVALID_ENUM_CNS;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, %d, pveIfPtr->type = %d",
                                         dev, port, enable, pveIf.type);

            pveIf.type = CPSS_INTERFACE_PORT_E;

            /*
                1.1.17. Call with enable [GT_FALSE]
                                  and pveIfPtr [NULL] (pveIfPtr is ignored).
                Expected: GT_OK.
            */
            enable = GT_FALSE;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, pveIfPtr = NULL",
                                         dev, port, enable);

            /*
                1.1.18. Call with enable [GT_TRUE]
                                  and pveIfPtr [NULL].
                Expected: GT_BAD_PTR.
            */
            enable = GT_TRUE;

            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, NULL);
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_BAD_PTR, st, "%d, %d, %d, pveIfPtr = NULL",
                                         dev, port, enable);
        }

        enable = GT_TRUE;

        pveIf.type = CPSS_INTERFACE_PORT_E;

        pveIf.devPort.devNum  = dev;
        pveIf.devPort.portNum = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    pveIf.type = CPSS_INTERFACE_PORT_E;

    pveIf.devPort.devNum  = dev;
    pveIf.devPort.portNum = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmBrgPrvEdgeVlanPortConfigSet(dev, port, enable, &pveIf);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgPrvEdgeVlanPortConfigGet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    OUT GT_BOOL                 *enablePtr,
    OUT CPSS_INTERFACE_INFO_STC *pveIfPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgPrvEdgeVlanPortConfigGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with non-NULL enablePtr
                     and non-NULL pveIfPtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL]
                     and other parameters the same as in 1.1.1.
    Expected: GT_BAD_PTR.
    1.1.3. Call with pveIfPtr [NULL]
                     and other parameters the same as in 1.1.1.
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL                 enable = GT_FALSE;
    CPSS_INTERFACE_INFO_STC pveIf;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with non-NULL enablePtr
                                 and non-NULL pveIfPtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgPrvEdgeVlanPortConfigGet(dev, port, &enable, &pveIf);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL]
                                 and other parameters the same as in 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgPrvEdgeVlanPortConfigGet(dev, port, NULL, &pveIf);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);

            /*
                1.1.3. Call with pveIfPtr [NULL]
                                 and other parameters the same as in 1.1.1.
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgPrvEdgeVlanPortConfigGet(dev, port, &enable, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, pveIfPtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgPrvEdgeVlanPortConfigGet(dev, port, &enable, &pveIf);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgPrvEdgeVlanPortConfigGet(dev, port, &enable, &pveIf);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgPrvEdgeVlanPortConfigGet(dev, port, &enable, &pveIf);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgPrvEdgeVlanPortConfigGet(dev, port, &enable, &pveIf);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmBrgPrvEdgeVlanPortConfigGet(dev, port, &enable, &pveIf);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet 
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,   
    IN  GT_BOOL     enable
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call function with enable [GT_TRUE / GT_FALSE].
    Expected: GT_OK.
    1.1.2. Call cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet with non-NULL enablePtr.
    Expected: GT_OK and the same enable.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL     enable    = GT_FALSE;
    GT_BOOL     enableGet = GT_FALSE;

    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with enable [GT_TRUE / GT_FALSE].
                Expected: GT_OK.
            */

            /* Call with enable [GT_FALSE] */
            enable = GT_FALSE;

            st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet with non-NULL enablePtr.
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
            
            /* Call with enable [GT_TRUE] */
            enable = GT_TRUE;

            st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet(dev, port, enable);
            UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, port, enable);

            /*
                1.1.2. Call cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet with non-NULL enablePtr. 
                Expected: GT_OK and the same enable.
            */
            st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet(dev, port, &enableGet);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, 
                       "cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet: %d, %d", dev, port);

            /* Verifying values */
            UTF_VERIFY_EQUAL2_STRING_MAC(enable, enableGet,
                       "get another enable than was set: %d, %d", dev, port);
        }

        enable = GT_TRUE;

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet(dev, port, enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet(dev, port, enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    enable = GT_TRUE;

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet(dev, port, enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet(dev, port, enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet 
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,   
    OUT GT_BOOL     *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet)
{
/*
    ITERATE_DEVICES_VIRT_CPU_PORT (ExMxPm)
    1.1.1. Call with non-NULL enablePtr.
    Expected: GT_OK.
    1.1.2. Call with enablePtr [NULL].
    Expected: GT_BAD_PTR.
*/
    GT_STATUS   st   = GT_OK;
    GT_U8       dev; 
    GT_U8       port = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

    GT_BOOL     enable = GT_FALSE;
    
    /* prepare iterator for go over all active devices */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.1. For all active devices go over all available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE))
        {
            /* 
                1.1.1. Call with non-NULL enablePtr.
                Expected: GT_OK.
            */
            st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);

            /*
                1.1.2. Call with enablePtr [NULL].
                Expected: GT_BAD_PTR.
            */
            st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet(dev, port, NULL);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, enablePtr = NULL", dev, port);
        }

        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

        /* 1.2. For all active devices go over all non available virtual ports. */
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            /* 1.2.1. Call function for each non-active port */
            st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet(dev, port, &enable);
            UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);
        }

        /* 1.3. For active device check that function returns GT_BAD_PARAM */
        /* for out of bound value for port number.                         */
        port = UTF_GET_MAX_VIRT_PORTS_NUM_MAC(dev);

        st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, port);

        /* 1.4. For active device check that function returns GT_OK        */
        /* for CPU port number.                                            */
        port = CPSS_CPU_PORT_NUM_CNS;

        st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet(dev, port, &enable);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, port);
    }

    /* 2. For not active devices check that function returns non GT_OK.*/
    port = BRG_EDGE_VLAN_VALID_VIRT_PORT_CNS;

    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, UTF_EXMXPM_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet(dev, port, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3.Call function with out of bound value for device id */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;
    
    st = cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet(dev, port, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cpssExMxPmBrgPrvEdgeVlan suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxPmBrgPrvEdgeVlan)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgPrvEdgeVlanEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgPrvEdgeVlanEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgPrvEdgeVlanPortConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgPrvEdgeVlanPortConfigGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxPmBrgPrvEdgeVlanPortControlPktsToPveIfGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxPmBrgPrvEdgeVlan)

