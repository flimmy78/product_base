/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxVbUT.c
*
* DESCRIPTION:
*       Unit tests for cpssExMxVb that provide CPSS ExMx VB (Value blade) support.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
/* includes */

#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/generic/cos/cpssCosTypes.h>

#include <cpss/exMx/exMxGen/vb/cpssExMxVb.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

/* defines */

/* (in ExMx device) max number of HW physical ports supported for VB */
/* From cpssExMxVb.c */
#define MAX_PHY_PORTS_NUM_CNS       12

/* max number of HW virtual ports supported for VB */
/* From cpssExMxVb.c */
#define MAX_VIRT_PORTS_NUM_CNS      52

/* mux device max number of ports supported for VB */
/* From cpssExMxVb.c */
#define MUX_MAX_PHY_PORTS_NUM_CNS   BIT_5 /*32*/

/* max number of ports used in VB egress pipe*/
/* From cpssExMxVb.c */
#define MAX_PORTS_NUM_CNS           BIT_6 /*64*/

/* Macro to define is device of 98Ex136 type       */
/* devType  - type of device (CPSS_PP_DEVICE_TYPE) */
#define IS_98EX136_DEV_MAC(devType)                 \
                ((CPSS_98EX136_CNS == devType) ||   \
                 (CPSS_98EX136DI_CNS == devType))

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxVbConfigSet
(
    IN GT_U8    devNum ,
    IN GT_U32   numOfGroups,
    IN CPSS_EXMX_VB_CSCD_GROUP_INFO_STC cscdGroupsArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxVbConfigSet)
{
/*
ITERATE_DEVICES
1.1. Call function with numOfGroups [1] and cscdGroupsArray [muxDevNum = 12, muxHwDevNum = 0, mirrorCscdConnectionIndex = 0, numOfCscdConnections = 1, cscdConnections [exMxCscdPort = 0, numOfVirtualPorts = 1, virtPortsPtr [networkMuxPortNum = 1, virtualPort = 0]]]. Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1.2. Call function with cscdGroupsArray[0]-> cscdConnections[0]->virtPortsPtr[0]->networkMuxPortNum [31] (mirrorCscdConnectionIndex used as default choice for mirrored packets from the mux device to the ExMx device). Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1. 3. For 98EX1x6 devices call function numOfGroups [0], and other parameters the same as in 1.1. Expected: GT_OK.
1.4. For 98EX136 devices in 4XG VB mode call function with cscdGroupsArray[0]->muxHwDevNum = 3, and other parameters the same as in 1.1. Expected: GT_OK.
1.5. For 98EX1x6 devices call function with cscdGroupsArray[0]->muxHwDevNum = 2, and other parameters the same as in 1.1. Expected: GT_OK.
1.6. For 98EX1x6 devices call function with cscdGroupsArray[0]-> mirrorCscdConnectionIndex = 3 (while numOfCscdConnections = 1), and other parameters the same as in 1.1. Expected: NON GT_OK.
1.7. For 98EX1x6 devices call function with numOfCscdConnections = 0, and other parameters the same as in 1.1. Expected: NON GT_OK.
1.8. For 98EX1x6 devices call function with cscdConnectionsPtr = NULL, and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.9. For 98EX1x6 devices call function with non-active for the currect device physical port cscdGroupsArray[0]-> cscdConnectionsPtr[0]->exMxCscdPort, and other parameters the same as in 1.1. Expected: NON GT_OK.
1.10. For 98EX1x6 devices call function with out of range physical port cscdGroupsArray[0]-> cscdConnectionsPtr[0]->exMxCscdPort [MAX_PHY_PORTS_NUM_CNS = 12], and other parameters the same as in 1.1. Expected: NON GT_OK.
1.11. For 98EX1x6 devices call function with cscdGroupsArray[0]->cscdConnectionsPtr[0]->numOfVirtPorts [0], and other parameters the same as in 1.1. Expected: NON GT_OK.
1.12. For 98EX1x6 devices call function with cscdGroupsArray[0]->cscdConnectionsPtr[0]->virtPortsPtr [NULL], and other parameters the same as in 1.1. Expected: GT_BAD_PTR.
1.13. For 98EX1x6 devices call function with out of range mux port num cscdGroupsArray[0]->cscdConnectionsPtr[0]->virtPortsPtr[0]->networkMuxPortNum [32], and other parameters the same as in 1.1. Expected: NON GT_OK.
1.14. For 98EX1x6 devices call function with non active virtual port cscdGroupsArray[0]->cscdConnectionsPtr[0]->virtPortsPtr[0]->virtualPort, and other parameters the same as in 1.1. Expected: NON GT_OK.
1.15. For 98EX1x6 devices call function with out of range virtual port cscdGroupsArray[0]->cscdConnectionsPtr[0]->virtPortsPtr[0]->virtualPort [MAX_VIRT_PORTS_NUM_CNS = 52], and other parameters the same as in 1.1. Expected: NON GT_OK.
1.16. Call function with numOfGroups [1] and cscdGroupsArray [muxDevNum = 12, muxHwDevNum = 0, mirrorCscdConnectionIndex = 0, numOfCscdConnections = 1, cscdConnections [exMxCscdPort = 0, numOfVirtualPorts = 2, virtPortsPtr [[networkMuxPortNum = 1, virtualPort = active_virt_port_for_the_dev_1]; [networkMuxPortNum = 2, virtualPort = active_virt_port_for_the_dev_2]]]]. Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1.17. For 98EX1x6 devices try to map the same mux device port two times - call function with cscdGroupsArray [0]->cscdConnections [0]->virtPortsPtr [[networkMuxPortNum = 1, virtualPort = active_virt_port_for_the_dev_1]; [networkMuxPortNum = 1, virtualPort = active_virtual_port_for_the_dev_2]] and other parameters the same as in 1.16. Expected: NON GT_OK.
1.18. For 98EX1x6 devices try to map the same virtual port two times - call function with cscdGroupsArray [0]->cscdConnections [0]->virtPortsPtr [[networkMuxPortNum = 1, virtualPort = 0]; [networkMuxPortNum = 2, virtualPort = 0]] and other parameters the same as in 1.16. Expected: NON GT_OK.
1.19. For device with numOfPorts >= 2 and numOf VirtPorts >= 2 call function with numOfGroups [1] and cscdGroupsArray [muxDevNum = 12, muxHwDevNum = 0, mirrorCscdConnectionIndex = 0, numOfCscdConnections = 2, cscdConnections[0]= [exMxCscdPort = active_phy_port_for_dev_1, numOfVirtualPorts = 1, virtPortsPtr [networkMuxPortNum = 1, virtualPort = active_virt_port_for_dev_1]], cscdConnections[1]= [exMxCscdPort = active_phy_port_for_dev_2, numOfVirtualPorts = 1, virtPortsPtr [networkMuxPortNum = 2, virtualPort = active_virt_port_for_dev_2]]]. Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1.20. For 98EX1x6 devices with numOfPorts >= 2 and numOf VirtPorts >= 2 try to map the same mux device port two times for different cscdConnections of one group - call function with cscdGroupsArray [0]->cscdConnections[0]->virtPortsPtr [networkMuxPortNum = 1, virtualPort = active_virt_port_for_the_dev_1]]; cscdGroupsArray [0]->cscdConnections [1]->virtPortsPtr [networkMuxPortNum = 1, virtualPort = active_virt_port_for_the_dev_2]] and other parameters the same as in 1.19. Expected: NON GT_OK.
1.21. For 98EX1x6 devices with numOfPorts >= 2 and numOf VirtPorts >= 1 try to map the same virtual port two times for different cscdConnections - call function with cscdGroupsArray [0]->cscdConnections [0]->virtPortsPtr [networkMuxPortNum = 1, virtualPort = 0]]; cscdGroupsArray [0]->cscdConnections [1]->virtPortsPtr [networkMuxPortNum = 2, virtualPort = 0]] and other parameters the same as in 1.19. Expected: NON GT_OK.
1.22.Devices with numOfPorts >= 1 and numOfVirtPorts >= 2 try to map the same physical port two times for different cscdConnections - call function with cscdGroupsArray [0]->cscdConnections [0]->exMxCscdPort = active_phy_port_for_the_dev_1; cscdGroupsArray [0]->cscdConnections [1]->exMxCscdPort = active_phy_port_for_the_dev_1; and other parameters the same as in 1.19. Expected: GT_OK for 98EX1x6 devices (can map different virt ports to one phy port) and NON GT_OK otherwise.
1.23. For device with numOfPorts >= 5 and numOfVirtPorts >= 5 call function with numOfGroups [5] and cscdGroupsArray[ii] = [muxDevNum = ii, muxHwDevNum = 0, mirrorCscdConnectionIndex = 0, numOfCscdConnections = 1, cscdConnections [exMxCscdPort = active_phy_port_for_dev_ii, numOfVirtualPorts = 1, virtPortsPtr [networkMuxPortNum = (ii+1), virtualPort = active_virt_port_for_dev_ii]]], where ii = [0,1, 2, 3, 4]. Expected: GT_OK for 98EX1x6 devices, and NON GT_OK otherwise.
1.24. For 98EX1x6 devices with numOfPorts >= 5 and numOfVirtPorts >= 5 check for out of range mux device port number while numOfGroups > 4 - call function with cscdGroupsArrayi0]->cscdConnections[0]->virtPortsPtr [networkMuxPortNum = 16, virtualPort = active_virt_port_for_dev_0]]] and other parameters the same as in 1.23. Expected: NON GT_OK.
1.25. For 98EX1x6 devices with numOfPorts >= 4 and numOfVirtPorts >= 5 try to map the same physical port two times for different Groups - call function with cscdGroupsArray[0]-> cscdConnections[0]->exMxCscdPort = active_phy_port_for_dev_0, cscdGroupsArray[1]-> cscdConnections[0]->exMxCscdPort = active_phy_port_for_dev_0 and other parameters the same as in 1.23. Expected: NON GT_OK, but only for 98EX136 devices in 4XG VB mode - GT_OK (because for them Group will be the same since muxHwDevNum is the same [0]).
1.26. For 98EX1x6 devices with numOfPorts >= 5 and numOfVirtPorts >= 4 try to map the same virtual port two times for different Groups - call function with cscdGroupsArray[0]-> cscdConnections[0]->virtPorts[0]->virtualPort = active_virt_port_for_dev_0, cscdGroupsArray[1]-> cscdConnections[0]-> ->virtPorts[0]->virtualPort = active_virt_port_for_dev_0 and other parameters the same as in 1.23. Expected: NON GT_OK.
1.27. For 98EX1x6 devices with numOfPorts >= 5 and numOfVirtPorts >= 5 try to map the same mux port two times for different Groups - call function with cscdGroupsArray[0]-> cscdConnections[0]->virtPorts[0]->networkMuxDevPortNum = 1, cscdGroupsArray[1]-> cscdConnections[0]-> ->virtPorts[0]-> networkMuxDevPortNum = 1 and other parameters the same as in 1.23. Expected: GT_OK, but only for 98EX136 devices in 4XG VB mode - NON GT_OK (because for them Group will be the same since muxHwDevNum is the same [0]).
1.28. For device with numOfPorts >= 8 and numOfVirtPorts >= 8 call function with numOfGroups [8] and cscdGroupsArray[ii] = [muxDevNum = ii, muxHwDevNum = 0, mirrorCscdConnectionIndex = 0, numOfCscdConnections = 1, cscdConnections [exMxCscdPort = active_phy_port_for_dev_ii, numOfVirtualPorts = 1, virtPortsPtr [networkMuxPortNum = (ii+1), virtualPort = active_virt_port_for_dev_ii]]], where ii = [0 - 7]. Expected: GT_OK for 98EX1x6 devices, and NON GT_OK otherwise.
1.29. For 98Ex1x6 device with numOfPorts >= 9 and numOfVirtPorts >= 9 call function with numOfGroups [9] and cscdGroupsArray[ii] = [muxDevNum = ii, muxHwDevNum = 0, mirrorCscdConnectionIndex = 0, numOfCscdConnections = 1, cscdConnections [exMxCscdPort = active_phy_port_for_dev_ii, numOfVirtualPorts = 1, virtPortsPtr [networkMuxPortNum = (ii+1), virtualPort = active_virt_port_for_dev_ii]]], where ii = [0 - 8]. Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                                           dev;
    GT_U32                                          numOfGroups;

    CPSS_EXMX_VB_CSCD_GROUP_INFO_STC                cscdGroupsArray[9];
    CPSS_EXMX_VB_CSCD_CONNECTION_STC                cscdConnections[9];
    CPSS_EXMX_VB_VIRTUAL_PORT_TO_MUX_PORT_MAP_STC   virtPorts[9];

    CPSS_PP_DEVICE_TYPE      devType;

    GT_U8                    numOfActivePhyPorts = 0;
    GT_U8                    numOfActiveVirtPorts = 0;
    GT_U8                    activePhyPorts[9];
    GT_U8                    nonActivePhyPort = 0xFF;
    GT_U8                    activeVirtPorts[9];
    GT_U8                    nonActiveVirtPort = 0xFF;
    GT_U8                    port;
    GT_U8                    ii;

    CPSS_EXMX_VB_MODE_ENT    vbMode;

    cpssOsBzero((GT_VOID*)cscdGroupsArray, sizeof(cscdGroupsArray));
    cpssOsBzero((GT_VOID*)cscdConnections, sizeof(cscdConnections));
    cpssOsBzero((GT_VOID*)virtPorts, sizeof(virtPorts));
    cpssOsBzero((GT_VOID*)activePhyPorts, sizeof(activePhyPorts));
    cpssOsBzero((GT_VOID*)activeVirtPorts, sizeof(activeVirtPorts));


    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device type */
        st = prvUtfDeviceTypeGet(dev, &devType);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceTypeGet: %d", dev);

        /* get VB mode for device */
        st = cpssExMxVbModeGet(dev, &vbMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "cpssExMxVbModeGet: %d", dev);

        /* get active virtual ports */
        ii = 0;
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while ((GT_OK == prvUtfNextVirtPortGet(&port, GT_TRUE)) && (ii < 9))
        {
            if (port >= MAX_VIRT_PORTS_NUM_CNS)
            {
                break;
            }
            activeVirtPorts[ii] = port;
            ++ii;
        }
        numOfActiveVirtPorts = ii;

        /* get non active virtual ports */
        /* if there is no - will be left value 0xFF - out of range port num */
        st = prvUtfNextVirtPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextVirtPortGet(&port, GT_FALSE))
        {
            nonActiveVirtPort = port;
        }

        /* get active physical ports */
        ii = 0;
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while ((GT_OK == prvUtfNextPhyPortGet(&port, GT_TRUE)) && (ii < 9))
        {
            if (port >= MAX_PHY_PORTS_NUM_CNS)
            {
                break;
            }
            activePhyPorts[ii] = port;
            ++ii;
        }
        numOfActivePhyPorts = ii;

        /* get non active physical ports */
        /* if there is no - will be left value 0xFF - out of range port num */
        st = prvUtfNextPhyPortReset(&port, dev);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
        while (GT_OK == prvUtfNextPhyPortGet(&port, GT_FALSE))
        {
            nonActivePhyPort = port;
        }

        /* if there no any active physical and virtual port - skip this device */
        if ((numOfActivePhyPorts < 1) || (numOfActiveVirtPorts < 1))
        {
            continue;
        }

        /* 1.1. Call function with numOfGroups [1] and cscdGroupsArray
        [muxDevNum = 12, muxHwDevNum = 0, mirrorCscdConnectionIndex = 0,
        numOfCscdConnections = 1, cscdConnections [exMxCscdPort = 0,
        numOfVirtualPorts = 1, virtPortsPtr [networkMuxPortNum = 1, virtualPort = 0]]].
        Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise. */
        numOfGroups = 1;
        cscdGroupsArray[0].muxDevNum = 12;
        cscdGroupsArray[0].muxHwDevNum = 0;
        cscdGroupsArray[0].mirrorCscdConnectionIndex = 0;
        cscdGroupsArray[0].numOfCscdConnections = 1;
        cscdGroupsArray[0].cscdConnectionsPtr = &cscdConnections[0];
        cscdConnections[0].exMxCscdPort = activePhyPorts[0];
        cscdConnections[0].numOfVirtualPorts = 1;
        cscdConnections[0].virtPortsPtr = &virtPorts[0];
        virtPorts[0].networkMuxPortNum = 1;
        virtPorts[0].virtualPort = activeVirtPorts[0];

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, st,
            "Tiger device: %d, muxDevNum= %d, muxHwDevNum = %d, mirrorCscdConnectionIndex = %d, exMxCscdPort = %d, networkMuxPortNum = %d, virtualPort = %d",
            dev, cscdGroupsArray[0].muxDevNum, cscdGroupsArray[0].muxHwDevNum,
            cscdGroupsArray[0].mirrorCscdConnectionIndex,
            cscdConnections[0].exMxCscdPort,
            virtPorts[0].networkMuxPortNum, virtPorts[0].virtualPort);

        /* 1.2. Call function with
        cscdGroupsArray[0]->cscdConnections[0]->virtPortsPtr[0]->networkMuxPortNum [31]
        (mirrorCscdConnectionIndex used as default choice for mirrored packets from
        the mux device to the ExMx device). Expected: GT_OK for 98EX1x6 devices and
        NON GT_OK otherwise. */
        virtPorts[0].networkMuxPortNum = 31;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, st,
            "Tiger device: %d, muxDevNum= %d, muxHwDevNum = %d, mirrorCscdConnectionIndex = %d, exMxCscdPort = %d, networkMuxPortNum = %d, virtualPort = %d",
            dev, cscdGroupsArray[0].muxDevNum, cscdGroupsArray[0].muxHwDevNum,
            cscdGroupsArray[0].mirrorCscdConnectionIndex,
            cscdConnections[0].exMxCscdPort,
            virtPorts[0].networkMuxPortNum, virtPorts[0].virtualPort);

        virtPorts[0].networkMuxPortNum = 1; /* restore to be the same as in 1.1. */

        /* 1.3. For 98EX1x6 devices call function numOfGroups [0],
        and other parameters the same as in 1.1. Expected: GT_OK. */
        numOfGroups = 0;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfGroups = %d",
                                     dev, numOfGroups);
        numOfGroups = 1; /* restore to be the same as in 1.1. */

        /* 1.4. For 98EX136 devices in 4XG VB mode call function with cscdGroupsArray[0]->muxHwDevNum = 3,
        and other parameters the same as in 1.1.
        Expected: GT_OK. */
        cscdGroupsArray[0].muxHwDevNum = 3;

        if ((IS_98EX136_DEV_MAC(devType)) && (CPSS_EXMX_VB_MODE_4XG_E == vbMode))
        {
            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, st,
                "98EX136 device in 4XG VB mode: %d, muxDevNum= %d, muxHwDevNum = %d, mirrorCscdConnectionIndex = %d, exMxCscdPort = %d, networkMuxPortNum = %d, virtualPort = %d",
                dev, cscdGroupsArray[0].muxDevNum, cscdGroupsArray[0].muxHwDevNum,
                cscdGroupsArray[0].mirrorCscdConnectionIndex,
                cscdConnections[0].exMxCscdPort,
                virtPorts[0].networkMuxPortNum, virtPorts[0].virtualPort);
        }

        cscdGroupsArray[0].muxHwDevNum = 0; /* restore to be the same as in 1.1. */

        /* 1.5. For 98EX1x6 devices call function with
        cscdGroupsArray[0]->muxHwDevNum = 2, and other parameters the same as in 1.1.
        Expected: GT_OK. */
        cscdGroupsArray[0].muxHwDevNum = 2;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, muxHwDevNum = %d",
                                     dev, cscdGroupsArray[0].muxHwDevNum);

        cscdGroupsArray[0].muxHwDevNum = 0; /* restore to be the same as in 1.1. */

        /* 1.6. For 98EX1x6 devices call function with
        cscdGroupsArray[0]-> mirrorCscdConnectionIndex = 3
        (while numOfCscdConnections = 1), and other parameters the same as in 1.1.
        Expected: NON GT_OK. */
        cscdGroupsArray[0].mirrorCscdConnectionIndex = 3;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, numOfCscdConnections = %d, mirrorCscdConnectionIndex = %d",
            dev, cscdGroupsArray[0].numOfCscdConnections, cscdGroupsArray[0].mirrorCscdConnectionIndex);

        cscdGroupsArray[0].mirrorCscdConnectionIndex = 0; /* restore to be the same as in 1.1. */

        /* 1.7. For 98EX1x6 devices call function with numOfCscdConnections = 0,
        and other parameters the same as in 1.1. Expected: NON GT_OK. */
        cscdGroupsArray[0].numOfCscdConnections = 0;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfCscdConnections = %d",
                                     dev, cscdGroupsArray[0].numOfCscdConnections);

        cscdGroupsArray[0].numOfCscdConnections = 1; /* restore to be the same as in 1.1. */

        /* 1.8. For 98EX1x6 devices call function with cscdConnectionsPtr = NULL,
        and other parameters the same as in 1.1. Expected: GT_BAD_PTR. */
        cscdGroupsArray[0].cscdConnectionsPtr = NULL;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, cscdConnectionsPtr = NULL", dev);

        cscdGroupsArray[0].cscdConnectionsPtr = &cscdConnections[0]; /* restore to be the same as in 1.1. */

        /* 1.9. For 98EX1x6 devices call function with non-active for the
        currect device physical port
        cscdGroupsArray[0]->cscdConnectionsPtr[0]->exMxCscdPort,
        and other parameters the same as in 1.1. Expected: NON GT_OK. */
        cscdConnections[0].exMxCscdPort = nonActivePhyPort;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, exMxCscdPort = %d",
                                         dev, cscdConnections[0].exMxCscdPort);

        /* restore to be the same as in 1.1. */
        cscdConnections[0].exMxCscdPort = activePhyPorts[0]; 

        /* 1.10. For 98EX1x6 devices call function with out of range physical
        port cscdGroupsArray[0]-> cscdConnectionsPtr[0]->exMxCscdPort
        [MAX_PHY_PORTS_NUM_CNS = 12], and other parameters the same as in 1.1.
        Expected: NON GT_OK. */
        cscdConnections[0].exMxCscdPort = MAX_PHY_PORTS_NUM_CNS;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, exMxCscdPort = %d",
                                         dev, cscdConnections[0].exMxCscdPort);
        /* restore to be the same as in 1.1. */
        cscdConnections[0].exMxCscdPort = activePhyPorts[0]; 

        /* 1.11. For 98EX1x6 devices call function with
        cscdGroupsArray[0]->cscdConnectionsPtr[0]->numOfVirtPorts [0],
        and other parameters the same as in 1.1. Expected: NON GT_OK. */
        cscdConnections[0].numOfVirtualPorts = 0;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, numOfVirtualPorts = %d",
                                         dev, cscdConnections[0].numOfVirtualPorts);
        /* restore to be the same as in 1.1. */
        cscdConnections[0].numOfVirtualPorts = 1;

        /* 1.12. For 98EX1x6 devices call function with
        cscdGroupsArray[0]->cscdConnectionsPtr[0]->virtPortsPtr [NULL],
        and other parameters the same as in 1.1. Expected: GT_BAD_PTR. */
        cscdConnections[0].virtPortsPtr = NULL;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "%d, virtPortsPtr = NULL", dev);

        /* restore to be the same as in 1.1. */
        cscdConnections[0].virtPortsPtr = &virtPorts[0];

        /* 1.13. For 98EX1x6 devices call function with out of range mux port num
        cscdGroupsArray[0]->cscdConnectionsPtr[0]->virtPortsPtr[0]->networkMuxPortNum [32],
        and other parameters the same as in 1.1. Expected: NON GT_OK. */
        virtPorts[0].networkMuxPortNum = 32;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, networkMuxPortNum = %d",
                                         dev, virtPorts[0].networkMuxPortNum);
        /* restore to be the same as in 1.1. */
        virtPorts[0].networkMuxPortNum = 1;

        /* 1.14. For 98EX1x6 devices call function with non active virtual port
        cscdGroupsArray[0]->cscdConnectionsPtr[0]->virtPortsPtr[0]->virtualPort,
        and other parameters the same as in 1.1. Expected: NON GT_OK. */
        virtPorts[0].virtualPort = nonActiveVirtPort;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, virtualPort = %d",
                                         dev, virtPorts[0].virtualPort);
        /* restore to be the same as in 1.1. */
        virtPorts[0].virtualPort = activeVirtPorts[0];

        /* 1.15. For 98EX1x6 devices call function with out of range virtual port
        cscdGroupsArray[0]->cscdConnectionsPtr[0]->virtPortsPtr[0]->virtualPort
        [MAX_VIRT_PORTS_NUM_CNS = 52], and other parameters the same as in 1.1.
        Expected: NON GT_OK. */
        virtPorts[0].virtualPort = MAX_VIRT_PORTS_NUM_CNS;

        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, virtualPort = %d",
                                         dev, virtPorts[0].virtualPort);
        /* restore to be the same as in 1.1. */
        virtPorts[0].virtualPort = activeVirtPorts[0];

        if (numOfActiveVirtPorts >= 2)
        {
            /* 1.16. Call function with numOfGroups [1] and cscdGroupsArray
            [muxDevNum = 12, muxHwDevNum = 0, mirrorCscdConnectionIndex = 0,
            numOfCscdConnections = 1, cscdConnections [exMxCscdPort = 0, numOfVirtualPorts = 2,
            virtPortsPtr [[networkMuxPortNum = 1, virtualPort = active_virt_port_for_the_dev_1];
            [networkMuxPortNum = 2, virtualPort = active_virt_port_for_the_dev_2]]]].
            Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise. */
            numOfGroups = 1;
            cscdGroupsArray[0].muxDevNum = 12;
            cscdGroupsArray[0].muxHwDevNum = 0;
            cscdGroupsArray[0].mirrorCscdConnectionIndex = 0;
            cscdGroupsArray[0].numOfCscdConnections = 1;
            cscdGroupsArray[0].cscdConnectionsPtr = &cscdConnections[0];

            cscdConnections[0].exMxCscdPort = activePhyPorts[0];
            cscdConnections[0].numOfVirtualPorts = 2;
            cscdConnections[0].virtPortsPtr = &virtPorts[0];

            virtPorts[0].networkMuxPortNum = 1;
            virtPorts[0].virtualPort = activeVirtPorts[0];
            virtPorts[1].networkMuxPortNum = 2;
            virtPorts[1].virtualPort = activeVirtPorts[1];

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, st,
                "Tiger device: %d, muxHwDevNum = %d, exMxCscdPort = %d, virtPorts[0].networkMuxPortNum = %d, virtPorts[0].virtualPort = %d, virtPorts[1].networkMuxPortNum = %d, virtPorts[1].virtualPort = %d",
                dev, cscdGroupsArray[0].muxHwDevNum,
                cscdConnections[0].exMxCscdPort,
                virtPorts[0].networkMuxPortNum, virtPorts[0].virtualPort,
                virtPorts[1].networkMuxPortNum, virtPorts[1].virtualPort);

            /* 1.17. For 98EX1x6 devices try to map the same mux device port two times
            - call function with cscdGroupsArray [0]->cscdConnections [0]->virtPortsPtr
            [[networkMuxPortNum = 1, virtualPort = active_virt_port_for_the_dev_1];
            [networkMuxPortNum = 1, virtualPort = active_virtual_port_for_the_dev_2]]
            and other parameters the same as in 1.16. Expected: NON GT_OK. */
            virtPorts[1].networkMuxPortNum = virtPorts[0].networkMuxPortNum;

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "%d, virtPorts[0].networkMuxPortNum == virtPorts[1].networkMuxPortNum = %d",
                dev, virtPorts[1].networkMuxPortNum);

            virtPorts[1].networkMuxPortNum = 2;

            /* 1.18. For 98EX1x6 devices try to map the same virtual port two times
            - call function with cscdGroupsArray [0]->cscdConnections [0]->virtPortsPtr
            [[networkMuxPortNum = 1, virtualPort = 0];
            [networkMuxPortNum = 2, virtualPort = 0]] and other parameters
            the same as in 1.16. Expected: NON GT_OK. */
            virtPorts[1].virtualPort = virtPorts[0].virtualPort;

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "%d, virtPorts[0].virtualPort == virtPorts[1].virtualPort = %d",
                dev, virtPorts[1].virtualPort);

            virtPorts[1].virtualPort = activeVirtPorts[1];
        } /* if (numOfActiveVirtPorts >= 2) */

        if ((numOfActivePhyPorts >=2) && (numOfActiveVirtPorts >=2))
        {
            /* 1.19. For device with numOfPorts >= 2 and numOfVirtPorts >= 2
            call function with numOfGroups [1] and cscdGroupsArray
            [muxDevNum = 12, muxHwDevNum = 0, mirrorCscdConnectionIndex = 0,
            numOfCscdConnections = 2, cscdConnections[0]=
            [exMxCscdPort = active_phy_port_for_dev_1, numOfVirtualPorts = 1,
            virtPortsPtr [networkMuxPortNum = 1, virtualPort = active_virt_port_for_dev_1]],
            cscdConnections[1]= [exMxCscdPort = active_phy_port_for_dev_2,
            numOfVirtualPorts = 1,
            virtPortsPtr [networkMuxPortNum = 2,virtualPort = active_virt_port_for_dev_2]]].
            Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise. */
            numOfGroups = 1;
            cscdGroupsArray[0].muxDevNum = 12;
            cscdGroupsArray[0].muxHwDevNum = 0;
            cscdGroupsArray[0].mirrorCscdConnectionIndex = 0;
            cscdGroupsArray[0].numOfCscdConnections = 2;
            cscdGroupsArray[0].cscdConnectionsPtr = &cscdConnections[0];

            cscdConnections[0].exMxCscdPort = activePhyPorts[0];
            cscdConnections[0].numOfVirtualPorts = 1;
            cscdConnections[0].virtPortsPtr = &virtPorts[0];
            virtPorts[0].networkMuxPortNum = 1;
            virtPorts[0].virtualPort = activeVirtPorts[0];

            cscdConnections[1].exMxCscdPort = activePhyPorts[1];
            cscdConnections[1].numOfVirtualPorts = 1;
            cscdConnections[1].virtPortsPtr = &virtPorts[1];
            virtPorts[1].networkMuxPortNum = 2;
            virtPorts[1].virtualPort = activeVirtPorts[1];

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, st,
                "Tiger device: %d,  cscdConnections[0]: exMxCscdPort = %d, networkMuxPortNum = %d, virtualPort = %d, cscdConnections[1]: exMxCscdPort = %d, networkMuxPortNum = %d, virtualPort = %d",
                dev, cscdConnections[0].exMxCscdPort,
                virtPorts[0].networkMuxPortNum, virtPorts[0].virtualPort,
                cscdConnections[1].exMxCscdPort,
                virtPorts[1].networkMuxPortNum, virtPorts[1].virtualPort);

            /* 1.20. For 98EX1x6 devices with numOfPorts >= 2 and
            numOfVirtPorts >= 2 try to map the same mux device port two times for
            different cscdConnections of one group - call function with
            cscdGroupsArray [0]->cscdConnections[0]->virtPortsPtr [networkMuxPortNum = 1,
            virtualPort = active_virt_port_for_the_dev_1]];
            cscdGroupsArray [0]->cscdConnections [1]->virtPortsPtr
            [networkMuxPortNum = 1, virtualPort = active_virt_port_for_the_dev_2]]
            and other parameters the same as in 1.19. Expected: NON GT_OK. */
            virtPorts[1].networkMuxPortNum = virtPorts[0].networkMuxPortNum;

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "%d, cscdConnections[0]-> networkMuxPortNum == cscdConnections[1]-> networkMuxPortNum = %d",
                dev, virtPorts[1].networkMuxPortNum);

            virtPorts[1].networkMuxPortNum = 2;

            /* 1.21. For 98EX1x6 devices with numOfPorts >= 2 and numOfVirtPorts >= 1
            try to map the same virtual port two times for different cscdConnections
            - call function with cscdGroupsArray [0]->cscdConnections [0]->virtPortsPtr
            [networkMuxPortNum = 1, virtualPort = 0]];
            cscdGroupsArray [0]->cscdConnections [1]->virtPortsPtr
            [networkMuxPortNum = 2, virtualPort = 0]] and other parameters
            the same as in 1.19. Expected: NON GT_OK.*/
            virtPorts[1].virtualPort = virtPorts[0].virtualPort;

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "%d, cscdConnections[0]-> virtualPort == cscdConnections[1]-> virtualPort = %d",
                dev, virtPorts[1].virtualPort);

            virtPorts[1].virtualPort = activeVirtPorts[1];

            /* 1.22. Devices with numOfPorts >= 1 and numOfVirtPorts >= 2
            try to map the same physical port two times for different cscdConnections
            - call function with
            cscdGroupsArray [0]->cscdConnections [0]->exMxCscdPort = active_phy_port_for_the_dev_1;
            cscdGroupsArray [0]->cscdConnections [1]->exMxCscdPort = active_phy_port_for_the_dev_1;
            and other parameters the same as in 1.19. Expected: GT_OK for 98EX1x6 devices
            (can map different virt ports to one phy port) and NON GT_OK otherwise. */
            cscdConnections[1].exMxCscdPort = cscdConnections[0].exMxCscdPort;

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "%d, cscdConnections[0].exMxCscdPort == cscdConnections[1].exMxCscdPort = %d",
                dev,  cscdConnections[1].exMxCscdPort);

            cscdConnections[1].exMxCscdPort = activePhyPorts[1];

        } /* if ((numOfActivePhyPorts >=2) && (numOfActiveVirtPorts >=2)) */

        if ((numOfActivePhyPorts >=5) && (numOfActiveVirtPorts >=5))
        {
            /* 1.23. For device with numOfPorts >= 5 and numOfVirtPorts >= 5 call function
            with numOfGroups [5] and cscdGroupsArray[ii] = [muxDevNum = ii,
            muxHwDevNum = 0, mirrorCscdConnectionIndex = 0, numOfCscdConnections = 1,
            cscdConnections [exMxCscdPort = active_phy_port_for_dev_ii,
            numOfVirtualPorts = 1, virtPortsPtr [networkMuxPortNum = ii +1,
            virtualPort = active_virt_port_for_dev_ii]]],
            where ii = [0,1, 2, 3, 4]. Expected: GT_OK for 98EX1x6 devices,
            and NON GT_OK otherwise. */
            numOfGroups = 5;
            for (ii = 0; ii < numOfGroups; ++ii)
            {
                cscdGroupsArray[ii].muxDevNum = ii;
                cscdGroupsArray[ii].muxHwDevNum = 0;
                cscdGroupsArray[ii].mirrorCscdConnectionIndex = 0;
                cscdGroupsArray[ii].numOfCscdConnections = 1;
                cscdGroupsArray[ii].cscdConnectionsPtr = &cscdConnections[ii];

                cscdConnections[ii].exMxCscdPort = activePhyPorts[ii];
                cscdConnections[ii].numOfVirtualPorts = 1;
                cscdConnections[ii].virtPortsPtr = &virtPorts[ii];

                virtPorts[ii].networkMuxPortNum = ii + 1;
                virtPorts[ii].virtualPort = activeVirtPorts[ii];
            }

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "Tiger device: %d,  numOfGroups = %d", dev, numOfGroups);

            /* 1.24. For 98EX1x6 devices with numOfPorts >= 5 and numOfVirtPorts >= 5
            check for out of range mux device port number while numOfGroups > 4 -
            call function with cscdGroupsArrayi0]->cscdConnections[0]->virtPortsPtr
            [networkMuxPortNum = 16, virtualPort = active_virt_port_for_dev_0]]]
            and other parameters the same as in 1.27. Expected: NON GT_OK. */
            virtPorts[0].networkMuxPortNum = 16;

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);

            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "%d, numOfGroups = %d, cscdGroupsArray[0]-> networkMuxPortNum = %d", 
                dev, numOfGroups, virtPorts[0].networkMuxPortNum);

            virtPorts[0].networkMuxPortNum = 1; /* restore */

            /* 1.25. For 98EX1x6 devices with numOfPorts >= 4 and numOfVirtPorts >= 5
            try to map the same physical port two times for different Groups -
            call function with
            cscdGroupsArray[0]-> cscdConnections[0]->exMxCscdPort = active_phy_port_for_dev_0,
            cscdGroupsArray[1]-> cscdConnections[0]->exMxCscdPort = active_phy_port_for_dev_0
            and other parameters the same as in 1.23. Expected: NON GT_OK,
            but only for 98EX136 devices in 4XG VB mode - GT_OK (because for them Group will
            be the same since muxHwDevNum is the same [0]). */
            cscdConnections[1].exMxCscdPort = cscdConnections[0].exMxCscdPort;

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            if ((IS_98EX136_DEV_MAC(devType)) && (CPSS_EXMX_VB_MODE_4XG_E == vbMode))
            {
                UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                    "98EX136 device in 4XG VB mode: %d, cscdGroupsArray[0].muxHwDevNum = %d, cscdGroupsArray[1].muxHwDevNum = %d, cscdGroupsArray[0]-> exMxCscdPort == cscdGroupsArray[1]-> exMxCscdPort = %d",
                    dev, cscdGroupsArray[0].muxHwDevNum, cscdGroupsArray[1].muxHwDevNum, cscdConnections[1].exMxCscdPort);
            }
            else
            {
                UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                    "non 98EX136 device in 4XG VB mode: %d, cscdGroupsArray[0]-> exMxCscdPort == cscdGroupsArray[1]-> exMxCscdPort = %d",
                    dev,  cscdConnections[1].exMxCscdPort);
            }

            cscdConnections[1].exMxCscdPort = activePhyPorts[1];

            /* 1.26. For 98EX1x6 devices with numOfPorts >= 5 and numOfVirtPorts >= 4
            try to map the same virtual port two times for different Groups -
            call function with
            cscdGroupsArray[0]-> cscdConnections[0]->virtPorts[0]->virtualPort =
            active_virt_port_for_dev_0,
            cscdGroupsArray[1]-> cscdConnections[0]-> ->virtPorts[0]->virtualPort =
            active_virt_port_for_dev_0 and other parameters the same as in 1.23.
            Expected: NON GT_OK. */
            virtPorts[1].virtualPort = virtPorts[0].virtualPort;

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "%d, cscdGroupsArray[0]-> virtualPort == cscdGroupsArray[1]-> virtualPort = %d",
                dev,  virtPorts[1].virtualPort);

            virtPorts[1].virtualPort = activeVirtPorts[1];

            /* 1.27. For 98EX1x6 devices with numOfPorts >= 5 and numOfVirtPorts >= 5
            try to map the same mux port two times for different Groups -
            call function with
            cscdGroupsArray[0]-> cscdConnections[0]->virtPorts[0]->networkMuxDevPortNum = 1,
            cscdGroupsArray[1]-> cscdConnections[0]-> ->virtPorts[0]-> networkMuxDevPortNum = 1
            and other parameters the same as in 1.23. Expected: GT_OK,
            but only for 98EX136 devices in 4XG VB mode - NON GT_OK (because for them Group
            will be the same since muxHwDevNum is the same [0]). */
            virtPorts[1].networkMuxPortNum = virtPorts[0].networkMuxPortNum;

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            if ((IS_98EX136_DEV_MAC(devType)) && (CPSS_EXMX_VB_MODE_4XG_E == vbMode))
            {
                UTF_VERIFY_NOT_EQUAL4_STRING_MAC(GT_OK, st,
                    "98EX136 device in 4XG VB mode: %d, cscdGroupsArray[0].muxHwDevNum = %d, cscdGroupsArray[1].muxHwDevNum = %d, cscdGroupsArray[0]-> networkMuxPortNum == cscdGroupsArray[1]-> networkMuxPortNum = %d",
                    dev, cscdGroupsArray[0].muxHwDevNum, cscdGroupsArray[1].muxHwDevNum, virtPorts[1].networkMuxPortNum);
            }
            else
            {
                UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                    "non 98EX136 device in 4XG VB mode: %d, cscdGroupsArray[0]-> networkMuxPortNum == cscdGroupsArray[1]-> networkMuxPortNum = %d",
                    dev,  virtPorts[1].networkMuxPortNum);
            }
            virtPorts[1].networkMuxPortNum = 2; /* restore */

        } /* if ((numOfActivePhyPorts >=5) && (numOfActiveVirtPorts >=5)) */

        if ((numOfActivePhyPorts >=8) && (numOfActiveVirtPorts >=8))
        {
            /* 1.28. For device with numOfPorts >= 8 and numOfVirtPorts >= 8
            call function with numOfGroups [8] and cscdGroupsArray[ii] = [muxDevNum = ii,
            muxHwDevNum = 0, mirrorCscdConnectionIndex = 0, numOfCscdConnections = 1,
            cscdConnections [exMxCscdPort = active_phy_port_for_dev_ii, numOfVirtualPorts = 1,
            virtPortsPtr [networkMuxPortNum = ii, virtualPort = active_virt_port_for_dev_ii]]],
            where ii = [0 - 7]. Expected: GT_OK for 98EX1x6 devices, and NON GT_OK otherwise. */
            numOfGroups = 8;
            for (ii = 0; ii < numOfGroups; ++ii)
            {
                cscdGroupsArray[ii].muxDevNum = ii;
                cscdGroupsArray[ii].muxHwDevNum = 0;
                cscdGroupsArray[ii].mirrorCscdConnectionIndex = 0;
                cscdGroupsArray[ii].numOfCscdConnections = 1;
                cscdGroupsArray[ii].cscdConnectionsPtr = &cscdConnections[ii];

                cscdConnections[ii].exMxCscdPort = activePhyPorts[ii];
                cscdConnections[ii].numOfVirtualPorts = 1;
                cscdConnections[ii].virtPortsPtr = &virtPorts[ii];

                virtPorts[ii].networkMuxPortNum = ii + 1;
                virtPorts[ii].virtualPort = activeVirtPorts[ii];
            }

            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st,
                "Tiger device: %d,  numOfGroups = %d", dev, numOfGroups);
        } /* if ((numOfActivePhyPorts >=8) && (numOfActiveVirtPorts >=8)) */


        /* 1.29. For 98Ex1x6 device with numOfPorts >= 9 and numOfVirtPorts >= 9
        call function with numOfGroups [9] and cscdGroupsArray[ii] =
        [muxDevNum = ii, muxHwDevNum = 0, mirrorCscdConnectionIndex = 0,
        numOfCscdConnections = 1, cscdConnections
        [exMxCscdPort = active_phy_port_for_dev_ii, numOfVirtualPorts = 1,
        virtPortsPtr [networkMuxPortNum = (ii+1),
        virtualPort = active_virt_port_for_dev_ii]]], where ii = [0 - 8].
        Expected: NON GT_OK. */
        if ((numOfActivePhyPorts >= 9) && (numOfActiveVirtPorts >= 9))
        {
            numOfGroups = 9;
            for (ii = 0; ii < numOfGroups; ++ii)
            {
                cscdGroupsArray[ii].muxDevNum = ii;
                cscdGroupsArray[ii].muxHwDevNum = 0;
                cscdGroupsArray[ii].mirrorCscdConnectionIndex = 0;
                cscdGroupsArray[ii].numOfCscdConnections = 1;
                cscdGroupsArray[ii].cscdConnectionsPtr = &cscdConnections[ii];

                cscdConnections[ii].exMxCscdPort = activePhyPorts[ii];
                cscdConnections[ii].numOfVirtualPorts = 1;
                cscdConnections[ii].virtPortsPtr = &virtPorts[ii];

                virtPorts[ii].networkMuxPortNum = ii + 1;
                virtPorts[ii].virtualPort = activeVirtPorts[ii];
            }
            st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
            UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, numOfGroups);
        } /* if ((numOfActivePhyPorts >= 9) && (numOfActiveVirtPorts >= 9))*/

        /* 1.30. For 98EX1x6 devices call function with numOfGroups [2] and
        cscdGroupsArray [NULL]. Expected: GT_BAD_PTR. */
        numOfGroups = 2;
        st = cpssExMxVbConfigSet(dev, numOfGroups, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, numOfGroups);

    } /* while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE)) */

    numOfGroups = 1;
    cscdGroupsArray[0].muxDevNum = 12;
    cscdGroupsArray[0].muxHwDevNum = 0;
    cscdGroupsArray[0].mirrorCscdConnectionIndex = 0;
    cscdGroupsArray[0].numOfCscdConnections = 1;
    cscdGroupsArray[0].cscdConnectionsPtr = &cscdConnections[0];
    cscdConnections[0].exMxCscdPort = activePhyPorts[0];
    cscdConnections[0].numOfVirtualPorts = 1;
    cscdConnections[0].virtPortsPtr = &virtPorts[0];
    virtPorts[0].networkMuxPortNum = 1;
    virtPorts[0].virtualPort = activeVirtPorts[0];

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active devices. */

    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxVbConfigSet(dev, numOfGroups, cscdGroupsArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/

/*
GT_STATUS cpssExMxVb8TcTo4TcMapSet
(
    IN GT_U8                    devNum,
    IN GT_U8                    value8Tc,
    IN GT_U8                    value4Tc
)
*/
UTF_TEST_CASE_MAC(cpssExMxVb8TcTo4TcMapSet)
{
/*
ITERATE_DEVICES
1.1. Call Function with value8Tc [3] and value4Tc [1]. Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1.2. For 98EX1x6 device call function with value8Tc [CPSS_TC_RANGE_CNS] and value4Tc [1]. Expected: GT_BAD_PARAM.
1.3. For 98EX1x6 device call function with value8Tc [3] and value4Tc [CPSS_4_TC_RANGE_CNS]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8  dev;
    GT_U8  value8Tc;
    GT_U8  value4Tc;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call Function with value8Tc [3] and value4Tc [1].
        Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise. */
        value8Tc = 3;
        value4Tc = 1;

        st = cpssExMxVb8TcTo4TcMapSet(dev, value8Tc, value4Tc);
        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d",
                                     dev, value8Tc, value4Tc);

        /* 1.2. For 98EX1x6 device call function with
        value8Tc [CPSS_TC_RANGE_CNS] and value4Tc [1].
        Expected: GT_BAD_PARAM. */
        value8Tc = CPSS_TC_RANGE_CNS;
        value4Tc = 1;

        st = cpssExMxVb8TcTo4TcMapSet(dev, value8Tc, value4Tc);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, value8Tc = %d", dev, value8Tc);

        /* 1.3. For 98EX1x6 device call function with value8Tc [3]
        and value4Tc [CPSS_4_TC_RANGE_CNS]. Expected: GT_BAD_PARAM. */
        value4Tc = CPSS_4_TC_RANGE_CNS;
        value8Tc = 3;

        st = cpssExMxVb8TcTo4TcMapSet(dev, value8Tc, value4Tc);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, value4Tc = %d", dev, value4Tc);
    }

    value8Tc = 3;
    value4Tc = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxVb8TcTo4TcMapSet(dev, value8Tc, value4Tc);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxVb8TcTo4TcMapSet(dev, value8Tc, value4Tc);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxVbCscdDsaEnableGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *enablePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxVbCscdDsaEnableGet)
{
/*
ITERATE_DEVICES
1.1. Call function with non-NULL enablePtr. Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1.2. For 98EX1x6 device call function with enablePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8       dev;
    GT_BOOL     enable;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL enablePtr.
        Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise. */

        st = cpssExMxVbCscdDsaEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "Tiger device: %d", dev);

        /* 1.2. For 98EX1x6 device call function with enablePtr [NULL].
        Expected: GT_BAD_PTR. */
        st = cpssExMxVbCscdDsaEnableGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxVbCscdDsaEnableGet(dev, &enable);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxVbCscdDsaEnableGet(dev, &enable);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxVbGroupModeGet
(
    IN GT_U8                devNum,
    OUT CPSS_EXMX_VB_GROUPS_MODE_ENT   *groupModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxVbGroupModeGet)
{
/*
ITERATE_DEVICES
1.1. Call function with non-NULL groupModePtr. Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1.2. For 98EX1x6 device call function with groupModePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                           dev;
    CPSS_EXMX_VB_GROUPS_MODE_ENT    groupMode;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL groupModePtr.
        Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise. */
        st = cpssExMxVbGroupModeGet(dev, &groupMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "Tiger device: %d", dev);

        /* 1.2. For 98EX1x6 device call function with groupModePtr [NULL].
        Expected: GT_BAD_PTR. */
        st = cpssExMxVbGroupModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxVbGroupModeGet(dev, &groupMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxVbGroupModeGet(dev, &groupMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxVbEgressVirtualPortsGet
(
    IN GT_U8    devNum,
    IN CPSS_EXMX_VB_MODE_ENT vbMode,
    IN GT_U8    virtualPortNum,
    OUT GT_U8   *physicalCscdPortNumPtr,
    IN GT_U8    muxHwDevNum,
    OUT GT_U8   *portNumOnMuxDevicePtr,
    OUT GT_BOOL  *addDsaPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxVbEgressVirtualPortsGet)
{
/*
ITERATE_DEVICES
1.1. Call function with vbMode [CPSS_EXMX_VB_MODE_REGULAR_E], virtualPortNum [10] and muxHwDevNum [0] and non-NULL pointers. Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1.2. Call function with vbMode [CPSS_EXMX_VB_MODE_REGULAR_E], virtualPortNum [52] and muxHwDevNum [0] and non-NULL pointers. Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1.3. Call function with vbMode [CPSS_EXMX_VB_MODE_4XG_E], virtualPortNum [10] and muxHwDevNum [0] and non-NULL pointers. Expected: GT_OK for 98EX136 devices and NON GT_OK.
1.4. For 98EX136 device call function with vbMode [CPSS_EXMX_VB_MODE_4XG_E], virtualPortNum [10] and muxHwDevNum [3/ 8] and non-NULL pointers. Expected: GT_OK.
1.5. Check for unsupported muxDevNum for 4XG VB mode. For 98EX136 device call function with vbMode [CPSS_EXMX_VB_MODE_4XG_E], virtualPortNum [10] and muxHwDevNum [1] and non-NULL pointers. Expected: NON GT_OK.
1.6. Check for unsupported muxDevNum for regular VB mode. For 98EX1x6 device call function with vbMode [CPSS_EXMX_VB_MODE_REGULAR_E], virtualPortNum [10] and muxHwDevNum [3/ 10] and non-NULL pointers. Expected: NON GT_OK.
1.7. Check for out of range virtualPortNum. For 98EX1x6 device call function with vbMode [CPSS_EXMX_VB_MODE_REGULAR_E], virtualPortNum [MAX_PORTS_NUM_CNS] and muxHwDevNum [3] and non-NULL pointers. Expected: NON GT_OK.
1.8. For 98EX1x6 device call function with physicalCscdPortNumPtr [NULL] and ather parametrs the same as in 1.1. Expected: GT_BAD_PTR.
1.9. For 98EX1x6 device call function with portNumOnMuxDevicePtr [NULL] and ather parametrs the same as in 1.1. Expected: GT_BAD_PTR.
1.10. For 98EX1x6 device call function with addDsaPtr [NULL] and ather parametrs the same as in 1.1. Expected: GT_BAD_PTR.
1.11. For 98EX1x6 device call function with vbMode [0x5AAAAAA5] and ather parametrs the same as in 1.1. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                   dev;
    CPSS_EXMX_VB_MODE_ENT   vbMode;
    GT_U8                   virtualPortNum;
    GT_U8                   physicalCscdPortNum;
    GT_U8                   muxHwDevNum;
    GT_U8                   portNumOnMuxDevice;
    GT_BOOL                 addDsa;

    CPSS_PP_DEVICE_TYPE      devType;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* get device type */
        st = prvUtfDeviceTypeGet(dev, &devType);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "prvUtfDeviceTypeGet: %d", dev);

        /* 1.1. Call function with vbMode [CPSS_EXMX_VB_MODE_REGULAR_E],
        virtualPortNum [10] and muxHwDevNum [0] and non-NULL pointers.
        Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise. */
        vbMode = CPSS_EXMX_VB_MODE_REGULAR_E;
        virtualPortNum = 10;
        muxHwDevNum = 0;

        st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
             &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);

        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
            "Tiger device: %d, %d, %d, muxHwDevNum = %d",
            dev, vbMode, virtualPortNum, muxHwDevNum);

        /* 1.2. Call function with vbMode [CPSS_EXMX_VB_MODE_REGULAR_E],
        virtualPortNum [52] and muxHwDevNum [0] and non-NULL pointers.
        Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise. */
        vbMode = CPSS_EXMX_VB_MODE_REGULAR_E;
        virtualPortNum = 52;
        muxHwDevNum = 0;

        st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
             &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);

        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
            "Tiger device: %d, %d, %d, muxHwDevNum = %d",
            dev, vbMode, virtualPortNum, muxHwDevNum);

        /* 1.3. Call function with vbMode [CPSS_EXMX_VB_MODE_4XG_E],
        virtualPortNum [10] and muxHwDevNum [0] and non-NULL pointers.
        Expected: GT_OK for 98EX136 devices and NON GT_OK. */
        vbMode = CPSS_EXMX_VB_MODE_4XG_E;
        virtualPortNum = 10;
        muxHwDevNum = 0;

        st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
             &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);

        if (IS_98EX136_DEV_MAC(devType))
        {
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                "98EX136 device: %d, %d, %d, muxHwDevNum = %d",
                dev, vbMode, virtualPortNum, muxHwDevNum);
        }
        else
        {
            UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
                "non-98EX136 device: %d, vbMode = %d", dev, vbMode);
        }

        if (IS_98EX136_DEV_MAC(devType))
        {
            /* 1.4. For 98EX136 device call function with
            vbMode [CPSS_EXMX_VB_MODE_4XG_E], virtualPortNum [10] and
            muxHwDevNum [3/ 8] and non-NULL pointers. Expected: GT_OK. */
            vbMode = CPSS_EXMX_VB_MODE_4XG_E;
            virtualPortNum = 10;

            muxHwDevNum = 3;

            st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
                 &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                "98EX136 device: %d, %d, %d, muxHwDevNum = %d",
                dev, vbMode, virtualPortNum, muxHwDevNum);

            muxHwDevNum = 8;

            st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
                 &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st,
                "98EX136 device: %d, %d, %d, muxHwDevNum = %d",
                dev, vbMode, virtualPortNum, muxHwDevNum);

            /* 1.5. Check for unsupported muxDevNum for 4XG VB mode.
            For 98EX136 device call function with vbMode [CPSS_EXMX_VB_MODE_4XG_E],
            virtualPortNum [10] and muxHwDevNum [1] and non-NULL pointers.
            Expected: NON GT_OK. */
            vbMode = CPSS_EXMX_VB_MODE_4XG_E;
            virtualPortNum = 10;
            muxHwDevNum = 1;

            st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
                 &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);
            UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
                "98EX136 device: %d, vbMode = %d, muxHwDevNum = %d", dev, vbMode, muxHwDevNum);
        }

        /* 1.6. Check for unsupported muxDevNum for regular VB mode.
        For 98EX1x6 device call function with vbMode [CPSS_EXMX_VB_MODE_REGULAR_E],
        virtualPortNum [10] and muxHwDevNum [3/ 10] and non-NULL pointers.
        Expected: NON GT_OK. */
        vbMode = CPSS_EXMX_VB_MODE_REGULAR_E;
        virtualPortNum = 10;

        muxHwDevNum = 3;

        st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
             &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, vbMode = %d, muxHwDevNum = %d", dev, vbMode, muxHwDevNum);

        muxHwDevNum = 10;

        st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
             &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);
        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st,
            "%d, vbMode = %d, muxHwDevNum = %d", dev, vbMode, muxHwDevNum);

        /* 1.7. Check for out of range virtualPortNum. For 98EX1x6 device
        call function with vbMode [CPSS_EXMX_VB_MODE_REGULAR_E],
        virtualPortNum [MAX_PORTS_NUM_CNS] and muxHwDevNum [0]
        and non-NULL pointers. Expected: NON GT_OK. */
        vbMode = CPSS_EXMX_VB_MODE_REGULAR_E;
        virtualPortNum = MAX_PORTS_NUM_CNS;
        muxHwDevNum = 0;

        st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
             &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);
        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st,
            "%d, virtualPortNum = %d", dev, virtualPortNum);

        /* 1.8. For 98EX1x6 device call function with
        physicalCscdPortNumPtr [NULL] and ather parametrs the same as in 1.1.
        Expected: GT_BAD_PTR. */
        vbMode = CPSS_EXMX_VB_MODE_REGULAR_E;
        virtualPortNum = 10;
        muxHwDevNum = 0;

        st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
             NULL, muxHwDevNum, &portNumOnMuxDevice, &addDsa);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, physicalCscdPortNumPtr = NULL", dev);

        /* 1.9. For 98EX1x6 device call function with portNumOnMuxDevicePtr [NULL]
        and ather parametrs the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
             &physicalCscdPortNum, muxHwDevNum, NULL, &addDsa);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, portNumOnMuxDevicePtr = NULL", dev);

        /* 1.10. For 98EX1x6 device call function with addDsaPtr [NULL]
        and ather parametrs the same as in 1.1. Expected: GT_BAD_PTR. */

        st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
             &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, addDsaPtr = NULL", dev);

        /* 1.11. For 98EX1x6 device call function with vbMode [0x5AAAAAA5]
        and ather parametrs the same as in 1.1. Expected: GT_BAD_PARAM. */
        vbMode = 0x5AAAAAA5;

        st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
             &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, vbMode = %d",
                                     dev, vbMode);
    }

    vbMode = CPSS_EXMX_VB_MODE_REGULAR_E;
    virtualPortNum = 10;
    muxHwDevNum = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
             &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxVbEgressVirtualPortsGet(dev, vbMode, virtualPortNum,
         &physicalCscdPortNum, muxHwDevNum, &portNumOnMuxDevice, &addDsa);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet
(
    IN GT_U8    devNum,
    IN CPSS_EXMX_VB_GROUPS_MODE_ENT    groupMode,
    IN GT_U32   cscdGroupNum,
    IN GT_U8    portNumOnMuxDevice,
    OUT GT_U8   *virtualPortNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet)
{
/*
ITERATE_DEVICES
1.1. Call function with groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E], cscdGroupNum [2], portNumOnMuxDevice [21] and non-NULL virtualPortNumPtr. Expected: GT_OK for 98EX1x6 devices.
1.2. Call function with groupMode [CPSS_EXMX_VB_8_GROUPS_MODE_E], cscdGroupNum [6] portNumOnMuxDevice [5] and non-NULL virtualPortNumPtr. Expected: GT_OK for 98EX1x6 devices.
1.3. Check for invalid cscdGroupNum for current groupMode. For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E], cscdGroupNum [4], portNumOnMuxDevice [21] and non-NULL virtualPortNumPtr. Expected: NON GT_OK.
1.4. Check for invalid cscdGroupNum for current groupMode. For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_8_GROUPS_MODE_E], cscdGroupNum [8], portNumOnMuxDevice [5] and non-NULL virtualPortNumPtr. Expected: NON GT_OK.
1.5. Check for invalid portNumOnMuxDevice for current groupMode. For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E], cscdGroupNum [2], portNumOnMuxDevice [32] and non-NULL virtualPortNumPtr. Expected: NON GT_OK.
1.6. Check for invalid portNumOnMuxDevice for current groupMode. For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_8_GROUPS_MODE_E], cscdGroupNum [6], portNumOnMuxDevice [16] and non-NULL virtualPortNumPtr. Expected: NON GT_OK.
1.7. Check for turbo port processing.Call function with groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E/ CPSS_EXMX_VB_8_GROUPS_MODE_E], cscdGroupNum [0], portNumOnMuxDevice [1] and non-NULL virtualPortNumPtr. Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1.8. Check for out of range turbo port. For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E], cscdGroupNum [0], portNumOnMuxDevice [MAX_PHY_PORTS_NUM_CNS = 12] and non-NULL virtualPortNumPtr. Expected: NON GT_OK.
1.9. Check for NULL pointer support. For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E], cscdGroupNum [2], portNumOnMuxDevice [21] and virtualPortNumPtr [NULL]. Expected: GT_BAD_PTR.
1.10. Check for out of enum support for groupMode. For 98EX1x6 device call function with groupMode [0x5AAAAAA5], cscdGroupNum [2], portNumOnMuxDevice [21] and non-NULL virtualPortNumPtr. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                           dev;
    CPSS_EXMX_VB_GROUPS_MODE_ENT    groupMode;
    GT_U32                          cscdGroupNum;
    GT_U8                           portNumOnMuxDevice;
    GT_U8                           virtualPortNum;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E],
        cscdGroupNum [2], portNumOnMuxDevice [21] and non-NULL virtualPortNumPtr.
        Expected: GT_OK for 98EX1x6 devices. */
        groupMode = CPSS_EXMX_VB_4_GROUPS_MODE_E;
        cscdGroupNum = 2;
        portNumOnMuxDevice = 21;

        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                            cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d, %d",
                                     dev, groupMode, cscdGroupNum, portNumOnMuxDevice);

        /* 1.2. Call function with groupMode [CPSS_EXMX_VB_8_GROUPS_MODE_E],
        cscdGroupNum [6] portNumOnMuxDevice [5] and non-NULL virtualPortNumPtr.
        Expected: GT_OK for 98EX1x6 devices. */
        groupMode = CPSS_EXMX_VB_8_GROUPS_MODE_E;
        cscdGroupNum = 6;
        portNumOnMuxDevice = 5;

        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                            cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d, %d",
                                     dev, groupMode, cscdGroupNum, portNumOnMuxDevice);

        /* 1.3. Check for invalid cscdGroupNum for current groupMode.
        For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E],
        cscdGroupNum [4], portNumOnMuxDevice [21] and non-NULL virtualPortNumPtr.
        Expected: NON GT_OK. */
        groupMode = CPSS_EXMX_VB_4_GROUPS_MODE_E;
        cscdGroupNum = 4;
        portNumOnMuxDevice = 21;

        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                            cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, groupMode, cscdGroupNum);

        /* 1.4. Check for invalid cscdGroupNum for current groupMode.
        For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_8_GROUPS_MODE_E],
        cscdGroupNum [8], portNumOnMuxDevice [5] and non-NULL virtualPortNumPtr.
        Expected: NON GT_OK. */
        groupMode = CPSS_EXMX_VB_8_GROUPS_MODE_E;
        cscdGroupNum = 8;
        portNumOnMuxDevice = 5;

        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                            cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, groupMode, cscdGroupNum);

        /* 1.5. Check for invalid portNumOnMuxDevice for current groupMode.
        For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E],
        cscdGroupNum [2], portNumOnMuxDevice [32] and non-NULL virtualPortNumPtr.
        Expected: NON GT_OK. */
        groupMode = CPSS_EXMX_VB_4_GROUPS_MODE_E;
        cscdGroupNum = 2;
        portNumOnMuxDevice = 32;

        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                            cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st,
                         dev, groupMode, cscdGroupNum, portNumOnMuxDevice);

        /* 1.6. Check for invalid portNumOnMuxDevice for current groupMode.
        For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_8_GROUPS_MODE_E],
        cscdGroupNum [6], portNumOnMuxDevice [16] and non-NULL virtualPortNumPtr.
        Expected: NON GT_OK. */
        groupMode = CPSS_EXMX_VB_8_GROUPS_MODE_E;
        cscdGroupNum = 6;
        portNumOnMuxDevice = 16;

        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                            cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st,
                         dev, groupMode, cscdGroupNum, portNumOnMuxDevice);

        /* 1.7. Check for turbo port processing. Call function with
        groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E/ CPSS_EXMX_VB_8_GROUPS_MODE_E],
        cscdGroupNum [0], portNumOnMuxDevice [1] and non-NULL virtualPortNumPtr.
        Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise. */
        cscdGroupNum = 0;
        portNumOnMuxDevice = 1;

        groupMode = CPSS_EXMX_VB_4_GROUPS_MODE_E;

        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                            cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d, %d",
                                     dev, groupMode, cscdGroupNum, portNumOnMuxDevice);

        groupMode = CPSS_EXMX_VB_8_GROUPS_MODE_E;

        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                            cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "Tiger device: %d, %d, %d, %d",
                                     dev, groupMode, cscdGroupNum, portNumOnMuxDevice);

        /* 1.8. Check for out of range turbo port.
        For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E],
        cscdGroupNum [0], portNumOnMuxDevice [MAX_PHY_PORTS_NUM_CNS = 12]
        and non-NULL virtualPortNumPtr. Expected: NON GT_OK. */
        groupMode = CPSS_EXMX_VB_4_GROUPS_MODE_E;
        cscdGroupNum = 0;
        portNumOnMuxDevice = MAX_PHY_PORTS_NUM_CNS;

        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                            cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st,
                         dev, groupMode, cscdGroupNum, portNumOnMuxDevice);

        /* 1.9. Check for NULL pointer support.
        For 98EX1x6 device call function with groupMode [CPSS_EXMX_VB_4_GROUPS_MODE_E],
        cscdGroupNum [2], portNumOnMuxDevice [21] and virtualPortNumPtr [NULL].
        Expected: GT_BAD_PTR. */
        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                                cscdGroupNum, portNumOnMuxDevice, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, virtualPortNumPtr = NULL", dev);

        /* 1.10. Check for out of enum support for groupMode.
        For 98EX1x6 device call function with groupMode [0x5AAAAAA5],
        cscdGroupNum [2], portNumOnMuxDevice [21] and non-NULL virtualPortNumPtr.
        Expected: GT_BAD_PARAM. */
        groupMode = 0x5AAAAAA5;
        cscdGroupNum = 2;
        portNumOnMuxDevice = 21;

        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                            cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, groupMode);
    }

    groupMode = CPSS_EXMX_VB_4_GROUPS_MODE_E;
    cscdGroupNum = 2;
    portNumOnMuxDevice = 21;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                                cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(dev, groupMode,
                            cscdGroupNum, portNumOnMuxDevice, &virtualPortNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxVbPortToCscdGroupMapGet
(
    IN GT_U8    devNum,
    IN GT_U8    physicalPortNum,
    OUT GT_U32  *cscdGroupNumPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxVbPortToCscdGroupMapGet)
{
/*
ITERATE DEVICES
1.1. Call function with physicalPortNum [1] and non-NULL cscdGroupNumPtr. Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1.2. For 98EX1x6 device call function with out of range physicalPortNum [MAX_PHY_PORTS_NUM_CNS = 12] and non-NULL cscdGroupNumPtr. Expected: NON GT_OK.
1.3. For 98EX1x6 device call function with physicalPortNum [1] and cscdGroupNumPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8   dev;
    GT_U8   physicalPortNum;
    GT_U32  cscdGroupNum;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with physicalPortNum [1] and
        non-NULL cscdGroupNumPtr. Expected: GT_OK for 98EX1x6 devices
        and NON GT_OK otherwise. */
        physicalPortNum = 1;

        st = cpssExMxVbPortToCscdGroupMapGet(dev, physicalPortNum, &cscdGroupNum);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "Tiger device: %d, %d",
                                     dev, physicalPortNum);

        /* 1.2. For 98EX1x6 device call function with out of range
        physicalPortNum [MAX_PHY_PORTS_NUM_CNS = 12] and non-NULL cscdGroupNumPtr.
        Expected: NON GT_OK. */
        physicalPortNum = MAX_PHY_PORTS_NUM_CNS;

        st = cpssExMxVbPortToCscdGroupMapGet(dev, physicalPortNum, &cscdGroupNum);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, physicalPortNum);

        /* 1.3. For 98EX1x6 device call function with physicalPortNum [1]
        and cscdGroupNumPtr [NULL]. Expected: GT_BAD_PTR. */
        physicalPortNum = 1;

        st = cpssExMxVbPortToCscdGroupMapGet(dev, physicalPortNum, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, physicalPortNum);
    }

    physicalPortNum = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxVbPortToCscdGroupMapGet(dev, physicalPortNum, &cscdGroupNum);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxVbPortToCscdGroupMapGet(dev, physicalPortNum, &cscdGroupNum);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxVbModeGet
(
    IN GT_U8                        devNum,
    OUT CPSS_EXMX_VB_MODE_ENT       *vbModePtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxVbModeGet)
{
/*
ITERATE_DEVICES
1.1. Call function with non-NULL vbModePtr. Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise.
1.2. For 98EX1x6 device call function with vbModePtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS       st = GT_OK;

    GT_U8                    dev;
    CPSS_EXMX_VB_MODE_ENT    vbMode;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with non-NULL vbModePtr.
        Expected: GT_OK for 98EX1x6 devices and NON GT_OK otherwise. */
        st = cpssExMxVbModeGet(dev, &vbMode);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "Tiger device: %d", dev);

        /* 1.2. For 98EX1x6 device call function with vbModePtr [NULL].
        Expected: GT_BAD_PTR. */
        st = cpssExMxVbModeGet(dev, NULL);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMXTG_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);
    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxVbModeGet(dev, &vbMode);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxVbModeGet(dev, &vbMode);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*
 * Configuration of cppExMxVb suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxVb)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxVbConfigSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxVb8TcTo4TcMapSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxVbCscdDsaEnableGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxVbGroupModeGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxVbEgressVirtualPortsGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxVbPortToCscdGroupMapGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxVbModeGet)
UTF_SUIT_END_TESTS_MAC(cpssExMxVb)
