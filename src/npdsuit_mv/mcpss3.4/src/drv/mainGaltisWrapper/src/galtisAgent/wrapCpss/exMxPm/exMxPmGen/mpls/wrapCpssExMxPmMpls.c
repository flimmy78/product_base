/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmMpls.c
*
* DESCRIPTION:
*       wrappers for cpssExMxPmMpls.c
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
#include <cpss/exMxPm/exMxPmGen/mpls/cpssExMxPmMpls.h>

/*******************************************************************************
* cpssExMxPmMplsLabelSpaceModeSet
*
* DESCRIPTION:
*       Defines whether the label space is common or per Interface
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - device number
*       mode            - MPLS Label Space mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on illegal parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsLabelSpaceModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MPLS_LABEL_SPACE_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_MPLS_LABEL_SPACE_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMplsLabelSpaceModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMplsLabelSpaceModeGet
*
* DESCRIPTION:
*       Get label space mode (common or per Interface)
*
* APPLICABLE DEVICES: All EXMXPM devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - (pointer to) MPLS Label Space mode
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsLabelSpaceModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MPLS_LABEL_SPACE_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmMplsLabelSpaceModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMplsEnableSet
*
* DESCRIPTION:
*       Enable MPLS LSR(Label Switch Router) switching
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       enable  - enable/disable feature
*               GT_FALSE - MPLS LSR switching is bypassed
*               GT_TRUE  - MPLS LSR switching is enabled

*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMplsEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMplsEnableGet
*
* DESCRIPTION:
*       Get MPLS LSR(Label Switch Router) switching state
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) MPLS Engine state
*                   GT_FALSE - MPLS LSR switching is bypassed
*                   GT_TRUE  - MPLS LSR switching is enabled
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PTR      - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmMplsEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


static GT_U32 mplsInterfaceMaxIndex;
static GT_U32 mplsInterfaceMinIndex;
static GT_U32 mplsInterfaceCurrentIndex;
/*******************************************************************************
* cpssExMxPmMplsInterfaceEntrySet
*
* DESCRIPTION:
*       Set the MPLS Interface Table Entry
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - device number
*       index       - MPLS Interface Table index calculated based on the VRF-ID
*       mplsInterfaceEntryStcPtr - (pointer to)The MPLS Interface entry to set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When the device is configured to work in Per-Platform Label Space Mode,
*       index 0 is used.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsInterfaceEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    CPSS_EXMXPM_MPLS_INTERFACE_ENTRY_STC mplsInterfaceEntryStc;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    index=(GT_U32)inFields[0];
    mplsInterfaceEntryStc.valid=(GT_BOOL)inFields[1];
    mplsInterfaceEntryStc.minLabel=(GT_U32)inFields[2];
    mplsInterfaceEntryStc.maxLabel=(GT_U32)inFields[3];
    mplsInterfaceEntryStc.baseIndex=(GT_U32)inFields[4];
    mplsInterfaceEntryStc.ecmpQosSize=(GT_U32)inFields[5];
    mplsInterfaceEntryStc.nextHopRouteMethod=(CPSS_EXMXPM_MPLS_NEXT_HOP_ROUTE_METHOD_ENT)inFields[6];

    /* call cpss api function */
    result = cpssExMxPmMplsInterfaceEntrySet(devNum, index, &mplsInterfaceEntryStc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMplsInterfaceEntryGet
*
* DESCRIPTION:
*       Get the MPLS Interface Table Entry
*
* APPLICABLE DEVICES: All EXMXPM devices
*
* INPUTS:
*       devNum  - device number
*       index   - MPLS Interface Table index calculated based on the VRF-ID
*
* OUTPUTS:
*       mplsInterfaceEntryStcPtr - (pointer to) The MPLS Interface entry
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PTR      - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When the device is configured to work in Per-Platform Label Space Mode,
*       index 0 is used.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsInterfaceEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MPLS_INTERFACE_ENTRY_STC mplsInterfaceEntryStc;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (mplsInterfaceCurrentIndex>mplsInterfaceMaxIndex)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmMplsInterfaceEntryGet(devNum, mplsInterfaceCurrentIndex, &mplsInterfaceEntryStc);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    inFields[0]=mplsInterfaceCurrentIndex;
    inFields[1]=mplsInterfaceEntryStc.valid;
    inFields[2]=mplsInterfaceEntryStc.minLabel;
    inFields[3]=mplsInterfaceEntryStc.maxLabel;
    inFields[4]=mplsInterfaceEntryStc.baseIndex;
    inFields[5]=mplsInterfaceEntryStc.ecmpQosSize;
    inFields[6]=mplsInterfaceEntryStc.nextHopRouteMethod;

       /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],  inFields[6]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/********************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsInterfaceEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

   mplsInterfaceMinIndex=(GT_U32)inArgs[1];
   mplsInterfaceMaxIndex=(GT_U32)inArgs[2];

   if(mplsInterfaceMinIndex>mplsInterfaceMaxIndex)
   {
       galtisOutput(outArgs, GT_BAD_VALUE, "%d", -1);
       return CMD_OK;
   }


   mplsInterfaceCurrentIndex=mplsInterfaceMinIndex;

   return wrCpssExMxPmMplsInterfaceEntryGet(inArgs,inFields,numFields,outArgs);

}
/********************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsInterfaceEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    mplsInterfaceCurrentIndex++;
    return wrCpssExMxPmMplsInterfaceEntryGet(inArgs,inFields,numFields,outArgs);

}

/*******************************************************************************
* cpssExMxPmMplsInvalidEntryCmdSet
*
* DESCRIPTION:
*       Set the command assigned to MPLS packets if any of the following is true:
*       - The MPLS Interface Table entry is not valid
*       - The incoming label is out of label space boundaries
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       cmd     - Valid commands:
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E - Packets are trapped to the CPU
*                                         with a CPU code of INVALID_ENTRY_ERROR
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - bad paramter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsInvalidEntryCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cmd = (CPSS_PACKET_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMplsInvalidEntryCmdSet(devNum, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMplsInvalidEntryCmdGet
*
* DESCRIPTION:
*       Get the command assigned to MPLS packets if any of the following is true:
*       - The MPLS Interface Table entry is not valid
*       - The incoming label is out of label space boundaries
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       cmdPtr  - (pointer to) Valid commands:
*               CPSS_PACKET_CMD_TRAP_TO_CPU_E - Packets are trapped to the CPU
*                                       with a CPU code of INVALID_ENTRY_ERROR
*               CPSS_PACKET_CMD_DROP_HARD_E
*               CPSS_PACKET_CMD_DROP_SOFT_E
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - bad paramter
*       GT_BAD_PTR      - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsInvalidEntryCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PACKET_CMD_ENT cmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmMplsInvalidEntryCmdGet(devNum, &cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cmd);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMplsSpecialRouterTriggerEnableSet
*
* DESCRIPTION:
*       When enabled, MPLS packets that were previously assigned a packet
*       command of TRAP or SOFT_DROP can be subject to MPLS router engine.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - device number
*       routerTriggerType - MPLS router trigger type
*       enable            - enable / disable router trigger
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - bad paramter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsSpecialRouterTriggerEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TYPE_ENT routerTriggerType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    routerTriggerType = (CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmMplsSpecialRouterTriggerEnableSet(devNum, routerTriggerType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMplsSpecialRouterTriggerEnableGet
*
* DESCRIPTION:
*       Get the router trigger state. When enabled, MPLS packets that were
*       previously assigned a packet command of TRAP or SOFT_DROP can be subject
*       to MPLS router engine.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - device number
*       routerTriggerType - MPLS router trigger type
*
* OUTPUTS:
*       enablePtr         - (pointer to) router trigger mode
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - bad paramter
*       GT_BAD_PTR      - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsSpecialRouterTriggerEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TYPE_ENT routerTriggerType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    routerTriggerType = (CPSS_EXMXPM_MPLS_SPECIAL_ROUTER_TRIGGER_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMplsSpecialRouterTriggerEnableGet(devNum, routerTriggerType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexSet
*
* DESCRIPTION:
*       Set MPLS Special Labels Table base index
*       Used for MPLS labels in the range 0-15.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       baseIndex    - base index of the MPLS Special Labels in the NHLFE table.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - bad paramter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It is recommended to set baseIndex to 0, meaning that these labels are
*       located at the beggining of the NHLFE Table.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsNhlfeSpecialLabelsBaseIndexSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 baseIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    baseIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexSet(devNum, baseIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet
*
* DESCRIPTION:
*       Get MPLS Special Labels Table base index
*       Used for MPLS labels in the range 0-15.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       baseIndexPtr - (pointer to) base index of the MPLS Special Labels
*                   in the NHLFE table.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PTR      - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If baseIndex is 0 it means that these labels are located
*       at the beggining of the NHLFE Table
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 baseIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet(devNum, &baseIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", baseIndex);

    return CMD_OK;
}
#define NHLF_ARRAY_GEN_SIZE 512
static CPSS_EXMXPM_MPLS_NHLFE_STC nhlfGenArr[NHLF_ARRAY_GEN_SIZE];
static GT_U32 setNhlfEntriesGenIndex;
static GT_U32 getNhlfEntriesGenIndex;
static GT_U32 getNumOfNhlfGenEntries;
/*******************************************************************************
* cpssExMxPmMplsNhlfEntriesWrite
*
* DESCRIPTION:
*       Write an array of NHLF entries to hw.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                  - device number
*       nhlfEntryBaseIndex      - The base Index in the NHLFE table
*                                 The entries will be written from this base on.
*       numOfNhlfEntries        - The number NHLF entries to write.
*       nhlfEntriesArrayPtr     - (pointer to) The NHLF entries array
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       No partial write.
*
*******************************************************************************/
static void prvWrCpssExMxPmMplsNhlfGenSetEntry
(
    IN  GT_32  inFields[CMD_MAX_ARGS],
    IN  GT_U32 index
)
{
    /*common params*/
    nhlfGenArr[index].nhlfeCommonParams.mplsLabel=(GT_U32)inFields[0];
    nhlfGenArr[index].nhlfeCommonParams.mplsCmd=(CPSS_EXMXPM_MPLS_CMD_ENT)inFields[1];

    nhlfGenArr[index].nhlfeCommonParams.outlifConfig.outlifType=(CPSS_EXMXPM_OUTLIF_TYPE_ENT)inFields[2];



    switch(nhlfGenArr[index].nhlfeCommonParams.outlifConfig.outlifType)
    {
    case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
        nhlfGenArr[index].nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr=(GT_U32)inFields[3];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
        nhlfGenArr[index].nhlfeCommonParams.outlifConfig.outlifPointer.ditPtr=(GT_U32)inFields[4];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:

        nhlfGenArr[index].nhlfeCommonParams.outlifConfig.outlifPointer.tunnelStartPtr.passengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[5];
        nhlfGenArr[index].nhlfeCommonParams.outlifConfig.outlifPointer.tunnelStartPtr.ptr=(GT_U32)inFields[6];


        break;
    default:
        break;
    }




    nhlfGenArr[index].nhlfeCommonParams.outlifConfig.interfaceInfo.type=(CPSS_INTERFACE_TYPE_ENT)inFields[7] ;

    switch(nhlfGenArr[index].nhlfeCommonParams.outlifConfig.interfaceInfo.type)
    {
    case CPSS_INTERFACE_PORT_E/*0*/:
        nhlfGenArr[index].nhlfeCommonParams.outlifConfig.interfaceInfo.devPort.devNum = (GT_U8)inFields[8];
        nhlfGenArr[index].nhlfeCommonParams.outlifConfig.interfaceInfo.devPort.portNum = (GT_U8)inFields[9];
        CONVERT_DEV_PORT_DATA_MAC(nhlfGenArr[index].nhlfeCommonParams.outlifConfig.interfaceInfo.devPort.devNum,
                   nhlfGenArr[index].nhlfeCommonParams.outlifConfig.interfaceInfo.devPort.portNum);

        break;

    case CPSS_INTERFACE_TRUNK_E/*1*/:
        nhlfGenArr[index].nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[10];
        break;

    case CPSS_INTERFACE_VIDX_E/*2*/:
        nhlfGenArr[index].nhlfeCommonParams.outlifConfig.interfaceInfo.vidx = (GT_U16)inFields[11];
        break;


    case CPSS_INTERFACE_FABRIC_VIDX_E/*5*/:
        nhlfGenArr[index].nhlfeCommonParams.outlifConfig.interfaceInfo.fabricVidx = (GT_U16)inFields[12];
        break;

    default:
        break;
    }




    nhlfGenArr[index].nhlfeCommonParams.nextHopVlanId=(GT_U16)inFields[13];
    nhlfGenArr[index].nhlfeCommonParams.ageRefresh=(GT_BOOL)inFields[14];
    nhlfGenArr[index].nhlfeCommonParams.mtuProfileIndex=(GT_U32)inFields[15];
    nhlfGenArr[index].nhlfeCommonParams.counterSetIndex=(CPSS_EXMXPM_MPLS_COUNTER_SET_INDEX_ENT)inFields[16];
    nhlfGenArr[index].nhlfeCommonParams.mirrorToIngressAnalyzer=(GT_BOOL)inFields[17];
    nhlfGenArr[index].nhlfeCommonParams.ttl=(GT_U32)inFields[18];
    nhlfGenArr[index].nhlfeCommonParams.ttlMode=(CPSS_EXMXPM_MPLS_TTL_MODE_ENT)inFields[19];
    nhlfGenArr[index].nhlfeCommonParams.bypassTtlExceptionCheckEnable=(GT_BOOL)inFields[20];
    nhlfGenArr[index].nhlfeCommonParams.cpuCodeIndex=(GT_U32)inFields[21];
    nhlfGenArr[index].nhlfeCommonParams.packetCmd=(CPSS_PACKET_CMD_ENT)inFields[22];

    /*qosModify and qos params*/
    nhlfGenArr[index].qosParamsModify.modifyTc=(GT_BOOL)inFields[23];
    nhlfGenArr[index].qosParams.tc=(GT_U32)inFields[24];
    nhlfGenArr[index].qosParamsModify.modifyUp=(GT_BOOL)inFields[25];
    nhlfGenArr[index].qosParams.up=(GT_U32)inFields[26];
    nhlfGenArr[index].qosParamsModify.modifyDp=(GT_BOOL)inFields[27];
    nhlfGenArr[index].qosParams.dp=(CPSS_DP_LEVEL_ENT)inFields[28];
    nhlfGenArr[index].qosParamsModify.modifyDscp=(GT_BOOL)inFields[29];
    nhlfGenArr[index].qosParams.dscp=(GT_U32)inFields[30];
    nhlfGenArr[index].qosParamsModify.modifyExp=(GT_BOOL)inFields[31];
    nhlfGenArr[index].qosParams.exp=(GT_U32)inFields[32];

}



/*****************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsNhlfEntriesWriteSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    setNhlfEntriesGenIndex=0;

    prvWrCpssExMxPmMplsNhlfGenSetEntry(inFields,setNhlfEntriesGenIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsNhlfEntriesWriteSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    setNhlfEntriesGenIndex++;

    prvWrCpssExMxPmMplsNhlfGenSetEntry(inFields,setNhlfEntriesGenIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsNhlfEntriesWriteEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 nhlfEntryBaseIndex;



    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    nhlfEntryBaseIndex=(GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMplsNhlfEntriesWrite(devNum, nhlfEntryBaseIndex, setNhlfEntriesGenIndex+1, nhlfGenArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmMplsNhlfEntriesRead
*
* DESCRIPTION:
*       Read an array of NHLF entries from hw.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                  - device number
*       nhlfEntryBaseIndex      - The base Index in the NHLFE table
*                                 The entries will be read from this base on.
*       numOfNhlfEntries        - The number NHLF entries to read.
*
*
* OUTPUTS:
*       nhlfEntriesArrayPtr     - (pointer to) The NHLF entries array
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - on NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsNhlfEntriesReadNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8 tempDevNum;
    GT_U8 tempPortNum;

    if (getNumOfNhlfGenEntries==0||getNhlfEntriesGenIndex>getNumOfNhlfGenEntries-1)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    cmdOsMemSet(inFields,0,CMD_MAX_FIELDS);

    /*common params */

    inFields[0]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.mplsLabel;
    inFields[1]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.mplsCmd;
    inFields[2]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.outlifType;



    switch(nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.outlifType)
    {
    case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
        inFields[3]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.outlifPointer.arpPtr;
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
        inFields[4]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.outlifPointer.ditPtr;
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:

        inFields[5]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.outlifPointer.tunnelStartPtr.passengerPacketType;
        inFields[6]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.outlifPointer.tunnelStartPtr.ptr;
        break;
    default:
        break;
    }




    inFields[7]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.interfaceInfo.type;

    switch(nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.interfaceInfo.type)
    {
    case CPSS_INTERFACE_PORT_E:
        tempDevNum=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.interfaceInfo.devPort.devNum;
        tempPortNum=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.interfaceInfo.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempDevNum,tempPortNum)

        inFields[8]=tempDevNum;
        inFields[9]=tempPortNum;

        break;

    case CPSS_INTERFACE_TRUNK_E:
        inFields[10]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.interfaceInfo.trunkId;
        break;

    case CPSS_INTERFACE_VIDX_E:
        inFields[11]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.interfaceInfo.vidx;
        break;


    case CPSS_INTERFACE_FABRIC_VIDX_E:
        inFields[12]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.outlifConfig.interfaceInfo.fabricVidx;
        break;

    default:
        break;
    }




    inFields[13]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.nextHopVlanId;
    inFields[14]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.ageRefresh;
    inFields[15]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.mtuProfileIndex;
    inFields[16]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.counterSetIndex;
    inFields[17]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.mirrorToIngressAnalyzer;
    inFields[18]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.ttl;
    inFields[19]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.ttlMode;
    inFields[20]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.bypassTtlExceptionCheckEnable;
    inFields[21]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.cpuCodeIndex;
    inFields[22]=nhlfGenArr[getNhlfEntriesGenIndex].nhlfeCommonParams.packetCmd;

    /*qosModify and qos params*/
    inFields[23]=nhlfGenArr[getNhlfEntriesGenIndex].qosParamsModify.modifyTc;
    inFields[24]=nhlfGenArr[getNhlfEntriesGenIndex].qosParams.tc;
    inFields[25]=nhlfGenArr[getNhlfEntriesGenIndex].qosParamsModify.modifyUp;
    inFields[26]=nhlfGenArr[getNhlfEntriesGenIndex].qosParams.up;
    inFields[27]=nhlfGenArr[getNhlfEntriesGenIndex].qosParamsModify.modifyDp;
    inFields[28]=nhlfGenArr[getNhlfEntriesGenIndex].qosParams.dp;
    inFields[29]=nhlfGenArr[getNhlfEntriesGenIndex].qosParamsModify.modifyDscp;
    inFields[30]=nhlfGenArr[getNhlfEntriesGenIndex].qosParams.dscp;
    inFields[31]=nhlfGenArr[getNhlfEntriesGenIndex].qosParamsModify.modifyExp;
    inFields[32]=nhlfGenArr[getNhlfEntriesGenIndex].qosParams.exp;







    getNhlfEntriesGenIndex++;

       /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],  inFields[6],
                inFields[7],  inFields[8],  inFields[9],  inFields[10],
                inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17], inFields[18],
                inFields[19], inFields[20], inFields[21], inFields[22],
                inFields[23], inFields[24],inFields[25],inFields[26],
                inFields[27], inFields[28],inFields[29],inFields[30],
                inFields[31], inFields[32]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}
/*****************************************************************************************************/

static CMD_STATUS wrCpssExMxPmMplsNhlfEntriesReadFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 nhlfEntryBaseIndex;




    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    nhlfEntryBaseIndex=(GT_U32)inArgs[1];
    getNumOfNhlfGenEntries=(GT_32)inArgs[2];

    cmdOsMemSet(nhlfGenArr,0,NHLF_ARRAY_GEN_SIZE);

    /* call cpss api function */
    result = cpssExMxPmMplsNhlfEntriesRead(devNum, nhlfEntryBaseIndex, getNumOfNhlfGenEntries, nhlfGenArr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    getNhlfEntriesGenIndex=0;

   return wrCpssExMxPmMplsNhlfEntriesReadNext(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxPmMplsExceptionCmdSet
*
* DESCRIPTION:
*       Set the exception command assigned to MPLS packets that are to be routed.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       exceptionType - Currently included exceptions:
*                      - Incoming TTL exception
*                      - Outgoing TTL exception
*                      - MPLS MTU Exceeded
*
*
*       exceptionCmd - Valid commands:
*           CPSS_PACKET_CMD_ROUTE_E : valid only for CPSS_EXMXPM_MPLS_MTU_EXCP_E
*           CPSS_PACKET_CMD_TRAP_TO_CPU_E: Packets are trapped to the CPU with
*               a CPU code of:
*               MPLS_TTL0_EXCEED - for Incoming TTL exception
*               MPLS_TTL1_EXCEED - for outgoing TTL exception
*               MPLS_MTU_EXCEEDED- for MPLS MTU Exceeded exception
*           CPSS_PACKET_CMD_DROP_HARD_E
*           CPSS_PACKET_CMD_DROP_SOFT_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - bad paramter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       TTL is about to be exceeded if the packet TTL=1 and
*       NHLFE<TTL Mode>="Dec TTL", or, if NHLFE<TTL Mode>="Dec By Entry",
*       and the NHLFE <TTL> is larger than the packet TTL
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsExceptionCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MPLS_EXCEPTION_TYPE_ENT exceptionType;
    CPSS_PACKET_CMD_ENT exceptionCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_EXMXPM_MPLS_EXCEPTION_TYPE_ENT)inArgs[1];
    exceptionCmd = (CPSS_PACKET_CMD_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmMplsExceptionCmdSet(devNum, exceptionType, exceptionCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMplsExceptionCmdGet
*
* DESCRIPTION:
*       Get the exception command assigned to MPLS packets that are to be routed.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       exceptionType - Currently included exceptions:
*                      - Incoming TTL exception
*                      - Outgoing TTL exception
*                      - MPLS MTU Exceeded
*
* OUTPUTS:
*       exceptionCmdPtr - (pointer to)  Valid commands:
*           CPSS_PACKET_CMD_ROUTE_E : valid only for CPSS_EXMXPM_MPLS_MTU_EXCP_E
*           CPSS_PACKET_CMD_TRAP_TO_CPU_E: Packets are trapped to the CPU with
*               a CPU code of:
*               MPLS_TTL0_EXCEED - for Incoming TTL exception
*               MPLS_TTL1_EXCEED - for outgoing TTL exception
*               MPLS_MTU_EXCEEDED- for MPLS MTU Exceeded exception
*           CPSS_PACKET_CMD_DROP_HARD_E
*           CPSS_PACKET_CMD_DROP_SOFT_E
**
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - bad paramter
*       GT_BAD_PTR      - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       TTL is about to be exceeded if the packet TTL=1 and
*       NHLFE<TTL Mode>="Dec TTL", or, if NHLFE<TTL Mode>="Dec By Entry",
*       and the NHLFE <TTL> is larger than the packet TTL
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsExceptionCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MPLS_EXCEPTION_TYPE_ENT exceptionType;
    CPSS_PACKET_CMD_ENT exceptionCmd;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_EXMXPM_MPLS_EXCEPTION_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMplsExceptionCmdGet(devNum, exceptionType, &exceptionCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", exceptionCmd);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmMplsFastRerouteEnableSet
*
* DESCRIPTION:
*      Enables/disables Fast Reroute feature.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum                  - the device number
*       enable                  - if GT_TRUE - FRR feature is enabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsFastRerouteEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    IN   GT_U8          devNum;
    IN   GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMplsFastRerouteEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPmMplsFastRerouteEnableGet
*
* DESCRIPTION:
*      Gets Fast Reroute feature enabled/disabled status.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum                  - the device number
*
* OUTPUTS:
*       enablePtr               - if GT_TRUE - FRR feature is enabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsFastRerouteEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    IN   GT_U8          devNum;
    OUT  GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmMplsFastRerouteEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*----- cpssExMxPmMplsFastRerouteTable -----*/

/** global variables for cpssExMxPmMplsFastRerouteTable **/
#define MAX_FRR_ENTRIES 2048
static GT_32  FRR_EntryIndex;
static GT_32  FRRrangeStart;
static GT_32  FRRrangeEnd;
/** END: global variables for cpssExMxPmMplsFastRerouteTable **/


/*******************************************************************************
* cpssExMxPmMplsFastRerouteEntrySet
*
* DESCRIPTION:
*      Configures single Fast Reroute table entry.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum                  - the device number
*       index                   - index of table entry to configure.
*                                 Values 0..2047.
*       frrEntryPtr             - (pointer to)Fast Reroute Table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsFastRerouteTableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    IN GT_U8                             devNum;

    IN GT_U32                            index;
    IN CPSS_EXMXPM_MPLS_FAST_REROUTE_STC frrEntry;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals and table fields to 'inlifEntry' structure */
    devNum = (GT_U8)inArgs[0];

    index = (GT_U32)inFields[0]; /* iterator */

    cmdOsMemSet(&frrEntry, 0, sizeof(frrEntry));

    /*** mapping table fields to 'inlifEntry' structure ***/

    /* frrEntry.outLif.outlifType = inArgs[]; */

    frrEntry.outLif.interfaceInfo.devPort.portNum = (GT_U8)inFields[1];
    frrEntry.outLif.outlifType = (CPSS_EXMXPM_OUTLIF_TYPE_ENT)inFields[2];
    frrEntry.outLif.interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[3];

    frrEntry.outLif.interfaceInfo.devPort.devNum = devNum;

    frrEntry.outLif.interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[4];
    frrEntry.outLif.interfaceInfo.vidx = (GT_U16)inFields[5];
    frrEntry.outLif.interfaceInfo.vlanId = (GT_U16)inFields[6];
    frrEntry.outLif.interfaceInfo.devNum = (GT_U8)inFields[7];
    frrEntry.outLif.interfaceInfo.fabricVidx = (GT_16)inFields[8];

    switch (frrEntry.outLif.outlifType) { /* because of 'union' */
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            frrEntry.outLif.outlifPointer.arpPtr = (GT_U32)inFields[9];
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            frrEntry.outLif.outlifPointer.ditPtr = (GT_U32)inFields[10];
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            frrEntry.outLif.outlifPointer.tunnelStartPtr.passengerPacketType =
                (CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[11];
            frrEntry.outLif.outlifPointer.tunnelStartPtr.ptr = inFields[12];
            break;
        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\noutlifType field has unsupported value!\n");
            return CMD_AGENT_ERROR;
    }

    /* call cpss api function */
    result = cpssExMxPmMplsFastRerouteEntrySet(devNum, index, &frrEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmMplsFastRerouteEntryGet
*
* DESCRIPTION:
*      Gets single Fast Reroute table entry content.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       devNum                  - the device number
*       index                   - index of table entry to configure.
*                                 Values 0..2047.
*
* OUTPUTS:
*       frrEntryPtr             - (pointer to)Fast Reroute Table entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMplsFastRerouteTableGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    IN GT_U8                             devNum;

    IN CPSS_EXMXPM_MPLS_FAST_REROUTE_STC frrEntry;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals and table fields to 'inlifEntry' structure */
    devNum = (GT_U8)inArgs[0];
    FRRrangeStart = (GT_32)inArgs[1];
    FRRrangeEnd   = (GT_32)inArgs[2];

    FRR_EntryIndex = FRRrangeStart;

    if (FRRrangeEnd < FRRrangeStart)
    {
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nRange of indices - INVALID");
        return CMD_AGENT_ERROR;
    }
    else if (FRRrangeStart < 0 && FRRrangeEnd < 0)
    {
        FRR_EntryIndex = 0;
        FRRrangeEnd = MAX_FRR_ENTRIES - 1;
    }
    else if (FRRrangeEnd > MAX_FRR_ENTRIES - 1)
    {
        FRRrangeEnd = MAX_FRR_ENTRIES - 1;
    }

    /* memset the struct before using it to get data from HW*/
    cmdOsMemSet(&frrEntry,0,sizeof(frrEntry));

    /* call cpss API */
    result = cpssExMxPmMplsFastRerouteEntryGet(devNum, FRR_EntryIndex, &frrEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /*** mapping 'inlifEntry' structure into table fields  ***/

    inFields[0] = FRR_EntryIndex; /* iterator */

    /* frrEntry.outLif.outlifType = inArgs[]; */

    inFields[1] = frrEntry.outLif.interfaceInfo.devPort.portNum;
    inFields[2] = frrEntry.outLif.outlifType;
    inFields[3] = frrEntry.outLif.interfaceInfo.type;

    frrEntry.outLif.interfaceInfo.devPort.devNum = devNum;

    inFields[4] = frrEntry.outLif.interfaceInfo.trunkId;
    inFields[5] = frrEntry.outLif.interfaceInfo.vidx;
    inFields[6] = frrEntry.outLif.interfaceInfo.vlanId;
    inFields[7] = frrEntry.outLif.interfaceInfo.devNum;
    inFields[8] = frrEntry.outLif.interfaceInfo.fabricVidx;

    switch (inFields[2]) { /* because of 'union' */
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            inFields[9] = frrEntry.outLif.outlifPointer.arpPtr;
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            inFields[10] = frrEntry.outLif.outlifPointer.ditPtr;
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            inFields[11] =
                frrEntry.outLif.outlifPointer.tunnelStartPtr.passengerPacketType;
            inFields[12] =
                frrEntry.outLif.outlifPointer.tunnelStartPtr.ptr;
            break;
        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\noutlifType field has unsupported value!\n");
            return CMD_AGENT_ERROR;
    }


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12]
                );

    galtisOutput(outArgs, GT_OK, "%f");

    FRR_EntryIndex++;

    return CMD_OK;
}

static CMD_STATUS wrCpssExMxPmMplsFastRerouteTableGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    IN GT_U8                             devNum;

    IN CPSS_EXMXPM_MPLS_FAST_REROUTE_STC frrEntry;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*** map input arguments to locals ***/
    devNum = (GT_U8)inArgs[0];

    if (FRR_EntryIndex > FRRrangeEnd) /* all of the range has been used-up */
    {
        galtisOutput(outArgs, GT_OK, "%d",  -1);
        return CMD_OK;
    }

    /* memset the struct before using it to get data from HW*/
    cmdOsMemSet(&frrEntry,0,sizeof(frrEntry));

    /* call cpss API */
    result = cpssExMxPmMplsFastRerouteEntryGet(devNum, FRR_EntryIndex, &frrEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /*** mapping 'inlifEntry' structure into table fields  ***/

    inFields[0] = FRR_EntryIndex; /* iterator */

    /* frrEntry.outLif.outlifType = inArgs[]; */

    inFields[1] = frrEntry.outLif.interfaceInfo.devPort.portNum;
    inFields[2] = frrEntry.outLif.outlifType;
    inFields[3] = frrEntry.outLif.interfaceInfo.type;

     /* frrEntry.outLif.interfaceInfo.devPort.devNum = devNum; */

    inFields[4] = frrEntry.outLif.interfaceInfo.trunkId;
    inFields[5] = frrEntry.outLif.interfaceInfo.vidx;
    inFields[6] = frrEntry.outLif.interfaceInfo.vlanId;
    inFields[7] = frrEntry.outLif.interfaceInfo.devNum;
    inFields[8] = frrEntry.outLif.interfaceInfo.fabricVidx;

    switch (inFields[2]) { /* because of 'union' */
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            inFields[9] = frrEntry.outLif.outlifPointer.arpPtr;
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            inFields[10] = frrEntry.outLif.outlifPointer.ditPtr;
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            inFields[11] =
                frrEntry.outLif.outlifPointer.tunnelStartPtr.passengerPacketType;
            inFields[12] =
                frrEntry.outLif.outlifPointer.tunnelStartPtr.ptr;
            break;
        default:
            galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\noutlifType field has unsupported value!\n");
            return CMD_AGENT_ERROR;
    }


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12]
                );

    galtisOutput(outArgs, GT_OK, "%f");

    FRR_EntryIndex++;

    return CMD_OK;
}
/*--- END: cpssExMxPmMplsFastRerouteTable ---*/


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPm2MplsLabelSpaceModeSet",
         &wrCpssExMxPmMplsLabelSpaceModeSet,
         2, 0},
        {"cpssExMxPm2MplsLabelSpaceModeGet",
         &wrCpssExMxPmMplsLabelSpaceModeGet,
         1, 0},
        {"cpssExMxPm2MplsEnableSet",
         &wrCpssExMxPmMplsEnableSet,
         2, 0},
        {"cpssExMxPm2MplsEnableGet",
         &wrCpssExMxPmMplsEnableGet,
         1, 0},
        {"cpssExMxPm2MplsInterfaceTableSet",
         &wrCpssExMxPmMplsInterfaceEntrySet,
         1, 7},
        {"cpssExMxPm2MplsInterfaceTableGetFirst",
         &wrCpssExMxPmMplsInterfaceEntryGetFirst,
         3, 0},
        {"cpssExMxPm2MplsInterfaceTableGetNext",
         &wrCpssExMxPmMplsInterfaceEntryGetNext,
         3, 0},

        {"cpssExMxPm2MplsInvalidEntryCmdSet",
         &wrCpssExMxPmMplsInvalidEntryCmdSet,
         2, 0},
        {"cpssExMxPm2MplsInvalidEntryCmdGet",
         &wrCpssExMxPmMplsInvalidEntryCmdGet,
         1, 0},
        {"cpssExMxPm2MplsSpecialRouterTriggerEnableSet",
         &wrCpssExMxPmMplsSpecialRouterTriggerEnableSet,
         3, 0},
        {"cpssExMxPm2MplsSpecialRouterTriggerEnableGet",
         &wrCpssExMxPmMplsSpecialRouterTriggerEnableGet,
         2, 0},
        {"cpssExMxPm2MplsNhlfeSpecialLabelsBaseIndexSet",
         &wrCpssExMxPmMplsNhlfeSpecialLabelsBaseIndexSet,
         2, 0},
        {"cpssExMxPm2MplsNhlfeSpecialLabelsBaseIndexGet",
         &wrCpssExMxPmMplsNhlfeSpecialLabelsBaseIndexGet,
         1, 0},
        {"cpssExMxPm2MplsNhlfEntriesSetFirst",
         &wrCpssExMxPmMplsNhlfEntriesWriteSetFirst,
         2, 33},
        {"cpssExMxPm2MplsNhlfEntriesSetNext",
         &wrCpssExMxPmMplsNhlfEntriesWriteSetNext,
         2, 34},
        {"cpssExMxPm2MplsNhlfEntriesEndSet",
         &wrCpssExMxPmMplsNhlfEntriesWriteEndSet,
         2, 0},
        {"cpssExMxPm2MplsNhlfEntriesGetFirst",
         &wrCpssExMxPmMplsNhlfEntriesReadFirst,
         3, 0},
        {"cpssExMxPm2MplsNhlfEntriesGetNext",
         &wrCpssExMxPmMplsNhlfEntriesReadNext,
         3, 0},
        {"cpssExMxPm2MplsExceptionCmdSet",
         &wrCpssExMxPmMplsExceptionCmdSet,
         3, 0},
        {"cpssExMxPm2MplsExceptionCmdGet",
         &wrCpssExMxPmMplsExceptionCmdGet,
         2, 0},

/*** START: revision 4 ***/
        {"cpssExMxPmMplsFastRerouteEnableSet",
         &wrCpssExMxPmMplsFastRerouteEnableSet,
         2, 0},
        {"cpssExMxPmMplsFastRerouteEnableGet",
         &wrCpssExMxPmMplsFastRerouteEnableGet,
         1, 0},

        /*** cpssExMxPmMplsFastRerouteTable ***/
        {"cpssExMxPmMplsFastRerouteTableSet",
            &wrCpssExMxPmMplsFastRerouteTableSet,
            1, 13},
        {"cpssExMxPmMplsFastRerouteTableGetFirst",
            &wrCpssExMxPmMplsFastRerouteTableGetFirst,
            3, 0},
        {"cpssExMxPmMplsFastRerouteTableGetNext",
            &wrCpssExMxPmMplsFastRerouteTableGetNext,
            3, 0}
        /*** END: cpssExMxPmMplsFastRerouteTable ***/
/*** END: revision 4 ***/
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmMpls
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
GT_STATUS cmdLibInitCpssExMxPmMpls
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

