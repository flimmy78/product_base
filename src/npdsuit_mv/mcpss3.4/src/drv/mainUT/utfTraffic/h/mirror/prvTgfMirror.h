/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfMirror.h
*
* DESCRIPTION:
*       CPSS Mirror
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfMirror
#define __prvTgfMirror

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvTgfIngressMirrorHopByHopConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set analyzer interface for index 4. 
*            Device number 0, port number 18, port interface.
*         -	Set analyzer interface for index 0. 
*            Device number 0, port number 23, port interface.
*         -	Set analyzer interface index to 4 and enable Rx mirroring. 
*         - Enable Rx mirroring on port 8. 
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
GT_VOID prvTgfIngressMirrorHopByHopConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorHopByHopTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*       Disable Rx mirroring.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18, 23.
*       Set analyzer interface index to 0 and enable  Rx mirroring. 
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*       Disable Rx mirroring on port 8.  
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18, 23.
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
GT_VOID prvTgfIngressMirrorHopByHopTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorHopByHopConfigurationRestore
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
GT_VOID prvTgfIngressMirrorHopByHopConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorSourceBasedConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*	      - Set source-based forwarding mode. 
*	      - Set analyzer interface for index 6. 
*           Device number 0, port number 18, port interface.
*	      - Set analyzer interface for index 0. 
*           Device number 0, port number 23, port interface.
*	      - Enable Rx mirroring on port 8 and set analyzer 
*           interface index for the port to 6. 
*         - Enable Rx mirroring on port 0 and set analyzer interface 
*           index for the port to 0. 
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
GT_VOID prvTgfIngressMirrorSourceBasedConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorSourceBasedTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*       Disable Rx mirroring on port 8.  
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18, 23.
*       Disable Rx mirroring on port 0.  
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 8, 18, 23.
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
GT_VOID prvTgfIngressMirrorSourceBasedTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorSourceBasedConfigurationRestore
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
GT_VOID prvTgfIngressMirrorSourceBasedConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorHighestIndexSelectionConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*	      - Set source-based forwarding mode. 
*	      - Set analyzer interface for index 3. 
*           Device number 0, port number 18, port interface.
*	      - Set analyzer interface for index 1. 
*           Device number 0, port number 23, port interface.
*	      - Enable Rx mirroring on port 8 and set analyzer 
*           interface index for the port to 1. 
*         - Set FDB entry with MAC address 00:00:00:00:00:01 
*           and mirrorToRxAnalyzerPortEn = GT_TRUE.
*         -	Set analyzer interface index to 3 and enable Rx mirroring.        
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
GT_VOID prvTgfIngressMirrorHighestIndexSelectionConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorHighestIndexSelectionTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:01,
*           Success Criteria:
*                1 packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*         - Set analyzer interface index to 0 and enable Rx mirroring. 
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
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
GT_VOID prvTgfIngressMirrorHighestIndexSelectionTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorHighestIndexSelectionConfigurationRestore
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
GT_VOID prvTgfIngressMirrorHighestIndexSelectionConfigurationRestore
(
    GT_VOID
);
/*******************************************************************************
* prvTgfEgressMirrorHopByHopConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set analyzer interface for index 2. 
*           Device number 0, port number 18, port interface.
*         -	Set analyzer interface for index 0. 
*           Device number 0, port number 23, port interface.
*	      - Set analyzer interface index to 2 and enable Tx mirroring. 
*	      - Enable Tx mirroring on port 0.  
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
GT_VOID prvTgfEgressMirrorHopByHopConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfEgressMirrorHopByHopTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*       Disable Tx mirroring.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18, 23.
*       Set analyzer interface index to 0 and enable  Tx mirroring. 
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*       Disable Rx mirroring on port 0.  
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18, 23.
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
GT_VOID prvTgfEgressMirrorHopByHopTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfEgressMirrorHopByHopConfigurationRestore
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
GT_VOID prvTgfEgressMirrorHopByHopConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfEgressMirrorSourceBasedConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*	      - Set source-based forwarding mode. 
*         -	Set analyzer interface for index 5. 
*           Device number 0, port number 17, port interface.
*         - Set analyzer interface for index 0. 
*           Device number 0, port number 0, port interface.
*         -	Enable Tx mirroring on port 23 and 
*           set analyzer interface index for the port to 5.
*         -	Enable Tx mirroring on port 18 and 
*           set analyzer interface index for the port to 0.
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
GT_VOID prvTgfEgressMirrorSourceBasedConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfEgressMirrorSourceBasedTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Learn MACs UC1 and UC2 on Tx mirrored ports 18 and 23
*           Send to device's port 8 two packets:
*               macDa = UC1,macDa = UC2,
*           Success Criteria:
*                1 packet is captured o n ports Tx mirrored 23.
*                1 Packets are captured on port Tx mirrored 18.
*                1 Packets are captured on analyzer port 0.
*                1 Packets are captured on analyzer port 17.
*       Disable Tx mirroring on port 18.  
*           Send to device's port 8 two packets:
*               macDa = UC1,macDa = UC2,
*           Success Criteria:
*                1 packet is captured o n ports Tx mirrored 23.
*                1 Packets are captured on port Tx mirrored 18.
*                0 Packets are captured on analyzer port 0.
*                1 Packets are captured on analyzer port 17.
*       Disable Tx mirroring on port 23.  
*           Send to device's port 8 two packets:
*               macDa = UC1,macDa = UC2,
*           Success Criteria:
*                1 packet is captured o n ports Tx mirrored 23.
*                1 Packets are captured on port Tx mirrored 18.
*                0 Packets are captured on analyzer port 0.
*                0 Packets are captured on analyzer port 17.
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
GT_VOID prvTgfEgressMirrorSourceBasedTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfEgressMirrorSourceBasedConfigurationRestore
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
GT_VOID prvTgfEgressMirrorSourceBasedConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorForwardingModeChangeConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set analyzer interface for index 3. 
*            Device number 0, port number 18, port interface.
*         -	Set analyzer interface for index 0. 
*            Device number 0, port number 23, port interface.
*         -	Set analyzer interface index to 3 and enable Rx mirroring. 
*         - Enable Rx mirroring on port 8. 
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
GT_VOID prvTgfIngressMirrorForwardingModeChangeConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorForwardingModeChangeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*       Disable Rx mirroring on port 8.  
*       Set source-based forwarding mode.
*       Enable Rx mirroring on port 8 and set 
*       analyzer interface index for the port to 0.  
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*       Disable Rx mirroring on port 8.  
*       Set hop-byhop forwarding mode.
*       Enable Rx mirroring on port 8.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
 
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
GT_VOID prvTgfIngressMirrorForwardingModeChangeTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorForwardingModeChangeConfigurationRestore
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
GT_VOID prvTgfIngressMirrorForwardingModeChangeConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfEgressMirrorForwardingModeChangeConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set analyzer interface for index 3. 
*            Device number 0, port number 18, port interface.
*         -	Set analyzer interface for index 0. 
*            Device number 0, port number 23, port interface.
*         -	Set analyzer interface index to 3 and enable Tx mirroring. 
*         - Enable Tx mirroring on port 0. 
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
GT_VOID prvTgfEgressMirrorForwardingModeChangeConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfEgressMirrorForwardingModeChangeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
*       Disable Tx mirroring on port 0.  
*       Set source-based forwarding mode.
*       Enable Tx mirroring on port 0 and set 
*       analyzer interface index for the port to 0.  
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                Packet is captured on ports 0, 18.
*                2 Packets are captured on port 23.
*       Disable Tx mirroring on port 0.  
*       Set hop-byhop forwarding mode.
*       Enable Tx mirroring on port 0.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*                1 packet is captured on ports 0, 23.
*                2 Packets are captured on port 18.
 
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
GT_VOID prvTgfEgressMirrorForwardingModeChangeTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfEgressMirrorForwardingModeChangeConfigurationRestore
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
GT_VOID prvTgfEgressMirrorForwardingModeChangeConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorVlanTagRemovalConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Create VLAN 5 with all ports.
*         - Remove port 18 from VLAN 5.
*	      - Set analyzer interface for index 3. 
*           Device number 0, port number 18, port interface.
*         - Enable Rx mirroring on port 8.  
*         -	Set analyzer interface index to 3 and enable Rx mirroring.   
*         - Enable VLAN tag removal of mirrored traffic.     
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
GT_VOID prvTgfIngressMirrorVlanTagRemovalConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorVlanTagRemovalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*           Success Criteria:
*               Untagged packet is captured on port 18.
*           Disable VLAN tag removal of mirrored traffic.
*           Send to device's port 8 packet:
*           Success Criteria:
*               Tagged packet is captured on port 18.
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
GT_VOID prvTgfIngressMirrorVlanTagRemovalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorVlanTagRemovalConfigurationRestore
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
GT_VOID prvTgfIngressMirrorVlanTagRemovalConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfEgressMirrorVlanTagRemovalConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Create VLAN 5 with all ports.
*         - Remove port 18 from VLAN 5.
*	      - Set analyzer interface for index 3. 
*           Device number 0, port number 18, port interface.
*         - Enable Rx mirroring on port 8.  
*         -	Set analyzer interface index to 3 and enable Rx mirroring.   
*         - Enable VLAN tag removal of mirrored traffic.     
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
GT_VOID prvTgfEgressMirrorVlanTagRemovalConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIngressMirrorVlanTagRemovalTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*           Success Criteria:
*               Untagged packet is captured on port 18.
*           Disable VLAN tag removal of mirrored traffic.
*           Send to device's port 8 packet:
*           Success Criteria:
*               Tagged packet is captured on port 18.
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
GT_VOID prvTgfEgressMirrorVlanTagRemovalTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfEgressMirrorVlanTagRemovalConfigurationRestore
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
GT_VOID prvTgfEgressMirrorVlanTagRemovalConfigurationRestore
(
    GT_VOID
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfMirror */

