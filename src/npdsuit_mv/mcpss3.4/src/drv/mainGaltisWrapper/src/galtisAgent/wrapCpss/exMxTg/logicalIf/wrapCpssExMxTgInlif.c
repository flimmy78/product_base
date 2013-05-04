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
* wrapInlifTgCpss.c
*
* DESCRIPTION:
*       Wrapper functions for Inlif Tg cpss functions
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
#include <cpss/exMx/exMxGen/logicalIf/cpssExMxGenInlifTypes.h>
#include <cpss/exMx/exMxTg/logicalIf/cpssExMxTgInlif.h>
#include <cpss/exMx/exMxGen/logicalIf/cpssExMxInlif.h>
#include <cpss/exMx/exMxGen/logicalIf/private/prvCpssExMxInlif.h>

/************ExMxTgInlifPort Table***********/
#define TG_INLIF_PORT_NUM_OF_FIELDS 37

/*******************************************************************************
* cpssExMxTgInlifPortFieldsSet
*
* DESCRIPTION:
*       This function sets number of fields in port InLIF entry
*
* APPLICABLE DEVICES: 98Ex1x6 devices
*
* INPUTS:
*      devNum        -   pp number of the port set.
*      portNum       -   virtual port number to be set. port is used as index
*                        in the Port-InLif Table
*      fieldTypePtr  -   The enumeration of the fields to be set.
*      fieldValuePtr -   The values to set
*      numOfFields   -   Number of fields to set
*.
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on other error.
*       GT_BAD_PARAM    - on wrong device/port number
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifPortFieldsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    GT_U8                           port;
    GT_U32                          numOfFields = 0;

    CPSS_EXMX_INLIF_FIELD_TYPE_ENT   fieldTypePtr[TG_INLIF_PORT_NUM_OF_FIELDS];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT  fieldValuePtr[TG_INLIF_PORT_NUM_OF_FIELDS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    port = (GT_U8)inFields[0];

    if(inFields[1] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DIS_BRG_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[1] - 1;
        numOfFields++;
    }
    if(inFields[2] < 0x7FFFFFFF)/*bit field*/
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PORT_FABRIC_LOAD_BALANCING_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[2];
        numOfFields++;
    }
    if(inFields[3] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV4_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[3] - 1;
        numOfFields++;
    }
    if(inFields[4] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV4_MC_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[4] - 1;
        numOfFields++;
    }
    if(inFields[5] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV6_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[5] - 1;
        numOfFields++;
    }
    if(inFields[6] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV6_MC_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[6] - 1;
        numOfFields++;
    }
    if(inFields[7] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_PCL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[7] - 1;
        numOfFields++;
    }
    if(inFields[8] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_PROFILE_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[8];
        numOfFields++;
    }
    if(inFields[9] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_FORCE_PCL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[9] - 1;
        numOfFields++;
    }
    if(inFields[10] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_MIRROR_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[10] - 1;
        numOfFields++;
    }
    if(inFields[11] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_ICMPV6_MLD_SNOOP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[11] - 1;
        numOfFields++;
    }
    if(inFields[12] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_SITEID_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[12];
        numOfFields++;
    }

    /*inFields[13]: VRID is allways 0 */

    if(inFields[14] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_NUM_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[14];
        numOfFields++;
    }
    if(inFields[15] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PREV_SIP_SPOOF_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[15] - 1;
        numOfFields++;
    }
    if(inFields[16] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_RES_SIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[16] - 1;
        numOfFields++;
    }
    if(inFields[17] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_INTER_SIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[17] - 1;
        numOfFields++;
    }
    if(inFields[18] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_MC_LOCAL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[18] - 1;
        numOfFields++;
    }
    if(inFields[19] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_RES_DIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[19] - 1;
        numOfFields++;
    }
    if(inFields[20] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_INTER_DIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[20] - 1;
        numOfFields++;
    }
    if(inFields[21] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_0_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[21];
        numOfFields++;
    }
    if(inFields[22] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_1_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[22];
        numOfFields++;
    }
    if(inFields[23] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_0_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[23] - 1;
        numOfFields++;
    }
    if(inFields[24] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_SRC_IP_SUB_LEN_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[24];
        numOfFields++;
    }
    if(inFields[25] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_1_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[25] - 1;
        numOfFields++;
    }
    if(cmdOsStrCmp((GT_8*)inFields[26], "00000000"))
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_SRC_IP_E;
        galtisIpAddr(&(fieldValuePtr[numOfFields].ipv4AddrField),
                    (GT_U8*)inFields[26]);
        numOfFields++;
    }
    if(inFields[27] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IGMP_DETECT_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[27] - 1;
        numOfFields++;
    }
    if(inFields[28] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_MIRROR_2_CPU_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[28] - 1;
        numOfFields++;
    }
    if(inFields[29] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DEFAULT_NH_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[29];
        numOfFields++;
    }
    if(inFields[30] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV6_MANAGEMENT_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[30] - 1;
        numOfFields++;
    }
    if(inFields[31] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV4_MANAGEMENT_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[31] - 1;
        numOfFields++;
    }
    if(inFields[32] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_RIP1_IPV4_BC_MIRROR_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[32] - 1;
        numOfFields++;
    }
    if(inFields[33] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[33] - 1;
        numOfFields++;
    }
    if(inFields[34] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DSCP_IPV6_TC_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[34];
        numOfFields++;
    }
    if(inFields[35] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_MARK_USER_PRIORITY_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[35] - 1;
        numOfFields++;
    }
    if(inFields[36] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_USER_PRIORITY_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[36];
        numOfFields++;
    }

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxTgInlifPortFieldsSet(devNum,port,fieldTypePtr,
                                    fieldValuePtr,numOfFields);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******ExMxTgInlifExpeditedTunnel Table*********/
static GT_U8 GetExpeditedTunnelNum;

/*******************************************************************************
* cpssExMxTgInlifExpeditedTunnelFieldsSet
*
* DESCRIPTION:
*       This function sets fields in Expedited Tunnel InLIF entry
*
* APPLICABLE DEVICES: 98Ex1x6 devices
*
* INPUTS:
*      devNum               -   pp number of the port set.
*      expeditedTunnelNum   -   Expedited Tunnel number to be set (0-7).
*      fieldTypePtr         -   The enumeration of the fields to be set.
*      fieldValuePtr        -   The values to set
*      numOfFields          -   Number of fields to set
*.
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on other error.
*       GT_BAD_PARAM    - on wrong device/port number
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifExpeditedTunnelFieldsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    GT_U8                           expeditedTunnelNum;
    GT_U32                          numOfFields = 0;

    CPSS_EXMX_INLIF_FIELD_TYPE_ENT   fieldTypePtr[TG_INLIF_PORT_NUM_OF_FIELDS];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT  fieldValuePtr[TG_INLIF_PORT_NUM_OF_FIELDS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    expeditedTunnelNum = (GT_U8)inFields[0];

    if(inFields[1] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DIS_BRG_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[1] - 1;
        numOfFields++;
    }
    if(inFields[2] < 0x7FFFFFFF)/*bit field*/
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PORT_FABRIC_LOAD_BALANCING_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[2];
        numOfFields++;
    }
    if(inFields[3] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV4_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[3] - 1;
        numOfFields++;
    }
    if(inFields[4] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV4_MC_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[4] - 1;
        numOfFields++;
    }
    if(inFields[5] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV6_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[5] - 1;
        numOfFields++;
    }
    if(inFields[6] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV6_MC_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[6] - 1;
        numOfFields++;
    }
    if(inFields[7] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_PCL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[7] - 1;
        numOfFields++;
    }
    if(inFields[8] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_PROFILE_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[8];
        numOfFields++;
    }
    if(inFields[9] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_FORCE_PCL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[9] - 1;
        numOfFields++;
    }
    if(inFields[10] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_MIRROR_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[10] - 1;
        numOfFields++;
    }
    if(inFields[11] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_ICMPV6_MLD_SNOOP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[11] - 1;
        numOfFields++;
    }
    if(inFields[12] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_SITEID_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[12];
        numOfFields++;
    }

    /*inFields[13]: VRID is allways 0 */

    if(inFields[14] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_NUM_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[14];
        numOfFields++;
    }
    if(inFields[15] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PREV_SIP_SPOOF_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[15] - 1;
        numOfFields++;
    }
    if(inFields[16] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_RES_SIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[16] - 1;
        numOfFields++;
    }
    if(inFields[17] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_INTER_SIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[17] - 1;
        numOfFields++;
    }
    if(inFields[18] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_MC_LOCAL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[18] - 1;
        numOfFields++;
    }
    if(inFields[19] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_RES_DIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[19] - 1;
        numOfFields++;
    }
    if(inFields[20] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_INTER_DIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[20] - 1;
        numOfFields++;
    }
    if(inFields[21] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_0_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[21];
        numOfFields++;
    }
    if(inFields[22] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_1_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[22];
        numOfFields++;
    }
    if(inFields[23] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_0_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[23] - 1;
        numOfFields++;
    }
    if(inFields[24] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_SRC_IP_SUB_LEN_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[24];
        numOfFields++;
    }
    if(inFields[25] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_1_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[25] - 1;
        numOfFields++;
    }
    if(cmdOsStrCmp((GT_8*)inFields[26], "00000000"))
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_SRC_IP_E;
        galtisIpAddr(&(fieldValuePtr[numOfFields].ipv4AddrField),
                    (GT_U8*)inFields[26]);
        numOfFields++;
    }
    if(inFields[27] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IGMP_DETECT_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[27] - 1;
        numOfFields++;
    }
    if(inFields[28] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_MIRROR_2_CPU_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[28] - 1;
        numOfFields++;
    }
    if(inFields[29] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DEFAULT_NH_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[29];
        numOfFields++;
    }
    if(inFields[30] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV6_MANAGEMENT_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[30] - 1;
        numOfFields++;
    }
    if(inFields[31] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV4_MANAGEMENT_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[31] - 1;
        numOfFields++;
    }
    if(inFields[32] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_RIP1_IPV4_BC_MIRROR_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[32] - 1;
        numOfFields++;
    }
    if(inFields[33] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[33] - 1;
        numOfFields++;
    }
    if(inFields[34] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DSCP_IPV6_TC_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[34];
        numOfFields++;
    }
    if(inFields[35] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_MARK_USER_PRIORITY_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[35] - 1;
        numOfFields++;
    }
    if(inFields[36] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_USER_PRIORITY_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[36];
        numOfFields++;
    }


    /* call cpss api function */
    result = cpssExMxTgInlifExpeditedTunnelFieldsSet(devNum,
                                                     expeditedTunnelNum,
                                                     fieldTypePtr,
                                                     fieldValuePtr,
                                                     numOfFields);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgInlifExpeditedTunnelFieldsGet
*
* DESCRIPTION:
*       This function get the Expedited Tunnel InLIF entry
*
* APPLICABLE DEVICES: 98Ex1x6 devices
*
* INPUTS:
*      devNum                   -   pp number of the port set.
*      expeditedTunnelNum       - Expedited Tunnel number to be set (0-7).
*.
* OUTPUTS:
*       inlifHwDataPtr  - The Inlif Entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on other error.
*       GT_BAD_PARAM    - on wrong device/port number
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifExpeditedTunnelFieldsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    type;

    CPSS_EXMX_INLIF_FIELD_TYPE_ENT   fieldTypeArr[36];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT  fieldValueArr[36];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(GetExpeditedTunnelNum > 7)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    devNum = (GT_U8)inArgs[0];
    type = (GT_U8)inArgs[1];

    fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
    fieldTypeArr[1] = CPSS_EXMX_INLIF_PORT_FABRIC_LOAD_BALANCING_E;
    fieldTypeArr[2] = CPSS_EXMX_INLIF_EN_IPV4_E;
    fieldTypeArr[3] = CPSS_EXMX_INLIF_EN_IPV4_MC_E;
    fieldTypeArr[4] = CPSS_EXMX_INLIF_EN_IPV6_E;
    fieldTypeArr[5] = CPSS_EXMX_INLIF_EN_IPV6_MC_E;
    fieldTypeArr[6] = CPSS_EXMX_INLIF_EN_PCL_E;
    fieldTypeArr[7] = CPSS_EXMX_INLIF_PCL_PROFILE_E;
    fieldTypeArr[8] = CPSS_EXMX_INLIF_FORCE_PCL_E;
    fieldTypeArr[9] = CPSS_EXMX_INLIF_EN_MIRROR_E;
    fieldTypeArr[10] = CPSS_EXMX_INLIF_EN_ICMPV6_MLD_SNOOP_E;
    fieldTypeArr[11] = CPSS_EXMX_INLIF_SITEID_E;
    fieldTypeArr[12] = CPSS_EXMX_INLIF_VRID_E;
    fieldTypeArr[13] = CPSS_EXMX_INLIF_NUM_E;
    fieldTypeArr[14] = CPSS_EXMX_INLIF_PREV_SIP_SPOOF_E;
    fieldTypeArr[15] = CPSS_EXMX_INLIF_DROP_RES_SIP_E;
    fieldTypeArr[16] = CPSS_EXMX_INLIF_DROP_INTER_SIP_E;
    fieldTypeArr[17] = CPSS_EXMX_INLIF_DROP_MC_LOCAL_E;
    fieldTypeArr[18] = CPSS_EXMX_INLIF_DROP_RES_DIP_E;
    fieldTypeArr[19] = CPSS_EXMX_INLIF_DROP_INTER_DIP_E;
    fieldTypeArr[20] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_0_E;
    fieldTypeArr[21] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_1_E;
    fieldTypeArr[22] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_0_E;
    fieldTypeArr[23] = CPSS_EXMX_INLIF_SRC_IP_SUB_LEN_E;
    fieldTypeArr[24] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_1_E;
    fieldTypeArr[25] = CPSS_EXMX_INLIF_SRC_IP_E;
    fieldTypeArr[26] = CPSS_EXMX_INLIF_EN_IGMP_DETECT_E;
    fieldTypeArr[27] = CPSS_EXMX_INLIF_MIRROR_2_CPU_E;
    fieldTypeArr[28] = CPSS_EXMX_INLIF_DEFAULT_NH_E;
    fieldTypeArr[29] = CPSS_EXMX_INLIF_EN_IPV6_MANAGEMENT_E;
    fieldTypeArr[30] = CPSS_EXMX_INLIF_EN_IPV4_MANAGEMENT_E;
    fieldTypeArr[31] = CPSS_EXMX_INLIF_EN_RIP1_IPV4_BC_MIRROR_E;
    fieldTypeArr[32] = CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E;
    fieldTypeArr[33] = CPSS_EXMX_INLIF_DSCP_IPV6_TC_E;
    fieldTypeArr[34] = CPSS_EXMX_INLIF_MARK_USER_PRIORITY_E;
    fieldTypeArr[35] = CPSS_EXMX_INLIF_USER_PRIORITY_E;

    result = cpssExMxTgInlifExpeditedTunnelFieldsGet(devNum,
                                                     GetExpeditedTunnelNum,
                                                     fieldTypeArr,
                                                     36,
                                                     fieldValueArr);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = GetExpeditedTunnelNum;
    inFields[1] = fieldValueArr[0].boolField+1;
    inFields[2] = fieldValueArr[1].u32Field;
    inFields[3] = fieldValueArr[2].boolField+1;
    inFields[4] = fieldValueArr[3].boolField+1;
    inFields[5] = fieldValueArr[4].boolField+1;
    inFields[6] = fieldValueArr[5].boolField+1;
    inFields[7] = fieldValueArr[6].boolField+1;
    inFields[8] = fieldValueArr[7].u32Field;
    inFields[9] = fieldValueArr[8].boolField+1;
    inFields[10] = fieldValueArr[9].boolField+1;
    inFields[11] = fieldValueArr[10].boolField+1;
    inFields[12] = fieldValueArr[11].u32Field;
    inFields[13] = fieldValueArr[12].u32Field;
    inFields[14] = fieldValueArr[13].u32Field;
    inFields[15] = fieldValueArr[14].boolField+1;
    inFields[16] = fieldValueArr[15].boolField+1;
    inFields[17] = fieldValueArr[16].boolField+1;
    inFields[18] = fieldValueArr[17].boolField+1;
    inFields[19] = fieldValueArr[18].boolField+1;
    inFields[20] = fieldValueArr[19].boolField+1;
    inFields[21] = fieldValueArr[20].u32Field;
    inFields[22] = fieldValueArr[21].u32Field;
    inFields[23] = fieldValueArr[22].boolField+1;
    inFields[24] = fieldValueArr[23].u32Field;
    inFields[25] = fieldValueArr[24].boolField+1;
    inFields[27] = fieldValueArr[26].boolField+1;
    inFields[28] = fieldValueArr[27].boolField+1;
    inFields[29] = fieldValueArr[28].u32Field;
    inFields[30] = fieldValueArr[29].boolField+1;
    inFields[31] = fieldValueArr[30].boolField+1;
    inFields[32] = fieldValueArr[31].boolField+1;
    inFields[33] = fieldValueArr[32].boolField+1;
    inFields[34] = fieldValueArr[33].u32Field;
    inFields[35] = fieldValueArr[34].boolField+1;
    inFields[36] = fieldValueArr[35].u32Field;

    GetExpeditedTunnelNum++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%4b%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10],inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25],
                fieldValueArr[25].ipv4AddrField.arIP, inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32], inFields[33], inFields[34], inFields[35],
                inFields[36]);

    galtisOutput(outArgs, GT_OK, "%d%f", type);

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifExpeditedTunnelFieldsGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GetExpeditedTunnelNum = 0;
    return wrCpssExMxTgInlifExpeditedTunnelFieldsGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifExpeditedTunnelFieldsGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssExMxTgInlifExpeditedTunnelFieldsGet(inArgs,inFields,numFields,outArgs);
}

/****************ExMxTgInlifVlan Table********************/

#define TG_INLIF_VLAN_NUM_OF_FIELDS 32
static GT_U16 vlanIndex;

/*******************************************************************************
* cpssExMxTgInlifVlanFieldsSet
*
* DESCRIPTION:
*       This function sets fieldS in vlan InLIF entry
*
* APPLICABLE DEVICES:  98Ex1x6 devices
*
* INPUTS:
*      devNum        -   pp number of the port set.
*      vlan          -   if multi inlif , vlan is used to index in Inlif Table.
*      fieldTypePtr  -   The enumeration of the fields to be set.
*      fieldValuePtr -   The values to set
*      numOfFields   -   Number of fields to set
*.
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on other error.
*       GT_BAD_PARAM    - on wrong device/vlan number, or fieldType
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       in reduced mode the addrOffset and fieldOffset must be according to the first
*       entry in the vlan inlifs line. The actual field that will be written depends
*       on the entry offset of the vlan inlif entry
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifVlanFieldsSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    GT_U16                          vlan;
    GT_U32                          numOfFields = 0;

    CPSS_EXMX_INLIF_FIELD_TYPE_ENT   fieldTypePtr[TG_INLIF_VLAN_NUM_OF_FIELDS];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT  fieldValuePtr[TG_INLIF_VLAN_NUM_OF_FIELDS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    vlan = (GT_U8)inFields[0];

    if(inFields[1] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DIS_BRG_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[1] - 1;
        numOfFields++;
    }
    if(inFields[2] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV4_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[2] - 1;
        numOfFields++;
    }
    if(inFields[3] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV4_MC_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[3] - 1;
        numOfFields++;
    }
    if(inFields[4] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV6_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[4] - 1;
        numOfFields++;
    }
    if(inFields[5] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV6_MC_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[5] - 1;
        numOfFields++;
    }
    if(inFields[6] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_PCL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[6] - 1;
        numOfFields++;
    }
    if(inFields[7] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_PROFILE_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[7];
        numOfFields++;
    }
    if(inFields[8] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_FORCE_PCL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[8] - 1;
        numOfFields++;
    }
    if(inFields[9] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_MIRROR_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[9] - 1;
        numOfFields++;
    }
    if(inFields[10] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_ICMPV6_MLD_SNOOP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[10] - 1;
        numOfFields++;
    }
    if(inFields[11] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_SITEID_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[11];
        numOfFields++;
    }

    /*inFields[12]: VRID is allways 0 */

    if(inFields[13] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_NUM_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[13];
        numOfFields++;
    }
    if(inFields[14] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PREV_SIP_SPOOF_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[14] - 1;
        numOfFields++;
    }
    if(inFields[15] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_RES_SIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[15] - 1;
        numOfFields++;
    }
    if(inFields[16] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_INTER_SIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[16] - 1;
        numOfFields++;
    }
    if(inFields[17] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_MC_LOCAL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[17] - 1;
        numOfFields++;
    }
    if(inFields[18] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_RES_DIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[18] - 1;
        numOfFields++;
    }
    if(inFields[19] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_INTER_DIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[19] - 1;
        numOfFields++;
    }
    if(inFields[20] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_0_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[20];
        numOfFields++;
    }
    if(inFields[21] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_1_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[21];
        numOfFields++;
    }
    if(inFields[22] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_0_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[22] - 1;
        numOfFields++;
    }
    if(inFields[23] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_SRC_IP_SUB_LEN_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[23];
        numOfFields++;
    }
    if(inFields[24] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_1_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[24] - 1;
        numOfFields++;
    }
    if(cmdOsStrCmp((GT_8*)inFields[25], "00000000"))
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_SRC_IP_E;
        galtisIpAddr(&(fieldValuePtr[numOfFields].ipv4AddrField),
                    (GT_U8*)inFields[25]);
        numOfFields++;
    }
    if(inFields[26] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IGMP_DETECT_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[26] - 1;
        numOfFields++;
    }
    if(inFields[27] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_MIRROR_2_CPU_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[27] - 1;
        numOfFields++;
    }
    if(inFields[28] < 0x7FFFFFFF)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DEFAULT_NH_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[28];
        numOfFields++;
    }
    if(inFields[29] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV6_MANAGEMENT_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[29] - 1;
        numOfFields++;
    }
    if(inFields[30] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV4_MANAGEMENT_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[30] - 1;
        numOfFields++;
    }
    if(inFields[31] > 0)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_RIP1_IPV4_BC_MIRROR_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[31] - 1;
        numOfFields++;
    }

    /* call cpss api function */
    result = cpssExMxTgInlifVlanFieldsSet(devNum, vlan, fieldTypePtr,
                                    fieldValuePtr, numOfFields);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgInlifVlanFieldsGet
*
* DESCRIPTION:
*       This function gets fieldS in vlan InLIF entry
*
* APPLICABLE DEVICES:  98Ex1x6 devices
*
* INPUTS:
*      devNum        -   pp number of the port get.
*      vlan          -   if multi inlif , vlan is used to index in Inlif Table [0..4095].
*      fieldTypePtr  -   The enumeration of the fields to get.
*      numOfFields   -   Number of fields to get
*.
* OUTPUTS:
*      fieldValuePtr -   The values to get
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_BAD_PARAM             - on wrong device/vlan number, or fieldType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       in reduced mode the addrOffset and fieldOffset must be according to the first
*       entry in the vlan inlifs line. The actual field that will be written depends
*       on the entry offset of the vlan inlif entry
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifVlanFieldsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    type;

    CPSS_EXMX_INLIF_FIELD_TYPE_ENT   fieldTypeArr[31];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT  fieldValueArr[31];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(vlanIndex > 4096)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    devNum = (GT_U8)inArgs[0];
    type = (GT_U8)inArgs[1];


    fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
    fieldTypeArr[1] = CPSS_EXMX_INLIF_EN_IPV4_E;
    fieldTypeArr[2] = CPSS_EXMX_INLIF_EN_IPV4_MC_E;
    fieldTypeArr[3] = CPSS_EXMX_INLIF_EN_IPV6_E;
    fieldTypeArr[4] = CPSS_EXMX_INLIF_EN_IPV6_MC_E;
    fieldTypeArr[5] = CPSS_EXMX_INLIF_EN_PCL_E;
    fieldTypeArr[6] = CPSS_EXMX_INLIF_PCL_PROFILE_E;
    fieldTypeArr[7] = CPSS_EXMX_INLIF_FORCE_PCL_E;
    fieldTypeArr[8] = CPSS_EXMX_INLIF_EN_MIRROR_E;
    fieldTypeArr[9] = CPSS_EXMX_INLIF_EN_ICMPV6_MLD_SNOOP_E;
    fieldTypeArr[10] = CPSS_EXMX_INLIF_SITEID_E;
    fieldTypeArr[11] = CPSS_EXMX_INLIF_VRID_E;
    fieldTypeArr[12] = CPSS_EXMX_INLIF_NUM_E;
    fieldTypeArr[13] = CPSS_EXMX_INLIF_PREV_SIP_SPOOF_E;
    fieldTypeArr[14] = CPSS_EXMX_INLIF_DROP_RES_SIP_E;
    fieldTypeArr[15] = CPSS_EXMX_INLIF_DROP_INTER_SIP_E;
    fieldTypeArr[16] = CPSS_EXMX_INLIF_DROP_MC_LOCAL_E;
    fieldTypeArr[17] = CPSS_EXMX_INLIF_DROP_RES_DIP_E;
    fieldTypeArr[18] = CPSS_EXMX_INLIF_DROP_INTER_DIP_E;
    fieldTypeArr[19] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_0_E;
    fieldTypeArr[20] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_1_E;
    fieldTypeArr[21] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_0_E;
    fieldTypeArr[22] = CPSS_EXMX_INLIF_SRC_IP_SUB_LEN_E;
    fieldTypeArr[23] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_1_E;
    fieldTypeArr[24] = CPSS_EXMX_INLIF_SRC_IP_E;
    fieldTypeArr[25] = CPSS_EXMX_INLIF_EN_IGMP_DETECT_E;
    fieldTypeArr[26] = CPSS_EXMX_INLIF_MIRROR_2_CPU_E;
    fieldTypeArr[27] = CPSS_EXMX_INLIF_DEFAULT_NH_E;
    fieldTypeArr[28] = CPSS_EXMX_INLIF_EN_IPV6_MANAGEMENT_E;
    fieldTypeArr[29] = CPSS_EXMX_INLIF_EN_IPV4_MANAGEMENT_E;
    fieldTypeArr[30] = CPSS_EXMX_INLIF_EN_RIP1_IPV4_BC_MIRROR_E;

    result = cpssExMxTgInlifVlanFieldsGet(devNum,vlanIndex,fieldTypeArr, 31, fieldValueArr);


    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = vlanIndex;
    inFields[1] = fieldValueArr[0].boolField+1;
    inFields[2] = fieldValueArr[1].boolField+1;
    inFields[3] = fieldValueArr[2].boolField+1;
    inFields[4] = fieldValueArr[3].boolField+1;
    inFields[5] = fieldValueArr[4].boolField+1;
    inFields[6] = fieldValueArr[5].boolField+1;
    inFields[7] = fieldValueArr[6].u32Field;
    inFields[8] = fieldValueArr[7].boolField+1;
    inFields[9] = fieldValueArr[8].boolField+1;
    inFields[10] = fieldValueArr[9].boolField+1;
    inFields[11] = fieldValueArr[10].u32Field;
    inFields[12] = fieldValueArr[11].u32Field;
    inFields[13] = fieldValueArr[12].u32Field;
    inFields[14] = fieldValueArr[13].boolField+1;
    inFields[15] = fieldValueArr[14].boolField+1;
    inFields[16] = fieldValueArr[15].boolField+1;
    inFields[17] = fieldValueArr[16].boolField+1;
    inFields[18] = fieldValueArr[17].boolField+1;
    inFields[19] = fieldValueArr[18].boolField+1;
    inFields[20] = fieldValueArr[19].u32Field;
    inFields[21] = fieldValueArr[20].u32Field;
    inFields[22] = fieldValueArr[21].boolField+1;
    inFields[23] = fieldValueArr[22].u32Field;
    inFields[24] = fieldValueArr[23].boolField+1;
    inFields[26] = fieldValueArr[25].boolField+1;
    inFields[27] = fieldValueArr[26].boolField+1;
    inFields[28] = fieldValueArr[27].u32Field;
    inFields[29] = fieldValueArr[28].boolField+1;
    inFields[30] = fieldValueArr[29].boolField+1;
    inFields[31] = fieldValueArr[30].boolField+1;

    vlanIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%4b%d%d%d%d%d%d",
                inFields[0], inFields[1],
                inFields[2], inFields[3], inFields[4], inFields[5],
                inFields[6], inFields[7], inFields[8], inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21],
                inFields[22], inFields[23], inFields[24],
                fieldValueArr[24].ipv4AddrField.arIP,
                inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31]);

    galtisOutput(outArgs, GT_OK, "%d%f", type);

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifVlanFieldsGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    vlanIndex=0;
    return wrCpssExMxTgInlifVlanFieldsGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifVlanFieldsGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssExMxTgInlifVlanFieldsGet(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxTgInlifVlanMappingSet
*
* DESCRIPTION:
*       Sets an entry in the per Vlan inlif mapping table.
*
* APPLICABLE DEVICES: 98Ex1x6 devices
*
* INPUTS:
*       devNum    - physical device number that represent the unit
*       vlan      - vlan is used as index in vlan to inlif mapping table.
*       index     - the inlif entry index
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifVlanMappingSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U16              vlan;
    GT_U32              index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlan = (GT_U16)inArgs[1];
    index = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxTgInlifVlanMappingSet(devNum, vlan, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgInlifExpeditedTunnelLookupModeSet
*
* DESCRIPTION:
*       The function configured the InLIf Lookup mode for the Expedited Tunnel.
*       Each Expedited Tunnel is independent and can work in every mode.
*
* APPLICABLE DEVICES: 98Ex1x6 devices
*
* INPUTS:
*
*       devNum               - pp number of the port set.
*       expeditedTunnelNum   - Expedited Tunnel number to be set (0-7).
*       inlifmode            - InLIF Lookup Mode
*
* OUTPUTS:
*           none.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on other error
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       legal values for inlifmode0 and inlifmode1
*       0 = Per Port InLIF - packets received on physical port are assigned the
*       Per Port InLIF entry
*       1 = Per VLAN InLIF -Packets received on physical port are assigned the
*       Per VLAN InLIF entry.
*       2 = Reserved
*       3 = Hybrid Per VLAN-Port InLIF- Packets received on physical port are
*       assigned some of the InLIF attributes from the Per Port VLAN and some
*       from the Per VLAN InLIF.
*

*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifExpeditedTunnelLookupModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    GT_U8                           expeditedTunnelNum;
    CPSS_EXMX_INLIF_PORT_MODE_ENT   inlifmode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    expeditedTunnelNum = (GT_U8)inArgs[1];
    inlifmode = (CPSS_EXMX_INLIF_PORT_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxTgInlifExpeditedTunnelLookupModeSet(devNum,
                                                         expeditedTunnelNum,
                                                         inlifmode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/*******************************************************************************
* cpssExMxTgInlifExpeditedTunnelLookupModeGet
*
* DESCRIPTION:
*       The function returns the InLIf Lookup mode for the Expedited Tunnel
*
* APPLICABLE DEVICES: 98Ex1x6 devices
*
* INPUTS:
*
*       devNum               - pp number of the port set.
*       expeditedTunnelNum   -   Expedited Tunnel number to be set (0-7).
*
* OUTPUTS:
*       inlifModePtr  - InLIF Lookup Mode
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on other error
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       legal values for inlifmode0 and inlifmode1
*       0 = Per Port InLIF - packets received on physical port are assigned the
*       Per Port InLIF entry
*       1 = Per VLAN InLIF -Packets received on physical port are assigned the
*       Per VLAN InLIF entry.
*       2 = Reserved
*       3 = Hybrid Per VLAN-Port InLIF- Packets received on physical port are
*       assigned some of the InLIF attributes from the Per Port VLAN and some
*       from the Per VLAN InLIF.
*       The decoding of LookUp mode field is done on the concatenation of both registers.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifExpeditedTunnelLookupModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    GT_U8                           expeditedTunnelNum;
    CPSS_EXMX_INLIF_PORT_MODE_ENT   inlifmode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    expeditedTunnelNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgInlifExpeditedTunnelLookupModeGet(devNum,
                                                         expeditedTunnelNum,
                                                         &inlifmode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", inlifmode);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable
*
* DESCRIPTION:
*       Enable Ipv4 over Mac Broadcast packets Mirror to CPU.
*
* APPLICABLE DEVICES: 98Ex1x6 devices
*
* INPUTS:
*       devNum    - physical device number
*       enable    - Enable/Disable the feature.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong device number
*       GT_HW_ERROR     - on hardware error
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8         devNum;
    GT_BOOL       enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgInlifIgmpCommandSet
*
* DESCRIPTION:
*       The following actions are applied to the packet if the <IGMP Detection>
*       is enabled on the InLIF
*
* APPLICABLE DEVICES: 98Ex1x6 devices
*
* INPUTS:
*       devNum     - physical device number
*       Command    -
*                0 = The packet’s forwarding decision remains as set by the
*                    forwarding engines.
*                1 = Trap to CPU. The packet will be forwarded to the CPU only.
*                2 = Mirror to CPU. The packet is forwarded as set by the
*                    device forwarding engines, and also transferred to the CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_BAD_PARAM    - on wrong device number, or command
*       GT_HW_ERROR     - on hardware error
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifIgmpCommandSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    CPSS_EXMX_INLIF_COMMAND_ENT     command;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    command = (CPSS_EXMX_INLIF_COMMAND_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTgInlifIgmpCommandSet(devNum, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTgInlifInit
*
* DESCRIPTION:
*       Initialize parameters in a given device
*
* APPLICABLE DEVICES: 98Ex1x6 devices
*
* INPUTS:
*       devNum - the device to set in
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on other error
*       GT_BAD_PARAM    - on wrong device/port number
*       GT_OUT_OF_CPU_MEM - on malloc failed
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxTgInlifInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifMainFieldsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32  UnionNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    UnionNum = (GT_U32)inArgs[1];

    switch(UnionNum)
    {

    case 0:
        return
          wrCpssExMxTgInlifPortFieldsSet(inArgs, inFields, numFields, outArgs);
        break;
    default:
        return CMD_AGENT_ERROR;
        break;
    }

}

static GT_U8 portNum = 0;
/*******************************************************************************
* cpssExMxTgInlifPortFieldsGet
*
* DESCRIPTION:
*       This function Gets number of fields in port InLIF entry
*
* APPLICABLE DEVICES: 98Ex1x6 devices
*
* INPUTS:
*      devNum        -   pp number of the port get.
*      portNum       -   virtual port number to get. port is used as index
*                        in the Port-InLif Table
*      fieldTypePtr  -   The enumeration of the fields to get.
*      numOfFields   -   Number of fields to get
*.
* OUTPUTS:
*      fieldValuePtr -   The values to Get
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_BAD_PARAM             - on wrong device/port number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifPortFieldsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    type;

    CPSS_EXMX_INLIF_FIELD_TYPE_ENT   fieldTypeArr[36];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT  fieldValueArr[36];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(portNum > 51) /* Num of Virtual ports. */
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    devNum = (GT_U8)inArgs[0];
    type = (GT_U8)inArgs[1];

    fieldTypeArr[0] = CPSS_EXMX_INLIF_DIS_BRG_E;
    fieldTypeArr[1] = CPSS_EXMX_INLIF_PORT_FABRIC_LOAD_BALANCING_E;
    fieldTypeArr[2] = CPSS_EXMX_INLIF_EN_IPV4_E;
    fieldTypeArr[3] = CPSS_EXMX_INLIF_EN_IPV4_MC_E;
    fieldTypeArr[4] = CPSS_EXMX_INLIF_EN_IPV6_E;
    fieldTypeArr[5] = CPSS_EXMX_INLIF_EN_IPV6_MC_E;
    fieldTypeArr[6] = CPSS_EXMX_INLIF_EN_PCL_E;
    fieldTypeArr[7] = CPSS_EXMX_INLIF_PCL_PROFILE_E;
    fieldTypeArr[8] = CPSS_EXMX_INLIF_FORCE_PCL_E;
    fieldTypeArr[9] = CPSS_EXMX_INLIF_EN_MIRROR_E;
    fieldTypeArr[10] = CPSS_EXMX_INLIF_EN_ICMPV6_MLD_SNOOP_E;
    fieldTypeArr[11] = CPSS_EXMX_INLIF_SITEID_E;
    fieldTypeArr[12] = CPSS_EXMX_INLIF_VRID_E;
    fieldTypeArr[13] = CPSS_EXMX_INLIF_NUM_E;
    fieldTypeArr[14] = CPSS_EXMX_INLIF_PREV_SIP_SPOOF_E;
    fieldTypeArr[15] = CPSS_EXMX_INLIF_DROP_RES_SIP_E;
    fieldTypeArr[16] = CPSS_EXMX_INLIF_DROP_INTER_SIP_E;
    fieldTypeArr[17] = CPSS_EXMX_INLIF_DROP_MC_LOCAL_E;
    fieldTypeArr[18] = CPSS_EXMX_INLIF_DROP_RES_DIP_E;
    fieldTypeArr[19] = CPSS_EXMX_INLIF_DROP_INTER_DIP_E;
    fieldTypeArr[20] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_0_E;
    fieldTypeArr[21] = CPSS_EXMX_INLIF_PCL_LOOKUP_ID_1_E;
    fieldTypeArr[22] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_0_E;
    fieldTypeArr[23] = CPSS_EXMX_INLIF_SRC_IP_SUB_LEN_E;
    fieldTypeArr[24] = CPSS_EXMX_INLIF_EN_LONG_PCL_LOOKUP_1_E;
    fieldTypeArr[25] = CPSS_EXMX_INLIF_SRC_IP_E;
    fieldTypeArr[26] = CPSS_EXMX_INLIF_EN_IGMP_DETECT_E;
    fieldTypeArr[27] = CPSS_EXMX_INLIF_MIRROR_2_CPU_E;
    fieldTypeArr[28] = CPSS_EXMX_INLIF_DEFAULT_NH_E;
    fieldTypeArr[29] = CPSS_EXMX_INLIF_EN_IPV6_MANAGEMENT_E;
    fieldTypeArr[30] = CPSS_EXMX_INLIF_EN_IPV4_MANAGEMENT_E;
    fieldTypeArr[31] = CPSS_EXMX_INLIF_EN_RIP1_IPV4_BC_MIRROR_E;
    fieldTypeArr[32] = CPSS_EXMX_INLIF_MARK_PORT_L3_COS_E;
    fieldTypeArr[33] = CPSS_EXMX_INLIF_DSCP_IPV6_TC_E;
    fieldTypeArr[34] = CPSS_EXMX_INLIF_MARK_USER_PRIORITY_E;
    fieldTypeArr[35] = CPSS_EXMX_INLIF_USER_PRIORITY_E;

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxTgInlifPortFieldsGet(devNum,portNum,fieldTypeArr,
                                    36,fieldValueArr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = portNum;
    inFields[1] = fieldValueArr[0].boolField+1;
    inFields[2] = fieldValueArr[1].u32Field;
    inFields[3] = fieldValueArr[2].boolField+1;
    inFields[4] = fieldValueArr[3].boolField+1;
    inFields[5] = fieldValueArr[4].boolField+1;
    inFields[6] = fieldValueArr[5].boolField+1;
    inFields[7] = fieldValueArr[6].boolField+1;
    inFields[8] = fieldValueArr[7].u32Field;
    inFields[9] = fieldValueArr[8].boolField+1;
    inFields[10] = fieldValueArr[9].boolField+1;
    inFields[11] = fieldValueArr[10].boolField+1;
    inFields[12] = fieldValueArr[11].u32Field;
    inFields[13] = fieldValueArr[12].u32Field;
    inFields[14] = fieldValueArr[13].u32Field;
    inFields[15] = fieldValueArr[14].boolField+1;
    inFields[16] = fieldValueArr[15].boolField+1;
    inFields[17] = fieldValueArr[16].boolField+1;
    inFields[18] = fieldValueArr[17].boolField+1;
    inFields[19] = fieldValueArr[18].boolField+1;
    inFields[20] = fieldValueArr[19].boolField+1;
    inFields[21] = fieldValueArr[20].u32Field;
    inFields[22] = fieldValueArr[21].u32Field;
    inFields[23] = fieldValueArr[22].boolField+1;
    inFields[24] = fieldValueArr[23].u32Field;
    inFields[25] = fieldValueArr[24].boolField+1;
    inFields[27] = fieldValueArr[26].boolField+1;
    inFields[28] = fieldValueArr[27].boolField+1;
    inFields[29] = fieldValueArr[28].u32Field;
    inFields[30] = fieldValueArr[29].boolField+1;
    inFields[31] = fieldValueArr[30].boolField+1;
    inFields[32] = fieldValueArr[31].boolField+1;
    inFields[33] = fieldValueArr[32].boolField+1;
    inFields[34] = fieldValueArr[33].u32Field;
    inFields[35] = fieldValueArr[34].boolField+1;
    inFields[36] = fieldValueArr[35].u32Field;


    portNum++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%4b%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10],inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25],
                fieldValueArr[25].ipv4AddrField.arIP, inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32], inFields[33], inFields[34], inFields[35],
                inFields[36]);

    galtisOutput(outArgs, GT_OK, "%d%f", type);

    return CMD_OK;
}


/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifPortFieldsGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    portNum = 0;
    return wrCpssExMxTgInlifPortFieldsGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxTgInlifPortFieldsGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssExMxTgInlifPortFieldsGet(inArgs,inFields,numFields,outArgs);
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

        {"cpssExMxTgInlifSet",
        &wrCpssExMxTgInlifMainFieldsSet,
        2, TG_INLIF_PORT_NUM_OF_FIELDS},

        {"cpssExMxTgInlifGetFirst",
        &wrCpssExMxTgInlifPortFieldsGetFirst,
        2, 0},

        {"cpssExMxTgInlifGetNext",
        &wrCpssExMxTgInlifPortFieldsGetNext,
        2, 0},

        {"cpssExMxTgInlifExpeditedTunnelSet",
        &wrCpssExMxTgInlifExpeditedTunnelFieldsSet,
        1, TG_INLIF_PORT_NUM_OF_FIELDS},

        {"cpssExMxTgInlifExpeditedTunnelGetFirst",
        &wrCpssExMxTgInlifExpeditedTunnelFieldsGetFirst,
        2, 0},

        {"cpssExMxTgInlifExpeditedTunnelGetNext",
        &wrCpssExMxTgInlifExpeditedTunnelFieldsGetNext,
        2, 0},

        {"cpssExMxTgInlifVlanSet",
        &wrCpssExMxTgInlifVlanFieldsSet,
        2, TG_INLIF_VLAN_NUM_OF_FIELDS},

        {"cpssExMxTgInlifVlanGetFirst",
        &wrCpssExMxTgInlifVlanFieldsGetFirst,
        2, 0},

        {"cpssExMxTgInlifVlanGetNext",
        &wrCpssExMxTgInlifVlanFieldsGetNext,
        2, 0},

        {"cpssExMxTgInlifVlanMappingSet",
        &wrCpssExMxTgInlifVlanMappingSet,
        3, 0},

        {"cpssExMxTgInlifExpeditedTunnelLookupModeSet",
        &wrCpssExMxTgInlifExpeditedTunnelLookupModeSet,
        3, 0},

        {"cpssExMxTgInlifExpeditedTunnelLookupModeGet",
        &wrCpssExMxTgInlifExpeditedTunnelLookupModeGet,
        2, 0},

        {"cpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable",
        &wrCpssExMxTgInlifIpv4OverMacBcCPUMirrorEnable,
        2, 0},

        {"cpssExMxTgInlifIgmpCommandSet",
        &wrCpssExMxTgInlifIgmpCommandSet,
        2, 0},

        {"cpssExMxTgInlifInit",
        &wrCpssExMxTgInlifInit,
        1, 0},


};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxInlif
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
GT_STATUS cmdLibInitCpssExMxInlifTg
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

