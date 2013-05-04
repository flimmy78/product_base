/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfVntGen.h
*
* DESCRIPTION:
*       Generic API for Virtual Network Tester (VNT) Technology facility API.
*       VNT features:
*       - Operation, Administration, and Maintenance (OAM).
*       - Connectivity Fault Management (CFM).
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __tgfVntGenh
#define __tgfVntGenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/vnt/cpssDxChVnt.h>
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/vnt/cpssExMxPmVnt.h>
#endif /* EXMXPM_FAMILY */


/******************************************************************************\
 *                       CPSS generic API section                             *
\******************************************************************************/

/*******************************************************************************
* prvTgfVntOamPortLoopBackModeEnableSet
*
* DESCRIPTION:
*      Enable/Disable 802.3ah Loopback mode on the specified port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number including CPU port
*       enable  - enable\disable OAM loopback mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or port
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfVntOamPortLoopBackModeEnableSet
(
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfVntOamPortLoopBackModeEnableGet
*
* DESCRIPTION:
*      Get 802.3ah Loopback mode on the specified port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number including CPU port
*
* OUTPUTS:
*       enablePtr - (pointer to) Loopback mode status
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or port
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfVntOamPortLoopBackModeEnableGet
(
    IN  GT_U8                         portNum,
    OUT GT_BOOL                      *enablePtr
);

/*******************************************************************************
* prvTgfVntOamPortUnidirectionalEnableSet
*
* DESCRIPTION:
*      Enable/Disable the port for unidirectional transmit
*
* INPUTS:
*       devNum  - device number
*       portNum - port number including CPU port
*       enable  - enable\disable the port for unidirectional transmit
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or port
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS prvTgfVntOamPortUnidirectionalEnableSet
(
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfVntOamPortUnidirectionalEnableGet
*
* DESCRIPTION:
*      Gets the current status of unidirectional transmit for the port
*
* INPUTS:
*       devNum  - device number
*       portNum - port number including CPU port
*
* OUTPUTS:
*       enablePtr - (pointer to) current status of unidirectional transmit
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum or port
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS prvTgfVntOamPortUnidirectionalEnableGet
(
    IN  GT_U8                         portNum,
    OUT GT_BOOL                      *enablePtr
);

/*******************************************************************************
* prvTgfVntCfmEtherTypeSet
*
* DESCRIPTION:
*       Sets the EtherType to identify CFM (Connectivity Fault Management) PDUs
*
* INPUTS:
*       devNum    - device number
*       etherType - CFM (Connectivity Fault Management) EtherType
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_OUT_OF_RANGE - on wrong etherType values
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS prvTgfVntCfmEtherTypeSet
(
    IN GT_U32                         etherType
);

/*******************************************************************************
* prvTgfVntCfmEtherTypeGet
*
* DESCRIPTION:
*       Gets the EtherType to identify CFM (Connectivity Fault Management) PDUs
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       etherTypePtr - (pointer to) CFM (Connectivity Fault Management) EtherType
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS prvTgfVntCfmEtherTypeGet
(
    IN  GT_U8                         devNum,
    OUT GT_U32                       *etherTypePtr
);

/*******************************************************************************
* prvTgfVntCfmLbrOpcodeSet
*
* DESCRIPTION:
*      Sets the CFM (Connectivity Fault Management) LBR (Loopback Response) opcode
*
* INPUTS:
*       devNum - device number
*       opcode - CFM LBR Opcode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong devNum, opcode.
*       GT_OUT_OF_RANGE - on wrong opcode values.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS prvTgfVntCfmLbrOpcodeSet
(
    IN GT_U32                         opcode
);

/*******************************************************************************
* prvTgfVntCfmLbrOpcodeGet
*
* DESCRIPTION:
*      Gets the current CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       opcodePtr - (pointer to) CFM LBR Opcode
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS prvTgfVntCfmLbrOpcodeGet
(
    IN  GT_U8                         devNum,
    OUT GT_U32                       *opcodePtr
);

/*******************************************************************************
* prvTgfVntLastReadTimeStampGet
*
* DESCRIPTION:
*      Gets the last-read-time-stamp counter value that represent the exact
*      timestamp of the last read operation to the PP registers
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       timeStampValuePtr - (pointer to) timestamp of the last read operation
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong devNum
*       GT_BAD_PTR   - on one of the parameters is NULL pointer
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS prvTgfVntLastReadTimeStampGet
(
    IN  GT_U8                         devNum,
    OUT GT_U32                       *timeStampValuePtr
);

/*******************************************************************************
* prvTgfVntOamPortPduTrapEnableSet
*
* DESCRIPTION:
*      Enable/Disable trap to CPU of 802.3ah Link Layer Control protocol on
*      the specified port.
*
*
* INPUTS:
*       devNum   - device number
*       port     - virtual port number including CPU port.
*       enable   - GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                  GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol.
*
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfVntOamPortPduTrapEnableSet
(
    IN GT_U8                          portNum,
    IN GT_BOOL                        enable
);

/*******************************************************************************
* prvTgfVntOamPortPduTrapEnableGet
*
* DESCRIPTION:
*      Get trap to CPU status(Enable/Disable) of 802.3ah Link Layer Control protocol on
*      the specified port.
*
* INPUTS:
*       devNum   - device number
*       port     - virtual port number including CPU port.
*
*
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                     GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfVntOamPortPduTrapEnableGet
(
    IN  GT_U8                         portNum,
    OUT GT_BOOL                      *enablePtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfVntGenh */

