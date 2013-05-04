/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfVlanKeepVlan1.h
*
* DESCRIPTION:
*       Keep Vlan1 UT.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfVlanKeepVlan1
#define __prvTgfVlanKeepVlan1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfVlanKeepVlan1ConfigurationSet
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
GT_VOID prvTgfVlanKeepVlan1ConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfVlanKeepVlan1EnTagStateVlan1Vlan0DoubleTagTrafficGenerate
*
* DESCRIPTION:
*           Enable Keep Vlan1 for destination port 18 and up 4.
*           Configure Vlan command to {Tag1, Tag0}
*       Generate traffic:
*           Send from port0 double tagged packet, where Tag0 = 2 and Tag1 = 7:
*               macDa = 00:00:22:22:22:22 (port 18)
*               macSa = 00:00:11:11:11:11 (port 0)
*           Success Criteria:
*               Tag1 is not removed and it's value 7
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
GT_VOID prvTgfVlanKeepVlan1EnTagStateVlan1Vlan0DoubleTagTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanKeepVlan1EnTagStateVlan0DoubleTagTrafficGenerate
*
* DESCRIPTION:
*           Enable Keep Vlan1 for destination port 18 and up 6.
*           Configure Vlan command to inner Tag0
*       Generate traffic:
*           Send from port0 double tagged packet, where Tag0 = 2 and Tag1 = 7:
*               macDa = 00:00:22:22:22:22 (port 18)
*               macSa = 00:00:11:11:11:11 (port 0)
*           Success Criteria:
*               Tag1 is not removed and it's value 7
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
GT_VOID prvTgfVlanKeepVlan1EnTagStateVlan0DoubleTagTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanKeepVlan1EnTagStateUntaggedDoubleTagTrafficGenerate
*
* DESCRIPTION:
*           Enable Keep Vlan1 for destination port 18 and up 4.
*           Configure Vlan command to inner Tag0
*       Generate traffic:
*           Send from port0 double tagged packet, where Tag0 = 2 and Tag1 = 7:
*               macDa = 00:00:22:22:22:22 (port 18)
*               macSa = 00:00:11:11:11:11 (port 0)
*           Success Criteria:
*               Tag1 is not removed and it's value 7
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
GT_VOID prvTgfVlanKeepVlan1EnTagStateUntaggedDoubleTagTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanKeepVlan1EnTagStateVlan0SingleTagTrafficGenerate
*
* DESCRIPTION:
*           Enable Keep Vlan1 for destination port 18 and up 6.
*           Configure Vlan command to inner Tag0
*       Generate traffic:
*           Send from port0 single tagged packet, where Tag0 = 2:
*               macDa = 00:00:22:22:22:22 (port 18)
*               macSa = 00:00:11:11:11:11 (port 0)
*           Success Criteria:
*               Tag1 is removed
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
GT_VOID prvTgfVlanKeepVlan1EnTagStateVlan0SingleTagTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanKeepVlan1DisTagStateVlan0DoubleTagTrafficGenerate
*
* DESCRIPTION:
*           Disable Keep Vlan1 for destination port 18 and up 4.
*           Configure Vlan command to inner Tag0
*       Generate traffic:
*           Send from port0 double tagged packet, where Tag0 = 2 and Tag1 = 7:
*               macDa = 00:00:22:22:22:22 (port 18)
*               macSa = 00:00:11:11:11:11 (port 0)
*           Success Criteria:
*               Tag1 is removed
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
GT_VOID prvTgfVlanKeepVlan1DisTagStateVlan0DoubleTagTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanKeepVlan1DisTagStateUntaggedDoubleTagTrafficGenerate
*
* DESCRIPTION:
*           Disable Keep Vlan1 for destination port 18 and up 4.
*           Configure Vlan command to untagged
*       Generate traffic:
*           Send from port0 double tagged packet, where Tag0 = 2 and Tag1 = 7:
*               macDa = 00:00:22:22:22:22 (port 18)
*               macSa = 00:00:11:11:11:11 (port 0)
*           Success Criteria:
*               Tag1 is removed
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
GT_VOID prvTgfVlanKeepVlan1DisTagStateUntaggedDoubleTagTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfVlanKeepVlan1ConfigurationRestore
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
GT_VOID prvTgfVlanKeepVlan1ConfigurationRestore
(
    GT_VOID
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfVlanKeepVlan1 */


