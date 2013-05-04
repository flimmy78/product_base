/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAutoDriverDfcdl.c
*
* DESCRIPTION:
*       This file includes implementation of the dfcdl library.
*       This file was automatic generated.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <dfcdl/private/gtDfcdlPrv.h>
#include <dfcdl/gtAutoDriverDfcdl.h>
#include <cpss/extServices/cpssExtServices.h>
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/events/cpssGenEventRequests.h>

#define SDL_SIZE    128
#define NUM_BURST   255

static  GT_STATUS   dfvDbPerformTest
(
    IN  GT_U8   devId,
    IN  GT_U32  ftdll,
    IN  GT_U32  waitTime,
    OUT GT_U32  *errCnt
);

static  GT_STATUS dfvNsPerformTest
(
    IN  GT_U8   devId,
    IN  GT_U32  MemSelect,
    IN  GT_U32  ftdll,
    IN  GT_U32  waitTime,
    OUT GT_U32  *errCnt
);

static  GT_STATUS   uplinkPerformTest
(
    IN  GT_U8   devId,
    IN  GT_U32  ftdll,
    IN  GT_U32  waitTime,
    OUT GT_U32  *eccCount,
    OUT GT_BOOL *UpDown
);

GT_STATUS __TASKCONV tgWideC2cGetEntryScanTaskFunc
(
    GT_VOID
);

GT_STATUS __TASKCONV tgNarrowC2cGetEntryScanTaskFunc
(
    GT_VOID
);

GT_STATUS __TASKCONV tgWideC2cAutoScanTaskFunc
(
    GT_VOID
);

GT_STATUS __TASKCONV tgNarrowC2cAutoScanTaskFunc
(
    GT_VOID
);

/* Local variables */
static GT_U32   dbDqs = 0;    /* supplemental DQS value            */
static GT_U32   dbClkOut = 0; /* supplemental ClkOut value         */
static GT_U32   dbStartBurst = 0; /* supplemental StartBurst value */

static GT_U32   c2cWideRx = 0;  /* supplemental Wide Rx Clk value   */
static GT_U32   c2cNarrowRx = 0;/* supplemental Narrow Rx Clk value */

/* dfcdl parameters - save const parameters for all refresh iterations */
static GT_U32 constDqs;
static GT_U32 constClkOut;
static GT_U32 constStartBurst;

/* indicates if need to stop the ftdll auto scan */
static GT_U32 ftdllAutoScanContinue = GT_TRUE;

/*  value to write to WC2C in case of ecc events */
static GT_U32 eccWc2cVal = 0;
 /*  value to write to NC2C in case of ecc events */
static GT_U32 eccNc2cVal = 0;

/* task ids for wide C2C scan */
static CPSS_TASK wideC2cGetEntryScanTaskId;
static CPSS_TASK wideC2cAutoScanTaskId;

/* task ids for narrow C2C scan */
static CPSS_TASK narrowC2cGetEntryScanTaskId;
static CPSS_TASK narrowC2cAutoScanTaskId;

/* handle for wide uplink interface ECC errors while getting first/next entry
   of the ftdllTigerWC2cScan table */
static GT_U32  wideC2cGetEntryEvReqHndl = 0;

/* handle for narrow uplink interface ECC errors while getting first/next entry
   of the ftdllTigerNC2cScan table */
static GT_U32  narrowC2cGetEntryEvReqHndl = 0;

/* handle for wide uplink interface ECC errors while performing auto scan */
static GT_U32  wideC2cAutoScanEvReqHndl = 0;

/* handle for narrow uplink interface ECC errors while performing auto scan */
static GT_U32  narrowC2cAutoScanEvReqHndl = 0;

/* Count ECC error events */
static GT_BOOL doWideUplinkCount = GT_FALSE;
static GT_BOOL doNarrowUplinkCount = GT_FALSE;

/* update DFCDL setting command */
static GT_BOOL updateWideUplinkCommand = GT_FALSE;
static GT_BOOL updateNarrowUplinkCommand = GT_FALSE;

/*******************************************************************************
* dbFtdllSet
*
* DESCRIPTION:
*       N. SRAM DFCDL setting command (Tiger only).
*
*   INPUTS:
*       GT_U8  devId - Device number
*       GT_U32 ftdll - DB SDRAM FTDLL RAM
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
static GT_STATUS    dbFtdllSet
(
    IN  GT_U8   devId,
    IN  GT_U32  ftdll
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      i;  /* iterator    */


    /* fill FTDLL */
    rc = cpssDrvPpHwRegisterWrite(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                  extMemRegs.dbFtdllDxAddrReg, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set ftdll value */
    for(i = 0; i < SDL_SIZE; i++)
    {
        rc = cpssDrvPpHwRegisterWrite(devId,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                      PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                      extMemRegs.dbFtdllDxDataReg, ftdll);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* uplinkFtdllSet
*
* DESCRIPTION:
*       Uplink DFCDL setting command (Tiger only).
*
*   INPUTS:
*       GT_U8  devId - Device number
*       GT_U32 ftdll - Uplink FTDLL RAM
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
static GT_STATUS    uplinkFtdllSet
(
    IN  GT_U8   devId,
    IN  GT_U32  ftdll
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      i;  /* iterator    */

    /* set adrress register to first sdl entry */
    rc = cpssDrvPpHwRegisterWrite(devId,CPSS_PORT_GROUP_UNAWARE_MODE_CNS, PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                  extMemRegs.srAddressReg, 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* fill Uplink FTDLL RAM */
    for(i = 0; i < SDL_SIZE; i++)
    {
        rc = cpssDrvPpHwRegisterWrite(devId,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                      PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                      extMemRegs.srDataReg, ftdll);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* uplinkFtdllIsrSet
*
* DESCRIPTION:
*       Uplink DFCDL setting command (Tiger only) call this function only
*       from ISR.
*
*   INPUTS:
*       GT_U8  devId - Device number
*       GT_U32 ftdll - Uplink FTDLL RAM
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
static GT_STATUS    uplinkFtdllIsrSet
(
    IN  GT_U8   devId,
    IN  GT_U32  ftdll
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      i;  /* iterator    */

    /* set adrress register to first sdl entry */
    rc = cpssDrvPpHwIsrWrite(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                             extMemRegs.srAddressReg, 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* fill Uplink FTDLL RAM */
    for(i = 0; i < SDL_SIZE; i++)
    {
        rc = cpssDrvPpHwIsrWrite(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                 extMemRegs.srDataReg, ftdll);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* nsFtdllSet
*
* DESCRIPTION:
*       Narrow SRAM FTDLL setting command.
*
*   INPUTS:
*       GT_U8  devId  - Device number
*       GT_U32 memSel - memory select, 0 - NSRAM1, 1 - NSRAM2
*       GT_U32 ftdll  - Uplink FTDLL RAM
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
static GT_STATUS  nsFtdllSet
(
    IN  GT_U8   devId,
    IN  GT_U32  memSel,
    IN  GT_U32  ftdll
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      i;  /* iterator    */

    /* fill FTDLL */
    rc = cpssDrvPpHwRegisterWrite(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                  extMemRegs.nsFtdllAddrReg + 0x40 * memSel, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set ftdll value */
    for(i = 0; i < SDL_SIZE; i++)
    {
        rc = cpssDrvPpHwRegisterWrite(devId,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                      PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                      extMemRegs.nsFtdllWrDatReg +
                                      0x40 * memSel, ftdll);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* gtFtdllTigerNSDelaySet
*
* DESCRIPTION:
*       N. SRAM DFCDL setting command (Tiger only).
*
*   INPUTS:
*       GT_U8  devNum -
*       Device number
*
*       ftdllTigerNSDelaySet_IN_MemSelect MemSelect -
*       Memory select
*
*       GT_U32 delay0 -
*       Determines the amount of delay element 0.
*
*       GT_U32 delay1 -
*       Determines the amount of delay element 1.
*
*       GT_U32 delay2 -
*       Determines the amount of delay element 2.
*
*       GT_U32 delay3 -
*       Determines the amount of delay element 3.
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNSDelaySet
(
    IN GT_U8  devNum,
    IN   ftdllTigerNSDelaySet_IN_MemSelect  MemSelect,
    IN GT_U32 delay0,
    IN GT_U32 delay1,
    IN GT_U32 delay2,
    IN GT_U32 delay3
)
{
    GT_STATUS   rc;     /* return code    */
    GT_U32      ftdll = 0;  /* FTDLL value    */
    GT_U32      i;      /* iterator       */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devNum);

    /* write register */
    U32_SET_FIELD_MAC(ftdll, 0, 7, delay0);
    U32_SET_FIELD_MAC(ftdll, 7, 7, delay1);
    U32_SET_FIELD_MAC(ftdll, 14, 7, delay2);
    U32_SET_FIELD_MAC(ftdll, 21, 7, delay3);

    /* fill FTDLL */
    rc = cpssDrvPpHwRegisterWrite(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                  PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                  extMemRegs.nsFtdllAddrReg +
                                  0x40 * MemSelect, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set ftdll value */
    for(i = 0; i < SDL_SIZE; i++)
    {
        rc = cpssDrvPpHwRegisterWrite(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                      PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                      extMemRegs.nsFtdllWrDatReg +
                                      0x40 * MemSelect, ftdll);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* gtFtdllTigerDbDqsScanGetTableEntry
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerDbDqsScan table.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 DQSStart -
*       Start value of DQS
*
*       GT_U32 DQSEnd -
*       End value of DQS
*
*       GT_U32 ClkOut -
*       Value of clock out delay
*
*       GT_U32 StartBurst -
*       StartBurst_out
*
*       GT_U32 WaitTime -
*       Wait time in ms between cycles
*
*       ftdllTigerDbDqsScan_FIELDS  tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   -
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerDbDqsScan_FIELDS  tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerDbDqsScanGetTableEntry
(
    IN   GT_U8  devId,
    IN   GT_U32 DQSStart,
    IN   GT_U32 DQSEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerDbDqsScan_FIELDS    *tableFields,
    IN  GT_U32  entryIndex
)
{
    GT_STATUS   rc;     /* return code       */
    GT_U32      ftdll;  /* FTDLL value       */
    GT_U32      regVal; /* register value    */
    static  GT_U32   ftdllFirst; /* start ftdll value */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devId);

    if(DQSStart + entryIndex > DQSEnd)
    {
        return GT_NO_MORE;
    }

    tableFields->tableFields.General.DQSValue = DQSStart + entryIndex;
    /* set ftdll value */
    ftdll = 0;
    U32_SET_FIELD_MAC(ftdll, 0, 7, DQSStart + entryIndex);
    rc = cpssDrvPpHwRegisterRead(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                 extMemRegs.dbFtdllReadDataReg, &regVal);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(entryIndex == 0)
    {/* store initial FTDLL value */
        ftdllFirst = regVal;
    }

    U32_SET_FIELD_MAC(ftdll, 7, 7, U32_GET_FIELD_MAC(regVal, 7, 7));
    U32_SET_FIELD_MAC(ftdll, 14, 7, U32_GET_FIELD_MAC(regVal, 14, 7));
    /* perform testing */
    rc = dfvDbPerformTest(devId, ftdll, WaitTime,
                          &tableFields->tableFields.General.ErrorCounter);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(DQSStart + entryIndex == DQSEnd)
    {/* restore initial FTDLL value */
       rc = dbFtdllSet(devId, ftdllFirst);
       if(rc != GT_OK)
       {
           return rc;
       }

    }
    return GT_OK;
}

/*******************************************************************************
* gtFtdllTigerDbClkOutScanGetTableEntry
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerDbClkOutScan table.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 ClkOutStart -
*       Start value of clock out
*
*       GT_U32 ClkOutEnd -
*       End value of clock out
*
*       GT_U32 Dqs -
*       Dqs value
*
*       GT_U32 StartBurst -
*       StartBurst_out
*
*       GT_U32 WaitTime -
*       Wait time in ms between cycles
*
*       ftdllTigerDbClkOutScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   -
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerDbClkOutScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerDbClkOutScanGetTableEntry
(
    IN   GT_U8  devId,
    IN   GT_U32 ClkOutStart,
    IN   GT_U32 ClkOutEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerDbClkOutScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
)
{
    GT_STATUS   rc;     /* return code       */
    GT_U32      ftdll;  /* FTDLL value       */
    GT_U32      regVal; /* register value    */
    static  GT_U32  ftdllFirst; /* start ftdll value */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devId);

    if(ClkOutStart + entryIndex > ClkOutEnd)
    {
        return GT_NO_MORE;
    }

    tableFields->tableFields.General.ClkOutValue = ClkOutStart + entryIndex;
    /* set ftdll value */
    ftdll = 0;
    U32_SET_FIELD_MAC(ftdll, 7, 7, ClkOutStart + entryIndex);

    rc = cpssDrvPpHwRegisterRead(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                 extMemRegs.dbFtdllReadDataReg, &regVal);
    if(rc != GT_OK)
    {
        return rc;
    }
    if(entryIndex == 0)
    {/* store initial ftdll value */
        ftdllFirst = regVal;
    }

    U32_SET_FIELD_MAC(ftdll, 0, 7, U32_GET_FIELD_MAC(regVal, 0, 7));
    U32_SET_FIELD_MAC(ftdll, 14, 7, U32_GET_FIELD_MAC(regVal, 14, 7));

    /* perform testing */
    rc = dfvDbPerformTest(devId, ftdll, WaitTime,
                          &tableFields->tableFields.General.ErrorCounter);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(ClkOutStart + entryIndex == ClkOutEnd)
    {/* restore initial ftdll value */
        rc = dbFtdllSet(devId, ftdllFirst);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* gtFtdllTigerDbStartBurstScanGetTableEntry
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerDbStartBurstScan table.
*
*   INPUTS:
*       GT_U8  devNum -
*       Dev Num
*
*       GT_U32 StartBurstStart -
*       Start value of StartBurst_out
*
*       GT_U32 StartBurstEnd -
*       End value of StartBurst_out
*
*       GT_U32 Dqs -
*       Dqs value
*
*       GT_U32 ClkOut -
*       ClkOut value
*
*       GT_U32 WaitTime -
*       Wait time in ms between cycles
*
*       ftdllTigerDbStartBurstScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   -
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerDbStartBurstScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerDbStartBurstScanGetTableEntry
(
    IN   GT_U8  devNum,
    IN   GT_U32 StartBurstStart,
    IN   GT_U32 StartBurstEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerDbStartBurstScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
)
{
    GT_STATUS   rc;     /* return code       */
    GT_U32      ftdll;  /* FTDLL value       */
    GT_U32      regVal; /* register value    */
    static  GT_U32 ftdllFirst; /* start FTDLL value */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devNum);

    if(StartBurstStart + entryIndex > StartBurstEnd)
    {
        return GT_NO_MORE;
    }

    tableFields->tableFields.General.StartBurstValue = StartBurstStart +
        entryIndex;
    /* set ftdll value */
    ftdll = 0;
    U32_SET_FIELD_MAC(ftdll, 14, 7, StartBurstStart + entryIndex);

    rc = cpssDrvPpHwRegisterRead(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                 extMemRegs.dbFtdllReadDataReg, &regVal);
    if(rc != GT_OK)
    {
        return rc;
    }
    if(entryIndex == 0)
    {/* store initial ftdll value */
        ftdllFirst = regVal;
    }

    U32_SET_FIELD_MAC(ftdll, 0, 7, U32_GET_FIELD_MAC(regVal, 0, 7));
    U32_SET_FIELD_MAC(ftdll, 7, 7, U32_GET_FIELD_MAC(regVal, 7, 7));

    /* perform testing */
    rc = dfvDbPerformTest(devNum, ftdll, WaitTime,
                          &tableFields->tableFields.General.ErrorCounter);
    if(rc != GT_OK)
    {
        return rc;
    }
    if(StartBurstStart + entryIndex == StartBurstEnd)
    {/* restore initial FTDLL value */
        rc = dbFtdllSet(devNum, ftdllFirst);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* gtFtdllTigerWC2cScanGetTableEntry
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerWC2cScan table.
*
*   INPUTS:
*       GT_U8  devNum -
*       Dev Num
*
*       GT_U32 DFCDLStart -
*       DFCDL Start
*
*       GT_U32 DFCDLEnd -
*       DFCDL End
*
*       GT_U32 nDfcdl -
*       Narrow DFCDL value
*
*       GT_U32 WaitTime -
*       Wait time defined before checking interrupt count
*
*       ftdllTigerWC2cScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   -
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerWC2cScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerWC2cScanGetTableEntry
(
    IN   GT_U8  devNum,
    IN   GT_U32 DFCDLStart,
    IN   GT_U32 DFCDLEnd,
    IN   GT_U32 nDfcdl,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerWC2cScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      ftdll;  /* FTDLL value */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devNum);

    if(DFCDLStart + entryIndex > DFCDLEnd)
    {
        return GT_NO_MORE;
    }

    if(entryIndex == 0)
    {
        doWideUplinkCount = GT_TRUE;
    }

    if (wideC2cGetEntryEvReqHndl == 0)
    {
        /* this is the first time, create TxEnd Hhandle */
        CPSS_UNI_EV_CAUSE_ENT     evCause[2] =
               {CPSS_PP_MISC_UPLINK_W_ECC_ON_DATA_E,
                CPSS_PP_MISC_UPLINK_W_ECC_ON_HEADER_E};

        rc = cpssEventBind(evCause, 2, &wideC2cGetEntryEvReqHndl);
        RETURN_ON_ERROR(rc);

        rc = dfcdlScanEccEventEnable(devNum);
        RETURN_ON_ERROR(rc);
    }

    rc = cpssOsTaskCreate("Tiger_Wide_C2C_Scan"      , /* Task Name */
                          DFCDL_TASK_PRIORITY        , /* Task Priority */
                          DFCDL_TASK_STACK_SIZE      , /* Stack Size */
                          (unsigned (__TASKCONV *)     /* function to run */
                          (void*))tgWideC2cGetEntryScanTaskFunc,
                          NULL                       , /* Arguments list */
                          &wideC2cGetEntryScanTaskId); /* task ID */
    RETURN_ON_ERROR(rc);

    tableFields->tableFields.General.DFCDLValue = DFCDLStart + entryIndex;

    /* set ftdll value */
    ftdll = 0;
    U32_SET_FIELD_MAC(ftdll, 0, 7, DFCDLStart + entryIndex);
    U32_SET_FIELD_MAC(ftdll, 7, 7, nDfcdl);
    /* perform test */
    rc = uplinkPerformTest(devNum, ftdll, WaitTime,
                           &tableFields->tableFields.General.NumOfECC,
                           (GT_BOOL*)&tableFields->tableFields.General.UpDown);

    if(DFCDLStart + entryIndex == DFCDLEnd)
    {
        doWideUplinkCount = GT_FALSE;
    }
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssOsTaskDelete (wideC2cGetEntryScanTaskId);
    return rc;
}

/*******************************************************************************
* gtFtdllTigerNC2cScanGetTableEntry
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerNC2cScan table.
*
*   INPUTS:
*       GT_U8  devNum -
*       Dev Num
*
*       GT_U32 DFCDLStart -
*       DFCDL Start
*
*       GT_U32 DFCDLEnd -
*       DFCDL End
*
*       GT_U32 wDfcdl -
*       Wide DFCDL value
*
*       GT_U32 WaitTime -
*       Wait time defined before checking interrupt count
*
*       ftdllTigerNC2cScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   -
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerNC2cScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNC2cScanGetTableEntry
(
    IN   GT_U8  devNum,
    IN   GT_U32 DFCDLStart,
    IN   GT_U32 DFCDLEnd,
    IN   GT_U32 wDfcdl,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerNC2cScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      ftdll;  /* FTDLL value */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devNum);

    if(DFCDLStart + entryIndex > DFCDLEnd)
    {
        return GT_NO_MORE;
    }
    if(entryIndex == 0)
    {
        doNarrowUplinkCount = GT_TRUE;
    }

    if (narrowC2cGetEntryEvReqHndl == 0)
    {
        /* this is the first time, create TxEnd Hhandle */
        CPSS_UNI_EV_CAUSE_ENT     evCause[2] =
               {CPSS_PP_MISC_UPLINK_N_ECC_ON_DATA_E,
                CPSS_PP_MISC_UPLINK_N_ECC_ON_HEADER_E};

        rc = cpssEventBind(evCause, 2, &narrowC2cGetEntryEvReqHndl);
        RETURN_ON_ERROR(rc);

        rc = dfcdlScanEccEventEnable(devNum);
        RETURN_ON_ERROR(rc);
    }

    rc = cpssOsTaskCreate("Tiger_Narrow_C2C_Scan"    , /* Task Name */
                          DFCDL_TASK_PRIORITY        , /* Task Priority */
                          DFCDL_TASK_STACK_SIZE      , /* Stack Size */
                          (unsigned (__TASKCONV *)     /* function to run */
                          (void*))tgNarrowC2cGetEntryScanTaskFunc,
                          NULL                       , /* Arguments list */
                          &narrowC2cGetEntryScanTaskId);/* task ID */
    RETURN_ON_ERROR(rc);

    tableFields->tableFields.General.DFCDLValue = DFCDLStart + entryIndex;

    /* set ftdll value */
    ftdll = 0;
    U32_SET_FIELD_MAC(ftdll, 0, 7, wDfcdl);
    U32_SET_FIELD_MAC(ftdll, 7, 7, DFCDLStart + entryIndex);

    /* perform test */
    rc = uplinkPerformTest(devNum, ftdll, WaitTime,
                           &tableFields->tableFields.General.NumOfECC,
                           (GT_BOOL*)&tableFields->tableFields.General.UpDown);

    if(DFCDLStart + entryIndex == DFCDLEnd)
    {
        doNarrowUplinkCount = GT_FALSE;
    }
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssOsTaskDelete (narrowC2cGetEntryScanTaskId);
    return rc;
}

/*******************************************************************************
* gtFtdllTigerNSClkOutScanGetTableEntry
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerNSClkOutScan table.
*
*   INPUTS:
*       GT_U8  devNum -
*       Dev Num
*
*       ftdllTigerNSClkOutScan_IN_MemSelect MemSelect -
*       Memory select
*
*       GT_U32 ClkOutStart -
*       DFCDL Start
*
*       GT_U32 ClkOutEnd -
*       DFCDL End
*
*       GT_U32 RetClk -
*       No description available.
*
*       GT_U32 RetClkInv -
*       No description available.
*
*       GT_U32 WaitTime -
*       Wait time in ms
*
*       ftdllTigerNSClkOutScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   -
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerNSClkOutScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNSClkOutScanGetTableEntry
(
    IN   GT_U8  devNum,
    IN   ftdllTigerNSClkOutScan_IN_MemSelect    MemSelect,
    IN   GT_U32 ClkOutStart,
    IN   GT_U32 ClkOutEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerNSClkOutScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
)
{
    GT_STATUS   rc;     /* return code    */
    GT_U32      ftdll;  /* FTDLL          */
    GT_U32      regVal; /* register value */
    static GT_U32 ftdllFirst; /* start FTDLL value */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devNum);

    if(ClkOutStart + entryIndex > ClkOutEnd)
    {
        return GT_NO_MORE;
    }

    tableFields->tableFields.General.DFCDLValue = ClkOutStart + entryIndex;
    /* set FTDLL value */
    ftdll = 0;
    U32_SET_FIELD_MAC(ftdll, 0, 7, ClkOutStart + entryIndex);

    rc = cpssDrvPpHwRegisterRead(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
                                 extMemRegs.nsFtdllReadDataReg +
                                 0x40 * MemSelect, &regVal);
    if(rc != GT_OK)
    {
        return rc;
    }
    if(entryIndex == 0)
    {/* store initial FTDLL value */
        ftdllFirst = regVal;
    }
    U32_SET_FIELD_MAC(ftdll, 7, 7, U32_GET_FIELD_MAC(regVal, 7, 7));
    U32_SET_FIELD_MAC(ftdll, 14, 7, U32_GET_FIELD_MAC(regVal, 14, 7));

    /* perform test */
    rc = dfvNsPerformTest(devNum, MemSelect, ftdll, WaitTime,
                          &tableFields->tableFields.General.ErrorCounter);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(ClkOutStart + entryIndex == ClkOutEnd)
    {/* restore initial FTDLL value */
        rc = nsFtdllSet(devNum, MemSelect, ftdllFirst);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* gtFtdllTigerNSRetClkScanGetTableEntry
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerNSRetClkScan table.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       ftdllTigerNSRetClkScan_IN_MemSelect MemSelect -
*       Memory select
*
*       GT_U32 RetClkStart -
*       No description available.
*
*       GT_U32 RetClkEnd -
*       No description available.
*
*       GT_U32 ClkOut -
*       No description available.
*
*       GT_U32 RetClkInv -
*       No description available.
*
*       GT_U32 WaitTime -
*       Wait time in ms
*
*       ftdllTigerNSRetClkScan_FIELDS   tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   -
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerNSRetClkScan_FIELDS   tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNSRetClkScanGetTableEntry
(
    IN   GT_U8  devId,
    IN   ftdllTigerNSRetClkScan_IN_MemSelect    MemSelect,
    IN   GT_U32 RetClkStart,
    IN   GT_U32 RetClkEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerNSRetClkScan_FIELDS *tableFields,
    IN  GT_U32  entryIndex
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      ftdll;  /* FTDLL       */
    GT_U32      regVal; /* register value */
    static GT_U32 ftdllFirst; /* start FTDLL value */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devId);

    if(RetClkStart + entryIndex > RetClkEnd)
    {
        return GT_NO_MORE;
    }


    tableFields->tableFields.General.DFCDLValue = RetClkStart + entryIndex;
    /* set FTDLL value */
    rc = cpssDrvPpHwRegisterRead(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                 extMemRegs.nsFtdllReadDataReg +
                                 0x40 * MemSelect, &regVal);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(entryIndex == 0)
    {/* store initial FTDLL value */
        ftdllFirst = regVal;
    }
    ftdll = 0;
    U32_SET_FIELD_MAC(ftdll, 0, 7, U32_GET_FIELD_MAC(regVal, 0,7));
    U32_SET_FIELD_MAC(ftdll, 7, 7, RetClkStart + entryIndex);
    U32_SET_FIELD_MAC(ftdll, 14, 7, U32_GET_FIELD_MAC(regVal, 14,7));


    /* perform test */
    rc = dfvNsPerformTest(devId, MemSelect, ftdll, WaitTime,
                          &tableFields->tableFields.General.ErrorCounter);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(RetClkStart + entryIndex == RetClkEnd)
    {/* restore initial FTDLL value */
        rc = nsFtdllSet(devId, MemSelect, ftdllFirst);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* gtFtdllTigerNSRetClkInvScanGetTableEntry
*
* DESCRIPTION:
*       Get first / next entry of the ftdllTigerNSRetClkInvScan table.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       ftdllTigerNSRetClkInvScan_IN_MemSelect MemSelect -
*       Memory select
*
*       GT_U32 RetClkInvStart -
*       No description available.
*
*       GT_U32 RetClkInvEnd -
*       No description available.
*
*       GT_U32 ClkOut -
*       No description available.
*
*       GT_U32 RetClk -
*       No description available.
*
*       GT_U32 WaitTime -
*       Wait time in ms
*
*       ftdllTigerNSRetClkInvScan_FIELDS    tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   -
*           index of current entry refresh index.
*
*   OUTPUTS:
*       ftdllTigerNSRetClkInvScan_FIELDS    tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNSRetClkInvScanGetTableEntry
(
    IN   GT_U8  devId,
    IN   ftdllTigerNSRetClkInvScan_IN_MemSelect MemSelect,
    IN   GT_U32 RetClkInvStart,
    IN   GT_U32 RetClkInvEnd,
    IN   GT_U32 WaitTime,
    INOUT ftdllTigerNSRetClkInvScan_FIELDS  *tableFields,
    IN  GT_U32  entryIndex
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      ftdll;  /* FTDLL       */
    GT_U32      regVal; /* register value */
    static GT_U32 ftdllFirst; /* start FTDLL value */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devId);

    if(RetClkInvStart + entryIndex > RetClkInvEnd)
    {
        return GT_NO_MORE;
    }

    tableFields->tableFields.General.DFCDLValue = RetClkInvStart + entryIndex;
    /* set FTDLL value */
    rc = cpssDrvPpHwRegisterRead(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                 extMemRegs.nsFtdllReadDataReg +
                                 0x40 * MemSelect, &regVal);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(entryIndex == 0)
    {/* store initial FTDLL value */
        ftdllFirst = regVal;
    }
    ftdll = 0;
    U32_SET_FIELD_MAC(ftdll, 0, 7, U32_GET_FIELD_MAC(regVal, 0, 7));
    U32_SET_FIELD_MAC(ftdll, 7, 7, U32_GET_FIELD_MAC(regVal, 7, 7));
    U32_SET_FIELD_MAC(ftdll, 14, 7, RetClkInvStart + entryIndex);

    /* perform test */
    rc = dfvNsPerformTest(devId, MemSelect, ftdll, WaitTime,
                          &tableFields->tableFields.General.ErrorCounter);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(RetClkInvStart + entryIndex == RetClkInvEnd)
    {/* restore initial FTDLL value */
        rc = nsFtdllSet(devId, MemSelect, ftdllFirst);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* dfvDbPerformTest
*
* DESCRIPTION:
*       Perform SDRAM test by using DFV.
*
*   INPUTS:
*       devId       - device Id
*       ftdll       - DB SDRAM FTDLL RAM
*       waitTime    - time in ms to wait after starting DFV test.
*
*   OUTPUTS:
*       errCnt  - error counter after test.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
static  GT_STATUS   dfvDbPerformTest
(
    IN  GT_U8   devId,
    IN  GT_U32  ftdll,
    IN  GT_U32  waitTime,
    OUT GT_U32  *errCnt
)
{
    GT_STATUS   rc;       /* return code    */
    GT_U32      val;      /* register value */
    GT_U32      chunkSel; /* chunk select   */
    GT_U32      cnt;      /* counter        */


    *errCnt = 0;
    /* set FTDLL*/
    rc = dbFtdllSet(devId, ftdll);
    if(rc != GT_OK)
    {
        return rc;
    }

    for(chunkSel = 0; chunkSel < 4; chunkSel++)
    {
        val = 0;
        /* clear error counter */
        U32_SET_FIELD_MAC(val, 29, 1, 1);
        /* set burst size */
        U32_SET_FIELD_MAC(val, 11, 15, NUM_BURST);
        /* set chunk select */
        U32_SET_FIELD_MAC(val, 27, 2, chunkSel);

        rc = cpssDrvPpHwRegBitMaskWrite(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                        extMemRegs.dbDfvCtrlReg, 0x39FFF801,
                                        val);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* reset counter clear */
        rc = cpssDrvPpHwRegBitMaskWrite(devId,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                        extMemRegs.dbDfvCtrlReg, 0x20000000,
                                        0x0);

        if(rc != GT_OK)
        {
            return rc;
        }
        /* start DFV */
        rc = cpssDrvPpHwRegBitMaskWrite(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                        extMemRegs.dbDfvCtrlReg, 0x1, 0x1);

        if(rc != GT_OK)
        {
            return rc;
        }
        /* wait */
        cpssOsTimerWkAfter(waitTime);

        /* stop DFV */
        rc = cpssDrvPpHwRegBitMaskWrite(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                        extMemRegs.dbDfvCtrlReg, 0x1, 0);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read error counter */
        rc = cpssDrvPpHwRegFieldGet(devId,CPSS_PORT_GROUP_UNAWARE_MODE_CNS, PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                    extMemRegs.dbDfvErrCnt, 0, 16, &cnt);
        if(rc != GT_OK)
        {
            return rc;
        }
        *errCnt += cnt;
    }

    return GT_OK;
}

/*******************************************************************************
* dfvNsPerformTest
*
* DESCRIPTION:
*       Perform Narrow SRAM test by using DFV.
*
*   INPUTS:
*       devId       - device Id
*       ftdll       - Narrow SRAM FTDLL RAM
*       waitTime    - time in ms to wait after starting DFV test.
*
*   OUTPUTS:
*       errCnt  - error counter after test.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
static  GT_STATUS dfvNsPerformTest
(
    IN  GT_U8   devId,
    IN  GT_U32  MemSelect,
    IN  GT_U32  ftdll,
    IN  GT_U32  waitTime,
    OUT GT_U32  *errCnt
)
{
    GT_STATUS   rc;  /* return code */
    GT_U32      val; /* value       */

    /* set FTDLL */
    rc = nsFtdllSet(devId, MemSelect, ftdll);
    if(rc != GT_OK)
    {
        return rc;
    }

    val = 0;
    /* clear error counter */
    U32_SET_FIELD_MAC(val, 29, 1, 1);
    /* set burst size */
    U32_SET_FIELD_MAC(val, 11, 15, NUM_BURST);
    /* memory select */
    U32_SET_FIELD_MAC(val, 6, 1, MemSelect);

    rc = cpssDrvPpHwRegBitMaskWrite(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                    extMemRegs.nsDfvCtrlReg, 0x2007F840, val);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* reset counter clear */
    rc = cpssDrvPpHwRegBitMaskWrite(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                    extMemRegs.nsDfvCtrlReg, 0x20000000, 0x0);

    if(rc != GT_OK)
    {
        return rc;
    }
    /* start DFV */
    rc = cpssDrvPpHwRegBitMaskWrite(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                    extMemRegs.nsDfvCtrlReg, 0x1, 0x1);

    if(rc != GT_OK)
    {
        return rc;
    }

    /* wait */
    cpssOsTimerWkAfter(waitTime);

    /* stop DFV */
    rc = cpssDrvPpHwRegBitMaskWrite(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                    extMemRegs.nsDfvCtrlReg, 0x1, 0);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* read error counter */
    rc = cpssDrvPpHwRegFieldGet(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                                extMemRegs.nsDfvErrCnt, 0, 16, errCnt);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* uplinkPerformTest
*
* DESCRIPTION:
*       Perform UpLink memory test.
*
*   INPUTS:
*       devId       - device Id
*       ftdll       - UPLINK FTDLL RAM
*       waitTime    - time in ms to wait after starting DFV test.
*
*   OUTPUTS:
*       eccCount    - number of ECC interupt
*       UpDown      - uplink status
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
static  GT_STATUS   uplinkPerformTest
(
    IN  GT_U8   devId,
    IN  GT_U32  ftdll,
    IN  GT_U32  waitTime,
    OUT GT_U32  *eccCount,
    OUT GT_BOOL *UpDown
)
{
    GT_STATUS   rc;    /* return code */
    GT_U32      value; /* value       */

    /* set FTDLL */
    rc = uplinkFtdllSet(devId,ftdll);
    if(rc != GT_OK)
    {
        return rc;
    }

     /* count ECC interrupts */
     *eccCount = dfcdlCountEccInt(devId, waitTime, ECC_PP_WIDE);
     /* Read value of the Wide C2C status register */
     cpssDrvPpHwRegisterRead(devId, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,PRV_CPSS_EXMX_DEV_REGS_MAC(devId)->
                             globalRegs.c2cStatus, &value);
    *UpDown = BIT2BOOL_MAC(value & 0x1);

    return GT_OK;
}


/*******************************************************************************
* gtFtdllTigerDqsSet
*
* DESCRIPTION:
*       No description available.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 Dqs -
*       Dqs for DB SDRAM FTDLL RAM
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerDqsSet
(
    IN   GT_U8  devId,
    IN   GT_U32 Dqs
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      ftdll;  /* FTDLL       */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devId);

    ftdll = 0;
    U32_SET_FIELD_MAC(ftdll, 0, 7, Dqs);
    U32_SET_FIELD_MAC(ftdll, 7, 7, dbClkOut);
    U32_SET_FIELD_MAC(ftdll, 14, 7, dbStartBurst);
    /* set FTDLL*/
    rc = dbFtdllSet(devId, ftdll);
    if(rc != GT_OK)
    {
        return rc;
    }

    dbDqs = Dqs; /* save Dqs */
    return GT_OK;
}

/*******************************************************************************
* gtFtdllTigerClkOutSet
*
* DESCRIPTION:
*       No description available.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 ClkOut -
*       ClkOut for DB SDRAM FTDLL RAM
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerClkOutSet
(
    IN   GT_U8  devId,
    IN   GT_U32 ClkOut
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      ftdll;  /* FTDLL       */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devId);

    ftdll = 0;
    U32_SET_FIELD_MAC(ftdll, 0, 7, dbDqs);
    U32_SET_FIELD_MAC(ftdll, 7, 7, ClkOut);
    U32_SET_FIELD_MAC(ftdll, 14, 7, dbStartBurst);
    /* set FTDLL*/
    rc = dbFtdllSet(devId, ftdll);
    if(rc != GT_OK)
    {
        return rc;
    }

    dbClkOut = ClkOut; /* save ClkOut */
    return GT_OK;
}

/*******************************************************************************
* gtFtdllTigerStartBurstSet
*
* DESCRIPTION:
*       No description available.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 StartBurst -
*       StartBurst for DB SDRAM FTDLL RAM
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerStartBurstSet
(
    IN   GT_U8  devId,
    IN   GT_U32 StartBurst
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      ftdll;  /* FTDLL       */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devId);

    ftdll = 0;
    U32_SET_FIELD_MAC(ftdll, 0, 7, dbDqs);
    U32_SET_FIELD_MAC(ftdll, 7, 7, dbClkOut);
    U32_SET_FIELD_MAC(ftdll, 14, 7, StartBurst);
    /* set FTDLL*/
    rc = dbFtdllSet(devId, ftdll);
    if(rc != GT_OK)
    {
        return rc;
    }

    dbStartBurst = StartBurst; /* save ClkOut */
    return GT_OK;
}

/*******************************************************************************
* gtFtdllTigerWC2CSet
*
* DESCRIPTION:
*       No description available.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 ftdll -
*       Wide rx clk
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerWC2CSet
(
    IN   GT_U8  devId,
    IN   GT_U32 ftdll
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      regVal;  /* FTDLL      */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devId);

    regVal = 0;
    U32_SET_FIELD_MAC(regVal, 0, 7, ftdll);
    U32_SET_FIELD_MAC(regVal, 7, 7, c2cNarrowRx);
    /* set FTDLL */
    rc = uplinkFtdllSet(devId,regVal);
    if(rc != GT_OK)
    {
        return rc;
    }

    c2cWideRx = ftdll; /* save Narrow Rx Clock */
    return GT_OK;
}

/*******************************************************************************
* gtFtdllTigerNC2CSet
*
* DESCRIPTION:
*       No description available.
*
*   INPUTS:
*       GT_U8  devId -
*       Device Id
*
*       GT_U32 ftdll -
*       Narrow rx clk
*
*   OUTPUTS:
*       None.
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtFtdllTigerNC2CSet
(
    IN   GT_U8  devId,
    IN   GT_U32 ftdll
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      regVal;  /* FTDLL      */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devId);

    regVal = 0;
    U32_SET_FIELD_MAC(regVal, 0, 7, c2cWideRx);
    U32_SET_FIELD_MAC(regVal, 7, 7, ftdll);
    /* set FTDLL */
    rc = uplinkFtdllSet(devId,regVal);
    if(rc != GT_OK)
    {
        return rc;
    }

    c2cNarrowRx = ftdll; /* save Narrow Rx Clock */
    return GT_OK;
}


/*******************************************************************************
* gtDfcdlAutoScanDqsGetTableEntry
*
* DESCRIPTION:
*       Get first / next entry of the dfcdlAutoScanDqs table.
*
*   INPUTS:
*       GT_U8  devNum -
*       device number
*
*       GT_U32 dqsStart -
*       dqs first value
*
*       GT_U32 dqsEnd -
*       dqs last value
*
*       GT_U32 waitTime -
*       time to wait before read MIB counters
*
*       dfcdlAutoScanDqs_IN_trafficType trafficType -
*       internal / external traffic
*
*       dfcdlAutoScanDqs_FIELDS tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   -
*           index of current entry refresh index.
*
*   OUTPUTS:
*       dfcdlAutoScanDqs_FIELDS tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtDfcdlAutoScanDqsGetTableEntry
(
    IN   GT_U8  devNum,
    IN   GT_U32 dqsStart,
    IN   GT_U32 dqsEnd,
    IN   GT_U32 waitTime,
    IN   dfcdlAutoScanDqs_IN_trafficType    trafficType,
    INOUT dfcdlAutoScanDqs_FIELDS   *tableFields,
    IN  GT_U32  entryIndex
)
{
    GT_STATUS   rc = GT_OK;   /* Return code */
    GT_U32      regAddr = 0;  /* Holds register address */
    GT_U32      fieldVal = 0; /* Use to set and get the register field value */
    DFCDL_BUFFER_DRAM_PARAMS scanParams; /* scan parameters */
    GT_U64      errorCnt;     /* counts error CRC */
    GT_BOOL     isFirst = GT_FALSE;  /* is first entry   */
    GT_BOOL     isLast = GT_FALSE;   /* is last entry    */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devNum);

    if (dqsEnd < dqsStart)
    {
        return GT_BAD_PARAM;
    }

    if (entryIndex == (dqsEnd - dqsStart + 1))
    {
        /* no more dqs valid value */
        return GT_NO_MORE;
    }

    if (entryIndex == (dqsEnd - dqsStart))
    {
        /* last valid dqs value */
        isLast = GT_TRUE;
    }

    if (entryIndex == 0)
    {
        /* first entry */
        isFirst = GT_TRUE;

        /* get the start burst & clkOut parameters from HW */
        /* read register - DB SDRAN FTDLL read data register */
        regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
            extMemRegs.dbFtdllReadDataReg;
        rc = cpssDrvPpHwRegisterRead(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,&fieldVal);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* clkOut - bis 7-13 */
        constClkOut = U32_GET_FIELD_MAC(fieldVal,7,7);

        /* startBurst - bis 14-20 */
        constStartBurst = U32_GET_FIELD_MAC(fieldVal,14,7);

    }

    /* Set scan parameters */

    /* curr dqs parameter = dqs first value + current index */
    scanParams.dqs = (GT_U8)(dqsStart + entryIndex);

    if (dfcdlAutoScanDqs_IN_trafficType_INTERNAL == trafficType)
    {
        scanParams.extTraffic = GT_FALSE;
    }
    else
    {
        scanParams.extTraffic = GT_TRUE;
    }

    scanParams.testDelay = waitTime;

    scanParams.clkOut = (GT_U8)constClkOut;
    scanParams.startBurst = (GT_U8)constStartBurst;


    /* call to the dfcdl function */
    rc = gtDfcdlPpScan(devNum,&scanParams,isFirst,isLast,&errorCnt);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* set the output parameter */
    tableFields->tableFields.General.Dqs = scanParams.dqs;
    tableFields->tableFields.General.errorCounterLow = errorCnt.l[0];
    tableFields->tableFields.General.errorCounterHigh = errorCnt.l[1];

    return GT_OK;
}

/*******************************************************************************
* gtDfcdlAutoScanClkOutGetTableEntry
*
* DESCRIPTION:
*       Get first / next entry of the dfcdlAutoScanClkOut table.
*
*   INPUTS:
*       GT_U8  devNum -
*       device number
*
*       GT_U32 clkOutStart -
*       first clkOut value
*
*       GT_U32 clkOutLast -
*       last clkOut value
*
*       GT_U32 waitTime -
*       time to wait to CRC error
*
*       dfcdlAutoScanClkOut_IN_trafficType trafficType -
*       internal / external traffic
*
*       dfcdlAutoScanClkOut_FIELDS  tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   -
*           index of current entry refresh index.
*
*   OUTPUTS:
*       dfcdlAutoScanClkOut_FIELDS  tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtDfcdlAutoScanClkOutGetTableEntry
(
    IN   GT_U8  devNum,
    IN   GT_U32 clkOutStart,
    IN   GT_U32 clkOutLast,
    IN   GT_U32 waitTime,
    IN   dfcdlAutoScanClkOut_IN_trafficType trafficType,
    INOUT dfcdlAutoScanClkOut_FIELDS    *tableFields,
    IN  GT_U32  entryIndex
)
{
    GT_STATUS   rc = GT_OK;   /* Return code */
    GT_U32      regAddr = 0;  /* Holds register address */
    GT_U32      fieldVal = 0; /* Use to set and get the register field value */
    DFCDL_BUFFER_DRAM_PARAMS scanParams; /* scan parameters */
    GT_U64      errorCnt;     /* counts error CRC */
    GT_BOOL     isFirst = GT_FALSE;  /* is first entry   */
    GT_BOOL     isLast = GT_FALSE;   /* is last entry    */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devNum);

    if (clkOutLast < clkOutStart)
    {
        return GT_BAD_PARAM;
    }


    if (entryIndex == (clkOutLast - clkOutStart + 1))
    {
        /* no more clkOut valid value */
        return GT_NO_MORE;
    }

    if (entryIndex == (clkOutLast - clkOutStart))
    {
        /* last valid clkOut value */
        isLast = GT_TRUE;
    }

    if (entryIndex == 0)
    {
        /* first entry */
        isFirst = GT_TRUE;

        /* get the start burst & dqs parameters from HW */
        /* read register - DB SDRAN FTDLL read data register */
        regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
            extMemRegs.dbFtdllReadDataReg;
        rc = cpssDrvPpHwRegisterRead(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,&fieldVal);

        /* dqs - bis 0-6 */
        constDqs = U32_GET_FIELD_MAC(fieldVal,0,7);

        /* startBurst - bis 14-20 */
        constStartBurst = U32_GET_FIELD_MAC(fieldVal,14,7);
    }

    /* Set scan parameters */

    /* curr clkOut parameter = clkOut first value + current index */
    scanParams.clkOut = (GT_U8)(clkOutStart + entryIndex);

    if (dfcdlAutoScanClkOut_IN_trafficType_INTERNAL == trafficType)
    {
        scanParams.extTraffic = GT_FALSE;
    }
    else
    {
        scanParams.extTraffic = GT_TRUE;
    }

    scanParams.testDelay = waitTime;

    scanParams.dqs = (GT_U8)constDqs;
    scanParams.startBurst = (GT_U8)constStartBurst;



    /* call to the dfcdl function */
    rc = gtDfcdlPpScan(devNum,&scanParams,isFirst,isLast,&errorCnt);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* set the output parameter */
    tableFields->tableFields.General.clkOut = scanParams.clkOut;
    tableFields->tableFields.General.errorCounterLow = errorCnt.l[0];
    tableFields->tableFields.General.errorCounterHigh = errorCnt.l[1];

    return GT_OK;
  }

/*******************************************************************************
* gtDfcdlAutoScanStartBurstGetTableEntry
*
* DESCRIPTION:
*       Get first / next entry of the dfcdlAutoScanStartBurst table.
*
*   INPUTS:
*       GT_U8  devNum -
*       device number
*
*       GT_U32 startBurstStart -
*       first startBurst value
*
*       GT_U32 lastBurstStart -
*       last startBurst value
*
*       GT_U32 waitTime -
*       wait time
*
*       dfcdlAutoScanStartBurst_IN_traffictype traffictype -
*       internal / external trrafic
*
*       dfcdlAutoScanStartBurst_FIELDS  tableFields -
*           the Galtis table fields
*
*       GT_U32 entryIndex   -
*           index of current entry refresh index.
*
*   OUTPUTS:
*       dfcdlAutoScanStartBurst_FIELDS  tableFields -
*            the Galtis table output fields
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtDfcdlAutoScanStartBurstGetTableEntry
(
    IN   GT_U8  devNum,
    IN   GT_U32 startBurstStart,
    IN   GT_U32 lastBurstStart,
    IN   GT_U32 waitTime,
    IN   dfcdlAutoScanStartBurst_IN_traffictype traffictype,
    INOUT dfcdlAutoScanStartBurst_FIELDS    *tableFields,
    IN  GT_U32  entryIndex
)
{
    GT_STATUS   rc = GT_OK;   /* Return code */
    GT_U32      regAddr = 0;  /* Holds register address */
    GT_U32      fieldVal = 0; /* Use to set and get the register field value */
    DFCDL_BUFFER_DRAM_PARAMS scanParams; /* scan parameters */
    GT_U64      errorCnt;     /* counts error CRC */
    GT_BOOL     isFirst = GT_FALSE;  /* is first entry   */
    GT_BOOL     isLast = GT_FALSE;   /* is last entry    */

    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devNum);

    if (lastBurstStart < startBurstStart)
    {
        return GT_BAD_PARAM;
    }

    if (entryIndex == (lastBurstStart - startBurstStart + 1))
    {
        /* no more startBurst valid value */
        return GT_NO_MORE;
    }

    if (entryIndex == (lastBurstStart - startBurstStart))
    {
        /* last valid startBurst value */
        isLast = GT_TRUE;
    }

    if (entryIndex == 0)
    {
        /* first entry */
        isFirst = GT_TRUE;

        /* get the clkOut & dqs parameters from HW */
        /* read register - DB SDRAN FTDLL read data register */
        regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->
            extMemRegs.dbFtdllReadDataReg;
        rc = cpssDrvPpHwRegisterRead(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,&fieldVal);
        if (GT_OK != rc)
        {
            return rc;
        }

        /* dqs - bis 0-6 */
        constDqs = U32_GET_FIELD_MAC(fieldVal,0,7);

        /* clkOut - bis 7-13 */
        constClkOut = U32_GET_FIELD_MAC(fieldVal,7,7);

    }

    /* Set scan parameters */

    /* curr startBurst parameter = startBurst first value + current index */
    scanParams.startBurst = (GT_U8)(startBurstStart + entryIndex);

    if (dfcdlAutoScanStartBurst_IN_traffictype_INTERNAL == traffictype)
    {
        scanParams.extTraffic = GT_FALSE;
    }
    else
    {
        scanParams.extTraffic = GT_TRUE;
    }

    scanParams.testDelay = waitTime;
    scanParams.dqs = (GT_U8)constDqs;
    scanParams.clkOut = (GT_U8)constClkOut;


    /* call to the dfcdl function */
    rc = gtDfcdlPpScan(devNum,&scanParams,isFirst,isLast,&errorCnt);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* set the output parameter */
    tableFields->tableFields.General.startBurst = scanParams.startBurst;
    tableFields->tableFields.General.errorCounterLow = errorCnt.l[0];
    tableFields->tableFields.General.errorCounterHigh = errorCnt.l[1];

    return GT_OK;
}


/*******************************************************************************
* eccPpWideC2cStop
*
* DESCRIPTION:
*       This is an interrupt handler function, its called uppon receiving
*       ecc interrupt.
*
* INPUTS:
*       devNum  - The device number from which this interrupt was received.
*       uniEv   - The event type.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - On success,
*       GT_FAIL - Otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS eccFoxC2cStop
(
    IN GT_U8        devNum,
    IN GT_U32       uniEv
)
{
    GT_U32  sdlValue; /* value to write to ftdll */

    if ((uniEv == CPSS_PP_MISC_C2C_DATA_ERR_E) ||
        (uniEv == CPSS_PP_MISC_MSG_TIME_OUT_E) ||
        (uniEv == CPSS_PP_MISC_UPLINK_W_ECC_ON_DATA_E) ||
        (uniEv == CPSS_PP_MISC_UPLINK_W_ECC_ON_HEADER_E) ||
        (uniEv == CPSS_PP_MISC_UPLINK_N_ECC_ON_DATA_E) ||
        (uniEv == CPSS_PP_MISC_UPLINK_N_ECC_ON_HEADER_E))
    {
        /* set ftdllAutoScanContinue to indicate stop scanning */
        ftdllAutoScanContinue = GT_FALSE;

        sdlValue = 0;
        U32_SET_FIELD_MAC(sdlValue, 0, 7, eccWc2cVal);
        U32_SET_FIELD_MAC(sdlValue, 7, 7, eccNc2cVal); /* write current parameter */

        /* set FTDLL for ISR */
        uplinkFtdllIsrSet(devNum,sdlValue);
    }

    return GT_OK;
}

/*******************************************************************************
* gtGtFtdllTigerWC2CAutoScan
*
* DESCRIPTION:
*       Perform a wide uplink scan to avoid system stuck during scan
*       Tiger-12G and Tiger-XG and return the min-max WC2C values
*
*   INPUTS:
*       GT_U8  devNum -
*       device number
*
*       GT_U32 ftdllWC2CStart -
*       WC2C Start FTDLL Value
*
*       GT_U32 ftdllWC2CEnd -
*       WC2C End FTDLL Value
*
*       GT_U32 waitTime -
*       Wait time to read ECC event
*
*       gtFtdllTigerWC2CAutoScan_IN_trafficType trafficType -
*       internal / external traffic
*
*   OUTPUTS:
*       GT_U32 minVal -
*       minimal WC2C value that cause event
*
*       GT_U32 maxVal -
*       maximal WC2C value that cause event
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtGtFtdllTigerWC2CAutoScan
(
    IN   GT_U8  devNum,
    IN   GT_U32 ftdllWC2CStart,
    IN   GT_U32 ftdllWC2CEnd,
    IN   GT_U32 waitTime,
    IN   gtFtdllTigerWC2CAutoScan_IN_trafficType    trafficType,
    OUT  GT_U32 *minVal,
    OUT  GT_U32 *maxVal
)
{
    GT_STATUS   rc = GT_OK;   /* Return code */
    GT_U32      regAddr = 0;  /* Holds register address */
    GT_U32      fieldVal = 0; /* Use to set and get the register field value */
    GT_U32      nC2c;         /* nrw_RxClk_DelayVal */
    GT_U32      i;            /* loop index */
    GT_U32      midVal;       /* middle value to start scan */

    /* chck device validity */
    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devNum);

    updateWideUplinkCommand = GT_TRUE;
    if (wideC2cAutoScanEvReqHndl == 0)
    {
        /* this is the first time, create TxEnd Hhandle */
        CPSS_UNI_EV_CAUSE_ENT     evCause[4] =
               {CPSS_PP_MISC_C2C_DATA_ERR_E,
                CPSS_PP_MISC_MSG_TIME_OUT_E,
                CPSS_PP_MISC_UPLINK_W_ECC_ON_DATA_E,
                CPSS_PP_MISC_UPLINK_W_ECC_ON_HEADER_E};

        rc = cpssEventBind(evCause, 4, &wideC2cAutoScanEvReqHndl);
        RETURN_ON_ERROR(rc);

        rc = dfcdlScanEccEventEnable(devNum);
        RETURN_ON_ERROR(rc);
    }

    rc = cpssOsTaskCreate("Tiger_Wide_C2C_Auto_Scan" , /* Task Name */
                          DFCDL_TASK_PRIORITY        , /* Task Priority */
                          DFCDL_TASK_STACK_SIZE      , /* Stack Size */
                          (unsigned (__TASKCONV *)     /* function to run */
                          (void*))tgWideC2cAutoScanTaskFunc,
                          NULL                       , /* Arguments list */
                          &wideC2cAutoScanTaskId);     /* task ID */
    RETURN_ON_ERROR(rc);

    /* set scan configuration */
    rc = gtDfcdlScanSetConfig(devNum,GT_TRUE,trafficType,GT_TRUE,
                              GT_PATTERN_TYPE_SS32);
    if (GT_OK != rc)
    {
        return rc;
    }


    /* get NC2C parameter from HW -
    read register Uplink FTDLL Sram Read Data Register*/
    regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srReadDataReg;
    rc = cpssDrvPpHwRegisterRead(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,&fieldVal);
    if (GT_OK != rc)
    {
        return GT_OK;
    }

    /* get NC2C from bits - 7:13 */
    nC2c = U32_GET_FIELD_MAC(fieldVal,7,7);

    eccNc2cVal = nC2c;

    ftdllAutoScanContinue = GT_TRUE;

    midVal = ((ftdllWC2CEnd + ftdllWC2CStart) / 2);


    for (i = midVal;
         (ftdllAutoScanContinue) && (i >= ftdllWC2CStart) && (i != -1);
          i--)
    {
        /* scan the wc2c values - from end to start
           stop when get ecc events - indicate by the
           ftdllAutoScanContinue parameter */

        /* set value of WC2C to write in case of ecc event */
        if (i == midVal)
        {
            eccWc2cVal = midVal;
        }
        else
        {
            eccWc2cVal = (i + 1);
        }


        /* write ftdll parameters */
        fieldVal = 0;
        U32_SET_FIELD_MAC(fieldVal, 0, 7, i); /* write current parameter */
        U32_SET_FIELD_MAC(fieldVal, 7, 7, nC2c);

        /* set FTDLL */
        rc = uplinkFtdllSet(devNum,fieldVal);
        if (GT_OK != rc)
        {
            return rc;
        }

        rc = cpssOsTimerWkAfter(waitTime);
        if (GT_OK != rc)
        {
            return rc;
        }

    }

    /* exit from loop if get ecc error or if scan all valid Wc2c values */
    *minVal = (i + 1);

    ftdllAutoScanContinue = GT_TRUE;

    for (i = midVal; (ftdllAutoScanContinue) && (i <= ftdllWC2CEnd);
          i++)
    {
        /* scan the wc2c values - from start to end
           stop when get ecc events - indicate by the
           ftdllAutoScanContinue parameter */

        /* set value of WC2C to write in case of ecc event */
        if (i == midVal)
        {
            eccWc2cVal = midVal;
        }
        else
        {
            eccWc2cVal = (i - 1);
        }

        /* write ftdll parameters */
        fieldVal = 0;
        U32_SET_FIELD_MAC(fieldVal, 0, 7, i); /* write current parameter */
        U32_SET_FIELD_MAC(fieldVal, 7, 7, nC2c);

        /* set FTDLL */
        rc = uplinkFtdllSet(devNum,fieldVal);
        if (GT_OK != rc)
        {
            return rc;
        }

        rc = cpssOsTimerWkAfter(waitTime);
        if (GT_OK != rc)
        {
            return rc;
        }

    }

    /* exit from loop if get ecc error or if scan all valid Wc2c values */
    *maxVal = (i - 1);

    updateWideUplinkCommand = GT_FALSE;

    /* end of scan configuration */
    rc = gtDfcdlScanSetConfig(devNum,GT_FALSE,trafficType,GT_TRUE,
                              GT_PATTERN_TYPE_SS32);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssOsTaskDelete (wideC2cAutoScanTaskId);
    return rc;
}

/*******************************************************************************
* gtGtFtdllTigerNC2CAutoScan
*
* DESCRIPTION:
*       Perform a Narrow uplink scan to avoid system stuck during scan
*       Tiger-12G and Tiger-XG and return minial and maximal values that
*       caused interrupts
*
*   INPUTS:
*       GT_U8  devNum -
*       device number
*
*       GT_U32 ftdllNC2CStart -
*       NC2C Start FTDLL Value
*
*       GT_U32 ftdllNC2CStop -
*       NC2C Stop FTDLL Value
*
*       GT_U32 waitTime -
*       Wait time to read ECC event
*
*       gtFtdllTigerNC2CAutoScan_IN_trafficType trafficType -
*       internal / external traffic
*
*   OUTPUTS:
*       GT_U32 minVal -
*       minimal NC2C value that caused  events
*
*       GT_U32 maxVal -
*       maximal NC2C value that caused  events
*
*
*   COMMENTS:
*       None.
*
*   RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - bad input value
*       GT_FAIL - on error
*
*******************************************************************************/
GT_STATUS gtGtFtdllTigerNC2CAutoScan
(
    IN   GT_U8  devNum,
    IN   GT_U32 ftdllNC2CStart,
    IN   GT_U32 ftdllNC2CStop,
    IN   GT_U32 waitTime,
    IN   gtFtdllTigerNC2CAutoScan_IN_trafficType    trafficType,
    OUT  GT_U32 *minVal,
    OUT  GT_U32 *maxVal
)
{

    GT_STATUS   rc = GT_OK;   /* Return code */
    GT_U32      regAddr = 0;  /* Holds register address */
    GT_U32      fieldVal = 0; /* Use to set and get the register field value */
    GT_U32      wC2c;         /* Wide_RxClk_DelayVal */
    GT_U32      i;            /* loop index */
    GT_U32      midVal;       /* middle value to start scan */


    /* chck device validity */
    PRV_CPSS_EXMXTG_DEV_CHECK_MAC(devNum);

    updateNarrowUplinkCommand = GT_TRUE;
    if (narrowC2cAutoScanEvReqHndl == 0)
    {
        /* this is the first time, create TxEnd Hhandle */
        CPSS_UNI_EV_CAUSE_ENT     evCause[4] =
               {CPSS_PP_MISC_C2C_DATA_ERR_E,
                CPSS_PP_MISC_MSG_TIME_OUT_E,
                CPSS_PP_MISC_UPLINK_N_ECC_ON_DATA_E,
                CPSS_PP_MISC_UPLINK_N_ECC_ON_HEADER_E};

        rc = cpssEventBind(evCause, 4, &narrowC2cAutoScanEvReqHndl);
        RETURN_ON_ERROR(rc);

        rc = dfcdlScanEccEventEnable(devNum);
        RETURN_ON_ERROR(rc);
    }

    rc = cpssOsTaskCreate("Tiger_Narrow_C2C_Auto_Scan", /* Task Name */
                          DFCDL_TASK_PRIORITY        , /* Task Priority */
                          DFCDL_TASK_STACK_SIZE      , /* Stack Size */
                          (unsigned (__TASKCONV *)     /* function to run */
                          (void*))tgNarrowC2cAutoScanTaskFunc,
                          NULL                       , /* Arguments list */
                          &narrowC2cAutoScanTaskId);   /* task ID */
    RETURN_ON_ERROR(rc);

    /* set scan configuration */
    rc = gtDfcdlScanSetConfig(devNum,GT_TRUE,trafficType,GT_TRUE,
                              GT_PATTERN_TYPE_SS32);
    if (GT_OK != rc)
    {
        return rc;
    }


    /* get WC2C parameter from HW -
    read register Uplink FTDLL Sram Read Data Register*/
    regAddr = PRV_CPSS_EXMX_DEV_REGS_MAC(devNum)->extMemRegs.srReadDataReg;
    rc = cpssDrvPpHwRegisterRead(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,&fieldVal);
    if (GT_OK != rc)
    {
        return GT_OK;
    }

    /* get WC2C from bits - 0:6 */
    wC2c = U32_GET_FIELD_MAC(fieldVal,0,7);

    eccWc2cVal = wC2c;

    ftdllAutoScanContinue = GT_TRUE;

    midVal = ((ftdllNC2CStop + ftdllNC2CStart) / 2);


    for (i = midVal;
         (ftdllAutoScanContinue) && (i >= ftdllNC2CStart) && (i != -1);
          i--)
    {
        /* scan the nc2c values - from end to start
           stop when get ecc events - indicate by the
           ftdllAutoScanContinue parameter */

        /* set value of NC2C to write in case of ecc event */
        if (i == midVal)
        {
            eccNc2cVal = midVal;
        }
        else
        {
            eccNc2cVal = (i + 1);
        }


        /* write ftdll parameters */
        fieldVal = 0;
        U32_SET_FIELD_MAC(fieldVal, 0, 7, wC2c);
        U32_SET_FIELD_MAC(fieldVal, 7, 7, i); /* write current parameter */

        /* set FTDLL */
        rc = uplinkFtdllSet(devNum,fieldVal);
        if (GT_OK != rc)
        {
            return rc;
        }

        rc = cpssOsTimerWkAfter(waitTime);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    /* exit from loop if get ecc error or if scan all valid Nc2c values */
    *minVal = (i + 1);

    ftdllAutoScanContinue = GT_TRUE;


    for (i = midVal; (ftdllAutoScanContinue) && (i <= ftdllNC2CStop);
          i++)
    {
        /* scan the nc2c values - from start to end
           stop when get ecc events - indicate by the
           ftdllAutoScanContinue parameter */

        /* set value of NC2C to write in case of ecc event */
        if (i == midVal)
        {
            eccNc2cVal = midVal;
        }
        else
        {
            eccNc2cVal = (i - 1);
        }

        /* write ftdll parameters */
        fieldVal = 0;
        U32_SET_FIELD_MAC(fieldVal, 0, 7, wC2c);
        U32_SET_FIELD_MAC(fieldVal, 7, 7, i); /* write current parameter */

        /* set FTDLL */
        rc = uplinkFtdllSet(devNum,fieldVal);
        if (GT_OK != rc)
        {
            return rc;
        }

        rc = cpssOsTimerWkAfter(waitTime);
        if (GT_OK != rc)
        {
            return rc;
        }
    }


    /* exit from loop if get ecc error or if scan all valid Nc2c values */
    *maxVal = (i - 1);

    updateNarrowUplinkCommand = GT_TRUE;

    /* end of scan configuration */
    rc = gtDfcdlScanSetConfig(devNum,GT_FALSE,trafficType,GT_TRUE,
                              GT_PATTERN_TYPE_SS32);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssOsTaskDelete (narrowC2cAutoScanTaskId);
    return rc;
}

/*******************************************************************************
* tgWideC2cGetEntryScanTaskFunc
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
GT_STATUS __TASKCONV tgWideC2cGetEntryScanTaskFunc
(
    GT_VOID
)
{
    GT_STATUS status;

    while (GT_TRUE)
    {
        status = eccEventHandle (wideC2cGetEntryEvReqHndl, doWideUplinkCount,
                                 &dfcdlEccPpC2cCount);
        if (status != GT_OK)
        {
            break;
        }
    }
    return status;
}

/*******************************************************************************
* tgNarrowC2cGetEntryScanTaskFunc
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
GT_STATUS __TASKCONV tgNarrowC2cGetEntryScanTaskFunc
(
    GT_VOID
)
{
    GT_STATUS status;

    while (GT_TRUE)
    {
        status = eccEventHandle (narrowC2cGetEntryEvReqHndl,
                                 doNarrowUplinkCount, &dfcdlEccPpC2cCount);
        if (status != GT_OK)
        {
            break;
        }
    }
    return status;
}

/*******************************************************************************
* tgWideC2cAutoScanTaskFunc
*
* DESCRIPTION:
*       This task waits for events of type ECC error on the wide uplink
*       interface. Upon receiving of such events it updates the uplink DFCDL
*       setting command.
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
GT_STATUS __TASKCONV tgWideC2cAutoScanTaskFunc
(
    GT_VOID
)
{
    GT_STATUS status;

    while (GT_TRUE)
    {
        status = eccEventHandle (wideC2cAutoScanEvReqHndl,
                                 updateWideUplinkCommand, &eccFoxC2cStop);
        if (status != GT_OK)
        {
            break;
        }
    }
    return status;
}

/*******************************************************************************
* tgNarrowC2cAutoScanTaskFunc
*
* DESCRIPTION:
*       This task waits for events of type ECC error on the narrow uplink
*       interface. Upon receiving of such events it updates the uplink DFCDL
*       setting command.
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
GT_STATUS __TASKCONV tgNarrowC2cAutoScanTaskFunc
(
    GT_VOID
)
{
    GT_STATUS status;

    while (GT_TRUE)
    {
        status = eccEventHandle (narrowC2cAutoScanEvReqHndl,
                                 updateNarrowUplinkCommand, &eccFoxC2cStop);
        if (status != GT_OK)
        {
            break;
        }
    }
    return status;
}

/************************************* debug functions ************************/
void debugUplinkAutoScan
(
    IN   GT_U8  devNum,
    IN   GT_U32 type, /* 0 - wc2c, 1 - nc2c */
    IN   GT_U32 start,
    IN   GT_U32 stop,
    IN   GT_U32 waitTime,
    IN   GT_U32 trafficType
)
{
    GT_U32 min,max;

    if (type == 0)
    {
        if (GT_OK != gtGtFtdllTigerWC2CAutoScan(devNum,start,stop,waitTime,
                                                trafficType,&min,&max))
        {
            cpssOsPrintf("\nError");
        }

    }

    if (type == 1)
    {
        if (GT_OK != gtGtFtdllTigerNC2CAutoScan(devNum,start,stop,waitTime,
                                                trafficType,&min,&max))
        {
            cpssOsPrintf("\nError");
        }
    }

    cpssOsPrintf("\nmin - %d, max - %d", min, max);

    return;
}


void debugAutoScan(
    IN   GT_U8  devNum,
    IN   GT_U32 type, /* 0 - dqs, 1 - clkOut, 2 startBurst */
    IN   GT_U32 start,
    IN   GT_U32 last,
    IN   GT_U32 trafficType,
    IN   GT_U32 waitTime
)
{
    GT_U32 i;
    GT_STATUS rc;


    for (i = 0; i < (last - start + 1); i++)
    {
        if (0 == type)
        {
            dfcdlAutoScanDqs_FIELDS fields;
            rc =  gtDfcdlAutoScanDqsGetTableEntry(devNum,start,last,waitTime,
                                                  trafficType,&fields,i);
            if (GT_OK != rc)
            {
                cpssOsPrintf("\nERROR\n");
                return;
            }

            cpssOsPrintf("\nIndex - %d, error - %d",i,
                         fields.tableFields.General.errorCounterLow);
        }
        if (1 == type)
        {
            dfcdlAutoScanClkOut_FIELDS fields;
            rc =  gtDfcdlAutoScanClkOutGetTableEntry(devNum,start,last,waitTime,
                                                  trafficType,&fields,i);
            if (GT_OK != rc)
            {
                cpssOsPrintf("\nERROR\n");
                return;
            }

            cpssOsPrintf("\nIndex - %d, error - %d",i,
                         fields.tableFields.General.errorCounterLow);
        }
        if (2 == type)
        {
            dfcdlAutoScanStartBurst_FIELDS fields;
            rc =  gtDfcdlAutoScanStartBurstGetTableEntry(devNum,start,last,
                                                         waitTime,
                                                  trafficType,&fields,i);
            if (GT_OK != rc)
            {
                cpssOsPrintf("\nERROR\n");
                return;
            }

            cpssOsPrintf("\nIndex - %d, error - %d",i,
                         fields.tableFields.General.errorCounterLow);
        }
    }

    return;
}

