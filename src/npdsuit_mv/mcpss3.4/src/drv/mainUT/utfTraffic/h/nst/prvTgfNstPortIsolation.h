/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfNstPortIsolation.h
*
* DESCRIPTION:
*       NST Port Isolation
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfNstPortIsolation
#define __prvTgfNstPortIsolation

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfNstL2PortIsolationConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Configure portIsolationMode on VLAN entry (VID = 5). 
*           Enable Port Isolation feature.
*           Set port isolation table entry for L2 with:
*               devNum        = 0
*               trafficType   = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E
*               srcInterface  = devNum = 0, portNum = 8
*               cpuPortMember = GT_FALSE
*               localPortsMembersPtr = 0x40001
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL2PortIsolationConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL2PortIsolationTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:05,
*           Success Criteria:
*               Packet is captured on ports 0 and 18
*       Additional Configuration:
*           Disable port isolation feature in VLAN entry.
*       Generate Traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:05,
*           Success Criteria:
*               Packet is captured on ports: 0,18,23
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL2PortIsolationTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL2PortIsolationConfigurationRestore
*
* DESCRIPTION:
*       Restore test configuration
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL2PortIsolationConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL3PortIsolationConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Configure portIsolationMode on VLAN entry (VID = 5). 
*           Enable Port Isolation feature.
*           Set port isolation table entry for L3 with:
*               devNum        = 0
*               trafficType   = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E
*               srcInterface  = devNum = 0, portNum = 8
*               cpuPortMember = GT_FALSE
*               localPortsMembersPtr = 0x40001
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL3PortIsolationConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL3PortIsolationTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:05,
*           Success Criteria:
*               Packet is captured on ports 0 and 18
*       Additional Configuration:
*           Disable port isolation feature in VLAN entry.
*       Generate Traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:05,
*           Success Criteria:
*               Packet is captured on ports: 0,18,23
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL3PortIsolationTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL3PortIsolationConfigurationRestore
*
* DESCRIPTION:
*       Restore test configuration
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL3PortIsolationConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL2L3PortIsolationConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Configure portIsolationMode on VLAN entry (VID = 5). 
*           Enable Port Isolation feature.
*           Set port isolation table entry for L3 with:
*               devNum        = 0
*               trafficType   = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E
*               srcInterface  = devNum = 0, portNum = 0
*               cpuPortMember = GT_FALSE
*               localPortsMembersPtr = 0x40001
*           Set port isolation table entry for L2 with:
*               devNum        = 0
*               trafficType   = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E
*               srcInterface  = devNum = 0, portNum = 0
*               cpuPortMember = GT_FALSE
*               localPortsMembersPtr = 0x100
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL2L3PortIsolationConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL2L3PortIsolationTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:05,
*           Send to device's port 0 two IP packets:
*               0000: 00 00 00 00 34 02 00 00 00 00 00 01 81 00 00 05
*               0010: 08 00 45 00 00 2A 00 00 00 00 40 04 76 CB 01 01
*               0020: 01 01 01 01 01 03 00 00 00 00 00 00 00 00 00 00
*               0030: 00 00 00 00 00 00 00 00 00 00 00 00
*
*               0000: 00 00 00 00 34 02 00 00 00 00 00 01 81 00 00 05
*               0010: 08 00 45 00 00 2A 00 00 00 00 40 04 72 C7 02 02
*               0020: 02 02 01 02 03 04 00 00 00 00 00 00 00 00 00 00
*               0030: 00 00 00 00 00 00 00 00 00 00 00 00
*
*           Success Criteria:
*               L2 packet is captured on port 8
*               L3 packet is captured on port 18
*       Additional Configuration:
*           Disable port isolation feature in VLAN entry
*       Generate Traffic:
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:05,
*           Send to device's port 0 two IP packets:
*               0000: 00 00 00 00 34 02 00 00 00 00 00 01 81 00 00 05
*               0010: 08 00 45 00 00 2A 00 00 00 00 40 04 76 CB 01 01
*               0020: 01 01 01 01 01 03 00 00 00 00 00 00 00 00 00 00
*               0030: 00 00 00 00 00 00 00 00 00 00 00 00
*
*               0000: 00 00 00 00 34 02 00 00 00 00 00 01 81 00 00 05
*               0010: 08 00 45 00 00 2A 00 00 00 00 40 04 72 C7 02 02
*               0020: 02 02 01 02 03 04 00 00 00 00 00 00 00 00 00 00
*               0030: 00 00 00 00 00 00 00 00 00 00 00 00
*
*           Success Criteria:
*               L2 packet is captured on port 8
*               L3 packet is captured on port 18, 23
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL2L3PortIsolationTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL2L3PortIsolationConfigurationRestore
*
* DESCRIPTION:
*       Restore test configuration
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL2L3PortIsolationConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL2TrunkPortIsolationConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Configure port 8 and port 23 as members in trunk with trunkId = 1
*           Configure portIsolationMode on VLAN entry (VID = 5).
*           Enable Port Isolation feature
*           Set port isolation table entry for L2 with:
*               devNum        = 0
*               trafficType   = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E
*               srcInterface.type    = CPSS_INTERFACE_TRUNK_E
*               srcInterface.trunkId = 1.
*               cpuPortMember        = GT_FALSE
*               localPortsMembersPtr = 0x40000
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL2TrunkPortIsolationConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL2TrunkPortIsolationTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*
*           Success Criteria:
*               Packet is captured on port 18
*
*       Additional Configuration:
*           Disable port isolation feature in VLAN entry
*       Generate Traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:05,
*
*           Success Criteria:
*               Packet is captured on ports 0, 18
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL2TrunkPortIsolationTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL2TrunkPortIsolationConfigurationRestore
*
* DESCRIPTION:
*       Restore test configuration
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL2TrunkPortIsolationConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL3TrunkPortIsolationConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Configure port 0 as member in trunk with truckId = 1
*           Configure portIsolationMode on VLAN entry (VID = 5). 
*           Enable Port Isolation feature.
*           Set port isolation table entry for L2 with:
*               devNum               = 0
*               trafficType          = CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E
*               srcInterface.type    = CPSS_INTERFACE_TRUNK_E
*               srcInterface.trunkId = 1.
*               cpuPortMember        = GT_FALSE
*               localPortsMembersPtr = 0x40000
*           Perform Two route entries UC IPV4 Routing:
*           Note: change port interface to truck interface.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL3TrunkPortIsolationConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL3TrunkPortIsolationTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 two IP packets:
*               0000: 00 00 00 00 34 02 00 00 00 00 00 01 81 00 00 05
*               0010: 08 00 45 00 00 2A 00 00 00 00 40 04 76 CB 01 01
*               0020: 01 01 01 01 01 03 00 00 00 00 00 00 00 00 00 00
*               0030: 00 00 00 00 00 00 00 00 00 00 00 00
*
*               0000: 00 00 00 00 34 02 00 00 00 00 00 01 81 00 00 05
*               0010: 08 00 45 00 00 2A 00 00 00 00 40 04 72 C7 02 02
*               0020: 02 02 01 02 03 04 00 00 00 00 00 00 00 00 00 00
*               0030: 00 00 00 00 00 00 00 00 00 00 00 00
*
*           Success Criteria:
*               One packet is captured on port 18
*
*           Additional Configuration:
*               Disable port isolation feature in VLAN entry.
*
*           Send to device's port 0 two IP packets:
*               0000: 00 00 00 00 34 02 00 00 00 00 00 01 81 00 00 05
*               0010: 08 00 45 00 00 2A 00 00 00 00 40 04 76 CB 01 01
*               0020: 01 01 01 01 01 03 00 00 00 00 00 00 00 00 00 00
*               0030: 00 00 00 00 00 00 00 00 00 00 00 00
*
*               0000: 00 00 00 00 34 02 00 00 00 00 00 01 81 00 00 05
*               0010: 08 00 45 00 00 2A 00 00 00 00 40 04 72 C7 02 02
*               0020: 02 02 01 02 03 04 00 00 00 00 00 00 00 00 00 00
*               0030: 00 00 00 00 00 00 00 00 00 00 00 00
*
*           Success Criteria:
*               Packet is captured on ports: 18,23
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL3TrunkPortIsolationTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfNstL3TrunkPortIsolationConfigurationRestore
*
* DESCRIPTION:
*       Restore test configuration
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfNstL3TrunkPortIsolationConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfNstPortIsolation */

