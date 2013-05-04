/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChDiag.c
*
* DESCRIPTION:
*       CPSS DXCH Diagnostic API
*
* PRBS sequence for tri-speed ports:
*  1. Enable PRBS checker on receiver port (cpssDxChDiagPrbsPortCheckEnableSet)
*  2. Check that checker initialization is done (cpssDxChDiagPrbsPortCheckReadyGet)
*  3. Set CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS_E transmit mode on transmiting port
*     (cpssDxChDiagPrbsPortTransmitModeSet)
*  4. Enable PRBS generator on transmiting port (cpssDxChDiagPrbsPortGenerateEnableSet)
*  5. Check results on receiving port (cpssDxChDiagPrbsPortStatusGet)
*
* PRBS sequence for XG ports:
*  1. Set CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E or CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS23_E
*      transmit mode on both ports(cpssDxChDiagPrbsPortTransmitModeSet)
*  2. Enable PRBS checker on receiver port (cpssDxChDiagPrbsPortCheckEnableSet)
*  3. Enable PRBS generator on transmiting port (cpssDxChDiagPrbsPortGenerateEnableSet)
*  4. Check results on receiving port (cpssDxChDiagPrbsPortStatusGet)
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiag.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/generic/diag/private/prvCpssGenDiag.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPort.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>


#ifdef CPSS_DEBUG
#define CPSS_DEBUG_DIAG
#endif

#ifdef CPSS_DEBUG_DIAG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

#define BUFFER_DRAM_PAGE_SIZE_CNS 64        /* buffer DRAM page size in bytes */
/* buffer DRAM page size in 32 bit words */
#define BUFFER_DRAM_PAGE_SIZE_IN_WORDS_CNS (BUFFER_DRAM_PAGE_SIZE_CNS / 4)
#define LION_BUFFER_DRAM_PAGE_SIZE_CNS 128  /* buffer DRAM page (line) size in bytes */
#define MAC_ENTRY_SIZE_CNS        16        /* MAC table entry size in bytes  */
#define CH_VLAN_ENTRY_SIZE_CNS    12        /* VLAN table entry size for CH in bytes */
#define CH2_VLAN_ENTRY_SIZE_CNS   16        /* VLAN table entry size for CH2,3.. in bytes */

/* VLAN table entry size for XCAT in bytes, the real "used" size is 179 bits,
   6 words */
#define XCAT_VLAN_ENTRY_SIZE_CNS  32

/* Egress VLAN Table entry size for Lion B0 ni bytes, the real "used" size is
   267 bits, 9 words */
#define LION_B0_EGR_VLAN_ENTRY_SIZE_CNS    64

/* The macro checks function return status and returns it if it's not GT_OK */
#define PRV_CH_DIAG_CHECK_RC_MAC(status)    \
    GT_STATUS retStatus = (status);         \
    if(retStatus != GT_OK)                  \
        return retStatus

#define PRV_CH_DIAG_MINIMAL_SLEEP_TIME 1 /* minimal sleep time in miliseconds */

/*******************************************************************************
* getMemBaseFromType
*
* DESCRIPTION:
*       This routine translates memType to PP base address.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       memType - The packet processor memory type to verify.
*
* OUTPUTS:
*       memBasePtr - memory base address.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - bad memory type
*       GT_FAIL       - on error
*
* COMMENTS:
*       Supported RAM types: only CPSS_DIAG_BUFFER_DRAM_E
*
*******************************************************************************/
static GT_STATUS getMemBaseFromType
(
    IN  GT_U8                       devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT  memType,
    OUT GT_U32                     *memBasePtr
)
{
    switch (memType)
    {
        case CPSS_DIAG_PP_MEM_BUFFER_DRAM_E:
            if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
                (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                *memBasePtr = 0x06800000;
            }
            else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                *memBasePtr = 0x10000000;
            }
            else
            {
                *memBasePtr = 0x06900000;
            }
            break;

        case CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E:
            *memBasePtr = 0x06400000;
            break;

        case CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                /* lion devices */
                /* Egress VLAN Table */
                *memBasePtr = 0x11800000;
            }
            else if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
                     (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                /* xCat or xCat2 devices */
                *memBasePtr = 0x0A200000;
            }
            else
            {
                *memBasePtr = 0x0A400000;
            }
            break;

        default:
            return GT_BAD_PARAM;
    }
    return GT_OK;
}

/*******************************************************************************
* getTableType
*
* DESCRIPTION:
*       Gets the table type from the memory address
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       addr      - Memory address
*
* OUTPUTS:
*       tableTypePtr - Table type
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getTableType
(
    IN GT_U8 devNum,
    IN  GT_U32                   addr,
    OUT PRV_CPSS_DXCH_TABLE_ENT  *tableTypePtr
)
{
    switch (addr & 0xFFF00000)
    {
        case 0x06400000:
            *tableTypePtr = PRV_CPSS_DXCH_TABLE_FDB_E;
            break;

        case 0x0A400000:
            if (! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                *tableTypePtr = PRV_CPSS_DXCH_TABLE_VLAN_E;
            }
            else
            {
                return GT_NOT_SUPPORTED;
            }
            break;

        case 0x0A200000:
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
               (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                /* xCat and xCat2 */
                *tableTypePtr = PRV_CPSS_DXCH_TABLE_VLAN_E;
            }
            else
            {
                return GT_NOT_SUPPORTED;
            }
            break;

        case 0x11800000:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                /* Lion */

                /* Only Egress VLAN table is supported */
                *tableTypePtr = PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E;
            }
            else
            {
                return GT_NOT_SUPPORTED;
            }
            break;

        default:
            return GT_NOT_SUPPORTED;
    }
    return GT_OK;
}

/*******************************************************************************
* getEntrySize
*
* DESCRIPTION:
*       Gets table entry size for a table type
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       tableType    - table type
*
* OUTPUTS:
*       entrySizePtr    - entry size in bytes
*
* RETURNS:
*       GT_OK              - on success
*       GT_NOT_SUPPORTED   - table type is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getEntrySize
(
    IN  GT_U8                   devNum,
    IN  PRV_CPSS_DXCH_TABLE_ENT tableType,
    OUT GT_U32                  *entrySizePtr
)
{
    if (tableType == PRV_CPSS_DXCH_TABLE_VLAN_E)
    {
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        {
            *entrySizePtr = CH_VLAN_ENTRY_SIZE_CNS;
        }
        else if (PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
            /* Lion */
            return GT_NOT_SUPPORTED;
        }
        else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat, xCat2 */
            *entrySizePtr = XCAT_VLAN_ENTRY_SIZE_CNS;
        }
        else
        {
            /* DxCh 2,3 and xCat A0 */
            *entrySizePtr = CH2_VLAN_ENTRY_SIZE_CNS;
        }
    }
    else if (tableType == PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E)
    {
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* Lion */
            *entrySizePtr = LION_B0_EGR_VLAN_ENTRY_SIZE_CNS;
        }
        else
        {
            return GT_NOT_SUPPORTED;
        }
    }
    else if (tableType == PRV_CPSS_DXCH_TABLE_FDB_E)
    {
        *entrySizePtr = MAC_ENTRY_SIZE_CNS;
    }
    else
    {
        return GT_NOT_SUPPORTED;
    }
    return GT_OK;
}

/*******************************************************************************
* getEntryIndexAndWordNum
*
* DESCRIPTION:
*       Gets the entry index and the word number in order to access table field
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       tableType    - Table type
*       addr         - memory address
*
* OUTPUTS:
*       entryIndexPtr   - index of the entry in the table
*       wordNumPtr      - number of entry for the address
*
* RETURNS:
*       GT_OK              - on success
*       GT_NOT_SUPPORTED   - table type is not supported
*       GT_FAIL            - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getEntryIndexAndWordNum
(
    IN  GT_U8                   devNum,
    IN  PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN  GT_U32                  addr,
    OUT GT_U32                  *entryIndexPtr,
    OUT GT_U32                  *wordNumPtr
)
{
    GT_STATUS status;       /* returned status */
    GT_U32 entrySize;       /* entry size */
    GT_U32 memBase;         /* base address for the memory */

    /* Get memory base address */
    if ((tableType == PRV_CPSS_DXCH_TABLE_VLAN_E) ||
        (tableType == PRV_CPSS_DXCH_LION_TABLE_VLAN_EGRESS_E))
    {
        status = getMemBaseFromType (devNum, CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E, &memBase);
    }
    else if (tableType == PRV_CPSS_DXCH_TABLE_FDB_E)
    {
        status = getMemBaseFromType (devNum, CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E, &memBase);
    }
    else
    {
        return GT_NOT_SUPPORTED;
    }

    if (status != GT_OK)
    {
        return status;
    }

    /* Get table entry size */
    status = getEntrySize (devNum, tableType, &entrySize);
    if (status != GT_OK)
    {
        return status;
    }

    /* Calculate table entry index */
    *entryIndexPtr = (addr - memBase) / entrySize;

    /* Calculate word number within table entry */
    *wordNumPtr = ((addr - memBase) % (entrySize)) / 4;

    return status;
}

/*******************************************************************************
* getEntryIndexAndNumOfEntries
*
* DESCRIPTION:
*       Gets the number of entries to access and first entry index in order to
*       access table field
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addr         - Address offset to read/write
*       devNum       - device number
*       tableType    - Table type
*       entrySize    - entry size in bytes
*       length       - Number of words (4 bytes) to read/write
*
* OUTPUTS:
*       firstEntryIndexPtr - First entry index
*       numOfEntriesPtr    - number of entries to read/write
*
* RETURNS:
*       GT_OK              - on success
*       GT_NOT_SUPPORTED   - table type is not supported
*       GT_FAIL            - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getEntryIndexAndNumOfEntries
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  addr,
    IN  PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN  GT_U32                  entrySize,
    IN  GT_U32                  length,
    OUT GT_U32                  *firstEntryIndexPtr,
    OUT GT_U32                  *numOfEntriesPtr
)
{
    GT_STATUS status;       /* returned status */
    GT_U32 memBase;         /* base address for the memory */

    entrySize /= 4;     /* Convert table entry from bytes to words */
    *numOfEntriesPtr = length / entrySize;

    /* Get memory base address */
    if (tableType == PRV_CPSS_DXCH_TABLE_VLAN_E)
    {
        /* devNum is not relevant for VLAN Table */
        status = getMemBaseFromType (devNum, CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E, &memBase);
    }
    else if (tableType == PRV_CPSS_DXCH_TABLE_FDB_E)
    {
        /* devNum is not relevant for FDB Table */
        status = getMemBaseFromType (devNum, CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E, &memBase);
    }
    else
    {
        return GT_NOT_SUPPORTED;
    }

    if (status != GT_OK)
    {
        return status;
    }

    /* Calculate table entry index */
    *firstEntryIndexPtr = (addr - memBase) / (entrySize * 4);

    return GT_OK;
}

/*******************************************************************************
* getMacTableSize
*
* DESCRIPTION:
*       Gets the size of the MAC table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - The device number
*
* OUTPUTS:
*      None
*
* RETURNS:
*       MAC table size in bytes
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
static GT_U32 getMacTableSize
(
    IN  GT_U8   devNum
)
{
    GT_U32                                      size;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC*    fineTuningPtr;

    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    size = (fineTuningPtr->tableSize.fdb * MAC_ENTRY_SIZE_CNS);

    return size;
}

/*******************************************************************************
* getVlanTableSize
*
* DESCRIPTION:
*       Gets the size of the VLAN table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - The device number
*
* OUTPUTS:
*      None
*
* RETURNS:
*       VLAN table size in bytes
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_U32 getVlanTableSize
(
    IN  GT_U8   devNum
)
{
    GT_U32 size;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        size = CH_VLAN_ENTRY_SIZE_CNS * _4KB;
    }
    else if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* Lion */
        size = LION_B0_EGR_VLAN_ENTRY_SIZE_CNS * _4KB;
    }
    else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and xCat2 */
        size = XCAT_VLAN_ENTRY_SIZE_CNS * _4KB;
    }
    else
    {
        /* Cheetah 2,3 */
        size = CH2_VLAN_ENTRY_SIZE_CNS * _4KB;
    }

    return size;
}

/*******************************************************************************
* getBanksParams
*
* DESCRIPTION:
*       Gets the banks start addresses and the banks sizes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - The device number to test
*
* OUTPUTS:
*      bank0StartPtr     - base address of bank 0
*      bank0SizePtr      - size in MB of bank 0
*      bank1StartPtr     - base address of bank 1
*      bank1SizePtr      - size in MB of bank 1
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_VOID getBanksParams
(
    IN  GT_U8   devNum,
    OUT GT_U32 *bank0StartPtr,
    OUT GT_U32 *bank0SizePtr,
    OUT GT_U32 *bank1StartPtr,
    OUT GT_U32 *bank1SizePtr
)
{
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    *bank0StartPtr = 0x06900000; /* Buffers memory bank 0 word 0 */

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
    {
        switch (PRV_CPSS_PP_MAC(devNum)->devType)
        {
            case CPSS_98DX106_CNS:
            case CPSS_98DX107_CNS:
                *bank0SizePtr = _512KB;     /* Note: This is bits, not bytes */
                *bank1SizePtr = _512KB;     /* Note: This is bits, not bytes */
                break;

            case CPSS_98DX163_CNS:
            case CPSS_98DX243_CNS:
                *bank0SizePtr = _3MB / 2;   /* Note: This is bits, not bytes */
                *bank1SizePtr = _3MB / 2;   /* Note: This is bits, not bytes */
                break;

            default:
                *bank0SizePtr  = _3MB;     /* Note: This is bits, not bytes */
                *bank1SizePtr  = _3MB;     /* Note: This is bits, not bytes */
        }
        *bank1StartPtr = 0x06980000; /* Buffers memory bank 1 word 0 */

    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
    {/* CPSS_PP_FAMILY_CHEETAH2_E */
        *bank0SizePtr  = _4MB;             /* Note: This is bits, not bytes */
        *bank1SizePtr  = _4MB;             /* Note: This is bits, not bytes */
        *bank1StartPtr = 0x06980000;       /* Buffers memory bank 1 word 0 */
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {/* CPSS_PP_FAMILY_CHEETAH3_E */
        fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

        /* Note: This is bits, not bytes */
        *bank0SizePtr  = (fineTuningPtr->tableSize.bufferMemory) / 2;
        *bank1SizePtr  = (fineTuningPtr->tableSize.bufferMemory) / 2;
        *bank1StartPtr = 0x07100000;        /* Buffers memory bank 1 word 0 */

    }
    else if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

        *bank0StartPtr = 0x06800000;        /* Buffers memory bank 1 word 0 */
        *bank1StartPtr = 0x07000000;        /* Buffers memory bank 1 word 0 */
        /* Note: This is bits, not bytes */
        *bank0SizePtr = (fineTuningPtr->tableSize.bufferMemory) / 2;
        *bank1SizePtr = (fineTuningPtr->tableSize.bufferMemory) / 2;
    }
    else /*if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)*/
    {
        /* lion */

        fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

        /* bit 20 used as bank select */
        *bank0StartPtr = 0x10000000;
        *bank0SizePtr = fineTuningPtr->tableSize.bufferMemory;
        /* Not relevant for Lion */
        *bank1StartPtr = 0x00000000;
        *bank1SizePtr = 0;
    }

    *bank0SizePtr /= 8;                    /* convert to bytes */
    *bank1SizePtr /= 8;                    /* convert to bytes */
}

/*******************************************************************************
* hwBuffMemBankGet
*
* DESCRIPTION:
*       Gets the bank and page for a given addr in the memory BUFFER RAM.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number to test
*       addr        - The address to check.
*       length      - The length from the addr in WORDS(32 bits).
*
* OUTPUTS:
*       bankPtr     - (pointer to) the HW bank for the given address
*       pagePtr     - (pointer to)the HW page for the given address
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - the address is not in the Cheetah buffer RAM memory space.
*
* COMMENTS:
*      Cheetah1 buffer RAM is divided into two spaces (banks).
*      each bank is 3Mbit == 384K byte == 96K words.
*      bank location:
*        bank0: 0x06900000...0x0695FFFC
*        bank1: 0x06980000...0x069DFFFC
*      For other devices see datasheet to get RAM size and addresses
*******************************************************************************/
static GT_STATUS hwBuffMemBankGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  addr,
    IN  GT_U32  length,
    OUT GT_U32 *bankPtr,
    OUT GT_U32 *pagePtr
)
{
    GT_U32     bank0Start;
    GT_U32     bank0Size;
    GT_U32     bank1Start;
    GT_U32     bank1Size;
    GT_U32     pageSize;
    GT_U32     regAddr;

    getBanksParams (devNum, &bank0Start, &bank0Size, &bank1Start, &bank1Size);
    pageSize  = BUFFER_DRAM_PAGE_SIZE_CNS; /* each page is 64 bytes (16 words)*/

    /* bank identify */
    if(getMemBaseFromType(devNum, CPSS_DIAG_PP_MEM_BUFFER_DRAM_E, &regAddr))
        return GT_FAIL;
    if(regAddr != bank0Start)
        return GT_FAIL;

    /* each bank is 3MB */
    if(addr >= bank0Start && (addr < (bank0Start+bank0Size)))
    {
        *bankPtr = 0;
        *pagePtr = (addr - bank0Start) / pageSize;
        /* check if all the length is in bank0 */
        if((addr + length) > (bank0Start + bank0Size))
            return GT_FAIL;
    }
    else if(addr >= bank1Start && (addr < (bank1Start+bank1Size)))
    {
        *bankPtr = 1;
        *pagePtr = (addr - bank1Start) / pageSize;
        if((addr + length) > (bank1Start + bank1Size))
            return GT_FAIL;
    }
    else
        return GT_FAIL;

    return GT_OK;
}

/*******************************************************************************
* hwPpBufferRamInWordsRead
*
* DESCRIPTION:
*       Read the memory WORD by WORD.(32 bits).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number.
*       portGroupId - the port group Id , to support multi-port-group devices
*                     that need to access specific port group.
*       addr        - addr to start writing from.
*       length      - length in WORDS (32 bit) must be in portion of 8.
*
* OUTPUTS:
*       dataPtr  - (pointer to) an array containing the data to be read.
*
* RETURNS:
*       None
*
* COMMENTS:
*       If the memory is 32 bits in each WORD (like Cheetah BUFFER_DRAM) each
*       address points to a 32 bits WORD and the memory segmentation is 1
*       instead of 4 in normal operation.
*       Using prvCpssDrvHwPpReadRam with length of more then 1 WORD would read
*       offset of 0,4,8... while WORD of 32 bits needs offset of 0,1,2,3...
*
*******************************************************************************/
static GT_STATUS hwPpBufferRamInWordsRead
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    IN  GT_U32 addr,
    IN  GT_U32 length,
    OUT GT_U32 *dataPtr
)
{
    GT_U32    readLoop;
    GT_STATUS ret = GT_OK;

    /* Check if the device is Cheetah3,
       then Buffer Memory Bank address differs from Buffer Memory Bank Write
       Register address (Bank0 = 0x06800000, Bank1 = 0x07000000) */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        addr = addr - 0x100000;

    }

    for(readLoop=0; readLoop < length; ++readLoop)
    {
        /* read only one WORD at the time */
        ret = prvCpssDrvHwPpPortGroupReadRam(devNum, portGroupId, addr + 4 * readLoop, 1,
                                    &(dataPtr[readLoop]));
    }

    return ret;
}

/*******************************************************************************
* hwPpRamBuffMemPageWrite
*
* DESCRIPTION:
*       BUFFER_DRAM (buff mem) memory pages write.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - The PP device number to write to.
*       portGroupId - the port group Id , to support multi-port-group devices
*                     that need to access specific port group
*       addr      - Address offset to write to.
*       length    - length in WORDS (32 bit) must be in portion of 16.
*       dataPtr   - (pointer to) data to be written.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_TIMEOUT - after max time that the PP not finished action
*
* COMMENTS:
*       Writting to this memory is not direct.
*       buffMemBank0Write is expecting to be written 16 times before triggering
*       for writting the buffered data into the memory.
*       Triggering is combined with the bank and page for HW writting.
*       The bank and page is pointing to the abs address segment.
*
*******************************************************************************/
static GT_STATUS hwPpRamBuffMemPageWrite
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *dataPtr
)
{
    GT_U32    regAddr;          /* register address */
    GT_U32    writeLoop;
    GT_U32    bank;
    GT_U32    page;
    GT_U32    lengthPos = 0;    /* The current writting location in WORDS */
    GT_U32    absAdr;           /* the current abs addr. to write to      */
    GT_U32    hwData;
    GT_STATUS ret = GT_OK;

    /* buffer dram must be written in portion of 16 */
    if ((length % (BUFFER_DRAM_PAGE_SIZE_CNS / 4)) != 0)
        return GT_FAIL;
    if ((addr % (BUFFER_DRAM_PAGE_SIZE_CNS / 4)) != 0)
        return GT_FAIL;

    while(lengthPos < length)
    {
        absAdr = addr + lengthPos;

        /* find bank and page */
        ret = hwBuffMemBankGet (devNum, absAdr, BUFFER_DRAM_PAGE_SIZE_CNS / 4,
                                &bank, &page);
        if (ret != GT_OK)
        {
            return ret;
        }

        /* step 1: Make sure the previous write is done */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            addrSpace.buffMemWriteControl;

#ifndef ASIC_SIMULATION
        /* wait for bit 0 to clear */
        ret = prvCpssDxChPortGroupBusyWait(devNum,portGroupId,regAddr,0,GT_FALSE);
        if(ret != GT_OK)
        {
            return ret;
        }
#endif /*!ASIC_SIMULATION*/
        /* step 2: Write the 16 words of the page data */
        if(bank == 0)
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            addrSpace.buffMemBank0Write;
        else
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            addrSpace.buffMemBank1Write;

        for (writeLoop = 0; writeLoop < BUFFER_DRAM_PAGE_SIZE_CNS / 4 ; ++writeLoop)
        {
            ret = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr,
                                              dataPtr[lengthPos+writeLoop]);
            if (ret != GT_OK)
            {
                return ret;
            }

        }

        /* step 3: Trigger the Write transaction */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            addrSpace.buffMemWriteControl;

        /* set hw data to write to access register */
        hwData = 1; /* WrTrig = 1 */

        /* check the device type */
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
        {/* DxCh3 */

            hwData |= (page << 1); /* PageAddr (15 bits)*/
            hwData |= (bank << 16); /* WrBank */
        }
        else if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) ||
                 (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {/* xCat and xCat2 */

            hwData |= (page << 1); /* PageAddr (14 bits )*/
            hwData |= (bank << 15); /* WrBank */
        }
        else
        {/* Other DxCh devices */

            hwData |= (page << 1); /* PageAddr (13 bits )*/
            hwData |= (bank << 14); /* WrBank */
        }

        /* Write the data to Buffers Memory Write Access Register */
        ret = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr, hwData);
        if (ret != GT_OK)
        {
            return ret;
        }

        lengthPos += (BUFFER_DRAM_PAGE_SIZE_CNS / 4);
    }
    return ret;
}

/*******************************************************************************
* hwPpRamBuffMemWordWrite
*
* DESCRIPTION:
*       BUFFER_DRAM (buff mem) memory word write.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - the port group Id , to support multi-port-group devices
*                     that need to access specific port group
*       addr        - Address offset to write to.
*       data        - the data to be written.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_TIMEOUT - after max time that the PP not finished action
* COMMENTS:
*       Since writing to buffer memory is done in pages, the whole page is read
*       and stored to a temporary buffer. Then the data in the relevant word is
*       changed and the whole temporary buffer is written into the memory.
*
*******************************************************************************/
static GT_STATUS hwPpRamBuffMemWordWrite
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 addr,
    IN GT_U32 data
)
{
    GT_U32    tempData[BUFFER_DRAM_PAGE_SIZE_IN_WORDS_CNS]; /* temporary buffer */
    GT_U32    bank;                            /* bank number to write to */
    GT_U32    page;                            /* page number to write to */
    GT_U32    pageAddr;                        /* address of the page to read */
    GT_U32    bank0Start;                      /* start address of bank 0 */
    GT_U32    bank0Size;                       /* size of bank 0 - unused */
    GT_U32    bank1Start;                      /* start address of bank 1 */
    GT_U32    bank1Size;                       /* size of bank 1 - unused */
    GT_U32    wordNum;                         /* word number in a page */
    GT_STATUS status;                          /* returned status */

    /* Find bank and page */
    status = hwBuffMemBankGet (devNum, addr, BUFFER_DRAM_PAGE_SIZE_CNS / 4, &bank,
                               &page);
    if (status != GT_OK)
    {
        return status;
    }

    /* Find the start address of the page */
    getBanksParams (devNum, &bank0Start, &bank0Size, &bank1Start, &bank1Size);
    if (bank == 0)
    {
        pageAddr = bank0Start + (page * BUFFER_DRAM_PAGE_SIZE_CNS);
    }
    else /* bank 1 */
    {
        pageAddr = bank1Start + (page * BUFFER_DRAM_PAGE_SIZE_CNS);
    }


    /* Read the whole page */
    status = hwPpBufferRamInWordsRead (devNum, portGroupId, pageAddr,
                                       BUFFER_DRAM_PAGE_SIZE_IN_WORDS_CNS,
                                       tempData);
    if (status != GT_OK)
    {
        return status;
    }

    /* Change temporary buffer and write it to the buffer memory */
    wordNum = (addr - pageAddr) / 4;
    if (wordNum >= BUFFER_DRAM_PAGE_SIZE_IN_WORDS_CNS)
    {
        return GT_FAIL;
    }

    tempData[wordNum] = data;

    return hwPpRamBuffMemPageWrite (devNum, portGroupId, pageAddr,
                                    BUFFER_DRAM_PAGE_SIZE_IN_WORDS_CNS,
                                    tempData);
}

/*******************************************************************************
* hwPpRamBuffMemLionPageWrite
*
* DESCRIPTION:
*       BUFFER_DRAM (buff mem) Lion memory pages write.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum    - The PP device number to write to.
*       portGroupId - the port group Id , to support multi-port-group devices
*                     that need to access specific port group
*       addr      - Address offset to write to.
*       length    - length in WORDS (32 bit) must be in portion of 32.
*       dataPtr   - (pointer to) data to be written.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_TIMEOUT - after max time that the PP not finished action
*
* COMMENTS:
*       Access to this memory is direct.
*       It is enough to write the first word from the data in the line, as all
*       words in the line are written with the same word.
*
*******************************************************************************/
static GT_STATUS hwPpRamBuffMemLionPageWrite
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *dataPtr
)
{
    GT_U32    regAddr;          /* register address                             */
    GT_U32    hwData;           /* hw data to write to each word of the line    */
    GT_U32    lineStart;        /* The first line to write                      */
    GT_U32    linesNum;         /* The number of lines to write                 */
    GT_U32    line;             /* Loop iterator                                */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_STATUS ret = GT_OK;

    /* buffer dram must be written in portion of 32 */
    if (length % ((LION_BUFFER_DRAM_PAGE_SIZE_CNS / 4)) != 0)
        return GT_FAIL;
    if ((addr % (LION_BUFFER_DRAM_PAGE_SIZE_CNS / 4)) != 0)
        return GT_FAIL;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->addrSpace.buffMemBank0Write;
    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* calculate the first line to write */
    lineStart = ((addr - regAddr) / LION_BUFFER_DRAM_PAGE_SIZE_CNS);

    /* calculate the number of lines to write */
    linesNum = (length / (LION_BUFFER_DRAM_PAGE_SIZE_CNS / 4));

    /* check validity */
    if (((lineStart + linesNum) * LION_BUFFER_DRAM_PAGE_SIZE_CNS * 8) >
        fineTuningPtr->tableSize.bufferMemory)
    {
        return GT_BAD_PARAM;
    }

    /* line bits in the address are 8..19 and bank is bit 20 */
    regAddr += (lineStart << 8);

    /* it is enough to write only one word to each line,
       so the first word will be written to each line  */
    for (line = lineStart; line < (linesNum + lineStart); line++)
    {
        hwData = dataPtr[((line - lineStart) * (LION_BUFFER_DRAM_PAGE_SIZE_CNS / 4))];

        ret = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                   regAddr, hwData);
        if (ret != GT_OK)
        {
            return ret;
        }

        /* increment line in the address */
        regAddr += 0x100;
    }

    return GT_OK;
}

/*******************************************************************************
* hwPpBufferLionRamInWordsRead
*
* DESCRIPTION:
*       Read the memory WORD by WORD.(32 bits).
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum      - device number.
*       portGroupId - the port group Id , to support multi-port-group devices
*                     that need to access specific port group.
*       addr        - addr to start reading from.
*       length      - length in WORDS (32 bit) must be in portion of 32.
*
* OUTPUTS:
*       dataPtr  - (pointer to) an array containing the data to be read.
*
* RETURNS:
*       None
*
* COMMENTS:
*       Access to this memory is direct.
*       It is enough to read the first word from the data in the line, as all
*       words in the line are written with the same word.
*
*******************************************************************************/
static GT_STATUS hwPpBufferLionRamInWordsRead
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    IN  GT_U32 addr,
    IN  GT_U32 length,
    OUT GT_U32 *dataPtr
)
{
    GT_U32    regAddr;          /* register address                             */
    GT_U32    hwData;           /* hw data to write to each word of the line    */
    GT_U32    lineStart;        /* The first line to write                      */
    GT_U32    linesNum;         /* The number of lines to write                 */
    GT_U32    wordsNumInLine;   /* The number of words in each line             */
    GT_U32    wordInd;          /* The index of the word in dataPtr             */
    GT_U32    line;             /* Loop iterator                                */
    GT_U32    i;                /* Loop iterator                                */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_STATUS ret = GT_OK;

    /* buffer dram must be written in portion of 32 */
    if (length % ((LION_BUFFER_DRAM_PAGE_SIZE_CNS / 4)) != 0)
        return GT_FAIL;
    if ((addr % (LION_BUFFER_DRAM_PAGE_SIZE_CNS / 4)) != 0)
        return GT_FAIL;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->addrSpace.buffMemBank0Write;
    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* calculate the first line to write */
    lineStart = ((addr - regAddr) / LION_BUFFER_DRAM_PAGE_SIZE_CNS);

    /* calculate the number of lines to write */
    linesNum = (length / (LION_BUFFER_DRAM_PAGE_SIZE_CNS / 4));

    /* check validity */
    if (((lineStart + linesNum) * LION_BUFFER_DRAM_PAGE_SIZE_CNS * 8) >
        fineTuningPtr->tableSize.bufferMemory)
    {
        return GT_BAD_PARAM;
    }

    wordsNumInLine = (LION_BUFFER_DRAM_PAGE_SIZE_CNS / 4);

    /* line bits in the address are 8..19 and bank is bit 20 */
    regAddr += (lineStart << 8);

    wordInd = 0;

    /* Read all words from the line and compare them to the first one,
       all words should be equal */
    for (line = lineStart; line < (linesNum + lineStart); line++)
    {
        for (i = 0; i < wordsNumInLine; i++)
        {
            ret = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,
                                                       regAddr, &hwData);
            if (ret != GT_OK)
            {
                return ret;
            }

            dataPtr[wordInd] = hwData;

            /* check that current and previous words are equal */
            /* compare current word with the previous one */
            if (i > 0)
            {
                if (dataPtr[wordInd] != dataPtr[(wordInd - 1)])
                {
                    /* Change the first word in the line to the wrong one */
                    dataPtr[((line - lineStart) * wordsNumInLine)] = dataPtr[wordInd];
                }
            }

            wordInd++;

            /* increment word in the address, bits 2..6 */
            regAddr += 0x4;
        }

        /* increment line in the address */
        regAddr += 0x100;
    }

    return GT_OK;
}

/*******************************************************************************
* diagTestBuffBanksMem
*
* DESCRIPTION:
*       This routine performs a memory test on memory banks which do not have to
*       be contiguous.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The local device number to test
*       memType     - The packet processor memory type to verify.
*       startOffset - The offset address to start the test from.
*       size        - The memory size in bytes to test (start from offset).
*       profile     - The test profile.
*       burstSize   - The memory burst size.
*
* OUTPUTS:
*       testStatusPtr  - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*      This routine supports only buffer DRAM memory type.
*
*******************************************************************************/
static GT_STATUS diagTestBuffBanksMem
(
    IN GT_U8                        devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT    memType,
    IN GT_U32                       startOffset,
    IN GT_U32                       size,
    IN CPSS_DIAG_TEST_PROFILE_ENT   profile,
    IN GT_U32                       burstSize,
    OUT GT_BOOL                     *testStatusPtr,
    OUT GT_U32                      *addrPtr,
    OUT GT_U32                      *readValPtr,
    OUT GT_U32                      *writeValPtr
)
{
    GT_U32      savedBurstSize;
    GT_U32      bank0Start;
    GT_U32      bank0Size;
    GT_U32      bank1Start;
    GT_U32      bank1Size;
    GT_U32      regAddr;
    GT_U32      ageAutoEnable;
    PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC specialRamFuncs;
    GT_U32      portGroupsBmp;
    GT_STATUS   ret = GT_OK;
    GT_STATUS   status = GT_OK;



    cpssOsMemSet (&specialRamFuncs, 0,
                  sizeof(PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC));

    /* this routine supports only buffer DRAM memory test */
    if (memType != CPSS_DIAG_PP_MEM_BUFFER_DRAM_E)
    {
        return GT_FAIL;
    }

    /* in Lion the access is direct, in other devices - indirect */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
    {
        specialRamFuncs.prvCpssDiagHwPpRamBufMemWriteFuncPtr =
            &hwPpRamBuffMemPageWrite;
        specialRamFuncs.prvCpssDiagHwPpRamInWordsReadFuncPtr =
            &hwPpBufferRamInWordsRead;
    }
    else
    {
        specialRamFuncs.prvCpssDiagHwPpRamBufMemWriteFuncPtr =
            &hwPpRamBuffMemLionPageWrite;
        specialRamFuncs.prvCpssDiagHwPpRamInWordsReadFuncPtr =
            &hwPpBufferLionRamInWordsRead;
    }

    getBanksParams (devNum, &bank0Start, &bank0Size, &bank1Start, &bank1Size);

    /* validate the mem test size does not overflow the total banks size */
    if (size > (bank0Size + bank1Size))
    {
        return GT_BAD_PARAM;
    }

    /* adjust the banks start address and size according to test config */
    if (startOffset < bank0Size)
    {
        /* the start address is within the first bank */
        bank0Start += startOffset;
        bank0Size  -= startOffset;

        if (bank0Size >= size)
        {
            /* the test is within the first bank only */
            bank0Size = size;
            bank1Size = 0;
        }
        else
        {
            /* the test goes on afterwards to bank1, set the bank size */
            bank1Size = size - bank0Size;
        }
    }
    else
    {
        /* the start address is within the second bank */
        bank1Start += (startOffset - bank0Size);
        bank1Size   = size;
        bank0Size = 0;
    }

    /* save and set the new burst size */
    savedBurstSize = prvCpssDiagBurstSizeGet();
    ret = prvCpssDiagBurstSizeSet(burstSize);
    if (ret != GT_OK)
    {
        return ret;
    }

    /* Buffer Management Aging Configuration Register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)-> bufferMng.bufMngAgingConfig;
    ret = prvCpssDrvHwPpGetRegField (devNum, regAddr, 10, 1, &ageAutoEnable);
    if (ret != GT_OK)
    {
        PRV_CH_DIAG_CHECK_RC_MAC(prvCpssDiagBurstSizeSet(savedBurstSize));
        return ret;
    }


    /* test the first bank */
    if (bank0Size > 0)
    {
        /* Clear ageAutoEnable bit in Buffer Management Aging Configuration
           Register */
        if (ageAutoEnable == 1)
        {
            ret = prvCpssDrvHwPpSetRegField (devNum, regAddr, 10, 1, 0);
            if (ret != GT_OK)
            {
                PRV_CH_DIAG_CHECK_RC_MAC(prvCpssDiagBurstSizeSet(savedBurstSize));
                return ret;
            }
        }

        /* Get valid portGroup bitmap */
        if(GT_FALSE ==
            prvCpssDxChDuplicatedMultiPortGroupsGet(devNum,bank0Start,&portGroupsBmp))
        {
            portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }

        ret = prvCpssDiagAnyMemTest(devNum, portGroupsBmp, bank0Start, bank0Size, profile,
                                    specialRamFuncs, testStatusPtr, addrPtr,
                                    readValPtr, writeValPtr);

        /* Restore ageAutoEnable bit in Buffer Management Aging Configuration
           Register to its previous value */
        if (ageAutoEnable == 1)
        {
            status = prvCpssDrvHwPpSetRegField (devNum, regAddr, 10, 1, ageAutoEnable);
            if (status != GT_OK)
            {
                PRV_CH_DIAG_CHECK_RC_MAC(prvCpssDiagBurstSizeSet(savedBurstSize));
                return ((ret != GT_OK) ? ret : status);
            }
        }
        if (ret != GT_OK)
        {
            PRV_CH_DIAG_CHECK_RC_MAC(prvCpssDiagBurstSizeSet(savedBurstSize));
            return ret;
        }

        if (*testStatusPtr == GT_FALSE)
        {
            ret = prvCpssDiagBurstSizeSet(savedBurstSize);
            if (ret != GT_OK)
            {
                return ret;
            }
            return GT_OK;
        }
    }

    /* test the second bank */
    if (bank1Size > 0)
    {
        /* Clear ageAutoEnable bit in Buffer Management Aging Configuration
           Register */
        if (ageAutoEnable == 1)
        {
            ret = prvCpssDrvHwPpSetRegField (devNum, regAddr, 10, 1, 0);
            if (ret != GT_OK)
            {
                PRV_CH_DIAG_CHECK_RC_MAC(prvCpssDiagBurstSizeSet(savedBurstSize));
                return ret;
            }

        }

        /* Get valid portGroup bitmap */
        if(GT_FALSE ==
            prvCpssDxChDuplicatedMultiPortGroupsGet(devNum,bank1Start,&portGroupsBmp))
        {
            portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }

        ret = prvCpssDiagAnyMemTest(devNum, portGroupsBmp, bank1Start, bank1Size, profile,
                                    specialRamFuncs, testStatusPtr, addrPtr,
                                    readValPtr, writeValPtr);

        /* Restore ageAutoEnable bit in Buffer Management Aging Configuration
           Register to its previous value */
        if (ageAutoEnable == 1)
        {
            status = prvCpssDrvHwPpSetRegField (devNum, regAddr, 10, 1, ageAutoEnable);
            if (status != GT_OK)
            {
                PRV_CH_DIAG_CHECK_RC_MAC(prvCpssDiagBurstSizeSet(savedBurstSize));
                return ((ret != GT_OK) ? ret : status);
            }
        }
    }

    /* restore the original burst size */
    if (ret != GT_OK)
    {
        PRV_CH_DIAG_CHECK_RC_MAC(prvCpssDiagBurstSizeSet(savedBurstSize));
        return ret;
    }
    else
    {
        return prvCpssDiagBurstSizeSet(savedBurstSize);
    }
}

/*******************************************************************************
* hwPpTableEntryWrite
*
* DESCRIPTION:
*       Indirect write to PP's table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP device number to write to
*       portGroupId - the port group Id , to support multi-port-group devices
*                     that need to access specific port group
*       addr        - Address offset to write to
*       length      - Number of Words (4 bytes) to write
*       dataPtr     - An array containing the data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpTableEntryWrite
(
    IN GT_U8 devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *dataPtr
)
{
    PRV_CPSS_DXCH_TABLE_ENT tableType;       /* type of table */
    GT_U32                  firstEntryIndex; /* index of first entry to write */
    GT_U32                  numOfEntries;    /* number of entries to write */
    GT_U32                  entrySize;       /* entry size in bytes */
    GT_U32                  i;               /* loop index */
    GT_STATUS               status;          /* returned status */

    /* Find table type by memory address */
    status = getTableType (devNum,addr, &tableType);
    if (status != GT_OK)
    {
        return status;
    }

    /* Get entry size */
    status = getEntrySize (devNum, tableType, &entrySize);
    if (status != GT_OK)
    {
        return status;
    }

    /* Get the first entry to write to and the number of entries to write */
    status = getEntryIndexAndNumOfEntries (devNum, addr, tableType,
                                           entrySize, length,
                                           &firstEntryIndex, &numOfEntries);
    if (status != GT_OK)
    {
        return status;
    }

    /* write to all the entries */
    for (i = 0; i < numOfEntries; i++)
    {
        status = prvCpssDxChPortGroupWriteTableEntry (devNum, portGroupId, tableType,
                                             firstEntryIndex + i, dataPtr);
        if (status != GT_OK)
        {
            return status;
        }

        dataPtr += entrySize / 4;

    }
    return GT_OK;
}

/*******************************************************************************
* hwPpTableEntryRead
*
* DESCRIPTION:
*       Indirect read from PP's table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP device number to read from
*       portGroupId - the port group Id , to support multi-port-group devices
*                     that need to access specific port group
*       addr        - Address offset to read from
*       length      - Number of Words (4 bytes) to read
*
* OUTPUTS:
*       dataPtr - An array containing the read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpTableEntryRead
(
    IN GT_U8 devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 addr,
    IN GT_U32 length,
    OUT GT_U32 *dataPtr
)
{
    PRV_CPSS_DXCH_TABLE_ENT tableType;       /* type of table */
    GT_U32                  firstEntryIndex; /* index of first entry to read */
    GT_U32                  numOfEntries;    /* number of entries to read */
    GT_U32                  entrySize;       /* entry size in bytes */
    GT_U32                  i;               /* loop index */
    GT_STATUS               status;          /* returned status */

    /* Find table type by memory address */
    status = getTableType (devNum,addr, &tableType);
    if (status != GT_OK)
    {
        return status;
    }

    /* Get entry size */
    status = getEntrySize (devNum, tableType, &entrySize);
    if (status != GT_OK)
    {
        return status;
    }

    /* Get the first entry to read from and the number of entries to read */
    status = getEntryIndexAndNumOfEntries (devNum, addr, tableType,
                                           entrySize, length,
                                           &firstEntryIndex, &numOfEntries);
    if (status != GT_OK)
    {
        return status;
    }

    /* read from all the entries */
    for (i = 0; i < numOfEntries; i++)
    {
        status = prvCpssDxChPortGroupReadTableEntry (devNum, portGroupId, tableType,
                                            firstEntryIndex + i, dataPtr);
        if (status != GT_OK)
        {
            return status;
        }

        dataPtr += entrySize / 4;
    }
    return GT_OK;
}


/*******************************************************************************
* hwPpTableEntryFieldWrite
*
* DESCRIPTION:
*       Indirect write to PP's table field.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP device number to write to
*       portGroupId - the port group Id , to support multi-port-group devices
*                     that need to access specific port group
*       addr        - Address offset to write to
*       data        - Data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpTableEntryFieldWrite
(
    IN GT_U8 devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 addr,
    IN GT_U32 data
)
{
    PRV_CPSS_DXCH_TABLE_ENT tableType;    /* table type */
    GT_STATUS               status;       /* returned status */
    GT_U32                  entryIndex;   /* index of entry to write */
    GT_U32                  wordNum;      /* word number inside the entry */

    /* Find table type by memory address */
    status = getTableType (devNum,addr, &tableType);
    if (status != GT_OK)
    {
        return status;
    }

    /* Retrieve the entry index and the word number */
    status = getEntryIndexAndWordNum (devNum, tableType, addr, &entryIndex,
                                      &wordNum);
    if (status != GT_OK)
    {
        return status;
    }

    /* Write field */
    return prvCpssDxChPortGroupWriteTableEntryField(devNum, portGroupId,
                                                    tableType, entryIndex,
                                                    wordNum, 0, 32, data);
}

/*******************************************************************************
* hwPpTableEntryFieldRead
*
* DESCRIPTION:
*       Indirect read from PP's table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP device number to read from
*       portGroupId - the port group Id , to support multi-port-group devices
*                     that need to access specific port group
*       addr        - Address offset to read from
*
* OUTPUTS:
*       dataPtr - (pointer to) the read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hwPpTableEntryFieldRead
(
    IN GT_U8 devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 addr,
    OUT GT_U32 *dataPtr
)
{
    PRV_CPSS_DXCH_TABLE_ENT tableType;    /* table type */
    GT_STATUS               status;       /* returned status */
    GT_U32                  entryIndex;   /* index of entry to write */
    GT_U32                  wordNum;      /* word number inside the entry */

    /* Find table type by memory address */
    status = getTableType (devNum,addr, &tableType);
    if (status != GT_OK)
    {
        return status;
    }

    /* Retrieve the entry index and the word number */
    status = getEntryIndexAndWordNum (devNum, tableType, addr, &entryIndex,
                                      &wordNum);
    if (status != GT_OK)
    {
        return status;
    }

    /* Read field */
    return prvCpssDxChPortGroupReadTableEntryField (devNum, portGroupId,
                                                    tableType, entryIndex,
                                                    wordNum, 0, 32, dataPtr);
}


/*******************************************************************************
* checkIfRegisterExist
*
* Description:
*      Check register existance in particular port group.
*
* INPUTS:
*       devNum          - device number
*       portGroupId     - the port group Id , to support multi-port-group
*                           devices that need to access specific port group
*       regAddr         - Register address
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE    - register exists in specific port group
*       GT_FALSE   - register doesn't exist in specific port group
*
* Comment:
*       None.
*
 *******************************************************************************/
static GT_BOOL checkIfRegisterExist
(
    IN GT_U8  devNum,
    IN GT_U32 portGroupId,
    IN GT_U32 regAddr
)
{
    GT_PORT_GROUPS_BMP  portGroupsBmp;  /*port groups bmp */
    GT_BOOL             existFlag = GT_FALSE;

    if(GT_FALSE ==
        prvCpssDxChDuplicatedMultiPortGroupsGet(devNum,regAddr,&portGroupsBmp))
    {
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    if ((1 << portGroupId) & portGroupsBmp)
    {
        existFlag = GT_TRUE;
    }

    return existFlag;
}

/*******************************************************************************
* cpssDxChDiagMemTest
*
* DESCRIPTION:
*       Performs memory test on a specified memory location and size for a
*       specified memory type.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number to test
*       memType     - The packet processor memory type to verify.
*       startOffset - The offset address to start the test from.
*       size        - The memory size in byte to test (start from offset).
*       profile     - The test profile.
*
* OUTPUTS:
*       testStatusPtr  - (pointer to) test status. GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - (pointer to) address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - (pointer to) value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - (pointer to) value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong memory type
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The test is done by writing and reading a test pattern.
*       The function may be called after Phase 2 initialization.
*       The test is destructive and leaves the memory corrupted.
*       Supported memories:
*       - Buffer DRAM
*       - MAC table memory
*       - VLAN table memory
*       For buffer DRAM:
*          startOffset must be aligned to 64 Bytes and size must be in 64 bytes
*          resolution.
*       For MAC table:
*          startOffset must be aligned to 16 Bytes and size must be in 16 bytes
*          resolution.
*       For VLAN table:
*          DX CH devices: startOffset must be aligned to 12 Bytes and size must
*                         be in 12 bytes resolution.
*          DX CH2 and above devices: startOffset must be aligned to 16 Bytes
*                 and size must be in 16 bytes resolution.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagMemTest
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         startOffset,
    IN GT_U32                         size,
    IN CPSS_DIAG_TEST_PROFILE_ENT     profile,
    OUT GT_BOOL                       *testStatusPtr,
    OUT GT_U32                        *addrPtr,
    OUT GT_U32                        *readValPtr,
    OUT GT_U32                        *writeValPtr
)
{
    GT_U32    memBase;                                /* RAM base Address */
    GT_U32    testBase;                               /* test base address */
    GT_STATUS ret;                                    /* returned value */
    GT_U32    savedBurstSize;                         /* system burst size */
    GT_U32    maskArray[LION_BUFFER_DRAM_PAGE_SIZE_CNS / 4];   /* mask array */
    GT_U32    maskArraySizeInWords;                   /* the size of mask array in words */
    GT_U32    regAddr;                                /* register address */
    GT_U32    macActionMode = 0;                      /* MAC action mode */
    GT_U32    i;                                      /* loop index */
    PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC specialRamFuncs;
    GT_U32    vlanEntrySize;                          /* Vlan entry size in bytes */
    GT_U32    portGroupsBmp;                          /* Bitmap of valid port groups */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(testStatusPtr);
    CPSS_NULL_PTR_CHECK_MAC(addrPtr);
    CPSS_NULL_PTR_CHECK_MAC(readValPtr);
    CPSS_NULL_PTR_CHECK_MAC(writeValPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    /* check input parameters */
    if (memType == CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E)
    {
        if (((startOffset % MAC_ENTRY_SIZE_CNS) != 0) ||
            ((size % MAC_ENTRY_SIZE_CNS) != 0))
        {
            return GT_BAD_PARAM;
        }
        if ((startOffset + size) > getMacTableSize(devNum))
        {
            return GT_BAD_PARAM;
        }
    }
    else if (memType == CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E)
    {
        if ((startOffset + size) > getVlanTableSize(devNum))
        {
            return GT_BAD_PARAM;
        }
    }
    else if (memType == CPSS_DIAG_PP_MEM_BUFFER_DRAM_E)
    {
        if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* xCat2, xCat, DxCh3, DxCh2 and DxCh1 */
            if (((startOffset % BUFFER_DRAM_PAGE_SIZE_CNS) != 0) ||
                ((size % BUFFER_DRAM_PAGE_SIZE_CNS) != 0))
            {
                return GT_BAD_PARAM;
            }
        }
        else
        {
            /* Lion */
            if (((startOffset % LION_BUFFER_DRAM_PAGE_SIZE_CNS) != 0) ||
                ((size % LION_BUFFER_DRAM_PAGE_SIZE_CNS) != 0))
            {
                return GT_BAD_PARAM;
            }
        }
    }
    else
    {
        return GT_BAD_PARAM;
    }

    cpssOsMemSet (&specialRamFuncs, 0,
                  sizeof(PRV_CPSS_DIAG_RAM_SPECIAL_TREATMENT_FUNC_STC));


    if (memType == CPSS_DIAG_PP_MEM_BUFFER_DRAM_E)
    {
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* Lion */
            maskArraySizeInWords = (LION_BUFFER_DRAM_PAGE_SIZE_CNS / 4);

            /* unmask the lsb word */
            maskArray[0] = 0xffffffff;

            /* The word is written to all words in the line - mask all words,
               staring from second word */
            for (i = 1; i < maskArraySizeInWords; i++)
            {
                maskArray[i] = 0x0;
            }
        }
        else
        {
            maskArraySizeInWords = (BUFFER_DRAM_PAGE_SIZE_CNS / 4);

            /* All the bits are read/write - unmask all of them */
            for (i = 0; i < maskArraySizeInWords; i++)
            {
                maskArray[i] = 0xffffffff;
            }
        }

        ret = prvCpssDiagMemMaskArraySet (maskArraySizeInWords, maskArray);
        if (ret != GT_OK)
        {
            return ret;
        }
        return diagTestBuffBanksMem (devNum, memType, startOffset, size,
                                     profile, (maskArraySizeInWords * 4),
                                     testStatusPtr, addrPtr, readValPtr,
                                     writeValPtr);
    }


    /* if we reached this place then memory is either MAC table or VLAN table */
    if ((memType == CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E) &&
        (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)))
    {
        /* xCat and above, direct access to the tables, do nothing */
    }
    else
    {
        /* indirect access to the tables */
        specialRamFuncs.prvCpssDiagHwPpMemoryReadFuncPtr = &hwPpTableEntryRead;
        specialRamFuncs.prvCpssDiagHwPpMemoryWriteFuncPtr = &hwPpTableEntryWrite;
    }

    /*Prepare Address Space Partitioning setings*/
    ret = getMemBaseFromType(devNum, memType, &memBase);
    if (ret != GT_OK)
    {
        return ret;
    }

    /*Save default burst size*/
    savedBurstSize = prvCpssDiagBurstSizeGet();

    testBase = memBase + startOffset;

    if (memType == CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E)
    {
        ret = prvCpssDiagBurstSizeSet (MAC_ENTRY_SIZE_CNS);
        if (ret != GT_OK)
        {
            return ret;
        }

        /* set array mask */
        maskArray[0] = 0xffffffff;
        maskArray[1] = 0xffffffff;
        maskArray[2] = 0xffffffff;
        maskArray[3] = 0x3ff;
        ret = prvCpssDiagMemMaskArraySet (4, maskArray);
        if (ret != GT_OK)
        {
            return ret;
        }

        /* Mac Table Action 0 Register */
        ret = prvCpssDrvHwPpGetRegField (devNum, regAddr, 2, 1, &macActionMode);
        if (ret != GT_OK)
        {
            return ret;
        }

        /* Set MAC action mode to triggered aging */
        if (macActionMode == 0)
        {
            ret = prvCpssDrvHwPpSetRegField (devNum, regAddr, 2, 1, 1);
            if (ret != GT_OK)
            {
                return ret;
            }
        }

    }
    else if (memType == CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E)
    {
        switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            case CPSS_PP_FAMILY_CHEETAH_E:
                vlanEntrySize = CH_VLAN_ENTRY_SIZE_CNS;

                /* check device type */
                if ((PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX107_CNS) ||
                    (PRV_CPSS_PP_MAC(devNum)->devType == CPSS_98DX106_CNS))
                {
                    /* set array mask
                       In Cheetah Dx107 and Dx106 there are only 10 ports,
                       the other port bits should be masked */
                    maskArray[0] = 0xffffffff;
                    maskArray[1] = 0xfff;
                    maskArray[2] = 0xffffc000;
                }
                else
                {
                    /* set array mask */
                    maskArray[0] = 0xffffffff;
                    maskArray[1] = 0xffffffff;
                    maskArray[2] = 0xffffffff;
                }
                break;

            case CPSS_PP_FAMILY_CHEETAH2_E:
            case CPSS_PP_FAMILY_CHEETAH3_E:
                /* DxCh2 and DxCh3 */
                vlanEntrySize = CH2_VLAN_ENTRY_SIZE_CNS;
    
                /* set array mask */
                maskArray[0] = 0xffffffff;
                maskArray[1] = 0xffffffff;
                maskArray[2] = 0xffffffff;
                maskArray[3] = 0x3ff;
                break;

            case CPSS_PP_FAMILY_DXCH_XCAT_E:
            case CPSS_PP_FAMILY_DXCH_XCAT2_E:
                /* xCat and xCat2 */
                vlanEntrySize = XCAT_VLAN_ENTRY_SIZE_CNS;
    
                /* set array mask */
                maskArray[0] = 0xffffffff;
                maskArray[1] = 0xffffffff;
                maskArray[2] = 0xffffffff;
                maskArray[3] = 0xffffffff;
                maskArray[4] = 0xffffffff;
                maskArray[5] = 0x7ffff;
                maskArray[6] = 0x0;
                maskArray[7] = 0x0;
                break;

            case CPSS_PP_FAMILY_DXCH_LION_E:
                /* Lion */
                        vlanEntrySize = LION_B0_EGR_VLAN_ENTRY_SIZE_CNS;

                        /* set array mask */
                        maskArray[0] = 0xffffffff;
                        maskArray[1] = 0xffffffff;
                        maskArray[2] = 0xffffffff;
                        maskArray[3] = 0xffffffff;
                        maskArray[4] = 0xffffffff;
                        maskArray[5] = 0xffffffff;
                        maskArray[6] = 0xffffffff;
                        maskArray[7] = 0xffffffff;
                        maskArray[8] = 0x1ff;

                        for (i = 9; i < (vlanEntrySize / 4); i++)
                        {
                            maskArray[i] = 0x0;
                        }
                break;

            default:
                return GT_FAIL;
        }


        if (((startOffset % vlanEntrySize) != 0) ||
            ((size % vlanEntrySize) != 0))
        {
            return GT_BAD_PARAM;
        }

        /* Set VLAN entry Burst size */
        ret = prvCpssDiagBurstSizeSet (vlanEntrySize);
        if (ret != GT_OK)
        {
            return ret;
        }

        /* Configure Mask Array */
        ret = prvCpssDiagMemMaskArraySet ((vlanEntrySize / 4), maskArray);
        if (ret != GT_OK)
        {
            return GT_FAIL;
        }
    }


    /* Get valid portGroup bitmap */
    if(GT_FALSE ==
        prvCpssDxChDuplicatedMultiPortGroupsGet(devNum,testBase,&portGroupsBmp))
    {
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    ret = prvCpssDiagAnyMemTest(devNum, portGroupsBmp, testBase, size, profile,
                                specialRamFuncs, testStatusPtr, addrPtr,
                                readValPtr, writeValPtr);
    if (ret != GT_OK)
    {
        return ret;
    }

    ret = prvCpssDiagBurstSizeSet(savedBurstSize);
    if (ret != GT_OK)
    {
        return ret;
    }

    if (memType == CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E)
    {
        /* Restore MAC action mode */
        if (macActionMode == 0)
        {
            PRV_CH_DIAG_CHECK_RC_MAC(prvCpssDrvHwPpSetRegField(devNum, regAddr,
                                                               2, 1,
                                                               macActionMode));
        }

    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagAllMemTest
*
* DESCRIPTION:
*       Performs memory test for all the internal and external memories.
*       Tested memories:
*       - Buffer DRAM
*       - MAC table memory
*       - VLAN table memory
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number to test
*
* OUTPUTS:
*       testStatusPtr  - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The test is done by invoking cpssDxChDiagMemTest in loop for all the
*       memory types and for AA-55, random and incremental patterns.
*       The function may be called after Phase 2 initialization.
*       The test is destructive and leaves the memory corrupted.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagAllMemTest
(
    IN GT_U8                     devNum,
    OUT GT_BOOL                  *testStatusPtr,
    OUT GT_U32                   *addrPtr,
    OUT GT_U32                   *readValPtr,
    OUT GT_U32                   *writeValPtr
)
{
    CPSS_DRAM_SIZE_ENT size;           /* size of DRAM */
    CPSS_DIAG_PP_MEM_TYPE_ENT memType; /* memory type */
    GT_STATUS status;                  /* returned status */
    GT_U32    bank0Start;              /* start of buffer DRAM bank 0 (unused)*/
    GT_U32    bank0Size;               /* size of buffer DRAM bank 0 */
    GT_U32    bank1Start;              /* start of buffer DRAM bank 1 (unused)*/
    GT_U32    bank1Size;               /* size of buffer DRAM bank 1 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(testStatusPtr);
    CPSS_NULL_PTR_CHECK_MAC(addrPtr);
    CPSS_NULL_PTR_CHECK_MAC(readValPtr);
    CPSS_NULL_PTR_CHECK_MAC(writeValPtr);

    /* Check Buffer RAM */
    memType = CPSS_DIAG_PP_MEM_BUFFER_DRAM_E;
    getBanksParams (devNum, &bank0Start, &bank0Size, &bank1Start, &bank1Size);

    size = bank0Size + bank1Size;
    status = prvCpssDiagMemoryForAllProfiles (devNum, memType, size,
                                              &cpssDxChDiagMemTest,
                                              testStatusPtr, addrPtr,
                                              readValPtr, writeValPtr);
    PRV_CPSS_DIAG_CHECK_TEST_RESULT_MAC(status, *testStatusPtr);


    /* Check MAC table memory */
    memType = CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E;
    size = getMacTableSize (devNum);
    status = prvCpssDiagMemoryForAllProfiles (devNum, memType, size,
                                              &cpssDxChDiagMemTest,
                                              testStatusPtr, addrPtr,
                                              readValPtr, writeValPtr);
    PRV_CPSS_DIAG_CHECK_TEST_RESULT_MAC(status, *testStatusPtr);

    /* Check VLAN table memory */
    memType = CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E;
    size = getVlanTableSize (devNum);
    status = prvCpssDiagMemoryForAllProfiles (devNum, memType, size,
                                              &cpssDxChDiagMemTest,
                                              testStatusPtr, addrPtr,
                                              readValPtr, writeValPtr);
    return status;
}

/*******************************************************************************
* cpssDxChDiagMemWrite
*
* DESCRIPTION:
*       performs a single 32 bit data write to one of the PP memory spaces.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device number
*       memType - The packet processor memory type
*       offset  - The offset address to write to
*       data    - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_TIMEOUT - after max time that the PP not finished action
*       GT_BAD_PARAM             - on wrong devNum, memType or offset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagMemWrite
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    IN GT_U32                         data
)
{
    return cpssDxChDiagPortGroupMemWrite(devNum,
                                         CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                         memType, offset, data);
}

/*******************************************************************************
* cpssDxChDiagMemRead
*
* DESCRIPTION:
*       performs a single 32 bit data read from one of the PP memory spaces.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device number
*       memType - The packet processor memory type
*       offset  - The offset address to read from
*
* OUTPUTS:
*       dataPtr - (pointer to) read data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_BAD_PARAM             - on wrong devNum, memType or offset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagMemRead
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    OUT GT_U32                        *dataPtr
)
{
    return cpssDxChDiagPortGroupMemRead(devNum,
                                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        memType, offset, dataPtr);
}

/*******************************************************************************
* cpssDxChDiagRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PP PCI configuration or
*       internal registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       ifChannel  - interface channel (PCI/SMI/TWSI)
*       regType    - The register type
*       offset     - the register offset
*       data       - data to write
*       doByteSwap - GT_TRUE:  byte swap will be done on the written data
*                    GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagRegWrite
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
)
{
    return prvCpssDiagRegWrite (baseAddr, ifChannel, regType, offset, data,
                                doByteSwap);
}

/*******************************************************************************
* cpssDxChDiagRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PP PCI configuration or
*       internal registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       ifChannel  - interface channel (PCI/SMI/TWSI)
*       regType    - The register type
*       offset     - the register offset
*       doByteSwap - GT_TRUE:  byte swap will be done on the read data
*                    GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - read data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagRegRead
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
)
{
    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    return prvCpssDiagRegRead (baseAddr, ifChannel, regType, offset, dataPtr,
                               doByteSwap);
}

/*******************************************************************************
* cpssDxChDiagPhyRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PHY registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr     - The base address to access the device
*       ifChannel    - interface channel (PCI/SMI/TWSI)
*       smiRegOffset - The SMI register offset
*       phyAddr      - phy address to access
*       offset       - PHY register offset
*       data         - data to write
*       doByteSwap   - GT_TRUE:  byte swap will be done on the written data
*                      GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPhyRegWrite
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN GT_U32                          smiRegOffset,
    IN GT_U32                          phyAddr,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
)
{
    return prvCpssDiagPhyRegWrite (baseAddr, ifChannel, smiRegOffset, phyAddr,
                                   offset, data, doByteSwap);
}

/*******************************************************************************
* cpssDxChDiagPhyRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PHY registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr     - The base address to access the device
*       ifChannel    - interface channel (PCI/SMI/TWSI)
*       smiRegOffset - The SMI register offset
*       phyAddr      - phy address to access
*       offset       - PHY register offset
*       doByteSwap   - GT_TRUE:  byte swap will be done on the read data
*                      GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - (pointer to) read data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPhyRegRead
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN GT_U32                          smiRegOffset,
    IN GT_U32                          phyAddr,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
)
{
    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    return prvCpssDiagPhyRegRead (baseAddr, ifChannel, smiRegOffset, phyAddr,
                                  offset, dataPtr, doByteSwap);
}

/*******************************************************************************
* cpssDxChDiagRegsNumGet
*
* DESCRIPTION:
*       Gets the number of registers for the PP.
*       Used to allocate memory for cpssDxChDiagRegsDump.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*
* OUTPUTS:
*       regsNumPtr    - (pointre to) number of registers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagRegsNumGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *regsNumPtr
)
{
    return cpssDxChDiagPortGroupRegsNumGet(devNum,
                                           CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                           regsNumPtr);
}

/*******************************************************************************
* cpssDxChDiagRegsDump
*
* DESCRIPTION:
*       Dumps the device register addresses and values according to the given
*       starting offset and number of registers to dump.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*       regsNumPtr    - (pointer to) number of registers to dump. This number must not be
*                       bigger than the number of registers that can be dumped
*                       (starting at offset).
*       offset        - The first register address to dump
*
* OUTPUTS:
*       regsNumPtr    - (pointer to) number of registers that were dumped
*       regAddrPtr    - (pointer to) addresses of the dumped registers
*                       The addresses are taken from the register DB.
*       regDataPtr    - (pointer to) data in the dumped registers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       To dump all the registers the user may call cpssDxChDiagRegsNumGet in
*       order to get the number of registers of the PP.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagRegsDump
(
    IN    GT_U8     devNum,
    INOUT GT_U32    *regsNumPtr,
    IN    GT_U32    offset,
    OUT   GT_U32    *regAddrPtr,
    OUT   GT_U32    *regDataPtr
)
{
    return cpssDxChDiagPortGroupRegsDump(devNum,
                                         CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                         regsNumPtr,
                                         offset,
                                         regAddrPtr,
                                         regDataPtr);
}

/*******************************************************************************
* cpssDxChDiagRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of a specific register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*       regAddr       - Register to test
*       regMask       - Register mask. The test verifies only the non-masked
*                       bits.
*       profile       - The test profile
*
* OUTPUTS:
*       testStatusPtr - (pointer to) to test result. GT_TRUE if the test succeeded or GT_FALSE for failure
*       readValPtr    - (pointer to) value read from the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       writeValPtr   - (pointer to) value written to the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       After the register is tested, the original value prior to the test is
*       restored.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagRegTest
(
    IN GT_U8                          devNum,
    IN GT_U32                         regAddr,
    IN GT_U32                         regMask,
    IN CPSS_DIAG_TEST_PROFILE_ENT     profile,
    OUT GT_BOOL                       *testStatusPtr,
    OUT GT_U32                        *readValPtr,
    OUT GT_U32                        *writeValPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(testStatusPtr);
    CPSS_NULL_PTR_CHECK_MAC(readValPtr);
    CPSS_NULL_PTR_CHECK_MAC(writeValPtr);

    return prvCpssDiagRegTest (devNum, regAddr, regMask, profile, testStatusPtr,
                               readValPtr, writeValPtr);
}

/*******************************************************************************
* cpssDxChDiagAllRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of all the registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*
* OUTPUTS:
*       testStatusPtr - (pointer to) test result. GT_TRUE if the test succeeded or GT_FALSE for failure
*       badRegPtr     - (pointer to) address of the register which caused the failure if
*                       testStatusPtr is GT_FALSE. Irrelevant if testStatusPtr
*                       is GT_TRUE.
*       readValPtr    - (pointer to) value read from the register which caused the
*                       failure if testStatusPtr is GT_FALSE. Irrelevant if
*                       testStatusPtr is GT_TRUE
*       writeValPtr   - (pointer to) value written to the register which caused the
*                       failure if testStatusPtr is GT_FALSE. Irrelevant if
*                       testStatusPtr is GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The test is done by invoking cpssDxChDiagRegTest in loop for all the
*       diagnostics registers and for all the patterns.
*       After each register is tested, the original value prior to the test is
*       restored.
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagAllRegTest
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *testStatusPtr,
    OUT GT_U32  *badRegPtr,
    OUT GT_U32  *readValPtr,
    OUT GT_U32  *writeValPtr
)
{
    GT_U32 regsArray[8];        /* diagnostic registers */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(testStatusPtr);
    CPSS_NULL_PTR_CHECK_MAC(badRegPtr);
    CPSS_NULL_PTR_CHECK_MAC(readValPtr);
    CPSS_NULL_PTR_CHECK_MAC(writeValPtr);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* lion */
        regsArray[0] = 0x018000B0;  /* Egress Interrupts Mask.                    */
        regsArray[1] = 0x11004010;  /* Egress Analyzer Enable                     */
        regsArray[2] = 0x02000300;  /* VLAN MRU Profile Configuration.            */
        regsArray[7] = 0x02800000;  /* Router Global Control0                     */
    }
    else
    {
        regsArray[0] = 0x0180011C;  /* Transmit Queue General Interrupt Mask Reg. */
        regsArray[1] = 0x01800010;  /* Bridge Egress Configuration Register       */
        regsArray[2] = 0x0B800004;  /* Ingress VLAN EtherType Configuration Reg.  */
        regsArray[7] = 0x01800040;  /* Profile 0 SDWRR Weights Config. Register 0 */
    }


    regsArray[3] = 0x04004100;  /* LED Interface 0 Control Register 0         */
    regsArray[4] = 0x02000800;  /* IEEE Reserved MC Configuration Register    */
    regsArray[5] = 0x020400B4;  /* MAC Address Count 1 Register               */
    regsArray[6] = 0x06000040;  /* Message from CPU Register 0                */


    return prvCpssDiagAllRegTest (devNum, regsArray, 8, testStatusPtr,
                                  badRegPtr, readValPtr, writeValPtr);
}


/*******************************************************************************
 PRBS sequence for tri-speed ports:
 1.


*******************************************************************************/

/*******************************************************************************
* cpssDxChDiagPrbsPortTransmitModeSet
*
* DESCRIPTION:
*       Set transmit mode for specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       mode     - transmit mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       CPU port doesn't support the transmit mode.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortTransmitModeSet
(
    IN   GT_U8                    devNum,
    IN   GT_U8                    portNum,
    IN   GT_U32                   laneNum,
    IN   CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT   mode
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      macCtrl2RegAddr;
    GT_U32      portState;  /* current port state (enable/disable) */
    GT_U32      value;      /* register field value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

  
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        /* FE ports not support PRBS */
        if(portMacType == PRV_CPSS_PORT_FE_E)
            return GT_NOT_SUPPORTED;

        switch(mode)
        {
            case CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E:
                value = 0;
                break;
            case CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS_E:
                value = 1;
                break;
            case CPSS_DXCH_DIAG_TRANSMIT_MODE_ZEROS_E:
                value = 2;
                break;
            case CPSS_DXCH_DIAG_TRANSMIT_MODE_ONES_E:
                value = 3;
                break;
            default:
                return GT_BAD_PARAM;
        }
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);

        /* store value of port state */
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 0, 1, &portState) != GT_OK)
            return GT_HW_ERROR;

        /* disable port if we need */
        if (portState == 1)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
                return GT_HW_ERROR;
        }

        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&macCtrl2RegAddr);

        /* set SelectDataToTransmit */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,macCtrl2RegAddr,12,2,value) != GT_OK)
            return GT_HW_ERROR;

        /* enable port if we need */
        if (portState == 1)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 1) != GT_OK)
                return GT_HW_ERROR;
        }
    }
    else
    {
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum) == 0)
        {
            return GT_NOT_SUPPORTED;
        }

        if (laneNum > 3)
        {
            return GT_BAD_PARAM;
        }
        switch(mode)
        {
            case CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E:
                value = 0;
                break;
            case CPSS_DXCH_DIAG_TRANSMIT_MODE_CYCLIC_E:
                value = 4;
                break;
            case CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E:
                value = 5;
                break;
            case CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS23_E:
                value = 6;
                break;
            default:
                return GT_BAD_PARAM;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].laneConfig0[laneNum];

        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 13, 3, value) != GT_OK)
            return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortTransmitModeGet
*
* DESCRIPTION:
*       Get transmit mode for specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       modePtr  - (pointer to) transmit mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       CPU port doesn't support the transmit mode.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortTransmitModeGet
(
    IN   GT_U8                    devNum,
    IN   GT_U8                    portNum,
    IN   GT_U32                   laneNum,
    OUT  CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT   *modePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);


    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        /* FE ports not support PRBS */
        if(portMacType == PRV_CPSS_PORT_FE_E)
            return GT_NOT_SUPPORTED;

        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);

        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 12, 2, &value)!= GT_OK)
            return GT_HW_ERROR;
        switch (value)
        {
            case 0:
                *modePtr = CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E;
                break;
            case 1:
                *modePtr = CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS_E;
                break;
            case 2:
                *modePtr = CPSS_DXCH_DIAG_TRANSMIT_MODE_ZEROS_E;
                break;
            case 3:
                *modePtr = CPSS_DXCH_DIAG_TRANSMIT_MODE_ONES_E;
                break;
            default:
                return GT_HW_ERROR;
        }
    }
    else
    {
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum) == 0)
        {
            return GT_NOT_SUPPORTED;
        }

        if (laneNum > 3)
        {
            return GT_BAD_PARAM;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].laneConfig0[laneNum];

        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 13, 3, &value)!= GT_OK)
            return GT_HW_ERROR;
        if ((value & 0x4) == 0)
        {
            *modePtr = CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E;
        }
        else
        {
            switch (value & 0x3)
            {
                case 0:
                    *modePtr = CPSS_DXCH_DIAG_TRANSMIT_MODE_CYCLIC_E;
                    break;
                case 1:
                    *modePtr = CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E;
                    break;
                case 2:
                    *modePtr = CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS23_E;
                    break;
                default:
                    return GT_HW_ERROR;
            }
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortGenerateEnableSet
*
* DESCRIPTION:
*       Enable/Disable PRBS (Pseudo Random Bit Generator) pattern generation
*       per Port and per lane.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       enable   - GT_TRUE - PRBS pattern generation is enabled
*                  GT_FALSE - PRBS pattern generation is disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       When operating, enable the PRBS checker before the generator.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortGenerateEnableSet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    IN   GT_BOOL        enable
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      macCtrl2RegAddr;
    GT_U32      portState;  /* current port state (enable/disable) */
    GT_U32      value;      /* register field value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

  
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    value = (enable == GT_TRUE) ? 1 : 0;

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        /* FE ports not support PRBS */
        if(portMacType == PRV_CPSS_PORT_FE_E)
            return GT_NOT_SUPPORTED;

        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);


        /* store value of port state */
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &portState) != GT_OK)
            return GT_HW_ERROR;

        /* disable port if we need */
        if (portState == 1)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
                return GT_HW_ERROR;
        }

        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&macCtrl2RegAddr);

        /* set PRBS Generate enable */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,macCtrl2RegAddr,11,1,value) != GT_OK)
            return GT_HW_ERROR;

        /* enable port if we need */
        if (portState == 1)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, 0, 1, 1) != GT_OK)
                return GT_HW_ERROR;
        }
    }
    else
    {
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum) == 0)
        {
            return GT_NOT_SUPPORTED;
        }

        if (laneNum > 3)
        {
            return GT_BAD_PARAM;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].laneConfig0[laneNum];

        if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, 12, 1, value) != GT_OK)
            return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortGenerateEnableGet
*
* DESCRIPTION:
*       Get the status of PRBS (Pseudo Random Bit Generator) pattern generation
*       per port and per lane.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - PRBS pattern generation is enabled
*                     GT_FALSE - PRBS pattern generation is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortGenerateEnableGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_BOOL        *enablePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_U32      offset;  /* field offset */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);


  
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        /* FE ports not support PRBS */
        if(portMacType == PRV_CPSS_PORT_FE_E)
            return GT_NOT_SUPPORTED;

        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
        offset = 11;
    }
    else
    {
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum) == 0)
        {
            return GT_NOT_SUPPORTED;
        }

        if (laneNum > 3)
        {
            return GT_BAD_PARAM;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].laneConfig0[laneNum];
        offset = 12;
    }

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, offset, 1, &value)!= GT_OK)
        return GT_HW_ERROR;

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable PRBS (Pseudo Random Bit Generator) checker per port and
*       per lane.
*       When the checker is enabled, it seeks to lock onto the incoming bit
*       stream, and once this is achieved the PRBS checker starts counting the
*       number of bit errors. Tne number of errors can be retrieved by
*       cpssDxChDiagPrbsGigPortStatusGet API.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       enable   - GT_TRUE - PRBS checker is enabled
*                  GT_FALSE - PRBS checker is disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       When operating, enable the PRBS checker before the generator.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortCheckEnableSet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    IN   GT_BOOL        enable
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_U32      offset;  /* field offset */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

  
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        /* FE ports not support PRBS */
        if(portMacType == PRV_CPSS_PORT_FE_E)
            return GT_NOT_SUPPORTED;

        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
        offset = 10;
    }
    else
    {
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum) == 0)
        {
            return GT_NOT_SUPPORTED;
        }

        if (laneNum > 3)
        {
            return GT_BAD_PARAM;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].laneConfig0[laneNum];
        offset = 11;
    }

    value = (enable == GT_TRUE) ? 1 : 0;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, offset, 1, value);
}

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckEnableGet
*
* DESCRIPTION:
*       Get the status (enabled or disabled) of PRBS (Pseudo Random Bit Generator)
*       checker per port and per lane.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       enablePtr   - (pointer to) PRBS checker state.
*                     GT_TRUE - PRBS checker is enabled
*                     GT_FALSE - PRBS checker is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortCheckEnableGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_BOOL        *enablePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_U32      offset;  /* field offset */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

  
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        /* FE ports not support PRBS */
        if(portMacType == PRV_CPSS_PORT_FE_E)
            return GT_NOT_SUPPORTED;

        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);

        offset = 10;
    }
    else
    {
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum) == 0)
        {
            return GT_NOT_SUPPORTED;
        }

        if (laneNum > 3)
        {
            return GT_BAD_PARAM;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].laneConfig0[laneNum];
        offset = 11;
    }

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, offset, 1, &value)!= GT_OK)
        return GT_HW_ERROR;

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckReadyGet
*
* DESCRIPTION:
*       Get the PRBS checker ready status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       isReadyPtr  - (pointer to) PRBS checker state.
*                     GT_TRUE - PRBS checker is ready.
*                     PRBS checker has completed the initialization phase.
*                     GT_FALSE - PRBS checker is not ready.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the Tri-speed ports.
*       The Check ready status indicates that the PRBS checker has completed
*       the initialization phase. The PRBS generator at the transmit side may
*       be enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortCheckReadyGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    OUT  GT_BOOL        *isReadyPtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(isReadyPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* FE ports not support PRBS */
    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
        return GT_NOT_SUPPORTED;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        macRegs.perPortRegs[portNum].prbsCheckStatus;

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 1, 1, &value)!= GT_OK)
        return GT_HW_ERROR;

    *isReadyPtr = (value == 1) ? GT_TRUE : GT_FALSE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortStatusGet
*
* DESCRIPTION:
*       Get PRBS (Pseudo Random Bit Generator) Error Counter and Checker Locked
*       status per port and per lane.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       checkerLockedPtr        - (pointer to) checker locked state.
*                                 GT_TRUE - checker is locked on the sequence stream.
*                                 GT_FALSE - checker isn't locked on the sequence
*                                           stream.
*       errorCntrPtr            - (pointer to) PRBS Error counter. This counter represents
*                                 the number of bit mismatches detected since
*                                 the PRBS checker of the port has locked.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PRBS Error counter is cleared on read.
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortStatusGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_BOOL        *checkerLockedPtr,
    OUT  GT_U32         *errorCntrPtr
)
{
    GT_U32      statusRegAddr;  /* register address for locked status*/
    GT_U32      counterRegAddr; /* register address for error counter*/
    GT_U32      value;          /* register field value */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(checkerLockedPtr);
    CPSS_NULL_PTR_CHECK_MAC(errorCntrPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        /* FE ports not support PRBS */
        if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
            return GT_NOT_SUPPORTED;

        statusRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].prbsCheckStatus;

        counterRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].prbsErrorCounter;

    }
    else
    {
        if (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum) == 0)
        {
            return GT_NOT_SUPPORTED;
        }

        if (laneNum > 3)
        {
            return GT_BAD_PARAM;
        }

        statusRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].laneStatus[laneNum];

        counterRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].prbsErrorCounterLane[laneNum];
    }

    /* get lock status */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,statusRegAddr, 0, 1, &value)!= GT_OK)
        return GT_HW_ERROR;

    *checkerLockedPtr = (value == 1) ? GT_TRUE : GT_FALSE;

    /* get error counter */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,counterRegAddr, 0, 16, errorCntrPtr)!= GT_OK)
        return GT_HW_ERROR;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChDiagPrbsCyclicDataSet
*
* DESCRIPTION:
*       Set cylic data for transmition. See cpssDxChDiagPrbsPortTransmitModeSet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*       laneNum         - lane number
*       cyclicDataArr[4] - cyclic data array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_SUPPORTED         - request is not supported for this port type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the XAUI/HGS ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsCyclicDataSet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    IN   GT_U32         cyclicDataArr[4]
)
{
    GT_U32      regAddr;  /* register address */
    GT_U32      i;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(cyclicDataArr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if (laneNum > 3)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        macRegs.perPortRegs[portNum].cyclicData[laneNum];

    for (i =0; i < 4;i++)
    {
        if (prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,cyclicDataArr[i]) != GT_OK)
            return GT_HW_ERROR;
        regAddr += 4;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsCyclicDataGet
*
* DESCRIPTION:
*       Get cylic data for transmition. See cpssDxChDiagPrbsPortTransmitModeSet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*       laneNum         - lane number
*
* OUTPUTS:
*       cyclicDataArr[4]  - cyclic data array
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this port type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the XAUI/HGS ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsCyclicDataGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_U32         cyclicDataArr[4]
)
{
    GT_U32      regAddr;  /* register address */
    GT_U32      i;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(cyclicDataArr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if (laneNum > 3)
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        macRegs.perPortRegs[portNum].cyclicData[laneNum];

    for (i =0; i < 4;i++)
    {
        if (prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,&cyclicDataArr[i]) != GT_OK)
            return GT_HW_ERROR;
        regAddr += 4;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChDiagPrbsSerdesIndexGet
*
* DESCRIPTION:
*       Get SERDES index for lane of a port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*
* OUTPUTS:
*       portGroupIdPtr - (pointer to) the port group Id 
*                        - support multi-port-groups device
*       serdesIndexPtr - (pointer to) SERDES index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Supported only for GE and FlexLink ports.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChDiagPrbsSerdesIndexGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_U32         *portGroupIdPtr,
    OUT  GT_U32         *serdesIndexPtr
)
{
    GT_STATUS rc;            /* function return value */
    GT_U32 firstSerdes;      /* index of first SERDES for port */ 
    GT_U32 numOfSerdesLanes; /* number of SERDES lanes for port */

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    *portGroupIdPtr = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    rc = prvCpssDxChPortNumberOfSerdesLanesGet(devNum, portNum, 
                                               &firstSerdes, &numOfSerdesLanes);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {
        /* xCat2 has only one SERDES for a port */
        *serdesIndexPtr = firstSerdes; 
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
        {
            if (laneNum >= numOfSerdesLanes)
            {
                return GT_BAD_PARAM;
            }

            /* stack/flex ports have up to 4 SERDESes per port */
            *serdesIndexPtr = firstSerdes + laneNum;
        }
        else 
        {
            /* each network port SERDES is used for 4 ports */
            *serdesIndexPtr = firstSerdes;
        }
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {        
        /* in RXAUI mode each port will use 2 SERDESs
           in XAUI mode a port will use it's 2 SERDESs + another 2 from
           the next port.
           XLG mode use 8 SERDESs of ports 8..11 */
        if (laneNum >= numOfSerdesLanes)
        {
            return GT_BAD_PARAM;
        }
        *serdesIndexPtr = firstSerdes + laneNum;
    }
    else
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    /* check if SERDES is initialized and make proper initialization
       if it is needed */
    rc = prvCpssDxChLpCheckAndInitNeededSerdes(devNum, *portGroupIdPtr, 
                                               *serdesIndexPtr, 1);
    return rc;
}


/*******************************************************************************
* cpssDxChDiagPrbsSerdesTestEnableSet
*
* DESCRIPTION:
*       Enable/Disable SERDES PRBS (Pseudo Random Bit Generator) test mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*       enable    - GT_TRUE - test enabled
*                   GT_FALSE - test disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.Transmit mode should be set before enabling test mode.
*       See test cpssDxChDiagPrbsSerdesTransmitModeSet.
*       2. Supported only for GE and FlexLink ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsSerdesTestEnableSet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    IN   GT_BOOL        enable
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 serdesIndex;/* SERDES index */
    GT_U32 value;      /* register field value */
    GT_STATUS rc;      /* function return value */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* check input parameters and get SERDES index */
    rc = prvCpssDxChDiagPrbsSerdesIndexGet(devNum, portNum, laneNum, 
                                           &portGroupId, &serdesIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    value = BOOL2BIT_MAC(enable);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIndex].phyTestReg0;
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 15,1,value);
}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTestEnableGet
*
* DESCRIPTION:
*       Get the status of PRBS (Pseudo Random Bit Generator) test mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - test enabled
*                     GT_FALSE - test disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported only for GE and FlexLink ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsSerdesTestEnableGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_BOOL       *enablePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 serdesIndex;/* SERDES index */
    GT_U32 value;      /* register field value */
    GT_STATUS rc;      /* function return value */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* check input parameters and get SERDES index */
    rc = prvCpssDxChDiagPrbsSerdesIndexGet(devNum, portNum, laneNum, 
                                           &portGroupId, &serdesIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIndex].phyTestReg0;
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 15,1,&value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *enablePtr = BIT2BOOL_MAC(value);
    return GT_OK;

}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTransmitModeSet
*
* DESCRIPTION:
*       Set transmit mode for SERDES PRBS on specified port/lane.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*       mode      - transmit mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*                                  or unsupported transmit mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Only CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E supported.
*       2. Supported only for GE and FlexLink ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsSerdesTransmitModeSet
(
    IN   GT_U8                            devNum,
    IN   GT_U8                            portNum,
    IN   GT_U32                           laneNum,
    IN   CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 serdesIndex;/* SERDES index */
    GT_STATUS rc;      /* function return value */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* Only CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E supported */
    if (mode != CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* check input parameters and get SERDES index */
    rc = prvCpssDxChDiagPrbsSerdesIndexGet(devNum, portNum, laneNum, 
                                           &portGroupId, &serdesIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIndex].phyTestReg0;
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0,4,0xE);
    if (rc != GT_OK)
    {
        return rc;
    }
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIndex].phyTestDataReg5;
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0,16,0x80);
}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTransmitModeGet
*
* DESCRIPTION:
*       Get transmit mode for SERDES PRBS on specified port/lane.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
* OUTPUTS:
*       modePtr   - transmit mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on unkonown transmit mode
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported only for GE and FlexLink ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsSerdesTransmitModeGet
(
    IN   GT_U8                             devNum,
    IN   GT_U8                             portNum,
    IN   GT_U32                            laneNum,
    OUT  CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT *modePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 serdesIndex;/* SERDES index */
    GT_STATUS rc;      /* function return value */
    GT_U32 value;      /* register value */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* check input parameters and get SERDES index */
    rc = prvCpssDxChDiagPrbsSerdesIndexGet(devNum, portNum, laneNum, 
                                           &portGroupId, &serdesIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIndex].phyTestReg0;
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0,4,&value);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (value != 0xE)
    {
        return GT_BAD_STATE;
    }
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIndex].phyTestDataReg5;
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0,16,&value);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (value != 0x80)
    {
        return GT_BAD_STATE;
    }
    *modePtr = CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E;
    return GT_OK;

}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesStatusGet
*
* DESCRIPTION:
*       Get SERDES PRBS (Pseudo Random Bit Generator) pattern detector state,
*       error counter and pattern counter.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*
* OUTPUTS:
*       lockedPtr      - (pointer to) Pattern detector state.
*                         GT_TRUE - Pattern detector had locked onto the pattern.
*                         GT_FALSE - Pattern detector is not locked onto
*                                   the pattern.

*       errorCntrPtr   - (pointer to) PRBS Error counter. This counter represents
*                                 the number of bit mismatches detected since
*                                 the PRBS checker of the port has locked.
*       patternCntrPtr - (pointer to) Pattern counter. Number of 40-bit patterns
*                                 received since acquiring pattern lock.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported only for GE and FlexLink ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsSerdesStatusGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    IN   GT_U32    laneNum,
    OUT  GT_BOOL  *lockedPtr,
    OUT  GT_U32   *errorCntrPtr,
    OUT  GT_U64   *patternCntrPtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 serdesIndex;/* SERDES index */
    GT_U32 value;      /* register field value */
    GT_STATUS rc;      /* function return value */
    GT_U32 i;
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(lockedPtr);
    CPSS_NULL_PTR_CHECK_MAC(errorCntrPtr);
    CPSS_NULL_PTR_CHECK_MAC(patternCntrPtr);

    /* check input parameters and get SERDES index */
    rc = prvCpssDxChDiagPrbsSerdesIndexGet(devNum, portNum, laneNum, 
                                           &portGroupId, &serdesIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* get PHY Test Pattern Lock */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIndex].phyTestReg1;
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 14,1,&value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *lockedPtr = BIT2BOOL_MAC(value);

    /* get PHY Test Error Count */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIndex].phyTestPrbsErrCntReg1;
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0,16,&value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *errorCntrPtr = value;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIndex].phyTestPrbsErrCntReg0;
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0,16,&value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *errorCntrPtr += (value << 16);

    /* get PHY Test Pattern Count  */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesIndex].phyTestPrbsCntReg2;
    for (i =0; i < 3; i++)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0,16,&value);
        if (rc != GT_OK)
        {
            return rc;
        }
        patternCntrPtr->s[i] = (GT_U16)value;
        regAddr -=4;
    }
    patternCntrPtr->s[3] = 0;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagMemoryBistsRun
*
* DESCRIPTION:
*       Runs BIST (Built-in self-test) on specified memory.
*
* APPLICABLE DEVICES:
*        DxCh3; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat2.
*
* INPUTS:
*       devNum        - device number
*       memBistType   - type of memory
*       timeOut       - maximal time in milisecond to wait for BIST finish.
*       clearMemoryAfterTest - GT_TRUE  - Clear memory after test.
*                              GT_FALSE - Don't clear memory after test.
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT    - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_UNFIXABLE_BIST_ERROR  - on unfixable problem in the TCAM
*
* COMMENTS:
*       1. This function should not be called under traffic.
*       2. Bist destroys the content of the memory. If clearMemoryAfterTest set
*          function will clear the content of the memory after the test.
*          Application responsible to restore the content after the function
*          completion.
*******************************************************************************/
GT_STATUS cpssDxChDiagMemoryBistsRun
(
    IN  GT_U8                           devNum,
    IN  CPSS_DIAG_PP_MEM_BIST_TYPE_ENT  memBistType,
    IN  GT_U32                          timeOut,
    IN  GT_BOOL                         clearMemoryAfterTest
)
{
    GT_STATUS   rc;                   /* return code  */
    GT_U32      tcamTestConfigAddr;   /* Policy TCAM Test Config Status Register */
    GT_U32      tcamOpcodeExtHitReAddr; /* TCAM Opcode Expected Hit Rel Register */
    GT_U32      tcamOpcodeExpectedHitRel; /* TCAM Opcode Expected Hit Rel value */
    GT_U32      tcamOpcodeCommandAddr;  /* TCAM Opcode Command Register */
    GT_U32      tcamConfig0Addr;        /* TCAM Config0 Register */
    GT_U32      tcamOpcodeCommandData;  /* TCAM Opcode Command data */
    GT_U32      interruptCauseReg;      /* interrupt cause register - ECC interrupt */
    GT_U32      bistOutcome;          /* TCAM Bist outcome */
    GT_U32      regData;              /* register data */
    GT_U32      iterations;           /* Counter for loops */
    GT_U32      maxIterations;        /* maximum iterations */
    static GT_U32 zeroBuff[6] = {0};  /* zero buffer */
    GT_U32      value;
    GT_U32      portGroupId;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    switch (memBistType)
    {
        case CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E:
            tcamTestConfigAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                pclRegs.policyTcamTest.configStatusReg;
            tcamOpcodeExtHitReAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                pclRegs.policyTcamTest.opcodeExpectedHitReg;
            tcamOpcodeCommandAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                pclRegs.policyTcamTest.opcodeCommandReg;
            tcamOpcodeCommandData = 0x1E3233;
            tcamOpcodeExpectedHitRel = 0xB41;
            interruptCauseReg = (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
                                    ? 0xB800010 : 0xB800004;
            tcamConfig0Addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                pclRegs.pclTcamConfig0;
            DBG_INFO(("CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E \n"));
            break;
        case CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E:
            tcamTestConfigAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ipRegs.routerTcamTest.configStatusReg;
            tcamOpcodeExtHitReAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ipRegs.routerTcamTest.opcodeExpectedHitReg;
            tcamOpcodeCommandAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ipRegs.routerTcamTest.opcodeCommandReg;
            tcamOpcodeCommandData = 0x1E3033;
            tcamOpcodeExpectedHitRel = 0xB41;
            interruptCauseReg = (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
                                    ? 0x02040130 : 0x0D800060;
            tcamConfig0Addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ipRegs.ipTcamConfig0;
            DBG_INFO(("CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E \n"));
            break;
        default:
            return GT_BAD_PARAM;
    }
    maxIterations = timeOut / PRV_CH_DIAG_MINIMAL_SLEEP_TIME ;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /* set default value to TCAM config register[1:4] */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,tcamConfig0Addr,1, 4, &value);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (value != 0)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,tcamConfig0Addr,1, 4, 0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* Trigger MBist */
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, tcamTestConfigAddr, 0x10085);
        if( rc != GT_OK )
        {
            return rc;
        }

        iterations = 0;
        /* now busy wait until the BIST is finished */
        do
        {
            /* check that number of iteration not over the limit */
            if (iterations++ >= maxIterations)
            {
                return GT_TIMEOUT;
            }
            cpssOsTimerWkAfter(PRV_CH_DIAG_MINIMAL_SLEEP_TIME);

            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, tcamTestConfigAddr, &regData);
            if( rc != GT_OK )
            {
                return rc;
            }

        } while ((regData & 0x8)== 0);

        bistOutcome = (regData & 0x70) >> 4;
        DBG_INFO(("MBIST bistOutcome = %d \n",bistOutcome));
        if (bistOutcome != 0 && bistOutcome != 3)
        {
            return GT_UNFIXABLE_BIST_ERROR;
        }

        /* Set CompBist expected results */
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, tcamOpcodeExtHitReAddr, tcamOpcodeExpectedHitRel);
        if( rc != GT_OK )
        {
            return rc;
        }

        /* Set CompBist parameters */
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, tcamOpcodeCommandAddr, tcamOpcodeCommandData);
        if( rc != GT_OK )
        {
            return rc;
        }

        /* Trigger CompBist */
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, tcamTestConfigAddr, 0x10086);
        if( rc != GT_OK )
        {
            return rc;
        }

        iterations = 0;
        /* now busy wait until the BIST is finished */
        do
        {
            /* check that number of iteration not over the limit */
            if (iterations++ >= maxIterations)
            {
                return GT_TIMEOUT;
            }
            cpssOsTimerWkAfter(PRV_CH_DIAG_MINIMAL_SLEEP_TIME);

            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, tcamTestConfigAddr, &regData);
            if( rc != GT_OK )
            {
                return rc;
            }

        } while ((regData & 0x8) == 0);

        bistOutcome = (regData & 0x70) >> 4;
        DBG_INFO(("COMPBIST bistOutcome = %d \n",bistOutcome));

        if (bistOutcome != 0 && bistOutcome != 3 &&
             bistOutcome != 4 && bistOutcome != 5)
        {
            return GT_UNFIXABLE_BIST_ERROR;
        }
        /* Set TCAM IF back to Management */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,tcamTestConfigAddr,16,1,0);
        if( rc != GT_OK )
        {
            return rc;
        }
        /* Clear on read the ECC interrupt */
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, interruptCauseReg, &regData);
        if( rc != GT_OK )
        {
            return rc;
        }

        /* restore the value of TCAM config register[1:4] */
        if (value != 0)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,tcamConfig0Addr,1, 4, value);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if (clearMemoryAfterTest == GT_TRUE)
        {
            if (memBistType == CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E)
            {
                maxIterations = (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum)) ?
                                            (4*PRV_DXCH3_PCL_TCAM_RAW_NUMBER_CNS ) : _3K;
                for( iterations = 0; iterations < maxIterations; iterations++)
                {
                    /*  Resets the TCAM data to the default value */
                    rc = prvCpssDxChPclTcamStdRuleXandYSet(
                            devNum, portGroupId,
                            iterations, zeroBuff, zeroBuff,
                            1 /* xValid0 */,1 /* yValid0 */,1 /* xValid */,1 /* yValid */,
                            0 /* xSize */,0 /* ySize */,0 /* xSpare */,0 /* ySpare  */);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }

            }
            else if (memBistType == CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E)
            {
                maxIterations = PRV_CPSS_DXCH_PP_MAC(devNum)->
                    fineTuning.tableSize.routerAndTunnelTermTcam;
                for( iterations = 0; iterations < maxIterations ; iterations++)
                {
                    rc = prvCpssDxChRouterTunnelTermTcamInvalidateLine(
                            devNum,portGroupId,iterations);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }

            }
        }

    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagMemoryBistBlockStatusGet
*
* DESCRIPTION:
*       Gets redundancy block status.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum        - device number
*       memBistType   - type of memory
*       blockIndex    - block index
*                       CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E:
*                         valid blockIndex: 0 to 13.
*                       CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E:
*                         valid blockIndex: 0 to 19.
*
* OUTPUTS:
*       blockFixedPtr   - (pointer to) block status
*                                 GT_TRUE - row in the block was fixed
*                                 GT_FALSE - row in the block wasn't fixed
*       replacedIndexPtr - (pointer to) replaced row index.
*                                Only valid if  *blockFixedPtr is GT_TRUE.
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*          Each redundancy block contains 256 rows. Block 0: rows 0 to 255,
*          block 1: raws 256 to 511 ...
*          One raw replacement is possible though the full redundancy block
*          of 256 rows.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagMemoryBistBlockStatusGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DIAG_PP_MEM_BIST_TYPE_ENT  memBistType,
    IN  GT_U32                          blockIndex,
    OUT GT_BOOL                         *blockFixedPtr,
    OUT GT_U32                          *replacedIndexPtr
)
{
    GT_U32 regAdrr;
    GT_U32 regData;
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(blockFixedPtr);
    CPSS_NULL_PTR_CHECK_MAC(replacedIndexPtr);

    switch (memBistType)
    {
        case CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E:
            if (blockIndex >= 14)
            {
                return GT_BAD_PARAM;
            }
            regAdrr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                pclRegs.policyTcamTest.pointerRelReg[blockIndex];
            break;
        case CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E:
            if (blockIndex >= 20)
            {
                return GT_BAD_PARAM;
            }
            regAdrr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                ipRegs.routerTcamTest.pointerRelReg[blockIndex];
            break;
        default:
            return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpReadRegister(devNum, regAdrr, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }
    *blockFixedPtr = BIT2BOOL_MAC((regData >> 8) & 1);
    if (*blockFixedPtr == GT_TRUE)
    {
        *replacedIndexPtr = regData & 0xFF;
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagDeviceTemperatureGet
*
* DESCRIPTION:
*       Gets the PP temperature.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
* OUTPUTS:
*       temperaturePtr  - (pointer to) temperature in Celsius degrees
*                         (can be negative)
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDeviceTemperatureGet
(
    IN  GT_U8    devNum,
    OUT GT_32    *temperaturePtr
)
{
    GT_STATUS  rc;            /* return code       */
    GT_U32     regAddr;       /* register address  */
    GT_U32     hwValue;       /* HW Value          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(temperaturePtr);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) 
    {
        /* lion */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        txqVer1.sht.dfx.dfxStatus0;
    }
    else
    {
        /* xCat2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            dfxUnits.server.temperatureSensorStatus;
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum, 0 /*portGroupId*/, regAddr, 19, 9, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Calculate temperature by formula below: */ 
    /* T(in Celsius) = (322 - hwValue )/1.3625 */
    
    *temperaturePtr = ((3220000 - (hwValue * 10000)) / 13625);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChDiagPortGroupMemWrite
*
* DESCRIPTION:
*       performs a single 32 bit data write to one of the specific port group in
*       PP memory spaces.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device number
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       memType     - The packet processor memory type
*       offset      - The offset address to write to
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_TIMEOUT - after max time that the PP not finished action
*       GT_BAD_PARAM             - on wrong devNum, memType or offset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPortGroupMemWrite
(
    IN GT_U8                          devNum,
    IN GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    IN GT_U32                         data
)
{
    GT_U32      memBase;            /* holds RAM base Address */
    GT_STATUS   ret;                /* function return result */
    GT_U32      addr;               /* address to write */
    GT_U32      bank0Start;         /* start address of the Memory Bank0 */
    GT_U32      bank0Size;          /* size of the Memory Bank0 */
    GT_U32      bank1Start;         /* start address of the Memory Bank1 */
    GT_U32      bank1Size;          /* size of the Memory Bank1 */
    GT_U32      lineAndBunkNum;     /* The number of line and bank to write to
                                       (Relevant only for Lion) */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_U32      portGroupId;  /*the port group Id - support multi-port-groups device */
    GT_U32      validPortGroupsBmp; /* bitmap of valid portGroups       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum, portGroupsBmp);

    /* check input parameters */
    if (offset % 4)
    {
        return GT_BAD_PARAM;
    }
    /*Prepare Address Space Partitioning settings*/
    ret = getMemBaseFromType(devNum, memType, &memBase);
    if (ret != GT_OK)
    {
        return ret;
    }

    /* Get bitmap of valid portGroups */
    if(GT_FALSE ==
        prvCpssDxChDuplicatedMultiPortGroupsGet(devNum,memBase,&validPortGroupsBmp))
    {
        validPortGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    portGroupsBmp &= validPortGroupsBmp;

    if (portGroupsBmp == 0)
    {
        return GT_BAD_PARAM;
    }


    /* calculate the base address */
    addr = memBase + offset;

    if ((memType == CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E) ||
        (memType == CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E))
    {
        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            ret = hwPpTableEntryFieldWrite (devNum, portGroupId, addr, data);
            if (ret != GT_OK)
            {
                return ret;
            }

        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

        return GT_OK;
    }
    else if (memType == CPSS_DIAG_PP_MEM_BUFFER_DRAM_E)
    {
        /* check if the device is Lion */
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* The data will be written to all words in this line */
            lineAndBunkNum = (offset / LION_BUFFER_DRAM_PAGE_SIZE_CNS);

            fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

            /* check validity */
            if ((lineAndBunkNum * LION_BUFFER_DRAM_PAGE_SIZE_CNS * 8) >
                fineTuningPtr->tableSize.bufferMemory)
            {
                return GT_BAD_PARAM;
            }

                addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->addrSpace.buffMemBank0Write +
                          (lineAndBunkNum << 8);

            /* loop on all active port groups in the bmp */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
            {
                ret = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                           addr, data);
                if (ret != GT_OK)
                {
                    return ret;
                }
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        }
        else
        {
            /* check if the device is Cheetah3 */
            /* In Cheetah and Cheetah2 the Banks are sequential, so when the offset slides
               to Bank1, the address will be still (memBase + offset).
               In Cheetah3, the Banks are not sequential, so when the offset slides to
               Bank1, the address should be recalculated */
            if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                getBanksParams (devNum, &bank0Start, &bank0Size, &bank1Start, &bank1Size);

                /* check if the offset is for Bank1 */
                if (offset >= bank0Size)
                {
                    addr = bank1Start + (offset - bank0Size);
                }
            }

            /* loop on all active port groups in the bmp */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
            {
                ret = hwPpRamBuffMemWordWrite (devNum, portGroupId, addr, data);
                if (ret != GT_OK)
                {
                    return ret;
                }
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        }

        return GT_OK;
    }
    return GT_BAD_PARAM;
}


/*******************************************************************************
* cpssDxChDiagPortGroupMemRead
*
* DESCRIPTION:
*       performs a single 32 bit data read from one of the specific port group
*       in the PP memory spaces.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported,
*                          in this case read is done from first active port group.
*       memType     - The packet processor memory type
*       offset      - The offset address to read from
*
* OUTPUTS:
*       dataPtr - (pointer to) read data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_BAD_PARAM             - on wrong devNum, memType or offset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPortGroupMemRead
(
    IN GT_U8                          devNum,
    IN GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    OUT GT_U32                        *dataPtr
)
{
    GT_U32      memBase;            /* holds RAM base Address */
    GT_STATUS   ret;                /* function return result */
    GT_U32      addr;               /* address to read */
    GT_U32      bank0Start;         /* start address of the Memory Bank0 */
    GT_U32      bank0Size;          /* size of the Memory Bank0 */
    GT_U32      bank1Start;         /* start address of the Memory Bank1 */
    GT_U32      bank1Size;          /* size of the Memory Bank1 */
    GT_U32      lineAndBunkNum;     /* The number of line and bank to write to
                                       (Relevant only for Lion) */
    GT_U32      wordNum;            /* Word number in the line          */
    GT_U32      portGroupId;        /* port group Id */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;
    GT_U32      validPortGroupsBmp; /* bitmap of valid portGroups       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    /* check input parameters */
    if(offset % 4)
    {
        return GT_BAD_PARAM;
    }

    /* Prepare Address Space Partitioning settings */
    ret = getMemBaseFromType(devNum, memType, &memBase);
    if(ret != GT_OK)
    {
        return ret;
    }

    /* Get bitmap of valid portGroups */
    if(GT_FALSE ==
        prvCpssDxChDuplicatedMultiPortGroupsGet(devNum,memBase,&validPortGroupsBmp))
    {
        validPortGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    portGroupsBmp &= validPortGroupsBmp;

    if (portGroupsBmp == 0)
    {
        return GT_BAD_PARAM;
    }

    /* Get the first active port group id */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        portGroupId);


    /* calculate the test base address */
    addr = memBase + offset;

    if ((memType == CPSS_DIAG_PP_MEM_MAC_TBL_MEM_E) ||
        (memType == CPSS_DIAG_PP_MEM_VLAN_TBL_MEM_E))
    {
        return hwPpTableEntryFieldRead (devNum, portGroupId, addr, dataPtr);
    }
    else if (memType == CPSS_DIAG_PP_MEM_BUFFER_DRAM_E)
    {
        /* check if the device is Lion */
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* calculate line and bunk number */
            lineAndBunkNum = (offset / LION_BUFFER_DRAM_PAGE_SIZE_CNS);

            /* calculate word number */
            wordNum = (offset % (LION_BUFFER_DRAM_PAGE_SIZE_CNS / 4));

            fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

            /* check validity */
            if ((lineAndBunkNum * LION_BUFFER_DRAM_PAGE_SIZE_CNS * 8) >
                fineTuningPtr->tableSize.bufferMemory)
            {
                return GT_BAD_PARAM;
            }

                addr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->addrSpace.buffMemBank0Write +
                       (lineAndBunkNum << 8) + (wordNum << 2);

            return prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, addr, dataPtr);
        }
        else
        {
            /* check if the device is Cheetah3 */
            if (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
            {
                getBanksParams (devNum, &bank0Start, &bank0Size, &bank1Start, &bank1Size);

                /* check if the offset is for Bank1 */
                if (offset >= bank0Size)
                {
                    addr = bank1Start + (offset - bank0Size);
                }
            }

            return hwPpBufferRamInWordsRead (devNum, portGroupId, addr, 1, dataPtr);
        }
    }

    return GT_BAD_PARAM;
}


/*******************************************************************************
* cpssDxChDiagPortGroupRegsNumGet
*
* DESCRIPTION:
*       Gets the number of registers for the specific port group in the PP.
*       Used to allocate memory for cpssDxChDiagRegsDump.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported,
*                          in this case read is done from first active port group.
*
* OUTPUTS:
*       regsNumPtr    - (pointre to) number of registers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPortGroupRegsNumGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_U32              *regsNumPtr
)
{
    GT_U32 *regsListPtr;    /* holds all elements in PRV_CPSS_DXCH_DEV_REGS_MAC */
    GT_U32 regsListSize;    /* size of regsListPtr */
    GT_U32  portGroupId;    /*the port group Id - support multi-port-groups device */
    GT_STATUS   ret;        /* function return result */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(regsNumPtr);
    /* Get the first active port group id */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        portGroupId);

    regsListPtr = (GT_U32*)PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
    regsListSize = sizeof(PRV_CPSS_DXCH_PP_REGS_ADDR_STC) / sizeof(GT_U32);

    ret = prvCpssDiagRegsNumGet (devNum, portGroupId, regsListPtr, regsListSize, 0,
                                 &checkIfRegisterExist, regsNumPtr);
    if (ret != GT_OK)
    {
        return ret;
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChDiagPortGroupRegsDump
*
* DESCRIPTION:
*       Dumps the device register addresses and values for the specific port
*       group in the PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported,
*                          in this case read is done from first active port group.
*       regsNumPtr    - (pointer to) number of registers to dump. This number must not be
*                       bigger than the number of registers that can be dumped
*                       (starting at offset).
*       offset        - The first register address to dump
*
* OUTPUTS:
*       regsNumPtr    - (pointer to) number of registers that were dumped
*       regAddrPtr    - (pointer to) addresses of the dumped registers
*       regDataPtr    - (pointer to) data in the dumped registers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       To dump all the registers the user may call cpssDxChDiagRegsNumGet in
*       order to get the number of registers of the PP.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPortGroupRegsDump
(
    IN    GT_U8                 devNum,
    IN    GT_PORT_GROUPS_BMP    portGroupsBmp,
    INOUT GT_U32                *regsNumPtr,
    IN    GT_U32                offset,
    OUT   GT_U32                *regAddrPtr,
    OUT   GT_U32                *regDataPtr
)
{
    GT_U32 *regsListPtr;  /* holds all elements in PRV_CPSS_DXCH_DEV_REGS_MAC */
    GT_U32 regsListSize;  /* size of regsListPtr */
    GT_U32  portGroupId;    /*the port group Id - support multi-port-groups device */
    GT_STATUS   ret;        /* function return result */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(regsNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(regAddrPtr);
    CPSS_NULL_PTR_CHECK_MAC(regDataPtr);
    /* Get the first active port group id */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum, portGroupsBmp,
                                                        portGroupId);

    regsListPtr = (GT_U32*)PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
    regsListSize = sizeof(PRV_CPSS_DXCH_PP_REGS_ADDR_STC) / sizeof(GT_U32);

    ret = prvCpssDiagRegsDataGet (devNum, portGroupId, regsListPtr, regsListSize,
                                  offset, &checkIfRegisterExist, regsNumPtr,
                                  regAddrPtr, regDataPtr);
    if (ret != GT_OK)
    {
        return ret;
    }

    return GT_OK;
}
