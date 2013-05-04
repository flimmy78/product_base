/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfBrgSrcId.h
*
* DESCRIPTION:
*       Bridge Source-Id UT.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfBrgSrcId
#define __prvTgfBrgSrcId

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfBrgSrcIdPortForceConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Create VLAN 2 on all ports.
*           Set MAC table with two entries:
*              - MAC = 00:00:00:00:00:01, VLAN = 2, intType = PORT, 
*               port = 0, sourceId = 1
*              - MAC = 00:00:00:00:00:02, VLAN = 2, intType = PORT,
*               port = 8, sourceId = 0
*           Source ID configuration:
*               Disable port force source-ID assigment
*               Configure Global Assigment mode to FDB SA-based.
*               Configure default port source-ID to 2.
*               Delete port 8 from source-ID group 1.
*               Enable Unicast Egreess filter.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01, 
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is not captured in all ports.
*
*       Additional Configuration:
*           Enable Port Force Src-ID assigment.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01,
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is captured in port: 8.
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
GT_VOID prvTgfBrgSrcIdPortForceConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgSrcIdPortForceTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
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
GT_VOID prvTgfBrgSrcIdPortForceTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgSrcIdPortForceConfigurationRestore
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
GT_VOID prvTgfBrgSrcIdPortForceConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgSrcIdFdbSaAssigmentConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Create VLAN 2 on all ports.
*           Set MAC table with two entries:
*              - MAC = 00:00:00:00:00:01, VLAN = 2, intType = PORT, 
*               port = 0, sourceId = 1
*              - MAC = 00:00:00:00:00:02, VLAN = 2, intType = PORT,
*               port = 8, sourceId = 0
*           Source ID configuration:
*               Disable port force source-ID assigment
*               Configure Global Assigment mode to FDB SA-based.
*               Configure default port source-ID to 2.
*               Delete port 8 from source-ID group 1.
*               Enable Unicast Egreess filter.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01, 
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is not captured in port 8.
*
*       Additional Configuration:
*           Add to Source-ID group 1 port 8.
*           Delete from Source-ID group 2 port 8.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01,
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is captured on port: 8.
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
GT_VOID prvTgfBrgSrcIdFdbSaAssigmentConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgSrcIdFdbSaAssigmentTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
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
GT_VOID prvTgfBrgSrcIdFdbSaAssigmentTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgSrcIdFdbSaAssigmentConfigurationRestore
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
GT_VOID prvTgfBrgSrcIdFdbSaAssigmentConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgSrcIdFdbDaAssigmentConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Create VLAN 2 on all ports.
*           Set MAC table with two entries:
*              - MAC = 00:00:00:00:00:01, VLAN = 2, intType = PORT, 
*               port = 0, sourceId = 1
*              - MAC = 00:00:00:00:00:02, VLAN = 2, intType = PORT,
*               port = 8, sourceId = 0
*           Source ID configuration:
*               Disable port force source-ID assigment
*               Configure Global Assigment mode to FDB SA-based.
*               Configure default port source-ID to 2.
*               Delete port 8 from source-ID group 1.
*               Enable Unicast Egreess filter.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01, 
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is not captured in port 8.
*
*       Additional Configuration:
*           Add to Source-ID group 1 port 8.
*           Delete from Source-ID group 2 port 8.
*
*       Generate Traffic:
*           Send to device's port 0 packet:
*               daMAC = 00:00:00:00:00:02, saMAC = 00:00:00:00:00:01,
*               etherType = 0x8100 vlanID = 0x2.
*
*       Success Criteria:
*           Packet is captured on port: 8.
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
GT_VOID prvTgfBrgSrcIdFdbDaAssigmentConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgSrcIdFdbDaAssigmentTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:22,
*               macSa = 00:00:00:00:00:11,
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
GT_VOID prvTgfBrgSrcIdFdbDaAssigmentTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgSrcIdFdbDaAssigmentConfigurationRestore
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
GT_VOID prvTgfBrgSrcIdFdbDaAssigmentConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfFdbAging */


