/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChCfgInit.c
*
* DESCRIPTION:
*       Core initialization of PPs and shadow data structures, and declarations
*       of global variables.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpm.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>

/*******************************************************************************
 * Internal usage variables
 ******************************************************************************/

/* number of entries in the cheetah3 mac2me table */
#define PRV_CPSS_DXCH3_MAC2ME_TABLE_MAX_ENTRIES_CNS 8

/* table not valid --> 0 entries */
#define TABLE_NOT_VALID_CNS     0

/* define the number of different FDB messages queues that we may use:
    1. primary AUQ
    2. primary FUQ
    3. secondary AUQ
*/
#define MESSAGE_QUEUE_NUM_CNS   3

#define QUEUE_NOT_USED_CNS  0xFF

/*******************************************************************************
* internalCfgAddrUpInit
*
* DESCRIPTION:
*       This function initializes the Core Address Update mechanism, by
*       allocating the AU descs. block.
*       supports 'per port group'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number to init the Au unit for.
*       portGroupId  - the port group Id
*       auDescBlockPtr  - (pointer to)A block to allocate the Au descriptors from.
*       auDescBlockSize - Size in bytes of Au descBlock.
*       fuDescBlockPtr  - (pointer to)A block to allocate the Fu descriptors from.
*                         Valid if useFuQueue is GT_TRUE.
*       fuDescBlockSize - Size in bytes of Fu descBlock.
*                         Valid if useFuQueue is GT_TRUE.
*       useFuQueue      - GT_TRUE  - Fu queue is used - for DxCh2 devices only.
*                       - GT_FALSE - Fu queue is unused.
*       ignoreAuq       - ignore the AUQ parameters
*                         GT_TRUE - ignore auDescBlockPtr,auDescBlockSize
*                         GT_FALSE - use auDescBlockPtr,auDescBlockSize
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internalCfgAddrUpInit
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U8    *auDescBlockPtr,
    IN GT_U32   auDescBlockSize,
    IN GT_U8    *fuDescBlockPtr,
    IN GT_U32   fuDescBlockSize,
    IN GT_BOOL  useFuQueue,
    IN GT_BOOL  ignoreAuq
)
{
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrl[MESSAGE_QUEUE_NUM_CNS];    /* Points to the desc. list. */
    PRV_CPSS_AU_DESC_CTRL_STC   *tmpDescCtrl[MESSAGE_QUEUE_NUM_CNS]; /* Points to the desc. list. */
    GT_U32                      phyAddr;/* The physical address of the AU block*/
    GT_STATUS                   retVal;
    GT_U32                      numOfIter;/* the number of iterations in initialization */
    GT_U32                      ii;          /* iterator */
    PRV_CPSS_AU_DESC_STC        *addrUpMsgPtr;   /* update message address */
    PRV_CPSS_AU_DESC_STC        *descBlockPtr;  /* pointer to AU/FU descr block. */
    GT_U8                       *descBlock[MESSAGE_QUEUE_NUM_CNS];  /* A block to allocate the Au/Fu descriptors from.*/
    GT_U32                      descBlockSize[MESSAGE_QUEUE_NUM_CNS]; /* Size in bytes of Fu/Au descBlock. */
    GT_BOOL                     initHwPointerAfterShutdown = GT_FALSE;
    GT_BOOL                     didHwReset;/* are we after HW reset or only after 'cpss SW re-init' (without HW reset since last call of this function)*/
    GT_U32                      alignmentNumBytes;/* number of bytes for alignment movement*/
    GT_U32                      auqIndex;/*index for the AU in the arrays of : descCtrl,descBlock,descBlockSize*/
    GT_U32                      fuqIndex;/*index for the FU in the arrays of : descCtrl,descBlock,descBlockSize*/
    GT_U32  addrUpdateQueueCtrl[MESSAGE_QUEUE_NUM_CNS];
    GT_U32  addrUpdateQueueBaseAddr[MESSAGE_QUEUE_NUM_CNS];
    GT_U32  secondaryAuqIndex = QUEUE_NOT_USED_CNS;/* index in the descCtrl[] that the secondary uses ,
                                    initialized with unreachable index value */
    GT_U8   *secondaryAuqDescBlockPtr;/*pointer to the secondary AUQ block */
    GT_U32  secondaryAuqDescBlockSize;/*size in bytes of the secondary AUQ block */
    GT_U32  regAddr;                  /* register address */

    if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) &&
       (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                   info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                       enabled == GT_FALSE))
    {
        numOfIter = 0;

        if(ignoreAuq == GT_FALSE)
        {
            if(auDescBlockPtr == NULL)
            {
                /* no Hw changes needed */
                /* nothing more to do */
                return GT_OK;
            }

            auqIndex = numOfIter;

            /* AUQ registers */
            addrUpdateQueueCtrl[numOfIter] =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;
            addrUpdateQueueBaseAddr[numOfIter] =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;

            descCtrl[auqIndex] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]);
            descBlock[auqIndex] = auDescBlockPtr;
            descBlockSize[auqIndex] = auDescBlockSize;

            numOfIter++;
        }
        else
        {
            auqIndex = QUEUE_NOT_USED_CNS;/*not relevant*/
        }

        if(useFuQueue == GT_TRUE)
        {
            /* FUQ registers */
            addrUpdateQueueCtrl[numOfIter] =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
            addrUpdateQueueBaseAddr[numOfIter] =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQBaseAddr;

            fuqIndex = numOfIter;
            descCtrl[fuqIndex] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
            descBlock[fuqIndex] = fuDescBlockPtr;
            descBlockSize[fuqIndex] = fuDescBlockSize;

            numOfIter++;
        }
        else
        {
            fuqIndex = QUEUE_NOT_USED_CNS;/*not relevant*/
        }

        /* check if need to set the secondary AUQ for WA */
        if((ignoreAuq == GT_FALSE) &&
            PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
             PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E) == GT_TRUE)
        {
            descCtrl[numOfIter] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]);

            secondaryAuqDescBlockSize =
                CPSS_DXCH_PP_FDB_AUQ_DEADLOCK_EXTRA_MESSAGES_NUM_WA_CNS *
                sizeof(PRV_CPSS_AU_DESC_STC);

            if(auDescBlockSize <= secondaryAuqDescBlockSize)
            {
                /* caller not giving enough resources */
                return GT_NO_RESOURCE;
            }

            /* we need to take the needed size from the primary AUQ */
            secondaryAuqDescBlockPtr = auDescBlockPtr +
                (auDescBlockSize - secondaryAuqDescBlockSize);

            /* update the primary AUQ size */
            auDescBlockSize -= secondaryAuqDescBlockSize;
            descBlockSize[auqIndex] = auDescBlockSize;

            /* NOTE : the auDescBlock is already aligned so we not need to align
               the secondaryAuqDescBlockPtr
            */


            descBlock[numOfIter] = secondaryAuqDescBlockPtr;
            descBlockSize[numOfIter] = secondaryAuqDescBlockSize;

            /* use the same registers as the primary AUQ */
            addrUpdateQueueCtrl[numOfIter] = addrUpdateQueueCtrl[auqIndex];
            addrUpdateQueueBaseAddr[numOfIter] = addrUpdateQueueBaseAddr[auqIndex];

            secondaryAuqIndex = numOfIter;

            numOfIter++;
        }

        if(numOfIter == 0)
        {
            /* nothing more to do */
            goto exit_cleanly_lbl;
        }


        /* get that state of the device , is the device after HW reset and
           not did the AU/FU queues settings */
        retVal = prvCpssPpConfigDevDbHwResetGet(devNum,&didHwReset);
        if(retVal != GT_OK)
        {
            return retVal;
        }

        /* we need to get the values that were last used , when the HW didn't do reset ,
           and when the DB of descCtrl[0]->blockAddr == 0 (not initialized yet)  */
        if(descCtrl[0]->blockAddr == 0 && didHwReset == GT_FALSE)

        {
            tmpDescCtrl[0] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]);
            tmpDescCtrl[1] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
            tmpDescCtrl[2] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]);

            /* get the info to synch with current HW state
               NOTE: the 'save' of this info was done in
               prvCpssPpConfigDevDbPrvInfoSet(...) */
            retVal = prvCpssPpConfigDevDbPrvInfoGet(devNum,portGroupId,
                                                    tmpDescCtrl[0],
                                                    tmpDescCtrl[1],
                                                    tmpDescCtrl[2]);
            if (retVal != GT_OK)
            {
                  return retVal;
            }


            if(auqIndex < MESSAGE_QUEUE_NUM_CNS)
            {
                descCtrl[auqIndex] = tmpDescCtrl[0];
            }

            if(fuqIndex < MESSAGE_QUEUE_NUM_CNS)
            {
                descCtrl[fuqIndex] = tmpDescCtrl[1];
            }

            if(secondaryAuqIndex < MESSAGE_QUEUE_NUM_CNS)
            {
                descCtrl[secondaryAuqIndex] = tmpDescCtrl[2];
            }
        }


        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* enable/disable  FU queue */
            retVal = prvCpssDrvHwPpPortGroupSetRegField(devNum,
                    portGroupId,
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl,
                    31,1,
                    (useFuQueue == GT_TRUE) ? 1 : 0);
            if (retVal != GT_OK)
                 return retVal;
        }

        for(ii = 0; ii < numOfIter; ii++)
        {

            /* if device is not after HW reset (meaning we already set the device
               with AU/FU queue size and base address , and we cant set those
               values again to HW , we only need to clean the AU/FU queues from
               leftovers messages) */
            if( didHwReset == GT_FALSE)
            {
                descBlockPtr = (PRV_CPSS_AU_DESC_STC*)(descCtrl[ii]->blockAddr);
                addrUpMsgPtr = &(descBlockPtr[descCtrl[ii]->currDescIdx]);

                /* handle AUQ or FUQ (when no CNC used) */
                if((ii == auqIndex)|| /*AUQ*/
                    (ii == fuqIndex /*FUQ*/&& (PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum)) &&
                     0 == descCtrl[ii]->unreadCncCounters)||
                     (ii == secondaryAuqIndex)/*secondary AUQ*/)
                {
                    /* Clear the AU queue until the HW pointer */
                    while (!AU_DESC_IS_NOT_VALID(addrUpMsgPtr))
                    {
                        descCtrl[ii]->currDescIdx =
                                    ((descCtrl[ii]->currDescIdx + 1) %
                                                    descCtrl[ii]->blockSize);
                        AU_DESC_RESET_MAC(addrUpMsgPtr);
                        addrUpMsgPtr = &(descBlockPtr[descCtrl[ii]->currDescIdx]);

                        /* prevent infinite loop */
                        if(descCtrl[ii]->currDescIdx == 0)
                        {
                            /* HW pointer at the end of AU/FU queue =>
                              Need to rewind it */
                            initHwPointerAfterShutdown = GT_TRUE;
                            break;
                        }
                    }
                }
                else if(ii == fuqIndex)/* relevant to CNC counters only */
                {
                    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum) &&
                       descCtrl[ii]->unreadCncCounters)
                    {
                        /*since we may have CNC messages on queue , clear it also */

                        if(descCtrl[ii]->unreadCncCounters & 1)
                        {
                            /* make the number even , because 2 counters in an entry */
                            descCtrl[ii]->unreadCncCounters ++;
                        }


                        while(descCtrl[ii]->unreadCncCounters)
                        {
                            descCtrl[ii]->unreadCncCounters -=2;/* 2 CNC counters in 'Single entry' */

                            descCtrl[ii]->currDescIdx =
                                        ((descCtrl[ii]->currDescIdx + 1) %
                                                        descCtrl[ii]->blockSize);
                            AU_DESC_RESET_MAC(addrUpMsgPtr);
                            addrUpMsgPtr = &(descBlockPtr[descCtrl[ii]->currDescIdx]);

                            /* prevent infinite loop */
                            if(descCtrl[ii]->currDescIdx == 0)
                            {
                                /* HW pointer at the end of AU/FU queue =>
                                  Need to rewind it */
                                initHwPointerAfterShutdown = GT_TRUE;
                                break;
                            }
                        }

                        descCtrl[ii]->unreadCncCounters = 0;
                    }
                }

                if (initHwPointerAfterShutdown == GT_FALSE)
                {
                    /* go to the next iteration */
                    continue;
                }
            }

            /* Set the descBlock to point to an aligned start address. */
            if((((GT_U32)descBlock[ii]) % AU_BLOCK_ALIGN) != 0)
            {
                alignmentNumBytes =  (AU_BLOCK_ALIGN -
                                 (((GT_U32)descBlock[ii]) % AU_BLOCK_ALIGN));
                descBlockSize[ii] -= alignmentNumBytes;/* update size according to new buffer alignment*/
                descBlock[ii]     += alignmentNumBytes;/* update buffer according to alignment */
            }

            descCtrl[ii]->currDescIdx = 0;
            descCtrl[ii]->blockAddr = (GT_U32)descBlock[ii];
            descCtrl[ii]->blockSize = descBlockSize[ii] /
                                                 sizeof(PRV_CPSS_AU_DESC_STC);
            descCtrl[ii]->unreadCncCounters = 0;

            cpssOsMemSet(descBlock[ii],0,sizeof(GT_U8) * descBlockSize[ii]);

            if(ii == secondaryAuqIndex)
            {
                /* no more to do for this queue as we not do any HW writings for
                    this queue */
                continue;
            }


            retVal = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                     addrUpdateQueueCtrl[ii],0,31,
                                     descCtrl[ii]->blockSize);

            if (retVal != GT_OK)
                 return retVal;

            cpssOsVirt2Phy(descCtrl[ii]->blockAddr,&phyAddr);

            retVal = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,
                                       addrUpdateQueueBaseAddr[ii],phyAddr);

            if (retVal != GT_OK)
                  return retVal;
        }
    }
    else
    {
        /* use FIFO registers not PCI.
           change auQBaseAddr register address to use FIFO.
           do not change auQControl register address. */
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr= 0x06000034;

        if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
        }

        /*  For DxCh1 and DxCh2 disable PCI interface
            in the Global Control register.
            For DxCh3 and above devices disable "AUMsgsToCPUIF"
            in FDB Global Configuration register. */
        retVal = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                 regAddr,
                 20,1,0);

        if (retVal != GT_OK)
            return retVal;

    }

exit_cleanly_lbl:
    if(portGroupId == PRV_CPSS_LAST_ACTIVE_PORT_GROUP_ID_MAC(devNum))
    {
        /* for 'multi port groups' device -- state that PP ready only after all port groups finished */

        /* state to the special DB that the device finished PP logical init */
        prvCpssPpConfigDevDbHwResetSet(devNum,GT_FALSE);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgDevRemove
*
* DESCRIPTION:
*
*       Remove the device from the CPSS.
*       This function we release the DB of CPSS that associated with the PP ,
*       and will "know" that the device is not longer valid.
*       This function will not do any HW actions , and therefore can be called
*       before or after the HW was actually removed.
*
*       After this function is called the devNum is no longer valid for any API
*       until this devNum is used in the initialization sequence for a "new" PP.
*
*       NOTE: 1. Application may want to disable the Traffic to the CPU , and
*             messages (AUQ) , prior to this operation (if the device still exists).
*             2. Application may want to a reset the HW PP , and there for need
*             to call the "hw reset" function (before calling this function)
*             and after calling this function to do the initialization
*             sequence all over again for the device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to remove.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - wrong dev
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevRemove
(
    IN GT_U8   devNum
)
{
    GT_STATUS   rc;
    CPSS_PP_FAMILY_TYPE_ENT         devFamily;
    GT_U32  ii;

    /* check that the device still exists */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /*store values needed for later use */
    devFamily = PRV_CPSS_PP_MAC(devNum)->devFamily;


    /* release the DXCH DBs */
    /* free the registers addresses memory */
    switch(devFamily)
    {
        case CPSS_PP_FAMILY_CHEETAH_E:
            prvCpssDxChHwRegAddrRemove(devNum);
            break;
        case CPSS_PP_FAMILY_CHEETAH2_E:
            prvCpssDxCh2HwRegAddrRemove(devNum);
            break;
        case CPSS_PP_FAMILY_CHEETAH3_E:
            prvCpssDxCh3HwRegAddrRemove(devNum);
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            prvCpssDxChXcatHwRegAddrRemove(devNum);
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            prvCpssDxChXcat2HwRegAddrRemove(devNum);
            break;
        case CPSS_PP_FAMILY_DXCH_LION_E:
            prvCpssDxChLionHwRegAddrRemove(devNum);
            break;
        default:
            return GT_BAD_STATE;
    }

    /* free allocations are 'DXCH' specific allocations (not generic) */
    /* need to free :
       1. pools
       2. dynamic allocations
       3. semaphores/mutexes
    */
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.enhancedInfoPtr);
    for(ii = 0;ii < PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->numOfUplinkPorts ;ii++)
    {
        FREE_PTR_MAC(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr[ii].internalConnectionsArr);
    }
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->uplinkInfoArr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->internalTrunksPtr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E.
            stackMacCntPtr);
    FREE_PTR_MAC(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
            extraTrunksInfoPtr);

    /* next part -- release the 'generic' DB */
    rc = prvCpssPpConfigDevDbRelease(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /*************************************************************/
    /* NOTE :  PRV_CPSS_PP_MAC(devNum) == NULL at this point !!! */
    /*************************************************************/

    /* check that the family of the cheetah is empty , so we can release the
       global needed DB , relate to the cheetah family */
    if(prvCpssFamilyInfoArray[devFamily] == NULL)
    {
        /* release the LPM DB if no more DXCH devices */
        rc = prvCpssDxChIpLpmDbRelease();
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgDevEnable
*
* DESCRIPTION:
*       This routine sets the device state.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to set.
*       enable - GT_TRUE device enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevEnable
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32          bit;        /* The bit to be written to the register    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    bit = ((enable == GT_TRUE) ? 1 : 0);

    /* Write 'bit' to device enable/disable bit in the
       the global control register.*/
    return prvCpssDrvHwPpSetRegField(devNum,
                   PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl,
                   0,1,bit);
}

/*******************************************************************************
* cfgAddrUpInit
*
* DESCRIPTION:
*       This function initializes the Core Address Update mechanism, by
*       allocating the AU descs. block.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number to init the Au unit for.
*       auDescBlockPtr  - (pointer to)A block to allocate the Au descriptors from.
*       auDescBlockSize - Size in bytes of Au descBlock.
*       fuDescBlockPtr  - (pointer to)A block to allocate the Fu descriptors from.
*                         Valid if useFuQueue is GT_TRUE.
*       fuDescBlockSize - Size in bytes of Fu descBlock.
*                         Valid if useFuQueue is GT_TRUE.
*       useFuQueue      - GT_TRUE  - Fu queue is used - for DxCh2 devices only.
*                       - GT_FALSE - Fu queue is unused.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS cfgAddrUpInit
(
    IN GT_U8    devNum,
    IN GT_U8    *auDescBlockPtr,
    IN GT_U32   auDescBlockSize,
    IN GT_U8    *fuDescBlockPtr,
    IN GT_U32   fuDescBlockSize,
    IN GT_BOOL  useFuQueue
)
{
    GT_STATUS   rc;
    GT_U32      portGroupId;
    GT_U32      numActivePortGroups = 0;
    GT_U32      perPortGroupAuSize;
    GT_U32      perPortGroupFuSize;
    GT_U32      index=0;
    GT_U8       *perPortGroupAuDescBlockPtr;

    if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) &&
       (PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                   info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                       enabled == GT_FALSE))
    {
        if(auDescBlockPtr == NULL)
        {
            /* no Hw changes needed */
            /* nothing more to do */
            return GT_OK;
        }
    }


    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        numActivePortGroups++;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
        {
            /* all port groups get the same AUQ size */
            perPortGroupAuSize = auDescBlockSize / numActivePortGroups;
            /* all port groups get the same FUQ size */
            perPortGroupFuSize = fuDescBlockSize / numActivePortGroups;
        }
        else
        {
            perPortGroupAuSize = auDescBlockSize;
            perPortGroupFuSize = fuDescBlockSize;
        }

        perPortGroupAuDescBlockPtr = auDescBlockPtr ? &auDescBlockPtr[perPortGroupAuSize * index] : NULL;

        rc = internalCfgAddrUpInit(devNum,
                    portGroupId,
                    perPortGroupAuDescBlockPtr,
                    perPortGroupAuSize,
                    (useFuQueue ? &fuDescBlockPtr[perPortGroupFuSize * index] : NULL),
                    perPortGroupFuSize,
                    useFuQueue,
                    GT_FALSE);
        index++;

        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgPpLogicalInit
*
* DESCRIPTION:
*       This function Performs PP RAMs divisions to memory pools, according to
*       the supported modules in system.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number
*       ppConfigPtr - (pointer to)includes PP specific logical initialization
*                     params.
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_PP_MEM -  If the given configuration can't fit into the given
*                           PP's memory capabilities, or
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_FAIL otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  This function should perform all PP initializations, which
*           includes:
*               -   Init the Driver's part (by calling driverPpInit()).
*               -   Ram structs init, and Ram size check.
*               -   Initialization of the interrupt mechanism data structure.
*       2.  When done, the function should leave the PP in disable state, and
*           forbid any packets forwarding, and any interrupts generation.
*       3.  The execution flow for preparing for the initialization of core
*           modules is described below:
*
*
*             +-----------------+
*             | Init the 3 RAMs |   This initialization includes all structs
*             | conf. structs   |   fields but the base address field.
*             |                 |
*             +-----------------+
*                     |
*   (Ram is big       |                 +-------------------------+
*    enough to hold   +--(No)-----------| return GT_OUT_OF_PP_MEM |
*    all tables?)     |                 +-------------------------+
*                     |
*                   (Yes)
*                     |
*                     V
*             +-------------------+
*             | Set the Rams base |
*             | addr. according   |
*             | to the location   |
*             | fields.           |
*             +-------------------+
*                     |
*                     |
*                     V
*             +----------------------------------+
*             | Init the                         |
*             | PRV_CPSS_DXCH_MODULE_CONFIG_STC  |
*             | struct, according                |
*             | to the info in RAM               |
*             | conf. struct.                    |
*             +----------------------------------+
*                     |
*                     |
*                     |
*                     V
*             +---------------------------------+
*             | set the prepared                |
*             | module Cfg struct               |
*             | in the appropriate              |
*             | PRV_CPSS_DXCH_PP_CONFIG_STC     |
*             | struct.                         |
*             +---------------------------------+
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgPpLogicalInit

(
    IN      GT_U8                           devNum,
    IN      CPSS_DXCH_PP_CONFIG_INIT_STC    *ppConfigPtr
)
{
    GT_STATUS         rc;
    GT_U32            value;
    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;/* pointer to the module
                                                configure of the PP's database*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ppConfigPtr);

    /* Configure the module configruation struct.   */
    moduleCfgPtr = &(PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg);

    /* Init the AU and FU (if fuqUseSeparate is GT_TRUE) mechanism according
     to the data. Set in the module configuration structure. */
    /* Init the AU mechanism according to the data  */
    /* set in the module configuration structure.   */
    rc = cfgAddrUpInit(devNum,moduleCfgPtr->auCfg.auDescBlock,
                              moduleCfgPtr->auCfg.auDescBlockSize,
                              moduleCfgPtr->fuCfg.fuDescBlock,
                              moduleCfgPtr->fuCfg.fuDescBlockSize,
                              moduleCfgPtr->fuqUseSeparate);

    if (rc != GT_OK)
                return rc;

    if( PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        switch(ppConfigPtr->routingMode)
        {
            case CPSS_DXCH_POLICY_BASED_ROUTING_ONLY_E:
                value = 0;
                break;
            case CPSS_DXCH_TCAM_ROUTER_BASED_E:
                value = 1;
                break;
            default:
                return GT_BAD_PARAM;
        }

        rc = prvCpssDrvHwPpSetRegField(devNum,
                                       PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ipRegs.routerAdditionalCtrlReg,
                                       0,1,value);
        if (rc != GT_OK)
            return rc;
    }

    moduleCfgPtr->ip.routingMode = ppConfigPtr->routingMode;

    return rc;

}

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
*
* DESCRIPTION:
*       Sets device ID modification for Routed packets.
*       Enables/Disables FORWARD DSA tag modification of the <source device>
*       and <source port> fields of packets routed by the local device.
*       The <source device> is set to the local device ID and the <source port>
*       is set to 61 (the virtual router port).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - Physical device number.
*       portNum         - Physical port number (or CPU port)
*       modifyEnable    - Boolean value of the FORWARD DSA tag modification:
*                             GT_TRUE  -  Device ID Modification is Enabled.
*                             GT_FALSE -  Device ID Modification is Disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     modifyEnable
)
{
    GT_U32  regAddr;     /* register address */
    GT_U32  regValue;    /* register value */
    GT_U32  fieldOffset; /* The start bit number in the register */
    GT_U32  portGroupId; /*the port group Id - support multi port groups device */
    GT_U8   localPort;   /* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi port groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regValue = (modifyEnable  == GT_TRUE) ? 1 : 0;

    fieldOffset = (localPort == CPSS_CPU_PORT_NUM_CNS) ?
                 PRV_CPSS_DXCH_PP_HW_INFO_HA_CPU_PORT_BIT_INDEX_MAC(devNum) :
                 localPort;

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
            routerDevIdModifyEnReg[OFFSET_TO_WORD_MAC(fieldOffset)];

    return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                     regAddr, OFFSET_TO_BIT_MAC(fieldOffset), 1, regValue);
}

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortModifySet
*
* DESCRIPTION:
*       Enables/Disables Device ID Modification upon packet sending to another
*       stack unit.
*
*       When Connecting DxCh Devices to SOHO in a Value Blade model to enable
*       DxCh grade Capabilities for FE Ports, in a staking system, we must be
*       able to overcome the 32 devices limitation.
*       To do that, SOHO Device Numbers are not unique and packets received
*       from the SOHO by the DxCh and are relayed to other stack units
*       have their Device ID changed to the DxCh Device ID.
*       On Upstream (packet from SOHO to DxCh):
*           The SOHO Sends a packet to the DxCh and the packet is sent back
*           to the SOHO. In this case we don't want to change the Device ID in
*           the DSA Tag so that the SOHO is able to filter the Src Port and is
*           able to send back the packet to its source when doing cross chip
*           flow control.
*       On Downsteam (packet from SOHO to SOHO):
*           The SOHO receives a packet from another SOHO in this case we
*           want to change the Device ID so that the packet is not filtered.
*
*       On top of the above, for forwarding packets between the DxCh devices
*       and for Auto Learning, the Port Number must also be changed.
*       In addition Changing the Device ID is needed only for FORWARD DSA Tag.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Physical device number.
*       modifedDsaSrcDev    - Boolean value of Enables/Disables Device ID
*                             Modification:
*                             GT_TRUE  -  DSA<SrcDev> is modified to the
*                                         DxCh Device ID and DSA<SrcPort>
*                                         is modified to the DxCh Ingress
*                                         port if all of the following are
*                                         conditions are met:
*                                           - packet is received with a
*                                           non-extended DSA Tag FORWARD    and
*                                           - DSA<SrcIsTrunk> = 0           and
*                                           - packet is transmitted with an
*                                           extended DSA Tag FORWARD.
*
*                             GT_FALSE -  DSA<SrcDev> is not modified when the
*                                         packet is sent to another stac unit
*                                         via an uplink.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This field is relevant for GigE Ports Only.
*       This change is in Addition to the SrcDev Change for Routed packets.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDsaTagSrcDevPortModifySet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     modifedDsaSrcDev
)
{
    GT_U32  regAddr;     /* register address */
    GT_U32  regValue;    /* register value */
    GT_U32  startBit;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regValue = (modifedDsaSrcDev == GT_TRUE) ? 1 : 0;

    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                    eggrDSATagTypeConf[0];
        startBit = 28;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                    hdrAltGlobalConfig;
        startBit = 16;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, startBit, 1, regValue);
}





/*******************************************************************************
* cpssDxChCfgTableNumEntriesGet
*
*
* DESCRIPTION:
*       the function return the number of entries of each individual table in
*       the HW
*
*       when several type of entries like ARP and tunnelStart resize in the same
*       table (memory) , the function returns the number of entries for the least
*       denominator type of entry --> in this case number of ARP entries.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*       table        - type of the specific table
*
* OUTPUTS:
*       numEntriesPtr - (pointer to) number of entries
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgTableNumEntriesGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_CFG_TABLES_ENT    table,
    OUT GT_U32                      *numEntriesPtr
)
{
    GT_U32  numEntries;/* tmp num entries value */
    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* validate the pointer */
    CPSS_NULL_PTR_CHECK_MAC(numEntriesPtr);

    switch(table)
    {
        case CPSS_DXCH_CFG_TABLE_VLAN_E:
            numEntries = PRV_CPSS_MAX_NUM_VLANS_CNS;
            break;
        case CPSS_DXCH_CFG_TABLE_FDB_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb;
            break;
        case CPSS_DXCH_CFG_TABLE_PCL_ACTION_E:
        case CPSS_DXCH_CFG_TABLE_PCL_TCAM_E:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily  <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                numEntries =
                    PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policyTcamRaws
                    * 2;
            }
            else /* ch3 and above */
            {
                numEntries =
                    PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policyTcamRaws
                    * 4;
            }

            /* the number of 'standard' rules (same number of actions) */

            break;
        case CPSS_DXCH_CFG_TABLE_ROUTER_NEXT_HOP_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerNextHop;
            break;
        case CPSS_DXCH_CFG_TABLE_ROUTER_LTT_E: /* same as router TCAM */
        case CPSS_DXCH_CFG_TABLE_ROUTER_TCAM_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerAndTunnelTermTcam;
            break;
        case CPSS_DXCH_CFG_TABLE_MLL_PAIR_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.mllPairs;
            break;
        case CPSS_DXCH_CFG_TABLE_POLICER_METERS_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum;
            break;
        case CPSS_DXCH_CFG_TABLE_POLICER_BILLING_COUNTERS_E:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily  <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                numEntries = TABLE_NOT_VALID_CNS;/* no such table*/
            }
            else /* ch3 and above */
            {
                numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policersNum;
            }
            break;
        case CPSS_DXCH_CFG_TABLE_VIDX_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.vidxNum;
            break;
        case CPSS_DXCH_CFG_TABLE_ARP_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerArp;
            break;
        case CPSS_DXCH_CFG_TABLE_TUNNEL_START_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelStart;
            break;
        case CPSS_DXCH_CFG_TABLE_STG_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.stgNum;
            break;
        case CPSS_DXCH_CFG_TABLE_QOS_PROFILE_E:
            numEntries = (GT_U32)(PRV_CPSS_DXCH_QOS_PROFILE_MAX_MAC(devNum));
            break;
        case CPSS_DXCH_CFG_TABLE_MAC_TO_ME_E:
            if (PRV_CPSS_PP_MAC(devNum)->devFamily  <= CPSS_PP_FAMILY_CHEETAH2_E)
            {
                numEntries = TABLE_NOT_VALID_CNS;/* no such table*/
            }
            else /* ch3 and above */
            {
                numEntries = PRV_CPSS_DXCH3_MAC2ME_TABLE_MAX_ENTRIES_CNS;
            }
            break;

        case CPSS_DXCH_CFG_TABLE_CNC_E:
            /* the number of CNC counters in the device */
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.cncBlocks *
                         PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.cncBlockNumEntries;
            break;

        case CPSS_DXCH_CFG_TABLE_CNC_BLOCK_E:
            /* the number of counters in CNC block */
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.cncBlockNumEntries;
            break;

        case CPSS_DXCH_CFG_TABLE_TRUNK_E:
            numEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.trunksNum;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* return the caller with needed info */
    *numEntriesPtr = numEntries;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgReNumberDevNum
*
* DESCRIPTION:
*       function allow the caller to modify the DB of the cpss ,
*       so all info that was 'attached' to 'oldDevNum' will be moved to 'newDevNum'.
*
*       NOTE:
*       1. it is the responsibility of application to update the 'devNum' of
*       HW entries from 'oldDevNum' to 'newDevNum' using appropriate API's
*       such as 'Trunk members','Fdb entries','NH entries','PCE entries'...
*       2. it's application responsibility to update the HW device number !
*          see API cpssDxChCfgHwDevNumSet
*       3. no HW operations involved in this API
*
*       NOTE:
*       this function MUST be called under 'Interrupts are locked' and under
*       'Tasks lock'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       oldDevNum  - old device number
*       newDevNum  - new device number (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - the device oldDevNum not exist
*       GT_OUT_OF_RANGE - the device newDevNum value > 0x1f (0..31)
*       GT_ALREADY_EXIST - the new device number is already used
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgReNumberDevNum
(
    IN GT_U8   oldDevNum,
    IN GT_U8   newDevNum
)
{
    GT_STATUS   rc;
    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(oldDevNum);

    /*validate the new device number*/
    if(newDevNum >= BIT_5)
    {
        /*device number limited to 5 bits */
        return GT_OUT_OF_RANGE;
    }

    if(oldDevNum == newDevNum)
    {
        /* nothing more to do */
        return GT_OK;
    }

    /* swap the special DB - for re-init support */
    rc = prvCpssPpConfigDevDbRenumber(oldDevNum,newDevNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* swap the cpss DB */
    if(prvCpssPpConfig[newDevNum] != NULL)
    {
        return GT_ALREADY_EXIST;
    }

    prvCpssPpConfig[newDevNum] = prvCpssPpConfig[oldDevNum];
    prvCpssPpConfig[oldDevNum] = NULL;

    /* let the cpssDriver also 'SWAP' pointers */
    rc = prvCpssDrvHwPpRenumber(oldDevNum,newDevNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(newDevNum))
    {
        /* UN-bind the old device from the DSMA Tx completed callback to the driver */
        rc = prvCpssDrvInterruptPpTxEndedCbBind(oldDevNum,NULL);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* bind the DSMA Tx completed callback to the driver - to the new device */
        rc = prvCpssDrvInterruptPpTxEndedCbBind(newDevNum,
                                    prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChCfgDevInfoGet
*
* DESCRIPTION:
*       the function returns device static information
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*
* OUTPUTS:
*       devInfoPtr   - (pointer to) device information
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgDevInfoGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_DXCH_CFG_DEV_INFO_STC   *devInfoPtr
)
{
    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* validate the pointer */
    CPSS_NULL_PTR_CHECK_MAC(devInfoPtr);

    /* fill in generic part of device info */
    return prvCpssPpConfigDevInfoGet(devNum,&(devInfoPtr->genDevInfo));
}


/*******************************************************************************
* cpssDxChCfgBindPortPhymacObject
*
* DESCRIPTION:
*         The function binds port phymac pointer
*         to 1540M PHYMAC object
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*    portNum        - port number
*    macDrvObjPtr   - port phymac object pointer
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChCfgBindPortPhymacObject(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN CPSS_MACDRV_OBJ_STC * const macDrvObjPtr
)
{
    GT_STATUS   rc;

    /* validate devNum and portNum */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    /* validate the MACDRV object pointer */
    CPSS_NULL_PTR_CHECK_MAC(macDrvObjPtr);

    rc = prvCpssPpConfigPortToPhymacObjectBind(devNum,portNum,macDrvObjPtr);
    if (rc != GT_OK) {
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChCfgIngressDropCntrSet
*
* DESCRIPTION:
*       Set the Ingress Drop Counter value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Physical device number.
*       counter     - Ingress Drop Counter value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgIngressDropCntrSet
(
    IN  GT_U8       devNum,
    IN  GT_U32      counter
)
{
    GT_U32      regAddr;     /* register address */

    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
        eqBlkCfgRegs.ingressDropCntrReg;

    return prvCpssDxChPortGroupsBmpCounterSet(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                              regAddr, 0, 32, counter);
}


/*******************************************************************************
* cpssDxChCfgIngressDropCntrGet
*
* DESCRIPTION:
*       Get the Ingress Drop Counter value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Physical device number.
*
* OUTPUTS:
*       counterPtr  - (pointer to) Ingress Drop Counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgIngressDropCntrGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *counterPtr
)
{
    GT_U32      regAddr;     /* register address */

    /* validate the device */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* validate the pointer */
    CPSS_NULL_PTR_CHECK_MAC(counterPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
        eqBlkCfgRegs.ingressDropCntrReg;

    return prvCpssDxChPortGroupsBmpCounterSummary(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                       regAddr, 0, 32,
                                                       counterPtr, NULL);
}
