/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPclBypass.h
*
* DESCRIPTION:
*       Second Lookup match
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfPclBypass
#define __prvTgfPclBypass

#include <trafficEngine/tgfTrafficGenerator.h>

#include <common/tgfCommon.h>
#include <common/tgfPclGen.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * typedef: enumPRV_TGF_PCL_BYPASS_ENT 
 *
 * Description:
 *    Type of Bypass
 */
typedef enum
{
    PRV_TGF_PCL_BYPASS_BRIDGE_E,
    PRV_TGF_PCL_BYPASS_INGRESS_PIPE_E,

    PRV_TGF_PCL_BYPASS_MAX_NUMBER_E
} PRV_TGF_PCL_BYPASS_ENT;



/*******************************************************************************
* prvTgfPclBypass
*
* DESCRIPTION:
*       Set test configuration
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
GT_VOID prvTgfPclBypass
(
    IN PRV_TGF_PCL_BYPASS_ENT fieldIndex
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfPclBypass */
