/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmFabricVoq.c
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
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricVoq.h>

/*******************************************************************************
* cpssExMxPmFabricVoqFportTxEnableSet
*
* DESCRIPTION:
*     This routine enables/disables dequeue packets of the specified
*     fabric port VOQs
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum     - the device number to set.
*       voqUnitNum - the VOQ Unit number, 0..1
*       fport      - fabric port to enable/disable dequeueing
*                    CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS for MC packets queue
*       enable     - GT_TRUE dequeue on fabric port enable,
*                    GT_FALSE dequeue on fabric port disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum or fport.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqFportTxEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_FPORT fport;
    GT_BOOL enable ;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    fport=(GT_FPORT)inArgs[2];
    enable=(GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqFportTxEnableSet(devNum, voqUnitNum, fport, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqFportTxEnableGet
*
* DESCRIPTION:
*     This routine gets enable/disable of dequeueing packets of the specified
*     fabric port VOQs
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum     - the device number to set.
*       voqUnitNum - the VOQ Unit number, 0..1
*       fport      - fabric port to enable/disable dequeueing
*                    CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS for MC packets queue
*
* OUTPUTS:
*       enablePtr  - GT_TRUE dequeue on fabric port enable,
*                    GT_FALSE dequeue on fabric port disable.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum or fport.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqFportTxEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_FPORT fport;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    fport=(GT_FPORT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqFportTxEnableGet(devNum, voqUnitNum, fport, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqDevTblSet
*
* DESCRIPTION:
*       This routine enables/disables  target devices according to bitmap array
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum        - the device number.
*       voqUnitNum    - the VOQ Unit number, 0..1
*       trgDevsBmpArr - full 128-bit bitmap of target devices
*                       value 0 - disable, 1 - enable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqDevTblSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 trgDevsBmpArr[4];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    trgDevsBmpArr[0]=(GT_U32)inArgs[2];
    trgDevsBmpArr[1]=(GT_U32)inArgs[3];
    trgDevsBmpArr[2]=0;
    trgDevsBmpArr[3]=0;

    /* call cpss api function */
    result = cpssExMxPmFabricVoqDevTblSet(devNum, voqUnitNum, trgDevsBmpArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqDevTblSet
*
* DESCRIPTION:
*       This routine enables/disables  target devices according to bitmap array
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum        - the device number.
*       voqUnitNum    - the VOQ Unit number, 0..1
*       trgDevsBmpArr - full 128-bit bitmap of target devices
*                       value 0 - disable, 1 - enable
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqDevTblSet_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 trgDevsBmpArr[4];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    trgDevsBmpArr[0]=(GT_U32)inArgs[2];
    trgDevsBmpArr[1]=(GT_U32)inArgs[3];
    trgDevsBmpArr[2]=(GT_U32)inArgs[4];
    trgDevsBmpArr[3]=(GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqDevTblSet(devNum, voqUnitNum, trgDevsBmpArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqDevEnableSet
*
* DESCRIPTION:
*       This routine enables/disables the target device in the VOQ target
*       device table.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum     - the device number.
*       voqUnitNum - the VOQ Unit number, 0..1
*       trgDev     - target device number 0..127 to enable/diable.
*       enable     - if GT_TRUE, enable target device.
*                       GT_FALSE, disable target device
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum or trgDev.
*
* COMMENTS:
*       The target devices enabled should correspond to target devices
*       used in the VOQ Device to Fport Map (unicast and multicasts),
*       See cpssExMxPmFabricVoqDevToFportMapSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqDevEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U8 trgDev;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    trgDev = (GT_U8)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqDevEnableSet(devNum, voqUnitNum, trgDev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqDevEnableGet
*
* DESCRIPTION:
*       This routine gets enable/disable state for the target device in the
*       VOQ target device table.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum     - the device number.
*       voqUnitNum - the VOQ Unit number, 0..1
*       trgDev     - target device number 0..127 to enable/diable.
*
*
* OUTPUTS:
*       enablePtr  - if GT_TRUE, enable target device.
*                       GT_FALSE, disable target device
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum or trgDev.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       The target devices enabled should correspond to target devices
*       used in the VOQ Device to Fport Map (unicast and multicasts),
*       See cpssExMxPmFabricVoqDevToFportMapSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqDevEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U8 trgDev;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    trgDev = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqDevEnableGet(devNum, voqUnitNum, trgDev, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqDevToFportMapSet
*
* DESCRIPTION:
*       This routine sets the target device to fabric port table.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum      - the device number.
*       voqUnitNum  - the VOQ Unit number, 0..1
*       trgDev      - target device number 0..127 to Map.
*       fportsArr   - Array of 4 target fport for the target device.
*                     4 target fports are configured for each target device.
*                     The Load Balancing Index (LBI), which is 2 bits,
*                     allows for 4 alternate paths.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum or trgDev.
*       GT_BAD_PTR      - on NULL pointer.
*                OUT_OF_RANGE          - on FPort out of range values.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqDevToFportMapSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U8 trgDev;
    GT_FPORT fportsArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    trgDev = (GT_U8)inArgs[2];
    fportsArr[0]=(GT_FPORT)inArgs[3];
    fportsArr[1]=(GT_FPORT)inArgs[4];
    fportsArr[2]=(GT_FPORT)inArgs[5];
    fportsArr[3]=(GT_FPORT)inArgs[6];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqDevToFportMapSet(devNum, voqUnitNum, trgDev, fportsArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqDevToFportMapGet
*
* DESCRIPTION:
*       This routine gets the target device to fabric port table.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum      - the device number.
*       voqUnitNum  - the VOQ Unit number, 0..1
*       trgDev      - target device number 0..127 to Map.
*
* OUTPUTS:
*       fportsArr   - Array of 4 target fport for the target device.
*                     4 target fports are configured for each target device.
*                     The Load Balancing Index (LBI), which is 2 bits,
*                     allows for 4 alternate paths.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum or trgDev.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqDevToFportMapGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U8 trgDev;
    GT_FPORT fportsArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    trgDev = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqDevToFportMapGet(devNum, voqUnitNum, trgDev, fportsArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",fportsArr[0],fportsArr[1],fportsArr[2],fportsArr[3]);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqDevToConfigSetMapSet
*
* DESCRIPTION:
*       This routine sets the configuration sets for the target device.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum            - the device number.
*       voqUnitNum        - the VOQ Unit number, 0..1
*       trgDev            - target device number 0..127 to Map.
*       configSetIndexArr - index of configuration set 0-3
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum or trgDev.
*       GT_BAD_PTR               - on NULL pointer.
*       OUT_OF_RANGE    - on Configuration Set Index out of range values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqDevToConfigSetMapSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U8 trgDev;
    GT_U32 configSetIndexArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    trgDev = (GT_U8)inArgs[2];
    configSetIndexArr[0]=(GT_U32)inArgs[3];
    configSetIndexArr[1]=(GT_U32)inArgs[4];
    configSetIndexArr[2]=(GT_U32)inArgs[5];
    configSetIndexArr[3]=(GT_U32)inArgs[6];
    /* call cpss api function */
    result = cpssExMxPmFabricVoqDevToConfigSetMapSet(devNum, voqUnitNum, trgDev, configSetIndexArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqDevToConfigSetMapGet
*
* DESCRIPTION:
*       This routine gets configuration set index of the target device
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       voqUnitNum     - the VOQ Unit number, 0..1
*       trgDev         - target device number 0..127 to Map.
*
* OUTPUTS:
*       configSetIndexArr - index of configuration set 0-3
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum or trgDev.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqDevToConfigSetMapGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U8 trgDev;
    GT_FPORT configSetIndexArr[CPSS_EXMXPM_FABRIC_LBI_SIZE_CNS];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    trgDev = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqDevToConfigSetMapGet(devNum, voqUnitNum, trgDev, configSetIndexArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",configSetIndexArr[0],configSetIndexArr[1],configSetIndexArr[2],
                                                                          configSetIndexArr[3]);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqTcToFabricTcMapSet
*
* DESCRIPTION:
*   This routine sets the Fabric Traffic Class assignment by mapping traffic
*    class (8 levels) assigned by the ingress processing to the Fabric Traffic
*    class (4 levels).
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum      - the device number.
*       voqUnitNum  - the VOQ Unit number, 0..1
*       pktType     - unicast or multicast
*       ingressTc   - 8 levels Traffic Class assigned by ingress processing.
*       fabricTc    - 4 levels Fabric Traffic Class.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum or other param.
*       GT_OUT_OF_RANGE - on out of range pp tc and fabric tc.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Packets received from the ingress processing are associated with an
*       8-level traffic class.  The VOQs fabric port and multicast VOQ
*       support 4 traffic classes.  The function allows for this
*       8 to 4 traffic class mapping.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqTcToFabricTcMapSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    CPSS_EXMXPM_FABRIC_PACKET_TYPE_ENT pktType;
    GT_U32 ingressTc;
    GT_U32 fabricTc;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    pktType = (CPSS_EXMXPM_FABRIC_PACKET_TYPE_ENT)inArgs[2];
    ingressTc = (GT_U32)inArgs[3];
    fabricTc = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqTcToFabricTcMapSet(devNum, voqUnitNum, pktType, ingressTc, fabricTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqTcToFabricTcMapGet
*
* DESCRIPTION:
*   This routine gets the Fabric Traffic Class (4 levels) assigned by mapping
*   traffic class (8 levels) to the Fabric Traffic class (4 levels).
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum      - the device number.
*       voqUnitNum  - the VOQ Unit number, 0..1
*       pktType     - unicast or multicast
*       ingressTc   - 8 levels Traffic Class assigned by ingress processing.
*
* OUTPUTS:
*       fabricTcPtr - (pointer to) 4 levels Fabric Traffic Class.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum or other param.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE - on pp tc out of range value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Packets received from the ingress processing are associated with an
*       8-level traffic class.  The VOQs fabric port and multicast VOQ
*       support 4 traffic classes.  The function allows for this
*       8 to 4 traffic class mapping.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqTcToFabricTcMapGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    CPSS_EXMXPM_FABRIC_PACKET_TYPE_ENT pktType;
    GT_U32 ingressTc;
    GT_U32 fabricTc;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    pktType = (CPSS_EXMXPM_FABRIC_PACKET_TYPE_ENT)inArgs[2];
    ingressTc = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqTcToFabricTcMapGet(devNum, voqUnitNum, pktType, ingressTc, &fabricTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", fabricTc);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqControlTrafficQosSet
*
* DESCRIPTION:
*       Set Fabric TC and DP to "CPU to Network", "Network to CPU",
*       "CPU to CPU" traffic types
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - the device number
*       voqUnitNum          - the VOQ Unit number, 0..1
*       ctrlQosAssignEnable - GT_TRUE: enable Fabric TC/DP Assignment
*                             GT_FALSE: disable Fabric TC/DP Assignment
*                             TC and DP for control traffic assigned as for
*                             "Network to Network" packets.
*       ctrlQosParamsPtr    - CPU traffic fabric TC and Dp assignments.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE - on out of range traffic qos values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqControlTrafficQosSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_BOOL ctrlQosAssignEnable;
    CPSS_EXMXPM_FABRIC_VOQ_CTRL_TRAFFIC_QOS_STC ctrlQosParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    ctrlQosAssignEnable=(GT_BOOL)inArgs[2];
    ctrlQosParams.netPortToCpuTc=(GT_U32)inArgs[3];
    ctrlQosParams.netPortToCpuDp=(CPSS_DP_LEVEL_ENT)inArgs[4];
    ctrlQosParams.cpuToNetPortTc=(GT_U32)inArgs[5];
    ctrlQosParams.cpuToNetPortDp=(CPSS_DP_LEVEL_ENT)inArgs[6];
    ctrlQosParams.cpuToCpuTc=(GT_U32)inArgs[7];
    ctrlQosParams.cpuToCpuDp=(CPSS_DP_LEVEL_ENT)inArgs[8];


    /* call cpss api function */
    result = cpssExMxPmFabricVoqControlTrafficQosSet(devNum, voqUnitNum, ctrlQosAssignEnable, &ctrlQosParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqControlTrafficQosGet
*
* DESCRIPTION:
*       Get Fabric TC and DP to "CPU to Network", "Network to CPU",
*       "CPU to CPU" traffic types
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum       - the device number
*       voqUnitNum   - the VOQ Unit number, 0..1
*
* OUTPUTS:
*       ctrlQosAssignEnablePtr - GT_TRUE: enable Fabric TC/DP Assignment
*                                GT_FALSE: disable Fabric TC/DP Assignment
*                                TC and DP for control traffic assigned as for
*                                "Network to Network" packets.
*       ctrlQosParamsPtr       - CPU traffic fabric TC and Dp assignments.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqControlTrafficQosGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_BOOL ctrlQosAssignEnable;
    CPSS_EXMXPM_FABRIC_VOQ_CTRL_TRAFFIC_QOS_STC ctrlQosParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqControlTrafficQosGet(devNum, voqUnitNum, &ctrlQosAssignEnable, &ctrlQosParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d",ctrlQosAssignEnable,ctrlQosParams.netPortToCpuTc,
                 ctrlQosParams.netPortToCpuDp,ctrlQosParams.cpuToNetPortTc,ctrlQosParams.cpuToNetPortDp,
                  ctrlQosParams.cpuToCpuTc,ctrlQosParams.cpuToCpuDp);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqGlobalLimitsSet
*
* DESCRIPTION:
*     This function sets the maximum number of descriptors
*     and buffers that can be allocated by the VOQ unit,
*     thus limiting the sum of the length of all VOQs.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       voqUnitNum     - the VOQ Unit number, 0..1
*       maxDescriptors - the maximum number of descriptors 0..2048
*       maxBuffers     - the maximum number of buffers 0..2048
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum.
*       GT_OUT_OF_RANGE - maxDescriptors or maxBuffers out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqGlobalLimitsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 maxDescriptors;
    GT_U32 maxBuffers;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    maxDescriptors = (GT_U32)inArgs[2];
    maxBuffers = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqGlobalLimitsSet(devNum, voqUnitNum, maxDescriptors, maxBuffers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqGlobalLimitsGet
*
* DESCRIPTION:
*     This function gets the maximum number of descriptors
*     and buffers that can be allocated by the VOQ unit,
*     thus limiting the sum of the length of all VOQs.
*
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       voqUnitNum     - the VOQ Unit number, 0..1
*
* OUTPUTS:
*       maxDescriptorsPtr - the maximum number of descriptors 0..2048
*       maxBuffersPtr     - the maximum number of buffers 0..2048
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqGlobalLimitsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 maxDescriptors;
    GT_U32 maxBuffers;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqGlobalLimitsGet(devNum, voqUnitNum, &maxDescriptors, &maxBuffers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", maxDescriptors, maxBuffers);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqStatCntrsConfigSet
*
* DESCRIPTION:
*    This routine sets the statistics counter set.
*    There are 4 statistics counter sets.
*    Each Set can be independently configured.
*    The counter set track packets that adhere to the
*    associated configuration set by this function.
*    If all the counter's filters
*    are disabled, the counters in the counter set reflect
*    all the VOQ packets.
*
*    The counter set can be retrieved using
*    cpssExMxPmFabricVoqStatCntrsGet
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - the device number
*       voqUnitNum          - the VOQ Unit number, 0..1
*       setNum              - The counters Set number (0-3)
*       counterSetConfigPtr - the criterions to define the counter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum or setNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE - on out of range configuration parameters.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqStatCntrsConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 setNum;
    CPSS_EXMXPM_FABRIC_VOQ_STAT_CNTRS_SET_CONFIG_STC counterSetConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    setNum=(GT_U32)inArgs[2];

    counterSetConfig.cntrTypeSelect=(CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_TYPE_ENT)inFields[0];
    counterSetConfig.dpType=(CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_ENT)inFields[1];
    counterSetConfig.countDpEnable=(GT_BOOL)inFields[2];
    counterSetConfig.countedDp=(CPSS_DP_LEVEL_ENT)inFields[3];
    counterSetConfig.tcType=(CPSS_EXMXPM_FABRIC_VOQ_INFO_SRC_ENT)inFields[4];
    counterSetConfig.countTcEnable=(GT_BOOL)inFields[5];
    counterSetConfig.countedTc=(GT_U32)inFields[6];
    counterSetConfig.countFportEnable=(GT_BOOL)inFields[7];
    counterSetConfig.countedFport=(GT_FPORT)inFields[8];
    counterSetConfig.countDevEnable=(GT_BOOL)inFields[9];
    counterSetConfig.countedDev=(GT_U8)inFields[10];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqStatCntrsConfigSet(devNum, voqUnitNum, setNum, &counterSetConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqStatCntrsConfigGet
*
* DESCRIPTION:
*    This routine gets the statistics counter set.
*    There are 4 statistics counter sets.
*    Each Set can be independently configured.
*    The counter set track packets that adhere to the
*    associated configuration set by this function.
*    If all the counter's filters
*    are disabled, the counters in the counter set reflect
*    all the VOQ packets.
*
*    The counter set can be retrieved using
*    cpssExMxPmFabricVoqStatCntrsGet
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - the device number
*       voqUnitNum          - the VOQ Unit number, 0..1
*       setNum             - The counters Set number (0-3)
*
* OUTPUTS:
*       counterSetConfigPtr - the criterions to define the counter
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum or other param.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqStatCntrsConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 setNum;
    CPSS_EXMXPM_FABRIC_VOQ_STAT_CNTRS_SET_CONFIG_STC counterSetConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    setNum=(GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqStatCntrsConfigGet(devNum, voqUnitNum, setNum, &counterSetConfig);

    if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
     }

    inFields[0]=counterSetConfig.cntrTypeSelect;
    inFields[1]=counterSetConfig.dpType;
    inFields[2]=counterSetConfig.countDpEnable;
    inFields[3]=counterSetConfig.countedDp;
    inFields[4]=counterSetConfig.tcType;
    inFields[5]=counterSetConfig.countTcEnable;
    inFields[6]=counterSetConfig.countedTc;
    inFields[7]=counterSetConfig.countFportEnable;
    inFields[8]=counterSetConfig.countedFport;
    inFields[9]=counterSetConfig.countDevEnable;
    inFields[10]=counterSetConfig.countedDev;

   fieldOutput("%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                           inFields[3],  inFields[4],  inFields[5],
                           inFields[6],  inFields[7],  inFields[8],
                           inFields[9],  inFields[10] );


    galtisOutput(outArgs, GT_OK, "%f");



    return CMD_OK;
}
/*************************************************************************
* cpssExMxPmGetNext
*
* DESCRIPTION:
*     This galtis function for reafresh
*
* INPUTS:
*
*
*       none
*
* OUTPUTS:
*      none
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum,fport.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     galtisOutput(outArgs, GT_OK, "%d", -1);
     return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqStatCntrsGet
*
* DESCRIPTION:
*    This routine retrieves the VOQ statistic counters.
*    There are 4 statistics counter sets.
*    Each Set can be independently configured.
*    The counter set track packets that adhere to the
*    associated configuration set by this function.
*    If all the counter's filters
*    are disabled, the counters in the counter set reflect
*    all the VOQ packets.
*
*    The counter set can be configured using
*    cpssExMxPmFabricVoqStatCntrsConfigSet
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum     - device number
*       voqUnitNum - the VOQ Unit number, 0..1
*       setNum     - The counters Set number (0-3)
*
* OUTPUTS:
*       cntrsValPtr  - statistic couters values
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum or setNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counters are read only and cleared on read.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqStatCntrsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 setNum;
    CPSS_EXMXPM_FABRIC_VOQ_STAT_CNTRS_STC cntrsVal;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    setNum=(GT_U32)inArgs[2];



    /* call cpss api function */
    result = cpssExMxPmFabricVoqStatCntrsGet(devNum, voqUnitNum, setNum, &cntrsVal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",cntrsVal.goodRxCount,cntrsVal.eqDropCount);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqGetGlobalCountersGet
*
* DESCRIPTION:
*       This routine retrieves the VOQ Global counters.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum       - device number.
*       voqUnitNum   - the VOQ Unit number, 0..1
*
* OUTPUTS:
*       voqGlobalCountersPtr - pointer to structure containing global counters
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counters are read only and cleared on read.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqGetGlobalCountersGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    CPSS_EXMXPM_FABRIC_VOQ_GLOBAL_CNTRS_STC voqGlobalCounters;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqGetGlobalCountersGet(devNum, voqUnitNum, &voqGlobalCounters);

    if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
     }

    inFields[0]=voqGlobalCounters.goodRxTcCount[0];
    inFields[1]=voqGlobalCounters.goodRxTcCount[1];
    inFields[2]=voqGlobalCounters.goodRxTcCount[2];
    inFields[3]=voqGlobalCounters.goodRxTcCount[3];
    inFields[4]=voqGlobalCounters.goodRxTcCount[4];
    inFields[5]=voqGlobalCounters.goodRxTcCount[5];
    inFields[6]=voqGlobalCounters.goodRxTcCount[6];
    inFields[7]=voqGlobalCounters.goodRxTcCount[7];
    inFields[8]=voqGlobalCounters.dropOnTailCount;
    inFields[9]=voqGlobalCounters.dropOnDevDisCount;
    inFields[10]=voqGlobalCounters.dropOnEccCount;

   fieldOutput("%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                           inFields[3],  inFields[4],  inFields[5],
                           inFields[6],  inFields[7],  inFields[8],
                           inFields[9],  inFields[10]
                 );

    galtisOutput(outArgs, GT_OK, "%f");



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet
*
* DESCRIPTION:
*       This routine retrieves the number of Descriptors currently allocated
*       (in use) for various kinds of packets and queues.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum       - device number.
*       voqUnitNum   - the VOQ Unit number, 0..1
*
* OUTPUTS:
*       cntrsPtr - pointer to structure containing counters of
*                  packet descriptors currently allocated
*                  i.e. in use).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counters are read only.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    CPSS_EXMXPM_FABRIC_VOQ_GLOBAL_RESOURCES_CNTRS_STC cntrs;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet(devNum, voqUnitNum, &cntrs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",cntrs.globalDescCnt,cntrs.globalBufCnt);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqFportResourcesInUseCntrsGet
*
* DESCRIPTION:
*       This routine retrieves the number of Descriptors currently allocated
*       (in use) for various kinds of packets and queues.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum       - device number.
*       voqUnitNum   - the VOQ Unit number, 0..1
*       fport        - fabric port or CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS
*                       for MC packets queue.
*
* OUTPUTS:
*       cntrsPtr - pointer to structure containing counters of
*                      packet descriptors currently allocated
*                      (i.e. in use).
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counters are read-only.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqFportResourcesInUseCntrsGetFirst
(                 
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_FPORT fport;
    CPSS_EXMXPM_FABRIC_VOQ_FPORT_RESOURCES_CNTRS_STC cntrs;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    fport=(GT_FPORT)inArgs[2];
    /* call cpss api function */
    result = cpssExMxPmFabricVoqFportResourcesInUseCntrsGet(devNum, voqUnitNum, fport, &cntrs);

    if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
     }

    inFields[0]=cntrs.buffCount;
    inFields[1]=cntrs.tcDescCount[0];
    inFields[2]=cntrs.tcDescCount[1];
    inFields[3]=cntrs.tcDescCount[2];
    inFields[4]=cntrs.tcDescCount[3];
    inFields[5]=cntrs.tcBuffCount[0];
    inFields[6]=cntrs.tcBuffCount[1];
    inFields[7]=cntrs.tcBuffCount[2];
    inFields[8]=cntrs.tcBuffCount[3];

   fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                           inFields[3],  inFields[4],  inFields[5],
                           inFields[6],  inFields[7],  inFields[8]);



    galtisOutput(outArgs, GT_OK, "%f");



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqMcToConfigSetMapSet
*
* DESCRIPTION:
*       This routine maps the Multicast VOQ to one of 4 configuration sets.
*       The configuration set defines the maximum number of descriptors to be
*       allocated by the Multicast VOQ.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum             - the device number.
*       voqUnitNum         - the VOQ Unit number, 0..1
*       configSetIndex     - the configuration set number (0-3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum or setNum.
*       GT_OUT_OF_RANGE - on Config Set out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqMcToConfigSetMapSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 configSetIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    configSetIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqMcToConfigSetMapSet(devNum, voqUnitNum, configSetIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqMcToConfigSetMapGet
*
* DESCRIPTION:
*       This routine gets map of the Multicast VOQ to one of 4 configuration
*       sets. The configuration set defines the maximum number of
*       descriptors to beallocated by the Multicast VOQ.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum     - the device number.
*       voqUnitNum - the VOQ Unit number, 0..1
*
* OUTPUTS:
*       configSetIndexPtr - the configuration set number (0-3)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqMcToConfigSetMapGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 configSetIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqMcToConfigSetMapGet(devNum, voqUnitNum, &configSetIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", configSetIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet
*
* DESCRIPTION:
*     This routine sets the maximum number of buffers to be
*     allocated to Fabric ports that are assigned to Configuration Set.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       voqUnitNum     - the VOQ Unit number, 0..1
*       configSetIndex - the configuration set number (0-3)
*       maxBuffers     - the maximum number of buffers for Fport/MC VOQ
*                        associated with this configuration set number.
*                        Range 0..2047, resolution - 1 buffer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum or config set number.
*       GT_OUT_OF_RANGE - on out of range max buffers number.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqConfigSetFportMaxBufLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 configSetIndex;
    GT_U32 maxBuffers;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    configSetIndex = (GT_U32)inArgs[2];
    maxBuffers = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitSet(devNum, voqUnitNum, configSetIndex, maxBuffers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet
*
* DESCRIPTION:
*     This routine gets the maximum number of buffers to be
*     allocated to Fabric ports that are assigned to Configuration Set.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       voqUnitNum     - the VOQ Unit number, 0..1
*       configSetIndex - the configuration set number (0-3)
*
* OUTPUTS:
*       maxBuffersPtr  - the maximum number of buffers for Fport/MC VOQ
*                        associated with this configuration set number.
*                        Range 0..2047, resolution - 1 buffer.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum or config set number.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqConfigSetFportMaxBufLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 configSetIndex;
    GT_U32 maxBuffers;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    configSetIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqConfigSetFportMaxBuffersLimitGet(devNum, voqUnitNum, configSetIndex, &maxBuffers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maxBuffers);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet
*
* DESCRIPTION:
*     This routine sets the Maximum Buffers Limit per
*     Configuration Set (0-3) + Fabric Priority (0-3) + Drop Precedence (0-2)
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       voqUnitNum     - the VOQ Unit number, 0..1
*       configSetIndex - the configuration set number (0-3)
*       fabricTc       - Fport/MC fabric priority queue (0-3)
*       dp             - the Drop Precedence (0-2)
*       maxBuffers     - The maximum number of buffers with given
*                        Drop Precedence to be allocated to Fabric Traffic Class
*                        (which are actual VOQs) that are assigned to Set.
*                        Range 0..2047, resolution - 1 buffer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, voqUnitNum, fabricTc, DP and
*                           configSetIndex.
*       GT_OUT_OF_RANGE - on max buffers out of range value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 configSetIndex;
    GT_U32 fabricTc;
    CPSS_DP_LEVEL_ENT dp;
    GT_U32 maxBuffers;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    configSetIndex = (GT_U32)inArgs[2];
    fabricTc = (GT_U32)inArgs[3];
    dp = (CPSS_DP_LEVEL_ENT)inArgs[4];
    maxBuffers = (GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet(devNum, voqUnitNum, configSetIndex, fabricTc, dp, maxBuffers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet
*
* DESCRIPTION:
*     This routine gets the Maximum Buffers Limit per
*     Configuration Set (0-3) + Fabric Priority (0-3) + Drop Precedence (0-2)
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       voqUnitNum     - the VOQ Unit number, 0..1
*       configSetIndex - the configuration set number (0-3)
*       fabricTc       - Fport/MC fabric priority queue (0-3)
*       dp             - the Drop Precedence (0-2)
*
* OUTPUTS:
*       maxBuffersPtr  - The maximum number of buffers with given
*                        Drop Precedence to be allocated to Fabric Traffic Class
*                        (which are actual VOQs) that are assigned to Set.
*                        Range 0..2047, resolution - 1 buffer.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, voqUnitNum, fabricTc, DP and
*                           configSetIndex.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 voqUnitNum;
    GT_U32 configSetIndex;
    GT_U32 fabricTc;
    CPSS_DP_LEVEL_ENT dp;
    GT_U32 maxBuffers;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    configSetIndex = (GT_U32)inArgs[2];
    fabricTc = (GT_U32)inArgs[3];
    dp = (CPSS_DP_LEVEL_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet(devNum, voqUnitNum, configSetIndex, fabricTc, dp, &maxBuffers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maxBuffers);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqLastDropDevGet
*
* DESCRIPTION:
*     This routine gets the device number of the last droped packet due to
*     Target device filtering.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       voqUnitNum     - the VOQ Unit number, 0..1
*
* OUTPUTS:
*       lastDropDevPtr  - The last device number of the device which was the
*                         target for the last descriptor that was dropped.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on HW error.
*       GT_BAD_PARAM                - on wrong devNum or voqUnitNum
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqLastDropDevGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      voqUnitNum;
    GT_U32      lastDropDev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqLastDropDevGet(devNum, voqUnitNum, &lastDropDev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", lastDropDev);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqSharedBuffersConfigSet
*
* DESCRIPTION:
*     This routine sets total number of shared buffers limit and resource sharing
*     per DP.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum           - the device number.
*       voqUnitNum       - the VOQ Unit number, 0..1
*       limitParamsPtr   - resource sharing limit parameters
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on HW error.
*       GT_BAD_PARAM                - on wrong devNum, voqUnitNum or usersDpMode
*       GT_OUT_OF_RANGE             - on poolBufLimit out of range.
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqSharedBuffersConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      voqUnitNum;
    CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_SHARED_LIMIT_STC    limitParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    limitParams.poolBufLimit = (GT_U32)inArgs[2];
    limitParams.usersDpMode = (CPSS_EXMXPM_FABRIC_VOQ_SHARED_POOL_USERS_DP_MODE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqSharedBuffersConfigSet(devNum, voqUnitNum, &limitParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqSharedBuffersConfigGet
*
* DESCRIPTION:
*     This routine gets total number of shared buffers limit and resource sharing
*     per DP.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum           - the device number.
*       voqUnitNum       - the VOQ Unit number, 0..1
*
* OUTPUTS:
*       limitParamsPtr   - resource sharing limit parameters
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on HW error.
*       GT_BAD_PARAM                - on wrong devNum or voqUnitNum
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_BAD_STATE                - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqSharedBuffersConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8                                               devNum;
    GT_U32                                              voqUnitNum;
    CPSS_EXMXPM_FABRIC_VOQ_RESOURCE_SHARED_LIMIT_STC    limitParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqSharedBuffersConfigGet(devNum, voqUnitNum, &limitParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", limitParams.poolBufLimit, limitParams.usersDpMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricVoqRandomTailDropSet
*
* DESCRIPTION:
*     This routine configures random Priority/VOQ mask and random VOQ mask.
*     The random mask is used in calculation of random tail-drop limit.
*     A random tail-drop limit is calculated as follows:
*     RndTailDropLimit = FixedTailDropLimit - (RND & Mask), where
*     FixedTailDropLimit is the configured limit (VoQ[n] or VoQ[n,tc]-DP[0..2]),
*     RND is a random number in the range 0–127 and MASK is a configurable
*     7 bits mask(voqRandomMask or voqPriorityRandomMask).
*     Random tail-drop helps to avoid global synchronization by randomly
*     discarding packets before the queue size has reached the tail-drop limit.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum                  - the device number.
*       voqUnitNum              - the VOQ Unit number, 0..1
*       voqRandomMask           - VOQ Random Mask (0..127)
*                                 value 0 means the randomization of the tail
*                                 drop threshold is disabled.
*       voqPriorityRandomMask   - VOQ/Priority Random Mask (0..127)
*                                 value 0 means the randomization of the tail
*                                 drop threshold is disabled.
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on HW error.
*       GT_BAD_PARAM                - on wrong devNum,voqUnitNum,
*                                              voqRandomMask or voqPriorityRandomMask
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqRandomTailDropSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      voqUnitNum;
    GT_U32      voqRandomMask;
    GT_U32      voqPriorityRandomMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    voqRandomMask = (GT_U32)inArgs[2];
    voqPriorityRandomMask = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqRandomTailDropSet(devNum, voqUnitNum, voqRandomMask, voqPriorityRandomMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmFabricVoqRandomTailDropGet
*
* DESCRIPTION:
*     This routine gets random Priority/VOQ mask and random VOQ mask.
*     The random mask is used in calculation of random tail-drop limit.
*     A random tail-drop limit is calculated as follows:
*     RndTailDropLimit = FixedTailDropLimit - (RND & Mask), where
*     FixedTailDropLimit is the configured limit (VoQ[n] or VoQ[n,tc]-DP[0..2]),
*     RND is a random number in the range 0–127 and MASK is a configurable
*     7 bits mask(voqRandomMask or voqPriorityRandomMask).
*     Random tail-drop helps to avoid global synchronization by randomly
*     discarding packets before the queue size has reached the tail-drop limit.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum                  - the device number.
*       voqUnitNum              - the VOQ Unit number, 0..1
* OUTPUTS:
*       voqRandomMaskPtr           - VOQ Random Mask
*       voqPriorityRandomMaskPtr   - VOQ/Priority Random Mask
* RETURNS:
*       GT_OK                       - on success.
*       GT_HW_ERROR                 - on HW error.
*       GT_BAD_PARAM                - on wrong devNum or voqUnitNum
*       GT_BAD_PTR                  - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricVoqRandomTailDropGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8   devNum;
    GT_U32  voqUnitNum;
    GT_U32  voqRandomMask;
    GT_U32  voqPriorityRandomMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricVoqRandomTailDropGet(devNum, voqUnitNum, &voqRandomMask, &voqPriorityRandomMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", voqRandomMask, voqPriorityRandomMask);

    return CMD_OK;
}

/**** database initialization **************************************/

/* TODO: replace X_X with real number of arguments */
#define X_X  0

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmFabricVoqFportTxEnableSet",
         &wrCpssExMxPmFabricVoqFportTxEnableSet,
         4, 0},
        {"cpssExMxPmFabricVoqFportTxEnableGet",
         &wrCpssExMxPmFabricVoqFportTxEnableGet,
         3, 0},
        {"cpssExMxPmFabricVoqDevTblSet",
         &wrCpssExMxPmFabricVoqDevTblSet,
         4, 0},
        {"cpssExMxPmFabricVoqDevTblSet_1",
         &wrCpssExMxPmFabricVoqDevTblSet_1,
         6, 0},
        {"cpssExMxPmFabricVoqDevEnableSet",
         &wrCpssExMxPmFabricVoqDevEnableSet,
         4, 0},
        {"cpssExMxPmFabricVoqDevEnableGet",
         &wrCpssExMxPmFabricVoqDevEnableGet,
         3, 0},
        {"cpssExMxPmFabricVoqDevToFportMapSet",
         &wrCpssExMxPmFabricVoqDevToFportMapSet,
         7, 0},
        {"cpssExMxPmFabricVoqDevToFportMapGet",
         &wrCpssExMxPmFabricVoqDevToFportMapGet,
         3, 0},
        {"cpssExMxPmFabricVoqDevToConfigSetMapSet",
         &wrCpssExMxPmFabricVoqDevToConfigSetMapSet,
         7, 0},
        {"cpssExMxPmFabricVoqDevToConfigSetMapGet",
         &wrCpssExMxPmFabricVoqDevToConfigSetMapGet,
         3, 0},
        {"cpssExMxPmFabricVoqTcToFabricTcMapSet",
         &wrCpssExMxPmFabricVoqTcToFabricTcMapSet,
         5, 0},
        {"cpssExMxPmFabricVoqTcToFabricTcMapGet",
         &wrCpssExMxPmFabricVoqTcToFabricTcMapGet,
         4, 0},
        {"cpssExMxPmFabricVoqControlTrafficQosSet",
         &wrCpssExMxPmFabricVoqControlTrafficQosSet,
         9,0},
        {"cpssExMxPmFabricVoqControlTrafficQosGet",
         &wrCpssExMxPmFabricVoqControlTrafficQosGet,
         2, 0},
        {"cpssExMxPmFabricVoqGlobalLimitsSet",
         &wrCpssExMxPmFabricVoqGlobalLimitsSet,
         4, 0},
        {"cpssExMxPmFabricVoqGlobalLimitsGet",
         &wrCpssExMxPmFabricVoqGlobalLimitsGet,
         2, 0},
        {"cpssExMxPmFabricVoqStatCntrsConfigSet",
         &wrCpssExMxPmFabricVoqStatCntrsConfigSet,
         3, 11},
        {"cpssExMxPmFabricVoqStatCntrsConfigGetFirst",
         &wrCpssExMxPmFabricVoqStatCntrsConfigGetFirst,
         3, 0},
        {"cpssExMxPmFabricVoqStatCntrsConfigGetNext",
         &wrCpssExMxPmGetNext,
         0, 0},
        {"cpssExMxPmFabricVoqStatCntrsGet",
         &wrCpssExMxPmFabricVoqStatCntrsGet,
         3, 0},
        {"cpssExMxPmFabricVoqGetGlobalCountersGetFirst",
         &wrCpssExMxPmFabricVoqGetGlobalCountersGetFirst,
         2, 0},
        {"cpssExMxPmFabricVoqGetGlobalCountersGetNext",
         &wrCpssExMxPmGetNext,
         0, 0},
        {"cpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet",
         &wrCpssExMxPmFabricVoqGlobalResourcesInUseCntrsGet,
         2, 0},
        {"cpssExMxPmFabricVoqFportResInUseCntrsGetFirst",
         &wrCpssExMxPmFabricVoqFportResourcesInUseCntrsGetFirst,
         3, 0},
        {"cpssExMxPmFabricVoqFportResInUseCntrsGetNext",
         &wrCpssExMxPmGetNext,
         0, 0},
        {"cpssExMxPmFabricVoqMcToConfigSetMapSet",
         &wrCpssExMxPmFabricVoqMcToConfigSetMapSet,
         3, 0},
        {"cpssExMxPmFabricVoqMcToConfigSetMapGet",
         &wrCpssExMxPmFabricVoqMcToConfigSetMapGet,
         2, 0},
        {"cpssExMxPmFabricVoqConfigSetFportMaxBufLimitSet",
         &wrCpssExMxPmFabricVoqConfigSetFportMaxBufLimitSet,
         4, 0},
        {"cpssExMxPmFabricVoqConfigSetFportMaxBufLimitGet",
         &wrCpssExMxPmFabricVoqConfigSetFportMaxBufLimitGet,
         3, 0},
        {"cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet",
         &wrCpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitSet,
         6, 0},
        {"cpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet",
         &wrCpssExMxPmFabricVoqConfigSetTcDpMaxBuffersLimitGet,
         5, 0},
        {"cpssExMxPmFabricVoqLastDropDevGet",
         &wrCpssExMxPmFabricVoqLastDropDevGet,
         2, 0},
        {"cpssExMxPmFabricVoqSharedBuffersConfigSet",
         &wrCpssExMxPmFabricVoqSharedBuffersConfigSet,
         4, 0},
        {"cpssExMxPmFabricVoqSharedBuffersConfigGet",
         &wrCpssExMxPmFabricVoqSharedBuffersConfigGet,
         2, 0},
        {"cpssExMxPmFabricVoqRandomTailDropSet",
         &wrCpssExMxPmFabricVoqRandomTailDropSet,
         4, 0},
        {"cpssExMxPmFabricVoqRandomTailDropGet",
         &wrCpssExMxPmFabricVoqRandomTailDropGet,
         2, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmFabricVoq
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
GT_STATUS cmdLibInitCpssExMxPmFabricVoq
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

