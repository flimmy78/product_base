/*******************************************************************************
*
*                   Copyright 2002, GALILEO TECHNOLOGY, LTD.
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.
*
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES,
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).
*
*******************************************************************************
* wrapFe200.c
*
* DESCRIPTION:
*       Wrappers for Dune FE200.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <SAND/Utils/include/sand_os_interface.h>
#include <FE200/include/fe200_api_general.h>
#include <FE200/include/fe200_api_statistics.h>
#include <FE200/include/fe200_api_links.h>
#include <FE200/include/fe200_api_routing.h>

#define GT_U_I unsigned int

/*
 * typedef: enum FE200_DIST_REPLIC_ENTRY_TYPE_ENT
 *
 * Description: Enumeration of multicast entry type
 *
 * Enumerations:
 *      FE1_DIST_MULTI_SHORT_ENTRY_E   - short format entry in FE1 multicast
 *                                       distribution table
 *      FE1_DIST_MULTI_LONG_ENTRY_E    - long format entry in FE1 multicast
 *                                       distribution table
 *      FE2_REPLIC_MULTI_SHORT_ENTRY_E - short format entry in FE2 multicast
 *                                       replication table
 *      FE2_REPLIC_MULTI_LONG_ENTRY_E  - long format entry in FE2 multicast
 *                                       replication table
 *      FE3_REPLIC_MULTI_SHORT_ENTRY_E - short format entry in FE3 multicast
 *                                       replication table
 *      FE3_REPLIC_MULTI_LONG_ENTRY_E  - long format entry in FE3 multicast
 *                                       replication table
 */
typedef enum
{
  FE1_DIST_MULTI_SHORT_ENTRY_E,
  FE1_DIST_MULTI_LONG_ENTRY_E,
  FE2_REPLIC_MULTI_SHORT_ENTRY_E,
  FE2_REPLIC_MULTI_LONG_ENTRY_E,
  FE3_REPLIC_MULTI_SHORT_ENTRY_E,
  FE3_REPLIC_MULTI_LONG_ENTRY_E
} FE200_DIST_REPLIC_ENTRY_TYPE_ENT;


/* index of table entry */
static GT_U_I index;

/* stores link status table */
static ALL_LINK_STATUS linksStatus;

/* stores link connectivity table */
static FE200_ALL_LINK_CONNECTIVITY linksConnectivity;

/* flag indicating whether any of the links reported an error */
static GT_U_I errorInSomeFlag;

/* Bitmap. Indication on whether any of the links has changed since last
   report. */
static GT_U64 changeInSome;

/* Bitmap. Indication on whether any of the links contains report on error
   condition. */
static GT_U64 errorInSome;

 /* Galtis result (conversion of SAND error word) */
static SAND_RET tableResult;

/* SAND error word for tables */
static GT_U32 tableRetStatus;

/* SerDes status */
static FE200_ALL_SERDES_STATUS *serdesStatus = NULL;

/* MAC Cell Counters */
static MAC_COUNTERS_REPORT *macCountersReport;

/*******************************************************************************
* wrFe200LogicalRead (general command)
*
* DESCRIPTION:
*     Read chip registers using logical identifier (which specify chip
*     'objects').
*
* INPUTS:
*     [0] GT_U_I deviceId   - device to access.
*     [1] GT_U32 logicalId  - logical object to read.
*
* OUTPUTS:
*     [0] GT_U32 value      - Logical object value.
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrFe200LogicalRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;
    GT_U32 logicalId;
    GT_U32 value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    deviceId = (GT_U_I)inArgs[0];
    logicalId = (GT_U32)inArgs[1];

    /* call Dune api function */
    retStatus = fe200_logical_read (deviceId, &value, logicalId);
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%x", value);
    return CMD_OK;
}

/*******************************************************************************
* wrFe200LogicalWrite (general command)
*
* DESCRIPTION:
*     Write into chip registers using logical identifier (which specify chip
*     'objects').
*
* INPUTS:
*     [0] GT_U_I deviceId   - device to access.
*     [1] GT_U32 logicalId  - logical object to write (update).
*     [2] GT_U32 value      - Logical object value.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrFe200LogicalWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;
    GT_U32 logicalId;
    GT_U32 value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    deviceId = (GT_U_I)inArgs[0];
    logicalId = (GT_U32)inArgs[1];
    value = (GT_U32)inArgs[2];

    /* call Dune api function */
    retStatus = fe200_logical_write (deviceId, &value, logicalId);
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrFe200SetMacCounterType (general command)
*
* DESCRIPTION:
*     Select the type of cells that are going to be counted by the MAC cell
*     counters.
*
* INPUTS:
*     [0] GT_U_I deviceId                        - device to access.
*     [1] FE200_MAC_COUNTER_TYPE_ENT txCountType - Type of counter to activate
*                                                  on the TRANSMIT direction
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrFe200SetMacCounterType
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;
    GT_U_I txCountType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    deviceId = (GT_U_I)inArgs[0];
    txCountType = (GT_U_I)inArgs[1];

    /* call Dune api function */
    retStatus = fe200_set_mac_counter_type (deviceId, txCountType);
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrFe200GetMacCounterType (general command)
*
* DESCRIPTION:
*     Get the type of cells that are being counted by the MAC cell counters.
*
* INPUTS:
*     [0] GT_U_I deviceId                        - device to access.
*
* OUTPUTS:
*     [0] FE200_MAC_COUNTER_TYPE_ENT txCountType - Type of counter to activate
*                                                  on the TRANSMIT direction
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrFe200GetMacCounterType
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;
    GT_U_I txCountType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    deviceId = (GT_U_I)inArgs[0];

    /* call Dune api function */
    retStatus = fe200_get_mac_counter_type (deviceId, &txCountType);
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", txCountType);
    return CMD_OK;
}

/*******************************************************************************
* wrFe200StartMacCountersCollection (general command)
*
* DESCRIPTION:
*     Activate TCM task that counts device mac cell counters into local
*     structures.
*
* INPUTS:
*     [0] GT_U_I deviceId                        - device to access.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrFe200StartMacCountersCollection
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    deviceId = (GT_U_I)inArgs[0];

    /* call Dune api function */
    retStatus = fe200_start_mac_counters_collection (deviceId);
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrFe200StopMacCountersCollection (general command)
*
* DESCRIPTION:
*     Disable mac counter collection.
*
* INPUTS:
*     [0] GT_U_I deviceId                        - device to access.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrFe200StopMacCountersCollection
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    deviceId = (GT_U_I)inArgs[0];

    /* call Dune api function */
    retStatus = fe200_stop_mac_counters_collection (deviceId);
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*
* Table: fe200LinkStatusTable
*
* Description:
*      FE200 link Status Table.
*
* Fields:
*  [0] GT_U_I linkNumber -
*          Index number of the link
*  [1] GT_BOOL valid -
*          whether this link status entry is meaningful
*  [2] GT_U_I leakyBucketCounter -
*          Value of leaky bucket counter of corresponding Serdes. Range: 0-63
*  [3] GT_BOOL crcError -
*          Flag. Indication on whether a cell with CRC error has been detected
*          on this link.
*  [4] GT_BOOL misalignmentError -
*          Flag. Indication on whether a cell with misalignment error (comma)
*          has been setected on this link.
*  [5] GT_BOOL codeGroupError -
*          Flag. Indication on whether a cell with code-group combination error
*          (comma) has been detected on this link.
*  [6] GT_BOOL sigLock -
*          Flag. Indication on whether valid signal has been locked on this link
*  [7] GT_BOOL acceptingCells -
*          Flag. Indication on what how this device perceives the link
*          concerning status of this link ('mask' register, NOT 'aclg').
*
* Comments:
*     None.
*/

/*******************************************************************************
* wrFe200LinkStatusGetFirst (table command)
*
* DESCRIPTION:
*     Get link status table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId        - device to access.
*     [1] GT_U32 listOfLinksLow  - Bitmap low part. Indication on which links
*                                  to monitor and get status
*     [2] GT_U32 listOfLinksHigh - Bitmap high part. Indication on which links
*                                  to monitor and get status
*
* OUTPUTS:
*     [0] GT_BOOL errorInSomeFlag - Flag. Indication on whether any of the link
*                       array elements below is reporting an error condition.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200LinkStatusGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U_I deviceId;
    GT_U32 listOfLinks[2];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    deviceId = (GT_U_I)inArgs[0];
    listOfLinks[1] = (GT_U_I)inArgs[1];
    listOfLinks[0] = (GT_U_I)inArgs[2];

    sand_os_memset (&linksStatus, 0, sizeof (ALL_LINK_STATUS));
    tableRetStatus = fe200_get_links_status (deviceId, &linksStatus,
                                             listOfLinks);
    tableResult = (SAND_RET)sand_get_error_code_from_error_word(tableRetStatus);
    errorInSomeFlag = linksStatus.error_in_some;
    inFields[0] = index = 0;
    inFields[1] = linksStatus.single_link_status[index].valid;
    inFields[2] = linksStatus.single_link_status[index].leaky_bucket_counter;
    inFields[3] = linksStatus.single_link_status[index].crc_error;
    inFields[4] = linksStatus.single_link_status[index].misalignment_error;
    inFields[5] = linksStatus.single_link_status[index].code_group_error;
    inFields[6] = linksStatus.single_link_status[index].sig_lock;
    inFields[7] = linksStatus.single_link_status[index].accepting_cells;

    /* print table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
               inFields[3], inFields[4], inFields[5], inFields[6], inFields[7]);
    galtisOutput(outArgs, tableResult, "%d%f", errorInSomeFlag);
    return CMD_OK;
}

/*******************************************************************************
* wrFe200LinkStatusGetNext (table command)
*
* DESCRIPTION:
*     Get link status table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId        - device to access.
*     [1] GT_U32 listOfLinksLow  - Bitmap low part. Indication on which links
*                                  to monitor and get status
*     [2] GT_U32 listOfLinksHigh - Bitmap high part. Indication on which links
*                                  to monitor and get status
*
* OUTPUTS:
*     [0] GT_BOOL errorInSomeFlag - Flag. Indication on whether any of the link
*                       array elements below is reporting an error condition.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200LinkStatusGetNext
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

    /* check if end of table */
    if (++index == FE200_MAX_NUM_FE2_LINKS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = index;
    inFields[1] = linksStatus.single_link_status[index].valid;
    inFields[2] = linksStatus.single_link_status[index].leaky_bucket_counter;
    inFields[3] = linksStatus.single_link_status[index].crc_error;
    inFields[4] = linksStatus.single_link_status[index].misalignment_error;
    inFields[5] = linksStatus.single_link_status[index].code_group_error;
    inFields[6] = linksStatus.single_link_status[index].sig_lock;
    inFields[7] = linksStatus.single_link_status[index].accepting_cells;

    /* print table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
               inFields[3], inFields[4], inFields[5], inFields[6], inFields[7]);
    galtisOutput(outArgs, tableResult, "%d%f", errorInSomeFlag);
    return CMD_OK;
}

/*
* Table: fe200ConnectivityMapTable
*
* Description:
*      FE200 Connectivity Map Table.
*
* Fields:
*  [0] GT_U_I linkNumber -
*          Index number of the link
*  [1] GT_BOOL acceptingCells -
*          Flag. Indication on what how this device perceives the link
*          concerning status of this link ('mask' register, NOT 'aclg').
*  [2] GT_U_I otherChipId -
*          Chip id of entity on the other side of this link. Range: 0-2047.
*  [3] GT_U_I otherEntityType -
*          Type of entity on the other side of this link.
*  [4] GT_BOOL otherEntityIllegal  -
*          Flag. Indication on whether entity on other side is legal with
*          respect to fabric logic.
*  [5] GT_BOOL otherLink -
*          Indication on what link this device is viewed from the device on the
*          other side (link number on other device, which is used for sending
*          cells to this device). Range: 0-63.
*
* Comments:
*     None.
*/

/*******************************************************************************
* wrFe200ConnectivityMapGetFirst (table command)
*
* DESCRIPTION:
*     Get link status table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId      - device to access.
*
* OUTPUTS:
*     [0] GT_U64 changeInSome  - Bitmap. Indication on whether any of the links
*                                has changed since last report.
*     [1] GT_U64 errorInSome   - Bitmap. Indication on whether any of the links
*                                contains report on error condition.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200ConnectivityMapGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U_I deviceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    deviceId = (GT_U_I)inArgs[0];

    sand_os_memset (&linksConnectivity, 0, sizeof(FE200_ALL_LINK_CONNECTIVITY));
    tableRetStatus = fe200_get_connectivity_map (deviceId, &linksConnectivity);
    tableResult = (SAND_RET)sand_get_error_code_from_error_word(tableRetStatus);
    changeInSome.l[0] = linksConnectivity.change_in_some[1];
    changeInSome.l[1] = linksConnectivity.change_in_some[0];
    errorInSome.l[0] = linksConnectivity.error_in_some[1];
    errorInSome.l[1] = linksConnectivity.error_in_some[0];
    inFields[0] = index = 0;
    inFields[1] =
        linksConnectivity.single_link_connectivity[index].accepting_cells;
    inFields[2] =
        linksConnectivity.single_link_connectivity[index].other_chip_id;
    inFields[3] =
        linksConnectivity.single_link_connectivity[index].other_entity_type;
    inFields[4] =
        linksConnectivity.single_link_connectivity[index].other_entity_illegal;
    inFields[5] =
        linksConnectivity.single_link_connectivity[index].other_link;

    /* print table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5]);
    galtisOutput(outArgs, tableResult, "%s%s%f", galtisU64COut(&changeInSome),
                 galtisU64COut(&errorInSome));
    return CMD_OK;
}

/*******************************************************************************
* wrFe200ConnectivityMapGetNext (table command)
*
* DESCRIPTION:
*     Get link status table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId      - device to access.
*
* OUTPUTS:
*     [0] GT_U64 changeInSome  - Bitmap. Indication on whether any of the links
*                                has changed since last report.
*     [1] GT_U64 errorInSome   - Bitmap. Indication on whether any of the links
*                                contains report on error condition.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200ConnectivityMapGetNext
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

    /* check if end of table */
    if (++index == FE200_MAX_NUM_FE2_LINKS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = index;
    inFields[1] =
        linksConnectivity.single_link_connectivity[index].accepting_cells;
    inFields[2] =
        linksConnectivity.single_link_connectivity[index].other_chip_id;
    inFields[3] =
        linksConnectivity.single_link_connectivity[index].other_entity_type;
    inFields[4] =
        linksConnectivity.single_link_connectivity[index].other_entity_illegal;
    inFields[5] =
        linksConnectivity.single_link_connectivity[index].other_link;

    /* print table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5]);
    galtisOutput(outArgs, tableResult, "%s%s%f", galtisU64COut(&changeInSome),
                 galtisU64COut(&errorInSome));
    return CMD_OK;
}

/*
* Table: fe200SerdesParamsTable
*
* Description:
*      FE200 SerDes Parameters Table.
*
* Fields:
*  [0] GT_U_I linkNumber -
*          Index number of the link
*  [1] GT_BOOL valid -
*          serdes status entry is meaningful
*  [2] GT_U_I commonMode -
*          Output driver common mode select - Vcms.
*  [3] GT_U_I mainControl -
*          Output driver main control  - Men.
*  [4] GT_U_I preEmphasis -
*          Output driver pre emphasis  - Pen.
*  [5] GT_U_I amplitude -
*          Amplitude - Amp.
*  [6] GT_U_I powerDownRxTx -
*          Power down both rx and tx blocks leaving tbg running.
*
* Comments:
*     None.
*/

/*******************************************************************************
* wrFe200SerdesParamsGetFirst (table command)
*
* DESCRIPTION:
*     Get SerDes parameters table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId      - device to access.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200SerdesParamsGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    deviceId = (GT_U_I)inArgs[0];

    if (NULL == serdesStatus)
    {
        serdesStatus = (FE200_ALL_SERDES_STATUS*)cmdOsStaticMalloc(
                    (sizeof(FE200_ALL_SERDES_STATUS)));
    }

    index = 0;
    serdesStatus->single_serdes_status[index].valid = 1;

    retStatus = fe200_get_serdes_params (deviceId, (GT_U32*)serdesStatus);
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);

    inFields[0] = index;
    inFields[1] = serdesStatus->single_serdes_status[index].valid;
    inFields[2] = serdesStatus->single_serdes_status[index].common_mode;
    inFields[3] = serdesStatus->single_serdes_status[index].main_control;
    inFields[4] = serdesStatus->single_serdes_status[index].pre_emphasis;
    inFields[5] = serdesStatus->single_serdes_status[index].amplitude;
    inFields[6] = serdesStatus->single_serdes_status[index].power_down_rx_tx;

    /* print table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5], inFields[6]);
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrFe200SerdesParamsGetNext (table command)
*
* DESCRIPTION:
*     Get SerDes parameters table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId      - device to access.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200SerdesParamsGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check if end of table */
    if (++index == FE200_NUM_OF_SERDESES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    deviceId = (GT_U_I)inArgs[0];

    serdesStatus->single_serdes_status[index].valid = 1;

    retStatus = fe200_get_serdes_params (deviceId, (GT_U32*)serdesStatus);
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);
    inFields[0] = index;
    inFields[1] =
        serdesStatus->single_serdes_status[index].valid;
    inFields[2] =
        serdesStatus->single_serdes_status[index].common_mode;
    inFields[3] =
        serdesStatus->single_serdes_status[index].main_control;
    inFields[4] =
        serdesStatus->single_serdes_status[index].pre_emphasis;
    inFields[5] =
        serdesStatus->single_serdes_status[index].amplitude;
    inFields[6] =
        serdesStatus->single_serdes_status[index].power_down_rx_tx;

    /* print table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5], inFields[6]);
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrFe200SerdesParamsSet (table command)
*
* DESCRIPTION:
*     Set SerDes parameters table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId      - device to access.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200SerdesParamsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U_I deviceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    deviceId = (GT_U_I)inArgs[0];

    if (NULL == serdesStatus)
    {
        serdesStatus = (FE200_ALL_SERDES_STATUS*)cmdOsStaticMalloc
                    (sizeof(FE200_ALL_SERDES_STATUS));
    }

    serdesStatus->single_serdes_status[index].valid = 1;

    fe200_get_serdes_params (deviceId, (GT_U32*)serdesStatus);

    index = (GT_U_I)inFields[0];
    serdesStatus->single_serdes_status[index].valid = (GT_U_I)inFields[1];
    serdesStatus->single_serdes_status[index].common_mode = (GT_U_I)inFields[2];
    serdesStatus->single_serdes_status[index].main_control =
        (GT_U_I)inFields[3];
    serdesStatus->single_serdes_status[index].pre_emphasis =
        (GT_U_I)inFields[4];
    serdesStatus->single_serdes_status[index].amplitude = (GT_U_I)inFields[5];
    serdesStatus->single_serdes_status[index].power_down_rx_tx =
                                                        (GT_U_I)inFields[6];

    fe200_set_serdes_params (deviceId, (GT_U32*)serdesStatus);
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*
* Table: fe200UnicastDistributionTable
*
* Description:
*      FE200 Unicast Distribution Table.
*
* Fields:
*  [0] SAND_DEVICE_ENTITY reportingEntityType -
*          Reporting entity. Allowed values are FE3_ENTITY, FE2_ENTITY,
*          FE1_ENTITY.
*  [1] GT_BOOL changesOnly -
*          Table format
*  [2] GT_U_I offset -
*          Number of entry (offset in table).
*  [3] GT_BOOL linkStatusChanged -
*          If non zero then one of the links has changed its state and is now
*          either 'up' or 'down'.
*  [4] GT_U64 linkStateUp -
*          Bit map. Indication on status of all links.
*  [6] GT_BOOL debugFlagsPending -
*          Flag. If non zero then table is NOT a true representation of chip
*          logic, due to some internal debug registers, which have been set by
*          the user.
*  [7] GT_U_I reachabilityErrorPending -
*          Flag. If non zero then some reachability error is pending and table
*          may not be a true representation of chip logic.
*  [8] GT_U_I indexOfEntry -
*          Index, in table, of entry. Range: 0 - 2047. (relevant only for long
*          table format).
*  [9] GT_U32 distributionEntry -
*          Active links for distribution/routing.
*
* Comments:
*     None.
*/

/*******************************************************************************
* wrFe200UnicastDistributionGetFirst (table command)
*
* DESCRIPTION:
*     Get unicast distribution table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId                        - device to access.
*     [1] GT_BOOL changesOnly                    - required table format.
*     [2] SAND_DEVICE_ENTITY reportingEntityType - the device from which a
*                                                  report is required.
*
* OUTPUTS:
*      None.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200UnicastDistributionGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;
    DISTRIBUTION_ROUTING_UNI_REPORT report;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    deviceId = (GT_U_I)inArgs[0];
    sand_os_memset(&report, 0, sizeof(DISTRIBUTION_ROUTING_UNI_REPORT));
    report.changes_only = (GT_U_I)inArgs[1];
    report.reporting_entity_type = (SAND_DEVICE_ENTITY)inArgs[2];
    report.offset = index = 0;
    report.num_entries = 1;

    retStatus = fe200_get_unicast_table (deviceId, (GT_U32*)&report,
                                       sizeof(DISTRIBUTION_ROUTING_UNI_REPORT));
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);
    inFields[0] = report.reporting_entity_type;
    inFields[1] = report.changes_only;
    inFields[2] = report.offset;
    inFields[3] = report.link_status_changed;
    inFields[4] = report.link_state_up[0];
    inFields[5] = report.link_state_up[1];
    inFields[6] = report.debug_flags_pending;
    inFields[7] = report.reachability_error_pending;

    if (report.reporting_entity_type == SAND_FE1_ENTITY)
    {
        if (report.changes_only == 0)
        {
            inFields[8] = 0;
            inFields[9] =
            report.dist_route_entry[0].fe1_dist_uni_short_entry.
                distribution_entry;
        }
        else
        {
            inFields[8] =
            report.dist_route_entry[0].fe1_dist_uni_long_entry.index_of_entry;
            inFields[9] =
            report.dist_route_entry[0].fe1_dist_uni_long_entry.
                distribution_entry;
        }
    }
    else if (report.reporting_entity_type == SAND_FE2_ENTITY)
    {
        if (report.changes_only == 0)
        {
            inFields[8] = 0;
            inFields[9] =
            report.dist_route_entry[0].fe2_route_uni_short_entry.routing_entry;
        }
        else
        {
            inFields[8] =
            report.dist_route_entry[0].fe2_route_uni_long_entry.index_of_entry;
            inFields[9] =
            report.dist_route_entry[0].fe2_route_uni_long_entry.routing_entry;
        }
    }
    else /* SAND_FE3_ENTITY */
    {
        if (report.changes_only == 0)
        {
            inFields[8] = 0;
            inFields[9] =
            report.dist_route_entry[0].fe3_route_uni_short_entry.route_chip_id;
        }
        else
        {
            inFields[8] =
            report.dist_route_entry[0].fe3_route_uni_long_entry.index_of_entry;
            inFields[9] =
            report.dist_route_entry[0].fe3_route_uni_long_entry.route_chip_id;
        }
    }

    /* print table fields */
    if ((report.changes_only == 0) || (report.link_status_changed == 1))
    {
        fieldOutput("%d%d%d%d%x%d%d%d%x", inFields[0], inFields[1],
                    inFields[2], inFields[3], inFields[4],
                    /*inFields[5] ,*/
                    inFields[6], inFields[7], inFields[8], inFields[9]);
        galtisOutput(outArgs, result, "%f");
    }
    return CMD_OK;
}

/*******************************************************************************
* wrFe200UnicastDistributionGetNext (table command)
*
* DESCRIPTION:
*     Get unicast distribution table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId                        - device to access.
*     [1] GT_BOOL changesOnly                    - required table format.
*     [2] SAND_DEVICE_ENTITY reportingEntityType - the device from which a
*                                                  report is required.
*
* OUTPUTS:
*      None.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200UnicastDistributionGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;
    DISTRIBUTION_ROUTING_UNI_REPORT report;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check if end of table */
    sand_os_memset (&report, 0, sizeof(DISTRIBUTION_ROUTING_UNI_REPORT));
    report.reporting_entity_type = (SAND_DEVICE_ENTITY)inArgs[2];
    ++index;
    if (
        ((report.reporting_entity_type == SAND_FE1_ENTITY) && (index == 2048))
        ||
        ((report.reporting_entity_type == SAND_FE2_ENTITY) && (index == 2048))
        ||
        ((report.reporting_entity_type == SAND_FE3_ENTITY) && (index == 32))
        )
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    deviceId = (GT_U_I)inArgs[0];
    sand_os_memset (&report, 0, sizeof(DISTRIBUTION_ROUTING_UNI_REPORT));
    report.changes_only = (GT_U_I)inArgs[1];
    report.reporting_entity_type = (SAND_DEVICE_ENTITY)inArgs[2];
    report.offset = index;
    report.num_entries = 1;

    retStatus = fe200_get_unicast_table (deviceId, (GT_U32*)&report,
                        sizeof(DISTRIBUTION_ROUTING_UNI_REPORT));
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);

    inFields[0] = report.reporting_entity_type;
    inFields[1] = report.changes_only;
    inFields[2] = report.offset;
    inFields[3] = report.link_status_changed;
    inFields[4] = report.link_state_up[0];
    inFields[5] = report.link_state_up[1];
    inFields[6] = report.debug_flags_pending;
    inFields[7] = report.reachability_error_pending;

    if (report.reporting_entity_type == SAND_FE1_ENTITY)
    {
        if (report.changes_only == 0)
        {
            inFields[8] = 0;
            inFields[9] =
            report.dist_route_entry[0].fe1_dist_uni_short_entry.
                distribution_entry;
        }
        else
        {
            inFields[8] =
            report.dist_route_entry[0].fe1_dist_uni_long_entry.index_of_entry;
            inFields[9] =
            report.dist_route_entry[0].fe1_dist_uni_long_entry.
                distribution_entry;
        }
    }
    else if (report.reporting_entity_type == SAND_FE2_ENTITY)
    {
        if (report.changes_only == 0)
        {
            inFields[8] = 0;
            inFields[9] =
            report.dist_route_entry[0].fe2_route_uni_short_entry.routing_entry;
        }
        else
        {
            inFields[8] =
            report.dist_route_entry[0].fe2_route_uni_long_entry.index_of_entry;
            inFields[9] =
            report.dist_route_entry[0].fe2_route_uni_long_entry.routing_entry;
        }
    }
    else /* SAND_FE3_ENTITY */
    {
        if (report.changes_only == 0)
        {
            inFields[8] = 0;
            inFields[9] =
            report.dist_route_entry[0].fe3_route_uni_short_entry.route_chip_id;
        }
        else
        {
            inFields[8] =
            report.dist_route_entry[0].fe3_route_uni_long_entry.index_of_entry;
            inFields[9] =
            report.dist_route_entry[0].fe3_route_uni_long_entry.route_chip_id;
        }
    }

    /* print table fields */
    if ((report.changes_only == 0) || (report.link_status_changed == 1))
    {
        fieldOutput("%d%d%d%d%x%d%d%d%x", inFields[0], inFields[1],
                    inFields[2], inFields[3], inFields[4],
                    /*inFields[5],*/
                    inFields[6], inFields[7], inFields[8], inFields[9]);
        galtisOutput(outArgs, result, "%f");
    }
    return CMD_OK;
}

/*
* Table: fe200MulticastDistributionTable
*
* Description:
*      FE200 Multicast Distribution Table.
*
* Fields:
*  [0] SAND_DEVICE_ENTITY reportingEntityType -
*          Reporting entity. Allowed values are FE3_ENTITY, FE2_ENTITY,
*          FE1_ENTITY.
*  [1] GT_BOOL changesOnly -
*          Table format
*  [2] GT_U_I offset -
*          Number of entry (offset in table).
*  [3] GT_BOOL linkStatusChanged -
*          If non zero then one of the links has changed its state and is now
*          either 'up' or 'down'.
*  [4] GT_U64 linkStateUp -
*          Bit map. Indication on status of all links.
*  [6] GT_BOOL debugFlagsPending -
*          Flag. If non zero then table is NOT a true representation of chip
*          logic, due to some internal debug registers, which have been set by
*          the user.
*  [7] GT_U_I reachabilityErrorPending -
*          Flag. If non zero then some reachability error is pending and table
*          may not be a true representation of chip logic.
*  [8] FE200_DIST_REPLIC_ENTRY_TYPE_ENT type -
*          Type of distribution/replication entry.
*  [9] GT_U_I indexOfEntry -
*          Index, in table, of entry reported by 'distribution_entry' element
*          below.Range: 0 - 2047. Relevant only for FE1_DIST_MULTI_LONG_ENTRY,
*          FE2_REPLIC_MULTI_LONG_ENTRY and FE3_REPLIC_MULTI_LONG_ENTRY.
*  [10] GT_U32 longEntry1 -
*          Bit map representing links via which multicast message, with
*          multicast id indicated by offset of this entry in table, is to be
*          distributed/replicated.
*  [11] GT_U32 longEntry2 -
*          Bit map representing links via which multicast message, with
*          multicast id indicated by offset of this entry in table, is to be
*          replicated. Relevant only for FE2_REPLIC_MULTI_SHORT_ENTRY and
*          FE2_REPLIC_MULTI_LONG_ENTRY.
*
* Comments:
*     None.
*/

/*******************************************************************************
* wrFe200MulticastDistributionGetFirst (table command)
*
* DESCRIPTION:
*     Get multicast distribution table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId                        - device to access.
*     [1] GT_BOOL changesOnly                    - required table format.
*     [2] SAND_DEVICE_ENTITY reportingEntityType - the device from which a
*                                                  report is required.
*
* OUTPUTS:
*      None.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200MulticastDistributionGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;
    DISTRIBUTION_REPLICATION_MULTI_REPORT report;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    deviceId = (GT_U_I)inArgs[0];
    sand_os_memset (&report, 0, sizeof(report));
    report.changes_only = (GT_U_I)inArgs[1];
    report.reporting_entity_type = (SAND_DEVICE_ENTITY)inArgs[2];
    report.offset = index = 0;
    report.num_entries = 1;

    retStatus = fe200_get_multicast_table (deviceId, (GT_U32*)&report,
                        sizeof(DISTRIBUTION_REPLICATION_MULTI_REPORT));
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);
    inFields[0] = report.reporting_entity_type;
    inFields[1] = report.changes_only;
    inFields[2] = report.offset;
    inFields[3] = report.link_status_changed;
    inFields[4] = report.link_state_up[0];
    inFields[5] = report.link_state_up[1];
    inFields[6] = report.debug_flags_pending;
    inFields[7] = report.reachability_error_pending;

    if (report.reporting_entity_type == SAND_FE1_ENTITY)
    {
        if (report.changes_only == 0)
        {
            inFields[8] = FE1_DIST_MULTI_SHORT_ENTRY_E;
            inFields[9] = 0;
            inFields[10] = report.dist_replic_entry[0].
                fe1_dist_multi_short_entry.distribution_entry;
        }
        else
        {
            inFields[8] = FE1_DIST_MULTI_LONG_ENTRY_E;
            inFields[9] = report.dist_replic_entry[0].fe1_dist_multi_long_entry.
                index_of_entry;
            inFields[10] = report.dist_replic_entry[0].
                fe1_dist_multi_long_entry.distribution_entry;
        }
        /* Second entry is irrelevant for this format. First entry will be
           presented */
        inFields[11] = inFields[10];

    }
    else if (report.reporting_entity_type == SAND_FE2_ENTITY)
    {
        if (report.changes_only == 0)
        {
            inFields[8] = FE2_REPLIC_MULTI_SHORT_ENTRY_E;
            inFields[9] = 0;
            inFields[10] = report.dist_replic_entry[0].
                fe2_replic_multi_short_entry.replication_entry[0];
            inFields[11] = report.dist_replic_entry[0].
                fe2_replic_multi_short_entry.replication_entry[1];
        }
        else
        {
            inFields[8] = FE2_REPLIC_MULTI_LONG_ENTRY_E;
            inFields[9] = report.dist_replic_entry[0].
                fe2_replic_multi_long_entry.index_of_entry;
            inFields[10] = report.dist_replic_entry[0].
                fe2_replic_multi_long_entry.replication_entry[0];
            inFields[11] = report.dist_replic_entry[0].
                fe2_replic_multi_long_entry.replication_entry[1];
        }
    }
    else /* SAND_FE3_ENTITY */
    {
        if (report.changes_only == 0)
        {
            inFields[8] = FE3_REPLIC_MULTI_SHORT_ENTRY_E;
            inFields[9] = 0;
            inFields[10] = report.dist_replic_entry[0].
                fe3_replic_multi_short_entry.replication_entry;
        }
        else
        {
            inFields[8] = FE3_REPLIC_MULTI_LONG_ENTRY_E;
            inFields[9] = report.dist_replic_entry[0].
                fe3_replic_multi_long_entry.index_of_entry;
            inFields[10] = report.dist_replic_entry[0].
                fe3_replic_multi_long_entry.replication_entry;
        }
        /* Second entry is irrelevant for this format. First entry will be
           presented */
        inFields[11] = inFields[10];
    }

    /* print table fields */
    if ((report.changes_only == 0) || (report.link_status_changed == 1))
    {
        fieldOutput("%d%d%d%d%x%x%d%d%d%d%x%x", inFields[0], inFields[1],
                    inFields[2], inFields[3], inFields[4], inFields[5],
                    inFields[6], inFields[7], inFields[8], inFields[9],
                    inFields[10], inFields[11]);
        galtisOutput(outArgs, result, "%f");
    }
    return CMD_OK;
}

