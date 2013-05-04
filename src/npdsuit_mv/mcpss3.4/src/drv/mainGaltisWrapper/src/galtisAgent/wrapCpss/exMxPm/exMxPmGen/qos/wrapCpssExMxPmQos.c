/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmQos.c
*
* DESCRIPTION:
*       Wrapper functions for
*           cpss/exMxPm/exMxPmGen/qos/cpssExMxPmQos.h API's
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
#include <cpss/exMxPm/exMxPmGen/qos/cpssExMxPmQos.h>


static  GT_U8 dsaTagQosIndexSet=0;
/********************************************************************************
* cpssExMxPmQosDsaTagQosProfileToQosParamMapSet
*
* DESCRIPTION:
*       Maps QoS Profile index from Tagged DSA packet to QoS Parameters.
*       Relevant when the packet is DSA Tagged and port trusts DSA Tag.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor number.
*       profileIndex    - the QoS Profile index from DSA Tag (0..127)
*       qosParamPtr     - pointer to qos parameters 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, dp or profileIndex
*       GT_OUT_OF_RANGE - QoS parameter is out of range
*       GT_HW_ERROR     - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosDsaTagQosProfileToQosParamSet
( 
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    IN GT_U8                        devNum;
    IN GT_U32                       profileIndex;
    IN CPSS_EXMXPM_QOS_PARAM_STC    qosParam;

    /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    profileIndex=(GT_U32)inFields[0];
    qosParam.dp=(CPSS_DP_LEVEL_ENT)inFields[1];
    qosParam.dscp=(GT_U32)inFields[2];
    qosParam.exp= (GT_U32)inFields[3];
    qosParam.tc=(GT_U32)inFields[4];
    qosParam.up=(GT_U32)inFields[5];

    result = cpssExMxPmQosDsaTagQosProfileToQosParamMapSet(devNum,dsaTagQosIndexSet,&qosParam);

    dsaTagQosIndexSet++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK; 

}
/********************************************************************************/
static  GT_U8 dsaTagQosIndexGet;

/*******************************************************************************
* cpssExMxPmQosDsaTagQosProfileToQosParamMapGet
*
* DESCRIPTION:
*       Get the QOS Parameters, mapped according to a given QoS Profile index  
*       from DSA Tagged packet.
*       Relevant when the packet is DSA Tagged and port trusts DSA Tag.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor number.
*       profileIndex    - the QoS Profile index from DSA Tag (0..127)
*
* OUTPUTS:
*       qosParamPtr     - pointer to qos parameters (tc,dp,up and dscp)
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, profileIndex
*       GT_HW_ERROR     - on writing to HW error.
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosDsaTagQosProfileToQosParamGetFirst
(  
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
    
)
{   GT_STATUS                       result;

    IN  GT_U8                       devNum;
    OUT CPSS_EXMXPM_QOS_PARAM_STC   qosParam;


    /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
 
    dsaTagQosIndexGet=0;/*reset on first*/

    
    /* call cpss api function */
    result= cpssExMxPmQosDsaTagQosProfileToQosParamMapGet(devNum,dsaTagQosIndexGet,&qosParam);

    if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
      } 
   inFields[0]=dsaTagQosIndexGet;
   inFields[1]=qosParam.dp;
   inFields[2]=qosParam.dscp;
   inFields[3]=qosParam.exp;
   inFields[4]=qosParam.tc;
   inFields[5]=qosParam.up;

   dsaTagQosIndexGet++;

   /* pack output arguments to galtis string */
   fieldOutput("%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                           inFields[3],  inFields[4],  inFields[5]
                              );

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosDsaTagQosProfileToQosParamGetNext
(  
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
    
)
{   GT_STATUS                       result;

    IN  GT_U8                       devNum;
    OUT CPSS_EXMXPM_QOS_PARAM_STC   qosParam;

     /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

     if(dsaTagQosIndexGet > 127)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

  result= cpssExMxPmQosDsaTagQosProfileToQosParamMapGet(devNum,dsaTagQosIndexGet,&qosParam);

   if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
      } 
   inFields[0]=dsaTagQosIndexGet;
   inFields[1]=qosParam.dp;
   inFields[2]=qosParam.dscp;
   inFields[3]=qosParam.exp;
   inFields[4]=qosParam.tc;
   inFields[5]=qosParam.up;

   dsaTagQosIndexGet++;

   /* pack output arguments to galtis string */
   fieldOutput("%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                           inFields[3],  inFields[4],  inFields[5]
                              );

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmQosDscpToQosParamMapSet
*
* DESCRIPTION:
*       Maps the IP packet QoS parameters to a QoS Parameters
*       (dp, tc, up and exp) according to packet's DSCP.
*       Relevant when packet is IP and port trusts DSCP.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum      - packet processor number.
*       dscp        - DSCP of an IP packet (0..63).
*       qosParamPtr - pointer to qos parameters. 
*                     The dscp parameter in qosParam is ignored.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, dscp or dp
*       GT_OUT_OF_RANGE - QoS parameter is out of range
*       GT_HW_ERROR     - on writing to HW error.
*
* COMMENTS:
*       
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosDscpToQosParamMapSet
(   
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER] 
    
)

{   
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_QOS_PARAM_STC       qosParam;

    /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    qosParam.dscp=(GT_U32)inFields[0];
    qosParam.dp=(CPSS_DP_LEVEL_ENT)inFields[1];
    qosParam.exp= (GT_U32)inFields[2];
    qosParam.tc=(GT_U32)inFields[3];
    qosParam.up=(GT_U32)inFields[4];

    result = cpssExMxPmQosDscpToQosParamMapSet(devNum,qosParam.dscp,&qosParam);

   

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK; 

}
/********************************************************************************/

static  GT_U8 dscpToQosIndexGet=0;
/*******************************************************************************
* cpssExMxPmQosDscpToQosParamMapGet
*
* DESCRIPTION:
*       Get QoS parameters (dp, tc, up and exp), that are mapped according to 
*       DSCP (or remapped DSCP) in IP packet.
*       Relevant when packet is IP and port trusts DSCP.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - packet processor number.
*       dscp        - DSCP of a IP packet (0..63), 
*                     index in the DSCP-to_QosParam table
*
* OUTPUTS:
*       qosParamPtr     - pointer to qos parameters (tc,dp,up and exp).
*                        The dscp parameter in qosParamPtr should be ignored.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum or dscp
*       GT_HW_ERROR     - on writing to HW error.
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*       
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosDscpToQosParamMapGetFirst
(   
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER] 
    
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_QOS_PARAM_STC       qosParam;

    /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;
    /* map input arguments to locals */
       devNum = (GT_U8)inArgs[0];

       dscpToQosIndexGet=0;/*reset on first*/


       /* call cpss api function */
       result= cpssExMxPmQosDscpToQosParamMapGet(devNum,dscpToQosIndexGet,&qosParam);

       if (result != GT_OK)
         {
             galtisOutput(outArgs, result, "%d", -1);
             return CMD_OK;
         } 
      
      inFields[0]=qosParam.dscp;
      inFields[1]=qosParam.dp;
      inFields[2]=qosParam.exp;
      inFields[3]=qosParam.tc;
      inFields[4]=qosParam.up;

      dscpToQosIndexGet++;

      /* pack output arguments to galtis string */
      fieldOutput("%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4]);
                                 

       galtisOutput(outArgs, GT_OK, "%f");

       return CMD_OK;
   }

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosDscpToQosParamMapGetNext
(   
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]  
    
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_QOS_PARAM_STC       qosParam;

    /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;
    /* map input arguments to locals */
       devNum = (GT_U8)inArgs[0];

        if(dscpToQosIndexGet > 63)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


       /* call cpss api function */
       result= cpssExMxPmQosDscpToQosParamMapGet(devNum,dscpToQosIndexGet,&qosParam);

       if (result != GT_OK)
         {
             galtisOutput(outArgs, result, "%d", -1);
             return CMD_OK;
         } 
      
      inFields[0]=qosParam.dscp;
      inFields[1]=qosParam.dp;
      inFields[2]=qosParam.exp;
      inFields[3]=qosParam.tc;
      inFields[4]=qosParam.up;

      dscpToQosIndexGet++;

      /* pack output arguments to galtis string */
      fieldOutput("%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4]);
                                 

     galtisOutput(outArgs, GT_OK, "%f");

     return CMD_OK;

}

/*******************************************************************************
* cpssExMxPmQosExpToQosParamMapSet
*
* DESCRIPTION:
*       Maps QoS Parameters from MPLS packet to Qos Parameters 
*       (dp, tc, up and dscp) according to packet's exp.
*       Relevant when packet is MPLS and port trusts MPLS pakets.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - packet processor number.
*       exp         - MPLS label exp value (0..7)
*       qosParamPtr - pointer to qos parameters, 
*                     the exp parameter in qosParam is ignored.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, dscp or dp
*       GT_OUT_OF_RANGE - QoS parameter is out of range
*       GT_HW_ERROR     - on writing to HW error.
*
* COMMENTS:
*       
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosExpToQosParamMapSet
(   
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER] 
    
)

{   
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_QOS_PARAM_STC       qosParam;

    /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    qosParam.exp=(GT_U32)inFields[0];
    qosParam.dp=(CPSS_DP_LEVEL_ENT)inFields[1];
    qosParam.dscp= (GT_U32)inFields[2];
    qosParam.tc=(GT_U32)inFields[3];
    qosParam.up=(GT_U32)inFields[4];

    result = cpssExMxPmQosExpToQosParamMapSet(devNum,qosParam.exp,&qosParam);

   

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK; 

}
static  GT_U8 expToQosIndexGet=0;

/*******************************************************************************
* cpssExMxPmQosExpToQosParamMapGet
*
* DESCRIPTION:
*       Get QoS Parameters (dp, tc, up and dscp), that are mapped according 
*       to MPLS packet's exp.
*       Relevant when packet is MPLS and port trusts MPLS pakets.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor number.
*       exp             - MPLS label exp value (0..7)
*
* OUTPUTS:
*       qosParamPtr     - pointer to qos parameters (tc,dp,up and dscp). 
*                         the exp parameter in qosParam should be ignored.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum or exp
*       GT_HW_ERROR     - on writing to HW error.
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*       
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosExpToQosParamMapGetFirst
(   
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER] 
    
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_QOS_PARAM_STC       qosParam;

    /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;
    /* map input arguments to locals */
       devNum = (GT_U8)inArgs[0];

       expToQosIndexGet=0;/*reset on first*/


       /* call cpss api function */
       result= cpssExMxPmQosExpToQosParamMapGet(devNum,expToQosIndexGet,&qosParam);

       if (result != GT_OK)
         {
             galtisOutput(outArgs, result, "%d", -1);
             return CMD_OK;
         } 
      
      inFields[0]=qosParam.exp;
      inFields[1]=qosParam.dp;
      inFields[2]=qosParam.dscp;
      inFields[3]=qosParam.tc;
      inFields[4]=qosParam.up;

      expToQosIndexGet++;

      /* pack output arguments to galtis string */
      fieldOutput("%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4]);
                                 

       galtisOutput(outArgs, GT_OK, "%f");

       return CMD_OK;
   }

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosExpToQosParamMapGetNext
(   
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]  
    
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_QOS_PARAM_STC       qosParam;

    /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;
    /* map input arguments to locals */
       devNum = (GT_U8)inArgs[0];

        if(expToQosIndexGet > 7)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


       /* call cpss api function */
       result= cpssExMxPmQosExpToQosParamMapGet(devNum,expToQosIndexGet,&qosParam);

       if (result != GT_OK)
         {
             galtisOutput(outArgs, result, "%d", -1);
             return CMD_OK;
         } 
      
      inFields[0]=qosParam.exp;
      inFields[1]=qosParam.dp;
      inFields[2]=qosParam.dscp;
      inFields[3]=qosParam.tc;
      inFields[4]=qosParam.up;

      expToQosIndexGet++;

      /* pack output arguments to galtis string */
      fieldOutput("%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4]);
                                 

     galtisOutput(outArgs, GT_OK, "%f");

     return CMD_OK;
   
}
/*******************************************************************************
* cpssExMxPmQosUpToQosParamMapSet
*
* DESCRIPTION:
*       Maps the VLAN Tagged packet QoS parameters to a QoS Parameters 
*       (dp, tc, exp and dscp) according to packet's user priority.
*       Relevant when packet is VLAN/Priority tagged and port trusts UP.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor number.
*       up              - IEEEE 802.1p User Priority (0..7),
*       qosParamPtr     - pointer to qos parameters,
*                         The User Priority in qosParam is ignored.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, up or dp
*       GT_OUT_OF_RANGE - QoS parameter is out of range
*       GT_HW_ERROR     - on writing to HW error.
*
* COMMENTS:
*       
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosUpToQosParamMapSet
(   
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER] 
    
)

{   
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_QOS_PARAM_STC       qosParam;

    /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    qosParam.up=(GT_U32)inFields[0];
    qosParam.dp=(CPSS_DP_LEVEL_ENT)inFields[1];
    qosParam.dscp= (GT_U32)inFields[2];
    qosParam.tc=(GT_U32)inFields[3];
    qosParam.exp=(GT_U32)inFields[4];

    result = cpssExMxPmQosUpToQosParamMapSet(devNum,qosParam.up,&qosParam);

   

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK; 

}
static  GT_U8 upToQosIndexGet=0;

/*******************************************************************************
* cpssExMxPmQosUpToQosParamMapGet
*
* DESCRIPTION:
*       Get QoS Parameters (dp, tc, exp and dscp), that are mapped according 
*       Source Tagged packet's User Priority.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum     - packet processor number.
*       up      - IEEEE 802.1p User Priority (0..7),
*
* OUTPUTS:
*       qosParamPtr     - pointer to qos parameters (tc,dp,exp and dscp).
*                         the User Priority in qosParam should be ignored.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum or up
*       GT_HW_ERROR     - on writing to HW error.
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*       
*       
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosUpToQosParamMapGetFirst
(   
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER] 
    
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_QOS_PARAM_STC       qosParam;

    /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;
    /* map input arguments to locals */
       devNum = (GT_U8)inArgs[0];

       upToQosIndexGet=0;/*reset on first*/


       /* call cpss api function */
       result= cpssExMxPmQosUpToQosParamMapGet(devNum,upToQosIndexGet,&qosParam);

       if (result != GT_OK)
         {
             galtisOutput(outArgs, result, "%d", -1);
             return CMD_OK;
         } 
      
      inFields[0]=qosParam.up;
      inFields[1]=qosParam.dp;
      inFields[2]=qosParam.dscp;
      inFields[3]=qosParam.tc;
      inFields[4]=qosParam.exp;

      upToQosIndexGet++;

      /* pack output arguments to galtis string */
      fieldOutput("%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4]);
                                 

       galtisOutput(outArgs, GT_OK, "%f");

       return CMD_OK;
   }

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosUpToQosParamMapGetNext
(   
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]  
    
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_EXMXPM_QOS_PARAM_STC       qosParam;

    /* check for valid arguments */
        if(!inArgs || !outArgs)
           return CMD_AGENT_ERROR;
    /* map input arguments to locals */
       devNum = (GT_U8)inArgs[0];

        if(upToQosIndexGet > 7)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


       /* call cpss api function */
       result= cpssExMxPmQosUpToQosParamMapGet(devNum,upToQosIndexGet,&qosParam);

       if (result != GT_OK)
         {
             galtisOutput(outArgs, result, "%d", -1);
             return CMD_OK;
         } 
      
      inFields[0]=qosParam.up;
      inFields[1]=qosParam.dp;
      inFields[2]=qosParam.dscp;
      inFields[3]=qosParam.tc;
      inFields[4]=qosParam.exp;

      upToQosIndexGet++;

      /* pack output arguments to galtis string */
      fieldOutput("%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4]);
                                 

     galtisOutput(outArgs, GT_OK, "%f");

     return CMD_OK;

}



/*******************************************************************************
* cpssExMxPmQosDscpToDscpMapSet
*
* DESCRIPTION:
*       Maps IP packet DSCP to another DSCP.
*       Relevant if the "Trust DSCP" path is followed in the Initial
*       QoS Assignment Algorithm and DSCP mutation enabled.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum     - packet processor number.
*       dscp        - DSCP of a IP packet
*       newDscp     - new DSCP, which is assigned to the IP packet
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or dscp
*       GT_OUT_OF_RANGE - newDscp is out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosDscpToDscpMapSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              dscp;
    GT_U32              newDscp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dscp = (GT_U32)inArgs[1];
    newDscp = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmQosDscpToDscpMapSet(devNum, dscp, newDscp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosDscpToDscpMapGet
*
* DESCRIPTION:
*       Get the Mapping of IP packet DSCP to another DSCP.
*       Relevant if the "Trust DSCP" path is followed in the Initial
*       QoS Assignment Algorithm and DSCP mutation enabled.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - packet processor number.
*       dscp        - DSCP of a IP packet
*
* OUTPUTS:
*       newDscpPtr - (pointer to) new DSCP, which is assigned to the IP packet
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or dscp
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosDscpToDscpMapGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              dscp;
    GT_U32              newDscpPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dscp = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmQosDscpToDscpMapGet(devNum, dscp, &newDscpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", newDscpPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosUpToUpMapSet
*
* DESCRIPTION:
*       Maps tagged packet UP to another UP.
*       Relevant if the "Trust UP" path is followed in the Initial
*       QoS Assignment Algorithm and UP mutation enabled.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - packet processor number.
*       up          - IEEEE 802.1p User Priority (0..7) of a tagged packet
*       newUp       - new IEEEE 802.1p User Priority (0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or up
*       GT_OUT_OF_RANGE - newUp is out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosUpToUpMapSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              up;
    GT_U32              newUp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    up = (GT_U32)inArgs[1];
    newUp = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmQosUpToUpMapSet(devNum, up, newUp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosUpToUpMapGet
*
* DESCRIPTION:
*       Get the mapping of the tagged packet UP to another UP.
*       Relevant if the "Trust UP" path is followed in the Initial
*       QoS Assignment Algorithm and UP mutation enabled.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - packet processor number.
*       up          - IEEEE 802.1p User Priority (0..7) of a tagged packet
*
* OUTPUTS:
*       newUpPtr    - (pointer to) new IEEEE 802.1p User Priority that is
*                     assigned to tagged packet.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or up
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosUpToUpMapGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              up;
    GT_U32              newUpPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    up = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmQosUpToUpMapGet(devNum, up, &newUpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", newUpPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosCfiToDpMapSet
*
* DESCRIPTION:
*       Maps tagged packet CFI to Drop Precedence.
*       Relevant only if packet is tagged and the "Trust UP" path is followed
*       in the Initial QoS Assignment Algorithm.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - packet processor number.
*       cfi         - packet cfi
*       dp          - the mapped Drop Precedence
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum, cfi or dp
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosCfiToDpMapSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              cfi;
    CPSS_DP_LEVEL_ENT   dp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cfi = (GT_U32)inArgs[1];
    dp = (CPSS_DP_LEVEL_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmQosCfiToDpMapSet(devNum, cfi, dp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosCfiToDpMapGet
*
* DESCRIPTION:
*       Get the mapping of tagged packet CFI to Drop Precedence.
*       Relevant only if packet is tagged and the "Trust UP" path is followed
*       in the Initial QoS Assignment Algorithm.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - packet processor number.
*       cfi         - packet cfi
*
* OUTPUTS:
*       dpPtr      - (pointer to) the mapped Drop Precedence
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or cfi
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosCfiToDpMapGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              cfi;
    CPSS_DP_LEVEL_ENT   dpPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cfi = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmQosCfiToDpMapGet(devNum, cfi, &dpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dpPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortCfiToDpMapEnableSet
*
* DESCRIPTION:
*       Enable/Disable per port the mapping of packet CFI to Drop Precedence.
*       Relevant only if packet is tagged and the "Trust UP" path is followed
*       in the Initial QoS Assignment Algorithm.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum     - packet processor number.
*       port    - port number.
*       enable  - GT_TRUE = enable the CFI to Drop Precedence mapping
*                 GT_FALSE = disable the CFI to Drop Precedence mapping
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or port
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortCfiToDpMapEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortCfiToDpMapEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortCfiToDpMapEnableGet
*
* DESCRIPTION:
*       Get per port the status of packet CFI mapping to Drop Precedence
*       (Enable/Disable).
*       Relevant only if packet is tagged and the "Trust UP" path is followed
*       in the Initial QoS Assignment Algorithm.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - packet processor number.
*       port        - port number.
*
* OUTPUTS:
*       enablePtr  - pointer to the status of CFI to Drop Precedence mapping.
*                    GT_TRUE = the CFI to Drop Precedence mapping enabled
*                    GT_FALSE = the CFI to Drop Precedence mapping disabled
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or port
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortCfiToDpMapEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortCfiToDpMapEnableGet(devNum, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDpToCfiMapEnableSet
*
* DESCRIPTION:
*       Enable/Disable per port the mapping of DP to packet CFI
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum     - packet processor number.
*       port    - port number.
*       enable  - GT_TRUE = enable the DP to CFI mapping
*                 GT_FALSE = disable the DP to CFI mapping
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or port
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDpToCfiMapEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     port;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortDpToCfiMapEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDpToCfiMapEnableGet
*
* DESCRIPTION:
*       Get per port status of DP to packet CFI mapping
*       (Enable/Disable).
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - packet processor number.
*       port        - port number.
*
* OUTPUTS:
*       enablePtr  - pointer to the status DP to CFI mapping.
*                    GT_TRUE = the DP to CFI mapping enabled
*                    GT_FALSE = the DP to CFI mapping disabled
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or port
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDpToCfiMapEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     port;
    GT_BOOL   enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortCfiToDpMapEnableGet(devNum, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosDpToCfiMapSet
*
* DESCRIPTION:
*       Maps DP to tagged packet CFI
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - packet processor number.
*       dp          - Drop Precedence
*       cfi         - the mapped packet cfi
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum, cfi or dp
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosDpToCfiMapSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           result;
    GT_U8               devNum;
    GT_U32              cfi;
    CPSS_DP_LEVEL_ENT   dp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dp = (CPSS_DP_LEVEL_ENT)inArgs[1];
    cfi = (GT_U32)inArgs[2];    

    /* call cpss api function */
    result = cpssExMxPmQosDpToCfiMapSet(devNum, dp, cfi);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosDpToCfiMapGet
*
* DESCRIPTION:
*       Get the mapping of Drop Precedence to tagged packet CFI.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum      - packet processor number.
*       dp         - Drop Precedence
*
* OUTPUTS:
*       cfiPtr     - (pointer to) the mapped cfi 
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or cfi
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosDpToCfiMapGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8               devNum;
    GT_U32              cfi;
    CPSS_DP_LEVEL_ENT   dp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dp = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmQosDpToCfiMapGet(devNum, dp, &cfi);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cfi);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortTrustEnableSet
*
* DESCRIPTION:
*       Enable/Disable per port one of QoS Trust modes.
*       Trust UP is relevant for tagged packets.
*       Trust DSCP is relevant for IP packets.
*       Trust EXP is relevant for MPLS packets.
*       Trust DSA Tag is relevant for cascading port only.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum              - packet processor number.
*       port                - port number.
*       trustParamType      - Trust QoS Parameter type: UP, DSCP, EXP or DSA Tag.
*       enable              - GT_TRUE = port trusts the given Trust mode
*                             GT_FALSE = port don't trust the given Trust mode
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or trustParamType
*
* COMMENTS:
*       The only QoS parameters that are used are: UP, DSCP, EXP and DSA Tag.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortTrustEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    CPSS_EXMXPM_QOS_PARAM_ENT  trustParamType;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    trustParamType = (CPSS_EXMXPM_QOS_PARAM_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortTrustEnableSet(devNum, port,
                                              trustParamType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortTrustConfigSet
*
* DESCRIPTION:
*       Enable/Disable per port one of QoS Trust modes.
*       Trust UP is relevant for tagged packets.
*       Trust DSCP is relevant for IP packets.
*       Trust EXP is relevant for MPLS packets.
*       Trust DSA Tag is relevant for cascading port only.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum           - packet processor number.
*       port             - port number.
*       trustParam       - port Trust Parameter for UP, DSCP, EXP or DSA Tag.
*                           GT_TRUE = port trusts the given Trust mode
*                           GT_FALSE = port don't trust the given Trust mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or trustParamType
*
* COMMENTS:
*       The only QoS parameters that are used are: UP, DSCP, EXP and DSA Tag.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortTrustConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8               devNum;
    GT_U8               port;
    CPSS_EXMXPM_QOS_PORT_TRUST_PARAM_STC trustParam;    

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum              = (GT_U8)inArgs[0];
    port                = (GT_U8)inArgs[1];
    trustParam.dsaTagQos = (GT_BOOL)inArgs[2];
    trustParam.up        = (GT_BOOL)inArgs[3];
    trustParam.dscp      = (GT_BOOL)inArgs[4];
    trustParam.exp       = (GT_BOOL)inArgs[5];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortTrustConfigSet(devNum, port, &trustParam);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortTrustGet
*
* DESCRIPTION:
*       Get status (Enable/Disable) per port one of QoS Trust modes.
*       Trust UP is relevant for tagged packets.
*       Trust DSCP is relevant for IP packets.
*       Trust EXP is relevant for MPLS packets.
*       Trust DSA Tag is relevant for cascading port only.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum              - packet processor number.
*       port                - port number.
*       trustParamType      - trust QoS Parameter type: UP, DSCP, EXP or DSA Tag.
*
* OUTPUTS:
*       enablePtr           - pointer to the status of Trust mode
*                           GT_TRUE = port trusts the given Trust mode
*                           GT_FALSE = port don't trust the given Trust mode
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum or trustParamType
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       The only QoS parameters that are used are: UP, DSCP, EXP and DSA Tag.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortTrustGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     port;
    CPSS_EXMXPM_QOS_PORT_TRUST_PARAM_STC trustParam;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortTrustConfigGet(devNum, port, &trustParam);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", 
                    trustParam.dsaTagQos, trustParam.up,
                    trustParam.dscp, trustParam.exp);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDefaultTcSet [SL: error in cpss func name - "Pm" is lost]
*
* DESCRIPTION:
*       Sets per port the default Traffic Class.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum       - packet processor number.
*       port      - port number.
*       tc        - default traffic class of a port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on wrong device number or wrong traffic class
*                         or wrong port number
*       GT_OUT_OF_RANGE - when tc is out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDefaultTcSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_U32              tc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    tc = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    /*result = cpssExMxPmQosPortDefaultTcSet(devNum, port, tc);*/
    result = cpssExMxPmQosPortDefaultTcSet(devNum, port, tc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDefaultTcGet
*
* DESCRIPTION:
*       Gets per port the default Traffic Class.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum     - packet processor number.
*       port    - port number.
*
* OUTPUTS:
*       tcPtr   - (poinetr to) default traffic class of a port.[0-7]
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong port number
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDefaultTcGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_U32              tcPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortDefaultTcGet(devNum, port, &tcPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tcPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDefaultUpSet
*
* DESCRIPTION:
*       Set default User Priority for untagged packet per port.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum  - packet processor device number.
*       port    - port number.(Not CPU port)
*       up      - default user priority (Vlan Priority Tag).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM- on wrong device number or port number
*       GT_OUT_OF_RANGE - when up is out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDefaultUpSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_U32              up;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    up = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortDefaultUpSet(devNum, port, up);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDefaultUpGet
*
* DESCRIPTION:
*       Get default User Priority for untagged packet per port.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - packet processor device number.
*       port     - port number.(Not CPU port)
*
* OUTPUTS:
*       upPtr    - (pointer to) default user priority (VPT).
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or port number
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDefaultUpGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_U32              upPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortDefaultUpGet(devNum, port, &upPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", upPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDefaultDscpSet
*
* DESCRIPTION:
*       Set default DSCP per port.
*       Relevant only if packet is non IP or Modify DSCP enabled and the
*       "No Trust" path is followed in the Initial QoS Assignment Algorithm.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor device number.
*       port            - port number.(Not CPU port)
*       dscp            - default DSCP.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or port number
*       GT_OUT_OF_RANGE - on dscp out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDefaultDscpSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_U32              dscp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    dscp = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortDefaultDscpSet(devNum, port, dscp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDefaultDscpGet
*
* DESCRIPTION:
*       Get default DSCP per port.
*       Relevant only if packet is non IP or Modify DSCP enabled and the
*       "No Trust" path is followed in the Initial QoS Assignment Algorithm.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor device number.
*       port            - port number.(Not CPU port)
*
* OUTPUTS:
*       dscpPtr  - (pointer to) port's default DSCP.
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or port number
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDefaultDscpGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_U32              dscpPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortDefaultDscpGet(devNum, port, &dscpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dscpPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDefaultDpSet
*
* DESCRIPTION:
*       Set default Drop Precedence per port.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor device number.
*       port            - port number.(Not CPU port)
*       dp              - default Drop Precedence
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or port number or dscp
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDefaultDpSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    CPSS_DP_LEVEL_ENT   dp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    dp = (CPSS_DP_LEVEL_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortDefaultDpSet(devNum, port, dp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDefaultDpGet
*
* DESCRIPTION:
*       Get default Drop Precedence per port.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor device number.
*       port            - port number.(Not CPU port)
*
* OUTPUTS:
*       dpPtr           - (pointer to) port's default Drop Precedence.
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or port number
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDefaultDpGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    CPSS_DP_LEVEL_ENT   dpPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortDefaultDpGet(devNum, port, &dpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dpPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDefaultExpSet
*
* DESCRIPTION:
*       Set default EXP for MPLS  per port.
*       Relevant only if packet is non MPLS or Modify EXP enabled and the
*       "No Trust" path is followed in the Initial QoS Assignment Algorithm.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor device number.
*       port            - port number.(Not CPU port)
*       exp             - default EXP.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number, port number
*       GT_OUT_OF_RANGE - on exp out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDefaultExpSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_U32              exp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    exp = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortDefaultExpSet(devNum, port, exp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortDefaultExpGet
*
* DESCRIPTION:
*       Get default EXP per port.
*       Relevant only if packet is non MPLS or Modify EXP enabled and the
*       "No Trust" path is followed in the Initial QoS Assignment Algorithm.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor device number.
*       port            - port number.(Not CPU port)
*
* OUTPUTS:
*       expPtr          - (pointer to) port's default EXP.
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or port number
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortDefaultExpGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_U32              expPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortDefaultExpGet(devNum, port, &expPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", expPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortQosPrecedenceSet
*
* DESCRIPTION:
*       Assign QoS precedence to a packet per port. The packet's attribute
*       assignment can be overridden or locked.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor device number.
*       port            - port number.(Not CPU port)
*       qosPrecedence   - assign QoS precedence
*                       CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E -
*                       The packet's attribute assignment can be overridden
*                       by the subsequent assignment mechanism
*                       CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E -
*                       The packet's attribute assignment can't be overridden.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or port number or qosPrecedence
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortQosPrecedenceSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  qosPrecedence;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortQosPrecedenceSet(devNum, port, qosPrecedence);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortQosPrecedenceGet
*
* DESCRIPTION:
*       Get the QoS precedence Assignment for a packet per port.
*
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor device number.
*       port            - port number.(Not CPU port)
*
* OUTPUTS:
*       qosPrecedencePtr    - assign QoS precedence
*                           CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E -
*                           The packet's attribute assignment can be overridden
*                           by the subsequent assignment mechanism
*                           CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E -
*                           The packet's attribute assignment can't be overridden.
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or port number
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortQosPrecedenceGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  qosPrecedencePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortQosPrecedenceGet(devNum, port,
                                                &qosPrecedencePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", qosPrecedencePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortModifyQoSParamEnableSet
*
* DESCRIPTION:
*       Enable/Disable per port to modify packet's the given QoS Parameter
*       (UP, DSCP and EXP)
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum     - packet processor device number.
*       port       - port number.(Not CPU port)
*       param      - one of the QoS Parameters:
*                    CPSS_EXMXPM_QOS_PARAM_UP_E,
*                    CPSS_EXMXPM_QOS_PARAM_DSCP_E,
*                    CPSS_EXMXPM_QOS_PARAM_EXP_E
*       enable     - GT_TRUE = packet's specified QoS Parameter is modified
*                              according to packets't type and port's Trust QoS mode.
*                    GT_FALSE = packet's specified QoS Parameter  is unchanged
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or port number
*
* COMMENTS:
*       The only QoS parameters that are used are: UP, DSCP and EXP
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortModifyQoSParamEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    CPSS_EXMXPM_QOS_PARAM_ENT  param;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    param = (CPSS_EXMXPM_QOS_PARAM_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortModifyQoSParamEnableSet(devNum, port, param,
                                                       enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortModifyQoSParamEnableGet
*
* DESCRIPTION:
*       Enable/Disable per port to modify packet's the given QoS Parameter
*       (UP, DSCP and EXP)
*       APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum     - packet processor device number.
*       port       - port number.(Not CPU port)
*       param      - one of the QoS Parameters:
*                    CPSS_EXMXPM_QOS_PARAM_UP_E,
*                    CPSS_EXMXPM_QOS_PARAM_DSCP_E,
*                    CPSS_EXMXPM_QOS_PARAM_EXP_E
*
* OUTPUTS:
*       enablePtr  - GT_TRUE = packet's specified QoS Parameter is modified
*                              according to packets't type and port's Trust QoS mode.
*                    GT_FALSE = packet's specified QoS Parameter  is unchanged
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on wrong device number or port number
*
* COMMENTS:
*       The only QoS parameters that are used are: UP, DSCP and EXP
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortModifyQoSParamEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    CPSS_EXMXPM_QOS_PARAM_ENT  param;
    GT_BOOL             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    param = (CPSS_EXMXPM_QOS_PARAM_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortModifyQoSParamEnableGet(devNum, port, param,
                                                       &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortReMapDscpEnableSet
*
* DESCRIPTION:
*       Enable/Disable DSCP remapping per port
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum             - packet processor number.
*       port            -  port number.
*       enable          - GT_TRUE: Packet's DSCP is remapped.
*                         GT_FALSE: Packet's DSCP is not remapped.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - wrong devNum or port.
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortReMapDscpEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortReMapDscpEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortReMapDscpEnableGet
*
* DESCRIPTION:
*       Get the status of DSCP remapping (enabled/disabled) per port
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum          - packet processor number.
*       port         - port number.
*
* OUTPUTS:
*       enablePtr    - pointer to status of DSCP remapping.
*                      GT_TRUE: Packet's DSCP is remapped.
*                      GT_FALSE: Packet's DSCP is not remapped.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - wrong devNum or port.
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortReMapDscpEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortReMapDscpEnableGet(devNum, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortReMapUpEnableSet
*
* DESCRIPTION:
*       Enable/Disable per port User Priority remapping.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum         - packet processor number.
*       port        - port number.
*       enable      - GT_TRUE: Packet's UP is remapped.
*                     GT_FALSE: Packet's UP is not remapped.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - wrong devNum or port.
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortReMapUpEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortReMapUpEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosPortReMapUpEnableGet
*
* DESCRIPTION:
*       Get per port the status of User Priority remapping (enabled/disabled).
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum                 - packet processor number.
*       port                - port number.
*
* OUTPUTS:
*       enablePtr   - pointer to status of User Priority remapping
*                     GT_TRUE: Packet's User Priority is remapped.
*                     GT_FALSE: Packet's User Priority is not remapped.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - wrong devNum or port.
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosPortReMapUpEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U8               port;
    GT_BOOL             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmQosPortReMapUpEnableGet(devNum, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/* cpssExMxPmQosMacQosEntry     Table    */
/*******************************************************************************
* cpssExMxPmQosMacQosEntrySet
*
* DESCRIPTION:
*       Set the QoS Attribute of the MAC QoS Table Entry
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum            - packet processor number.
*       entryIdx       - Index of a MAC QoS table's entry. Valid values 1 - 8.
*       macQosCfgPtr   - Pointer to QoS related configuration of a MAC QoS 
*                        table's entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, entryIndex or macQosCfgPtr
*       GT_OUT_OF_RANGE - QoS parameter out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       Indexes 1-7 are approached from FDB and index 8 is 
*       reserved for unknown unicast or unregistered 
*       multicast and broadcast packets.
*
**********************************************************************************/
static CMD_STATUS wrCpssExMxPmQosMacQosEntrySet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    GT_U8                            devNum;
    GT_U32                           entryIdx;
    CPSS_EXMXPM_QOS_ENTRY_STC        macQosCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    entryIdx = (GT_U8)inFields[0];

    macQosCfg.qosParamsModify.modifyDp= (GT_BOOL)inFields[1];
    macQosCfg.qosParamsModify.modifyDscp=(GT_BOOL)inFields[2];
   
    macQosCfg.qosParamsModify.modifyTc=(GT_BOOL)inFields[3];
    macQosCfg.qosParamsModify.modifyUp=(GT_BOOL)inFields[4];
    macQosCfg.qosParamsModify.modifyExp = GT_FALSE;
                       
    macQosCfg.qosParams.tc =(GT_U32)inFields[5];                    
    macQosCfg.qosParams.dp =(CPSS_DP_LEVEL_ENT)inFields[6];
    macQosCfg.qosParams.up =(GT_U32)inFields[7];                         
    macQosCfg.qosParams.dscp=(GT_U32)inFields[8];
    macQosCfg.qosParams.exp = 0;
   
   

    macQosCfg.qosPrecedence =(CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[9];

    /* call cpss api function */
    result = cpssExMxPmQosMacQosEntrySet(devNum, entryIdx, &macQosCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}




static  GT_U8 entryCnt;
/*******************************************************************************
* cpssExMxPmQosMacQosEntryGet
*
* DESCRIPTION:
*       Set the QoS Attribute of the MAC QoS Table Entry
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum         - packet processor number.
*       entryIdx       - Index of a MAC QoS table's entry. Valid values 1- 8.
*
* OUTPUTS:
*       macQosCfgPtr   - Pointer to QoS related configuration of a MAC QoS 
*                        table's entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, entryIndex or macQosCfgPtr
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       Indexes 1-7 are approached from FDB and index 8 is 
*       reserved for unknown unicast or unregistered 
*       multicast and broadcast packets.
*
********************************************************************************/
static CMD_STATUS wrCpssExMxPmQosMacQosEntryGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;
    GT_U8                            devNum;
    CPSS_EXMXPM_QOS_ENTRY_STC        macQosCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    entryCnt=0;   

    /* call cpss api function */
    result = cpssExMxPmQosMacQosEntryGet(devNum, entryCnt, &macQosCfg);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = entryCnt;
    inFields[1] = macQosCfg.qosParamsModify.modifyDp;
    inFields[2] =macQosCfg.qosParamsModify.modifyDscp;
    inFields[3] = macQosCfg.qosParamsModify.modifyTc;
    inFields[4] =macQosCfg.qosParamsModify.modifyUp;

    inFields[5]= macQosCfg.qosParams.tc;
    inFields[6]=macQosCfg.qosParams.dp;
    inFields[7]=macQosCfg.qosParams.up;
    inFields[8]=macQosCfg.qosParams.dscp;
    
    inFields[9]= macQosCfg.qosPrecedence;

     entryCnt++;
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],
               inFields[2], inFields[3],  inFields[4],
               inFields[5], inFields[6],  inFields[7],
               inFields[8], inFields[9] ); 
              
   

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}




/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosMacQosEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_STATUS                        result;
    GT_U8                            devNum;
    CPSS_EXMXPM_QOS_ENTRY_STC        macQosCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    if(entryCnt > 8)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPmQosMacQosEntryGet(devNum, entryCnt, &macQosCfg);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = entryCnt;
    inFields[1] = macQosCfg.qosParamsModify.modifyDp;
    inFields[2] =macQosCfg.qosParamsModify.modifyDscp;
    inFields[3] = macQosCfg.qosParamsModify.modifyTc;
    inFields[4] =macQosCfg.qosParamsModify.modifyUp;

    inFields[5]= macQosCfg.qosParams.tc;
    inFields[6]=macQosCfg.qosParams.dp;
    inFields[7]=macQosCfg.qosParams.up;
    inFields[8]=macQosCfg.qosParams.dscp;


   inFields[9]= macQosCfg.qosPrecedence;

   entryCnt++;
/* pack and output table fields */
   fieldOutput("%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],
           inFields[2], inFields[3],  inFields[4],
           inFields[5], inFields[6],  inFields[7],
           inFields[8], inFields[9] ); 



  galtisOutput(outArgs, GT_OK, "%f");
  return CMD_OK;
}





/*******************************************************************************
* cpssExMxPmQosMacQosConflictResolutionSet
*
* DESCRIPTION:
*       Configure QoS Marking Conflict Mode.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum                 - packet processor number.
*       macQosResolvType    - Enum holding six attributes for selecting the
*                             SA command or the DA command.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, entryIndex or macQosResolvType
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosMacQosConflictResolutionSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_EXMXPM_QOS_MAC_RESOLVE_ENT  macQosResolvType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    macQosResolvType = (CPSS_EXMXPM_QOS_MAC_RESOLVE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmQosMacQosConflictResolutionSet(devNum,
                                                       macQosResolvType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosMacQosConflictResolutionGet
*
* DESCRIPTION:
*       Get the configuration of QoS Marking Conflict Mode.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum                 - packet processor number.
* OUTPUTS:
*       macQosResolvTypePtr - (pointer to) Enum holding six attributes for
*                             selecting the SA command or the DA command:
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosMacQosConflictResolutionGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_EXMXPM_QOS_MAC_RESOLVE_ENT  macQosResolvTypePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmQosMacQosConflictResolutionGet(devNum,
                                                       &macQosResolvTypePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", macQosResolvTypePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosUnkUcEnableSet
*
* DESCRIPTION:
*       Enable or disable assignment of a default QoS profile (MAC QoS entry#8 )
*       for Unknown Unicast packets.
*       When the assignment is enabled, its corresponding packets are
*       unconditionally assigned QoS based on the MAC QoS Entry #8 (regardless
*       of SA QoS)
*       If the the assignment is disabled, it's corresponding packets are
*       assigned QoS based on the regular algorithm (MAC-SA QoS if found, or
*       preserve previous settings if MAC-SA not found)
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum       - packet processor number.
*       enable    - GT_TRUE = the corresponding packets are unconditionally
*                             assigned QoS based on the Entry #8
*                             (regardless of SA QoS)
*                   GT_FALSE = the corresponding packets are assigned QoS
*                              based on the regular algorithm
*                              (MAC-SA QoS if found, or preserve previous
*                              settings if MAC-SA not found)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosUnkUcEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmQosUnkUcEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosUnkUcEnableGet
*
* DESCRIPTION:
*       Get the status (Enable/Disable) of assignment of a default QoS profile
*       (MAC QoS entry#8 ) for Unknown Unicast packets.
*       When the assignment is enabled, its corresponding packets are
*       unconditionally assigned QoS based on the MAC QoS Entry #8 (regardless
*       of SA QoS)
*       If the the assignment is disabled, it's corresponding packets are
*       assigned QoS based on the regular algorithm (MAC-SA QoS if found, or
*       preserve previous settings if MAC-SA not found)
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum       - packet processor number.
*
* OUTPUTS:
*       enablePtr - GT_TRUE = the corresponding packets are unconditionally
*                             assigned QoS based on the Entry #9
*                             (regardless of SA QoS)
*                   GT_FALSE = the corresponding packets are assigned QoS
*                              based on the regular algorithm
*                              (MAC-SA QoS if found, or preserve previous
*                              settings if MAC-SA not found)
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosUnkUcEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_BOOL             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmQosUnkUcEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosUnregMcBcEnableSet
*
* DESCRIPTION:
*       Enable or disable assignment of a default QoS profile (MAC QoS entry#8 )
*       for Unregistered Multicast and Broadcast packets.
*       When the assignment is enabled, its corresponding packets are
*       unconditionally assigned QoS based on the MAC QoS Entry #8 (regardless
*       of SA QoS)
*       If the the assignment is disabled, it's corresponding packets are
*       assigned QoS based on the regular algorithm (MAC-SA QoS if found, or
*       preserve previous settings if MAC-SA not found)
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum       - packet processor number.
*       enable    - GT_TRUE = the corresponding packets are unconditionally
*                             assigned QoS based on the Entry #8
*                             (regardless of SA QoS)
*                   GT_FALSE = the corresponding packets are assigned QoS
*                              based on the regular algorithm
*                              (MAC-SA QoS if found, or preserve previous
*                              settings if MAC-SA not found)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosUnregMcBcEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmQosUnregMcBcEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosUnregMcBcEnableGet
*
* DESCRIPTION:
*       Get the status (Enable/Disable) of assignment of a default QoS profile
*       (MAC QoS entry#8 ) for Unregistered Multicast and Broadcast packets.
*       When the assignment is enabled, its corresponding packets are
*       unconditionally assigned QoS based on the MAC QoS Entry #8 (regardless
*       of SA QoS)
*       If the the assignment is disabled, it's corresponding packets are
*       assigned QoS based on the regular algorithm (MAC-SA QoS if found, or
*       preserve previous settings if MAC-SA not found)
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum       - packet processor number.
*
* OUTPUTS:
*       enablePtr - GT_TRUE = the corresponding packets are unconditionally
*                             assigned QoS based on the Entry #9
*                             (regardless of SA QoS)
*                   GT_FALSE = the corresponding packets are assigned QoS
*                              based on the regular algorithm
*                              (MAC-SA QoS if found, or preserve previous
*                              settings if MAC-SA not found)
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosUnregMcBcEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_BOOL             enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmQosUnregMcBcEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosAutoLearnQosProfileSet
*
* DESCRIPTION:
*       Set AutoLearnQosProfileIndex, when performing auto-learning, the FDB 
*       Entry<SA/DA QoS Profile> is updated with the value of this field.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - packet processor number.
*       index    - AutoLearnQosProfileIndex (0...7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum
*       GT_OUT_OF_RANGE - on wrong index
*       GT_HW_ERROR     - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosAutoLearnQosProfileSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmQosAutoLearnQosProfileSet(devNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmQosAutoLearnQosProfileGet
*
* DESCRIPTION:
*       Get AutoLearnQosProfileIndex, when performing auto-learning, the FDB 
*       Entry<SA/DA QoS Profile> is updated with the value of this field.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum   - packet processor number.
*
* OUTPUTS:
*       indexPtr    - (pointer to) AutoLearnQosProfileIndex value
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum
*       GT_BAD_PTR      - on indexPtr null pointer
*       GT_HW_ERROR     - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmQosAutoLearnQosProfileGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmQosAutoLearnQosProfileGet(devNum, &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", index);
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{   
    {"cpssExMxPmQosDsaTagQosProfileToQosParamSet",
        &wrCpssExMxPmQosDsaTagQosProfileToQosParamSet,
        1, 6},
    {"cpssExMxPmQosDsaTagQosProfileToQosParamGetFirst",
        &wrCpssExMxPmQosDsaTagQosProfileToQosParamGetFirst,
        1, 0},
    {"cpssExMxPmQosDsaTagQosProfileToQosParamGetNext",
        &wrCpssExMxPmQosDsaTagQosProfileToQosParamGetNext,
        1, 0},

    {"cpssExMxPmQosDscpToQosParamMapSet",
        &wrCpssExMxPmQosDscpToQosParamMapSet,
        1, 5},
    {"cpssExMxPmQosDscpToQosParamMapGetFirst",
        &wrCpssExMxPmQosDscpToQosParamMapGetFirst,
        1, 0},
    {"cpssExMxPmQosDscpToQosParamMapGetNext",
        &wrCpssExMxPmQosDscpToQosParamMapGetNext,
        1, 0},

    {"cpssExMxPmQosExpToQosParamMapSet",
        &wrCpssExMxPmQosExpToQosParamMapSet,
        1, 5},
    {"cpssExMxPmQosExpToQosParamMapGetFirst",
        &wrCpssExMxPmQosExpToQosParamMapGetFirst,
        1, 0},
    {"cpssExMxPmQosExpToQosParamMapGetNext",
        &wrCpssExMxPmQosExpToQosParamMapGetNext,
        1, 0},

    {"cpssExMxPmQosUpToQosParamMapSet",
        &wrCpssExMxPmQosUpToQosParamMapSet,
        1, 5},
    {"cpssExMxPmQosUpToQosParamMapGetFirst",
        &wrCpssExMxPmQosUpToQosParamMapGetFirst,
        1, 0},
    {"cpssExMxPmQosUpToQosParamMapGetNext",
        &wrCpssExMxPmQosUpToQosParamMapGetNext,
        1, 0},

    {"cpssExMxPmQosDscpToDscpMapSet",
        &wrCpssExMxPmQosDscpToDscpMapSet,
        3, 0},

    {"cpssExMxPmQosDscpToDscpMapGet",
        &wrCpssExMxPmQosDscpToDscpMapGet,
        2, 0},

    {"cpssExMxPmQosUpToUpMapSet",
        &wrCpssExMxPmQosUpToUpMapSet,
        3, 0},

    {"cpssExMxPmQosUpToUpMapGet",
        &wrCpssExMxPmQosUpToUpMapGet,
        2, 0},

    {"cpssExMxPmQosCfiToDpMapSet",
        &wrCpssExMxPmQosCfiToDpMapSet,
        3, 0},

    {"cpssExMxPmQosCfiToDpMapGet",
        &wrCpssExMxPmQosCfiToDpMapGet,
        2, 0},

    {"cpssExMxPmQosPortCfiToDpMapEnableSet",
        &wrCpssExMxPmQosPortCfiToDpMapEnableSet,
        3, 0},

    {"cpssExMxPmQosPortCfiToDpMapEnableGet",
        &wrCpssExMxPmQosPortCfiToDpMapEnableGet,
        2, 0},

    {"cpssExMxPmQosDpToCfiMapSet",
        &wrCpssExMxPmQosDpToCfiMapSet,
        3, 0},

    {"cpssExMxPmQosDpToCfiMapGet",
        &wrCpssExMxPmQosDpToCfiMapGet,
        2, 0},

    {"cpssExMxPmQosPortDpToCfiMapEnableSet",
        &wrCpssExMxPmQosPortDpToCfiMapEnableSet,
        3, 0},

    {"cpssExMxPmQosPortDpToCfiMapEnableGet",
        &wrCpssExMxPmQosPortDpToCfiMapEnableGet,
        2, 0},

    {"cpssExMxPmQosPortTrustEnableSet",
        &wrCpssExMxPmQosPortTrustEnableSet,
        4, 0},

    {"cpssExMxPmQosPortTrustConfigSet",
        &wrCpssExMxPmQosPortTrustConfigSet,
        6, 0},

    {"cpssExMxPmQosPortTrustConfigGet",
        &wrCpssExMxPmQosPortTrustGet,
        2, 0},

    {"cpssExMxPmQosPortDefaultTcSet",
        &wrCpssExMxPmQosPortDefaultTcSet,
        3, 0},

    {"cpssExMxPmQosPortDefaultTcGet",
        &wrCpssExMxPmQosPortDefaultTcGet,
        2, 0},

    {"cpssExMxPmQosPortDefaultUpSet",
        &wrCpssExMxPmQosPortDefaultUpSet,
        3, 0},

    {"cpssExMxPmQosPortDefaultUpGet",
        &wrCpssExMxPmQosPortDefaultUpGet,
        2, 0},

    {"cpssExMxPmQosPortDefaultDscpSet",
        &wrCpssExMxPmQosPortDefaultDscpSet,
        3, 0},

    {"cpssExMxPmQosPortDefaultDscpGet",
        &wrCpssExMxPmQosPortDefaultDscpGet,
        2, 0},

    {"cpssExMxPmQosPortDefaultDpSet",
        &wrCpssExMxPmQosPortDefaultDpSet,
        3, 0},

    {"cpssExMxPmQosPortDefaultDpGet",
        &wrCpssExMxPmQosPortDefaultDpGet,
        2, 0},

    {"cpssExMxPmQosPortDefaultExpSet",
        &wrCpssExMxPmQosPortDefaultExpSet,
        3, 0},

    {"cpssExMxPmQosPortDefaultExpGet",
        &wrCpssExMxPmQosPortDefaultExpGet,
        2, 0},

    {"cpssExMxPmQosPortQosPrecedenceSet",
        &wrCpssExMxPmQosPortQosPrecedenceSet,
        3, 0},

    {"cpssExMxPmQosPortQosPrecedenceGet",
        &wrCpssExMxPmQosPortQosPrecedenceGet,
        2, 0},

    {"cpssExMxPmQosPortModifyQoSParamEnableSet",
        &wrCpssExMxPmQosPortModifyQoSParamEnableSet,
        4, 0},

    {"cpssExMxPmQosPortModifyQoSParamEnableGet",
        &wrCpssExMxPmQosPortModifyQoSParamEnableGet,
        3, 0},

    {"cpssExMxPmQosPortReMapDscpEnableSet",
        &wrCpssExMxPmQosPortReMapDscpEnableSet,
        3, 0},

    {"cpssExMxPmQosPortReMapDscpEnableGet",
        &wrCpssExMxPmQosPortReMapDscpEnableGet,
        2, 0},

    {"cpssExMxPmQosPortReMapUpEnableSet",
        &wrCpssExMxPmQosPortReMapUpEnableSet,
        3, 0},

    {"cpssExMxPmQosPortReMapUpEnableGet",
        &wrCpssExMxPmQosPortReMapUpEnableGet,
        2, 0},

     {"cpssExMxPmQosMacQosEntrySet",
        &wrCpssExMxPmQosMacQosEntrySet,
        1, 13},

    {"cpssExMxPmQosMacQosEntryGetFirst",
        &wrCpssExMxPmQosMacQosEntryGetFirst,
        1, 0},

    {"cpssExMxPmQosMacQosEntryGetNext",
        &wrCpssExMxPmQosMacQosEntryGetNext,
        1, 0},

    {"cpssExMxPmQosMacQosConflictResolutionSet",
        &wrCpssExMxPmQosMacQosConflictResolutionSet,
        2, 0},

    {"cpssExMxPmQosMacQosConflictResolutionGet",
        &wrCpssExMxPmQosMacQosConflictResolutionGet,
        1, 0},

    {"cpssExMxPmQosUnkUcEnableSet",
        &wrCpssExMxPmQosUnkUcEnableSet,
        2, 0},

    {"cpssExMxPmQosUnkUcEnableGet",
        &wrCpssExMxPmQosUnkUcEnableGet,
        1, 0},

    {"cpssExMxPmQosUnregMcBcEnableSet",
        &wrCpssExMxPmQosUnregMcBcEnableSet,
        2, 0},

    {"cpssExMxPmQosUnregMcBcEnableGet",
        &wrCpssExMxPmQosUnregMcBcEnableGet,
        1, 0},

    {"cpssExMxPmQosAutoLearnQosProfileSet",
        &wrCpssExMxPmQosAutoLearnQosProfileSet,
        2, 0},

    {"cpssExMxPmQosAutoLearnQosProfileGet",
        &wrCpssExMxPmQosAutoLearnQosProfileGet,
        1, 0}

    /* CMD_END */

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmQos
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
GT_STATUS cmdLibInitCpssExMxPmQos
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


