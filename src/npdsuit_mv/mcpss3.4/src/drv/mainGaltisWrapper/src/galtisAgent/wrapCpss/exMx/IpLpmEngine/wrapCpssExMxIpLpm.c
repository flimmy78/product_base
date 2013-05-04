/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxIpLpm.c
*
* DESCRIPTION:
*       Wrapper functions for Ip Lpm cpss functions for ExMx.
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
#include <cpss/exMx/exMxGen/ipLpmEngine/cpssExMxIpLpm.h>
#include <cpss/exMx/exMxGen/ipLpmEngine/cpssExMxIpLpmDbg.h>

/* typedef: enum WRAP_BULK_ENTRY_STATE_ENT (type)
 *
 * Description: the role of the entry in wrapper for bulk functions purposes.
 *              indicates the role of an entry in order to determain if to
 *              gather it in a bulk or not and to determain if start or end of
 *              bulk.
 *
 * Enumerations:
        REGULAR_ENTRY       - Does not impose any special bulk operation
 *      BULK_STARTING_ENTRY - indicates that this entry starts a bulk of entries
 *      BULK_ENDING_ENTRY   - indicates that this entry ends a bulk of entries
 */
typedef enum
{
    REGULAR_ENTRY = 0,
    BULK_STARTING_ENTRY = 1,
    BULK_ENDING_ENTRY

}WRAP_BULK_ENTRY_ROLE_ENT;


/****************cpssExMxIpLpmDBCreate Table************/
static CPSS_EXMX_IP_MEM_CFG_STC*        gShadowCfgArray = NULL;
static GT_U32                           gnumOfShadowCfg;

/*******************************************************************************
* cpssExMxIpLpmDBCreate
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*   this function creates an LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId               - the LPM DB id.
*       shadowCfgArray        - array of shadow configurations this LPM DB
*                               supports
*       numOfShadowCfg        - number of shadows configurations in the
*                               shadowCfgArray
*       protocolStack         - the type of protocol stack this shodow support.
*       ipv6MemShare          - percentage of ip memory reserved for IPv6 use
*                               (in the case the Ipv6 is enabled)
*       numOfVirtualRouters   - the number of virtual routers to be supported in
*                               this LPM DB.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_ALREADY_EXIST - if the LPM DB id is already used.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       none.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmDBCreateSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                          ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    gnumOfShadowCfg = 0;/*reset on first*/

    /* map input arguments to locals */
    if(gShadowCfgArray == NULL)
    {
        gShadowCfgArray = (CPSS_EXMX_IP_MEM_CFG_STC*)
                    cmdOsMalloc(sizeof(CPSS_EXMX_IP_MEM_CFG_STC));
    }
    if(gShadowCfgArray == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }


    ind = (GT_U32)inFields[0];

    if(ind > gnumOfShadowCfg)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gShadowCfgArray[ind].routingSramCfgType = (CPSS_NARROW_SRAM_CFG_ENT)inFields[1];
    gShadowCfgArray[ind].numOfSramsSizes = (GT_U32)inFields[2];
    gShadowCfgArray[ind].sramsSizeArray[0] = (CPSS_SRAM_SIZE_ENT)inFields[3];
    gShadowCfgArray[ind].sramsSizeArray[1] = (CPSS_SRAM_SIZE_ENT)inFields[4];
    gShadowCfgArray[ind].sramsSizeArray[2] = (CPSS_SRAM_SIZE_ENT)inFields[5];
    gShadowCfgArray[ind].sramsSizeArray[3] = (CPSS_SRAM_SIZE_ENT)inFields[6];
    gShadowCfgArray[ind].sramsSizeArray[4] = (CPSS_SRAM_SIZE_ENT)inFields[7];

    gnumOfShadowCfg++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmDBCreate
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*   this function creates an LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId               - the LPM DB id.
*       shadowCfgArray        - array of shadow configurations this LPM DB
*                               supports
*       numOfShadowCfg        - number of shadows configurations in the
*                               shadowCfgArray
*       protocolStack         - the type of protocol stack this shodow support.
*       ipv6MemShare          - percentage of ip memory reserved for IPv6 use
*                               (in the case the Ipv6 is enabled)
*       numOfVirtualRouters   - the number of virtual routers to be supported in
*                               this LPM DB.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_ALREADY_EXIST - if the LPM DB id is already used.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       none.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmDBCreateSetNext
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
    gShadowCfgArray = (CPSS_EXMX_IP_MEM_CFG_STC*)
       cmdOsRealloc(gShadowCfgArray,
       sizeof(CPSS_EXMX_IP_MEM_CFG_STC) * (gnumOfShadowCfg+1));

    if(gShadowCfgArray == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gnumOfShadowCfg)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gShadowCfgArray[ind].routingSramCfgType = (CPSS_NARROW_SRAM_CFG_ENT)inFields[1];
    gShadowCfgArray[ind].numOfSramsSizes = (GT_U32)inFields[2];
    gShadowCfgArray[ind].sramsSizeArray[0] = (CPSS_SRAM_SIZE_ENT)inFields[3];
    gShadowCfgArray[ind].sramsSizeArray[1] = (CPSS_SRAM_SIZE_ENT)inFields[4];
    gShadowCfgArray[ind].sramsSizeArray[2] = (CPSS_SRAM_SIZE_ENT)inFields[5];
    gShadowCfgArray[ind].sramsSizeArray[3] = (CPSS_SRAM_SIZE_ENT)inFields[6];
    gShadowCfgArray[ind].sramsSizeArray[4] = (CPSS_SRAM_SIZE_ENT)inFields[7];

    gnumOfShadowCfg++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmDBCreate
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*   this function creates an LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId               - the LPM DB id.
*       shadowCfgArray        - array of shadow configurations this LPM DB
*                               supports
*       numOfShadowCfg        - number of shadows configurations in the
*                               shadowCfgArray
*       protocolStack         - the type of protocol stack this shodow support.
*       ipv6MemShare          - percentage of ip memory reserved for IPv6 use
*                               (in the case the Ipv6 is enabled)
*       numOfVirtualRouters   - the number of virtual routers to be supported in
*                               this LPM DB.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_ALREADY_EXIST - if the LPM DB id is already used.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       none.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmDBCreateEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                          lpmDBId;
    CPSS_IP_PROTOCOL_STACK_ENT      protocolStack;
    GT_U32                          ipv6MemShare;
    GT_U32                          numOfVirtualRouters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    ipv6MemShare = (GT_U32)inArgs[2];
    numOfVirtualRouters = (GT_U32)inArgs[3];

    /* call cpss api function */
    status =  cpssExMxIpLpmDBCreate(lpmDBId, gShadowCfgArray, gnumOfShadowCfg,
                                    protocolStack, ipv6MemShare,
                                    numOfVirtualRouters);

    cmdOsFree(gShadowCfgArray);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


/****************cpssExMxIpLpmDBDevList Table************/
static GT_U8*        gDevList = NULL;
static GT_U32        gNumOfDevs;

