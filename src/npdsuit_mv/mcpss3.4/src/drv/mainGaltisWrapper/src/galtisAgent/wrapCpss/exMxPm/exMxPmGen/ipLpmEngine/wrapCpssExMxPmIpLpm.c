/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmIpLpm.c
*
* DESCRIPTION:
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
#include <cpss/exMxPm/exMxPmGen/ipLpmEngine/cpssExMxPmIpLpm.h>
#include <cpss/exMxPm/exMxPmGen/ipLpmEngine/cpssExMxPmIpLpmDbg.h>

/******************************************************************************/
static GT_U32 numOfVirtualRouters;
static GT_U32 vrId;
static GT_BOOL      isDefaultVrfIdEntry = GT_FALSE;
static GT_U32       lastVrfIdTreated;
static GT_IPADDR    ipv4Group;
static GT_IPADDR    ipv4Src;
static GT_U32       ipv4SrcPrefixLen;
static GT_U32       *lpmDb[100];

/*******************************************************************************
* cpssExMxPmIpLpmDbCreate
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       LPM DB is a database that holds LPM shadow information that is shared
*       to one PP or more. LPM DB holds one virtual router or more according
*       to the configuration of the LPM DB. The LPM DB manage adding, deleting\
*       and searching IPv4/6 UC/MC prefixes.
*       This function creates LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDBId           - the LPM DB id that is used to refer to this LPM DB
*                           in other API's (range 32 bits)
*       lpmDBConfigPtr    - points to configuration parameters for the lpm DB,
*                           refer to CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC for more
*                           info
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_ALREADY_EXIST  - if the LPM DB id is already used
*       GT_BAD_PARAM      - on illegal parameter
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*       GT_FAIL           - on error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/

static GT_U32 ipMemCfgArrayId;
static CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC lpmDbConfig;
static GT_U32 lpmDbId;
static CPSS_EXMXPM_IP_MEM_CFG_STC ipMemCfgArray[10];

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbCreateSetFirst
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

    if(inFields[7] > 10)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(ipMemCfgArray, 0, sizeof(ipMemCfgArray));
    ipMemCfgArrayId = 0; /* reset on first*/

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipMemCfgArray[ipMemCfgArrayId].routingSramCfgType = (CPSS_EXMXPM_LPM_SRAM_CFG_ENT)inFields[0];
    ipMemCfgArray[ipMemCfgArrayId].sramsSizeArray[0] = (CPSS_SRAM_SIZE_ENT)inFields[1];
    ipMemCfgArray[ipMemCfgArrayId].sramsSizeArray[1] = (CPSS_SRAM_SIZE_ENT)inFields[2];
    ipMemCfgArray[ipMemCfgArrayId].sramsSizeArray[2] = (CPSS_SRAM_SIZE_ENT)inFields[3];
    ipMemCfgArray[ipMemCfgArrayId].sramsSizeArray[3] = (CPSS_SRAM_SIZE_ENT)inFields[4];
    ipMemCfgArray[ipMemCfgArrayId].sramsSizeArray[4] = (CPSS_SRAM_SIZE_ENT)inFields[5];
    ipMemCfgArray[ipMemCfgArrayId].numOfSramsSizes = inFields[6];
    lpmDbConfig.numOfMemCfg = inFields[7];
    lpmDbConfig.protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inFields[8];
    lpmDbConfig.ipv6MemShare = inFields[9];
    lpmDbConfig.numOfVirtualRouters = inFields[10];

    ipMemCfgArrayId++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbCreateSetNext
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
    ipMemCfgArray[ipMemCfgArrayId].routingSramCfgType = (CPSS_EXMXPM_LPM_SRAM_CFG_ENT)inFields[0];
    ipMemCfgArray[ipMemCfgArrayId].sramsSizeArray[0] = (CPSS_SRAM_SIZE_ENT)inFields[1];
    ipMemCfgArray[ipMemCfgArrayId].sramsSizeArray[1] = (CPSS_SRAM_SIZE_ENT)inFields[2];
    ipMemCfgArray[ipMemCfgArrayId].sramsSizeArray[2] = (CPSS_SRAM_SIZE_ENT)inFields[3];
    ipMemCfgArray[ipMemCfgArrayId].sramsSizeArray[3] = (CPSS_SRAM_SIZE_ENT)inFields[4];
    ipMemCfgArray[ipMemCfgArrayId].sramsSizeArray[4] = (CPSS_SRAM_SIZE_ENT)inFields[5];
    ipMemCfgArray[ipMemCfgArrayId].numOfSramsSizes = inFields[6];

    ipMemCfgArrayId++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbCreateEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDbConfig.memCfgArray = ipMemCfgArray;

    /* call cpss api function */
    result = cpssExMxPmIpLpmDbCreate(lpmDbId, &lpmDbConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmDbDelete
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       LPM DB is a database that holds LPM shadow information that is shared
*       to one PP or more. LPM DB holds one virtual router or more according
*       to the configuration of the LPM DB. The LPM DB manage adding, deleting\
*       and searching IPv4/6 UC/MC prefixes.
*       This function deletes LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - the LPM DB id (range 32 bits)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_FOUND      - if the LPM DB id is not found
*       GT_HW_ERROR       - on hardware error
*       GT_FAIL           - on error
*
* COMMENTS:
*       Deleting LPM DB is allowed only if no VR present in the LPM DB (since
*       prefixes reside within VR it means that no prefixes present as well).
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    lpmDbId = (GT_U32)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpLpmDbDelete(lpmDbId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmDbConfigGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       LPM DB is a database that holds LPM shadow information that is shared
*       to one PP or more. LPM DB holds one virtual router or more according
*       to the configuration of the LPM DB. The LPM DB manage adding, deleting
*       and searching IPv4/6 UC/MC prefixes.
*       This function retrieves configuration of LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - the LPM DB id (range 32 bits)
*       lpmDbConfigPtr    - points to lpm DB configuration
*
* OUTPUTS:
*       lpmDbConfigPtr    - points to lpm DB configuration
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_FOUND      - if the LPM DB id is not found
*       GT_BAD_PTR        - on NULL pointer
*       GT_BAD_SIZE       - in case not enough memory was allocated to memory
*                           configurations
*
* COMMENTS:
*       The application should allocate memory for the memory configurations
*       array (field in the LPM DB configuration). The number of memory
*       configuration allocated is passed in the LPM DB configuration structure.
*       If the function returns GT_OK, then memory configuration number holds
*       the number of memory configurations the function filled.
*       If the function returns GT_BAD_SIZE, then the memory allocated by the
*       application to the memory configurations is not enough. In this case the
*       number of memory configurations field will hold the number of needed
*       memory configurations.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbConfigGet
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
    inFields[0] = lpmDbConfig.memCfgArray[ipMemCfgArrayId].routingSramCfgType;
    inFields[1] = lpmDbConfig.memCfgArray[ipMemCfgArrayId].sramsSizeArray[0];
    inFields[2] = lpmDbConfig.memCfgArray[ipMemCfgArrayId].sramsSizeArray[1];
    inFields[3] = lpmDbConfig.memCfgArray[ipMemCfgArrayId].sramsSizeArray[2];
    inFields[4] = lpmDbConfig.memCfgArray[ipMemCfgArrayId].sramsSizeArray[3];
    inFields[5] = lpmDbConfig.memCfgArray[ipMemCfgArrayId].sramsSizeArray[4];
    inFields[6] = lpmDbConfig.memCfgArray[ipMemCfgArrayId].numOfSramsSizes;
    inFields[7] = lpmDbConfig.numOfMemCfg;
    inFields[8] = lpmDbConfig.protocolStack;
    inFields[9] = lpmDbConfig.ipv6MemShare;
    inFields[10] = lpmDbConfig.numOfVirtualRouters;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2],  inFields[3],
                inFields[4], inFields[5], inFields[6],  inFields[7],
                inFields[8], inFields[9], inFields[10]);

    ipMemCfgArrayId++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC lpmDbConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    ipMemCfgArrayId = 0; /* reset on first*/

    /* map input arguments to locals */
    lpmDbId = inArgs[0];

    cmdOsMemSet(ipMemCfgArray, 0, sizeof(ipMemCfgArray));
    lpmDbConfig.memCfgArray = ipMemCfgArray;
    lpmDbConfig.numOfMemCfg = 10;

    /* call cpss api function */
    result = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfig);

    if (result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    return wrCpssExMxPmIpLpmDbConfigGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxPmIpLpmDbDevListAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds list of devices to an existing LPM DB. Adding a
*       device will invoke hot sync to synchronize the newly added deviced with
*       the current prefixes in the LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - the LPM DB id (range 32 bits)
*       numOfDevs         - the number of device ids in the array ( > 0 )
*       devListArray      - array of device ids to add to the LPM DB
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_FOUND      - if the LPM DB id is not found
*       GT_BAD_PARAM      - on illegal parameter
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*       GT_FAIL           - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32 lpmDbId;
static GT_U32 numOfDevs;
static GT_U32 devListArrIndex;
static GT_U8 devListArray[10];

static CMD_STATUS wrCpssExMxPmIpLpmDbDevListAddFirst
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
    lpmDbId = inArgs[0];
    numOfDevs = inArgs[1];

    devListArrIndex = 0; /* reset on first*/
    devListArray[devListArrIndex] = (GT_U8)inFields[0];

    devListArrIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbDevListAddNext
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
    devListArray[devListArrIndex] = (GT_U8)inFields[0];

    devListArrIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbDevListAddEnd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

   /* call cpss api function */
    result = cpssExMxPmIpLpmDbDevListAdd(lpmDbId, numOfDevs, devListArray);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmDbDevListRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function removes devices from an existing LPM DB. Removing a
*       device will invoke a hot sync removal of the devices.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - the LPM DB id
*       numOfDevs         - the number of device ids in the array ( > 0 )
*       devListArray      - array of device ids to remove from the LPM DB
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_FOUND      - if the LPM DB id is not found
*       GT_BAD_PARAM      - on illegal parameter
*       GT_BAD_PTR        - on NULL pointer
*       GT_HW_ERROR       - on hardware error
*       GT_FAIL           - on error
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbDevListRemove
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 numOfDevs;
    GT_U8 devListArray[1];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    numOfDevs = inArgs[1];
    devListArray[0] = (GT_U8)inFields[0];

    /* call cpss api function */
    result = cpssExMxPmIpLpmDbDevListRemove(lpmDbId, numOfDevs, devListArray);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmDbDevListGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function retrieves the list of devices in an existing LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - the LPM DB id.
*       numOfDevsPtr      - points to the size of devListArray
*
* OUTPUTS:
*       numOfDevsPtr      - points to the number of devices retreived
*       devListArray      - array of device ids in the LPM DB
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_FOUND      - if the LPM DB id is not found.
*       GT_BAD_PARAM      - on wrong parameters
*       GT_BAD_SIZE       - in case not enough memory was allocated to device list
*
* COMMENTS:
*       The application should allocate memory for the device list array.
*       If the function returns GT_OK, then number of devices holds
*       the number of devices the function filled.
*       If the function returns GT_BAD_SIZE, then the memory allocated by the
*       application to the device list is not enough. In this case the
*       number of devices will hold the size of array needed.
*
*******************************************************************************/
static GT_U32 numOfDevs;

static CMD_STATUS wrCpssExMxPmIpLpmDbDevListGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    numOfDevs = inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpLpmDbDevListGet(lpmDbId, &numOfDevs, devListArray);

    if (result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    devListArrIndex = 0;

    inFields[0] = (GT_U8)devListArray[devListArrIndex];

    devListArrIndex++;

    /* pack and output table fields */
    fieldOutput("%d", inFields[0]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbDevListGetNext
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

    if (devListArrIndex >= numOfDevs)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    /* map input arguments to locals */
    inFields[0] = (GT_U8)devListArray[devListArrIndex];

    devListArrIndex++;

    /* pack and output table fields */
    fieldOutput("%d", inFields[0]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - the LPM DB id.
*       vrId              - The virtual's router ID.
*       vrConfigPtr       - Configuration of the virtual router
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_NOT_FOUND      - if the LPM DB id is not found
*       GT_BAD_PTR        - on NULL pointer
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_ALREADY_EXIST  - if the vr id is already used
*       GT_OUT_OF_PP_MEM  - if failed to allocate TCAM memory.
*       GT_BAD_STATE      - if the existing VR is not empty.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2IpLpmVirtualRouterAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC vrConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    vrId = inFields[0];
    vrConfig.supportUcIpv4 = (GT_BOOL)inFields[1];
    vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = inFields[2];
    vrConfig.defaultUcIpv4RouteEntry.blockSize = inFields[3];
    vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[4];
    vrConfig.supportMcIpv4 = (GT_BOOL)inFields[5];
    vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = inFields[6];
    vrConfig.defaultMcIpv4RouteEntry.blockSize = inFields[7];
    vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[8];
    vrConfig.supportUcIpv6 = (GT_BOOL)inFields[9];
    vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = inFields[10];
    vrConfig.defaultUcIpv6RouteEntry.blockSize = inFields[11];
    vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[12];
    vrConfig.supportMcIpv6 = (GT_BOOL)inFields[13];
    vrConfig.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr = inFields[14];
    vrConfig.defaultMcIpv6RouteEntry.blockSize = inFields[15];
    vrConfig.defaultMcIpv6RouteEntry.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[16];
    vrConfig.defaultMcIpv6RuleIndex = inFields[17];
    vrConfig.defaultMcIpv6PclId = inFields[18];


    /* call cpss api function */
    result = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - the LPM DB id.
*       vrId              - The virtual's router ID.
*       vrConfigPtr       - Configuration of the virtual router
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_NOT_FOUND      - if the LPM DB id is not found
*       GT_BAD_PTR        - on NULL pointer
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_ALREADY_EXIST  - if the vr id is already used
*       GT_OUT_OF_PP_MEM  - if failed to allocate TCAM memory.
*       GT_BAD_STATE      - if the existing VR is not empty.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmVirtualRouterAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC vrConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    vrId = inFields[0];
    vrConfig.supportUcIpv4 = (GT_BOOL)inFields[1];
    vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr = inFields[2];
    vrConfig.defaultUcIpv4RouteEntry.blockSize = inFields[3];
    vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[4];
    vrConfig.supportMcIpv4 = (GT_BOOL)inFields[5];
    vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr = inFields[6];
    vrConfig.defaultMcIpv4RouteEntry.blockSize = inFields[7];
    vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[8];
    vrConfig.supportUcIpv6 = (GT_BOOL)inFields[9];
    vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr = inFields[10];
    vrConfig.defaultUcIpv6RouteEntry.blockSize = inFields[11];
    vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[12];

    /*currently this is hard coded - need new wrapper*/
    vrConfig.supportMcIpv6 = GT_FALSE;

    /* call cpss api function */
    result = cpssExMxPmIpLpmVirtualRouterAdd(lpmDbId, vrId, &vrConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmVirtualRouterDelete
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function deletes a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - the LPM DB id.
*       vrId              - The virtual's router ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_NOT_FOUND      - if the LPM DB id or vr id does not found
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM  - if failed to allocate TCAM memory.
*       GT_BAD_STATE      - if the existing VR is not empty.
*
* COMMENTS:
*       All prefixes must be previously deleted.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmVirtualRouterDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpLpmVirtualRouterDelete(lpmDbId, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmVirtualRouterConfigGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets configuration of a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - the LPM DB id.
*       vrId              - The virtual's router ID.
*
* OUTPUTS:
*       vrConfigPtr       - Configuration of the virtual router
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_NOT_FOUND      - if the LPM DB id or vr id does not found
*       GT_BAD_PTR        - on NULL pointer
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM  - if failed to allocate TCAM memory.
*       GT_BAD_STATE      - if the existing VR is not empty.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2IpLpmVirtualRouterConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U32 lpmDbId;
    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC vrConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&vrConfig,0,sizeof(CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC));

    lpmDbId = inArgs[0];
    for( ; vrId < numOfVirtualRouters ; vrId++)
    {
        /* call cpss api function */
        result = cpssExMxPmIpLpmVirtualRouterConfigGet(lpmDbId, vrId, &vrConfig);

        if (result == GT_NOT_FOUND)
            continue;
        else
            break;
    }

    if ((vrId == numOfVirtualRouters) && ((result == GT_NOT_FOUND)||(result == GT_OK)))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }


    inFields[0] = vrId;
    inFields[1] = vrConfig.supportUcIpv4;
    inFields[2] = vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr;
    inFields[3] = vrConfig.defaultUcIpv4RouteEntry.blockSize;
    inFields[4] = vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod;
    inFields[5] = vrConfig.supportMcIpv4;
    inFields[6] = vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr;
    inFields[7] = vrConfig.defaultMcIpv4RouteEntry.blockSize;
    inFields[8] = vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod;
    inFields[9] = vrConfig.supportUcIpv6;
    inFields[10] = vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr;
    inFields[11] = vrConfig.defaultUcIpv6RouteEntry.blockSize;
    inFields[12] = vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod;
    inFields[13] = vrConfig.supportMcIpv6;
    inFields[14] = vrConfig.defaultMcIpv6RouteEntry.routeEntryBaseMemAddr;
    inFields[15] = vrConfig.defaultMcIpv6RouteEntry.blockSize;
    inFields[16] = vrConfig.defaultMcIpv6RouteEntry.routeEntryMethod;
    inFields[17] = vrConfig.defaultMcIpv6RuleIndex;
    inFields[18] = vrConfig.defaultMcIpv6PclId;
    vrId++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2],  inFields[3],
                inFields[4], inFields[5], inFields[6],  inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2IpLpmVirtualRouterConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC lpmDbConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];

    cmdOsMemSet(ipMemCfgArray, 0, sizeof(ipMemCfgArray));
    lpmDbConfig.memCfgArray = ipMemCfgArray;
    lpmDbConfig.numOfMemCfg = 10;

    result = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfig);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);

        return CMD_OK;
    }
    numOfVirtualRouters = lpmDbConfig.numOfVirtualRouters;
    vrId = 0;

    return wrCpssExMxPm2IpLpmVirtualRouterConfigGet(inArgs,inFields,numFields,outArgs);
}


/*******************************************************************************
* cpssExMxPmIpLpmVirtualRouterConfigGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets configuration of a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - the LPM DB id.
*       vrId              - The virtual's router ID.
*
* OUTPUTS:
*       vrConfigPtr       - Configuration of the virtual router
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_NOT_FOUND      - if the LPM DB id or vr id does not found
*       GT_BAD_PTR        - on NULL pointer
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM  - if failed to allocate TCAM memory.
*       GT_BAD_STATE      - if the existing VR is not empty.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmVirtualRouterConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U32 lpmDbId;
    CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC vrConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&vrConfig,0,sizeof(CPSS_EXMXPM_IP_LPM_VR_CONFIG_STC));

    lpmDbId = inArgs[0];
    for( ; vrId < numOfVirtualRouters ; vrId++)
    {
        /* call cpss api function */
        result = cpssExMxPmIpLpmVirtualRouterConfigGet(lpmDbId, vrId, &vrConfig);

        if (result == GT_NOT_FOUND)
            continue;
        else
            break;
    }

    if ((vrId == numOfVirtualRouters) && ((result == GT_NOT_FOUND)||(result == GT_OK)))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = vrId;
    inFields[1] = vrConfig.supportUcIpv4;
    inFields[2] = vrConfig.defaultUcIpv4RouteEntry.routeEntryBaseMemAddr;
    inFields[3] = vrConfig.defaultUcIpv4RouteEntry.blockSize;
    inFields[4] = vrConfig.defaultUcIpv4RouteEntry.routeEntryMethod;
    inFields[5] = vrConfig.supportMcIpv4;
    inFields[6] = vrConfig.defaultMcIpv4RouteEntry.routeEntryBaseMemAddr;
    inFields[7] = vrConfig.defaultMcIpv4RouteEntry.blockSize;
    inFields[8] = vrConfig.defaultMcIpv4RouteEntry.routeEntryMethod;
    inFields[9] = vrConfig.supportUcIpv6;
    inFields[10] = vrConfig.defaultUcIpv6RouteEntry.routeEntryBaseMemAddr;
    inFields[11] = vrConfig.defaultUcIpv6RouteEntry.blockSize;
    inFields[12] = vrConfig.defaultUcIpv6RouteEntry.routeEntryMethod;

    vrId++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2],  inFields[3],
                inFields[4], inFields[5], inFields[6],  inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmVirtualRouterConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC lpmDbConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];

    cmdOsMemSet(ipMemCfgArray, 0, sizeof(ipMemCfgArray));
    lpmDbConfig.memCfgArray = ipMemCfgArray;
    lpmDbConfig.numOfMemCfg = 10;

    result = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfig);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);

        return CMD_OK;
    }
    numOfVirtualRouters = lpmDbConfig.numOfVirtualRouters;
    vrId = 0;

    return wrCpssExMxPmIpLpmVirtualRouterConfigGet(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxPmIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds a new IPv4 prefix to a Virtual Router in a
*       specific LPM DB or overrides an existing existing IPv4 prefix.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router id.
*       ipAddr            - The destination IP address of this prefix.
*       prefixLen         - The number of bits that are actual valid in the ipAddr.
*       nextHopPointer    - The next hop pointer to set for this prefix.
*       override          - if GT_TRUE, override existing entry if it exists and
*                           return GT_NOT_FOUND if it doesn't exist
*                           if GT_FALSE, add the prefix if it doesn't exist and
*                           return GT_ALREADY_EXIST if the prefix exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_ERROR          - If the vrId was not created yet, or
*       GT_NOT_FOUND      - prefix was not found when override is GT_TRUE
*       GT_ALREADY_EXIST  - prefix already exist when override is GT_FALSE
*       GT_BAD_PTR        - on NULL pointer
*       GT_OUT_OF_CPU_MEM - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM  - If failed to allocate PP memory, or
*       GT_FAIL           - otherwise.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipAddr;
    GT_U32 prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC nextHopPointer;
    GT_BOOL override;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    vrId = inFields[0];
    galtisIpAddr(&ipAddr, (GT_U8*)inFields[1]);
    prefixLen = inFields[2];
    nextHopPointer.routeEntryBaseMemAddr = inFields[3];
    nextHopPointer.blockSize = inFields[4];
    nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
    override = (GT_BOOL)inFields[6];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4UcPrefixAdd(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer, override);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4UcPrefixBulkAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Creates a new or override an existing bulk of IPv4 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId               - The LPM DB id.
*       ipv4PrefixArrayLen    - Length of UC prefix array.
*       ipv4PrefixArray       - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if all prefixes were successfully added
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - if adding one or more prefixes failed; the
*                               array will contain return status for each prefix
*
* COMMENTS:
*       None.
*
*******************************************************************************/

static CPSS_EXMXPM_IP_LPM_IPV4_UC_ADD_PREFIX_STC *ipv4PrefixArrayPtr = NULL;
static GT_U32 ipv4PrefixArrayAllocSize = 0;
static GT_U32 ipv4PrefixArrayInd;

static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkAddFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lpmDbId = 0;
    GT_U32 ipv4PrefixArrayLen = 0;
    ipv4PrefixArrayInd = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv4PrefixArrayLen = inArgs[1];

    /* free previously allocated bulk array (if still allocated) */
    if(ipv4PrefixArrayPtr != NULL)
    {
        cmdOsFree(ipv4PrefixArrayPtr);
        ipv4PrefixArrayPtr = NULL;
        ipv4PrefixArrayAllocSize = 0;
    }

    /* check parameter send by user */
    if (ipv4PrefixArrayLen == 0)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    /* allocate memory to hold bulk entries */
    ipv4PrefixArrayPtr = (CPSS_EXMXPM_IP_LPM_IPV4_UC_ADD_PREFIX_STC*)cmdOsMalloc(ipv4PrefixArrayLen * (sizeof(CPSS_EXMXPM_IP_LPM_IPV4_UC_ADD_PREFIX_STC)));
    if (ipv4PrefixArrayPtr == NULL)
    {
        galtisOutput(outArgs, GT_OUT_OF_CPU_MEM, "");
        return CMD_OK;
    }
    ipv4PrefixArrayAllocSize = ipv4PrefixArrayLen;

    /* map input arguments to locals */
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].vrId = inFields[0];
    galtisIpAddr(&ipv4PrefixArrayPtr[ipv4PrefixArrayInd].ipAddr, (GT_U8*)inFields[1]);
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].prefixLen = inFields[2];
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.routeEntryBaseMemAddr = inFields[3];
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.blockSize = inFields[4];
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].override = (GT_BOOL)inFields[6];
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].returnStatus = (GT_STATUS)inFields[7];

    /* prepare for next entry */
    ipv4PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkAddNext
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

    /* verify there are enough lines in galtis */
    if (ipv4PrefixArrayInd >= ipv4PrefixArrayAllocSize)
    {
        /* free memory allocated to bulk array as add failed */
        if (ipv4PrefixArrayPtr != NULL)
        {
            cmdOsFree(ipv4PrefixArrayPtr);
            ipv4PrefixArrayPtr = NULL;
        }
        ipv4PrefixArrayAllocSize = 0;

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: number of entries is bigger than ipv4PrefixArrayLen.\n");
        return CMD_OK;
    }

    /* map input arguments to locals */
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].vrId = inFields[0];
    galtisIpAddr(&ipv4PrefixArrayPtr[ipv4PrefixArrayInd].ipAddr, (GT_U8*)inFields[1]);
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].prefixLen = inFields[2];
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.routeEntryBaseMemAddr = inFields[3];
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.blockSize = inFields[4];
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].override = (GT_BOOL)inFields[6];
    ipv4PrefixArrayPtr[ipv4PrefixArrayInd].returnStatus = (GT_STATUS)inFields[7];

    /* prepare for next entry */
    ipv4PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkEndAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId = 0;
    GT_U32 ipv4PrefixArrayLen=0;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv4PrefixArrayLen = inArgs[1];

    if(ipv4PrefixArrayLen != ipv4PrefixArrayInd)
    {
        /* free memory allocated to bulk array as add failed */
        if (ipv4PrefixArrayPtr != NULL)
        {
            cmdOsFree(ipv4PrefixArrayPtr);
            ipv4PrefixArrayPtr = NULL;
        }
        ipv4PrefixArrayAllocSize = 0;

        /* pack output arguments to */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: number of entries is smaller than ipv4PrefixArrayLen.\n");
        return CMD_OK;
    }
    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4UcPrefixBulkAdd(lpmDbId, ipv4PrefixArrayLen, ipv4PrefixArrayPtr);

    /* note that the bulk memory array is not freed as it holds the
       return code for each element insertion and will be used by
       the refresh to get the result of each bulk entry insertion   */

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkAddGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lpmDbId;
    GT_U32 ipv4PrefixArrayLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* note that the refresh is only used to display the result of
       each of the prefixes added in the add bulk. therefore it
       only shows the result of the last bulk add */

    /* note that in the refresh, the ipv4PrefixArrayLen (arg[1]) is ignored
       as refresh is used to retrieve the result of each of the prefixes
       add in the aadd bulk. therefore the number of prefixes refreshed is
       set according to the number of prefixes added in the add bulk */

    /* verify that there was a previouly add bulk operation */
    if ((ipv4PrefixArrayPtr == NULL) || (ipv4PrefixArrayAllocSize == 0))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_EMPTY, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv4PrefixArrayLen = inArgs[1];

    ipv4PrefixArrayInd = 0;

    inFields[0] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].vrId;
    inFields[2] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].prefixLen;
    inFields[3] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.routeEntryBaseMemAddr;
    inFields[4] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.blockSize;
    inFields[5] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.routeEntryMethod;
    inFields[6] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].override;
    inFields[7] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].returnStatus;

    /* pack and output table fields */
    fieldOutput("%d%4b%d%d%d%d%d%d",
                inFields[0],  ipv4PrefixArrayPtr[ipv4PrefixArrayInd].ipAddr.arIP,
                inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7]);


    ipv4PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkAddGetNext
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

    /* note that in the refresh, the ipv4PrefixArrayLen (arg[1]) is ignored
       as refresh is used to retrieve the result of each of the prefixes
       add in the aadd bulk. therefore the number of prefixes refreshed is
       set according to the number of prefixes added in the add bulk */

    if (ipv4PrefixArrayInd == ipv4PrefixArrayAllocSize)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].vrId;
    inFields[2] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].prefixLen;
    inFields[3] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.routeEntryBaseMemAddr;
    inFields[4] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.blockSize;
    inFields[5] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].nextHopPointer.routeEntryMethod;
    inFields[6] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].override;
    inFields[7] = ipv4PrefixArrayPtr[ipv4PrefixArrayInd].returnStatus;


    /* pack and output table fields */
    fieldOutput("%d%4b%d%d%d%d%d%d",
                inFields[0],  ipv4PrefixArrayPtr[ipv4PrefixArrayInd].ipAddr.arIP,
                inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7]);


    ipv4PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4UcPrefixBulkDelete
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*       Deletes an existing bulk of Ipv4 prefixes in a Virtual Router for the
*       specified LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId               - The LPM DB id.
*       ipv4PrefixArrayLen    - Length of UC prefix array.
*       ipv4PrefixArray       - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if all prefixes were successfully deleted
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - if deleting one or more prefixes failed; the
*                               array will contain return status for each prefix
*
* COMMENTS:
*       None.
*
*******************************************************************************/

static CPSS_EXMXPM_IP_LPM_IPV4_UC_DELETE_PREFIX_STCT* ipv4PrefixDeleteArray;
static GT_U32 ipv4PrefixArrayDeleteAllocSize = 0;

static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkDeleteFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lpmDbId=0;
    GT_U32 ipv4PrefixArrayLen=0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv4PrefixArrayLen = inArgs[1];
    ipv4PrefixArrayInd = 0;

    if (ipv4PrefixArrayLen == 0)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    if(ipv4PrefixArrayLen > ipv4PrefixArrayDeleteAllocSize)
    {
        if(ipv4PrefixArrayDeleteAllocSize != 0)
           cmdOsFree(ipv4PrefixDeleteArray);
        ipv4PrefixDeleteArray = (CPSS_EXMXPM_IP_LPM_IPV4_UC_DELETE_PREFIX_STCT*)cmdOsMalloc(ipv4PrefixArrayLen * (sizeof(CPSS_EXMXPM_IP_LPM_IPV4_UC_DELETE_PREFIX_STCT)));
        ipv4PrefixArrayDeleteAllocSize = ipv4PrefixArrayLen;
    }

    ipv4PrefixDeleteArray[ipv4PrefixArrayInd].vrId = inFields[0];
    galtisIpAddr(&ipv4PrefixDeleteArray[ipv4PrefixArrayInd].ipAddr, (GT_U8*)inFields[1]);
    ipv4PrefixDeleteArray[ipv4PrefixArrayInd].prefixLen = inFields[2];
    ipv4PrefixDeleteArray[ipv4PrefixArrayInd].returnStatus = (GT_STATUS)inFields[3];

    ipv4PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkDeleteNext
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

    if (ipv4PrefixArrayInd == ipv4PrefixArrayDeleteAllocSize)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: number of entries is bigger than ipv4PrefixArrayLen.\n");
        return CMD_OK;
    }

    ipv4PrefixDeleteArray[ipv4PrefixArrayInd].vrId = inFields[0];
    galtisIpAddr(&ipv4PrefixDeleteArray[ipv4PrefixArrayInd].ipAddr, (GT_U8*)inFields[1]);
    ipv4PrefixDeleteArray[ipv4PrefixArrayInd].prefixLen = inFields[2];
    ipv4PrefixDeleteArray[ipv4PrefixArrayInd].returnStatus = (GT_STATUS)inFields[3];

    ipv4PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkDeleteEnd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId=0;
    GT_U32 ipv4PrefixArrayLen=0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv4PrefixArrayLen = inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4UcPrefixBulkDelete(lpmDbId, ipv4PrefixArrayLen, ipv4PrefixDeleteArray);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkDeleteGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lpmDbId;
    GT_U32 ipv4PrefixArrayLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv4PrefixArrayLen = inArgs[1];

    ipv4PrefixArrayInd = 0;

    if (ipv4PrefixArrayDeleteAllocSize == 0)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_NOT_FOUND, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = ipv4PrefixDeleteArray[ipv4PrefixArrayInd].vrId;
    inFields[2] = ipv4PrefixDeleteArray[ipv4PrefixArrayInd].prefixLen;
    inFields[3] = ipv4PrefixDeleteArray[ipv4PrefixArrayInd].returnStatus;

    /* pack and output table fields */
    fieldOutput("%d%4b%d%d",
                inFields[0],  ipv4PrefixDeleteArray[ipv4PrefixArrayInd].ipAddr.arIP,
                inFields[2],  inFields[3]);

    ipv4PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkDeleteGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 ipv4PrefixArrayLen=0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    ipv4PrefixArrayLen = inArgs[1];

    if (ipv4PrefixArrayInd == ipv4PrefixArrayLen)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = ipv4PrefixDeleteArray[ipv4PrefixArrayInd].vrId;
    inFields[2] = ipv4PrefixDeleteArray[ipv4PrefixArrayInd].prefixLen;
    inFields[3] = ipv4PrefixDeleteArray[ipv4PrefixArrayInd].returnStatus;


    /* pack and output table fields */
    fieldOutput("%d%4b%d%d",
                inFields[0],  ipv4PrefixDeleteArray[ipv4PrefixArrayInd].ipAddr.arIP,
                inFields[2],  inFields[3]);


    ipv4PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4UcPrefixBulkOperation
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Applies various prefix operations in single API call.
*       Refer to CPSS_EXMXPM_IP_LPM_IPV4_UC_PREFIX_OPERATION_STC to the possible
*       operations.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       lpmDbId                   - The LPM DB id
*       prefixOperationArrayLen   - Length of prefix array (positive number)
*       prefixOperationArrayPtr   - The prefix array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - if all prefixes were successfully added
*       GT_BAD_PTR                - on NULL pointer
*       GT_FAIL                   - if adding one or more prefixes failed; the
*                                   array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       Operations will be executed according to array order and stop after
*       first failure.
*
*******************************************************************************/
static CPSS_EXMXPM_IP_LPM_IPV4_UC_PREFIX_OPERATION_STC  *ipv4PrefixOperationArray = NULL;
static GT_U32                                           ipv4PrefixOperationArrayLen = 0;
static GT_U32                                           ipv4PrefixOperationArrayIdx = 0;

static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkOperationSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_EXMXPM_IP_LPM_IPV4_UC_PREFIX_OPERATION_STC *tmpPrefixOperation = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* reset bulk operations array */
    if (ipv4PrefixOperationArray != NULL)
        cmdOsFree(ipv4PrefixOperationArray);
    ipv4PrefixOperationArrayLen = 0;
    ipv4PrefixOperationArrayIdx = 0;

    /* allocate memory for this operation */
    ipv4PrefixOperationArrayLen = 1;
    ipv4PrefixOperationArrayIdx = 0;
    ipv4PrefixOperationArray = (CPSS_EXMXPM_IP_LPM_IPV4_UC_PREFIX_OPERATION_STC*)cmdOsRealloc(ipv4PrefixOperationArray,
                               sizeof(CPSS_EXMXPM_IP_LPM_IPV4_UC_PREFIX_OPERATION_STC) * ipv4PrefixOperationArrayLen);
    if (ipv4PrefixOperationArray == NULL)
    {
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Memory allocation failed.\n");
        return CMD_AGENT_ERROR;
    }

    tmpPrefixOperation = &ipv4PrefixOperationArray[ipv4PrefixOperationArrayIdx];

    /* map current operation to the new entry */
    switch (inFields[0])
    {
    case 0:
        tmpPrefixOperation->type = CPSS_BULK_OPERATION_ADD_E;
        tmpPrefixOperation->data.addData.returnStatus = GT_FAIL; /* inFields[1] */
        tmpPrefixOperation->data.addData.vrId = inFields[2];
        galtisIpAddr(&(tmpPrefixOperation->data.addData.ipAddr), (GT_U8*)inFields[3]);
        tmpPrefixOperation->data.addData.prefixLen = inFields[4];
        tmpPrefixOperation->data.addData.nextHopPointer.routeEntryBaseMemAddr = inFields[5];
        tmpPrefixOperation->data.addData.nextHopPointer.blockSize = inFields[6];
        tmpPrefixOperation->data.addData.nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[7];
        tmpPrefixOperation->data.addData.override = (GT_BOOL)inFields[8];
        break;

    case 1:
        tmpPrefixOperation->type = CPSS_BULK_OPERATION_DELETE_E;
        tmpPrefixOperation->data.deleteData.returnStatus = GT_FAIL; /* inFields[1] */
        tmpPrefixOperation->data.deleteData.vrId = inFields[2];
        galtisIpAddr(&(tmpPrefixOperation->data.deleteData.ipAddr), (GT_U8*)inFields[3]);
        tmpPrefixOperation->data.deleteData.prefixLen = inFields[4];
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Unknown operation.\n");
        return CMD_AGENT_ERROR;
    }

    ipv4PrefixOperationArrayIdx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkOperationSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_EXMXPM_IP_LPM_IPV4_UC_PREFIX_OPERATION_STC *tmpPrefixOperation = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* reallocate memory for this operation */
    ipv4PrefixOperationArrayLen++;
    ipv4PrefixOperationArray = (CPSS_EXMXPM_IP_LPM_IPV4_UC_PREFIX_OPERATION_STC*)cmdOsRealloc(ipv4PrefixOperationArray,
                               sizeof(CPSS_EXMXPM_IP_LPM_IPV4_UC_PREFIX_OPERATION_STC) * ipv4PrefixOperationArrayLen);
    if (ipv4PrefixOperationArray == NULL)
    {
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Memory allocation failed.\n");
        return CMD_AGENT_ERROR;
    }

    tmpPrefixOperation = &ipv4PrefixOperationArray[ipv4PrefixOperationArrayIdx];

    /* map current operation to the new entry */
    switch (inFields[0])
    {
    case 0:
        tmpPrefixOperation->type = CPSS_BULK_OPERATION_ADD_E;
        tmpPrefixOperation->data.addData.returnStatus = GT_FAIL; /* inFields[1] */
        tmpPrefixOperation->data.addData.vrId = inFields[2];
        galtisIpAddr(&(tmpPrefixOperation->data.addData.ipAddr), (GT_U8*)inFields[3]);
        tmpPrefixOperation->data.addData.prefixLen = inFields[4];
        tmpPrefixOperation->data.addData.nextHopPointer.routeEntryBaseMemAddr = inFields[5];
        tmpPrefixOperation->data.addData.nextHopPointer.blockSize = inFields[6];
        tmpPrefixOperation->data.addData.nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[7];
        tmpPrefixOperation->data.addData.override = (GT_BOOL)inFields[8];
        break;

    case 1:
        tmpPrefixOperation->type = CPSS_BULK_OPERATION_DELETE_E;
        tmpPrefixOperation->data.deleteData.returnStatus = GT_FAIL; /* inFields[1] */
        tmpPrefixOperation->data.deleteData.vrId = inFields[2];
        galtisIpAddr(&(tmpPrefixOperation->data.deleteData.ipAddr), (GT_U8*)inFields[3]);
        tmpPrefixOperation->data.deleteData.prefixLen = inFields[4];
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Unknown operation.\n");
        return CMD_AGENT_ERROR;
    }

    ipv4PrefixOperationArrayIdx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkOperationSetEnd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U32      lpmDbId = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4UcPrefixBulkOperation(lpmDbId,ipv4PrefixOperationArrayLen,ipv4PrefixOperationArray);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkOperationGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_EXMXPM_IP_LPM_IPV4_UC_PREFIX_OPERATION_STC *tmpPrefixOperation = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Note that the refresh is only used to display the results of
       each of the operations performed in the last bulk operation */

    ipv4PrefixOperationArrayIdx = 0;

    if ((ipv4PrefixOperationArrayLen == 0) || (ipv4PrefixOperationArray == NULL))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_NOT_FOUND, "%d", -1);
        return CMD_OK;
    }

    tmpPrefixOperation = &ipv4PrefixOperationArray[ipv4PrefixOperationArrayIdx];

    /* map current operation to the new entry */
    switch (tmpPrefixOperation->type/*inFields[0]*/)
    {
    case CPSS_BULK_OPERATION_ADD_E:
        inFields[0] = 0; /*tmpPrefixOperation->type */
        inFields[1] = tmpPrefixOperation->data.addData.returnStatus;
        inFields[2] = tmpPrefixOperation->data.addData.vrId;
        /*inFields[3] = tmpPrefixOperation->data.addData.ipAddr */
        inFields[4] = tmpPrefixOperation->data.addData.prefixLen;
        inFields[5] = tmpPrefixOperation->data.addData.nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = tmpPrefixOperation->data.addData.nextHopPointer.blockSize;
        inFields[7] = tmpPrefixOperation->data.addData.nextHopPointer.routeEntryMethod;
        inFields[8] = tmpPrefixOperation->data.addData.override;
        break;

    case 1:
        inFields[0] = 1; /* tmpPrefixOperation->type */
        inFields[1] = tmpPrefixOperation->data.deleteData.returnStatus;
        inFields[2] = tmpPrefixOperation->data.deleteData.vrId;
        /*inFields[3] = tmpPrefixOperation->data.deleteData.ipAddr */
        inFields[4] = tmpPrefixOperation->data.deleteData.prefixLen;
        /* next fields has no meaning for delete and will be set to 0 */
        inFields[5] = 0; /* nextHopPointer.routeEntryBaseMemAddr */
        inFields[6] = 0; /* nextHopPointer.blockSize */
        inFields[7] = 0; /* nextHopPointer.routeEntryMethod */
        inFields[8] = 0; /* override */
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Unknown operation.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%4b%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2],
                tmpPrefixOperation->data.deleteData.ipAddr.arIP,
                inFields[4], inFields[5], inFields[6],
                inFields[7], inFields[8]);

    ipv4PrefixOperationArrayIdx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixBulkOperationGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_EXMXPM_IP_LPM_IPV4_UC_PREFIX_OPERATION_STC *tmpPrefixOperation = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Note that the refresh is only used to display the results of
       each of the operations performed in the last bulk operation */

    if (ipv4PrefixOperationArrayIdx == ipv4PrefixOperationArrayLen)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    tmpPrefixOperation = &ipv4PrefixOperationArray[ipv4PrefixOperationArrayIdx];

    /* map current operation to the new entry */
    switch (tmpPrefixOperation->type/*inFields[0]*/)
    {
    case CPSS_BULK_OPERATION_ADD_E:
        inFields[0] = 0; /*tmpPrefixOperation->type */
        inFields[1] = tmpPrefixOperation->data.addData.returnStatus;
        inFields[2] = tmpPrefixOperation->data.addData.vrId;
        /*inFields[3] = tmpPrefixOperation->data.addData.ipAddr */
        inFields[4] = tmpPrefixOperation->data.addData.prefixLen;
        inFields[5] = tmpPrefixOperation->data.addData.nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = tmpPrefixOperation->data.addData.nextHopPointer.blockSize;
        inFields[7] = tmpPrefixOperation->data.addData.nextHopPointer.routeEntryMethod;
        inFields[8] = tmpPrefixOperation->data.addData.override;
        break;

    case 1:
        inFields[0] = 1; /* tmpPrefixOperation->type */
        inFields[1] = tmpPrefixOperation->data.deleteData.returnStatus;
        inFields[2] = tmpPrefixOperation->data.deleteData.vrId;
        /*inFields[3] = tmpPrefixOperation->data.deleteData.ipAddr */
        inFields[4] = tmpPrefixOperation->data.deleteData.prefixLen;
        /* next fields has no meaning for delete and will be set to 0 */
        inFields[5] = 0; /* nextHopPointer.routeEntryBaseMemAddr */
        inFields[6] = 0; /* nextHopPointer.blockSize */
        inFields[7] = 0; /* nextHopPointer.routeEntryMethod */
        inFields[8] = 0; /* override */
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Unknown operation.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%4b%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2],
                tmpPrefixOperation->data.deleteData.ipAddr.arIP,
                inFields[4], inFields[5], inFields[6],
                inFields[7], inFields[8]);

    ipv4PrefixOperationArrayIdx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4UcPrefixDelete
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Deletes an existing IPv4 prefix in a Virtual Router for the specified
*       LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success, or
*       GT_ERROR        - If the vrId was not created yet, or
*       GT_NO_SUCH      - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipAddr;
    GT_U32 prefixLen;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    vrId = inFields[0];
    galtisIpAddr(&ipAddr, (GT_U8*)inFields[1]);
    prefixLen = inFields[2];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4UcPrefixDelete(lpmDbId, vrId, ipAddr, prefixLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmIpLpmIpv4UcPrefixesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Flushes the unicast IPv4 Routing table and stays with the default prefix
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId       - The LPM DB id.
*       vrId          - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL       - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixesFlush
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4UcPrefixesFlush(lpmDbId, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/******************************************************************************/
static  GT_IPADDR ipAddr;
static  GT_U32    prefixLen;

/*******************************************************************************
* cpssExMxPmIpLpmIpv4UcPrefixGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function searches for a given ip-uc address, and returns the next
*       hop pointer information bound to it if found.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router id.
*       ipAddr            - The destination IP address to look for.
*       prefixLen         - The number of bits that are actual valid in the
*                           ipAddr.
*
* OUTPUTS:
*       nextHopPointerPtr - If found this is The next hop pointer to for this
*                           prefix.
*
* RETURNS:
*       GT_OK             - if the required entry was found, or
*       GT_BAD_PTR        - on NULL pointer
*       GT_NOT_FOUND      - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC lpmDbConfig;

    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC nextHopPointer;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];

    cmdOsMemSet(ipMemCfgArray, 0, sizeof(ipMemCfgArray));
    lpmDbConfig.memCfgArray = ipMemCfgArray;
    lpmDbConfig.numOfMemCfg = 10;

    result = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfig);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);

        return CMD_OK;
    }
    numOfVirtualRouters = lpmDbConfig.numOfVirtualRouters;

    vrId = 0;
    ipAddr.u32Ip = 0;
    prefixLen = 0;
    isDefaultVrfIdEntry = GT_FALSE;

    for( ; vrId < numOfVirtualRouters ; vrId++)
    {
        /* call cpss api function */
        result = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, prefixLen, &nextHopPointer);

        /* result is GT_NOT_INITIALIZED if the virtual router doesn't exist */
        if (result == GT_NOT_INITIALIZED)
            continue;
        else
        {
            lastVrfIdTreated = vrId;
            break;
        }
    }

    /* all virtual routers were scanned */
    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = vrId;
    inFields[3] = nextHopPointer.routeEntryBaseMemAddr;
    inFields[4] = nextHopPointer.blockSize;
    inFields[5] = nextHopPointer.routeEntryMethod;

    /* inFields[6] is the override field. It has only meaning in the set, but
    we should display it also in the get, else the command wrapper will fail*/
    inFields[6] = 0;

    /* pack and output table fields */
    fieldOutput("%d%4b%d%d%d%d%d",
                inFields[0], ipAddr.arIP, prefixLen,  inFields[3],
                inFields[4],inFields[5],inFields[6]);

    galtisOutput(outArgs, GT_OK, "%f");


    return CMD_OK;

}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns an IP-Unicast prefix with larger (ipAddrPtr,prefixLenPtr)
*       than the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router Id to get the entry from.
*       ipAddrPtr         - The ip address to start the search from.
*       prefixLenPtr      - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr         - The ip address of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopPointerPtr - the next hop pointer associated with the found
*                           ipAddr.
*
* RETURNS:
*       GT_OK             - if the required entry was found, or
*       GT_BAD_PTR        - on NULL pointer
*       GT_NOT_FOUND      - if no more entries are left in the IP table.
*
* COMMENTS:
*       1.  The values of (ipAddrPtr,prefixLenPtr) must be a valid values, it
*           means that they exist in the IP-UC Table, unless this is the first
*           call to this function, then the value of (ipAddrPtr,prefixLenPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv4
*           UC prefix get function.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U32 lpmDbId;

    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC nextHopPointer;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* all virtual routers were scanned */
    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    lpmDbId = inArgs[0];
    cmdOsMemSet(&nextHopPointer, 0, sizeof(nextHopPointer));

    if(vrId != lastVrfIdTreated)
    {
        ipAddr.u32Ip = 0;
        prefixLen = 0;

        for( ; vrId < numOfVirtualRouters ; vrId++)
        {
            /* call cpss api function */
            result = cpssExMxPmIpLpmIpv4UcPrefixGet(lpmDbId, vrId, ipAddr, 0, &nextHopPointer);

            /* result is GT_NOT_INITIALIZED if the virtual router doesn't exist */
            if (result == GT_NOT_INITIALIZED)
                continue;
            else
            {
                lastVrfIdTreated = vrId;
                isDefaultVrfIdEntry = GT_TRUE;
                break;
            }
        }
    }

    /* all virtual routers were scanned */
    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(isDefaultVrfIdEntry == GT_TRUE)
    {
        /* The first default entry was already treated in
           wrCpssExMxPmIpLpmIpv4UcPrefixGetFirst or in previuos call
           to this wrapper                                                   */

        /* show default entry for vrId*/
        inFields[0] = vrId;
        inFields[3] = nextHopPointer.routeEntryBaseMemAddr;
        inFields[4] = nextHopPointer.blockSize;
        inFields[5] = nextHopPointer.routeEntryMethod;

        /* inFields[6] is the override field. It has only meaning in the set, but
           we should display it also in the get, else the command wrapper will fail*/
        inFields[6] = 0;

        /* pack and output table fields */
        fieldOutput("%d%4b%d%d%d%d%d",
                    inFields[0], ipAddr.arIP, prefixLen,  inFields[3],
                    inFields[4],inFields[5],inFields[6]);

        galtisOutput(outArgs, GT_OK, "%f");

        isDefaultVrfIdEntry = GT_FALSE;
    }

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4UcPrefixGetNext(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);

    if(result == GT_NOT_FOUND)
    {
        vrId++;

        return wrCpssExMxPmIpLpmIpv4UcPrefixGetNext(inArgs,inFields,numFields,outArgs);
    }

    inFields[0] = vrId;
    inFields[3] = nextHopPointer.routeEntryBaseMemAddr;
    inFields[4] = nextHopPointer.blockSize;
    inFields[5] = nextHopPointer.routeEntryMethod;

    /* inFields[6] is the override field. It has only meaning in the set, but
    we should display it also in the get, else the command wrapper will fail*/
    inFields[6] = 0;

    /* pack and output table fields */
    fieldOutput("%d%4b%d%d%d%d%d",
                inFields[0], ipAddr.arIP, prefixLen,  inFields[3],
                inFields[4],inFields[5],inFields[6]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4UcLpmGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets a given ip address, and returns the next
*       hop pointer information bounded to the longest prefix match.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router id.
*       ipAddr            - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr      - Points to the number of bits that are actual valid
*                           in the longest match
*       nextHopPointerPtr - The next hop pointer bounded to the longest match
*
* RETURNS:
*       GT_OK             - if the required entry was found, or
*       GT_BAD_PTR        - on NULL pointer
*       GT_NOT_FOUND      - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcLpmGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipAddr;
    GT_U32 prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC nextHopPointer;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    vrId = inArgs[1];
    galtisIpAddr(&ipAddr,(GT_U8*)inArgs[2]);

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4UcLpmGet(lpmDbId, vrId, ipAddr, &prefixLen, &nextHopPointer);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",
                 prefixLen,
                 nextHopPointer.routeEntryBaseMemAddr,
                 nextHopPointer.blockSize,
                 nextHopPointer.routeEntryMethod);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4McEntryAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual private network identifier.
*       ipGroup           - The IP multicast group address.
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefix       - The number of bits that are actual valid in,
*                           the ipSrc.
*       mcRoutePointerPtr - the mc Route pointer to set for the mc entry.
*       override          - if GT_TRUE, override existing entry if it exists and
*                           return GT_NOT_FOUND if it doesn't exist
*                           if GT_FALSE, add the prefix if it doesn't exist and
*                           return GT_ALREADY_EXIST if the prefix exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_ERROR          - if the virtual router does not exist.
*       GT_NOT_FOUND      - prefix was not found when override is GT_TRUE
*       GT_ALREADY_EXIST  - prefix already exist when override is GT_FALSE
*       GT_BAD_PTR        - on NULL pointer
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM  - if failed to allocate PP memory, or
*       GT_FAIL           - otherwise.
*
* COMMENTS:
*       1.  To override the default mc route use ipGroup = 0.
*       2.  If (S,G) MC group is added when (*,G) doesn't exists then implicit
*           (*,G) is added pointing to (*,*) default. Application added (*,G)
*           will override the implicit (*,G).
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4McEntryAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipGroup;
    GT_IPADDR ipSrc;
    GT_U32 ipSrcPrefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC mcRoutePointer;
    GT_BOOL override;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inFields[0];
    galtisIpAddr(&ipGroup, (GT_U8*)inFields[1]);
    galtisIpAddr(&ipSrc, (GT_U8*)inFields[2]);
    ipSrcPrefixLen = inFields[3];
    mcRoutePointer.routeEntryBaseMemAddr = inFields[4];
    mcRoutePointer.blockSize = inFields[5];
    mcRoutePointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[6];
    override = (GT_BOOL)inFields[7];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4McEntryAdd(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen, &mcRoutePointer, override);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4McEntryDelete
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router identifier.
*       ipGroup           - The IP multicast group address.
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefixLen    - The number of bits that are actual valid in,
*                           the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_ERROR          - if the virtual router does not exist, or
*       GT_NOT_FOUND      - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL           - otherwise.
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*       2.  If no (*,G) was added but (S,G) were added, then implicit (*,G)
*           that points to (*,*) is added. If (*,G) is added later, it will
*           replace the implicit (*,G).
*           When deleting (*,G), then if there are still (S,G), an implicit (*,G)
*           pointing to (*,*) will be added.
*           When deleting last (S,G) and the (*,G) was implicitly added, then
*           the (*,G) will be deleted as well.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4McEntryDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipGroup;
    GT_IPADDR ipSrc;
    GT_U32 ipSrcPrefixLen;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inFields[0];
    galtisIpAddr(&ipGroup, (GT_U8*)inFields[1]);
    galtisIpAddr(&ipSrc, (GT_U8*)inFields[2]);
    ipSrcPrefixLen = inFields[3];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4McEntryDelete(lpmDbId, vrId, ipGroup, ipSrc, ipSrcPrefixLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4McEntriesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       flushes the multicast IP Routing table and stays with the default entry
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId       - The LPM DB id.
*       vrId          - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL       - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4McEntriesFlush
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4McEntriesFlush(lpmDbId, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4McEntryGetFirst
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the next muticast (ipSrcPtr,ipGroupPtr) entry, used
*       to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router Id.
*       ipGroupPtr        - The ip Group address to get the next entry for.
*       ipSrcPtr          - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr        - The next ip Group address.
*       ipSrcPtr          - The next ip Source address.
*       ipSrcPrefixLenPtr - ipSrc prefix length.
*       mcRoutePointerPtr - the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK             - if found, or
*       GT_BAD_PTR        - on NULL pointer
*       GT_NOT_FOUND      - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       1.  The values of (ipSrcPtr,ipGroupPtr) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipSrcPtr,ipGroupPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv4
*           MC get function.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4McEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC lpmDbConfig;

    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC mcRoutePointer;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];

    cmdOsMemSet(ipMemCfgArray, 0, sizeof(ipMemCfgArray));
    lpmDbConfig.memCfgArray = ipMemCfgArray;
    lpmDbConfig.numOfMemCfg = 10;

    result = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfig);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);

        return CMD_OK;
    }
    numOfVirtualRouters = lpmDbConfig.numOfVirtualRouters;

    vrId = 0;
    ipv4Group.u32Ip = 0;
    ipv4Src.u32Ip = 0;
    ipv4SrcPrefixLen = 0;
    isDefaultVrfIdEntry = GT_FALSE;

    for( ; vrId < numOfVirtualRouters ; vrId++)
    {
        /* call cpss api function */
        result = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipv4Group, ipv4Src, ipv4SrcPrefixLen, &mcRoutePointer);

        /* result is GT_NOT_INITIALIZED if the virtual router doesn't exist */
        if (result == GT_NOT_INITIALIZED)
            continue;
        else
        {
            lastVrfIdTreated = vrId;
            break;
        }
    }

    /* all virtual routers were scanned */
    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }


    inFields[0] = vrId;
    inFields[3] = ipv4SrcPrefixLen;
    inFields[4] = mcRoutePointer.routeEntryBaseMemAddr;
    inFields[5] = mcRoutePointer.blockSize;
    inFields[6] = mcRoutePointer.routeEntryMethod;
    /* inFields[7] is the override field. It has only meaning in the set, but
    we should display it also in the get, else the command wrapper will fail*/
    inFields[7] = 0;

    /* pack and output table fields */
    fieldOutput("%d%4b%4b%d%d%d%d%d",
                inFields[0], ipv4Group.arIP, ipv4Src.arIP, inFields[3], inFields[4],  inFields[5],
                inFields[6],inFields[7]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the next muticast (ipSrcPtr,ipGroupPtr) entry, used
*       to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router Id.
*       ipGroupPtr        - The ip Group address to get the next entry for.
*       ipSrcPtr          - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr        - The next ip Group address.
*       ipSrcPtr          - The next ip Source address.
*       ipSrcPrefixLenPtr - ipSrc prefix length.
*       mcRoutePointerPtr - the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK             - if found, or
*       GT_BAD_PTR        - on NULL pointer
*       GT_NOT_FOUND      - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       1.  The values of (ipSrcPtr,ipGroupPtr) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipSrcPtr,ipGroupPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv4
*           MC get function.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4McEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U32 lpmDbId;

    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC mcRoutePointer;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    lpmDbId = inArgs[0];
    cmdOsMemSet(&mcRoutePointer, 0, sizeof(mcRoutePointer));

    if(vrId != lastVrfIdTreated)
    {

        ipv4Group.u32Ip = 0;
        ipv4Src.u32Ip = 0;
        ipv4SrcPrefixLen = 0;

        for( ; vrId < numOfVirtualRouters ; vrId++)
        {
            /* call cpss api function */
            result = cpssExMxPmIpLpmIpv4McEntryGet(lpmDbId, vrId, ipv4Group, ipv4Src, ipv4SrcPrefixLen, &mcRoutePointer);

            /* result is GT_NOT_INITIALIZED if the virtual router doesn't exist */
            if (result == GT_NOT_INITIALIZED)
                continue;
            else
            {
                lastVrfIdTreated = vrId;
                isDefaultVrfIdEntry = GT_TRUE;
                break;
            }
        }
    }

    /* all virtual routers were scanned */
    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(isDefaultVrfIdEntry == GT_TRUE)
    {
        /* The first default entry was already treated in w
           wrCpssExMxPmIpLpmIpv4McEntryGetFirst or in previuos call
           to this wrapper                                                   */

        /* show default entry for vrId*/
        inFields[0] = vrId;
        inFields[3] = ipv4SrcPrefixLen;
        inFields[4] = mcRoutePointer.routeEntryBaseMemAddr;
        inFields[5] = mcRoutePointer.blockSize;
        inFields[6] = mcRoutePointer.routeEntryMethod;
        /* inFields[7] is the override field. It has only meaning in the set, but
        we should display it also in the get, else the command wrapper will fail*/
        inFields[7] = 0;

        /* pack and output table fields */
        fieldOutput("%d%4b%4b%d%d%d%d%d",
                inFields[0], ipv4Group.arIP, ipv4Src.arIP, inFields[3], inFields[4],  inFields[5],
                inFields[6],inFields[7]);

        galtisOutput(outArgs, GT_OK, "%f");

        isDefaultVrfIdEntry = GT_FALSE;
    }


    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4McEntryGetNext(lpmDbId, vrId, &ipv4Group, &ipv4Src, &ipv4SrcPrefixLen, &mcRoutePointer);

    if(result == GT_NOT_FOUND)
    {
        vrId++;

        return wrCpssExMxPmIpLpmIpv4McEntryGetNext(inArgs,inFields,numFields,outArgs);
    }

    inFields[0] = vrId;
    inFields[3] = ipv4SrcPrefixLen;
    inFields[4] = mcRoutePointer.routeEntryBaseMemAddr;
    inFields[5] = mcRoutePointer.blockSize;
    inFields[6] = mcRoutePointer.routeEntryMethod;
    /* inFields[7] is the override field. It has only meaning in the set, but
    we should display it also in the get, else the command wrapper will fail*/
    inFields[7] = 0;

    /* pack and output table fields */
    fieldOutput("%d%4b%4b%d%d%d%d%d",
                inFields[0], ipv4Group.arIP, ipv4Src.arIP, inFields[3], inFields[4],  inFields[5],
                inFields[6],inFields[7]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv6UcPrefixAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       creates a new or override an existing Ipv6 prefix in a Virtual Router
*       for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router id.
*       ipAddrPtr         - Points to the destination IP address of this prefix.
*       prefixLen         - The number of bits that are actual valid in the ipAddr.
*       nextHopPointer    - The next hop pointer to set for this prefix.
*       override          - if GT_TRUE, override existing entry if it exists and
*                           return GT_NOT_FOUND if it doesn't exist
*                           if GT_FALSE, add the prefix if it doesn't exist and
*                           return GT_ALREADY_EXIST if the prefix exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success, or
*       GT_ERROR          - If the vrId was not created yet, or
*       GT_NOT_FOUND      - prefix was not found when override is GT_TRUE
*       GT_ALREADY_EXIST  - prefix already exist when override is GT_FALSE
*       GT_BAD_PTR        - on NULL pointer
*       GT_OUT_OF_CPU_MEM - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM  - If failed to allocate PP memory, or
*       GT_FAIL           - otherwise.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPV6ADDR ipAddr;
    GT_U32 prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC nextHopPointer;
    GT_BOOL override;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    vrId = inFields[0];
    galtisIpv6Addr((GT_IPV6ADDR*)&ipAddr, (GT_U8*)inFields[1]);
    prefixLen = inFields[2];
    nextHopPointer.routeEntryBaseMemAddr = inFields[3];
    nextHopPointer.blockSize = inFields[4];
    nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
    override = (GT_BOOL)inFields[6];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6UcPrefixAdd(lpmDbId, vrId, &ipAddr, prefixLen, &nextHopPointer, override);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv6UcPrefixBulkAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId               - The LPM DB id.
*       ipv6PrefixArrayLen    - Length of UC prefix array.
*       ipv6PrefixArray       - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if all prefixes were successfully added
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - if adding one or more prefixes failed; the
*                               array will contain return status for each prefix
*
* COMMENTS:
*       none.
*
*******************************************************************************/

static CPSS_EXMXPM_IP_LPM_IPV6_UC_ADD_PREFIX_STC* ipv6PrefixArrayPtr=NULL;
static GT_U32 ipv6PrefixArrayAllocSize = 0;
static GT_U32 ipv6PrefixArrayInd;

static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkAddFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lpmDbId=0;
    GT_U32 ipv6PrefixArrayLen=0;

    ipv6PrefixArrayInd=0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv6PrefixArrayLen = inArgs[1];

    /* free previously allocated bulk array (if still allocated) */
    if(ipv6PrefixArrayPtr != NULL)
    {
        cmdOsFree(ipv6PrefixArrayPtr);
        ipv6PrefixArrayPtr = NULL;
        ipv6PrefixArrayAllocSize = 0;
    }

    /* check parameter send by user */
    if (ipv6PrefixArrayLen == 0)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    /* allocate memory to hold bulk entries */
    ipv6PrefixArrayPtr = (CPSS_EXMXPM_IP_LPM_IPV6_UC_ADD_PREFIX_STC*)cmdOsMalloc(ipv6PrefixArrayLen * (sizeof(CPSS_EXMXPM_IP_LPM_IPV6_UC_ADD_PREFIX_STC)));
    if (ipv6PrefixArrayPtr == NULL)
    {
        galtisOutput(outArgs, GT_OUT_OF_CPU_MEM, "");
        return CMD_OK;
    }
    ipv6PrefixArrayAllocSize = ipv6PrefixArrayLen;

    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].vrId = inFields[0];
    galtisIpv6Addr((GT_IPV6ADDR*)(&ipv6PrefixArrayPtr[ipv6PrefixArrayInd].ipAddr), (GT_U8*)inFields[1]);
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].prefixLen = inFields[2];
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.routeEntryBaseMemAddr = inFields[3];
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.blockSize = inFields[4];
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].override = (GT_BOOL)inFields[6];
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].returnStatus = (GT_STATUS)inFields[7];

    ipv6PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkAddNext
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

    /* verify there are enough lines in galtis */
    if (ipv6PrefixArrayInd >= ipv6PrefixArrayAllocSize)
    {
        /* free memory allocated to bulk array as add failed */
        if (ipv6PrefixArrayPtr != NULL)
        {
            cmdOsFree(ipv6PrefixArrayPtr);
            ipv6PrefixArrayPtr = NULL;
        }
        ipv6PrefixArrayAllocSize = 0;

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: number of entries is bigger than ipv6PrefixArrayLen.\n");
        return CMD_OK;
    }
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].vrId = inFields[0];
    galtisIpv6Addr((GT_IPV6ADDR*)(&ipv6PrefixArrayPtr[ipv6PrefixArrayInd].ipAddr), (GT_U8*)inFields[1]);
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].prefixLen = inFields[2];
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.routeEntryBaseMemAddr = inFields[3];
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.blockSize = inFields[4];
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].override = (GT_BOOL)inFields[6];
    ipv6PrefixArrayPtr[ipv6PrefixArrayInd].returnStatus = (GT_STATUS)inFields[7];

    ipv6PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkAddEnd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 ipv6PrefixArrayLen=0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv6PrefixArrayLen = inArgs[1];

    if(ipv6PrefixArrayLen != ipv6PrefixArrayInd)
    {
        /* free memory allocated to bulk array as add failed */
        if (ipv6PrefixArrayPtr != NULL)
        {
            cmdOsFree(ipv6PrefixArrayPtr);
            ipv6PrefixArrayPtr = NULL;
        }
        ipv6PrefixArrayAllocSize = 0;

        /* pack output arguments to */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: number of entries is smaller than ipv6PrefixArrayLen.\n");
        return CMD_OK;
    }
    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6UcPrefixBulkAdd(lpmDbId, ipv6PrefixArrayLen, ipv6PrefixArrayPtr);

    /* note that the bulk memory array is not freed as it holds the
       return code for each element insertion and will be used by
       the refresh to get the result of each bulk entry insertion   */

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkAddGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
      GT_U32 lpmDbId;
      GT_U32 ipv6PrefixArrayLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* note that the refresh is only used to display the result of
       each of the prefixes added in the add bulk. therefore it
       only shows the result of the last bulk add */

    /* note that in the refresh, the ipv6PrefixArrayLen (arg[1]) is ignored
       as refresh is used to retrieve the result of each of the prefixes
       add in the aadd bulk. therefore the number of prefixes refreshed is
       set according to the number of prefixes added in the add bulk */

    /* verify that there was a previouly add bulk operation */
    if ((ipv6PrefixArrayPtr == NULL) || (ipv6PrefixArrayAllocSize == 0))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_EMPTY, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv6PrefixArrayLen = inArgs[1];

    ipv6PrefixArrayInd = 0;

    inFields[0] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].vrId;
    inFields[2] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].prefixLen;
    inFields[3] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.routeEntryBaseMemAddr;
    inFields[4] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.blockSize;
    inFields[5] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.routeEntryMethod;
    inFields[6] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].override;
    inFields[7] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].returnStatus;

    /* pack and output table fields */
    fieldOutput("%d%16b%d%d%d%d%d%d",
                inFields[0],  ipv6PrefixArrayPtr[ipv6PrefixArrayInd].ipAddr.arIP,
                inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7]);


    ipv6PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkAddGetNext
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

    /* note that in the refresh, the ipv6PrefixArrayLen (arg[1]) is ignored
       as refresh is used to retrieve the result of each of the prefixes
       add in the aadd bulk. therefore the number of prefixes refreshed is
       set according to the number of prefixes added in the add bulk */

    if (ipv6PrefixArrayInd == ipv6PrefixArrayAllocSize)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].vrId;
    inFields[2] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].prefixLen;
    inFields[3] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.routeEntryBaseMemAddr;
    inFields[4] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.blockSize;
    inFields[5] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].nextHopPointer.routeEntryMethod;
    inFields[6] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].override;
    inFields[7] = ipv6PrefixArrayPtr[ipv6PrefixArrayInd].returnStatus;

    /* pack and output table fields */
    fieldOutput("%d%16b%d%d%d%d%d%d",
                inFields[0],  ipv6PrefixArrayPtr[ipv6PrefixArrayInd].ipAddr.arIP,
                inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7]);

    ipv6PrefixArrayInd++;

   /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv6UcPrefixDelete
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       deletes an existing Ipv6 prefix in a Virtual Router for the specified
*       LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId       - The LPM DB id.
*       vrId          - The virtual router id.
*       ipAddrPtr     - Points to the destination IP address of the prefix.
*       prefixLen     - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success, or
*       GT_ERROR      - If the vrId was not created yet, or
*       GT_BAD_PTR    - on NULL pointer
*       GT_NO_SUCH    - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL       - otherwise.
*
* COMMENTS:
*       The default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPV6ADDR ipAddr;
    GT_U32 prefixLen;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    vrId = inFields[0];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inFields[1]);
    prefixLen = inFields[2];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6UcPrefixDelete(lpmDbId, vrId, &ipAddr, prefixLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv6UcPrefixBulkDelete
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       deletes an existing bulk of Ipv6 prefixes in a Virtual Router for the
*       specified LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId               - The LPM DB id.
*       ipv6PrefixArrayLen    - Length of UC prefix array.
*       ipv6PrefixArray       - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                 - if all prefixes were successfully deleted
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - if deleting one or more prefixes failed; the
*                               array will contain return status for each prefix
*
* COMMENTS:
*       none.
*
*******************************************************************************/

static CPSS_EXMXPM_IP_LPM_IPV6_UC_DELETE_PREFIX_STC* ipv6PrefixArrayDeletePtr;
static GT_U32 ipv6PrefixArrayDeleteAllocSize = 0;
static GT_U32 ipv6PrefixArrayDeleteInd;

static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkDeleteFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lpmDbId;
    GT_U32 ipv6PrefixArrayLen;
    ipv6PrefixArrayDeleteInd = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv6PrefixArrayLen = inArgs[1];

    if (ipv6PrefixArrayLen == 0)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    if(ipv6PrefixArrayLen > ipv6PrefixArrayDeleteAllocSize)
    {
        if(ipv6PrefixArrayDeleteAllocSize != 0)
        {
           cmdOsFree(ipv6PrefixArrayDeletePtr);
        }
        ipv6PrefixArrayDeletePtr = (CPSS_EXMXPM_IP_LPM_IPV6_UC_DELETE_PREFIX_STC*)cmdOsMalloc(ipv6PrefixArrayLen * (sizeof(CPSS_EXMXPM_IP_LPM_IPV6_UC_DELETE_PREFIX_STC)));
        ipv6PrefixArrayDeleteAllocSize = ipv6PrefixArrayLen;
    }

    ipv6PrefixArrayDeletePtr[ipv6PrefixArrayDeleteInd].vrId = inFields[0];
    galtisIpv6Addr((GT_IPV6ADDR*)(&ipv6PrefixArrayDeletePtr[ipv6PrefixArrayDeleteInd].ipAddr), (GT_U8*)inFields[1]);
    ipv6PrefixArrayDeletePtr[ipv6PrefixArrayDeleteInd].prefixLen = inFields[2];
    ipv6PrefixArrayDeletePtr[ipv6PrefixArrayDeleteInd].returnStatus = (GT_STATUS)inFields[3];

    ipv6PrefixArrayDeleteInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkDeleteNext
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

    if (ipv6PrefixArrayDeleteInd == ipv6PrefixArrayDeleteAllocSize)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: number of entries is bigger than ipv6PrefixArrayLen.\n");
        return CMD_OK;
    }

    ipv6PrefixArrayDeletePtr[ipv6PrefixArrayDeleteInd].vrId = inFields[0];
    galtisIpv6Addr((GT_IPV6ADDR*)(&ipv6PrefixArrayDeletePtr[ipv6PrefixArrayDeleteInd].ipAddr), (GT_U8*)inFields[1]);
    ipv6PrefixArrayDeletePtr[ipv6PrefixArrayDeleteInd].prefixLen = inFields[2];
    ipv6PrefixArrayDeletePtr[ipv6PrefixArrayDeleteInd].returnStatus = (GT_STATUS)inFields[3];

    ipv6PrefixArrayDeleteInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkDeleteEnd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 ipv6PrefixArrayLen;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv6PrefixArrayLen = inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6UcPrefixBulkDelete(lpmDbId, ipv6PrefixArrayLen, ipv6PrefixArrayDeletePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkDeleteGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 lpmDbId;
    GT_U32 ipv6PrefixArrayLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    ipv6PrefixArrayLen = inArgs[1];

    ipv6PrefixArrayInd = 0;

    if (ipv6PrefixArrayDeleteAllocSize == 0)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_NOT_FOUND, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = ipv6PrefixArrayDeletePtr[ipv6PrefixArrayInd].vrId;
    inFields[2] = ipv6PrefixArrayDeletePtr[ipv6PrefixArrayInd].prefixLen;
    inFields[3] = ipv6PrefixArrayDeletePtr[ipv6PrefixArrayInd].returnStatus;

    /* pack and output table fields */
    fieldOutput("%d%16b%d%d",
                inFields[0],  ipv6PrefixArrayDeletePtr[ipv6PrefixArrayInd].ipAddr.arIP,
                inFields[2],  inFields[3]);

    ipv6PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkDeleteGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 ipv6PrefixArrayLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    ipv6PrefixArrayLen = inArgs[1];

    if (ipv6PrefixArrayInd == ipv6PrefixArrayLen)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = ipv6PrefixArrayDeletePtr[ipv6PrefixArrayInd].vrId;
    inFields[2] = ipv6PrefixArrayDeletePtr[ipv6PrefixArrayInd].prefixLen;
    inFields[3] = ipv6PrefixArrayDeletePtr[ipv6PrefixArrayInd].returnStatus;

    /* pack and output table fields */
    fieldOutput("%d%16b%d%d",
                inFields[0],  ipv6PrefixArrayDeletePtr[ipv6PrefixArrayInd].ipAddr.arIP,
                inFields[2],  inFields[3]);


    ipv6PrefixArrayInd++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv6UcPrefixBulkOperation
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Applies various prefix operations in single API call.
*       Refer to CPSS_EXMXPM_IP_LPM_IPV6_UC_PREFIX_OPERATION_STC to the possible
*       operations.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       lpmDbId                   - The LPM DB id
*       prefixOperationArrayLen   - Length of prefix array (positive number)
*       prefixOperationArrayPtr   - The prefix array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - if all prefixes were successfully added
*       GT_BAD_PTR                - on NULL pointer
*       GT_FAIL                   - if adding one or more prefixes failed; the
*                                   array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       Operations will be executed according to array order and stop after
*       first failure.
*
*******************************************************************************/
static CPSS_EXMXPM_IP_LPM_IPV6_UC_PREFIX_OPERATION_STC  *ipv6PrefixOperationArray = NULL;
static GT_U32                                           ipv6PrefixOperationArrayLen = 0;
static GT_U32                                           ipv6PrefixOperationArrayIdx = 0;

static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkOperationSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_EXMXPM_IP_LPM_IPV6_UC_PREFIX_OPERATION_STC *tmpPrefixOperation = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* reset bulk operations array */
    if (ipv6PrefixOperationArray != NULL)
        cmdOsFree(ipv6PrefixOperationArray);
    ipv6PrefixOperationArrayLen = 0;
    ipv6PrefixOperationArrayIdx = 0;

    /* allocate memory for this operation */
    ipv6PrefixOperationArrayLen = 1;
    ipv6PrefixOperationArrayIdx = 0;
    ipv6PrefixOperationArray = (CPSS_EXMXPM_IP_LPM_IPV6_UC_PREFIX_OPERATION_STC*)cmdOsRealloc(ipv6PrefixOperationArray,
                               sizeof(CPSS_EXMXPM_IP_LPM_IPV6_UC_PREFIX_OPERATION_STC) * ipv6PrefixOperationArrayLen);
    if (ipv6PrefixOperationArray == NULL)
    {
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Memory allocation failed.\n");
        return CMD_AGENT_ERROR;
    }

    tmpPrefixOperation = &ipv6PrefixOperationArray[ipv6PrefixOperationArrayIdx];

    /* map current operation to the new entry */
    switch (inFields[0])
    {
    case 0:
        tmpPrefixOperation->type = CPSS_BULK_OPERATION_ADD_E;
        tmpPrefixOperation->data.addData.returnStatus = GT_FAIL; /* inFields[1] */
        tmpPrefixOperation->data.addData.vrId = inFields[2];
        galtisIpv6Addr((GT_IPV6ADDR*)(&(tmpPrefixOperation->data.addData.ipAddr)), (GT_U8*)inFields[3]);
        tmpPrefixOperation->data.addData.prefixLen = inFields[4];
        tmpPrefixOperation->data.addData.nextHopPointer.routeEntryBaseMemAddr = inFields[5];
        tmpPrefixOperation->data.addData.nextHopPointer.blockSize = inFields[6];
        tmpPrefixOperation->data.addData.nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[7];
        tmpPrefixOperation->data.addData.override = (GT_BOOL)inFields[8];
        break;

    case 1:
        tmpPrefixOperation->type = CPSS_BULK_OPERATION_DELETE_E;
        tmpPrefixOperation->data.deleteData.returnStatus = GT_FAIL; /* inFields[1] */
        tmpPrefixOperation->data.deleteData.vrId = inFields[2];
        galtisIpv6Addr((GT_IPV6ADDR*)(&(tmpPrefixOperation->data.deleteData.ipAddr)), (GT_U8*)inFields[3]);
        tmpPrefixOperation->data.deleteData.prefixLen = inFields[4];
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Unknown operation.\n");
        return CMD_AGENT_ERROR;
    }

    ipv6PrefixOperationArrayIdx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkOperationSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_EXMXPM_IP_LPM_IPV6_UC_PREFIX_OPERATION_STC *tmpPrefixOperation = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* reallocate memory for this operation */
    ipv6PrefixOperationArrayLen++;
    ipv6PrefixOperationArray = (CPSS_EXMXPM_IP_LPM_IPV6_UC_PREFIX_OPERATION_STC*)cmdOsRealloc(ipv6PrefixOperationArray,
                               sizeof(CPSS_EXMXPM_IP_LPM_IPV6_UC_PREFIX_OPERATION_STC) * ipv6PrefixOperationArrayLen);
    if (ipv6PrefixOperationArray == NULL)
    {
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Memory allocation failed.\n");
        return CMD_AGENT_ERROR;
    }

    tmpPrefixOperation = &ipv6PrefixOperationArray[ipv6PrefixOperationArrayIdx];

    /* map current operation to the new entry */
    switch (inFields[0])
    {
    case 0:
        tmpPrefixOperation->type = CPSS_BULK_OPERATION_ADD_E;
        tmpPrefixOperation->data.addData.returnStatus = GT_FAIL; /* inFields[1] */
        tmpPrefixOperation->data.addData.vrId = inFields[2];
        galtisIpv6Addr((GT_IPV6ADDR*)(&(tmpPrefixOperation->data.addData.ipAddr)), (GT_U8*)inFields[3]);
        tmpPrefixOperation->data.addData.prefixLen = inFields[4];
        tmpPrefixOperation->data.addData.nextHopPointer.routeEntryBaseMemAddr = inFields[5];
        tmpPrefixOperation->data.addData.nextHopPointer.blockSize = inFields[6];
        tmpPrefixOperation->data.addData.nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[7];
        tmpPrefixOperation->data.addData.override = (GT_BOOL)inFields[8];
        break;

    case 1:
        tmpPrefixOperation->type = CPSS_BULK_OPERATION_DELETE_E;
        tmpPrefixOperation->data.deleteData.returnStatus = GT_FAIL; /* inFields[1] */
        tmpPrefixOperation->data.deleteData.vrId = inFields[2];
        galtisIpv6Addr((GT_IPV6ADDR*)(&(tmpPrefixOperation->data.deleteData.ipAddr)), (GT_U8*)inFields[3]);
        tmpPrefixOperation->data.deleteData.prefixLen = inFields[4];
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Unknown operation.\n");
        return CMD_AGENT_ERROR;
    }

    ipv6PrefixOperationArrayIdx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkOperationSetEnd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U32      lpmDbId = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6UcPrefixBulkOperation(lpmDbId,ipv6PrefixOperationArrayLen,ipv6PrefixOperationArray);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkOperationGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_EXMXPM_IP_LPM_IPV6_UC_PREFIX_OPERATION_STC *tmpPrefixOperation = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Note that the refresh is only used to display the results of
       each of the operations performed in the last bulk operation */

    ipv6PrefixOperationArrayIdx = 0;

    if ((ipv6PrefixOperationArrayLen == 0) || (ipv6PrefixOperationArray == NULL))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_NOT_FOUND, "%d", -1);
        return CMD_OK;
    }

    tmpPrefixOperation = &ipv6PrefixOperationArray[ipv6PrefixOperationArrayIdx];

    /* map current operation to the new entry */
    switch (tmpPrefixOperation->type/*inFields[0]*/)
    {
    case CPSS_BULK_OPERATION_ADD_E:
        inFields[0] = 0; /*tmpPrefixOperation->type */
        inFields[1] = tmpPrefixOperation->data.addData.returnStatus;
        inFields[2] = tmpPrefixOperation->data.addData.vrId;
        /*inFields[3] = tmpPrefixOperation->data.addData.ipAddr */
        inFields[4] = tmpPrefixOperation->data.addData.prefixLen;
        inFields[5] = tmpPrefixOperation->data.addData.nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = tmpPrefixOperation->data.addData.nextHopPointer.blockSize;
        inFields[7] = tmpPrefixOperation->data.addData.nextHopPointer.routeEntryMethod;
        inFields[8] = tmpPrefixOperation->data.addData.override;
        break;

    case 1:
        inFields[0] = 1; /* tmpPrefixOperation->type */
        inFields[1] = tmpPrefixOperation->data.deleteData.returnStatus;
        inFields[2] = tmpPrefixOperation->data.deleteData.vrId;
        /*inFields[3] = tmpPrefixOperation->data.deleteData.ipAddr */
        inFields[4] = tmpPrefixOperation->data.deleteData.prefixLen;
        /* next fields has no meaning for delete and will be set to 0 */
        inFields[5] = 0; /* nextHopPointer.routeEntryBaseMemAddr */
        inFields[6] = 0; /* nextHopPointer.blockSize */
        inFields[7] = 0; /* nextHopPointer.routeEntryMethod */
        inFields[8] = 0; /* override */
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Unknown operation.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%16b%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2],
                tmpPrefixOperation->data.deleteData.ipAddr.arIP,
                inFields[4], inFields[5], inFields[6],
                inFields[7], inFields[8]);

    ipv6PrefixOperationArrayIdx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixBulkOperationGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_EXMXPM_IP_LPM_IPV6_UC_PREFIX_OPERATION_STC *tmpPrefixOperation = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Note that the refresh is only used to display the results of
       each of the operations performed in the last bulk operation */

    if (ipv6PrefixOperationArrayIdx == ipv6PrefixOperationArrayLen)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    tmpPrefixOperation = &ipv6PrefixOperationArray[ipv6PrefixOperationArrayIdx];

    /* map current operation to the new entry */
    switch (tmpPrefixOperation->type/*inFields[0]*/)
    {
    case CPSS_BULK_OPERATION_ADD_E:
        inFields[0] = 0; /*tmpPrefixOperation->type */
        inFields[1] = tmpPrefixOperation->data.addData.returnStatus;
        inFields[2] = tmpPrefixOperation->data.addData.vrId;
        /*inFields[3] = tmpPrefixOperation->data.addData.ipAddr */
        inFields[4] = tmpPrefixOperation->data.addData.prefixLen;
        inFields[5] = tmpPrefixOperation->data.addData.nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = tmpPrefixOperation->data.addData.nextHopPointer.blockSize;
        inFields[7] = tmpPrefixOperation->data.addData.nextHopPointer.routeEntryMethod;
        inFields[8] = tmpPrefixOperation->data.addData.override;
        break;

    case 1:
        inFields[0] = 1; /* tmpPrefixOperation->type */
        inFields[1] = tmpPrefixOperation->data.deleteData.returnStatus;
        inFields[2] = tmpPrefixOperation->data.deleteData.vrId;
        /*inFields[3] = tmpPrefixOperation->data.deleteData.ipAddr */
        inFields[4] = tmpPrefixOperation->data.deleteData.prefixLen;
        /* next fields has no meaning for delete and will be set to 0 */
        inFields[5] = 0; /* nextHopPointer.routeEntryBaseMemAddr */
        inFields[6] = 0; /* nextHopPointer.blockSize */
        inFields[7] = 0; /* nextHopPointer.routeEntryMethod */
        inFields[8] = 0; /* override */
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nError: Unknown operation.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%16b%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2],
                tmpPrefixOperation->data.deleteData.ipAddr.arIP,
                inFields[4], inFields[5], inFields[6],
                inFields[7], inFields[8]);

    ipv6PrefixOperationArrayIdx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv6UcPrefixesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       flushes the unicast IPv6 Routing table and stays with the default prefix
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId       - The LPM DB id.
*       vrId          - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL       - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixesFlush
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6UcPrefixesFlush(lpmDbId, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/******************************************************************************/

static  GT_IPV6ADDR ipv6Addr;
static  GT_U32      ipv6PrefixLen;

/*******************************************************************************
* cpssExMxPmIpLpmIpv6UcPrefixGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns an IP-Unicast prefix with larger (ipAddrPtr,prefixLenPtr)
*       than the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router Id to get the entry from.
*       ipAddrPtr         - The ip address to start the search from.
*       prefixLenPtr      - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr         - The ip address of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopPointerPtr - the next hop pointer associated with the found
*                           ipAddr.
*
* RETURNS:
*       GT_OK             - if the required entry was found, or
*       GT_BAD_PTR        - on NULL pointer
*       GT_NOT_FOUND      - if no more entries are left in the IP table.
*
* COMMENTS:
*       1.  The values of (ipAddrPtr,prefixLenPtr) must be a valid values, it
*           means that they exist in the IP-UC Table, unless this is the first
*           call to this function, then the value of (ipAddrPtr,prefixLenPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv6
*           UC prefix get function.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC lpmDbConfig;

    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC nextHopPointer;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];

    cmdOsMemSet(ipMemCfgArray, 0, sizeof(ipMemCfgArray));
    lpmDbConfig.memCfgArray = ipMemCfgArray;
    lpmDbConfig.numOfMemCfg = 10;

    result = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfig);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);

        return CMD_OK;
    }
    numOfVirtualRouters = lpmDbConfig.numOfVirtualRouters;

    vrId = 0;
    ipv6Addr.u32Ip[0] = 0;
    ipv6Addr.u32Ip[1] = 0;
    ipv6Addr.u32Ip[2] = 0;
    ipv6Addr.u32Ip[3] = 0;
    ipv6PrefixLen = 0;
    isDefaultVrfIdEntry = GT_FALSE;

    for( ; vrId < numOfVirtualRouters ; vrId++)
    {
        /* call cpss api function */
        result = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipv6Addr, ipv6PrefixLen, &nextHopPointer);

         /* result is GT_NOT_INITIALIZED if the virtual router doesn't exist */
        if (result == GT_NOT_INITIALIZED)
            continue;
        else
        {
            lastVrfIdTreated = vrId;
            break;
        }
    }

    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = vrId;
    inFields[2] = ipv6PrefixLen;
    inFields[3] = nextHopPointer.routeEntryBaseMemAddr;
    inFields[4] = nextHopPointer.blockSize;
    inFields[5] = nextHopPointer.routeEntryMethod;

    /* pack and output table fields */
    fieldOutput("%d%16b%d%d%d%d",
                inFields[0], ipv6Addr.arIP, inFields[2],  inFields[3],
                inFields[4],inFields[5]);

    galtisOutput(outArgs, GT_OK, "%f");


    return CMD_OK;

}
/*******************************************************************************
* cpssExMxPmIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function searches for a given ip-uc address, and returns the next
*       hop pointer information bound to it if found.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router id.
*       ipAddrPtr         - Points to the destination IP address to look for.
*       prefixLen         - The number of bits that are actual valid in the
*                           ipAddr.
*
* OUTPUTS:
*       nextHopPointerPtr - If found this is The next hop pointer to for this
*                           prefix.
*
* RETURNS:
*       GT_OK             - if the required entry was found, or
*       GT_BAD_PTR        - on NULL pointer
*       GT_NOT_FOUND      - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcPrefixGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U32 lpmDbId;

    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC nextHopPointer;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* all virtual routers were scanned */
    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    lpmDbId = inArgs[0];
    cmdOsMemSet(&nextHopPointer, 0, sizeof(nextHopPointer));

    if(vrId != lastVrfIdTreated)
    {
        ipv6Addr.u32Ip[0] = 0;
        ipv6Addr.u32Ip[1] = 0;
        ipv6Addr.u32Ip[2] = 0;
        ipv6Addr.u32Ip[3] = 0;
        ipv6PrefixLen = 0;

    for( ; vrId < numOfVirtualRouters ; vrId++)
    {
        /* call cpss api function */
        result = cpssExMxPmIpLpmIpv6UcPrefixGet(lpmDbId, vrId, &ipv6Addr, ipv6PrefixLen, &nextHopPointer);

        /* result is GT_NOT_INITIALIZED if the virtual router doesn't exist */
        if (result == GT_NOT_INITIALIZED)
            continue;
        else
            {
                lastVrfIdTreated = vrId;
                isDefaultVrfIdEntry = GT_TRUE;
                break;
            }
        }
    }

    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(isDefaultVrfIdEntry == GT_TRUE)
    {
        /* The first default entry was already treated in
           wrCpssExMxPmIpLpmIpv6UcPrefixGetFirst or in previuos call
           to this wrapper                                                   */

        /* show default entry for vrId*/
        inFields[0] = vrId;
        inFields[2] = ipv6PrefixLen;
        inFields[3] = nextHopPointer.routeEntryBaseMemAddr;
        inFields[4] = nextHopPointer.blockSize;
        inFields[5] = nextHopPointer.routeEntryMethod;

        /* pack and output table fields */
        fieldOutput("%d%16b%d%d%d%d",
                    inFields[0], ipv6Addr.arIP, inFields[2],  inFields[3],
                    inFields[4],inFields[5]);

        galtisOutput(outArgs, GT_OK, "%f");

        isDefaultVrfIdEntry = GT_FALSE;
    }

        /* call cpss api function */
        result = cpssExMxPmIpLpmIpv6UcPrefixGetNext(lpmDbId, vrId, &ipv6Addr, &ipv6PrefixLen, &nextHopPointer);

    if(result == GT_NOT_FOUND)
    {
        vrId++;

        return wrCpssExMxPmIpLpmIpv6UcPrefixGetNext(inArgs,inFields,numFields,outArgs);
    }

    inFields[0] = vrId;
    inFields[2] = ipv6PrefixLen;
    inFields[3] = nextHopPointer.routeEntryBaseMemAddr;
    inFields[4] = nextHopPointer.blockSize;
    inFields[5] = nextHopPointer.routeEntryMethod;

    /* pack and output table fields */
    fieldOutput("%d%16b%d%d%d%d",
                inFields[0], ipv6Addr.arIP, inFields[2],  inFields[3],
                inFields[4],inFields[5]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv6UcLpmGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets a given ip address, and returns the next
*       hop pointer information bounded to the longest prefix match.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router id.
*       ipAddrPtr         - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr      - Points to the number of bits that are actual valid
*                           in the longest match
*       nextHopPointerPtr - The next hop pointer bounded to the longest match
*
* RETURNS:
*       GT_OK             - if the required entry was found, or
*       GT_BAD_PTR        - on NULL pointer
*       GT_NOT_FOUND      - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6UcLpmGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPV6ADDR ipAddr;
    GT_U32 prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC nextHopPointer;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    vrId = inArgs[1];
    galtisIpv6Addr((GT_IPV6ADDR*)(&ipAddr), (GT_U8*)inArgs[2]);

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6UcLpmGet(lpmDbId, vrId, &ipAddr, &prefixLen, &nextHopPointer);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",
                 prefixLen,
                 nextHopPointer.routeEntryBaseMemAddr,
                 nextHopPointer.blockSize,
                 nextHopPointer.routeEntryMethod);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmDbMemSizeGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets the memory size needed to export the Lpm DB,
*       (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId               - The LPM DB id.
*       numOfEntriesInStep    - how many entries in each iteration step
*       lpmDbSizePtr          - the table size calculated up to now (start with 0)
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbSizePtr          - the table size calculated (in bytes)
*       iterPtr               - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbMemSizeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 numOfEntriesInStep;
    GT_U32 lpmDbSize;
    GT_U32 iter;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    lpmDbId = (GT_U32)inArgs[0];
    numOfEntriesInStep = (GT_U32)inArgs[1];
    lpmDbSize = (GT_U32)inArgs[2];
    iter = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmIpLpmDbMemSizeGet(lpmDbId, numOfEntriesInStep, &lpmDbSize, &iter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", lpmDbSize, iter);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmIpLpmIpv6McEntrySearch
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function returns the muticast (ipSrc,ipGroup) entry, used
*       to find specific multicast adrress entry
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId           - The LPM DB id.
*       vrId              - The virtual router Id.
*       ipGroup           - The ip Group address to get the entry for.
*       ipSrc             - The ip Source address to get the entry for.
*       ipSrcPrefixLenPtr - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupRuleIndexPtr  - index of the IPv6 mc group rule in the TCAM.
*       mcRoutePointerPtr    - the mc route entry ptr of the found mc route
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6McEntrySearch
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                              result;
    GT_U32                                 lpmDBId;
    GT_U32                                 vrId;
    GT_IPV6ADDR                            ipGroup;
    GT_U32                                 ipGroupRuleIndex;
    GT_IPV6ADDR                            ipSrc;
    GT_U32                                 ipSrcPrefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC mcRoutePointer;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr((GT_IPV6ADDR*)(&ipGroup), (GT_U8*)inArgs[2]);
    galtisIpv6Addr((GT_IPV6ADDR*)(&ipSrc), (GT_U8*)inArgs[3]);
    ipSrcPrefixLen = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6McEntrySearch(lpmDBId,
                                             vrId,
                                             ipGroup,
                                             &ipGroupRuleIndex,
                                             ipSrc,
                                             ipSrcPrefixLen,
                                             &mcRoutePointer);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", ipGroupRuleIndex,
                 mcRoutePointer.routeEntryBaseMemAddr,
                 mcRoutePointer.blockSize,
                 mcRoutePointer.routeEntryMethod);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmIpLpmDbExport
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function exports the Lpm DB into the preallocated memory,
*       (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId               - The LPM DB id.
*       numOfEntriesInStep    - how many entries in each iteration step
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbPtr              - the lpm DB info block allocated
*       iterPtr               - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbExport
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 numOfEntriesInStep;
    GT_U32  lpmDbSize=0;
    GT_U32 iter;
    GT_U32 sizeIter = 0;
    GT_U8  i = 0;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    lpmDbId = (GT_U32)inArgs[0];
    numOfEntriesInStep = (GT_U32)inArgs[1];
    iter = (GT_U32)inArgs[2];

    /* cpssExMxPmIpLpmDbMemSizeGet is used to allocate exactly the amount
       of space needed for the Export.
       In this case the allocated space need to be freed after Importing the data,
       since calling the Export many time will result in an error (GT_OUT_OF_CPU_MEM).
       By freeing the space after the call to the Import, we will be unable to
       call the Import more then one time.
    */
   do
   {
    lpmDbSize = 0;
    result =  cpssExMxPmIpLpmDbMemSizeGet(lpmDbId, numOfEntriesInStep, &lpmDbSize, &sizeIter);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    lpmDb[i] = cmdOsMalloc(lpmDbSize*sizeof(GT_U32));
    cmdOsMemSet(lpmDb[i], 0, lpmDbSize*sizeof(GT_U32));


    /* call cpss api function */
    result = cpssExMxPmIpLpmDbExport(lpmDbId, numOfEntriesInStep, lpmDb[i], &iter);
    i++;
   }while(iter != 0);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", lpmDb, iter);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmIpLpmDbImport
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function imports the Lpm DB recived and reconstruct it,
*       (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       lpmDbId               - The LPM DB id.
*       numOfEntriesInStep    - how many entries in each iteration step
*       lpmDbPtr              - the lpm DB info block
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       iterPtr               - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on NULL pointer
*       GT_FAIL               - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmDbImport
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 numOfEntriesInStep;
    GT_U32 iter;

    GT_U8 i = 0;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    lpmDbId = (GT_U32)inArgs[0];
    numOfEntriesInStep = (GT_U32)inArgs[2];
    iter = (GT_U32)inArgs[3];

    do
   {
    /* call cpss api function */
    result = cpssExMxPmIpLpmDbImport(lpmDbId, lpmDb[i], numOfEntriesInStep, &iter);
    cmdOsFree(lpmDb[i]);
    i++;
   }while(iter != 0);

   /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", iter);

    return CMD_OK;
}
static CMD_STATUS wrCpssExMxPmLpmDbDump
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8  devNum;
    GT_U32  vrId;
    GT_BOOL isIpv4;
    GT_BOOL isUnicast;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    isIpv4 = (GT_BOOL)inArgs[2];
    isUnicast = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmIpLpmDump(devNum, vrId, isIpv4, isUnicast);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vrId);

    return CMD_OK;
}




/*******************************************************************************
* cpssExMxPmIpLpmIpv6McEntryAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual private network identifier.
*       ipGroup           - The IP multicast group address.
*       ipGroupRuleIndex  - index of the IPv6 mc group rule in the TCAM.
*                           irrelevant for override = GT_TRUE
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefixLen    - The number of bits that are actual valid in,
*                           the ipSrc.
*       mcRoutePointerPtr - (pointer to) the mc Route pointer to set for the mc entry.
*       override          - if GT_TRUE, override existing entry if it exists and
*                           return GT_NOT_FOUND if it doesn't exist
*                           if GT_FALSE, add the prefix if it doesn't exist and
*                           return GT_ALREADY_EXIST if the prefix exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_NOT_FOUND             - prefix was not found when override is GT_TRUE
*       GT_ALREADY_EXIST         - prefix already exist when override is GT_FALSE
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  To override the default mc route use ipGroup = 0.
*       2.  If (S,G) MC group is added when (*,G) doesn't exists then implicit
*           (*,G) is added pointing to (*,*) default. Application added (*,G)
*           will override the implicit (*,G).
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6McEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                              result;
    GT_U32                                 lpmDbId;
    GT_U32                                 vrId;
    GT_IPV6ADDR                            ipGroup;
    GT_U32                                 ipGroupRuleIndex;
    GT_IPV6ADDR                            ipSrc;
    GT_U32                                 ipSrcPrefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC mcRoutePointer;
    GT_BOOL                                override;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inFields[0];
    galtisIpv6Addr((GT_IPV6ADDR*)&ipGroup, (GT_U8*)inFields[1]);
    ipGroupRuleIndex = (GT_U32)inFields[2];
    galtisIpv6Addr((GT_IPV6ADDR*)&ipSrc, (GT_U8*)inFields[3]);
    ipSrcPrefixLen = (GT_U32)inFields[4];
    mcRoutePointer.routeEntryBaseMemAddr = inFields[5];
    mcRoutePointer.blockSize = (GT_U32)inFields[6];
    mcRoutePointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inFields[7];
    override = (GT_BOOL)inFields[8];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6McEntryAdd(lpmDbId,
                                         vrId,
                                         ipGroup,
                                         ipGroupRuleIndex,
                                         ipSrc,
                                         ipSrcPrefixLen,
                                         &mcRoutePointer,
                                         override);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv6McEntryDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router identifier.
*       ipGroup           - The IP multicast group address.
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefixLen    - The number of bits that are actual valid in,
*                           the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefixLen = 0.
*       2.  If no (*,G) was added but (S,G) were added, then implicit (*,G)
*           that points to (*,*) is added. If (*,G) is added later, it will
*           replace the implicit (*,G).
*           When deleting (*,G), then if there are still (S,G), an implicit (*,G)
*           pointing to (*,*) will be added.
*           When deleting last (S,G) and the (*,G) was implicitly added, then
*           the (*,G) will be deleted as well.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6McEntryDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U32      lpmDbId;
    GT_U32      vrId;
    GT_IPV6ADDR ipGroup;
    GT_IPV6ADDR ipSrc;
    GT_U32      ipSrcPrefixLen;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inFields[0];
    galtisIpv6Addr((GT_IPV6ADDR*)(&ipGroup), (GT_U8*)inFields[1]);
    galtisIpv6Addr((GT_IPV6ADDR*)(&ipSrc), (GT_U8*)inFields[3]);
    ipSrcPrefixLen = (GT_U32)inFields[4];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6McEntryDel(lpmDbId,
                                         vrId,
                                         ipGroup,
                                         ipSrc,
                                         ipSrcPrefixLen);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv6McEntriesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       flushes the multicast IP Routing table and stays with the default entry
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       lpmDbId       - The LPM DB id.
*       vrId          - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6McEntryClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32    lpmDbId;
    GT_U32    vrId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inFields[0];

    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6McEntriesFlush(lpmDbId, vrId);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/******************************************************************************/

