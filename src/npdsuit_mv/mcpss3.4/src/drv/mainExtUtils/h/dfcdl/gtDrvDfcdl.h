/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDrvDfcdl.h
*
* DESCRIPTION:
*       This file includes functions for PP DFCDL interface.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __gtDriverDfcdlh
#define __gtDriverDfcdlh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <dfcdl/gtDfcdl.h>

#define SDL_PP_NSRAM_SIZE  64  /* DFCDL SDL size of PP Narrow SRAM */
#define SDL_PP_C2C_SIZE    64  /* DFCDL SDL size of PP Uplink */

typedef GT_STATUS (*FUNC_DFCDL_FIELD_SET)(GT_U8, GT_U8);

/*******************************************************************************
* dfcdlPpNSramFbScan
*
* DESCRIPTION:
*       This function tests a range of narrow sram feedback value.
*
* INPUTS:
*
*       devNum        - device number.
*       dfcdlStart    - The dfcdl value to start scan
*       dfcdlEnd      - The dfcdl value to end scan
*       patternType   - memory test profile
*
* OUTPUTS:
*       result - DFCDL values are valid for reliable interface timing.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number, pattern type or
*                      DFCDL values are illegal
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpNSramFbScan
(
    IN GT_U8                devNum,
    IN GT_U8                dfcdlStart,
    IN GT_U8                dfcdlEnd,
    IN GT_MEM_TEST_PROFILE  patternType,
    OUT GT_BOOL             result[DFCDL_MAX_DELAY]
);


/*******************************************************************************
* dfcdlPpNSramFbSet
*
* DESCRIPTION:
*       This function sets narrow sram feedback value in all dfcdl Table
*       The value is saved in shadow and used as default value other scans.
*
* INPUTS:
*
*       devNum  - The device number to set
*       delay   - value to set in narrow sram feedback table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number or
*                      DFCDL values are illegal
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpNSramFbSet
(
    IN GT_U8              devNum,
    IN GT_U8              delay
);

/*******************************************************************************
* dfcdlPpWSramFbSet
*
* DESCRIPTION:
*       This function sets wide sram feedback value in all dfcdl Table
*       The value is saved in shadow and used as default value other scans.
*
* INPUTS:
*
*       devNum  - The device number to set
*       delay   - value to set in wide sram feedback table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number or
*                      DFCDL values are illegal
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpWSramFbSet
(
    IN GT_U8              devNum,
    IN GT_U8              delay
);

/*******************************************************************************
* dfcdlPpSramClkSet
*
* DESCRIPTION:
*       This function sets sram clock value in dfcdl Table
*       The value is saved in shadow and used as default value other scans.
*
* INPUTS:
*
*       devNum - The device number to set
*       delay - value to set in sram clock table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number or
*                      DFCDL values are illegal
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS  dfcdlPpSramClkSet
(
    IN GT_U8              devNum,
    IN GT_U8              delay
);

/*******************************************************************************
* dfcdlPpDqsSet
*
* DESCRIPTION:
*       This function sets DQS delay value in buffer SDRAM DFCDL table (PP).
*       The value is saved in shadow and used as default value.
*
* INPUTS:
*
*       devNum - The device number to set
*       dqsDel - value to set in dram table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number or
*                      DFCDL values are illegal
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpDqsSet
(
    IN GT_U8              devNum,
    IN GT_U8              dqsDel
);

/*******************************************************************************
* dfcdlPpDqsScan
*
* DESCRIPTION:
*       This function tests a range of DQS delay value in buffer SDRAM DFCDL
*       table (PP).
*       The function performs DQS DFCDL value scans between 2 values and
*       returns to the user on what DFCDL values are valid for reliable
*       interface timing.
*
* INPUTS:
*
*       devNum      - device number.
*       dfcdlStart    - The dfcdl value to start scan.
*       dfcdlEnd      - The dfcdl value to end scan.
*       patternType   - memory test profile
*
* OUTPUTS:
*       result - DFCDL values are valid for reliable interface timing.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number, pattern type or
*                      DFCDL values are illegal
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpDqsScan
(
    IN GT_U8                devNum,
    IN GT_U8                dfcdlStart,
    IN GT_U8                dfcdlEnd,
    IN GT_MEM_TEST_PROFILE  patternType,
    OUT GT_BOOL             result[DFCDL_MAX_DELAY]
);

/*******************************************************************************
* dfcdlPpClkOutSet
*
* DESCRIPTION:
*       This function sets TX Clock delay value in buffer SDRAM DFCDL table.
*       The value is saved in shadow and used as default value.
*
* INPUTS:
*
*       devNum - The device number to set
*       clkOutDel - value to set in dram table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpClkOutSet
(
    IN GT_U8              devNum,
    IN GT_U8              clkOutDel
);

/*******************************************************************************
* svDfcdlClkOutScan
*
* DESCRIPTION:
*       This function tests a range of TX clock delay value in buffer SDRAM
*       DFCDL table (PP).
*
* INPUTS:
*
*       devNum      - device number.
*       dfcdlStart    - The dfcdl value to start scan
*       dfcdlEnd      - The dfcdl value to end scan
*       patternType   - memory test profile
*
* OUTPUTS:
*       result - DFCDL values are valid for reliable interface timing.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number, pattern type or
*                      DFCDL values are illegal
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpClkOutScan
(
    IN GT_U8                devNum,
    IN GT_U8                dfcdlStart,
    IN GT_U8                dfcdlEnd,
    IN GT_MEM_TEST_PROFILE  patternType,
    OUT GT_BOOL             result[DFCDL_MAX_DELAY]
);

/*******************************************************************************
* dfcdlPpRxClkSet
*
* DESCRIPTION:
*       This function sets RX Clock delay value in buffer SDRAM DFCDL table.
*       The value is saved in shadow and used as default value.
*
* INPUTS:
*
*       devNum - The device number to set
*       rxClkDel - value to set in dram table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpRxClkSet
(
    IN GT_U8              devNum,
    IN GT_U8              rxClkDel
);

/*******************************************************************************
* dfcdlPpRxClkScan
*
* DESCRIPTION:
*       This function tests a range of TX clock delay value in buffer SDRAM
*       DFCDL table (PP).
*
* INPUTS:
*       devNum      - device number.
*       dfcdlStart    - The dfcdl value to start scan
*       dfcdlEnd      - The dfcdl value to end scan
*       patternType   - memory test profile
*
* OUTPUTS:
*       result - DFCDL values are valid for reliable interface timing.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number, pattern type or
*                      DFCDL values are illegal
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpRxClkScan
(
    IN GT_U8                devNum,
    IN GT_U8                dfcdlStart,
    IN GT_U8                dfcdlEnd,
    IN GT_MEM_TEST_PROFILE  patternType,
    OUT GT_BOOL             result[DFCDL_MAX_DELAY]
);

/*******************************************************************************
* dfcdlPpDramFindEntry
*
* DESCRIPTION:
*       This function finds values in the buffer SDRAM DFCDL table (PP).
*
* INPUTS:
*       devNum            - device number.
*       dfcdlDqsValue     - the DQS DFCDL value to be used in the SDL table
*       dfcdlClkOutValue  - the clock out DFCDL value to be used
*       dfcdlRxClkValue   - the Rx clock DFCDL value to be used
*       loopCounter       - number of iterations to be used
*
* OUTPUTS:
*       entry - the SDL DFCDL entry
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number, loop counter or DFCDL values
*                      are illegal or
*                      if pointer to entry is NULL
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpDramFindEntry
(
    IN  GT_U8  devNum,
    IN  GT_U8  dfcdlDqsValue,
    IN  GT_U8  dfcdlClkOutValue,
    IN  GT_U8  dfcdlRxClkValue,
    IN  GT_U8  loopCounter,
    OUT GT_U8* entry
);

/*******************************************************************************
* dfcdlPpWc2cSet
*
* DESCRIPTION:
*       This function sets wide C2C delay value in Uplink DFCDL table (PP).
*       The value is saved in shadow and used as default value.
*
* INPUTS:
*
*       devNum - The device number to set
*       c2cDelay - value to set in Wide C2c table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpWc2cSet
(
    IN GT_U8             devNum,
    IN GT_U8             c2cDelay
);

/*******************************************************************************
* dfcdlPpWc2cScan
*
* DESCRIPTION:
*       This function tests the range of wide C2C delay value in Uplink
*       DFCDL table (PP).
*
* INPUTS:
*
*       devNum      - device number.
*       c2cStart    - The c2cDelay to start scan
*       c2cEnd      - The c2cDelay to end scan
*       waitTime    - wait time defined before checking interrupt count.
*
* OUTPUTS:
*       c2cStatus   - array of C2C statuses
*       eccCount    - array of counters for number of ECC interrupts.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number, wait time or DFCDL values are illegal
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpWc2cScan
(
    IN GT_U8    devNum,
    IN GT_U8    c2cStart,
    IN GT_U8    c2cEnd,
    IN GT_U32   waitTime,
    OUT GT_BOOL c2cStatus[DFCDL_MAX_DELAY],
    OUT GT_U32  eccCount[DFCDL_MAX_DELAY]
);

/*******************************************************************************
* dfcdlPpNc2cSet
*
* DESCRIPTION:
*       This function sets narrow c2c Delay value in DFCDL Table.
*       The value is saved in shadow and used as default value.
*
* INPUTS:
*
*       devNum - The device number to set
*       c2cDelay - value to set in Narrow C2C table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpNc2cSet
(
    IN GT_U8             devNum,
    IN GT_U8             c2cDelay
);

/*******************************************************************************
* dfcdlPpNc2cScan
*
* DESCRIPTION:
*       This function tests a range of narrow c2c dfcdl values .
*       The Narrow ECC errors interrupts count is print out after
*       a pre-defined wait time defined by the 'testDelay' parameter.
*
* INPUTS:
*
*       devNum      - device number.
*       c2cStart    - The c2cDelay to start scan
*       c2cEnd      - The c2cDelay to end scan
*       waitTime    - wait time defined before checking interrupt count.
*
* OUTPUTS:
*       c2cStatus   - array of C2C statuses
*       eccCount    - array of counters for number of ECC interrupts.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number, wait time or DFCDL values are illegal
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpNc2cScan
(
    IN GT_U8    devNum,
    IN GT_U8    c2cStart,
    IN GT_U8    c2cEnd,
    IN GT_U32   waitTime,
    OUT GT_BOOL c2cStatus[DFCDL_MAX_DELAY],
    OUT GT_U32  eccCount[DFCDL_MAX_DELAY]
);

/*******************************************************************************
* dfcdlPpC2cFindEntry
*
* DESCRIPTION:
*       This function finds values in the buffer SDRAM DFCDL table (PP).
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       entry - the c2c DFCDL entry
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number, loop counter or DFCDL values
*                      are illegal or
*                      if pointer to entry is NULL
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dfcdlPpC2cFindEntry
(
    IN  GT_U8  devNum,
    OUT GT_U8* entry
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtDriverDfcdlh */

