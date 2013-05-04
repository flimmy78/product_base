/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgFdbHash.c
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
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgFdbHash.h>

/*******************************************************************************
* cpssExMxPmBrgFdbInternalHashCalc
*
* DESCRIPTION:
*       This function calculates the hash index for the FDB table.
*       The FDB table hold 2 types of entries :
*       1. for specific mac address and VLAN id.
*       2. for specific src IP , dst IP and VLAN id.
*
*       for more details see CPSS_MAC_ENTRY_EXT_KEY_STC description.
*
*       Used only for internal memory.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
*
* INPUTS:
*       devNum          - device number
*       fdbEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       fdbHashIndexPtr - (pointer to) the hash index.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong devNum, bad vlan-Id
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbInternalHashCalc
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC       fdbEntryKey;
    GT_BYTE_ARRY                        sipBArr, dipBArr;
    GT_U32                              fdbHashIndex;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    fdbEntryKey.entryType = (CPSS_EXMXPM_FDB_ENTRY_TYPE_ENT)inArgs[0];

    switch(inArgs[1])
    {
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:

        galtisMacAddr(&fdbEntryKey.key.macFid.macAddr,
                        (GT_U8*)inArgs[2]);
        fdbEntryKey.key.macFid.fId = (GT_U32)inArgs[3];
        break;

    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:

        fdbEntryKey.key.ipMcast.fId = (GT_U32)inArgs[3];

        galtisBArray(&sipBArr, (GT_U8*)inArgs[4]);

        cmdOsMemCpy(
            fdbEntryKey.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inArgs[5]);

        cmdOsMemCpy(
            fdbEntryKey.key.ipMcast.dip, dipBArr.data, 4);
        break;

    default:
        break;
    }

    /* call cpss api function */
    result = cpssExMxPmBrgFdbInternalHashCalc(devNum, &fdbEntryKey, &fdbHashIndex);

    /* send the value of the hash index */
    fieldOutput("%d",fdbHashIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgFdbExternalHashCalc
*
* DESCRIPTION:
*       This function calculates the hash index for the LUT and FDB external
*       tables.
*       The FDB table hold 2 types of entries :
*       1. for specific mac address and VLAN id.
*       2. for specific src IP , dst IP and VLAN id.
*
*       for more details see CPSS_MAC_ENTRY_EXT_KEY_STC description.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
*
* INPUTS:
*       devNum      - device number
*       fdbEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       lutHashIndexPtr - (pointer to) the hash index in LU table.
*       fdbHashIndexPtr - (pointer to) the hash index in the FDB table
*       lutSecHashValuePtr - (pointer to) the secondary hash index
*                            (bits [23:31] from the CRC32 hashing)
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong devNum, bad vlan-Id
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgFdbExternalHashCalc
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_EXMXPM_FDB_ENTRY_KEY_STC       fdbEntryKey;
    GT_BYTE_ARRY                        sipBArr, dipBArr;
    GT_U32                              lutHashIndex;
    GT_U32                              lutSecHashValue;
    GT_U32                              fdbHashIndex;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    fdbEntryKey.entryType = (CPSS_EXMXPM_FDB_ENTRY_TYPE_ENT)inArgs[1];

    switch(inArgs[1])
    {
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_MAC_ADDR_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_VPLS_E:

        galtisMacAddr(&fdbEntryKey.key.macFid.macAddr,
                        (GT_U8*)inArgs[2]);
        fdbEntryKey.key.macFid.fId = (GT_U32)inArgs[3];
        break;

    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV4_MCAST_E:
    case CPSS_EXMXPM_FDB_ENTRY_TYPE_IPV6_MCAST_E:

        fdbEntryKey.key.ipMcast.fId = (GT_U32)inArgs[3];

        galtisBArray(&sipBArr, (GT_U8*)inArgs[4]);

        cmdOsMemCpy(
            fdbEntryKey.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inArgs[5]);

        cmdOsMemCpy(
            fdbEntryKey.key.ipMcast.dip, dipBArr.data, 4);
        break;

    default:
        break;
    }

    /* call cpss api function */
    result = cpssExMxPmBrgFdbExternalHashCalc(devNum, &fdbEntryKey, &lutHashIndex, &lutSecHashValue, &fdbHashIndex);

    /* send the value of the hash index */
    fieldOutput("%d%d%d",lutHashIndex,lutSecHashValue,fdbHashIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmBrgFdbInternalHashCalc",
         &wrCpssExMxPmBrgFdbInternalHashCalc,
         6, 0},
        {"cpssExMxPmBrgFdbExternalHashCalc",
         &wrCpssExMxPmBrgFdbExternalHashCalc,
         6, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmBrgFdbHash
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
GT_STATUS cmdLibInitCpssExMxPmBrgFdbHash
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

