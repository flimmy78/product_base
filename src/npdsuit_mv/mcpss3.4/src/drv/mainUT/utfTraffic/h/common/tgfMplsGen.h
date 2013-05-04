/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfMplsGen.h
*
* DESCRIPTION:
*       Generic API for MPLS
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfMplsGenh
#define __tgfMplsGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/

/*
 * typedef: enum PRV_TGF_MPLS_CMD_ENT
 *
 * Description: Enumeration of MPLS command assigned to the packet
 *
 * Enumerations:
 *  PRV_TGF_MPLS_NOP_CMD_E  - Do nothing
 *  PRV_TGF_MPLS_SWAP_CMD_E - Swap the most outer label with another Label
 *  PRV_TGF_MPLS_PUSH_CMD_E - Add a new label on top of the MPLS labels stack
 *  PRV_TGF_MPLS_PHP_CMD_E  - Forwarding decision is based on the popped label
 *
 */
typedef enum {
    PRV_TGF_MPLS_NOP_CMD_E,
    PRV_TGF_MPLS_SWAP_CMD_E,
    PRV_TGF_MPLS_PUSH_CMD_E,
    PRV_TGF_MPLS_PHP_CMD_E
} PRV_TGF_MPLS_CMD_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfMplsGenh */

