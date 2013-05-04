/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPortGroupAlarms.h
*
* DESCRIPTION:
*       IPFIX Alarms per port group test for IPFIX declarations
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfPortGroupAlarmsh
#define __prvTgfPortGroupAlarmsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    /*******************************************************************************
* prvTgfIpfixPortGroupAlarmsTestInit
*
* DESCRIPTION:
*       IPFIX per port group alarms manipulation test configuration set.
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
GT_VOID prvTgfIpfixPortGroupAlarmsTestInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixPortGroupAlarmsTestTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
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
GT_VOID prvTgfIpfixPortGroupAlarmsTestTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixPortGroupEntryTestRestore
*
* DESCRIPTION:
*       IPFIX per port group alarms manipulation test configuration restore.
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
GT_VOID prvTgfIpfixPortGroupAlarmsTestRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfPortGroupAlarmsh */
