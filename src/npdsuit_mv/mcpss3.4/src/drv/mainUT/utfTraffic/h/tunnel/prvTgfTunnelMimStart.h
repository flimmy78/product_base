/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTunnelMimStart.h
*
* DESCRIPTION:
*       Tunnel Start: Mac In Mac Functionality
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfTunnelMimStarth
#define __prvTgfTunnelMimStarth

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * typedef: enum PRV_TGF_TUNNEL_MIM_TYPE_ENT 
 *
 * Description:
 *    Type of MAC In MAC Tunnel Start Test
 */
typedef enum
{
    PRV_TGF_TUNNEL_MIM_START_FROM_TTI_E,
    PRV_TGF_TUNNEL_MIM_START_FROM_PCL_E,

    PRV_TGF_TUNNEL_MIM_START_TYPE_MAX_E
} PRV_TGF_TUNNEL_MIM_START_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_TUNNEL_MPLS_LSR_PUSH_TYPE_ENT 
 *
 * Description:
 *    Type of MPLS LSR PUSH Tunnel Start Test
 */
typedef enum
{
    PRV_TGF_TUNNEL_MPLS_LSR_PUSH_1_LABEL_E,
    PRV_TGF_TUNNEL_MPLS_LSR_PUSH_2_LABEL_E,
    PRV_TGF_TUNNEL_MPLS_LSR_PUSH_3_LABEL_E,
    PRV_TGF_TUNNEL_MPLS_LSR_SWAP_E,
    PRV_TGF_TUNNEL_MPLS_LSR_POP_SWAP_E,

    PRV_TGF_TUNNEL_MPLS_LSR_PUSH_TYPE_MAX_E
} PRV_TGF_TUNNEL_MPLS_LSR_PUSH_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_TUNNEL_MPLS_LSR_POP_TYPE_ENT 
 *
 * Description:
 *    Type of MPLS LSR POP Tunnel Start Test
 */
typedef enum
{
    PRV_TGF_TUNNEL_MPLS_LSR_POP_1_LABEL_E,
    PRV_TGF_TUNNEL_MPLS_LSR_POP_2_LABEL_E,

    PRV_TGF_TUNNEL_MPLS_LSR_POP_TYPE_MAX_E
} PRV_TGF_TUNNEL_MPLS_LSR_POP_TYPE_ENT;


/*******************************************************************************
* prvTgfTunnelMimStart
*
* DESCRIPTION:
*       MIM Tunnel Start
*
* INPUTS:
*       startType - Type of MAC in MAC Test 
*
* OUTPUTS:
*       None
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfTunnelMimStart
(
    IN PRV_TGF_TUNNEL_MIM_START_TYPE_ENT startType
);

/*******************************************************************************
* prvTgfTunnelMplsLsrPush
*
* DESCRIPTION:
*       MPLS LSR PUSH Functionality
*
* INPUTS:
*       pushType - Type of MPLS LSR PUSH Tunnel Start Test 
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
GT_VOID prvTgfTunnelMplsPush
(
    IN PRV_TGF_TUNNEL_MPLS_LSR_PUSH_TYPE_ENT pushType
);

/*******************************************************************************
* prvTgfTunnelMplsLsrPop
*
* DESCRIPTION:
*       MPLS LSR POP Functionality
*
* INPUTS:
*       popType - Type of MPLS LSR POP Tunnel Start Test 
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
GT_VOID prvTgfTunnelMplsPop
(
    IN PRV_TGF_TUNNEL_MPLS_LSR_POP_TYPE_ENT popType
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfTunnelMimStarth */

