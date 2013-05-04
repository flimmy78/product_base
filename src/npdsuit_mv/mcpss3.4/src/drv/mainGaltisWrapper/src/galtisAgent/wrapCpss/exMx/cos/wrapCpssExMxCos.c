/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCosCpss.c
*
* DESCRIPTION:
*       Wrapper functions for Cos cpss functions
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/


/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/exMx/exMxGen/cos/cpssExMxL2Cos.h>
#include <cpss/exMx/exMxGen/cos/cpssExMxCosMap.h>
#include <cpss/exMx/exMxGen/vb/cpssExMxVb.h>



/*******************************************************************************
* cpssExMxCosByDscpDpSet
*
* DESCRIPTION:
*       Sets the CoS Mapping entry for giving DSCP value and Drop Precedence
*       level in one of the two types of tables:
*       1. DSCP to CoS mapping marking table. This table holds 64 set of CoS
*          attributes. A set is selected by indexing the table with the packet
*          -DSCP attribute (Dp is always 0)
*       2. DSCP to CoS mapping re-marking table based on L3 CoS. This table
*          holds 192 set of CoS attributes. A set is selected by indexing the
*          table with the packet - DSCP(0-63) attribute and Dp level (0-2).
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - packet processor number.
*       dscp        - DiffServ Code point index.[0-63]
*       dpLevel     - Drop Precedence level.[0-2]
*       cosEntryPtr - CoS mapping entry
*       tableType   - Remarking or marking table.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or wrong DSCP number
*
* COMMENTS:
*       in case that the dpLevel = 0, this function also sets the DSCP -> COS
*       table in the MPLS block.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosByDscpDpSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                   dev;
    GT_U8                   dscp;
    CPSS_DP_LEVEL_ENT       dpLevel;
    CPSS_COS_MAP_STC        cosEntryPtr;
    CPSS_COS_TABLE_TYPE_ENT tableType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    tableType = (GT_U8)inArgs[1];
    dscp = (GT_U8)inFields[0];
    dpLevel = (CPSS_DP_LEVEL_ENT)inFields[1];
    cosEntryPtr.dscp = (GT_U8)inFields[2];
    cosEntryPtr.cosParam.dropPrecedence = (CPSS_DP_LEVEL_ENT)inFields[3];
    cosEntryPtr.cosParam.userPriority = (GT_U8)inFields[4];
    cosEntryPtr.cosParam.trafficClass = (GT_U8)inFields[5];
    cosEntryPtr.expField = (GT_U8)inFields[6];
    cosEntryPtr.userDef = (GT_U8)inFields[7];


    /* call cpss api function */
    result = cpssExMxCosByDscpDpSet(dev, dscp, dpLevel, &cosEntryPtr, tableType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxCosByDscpDpGet
*
* DESCRIPTION:
*       Get the CoS Mapping entry for giving DSCP value and Drop Precedence
*       level from one of the two types of tables:
*       1. DSCP to CoS mapping marking table. This table holds 64 set of CoS
*          attributes. A set is selected by indexing the table with the packet
*          DSCP attribute (Dp is always 0).
*       2. DSCP to CoS mapping re-marking table based on L3 CoS. This table
*          holds 192 set of CoS attributes. A set is selected by indexing the
*          table with the packet - DSCP(0-63) attribute and Dp level (0-2).
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - packet processor number.
*       dscp     - DiffServ Code point index.[0-63]
*       dpLevel  - Drop Precedence level.[0-2]
*       tableType - Remarking or marking table.
*
* OUTPUTS:
*       cosEntryPtr - CoS mapping entry
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or wrong DSCP number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#if 0
static CMD_STATUS wrcpssExMxCosByDscpDpGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 dscp;
    CPSS_DP_LEVEL_ENT dpLevel;
    CPSS_COS_TABLE_TYPE_ENT tableType;
    CPSS_COS_MAP_STC cosEntryPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&cosEntryPtr, 0, sizeof(cosEntryPtr));

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    dscp = (GT_U8)inArgs[1];
    dpLevel = (CPSS_DP_LEVEL_ENT)inArgs[2];
    tableType = (CPSS_COS_TABLE_TYPE_ENT)inArgs[3];



     /* call cpss api function */
    result = cpssExMxCosByDscpDpGet(dev, dscp, dpLevel, tableType, &cosEntryPtr);

    if (result != GT_OK)
   {
       if (result == GT_NO_SUCH)
           galtisOutput(outArgs, GT_OK, "%d", -1);
       else
           galtisOutput(outArgs, result, "");
       return CMD_OK;
   }


    inFields[0] = cosEntryPtr.dscp;
    inFields[1] = cosEntryPtr.cosParam.dropPrecedence;
    inFields[2] = cosEntryPtr.cosParam.userPriority;
    inFields[3] = cosEntryPtr.cosParam.trafficClass;
    inFields[4] = cosEntryPtr.expField;
    inFields[5] = cosEntryPtr.userDef;

    /* pack and output table fields */
   fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}
#endif
/*******************************************************************************
* cpssExMxCosByExpDpSet
*
* DESCRIPTION:
*       Sets the CoS Mapping entry for giving Exp field in the MPLS Shim header
*       and Drop Precedence level in one of the two types of tables:
*       1. Exp field to CoS mapping marking table. This table holds 8 set of CoS
*          attributes. A set is selected by indexing the table with the
*          packet-MPLS Shim header (0-7) attribute (Dp is always 0)
*       2. Exp field to CoS mapping re-marking table. This table holds 24 set
*          of CoS attributes. A set is selected by indexing the table with
*          the packet - MPLS Shim header(0-7) and Dp level(0-2).
*
*       APPLICABLE DEVICES:  All ExMx devices
*
*
* INPUTS:
*       dev          - packet processor number.
*       expField     - Experimental field in MPLS Shim header, used for DiffServ
*                      support of MPLS [0-7]
*       dpLevel      - Drop Precedence level for internal dropping policy in the
*                      transmit queues in the case of congestion
*                            [0 on mraking or 0-2 on re-marking]
*       cosEntryPtr  - CoS mapping entry
*       tableType    - Remarking or marking table.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or wrong Experimental
*                     field in MPLS Shim header
*
* COMMENTS:
*       in case that the dpLevel = 0, this function also sets the EXP -> COS
*       table in the MPLS block.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosByExpDpSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                   dev;
    GT_U8                   exp;
    CPSS_DP_LEVEL_ENT       dpLevel;
    CPSS_COS_MAP_STC        cosEntryPtr;
    CPSS_COS_TABLE_TYPE_ENT tableType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    tableType = (GT_U8)inArgs[1];
    exp = (GT_U8)inFields[0];
    dpLevel = (CPSS_DP_LEVEL_ENT)inFields[1];
    cosEntryPtr.dscp = (GT_U8)inFields[2];
    cosEntryPtr.cosParam.dropPrecedence = (CPSS_DP_LEVEL_ENT)inFields[3];
    cosEntryPtr.cosParam.userPriority = (GT_U8)inFields[4];
    cosEntryPtr.cosParam.trafficClass = (GT_U8)inFields[5];
    cosEntryPtr.expField = (GT_U8)inFields[6];
    cosEntryPtr.userDef = (GT_U8)inFields[7];


    /* call cpss api function */
    result = cpssExMxCosByExpDpSet(dev, exp, dpLevel, &cosEntryPtr, tableType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCosByExpDpGet
*
* DESCRIPTION:
*       Get the CoS Mapping entry for giving Exp field in the MPLS Shim header
*       and Drop Precedence level from one of the two types of tables:
*       1. Exp field to CoS mapping marking table. This table holds 8 set of CoS
*          attributes. A set is selected by indexing the table with the
*          packet-MPLS Shim header (0-7) attribute (Dp is always 0)
*       2. Exp field to CoS mapping re-marking table. This table holds 24 set
*          of CoS attributes. A set is selected by indexing the table
*          with the packet - MPLS Shim header(0-7) and Dp level(0-2).
*
*       APPLICABLE DEVICES:  All ExMx devices
*
*
* INPUTS:
*       dev       - packet processor number.
*       expField  - Experimental field in MPLS Shim header, used for DiffServ
*                   support of MPLS [0-7]
*       dpLevel   - Drop Precedence level for internal dropping policy in the
*                   transmit queues in the case of congestion [0 on mraking
*                   or 0-2 on re-marking]
*       tableType - Remarking or marking table.
*
* OUTPUTS:
*       cosEntryPtr - CoS mapping entry
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or wrong Experimental field in
*                     MPLS Shim header
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#if 0
static CMD_STATUS wrcpssExMxCosByExpDpGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 exp;
    CPSS_DP_LEVEL_ENT dpLevel;
    CPSS_COS_TABLE_TYPE_ENT tableType;
    CPSS_COS_MAP_STC cosEntryPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&cosEntryPtr, 0, sizeof(cosEntryPtr));

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    exp = (GT_U8)inArgs[1];
    dpLevel = (CPSS_DP_LEVEL_ENT)inArgs[2];
    tableType = (CPSS_COS_TABLE_TYPE_ENT)inArgs[3];



     /* call cpss api function */
    result = cpssExMxCosByExpDpGet(dev, exp, dpLevel, tableType, &cosEntryPtr);

    if (result != GT_OK)
   {
       if (result == GT_NO_SUCH)
           galtisOutput(outArgs, GT_OK, "%d", -1);
       else
           galtisOutput(outArgs, result, "");
       return CMD_OK;
   }


    inFields[0] = cosEntryPtr.dscp;
    inFields[1] = cosEntryPtr.cosParam.dropPrecedence;
    inFields[2] = cosEntryPtr.cosParam.userPriority;
    inFields[3] = cosEntryPtr.cosParam.trafficClass;
    inFields[4] = cosEntryPtr.expField;
    inFields[5] = cosEntryPtr.userDef;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}
#endif

/*******************************************************************************
* cpssExMxCosByTcDpSet
*
* DESCRIPTION:
*       Sets the CoS Mapping entry for giving trafic class value and
*       Drop Precedence level in L2 re-mark table. This table holds 24 entries.
*       A set is selected by indexing the table with the packet- TC(0-7)
*       and Dp(0-2).
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - packet processor number.
*       trClass     - traffic class for internal forwarding priority [0-7]
*       dpLevel     - Drop Precedence level for internal dropping policy in the
*                     transmit queues in the case of congestion [0-2]
*       cosEntryPtr - CoS mapping entry.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or wrong traffic class number
*
* COMMENTS:
*       When mapping Traffic Class + Drop Precedence to CoS, only the
*       User Priority and User Defined field are remapped; the other
*       CoS fields are not relevant.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosByTcDpSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 trfClass;
    CPSS_DP_LEVEL_ENT dpLevel;
    CPSS_COS_TABLE_TYPE_ENT tableType;
    CPSS_COS_MAP_STC cosEntryPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&cosEntryPtr, 0, sizeof(cosEntryPtr));

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    tableType = (CPSS_COS_TABLE_TYPE_ENT)inArgs[1];
    trfClass = (GT_U8)inFields[0];
    dpLevel = (CPSS_DP_LEVEL_ENT)inFields[1];
    cosEntryPtr.dscp = (GT_U8)inFields[2];
    cosEntryPtr.cosParam.dropPrecedence = (CPSS_DP_LEVEL_ENT)inFields[3];
    cosEntryPtr.cosParam.userPriority = (GT_U8)inFields[4];
    cosEntryPtr.cosParam.trafficClass = (GT_U8)inFields[5];
    cosEntryPtr.expField = (GT_U8)inFields[6];
    cosEntryPtr.userDef = (GT_U8)inFields[7];


    /* call cpss api function */
    result = cpssExMxCosByTcDpSet(dev, trfClass, dpLevel, tableType, &cosEntryPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxCosByTcDpGet
*
* DESCRIPTION:
*       Gets the CoS Mapping entry for giving trafic class value
*       and Drop Precedence level in L2 re-mark table. This table holds 24
*       entries. A set is selected by indexing the table with the packet -
*       TC(0-7) and Dp(0-2).
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - packet processor number.
*       trClass   - traffic class for internal forwarding priority [0-7]
*       dpLevel   - Drop Precedence level for internal dropping policy in the
*                   transmit queues in the case of congestion [0-2]
*       tableType - Remarking or marking table.[Remarking only]
*
* OUTPUTS:
*       cosEntryPtr - CoS mapping entry
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or wrong traffic class number
*
* COMMENTS:
*       When mapping Traffic Class + Drop Precedence to CoS, only the
*       User Priority and User Defined field are remapped; the other
*       CoS fields are not relevant.
*
*******************************************************************************/
#if 0
static CMD_STATUS wrcpssExMxCosByTcDpGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 trfClass;
    CPSS_DP_LEVEL_ENT dpLevel;
    CPSS_COS_TABLE_TYPE_ENT tableType;
    CPSS_COS_MAP_STC cosEntryPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&cosEntryPtr, 0, sizeof(cosEntryPtr));

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    trfClass = (GT_U8)inArgs[1];
    dpLevel = (CPSS_DP_LEVEL_ENT)inArgs[2];
    tableType = (CPSS_COS_TABLE_TYPE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxCosByTcDpGet(dev, trfClass, dpLevel, tableType, &cosEntryPtr);

    if (result != GT_OK)
   {
       if (result == GT_NO_SUCH)
           galtisOutput(outArgs, GT_OK, "%d", -1);
       else
           galtisOutput(outArgs, result, "");
       return CMD_OK;
   }


    inFields[0] = cosEntryPtr.dscp;
    inFields[1] = cosEntryPtr.cosParam.dropPrecedence;
    inFields[2] = cosEntryPtr.cosParam.userPriority;
    inFields[3] = cosEntryPtr.cosParam.trafficClass;
    inFields[4] = cosEntryPtr.expField;
    inFields[5] = cosEntryPtr.userDef;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);
    galtisOutput(outArgs, result, "%f");
     return CMD_OK;
}
#endif

/*******************************************************************************
* cpssExMxCosPortDefaultTcSet
*
* DESCRIPTION:
*       Sets the default traffic class for a specific port.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - packet processor number.
*       port      - port number.
*       trfClass  - default traffic class of a port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or wrong traffic class number
*                     or wrong port number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosPortDefaultTcSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 port;
    GT_U8 trfClass;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    trfClass = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxCosPortDefaultTcSet(dev, port, trfClass);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxCosPortDefaultTcGet
*
* DESCRIPTION:
*       Gets the the port default traffic class.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - packet processor number.
*       port      - port number.
*
* OUTPUTS:
*       trfClassPtr - default traffic class of a port.[0-7]
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or wrong port number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosPortDefaultTcGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 port;
    GT_U8 trfClassPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxCosPortDefaultTcGet(dev, port, &trfClassPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", trfClassPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxCosUserPrio2TcSet
*
* DESCRIPTION:
*       Sets the traffic class to mapped to a packet received with the requested
*       user priority (VPT).
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - packet processor number.
*       userPrio  - user priority (VPT).
*       trfClass  - The Traffic Class the received frame is assigned.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or wrong user priority number
*                     or wrong port number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosUserPrio2TcSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 userPrio;
    GT_U8 trfClass;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    userPrio = (GT_U8)inArgs[1];
    trfClass = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxCosUserPrio2TcSet(dev, userPrio, trfClass);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxCosUserPrio2TcGet
*
* DESCRIPTION:
*       Returns the traffic class mapped to the requested user priority (VPT).
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - packet processor number.
*       userPrio - user priority
*
* OUTPUTS:
*       trfClassPtr - The Traffic Class the received frame is assigned.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or user priority number
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosUserPrio2TcGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 userPrio;
    GT_U8 trfClassPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    userPrio = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxCosUserPrio2TcGet(dev, userPrio, &trfClassPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", trfClassPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCosTc2UserPrioSet
*
* DESCRIPTION:
*       Sets an entry in the traffic class to user priority (VPT) table, for
*       the requested traffic class.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - packet processor device number.
*       trfClass - the Traffic Class the received frame is assigned.
*       userPrio - user priority to assign.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or user priority number
*                     or traffic class
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosTc2UserPrioSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 trfClass;
    GT_U8 userPrio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    trfClass = (GT_U8)inArgs[1];
    userPrio = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxCosTc2UserPrioSet(dev, trfClass, userPrio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCosTc2UserPrioGet
*
* DESCRIPTION:
*       Returns the user priority mapped to the traffic class, in the traffic
*       class to user priority (VPT) table.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - packet processor device number.
*       trfClass  - the Traffic Class the received frame is assigned.
*
* OUTPUTS:
*       userPrioPtr - user priority to assign.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or traffic class
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosTc2UserPrioGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 trfClass;
    GT_U8 userPrioPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    trfClass = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxCosTc2UserPrioGet(dev, trfClass, &userPrioPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", userPrioPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCosUserPrioModeSet
*
* DESCRIPTION:
*       Set the user priority mode for untagged packets to be accroding to the
*       ingress port user priority (VPT) or
*       according to the ingress TC and the mapping between TC to VPT.
*       (using the TC to VPT mapping table).
*
*       APPLICABLE DEVICES:  98EX1x6 family
*
* INPUTS:
*       devNum      - packet processor device number.
*       port        - Port number
*       userPrioMode-
*          GT_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E :
*              Mapping priority to VPT according to the port default UP.
*          GT_COS_USER_PRIO_MODE_TC_PER_PORT_E :
*              Mapping According to the assignment TC to UP.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_NOT_SUPPORTED - the ASIC do not support this mode.
*       GT_BAD_PARAM- on wrong device number or port number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosUserPrioModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 devNum;
    GT_U8 port;
    CPSS_COS_USER_PRIO_MODE_ENT userPrioMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    userPrioMode = (CPSS_COS_USER_PRIO_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxCosUserPrioModeSet(devNum, port, userPrioMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxCosUserPrioModeDebugCmdGet
*
* DESCRIPTION:
*       Get the user priority mode for untagged packets which is: accroding
*       to the ingress port user priority (VPT) or
*       according to the ingress TC and the mapping between TC to VPT.
*       (using the TC to VPT mapping table).
*
*       APPLICABLE DEVICES:  98EX1x6 family
*
* INPUTS:
*       devNum      - packet processor device number.
*       port        - Port number

*
* OUTPUTS:
*       userPrioModePtr-
*          GT_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E :
*              Mapping priority to VPT according to the port default UP.
*          GT_COS_USER_PRIO_MODE_TC_PER_PORT_E :
*              Mapping According to the assignment TC to UP.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or port number
*
* COMMENTS:
*       This function is only for debug.
*       This function reads from the HW.
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosUserPrioModeDebugCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    GT_U8                       port;
    CPSS_COS_USER_PRIO_MODE_ENT userPrioModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxCosUserPrioModeDebugCmdGet(devNum, port, &userPrioModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", userPrioModePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCosPortDefaultUserPrioSet
*
* DESCRIPTION:
*       Set default user priority (VPT) for untagged packet to a given port.
*       This function is supported only in 98EX1x6 family.
*
*       APPLICABLE DEVICES:  98EX1x6 family
*
* INPUTS:
*       devNum    - packet processor device number.
*       port      - port number.(Not CPU port)
*       defaultUserPrio  - default user priority (VPT).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or port number or UP number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosPortDefaultUserPrioSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 devNum;
    GT_U8 port;
    GT_U8 defaultUserPrio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    defaultUserPrio = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxCosPortDefaultUserPrioSet(devNum, port, defaultUserPrio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCosPortDefaultUserPrioDebugCmdGet
*
* DESCRIPTION:
*       Get the default user priority (VPT) for untagged packet for a given port.
*       This parameter is relevant only if mark port L3 is set.
*
*       APPLICABLE DEVICES:  98EX1x6 family
*
* INPUTS:
*       devNum    - packet processor device number.
*       port      - port number.(Not CPU port)
*
* OUTPUTS:
*       defaultUserPrioPtr - user priority (VPT) for a given port [0-7]
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or port number
*
* COMMENTS:
*       This function is only for debug.
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosPortDefaultUserPrioDebugCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 devNum;
    GT_U8 port;
    GT_U8 defaultUserPrioPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxCosPortDefaultUserPrioDebugCmdGet(devNum, port, &defaultUserPrioPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", defaultUserPrioPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxCosUserPrio2DpSet
*
* DESCRIPTION:
*       Sets an entry in the user priority to Drop Precedence table, for a
*       specific user priority.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - packet processor device number.
*       userPrio  - user priority .
*       dpLevel   - DP level the received frame with specific user priority is
*                   mapped to.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or user priority number
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosUserPrio2DpSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 userPrio;
    CPSS_DP_LEVEL_ENT dpLevel;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    userPrio = (GT_U8)inArgs[1];
    dpLevel = (CPSS_DP_LEVEL_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxCosUserPrio2DpSet(dev, userPrio, dpLevel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCosUserPrio2DpGet
*
* DESCRIPTION:
*       Get the drop precedence level from user priority table.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - packet processor device number.
*       userPrio - user priority.[0-7]
*
* OUTPUTS:
*       dpLevelPtr - DP level the received frame with specific user priority is
*                    mapped to.[0-2]
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosUserPrio2DpGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 userPrio;
    CPSS_DP_LEVEL_ENT dpLevelPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    userPrio = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxCosUserPrio2DpGet(dev, userPrio, &dpLevelPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dpLevelPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCosMacEntryTcOverrideSet
*
* DESCRIPTION:
*       Enable / Disable TC assignment based on SA/DA mac.
*       if MAC TC assignment is enabled (CPSS_COS_MAC_ADDR_TYPE_ALL_E)
*       then the TC is set as the following:
*          1. If SA and DA were found --> TC = max(TCs,TCd)
*          2. If only SA was found --> TC = TCs
*          3. If only DA was found --> TC = TCd
*          4. None was found --> TC preserved from previous stage
*
*       APPLICABLE DEVICES:  All ExMx devices
*
*
* INPUTS:
*       devNum      - packet processor device number.
*       macEntryTcOverrideEnable - CPSS_COS_MAC_ADDR_TYPE_ALL_E for enable,
*                                  CPSS_COS_MAC_ADDR_TYPE_NONE_E for disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosMacEntryTcOverrideSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 devNum;
    CPSS_COS_MAC_ADDR_TYPE_ENT macEntryTcOverrideEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    macEntryTcOverrideEnable = (CPSS_COS_MAC_ADDR_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxCosMacEntryTcOverrideSet(devNum, macEntryTcOverrideEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxCosPortOverrideTcEnable
*
* DESCRIPTION:
*       Enable/disable the override TrafficClass for packets received on
*       a specific port. The overriding is relevant only for tagged and priority
*       tagged packets.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       state - GT_TRUE for enable override, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or wrong port number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosPortOverrideTcEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 port;
    GT_BOOL state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxCosPortOverrideTcEnable(dev, port, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


#if 0

/*******************************************************************************
* cpssExMxCosCosMap8To4TcSet
*
* DESCRIPTION:
*       Configures traffic class conversion. Range 0-7 to 0-3.
*       The conversion is necessary for the cascading port which
*       has a cascading device with 4 queues. (i.e, 98DX240)
*
*       APPLICABLE DEVICES:  98EX1x6 devices in the Value Blade
*
* INPUTS:
*       dev                 - packet processor number.
*       value8Tc            - TC valuue in range 0-7.
*       value4Tc            - TC valuue in range 0-3.
*                             GT_FALSE - disable DSCP to new DSCP mutation,
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on error.
*       GT_BAD_PARAM    - on wrong device number or (8) trafic class or
*                         or )4) traffic class number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxCosMap8To4TcSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8 dev;
    GT_U8 value8Tc;
    GT_U8 value4Tc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    value8Tc = (GT_U8)inArgs[1];
    value4Tc = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxVb8TcTo4TcMapSet(dev, value8Tc, value4Tc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

#endif


static CMD_COMMAND dbCommands[] =
{
   {"cpssExMxCosByDscpDpSet",
        &wrcpssExMxCosByDscpDpSet,
        2, 8},
 /*
    {"cpssExMxCosByDscpDpGetFirst",
        &wrcpssExMxCosByDscpDpGet,
        4, 0},

    {"cpssExMxCosByDscpDpGetNext",
           &wrcpssExMxCosByDscpDpGet,
           4, 0},
*/
    {"cpssExMxCosByExpDpSet",
        &wrcpssExMxCosByExpDpSet,
        2, 8},
/*
    {"cpssExMxCosByExpDpGetFirst",
        &wrcpssExMxCosByExpDpGet,
        4, 0},

    {"cpssExMxCosByExpDpGetNext",
        &wrcpssExMxCosByExpDpGet,
        4, 0},
*/
    {"cpssExMxCosByTcDpSet",
        &wrcpssExMxCosByTcDpSet,
        2, 8},
/*
    {"cpssExMxCosByTcDpGetFirst",
        &wrcpssExMxCosByTcDpGet,
        4, 0},

    {"cpssExMxCosByTcDpGetNext",
        &wrcpssExMxCosByTcDpGet,
        4, 0},
*/
    {"cpssExMxCosPortDefaultTcSet",
        &wrcpssExMxCosPortDefaultTcSet,
        3, 0},

    {"cpssExMxCosPortDefaultTcGet",
        &wrcpssExMxCosPortDefaultTcGet,
        2, 0},

    {"cpssExMxCosUserPrio2TcSet",
        &wrcpssExMxCosUserPrio2TcSet,
        3, 0},

    {"cpssExMxCosUserPrio2TcGet",
        &wrcpssExMxCosUserPrio2TcGet,
        2, 0},

    {"cpssExMxCosTc2UserPrioSet",
        &wrcpssExMxCosTc2UserPrioSet,
        3, 0},

    {"cpssExMxCosTc2UserPrioGet",
        &wrcpssExMxCosTc2UserPrioGet,
        2, 0},

    {"cpssExMxCosUserPrioModeSet",
        &wrcpssExMxCosUserPrioModeSet,
        3, 0},

    {"cpssExMxCosUserPrioModeDebugCmdGet",
        &wrcpssExMxCosUserPrioModeDebugCmdGet,
        2, 0},

    {"cpssExMxCosPortDefaultUserPrioSet",
        &wrcpssExMxCosPortDefaultUserPrioSet,
        3, 0},

    {"cpssExMxCosPortDefaultUserPrioDebugCmdGet",
        &wrcpssExMxCosPortDefaultUserPrioDebugCmdGet,
        2, 0},

    {"cpssExMxCosUserPrio2DpSet",
        &wrcpssExMxCosUserPrio2DpSet,
        3, 0},

    {"cpssExMxCosUserPrio2DpGet",
        &wrcpssExMxCosUserPrio2DpGet,
        2, 0},

    {"cpssExMxCosMacEntryTcOverrideSet",
        &wrcpssExMxCosMacEntryTcOverrideSet,
        2, 0},

    {"cpssExMxCosPortOverrideTcEnable",
        &wrcpssExMxCosPortOverrideTcEnable,
        3, 0},
/*
    {"cpssExMxCosMap8To4TcSet",
        &wrcpssExMxCosMap8To4TcSet,
        3, 0}
*/
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))



/*******************************************************************************
* cmdLibInitCpssExMxCos
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
GT_STATUS cmdLibInitCpssExMxCos
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}




