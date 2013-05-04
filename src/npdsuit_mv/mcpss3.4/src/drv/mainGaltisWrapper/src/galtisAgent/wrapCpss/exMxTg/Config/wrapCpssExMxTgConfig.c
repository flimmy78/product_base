/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapTgConfigCpss.c
*
* DESCRIPTION:
*       Wrapper functions for TgConfig cpss functions.
*
* DEPENDENCIES:
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
#include <cpss/exMx/exMxTg/config/cpssExMxTgCfgInit.h>
/*#include <galtisAgent/wrapCpss/exMx/wrapTgConfigCpss.h>*/

/*******************************************************************************
* cpssExMxTgCfgPpLogicalInit
*
* DESCRIPTION:
*       packet processor initialization configuration function.
*       This function Performs PP RAMs divisions to memory pools.
*
* APPLICABLE DEVICES:  98Ex1x6 devices
*
* INPUTS:
*       ppConfigPtr - (pointer to)includes PP specific logical initialization
*                     params.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory
*       GT_OUT_OF_PP_MEM  - the table sizes requested by application exceeds the
*                           PP RAM size
*       GT_FAIL - on error
*
* COMMENTS:
*
*       1.  This function should perform all PP initializations, which
*           includes:
*               -   Ram structs init, and Ram size check.
*               -   Init the AU (address update messages) mechanism according to
*                   the data.
*
*       Execution flow of function:
*
*
*             +-----------------+
*             | Init the 3 RAMs |   This initialization includes all structs
*             | conf. structs   |   fields but the base address field.
*             |                 |
*             +-----------------+
*                     |
*   (Ram is big       |                 +-------------------------+
*    enough to hold   +--(No)-----------| return GT_OUT_OF_PP_MEM |
*    all tables?)     |                 +-------------------------+
*                     |
*                   (Yes)
*                     |
*                     V
*             +-------------------+
*             | Set the Rams base |
*             | addr. according   |
*             | to the location   |
*             | fields.           |
*             +-------------------+
*                     |
*                     |
*                     V
*             +-----------------------+
*             | Init the internal CPSS|
*             | meta-data on RAM info |
*             | struct, according     |
*             | to the info in RAM    |
*             | conf. struct.         |
*             +-----------------------+
*                    |
*                    |
*                    V
*             +-----------------------+
*             | Init the AU mechanism |
*             | according to the data |
*             +-----------------------+
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxTgCfgPpLogicalInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    CPSS_EXMXTG_PP_CONFIG_INIT_STC  ppConfigPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ppConfigPtr.maxNumOfInlifs = (GT_U32)inArgs[1];
    ppConfigPtr.maxNumOfPolicerEntries = (GT_U32)inArgs[2];/* policer entries */
    ppConfigPtr.policerConformCountEn = (GT_BOOL)inArgs[3];/* policer entries */
    ppConfigPtr.maxNumOfNhlfe = (GT_U32)inArgs[4];/* MPLS */
    ppConfigPtr.maxNumOfMplsIfs = (GT_U32)inArgs[5];/* MPLS */
    ppConfigPtr.maxNumOfPclAction = (GT_U32)inArgs[6];/* PCL */
    ppConfigPtr.pclActionSize = (CPSS_EXMX_PCL_ACTION_SIZE_TYPE_ENT)inArgs[7];/* PCL */
    ppConfigPtr.maxVid = (GT_U16)inArgs[8];/*Lib init params. */
    ppConfigPtr.maxMcGroups = (GT_U16)inArgs[9];/*Lib init params. */
    ppConfigPtr.tunnelTableSize = (GT_U32)inArgs[10];/*Lib init params. */
    ppConfigPtr.maxNumOfVirtualRouters = (GT_U32)inArgs[11];/* IP Unicast     */
    ppConfigPtr.maxNumOfIpv4NextHop = (GT_U32)inArgs[12];/* IPv4 Unicast     */
    ppConfigPtr.maxNumOfIpv4Prefixes = (GT_U32)inArgs[13];/* IPv4 Unicast     */
    ppConfigPtr.maxNumOfMll = (GT_U32)inArgs[14];/* IP Multicast   */
    ppConfigPtr.maxNumOfIpv4McEntries = (GT_U32)inArgs[15];/* IPv4 Multicast   */
    ppConfigPtr.maxNumOfIpv6Prefixes = (GT_U32)inArgs[16];/* IPv6 Unicast     */
    ppConfigPtr.maxNumOfIpv6McGroups = (GT_U32)inArgs[17];/* IPv6 Multicast   */
    ppConfigPtr.maxNumOfTunnelEntries = (GT_U32)inArgs[18];/* Tunnels */
    ppConfigPtr.maxNumOfIpv4TunnelTerms = (GT_U32)inArgs[19];/* ipv4 tunnel termination */
    ppConfigPtr.vlanInlifEntryType = (CPSS_EXMX_VLAN_INLIF_ENTRY_TYPE_ENT)inArgs[20];/* inlifs */
    ppConfigPtr.ipMemDivisionOn = (GT_BOOL)inArgs[21];/* if GT_FALSE - no division of memory pools for the use of different IP versions */
    ppConfigPtr.ipv6MemShare = (GT_U32)inArgs[22];/* division of memory between IPv4 and IPv6 prefixes */



    /* call cpss api function */
    result = cpssExMxTgCfgPpLogicalInit(devNum, &ppConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxTgCfgPpLogicalInit",
        &wrcpssExMxTgCfgPpLogicalInit,
        23, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitCpssExMxTgConfig
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
GT_STATUS cmdLibInitCpssExMxTgConfig
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

