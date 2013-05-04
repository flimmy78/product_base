/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* wrapSysconfig.c
*
* DESCRIPTION:
*       This file implements wrapper functions for the 'sysconfig' library;
*       It also defines the command database entries for the commander engine.
*
* DEPENDENCIES:
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cmdShell/common/cmdCommon.h>
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <galtisAgent/wrapUtil/cmdCpssSysConfig.h>

#include <cpss/generic/config/private/prvCpssConfigTypes.h>

/*******************************************************************************
 * External usage variables
 ******************************************************************************/
extern GT_INTFUNCPTR    gtInitSystemFuncPtr;


#if (defined ASIC_SIMULATION) && (! (defined RTOS_ON_SIM))
    #define USE_WIN_SYS_CONF_OUT_STR_CNS
    extern GT_CHAR winSysconfOutStr[/*100*/];
#endif

/* flag for debug to remove the hwDevNum convert*/
GT_BOOL gtHwDevNumConvert = GT_TRUE;

/*******************************************************************************
* mngInitSystem (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngInitSystem
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
    return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = mngInitSystem();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* mngGetSysFirstElement (general command)
*
* DESCRIPTION:
*     Returns the first element of the sysconf.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngGetSysFirstElement
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
        GT_STATUS result;

        /* check for valid arguments */
        if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

        /* call tapi api function */
        result = mngGetSysFirstElement();

        /* pack output arguments to galtis string */
        *outArgs = 0;
#ifdef USE_WIN_SYS_CONF_OUT_STR_CNS
        cmdOsStrCpy((GT_CHAR*)outArgs,winSysconfOutStr);
#endif /* USE_WIN_SYS_CONF_OUT_STR_CNS */
        return CMD_OK;
}

/*******************************************************************************
* mngGetSysNextElement (general command)
*
* DESCRIPTION:
*     Returns the next element of the sysconf.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngGetSysNextElement
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
    return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = mngGetSysNextElement();

    /* pack output arguments to galtis string */
    *outArgs = 0;
#ifdef USE_WIN_SYS_CONF_OUT_STR_CNS
    cmdOsStrCpy((GT_CHAR*)outArgs,winSysconfOutStr);
#endif /* USE_WIN_SYS_CONF_OUT_STR_CNS */
    return CMD_OK;
}

/*******************************************************************************
* sys (general command)
*
* DESCRIPTION:
*     Prints the sysconf elements.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrSys
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
    return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = sys();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

GT_U8 portDevMapArray[4][4];
/*******************************************************************************
* wrMngPortDevConvertSet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortDevConvertSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(inFields[0] > 3)
    {
        galtisOutput(outArgs, result, "\nIndex out of range 0..3\n");
        return CMD_OK;
    }

    portDevMapArray[inFields[0]][0] = (GT_U8)inFields[1];
    portDevMapArray[inFields[0]][1] = (GT_U8)inFields[2];
    portDevMapArray[inFields[0]][2] = (GT_U8)inFields[3];
    portDevMapArray[inFields[0]][3] = (GT_U8)inFields[4];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrMngPortDevConvertGet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_U32 tableIndex;
static CMD_STATUS wrMngPortDevConvertGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tableIndex = 0;

    inFields[0] = tableIndex;
    inFields[1] = (GT_U32)portDevMapArray[0][0];
    inFields[2] = (GT_U32)portDevMapArray[0][1];
    inFields[3] = (GT_U32)portDevMapArray[0][2];
    inFields[4] = (GT_U32)portDevMapArray[0][3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

static CMD_STATUS wrMngPortDevConvertGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tableIndex++;
    if(tableIndex == 4)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = tableIndex;
    inFields[1] = (GT_U32)portDevMapArray[tableIndex][0];
    inFields[2] = (GT_U32)portDevMapArray[tableIndex][1];
    inFields[3] = (GT_U32)portDevMapArray[tableIndex][2];
    inFields[4] = (GT_U32)portDevMapArray[tableIndex][3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* wrMngPortDevConvertSet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortDevConvertClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(portDevMapArray, 0, sizeof(portDevMapArray));

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


GT_VOID gtPortsDevConvertDataBack
(
   INOUT GT_U8 *devNum,
   INOUT GT_U8 *portNum
)
{
    GT_U32 i;
    GT_U8   hwDevNum = *devNum;

    if(GT_TRUE == gtHwDevNumConvert)
    {
        /* first we need to find the SW device that hold the hwDevNum */
        for(i = 0 ;i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
        {
            if(prvCpssPpConfig[i] == NULL)
            {
                continue;
            }

            if(PRV_CPSS_HW_DEV_NUM_MAC(i) == hwDevNum)
            {
                /* this is the 'sw_devNum' we look for */
                *devNum = (GT_U8) i;
                break;
            }
        }

        /* if we got here and i == PRV_CPSS_MAX_PP_DEVICES_CNS , meaning that the
           HW-devNum not belongs to any of the cpss devices , so we assume 'remote device'
           and we can't convert it to 'sw-devNum' so we will use it 'as is' */
    }

    for(i=0 ; i<4 ; i++)
    {
        if((portDevMapArray[i][2] == (*devNum)) && (portDevMapArray[i][3] == (*portNum)))
        {
            *devNum = portDevMapArray[i][0];
            *portNum = portDevMapArray[i][1];
            break;
        }
    }
}

GT_VOID gtPortsBitmapDevConvert
(
   INOUT GT_U8              *devNum,
   INOUT CPSS_PORTS_BMP_STC *portBitMap
)
{
    GT_U32 i, j;
    CPSS_PORTS_BMP_STC localPortBitMap = {{0,0}};
    GT_U8   localDevNum = *devNum;

    for( i = 0 ; i < PRV_CPSS_MAX_PP_PORTS_NUM_CNS ; i++ )
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(portBitMap,i))
        {
            for ( j = 0 ; j < 4 ; j++ )
            {
                if ( (portDevMapArray[j][0] == localDevNum) && (portDevMapArray[j][1] == i) )
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(&localPortBitMap,portDevMapArray[j][3]);

                    *devNum = portDevMapArray[j][0];

                    break;
                }
            }

            if ( 4 == j)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(&localPortBitMap,i);
            }
        }
    }

    *portBitMap = localPortBitMap;
}

GT_VOID gtPortsBitmapDevConvertDataBack
(
   INOUT GT_U8              *devNum,
   INOUT CPSS_PORTS_BMP_STC *portBitMap
)
{
    GT_U32 i, j;
    CPSS_PORTS_BMP_STC localPortBitMap = {{0,0}};
    GT_U8   localDevNum = *devNum;

    if(GT_TRUE == gtHwDevNumConvert)
    {
        /* first we need to find the SW device that hold the hwDevNum */
        for(i = 0 ;i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
        {
            if(prvCpssPpConfig[i] == NULL)
            {
                continue;
            }

            if(PRV_CPSS_HW_DEV_NUM_MAC(i) == localDevNum)
            {
                /* this is the 'sw_devNum' we look for */
                localDevNum = (GT_U8) i;
                break;
            }
        }

        /* if we got here and i == PRV_CPSS_MAX_PP_DEVICES_CNS , meaning that the
           HW-devNum not belongs to any of the cpss devices , so we assume 'remote device'
           and we can't convert it to 'sw-devNum' so we will use it 'as is' */
    }

    for( i = 0 ; i < PRV_CPSS_MAX_PP_PORTS_NUM_CNS ; i++ )
    {
        if ((!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(localDevNum, i))
            && (i != CPSS_CPU_PORT_NUM_CNS))
            continue;

        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(portBitMap,i))
        {
                    /*CPSS_PORTS_BMP_PORT_SET_MAC(localPortBitMap,portDevMapArray[j][3]);*/
            for ( j = 0 ; j < 4 ; j++ )
            {
                if ( (portDevMapArray[j][2] == localDevNum) && (portDevMapArray[j][3] == i) )
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(&localPortBitMap,portDevMapArray[j][1]);

                    *devNum = portDevMapArray[j][0];

                    break;
                }
            }

            if ( 4 == j)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(&localPortBitMap,i);
            }
        }
        else
        {
            for ( j = 0 ; j < 4 ; j++ )
            {
                if ( (portDevMapArray[j][2] == localDevNum) && (portDevMapArray[j][3] == i) )
                {
                    CPSS_PORTS_BMP_PORT_CLEAR_MAC(&localPortBitMap,portDevMapArray[j][1]);

                    *devNum = portDevMapArray[j][0];

                    break;
                }
            }
        }
    }

    *portBitMap = localPortBitMap;
}

/* enable/disable the hwDevNum convert .
function sets the flag of gtHwDevNumConvert */
GT_STATUS gtHwDevNumConvertSet
(
   IN GT_BOOL enableConvert
)
{
    gtHwDevNumConvert = enableConvert;
    return GT_OK;
}

/*******************************************************************************
* cpssInitSystem
*
* DESCRIPTION:
*       This is the main board initialization function.
*
* INPUTS:
*       boardIdx      - The index of the board to be initialized from the board
*                       list.
*       boardRevId    - Board revision Id.
*       reloadEeprom  - Whether the Eeprom should be reloaded when
*                       corePpHwStartInit() is called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CMD_STATUS wrCpssInitSystem
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     GT_STATUS result;
     GT_U32  boardIdx;
     GT_U32  boardRevId;
     GT_U32  reloadEeprom;

   /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    boardIdx=(GT_U32)inArgs[0];
    boardRevId=(GT_U32)inArgs[1];
    reloadEeprom=(GT_U32)inArgs[2];

    result = cmdInitSystem(boardIdx,boardRevId,reloadEeprom);
   /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/*******************************************************************************
* confi2InitSystem
*
* DESCRIPTION:
*       This is the main board initialization function.
*
* INPUTS:
*       theConfiId  - the confi ID
*       echoOn      - if GT_TRUE the echo is on, otherwise the echo is off.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrConfi2InitSystem
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    IN GT_U8        theConfiId;
    IN GT_BOOL      echoOn;
    GT_STATUS result;
      /* check for valid arguments */
       if (!inArgs || !outArgs)
           return CMD_AGENT_ERROR;

       theConfiId=(GT_U8)inArgs[0];
       if ((GT_U32)inArgs[1] == 0)
       {
           echoOn = GT_FALSE;
       }
       else
       {
           echoOn = GT_TRUE;
       }
       result = cmdInitConfi(theConfiId,echoOn);
      /* pack output arguments to galtis string */
       galtisOutput(outArgs, result, "");
       return CMD_OK;

}
GT_U8 portGroupDevMapArray[4][4];
/*******************************************************************************
* wrMngPortGroupConvertSet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupConvertSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(inFields[0] > 3)
    {
        galtisOutput(outArgs, result, "\nIndex out of range 0..3\n");
        return CMD_OK;
    }

    portGroupDevMapArray[inFields[0]][0] = (GT_U8)inFields[1];
    portGroupDevMapArray[inFields[0]][1] = (GT_U8)inFields[2];
    portGroupDevMapArray[inFields[0]][2] = (GT_U8)inFields[3];
    portGroupDevMapArray[inFields[0]][3] = (GT_U8)inFields[4];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrMngPortGroupConvertGet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupConvertGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tableIndex = 0;

    inFields[0] = tableIndex;
    inFields[1] = (GT_U32)portGroupDevMapArray[0][0];
    inFields[2] = (GT_U32)portGroupDevMapArray[0][1];
    inFields[3] = (GT_U32)portGroupDevMapArray[0][2];
    inFields[4] = (GT_U32)portGroupDevMapArray[0][3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

static CMD_STATUS wrMngPortGroupConvertGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tableIndex++;
    if(tableIndex == 4)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = tableIndex;
    inFields[1] = (GT_U32)portGroupDevMapArray[tableIndex][0];
    inFields[2] = (GT_U32)portGroupDevMapArray[tableIndex][1];
    inFields[3] = (GT_U32)portGroupDevMapArray[tableIndex][2];
    inFields[4] = (GT_U32)portGroupDevMapArray[tableIndex][3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* wrMngPortGroupConvertClear (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupConvertClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(portGroupDevMapArray, 0, sizeof(portGroupDevMapArray));

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/* convert from
    devNum,portGroupsBmp    of test values  (received from Galtis/RDE test)

    to

    devNum,portGroupsBmp    of CPSS values
*/
GT_VOID gtPortGroupDevConvert
(
   INOUT GT_U8 *devNum,
   INOUT GT_PORT_GROUPS_BMP *portGroupsBmpPtr
)
{
    GT_U32 i;

    for(i=0 ; i<4 ; i++)
    {
        if((portGroupDevMapArray[i][0] == (*devNum)) && (portGroupDevMapArray[i][1] == (*portGroupsBmpPtr)))
        {
            *devNum = portGroupDevMapArray[i][2];
            *portGroupsBmpPtr = portGroupDevMapArray[i][3];
            break;
        }
    }
}

/* convert from
    devNum,portGroupsBmp    of CPSS values

    to

    devNum,portGroupsBmp    of test values (to send to Galtis/RDE test)
*/
GT_VOID gtPortGroupDevConvertBack
(
   INOUT GT_U8 *devNum,
   INOUT GT_PORT_GROUPS_BMP *portGroupsBmpPtr
)
{
    GT_U32 i;

    for(i=0 ; i<4 ; i++)
    {
        if((portGroupDevMapArray[i][2] == (*devNum)) && (portGroupDevMapArray[i][3] == (*portGroupsBmpPtr)))
        {
            *devNum = portGroupDevMapArray[i][0];
            *portGroupsBmpPtr = portGroupDevMapArray[i][1];
            break;
        }
    }
}

/*******************************************************************************
* wrMngPortGroupsBmpEnable (general command)
*
* DESCRIPTION:
*     enable/disable usage of API with new portGroupsBmp parameter
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupsBmpEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc;
    GT_U8           devNum;
    GT_BOOL         enable,tmpEnable;
    GT_PORT_GROUPS_BMP  portGroupsBmp;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    rc = utilMultiPortGroupsBmpGet(devNum,&tmpEnable,&portGroupsBmp);

    enable = (GT_BOOL)inArgs[1];

    rc = utilMultiPortGroupsBmpSet(devNum,enable,portGroupsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrMngPortGroupsBmpSet (general command)
*
* DESCRIPTION:
*     Set the 'portGroupsBmp'
*
Arguments:
1.         GT_BOOL         unawareMode    -- True ' all port groups (unaware mode) , and we ignore all other parameters (portGroupId_0..7)
                                                               False ' use info in portGroupId_0..7
2.         GT_BOOL         portGroupId_0    - True ' set that port group Id 0 need to be in the bitmap of portGroupsBmp
                                                              False ' port group Id 0 NOT need to be in the bitmap of portGroupsBmp
3.         GT_BOOL         portGroupId_1 . Similar to portGroupId_0 but refer to port group Id 1
4.         GT_BOOL         portGroupId_2 . Similar to portGroupId_0 but refer to port group Id 2
5.         GT_BOOL         portGroupId_3 . Similar to portGroupId_0 but refer to port group Id 3
6.         GT_BOOL         portGroupId_4 . Similar to portGroupId_0 but refer to port group Id 4
7.         GT_BOOL         portGroupId_5 . Similar to portGroupId_0 but refer to port group Id 5
8.         GT_BOOL         portGroupId_6 . Similar to portGroupId_0 but refer to port group Id 6
9.         GT_BOOL         portGroupId_7 . Similar to portGroupId_0 but refer to port group Id 7
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupsBmpSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc;
    GT_BOOL         unawareMode;
    GT_U32          ii = 0;
    GT_U8           devNum;
    GT_U32          startIndex;
    GT_BOOL         enable;
    GT_PORT_GROUPS_BMP  portGroupsBmp,tmpBmp;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[ii++];
    unawareMode = (GT_BOOL)inArgs[ii++];

    if(unawareMode == GT_TRUE)
    {
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }
    else
    {
        portGroupsBmp = 0;

        startIndex = ii;

        for(/* continue ii*/ ; ii < (startIndex + 8) ; ii++)
        {
            if(((GT_BOOL)inArgs[ii]) == GT_TRUE)
            {
                portGroupsBmp |= 1 << (ii - startIndex);
            }
        }
    }

    rc = utilMultiPortGroupsBmpGet(devNum,&enable,&tmpBmp);

    rc = utilMultiPortGroupsBmpSet(devNum,enable,portGroupsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrMngPortGroupsBmpSet (general command)
*
* DESCRIPTION:
*     Get the 'portGroupsBmp'
*
Arguments:
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupsBmpGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc;
    GT_U8           devNum;
    GT_BOOL         enable;
    GT_PORT_GROUPS_BMP  portGroupsBmp;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    rc = utilMultiPortGroupsBmpGet(devNum,&enable,&portGroupsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d",enable,portGroupsBmp);
    return CMD_OK;
}

/*******************************************************************************
* allowProcessingOfAuqMessages (general command)
*
* DESCRIPTION:
*     Function to allow set the flag of : allowProcessingOfAuqMessages
*
* INPUTS:
*       enable - GT_TRUE: allow the processing of the AUQ messages
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrAllowProcessingOfAuqMessages
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc;
    GT_BOOL         enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    enable = (GT_BOOL)inArgs[0];

    rc = cmdAppAllowProcessingOfAuqMessages(enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* gtIsPortsMapConvert
*
* DESCRIPTION:
*       Do ports mapping.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       none
*
* COMMENTS:
*       used by npd, cpss1.3
*
*******************************************************************************/
void gtPortsDevConvert
(
   INOUT GT_U8 *devNum,
   INOUT GT_U8 *portNum
)
{
    GT_U32 i;

    for(i=0 ; i<4 ; i++)
    {
        if((portDevMapArray[i][0] == (*devNum)) && (portDevMapArray[i][1] == (*portNum)))
        {
            *devNum = portDevMapArray[i][2];
            *portNum = portDevMapArray[i][3];
            break;
        }
    }
}

void gtPortsDevConvertBack
(
   INOUT GT_U8 *devNum,
   INOUT GT_U8 *portNum
)
{
    GT_U32 i;

    for(i=0 ; i<4 ; i++)
    {
        if((portDevMapArray[i][2] == (*devNum)) && (portDevMapArray[i][3] == (*portNum)))
        {
            *devNum = portDevMapArray[i][0];
            *portNum = portDevMapArray[i][1];
            break;
        }
    }
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] = {

    {"mngInitSystem",
        &wrMngInitSystem,
        0, 0},

    {"mngGetSysFirstElement",
        &wrMngGetSysFirstElement,
        0, 0},

    {"mngGetSysNextElement",
        &wrMngGetSysNextElement,
        0, 0},

    {"sys",
        &wrSys,
        0, 0},


    {"mngPortDevConvertSet",
     &wrMngPortDevConvertSet,
     0, 5},

    {"mngPortDevConvertGetFirst",
     &wrMngPortDevConvertGetFirst,
     0, 0},

    {"mngPortDevConvertGetNext",
     &wrMngPortDevConvertGetNext,
     0, 0},

    {"mngPortDevConvertClear",
     &wrMngPortDevConvertClear,
     0, 0},

    /* start table mngPortGroupConvert */
    {"mngPortGroupConvertSet",
     &wrMngPortGroupConvertSet,
     0, 5},

    {"mngPortGroupConvertGetFirst",
     &wrMngPortGroupConvertGetFirst,
     0, 0},

    {"mngPortGroupConvertGetNext",
     &wrMngPortGroupConvertGetNext,
     0, 0},

    {"mngPortGroupConvertClear",
     &wrMngPortGroupConvertClear,
     0, 0},
    /* end table mngPortGroupConvert */

    {"mngPortGroupsBmpEnable",
        &wrMngPortGroupsBmpEnable,
        2, 0},

    {"mngPortGroupsBmpSet",
        &wrMngPortGroupsBmpSet,
        10, 0},

    {"mngPortGroupsBmpGet",
        &wrMngPortGroupsBmpGet,
        1, 0},

    {"allowProcessingOfAuqMessages",
        &wrAllowProcessingOfAuqMessages,
        1, 0},


    {"wrCpssInitSystem",
     &wrCpssInitSystem,
     3, 0},
    {"confi2InitSystem",
     &wrConfi2InitSystem,
     2, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitSysconfig
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
GT_STATUS cmdCpssLibInitSysconfig()
{
    return cmdInitLibrary(dbCommands, numCommands);
}

