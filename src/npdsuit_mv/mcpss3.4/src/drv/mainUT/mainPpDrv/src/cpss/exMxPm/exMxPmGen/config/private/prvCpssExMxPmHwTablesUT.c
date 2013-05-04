/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssExMxPmHwTablesUT.c
*
* DESCRIPTION:
*     test private API implementation for tables access
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmTables.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#include <cpss/exMxPm/exMxPmGen/cpssHwInit/private/prvCpssExMxPmHwEccCalc.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsTimer.h>
#include <cpss/exMxPm/exMxPmGen/tti/private/prvCpssExMxPmTti.h>
#include <cpss/exMxPm/exMxPmGen/pcl/private/prvCpssExMxPmPcl.h>


GT_U32 prvExMxPmTestMemReadWrite = 0;/* variable for linking usage */

static PRV_CPSS_EXMXPM_TABLE_INFO_STC dummyTable;

/* table for ecc auto correction test */
static PRV_CPSS_EXMXPM_TABLE_INFO_STC eccAutoCorrectTable;

static GT_U32 dummyTableInit = 0;
#define TEST_NUM_WORDS  8

#ifdef ASIC_SIMULATION
static GT_U32   doSleep = 0;
#endif /*ASIC_SIMULATION*/

#define STR_NAME(index) \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_LUT_E                 )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_LUT_E                 ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_FDB_E                 )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_FDB_E                 ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_EPCL_ACTION_E         )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_EPCL_ACTION_E         ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_IPCL_ACTION_E         )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_IPCL_ACTION_E         ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_INLIF_E               )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_INLIF_E               ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_TTI_ACTION_REDUCED_E  )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_TTI_ACTION_REDUCED_E  ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_TTI_ACTION_STANDARD_E )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_TTI_ACTION_STANDARD_E ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_TTI_ACTION_EXTENDED_E )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_TTI_ACTION_EXTENDED_E ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_NH_UC_RPF_ECMP_E      )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_NH_UC_RPF_ECMP_E      ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_NH_UC_ROUTE_E         )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_NH_UC_ROUTE_E         ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_NH_MC_ROUTE_E         )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_NH_MC_ROUTE_E         ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_MPLS_NHLFE_E          )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_MPLS_NHLFE_E          ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_IPCL_ACTION_REDUCED_E )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_IPCL_ACTION_REDUCED_E ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_IP_UC_E           )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_IP_UC_E           ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_IP_MC_E           )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_IP_MC_E           ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_MPLS_UC_E         )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_MPLS_UC_E         ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_MPLS_MC_E         )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_MPLS_MC_E         ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_VPLS_UC_E         )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_VPLS_UC_E         ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_VPLS_MC_E         )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_VPLS_MC_E         ": \
    (index == (CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_IP_MC_PAIR_E      )) ? "CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_DIT_IP_MC_PAIR_E      ": \
    "???"

/*******************************************************************************
* testCpssExMxPmExternalMemWrite
*
* DESCRIPTION:
*       test function that write to external memory
*
* INPUTS:
*       devNum          - the device number
*       numBitsInEntry  - number of bits in the entry
*       dataContainParityBit -  GT_TRUE  - the data has 'Place holder' for 'Parity per line' mode
*                               GT_FALSE - the data not hold place for parity bit
*       word0-7         - data to write to memory
*
*
* OUTPUT:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS testCpssExMxPmExternalMemWrite
(
    IN GT_U8                devNum,
    IN GT_U32               startIndex,
    IN GT_U32               numBitsInEntry,
    IN GT_U32               dataContainParityBit,
    IN GT_U32               calcEcc,
    IN GT_U32               word0,/*32 bits*/
    IN GT_U32               word1,/*64 bits*/
    IN GT_U32               word2,/*96 bits*/
    IN GT_U32               word3,/*128 bits*/
    IN GT_U32               word4,/*160 bits*/
    IN GT_U32               word5,/*192 bits*/
    IN GT_U32               word6,/*224 bits*/
    IN GT_U32               word7 /*256 bits*/
)
{
    GT_U32  entryWords[TEST_NUM_WORDS];

    entryWords[0] = word0;
    entryWords[1] = word1;
    entryWords[2] = word2;
    entryWords[3] = word3;
    entryWords[4] = word4;
    entryWords[5] = word5;
    entryWords[6] = word6;
    entryWords[7] = word7;

    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(devNum);

    if(numBitsInEntry > (TEST_NUM_WORDS*32))
    {
        return GT_BAD_PARAM;
    }

    if(dummyTableInit == 0)
    {
        dummyTableInit = 1;
        /* set table parameters */
#ifdef DEBUG_REG_AND_TBL_NAME
        dummyTable.tableName = "dummy table for testing";
#endif /*DEBUG_REG_AND_TBL_NAME*/

        /* set base line as offset in the CSU-0 */
        dummyTable.baseAddr =
            PRV_CPSS_EXMXPM_PP_MAC(devNum)->tblsAddr.CSU0.externalCtrlMemory.baseAddr;

        dummyTable.memType = 1;/*external memory */
        /* set alignment as in CSU-0 */
        dummyTable.lineAddrAlign =
            PRV_CPSS_EXMXPM_PP_MAC(devNum)->tblsAddr.CSU0.externalCtrlMemory.lineAddrAlign;
        /* bind table to memory 'Csu-0' */
        dummyTable.memCtrlPtr =
            &(PRV_CPSS_EXMXPM_PP_MAC(devNum)->extMemsInfo.prvCpssExmxpmMemoryControllerInfoCsu0.controlerInfo);
        dummyTable.swNeedCalcEccOrParity = GT_TRUE;
        dummyTable.callerUseSpecificLineIndex = GT_FALSE;
    }
    dummyTable.numOfEntries = startIndex + 4;/* allow write of up to 4 lines */
    dummyTable.entrySize = numBitsInEntry;
    dummyTable.dataContainParityBit = dataContainParityBit ? GT_TRUE : GT_FALSE;
    dummyTable.memCtrlPtr->memCheckType = calcEcc ?
                    PRV_CPSS_EXMXPM_MEMORY_CHECK_ECC_E :
                    PRV_CPSS_EXMXPM_MEMORY_CHECK_PARITY_PER_LINE_E;

    return prvCpssExMxPmWriteTableEntry(devNum,&dummyTable,startIndex,entryWords);

}

/*******************************************************************************
* testCpssExMxPmExternalMemRead
*
* DESCRIPTION:
*       test function that read to external memory, function print the data read
*
* INPUTS:
*       devNum          - the device number
*       startIndex      - start index in the control memory
*
* OUTPUT:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In Puma the data is updated only when the last word in the entry was written
*
*******************************************************************************/
GT_STATUS testCpssExMxPmExternalMemRead
(
    IN GT_U8                devNum,
    IN GT_U32               startIndex
)
{
    GT_STATUS rc;
    GT_U32  entryWords[TEST_NUM_WORDS]={0,0,0,0,0,0,0,0};

    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(devNum);

    rc = prvCpssExMxPmReadTableEntry(devNum,&dummyTable,startIndex,entryWords);
    if(rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("testCpssExMxPmExternalMemRead : [0x%8.8x] [0x%8.8x] [0x%8.8x] [0x%8.8x] [0x%8.8x] [0x%8.8x] [0x%8.8x] [0x%8.8x]\n",
                  entryWords[0],
                  entryWords[1],
                  entryWords[2],
                  entryWords[3],
                  entryWords[4],
                  entryWords[5],
                  entryWords[6],
                  entryWords[7]);

    return GT_OK;
}


/*******************************************************************************
* prvCompArraysByBit
*
* DESCRIPTION:
*       The function compare the content of two arrays by the amount of bits
*       specified.
*
* INPUTS:
*       numOfBits   - the number of bits to compare
*       dataAPtr    - (pointer to) first data to compare
*       dataBPtr    - (pointer to) second data to compare
*
* OUTPUT:
*       isEqualPtr  - (pointer to) indicate the compare result:
*                     GT_TRUE - the data is the same
*                     GT_FALSE - the data is different
*
* RETURNS:
*       GT_OK
*
*******************************************************************************/
static GT_STATUS prvCompArraysByBit
(
    IN GT_U32 numOfBits,
    IN GT_U32 *dataAPtr,
    IN GT_U32 *dataBPtr,
    OUT GT_BOOL *isEqualPtr
)
{
    GT_U32  i;
    GT_U32  numOfWordsToCompare;
    GT_U32  numOfExtraBitsToCompare;

    numOfWordsToCompare = numOfBits/32;
    numOfExtraBitsToCompare = numOfBits%32;

    for( i = 0 ; i < numOfWordsToCompare ; i++ )
    {
        if( dataAPtr[i] != dataBPtr[i] )
        {
            *isEqualPtr = GT_FALSE;
            return GT_OK;
        }
    }

    if( (dataAPtr[i] & BIT_MASK_MAC(numOfExtraBitsToCompare)) !=
            (dataBPtr[i] & BIT_MASK_MAC(numOfExtraBitsToCompare)) )
    {
        *isEqualPtr = GT_FALSE;
        return GT_OK;
    }

    *isEqualPtr = GT_TRUE;
    return GT_OK;
}


/*******************************************************************************
* prvPrintReadAndWriteData
*
* DESCRIPTION:
*       The function prints the content of the supplied array according to the
*       relevant bit number.
*
* INPUTS:
*       numOfBits   - the number of bits to compare
*       dataPtr     - (pointer to) data to print
*
* OUTPUT:
*       None.
*
* RETURNS:
*       GT_OK
*
*******************************************************************************/
static GT_STATUS prvPrintReadAndWriteData
(
    GT_U32 numOfBits,
    GT_U32 *dataPtr
)
{
    GT_U32  i;
    GT_U32  numOfWordsToCompare;
    GT_U32  numOfExtraBitsToCompare;

    numOfWordsToCompare = numOfBits/32;
    numOfExtraBitsToCompare = numOfBits%32;

    for( i = 0 ; i < numOfWordsToCompare ; i++ )
    {
        cpssOsPrintf(" [0x%8.8x]", dataPtr[i]);
    }

    if( numOfExtraBitsToCompare > 0 )
    {
        cpssOsPrintf(" [0x%8.8x]", (dataPtr[i]  & BIT_MASK_MAC(numOfExtraBitsToCompare)) );
    }

    cpssOsPrintf("\n");

    return GT_OK;
}

/*******************************************************************************
* testCpssExMxPmExternalMemReadAndWriteBits
*
* DESCRIPTION:
*       test function that writes, reads and compare the result of various bit
*       length operations (1 to 256 bit write and read operations).
*
* INPUTS:
*       devNum          - the device number
*       startIndex      - start index in the control memory
*       startNumBits    - the number of bits to write (start number)
*       stopNumberBits         - the number of bits to write (end number -- including)
* OUTPUT:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In Puma the data is updated only when the last word in the entry was written
*
*******************************************************************************/
GT_STATUS testCpssExMxPmExternalMemReadAndWriteBits
(
    IN GT_U8                devNum,
    IN GT_U32               startIndex,
    IN GT_U32               startNumBits,
    IN GT_U32               stopNumberBits
)
{
    GT_STATUS rc;
    GT_U32    ii,jj;
    GT_BOOL   isEqual;
    GT_U32    memData[TEST_NUM_WORDS];

    GT_U32    data55[TEST_NUM_WORDS] = {0x55555555,0x55555555,0x55555555,0x55555555,
                                        0x55555555,0x55555555,0x55555555,0x55555555};
    GT_U32    dataAA[TEST_NUM_WORDS] = {0xAAAAAAAA,0xAAAAAAAA,0xAAAAAAAA,0xAAAAAAAA,
                                        0xAAAAAAAA,0xAAAAAAAA,0xAAAAAAAA,0xAAAAAAAA};
    GT_U32    dataFF[TEST_NUM_WORDS] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
                                        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
    GT_U32    dataRand[TEST_NUM_WORDS]={0x12345678,0x87654321,0xabcdef12,0x21fedeca,
                                        0x97643185,0x58134679,0x75362048,0x84026357};
    GT_U32    *dataArrayPtr[4] = {0,0,0,0};/* pointer to needed data array */
    GT_U32    parityEmbedded , ecc;

    ii = 0;
    dataArrayPtr[ii++] = data55;
    dataArrayPtr[ii++] = dataAA;
    dataArrayPtr[ii++] = dataFF;
    dataArrayPtr[ii++] = dataRand;

    if(startNumBits == 0 || startNumBits > stopNumberBits || stopNumberBits > 256)
        return GT_OUT_OF_RANGE;

    for ( ii = startNumBits ; ii <= stopNumberBits ; ii++ )
    {
        if((ii%10)==0)
        {
            osPrintf(" [%d] ",ii);
        }

        for(jj = 0 ; jj < 4; jj++) /* loop on 4 patterns */
        {
            for(parityEmbedded = 0 ; parityEmbedded < 2 ;parityEmbedded ++)
            {
                for(ecc = 0 ; ecc < 2 ; ecc ++)
                {
                    if(ecc)
                    {
                        if((ii >= (72-8) && ii <= 72) ||
                           (ii >= ((72*2)-9) && ii <= (72*2)) ||
                           (ii >= ((72*3)-9) && ii <= (72*3)) )
                        {
                            /* the ECC entry need to fit into line and not cause 'slip'
                               into next line

                                so since the ecc needs 8 bits for entry up to 64 bits
                                and need 9 bits from up to 256 bits
                            */

                            break;
                        }
                    }

                    rc = testCpssExMxPmExternalMemWrite(devNum, startIndex, ii,
                                                        parityEmbedded, ecc,
                                                        dataArrayPtr[jj][0],
                                                        dataArrayPtr[jj][1],
                                                        dataArrayPtr[jj][2],
                                                        dataArrayPtr[jj][3],
                                                        dataArrayPtr[jj][4],
                                                        dataArrayPtr[jj][5],
                                                        dataArrayPtr[jj][6],
                                                        dataArrayPtr[jj][7]);
                    if( rc != GT_OK )
                    {
                        osPrintf(" testCpssExMxPmExternalMemReadAndWriteBits FAILED [%ld] in numBits[%ld] in line[%ld]\n",rc,ii,__LINE__);
                        return rc;
                    }
#ifdef ASIC_SIMULATION
                    if(doSleep)
                    {
                        osTimerWkAfter(doSleep);/* GM issue */
                    }
#endif /*ASIC_SIMULATION*/

                    rc = prvCpssExMxPmReadTableEntry(devNum,&dummyTable,startIndex,memData);
                    if( rc != GT_OK )
                    {
                        osPrintf(" testCpssExMxPmExternalMemReadAndWriteBits FAILED [%ld] in numBits[%ld] in line[%ld]\n",rc,ii,__LINE__);
                        return rc;
                    }

                    rc = prvCompArraysByBit(ii, memData, dataArrayPtr[jj], &isEqual);
                    if( rc != GT_OK )
                    {
                        osPrintf(" testCpssExMxPmExternalMemReadAndWriteBits FAILED [%ld] in numBits[%ld] in line[%ld]\n",rc,ii,__LINE__);
                        return rc;
                    }

                    if( isEqual != GT_TRUE)
                    {
                        osPrintf("testCpssExMxPmExternalMemReadAndWriteBits:\n");
                        osPrintf("Different values in %d bits operation in pattern[%d], ecc[%d] parityEmbedded[%d]\n",ii,jj,jj,ecc,parityEmbedded);
                        osPrintf("Data read from memory:");
                        prvPrintReadAndWriteData(ii, memData);
                        osPrintf("Data written to memory:");
                        prvPrintReadAndWriteData(ii, dataArrayPtr[jj]);

                        /* failed already no need to fail on next pattern as well */
                        break;/* ecc */
                    }
                }/* end loop ecc */
                if(ecc != 2)
                {
                    /* failed already no need to fail on next pattern as well */
                    break;
                }
            }/*end loop parityEmbedded */

            if(parityEmbedded != 2)
            {
                /* failed already no need to fail on next pattern as well */
                break;
            }
        }/* end loop jj */

    }/* end loop ii */

    osPrintf(" testCpssExMxPmExternalMemReadAndWriteBits : finished \n");


    return GT_OK;
}


typedef GT_STATUS    (*GET_CSU_TABLE_UT)
(
    IN  GT_U8                            devNum,
    IN CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLES_ENT   tableType,
    OUT PRV_CPSS_EXMXPM_TABLE_INFO_STC  **tableInfoPtrPtr
);



/*******************************************************************************
* prvCpssExMxPmEccEmulationAutoCorrectionTestUT
*
* DESCRIPTION:
*       Function read table entry from the HW , modify single bit , and force the
*       HW not to re-calculate the ECCs.
*       then the function
* INPUTS:
*       devNum          - the device number
*       getCsuTableFunc - pointer to function that get the table info
*       maxNumOfTables  - max number of tables
* OUTPUTS:
*
* RETURNS:
*       GT_OK - entry is ok or was fixed
*       GT_BAD_PARAM - bad device or bad table type or entry index out of range
*       GT_BAD_PTR - on NULL pointer
*       GT_HW_ERROR - on HW error (read/write)
*       GT_NOT_SUPPORTED - the table not bound to controller ,
*                          or the CSU not set to ECC mode(see comment 2)
*       GT_UNFIXABLE_ECC_ERROR   - the CPSS detected ECC error that can't
*                                  be fixed when read from the memory that
*                                  protected by ECC generated.
*                                  if entry can't be fixed due to 2 data errors
*                                  NOTE: relevant only to memory controller that
*                                    work with ECC protection , and the CPSS need
*                                    to emulate ECC protection.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS prvCpssExMxPmEccEmulationAutoCorrectionTestUT
(
    IN  GT_U8                            devNum,
    IN  GET_CSU_TABLE_UT                 getCsuTableFunc,
    IN  GT_U32                           maxNumOfTables
)
{
    GT_STATUS   rc;
    GT_U32      ii;

    GT_U32    data55[TEST_NUM_WORDS] = {0x55555555,0x55555555,0x55555555,0x55555555,
                                        0x55555555,0x55555555,0x55555555,0x55555555};
    GT_U32    dataAA[TEST_NUM_WORDS] = {0xAAAAAAAA,0xAAAAAAAA,0xAAAAAAAA,0xAAAAAAAA,
                                        0xAAAAAAAA,0xAAAAAAAA,0xAAAAAAAA,0xAAAAAAAA};
    GT_U32    dataFF[TEST_NUM_WORDS] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
                                        0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
    GT_U32    dataRand[TEST_NUM_WORDS]={0x12345678,0x87654321,0xabcdef12,0x21fedeca,
                                        0x97643185,0x58134679,0x75362048,0x84026357};
    GT_U32    *dataArrayPtr[4] = {0,0,0,0};/* pointer to needed data array */
    GT_U32    tableType; /*CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLES_ENT*/
    PRV_CPSS_EXMXPM_TABLE_INFO_STC  *tableInfoPtr;
    GT_BOOL     entryWasFixed;
    GT_U32      numOfRetry;
    GT_U32      numErrors;
    GT_U32      errorCounter = 0;/* number of detected errors in the test */

    ii = 0;
    dataArrayPtr[ii++] = data55;
    dataArrayPtr[ii++] = dataAA;
    dataArrayPtr[ii++] = dataFF;
    dataArrayPtr[ii++] = dataRand;

    for(numErrors = 2 ; numErrors > 0; numErrors--)
    {
        for(tableType = 0 ; tableType < maxNumOfTables ; tableType++)
        {
            errorCounter = 0;

            rc = getCsuTableFunc(devNum,tableType,&tableInfoPtr);
            if(rc != GT_OK || tableInfoPtr->memCtrlPtr == NULL)
            {
                if(numErrors == 2)
                {
                    osPrintf(" tableType[%s] not supported \n",STR_NAME(tableType));
                }

                /* table not supported */
                continue;
            }
            else if(tableInfoPtr->memCtrlPtr->memCheckType != PRV_CPSS_EXMXPM_MEMORY_CHECK_ECC_E)
            {
                if(numErrors == 2)
                {
                    osPrintf(" controller of tableType[%s] not use ECC \n",STR_NAME(tableType));
                }

                /* the controller NOT use ECC */
                continue;
            }

            numOfRetry = 1000;

            eccAutoCorrectTable = *tableInfoPtr;

            if(eccAutoCorrectTable.baseAddr == 0)
            {
                eccAutoCorrectTable.baseAddr =
                     PRV_CPSS_EXMXPM_PP_MAC(devNum)->tblsAddr.CSU0.externalCtrlMemory.baseAddr;
            }

            eccAutoCorrectTable.tableType = tableType;
            while(numOfRetry--)
            {
                for(ii = 0 ; ii < 4; ii++)
                {
                    rc = prvCpssExMxPmTableEccEmulationAutoCorrectionTest_debug(devNum,
                            &eccAutoCorrectTable,
                            dataArrayPtr[ii],
                            &entryWasFixed,
                            numErrors);

                    if(numErrors == 1)
                    {
                        if(rc != GT_OK)
                        {
                            osPrintf(" testCpssExMxPmTableEccEmulationAutoCorrectionTest_debug FAILED [%ld] in ii[%ld] , numOfRetry[%ld] ,tableType[%ld],numErrors[%ld], in line[%ld]\n",rc,ii,numOfRetry,tableType,numErrors,__LINE__);
                            errorCounter++;
                        }
                        else if(entryWasFixed != GT_TRUE)
                        {
                            osPrintf(" testCpssExMxPmTableEccEmulationAutoCorrectionTest_debug not fixed in ii[%ld] , numOfRetry[%ld] ,tableType[%ld],numErrors[%ld], in line[%ld]\n",ii,numOfRetry,tableType,numErrors,__LINE__);
                            errorCounter++;
                        }
                    }
                    else /*numErrors == 2*/
                    {
                        if(rc != GT_UNFIXABLE_ECC_ERROR)
                        {
                            osPrintf(" testCpssExMxPmTableEccEmulationAutoCorrectionTest_debug should get GT_UNFIXABLE_ECC_ERROR , but get [%ld] in ii[%ld] , numOfRetry[%ld] ,tableType[%ld],numErrors[%ld], in line[%ld]\n",rc,ii,numOfRetry,tableType,numErrors,__LINE__);
                            errorCounter++;
                        }
                    }

                    if(errorCounter >= 4)
                    {
                        /* no need to get more errors */
                        goto jump_to_next_tables;
                    }

                }/*for ii*/

            }/*while numOfRetry */

            jump_to_next_tables:
            ;/* ; needed for compilation after label */
       }/*for tableType*/
    }/* for numErrors */

   return GT_OK;
}/*function*/

/*******************************************************************************
* cpssExMxPmEccEmulationAutoCorrectionTestUT
*
* DESCRIPTION:
*       Function read table entry from the HW , modify single bit , and force the
*       HW not to re-calculate the ECCs.
*       then the function
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK - entry is ok or was fixed
*       GT_BAD_PARAM - bad device or bad table type or entry index out of range
*       GT_BAD_PTR - on NULL pointer
*       GT_HW_ERROR - on HW error (read/write)
*       GT_NOT_SUPPORTED - the table not bound to controller ,
*                          or the CSU not set to ECC mode(see comment 2)
*       GT_UNFIXABLE_ECC_ERROR   - the CPSS detected ECC error that can't
*                                  be fixed when read from the memory that
*                                  protected by ECC generated.
*                                  if entry can't be fixed due to 2 data errors
*                                  NOTE: relevant only to memory controller that
*                                    work with ECC protection , and the CPSS need
*                                    to emulate ECC protection.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssExMxPmEccEmulationAutoCorrectionTestUT
(
    IN  GT_U8                            devNum
)
{
    return prvCpssExMxPmEccEmulationAutoCorrectionTestUT(devNum,
            prvCpssExMxPmTableEccEmulationAutoCorrectionCsuTableGet,
            CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_LAST_E);
}

/*************************************************************************
* cpssExMxPmExternalMemoryCsuEccEmulationCheckUT
*
* DESCRIPTION:
*       function check some APIs of ECC emulation.
*       loop on all CSU tables and try to check ECC emulation APIs
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - the device number
*
* OUTPUT:
*
*
* RETURNS:
*       GT_OK - entry is ok or was fixed
*
* COMMENTS:
*       1. part of the ECC emulation in the CPSS.
*       2. All the CSU ECC emulations relevant only when the 'phase 1' init
*       parameter indicates that the CSU  used with ECC.
*       Meaning : CPSS_EXMXPM_PP_PHASE1_INIT_INFO_STC::controlSramProtect == CPSS_EXMXPM_CSU_PROTECT_ECC_E
*
*******************************************************************************/
GT_STATUS cpssExMxPmExternalMemoryCsuEccEmulationCheckUT
(
    IN  GT_U8                            devNum
)
{
    GT_STATUS   rc;
    CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLES_ENT    tableType;
    PRV_CPSS_EXMXPM_TABLE_INFO_STC  *tableInfoPtr;
    GT_U32    entryIndex;
    GT_BOOL   entryWasFixed,wasError;
    GT_U32    singleErrorCounter,doubleErrorCounter;
    GT_U32    lastErrorIndex;

    for(tableType = 0 ; tableType < CPSS_EXMXPM_EXTERNAL_MEMORY_CSU_TABLE_LAST_E ; tableType++)
    {
        rc = prvCpssExMxPmTableEccEmulationAutoCorrectionCsuTableGet(devNum,tableType,&tableInfoPtr);
        if(rc != GT_OK || tableInfoPtr->memCtrlPtr == NULL)
        {
            osPrintf(" tableType[%s] not supported \n",STR_NAME(tableType));

            /* table not supported */
            continue;
        }
        else if(tableInfoPtr->memCtrlPtr->memCheckType != PRV_CPSS_EXMXPM_MEMORY_CHECK_ECC_E)
        {
            osPrintf(" controller of tableType[%s] not use ECC \n",STR_NAME(tableType));

            /* the controller NOT use ECC */
            continue;
        }

        tableInfoPtr->tableType = tableType;

        for(entryIndex = 0 ; entryIndex < tableInfoPtr->numOfEntries; entryIndex++)
        {
            rc = cpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionTableEntry(devNum,tableType,entryIndex,&entryWasFixed);
            if(rc != GT_OK)
            {
                if(rc == GT_UNFIXABLE_ECC_ERROR)
                {
                    osPrintf(" (ok) cpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionTableEntry has unfixable ecc error in entryIndex[%ld] , tableType[%ld], in line[%ld]\n",entryIndex,tableType,__LINE__);
                    continue;
                }
                else
                {
                    osPrintf(" cpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionTableEntry FAILED in entryIndex[%ld] , tableType[%ld], in line[%ld]\n",entryIndex,tableType,__LINE__);
                }
                break;
            }

            if(entryWasFixed ==GT_TRUE)
            {
                osPrintf(" cpssExMxPmExternalMemoryCsuEccEmulationAutoCorrectionTableEntry fixed line in entryIndex[%ld] , tableType[%ld], in line[%ld]\n",entryIndex,tableType,__LINE__);
            }
        }

        rc = cpssExMxPmExternalMemoryCsuEccEmulationErrorCntrGet(devNum,&singleErrorCounter,&doubleErrorCounter);
        if(rc != GT_OK)
        {
            osPrintf(" cpssExMxPmExternalMemoryCsuEccEmulationErrorCntrGet FAILED in tableType[%ld], in line[%ld]\n",tableType,__LINE__);
        }
        else if((singleErrorCounter != 0) ||(doubleErrorCounter != 0))
        {
            osPrintf(" singleErrorCounter[%ld] ,doubleErrorCounter[%ld] in tableType[%ld], in line[%ld]\n",singleErrorCounter,doubleErrorCounter,tableType,__LINE__);
        }

    }

    rc = cpssExMxPmExternalMemoryCsuEccEmulationLastErrorIndexGet(devNum,&wasError,&tableType,&lastErrorIndex);
    if(rc != GT_OK)
    {
        osPrintf(" cpssExMxPmExternalMemoryCsuEccEmulationLastErrorIndexGet FAILED in tableType[%ld], in line[%ld]\n",tableType,__LINE__);
    }
    else if(wasError == GT_TRUE)
    {
        osPrintf(" tableType[%s] had last error , in index[%ld] ,in line[%ld] \n",STR_NAME(tableType),lastErrorIndex,__LINE__);
    }


    return GT_OK;
}

