/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* wrapCpssDxChPtp.c
*
* DESCRIPTION:
*       Wrapper functions for PTP cpss.dxCh functions
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
#include <cpss/dxCh/dxChxGen/ptp/cpssDxChPtp.h>


/* support for multi port groups */

/*******************************************************************************
* ptpMultiPortGroupsBmpGet
*
* DESCRIPTION:
*       Get the portGroupsBmp for multi port groups device.
*       when 'enabled' --> wrappers will use the APIs
*       with portGroupsBmp parameter
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)enable / disable the use of APIs with portGroupsBmp parameter.
*       portGroupsBmpPtr - (pointer to)port groups bmp , relevant only when enable = GT_TRUE
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*******************************************************************************/
static void ptpMultiPortGroupsBmpGet
(
    IN   GT_U8               devNum,
    OUT  GT_BOOL             *enablePtr,
    OUT  GT_PORT_GROUPS_BMP  *portGroupsBmpPtr
)
{
    /* default */
    *enablePtr  = GT_FALSE;
    *portGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if (0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return;
    }

    utilMultiPortGroupsBmpGet(devNum, enablePtr, portGroupsBmpPtr);
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */

static GT_STATUS pg_wrap_cpssDxChPtpTodCounterFunctionSet
(
    IN GT_U8                                    devNum,
    IN CPSS_PORT_DIRECTION_ENT                  direction,
    IN CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT       function
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterFunctionSet(devNum, direction, function);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterFunctionSet(devNum, pgBmp, 
                                                          direction, function);
    }
}


static GT_STATUS pg_wrap_cpssDxChPtpTodCounterFunctionGet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_PORT_DIRECTION_ENT                 direction,
    OUT CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT      *functionPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterFunctionGet(devNum, direction, functionPtr);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterFunctionGet(devNum, pgBmp, 
                                                          direction, functionPtr);
    }
}


static GT_STATUS pg_wrap_cpssDxChPtpTodCounterFunctionTriggerSet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterFunctionTriggerSet(devNum, direction);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterFunctionTriggerSet(devNum, pgBmp, 
                                                                 direction);
    }
}


static GT_STATUS pg_wrap_cpssDxChPtpTodCounterFunctionTriggerGet
(
    IN GT_U8                    devNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterFunctionTriggerGet(devNum, direction, 
                                                        enablePtr);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterFunctionTriggerGet(devNum, pgBmp, 
                                                                 direction, 
                                                                 enablePtr);
    }
}


static GT_STATUS pg_wrap_cpssDxChPtpTodCounterShadowSet
(
    IN GT_U8                        devNum,
    IN CPSS_PORT_DIRECTION_ENT      direction,
    IN CPSS_DXCH_PTP_TOD_COUNT_STC  *todCounterPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterShadowSet(devNum, direction, todCounterPtr);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterShadowSet(devNum, pgBmp, direction,
                                                        todCounterPtr);
    }
}


static GT_STATUS pg_wrap_cpssDxChPtpTodCounterShadowGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_DIRECTION_ENT         direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterShadowGet(devNum, direction, todCounterPtr);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterShadowGet(devNum, pgBmp, direction,
                                                        todCounterPtr);
    }
}

static GT_STATUS pg_wrap_cpssDxChPtpTodCounterGet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_DIRECTION_ENT          direction,
    OUT CPSS_DXCH_PTP_TOD_COUNT_STC     *todCounterPtr
)
{

    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ptpMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return  cpssDxChPtpTodCounterGet(devNum, direction, todCounterPtr);
    }
    else
    {
        return  cpssDxChPtpPortGroupTodCounterGet(devNum, pgBmp, direction,
                                                  todCounterPtr);
    }
}


/*******************************************************************************
* cpssDxChPtpEtherTypeSet
*
* DESCRIPTION:
*       Configure ethertype0/ethertype1 of PTP over Ethernet packets.
*       A packet is identified as PTP over Ethernet if its EtherType matches 
*       one of the configured values.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index, values 0..1
*       etherType       - PTP EtherType0 or EtherType1 according to the index, 
*                         values 0..0xFFFF
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_OUT_OF_RANGE          - on wrong etherType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEtherTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      etherTypeIndex;
    GT_U32      etherType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    etherTypeIndex = (GT_U32)inArgs[1];
    etherType = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpEtherTypeSet(devNum, etherTypeIndex, etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpEtherTypeGet
*
* DESCRIPTION:
*       Get ethertypes of PTP over Ethernet packets.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       etherTypeIndex  - PTP EtherType index, values 0..1
*
* OUTPUTS:
*       etherTypePtr    - (pointer to) PTP EtherType0 or EtherType1 according to 
*                          the index 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or etherTypeIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEtherTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      etherTypeIndex;
    GT_U32      etherType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    etherTypeIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpEtherTypeGet(devNum, etherTypeIndex, &etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", etherType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpUdpDestPortSet
*
* DESCRIPTION:
*       Configure UDP destination port0/port1 of PTP over UDP packets.
*       A packet is identified as PTP over UDP if it is a UDP packet, whose 
*       destination port matches one of the configured ports.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index , values 0..1
*       udpPortNum      - UDP port1/port0 number according to the index, 
*                         values 0..0xFFFF
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_OUT_OF_RANGE          - on wrong udpPortNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpUdpDestPortsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      udpPortIndex;
    GT_U32      udpPortNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    udpPortIndex = (GT_U32)inArgs[1];
    udpPortNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpUdpDestPortsSet(devNum, udpPortIndex, udpPortNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpUdpDestPortGet
*
* DESCRIPTION:
*       Get UDP destination port0/port1 of PTP over UDP packets.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       udpPortIndex    - UDP port index , values 0..1
*
* OUTPUTS:
*       udpPortNumPtr     - (pointer to) UDP port0/port1 number, according to 
*                           the index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or udpPortIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpUdpDestPortsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      udpPortIndex;
    GT_U32      udpPortNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    udpPortIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpUdpDestPortsGet(devNum, udpPortIndex, &udpPortNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", udpPortNum);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPtpMessageTypeCmdSet
*
* DESCRIPTION:
*       Configure packet command per PTP message type.
*       The message type is extracted from the PTP header.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       messageType     - message type, values 0..15
*       command         - assigned command to the packet. 
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or command
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpMessageTypeCmdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    GT_U32                  messageType;
    CPSS_PACKET_CMD_ENT     command;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    messageType = (GT_U32)inArgs[1];
    command = (CPSS_PACKET_CMD_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpMessageTypeCmdSet(devNum, messageType, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeCmdGet
*
* DESCRIPTION:
*       Get packet command per PTP message type.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       messageType     - message type, values 0..15.
*
* OUTPUTS:
*       commandPtr      - (pointer to) assigned command to the packet. 
*                         Acceptable values: FORWARD, MIRROR, TRAP, HARD_DROP and
*                                            SOFT_DROP.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or messageType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpMessageTypeCmdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    GT_U32                  messageType;
    CPSS_PACKET_CMD_ENT     command;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    messageType = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpMessageTypeCmdGet(devNum, messageType, &command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", command);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpCpuCodeBaseSet
*
* DESCRIPTION:
*       Set CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType 
*       (where message type is taken from PTP header).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       cpuCode         - The base of CPU code assigned to PTP packets mirrored  
*                         or trapped to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpCpuCodeBaseSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpCpuCodeBaseSet(devNum, cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpCpuCodeBaseGet
*
* DESCRIPTION:
*       Get CPU code base assigned to trapped and mirrored PTP packets.
*       A PTP packet's CPU code is computed as base CPU code + messageType 
*       (where message type is taken from PTP header).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       cpuCodePtr      - (pointer to) The base of CPU code assigned to PTP 
*                         packets mirrored or trapped to CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpCpuCodeBaseGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPtpCpuCodeBaseGet(devNum, &cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cpuCode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionSet
*
* DESCRIPTION:
*       The function configures type of TOD counter action that will be 
*       performed once triggered by cpssDxChPtpTodCounterFunctionTriggerSet() 
*       API or by pulse from an external interface.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       function        - One of the four possible TOD counter functions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction 
*                                  or function
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_PORT_DIRECTION_ENT             direction;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT  function;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    function = (CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterFunctionSet(devNum, direction, 
                                                      function);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionGet
*
* DESCRIPTION:
*       Get type of TOD counter action that will be performed once triggered by 
*       cpssDxChPtpTodCounterFunctionTriggerSet() API or by pulse from an 
*       external interface.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       functionPtr     - (pointer to) One of the four possible TOD counter 
*                          functions
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on bad value found in HW
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_PORT_DIRECTION_ENT             direction;
    CPSS_DXCH_PTP_TOD_COUNTER_FUNC_ENT  function;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterFunctionGet(devNum, direction, &function);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", function);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerSet
*
* DESCRIPTION:
*       Trigger TOD (Time of Day) counter function accorging to the function set
*       by cpssDxChPtpTodCounterFunctionSet().
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - when previous TOD triggered action is not finished yet
*       GT_BAD_PARAM             - on wrong devNum or direction 
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionTriggerSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterFunctionTriggerSet(devNum, direction);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterFunctionTriggerGet
*
* DESCRIPTION:
*       Get status of trigger TOD (Time of Day) counter function.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The trigger is on, the TOD 
*                                               function is not finished.
*                         GT_FALSE - The trigger is off, the TOD function is 
*                                   finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction 
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterFunctionTriggerGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterFunctionTriggerGet(devNum, direction, 
                                                             &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterShadowSet
*
* DESCRIPTION:
*       Configure TOD timestamping shadow counter values.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       todCounterPtr   - pointer to TOD counter shadow
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction 
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterShadowSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    CPSS_DXCH_PTP_TOD_COUNT_STC todCounter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    todCounter.nanoSeconds = (GT_U32)inArgs[2];
    todCounter.seconds.l[0] = (GT_U32)inArgs[3];
    todCounter.seconds.l[1] = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterShadowSet(devNum, direction, 
                                                    &todCounter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterShadowGet
*
* DESCRIPTION:
*       Get TOD timestamping shadow counter values.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter shadow
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction 
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterShadowGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    CPSS_DXCH_PTP_TOD_COUNT_STC todCounter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterShadowGet(devNum, direction, 
                                                    &todCounter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%X%X%X", todCounter.nanoSeconds, 
                 todCounter.seconds.l[0], todCounter.seconds.l[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTodCounterGet
*
* DESCRIPTION:
*       Get TOD timestamping counter value.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*
* OUTPUTS:
*       todCounterPtr      - (pointer to) TOD counter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction 
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTodCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    CPSS_DXCH_PTP_TOD_COUNT_STC todCounter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChPtpTodCounterGet(devNum, direction, &todCounter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%X%X%X", todCounter.nanoSeconds, 
                 todCounter.seconds.l[0], todCounter.seconds.l[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpPortTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable port for PTP timestamping.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       portNum         - port number 
*                         Note: For Ingress timestamping the trigger is based 
*                               on the local source port.
*                               For Egress timestamping the trigger is based 
*                               on the local target port.
*       enable          - GT_TRUE - The port is enabled for ingress/egress 
*                                   timestamping.
*                         GT_FALSE - The port is disabled for ingress/egress 
*                                   timestamping.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpPortTimeStampEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_U8                       portNum;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    portNum = (GT_U8)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpPortTimeStampEnableSet(devNum, direction, portNum, 
                                               enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpPortTimeStampEnableGet
*
* DESCRIPTION:
*       Get port PTP timestamping status (enabled/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       portNum         - port number 
*                         Note: For Ingress timestamping the trigger is based 
*                               on the local source port.
*                               For Egress timestamping the trigger is based 
*                               on the local target port.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - The port is enabled for 
*                                               ingress/egress timestamping.
*                         GT_FALSE - The port is disabled for ingress/egress 
*                                   timestamping.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, direction or portNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpPortTimeStampEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_U8                       portNum;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    portNum = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPtpPortTimeStampEnableGet(devNum, direction, portNum, 
                                               &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPtpEthernetTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping over Ethernet packets.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       enable          - GT_TRUE - PTP Timestamping is enabled over Ethernet.
*                         GT_FALSE - PTP Timestamping is disabled over Ethernet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEthernetTimeStampEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpEthernetTimeStampEnableSet(devNum, direction, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPtpEthernetTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status over Ethernet packets (enable/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is enabled 
*                                               over Ethernet.
*                         GT_FALSE - PTP Timestamping is disabled over Ethernet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpEthernetTimeStampEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpEthernetTimeStampEnableGet(devNum, direction, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpUdpTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping over UDP packets.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       protocolStack   - types of IP to set: IPV4 or IPV6.
*       enable          - GT_TRUE - PTP Timestamping is enabled over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Timestamping is disabled over UDP (IPV4/IPV6).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpUdpTimeStampEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpUdpTimeStampEnableSet(devNum, direction, protocolStack, 
                                              enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPtpUdpTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status over UDP packets (enable/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       protocolStack   - types of IP to set: IPV4 or IPV6.
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is enabled 
*                                               over UDP (IPV4/IPV6).
*                         GT_FALSE - PTP Timestamping is disabled over UDP (IPV4/IPV6).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, protocolStack or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpUdpTimeStampEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    CPSS_IP_PROTOCOL_STACK_ENT  protocolStack;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpUdpTimeStampEnableGet(devNum, direction, protocolStack, 
                                              &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeTimeStampEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping for specific message type of the packet.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
*       messageType     - message type (0..15).
*       enable          - GT_TRUE - PTP Timestamping is enabled for specific message type.
*                         GT_FALSE - PTP Timestamping is disabled for specific message type.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpMessageTypeTimeStampEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_U32                      messageType;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    messageType = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpMessageTypeTimeStampEnableSet(devNum, direction, 
                                                      messageType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpMessageTypeTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status for specific message type of the packet 
*       (enable/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       messageType     - message type (0..15).
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is enabled 
*                                               for specific message type.
*                         GT_FALSE - PTP Timestamping is disabled for specific 
*                         message type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, messageType or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpMessageTypeTimeStampEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_U32                      messageType;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    messageType = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpMessageTypeTimeStampEnableGet(devNum, direction, 
                                                      messageType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable PTP Timestamping check of the packet's transport specific value.
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx, Rx or both directions
*       enable      - GT_TRUE - PTP Timestamping check of the packet's transport 
*                               specific value is enabled. 
*                     GT_FALSE - PTP Timestamping check of the packet's transport 
*                                specific value is disabled. 
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTransportSpecificCheckEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpTransportSpecificCheckEnableSet(devNum, direction, 
                                                        enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificCheckEnableGet
*
* DESCRIPTION:
*       Get status of PTP Timestamping check of the packet's transport specific 
*       value (the check is enabled/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum      - device number
*       direction   - Tx or Rx
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - PTP Timestamping check of the 
*                                  packet's transport specific value is enabled. 
*                     GT_FALSE - PTP Timestamping check of the packet's transport 
*                                specific value is disabled. 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTransportSpecificCheckEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpTransportSpecificCheckEnableGet(devNum, direction, 
                                                        &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificTimeStampEnableSet
*
* DESCRIPTION:
*       Enable PTP Timestamping for transport specific value of the packet.
*       Note: Relevant only if global configuration determines whether the
*       transport specific field should be checked by 
*       cpssDxChPtpTransportSpecificCheckEnableSet().
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum                  - device number
*       direction               - Tx, Rx or both directions
*       transportSpecificVal    - transport specific value taken from PTP header 
*                                 bits [7..4]. Acceptable values (0..15).
*       enable                  - GT_TRUE - PTP Timestamping is enabled for this 
*                                           transport specific value.
*                                GT_FALSE - PTP Timestamping is disabled for this 
*                                           transport specific value.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or 
*                                  direction
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTransportSpecificTimeStampEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_U32                      transportSpecificVal;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    transportSpecificVal = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPtpTransportSpecificTimeStampEnableSet(devNum, direction, 
                                                            transportSpecificVal,
                                                            enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPtpTransportSpecificTimeStampEnableGet
*
* DESCRIPTION:
*       Get PTP Timestamping status for specific message type of the packet 
*       (enable/disabled).
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx or Rx
*       transportSpecificVal    - transport specific value taken from PTP header 
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - PTP Timestamping is 
                                      enabled for this transport specific value.
*                         GT_FALSE - PTP Timestamping is disabled for this 
*                                    transport specific value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, transportSpecificVal or 
*                                  direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTransportSpecificTimeStampEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_PORT_DIRECTION_ENT     direction;
    GT_U32                      transportSpecificVal;
    GT_BOOL                     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];
    transportSpecificVal = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPtpTransportSpecificTimeStampEnableGet(devNum, direction, 
                                                            transportSpecificVal,
                                                            &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChPtpTimestampEntryGet
*
* DESCRIPTION:
*       Read current Timestamp entry from the timestamp FIFO queue. 
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       direction       - Tx, Rx or both directions
* OUTPUTS:
*       entryPtr        - (pointer to) timestamp entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE               - the action succeeded and there are no more waiting
*                                  Timestamp entries
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or direction
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPtpTimestampEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_PORT_DIRECTION_ENT             direction;
    CPSS_DXCH_PTP_TIMESTAMP_ENTRY_STC   entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPtpTimestampEntryGet(devNum, direction, &entry);

    /* convert port number */
    CONVERT_BACK_DEV_PORT_DATA_MAC(devNum, entry.portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d %d %d %d", entry.portNum, entry.messageType, 
                 entry.sequenceId, entry.timeStampVal);

    return CMD_OK;
}




/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChPtpEtherTypeSet",
        &wrCpssDxChPtpEtherTypeSet,
        3, 0},

    {"cpssDxChPtpEtherTypeGet",
        &wrCpssDxChPtpEtherTypeGet,
        2, 0},

    {"cpssDxChPtpUdpDestPortsSet",
        &wrCpssDxChPtpUdpDestPortsSet,
        3, 0},

    {"cpssDxChPtpUdpDestPortsGet",
        &wrCpssDxChPtpUdpDestPortsGet,
        2, 0},

    {"cpssDxChPtpMessageTypeCmdSet",
        &wrCpssDxChPtpMessageTypeCmdSet,
        3, 0},

    {"cpssDxChPtpMessageTypeCmdGet",
        &wrCpssDxChPtpMessageTypeCmdGet,
        2, 0},

    {"cpssDxChPtpCpuCodeBaseSet",
        &wrCpssDxChPtpCpuCodeBaseSet,
        2, 0},

    {"cpssDxChPtpCpuCodeBaseGet",
        &wrCpssDxChPtpCpuCodeBaseGet,
        1, 0},

    {"cpssDxChPtpTodCounterFunctionSet",
        &wrCpssDxChPtpTodCounterFunctionSet,
        3, 0},

    {"cpssDxChPtpTodCounterFunctionGet",
        &wrCpssDxChPtpTodCounterFunctionGet,
        2, 0},

    {"cpssDxChPtpTodCounterFunctionTriggerSet",
        &wrCpssDxChPtpTodCounterFunctionTriggerSet,
        2, 0},

    {"cpssDxChPtpTodCounterFunctionTriggerGet",
        &wrCpssDxChPtpTodCounterFunctionTriggerGet,
        2, 0},

    {"cpssDxChPtpTodCounterShadowSet",
        &wrCpssDxChPtpTodCounterShadowSet,
        5, 0},

    {"cpssDxChPtpTodCounterShadowGet",
        &wrCpssDxChPtpTodCounterShadowGet,
        2, 0},

    {"cpssDxChPtpTodCounterGet",
        &wrCpssDxChPtpTodCounterGet,
        2, 0},

    {"cpssDxChPtpPortTimeStampEnableSet",
        &wrCpssDxChPtpPortTimeStampEnableSet,
        4, 0},

    {"cpssDxChPtpPortTimeStampEnableGet",
        &wrCpssDxChPtpPortTimeStampEnableGet,
        3, 0},

    {"cpssDxChPtpEthernetTimeStampEnableSet",
        &wrCpssDxChPtpEthernetTimeStampEnableSet,
        3, 0},

    {"cpssDxChPtpEthernetTimeStampEnableGet",
        &wrCpssDxChPtpEthernetTimeStampEnableGet,
        2, 0},

    {"cpssDxChPtpUdpTimeStampEnableSet",
        &wrCpssDxChPtpUdpTimeStampEnableSet,
        4, 0},

    {"cpssDxChPtpUdpTimeStampEnableGet",
        &wrCpssDxChPtpUdpTimeStampEnableGet,
        3, 0},

    {"cpssDxChPtpMessageTypeTimeStampEnableSet",
        &wrCpssDxChPtpMessageTypeTimeStampEnableSet,
        4, 0},

    {"cpssDxChPtpMessageTypeTimeStampEnableGet",
        &wrCpssDxChPtpMessageTypeTimeStampEnableGet,
        3, 0},

    {"cpssDxChPtpTransportSpecificCheckEnableSet",
        &wrCpssDxChPtpTransportSpecificCheckEnableSet,
        3, 0},

    {"cpssDxChPtpTransportSpecificCheckEnableGet",
        &wrCpssDxChPtpTransportSpecificCheckEnableGet,
        2, 0},

    {"cpssDxChPtpTransportSpecificTimeStampEnableSet",
        &wrCpssDxChPtpTransportSpecificTimeStampEnableSet,
        4, 0},

    {"cpssDxChPtpTransportSpecificTimeStampEnableGet",
        &wrCpssDxChPtpTransportSpecificTimeStampEnableGet,
        3, 0},

    {"cpssDxChPtpTimestampEntryGet",
        &wrCpssDxChPtpTimestampEntryGet,
        2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPtp
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
CMD_STATUS cmdLibInitCpssDxChPtp
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


