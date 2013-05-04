/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmVb.c
*
* DESCRIPTION:
*       Wrappers for cpssExMxPmVb.c
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
#include <cpss/exMxPm/exMxPmGen/vb/cpssExMxPmVb.h>


/*******************************************************************************
* wrCpssExMxPmVbPortCascadingModeSet
*
* DESCRIPTION:
*       Set ingress/egress port mode (Network or Cascading) 
*       and DSA tag type (Non-DSA, DSA or extended DSA) accordingly.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number
*       physicalPortNum - physical port number (CPU port for egress configuration 
*                                               is supported)
*       direction       - ingress or egress
*       mode            - cascading port mode: Network(No-DSA), DSA or 
*                                              extended DSA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success
*       CMD_AGENT_ERROR             - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbPortCascadingModeSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    IN    GT_U8                       devNum;
    IN    GT_U8                       physicalPortNum;
    IN    CPSS_DIRECTION_ENT          direction;
    IN    CPSS_CSCD_PORT_TYPE_ENT     mode;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum = (GT_U8)inArgs[0];
    physicalPortNum = (GT_U8)inArgs[1];
    direction = (CPSS_DIRECTION_ENT)inArgs[2];
    mode = (CPSS_CSCD_PORT_TYPE_ENT)inArgs[3];

    /* call cpss API function */
    result = cpssExMxPmVbPortCascadingModeSet(devNum, physicalPortNum, direction, mode);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssExMxPmVbPortCascadingModeGet
*
* DESCRIPTION:
*       Get ingress/egress port mode and DSA tag type.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number
*       physicalPortNum - physical port number (CPU port for egress configuration 
*                                               is supported)
*       direction       - ingress or egress
*
* OUTPUTS:
*       modePtr     - (pointer to) cascading port mode: Network(No-DSA), 
*                      DSA or extended DSA.
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbPortCascadingModeGet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    IN    GT_U8                       devNum;
    IN    GT_U8                       physicalPortNum;
    IN    CPSS_DIRECTION_ENT          direction;

    OUT   CPSS_CSCD_PORT_TYPE_ENT     mode;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum = (GT_U8)inArgs[0];
    physicalPortNum = (GT_U8)inArgs[1];
    direction = (CPSS_DIRECTION_ENT)inArgs[2];

    /* call cpss API function */
    result = cpssExMxPmVbPortCascadingModeGet(devNum, physicalPortNum, direction, &mode);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}



/*******************************************************************************
* wrCpssExMxPmVbIngressPortDsaTagManipulationEnableSet
*
* DESCRIPTION:
*       Enable/Disable DSA Tag manipulation per port, when packet is written 
*       to packet buffer memory.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number
*       physicalPortNum - physical port number,  CPU port for is supported too
*       enable          - GT_TRUE: enable packet DSA Tag manipulation. DSA Tag 
*                                  can be removed or overwritten (depends on 
*                                  port configuration).
*                          GT_FALSE: disable packet DSA Tag manipulation. Packet 
*                                  is written to memory unmodified.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                   - on success
*       CMD_AGENT_ERROR          - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbIngressPortDsaTagManipulationEnableSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    IN    GT_U8      devNum;
    IN    GT_U8      physicalPortNum;
    IN    GT_BOOL    enable;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum = (GT_U8)inArgs[0];
    physicalPortNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss API function */
    result = cpssExMxPmVbIngressPortDsaTagManipulationEnableSet(devNum, physicalPortNum, enable);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* wrCpssExMxPmVbIngressPortDsaTagManipulationEnableGet
*
* DESCRIPTION:
*       Get DSA Tag manipulation per port (Enabled/Disabled).
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number
*       physicalPortNum - physical port number,  CPU port for is supported too
*
* OUTPUTS:
*       enablePtr   - (pointer to)         
*                          GT_TRUE: enable packet DSA Tag manipulation. DSA Tag 
*                                  can be removed or overwritten (depends on 
*                                  port configuration).
*                          GT_FALSE: disable packet DSA Tag manipulation. Packet 
*                                  is written to memory unmodified.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbIngressPortDsaTagManipulationEnableGet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS         result;

    IN    GT_U8       devNum;
    IN    GT_U8       physicalPortNum;

    OUT   GT_BOOL     enable;


    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum = (GT_U8)inArgs[0];
    physicalPortNum = (GT_U8)inArgs[1];

    /* call cpss API function */
    result = cpssExMxPmVbIngressPortDsaTagManipulationEnableGet(devNum,physicalPortNum, &enable);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}



/*******************************************************************************
* wrCpssExMxPmVbIngressPortEtherTypeSelectSet
*
* DESCRIPTION:
*       Set per port the etherType mode of the packet, when packet is written 
*       modified to the memory.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number
*       physicalPortNum - physical port number, CPU port for is supported too
*       mode            - etherType mode.
*                     CPSS_VLAN_ETHERTYPE0_E - select etherType0 for written 
*                                              modified packet to memory
*                     CPSS_VLAN_ETHERTYPE1_E - select etherType1 for written 
*                                              modified packet to memory
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbIngressPortEtherTypeSelectSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;

    IN    GT_U8                 devNum;
    IN    GT_U8                 physicalPortNum;
    IN    CPSS_ETHER_MODE_ENT   mode;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum = (GT_U8)inArgs[0];
    physicalPortNum = (GT_U8)inArgs[1];
    mode = (CPSS_ETHER_MODE_ENT)inArgs[2];

    /* call cpss API function */
    result = cpssExMxPmVbIngressPortEtherTypeSelectSet(devNum, physicalPortNum, mode);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbIngressPortEtherTypeSelectGet
*
* DESCRIPTION:
*       Get per port the etherType mode of the packet, when packet is written 
*       modified to the memory.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number
*       physicalPortNum - physical port number, CPU port for is supported too
*
* OUTPUTS:
*       modePtr     - (pointer to) etherType mode.
*                     CPSS_VLAN_ETHERTYPE0_E - select etherType0 for written 
*                                              modified packet to memory
*                     CPSS_VLAN_ETHERTYPE1_E - select etherType1 for written 
*                                              modified packet to memory
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbIngressPortEtherTypeSelectGet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;

    IN    GT_U8                 devNum;
    IN    GT_U8                 physicalPortNum;

    OUT   CPSS_ETHER_MODE_ENT   mode;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum = (GT_U8)inArgs[0];
    physicalPortNum = (GT_U8)inArgs[1];

    /* call cpss API function */
    result = cpssExMxPmVbIngressPortEtherTypeSelectGet(devNum, physicalPortNum, &mode);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}



/*******************************************************************************
* wrCpssExMxPmVbVirtualPortsMapModeSet
*
* DESCRIPTION:
*       Set the access mode of the "Virtual Port-ID assignment" table.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - physical device number
*       vbMode              - virtual port map mode: 
*                                               physical based or DSA tag based.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None  
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbVirtualPortsMapModeSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    IN GT_U8                                        devNum;
    IN CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_ENT    vbMode;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum = (GT_U8)inArgs[0];
    vbMode = (CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_ENT)inArgs[1];

    /* call cpss API function */
    result = cpssExMxPmVbVirtualPortsMapModeSet(devNum, vbMode);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbVirtualPortsMapModeGet
*
* DESCRIPTION:
*       Get the access mode of the "Virtual Port-ID assignment" table.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       vbModePtr           - (pointer to) virtual port map mode: 
*                                               physical based or DSA tag based.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None  
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbVirtualPortsMapModeGet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    IN GT_U8                                        devNum;

    OUT CPSS_EXMXPM_VB_VIRTUAL_PORTS_MAP_MODE_ENT   vbMode;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss API function */
    result = cpssExMxPmVbVirtualPortsMapModeGet(devNum, &vbMode);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d", vbMode);

    return CMD_OK;
}



/*******************************************************************************
* wrCpssExMxPmVbPortValueModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable value mode on port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - physical device number
*       physicalPortNum     - physical port number
*       enable              - GT_TRUE: The index to the Virtual port ID mapping table
*                                      is controlled by virtual port map mode,
*                                      cpssExMxPmVbVirtualPortsMapModeSet API.
*                             GT_FALSE: The index to the Virtual port ID mapping table
*                                      is physical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGNET_ERROR           - on error
*
* COMMENTS:
*       None  
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbPortValueModeEnableSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS     result;

    IN GT_U8      devNum;
    IN GT_U8      physicalPortNum;
    IN GT_BOOL    enable;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum = (GT_U8)inArgs[0];
    physicalPortNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss API function */
    result = cpssExMxPmVbPortValueModeEnableSet(devNum, physicalPortNum, enable);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbPortValueModeEnableGet
*
* DESCRIPTION:
*       Get value mode on port (Enabled/Disabled).
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - physical device number
*       physicalPortNum     - physical port number
*
* OUTPUTS:
*       enablePtr           - GT_TRUE: The index to the Virtual port ID mapping table
*                                      is controlled by virtual port map mode,
*                                      cpssExMxPmVbVirtualPortsMapModeSet API.
*                             GT_FALSE: The index to the Virtual port ID mapping table
*                                      is physical port number.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None  
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbPortValueModeEnableGet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    IN  GT_U8      devNum;
    IN  GT_U8      physicalPortNum;

    OUT GT_BOOL    enable;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum = (GT_U8)inArgs[0];
    physicalPortNum = (GT_U8)inArgs[1];

    /* call cpss API function */
    result = cpssExMxPmVbPortValueModeEnableGet(devNum, physicalPortNum, &enable);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet
*
* DESCRIPTION:
*       Map Ingress Physical port number and DSA tag info to Virtual port ID.
*       The function configures both ingress VB BM (Buffer Management) table 
*       and VP assignment.
*       This API must be used after configuration of cascading ingress port 
*       and global VB mode. 
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - physical device number
*       physicalPortNum     - physical port number
*       dsaTagSrcDev        - source device number of the mux (from DSA tag)
*       dsaTagSrcPort       - network port of the mux device (from DSA tag)
*       portNum             - virtual port number (0..54)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       Entries 0-1023 are accessed according to DSA tag information. When port
*       is a value port: If virtual port map mode is physical based then 
*       Virtual-Port-ID table is accessed according to {group-ID, physical port 
*       and DSA<src port>}, if virtual port map is DSA based then 
*       Virtual-Port-ID table is accessed according to {group-ID, 
*       DSA<srcDevice> and DSA<srcPort>}.
*       Entries 1024-1051 are mapped according to physical ports.
*       Where 1024-1047 are Gig ports, 1048 and 1049 are XG ports, 
*       1050 - service port and 1051 - loopback port.  
*       
*       If one of the following parameters is changed: Value mode of the port 
*       by cpssExMxPmVbPortValueModeEnableSet or Virtual Ports Map mode by 
*       cpssExMxPmVbVirtualPortsMapModeSet, then this API should be called again, 
*       in order to set Virtual Port ID in the correct index.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;

    IN    GT_U8         devNum;
    IN    GT_U8         physicalPortNum;
    IN    GT_U8         dsaTagSrcDev;
    IN    GT_U8         dsaTagSrcPort;
    IN    GT_U8         portNum;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    physicalPortNum     = (GT_U8)inArgs[1];
    dsaTagSrcDev        = (GT_U8)inArgs[2];
    dsaTagSrcPort       = (GT_U8)inArgs[3];
    portNum             = (GT_U8)inArgs[4];

    /* call cpss API function */
    result = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet(devNum, physicalPortNum, dsaTagSrcDev,
                                                               dsaTagSrcPort, portNum);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet 
*
* DESCRIPTION:
*       Get the mapping of Ingress Physical port number and DSA tag info to 
*       Virtual port ID.
*       This API must be used after configuration of cascading ingress port 
*       and global VB mode. 
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number
*       physicalPortNum - physical port number
*       dsaTagSrcDev    - source device number of the mux (from DSA tag)
*       dsaTagSrcPort   - network port of the mux device (from DSA tag)
*
* OUTPUTS:
*       portNumPtr              - (pointer to) virtual port number
*       bmPhysicalPortNumPtr    - (pointer to) physical port number for 
*                                  ingress BM 
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       Entries 0-1023 are accessed according to DSA tag information. When port
*       is a value port: If virtual port map mode is physical based then 
*       Virtual-Port-ID table is accessed according to {group-ID, physical port 
*       and DSA<src port>}, if virtual port map is DSA based then 
*       Virtual-Port-ID table is accessed according to {group-ID, 
*       DSA<srcDevice> and DSA<srcPort>}
*       Entries 1024-1051 are mapped according to physical ports.
*       Where 1024-1047 are Gig ports, 1048 and 1049 are XG ports, 
*       1050 - service port and 1051 - loopback port.
* 
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;

    IN    GT_U8         devNum;
    IN    GT_U8         physicalPortNum;
    IN    GT_U8         dsaTagSrcDev;
    IN    GT_U8         dsaTagSrcPort;

    OUT   GT_U8         portNum;
    OUT   GT_U8         bmPhysicalPortNum;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    physicalPortNum     = (GT_U8)inArgs[1];
    dsaTagSrcDev        = (GT_U8)inArgs[2];
    dsaTagSrcPort       = (GT_U8)inArgs[3];

    /* call cpss API function */
    result = cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet(devNum, physicalPortNum, dsaTagSrcDev,
                                                               dsaTagSrcPort, &portNum, &bmPhysicalPortNum);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d%d", portNum, bmPhysicalPortNum);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet
*
* DESCRIPTION:
*       Map Ingress physical non-cascading port number to Virtual port ID.
*       The function configures both ingress VB BM (Buffer Management) table 
*       and VP assignment.
*       This API must be used after configuration of global VB mode 
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - physical device number
*       physicalPortNum     - physical port number
*       portNum             - virtual port number (0..54)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       Entries 1024-1051 are mapped according to physical ports.
*       Where 1024-1047 are Gig ports, 1048 and 1049 are XG ports, 
*       1050 - service port and 1051 - loopback port.  
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;

    IN    GT_U8         devNum;
    IN    GT_U8         physicalPortNum;
    IN    GT_U8         portNum;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    physicalPortNum     = (GT_U8)inArgs[1];
    portNum             = (GT_U8)inArgs[2];

    /* call cpss API function */
    result = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(devNum, physicalPortNum, portNum);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet
*
* DESCRIPTION:
*       Get the mapping of ingress physical non-cascading port number to Virtual 
*       port ID.
*       The function reads both ingress VB BM (Buffer Management) mapping  
*       and VP assignment.
*       This API must be used after configuration of global VB mode.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - physical device number
*       physicalPortNum     - physical port number
*
* OUTPUTS:
*       portNumPtr              - (pointer to) virtual port number
*       bmPhysicalPortNumPtr    - (pointer to) physical port number for 
*                                  ingress BM 
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGNET_ERROR           - on error
*
* COMMENTS:
*       Entries 1024-1051 are mapped according to physical ports.
*       Where 1024-1047 are Gig ports, 1048 and 1049 are XG ports, 
*       1050 - service port and 1051 - loopback port.  
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;

    IN    GT_U8         devNum;
    IN    GT_U8         physicalPortNum;

    OUT   GT_U8         portNum;
    OUT   GT_U8         bmPhysicalPortNum;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    physicalPortNum     = (GT_U8)inArgs[1];

    /* call cpss API function */
    result = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet(physicalPortNum,physicalPortNum,
                                                                  &portNum, &bmPhysicalPortNum);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d%d", portNum, bmPhysicalPortNum);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbEgressVirtualPortToDsaTagMappingSet
*
* DESCRIPTION:
*       Mapping Egress Virtual port number to DSA tag
*       {target device and target port}. 
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - virtual port number (0..54)
*       dsaTagTrgDev    - target device number mapped virtual port
*       dsaTagTrgPort   - target port number mapped virtual port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbEgressVirtualPortToDsaTagMappingSet 
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;

    IN  GT_U8           devNum;
    IN  GT_U8           portNum;
    IN  GT_U8           dsaTagTrgDev;
    IN  GT_U8           dsaTagTrgPort;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    portNum             = (GT_U8)inArgs[1];
    dsaTagTrgDev        = (GT_U8)inArgs[2];
    dsaTagTrgPort       = (GT_U8)inArgs[3];

    /* call cpss API function */
    result = cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet(devNum, portNum, dsaTagTrgDev, dsaTagTrgPort);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* wrCpssExMxPmVbEgressVirtualPortToDsaTagMappingGet
*
* DESCRIPTION:
*       Get Mapping of Egress Virtual port number to DSA tag
*       {target device and target port}.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - virtual port number (0..54)
*
* OUTPUTS:
*       dsaTagTrgDevPtr     - (pointer to) target device number mapped virtual port
*       dsaTagTrgPortPtr    - (pointer to) target port number mapped virtual port
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbEgressVirtualPortToDsaTagMappingGet 
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;

    IN  GT_U8           devNum;
    IN  GT_U8           portNum;

    OUT GT_U8           dsaTagTrgDev;
    OUT GT_U8           dsaTagTrgPort;  

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    portNum             = (GT_U8)inArgs[1];

    /* call cpss API function */
    result = cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet(devNum, portNum,
                                                             &dsaTagTrgDev, &dsaTagTrgPort);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d%d", dsaTagTrgDev, dsaTagTrgPort);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbEgressVirtualPortToPhysicalMappingSet
*
* DESCRIPTION:
*       Mapping Egress Virtual port number to Physical port number 
*       {physical port and physical tc}.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - virtual port number (0..54)
*       tc              - traffic class
*       physicalPortNum - physical port number
*       physicalTc      - egress traffic class
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbEgressVirtualPortToPhysicalMappingSet 
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;

    IN  GT_U8           devNum;
    IN  GT_U8           portNum;
    IN  GT_U32          tc;
    IN  GT_U8           physicalPortNum;
    IN  GT_U32          physicalTc;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    portNum             = (GT_U8)inArgs[1];
    tc                  = (GT_U32)inArgs[2];
    physicalPortNum     = (GT_U8)inArgs[3];
    physicalTc          = (GT_U32)inArgs[4];

    /* call cpss API function */
    result = cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet(devNum, portNum, tc, physicalPortNum, physicalTc);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbEgressVirtualPortToPhysicalMappingGet
*
* DESCRIPTION:
*       Get mapping of Egress Virtual port number to Physical port number 
*       {physical port and physical tc}.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - virtual port number (0..54)
*       tc              - traffic class
*
* OUTPUTS:
*       physicalPortNumPtr  - (pointer to) physical port number
*       physicalTcPtr       - (pointer to) egress traffic class
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, tc or device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - on bad physical port or physical tc
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbEgressVirtualPortToPhysicalMappingGet 
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;

    IN  GT_U8           devNum;
    IN  GT_U8           portNum;
    IN  GT_U32          tc;

    OUT  GT_U8           physicalPortNum;
    OUT  GT_U32          physicalTc;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    portNum             = (GT_U8)inArgs[1];
    tc                  = (GT_U32)inArgs[2];

    /* call cpss API function */
    result = cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet(devNum, portNum, tc,
                                                               &physicalPortNum, &physicalTc);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d%d", physicalPortNum, physicalTc);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbEgressVirtualPortAggregationModeSet
*
* DESCRIPTION:
*       Set egress ports queue aggregation mode.
*
* APPLICABLE DEVICES:  Only ExMxPm2 XG devices
*
* INPUTS:
*       devNum   - physical device number
*       mode     - aggregation mode: Non-Aggregated or Aggregated
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbEgressVirtualPortAggregationModeSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;

    IN    GT_U8                                         devNum;
    IN    CPSS_EXMXPM_VB_QUEUE_AGGREGATION_MODE_ENT     mode;



    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    mode                = (CPSS_EXMXPM_VB_QUEUE_AGGREGATION_MODE_ENT)inArgs[1];

    /* call cpss API function */
    result = cpssExMxPmVbEgressVirtualPortAggregationModeSet(devNum, mode);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbEgressVirtualPortAggregateModeGet
*
* DESCRIPTION:
*       Get egress ports queue aggregation mode.
*
* APPLICABLE DEVICES:  Only ExMxPm2 XG devices
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       modePtr - (pointer to) aggregation mode: Non-Aggregated or Aggregated
*
* RETURNS:
*       CMD_OK                  - on success
*       CMD_AGENT_ERROR         - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbEgressVirtualPortAggregateModeGet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                           result;

    IN    GT_U8                                         devNum;

    OUT   CPSS_EXMXPM_VB_QUEUE_AGGREGATION_MODE_ENT     mode;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];

    /* call cpss API function */
    result = cpssExMxPmVbEgressVirtualPortAggregateModeGet(devNum, &mode);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbServicePortVirtualPortNumSet
*
* DESCRIPTION:
*       Set Virtual port number for service port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - service port virtual port number (0..54)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbServicePortVirtualPortNumSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    IN    GT_U8     devNum;
    IN    GT_U8     portNum;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    portNum             = (GT_U8)inArgs[1];

    /* call cpss API function */
    result = cpssExMxPmVbServicePortVirtualPortNumSet(devNum, portNum);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbServicePortVirtualPortNumGet
*
* DESCRIPTION:
*       Get Virtual port number of service port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       portNumPtr     - (pointer to) virtual port number
*
* RETURNS:
*       CMD_OK                   - on success
*       CMD_AGNET_ERROR          - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbServicePortVirtualPortNumGet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    IN    GT_U8     devNum;
    OUT   GT_U8     portNum;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    portNum          = (GT_U8)inArgs[1];

    /* call cpss API function */
    result = cpssExMxPmVbServicePortVirtualPortNumGet(devNum, &portNum);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d", portNum);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbLoopbackPortVirtualPortNumSet
*
* DESCRIPTION:
*       Set Virtual port number for loopback port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - virtual port number (0..54)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbLoopbackPortVirtualPortNumSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    IN    GT_U8     devNum;
    IN    GT_U8     portNum;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    portNum             = (GT_U8)inArgs[1];

    /* call cpss API function */
    result = cpssExMxPmVbLoopbackPortVirtualPortNumSet(devNum, portNum);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmVbLoopbackPortVirtualPortNumGet
*
* DESCRIPTION:
*       Get Virtual port number of loopback port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       portNumPtr     - virtual port number
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbLoopbackPortVirtualPortNumGet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    IN    GT_U8     devNum;
    OUT   GT_U8     portNum;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss API function */
    result = cpssExMxPmVbLoopbackPortVirtualPortNumGet(devNum, &portNum);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d", portNum);

    return CMD_OK;
}



/*******************************************************************************
* wrCpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet
*
* DESCRIPTION:
*       Map E2E Egress message from physical port to virtual. Associate virtual
*       ports with physical.
*       Egress Queue status advertisement indicates the status of egress queues
*       to ingress device via management cells. The status message is in Virtual
*       Port notation, but the egress queue is managed per physical port.
*       The status of the physical port is translated to virtual port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - physical device number
*       physicalPortNum     - physical port number
*       portsBitmapPtr      - (pointer to) virtual ports bitmap, virtual ports associated 
*                             with this physical port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None  
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;

    IN    GT_U8                 devNum;
    IN    GT_U8                 physicalPortNum;
    IN    CPSS_PORTS_BMP_STC    portsBitmap;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum               = (GT_U8)inArgs[0];
    physicalPortNum      = (GT_U8)inArgs[1];
    portsBitmap.ports[0] = (GT_U32)inArgs[2];
    portsBitmap.ports[1] = (GT_U32)inArgs[3];

    /* call cpss API function */
    result = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet(devNum, physicalPortNum, &portsBitmap);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* wrCpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet
*
* DESCRIPTION:
*       Get the mapping of E2E Egress message from physical port to virtual.
*       Egress Queue status advertisement indicates the status of egress queues
*       to ingress device via management cells. The status message is in Virtual
*       Port notation, but the egress queue is managed per physical port.
*       The status of the physical port is translated to virtual port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                  - physical device number
*       physicalPortNum         - physical port number
*       portsBitmapPtr          - (pointer to) virtual ports bitmap, ports 
*                                 that are associated  with this physical port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on error
*
* COMMENTS:
*       None  
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;

    IN    GT_U8                 devNum;
    IN    GT_U8                 physicalPortNum;
    OUT   CPSS_PORTS_BMP_STC    portsBitmap;

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments into locals */
    devNum              = (GT_U8)inArgs[0];
    physicalPortNum     = (GT_U8)inArgs[1];

    /* call cpss API function */
    result = cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet(devNum, physicalPortNum, &portsBitmap);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d%d", portsBitmap.ports[0], portsBitmap.ports[1]);

    return CMD_OK;
}


/**************** cpssExMxPmVbConfig Table **************************************/

/*******************************************************************************
* wrCpssExMxPmVbConfigSet
*
* DESCRIPTION:
*       Set the VB configuration for all virtual ports to the network ports in
*       the mux devices.
*       All physical ExMxPm ports that are not included in the INPUT info
*       (in cscdPortsArray) will be set to HW as non cascade ports.
*
*       This function configures the following tables: 
*       1. Configure port mode (Cascading) and DSA tag (regular or extended)
*          type per port by cpssExMxPmVbPortCascadingModeSet. 
*       2. Ingress Virtual port ID assignment and VB BM (Buffer Management) 
*          table by calling cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet.
*       3. Egress Virtual port to DSA tag mapping table by calling
*          cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet.
*       4. Egress Virtual port to Physical port mapping by calling
*          cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet.
*       5. Configure E2E Physical to Virtual Port association by calling
*          cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet.
*       The function also configures port mapings for: LBP, service and CPU 
*       ports.
*
*       APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number.
*       cscdPortsArraySize  - size of cscd ports array
*       cscdPortsArray      - (array of) cascade ports
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success.
*       CMD_AGENT_ERROR           - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#define VB_MAX_ARRAY_SIZE 29

static GT_U8 numOfEntries;
static CPSS_EXMXPM_VB_CSCD_PORT_INFO_STC cscdPortsArray[VB_MAX_ARRAY_SIZE];
static CPSS_EXMXPM_VB_VIRT_PORT_INFO_STC virtPortsInfoArray[VB_MAX_ARRAY_SIZE];


/********** prvCpssExMxPmVbConfigSet: a private set function **********/
static CMD_STATUS prvWrCpssExMxPmVbConfigSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8 i; /* indexing */

    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cscdPortsArray[numOfEntries].cscdIngressPhysicalPortNum = (GT_U8)inFields[0];
    cscdPortsArray[numOfEntries].cscdIngressPortDsaType = (CPSS_EXMXPM_NET_DSA_TYPE_ENT)inFields[1];
    cscdPortsArray[numOfEntries].virtPortsInfoSize = (GT_U8)inFields[2];
    cscdPortsArray[numOfEntries].virtPortsInfoPtr =
        (CPSS_EXMXPM_VB_VIRT_PORT_INFO_STC*) &virtPortsInfoArray[numOfEntries];

    /* map virtPortsInfoPtr's fields */
    virtPortsInfoArray[numOfEntries].portNum = (GT_U8) inFields[3];
    virtPortsInfoArray[numOfEntries].dsaTagSrcDev = (GT_U8) inFields[4];
    virtPortsInfoArray[numOfEntries].dsaTagSrcPort = (GT_U8) inFields[5];

    for (i=6; i < CPSS_TC_RANGE_CNS+6; i++) {
        virtPortsInfoArray[numOfEntries].egressPhysicalPortsNum[i - 6]
            = (GT_U8) inFields[i];
        virtPortsInfoArray[numOfEntries].egressPhysicalTcMap[i - 6]
            = (GT_U32) inFields[i + CPSS_TC_RANGE_CNS];
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}
/*******************************************************************************
* wrCpssExMxPmVbConfigSet
*
* DESCRIPTION:
*       Set the VB configuration for all virtual ports to the network ports in
*       the mux devices.
*       All physical ExMxPm ports that are not included in the INPUT info
*       (in cscdPortsArray) will be set to HW as non cascade ports.
*
*       This function configures the following tables: 
*       1. Configure port mode (Cascading) and DSA tag (regular or extended)
*          type per port by cpssExMxPmVbPortCascadingModeSet. 
*       2. Ingress Virtual port ID assignment and VB BM (Buffer Management) 
*          table by calling cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet.
*       3. Egress Virtual port to DSA tag mapping table by calling
*          cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet.
*       4. Egress Virtual port to Physical port mapping by calling
*          cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet.
*       5. Configure E2E Physical to Virtual Port association by calling
*          cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet.
*       The function also configures port mapings for: LBP, service and CPU 
*       ports.
*
*       APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number.
*       cscdPortsArraySize  - size of cscd ports array
*       cscdPortsArray      - (array of) cascade ports
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success.
*       CMD_AGENT_ERROR           - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

static CMD_STATUS wrCpssExMxPmVbConfigSetFirst
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    numOfEntries = 0;

    return prvWrCpssExMxPmVbConfigSet(inArgs, inFields, numFields, outArgs);
}

/*******************************************************************************
* wrCpssExMxPmVbConfigSet
*
* DESCRIPTION:
*       Set the VB configuration for all virtual ports to the network ports in
*       the mux devices.
*       All physical ExMxPm ports that are not included in the INPUT info
*       (in cscdPortsArray) will be set to HW as non cascade ports.
*
*       This function configures the following tables: 
*       1. Configure port mode (Cascading) and DSA tag (regular or extended)
*          type per port by cpssExMxPmVbPortCascadingModeSet. 
*       2. Ingress Virtual port ID assignment and VB BM (Buffer Management) 
*          table by calling cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet.
*       3. Egress Virtual port to DSA tag mapping table by calling
*          cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet.
*       4. Egress Virtual port to Physical port mapping by calling
*          cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet.
*       5. Configure E2E Physical to Virtual Port association by calling
*          cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet.
*       The function also configures port mapings for: LBP, service and CPU 
*       ports.
*
*       APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number.
*       cscdPortsArraySize  - size of cscd ports array
*       cscdPortsArray      - (array of) cascade ports
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success.
*       CMD_AGENT_ERROR           - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmVbConfigSetNext
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{    
   /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;
    numOfEntries++;

    return prvWrCpssExMxPmVbConfigSet(inArgs, inFields, numFields, outArgs);
}
/*******************************************************************************
* wrCpssExMxPmVbConfigSet
*
* DESCRIPTION:
*       Set the VB configuration for all virtual ports to the network ports in
*       the mux devices.
*       All physical ExMxPm ports that are not included in the INPUT info
*       (in cscdPortsArray) will be set to HW as non cascade ports.
*
*       This function configures the following tables: 
*       1. Configure port mode (Cascading) and DSA tag (regular or extended)
*          type per port by cpssExMxPmVbPortCascadingModeSet. 
*       2. Ingress Virtual port ID assignment and VB BM (Buffer Management) 
*          table by calling cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet.
*       3. Egress Virtual port to DSA tag mapping table by calling
*          cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet.
*       4. Egress Virtual port to Physical port mapping by calling
*          cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet.
*       5. Configure E2E Physical to Virtual Port association by calling
*          cpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet.
*       The function also configures port mapings for: LBP, service and CPU 
*       ports.
*
*       APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number.
*       cscdPortsArraySize  - size of cscd ports array
*       cscdPortsArray      - (array of) cascade ports
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success.
*       CMD_AGENT_ERROR           - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

static CMD_STATUS wrCpssExMxPmVbConfigEndSet
(
    IN  GT_32   inArgs[CMD_MAX_ARGS],
    IN  GT_32   inFields[CMD_MAX_FIELDS],
    IN  GT_32   numFields,
    OUT GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;

    IN GT_U8    devNum;
    
    /* check validity of arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum              = (GT_U8)inArgs[0];
        
    /* call cpss API function */
    status = cpssExMxPmVbConfigSet(devNum, numOfEntries, cscdPortsArray);

    

    if (status != GT_OK) {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}
/**************** end of 'cpssExMxPmVbConfig Table' ****************/


/******************* database initialization ***********************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxPmVbPortCascadingModeSet",
        &wrCpssExMxPmVbPortCascadingModeSet,
        4, 0},
    {"cpssExMxPmVbPortCascadingModeGet",
        &wrCpssExMxPmVbPortCascadingModeGet,
        3, 0},
    {"cpssExMxPmVbIngressPortDsaTagManipulationEnableSet",
        &wrCpssExMxPmVbIngressPortDsaTagManipulationEnableSet,
        3, 0},
    {"cpssExMxPmVbIngressPortDsaTagManipulationEnableGet",
        &wrCpssExMxPmVbIngressPortDsaTagManipulationEnableGet,
        2, 0},
    {"cpssExMxPmVbIngressPortEtherTypeSelectSet",
        &wrCpssExMxPmVbIngressPortEtherTypeSelectSet,
        3, 0},
    {"cpssExMxPmVbIngressPortEtherTypeSelectGet",
        &wrCpssExMxPmVbIngressPortEtherTypeSelectGet,
        2, 0},
    {"cpssExMxPmVbVirtualPortsMapModeSet",
        &wrCpssExMxPmVbVirtualPortsMapModeSet,
        2, 0},
    {"cpssExMxPmVbVirtualPortsMapModeGet",
        &wrCpssExMxPmVbVirtualPortsMapModeGet,
        1, 0},
    {"cpssExMxPmVbPortValueModeEnableSet",
        &wrCpssExMxPmVbPortValueModeEnableSet,
        3, 0},
    {"cpssExMxPmVbPortValueModeEnableGet",
        &wrCpssExMxPmVbPortValueModeEnableGet,
        2, 0},
    {"cpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet",
        &wrCpssExMxPmVbIngressCscdVirtualPortIdAssignmentSet,
        5, 0},
    {"cpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet",
        &wrCpssExMxPmVbIngressCscdVirtualPortIdAssignmentGet,
        4, 0},
    {"cpssExMxPmVbIngressNonCscdVirtualPortIdAssignSet",
        &wrCpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet,
        3, 0},
    {"cpssExMxPmVbIngressNonCscdVirtualPortIdAssignGet",
        &wrCpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentGet,
        2, 0},
    {"cpssExMxPmVbEgressVirtualPortToDsaTagMappingSet",
        &wrCpssExMxPmVbEgressVirtualPortToDsaTagMappingSet,
        4, 0},
    {"cpssExMxPmVbEgressVirtualPortToDsaTagMappingGet",
        &wrCpssExMxPmVbEgressVirtualPortToDsaTagMappingGet,
        2, 0},
    {"cpssExMxPmVbEgressVirtualPortToPhysicalMappingSet",
        &wrCpssExMxPmVbEgressVirtualPortToPhysicalMappingSet,
        5, 0},
    {"cpssExMxPmVbEgressVirtualPortToPhysicalMappingGet",
        &wrCpssExMxPmVbEgressVirtualPortToPhysicalMappingGet,
        3, 0},
    {"cpssExMxPmVbEgressVirtualPortAggregationModeSet",
        &wrCpssExMxPmVbEgressVirtualPortAggregationModeSet,
        2, 0},
    {"cpssExMxPmVbEgressVirtualPortAggregationModeGet",
        &wrCpssExMxPmVbEgressVirtualPortAggregateModeGet,
        1, 0},
    {"cpssExMxPmVbServicePortVirtualPortNumSet",
        &wrCpssExMxPmVbServicePortVirtualPortNumSet,
        2, 0},
    {"cpssExMxPmVbServicePortVirtualPortNumGet",
        &wrCpssExMxPmVbServicePortVirtualPortNumGet,
        1, 0},
    {"cpssExMxPmVbLoopbackPortVirtualPortNumSet",
        &wrCpssExMxPmVbLoopbackPortVirtualPortNumSet,
        2, 0},
    {"cpssExMxPmVbLoopbackPortVirtualPortNumGet",
        &wrCpssExMxPmVbLoopbackPortVirtualPortNumGet,
        1, 0},
    {"cpssExMxPmVbEgressE2eMsgPhysiToVirtPortMappingSet",
        &wrCpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingSet,
        4, 0},
    {"cpssExMxPmVbEgressE2eMsgPhysiToVirtPortMappingGet",
        &wrCpssExMxPmVbEgressE2eMsgPhysicalToVirtualPortMappingGet,
        2, 0},
    {"cpssExMxPmVbConfigSetFirst",
        &wrCpssExMxPmVbConfigSetFirst,
        1, 22},
    {"cpssExMxPmVbConfigSetNext",
        &wrCpssExMxPmVbConfigSetNext,
        1, 22},
    {"cpssExMxPmVbConfigEndSet",
        &wrCpssExMxPmVbConfigEndSet,
        1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmVb
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
GT_STATUS cmdLibInitCpssExMxPmVb
(
GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

