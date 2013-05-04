/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfNstGen.h
*
* DESCRIPTION:
*       Generic API for Nst
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfNstGenh
#define __tgfNstGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/


/*
 * typedef: enum PRV_TGF_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT
 *
 * Description: Enumeration for L2 or/and L3 traffic type
 *
 * Enumerations:
 *    PRV_TGF_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E - Port isolation for L2
 *                                         packets (L2 Port Isolation table)
 *    PRV_TGF_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E - Port isolation for L3
 *                                         packets (L3 Port Isolation table)
 *
 */
typedef enum
{
   PRV_TGF_NST_PORT_ISOLATION_TRAFFIC_TYPE_L2_E,
   PRV_TGF_NST_PORT_ISOLATION_TRAFFIC_TYPE_L3_E
}PRV_TGF_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT;

/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/
/*******************************************************************************
* prvTgfNstPortIsolationEnableSet
*
* DESCRIPTION:
*       Function enables/disables the port isolation feature.
*
* INPUTS:
*       devNum    - device number
*       enable    - port isolation feature enable/disable
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfNstPortIsolationEnableSet
(
    IN  GT_U8                devNum,
    IN  GT_BOOL              enable
);

/*******************************************************************************
* prvTgfNstPortIsolationTableEntrySet
*
* DESCRIPTION:
*       Function sets port isolation table entry. 
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking. If it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interface.
*                              Only portDev and Trunk are supported.
*       cpuPortMember        - port isolation for CPU Port
*                               GT_TRUE - member
*                               GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written to the
*                              L2/L3 PI table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface or 
*                                  localPortsMembersPtr
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may 
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
GT_STATUS prvTgfNstPortIsolationTableEntrySet
(
    IN GT_U8                                        devNum,
    IN PRV_TGF_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType,
    IN CPSS_INTERFACE_INFO_STC                      srcInterface,
    IN GT_BOOL                                      cpuPortMember,                      
    IN CPSS_PORTS_BMP_STC                           *localPortsMembersPtr
);

/******************************************************************************\
 *                       API for default settings                             *
\******************************************************************************/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfNstGenh */

