/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPclTrunkHashVal.h
*
* DESCRIPTION:
*       Ingress PCL Trunk Hash Value using
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfPclTrunkHashVal
#define __prvTgfPclTrunkHashVal

#include <trafficEngine/tgfTrafficGenerator.h>

#include <common/tgfCommon.h>
#include <common/tgfPclGen.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* birst count */
#define PRV_TGF_PCL_BURST_COUNT_0_CNS         3

/* index of port in port-array for send port */
#define PRV_TGF_PCL_SEND_PORT_INDEX_CNS        1

/* index of port in port-array for receive (egress) port */
#define PRV_TGF_PCL_RECEIVE_PORT_INDEX_CNS     2

/* VID used in test */
#define PRV_TGF_PCL_TEST_VID0 5

/* modified VLAN ID*/
#define PRV_TGF_PCL_MODIFIED_VLANID_0_CNS     6

/* User Priorityes used in test */
#define PRV_TGF_PCL_TEST_UP0 0

/* CFI used in test */
#define PRV_TGF_PCL_TEST_CFI0 0

/*******************************************************************************
* prvTgfPclTrunkHashTrafficEgressVidCheck
*
* DESCRIPTION:
*       Checks traffic egress VID in the Tag
*
* INPUTS:
*       portIndex - port index
*       egressVid - VID found in egressed packets VLAN Tag
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
GT_VOID prvTgfPclTrunkHashTrafficEgressVidCheck
(
    IN GT_U32  portIndex,
    IN GT_U16  egressVid
);

/*******************************************************************************
* prvTgfPclTrunkHashTrafficTest
*
* DESCRIPTION:
*       Full Trunk Hash value test
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
GT_VOID prvTgfPclTrunkHashTrafficTest
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfPclTrunkHashVal */
