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
* wrapBridgeCpssDxCh.c
*
* DESCRIPTION:
*       Wrapper functions for Bridge cpss.dxCh functions
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
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgEgrFlt.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdbHash.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgStp.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgMc.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgNestVlan.h>

#if 0
/*******************************************************************************
* cpssDxChBrgMacHashCalc
*
* DESCRIPTION:
*       This function calculates the hash index for the mac address table.
*       for specific mac address and VLAN id.
*
* APPLICABLE DEVICES: All DxCh device
*
* INPUTS:
*       devNum      - device number
*       addrPtr     - (pointer to) the mac address.
*       vid         - the VLAN id.
*
* OUTPUTS:
*       hashPtr - (pointer to) the hash index.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgMacHashCalc

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    GT_U8           devNum;
    GT_ETHERADDR    addrPtr;
    GT_U16          vid;
    GT_U32          hashPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    galtisMacAddr(&addrPtr, (GT_U8*)inArgs[1]);
    vid = (GT_U16)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgMacHashCalc(devNum, &addrPtr, vid, &hashPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", hashPtr);
    return CMD_OK;
}
#endif /*0*/



/*******************************************************************************
* cpssDxChBrgStpInit
*
* DESCRIPTION:
*       Initialize STP library.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev    - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgStpInit

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgStpInit(dev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgStpStateSet
*
* DESCRIPTION:
*       Sets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev    - device number
*       port   - port number
*       stpId  - STG (Spanning Tree Group) index  (0..255)
*       state  - STP port state.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on bad device or port or stpId or state
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgStpStateSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         dev;
    GT_U8                         port;
    GT_U16                        stpId;
    CPSS_STP_STATE_ENT            state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stpId = (GT_U16)inArgs[2];
    state = (CPSS_STP_STATE_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgStpStateSet(dev, port, stpId, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgStpStateGet
*
* DESCRIPTION:
*       Gets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       dev         - device number
*       port        - port number
*       stpId       - STG (Spanning Tree Group) index  (0..255)
*
* OUTPUTS:
*       statePtr    - (pointer to) STP port state.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or port or stpId or state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgStpStateGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         dev;
    GT_U8                         port;
    GT_U16                        stpId;
    CPSS_STP_STATE_ENT            state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stpId = (GT_U16)inArgs[2];
    state = (CPSS_STP_STATE_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgStpStateGet(dev, port, stpId, &state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", state);

    return CMD_OK;
}

/*cpssDxChBrgStpEntry Table*/
static GT_U16   stpIdCnt;

/*
* cpssDxChBrgStpEntryGet
*
* DESCRIPTION:
*       Gets the RAW STP entry from HW.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev    - device number
*       stpId  - STG (Spanning Tree Group) index  (0..255)
*
* OUTPUTS:
*       stpEntryPtr     - (Pointer to) The raw stp entry to get.
*                         memory allocated by the caller.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on bad device or stpId
*       GT_BAD_PTR  - on NULL pointer parameter
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgStpEntryGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          dev, i;
    GT_U32         stpEntryPtr[CPSS_DXCH_STG_ENTRY_SIZE_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    stpIdCnt = 0;

    /* call cpss api function */
    result = cpssDxChBrgStpEntryGet(dev, stpIdCnt, stpEntryPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = stpIdCnt;

    for(i = 0; i < CPSS_DXCH_STG_ENTRY_SIZE_CNS; i++)
    {
        inFields[i + 1] = stpEntryPtr[i];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/

static CMD_STATUS wrCpssDxChBrgStpEntryGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          dev, i;
    GT_U32         stpEntryPtr[CPSS_DXCH_STG_ENTRY_SIZE_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    stpIdCnt++;

    if(stpIdCnt > 255)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


    /* call cpss api function */
    result = cpssDxChBrgStpEntryGet(dev, stpIdCnt, stpEntryPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = stpIdCnt;

    for(i = 0; i < CPSS_DXCH_STG_ENTRY_SIZE_CNS; i++)
    {
        inFields[i + 1] = stpEntryPtr[i];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgStpEntryWrite
*
* DESCRIPTION:
*       Writes a raw STP entry to HW.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev             - device number
*       stpId           - STG (Spanning Tree Group) index  (0..255)
*       stpEntryPtr     - (Pointer to) The raw stp entry to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on bad device or stpId
*       GT_BAD_PTR  - on NULL pointer parameter
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgStpEntryWrite

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS     result;

    GT_U8         dev, i;
    GT_U16        stpId;
    GT_U32        stpEntryPtr[CPSS_DXCH_STG_ENTRY_SIZE_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];


    stpId = (GT_U8)inFields[0];

    for(i = 0; i < CPSS_DXCH_STG_ENTRY_SIZE_CNS; i++)
    {
        stpEntryPtr[i] = (GT_U32)inFields[i + 1];
    }

    /* call cpss api function */
    result = cpssDxChBrgStpEntryWrite(dev, stpId, stpEntryPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssDxChBrgPortEgrFltUnkEnable
*
* DESCRIPTION:
*       If enabled, bridged unknown unicast packets are filtered on the
*       specified egress port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       enable  - GT_TRUE/GT_FALSE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong dev or port
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPortEgrFltUnkEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U8    port;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgPortEgrFltUnkEnable(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgPortEgrFltUregMcastEnable
*
* DESCRIPTION:
*       If enabled, bridged unregistered multicast packets are filtered on the
*       specified egress port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       enable  - GT_TRUE/GT_FALSE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong dev or port
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPortEgrFltUregMcastEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U8    port;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgPortEgrFltUregMcastEnable(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgVlanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/disable VLAN Egress Filtering on bridged known unicast packets
*       on specified device.
*       When enabled, VLAN egress filtering verifies that the egress port is a
*                     member of the packet's VLAN classification
*
*       When disabled, the VLAN egress filtering check is disabled.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev - physical device number
*       enable  - GT_TRUE for enable or GT_FLASE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong dev
*
* COMMENTS:
*       VLAN egress filtering is required by 802.1Q, but if desired, this
*       mechanism can be disabled, thus allowing "leaky VLANs".
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEgressFilteringEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgVlanEgressFilteringEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgRoutedUnicastEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable vlan egress filtering on routed packets
*       The destination port may or may not be a member of the vlan
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev - physical device number
*       enable  - GT_TRUE for enable or GT_FLASE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong dev
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgRoutedUnicastEgressFilteringEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgRoutedUnicastEgressFilteringEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgRoutedSpanEgressFilteringEnable
*
* DESCRIPTION:
*       Enable/Disable STP egress Filtering on Routed Packets
*
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev - physical device number
*       enable  - GT_TRUE for enable or GT_FLASE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong dev
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgRoutedSpanEgressFilteringEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgRoutedSpanEgressFilteringEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgPortEgrFltUregBcEnable
*
* DESCRIPTION:
*       Enables or disables egress filtering of unregistered broadcast packets.
*       Unregistered broadcast packets are:
*        - packets with destination MAC ff-ff-ff-ff-ff-ff
*        - destination MAC address lookup not finds matching entry
*        - packets were not routed
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       dev     - device number
*       port    - port number
*       enable  - GT_TRUE  - enable filtering of unregistered broadcast packets.
*                            unregistered broadcast packets are dropped
*                 GT_FALSE - disable filtering of unregistered broadcast packets.
*                            unregistered broadcast packets are not dropped
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong dev
*       GT_NOT_SUPPORTED   - This request is not supported.
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPortEgrFltUregBcEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U8    port;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgPortEgrFltUregBcEnable(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgPortEgressMcastLocalEnable
*
* DESCRIPTION:
*       Enables filtering unregistered multicast packets
*
* INPUTS:
*       dev     - device number
*       port    - port number
*       enable  - enable/disable (TRUE/FALSE)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong dev
*
* COMMENTS:
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPortEgressMcastLocalEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_U8    port;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgPortEgressMcastLocalEnable(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgMcInit
*
* DESCRIPTION:
*       Initializes Bridge engine Multicast Library.
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*       devNum          - PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PARAM        - on wrong devNum.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgMcInit

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgMcInit(dev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgMcIpv6BytesSelectSet
*
* DESCRIPTION:
*
*       Sets selected 4 bytes from the IPv6 SIP and 4 bytes from the IPv6 DIP
*       are used upon IPv6 MC bridging, based on the packet's IP addresses (due
*       to the limitation of Cheetah FDB Entry, which contains only 32 bits for
*       DIP and 32 bits for SIP, in spite of 128 bits length of IPv6 address).
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       dipBytesSelectMap   - array, which contains numbers of 4 bytes from the
*                             DIP IPv6 address are used upon IPv6 MC bridging.
*                             The first array element contains DIP byte 0,
*                             respectively the fourth array element contains
*                             DIP byte 3.
*
*                             The numbers of IPv6 address bytes are in the
*                             Network format and in the range [0:15], where 0
*                             is MSB of IP address and 15 is LSB of IP address.
*
*       sipBytesSelectMap   - array, which contains numbers of 4 bytes from the
*                             SIP IPv6 address are used upon IPv6 MC bridging.
*                             The first array element contains SIP byte 0,
*                             respectively the fourth array element contains
*                             SIP byte 3.
*
*                             The numbers of IPv6 address bytes are in the
*                             Network format and in the range [0:15], where 0
*                             is MSB of IP address and 15 is LSB of IP address.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PTR          - on NULL pointers.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgMcIpv6BytesSelectSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS     result;

    GT_U8         devNum, i;
    GT_U8         dipBytesSelectMap[4];
    GT_U8         sipBytesSelectMap[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    for(i = 0; i < 4; i++)
    {
        dipBytesSelectMap[i] = (GT_U8)inFields[i];
    }

    for(i = 0; i < 4; i++)
    {
        sipBytesSelectMap[i] = (GT_U8)inFields[i + 4];
    }

    /* call cpss api function */
    result = cpssDxChBrgMcIpv6BytesSelectSet(devNum, dipBytesSelectMap,
                                                     sipBytesSelectMap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgMcIpv6BytesSelectGet
*
* DESCRIPTION:
*
*       Gets selected 4 bytes from the IPv6 SIP and 4 bytes from the IPv6 DIP
*       are used upon IPv6 MC bridging, based on the packet's IP addresses (due
*       to the limitation of Cheetah FDB Entry, which contains only 32 bits for
*       DIP and 32 bits for SIP, in spite of 128 bits length of IPv6 address).
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*
* OUTPUTS:
*       dipBytesSelectMap   - array, which contains numbers of 4 bytes from the
*                             DIP IPv6 address are used upon IPv6 MC bridging.
*                             The first array element contains DIP byte 0,
*                             respectively the fourth array element contains
*                             DIP byte 3.
*
*                             The numbers of IPv6 address bytes are in the
*                             Network format and in the range [0:15], where 0
*                             is MSB of IP address and 15 is LSB of IP address.
*
*       sipBytesSelectMap   - array, which contains numbers of 4 bytes from the
*                             SIP IPv6 address are used upon IPv6 MC bridging.
*                             The first array element contains SIP byte 0,
*                             respectively the fourth array element contains
*                             SIP byte 3.
*
*                             The numbers of IPv6 address bytes are in the
*                             Network format and in the range [0:15], where 0
*                             is MSB of IP address and 15 is LSB of IP address.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PTR          - on NULL pointers.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgMcIpv6BytesSelectGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS     result;

    GT_U8         devNum, i;
    GT_U8         dipBytesSelectMap[4];
    GT_U8         sipBytesSelectMap[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgMcIpv6BytesSelectGet(devNum, dipBytesSelectMap,
                                                     sipBytesSelectMap);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    for(i = 0; i < 4; i++)
    {
        inFields[i] = dipBytesSelectMap[i];
    }

    for(i = 0; i < 4; i++)
    {
        inFields[i + 4] = sipBytesSelectMap[i];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1],
                       inFields[2], inFields[3], inFields[4],
                       inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/

static CMD_STATUS wrCpssDxChBrgMcIpv6BytesSelectGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}
/*******************************************************************************
* table cpssDxChBrgMc
*
*
* cpssDxChBrgMcEntryWrite
*
* DESCRIPTION:
*       Writes Multicast Group entry to the HW.
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
*
* INPUTS:
*       devNum          - PP's device number.
*       vidx            - multicast group index (VIDX<4095).
*       portBitmapPtr   - pointer to the bitmap of ports are belonged to the
*                         vidx MC group of specified PP device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum or ports bitmap value.
*       GT_OUT_OF_RANGE     - if vidx is larger than the allowed value.
*
* COMMENTS:
*       The function doesn't allow rewrite the VIDX 0xFFF Multicast Table Entry.
*       It should allow the Multicast packets to be flooded to all VLAN ports.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgMcEntryWrite

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_U16                    vidx;
    CPSS_PORTS_BMP_STC        portBitmap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    vidx = (GT_U16)inFields[0];

    portBitmap.ports[0] = inFields[1];
    portBitmap.ports[1] = inFields[2];
    CONVERT_DEV_PORTS_BMP_MAC(devNum,portBitmap);

    /* call cpss api function */
    result = cpssDxChBrgMcEntryWrite(devNum, vidx, &portBitmap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*
* cpssDxChBrgMcEntryRead
*
* DESCRIPTION:
*       Reads the Multicast Group entry from the HW.
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*       devNum          - PP's device number.
*       vidx            - multicast group index (VIDX<=4095).
*
* OUTPUTS:
*       portBitmapPtr   - pointer to the bitmap of ports are belonged to the
*                         vidx MC group of specified PP device.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_OUT_OF_RANGE     - if vidx is larger than the allowed value.
*
* COMMENTS:
*/

static GT_U16   vidxIndex;
static GT_U16   vidxMaxGet;

/******************************************************************************/
static CMD_STATUS wrCpssDxChBrgMcEntryReadFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_U16                    tmpEntryGet;
    CPSS_PORTS_BMP_STC        portBitmap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vidxIndex = (GT_U16)inArgs[1];
    tmpEntryGet = (GT_U16)inArgs[2];

    vidxMaxGet = vidxIndex + tmpEntryGet;

    /* call cpss api function */
    result = cpssDxChBrgMcEntryRead(devNum, vidxIndex, &portBitmap);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    CONVERT_BACK_DEV_PORTS_BMP_MAC(devNum,portBitmap);
    inFields[0] = vidxIndex;
    inFields[1] = portBitmap.ports[0];
    inFields[2] = portBitmap.ports[1];


    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChBrgMcEntryReadNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    CPSS_PORTS_BMP_STC        portBitmap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vidxIndex++;

    if(vidxIndex > 4095 || vidxIndex >= vidxMaxGet)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChBrgMcEntryRead(devNum, vidxIndex, &portBitmap);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    CONVERT_BACK_DEV_PORTS_BMP_MAC(devNum,portBitmap);
    inFields[0] = vidxIndex;
    inFields[1] = portBitmap.ports[0];
    inFields[2] = portBitmap.ports[1];


    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgMcGroupDelete
*
* DESCRIPTION:
*       Deletes the Multicast Group entry from the HW.
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*       devNum      - PP's device number.
*       vidx        - multicast group index (VIDX<4095).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_OUT_OF_RANGE     - if vidx is larger than the allowed value.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgMcGroupDelete

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vidx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    vidx = (GT_U16)inFields[0];

    /* call cpss api function */
    result = cpssDxChBrgMcGroupDelete(devNum, vidx);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgMcMemberAdd
*
* DESCRIPTION:
*       Add new port member to the Multicast Group entry.
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*       devNum          - PP's device number.
*       vidx            - multicast group index (VIDX<4095).
*       portNum         - physical port number.
*                         Range of portNum is: [0-27] for DXCh2;
*                                              [0-26] for DXCh.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_OUT_OF_RANGE     - if vidx is larger than the allowed value.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgMcMemberAdd

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vidx;
    GT_U8    portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vidx = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgMcMemberAdd(devNum, vidx, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgMcMemberDelete
*
* DESCRIPTION:
*       Delete port member from the Multicast Group entry.
*
* APPLICABLE DEVICES: ALL DxCh Devices.
*
* INPUTS:
*       devNum          - PP's device number.
*       vidx            - multicast group index (VIDX<4095).
*       portNum         - physical port number.
*                         Range of portNum is: [0-27] for DxCh2;
*                                              [0-26] for DxCh.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_OUT_OF_RANGE     - if vidx is larger than the allowed value.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgMcMemberDelete

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U16   vidx;
    GT_U8    portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vidx = (GT_U16)inArgs[1];
    portNum = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgMcMemberDelete(devNum, vidx, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgNestVlanAccessPortSet
*
* DESCRIPTION:
*       Configure given port as nested VLAN access port.
*       The VID of all the packets received on nested VLAN access port is
*       discarded and they are assigned with the Port VID that set by
*       cpssDxChBrgVlanPortVidSet(). This VLAN assignment may be subsequently
*       overridden by the protocol based VLANs or policy based VLANs algorithms.
*       When a packet received on an access port is transmitted via a core port
*       or a cascading port, a VLAN tag is added to the packet, in addition to
*       any existing VLAN tag.
*       The 802.1p User Priority field of this added tag may be one of the
*       following, based on the ModifyUP QoS parameter set to the packet at
*       the end of the Ingress pipe:
*              - If ModifyUP is "Modify", it is the UP extracted from the
*              QoS Profile table entry that configured by the
*              cpssDxChCosProfileEntrySet().
*              - If ModifyUP is "Not Modify", it is the original packet
*              802.1p User Priority field, if it is tagged. If untagged,
*              it is ingress port's default user priority that configured by
*              cpssDxChPortDefaultUPSet().
*
* APPLICABLE DEVICES:  All DxCh devices.
*
* INPUTS:
*       devNum  - PP's device number.
*       portNum - physical port number.
*       enable  -  GT_TRUE  - port is nested VLAN Access Port.
*                  GT_FALSE - port is usual port (Core or Customers).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum.
*
* COMMENTS:
*       To complete Access Port Configuration do the following:
*       -  by API cpssDxChBrgNestVlanEtherTypeSelectSet(), set
*          the Port VLAN Selection to the CPSS_VLAN_ETHERTYPE0_E;
*       -  update egress Port VLAN EtherType;
*       -  by API cpssDxChBrgVlanForcePvidEnable(), set
*          Port VID Assignment mode.
*       To complete Core Port Configuration, select CPSS_VLAN_ETHERTYPE1_E
*       for Usual Port.
*       To complete Customers Bridges(bridges that don't employ nested VLANs)
*       Configuration, select CPSS_VLAN_ETHERTYPE0_E for Usual Port.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgNestVlanAccessPortSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_U8        portNum;
    GT_BOOL      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgNestVlanAccessPortSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgNestVlanAccessPortGet
*
* DESCRIPTION:
*       Gets configuration of the given port (Nested VLAN access port
*       or Core/Customer port).
*       The VID of all the packets received on Nested VLAN access port is
*       discarded and they are assigned with the Port VID that set by
*       cpssDxChBrgVlanPortVidSet(). This VLAN assignment may be subsequently
*       overridden by the protocol based VLANs or policy based VLANs algorithms.
*       When a packet received on an access port is transmitted via a core port
*       or a cascading port, a VLAN tag is added to the packet, in addition to
*       any existing VLAN tag.
*       The 802.1p User Priority field of this added tag may be one of the
*       following, based on the ModifyUP QoS parameter set to the packet at
*       the end of the Ingress pipe:
*              - If ModifyUP is "Modify", it is the UP extracted from the
*              QoS Profile table entry that configured by the
*              cpssDxChCosProfileEntrySet().
*              - If ModifyUP is "Not Modify", it is the original packet
*              802.1p User Priority field, if it is tagged. If untagged,
*              it is ingress port's default user priority that configured by
*              cpssDxChPortDefaultUPSet().
*
* APPLICABLE DEVICES:  All DxCh devices.
*
* INPUTS:
*       devNum      - PP's device number.
*       portNum     - physical port number.
*
* OUTPUTS:
*       enablePtr   - Pointer to the Boolean value:
*                     GT_TRUE   - port is Nested VLAN Access Port.
*                     GT_FALSE  - port is usual port:
*                                      - Core Port in case of
*                                         CPSS_VLAN_ETHERTYPE1_E selected;
*                                      - Customers Bridges (bridges that don't
*                                         employ Nested VLANs) in case of
*                                         CPSS_VLAN_ETHERTYPE0_E selected;
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum values.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgNestVlanAccessPortGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_U8        portNum;
    GT_BOOL      enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgNestVlanAccessPortGet(devNum, portNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgNestVlanEtherTypeSelectSet
*
* DESCRIPTION:
*       Sets selected port VLAN EtherType.
*
* APPLICABLE DEVICES:  All DxCh devices.
*
* INPUTS:
*       devNum          - PP's device number.
*       portNum         - physical port number.
*       ingressVlanSel  - selected port VLAN EtherType.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum or ingressVlanSel values.
*
* COMMENTS:
*       For the VLAN EtherType coherency between Ingress and Egress,
*       the VLAN EtherType shall be configured to match the ingressVlanSel.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgNestVlanEtherTypeSelectSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                result;

    GT_U8                    devNum;
    GT_U8                    portNum;
    CPSS_ETHER_MODE_ENT      ingressVlanSel;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ingressVlanSel = (CPSS_ETHER_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgNestVlanEtherTypeSelectSet(devNum, portNum,
                                                    ingressVlanSel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgNestVlanEtherTypeSelectGet
*
* DESCRIPTION:
*       Gets selected port VLAN EtherType.
*
* APPLICABLE DEVICES:  All DxCh devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       portNum             - physical port number.
*
* OUTPUTS:
*       ingressVlanSelPtr   - pointer to selected port VLAN EtherType.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or portNum values.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgNestVlanEtherTypeSelectGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                result;

    GT_U8                    devNum;
    GT_U8                    portNum;
    CPSS_ETHER_MODE_ENT      ingressVlanSelPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgNestVlanEtherTypeSelectGet(devNum, portNum,
                                                &ingressVlanSelPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ingressVlanSelPtr);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChBrgPortEgrFltUnkEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for bridged Unknown Unicast packets on the specified egress port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev         - device number
*       port        - CPU port, physical port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged unknown unicast packets filtering
*                   GT_TRUE:  Unknown Unicast packets are filtered and are
*                             not forwarded to this port.
*                   GT_FALSE: Unknown Unicast packets are not filtered and may
*                             be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPortEgrFltUnkEnableGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgPortEgrFltUnkEnableGet(dev, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChBrgPortEgrFltUregMcastEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for bridged Unregistered Multicast packets on the specified egress port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev         - device number
*       port        - CPU port, physical port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged unregistered multicast packets filtering
*                   GT_TRUE:  Unregistered Multicast packets are filtered and
*                             are not forwarded to this port.
*                   GT_FALSE: Unregistered Multicast packets are not filtered
*                             and may be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPortEgrFltUregMcastEnableGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgPortEgrFltUregMcastEnableGet(dev, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgVlanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on specified device for Bridged Known Unicast packets.
*      If enabled the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) bridged known unicast packets filtering
*                   GT_TRUE:   VLAN egress filtering verifies that the egress
*                              port is a member of the packet's VLAN classification
*                   GT_FLASE: the VLAN egress filtering check is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgVlanEgressFilteringEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 dev;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgVlanEgressFilteringEnableGet(dev, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChBrgRoutedUnicastEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the VLAN Egress Filtering current state (enable/disable)
*      on Routed Unicast packets.
*      If disabled, the destination port may or may not be a member of the VLAN.
*      If enabled, the VLAN egress filter verifies that the egress port is a
*      member of the VID assigned to the packet.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) routed unicast packets filtering
*                   GT_TRUE:  Egress filtering is enabled
*                   GT_FLASE: Egress filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgRoutedUnicastEgressFilteringEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 dev;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgRoutedUnicastEgressFilteringEnableGet(dev, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChBrgRoutedSpanEgressFilteringEnableGet
*
* DESCRIPTION:
*      This function gets the STP egress Filtering current state (enable/disable)
*      on Routed packets.
*      If disabled the packet may be forwarded to its egress port,
*      regardless of its Span State.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) routed packets filtering
*                   GT_TRUE:  Span state filtering is enabled
*                   GT_FLASE: Span state filtering is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgRoutedSpanEgressFilteringEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 dev;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgRoutedSpanEgressFilteringEnableGet(dev, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChBrgPortEgrFltUregBcEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      of unregistered broadcast packets.
*      Unregistered broadcast packets are:
*      - packets with destination MAC ff-ff-ff-ff-ff-ff
*      - destination MAC address lookup not finds matching entry
*      - packets were not routed
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       dev         - device number
*       port        - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) unregistered broadcast packets filtering
*                   GT_TRUE  - enable filtering of unregistered broadcast packets.
*                              unregistered broadcast packets are dropped
*                   GT_FALSE - disable filtering of unregistered broadcast packets.
*                              unregistered broadcast packets are not dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPortEgrFltUregBcEnableGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgPortEgrFltUregBcEnableGet(dev, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChBrgPortEgressMcastLocalEnableGet
*
* DESCRIPTION:
*      This function gets current state (enable/disable) for sending
*      Multicast packets back to its source port on the local device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev         - device number
*       port        - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) sending Multicast packets back to its source
*                   GT_TRUE  - Multicast packets may be sent back to
*                              their source port on the local device.
*                   GT_FALSE - Multicast packets are not sent back to
*                              their source port on the local device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPortEgressMcastLocalEnableGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgPortEgressMcastLocalEnableGet(dev, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgPortEgrFltIpmRoutedEnable
*
* DESCRIPTION:
*      Enable/Disable egress filtering for IP Multicast Routed packets
*      on the specified egress port.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       dev      - device number
*       port     - physical port number
*       enable   - GT_TRUE: IP Multicast Routed packets are filtered and are
*                           not forwarded to this port.
*                  GT_FALSE: IP Multicast Routed packets are not filtered and may
*                           be forwarded to this port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPortEgrFltIpMcRoutedEnable
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgPortEgrFltIpMcRoutedEnable(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChBrgPortEgrFltIpMcRoutedEnableGet
*
* DESCRIPTION:
*      This function gets the egress Filtering current state (enable/disable)
*      for IP Multicast Routed packets on the specified egress port.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       dev         - device number
*       port        - port number
*
* OUTPUTS:
*       enablePtr   - points to (enable/disable) IP Multicast Routed packets filtering
*                   GT_TRUE:  IP Multicast Routed packets are filtered and are
*                             not forwarded to this port.
*                   GT_FALSE: IP Multicast Routed packets are not filtered and may
*                             be forwarded to this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong dev or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgPortEgrFltIpMcRoutedEnableGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev,port);


    /* call cpss api function */
    result = cpssDxChBrgPortEgrFltIpMcRoutedEnableGet(dev, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
#if 0
    {"cpssDxChBrgMacHashCalc",
        &wrCpssDxChBrgMacHashCalc,
        3, 0},
#endif /* 0 */

    {"cpssDxChBrgStpInit",
        &wrCpssDxChBrgStpInit,
        1, 0},

    {"cpssDxChBrgStpStateSet",
        &wrCpssDxChBrgStpStateSet,
        4, 0},

    {"cpssDxChBrgStpStateGet",
        &wrCpssDxChBrgStpStateGet,
        3, 0},

    {"cpssDxChBrgStpEntryGetFirst",
        &wrCpssDxChBrgStpEntryGetFirst,
        1, 0},

    {"cpssDxChBrgStpEntryGetNext",
        &wrCpssDxChBrgStpEntryGetNext,
        1, 0},

    {"cpssDxChBrgStpEntrySet",
        &wrCpssDxChBrgStpEntryWrite,
        1, 2 + 1},

    {"cpssDxChBrgPortEgrFltUnkEnable",
        &wrCpssDxChBrgPortEgrFltUnkEnable,
        3, 0},

    {"cpssDxChBrgPortEgrFltUregMcastEnable",
        &wrCpssDxChBrgPortEgrFltUregMcastEnable,
        3, 0},

    {"cpssDxChBrgVlanEgressFilteringEnable",
        &wrCpssDxChBrgVlanEgressFilteringEnable,
        2, 0},

    {"cpssDxChBrgRoutedUnicastEgressFilteringEnable",
        &wrCpssDxChBrgRoutedUnicastEgressFilteringEnable,
        2, 0},

    {"cpssDxChBrgRoutedSpanEgressFilteringEnable",
        &wrCpssDxChBrgRoutedSpanEgressFilteringEnable,
        2, 0},

    {"cpssDxChBrgPortEgrFltUregBcEnable",
        &wrCpssDxChBrgPortEgrFltUregBcEnable,
        3, 0},

    {"cpssDxChBrgPortEgressMcastLocalEnable",
        &wrCpssDxChBrgPortEgressMcastLocalEnable,
        3, 0},

    {"cpssDxChBrgMcInit",
        &wrCpssDxChBrgMcInit,
        1, 0},

    {"cpssDxChBrgMcIpv6BytesSelectSet",
        &wrCpssDxChBrgMcIpv6BytesSelectSet,
        1, 7},

    {"cpssDxChBrgMcIpv6BytesSelectGetFirst",
        &wrCpssDxChBrgMcIpv6BytesSelectGetFirst,
        1, 0},

    {"cpssDxChBrgMcIpv6BytesSelectGetNext",
        &wrCpssDxChBrgMcIpv6BytesSelectGetNext,
        1, 0},

    {"cpssDxChBrgMcSet",
        &wrCpssDxChBrgMcEntryWrite,
        3, 3},

    {"cpssDxChBrgMcGetFirst",
        &wrCpssDxChBrgMcEntryReadFirst,
        3, 0},

    {"cpssDxChBrgMcGetNext",
        &wrCpssDxChBrgMcEntryReadNext,
        3, 0},

    {"cpssDxChBrgMcDelete",
        &wrCpssDxChBrgMcGroupDelete,
        3, 1},

    {"cpssDxChBrgMcMemberAdd",
        &wrCpssDxChBrgMcMemberAdd,
        3, 0},

    {"cpssDxChBrgMcMemberDelete",
        &wrCpssDxChBrgMcMemberDelete,
        3, 0},

    {"cpssDxChBrgNestVlanAccessPortSet",
        &wrCpssDxChBrgNestVlanAccessPortSet,
        3, 0},

    {"cpssDxChBrgNestVlanAccessPortGet",
        &wrCpssDxChBrgNestVlanAccessPortGet,
        2, 0},

    {"cpssDxChBrgNestVlanEtherTypeSelectSet",
        &wrCpssDxChBrgNestVlanEtherTypeSelectSet,
        3, 0},

    {"cpssDxChBrgNestVlanEtherTypeSelectGet",
        &wrCpssDxChBrgNestVlanEtherTypeSelectGet,
        2, 0},

    {"cpssDxChBrgPortEgrFltUnkEnableGet",
         &wrCpssDxChBrgPortEgrFltUnkEnableGet,
         2, 0},

    {"cpssDxChBrgPortEgrFltUregMcastEnableGet",
         &wrCpssDxChBrgPortEgrFltUregMcastEnableGet,
         2, 0},

    {"cpssDxChBrgVlanEgressFilteringEnableGet",
         &wrCpssDxChBrgVlanEgressFilteringEnableGet,
         1, 0},

    {"cpssDxChBrgRoutedUnicastEgressFilteringEnableGet",
         &wrCpssDxChBrgRoutedUnicastEgressFilteringEnableGet,
         1, 0},

    {"cpssDxChBrgRoutedSpanEgressFilteringEnableGet",
         &wrCpssDxChBrgRoutedSpanEgressFilteringEnableGet,
         1, 0},

    {"cpssDxChBrgPortEgrFltUregBcEnableGet",
         &wrCpssDxChBrgPortEgrFltUregBcEnableGet,
         2, 0},

    {"cpssDxChBrgPortEgressMcastLocalEnableGet",
         &wrCpssDxChBrgPortEgressMcastLocalEnableGet,
         2, 0},

    {"cpssDxChBrgPortEgrFltIpMcRoutedEnable",
         &wrCpssDxChBrgPortEgrFltIpMcRoutedEnable,
         3, 0},

    {"cpssDxChBrgPortEgrFltIpMcRoutedEnableGet",
         &wrCpssDxChBrgPortEgrFltIpMcRoutedEnableGet,
         2, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChBridge
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
GT_STATUS cmdLibInitCpssDxChBridge
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}



