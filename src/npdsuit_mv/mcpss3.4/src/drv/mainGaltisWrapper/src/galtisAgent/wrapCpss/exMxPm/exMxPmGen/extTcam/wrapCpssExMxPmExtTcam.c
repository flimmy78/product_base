/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmExtTcam.c
*
* DESCRIPTION:
*       wrappers for cpssExMxPmExtTcam.c
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
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/exMxPm/exMxPmGen/extTcam/cpssExMxPmExternalTcam.h>

/*******************************************************************************
* cpssExMxPmExtTcamRegSet
*
* DESCRIPTION:
*   The function sets register of External TCAM device
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       regAddr         - register address.
*       tcamReg         - the register value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamRegSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tcamId;
    GT_U32 regAddr;
    CPSS_EXMXPM_EXTERNAL_TCAM_REG tcamReg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    tcamId=(GT_U32)inArgs[1];
    regAddr=(GT_U32)inArgs[2];
    tcamReg[0]=(GT_U32)inArgs[3];
    tcamReg[1]=(GT_U32)inArgs[4];
    tcamReg[2]=(GT_U32)inArgs[5];


    /* call cpss api function */
    result = cpssExMxPmExternalTcamRegSet(devNum, tcamId, regAddr, tcamReg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmExternalTcamRegGet
*
* DESCRIPTION:
*   The function gets register of External TCAM device
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       regAddr         - register address.
*
* OUTPUTS:
*       tcamReg         - the register value
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamRegGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tcamId;
    GT_U32 regAddr;
    CPSS_EXMXPM_EXTERNAL_TCAM_REG tcamReg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    tcamId=(GT_U32)inArgs[1];
    regAddr=(GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmExternalTcamRegGet(devNum, tcamId, regAddr, tcamReg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", tcamReg[0], tcamReg[1], tcamReg[2]);

    return CMD_OK;
}

/* --- cpssExMxPmExternalTcamRule Table--- */

/* tables cpssExMxPmExternalTcamRule global variables */

static    GT_U32      maskData[20];
static    GT_U32      patternData[20];
static    GT_BOOL     patternValid;

static    GT_BOOL                       mask_set = GT_FALSE; /* is mask set*/
static    GT_BOOL                       pattern_set = GT_FALSE; /* is pattern set*/
static    GT_U32                        mask_ruleIndex = 0;
static    GT_U32                        pattern_ruleIndex = 0;

/*******************************************************************************
* cpssExMxPmExternalTcamRuleSet
*
* DESCRIPTION:
*   The function sets rule on External TCAM device
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       segmentNum      - Number of segment 0..31 in TCAM memory.
*       ruleSize        - size of rule entry (40,80,160,320 or 640 bits)
*       ruleIndex       - index of rule in the segment (assumed that all
*                         rules in the segment have the specified size)
*       dataSize        - number of meaningfull bits in
*                         the patternOrXArr and maskOrYArr.
*                         These bits is the LSB of the rule (if it is greater)
*                         All rule bits out of "dataSize" will be zeros.
*                         Both in pattern/mask and X/Y formats it is "don't Care"
*       valid             GT_TRUE - valid rule, GT_FALSE - invalid
*       patternOrXArr   - array of 32-bit words that contains the pattern
*                         or X-data, word0 - bits0-31, word1 - bits32-63, ...
*       maskOrYArr      - array of 32-bit words that contains the mask
*                         or Y-data, word0 - bits0-31, word1 - bits32-63, ...
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamRuleSetFirst
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

    cmdOsMemSet(&maskData , 0, sizeof(GT_U32)*20);
    cmdOsMemSet(&patternData, 0, sizeof(GT_U32)*20);

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;
    patternValid= GT_FALSE;

    if(inFields[1] == 0) /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];

        maskData[0]= (GT_U32)inFields[3];
        maskData[1]= (GT_U32)inFields[4];
        maskData[2]= (GT_U32)inFields[5];
        maskData[3]= (GT_U32)inFields[6];
        maskData[4]= (GT_U32)inFields[7];
        maskData[5]= (GT_U32)inFields[8];
        maskData[6]= (GT_U32)inFields[9];
        maskData[7]= (GT_U32)inFields[10];
        maskData[8]= (GT_U32)inFields[11];
        maskData[9]= (GT_U32)inFields[12];
        maskData[10]= (GT_U32)inFields[13];
        maskData[11]= (GT_U32)inFields[14];
        maskData[12]= (GT_U32)inFields[15];
        maskData[13]= (GT_U32)inFields[16];
        maskData[14]= (GT_U32)inFields[17];
        maskData[15]= (GT_U32)inFields[18];
        maskData[16]= (GT_U32)inFields[19];
        maskData[17]= (GT_U32)inFields[20];
        maskData[18]= (GT_U32)inFields[21];
        maskData[19]= (GT_U32)inFields[22];


        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternValid  = (GT_BOOL)inFields[2];
        patternData[0]= (GT_U32)inFields[3];
        patternData[1]= (GT_U32)inFields[4];
        patternData[2]= (GT_U32)inFields[5];
        patternData[3]= (GT_U32)inFields[6];
        patternData[4]= (GT_U32)inFields[7];
        patternData[5]= (GT_U32)inFields[8];
        patternData[6]= (GT_U32)inFields[9];
        patternData[7]= (GT_U32)inFields[10];
        patternData[8]= (GT_U32)inFields[11];
        patternData[9]= (GT_U32)inFields[12];
        patternData[10]= (GT_U32)inFields[13];
        patternData[11]= (GT_U32)inFields[14];
        patternData[12]= (GT_U32)inFields[15];
        patternData[13]= (GT_U32)inFields[16];
        patternData[14]= (GT_U32)inFields[17];
        patternData[15]= (GT_U32)inFields[18];
        patternData[16]= (GT_U32)inFields[19];
        patternData[17]= (GT_U32)inFields[20];
        patternData[18]= (GT_U32)inFields[21];
        patternData[19]= (GT_U32)inFields[22];


        pattern_set = GT_TRUE;
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/***********************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamRuleSetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U8 devNum;
    GT_U32 tcamId;
    GT_U32 segmentNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT ruleSize;
    GT_U32 ruleIndex;
    GT_U32 dataSize;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;
    devNum=(GT_U8)inArgs[0];
    tcamId=(GT_U32)inArgs[1];
    segmentNum=(GT_U32)inArgs[2];
    ruleSize=(CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT)inArgs[3];
    dataSize=(GT_U32)inArgs[4];

    if(inFields[1] > 1)
 {
     /* pack output arguments to galtis string */
     galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
     return CMD_AGENT_ERROR;
 }


 if(inFields[1] == 0 && mask_set)
 {
     /* pack output arguments to galtis string */
     galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of Ingress {Rule,Mask}.\n");
     return CMD_AGENT_ERROR;
 }

 if(inFields[1] == 1 && pattern_set)
 {
     /* pack output arguments to galtis string */
     galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of Ingress {Rule,Mask}.\n");
     return CMD_AGENT_ERROR;
 }

  /*
       inFields[1] = mask/pattern indication
       mask = 0
       pattern = 1
    */

   if(inFields[1] == 0) /* mask */
    {
        mask_ruleIndex = (GT_U32)inFields[0];

        maskData[0]= (GT_U32)inFields[3];
        maskData[1]= (GT_U32)inFields[4];
        maskData[2]= (GT_U32)inFields[5];
        maskData[3]= (GT_U32)inFields[6];
        maskData[4]= (GT_U32)inFields[7];
        maskData[5]= (GT_U32)inFields[8];
        maskData[6]= (GT_U32)inFields[9];
        maskData[7]= (GT_U32)inFields[10];
        maskData[8]= (GT_U32)inFields[11];
        maskData[9]= (GT_U32)inFields[12];
        maskData[10]= (GT_U32)inFields[13];
        maskData[11]= (GT_U32)inFields[14];
        maskData[12]= (GT_U32)inFields[15];
        maskData[13]= (GT_U32)inFields[16];
        maskData[14]= (GT_U32)inFields[17];
        maskData[15]= (GT_U32)inFields[18];
        maskData[16]= (GT_U32)inFields[19];
        maskData[17]= (GT_U32)inFields[20];
        maskData[18]= (GT_U32)inFields[21];
        maskData[19]= (GT_U32)inFields[22];
        ruleIndex=mask_ruleIndex;

        mask_set = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_ruleIndex = (GT_U32)inFields[0];
        patternValid =  (GT_BOOL)inFields[2];
        patternData[0]= (GT_U32)inFields[3];
        patternData[1]= (GT_U32)inFields[4];
        patternData[2]= (GT_U32)inFields[5];
        patternData[3]= (GT_U32)inFields[6];
        patternData[4]= (GT_U32)inFields[7];
        patternData[5]= (GT_U32)inFields[8];
        patternData[6]= (GT_U32)inFields[9];
        patternData[7]= (GT_U32)inFields[10];
        patternData[8]= (GT_U32)inFields[11];
        patternData[9]= (GT_U32)inFields[12];
        patternData[10]= (GT_U32)inFields[13];
        patternData[11]= (GT_U32)inFields[14];
        patternData[12]= (GT_U32)inFields[15];
        patternData[13]= (GT_U32)inFields[16];
        patternData[14]= (GT_U32)inFields[17];
        patternData[15]= (GT_U32)inFields[18];
        patternData[16]= (GT_U32)inFields[19];
        patternData[17]= (GT_U32)inFields[20];
        patternData[18]= (GT_U32)inFields[21];
        patternData[19]= (GT_U32)inFields[22];

        ruleIndex=pattern_ruleIndex;

        pattern_set = GT_TRUE;
    }

    if(mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }
    if(mask_set && pattern_set){

    /* call cpss api function */
    result = cpssExMxPmExternalTcamRuleWrite(devNum, tcamId, ruleIndex,
                                             ruleSize, dataSize, patternValid,
                                             GT_FALSE, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_AND_LOGIC_E,
                                             patternData, maskData);
    mask_set = GT_FALSE;
    pattern_set = GT_FALSE;

    cmdOsMemSet(&maskData , 0,  sizeof(GT_U32)*20);
    cmdOsMemSet(&patternData, 0,  sizeof(GT_U32)*20);
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/********************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamRuleEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(mask_set || pattern_set)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmExternalTcamRuleGet
*
* DESCRIPTION:
*   The function gets rule on External TCAM device
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       segmentNum      - Number of segment 0..31 in TCAM memory.
*       ruleSize        - size of rule entry (40,80,160,320 or 640 bits)
*       ruleIndex       - index of rule in the segment (assumed that all
*                         rules in the segment have the specified size)
*       dataSize        - number of meaningfull bits in
*                         the patternOrXArr and maskOrYArr
*       ruleFormat      - if specified data/mask format - converted
*                         x/y => data/mask, if x/y format - returned as is
*
* OUTPUTS:
*       validPtr          GT_TRUE - valid rule, GT_FALSE - invalid
*       patternOrXArr   - array of 32-bit words that contains the pattern
*                         or X-data, word0 - bits0-31, word1 - bits32-63, ...
*       maskOrYArr      - array of 32-bit words that contains the mask
*                         or Y-data, word0 - bits0-31, word1 - bits32-63, ...
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/

static GT_U32 ruleIndex;
static GT_BOOL isMask;

static CMD_STATUS wrCpssExMxPmExtTcamRuleGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U8 devNum;
    GT_U32 tcamId;
    GT_U32 segmentNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT ruleSize;

    GT_U32 dataSize;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT ruleFormat;
    GT_BOOL valid, age;

    /* map input arguments to locals */
    devNum=(GT_U8)inArgs[0];
    tcamId=(GT_U32)inArgs[1];
    segmentNum=(GT_U32)inArgs[2];
    ruleSize=(CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT)inArgs[3];
    dataSize=(GT_U32)inArgs[4];
    ruleFormat=(CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT)inArgs[5];

    do
    {
        if (ruleIndex > 1024)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_OK, "%d", -1);

            return CMD_OK;
        }

        /* call cpss api function */
        result = cpssExMxPmExternalTcamRuleRead(devNum, tcamId, ruleIndex, ruleSize, dataSize, ruleFormat, &valid, &age, patternData, maskData);
 #if defined (ASIC_SIMULATION)
 /* the simulation always returns FALSE indication */
        valid = GT_TRUE;
 #else
        if( valid == GT_FALSE)
        {
            ruleIndex+=ruleSize;
        }
 #endif
    }
    while (valid == GT_FALSE);

    if(isMask) /* mask */
    {
        inFields[0]=ruleIndex;
        inFields[1]=0;
        inFields[2]=valid;
        inFields[3]=maskData[0];
        inFields[4]=maskData[1];
        inFields[5]=maskData[2];
        inFields[6]=maskData[3];
        inFields[7]=maskData[4];
        inFields[8]=maskData[5];
        inFields[9]=maskData[6];
        inFields[10]=maskData[7];
        inFields[11]=maskData[8];
        inFields[12]=maskData[9];
        inFields[13]=maskData[10];
        inFields[14]=maskData[11];
        inFields[15]=maskData[12];
        inFields[16]=maskData[13];
        inFields[17]=maskData[14];
        inFields[28]=maskData[15];
        inFields[19]=maskData[16];
        inFields[20]=maskData[17];
        inFields[21]=maskData[18];
        inFields[22]=maskData[19];

        isMask = GT_FALSE;
        ruleIndex+=ruleSize;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2], inFields[3],
                    inFields[4], inFields[5], inFields[6],
                    inFields[7], inFields[8], inFields[9],
                    inFields[10], inFields[11], inFields[12],
                    inFields[13], inFields[14],  inFields[15], inFields[16],
                    inFields[17], inFields[18], inFields[19],
                    inFields[20], inFields[21], inFields[22]  );

    }
    else /* pattern */
    {
        inFields[0]=ruleIndex;
        inFields[1]=1;
        inFields[2]=valid;
        inFields[3]=patternData[0];
        inFields[4]=patternData[1];
        inFields[5]=patternData[2];
        inFields[6]=patternData[3];
        inFields[7]=patternData[4];
        inFields[8]=patternData[5];
        inFields[9]=patternData[6];
        inFields[10]=patternData[7];
        inFields[11]=patternData[8];
        inFields[12]=patternData[9];
        inFields[13]=patternData[10];
        inFields[14]=patternData[11];
        inFields[15]=patternData[12];
        inFields[16]=patternData[13];
        inFields[17]=patternData[14];
        inFields[18]=patternData[15];
        inFields[19]=patternData[16];
        inFields[20]=patternData[17];
        inFields[21]=patternData[18];
        inFields[22]=patternData[19];

        isMask = GT_TRUE;

         fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2], inFields[3],
                    inFields[4], inFields[5], inFields[6],
                    inFields[7], inFields[8], inFields[9],
                    inFields[10], inFields[11], inFields[12],
                    inFields[13], inFields[14],  inFields[15], inFields[16],
                    inFields[17], inFields[18], inFields[19],
                    inFields[20], inFields[21], inFields[22]);

    }



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*********************************************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamRuleGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ruleIndex = 0;

    /* first to get is pattern */
    isMask = GT_FALSE;

    return wrCpssExMxPmExtTcamRuleGet(inArgs,inFields,numFields,outArgs);
}
/**********Table:cpssExMxPmExternalTcamHitIndexConvert*******************************/


static GT_U32 gInd;

/*******************************************************************************
* cpssExMxPmExternalTcamHitIndexConvertSet
*
* DESCRIPTION:
*   The function sets "hit index to action index" conversion parameters.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       lookupNum        - Number of lookup 0..1 in TCAM Search Scheme.
*                          TCAM Search Scheme contains 4 lookups, but
*                          ExMxPm devices version1 support only #0 and #1
*       clientEntryIndex - the index of client entry 0..15 that configured
*                          the request
*       indexConvertPtr - index conversion parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamHitIndexConvertSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    tcamId;
    GT_U32    lookupNum;
    GT_U32    clientEntryIndex;

    CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC indexConvert;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum=(GT_U8)inArgs[0];
    tcamId=(GT_U32)inArgs[1];

    lookupNum=(GT_U32)inFields[0];
    clientEntryIndex=(GT_U32)inFields[1];

    indexConvert.shiftOperation=(CPSS_SHIFT_OPERATION_ENT)inFields[2];
    indexConvert.shiftValue=(GT_U32)inFields[3];
    indexConvert.baseAddrOperation=(CPSS_ADJUST_OPERATION_ENT)inFields[4];
    indexConvert.baseAddrOffset=(GT_U32)inFields[5];

    /* call cpss api function */

#if 0
    result = cpssExMxPmExternalTcamHitIndexConvertSet(devNum, tcamId, lookupNum, clientEntryIndex, &indexConvert);
#endif
   result = GT_OK;
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmExternalTcamHitIndexConvertGet
*
* DESCRIPTION:
*   The function gets "hit index to action index" conversion parameters.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum           - device number
*       tcamId           - cascaded TCAM Id 0..3
*       lookupNum        - Number of lookup 0..1 in TCAM Search Scheme.
*                          TCAM Search Scheme contains 4 lookups, but
*                          ExMxPm devices version1 support only #0 and #1
*       clientEntryIndex - ihe index of client entry 0..15 that configured
*                          the request
*
* OUTPUTS:
*       indexConvertPtr  - index conversion parameters
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamHitIndexConvertGetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    GT_U32  tcamId;
    GT_U32  lookupNum;
    GT_U32  clientEntryIndex;

    CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC indexConvert;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    tcamId=(GT_U32)inArgs[1];

    lookupNum = gInd / 16;
    clientEntryIndex = gInd % 16;;
    /* call cpss api function */
#if 0
    result = cpssExMxPmExternalTcamHitIndexConvertGet(devNum, tcamId, lookupNum, clientEntryIndex, &indexConvert);
#else
    /* fix warnings */
    result = GT_OK;
    cmdOsMemSet(&indexConvert,0, sizeof(indexConvert));

#endif
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if (gInd>31)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]=lookupNum;
    inFields[1]=clientEntryIndex;
    inFields[2]=indexConvert.shiftOperation;
    inFields[3]=indexConvert.shiftValue;
    inFields[4]=indexConvert.baseAddrOperation;
    inFields[5]=indexConvert.baseAddrOffset;

    fieldOutput("%d%d%d%d%d%d", inFields[0],
                    inFields[1],  inFields[2],inFields[3],  inFields[4],  inFields[5]);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmExternalTcamHitIndexConvertGet
*
* DESCRIPTION:
*   The function gets "hit index to action index" conversion parameters.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum           - device number
*       tcamId           - cascaded TCAM Id 0..3
*       segmentNum       - Number of segment 0..31 in TCAM memory.
*
* OUTPUTS:
*       indexConvertPtr  - index conversion parameters
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamHitIndexConvertGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    gInd=0;
    return wrCpssExMxPmExtTcamHitIndexConvertGetEntry(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxPmExternalTcamHitIndexConvertGet
*
* DESCRIPTION:
*   The function gets "hit index to action index" conversion parameters.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum           - device number
*       tcamId           - cascaded TCAM Id 0..3
*       segmentNum       - Number of segment 0..31 in TCAM memory.
*
* OUTPUTS:
*       indexConvertPtr  - index conversion parameters
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamHitIndexConvertGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gInd++;
    return wrCpssExMxPmExtTcamHitIndexConvertGetEntry(inArgs,inFields,numFields,outArgs);


}
/***********Table:cpssExMxPmExternalTcamClientConfig***********************************/

static GT_U32 gTcamClientInd;
/*******************************************************************************
* cpssExMxPmExternalTcamClientConfigSet
*
* DESCRIPTION:
*   The function sets EXMXPM device Ext TCAM unit client
*   configuratrion table entry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamClient      - EXMXPM device Ext TCAM unit client,
*                         use CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_0_1_2_E
*                         for EXMXPM device ver1
*       configIndex     - index of configuration  0..15.
*       addrConvertPtr  - address conversion parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamClientConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT tcamClient;
    GT_U32 configIndex;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC clientConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

     devNum=(GT_U8)inArgs[0];
     tcamClient=(CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT)inArgs[1];
     configIndex=(GT_U32)inFields[0];

     clientConfig.tcamDeviceSelectBmp=(GT_U32)inFields[1];
     clientConfig.globalMask=(GT_U32)inFields[2];
     clientConfig.lookupSchemeIndex=(GT_U32)inFields[3];
    /* call cpss api function */
#if 0
    result = cpssExMxPmExternalTcamClientConfigSet(devNum, tcamClient, configIndex, &clientConfig);
#endif
   result = GT_OK;
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmExternalTcamClientConfigGet
*
* DESCRIPTION:
*   The function gets EXMXPM device Ext TCAM unit
*   client configuratrion table entry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamClient      - EXMXPM device Ext TCAM unit client
*                         don't use CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_0_1_2_E
*       configIndex     - index of configuration  0..15.
*
* OUTPUTS:
*       addrConvertPtr  - address conversion parameters
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamClientConfigGetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT tcamClient;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC clientConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    tcamClient=(CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT)inArgs[1];

    /* call cpss api function */
#if 0
    result = cpssExMxPmExternalTcamClientConfigGet(devNum, tcamClient, gTcamClientInd, &clientConfig);
#else
    result = GT_OK;
    cmdOsMemSet(&clientConfig, 0, sizeof(clientConfig));
#endif

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if (gInd>15)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

     inFields[0]=gTcamClientInd;
     inFields[1]= clientConfig.tcamDeviceSelectBmp;
     inFields[2]=clientConfig.globalMask;
     inFields[3]=clientConfig.lookupSchemeIndex;

     fieldOutput("%d%d%d%d", inFields[0],
                    inFields[1],  inFields[2],inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmExternalTcamClientConfigGet
*
* DESCRIPTION:
*   The function gets EXMXPM device Ext TCAM unit
*   client configuratrion table entry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamClient      - EXMXPM device Ext TCAM unit client
*                         don't use CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_0_1_2_E
*       configIndex     - index of configuration  0..15.
*
* OUTPUTS:
*       addrConvertPtr  - address conversion parameters
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamClientConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gTcamClientInd=0;
    return wrCpssExMxPmExtTcamClientConfigGetEntry(inArgs,inFields,numFields,outArgs);

}
/*******************************************************************************
* cpssExMxPmExternalTcamClientConfigGet
*
* DESCRIPTION:
*   The function gets EXMXPM device Ext TCAM unit
*   client configuratrion table entry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamClient      - EXMXPM device Ext TCAM unit client
*                         don't use CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_0_1_2_E
*       configIndex     - index of configuration  0..15.
*
* OUTPUTS:
*       addrConvertPtr  - address conversion parameters
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExtTcamClientConfigGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gTcamClientInd++;
    return wrCpssExMxPmExtTcamClientConfigGetEntry(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxPmExternalTcamClientConfigSet
*
* DESCRIPTION:
*   The function sets external TCAM unit client configuration table entry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamClient      - EXMXPM device External TCAM unit client,
*       clientKeyType   - The selected key type enumeration per client:
*                         please refer to Functional Spec for key types number.
*       lookupType      - Type of lookup parallel searches:
*                         single\dual.
*       clientConfigPtr - search scheme parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamClientConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT tcamClient;
    GT_U32 clientKeyType;
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT lookupType;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC clientConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    tcamClient=(CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT)inArgs[1];
    clientKeyType=(GT_U32)inArgs[2];
    lookupType=(CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT)inArgs[3];
    clientConfig.tcamDeviceSelectBmp=(GT_U32)inArgs[4];
    clientConfig.globalMask=(GT_U32)inArgs[5];
    clientConfig.lookupSchemeIndex=(GT_U32)inArgs[6];

    /* call cpss api function */
    result = cpssExMxPmExternalTcamClientConfigSet(devNum, tcamClient, clientKeyType, lookupType, &clientConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmExternalTcamClientConfigGet
*
* DESCRIPTION:
*   The function gets EXMXPM device External TCAM unit
*   client configuration table entry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamClient      - EXMXPM device External TCAM unit client,
*       clientKeyType   - The selected key type enumeration per client:
*                         please refer to Functional Spec for key types number.
*       lookupType      - Type of lookup parallel searches:
*                         single\dual
*
* OUTPUTS:
*       clientConfigPtr - search scheme parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamClientConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT tcamClient;
    GT_U32 clientKeyType;
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT lookupType;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC clientConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    tcamClient=(CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT)inArgs[1];
    clientKeyType=(GT_U32)inArgs[2];
    lookupType=(CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT)inArgs[3];


    /* call cpss api function */
    result = cpssExMxPmExternalTcamClientConfigGet(devNum, tcamClient, clientKeyType, lookupType, &clientConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",clientConfig.tcamDeviceSelectBmp,clientConfig.globalMask,clientConfig.lookupSchemeIndex);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmExternalTcamHitIndexConvertSet
*
* DESCRIPTION:
*   The function sets "hit index to action index" conversion parameters.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum           - device number
*       tcamClient       - EXMXPM device External TCAM unit client.
*       clientKeyType    - The selected key type enumeration per client:
*                          please refer to FS for key types number.
*       lookupType       - Type of lookup parallel searches:
*                          single\dual.
*       lookupIndex      - The number of lookup: 0..up to lookup type.
*       indexConvertPtr  - index conversion parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamHitIndexConvertSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT tcamClient;
    GT_U32 clientKeyType;
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT lookupType;
    GT_U32 lookupIndex;
    CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC indexConvert;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    tcamClient=(CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT)inArgs[1];
    clientKeyType=(GT_U32)inArgs[2];
    lookupType=(CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT)inArgs[3];
    lookupIndex=(GT_U32)inArgs[4];
    indexConvert.shiftOperation=(CPSS_SHIFT_OPERATION_ENT)inArgs[5];
    indexConvert.shiftValue=(GT_U32)inArgs[6];
    indexConvert.baseAddrOperation=(CPSS_ADJUST_OPERATION_ENT)inArgs[7];
    indexConvert.baseAddrOffset=(GT_U32)inArgs[8];

    /* call cpss api function */
    result = cpssExMxPmExternalTcamHitIndexConvertSet(devNum, tcamClient, clientKeyType, lookupType, lookupIndex, &indexConvert);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmExternalTcamHitIndexConvertGet
*
* DESCRIPTION:
*   The function gets "hit index to action index" conversion parameters.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum           - device number
*       tcamClient       - EXMXPM device External TCAM unit client.
*       clientKeyType    - The selected key type enumeration per client:
*                          please refer to FS for key types number.
*       lookupType      - Type of lookup parallel searches:
*                         single\dual.
*       lookupIndex     - The number of lookup: 0..up to lookup type.
*
* OUTPUTS:
*       indexConvertPtr  - index conversion parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamHitIndexConvertGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT tcamClient;
    GT_U32 clientKeyType;
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT lookupType;
    GT_U32 lookupIndex;
    CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC indexConvert;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    tcamClient=(CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT)inArgs[1];
    clientKeyType=(GT_U32)inArgs[2];
    lookupType=(CPSS_EXMXPM_EXTERNAL_TCAM_LOOKUP_TYPE_ENT)inArgs[3];
    lookupIndex=(GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmExternalTcamHitIndexConvertGet(devNum, tcamClient, clientKeyType, lookupType, lookupIndex, &indexConvert);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",indexConvert.shiftOperation,indexConvert.shiftValue,indexConvert.baseAddrOperation,indexConvert.baseAddrOffset);

    return CMD_OK;
}
/*************************************************************************
* cpssExMxPmExternalTcamGlobalWriteModeSet
*
* DESCRIPTION:
*       The function sets the global write mode - ONLY in the CPSS DB -
*       per PP device and TCAM number (supported only for IDT).
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       ruleFormat      - X\Y or DATA\MASK rule format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_SUPPORTED         - not supported for NL TCAM.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       An aplication must use this function to configure the CPSS DB before
*       the first call rule read or write APIs.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamGlobalWriteModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tcamId;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT ruleFormat;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tcamId = (GT_U32)inArgs[1];
    ruleFormat = (CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmExternalTcamGlobalWriteModeSet(devNum, tcamId, ruleFormat);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmExternalTcamGlobalWriteModeGet
*
* DESCRIPTION:
*       The function gets the global write mode - FROM the CPSS DB -
*       per PP device and TCAM number (supported only for IDT).
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*
* OUTPUTS:
*       ruleFormatPtr   - X\Y or DATA\MASK rule format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_SUPPORTED         - not supported for NL TCAM.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamGlobalWriteModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tcamId;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT ruleFormat;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tcamId = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmExternalTcamGlobalWriteModeGet(devNum, tcamId, &ruleFormat);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ruleFormat);

    return CMD_OK;
}
/*************************************************************************
* cpssExMxPmExternalTcamTypeGet
*
* DESCRIPTION:
*       Get the device type of the external TCAM from CPSS DB.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*
* OUTPUTS:
*       tcamTypePtr     - TCAM device type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 tcamId;
    CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_ENT tcamType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tcamId = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmExternalTcamTypeGet(devNum, tcamId, &tcamType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tcamType);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmExternalTcamActionWrite
*
* DESCRIPTION:
*   The function writes an action in the Control Memory space.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       actionCsu       - the control memory in which the action resides,
*                         CSU-0 or CSU-1
*       lineStartIndex  - relative index of control memory line of external
*                         TCAM actions chunk. Size of the chunk need to be
*                         set by cpssExMxPmCfgPpLogicalInit.
*                         The cpssExMxPmCfgPpLogicalInit defines the base
*                         address for whole chunk.
*       actionSize      - action size in bits
*       actionDataPtr   - the action data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamActionWrite
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_CONTROL_MEMORY_ENT actionCsu;
    GT_U32 lineStartIndex;
    GT_U32 actionSize;
    GT_U32 actionData[7];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    actionCsu = (CPSS_EXMXPM_CONTROL_MEMORY_ENT)inArgs[1];
    lineStartIndex = (GT_U32)inArgs[2];
    actionSize = (GT_U32)inArgs[3];
    actionData[0] = (GT_U32)inArgs[4];
    actionData[1] = (GT_U32)inArgs[5];
    actionData[2] = (GT_U32)inArgs[6];
    actionData[3] = (GT_U32)inArgs[7];
    actionData[4] = (GT_U32)inArgs[8];
    actionData[5] = (GT_U32)inArgs[9];
    actionData[6] = (GT_U32)inArgs[10];


    /* call cpss api function */
    result = cpssExMxPmExternalTcamActionWrite(devNum, actionCsu, lineStartIndex, actionSize,actionData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmExternalTcamActionRead
*
* DESCRIPTION:
*   The function reads an action from the Control SRAM space.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       actionCsu       - the control memory in which the action resides,
*                         CSU-0 or CSU-1
*       lineStartIndex  - relative index of control memory line of external
*                         TCAM actions chunk. Size of the chunk need to be
*                         set by cpssExMxPmCfgPpLogicalInit.
*                         The cpssExMxPmCfgPpLogicalInit defines the base
*                         address for whole chunk.
*       actionSize      - action size to read in bits
*
* OUTPUTS:
       actionDataPtr    - the action data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamActionRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_CONTROL_MEMORY_ENT actionCsu;
    GT_U32 lineStartIndex;
    GT_U32 actionSize;
    GT_U32 actionData[7];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    actionCsu = (CPSS_EXMXPM_CONTROL_MEMORY_ENT)inArgs[1];
    lineStartIndex = (GT_U32)inArgs[2];
    actionSize = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmExternalTcamActionRead(devNum, actionCsu, lineStartIndex, actionSize, actionData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d",actionData[0],actionData[1],actionData[2],actionData[3],
                                       actionData[4],actionData[5],actionData[6]);
    return CMD_OK;
}
/* tables cpssExMxPm2ExternalTcamRule global variables */

/*table for Puma 2-different from cpssExMxPmExternalTcamRule table*/
static    GT_U32      maskPm2Data[20];
static    GT_U32      patternPm2Data[20];


static    GT_BOOL                       maskPm2_set = GT_FALSE; /* is mask set*/
static    GT_BOOL                       patternPm2_set = GT_FALSE; /* is pattern set*/
static    GT_U32                        maskPm2_ruleIndex = 0;
static    GT_U32                        patternPm2_ruleIndex = 0;
/*******************************************************************************
* cpssExMxPmExternalTcamRuleWrite
*
* DESCRIPTION:
*   The function sets rule on External TCAM device
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       ruleStartIndex  - index of rule in external TCAM. Please refer to the COMMENTS
*                         section here below for explanations of how to calculate
*                         the rule index value.
*       ruleSize        - size of rule entry.
*       dataSize        - number of meaningful bits in the patternOrXArr and maskOrYArr.
*                         All rule bits out of "dataSize" will set as don't cares
*       valid           - Rule's valid status:
*                         GT_TRUE - valid rule
*                         GT_FALSE - invalid rule
*       ageEnable       - Rule's aging enable status
*                         Relevant only if aging function is enabled and configured:
*                         GT_TRUE - aging is enabled for rule
*                         GT_FALSE - aging is disabled for rule.
*       dataFormat      - Indicates whether the data supplied in X\Y or pattern\mask
*                         format.
*       patternOrXArr   - array of 32-bit words that contains the pattern
*                         or X-data, word0 - bits0-31, word1 - bits32-63, ...
*       maskOrYArr      - array of 32-bit words that contains the mask
*                         or Y-data, word0 - bits0-31, word1 - bits32-63, ...
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The rule index value is always calculate in regard to 80 bits chunks.
*       It is the absolute offset from external TCAM start based on 80 bits chunks.
*       For a given rule size the user is not free to choose any value, but the value
*       must be in the rule size boundaries - as if one assumed that all rules in the
*       external TCAM have the same specified size.
*       Examples:
*       - Index 100 for 80 bits rule size means that there is a place for another
*       100 80 bits rule size before it.
*       - Index 100 for 160 bits rule size means that there is a place for another
*       100 80 bits rule size before it but for only 50 160 bits rule size before it.
*       - Index 100 for 640 bits rule size is invalid because it is not on 8 boundaries
*       (640/80 = 8).
*       - Odd indexes are invalid for 160 bits rule size.
*       - Indexes that are not 4 dividable with no remainder are invalid for 320 bits rule size.
*       - Indexes that are not 8 dividable with no remainder are invalid for 160 bits rule size.
*
*       If the index calculation is based on assuming that all rules in the
*       external TCAM have the same specified size, the following formula can be used:
*       Index = (required index in context of X bits rule size only) * (X/80)
*
*******************************************************************************/
static void prvWrSetExternalTcamRule
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(inFields[1] == 0) /* mask */
     {
         maskPm2_ruleIndex = (GT_U32)inFields[0];

         maskPm2Data[0]= (GT_U32)inFields[4];
         maskPm2Data[1]= (GT_U32)inFields[5];
         maskPm2Data[2]= (GT_U32)inFields[6];
         maskPm2Data[3]= (GT_U32)inFields[7];
         maskPm2Data[4]= (GT_U32)inFields[8];
         maskPm2Data[5]= (GT_U32)inFields[9];
         maskPm2Data[6]= (GT_U32)inFields[10];
         maskPm2Data[7]= (GT_U32)inFields[11];
         maskPm2Data[8]= (GT_U32)inFields[12];
         maskPm2Data[9]= (GT_U32)inFields[13];
         maskPm2Data[10]= (GT_U32)inFields[14];
         maskPm2Data[11]= (GT_U32)inFields[15];
         maskPm2Data[12]= (GT_U32)inFields[16];
         maskPm2Data[13]= (GT_U32)inFields[17];
         maskPm2Data[14]= (GT_U32)inFields[18];
         maskPm2Data[15]= (GT_U32)inFields[19];
         maskPm2Data[16]= (GT_U32)inFields[20];
         maskPm2Data[17]= (GT_U32)inFields[21];
         maskPm2Data[18]= (GT_U32)inFields[22];
         maskPm2Data[19]= (GT_U32)inFields[23];
         ruleIndex=mask_ruleIndex;

         maskPm2_set = GT_TRUE;
     }
     else /* pattern */
     {
         patternPm2_ruleIndex = (GT_U32)inFields[0];

         patternPm2Data[0]= (GT_U32)inFields[4];
         patternPm2Data[1]= (GT_U32)inFields[5];
         patternPm2Data[2]= (GT_U32)inFields[6];
         patternPm2Data[3]= (GT_U32)inFields[7];
         patternPm2Data[4]= (GT_U32)inFields[8];
         patternPm2Data[5]= (GT_U32)inFields[9];
         patternPm2Data[6]= (GT_U32)inFields[10];
         patternPm2Data[7]= (GT_U32)inFields[11];
         patternPm2Data[8]= (GT_U32)inFields[12];
         patternPm2Data[9]= (GT_U32)inFields[13];
         patternPm2Data[10]= (GT_U32)inFields[14];
         patternPm2Data[11]= (GT_U32)inFields[15];
         patternPm2Data[12]= (GT_U32)inFields[16];
         patternPm2Data[13]= (GT_U32)inFields[17];
         patternPm2Data[14]= (GT_U32)inFields[18];
         patternPm2Data[15]= (GT_U32)inFields[19];
         patternPm2Data[16]= (GT_U32)inFields[20];
         patternPm2Data[17]= (GT_U32)inFields[21];
         patternPm2Data[18]= (GT_U32)inFields[22];
         patternPm2Data[19]= (GT_U32)inFields[23];

         patternPm2_set = GT_TRUE;
     }


}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamRuleWriteFirst
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

    cmdOsMemSet(&maskPm2Data , 0, sizeof(GT_U32)*20);
    cmdOsMemSet(&patternPm2Data, 0, sizeof(GT_U32)*20);

    maskPm2_set    = GT_FALSE;
    patternPm2_set = GT_FALSE;


    prvWrSetExternalTcamRule(inArgs,inFields,numFields,outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/************************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamRuleWriteNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U8 devNum;
    GT_U32 tcamId;
    GT_U32 dataSize;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT ruleSize;
    GT_U32    ruleStartIndex;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   dataFormat;
    GT_BOOL ageEnable;
    GT_BOOL valid;



    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum=(GT_U8)inArgs[0];
    tcamId=(GT_U32)inArgs[1];
    ruleStartIndex=(GT_U32)inArgs[2];
    ruleSize=(CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT)inArgs[3];
    dataSize=(GT_U32)inArgs[4];
    dataFormat=(CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT)inArgs[5];

    valid=(GT_BOOL)inFields[2];
    ageEnable=(GT_BOOL)inFields[3];


    if(inFields[1] > 1)
    {
     /* pack output arguments to galtis string */
     galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
     return CMD_AGENT_ERROR;
    }


    if(inFields[1] == 0 && maskPm2_set)
    {
     /* pack output arguments to galtis string */
     galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of Ingress {Rule,Mask}.\n");
     return CMD_AGENT_ERROR;
    }

    if(inFields[1] == 1 && patternPm2_set)
    {
     /* pack output arguments to galtis string */
     galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of Ingress {Rule,Mask}.\n");
     return CMD_AGENT_ERROR;
    }

    prvWrSetExternalTcamRule(inArgs,inFields,numFields,outArgs);


    if(maskPm2_set && patternPm2_set && (maskPm2_ruleIndex != patternPm2_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }
    if(maskPm2_set && patternPm2_set){

    /* call cpss api function */
    result = cpssExMxPmExternalTcamRuleWrite(devNum, tcamId, maskPm2_ruleIndex,
                                             ruleSize, dataSize, valid,
                                             ageEnable, dataFormat,
                                             patternPm2Data, maskPm2Data);
    maskPm2_set = GT_FALSE;
    patternPm2_set = GT_FALSE;

    cmdOsMemSet(&maskPm2Data , 0,  sizeof(GT_U32)*20);
    cmdOsMemSet(&patternPm2Data, 0,  sizeof(GT_U32)*20);
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

static GT_U32 numOfEntries;/*number of entries for refreash*/
static GT_U32 step;/*step size*/
static GT_BOOL pattern_valid;
static GT_BOOL pattern_ageEn;
/*******************************************************************************
* cpssExMxPmExternalTcamRuleRead
*
* DESCRIPTION:
*   The function gets rule on External TCAM device
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       ruleStartIndex  - index of rule in external TCAM. Please refer to the COMMENTS
*                         section here below for explanations of how to calculate
*                         the rule index value.
*       ruleSize        - size of rule entry.
*       dataSize        - number of meaningful bits in the patternOrXArr and maskOrYArr
*       dataFormat      - if specified data/mask format - converted
*                         x\y => data\mask, if x\y format - returned as is
*
* OUTPUTS:
*       validPtr        - Rule's valid status:
*                         GT_TRUE - valid rule
*                         GT_FALSE - invalid rule
*       ageEnablePtr    - Rule's aging enable status
*                         Relevant only if aging function is enabled and configured:
*                         GT_TRUE - aging is enabled for rule
*                         GT_FALSE - aging is disabled for rule.
*       patternOrXArr   - array of 32-bit words that contains the pattern
*                         or X-data, word0 - bits0-31, word1 - bits32-63, ...
*       maskOrYArr      - array of 32-bit words that contains the mask
*                         or Y-data, word0 - bits0-31, word1 - bits32-63, ...
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
 COMMENTS:
*       The rule index value is always calculate in regard to 80 bits chunks.
*       It is the absolute offset from external TCAM start based on 80 bits chunks.
*       For a given rule size the user is not free to choose any value, but the value
*       must be in the rule size boundaries - as if one assumed that all rules in the
*       external TCAM have the same specified size.
*       Examples:
*       - Index 100 for 80 bits rule size means that there is a place for another
*       100 80 bits rule size before it.
*       - Index 100 for 160 bits rule size means that there is a place for another
*       100 80 bits rule size before it but for only 50 160 bits rule size before it.
*       - Index 100 for 640 bits rule size is invalid because it is not on 8 boundaries
*       (640/80 = 8).
*       - Odd indexes are invalid for 160 bits rule size.
*       - Indexes that are not 4 dividable with no remainder are invalid for 320 bits rule size.
*       - Indexes that are not 8 dividable with no remainder are invalid for 160 bits rule size.
*
*       If the index calculation is based on assuming that all rules in the
*       external TCAM have the same specified size, the following formula can be used:
*       Index = (required index in context of X bits rule size only) * (X/80)
*
*******************************************************************************/
static GT_STATUS prvWrGetExternalTcamRule
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    IN  GT_BOOL valid,
    IN  GT_BOOL ageEnable,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(isMask) /* mask */
    {
        inFields[0]=ruleIndex;
        inFields[1]=0;
        inFields[2]=pattern_valid;
        inFields[3]=pattern_ageEn;
        inFields[4]=maskPm2Data[0];
        inFields[5]=maskPm2Data[1];
        inFields[6]=maskPm2Data[2];
        inFields[7]=maskPm2Data[3];
        inFields[8]=maskPm2Data[4];
        inFields[9]=maskPm2Data[5];
        inFields[10]=maskPm2Data[6];
        inFields[11]=maskPm2Data[7];
        inFields[12]=maskPm2Data[8];
        inFields[13]=maskPm2Data[9];
        inFields[14]=maskPm2Data[10];
        inFields[15]=maskPm2Data[11];
        inFields[16]=maskPm2Data[12];
        inFields[17]=maskPm2Data[13];
        inFields[18]=maskPm2Data[14];
        inFields[19]=maskPm2Data[15];
        inFields[20]=maskPm2Data[16];
        inFields[21]=maskPm2Data[17];
        inFields[22]=maskPm2Data[18];
        inFields[23]=maskPm2Data[19];

        ruleIndex+=step;
        isMask = GT_FALSE;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2], inFields[3],
                    inFields[4], inFields[5], inFields[6],
                    inFields[7], inFields[8], inFields[9],
                    inFields[10], inFields[11], inFields[12],
                    inFields[13], inFields[14],  inFields[15], inFields[16],
                    inFields[17], inFields[18], inFields[19],
                    inFields[20], inFields[21], inFields[22],inFields[23]);

    }
    else /* pattern */
    {
        inFields[0]=ruleIndex;
        inFields[1]=1;
        inFields[2]=valid;
        inFields[3]=ageEnable;
        inFields[4]=patternPm2Data[0];
        inFields[5]=patternPm2Data[1];
        inFields[6]=patternPm2Data[2];
        inFields[7]=patternPm2Data[3];
        inFields[8]=patternPm2Data[4];
        inFields[9]=patternPm2Data[5];
        inFields[10]=patternPm2Data[6];
        inFields[11]=patternPm2Data[7];
        inFields[12]=patternPm2Data[8];
        inFields[13]=patternPm2Data[9];
        inFields[14]=patternPm2Data[10];
        inFields[15]=patternPm2Data[11];
        inFields[16]=patternPm2Data[12];
        inFields[17]=patternPm2Data[13];
        inFields[18]=patternPm2Data[14];
        inFields[19]=patternPm2Data[15];
        inFields[20]=patternPm2Data[16];
        inFields[21]=patternPm2Data[17];
        inFields[22]=patternPm2Data[18];
        inFields[23]=patternPm2Data[19];

        isMask = GT_TRUE;

         fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2], inFields[3],
                    inFields[4], inFields[5], inFields[6],
                    inFields[7], inFields[8], inFields[9],
                    inFields[10], inFields[11], inFields[12],
                    inFields[13], inFields[14],  inFields[15], inFields[16],
                    inFields[17], inFields[18], inFields[19],
                    inFields[20], inFields[21], inFields[22],inFields[23]);

    }



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}
/********************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamRuleRead
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    GT_U8 devNum;
    GT_U32 tcamId;
    GT_U32 dataSize;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT ruleSize;

    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   dataFormat;
    GT_BOOL ageEnable;
    GT_BOOL valid;
    GT_U32    ruleStartIndex;





    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    if(isMask==GT_FALSE)
    {

      devNum=(GT_U8)inArgs[0];
      tcamId=(GT_U32)inArgs[1];
      ruleStartIndex=(GT_U32)inArgs[2];
      ruleSize=(CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT)inArgs[3];
      dataSize=(GT_U32)inArgs[4];
      dataFormat=(CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT)inArgs[5];

      if(ruleIndex>ruleStartIndex+numOfEntries-1)
       {
          galtisOutput(outArgs, GT_OK, "%d", -1);
          return CMD_OK;
       }


      /* call cpss api function */
      result = cpssExMxPmExternalTcamRuleRead(devNum, tcamId, ruleIndex, ruleSize, dataSize, dataFormat, &valid, &ageEnable,
                                           patternPm2Data, maskPm2Data);
      pattern_valid=valid;
      pattern_ageEn=ageEnable;

     if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
      }



     }
  return prvWrGetExternalTcamRule(inArgs,inFields,numFields,valid,ageEnable,outArgs);

}
/********************************************************************************/
static CMD_STATUS wrCpssExMxPmExternalTcamRuleReadFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT ruleSize;
     GT_U32    ruleStartIndex;

     ruleStartIndex=(GT_U32)inArgs[2];
     ruleSize=(CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT)inArgs[3];
     numOfEntries=(GT_U32)inArgs[6];
     ruleIndex=ruleStartIndex;


     switch(ruleSize){

     case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E:
         step=1;
         break;
     case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E:
         step=2;
         break;
     case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E:
         step=4;
         break;
     case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E:
         step=8;
         break;
     default:
         break;
     }

    /*recalculate number of enries*/
    numOfEntries*=step;

    /* first to get is pattern */
    isMask = GT_FALSE;

    return wrCpssExMxPmExternalTcamRuleRead(inArgs,inFields,numFields,outArgs);
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmExternalTcamRegSet",
         &wrCpssExMxPmExtTcamRegSet,
         6, 0},
        {"cpssExMxPmExternalTcamRegGet",
         &wrCpssExMxPmExtTcamRegGet,
         3, 0},
        {"cpssExMxPmExternalTcamRuleSetFirst",
         &wrCpssExMxPmExtTcamRuleSetFirst,
         0, 23},
        {"cpssExMxPmExternalTcamRuleSetNext",
         &wrCpssExMxPmExtTcamRuleSetNext,
         5, 23},
        {"cpssExMxPmExternalTcamRuleEndSet",
         &wrCpssExMxPmExtTcamRuleEndSet,
         0, 0},
        {"cpssExMxPmExternalTcamRuleGetFirst",
         &wrCpssExMxPmExtTcamRuleGetFirst,
         6, 0},
        {"cpssExMxPmExternalTcamRuleGetNext",
         &wrCpssExMxPmExtTcamRuleGet,
         6, 0},
        {"cpssExMxPmExternalTcamHitIndexConvertSet",
         &wrCpssExMxPmExtTcamHitIndexConvertSet,
         2, 6},
        {"cpssExMxPmExternalTcamHitIndexConvertGetFirst",
         &wrCpssExMxPmExtTcamHitIndexConvertGetFirst,
         2, 0},
        {"cpssExMxPmExternalTcamHitIndexConvertGetNext",
         &wrCpssExMxPmExtTcamHitIndexConvertGetNext,
         2, 0},
        {"cpssExMxPmExternalTcamClientConfigSet",
         &wrCpssExMxPmExtTcamClientConfigSet,
         2, 4},
        {"cpssExMxPmExternalTcamClientConfigGetFirst",
         &wrCpssExMxPmExtTcamClientConfigGetFirst,
         2, 0},
        {"cpssExMxPmExternalTcamClientConfigGetNext",
         &wrCpssExMxPmExtTcamClientConfigGetNext,
         2, 0},

         /*puma 2*/

         {"cpssExMxPm2ExternalTcamClientConfigSet",
         &wrCpssExMxPmExternalTcamClientConfigSet,
         7, 0},
        {"cpssExMxPm2ExternalTcamClientConfigGet",
         &wrCpssExMxPmExternalTcamClientConfigGet,
         4, 0},
        {"cpssExMxPm2ExternalTcamHitIndexConvertSet",
         &wrCpssExMxPmExternalTcamHitIndexConvertSet,
         9, 0},
        {"cpssExMxPm2ExternalTcamHitIndexConvertGet",
         &wrCpssExMxPmExternalTcamHitIndexConvertGet,
         5, 0},
        {"cpssExMxPm2ExternalTcamGlobalWriteModeSet",
         &wrCpssExMxPmExternalTcamGlobalWriteModeSet,
         3, 0},
        {"cpssExMxPm2ExternalTcamGlobalWriteModeGet",
         &wrCpssExMxPmExternalTcamGlobalWriteModeGet,
         2, 0},
        {"cpssExMxPm2ExternalTcamTypeGet",
         &wrCpssExMxPmExternalTcamTypeGet,
         2, 0},
        {"cpssExMxPm2ExternalTcamActionWrite",
         &wrCpssExMxPmExternalTcamActionWrite,
         11, 0},
        {"cpssExMxPm2ExternalTcamActionRead",
         &wrCpssExMxPmExternalTcamActionRead,
         4, 0},
        {"cpssExMxPm2ExternalTcamRuleSetFirst",
         &wrCpssExMxPmExternalTcamRuleWriteFirst,
         0, 24},
        {"cpssExMxPm2ExternalTcamRuleSetNext",
          &wrCpssExMxPmExternalTcamRuleWriteNext,
         6, 24},
        {"cpssExMxPm2ExternalTcamRuleEndSet",
         &wrCpssExMxPmExtTcamRuleEndSet,
         0, 0},
        {"cpssExMxPm2ExternalTcamRuleGetFirst",
         &wrCpssExMxPmExternalTcamRuleReadFirst,
         7, 0},
        {"cpssExMxPm2ExternalTcamRuleGetNext",
         &wrCpssExMxPmExternalTcamRuleRead,
         7, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmExtTcam
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
GT_STATUS cmdLibInitCpssExMxPmExtTcam
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

