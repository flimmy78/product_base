/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfVlanManipulation.h
*
* DESCRIPTION:
*       VLAN Manipulation
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfVlanManipulation
#define __prvTgfVlanManipulation

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "Untagged" tagging command for all ports in VLAN.
*         - Build VLAN Entry with VID = 10 and portsMember = 0, 8, 18, 23 and
*           tagging command "TAG0" for all ports in VLAN to be sure that
*           decision is according to VID = 5.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdDoubleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 10
*           Success Criteria:
*               Output packet has no any tags
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdDoubleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdDoubleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "Untagged" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdSingleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has no any tags
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdSingleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdSingleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "Untagged" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdUntagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12
*           Success Criteria:
*               Output packet has no any tags
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdUntagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationUntaggedCmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationUntaggedCmdUntagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 25 and portsMember = 0, 8, 18, 23 
*           and "TAG0" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdDoubleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 25
*               TAG1:  ethertype = 0x88a8, vlan = 4095
*           Success Criteria:
*               Output packet has one tag TAG0 with
*               ethertype = 0x9100, vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdDoubleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdDoubleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "TAG0" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdSingleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has one tag TAG0 with
*               ethertype = 0x9100, vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdSingleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdSingleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "TAG0" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdUntagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12
*           Success Criteria:
*               Output packet has one tag TAG0 with
*               ethertype = 0x9100, vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdUntagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag0CmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag0CmdUntagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "TAG1" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdDoubleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 10
*           Success Criteria:
*               Output packet has one tag TAG1 with
*               ethertype = 0x98a8, vlan = 10
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdDoubleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdDoubleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "TAG1" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdSingleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has one tag TAG1 with
*               ethertype = 0x98a8, vlan = 10
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdSingleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdSingleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "TAG1" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdUntagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12
*           Success Criteria:
*               Output packet has one tag TAG1 with
*               ethertype = 0x98a8, vlan = 10
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdUntagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationTag1CmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationTag1CmdUntagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Outer Tag0, Inner Tag1" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG1:  ethertype = 0x88a8, vlan = 10
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has two tags in the following order: TAG0 with
*               ethertype = 0x9100, vlan = 5 and TAG1 with ethertype = 0x98a8,
*               vlan = 10
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdDoubleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Outer Tag0, Inner Tag1" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has two tags in the following order: TAG0 with
*               ethertype = 0x9100, vlan = 5 and TAG1 with ethertype = 0x98a8,
*               vlan = 0
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdSingleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Outer Tag0, Inner Tag1" for all ports in VLAN.
*         - Set PVID to 5 - for untagged packet
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Output packet has two tags in the following order: TAG0 with
*               ethertype = 0x9100, vlan = 5 and TAG1 with ethertype = 0x98a8,
*               vlan = 0
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag0InnerTag1CmdUntagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "Outer Tag1, Inner Tag0" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG1:  ethertype = 0x88a8, vlan = 10
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has two tags in the following order: TAG1 with 
*               ethertype = 0x98a8, vlan = 10 and TAG0 with ethertype = 0x9100,
*               vlan = 5.
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdDoubleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Outer Tag1, Inner Tag0" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has two tags in the following order: TAG1 with
*               ethertype = 0x98a8, vlan = 0 and TAG0 with ethertype = 0x9100,
*               vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdSingleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Outer Tag1, Inner Tag0" for all ports in VLAN.
*         - Set PVID to 5 - for untagged packet
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Output packet has two tags in the following order: TAG1 with
*               ethertype = 0x98a8, vlan = 0 and TAG0 with ethertype = 0x9100,
*               vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationOuterTag1InnerTag0CmdUntagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 and 
*           tagging command "Push Tag0" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdDoubleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 10
*           Success Criteria:
*               Output packet has tree tags in the following order: TAG0 with 
*               ethertype = 0x9100, vlan = 5 and TAG0 with ethertype = 0x8100,
*               vlan = 5, TAG1 with ethertype = 0x88a8, vlan = 10.
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdDoubleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdDoubleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Push Tag0" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdSingleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet has two tags in the following order: TAG0 with
*               ethertype = 0x9100, vlan = 5 and TAG0 with ethertype = 0x8100,
*               vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdSingleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdSingleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdUntagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Push Tag0" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdUntagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdUntagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*           Success Criteria:
*               Output packet has one tag: TAG0 with ethertype = 0x9100, vlan = 5
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdUntagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPushTag0CmdUntagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationPushTag0CmdUntagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 and
*           tagging command "Pop Outer Tag" for all ports in VLAN.
*         - Build VLAN Entry with VID = 10 and portsMember = 0, 8, 18, 23 and
*           tagging command "UNTAGGED" for all ports in VLAN to be sure that
*           decision is according to VID = 5.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 10
*           Success Criteria:
*                Output packet has one tag: TAG1 with 
*                ethertype = 0x98a8,vlan = 10.
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdDoubleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to: Ethertype0 = 0x8100, Ethertype1 = 0x88a8
*         - Set ingress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23
*           and tagging command "Pop Outer Tag" for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 - is Ethertype1.
*
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*           Success Criteria:
*               Output packet is untagged
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationPopOuterTagCmdSingleTagConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment1ConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to:
*                       Ethertype0 = 0x8100,
*                       Ethertype1 = 0x88a8,
*                       Ethertype2 = 0x5000,
*                       Ethertype3 = 0xA0A0,
*                       Ethertype4 = 0x5050,
*                       Ethertype5 = 0x2525,
*                       Ethertype6 = 0x5555,
*                       Ethertype7 = 0xAAAA,
*         - Set ingress TPID select to: TAG0 is Ethertype0, Ethertype3, Ethertype5,
*           Ethertype6, Ethertype7, TAG1 is Ethertype1, Ethertype2, Ethertype4.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "TAG0" tagging command for all ports in VLAN.
*         - Build VLAN Entry with VID = 25 and portsMember = 0, 8, 18, 23 
*           and "TAG0" tagging command for all ports in VLAN.
*         - Build VLAN Entry with VID = 4000 and portsMember = 0, 8, 18, 23 
*           and "TAG0" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment1ConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment1TrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:03,
*               macSa = 00:00:00:00:00:13,
*               TAG0:  ethertype = 0x8100, vlan = 25
*
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:04,
*               macSa = 00:00:00:00:00:14,
*               TAG0:  ethertype = 0x8100, vlan = 4000
*
*           Success Criteria:
*               First  packet have TAG0:  ethertype =0x9100, vlan = 5
*               Second packet have TAG0:  ethertype =0x9100, vlan = 25
*               Third  packet have TAG0:  ethertype =0x9100, vlan = 4000
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment1TrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment1ConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment1ConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment2ConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to:
*                       Ethertype0 = 0x8100,
*                       Ethertype1 = 0x88a8,
*                       Ethertype2 = 0x5000,
*                       Ethertype3 = 0xA0A0,
*                       Ethertype4 = 0x5050,
*                       Ethertype5 = 0x2525,
*                       Ethertype6 = 0x5555,
*                       Ethertype7 = 0xAAAA,
*         - Set ingress TPID select to: TAG0 is Ethertype0, Ethertype3, Ethertype5,
*           Ethertype6, Ethertype7, TAG1 is Ethertype1, Ethertype2, Ethertype4.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "TAG1" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment2ConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment2TrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 25
*
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:03,
*               macSa = 00:00:00:00:00:13,
*               TAG0:  ethertype = 0xA0A0, vlan = 25
*               TAG1:  ethertype = 0x5050, vlan = 100
*
*           Success Criteria:
*               First  packet have TAG1:  ethertype =0x98a8, vlan = 25
*               Second packet have TAG1:  ethertype =0x98a8, vlan = 100
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment2TrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationIngressVlanAssignment2ConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationIngressVlanAssignment2ConfigurationRestore
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationChangeEgressTpidConfigurationSet
*
* DESCRIPTION:
*       Set test configuration:
*         - Set ingress TPID table to:
*                       Ethertype0 = 0x8100,
*                       Ethertype1 = 0x88a8,
*                       Ethertype2 = 0x5000,
*                       Ethertype3 = 0xA0A0,
*                       Ethertype4 = 0x5050,
*                       Ethertype5 = 0x2525,
*                       Ethertype6 = 0x5555,
*                       Ethertype7 = 0xAAAA,
*         - Set ingress TPID select to: TAG0 is Ethertype0, Ethertype3, Ethertype5,
*           Ethertype6, Ethertype7, TAG1 is Ethertype1, Ethertype2, Ethertype4.
*         - Build VLAN Entry with VID = 5 and portsMember = 0, 8, 18, 23 
*           and "Outer Tag0, Inner Tag1" tagging command for all ports in VLAN.
*         - Set egress TPID table to: Ethertype0 = 0x9100, Ethertype1 = 0x98a8
*         - Set egress TPID select to: TAG0 is Ethertype0, TAG1 is Ethertype1.
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
GT_VOID prvTgfBrgVlanManipulationChangeEgressTpidConfigurationSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationChangeEgressTpidTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic:
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 100
*
*       Additional configuration:
*           Set egress TPID table to: Ethertype0 = 0x2222 and Ethertype1 = 0x3333
*
*           Send to device's port 0 packet:
*               macDa = 00:00:00:00:00:02,
*               macSa = 00:00:00:00:00:12,
*               TAG0:  ethertype = 0x8100, vlan = 5
*               TAG1:  ethertype = 0x88a8, vlan = 100
*
*           Success Criteria:
*               First  packet have TAG0:  ethertype =0x9100, vlan = 5;
*                                  TAG1:  ethertype =0x98a8, vlan = 100;
*               Second packet have TAG0:  ethertype =0x2222, vlan = 5;
*                                  TAG1:  ethertype =0x3333, vlan = 100;
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
GT_VOID prvTgfBrgVlanManipulationChangeEgressTpidTrafficGenerate
(
    GT_VOID
);

/*******************************************************************************
* prvTgfBrgVlanManipulationChangeEgressTpidConfigurationRestore
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
GT_VOID prvTgfBrgVlanManipulationChangeEgressTpidConfigurationRestore
(
    GT_VOID
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfVlanManipulation */

