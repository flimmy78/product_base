/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfVlanTag1RemIfZero.h
*
* DESCRIPTION:
*       Bridge VLAN Tag1 Removal If Zero UT.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfVlanTag1RemIfZero
#define __prvTgfVlanTag1RemIfZero

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*
*       Generate Traffic:
*
*       Success Criteria:
*
*       Additional Configuration:
*
*       Generate Traffic:
*
*       Success Criteria:
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
GT_VOID prvTgfVlanTag1RemIfZeroConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroDisabledTrafficGenerate
*
* DESCRIPTION:
*           Disable VLan Tag1 Removal.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 0.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 single tagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is not removed and it's value 0
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
GT_VOID prvTgfVlanTag1RemIfZeroDisabledTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroSingleTagTrafficGenerate
*
* DESCRIPTION:
*           Enable VLan Tag1 Removal for not double tagged packets.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 0.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 single tagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is removed
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
GT_VOID prvTgfVlanTag1RemIfZeroSingleTagTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroDoubleTagTrafficGenerate
*
* DESCRIPTION:
*           Enable VLan Tag1 Removal for all packets.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 0.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 double tagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is removed
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
GT_VOID prvTgfVlanTag1RemIfZeroDoubleTagTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroUntagTrafficGenerate
*
* DESCRIPTION:
*           Enable VLan Tag1 Removal for all packets.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 0.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 untagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is removed
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
GT_VOID prvTgfVlanTag1RemIfZeroUntagTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroDoubleTagDisableTrafficGenerate
*
* DESCRIPTION:
*           Enable VLan Tag1 Removal for not double tagged packets.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 0.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 double tagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is not removed
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
GT_VOID prvTgfVlanTag1RemIfZeroDoubleTagDisableTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroTag1AssignmentIsNotZeroTrafficGenerate
*
* DESCRIPTION:
*           Enable VLan Tag1 Removal for all packets.
*           Configure Vlan command to Tag1 inner tag and Tag0 outer tag
*           TTI Action for Tag1 is Modify_ALL and VID = 2.
*           TTI Action for Tag0 is Modify_ALL and VID = 5.
*       Generate traffic:
*           Send from port0 untagged packet:
*               macDa = 00:00:22:22:22:22
*               macSa = 00:00:11:11:11:11
*           Success Criteria:
*               Tag1 is removed
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
GT_VOID prvTgfVlanTag1RemIfZeroTag1AssignmentIsNotZeroTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanTag1RemIfZeroConfigurationRestore
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
GT_VOID prvTgfVlanTag1RemIfZeroConfigurationRestore
(
    GT_VOID
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfVlanTag1RemIfZero */