/*******************************************************************************
* cpssExMxIpLpmDBDevListAdd
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*   this function adds devices to an existing LPM DB . this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devList        - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND - if the LPM DB id is not found.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmDBDevListAddSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                          ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    gNumOfDevs = 0;/*reset on first*/

    /* map input arguments to locals */
    if(gDevList == NULL)
    {
        gDevList = (GT_U8*)cmdOsMalloc(sizeof(GT_U8));
    }
    if(gDevList == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }


    ind = (GT_U32)inFields[0];

    if(ind > gNumOfDevs)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gDevList[ind] = (GT_U8)inFields[1];

    gNumOfDevs++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmDBDevListAdd
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*   this function adds devices to an existing LPM DB . this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devList        - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND - if the LPM DB id is not found.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmDBDevListAddSetNext
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
    gDevList = (GT_U8*)
       cmdOsRealloc(gDevList,
       sizeof(GT_U8) * (gNumOfDevs+1));

    if(gDevList == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gNumOfDevs)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gDevList[ind] = (GT_U8)inFields[1];

    gNumOfDevs++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmDBDevListAdd
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*   this function adds devices to an existing LPM DB . this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devList        - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND - if the LPM DB id is not found.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmDBDevListAddEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32      lpmDBId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    /* call cpss api function */
    status =  cpssExMxIpLpmDBDevListAdd(lpmDBId, gDevList, gNumOfDevs);

    cmdOsFree(gDevList);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmDBDevsListRemove
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*   this function removes devices from an existing LPM DB . this remove will
*   invoke a hot sync removal of the devices.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devList        - the array of device ids to remove from the
*                        LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND - if the LPM DB id is not found.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmDBDevsListRemove
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32   lpmDBId;
    GT_U8    devList[1];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    devList[0] = (GT_U8)inFields[1];

    /* call cpss api function */
    status =  cpssExMxIpLpmDBDevsListRemove(lpmDBId, devList, 1);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmVirtualRouterSet
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function sets a virtual router in system for specific LPM DB.
*
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*       defUcNextHopPointer - the pointer info of the default UC next hop for
*                             this virtual router.
*       defMcRoutePointer   - the pointer info of the default MC route for this
*                             virtual router.
*       protocolStack       - types of IP stack used in this virtual router.
*       defIpv6McRuleIndex  - in the case a Ipv6 VR is initialized, this is the
*                             Rule index for the ipv6 Default Mc Group.
*       vrIpv6McPclId       - in the case a Ipv6 VR is initialized, this is the
*                             Pcl id for the ipv6 Mc Group entries.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - if failed to allocate PP memory.
*       GT_BAD_STATE - if the existing VR is not empty.
*
* COMMENTS:
*       if this is a existing VR and ,the VR must be empty from prefixes in
*       order to be Re-set!
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmVirtualRouterSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                               lpmDBId;
    GT_U32                               vrId;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC defUcNextHopPointer;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC defMcRoutePointer;
    CPSS_IP_PROTOCOL_STACK_ENT           protocolStack;
    GT_U32                               defIpv6McRuleIndex;
    GT_U32                               vrIpv6McPclId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    vrId = (GT_U32)inArgs[1];
    defUcNextHopPointer.routeEntryBaseMemAddr = (GT_U32)inArgs[2];
    defUcNextHopPointer.blockSize = (GT_U32)inArgs[3];
    defUcNextHopPointer.routeEntryMethod =
                        (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)inArgs[4];

    defMcRoutePointer.routeEntryBaseMemAddr = (GT_U32)inArgs[5];
    defMcRoutePointer.blockSize = (GT_U32)inArgs[6];
    defMcRoutePointer.routeEntryMethod =
                        (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)inArgs[7];

    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[8];
    defIpv6McRuleIndex = (GT_U32)inArgs[9];
    vrIpv6McPclId = (GT_U32)inArgs[10];


    /* call cpss api function */
    status =  cpssExMxIpLpmVirtualRouterSet(lpmDBId, vrId, defUcNextHopPointer,
                                            defMcRoutePointer, protocolStack,
                                            defIpv6McRuleIndex, vrIpv6McPclId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmVirtualRouterDelete
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function deletes a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
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
static CMD_STATUS wrCpssExMxIpLpmVirtualRouterDelete
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
    result = cpssExMxIpLpmVirtualRouterDelete(lpmDbId, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/***********Table: cpssExMxIpLpmIpv4UcPrefix***************/
/*set*/
static  GT_U32                                  gIpv4PrefixArrayLen;
static  CPSS_EXMX_IP_LPM_IPV4_UC_PREFIX_STC*    gIpv4PrefixArray;

/*get*/
static  GT_IPADDR                               gIpAddr;
static  GT_U32                                  gPrefixLen;
static  GT_U32                                  gIndex;

static  GT_BOOL                                 gBulkGather = GT_FALSE;

/*******************************************************************************
* cpssExMxIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS wrCpssExMxIpLpmBulkGatherFirst
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

    gIpv4PrefixArrayLen = 0; /*Reset on First*/

    /* map input arguments to locals */
    if(gIpv4PrefixArray == NULL)
    {
        gIpv4PrefixArray = (CPSS_EXMX_IP_LPM_IPV4_UC_PREFIX_STC*)cmdOsMalloc
                                (sizeof(CPSS_EXMX_IP_LPM_IPV4_UC_PREFIX_STC));
    }
    if(gIpv4PrefixArray == NULL)
    {
      return GT_NO_RESOURCE;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIpv4PrefixArrayLen)
    {
      return GT_BAD_VALUE;
    }

    gIpv4PrefixArray[ind].vrId = (GT_U32)inFields[2];
    galtisIpAddr(&(gIpv4PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
    gIpv4PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
    gIpv4PrefixArray[ind].nextHopPointer.routeEntryBaseMemAddr =
                                                            (GT_U32)inFields[5];
    gIpv4PrefixArray[ind].nextHopPointer.blockSize = (GT_U32)inFields[6];
    gIpv4PrefixArray[ind].nextHopPointer.routeEntryMethod =
                               (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)inFields[7];
    gIpv4PrefixArray[ind].override = (GT_BOOL)inFields[8];
    gIpv4PrefixArrayLen++;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS wrCpssExMxIpLpmBulkGatherNext
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

    gIpv4PrefixArray = (CPSS_EXMX_IP_LPM_IPV4_UC_PREFIX_STC*)cmdOsRealloc
                                (gIpv4PrefixArray, (gIpv4PrefixArrayLen + 1) *
                                 sizeof(CPSS_EXMX_IP_LPM_IPV4_UC_PREFIX_STC));

    if(gIpv4PrefixArray == NULL)
    {
      return GT_NO_RESOURCE;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIpv4PrefixArrayLen)
    {
      return GT_BAD_VALUE;
    }

    gIpv4PrefixArray[ind].vrId = (GT_U32)inFields[2];
    galtisIpAddr(&(gIpv4PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
    gIpv4PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
    gIpv4PrefixArray[ind].nextHopPointer.routeEntryBaseMemAddr =
                                                            (GT_U32)inFields[5];
    gIpv4PrefixArray[ind].nextHopPointer.blockSize = (GT_U32)inFields[6];
    gIpv4PrefixArray[ind].nextHopPointer.routeEntryMethod =
                               (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)inFields[7];
    gIpv4PrefixArray[ind].override = (GT_BOOL)inFields[8];
    gIpv4PrefixArrayLen++;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv4UcPrefixEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{/*just a dummy for galtis*/
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv4UcPrefixDelBulk
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       deletes an existing bulk of Ipv4 prefixes in a Virtual Router for the
*       specified LPM DB.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv4UcPrefixDel
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status1=GT_ERROR, status2=GT_ERROR;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipAddr;
    GT_U32                                   prefixLen;
    WRAP_BULK_ENTRY_ROLE_ENT                 entryBulkRole;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    entryBulkRole = (WRAP_BULK_ENTRY_ROLE_ENT)inFields[1];

    lpmDBId = (GT_U32)inArgs[0];

    switch(entryBulkRole)
    {

    case REGULAR_ENTRY:
        if(gBulkGather)
        {
            status1 = wrCpssExMxIpLpmBulkGatherNext(inArgs, inFields,
                                            numFields, outArgs);
        }
        else/*Regular non bulk one step delete*/
        {
            vrId = (GT_U32)inFields[2];
            galtisIpAddr(&ipAddr, (GT_U8*)inFields[3]);
            prefixLen = (GT_U32)inFields[4];

            /* call cpss api function */
            status1 =  cpssExMxIpLpmIpv4UcPrefixDel(lpmDBId, vrId,
                                                   ipAddr, prefixLen);
        }
        break;

    case BULK_STARTING_ENTRY:
        gBulkGather = GT_TRUE; /*Indicate that Bulk gathering is taking place*/
        status1 = wrCpssExMxIpLpmBulkGatherFirst(inArgs, inFields,
                                                numFields, outArgs);
        break;

    case BULK_ENDING_ENTRY:
        status2 = wrCpssExMxIpLpmBulkGatherNext(inArgs, inFields,
                                               numFields, outArgs);

        status1 =  cpssExMxIpLpmIpv4UcPrefixDelBulk(lpmDBId, gIpv4PrefixArrayLen,
                                                             gIpv4PrefixArray);
        if(status2 != GT_OK)
        {
            status1 = status2;
        }

        cmdOsFree(gIpv4PrefixArray);

        gBulkGather = GT_FALSE;/*End Bulk Gathering indication*/

        break;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status1, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssExMxIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv4UcPrefixAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status1=GT_ERROR, status2=GT_ERROR;

    GT_U32                                   lpmDBId;
    WRAP_BULK_ENTRY_ROLE_ENT                 entryBulkRole;
    GT_BOOL                                  searchMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    entryBulkRole = (WRAP_BULK_ENTRY_ROLE_ENT)inFields[1];

    lpmDBId = (GT_U32)inArgs[0];
    searchMode = (GT_BOOL)inArgs[1];

    switch(entryBulkRole)
    {

    case REGULAR_ENTRY:
        if(gBulkGather)
        {
            status1 = wrCpssExMxIpLpmBulkGatherNext(inArgs, inFields,
                                            numFields, outArgs);
        }
        else/*Regular non bulk one step SET or SEARCH*/
        {
            status2 = wrCpssExMxIpLpmBulkGatherFirst(inArgs, inFields,
                                                    numFields, outArgs);
            /* call cpss api function */
            if(!searchMode)
            {
                status1 =  cpssExMxIpLpmIpv4UcPrefixAdd(lpmDBId,
                                              gIpv4PrefixArray[0].vrId,
                                              gIpv4PrefixArray[0].ipAddr,
                                              gIpv4PrefixArray[0].prefixLen,
                                              &(gIpv4PrefixArray[0].nextHopPointer),
                                              gIpv4PrefixArray[0].override);
                cmdOsFree(gIpv4PrefixArray);
            }
            else/*search mode*/
            {
                status1 =  cpssExMxIpLpmIpv4UcPrefixSearch(lpmDBId,
                                        gIpv4PrefixArray[0].vrId,
                                        gIpv4PrefixArray[0].ipAddr,
                                        gIpv4PrefixArray[0].prefixLen,
                                        &(gIpv4PrefixArray[0].nextHopPointer));
            }

            if(status2 != GT_OK)
            {
                status1 = status2;
            }
        }
        break;

    case BULK_STARTING_ENTRY:
        gBulkGather = GT_TRUE; /*Indicate that Bulk gathering is taking place*/
        status1 = wrCpssExMxIpLpmBulkGatherFirst(inArgs, inFields,
                                                numFields, outArgs);
        break;

    case BULK_ENDING_ENTRY:
        status2 = wrCpssExMxIpLpmBulkGatherNext(inArgs, inFields,
                                               numFields, outArgs);
        if(!searchMode)
        {
            status1 =  cpssExMxIpLpmIpv4UcPrefixAddBulk(lpmDBId,
                                                        gIpv4PrefixArrayLen,
                                                        gIpv4PrefixArray);
            cmdOsFree(gIpv4PrefixArray);
        }
        else/*search mode*/
        {
            status1 =  cpssExMxIpLpmIpv4UcEntryBulkSearch(lpmDBId,
                                                        gIpv4PrefixArrayLen,
                                                        gIpv4PrefixArray);
            /*not freeing array in search mode because it is used later
            when refreshing in search mode*/
        }
        if(status2 != GT_OK)
        {
            status1 = status2;
        }

        gBulkGather = GT_FALSE;/*End Bulk Gathering indication*/

        break;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status1, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssExMxIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function returns an IP-Unicast prefix with larger (ip,prefix) than
*       the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr         - The ip address of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopPointerPtr - the next hop pointer associated with the found
*                           ipAddr.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if no more entries are left in the IP table.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv4UcPrefixGetNext_GetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result= GT_OK;

    GT_U32                                    lpmDBId;
    GT_U32                                    vrId;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC      nextHopPointer;
    GT_BOOL                                   searchMode;

        /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    searchMode = inArgs[1];

    if(!searchMode)
    {
        /* map input arguments to locals */
        lpmDBId = (GT_U32)inArgs[0];
        gIndex = 0;
        vrId = 0;
        gIpAddr.u32Ip = 0; /*for first entry 0 is entered*/
        gPrefixLen = 0; /*for first entry 0 is entered*/

        /* call cpss api function */
        result = cpssExMxIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, gIpAddr,
                                                  gPrefixLen, &nextHopPointer);

        if (result != GT_OK)
        {
            if(result == GT_NOT_FOUND)
            {/*if no more entries are left in the table.*/
                result = GT_OK;
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }
            else
            {
                galtisOutput(outArgs, result, "");
                return CMD_OK;
            }
        }

        inFields[0] = gIndex;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        inFields[4] = gPrefixLen;
        inFields[5] = nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = nextHopPointer.blockSize;
        inFields[7] = nextHopPointer.routeEntryMethod;

        /* pack and output table fields */
        fieldOutput("%d%d%d%4b%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpAddr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7]);
    }
    else/*search mode.  gIpv4PrefixArray was formerly filled in SET operation*/
    {
        if(gIndex >= gIpv4PrefixArrayLen)
        {
            cmdOsFree(gIpv4PrefixArray);
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
        inFields[0] = gIndex;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = gIpv4PrefixArray[gIndex].vrId;
        inFields[4] = gIpv4PrefixArray[gIndex].prefixLen;
        inFields[5] = gIpv4PrefixArray[gIndex].nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = gIpv4PrefixArray[gIndex].nextHopPointer.blockSize;
        inFields[7] = gIpv4PrefixArray[gIndex].nextHopPointer.routeEntryMethod;
        inFields[8] = gIpv4PrefixArray[gIndex].override;
        inFields[9] = gIpv4PrefixArray[gIndex].returnStatus;

        /* pack and output table fields */
        fieldOutput("%d%d%d%4b%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpv4PrefixArray[gIndex].ipAddr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9]);

    }

    galtisOutput(outArgs, result, "%f");

    gIndex++;

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function returns an IP-Unicast prefix with larger (ip,prefix) than
*       the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr         - The ip address of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopPointerPtr - the next hop pointer associated with the found
*                           ipAddr.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if no more entries are left in the IP table.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv4UcPrefixGetNext_GetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result= GT_OK;

    GT_U32                                    lpmDBId;
    GT_U32                                    vrId;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC      nextHopPointer;
    GT_BOOL                                   searchMode;

        /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    searchMode = inArgs[1];

    if(!searchMode)
    {
        /* map input arguments to locals */
        lpmDBId = (GT_U32)inArgs[0];
        vrId = 0;

        /* call cpss api function */
        result = cpssExMxIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &gIpAddr,
                                                  &gPrefixLen, &nextHopPointer);

        if (result != GT_OK)
        {
            if(result == GT_NOT_FOUND)
            {/*if no more entries are left in the table.*/
                result = GT_OK;
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }
            else
            {
                galtisOutput(outArgs, result, "");
                return CMD_OK;
            }
        }

        inFields[0] = gIndex;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        inFields[4] = gPrefixLen;
        inFields[5] = nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = nextHopPointer.blockSize;
        inFields[7] = nextHopPointer.routeEntryMethod;

        /* pack and output table fields */
        fieldOutput("%d%d%d%4b%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpAddr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7]);
    }
    else/*search mode.  gIpv4PrefixArray was formerly filled in SET operation*/
    {
        if(gIndex >= gIpv4PrefixArrayLen)
        {

            cmdOsFree(gIpv4PrefixArray);
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
        inFields[0] = gIndex;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = gIpv4PrefixArray[gIndex].vrId;
        inFields[4] = gIpv4PrefixArray[gIndex].prefixLen;
        inFields[5] = gIpv4PrefixArray[gIndex].nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = gIpv4PrefixArray[gIndex].nextHopPointer.blockSize;
        inFields[7] = gIpv4PrefixArray[gIndex].nextHopPointer.routeEntryMethod;
        inFields[8] = gIpv4PrefixArray[gIndex].override;
        inFields[9] = gIpv4PrefixArray[gIndex].returnStatus;

        /* pack and output table fields */
        fieldOutput("%d%d%d%4b%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpv4PrefixArray[gIndex].ipAddr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9]);

    }

    galtisOutput(outArgs, result, "%f");

    gIndex++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv4UcPrefixesFlush
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       flushes the unicast IPv4 Routing table and stays with the default prefix
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
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
static CMD_STATUS wrCpssExMxIpLpmIpv4UcPrefixesFlush
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrId    = 0;

    /* call cpss api function */
    status =  cpssExMxIpLpmIpv4UcPrefixesFlush(lpmDBId, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*****************Table: cpssExMxIpLpmIpv4Mc**************/
static GT_IPADDR    gIpv4McGroup;
static GT_IPADDR    gIpv4McSrc;
static GT_U32       gIpv4McSrcPrefixLen;
static GT_U32       gIpv4McInd;
/*******************************************************************************
* cpssExMxIpLpmIpv4McEntryAdd
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual private network identifier.
*       ipGroup         - The IP multicast group address.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefix     - The number of bits that are actual valid in,
*                         the ipSrc.
*       mcRoutePointerPtr - the mc Route pointer to set for the mc entry.
*       override        - weather to override an mc Route pointer for the given
*                         prefix
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM if failed to allocate PP memory, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       to override the default mc route use ipGroup = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv4McEntryAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                               status;

    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    GT_IPADDR                               ipGroup;
    GT_IPADDR                               ipSrc;
    GT_U32                                  ipSrcPrefixLen;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC    mcRoutePointer;
    GT_BOOL                                 override;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = inArgs[0];

    vrId = (GT_U32)inFields[1];
    galtisIpAddr(&ipGroup, (GT_U8*)inFields[2]);
    galtisIpAddr(&ipSrc, (GT_U8*)inFields[3]);
    ipSrcPrefixLen = (GT_U32)inFields[4];
    mcRoutePointer.routeEntryBaseMemAddr = (GT_U32)inFields[5];
    mcRoutePointer.blockSize = (GT_U32)inFields[6];
    mcRoutePointer.routeEntryMethod =
                               (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)inFields[7];
    override = (GT_BOOL)inFields[8];

    /* call cpss api function */
    status =  cpssExMxIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup,
                                          ipSrc, ipSrcPrefixLen ,
                                          &mcRoutePointer, override);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv4McEntryDel
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.
*       ipGroup         - The IP multicast group address.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - if the virtual router does not exist, or
*       GT_NOT_FOUND - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv4McEntryDel
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       status;

    GT_U32                          lpmDBId;
    GT_U32                          vrId;
    GT_IPADDR                       ipGroup;
    GT_IPADDR                       ipSrc;
    GT_U32                          ipSrcPrefixLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = inArgs[0];

    vrId = (GT_U32)inFields[1];
    galtisIpAddr(&ipGroup, (GT_U8*)inFields[2]);
    galtisIpAddr(&ipSrc, (GT_U8*)inFields[3]);
    ipSrcPrefixLen = (GT_U32)inFields[4];

    /* call cpss api function */
    status =  cpssExMxIpLpmIpv4McEntryDel(lpmDBId, vrId, ipGroup,
                                          ipSrc, ipSrcPrefixLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv4McEntriesFlush
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       flushes the multicast IP Routing table and stays with the default entry
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
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
static CMD_STATUS wrCpssExMxIpLpmIpv4McEntriesFlush
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       status;

    GT_U32                          lpmDBId;
    GT_U32                          vrId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = inArgs[0];

    vrId = 0;

    /* call cpss api function */
    status =  cpssExMxIpLpmIpv4McEntriesFlush(lpmDBId, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function returns the next muticast (ipSrc,ipGroup) entry, used
*       to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId           - The LPM DB id.
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
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       1.  The values of (ipGroup,ipGroupPrefix) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipGroup,ipSrc) is
*           (0,0).
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv4McEntryGetNext_GetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U32                                      lpmDBId;
    GT_U32                                      vrId;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC        mcRoutePointer;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    gIpv4McInd = 0;
    vrId = 0;/*currently only 0 is valid*/
    gIpv4McGroup.u32Ip = 0; /*for first entry 0 is entered*/
    gIpv4McSrc.u32Ip = 0; /*for first entry 0 is entered*/
    gIpv4McSrcPrefixLen = 0; /*for first entry 0 is entered*/

    /* call cpss api function */
    result = cpssExMxIpLpmIpv4McEntrySearch(lpmDBId, vrId, gIpv4McGroup, gIpv4McSrc, gIpv4McSrcPrefixLen,&mcRoutePointer);

    if (result != GT_OK)
    {
        if(result == GT_NOT_FOUND)
        {/*if no more entries are left in the table.*/
            result = GT_OK;
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
        else
        {
            galtisOutput(outArgs, result, "");
            return result;
        }
    }

    /* pack and output table fields*/
    inFields[0] = gIpv4McInd;
    inFields[1] = vrId;
    inFields[4] = gIpv4McSrcPrefixLen;
    inFields[5] = mcRoutePointer.routeEntryBaseMemAddr;
    inFields[6] = mcRoutePointer.blockSize;
    inFields[7] = mcRoutePointer.routeEntryMethod;
    /* inFields[8] is the override field. It has only meaning in the set, but
    we should display it also in the get, else the command wrapper will fail*/
    inFields[8] = 0;

    /* pack and output table fields */
    fieldOutput("%d%d%4b%4b%d%d%d%d%d"
                ,inFields[0], inFields[1], gIpv4McGroup.arIP, gIpv4McSrc.arIP,
                inFields[4], inFields[5], inFields[6], inFields[7],inFields[8]);

    galtisOutput(outArgs, result, "%f");

    gIpv4McInd++;

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function returns the next muticast (ipSrc,ipGroup) entry, used
*       to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId           - The LPM DB id.
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
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       1.  The values of (ipGroup,ipGroupPrefix) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipGroup,ipSrc) is
*           (0,0).
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv4McEntryGetNext_GetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC    mcRoutePointer;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = 0;/*currently only 0 is valid*/

    /* call cpss api function */
    result = cpssExMxIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &gIpv4McGroup,
                                             &gIpv4McSrc, &gIpv4McSrcPrefixLen,
                                             &mcRoutePointer);

    if (result != GT_OK)
    {
        if(result == GT_NOT_FOUND)
        {/*if no more entries are left in the table.*/
            result = GT_OK;
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
        else
        {
            galtisOutput(outArgs, result, "");
            return result;
        }
    }

    /* pack and output table fields*/
    inFields[0] = gIpv4McInd;
    inFields[1] = vrId;
    inFields[4] = gIpv4McSrcPrefixLen;
    inFields[5] = mcRoutePointer.routeEntryBaseMemAddr;
    inFields[6] = mcRoutePointer.blockSize;
    inFields[7] = mcRoutePointer.routeEntryMethod;
    /* inFields[8] is the override field. It has only meaning in the set, but
    we should display it also in the get, else the command wrapper will fail*/
    inFields[8] = 0;

    /* pack and output table fields */
    fieldOutput("%d%d%4b%4b%d%d%d%d%d"
                ,inFields[0], inFields[1], gIpv4McGroup.arIP, gIpv4McSrc.arIP,
                inFields[4], inFields[5], inFields[6], inFields[7],inFields[8]);

    galtisOutput(outArgs, result, "%f");

    gIpv4McInd++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv4McEntrySearch
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
*       lpmDBId        - The LPM DB id.
*       vrId           - The virtual router Id.
*       ipGroup        - The ip Group address to get the next entry for.
*       ipSrc          - The ip Source address to get the next entry for.
*       ipSrcPrefixLen - ipSrc prefix length.
*
* OUTPUTS:
*       mcRoutePointerPtr - the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv4McEntrySearch
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipGroup;
    GT_IPADDR                                ipSrc;
    GT_U32                                   ipSrcPrefixLen;
    OUT CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC mcRoutePointer;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipGroup, (GT_U8*)inArgs[2]);
    galtisIpAddr(&ipSrc, (GT_U8*)inArgs[3]);
    ipSrcPrefixLen = (GT_U32)inArgs[4];

    /* call cpss api function */
    status =  cpssExMxIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup, ipSrc,
                                             ipSrcPrefixLen, &mcRoutePointer);

    galtisOutput(outArgs, status, "%d%d%d",
                 mcRoutePointer.routeEntryBaseMemAddr,
                 mcRoutePointer.blockSize,
                 mcRoutePointer.routeEntryMethod);

    return CMD_OK;
}

/***********Table: cpssExMxIpLpmIpv6UcPrefix***************/
/*set*/
static  GT_U32                                  gIpV6PrefixArrayLen;
static  CPSS_EXMX_IP_LPM_IPV6_UC_PREFIX_STC*    gIpV6PrefixArray;

/*get*/
static  GT_IPV6ADDR                             gIpV6Addr;
static  GT_U32                                  gIpV6PrefixLen;
static  GT_U32                                  gIpV6Index;

static  GT_BOOL                                 gIpV6BulkGather = GT_FALSE;
/*******************************************************************************
* cpssExMxIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS wrCpssExMxIpV6LpmBulkGatherFirst
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

    gIpV6PrefixArrayLen = 0; /*Reset on First*/

    /* map input arguments to locals */
    if(gIpV6PrefixArray == NULL)
    {
        gIpV6PrefixArray = (CPSS_EXMX_IP_LPM_IPV6_UC_PREFIX_STC*)cmdOsMalloc
                                (sizeof(CPSS_EXMX_IP_LPM_IPV6_UC_PREFIX_STC));
    }
    if(gIpV6PrefixArray == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIpV6PrefixArrayLen)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gIpV6PrefixArray[ind].vrId = (GT_U32)inFields[2];
    galtisIpv6Addr(&(gIpV6PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
    gIpV6PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
    gIpV6PrefixArray[ind].nextHopPointer.routeEntryBaseMemAddr =
                                                            (GT_U32)inFields[5];
    gIpV6PrefixArray[ind].nextHopPointer.blockSize = (GT_U32)inFields[6];
    gIpV6PrefixArray[ind].nextHopPointer.routeEntryMethod =
                               (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)inFields[7];
    gIpV6PrefixArray[ind].override = (GT_BOOL)inFields[8];

    gIpV6PrefixArrayLen++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS wrCpssExMxIpV6LpmBulkGatherNext
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

    gIpV6PrefixArray = (CPSS_EXMX_IP_LPM_IPV6_UC_PREFIX_STC*)cmdOsRealloc
                                (gIpV6PrefixArray, (gIpV6PrefixArrayLen + 1) *
                                 sizeof(CPSS_EXMX_IP_LPM_IPV6_UC_PREFIX_STC));

    if(gIpV6PrefixArray == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gIpV6PrefixArrayLen)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gIpV6PrefixArray[ind].vrId = (GT_U32)inFields[2];
    galtisIpv6Addr(&(gIpV6PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
    gIpV6PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
    gIpV6PrefixArray[ind].nextHopPointer.routeEntryBaseMemAddr =
                                                            (GT_U32)inFields[5];
    gIpV6PrefixArray[ind].nextHopPointer.blockSize = (GT_U32)inFields[6];
    gIpV6PrefixArray[ind].nextHopPointer.routeEntryMethod =
                               (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)inFields[7];
    gIpV6PrefixArray[ind].override = (GT_BOOL)inFields[8];

    gIpV6PrefixArrayLen++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpV6UcPrefixAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status1=GT_ERROR, status2=GT_ERROR;

    GT_U32                                   lpmDBId;
    WRAP_BULK_ENTRY_ROLE_ENT                 entryBulkRole;
    GT_BOOL                                  searchMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    entryBulkRole = (WRAP_BULK_ENTRY_ROLE_ENT)inFields[1];

    lpmDBId = (GT_U32)inArgs[0];
    searchMode = (GT_BOOL)inArgs[1];

    switch(entryBulkRole)
    {

    case REGULAR_ENTRY:
        if(gIpV6BulkGather)
        {
            status1 = wrCpssExMxIpV6LpmBulkGatherNext(inArgs, inFields,
                                            numFields, outArgs);
        }
        else/*Regular non bulk one step SET or SEARCH*/
        {
            status2 = wrCpssExMxIpV6LpmBulkGatherFirst(inArgs, inFields,
                                                    numFields, outArgs);
            /* call cpss api function */
            if(!searchMode)
            {
                status1 =  cpssExMxIpLpmIpv6UcPrefixAdd(lpmDBId,
                                              gIpV6PrefixArray[0].vrId,
                                              gIpV6PrefixArray[0].ipAddr,
                                              gIpV6PrefixArray[0].prefixLen,
                                              &(gIpV6PrefixArray[0].nextHopPointer),
                                              gIpV6PrefixArray[0].override);
                cmdOsFree(gIpV6PrefixArray);
            }
            else/*search mode*/
            {
                status1 =  cpssExMxIpLpmIpv6UcPrefixSearch(lpmDBId,
                                        gIpV6PrefixArray[0].vrId,
                                        gIpV6PrefixArray[0].ipAddr,
                                        gIpV6PrefixArray[0].prefixLen,
                                        &(gIpV6PrefixArray[0].nextHopPointer));
            }

            if(status2 != GT_OK)
            {
                status1 = status2;
            }
        }
        break;

    case BULK_STARTING_ENTRY:
        gIpV6BulkGather = GT_TRUE; /*Indicate that Bulk gathering is taking place*/
        status1 = wrCpssExMxIpV6LpmBulkGatherFirst(inArgs, inFields,
                                                numFields, outArgs);
        break;

    case BULK_ENDING_ENTRY:
        status2 = wrCpssExMxIpV6LpmBulkGatherNext(inArgs, inFields,
                                               numFields, outArgs);
        if(!searchMode)
        {
            status1 =  cpssExMxIpLpmIpv6UcPrefixAddBulk(lpmDBId,
                                                        gIpV6PrefixArrayLen,
                                                        gIpV6PrefixArray);
            cmdOsFree(gIpV6PrefixArray);
        }
        else/*search mode*/
        {
            status1 =  cpssExMxIpLpmIpv6UcEntryBulkSearch(lpmDBId,
                                                        gIpV6PrefixArrayLen,
                                                        gIpV6PrefixArray);
            /*not freeing array in search mode because it is used later
            when refreshing in search mode*/
        }
        if(status2 != GT_OK)
        {
            status1 = status2;
        }

        gIpV6BulkGather = GT_FALSE;/*End Bulk Gathering indication*/

        break;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status1, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssExMxIpLpmIpv6UcPrefixDel
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       deletes an existing Ipv6 prefix in a Virtual Router for the specified
*       LPM DB.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - If the vrId was not created yet, or
*       GT_NO_SUCH - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv6UcPrefixDel
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status1=GT_ERROR, status2=GT_ERROR;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipAddr;
    GT_U32                                   prefixLen;
    WRAP_BULK_ENTRY_ROLE_ENT                 entryBulkRole;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    entryBulkRole = (WRAP_BULK_ENTRY_ROLE_ENT)inFields[1];

    lpmDBId = (GT_U32)inArgs[0];

    switch(entryBulkRole)
    {

    case REGULAR_ENTRY:
        if(gIpV6BulkGather)
        {
            status1 = wrCpssExMxIpV6LpmBulkGatherNext(inArgs, inFields,
                                            numFields, outArgs);
        }
        else/*Regular non bulk one step delete*/
        {
            vrId = (GT_U32)inFields[2];
            galtisIpv6Addr(&ipAddr, (GT_U8*)inFields[3]);
            prefixLen = (GT_U32)inFields[4];

            /* call cpss api function */
            status1 =  cpssExMxIpLpmIpv6UcPrefixDel(lpmDBId, vrId,
                                                   ipAddr, prefixLen);
        }
        break;

    case BULK_STARTING_ENTRY:
        gIpV6BulkGather = GT_TRUE; /*Indicate that Bulk gathering is taking place*/
        status1 = wrCpssExMxIpV6LpmBulkGatherFirst(inArgs, inFields,
                                                numFields, outArgs);
        break;

    case BULK_ENDING_ENTRY:
        status2 = wrCpssExMxIpV6LpmBulkGatherNext(inArgs, inFields,
                                               numFields, outArgs);

        status1 =  cpssExMxIpLpmIpv6UcPrefixDelBulk(lpmDBId, gIpV6PrefixArrayLen,
                                                             gIpV6PrefixArray);
        if(status2 != GT_OK)
        {
            status1 = status2;
        }

        cmdOsFree(gIpV6PrefixArray);

        gIpV6BulkGather = GT_FALSE;/*End Bulk Gathering indication*/

        break;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status1, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssExMxIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv6UcPrefixEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{/*just a dummy for galtis*/
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function returns an IP-Unicast prefix with larger (ip,prefix) than
*       the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr         - The ip address of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopPointerPtr - the next hop pointer associated with the found
*                           ipAddr.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if no more entries are left in the IP table.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv6UcPrefixGetNext_GetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result=GT_OK;

    GT_U32                                    lpmDBId;
    GT_U32                                    vrId;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC      nextHopPointer;
    GT_BOOL                                   searchMode;

        /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    searchMode = inArgs[1];

    if(!searchMode)
    {
        /* map input arguments to locals */
        lpmDBId = (GT_U32)inArgs[0];
        gIpV6Index = 0;
        vrId = 0;/*for first entry 0 is entered*/
        gIpV6Addr.u32Ip[0] = 0; /*for first entry 0 is entered*/
        gIpV6Addr.u32Ip[1] = 0; /*for first entry 0 is entered*/
        gIpV6Addr.u32Ip[2] = 0; /*for first entry 0 is entered*/
        gIpV6Addr.u32Ip[3] = 0; /*for first entry 0 is entered*/
        gIpV6PrefixLen = 0; /*for first entry 0 is entered*/

        /* call cpss api function */
        result = cpssExMxIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, gIpV6Addr,
                                                  gIpV6PrefixLen, &nextHopPointer);

        if (result != GT_OK)
        {
            if(result == GT_NOT_FOUND)
            {/*if no more entries are left in the table.*/
                result = GT_OK;
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }
            else
            {
                galtisOutput(outArgs, result, "");
                return result;
            }
        }

        inFields[0] = gIpV6Index;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        inFields[4] = gIpV6PrefixLen;
        inFields[5] = nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = nextHopPointer.blockSize;
        inFields[7] = nextHopPointer.routeEntryMethod;

        /* pack and output table fields */
        fieldOutput("%d%d%d%16b%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpV6Addr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7]);

    }
    else/*search mode.  gIpv6PrefixArray was formerly filled in SET operation*/
    {
        if(gIpV6Index >= gIpV6PrefixArrayLen)
        {
            cmdOsFree(gIpV6PrefixArray);
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
        inFields[0] = gIpV6Index;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = gIpV6PrefixArray[gIndex].vrId;
        inFields[4] = gIpV6PrefixArray[gIndex].prefixLen;
        inFields[5] = gIpV6PrefixArray[gIndex].nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = gIpV6PrefixArray[gIndex].nextHopPointer.blockSize;
        inFields[7] = gIpV6PrefixArray[gIndex].nextHopPointer.routeEntryMethod;
        inFields[8] = gIpV6PrefixArray[gIndex].override;
        inFields[9] = gIpV6PrefixArray[gIndex].returnStatus;

        /* pack and output table fields */
        fieldOutput("%d%d%d%16b%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpV6PrefixArray[gIndex].ipAddr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9]);

    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    gIpV6Index++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function returns an IP-Unicast prefix with larger (ip,prefix) than
*       the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr         - The ip address of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopPointerPtr - the next hop pointer associated with the found
*                           ipAddr.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if no more entries are left in the IP table.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv6UcPrefixGetNext_GetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result=GT_OK;

    GT_U32                                    lpmDBId;
    GT_U32                                    vrId;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC      nextHopPointer;

    GT_BOOL                                   searchMode;

        /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    searchMode = inArgs[1];

    if(!searchMode)
    {
        /* map input arguments to locals */
        lpmDBId = (GT_U32)inArgs[0];
        vrId = 0;/*for first entry 0 is entered*/

        /* call cpss api function */
        result = cpssExMxIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, &gIpV6Addr,
                                                  &gIpV6PrefixLen, &nextHopPointer);

        if (result != GT_OK)
        {
            if(result == GT_NOT_FOUND)
            {/*if no more entries are left in the table.*/
                result = GT_OK;
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }
            else
            {
                galtisOutput(outArgs, result, "");
                return result;
            }
        }

        inFields[0] = gIpV6Index;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        inFields[4] = gIpV6PrefixLen;
        inFields[5] = nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = nextHopPointer.blockSize;
        inFields[7] = nextHopPointer.routeEntryMethod;

        /* pack and output table fields */
        fieldOutput("%d%d%d%16b%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpV6Addr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7]);
    }

    else/*search mode.  gIpv6PrefixArray was formerly filled in SET operation*/
    {
        if(gIpV6Index >= gIpV6PrefixArrayLen)
        {
            cmdOsFree(gIpV6PrefixArray);
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
        inFields[0] = gIpV6Index;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = gIpV6PrefixArray[gIndex].vrId;
        inFields[4] = gIpV6PrefixArray[gIndex].prefixLen;
        inFields[5] = gIpV6PrefixArray[gIndex].nextHopPointer.routeEntryBaseMemAddr;
        inFields[6] = gIpV6PrefixArray[gIndex].nextHopPointer.blockSize;
        inFields[7] = gIpV6PrefixArray[gIndex].nextHopPointer.routeEntryMethod;
        inFields[8] = gIpV6PrefixArray[gIndex].override;
        inFields[9] = gIpV6PrefixArray[gIndex].returnStatus;

        /* pack and output table fields */
        fieldOutput("%d%d%d%16b%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpV6PrefixArray[gIndex].ipAddr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9]);

    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    gIpV6Index++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv6UcPrefixesFlush
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       flushes the unicast IPv6 Routing table and stays with the default prefix
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
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
static CMD_STATUS wrCpssExMxIpLpmIpv6UcPrefixesFlush
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrId = 0;

    /* call cpss api function */
    status =  cpssExMxIpLpmIpv6UcPrefixesFlush(lpmDBId, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*****************Table: cpssExMxIpLpmIpv6Mc**************/
static GT_IPV6ADDR      gIpv6McGroup;
static GT_IPV6ADDR      gIpv6McSrc;
static GT_U32           gIpv6McSrcPrefixLen;
static GT_U32           gIpv6McInd;
/*******************************************************************************
* cpssExMxIpLpmIpv6McEntryAdd
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual private network identifier.
*       ipGroup         - The IP multicast group address.
*       ipGroupAddressType - the type of address this ip group is (mangement or
*                         regular)
*       ipGroupRuleIndex- index of the IPv6 mc group rule in the TCAM.
*                         irrelevant for override = GT_TRUE
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefix     - The number of bits that are actual valid in,
*                         the ipSrc.
*       groupScope      - address scope of the  multicast group. irrelevant for
*                         override = GT_TRUE
*       mcRoutePointerPtr - the mc Route pointer to set for the mc entry.
*       override        - weather to override the mcRoutePointerPtr for the
*                         given prefix
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM if failed to allocate PP memory, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       to override the default mc route use ipGroup = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv6McEntryAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                               status;

    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    GT_IPV6ADDR                             ipGroup;
    CPSS_EXMX_IP_MC_ADDRESS_TYPE_ENT        ipGroupAddressType;
    GT_U32                                  ipGroupRuleIndex;
    GT_IPV6ADDR                             ipSrc;
    GT_U32                                  ipSrcPrefixLen;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT         groupScope;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC    mcRoutePointer;
    GT_BOOL                                 override;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = inArgs[0];

    vrId = (GT_U32)inFields[1];
    galtisIpv6Addr(&ipGroup, (GT_U8*)inFields[2]);
    ipGroupAddressType = (CPSS_EXMX_IP_MC_ADDRESS_TYPE_ENT)inFields[3];
    ipGroupRuleIndex = (GT_U32)inFields[4];
    galtisIpv6Addr(&ipSrc, (GT_U8*)inFields[5]);
    ipSrcPrefixLen = (GT_U32)inFields[6];
    groupScope = (CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT)inFields[7];
    mcRoutePointer.routeEntryBaseMemAddr = (GT_U32)inFields[8];
    mcRoutePointer.blockSize = (GT_U32)inFields[9];
    mcRoutePointer.routeEntryMethod =
                              (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)inFields[10];
    override = (GT_BOOL)inFields[11];

    /* call cpss api function */
    status =  cpssExMxIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup,
                                          ipGroupAddressType, ipGroupRuleIndex,
                                          ipSrc, ipSrcPrefixLen, groupScope,
                                          &mcRoutePointer, override);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv6McEntryDel
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.
*       ipGroup         - The IP multicast group address.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - if the virtual router does not exist, or
*       GT_NOT_FOUND - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv6McEntryDel
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       status;

    GT_U32                          lpmDBId;
    GT_U32                          vrId;
    GT_IPV6ADDR                     ipGroup;
    GT_IPV6ADDR                     ipSrc;
    GT_U32                          ipSrcPrefixLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = inArgs[0];

    vrId = (GT_U32)inFields[1];
    galtisIpv6Addr(&ipGroup, (GT_U8*)inFields[2]);
    galtisIpv6Addr(&ipSrc, (GT_U8*)inFields[5]);
    ipSrcPrefixLen = (GT_U32)inFields[6];

    /* call cpss api function */
    status =  cpssExMxIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup, ipSrc,
                                          ipSrcPrefixLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv6McEntriesFlush
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       flushes the multicast IP Routing table and stays with the default entry
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
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
static CMD_STATUS wrCpssExMxIpLpmIpv6McEntriesFlush
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       status;

    GT_U32                          lpmDBId;
    GT_U32                          vrId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = inArgs[0];

    vrId = 0;

    /* call cpss api function */
    status =  cpssExMxIpLpmIpv6McEntriesFlush(lpmDBId, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function returns the next muticast (ipSrc,ipGroup) entry, used
*       to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId           - The LPM DB id.
*       vrId              - The virtual router Id.
*       ipGroupPtr        - The ip Group address to get the next entry for.
*       ipSrcPtr          - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr           - The next ip Group address.
*       ipGroupAddressTypePtr- the type of address this ip group is (mangement
*                              or regular)
*       ipGroupRuleIndexPtr  - index of the IPv6 mc group rule in the TCAM.
*       ipSrcPtr             - The next ip Source address.
*       ipSrcPrefixLenPtr    - ipSrc prefix length.
*       mcRoutePointerPtr    - the mc route entry ptr of the found mc mc route
*       groupScopePtr        - address scope of the  multicast group.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       1.  The values of (ipGroup,ipGroupPrefix) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipGroup,ipSrc) is
*           (0,0).
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv6McEntryGetNext_GetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    CPSS_EXMX_IP_MC_ADDRESS_TYPE_ENT        ipGroupAddressType;
    GT_U32                                  ipGroupRuleIndex;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT         groupScope;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC    mcRoutePointer;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    gIpv6McInd = 0;
    vrId = 0;/*currently only 0 is valid*/
    gIpv6McGroup.u32Ip[0] = 0; /*for first entry 0 is entered*/
    gIpv6McGroup.u32Ip[1] = 0; /*for first entry 0 is entered*/
    gIpv6McGroup.u32Ip[2] = 0; /*for first entry 0 is entered*/
    gIpv6McGroup.u32Ip[3] = 0; /*for first entry 0 is entered*/
    gIpv6McSrc.u32Ip[0] = 0; /*for first entry 0 is entered*/
    gIpv6McSrc.u32Ip[1] = 0; /*for first entry 0 is entered*/
    gIpv6McSrc.u32Ip[2] = 0; /*for first entry 0 is entered*/
    gIpv6McSrc.u32Ip[3] = 0; /*for first entry 0 is entered*/
    gIpv6McSrcPrefixLen = 0; /*for first entry 0 is entered*/

    /* call cpss api function */
    result = cpssExMxIpLpmIpv6McEntrySearch(lpmDBId, vrId, gIpv6McGroup,&ipGroupAddressType,
                                            &ipGroupRuleIndex, gIpv6McSrc, gIpv6McSrcPrefixLen,
                                            &groupScope,&mcRoutePointer);
    if (result != GT_OK)
    {
        if(result == GT_NOT_FOUND)
        {/*if no more entries are left in the table.*/
            result = GT_OK;
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
        else
        {
            galtisOutput(outArgs, result, "");
            return result;
        }
    }

    /* pack and output table fields*/
    inFields[0] = gIpv6McInd;
    inFields[1] = vrId;
    inFields[3] = (CPSS_EXMX_IP_MC_ADDRESS_TYPE_ENT)ipGroupAddressType;
    inFields[4] = (GT_U32)ipGroupRuleIndex;
    inFields[6] = (GT_U32)gIpv6McSrcPrefixLen;
    inFields[7] = (CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT)groupScope;
    inFields[8] = (GT_U32)mcRoutePointer.routeEntryBaseMemAddr;
    inFields[9] = (GT_U32)mcRoutePointer.blockSize;
    inFields[10] =
        (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)mcRoutePointer.routeEntryMethod;

    /* pack and output table fields */
    fieldOutput("%d%d%16b%d%d%16b%d%d%d%d%d"
                ,inFields[0], inFields[1], gIpv6McGroup.arIP, inFields[3],
                inFields[4], gIpv6McSrc.arIP, inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10]);

    galtisOutput(outArgs, result, "%f");

    gIpv6McInd++;

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function returns the next muticast (ipSrc,ipGroup) entry, used
*       to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       TIGER
*
* INPUTS:
*       lpmDBId           - The LPM DB id.
*       vrId              - The virtual router Id.
*       ipGroupPtr        - The ip Group address to get the next entry for.
*       ipSrcPtr          - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr           - The next ip Group address.
*       ipGroupAddressTypePtr- the type of address this ip group is (mangement
*                              or regular)
*       ipGroupRuleIndexPtr  - index of the IPv6 mc group rule in the TCAM.
*       ipSrcPtr             - The next ip Source address.
*       ipSrcPrefixLenPtr    - ipSrc prefix length.
*       mcRoutePointerPtr    - the mc route entry ptr of the found mc mc route
*       groupScopePtr        - address scope of the  multicast group.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       1.  The values of (ipGroup,ipGroupPrefix) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipGroup,ipSrc) is
*           (0,0).
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv6McEntryGetNext_GetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    CPSS_EXMX_IP_MC_ADDRESS_TYPE_ENT        ipGroupAddressType;
    GT_U32                                  ipGroupRuleIndex;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT         groupScope;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC    mcRoutePointer;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = 0;/*currently only 0 is valid*/

    /* call cpss api function */
    result = cpssExMxIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &gIpv6McGroup,
                                             &ipGroupAddressType,
                                             &ipGroupRuleIndex, &gIpv6McSrc,
                                             &gIpv6McSrcPrefixLen, &groupScope,
                                             &mcRoutePointer);

    if (result != GT_OK)
    {
        if(result == GT_NOT_FOUND)
        {/*if no more entries are left in the table.*/
            result = GT_OK;
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
        else
        {
            galtisOutput(outArgs, result, "");
            return result;
        }
    }

    /* pack and output table fields*/
    inFields[0] = gIpv6McInd;
    inFields[1] = vrId;
    inFields[3] = (CPSS_EXMX_IP_MC_ADDRESS_TYPE_ENT)ipGroupAddressType;
    inFields[4] = (GT_U32)ipGroupRuleIndex;
    inFields[6] = (GT_U32)gIpv6McSrcPrefixLen;
    inFields[7] = (CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT)groupScope;
    inFields[8] = (GT_U32)mcRoutePointer.routeEntryBaseMemAddr;
    inFields[9] = (GT_U32)mcRoutePointer.blockSize;
    inFields[10] =
        (CPSS_EXMX_IP_ROUTE_ENTRY_METHOD_ENT)mcRoutePointer.routeEntryMethod;

    /* pack and output table fields */
    fieldOutput("%d%d%16b%d%d%16b%d%d%d%d%d"
                ,inFields[0], inFields[1], gIpv6McGroup.arIP, inFields[3],
                inFields[4], gIpv6McSrc.arIP, inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10]);

    galtisOutput(outArgs, result, "%f");

    gIpv6McInd++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmIpv6McEntrySearch
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
*       ipGroupAddressTypePtr- the type of address this ip group is (mangement
*                              or regular)
*       mcRoutePointerPtr    - the mc route entry ptr of the found mc route
*       groupScopePtr        - address scope of the multicast group.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmIpv6McEntrySearch
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                lpmDBId;
    GT_U32                                vrId;
    GT_IPV6ADDR                           ipGroup;
    CPSS_EXMX_IP_MC_ADDRESS_TYPE_ENT      ipGroupAddressType;
    GT_U32                                ipGroupRuleIndex;
    GT_IPV6ADDR                           ipSrc;
    GT_U32                                ipSrcPrefixLen;
    CPSS_EXMX_IPV6_PREFIX_SCOPE_ENT       groupScope;
    CPSS_EXMX_IP_ROUTE_ENTRY_POINTER_STC  mcRoutePointer;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipGroup, (GT_U8*)inArgs[2]);
    galtisIpv6Addr(&ipSrc, (GT_U8*)inArgs[3]);
    ipSrcPrefixLen = (GT_U32)inArgs[4];

    /* call cpss api function */
    status =  cpssExMxIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup,
                                             &ipGroupAddressType,
                                             &ipGroupRuleIndex, ipSrc,
                                             ipSrcPrefixLen, &groupScope,
                                             &mcRoutePointer);

    galtisOutput(outArgs, status, "%d%d%d%d%d%d",
                 ipGroupAddressType, ipGroupRuleIndex, groupScope,
                 mcRoutePointer.routeEntryBaseMemAddr,
                 mcRoutePointer.blockSize,
                 mcRoutePointer.routeEntryMethod);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxIpLpmDBMemSizeGet
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function gets the memory size needed to export the Lpm DB,
*       (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       numOfEntriesInStep - how many entries in each iteration step
*       lpmDbSizePtr       - the table size calculated up to now (start with 0)
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbSizePtr  - the table size calculated (in bytes)
*       iterPtr       - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK on success
*       GT_FAIL otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmDBMemSizeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32 lpmDBId;
    GT_U32 lpmDbSize;
    GT_U32 numOfEntriesInStep;
    GT_U32 iter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    lpmDbSize = (GT_U32)inArgs[1];
    numOfEntriesInStep = (GT_U32)inArgs[2];
    iter = (GT_U32)inArgs[3];

    /* call cpss api function */
    status =  cpssExMxIpLpmDBMemSizeGet(lpmDBId, &lpmDbSize, numOfEntriesInStep,&iter);

    galtisOutput(outArgs, status, "%d%d", lpmDbSize, iter);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmDBExport
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function exports the Lpm DB into the preallocated memory,
*       (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       numOfEntriesInStep - how many entries in each iteration step
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbPtr  - the lpm DB info block allocated
*       iterPtr   - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK on success
*       GT_FAIL otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmDBExport
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32 lpmDBId;
    GT_U32 *lpmDb;
    GT_U32 lpmDbSize=0;
    GT_U32 numOfEntriesInStep;
    GT_U32 iter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    numOfEntriesInStep = (GT_U32)inArgs[1];
    iter = (GT_U32)inArgs[2];

    /* cpssExMxIpLpmDbMemSizeGet is used to allocate exactly the amount
       of space needed for the Export.
       In this case the allocated space need to be freed after Importing the data,
       since calling the Export many time will result in an error (GT_OUT_OF_CPU_MEM).
       By freeing the space after the call to the Import, we will be unable to
       call the Import more then one time.
    */

    status =  cpssExMxIpLpmDBMemSizeGet(lpmDBId, &lpmDbSize, numOfEntriesInStep, &iter);
    if(status != GT_OK)
    {
        galtisOutput(outArgs, status, "");
        return CMD_OK;
    }

    lpmDb = cmdOsMalloc(lpmDbSize*sizeof(GT_U32));
    cmdOsMemSet(lpmDb, 0, lpmDbSize*sizeof(GT_U32));


    /* call cpss api function */
    status =  cpssExMxIpLpmDBExport(lpmDBId, lpmDb, numOfEntriesInStep, &iter);

    galtisOutput(outArgs, status, "%d%d", lpmDb, iter);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxIpLpmDBImport
*
* DESCRIPTION:
*   Function Relevant mode : High Level mode
*       This function imports the Lpm DB recived and reconstruct it,
*       (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*       TWIST, TWIST-D , SAMBA/RUMBA , TIGER
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       numOfEntriesInStep - how many entries in each iteration step
*       lpmDbPtr           - the lpm DB info block
*       iterPtr            - the iterator, to start - set to 0.
*
* OUTPUTS:
*       iterPtr   - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK on success
*       GT_FAIL otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxIpLpmDBImport
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32 lpmDBId;
    GT_U32 *lpmDb=NULL;
    GT_U32 numOfEntriesInStep;
    GT_U32 iter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    lpmDb = (GT_U32*)inArgs[1];
    numOfEntriesInStep = (GT_U32)inArgs[2];
    iter = (GT_U32)inArgs[3];

    /* call cpss api function */
    status =  cpssExMxIpLpmDBImport(lpmDBId, lpmDb, numOfEntriesInStep, &iter);

    /* allocated space need to be freed after Importing the data */

    cmdOsFree(lpmDb);
    galtisOutput(outArgs, status, "%d", iter);

    return CMD_OK;
}

static CMD_STATUS wrCpssExMxLpmDbDump
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
    result = cpssExMxIpLpmDump(devNum, vrId, isIpv4, isUnicast);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vrId);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxIpLpmDBCreateSetFirst",
        &wrCpssExMxIpLpmDBCreateSetFirst,
        4, 8},

    {"cpssExMxIpLpmDBCreateSetNext",
        &wrCpssExMxIpLpmDBCreateSetNext,
        4, 8},

    {"cpssExMxIpLpmDBCreateEndSet",
        &wrCpssExMxIpLpmDBCreateEndSet,
        4, 8},

    {"cpssExMxIpLpmDBDevListSetFirst",
        &wrCpssExMxIpLpmDBDevListAddSetFirst,
        1, 2},

    {"cpssExMxIpLpmDBDevListSetNext",
        &wrCpssExMxIpLpmDBDevListAddSetNext,
        1, 2},

    {"cpssExMxIpLpmDBDevListEndSet",
        &wrCpssExMxIpLpmDBDevListAddEndSet,
        1, 2},

    {"cpssExMxIpLpmDBDevListDelete",
        &wrCpssExMxIpLpmDBDevsListRemove,
        1, 2},

    {"cpssExMxIpLpmVirtualRouterSet",
        &wrCpssExMxIpLpmVirtualRouterSet,
        10, 0},

    {"cpssExMxIpLpmVirtualRouterDelete",
         &wrCpssExMxIpLpmVirtualRouterDelete,
         2, 0},

    {"cpssExMxIpLpmIpv4UcPrefixClear",
        &wrCpssExMxIpLpmIpv4UcPrefixesFlush,
        2, 0},

    {"cpssExMxIpLpmIpv4UcPrefixDelete",
        &wrCpssExMxIpLpmIpv4UcPrefixDel,
        2, 5},

    {"cpssExMxIpLpmIpv4UcPrefixSetFirst",
        &wrCpssExMxIpLpmIpv4UcPrefixAdd,
        2, 10},

    {"cpssExMxIpLpmIpv4UcPrefixSetNext",
        &wrCpssExMxIpLpmIpv4UcPrefixAdd,
        2, 10},

    {"cpssExMxIpLpmIpv4UcPrefixEndSet",
        &wrCpssExMxIpLpmIpv4UcPrefixEndSet,
        2, 0},

    {"cpssExMxIpLpmIpv4UcPrefixGetFirst",
        &wrCpssExMxIpLpmIpv4UcPrefixGetNext_GetFirst,
        2, 0},

    {"cpssExMxIpLpmIpv4UcPrefixGetNext",
        &wrCpssExMxIpLpmIpv4UcPrefixGetNext_GetNext,
        2, 0},

    {"cpssExMxIpLpmIpv4McSet",
        &wrCpssExMxIpLpmIpv4McEntryAdd,
        1, 9},

    {"cpssExMxIpLpmIpv4McDelete",
        &wrCpssExMxIpLpmIpv4McEntryDel,
        1, 9},

    {"cpssExMxIpLpmIpv4McClear",
        &wrCpssExMxIpLpmIpv4McEntriesFlush,
        1, 0},

    {"cpssExMxIpLpmIpv4McGetFirst",
        &wrCpssExMxIpLpmIpv4McEntryGetNext_GetFirst,
        1, 0},

    {"cpssExMxIpLpmIpv4McGetNext",
        &wrCpssExMxIpLpmIpv4McEntryGetNext_GetNext,
        1, 0},

    {"cpssExMxIpLpmIpv4McEntrySearch",
        &wrCpssExMxIpLpmIpv4McEntrySearch,
        5, 0},

    {"cpssExMxIpLpmIpv6UcPrefixSetFirst",
        &wrCpssExMxIpLpmIpV6UcPrefixAdd,
        2, 9},

    {"cpssExMxIpLpmIpv6UcPrefixSetNext",
        &wrCpssExMxIpLpmIpV6UcPrefixAdd,
        2, 9},

    {"cpssExMxIpLpmIpv6UcPrefixEndSet",
        &wrCpssExMxIpLpmIpv6UcPrefixEndSet,
        2, 0},

    {"cpssExMxIpLpmIpv6UcPrefixGetFirst",
        &wrCpssExMxIpLpmIpv6UcPrefixGetNext_GetFirst,
        2, 0},

    {"cpssExMxIpLpmIpv6UcPrefixGetNext",
        &wrCpssExMxIpLpmIpv6UcPrefixGetNext_GetNext,
        2, 0},

    {"cpssExMxIpLpmIpv6UcPrefixDelete",
        &wrCpssExMxIpLpmIpv6UcPrefixDel,
        2, 9},

    {"cpssExMxIpLpmIpv6UcPrefixClear",
        &wrCpssExMxIpLpmIpv6UcPrefixesFlush,
        2, 0},

    {"cpssExMxIpLpmIpv6McSet",
        &wrCpssExMxIpLpmIpv6McEntryAdd,
        1, 11},

    {"cpssExMxIpLpmIpv6McDelete",
        &wrCpssExMxIpLpmIpv6McEntryDel,
        1, 11},

    {"cpssExMxIpLpmIpv6McClear",
        &wrCpssExMxIpLpmIpv6McEntriesFlush,
        1, 0},

    {"cpssExMxIpLpmIpv6McGetFirst",
        &wrCpssExMxIpLpmIpv6McEntryGetNext_GetFirst,
        1, 0},

    {"cpssExMxIpLpmIpv6McGetNext",
        &wrCpssExMxIpLpmIpv6McEntryGetNext_GetNext,
        1, 0},

    {"cpssExMxIpLpmIpv6McEntrySearch",
        &wrCpssExMxIpLpmIpv6McEntrySearch,
        5, 0},

    {"cpssExMxIpLpmDBMemSizeGet",
        &wrCpssExMxIpLpmDBMemSizeGet,
        4, 0},

    {"cpssExMxIpLpmDBExport",
        &wrCpssExMxIpLpmDBExport,
        3, 0},

    {"cpssExMxIpLpmDBImport",
        &wrCpssExMxIpLpmDBImport,
        4, 0},

    {"cpssExMxLpmDump",
        &wrCpssExMxLpmDbDump,
        3, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxIpLpm
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
GT_STATUS cmdLibInitCpssExMxIpLpm
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}