/*******************************************************************************
* wrFe200MulticastDistributionGetNext (table command)
*
* DESCRIPTION:
*     Get multicast distribution table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId                        - device to access.
*     [1] GT_BOOL changesOnly                    - required table format.
*     [2] SAND_DEVICE_ENTITY reportingEntityType - the device from which a
*                                                  report is required.
*
* OUTPUTS:
*      None.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200MulticastDistributionGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;
    DISTRIBUTION_REPLICATION_MULTI_REPORT report;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* check if end of table */
    sand_os_memset (&report, 0, sizeof(report));
    if (++index == 2048)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    deviceId = (GT_U_I)inArgs[0];
    report.changes_only = (GT_U_I)inArgs[1];
    report.reporting_entity_type = (SAND_DEVICE_ENTITY)inArgs[2];
    report.offset = index;
    report.num_entries = 1;

    retStatus = fe200_get_multicast_table (deviceId, (GT_U32*)&report,
                        sizeof(DISTRIBUTION_REPLICATION_MULTI_REPORT));
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);

    inFields[0] = report.reporting_entity_type;
    inFields[1] = report.changes_only;
    inFields[2] = report.offset;
    inFields[3] = report.link_status_changed;
    inFields[4] = report.link_state_up[0];
    inFields[5] = report.link_state_up[1];
    inFields[6] = report.debug_flags_pending;
    inFields[7] = report.reachability_error_pending;

    if (report.reporting_entity_type == SAND_FE1_ENTITY)
    {
        if (report.changes_only == 0)
        {
            inFields[8] = FE1_DIST_MULTI_SHORT_ENTRY_E;
            inFields[9] = 0;
            inFields[10] = report.dist_replic_entry[0].
                fe1_dist_multi_short_entry.distribution_entry;
        }
        else
        {
            inFields[8] = FE1_DIST_MULTI_LONG_ENTRY_E;
            inFields[9] = report.dist_replic_entry[0].fe1_dist_multi_long_entry.
                index_of_entry;
            inFields[10] = report.dist_replic_entry[0].
                fe1_dist_multi_long_entry.distribution_entry;
        }
        /* Second entry is irrelevant for this format. First entry will be
           presented */
        inFields[11] = inFields[10];
    }
    else if (report.reporting_entity_type == SAND_FE2_ENTITY)
    {
        if (report.changes_only == 0)
        {
            inFields[8] = FE2_REPLIC_MULTI_SHORT_ENTRY_E;
            inFields[9] = 0;
            inFields[10] = report.dist_replic_entry[0].
                fe2_replic_multi_short_entry.replication_entry[0];
            inFields[11] = report.dist_replic_entry[0].
                fe2_replic_multi_short_entry.replication_entry[1];
        }
        else
        {
            inFields[8] = FE2_REPLIC_MULTI_LONG_ENTRY_E;
            inFields[9] = report.dist_replic_entry[0].
                fe2_replic_multi_long_entry.index_of_entry;
            inFields[10] = report.dist_replic_entry[0].
                fe2_replic_multi_long_entry.replication_entry[0];
            inFields[11] = report.dist_replic_entry[0].
                fe2_replic_multi_long_entry.replication_entry[1];
        }
    }
    else /* SAND_FE3_ENTITY */
    {
        if (report.changes_only == 0)
        {
            inFields[8] = FE3_REPLIC_MULTI_SHORT_ENTRY_E;
            inFields[9] = 0;
            inFields[10] = report.dist_replic_entry[0].
                fe3_replic_multi_short_entry.replication_entry;
         }
        else
        {
            inFields[8] = FE3_REPLIC_MULTI_LONG_ENTRY_E;
            inFields[9] = report.dist_replic_entry[0].
                fe3_replic_multi_long_entry.index_of_entry;
            inFields[10] = report.dist_replic_entry[0].
                fe3_replic_multi_long_entry.replication_entry;
        }
        /* Second entry is irrelevant for this format. First entry will be
           presented */
        inFields[11] = inFields[10];
    }

    /* print table fields */
    if ((report.changes_only == 0) || (report.link_status_changed == 1))
    {
        fieldOutput("%d%d%d%d%x%x%d%d%d%d%x%x", inFields[0], inFields[1],
                    inFields[2], inFields[3], inFields[4], inFields[5],
                    inFields[6], inFields[7], inFields[8], inFields[9],
                    inFields[10], inFields[11]);
        galtisOutput(outArgs, result, "%f");
    }
    return CMD_OK;
}

/*******************************************************************************
* wrFe200MulticastDistributionSet (table command)
*
* DESCRIPTION:
*     Set multicast distribution table entry.
*
* INPUTS:
*     [0] GT_U_I deviceId                        - device to access.
*     [1] GT_BOOL changesOnly                    - required table format.
*     [2] SAND_DEVICE_ENTITY reportingEntityType - the device from which a
*                                                  report is required.
*
* OUTPUTS:
*      None.
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_SYNTAX_ERROR    - invalid data by user.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     None.
*
*******************************************************************************/
CMD_STATUS wrFe200MulticastDistributionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;         /* Galtis result (conversion of SAND error word) */
    GT_U32 retStatus;        /* SAND error word */
    GT_U_I deviceId;
    DISTRIBUTION_REPLICATION_MULTI_REPORT report;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(numFields < 12)
        return CMD_FIELD_UNDERFLOW;
    if(numFields > 12)
        return CMD_FIELD_OVERFLOW;
    if(inArgs[1] != inFields[1])
    {
        return CMD_SYNTAX_ERROR;
    }
    if(inArgs[2] != inFields[0])
    {
        return CMD_SYNTAX_ERROR;
    }

    /* map input arguments to locals */
    deviceId = (GT_U_I)inArgs[0];
    sand_os_memset (&report, 0, sizeof (DISTRIBUTION_REPLICATION_MULTI_REPORT));
    report.reporting_entity_type = (SAND_DEVICE_ENTITY)inFields[0];
    report.changes_only = (GT_U32)inFields[1];
    report.offset = (GT_U_I)inFields[2];
    report.link_status_changed = (GT_U_I)inFields[3];
    report.link_state_up[0] = (GT_U_I)inFields[4];
    report.link_state_up[1] = (GT_U_I)inFields[5];
    report.debug_flags_pending = (GT_U_I)inFields[6];
    report.reachability_error_pending = (GT_U_I)inFields[7];
    switch (inFields[8])
    {
        case FE1_DIST_MULTI_SHORT_ENTRY_E:
        {
            /* inFields[9], inFields[11] are ignored */
            report.dist_replic_entry[0].
               fe1_dist_multi_short_entry.distribution_entry = inFields[10];
            break;
        }
        case FE1_DIST_MULTI_LONG_ENTRY_E:
        {
            /* inFields[11] is ignored */
            report.dist_replic_entry[0].
                fe1_dist_multi_long_entry.index_of_entry = inFields[9];
            report.dist_replic_entry[0].
                fe1_dist_multi_long_entry.distribution_entry = inFields[10];
            break;
        }
        case FE2_REPLIC_MULTI_SHORT_ENTRY_E:
        {
            /* inFields[9] is ignored */
            report.dist_replic_entry[0].
               fe2_replic_multi_short_entry.replication_entry[0] = inFields[10];
            report.dist_replic_entry[0].
               fe2_replic_multi_short_entry.replication_entry[1] = inFields[11];
            break;
        }
        case FE2_REPLIC_MULTI_LONG_ENTRY_E:
        {
            report.dist_replic_entry[0].
                fe2_replic_multi_long_entry.index_of_entry = inFields[9];
            report.dist_replic_entry[0].
                fe2_replic_multi_long_entry.replication_entry[0] = inFields[10];
            report.dist_replic_entry[0].
                fe2_replic_multi_long_entry.replication_entry[1] = inFields[11];
            break;
        }
        case FE3_REPLIC_MULTI_SHORT_ENTRY_E:
        {
            /* inFields[9], inFields[11] are ignored */
            report.dist_replic_entry[0].fe3_replic_multi_short_entry.
                replication_entry = inFields[10];
            break;
        }
        case FE3_REPLIC_MULTI_LONG_ENTRY_E:
        {
            /* inFields[11] is ignored */
            report.dist_replic_entry[0].fe3_replic_multi_long_entry.
                index_of_entry = inFields[9];
            report.dist_replic_entry[0].fe3_replic_multi_long_entry.
                replication_entry = inFields[10];
            break;
        }
    }

    retStatus = fe200_set_multicast_table_entry (deviceId, (GT_U32*)&report);
    result = (SAND_RET)sand_get_error_code_from_error_word (retStatus);
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*
* Table: fe200MacCellCountersTable
*
* Description:
*      FE200 MAC Cell Counters Table.
*
* Fields:
*  [0] GT_U_I link -
*          Link number
*  [1] GT_U32 txCounter -
*          Counter of transmitted cells
*  [2] GT_BOOL txCounterOverflowed -
*          Flag. If non-zero then txCounter, specified above, has been overflown
*          since it was last read.
*  [3] GT_BOOL rxCrcError -
*          Flag. If non-zero then CRC error has been detected on received cells
*          since counter was last read.
*  [4] GT_BOOL rxMisalignmentError -
*          Flag. If non-zero then misalignment error has been detected on
*          received cells since counter was last read.
*  [5] GT_BOOL rxCodeGroupError -
*          Flag. If non-zero then code-group error has been detected on
*          received cells since counter was last read.
*
* Comments:
*     None.
*/

/*******************************************************************************
* wrFe200MacCellCountersGetFirst (table command)
*
* DESCRIPTION:
*     Gets the value of MAC cell counters.
*
* INPUTS:
*     [0] GT_U_I deviceId                          - device to access.
*
* OUTPUTS:
*     [0] FE200_MAC_COUNTER_TYPE_ENT txCountType  - Type of counter on the
*                                                   TRANSMIT direction
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     The counters are cleared after the reading.
*
*******************************************************************************/
CMD_STATUS wrFe200MacCellCountersGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U_I deviceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    macCountersReport = (MAC_COUNTERS_REPORT*)cmdOsMalloc
        (sizeof(MAC_COUNTERS_REPORT));
    deviceId = (GT_U_I)inArgs[0];

    tableRetStatus = fe200_get_count_of_mac_cells (deviceId,
                                                   (GT_U32*)macCountersReport);
    tableResult = (SAND_RET)sand_get_error_code_from_error_word(tableRetStatus);

    inFields[0] = index = 0;
    inFields[1] = macCountersReport->single_mac_counter[index].tx_counter;
    inFields[2] =
        macCountersReport->single_mac_counter[index].tx_counter_overflowed;
    inFields[3] =
        macCountersReport->single_mac_counter[index].rx_crc_error;
    inFields[4] =
        macCountersReport->single_mac_counter[index].rx_misalignment_error;
    inFields[5] =
        macCountersReport->single_mac_counter[index].rx_code_group_error;

    /* print table fields */
    fieldOutput("%d%x%d%d%d%d", inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5]);
    galtisOutput
        (outArgs, tableResult, "%d%f", macCountersReport->tx_count_type);
    return CMD_OK;
}

/*******************************************************************************
* wrFe200MacCellCountersGetNext (table command)
*
* DESCRIPTION:
*     Gets the value of MAC cell counters.
*
* INPUTS:
*     [0] GT_U_I deviceId                          - device to access.
*
* OUTPUTS:
*     [0] FE200_MAC_COUNTER_TYPE_ENT txCountType  - Type of counter on the
*                                                   TRANSMIT direction
*
* RETURNS:
*     CMD_OK              - on success.
*     CMD_AGENT_ERROR     - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     The counters are cleared after the reading.
*
*******************************************************************************/
CMD_STATUS wrFe200MacCellCountersGetNext
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

    /* check if end of table */
    if (++index == FE200_MAX_NUM_FE2_LINKS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        cmdOsFree (macCountersReport);
        return CMD_OK;
    }

    inFields[0] = index;
    inFields[1] = macCountersReport->single_mac_counter[index].tx_counter;
    inFields[2] =
        macCountersReport->single_mac_counter[index].tx_counter_overflowed;
    inFields[3] =
        macCountersReport->single_mac_counter[index].rx_crc_error;
    inFields[4] =
        macCountersReport->single_mac_counter[index].rx_misalignment_error;
    inFields[5] =
        macCountersReport->single_mac_counter[index].rx_code_group_error;

    /* print table fields */
    fieldOutput("%d%x%d%d%d%d", inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5]);
    galtisOutput
        (outArgs, tableResult, "%d%f", macCountersReport->tx_count_type);
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] = {
    /* general commands */
    {"fe200LogicalRead",
        &wrFe200LogicalRead,
        2, 0},
    {"fe200LogicalWrite",
        &wrFe200LogicalWrite,
        3, 0},
    {"fe200SetMacCounterType",
        &wrFe200SetMacCounterType,
        2, 0},
    {"fe200GetMacCounterType",
        &wrFe200GetMacCounterType,
        1, 0},
    {"fe200StartMacCountersCollection",
        &wrFe200StartMacCountersCollection,
        1, 0},
    {"fe200StopMacCountersCollection",
        &wrFe200StopMacCountersCollection,
        1, 0},

    /* table commands */
    {"fe200LinkStatusGetFirst",
        &wrFe200LinkStatusGetFirst,
        3, 0},
    {"fe200LinkStatusGetNext",
        &wrFe200LinkStatusGetNext,
        0, 0},
    {"fe200ConnectivityMapGetFirst",
        &wrFe200ConnectivityMapGetFirst,
        1, 0},
    {"fe200ConnectivityMapGetNext",
        &wrFe200ConnectivityMapGetNext,
        0, 0},
    {"fe200SerdesParamsGetFirst",
        &wrFe200SerdesParamsGetFirst,
        1, 0},
    {"fe200SerdesParamsGetNext",
        &wrFe200SerdesParamsGetNext,
        1, 0},
    {"fe200SerdesParamsSet",
        &wrFe200SerdesParamsSet,
        1, 7},
    {"fe200UnicastDistributionGetFirst",
        &wrFe200UnicastDistributionGetFirst,
        3, 0},
    {"fe200UnicastDistributionGetNext",
        &wrFe200UnicastDistributionGetNext,
        3, 0},
    {"fe200MulticastDistributionGetFirst",
        &wrFe200MulticastDistributionGetFirst,
        3, 0},
    {"fe200MulticastDistributionGetNext",
        &wrFe200MulticastDistributionGetNext,
        3, 0},
    {"fe200MulticastDistributionSet",
        &wrFe200MulticastDistributionSet,
        3, 12},
    {"fe200MacCellCountersGetFirst",
        &wrFe200MacCellCountersGetFirst,
        1, 0},
    {"fe200MacCellCountersGetNext",
        &wrFe200MacCellCountersGetNext,
        0, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFe200
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
GT_STATUS cmdLibInitFe200 (GT_VOID)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

