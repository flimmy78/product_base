/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgMc.c
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
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgMc.h>

/*******************************************************************************
* cpssExMxPmBrgMcIpv6BytesSelectSet
*
* DESCRIPTION:
*       
*       Sets selected 4 bytes from the IPv6 SIP and 4 bytes from the IPv6 DIP 
*       that are used for IPv6 MC bridging, based on the packet's IP addresses. 
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum              - device number.
*       dipBytesSelectMap   - Array, which contains indexes of 4 bytes from the
*                             DIP IPv6 address are used for IPv6 MC bridging.   
*                             Value in cell i must be lower then value in 
*                             cell i+1.
*
*       sipBytesSelectMap   - Array, which contains indexes of 4 bytes from the 
*                             SIP IPv6 address are used for IPv6 MC bridging.   
*                             Value in cell i must be lower then value in 
*                             cell i+1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_BAD_PTR          - on NULL pointers.
*       GT_HW_ERROR         - on hardware error.
*
* COMMENTS:
*       The indexes of IPv6 address bytes are in the 
*       Network order and in the range [0:15], where 0
*       is MSB of IP address and 15 is LSB of IP address.
*                             
*       IPv6 address is 128 bits length, ExMxPM set only 32 bits for DIP
*       and 32 bits for SIP, due to the limitation of the FDB Entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgMcIpv6BytesSelectSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum,i;
    GT_U8 dipBytesSelectMap[4];
    GT_U8 sipBytesSelectMap[4];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];


    for(i = 0; i < 4; i++)
    {
        dipBytesSelectMap[i] = (GT_U8)inFields[i];
    }

    for(i = 0; i < 4; i++)
    {
        sipBytesSelectMap[i] = (GT_U8)inFields[i + 4];
    }

    /* call cpss api function */
    result = cpssExMxPmBrgMcIpv6BytesSelectSet(devNum, dipBytesSelectMap, sipBytesSelectMap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgMcIpv6BytesSelectGet
*
* DESCRIPTION:
*       
*       Gets selected 4 bytes from the IPv6 SIP and 4 bytes from the IPv6 DIP 
*       are used for IPv6 MC bridging, based on the packet's IP addresses.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum              - device number.
*
* OUTPUTS:
*       dipBytesSelectMap   - Array, which contains indexes of 4 bytes from the
*                             DIP IPv6 address are used for IPv6 MC bridging.   
*                             Value in cell i must be lower then value in 
*                             cell i+1.
*
*       sipBytesSelectMap   - Array, which contains indexes of 4 bytes from the 
*                             SIP IPv6 address are used for IPv6 MC bridging.   
*                             Value in cell i must be lower then value in 
*                             cell i+1.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PTR          - on NULL pointers.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum.
*
* COMMENTS:
*       The indexes of IPv6 address bytes are in the 
*       Network order and in the range [0:15], where 0
*       is MSB of IP address and 15 is LSB of IP address.
*
*       IPv6 address is 128 bits length, ExMxPM set only 32 bits for DIP
*       and 32 bits for SIP, due to the limitation of the FDB Entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgMcIpv6BytesSelectGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum,i;
    GT_U8 dipBytesSelectMap[4];
    GT_U8 sipBytesSelectMap[4];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgMcIpv6BytesSelectGet(devNum, dipBytesSelectMap, sipBytesSelectMap);

  if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    for(i = 0; i < 4; i++)
    {
        inFields[i] = dipBytesSelectMap[i];
    }

    for(i = 0; i < 4; i++)
    {
        inFields[i + 4] = sipBytesSelectMap[i];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1],
                       inFields[2], inFields[3], inFields[4],
                       inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

static CMD_STATUS wrCpssExMxPmBrgMcIpv6BytesSelectGetNext
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
* cpssExMxPmBrgMcEntryWrite
*
* DESCRIPTION:
*       Writes Multicast Group entry to the HW.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
*
* INPUTS:
*       devNum          - device number.
*       vidx            - multicast group entry index.
*       isCpuMember     - if GT_TRUE, CPU is a member of this multicast group
*       portBitmapPtr   - pointer to the bitmap of ports which belong to the 
*                         vidx MC group of specified PP device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum or ports bitmap value.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - if vidx is larger than allowed.
*       GT_HW_ERROR     - on hardware error.
*
* COMMENTS:
*       By Default last entry in MC Group Table is all ones, and it 
*       used for flooding traffic to all Vlan Ports.
*       It is highly recommanded not changing the default.
*       
*******************************************************************************/
/***********************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgMcEntryWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vidx;
    GT_BOOL isCpuMember;
    CPSS_PORTS_BMP_STC portBitmap;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    vidx = (GT_U16)inFields[0];

    isCpuMember=(GT_BOOL)inFields[1]; 
   
    portBitmap.ports[0] = inFields[2];
    portBitmap.ports[1] = inFields[3];

    /* call cpss api function */
    result = cpssExMxPmBrgMcEntryWrite(devNum, vidx, isCpuMember, &portBitmap);    

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgMcEntryRead
*
* DESCRIPTION:
*       Reads the Multicast Group entry from the HW.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       vidx            - multicast group entry index.
*
* OUTPUTS:
*       portBitmapPtr   - pointer to the bitmap of ports are belonged to the 
*                         vidx MC group of specified PP device.
*       isCpuMemberPtr  - if GT_TRUE, CPU is a member of this multicast group
*       portBitmapPtr   - pointer to the bitmap of ports are belonged to the 
*                         vidx MC group of specified PP device.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - if vidx is larger than allowed.
*       GT_HW_ERROR     - on hardware error.
*
* COMMENTS:
*/



static GT_U16   vidxIndex;
static GT_U16   vidxMaxGet;

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgMcEntryReadFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16   tmpEntryGet;
    GT_U16 vidx;
    GT_BOOL isCpuMember;
    CPSS_PORTS_BMP_STC portBitmap;
    
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vidx = (GT_U16)inArgs[1];
    tmpEntryGet = (GT_U16)inArgs[2];
    vidxMaxGet = vidx+ tmpEntryGet;
    vidxIndex=vidx;

   
    /* call cpss api function */
    result = cpssExMxPmBrgMcEntryRead(devNum, vidx, &isCpuMember, &portBitmap);
   
   
    inFields[0] = vidx;
    inFields[1]=isCpuMember;
    inFields[2] = portBitmap.ports[0];
    inFields[3] = portBitmap.ports[1];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2],inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;

   
}
static CMD_STATUS wrCpssExMxPmBrgMcEntryReadNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL isCpuMember;
    CPSS_PORTS_BMP_STC portBitmap;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    
    devNum = (GT_U8)inArgs[0];
    vidxIndex++;

    if(vidxIndex > 4095 || vidxIndex >= vidxMaxGet)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


   

    /* call cpss api function */
    result = cpssExMxPmBrgMcEntryRead(devNum, vidxIndex, &isCpuMember, &portBitmap);

   if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    
   
    inFields[0] = vidxIndex;
    inFields[1]=isCpuMember;
    inFields[2] = portBitmap.ports[0];
    inFields[3] = portBitmap.ports[1];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2],inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;

     

}


/*******************************************************************************
* cpssExMxPmBrgMcMemberAdd
*
* DESCRIPTION:
*       Add new port member to the Multicast Group entry.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       vidx            - multicast group entry index.
*       portNum         - port number.
*                       
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_OUT_OF_RANGE - if vidx or portNum are larger than the allowed value.
*       GT_HW_ERROR     - on hardware error.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgMcMemberAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vidx;
    GT_U8 portNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vidx = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgMcMemberAdd(devNum, vidx, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgMcMemberDelete
*
* DESCRIPTION:
*       Delete port member from the Multicast Group entry.
*
* APPLICABLE DEVICES: ALL ExMxPm Devices.
*
* INPUTS:
*       devNum          - device number.
*       vidx            - multicast group entry index.
*       portNum         - port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_OUT_OF_RANGE - if vidx is larger than allowed.
*       GT_HW_ERROR     - on hardware error.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgMcMemberDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vidx;
    GT_U8 portNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vidx = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmBrgMcMemberDelete(devNum, vidx, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmBrgMcIpv6BytesSelectSet",
         &wrCpssExMxPmBrgMcIpv6BytesSelectSet,
         1, 7},
        {"cpssExMxPmBrgMcIpv6BytesSelectGetFirst",
         &wrCpssExMxPmBrgMcIpv6BytesSelectGetFirst,
         1, 0},
        {"cpssExMxPmBrgMcIpv6BytesSelectGetNext",
         &wrCpssExMxPmBrgMcIpv6BytesSelectGetNext,
         1, 0},
        {"cpssExMxPmBrgMcEntrySet",
         &wrCpssExMxPmBrgMcEntryWrite,
         3, 4},
        {"cpssExMxPmBrgMcEntryGetFirst",
         &wrCpssExMxPmBrgMcEntryReadFirst,
         3, 0},
        {"cpssExMxPmBrgMcEntryGetNext",
         &wrCpssExMxPmBrgMcEntryReadNext,
         3, 0},
        {"cpssExMxPmBrgMcEntry1Set",
         &wrCpssExMxPmBrgMcEntryWrite,
         3, 4},
        {"cpssExMxPmBrgMcEntry1GetFirst",
         &wrCpssExMxPmBrgMcEntryReadFirst,
         3, 0},
        {"cpssExMxPmBrgMcEntry1GetNext",
         &wrCpssExMxPmBrgMcEntryReadNext,
         3, 0},
        {"cpssExMxPmBrgMcMemberAdd",
         &wrCpssExMxPmBrgMcMemberAdd,
         3, 0},
        {"cpssExMxPmBrgMcMemberDelete",
         &wrCpssExMxPmBrgMcMemberDelete,
         3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmBrgMc
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
GT_STATUS cmdLibInitCpssExMxPmBrgMc
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

