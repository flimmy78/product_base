/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapPhyCpss.c
*
* DESCRIPTION:
*       Wrapper functions for Phy cpss functions.
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
#include <cpss/exMx/exMxGen/phy/cpssExMxPhySmi.h>


/*******************************************************************************
* cpssExMxPhyPortSmiRegisterRead
*
* DESCRIPTION:
*       Read specified SMI Register on a specified port on specified device.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       phyReg - SMI register
*
* OUTPUTS:
*       data  - data read.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAMS - on invalid device number or port number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPhyPortSmiRegisterRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  dev;
    GT_U8  port;
    GT_U8  phyReg;
    GT_U16 dataPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    phyReg = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPhyPortSmiRegisterRead(dev, port, phyReg, &dataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPhyPortRegisterWrite
*
* DESCRIPTION:
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       phyReg - SMI register
*       data   - value to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAMS - on invalid device number or port number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPhyPortRegisterWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  dev;
    GT_U8  port;
    GT_U8  phyReg;
    GT_U16 data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    phyReg = (GT_U8)inArgs[2];
    data = (GT_U16)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPhyPortSmiRegisterWrite(dev, port, phyReg, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPhyPort10GSmiRegisterRead
*
* DESCRIPTION:
*       Read specified 10G SMI Register from PHY device
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       phyId  - ID of external 10G PHY
*       useExternalPhy - boolean variable, defines if to use external 10G PHY
*       phyReg - 10G SMI register, the register of PHY to read from
*       phyDev - the PHY device to read from
*
* OUTPUTS:
*       dataPtr  - data read.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAMS - on invalid port type
*       GT_BAD_PTR - on NULL address
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPhyPort10GSmiRegisterRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     port;
    GT_U8     phyId;
    GT_BOOL   useExternalPhy;
    GT_U16    phyReg;
    GT_U8     phyDev;
    GT_U16    dataPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    phyId = (GT_U8)inArgs[2];
    useExternalPhy = (GT_BOOL)inArgs[3];
    phyReg = (GT_U16)inArgs[4];
    phyDev = (GT_U8)inArgs[5];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPhyPort10GSmiRegisterRead(dev, port, phyId, useExternalPhy, phyReg, phyDev, &dataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPhyPort10GSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to specified 10G SMI Register to PHY device
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       phyId  - ID of external 10G PHY
*       useExternalPhy - boolean variable, defines if to use external 10G PHY
*       phyReg - 10G SMI register, the register of PHY to read from
*       phyDev - the PHY device to read from
*       data  - data read.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAMS - on invalid port type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPhyPort10GSmiRegisterWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  dev;
    GT_U8  port;
    GT_U8  phyId;
    GT_BOOL   useExternalPhy;
    GT_U16    phyReg;
    GT_U8     phyDev;
    GT_U16    data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    phyId = (GT_U8)inArgs[2];
    useExternalPhy = (GT_BOOL)inArgs[3];
    phyReg = (GT_U16)inArgs[4];
    phyDev = (GT_U8)inArgs[5];
    data = (GT_U16)inArgs[6];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPhyPort10GSmiRegisterWrite(dev, port, phyId, useExternalPhy, phyReg, phyDev, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortPhyAddrSet
*
* DESCRIPTION:
*       Configure the port's default phy address, this function should be used
*       to change the default port's phy address.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum  - The Pp device number.
*       portNum - The port number to update the phy address for.
*       phyAddr - The new phy address, only the lower 5 bits of this param are
*                 relevant.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAMS - on invalid device number or port number
*
* COMMENTS:
*       This function should be called after corePpHwPhase1Init(), and before
*       gtPortSmiInit() (in Tapi level).
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPhyPortAddrSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_U8   portNum;
    GT_U8   phyAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    phyAddr = (GT_U8)inArgs[2];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPhyPortAddrSet(devNum, portNum, phyAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPhyPortSmiInit
*
* DESCRIPTION:
*       Init port SMI facility.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_HW_ERROR       - on hardware error
*       GT_OUT_OF_CPU_MEM - oaMalloc failed
*       GT_BAD_PARAMS - on invalid device number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPhyPortSmiInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];


    /* call cpss api function */
    result = cpssExMxPhyPortSmiInit(dev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxPhyPortSmiRegisterRead",
        &wrcpssExMxPhyPortSmiRegisterRead,
        3, 0},

    {"cpssExMxPhyPortRegisterWrite",
        &wrcpssExMxPhyPortRegisterWrite,
        4, 0},

     {"cpssExMxPhyPort10GSmiRegisterRead",
        &wrcpssExMxPhyPort10GSmiRegisterRead,
        6, 0},

    {"cpssExMxPhyPort10GSmiRegisterWrite",
        &wrcpssExMxPhyPort10GSmiRegisterWrite,
        7, 0},

    {"cpssExMxPhyPortAddrSet",
        &wrcpssExMxPhyPortAddrSet,
        3, 0},

    {"cpssExMxPhyPortSmiInit",
        &wrcpssExMxPhyPortSmiInit,
        1, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))



/*******************************************************************************
* cmdLibInitCpssExMxPhy
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
GT_STATUS cmdLibInitCpssExMxPhy
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}






