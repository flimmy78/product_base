/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfSampling.h
*
* DESCRIPTION:
*       Sampling tests for IPFIX declarations
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfSamplingh
#define __prvTgfSamplingh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfIpfixSamplingSimpleTestInit
*
* DESCRIPTION:
*       Set test configuration:
*       Enable the Ingress Policy Engine set Pcl rule.  
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
GT_VOID prvTgfIpfixSamplingSimpleTestInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixSamplingToCpuTestInit
*
* DESCRIPTION:
*       Set test configuration:
*       Enable the Ingress Policy Engine set Pcl rule.  
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
GT_VOID prvTgfIpfixSamplingToCpuTestInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixAlarmEventsTestInit
*
* DESCRIPTION:
*       Set test configuration:
*       Enable the Ingress Policy Engine set Pcl rule.  
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
GT_VOID prvTgfIpfixAlarmEventsTestInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixTimestampToCpuTestInit
*
* DESCRIPTION:
*       Set test configuration:
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
GT_VOID prvTgfIpfixTimestampToCpuTestInit
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixSamplingSimpleTestTrafficGenerate
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
GT_VOID prvTgfIpfixSamplingSimpleTestTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixSamplingToCpuTestTrafficGenerate
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
GT_VOID prvTgfIpfixSamplingToCpuTestTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixAlarmEventsTestTrafficGenerate
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
GT_VOID prvTgfIpfixAlarmEventsTestTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixTimestampToCpuTestTrafficGenerate
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
GT_VOID prvTgfIpfixTimestampToCpuTestTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixSamplingTestRestore
*
* DESCRIPTION:
*       Restore configuration
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
GT_VOID prvTgfIpfixSamplingTestRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfIpfixTimestampToCpuTestRestore
*
* DESCRIPTION:
*       Restore configuration
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
GT_VOID prvTgfIpfixTimestampToCpuTestRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfSamplingh */
