/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmIpStat.c
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
#include <cpss/exMxPm/exMxPmGen/ip/cpssExMxPmIpStat.h>
#include <cpss/exMxPm/exMxPmGen/dit/cpssExMxPmDit.h>

/*******************************************************************************
* cpssExMxPmIpCounterConfigSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       A counter set can be bound to an interface or to Route entries.
*       Binding a counter to route entries updates the counter set by traffic
*       that matches the Route entry.
*       When binding counter to an interface, the interface is defined by:
*           {device+port or Trunk, VLAN, packet type: IPv4 or IPv6}
*       One or more of the elements can be configured to be a wildcard.
*       This function sets the binding mode and interface of a counter-set.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                - device number
*       counterIndex          - index of counter set (range 0..3)
*       bindMode              - binding mode, route entry or interface
*       interfaceConfigPtr    - in case the bindMode is Interface mode, points
*                               to the interface configuration
*                               Valid values for packet types:
*                                   CPSS_EXMXPM_IP_PACKET_TYPE_IPV4_COUNTER_MODE_E
*                                   CPSS_EXMXPM_IP_PACKET_TYPE_IPV6_COUNTER_MODE_E
*                                   CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad parameter
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpCounterConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 counterIndex;
    CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT bindMode;
    CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC interfaceConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =        (GT_U8)inArgs[0];
    counterIndex =  inFields[0];
    bindMode =      (CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT)inFields[1];

    switch (bindMode)
    {
        /* Interface counter */
        case CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E:
            interfaceConfig.portTrunk.devPort.devNum = devNum;
            /* Due to a problem in the wrapper command enum the
               CPSS_EXMXPM_IP_PACKET_TYPE_IPV4V6_COUNTER_MODE_E
               option is not supported in this wrapper*/
            switch(inFields[2])
            {
                case 0:
                    galtisOutput(outArgs, GT_BAD_VALUE, "");
                    return CMD_OK;
                case 1:
                    interfaceConfig.packetType = CPSS_EXMXPM_IP_PACKET_TYPE_IPV4_COUNTER_MODE_E;
                    break;
                case 2:
                    interfaceConfig.packetType = CPSS_EXMXPM_IP_PACKET_TYPE_IPV6_COUNTER_MODE_E;
                    break;
                case 3:
                    interfaceConfig.packetType = CPSS_EXMXPM_IP_PACKET_TYPE_MPLS_COUNTER_MODE_E;
                    break;
                case 4:
                    interfaceConfig.packetType = CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E;
                    break;
                default:
                    galtisOutput(outArgs, GT_BAD_VALUE, "");
                    return CMD_OK;
            }
            interfaceConfig.portTrunkMode = (CPSS_EXMXPM_IP_PORT_TRUNK_COUNTER_MODE_ENT)inFields[3];
            switch(interfaceConfig.portTrunkMode)
            {
                case CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E:
                    interfaceConfig.portTrunk.devPort.port =  (GT_U8)inFields[4];
                    break;
                case CPSS_EXMXPM_IP_TRUNK_COUNTER_MODE_E:
                    interfaceConfig.portTrunk.trunk = (GT_TRUNK_ID)inFields[5];
                    break;
                case CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E:
                    break;
                default:
                    galtisOutput(outArgs, GT_BAD_VALUE, "");
                    return CMD_OK;
            }

            interfaceConfig.vlanMode = (CPSS_EXMXPM_IP_VLAN_COUNTER_MODE_ENT)inFields[6];
            switch (interfaceConfig.vlanMode)
            {
                case CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E:
                    interfaceConfig.vlanId = (GT_U16)inFields[7];
                    break;

                case CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E:
                    break;

                default:
                    galtisOutput(outArgs, GT_BAD_VALUE, "");
                    return CMD_OK;
            }

        case CPSS_EXMXPM_IP_COUNTER_BIND_TO_ROUTE_ENTRY_E:
            break;

        default:
            galtisOutput(outArgs, GT_BAD_VALUE, "");
            return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPmIpCounterConfigSet(devNum, counterIndex, bindMode, &interfaceConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpCounterConfigGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       A counter set can be bound to an interface or to Route entries.
*       Binding a counter to route entries updates the counter set by traffic
*       that matches the Route entry.
*       When binding counter to an interface, the interface is defined by:
*           {device+port or Trunk, VLAN, packet type: IPv4 or IPv6}
*       One or more of the elements can be configured to be a wildcard.
*       This function gets the binding mode and interface of a counter-set.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                - device number
*       counterIndex          - index of counter set (range 0..3)
*
* OUTPUTS:
*       bindModePtr           - binding mode, route entry or interface
*       interfaceConfigPtr    - in case the bindMode is Interface mode, points
*                               to the interface configuration
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad parameter
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32 counterIndex;

static CMD_STATUS wrCpssExMxPmIpCounterConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT bindMode;
    CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC interfaceConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (counterIndex == 4)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpCounterConfigGet(devNum, counterIndex, &bindMode, &interfaceConfig);

    inFields[0] = counterIndex;
    inFields[1] = bindMode;

    switch (bindMode)
    {
                /* Interface counter */
        case CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E:
            /* Due to a problem in the wrapper command enum the
               CPSS_EXMXPM_IP_PACKET_TYPE_IPV4V6_COUNTER_MODE_E
               option is not supported in this wrapper*/
            switch(interfaceConfig.packetType)
            {
                case 0:
                    inFields[2] = 1;
                    break;
                case 1:
                    inFields[2] = 2;
                    break;
                case 3:
                    inFields[2] = 3;
                    break;
                case 4:
                    inFields[2] = 4;
                    break;
                default:
                    galtisOutput(outArgs, GT_BAD_STATE, "");
                    return CMD_OK;

            }

            inFields[3] = interfaceConfig.portTrunkMode;
            switch(interfaceConfig.portTrunkMode)
            {
                case CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E:
                    inFields[4] = interfaceConfig.portTrunk.devPort.port;
                    inFields[5] = 0;
                    break;
                case CPSS_EXMXPM_IP_TRUNK_COUNTER_MODE_E:
                    inFields[4]=0;
                    inFields[5] = interfaceConfig.portTrunk.trunk;
                    break;
                case CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E:
                    inFields[4]=0;
                    inFields[5]=0;
                    break;
                default:
                    galtisOutput(outArgs, GT_BAD_VALUE, "");
                    return CMD_OK;
            }

            inFields[6] = interfaceConfig.vlanMode;
            switch (interfaceConfig.vlanMode)
            {
                case CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E:
                    inFields[7] = interfaceConfig.vlanId;
                    break;

                case CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E:
                    inFields[7]=0;
                    break;

                default:
                  galtisOutput(outArgs, GT_BAD_VALUE, "");
                  return CMD_OK;

            }
            break;

        case CPSS_EXMXPM_IP_COUNTER_BIND_TO_ROUTE_ENTRY_E:
            inFields[2] = 0;
            inFields[3] = 0;
            inFields[4] = 0;
            inFields[5] = 0;
            inFields[6] = 0;
            inFields[7] = 0;
            break;

        default:
            galtisOutput(outArgs, GT_BAD_VALUE, "");
            return CMD_OK;
    }

    counterIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;

}

/********************************************************************************/
static CMD_STATUS wrCpssExMxPmIpCounterConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    counterIndex = 0;
    return wrCpssExMxPmIpCounterConfigGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxPmIpCounterConfigSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       A counter set can be bound to an interface or to Route entries.
*       Binding a counter to route entries updates the counter set by traffic
*       that matches the Route entry.
*       When binding counter to an interface, the interface is defined by:
*           {device+port or Trunk, VLAN, packet type: IPv4 or IPv6}
*       One or more of the elements can be configured to be a wildcard.
*       This function sets the binding mode and interface of a counter-set.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                - device number
*       counterIndex          - index of counter set (range 0..3)
*       bindMode              - binding mode, route entry or interface
*       interfaceConfigPtr    - in case the bindMode is Interface mode, points
*                               to the interface configuration
*                               Valid values for packet types:
*                                   CPSS_EXMXPM_IP_PACKET_TYPE_IPV4_COUNTER_MODE_E
*                                   CPSS_EXMXPM_IP_PACKET_TYPE_IPV6_COUNTER_MODE_E
*                                   CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad parameter
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2IpCounterConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 counterIndex;
    CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT bindMode;
    CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC interfaceConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =        (GT_U8)inArgs[0];
    counterIndex =  inFields[0];
    bindMode =      (CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT)inFields[1];

    switch (bindMode)
    {
        /* Interface counter */
        case CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E:
            interfaceConfig.portTrunk.devPort.devNum = devNum;
            interfaceConfig.packetType = (CPSS_EXMXPM_IP_PACKET_TYPE_COUNTER_MODE_ENT)inFields[2];
            interfaceConfig.portTrunkMode = (CPSS_EXMXPM_IP_PORT_TRUNK_COUNTER_MODE_ENT)inFields[3];
            switch(interfaceConfig.portTrunkMode)
            {
                case CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E:
                    interfaceConfig.portTrunk.devPort.port =  (GT_U8)inFields[4];
                    break;
                case CPSS_EXMXPM_IP_TRUNK_COUNTER_MODE_E:
                    interfaceConfig.portTrunk.trunk = (GT_TRUNK_ID)inFields[5];
                    break;
                case CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E:
                    break;
                default:
                    galtisOutput(outArgs, GT_BAD_VALUE, "");
                    return CMD_OK;
            }

            interfaceConfig.vlanMode = (CPSS_EXMXPM_IP_VLAN_COUNTER_MODE_ENT)inFields[6];
            switch (interfaceConfig.vlanMode)
            {
                case CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E:
                    interfaceConfig.vlanId = (GT_U16)inFields[7];
                    break;

                case CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E:
                    break;

                default:
                    galtisOutput(outArgs, GT_BAD_VALUE, "");
                    return CMD_OK;
            }

        case CPSS_EXMXPM_IP_COUNTER_BIND_TO_ROUTE_ENTRY_E:
            break;

        default:
            galtisOutput(outArgs, GT_BAD_VALUE, "");
            return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPmIpCounterConfigSet(devNum, counterIndex, bindMode, &interfaceConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpCounterConfigGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       A counter set can be bound to an interface or to Route entries.
*       Binding a counter to route entries updates the counter set by traffic
*       that matches the Route entry.
*       When binding counter to an interface, the interface is defined by:
*           {device+port or Trunk, VLAN, packet type: IPv4 or IPv6}
*       One or more of the elements can be configured to be a wildcard.
*       This function gets the binding mode and interface of a counter-set.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum                - device number
*       counterIndex          - index of counter set (range 0..3)
*
* OUTPUTS:
*       bindModePtr           - binding mode, route entry or interface
*       interfaceConfigPtr    - in case the bindMode is Interface mode, points
*                               to the interface configuration
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad parameter
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32 counterIndexPm2;

static CMD_STATUS wrCpssExMxPm2IpCounterConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_IP_COUNTER_BIND_MODE_ENT bindMode;
    CPSS_EXMXPM_IP_COUNTER_SET_CFG_STC interfaceConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (counterIndexPm2 == 4)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpCounterConfigGet(devNum, counterIndexPm2, &bindMode, &interfaceConfig);

    inFields[0] = counterIndexPm2;
    inFields[1] = bindMode;

    switch (bindMode)
    {
                /* Interface counter */
        case CPSS_EXMXPM_IP_COUNTER_BIND_TO_INTERFACE_E:
            inFields[2] = interfaceConfig.packetType;
            inFields[3] = interfaceConfig.portTrunkMode;
            switch(interfaceConfig.portTrunkMode)
            {
                case CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E:
                    inFields[4] = interfaceConfig.portTrunk.devPort.port;
                    inFields[5] = 0;
                    break;
                case CPSS_EXMXPM_IP_TRUNK_COUNTER_MODE_E:
                    inFields[4]=0;
                    inFields[5] = interfaceConfig.portTrunk.trunk;
                    break;
                case CPSS_EXMXPM_IP_DISREGARD_PORT_TRUNK_COUNTER_MODE_E:
                    inFields[4]=0;
                    inFields[5]=0;
                    break;
                default:
                    galtisOutput(outArgs, GT_BAD_VALUE, "");
                    return CMD_OK;
            }

            inFields[6] = interfaceConfig.vlanMode;
            switch (interfaceConfig.vlanMode)
            {
                case CPSS_EXMXPM_IP_USE_VLAN_COUNTER_MODE_E:
                    inFields[7] = interfaceConfig.vlanId;
                    break;

                case CPSS_EXMXPM_IP_DISREGARD_VLAN_COUNTER_MODE_E:
                    inFields[7]=0;
                    break;

                default:
                  galtisOutput(outArgs, GT_BAD_VALUE, "");
                  return CMD_OK;

            }
            break;

        case CPSS_EXMXPM_IP_COUNTER_BIND_TO_ROUTE_ENTRY_E:
                inFields[2] = 0;
                inFields[3] = 0;
                inFields[4] = 0;
                inFields[5] = 0;
                inFields[6] = 0;
                inFields[7] = 0;
            break;

        default:
            galtisOutput(outArgs, GT_BAD_VALUE, "");
            return CMD_OK;
    }

    counterIndexPm2++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;

}

/********************************************************************************/
static CMD_STATUS wrCpssExMxPm2IpCounterConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    counterIndexPm2 = 0;
    return wrCpssExMxPm2IpCounterConfigGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxPmIpCounterSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This function sets the values of the various counters in the IP
*       Router Management Counter-Set.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       counterIndex  - index of counter set (range 0..3)
*       countersPtr   - the counters values to set to the counter set
*                       Valid values:
*                              CPSS_EXMXPM_IP_PACKET_TYPE_IPV4_COUNTER_MODE_E
*                              CPSS_EXMXPM_IP_PACKET_TYPE_IPV6_COUNTER_MODE_E
*                              CPSS_EXMXPM_IP_PACKET_TYPE_ALL_COUNTER_MODE_E
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad parameter
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpCounterSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 counterIndex;
    CPSS_EXMXPM_IP_COUNTER_SET_STC counters;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    counterIndex = inFields[0];
    counters.inUcPkts = inFields[1];
    counters.inMcPkts = inFields[2];
    counters.inUcNonRoutedExceptionPkts = inFields[3];
    counters.inUcNonRoutedNonExceptionPkts = inFields[4];
    counters.inMcNonRoutedExceptionPkts = inFields[5];
    counters.inMcNonRoutedNonExceptionPkts = inFields[6];
    counters.inUcTrappedMirrorPkts = inFields[7];
    counters.inMcTrappedMirrorPkts = inFields[8];
    counters.inMcRfpFailPkts = inFields[9];
    counters.outUcPkts = inFields[10];

    /* call cpss api function */
    result = cpssExMxPmIpCounterSet(devNum, counterIndex, &counters);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpCounterGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This function gets the values of the various counters in the IP
*       Router Management Counter-Set.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       counterIndex  - index of counter set (range 0..3)
*
* OUTPUTS:
*       countersPtr   - the counters values in the counter set
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad parameter
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_IP_COUNTER_SET_STC counters;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (counterIndex == 4)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpCounterGet(devNum, counterIndex, &counters);

    inFields[0] = counterIndex;
    inFields[1] = counters.inUcPkts;
    inFields[2] = counters.inMcPkts;
    inFields[3] = counters.inUcNonRoutedExceptionPkts;
    inFields[4] = counters.inUcNonRoutedNonExceptionPkts;
    inFields[5] = counters.inMcNonRoutedExceptionPkts;
    inFields[6] = counters.inMcNonRoutedNonExceptionPkts;
    inFields[7] = counters.inUcTrappedMirrorPkts;
    inFields[8] = counters.inMcTrappedMirrorPkts;
    inFields[9] = counters.inMcRfpFailPkts;
    inFields[10] = counters.outUcPkts;

    counterIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],
                inFields[9],  inFields[10]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpCounterGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    counterIndex = 0;
    return wrCpssExMxPmIpCounterGet(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxPmIpDropCounterModeSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This function sets the mode of the Drop Counter (count packets dropped
*       cause of all reasons or just cause of specific reason).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       mode          - drop counter mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad parameter
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDropCounterModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_IP_DROP_COUNTER_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_IP_DROP_COUNTER_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpDropCounterModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpDropCounterModeGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This function gets the mode of the Drop Counter (count packets dropped
*       cause of all reasons or just cause of specific reason).
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       modePtr       - points to drop counter mode
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad parameter
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDropCounterModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_IP_DROP_COUNTER_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpDropCounterModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpDropCounterSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This function sets the Drop Counter value.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       value         - drop counter value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad parameter
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDropCounterSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 value;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    value = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmIpDropCounterSet(devNum, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpDropCounterGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       This function gets the Drop Counter value.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       valuePtr      - points to drop counter value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on bad parameter
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDropCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 value;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmIpDropCounterGet(devNum, &value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", value);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpDitCounterConfigSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Configures DIT (Downstream Interface Table) egress counter-set.
*       A counter can be bound to any interface defined by the 3-tuple:
*       {device+port or Trunk, VLAN, packet type: IPv4 or IPv6}.
*       One or more of the elements of the 3-tuple can be configured to be a
*       wildcard. For example: {*, VLAN 5, IPv6}.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*       counterIndex    - counter index. Range 0..1
*       counterCfgPtr   - points to DIT counter configuration;
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - on NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDitCounterConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 counterIndex;
    CPSS_EXMXPM_DIT_COUNTER_SET_CFG_STC counterCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    counterIndex = inFields[0];
    counterCfg.portTrunk.devPort.devNum = devNum;
    counterCfg.portTrunkMode = (CPSS_EXMXPM_IP_PORT_TRUNK_COUNTER_MODE_ENT)inFields[1];
    switch (counterCfg.portTrunkMode)
    {
        case CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E:
            counterCfg.portTrunk.devPort.port = (GT_U8)inFields[2];
            break;

        case CPSS_EXMXPM_DIT_TRUNK_COUNTER_MODE_E:
            counterCfg.portTrunk.trunk = (GT_TRUNK_ID)inFields[2];
            break;

        case CPSS_EXMXPM_DIT_DISREGARD_PORT_TRUNK_COUNTER_MODE_E:
            break;

        default:
            galtisOutput(outArgs, GT_BAD_VALUE, "");
            return CMD_OK;
    }

    counterCfg.packetType = inFields[3] - 1; /* first enum in db is removed from code */
    counterCfg.vlanMode = (CPSS_EXMXPM_DIT_VLAN_COUNTER_MODE_ENT)inFields[4];
    counterCfg.vlanId = (GT_U16)inFields[5];

    /* call cpss api function */
    result = cpssExMxPmDitCounterConfigSet(devNum, counterIndex, CPSS_MULTICAST_E, &counterCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIpDitCounterConfigGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Retreives DIT (Downstream Interface Table) egress counter-set.
*       A counter can be bound to any interface defined by the 3-tuple:
*       {device+port or Trunk, VLAN, packet type: IPv4 or IPv6}.
*       One or more of the elements of the 3-tuple can be configured to be a
*       wildcard. For example: {*, VLAN 5, IPv6}.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*       counterIndex    - counter index. Range 0..1
*
* OUTPUTS:
*       counterCfgPtr   - points to DIT counter configuration;
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - on NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDitCounterConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_DIT_COUNTER_SET_CFG_STC counterCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (counterIndex == 2)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmDitCounterConfigGet(devNum, counterIndex, CPSS_MULTICAST_E, &counterCfg);

    inFields[0] = counterIndex;
    inFields[1] = counterCfg.portTrunkMode;

    switch (counterCfg.portTrunkMode)
    {
        case CPSS_EXMXPM_DIT_PORT_COUNTER_MODE_E:
            inFields[2] = counterCfg.portTrunk.devPort.port;
            break;

        case CPSS_EXMXPM_DIT_TRUNK_COUNTER_MODE_E:
            inFields[2] = counterCfg.portTrunk.trunk;
            break;

        case CPSS_EXMXPM_DIT_DISREGARD_PORT_TRUNK_COUNTER_MODE_E:
            break;

        default:
            galtisOutput(outArgs, GT_BAD_VALUE, "");
            return CMD_OK;
    }

    inFields[3] = counterCfg.packetType + 1; /* first enum in db is removed from code */
    inFields[4] = counterCfg.vlanMode;

    switch (counterCfg.vlanMode)
    {
          case CPSS_EXMXPM_DIT_USE_VLAN_COUNTER_MODE_E:
              inFields[5] = counterCfg.vlanId;
              break;

          case CPSS_EXMXPM_DIT_DISREGARD_VLAN_COUNTER_MODE_E:
              break;

          default:
              galtisOutput(outArgs, GT_BAD_VALUE, "");
              return CMD_OK;
    }

    counterIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;

}

/********************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDitCounterConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    counterIndex = 0;
    return wrCpssExMxPmIpDitCounterConfigGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxPm2IpDitCounterConfigSet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Configures DIT (Downstream Interface Table) egress counter-set.
*       A counter can be bound to any interface defined by the 3-tuple:
*       {device+port or Trunk, VLAN, packet type: IPv4 or IPv6}.
*       One or more of the elements of the 3-tuple can be configured to be a
*       wildcard. For example: {*, VLAN 5, IPv6}.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*       counterIndex    - counter index. Range 0..1
*       counterCfgPtr   - points to DIT counter configuration;
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - on NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2IpDitCounterConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 counterIndex;
    CPSS_EXMXPM_DIT_COUNTER_SET_CFG_STC counterCfg;
    CPSS_UNICAST_MULTICAST_ENT          type;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    counterIndex = inFields[0];
    type = (CPSS_UNICAST_MULTICAST_ENT)inArgs[1];

    counterCfg.portTrunk.devPort.devNum = devNum;
    counterCfg.portTrunkMode = (CPSS_EXMXPM_IP_PORT_TRUNK_COUNTER_MODE_ENT)inFields[1];

    switch (counterCfg.portTrunkMode)
    {
        case CPSS_EXMXPM_IP_PORT_COUNTER_MODE_E:
            counterCfg.portTrunk.devPort.port = (GT_U8)inFields[2];
            break;

        case CPSS_EXMXPM_DIT_TRUNK_COUNTER_MODE_E:
            counterCfg.portTrunk.trunk = (GT_TRUNK_ID)inFields[3];
            break;

        case CPSS_EXMXPM_DIT_DISREGARD_PORT_TRUNK_COUNTER_MODE_E:
            break;

        default:
            galtisOutput(outArgs, GT_BAD_VALUE, "");
            return CMD_OK;
    }

    counterCfg.packetType = (CPSS_EXMXPM_DIT_PACKET_TYPE_COUNTER_MODE_ENT)inFields[3];
    counterCfg.vlanMode = (CPSS_EXMXPM_DIT_VLAN_COUNTER_MODE_ENT)inFields[4];
    counterCfg.vlanId = (GT_U16)inFields[5];

    /* call cpss api function */
    result = cpssExMxPmDitCounterConfigSet(devNum, counterIndex, type, &counterCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmIp2DitCounterConfigGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       Retreives DIT (Downstream Interface Table) egress counter-set.
*       A counter can be bound to any interface defined by the 3-tuple:
*       {device+port or Trunk, VLAN, packet type: IPv4 or IPv6}.
*       One or more of the elements of the 3-tuple can be configured to be a
*       wildcard. For example: {*, VLAN 5, IPv6}.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum          - device number
*       counterIndex    - counter index. Range 0..1
*
* OUTPUTS:
*       counterCfgPtr   - points to DIT counter configuration;
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - on NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2IpDitCounterConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_DIT_COUNTER_SET_CFG_STC counterCfg;
    CPSS_UNICAST_MULTICAST_ENT          type;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (counterIndex == 2)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    type = (CPSS_UNICAST_MULTICAST_ENT)inArgs[1];

    cmdOsMemSet(&counterCfg, 0, sizeof(counterCfg));

    /* call cpss api function */
    result = cpssExMxPmDitCounterConfigGet(devNum, counterIndex, type, &counterCfg);

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = counterIndex;
    inFields[1] = counterCfg.portTrunkMode;

    switch (counterCfg.portTrunkMode)
    {
        case CPSS_EXMXPM_DIT_PORT_COUNTER_MODE_E:
            inFields[2] = counterCfg.portTrunk.devPort.port;
            break;

        case CPSS_EXMXPM_DIT_TRUNK_COUNTER_MODE_E:
            inFields[2] = counterCfg.portTrunk.trunk;
            break;

    case CPSS_EXMXPM_DIT_DISREGARD_PORT_TRUNK_COUNTER_MODE_E:
            inFields[2] = 0;
            break;

        default:
            galtisOutput(outArgs, GT_BAD_VALUE, "");
            return CMD_OK;
    }

    inFields[3] = counterCfg.packetType;
    inFields[4] = counterCfg.vlanMode;

    switch (counterCfg.vlanMode)
    {
          case CPSS_EXMXPM_DIT_USE_VLAN_COUNTER_MODE_E:
              inFields[5] = counterCfg.vlanId;
              break;

          case CPSS_EXMXPM_DIT_DISREGARD_VLAN_COUNTER_MODE_E:
              inFields[5] = 0;
              break;

          default:
              galtisOutput(outArgs, GT_BAD_VALUE, "");
              return CMD_OK;
    }

    counterIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;

}

/********************************************************************************/
static CMD_STATUS wrCpssExMxPm2IpDitCounterConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    counterIndex = 0;
    return wrCpssExMxPm2IpDitCounterConfigGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxPmIpDitCounterGet
*
* DESCRIPTION:
*       Function Relevant mode : High/Low Level API modes
*       DIT (downsteam interface table) counter counts the number of multicast
*       packets that triggered the Router engine and have a downstream interface
*       associated with this counter.
*       This function gets DIT counter value.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       counterIndex  - Counter index. Range 0..1
*       valuePtr      - point to DIT counter value (range 32 bit)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_BAD_PARAM  - on bad parameter
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmIpDitCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 counterIndex;
    GT_U32 value;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    counterIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmDitCounterGet(devNum, counterIndex, CPSS_MULTICAST_E, &value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", value);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmIpCounterConfigSet",
         &wrCpssExMxPmIpCounterConfigSet,
         1, 8},
        {"cpssExMxPmIpCounterConfigGetFirst",
         &wrCpssExMxPmIpCounterConfigGetFirst,
         1, 0},
        {"cpssExMxPmIpCounterConfigGetNext",
         &wrCpssExMxPmIpCounterConfigGet,
         1, 0},
        {"cpssExMxPm2IpCounterConfigSet",
         &wrCpssExMxPm2IpCounterConfigSet,
         1, 8},
        {"cpssExMxPm2IpCounterConfigGetFirst",
         &wrCpssExMxPm2IpCounterConfigGetFirst,
         1, 0},
        {"cpssExMxPm2IpCounterConfigGetNext",
         &wrCpssExMxPm2IpCounterConfigGet,
         1, 0},
        {"cpssExMxPmIpCounterSet",
         &wrCpssExMxPmIpCounterSet,
         1, 11},
        {"cpssExMxPmIpCounterGetFirst",
         &wrCpssExMxPmIpCounterGetFirst,
         1, 0},
        {"cpssExMxPmIpCounterGetNext",
         &wrCpssExMxPmIpCounterGet,
         1, 0},
        {"cpssExMxPmIpDropCounterModeSet",
         &wrCpssExMxPmIpDropCounterModeSet,
         2, 0},
        {"cpssExMxPmIpDropCounterModeGet",
         &wrCpssExMxPmIpDropCounterModeGet,
         1, 0},
        {"cpssExMxPmIpDropCounterSet",
         &wrCpssExMxPmIpDropCounterSet,
         2, 0},
        {"cpssExMxPmIpDropCounterGet",
         &wrCpssExMxPmIpDropCounterGet,
         1, 0},
        {"cpssExMxPmIpDitCounterConfigSet",
         &wrCpssExMxPmIpDitCounterConfigSet,
         1, 6},
        {"cpssExMxPmIpDitCounterConfigGetFirst",
         &wrCpssExMxPmIpDitCounterConfigGetFirst,
         1, 0},
        {"cpssExMxPmIpDitCounterConfigGetNext",
         &wrCpssExMxPmIpDitCounterConfigGet,
         1, 0},
        {"cpssExMxPm2IpDitCounterConfigSet",
         &wrCpssExMxPm2IpDitCounterConfigSet,
         2, 6},
        {"cpssExMxPm2IpDitCounterConfigGetFirst",
         &wrCpssExMxPm2IpDitCounterConfigGetFirst,
         2, 0},
        {"cpssExMxPm2IpDitCounterConfigGetNext",
         &wrCpssExMxPm2IpDitCounterConfigGet,
         2, 0},
        {"cpssExMxPmIpDitCounterGet",
         &wrCpssExMxPmIpDitCounterGet,
         2, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmIpStat
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
GT_STATUS cmdLibInitCpssExMxPmIpStat
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

