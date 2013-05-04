/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmMirror.c
*
* DESCRIPTION:
*       wrappers for cpssExMxPmMirror.c
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
#include <cpss/exMxPm/exMxPmGen/monitor/cpssExMxPmMirror.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

/*******************************************************************************
* cpssExMxPmMirrorStatEnableSet
*
* DESCRIPTION:
*       Enable/Disable Statistical Mirroring to Analyzer for given
*       mirroring type (Ingress/Egress/Ingress Policy).
*
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - device number.
*       mirrorType  - mirror type: Ingress/Egress or Ingress Policy
*       enable      - GT_TRUE = enable Statistical Mirroring for given mirror type.
*                     GT_FALSE = disable Statistical Mirroring for given mirror type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong device number or mirrorType.
*       GT_HW_ERROR     - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMirrorStatEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrorType = (CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmMirrorStatEnableSet(devNum, mirrorType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorStatEnableGet
*
* DESCRIPTION:
*       Get the status of Statistical Mirroring to Analyzer
*       (Enabled/Disabled) for given mirroring type (Ingress/Egress/Ingress Policy).
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - device number.
*       mirrorType  - mirror type: Ingress/Egress or Ingress Policy
*
* OUTPUTS:
*       enablePtr   - GT_TRUE = enable Statistical Mirroring for given mirror type.
*                     GT_FALSE = disable Statistical Mirroring for given mirror type.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong device number or mirrorType.
*       GT_HW_ERROR     - on writing to HW error.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMirrorStatEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrorType = (CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMirrorStatEnableGet(devNum, mirrorType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorStatRatioSet
*
* DESCRIPTION:
*       Set the statistical mirroring rate for Analyzer for given mirroring
*       type.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - device number.
*       mirrorType  - mirror type: Ingress/Egress or Ingress Policy
*       ratio       - Indicates the ratio of egress mirrored to analyzer port packets
*                     forwarded to the analyzer port. 1 of every 'ratio' packets are
*                     forwarded to the analyzer port. The range is 0 - 2047.
*
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong device or mirror type.
*       GT_HW_ERROR     - on writing to HW error.
*       GT_OUT_OF_RANGE - for bad ratio
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMirrorStatRatioSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    GT_U32 ratio;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrorType = (CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[1];
    ratio = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmMirrorStatRatioSet(devNum, mirrorType, ratio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorStatRatioGet
*
* DESCRIPTION:
*       Get the statistical mirroring rate for Analyzer for given mirroring
*       type.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - device number.
*       mirrorType  - mirror type: Ingress/Egress or Ingress Policy
*
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
* OUTPUTS:
*       ratioPtr    - Indicates the ratio of egress mirrored to analyzer port packets
*                     forwarded to the analyzer port. 1 of every 'ratio' packets are
*                     forwarded to the analyzer port. The range is 0 - 2047.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong device or mirror type.
*       GT_HW_ERROR     - on writing to HW error.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMirrorStatRatioGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    GT_U32 ratio;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrorType = (CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMirrorStatRatioGet(devNum, mirrorType, &ratio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ratio);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorPortEnableSet
*
* DESCRIPTION:
*       Enable/Disable per port mirroring to Ingress/Egress Analyzer.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - the device number
*       port        - port number
*       mirrorType  - mirror type: Ingress/Egress
*       enable      - GT_TRUE = enable ingress/egress mirroring to analyzer.
*                     GT_FALSE = disable ingress/egress mirroring to analyzer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number, port number or mirror type.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMirrorPortEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 mirrPort;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrPort = (GT_U8)inArgs[1];
    mirrorType = (CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmMirrorPortEnableSet(devNum, mirrPort, mirrorType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorPortEnableGet
*
* DESCRIPTION:
*       Get per port the status of mirroring to Ingress/Egress Analyzer.
*       (Enable/Disable).
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - the device number
*       port        - port number
*       mirrorType  - mirror type: Ingress/Egress
*
* OUTPUTS:
*       enablePtr  - GT_TRUE = enable ingress/egress mirroring to analyzer.
*                    GT_FALSE = disable ingress/egress mirroring to analyzer.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number, port number or mirror type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMirrorPortEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 mirrPort;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrPort = (GT_U8)inArgs[1];
    mirrorType = (CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmMirrorPortEnableGet(devNum, mirrPort, mirrorType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorDestInterfaceSet
*
* DESCRIPTION:
*       Set for given mirror type (Ingerss/Egress/Policer) Monitor Configuration.
*       Monitor Parameters are set according to Monitor Configuration type: port
*       trunk, vlan and tunnel.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum              - the device number
*       mirrorType          - mirror type: Ingress/Egress or Ingress Policy
*       interfaceInfoPtr    - (pointer to) destination interface. The accepted
*                             interfaces are: portNum and devNum when interface
*                             is port, trunkId when interface is Trunk, vlanId
*                             when monitor is vlan and tunnel type, pointer,
*                             deviceNum and portNum when interface is tunnel.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or mirror type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - interface parameter out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
/*puma 1 backward compatibility wrapper*/
static CMD_STATUS wrCpssExMxPmMirrorDestInterfaceSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    CPSS_EXMXPM_OUTLIF_INFO_STC interfaceInfo;
    GT_U32 analyzerNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

   devNum = (GT_U8)inArgs[0];
   mirrorType=(CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[1];
   interfaceInfo.outlifType=inArgs[2];
   analyzerNum =
       (mirrorType == CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E)
        ? 1 : 0;

   switch(interfaceInfo.outlifType){

   case 0:
        interfaceInfo.interfaceInfo.type=(CPSS_INTERFACE_TYPE_ENT)inFields[0];
        interfaceInfo.interfaceInfo.devPort.devNum=(GT_U8)inFields[1];
        interfaceInfo.interfaceInfo.devPort.portNum=(GT_U8)inFields[2];
        interfaceInfo.interfaceInfo.trunkId=(GT_TRUNK_ID)inFields[3];
        interfaceInfo.interfaceInfo.vidx=(GT_U16)inFields[4];
        interfaceInfo.interfaceInfo. vlanId=(GT_U16)inFields[5];
        break;

   case 1:

       interfaceInfo.outlifPointer.tunnelStartPtr.passengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[0];
       interfaceInfo.outlifPointer.tunnelStartPtr.ptr=(GT_U32)inFields[1];
       interfaceInfo.interfaceInfo.devPort.devNum=(GT_U8)inFields[2];
       interfaceInfo.interfaceInfo.devPort.portNum=(GT_U8)inFields[3];
       break;


   default:
       return GT_BAD_PARAM;
   }

    /* call cpss api function */
    result = cpssExMxPmMirrorDestInterfaceSet(
        devNum, mirrorType, analyzerNum, &interfaceInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmMirrorDestInterfaceSet
*
* DESCRIPTION:
*       Set Ingerss/Egress/Ingress Policy Monitor Configuration.
*       Set Monitor type: port trunk, vlan or tunnel and Monitor Parameters.
*       Monitor Parameters are set according to Monitor Configuration type.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum              - the device number
*       mirrorType          - mirror type: Ingress/Egress/Ingress Policy
*       interfaceInfoPtr    - (pointer to) destination interface. The accepted
*                             interfaces are: portNum and devNum when interface
*                             is port, trunkId when interface is Trunk, vlanId
*                             when monitor is vlan and tunnel type, pointer,
*                             deviceNum and portNum when interface is tunnel.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM  - wrong device number,port number or mirror type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - interface parameter out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
/*puma 2 wrapper*/
static CMD_STATUS wrCpssExMxPm2MirrorDestInterfaceSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    CPSS_EXMXPM_OUTLIF_INFO_STC interfaceInfo;
    GT_U32 analyzerNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

   devNum = (GT_U8)inArgs[0];
   mirrorType=(CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[1];
   analyzerNum =
       (mirrorType == CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E)
        ? 1 : 0;


    cmdOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));

    interfaceInfo.outlifType = (CPSS_EXMXPM_OUTLIF_TYPE_ENT)inFields[0];

    switch(interfaceInfo.outlifType)
    {
    case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
        interfaceInfo.outlifPointer.arpPtr=(GT_U32)inFields[1];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
        interfaceInfo.outlifPointer.ditPtr=(GT_U32)inFields[2];
        break;
    case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:

        interfaceInfo.outlifPointer.tunnelStartPtr.passengerPacketType=(CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[3];
        interfaceInfo.outlifPointer.tunnelStartPtr.ptr=(GT_U32)inFields[4];


        break;
    default:
        break;
    }


    interfaceInfo.interfaceInfo.type=(CPSS_INTERFACE_TYPE_ENT)inFields[5] ;

    switch(interfaceInfo.interfaceInfo.type)
    {
    case 0:
        interfaceInfo.interfaceInfo.devPort.devNum = (GT_U8)inFields[6];
        interfaceInfo.interfaceInfo.devPort.portNum = (GT_U8)inFields[7];
        CONVERT_DEV_PORT_DATA_MAC(interfaceInfo.interfaceInfo.devPort.devNum,
            interfaceInfo.interfaceInfo.devPort.portNum);

        break;

    case 1:
        interfaceInfo.interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[8];
        break;

    case 2:
        interfaceInfo.interfaceInfo.vidx = (GT_U16)inFields[9];
        break;

    case 3:
        interfaceInfo.interfaceInfo.vlanId = (GT_U16)inFields[10];
        break;

    default:
        break;
    }
    /* call cpss api function */
    result = cpssExMxPmMirrorDestInterfaceSet(
        devNum, mirrorType, analyzerNum, &interfaceInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorDestInterfaceGet
*
* DESCRIPTION:
*       Get for given mirror type (Ingerss/Egress/Policer) Monitor Configuration.
*       Monitor Parameters are set according to Monitor Configuration type: port
*       trunk, vlan and tunnel.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - the device number
*       mirrorType  - mirror type: Ingress/Egress or Ingress Policy
*
* OUTPUTS:
*       interfaceInfoPtr    - (pointer to) destination interface. The accepted
*                             interfaces are: portNum and devNum when interface
*                             is port, trunkId when interface is Trunk, vlanId
*                             when monitor is vlan and tunnel type, pointer,
*                             deviceNum and portNum when interface is tunnel.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or mirror type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*      Wrapper to get first element
*
*******************************************************************************/

/*puma 1 backward compatibility wrapper*/
static CMD_STATUS wrCpssExMxPmMirrorDestInterfaceGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    CPSS_EXMXPM_OUTLIF_INFO_STC interfaceInfo;
    GT_U32 analyzerNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrorType=(CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[1];
    analyzerNum =
        (mirrorType == CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E)
         ? 1 : 0;

    cmdOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));

    /* call cpss api function */
    result = cpssExMxPmMirrorDestInterfaceGet(
        devNum, mirrorType, analyzerNum, &interfaceInfo);

    if(result !=GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

   switch(interfaceInfo.outlifType){

   case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
   case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:

        inFields[0]=interfaceInfo.interfaceInfo.type;
        inFields[1]=interfaceInfo.interfaceInfo.devPort.devNum;
        inFields[2]=interfaceInfo.interfaceInfo.devPort.portNum;
        inFields[3]=interfaceInfo.interfaceInfo.trunkId ;
        inFields[4]=interfaceInfo.interfaceInfo.vidx;
        inFields[5]=interfaceInfo.interfaceInfo. vlanId ;

        /* pack and output table fields */
       fieldOutput("%d%d%d%d%d%d",inFields[0],inFields[1],inFields[2],
                                  inFields[3], inFields[4],inFields[5]);

       /* pack output arguments to galtis string */
       galtisOutput(outArgs, result, "%d%f", 0);

        break;

   case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
       inFields[0]=interfaceInfo.outlifPointer.tunnelStartPtr.passengerPacketType;
       inFields[1]=interfaceInfo.outlifPointer.tunnelStartPtr.ptr;
       inFields[2]=interfaceInfo.interfaceInfo.devPort.devNum;
       inFields[3]=interfaceInfo.interfaceInfo.devPort.portNum;

       fieldOutput("%d%d%d%d",inFields[0],inFields[1],inFields[2],inFields[3]);
        /* pack output arguments to galtis string */
       galtisOutput(outArgs, result, "%d%f", 1);
       break;


   default:
       return GT_BAD_PARAM;
   }



    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmMirrorDestInterfaceGet
*
* DESCRIPTION:
*       Get Ingerss/Egress/Ingress Policy Monitor Configuration.
*       Get Monitor type: port trunk, vlan or tunnel and Monitor Parameters.
*       Monitor Parameters are set according to Monitor Configuration type.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - the device number
*       mirrorType  - mirror type: Ingress/Egress/Ingress Policy
*
* OUTPUTS:
*       interfaceInfoPtr    - (pointer to) destination interface. The accepted
*                             interfaces are: portNum and devNum when interface
*                             is port, trunkId when interface is Trunk, vlanId
*                             when monitor is vlan and tunnel type, pointer,
*                             deviceNum and portNum when interface is tunnel.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM  - wrong device number,port number or mirror type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
/*puma 2 wrapper*/
static CMD_STATUS wrCpssExMxPm2MirrorDestInterfaceGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    CPSS_EXMXPM_OUTLIF_INFO_STC interfaceInfo;
    GT_U8                      tempDev,tempPort;
    GT_U32                     analyzerNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrorType=(CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[1];
    analyzerNum =
        (mirrorType == CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E)
         ? 1 : 0;

    cmdOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));

    /* call cpss api function */
    result = cpssExMxPmMirrorDestInterfaceGet(
        devNum, mirrorType, analyzerNum, &interfaceInfo);

     if(result !=GT_OK)
 {
     galtisOutput(outArgs, result, "");
     return CMD_OK;
 }


     inFields[0]=interfaceInfo.outlifType;



     switch(interfaceInfo.outlifType)
     {
     case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
         inFields[1]=interfaceInfo.outlifPointer.arpPtr;
         break;
     case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
         inFields[2]=interfaceInfo.outlifPointer.ditPtr;
         break;
     case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:

         inFields[3]=interfaceInfo.outlifPointer.tunnelStartPtr.passengerPacketType;
         inFields[4]=interfaceInfo.outlifPointer.tunnelStartPtr.ptr;
         break;
     default:
         break;
     }


     inFields[5]=interfaceInfo.interfaceInfo.type;

     switch(inFields[5])
     {
     case 0:
         tempDev=interfaceInfo.interfaceInfo.devPort.devNum;
         tempPort=interfaceInfo.interfaceInfo.devPort.portNum ;
         CONVERT_BACK_DEV_PORT_DATA_MAC(tempDev,tempPort);
         inFields[6]=tempDev;
         inFields[7]=tempPort;
         break;

     case 1:
         inFields[8]=interfaceInfo.interfaceInfo.trunkId;
         break;

     case 2:
         inFields[9]=interfaceInfo.interfaceInfo.vidx;
         break;

     case 3:
         inFields[10]=interfaceInfo.interfaceInfo.vlanId;
         break;

     default:
         break;
     }


   fieldOutput("%d%d%d%d%d%d%d%d%d%d%d",inFields[0],inFields[1],inFields[2],inFields[3],
               inFields[4],inFields[5],inFields[6],inFields[7],inFields[8],
               inFields[9],inFields[10]);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmMirrorDestInterfaceGet
*
* DESCRIPTION:
*       Get for given mirror type (Ingerss/Egress/Policer) Monitor Configuration.
*       Monitor Parameters are set according to Monitor Configuration type: port
*       trunk, vlan and tunnel.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - the device number
*       mirrorType  - mirror type: Ingress/Egress or Ingress Policy
*
* OUTPUTS:
*       interfaceInfoPtr    - (pointer to) destination interface. The accepted
*                             interfaces are: portNum and devNum when interface
*                             is port, trunkId when interface is Trunk, vlanId
*                             when monitor is vlan and tunnel type, pointer,
*                             deviceNum and portNum when interface is tunnel.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or mirror type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*      Wrapper to end refreash function
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMirrorDestInterfaceGetNext
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
* cpssExMxPmMirrorDestInterfaceSet
*
* DESCRIPTION:
*       Set for given mirror type (Ingerss/Egress/Policer) Monitor Configuration.
*       Monitor Parameters are set according to Monitor Configuration type: port
*       trunk, vlan and tunnel.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum              - the device number
*       mirrorType          - mirror type: Ingress/Egress or Ingress Policy
*       interfaceInfoPtr    - (pointer to) destination interface. The accepted
*       pclAnalyzerNum      - PCL Analyzer number (1..7) - relevant only for
*                             mirrorType == CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E
*                             interfaces are: portNum and devNum when interface
*                             is port, trunkId when interface is Trunk, vlanId
*                             when monitor is vlan and tunnel type, pointer,
*                             deviceNum and portNum when interface is tunnel.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or mirror type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - interface parameter out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
/*puma 2 wrapper*/
static CMD_STATUS wrCpssExMxPm2MirrorDestInterfaceV1Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    CPSS_EXMXPM_OUTLIF_INFO_STC interfaceInfo;
    GT_U32                      pclAnalyzerNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrorType = (CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[1];

    if (mirrorType == CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E)
        pclAnalyzerNum = (GT_U32)inArgs[2];
    else
        pclAnalyzerNum = 0;

    cmdOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));

    interfaceInfo.outlifType = (CPSS_EXMXPM_OUTLIF_TYPE_ENT)inFields[0];

    switch(interfaceInfo.outlifType)
    {
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            interfaceInfo.outlifPointer.arpPtr = (GT_U32)inFields[1];
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            interfaceInfo.outlifPointer.ditPtr = (GT_U32)inFields[2];
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            interfaceInfo.outlifPointer.tunnelStartPtr.passengerPacketType = (CPSS_EXMXPM_TS_PASSENGER_PACKET_TYPE_ENT)inFields[3];
            interfaceInfo.outlifPointer.tunnelStartPtr.ptr = (GT_U32)inFields[4];
            break;
        default:
            break;
    }

    interfaceInfo.interfaceInfo.type = (CPSS_INTERFACE_TYPE_ENT)inFields[5];

    switch(interfaceInfo.interfaceInfo.type)
    {
        case 0:
            interfaceInfo.interfaceInfo.devPort.devNum = (GT_U8)inFields[6];
            interfaceInfo.interfaceInfo.devPort.portNum = (GT_U8)inFields[7];
            CONVERT_DEV_PORT_DATA_MAC(interfaceInfo.interfaceInfo.devPort.devNum,
                                 interfaceInfo.interfaceInfo.devPort.portNum);
            break;
        case 1:
            interfaceInfo.interfaceInfo.trunkId = (GT_TRUNK_ID)inFields[8];
            break;
        case 2:
            interfaceInfo.interfaceInfo.vidx = (GT_U16)inFields[9];
            break;
        case 3:
            interfaceInfo.interfaceInfo.vlanId = (GT_U16)inFields[10];
            break;
        default:
            break;
    }


    /* call cpss api function */
    result = cpssExMxPmMirrorDestInterfaceSet(
        devNum, mirrorType, pclAnalyzerNum, &interfaceInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorDestInterfaceGet
*
* DESCRIPTION:
*       Get Ingerss/Egress/Ingress Policy Monitor Configuration.
*       Get Monitor type: port trunk, vlan or tunnel and Monitor Parameters.
*       Monitor Parameters are set according to Monitor Configuration type.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - the device number
*       mirrorType  - mirror type: Ingress/Egress/Ingress Policy
*       pclAnalyzerNum      - PCL Analyzer number (1..7) - relevant only for
*                             mirrorType == CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E
*
* OUTPUTS:
*       interfaceInfoPtr    - (pointer to) destination interface. The accepted
*                             interfaces are: portNum and devNum when interface
*                             is port, trunkId when interface is Trunk, vlanId
*                             when monitor is vlan and tunnel type, pointer,
*                             deviceNum and portNum when interface is tunnel.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM  - wrong device number,port number or mirror type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
/*puma 2 wrapper*/
static CMD_STATUS wrCpssExMxPm2MirrorDestInterfaceV1GetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_EXMXPM_MIRROR_TYPE_ENT mirrorType;
    CPSS_EXMXPM_OUTLIF_INFO_STC interfaceInfo;
    GT_U8                       tempDev;
    GT_U8                       tempPort;
    GT_U32                      pclAnalyzerNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrorType = (CPSS_EXMXPM_MIRROR_TYPE_ENT)inArgs[1];

    if (mirrorType == CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E)
        pclAnalyzerNum = (GT_U32)inArgs[2];
    else
        pclAnalyzerNum = 0;

    cmdOsMemSet(&interfaceInfo, 0, sizeof(interfaceInfo));

    /* call cpss api function */
    result = cpssExMxPmMirrorDestInterfaceGet(
        devNum, mirrorType, pclAnalyzerNum, &interfaceInfo);

     if(result != GT_OK)
     {
         galtisOutput(outArgs, result, "");
         return CMD_OK;
     }

     inFields[0] = interfaceInfo.outlifType;

     switch(interfaceInfo.outlifType)
     {
        case CPSS_EXMXPM_OUTLIF_TYPE_LL_E:
            inFields[1]=interfaceInfo.outlifPointer.arpPtr;
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_DIT_E:
            inFields[2]=interfaceInfo.outlifPointer.ditPtr;
            break;
        case CPSS_EXMXPM_OUTLIF_TYPE_TUNNEL_E:
            inFields[3]=interfaceInfo.outlifPointer.tunnelStartPtr.passengerPacketType;
            inFields[4]=interfaceInfo.outlifPointer.tunnelStartPtr.ptr;
            break;
        default:
            break;
     }

     inFields[5] = interfaceInfo.interfaceInfo.type;

     switch(interfaceInfo.interfaceInfo.type)
     {
        case 0:
            tempDev = interfaceInfo.interfaceInfo.devPort.devNum;
            tempPort = interfaceInfo.interfaceInfo.devPort.portNum ;
            CONVERT_BACK_DEV_PORT_DATA_MAC(tempDev,tempPort);
            inFields[6] = tempDev;
            inFields[7] = tempPort;
            break;
        case 1:
            inFields[8] = interfaceInfo.interfaceInfo.trunkId;
            break;
        case 2:
            inFields[9] = interfaceInfo.interfaceInfo.vidx;
            break;
        case 3:
            inFields[10] = interfaceInfo.interfaceInfo.vlanId;
            break;
        default:
            break;
     }


     fieldOutput("%d%d%d%d%d%d%d%d%d%d%d",
                 inFields[0], inFields[1], inFields[2], inFields[3],
                 inFields[4], inFields[5], inFields[6], inFields[7],
                 inFields[8], inFields[9], inFields[10]);

     /* pack output arguments to galtis string */
     galtisOutput(outArgs, result, "%f");

     return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmMirrorDestInterfaceGet
*
* DESCRIPTION:
*       Get for given mirror type (Ingerss/Egress/Policer) Monitor Configuration.
*       Monitor Parameters are set according to Monitor Configuration type: port
*       trunk, vlan and tunnel.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - the device number
*       mirrorType  - mirror type: Ingress/Egress or Ingress Policy
*       pclAnalyzerNum      - PCL Analyzer number (1..7) - relevant only for
*                             mirrorType == CPSS_EXMXPM_MIRROR_TYPE_INGRESS_POLICY_E
*
* OUTPUTS:
*       interfaceInfoPtr    - (pointer to) destination interface. The accepted
*                             interfaces are: portNum and devNum when interface
*                             is port, trunkId when interface is Trunk, vlanId
*                             when monitor is vlan and tunnel type, pointer,
*                             deviceNum and portNum when interface is tunnel.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or mirror type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*      Wrapper to end refreash function
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPm2MirrorDestInterfaceV1GetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!outArgs)
        return CMD_AGENT_ERROR;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorQosSet
*
* DESCRIPTION:
*       Set Monitor QoS parameters assigned to packets mirrored to Analyzer:
*       enable Qos, TC and DP.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum              - the device number
*       mirrorQosType       - mirror QoS type: Ingress, Egress, Ingress Policy
*                                          duplicated STC, duplicated mirrored
*       monitorQosConfigPtr - (pointer to) Analyzer QoS Configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number, mirror type or DP.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on TC out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMirrorQosSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_QOS_TYPE_ENT mirrorType;
    CPSS_EXMXPM_MIRROR_QOS_STC monitorQosConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrorType=(CPSS_EXMXPM_MIRROR_QOS_TYPE_ENT)inArgs[1];
    monitorQosConfig.dp=(CPSS_DP_LEVEL_ENT)inArgs[2];
    monitorQosConfig.qosEnable=(GT_BOOL)inArgs[3];
    monitorQosConfig.tc=(GT_U32)inArgs[4];


    /* call cpss api function */
    result = cpssExMxPmMirrorQosSet(devNum, mirrorType, &monitorQosConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorQosGet
*
* DESCRIPTION:
*       Get Monitor QoS parameters assigned to packets mirrored to Analyzer:
*       enable Qos, TC and DP.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - the device number
*       mirrorQosType       - mirror QoS type: Ingress, Egress, Ingress Policy
*                                          duplicated STC, duplicated mirrored
*
* OUTPUTS:
*       monitorQosConfigPtr - (pointer to)  Analyzer QoS Configuration
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number, mirror type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmMirrorQosGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_QOS_TYPE_ENT mirrorType;
    CPSS_EXMXPM_MIRROR_QOS_STC monitorQosConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mirrorType=(CPSS_EXMXPM_MIRROR_QOS_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMirrorQosGet(devNum, mirrorType, &monitorQosConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", monitorQosConfig.dp, monitorQosConfig.qosEnable, monitorQosConfig.tc);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorVlanTagInfoSet
*
* DESCRIPTION:
*      Configure Analyzer Vlan Tag details: EtherType, VID, VPT, CFI.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*      devNum                  - device number.
*      mirrorType              - mirror type: Ingress/Egress or Ingress Policy
*      infoPtr                 - Pointer to analyzer Vlan tag info:
*                                 EtherType, VPT, CFI and VID
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number or mirror type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - one of VLAN tag parameters out of range
*
*
* COMMENTS:
*
*******************************************************************************/
/*puma 1 backward compatibility wrapper*/
static CMD_STATUS wrCpssExMxPmMirrorVlanTagInfoSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;

    CPSS_EXMXPM_MIRROR_VLAN_TAG_INFO_STC info;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    info.vpt=(GT_U32)inArgs[3];
    info.cfiBit= (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmMirrorVlanTagInfoSet(devNum,  &info);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorVlanTagInfoGet
*
* DESCRIPTION:
*      Configure Analyzer Vlan Tag details: EtherType, VID, VPT, CFI.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*      devNum       - device number.
*      mirrorType   - mirror type: Ingress/Egress or Ingress Policy
*
* OUTPUTS:
*      infoPtr      - Pointer to analyzer Vlan tag info:
*                                 EtherType, VPT, CFI and VID
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number or mirror type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
*
* COMMENTS:
*
*******************************************************************************/
/*puma 1 backward compatibility wrapper*/
static CMD_STATUS wrCpssExMxPmMirrorVlanTagInfoGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_VLAN_TAG_INFO_STC info;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];


    /* call cpss api function */
    result = cpssExMxPmMirrorVlanTagInfoGet(devNum,&info);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",0,info.vpt,info.cfiBit,0);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmMirrorVlanTagInfoSet
*
* DESCRIPTION:
*      Configure Analyzer Vlan Tag details: VPT and CFI.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*      devNum                  - device number.
*      infoPtr                 - Pointer to analyzer Vlan tag info: VPT and CFI
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number or mirror type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - one of VLAN tag parameters out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
/*puma 2 wrapper*/
static CMD_STATUS wrCpssExMxPm2MirrorVlanTagInfoSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_VLAN_TAG_INFO_STC info;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

     devNum = (GT_U8)inArgs[0];
     info.vpt=(GT_U32)inArgs[1];
     info.cfiBit= (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmMirrorVlanTagInfoSet(devNum, &info);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorVlanTagInfoGet
*
* DESCRIPTION:
*      Configure Analyzer Vlan Tag details: VPT and CFI.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*      devNum       - device number.
*
* OUTPUTS:
*      infoPtr      - Pointer to analyzer Vlan tag info: VPT and CFI
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
/*puma 2 wrapper*/
static CMD_STATUS wrCpssExMxPm2MirrorVlanTagInfoGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_MIRROR_VLAN_TAG_INFO_STC info;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmMirrorVlanTagInfoGet(devNum, &info);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",info.vpt,info.cfiBit);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorPortAnalyzerToVlanEnableSet
*
* DESCRIPTION:
*       Enable/Disable adding a remote analyzer VLAN tag to an analyzer port.
*       This configuration is valid only when the port is a member in the
*       VLAN analyzer.
*       Analyzer VLAN tag is set by cpssExMxPmMirrorDestInterfaceSet.
*       CFI and VPT is configured by cpssExMxPmMirrorDestInterfaceSet
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number
*       enable      - GT_TRUE - analyzer VLAN tag is added to packets mirrored to
*                               the port.
*                     GT_FALSE - analyzer VLAN tag is not added to packets mirrored
*                               to the port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device number
*       GT_HW_ERROR              - on writing to HW error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
/*puma 2 wrapper*/
static CMD_STATUS wrCpssExMxPmMirrorPortAnalyzerToVlanEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    GT_BOOL     enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

     devNum  = (GT_U8)inArgs[0];
     portNum = (GT_U8)inArgs[1];
     enable  = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmMirrorPortAnalyzerToVlanEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmMirrorPortAnalyzerToVlanEnableGet
*
* DESCRIPTION:
*       Get the status of adding a remote analyzer VLAN tag to an analyzer port.
*       This configuration is valid only when the port is a member in the
*       VLAN analyzer.
*       Analyzer VLAN tag is set by cpssExMxPmMirrorDestInterfaceSet.
*       CFI and VPT is configured by cpssExMxPmMirrorDestInterfaceSet
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:

*       devNum      - device number.
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - analyzer VLAN tag is added to packets mirrored to
*                               the port.
*                     GT_FALSE - analyzer VLAN tag is not added to packets mirrored
*                               to the port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device number
*       GT_HW_ERROR              - on writing to HW error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
/*puma 2 wrapper*/
static CMD_STATUS wrCpssExMxPmMirrorPortAnalyzerToVlanEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    GT_BOOL     enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmMirrorPortAnalyzerToVlanEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",enable);

    return CMD_OK;
}


/**** database initialization **************************************/




static CMD_COMMAND dbCommands[] =
{

        {"cpssExMxPmMirrorStatEnableSet",
         &wrCpssExMxPmMirrorStatEnableSet,
         3, 0},
        {"cpssExMxPmMirrorStatEnableGet",
         &wrCpssExMxPmMirrorStatEnableGet,
         2, 0},
        {"cpssExMxPmMirrorStatRatioSet",
         &wrCpssExMxPmMirrorStatRatioSet,
         3, 0},
        {"cpssExMxPmMirrorStatRatioGet",
         &wrCpssExMxPmMirrorStatRatioGet,
         2, 0},
        {"cpssExMxPmMirrorPortEnableSet",
         &wrCpssExMxPmMirrorPortEnableSet,
         4, 0},
        {"cpssExMxPmMirrorPortEnableGet",
         &wrCpssExMxPmMirrorPortEnableGet,
         3, 0},

        {"cpssExMxPmMirrorDestInterfaceSet",
         &wrCpssExMxPmMirrorDestInterfaceSet,
         3, 6},
        {"cpssExMxPmMirrorDestInterfaceGetFirst",
         &wrCpssExMxPmMirrorDestInterfaceGetFirst,
         3, 0},
        {"cpssExMxPmMirrorDestInterfaceGetNext",
         &wrCpssExMxPmMirrorDestInterfaceGetNext,
         3, 0},

        {"cpssExMxPm2MirrorDestInterfaceSet",
         &wrCpssExMxPm2MirrorDestInterfaceSet,
         2, 11},
        {"cpssExMxPm2MirrorDestInterfaceGetFirst",
         &wrCpssExMxPm2MirrorDestInterfaceGetFirst,
         2, 0},
        {"cpssExMxPm2MirrorDestInterfaceGetNext",
         &wrCpssExMxPmMirrorDestInterfaceGetNext,
         2, 0},

        {"cpssExMxPm2MirrorDestInterfaceV1Set",
         &wrCpssExMxPm2MirrorDestInterfaceV1Set,
         3, 11},
        {"cpssExMxPm2MirrorDestInterfaceV1GetFirst",
         &wrCpssExMxPm2MirrorDestInterfaceV1GetFirst,
         3, 0},
        {"cpssExMxPm2MirrorDestInterfaceGetNext",
         &wrCpssExMxPm2MirrorDestInterfaceV1GetNext,
         3, 0},

        {"cpssExMxPmMirrorQosSet",
         &wrCpssExMxPmMirrorQosSet,
         5, 0},
        {"cpssExMxPmMirrorQosGet",
         &wrCpssExMxPmMirrorQosGet,
         2, 0},
        {"cpssExMxPmMirrorQosGetExt",
         &wrCpssExMxPmMirrorQosGet,
         2, 0},
        {"cpssExMxPmMirrorVlanTagInfoSet",
         &wrCpssExMxPmMirrorVlanTagInfoSet,
         6, 0},
        {"cpssExMxPmMirrorVlanTagInfoGet",
         &wrCpssExMxPmMirrorVlanTagInfoGet,
         2, 0},

        {"cpssExMxPm2MirrorVlanTagInfoSet",
         &wrCpssExMxPm2MirrorVlanTagInfoSet,
         3, 0},
        {"cpssExMxPm2MirrorVlanTagInfoGet",
         &wrCpssExMxPm2MirrorVlanTagInfoGet,
         1, 0},

        {"cpssExMxPmMirrorPortAnalyzerToVlanEnableSet",
         &wrCpssExMxPmMirrorPortAnalyzerToVlanEnableSet,
         3, 0},
        {"cpssExMxPmMirrorPortAnalyzerToVlanEnableGet",
         &wrCpssExMxPmMirrorPortAnalyzerToVlanEnableGet,
         2, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmMirror
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
GT_STATUS cmdLibInitCpssExMxPmMirror
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

