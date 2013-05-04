/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssGenSmi.c
*
* DESCRIPTION:
*       Wrapper functions for API for read/write register of device, which 
*           connected to SMI master controller of packet processor
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
#include <cpss/generic/phy/cpssGenPhySmi.h>
#include <cpss/generic/smi/cpssGenSmi.h>

/*******************************************************************************
* cpssSmiRegisterRead
*
* DESCRIPTION:
*      The function reads register of a device, which connected to SMI master 
*           controller of packet processor
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       data - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssSmiRegisterRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    CPSS_PHY_SMI_INTERFACE_ENT   smiInterface;
    GT_U32  smiAddr, regAddr, data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    smiInterface = (CPSS_PHY_SMI_INTERFACE_ENT)inArgs[2];
    smiAddr = (GT_U32)inArgs[3];
    regAddr = (GT_U32)inArgs[4];

    result = cpssSmiRegisterRead(devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, &data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "0x%08x", data);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssSmiRegisterWrite
*
* DESCRIPTION:
*      The function reads register of a device, which connected to SMI master 
*           controller of packet processor
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - register address
*       data - pointer to place data from read operation
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssSmiRegisterWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    CPSS_PHY_SMI_INTERFACE_ENT   smiInterface;
    GT_U32  smiAddr, regAddr, data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    smiInterface = (CPSS_PHY_SMI_INTERFACE_ENT)inArgs[2];
    smiAddr = (GT_U32)inArgs[3];
    regAddr = (GT_U32)inArgs[4];
    data = (GT_U32)inArgs[5];

    result = cpssSmiRegisterWrite(devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssSmiRegisterRead",
        &wrCpssSmiRegisterRead,
        5, 0},

    {"cpssSmiRegisterWrite",
        &wrCpssSmiRegisterWrite,
        6, 0}
};


#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))



/*******************************************************************************
* cmdLibInitCpssGenSmi
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
GT_STATUS cmdLibInitCpssGenSmi
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

