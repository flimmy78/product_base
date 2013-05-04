/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfVlanFloodVidx.h
*
* DESCRIPTION:
*       VLAN Flood VIDX
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfVlanFloodVidx
#define __prvTgfVlanFloodVidx

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfBrgVlanFloodVidxConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Configure floodVidxMode = UNREG MC on VLAN entry (VID = 5). 
*           Configure floodVidx = 10 on VLAN entry (VID = 5).
*           Build MC Group (devNum = 0, vidx = 10, portBitmapPtr[0] = 0x101)
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
GT_VOID prvTgfBrgVlanFloodVidxConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanFloodVidxTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 01:01:01:01:01:01,
*               macSa = 00:00:00:00:00:05,
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:05,
*           Success Criteria:
*               MC Packet is captured on port 8
*               BC Packet is captured on ports 8, 18, 23
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
GT_VOID prvTgfBrgVlanFloodVidxTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanFloodVidxConfigurationRestore
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
GT_VOID prvTgfBrgVlanFloodVidxConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanFloodVidxAllFloodedConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*           Configure floodVidxMode = ALL_FLOODED_TRAFFIC on VLAN entry (VID = 5). 
*           Configure floodVidx = 4094 on VLAN entry (VID = 5).
*           Build MC Group (devNum = 0, vidx = 10, portBitmapPtr[0] = 0x101)
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
GT_VOID prvTgfBrgVlanFloodVidxAllFloodedConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanFloodVidxAllFloodedTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 01:01:01:01:01:01,
*               macSa = 00:00:00:00:00:05,
*           Send to device's port 0 packet:
*               macDa = FF:FF:FF:FF:FF:FF,
*               macSa = 00:00:00:00:00:05,
*           Success Criteria:
*               MC Packet is captured on port 8
*               BC Packet is captured on port 8
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
GT_VOID prvTgfBrgVlanFloodVidxAllFloodedTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanFloodVidxAllFloodedConfigurationRestore
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
GT_VOID prvTgfBrgVlanFloodVidxAllFloodedConfigurationRestore
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfVlanFloodVidx */

