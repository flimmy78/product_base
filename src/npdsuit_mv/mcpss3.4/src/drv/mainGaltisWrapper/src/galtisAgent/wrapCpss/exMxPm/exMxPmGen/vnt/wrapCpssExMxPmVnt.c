/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmVnt.c
*
* DESCRIPTION:
*       TODO: Add proper description of this file here
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/exMxPm/exMxPmGen/vnt/cpssExMxPmVnt.h>

/*******************************************************************************
* cpssExMxPmVntOamPortLoopbackModeEnableSet
*
* DESCRIPTION:
*      Enable/Disable 802.3ah Loopback mode on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port all Non-OAM-PDU packets
*      arriving on the port are:
*      - Passed transparently through the ingress and egress pipe.
*      - Redirected back to the source port on which it was received.
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*       enable   - GT_TRUE:  Enable OAM loopback mode.
*                  GT_FALSE: Disable OAM loopback mode.
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
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntOamPortLoopbackModeEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmVntOamPortLoopbackModeEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntOamPortLoopbackModeEnableGet
*
* DESCRIPTION:
*      Get 802.3ah Loopback mode(Enable/Disable) on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port all Non-OAM-PDU packets
*      arriving on the port are:
*      - Passed transparently through the ingress and egress pipe.
*      - Redirected back to the source port on which it was received.
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum      - device number
*       port        - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable OAM loopback mode.
*                     GT_FALSE: Disable OAM loopback mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntOamPortLoopbackModeEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmVntOamPortLoopbackModeEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntOamPortPduTrapEnableSet
*
* DESCRIPTION:
*      Enable/Disable trap to CPU of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs destined for this port are trapped to the CPU.
*      - Packets identified as OAM-PDUs destined for this port are passed transparently.
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
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
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntOamPortPduTrapEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmVntOamPortPduTrapEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntOamPortPduTrapEnableGet
*
* DESCRIPTION:
*      Get trap to CPU status(Enable/Disable) of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs are trapped to the CPU.
*      - Packets identified as OAM-PDUs are passed transparently.
*
* APPLICABLE DEVICES: ExMxPm and above.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*
*
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                     GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol..
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntOamPortPduTrapEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmVntOamPortPduTrapEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntOamPortUnidirectionalEnableSet
*
* DESCRIPTION:
*      Enable/disable Unidirectional transmit operation i.e. a mode to transmit 
*      packets even when the port’s receive link state is down.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number
*       enable   - GT_TRUE:  Enable Unidirectional transmit operation
*                  GT_FALSE: Disable Unidirectional transmit operation 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*  The Link fault signaling of the Reconciliation Sublayer is affected when the 
*  Unidirectional mode of operation is enabled, by allowing transmit of
*  MAC data even when Remote Fault is received.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntOamPortUnidirectionalEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmVntOamPortUnidirectionalEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntOamPortUnidirectionalEnableGet
*
* DESCRIPTION:
*      Enable/disable Unidirectional transmit operation i.e. a mode to transmit 
*      packets even when the port’s receive link 
*      state is down.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number 
*
* OUTPUTS:
*       enablePtr   - pointer to unidirectional transmit status:
*                     GT_TRUE:  Enable Unidirectional transmit operation
*                     GT_FALSE: Disable Unidirectional transmit operation
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntOamPortUnidirectionalEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmVntOamPortUnidirectionalEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntOamPortDropAllNonLocalTrafEnableSet
*
* DESCRIPTION:
*  Enable/disable drop of all traffic destined for this port that was not 
*  ingressed on this port.
*  This filter is used for loop back mode. when active the port transmit only 
*  packet target to this port and drop all other trafic exept some control trafic.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum   - device number
*       port     - port number including CPU port.
*       enable   - GT_TRUE:  Enable drop all non local traffic
*                  GT_FALSE: Disable drop all non local traffic
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntOamPortDropAllNonLocalTrafEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmVntOamPortDropAllNonLocalTrafEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntOamPortDropAllNonLocalTrafEnableGet
*
* DESCRIPTION:
*  Get drop status of all traffic destined for this port that was not 
*  ingressed on this port.
*  This filter is used for loop back mode. when active the port transmit only 
*  packet target to this port and drop all other trafic exept some control trafic.
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum   - device number
*       port     - port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to drop all non local traffic status:
*                     GT_TRUE:  Enable drop all non local traffic
*                     GT_FALSE: Disable drop all non local traffic
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntOamPortDropAllNonLocalTrafEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmVntOamPortDropAllNonLocalTrafEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntCfmLbmOpcodeSet
*
* DESCRIPTION:
*      Configure opcode for Loopback message (LBM) of Connectivity fault 
*      management service (CFM)
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum   - device number
*       opcode   - opcode (0 .. 255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - opcode out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*  
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntCfmLbmOpcodeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;    
    GT_U32 opcode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    opcode = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmVntCfmLbmOpcodeSet(devNum, opcode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntCfmLbmOpcodeGet
*
* DESCRIPTION:
*      Get opcode for Loopback message (LBM) of Connectivity fault management 
*      service (CFM)
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       opcodePtr   - pointer to opcode value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntCfmLbmOpcodeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 opcode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmVntCfmLbmOpcodeGet(devNum, &opcode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", opcode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntCfmLbrOpcodeSet
*
* DESCRIPTION:
*      Configure opcode for Loopback response (LBR) of Connectivity fault 
*      management service (CFM)
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum   - device number
*       opcode   - opcode (0 .. 255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - opcode out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntCfmLbrOpcodeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;    
    GT_U32 opcode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    opcode = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmVntCfmLbrOpcodeSet(devNum, opcode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntCfmLbrOpcodeGet
*
* DESCRIPTION:
*      Get cpu opcode for Loopback response (LBR) of Connectivity fault 
*      management service (CFM)
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       opcodePtr   - pointer to opcode value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntCfmLbrOpcodeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 opcode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmVntCfmLbrOpcodeGet(devNum, &opcode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", opcode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmVntLastReadTimeStampGet
*
* DESCRIPTION:
*      Get the time stamp assigned of the last read (see more comments below).
*
* APPLICABLE DEVICES: All ExMxPm Devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       timeStampPtr   - (pointer to) time stamp value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*   To calculate event rate, SNMP and RMON rate counters are usually read 
*   periodically. The period between two sequential reads is scheduled by 
*   software tasks and may not be known accurately. To determine the exact 
*   reading time, the device maintains a timestamp register that is updated by 
*   the real-time clock every time a counter or any internal register is read.
*   The real-time clock is a free-running counter that increments at the core 
*   frequency rate.The timestamp register records the core counter’s value 
*   during host CPU read or write access to any of the device’s registers, 
*   except for the timestamp register itself and for the PCI address space
*   configuration registers. When doing burst read, the first word of the burst 
*   triggers the timestamp counter. The host can read the timestamp register 
*   after reading an SNMP and RMON counter or other register, to retrieve the 
*   precise time the register was sampled.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVntLastReadTimeStampGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 timeStamp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmVntLastReadTimeStampGet(devNum, &timeStamp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", timeStamp);

    return CMD_OK;
}

/**** database initialization **************************************/


static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmVntOamPortLoopbackModeEnableSet",
         &wrCpssExMxPmVntOamPortLoopbackModeEnableSet,
         3, 0},
        {"cpssExMxPmVntOamPortLoopbackModeEnableGet",
         &wrCpssExMxPmVntOamPortLoopbackModeEnableGet,
         2, 0},
        {"cpssExMxPmVntOamPortPduTrapEnableSet",
         &wrCpssExMxPmVntOamPortPduTrapEnableSet,
         3, 0},
        {"cpssExMxPmVntOamPortPduTrapEnableGet",
         &wrCpssExMxPmVntOamPortPduTrapEnableGet,
         2, 0},

        {"cpssExMxPmVntOamPortUnidirectionalEnableSet",
         &wrCpssExMxPmVntOamPortUnidirectionalEnableSet,
         3, 0},
        {"cpssExMxPmVntOamPortUnidirectionalEnableGet",
         &wrCpssExMxPmVntOamPortUnidirectionalEnableGet,
         2, 0},

        {"cpssExMxPmVntOamPortDropAllNonLocalTrafEnableSet",
         &wrCpssExMxPmVntOamPortDropAllNonLocalTrafEnableSet,
         3, 0},
        {"cpssExMxPmVntOamPortDropAllNonLocalTrafEnableGet",
         &wrCpssExMxPmVntOamPortDropAllNonLocalTrafEnableGet,
         2, 0},

        {"cpssExMxPmVntCfmLbmOpcodeSet",
         &wrCpssExMxPmVntCfmLbmOpcodeSet,
         2, 0},
        {"cpssExMxPmVntCfmLbmOpcodeGet",
         &wrCpssExMxPmVntCfmLbmOpcodeGet,
         1, 0},

        {"cpssExMxPmVntCfmLbrOpcodeSet",
         &wrCpssExMxPmVntCfmLbrOpcodeSet,
         2, 0},
        {"cpssExMxPmVntCfmLbrOpcodeGet",
         &wrCpssExMxPmVntCfmLbrOpcodeGet,
         1, 0},

        {"cpssExMxPmVntLastReadTimeStampGet",
         &wrCpssExMxPmVntLastReadTimeStampGet,
         1, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmVnt
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitCpssExMxPmVnt
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

