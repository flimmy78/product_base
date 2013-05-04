/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfFdbAuNaMessageExtFormat.h
*
* DESCRIPTION:
*       Bridge FDB AU NA Message with new format UT.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfFdbAuNaMessageExtFormat
#define __prvTgfFdbAuNaMessageExtFormat

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Create VLAN 2 on all ports (0,8,18,23)
*
* INPUTS:
*       vlanId  - VLAN ID
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
GT_VOID prvTgfFdbAuNaMessageExtFormatConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatWithTag1DueToNewAddrTrafficGenerate
*
* DESCRIPTION:
*           Disable learning.
*           Send double tagged packet with unknown unicast.
*       Generate traffic:
*           Send from port1 double tagged packet:
*           Success Criteria:
*               The AU NA message is updated correctly including the extended 
*               parameters: vid1, up0 and isMoved.
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
GT_VOID prvTgfFdbAuNaMessageExtFormatWithTag1DueToNewAddrTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatDueToMovedPortTrafficGenerate
*
* DESCRIPTION:
*           Learn MAC on FDB on port2.
*           Disable learning.
*           Send single tagged packet with known unicast, but from different 
*           source (port).
*       Generate traffic:
*           Send from port1 single tagged packet:
*           Success Criteria:
*               The AU NA message is updated correctly including the extended 
*               parameters: up0, isMoved oldSrcId, oldDstInterface and
*               oldAssociatedDevNum.
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
GT_VOID prvTgfFdbAuNaMessageExtFormatDueToMovedPortTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatDueToMovedTrunkTrafficGenerate
*
* DESCRIPTION:
*           Learn MAC on FDB on trunk.
*           Disable learning.
*           Send single tagged packet with known unicast, but from different
*           source (port).
*       Generate traffic:
*           Send from port1 single tagged packet:
*           Success Criteria:
*               The AU NA message is updated correctly including the extended 
*               parameters: up0, isMoved oldSrcId, oldDstInterface and
*               oldAssociatedDevNum.
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
GT_VOID prvTgfFdbAuNaMessageExtFormatDueToMovedTrunkTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfFdbAuNaMessageExtFormatConfigurationRestore
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
GT_VOID prvTgfFdbAuNaMessageExtFormatConfigurationRestore
(
    void
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfFdbAuNaMessageExtFormat */


