/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* prvCpssDxChPcl.c
*
* DESCRIPTION:
*       Private CPSS CH PCL API implementation
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetIf.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsCheetah3.h>
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>

#define TCAM_OPER_TRACE_MAC(x) /*cpssOsPrintf x*/

/*
 * typedef enum PRV_CPSS_DXCH_PCL_TCAM_OPERATION
 *
 * Description: commands of copying words and half-wors
 *              from TCAM address space to application memory
 *
 * Enumerations:
 *
 *    PRV_TCAM_REG_COPY_NONE_E        - "do nothing" command
 *    PRV_TCAM_REG_COPY_32_E          - copy all 32 bits as is
 *    PRV_TCAM_REG_COPY_16LOW2LOW_E   - copy low 16 bits to low 16 bits
 *    PRV_TCAM_REG_COPY_16LOW2HIGH_E  - copy low 16 bits to high 16 bits
 *    PRV_TCAM_REG_COPY_16HIGH2LOW_E  - copy high 16 bits to low 16 bits
 *
 */
typedef enum
{
    PRV_TCAM_REG_COPY_NONE_E,
    PRV_TCAM_REG_COPY_32_E,
    PRV_TCAM_REG_COPY_16LOW2LOW_E,
    PRV_TCAM_REG_COPY_16LOW2HIGH_E,
    PRV_TCAM_REG_COPY_16HIGH2LOW_E
} PRV_TCAM_REG_COPY_ENT;

/*
 * typedef: struct PRV_TCAM_REG_COPY_CMD_STC
 *
 * Description: Rule of word or half-word copy
 *              from TCAM address space to application memory
 *
 * Fields:
 *    copyRule           - copy command
 *    dataWordIndex      - index of word in application memory
 *
 * Comments:
 *
 */
typedef struct
{
    PRV_TCAM_REG_COPY_ENT copyRule;
    GT_U8 dataWordIndex; /* word of DATA entry */
} PRV_TCAM_REG_COPY_CMD_STC;

/*
 * typedef: struct PRV_TCAM_REG_TO_DATA_STC
 *
 * Description: Rule word copy
 *              from TCAM address space to application memory
 *
 * Fields:
 *    tcamWordIndex      - index of word in TCAM address space
 *    copyTo             - two commands to copy this TCAM word
 *                         or two it's half-word to application memory
 *
 * Comments:
 *
 */
typedef struct
{
    GT_U8                     tcamWordIndex; /* word of TCAM entry */
    PRV_TCAM_REG_COPY_CMD_STC copyTo[2];
} PRV_TCAM_REG_TO_DATA_STC;

/* Cheetah1 */
static PRV_TCAM_REG_TO_DATA_STC prvCpssDxChPclCh1Reg2Data[] =
{
    {0, {{PRV_TCAM_REG_COPY_32_E, 0}, {PRV_TCAM_REG_COPY_NONE_E, 0}}},
    {1, {{PRV_TCAM_REG_COPY_32_E, 1}, {PRV_TCAM_REG_COPY_NONE_E, 0}}},
    {2, {{PRV_TCAM_REG_COPY_32_E, 2}, {PRV_TCAM_REG_COPY_NONE_E, 0}}},
    {4, {{PRV_TCAM_REG_COPY_32_E, 3}, {PRV_TCAM_REG_COPY_NONE_E, 0}}},
    {5, {{PRV_TCAM_REG_COPY_32_E, 4}, {PRV_TCAM_REG_COPY_NONE_E, 0}}},
    {6, {{PRV_TCAM_REG_COPY_32_E, 5}, {PRV_TCAM_REG_COPY_NONE_E, 0}}},
};

static GT_U8 prvCpssDxChPclCh1CopyNum =
    sizeof(prvCpssDxChPclCh1Reg2Data) / sizeof(prvCpssDxChPclCh1Reg2Data[0]) ;

/* Cheetah2 */

/* any 24 byte data and 24 byte mask of PCE                 */
/* may be direct read (and written) via                     */
/* 4 register sets: data and data-mask registers            */
/* and control and control-mask registers                   */
/* the 24 bytes represented inside HW as 5 "words"          */
/* that contain 32,48,48,32 and 32 bits                     */
/* 32 bit words reside in data and data-mask registers      */
/* the 32 low bits 48 bit words also reside in              */
/* data and data-mask registers and the high 16 bit of them */
/* reside in control and control-mask registers             */
/* the table below shows how to retrieve the 24 sequence    */
/* from these registers                                     */
/* the data registers - even, the contol - odd              */

/* Cheetah2 */
static PRV_TCAM_REG_TO_DATA_STC prvCpssDxChPclCh2Reg2Data[] =
{
    {0, {{PRV_TCAM_REG_COPY_32_E, 0}, {PRV_TCAM_REG_COPY_NONE_E, 0}}},
    {2, {{PRV_TCAM_REG_COPY_32_E, 1}, {PRV_TCAM_REG_COPY_NONE_E, 0}}},
    {3, {{PRV_TCAM_REG_COPY_16LOW2LOW_E, 2},
        {PRV_TCAM_REG_COPY_NONE_E, 0}}},
    {4, {{PRV_TCAM_REG_COPY_16LOW2HIGH_E, 2},
        {PRV_TCAM_REG_COPY_16HIGH2LOW_E, 3}}},
    {5, {{PRV_TCAM_REG_COPY_16LOW2HIGH_E, 3},
        {PRV_TCAM_REG_COPY_NONE_E, 0}}},
    {6, {{PRV_TCAM_REG_COPY_32_E, 4}, {PRV_TCAM_REG_COPY_NONE_E, 0}}},
    {8, {{PRV_TCAM_REG_COPY_32_E, 5}, {PRV_TCAM_REG_COPY_NONE_E, 0}}},
};

static GT_U8 prvCpssDxChPclCh2CopyNum =
    sizeof(prvCpssDxChPclCh2Reg2Data) / sizeof(prvCpssDxChPclCh2Reg2Data[0]) ;


/* DxCh only IPCL LOOKUP 0 */
static PRV_PCL_CFG_LOOKUP_POS_STC prvCpssDxChPclCfgTblHwChPlusIngrL0Pos =
{
    0,     1,     /*enableLookupOff/len   */
    2,     10,    /*pclIdOff/Len          */
    12,    1,     /*nonIpKeyTypeOff/Len   */
    1,     1,     /*ipv4KeyTypeOff/Len    */    /* L2/L4       */
    0xFF,  0,     /*ipv6KeyTypeOff/Len    */    /* not-relevant*/
    0xFF,  0,     /*dualLookupOff/Len     */    /* not-relevant*/
    0xFF,  0      /*pclIdL01Off/Len       */    /* not-relevant*/
};

/* DxCh only IPCL LOOKUP 1 */
static PRV_PCL_CFG_LOOKUP_POS_STC prvCpssDxChPclCfgTblHwChPlusIngrL1Pos =
{
    14,    1,     /*enableLookupOff/len   */
    16,   10,     /*pclIdOff/Len          */
    26,    1,     /*nonIpKeyTypeOff/Len   */
    15,    1,     /*ipv4KeyTypeOff/Len    */ /* L2/L4 */
    27,    1,     /*ipv6KeyTypeOff/Len    */ /* std-DIP */
    0xFF,  0,     /*dualLookupOff/Len     */    /* not-relevant*/
    0xFF,  0      /*pclIdL01Off/Len       */    /* not-relevant*/
};


/* DxCh2 only IPCL LOOKUP 0 */
static PRV_PCL_CFG_LOOKUP_POS_STC prvCpssDxChPclCfgTblHwCh2IngrL0Pos =
{
    0,     1,     /*enableLookupOff/len   */
    2,    10,     /*pclIdOff/Len          */
    1,     1,     /*nonIpKeyTypeOff/Len   */
    12,    2,     /*ipv4KeyTypeOff/Len    */
    14,    2,     /*ipv6KeyTypeOff/Len    */
    0xFF,  0,     /*dualLookupOff/Len     */    /* not-relevant*/
    0xFF,  0      /*pclIdL01Off/Len       */    /* not-relevant*/
};

/* DxCh2 only IPCL LOOKUP 1 */
static PRV_PCL_CFG_LOOKUP_POS_STC prvCpssDxChPclCfgTblHwCh2IngrL1Pos =
{
    16,    1,     /*enableLookupOff/len   */
    18,   10,     /*pclIdOff/Len          */
    17,    1,     /*nonIpKeyTypeOff/Len   */
    28,    2,     /*ipv4KeyTypeOff/Len    */
    30,    2,     /*ipv6KeyTypeOff/Len    */
    0xFF,  0,     /*dualLookupOff/Len     */    /* not-relevant*/
    0xFF,  0      /*pclIdL01Off/Len       */    /* not-relevant*/
};

/* DxCh2 only EPCL */
static PRV_PCL_CFG_LOOKUP_POS_STC prvCpssDxChPclCfgTblHwCh2EgrPos =
{
    0,     1,     /*enableLookupOff/len   */
    2,     10,    /*pclIdOff/Len          */
    1,     1,     /*nonIpKeyTypeOff/Len   */
    12,    2,     /*ipv4KeyTypeOff/Len    */
    14,    2,     /*ipv6KeyTypeOff/Len    */
    0xFF,  0,     /*dualLookupOff/Len     */    /* not-relevant*/
    0xFF,  0      /*pclIdL01Off/Len       */    /* not-relevant*/
};

/* DxCh3 only IPCL LOOKUP 0 */
static PRV_PCL_CFG_LOOKUP_POS_STC prvCpssDxChPclCfgTblHwCh3IngrL0Pos =
{
    0,     1,     /*enableLookupOff/len   */
    11,   10,     /*pclIdOff/Len          */
    1,     3,     /*nonIpKeyTypeOff/Len   */
    4,     3,     /*ipv4KeyTypeOff/Len    */
    7,     3,     /*ipv6KeyTypeOff/Len    */
    10,    1,     /*dualLookupOff/Len     */
    0xFF,  0      /*pclIdL01Off/Len       */    /* not-relevant*/
};

/* DxCh3 only IPCL LOOKUP 1 */
static PRV_PCL_CFG_LOOKUP_POS_STC prvCpssDxChPclCfgTblHwCh3IngrL1Pos =
{
    21,    1,     /*enableLookupOff/len   */
    32,   10,     /*pclIdOff/Len          */
    22,    3,     /*nonIpKeyTypeOff/Len   */
    25,    3,     /*ipv4KeyTypeOff/Len    */
    28,    3,     /*ipv6KeyTypeOff/Len    */
    0xFF,  0,     /*dualLookupOff/Len     */    /* not-relevant*/
    0xFF,  0      /*pclIdL01Off/Len       */    /* not-relevant*/
};

/* xCat A1 and above IPCL LOOKUP 0 */
static PRV_PCL_CFG_LOOKUP_POS_STC prvCpssDxChPclCfgTblHwXCatIngrL0Pos =
{
    0,     1,     /*enableLookupOff/len   */
    13,   10,     /*pclIdOff/Len          */
    1,     3,     /*nonIpKeyTypeOff/Len   */
    4,     3,     /*ipv4KeyTypeOff/Len    */
    7,     3,     /*ipv6KeyTypeOff/Len    */
    10,    1,     /*dualLookupOff/Len     */
    23,   10      /*pclIdL01Off/Len       */
};

/* xCat A1 and above IPCL LOOKUP 0 */
static PRV_PCL_CFG_LOOKUP_POS_STC prvCpssDxChPclCfgTblHwXCatIngrL1Pos =
{
    0,     1,     /*enableLookupOff/len   */
    13,   10,     /*pclIdOff/Len          */
    1,     3,     /*nonIpKeyTypeOff/Len   */
    4,     3,     /*ipv4KeyTypeOff/Len    */
    7,     3,     /*ipv6KeyTypeOff/Len    */
    0xFF,  0,     /*dualLookupOff/Len     */    /* not-relevant*/
    0xFF,  0      /*pclIdL01Off/Len       */    /* not-relevant*/
};

/* DxCh3 and above EPCL */
static PRV_PCL_CFG_LOOKUP_POS_STC prvCpssDxChPclCfgTblHwCh3EgrPos =
{
    0,     1,     /*enableLookupOff/len   */
    3,    10,     /*pclIdOff/Len          */
    1,     2,     /*nonIpKeyTypeOff/Len   */
    13,    2,     /*ipv4KeyTypeOff/Len    */
    15,    2,     /*ipv6KeyTypeOff/Len    */
    0xFF,  0,     /*dualLookupOff/Len     */    /* not-relevant*/
    0xFF,  0      /*pclIdL01Off/Len       */    /* not-relevant*/
};

#if PRV_CPSS_DXCH_ERRATA_REFERENCE_CNS  /* macro for references purpose only -- must be 0 --> never 1 */
    /* note: the file uses 'direct read' from the PCL TCAM , so not needed
             the WA to FEr#2004 :
             Policy and Router TCAM Tables indirect read is not functional
       see --> PRV_CPSS_DXCH3_POLICY_AND_ROUTER_TCAM_TABLES_INDIRECT_READ_WA_E */
    PRV_CPSS_DXCH3_POLICY_AND_ROUTER_TCAM_TABLES_INDIRECT_READ_WA_E

    /* note: the file uses 'direct write' to the PCL TCAM , so not needed
             the WA to FEr#2744 :
             Indirect access to TCAM (Router and PCL) is not supported
       see --> PRV_CPSS_DXCH_LION_INDIRECT_ACCESS_TO_TCAM_IS_NOT_SUPPORTED_WA_E */
    PRV_CPSS_DXCH_LION_INDIRECT_ACCESS_TO_TCAM_IS_NOT_SUPPORTED_WA_E

#endif /* PRV_CPSS_DXCH_ERRATA_REFERENCE_CNS */


/*******************************************************************************
* prvCpssDxChPclWriteTabEntry
*
* DESCRIPTION:
*       Writes data entry triggered by the last word writing.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupId     - The port group Id. relevant only to 'multi-port-groups' devices.
*                         supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       baseAddr        - The base register's address to write to.
*                         incremented by 4 for each word.
*       dataWords       - The length of data array in words.
*       dataArr         - The data array to be written.
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclWriteTabEntry
(
    IN    GT_U8                             devNum,
    IN    GT_U32                            portGroupId,
    IN    GT_U32                            baseAddr,
    IN    GT_U32                            dataWords,
    IN    GT_U32                            dataArr[]
)
{
    GT_STATUS rc; /* return code */
    GT_U32    i;  /* loop index  */

    for (i = dataWords; (i > 0); i--)
    {
        if (i == 1)
        {
            /* verify that all words were written before write last one.*/
            /* Because last one triggers write of whole entry.          */
            GT_SYNC;
        }

        rc = prvCpssDrvHwPpPortGroupWriteRegister(
            devNum, portGroupId, baseAddr, *dataArr);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* increment register address and data pointer */
        baseAddr += 4;
        dataArr  ++;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclErrataTcamEccInterruptLock
*
* DESCRIPTION:
*       Locks/unlocks the task switching and disables/enables the TCAM TCC interrupt
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portGroupId     - The port group Id. relevant only to 'multi-port-groups' devices.
*                         supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       lock            - GT_TRUE - to lock task switching and to disable interrupt
*                       - GT_FALSE - to enable interrupt and to unlock task switching
*       savedStatePtr   - for "lock" - nothing, for "unlock" - saved state
* OUTPUTS:
*       savedStatePtr   - for "lock" - place to save state, for "unlock" - nothing
*
* RETURNS :
*       GT_OK    - success
*       GT_NOT_INITIALIZED - the internal DB for the 'WA' was not initialized
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclErrataTcamEccInterruptLock
(
    IN    GT_U8                             devNum,
    IN    GT_U32                            portGroupId,
    IN    GT_BOOL                           lock,
    INOUT GT_U32                            *savedStatePtr
)
{
    GT_BOOL   eccIntEn;    /* TCAM TCC interrupt enable          */
    GT_32     intKey;      /* CPU interrupt state                */
    GT_U32    data;        /* work data                          */
    GT_STATUS rc;          /* return code                        */
    GT_STATUS dummyRc;     /* return code to avoid warnings only */
    GT_U32    taskLockKey; /* task Lock Key                      */

    if (lock != GT_FALSE)
    {
        /* initialize for abnormal return case */
        *savedStatePtr = 0;
    }

    if (GT_FALSE == PRV_CPSS_DXCH_ERRATA_GET_MAC(
        devNum, PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E))
    {
        return GT_OK;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.
        pclInterruptCauseRegAddr == 0)
    {
        return GT_NOT_INITIALIZED;
    }

    if (lock == GT_FALSE)
    {
        /* UNLOCK case */

        eccIntEn = (GT_BOOL)*savedStatePtr;
        rc = GT_OK;

        /* ECC - interrupt enable */

        if (GT_FALSE != eccIntEn)
        {
            taskLockKey = cpssExtDrvSetIntLockUnlock(
                CPSS_OS_INTR_MODE_LOCK_E, &intKey);

            /* clear Policy Engine Interrupt Cause register by read */
            rc = prvCpssDrvHwPpPortGroupReadRegister(
                devNum, portGroupId,
                PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.
                    pclInterruptCauseRegAddr,
                &data);

            /* unmask - i.e. event enable.                */
            /* prvCpssDrvEventsMask may return not GT_OK  */
            /* if interrupt was not binded by application */
            /* or interrupts are not used.                */
            /* Need to ignore this fail.                  */
            dummyRc = prvCpssDrvEventsMask(
                devNum,
                PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.
                pclInterruptIndex,
                GT_FALSE);

            taskLockKey = cpssExtDrvSetIntLockUnlock(
                CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);
        }

        dummyRc = cpssOsTaskUnLock();

        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* LOCK case */

        dummyRc = cpssOsTaskLock();

        taskLockKey = cpssExtDrvSetIntLockUnlock(
            CPSS_OS_INTR_MODE_LOCK_E, &intKey);

        prvCpssDrvEventsMaskGet(
            devNum,
            PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.
                pclInterruptIndex,
            &eccIntEn);

        if (GT_FALSE != eccIntEn)
        {
            /* ECC - interrupt disable                    */
            /* mask - i.e. event disable.                 */
            /* prvCpssDrvEventsMask may return not GT_OK  */
            /* if interrupt was not binded by application */
            /* or interrupts are not used.                */
            /* Need to ignore this fail.                  */
            dummyRc = prvCpssDrvEventsMask(
                devNum,
                PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                    info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E.
                    pclInterruptIndex,
                GT_TRUE);
        }

        taskLockKey = cpssExtDrvSetIntLockUnlock(
            CPSS_OS_INTR_MODE_UNLOCK_E, &intKey);

        *savedStatePtr = (GT_U32)eccIntEn;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclLopokupParamCheck
*
* DESCRIPTION:
*       Checks lookup paraqmeters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum            - device number
*    direction         - Policy Engine direction, Ingress or Egress
*    lookupNum         - lookup number: 0,1
*    subLookupNum      - Sub lookup Number - for ingress lookup0
*                        means 0: lookup0_0, 1: lookup0_1,
*                        for other cases not relevant.
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclLopokupParamCheck
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT    lookupNum,
    IN  GT_U32                        subLookupNum
)
{
    CPSS_PCL_DIRECTION_ENT     maxDirection;          /* max direction                  */
    CPSS_PCL_LOOKUP_NUMBER_ENT maxIngrLookupNum;      /* max IPCL lookup num            */
    GT_U32                     maxIngrLookup0SubNum;  /* max IPCL lookup0 sublookup num */

    switch (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
            maxDirection = CPSS_PCL_DIRECTION_INGRESS_E;
            maxIngrLookupNum = CPSS_PCL_LOOKUP_1_E;
            maxIngrLookup0SubNum = 0xFFFFFFFF; /* don't care */
            break;
        case CPSS_PP_FAMILY_CHEETAH2_E:
        case CPSS_PP_FAMILY_CHEETAH3_E:
            maxDirection = CPSS_PCL_DIRECTION_EGRESS_E;
            maxIngrLookupNum = CPSS_PCL_LOOKUP_1_E;
            maxIngrLookup0SubNum = 0xFFFFFFFF; /* don't care */
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
        case CPSS_PP_FAMILY_DXCH_LION_E:
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
        default:
            maxDirection = CPSS_PCL_DIRECTION_EGRESS_E;
            maxIngrLookupNum = CPSS_PCL_LOOKUP_0_1_E;
            maxIngrLookup0SubNum = 1;
            break;
    }

    if ((GT_U32)direction > (GT_U32)maxDirection)
    {
        return GT_BAD_PARAM;
    }

    if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        if ((GT_U32)lookupNum > (GT_U32)maxIngrLookupNum)
        {
            return GT_BAD_PARAM;
        }

        if (lookupNum == CPSS_PCL_LOOKUP_0_E)
        {
            if (subLookupNum > maxIngrLookup0SubNum)
            {
                return GT_BAD_PARAM;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamRuleAbsoluteIndexGet
*
* DESCRIPTION:
*       Gets absolute rule index, i.e. it's action index in action table.
*       It also is an index of the first standard-sized part of the rule.
*       The application rule index is an amount of rules with the same
*       size that can be placed before the given rule
*       In complicated TCAM
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       ruleSize        - sizeof rule, measured in standard rules
*       ruleIndex       - index of rule
*       ruleSegment     - the number of STD-rule sized segment of rule
* OUTPUTS:
*       ruleAbsIndexPtr - (pointer to) rule absolute index
*
* RETURNS :
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleAbsoluteIndexGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            ruleSize,
    IN  GT_U32                            ruleIndex,
    IN  GT_U32                            ruleSegment,
    OUT GT_U32                            *ruleAbsIndexPtr
)
{
    GT_U32 tcamRaws; /* amount of rows in the TCAM */
    GT_U32  columnEntries=0;/* amount of entries in column */

    tcamRaws = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policyTcamRaws;
    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
        case CPSS_PP_FAMILY_CHEETAH2_E:
            break;
        case CPSS_PP_FAMILY_CHEETAH3_E:
            columnEntries = 3584/*3.5K*/;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
        case CPSS_PP_FAMILY_DXCH_LION_E:
            columnEntries = 768 /*0.75K*/;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            columnEntries = 256 /*0.25K*/;
            break;
        default:
            return GT_NOT_SUPPORTED;
    }

    if(tcamRaws == 0)
    {
        /* the device not support the TCAM */
        return GT_NOT_APPLICABLE_DEVICE;
    }

    /* DXCH and DXCH2 devices */
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        switch (ruleSize)
        {
            case 1: /* standard */
                if ((ruleIndex >= (tcamRaws * 2)) || (ruleSegment > 0))
                {
                    return GT_BAD_PARAM;
                }
                *ruleAbsIndexPtr =
                    ((ruleIndex / tcamRaws) * 512) + ((ruleIndex % tcamRaws));
                return GT_OK;

            case 2: /* extended */
                if ((ruleIndex >= tcamRaws) || (ruleSegment > 1))
                {
                    return GT_BAD_PARAM;
                }
                *ruleAbsIndexPtr = ruleIndex + (ruleSegment * 512);
                return GT_OK;

            default: return GT_BAD_PARAM;
        }
    }
    else if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        == CPSS_PP_FAMILY_CHEETAH3_E)
    {
    /* the TCAM is a matrix of PRV_DXCH3_PCL_TCAM_RAW_NUMBER_CNS rows */
    /* with 4 standard rules or 2 extended rules or one ultra rule.   */
    /* Rule indexes are increasing first in column (top to down) and  */
    /* then in row:                                                   */
    /*    **********************                                      */
    /*    *  0    *  n     * ...                                      */
    /*    **********************                                      */
    /*    *  1    *  n+1   * ...                                      */
    /*    **********************                                      */
    /*    *  2    *  n+2   * ...                                      */
    /*    **********************                                      */
    /*    *  .    *  .     * ...                                      */
    /*    **********************                                      */
    /*    *  .    *  .     * ...                                      */
    /*    **********************                                      */
    /*    *  n-1  *  2n-1  * ...                                      */
    /*    **********************                                      */

        switch (ruleSize)
        {
            case 1: /* standard */
                if ((ruleIndex >= (tcamRaws * 4)) || (ruleSegment > 0))
                {
                    return GT_BAD_PARAM;
                }
                *ruleAbsIndexPtr =
                    ((ruleIndex / tcamRaws) * columnEntries)
                    + ((ruleIndex % tcamRaws));
                return GT_OK;

            case 2: /* extended */
                if ((ruleIndex >= (tcamRaws * 2)) || (ruleSegment > 1))
                {
                    return GT_BAD_PARAM;
                }
                *ruleAbsIndexPtr =
                    /* skip columns pairs */
                    ((ruleIndex / tcamRaws)
                        * (2 * columnEntries))
                    /* offset in column that contains the origin */
                        + (ruleIndex % tcamRaws)
                    /* skip columns with the previous segments */
                        + (ruleSegment * columnEntries);
                return GT_OK;

            case 3: /* ultra */
                if ((ruleIndex >= tcamRaws) || (ruleSegment > 2))
                {
                    return GT_BAD_PARAM;
                }
                *ruleAbsIndexPtr = ruleIndex +
                    (ruleSegment * columnEntries);
                return GT_OK;

            default: return GT_BAD_PARAM;
        }
    }
    else /* xCat , Lion, xCat2 */
    {
    /* the TCAM is a matrix of PRV_DXCH3_PCL_TCAM_RAW_NUMBER_CNS rows */
    /* with 4 standard rules or 2 extended rules or one ultra rule.   */
    /* Rule indexes are increasing first in row and then in column    */
    /* (top to down):                                                 */
    /*    *************************                                   */
    /*    *  0  *  1  *  2  *  3  *                                   */
    /*    *************************                                   */
    /*    *  4  *  5  *  6  *  7  *                                   */
    /*    *************************                                   */
    /*    *  .  *  .  *  .  *  .  *                                   */
    /*    *************************                                   */

        switch (ruleSize)
        {
            case 1: /* standard */
                if ((ruleIndex >= (tcamRaws * 4)) || (ruleSegment > 0))
                {
                    return GT_BAD_PARAM;
                }
                *ruleAbsIndexPtr = ruleIndex;
                return GT_OK;

            case 2: /* extended */
                if ((ruleIndex >= (tcamRaws * 2)) || (ruleSegment > 1))
                {
                    return GT_BAD_PARAM;
                }
                *ruleAbsIndexPtr = (ruleIndex * 2) + ruleSegment;
                return GT_OK;

            case 3: /* ultra */
                if ((ruleIndex >= tcamRaws) || (ruleSegment > 2))
                {
                    return GT_BAD_PARAM;
                }
                *ruleAbsIndexPtr = (ruleIndex * 4) + ruleSegment;
                return GT_OK;

            default: return GT_BAD_PARAM;
        }
    }
}

/*******************************************************************************
* prvCpssDxChPclTcamOperationWait
*
* DESCRIPTION:
*       Waits for operation completion (if started)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT      - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamOperationWait
(
    IN  GT_U8                         devNum
)
{
    /* wait for bit 0 to clear */
    return prvCpssDxChPortGroupBusyWait(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.tcamOperationTrigger,
        0,GT_FALSE);
}

/*******************************************************************************
* prvCpssDxChPclTcamOperationStart
*
* DESCRIPTION:
*       Starts Read/Write/Invalidate rule(standard and extended)
*       Mask/Pattern/Action
*       Does exchance between intermediate regsters set and TCAM
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule,  0-1023, extended 0-511
*       waitPrevReady  - GT_TRUE - to wait that previous operation finished
*       tcamOperation  - read Action, write Mask/Pattern/Action, invalidate
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
 GT_STATUS prvCpssDxChPclTcamOperationStart
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            ruleSize,
    IN  GT_U32                            ruleIndex,
    IN  GT_BOOL                           waitPrevReady,
    IN  PRV_CPSS_DXCH_PCL_TCAM_OPERATION  tcamOperation
)
{
    GT_U32       data;      /* register data               */
    GT_U32       absIndex;  /* absolute rule index         */
    GT_U32       valid;     /* valid PCE-1, invalid-0      */
    GT_U32       extended;  /* extended PCE-1, standard-0  */
    GT_U32       action;    /* action-1, TCAM-0            */
    GT_U32       mask;      /* type of TCAM data (pattern or mask) */
    GT_U32       write;     /* to write-1, to read-0       */
    GT_STATUS    rc;        /* return code                 */
    GT_U32       regAddr;   /* register address            */
    GT_U32       accParma1RegAddr;   /* access param1 register address */
    GT_U32       compMode;  /* Cheetah2 compare mode       */
    GT_U32       accParam1; /* indirect access param1 CH3  */
    GT_U32       indexInBank; /* action index in one of 4 banks CH3  */
    GT_U32       indexOfBank; /* index of one of 4 action banks CH3  */

    TCAM_OPER_TRACE_MAC((
        "TCAM_OPER: dev: %d, ruleIndex: %d, extRule: %d, operation: %d\n",
        devNum, ruleIndex, extRule, tcamOperation));

    if (waitPrevReady != GT_FALSE)
    {
        rc = prvCpssDxChPclTcamOperationWait(devNum);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.tcamOperationTrigger;

    /* set valid bit, only relevant when a write transaction is performed
       to the TCAM Data field */
    valid = (tcamOperation == PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_VALID) ? 1 : 0;

    /* set extended bit, only relevant when a write transaction is TCAM data
      or mask fields, */
    extended = (ruleSize != 1) ? 1 : 0;

    /* set table type bit, either Action table or TCAM one */
    action = ((tcamOperation == PRV_CPSS_DXCH_PCL_TCAM_READ_ACTION) ||
              (tcamOperation == PRV_CPSS_DXCH_PCL_TCAM_WRITE_ACTION))
                ? 1 : 0;

    /* set operation type, either read or write.
      Only Action table is read by indirect access. */
    write = (tcamOperation == PRV_CPSS_DXCH_PCL_TCAM_READ_ACTION) ? 0 : 1;

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* Cheetah1 */

        /* The mask bit unlike Cheetah 2 means: 0 - mask, 1-pattern*/
        mask = (tcamOperation == PRV_CPSS_DXCH_PCL_TCAM_WRITE_MASK) ? 0 : 1;
        data = (
              (valid    << 18)
            | (extended << 17)
            | (action   << 16)
            | (mask     << 15)
            | ((ruleIndex & 0x1FFF) << 2)
            | (write    << 1)
            | 1 /* trigger bit */);
    }

    else if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        == CPSS_PP_FAMILY_CHEETAH2_E)
    {
        /* Cheetah2 */
        compMode = 3; /* default: at write mask */
        if (tcamOperation != PRV_CPSS_DXCH_PCL_TCAM_WRITE_MASK)
        {
            /* 1 - short pattern, 2 - long pattern */
            compMode = (ruleSize != 1) ? 2 : 1;
        }

        /* The mask bit means: 1 - mask, 0-pattern*/
        mask = (tcamOperation == PRV_CPSS_DXCH_PCL_TCAM_WRITE_MASK) ? 1 : 0;
        data = (
              (compMode << 19)
            | (valid    << 18)
            | (extended << 17)
            | (action   << 16)
            | (mask     << 15)
            | ((ruleIndex & 0x1FFF) << 2)
            | (write    << 1)
            | 1 /* trigger bit */);
    }

    else
    {
        /* DXCH3 and above */
        if (tcamOperation != PRV_CPSS_DXCH_PCL_TCAM_WRITE_ACTION)
        {
            return GT_BAD_VALUE;
        }

        rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
            devNum, ruleSize, ruleIndex, 0/*ruleSegment*/, &absIndex);
        if (rc != GT_OK)
        {
            return rc;
        }

        indexInBank = (absIndex % PRV_DXCH3_PCL_TCAM_RAW_NUMBER_CNS);
        indexOfBank = (absIndex / PRV_DXCH3_PCL_TCAM_RAW_NUMBER_CNS);

        data = (
                ((indexInBank & 0x2000) << 5)
                | (1 /*action*/   << 16)
                | ((indexInBank & 0x1FFF) << 2)
                | (1 /*write*/    << 1)
                | 1 /* trigger bit */);


        /* 4 "enable bits" for 4 words in the needed bank (of 4 banks) */
        accParam1 = (0x0F << (indexOfBank * 4));

        accParma1RegAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.tcamOperationParam1;

        rc = prvCpssDrvHwPpWriteRegister(devNum, accParma1RegAddr, accParam1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* start operation*/
    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, data);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPclTcamOperDataPut
*
* DESCRIPTION:
*       Writes PCE(standard and extended) Mask/Pattern/Action data
*       to addressable regster set for use by TCAM operation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - sizeof rule, measured in standard rules
*       tcamOperation  - write Mask/Pattern/Action
*       dataPtr        - data - 3 words for action,
*                               6/12 bytes for pattern/mask
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - on success.
*       other           - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamOperDataPut
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            ruleSize,
    IN  PRV_CPSS_DXCH_PCL_TCAM_OPERATION  tcamOperation,
    IN  GT_U32                            *dataPtr
)
{
    GT_U32       i;         /* loop index                     */
    GT_STATUS    rc;        /* return code                    */
    GT_U32       amount;    /* amount of operation registers  */
    GT_U32       start;     /* first operation register index */
    GT_U32       *regAddr;  /* register address array[12]     */

    regAddr =
        &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.tcamWriteData[0]);

    /* check that previous operation was completed */
    rc = prvCpssDxChPclTcamOperationWait(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* calculate number of registers and it's addresses */
    switch (tcamOperation)
    {

        case PRV_CPSS_DXCH_PCL_TCAM_WRITE_ACTION:
        if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
            <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                /* Cheetah1 & Cheetah2 */
                amount = 3;
                start  = 9;
            }
            else /* CPSS_PP_FAMILY_CHEETAH3_E and above */
            {
                /* Cheetah3 */
                amount = 4;
                start  = 6;
            }
            break;
        case PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_VALID:
        case PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_INVALID:
        case PRV_CPSS_DXCH_PCL_TCAM_WRITE_MASK:
            amount = (ruleSize != 1) ? 12 : 6;
            start  = (ruleSize != 1) ? 0 : 6;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* put action or mask or pattern to data registers */
    for (i = 0; (i < amount); i++)
    {
        rc = prvCpssDrvHwPpWriteRegister(
            devNum, regAddr[start + i], dataPtr[i]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh1PclTcamActionGet
*
* DESCRIPTION:
*       Reads PCE(standard and extended) Action data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of PCE  - owner of action to read
*       waitPrevReady  - GT_TRUE - to wait that previous operation finished
* OUTPUTS:
*       dataPtr        - data - 4 words for action
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh1PclTcamActionGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       waitPrevReady,
    OUT GT_U32                        *dataPtr
)
{
    GT_U32       i;         /* loop index                     */
    GT_STATUS    rc;        /* return code                    */
    GT_U32       *regAddr;  /* register address array[12]     */
    GT_U32       regAddrDr; /* register address direct read   */

    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH2_E) &&
        (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
           PRV_CPSS_DXCH2_POLICY_ACTION_DIRECT_BURST_ONLY_READ_WA_E)
            == GT_FALSE))
    {
        /* Cheetah2 direct action table access address */
        regAddrDr = 0x0B8C0000;

        for (i = 0; (i < 3); i++)
        {
            rc = prvCpssDrvHwPpReadRegister(
                devNum, (regAddrDr + (0x10 * ruleIndex) + (i * 4)),
                &(dataPtr[i]));
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;
    }

    /* Cheetah && Cheetah2 indirect read */
    regAddr =
        &(PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.tcamWriteData[0]);

    /* write command to read Action entry and put it to data registers */
    rc = prvCpssDxChPclTcamOperationStart(
        devNum, ruleSize, ruleIndex, waitPrevReady,
        PRV_CPSS_DXCH_PCL_TCAM_READ_ACTION);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* wait that operation was completed */
    rc = prvCpssDxChPclTcamOperationWait(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* read action entry from data registers */
    for (i = 0; (i < 3); i++)
    {
        rc = prvCpssDrvHwPpReadRegister(
            devNum, regAddr[i + 9], &(dataPtr[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamActionGet
*
* DESCRIPTION:
*       Reads PCE(standard and extended) Action data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of PCE  - owner of action to read
*       waitPrevReady  - GT_TRUE - to wait that previous operation finished
*                        relevant only for DxCh1 and DxCh2 indirect read
*                        ignored for DxCh3 and above devices
* OUTPUTS:
*       dataPtr        - data - 4 words for action
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamActionGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       waitPrevReady,
    OUT GT_U32                        *dataPtr
)
{
    GT_U32       i;         /* loop index                     */
    GT_STATUS    rc;        /* return code                    */
    GT_U32       regAddrDr; /* register address direct read   */
    GT_U32       absIndex;

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        return prvCpssDxCh1PclTcamActionGet(
            devNum, ruleSize, ruleIndex, waitPrevReady, dataPtr);
    }

    /* DXCH3 and above - direct read */
    rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
        devNum, ruleSize, ruleIndex, 0/*ruleSegment*/, &absIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    regAddrDr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.actionTableBase
        + (4 * 4 * absIndex);

    for (i = 0; (i < 4); i++)
    {
        rc = prvCpssDrvHwPpPortGroupReadRegister(
            devNum, portGroupId, (regAddrDr + (4 * i)), &(dataPtr[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamReadStdRuleAddrGet
*
* DESCRIPTION:
*       Calculate standard rule Mask/Pattern direct read address
*       both data and control registers
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule  - owner of action to read
*       wordIndex      - index of word in the rule
*       pattern        - GT_TRUE - pattern, else mask
* OUTPUTS:
*       addrPtr        - address of:  6 short rule words for pattern/mask
*                        the long rule represented as two shorts
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*       For Cheetah line consists of 8 registers
*           0,1,2,4,5,6 - 32-bit data,
*           3,7 - control (bit1 - "valid", bit3 - "long")
*       For Cheetah2 line consists of 10 registers
*           called in the document "data0-4" and "control0-4"
*           the control1 and control2 also contain 16 bits of data.
*           The short rule contains 24 * 8 = 192 bits,
*           "control" means bit19-18 "compMode", bit17 "valid"
*           "compMode" is 1 - short, 2 - long in pattern and 3 in mask
*           The representation in this program is
*           word0 - data-bit31:0
*           word1 - "control"
*           word2 - data-bit63:32
*           word3 - "control" data-bit79:64
*           word4 - data-bit111:80
*           word5 - "control" data-bit127:112
*           word6 - data-bit159:128
*           word7 - "control"
*           word8 - data-bit191:160
*           word9 - "control"
*       For Cheetah3 line consists of 8 registers
*           called in the document "data0-3" and "control0-3
*           the control0-3 also contain 16 bits of data each (bits 15:0).
*           The short rule contains 24 * 8 = 192 bits,
*           "control" means bit19-18 "compMode", bit17 "valid" , bit16 "spare"
*           "compMode" is 1 - standard, 2 - extended, 3 - ultra
*           The representation in this program is
*           word0 - data-bit31:0
*           word1 - "control" data-bit47:32
*           word2 - data-bit79:48
*           word3 - "control" data-bit95:80
*           word4 - data-bit127:96
*           word5 - "control" data-bit143:128
*           word6 - data-bit175:144
*           word7 - "control" data-bit191:176
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamReadStdRuleAddrGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    IN  GT_U32                        wordIndex,
    IN  GT_BOOL                       pattern,
    OUT GT_U32                        *addrPtr
)
{
    GT_U32       regAddr;  /* register address for direct read    */
    GT_U32       wordOff;  /* word address offset                 */
    GT_U32       maxWordIndex;   /* max Word Index                */
    GT_U32       *tcamReadWordOffsetPtr; /*offsets for words      */
    GT_U32       tcamReadEntryInc; /* address increment per entry */
    GT_U32       tcamReadMaskBase;    /* tcam Read Mask Base      */
    GT_U32       tcamReadPatternBase; /* tcam Read Pattern Base   */

    /* The adreses of read access to TCAM patterns/masks              */
    /* Both data and contol registers in common entry                 */
    /* The values for address calculation by such expression:         */
    /*    ((pattern ? tcamReadPatternBase : tcamReadMaskBase)         */
    /*    + (tcamReadEntryInc * entry_index)                          */
    /*    + tcamReadWordOffset[word_index]                            */

    /* offsets for words of the same TCAM READ entry */
    static GT_U32  ch1TcamReadWordOffset[8] =
        {0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C};

    static GT_U32  ch2TcamReadWordOffset[10] =
        {0x00, 0x04, 0x4000, 0x4004, 0x8000, 0x8004,
            0xC000, 0xC004, 0x10000, 0x10004};

    static GT_U32  ch3TcamReadWordOffset[8] =
        {0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C};

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
             == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* Cheetah1 */
        if (ruleIndex >= 1024)
        {
            return GT_BAD_PARAM;
        }
        maxWordIndex          = 8;
        tcamReadWordOffsetPtr = ch1TcamReadWordOffset;
        tcamReadEntryInc      = 0x20;
        tcamReadMaskBase      = 0x0B810000;
        tcamReadPatternBase   = 0x0B818000;
    }
    else if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
             == CPSS_PP_FAMILY_CHEETAH2_E)
    {
        /* Cheetah2 */
        if (ruleIndex >= 1024)
        {
            return GT_BAD_PARAM;
        }
        maxWordIndex          = 10;
        tcamReadWordOffsetPtr = ch2TcamReadWordOffset;
        tcamReadEntryInc      = 0x10;
        tcamReadMaskBase      = 0x0B880008;
        tcamReadPatternBase   = 0x0B880000;
    }
    else  if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
             == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        /* Cheetah3 */
        if (ruleIndex >= (PRV_DXCH3_PCL_TCAM_RAW_NUMBER_CNS * 4))
        {
            return GT_BAD_PARAM;
        }
        maxWordIndex          = 8;
        tcamReadWordOffsetPtr = ch3TcamReadWordOffset;
        tcamReadEntryInc      = 0x40;
        tcamReadMaskBase      = 0x0BA00020;
        tcamReadPatternBase   = 0x0BA00000;
    }
    else  /*xCat...*/
    {
        if (ruleIndex >= _3K)
        {
            return GT_BAD_PARAM;
        }

        maxWordIndex          = 8;
        tcamReadWordOffsetPtr = ch3TcamReadWordOffset;
        tcamReadEntryInc      = 0x40;

        if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat2 and above */
            tcamReadMaskBase      = 0x0E040020;
            tcamReadPatternBase   = 0x0E040000;
        }
        else if (PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
        {
            /* Lion and above */
            tcamReadMaskBase      = 0x0d040020;
            tcamReadPatternBase   = 0x0d040000;
        }
        else
        {
            /* xCat */
            tcamReadMaskBase      = 0x0E040020;
            tcamReadPatternBase   = 0x0E040000;
        }
    }

    if (wordIndex >= maxWordIndex)
    {
        return GT_BAD_PARAM;
    }

    regAddr =
        (pattern != GT_FALSE) ? tcamReadPatternBase : tcamReadMaskBase;

    wordOff = tcamReadWordOffsetPtr[wordIndex];

    *addrPtr = regAddr + (tcamReadEntryInc * ruleIndex) + wordOff;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleDataGet
*
* DESCRIPTION:
*       Reads standard rule Mask/Pattern data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule  - owner of action to read
*       pattern        - GT_TRUE - pattern, else mask
* OUTPUTS:
*       dataPtr        - data:  6 words for pattern/mask
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*        Cheetah and  Cheetah2, not relevant for Cheetah3
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleDataGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       pattern,
    OUT GT_U32                        *dataPtr
)
{
    GT_U8        i,j;         /* loop index                              */
    GT_U32       data;        /* register data                           */
    GT_STATUS    rc;          /* return code                             */
    GT_U32       regAddr;     /* register address                        */

    PRV_TCAM_REG_TO_DATA_STC *regCopyPtr;
    GT_U8                    regCopyNum;

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH_E)
    {
        regCopyPtr = prvCpssDxChPclCh1Reg2Data;
        regCopyNum = prvCpssDxChPclCh1CopyNum;
    }
    else
    {
        regCopyPtr = prvCpssDxChPclCh2Reg2Data;
        regCopyNum = prvCpssDxChPclCh2CopyNum;
    }

    /* clear oputput data */
    for (i = 0; (i < 6); i++)
    {
        dataPtr[i] = 0;
    }

    for (i = 0; (i < regCopyNum); i++)
    {
        rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
            devNum, ruleIndex, regCopyPtr[i].tcamWordIndex, pattern, &regAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &data);
        if (rc != GT_OK)
        {
            return rc;
        }

        for (j = 0; (j < 2); j++)
        {
            switch (regCopyPtr[i].copyTo[j].copyRule)
            {
                case PRV_TCAM_REG_COPY_NONE_E:
                    /* nothing need */
                    break;
                case PRV_TCAM_REG_COPY_32_E:
                    dataPtr[regCopyPtr[i].copyTo[j].dataWordIndex] |= data;
                    break;
                case PRV_TCAM_REG_COPY_16LOW2LOW_E:
                    dataPtr[regCopyPtr[i].copyTo[j].dataWordIndex] |=
                        (data & 0x0000FFFF);
                    break;
                case PRV_TCAM_REG_COPY_16LOW2HIGH_E:
                    dataPtr[regCopyPtr[i].copyTo[j].dataWordIndex] |=
                        ((data << 16) & 0xFFFF0000);
                    break;
                case PRV_TCAM_REG_COPY_16HIGH2LOW_E:
                    dataPtr[regCopyPtr[i].copyTo[j].dataWordIndex] |=
                        ((data >> 16) & 0x0000FFFF);
                    break;
                /* must never occur */
                default: return GT_BAD_PARAM;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
*  prvCpssDxCh1PclTcamRuleDataGet
*
* DESCRIPTION:
*       Reads rule Mask/Pattern data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
* OUTPUTS:
*       maskPtr        - data:  (ruleSize * 6) words for mask
*       patternPtr     - data:  (ruleSize * 6) words for pattern
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh1PclTcamRuleDataGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_U32                        *maskPtr,
    OUT GT_U32                        *patternPtr
)
{
    GT_STATUS    rc;          /* return code                             */
    GT_U32       i;           /* loop index                              */
    GT_U32       absIndex;    /* absolute rule index                     */

    for (i = 0; (i < ruleSize); i++)
    {
        /* the i-th part of the data */
        rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
            devNum, ruleSize, ruleIndex, i/*ruleSegment*/, &absIndex);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPclTcamStdRuleDataGet(
            devNum, absIndex,
            GT_FALSE /*pattern*/, &(maskPtr[i * 6]));
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDxChPclTcamStdRuleDataGet(
            devNum, absIndex,
            GT_TRUE /*pattern*/, &(patternPtr[i * 6]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
*  prvCpssDxChPclTcamRuleDataGet
*
* DESCRIPTION:
*       Reads rule Mask/Pattern data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
* OUTPUTS:
*       maskPtr        - data:  (ruleSize * 6) words for mask
*       patternPtr     - data:  (ruleSize * 6) words for pattern
*                        for xCat2 and above: (ruleSize * 7) words
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleDataGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_U32                        *maskPtr,
    OUT GT_U32                        *patternPtr
)
{
    GT_STATUS    rc;          /* return code                  */
    GT_U32       i;           /* loop index                   */
    GT_U32       absIndex;    /* absolute rule index          */
    GT_U32       x,y;         /* work variables               */
    GT_U32       savedStatus; /* ECC interrupt enable/disable */
    GT_STATUS    rc1;         /* return code                  */
    GT_U32       stdRuleWords;/* words in the standard rule   */

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        return prvCpssDxCh1PclTcamRuleDataGet(
            devNum, ruleSize, ruleIndex, maskPtr, patternPtr);
    }

    if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat2 and above */
        stdRuleWords = 7;
    }
    else
    {
        /* CH devices, xCat, Lion */
        stdRuleWords = 6;
    }

    rc = prvCpssDxChPclErrataTcamEccInterruptLock(
        devNum, portGroupId, GT_TRUE/*lock*/, &savedStatus);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0; (i < ruleSize); i++)
    {
        /* the i-th part of the data */
        rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
            devNum, ruleSize, ruleIndex, i/*ruleSegment*/, &absIndex);
        if (rc != GT_OK)
        {
            break;
        }

        rc = prvCpssDxChPclTcamStdRuleDataXandYGet(
            devNum, portGroupId, absIndex,
            &(patternPtr[i * stdRuleWords]),
            &(maskPtr[i * stdRuleWords]));
        if (rc != GT_OK)
        {
            break;
        }
    }

    rc1 = prvCpssDxChPclErrataTcamEccInterruptLock(
        devNum, portGroupId, GT_FALSE/*lock*/, &savedStatus);

    if (rc != GT_OK)
    {
        return rc;
    }

    /* convert from X/Y format to pattern/mask format */
    for (i = 0; (i < (ruleSize * stdRuleWords)); i++)
    {
        x = patternPtr[i];
        y = maskPtr[i];
        patternPtr[i] = PRV_CPSS_DXCH_PCL_XY_TO_P_MAC(x, y);
        maskPtr[i] =    PRV_CPSS_DXCH_PCL_XY_TO_M_MAC(x, y);
    }

    return rc1;
}

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleStateGet
*
* DESCRIPTION:
*       Reads standard rule control and converts it to state
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum         - device number
*       ruleIndex      - index of rule  - owner of action to read
* OUTPUTS:
*       validPtr       -  GT_TRUE - valid
*       ruleSizePtr    -  sizeof rule, measured in standard rules
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*        Cheetah and  Cheetah2, not relevant for Cheetah3
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT GT_U32                        *ruleSizePtr
)
{
    GT_U32       data;        /* register data                           */
    GT_STATUS    rc;          /* return code                             */
    GT_U32       regAddr;     /* register address                        */
    GT_U32       wordOff;     /* control register offset                 */
    GT_U32       extendedBit; /* bit to check for extended rule          */
    GT_U32       validBit;    /* bit to check for long rule              */

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* Cheetah1 */
        wordOff     = 3;
        extendedBit = 3;
        validBit    = 1;
    }
    else
    {
        /* Cheetah2 */
        wordOff     = 1;
        extendedBit = 19;
        validBit    = 17;
    }

    rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
        devNum, ruleIndex, wordOff, GT_TRUE /*pattern*/, &regAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *validPtr    = ((data & (1 << validBit)) != 0) ? GT_TRUE : GT_FALSE;
    *ruleSizePtr = ((data & (1 << extendedBit)) != 0) ? 2 : 1;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamRuleStateGet
*
* DESCRIPTION:
*       Reads standard rule control and converts it to state
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
* OUTPUTS:
*       validPtr            -  GT_TRUE - valid
*       ruleFoundSizePtr    -  sizeof rule, measured in standard rules
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleStateGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    OUT GT_BOOL                       *validPtr,
    OUT GT_U32                        *ruleFoundSizePtr
)
{
    GT_STATUS    rc;          /* return code                  */
    GT_U32       absIndex;    /* absolute rule index          */
    GT_U32       xValid;      /* xValid                       */
    GT_U32       yValid;      /* yValid                       */
    GT_U32       xSize;       /* xSize                        */
    GT_U32       ySize;       /* ySize                        */
    GT_U32       xSpare;      /* xSpare                       */
    GT_U32       ySpare;      /* ySpare                       */
    GT_U32       mValid;      /* mValid                       */
    GT_U32       pValid;      /* pValid                       */
    GT_U32       savedStatus; /* ECC interrupt enable/disable */
    GT_STATUS    rc1;         /* return code                  */
    GT_U32       xData;       /* X register data              */
    GT_U32       yData;       /* Y register data              */
    GT_U32       regAddr;     /* register address             */

    rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
        devNum, ruleSize, ruleIndex, 0/*ruleSegment*/, &absIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        return prvCpssDxChPclTcamStdRuleStateGet(
            devNum, absIndex, validPtr, ruleFoundSizePtr);
    }

    if (0 == PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* CH3, xCat Lion */

        rc = prvCpssDxChPclErrataTcamEccInterruptLock(
            devNum, portGroupId, GT_TRUE/*lock*/, &savedStatus);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPclTcamStdRuleStateXandYGet(
            devNum, portGroupId, absIndex,
            &xValid, &yValid, &xSize, &ySize, &xSpare, &ySpare);

        rc1 = prvCpssDxChPclErrataTcamEccInterruptLock(
            devNum, portGroupId, GT_FALSE/*lock*/, &savedStatus);

        if (rc != GT_OK)
        {
            return rc;
        }

        if (rc1 != GT_OK)
        {
            return rc1;
        }

        /* all 2 bits of the size must be checked in any state */
        if (PRV_CPSS_DXCH_PCL_XY_TO_M_MAC(xSize, ySize) != 3)
        {
            return GT_BAD_STATE;
        }

        mValid = PRV_CPSS_DXCH_PCL_XY_TO_M_MAC(xValid, yValid);
        pValid = PRV_CPSS_DXCH_PCL_XY_TO_P_MAC(xValid, yValid);

        switch (((mValid & 1) << 1) | (pValid & 1))
        {
            case 0: *validPtr = GT_TRUE;  break;  /* don't care */
            case 1: *validPtr = GT_FALSE; break;  /* never match */
            case 2: *validPtr = GT_FALSE; break;  /* match 0 only */
            case 3: *validPtr = GT_TRUE;  break;  /* match 1 only */
            default: return GT_BAD_STATE;
        }

        switch (PRV_CPSS_DXCH_PCL_XY_TO_P_MAC(xSize, ySize))
        {
            case 1: *ruleFoundSizePtr = 1; break;
            case 2: *ruleFoundSizePtr = 2; break;
            case 3: *ruleFoundSizePtr = 3; break;
            default: return GT_BAD_STATE;
        }

        return GT_OK;
    }

    /* xCat2 and above */

    rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
        devNum, absIndex, 0, GT_TRUE /*X*/, &regAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &xData);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
        devNum, absIndex, 0, GT_FALSE /*Y*/, &regAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &yData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* cut the rule size bits */
    xData &= 3;
    yData &= 3;

    switch (xData)
    {
        case 1: *ruleFoundSizePtr = 1; break;
        case 2: *ruleFoundSizePtr = 2; break;
        case 3: *ruleFoundSizePtr = 3; break;
        default: return GT_BAD_STATE;
    }

    if ((PRV_CPSS_DXCH_PCL_XY_TO_M_MAC(xData, yData) == 3)
        && (PRV_CPSS_DXCH_PCL_XY_TO_P_MAC(xData, yData) == xData))
    {
        *validPtr = GT_TRUE;
    }
    else
    {
        *validPtr = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh1PclTcamRuleWrite
*
* DESCRIPTION:
*       Writes standard or extended rule Action, Mask and Pattern to TCAM
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum         - device number
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
*       validRule      - GT_TRUE - valid, GT_FALSE - invalid
*       waitPrevReady  - GT_TRUE - to wait that previous operation finished
*       actionPtr      - action
*       maskPtr        - mask
*       patternPtr     - pattern
* OUTPUTS:
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh1PclTcamRuleWrite
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       validRule,
    IN  GT_BOOL                       waitPrevReady,
    IN  GT_U32                        *actionPtr,
    IN  GT_U32                        *maskPtr,
    IN  GT_U32                        *patternPtr
)
{
    GT_STATUS                        rc;       /* return code            */
    PRV_CPSS_DXCH_PCL_TCAM_OPERATION tcamOper; /* write valid or invalid */

    /* CH and CH2 devices */
    tcamOper = (validRule == GT_FALSE)
        ? PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_INVALID
        : PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_VALID;

    /* invalidate rule */
    rc = prvCpssDxChPclTcamOperationStart(
        devNum, ruleSize, ruleIndex, waitPrevReady,
        PRV_CPSS_DXCH_PCL_TCAM_WR_PATTERN_INVALID);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* put action data to buffer registers */
    rc = prvCpssDxChPclTcamOperDataPut(
        devNum, ruleSize,
        PRV_CPSS_DXCH_PCL_TCAM_WRITE_ACTION, actionPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* store action */
    rc = prvCpssDxChPclTcamOperationStart(
        devNum, ruleSize, ruleIndex, GT_FALSE /*waitPrevReady*/,
        PRV_CPSS_DXCH_PCL_TCAM_WRITE_ACTION);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* put mask data to buffer registers */
    rc = prvCpssDxChPclTcamOperDataPut(
        devNum, ruleSize,
        PRV_CPSS_DXCH_PCL_TCAM_WRITE_MASK, maskPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* store mask */
    rc = prvCpssDxChPclTcamOperationStart(
        devNum, ruleSize, ruleIndex, GT_FALSE /*waitPrevReady*/,
        PRV_CPSS_DXCH_PCL_TCAM_WRITE_MASK);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* put pattern data to buffer registers */
    rc = prvCpssDxChPclTcamOperDataPut(
        devNum, ruleSize,
        tcamOper, patternPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* store pattern */
    rc = prvCpssDxChPclTcamOperationStart(
        devNum, ruleSize, ruleIndex, GT_FALSE /*waitPrevReady*/,
        tcamOper);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamRuleWrite
*
* DESCRIPTION:
*       Writes standard or extended rule Action, Mask and Pattern to TCAM
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
*       validRule      - GT_TRUE - valid, GT_FALSE - invalid
*       waitPrevReady  - GT_TRUE - to wait that previous operation finished
*       actionPtr      - action
*       maskPtr        - mask
*       patternPtr     - pattern
* OUTPUTS:
*       None.
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*       For all devices the mask and pattern of rule
*       is a sequence standard sized segments.
*       For all CH devices, xCat and Lion such segment is
*       192 bits in 6 32-bit words.
*       For xCat2 and above devices such segment is
*       206 bits in 7 32-bit words, padded 18 bits unused.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleWrite
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       validRule,
    IN  GT_BOOL                       waitPrevReady,
    IN  GT_U32                        *actionPtr,
    IN  GT_U32                        *maskPtr,
    IN  GT_U32                        *patternPtr
)
{
    GT_STATUS    rc;                           /* return code            */
    GT_U32       x[7];                         /* TCAM X - data          */
    GT_U32       y[7];                         /* TCAM Y - data          */
    GT_U32       seg;                          /* rule segment index     */
    GT_U32       absIndex;                     /* absolute rule index    */
    GT_U32       i;                            /* loop index             */
    GT_U32       j;                            /* work index             */
    GT_BOOL      segValid;                     /* rule segment valid     */
    GT_U32       actionStartAddr;              /* action start address in action table */
    GT_U32       stdRuleWords;                 /* words in STD rule data */

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        /* DxCh1 and DxCh1 devices */
        return prvCpssDxCh1PclTcamRuleWrite(
            devNum, ruleSize, ruleIndex, validRule, waitPrevReady,
            actionPtr, maskPtr, patternPtr);
    }

    /* invalidate rule */
    rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
        devNum, ruleSize, ruleIndex, 0/*ruleSegment*/, &absIndex);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* invalidate rule */
    rc = prvCpssDxChPclTcamStdRuleValidStateSet(
        devNum, portGroupId, absIndex, GT_FALSE);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        <= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        /* for CH3 indirect action write */
        /* put action data to buffer registers */
        rc = prvCpssDxChPclTcamOperDataPut(
            devNum, ruleSize,
            PRV_CPSS_DXCH_PCL_TCAM_WRITE_ACTION, actionPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* store action */
        rc = prvCpssDxChPclTcamOperationStart(
            devNum, ruleSize, ruleIndex, GT_FALSE /*waitPrevReady*/,
            PRV_CPSS_DXCH_PCL_TCAM_WRITE_ACTION);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else /* xCat and above */
    {
        /* direct action write                       */
        /* action table line alignment is 0x10 bytes */
        actionStartAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.actionTableBase + 0x10*absIndex;

        rc = prvCpssDxChPclWriteTabEntry(
            devNum, portGroupId,
            actionStartAddr, 4, actionPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

    if (0 == PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* CH3, xCat, Lion */
        stdRuleWords = 6;
    }
    else
    {
        /* xCat2 and above */
        stdRuleWords = 7;
    }

    /* end of loop done by "break" */
    for (seg = (ruleSize - 1); (1) ; seg--)
    {
        /* convert pattern/mask to X/Y format */
        for (i = 0; (i < stdRuleWords); i++)
        {
            j = (seg * stdRuleWords) + i;
            x[i] = PRV_CPSS_DXCH_PCL_MP_TO_X_MAC(maskPtr[j], patternPtr[j]);
            y[i] = PRV_CPSS_DXCH_PCL_MP_TO_Y_MAC(maskPtr[j], patternPtr[j]);
        }

        rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
            devNum, ruleSize, ruleIndex, seg/*ruleSegment*/, &absIndex);
        if (rc != GT_OK)
        {
            return rc;
        }

        if ((validRule == GT_FALSE) && (seg == 0))
        {
            /* for long rule only first segment may be invalid */
            segValid = GT_FALSE;
        }
        else
        {
            segValid = GT_TRUE;
        }

        rc = prvCpssDxChPclTcamStdRuleSizeAndValidSet(
            devNum, portGroupId, absIndex, x, y,
            segValid, ruleSize);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* end of loop */
        if (seg == 0) break;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamRuleActionUpdate
*
* DESCRIPTION:
*       Writes standard or extended rule Action to TCAM
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
*       actionPtr      - action
* OUTPUTS:
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleActionUpdate
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    IN  GT_U32                        *actionPtr
)
{
    GT_STATUS    rc;                /* return code                          */
    GT_U32       absIndex;          /* absolute rule index                  */
    GT_U32       actionStartAddr;   /* action start address in action table */

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* DxCh1-3 indirect write              */
        /* put action data to buffer registers */
        rc = prvCpssDxChPclTcamOperDataPut(
            devNum, ruleSize,
            PRV_CPSS_DXCH_PCL_TCAM_WRITE_ACTION, actionPtr);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* store action */
        rc = prvCpssDxChPclTcamOperationStart(
            devNum, ruleSize, ruleIndex, GT_FALSE /*waitPrevReady*/,
            PRV_CPSS_DXCH_PCL_TCAM_WRITE_ACTION);

        return rc;
    }
    else
    {
        /* direct action write                       */
        /* action table line alignment is 0x10 bytes */
        rc = prvCpssDxChPclTcamRuleAbsoluteIndexGet(
        devNum, ruleSize, ruleIndex, 0/*ruleSegment*/, &absIndex);
        if (rc != GT_OK)
        {
            return rc;
        }
        actionStartAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.actionTableBase + 0x10*absIndex;

        rc = prvCpssDxChPclWriteTabEntry(
            devNum, portGroupId,
            actionStartAddr, 4, actionPtr);
            if (rc != GT_OK)
            {
                return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclRuleCopy
*
* DESCRIPTION:
*  The function dedicated for one of port groups of multi port group device or
*  for not multi port group device.
*  The function copies the Rule's mask, pattern and action to new TCAM position.
*  The source Rule is not invalidated by the function. To implement move Policy
*  Rule from old position to new one at first cpssDxChPclRuleCopy should be
*  called. And after this cpssDxChPclRuleInvalidate should be used to invalidate
*  Rule in old position
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       portGroupId           - The port group Id. relevant only to 'multi-port-groups' devices.
*                          supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleSizeVal      - size of Rule in STD rules.
*       ruleSrcIndex     - index of the rule in the TCAM from which pattern,
*                           mask and action are taken.
*       ruleDstIndex     - index of the rule in the TCAM to which pattern,
*                           mask and action are placed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleCopy
(
    IN GT_U8                              devNum,
    IN GT_U32                             portGroupId,
    IN GT_U32                             ruleSizeVal,
    IN GT_U32                             ruleSrcIndex,
    IN GT_U32                             ruleDstIndex
)
{
    GT_STATUS    rc;          /* return code                 */
    GT_U32       action[PRV_CPSS_DXCH_PCL_ACTION_SIZE_IN_WORDS_CNS]; /* action */
    GT_U32       mask[PRV_CPSS_DXCH_PCL_RULE_SIZE_IN_WORDS_CNS];    /* mask    */
    GT_U32       pattern[PRV_CPSS_DXCH_PCL_RULE_SIZE_IN_WORDS_CNS]; /* pattern */
    GT_BOOL      valid;       /* valid source rule           */
    GT_U32       ruleSizeFound; /* rule size found           */

    /* get Validity and size of source rule */
    rc = prvCpssDxChPclTcamRuleStateGet(
        devNum, portGroupId, ruleSizeVal, ruleSrcIndex, &valid, &ruleSizeFound);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (ruleSizeVal != ruleSizeFound)
    {
        /* source rule has another size */
        return GT_BAD_STATE;
    }

    /* get action of source rule */
    rc = prvCpssDxChPclTcamActionGet(
        devNum, portGroupId, ruleSizeVal, ruleSrcIndex,
        GT_TRUE /*waitPrevReady*/, action);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* get mask and pattern*/
    rc = prvCpssDxChPclTcamRuleDataGet(
        devNum, portGroupId, ruleSizeVal, ruleSrcIndex, mask, pattern);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* write destination rule */
    return prvCpssDxChPclTcamRuleWrite(
        devNum, portGroupId, ruleSizeVal, ruleDstIndex, valid,
        GT_TRUE /*waitPrevReady*/,
        action, mask, pattern);
}

/*******************************************************************************
* prvCpssDxChXCatPclTcamRuleIngressActionSw2HwConvert
*
* DESCRIPTION:
*       Converts TCAM Ingress Action from SW to HW format
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum           - device number
*       swActionPtr      - action in SW format
* OUTPUTS:
*       hwActionPtr      - action in HW format (4 words)
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_OUT_OF_RANGE  - one of the parameters is out of range.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXCatPclTcamRuleIngressActionSw2HwConvert
(
    IN  GT_U8                         devNum,
    IN  CPSS_DXCH_PCL_ACTION_STC      *swActionPtr,
    OUT GT_U32                        *hwActionPtr
)
{
    GT_STATUS                              rc;           /* return code            */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode;   /* DSA  Cpu Code          */
    GT_U32                                 rawCpuCode;   /* raw  Cpu Code          */
    GT_U32                                 hwValue;      /* value for enums        */
    GT_U32                                 routerLttIndex = 0; /* router LTT Index       */
    GT_U32                                 ppVersion;    /* PP version             */
    GT_U32                                 tempRouterLttPtr = 0;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            ppVersion = 0;
            break;
        case CPSS_PP_FAMILY_DXCH_LION_E:
            ppVersion = 1;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
        default:
            ppVersion = 2;
            break;
    }

    /* clear */
    hwActionPtr[0] = 0;
    hwActionPtr[1] = 0;
    hwActionPtr[2] = 0;
    hwActionPtr[3] = 0;

    if (swActionPtr->egressPolicy != GT_FALSE)
    {
        /* egress action must be translated by DxCh3 related code */
        return GT_BAD_PARAM;
    }

    if (swActionPtr->ipUcRoute.doIpUcRoute != GT_FALSE)
    {
        /* ingress Uc Rout to NextHop action must be translated by */
        /* DxCh3 related code                                      */
        return GT_BAD_PARAM;
    }

    /* ingress action */

    /* calculate HW_CPU_CODE if needed */

    if ((swActionPtr->pktCmd == CPSS_PACKET_CMD_MIRROR_TO_CPU_E)
        || (swActionPtr->pktCmd == CPSS_PACKET_CMD_TRAP_TO_CPU_E))
    {
        rc = prvCpssDxChNetIfCpuToDsaCode(
            swActionPtr->mirror.cpuCode, &dsaCpuCode);
        if (rc != GT_OK)
        {
            return GT_BAD_PARAM;
        }
        rawCpuCode = dsaCpuCode;
    }
    else
    {
        /* default */
        rawCpuCode = 0;
    }

    /* GENERAL */

    /* packet command - word0, bits 2:0 */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(
        hwValue, swActionPtr->pktCmd);
    /* bits 2:0 */
    hwActionPtr[0] |= hwValue;


    /* prevent the next lookups */
    hwActionPtr[2] |= ((BOOL2BIT_MAC(swActionPtr->actionStop)) << 15);

    /* bypass bridge */
    hwActionPtr[2] |= ((BOOL2BIT_MAC(swActionPtr->bypassBridge)) << 16);

    /* bypass ingress pipe */
    hwActionPtr[2] |= ((BOOL2BIT_MAC(swActionPtr->bypassIngressPipe)) << 17);

    if (swActionPtr->redirect.redirectCmd
        == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        /* If specified redirection to egress interface      */
        /* one of bypassBridge or bypassIngressPipe          */
        /* must be specified - otherwise following ingress   */
        /* engines like bridge may to override output        */
        /* interface or forwarding command                   */
        if ((swActionPtr->bypassIngressPipe == 0)
            && (swActionPtr->bypassBridge == 0))
        {
            /* bypass bridge */
            hwActionPtr[2] |= (1 << 16);
        }
    }

    /* MIRROR */

    /* CPU code */
    hwActionPtr[0] |= ((rawCpuCode & 0x00FF) << 3);

    /* mirror to analyzer port */
    hwActionPtr[0] |=
        ((BOOL2BIT_MAC(swActionPtr->mirror.mirrorToRxAnalyzerPort)) << 11);

    /* mirror TCP disconnect messages to CPU */
    hwActionPtr[3] |=
        ((BOOL2BIT_MAC(swActionPtr->mirror.mirrorTcpRstAndFinPacketsToCpu)) << 17);

    /* LOOKUP */

    /* IPCL0_1 override Cfg table access index */
    hwActionPtr[2] |=
        ((BOOL2BIT_MAC(swActionPtr->lookupConfig.pcl0_1OverrideConfigIndex)) << 19);

    /* IPCL1 override Cfg table access index */
    hwActionPtr[2] |=
        ((BOOL2BIT_MAC(swActionPtr->lookupConfig.pcl1OverrideConfigIndex)) << 18);

    /* IPCL Cfg table access index */
    if (swActionPtr->lookupConfig.ipclConfigIndex != 0)
    {
        if (swActionPtr->redirect.redirectCmd
            == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
        {
            return GT_BAD_PARAM;
        }

        if (swActionPtr->lookupConfig.ipclConfigIndex >= BIT_13)
        {
            return GT_OUT_OF_RANGE;
        }

        hwActionPtr[0] |= ((swActionPtr->lookupConfig.ipclConfigIndex & 1) << 31);
        hwActionPtr[1] |= (swActionPtr->lookupConfig.ipclConfigIndex >> 1);

    }

    /* REDIRECT */

    switch (swActionPtr->redirect.redirectCmd)
    {
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E:
            /* bits 14:12 already contain 0 */;
            break;
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E:
            hwActionPtr[0] |= (1 << 12);
            /* redirect interface */
            switch (swActionPtr->redirect.data.outIf.outInterface.type)
            {
                case CPSS_INTERFACE_PORT_E:
                    /* device */
                    if(swActionPtr->redirect.data.outIf.outInterface.devPort.devNum
                       >= BIT_5)
                    {
                        return GT_OUT_OF_RANGE;
                    }
                    hwActionPtr[0] |=
                        (swActionPtr->redirect.data.outIf.outInterface.devPort.devNum
                         << 22);
                    /* port */
                    if (swActionPtr->redirect.data.outIf.outInterface.devPort.portNum
                       >= BIT_6)
                    {
                        return GT_OUT_OF_RANGE;
                    }
                    hwActionPtr[0] |=
                        (swActionPtr->redirect.data.outIf.outInterface.devPort.portNum
                         << 16);
                    break;
                case CPSS_INTERFACE_TRUNK_E:
                    /* TRUNK IF */
                    hwActionPtr[0] |= (1 << 15);
                    /* trunk ID */
                    if(swActionPtr->redirect.data.outIf.outInterface.trunkId
                       >= BIT_7)
                    {
                        return GT_OUT_OF_RANGE;
                    }
                    hwActionPtr[0] |=
                        (swActionPtr->redirect.data.outIf.outInterface.trunkId
                         << 16);
                    break;
                case CPSS_INTERFACE_VIDX_E:
                    /* multicast IF */
                    hwActionPtr[0] |= (1 << 27);
                    /* VIDX */
                    if(swActionPtr->redirect.data.outIf.outInterface.vidx
                       >= BIT_12)
                    {
                        return GT_OUT_OF_RANGE;
                    }
                    hwActionPtr[0] |=
                        (swActionPtr->redirect.data.outIf.outInterface.vidx
                           << 15);
                    break;
                case CPSS_INTERFACE_VID_E:
                    /* multicast IF */
                    hwActionPtr[0] |= (1 << 27);
                    /* VID - use constant 0xFFF for send to VLAN */
                    hwActionPtr[0] |= (0x0FFF << 15);
                    break;
                default: return GT_BAD_PARAM;
            }

            /* tunnel start */
            if (swActionPtr->redirect.data.outIf.tunnelStart == GT_TRUE)
            {
                /* tunnel start */
                hwActionPtr[0] |= (1 << 28);

                /* tunnel PTR bits 41:29 */
                if (swActionPtr->redirect.data.outIf.tunnelPtr >= BIT_13)
                {
                    return GT_OUT_OF_RANGE;
                }
                hwActionPtr[0] |=
                    ((swActionPtr->redirect.data.outIf.tunnelPtr & 7) << 29);
                hwActionPtr[1] |=
                    (swActionPtr->redirect.data.outIf.tunnelPtr >> 3);
                /* tunnel type */
                switch (swActionPtr->redirect.data.outIf.tunnelType)
                {
                    case CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E:
                        /* the data already contains 0 */
                        break;
                    case CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E:
                        hwActionPtr[1] |= (1 << 10);
                        break;
                    default: return GT_BAD_PARAM;
                }

            }
            else
            {
                /* vntL2Echo */
                hwActionPtr[1] |=
                    ((BOOL2BIT_MAC(swActionPtr->redirect.data.outIf.vntL2Echo))
                     << 11);
                if (ppVersion > 0)
                {
                    if (swActionPtr->redirect.data.outIf.arpPtr >= BIT_14)
                    {
                        return GT_OUT_OF_RANGE;
                    }
                    hwActionPtr[0] |=
                        ((swActionPtr->redirect.data.outIf.arpPtr & 7) << 29);
                    hwActionPtr[1] |=
                        (swActionPtr->redirect.data.outIf.arpPtr >> 3);
                    hwActionPtr[3] |=
                        ((BOOL2BIT_MAC(swActionPtr->redirect.data.outIf.modifyMacDa))
                         << 20);
                    hwActionPtr[3] |=
                        ((BOOL2BIT_MAC(swActionPtr->redirect.data.outIf.modifyMacSa))
                         << 21);
                }
            }
            break;
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E:
            if (ppVersion == 2)
            {
                /* xCat2 */
                return GT_NOT_APPLICABLE_DEVICE;
            }
            hwActionPtr[0] |= (2 << 12);
             /* ROUTER */
            if(swActionPtr->redirect.data.routerLttIndex>= BIT_15)
            {
                return GT_OUT_OF_RANGE;
            }

            if (swActionPtr->redirect.data.routerLttIndex > (fineTuningPtr->tableSize.routerAndTunnelTermTcam * 4))
                return GT_BAD_PARAM;

            /* FEr#2018: Limited number of Policy-based routes */
            if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E))
            {
                /* xCat devices support index 0,4,8,12…(max IP TCAM row * 4) only */
                if ((swActionPtr->redirect.data.routerLttIndex % 4) != 0)
                    return GT_NOT_SUPPORTED;
            }

            /* LTT entry is row based however in this field the LTT is treated as column based.
               Bits [11:0] indicate row while bits [14:13] indicate column, bit 12 is not used.
               The Formula for translating the LTT entry to column based is as follow:
               [11:0] << 2 + [14:13]   (Note: bit 12 not used). */
            tempRouterLttPtr = (((routerLttIndex & 0x3FFC) >> 2) | ((routerLttIndex & 0x3) << 13));
            hwActionPtr[0] |= (tempRouterLttPtr << 15);

        break;
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E:
            if (ppVersion == 2)
            {
                /* xCat2 */
                return GT_NOT_APPLICABLE_DEVICE;
            }
            hwActionPtr[0] |= (4 << 12);
             /* VIRT_ROUTER */
            if(swActionPtr->redirect.data.vrfId>= BIT_12)
            {
                return GT_OUT_OF_RANGE;
            }
            hwActionPtr[0] |=
                (swActionPtr->redirect.data.vrfId << 15);
            break;
        default: return GT_BAD_PARAM;
    }

    /* Match counter */

    /* bind to CNC counter */
    hwActionPtr[1] |=
        ((BOOL2BIT_MAC(swActionPtr->matchCounter.enableMatchCount))
         << 12);

    /* counter index */
    if (swActionPtr->matchCounter.matchCounterIndex >= BIT_14)
    {
        return GT_OUT_OF_RANGE;
    }
    hwActionPtr[1] |= (swActionPtr->matchCounter.matchCounterIndex << 13);

    /* POLICER */
    switch (swActionPtr->policer.policerEnable)
    {
        case CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E:
            /* assumed hwActionPtr[1] bis 28:27 are 0 */
            break;
        case CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E:
            /* bind to POLICER counter */
            /* bind to POLICER meter */
            hwActionPtr[1] |= (3 << 27);
            break;
        case CPSS_DXCH_PCL_POLICER_ENABLE_METER_ONLY_E:
            /* bind to POLICER meter */
            hwActionPtr[1] |= (1 << 27);
            break;
        case CPSS_DXCH_PCL_POLICER_ENABLE_COUNTER_ONLY_E:
            /* bind to POLICER counter */
            hwActionPtr[1] |= (1 << 28);
            break;
        default: return GT_BAD_PARAM;
    }

    /* counter index */
    if (swActionPtr->policer.policerId >= BIT_12)
    {
        return GT_OUT_OF_RANGE;
    }
    hwActionPtr[1] |= ((swActionPtr->policer.policerId & 7) << 29);
    hwActionPtr[2] |= (swActionPtr->policer.policerId >> 3);

    /* SOURCE ID */

    /* source Id assignment enable */
    hwActionPtr[2] |=
        ((BOOL2BIT_MAC(swActionPtr->sourceId.assignSourceId)) << 9);

    /* source Id value */
    if (swActionPtr->sourceId.sourceIdValue >= BIT_5)
    {
        return GT_OUT_OF_RANGE;
    }
    hwActionPtr[2] |= (swActionPtr->sourceId.sourceIdValue << 10);

    /* VLAN assignment */

    /* vlan precedence */
    PRV_CPSS_CONVERT_PRECEDENCE_TO_HW_VAL_MAC(
        hwValue, swActionPtr->vlan.precedence);
    hwActionPtr[2] |= (hwValue << 20);

    /* nested VLAN */
    hwActionPtr[2] |=
        ((BOOL2BIT_MAC(swActionPtr->vlan.nestedVlan)) << 21);

    /* modify vlan command */
    switch (swActionPtr->vlan.modifyVlan)
    {
        case CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E:
            hwValue = 0;
            break;
        case CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E:
            hwValue = 1;
            break;
        case CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E:
            hwValue = 2;
            break;
        case CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E:
            hwValue = 3;
            break;
        default: return GT_BAD_PARAM;
    }

    hwActionPtr[2] |= (hwValue << 22);

    /* VLAN id */
    if (swActionPtr->vlan.vlanId >= BIT_12)
    {
        return GT_OUT_OF_RANGE;
    }
    hwActionPtr[2] |= ((swActionPtr->vlan.vlanId & 0xFF) << 24);
    hwActionPtr[3] |= (swActionPtr->vlan.vlanId >> 8);

    /* QOS */

    /* qos profile assignment enable */
    hwActionPtr[3] |=
        ((BOOL2BIT_MAC(swActionPtr->qos.qos.ingress.profileAssignIndex)) << 4);

    /* qos profile assignment precedence */
    PRV_CPSS_CONVERT_PRECEDENCE_TO_HW_VAL_MAC(
        hwValue, swActionPtr->qos.qos.ingress.profilePrecedence)
    hwActionPtr[3] |= (hwValue << 5);

    /* qos profile */
    if (swActionPtr->qos.qos.ingress.profileIndex >= BIT_7)
    {
        return GT_OUT_OF_RANGE;
    }
    hwActionPtr[3] |= (swActionPtr->qos.qos.ingress.profileIndex << 6);

    /* enable modify DSCP */
    PRV_CPSS_PCL_CONVERT_ATTR_MODIFY_TO_HW_VAL_MAC(
        hwValue, swActionPtr->qos.modifyDscp);
    hwActionPtr[3] |= (hwValue << 13);

    /* enable modify UP */
    PRV_CPSS_PCL_CONVERT_ATTR_MODIFY_TO_HW_VAL_MAC(
        hwValue, swActionPtr->qos.modifyUp);
    hwActionPtr[3] |= (hwValue << 15);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXCatPclTcamRuleIngressActionSw2HwConvert
*
* DESCRIPTION:
*       Converts TCAM Ip Unicast Routing Action from SW to HW format
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       swActionPtr      - action in SW format
* OUTPUTS:
*       hwActionPtr      - action in HW format (4 words)
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_OUT_OF_RANGE  - one of the parameters is out of range.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXCatPclTcamRuleIpUcRoutingActionSw2HwConvert
(
    IN  CPSS_DXCH_PCL_ACTION_STC      *swActionPtr,
    OUT GT_U32                        *hwActionPtr
)
{
    GT_U32                                 hwValue;      /* value for enums        */

    /* Ip Unicast Routing action                       */
    /* Assumed but not checked:                        */
    /* swActionPtr->egressPolicy          == GT_FALSE  */
    /* swActionPtr->ipUcRoute.doIpUcRoute == GT_TRUE   */

    /* clear */
    hwActionPtr[0] = 0;
    hwActionPtr[1] = 0;
    hwActionPtr[2] = 0;
    hwActionPtr[3] = 0;

    /* GENERAL */

    /* packet command - word0, bits 2:0 */
    PRV_CPSS_CONVERT_PACKET_CMD_TO_HW_VAL_MAC(
        hwValue, swActionPtr->pktCmd);
    /* bits 2:0 */
    hwActionPtr[0] |= hwValue;

    /* MIRROR */

    /* mirror to analyzer port */
    hwActionPtr[0] |=
        ((BOOL2BIT_MAC(swActionPtr->mirror.mirrorToRxAnalyzerPort)) << 11);

    /* REDIRECT */

    /* Action used as next hop entry                  */
    /* swActionPtr->redirect.redirectCmd ignored      */
    /* The data treared as interface                  */
    /* for CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E */
    /* All tunnel related data ignored                */
    hwActionPtr[0] |= (3 << 12);

    /* redirect interface */
    switch (swActionPtr->redirect.data.outIf.outInterface.type)
    {
        case CPSS_INTERFACE_PORT_E:
            /* device */
            if(swActionPtr->redirect.data.outIf.outInterface.devPort.devNum
               >= BIT_5)
            {
                return GT_OUT_OF_RANGE;
            }
            hwActionPtr[1] |=
                (swActionPtr->redirect.data.outIf.outInterface.devPort.devNum
                 << 9);
            /* port */
            if (swActionPtr->redirect.data.outIf.outInterface.devPort.portNum
               >= BIT_6)
            {
                return GT_OUT_OF_RANGE;
            }
            hwActionPtr[1] |=
                (swActionPtr->redirect.data.outIf.outInterface.devPort.portNum
                 << 3);
            break;
        case CPSS_INTERFACE_TRUNK_E:
            /* TRUNK IF */
            hwActionPtr[1] |= (1 << 2);
            /* trunk ID */
            if(swActionPtr->redirect.data.outIf.outInterface.trunkId
               >= BIT_7)
            {
                return GT_OUT_OF_RANGE;
            }
            hwActionPtr[1] |=
                (swActionPtr->redirect.data.outIf.outInterface.trunkId
                 << 3);
            break;
        case CPSS_INTERFACE_VIDX_E:
            /* multicast IF */
            hwActionPtr[1] |= (1 << 14);
            /* VIDX */
            if(swActionPtr->redirect.data.outIf.outInterface.vidx
               >= BIT_12)
            {
                return GT_OUT_OF_RANGE;
            }
            hwActionPtr[1] |=
                (swActionPtr->redirect.data.outIf.outInterface.vidx
                   << 2);
            break;
        case CPSS_INTERFACE_VID_E:
            /* multicast IF */
            hwActionPtr[1] |= (1 << 14);
            /* VID - use constant 0xFFF for send to VLAN */
            hwActionPtr[1] |= (0x0FFF << 2);
            break;
        default: return GT_BAD_PARAM;
    }


    /* Match counter */

    /* bind to CNC counter */
    hwActionPtr[0] |=
        ((BOOL2BIT_MAC(swActionPtr->matchCounter.enableMatchCount))
         << 28);

    /* counter index */
    if (swActionPtr->matchCounter.matchCounterIndex >= BIT_5)
    {
        return GT_OUT_OF_RANGE;
    }
    hwActionPtr[0] |=
        ((swActionPtr->matchCounter.matchCounterIndex & 0x07) << 29);
    hwActionPtr[1] |=
        ((swActionPtr->matchCounter.matchCounterIndex >> 3) & 0x03);

    /* VLAN assignment */

    /* vlan precedence ignored      */
    /* nested VLAN ignored          */
    /* modify vlan command  ignored */
    /* VLAN id */
    if (swActionPtr->vlan.vlanId >= BIT_12)
    {
        return GT_OUT_OF_RANGE;
    }
    hwActionPtr[1] |= (swActionPtr->vlan.vlanId << 19);

    /* QOS */

    /* qos profile assignment enable */
    hwActionPtr[0] |=
        ((BOOL2BIT_MAC(swActionPtr->qos.qos.ingress.profileAssignIndex)) << 16);

    /* qos profile assignment precedence */
    PRV_CPSS_CONVERT_PRECEDENCE_TO_HW_VAL_MAC(
        hwValue, swActionPtr->qos.qos.ingress.profilePrecedence)
    hwActionPtr[0] |= (hwValue << 15);

    /* qos profile */
    if (swActionPtr->qos.qos.ingress.profileIndex >= BIT_7)
    {
        return GT_OUT_OF_RANGE;
    }
    hwActionPtr[0] |= (swActionPtr->qos.qos.ingress.profileIndex << 17);

    /* enable modify DSCP */
    PRV_CPSS_PCL_CONVERT_ATTR_MODIFY_TO_HW_VAL_MAC(
        hwValue, swActionPtr->qos.modifyDscp);
    hwActionPtr[0] |= (hwValue << 24);

    /* enable modify UP */
    PRV_CPSS_PCL_CONVERT_ATTR_MODIFY_TO_HW_VAL_MAC(
        hwValue, swActionPtr->qos.modifyUp);
    hwActionPtr[0] |= (hwValue << 26);

    /* Ip Uc specific */

    /* arpDaIndex */
    if (swActionPtr->ipUcRoute.arpDaIndex >= BIT_10)
    {
        return GT_OUT_OF_RANGE;
    }
    hwActionPtr[0] |= ((swActionPtr->ipUcRoute.arpDaIndex & 0x00FF) << 3);
    hwActionPtr[1] |= (((swActionPtr->ipUcRoute.arpDaIndex >> 8) & 0x03) << 17);

    /* icmpRedirectCheck */
    hwActionPtr[2] |= (BOOL2BIT_MAC(swActionPtr->ipUcRoute.icmpRedirectCheck) << 1);

    /* bypassTTLCheck */
    hwActionPtr[2] |= (BOOL2BIT_MAC(swActionPtr->ipUcRoute.bypassTTLCheck) << 2);

    /* decrementTTL */
    hwActionPtr[2] |= (BOOL2BIT_MAC(swActionPtr->ipUcRoute.decrementTTL) << 3);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamRuleActionSw2HwConvert
*
* DESCRIPTION:
*       Converts TCAM Action from SW to HW format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       swActionPtr      - action in SW format
* OUTPUTS:
*       hwActionPtr      - action in HW format (4 words)
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_OUT_OF_RANGE  - one of the parameters is out of range.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleActionSw2HwConvert
(
    IN  GT_U8                         devNum,
    IN  CPSS_DXCH_PCL_ACTION_STC      *swActionPtr,
    OUT GT_U32                        *hwActionPtr
)
{
    GT_U32    cheetahVer;    /* version of Cheetah                  */
    GT_STATUS rc;            /* return code                         */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT   dsaCpuCode; /* cpuCode ih HW format */
    GT_BOOL   useCpuCode;    /* GT_TRUE for TRAP & mirror to CPU    */
    GT_U32    routerLttIndex = 0; /* router LTT index                    */
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
            cheetahVer = 1;
            break;
        case CPSS_PP_FAMILY_CHEETAH2_E:
            cheetahVer = 2;
            break;
        case CPSS_PP_FAMILY_CHEETAH3_E:
            cheetahVer = 3;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
        case CPSS_PP_FAMILY_DXCH_LION_E:
        default:
            if (swActionPtr->egressPolicy == GT_FALSE)
            {
                if (swActionPtr->ipUcRoute.doIpUcRoute == GT_FALSE)
                {
                    /* xCat and above INGRESS action */
                    return prvCpssDxChXCatPclTcamRuleIngressActionSw2HwConvert(
                        devNum, swActionPtr, hwActionPtr);
                }
                else
                {
                    /* xCat and above IP Uc routing action */
                    return prvCpssDxChXCatPclTcamRuleIpUcRoutingActionSw2HwConvert(
                        swActionPtr, hwActionPtr);
                }
            }

            /* EGRESS action is the same as for DxCh3        */
            cheetahVer = 4;
            break;
    }

    /* validate Egress action                */
    /* Only fields that not ignored by code  */
    /* common for CH3 ingress and any Egress */
    if (swActionPtr->egressPolicy != GT_FALSE)
    {
        switch (swActionPtr->pktCmd)
        {
            case CPSS_PACKET_CMD_FORWARD_E: break;
            case CPSS_PACKET_CMD_DROP_HARD_E: break;
            default: return GT_BAD_PARAM;
        }
        if (swActionPtr->actionStop != GT_FALSE)
        {
            return GT_BAD_PARAM;
        }
    }

    /* clear */
    hwActionPtr[0] = 0;
    hwActionPtr[1] = 0;
    hwActionPtr[2] = 0;
    hwActionPtr[3] = 0;

    useCpuCode = GT_FALSE;
    /* packet command - word0, bits 2:0 */
    switch (swActionPtr->pktCmd)
    {
        case CPSS_PACKET_CMD_FORWARD_E:
            /* hwActionPtr[0] already 0 */
            break;
        case CPSS_PACKET_CMD_MIRROR_TO_CPU_E:
            hwActionPtr[0] |= 1;
            useCpuCode = GT_TRUE;
            break;
        case CPSS_PACKET_CMD_TRAP_TO_CPU_E:
            hwActionPtr[0] |= 2;
            useCpuCode = GT_TRUE;
            break;
        case CPSS_PACKET_CMD_DROP_HARD_E:
            hwActionPtr[0] |= 3;
            break;
        case CPSS_PACKET_CMD_DROP_SOFT_E:
            hwActionPtr[0] |= 4;
            break;
        default: return GT_BAD_PARAM;
    }

    if (cheetahVer >= 3)
    {
        if ((swActionPtr->actionStop != GT_FALSE) &&
            (swActionPtr->ipUcRoute.doIpUcRoute == GT_FALSE))
        {
            /* prevent the next lookups */
            hwActionPtr[3] |= (1 << 1);
        }
    }

    /* mirror bit 11 - to-analyzer-port, bits 10:3 CpuCode */

    if (swActionPtr->mirror.mirrorToRxAnalyzerPort == GT_TRUE)
    {
        hwActionPtr[0] |= (1 << 11);
    }

    dsaCpuCode = PRV_CPSS_DXCH_NET_DSA_TAG_REGULAR_FORWARD_E;

    if (useCpuCode != GT_FALSE)
    {
        rc = prvCpssDxChNetIfCpuToDsaCode(
            swActionPtr->mirror.cpuCode, &dsaCpuCode);
        if (rc != GT_OK)
        {
            return GT_BAD_PARAM;
        }
    }

    if(dsaCpuCode >= BIT_8)
        return GT_OUT_OF_RANGE;

    hwActionPtr[0] |= ((dsaCpuCode & 0xFF) << 3);

    /* matc counter bit 25 - enable, bits 30:26 - index */
    if (swActionPtr->matchCounter.enableMatchCount == GT_TRUE)
    {
        if (swActionPtr->egressPolicy == GT_FALSE)
        {
            hwActionPtr[0] |= (1 << 25);
            hwActionPtr[0] |=
                ((swActionPtr->matchCounter.matchCounterIndex & 0x1F) << 26);
        }
        else
        {
            if (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(
                devNum, PRV_CPSS_DXCH2_PCL_EGRESS_ACTION_CNTR_VLAN_WRONG_BITS_POS_WA_E))
            {
                hwActionPtr[0] |= (1 << 30);
                hwActionPtr[0] |=
                    ((swActionPtr->matchCounter.matchCounterIndex & 0x1F) << 25);
            }
            else
            {
                hwActionPtr[0] |= (1 << 25);
                hwActionPtr[0] |=
                    ((swActionPtr->matchCounter.matchCounterIndex & 0x1F) << 26);
            }
        }

        if (cheetahVer >= 3)
        {
            if(swActionPtr->matchCounter.matchCounterIndex >= BIT_14)
                return GT_OUT_OF_RANGE;

            /* counter index 5 */
            hwActionPtr[0] |=
                (((swActionPtr->matchCounter.matchCounterIndex >> 5) & 0x1) << 31);

            /* counter index 13:6 */
            hwActionPtr[3] |=
                (((swActionPtr->matchCounter.matchCounterIndex >> 6) & 0xFF) << 2);
        }
        else
        {
            if(swActionPtr->matchCounter.matchCounterIndex >= BIT_5)
                return GT_OUT_OF_RANGE;
        }
    }


    /* QOS */

    /* common for ingress and egress */

    /* modifyDscp bits 22:21 */
    switch (swActionPtr->qos.modifyDscp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            /* hwActionPtr[0] already 0 */
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            if (swActionPtr->egressPolicy == GT_TRUE)
            {
                /* this option is not supported for Egress Policy*/
                return GT_BAD_PARAM;
            }
            hwActionPtr[0] |= (2 << 21);
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            hwActionPtr[0] |= (1 << 21);
            break;
        default: return GT_BAD_PARAM;
    }

    /* modifyUp bits 24:23 */
    switch (swActionPtr->qos.modifyUp)
    {
        case CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E:
            /* hwActionPtr[0] already 0 */
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E:
            if (swActionPtr->egressPolicy == GT_TRUE)
            {
                /* this option is not supported for Egress Policy*/
                return GT_BAD_PARAM;
            }
            hwActionPtr[0] |= (2 << 23);
            break;
        case CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E:
            hwActionPtr[0] |= (1 << 23);
            break;
        default: return GT_BAD_PARAM;
    }

    if (swActionPtr->egressPolicy == GT_FALSE)
    {
        /* ingress */

        /* QOS profile bits 20:14 */
        if(swActionPtr->qos.qos.ingress.profileIndex >= BIT_7)
            return GT_OUT_OF_RANGE;
        hwActionPtr[0] |=
            ((swActionPtr->qos.qos.ingress.profileIndex & 0x7F) << 14);

        /* QOS profile marking enable bit 13 */
        if (swActionPtr->qos.qos.ingress.profileAssignIndex == GT_TRUE)
        {
            hwActionPtr[0] |= (1 << 13);
        }

        /* QOS precedence bit 12 */
        switch (swActionPtr->qos.qos.ingress.profilePrecedence)
        {
            case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
                /* hwActionPtr[0] already 0 */
                break;
            case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
                hwActionPtr[0] |= (1 << 12);
                break;
            default: return GT_BAD_PARAM;
        }
    }
    else
    {
        /* egress */
        if (cheetahVer == 1)
        {
            /* the Cheetah not supports Egress Policy*/
            return GT_BAD_PARAM;
        }

        /* UP bits 20:18 */
        if(swActionPtr->qos.qos.egress.up >= BIT_3)
            return GT_OUT_OF_RANGE;
        hwActionPtr[0] |= ((swActionPtr->qos.qos.egress.up & 0x07) << 18);

        /* DSCP bits 17:12 */
        if(swActionPtr->qos.qos.egress.dscp >= BIT_6)
            return GT_OUT_OF_RANGE;
        hwActionPtr[0] |= ((swActionPtr->qos.qos.egress.dscp & 0x3F) << 12);
    }

    /* redirect */

    /* command - word1, bits 1:0 */
    switch (swActionPtr->redirect.redirectCmd)
    {
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E:
            /* hwActionPtr[0] already 0 */
            break;

        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E:

            /* redirection command */
            hwActionPtr[1] |= 1;

            /* redirect interface */
            switch (swActionPtr->redirect.data.outIf.outInterface.type)
            {
                case CPSS_INTERFACE_PORT_E:
                    /* device */
                    if(swActionPtr->redirect.data.outIf.
                          outInterface.devPort.devNum >= BIT_5)
                        return GT_OUT_OF_RANGE;
                    hwActionPtr[1] |=
                        ((swActionPtr->redirect.data.outIf.
                          outInterface.devPort.devNum & 0x1F) << 9);
                    /* port */
                    if(swActionPtr->redirect.data.outIf.
                          outInterface.devPort.portNum >= BIT_6)
                        return GT_OUT_OF_RANGE;
                    hwActionPtr[1] |=
                        ((swActionPtr->redirect.data.outIf.
                          outInterface.devPort.portNum & 0x3F) << 3);
                    break;
                case CPSS_INTERFACE_TRUNK_E:
                    /* TRUNK IF */
                    hwActionPtr[1] |= (1 << 2);
                    /* trunk ID */
                    if(swActionPtr->redirect.data.outIf.
                          outInterface.trunkId >= BIT_7)
                        return GT_OUT_OF_RANGE;
                    hwActionPtr[1] |=
                        ((swActionPtr->redirect.data.outIf.
                          outInterface.trunkId & 0x7F) << 3);
                    break;
                case CPSS_INTERFACE_VIDX_E:
                    /* multicast IF */
                    hwActionPtr[1] |= (1 << 14);
                    /* VIDX */
                    if(swActionPtr->redirect.data.outIf.outInterface.vidx >= BIT_12)
                        return GT_OUT_OF_RANGE;
                    hwActionPtr[1] |=
                        ((swActionPtr->redirect.data.outIf.outInterface.vidx
                          & 0x0FFF) << 2);
                    break;
                case CPSS_INTERFACE_VID_E:
                    /* multicast IF */
                    hwActionPtr[1] |= (1 << 14);
                    /* VID - use constant 0xFFF for send to VLAN */
                    hwActionPtr[1] |= (0x0FFF << 2);
                    break;
                default: return GT_BAD_PARAM;
            }

            /* tunnel start */
            if (swActionPtr->redirect.data.outIf.tunnelStart == GT_TRUE)
            {
                if (cheetahVer == 1)
                {
                    /* the Cheetah not supports tunnel */
                    return GT_BAD_PARAM;
                }

                /* tunnel start */
                hwActionPtr[2] |= (1 << 10);

                /* tunnel PTR */
                hwActionPtr[2] |= ((swActionPtr->redirect.data.outIf.tunnelPtr
                                    & 0x03FF) << 11);

                if (cheetahVer >= 3)
                {
                    /* tunnel PTR bits 12:10 */
                    if(swActionPtr->redirect.data.outIf.tunnelPtr >= BIT_13)
                        return GT_OUT_OF_RANGE;
                    hwActionPtr[3] |=
                        (((swActionPtr->redirect.data.outIf.tunnelPtr
                                       >> 10) & 7) << 14);
                    /* tunnel type */
                    switch (swActionPtr->redirect.data.outIf.tunnelType)
                    {
                        case CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E:
                            /* the data already contains 0 */
                            break;
                        case CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E:
                            hwActionPtr[2] |= (1 << 21);
                            break;
                        default: return GT_BAD_PARAM;
                    }
                }
                else
                {
                    if(swActionPtr->redirect.data.outIf.tunnelPtr >= BIT_10)
                        return GT_OUT_OF_RANGE;
                }

            }

            /* vntL2Echo */
            if (swActionPtr->redirect.data.outIf.vntL2Echo == GT_TRUE)
            {
                if (cheetahVer == 1)
                {
                    /* the Cheetah not supports VNT */
                    return GT_BAD_PARAM;
                }

                /* vntL2Echo */
                hwActionPtr[2] |= (1 << 9);

            }

            break;
        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E:

            if (cheetahVer == 1)
            {
                /* the Cheetah not supports redirect to Next Hop */
                return GT_BAD_PARAM;
            }

            /* redirection command */
            hwActionPtr[1] |= 2;

            /* routerLttIndex */
            if (cheetahVer >=  3)
            {
                /* routerLttIndex bits 14:13 */
                if(swActionPtr->redirect.data.routerLttIndex >= BIT_15)
                    return GT_OUT_OF_RANGE;
            }
            else
            {
                if(swActionPtr->redirect.data.routerLttIndex >= BIT_13)
                    return GT_OUT_OF_RANGE;
            }

            /* FEr#2018: Limited number of Policy-based routes */
            if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E))
            {
                switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
                {
                /* DxCh3 devices support index 0,1,2,3…(max IP TCAM row) only */
                case CPSS_PP_FAMILY_CHEETAH3_E:
                    if (swActionPtr->redirect.data.routerLttIndex >= fineTuningPtr->tableSize.routerAndTunnelTermTcam)
                        return GT_NOT_SUPPORTED;
                    break;
                default:
                    /* Do nothing */
                    break;
                }
            }
            else
            {
                routerLttIndex = swActionPtr->redirect.data.routerLttIndex;

            }

            /* routerLttIndex */
            hwActionPtr[1] |= ((routerLttIndex & 0x1FFF) << 2);

            if (cheetahVer >=  3)
            {
                /* routerLttIndex bits 14:13 */
                hwActionPtr[3] |= (((routerLttIndex >> 13)& 3) << 24);
            }

            break;

        case CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E:
            if (cheetahVer < 3)
            {
                /* the CH and CH2 not support redirect to virtual router */
                return GT_BAD_PARAM;
            }

            /* redirection command - 4 */
            /* bits 33:32 assumed to be 0 */
            /* bit 119 is redirection command bit 2 */
            hwActionPtr[3] |= (1 << 23);

            /* routerLttIndex */
            if(swActionPtr->redirect.data.vrfId >= BIT_12)
                return GT_OUT_OF_RANGE;
            hwActionPtr[1] |=
                ((swActionPtr->redirect.data.vrfId & 0x0FFF) << 2);

            break;

        default: return GT_BAD_PARAM;
    }

    /* policer Ch-Ch3 ingress only */
    if ((swActionPtr->policer.policerEnable == GT_TRUE)  &&
       (swActionPtr->egressPolicy == GT_FALSE)   &&
       (swActionPtr->ipUcRoute.doIpUcRoute == GT_FALSE) )
    {
        /* policer enable */
        hwActionPtr[2] |= 1;

        /* policer index */
        hwActionPtr[2] |= ((swActionPtr->policer.policerId & 0xFF) << 1);

        if (cheetahVer >= 3)
        {
            /* policer index bits 11:8 */
            if(swActionPtr->policer.policerId >= BIT_12)
                return GT_OUT_OF_RANGE;
            hwActionPtr[3] |= (((swActionPtr->policer.policerId >> 8) & 0x0F) << 10);
        }
        else
        {
            if(swActionPtr->policer.policerId >= BIT_8)
                return GT_OUT_OF_RANGE;
        }

    }

    /* Egress Policer xCat and above devices     */
    /* Ingress action for xCat in other function */
    if ((swActionPtr->egressPolicy != GT_FALSE)
        && (cheetahVer >= 4))
    {

        if (swActionPtr->policer.policerId >= BIT_12)
        {
            return GT_OUT_OF_RANGE;
        }

        /* Policer Index[7..0] bits 72..65 */
        hwActionPtr[2] |= ((swActionPtr->policer.policerId & 0x00FF) << 1);

        /* Policer Index[11..8] bits 109..106 */
        hwActionPtr[3] |= ((swActionPtr->policer.policerId & 0x0F00) << 2);

        switch (swActionPtr->policer.policerEnable)
        {
            case CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E:
                /* bit 63  Bind To Policer Counter assumed 0 */
                /* bit 64  Bind To Policer Meter assumed 0   */
                break;
            case CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E:
                /* bit 63  Bind To Policer Counter */
                hwActionPtr[1] |= (1 << 31);
                /* bit 64  Bind To Policer Meter   */
                hwActionPtr[2] |= 1;
                break;
            case CPSS_DXCH_PCL_POLICER_ENABLE_METER_ONLY_E:
                /* bit 64  Bind To Policer Meter */
                hwActionPtr[2] |= 1;
                break;
            case CPSS_DXCH_PCL_POLICER_ENABLE_COUNTER_ONLY_E:
                /* bit 63  Bind To Policer Counter */
                hwActionPtr[1] |= (1 << 31);
                break;
            default: return GT_BAD_PARAM;
        }

        /* set bit 11 to be always 1 */
        hwActionPtr[0] |= (1 << 11);
    }

    /* vlan */
    if(swActionPtr->vlan.vlanId >= BIT_12)
        return GT_OUT_OF_RANGE;

    /* vlan command word1 bits 18:17 */
    if (swActionPtr->egressPolicy == GT_FALSE)
    {
        /* Ingress Policy */
        switch (swActionPtr->vlan.modifyVlan)
        {
            case CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E:
                /* hwActionPtr[1] already 0 */
                break;
            case CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E:
                hwActionPtr[1] |= (2 << 17);
                break;
            case CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E:
                hwActionPtr[1] |= (1 << 17);
                break;
            case CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E:
                hwActionPtr[1] |= (3 << 17);
                break;
            default: return GT_BAD_PARAM;
        }

        /* VLAN Id */
        hwActionPtr[1] |= ((swActionPtr->vlan.vlanId & 0x0FFF) << 19);
    }
    else
    {
        /* Egress Policy */
        if (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(
            devNum, PRV_CPSS_DXCH2_PCL_EGRESS_ACTION_CNTR_VLAN_WRONG_BITS_POS_WA_E))
        {
            switch (swActionPtr->vlan.modifyVlan)
            {
                case CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E:
                    /* hwActionPtr[1] already 0 */
                    break;
                case CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E:
                    hwActionPtr[1] |= (1 << 18);
                    break;
                default: return GT_BAD_PARAM;
            }

            /* VLAN Id */
            hwActionPtr[1] |= ((swActionPtr->vlan.vlanId & 0x07FF) << 20);
            hwActionPtr[2] |= ((swActionPtr->vlan.vlanId >> 11) & 0x01);
        }
        else
        {
            switch (swActionPtr->vlan.modifyVlan)
            {
                case CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E:
                    /* hwActionPtr[1] already 0 */
                    break;
                case CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E:
                    hwActionPtr[1] |= (1 << 17);
                    break;
                default: return GT_BAD_PARAM;
            }

            /* VLAN Id */
            hwActionPtr[1] |= ((swActionPtr->vlan.vlanId & 0x0FFF) << 19);
        }
    }

    /* nested VLAN */
    if (swActionPtr->vlan.nestedVlan == GT_TRUE)
    {
        hwActionPtr[1] |= (1 << 16);
    }


    /* VLAN command precedence */
    switch (swActionPtr->vlan.precedence)
    {
        case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E:
            /* hwActionPtr[0] already 0 */
            break;
        case CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E:
            hwActionPtr[1] |= (1 << 15);
            break;
        default: return GT_BAD_PARAM;
    }

    if ((cheetahVer >= 3)  &&
       (swActionPtr->egressPolicy == GT_FALSE)  &&
       (swActionPtr->ipUcRoute.doIpUcRoute == GT_FALSE))
    {
        /* MODIFY SOURCE ID */
        if (swActionPtr->sourceId.assignSourceId != GT_FALSE)
        {
            /* modify source id enable */
            hwActionPtr[3] |= (1 << 17);

            if(swActionPtr->sourceId.sourceIdValue >= BIT_5)
                return GT_OUT_OF_RANGE;
            hwActionPtr[3] |=
                ((swActionPtr->sourceId.sourceIdValue & 0x1F) << 18);
        }
    }

    /* IP Unicast routing for DxCh and xCat only (not for DxCh2 or DxCh3) */
    if (swActionPtr->ipUcRoute.doIpUcRoute == GT_TRUE)
    {
        if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH2_E) ||
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E))
        {
            /* The DxCh2 and DxCh3 devices not support this feature */
            return GT_BAD_PARAM;
        }

        /* check required parameters from other parts of the action */
        /* assumed they contain the needed relevant data            */

        if (swActionPtr->redirect.redirectCmd
            != CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
        {
            /* the redirect command should be Redirect to Out Interface */
            return GT_BAD_PARAM;
        }

        if (swActionPtr->policer.policerEnable == GT_TRUE)
        {
            /* Policer must be disabled */
            return GT_BAD_PARAM;
        }

        if ((swActionPtr->egressPolicy == GT_TRUE)
            || (swActionPtr->vlan.modifyVlan ==
                CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E)
            || (swActionPtr->vlan.nestedVlan == GT_TRUE))
        {
            /* VLAN modification should be for all types of packets
               Nested VLAN must be disabled  */
            return GT_BAD_PARAM;
        }

        /* update redirection command to be 3 - routing */
        hwActionPtr[1] &= (~ 3);
        hwActionPtr[1] |= 3;

        /* ARP DA index */
        /* 8 low bits */
        hwActionPtr[0] &= (~ (0x00FF << 3));
        hwActionPtr[0] |=
            (swActionPtr->ipUcRoute.arpDaIndex & 0x00FF) << 3;
        /* 2 high bits */
        hwActionPtr[1] &= (~ (0x03 << 17));
        hwActionPtr[1] |=
            ((swActionPtr->ipUcRoute.arpDaIndex >> 8) & 0x03) << 17;

        /* reset 15 - 16 bits of word 1 */
        hwActionPtr[1] &= (~ (0x03 << 15));

        /* reset word 2 */
        hwActionPtr[2] = 0;

        /* enable decrement TTL */
        if (swActionPtr->ipUcRoute.decrementTTL == GT_TRUE)
        {
            hwActionPtr[2] |= (1 << 3);
        }

        /* Enable TTL and Options check bypass. */
        if (swActionPtr->ipUcRoute.bypassTTLCheck == GT_TRUE)
        {
            hwActionPtr[2] |= (1 << 2);
        }

        /* ICMP Redirect Check Enable */
        if (swActionPtr->ipUcRoute.icmpRedirectCheck == GT_TRUE)
        {
            hwActionPtr[2] |= (1 << 1);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXCatPclTcamRuleIngressActionHw2SwConvert
*
* DESCRIPTION:
*       Converts TCAM Ingress Action from HW to SW format
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       hwActionPtr      - action in HW format (4 words)
* OUTPUTS:
*       swActionPtr      - action in SW format
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_BAD_STATE     - one of the values not supported.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXCatPclTcamRuleIngressActionHw2SwConvert
(
    IN  GT_U32                        *hwActionPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC      *swActionPtr
)
{
    GT_U32    hwValue; /* HW value */
    GT_STATUS rc;
    GT_U32    tempRouterLttPtr = 0;

    /* default settings - forward only */
    cpssOsMemSet(swActionPtr, 0, sizeof(CPSS_DXCH_PCL_ACTION_STC));

    /* pktCmd */
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(
        swActionPtr->pktCmd,(hwActionPtr[0] & 0x07));

    swActionPtr->egressPolicy = GT_FALSE;
    swActionPtr->ipUcRoute.doIpUcRoute = GT_FALSE;

    swActionPtr->actionStop        = BIT2BOOL_MAC(((hwActionPtr[2] >> 15) & 1));
    swActionPtr->bypassBridge      = BIT2BOOL_MAC(((hwActionPtr[2] >> 16) & 1));
    swActionPtr->bypassIngressPipe = BIT2BOOL_MAC(((hwActionPtr[2] >> 17) & 1));

    /* Lookup config */
    swActionPtr->lookupConfig.ipclConfigIndex =
        ((hwActionPtr[0] >> 31) & 1) | ((hwActionPtr[1] & 0x0FFF) << 1);

    swActionPtr->lookupConfig.pcl0_1OverrideConfigIndex =
        ((hwActionPtr[2] >> 19) & 1)
        ? CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E
        : CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;

    swActionPtr->lookupConfig.pcl1OverrideConfigIndex =
        ((hwActionPtr[2] >> 18) & 1)
        ? CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_OVERRIDE_E
        : CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;

    /* Mirror */
    swActionPtr->mirror.mirrorToRxAnalyzerPort =
        BIT2BOOL_MAC(((hwActionPtr[0] >> 11) & 1));

    swActionPtr->mirror.mirrorTcpRstAndFinPacketsToCpu =
        BIT2BOOL_MAC(((hwActionPtr[3] >> 17) & 1));

    /* CPU code */

    /* default */
    swActionPtr->mirror.cpuCode = CPSS_NET_CLASS_KEY_TRAP_E;

    if ((swActionPtr->pktCmd == CPSS_PACKET_CMD_TRAP_TO_CPU_E)
        || (swActionPtr->pktCmd == CPSS_PACKET_CMD_MIRROR_TO_CPU_E))
    {
        hwValue = ((hwActionPtr[0] >> 3) & 0x00FF);

        rc = prvCpssDxChNetIfDsaToCpuCode(
            (PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT)hwValue,
            &swActionPtr->mirror.cpuCode);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* Match counting in CNC */

    swActionPtr->matchCounter.enableMatchCount =
        BIT2BOOL_MAC(((hwActionPtr[1] >> 12) & 1));

    swActionPtr->matchCounter.matchCounterIndex =
        ((hwActionPtr[1] >> 13) & 0x3FFF);

    /* QOS */

    /* modifyDscp */
    hwValue = ((hwActionPtr[3] >> 13) & 3);
    PRV_CPSS_PCL_CONVERT_HW_VAL_TO_ATTR_MODIFY_MAC(
        swActionPtr->qos.modifyDscp, hwValue);

    /* modifyUp */
    hwValue = ((hwActionPtr[3] >> 15) & 3);
    PRV_CPSS_PCL_CONVERT_HW_VAL_TO_ATTR_MODIFY_MAC(
        swActionPtr->qos.modifyUp, hwValue);

    /* profileIndex */
    swActionPtr->qos.qos.ingress.profileIndex =
        ((hwActionPtr[3] >> 6) & 0x7F);

    /* profileAssignIndex */
    swActionPtr->qos.qos.ingress.profileAssignIndex =
        BIT2BOOL_MAC(((hwActionPtr[3] >> 4) & 1));

    /* profilePrecedence */
    hwValue = ((hwActionPtr[3] >> 5) & 1);
    PRV_CPSS_CONVERT_HW_VAL_TO_PRECEDENCE_MAC(
        swActionPtr->qos.qos.ingress.profilePrecedence, hwValue);

    /* redirect */

    switch ((hwActionPtr[0] >> 12) & 7)
    {
        case 0:
            swActionPtr->redirect.redirectCmd =
                CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E;
            break;
        case 1:
            swActionPtr->redirect.redirectCmd
                = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;

            /*bit 27 - IsVidx*/
            if ((hwActionPtr[0] >> 27) & 1)
            {
                swActionPtr->redirect.data.outIf.outInterface.vidx =
                    (GT_U16)((hwActionPtr[0] >> 15) & 0x0FFF);

                swActionPtr->redirect.data.outIf.outInterface.type =
                    (swActionPtr->redirect.data.outIf.outInterface.vidx == 0x0FFF)
                /* the packet's VID flooding */
                ? CPSS_INTERFACE_VID_E
                /* redirect to specified MA group */
                : CPSS_INTERFACE_VIDX_E;
            }
            /* bit 15 isTrunk*/
            else if ((hwActionPtr[0] >> 15) & 1)
            {
                swActionPtr->redirect.data.outIf.outInterface.type =
                    CPSS_INTERFACE_TRUNK_E;
                swActionPtr->redirect.data.outIf.outInterface.trunkId =
                    (GT_TRUNK_ID)((hwActionPtr[0] >> 16) & 0x007F);
            }
            /* port*/
            else
            {
                swActionPtr->redirect.data.outIf.outInterface.type =
                    CPSS_INTERFACE_PORT_E;
                swActionPtr->redirect.data.outIf.outInterface.devPort.devNum =
                    (GT_U8)((hwActionPtr[0] >> 22) & 0x001F);
                swActionPtr->redirect.data.outIf.outInterface.devPort.portNum =
                    (GT_U8)((hwActionPtr[0] >> 16) & 0x003F);
            }

            swActionPtr->redirect.data.outIf.tunnelStart =
                BIT2BOOL_MAC((hwActionPtr[0] >> 28) & 1);
            swActionPtr->redirect.data.outIf.tunnelPtr =
                ((hwActionPtr[0] >> 29) & 7) | ((hwActionPtr[1] & 0x03FF) << 3) ;
            swActionPtr->redirect.data.outIf.tunnelType =
                ((hwActionPtr[1] >> 10) & 1)
                ? CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E
                : CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E;
            swActionPtr->redirect.data.outIf.vntL2Echo =
                BIT2BOOL_MAC((hwActionPtr[1] >> 11) & 1);

            break;
        case 2:
            swActionPtr->redirect.redirectCmd =
                CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E;

            /* LTT entry is row based however in this field the LTT is treated as column based.
               Bits [11:0] indicate row while bits [14:13] indicate column, bit 12 is not used.
               The Formula for translating the LTT entry to column based is as follow:
               [11:0] << 2 + [14:13]   (Note: bit 12 not used). */
            tempRouterLttPtr = ((hwActionPtr[0] >> 15) & 0x7FFF);
            swActionPtr->redirect.data.routerLttIndex = (((tempRouterLttPtr & 0xFFF) << 2) |
                                                         ((tempRouterLttPtr & 0x6000) >> 13));
            break;
        case 4:
            swActionPtr->redirect.redirectCmd =
                CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E;

            swActionPtr->redirect.data.vrfId =
                ((hwActionPtr[0] >> 15) & 0x0FFF);
            break;
        default: return GT_BAD_STATE;
    }

    /* Policer */

    /* bind to meter (bit 59) and to counter (bit 60) */
    switch (((hwActionPtr[1] >> 26) & 2) | ((hwActionPtr[1] >> 28) & 1))
    {
        case 0:
            swActionPtr->policer.policerEnable =
                CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E;
            break;
        case 1:
            swActionPtr->policer.policerEnable =
                CPSS_DXCH_PCL_POLICER_ENABLE_COUNTER_ONLY_E;
            break;
        case 2:
            swActionPtr->policer.policerEnable =
                CPSS_DXCH_PCL_POLICER_ENABLE_METER_ONLY_E;
            break;
        case 3:
            swActionPtr->policer.policerEnable =
                CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E;
            break;
        /* cannot occur */
        default: return GT_BAD_STATE;
    }

    /* policerId */
    swActionPtr->policer.policerId =
        ((hwActionPtr[1] >> 29) & 7)
        | ((hwActionPtr[2] << 3) & 0x0FF8);

    /* VLAN */

    /* modifyVlan */
    hwValue = ((hwActionPtr[2] >> 22) & 3);
    PRV_CPSS_PCL_CONVERT_HW_VAL_TO_ATTR_MODIFY_MAC(
        swActionPtr->vlan.modifyVlan, hwValue);

    /* vlanId */
    swActionPtr->vlan.vlanId =
        (GT_U16)(((hwActionPtr[2] >> 24) & 0x00FF)
         | ((hwActionPtr[3] << 8) & 0x0F00));

    /* precedence */
    hwValue = ((hwActionPtr[2] >> 20) & 1);
    PRV_CPSS_CONVERT_HW_VAL_TO_PRECEDENCE_MAC(
        swActionPtr->vlan.precedence, hwValue);

    /* nestedVlan */
    swActionPtr->vlan.nestedVlan =
        BIT2BOOL_MAC((hwActionPtr[2] >> 21) & 1);

    /* Source Id */

    /* assignSourceId */
    swActionPtr->sourceId.assignSourceId =
        BIT2BOOL_MAC((hwActionPtr[2] >> 9) & 1);

    /* sourceIdValue */
    swActionPtr->sourceId.sourceIdValue =
        ((hwActionPtr[2] >> 10) & 0x1F);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXCatPclTcamRuleIpUcRoutingActionHw2SwConvert
*
* DESCRIPTION:
*       Converts TCAM Ip Unicast Routing Action from HW to SW format
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       hwActionPtr      - action in HW format (4 words)
* OUTPUTS:
*       swActionPtr      - action in SW format
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_BAD_STATE     - one of the values not supported.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXCatPclTcamRuleIpUcRoutingActionHw2SwConvert
(
    IN  GT_U32                        *hwActionPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC      *swActionPtr
)
{
    GT_U32    hwValue; /* HW value */

    /* default settings - forward only */
    cpssOsMemSet(swActionPtr, 0, sizeof(CPSS_DXCH_PCL_ACTION_STC));

    /* pktCmd */
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(
        swActionPtr->pktCmd,(hwActionPtr[0] & 0x07));

    swActionPtr->egressPolicy = GT_FALSE;
    swActionPtr->ipUcRoute.doIpUcRoute = GT_TRUE;

    swActionPtr->actionStop        = GT_FALSE;
    swActionPtr->bypassBridge      = GT_FALSE;
    swActionPtr->bypassIngressPipe = GT_FALSE;

    /* Lookup config */
    swActionPtr->lookupConfig.ipclConfigIndex = 0;

    swActionPtr->lookupConfig.pcl0_1OverrideConfigIndex =
        CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;

    swActionPtr->lookupConfig.pcl1OverrideConfigIndex =
        CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_RETAIN_E;

    /* Mirror */
    swActionPtr->mirror.mirrorToRxAnalyzerPort =
        BIT2BOOL_MAC(((hwActionPtr[0] >> 11) & 1));

    swActionPtr->mirror.mirrorTcpRstAndFinPacketsToCpu = GT_FALSE;

    /* CPU code */

    /* default */
    swActionPtr->mirror.cpuCode = CPSS_NET_CLASS_KEY_TRAP_E;

    /* Match counting in CNC */

    swActionPtr->matchCounter.enableMatchCount =
        BIT2BOOL_MAC(((hwActionPtr[0] >> 28) & 1));

    swActionPtr->matchCounter.matchCounterIndex =
        ((hwActionPtr[0] >> 29) & 0x07) | ((hwActionPtr[1] & 0x03) << 3);

    /* QOS */

    /* modifyDscp */
    hwValue = ((hwActionPtr[0] >> 24) & 3);
    PRV_CPSS_PCL_CONVERT_HW_VAL_TO_ATTR_MODIFY_MAC(
        swActionPtr->qos.modifyDscp, hwValue);

    /* modifyUp */
    hwValue = ((hwActionPtr[0] >> 26) & 3);
    PRV_CPSS_PCL_CONVERT_HW_VAL_TO_ATTR_MODIFY_MAC(
        swActionPtr->qos.modifyUp, hwValue);

    /* profileIndex */
    swActionPtr->qos.qos.ingress.profileIndex =
        ((hwActionPtr[0] >> 17) & 0x7F);

    /* profileAssignIndex */
    swActionPtr->qos.qos.ingress.profileAssignIndex =
        BIT2BOOL_MAC(((hwActionPtr[0] >> 16) & 1));

    /* profilePrecedence */
    hwValue = ((hwActionPtr[0] >> 15) & 1);
    PRV_CPSS_CONVERT_HW_VAL_TO_PRECEDENCE_MAC(
        swActionPtr->qos.qos.ingress.profilePrecedence, hwValue);

    /* redirect */

    /* Only to explain SW data format */
    swActionPtr->redirect.redirectCmd
        = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;


    /* bit 46 - IsVidx */
    if ((hwActionPtr[1] >> 14) & 1)
    {
        swActionPtr->redirect.data.outIf.outInterface.vidx =
            (GT_U16)((hwActionPtr[1] >> 2) & 0x0FFF);

        swActionPtr->redirect.data.outIf.outInterface.type =
            (swActionPtr->redirect.data.outIf.outInterface.vidx == 0x0FFF)
        /* the packet's VID flooding */
        ? CPSS_INTERFACE_VID_E
        /* redirect to specified MA group */
        : CPSS_INTERFACE_VIDX_E;
    }
    /* bit 34 isTrunk*/
    else if ((hwActionPtr[1] >> 2) & 1)
    {
        swActionPtr->redirect.data.outIf.outInterface.type =
            CPSS_INTERFACE_TRUNK_E;
        swActionPtr->redirect.data.outIf.outInterface.trunkId =
            (GT_TRUNK_ID)((hwActionPtr[1] >> 3) & 0x007F);
    }
    /* port*/
    else
    {
        swActionPtr->redirect.data.outIf.outInterface.type =
            CPSS_INTERFACE_PORT_E;
        swActionPtr->redirect.data.outIf.outInterface.devPort.devNum =
            (GT_U8)((hwActionPtr[1] >> 9) & 0x001F);
        swActionPtr->redirect.data.outIf.outInterface.devPort.portNum =
            (GT_U8)((hwActionPtr[1] >> 3) & 0x003F);
    }

    /* not relevant data */
    swActionPtr->redirect.data.outIf.tunnelStart = GT_FALSE;
    swActionPtr->redirect.data.outIf.tunnelPtr = 0;
    swActionPtr->redirect.data.outIf.tunnelType =
        CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E;
    swActionPtr->redirect.data.outIf.vntL2Echo = GT_FALSE;

    /* Policer */

    swActionPtr->policer.policerEnable =
        CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E;

    /* policerId */
    swActionPtr->policer.policerId = 0;

    /* VLAN */

    /* modifyVlan */
    swActionPtr->vlan.modifyVlan =
        CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;

    /* vlanId */
    swActionPtr->vlan.vlanId =
        (GT_U16)((hwActionPtr[1] >> 19) & 0x0FFF);

    /* precedence */
    swActionPtr->vlan.precedence =
        CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;

    /* nestedVlan */
    swActionPtr->vlan.nestedVlan = GT_FALSE;

    /* Source Id */

    /* assignSourceId */
    swActionPtr->sourceId.assignSourceId = GT_FALSE;

    /* sourceIdValue */
    swActionPtr->sourceId.sourceIdValue = 0;

    /* Ip Uc specific */

    swActionPtr->ipUcRoute.arpDaIndex =
        (((hwActionPtr[0] >> 3) & 0x00FF)
        | ((hwActionPtr[1] >> 9) & 0x0300));

    swActionPtr->ipUcRoute.icmpRedirectCheck =
        BIT2BOOL_MAC(((hwActionPtr[2] >> 1) & 1));

    swActionPtr->ipUcRoute.bypassTTLCheck =
        BIT2BOOL_MAC(((hwActionPtr[2] >> 2) & 1));

    swActionPtr->ipUcRoute.decrementTTL =
        BIT2BOOL_MAC(((hwActionPtr[2] >> 3) & 1));

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamRuleActionHw2SwConvert
*
* DESCRIPTION:
*       Converts TCAM Action from HW to SW format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       direction        - Policy direction:
*                          Ingress or Egress
*                          Needed for parsing
*       hwActionPtr      - action in HW format (4 words)
* OUTPUTS:
*       swActionPtr      - action in SW format
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_BAD_STATE     - one of the values not supported.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleActionHw2SwConvert
(
    IN  GT_U8                         devNum,
    IN  CPSS_PCL_DIRECTION_ENT        direction,
    IN  GT_U32                        *hwActionPtr,
    OUT CPSS_DXCH_PCL_ACTION_STC      *swActionPtr
)
{
    GT_U16 usedVidx;         /* used Vidx                           */
    GT_STATUS rc;            /* return code                         */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT   dsaCpuCode; /* cpuCode ih HW format */
    GT_U32    cheetahVer;    /* version of Cheetah                  */
    GT_U32    redirCmd;      /* redirect command                    */

    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
            cheetahVer = 1;
            break;
        case CPSS_PP_FAMILY_CHEETAH2_E:
            cheetahVer = 2;
            break;
        case CPSS_PP_FAMILY_CHEETAH3_E:
            cheetahVer = 3;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
        case CPSS_PP_FAMILY_DXCH_LION_E:
        default:
            if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
            {
                if  (((hwActionPtr[0] >> 12) & 3) != 3)
                {
                    /* xCat and above INGRESS action         */
                    /* but not special "Route to Next Hop action */
                    return prvCpssDxChXCatPclTcamRuleIngressActionHw2SwConvert(
                        hwActionPtr, swActionPtr);
                }
                else
                {
                    /* xCat and above Route to Next Hop action */
                    return prvCpssDxChXCatPclTcamRuleIpUcRoutingActionHw2SwConvert(
                        hwActionPtr, swActionPtr);
                }
            }

            /* EGRESS action is the same as for DxCh3        */
            cheetahVer = 4;
            break;
    }

    /* default settings - forward only */
    cpssOsMemSet(swActionPtr, 0, sizeof(CPSS_DXCH_PCL_ACTION_STC));

    /* pktCmd */
    PRV_CPSS_CONVERT_HW_VAL_TO_PACKET_CMD_MAC(
        swActionPtr->pktCmd,(hwActionPtr[0] & 0x07));

    swActionPtr->egressPolicy =
        (direction == CPSS_PCL_DIRECTION_EGRESS_E) ? GT_TRUE : GT_FALSE;

    swActionPtr->policer.policerEnable = GT_FALSE;
    swActionPtr->ipUcRoute.doIpUcRoute = GT_FALSE;

    if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        /* determinate type of action: regular or 98DX2X3 IP UC routing */
        swActionPtr->ipUcRoute.doIpUcRoute =
            ((hwActionPtr[1] & 3) == 3) ? GT_TRUE : GT_FALSE;

        /* Parsing of multiplexed fields */
        if (swActionPtr->ipUcRoute.doIpUcRoute == GT_FALSE)
        {
            /* policer */
            swActionPtr->policer.policerEnable =
                ((hwActionPtr[2] & 1) != 0) ? GT_TRUE : GT_FALSE;

            swActionPtr->policer.policerId = (hwActionPtr[2] >> 1) & 0x00FF;

            if (cheetahVer >= 3)
            {
                /* policer bits 11:8 */
                swActionPtr->policer.policerId |= (((hwActionPtr[3] >> 10) & 0x0F) << 8);
            }

            /* CPU Code */
            dsaCpuCode = (GT_U8)((hwActionPtr[0] >> 3) & 0xFF);

            rc = prvCpssDxChNetIfDsaToCpuCode(
                dsaCpuCode, &swActionPtr->mirror.cpuCode);

            if (rc != GT_OK)
            {
                /* default */
                swActionPtr->mirror.cpuCode = CPSS_NET_CLASS_KEY_TRAP_E;
            }
        }
        else
        {
            /* policer not used for IP UC Router entries */
            swActionPtr->policer.policerEnable = GT_FALSE;

            /* ARP DA Index */
            swActionPtr->ipUcRoute.arpDaIndex =
                ((hwActionPtr[0] >> 3) & 0x00FF)
                | (((hwActionPtr[1] >> 17) & 3) << 8);

            /* bypass TTL */
            swActionPtr->ipUcRoute.bypassTTLCheck =
                (((hwActionPtr[2] >> 2) & 1) != 0) ? GT_TRUE : GT_FALSE;

            /* decrement TTL*/
            swActionPtr->ipUcRoute.decrementTTL =
                (((hwActionPtr[2] >> 3) & 1) != 0) ? GT_TRUE : GT_FALSE;

            /* ICMP check */
            swActionPtr->ipUcRoute.icmpRedirectCheck =
                (((hwActionPtr[2] >> 1) & 1) != 0) ? GT_TRUE : GT_FALSE;

        }

        swActionPtr->actionStop = GT_FALSE;
        if (cheetahVer >= 3)
        {
            /* bit 97 - action stop enable */
            if (hwActionPtr[3] & 2)
            {
                swActionPtr->actionStop = GT_TRUE;
            }
        }

        swActionPtr->sourceId.assignSourceId = GT_FALSE;
        if (cheetahVer >= 3)
        {
            /* assign source id enable: bit 113 */
            if ((hwActionPtr[3] >> 17) & 1)
            {
                swActionPtr->sourceId.assignSourceId = GT_TRUE;
                swActionPtr->sourceId.sourceIdValue =
                    ((hwActionPtr[3] >> 18) & 0x1F);
            }
        }

        /* mirror */
        swActionPtr->mirror.mirrorToRxAnalyzerPort =
            (((hwActionPtr[0] >> 11) & 1) != 0) ? GT_TRUE : GT_FALSE;
    }

    /* matchCounter */

    if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        swActionPtr->matchCounter.matchCounterIndex =
            (hwActionPtr[0] >> 26) & 0x1F;
        swActionPtr->matchCounter.enableMatchCount =
            (((hwActionPtr[0] >> 25) & 1) != 0) ? GT_TRUE : GT_FALSE;
    }
    else
    {
        if (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(
            devNum, PRV_CPSS_DXCH2_PCL_EGRESS_ACTION_CNTR_VLAN_WRONG_BITS_POS_WA_E))
        {
            swActionPtr->matchCounter.matchCounterIndex =
                (hwActionPtr[0] >> 25) & 0x1F;
            swActionPtr->matchCounter.enableMatchCount =
                (((hwActionPtr[0] >> 30) & 1) != 0) ? GT_TRUE : GT_FALSE;
        }
        else
        {
            swActionPtr->matchCounter.matchCounterIndex =
                (hwActionPtr[0] >> 26) & 0x1F;
            swActionPtr->matchCounter.enableMatchCount =
                (((hwActionPtr[0] >> 25) & 1) != 0) ? GT_TRUE : GT_FALSE;
        }
    }

    if (cheetahVer >= 3)
    {
        /* counter index's bit 5 - 31 */
        swActionPtr->matchCounter.matchCounterIndex |=
            (((hwActionPtr[0] >> 31) & 0x1) << 5);
        /* counter index's bits 13:6 - 105:98 */
        swActionPtr->matchCounter.matchCounterIndex |=
            (((hwActionPtr[3] >> 2) & 0x00FF) << 6);
    }

    /* vlan */

    if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        switch ((hwActionPtr[1] >> 17) & 0x03)
        {
            default: /* reserved values*/
            case 0:
                swActionPtr->vlan.modifyVlan =
                    CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
                break;
            case 1:
                swActionPtr->vlan.modifyVlan =
                    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E;
                break;
            case 2:
                swActionPtr->vlan.modifyVlan =
                    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
                break;
            case 3:
                swActionPtr->vlan.modifyVlan =
                    CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
                break;
        }

        swActionPtr->vlan.vlanId = (GT_U16)((hwActionPtr[1] >> 19) & 0x0FFF);
    }
    else
    {
        if (GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(
            devNum, PRV_CPSS_DXCH2_PCL_EGRESS_ACTION_CNTR_VLAN_WRONG_BITS_POS_WA_E))
        {
            switch ((hwActionPtr[1] >> 18) & 0x03)
            {
                default: /* reserved values*/
                case 0:
                    swActionPtr->vlan.modifyVlan =
                        CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
                    break;
                case 1:
                case 2:
                    swActionPtr->vlan.modifyVlan =
                        CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
                    break;
                case 3:
                    swActionPtr->vlan.modifyVlan =
                        CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
                    break;
            }

            swActionPtr->vlan.vlanId = (GT_U16)((hwActionPtr[1] >> 20) & 0x07FF);
            swActionPtr->vlan.vlanId |= (GT_U16)((hwActionPtr[2] & 1) << 11);
        }
        else
        {
            switch ((hwActionPtr[1] >> 17) & 0x03)
            {
                default: /* reserved values*/
                case 0:
                    swActionPtr->vlan.modifyVlan =
                        CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E;
                    break;
                case 1:
                case 2:
                    swActionPtr->vlan.modifyVlan =
                        CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
                    break;
                case 3:
                    swActionPtr->vlan.modifyVlan =
                        CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
                    break;
            }

            swActionPtr->vlan.vlanId = (GT_U16)((hwActionPtr[1] >> 19) & 0x0FFF);
        }
    }

    swActionPtr->vlan.nestedVlan =
        (((hwActionPtr[1] >> 16) & 1) != 0) ? GT_TRUE : GT_FALSE;

    swActionPtr->vlan.precedence =
        (((hwActionPtr[1] >> 15) & 1) != 0)
        ? CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E
        : CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;

    /* QOS */

    /* Modify DSCP */
    PRV_CPSS_PCL_CONVERT_HW_VAL_TO_ATTR_MODIFY_MAC(
        swActionPtr->qos.modifyDscp, ((hwActionPtr[0] >> 21) & 3));

    /* Modify UP */
    PRV_CPSS_PCL_CONVERT_HW_VAL_TO_ATTR_MODIFY_MAC(
        swActionPtr->qos.modifyUp, ((hwActionPtr[0] >> 23) & 3));

    if (direction == CPSS_PCL_DIRECTION_EGRESS_E)
    {
        /* egress */
        swActionPtr->qos.qos.egress.dscp =
            (GT_U8)((hwActionPtr[0] >> 12) & 0x3F);
        swActionPtr->qos.qos.egress.up   =
            (GT_U8)((hwActionPtr[0] >> 18) & 0x07);
    }
    else
    {
        /* ingress */
        swActionPtr->qos.qos.ingress.profileAssignIndex =
            (((hwActionPtr[0] >> 13) & 1) != 0) ? GT_TRUE : GT_FALSE;

        swActionPtr->qos.qos.ingress.profileIndex =
            (hwActionPtr[0] >> 14) & 0x7F;

        swActionPtr->qos.qos.ingress.profilePrecedence =
            (((hwActionPtr[0] >> 12) & 1) != 0)
            ? CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E
            : CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
    }

    /* REDIRECT */

    if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
    {
        redirCmd = hwActionPtr[1] & 3;
        if (cheetahVer >= 3)
        {
            /* redirCmd bit 2 - bit 119 */
            redirCmd |= (((hwActionPtr[3] >> 23) & 1) << 2);
        }

        switch (redirCmd)
        {
            default: /* must never occur      */
            case 0:  /* no redirection        */
                swActionPtr->redirect.redirectCmd =
                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_NONE_E;
                break;

            case 3:  /* 98DX2X3 IP UC routing */
            case 1:  /* redirect to interface */
                swActionPtr->redirect.redirectCmd =
                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;

                if (((hwActionPtr[1] >> 14) & 1) != 0)
                {
                    /* use vidx */
                    usedVidx = (GT_U16)((hwActionPtr[1] >> 2) & 0x0FFF);

                    /* the constant 0xFFF used for send packet to VLAN */
                    if (usedVidx != 0xFFF)
                    {
                        /* vidx */
                        swActionPtr->redirect.data.outIf.outInterface.type =
                            CPSS_INTERFACE_VIDX_E;

                        swActionPtr->redirect.data.outIf.outInterface.vidx =
                            usedVidx;
                    }
                    else
                    {
                        /* vid */
                        swActionPtr->redirect.data.outIf.outInterface.type =
                            CPSS_INTERFACE_VID_E;

                        swActionPtr->redirect.data.outIf.outInterface.vlanId =
                            usedVidx;
                    }
                }
                else
                {
                    /* port or trunk */
                    if (((hwActionPtr[1] >> 2) & 1) != 0)
                    {
                        /* trunk */
                        swActionPtr->redirect.data.outIf.outInterface.type =
                            CPSS_INTERFACE_TRUNK_E;

                        swActionPtr->redirect.data.outIf.outInterface.trunkId =
                            (GT_U16)((hwActionPtr[1] >> 3) & 0x07F);
                    }
                    else
                    {
                        /* port */
                        swActionPtr->redirect.data.outIf.outInterface.type =
                            CPSS_INTERFACE_PORT_E;

                        swActionPtr->redirect.data.outIf.outInterface.
                            devPort.devNum = (GT_U8)((hwActionPtr[1] >> 9) & 0x1F);

                        swActionPtr->redirect.data.outIf.outInterface.
                            devPort.portNum = (GT_U8)((hwActionPtr[1] >> 3) & 0x3F);
                    }
                }

                /* tunnel and VNT support for DxCh2 and above devices */
                if (cheetahVer >= 2)
                {
                    swActionPtr->redirect.data.outIf.tunnelPtr =
                        (hwActionPtr[2] >> 11) & 0x03FF;
                    if (cheetahVer >= 3)
                    {
                        /* tunnelPtr bits 12:10 - bits 112:110 */
                        swActionPtr->redirect.data.outIf.tunnelPtr |=
                            (((hwActionPtr[3] >> 14) & 7) << 10);
                    }

                    swActionPtr->redirect.data.outIf.tunnelStart =
                        (((hwActionPtr[2] >> 10) & 1) != 0) ? GT_TRUE : GT_FALSE;

                    swActionPtr->redirect.data.outIf.vntL2Echo =
                        (((hwActionPtr[2] >> 9) & 1) != 0) ? GT_TRUE : GT_FALSE;

                    if (cheetahVer >= 3)
                    {
                        /* bit 85 : tunnel type */
                        swActionPtr->redirect.data.outIf.tunnelType =
                            ((hwActionPtr[2] >> 21) == 0)
                            ? CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_ETHERNET_E
                            : CPSS_DXCH_PCL_ACTION_REDIRECT_TUNNEL_TYPE_IP_E;
                    }
                }

                break;


            case 2:  /* redirect to IP router */
                swActionPtr->redirect.redirectCmd =
                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E;
                swActionPtr->redirect.data.routerLttIndex =
                    (hwActionPtr[1] >> 2) & 0x1FFF;
                if (cheetahVer >= 3)
                {
                    /* bits 121:120 - bits 14:13 of LttIndex */
                    swActionPtr->redirect.data.routerLttIndex |=
                        (((hwActionPtr[3] >> 24) & 3) << 13);
                }

                break;

            case 4: /* virtual router */
                swActionPtr->redirect.redirectCmd =
                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E;
                swActionPtr->redirect.data.vrfId =
                    (hwActionPtr[1] >> 2) & 0x0FFF;

                break;
        }
    }

    /* Egress Action Policer xCat and above devices */
    if ((direction == CPSS_PCL_DIRECTION_EGRESS_E)
        && (cheetahVer >= 4))
    {
        /* bind to meter and to counter */
        switch (((hwActionPtr[2] & 1) << 1) | ((hwActionPtr[1] >> 31) & 1))
        {
            case 0:
                swActionPtr->policer.policerEnable =
                    CPSS_DXCH_PCL_POLICER_DISABLE_ALL_E;
                break;
            case 1:
                swActionPtr->policer.policerEnable =
                    CPSS_DXCH_PCL_POLICER_ENABLE_COUNTER_ONLY_E;
                break;
            case 2:
                swActionPtr->policer.policerEnable =
                    CPSS_DXCH_PCL_POLICER_ENABLE_METER_ONLY_E;
                break;
            case 3:
                swActionPtr->policer.policerEnable =
                    CPSS_DXCH_PCL_POLICER_ENABLE_METER_AND_COUNTER_E;
                break;
            /* cannot occur */
            default: return GT_BAD_STATE;
        }

        swActionPtr->policer.policerId =
            ((hwActionPtr[2] >> 1) & 0x00FF)
            | ((hwActionPtr[3] >> 2) & 0x0F00);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXCatIngressPclTcamRuleDataSw2HwConvert
*
* DESCRIPTION:
*       Converts XCat TCAM pattern or mask from SW to HW Ingress ket format
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       ruleFormat     - one of supported ingress key formats
*       swDataPtr      - rule pattern or mask in SW format
* OUTPUTS:
*       hwDataPtr      - rule pattern or mask in HW format (6 or 12 words)
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_OUT_OF_RANGE  - one of the parameters is out of range.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXCatIngressPclTcamRuleDataSw2HwConvert
(
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_UNT      *swDataPtr,
    OUT GT_U32                             *hwDataPtr
)
{
    GT_U32  size;   /* size of heData in words    */
    GT_U32  i;      /* loop index                 */

    /* format depended pointers into swDataPtr */
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC                 *commonPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC             *commonExtPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC          *commonStdIpPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_COMMON_STC     *commonUdbPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC             *stdNotIpPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC          *stdIpL2QosPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC            *stdIpL4Ptr;
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STANDARD_UDB_STC   *stdUdbPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC           *extNotIpv6Ptr;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC            *extIpv6L2Ptr;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC            *extIpv6L4Ptr;
    CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC   *extUdbPtr;

     /* clear all format depended pointers    */
    /* only relevant will get not null value */
    commonPtr         = NULL;
    commonExtPtr      = NULL;
    commonStdIpPtr    = NULL;
    commonUdbPtr      = NULL;
    stdNotIpPtr       = NULL;
    stdIpL2QosPtr     = NULL;
    stdIpL4Ptr        = NULL;
    stdUdbPtr         = NULL;
    extNotIpv6Ptr     = NULL;
    extIpv6L2Ptr      = NULL;
    extIpv6L4Ptr      = NULL;
    extUdbPtr         = NULL;

    switch (ruleFormat)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            stdNotIpPtr    = &(swDataPtr->ruleStdNotIp);
            commonPtr      = &(stdNotIpPtr->common);
            size           = 6;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            stdIpL2QosPtr  = &(swDataPtr->ruleStdIpL2Qos);
            commonPtr      = &(stdIpL2QosPtr->common);
            commonStdIpPtr = &(stdIpL2QosPtr->commonStdIp);
            size           = 6;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            stdIpL4Ptr     = &(swDataPtr->ruleStdIpv4L4);
            commonPtr      = &(stdIpL4Ptr->common);
            commonStdIpPtr = &(stdIpL4Ptr->commonStdIp);
            size           = 6;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
            stdUdbPtr      = &(swDataPtr->ruleIngrStdUdb);
            commonUdbPtr   = &(stdUdbPtr->commonIngrUdb);
            size           = 6;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            extNotIpv6Ptr  = &(swDataPtr->ruleExtNotIpv6);
            commonPtr      = &(extNotIpv6Ptr->common);
            commonExtPtr   = &(extNotIpv6Ptr->commonExt);
            size           = 12;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            extIpv6L2Ptr   = &(swDataPtr->ruleExtIpv6L2);
            commonPtr      = &(extIpv6L2Ptr->common);
            commonExtPtr   = &(extIpv6L2Ptr->commonExt);
            size           = 12;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            extIpv6L4Ptr   = &(swDataPtr->ruleExtIpv6L4);
            commonPtr      = &(extIpv6L4Ptr->common);
            commonExtPtr   = &(extIpv6L4Ptr->commonExt);
            size           = 12;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
            extUdbPtr      = &(swDataPtr->ruleIngrExtUdb);
            commonUdbPtr   = &(extUdbPtr->commonIngrUdb);
            size           = 12;
            break;
        default: return GT_BAD_PARAM;
    }

    /* clear the hwData before passing data from SW */
    for (i = 0; (i < size); i++)
    {
        hwDataPtr[i] = 0;
    }

    /* common for ingress formats */
    if (commonPtr      != NULL)
    {
        /* PCL Id*/
        hwDataPtr[0] |= (commonPtr->pclId & 0x03FF);

        /* macToMe */
        hwDataPtr[0] |= ((commonPtr->macToMe & 1) << 10);

        /* source port */
        hwDataPtr[0] |= ((commonPtr->sourcePort & 0x3F) << 11);

        /* isTagged */
        hwDataPtr[0] |= ((commonPtr->isTagged & 0x01) << 17);

        /* VID */
        hwDataPtr[0] |= ((commonPtr->vid & 0x0FFF) << 18);

        /* UP */
        hwDataPtr[0] |= ((commonPtr->up & 0x03) << 30);
        hwDataPtr[1] |= ((commonPtr->up & 0x04) >> 2);

        /* Qos Profile */
        hwDataPtr[1] |= ((commonPtr->qosProfile & 0x7F) << 1);

        /* is IP */
        hwDataPtr[1] |= ((commonPtr->isIp & 0x01) << 9);

        /* is L2 valid */
        hwDataPtr[3] |= ((commonPtr->isL2Valid & 0x01) << 11);

        /* is UDB Valid */
        hwDataPtr[3] |= ((commonPtr->isUdbValid & 0x01) << 10);

    }

    /* common for all extended ingress formats */
    if (commonExtPtr   != NULL)
    {
        /* is IPV6 */
        hwDataPtr[1] |= ((commonExtPtr->isIpv6 & 0x01) << 8);

        /* ip Protocol */
        hwDataPtr[1] |= (commonExtPtr->ipProtocol << 16);

        /* DSCP */
        hwDataPtr[1] |= ((commonExtPtr->dscp & 0x3F) << 10);

        /* isL4Valid */
        hwDataPtr[3] |= ((commonExtPtr->isL4Valid & 0x01) << 8);

        /* l4Byte0 */
        hwDataPtr[2] |= (commonExtPtr->l4Byte0 << 16);

        /* l4Byte1 */
        hwDataPtr[2] |= (commonExtPtr->l4Byte1 << 24);

        /* l4Byte2 */
        hwDataPtr[2] |= (commonExtPtr->l4Byte2 << 8);

        /* l4Byte3 */
        hwDataPtr[2] |= commonExtPtr->l4Byte3;

        /* l4Byte13 */
        hwDataPtr[3] |= commonExtPtr->l4Byte13;

        /* ipHeaderOk */
        hwDataPtr[3] |= ((commonExtPtr->ipHeaderOk & 0x01) << 13);
    }

    if (commonStdIpPtr != NULL)
    {
        /* is IPV4 */
        hwDataPtr[1] |= ((commonStdIpPtr->isIpv4 & 0x01) << 8);

        /* ip Protocol */
        hwDataPtr[1] |= (commonStdIpPtr->ipProtocol << 16);

        /* DSCP */
        hwDataPtr[1] |= ((commonStdIpPtr->dscp & 0x3F) << 10);

        /* isL4Valid */
        hwDataPtr[3] |= ((commonStdIpPtr->isL4Valid & 0x01) << 8);

        /* l4Byte2 */
        hwDataPtr[2] |= (commonStdIpPtr->l4Byte2 << 8);

        /* l4Byte3 */
        hwDataPtr[2] |= commonStdIpPtr->l4Byte3;

        /* ipHeaderOk */
        hwDataPtr[3] |= ((commonStdIpPtr->ipHeaderOk & 0x01) << 13);

        /* ipv4Fragmented */
        hwDataPtr[3] |= ((commonStdIpPtr->ipv4Fragmented & 0x01) << 12);
    }

    /* standard ingress not IP */
    if (stdNotIpPtr    != NULL)
    {
        /* is IPV4 */
        hwDataPtr[1] |= ((stdNotIpPtr->isIpv4 & 0x01) << 8);

        /* etherType */
        hwDataPtr[1] |= (stdNotIpPtr->etherType << 10);

        /* isArp */
        hwDataPtr[3] |= ((stdNotIpPtr->isArp & 0x01) << 9);

        /* l2Encap */
        hwDataPtr[3] |= ((stdNotIpPtr->l2Encap & 0x01) << 12);

        /* MAC DA */
        hwDataPtr[4] |= (stdNotIpPtr->macDa.arEther[5] << 16);
        hwDataPtr[4] |= (stdNotIpPtr->macDa.arEther[4] << 24);
        hwDataPtr[5] |=  stdNotIpPtr->macDa.arEther[3];
        hwDataPtr[5] |= (stdNotIpPtr->macDa.arEther[2] << 8);
        hwDataPtr[5] |= (stdNotIpPtr->macDa.arEther[1] << 16);
        hwDataPtr[5] |= (stdNotIpPtr->macDa.arEther[0] << 24);

        /* MAC SA */
        hwDataPtr[2] |= (stdNotIpPtr->macSa.arEther[5] << 16);
        hwDataPtr[2] |= (stdNotIpPtr->macSa.arEther[4] << 24);
        hwDataPtr[3] |=  stdNotIpPtr->macSa.arEther[3];
        hwDataPtr[3] |= (stdNotIpPtr->macSa.arEther[2] << 24);
        hwDataPtr[4] |=  stdNotIpPtr->macSa.arEther[1];
        hwDataPtr[4] |= (stdNotIpPtr->macSa.arEther[0] << 8);

        /* UDB15 */
        hwDataPtr[2] |=  stdNotIpPtr->udb[0];
        /* UDB16 */
        hwDataPtr[2] |= (stdNotIpPtr->udb[1] << 8);
        /* UDB17 */
        hwDataPtr[3] |= (stdNotIpPtr->udb[2] << 16);
    }

    /* standard ingress IPV4 and IPV6 L2 Qos */
    if (stdIpL2QosPtr  != NULL)
    {
        /* isArp */
        hwDataPtr[3] |= ((stdIpL2QosPtr->isArp & 0x01) << 9);

        /* isIpv6ExtHdrExist */
        hwDataPtr[3] |= ((stdIpL2QosPtr->isIpv6ExtHdrExist & 0x01) << 14);

        /* isIpv6HopByHop */
        hwDataPtr[3] |= ((stdIpL2QosPtr->isIpv6HopByHop & 0x01) << 15);

        /* MAC DA */
        hwDataPtr[4] |= (stdIpL2QosPtr->macDa.arEther[5] << 16);
        hwDataPtr[4] |= (stdIpL2QosPtr->macDa.arEther[4] << 24);
        hwDataPtr[5] |=  stdIpL2QosPtr->macDa.arEther[3];
        hwDataPtr[5] |= (stdIpL2QosPtr->macDa.arEther[2] << 8);
        hwDataPtr[5] |= (stdIpL2QosPtr->macDa.arEther[1] << 16);
        hwDataPtr[5] |= (stdIpL2QosPtr->macDa.arEther[0] << 24);

        /* MAC SA */
        hwDataPtr[2] |= (stdIpL2QosPtr->macSa.arEther[5] << 16);
        hwDataPtr[2] |= (stdIpL2QosPtr->macSa.arEther[4] << 24);
        hwDataPtr[3] |=  stdIpL2QosPtr->macSa.arEther[3];
        hwDataPtr[3] |= (stdIpL2QosPtr->macSa.arEther[2] << 24);
        hwDataPtr[4] |=  stdIpL2QosPtr->macSa.arEther[1];
        hwDataPtr[4] |= (stdIpL2QosPtr->macSa.arEther[0] << 8);

        /* UDB18 */
        hwDataPtr[1] |= (stdIpL2QosPtr->udb[0] << 24);
        /* UDB19 */
        hwDataPtr[3] |= (stdIpL2QosPtr->udb[1] << 16);
    }

    /* standard ingress IPV4 L4 */
    if (stdIpL4Ptr     != NULL)
    {
        /* isArp */
        hwDataPtr[3] |= ((stdIpL4Ptr->isArp & 0x01) << 9);

        /* isBc */
        hwDataPtr[3] |= ((stdIpL4Ptr->isBc & 0x01) << 14);

        /* l4Byte0 */
        hwDataPtr[2] |= (stdIpL4Ptr->l4Byte0 << 16);

        /* l4Byte1 */
        hwDataPtr[2] |= (stdIpL4Ptr->l4Byte1 << 24);

        /* l4Byte13 */
        hwDataPtr[3] |=  stdIpL4Ptr->l4Byte13;

        /* SIP */
        hwDataPtr[3] |= (stdIpL4Ptr->sip.arIP[3] << 16);
        hwDataPtr[3] |= (stdIpL4Ptr->sip.arIP[2] << 24);
        hwDataPtr[4] |=  stdIpL4Ptr->sip.arIP[1];
        hwDataPtr[4] |= (stdIpL4Ptr->sip.arIP[0] << 8);

        /* DIP */
        hwDataPtr[4] |= (stdIpL4Ptr->dip.arIP[3] << 16);
        hwDataPtr[4] |= (stdIpL4Ptr->dip.arIP[2] << 24);
        hwDataPtr[5] |=  stdIpL4Ptr->dip.arIP[1];
        hwDataPtr[5] |= (stdIpL4Ptr->dip.arIP[0] << 8);

        /* UDB20 */
        hwDataPtr[1] |= (stdIpL4Ptr->udb[0] << 24);
        /* UDB21 */
        hwDataPtr[5] |= (stdIpL4Ptr->udb[1] << 16);
        /* UDB22 */
        hwDataPtr[5] |= (stdIpL4Ptr->udb[2] << 24);
    }

    /* extended ingress Not IP and IPV4 */
    if (extNotIpv6Ptr  != NULL)
    {
        /* etherType */
        hwDataPtr[5] |= (extNotIpv6Ptr->etherType << 16);

        /* l2Encap */
        hwDataPtr[3] |= ((extNotIpv6Ptr->l2Encap & 0x01) << 9);

        /* ipv4Fragmented */
        hwDataPtr[3] |= ((extNotIpv6Ptr->ipv4Fragmented & 0x01) << 12);

        /* SIP */
        hwDataPtr[3] |= (extNotIpv6Ptr->sip.arIP[3] << 16);
        hwDataPtr[3] |= (extNotIpv6Ptr->sip.arIP[2] << 24);
        hwDataPtr[4] |=  extNotIpv6Ptr->sip.arIP[1];
        hwDataPtr[4] |= (extNotIpv6Ptr->sip.arIP[0] << 8);

        /* DIP */
        hwDataPtr[4] |= (extNotIpv6Ptr->dip.arIP[3] << 16);
        hwDataPtr[4] |= (extNotIpv6Ptr->dip.arIP[2] << 24);
        hwDataPtr[5] |=  extNotIpv6Ptr->dip.arIP[1];
        hwDataPtr[5] |= (extNotIpv6Ptr->dip.arIP[0] << 8);

        /* MAC DA */
        hwDataPtr[9]  |= (extNotIpv6Ptr->macDa.arEther[5] << 8);
        hwDataPtr[9]  |= (extNotIpv6Ptr->macDa.arEther[4] << 16);
        hwDataPtr[9]  |= (extNotIpv6Ptr->macDa.arEther[3] << 24);
        hwDataPtr[10] |=  extNotIpv6Ptr->macDa.arEther[2];
        hwDataPtr[10] |= (extNotIpv6Ptr->macDa.arEther[1] << 8);
        hwDataPtr[10] |= (extNotIpv6Ptr->macDa.arEther[0] << 16);

        /* MAC SA */
        hwDataPtr[7] |= (extNotIpv6Ptr->macSa.arEther[5] << 24);
        hwDataPtr[8] |=  extNotIpv6Ptr->macSa.arEther[4];
        hwDataPtr[8] |= (extNotIpv6Ptr->macSa.arEther[3] << 8);
        hwDataPtr[8] |= (extNotIpv6Ptr->macSa.arEther[2] << 16);
        hwDataPtr[8] |= (extNotIpv6Ptr->macSa.arEther[1] << 24);
        hwDataPtr[9] |=  extNotIpv6Ptr->macSa.arEther[0];

        /* UDB0 */
        hwDataPtr[11] |= (extNotIpv6Ptr->udb[0] << 24);
        /* UDB1 */
        hwDataPtr[10] |= (extNotIpv6Ptr->udb[1] << 24);
        /* UDB2 */
        hwDataPtr[11] |=  extNotIpv6Ptr->udb[2];
        /* UDB3 */
        hwDataPtr[11] |= (extNotIpv6Ptr->udb[3] << 8);
        /* UDB4 */
        hwDataPtr[11] |= (extNotIpv6Ptr->udb[4] << 16);
        /* UDB5 */
        hwDataPtr[1]  |= (extNotIpv6Ptr->udb[5] << 24);
    }

    /* extended ingress IPV6 L2 */
    if (extIpv6L2Ptr   != NULL)
    {
        /* SIP */
        hwDataPtr[3] |= (extIpv6L2Ptr->sip.arIP[15] << 16);
        hwDataPtr[3] |= (extIpv6L2Ptr->sip.arIP[14] << 24);
        hwDataPtr[4] |=  extIpv6L2Ptr->sip.arIP[13];
        hwDataPtr[4] |= (extIpv6L2Ptr->sip.arIP[12] << 8);
        hwDataPtr[4] |= (extIpv6L2Ptr->sip.arIP[11] << 16);
        hwDataPtr[4] |= (extIpv6L2Ptr->sip.arIP[10] << 24);
        hwDataPtr[5] |=  extIpv6L2Ptr->sip.arIP[9];
        hwDataPtr[5] |= (extIpv6L2Ptr->sip.arIP[8]  << 8);
        hwDataPtr[5] |= (extIpv6L2Ptr->sip.arIP[7]  << 16);
        hwDataPtr[5] |= (extIpv6L2Ptr->sip.arIP[6]  << 24);
        hwDataPtr[6] |=  extIpv6L2Ptr->sip.arIP[5];
        hwDataPtr[6] |= (extIpv6L2Ptr->sip.arIP[4]  << 8);
        hwDataPtr[6] |= (extIpv6L2Ptr->sip.arIP[3]  << 16);
        hwDataPtr[6] |= (extIpv6L2Ptr->sip.arIP[2]  << 24);
        hwDataPtr[7] |=  extIpv6L2Ptr->sip.arIP[1];
        hwDataPtr[7] |= (extIpv6L2Ptr->sip.arIP[0]  << 8);

        /* isIpv6ExtHdrExist */
        hwDataPtr[3] |= ((extIpv6L2Ptr->isIpv6ExtHdrExist & 0x01) << 9);

        /* isIpv6HopByHop */
        hwDataPtr[3] |= ((extIpv6L2Ptr->isIpv6HopByHop & 0x01) << 12);

        /* dipBits127to120 */
        hwDataPtr[7] |= (extIpv6L2Ptr->dipBits127to120 << 16);

        /* MAC DA */
        hwDataPtr[9]  |= (extIpv6L2Ptr->macDa.arEther[5] << 8);
        hwDataPtr[9]  |= (extIpv6L2Ptr->macDa.arEther[4] << 16);
        hwDataPtr[9]  |= (extIpv6L2Ptr->macDa.arEther[3] << 24);
        hwDataPtr[10] |=  extIpv6L2Ptr->macDa.arEther[2];
        hwDataPtr[10] |= (extIpv6L2Ptr->macDa.arEther[1] << 8);
        hwDataPtr[10] |= (extIpv6L2Ptr->macDa.arEther[0] << 16);

        /* MAC SA */
        hwDataPtr[7] |= (extIpv6L2Ptr->macSa.arEther[5] << 24);
        hwDataPtr[8] |=  extIpv6L2Ptr->macSa.arEther[4];
        hwDataPtr[8] |= (extIpv6L2Ptr->macSa.arEther[3] << 8);
        hwDataPtr[8] |= (extIpv6L2Ptr->macSa.arEther[2] << 16);
        hwDataPtr[8] |= (extIpv6L2Ptr->macSa.arEther[1] << 24);
        hwDataPtr[9] |=  extIpv6L2Ptr->macSa.arEther[0];

        /* UDB6 */
        hwDataPtr[10] |= (extIpv6L2Ptr->udb[0] << 24);
        /* UDB7 */
        hwDataPtr[11] |=  extIpv6L2Ptr->udb[1];
        /* UDB8 */
        hwDataPtr[11] |= (extIpv6L2Ptr->udb[2] << 8);
        /* UDB9 */
        hwDataPtr[11] |= (extIpv6L2Ptr->udb[3] << 16);
        /* UDB10 */
        hwDataPtr[11] |= (extIpv6L2Ptr->udb[4] << 24);
        /* UDB11 */
        hwDataPtr[1]  |= (extIpv6L2Ptr->udb[5] << 24);
    }

    /* extended ingress IPV6 L4 */
    if (extIpv6L4Ptr   != NULL)
    {
        /* SIP */
        /* sip 31:0  bits 143:112 */
        hwDataPtr[3] |= (extIpv6L4Ptr->sip.arIP[15] << 16);
        hwDataPtr[3] |= (extIpv6L4Ptr->sip.arIP[14] << 24);
        hwDataPtr[4] |=  extIpv6L4Ptr->sip.arIP[13];
        hwDataPtr[4] |= (extIpv6L4Ptr->sip.arIP[12] << 8);
        /* sip 127:32  bits 271:176 */
        hwDataPtr[5] |= (extIpv6L4Ptr->sip.arIP[11] << 16);
        hwDataPtr[5] |= (extIpv6L4Ptr->sip.arIP[10] << 24);
        hwDataPtr[6] |=  extIpv6L4Ptr->sip.arIP[9];
        hwDataPtr[6] |= (extIpv6L4Ptr->sip.arIP[8]  << 8);
        hwDataPtr[6] |= (extIpv6L4Ptr->sip.arIP[7]  << 16);
        hwDataPtr[6] |= (extIpv6L4Ptr->sip.arIP[6]  << 24);
        hwDataPtr[7] |=  extIpv6L4Ptr->sip.arIP[5];
        hwDataPtr[7] |= (extIpv6L4Ptr->sip.arIP[4]  << 8);
        hwDataPtr[7] |= (extIpv6L4Ptr->sip.arIP[3]  << 16);
        hwDataPtr[7] |= (extIpv6L4Ptr->sip.arIP[2]  << 24);
        hwDataPtr[8] |=  extIpv6L4Ptr->sip.arIP[1];
        hwDataPtr[8] |= (extIpv6L4Ptr->sip.arIP[0]  << 8);

        /* DIP */
        /* dip 31:0 */
        hwDataPtr[4]  |= (extIpv6L4Ptr->dip.arIP[15] << 16);
        hwDataPtr[4]  |= (extIpv6L4Ptr->dip.arIP[14] << 24);
        hwDataPtr[5]  |=  extIpv6L4Ptr->dip.arIP[13];
        hwDataPtr[5]  |= (extIpv6L4Ptr->dip.arIP[12] << 8);
        /* dip 127:120 */
        hwDataPtr[8]  |= (extIpv6L4Ptr->dip.arIP[0] << 16);
        /* dip 119:32 */
        hwDataPtr[8]  |= (extIpv6L4Ptr->dip.arIP[11] << 24);
        hwDataPtr[9]  |=  extIpv6L4Ptr->dip.arIP[10];
        hwDataPtr[9]  |= (extIpv6L4Ptr->dip.arIP[9] << 8);
        hwDataPtr[9]  |= (extIpv6L4Ptr->dip.arIP[8] << 16);
        hwDataPtr[9]  |= (extIpv6L4Ptr->dip.arIP[7] << 24);
        hwDataPtr[10] |=  extIpv6L4Ptr->dip.arIP[6];
        hwDataPtr[10] |= (extIpv6L4Ptr->dip.arIP[5] << 8);
        hwDataPtr[10] |= (extIpv6L4Ptr->dip.arIP[4] << 16);
        hwDataPtr[10] |= (extIpv6L4Ptr->dip.arIP[3] << 24);
        hwDataPtr[11] |=  extIpv6L4Ptr->dip.arIP[2];
        hwDataPtr[11] |= (extIpv6L4Ptr->dip.arIP[1] << 8);

        /* isIpv6ExtHdrExist */
        hwDataPtr[3] |= ((extIpv6L4Ptr->isIpv6ExtHdrExist & 0x01) << 9);

        /* isIpv6HopByHop */
        hwDataPtr[3] |= ((extIpv6L4Ptr->isIpv6HopByHop & 0x01) << 12);

        /* UDB12 */
        hwDataPtr[11] |= (extIpv6L4Ptr->udb[0] << 16);
        /* UDB13 */
        hwDataPtr[11] |= (extIpv6L4Ptr->udb[1] << 24);
        /* UDB14 */
        hwDataPtr[1]  |= (extIpv6L4Ptr->udb[2] << 24);
    }

    if (commonUdbPtr!= NULL)
    {
        /* PCL Id*/
        hwDataPtr[0] |= (commonUdbPtr->pclId & 0x03FF);

        /* macToMe */
        hwDataPtr[0] |= ((commonUdbPtr->macToMe & 1) << 10);

        /* source port */
        hwDataPtr[0] |= ((commonUdbPtr->sourcePort & 0x3F) << 11);

        /* VID */
        hwDataPtr[0] |= ((commonUdbPtr->vid & 0x0FFF) << 18);

        /* UP */
        hwDataPtr[0] |= ((commonUdbPtr->up & 0x03) << 30);
        hwDataPtr[1] |= ((commonUdbPtr->up & 0x04) >> 2);

        /* is IP */
        hwDataPtr[1] |= ((commonUdbPtr->isIp & 0x01) << 9);

        /* is L2 valid */
        hwDataPtr[2] |= ((commonUdbPtr->isL2Valid & 0x01) << 31);

        /* is UDB Valid */
        hwDataPtr[2] |= ((commonUdbPtr->isUdbValid & 0x01) << 30);

        /* isIpv6ExtHdrExist */
        hwDataPtr[2] |= ((commonUdbPtr->isIpv6Eh & 0x01) << 28);

        /* isIpv6HopByHop */
        hwDataPtr[2] |= ((commonUdbPtr->isIpv6HopByHop & 0x01) << 27);

        /* DSCP or EXP */
        hwDataPtr[1] |= ((commonUdbPtr->dscpOrExp & 0x3F) << 10);

        /* pktTagging */
        hwDataPtr[0] |= ((commonUdbPtr->pktTagging & 0x01) << 17);
        hwDataPtr[1] |= (commonUdbPtr->pktTagging & 0x02);

        /* l3OffsetInvalid */
        hwDataPtr[1] |= ((commonUdbPtr->l3OffsetInvalid & 0x01) << 2);

        /* l4ProtocolType */
        hwDataPtr[1] |= ((commonUdbPtr->l4ProtocolType & 0x03) << 3);

        /* pktType */
        hwDataPtr[1] |= ((commonUdbPtr->pktType & 0x03) << 5);
        hwDataPtr[2] |= ((commonUdbPtr->pktType & 0xFC) << 17);

        /* ipHeaderOk */
        hwDataPtr[1] |= ((commonUdbPtr->ipHeaderOk & 0x01) << 7);

        /* macDaType */
        hwDataPtr[2] |= ((commonUdbPtr->macDaType & 0x03) << 16);

        /* l4OffsetInalid */
        hwDataPtr[2] |= ((commonUdbPtr->l4OffsetInalid & 0x01) << 18);

        /* l2Encapsulation */
        hwDataPtr[2] |= ((commonUdbPtr->l2Encapsulation & 0x03) << 25);

    }

    if (stdUdbPtr != NULL)
    {
        /* isIpv4 */
        hwDataPtr[1] |= ((stdUdbPtr->isIpv4 & 0x01) << 8);

        /* UDB0 */
        hwDataPtr[1] |= (stdUdbPtr->udb[0]  << 16);
        /* UDB1 */
        hwDataPtr[1] |= (stdUdbPtr->udb[1]  << 24);
        /* UDB2 */
        hwDataPtr[2] |=  stdUdbPtr->udb[2];
        /* UDB3 */
        hwDataPtr[3] |=  stdUdbPtr->udb[3];
        /* UDB4 */
        hwDataPtr[3] |= (stdUdbPtr->udb[4]  << 8);
        /* UDB5 */
        hwDataPtr[2] |= (stdUdbPtr->udb[5]  << 8);
        /* UDB6 */
        hwDataPtr[3] |= (stdUdbPtr->udb[6]  << 16);
        /* UDB7 */
        hwDataPtr[3] |= (stdUdbPtr->udb[7]  << 24);
        /* UDB8 */
        hwDataPtr[4] |=  stdUdbPtr->udb[8];
        /* UDB9 */
        hwDataPtr[4] |= (stdUdbPtr->udb[9]  << 8);
        /* UDB10 */
        hwDataPtr[4] |= (stdUdbPtr->udb[10] << 16);
        /* UDB11 */
        hwDataPtr[4] |= (stdUdbPtr->udb[11] << 24);
        /* UDB12 */
        hwDataPtr[5] |=  stdUdbPtr->udb[12];
        /* UDB12 */
        hwDataPtr[5] |= (stdUdbPtr->udb[13] << 8);
        /* UDB14 */
        hwDataPtr[5] |= (stdUdbPtr->udb[14] << 16);
        /* UDB15 */
        hwDataPtr[5] |= (stdUdbPtr->udb[15] << 24);

    }

    if (extUdbPtr != NULL)
    {
        /* isIpv6 */
        hwDataPtr[1] |= ((extUdbPtr->isIpv6 & 0x01) << 8);

        /* ipProtocol */
        hwDataPtr[1] |= (extUdbPtr->ipProtocol << 16);

        /* sipBits31_0 */
        hwDataPtr[4] |= (extUdbPtr->sipBits31_0[3] << 16);
        hwDataPtr[4] |= (extUdbPtr->sipBits31_0[2] << 24);
        hwDataPtr[5] |=  extUdbPtr->sipBits31_0[1];
        hwDataPtr[5] |= (extUdbPtr->sipBits31_0[0] << 8);

        /* sipBits79_32orMacSa */
        hwDataPtr[5] |= (extUdbPtr->sipBits79_32orMacSa[5] << 16);
        hwDataPtr[5] |= (extUdbPtr->sipBits79_32orMacSa[4] << 24);
        hwDataPtr[6] |=  extUdbPtr->sipBits79_32orMacSa[3];
        hwDataPtr[6] |= (extUdbPtr->sipBits79_32orMacSa[2] << 8);
        hwDataPtr[6] |= (extUdbPtr->sipBits79_32orMacSa[1] << 16);
        hwDataPtr[6] |= (extUdbPtr->sipBits79_32orMacSa[0] << 24);

        /* sipBits127_80orMacDa */
        hwDataPtr[7] |=  extUdbPtr->sipBits127_80orMacDa[5];
        hwDataPtr[7] |= (extUdbPtr->sipBits127_80orMacDa[4] << 8);
        hwDataPtr[7] |= (extUdbPtr->sipBits127_80orMacDa[3] << 16);
        hwDataPtr[7] |= (extUdbPtr->sipBits127_80orMacDa[2] << 24);
        hwDataPtr[8] |=  extUdbPtr->sipBits127_80orMacDa[1];
        hwDataPtr[8] |= (extUdbPtr->sipBits127_80orMacDa[0] << 8);

        /* dipBits127_112 */
        hwDataPtr[8] |= (extUdbPtr->dipBits127_112[1] << 16);
        hwDataPtr[8] |= (extUdbPtr->dipBits127_112[0] << 24);

        /* dipBits31_0 */
        hwDataPtr[9] |=  extUdbPtr->dipBits31_0[3];
        hwDataPtr[9] |= (extUdbPtr->dipBits31_0[2] << 8);
        hwDataPtr[9] |= (extUdbPtr->dipBits31_0[1] << 16);
        hwDataPtr[9] |= (extUdbPtr->dipBits31_0[0] << 24);

        /* UDB0 */
        hwDataPtr[11] |= (extUdbPtr->udb[0]  << 16);
        /* UDB1 */
        hwDataPtr[1]  |= (extUdbPtr->udb[1]  << 24);
        /* UDB2 */
        hwDataPtr[2]  |=  extUdbPtr->udb[2];
        /* UDB3 */
        hwDataPtr[3]  |=  extUdbPtr->udb[3];
        /* UDB4 */
        hwDataPtr[3]  |= (extUdbPtr->udb[4]  << 8);
        /* UDB5 */
        hwDataPtr[2]  |= (extUdbPtr->udb[5]  << 8);
        /* UDB6 */
        hwDataPtr[3]  |= (extUdbPtr->udb[6]  << 16);
        /* UDB7 */
        hwDataPtr[3]  |= (extUdbPtr->udb[7]  << 24);
        /* UDB8 */
        hwDataPtr[4]  |=  extUdbPtr->udb[8];
        /* UDB9 */
        hwDataPtr[4]  |= (extUdbPtr->udb[9]  << 8);
        /* UDB10 */
        hwDataPtr[10] |= extUdbPtr->udb[10];
        /* UDB11 */
        hwDataPtr[10] |= (extUdbPtr->udb[11] << 8);
        /* UDB12 */
        hwDataPtr[10] |= (extUdbPtr->udb[12] << 16);
        /* UDB13 */
        hwDataPtr[10] |= (extUdbPtr->udb[13] << 24);
        /* UDB14 */
        hwDataPtr[11] |=  extUdbPtr->udb[14];
        /* UDB15 */
        hwDataPtr[11] |= (extUdbPtr->udb[15] << 8);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamRuleDataSw2HwConvert
*
* DESCRIPTION:
*       Converts TCAM pattern or mask from SW to HW format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       ruleFormat     - one of 13 supported key formats
*       swDataPtr      - rule pattern or mask in SW format
*       egrPacketType  - egress packet type
*                        0 - packet to CPU
*                        1 - packet from CPU
*                        2 - packet to ANALYZER
*                        3 - forward DATA packet
*                        0xFF - not specified
*                        needed to parse depended fields
* OUTPUTS:
*       hwDataPtr      - rule pattern or mask in HW format (6 or 12 words)
*
* RETURNS :
*       GT_OK            - on success.
*       GT_BAD_PARAM     - on wrong parameters.
*       GT_OUT_OF_RANGE  - one of the parameters is out of range.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleDataSw2HwConvert
(
    IN  GT_U8                              devNum,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN  CPSS_DXCH_PCL_RULE_FORMAT_UNT      *swDataPtr,
    IN  GT_U8                              egrPacketType,
    OUT GT_U32                             *hwDataPtr
)
{
    GT_U32  size;   /* size of heData in words    */
    GT_U32  i;      /* loop index                 */
    GT_U32  bitPos; /* position of bit in the key */
    GT_BOOL hasUDB; /* GT_TRUE - the key has UDBs */

    /* format depended pointers into swDataPtr */
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STC            *commonPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_EXT_STC        *commonExtPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_COMMON_STD_IP_STC     *commonStdIpPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_NOT_IP_STC        *stdNotIpPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IP_L2_QOS_STC     *stdIpL2QosPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV4_L4_STC       *stdIpL4Ptr;
    CPSS_DXCH_PCL_RULE_FORMAT_STD_IPV6_DIP_STC      *stdIpv6DipPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_NOT_IPV6_STC      *extNotIpv6Ptr;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L2_STC       *extIpv6L2Ptr;
    CPSS_DXCH_PCL_RULE_FORMAT_EXT_IPV6_L4_STC       *extIpv6L4Ptr;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STC        *commonEgrPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_EXT_STC    *commonEgrExtPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_COMMON_STD_IP_STC *commonEgrStdIpPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_NOT_IP_STC    *stdEgrNotIpPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IP_L2_QOS_STC *stdEgrIpL2QosPtr;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_STD_IPV4_L4_STC   *stdEgrIpL4Ptr;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_NOT_IPV6_STC  *extEgrNotIpv6Ptr;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L2_STC   *extEgrIpv6L2Ptr;
    CPSS_DXCH_PCL_RULE_FORMAT_EGR_EXT_IPV6_L4_STC   *extEgrIpv6L4Ptr;

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat and above ingress keys special conversion */
        switch (ruleFormat)
        {
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
                return prvCpssDxChXCatIngressPclTcamRuleDataSw2HwConvert(
                    ruleFormat, swDataPtr, hwDataPtr);
            default: break;
        }
    }

    /* clear all format depended pointers    */
    /* only relevant will get not null value */
    commonPtr         = NULL;
    commonExtPtr      = NULL;
    commonStdIpPtr    = NULL;
    stdNotIpPtr       = NULL;
    stdIpL2QosPtr     = NULL;
    stdIpL4Ptr        = NULL;
    stdIpv6DipPtr     = NULL;
    extNotIpv6Ptr     = NULL;
    extIpv6L2Ptr      = NULL;
    extIpv6L4Ptr      = NULL;
    commonEgrPtr      = NULL;
    commonEgrExtPtr   = NULL;
    commonEgrStdIpPtr = NULL;
    stdEgrNotIpPtr    = NULL;
    stdEgrIpL2QosPtr  = NULL;
    stdEgrIpL4Ptr     = NULL;
    extEgrNotIpv6Ptr  = NULL;
    extEgrIpv6L2Ptr   = NULL;
    extEgrIpv6L4Ptr   = NULL;

    hasUDB = GT_FALSE;

    switch (ruleFormat)
    {
        /* ingress formats */
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
            stdNotIpPtr    = &(swDataPtr->ruleStdNotIp);
            commonPtr      = &(stdNotIpPtr->common);
            size           = 6;
            hasUDB         = GT_TRUE;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
            stdIpL2QosPtr  = &(swDataPtr->ruleStdIpL2Qos);
            commonPtr      = &(stdIpL2QosPtr->common);
            commonStdIpPtr = &(stdIpL2QosPtr->commonStdIp);
            size           = 6;
            hasUDB         = GT_TRUE;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
            stdIpL4Ptr     = &(swDataPtr->ruleStdIpv4L4);
            commonPtr      = &(stdIpL4Ptr->common);
            commonStdIpPtr = &(stdIpL4Ptr->commonStdIp);
            size           = 6;
            hasUDB         = GT_TRUE;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
            stdIpv6DipPtr  = &(swDataPtr->ruleStdIpv6Dip);
            commonPtr      = &(stdIpv6DipPtr->common);
            commonStdIpPtr = &(stdIpv6DipPtr->commonStdIp);
            size           = 6;
            hasUDB         = GT_FALSE;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
            extNotIpv6Ptr  = &(swDataPtr->ruleExtNotIpv6);
            commonPtr      = &(extNotIpv6Ptr->common);
            commonExtPtr   = &(extNotIpv6Ptr->commonExt);
            size           = 12;
            hasUDB         = GT_TRUE;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
            extIpv6L2Ptr   = &(swDataPtr->ruleExtIpv6L2);
            commonPtr      = &(extIpv6L2Ptr->common);
            commonExtPtr   = &(extIpv6L2Ptr->commonExt);
            size           = 12;
            hasUDB         = GT_TRUE;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
            extIpv6L4Ptr   = &(swDataPtr->ruleExtIpv6L4);
            commonPtr      = &(extIpv6L4Ptr->common);
            commonExtPtr   = &(extIpv6L4Ptr->commonExt);
            size           = 12;
            hasUDB         = GT_TRUE;
            break;
        /* egress formats */
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
            stdEgrNotIpPtr    = &(swDataPtr->ruleEgrStdNotIp);
            commonEgrPtr      = &(stdEgrNotIpPtr->common);
            size           = 6;
            hasUDB         = GT_FALSE;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
            stdEgrIpL2QosPtr  = &(swDataPtr->ruleEgrStdIpL2Qos);
            commonEgrPtr      = &(stdEgrIpL2QosPtr->common);
            commonEgrStdIpPtr = &(stdEgrIpL2QosPtr->commonStdIp);
            size           = 6;
            hasUDB         = GT_FALSE;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            stdEgrIpL4Ptr     = &(swDataPtr->ruleEgrStdIpv4L4);
            commonEgrPtr      = &(stdEgrIpL4Ptr->common);
            commonEgrStdIpPtr = &(stdEgrIpL4Ptr->commonStdIp);
            size           = 6;
            hasUDB         = GT_FALSE;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            extEgrNotIpv6Ptr  = &(swDataPtr->ruleEgrExtNotIpv6);
            commonEgrPtr      = &(extEgrNotIpv6Ptr->common);
            commonEgrExtPtr   = &(extEgrNotIpv6Ptr->commonExt);
            size           = 12;
            hasUDB         = GT_FALSE;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
            extEgrIpv6L2Ptr   = &(swDataPtr->ruleEgrExtIpv6L2);
            commonEgrPtr      = &(extEgrIpv6L2Ptr->common);
            commonEgrExtPtr   = &(extEgrIpv6L2Ptr->commonExt);
            size           = 12;
            hasUDB         = GT_FALSE;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
            extEgrIpv6L4Ptr   = &(swDataPtr->ruleEgrExtIpv6L4);
            commonEgrPtr      = &(extEgrIpv6L4Ptr->common);
            commonEgrExtPtr   = &(extEgrIpv6L4Ptr->commonExt);
            size           = 12;
            hasUDB         = GT_FALSE;
            break;
        default: return GT_BAD_PARAM;
    }

    /* clear the hwData before passing data from SW */
    for (i = 0; (i < size); i++)
    {
        hwDataPtr[i] = 0;
    }

    /* common for ingress formats */
    if (commonPtr      != NULL)
    {
        if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
            <= CPSS_PP_FAMILY_CHEETAH2_E)
        {
            /* CH and CH2 */
            /* PCL Id*/
            hwDataPtr[0] |= ((commonPtr->pclId & 0x03FF) << 1);
        }
        else
        {
            /* CH3 and above */
            /* PCL Id*/
            hwDataPtr[0] |= (commonPtr->pclId & 0x03FF);
            hwDataPtr[0] |= ((commonPtr->macToMe & 1) << 10);
        }

        /* source port */
        hwDataPtr[0] |= ((commonPtr->sourcePort & 0x3F) << 11);

        /* isTagged */
        hwDataPtr[0] |= ((commonPtr->isTagged & 0x01) << 17);

        /* VID */
        hwDataPtr[0] |= ((commonPtr->vid & 0x0FFF) << 18);

        /* UP */
        hwDataPtr[0] |= ((commonPtr->up & 0x03) << 30);
        hwDataPtr[1] |= ((commonPtr->up & 0x04) >> 2);

        /* Qos Profile */
        hwDataPtr[1] |= ((commonPtr->qosProfile & 0x7F) << 1);

        /* is IP */
        hwDataPtr[1] |= ((commonPtr->isIp & 0x01) << 9);

        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* is L2 valid. Supported by CH2 and above */
            bitPos = (size <= 6) ? 189 : 381;
            hwDataPtr[bitPos / 32] |=
                ((commonPtr->isL2Valid & 0x01) << (bitPos % 32));
        }

        /* is UDB Valid */
        if (hasUDB != GT_FALSE)
        {
            bitPos = (size <= 6) ? 188 : 382;
            hwDataPtr[bitPos / 32] |=
                ((commonPtr->isUdbValid & 0x01) << (bitPos % 32));
        }

    }

    /* common for egress formats */
    if (commonEgrPtr      != NULL)
    {
        /* valid */
        hwDataPtr[0] |= (commonEgrPtr->valid & 0x01);

        /* PCL Id*/
        hwDataPtr[0] |= ((commonEgrPtr->pclId & 0x03FF) << 1);

        /* source port */
        hwDataPtr[0] |= ((commonEgrPtr->sourcePort & 0x3F) << 11);

        /* isTagged */
        hwDataPtr[0] |= ((commonEgrPtr->isTagged & 0x01) << 17);

        /* VID */
        hwDataPtr[0] |= ((commonEgrPtr->vid & 0x0FFF) << 18);

        /* UP */
        hwDataPtr[0] |= ((commonEgrPtr->up & 0x03) << 30);
        hwDataPtr[1] |= ((commonEgrPtr->up & 0x04) >> 2);

        /* is IP */
        hwDataPtr[1] |= ((commonEgrPtr->isIp & 0x01) << 9);

        /* is L2 valid */
        bitPos = (size <= 6) ? 90 : 370;
        hwDataPtr[bitPos / 32] |=
            ((commonEgrPtr->isL2Valid & 0x01) << (bitPos % 32));

        /* egress Packet Type */
        bitPos = (size <= 6) ? 188 : 381;
        hwDataPtr[bitPos / 32] |=
            ((commonEgrPtr->egrPacketType & 0x03) << (bitPos % 32));

        /* packet type dependend data */
        switch (egrPacketType)
        {
            case 0: /* packet to CPU */
                /* cpuCode bits6-0 and the bit7 */
                hwDataPtr[1] |=
                    ((commonEgrPtr->egrPktType.toCpu.cpuCode & 0x7F) << 1);
                bitPos = (size <= 6) ? 89 : 369;
                hwDataPtr[bitPos / 32] |=
                    (((commonEgrPtr->egrPktType.toCpu.cpuCode >> 7) & 0x01)
                     << (bitPos % 32));

                /* SrcTrg */
                bitPos = (size <= 6) ? 81 : 361;
                hwDataPtr[bitPos / 32] |=
                    ((commonEgrPtr->egrPktType.toCpu.srcTrg & 0x01)
                     << (bitPos % 32));
                break;
            case 1: /* packet from CPU */
                /* traffic class */
                hwDataPtr[1] |=
                    ((commonEgrPtr->egrPktType.fromCpu.tc & 0x07) << 3);

                /* drop priority */
                hwDataPtr[1] |=
                    ((commonEgrPtr->egrPktType.fromCpu.dp & 0x03) << 1);

                /* egress filtering enable */
                bitPos = (size <= 6) ? 80 : 360;
                hwDataPtr[bitPos / 32] |=
                    ((commonEgrPtr->egrPktType.fromCpu.egrFilterEnable & 0x01)
                     << (bitPos % 32));
                break;
            case 2: /* packet to ANALYZER */
                /* to Rx sniffer */
                bitPos = (size <= 6) ? 81 : 361;
                hwDataPtr[bitPos / 32] |=
                    ((commonEgrPtr->egrPktType.toAnalyzer.rxSniff & 0x01)
                     << (bitPos % 32));
                break;
            case 3: /* forward DATA packet */
                /* Qos Profile */
                hwDataPtr[1] |=
                    ((commonEgrPtr->egrPktType.fwdData.qosProfile & 0x7F) << 1);

                /* source is trunk Id */
                bitPos = (size <= 6) ? 75 : 355;
                hwDataPtr[bitPos / 32] |=
                    ((commonEgrPtr->egrPktType.fwdData.srcTrunkId & 0x7F)
                     << (bitPos % 32));

                /* source is trunk */
                bitPos = (size <= 6) ? 82 : 362;
                hwDataPtr[bitPos / 32] |=
                    ((commonEgrPtr->egrPktType.fwdData.srcIsTrunk & 0x01)
                     << (bitPos % 32));

                /* MAC DA is Unknown */
                bitPos = (size <= 6) ? 89 : 369;
                hwDataPtr[bitPos / 32] |=
                    ((commonEgrPtr->egrPktType.fwdData.isUnknown & 0x01)
                     << (bitPos % 32));

                /* is Routed */
                bitPos = (size <= 6) ? 88 : 368;
                hwDataPtr[bitPos / 32] |=
                    ((commonEgrPtr->egrPktType.fwdData.isRouted & 0x01)
                     << (bitPos % 32));

                break;
            default: break;
        }

        /* source Device (if not forwarding from trunk) */
        bitPos = (size <= 6) ? 75 : 355;
        hwDataPtr[bitPos / 32] |=
            ((commonEgrPtr->srcDev & 0x1F) << (bitPos % 32));

        /* source Id */
        bitPos = (size <= 6) ? 83 : 363;
        hwDataPtr[bitPos / 32] |=
            ((commonEgrPtr->sourceId & 0x1F) << (bitPos % 32));

        /* is Vidx */
        bitPos = (size <= 6) ? 191 : 383;
        hwDataPtr[bitPos / 32] |=
            ((commonEgrPtr->isVidx & 0x01) << (bitPos % 32));
    }

    /* common for all extended ingress formats */
    if (commonExtPtr   != NULL)
    {
        /* is IPV6 */
        hwDataPtr[1] |= ((commonExtPtr->isIpv6 & 0x01) << 8);

        /* ip Protocol */
        hwDataPtr[1] |= ((commonExtPtr->ipProtocol & 0x00FF) << 10);

        /* DSCP */
        hwDataPtr[1] |= ((commonExtPtr->dscp & 0x3F) << 18);

        /* isL4Valid */
        hwDataPtr[1] |= ((commonExtPtr->isL4Valid & 0x01) << 24);

        /* l4Byte3 */
        hwDataPtr[1] |= ((commonExtPtr->l4Byte3 & 0x007F) << 25);
        hwDataPtr[2] |= ((commonExtPtr->l4Byte3 & 0x0080) >> 7);

        /* l4Byte2 */
        hwDataPtr[2] |= ((commonExtPtr->l4Byte2 & 0x00FF) << 1);

        /* l4Byte13 */
        hwDataPtr[2] |= ((commonExtPtr->l4Byte13 & 0x00FF) << 9);

        /* l4Byte1 */
        hwDataPtr[2] |= ((commonExtPtr->l4Byte1 & 0x00FF) << 17);

        /* l4Byte0 */
        hwDataPtr[2] |= ((commonExtPtr->l4Byte0 & 0x007F) << 25);
        hwDataPtr[3] |= ((commonExtPtr->l4Byte0 & 0x0080) >> 7);

        /* ipHeaderOk */
        hwDataPtr[11] |= ((commonExtPtr->ipHeaderOk & 0x01) << 31);
    }

    /* common for all extended egress formats */
    if (commonEgrExtPtr   != NULL)
    {
        /* is IPV6 */
        hwDataPtr[1] |= ((commonEgrExtPtr->isIpv6 & 0x01) << 8);

        /* ip Protocol */
        hwDataPtr[1] |= ((commonEgrExtPtr->ipProtocol & 0x00FF) << 10);

        /* DSCP */
        hwDataPtr[1] |= ((commonEgrExtPtr->dscp & 0x3F) << 18);

        /* isL4Valid */
        hwDataPtr[1] |= ((commonEgrExtPtr->isL4Valid & 0x01) << 24);

        /* l4Byte3 */
        hwDataPtr[1] |= ((commonEgrExtPtr->l4Byte3 & 0x007F) << 25);
        hwDataPtr[2] |= ((commonEgrExtPtr->l4Byte3 & 0x0080) >> 7);

        /* l4Byte2 */
        hwDataPtr[2] |= ((commonEgrExtPtr->l4Byte2 & 0x00FF) << 1);

        /* l4Byte13 */
        hwDataPtr[2] |= ((commonEgrExtPtr->l4Byte13 & 0x00FF) << 9);

        /* l4Byte1 */
        hwDataPtr[2] |= ((commonEgrExtPtr->l4Byte1 & 0x00FF) << 17);

        /* l4Byte0 */
        hwDataPtr[2] |= ((commonEgrExtPtr->l4Byte0 & 0x007F) << 25);
        hwDataPtr[3] |= ((commonEgrExtPtr->l4Byte0 & 0x0080) >> 7);

        /* egrTcpUdpPortComparator */
        hwDataPtr[11] |=
            ((commonEgrExtPtr->egrTcpUdpPortComparator & 0xFF) << 19);
    }

    /* common for all standard ingress IP formats */
    if (commonStdIpPtr != NULL)
    {
        /* is IPV4 */
        hwDataPtr[1] |= ((commonStdIpPtr->isIpv4 & 0x01) << 8);

        /* ip Protocol */
        hwDataPtr[1] |= ((commonStdIpPtr->ipProtocol & 0x00FF) << 10);

        /* DSCP */
        hwDataPtr[1] |= ((commonStdIpPtr->dscp & 0x3F) << 18);

        /* isL4Valid */
        hwDataPtr[1] |= ((commonStdIpPtr->isL4Valid & 0x01) << 24);

        /* l4Byte3 */
        hwDataPtr[1] |= ((commonStdIpPtr->l4Byte3 & 0x007F) << 25);
        hwDataPtr[2] |= ((commonStdIpPtr->l4Byte3 & 0x0080) >> 7);

        /* l4Byte2 */
        hwDataPtr[2] |= ((commonStdIpPtr->l4Byte2 & 0x00FF) << 1);

        /* ipv4Fragmented */
        hwDataPtr[5] |= ((commonStdIpPtr->ipv4Fragmented & 0x01) << 30);

        /* ipHeaderOk */
        hwDataPtr[5] |= ((commonStdIpPtr->ipHeaderOk & 0x01) << 31);
    }

    /* common for all standard egress IP formats */
    if (commonEgrStdIpPtr != NULL)
    {
        /* is IPV4 */
        hwDataPtr[1] |= ((commonEgrStdIpPtr->isIpv4 & 0x01) << 8);

        /* ip Protocol */
        hwDataPtr[1] |= ((commonEgrStdIpPtr->ipProtocol & 0x00FF) << 10);

        /* DSCP */
        hwDataPtr[1] |= ((commonEgrStdIpPtr->dscp & 0x3F) << 18);

        /* isL4Valid */
        hwDataPtr[1] |= ((commonEgrStdIpPtr->isL4Valid & 0x01) << 24);

        /* l4Byte3 */
        hwDataPtr[1] |= ((commonEgrStdIpPtr->l4Byte3 & 0x007F) << 25);
        hwDataPtr[2] |= ((commonEgrStdIpPtr->l4Byte3 & 0x0080) >> 7);

        /* l4Byte2 */
        hwDataPtr[2] |= ((commonEgrStdIpPtr->l4Byte2 & 0x00FF) << 1);

        /* ipv4Fragmented */
        hwDataPtr[5] |= ((commonEgrStdIpPtr->ipv4Fragmented & 0x01) << 30);

        /* egrTcpUdpPortComparator */
        hwDataPtr[2] |=
            ((commonEgrStdIpPtr->egrTcpUdpPortComparator & 0x1F) << 27);
        hwDataPtr[3] |=
            ((commonEgrStdIpPtr->egrTcpUdpPortComparator & 0xE0) >> 5);
    }

    /* standard ingress not IP */
    if (stdNotIpPtr    != NULL)
    {
        /* is IPV4 */
        hwDataPtr[1] |= ((stdNotIpPtr->isIpv4 & 0x01) << 8);

        /* etherType */
        hwDataPtr[1] |= ((stdNotIpPtr->etherType & 0xFFFF) << 11);

        /* isArp */
        hwDataPtr[2] |= ((stdNotIpPtr->isArp & 0x01) << 9);

        /* l2Encap */
        hwDataPtr[2] |= ((stdNotIpPtr->l2Encap & 0x01) << 27);

        /* MAC DA */
        hwDataPtr[4] |= ((stdNotIpPtr->macDa.arEther[5] & 0xFF) << 12);
        hwDataPtr[4] |= ((stdNotIpPtr->macDa.arEther[4] & 0xFF) << 20);
        hwDataPtr[4] |= ((stdNotIpPtr->macDa.arEther[3] & 0x0F) << 28);
        hwDataPtr[5] |= ((stdNotIpPtr->macDa.arEther[3] & 0xF0) >> 4);
        hwDataPtr[5] |= ((stdNotIpPtr->macDa.arEther[2] & 0xFF) << 4);
        hwDataPtr[5] |= ((stdNotIpPtr->macDa.arEther[1] & 0xFF) << 12);
        hwDataPtr[5] |= ((stdNotIpPtr->macDa.arEther[0] & 0xFF) << 20);

        /* MAC SA */
        hwDataPtr[2] |= ((stdNotIpPtr->macSa.arEther[5] & 0x0F) << 28);
        hwDataPtr[3] |= ((stdNotIpPtr->macSa.arEther[5] & 0xF0) >> 4);
        hwDataPtr[3] |= ((stdNotIpPtr->macSa.arEther[4] & 0xFF) << 4);
        hwDataPtr[3] |= ((stdNotIpPtr->macSa.arEther[3] & 0xFF) << 12);
        hwDataPtr[3] |= ((stdNotIpPtr->macSa.arEther[2] & 0xFF) << 20);
        hwDataPtr[3] |= ((stdNotIpPtr->macSa.arEther[1] & 0x0F) << 28);
        hwDataPtr[4] |= ((stdNotIpPtr->macSa.arEther[1] & 0xF0) >> 4);
        hwDataPtr[4] |= ((stdNotIpPtr->macSa.arEther[0] & 0xFF) << 4);

        /* UDB */
        hwDataPtr[2] |= ((stdNotIpPtr->udb[0] & 0xFF) << 11);
        hwDataPtr[2] |= ((stdNotIpPtr->udb[1] & 0xFF) << 19);
        hwDataPtr[1] |= ((stdNotIpPtr->udb[2] & 0x1F) << 27);
        hwDataPtr[2] |= ((stdNotIpPtr->udb[2] & 0xE0) >> 5);
    }

    /* standard egress not IP */
    if (stdEgrNotIpPtr    != NULL)
    {
        /* is IPV4 */
        hwDataPtr[1] |= ((stdEgrNotIpPtr->isIpv4 & 0x01) << 8);

        /* etherType */
        hwDataPtr[1] |= ((stdEgrNotIpPtr->etherType & 0xFFFF) << 11);

        /* isArp */
        hwDataPtr[2] |= ((stdEgrNotIpPtr->isArp & 0x01) << 9);

        /* l2Encap */
        hwDataPtr[2] |= ((stdEgrNotIpPtr->l2Encap & 0x01) << 27);

        /* MAC DA */
        hwDataPtr[4] |= ((stdEgrNotIpPtr->macDa.arEther[5] & 0xFF) << 12);
        hwDataPtr[4] |= ((stdEgrNotIpPtr->macDa.arEther[4] & 0xFF) << 20);
        hwDataPtr[4] |= ((stdEgrNotIpPtr->macDa.arEther[3] & 0x0F) << 28);
        hwDataPtr[5] |= ((stdEgrNotIpPtr->macDa.arEther[3] & 0xF0) >> 4);
        hwDataPtr[5] |= ((stdEgrNotIpPtr->macDa.arEther[2] & 0xFF) << 4);
        hwDataPtr[5] |= ((stdEgrNotIpPtr->macDa.arEther[1] & 0xFF) << 12);
        hwDataPtr[5] |= ((stdEgrNotIpPtr->macDa.arEther[0] & 0xFF) << 20);

        /* MAC SA */
        hwDataPtr[2] |= ((stdEgrNotIpPtr->macSa.arEther[5] & 0x0F) << 28);
        hwDataPtr[3] |= ((stdEgrNotIpPtr->macSa.arEther[5] & 0xF0) >> 4);
        hwDataPtr[3] |= ((stdEgrNotIpPtr->macSa.arEther[4] & 0xFF) << 4);
        hwDataPtr[3] |= ((stdEgrNotIpPtr->macSa.arEther[3] & 0xFF) << 12);
        hwDataPtr[3] |= ((stdEgrNotIpPtr->macSa.arEther[2] & 0xFF) << 20);
        hwDataPtr[3] |= ((stdEgrNotIpPtr->macSa.arEther[1] & 0x0F) << 28);
        hwDataPtr[4] |= ((stdEgrNotIpPtr->macSa.arEther[1] & 0xF0) >> 4);
        hwDataPtr[4] |= ((stdEgrNotIpPtr->macSa.arEther[0] & 0xFF) << 4);
    }

    /* standard ingress IPV4 and IPV6 L2 Qos */
    if (stdIpL2QosPtr  != NULL)
    {
        /* isArp */
        hwDataPtr[2] |= ((stdIpL2QosPtr->isArp & 0x01) << 9);

        /* isIpv6ExtHdrExist */
        hwDataPtr[2] |= ((stdIpL2QosPtr->isIpv6ExtHdrExist & 0x01) << 10);

        /* isIpv6HopByHop */
        hwDataPtr[2] |= ((stdIpL2QosPtr->isIpv6HopByHop & 0x01) << 27);

        /* MAC DA */
        hwDataPtr[4] |= ((stdIpL2QosPtr->macDa.arEther[5] & 0xFF) << 12);
        hwDataPtr[4] |= ((stdIpL2QosPtr->macDa.arEther[4] & 0xFF) << 20);
        hwDataPtr[4] |= ((stdIpL2QosPtr->macDa.arEther[3] & 0x0F) << 28);
        hwDataPtr[5] |= ((stdIpL2QosPtr->macDa.arEther[3] & 0xF0) >> 4);
        hwDataPtr[5] |= ((stdIpL2QosPtr->macDa.arEther[2] & 0xFF) << 4);
        hwDataPtr[5] |= ((stdIpL2QosPtr->macDa.arEther[1] & 0xFF) << 12);
        hwDataPtr[5] |= ((stdIpL2QosPtr->macDa.arEther[0] & 0xFF) << 20);

        /* MAC SA */
        hwDataPtr[2] |= ((stdIpL2QosPtr->macSa.arEther[5] & 0x0F) << 28);
        hwDataPtr[3] |= ((stdIpL2QosPtr->macSa.arEther[5] & 0xF0) >> 4);
        hwDataPtr[3] |= ((stdIpL2QosPtr->macSa.arEther[4] & 0xFF) << 4);
        hwDataPtr[3] |= ((stdIpL2QosPtr->macSa.arEther[3] & 0xFF) << 12);
        hwDataPtr[3] |= ((stdIpL2QosPtr->macSa.arEther[2] & 0xFF) << 20);
        hwDataPtr[3] |= ((stdIpL2QosPtr->macSa.arEther[1] & 0x0F) << 28);
        hwDataPtr[4] |= ((stdIpL2QosPtr->macSa.arEther[1] & 0xF0) >> 4);
        hwDataPtr[4] |= ((stdIpL2QosPtr->macSa.arEther[0] & 0xFF) << 4);

        /* UDB */
        hwDataPtr[2] |= ((stdIpL2QosPtr->udb[0] & 0xFF) << 11);
        hwDataPtr[2] |= ((stdIpL2QosPtr->udb[1] & 0xFF) << 19);
    }

    /* standard egress IPV4 and IPV6 L2 Qos */
    if (stdEgrIpL2QosPtr  != NULL)
    {
        /* isArp */
        hwDataPtr[2] |= ((stdEgrIpL2QosPtr->isArp & 0x01) << 9);

        /* dipBits0to31 */
        hwDataPtr[3] |= ((stdEgrIpL2QosPtr->dipBits0to31[3] & 0xFF) <<  3);
        hwDataPtr[3] |= ((stdEgrIpL2QosPtr->dipBits0to31[2] & 0xFF) << 11);
        hwDataPtr[3] |= ((stdEgrIpL2QosPtr->dipBits0to31[1] & 0xFF) << 19);
        hwDataPtr[3] |= ((stdEgrIpL2QosPtr->dipBits0to31[0] & 0x1F) << 27);
        hwDataPtr[4] |= ((stdEgrIpL2QosPtr->dipBits0to31[0] & 0xE0) >>  5);

        /* l4Byte13 */
        hwDataPtr[4] |= ((stdEgrIpL2QosPtr->l4Byte13 & 0xFF) <<  3);

        /* MAC DA */
        hwDataPtr[4] |= ((stdEgrIpL2QosPtr->macDa.arEther[5] & 0xFF) << 12);
        hwDataPtr[4] |= ((stdEgrIpL2QosPtr->macDa.arEther[4] & 0xFF) << 20);
        hwDataPtr[4] |= ((stdEgrIpL2QosPtr->macDa.arEther[3] & 0x0F) << 28);
        hwDataPtr[5] |= ((stdEgrIpL2QosPtr->macDa.arEther[3] & 0xF0) >> 4);
        hwDataPtr[5] |= ((stdEgrIpL2QosPtr->macDa.arEther[2] & 0xFF) << 4);
        hwDataPtr[5] |= ((stdEgrIpL2QosPtr->macDa.arEther[1] & 0xFF) << 12);
        hwDataPtr[5] |= ((stdEgrIpL2QosPtr->macDa.arEther[0] & 0xFF) << 20);
    }

    /* standard ingress IPV4 L4 */
    if (stdIpL4Ptr     != NULL)
    {
        /* isArp */
        hwDataPtr[2] |= ((stdIpL4Ptr->isArp & 0x01) << 9);

        /* isBc */
        hwDataPtr[2] |= ((stdIpL4Ptr->isBc & 0x01) << 10);

        /* l4Byte13 */
        hwDataPtr[5] |= ((stdIpL4Ptr->l4Byte13 & 0xFF) << 3);

        /* l4Byte1 */
        hwDataPtr[5] |= ((stdIpL4Ptr->l4Byte1 & 0xFF) << 11);

        /* l4Byte0 */
        hwDataPtr[5] |= ((stdIpL4Ptr->l4Byte0 & 0xFF) << 19);

        /* SIP */
        hwDataPtr[3] |= ((stdIpL4Ptr->sip.arIP[3] & 0xFF) << 3);
        hwDataPtr[3] |= ((stdIpL4Ptr->sip.arIP[2] & 0xFF) << 11);
        hwDataPtr[3] |= ((stdIpL4Ptr->sip.arIP[1] & 0xFF) << 19);
        hwDataPtr[3] |= ((stdIpL4Ptr->sip.arIP[0] & 0x1F) << 27);
        hwDataPtr[4] |= ((stdIpL4Ptr->sip.arIP[0] & 0xE0) >> 5);

        /* DIP */
        hwDataPtr[4] |= ((stdIpL4Ptr->dip.arIP[3] & 0xFF) << 3);
        hwDataPtr[4] |= ((stdIpL4Ptr->dip.arIP[2] & 0xFF) << 11);
        hwDataPtr[4] |= ((stdIpL4Ptr->dip.arIP[1] & 0xFF) << 19);
        hwDataPtr[4] |= ((stdIpL4Ptr->dip.arIP[0] & 0x1F) << 27);
        hwDataPtr[5] |= ((stdIpL4Ptr->dip.arIP[0] & 0xE0) >> 5);

        /* UDB */
        hwDataPtr[2] |= ((stdIpL4Ptr->udb[0] & 0xFF) << 11);
        hwDataPtr[2] |= ((stdIpL4Ptr->udb[1] & 0xFF) << 19);
        hwDataPtr[2] |= ((stdIpL4Ptr->udb[2] & 0x1F) << 27);
        hwDataPtr[3] |= ((stdIpL4Ptr->udb[2] & 0xE0) >> 5);
    }

    /* standard egress IPV4 L4 */
    if (stdEgrIpL4Ptr     != NULL)
    {
        /* isArp */
        hwDataPtr[2] |= ((stdEgrIpL4Ptr->isArp & 0x01) << 9);

        /* isBc */
        hwDataPtr[2] |= ((stdEgrIpL4Ptr->isBc & 0x01) << 10);

        /* l4Byte13 */
        hwDataPtr[5] |= ((stdEgrIpL4Ptr->l4Byte13 & 0xFF) << 3);

        /* l4Byte1 */
        hwDataPtr[5] |= ((stdEgrIpL4Ptr->l4Byte1 & 0xFF) << 11);

        /* l4Byte0 */
        hwDataPtr[5] |= ((stdEgrIpL4Ptr->l4Byte0 & 0xFF) << 19);

        /* SIP */
        hwDataPtr[3] |= ((stdEgrIpL4Ptr->sip.arIP[3] & 0xFF) << 3);
        hwDataPtr[3] |= ((stdEgrIpL4Ptr->sip.arIP[2] & 0xFF) << 11);
        hwDataPtr[3] |= ((stdEgrIpL4Ptr->sip.arIP[1] & 0xFF) << 19);
        hwDataPtr[3] |= ((stdEgrIpL4Ptr->sip.arIP[0] & 0x1F) << 27);
        hwDataPtr[4] |= ((stdEgrIpL4Ptr->sip.arIP[0] & 0xE0) >> 5);

        /* DIP */
        hwDataPtr[4] |= ((stdEgrIpL4Ptr->dip.arIP[3] & 0xFF) << 3);
        hwDataPtr[4] |= ((stdEgrIpL4Ptr->dip.arIP[2] & 0xFF) << 11);
        hwDataPtr[4] |= ((stdEgrIpL4Ptr->dip.arIP[1] & 0xFF) << 19);
        hwDataPtr[4] |= ((stdEgrIpL4Ptr->dip.arIP[0] & 0x1F) << 27);
        hwDataPtr[5] |= ((stdEgrIpL4Ptr->dip.arIP[0] & 0xE0) >> 5);
    }

    /* standard ingress IPV6 DIP */
    if (stdIpv6DipPtr  != NULL)
    {
        /* isArp */
        hwDataPtr[2] |= ((stdIpv6DipPtr->isArp & 0x01) << 9);

        /* isIpv6ExtHdrExist */
        hwDataPtr[2] |= ((stdIpv6DipPtr->isIpv6ExtHdrExist & 0x01) << 10);

        /* isIpv6HopByHop */
        hwDataPtr[2] |= ((stdIpv6DipPtr->isIpv6HopByHop & 0x01) << 27);

        /* DIP */
        /* dip 15:0 */
        hwDataPtr[1] |= ((stdIpv6DipPtr->dip.arIP[15] & 0x7F) << 25);
        hwDataPtr[2] |= ((stdIpv6DipPtr->dip.arIP[15] & 0x80) >> 7);
        hwDataPtr[2] |= ((stdIpv6DipPtr->dip.arIP[14] & 0xFF) << 1);
        /* dip 31:16 */
        hwDataPtr[2] |= ((stdIpv6DipPtr->dip.arIP[13] & 0xFF) << 11);
        hwDataPtr[2] |= ((stdIpv6DipPtr->dip.arIP[12] & 0xFF) << 19);
        /* dip 127:32 */
        hwDataPtr[2] |= ((stdIpv6DipPtr->dip.arIP[11] & 0x0F) << 28);
        hwDataPtr[3] |= ((stdIpv6DipPtr->dip.arIP[11] & 0xF0) >> 4);
        hwDataPtr[3] |= ((stdIpv6DipPtr->dip.arIP[10] & 0xFF) << 4);
        hwDataPtr[3] |= ((stdIpv6DipPtr->dip.arIP[9] & 0xFF) << 12);
        hwDataPtr[3] |= ((stdIpv6DipPtr->dip.arIP[8] & 0xFF) << 20);
        hwDataPtr[3] |= ((stdIpv6DipPtr->dip.arIP[7] & 0x0F) << 28);
        hwDataPtr[4] |= ((stdIpv6DipPtr->dip.arIP[7] & 0xF0) >> 4);
        hwDataPtr[4] |= ((stdIpv6DipPtr->dip.arIP[6] & 0xFF) << 4);
        hwDataPtr[4] |= ((stdIpv6DipPtr->dip.arIP[5] & 0xFF) << 12);
        hwDataPtr[4] |= ((stdIpv6DipPtr->dip.arIP[4] & 0xFF) << 20);
        hwDataPtr[4] |= ((stdIpv6DipPtr->dip.arIP[3] & 0x0F) << 28);
        hwDataPtr[5] |= ((stdIpv6DipPtr->dip.arIP[3] & 0xF0) >> 4);
        hwDataPtr[5] |= ((stdIpv6DipPtr->dip.arIP[2] & 0xFF) << 4);
        hwDataPtr[5] |= ((stdIpv6DipPtr->dip.arIP[1] & 0xFF) << 12);
        hwDataPtr[5] |= ((stdIpv6DipPtr->dip.arIP[0] & 0xFF) << 20);

    }

    /* extended ingress Not IP and IPV4 */
    if (extNotIpv6Ptr  != NULL)
    {
        /* etherType */
        hwDataPtr[5] |= ((extNotIpv6Ptr->etherType & 0xFFFF) << 2);

        /* l2Encap */
        hwDataPtr[5] |= ((extNotIpv6Ptr->l2Encap & 0x01) << 1);

        /* ipv4Fragmented */
        hwDataPtr[5] |= ((extNotIpv6Ptr->ipv4Fragmented & 0x01) << 18);

        /* SIP */
        hwDataPtr[3] |= ((extNotIpv6Ptr->sip.arIP[3] & 0xFF) << 1);
        hwDataPtr[3] |= ((extNotIpv6Ptr->sip.arIP[2] & 0xFF) << 9);
        hwDataPtr[3] |= ((extNotIpv6Ptr->sip.arIP[1] & 0xFF) << 17);
        hwDataPtr[3] |= ((extNotIpv6Ptr->sip.arIP[0] & 0x7F) << 25);
        hwDataPtr[4] |= ((extNotIpv6Ptr->sip.arIP[0] & 0x80) >> 7);

        /* DIP */
        hwDataPtr[4] |= ((extNotIpv6Ptr->dip.arIP[3] & 0xFF) << 1);
        hwDataPtr[4] |= ((extNotIpv6Ptr->dip.arIP[2] & 0xFF) << 9);
        hwDataPtr[4] |= ((extNotIpv6Ptr->dip.arIP[1] & 0xFF) << 17);
        hwDataPtr[4] |= ((extNotIpv6Ptr->dip.arIP[0] & 0x7F) << 25);
        hwDataPtr[5] |= ((extNotIpv6Ptr->dip.arIP[0] & 0x80) >> 7);

        /* MAC DA */
        hwDataPtr[8] |= ((extNotIpv6Ptr->macDa.arEther[5] & 0x1F) << 27);
        hwDataPtr[9] |= ((extNotIpv6Ptr->macDa.arEther[5] & 0xE0) >> 5);
        hwDataPtr[9] |= ((extNotIpv6Ptr->macDa.arEther[4] & 0xFF) << 3);
        hwDataPtr[9] |= ((extNotIpv6Ptr->macDa.arEther[3] & 0xFF) << 11);
        hwDataPtr[9] |= ((extNotIpv6Ptr->macDa.arEther[2] & 0xFF) << 19);
        hwDataPtr[9] |= ((extNotIpv6Ptr->macDa.arEther[1] & 0x1F) << 27);
        hwDataPtr[10] |= ((extNotIpv6Ptr->macDa.arEther[1] & 0xE0) >> 5);
        hwDataPtr[10] |= ((extNotIpv6Ptr->macDa.arEther[0] & 0xFF) << 3);

        /* MAC SA */
        hwDataPtr[7] |= ((extNotIpv6Ptr->macSa.arEther[5] & 0xFF) << 11);
        hwDataPtr[7] |= ((extNotIpv6Ptr->macSa.arEther[4] & 0xFF) << 19);
        hwDataPtr[7] |= ((extNotIpv6Ptr->macSa.arEther[3] & 0x1F) << 27);
        hwDataPtr[8] |= ((extNotIpv6Ptr->macSa.arEther[3] & 0xE0) >> 5);
        hwDataPtr[8] |= ((extNotIpv6Ptr->macSa.arEther[2] & 0xFF) << 3);
        hwDataPtr[8] |= ((extNotIpv6Ptr->macSa.arEther[1] & 0xFF) << 11);
        hwDataPtr[8] |= ((extNotIpv6Ptr->macSa.arEther[0] & 0xFF) << 19);

        /* UDB */
        hwDataPtr[10] |= ((extNotIpv6Ptr->udb[3] & 0xFF) << 11);
        hwDataPtr[10] |= ((extNotIpv6Ptr->udb[4] & 0xFF) << 19);
        hwDataPtr[10] |= ((extNotIpv6Ptr->udb[5] & 0x1F) << 27);
        hwDataPtr[11] |= ((extNotIpv6Ptr->udb[5] & 0xE0) >> 5);
        hwDataPtr[11] |= ((extNotIpv6Ptr->udb[0] & 0xFF) << 3);
        hwDataPtr[11] |= ((extNotIpv6Ptr->udb[1] & 0xFF) << 11);
        hwDataPtr[11] |= ((extNotIpv6Ptr->udb[2] & 0xFF) << 19);
    }

    /* extended egress Not IP and IPV4 */
    if (extEgrNotIpv6Ptr  != NULL)
    {
        /* etherType */
        hwDataPtr[5] |= ((extEgrNotIpv6Ptr->etherType & 0xFFFF) << 2);

        /* l2Encap */
        hwDataPtr[5] |= ((extEgrNotIpv6Ptr->l2Encap & 0x01) << 1);

        /* ipv4Fragmented */
        hwDataPtr[5] |= ((extEgrNotIpv6Ptr->ipv4Fragmented & 0x01) << 18);

        /* SIP */
        hwDataPtr[3] |= ((extEgrNotIpv6Ptr->sip.arIP[3] & 0xFF) << 1);
        hwDataPtr[3] |= ((extEgrNotIpv6Ptr->sip.arIP[2] & 0xFF) << 9);
        hwDataPtr[3] |= ((extEgrNotIpv6Ptr->sip.arIP[1] & 0xFF) << 17);
        hwDataPtr[3] |= ((extEgrNotIpv6Ptr->sip.arIP[0] & 0x7F) << 25);
        hwDataPtr[4] |= ((extEgrNotIpv6Ptr->sip.arIP[0] & 0x80) >> 7);

        /* DIP */
        hwDataPtr[4] |= ((extEgrNotIpv6Ptr->dip.arIP[3] & 0xFF) << 1);
        hwDataPtr[4] |= ((extEgrNotIpv6Ptr->dip.arIP[2] & 0xFF) << 9);
        hwDataPtr[4] |= ((extEgrNotIpv6Ptr->dip.arIP[1] & 0xFF) << 17);
        hwDataPtr[4] |= ((extEgrNotIpv6Ptr->dip.arIP[0] & 0x7F) << 25);
        hwDataPtr[5] |= ((extEgrNotIpv6Ptr->dip.arIP[0] & 0x80) >> 7);

        /* MAC SA */
        hwDataPtr[7] |= ((extEgrNotIpv6Ptr->macSa.arEther[5] & 0xFF) << 11);
        hwDataPtr[7] |= ((extEgrNotIpv6Ptr->macSa.arEther[4] & 0xFF) << 19);
        hwDataPtr[7] |= ((extEgrNotIpv6Ptr->macSa.arEther[3] & 0x1F) << 27);
        hwDataPtr[8] |= ((extEgrNotIpv6Ptr->macSa.arEther[3] & 0xE0) >> 5);
        hwDataPtr[8] |= ((extEgrNotIpv6Ptr->macSa.arEther[2] & 0xFF) << 3);
        hwDataPtr[8] |= ((extEgrNotIpv6Ptr->macSa.arEther[1] & 0xFF) << 11);
        hwDataPtr[8] |= ((extEgrNotIpv6Ptr->macSa.arEther[0] & 0xFF) << 19);

        /* MAC DA */
        hwDataPtr[8] |= ((extEgrNotIpv6Ptr->macDa.arEther[5] & 0x1F) << 27);
        hwDataPtr[9] |= ((extEgrNotIpv6Ptr->macDa.arEther[5] & 0xE0) >> 5);
        hwDataPtr[9] |= ((extEgrNotIpv6Ptr->macDa.arEther[4] & 0xFF) << 3);
        hwDataPtr[9] |= ((extEgrNotIpv6Ptr->macDa.arEther[3] & 0xFF) << 11);
        hwDataPtr[9] |= ((extEgrNotIpv6Ptr->macDa.arEther[2] & 0xFF) << 19);
        hwDataPtr[9] |= ((extEgrNotIpv6Ptr->macDa.arEther[1] & 0x1F) << 27);
        hwDataPtr[10] |= ((extEgrNotIpv6Ptr->macDa.arEther[1] & 0xE0) >> 5);
        hwDataPtr[10] |= ((extEgrNotIpv6Ptr->macDa.arEther[0] & 0xFF) << 3);
    }

    /* extended ingress IPV6 L2 */
    if (extIpv6L2Ptr   != NULL)
    {
        /* SIP */
        /* sip 31:0*/
        hwDataPtr[3] |= ((extIpv6L2Ptr->sip.arIP[15] & 0xFF) << 1);
        hwDataPtr[3] |= ((extIpv6L2Ptr->sip.arIP[14] & 0xFF) << 9);
        hwDataPtr[3] |= ((extIpv6L2Ptr->sip.arIP[13] & 0xFF) << 17);
        hwDataPtr[3] |= ((extIpv6L2Ptr->sip.arIP[12] & 0x7F) << 25);
        hwDataPtr[4] |= ((extIpv6L2Ptr->sip.arIP[12] & 0x80) >> 7);
        /* sip 127:32 */
        hwDataPtr[4] |= ((extIpv6L2Ptr->sip.arIP[11] & 0xFF) << 1);
        hwDataPtr[4] |= ((extIpv6L2Ptr->sip.arIP[10] & 0xFF) << 9);
        hwDataPtr[4] |= ((extIpv6L2Ptr->sip.arIP[9] & 0xFF) << 17);
        hwDataPtr[4] |= ((extIpv6L2Ptr->sip.arIP[8] & 0x7F) << 25);
        hwDataPtr[5] |= ((extIpv6L2Ptr->sip.arIP[8] & 0x80) >> 7);
        hwDataPtr[5] |= ((extIpv6L2Ptr->sip.arIP[7] & 0xFF) << 1);
        hwDataPtr[5] |= ((extIpv6L2Ptr->sip.arIP[6] & 0xFF) << 9);
        hwDataPtr[5] |= ((extIpv6L2Ptr->sip.arIP[5] & 0xFF) << 17);
        hwDataPtr[5] |= ((extIpv6L2Ptr->sip.arIP[4] & 0x7F) << 25);
        hwDataPtr[6] |= ((extIpv6L2Ptr->sip.arIP[4] & 0x80) >> 7);
        hwDataPtr[6] |= ((extIpv6L2Ptr->sip.arIP[3] & 0xFF) << 1);
        hwDataPtr[6] |= ((extIpv6L2Ptr->sip.arIP[2] & 0xFF) << 9);
        hwDataPtr[6] |= ((extIpv6L2Ptr->sip.arIP[1] & 0xFF) << 17);
        hwDataPtr[6] |= ((extIpv6L2Ptr->sip.arIP[0] & 0x7F) << 25);
        hwDataPtr[7] |= ((extIpv6L2Ptr->sip.arIP[0] & 0x80) >> 7);

        /* isIpv6ExtHdrExist */
        hwDataPtr[7] |= ((extIpv6L2Ptr->isIpv6ExtHdrExist & 0x01) << 1);

        /* isIpv6HopByHop */
        hwDataPtr[7] |= ((extIpv6L2Ptr->isIpv6HopByHop & 0x01) << 2);

        /* dipBits127to120 */
        hwDataPtr[7] |= ((extIpv6L2Ptr->dipBits127to120 & 0xFF) << 3);

        /* MAC DA */
        hwDataPtr[8] |= ((extIpv6L2Ptr->macDa.arEther[5] & 0x1F) << 27);
        hwDataPtr[9] |= ((extIpv6L2Ptr->macDa.arEther[5] & 0xE0) >> 5);
        hwDataPtr[9] |= ((extIpv6L2Ptr->macDa.arEther[4] & 0xFF) << 3);
        hwDataPtr[9] |= ((extIpv6L2Ptr->macDa.arEther[3] & 0xFF) << 11);
        hwDataPtr[9] |= ((extIpv6L2Ptr->macDa.arEther[2] & 0xFF) << 19);
        hwDataPtr[9] |= ((extIpv6L2Ptr->macDa.arEther[1] & 0x1F) << 27);
        hwDataPtr[10] |= ((extIpv6L2Ptr->macDa.arEther[1] & 0xE0) >> 5);
        hwDataPtr[10] |= ((extIpv6L2Ptr->macDa.arEther[0] & 0xFF) << 3);

        /* MAC SA */
        hwDataPtr[7] |= ((extIpv6L2Ptr->macSa.arEther[5] & 0xFF) << 11);
        hwDataPtr[7] |= ((extIpv6L2Ptr->macSa.arEther[4] & 0xFF) << 19);
        hwDataPtr[7] |= ((extIpv6L2Ptr->macSa.arEther[3] & 0x1F) << 27);
        hwDataPtr[8] |= ((extIpv6L2Ptr->macSa.arEther[3] & 0xE0) >> 5);
        hwDataPtr[8] |= ((extIpv6L2Ptr->macSa.arEther[2] & 0xFF) << 3);
        hwDataPtr[8] |= ((extIpv6L2Ptr->macSa.arEther[1] & 0xFF) << 11);
        hwDataPtr[8] |= ((extIpv6L2Ptr->macSa.arEther[0] & 0xFF) << 19);

        /* UDB */
        hwDataPtr[10] |= ((extIpv6L2Ptr->udb[3] & 0xFF) << 11);
        hwDataPtr[10] |= ((extIpv6L2Ptr->udb[4] & 0xFF) << 19);
        hwDataPtr[10] |= ((extIpv6L2Ptr->udb[5] & 0x1F) << 27);
        hwDataPtr[11] |= ((extIpv6L2Ptr->udb[5] & 0xE0) >> 5);
        hwDataPtr[11] |= ((extIpv6L2Ptr->udb[0] & 0xFF) << 3);
        hwDataPtr[11] |= ((extIpv6L2Ptr->udb[1] & 0xFF) << 11);
        hwDataPtr[11] |= ((extIpv6L2Ptr->udb[2] & 0xFF) << 19);
    }

    /* extended egress IPV6 L2 */
    if (extEgrIpv6L2Ptr   != NULL)
    {
        /* SIP */
        /* sip 31:0*/
        hwDataPtr[3] |= ((extEgrIpv6L2Ptr->sip.arIP[15] & 0xFF) << 1);
        hwDataPtr[3] |= ((extEgrIpv6L2Ptr->sip.arIP[14] & 0xFF) << 9);
        hwDataPtr[3] |= ((extEgrIpv6L2Ptr->sip.arIP[13] & 0xFF) << 17);
        hwDataPtr[3] |= ((extEgrIpv6L2Ptr->sip.arIP[12] & 0x7F) << 25);
        hwDataPtr[4] |= ((extEgrIpv6L2Ptr->sip.arIP[12] & 0x80) >> 7);
        /* sip 127:32 */
        hwDataPtr[4] |= ((extEgrIpv6L2Ptr->sip.arIP[11] & 0xFF) << 1);
        hwDataPtr[4] |= ((extEgrIpv6L2Ptr->sip.arIP[10] & 0xFF) << 9);
        hwDataPtr[4] |= ((extEgrIpv6L2Ptr->sip.arIP[9] & 0xFF) << 17);
        hwDataPtr[4] |= ((extEgrIpv6L2Ptr->sip.arIP[8] & 0x7F) << 25);
        hwDataPtr[5] |= ((extEgrIpv6L2Ptr->sip.arIP[8] & 0x80) >> 7);
        hwDataPtr[5] |= ((extEgrIpv6L2Ptr->sip.arIP[7] & 0xFF) << 1);
        hwDataPtr[5] |= ((extEgrIpv6L2Ptr->sip.arIP[6] & 0xFF) << 9);
        hwDataPtr[5] |= ((extEgrIpv6L2Ptr->sip.arIP[5] & 0xFF) << 17);
        hwDataPtr[5] |= ((extEgrIpv6L2Ptr->sip.arIP[4] & 0x7F) << 25);
        hwDataPtr[6] |= ((extEgrIpv6L2Ptr->sip.arIP[4] & 0x80) >> 7);
        hwDataPtr[6] |= ((extEgrIpv6L2Ptr->sip.arIP[3] & 0xFF) << 1);
        hwDataPtr[6] |= ((extEgrIpv6L2Ptr->sip.arIP[2] & 0xFF) << 9);
        hwDataPtr[6] |= ((extEgrIpv6L2Ptr->sip.arIP[1] & 0xFF) << 17);
        hwDataPtr[6] |= ((extEgrIpv6L2Ptr->sip.arIP[0] & 0x7F) << 25);
        hwDataPtr[7] |= ((extEgrIpv6L2Ptr->sip.arIP[0] & 0x80) >> 7);

        /* dipBits127to120 */
        hwDataPtr[7] |= ((extEgrIpv6L2Ptr->dipBits127to120 & 0xFF) << 3);

        /* MAC SA */
        hwDataPtr[7] |= ((extEgrIpv6L2Ptr->macSa.arEther[5] & 0xFF) << 11);
        hwDataPtr[7] |= ((extEgrIpv6L2Ptr->macSa.arEther[4] & 0xFF) << 19);
        hwDataPtr[7] |= ((extEgrIpv6L2Ptr->macSa.arEther[3] & 0x1F) << 27);
        hwDataPtr[8] |= ((extEgrIpv6L2Ptr->macSa.arEther[3] & 0xE0) >> 5);
        hwDataPtr[8] |= ((extEgrIpv6L2Ptr->macSa.arEther[2] & 0xFF) << 3);
        hwDataPtr[8] |= ((extEgrIpv6L2Ptr->macSa.arEther[1] & 0xFF) << 11);
        hwDataPtr[8] |= ((extEgrIpv6L2Ptr->macSa.arEther[0] & 0xFF) << 19);

        /* MAC DA */
        hwDataPtr[8] |= ((extEgrIpv6L2Ptr->macDa.arEther[5] & 0x1F) << 27);
        hwDataPtr[9] |= ((extEgrIpv6L2Ptr->macDa.arEther[5] & 0xE0) >> 5);
        hwDataPtr[9] |= ((extEgrIpv6L2Ptr->macDa.arEther[4] & 0xFF) << 3);
        hwDataPtr[9] |= ((extEgrIpv6L2Ptr->macDa.arEther[3] & 0xFF) << 11);
        hwDataPtr[9] |= ((extEgrIpv6L2Ptr->macDa.arEther[2] & 0xFF) << 19);
        hwDataPtr[9] |= ((extEgrIpv6L2Ptr->macDa.arEther[1] & 0x1F) << 27);
        hwDataPtr[10] |= ((extEgrIpv6L2Ptr->macDa.arEther[1] & 0xE0) >> 5);
        hwDataPtr[10] |= ((extEgrIpv6L2Ptr->macDa.arEther[0] & 0xFF) << 3);
    }

    /* extended ingress IPV6 L4 */
    if (extIpv6L4Ptr   != NULL)
    {
        /* SIP */
        /* sip 31:0*/
        hwDataPtr[3] |= ((extIpv6L4Ptr->sip.arIP[15] & 0xFF) << 1);
        hwDataPtr[3] |= ((extIpv6L4Ptr->sip.arIP[14] & 0xFF) << 9);
        hwDataPtr[3] |= ((extIpv6L4Ptr->sip.arIP[13] & 0xFF) << 17);
        hwDataPtr[3] |= ((extIpv6L4Ptr->sip.arIP[12] & 0x7F) << 25);
        hwDataPtr[4] |= ((extIpv6L4Ptr->sip.arIP[12] & 0x80) >> 7);
        /* sip 127:32 */
        hwDataPtr[4] |= ((extIpv6L4Ptr->sip.arIP[11] & 0xFF) << 1);
        hwDataPtr[4] |= ((extIpv6L4Ptr->sip.arIP[10] & 0xFF) << 9);
        hwDataPtr[4] |= ((extIpv6L4Ptr->sip.arIP[9] & 0xFF) << 17);
        hwDataPtr[4] |= ((extIpv6L4Ptr->sip.arIP[8] & 0x7F) << 25);
        hwDataPtr[5] |= ((extIpv6L4Ptr->sip.arIP[8] & 0x80) >> 7);
        hwDataPtr[5] |= ((extIpv6L4Ptr->sip.arIP[7] & 0xFF) << 1);
        hwDataPtr[5] |= ((extIpv6L4Ptr->sip.arIP[6] & 0xFF) << 9);
        hwDataPtr[5] |= ((extIpv6L4Ptr->sip.arIP[5] & 0xFF) << 17);
        hwDataPtr[5] |= ((extIpv6L4Ptr->sip.arIP[4] & 0x7F) << 25);
        hwDataPtr[6] |= ((extIpv6L4Ptr->sip.arIP[4] & 0x80) >> 7);
        hwDataPtr[6] |= ((extIpv6L4Ptr->sip.arIP[3] & 0xFF) << 1);
        hwDataPtr[6] |= ((extIpv6L4Ptr->sip.arIP[2] & 0xFF) << 9);
        hwDataPtr[6] |= ((extIpv6L4Ptr->sip.arIP[1] & 0xFF) << 17);
        hwDataPtr[6] |= ((extIpv6L4Ptr->sip.arIP[0] & 0x7F) << 25);
        hwDataPtr[7] |= ((extIpv6L4Ptr->sip.arIP[0] & 0x80) >> 7);

        /* isIpv6ExtHdrExist */
        hwDataPtr[7] |= ((extIpv6L4Ptr->isIpv6ExtHdrExist & 0x01) << 1);

        /* isIpv6HopByHop */
        hwDataPtr[7] |= ((extIpv6L4Ptr->isIpv6HopByHop & 0x01) << 2);

        /* DIP */
        /* dip 127:120 */
        hwDataPtr[7] |= ((extIpv6L4Ptr->dip.arIP[0] & 0xFF) << 3);
        /* dip 119:32 */
        hwDataPtr[7] |= ((extIpv6L4Ptr->dip.arIP[11] & 0xFF) << 11);
        hwDataPtr[7] |= ((extIpv6L4Ptr->dip.arIP[10] & 0xFF) << 19);
        hwDataPtr[7] |= ((extIpv6L4Ptr->dip.arIP[9] & 0x1F) << 27);
        hwDataPtr[8] |= ((extIpv6L4Ptr->dip.arIP[9] & 0xE0) >> 5);
        hwDataPtr[8] |= ((extIpv6L4Ptr->dip.arIP[8] & 0xFF) << 3);
        hwDataPtr[8] |= ((extIpv6L4Ptr->dip.arIP[7] & 0xFF) << 11);
        hwDataPtr[8] |= ((extIpv6L4Ptr->dip.arIP[6] & 0xFF) << 19);
        hwDataPtr[8] |= ((extIpv6L4Ptr->dip.arIP[5] & 0x1F) << 27);
        hwDataPtr[9] |= ((extIpv6L4Ptr->dip.arIP[5] & 0xE0) >> 5);
        hwDataPtr[9] |= ((extIpv6L4Ptr->dip.arIP[4] & 0xFF) << 3);
        hwDataPtr[9] |= ((extIpv6L4Ptr->dip.arIP[3] & 0xFF) << 11);
        hwDataPtr[9] |= ((extIpv6L4Ptr->dip.arIP[2] & 0xFF) << 19);
        hwDataPtr[9] |= ((extIpv6L4Ptr->dip.arIP[1] & 0x1F) << 27);
        hwDataPtr[10] |= ((extIpv6L4Ptr->dip.arIP[1] & 0xE0) >> 5);

        /* dip 31:0 */
        hwDataPtr[10] |= ((extIpv6L4Ptr->dip.arIP[15] & 0xFF) << 3);
        hwDataPtr[10] |= ((extIpv6L4Ptr->dip.arIP[14] & 0xFF) << 11);
        hwDataPtr[10] |= ((extIpv6L4Ptr->dip.arIP[13] & 0xFF) << 19);
        hwDataPtr[10] |= ((extIpv6L4Ptr->dip.arIP[12] & 0x1F) << 27);
        hwDataPtr[11] |= ((extIpv6L4Ptr->dip.arIP[12] & 0xE0) >> 5);

        /* UDB */
        hwDataPtr[11] |= ((extIpv6L4Ptr->udb[0] & 0xFF) << 3);
        hwDataPtr[11] |= ((extIpv6L4Ptr->udb[1] & 0xFF) << 11);
        hwDataPtr[11] |= ((extIpv6L4Ptr->udb[2] & 0xFF) << 19);
    }

    /* extended egress IPV6 L4 */
    if (extEgrIpv6L4Ptr   != NULL)
    {
        /* SIP */
        /* sip 31:0*/
        hwDataPtr[3] |= ((extEgrIpv6L4Ptr->sip.arIP[15] & 0xFF) << 1);
        hwDataPtr[3] |= ((extEgrIpv6L4Ptr->sip.arIP[14] & 0xFF) << 9);
        hwDataPtr[3] |= ((extEgrIpv6L4Ptr->sip.arIP[13] & 0xFF) << 17);
        hwDataPtr[3] |= ((extEgrIpv6L4Ptr->sip.arIP[12] & 0x7F) << 25);
        hwDataPtr[4] |= ((extEgrIpv6L4Ptr->sip.arIP[12] & 0x80) >> 7);
        /* sip 127:32 */
        hwDataPtr[4] |= ((extEgrIpv6L4Ptr->sip.arIP[11] & 0xFF) << 1);
        hwDataPtr[4] |= ((extEgrIpv6L4Ptr->sip.arIP[10] & 0xFF) << 9);
        hwDataPtr[4] |= ((extEgrIpv6L4Ptr->sip.arIP[9] & 0xFF) << 17);
        hwDataPtr[4] |= ((extEgrIpv6L4Ptr->sip.arIP[8] & 0x7F) << 25);
        hwDataPtr[5] |= ((extEgrIpv6L4Ptr->sip.arIP[8] & 0x80) >> 7);
        hwDataPtr[5] |= ((extEgrIpv6L4Ptr->sip.arIP[7] & 0xFF) << 1);
        hwDataPtr[5] |= ((extEgrIpv6L4Ptr->sip.arIP[6] & 0xFF) << 9);
        hwDataPtr[5] |= ((extEgrIpv6L4Ptr->sip.arIP[5] & 0xFF) << 17);
        hwDataPtr[5] |= ((extEgrIpv6L4Ptr->sip.arIP[4] & 0x7F) << 25);
        hwDataPtr[6] |= ((extEgrIpv6L4Ptr->sip.arIP[4] & 0x80) >> 7);
        hwDataPtr[6] |= ((extEgrIpv6L4Ptr->sip.arIP[3] & 0xFF) << 1);
        hwDataPtr[6] |= ((extEgrIpv6L4Ptr->sip.arIP[2] & 0xFF) << 9);
        hwDataPtr[6] |= ((extEgrIpv6L4Ptr->sip.arIP[1] & 0xFF) << 17);
        hwDataPtr[6] |= ((extEgrIpv6L4Ptr->sip.arIP[0] & 0x7F) << 25);
        hwDataPtr[7] |= ((extEgrIpv6L4Ptr->sip.arIP[0] & 0x80) >> 7);

        /* DIP */
        /* dip 127:120 */
        hwDataPtr[7] |= ((extEgrIpv6L4Ptr->dip.arIP[0] & 0xFF) << 3);

        /* dip 119:32 */
        hwDataPtr[7] |= ((extEgrIpv6L4Ptr->dip.arIP[11] & 0xFF) << 11);
        hwDataPtr[7] |= ((extEgrIpv6L4Ptr->dip.arIP[10] & 0xFF) << 19);
        hwDataPtr[7] |= ((extEgrIpv6L4Ptr->dip.arIP[9] & 0x1F) << 27);
        hwDataPtr[8] |= ((extEgrIpv6L4Ptr->dip.arIP[9] & 0xE0) >> 5);
        hwDataPtr[8] |= ((extEgrIpv6L4Ptr->dip.arIP[8] & 0xFF) << 3);
        hwDataPtr[8] |= ((extEgrIpv6L4Ptr->dip.arIP[7] & 0xFF) << 11);
        hwDataPtr[8] |= ((extEgrIpv6L4Ptr->dip.arIP[6] & 0xFF) << 19);
        hwDataPtr[8] |= ((extEgrIpv6L4Ptr->dip.arIP[5] & 0x1F) << 27);
        hwDataPtr[9] |= ((extEgrIpv6L4Ptr->dip.arIP[5] & 0xE0) >> 5);
        hwDataPtr[9] |= ((extEgrIpv6L4Ptr->dip.arIP[4] & 0xFF) << 3);
        hwDataPtr[9] |= ((extEgrIpv6L4Ptr->dip.arIP[3] & 0xFF) << 11);
        hwDataPtr[9] |= ((extEgrIpv6L4Ptr->dip.arIP[2] & 0xFF) << 19);
        hwDataPtr[9] |= ((extEgrIpv6L4Ptr->dip.arIP[1] & 0x1F) << 27);
        hwDataPtr[10] |= ((extEgrIpv6L4Ptr->dip.arIP[1] & 0xE0) >> 5);

        /* dip 31:0 */
        hwDataPtr[10] |= ((extEgrIpv6L4Ptr->dip.arIP[15] & 0xFF) << 3);
        hwDataPtr[10] |= ((extEgrIpv6L4Ptr->dip.arIP[14] & 0xFF) << 11);
        hwDataPtr[10] |= ((extEgrIpv6L4Ptr->dip.arIP[13] & 0xFF) << 19);
        hwDataPtr[10] |= ((extEgrIpv6L4Ptr->dip.arIP[12] & 0x1F) << 27);
        hwDataPtr[11] |= ((extEgrIpv6L4Ptr->dip.arIP[12] & 0xE0) >> 5);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclCfgTblHwSelectTblAndPos
*
* DESCRIPTION:
*    Selects the PCL Configuration table HW id and the field positions descriptor.
*    The position descriptor is a SW structure that contains offsets lengts
*    of the relevasnt subfields of the table entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       direction         - Policy direction:
*                           Ingress or Egress
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       subLookupNum      - Sub lookup Number (APPLICABLE DEVICES: xCat2)
*
* OUTPUTS:
*       tableIdPtr        - (pointer to) HW table Id
*       posPtrPtr         - (pointer to) (pointer to) position descriptor
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclCfgTblHwSelectTblAndPos
(
    IN  GT_U8                                  devNum,
    IN  CPSS_PCL_DIRECTION_ENT                 direction,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT             lookupNum,
    IN  GT_U32                                 subLookupNum,
    OUT PRV_CPSS_DXCH_TABLE_ENT                *tableIdPtr,
    OUT PRV_PCL_CFG_LOOKUP_POS_STC             **posPtrPtr
)
{
    switch (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
            if (direction != CPSS_PCL_DIRECTION_INGRESS_E)
            {
                return GT_BAD_PARAM;
            }
            switch (lookupNum)
            {
                case CPSS_PCL_LOOKUP_0_E:
                    *posPtrPtr = &prvCpssDxChPclCfgTblHwChPlusIngrL0Pos;
                    break;
                case CPSS_PCL_LOOKUP_1_E:
                    *posPtrPtr = &prvCpssDxChPclCfgTblHwChPlusIngrL1Pos;
                    break;
                default: return GT_BAD_PARAM;
            }
            break;
        case CPSS_PP_FAMILY_CHEETAH2_E:
            switch (direction)
            {
                case CPSS_PCL_DIRECTION_INGRESS_E:
                    switch (lookupNum)
                    {
                        case CPSS_PCL_LOOKUP_0_E:
                            *posPtrPtr = &prvCpssDxChPclCfgTblHwCh2IngrL0Pos;
                            break;
                        case CPSS_PCL_LOOKUP_1_E:
                            *posPtrPtr = &prvCpssDxChPclCfgTblHwCh2IngrL1Pos;
                            break;
                        default: return GT_BAD_PARAM;
                    }
                    break;
                case CPSS_PCL_DIRECTION_EGRESS_E:
                    /* ignore lookup number */
                    *posPtrPtr = &prvCpssDxChPclCfgTblHwCh2EgrPos;
                    break;
                default: return GT_BAD_PARAM;
            }
            break;
        default:
            if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat2 and above */
                switch (direction)
                {
                    case CPSS_PCL_DIRECTION_INGRESS_E:
                        /* the same for 3 lookups 0, 1 and 0_1 */
                        *posPtrPtr = &prvCpssDxChPclCfgTblHwXCatIngrL1Pos;
                        break;
                    case CPSS_PCL_DIRECTION_EGRESS_E:
                        /* ignore lookup number */
                        *posPtrPtr = &prvCpssDxChPclCfgTblHwCh3EgrPos;
                        break;
                    default: return GT_BAD_PARAM;
                }
            }
            else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat and Lion */
                switch (direction)
                {
                    case CPSS_PCL_DIRECTION_INGRESS_E:
                        switch (lookupNum)
                        {
                            case CPSS_PCL_LOOKUP_0_E:
                                *posPtrPtr = &prvCpssDxChPclCfgTblHwXCatIngrL0Pos;
                                break;
                            case CPSS_PCL_LOOKUP_1_E:
                                *posPtrPtr = &prvCpssDxChPclCfgTblHwXCatIngrL1Pos;
                                break;
                            default: return GT_BAD_PARAM;
                        }
                        break;
                    case CPSS_PCL_DIRECTION_EGRESS_E:
                        /* ignore lookup number */
                        *posPtrPtr = &prvCpssDxChPclCfgTblHwCh3EgrPos;
                        break;
                    default: return GT_BAD_PARAM;
                }
            }
            else
            {
                /* DxCh3 */
                switch (direction)
                {
                    case CPSS_PCL_DIRECTION_INGRESS_E:
                        switch (lookupNum)
                        {
                            case CPSS_PCL_LOOKUP_0_E:
                                *posPtrPtr = &prvCpssDxChPclCfgTblHwCh3IngrL0Pos;
                                break;
                            case CPSS_PCL_LOOKUP_1_E:
                                *posPtrPtr = &prvCpssDxChPclCfgTblHwCh3IngrL1Pos;
                                break;
                            default: return GT_BAD_PARAM;
                        }
                        break;
                    case CPSS_PCL_DIRECTION_EGRESS_E:
                        /* ignore lookup number */
                        *posPtrPtr = &prvCpssDxChPclCfgTblHwCh3EgrPos;
                        break;
                    default: return GT_BAD_PARAM;
                }
            }
            break;
    }

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            if (PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
            {
                /* xCat2 and above */
                switch (lookupNum)
                {
                    case CPSS_PCL_LOOKUP_0_E:
                        *tableIdPtr =
                            (subLookupNum == 0)
                            ? PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E
                            : PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E;
                        break;
                    case CPSS_PCL_LOOKUP_1_E:
                        /* xCat2 IPCL Lookup1 Cfg Table */
                        *tableIdPtr = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E;
                        break;
                    case CPSS_PCL_LOOKUP_0_0_E:
                        /* xCat2 IPCL Lookup00 Cfg Table */
                        *tableIdPtr = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
                        break;
                    case CPSS_PCL_LOOKUP_0_1_E:
                        /* xCat2 IPCL Lookup01 Cfg Table */
                        *tableIdPtr = PRV_CPSS_DXCH_XCAT2_TABLE_INGRESS_PCL_LOOKUP01_CONFIG_E;
                        break;
                    default: return GT_BAD_PARAM;
                }
            }
            else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum)
                && (lookupNum == CPSS_PCL_LOOKUP_1_E))
            {
                /* xCat and Lion IPCL Lookup1 Cfg Table */
                *tableIdPtr = PRV_CPSS_DXCH_XCAT_TABLE_INGRESS_PCL_LOOKUP1_CONFIG_E;
            }
            else
            {
                /* DxCh1-3 any lookup                   */
                /* DxCat and Lion lookup00 and lookup01 */
                *tableIdPtr = PRV_CPSS_DXCH_TABLE_PCL_CONFIG_E;
            }

            break;
        case CPSS_PCL_DIRECTION_EGRESS_E:
            *tableIdPtr = PRV_CPSS_DXCH2_TABLE_EGRESS_PCL_CONFIG_E;
            break;
        default: return GT_BAD_PARAM;
    }

    return  GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclCfgTblEntryIndexGet
*
* DESCRIPTION:
*    Calculates the index of PCL Configuration table entry's for given interface.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       interfaceInfoPtr  - interface data: either port or VLAN
*       direction         - Policy direction:
*                           Ingress or Egress
*
* OUTPUTS:
*       indexPtr          - (pointer to) calculated index
*       portGroupIdPtr         - (pointer to) the port group Id that the index refers to.
*                               needed to support multi-port-groups device.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_HW_ERROR        - on HW error
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclCfgTblEntryIndexGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_INTERFACE_INFO_STC         *interfaceInfoPtr,
    IN  CPSS_PCL_DIRECTION_ENT          direction,
    OUT GT_U32                          *indexPtr,
    OUT GT_U32                          *portGroupIdPtr
)
{
    GT_U32    regAddr;       /* register address                    */
    GT_STATUS res;           /* return code                         */
    GT_U32    entryIndex;    /* pcl configuration table entry Index */
    GT_U32    gloRegData;    /* data from an appropriate global register */
    GT_BOOL   nonLocalAccess;/* GT_TRUE - non Local Access          */
    GT_U32    nonLocalBase;  /* base 0,1024,2048,3072               */
    GT_U32    portsPerDev;   /* amount of ports Per Device          */
    GT_U8     localPort;     /* local port - support multi-port-groups device */

    /* state that the index belongs to all port groups -- for multi-port-groups device */
    *portGroupIdPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    switch (direction)
    {
        case CPSS_PCL_DIRECTION_INGRESS_E:
            if (interfaceInfoPtr->type == CPSS_INTERFACE_INDEX_E)
            {
                if (interfaceInfoPtr->index < PRV_DXCH_IPCL_CONF_TBL_ROW_NUMBER_CNS)
                {
                    *indexPtr = interfaceInfoPtr->index;
                    return GT_OK;
                }
                else
                {
                    return GT_BAD_PARAM;
                }
            }
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.control;
            break;
        case CPSS_PCL_DIRECTION_EGRESS_E:
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
                 == CPSS_PP_FAMILY_CHEETAH_E)
            {
                return GT_BAD_PARAM;
            }

            if (interfaceInfoPtr->type == CPSS_INTERFACE_INDEX_E)
            {
                if (interfaceInfoPtr->index < PRV_DXCH_EPCL_CONF_TBL_ROW_NUMBER_CNS)
                {
                    *indexPtr = interfaceInfoPtr->index;
                    return GT_OK;
                }
                else
                {
                    return GT_BAD_PARAM;
                }
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.epclGlobalConfig;
            break;
        default: return GT_BAD_PARAM;
    }

    /* for ingress and egress used the same configuration bites 1 and 18-16 */
    /* in ingress and in egress global configuration registers              */
    res = prvCpssDrvHwPpPortGroupReadRegister(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, &gloRegData);
    if (res != GT_OK)
    {
        return res;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* not as in Cheetah2 - only this bit relevant */
        nonLocalAccess = ((gloRegData & 2) == 0) ? GT_TRUE : GT_FALSE;
        switch (interfaceInfoPtr->type)
        {
            case CPSS_INTERFACE_PORT_E:
                if (nonLocalAccess == GT_FALSE)
                {
                    entryIndex = 1024 + interfaceInfoPtr->devPort.portNum;
                }
                else
                {
                    entryIndex = (interfaceInfoPtr->devPort.devNum * 32)
                        + interfaceInfoPtr->devPort.portNum;
                }
                break;
            case CPSS_INTERFACE_VID_E:
                if (interfaceInfoPtr->vlanId >= 1024)
                {
                    return GT_BAD_PARAM;
                }
                entryIndex = interfaceInfoPtr->vlanId;
                break;
            case CPSS_INTERFACE_TRUNK_E:
                if (nonLocalAccess == GT_FALSE)
                {
                    return GT_BAD_PARAM;
                }
                entryIndex = 1024 + interfaceInfoPtr->trunkId;
                break;
            /* all other types not relevant */
            default: return GT_BAD_PARAM;
        }

        *indexPtr = entryIndex;
        return GT_OK;
    }

    /* DXCH2 and above */
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* XCAT and above Ingress */
        if (direction == CPSS_PCL_DIRECTION_INGRESS_E)
        {
            nonLocalAccess = ((gloRegData & 4) != 0) ? GT_TRUE : GT_FALSE;
            portsPerDev    = ((gloRegData & (1 << 7)) != 0) ? 64 : 32;
            nonLocalBase   = (portsPerDev == 32)
                ? (1024 * ((gloRegData >> 3) & 3))
                : (2048 * ((gloRegData >> 3) & 1));
        }
        else
        {
            /* XCAT and above Egress */
            nonLocalAccess = ((gloRegData & 2) != 0) ? GT_TRUE : GT_FALSE;
            portsPerDev    = ((gloRegData & (1 << 18)) != 0) ? 64 : 32;
            nonLocalBase   = (2048 * ((gloRegData >> 16) & 1));
        }

    }
    else
    {
        /* CH1-3 Ingess and Egress */
        nonLocalAccess = ((gloRegData & 2) != 0) ? GT_TRUE : GT_FALSE;
        portsPerDev    = ((gloRegData & (1 << 18)) != 0) ? 64 : 32;
        nonLocalBase   = (portsPerDev == 32)
            ? (1024 * ((gloRegData >> 16) & 3))
            : (2048 * ((gloRegData >> 16) & 1));
    }



    switch (interfaceInfoPtr->type)
    {
        case CPSS_INTERFACE_PORT_E:
            if (nonLocalAccess == GT_FALSE)
            {
                /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
                *portGroupIdPtr = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,interfaceInfoPtr->devPort.portNum);
                localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,interfaceInfoPtr->devPort.portNum);
                entryIndex = 4096 + localPort;
            }
            else
            {
                entryIndex = nonLocalBase
                    + (interfaceInfoPtr->devPort.devNum * portsPerDev)
                    + interfaceInfoPtr->devPort.portNum;
            }
            break;
        case CPSS_INTERFACE_VID_E:
            if (interfaceInfoPtr->vlanId >= 4096)
            {
                return GT_BAD_PARAM;
            }

            entryIndex = interfaceInfoPtr->vlanId;
            break;
        case CPSS_INTERFACE_TRUNK_E:
            if (direction != CPSS_PCL_DIRECTION_INGRESS_E)
            {
                return GT_BAD_PARAM;
            }
            if (nonLocalAccess == GT_FALSE)
            {
                return GT_BAD_PARAM;
            }
            entryIndex = 4096 + interfaceInfoPtr->trunkId;
            break;
        /* all other types not relevant */
        default: return GT_BAD_PARAM;
    }

    *indexPtr = entryIndex;
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclCfgTblHwWrite
*
* DESCRIPTION:
*    Write PCL Configuration table entry (fields in HW format)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId            - port group Id , to support multi-port-groups device
*       direction         - Policy direction:
*                           Ingress or Egress
*       entryIndex        - entry Index
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       subLookupNum      - Sub lookup Number (APPLICABLE DEVICES: xCat2)
*       lookupCfgPtr      - HW lookup configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_TIMEOUT         - after max number of retries checking if PP ready
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclCfgTblHwWrite
(
    IN GT_U8                                  devNum,
    IN GT_U32                                 portGroupId,
    IN CPSS_PCL_DIRECTION_ENT                 direction,
    IN GT_U32                                 entryIndex,
    IN CPSS_PCL_LOOKUP_NUMBER_ENT             lookupNum,
    IN  GT_U32                                subLookupNum,
    IN PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC    *lookupCfgPtr
)
{
    GT_STATUS                  res;            /* return code          */
    PRV_PCL_CFG_LOOKUP_POS_STC *posPtr;        /* field layouts        */
    PRV_CPSS_DXCH_TABLE_ENT    tableId;        /* HW table Id          */
    GT_U32                     cfgEntry[2];    /* HW table entry data  */
    GT_U32                     fieldAmount;    /* amount of HW fields  */

    /* field value          */
    GT_U32  valueArray[PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_FIELDS_MAX_NUM_CNS];
    /* field offset         */
    GT_U32  offsetArray[PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_FIELDS_MAX_NUM_CNS];
    /* field length         */
    GT_U32  lenArray[PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_FIELDS_MAX_NUM_CNS];

    res = prvCpssDxChPclCfgTblHwSelectTblAndPos(
        devNum, direction, lookupNum, subLookupNum, &tableId, &posPtr);
    if (res != GT_OK)
    {
        return res;
    }

    fieldAmount = 0;

    valueArray[fieldAmount]  = lookupCfgPtr->enableLookup;
    offsetArray[fieldAmount] = posPtr->enableLookupOff;
    lenArray[fieldAmount]    = posPtr->enableLookupLen;
    fieldAmount ++;

    valueArray[fieldAmount]  = lookupCfgPtr->nonIpKeyType;
    offsetArray[fieldAmount] = posPtr->nonIpKeyTypeOff;
    lenArray[fieldAmount]    = posPtr->nonIpKeyTypeLen;
    fieldAmount ++;

    valueArray[fieldAmount]  = lookupCfgPtr->ipv4KeyType;
    offsetArray[fieldAmount] = posPtr->ipv4KeyTypeOff;
    lenArray[fieldAmount]    = posPtr->ipv4KeyTypeLen;
    fieldAmount ++;

    valueArray[fieldAmount]  = lookupCfgPtr->ipv6KeyType;
    offsetArray[fieldAmount] = posPtr->ipv6KeyTypeOff;
    lenArray[fieldAmount]    = posPtr->ipv6KeyTypeLen;
    fieldAmount ++;

    valueArray[fieldAmount]  = lookupCfgPtr->pclId;
    offsetArray[fieldAmount] = posPtr->pclIdOff;
    lenArray[fieldAmount]    = posPtr->pclIdLen;
    fieldAmount ++;

    valueArray[fieldAmount]  = lookupCfgPtr->dualLookup;
    offsetArray[fieldAmount] = posPtr->dualLookupOff;
    lenArray[fieldAmount]    = posPtr->dualLookupLen;
    fieldAmount ++;

    valueArray[fieldAmount]  = lookupCfgPtr->pclIdL01;
    offsetArray[fieldAmount] = posPtr->pclIdL01Off;
    lenArray[fieldAmount]    = posPtr->pclIdL01Len;
    fieldAmount ++;

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
        == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* enable the entry */
        valueArray[fieldAmount]  = 1;
        offsetArray[fieldAmount] = 28;
        lenArray[fieldAmount]    = 1;
        fieldAmount ++;
    }

    return prvCpssDxChPortGroupWriteTableEntryFieldList(
        devNum, portGroupId, tableId, entryIndex, cfgEntry,
        fieldAmount, offsetArray, lenArray, valueArray);
}

/*******************************************************************************
* prvCpssDxChPclCfgTblHwRead
*
* DESCRIPTION:
*    Read PCL Configuration table entry (fields in HW format)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - device number
*       portGroupId            - port group Id , to support multi-port-groups device
*       direction         - Policy direction:
*                           Ingress or Egress
*       entryIndex        - entry Index
*       lookupNum         - Lookup number:
*                           lookup0 or lookup1
*       subLookupNum      - Sub lookup Number (APPLICABLE DEVICES: xCat2)
*
* OUTPUTS:
*       lookupCfgPtr      - HW lookup configuration
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - one of the input parameters is not valid.
*       GT_TIMEOUT         - after max number of retries checking if PP ready
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclCfgTblHwRead
(
    IN  GT_U8                                  devNum,
    IN GT_U32                                 portGroupId,
    IN  CPSS_PCL_DIRECTION_ENT                 direction,
    IN  GT_U32                                 entryIndex,
    IN  CPSS_PCL_LOOKUP_NUMBER_ENT             lookupNum,
    IN  GT_U32                                 subLookupNum,
    OUT PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC    *lookupCfgPtr
)
{
    GT_STATUS                  res;            /* return code          */
    PRV_PCL_CFG_LOOKUP_POS_STC *posPtr;        /* field layouts        */
    PRV_CPSS_DXCH_TABLE_ENT    tableId;        /* HW table Id          */
    GT_U32                     cfgEntry[2];    /* HW table entry data  */
    GT_U32                     fieldAmount;    /* amount of HW fields  */

    /* field value          */
    GT_U32  valueArray[PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_FIELDS_MAX_NUM_CNS];
    /* field offset         */
    GT_U32  offsetArray[PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_FIELDS_MAX_NUM_CNS];
    /* field length         */
    GT_U32  lenArray[PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_FIELDS_MAX_NUM_CNS];

    res = prvCpssDxChPclCfgTblHwSelectTblAndPos(
        devNum, direction, lookupNum, subLookupNum, &tableId, &posPtr);
    if (res != GT_OK)
    {
        return res;
    }

    fieldAmount = 0;

    offsetArray[fieldAmount] = posPtr->enableLookupOff;
    lenArray[fieldAmount]    = posPtr->enableLookupLen;
    fieldAmount ++;

    offsetArray[fieldAmount] = posPtr->nonIpKeyTypeOff;
    lenArray[fieldAmount]    = posPtr->nonIpKeyTypeLen;
    fieldAmount ++;

    offsetArray[fieldAmount] = posPtr->ipv4KeyTypeOff;
    lenArray[fieldAmount]    = posPtr->ipv4KeyTypeLen;
    fieldAmount ++;

    offsetArray[fieldAmount] = posPtr->ipv6KeyTypeOff;
    lenArray[fieldAmount]    = posPtr->ipv6KeyTypeLen;
    fieldAmount ++;

    offsetArray[fieldAmount] = posPtr->pclIdOff;
    lenArray[fieldAmount]    = posPtr->pclIdLen;
    fieldAmount ++;

    offsetArray[fieldAmount] = posPtr->dualLookupOff;
    lenArray[fieldAmount]    = posPtr->dualLookupLen;
    fieldAmount ++;

    offsetArray[fieldAmount] = posPtr->pclIdL01Off;
    lenArray[fieldAmount]    = posPtr->pclIdL01Len;
    fieldAmount ++;

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* Cheetah Plus "entry-valid" bit */
        offsetArray[fieldAmount] = 28;
        lenArray[fieldAmount]    = 1;
        fieldAmount ++;
    }

    res = prvCpssDxChPortGroupReadTableEntryFieldList(
        devNum, portGroupId , tableId, entryIndex, cfgEntry,
        fieldAmount, offsetArray, lenArray, valueArray);
    if (res != GT_OK)
    {
        return res;
    }

    /* erase all, not relevant fields will not been updated */
    cpssOsMemSet(lookupCfgPtr, 0, sizeof(PRV_CPSS_DXCH_PCL_LOOKUP_CFG_HW_STC));

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
         == CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* check Cheetah Plus "entry-valid" bit */
        if (valueArray[fieldAmount - 1] == 0)
        {
            return GT_OK;
        }
    }

    lookupCfgPtr->enableLookup = (GT_U8)valueArray[0];
    lookupCfgPtr->nonIpKeyType = (GT_U8)valueArray[1];
    lookupCfgPtr->ipv4KeyType  = (GT_U8)valueArray[2];
    lookupCfgPtr->ipv6KeyType  = (GT_U8)valueArray[3];
    lookupCfgPtr->pclId        = (GT_U16)valueArray[4];
    lookupCfgPtr->dualLookup   = (GT_U8)valueArray[5];
    lookupCfgPtr->pclIdL01     = (GT_U16)valueArray[6];

    return  GT_OK;
}

/* CH3 specific utils */

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleDataXorYGet
*
* DESCRIPTION:
*       Reads standard rule X or Y data
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex      - index of rule
*       xORy           - GT_TRUE for X, GT_FALSE for Y.
*
* OUTPUTS:
*       dataPtr        - data:  6 words for X or Y
*                       For xCat2 and above devices - 7 words.
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclTcamStdRuleDataXorYGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       xORy,
    OUT GT_U32                        *dataPtr
)
{
    GT_U8        i;           /* loop index                              */
    GT_U32       data;        /* register data                           */
    GT_STATUS    rc;          /* return code                             */
    GT_U32       regAddr;     /* register address                        */

    for (i = 0; (i < 8); i++)
    {
        rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
            devNum, ruleIndex, i, xORy, &regAddr);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpPortGroupReadRegister(
            devNum, portGroupId, regAddr, &data);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (0 != PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
        {
            /* xCat2 and above */
            switch (i)
            {
                case 0: /* 30 bits */
                    dataPtr[0] =  ((data >> 2) & 0x3FFFFFFF);
                    break;
                case 1: /* 2 + 18 bits */
                    dataPtr[0] |= ((data & 3) << 30);
                    dataPtr[1] =  ((data >> 2) & 0x3FFFF);
                    break;
                case 2: /* 14 + 18 bits */
                    dataPtr[1] |= ((data & 0x3FFF) << 18);
                    dataPtr[2] =  ((data >> 14) & 0x3FFFF);
                    break;
                case 3: /* 14 + 6 bits */
                    dataPtr[2] |= ((data  & 0x3FFF) << 18);
                    dataPtr[3] =  ((data >> 14) & 0x3F);
                    break;
                case 4: /* 26 + 6 bits */
                    dataPtr[3] |=  ((data & 0x3FFFFFF) << 6);
                    dataPtr[4] =  ((data >> 26) & 0x3F);
                    break;
                case 5: /* 20 bits */
                    dataPtr[4] |=  ((data & 0xFFFFF) << 6);
                    break;
                case 6: /* 6 + 26 bits */
                    dataPtr[4] |= ((data & 0x3F) << 26);
                    dataPtr[5] =  ((data >> 6) & 0x3FFFFFF);
                    break;
                case 7: /* 6 + 14 bits*/
                    dataPtr[5] |= ((data & 0x3F) << 26);
                    dataPtr[6]  = ((data >> 6) & 0x3FFF);
                    break;

                default:
                        break;
            }
        }
        else
        {
            /* CH3, xCat, Lion */
            switch (i)
            {
                case 0: dataPtr[0] =  data; break;
                case 1: dataPtr[1] =  (data & 0x0000FFFF); break;
                case 2: dataPtr[1] |= (data << 16) & 0xFFFF0000;
                        dataPtr[2] =  (data >> 16) & 0x0000FFFF;
                        break;
                case 3: dataPtr[2] |= (data << 16) & 0xFFFF0000; break;
                case 4: dataPtr[3] =  data; break;
                case 5: dataPtr[4] =  (data & 0x0000FFFF);break;
                case 6: dataPtr[4] |= (data << 16) & 0xFFFF0000;
                        dataPtr[5] =  (data >> 16) & 0x0000FFFF;
                        break;
                case 7: dataPtr[5] |= (data << 16) & 0xFFFF0000; break;
    
                default:
                        break;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleDataXandYGet
*
* DESCRIPTION:
*       Reads standard rule X and Y data
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex      - index of rule
*
* OUTPUTS:
*       xPtr          - data:  6 words for X
*       yPtr          - data:  6 words for Y
*                       For xCat2 and above devices - 7 words.
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleDataXandYGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    OUT GT_U32                        *xPtr,
    OUT GT_U32                        *yPtr
)
{
    GT_STATUS    rc;          /* return code                             */

    rc = prvCpssDxChPclTcamStdRuleDataXorYGet(
            devNum, portGroupId, ruleIndex, GT_TRUE, xPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPclTcamStdRuleDataXorYGet(
            devNum, portGroupId, ruleIndex, GT_FALSE, yPtr);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleStateXorYGet
*
* DESCRIPTION:
*       Reads standard rule X or Y control bits of the 0-th 48-bit column of 4
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex      - index of rule
*       xORy           - GT_TRUE for X, GT_FALSE for Y.
*
* OUTPUTS:
*       dValidPtr     -  xValidPtr or yValidPtr
*       dSizePtr      -  xSizePtr or ySizePtr
*       dSparePtr     -  xSparePtr or ySparePtr
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclTcamStdRuleStateXorYGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       xORy,
    OUT GT_U32                        *dValidPtr,
    OUT GT_U32                        *dSizePtr,
    OUT GT_U32                        *dSparePtr
)
{
    GT_U32       data;        /* register data                           */
    GT_STATUS    rc;          /* return code                             */
    GT_U32       regAddr;     /* register address                        */
    GT_U32       validBitOffset; /* valid bit offset in control word     */
    GT_U32       spareBitOffset; /* spare bit offset in control word     */

    rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
            devNum, ruleIndex, 1, xORy, &regAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpPortGroupReadRegister(
        devNum, portGroupId, regAddr, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
             == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        validBitOffset = 17;
        spareBitOffset = 16;
    }
    else /* xCat */
    {
        validBitOffset = 16;
        spareBitOffset = 17;
    }

    *dSizePtr  = (data >> 18) & 3;
    *dValidPtr = (data >> validBitOffset) & 1;
    *dSparePtr = (data >> spareBitOffset) & 1;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleStateXandYGet
*
* DESCRIPTION:
*       Reads standard rule X and Y control bits of the 0-th 48-bit column of 4
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex      - index of rule
*
* OUTPUTS:
*       xValidPtr     -  xValidPtr
*       yValidPtr     -  yValidPtr
*       xSizePtr      -  xSizePtr
*       ySizePtr      -  ySizePtr
*       xSparePtr     -  xSparePtr
*       ySparePtr     -  ySparePtr
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleStateXandYGet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    OUT GT_U32                        *xValidPtr,
    OUT GT_U32                        *yValidPtr,
    OUT GT_U32                        *xSizePtr,
    OUT GT_U32                        *ySizePtr,
    OUT GT_U32                        *xSparePtr,
    OUT GT_U32                        *ySparePtr
)
{
    GT_STATUS    rc;          /* return code                             */

    rc = prvCpssDxChPclTcamStdRuleStateXorYGet(
            devNum, portGroupId, ruleIndex, GT_TRUE,
            xValidPtr, xSizePtr, xSparePtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPclTcamStdRuleStateXorYGet(
            devNum, portGroupId, ruleIndex, GT_FALSE,
            yValidPtr, ySizePtr, ySparePtr);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleValidStateXorYSet
*
* DESCRIPTION:
*       Writes standard rule X or Y valid bit of the 0-th 48-bit column of 4
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        does not support value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex      - index of rule
*       xORy           - GT_TRUE for X, GT_FALSE for Y.
*       dValid         - xValid or yValid
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclTcamStdRuleValidStateXorYSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       xORy,
    IN  GT_U32                        dValid
)
{
    GT_STATUS    rc;          /* return code                             */
    GT_U32       regAddrData; /* register address                        */
    GT_U32       regAddrCtrl; /* register address                        */
    GT_U32       validBitOffset; /* valid bit offset in control word     */
    GT_U32       regData;     /* register data                           */
    GT_U32       regCtrl;     /* register control                        */

    rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
            devNum, ruleIndex, 0, xORy, &regAddrData);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
            devNum, ruleIndex, 1, xORy, &regAddrCtrl);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
             == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        validBitOffset = 17;
    }
    else /* xCat */
    {
        validBitOffset = 16;
    }

    rc = prvCpssDrvHwPpPortGroupReadRegister(
        devNum, portGroupId, regAddrData, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpPortGroupReadRegister(
        devNum, portGroupId, regAddrCtrl, &regCtrl);
    if (rc != GT_OK)
    {
        return rc;
    }

    regCtrl &= (~ (1 << validBitOffset));
    regCtrl |= (dValid << validBitOffset);

    /* rewrite the same data without change           */
    /* must be done before the control register write */
    /* that triggers the TCAM update                  */

    rc = prvCpssDrvHwPpPortGroupWriteRegister(
        devNum, portGroupId, regAddrData, regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* verify that all words were written before write last one.
        because last one triggers write of whole entry */
    GT_SYNC;

    rc = prvCpssDrvHwPpPortGroupWriteRegister(
        devNum, portGroupId, regAddrCtrl, regCtrl);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleValidStateXandYSet
*
* DESCRIPTION:
*       Writes standard rule X and Y valid bits of the 0-th 48-bit column of 4
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex      - index of rule
*       xValid         -  xValid
*       yValid         -  yValid
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleValidStateXandYSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_U32                        xValid,
    IN  GT_U32                        yValid
)
{
    GT_STATUS    rc;          /* return code                             */

    rc = prvCpssDxChPclTcamStdRuleValidStateXorYSet(
            devNum, portGroupId, ruleIndex, GT_TRUE, xValid);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPclTcamStdRuleValidStateXorYSet(
            devNum, portGroupId, ruleIndex, GT_FALSE, yValid);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleValidStateSet
*
* DESCRIPTION:
*       Writes standard rule X and Y valid bits
*       The algorithms for xCat2 and above and
*       the algorithm for DxCh3, xCat, Lion are different.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum         - device number
*       portGroupId         - The port group Id. relevant only to 'multi-port-groups' devices.
*                        supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex      - index of rule
*       valid          -  valid
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleValidStateSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       valid
)
{
    GT_STATUS    rc;          /* return code         */
    GT_U32       xValid;      /* xValid              */
    GT_U32       yValid;      /* yValid              */
    GT_U32       regAddr;     /* register address    */
    GT_U32       regData[2];  /* register data       */
    GT_U32       i;           /* loop index          */
    GT_U32       yBitsWrite;  /* Y bits to write     */


    if (0 == PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* DxCh3; xCat; Lion; */
        if (valid == GT_FALSE)
        {
            /* any "valid bit" value will not be matched in the key */
            xValid = PRV_CPSS_DXCH_PCL_MP_TO_X_MAC(0, 1);
            yValid = PRV_CPSS_DXCH_PCL_MP_TO_Y_MAC(0, 1);
        }
        else
        {
            /* "valid bit" value 1 only will be matched in the key */
            xValid = PRV_CPSS_DXCH_PCL_MP_TO_X_MAC(1, 1);
            yValid = PRV_CPSS_DXCH_PCL_MP_TO_Y_MAC(1, 1);
        }

        return prvCpssDxChPclTcamStdRuleValidStateXandYSet(
            devNum, portGroupId, ruleIndex, xValid, yValid);
    }

    /* xCat2 and above */

    /* calculate new value for Y-bits for rule size (2 bits) */
    /* the X-bits of the rule never updated                  */

    /* The yBitsWrite here gets all 32 bits                  */
    /* The proper 2 bits that contain rule size will be      */
    /* cut off it below - at bit-replacing algorithm         */

    if (valid == GT_FALSE)
    {
        /* For the rule written to TCAM by CPSS         */
        /* X bits of rule size can contain 1,2 or 3     */
        /* in any case at least one pare (x==1 && y==1) */
        /* i.e. never-match pair will be created.       */
        /* Even in the case of the garbage in TCAM if   */
        /* X-bits of the rule size contain 0, the       */
        /* updated rule will match only keys that       */
        /* contain 0 in rule size bits - that cannot be.*/

        yBitsWrite = 0xFFFFFFFF;
    }
    else
    {
        rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
                devNum, ruleIndex, 0, GT_TRUE, &regAddr);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDrvHwPpPortGroupReadRegister(
            devNum, portGroupId, regAddr, &(regData[0]));
        if (rc != GT_OK)
        {
            return rc;
        }

        /* exact matching of the first 32 X - bits */
        yBitsWrite = (~ regData[0]);
    }

    /* Update the Y-bits of rule size                            */
    /* Two words must be written to TCAM.                        */
    /* The second word not changed but triggers the TCAM update. */

    rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
            devNum, ruleIndex, 0, GT_FALSE, &regAddr);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0; (i < 2); i++)
    {
        rc = prvCpssDrvHwPpPortGroupReadRegister(
            devNum, portGroupId, (regAddr + (4 * i)), &(regData[i]));
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* Replacing two Y bits at the position of the size of rule */
    regData[0] &= (~ 3);
    regData[0] |= (yBitsWrite & 3);

    rc = prvCpssDxChPclWriteTabEntry(
        devNum, portGroupId,
        regAddr, 2, regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleXorYSet
*
* DESCRIPTION:
*       Writes standard rule X or Y data and control bits
*       The first 48-bit column control bits supposed to be invalid and will
*       be overwritten last
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupId        - The port group Id. relevant only to 'multi-port-groups' devices.
*                       supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex     - index of rule
*       xORy          - GT_TRUE for X, GT_FALSE for Y.
*       dPtr          - data:  6 words for X or Y
*       dValid0       - xValid or yValid bit of the word0
*       dValid        - xValid or yValid of words 1-5
*       dSize         - xSize or ySize
*       dSpare        - xSpare or ySpare
*
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPclTcamStdRuleXorYSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       xORy,
    IN  GT_U32                        *dPtr,
    IN  GT_U32                        dValid0,
    IN  GT_U32                        dValid,
    IN  GT_U32                        dSize,
    IN  GT_U32                        dSpare
)
{
    GT_32        i;           /* loop index                              */
    GT_32        wordIndex;   /* word index derived from the loop index  */
    GT_U32       data;        /* word register data                      */
    GT_U32       ctrl;        /* control register data                   */
    GT_STATUS    rc;          /* return code                             */
    GT_U32       regAddrData; /* data register address                   */
    GT_U32       regAddrCtrl; /* data register address                   */
    GT_U32       validBitOffset; /* valid bit offset in control word     */
    GT_U32       spareBitOffset; /* spare bit offset in control word     */

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily
             == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        validBitOffset = 17;
        spareBitOffset = 16;
    }
    else /* xCat */
    {
        validBitOffset = 16;
        spareBitOffset = 17;
    }

    /* size and spare control bits are the same for all segments. */
    /* valid control bit is the same for all segments when setting rule to valid status, when setting   */
    /* rule to invalid status it is enough to invalid one segment, in our case the first one is choosen */
    ctrl = ((dSize & 3) << 18) | ((dSpare & 1) << spareBitOffset) | ((dValid0 & 1) << validBitOffset);

    /* during each period two words are written. The first contains only data, while */
    /* the second contains data, control bits and triggers the TCAM write operation. */
    for (i = 0; (i < 8) ; i+=2)
    {
        /* descending or ascending order for valid or invalid operation respectively. */
        /* When setting valid status the first segment is written last.               */
        /* When setting invalid status the first segment is written first.            */
        /* Setting invalid status is done by setting invalid for the first segment only, */
        /* therefore when all valid bits have the same value we are in the case of       */
        /* setting valid status to the rule, when invalid status is requested the valid  */
        /* bit value of the first segment is opposite to the other segments valid bits   */
        /* value.                                                                        */
        wordIndex = ( dValid0 == dValid ) ? ( 6 - i ) : i ;
        ctrl &= 0xFFFF0000;

        switch(wordIndex)
        {
            case 0: data =  dPtr[0];
                    ctrl |= (dPtr[1] & 0x000FFFF);
                    break;
            case 2: /* In ascending order, we previously set the valid bit of the first */
                    /* segment to invalid status, and from now on valid status is set   */
                    /* for the other segments.                                          */
                    /* In descending order, used when valid status is set for all       */
                    /* segments valid bit setting has no influence since                */
                    /* dValid0 == dValid.                                               */
                    data =  (dPtr[1] >> 16) & 0x0000FFFF;
                    data |= (dPtr[2] << 16) & 0xFFFF0000;
                    ctrl |= (dPtr[2] >> 16) & 0x0000FFFF;
                    ctrl &= (~(1 << validBitOffset));
                    ctrl |= ((dValid & 1) << validBitOffset);
                    break;
            case 4: data =  dPtr[3];
                    ctrl |= (dPtr[4] & 0x0000FFFF);
                    break;
            case 6: data =  (dPtr[4] >> 16) & 0x0000FFFF;
                    data |= (dPtr[5] << 16) & 0xFFFF0000;
                    ctrl |= (dPtr[5] >> 16) & 0x0000FFFF;
                    break;

            default: return GT_FAIL; /* this code should never be executed */
        }

        rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
            devNum, ruleIndex, wordIndex, xORy, &regAddrData);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
            devNum, ruleIndex, (wordIndex + 1), xORy, &regAddrCtrl);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDrvHwPpPortGroupWriteRegister(
            devNum, portGroupId, regAddrData, data);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* verify that all words were written before write last one.
            because last one triggers write of whole entry */
        GT_SYNC;

        rc = prvCpssDrvHwPpPortGroupWriteRegister(
            devNum, portGroupId, regAddrCtrl, ctrl);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleXandYSet
*
* DESCRIPTION:
*       Writes standard rule X and Y data and control bits
*       The first 48-bit column control bits supposed to be invalid and will
*       be overwritten last
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2.
*
* INPUTS:
*       devNum        - device number
*       portGroupId        - The port group Id. relevant only to 'multi-port-groups' devices.
*                       supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex     - index of rule
*       xPtr          - data:  6 words for X
*       yPtr          - data:  6 words for Y
*       xValid0       - xValid0 bit of the word0
*       yValid0       - yValid0 bit of the word0
*       xValid        - xValid  bit of the words 1-5
*       yValid        - yValid  bit of the words 1-5
*       xSize         - xSize
*       ySize         - ySize
*       xSpare        - xSpare
*       ySpare        - ySpare
*
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleXandYSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_U32                        *xPtr,
    IN  GT_U32                        *yPtr,
    IN  GT_U32                        xValid0,
    IN  GT_U32                        yValid0,
    IN  GT_U32                        xValid,
    IN  GT_U32                        yValid,
    IN  GT_U32                        xSize,
    IN  GT_U32                        ySize,
    IN  GT_U32                        xSpare,
    IN  GT_U32                        ySpare
)
{
    GT_STATUS    rc;          /* return code                             */

    rc = prvCpssDxChPclTcamStdRuleXorYSet(
            devNum, portGroupId, ruleIndex, GT_TRUE,
            xPtr, xValid0, xValid, xSize, xSpare);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPclTcamStdRuleXorYSet(
            devNum, portGroupId, ruleIndex, GT_FALSE,
            yPtr, yValid0, yValid, ySize, ySpare);

    return rc;
}

/*******************************************************************************
* prvCpssDxChPclTcamStdRuleSizeAndValidSet
*
* DESCRIPTION:
*       Writes standard rule X and Y data and control bits
*       The previous TCAM rule state supposed to be invalid and will
*       be overwritten last.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum        - device number
*       portGroupId        - The port group Id. relevant only to 'multi-port-groups' devices.
*                       supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       ruleIndex     - index of rule
*       xPtr          - data:  6 words for X
*       yPtr          - data:  6 words for Y
*                       xPtr and yPtr for xCat2 and above devices are 7 words
*                       The contain 206-bit data starting with pclId and
*                       not contain the 2-bit rule size.
*       validRule     - valid
*       ruleSize      - rule Size (1-3)
*
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamStdRuleSizeAndValidSet
(
    IN  GT_U8                         devNum,
    IN  GT_U32                        portGroupId,
    IN  GT_U32                        ruleIndex,
    IN  GT_U32                        *xPtr,
    IN  GT_U32                        *yPtr,
    IN  GT_BOOL                       validRule,
    IN  GT_U32                        ruleSize
)
{
    GT_STATUS    rc;           /* return cose                 */
    GT_U32       hwRuleSize;   /* hw Rule Size                */
    GT_U32       xValid;       /* xValid                      */
    GT_U32       yValid;       /* yValid                      */
    GT_U32       yRuleSize;    /* 2-bit Y-value for rule size */
    GT_U32       i;            /* loop index                  */
    GT_U32       xData[2];     /* X data to write             */
    GT_U32       yData[2];     /* Y data to write             */
    GT_U32       xRegBase;     /* regiser address base fo X   */
    GT_U32       yRegBase;     /* regiser address base fo Y   */

    switch (ruleSize)
    {
        case 1: hwRuleSize = 1; break;
        case 2: hwRuleSize = 2; break;
        case 3: hwRuleSize = 3; break;
        default: return GT_BAD_PARAM;
    }

    if (0 == PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* CH3, xCat, Lion */
        if (validRule == GT_FALSE)
        {
            /* any "valid bit" value will not be matched in the key */
            xValid = PRV_CPSS_DXCH_PCL_MP_TO_X_MAC(0, 1);
            yValid = PRV_CPSS_DXCH_PCL_MP_TO_Y_MAC(0, 1);
        }
        else
        {
            /* "valid bit" value 1 only will be matched in the key */
            xValid = PRV_CPSS_DXCH_PCL_MP_TO_X_MAC(1, 1);
            yValid = PRV_CPSS_DXCH_PCL_MP_TO_Y_MAC(1, 1);
        }

        return prvCpssDxChPclTcamStdRuleXandYSet(
            devNum, portGroupId, ruleIndex, xPtr, yPtr,
            xValid /*xValid0*/, yValid /*yValid0*/,
            PRV_CPSS_DXCH_PCL_MP_TO_X_MAC(1, 1) /*xValid*/,
            PRV_CPSS_DXCH_PCL_MP_TO_Y_MAC(1, 1) /*yValid*/,
            PRV_CPSS_DXCH_PCL_MP_TO_X_MAC(3, hwRuleSize) /*xSize*/,
            PRV_CPSS_DXCH_PCL_MP_TO_Y_MAC(3, hwRuleSize) /*ySize*/,
            PRV_CPSS_DXCH_PCL_MP_TO_X_MAC(0, 0) /*xSpare*/,
            PRV_CPSS_DXCH_PCL_MP_TO_Y_MAC(0, 0) /*xSpare*/);
    }

    /* xCat2 and above */

    /* if Y==3 provides at least of one pair <x==1&&y==1> i.e. never match */
    /* the hwRuleSize always written to X rule size bits                   */
    /* hwRuleSize == PRV_CPSS_DXCH_PCL_MP_TO_X_MAC(3, hwRuleSize)          */
    yRuleSize = (validRule == GT_FALSE) ? 3
        : PRV_CPSS_DXCH_PCL_MP_TO_Y_MAC(3, hwRuleSize);

    rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
        devNum, ruleIndex, 0, GT_TRUE, &xRegBase);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPclTcamReadStdRuleAddrGet(
        devNum, ruleIndex, 0, GT_FALSE, &yRegBase);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* writing 52-bit groups 3,2,1,0 */
    /* 7 words contain 32+32+32+32+32+32+14 == 206 bits */

    for (i = 4; (i > 0); i--)
    {
        switch (i)
        {
            case 4:
                /* the 3-th 52 bit - 6+26 + 6+14 */
                xData[0] = ((xPtr[4] >> 26) & 0x3F) | ((xPtr[5] & 0x3FFFFFF) << 6);
                yData[0] = ((yPtr[4] >> 26) & 0x3F) | ((yPtr[5] & 0x3FFFFFF) << 6);
                xData[1] = ((xPtr[5] >> 26) & 0x3F) | ((xPtr[6] & 0x3FFF) << 6);
                yData[1] = ((yPtr[5] >> 26) & 0x3F) | ((yPtr[6] & 0x3FFF) << 6);
                break;
            case 3:
                /* the 2-th 52 bit - 26+6 + 20 */
                xData[0] = ((xPtr[3] >> 6) & 0x3FFFFFF) | ((xPtr[4] & 0x3F) << 26);
                yData[0] = ((yPtr[3] >> 6) & 0x3FFFFFF) | ((yPtr[4] & 0x3F) << 26);
                xData[1] = ((xPtr[4] >> 6) & 0xFFFFF);
                yData[1] = ((yPtr[4] >> 6) & 0xFFFFF);
                break;
            case 2:
                /* the 1-th 52 bit - 14+18 + 14+6 */
                xData[0] = ((xPtr[1] >> 18) & 0x3FFF) | ((xPtr[2] & 0x3FFFF) << 14);
                yData[0] = ((yPtr[1] >> 18) & 0x3FFF) | ((yPtr[2] & 0x3FFFF) << 14);
                xData[1] = ((xPtr[2] >> 18) & 0x3FFF) | ((xPtr[3] & 0x3F) << 14);
                yData[1] = ((yPtr[2] >> 18) & 0x3FFF) | ((yPtr[3] & 0x3F) << 14);
                break;
            case 1:
                /* the 0-th 52 bit - 2+30 && 2+18 */
                xData[0] = hwRuleSize | (xPtr[0] << 2);
                yData[0] = yRuleSize  | (yPtr[0] << 2);
                xData[1] = ((xPtr[0] >> 30) & 3) | ((xPtr[1] & 0x3FFFF) << 2);
                yData[1] = ((yPtr[0] >> 30) & 3) | ((yPtr[1] & 0x3FFFF) << 2);
                break;
            default: break;
        }

        /* write 52 X-bits */
        rc = prvCpssDxChPclWriteTabEntry(
            devNum, portGroupId,
            (xRegBase + ((2 * (i - 1)) * 4)), 2, xData);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* write 52 Y-bits */
        rc = prvCpssDxChPclWriteTabEntry(
            devNum, portGroupId,
            (yRegBase + ((2 * (i - 1)) * 4)), 2, yData);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclTcamRuleModifiedWrite
*
* DESCRIPTION:
*       Writes rule Action, Mask and Pattern to TCAM.
*       Supports rules of any sizes.
*       Supports SrcPort field modification for multi port group devices
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number
*       portGroupsBmp    - bitmap of Port Groups.
*                        NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ruleSize       - sizeof rule, measured in standard rules
*       ruleIndex      - index of rule  - owner of action to read
*       egressRule     - GT_TRUE - egress Rule, GT_FALSE ingress
*       validRule      - GT_TRUE - valid, GT_FALSE - invalid
*       waitPrevReady  - GT_TRUE - to wait that previous operation finished
*                        relevant only for DxCh1 and DxCh2 devices,
*                        ignored for DxCh3 and above devices
*       actionPtr      - action
*       maskPtr        - mask
*       patternPtr     - pattern
* OUTPUTS:
*
* RETURNS :
*       GT_OK           - on success.
*       GT_FAIL         - on failure.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclTcamRuleModifiedWrite
(
    IN  GT_U8                         devNum,
    IN  GT_PORT_GROUPS_BMP            portGroupsBmp,
    IN  GT_U32                        ruleSize,
    IN  GT_U32                        ruleIndex,
    IN  GT_BOOL                       egressRule,
    IN  GT_BOOL                       validRule,
    IN  GT_BOOL                       waitPrevReady,
    IN  GT_U32                        *actionPtr,
    IN  GT_U32                        *maskPtr,
    IN  GT_U32                        *patternPtr
)
{
    GT_STATUS    rc;            /* return code                        */
    GT_BOOL      multiPortGroupModify;/* to modify rule for different port groups*/
    GT_U32       srcPortMask;      /* src Port Mask                   */
    GT_U32       srcPortPattern;   /* src Port Pattern                */
    GT_U32       portGroupIdMask;       /* port group Id Mask                    */
    GT_U32       portGroupIdPattern;    /* port group Id  Pattern                */
    GT_U32       portGroupId;           /* port group Id                         */

    /* initialization to avoid compiler warnings */
    srcPortMask    = 0;
    srcPortPattern = 0;

    /* default value - write without multi port group modify */
    multiPortGroupModify = GT_FALSE;

    if ((PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        && (egressRule == GT_FALSE))
    {
        /* default value - write with multi port group modify */
        /* overridden due several source port           */
        /* mask anf pattern conditions                  */
        multiPortGroupModify = GT_TRUE;

        srcPortMask    = ((maskPtr[0] >> 11) & 0x3F);
        srcPortPattern = ((patternPtr[0] >> 11) & 0x3F);

        if ((srcPortPattern & srcPortMask) != srcPortPattern)
        {
            /* if there is a bit (in this or any other field) */
            /* that it's mask == 0 and it's pattern == 1      */
            /* this rule does not match any key               */
            /* Must be replaced by "srcPort==NULL_PORT" rule  */

            /* clear srcPort bits in mask and pattern */
            maskPtr[0]    &= (~ (0x3F << 11));
            patternPtr[0] &= (~ (0x3F << 11));
            /* require srcPort to be NULL port */
            maskPtr[0]    |= (0x3F << 11);
            patternPtr[0] |= (PRV_CPSS_DXCH_NULL_PORT_NUM_CNS << 11);

            /* write port group unaware */
            multiPortGroupModify = GT_FALSE;
        }

        if ((srcPortPattern & 0x3C) == 0x3C)
        {
            /* big srcPort bits matched - CPU port or NULL port */
            multiPortGroupModify = GT_FALSE;
        }

        if (srcPortMask == 0)
        {
            /* source port field - "don't care" */
            multiPortGroupModify = GT_FALSE;
        }
    }

    if (multiPortGroupModify == GT_FALSE)
    {
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)
        {
            /* write rule to all port groups without modification */
            rc = prvCpssDxChPclTcamRuleWrite(
                devNum, portGroupId,
                ruleSize, ruleIndex, validRule,
                waitPrevReady, actionPtr, maskPtr, patternPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
            devNum, portGroupsBmp, portGroupId)

        return GT_OK;
    }

    /* Multi port group modifying write */

    portGroupIdMask     = (srcPortMask >> 4) & 0x03;
    portGroupIdPattern  = (srcPortPattern >> 4) & 0x03;
    /* add high bits to require srcPort to be port-group-local */
    srcPortMask    |= 0x30;
    /* truncate pattern to port-group-local port value */
    srcPortPattern &= 0x0F;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* clear srcPort bits in mask and pattern */
        maskPtr[0]    &= (~ (0x3F << 11));
        patternPtr[0] &= (~ (0x3F << 11));

        if ((portGroupIdMask & portGroupId) == portGroupIdPattern)
        {
            /* put mask and pattern */
            maskPtr[0]    |= (srcPortMask << 11);
            patternPtr[0] |= (srcPortPattern << 11);
        }
        else
        {
            /* require srcPort to be NULL port */
            maskPtr[0]    |= (0x3F << 11);
            patternPtr[0] |= (PRV_CPSS_DXCH_NULL_PORT_NUM_CNS << 11);
        }

        rc = prvCpssDxChPclTcamRuleWrite(
            devNum, portGroupId, ruleSize, ruleIndex, validRule,
            waitPrevReady, actionPtr, maskPtr, patternPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclRuleDataXCat2Adjust
*
* DESCRIPTION:
*   The function adjusts the data converted from SW to HW format
*   to xCat2 specific HW format: 7 words per standard rule.
*   An algorithm contains 3 stages:
*   1) Drugging high rule segments by such way that each 6-word segment
*      will be padded by the 7-th word contains zero.
*   2) For egress rules the first segment of the rule left shifted to 1
*      because the "bit0 == valid" not exists for xCat2 and above devices.
*   3) If portListEnabled TRUE - clear the mask and pattern of all bits
*                that will be overridden by portListBmp field
*                are zeros and write portListBmp fields
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       addedFldPtr      - xCat2 specific data
*       hwMaskArr        - array with mask converted to HW format
*       hwPatternArr     - array with pattern converted to HW format
*
* OUTPUTS:
*       hwMaskArr        - array with mask adjusted for xCat2
*       hwPatternArr     - array with pattern adjusted for xCat2
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on wrong addedFldPtr data
*
* COMMENTS:
*   The xCat2 specific "key size" bits are added at low level write
*   function prvCpssDxChPclTcamStdRuleSizeAndValidSet.
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleDataXCat2Adjust
(
    IN     PRV_CPSS_DXCH_PCL_XCAT2_RULE_ADD_FLD_STC *addedFldPtr,
    INOUT  GT_U32                                   hwMaskArr[],
    INOUT  GT_U32                                   hwPatternArr[]
)
{
    GT_U32 segIdx;       /* index of rule segment                          */
    GT_U32 wordIdx;      /* index of word                                  */
    GT_U32 segOldBase;   /* index of the old base word of the rule segment */
    GT_U32 segNewBase;   /* index of the new base word of the rule segment */
    GT_U32 i, j;         /* loop index                                     */

    /* move all segments with index more then 0, the 0-th not moved */
    for (i = 1; (i < addedFldPtr->ruleSize); i++)
    {
        segIdx = addedFldPtr->ruleSize - i;
        segOldBase = segIdx * 6;
        segNewBase = segIdx * 7;
        for (j = 0; (j <= 5); j++)
        {
            wordIdx = 5 - j;
            hwMaskArr[segNewBase + wordIdx] =
                hwMaskArr[segOldBase + wordIdx];
            hwPatternArr[segNewBase + wordIdx] =
                hwPatternArr[segOldBase + wordIdx];
        }
        /* clear the additional 7-th word */
        hwMaskArr[segNewBase + 6] = 0;
        hwPatternArr[segNewBase + 6] = 0;
    }
    /* clear the additional 7-th word of the first segment */
    hwMaskArr[6] = 0;
    hwPatternArr[6] = 0;

    /* shift the first segment of egress rule*/
    if (addedFldPtr->isEgress != GT_FALSE)
    {
        for (j = 0; (j <= 5); j++)
        {
            hwMaskArr[j] =
                (((hwMaskArr[j] >> 1) & 0x7FFFFFFF)
                | ((hwMaskArr[j + 1] & 1) << 31));
            hwPatternArr[j] =
                (((hwPatternArr[j] >> 1) & 0x7FFFFFFF)
                | ((hwPatternArr[j + 1] & 1) << 31));
        }
    }

    if (addedFldPtr->portListEnabled != GT_FALSE)
    {
        /* clear the mask and pattern of portListBmp not overlapped with */
        /* masks and patterns of other fields                            */

        /* portListBmp[7:0] is a rule[7:0] */
        hwMaskArr[0]    &= (~ 0xFF);
        hwPatternArr[0] &= (~ 0xFF);

        if (addedFldPtr->isEgress == GT_FALSE)
        {
            /* ingress PCL */
            /* portListBmp[13:8] is a rule[16:11] */
            hwMaskArr[0]    &= (~ 0x1F800);
            hwPatternArr[0] &= (~ 0x1F800);
        }
        else
        {
            /* egress PCL */
            /* portListBmp[13:8] is a rule[37:32] */
            hwMaskArr[1]    &= (~ 0x3F);
            hwPatternArr[1] &= (~ 0x3F);
        }

        /* portListBmp[27:14] is a rule[205:192] */
        hwMaskArr[6]    &= (~ 0x3FFF);
        hwPatternArr[6] &= (~ 0x3FFF);

        /* OR portListBmp with the data */

        /* portListBmp[7:0] to rule[7:0] */
        hwMaskArr[0] |= (addedFldPtr->portListBmpMask & 0xFF);
        hwPatternArr[0] |= (addedFldPtr->portListBmpPattern & 0xFF);

        if (addedFldPtr->isEgress == GT_FALSE)
        {
            /* ingress PCL */
            /* portListBmp[13:8] to rule[16:11] */
            hwMaskArr[0] |= ((addedFldPtr->portListBmpMask & 0x3F00) << 3);
            hwPatternArr[0] |= ((addedFldPtr->portListBmpPattern & 0x3F00) << 3);
        }
        else
        {
            /* egress PCL */
            /* portListBmp[13:8] to rule[37:32] */
            hwMaskArr[1] |= ((addedFldPtr->portListBmpMask & 0x3F00) >> 8);
            hwPatternArr[1] |= ((addedFldPtr->portListBmpPattern & 0x3F00) >> 8);
        }

        /* portListBmp[27:14] to rule[205:192] */
        hwMaskArr[6] |= ((addedFldPtr->portListBmpMask >> 14) & 0x3FFF);
        hwPatternArr[6] |= ((addedFldPtr->portListBmpPattern >> 14) & 0x3FFF);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPclRuleDataSw2HwConvert
*
* DESCRIPTION:
*   The function Converts the Policy Rule Mask, Pattern and Action
*   to HW format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       ruleFormat       - format of the Rule.
*       ruleOptionsBmp   - Bitmap of rule's options.
*                          The CPSS_DXCH_PCL_RULE_OPTION_ENT defines meaning of each bit.
*                          Samples:
*                            ruleOptionsBmp = 0 - no options are defined.
*                               Write rule to TCAM not using port-list format.
*                               Rule state is valid.
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E -
*                               write all fields of rule to TCAM but rule's
*                               state is invalid (no match during lookups).
*                            ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E |
*                                             CPSS_DXCH_PCL_RULE_OPTION_PORT_LIST_ENABLED_E -
*                               write all fields of rule to TCAM using port-list format
*                               but rule's state is invalid
*                              (no match during lookups).
*       maskPtr          - rule mask. The rule mask is AND styled one. Mask
*                          bit's 0 means don't care bit (corresponding bit in
*                          the pattern is not using in the TCAM lookup).
*                          Mask bit's 1 means that corresponding bit in the
*                          pattern is using in the TCAM lookup.
*                          The format of mask is defined by ruleFormat
*
*       patternPtr       - rule pattern.
*                          The format of pattern is defined by ruleFormat
*
*       actionPtr        - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* OUTPUTS:
*       hwMaskArr        - array for mask converted to HW format
*       hwPatternArr     - array for pattern converted to HW format
*       hwActionArr      - array for action converted to HW format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChPclRuleDataSw2HwConvert
(
    IN   GT_U8                              devNum,
    IN   CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT ruleFormat,
    IN   CPSS_DXCH_PCL_RULE_OPTION_ENT      ruleOptionsBmp,
    IN   CPSS_DXCH_PCL_RULE_FORMAT_UNT      *maskPtr,
    IN   CPSS_DXCH_PCL_RULE_FORMAT_UNT      *patternPtr,
    IN   CPSS_DXCH_PCL_ACTION_STC           *actionPtr,
    OUT  GT_U32                             hwMaskArr[],
    OUT  GT_U32                             hwPatternArr[],
    OUT  GT_U32                             hwActionArr[]
)
{
    GT_STATUS    rc;                   /* return code                 */
    GT_U8        egrPacketTypePattern; /* egress Packet Type  Pattern */
    GT_U8        egrPacketTypeMask;    /* egress Packet Type  Mask    */
    PRV_CPSS_DXCH_PCL_XCAT2_RULE_ADD_FLD_STC xCat2AddFld; /* xCat2 Add port-list related data */
    GT_U32       encapHighBitMask;     /* the mask of high bit of encapsulation     */
    GT_U32       encapHighBitPattern;  /* the pattern of high bit of encapsulation  */
    GT_U32       encapHighBitWord;     /* the word num of high bit of encapsulation */
    GT_U32       encapHighBitPos;      /* the position of high bit of encapsulation */
    GT_U32       workMask;             /* the mask of high bit of encapsulation     */
    GT_U32       workPattern;          /* the pattern of high bit of encapsulation  */

    /* default - wrong value, true values 0-3 */
    egrPacketTypePattern = 0xFF;
    egrPacketTypeMask    = 0xFF;

    switch (ruleFormat)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
            egrPacketTypePattern =
                patternPtr->ruleEgrStdNotIp.common.egrPacketType;
            egrPacketTypeMask =
                maskPtr->ruleEgrStdNotIp.common.egrPacketType;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
            egrPacketTypePattern =
                patternPtr->ruleEgrStdIpL2Qos.common.egrPacketType;
            egrPacketTypeMask =
                maskPtr->ruleEgrStdIpL2Qos.common.egrPacketType;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
            egrPacketTypePattern =
                patternPtr->ruleEgrStdIpv4L4.common.egrPacketType;
            egrPacketTypeMask =
                maskPtr->ruleEgrStdIpv4L4.common.egrPacketType;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
            egrPacketTypePattern =
                patternPtr->ruleEgrExtNotIpv6.common.egrPacketType;
            egrPacketTypeMask =
                maskPtr->ruleEgrExtNotIpv6.common.egrPacketType;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
            egrPacketTypePattern =
                patternPtr->ruleEgrExtIpv6L2.common.egrPacketType;
            egrPacketTypeMask =
                maskPtr->ruleEgrExtIpv6L2.common.egrPacketType;
            break;
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
            egrPacketTypePattern =
                patternPtr->ruleEgrExtIpv6L4.common.egrPacketType;
            egrPacketTypeMask =
                maskPtr->ruleEgrExtIpv6L4.common.egrPacketType;
            break;
        default: break;
    }

    /* truncate out-of-range bits */
    egrPacketTypeMask    &= 3;
    egrPacketTypePattern &= 3;

    if (egrPacketTypeMask != 3)
    {
        /* more then one packet type matched - ingnore type-dependend-data */
        egrPacketTypePattern = 0xFF;
    }

    /* convert action to HW format */
    rc = prvCpssDxChPclTcamRuleActionSw2HwConvert(
        devNum, actionPtr, hwActionArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* convert Mask to HW format */
    rc = prvCpssDxChPclTcamRuleDataSw2HwConvert(
        devNum, ruleFormat, maskPtr, egrPacketTypePattern, hwMaskArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* convert Pattern to HW format */
    rc = prvCpssDxChPclTcamRuleDataSw2HwConvert(
        devNum, ruleFormat, patternPtr, egrPacketTypePattern, hwPatternArr);

    if (0 != PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat2 and above */
        xCat2AddFld.portListEnabled =
            (ruleOptionsBmp & CPSS_DXCH_PCL_RULE_OPTION_PORT_LIST_ENABLED_E)
            ? GT_TRUE : GT_FALSE;

        encapHighBitMask    = 0;
        encapHighBitPattern = 0;
        encapHighBitPos     = 0;
        encapHighBitWord    = 0;

        switch (ruleFormat)
        {
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E:
                xCat2AddFld.isEgress = GT_FALSE;
                xCat2AddFld.ruleSize = 1;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleStdNotIp.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleStdNotIp.common.portListBmp;
                encapHighBitMask =
                    ((maskPtr->ruleStdNotIp.l2Encap >> 1) & 1);
                encapHighBitPattern =
                    ((patternPtr->ruleStdNotIp.l2Encap >> 1) & 1);
                encapHighBitPos  = 13;
                encapHighBitWord = 3;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E:
                xCat2AddFld.isEgress = GT_FALSE;
                xCat2AddFld.ruleSize = 1;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleStdIpL2Qos.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleStdIpL2Qos.common.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E:
                xCat2AddFld.isEgress = GT_FALSE;
                xCat2AddFld.ruleSize = 1;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleStdIpv4L4.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleStdIpv4L4.common.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E:
                xCat2AddFld.isEgress = GT_FALSE;
                xCat2AddFld.ruleSize = 1;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleStdIpv6Dip.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleStdIpv6Dip.common.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E:
                xCat2AddFld.isEgress = GT_FALSE;
                xCat2AddFld.ruleSize = 2;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleExtNotIpv6.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleExtNotIpv6.common.portListBmp;
                encapHighBitMask =
                    ((maskPtr->ruleExtNotIpv6.l2Encap >> 1) & 1);
                encapHighBitPattern =
                    ((patternPtr->ruleExtNotIpv6.l2Encap >> 1) & 1);
                encapHighBitPos  = 14;
                encapHighBitWord = 3;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E:
                xCat2AddFld.isEgress = GT_FALSE;
                xCat2AddFld.ruleSize = 2;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleExtIpv6L2.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleExtIpv6L2.common.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E:
                xCat2AddFld.isEgress = GT_FALSE;
                xCat2AddFld.ruleSize = 2;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleExtIpv6L4.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleExtIpv6L4.common.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E:
                xCat2AddFld.isEgress = GT_TRUE;
                xCat2AddFld.ruleSize = 1;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleEgrStdNotIp.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleEgrStdNotIp.common.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E:
                xCat2AddFld.isEgress = GT_TRUE;
                xCat2AddFld.ruleSize = 1;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleEgrStdIpL2Qos.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleEgrStdIpL2Qos.common.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E:
                xCat2AddFld.isEgress = GT_TRUE;
                xCat2AddFld.ruleSize = 1;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleEgrStdIpv4L4.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleEgrStdIpv4L4.common.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E:
                xCat2AddFld.isEgress = GT_TRUE;
                xCat2AddFld.ruleSize = 2;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleEgrExtNotIpv6.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleEgrExtNotIpv6.common.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E:
                xCat2AddFld.isEgress = GT_TRUE;
                xCat2AddFld.ruleSize = 2;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleEgrExtIpv6L2.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleEgrExtIpv6L2.common.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E:
                xCat2AddFld.isEgress = GT_TRUE;
                xCat2AddFld.ruleSize = 2;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleEgrExtIpv6L4.common.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleEgrExtIpv6L4.common.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E:
                xCat2AddFld.isEgress = GT_FALSE;
                xCat2AddFld.ruleSize = 1;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleIngrStdUdb.commonIngrUdb.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleIngrStdUdb.commonIngrUdb.portListBmp;
                break;
            case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E:
                xCat2AddFld.isEgress = GT_FALSE;
                xCat2AddFld.ruleSize = 2;
                xCat2AddFld.portListBmpMask =
                    maskPtr->ruleIngrExtUdb.commonIngrUdb.portListBmp;
                xCat2AddFld.portListBmpPattern =
                    patternPtr->ruleIngrExtUdb.commonIngrUdb.portListBmp;
                break;
            default: return GT_BAD_PARAM;
        }

        /* adjust rule HW format to xCat2 compatible */
        rc = prvCpssDxChPclRuleDataXCat2Adjust(
            &xCat2AddFld, hwMaskArr, hwPatternArr);

        if (encapHighBitMask != 0)
        {
            workMask = (1 << encapHighBitPos);
            /* update mask */
            workPattern = (encapHighBitMask << encapHighBitPos);
            hwMaskArr[encapHighBitWord] &= (~ workMask);
            hwMaskArr[encapHighBitWord] |= workPattern;
            /* update pattern */
            workPattern = (encapHighBitPattern << encapHighBitPos);
            hwPatternArr[encapHighBitWord] &= (~ workMask);
            hwPatternArr[encapHighBitWord] |= workPattern;
        }
    }

    return rc;
}


