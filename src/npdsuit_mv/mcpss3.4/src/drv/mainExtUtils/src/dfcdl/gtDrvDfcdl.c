/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDrvDfcdl.c
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

#include <dfcdl/gtDrvDfcdl.h>
#include <dfcdl/private/gtDfcdlPrv.h>
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/events/cpssGenEventRequests.h>
#include <cpss/generic/diag/private/prvCpssGenDiag.h>

GT_U32 sdlPpNarrowShadow = 0;  /* shadow of PP Flow SRAM SDL */
GT_U32 sdlPpBufferShadow = 0;  /* shadow of PP Buffer SDRAM SDL */

/* task id for wide C2C scan */
static CPSS_TASK wideC2cScanTaskId;

/* task id for narrow C2C scan */
static CPSS_TASK narrowC2cScanTaskId;

/* handle for wide uplink interface ECC errors */
static GT_U32  wideC2cEvReqHndl = 0;

/* handle for narrow uplink interface ECC errors */
static GT_U32  narrowC2cEvReqHndl = 0;

static GT_STATUS dfcdlPpScan
(
    IN GT_U8                devNum,
    IN GT_U8                dfcdlStart,
    IN GT_U8                dfcdlEnd,
    IN GT_MEM_TEST_PROFILE  patternType,
    IN FUNC_DFCDL_FIELD_SET setDfcdlField,
    IN GT_U32               memBase,
    IN GT_U32               length,
    OUT GT_BOOL             result[DFCDL_MAX_DELAY]
);

static GT_STATUS dfcdlRamVerify
(
    IN GT_U8                devNum,
    IN GT_U32               memBase,
    IN GT_U32               length,
    IN GT_MEM_TEST_PROFILE  patternType
);

GT_STATUS __TASKCONV wideC2cScanTaskFunc
(
    GT_VOID
);

GT_STATUS __TASKCONV narrowC2cScanTaskFunc
(
    GT_VOID
);

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
*       dfcdlEnd      - The dfcdl value to end scan (included)
*       patternType   - memory test profile
*
* OUTPUTS:
*       result - DFCDL values are valid for reliable interface timing.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - if device number, pattern type or
*                      DFCDL values are illegal
*       GT_FAIL      - on error
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
)
{
    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    return dfcdlPpScan(devNum, dfcdlStart, dfcdlEnd, patternType,
                       dfcdlPpNSramFbSet, 0x30000000, 256, result);
}

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
)
{
    GT_U32 sdlValue;
    GT_U32 addr;
    GT_U32 i;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    if (delay >= DFCDL_MAX_DELAY)
    {
        return GT_BAD_PARAM;
    }

    sdlValue = sdlPpNarrowShadow;
    U32_SET_FIELD_MAC(sdlValue, 12, 6, delay);

    /* set adrress register to first sdl entry */
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srAddressReg;
    if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, 0) != GT_OK)
    {
        return GT_FAIL;
    }

    /* fill table with delay value*/
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srDataReg;
    for (i = 0; i < SDL_PP_NSRAM_SIZE; i++)
    {
        if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, sdlValue) != GT_OK)
        {
            return GT_FAIL;
        }
    }

    sdlPpNarrowShadow = sdlValue;

    return GT_OK;
}

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
)
{
    GT_U32 sdlValue;
    GT_U32 addr;
    GT_U32 i;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    if (delay >= DFCDL_MAX_DELAY)
    {
        return GT_BAD_PARAM;
    }

    sdlValue = sdlPpNarrowShadow;
    U32_SET_FIELD_MAC(sdlValue, 6, 6, delay);

    /* set adrress register to first sdl entry */
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srAddressReg;
    if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, 0) != GT_OK)
    {
        return GT_FAIL;
    }

    /* fill table with delay value*/
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srDataReg;
    for (i = 0; i < SDL_PP_NSRAM_SIZE; i++)
    {
        if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, sdlValue) != GT_OK)
        {
            return GT_FAIL;
        }
    }

    sdlPpNarrowShadow = sdlValue;

    return GT_OK;
}
/*******************************************************************************
* dfcdlPpSramClkSet
*
* DESCRIPTION:
*       This function sets sram clock out delay value in dfcdl Table
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
)
{
    GT_U32 sdlValue;
    GT_U32 addr;
    GT_U32 i;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    if (delay >= DFCDL_MAX_DELAY)
    {
        return GT_BAD_PARAM;
    }

    sdlValue = sdlPpNarrowShadow;
    U32_SET_FIELD_MAC(sdlValue, 0, 6, delay);

    /* set adrress register to first sdl entry */
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srAddressReg;
    if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, 0) != GT_OK)
    {
        return GT_FAIL;
    }

    /* fill table with delay value*/
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srDataReg;
    for (i = 0; i < SDL_PP_NSRAM_SIZE; i++)
    {
        if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, sdlValue) != GT_OK)
        {
            return GT_FAIL;
        }
    }

    sdlPpNarrowShadow = sdlValue;

    return GT_OK;
}


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
)
{
    GT_U32 sdlValue;
    GT_U32 addr;
    GT_U32 i;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    if (dqsDel >= DFCDL_MAX_DELAY)
    {
        return GT_BAD_PARAM;
    }

    sdlValue = sdlPpBufferShadow;
    U32_SET_FIELD_MAC(sdlValue, 0, 6, dqsDel);

    /* set adrress register to first sdl entry */
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdAddressReg;
    if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, 0) != GT_OK)
    {
        return GT_FAIL;
    }

    /* fill table with delay value*/
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdDataReg;
    for (i = 0; i < SDL_PP_SDRAM_SIZE; i++)
    {
        if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, sdlValue) != GT_OK)
        {
            return GT_FAIL;
        }
    }

    sdlPpBufferShadow = sdlValue;
    return GT_OK;
}


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
)
{
    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    return dfcdlPpScan(devNum, dfcdlStart, dfcdlEnd, patternType,
                       dfcdlPpDqsSet, 0x20000000, 512, result);
}


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
)
{
    GT_U32 sdlValue;
    GT_U32 addr;
    GT_U32 i;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    if (clkOutDel >= DFCDL_MAX_DELAY)
    {
        return GT_BAD_PARAM;
    }

    sdlValue = sdlPpBufferShadow;
    U32_SET_FIELD_MAC(sdlValue, 6, 6, clkOutDel);

    /* set adrress register to first sdl entry */
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdAddressReg;
    if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, 0) != GT_OK)
    {
        return GT_FAIL;
    }

    /* fill table with delay value*/
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdDataReg;
    for (i = 0; i < SDL_PP_SDRAM_SIZE; i++)
    {
        if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, sdlValue) != GT_OK)
        {
            return GT_FAIL;
        }
    }

    sdlPpBufferShadow = sdlValue;

    return GT_OK;
}


/*******************************************************************************
* dfcdlPpClkOutScan
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
)
{
    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    return dfcdlPpScan(devNum, dfcdlStart, dfcdlEnd, patternType,
                       dfcdlPpClkOutSet, 0x20000000, 512, result);
}


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
)
{
    GT_U32 sdlValue;
    GT_U32 addr;
    GT_U32 i;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    if (rxClkDel >= DFCDL_MAX_DELAY)
    {
        return GT_BAD_PARAM;
    }

    sdlValue = sdlPpBufferShadow;
    U32_SET_FIELD_MAC(sdlValue, 12, 6, rxClkDel);

    /* set adrress register to first sdl entry */
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdAddressReg;
    if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, 0) != GT_OK)
    {
        return GT_FAIL;
    }

    /* fill table with delay value*/
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdDataReg;
    for (i = 0; i < SDL_PP_SDRAM_SIZE; i++)
    {
        if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, sdlValue) != GT_OK)
        {
            return GT_FAIL;
        }
    }

    sdlPpBufferShadow = sdlValue;

    return GT_OK;
}


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
)
{
    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    return dfcdlPpScan(devNum, dfcdlStart, dfcdlEnd, patternType,
                       dfcdlPpRxClkSet, 0x20000000, 512, result);
}


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
)
{
    GT_U32 sdlValue;
    GT_U32 addr;
    GT_U8  i;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entry);

    if ((dfcdlDqsValue >= DFCDL_MAX_DELAY) ||
        (dfcdlClkOutValue >= DFCDL_MAX_DELAY) ||
        (dfcdlRxClkValue >= DFCDL_MAX_DELAY))
    {
        return GT_BAD_PARAM;
    }

    /* set adrress register to first sdl entry */
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdAddressReg;
    if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, 0) != GT_OK)
    {
        return GT_FAIL;
    }

    *entry = SDL_PP_SDRAM_SIZE;
    sdlValue = sdlPpBufferShadow | (dfcdlDqsValue & 0x3f) |
               ((GT_U32)(dfcdlClkOutValue & 0x3f) << 6) |
               ((GT_U32)(dfcdlRxClkValue & 0x3f) << 12);

    /* fill table with delay value*/
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.bdDataReg;
    /* test each entry */
    for (i = 0; i < loopCounter; i++)
    {
        if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, sdlValue) != GT_OK)
        {
            return GT_FAIL;
        }

        /* set entry number */
        if (dfcdlRamVerify(devNum, 0x20000000, 256, GT_AA_55) != GT_OK)
        {
            *entry = i;
            return GT_OK;
        }
    }

    return GT_OK;
}


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
)
{
    GT_U32 sdlValue;
    GT_U32 addr;
    GT_U32 i;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    if (c2cDelay >= DFCDL_MAX_DELAY)
    {
        return GT_BAD_PARAM;
    }

    sdlValue = sdlPpNarrowShadow;
    U32_SET_FIELD_MAC(sdlValue, 24, 6, c2cDelay);

    /* set adrress register to first sdl entry */
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srAddressReg;
    if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, 0) != GT_OK)
    {
        return GT_FAIL;
    }

    /* fill table with delay value*/
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srDataReg;
    for (i = 0; i < SDL_PP_SDRAM_SIZE; i++)
    {
        if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, sdlValue) != GT_OK)
        {
            return GT_FAIL;
        }
    }

    sdlPpNarrowShadow = sdlValue;

    return GT_OK;
}


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
)
{
    GT_U32 value;
    GT_U8  i;
    GT_STATUS rc;

    /* check parameters */
    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    if ((c2cStart >= DFCDL_MAX_DELAY) ||
        (c2cEnd >= DFCDL_MAX_DELAY) ||
        (c2cStart > c2cEnd))
    {
        return GT_BAD_PARAM;
    }

    /* set result array to GT_FALSE */
    for (i = DFCDL_MIN_DELAY; i < DFCDL_MAX_DELAY; i++)
    {
        c2cStatus[i] = GT_FALSE;
        eccCount[i] = 0;
    }

    if (wideC2cEvReqHndl == 0)
    {
        /* this is the first time, create TxEnd Hhandle */
        CPSS_UNI_EV_CAUSE_ENT     evCause[2] =
               {CPSS_PP_MISC_UPLINK_W_ECC_ON_DATA_E,
                CPSS_PP_MISC_UPLINK_W_ECC_ON_HEADER_E};

        rc = cpssEventBind(evCause, 2, &wideC2cEvReqHndl);
        RETURN_ON_ERROR(rc);

        rc = dfcdlScanEccEventEnable(devNum);
        RETURN_ON_ERROR(rc);
    }

    rc = cpssOsTaskCreate("Wide_C2C_Scan"            , /* Task Name */
                          DFCDL_TASK_PRIORITY        , /* Task Priority */
                          DFCDL_TASK_STACK_SIZE      , /* Stack Size */
                          (unsigned (__TASKCONV *)
                          (void*))wideC2cScanTaskFunc, /* function to run */
                          NULL                       , /* Arguments list */
                          &wideC2cScanTaskId) ;        /* task ID */
    RETURN_ON_ERROR(rc);

    /* test each value from range */
    for (i = c2cStart; i <= c2cEnd; i++)
    {
        if (dfcdlPpWc2cSet(devNum, i) != GT_OK)
        {
            return GT_FAIL;
        }
        /* perform soft reset to Wide C2C */
        cpssDrvPpHwRegFieldSet(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                               globalRegs.wideC2cCtrl,1,1,1);
        eccCount[i] = dfcdlCountEccInt(devNum,waitTime, ECC_PP_WIDE);

        /* Read value of the Wide C2C status register */
        cpssDrvPpHwRegisterRead(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                globalRegs.c2cStatus,&value);
        c2cStatus[i] = (value & 0x1) ? GT_TRUE : GT_FALSE;
    }

    rc = cpssOsTaskDelete (wideC2cScanTaskId);
    return rc;
}


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
)
{
    GT_U32 sdlValue;
    GT_U32 addr;
    GT_U32 i;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    if (c2cDelay >= DFCDL_MAX_DELAY)
    {
        return GT_BAD_PARAM;
    }

    sdlValue = sdlPpNarrowShadow;
    U32_SET_FIELD_MAC(sdlValue, 18, 6, c2cDelay);

    /* set adrress register to first sdl entry */
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srAddressReg;
    if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, 0) != GT_OK)
    {
        return GT_FAIL;
    }

    /* fill table with delay value*/
    addr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srDataReg;
    for (i = 0; i < SDL_PP_SDRAM_SIZE; i++)
    {
        if (cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr, sdlValue) != GT_OK)
        {
            return GT_FAIL;
        }
    }

    sdlPpNarrowShadow = sdlValue;

    return GT_OK;
}


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
)
{
    GT_U32 value;
    GT_U8  i;
    GT_STATUS rc;

    /* check parameters */
    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    if ((c2cStart >= DFCDL_MAX_DELAY) ||
        (c2cEnd >= DFCDL_MAX_DELAY) ||
        (c2cStart > c2cEnd))
    {
        return GT_BAD_PARAM;
    }

    /* set result array to GT_FALSE */
    for (i = DFCDL_MIN_DELAY; i < DFCDL_MAX_DELAY; i++)
    {
        c2cStatus[i] = GT_FALSE;
        eccCount[i] = 0;
    }

    if (narrowC2cEvReqHndl == 0)
    {
        /* this is the first time, create TxEnd Hhandle */
        CPSS_UNI_EV_CAUSE_ENT     evCause[2] =
               {CPSS_PP_MISC_UPLINK_N_ECC_ON_DATA_E,
                CPSS_PP_MISC_UPLINK_N_ECC_ON_HEADER_E};

        rc = cpssEventBind(evCause, 2, &narrowC2cEvReqHndl);
        RETURN_ON_ERROR(rc);

        rc = dfcdlScanEccEventEnable(devNum);
        RETURN_ON_ERROR(rc);
    }

    rc = cpssOsTaskCreate("Narrow_C2C_Scan"          ,  /* Task Name */
                          DFCDL_TASK_PRIORITY        ,  /* Task Priority */
                          DFCDL_TASK_STACK_SIZE      ,  /* Stack Size */
                          (unsigned (__TASKCONV *)
                          (void*))narrowC2cScanTaskFunc,/* function to run*/
                          NULL                       ,  /* Arguments list */
                          &narrowC2cScanTaskId) ;       /* task ID */
    RETURN_ON_ERROR(rc);

    /* test each value from range */
    for (i = c2cStart; i <= c2cEnd; i++)
    {
        if (dfcdlPpNc2cSet(devNum, i) != GT_OK)
        {
            return GT_FAIL;
        }
        /* perform soft reset to Narrow C2C */
        cpssDrvPpHwRegFieldSet(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS, PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                               globalRegs.narrowC2cCtrl,1,1,1);
        eccCount[i] = dfcdlCountEccInt(devNum,waitTime, ECC_PP_NARROW);

        /* Read value of the Wide C2C status register */
        cpssDrvPpHwRegisterRead(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                globalRegs.c2cStatus,&value);
        c2cStatus[i] = (value & 0x2) ? GT_TRUE : GT_FALSE;
    }

    rc = cpssOsTaskDelete (narrowC2cScanTaskId);
    return rc;
}


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
)
{
    GT_U32 value;

    PRV_CPSS_EXMX_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entry);

    *entry = 0;
    if (cpssDrvPpHwRegisterRead(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,0x028001D8, &value) != GT_OK)
    {
        return GT_FAIL;
    }

    *entry = (GT_U8)(value & 0x3f);

    return GT_OK;
}


/*********************** private functions ************************************/

/*******************************************************************************
* dfcdlRamVerify
*
* DESCRIPTION:
*       This function verifies read and write to a specific block address.
*
* INPUTS:
*       devNum  - device number.
*       memBase - start of memory address
*       length - length of the block to verify
*       patternType   - memory test profile
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - if device number or
*                      if pointer to block is NULL
*       GT_FAIL - on error in testing the RAM area
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS dfcdlRamVerify
(
    IN GT_U8                devNum,
    IN GT_U32               memBase,
    IN GT_U32               length,
    IN GT_MEM_TEST_PROFILE  patternType
)
{
    GT_U32 addrError;
    GT_U32 readVal;
    GT_U32 writeVal;
    GT_BOOL testStatus;
    PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC specialRamFuncs;
    CPSS_DIAG_TEST_PROFILE_ENT cpssProfile;

    dfcdlConvertProfile (patternType, &cpssProfile);

    cpssOsMemSet (&specialRamFuncs, 0,
                  sizeof(PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC));

    if (prvCpssDiagAnyMemTest(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                              memBase, length, cpssProfile,
                              specialRamFuncs, &testStatus, &addrError,
                              &readVal, &writeVal) != GT_OK)
    {
        return GT_FAIL;
    }
    if (testStatus == GT_FALSE)
    {
        return GT_FAIL;
    }

    return GT_OK;
}

static GT_STATUS dfcdlPpScan
(
    IN GT_U8                devNum,
    IN GT_U8                dfcdlStart,
    IN GT_U8                dfcdlEnd,
    IN GT_MEM_TEST_PROFILE  patternType,
    IN FUNC_DFCDL_FIELD_SET setDfcdlField,
    IN GT_U32               memBase,
    IN GT_U32               length,
    OUT GT_BOOL             result[DFCDL_MAX_DELAY]
)
{
    GT_U8 i;

    /* check parameters */
    if ((dfcdlStart >= DFCDL_MAX_DELAY) ||
        (dfcdlEnd >= DFCDL_MAX_DELAY) ||
        (dfcdlStart > dfcdlEnd))
    {
        return GT_BAD_PARAM;
    }

    /* set result array to GT_FALSE */
    for (i = DFCDL_MIN_DELAY; i < DFCDL_MAX_DELAY; i++)
    {
        result[i] = GT_FALSE;
    }

    /* test each value from range */
    for (i = dfcdlStart; i <= dfcdlEnd; i++)
    {
        if (setDfcdlField(devNum, i) != GT_OK)
        {
            return GT_FAIL;
        }
        if (dfcdlRamVerify(devNum, memBase, length, patternType) == GT_OK)
        {
            result[i] = GT_TRUE;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* wideC2cScanTaskFunc
*
* DESCRIPTION:
*       This task waits for events of type ECC error on the wide uplink
*       interface. Upon receiving of such events it updates the events counters.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS __TASKCONV wideC2cScanTaskFunc
(
    GT_VOID
)
{
    GT_STATUS status;

    while (GT_TRUE)
    {
        status = eccEventHandle (wideC2cEvReqHndl, GT_TRUE,
                                 &dfcdlEccPpC2cCount);
        if (status != GT_OK)
        {
            break;
        }
    }
    return status;
}

/*******************************************************************************
* narrowC2cScanTaskFunc
*
* DESCRIPTION:
*       This task waits for events of type ECC error on the narrow uplink
*       interface. Upon receiving of such events it updates the events counters.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS __TASKCONV narrowC2cScanTaskFunc
(
    GT_VOID
)
{
    GT_STATUS status;

    while (GT_TRUE)
    {
        status = eccEventHandle (narrowC2cEvReqHndl, GT_TRUE,
                                 &dfcdlEccPpC2cCount);
        if (status != GT_OK)
        {
            break;
        }
    }
    return status;
}

/**************** unit test ***************************************************/
void dfcdlTest(int ram)
{
    GT_U8   entry;
    GT_BOOL result[DFCDL_MAX_DELAY];
    GT_U32  eccCount[DFCDL_MAX_DELAY];
    GT_STATUS rc;

    switch (ram)
    {
        case 0:
            /* narrow */
            cpssOsMemSet(result,0,sizeof(result));
            rc = dfcdlPpNSramFbScan(0, 1, 40, GT_AA_55, result);
            rc = dfcdlPpSramClkSet(0, 35);

            cpssOsMemSet(result,0,sizeof(result));
            cpssOsMemSet(eccCount,0,sizeof(result));
            rc = dfcdlPpWc2cScan(0,2,10,5,result,eccCount);

            cpssOsMemSet(result,0,sizeof(result));
            cpssOsMemSet(eccCount,0,sizeof(result));
            rc = dfcdlPpNc2cScan(0,12,33,4,result,eccCount);

            rc = dfcdlPpC2cFindEntry(0,&entry);
            break;
        case 1:
            /* buffer dram */
            cpssOsMemSet(result,0,sizeof(result));
            rc = dfcdlPpDqsScan(0,1,40,GT_AA_55,result);
            cpssOsMemSet(result,0,sizeof(result));
            rc = dfcdlPpClkOutScan(0,1,40,GT_AA_55,result);
            cpssOsMemSet(result,0,sizeof(result));
            rc = dfcdlPpRxClkScan(0,1,40,GT_AA_55,result);

            rc = dfcdlPpDramFindEntry(0,2,5,23,10,&entry);

            cpssOsMemSet(result,0,sizeof(result));

            break;
        default:
            return;
    }
    return;
}

