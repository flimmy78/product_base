/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfVntGen.c
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

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <utf/private/prvUtfHelpers.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#include <trafficEngine/tgfTrafficEngine.h>
#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfVntGen.h>
#include <common/tgfPclGen.h>

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
)
{
    GT_U32  portIter = 0;


    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == portNum)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != portNum)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", portNum);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChVntOamPortLoopBackModeEnableSet(prvTgfDevsArray[portIter], portNum, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmVntOamPortLoopbackModeEnableSet(prvTgfDevsArray[portIter], portNum, enable);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
    GT_U32  portIter = 0;
    

    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == portNum)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != portNum)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", portNum);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChVntOamPortLoopBackModeEnableGet(prvTgfDevsArray[portIter], portNum, enablePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmVntOamPortLoopbackModeEnableGet(prvTgfDevsArray[portIter], portNum, enablePtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
    GT_U32  portIter = 0;


    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == portNum)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != portNum)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", portNum);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChVntOamPortUnidirectionalEnableSet(prvTgfDevsArray[portIter], portNum, enable);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmVntOamPortUnidirectionalEnableSet(prvTgfDevsArray[portIter], portNum, enable);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
    GT_U32  portIter = 0;
    

    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == portNum)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != portNum)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", portNum);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChVntOamPortUnidirectionalEnableGet(prvTgfDevsArray[portIter], portNum, enablePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* call device specific API */
    return cpssExMxPmVntOamPortUnidirectionalEnableGet(prvTgfDevsArray[portIter], portNum, enablePtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
#ifdef CHX_FAMILY
    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);
    
    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChVntCfmEtherTypeSet(devNum, etherType);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChVntCfmEtherTypeSet FAILED, rc = [%d]", rc);
    
            rc1 = rc;
        }
    }

    if(rc1 != GT_OK)
    {
        return rc1;
    }

    /* set also UDE0 for the PCL */
    return prvTgfPclUdeEtherTypeSet(0/*UDE0*/,etherType);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* avoid warnings */
    TGF_PARAM_NOT_USED(etherType);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChVntCfmEtherTypeGet(devNum, etherTypePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
     /* avoid warnings */
    TGF_PARAM_NOT_USED(devNum);
    TGF_PARAM_NOT_USED(etherTypePtr);

    return GT_NOT_IMPLEMENTED;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{

    GT_U8       devNum  = 0;
    GT_STATUS   rc, rc1 = GT_OK;

#ifdef CHX_FAMILY
    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(&devNum, rc, UTF_NONE_FAMILY_E);
    
    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssDxChVntCfmLbrOpcodeSet(devNum, opcode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssDxChVntCfmLbrOpcodeSet FAILED, rc = [%d]", rc);
    
            rc1 = rc;
        }
    }

    return rc1;
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
    /* prepare device iterator */
    rc = prvUtfNextDeviceReset(&devNum, UTF_EXMXPM_FAMILY_SET_CNS);
    if (GT_OK != rc)
    {
        PRV_UTF_LOG1_MAC("[TGF]: prvUtfNextDeviceReset FAILED, rc = [%d]", rc);

        return rc;
    }
    
    /* go over all active devices */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        /* call device specific API */
        rc = cpssExMxPmVntCfmLbrOpcodeSet(devNum, opcode);
        if (GT_OK != rc)
        {
            PRV_UTF_LOG1_MAC("[TGF]: cpssExMxPmVntCfmLbrOpcodeSet FAILED, rc = [%d]", rc);
    
            rc1 = rc;
        }
    }

    return rc1;
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChVntCfmLbrOpcodeGet(devNum, opcodePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
     /* call device specific API */
    return cpssExMxPmVntCfmLbrOpcodeGet(devNum, opcodePtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}

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
)
{
#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChVntLastReadTimeStampGet(devNum, timeStampValuePtr);
#endif /* CHX_FAMILY */

#ifdef EXMXPM_FAMILY
     /* call device specific API */
    return cpssExMxPmVntLastReadTimeStampGet(devNum, timeStampValuePtr);
#endif /* EXMXPM_FAMILY */

#if !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY)
    return GT_BAD_STATE;
#endif /* !(defined CHX_FAMILY) && !(defined EXMXPM_FAMILY) */
}


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
)
{
    GT_U32  portIter = 0;

    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == portNum)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != portNum)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", portNum);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChVntOamPortPduTrapEnableSet(prvTgfDevsArray[portIter], portNum, enable);
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) 
    /* call device specific API */
    return cpssExMxPmVntOamPortPduTrapEnableSet(prvTgfDevsArray[portIter], portNum, enable);
#endif /* !(defined CHX_FAMILY) */
}

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
)
{
    GT_U32  portIter = 0;
    

    /* find port index */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        if (prvTgfPortsArray[portIter] == portNum)
        {
            break;
        }
    }

    /* check if port is valid */
    if (prvTgfPortsArray[portIter] != portNum)
    {
        PRV_UTF_LOG1_MAC("[TGF]: Port [%d] is not valid", portNum);

        return GT_BAD_PARAM;
    }

#ifdef CHX_FAMILY
    /* call device specific API */
    return cpssDxChVntOamPortPduTrapEnableGet(prvTgfDevsArray[portIter], portNum, enablePtr);
#endif /* CHX_FAMILY */

#if !(defined CHX_FAMILY) 
    /* call device specific API */
    return cpssExMxPmVntOamPortPduTrapEnableGet(prvTgfDevsArray[portIter], portNum, enablePtr);
#endif /* !(defined CHX_FAMILY) */
}