static GT_IPV6ADDR  ipv6Group;
static GT_IPV6ADDR  ipv6Src;
static GT_U32       ipv6SrcPrefixLen;

/*******************************************************************************
* cpssExMxPmIpLpmIpv6McEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the muticast (ipSrc,ipGroup) entry, used
*       to find specific multicast adrress entry
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router Id.
*       ipGroup           - The ip Group address to get the next entry for.
*       ipSrc             - The ip Source address to get the next entry for.
*       ipSrcPrefixLen    - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupRuleIndexPtr  - (pointer to) index of the IPv6 mc group rule in the TCAM.
*       mcRoutePointerPtr    - (pointer to) the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK                    - if found, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - If the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6McEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;
    GT_U32                           lpmDbId;
    CPSS_EXMXPM_IP_LPM_DB_CONFIG_STC lpmDbConfig;

    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC mcRoutePointer;
    GT_U32                                 ipGroupRuleIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];

    cmdOsMemSet(ipMemCfgArray, 0, sizeof(ipMemCfgArray));
    lpmDbConfig.memCfgArray = ipMemCfgArray;
    lpmDbConfig.numOfMemCfg = 10;

    result = cpssExMxPmIpLpmDbConfigGet(lpmDbId, &lpmDbConfig);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);

        return CMD_OK;
    }
    numOfVirtualRouters = lpmDbConfig.numOfVirtualRouters;
    vrId = 0;

    ipv6Group.u32Ip[0] = 0;
    ipv6Group.u32Ip[1] = 0;
    ipv6Group.u32Ip[2] = 0;
    ipv6Group.u32Ip[3] = 0;
    ipv6Src.u32Ip[0] = 0;
    ipv6Src.u32Ip[1] = 0;
    ipv6Src.u32Ip[2] = 0;
    ipv6Src.u32Ip[3] = 0;
    ipv6SrcPrefixLen = 0;
    ipGroupRuleIndex = 0;
    isDefaultVrfIdEntry = GT_FALSE;

    for( ; vrId < numOfVirtualRouters ; vrId++)
    {
        /* call cpss api function */
        result = cpssExMxPmIpLpmIpv6McEntryGet(lpmDbId, vrId, ipv6Group, ipv6Src, ipv6SrcPrefixLen,&ipGroupRuleIndex, &mcRoutePointer);

        /* result is GT_NOT_INITIALIZED if the virtual router doesn't exist */
        if (result == GT_NOT_INITIALIZED)
            continue;
        else
        {
            lastVrfIdTreated = vrId;
            break;
        }
    }

    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = vrId;
    inFields[2] = ipGroupRuleIndex;
    inFields[4] = ipv6SrcPrefixLen;
    inFields[5] = mcRoutePointer.routeEntryBaseMemAddr;
    inFields[6] = mcRoutePointer.blockSize;
    inFields[7] = mcRoutePointer.routeEntryMethod;
    /* inFields[8] is the override field. It has only meaning in the set, but
    we should display it also in the get, else the command wrapper will fail*/
    inFields[8] = 0;

    /* pack and output table fields */
    fieldOutput("%d%16b%d%16b%d%d%d%d%d",
                inFields[0], ipv6Group.arIP, inFields[2], ipv6Src.arIP, inFields[4], inFields[5],  inFields[6],
                inFields[7],inFields[8]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;

}

