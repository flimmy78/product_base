/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapConfigCpss.c
*
* DESCRIPTION:
*       Wrapper functions for Config cpss functions.
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
#include <cpss/exMx/exMxGen/config/cpssExMxCfgInit.h>


/*******************************************************************************
* cpssExMxCfgDevRemove
*
* DESCRIPTION:
*       remove the device from the CPSS.
*
*       Model:
*       Application need to call "phase 1" , "phase 2" for a device that need to
*       use this deleted devNum.
*
*       Application may want to "replace" this remove device with another device
*       Or may want to "reset" the same device.
*
*       A "reset" of the same device can be with HW reset or without , but
*       application is responsible to reset HW tables , if not reset the HW.
*
*       NOTE: 1. Application may want to disable the Traffic to the CPU , and
*             messages , prior to this operation (if the device still exists).
*             2. this function will not do any HW read/write operations.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum - device number to remove.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - wrong dev
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCfgDevRemove
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxCfgDevRemove(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCfgDevEnable
*
* DESCRIPTION:
*       This routine enable/disable the device to/from processing any packets
*       While disable no traffic is received/transmitted from networks ports
*       or uplink port
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum - device number to set.
*       enable - GT_TRUE device enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong dev
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCfgDevEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxCfgDevEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCfgMacAddrSet
*
* DESCRIPTION:
*       Sets system Mac[48:8] MSB Addr on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev - device number
*       macPtr - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       mac[7:0] LSB is overridden by the per port MAC LSB configuration or
*       mac[11:0] LSB is overridden by packet's VID.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCfgDevMacAddrSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_ETHERADDR  macPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    galtisMacAddr(&macPtr, (GT_U8*)inArgs[1]);


    /* call cpss api function */
    result = cpssExMxCfgDevMacAddrSet(devNum, &macPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*{
    GT_U8       arEther[6];
  }GT_ETHERADDR;
;*/

/*******************************************************************************
* cpssExMxCfgDevMacAddrLsbModeSet
*
* DESCRIPTION:
*       Sets the mode in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       saLsbMode - The MAC SA least-significant bit mode
*       dev       - The system unique device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_SUPPORTED- if the feature does not supported by specified device
*       GT_FAIL         - on error
*
* COMMENTS:
*
*       The device 5 most significant bytes are set by cpssExMxCfgMacAddrSet().
*
*       If saLsbMode is Port Mode, the Port library function
*       cpssExMxPortMacSaLsbSet() sets the per port least significant byte.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCfgDevMacAddrLsbModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    saLsbMode = (CPSS_MAC_SA_LSB_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxCfgDevMacAddrLsbModeSet(dev, saLsbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCfgIngressTrafficToFabricEnable
*
* DESCRIPTION:
*       Configure the packet processors specified enable/disable
*       mode which forces all local ingress traffic to be sent to uplink.
*
*       When Enabled, all packets are transmitted to the fabric adapter,
*       except for trapped packets.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum - The PP's device number .
*       enable - GT_TRUE - to enable -> send ingress traffic to FA
*                GT_FALSE - to disable -> not send ingress traffic to FA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*      This mode is not applicable for standalone packet processor configuration
*       or back-2-back packet processor configurations.
*       When enabled, in order to allow traffic to be sent back to the packet
*       processor, the fabric adapter internal cross bar fabric port 4 must be
*       enable for local switching as well.use XBAR API(s)
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCfgIngressTrafficToFabricEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxCfgIngressTrafficToFabricEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxCfgTrapToFabricEnable
*
* DESCRIPTION:
*       Enable/Disable the trapped traffic to the Uplink (FA) instead of local
*       CPU
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum - The PP's device number .
*       enable - GT_TRUE - to enable -> send trapped traffic to FA
*                GT_FALSE - to disable -> send trapped traffic to local CPU
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCfgTrapToFabricEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxCfgTrapToFabricEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxCfgPpLogicalInit
*
* DESCRIPTION:
*       packet processor initialization configuration function.
*       This function Performs PP RAMs divisions to memory pools.
*
* APPLICABLE DEVICES:  All ExMx devices , exclude 98Ex1x6 devices
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
*             | Init the   RAMs |   This initialization includes all structs
*             | conf. structs   |   fields but the base address field.
*             |                 |   The Rams are : internal Ram , Narrow Sram ,
*             |                 |   Wide Sram (Flow Dram - Mx devices).
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
static CMD_STATUS wrCpssExMxCfgPpLogicalInit

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                         devNum;
    CPSS_EXMX_PP_CONFIG_INIT_STC  ppConfigPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ppConfigPtr.maxNumOfInlifs = (GT_U32)inArgs[1];
    ppConfigPtr.maxNumOfPolicerEntries = (GT_U32)inArgs[2];
    ppConfigPtr.policerConformCountEn = (GT_BOOL)inArgs[3];
    ppConfigPtr.maxNumOfNhlfe = (GT_U32)inArgs[4];
    ppConfigPtr.maxNumOfMplsIfs = (GT_U32)inArgs[5];
    ppConfigPtr.maxNumOfPclAction = (GT_U32)inArgs[6];
    ppConfigPtr.pclActionSize = (CPSS_EXMX_PCL_ACTION_SIZE_TYPE_ENT)inArgs[7];
    ppConfigPtr.maxVid = (GT_U16)inArgs[8];
    ppConfigPtr.maxMcGroups = (GT_U16)inArgs[9];
    ppConfigPtr.tunnelTableSize = (GT_U32)inArgs[10];
    ppConfigPtr.maxNumOfVirtualRouters = (GT_U32)inArgs[11];
    ppConfigPtr.maxNumOfIpv4NextHop = (GT_U32)inArgs[12];
    ppConfigPtr.maxNumOfIpv4Prefixes = (GT_U32)inArgs[13];
    ppConfigPtr.maxNumOfMll = (GT_U32)inArgs[14];
    ppConfigPtr.maxNumOfIpv4McEntries = (GT_U32)inArgs[15];
    ppConfigPtr.maxNumOfIpv6Prefixes = (GT_U32)inArgs[16];
    ppConfigPtr.maxNumOfIpv6McGroups = (GT_U32)inArgs[17];
    ppConfigPtr.maxNumOfTunnelEntries = (GT_U32)inArgs[18];
    ppConfigPtr.maxNumOfIpv4TunnelTerms = (GT_U32)inArgs[19];
    ppConfigPtr.vlanInlifEntryType = (GT_U32)inArgs[20];
    ppConfigPtr.ipMemDivisionOn = (GT_BOOL)inArgs[21];
    ppConfigPtr.ipv6MemShare = (CPSS_EXMX_VLAN_INLIF_ENTRY_TYPE_ENT)inArgs[22];


    /* call cpss api function */
    result = cpssExMxCfgPpLogicalInit(devNum, &ppConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxCfgDevRemove",
        &wrcpssExMxCfgDevRemove,
        1, 0},

    {"cpssExMxCfgDevEnable",
        &wrcpssExMxCfgDevEnable,
        2, 0},

     {"cpssExMxCfgDevMacAddrSet",
        &wrcpssExMxCfgDevMacAddrSet,
       2, 0},

    {"cpssExMxCfgDevMacAddrLsbModeSet",
        &wrcpssExMxCfgDevMacAddrLsbModeSet,
        2, 0},

    {"cpssExMxCfgIngressTrafficToFabricEnable",
        &wrcpssExMxCfgIngressTrafficToFabricEnable,
        2, 0},

    {"cpssExMxCfgTrapToFabricEnable",
        &wrcpssExMxCfgTrapToFabricEnable,
        2, 0},

    {"cpssExMxCfgPpLogicalInit",
        &wrCpssExMxCfgPpLogicalInit,
        23, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxConfig
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
GT_STATUS cmdLibInitCpssExMxConfig
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}




