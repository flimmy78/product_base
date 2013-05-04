/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxIp.c
*
* DESCRIPTION:
*       Wrapper functions for Ip cpss functions.
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
#include <cpss/exMx/exMxGen/ip/cpssExMxIp.h>





/***************cpssExMxIpHwLpmNode Table*****************/
/*set*/
static CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT  grangeSelectSec;

static CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC*        gnextPointerArrayPtr = NULL;
static GT_U32                                         gnumOfNextPointers = 0;
static GT_U8                                          gNodeType = 0xFF;
static GT_BOOL                                        gSetNodeNxtPtr;

/*get*/
#define NEXT_POINER_ARR_MAX_SIZE 1024

static CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC
                            gGetNextPointerArrayPtr[NEXT_POINER_ARR_MAX_SIZE];
static GT_U32               gGetNextPointersCurrIndx = 0;
static GT_U32               gGetNumOfNextPointers;

/*******************************************************************************
* ipHwLpmNodeWriteRegularNodeType
*
*
*
* Description:     Fills the CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT
*                  Representation of the lpm node range selection section
*                  weather it's the bit vector in a regular node or the range
*                  index arrays in compressed 1 and compressed 2 nodes.
*
* Fields:
* ranges1_4 - the range index array (table) of ranges 1 to 4 (for compressed 1
*             or 2)
* ranges6_9 - the range index array (table) of ranges 6 to 9 (for compressed 2)
*          1 = next : the current bit location belongs to a new address range.
* bitVectorEntry - the 11 bit vector entries for a regular node .
*
*
*******************************************************************************/
static CMD_STATUS ipHwLpmNodeWriteRegularNodeType
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;



    /* map input arguments to locals */

    gSetNodeNxtPtr = 0;/*unset flag for end set*/

    gNodeType = 0; /*CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E*/

    grangeSelectSec.regularNodeType.bitVectorEntry[0].rangeCounter =
                                                          (GT_U8)inFields[0];
    grangeSelectSec.regularNodeType.bitVectorEntry[0].bitMap =
                                                          (GT_U32)inFields[1];
    grangeSelectSec.regularNodeType.bitVectorEntry[1].rangeCounter =
                                                          (GT_U8)inFields[2];
    grangeSelectSec.regularNodeType.bitVectorEntry[1].bitMap =
                                                          (GT_U32)inFields[3];
    grangeSelectSec.regularNodeType.bitVectorEntry[2].rangeCounter =
                                                          (GT_U8)inFields[4];
    grangeSelectSec.regularNodeType.bitVectorEntry[2].bitMap =
                                                          (GT_U32)inFields[5];
    grangeSelectSec.regularNodeType.bitVectorEntry[3].rangeCounter =
                                                          (GT_U8)inFields[6];
    grangeSelectSec.regularNodeType.bitVectorEntry[3].bitMap =
                                                          (GT_U32)inFields[7];
    grangeSelectSec.regularNodeType.bitVectorEntry[4].rangeCounter =
                                                          (GT_U8)inFields[8];
    grangeSelectSec.regularNodeType.bitVectorEntry[4].bitMap =
                                                          (GT_U32)inFields[9];
    grangeSelectSec.regularNodeType.bitVectorEntry[5].rangeCounter =
                                                          (GT_U8)inFields[10];
    grangeSelectSec.regularNodeType.bitVectorEntry[5].bitMap =
                                                          (GT_U32)inFields[11];
    grangeSelectSec.regularNodeType.bitVectorEntry[6].rangeCounter =
                                                          (GT_U8)inFields[12];
    grangeSelectSec.regularNodeType.bitVectorEntry[6].bitMap =
                                                          (GT_U32)inFields[13];
    grangeSelectSec.regularNodeType.bitVectorEntry[7].rangeCounter =
                                                          (GT_U8)inFields[14];
    grangeSelectSec.regularNodeType.bitVectorEntry[7].bitMap =
                                                          (GT_U32)inFields[15];
    grangeSelectSec.regularNodeType.bitVectorEntry[8].rangeCounter =
                                                          (GT_U8)inFields[16];
    grangeSelectSec.regularNodeType.bitVectorEntry[8].bitMap =
                                                          (GT_U32)inFields[17];
    grangeSelectSec.regularNodeType.bitVectorEntry[9].rangeCounter =
                                                          (GT_U8)inFields[18];
    grangeSelectSec.regularNodeType.bitVectorEntry[9].bitMap =
                                                          (GT_U32)inFields[19];
    grangeSelectSec.regularNodeType.bitVectorEntry[10].rangeCounter =
                                                          (GT_U8)inFields[20];
    grangeSelectSec.regularNodeType.bitVectorEntry[10].bitMap =
                                                          (GT_U32)inFields[21];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* ipHwLpmNodeWriteCompressed1NodeType
*
*
*
* Description:     Fills the CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT
*                  Representation of the lpm node range selection section
*                  weather it's the bit vector in a regular node or the range
*                  index arrays in compressed 1 and compressed 2 nodes.
*
* Fields:
* ranges1_4 - the range index array (table) of ranges 1 to 4 (for compressed 1
*             or 2)
* ranges6_9 - the range index array (table) of ranges 6 to 9 (for compressed 2)
*          1 = next : the current bit location belongs to a new address range.
* bitVectorEntry - the 11 bit vector entries for a regular node .
*
*
*******************************************************************************/
static CMD_STATUS ipHwLpmNodeWriteCompressed1NodeType
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    gSetNodeNxtPtr = 0;/*unset flag for end set*/

    gNodeType = 1; /*CPSS_EXMX_IP_COMPRESSED_1_NODE_PTR_TYPE_E*/

    grangeSelectSec.compressed1NodeType.ranges1_4[0] = (GT_U8)inFields[0];
    grangeSelectSec.compressed1NodeType.ranges1_4[1] = (GT_U8)inFields[1];
    grangeSelectSec.compressed1NodeType.ranges1_4[2] = (GT_U8)inFields[2];
    grangeSelectSec.compressed1NodeType.ranges1_4[3] = (GT_U8)inFields[3];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* ipHwLpmNodeWriteCompressed2NodeType
*
*
*
* Description:     Fills the CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT
*                  Representation of the lpm node range selection section
*                  weather it's the bit vector in a regular node or the range
*                  index arrays in compressed 1 and compressed 2 nodes.
*
* Fields:
* ranges1_4 - the range index array (table) of ranges 1 to 4 (for compressed 1
*             or 2)
* ranges6_9 - the range index array (table) of ranges 6 to 9 (for compressed 2)
*          1 = next : the current bit location belongs to a new address range.
* bitVectorEntry - the 11 bit vector entries for a regular node .
*
*
*******************************************************************************/
static CMD_STATUS ipHwLpmNodeWriteCompressed2NodeType
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    gSetNodeNxtPtr = 0;/*unset flag for end set*/

    gNodeType = 2; /*CPSS_EXMX_IP_COMPRESSED_2_NODE_PTR_TYPE_E*/

    grangeSelectSec.compressed2NodeType.ranges1_4[0] = (GT_U8)inFields[0];
    grangeSelectSec.compressed2NodeType.ranges1_4[1] = (GT_U8)inFields[1];
    grangeSelectSec.compressed2NodeType.ranges1_4[2] = (GT_U8)inFields[2];
    grangeSelectSec.compressed2NodeType.ranges1_4[3] = (GT_U8)inFields[3];
    grangeSelectSec.compressed2NodeType.ranges6_9[0] = (GT_U8)inFields[4];
    grangeSelectSec.compressed2NodeType.ranges6_9[1] = (GT_U8)inFields[5];
    grangeSelectSec.compressed2NodeType.ranges6_9[2] = (GT_U8)inFields[6];
    grangeSelectSec.compressed2NodeType.ranges6_9[3] = (GT_U8)inFields[7];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxIpHwLpmNodeWriteNextPointerArraySetFirst
*
*
*
* Description:     Fills the nextPointerArrayPtr - the next pointer array.
*                  This func fills the first entry of the Array depending on
*                  pointer type.
*                  The First switch is used to call other funcs that are part of
*                  the galtis Union That are called with the same name
*                  but differ by the TAB index of the Union.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpHwLpmNodeWriteNextPointerArraySetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*The switch is used to call other funcs that are part of the galtis Union
    That are called with the same name but differ by the TAB index of the Union*/
    switch(inArgs[5])
    {
    case 0:
        return ipHwLpmNodeWriteRegularNodeType(inArgs, inFields,
                                               numFields, outArgs);
        break;

    case 1:
        return ipHwLpmNodeWriteCompressed1NodeType(inArgs, inFields,
                                                   numFields, outArgs);
        break;

    case 2:
        return ipHwLpmNodeWriteCompressed2NodeType(inArgs, inFields,
                                                   numFields, outArgs);
        break;

    case 3:
        gSetNodeNxtPtr = 1;/*set flag to note that end set should set this tbl*/
        break;

    default:
        return CMD_AGENT_ERROR;

    }

    gnumOfNextPointers = 0;/*reset on first*/

    /* map input arguments to locals */
    if(gnextPointerArrayPtr == NULL)
    {
        gnextPointerArrayPtr = (CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC));
    }
    if(gnextPointerArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gnumOfNextPointers)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gnextPointerArrayPtr[ind].pointerType =
                                (CPSS_EXMX_IP_NEXT_POINTER_TYPE_ENT)inFields[1];

    switch(gnextPointerArrayPtr[ind].pointerType)
    {
    case CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E:
    case CPSS_EXMX_IP_COMPRESSED_1_NODE_PTR_TYPE_E:
    case CPSS_EXMX_IP_COMPRESSED_2_NODE_PTR_TYPE_E:
        gnextPointerArrayPtr[ind].pointerData.nextNodePointer.nextPointer =
                                                                (GT_U32)inFields[2];
        gnextPointerArrayPtr[ind].pointerData.nextNodePointer.range5Index =
                                                                (GT_U32)inFields[3];
    break;

    case CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E:
        gnextPointerArrayPtr[ind].pointerData.routeEntryPointer.routeEntryBaseMemAddr
                                                              = (GT_U32)inFields[4];
        gnextPointerArrayPtr[ind].pointerData.routeEntryPointer.blockSize =
                                                                (GT_U32)inFields[5];
        gnextPointerArrayPtr[ind].pointerData.routeEntryPointer.routeEntryMethod =
                                   (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)inFields[6];
    break;

    default:
        return CMD_AGENT_ERROR;

    }

    gnumOfNextPointers++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxIpHwLpmNodeWriteNextPointerArraySetNext
*
*
*
* Description:     Fills the nextPointerArrayPtr - the next pointer array.
*                  This func fills the Next entries of the Array depending on
*                  pointer type.
*                  The First switch is used to call other funcs that are part of
*                  the galtis Union That are called with the same name
*                  but differ by the TAB index of the Union.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpHwLpmNodeWriteNextPointerArraySetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*if the user wrongly tries to write more than one line to LpmNodeWrite*/
    switch(inArgs[5])
    {
    case 0:
        galtisOutput(outArgs, GT_BAD_VALUE, "");
        return CMD_OK;
        break;

    case 1:
        galtisOutput(outArgs, GT_BAD_VALUE, "");
        return CMD_OK;
        break;

    case 2:
        galtisOutput(outArgs, GT_BAD_VALUE, "");
        return CMD_OK;
        break;

    case 3:
        break;

    default:
        return CMD_AGENT_ERROR;

    }

    /* map input arguments to locals */
    gnextPointerArrayPtr = (CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC*)
       cmdOsRealloc(gnextPointerArrayPtr,
       sizeof(CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC) * (gnumOfNextPointers+1));

    if(gnextPointerArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gnumOfNextPointers)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gnextPointerArrayPtr[ind].pointerType =
                                (CPSS_EXMX_IP_NEXT_POINTER_TYPE_ENT)inFields[1];

    switch(gnextPointerArrayPtr[ind].pointerType)
    {
    case CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E:
    case CPSS_EXMX_IP_COMPRESSED_1_NODE_PTR_TYPE_E:
    case CPSS_EXMX_IP_COMPRESSED_2_NODE_PTR_TYPE_E:
        gnextPointerArrayPtr[ind].pointerData.nextNodePointer.nextPointer =
                                                                (GT_U32)inFields[2];
        gnextPointerArrayPtr[ind].pointerData.nextNodePointer.range5Index =
                                                                (GT_U32)inFields[3];
    break;

    case CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E:
        gnextPointerArrayPtr[ind].pointerData.routeEntryPointer.routeEntryBaseMemAddr
                                                              = (GT_U32)inFields[4];
        gnextPointerArrayPtr[ind].pointerData.routeEntryPointer.blockSize =
                                                                (GT_U32)inFields[5];
        gnextPointerArrayPtr[ind].pointerData.routeEntryPointer.routeEntryMethod =
                                   (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)inFields[6];
    break;

    default:
        return CMD_AGENT_ERROR;

    }

    gnumOfNextPointers++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxIpHwLpmNodeWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write an LPM Node (Bucket) to the HW.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev - the device number
*       memAddr - the memory address (offset).
*       sramIndex - relevant for Ipv6 enabled device only (all other device use
*                   0). The index of the sram memory out of the possible srams
*                   in the current formation.
*       nodeType - the type of node (bucket) to write.
*       rangeSelectSecPtr - range selection section (bitmap)
*       nextPointerArrayPtr - the next pointer array.
*       numOfNextPointers - the number of next pointers in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS: If RangeSelect hasnt been initialized
*
*           cpssExMxIpHwLpmNodeNextPointersWrite
*
*           is called to set nextPointerArrayPtr without rangeSelectSecPtr.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpHwLpmNodeWriteNextPointerArrayEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U8                                          dev;
    GT_U32                                         memAddr;
    GT_U32                                         sramIndex;
    GT_U32                                         pointingToSramIndex;

    if(!gSetNodeNxtPtr)/*for ending RangeSelect tables*/
    {
        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];
    sramIndex = (GT_U32)inArgs[2];
    pointingToSramIndex = (GT_U32)inArgs[3];

    /* call cpss api function */
    if(gNodeType == 0xFF)/*RangeSelect hasnt been initialized - Dont set it*/
    {
        status =  cpssExMxIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex,
                                                  pointingToSramIndex,
                                                  gnextPointerArrayPtr,
                                                  gnumOfNextPointers);
    }
    else
    {
        status =  cpssExMxIpHwLpmNodeWrite(dev,memAddr,sramIndex,
                                           pointingToSramIndex,
                (CPSS_EXMX_IP_NEXT_POINTER_TYPE_ENT)gNodeType, &grangeSelectSec,
                                       gnextPointerArrayPtr,gnumOfNextPointers);
    }

    cmdOsFree(gnextPointerArrayPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpHwLpmNodeRead  (read rangeSelectSec only)
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Read a LPM Node (Bucket) to the HW.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev - the device number
*       memAddr - the memory address (offset).
*       sramIndex - relevant for Ipv6 enabled device only (all other device use
*                   0). The index of the sram memory out of the possible srams
*                   in the current formation.
*
* OUTPUTS:
*       nodeTypePtr - the type of node (bucket) .
*       rangeSelectSecPTr - range selection section (bitmap)
*       nextPointerArrayPtr - the next pointer array.
*       numOfNextPointersPtr - the number of next pointers in the array
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:  Here used only to read the rangeSelectSec
*
*
*******************************************************************************/
static CMD_STATUS ipHwLpmNodeReadRangeSelectGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                          dev;
    GT_U32                                         memAddr;
    GT_U32                                         sramIndex;
    CPSS_EXMX_IP_NEXT_POINTER_TYPE_ENT             nodeType;
    CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT  rangeSelectSec;
    CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC         *nextPointerArray;
    GT_U32                                         numOfNextPointers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];
    sramIndex = (GT_U32)inArgs[2];
    nodeType = (CPSS_EXMX_IP_NEXT_POINTER_TYPE_ENT)inArgs[4];

    nextPointerArray = (CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC*)
            cmdOsMalloc(sizeof(CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC)*NEXT_POINER_ARR_MAX_SIZE);
    if (nextPointerArray == NULL)
    {
        galtisOutput(outArgs, GT_OUT_OF_CPU_MEM, "");
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxIpHwLpmNodeRead(dev,memAddr,sramIndex, nodeType,
                                     &rangeSelectSec, nextPointerArray,
                                     &numOfNextPointers);

    if (result != GT_OK)
    {
        cmdOsFree(nextPointerArray);
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    switch(nodeType)
    {
    case CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E:

        inFields[0] =
            rangeSelectSec.regularNodeType.bitVectorEntry[0].rangeCounter;
        inFields[1] =
            rangeSelectSec.regularNodeType.bitVectorEntry[0].bitMap;
        inFields[2] =
            rangeSelectSec.regularNodeType.bitVectorEntry[1].rangeCounter;
        inFields[3] =
            rangeSelectSec.regularNodeType.bitVectorEntry[1].bitMap;
        inFields[4] =
            rangeSelectSec.regularNodeType.bitVectorEntry[2].rangeCounter;
        inFields[5] =
            rangeSelectSec.regularNodeType.bitVectorEntry[2].bitMap;
        inFields[6] =
            rangeSelectSec.regularNodeType.bitVectorEntry[3].rangeCounter;
        inFields[7] =
            rangeSelectSec.regularNodeType.bitVectorEntry[3].bitMap;
        inFields[8] =
            rangeSelectSec.regularNodeType.bitVectorEntry[4].rangeCounter;
        inFields[9] =
            rangeSelectSec.regularNodeType.bitVectorEntry[4].bitMap;
        inFields[10] =
            rangeSelectSec.regularNodeType.bitVectorEntry[5].rangeCounter;
        inFields[11] =
            rangeSelectSec.regularNodeType.bitVectorEntry[5].bitMap;
        inFields[12] =
            rangeSelectSec.regularNodeType.bitVectorEntry[6].rangeCounter;
        inFields[13] =
            rangeSelectSec.regularNodeType.bitVectorEntry[6].bitMap;
        inFields[14] =
            rangeSelectSec.regularNodeType.bitVectorEntry[7].rangeCounter;
        inFields[15] =
            rangeSelectSec.regularNodeType.bitVectorEntry[7].bitMap;
        inFields[16] =
            rangeSelectSec.regularNodeType.bitVectorEntry[8].rangeCounter;
        inFields[17] =
            rangeSelectSec.regularNodeType.bitVectorEntry[8].bitMap;
        inFields[18] =
            rangeSelectSec.regularNodeType.bitVectorEntry[9].rangeCounter;
        inFields[19] =
            rangeSelectSec.regularNodeType.bitVectorEntry[9].bitMap;
        inFields[20] =
            rangeSelectSec.regularNodeType.bitVectorEntry[10].rangeCounter;
        inFields[21] =
            rangeSelectSec.regularNodeType.bitVectorEntry[10].bitMap;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],
                    inFields[1], inFields[2] , inFields[3], inFields[4],
                    inFields[5], inFields[6] , inFields[7], inFields[8],
                    inFields[9] , inFields[10], inFields[11], inFields[12],
                    inFields[13] , inFields[14], inFields[15], inFields[16],
                    inFields[17] , inFields[18], inFields[19], inFields[20],
                    inFields[21]);

        break;

    case CPSS_EXMX_IP_COMPRESSED_1_NODE_PTR_TYPE_E:

        inFields[0] = rangeSelectSec.compressed1NodeType.ranges1_4[0];
        inFields[1] = rangeSelectSec.compressed1NodeType.ranges1_4[1];
        inFields[2] = rangeSelectSec.compressed1NodeType.ranges1_4[2];
        inFields[3] = rangeSelectSec.compressed1NodeType.ranges1_4[3];

        /* pack and output table fields */
        fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2] ,
                    inFields[3]);
        break;

    case CPSS_EXMX_IP_COMPRESSED_2_NODE_PTR_TYPE_E:

        inFields[0] = rangeSelectSec.compressed2NodeType.ranges1_4[0];
        inFields[1] = rangeSelectSec.compressed2NodeType.ranges1_4[1];
        inFields[2] = rangeSelectSec.compressed2NodeType.ranges1_4[2];
        inFields[3] = rangeSelectSec.compressed2NodeType.ranges1_4[3];
        inFields[4] = rangeSelectSec.compressed2NodeType.ranges6_9[0];
        inFields[5] = rangeSelectSec.compressed2NodeType.ranges6_9[1];
        inFields[6] = rangeSelectSec.compressed2NodeType.ranges6_9[2];
        inFields[7] = rangeSelectSec.compressed2NodeType.ranges6_9[3];

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2] ,
                    inFields[3], inFields[4], inFields[5], inFields[6],
                    inFields[7]);
        break;

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        break;
    }

    galtisOutput(outArgs, result, "%f");

    cmdOsFree(nextPointerArray);

    return CMD_OK;
}


/*******************************************************************************
* ipHwLpmNodeReadRangeSelectGetNext
*
* DESCRIPTION:
*    Used as completion to ipHwLpmNodeReadRangeSelectGetFirst in order to
*    Read a LPM Node and show it one line and stop after first and only one.
*
*
*
*******************************************************************************/
static CMD_STATUS ipHwLpmNodeReadRangeSelectGetNext
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
* cpssExMxIpHwLpmNodeRead  (read gGetNextPointerArrayPtr)
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Read a LPM Node (Bucket) to the HW.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev - the device number
*       memAddr - the memory address (offset).
*       sramIndex - relevant for Ipv6 enabled device only (all other device use
*                   0). The index of the sram memory out of the possible srams
*                   in the current formation.
*
* OUTPUTS:
*       nodeTypePtr - the type of node (bucket) .
*       rangeSelectSecPTr - range selection section (bitmap)
*       nextPointerArrayPtr - the next pointer array.
*       numOfNextPointersPtr - the number of next pointers in the array
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:  Here used to read the gGetNextPointerArrayPtr
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpHwLpmNodeReadNextPointerGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                          dev;
    GT_U32                                         memAddr;
    GT_U32                                         sramIndex;
    CPSS_EXMX_IP_NEXT_POINTER_TYPE_ENT             nodeType;
    CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT  rangeSelectSec;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];
    sramIndex = (GT_U32)inArgs[2];
    nodeType = (CPSS_EXMX_IP_NEXT_POINTER_TYPE_ENT)inArgs[4];

    /*if the galtis tab refreshing is rangeSelect and not NextPointerArray*/
        if(nodeType < 3)
        {
                return ipHwLpmNodeReadRangeSelectGetFirst(inArgs, inFields,
                                                  numFields, outArgs);
        }


    gGetNextPointersCurrIndx = 0;/*reset on first*/

    /* call cpss api function */
    result = cpssExMxIpHwLpmNodeRead(dev,memAddr,sramIndex, nodeType,
                                     &rangeSelectSec, gGetNextPointerArrayPtr,
                                     &gGetNumOfNextPointers);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gGetNextPointersCurrIndx;

    inFields[1] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerType;
    inFields[2] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerData.nextNodePointer.nextPointer;
    inFields[3] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerData.nextNodePointer.range5Index;
    inFields[4] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerData.routeEntryPointer.routeEntryBaseMemAddr;
    inFields[5] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerData.routeEntryPointer.blockSize;
    inFields[6] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerData.routeEntryPointer.routeEntryMethod;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2] ,
                inFields[3], inFields[4], inFields[5], inFields[6]);

    galtisOutput(outArgs, result, "%f");

    gGetNextPointersCurrIndx++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxIpHwLpmNodeReadNextPointerGetNext
*
* DESCRIPTION:
*    Used as completion to wrCpssExMxIpHwLpmNodeReadNextPointerGetFirst
*    in order to go on refreshing to galtis the whole gGetNextPointerArrayPtr
*    or if the tab refreshing in galtis union is RangeSelect call
*    ipHwLpmNodeReadRangeSelectGetNext
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpHwLpmNodeReadNextPointerGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    if(inArgs[4] < 3)
    {
        return ipHwLpmNodeReadRangeSelectGetNext(inArgs, inFields,
                                                        numFields, outArgs);
    }

    if(gGetNextPointersCurrIndx >= gGetNumOfNextPointers)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = gGetNextPointersCurrIndx;

    inFields[1] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerType;
    inFields[2] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerData.nextNodePointer.nextPointer;
    inFields[3] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerData.nextNodePointer.range5Index;
    inFields[4] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerData.routeEntryPointer.routeEntryBaseMemAddr;
    inFields[5] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerData.routeEntryPointer.blockSize;
    inFields[6] = gGetNextPointerArrayPtr[gGetNextPointersCurrIndx].
        pointerData.routeEntryPointer.routeEntryMethod;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2] ,
                inFields[3], inFields[4], inFields[5], inFields[6]);

    galtisOutput(outArgs, GT_OK, "%f");

    gGetNextPointersCurrIndx++;

    return CMD_OK;
}

/****************cpssExMxIpMcTreeNode Table************/
/*set*/
static CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC*        gMTNnextPointerArrayPtr = NULL;
static GT_U32                                        gMTNnumOfNextPointers = 0;

/*get*/
#define MTN_NEXT_POINER_ARR_MAX_SIZE 1024

static CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC
                       gMTNGetNextPointerArrayPtr[MTN_NEXT_POINER_ARR_MAX_SIZE];
static GT_U32          gMTNGetNextPointersCurrIndx = 0;
static GT_U32          gMTNGetNumOfNextPointers;
/*******************************************************************************
* cpssExMxIpMcTreeNodeWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write an MC Tree Node (Bucket) to the HW - for MC Group search struct.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA
*
* INPUTS:
*       dev - the device number
*       memAddr - the memory address (offset).
*       nextPointerArrayPtr - the next pointer array.
*       numOfNextPointers - the number of next pointers in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcTreeNodeWriteSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    gMTNnumOfNextPointers = 0;/*reset on first*/

    /* map input arguments to locals */
    if(gMTNnextPointerArrayPtr == NULL)
    {
        gMTNnextPointerArrayPtr = (CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC));
    }
    if(gMTNnextPointerArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gMTNnumOfNextPointers)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gMTNnextPointerArrayPtr[ind].nextPointer = (GT_U32)inFields[1];
    gMTNnextPointerArrayPtr[ind].nextMcNodeMin = (GT_U8)inFields[2];
    gMTNnextPointerArrayPtr[ind].nextMcNodeMax = (GT_U8)inFields[3];

    gMTNnumOfNextPointers++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpMcTreeNodeWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write an MC Tree Node (Bucket) to the HW - for MC Group search struct.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA
*
* INPUTS:
*       dev - the device number
*       memAddr - the memory address (offset).
*       nextPointerArrayPtr - the next pointer array.
*       numOfNextPointers - the number of next pointers in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcTreeNodeWriteSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    gMTNnextPointerArrayPtr = (CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC*)
       cmdOsRealloc(gMTNnextPointerArrayPtr,
       sizeof(CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC) * (gMTNnumOfNextPointers+1));

    if(gMTNnextPointerArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gMTNnumOfNextPointers)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gMTNnextPointerArrayPtr[ind].nextPointer = (GT_U32)inFields[1];
    gMTNnextPointerArrayPtr[ind].nextMcNodeMin = (GT_U8)inFields[2];
    gMTNnextPointerArrayPtr[ind].nextMcNodeMax = (GT_U8)inFields[3];

    gMTNnumOfNextPointers++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxIpMcTreeNodeWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write an MC Tree Node (Bucket) to the HW - for MC Group search struct.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA
*
* INPUTS:
*       dev - the device number
*       memAddr - the memory address (offset).
*       nextPointerArrayPtr - the next pointer array.
*       numOfNextPointers - the number of next pointers in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcTreeNodeWriteEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U8                                          dev;
    GT_U32                                         memAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];

    /* call cpss api function */
    status =  cpssExMxIpMcTreeNodeWrite(dev,memAddr, gMTNnextPointerArrayPtr,
                                       gMTNnumOfNextPointers);

    cmdOsFree(gMTNnextPointerArrayPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpMcTreeNodeRead
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       read a MC Tree Node (Bucket) from the HW .
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA
*
* INPUTS:
*       dev - the device number
*       memAddr - the memory address (offset).
*       numOfNextPointers - the number of next pointers in the array
*
* OUTPUTS:
*       nextPointerArrayPtr - the next pointer array.
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcTreeNodeReadGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                          dev;
    GT_U32                                         memAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];


    gMTNGetNextPointersCurrIndx = 0;/*reset on first*/

    /* call cpss api function */
    result = cpssExMxIpMcTreeNodeRead(dev,memAddr, gMTNGetNextPointerArrayPtr,
                                     gMTNGetNumOfNextPointers);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gMTNGetNextPointersCurrIndx;

    inFields[1] = gMTNnextPointerArrayPtr[gMTNGetNextPointersCurrIndx].nextPointer;
    inFields[2] = gMTNnextPointerArrayPtr[gMTNGetNextPointersCurrIndx].nextMcNodeMin;
    inFields[3] = gMTNnextPointerArrayPtr[gMTNGetNextPointersCurrIndx].nextMcNodeMax;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2] , inFields[3]);

    galtisOutput(outArgs, result, "%f");

    gMTNGetNextPointersCurrIndx++;

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxIpMcTreeNodeRead
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       read a MC Tree Node (Bucket) from the HW .
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA
*
* INPUTS:
*       dev - the device number
*       memAddr - the memory address (offset).
*       numOfNextPointers - the number of next pointers in the array
*
* OUTPUTS:
*       nextPointerArrayPtr - the next pointer array.
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcTreeNodeReadGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(gMTNGetNextPointersCurrIndx >= gMTNGetNumOfNextPointers)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = gMTNGetNextPointersCurrIndx;

    inFields[1] = gMTNnextPointerArrayPtr[gMTNGetNextPointersCurrIndx].nextPointer;
    inFields[2] = gMTNnextPointerArrayPtr[gMTNGetNextPointersCurrIndx].nextMcNodeMin;
    inFields[3] = gMTNnextPointerArrayPtr[gMTNGetNextPointersCurrIndx].nextMcNodeMax;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2] , inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");

    gMTNGetNextPointersCurrIndx++;

    return CMD_OK;
}



/****************cpssExMxIpUcRouteEntries Table************/
/*set*/
static CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC*        gIPUCrouteEntryArrayPtr = NULL;
static GT_U32                                  gIPUCnumOfEntries = 0;

/*get*/
#define IPUC_ROUTE_ARR_MAX_SIZE 1024

static CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC
                       gIPUCgetRouteEntryArrayPtr[IPUC_ROUTE_ARR_MAX_SIZE];