/*******************************************************************************
* cpssExMxPmIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the next muticast (ipSrcPtr,ipGroupPtr) entry, used
*       to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES: All ExMxPm devices.
*
* INPUTS:
*       lpmDbId           - The LPM DB id.
*       vrId              - The virtual router Id.
*       ipGroupPtr        - (pointer to) The ip Group address to get the next entry for.
*       ipSrcPtr          - (pointer to) The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr - (pointer to) ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr        - (pointer to) The next ip Group address.
*       ipSrcPtr          - (pointer to) The next ip Source address.
*       ipSrcPrefixLenPtr - (pointer to) ipSrc prefix length.
*       ipGroupRuleIndexPtr  - (pointer to) index of the IPv6 mc group rule in the TCAM.
*       mcRoutePointerPtr - (pointer to) the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK                    - if found, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - If the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  The values of (ipSrcPtr,ipGroupPtr) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipSrcPtr,ipGroupPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv6
*           MC get function.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv6McEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U32 lpmDbId;
    GT_U32                                 ipGroupRuleIndex = 0;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC mcRoutePointer;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    lpmDbId = inArgs[0];
    cmdOsMemSet(&mcRoutePointer, 0, sizeof(mcRoutePointer));

    if(vrId != lastVrfIdTreated)
    {

        ipv6Group.u32Ip[0] = 0;
        ipv6Group.u32Ip[1] = 0;
        ipv6Group.u32Ip[2] = 0;
        ipv6Group.u32Ip[3] = 0;
        ipv6Src.u32Ip[0] = 0;
        ipv6Src.u32Ip[1] = 0;
        ipv6Src.u32Ip[2] = 0;
        ipv6Src.u32Ip[3] = 0;
        ipv6SrcPrefixLen = 0;
        ipGroupRuleIndex = 0;

        for( ; vrId < numOfVirtualRouters ; vrId++)
        {
            /* call cpss api function */
            result = cpssExMxPmIpLpmIpv6McEntryGet(lpmDbId, vrId, ipv6Group, ipv6Src, ipv6SrcPrefixLen, &ipGroupRuleIndex, &mcRoutePointer);
            /* result is GT_NOT_INITIALIZED if the virtual router doesn't exist */
            if (result == GT_NOT_INITIALIZED)
                continue;
            else
            {
                lastVrfIdTreated = vrId;
                isDefaultVrfIdEntry = GT_TRUE;
                break;
            }
        }
    }

    if (vrId == numOfVirtualRouters)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(isDefaultVrfIdEntry == GT_TRUE)
    {
        /* The first default entry was already treated in
           wrCpssExMxPmIpLpmIpv6McEntryGetFirst or in previuos call
           to this wrapper                                                   */

        /* show default entry for vrId*/
        inFields[0] = vrId;
        inFields[2] = ipGroupRuleIndex;
        inFields[4] = ipv6SrcPrefixLen;
        inFields[5] = mcRoutePointer.routeEntryBaseMemAddr;
        inFields[6] = mcRoutePointer.blockSize;
        inFields[7] = mcRoutePointer.routeEntryMethod;

        /* inFields[8] is the override field. It has only meaning in the set, but
        we should display it also in the get, else the command wrapper will fail*/
        inFields[8] = 0;


        /* pack and output table fields */
        fieldOutput("%d%16b%d%16b%d%d%d%d%d",
                    inFields[0], ipv6Group.arIP, inFields[2], ipv6Src.arIP, inFields[4], inFields[5],  inFields[6],
                    inFields[7],inFields[8]);

        galtisOutput(outArgs, GT_OK, "%f");

        isDefaultVrfIdEntry = GT_FALSE;
    }


    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv6McEntryGetNext(lpmDbId, vrId, &ipv6Group, &ipv6Src, &ipv6SrcPrefixLen, &ipGroupRuleIndex, &mcRoutePointer);

    if(result == GT_NOT_FOUND)
    {
        vrId++;

        return wrCpssExMxPmIpLpmIpv6McEntryGetNext(inArgs,inFields,numFields,outArgs);
    }

    inFields[0] = vrId;
    inFields[2] = ipGroupRuleIndex;
    inFields[4] = ipv6SrcPrefixLen;
    inFields[5] = mcRoutePointer.routeEntryBaseMemAddr;
    inFields[6] = mcRoutePointer.blockSize;
    inFields[7] = mcRoutePointer.routeEntryMethod;
    /* inFields[8] is the override field. It has only meaning in the set, but
    we should display it also in the get, else the command wrapper will fail*/
    inFields[8] = 0;

    /* pack and output table fields */
    fieldOutput("%d%16b%d%16b%d%d%d%d%d",
                inFields[0], ipv6Group.arIP, inFields[2], ipv6Src.arIP, inFields[4], inFields[5],  inFields[6],
                inFields[7],inFields[8]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4UcPrefixAddMany
*
* DESCRIPTION:
*       This function tries to add many sequential IPv4 Unicast prefixes and
*       returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*
* OUTPUTS:
*       lastIpAddrAddedPtr    - points to the last prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixAddMany
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipAddr;
    GT_IPADDR lastIpAddr;
    GT_U32 prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC nextHopPointer;
    GT_BOOL override;
    GT_U32 numberOfPrefixesToAdd;
    GT_U32 numOfAddedPrefixes;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    vrId = inArgs[1];
    galtisIpAddr(&ipAddr,(GT_U8*)inArgs[2]);
    prefixLen = inArgs[3];
    nextHopPointer.routeEntryBaseMemAddr = inArgs[4];
    nextHopPointer.blockSize = inArgs[5];
    nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inArgs[6];
    override = (GT_BOOL)inArgs[7];
    numberOfPrefixesToAdd = (GT_U32)inArgs[8];
    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4UcPrefixAddMany(lpmDbId,
                                                vrId,
                                                ipAddr,
                                                nextHopPointer.routeEntryBaseMemAddr,
                                                numberOfPrefixesToAdd,
                                                &lastIpAddr,
                                                &numOfAddedPrefixes);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfAddedPrefixes);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpLpmIpv4UcPrefixGetMany
*
* DESCRIPTION:
*       This function tries to get many sequential IPv4 Unicast prefixes (that were
*       added by cpssExMxPmIpLpmIpv4UcPrefixAddMany) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES: All ExMxPm Devices.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToGet    - the number of prefixes to get
*
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpLpmIpv4UcPrefixGetMany
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipAddr;
    GT_U32 prefixLen;
    CPSS_EXMXPM_IP_ROUTE_ENTRY_POINTER_STC nextHopPointer;
    GT_U32 numberOfPrefixesToGet;
    GT_U32 numOfRetrievedPrefixes;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = inArgs[0];
    vrId = inArgs[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen = inArgs[3];
    nextHopPointer.routeEntryBaseMemAddr = inArgs[4];
    nextHopPointer.blockSize = inArgs[5];
    nextHopPointer.routeEntryMethod = (CPSS_EXMXPM_IP_ROUTE_ENTRY_METHOD_ENT)inArgs[6];
    numberOfPrefixesToGet = inArgs[7];
    /* call cpss api function */
    result = cpssExMxPmIpLpmIpv4UcPrefixGetMany(lpmDbId,
                                                vrId,
                                                ipAddr,
                                                nextHopPointer.routeEntryBaseMemAddr,
                                                numberOfPrefixesToGet,
                                                &numOfRetrievedPrefixes);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfRetrievedPrefixes);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
       {"cpssExMxPmIpLpmDbSetFirst",
         &wrCpssExMxPmIpLpmDbCreateSetFirst,
         0, 11},
        {"cpssExMxPmIpLpmDbSetNext",
         &wrCpssExMxPmIpLpmDbCreateSetNext,
         0, 11},
        {"cpssExMxPmIpLpmDbEndSet",
         &wrCpssExMxPmIpLpmDbCreateEndSet,
         1, 0},
        {"cpssExMxPmIpLpmDbDelete",
         &wrCpssExMxPmIpLpmDbDelete,
         1, 0},
        {"cpssExMxPmIpLpmDbGetFirst",
         &wrCpssExMxPmIpLpmDbConfigGetFirst,
         1, 0},
        {"cpssExMxPmIpLpmDbConfigGetNext",
         &wrCpssExMxPmIpLpmDbConfigGet,
         0, 0},
        {"cpssExMxPmIpLpmDbDevListSetFirst",
         &wrCpssExMxPmIpLpmDbDevListAddFirst,
         2, 1},
        {"cpssExMxPmIpLpmDbDevListSetNext",
         &wrCpssExMxPmIpLpmDbDevListAddNext,
         2, 1},
        {"cpssExMxPmIpLpmDbDevListEndSet",
         &wrCpssExMxPmIpLpmDbDevListAddEnd,
         0, 0},
        {"cpssExMxPmIpLpmDbDevListDelete",
         &wrCpssExMxPmIpLpmDbDevListRemove,
         2, 1},
        {"cpssExMxPmIpLpmDbDevListGetFirst",
         &wrCpssExMxPmIpLpmDbDevListGetFirst,
         2, 0},
        {"cpssExMxPmIpLpmDbDevListGetNext",
         &wrCpssExMxPmIpLpmDbDevListGetNext,
         2, 0},
        {"cpssExMxPm2IpLpmVirtualRouterConfigSet",
         &wrCpssExMxPm2IpLpmVirtualRouterAdd,
         1, 19},
        {"cpssExMxPm2IpLpmVirtualRouterDelete",
         &wrCpssExMxPmIpLpmVirtualRouterDelete,
         2, 0},
        {"cpssExMxPm2IpLpmVirtualRouterConfigGetFirst",
         &wrCpssExMxPm2IpLpmVirtualRouterConfigGetFirst,
         1, 0},
        {"cpssExMxPm2IpLpmVirtualRouterConfigGetNext",
         &wrCpssExMxPm2IpLpmVirtualRouterConfigGet,
         1, 0},
        {"cpssExMxPmIpLpmVirtualRouterConfigSet",
         &wrCpssExMxPmIpLpmVirtualRouterAdd,
         1, 13},
        {"cpssExMxPmIpLpmVirtualRouterDelete",
         &wrCpssExMxPmIpLpmVirtualRouterDelete,
         2, 0},
        {"cpssExMxPmIpLpmVirtualRouterConfigGetFirst",
         &wrCpssExMxPmIpLpmVirtualRouterConfigGetFirst,
         1, 0},
        {"cpssExMxPmIpLpmVirtualRouterConfigGetNext",
         &wrCpssExMxPmIpLpmVirtualRouterConfigGet,
         1, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixSet",
         &wrCpssExMxPmIpLpmIpv4UcPrefixAdd,
         1, 7},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkAddSetFirst",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkAddFirst,
         2, 8},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkAddSetNext",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkAddNext,
         0, 8},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkAddEndSet",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkEndAdd,
         2, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkAddGetFirst",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkAddGetFirst,
         2, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkAddGetNext",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkAddGetNext,
         2, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixDelete",
         &wrCpssExMxPmIpLpmIpv4UcPrefixDelete,
         1, 3},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkDeleteSetFirst",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkDeleteFirst,
         2, 4},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkDeleteSetNext",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkDeleteNext,
         2, 4},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkDeleteEndSet",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkDeleteEnd,
         2, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkDeleteGetFirst",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkDeleteGetFirst,
         2, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkDeleteGetNext",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkDeleteGetNext,
         2, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkOperationSetFirst",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkOperationSetFirst,
         1, 9},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkOperationSetNext",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkOperationSetNext,
         1, 9},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkOperationEndSet",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkOperationSetEnd,
         1, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkOperationGetFirst",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkOperationGetFirst,
         1, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixBulkOperationGetNext",
         &wrCpssExMxPmIpLpmIpv4UcPrefixBulkOperationGetNext,
         1, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixesFlush",
         &wrCpssExMxPmIpLpmIpv4UcPrefixesFlush,
         2, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixGetFirst",
         &wrCpssExMxPmIpLpmIpv4UcPrefixGetFirst,
         1, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixGetNext",
         &wrCpssExMxPmIpLpmIpv4UcPrefixGetNext,
         1, 0},
        {"cpssExMxPmIpLpmIpv4UcLpmGet",
         &wrCpssExMxPmIpLpmIpv4UcLpmGet,
         3, 0},
        {"cpssExMxPmIpLpmIpv4McEntrySet",
         &wrCpssExMxPmIpLpmIpv4McEntryAdd,
         1, 8},
        {"cpssExMxPmIpLpmIpv4McEntryDelete",
         &wrCpssExMxPmIpLpmIpv4McEntryDelete,
         1, 4},
        {"cpssExMxPmIpLpmIpv4McEntriesFlush",
         &wrCpssExMxPmIpLpmIpv4McEntriesFlush,
         2, 0},
        {"cpssExMxPmIpLpmIpv4McEntryGetFirst",
         &wrCpssExMxPmIpLpmIpv4McEntryGetFirst,
         1, 0},
        {"cpssExMxPmIpLpmIpv4McEntryGetNext",
         &wrCpssExMxPmIpLpmIpv4McEntryGetNext,
         1, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixSet",
         &wrCpssExMxPmIpLpmIpv6UcPrefixAdd,
         1, 7},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkAddSetFirst",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkAddFirst,
         2, 7},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkAddSetNext",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkAddNext,
         2, 7},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkAddEndSet",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkAddEnd,
         2, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkAddGetFirst",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkAddGetFirst,
         2, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkAddGetNext",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkAddGetNext,
         2, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixDelete",
         &wrCpssExMxPmIpLpmIpv6UcPrefixDelete,
         1, 3},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkDeleteSetFirst",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkDeleteFirst,
         2, 4},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkDeleteSetNext",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkDeleteNext,
         2, 4},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkDeleteEndSet",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkDeleteEnd,
         2, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkDeleteGetFirst",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkDeleteGetFirst,
         2, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkDeleteGetNext",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkDeleteGetNext,
         2, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkOperationSetFirst",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkOperationSetFirst,
         1, 9},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkOperationSetNext",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkOperationSetNext,
         1, 9},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkOperationEndSet",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkOperationSetEnd,
         1, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkOperationGetFirst",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkOperationGetFirst,
         1, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixBulkOperationGetNext",
         &wrCpssExMxPmIpLpmIpv6UcPrefixBulkOperationGetNext,
         1, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixesFlush",
         &wrCpssExMxPmIpLpmIpv6UcPrefixesFlush,
         2, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixGetFirst",
         &wrCpssExMxPmIpLpmIpv6UcPrefixGetFirst,
         1, 0},
        {"cpssExMxPmIpLpmIpv6UcPrefixGetNext",
         &wrCpssExMxPmIpLpmIpv6UcPrefixGetNext,
         1, 0},
        {"cpssExMxPmIpLpmIpv6UcLpmGet",
         &wrCpssExMxPmIpLpmIpv6UcLpmGet,
         3, 0},
        {"cpssExMxPm2IpLpmIpv6UcLpmGet",
         &wrCpssExMxPmIpLpmIpv6UcLpmGet,
         3, 0},

        {"cpssExMxPmIpLpmDbMemSizeGet",
         &wrCpssExMxPmIpLpmDbMemSizeGet,
         4, 0},
        {"cpssExMxPmIpLpmDbExport",
         &wrCpssExMxPmIpLpmDbExport,
         3, 0},
        {"cpssExMxPmIpLpmDbImport",
         &wrCpssExMxPmIpLpmDbImport,
         4, 0},
        {"cpssExMxPmLpmDump",
         &wrCpssExMxPmLpmDbDump,
         4, 0},
        {"cpssExMxPmIpLpmIpv6McEntrySearch",
         &wrCpssExMxPmIpLpmIpv6McEntrySearch,
         5, 0},
        {"cpssExMxPmIpLpmIpv6McEntrySet",
         &wrCpssExMxPmIpLpmIpv6McEntrySet,
         1, 9},
        {"cpssExMxPmIpLpmIpv6McEntryDelete",
         &wrCpssExMxPmIpLpmIpv6McEntryDelete,
         1, 5},
        {"cpssExMxPmIpLpmIpv6McEntryClear",
         &wrCpssExMxPmIpLpmIpv6McEntryClear,
         1, 0},
        {"cpssExMxPmIpLpmIpv6McEntryGetFirst",
         &wrCpssExMxPmIpLpmIpv6McEntryGetFirst,
         1, 0},
        {"cpssExMxPmIpLpmIpv6McEntryGetNext",
         &wrCpssExMxPmIpLpmIpv6McEntryGetNext,
         1, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixAddMany",
         &wrCpssExMxPmIpLpmIpv4UcPrefixAddMany,
         9, 0},
        {"cpssExMxPmIpLpmIpv4UcPrefixGetMany",
         &wrCpssExMxPmIpLpmIpv4UcPrefixGetMany,
         8, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmIpLpm
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
GT_STATUS cmdLibInitCpssExMxPmIpLpm
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

