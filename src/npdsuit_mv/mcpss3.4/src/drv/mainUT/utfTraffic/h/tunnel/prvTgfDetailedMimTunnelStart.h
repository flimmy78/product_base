/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfDetailedMimTunnelStart.h
*
* DESCRIPTION:
*       Verify the functionality of UP marking in MIM tunnel start
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfDetailedMimTunnelStarth
#define __prvTgfDetailedMimTunnelStarth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvTgfTunnelStartUpMarkingModeConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_SA_E
*         - set MAC mask as disable
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
GT_VOID prvTgfTunnelStartUpMarkingModeConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartUpMarkingModeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 MIM tunneled packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800
*
*           Success Criteria:
*               Packet striped and forwarded to port 23 VLAN 6
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
GT_VOID prvTgfTunnelStartUpMarkingModeTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartUpMarkingModeConfigRestore
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
GT_VOID prvTgfTunnelStartUpMarkingModeConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartMimEtherTypeConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
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
GT_VOID prvTgfTunnelStartMimEtherTypeConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartMimEtherTypeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macSa = 00:00:00:00:00:22,
*               macDa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800
*
*           Success Criteria:
*               Packet striped and forwarded to port 23 VLAN 6
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
GT_VOID prvTgfTunnelStartMimEtherTypeTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartMimEtherTypeConfigRestore
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
GT_VOID prvTgfTunnelStartMimEtherTypeConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartIdeiMarkingModeConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
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
GT_VOID prvTgfTunnelStartIdeiMarkingModeConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartIdeiMarkingModeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macSa = 00:00:00:00:00:22,
*               macDa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x0800
*
*           Success Criteria:
*               Packet striped and forwarded to port 23 VLAN 6
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
GT_VOID prvTgfTunnelStartIdeiMarkingModeTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelStartIdeiMarkingModeConfigRestore
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
GT_VOID prvTgfTunnelStartIdeiMarkingModeConfigRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfDetailedMimTunnelStarth */
