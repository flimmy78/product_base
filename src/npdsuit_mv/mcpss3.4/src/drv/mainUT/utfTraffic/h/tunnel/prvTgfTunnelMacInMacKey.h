/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelMacInMacKey.h
*
* DESCRIPTION:
*       Verify the functionality of MIM TTI key MAC DA or MAC SA
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfTunnelMacInMacKeyh
#define __prvTgfTunnelMacInMacKeyh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvTgfTunnelTermUseMacSaInMimTtiLookupConfigSet
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
GT_VOID prvTgfTunnelTermUseMacSaInMimTtiLookupConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermUseMacSaInMimTtiLookupTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 MIM tunneled packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x8988 (MIM Ether type) 
*               iSid: 0x123456
*               iUP: 0x5
*               iDP: 0
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
GT_VOID prvTgfTunnelTermUseMacSaInMimTtiLookupTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermUseMacSaInMimTtiLookupConfigRestore
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
GT_VOID prvTgfTunnelTermUseMacSaInMimTtiLookupConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermMaskMacInMimTtiLookupConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
*         - set MAC mask as enable
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
GT_VOID prvTgfTunnelTermMaskMacInMimTtiLookupConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermMaskMacInMimTtiLookupTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 MIM tunneled packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:05,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x8988 (MIM Ether type) 
*               iSid: 0x123456
*               iUP: 0x5
*               iDP: 0
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
GT_VOID prvTgfTunnelTermMaskMacInMimTtiLookupTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermMaskMacInMimTtiLookupConfigRestore
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
GT_VOID prvTgfTunnelTermMaskMacInMimTtiLookupConfigRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermMimBasicConfigSet
*
* DESCRIPTION:
*       Set test configuration:
*         - set index of the rule in the TCAM to 0
*         - set macMode PRV_TGF_TTI_MAC_MODE_DA_E
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
GT_VOID prvTgfTunnelTermMimBasicConfigSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermMimBasicTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 MIM tunneled packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:34:02,
*               Tagged: true (with VLAN tag 5)
*               Ether Type: 0x8988 (MIM Ether type) 
*               iSid: 0x123456
*               iUP: 0x5
*               iDP: 0
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
GT_VOID prvTgfTunnelTermMimBasicTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTunnelTermBasicMimConfigRestore
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
GT_VOID prvTgfTunnelTermBasicMimConfigRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfTunnelMacInMacKeyh */

