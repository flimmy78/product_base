/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmNstIp.c
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
#include <cpss/exMxPm/exMxPmGen/nst/cpssExMxPmNstIp.h>


static GT_U32  gEntryIndex;
/*******************************************************************************
* cpssExMxPmNstIpProfileTableEntrySet
*
* DESCRIPTION:
*       IP traffic is subject to the IP NST mechanisms based on its IP NST
*       Profile, which is assigned per InLIF. The IP NST Profile attributes
*       enable or disable IP NST features.
*       This function sets IP NST profile in IP NST profile table.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       profileIndex  - profile index (range 0..15)
*       profilePtr    - points to profile to set
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
static CMD_STATUS wrCpssExMxPmNstIpProfileTableEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    CPSS_EXMXPM_NST_IP_PROFILE_STC profile;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cmdOsMemSet(&profile, 0, sizeof(profile));

    profileIndex=(GT_U32)inFields[0];
    profile.enableArpSipFilter=(GT_BOOL)inFields[1];
    profile.enableArpSipSaCheck=(GT_BOOL)inFields[2];
    profile.enableArpuRpf=(GT_BOOL)inFields[3];
    profile.enableIpv4HeaderCheckForBridgedTraffic=(GT_BOOL)inFields[4];
    profile.enableIpv4SipFilterForBridgedTraffic=(GT_BOOL)inFields[5];
    profile.enableIpv4SipFilterForRoutedTraffic=(GT_BOOL)inFields[6];
    profile.enableIpv4SipLookupForAccessMatrix=(GT_BOOL)inFields[7];
    profile.enableIpv4SipSaCheckForBridgedTraffic=(GT_BOOL)inFields[8];
    profile.enableIpv4SipSaCheckForRoutedTraffic=(GT_BOOL)inFields[9];
    profile.enableIpv4uRpfForBridgedTraffic=(GT_BOOL)inFields[10];
    profile.enableIpv4uRpfForRoutedTraffic=(GT_BOOL)inFields[11];
    profile.ipv4DefaultSipAccessLevel=(GT_U32)inFields[12];

    /* call cpss api function */
    result = cpssExMxPmNstIpProfileTableEntrySet(devNum, profileIndex, &profile);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstIpProfileTableEntryGet
*
* DESCRIPTION:
*       IP traffic is subject to the IP NST mechanisms based on its IP NST
*       Profile, which is assigned per InLIF. The IP NST Profile attributes
*       enable or disable IP NST features.
*       This function gets IP NST profile from IP NST profile table.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       profileIndex  - profile index (range 0..15)
*
* OUTPUTS:
*       profilePtr    - points to profile
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
static CMD_STATUS wrCpssExMxPmNstIpProfileTableEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_NST_IP_PROFILE_STC profile;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
      devNum = (GT_U8)inArgs[0];

      gEntryIndex = 0;  /*reset on first*/


      /* call cpss api function */
      result = cpssExMxPmNstIpProfileTableEntryGet(devNum,  gEntryIndex, &profile);

      if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
      }

      inFields[0]= gEntryIndex;
      inFields[1]= profile.enableArpSipFilter;
      inFields[2]= profile.enableArpSipSaCheck;
      inFields[3]= profile.enableArpuRpf;
      inFields[4]= profile.enableIpv4HeaderCheckForBridgedTraffic;
      inFields[5]= profile.enableIpv4SipFilterForBridgedTraffic;
      inFields[6]= profile.enableIpv4SipFilterForRoutedTraffic;
      inFields[7]= profile.enableIpv4SipLookupForAccessMatrix;
      inFields[8]= profile.enableIpv4SipSaCheckForBridgedTraffic;
      inFields[9]= profile.enableIpv4SipSaCheckForRoutedTraffic;
      inFields[10]=profile.enableIpv4uRpfForBridgedTraffic;
      inFields[11]=profile.enableIpv4uRpfForRoutedTraffic;
      inFields[12]=profile.ipv4DefaultSipAccessLevel;

      gEntryIndex++;

    /* pack output arguments to galtis string */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10], inFields[11], inFields[12]
                              );

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


static CMD_STATUS wrCpssExMxPmNstIpProfileTableEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_NST_IP_PROFILE_STC profile;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

     if(gEntryIndex > 15)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    result = cpssExMxPmNstIpProfileTableEntryGet(devNum,  gEntryIndex, &profile);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
      inFields[0]=gEntryIndex ;
      inFields[1]= profile.enableArpSipFilter;
      inFields[2]= profile.enableArpSipSaCheck;
      inFields[3]= profile.enableArpuRpf;
      inFields[4]= profile.enableIpv4HeaderCheckForBridgedTraffic;
      inFields[5]= profile.enableIpv4SipFilterForBridgedTraffic;
      inFields[6]= profile.enableIpv4SipFilterForRoutedTraffic;
      inFields[7]= profile.enableIpv4SipLookupForAccessMatrix;
      inFields[8]= profile.enableIpv4SipSaCheckForBridgedTraffic;
      inFields[9]= profile.enableIpv4SipSaCheckForRoutedTraffic;
      inFields[10]=profile.enableIpv4uRpfForBridgedTraffic;
      inFields[11]=profile.enableIpv4uRpfForRoutedTraffic;
      inFields[12]=profile.ipv4DefaultSipAccessLevel;

      gEntryIndex++;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10], inFields[11],inFields[12]
                              );
        /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*****************************************************/
/*!!! UPDATE for cpssExMxPmNstIpProfileTableEntry !!!*/
/*****************************************************/
static GT_U32  gEntryIndex_1;
/*******************************************************************************
* cpssExMxPmNstIpProfileTableEntryExtSet
*
* DESCRIPTION:
*       IP traffic is subject to the IP NST mechanisms based on its IP NST
*       Profile, which is assigned per InLIF. The IP NST Profile attributes
*       enable or disable IP NST features.
*       This function sets IP NST profile in IP NST profile table.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       profileIndex  - profile index (range 0..15)
*       profilePtr    - points to profile to set
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
static CMD_STATUS wrCpssExMxPmNstIpProfileTableEntryExtSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    CPSS_EXMXPM_NST_IP_PROFILE_STC profile;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    profileIndex=(GT_U32)inFields[0];
    profile.enableArpSipFilter=(GT_BOOL)inFields[1];
    profile.enableArpSipSaCheck=(GT_BOOL)inFields[2];
    profile.enableArpuRpf=(GT_BOOL)inFields[3];
    profile.enableIpv4HeaderCheckForBridgedTraffic=(GT_BOOL)inFields[4];
    profile.enableIpv4SipFilterForBridgedTraffic=(GT_BOOL)inFields[5];
    profile.enableIpv4SipFilterForRoutedTraffic=(GT_BOOL)inFields[6];
    profile.enableIpv4SipLookupForAccessMatrix=(GT_BOOL)inFields[7];
    profile.enableIpv4SipSaCheckForBridgedTraffic=(GT_BOOL)inFields[8];
    profile.enableIpv4SipSaCheckForRoutedTraffic=(GT_BOOL)inFields[9];
    profile.enableIpv4uRpfForBridgedTraffic=(GT_BOOL)inFields[10];
    profile.enableIpv4uRpfForRoutedTraffic=(GT_BOOL)inFields[11];
    profile.ipv4DefaultSipAccessLevel=(GT_U32)inFields[12];

    /* additional fields */
    profile.enableIpv6uRpfForBridgedTraffic = (GT_BOOL)inFields[13];
    profile.enableIpv6HeaderCheckForBridgedTraffic = (GT_BOOL)inFields[14];
    profile.enableIpv6SipSaCheckForBridgedTraffic = (GT_BOOL)inFields[15];
    profile.enableIpv6SipFilterForBridgedTraffic = (GT_BOOL)inFields[16];

    profile.enableIpv6uRpfForRoutedTraffic = (GT_BOOL)inFields[17];
    profile.enableIpv6SipSaCheckForRoutedTraffic = (GT_BOOL)inFields[18];
    profile.enableIpv6SipFilterForRoutedTraffic = (GT_BOOL)inFields[19];
    profile.enableIpv6SipLookupForAccessMatrix = (GT_BOOL)inFields[20];
    profile.ipv6DefaultSipAccessLevel = (GT_U32)inFields[21];

    /* call cpss api function */
    result = cpssExMxPmNstIpProfileTableEntrySet(devNum, profileIndex, &profile);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstIpProfileTableEntryGet
*
* DESCRIPTION:
*       IP traffic is subject to the IP NST mechanisms based on its IP NST
*       Profile, which is assigned per InLIF. The IP NST Profile attributes
*       enable or disable IP NST features.
*       This function gets IP NST profile from IP NST profile table.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       profileIndex  - profile index (range 0..15)
*
* OUTPUTS:
*       profilePtr    - points to profile
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
static CMD_STATUS wrCpssExMxPmNstIpProfileTableEntryExtGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_NST_IP_PROFILE_STC profile;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* reset profile fields */
    cmdOsMemSet(&profile,0,sizeof(profile));

    /* map input arguments to locals */
      devNum = (GT_U8)inArgs[0];

      gEntryIndex_1 = 0;  /*reset on first*/


      /* call cpss api function */
      result = cpssExMxPmNstIpProfileTableEntryGet(devNum,  gEntryIndex_1, &profile);

      if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
      }

      inFields[0]= gEntryIndex_1;
      inFields[1]= profile.enableArpSipFilter;
      inFields[2]= profile.enableArpSipSaCheck;
      inFields[3]= profile.enableArpuRpf;
      inFields[4]= profile.enableIpv4HeaderCheckForBridgedTraffic;
      inFields[5]= profile.enableIpv4SipFilterForBridgedTraffic;
      inFields[6]= profile.enableIpv4SipFilterForRoutedTraffic;
      inFields[7]= profile.enableIpv4SipLookupForAccessMatrix;
      inFields[8]= profile.enableIpv4SipSaCheckForBridgedTraffic;
      inFields[9]= profile.enableIpv4SipSaCheckForRoutedTraffic;
      inFields[10]=profile.enableIpv4uRpfForBridgedTraffic;
      inFields[11]=profile.enableIpv4uRpfForRoutedTraffic;
      inFields[12]=profile.ipv4DefaultSipAccessLevel;

      /* additional fields */
      inFields[13] = profile.enableIpv6uRpfForBridgedTraffic;
      inFields[14] = profile.enableIpv6HeaderCheckForBridgedTraffic;
      inFields[15] = profile.enableIpv6SipSaCheckForBridgedTraffic;
      inFields[16] = profile.enableIpv6SipFilterForBridgedTraffic;

      inFields[17] = profile.enableIpv6uRpfForRoutedTraffic;
      inFields[18] = profile.enableIpv6SipSaCheckForRoutedTraffic;
      inFields[19] = profile.enableIpv6SipFilterForRoutedTraffic;
      inFields[20] = profile.enableIpv6SipLookupForAccessMatrix;
      inFields[21] = profile.ipv6DefaultSipAccessLevel;

      gEntryIndex_1++;

    /* pack output arguments to galtis string */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10], inFields[11],inFields[12],
                              inFields[13], inFields[14], inFields[15], inFields[16], inFields[17],
                              inFields[18],  inFields[19], inFields[20], inFields[21]
                              );

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


static CMD_STATUS wrCpssExMxPmNstIpProfileTableEntryExtGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_NST_IP_PROFILE_STC profile;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* reset profile fields */
    cmdOsMemSet(&profile,0,sizeof(profile));

    devNum = (GT_U8)inArgs[0];

     if(gEntryIndex_1 > 15)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    result = cpssExMxPmNstIpProfileTableEntryGet(devNum,  gEntryIndex_1, &profile);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
      inFields[0]=gEntryIndex_1;
      inFields[1]= profile.enableArpSipFilter;
      inFields[2]= profile.enableArpSipSaCheck;
      inFields[3]= profile.enableArpuRpf;
      inFields[4]= profile.enableIpv4HeaderCheckForBridgedTraffic;
      inFields[5]= profile.enableIpv4SipFilterForBridgedTraffic;
      inFields[6]= profile.enableIpv4SipFilterForRoutedTraffic;
      inFields[7]= profile.enableIpv4SipLookupForAccessMatrix;
      inFields[8]= profile.enableIpv4SipSaCheckForBridgedTraffic;
      inFields[9]= profile.enableIpv4SipSaCheckForRoutedTraffic;
      inFields[10]=profile.enableIpv4uRpfForBridgedTraffic;
      inFields[11]=profile.enableIpv4uRpfForRoutedTraffic;
      inFields[12]=profile.ipv4DefaultSipAccessLevel;

      /* additional fields */
      inFields[13] = profile.enableIpv6uRpfForBridgedTraffic;
      inFields[14] = profile.enableIpv6HeaderCheckForBridgedTraffic;
      inFields[15] = profile.enableIpv6SipSaCheckForBridgedTraffic;
      inFields[16] = profile.enableIpv6SipFilterForBridgedTraffic;

      inFields[17] = profile.enableIpv6uRpfForRoutedTraffic;
      inFields[18] = profile.enableIpv6SipSaCheckForRoutedTraffic;
      inFields[19] = profile.enableIpv6SipFilterForRoutedTraffic;
      inFields[20] = profile.enableIpv6SipLookupForAccessMatrix;
      inFields[21] = profile.ipv6DefaultSipAccessLevel;

      gEntryIndex_1++;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10], inFields[11],inFields[12],
                              inFields[13], inFields[14], inFields[15], inFields[16], inFields[17],
                              inFields[18],  inFields[19], inFields[20], inFields[21]
                              );
        /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*********************************************************/
/*!!! end UPDATE for cpssExMxPmNstIpProfileTableEntry !!!*/
/*********************************************************/

/*****************************************************/
/*!!! UPDATE 2 for cpssExMxPmNstIpProfileTableEntry !!!*/
/*****************************************************/
static GT_U32 nstMaxIndex;
static GT_U32 nstMinIndex;
static GT_U32 nstCurrentIndex;
/*******************************************************************************
* cpssExMxPmNstIpProfileTableEntryExtSet
*
* DESCRIPTION:
*       IP traffic is subject to the IP NST mechanisms based on its IP NST
*       Profile, which is assigned per InLIF. The IP NST Profile attributes
*       enable or disable IP NST features.
*       This function sets IP NST profile in IP NST profile table.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       profileIndex  - profile index (range 0..15)
*       profilePtr    - points to profile to set
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
static CMD_STATUS wrCpssExMxPmNstIpProfileTableEntryExt_1Set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    CPSS_EXMXPM_NST_IP_PROFILE_STC profile;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    profileIndex=(GT_U32)inFields[0];
    profile.enableArpSipFilter=(GT_BOOL)inFields[1];
    profile.enableArpSipSaCheck=(GT_BOOL)inFields[2];
    profile.enableArpuRpf=(GT_BOOL)inFields[3];
    profile.enableIpv4HeaderCheckForBridgedTraffic=(GT_BOOL)inFields[4];
    profile.enableIpv4SipFilterForBridgedTraffic=(GT_BOOL)inFields[5];
    profile.enableIpv4SipFilterForRoutedTraffic=(GT_BOOL)inFields[6];
    profile.enableIpv4SipLookupForAccessMatrix=(GT_BOOL)inFields[7];
    profile.enableIpv4SipSaCheckForBridgedTraffic=(GT_BOOL)inFields[8];
    profile.enableIpv4SipSaCheckForRoutedTraffic=(GT_BOOL)inFields[9];
    profile.enableIpv4uRpfForBridgedTraffic=(GT_BOOL)inFields[10];
    profile.enableIpv4uRpfForRoutedTraffic=(GT_BOOL)inFields[11];
    profile.ipv4DefaultSipAccessLevel=(GT_U32)inFields[12];

    /* additional fields */
    profile.enableIpv6uRpfForBridgedTraffic = (GT_BOOL)inFields[13];
    profile.enableIpv6HeaderCheckForBridgedTraffic = (GT_BOOL)inFields[14];
    profile.enableIpv6SipSaCheckForBridgedTraffic = (GT_BOOL)inFields[15];
    profile.enableIpv6SipFilterForBridgedTraffic = (GT_BOOL)inFields[16];

    profile.enableIpv6uRpfForRoutedTraffic = (GT_BOOL)inFields[17];
    profile.enableIpv6SipSaCheckForRoutedTraffic = (GT_BOOL)inFields[18];
    profile.enableIpv6SipFilterForRoutedTraffic = (GT_BOOL)inFields[19];
    profile.enableIpv6SipLookupForAccessMatrix = (GT_BOOL)inFields[20];
    profile.ipv6DefaultSipAccessLevel = (GT_U32)inFields[21];

    /* call cpss api function */
    result = cpssExMxPmNstIpProfileTableEntrySet(devNum, profileIndex, &profile);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstIpProfileTableEntryGet_1
*
* DESCRIPTION:
*       IP traffic is subject to the IP NST mechanisms based on its IP NST
*       Profile, which is assigned per InLIF. The IP NST Profile attributes
*       enable or disable IP NST features.
*       This function gets IP NST profile from IP NST profile table.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       profileIndex  - profile index (range 0..15)
*
* OUTPUTS:
*       profilePtr    - points to profile
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
static CMD_STATUS wrCpssExMxPmNstIpProfileTableEntryExt_1GetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_NST_IP_PROFILE_STC profile;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* reset profile fields */
    cmdOsMemSet(&profile,0,sizeof(profile));

    /* map input arguments to locals */
      devNum = (GT_U8)inArgs[0];

      nstMinIndex=(GT_U32)inArgs[1];
      nstMaxIndex=(GT_U32)inArgs[2];

      if(nstMinIndex > nstMaxIndex)
      {
          galtisOutput(outArgs, GT_BAD_VALUE, "%d", -1);
          return CMD_OK;
      }

      nstCurrentIndex = nstMinIndex;

      /* call cpss api function */
      result = cpssExMxPmNstIpProfileTableEntryGet(devNum,  nstCurrentIndex, &profile);

      if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
      }

      inFields[0]= nstCurrentIndex;
      inFields[1]= profile.enableArpSipFilter;
      inFields[2]= profile.enableArpSipSaCheck;
      inFields[3]= profile.enableArpuRpf;
      inFields[4]= profile.enableIpv4HeaderCheckForBridgedTraffic;
      inFields[5]= profile.enableIpv4SipFilterForBridgedTraffic;
      inFields[6]= profile.enableIpv4SipFilterForRoutedTraffic;
      inFields[7]= profile.enableIpv4SipLookupForAccessMatrix;
      inFields[8]= profile.enableIpv4SipSaCheckForBridgedTraffic;
      inFields[9]= profile.enableIpv4SipSaCheckForRoutedTraffic;
      inFields[10]=profile.enableIpv4uRpfForBridgedTraffic;
      inFields[11]=profile.enableIpv4uRpfForRoutedTraffic;
      inFields[12]=profile.ipv4DefaultSipAccessLevel;

      /* additional fields */
      inFields[13] = profile.enableIpv6uRpfForBridgedTraffic;
      inFields[14] = profile.enableIpv6HeaderCheckForBridgedTraffic;
      inFields[15] = profile.enableIpv6SipSaCheckForBridgedTraffic;
      inFields[16] = profile.enableIpv6SipFilterForBridgedTraffic;

      inFields[17] = profile.enableIpv6uRpfForRoutedTraffic;
      inFields[18] = profile.enableIpv6SipSaCheckForRoutedTraffic;
      inFields[19] = profile.enableIpv6SipFilterForRoutedTraffic;
      inFields[20] = profile.enableIpv6SipLookupForAccessMatrix;
      inFields[21] = profile.ipv6DefaultSipAccessLevel;

      nstCurrentIndex++;

    /* pack output arguments to galtis string */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10], inFields[11],inFields[12],
                              inFields[13], inFields[14], inFields[15], inFields[16], inFields[17],
                              inFields[18],  inFields[19], inFields[20], inFields[21]
                              );

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


static CMD_STATUS wrCpssExMxPmNstIpProfileTableEntryExt_1GetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_NST_IP_PROFILE_STC profile;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* reset profile fields */
    cmdOsMemSet(&profile,0,sizeof(profile));

    devNum = (GT_U8)inArgs[0];

    if(nstCurrentIndex > 15 || nstCurrentIndex > nstMaxIndex)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    result = cpssExMxPmNstIpProfileTableEntryGet(devNum,  nstCurrentIndex, &profile);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
      inFields[0]= nstCurrentIndex;
      inFields[1]= profile.enableArpSipFilter;
      inFields[2]= profile.enableArpSipSaCheck;
      inFields[3]= profile.enableArpuRpf;
      inFields[4]= profile.enableIpv4HeaderCheckForBridgedTraffic;
      inFields[5]= profile.enableIpv4SipFilterForBridgedTraffic;
      inFields[6]= profile.enableIpv4SipFilterForRoutedTraffic;
      inFields[7]= profile.enableIpv4SipLookupForAccessMatrix;
      inFields[8]= profile.enableIpv4SipSaCheckForBridgedTraffic;
      inFields[9]= profile.enableIpv4SipSaCheckForRoutedTraffic;
      inFields[10]=profile.enableIpv4uRpfForBridgedTraffic;
      inFields[11]=profile.enableIpv4uRpfForRoutedTraffic;
      inFields[12]=profile.ipv4DefaultSipAccessLevel;

      /* additional fields */
      inFields[13] = profile.enableIpv6uRpfForBridgedTraffic;
      inFields[14] = profile.enableIpv6HeaderCheckForBridgedTraffic;
      inFields[15] = profile.enableIpv6SipSaCheckForBridgedTraffic;
      inFields[16] = profile.enableIpv6SipFilterForBridgedTraffic;

      inFields[17] = profile.enableIpv6uRpfForRoutedTraffic;
      inFields[18] = profile.enableIpv6SipSaCheckForRoutedTraffic;
      inFields[19] = profile.enableIpv6SipFilterForRoutedTraffic;
      inFields[20] = profile.enableIpv6SipLookupForAccessMatrix;
      inFields[21] = profile.ipv6DefaultSipAccessLevel;

      nstCurrentIndex++;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10], inFields[11],inFields[12],
                              inFields[13], inFields[14], inFields[15], inFields[16], inFields[17],
                              inFields[18],  inFields[19], inFields[20], inFields[21]
                              );
        /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*********************************************************/
/*!!! end UPDATE 2 for cpssExMxPmNstIpProfileTableEntry !!!*/
/*********************************************************/



/*******************************************************************************
* cpssExMxPmNstIpRouterAccessMatrixCommandSet
*
* DESCRIPTION:
*       The Router Access Matrix enables access control for Unicast IPv4
*       routed packets based on the packet SIP and DIP.
*       This function sets the command for a given {SIP access level,
*       DIP access level} pair,
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       sipAccessLevel    - SIP Access Level (0..7)
*       dipAccessLevel    - DIP Access Level (0..7)
*       command           - command to set; valid values:
*                               CPSS_PACKET_CMD_ROUTE_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*                               CPSS_PACKET_CMD_DROP_SOFT_E
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
static CMD_STATUS wrCpssExMxPmNstIpRouterAccessMatrixCommandSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 sipAccessLevel;
    GT_U32 dipAccessLevel;
    CPSS_PACKET_CMD_ENT command;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    sipAccessLevel = (GT_U32)inArgs[1];
    dipAccessLevel = (GT_U32)inArgs[2];
    command = (CPSS_PACKET_CMD_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmNstIpRouterAccessMatrixCommandSet(devNum, sipAccessLevel, dipAccessLevel, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstIpRouterAccessMatrixCommandGet
*
* DESCRIPTION:
*       The Router Access Matrix enables access control for Unicast IPv4
*       routed packets based on the packet SIP and DIP.
*       This function gets the command set for a given {SIP access level,
*       DIP access level} pair,
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum            - device number
*       sipAccessLevel    - SIP Access Level (0..7)
*       dipAccessLevel    - DIP Access Level (0..7)
*
* OUTPUTS:
*       commandPtr        - command to set; valid values:
*                               CPSS_PACKET_CMD_ROUTE_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*                               CPSS_PACKET_CMD_DROP_SOFT_E
*                               CPSS_PACKET_CMD_INVALID_E
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
static CMD_STATUS wrCpssExMxPmNstIpRouterAccessMatrixCommandGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 sipAccessLevel;
    GT_U32 dipAccessLevel;
    CPSS_PACKET_CMD_ENT command;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    sipAccessLevel = (GT_U32)inArgs[1];
    dipAccessLevel = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmNstIpRouterAccessMatrixCommandGet(devNum, sipAccessLevel, dipAccessLevel, &command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", command);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstIpBridgedPktExceptionsCounterSet
*
* DESCRIPTION:
*       The Router Bridged Packets Exceptions Counter counts the number of bridge
*       packets which used IP NST services and failed any of the checks.
*       This function sets the value of the counter.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*       value         - value to set in the counter (range: 32 bit unsigned)
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
static CMD_STATUS wrCpssExMxPmNstIpBridgedPktExceptionsCounterSet
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
    result = cpssExMxPmNstIpBridgedPktExceptionsCounterSet(devNum, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNstIpBridgedPktExceptionsCounterGet
*
* DESCRIPTION:
*       The Router Bridged Packets Exceptions Counter counts the number of bridge
*       packets which used IP NST services and failed any of the checks.
*       This function gets the value of the counter.
*
* APPLICABLE DEVICES:
*       All ExMxPm devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       valuePtr      - value of the counter (range: 32 bit unsigned)
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
static CMD_STATUS wrCpssExMxPmNstIpBridgedPktExceptionsCounterGet
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
    result = cpssExMxPmNstIpBridgedPktExceptionsCounterGet(devNum, &value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmNstIpProfileTableEntrySet",
         &wrCpssExMxPmNstIpProfileTableEntrySet,
         1, 13},
        {"cpssExMxPmNstIpProfileTableEntryGetFirst",
         &wrCpssExMxPmNstIpProfileTableEntryGetFirst,
         1, 0},
        {"cpssExMxPmNstIpProfileTableEntryGetNext",
         &wrCpssExMxPmNstIpProfileTableEntryGetNext,
         1, 0},

/***** extended UPDATE to the table defined above *****/
        {"cpssExMxPmNstIpProfileTableEntryExtSet",
         &wrCpssExMxPmNstIpProfileTableEntryExtSet,
         1, 22},
        {"cpssExMxPmNstIpProfileTableEntryExtGetFirst",
         &wrCpssExMxPmNstIpProfileTableEntryExtGetFirst,
         1, 0},
        {"cpssExMxPmNstIpProfileTableEntryExtGetNext",
         &wrCpssExMxPmNstIpProfileTableEntryExtGetNext,
         1, 0},
/***** end: extended UPDATE to the table defined above *****/

/***** extended UPDATE to the table defined above *****/
        {"cpssExMxPmNstIpProfileTableEntryExt_1Set",
         &wrCpssExMxPmNstIpProfileTableEntryExt_1Set,
         3, 22},
        {"cpssExMxPmNstIpProfileTableEntryExt_1GetFirst",
         &wrCpssExMxPmNstIpProfileTableEntryExt_1GetFirst,
         3, 0},
        {"cpssExMxPmNstIpProfileTableEntryExt_1GetNext",
         &wrCpssExMxPmNstIpProfileTableEntryExt_1GetNext,
         3, 0},
/***** end: extended UPDATE to the table defined above *****/

        {"cpssExMxPmNstIpRouterAccessMatrixCommandSet",
         &wrCpssExMxPmNstIpRouterAccessMatrixCommandSet,
         4, 0},
        {"cpssExMxPmNstIpRouterAccessMatrixCommandGet",
         &wrCpssExMxPmNstIpRouterAccessMatrixCommandGet,
         3, 0},
        {"cpssExMxPmNstIpBridgedPktExceptionsCounterSet",
         &wrCpssExMxPmNstIpBridgedPktExceptionsCounterSet,
         2, 0},
        {"cpssExMxPmNstIpBridgedPktExceptionsCounterGet",
         &wrCpssExMxPmNstIpBridgedPktExceptionsCounterGet,
         2, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmNstIp
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
GT_STATUS cmdLibInitCpssExMxPmNstIp
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