static GT_U32          gIPUCgetRouteEntryCurrIndx = 0;
static GT_U32          gIPUCgetNumOfEntries;
/*******************************************************************************
* cpssExMxIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       baseMemAddr          - the base memory address (offset). The
*                              entries will be written from this base on.
*       routeEntriesArrayPtr - the uc route entries array
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpUcRouteEntriesWriteSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gIPUCnumOfEntries = 0;/*reset on first*/

    /* map input arguments to locals */
    if(gIPUCrouteEntryArrayPtr == NULL)
    {
        gIPUCrouteEntryArrayPtr = (CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC));
    }
    if(gIPUCrouteEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIPUCnumOfEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gIPUCrouteEntryArrayPtr[ind].cmd =
                                 (CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ENT)inFields[1];
    gIPUCrouteEntryArrayPtr[ind].mirrorToAnalyzer = (GT_BOOL)inFields[2];
    gIPUCrouteEntryArrayPtr[ind].forceCos = (GT_BOOL)inFields[3];
    gIPUCrouteEntryArrayPtr[ind].trafficClass = (GT_U8)inFields[4];
    gIPUCrouteEntryArrayPtr[ind].dropPrecedence =
                                                (CPSS_DP_LEVEL_ENT)inFields[5];
    gIPUCrouteEntryArrayPtr[ind].userPriority = (GT_U8)inFields[6];
    gIPUCrouteEntryArrayPtr[ind].ipCounterSet =
                                         (CPSS_EXMX_IP_CNT_SET_ENT)inFields[7];
    gIPUCrouteEntryArrayPtr[ind].enableDecTtlHopLimit = (GT_BOOL)inFields[8];
    gIPUCrouteEntryArrayPtr[ind].age = (GT_BOOL)inFields[9];
    gIPUCrouteEntryArrayPtr[ind].forceUserPriority = (GT_BOOL)inFields[10];
    gIPUCrouteEntryArrayPtr[ind].enableScopeCheck = (GT_BOOL)inFields[11];
    gIPUCrouteEntryArrayPtr[ind].destinationSiteId =
                                        (CPSS_EXMX_IP_SITE_ID_ENT)inFields[12];
    gIPUCrouteEntryArrayPtr[ind].enableICMPRedirectHook = (GT_BOOL)inFields[13];
    gIPUCrouteEntryArrayPtr[ind].mtu = (GT_U16)inFields[14];
    gIPUCrouteEntryArrayPtr[ind].greIPv4TunnelTerminate = (GT_BOOL)inFields[15];
    gIPUCrouteEntryArrayPtr[ind].ipIPv4TunnelTerminate = (GT_BOOL)inFields[16];
    gIPUCrouteEntryArrayPtr[ind].tunnelCopyDscp = (GT_BOOL)inFields[17];
    gIPUCrouteEntryArrayPtr[ind].tunnelCopyTtl = (GT_BOOL)inFields[18];
    gIPUCrouteEntryArrayPtr[ind].pushMplsLabel = (GT_BOOL)inFields[19];
    gIPUCrouteEntryArrayPtr[ind].setExp = (GT_BOOL)inFields[20];
    gIPUCrouteEntryArrayPtr[ind].label = (GT_U32)inFields[21];
    gIPUCrouteEntryArrayPtr[ind].exp = (GT_U8)inFields[22];
    gIPUCrouteEntryArrayPtr[ind].ttlHopLimit = (GT_U8)inFields[23];
    gIPUCrouteEntryArrayPtr[ind].mirrorToCpu = (GT_BOOL)inFields[24];
    gIPUCrouteEntryArrayPtr[ind].nextHopInformation.arpPointer =
                                                          (GT_U32)inFields[25];

    gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifType =
                                    (CPSS_EXMX_IP_OUTLIF_TYPE_ENT)inFields[26];

    if(gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifType ==
                                                      CPSS_EXMX_IP_OUTLIF_LL_E)
    {
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                        linkLayerOutLif.vid = (GT_U16)inFields[27];
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                   linkLayerOutLif.useVidx = (GT_BOOL)inFields[28];
        if(!gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                        linkLayerOutLif.useVidx)
        {
            gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.linkLayerOutLif.
                            vidxOrTrunkPort.tarInfo.tarIsTrunk = (GT_BOOL)inFields[29];
            if(!gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.linkLayerOutLif.
                                            vidxOrTrunkPort.tarInfo.tarIsTrunk)
            {
                gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                    linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort =
                                                                       (GT_U8)inFields[30];
                gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                    linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev =
                                                                       (GT_U8)inFields[31];
            }
            else
                gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk =
                                                                   (GT_U8)inFields[32];
        }
        else
            gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                       linkLayerOutLif.vidxOrTrunkPort.vidx = (GT_U16)inFields[33];
    }
    else
    {
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                     tunnelOutLif.tunnelType = (GT_U8)inFields[34];
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                           tunnelOutLif.vid = (GT_U16)inFields[35];
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                 tunnelOutLif.tunnelPointer = (GT_U16)inFields[36];
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                   tunnelOutLif.tarIsTrunk = (GT_BOOL)inFields[37];
        if(!gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                        tunnelOutLif.tarIsTrunk)
        {
            gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                       tunnelOutLif.trunkOrPort.portInfo.tarPort = (GT_U8)inFields[38];
            gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                        tunnelOutLif.trunkOrPort.portInfo.tarDev = (GT_U8)inFields[39];
        }
        else
            gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                 tunnelOutLif.trunkOrPort.trunkInfo.tarTrunk = (GT_U8)inFields[40];
    }

    gIPUCnumOfEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       baseMemAddr          - the base memory address (offset). The
*                              entries will be written from this base on.
*       routeEntriesArrayPtr - the uc route entries array
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpUcRouteEntriesWriteSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    gIPUCrouteEntryArrayPtr = (CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC*)
       cmdOsRealloc(gIPUCrouteEntryArrayPtr,
       sizeof(CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC) * (gIPUCnumOfEntries+1));

    if(gIPUCrouteEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIPUCnumOfEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gIPUCrouteEntryArrayPtr[ind].cmd =
                                 (CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ENT)inFields[1];
    gIPUCrouteEntryArrayPtr[ind].mirrorToAnalyzer = (GT_BOOL)inFields[2];
    gIPUCrouteEntryArrayPtr[ind].forceCos = (GT_BOOL)inFields[3];
    gIPUCrouteEntryArrayPtr[ind].trafficClass = (GT_U8)inFields[4];
    gIPUCrouteEntryArrayPtr[ind].dropPrecedence =
                                                (CPSS_DP_LEVEL_ENT)inFields[5];
    gIPUCrouteEntryArrayPtr[ind].userPriority = (GT_U8)inFields[6];
    gIPUCrouteEntryArrayPtr[ind].ipCounterSet =
                                         (CPSS_EXMX_IP_CNT_SET_ENT)inFields[7];
    gIPUCrouteEntryArrayPtr[ind].enableDecTtlHopLimit = (GT_BOOL)inFields[8];
    gIPUCrouteEntryArrayPtr[ind].age = (GT_BOOL)inFields[9];
    gIPUCrouteEntryArrayPtr[ind].forceUserPriority = (GT_BOOL)inFields[10];
    gIPUCrouteEntryArrayPtr[ind].enableScopeCheck = (GT_BOOL)inFields[11];
    gIPUCrouteEntryArrayPtr[ind].destinationSiteId =
                                        (CPSS_EXMX_IP_SITE_ID_ENT)inFields[12];
    gIPUCrouteEntryArrayPtr[ind].enableICMPRedirectHook = (GT_BOOL)inFields[13];
    gIPUCrouteEntryArrayPtr[ind].mtu = (GT_U16)inFields[14];
    gIPUCrouteEntryArrayPtr[ind].greIPv4TunnelTerminate = (GT_BOOL)inFields[15];
    gIPUCrouteEntryArrayPtr[ind].ipIPv4TunnelTerminate = (GT_BOOL)inFields[16];
    gIPUCrouteEntryArrayPtr[ind].tunnelCopyDscp = (GT_BOOL)inFields[17];
    gIPUCrouteEntryArrayPtr[ind].tunnelCopyTtl = (GT_BOOL)inFields[18];
    gIPUCrouteEntryArrayPtr[ind].pushMplsLabel = (GT_BOOL)inFields[19];
    gIPUCrouteEntryArrayPtr[ind].setExp = (GT_BOOL)inFields[20];
    gIPUCrouteEntryArrayPtr[ind].label = (GT_U32)inFields[21];
    gIPUCrouteEntryArrayPtr[ind].exp = (GT_U8)inFields[22];
    gIPUCrouteEntryArrayPtr[ind].ttlHopLimit = (GT_U8)inFields[23];
    gIPUCrouteEntryArrayPtr[ind].mirrorToCpu = (GT_BOOL)inFields[24];
    gIPUCrouteEntryArrayPtr[ind].nextHopInformation.arpPointer =
                                                          (GT_U32)inFields[25];

    gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifType =
                                    (CPSS_EXMX_IP_OUTLIF_TYPE_ENT)inFields[26];

    if(gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifType ==
                                                      CPSS_EXMX_IP_OUTLIF_LL_E)
    {
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                        linkLayerOutLif.vid = (GT_U16)inFields[27];
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                   linkLayerOutLif.useVidx = (GT_BOOL)inFields[28];
        if(!gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                        linkLayerOutLif.useVidx)
        {
            gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.linkLayerOutLif.
                            vidxOrTrunkPort.tarInfo.tarIsTrunk = (GT_BOOL)inFields[29];
            if(!gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.linkLayerOutLif.
                                            vidxOrTrunkPort.tarInfo.tarIsTrunk)
            {
                gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                    linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort =
                                                                       (GT_U8)inFields[30];
                gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                    linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev =
                                                                       (GT_U8)inFields[31];
            }
            else
                gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk =
                                                                   (GT_U8)inFields[32];
        }
        else
            gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                       linkLayerOutLif.vidxOrTrunkPort.vidx = (GT_U16)inFields[33];
    }
    else
    {
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                     tunnelOutLif.tunnelType = (GT_U8)inFields[34];
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                           tunnelOutLif.vid = (GT_U16)inFields[35];
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                 tunnelOutLif.tunnelPointer = (GT_U16)inFields[36];
        gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                   tunnelOutLif.tarIsTrunk = (GT_BOOL)inFields[37];
        if(!gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                        tunnelOutLif.tarIsTrunk)
        {
            gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                       tunnelOutLif.trunkOrPort.portInfo.tarPort = (GT_U8)inFields[38];
            gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                        tunnelOutLif.trunkOrPort.portInfo.tarDev = (GT_U8)inFields[39];
        }
        else
            gIPUCrouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                 tunnelOutLif.trunkOrPort.trunkInfo.tarTrunk = (GT_U8)inFields[40];
    }
    gIPUCnumOfEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       baseMemAddr          - the base memory address (offset). The
*                              entries will be written from this base on.
*       routeEntriesArrayPtr - the uc route entries array
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpUcRouteEntriesWriteEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U8                                          dev;
    GT_U32                                         memAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];

    /* call cpss api function */
   status =  cpssExMxIpUcRouteEntriesWrite(dev,memAddr, gIPUCrouteEntryArrayPtr,
                                       gIPUCnumOfEntries);

   cmdOsFree(gIPUCrouteEntryArrayPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpUcRouteEntriesRead
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       read an array of uc route entries from the hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       baseMemAddr          - the base memory address (offset). The
*                              entries will be read from this base on.
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       routeEntriesArrayPtr - the uc route entries array
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpUcRouteEntriesReadGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U32  memAddr;
    GT_U32  ind;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gIPUCgetRouteEntryCurrIndx = 0;/*reset on first*/
    ind = gIPUCgetRouteEntryCurrIndx;
    gIPUCgetNumOfEntries = IPUC_ROUTE_ARR_MAX_SIZE;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxIpUcRouteEntriesRead(dev,memAddr, gIPUCgetRouteEntryArrayPtr,
                                     gIPUCgetNumOfEntries);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gIPUCgetRouteEntryCurrIndx;

    inFields[1] = gIPUCgetRouteEntryArrayPtr[ind].cmd;
    inFields[2] = gIPUCgetRouteEntryArrayPtr[ind].mirrorToAnalyzer;
    inFields[3] = gIPUCgetRouteEntryArrayPtr[ind].forceCos;
    inFields[4] = gIPUCgetRouteEntryArrayPtr[ind].trafficClass;
    inFields[5] = gIPUCgetRouteEntryArrayPtr[ind].dropPrecedence;
    inFields[6] = gIPUCgetRouteEntryArrayPtr[ind].userPriority;
    inFields[7] = gIPUCgetRouteEntryArrayPtr[ind].ipCounterSet;
    inFields[8] = gIPUCgetRouteEntryArrayPtr[ind].enableDecTtlHopLimit;
    inFields[9] = gIPUCgetRouteEntryArrayPtr[ind].age;
    inFields[10] = gIPUCgetRouteEntryArrayPtr[ind].forceUserPriority;
    inFields[11] = gIPUCgetRouteEntryArrayPtr[ind].enableScopeCheck;
    inFields[12] = gIPUCgetRouteEntryArrayPtr[ind].destinationSiteId;
    inFields[13] = gIPUCgetRouteEntryArrayPtr[ind].enableICMPRedirectHook;
    inFields[14] = gIPUCgetRouteEntryArrayPtr[ind].mtu;
    inFields[15] = gIPUCgetRouteEntryArrayPtr[ind].greIPv4TunnelTerminate;
    inFields[16] = gIPUCgetRouteEntryArrayPtr[ind].ipIPv4TunnelTerminate;
    inFields[17] = gIPUCgetRouteEntryArrayPtr[ind].tunnelCopyDscp;
    inFields[18] = gIPUCgetRouteEntryArrayPtr[ind].tunnelCopyTtl;
    inFields[19] = gIPUCgetRouteEntryArrayPtr[ind].pushMplsLabel;
    inFields[20] = gIPUCgetRouteEntryArrayPtr[ind].setExp;
    inFields[21] = gIPUCgetRouteEntryArrayPtr[ind].label;
    inFields[22] = gIPUCgetRouteEntryArrayPtr[ind].exp;
    inFields[23] = gIPUCgetRouteEntryArrayPtr[ind].ttlHopLimit;
    inFields[24] = gIPUCgetRouteEntryArrayPtr[ind].mirrorToCpu;
    inFields[25] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.arpPointer;
    inFields[26] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifType;
    inFields[27] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                            linkLayerOutLif.vid;
    inFields[28] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                        linkLayerOutLif.useVidx;
    inFields[29] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                             linkLayerOutLif.vidxOrTrunkPort.tarInfo.tarIsTrunk;
    inFields[30] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
           linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort;
    inFields[31] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
            linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev;
    inFields[32] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
         linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk;
    inFields[33] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                           linkLayerOutLif.vidxOrTrunkPort.vidx;
    inFields[34] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                        tunnelOutLif.tunnelType;
    inFields[35] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                               tunnelOutLif.vid;
    inFields[36] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                     tunnelOutLif.tunnelPointer;
    inFields[37] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                        tunnelOutLif.tarIsTrunk;
    inFields[38] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                      tunnelOutLif.trunkOrPort.portInfo.tarPort;
    inFields[39] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                       tunnelOutLif.trunkOrPort.portInfo.tarDev;
    inFields[40] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                    tunnelOutLif.trunkOrPort.trunkInfo.tarTrunk;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32], inFields[33], inFields[34], inFields[35],
                inFields[36], inFields[37], inFields[38], inFields[39],
                inFields[40]);

    galtisOutput(outArgs, result, "%f");

    gIPUCgetRouteEntryCurrIndx++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpUcRouteEntriesRead
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       read an array of uc route entries from the hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       baseMemAddr          - the base memory address (offset). The
*                              entries will be read from this base on.
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       routeEntriesArrayPtr - the uc route entries array
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpUcRouteEntriesReadGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32    ind = gIPUCgetRouteEntryCurrIndx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(gIPUCgetRouteEntryCurrIndx >= gIPUCgetNumOfEntries)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = gIPUCgetRouteEntryCurrIndx;
    inFields[1] = gIPUCgetRouteEntryArrayPtr[ind].cmd;
    inFields[2] = gIPUCgetRouteEntryArrayPtr[ind].mirrorToAnalyzer;
    inFields[3] = gIPUCgetRouteEntryArrayPtr[ind].forceCos;
    inFields[4] = gIPUCgetRouteEntryArrayPtr[ind].trafficClass;
    inFields[5] = gIPUCgetRouteEntryArrayPtr[ind].dropPrecedence;
    inFields[6] = gIPUCgetRouteEntryArrayPtr[ind].userPriority;
    inFields[7] = gIPUCgetRouteEntryArrayPtr[ind].ipCounterSet;
    inFields[8] = gIPUCgetRouteEntryArrayPtr[ind].enableDecTtlHopLimit;
    inFields[9] = gIPUCgetRouteEntryArrayPtr[ind].age;
    inFields[10] = gIPUCgetRouteEntryArrayPtr[ind].forceUserPriority;
    inFields[11] = gIPUCgetRouteEntryArrayPtr[ind].enableScopeCheck;
    inFields[12] = gIPUCgetRouteEntryArrayPtr[ind].destinationSiteId;
    inFields[13] = gIPUCgetRouteEntryArrayPtr[ind].enableICMPRedirectHook;
    inFields[14] = gIPUCgetRouteEntryArrayPtr[ind].mtu;
    inFields[15] = gIPUCgetRouteEntryArrayPtr[ind].greIPv4TunnelTerminate;
    inFields[16] = gIPUCgetRouteEntryArrayPtr[ind].ipIPv4TunnelTerminate;
    inFields[17] = gIPUCgetRouteEntryArrayPtr[ind].tunnelCopyDscp;
    inFields[18] = gIPUCgetRouteEntryArrayPtr[ind].tunnelCopyTtl;
    inFields[19] = gIPUCgetRouteEntryArrayPtr[ind].pushMplsLabel;
    inFields[20] = gIPUCgetRouteEntryArrayPtr[ind].setExp;
    inFields[21] = gIPUCgetRouteEntryArrayPtr[ind].label;
    inFields[22] = gIPUCgetRouteEntryArrayPtr[ind].exp;
    inFields[23] = gIPUCgetRouteEntryArrayPtr[ind].ttlHopLimit;
    inFields[24] = gIPUCgetRouteEntryArrayPtr[ind].mirrorToCpu;
    inFields[25] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.arpPointer;
    inFields[26] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifType;
    inFields[27] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                            linkLayerOutLif.vid;
    inFields[28] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                        linkLayerOutLif.useVidx;
    inFields[29] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                             linkLayerOutLif.vidxOrTrunkPort.tarInfo.tarIsTrunk;
    inFields[30] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
           linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort;
    inFields[31] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
            linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev;
    inFields[32] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
         linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk;
    inFields[33] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                           linkLayerOutLif.vidxOrTrunkPort.vidx;
    inFields[34] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                        tunnelOutLif.tunnelType;
    inFields[35] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                               tunnelOutLif.vid;
    inFields[36] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                     tunnelOutLif.tunnelPointer;
    inFields[37] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                                        tunnelOutLif.tarIsTrunk;
    inFields[38] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                      tunnelOutLif.trunkOrPort.portInfo.tarPort;
    inFields[39] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                       tunnelOutLif.trunkOrPort.portInfo.tarDev;
    inFields[40] = gIPUCgetRouteEntryArrayPtr[ind].nextHopInformation.outlif.outlifInfo.
                                    tunnelOutLif.trunkOrPort.trunkInfo.tarTrunk;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32], inFields[33], inFields[34], inFields[35],
                inFields[36], inFields[37], inFields[38], inFields[39],
                inFields[40]);

    galtisOutput(outArgs, GT_OK, "%f");

    gIPUCgetRouteEntryCurrIndx++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpRouteEntryActiveBitReadAndReset
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       read a reset route entry active bit in the hw.
*       (this API is for both mc & uc route entries)
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       memAddr              - the memory address (offset).
*       doReset              - weather to preform a reset after the read.
*
* OUTPUTS:
*       activeBitPtr    - the active bit.
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpRouteEntryActiveBitReadAndReset
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U32  cmemAddr;
    GT_BOOL activeBit;
    GT_BOOL doReset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    cmemAddr = (GT_U32)inArgs[1];
    doReset = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxIpRouteEntryActiveBitReadAndReset(dev, cmemAddr, doReset,
                                                       &activeBit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", activeBit);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpRouteEntryFieldWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write a route entry field to hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       memAddr              - the memory address (offset) of the route entry.
*       routeEntriesArrayPtr - the mc route entries array
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpRouteEntryFieldWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                   dev;
    GT_U32                                  memAddr;
    CPSS_EXMX_IP_ROUTE_ENTRY_FIELD_TYPE_ENT fieldType;
    CPSS_EXMX_IP_ROUTE_ENTRY_FIELD_UNT      fieldValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];
    fieldType = (CPSS_EXMX_IP_ROUTE_ENTRY_FIELD_TYPE_ENT)inArgs[2];
    switch(fieldType)
    {
    case CPSS_EXMX_IP_ROUTE_ENTRY_CMD_FIELD_E:
        fieldValue.cmd = (CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ENT)inArgs[3];
        break;
    case CPSS_EXMX_IP_ROUTE_ENTRY_CNT_SET_FIELD_E:
        fieldValue.cntSet = (CPSS_EXMX_IP_CNT_SET_ENT)inArgs[4];
        break;
    case CPSS_EXMX_IP_ROUTE_ENTRY_MTU_FIELD_E:
        fieldValue.mtu = (GT_U16)inArgs[5];
        break;
    case CPSS_EXMX_IP_ROUTE_ENTRY_ANALYZER_MIRROR_FIELD_E:
        fieldValue.mirrorToAnalyzer = (GT_BOOL)inArgs[6];
        break;
    default:
        return CMD_AGENT_ERROR;
    }

    /* call cpss api function */
    result = cpssExMxIpRouteEntryFieldWrite(dev, memAddr,
                                            fieldType, fieldValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpRouteEntryMplsPushParamsWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write a route entry MPLS push parameter to the hw
*
* APPLICABLE DEVICES:
*        SAMBA/RUMBA
*
* INPUTS:
*       dev                  - the device number
*       memAddr              - the memory address (offset) of the route entry.
*       mplsParamsPtr        - the mpls push params.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpRouteEntryMplsPushParamsWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                       dev;
    GT_U32                                      memAddr;
    CPSS_EXMX_ROUTE_ENTRY_MPLS_PUSH_PARAMS_STC  mplsParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];
    mplsParams.pushMplsLabel = (GT_BOOL)inArgs[2];
    mplsParams.setExp = (GT_BOOL)inArgs[3];
    mplsParams.label = (GT_U32)inArgs[4];
    mplsParams.exp = (GT_U8)inArgs[5];
    mplsParams.ttl = (GT_U8)inArgs[6];


    /* call cpss api function */
    result = cpssExMxIpRouteEntryMplsPushParamsWrite(dev, memAddr, &mplsParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpUcRouteEntryTunnelTermWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       This function set the Tunnel termination parameters in the unicast
*       route entry.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev               - the device number
*       memAddr           - the memory address (offset) of the route entry.
*       tunnelTermInfoPtr - the Tunnel termination parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpUcRouteEntryTunnelTermWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                            dev;
    GT_U32                                           memAddr;
    CPSS_EXMX_UC_ROUTE_ENTRY_TUNNEL_TERM_PARAMS_STC  tunnelTermInfo;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];
    tunnelTermInfo.greTunnelTerm = (GT_BOOL)inArgs[2];
    tunnelTermInfo.ipTunnelTerm = (GT_BOOL)inArgs[3];
    tunnelTermInfo.tunnelCopyDscpExp = (GT_BOOL)inArgs[4];
    tunnelTermInfo.tunnelCopyTtl = (GT_BOOL)inArgs[5];


    /* call cpss api function */
    result = cpssExMxIpUcRouteEntryTunnelTermWrite(dev, memAddr, &tunnelTermInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/****************cpssExMxIpMcRouteEntries Table************/
/*set*/
static CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC*        gIPMCrouteEntryArrayPtr = NULL;
static GT_U32                                  gIPMCnumOfEntries = 0;

/*get*/
#define IPMC_ROUTE_ARR_MAX_SIZE 1024

static CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC
                       gIPMCgetRouteEntryArrayPtr[IPMC_ROUTE_ARR_MAX_SIZE];
static GT_U32          gIPMCgetRouteEntryCurrIndx = 0;
static GT_U32          gIPMCgetNumOfEntries;
/*******************************************************************************
* cpssExMxIpMcRouteEntryWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write an array of Mc route entries to the hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       baseMemAddr          - the base memory address (offset). The
*                              entries will be written from this base on.
*       routeEntriesArrayPtr - the mc route entries array
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcRouteEntriesWriteSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gIPMCnumOfEntries = 0;/*reset on first*/

    /* map input arguments to locals */
    if(gIPMCrouteEntryArrayPtr == NULL)
    {
        gIPMCrouteEntryArrayPtr = (CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC));
    }
    if(gIPMCrouteEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIPMCnumOfEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gIPMCrouteEntryArrayPtr[ind].cmd =
                                 (CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ENT)inFields[1];
    gIPMCrouteEntryArrayPtr[ind].mirrorToAnalyzer = (GT_BOOL)inFields[2];
    gIPMCrouteEntryArrayPtr[ind].forceCos = (GT_BOOL)inFields[3];
    gIPMCrouteEntryArrayPtr[ind].trafficClass = (GT_U8)inFields[4];
    gIPMCrouteEntryArrayPtr[ind].dropPrecedence =
                                                (CPSS_DP_LEVEL_ENT)inFields[5];
    gIPMCrouteEntryArrayPtr[ind].userPriority = (GT_U8)inFields[6];
    gIPMCrouteEntryArrayPtr[ind].ipCounterSet =
                                         (CPSS_EXMX_IP_CNT_SET_ENT)inFields[7];
    gIPMCrouteEntryArrayPtr[ind].enableDecTtlHopLimit = (GT_BOOL)inFields[8];
    gIPMCrouteEntryArrayPtr[ind].age = (GT_BOOL)inFields[9];
    gIPMCrouteEntryArrayPtr[ind].forceUserPriority = (GT_BOOL)inFields[10];
    gIPMCrouteEntryArrayPtr[ind].enableScopeCheck = (GT_BOOL)inFields[11];
    gIPMCrouteEntryArrayPtr[ind].destinationSiteId =
                                        (CPSS_EXMX_IP_SITE_ID_ENT)inFields[12];
    gIPMCrouteEntryArrayPtr[ind].internalMllPointer = (GT_U32)inFields[13];
    gIPMCrouteEntryArrayPtr[ind].mtu = (GT_U16)inFields[14];
    gIPMCrouteEntryArrayPtr[ind].rpfCommand =
                                     (CPSS_EXMX_IP_MC_RPF_CMD_ENT)inFields[15];
    gIPMCrouteEntryArrayPtr[ind].rpfInlifVid = (GT_U16)inFields[16];
    gIPMCrouteEntryArrayPtr[ind].externalMllPointer = (GT_U32)inFields[17];
    gIPMCrouteEntryArrayPtr[ind].deviceVidx = (GT_U16)inFields[18];
    gIPMCrouteEntryArrayPtr[ind].pushMplsLabel = (GT_BOOL)inFields[19];
    gIPMCrouteEntryArrayPtr[ind].setExp = (GT_BOOL)inFields[20];
    gIPMCrouteEntryArrayPtr[ind].label = (GT_U32)inFields[21];
    gIPMCrouteEntryArrayPtr[ind].exp = (GT_U8)inFields[22];
    gIPMCrouteEntryArrayPtr[ind].ttlHopLimit = (GT_U8)inFields[23];
    gIPMCrouteEntryArrayPtr[ind].mcMirrorCpuCodeEnable = (GT_BOOL)inFields[24];

    if(inFields[24] == 1)
    {
        gIPMCrouteEntryArrayPtr[ind].mirrorCmd.mcMirrorCmd =
                                  (CPSS_EXMX_IP_MC_MIRROR_CMD_ENT)inFields[25];
    }
    else
    {
     gIPMCrouteEntryArrayPtr[ind].mirrorCmd.mirrorToCpu = (GT_BOOL)inFields[26];
    }

    gIPMCnumOfEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpMcRouteEntryWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write an array of Mc route entries to the hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       baseMemAddr          - the base memory address (offset). The
*                              entries will be written from this base on.
*       routeEntriesArrayPtr - the mc route entries array
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcRouteEntriesWriteSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    gIPMCrouteEntryArrayPtr = (CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC*)
       cmdOsRealloc(gIPMCrouteEntryArrayPtr,
       sizeof(CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC) * (gIPMCnumOfEntries+1));

    if(gIPMCrouteEntryArrayPtr == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIPMCnumOfEntries)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gIPMCrouteEntryArrayPtr[ind].cmd =
                                 (CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ENT)inFields[1];
    gIPMCrouteEntryArrayPtr[ind].mirrorToAnalyzer = (GT_BOOL)inFields[2];
    gIPMCrouteEntryArrayPtr[ind].forceCos = (GT_BOOL)inFields[3];
    gIPMCrouteEntryArrayPtr[ind].trafficClass = (GT_U8)inFields[4];
    gIPMCrouteEntryArrayPtr[ind].dropPrecedence =
                                                (CPSS_DP_LEVEL_ENT)inFields[5];
    gIPMCrouteEntryArrayPtr[ind].userPriority = (GT_U8)inFields[6];
    gIPMCrouteEntryArrayPtr[ind].ipCounterSet =
                                         (CPSS_EXMX_IP_CNT_SET_ENT)inFields[7];
    gIPMCrouteEntryArrayPtr[ind].enableDecTtlHopLimit = (GT_BOOL)inFields[8];
    gIPMCrouteEntryArrayPtr[ind].age = (GT_BOOL)inFields[9];
    gIPMCrouteEntryArrayPtr[ind].forceUserPriority = (GT_BOOL)inFields[10];
    gIPMCrouteEntryArrayPtr[ind].enableScopeCheck = (GT_BOOL)inFields[11];
    gIPMCrouteEntryArrayPtr[ind].destinationSiteId =
                                        (CPSS_EXMX_IP_SITE_ID_ENT)inFields[12];
    gIPMCrouteEntryArrayPtr[ind].internalMllPointer = (GT_U32)inFields[13];
    gIPMCrouteEntryArrayPtr[ind].mtu = (GT_U16)inFields[14];
    gIPMCrouteEntryArrayPtr[ind].rpfCommand =
                                     (CPSS_EXMX_IP_MC_RPF_CMD_ENT)inFields[15];
    gIPMCrouteEntryArrayPtr[ind].rpfInlifVid = (GT_U16)inFields[16];
    gIPMCrouteEntryArrayPtr[ind].externalMllPointer = (GT_U32)inFields[17];
    gIPMCrouteEntryArrayPtr[ind].deviceVidx = (GT_U16)inFields[18];
    gIPMCrouteEntryArrayPtr[ind].pushMplsLabel = (GT_BOOL)inFields[19];
    gIPMCrouteEntryArrayPtr[ind].setExp = (GT_BOOL)inFields[20];
    gIPMCrouteEntryArrayPtr[ind].label = (GT_U32)inFields[21];
    gIPMCrouteEntryArrayPtr[ind].exp = (GT_U8)inFields[22];
    gIPMCrouteEntryArrayPtr[ind].ttlHopLimit = (GT_U8)inFields[23];
    gIPMCrouteEntryArrayPtr[ind].mcMirrorCpuCodeEnable = (GT_BOOL)inFields[24];

    if(inFields[24] == 1)
    {
        gIPMCrouteEntryArrayPtr[ind].mirrorCmd.mcMirrorCmd =
                                  (CPSS_EXMX_IP_MC_MIRROR_CMD_ENT)inFields[25];
    }
    else
    {
     gIPMCrouteEntryArrayPtr[ind].mirrorCmd.mirrorToCpu = (GT_BOOL)inFields[26];
    }

    gIPMCnumOfEntries++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpMcRouteEntryWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write an array of Mc route entries to the hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       baseMemAddr          - the base memory address (offset). The
*                              entries will be written from this base on.
*       routeEntriesArrayPtr - the mc route entries array
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcRouteEntriesWriteEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U8                                          dev;
    GT_U32                                         memAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];

    /* call cpss api function */
    status =  cpssExMxIpMcRouteEntryWrite(dev,memAddr, gIPMCrouteEntryArrayPtr,
                                       gIPMCnumOfEntries);

    cmdOsFree(gIPMCrouteEntryArrayPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

   return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpMcRouteEntryRead
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       read an array of Mc route entries from the hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       baseMemAddr          - the base memory address (offset). The
*                              entries will be read from this base on.
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       routeEntriesArrayPtr - the mc route entries array
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcRouteEntriesReadGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U32  memAddr;
    GT_U32  ind = gIPMCgetRouteEntryCurrIndx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    gIPMCgetRouteEntryCurrIndx = 0;/*reset on first*/

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxIpMcRouteEntryRead(dev,memAddr, gIPMCgetRouteEntryArrayPtr,
                                     gIPMCgetNumOfEntries);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gIPMCgetRouteEntryCurrIndx;

    inFields[1] = gIPMCgetRouteEntryArrayPtr[ind].cmd;
    inFields[2] = gIPMCgetRouteEntryArrayPtr[ind].mirrorToAnalyzer;
    inFields[3] = gIPMCgetRouteEntryArrayPtr[ind].forceCos;
    inFields[4] = gIPMCgetRouteEntryArrayPtr[ind].trafficClass;
    inFields[5] = gIPMCgetRouteEntryArrayPtr[ind].dropPrecedence;
    inFields[6] = gIPMCgetRouteEntryArrayPtr[ind].userPriority;
    inFields[7] = gIPMCgetRouteEntryArrayPtr[ind].ipCounterSet;
    inFields[8] = gIPMCgetRouteEntryArrayPtr[ind].enableDecTtlHopLimit;
    inFields[9] = gIPMCgetRouteEntryArrayPtr[ind].age;
    inFields[10] = gIPMCgetRouteEntryArrayPtr[ind].forceUserPriority;
    inFields[11] = gIPMCgetRouteEntryArrayPtr[ind].enableScopeCheck;
    inFields[12] = gIPMCgetRouteEntryArrayPtr[ind].destinationSiteId;
    inFields[13] = gIPMCgetRouteEntryArrayPtr[ind].internalMllPointer;
    inFields[14] = gIPMCgetRouteEntryArrayPtr[ind].mtu;
    inFields[15] = gIPMCgetRouteEntryArrayPtr[ind].rpfCommand;
    inFields[16] = gIPMCgetRouteEntryArrayPtr[ind].rpfInlifVid;
    inFields[17] = gIPMCgetRouteEntryArrayPtr[ind].externalMllPointer;
    inFields[18] = gIPMCgetRouteEntryArrayPtr[ind].deviceVidx;
    inFields[19] = gIPMCgetRouteEntryArrayPtr[ind].pushMplsLabel;
    inFields[20] = gIPMCgetRouteEntryArrayPtr[ind].setExp;
    inFields[21] = gIPMCgetRouteEntryArrayPtr[ind].label;
    inFields[22] = gIPMCgetRouteEntryArrayPtr[ind].exp;
    inFields[23] = gIPMCgetRouteEntryArrayPtr[ind].ttlHopLimit;
    inFields[24] = gIPMCgetRouteEntryArrayPtr[ind].mcMirrorCpuCodeEnable;
    inFields[25] = gIPMCgetRouteEntryArrayPtr[ind].mirrorCmd.mcMirrorCmd;
    inFields[26] = gIPMCgetRouteEntryArrayPtr[ind].mirrorCmd.mirrorToCpu;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26]);

    galtisOutput(outArgs, result, "%f");

    gIPMCgetRouteEntryCurrIndx++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpMcRouteEntryRead
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       read an array of Mc route entries from the hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev                  - the device number
*       baseMemAddr          - the base memory address (offset). The
*                              entries will be read from this base on.
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       routeEntriesArrayPtr - the mc route entries array
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcRouteEntriesReadGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32    ind = gIPMCgetRouteEntryCurrIndx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(gIPMCgetRouteEntryCurrIndx >= gIPMCgetNumOfEntries)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = gIPMCgetRouteEntryCurrIndx;

    inFields[1] = gIPMCgetRouteEntryArrayPtr[ind].cmd;
    inFields[2] = gIPMCgetRouteEntryArrayPtr[ind].mirrorToAnalyzer;
    inFields[3] = gIPMCgetRouteEntryArrayPtr[ind].forceCos;
    inFields[4] = gIPMCgetRouteEntryArrayPtr[ind].trafficClass;
    inFields[5] = gIPMCgetRouteEntryArrayPtr[ind].dropPrecedence;
    inFields[6] = gIPMCgetRouteEntryArrayPtr[ind].userPriority;
    inFields[7] = gIPMCgetRouteEntryArrayPtr[ind].ipCounterSet;
    inFields[8] = gIPMCgetRouteEntryArrayPtr[ind].enableDecTtlHopLimit;
    inFields[9] = gIPMCgetRouteEntryArrayPtr[ind].age;
    inFields[10] = gIPMCgetRouteEntryArrayPtr[ind].forceUserPriority;
    inFields[11] = gIPMCgetRouteEntryArrayPtr[ind].enableScopeCheck;
    inFields[12] = gIPMCgetRouteEntryArrayPtr[ind].destinationSiteId;
    inFields[13] = gIPMCgetRouteEntryArrayPtr[ind].internalMllPointer;
    inFields[14] = gIPMCgetRouteEntryArrayPtr[ind].mtu;
    inFields[15] = gIPMCgetRouteEntryArrayPtr[ind].rpfCommand;
    inFields[16] = gIPMCgetRouteEntryArrayPtr[ind].rpfInlifVid;
    inFields[17] = gIPMCgetRouteEntryArrayPtr[ind].externalMllPointer;
    inFields[18] = gIPMCgetRouteEntryArrayPtr[ind].deviceVidx;
    inFields[19] = gIPMCgetRouteEntryArrayPtr[ind].pushMplsLabel;
    inFields[20] = gIPMCgetRouteEntryArrayPtr[ind].setExp;
    inFields[21] = gIPMCgetRouteEntryArrayPtr[ind].label;
    inFields[22] = gIPMCgetRouteEntryArrayPtr[ind].exp;
    inFields[23] = gIPMCgetRouteEntryArrayPtr[ind].ttlHopLimit;
    inFields[24] = gIPMCgetRouteEntryArrayPtr[ind].mcMirrorCpuCodeEnable;
    inFields[25] = gIPMCgetRouteEntryArrayPtr[ind].mirrorCmd.mcMirrorCmd;
    inFields[26] = gIPMCgetRouteEntryArrayPtr[ind].mirrorCmd.mirrorToCpu;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26]);

    galtisOutput(outArgs, GT_OK, "%f");

    gIPMCgetRouteEntryCurrIndx++;

    return CMD_OK;
}


/*************cpssExMxIpMcLinkList Table**********************/
static GT_U32   NextMemAddr;
static GT_BOOL  theEnd = 0;
/*******************************************************************************
* cpssExMxIpMcLinkListWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write a Mc Link List (MLL) entry to hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev         - the device number
*       memAddr     - the memory address (offset from base).
*       mllEntryPtr - the Mc lisk list entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpMcLinkListWriteSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                          dev;
    GT_U32                         memAddr;
    CPSS_EXMX_IP_MC_LINK_LIST_STC  mllEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    memAddr = (GT_U32)inFields[0];

    mllEntry.nextPointer = (GT_U16)inFields[1];

    mllEntry.firstMllNode.last = (GT_BOOL)inFields[2];

    mllEntry.firstMllNode.outlif.outlifType =
                                    (CPSS_EXMX_IP_OUTLIF_TYPE_ENT)inFields[3];

    if(mllEntry.firstMllNode.outlif.outlifType == CPSS_EXMX_IP_OUTLIF_LL_E)
    {
        mllEntry.firstMllNode.outlif.outlifInfo.
                                        linkLayerOutLif.vid = (GT_U16)inFields[4];
        mllEntry.firstMllNode.outlif.outlifInfo.
                                   linkLayerOutLif.useVidx = (GT_BOOL)inFields[5];

        if(!mllEntry.firstMllNode.outlif.outlifInfo.linkLayerOutLif.useVidx)
        {
            mllEntry.firstMllNode.outlif.outlifInfo.linkLayerOutLif.
                            vidxOrTrunkPort.tarInfo.tarIsTrunk = (GT_BOOL)inFields[6];
            if(!mllEntry.firstMllNode.outlif.outlifInfo.linkLayerOutLif.
                                            vidxOrTrunkPort.tarInfo.tarIsTrunk)
            {
                mllEntry.firstMllNode.outlif.outlifInfo.
                    linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort =
                                                                       (GT_U8)inFields[7];
                mllEntry.firstMllNode.outlif.outlifInfo.
                    linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev =
                                                                       (GT_U8)inFields[8];
            }
            else
                mllEntry.firstMllNode.outlif.outlifInfo.
                linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk =
                                                                   (GT_U8)inFields[9];
        }
        else
            mllEntry.firstMllNode.outlif.outlifInfo.
                       linkLayerOutLif.vidxOrTrunkPort.vidx = (GT_U16)inFields[10];
    }

    else
    {
        mllEntry.firstMllNode.outlif.outlifInfo.
                                     tunnelOutLif.tunnelType = (GT_U8)inFields[11];
        mllEntry.firstMllNode.outlif.outlifInfo.
                                           tunnelOutLif.vid = (GT_U16)inFields[12];
        mllEntry.firstMllNode.outlif.outlifInfo.
                                 tunnelOutLif.tunnelPointer = (GT_U16)inFields[13];
        mllEntry.firstMllNode.outlif.outlifInfo.
                                   tunnelOutLif.tarIsTrunk = (GT_BOOL)inFields[14];
        if(!mllEntry.firstMllNode.outlif.outlifInfo.tunnelOutLif.tarIsTrunk)
        {
            mllEntry.firstMllNode.outlif.outlifInfo.
                       tunnelOutLif.trunkOrPort.portInfo.tarPort = (GT_U8)inFields[15];
            mllEntry.firstMllNode.outlif.outlifInfo.
                        tunnelOutLif.trunkOrPort.portInfo.tarDev = (GT_U8)inFields[16];
        }
        else
            mllEntry.firstMllNode.outlif.outlifInfo.
                 tunnelOutLif.trunkOrPort.trunkInfo.tarTrunk = (GT_U8)inFields[17];
    }
    mllEntry.firstMllNode.hopLimitThreshold = (GT_U16)inFields[18];
    mllEntry.firstMllNode.excludeSrcVlan = (GT_BOOL)inFields[19];
    mllEntry.firstMllNode.rpfCmd = (CPSS_EXMX_IP_MC_MLL_RPF_CMD_ENT)inFields[20];

    mllEntry.secondMllNode.last = (GT_BOOL)inFields[21];

    mllEntry.secondMllNode.outlif.outlifType =
                                    (CPSS_EXMX_IP_OUTLIF_TYPE_ENT)inFields[22];

    if(mllEntry.secondMllNode.outlif.outlifType == CPSS_EXMX_IP_OUTLIF_LL_E)
    {
        mllEntry.secondMllNode.outlif.outlifInfo.
                                        linkLayerOutLif.vid = (GT_U16)inFields[23];
        mllEntry.secondMllNode.outlif.outlifInfo.
                                   linkLayerOutLif.useVidx = (GT_BOOL)inFields[24];

        if(!mllEntry.secondMllNode.outlif.outlifInfo.linkLayerOutLif.useVidx)
        {
            mllEntry.secondMllNode.outlif.outlifInfo.linkLayerOutLif.
                            vidxOrTrunkPort.tarInfo.tarIsTrunk = (GT_BOOL)inFields[25];
            if(!mllEntry.secondMllNode.outlif.outlifInfo.linkLayerOutLif.
                                            vidxOrTrunkPort.tarInfo.tarIsTrunk)
            {
                mllEntry.secondMllNode.outlif.outlifInfo.
                    linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort =
                                                                       (GT_U8)inFields[26];
                mllEntry.secondMllNode.outlif.outlifInfo.
                    linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev =
                                                                       (GT_U8)inFields[27];
            }
            else
                mllEntry.secondMllNode.outlif.outlifInfo.
                linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk =
                                                                   (GT_U8)inFields[28];
        }
        else
            mllEntry.secondMllNode.outlif.outlifInfo.
                       linkLayerOutLif.vidxOrTrunkPort.vidx = (GT_U16)inFields[29];
    }

    else
    {
        mllEntry.secondMllNode.outlif.outlifInfo.
                                     tunnelOutLif.tunnelType = (GT_U8)inFields[30];
        mllEntry.secondMllNode.outlif.outlifInfo.
                                           tunnelOutLif.vid = (GT_U16)inFields[31];
        mllEntry.secondMllNode.outlif.outlifInfo.
                                 tunnelOutLif.tunnelPointer = (GT_U16)inFields[32];
        mllEntry.secondMllNode.outlif.outlifInfo.
                                   tunnelOutLif.tarIsTrunk = (GT_BOOL)inFields[33];
        if(!mllEntry.secondMllNode.outlif.outlifInfo.tunnelOutLif.tarIsTrunk)
        {
            mllEntry.secondMllNode.outlif.outlifInfo.
                       tunnelOutLif.trunkOrPort.portInfo.tarPort = (GT_U8)inFields[34];
            mllEntry.secondMllNode.outlif.outlifInfo.
                        tunnelOutLif.trunkOrPort.portInfo.tarDev = (GT_U8)inFields[35];
        }
        else
            mllEntry.secondMllNode.outlif.outlifInfo.
                 tunnelOutLif.trunkOrPort.trunkInfo.tarTrunk = (GT_U8)inFields[36];
    }
    mllEntry.secondMllNode.hopLimitThreshold = (GT_U16)inFields[37];
    mllEntry.secondMllNode.excludeSrcVlan = (GT_BOOL)inFields[38];
    mllEntry.secondMllNode.rpfCmd = (CPSS_EXMX_IP_MC_MLL_RPF_CMD_ENT)inFields[39];


    /* call cpss api function */
    result = cpssExMxIpMcLinkListWrite(dev, memAddr, &mllEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpMcLinkListRead
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Read a Mc Link List (MLL) entry from the hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev      - the device number
*       memAddr  - the memory address (offset from base).
*
*
* OUTPUTS:
*       mllEntryPtr - the Mc lisk list entry
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcLinkListReadGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           dev;
    GT_U32                          memAddr;
    CPSS_EXMX_IP_MC_LINK_LIST_STC   mllEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxIpMcLinkListRead(dev, memAddr, &mllEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = memAddr;
    inFields[1] = mllEntry.nextPointer;
    inFields[2] = mllEntry.firstMllNode.last;
    inFields[3] = mllEntry.firstMllNode.outlif.outlifType;
    inFields[4] = mllEntry.firstMllNode.outlif.outlifInfo.linkLayerOutLif.vid;
    inFields[5] = mllEntry.firstMllNode.outlif.outlifInfo.
                                                      linkLayerOutLif.useVidx;
    inFields[6] = mllEntry.firstMllNode.outlif.outlifInfo.linkLayerOutLif.
                                           vidxOrTrunkPort.tarInfo.tarIsTrunk;
    inFields[7] = mllEntry.firstMllNode.outlif.outlifInfo.
         linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort;
    inFields[8] = mllEntry.firstMllNode.outlif.outlifInfo.
        linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev;
    inFields[9] = mllEntry.firstMllNode.outlif.outlifInfo.
        linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk;
    inFields[10] = mllEntry.firstMllNode.outlif.outlifInfo.
                                          linkLayerOutLif.vidxOrTrunkPort.vidx;
    inFields[11] = mllEntry.firstMllNode.outlif.outlifInfo.
                                                       tunnelOutLif.tunnelType;
    inFields[12] = mllEntry.firstMllNode.outlif.outlifInfo.tunnelOutLif.vid;
    inFields[13] = mllEntry.firstMllNode.outlif.outlifInfo.
                                                    tunnelOutLif.tunnelPointer;
    inFields[14] = mllEntry.firstMllNode.outlif.outlifInfo.
                                                       tunnelOutLif.tarIsTrunk;
    inFields[15] = mllEntry.firstMllNode.outlif.outlifInfo.
                                     tunnelOutLif.trunkOrPort.portInfo.tarPort;
    inFields[16] = mllEntry.firstMllNode.outlif.outlifInfo.
                                      tunnelOutLif.trunkOrPort.portInfo.tarDev;
    inFields[17] = mllEntry.firstMllNode.outlif.outlifInfo.
                                   tunnelOutLif.trunkOrPort.trunkInfo.tarTrunk;

    inFields[18] = mllEntry.firstMllNode.hopLimitThreshold;
    inFields[19] = mllEntry.firstMllNode.excludeSrcVlan;
    inFields[20] = mllEntry.firstMllNode.rpfCmd;
    inFields[21] = mllEntry.secondMllNode.last;

    inFields[22] = mllEntry.secondMllNode.outlif.outlifType;
    inFields[23] = mllEntry.secondMllNode.outlif.outlifInfo.linkLayerOutLif.vid;
    inFields[24] = mllEntry.secondMllNode.outlif.outlifInfo.
                                                       linkLayerOutLif.useVidx;
    inFields[25] = mllEntry.secondMllNode.outlif.outlifInfo.linkLayerOutLif.
                                            vidxOrTrunkPort.tarInfo.tarIsTrunk;
    inFields[26] = mllEntry.secondMllNode.outlif.outlifInfo.
          linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort;
    inFields[27] = mllEntry.secondMllNode.outlif.outlifInfo.
           linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev;
    inFields[28] = mllEntry.secondMllNode.outlif.outlifInfo.
        linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk;
    inFields[29] = mllEntry.secondMllNode.outlif.outlifInfo.
                                          linkLayerOutLif.vidxOrTrunkPort.vidx;
    inFields[30] = mllEntry.secondMllNode.outlif.outlifInfo.
                                                       tunnelOutLif.tunnelType;
    inFields[31] = mllEntry.secondMllNode.outlif.outlifInfo.tunnelOutLif.vid;
    inFields[32] = mllEntry.secondMllNode.outlif.outlifInfo.
                                                    tunnelOutLif.tunnelPointer;
    inFields[33] = mllEntry.secondMllNode.outlif.outlifInfo.
                                                       tunnelOutLif.tarIsTrunk;
    inFields[34] = mllEntry.secondMllNode.outlif.outlifInfo.
                                     tunnelOutLif.trunkOrPort.portInfo.tarPort;
    inFields[35] = mllEntry.secondMllNode.outlif.outlifInfo.
                                      tunnelOutLif.trunkOrPort.portInfo.tarDev;
    inFields[36] = mllEntry.secondMllNode.outlif.outlifInfo.
                                   tunnelOutLif.trunkOrPort.trunkInfo.tarTrunk;

    inFields[37] = mllEntry.secondMllNode.hopLimitThreshold;
    inFields[38] = mllEntry.secondMllNode.excludeSrcVlan;
    inFields[39] = mllEntry.secondMllNode.rpfCmd;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32], inFields[33], inFields[34], inFields[35],
                inFields[36], inFields[37], inFields[38], inFields[39]);

    galtisOutput(outArgs, result, "%f");

    NextMemAddr = mllEntry.nextPointer;
    theEnd = !(mllEntry.firstMllNode.last);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpMcLinkListRead
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Read a Mc Link List (MLL) entry from the hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev      - the device number
*       memAddr  - the memory address (offset from base).
*
*
* OUTPUTS:
*       mllEntryPtr - the Mc lisk list entry
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpMcLinkListReadGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           dev;
    GT_U32                          memAddr;
    CPSS_EXMX_IP_MC_LINK_LIST_STC   mllEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    memAddr = NextMemAddr;

    if(theEnd)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxIpMcLinkListRead(dev, memAddr, &mllEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = memAddr;
    inFields[1] = mllEntry.nextPointer;
    inFields[2] = mllEntry.firstMllNode.last;
    inFields[3] = mllEntry.firstMllNode.outlif.outlifType;
    inFields[4] = mllEntry.firstMllNode.outlif.outlifInfo.linkLayerOutLif.vid;
    inFields[5] = mllEntry.firstMllNode.outlif.outlifInfo.
                                                      linkLayerOutLif.useVidx;
    inFields[6] = mllEntry.firstMllNode.outlif.outlifInfo.linkLayerOutLif.
                                           vidxOrTrunkPort.tarInfo.tarIsTrunk;
    inFields[7] = mllEntry.firstMllNode.outlif.outlifInfo.
         linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort;
    inFields[8] = mllEntry.firstMllNode.outlif.outlifInfo.
        linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev;
    inFields[9] = mllEntry.firstMllNode.outlif.outlifInfo.
        linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk;
    inFields[10] = mllEntry.firstMllNode.outlif.outlifInfo.
                                          linkLayerOutLif.vidxOrTrunkPort.vidx;
    inFields[11] = mllEntry.firstMllNode.outlif.outlifInfo.
                                                       tunnelOutLif.tunnelType;
    inFields[12] = mllEntry.firstMllNode.outlif.outlifInfo.tunnelOutLif.vid;
    inFields[13] = mllEntry.firstMllNode.outlif.outlifInfo.
                                                    tunnelOutLif.tunnelPointer;
    inFields[14] = mllEntry.firstMllNode.outlif.outlifInfo.
                                                       tunnelOutLif.tarIsTrunk;
    inFields[15] = mllEntry.firstMllNode.outlif.outlifInfo.
                                     tunnelOutLif.trunkOrPort.portInfo.tarPort;
    inFields[16] = mllEntry.firstMllNode.outlif.outlifInfo.
                                      tunnelOutLif.trunkOrPort.portInfo.tarDev;
    inFields[17] = mllEntry.firstMllNode.outlif.outlifInfo.
                                   tunnelOutLif.trunkOrPort.trunkInfo.tarTrunk;

    inFields[18] = mllEntry.firstMllNode.hopLimitThreshold;
    inFields[19] = mllEntry.firstMllNode.excludeSrcVlan;
    inFields[20] = mllEntry.firstMllNode.rpfCmd;
    inFields[21] = mllEntry.secondMllNode.last;

    inFields[22] = mllEntry.secondMllNode.outlif.outlifType;
    inFields[23] = mllEntry.secondMllNode.outlif.outlifInfo.linkLayerOutLif.vid;
    inFields[24] = mllEntry.secondMllNode.outlif.outlifInfo.
                                                       linkLayerOutLif.useVidx;
    inFields[25] = mllEntry.secondMllNode.outlif.outlifInfo.linkLayerOutLif.
                                            vidxOrTrunkPort.tarInfo.tarIsTrunk;
    inFields[26] = mllEntry.secondMllNode.outlif.outlifInfo.
          linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort;
    inFields[27] = mllEntry.secondMllNode.outlif.outlifInfo.
           linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev;
    inFields[28] = mllEntry.secondMllNode.outlif.outlifInfo.
        linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.trunkInfo.tarTrunk;
    inFields[29] = mllEntry.secondMllNode.outlif.outlifInfo.
                                          linkLayerOutLif.vidxOrTrunkPort.vidx;
    inFields[30] = mllEntry.secondMllNode.outlif.outlifInfo.
                                                       tunnelOutLif.tunnelType;
    inFields[31] = mllEntry.secondMllNode.outlif.outlifInfo.tunnelOutLif.vid;
    inFields[32] = mllEntry.secondMllNode.outlif.outlifInfo.
                                                    tunnelOutLif.tunnelPointer;
    inFields[33] = mllEntry.secondMllNode.outlif.outlifInfo.
                                                       tunnelOutLif.tarIsTrunk;
    inFields[34] = mllEntry.secondMllNode.outlif.outlifInfo.
                                     tunnelOutLif.trunkOrPort.portInfo.tarPort;
    inFields[35] = mllEntry.secondMllNode.outlif.outlifInfo.
                                      tunnelOutLif.trunkOrPort.portInfo.tarDev;
    inFields[36] = mllEntry.secondMllNode.outlif.outlifInfo.
                                   tunnelOutLif.trunkOrPort.trunkInfo.tarTrunk;

    inFields[37] = mllEntry.secondMllNode.hopLimitThreshold;
    inFields[38] = mllEntry.secondMllNode.excludeSrcVlan;
    inFields[39] = mllEntry.secondMllNode.rpfCmd;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32], inFields[33], inFields[34], inFields[35],
                inFields[36], inFields[37], inFields[38], inFields[39]);

    galtisOutput(outArgs, result, "%f");

    NextMemAddr = mllEntry.nextPointer;
    theEnd = !(mllEntry.firstMllNode.last);


    return CMD_OK;
}


/*******************************************************************************
* cpssExMxIpMcLinkListLastBitWrite
*
* DESCRIPTION:
*    Function Relevant mode : Low Level mode
*       Write a Mc Link List (MLL) Last bit to the hw.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev         - the device number
*       memAddr     - the memory address (offset from base).
*       lastBit     - the Last bit
*       writeToFirst- weather to write the last bit of the first Mc link list in
*                     the MLL or the second (on = GT_FALSE)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpMcLinkListLastBitWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                         dev;
    GT_U32                        memAddr;
    GT_BOOL                       lastBit;
    GT_BOOL                       writeToFirst;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];
    lastBit = (GT_BOOL)inArgs[2];
    writeToFirst = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxIpMcLinkListLastBitWrite(dev, memAddr, lastBit,
                                                                writeToFirst);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}





/*ctrl*/


/*******************************************************************************
* cpssExMxIpCtrlAgeRefreshEnable
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       This routine enable disable the IP aging refreash
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       devNum - The device number.
*       enable - enable or disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpCtrlAgeRefreshEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                         devNum;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxIpCtrlAgeRefreshEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       This routine sets the 2 MSB of the flow redirect route entries base
*       address
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA
*
* INPUTS:
*       devNum - The device number.
*       flowRouteEntBaseAddr - the flow redirect route entries base address
*                              (offset)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_U32  flowRouteEntBaseAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    flowRouteEntBaseAddr = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet(devNum,
                                                         flowRouteEntBaseAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpMLLEntriesBaseAddrSet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       This routine sets the Multicast Link List entries base address
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA
*
* INPUTS:
*       devNum - The device number.
*       mllBaseAddr - the Multicast Link List entries base address (offset)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpMLLEntriesBaseAddrSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_U32  mllBaseAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mllBaseAddr = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxIpMLLEntriesBaseAddrSet(devNum, mllBaseAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpVrBaseAddrSet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*       Sets the Ip Vr table base Address in Hw MC,UC or both
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       devNum - The device number.
*       protocolStack - types of IP VR base to set.
*       baseAddrType - weather this base address is of UC ,MC or both.
*       vrBaseAddr - the Vr table base Address (offset)
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*
* COMMENTS:
*       none.
*******************************************************************************/
CMD_STATUS wrCpssExMxIpVrBaseAddrSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    CPSS_IP_PROTOCOL_STACK_ENT          protocolStack;
    CPSS_EXMX_IP_VR_BASE_ADDR_TYPE_ENT  baseAddrType;
    GT_U32                              vrBaseAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    baseAddrType = (CPSS_EXMX_IP_VR_BASE_ADDR_TYPE_ENT)inArgs[2];
    vrBaseAddr = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxIpVrBaseAddrSet(devNum, protocolStack, baseAddrType,
                                                                    vrBaseAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*************cpssExMxIpCntSetGet shown as Table****************/

static CPSS_EXMX_IP_CNT_SET_ENT   gCntSet;
static CPSS_IP_PROTOCOL_STACK_ENT gProtocolStack;
/*******************************************************************************
* cpssExMxIpCntSetGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       Return the IP counter set requested.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev       - physical device number.
*       cntSet    - counters set to retrieve.
*       protocolStack - types of IP cnt to get.
*
* OUTPUTS:
*       counters  - struct contains the counter values.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpCntSetGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    CPSS_EXMX_IP_COUNTERS_STC  counters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gProtocolStack = CPSS_IP_PROTOCOL_IPV4_E;/*reset on first*/
    gCntSet = CPSS_EXMX_IP_CNT_SET0_E;/*reset on first*/

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    cmdOsMemSet(&counters, 0, sizeof(counters));

    /* call cpss api function */
    result = cpssExMxIpCntSetGet(dev, gCntSet, gProtocolStack, &counters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gProtocolStack;
    inFields[1] = gCntSet;

    inFields[2] = counters.rxPkts;
    inFields[3] = (GT_U32)galtisU64FOut(&(counters.rxOctets));
    inFields[4] = counters.discardedPkts;
    inFields[5] = counters.discardedOctets;
    inFields[6] = counters.ipMcPckts;
    inFields[7] = counters.trappedPkts;
    inFields[8] = counters.terminatedTunnelPkts;
    inFields[9] = counters.rfpFailPkts;

    /* pack and output table fields */
    fieldOutput("%d%d%d%s%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9]);

    galtisOutput(outArgs, result, "%f");

    gCntSet++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpCntSetGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       Return the IP counter set requested.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev       - physical device number.
*       cntSet    - counters set to retrieve.
*       protocolStack - types of IP cnt to get.
*
* OUTPUTS:
*       counters  - struct contains the counter values.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpCntSetGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    CPSS_EXMX_IP_COUNTERS_STC  counters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;



    if(gCntSet >= CPSS_EXMX_IP_CNT_NO_SET_E)
    {
       gProtocolStack++;
       gCntSet = CPSS_EXMX_IP_CNT_SET0_E;
    }

    /*CPSS_IP_PROTOCOL_IPV4V6_E Not supported by API*/
    if(gProtocolStack >= CPSS_IP_PROTOCOL_IPV4V6_E)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* init with zeros */
    cmdOsMemSet(&counters, 0, sizeof(counters));

    /* call cpss api function */
    result = cpssExMxIpCntSetGet(dev, gCntSet, gProtocolStack, &counters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gProtocolStack;
    inFields[1] = gCntSet;

    inFields[2] = counters.rxPkts;
    inFields[3] = (GT_U32)galtisU64FOut(&(counters.rxOctets));
    inFields[4] = counters.discardedPkts;
    inFields[5] = counters.discardedOctets;
    inFields[6] = counters.ipMcPckts;
    inFields[7] = counters.trappedPkts;
    inFields[8] = counters.terminatedTunnelPkts;
    inFields[9] = counters.rfpFailPkts;

    /* pack and output table fields */
    fieldOutput("%d%d%d%s%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9]);

    galtisOutput(outArgs, result, "%f");

    gCntSet++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpCntSetClear
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       Clear the IP counter set requested.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev       - physical device number.
*       cntSet    - counters set to clear.
*       protocolStack - types of IP cnt to clear.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpCntSetClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    CPSS_EXMX_IP_CNT_SET_ENT    cntSet;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntSet = (CPSS_EXMX_IP_CNT_SET_ENT)inFields[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inFields[0];

    /* call cpss api function */
    result = cpssExMxIpCntSetClear(devNum, cntSet, protocolStack);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*************cpssExMxIpCntGlblGet shown as Table****************/

static CPSS_IP_PROTOCOL_STACK_ENT gProtocolStack;
/*******************************************************************************
* cpssExMxIpCntGlblGet
*
* DESCRIPTION:
*       Get the IP Global counters.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev       - physical device number.
*       protocolStack  - type of ip protocol stack to work on.
*
* OUTPUTS:
*       counters  - struct contains the counter values.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
***********************************************************************/
static CMD_STATUS wrCpssExMxIpCntGlblGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    CPSS_EXMX_IP_GLOBAL_COUNTERS_STC  counters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gProtocolStack = CPSS_IP_PROTOCOL_IPV4_E;/*reset on first*/

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* init with zeros */
    cmdOsMemSet(&counters, 0, sizeof(counters));

    /* call cpss api function */
    result = cpssExMxIpCntGlblGet(dev, gProtocolStack, &counters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gProtocolStack;
    inFields[1] = gCntSet;

    inFields[2] = counters.rxPkts;
    inFields[3] = (GT_U32)galtisU64FOut(&(counters.rxOctets));
    inFields[4] = counters.discardedPkts;
    inFields[5] = (GT_U32)galtisU64FOut(&(counters.discardedOctets));

    /* pack and output table fields */
    fieldOutput("%d%d%d%s%d%s",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5]);

    galtisOutput(outArgs, result, "%f");

    gProtocolStack++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpCntGlblGet
*
* DESCRIPTION:
*       Get the IP Global counters.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       dev       - physical device number.
*       protocolStack  - type of ip protocol stack to work on.
*
* OUTPUTS:
*       counters  - struct contains the counter values.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
***********************************************************************/
static CMD_STATUS wrCpssExMxIpCntGlblGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    CPSS_EXMX_IP_GLOBAL_COUNTERS_STC  counters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*CPSS_IP_PROTOCOL_IPV4V6_E Not supported by API*/
    if(gProtocolStack >= CPSS_IP_PROTOCOL_IPV4V6_E)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* init with zeros */
    cmdOsMemSet(&counters, 0, sizeof(counters));

    /* call cpss api function */
    result = cpssExMxIpCntGlblGet(dev, gProtocolStack, &counters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gProtocolStack;
    inFields[1] = gCntSet;

    inFields[2] = counters.rxPkts;
    inFields[3] = (GT_U32)galtisU64FOut(&(counters.rxOctets));
    inFields[4] = counters.discardedPkts;
    inFields[5] = (GT_U32)galtisU64FOut(&(counters.discardedOctets));

    /* pack and output table fields */
    fieldOutput("%d%d%d%s%d%s",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5]);

    galtisOutput(outArgs, result, "%f");

    gProtocolStack++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpCtrlExceptionCmdSet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       This routine sets the command for a specific exception.
*       to see which execptions , protocolStacks, and commands are supported
*       for which device see CPSS_EXMX_IP_EXCP_TYPE_ENT
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       devNum        - the device number.
*       exceptionType - the exception type.
*       protocolStack - the ip protocol to set for.
*       exceptionCmd  - the exception
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpCtrlExceptionCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                         devNum;
    CPSS_EXMX_IP_EXCP_TYPE_ENT    exceptionType;
    CPSS_IP_PROTOCOL_STACK_ENT    protocolStack;
    CPSS_EXMX_IP_EXCP_CMD_ENT     exceptionCmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_EXMX_IP_EXCP_TYPE_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    exceptionCmd = (CPSS_EXMX_IP_EXCP_CMD_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxIpCtrlExceptionCmdSet(devNum, exceptionType,
                                           protocolStack, exceptionCmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpCtrlMcRpfCheckModeSet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       Set RPF check mode to VLAN based or InLif based on a device.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       devNum - the device number
*       protocolStack - types of IP protocol stack to set.
*       rpfCheckMode - RPF check mode - VLAN based or InLif based
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpCtrlMcRpfCheckModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    CPSS_IP_PROTOCOL_STACK_ENT          protocolStack;
    CPSS_EXMX_IP_RPF_CHECK_MODE_ENT     rpfCheckMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    rpfCheckMode = (CPSS_EXMX_IP_RPF_CHECK_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxIpCtrlMcRpfCheckModeSet(devNum, protocolStack,
                                                                rpfCheckMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpv4CtrlSpecialMcRouteEnable
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       Enable/Disable IP multicast lookup and forwarding for special
*       multicast packets in the range 224.0.0/24
*
* APPLICABLE DEVICES:
*       TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       devNum      - the device number.
*       enable  - GT_TRUE enable this feature
*                 when enabled, IP multicast packet in the range 224.0.0./24
*                 are handled by the Multicast router
*                 GT_False Disable the feature
*                 When disabled (default), IP multicast in the range 224.0.0/24
*                 are treated transparently by the IP Multicast router, i.e
*                 there is no lookup nor any IPM forwarding
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_NOT_SUPPORTED - not supported on one or more device in the system
*       GT_FAIL - on error
*
* COMMENTS:
*       This feature is not supported on all devices
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpv4CtrlSpecialMcRouteEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8            devNum;
    GT_BOOL          enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxIpv4CtrlSpecialMcRouteEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpv4CtrlTrapPktTcSet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       This routine set the IPv4 trap to CPU packet traffic class.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA
*
* INPUTS:
*       devNum  - the device number.
*       tc      - traffic class to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpv4CtrlTrapPktTcSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8          devNum;
    GT_U8          tc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tc = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxIpv4CtrlTrapPktTcSet(devNum, tc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpv4AgeOverrunEnable
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       This routine enable disable the IPv4 aging overrun.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       devNum - The device number.
*       enable - enable or disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*           Twist 2 Only - Set bit number 14 offset 0x02800100.
*******************************************************************************/
CMD_STATUS wrCpssExMxIpv4AgeOverrunEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8            devNum;
    GT_BOOL          enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxIpv4AgeOverrunEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpv4McBridgeRpfFailureEnable
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       Globally enable/disable bridging of IP Multicast packets to the source
*       VLAN in the event that it failed its RPF check and its RPF command is
*       set to CPSS_EXMX_IP_RPF_FAIL_DO_NOT_ROUTE_E
*
* APPLICABLE DEVICES:
*       TWIST-D
*
* INPUTS:
*       devNum      - the device number.
*       Enable      - GT_TRUE enable this feature
*                     GT_False Disable the feature
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - not supported on one or more device in the system
*
* COMMENTS:
*       This feature is not supported on all devices
*
*
* GalTis:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxIpv4McBridgeRpfFailureEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8            devNum;
    GT_BOOL          enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxIpv4McBridgeRpfFailureEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* Function: cpssExMxIpDefaultMcRouteEntriesBaseAddrSet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*       Set the Default Mc route Entries base Address in the Hw.
*       The default Mc route entries for each of the VR should be in an array
*       from this address on.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA
*
* INPUTS:
*       devNum      - the device number.
*       memAddr    - the defualt mc route entries address (offset)
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*
* COMMENTS:
*       none.
*******************************************************************************/
CMD_STATUS wrCpssExMxIpDefaultMcRouteEntriesBaseAddrSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8           devNum;
    GT_U32          memAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memAddr = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxIpDefaultMcRouteEntriesBaseAddrSet(devNum, memAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxIpHwLpmNodeSetFirst",
        &wrCpssExMxIpHwLpmNodeWriteNextPointerArraySetFirst,
        5, 22},

    {"cpssExMxIpHwLpmNodeSetNext",
        &wrCpssExMxIpHwLpmNodeWriteNextPointerArraySetNext,
        5, 22},

    {"cpssExMxIpHwLpmNodeEndSet",
        &wrCpssExMxIpHwLpmNodeWriteNextPointerArrayEndSet,
        5, 0},

    {"cpssExMxIpHwLpmNodeGetFirst",
        &wrCpssExMxIpHwLpmNodeReadNextPointerGetFirst,
        5, 0},

    {"cpssExMxIpHwLpmNodeGetNext",
        &wrCpssExMxIpHwLpmNodeReadNextPointerGetNext,
        5, 0},

    {"cpssExMxIpMcTreeNodeSetFirst",
        &wrCpssExMxIpMcTreeNodeWriteSetFirst,
        2, 4},

    {"cpssExMxIpMcTreeNodeSetNext",
        &wrCpssExMxIpMcTreeNodeWriteSetNext,
        2, 4},

    {"cpssExMxIpMcTreeNodeEndSet",
        &wrCpssExMxIpMcTreeNodeWriteEndSet,
        2, 0},

    {"cpssExMxIpMcTreeNodeGetFirst",
        &wrCpssExMxIpMcTreeNodeReadGetFirst,
        2, 0},

    {"cpssExMxIpMcTreeNodeGetNext",
        &wrCpssExMxIpMcTreeNodeReadGetNext,
        2, 0},


    {"cpssExMxIpUcRouteSetFirst",
        &wrCpssExMxIpUcRouteEntriesWriteSetFirst,
        0, 41},

    {"cpssExMxIpUcRouteSetNext",
        &wrCpssExMxIpUcRouteEntriesWriteSetNext,
        0, 41},

    {"cpssExMxIpUcRouteEndSet",
        &wrCpssExMxIpUcRouteEntriesWriteEndSet,
        2, 0},

    {"cpssExMxIpUcRouteGetFirst",
        &wrCpssExMxIpUcRouteEntriesReadGetFirst,
        2, 0},

    {"cpssExMxIpUcRouteGetNext",
        &wrCpssExMxIpUcRouteEntriesReadGetNext,
        2, 0},


    {"cpssExMxIpRouteEntryActiveBitReadAndReset",
        &wrCpssExMxIpRouteEntryActiveBitReadAndReset,
        3, 0},


    {"cpssExMxIpRouteEntryFieldWrite",
        &wrCpssExMxIpRouteEntryFieldWrite,
        7, 0},


    {"cpssExMxIpRouteEntryMplsPushParamsWrite",
        &wrCpssExMxIpRouteEntryMplsPushParamsWrite,
        7, 0},


    {"cpssExMxIpUcRouteEntryTunnelTermWrite",
        &wrCpssExMxIpUcRouteEntryTunnelTermWrite,
        6, 0},


    {"cpssExMxIpMcRouteSetFirst",
        &wrCpssExMxIpMcRouteEntriesWriteSetFirst,
        0, 27},

    {"cpssExMxIpMcRouteSetNext",
        &wrCpssExMxIpMcRouteEntriesWriteSetNext,
        0, 27},

    {"cpssExMxIpMcRouteEndSet",
        &wrCpssExMxIpMcRouteEntriesWriteEndSet,
        2, 0},

    {"cpssExMxIpMcRouteGetFirst",
        &wrCpssExMxIpMcRouteEntriesReadGetFirst,
        2, 0},

    {"cpssExMxIpMcRouteGetNext",
        &wrCpssExMxIpMcRouteEntriesReadGetNext,
        2, 0},


    {"cpssExMxIpMcLinkListSet",
        &wrCpssExMxIpMcLinkListWriteSet,
        1, 40},

    {"cpssExMxIpMcLinkListGetFirst",
        &wrCpssExMxIpMcLinkListReadGetFirst,
        1, 0},

    {"cpssExMxIpMcLinkListGetNext",
        &wrCpssExMxIpMcLinkListReadGetNext,
        1, 0},


    {"cpssExMxIpMcLinkListLastBitWrite",
        &wrCpssExMxIpMcLinkListLastBitWrite,
        4, 0},


/*ctrl*/


    {"cpssExMxIpCtrlAgeRefreshEnable",
        &wrCpssExMxIpCtrlAgeRefreshEnable,
        2, 0},

    {"cpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet",
        &wrCpssExMxIpFlowRedirectRouteEntriesMSBBaseAddrSet,
        2, 0},


    {"cpssExMxIpMLLEntriesBaseAddrSet",
        &wrCpssExMxIpMLLEntriesBaseAddrSet,
        2, 0},


    {"cpssExMxIpVrBaseAddrSet",
        &wrCpssExMxIpVrBaseAddrSet,
        4, 0},


    {"cpssExMxIpCntSetGetFirst",
        &wrCpssExMxIpCntSetGetFirst,
        1, 0},


    {"cpssExMxIpCntSetGetNext",
        &wrCpssExMxIpCntSetGetNext,
        1, 0},


    {"cpssExMxIpCntSetDelete",
        &wrCpssExMxIpCntSetClear,
        1, 2},

    {"cpssExMxIpCntGlblGetFirst",
        &wrCpssExMxIpCntGlblGetFirst,
        1, 0},

    {"cpssExMxIpCntGlblGetNext",
        &wrCpssExMxIpCntGlblGetNext,
        1, 0},

    {"cpssExMxIpCtrlExceptionCmdSet",
        &wrCpssExMxIpCtrlExceptionCmdSet,
        4, 0},

    {"cpssExMxIpCtrlMcRpfCheckModeSet",
        &wrCpssExMxIpCtrlMcRpfCheckModeSet,
        3, 0},


    {"cpssExMxIpv4CtrlSpecialMcRouteEnable",
        &wrCpssExMxIpv4CtrlSpecialMcRouteEnable,
        2, 0},

    {"cpssExMxIpv4CtrlTrapPktTcSet",
        &wrCpssExMxIpv4CtrlTrapPktTcSet,
        2, 0},

    {"cpssExMxIpv4AgeOverrunEnable",
        &wrCpssExMxIpv4AgeOverrunEnable,
        2, 0},


    {"cpssExMxIpv4McBridgeRpfFailureEnable",
        &wrCpssExMxIpv4McBridgeRpfFailureEnable,
        2, 0},

    {"cpssExMxIpDefaultMcRouteEntriesBaseAddrSet",
        &wrCpssExMxIpDefaultMcRouteEntriesBaseAddrSet,
        2, 0}



};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPcl
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
GT_STATUS cmdLibInitCpssExMxIp
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


