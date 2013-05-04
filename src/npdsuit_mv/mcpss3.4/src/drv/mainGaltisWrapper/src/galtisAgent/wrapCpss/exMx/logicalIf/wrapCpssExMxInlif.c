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
* wrapInlifCpss.c
*
* DESCRIPTION:
*       Wrapper functions for Inlif cpss functions
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
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/logicalIf/cpssExMxGenInlifTypes.h>
#include <cpss/exMx/exMxGen/logicalIf/cpssExMxInlif.h>
#include <cpss/exMx/exMxGen/logicalIf/private/prvCpssExMxInlif.h>


/*************** INLIF TABLE ***********************************/
#define INLIF_NUM_OF_FIELDS 31
/*
typedef struct
{
    GT_U8   port;
    GT_U16   vid;
}CPSS_EXMX_INLIF_WRAP_ST;

static CPSS_EXMX_INLIF_WRAP_ST* entriesSetIndexesArr = NULL;
static GT_U32  inlifCurrNumOfEntries = 0;
*/
static GT_U8  inlifGetCurrPort = 0;
static GT_U16  inlifGetCurrVlanOffset = 0;
#define MAX_NUM_OF_PORTS 256

/*******************************************************************************
* prvCpssExMxConvertHwEntryToInlifFields
*
* DESCRIPTION:
*      This function set block of inlif to the port/vlan inlif table.
*
* APPLICABLE DEVICES: Twist/Samba devices
*
* INPUTS:
*       devNum              -  The device to convert the inlif for
*       fieldTypePtr        -  The enumeration of the fields to be set.
*       fieldValuePtr       -  The values to set
*       numOfFields         -  Number of fields to set
*       inLifEntryPtr       -  The InLif entry to update
*
* OUTPUTS:
*       inLifEntryPtr - the updated inlif entry
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on other error.
*       GT_BAD_PARAM    - on wrong device/port number, or fieldType
*       GT_HW_ERROR     - on hardware error
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssExMxConvertHwEntryToInlifFields
(
    IN      GT_U8                            devNum,
    IN      GT_U32                           *inLifEntry,
    OUT     CPSS_EXMX_INLIF_FIELD_VALUE_UNT  *fieldValue
)
{
    GT_U32                           bitNumber = 0;
    GT_U32                           fieldNumber = 0;
    GT_U32                           partNumber;
    GT_U32                           partOfInlifEntry = 0;
    GT_BOOL                          bitValue;

   /* convert the 128 bit hw inlif entry to sw inlif entry   */
    for (bitNumber = 0; bitNumber < 128; bitNumber++)
    {
        partNumber = bitNumber / 32;
        if(bitNumber % 32 == 0)
            partOfInlifEntry = inLifEntry[partNumber];
        else
            partOfInlifEntry = partOfInlifEntry >> 1;

        bitValue = (GT_BOOL)(partOfInlifEntry && 1);

        switch(bitNumber)
        {

        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 11:
        case 13:
        case 48:
        case 49:
        case 50:
        case 52:
        case 53:
        case 54:
            fieldValue[fieldNumber].boolField = bitValue;
            fieldNumber++;
            break;

        case 7:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_SAMBA_E)
            {
                fieldValue[7].boolField = bitValue;
                fieldValue[10].boolField = bitValue;
            }
            fieldNumber++;
            break;

        case 10:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_TWISTD_E)
            {
                fieldValue[11].boolField = bitValue;
                fieldValue[9].boolField = 2; /*DONT_SET*/
            }
            else
            {
                 fieldValue[9].boolField = bitValue;
                 fieldValue[11].boolField = 2; /*DONT_SET*/
            }
            fieldNumber = 12;
            break;


        case 12:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_TIGER_E)
            {
                fieldValue[11].boolField = bitValue;
                fieldValue[13].boolField = 2; /*DONT_SET*/
                fieldValue[14].boolField = 2; /*DONT_SET*/
            }
            else
            {
                 fieldValue[11].boolField = 2; /*DONT_SET*/
                 fieldValue[13].boolField = bitValue;
                 fieldValue[14].boolField = bitValue;
            }
            fieldNumber = 15;
            break;

        case 14:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_SAMBA_E)
            {
                fieldValue[7].boolField = bitValue;
                fieldValue[10].boolField = bitValue;
            }
            break;

        case 15:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_SAMBA_E)
            {
                fieldValue[11].boolField = bitValue;
            }
            break;

        case 16:
            fieldValue[fieldNumber].u32Field = partOfInlifEntry && 0x1FFF;
            fieldNumber++;
            break;

        case 32:
            fieldValue[fieldNumber].u32Field = (GT_U16)partOfInlifEntry;
            fieldNumber++;
            break;

        case 58:
            fieldValue[fieldNumber].u32Field = partOfInlifEntry && 0x3F;
            fieldNumber++;
            break;

        case 64:
            fieldValue[fieldNumber].ipv4AddrField.u32Ip = partOfInlifEntry;
            fieldNumber++;
            break;

        case 96:
            fieldValue[29].u32Field = partOfInlifEntry && 0x3F;
            fieldValue[26].u32Field = partOfInlifEntry && 0x3FF;
            break;

        case 102:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_SAMBA_E)
            {
                fieldValue[30].u32Field = partOfInlifEntry && 0x3F;
            }
            else
                fieldValue[30].u32Field = 0;

            break;

        case 106:
            fieldValue[27].u32Field = partOfInlifEntry && 0x3FF;
            break;


        case 116:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_SAMBA_E)
            {
                fieldValue[28].u32Field = partOfInlifEntry && 0xF;
            }
            else
                fieldValue[28].u32Field = 0;
            break;

        default:
            break;
        }

    }

    return GT_OK;
}

/*******************************************************************************
* cpssExMxInlifPortEntryGet
*
* DESCRIPTION:
*       This function get the InLIF entry
*
* APPLICABLE DEVICES: All ExMx Devices
*
* INPUTS:
*      devNum        -   pp number of the port set.
*      portNum       -   virtual port number to be set. port is used as index
*                        in the Port-InLif Table
*.
* OUTPUTS:
*       inlifHwData  -  (pointer to)Hw Inlif Entry, Entry data size is 4 words
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
static CMD_STATUS wrCpssExMxInlifEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS result;

    GT_U8    devNum;
    GT_U32   inlifHwData[4];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT  fieldValuePtr[INLIF_NUM_OF_FIELDS];

    CPSS_EXMX_INLIF_PORT_MODE_ENT    inlifMode;
    GT_U32      vlanBaseAdd;
    GT_U16      portInLifMinVid;
    GT_U16      portInLifMaxVid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    devNum = (GT_U8)inArgs[0];

    inlifGetCurrPort = 0;
    inlifGetCurrVlanOffset = 0;

    result = cpssExMxInlifPortLookupModeGet(devNum, inlifGetCurrPort, &inlifMode);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(inlifMode == CPSS_EXMX_MULTIPLE_INLIF_E)
    {
        result = cpssExMxInlifVlanConfigGet(devNum, inlifGetCurrPort,
                                            &vlanBaseAdd, &portInLifMinVid,
                                            &portInLifMaxVid);
        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        result = cpssExMxInlifVlanEntryGet(devNum, inlifGetCurrPort,
                            (GT_U16)(portInLifMinVid + inlifGetCurrVlanOffset),
                            inlifHwData);

        inFields[0] = inlifGetCurrPort;
        inFields[1] = portInLifMinVid;

        inlifGetCurrVlanOffset++;
    }
    else
    {
        result = cpssExMxInlifPortEntryGet(devNum, inlifGetCurrPort,
                                           inlifHwData);

        inFields[0] = inlifGetCurrPort;
        inFields[1] = 0xFFFFFFFF;

        inlifGetCurrPort++;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /*here call the prv reciprocal func(IN inlifHwData, OUT fieldValuePtr)*/
    prvCpssExMxConvertHwEntryToInlifFields(devNum, inlifHwData, fieldValuePtr);

    inFields[2] = fieldValuePtr[0].boolField;
    inFields[3] = fieldValuePtr[1].boolField;
    inFields[4] = fieldValuePtr[2].boolField;
    inFields[5] = fieldValuePtr[3].boolField;
    inFields[6] = fieldValuePtr[4].boolField;
    inFields[7] = fieldValuePtr[5].boolField;
    inFields[8] = fieldValuePtr[6].boolField;
    inFields[9] = fieldValuePtr[7].boolField;
    inFields[10] = fieldValuePtr[8].boolField;
    inFields[11] = fieldValuePtr[9].boolField;
    inFields[12] = fieldValuePtr[10].boolField;
    inFields[13] = fieldValuePtr[11].boolField;
    inFields[14] = fieldValuePtr[12].boolField;
    inFields[15] = fieldValuePtr[13].boolField;
    inFields[16] = fieldValuePtr[14].boolField;
    inFields[17] = fieldValuePtr[15].boolField;
    inFields[18] = 0;
    inFields[19] = fieldValuePtr[16].u32Field;
    inFields[20] = 0;
    inFields[21] = fieldValuePtr[17].u32Field;
    inFields[22] = fieldValuePtr[18].boolField;
    inFields[23] = fieldValuePtr[19].boolField;
    inFields[24] = fieldValuePtr[20].boolField;
    inFields[25] = fieldValuePtr[21].boolField;
    inFields[26] = fieldValuePtr[22].boolField;
    inFields[27] = fieldValuePtr[23].boolField;
    inFields[28] = 0;
    inFields[29] = fieldValuePtr[24].u32Field;
    inFields[30] = 0;
    inFields[31] = fieldValuePtr[25].ipv4AddrField.u32Ip;
    inFields[32] = 0;
    inFields[33] = fieldValuePtr[26].u32Field;
    inFields[34] = 0;
    inFields[35] = fieldValuePtr[27].u32Field;
    inFields[36] = 0;
    inFields[37] = fieldValuePtr[28].u32Field;
    inFields[38] = 0;
    inFields[39] = fieldValuePtr[29].u32Field;
    inFields[40] = 0;
    inFields[41] = fieldValuePtr[30].u32Field;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1],
                inFields[2], inFields[3], inFields[4], inFields[5],
                inFields[6], inFields[7], inFields[8], inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21],
                inFields[22], inFields[23], inFields[24], inFields[25],
                inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32], inFields[33],
                inFields[34], inFields[35], inFields[36], inFields[37],
                inFields[38], inFields[39], inFields[40], inFields[41]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxInlifPortEntryGetNext
*
* DESCRIPTION:
*       This function get the InLIF entry
*
* APPLICABLE DEVICES: All ExMx Devices
*
* INPUTS:
*      devNum        -   pp number of the port set.
*      portNum       -   virtual port number to be set. port is used as index
*                        in the Port-InLif Table
*.
* OUTPUTS:
*       inlifHwData  -  (pointer to)Hw Inlif Entry, Entry data size is 4 words
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
static CMD_STATUS wrCpssExMxInlifEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS result;

    GT_U8    devNum;
    GT_U32   inlifHwData[4];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT  fieldValuePtr[INLIF_NUM_OF_FIELDS];

    CPSS_EXMX_INLIF_PORT_MODE_ENT    inlifMode;
    GT_U32      vlanBaseAdd;
    GT_U16      portInLifMinVid;
    GT_U16      portInLifMaxVid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    devNum = (GT_U8)inArgs[0];

    if(inlifGetCurrPort == (MAX_NUM_OF_PORTS-1))
    {
       galtisOutput(outArgs, GT_FAIL, "%d", -1);
       return CMD_OK;
    }

    result = cpssExMxInlifPortLookupModeGet(devNum, inlifGetCurrPort, &inlifMode);

    if (result != GT_OK ||
       (inlifMode != CPSS_EXMX_MULTIPLE_INLIF_E &&
        inlifMode != CPSS_EXMX_SINGLE_INLIF_E))
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(inlifMode == CPSS_EXMX_MULTIPLE_INLIF_E)
    {
        result = cpssExMxInlifVlanConfigGet(devNum, inlifGetCurrPort,
                                            &vlanBaseAdd, &portInLifMinVid,
                                            &portInLifMaxVid);
        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        if (portInLifMinVid + inlifGetCurrVlanOffset > portInLifMaxVid)
        {
            inlifGetCurrPort++;
            inlifGetCurrVlanOffset = 0;
        }
        else
        {
            result = cpssExMxInlifVlanEntryGet(devNum, inlifGetCurrPort,
                            (GT_U16)(portInLifMinVid + inlifGetCurrVlanOffset),
                            inlifHwData);

            inFields[0] = inlifGetCurrPort;
            inFields[1] = portInLifMinVid + inlifGetCurrVlanOffset;
            inlifGetCurrVlanOffset++;
        }
    }
    if(inlifMode == CPSS_EXMX_SINGLE_INLIF_E || inlifGetCurrVlanOffset == 0)
    {
        result = cpssExMxInlifPortEntryGet(devNum, inlifGetCurrPort,
                                           inlifHwData);

        inFields[0] = inlifGetCurrPort;
        inFields[1] = 0xFFFFFFFF;

        inlifGetCurrPort++;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /*here call the prv reciprocal func(IN inlifHwData, OUT fieldValuePtr)*/
    prvCpssExMxConvertHwEntryToInlifFields(devNum, inlifHwData, fieldValuePtr);

    inFields[2] = fieldValuePtr[0].boolField;
    inFields[3] = fieldValuePtr[1].boolField;
    inFields[4] = fieldValuePtr[2].boolField;
    inFields[5] = fieldValuePtr[3].boolField;
    inFields[6] = fieldValuePtr[4].boolField;
    inFields[7] = fieldValuePtr[5].boolField;
    inFields[8] = fieldValuePtr[6].boolField;
    inFields[9] = fieldValuePtr[7].boolField;
    inFields[10] = fieldValuePtr[8].boolField;
    inFields[11] = fieldValuePtr[9].boolField;
    inFields[12] = fieldValuePtr[10].boolField;
    inFields[13] = fieldValuePtr[11].boolField;
    inFields[14] = fieldValuePtr[12].boolField;
    inFields[15] = fieldValuePtr[13].boolField;
    inFields[16] = fieldValuePtr[14].boolField;
    inFields[17] = fieldValuePtr[15].boolField;
    inFields[18] = 0;
    inFields[19] = fieldValuePtr[16].u32Field;
    inFields[20] = 0;
    inFields[21] = fieldValuePtr[17].u32Field;
    inFields[22] = fieldValuePtr[18].boolField;
    inFields[23] = fieldValuePtr[19].boolField;
    inFields[24] = fieldValuePtr[20].boolField;
    inFields[25] = fieldValuePtr[21].boolField;
    inFields[26] = fieldValuePtr[22].boolField;
    inFields[27] = fieldValuePtr[23].boolField;
    inFields[28] = 0;
    inFields[29] = fieldValuePtr[24].u32Field;
    inFields[30] = 0;
    inFields[31] = fieldValuePtr[25].ipv4AddrField.u32Ip;
    inFields[32] = 0;
    inFields[33] = fieldValuePtr[26].u32Field;
    inFields[34] = 0;
    inFields[35] = fieldValuePtr[27].u32Field;
    inFields[36] = 0;
    inFields[37] = fieldValuePtr[28].u32Field;
    inFields[38] = 0;
    inFields[39] = fieldValuePtr[29].u32Field;
    inFields[40] = 0;
    inFields[41] = fieldValuePtr[30].u32Field;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1],
                inFields[2], inFields[3], inFields[4], inFields[5],
                inFields[6], inFields[7], inFields[8], inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21],
                inFields[22], inFields[23], inFields[24], inFields[25],
                inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32], inFields[33],
                inFields[34], inFields[35], inFields[36], inFields[37],
                inFields[38], inFields[39], inFields[40], inFields[41]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxInlifPortLookupModeSet
*
* DESCRIPTION:
*       The function configured the InLIf Lookup mode for the Ingress port.
*       Each port is independent and can work in every mode.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*
*       devNum      - pp number of the port set.
*       portNum     - port number
*       inlifmode   - InLIF Lookup Mode
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
*       legal values for ExMx devices exclude Ex1x6:
*       0 = The physical port is to be considered a single InLIF
*       1 = This physical port is partitioned into more than a single InLIF,
*           and it uses the VLAN-Id to assign the InLIF number.
*
*       legal values for Ex1x6 devices
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
static CMD_STATUS wrCpssExMxInlifPortLookupModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    GT_U8                           portNum;
    CPSS_EXMX_INLIF_PORT_MODE_ENT   inlifMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    inlifMode = (CPSS_EXMX_INLIF_PORT_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxInlifPortLookupModeSet(devNum, portNum, inlifMode);


      /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxInlifPortLookupModeGet
*
* DESCRIPTION:
*       The function returns the InLIf Lookup mode for the Ingress port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*
*       devNum         - pp number of the port set.
*       portNum        - port number
*
* OUTPUTS:
*       inlifModePtr   - InLIF Lookup Mode
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on other error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_PARAM    - on bad parameters
*
* COMMENTS:
*       legal values for ExMx devices exclude Ex1x6:
*       0 = The physical port is to be considered a single InLIF
*       1 = This physical port is partitioned into more than a single InLIF,
*           and it uses the VLAN-Id to assign the InLIF number.
*
*       legal values for Ex1x6 devices
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
static CMD_STATUS wrCpssExMxInlifPortLookupModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    GT_U8                           portNum;
    CPSS_EXMX_INLIF_PORT_MODE_ENT   inlifMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxInlifPortLookupModeGet(devNum, portNum, &inlifMode);


      /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", inlifMode);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxInlifFieldsSet
*
* DESCRIPTION:
*       This function sets number of fields in single or multiple InLIF entry
*
* APPLICABLE DEVICES: Twist/Samba devices
*
* INPUTS:
*      devNum        -   pp number of the port set.
*      portNum       -   virtual port number to be set. port is used as index
*                        in the Port-InLif Table
*      vid           -   if multi inlif , vid is used to index by Inlif Table.
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
static CMD_STATUS wrCpssExMxInlifFieldsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

/*    GT_BOOL                         allreadyExists = 0;*/
    GT_U8                           devNum;
    GT_U8                           port;
    GT_U16                          vid;
    GT_U32                          numOfFields = 0;
/*    GT_U32                          i;*/

    CPSS_EXMX_INLIF_FIELD_TYPE_ENT   fieldTypePtr[INLIF_NUM_OF_FIELDS];
    CPSS_EXMX_INLIF_FIELD_VALUE_UNT  fieldValuePtr[INLIF_NUM_OF_FIELDS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    port = (GT_U8)inFields[0];
    vid = (GT_U16)inFields[1];

    if(inFields[2] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DIS_BRG_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[2];
        numOfFields++;
    }
    if(inFields[3] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_MPLS_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[3];
        numOfFields++;
    }
    if(inFields[4] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_MPLS_MC_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[4];
        numOfFields++;
    }
    if(inFields[5] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV4_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[5];
        numOfFields++;
    }
    if(inFields[6] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV4_MC_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[6];
        numOfFields++;
    }
    if(inFields[7] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV6_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[7];
        numOfFields++;
    }
    if(inFields[8] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_IPV6_MC_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[8];
        numOfFields++;
    }
    if(inFields[9] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_CLASS_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[9];
        numOfFields++;
    }
    if(inFields[10] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_FT_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[10];
        numOfFields++;
    }
    if(inFields[11] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_FORCE_CLASS_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[11];
        numOfFields++;
    }
    if(inFields[12] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_PCL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[12];
        numOfFields++;
    }
    if(inFields[13] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_FORCE_PCL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[13];
        numOfFields++;
    }
    if(inFields[14] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_EPI_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[14];
        numOfFields++;
    }
    if(inFields[15] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_MIRROR_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[15];
        numOfFields++;
    }
    if(inFields[16] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_FORCE_EPI_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[16];
        numOfFields++;
    }
    if(inFields[17] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_EN_DO_NAT_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[17];
        numOfFields++;
    }
    if(inFields[18])
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_VRID_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[19];
        numOfFields++;
    }
    if(inFields[20])
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_NUM_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[21];
        numOfFields++;
    }
    if(inFields[22] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PREV_SIP_SPOOF_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[22];
        numOfFields++;
    }
    if(inFields[23] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_RES_SIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[23];
        numOfFields++;
    }
    if(inFields[24] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_INTER_SIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[24];
        numOfFields++;
    }
    if(inFields[25] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_MC_LOCAL_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[25];
        numOfFields++;
    }
    if(inFields[26] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_RES_DIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[26];
        numOfFields++;
    }
    if(inFields[27] < 2)
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_DROP_INTER_DIP_E;
        fieldValuePtr[numOfFields].boolField = (GT_BOOL)inFields[27];
        numOfFields++;
    }
    if(inFields[28])
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_SRC_IP_SUB_LEN_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[29];
        numOfFields++;
    }
    if(inFields[30])
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_SRC_IP_E;
        fieldValuePtr[numOfFields].ipv4AddrField.u32Ip = (GT_U32)inFields[31];
        numOfFields++;
    }
    if(inFields[32])
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_BASE_ADDR_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[33];
        numOfFields++;
    }
    if(inFields[34])
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_MAX_HOP_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[35];
        numOfFields++;
    }
    if(inFields[36])
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_NUMBER_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[37];
        numOfFields++;
    }
    if(inFields[38])
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_NUMBER_SLOT0_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[39];
        numOfFields++;
    }
    if(inFields[40])
    {
        fieldTypePtr[numOfFields] = CPSS_EXMX_INLIF_PCL_NUMBER_SLOT1_E;
        fieldValuePtr[numOfFields].u32Field = (GT_U32)inFields[41];
        numOfFields++;
    }

    /* call cpss api function */
    result = cpssExMxInlifFieldsSet(devNum,port,vid,fieldTypePtr,
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

/*******************************************************************************
* cpssExMxInlifVlanConfigSet
*
* DESCRIPTION:
*       If the port is not a single Inlif this function set the base address of
*       the InLIF table found in the external table and the vlan Inlif range

* APPLICABLE DEVICES:  Twist/Samba devices
*
* INPUTS:
*
*       devNum              - pp number of the port set.
*       portNum             - port number
*       vlanBaseAdd         - the vlan base address
*       portInLifMinVid     - minimum InLIF VID (inclusive)
*                           for a port working in multiple mode
*       portInLifMaxVid     - maximum InLIF VID (inclusive)
*                           for a port working in multiple mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on other error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on bad parameters
*       GT_NOT_SUPPORTED - type isn't supported for the device
*
* COMMENTS:
*       This function can be used only when the inlif mode is multiple.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxInlifVlanConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_U8       portNum;
    GT_U32      vlanBaseAdd;
    GT_U16      portInLifMinVid;
    GT_U16      portInLifMaxVid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    vlanBaseAdd = (GT_U32)inArgs[2];
    portInLifMinVid = (GT_U16)inArgs[3];
    portInLifMaxVid = (GT_U16)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxInlifVlanConfigSet(devNum, portNum, vlanBaseAdd,
                                        portInLifMinVid, portInLifMaxVid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxInlifVlanConfigGet
*
* DESCRIPTION:
*       If the port is not a single Inlif this function get the base address of
*       the InLIF table found in the external table and the vlan Inlif range
*
* APPLICABLE DEVICES:  Twist/Samba devices
*
* INPUTS:
*
*       devNum      - pp number of the port set.
*       portNum     - port number
*
* OUTPUTS:
*       vlanBaseAddPtr          - (pointer to) the vlan base address
*       portInLifMinVidPtr     - (pointer to) minimum InLIF VID (inclusive)
*                               for a port working in multiple mode
*       portInLifMaxVidPtr     - (pointer to) maximum InLIF VID (inclusive)
*                               for a port working in multiple mode
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on other error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_PARAM    - on bad parameters
*
* COMMENTS:
*       This function can be used only when the inlif mode is multiple.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxInlifVlanConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_U8       portNum;
    GT_U32      vlanBaseAdd;
    GT_U16      portInLifMinVid;
    GT_U16      portInLifMaxVid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxInlifVlanConfigGet(devNum, portNum, &vlanBaseAdd,
                                        &portInLifMinVid, &portInLifMaxVid);


      /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", vlanBaseAdd, portInLifMinVid,
                                            portInLifMaxVid);
    return CMD_OK;
}






/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssExMxInlifGetFirst",
        &wrCpssExMxInlifEntryGetFirst,
        1, 0},

    {"cpssExMxInlifGetNext",
        &wrCpssExMxInlifEntryGetNext,
        1, 0},

    {"cpssExMxInlifSet",
        &wrCpssExMxInlifFieldsSet,
        1, 42},

    {"cpssExMxInlifPortLookupMode_Set",
        &wrCpssExMxInlifPortLookupModeSet,
        3, 0},

    {"cpssExMxInlifPortLookupModeGet",
        &wrCpssExMxInlifPortLookupModeGet,
        2, 0},

    {"cpssExMxInlifVlanConfigSet",
        &wrCpssExMxInlifVlanConfigSet,
        5, 0},

    {"cpssExMxInlifVlanConfigGet",
        &wrCpssExMxInlifVlanConfigGet,
        2, 0}
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
GT_STATUS cmdLibInitCpssExMxInlif
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

