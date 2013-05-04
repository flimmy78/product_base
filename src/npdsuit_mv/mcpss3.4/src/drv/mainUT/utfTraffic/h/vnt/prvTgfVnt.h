/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfVnt.h
*
* DESCRIPTION:
*       Virtual Network Tester (VNT)
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfVnt
#define __prvTgfVnt

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfVntOamPortPduTrapSet
*
* DESCRIPTION:
*       Set test configuration:
*           Enable OAM Port PPU Trap feature.
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
GT_VOID prvTgfVntOamPortPduTrapSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntLoopbackModeConfSet
*
* DESCRIPTION:
*       Set test configuration:
*           Configure Loopback Mode om port 8.
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
GT_VOID prvTgfVntLoopbackModeConfSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntOamPortPduTrapTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = 01:80:C2:00:00:02,
*               macSa = 00:00:00:00:00:02
*           Success Criteria:
*               Packet is captured on ports 0 and 18, 23 and CPU.
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
GT_VOID prvTgfVntOamPortPduTrapTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntLoopbackTrafficConfGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 OAM control packet:
*               macDa = 01:80:C2:00:00:02,
*               macSa = 00:00:00:00:00:02,
*	            Ether Type 8809.
*	            Subtype 0x03.
*           Success Criteria:
*               Packet is captured on CPU.
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*               Packet is captured on port 8.
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
GT_VOID prvTgfVntLoopbackTrafficConfGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntLoopbackConfRestore
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
GT_VOID prvTgfVntLoopbackConfRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntOamPortPduTrapRestore
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
GT_VOID prvTgfVntOamPortPduTrapRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntCfmEtherTypeIdentificationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Enable the Ingress Policy Engine.
*           Set Pcl rule to trap packets with ethertype 3434.
*           Set CFM ethertype to 3434.  
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
GT_VOID prvTgfVntCfmEtherTypeIdentificationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntCfmEtherTypeIdentificationTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*               ethertype = 2222
*           Success Criteria:
*               Packet is captured on ports 0,18,23.
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*               ethertype = 3434
*           Success Criteria:
*               Packet is captured on CPU only.
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
GT_VOID prvTgfVntCfmEtherTypeIdentificationTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntCfmEtherTypeIdentificationRestore
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
GT_VOID prvTgfVntCfmEtherTypeIdentificationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntCfmOpcodeIdentificationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Enable the Ingress Policy Engine.
*           Set Pcl rule to trap packets with ethertype 3434 and UDB 0xFF and
*           to redirect them to the send port. 
*           Set CFM ethertype to 3434. 
*           Set opcode to 0x7F.
*            
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
GT_VOID prvTgfVntCfmOpcodeIdentificationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntCfmOpcodeIdentificationTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*               ethertype = 3434
*           Success Criteria:
*               Packet is captured on CPU.
*               Packet is captured on port 8 with opcode 0x7F
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
GT_VOID prvTgfVntCfmOpcodeIdentificationTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntCfmEtherTypeIdentificationRestore
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
GT_VOID prvTgfVntCfmOpcodeIdentificationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntOamPortUnidirectionalTransmitModeSet
*
* DESCRIPTION:
*       Set test configuration:
*           Enable/Disable the port for unidirectional transmit.
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
GT_VOID prvTgfVntOamPortUnidirectionalTransmitModeSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntOamPortUnidirectionalTransmitModeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 8 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:02,
*           Success Criteria:
*               Packet is captured on ports 0,18,23.
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
GT_VOID prvTgfVntOamPortUnidirectionalTransmitModeTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVntOamPortUnidirectionalTransmitModeRestore
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
GT_VOID prvTgfVntOamPortUnidirectionalTransmitModeRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfVnt */

