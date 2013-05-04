/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgStp.c
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
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgStp.h>


static GT_U32   stpIdCnt;
static GT_U32   stgIdMax=0;
/*******************************************************************************
* cpssExMxPmBrgStpEntryRead
*
* DESCRIPTION:
*       Reads a raw STP group entry from HW.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       stgId        - STG (Spanning Tree Group) index  (0..255)
*
* OUTPUTS:
*       stgEntryPtr  - (Pointer to) The raw stp entry to get.
*                      memory allocated by the caller.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad device
*       GT_OUT_OF_RANGE - on stgId is larger than allowed
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgStpEntryReadFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum,i;
    GT_U32 stgEntry[CPSS_EXMXPM_STG_ENTRY_SIZE_CNS];


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    stpIdCnt = 0;

    /* call cpss api function */
    result = cpssExMxPmBrgStpEntryRead(devNum, stpIdCnt, stgEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = stpIdCnt;

    for(i = 0; i < CPSS_EXMXPM_STG_ENTRY_SIZE_CNS; i++)
    {
        inFields[i + 1] = stgEntry[i];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],inFields[3], inFields[4]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgStpEntryReadNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum,i;
    GT_U32 stgEntry[CPSS_EXMXPM_STG_ENTRY_SIZE_CNS];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    stpIdCnt++;

    if(stpIdCnt > stgIdMax)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


    /* call cpss api function */
    result = cpssExMxPmBrgStpEntryRead(devNum, stpIdCnt, stgEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = stpIdCnt;

    for(i = 0; i < CPSS_EXMXPM_STG_ENTRY_SIZE_CNS; i++)
    {
        inFields[i + 1] = stgEntry[i];
    }

    /* pack and output table fields */
     fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],inFields[3], inFields[4]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmBrgStpEntryWrite
*
* DESCRIPTION:
*       Writes a raw STP group entry to HW.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       stgId        - STG (Spanning Tree Group) index  (0..255)
*       stgEntryPtr  - (Pointer to) The raw stp entry to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad device
*       GT_OUT_OF_RANGE - on stgId is larger than allowed
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgStpEntryWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum,i;
    GT_U32 stgId;
    GT_U32 stgEntry[CPSS_EXMXPM_STG_ENTRY_SIZE_CNS];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    stgId = (GT_U8)inFields[0];

    if(stgId>stgIdMax)stgIdMax=stgId;

    for(i = 0; i < CPSS_EXMXPM_STG_ENTRY_SIZE_CNS; i++)
    {
        stgEntry[i] = (GT_U32)inFields[i + 1];
    }

    /* call cpss api function */
    result = cpssExMxPmBrgStpEntryWrite(devNum, stgId, stgEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgStpPortStateSet
*
* DESCRIPTION:
*       Sets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       portNum      - port number
*       stgId        - STG (Spanning Tree Group) index  (0..255)
*       state        - STP port state.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad device or port or stgId or state
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgStpPortStateSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 stgId;
    CPSS_STP_STATE_ENT state;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    stgId = (GT_U32)inArgs[2];
    state = (CPSS_STP_STATE_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgStpPortStateSet(devNum, portNum, stgId, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgStpPortStateGet
*
* DESCRIPTION:
*       Get STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       portNum      - port number
*       stgId        - STG (Spanning Tree Group) index  (0..255)
*
* OUTPUTS:
*       statePtr     - (pointer ot) STP port state.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad device or port or stgId
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_BAD_VALUE - on bad stp state value
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgStpPortStateGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_U32 stgId;
    CPSS_STP_STATE_ENT state;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    stgId = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgStpPortStateGet(devNum, portNum, stgId, &state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", state);

    return CMD_OK;
}

/**** database initialization **************************************/




static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmBrgStpEntryGetFirst",
         &wrCpssExMxPmBrgStpEntryReadFirst,
         1, 0},
        {"cpssExMxPmBrgStpEntryGetNext",
         &wrCpssExMxPmBrgStpEntryReadNext,
         1, 0},
        {"cpssExMxPmBrgStpEntrySet",
         &wrCpssExMxPmBrgStpEntryWrite,
         1, 5},
        {"cpssExMxPmBrgStpPortStateSet",
         &wrCpssExMxPmBrgStpPortStateSet,
         4, 0},
        {"cpssExMxPmBrgStpPortStateGet",
         &wrCpssExMxPmBrgStpPortStateGet,
         3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmBrgStp
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
GT_STATUS cmdLibInitCpssExMxPmBrgStp
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

