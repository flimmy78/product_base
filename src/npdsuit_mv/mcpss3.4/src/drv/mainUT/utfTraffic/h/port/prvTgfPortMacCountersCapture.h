/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPortMacCountersCapture.h
*
* DESCRIPTION:
*       Specific Port MAC Counters features testing
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfPortMacCountersCaptureh
#define __prvTgfPortMacCountersCaptureh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfPortMacCountersVlanConfigurationSet
*
* DESCRIPTION:
*       Set Default Vlan Port MAC Counters configuration.
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
GT_VOID prvTgfPortMacCountersVlanConfigurationSet
(
    GT_VOID    
);

/*******************************************************************************
* prvTgfPortMacCountersConfigurationSet
*
* DESCRIPTION:
*       Set configuration.
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
GT_VOID prvTgfPortMacCountersConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfPortMacCountersRestore
*
* DESCRIPTION:
*       None
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
GT_VOID prvTgfPortMacCountersRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgPortMacCounterCaptureBroadcastTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0packets:
*               macDa = ff:ff:ff:ff:ff:ff,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               MAC MIB Captured counters:
*               1 Broadcast Packet sent from port 0.
*               1 Broadcast Packets received on ports 8, 18 and 23.
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
GT_VOID prvTgPortMacCounterCaptureBroadcastTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgPortMacCounterCaptureUnicastTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0packets:
*               macDa = 00:00:00:00:11:11,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               MAC MIB Captured counters:
*               1 Unicast Packet sent from port 18.
*               1 Unicast Packets received on ports 0, 8 and 23.
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
GT_VOID prvTgPortMacCounterCaptureUnicastTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgPortMacCounterCaptureMulticastTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0packets:
*               macDa = 01:02:03:04:05:06,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               MAC MIB Captured counters:
*               1 Multicast Packet sent from port 0.
*               1 Multicast Packets received on ports 8, 18 and 23.
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
GT_VOID prvTgPortMacCounterCaptureMulticastTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgPortMacCounterCaptureOversizeTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0 packets:
*               macDa = 00:00:00:00:11:11,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               MAC MIB Captured counters:
*               1 Unicast Packet sent from port 0.
*               1 Oversize Packets received on ports 8, 18 and 23.
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
GT_VOID prvTgPortMacCounterCaptureOversizeTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgPortMacCounterCaptureHistogramTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0packets:
*               macDa = 01:02:03:04:05:06,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               Histogram Captured counters:
*               Check, that when the feature is disabled, the counters are not updated.
*               Check, that when the feature is enabled, the counters are updated.
*               Check for different packet sizes: 
*                   packetSize = 64.
*                   64<packetSize<128
*                   127<packetSize<256
*                   255<packetSize<512
*                   511<packetSize<1024
*                   1023<packetSize
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
GT_VOID prvTgPortMacCounterCaptureHistogramTrafficGenerate
(
    void
);

/*******************************************************************************
* prvTgPortMacCounterClearOnReadTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send from port 0 packets:
*               macDa = 00:00:00:00:11:11,
*               macSa = 00:00:00:00:22:22,
*           Success Criteria:
*               MAC MIB counters:
*               Check that when the feature enabled, counters are cleared on read.
*               Check that when the feature disabled, counters are not cleared on read.
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
GT_VOID prvTgPortMacCounterClearOnReadTrafficGenerate
(
    void
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfPortMacCountersCapture */


