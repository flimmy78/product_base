/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfBrgRateLimitMcastReg.h
*
* DESCRIPTION:
*       Bridge Generic Port Rate Limit for Multicast Registered traffic.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfBrgRateLimitMcastReg
#define __prvTgfBrgRateLimitMcastReg

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Create VLAN 2 on all ports (0,8,18,23)
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
GT_VOID prvTgfBrgGenRateLimitMcastRegConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegEnableRegTrafficGenerate
*
* DESCRIPTION:
*           Enable Rate Limit on port port2 for multicast registered packets.
*           Create vidx 0xF with port members port0 and port1.
*           Configure MAC 01:22:33:44:55:66 on vidx 0xF 
*           Set window size to maximum and drop command to hard
*           Set rate limit to 1.
*       Generate traffic:
*           Send from port2 single tagged packet:
*               macDa = 01:22:33:44:55:66
*               macSa = 00:00:00:00:22:22
*           Success Criteria:
*               One packet is received on port0 and port1 and one packet is 
*               dropped.
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
GT_VOID prvTgfBrgGenRateLimitMcastRegEnableRegTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegDisableRegTrafficGenerate
*
* DESCRIPTION:
*           Disable Rate Limit on port port2 for multicast registered packets.
*           Create vidx 0xF with port members port0 and port1.
*           Configure MAC 01:22:33:44:55:66 on vidx 0xF 
*           Set window size to maximum and drop command to hard
*           Set rate limit to 1.
*       Generate traffic:
*           Send from port2 single tagged packet:
*               macDa = 01:22:33:44:55:66
*               macSa = 00:00:00:00:22:22
*           Success Criteria:
*               Two packets are received on port0 and port1 and no packet is 
*               dropped.
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
GT_VOID prvTgfBrgGenRateLimitMcastRegDisableRegTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegEnableUnregTrafficGenerate
*
* DESCRIPTION:
*           Enable Rate Limit on port port2 for multicast registered packets.
*           Create vidx 0xF with port members port0 and port1.
*           Configure MAC 01:22:33:44:55:66 on vidx 0xF 
*           Set window size to maximum and drop command to hard
*           Set rate limit to 1.
*       Generate traffic:
*           Send from port2 single tagged packet:
*               macDa = 01:77:88:99:AA:BB
*               macSa = 00:00:00:00:22:22
*           Success Criteria:
*               Two packets are received on port0 and port1 and no packet is 
*               dropped.
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
GT_VOID prvTgfBrgGenRateLimitMcastRegEnableUnregTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegDisableUnregEnableRegTrafficGenerate
*
* DESCRIPTION:
*           Disable Rate Limit on port port2 for multicast registered packets.
*           Enable Rate Limit on port port2 for multicast unregistered packets.
*           Create vidx 0xF with port members port0 and port1.
*           Configure MAC 01:22:33:44:55:66 on vidx 0xF 
*           Set window size to maximum and drop command to hard
*           Set rate limit to 1.
*       Generate traffic:
*           Send from port2 single tagged packet:
*               macDa = 01:22:33:44:55:66
*               macSa = 00:00:00:00:22:22
*           Success Criteria:
*               Both packets are received on port0 and port1 and no packet is 
*               dropped.
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
GT_VOID prvTgfBrgGenRateLimitMcastRegDisableUnregEnableRegTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgfBrgGenRateLimitMcastRegConfigurationRestore
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
GT_VOID prvTgfBrgGenRateLimitMcastRegConfigurationRestore
(
    void
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfBrgRateLimitMcastReg */


