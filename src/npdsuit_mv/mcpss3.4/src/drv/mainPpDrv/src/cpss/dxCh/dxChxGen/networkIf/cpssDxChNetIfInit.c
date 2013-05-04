/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNetIfInit.c
*
* DESCRIPTION:
*       Include CPSS DXCH SDMA/"Ethernet port" network interface initialization
*       functions.
*
* FILE REVISION NUMBER:
*       $Revision: 1.2 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetIf.h>
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>

#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfMii.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfMii.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>

#define ALIGN_4B_MASK_CNS   0x3
#define ALIGN_4B_CNS        4
#define AX_FULL_DEBUG   /* debug */

/*******************************************************************************
* sdmaTxRegConfig
*
* DESCRIPTION:
*       Set the needed values for SDMA registers to enable Tx activity.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The Pp device number.
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
static GT_STATUS sdmaTxRegConfig
(
    IN GT_U8 devNum
)
{
    GT_U8 i;
    GT_STATUS rc = GT_OK;
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    /* Since working in SP the configure transmit queue WRR value to 0 */
    for(i = 0; i < 8; i++)
    {
       rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId, PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txQWrrPrioConfig[i], 0);
       if(rc != GT_OK)
           return rc;

        /********* Tx SDMA Token-Bucket Queue<n> Counter ************/
        rc = prvCpssDrvHwPpWriteRegister(devNum, 0x2700 + (i * 0x10), 0);
        if(rc != GT_OK)
            return rc;

        /********** Tx SDMA Token Bucket Queue<n> Configuration ***********/
        rc = prvCpssDrvHwPpWriteRegister(devNum, 0x2704 + (i * 0x10) , 0xfffffcff);
        if(rc != GT_OK)
            return rc;
    }

    /*********************/
    rc = prvCpssDrvHwPpWriteRegister(devNum, 0x2874, 0xffffffc1);
    if(rc != GT_OK)
        return rc;

    /*********** Set all queues to Fix priority **********/
    rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txQFixedPrioConfig, 0xFF);
    if(rc != GT_OK)
        return rc;
	
    osPrintfDbg("********************** sdmaTxRegConfig Ok !!\n");
    return rc;
}


/*******************************************************************************
* sdmaTxInit
*
* DESCRIPTION:
*       This function initializes the Core Tx module, by allocating the cyclic
*       Tx descriptors list, and the tx Headers buffers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - The device number to init the Tx unit for.
*       descBlockPtr   - A block to allocate the descriptors from.
*       descBlockSize  - Size in bytes of descBlockPtr.
*
* OUTPUTS:
*       numOfDescsPtr  - Number of allocated Tx descriptors.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS sdmaTxInit
(
    IN GT_U8    devNum,
    IN GT_U8    *descBlockPtr,
    IN GT_U32   descBlockSize,
    OUT GT_U32  *numOfDescsPtr
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddr;
    PRV_CPSS_TX_DESC_LIST_STC *txDescList;   /* Points to the relevant Tx desc. list */

    PRV_CPSS_TX_DESC_STC *firstTxDesc;/* Points to the first Tx desc in list. */
    GT_U8 txQueue;              /* Index of the Tx Queue.               */
    GT_U32 numOfTxDesc;         /* Number of Tx desc. that may be       */
                                /* allocated from the given block.      */
    GT_U32 sizeOfDesc;          /* The amount of memory (in bytes) that*/
                                /* a single desc. will occupy, including*/
                                /* the alignment.                       */
    GT_U32 descPerQueue;        /* Number of descriptors per Tx Queue.  */
    GT_U32 i;

    GT_U8 *txHeaderBlock;       /* The Tx header block taken from the   */
                                /* descBlockPtr.                        */
    GT_U8 *shortBuffsBlock;     /* The Tx short buffers block taken     */
                                /* from the descBlockPtr.               */
    PRV_CPSS_SW_TX_DESC_STC *swTxDesc = NULL;/* Points to the Sw Tx desc to   */
                                /* init.                                */
    PRV_CPSS_SW_TX_DESC_STC *firstSwTxDesc;/* Points to the first Sw Tx desc  */
                                /* in list.                             */
    GT_U32  phyNext2Feed;       /* The physical address of the next2Feed*/
                                /* field.                               */
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    txDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList;
    regsAddr    = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    /* Set the descBlockPtr to point to an aligned start address. */
    if(((GT_U32)descBlockPtr % TX_DESC_ALIGN) != 0)
    {
        descBlockSize -= TX_DESC_ALIGN - (((GT_U32)descBlockPtr) % TX_DESC_ALIGN);

        descBlockPtr =
            (GT_U8*)((GT_U32)descBlockPtr +
                     (TX_DESC_ALIGN - (((GT_U32)descBlockPtr) % TX_DESC_ALIGN)));
    }

    /* Tx Descriptor list initialization.   */
    sizeOfDesc  = sizeof(PRV_CPSS_TX_DESC_STC);

    if((sizeOfDesc % TX_DESC_ALIGN) != 0)
    {
        sizeOfDesc += (TX_DESC_ALIGN-(sizeof(PRV_CPSS_TX_DESC_STC) % TX_DESC_ALIGN));

    }

    numOfTxDesc = descBlockSize / sizeOfDesc;

    /* 1/2 of the tx desc. space goes for the tx buffers headers,   */
    /* 1/4 for the Tx headers & 1/4 for the short buffers.          */
    numOfTxDesc = numOfTxDesc / 2;

    txHeaderBlock   = (GT_U8*)(((GT_U32)descBlockPtr) + (descBlockSize >> 1));
    shortBuffsBlock = (GT_U8*)(((GT_U32)txHeaderBlock) + (descBlockSize >> 2));

    descPerQueue = numOfTxDesc / NUM_OF_TX_QUEUES;
    /* Number of descriptors must be divided by 2.  */
    descPerQueue -= (descPerQueue & 0x1);

    *numOfDescsPtr = descPerQueue * NUM_OF_TX_QUEUES;

    for(txQueue = 0; txQueue < NUM_OF_TX_QUEUES; txQueue++)
    {
        txDescList[txQueue].freeDescNum = descPerQueue;
        txDescList[txQueue].maxDescNum = descPerQueue;

        firstTxDesc = (PRV_CPSS_TX_DESC_STC*)descBlockPtr;

        /* hotSynch/shutdown treatment - allocate Mem only once */
        if (txDescList[txQueue].swTxDescBlock == NULL)
        {
            firstSwTxDesc = (PRV_CPSS_SW_TX_DESC_STC *)
                         cpssOsMalloc(sizeof(PRV_CPSS_SW_TX_DESC_STC )*descPerQueue);
            if(firstSwTxDesc == NULL)
            {
                return GT_OUT_OF_CPU_MEM;
            }
            cpssOsMemSet(firstSwTxDesc, 0, sizeof(PRV_CPSS_SW_TX_DESC_STC )*descPerQueue);

            /* save pointer to allocated Mem block on per queue structure */
            txDescList[txQueue].swTxDescBlock = firstSwTxDesc;
        }
        else
        {
            firstSwTxDesc = txDescList[txQueue].swTxDescBlock;
        }

        if(firstSwTxDesc == NULL)
            return GT_FAIL;

        /* create Tx End FIFO in request driven mode */
        if (txDescList[txQueue].poolId == 0)
        {
            if (cpssBmPoolCreate(sizeof(PRV_CPSS_SW_TX_FREE_DATA_STC),
                                 CPSS_BM_POOL_4_BYTE_ALIGNMENT_E,
                                 descPerQueue,
                                 &txDescList[txQueue].poolId) != GT_OK)
            {
                return GT_NO_RESOURCE;
            }

        }
        else
        {
            if (cpssBmPoolReCreate(txDescList[txQueue].poolId) != GT_OK)
            {
                return GT_NO_RESOURCE;
            }
        }

        for(i = 0; i < descPerQueue; i++)
        {

            swTxDesc = firstSwTxDesc + i;


            swTxDesc->txDesc = (PRV_CPSS_TX_DESC_STC*)descBlockPtr;
            descBlockPtr   = (GT_U8*)((GT_U32)descBlockPtr + sizeOfDesc);

            TX_DESC_RESET_MAC(swTxDesc->txDesc);

            if((descPerQueue - 1) != i)
            {
                /* Next descriptor should not be configured for the last one.*/
                swTxDesc->swNextDesc  = firstSwTxDesc + i + 1;
                cpssOsVirt2Phy((GT_U32)descBlockPtr,
                           ((GT_U32*)&(swTxDesc->txDesc->nextDescPointer)));
                swTxDesc->txDesc->nextDescPointer =
                    prvCpssDrvHwByteSwap(devNum, swTxDesc->txDesc->nextDescPointer);
            }

            /* Set the tx header param of the Sw Tx desc. */
            cpssOsMemSet(txHeaderBlock,0,TX_HEADER_SIZE);

            if((i & 0x1) == 1)
                txHeaderBlock =(GT_U8*)((GT_U32)txHeaderBlock + TX_HEADER_SIZE);

            swTxDesc->shortBuffer = shortBuffsBlock;

            shortBuffsBlock =
                (GT_U8*)((GT_U32)shortBuffsBlock + TX_SHORT_BUFF_SIZE);
        }

        /* Close the cyclic desc. list. */
        swTxDesc->swNextDesc = firstSwTxDesc;
        cpssOsVirt2Phy((GT_U32)firstTxDesc,
                   ((GT_U32*)&(swTxDesc->txDesc->nextDescPointer)));
        swTxDesc->txDesc->nextDescPointer =
            prvCpssDrvHwByteSwap(devNum, swTxDesc->txDesc->nextDescPointer);

        txDescList[txQueue].next2Feed   = firstSwTxDesc;
        txDescList[txQueue].next2Free   = firstSwTxDesc;
    }

    for(i = 0; i < NUM_OF_TX_QUEUES; i++)
    {
        cpssOsVirt2Phy((GT_U32)(txDescList[i].next2Feed->txDesc),&phyNext2Feed);
        rc = prvCpssDrvHwPpPortGroupWriteRegister(
            devNum,
            portGroupId,
            regsAddr->sdmaRegs.txDmaCdp[i],phyNext2Feed);

        if(rc != GT_OK)
            return rc;

        /* add from 275 zhangdi */
        #ifdef AX_FULL_DEBUG		
		printf("sdmaTxInit::set queue %d tx dma cpdReg[%#0x] phy %#0x\r\n", \
					i,regsAddr->sdmaRegs.txDmaCdp[i],phyNext2Feed);
		cpssDxChNetIfSdmaPrint(devNum, i, 1, NULL);
		#endif
		/* end */
		
    }

    return sdmaTxRegConfig(devNum);
}

/*******************************************************************************
* sdmaRxInit
*
* DESCRIPTION:
*       This function initializes the Core Rx module, by allocating the cyclic
*       Rx descriptors list, and the rx buffers. -- SDMA
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number to init the Rx unit for.
*       descBlockPtr  - A block to allocate the descriptors from.
*       descBlockSize - Size in bytes of descBlock.
*       rxBufInfoPtr  - A block to allocate the Rx buffers from.
*
* OUTPUTS:
*       numOfDescsPtr  - Number of Rx descriptors allocated.
*       numOfBufsPtr   - Number of Rx buffers allocated.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS sdmaRxInit
(
    IN GT_U8                    devNum,
    IN GT_U8                    *descBlockPtr,
    IN GT_U32                   descBlockSize,
    IN CPSS_RX_BUF_INFO_STC     *rxBufInfoPtr,
    OUT GT_U32                  *numOfDescsPtr,
    OUT GT_U32                  *numOfBufsPtr
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddr;
    PRV_CPSS_RX_DESC_LIST_STC *rxDescList;   /* Points to the relevant Rx desc. list */
    PRV_CPSS_RX_DESC_STC *rxDesc = NULL; /* Points to the Rx desc to init.    */
    PRV_CPSS_RX_DESC_STC *firstRxDesc;/* Points to the first Rx desc in list. */
    PRV_CPSS_RX_DESC_STC *theFirstRxDesc;/* Points to the first Rx desc. */
    GT_U8 rxQueue;              /* Index of the Rx Queue.               */
    GT_U32 numOfRxDesc;         /* Number of Rx desc. that may be       */
                                /* allocated from the given block.      */
    GT_U32 sizeOfDesc;          /* The amount of memory (in bytes) that*/
                                /* a single desc. will occupy, including*/
                                /* the alignment.                       */
    GT_U32 actualBuffSize;      /* Size of a single buffer, after taking*/
                                /* the required alignment into account.*/
    PRV_CPSS_SW_RX_DESC_STC *swRxDesc = NULL;/* Points to the Sw Rx desc to   */
                                /* init.                                */
    PRV_CPSS_SW_RX_DESC_STC *firstSwRxDesc;/* Points to the first Sw Rx desc  */
                                /* in list.                             */
    GT_U32 headerOffsetSize;    /* Size in bytes of the header offset   */
                                /* after alignment to RX_BUFF_ALIGN.    */
    GT_U32 virtBuffAddr;        /* The virtual address of the Rx buffer */
                                /* To be enqueued into the current Rx   */
                                /* Descriptor.                          */
    GT_U32 phyAddr;             /* Physical Rx descriptor's address.    */
    GT_U32 i;                   /* Loop counter                         */
    GT_U32 rxSdmaRegVal;        /* Rx SDMA Queues Register value        */

    /* The following vars. will hold the data from rxBufInfoPtr, and only  */
    /* some of them will be used, depending on the allocation method.   */
    CPSS_RX_BUFF_ALLOC_METHOD_ENT allocMethod;
    GT_U32 buffBlockSize = 0;   /* The static buffer block size (bytes) */
    GT_U8 *buffBlock = NULL;    /* A pointer to the static buffers block*/
    CPSS_RX_BUFF_MALLOC_FUNC mallocFunc = NULL;/* Function for allocating the buffers. */
    GT_U32         buffPercentageSum;/* buffer percentage summary       */
    GT_BOOL        needNetIfInit; /* network interface initialization or reconstruction.*/
    GT_U32         tmpValue;/* tempo value */
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */
    GT_U32         headerOffset = rxBufInfoPtr->headerOffset;
    GT_U32         buffSize = rxBufInfoPtr->rxBufSize;

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    buffPercentageSum = 0;

    *numOfDescsPtr = 0;
    *numOfBufsPtr  = 0;

    /* save space before user header for internal packets indication */
    buffSize     += PRV_CPSS_DXCH_NET_INTERNAL_RX_PACKET_INFO_SIZE_CNS;
    headerOffset += PRV_CPSS_DXCH_NET_INTERNAL_RX_PACKET_INFO_SIZE_CNS;

    rxDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList;
    regsAddr    = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    #ifdef AX_FULL_DEBUG
    printf("-----------------------sdmaRxInit-------------------------\n");
    printf("*Input parameters*\n");
    printf("devNum: %d\n", devNum);
    printf("descBlockPtr: [%p]\n", descBlockPtr);
    printf("descBlockSize: %d\n", descBlockSize);
    printf("rxBufInfoPtr->allocMethod:  %d\n", rxBufInfoPtr->allocMethod);
    printf("rxBufInfoPtr->rxBufSize:    %d\n", rxBufInfoPtr->rxBufSize);
    printf("rxBufInfoPtr->headerOffset: %d\n", rxBufInfoPtr->headerOffset);
    printf("rxBufInfoPtr->buffData.staticAlloc.rxBufBlockPtr: [%p]\n", 
        rxBufInfoPtr->buffData.staticAlloc.rxBufBlockPtr);
    printf("rxBufInfoPtr->buffData.staticAlloc.rxBufBlockSize: %d\n", 
        rxBufInfoPtr->buffData.staticAlloc.rxBufBlockSize);
    printf("\n");
    printf("portGroupId: %d\n", portGroupId);
    printf("rxDescList:  [%p]\n", rxDescList);
    printf("regsAddr:    [%p]\n", regsAddr);
	#endif

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /*  we need to set TO all port groups that the representative for 'TO_CPU packets'
            is the first active port group.
        */
        tmpValue = portGroupId << 1;

        if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
        {
        /* bit 29 - <CpuPortMode> set to 0 'global mode' */
        /* bits 30..31 -  <CpuTargetPipe> set to the 'first active port group'  */
        rc = prvCpssDrvHwPpSetRegField(devNum,
                regsAddr->egrTxQConf.txQueueResSharingAndTunnelEgrFltr,29,3,
                tmpValue);
        }
        else
        {
            /* CPU Port Distribution Register - Lion <0x01800000> */
            /* bit 0 - <CpuPortMode> set to 0 'global mode' */
            /* bits 1..2 -  <CpuTargetCore> set to the 'first active port group'  */
            rc = prvCpssDxChHwPpSetRegField(devNum,
                    regsAddr->txqVer1.egr.global.cpuPortDistribution,0,3,
                    tmpValue);
        }

        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* get that state of the device , check if the device need network interface */
    /* initialization or reconstruction.                                         */
    rc = prvCpssPpConfigDevDbNetIfInitGet(devNum, &needNetIfInit);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* we need to get the values that were last used , when the HW didn't do reset */
    if(needNetIfInit == GT_FALSE)
    {
        /* get the info to synch with current HW state
           NOTE: the 'save' of this info was done in
           prvCpssPpConfigDevDbPrvInfoSet(...) */
        rc = prvCpssPpConfigDevDbPrvNetInfInfoGet(devNum, rxDescList);
        if (rc != GT_OK)
        {
            return rc;
        }

        for(rxQueue = 0; rxQueue < NUM_OF_RX_QUEUES; rxQueue++)
        {
            /* update the total number of desc */
            *numOfDescsPtr += rxDescList[rxQueue].freeDescNum;

            /* update the total number of buffers */
            *numOfBufsPtr += rxDescList[rxQueue].freeDescNum;

            /* Enable Rx SDMA Queue */
            rc = cpssDxChNetIfSdmaRxQueueEnable(devNum, rxQueue, GT_TRUE);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        return GT_OK;;
    }

    /* Due to Cheetah 3 Errata - " FEr#2009: DSDMA resource errors may cause
       PCI Express packets reception malfunction"
       -- see PRV_CPSS_DXCH3_SDMA_WA_E */
    if (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH3_SDMA_WA_E) == GT_TRUE)
    {
        /* A bit per TC defines the behavior in case of Rx resource error.
           Set all bits to 1=drop; In a case of resource error, drop the
           current packet */

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regsAddr->sdmaRegs.sdmaCfgReg,8,8,0xff);
        if(rc != GT_OK)
            return rc;
    }

    /* Set SelPortSDMA to SDMA */
    if ((PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >= CPSS_PP_FAMILY_CHEETAH3_E))
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                                regsAddr->globalRegs.globalControl,20,1,1);
        if(rc != GT_OK)
            return rc;
    }

#if 0  /* cancel compare to 275 */
	
    /* Set CPUPortActive to not active */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                                regsAddr->globalRegs.cpuPortCtrlReg,0,1,0);
        if(rc != GT_OK)
            return rc;
#endif
    allocMethod = rxBufInfoPtr->allocMethod;
    if(allocMethod == CPSS_RX_BUFF_DYNAMIC_ALLOC_E)
    {
        mallocFunc = rxBufInfoPtr->buffData.dynamicAlloc.mallocFunc;
    }
    else
    {
        buffBlock = (GT_U8*)rxBufInfoPtr->buffData.staticAlloc.rxBufBlockPtr;
        buffBlockSize = rxBufInfoPtr->buffData.staticAlloc.rxBufBlockSize;
    }

    /* Set the descBlockPtr to point to an aligned start address. */
    if(((GT_U32)descBlockPtr % RX_DESC_ALIGN) != 0)
    {
        descBlockSize = descBlockSize - (RX_DESC_ALIGN - ((GT_U32)descBlockPtr % RX_DESC_ALIGN));
        descBlockPtr = descBlockPtr + (RX_DESC_ALIGN - ((GT_U32)descBlockPtr % RX_DESC_ALIGN));
    }

    /* Rx Descriptor list initialization.   */
    sizeOfDesc = sizeof(PRV_CPSS_RX_DESC_STC);

    if((sizeOfDesc % RX_DESC_ALIGN) != 0)
    {
        sizeOfDesc += (RX_DESC_ALIGN -(sizeof(PRV_CPSS_RX_DESC_STC) % RX_DESC_ALIGN));
    }

    /* The buffer size must be a multiple of RX_BUFF_SIZE_MULT  */
    actualBuffSize = buffSize - (buffSize % RX_BUFF_SIZE_MULT);
    if(actualBuffSize == 0)
        return GT_INIT_ERROR;

    if(allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E)
    {
        /* Number of Rx descriptors is calculated according to the  */
        /* size of the given Rx Buffers block.                      */
        /* Take the "dead" block in head of the buffers block as a  */
        /* result of the allignment.                                */
        numOfRxDesc = buffBlockSize / actualBuffSize;

        /* Set numOfRxDesc according to the number of descriptors that  */
        /* may be allocated from descBlockPtr and the number of buffers */
        /* that may be allocated from buffBlock.                        */
        if((descBlockSize / sizeOfDesc) < numOfRxDesc)
            numOfRxDesc = descBlockSize / sizeOfDesc;
    }
    else
    {
        /* Number of Rx descriptors is calculated according to the  */
        /* (fixed) size of the given Rx Descriptors block.                  */
        numOfRxDesc = descBlockSize / sizeOfDesc;
    }

    theFirstRxDesc = (PRV_CPSS_RX_DESC_STC*)descBlockPtr;

    
    #ifdef AX_FULL_DEBUG
    printf("descBlockSize: %d\n", descBlockSize);
    printf("sizeOfDesc:    %d\n", sizeOfDesc);
    printf("numOfRxDesc:   %d\n", numOfRxDesc);
    printf("descBlockPtr:   [%p]\n", descBlockPtr);
    printf("theFirstRxDesc: [%p]\n", theFirstRxDesc);

    #endif

    for(rxQueue = 0; rxQueue < NUM_OF_RX_QUEUES; rxQueue++)
    {
        buffPercentageSum += rxBufInfoPtr->bufferPercentage[rxQueue];

        /* validate the total percentage */
        if (buffPercentageSum > 100)
        {
            return GT_FAIL;
        }

        /* set the desc count according to the percentage */
        rxDescList[rxQueue].freeDescNum =
            (rxBufInfoPtr->bufferPercentage[rxQueue] * numOfRxDesc) / 100;

        /* update the total number of desc */
        *numOfDescsPtr += rxDescList[rxQueue].freeDescNum;

        rxDescList[rxQueue].forbidQEn = GT_FALSE;

        firstRxDesc = (PRV_CPSS_RX_DESC_STC*)descBlockPtr;

        /* hotSynch/shutdown treatment - Allocate Mem only once */
        if (rxDescList[rxQueue].swRxDescBlock == NULL)
        {
            firstSwRxDesc = (PRV_CPSS_SW_RX_DESC_STC*)
                         cpssOsMalloc(sizeof(PRV_CPSS_SW_RX_DESC_STC)*
                                        rxDescList[rxQueue].freeDescNum);
            /* save pointer to allocated Mem block on per queue structure */
            rxDescList[rxQueue].swRxDescBlock = firstSwRxDesc;
        }
        else
        {
            firstSwRxDesc = rxDescList[rxQueue].swRxDescBlock;
        }

        if(firstSwRxDesc == NULL)
            return GT_FAIL;

        for(i = 0; i < rxDescList[rxQueue].freeDescNum; i++)
        {
            swRxDesc = firstSwRxDesc + i;

            rxDesc      = (PRV_CPSS_RX_DESC_STC*)descBlockPtr;
            descBlockPtr   = (GT_U8*)((GT_U32)descBlockPtr + sizeOfDesc);
            swRxDesc->rxDesc = rxDesc;
            RX_DESC_RESET_MAC(devNum,swRxDesc->rxDesc);

            if((rxDescList[rxQueue].freeDescNum - 1) != i)
            {
                /* Next descriptor should not be configured for the last one.*/
                swRxDesc->swNextDesc  = (firstSwRxDesc + i + 1);
                cpssOsVirt2Phy((GT_U32)descBlockPtr,
                           ((GT_U32*)(&(swRxDesc->rxDesc->nextDescPointer))));
                swRxDesc->rxDesc->nextDescPointer =
                    prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->nextDescPointer);
            }
        }

        /* Close the cyclic desc. list. */
        swRxDesc->swNextDesc = firstSwRxDesc;
        cpssOsVirt2Phy((GT_U32)firstRxDesc,
                   ((GT_U32*)(&(swRxDesc->rxDesc->nextDescPointer))));
        swRxDesc->rxDesc->nextDescPointer =
            prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->nextDescPointer);

        rxDescList[rxQueue].next2Receive    = firstSwRxDesc;
        rxDescList[rxQueue].next2Return     = firstSwRxDesc;
    }

    /* save Rx descriptor initial configuration parameters for later use after SW restart */
    rc  = prvCpssPpConfigDevDbPrvNetInfInfoSet(devNum, theFirstRxDesc, rxDescList);
    if(rc != GT_OK)
        return rc;

    /* Rx Buffers initialization.           */

    if(allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E)
    {
        /* Set the buffers block to point to a properly alligned block. */
        if(((GT_U32)buffBlock % RX_BUFF_ALIGN) != 0)
        {
            buffBlockSize = (buffBlockSize -
                             (RX_BUFF_ALIGN -
                              ((GT_U32)buffBlock % RX_BUFF_ALIGN)));

            buffBlock =
                (GT_U8*)((GT_U32)buffBlock +
                         (RX_BUFF_ALIGN - ((GT_U32)buffBlock % RX_BUFF_ALIGN)));
        }

        /* Check if the given buffers block, is large enough to be cut  */
        /* into the needed number of buffers.                           */
        if((buffBlockSize / (*numOfDescsPtr)) < RX_BUFF_SIZE_MULT)
            return GT_FAIL;
    }

    headerOffsetSize = headerOffset;
    if((headerOffsetSize % RX_BUFF_ALIGN) != 0)
    {
        headerOffsetSize += (RX_BUFF_ALIGN -(headerOffsetSize % RX_BUFF_ALIGN));
    }

    for(rxQueue = 0; rxQueue < NUM_OF_RX_QUEUES; rxQueue++)
    {
        swRxDesc = rxDescList[rxQueue].next2Receive;
        rxDescList[rxQueue].headerOffset = headerOffset;

        /* update the total number of buffers */
        *numOfBufsPtr += rxDescList[rxQueue].freeDescNum;

        for(i = 0; i < rxDescList[rxQueue].freeDescNum; i++)
        {
            RX_DESC_SET_BUFF_SIZE_FIELD_MAC(devNum,swRxDesc->rxDesc,
                                        (actualBuffSize - headerOffsetSize));

            /* Set the Rx desc. buff pointer field. */
            if(allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E)
            {
                virtBuffAddr = ((GT_U32)buffBlock) + headerOffsetSize;
                cpssOsVirt2Phy(virtBuffAddr,
                           ((GT_U32*)(&(swRxDesc->rxDesc->buffPointer))));
                swRxDesc->rxDesc->buffPointer =
                    prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->buffPointer);

                buffBlock = (GT_U8*)(((GT_U32)buffBlock) + actualBuffSize);

                /* Set the buffers block to point to a properly alligned block*/
                if(((GT_U32)buffBlock % RX_BUFF_ALIGN) != 0)
                {
                    buffBlock =
                        (GT_U8*)((GT_U32)buffBlock +
                                 (RX_BUFF_ALIGN -
                                  ((GT_U32)buffBlock % RX_BUFF_ALIGN)));
                }
            }
            else
            {
                virtBuffAddr = (GT_U32)mallocFunc(actualBuffSize,RX_BUFF_ALIGN);
                if(virtBuffAddr == 0)
                    return GT_OUT_OF_CPU_MEM;

                cpssOsVirt2Phy(virtBuffAddr + headerOffsetSize,
                           ((GT_U32*)(&(swRxDesc->rxDesc->buffPointer))));
                swRxDesc->rxDesc->buffPointer =
                    prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->buffPointer);
            }

            swRxDesc = swRxDesc->swNextDesc;
        }
    }

    /* read rxSDMA Queue Register */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg,&rxSdmaRegVal);
    if(rc != GT_OK)
        return rc;

    /* fix of CQ 106607 : disable SDMA if it is enabled (during cpss restart)
       because if traffic is send to the CPU queues, then it will change the
       'link list' that we try to set to the PP during this function, that may
       cause asynchronous settings between the SW descriptors and the HW descriptors

       --> so we disable the queues at this stage and will enable right after the
       settings of the first descriptor into each queue.
    */
    if(rxSdmaRegVal & 0xFF)/* one of the queues enabled */
    {
        for(i = 0; i < NUM_OF_RX_QUEUES; i++)
        {
            /* for each queue set Enable bit to 0, and Disable bit to 1 */
            rxSdmaRegVal |= 1 << (i + NUM_OF_RX_QUEUES);
            rxSdmaRegVal &= (~(1 << i));

            /* write Rx SDMA Queues Reg - Disable Rx SDMA Queues */
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg,rxSdmaRegVal);
            if(rc != GT_OK)
                return rc;
        }

        /* wait for DMA operations to end on all queues */
        /* wait for bits 0xff to clear */
        rc = prvCpssDxChPortGroupBusyWaitByMask(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg,0xff,GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* read rxSDMA Queue Register */
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg,&rxSdmaRegVal);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    for(i = 0; i < NUM_OF_RX_QUEUES; i++)
    {
        /* set the first descriptor (start of link list) to the specific queue */
        cpssOsVirt2Phy((GT_U32)(rxDescList[i].next2Receive->rxDesc),
                   &phyAddr);
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regsAddr->sdmaRegs.rxDmaCdp[i],phyAddr);
        if(rc != GT_OK)
            return rc;

        /* Set the Receive Interrupt Frame Boundaries   */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regsAddr->sdmaRegs.sdmaCfgReg,0,1,1);
        if(rc != GT_OK)
            return rc;

        /* for each queue set Enable bit to 1, and Disable bit to 0.*/
        rxSdmaRegVal |= 1 << i;
        rxSdmaRegVal &= (~(1 << (i + NUM_OF_RX_QUEUES)));

		#ifdef AX_FULL_DEBUG
		printf("sdmaRxInit::set queue %d rx dma cdpReg[%#0x] phy %#0x\r\n", \
					i,regsAddr->sdmaRegs.rxDmaCdp[i],phyAddr);
		printf("sdmaRxInit::set queue %d rx dma cftRed[%#0x] bit0 value 1\r\n", \
					i,regsAddr->sdmaRegs.sdmaCfgReg);
		cpssDxChNetIfSdmaPrint(devNum, i, 0, NULL);
		#endif		
		
    }

    /* write Rx SDMA Queues Reg - Enable Rx SDMA Queues */
    rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg,rxSdmaRegVal);
    if(rc != GT_OK)
        return rc;

	#ifdef AX_FULL_DEBUG
	printf("sdmaRxInit::dev %d set rx queue cmdReg[%#0x] val %#0x\r\n", \
				devNum,regsAddr->sdmaRegs.rxQCmdReg,rxSdmaRegVal);	
	#endif

    return rc;
}


/*******************************************************************************
* cpssDxChNetIfInit
*
* DESCRIPTION:
*       Initialize the network interface SDMA structures, Rx descriptors & buffers
*       and Tx descriptors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device to initialize.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   The application must call only one of the following APIs per device:
*   cpssDxChNetIfMiiInit - for MII/RGMII Ethernet port networkIf initialization.
*   cpssDxChNetIfInit - for SDMA networkIf initialization.
*   In case more than one of the mentioned above API is called
*   GT_FAIL will be return.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfInit
(
    IN  GT_U8       devNum
)
{
    GT_STATUS          rc;
    GT_U32             numRxDataBufs;   /* These vars. are used to be   */
    GT_U32             numRxDescriptors;/* sent to Tx /Rx initialization*/
    GT_U32             numTxDescriptors;/* functions. and hold the      */
                                        /* number of the different descs*/
                                        /* and buffers allocated.       */

    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    moduleCfgPtr = PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum);

    /* initialize the DB , to be like the HW reset value */
    rc = cpssDxChNetIfPrePendTwoBytesHeaderSet(devNum,GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }
    
    if (!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        /* SDMA mode */
        printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);/*xcat debug*/	
		printf("\t\t*********** SDMA mode RX&TX init!!!  zhangdi\n");

        /* Check for valid configuration parameters */
        if((moduleCfgPtr->netIfCfg.rxDescBlock == NULL) ||
           (moduleCfgPtr->netIfCfg.rxDescBlockSize == 0) ||
           ((moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E) &&
            ((moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockPtr == NULL) ||
             (moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockSize == 0))) ||
           ((moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_DYNAMIC_ALLOC_E) &&
            (moduleCfgPtr->netIfCfg.rxBufInfo.buffData.dynamicAlloc.mallocFunc == NULL)) ||
           (moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_NO_ALLOC_E) ||
           (moduleCfgPtr->netIfCfg.txDescBlock == NULL) ||
           (moduleCfgPtr->netIfCfg.txDescBlockSize== 0))
        {
            return GT_FAIL;
        }

        /* init RX */
        rc = sdmaRxInit(devNum,(GT_U8*)moduleCfgPtr->netIfCfg.rxDescBlock,
                        moduleCfgPtr->netIfCfg.rxDescBlockSize,
                        &(moduleCfgPtr->netIfCfg.rxBufInfo),
                        &numRxDescriptors,&numRxDataBufs);
        if (GT_OK != rc)
        {
            return rc;
        }
		printf("\t\t*********** SDMA mode RX OK!!!  zhangdi\n");

        /* init TX */
        rc = sdmaTxInit(devNum,(GT_U8*)moduleCfgPtr->netIfCfg.txDescBlock,
                        moduleCfgPtr->netIfCfg.txDescBlockSize,
                        &numTxDescriptors);
        if (GT_OK != rc)
        {
            return rc;
        }

		printf("\t\t*********** SDMA mode TX OK!!!  zhangdi\n");
        /* bind the DSMA Tx completed callback to the driver */
        rc = prvCpssDrvInterruptPpTxEndedCbBind(devNum,
                                    prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle);
        if(rc != GT_OK)
        {
            return rc;
        }

    }
    else
    {
        /* SDMA emulation over RGMII/MII */
        CPSS_DXCH_NETIF_MII_INIT_STC neifMiiInit;
        GT_U32 miiDevNum;
        GT_U32 numOfRxBufs;
        GT_U8 *tmpRxBufBlockPtr;
        GT_U32 i;

        miiDevNum = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_E.devNum;
        if (miiDevNum == devNum)
        {
            /* Check for valid configuration parameters */
            if(((moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E) &&
                ((moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockPtr == NULL) ||
                 (moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockSize == 0))) ||
               ((moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_DYNAMIC_ALLOC_E) &&
                (moduleCfgPtr->netIfCfg.rxBufInfo.buffData.dynamicAlloc.mallocFunc == NULL)) ||
               (moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_NO_ALLOC_E) ||
               (moduleCfgPtr->netIfCfg.txDescBlock == NULL) ||
               (moduleCfgPtr->netIfCfg.txDescBlockSize== 0))
            {
                return GT_FAIL;
            }

            neifMiiInit.txInternalBufBlockPtr = (GT_U8 *)moduleCfgPtr->netIfCfg.txDescBlock;
            neifMiiInit.txInternalBufBlockSize = moduleCfgPtr->netIfCfg.txDescBlockSize;
            /* Set the number of TX descriptors to (number of tx header buffers) * 2 */
            neifMiiInit.numOfTxDesc =
                (neifMiiInit.txInternalBufBlockSize / CPSS_GEN_NETIF_MII_TX_INTERNAL_BUFF_SIZE_CNS)*2;
            neifMiiInit.rxBufSize = moduleCfgPtr->netIfCfg.rxBufInfo.rxBufSize;
            neifMiiInit.headerOffset = moduleCfgPtr->netIfCfg.rxBufInfo.headerOffset;


            for (i = 0;i < CPSS_MAX_RX_QUEUE_CNS;i++)
            {
                neifMiiInit.bufferPercentage[i] =
                    moduleCfgPtr->netIfCfg.rxBufInfo.bufferPercentage[i];
            }

            if (moduleCfgPtr->netIfCfg.rxBufInfo.allocMethod == CPSS_RX_BUFF_STATIC_ALLOC_E)
            {
                neifMiiInit.rxBufBlockPtr = (GT_U8 *)moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockPtr;
                neifMiiInit.rxBufBlockSize = moduleCfgPtr->netIfCfg.rxBufInfo.buffData.staticAlloc.rxBufBlockSize;
            }
            else
            {
                numOfRxBufs = moduleCfgPtr->netIfCfg.rxBufInfo.buffData.dynamicAlloc.numOfRxBuffers;

                /* set the requested buffer block size */
                neifMiiInit.rxBufBlockSize = numOfRxBufs * neifMiiInit.rxBufSize;

                /* allocate memory for buffer pool */
                tmpRxBufBlockPtr = moduleCfgPtr->netIfCfg.rxBufInfo.buffData.dynamicAlloc.mallocFunc(
                                    neifMiiInit.rxBufBlockSize, ALIGN_4B_CNS);

                if (NULL == tmpRxBufBlockPtr)
                {
                    return GT_FAIL;
                }
                neifMiiInit.rxBufBlockPtr = tmpRxBufBlockPtr;
            }
            rc = prvCpssDxChNetIfMiiInitNetIfDev(devNum,&neifMiiInit);
            if (rc != GT_OK)
            {
                return GT_FAIL;
            }
        }
    }

	/* change for line 985 to here, zhangdi 2011-02-18*/
    PRV_CPSS_PP_MAC(devNum)->cpuPortMode = PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E;
	printf("\t\t ******** devNum:%d \n",devNum);
    printf("\t\t ******** init the cpuPortMode before MIB clear !!! zhangdi \n");

    /*luoxun --combination may be wrong, attention*/
    #ifndef __AX_PLATFORM__ /* by qinhs@autelan.com 2008-11-26 : reset CPU sdma mib counter */
	rc = cpssDxChNetIfSdmaRxMibClear(devNum);
	if(rc != GT_OK) {
		osPrintfErr("device %d clear cpu port sdma mib counter error %d\n",rc);
		return rc;
	}
    #endif

    #if 0
    PRV_CPSS_PP_MAC(devNum)->cpuPortMode = PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E;
    #endif
    PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.initDone = GT_TRUE;

    /* indicate that network interface init is done and parameters for SW restart are saved */
    rc = prvCpssPpConfigDevDbNetIfInitSet(devNum, GT_FALSE);
    printf("\t\t ******** cpssDxChNetIfInit() OK !!! zhangdi \n");

    return rc;
}
/*******************************************************************************
* cpssDxChNetIfRemove
*
* DESCRIPTION:
*       This function is called upon Hot removal of a device, inorder to release
*       all Network Interface related structures.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device that was removed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfRemove
(
    IN  GT_U8   devNum
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    rxSdmaRegVal; /* Rx SDMA Queues Register value*/
    GT_U32    portGroupId;  /* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    if(PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum))
    {
        /* Delete the Tx descriptors linked-list    */

        /* read Rx SDMA Queues Register. for each queue set disable */
        /* bit to 1, and enable bit to 0.                           */
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                   sdmaRegs.rxQCmdReg,&rxSdmaRegVal);

        if(rc != GT_OK)
        {
            return rc;
        }

        rxSdmaRegVal |= 0x0000FF00;/* set 8 bits */
        rxSdmaRegVal &= 0xFFFFFF00;/* clear 8 bits */

        /* write Rx SDMA Queues Reg - Disable Rx SDMA Queues */
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    sdmaRegs.rxQCmdReg,rxSdmaRegVal);
        if(rc != GT_OK)
        {
            return rc;
        }
        }
    }
    else
    {
        return prvCpssGenNetIfMiiRemove(devNum);
    }

    return GT_OK;
}



#ifndef __AX_PLATFORM__
extern GT_U32 appDemoPpConfigDevAmount;

/*******************************************************************************
* cpssDxChNetIfSdmaPrint
*
* DESCRIPTION:
*       This function print out SDMA Rx/Tx DESC and corresponding packet buffers
*       
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*     devNum  - The device that was removed.
*     queueIdx - SDMA queue index (0-7)
*	direction - for Rx/Tx or both directions( 0 for Rx, 1 for Tx, 2 for both)
* 	printSize - print out buffer size
*
* OUTPUTS:
*       None if printStr is NULL
*	  SDMA queues' Rx or Tx descriptor detailed info
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - one of the parameters value is wrong
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void cpssDxChNetIfSdmaPrint
(
	IN GT_U8	devNum,
	IN GT_U8	queueIdx,
	IN GT_U8   direction,
	OUT GT_U8 	**printStr,
	IN GT_U32	printSize
)
{
char helpStr[] = 	\
  "Usage:cpssDxChRxSdmaPrint devNum,queue,direction\n\
    devNum\tdevice number 0 or 1\n\
    queue\tsdma queue index 0-7\n\
    direction\tsdma rx or tx or both\n";

	int i = 0;
	PRV_CPSS_RX_DESC_LIST_STC *rxDescList;   /* Points to the relevant Rx desc. list */
	PRV_CPSS_TX_DESC_LIST_STC *txDescList; 	/* Points to the relevant Tx desc. list */
    PRV_CPSS_RX_DESC_STC *rxDesc = NULL; /* Points to the Rx desc to init.    */
	PRV_CPSS_TX_DESC_STC *txDesc = NULL; /* Points to Tx desc */
	PRV_CPSS_SW_RX_DESC_STC *firstSwRxDesc = NULL,*swRxDesc = NULL;		
	PRV_CPSS_SW_TX_DESC_STC *firstSwTxDesc = NULL,*swTxDesc = NULL;
	static GT_U32 numOfRxDesc[NUM_OF_RX_QUEUES] = {0}, numOfTxDesc[NUM_OF_TX_QUEUES] = {0};
	unsigned char directOut = 0; /* print result directly out to stderr or stdout */
	unsigned int curLen = 0;
	unsigned char *pos = NULL;

	if(!printStr || !(*printStr)) {
		directOut = 1;
	}
	else {
		pos = *printStr;
		curLen = 0;
	}
	
	if((devNum >= appDemoPpConfigDevAmount)|| \
		(direction > 2) || \
		(queueIdx >= NUM_OF_RX_QUEUES)){
		if(directOut)
			fprintf(stderr,"%s",helpStr);
		else 
			curLen += sprintf(pos, "%s",helpStr);
		return;
	}

	if((0 == direction) ||(2 == direction)) { /* for Rx SDMA */
		rxDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList;

		if(0 == numOfRxDesc[queueIdx]) {
			numOfRxDesc[queueIdx] = rxDescList[queueIdx].freeDescNum;
		}

		firstSwRxDesc = rxDescList[queueIdx].swRxDescBlock;
		if(NULL == firstSwRxDesc) {
			if(directOut)
				fprintf(stderr,"Rx descriptor null for queue %d on device %d\n",queueIdx,devNum);
			else 
				curLen += sprintf(pos,"Rx descriptor null for queue %d on device %d\n",queueIdx,devNum);
			return;
		}

		if((curLen + 100) > printSize) return;

		if(pos) {
			curLen += sprintf(pos ,"----------------<<<Rx-Q%d-%d free>>>----------------\r\n", \
							queueIdx,rxDescList[queueIdx].freeDescNum);
			pos = *printStr + curLen;
			curLen += sprintf(pos,"%-7s%-9s%-9s%-9s%-9s%-9s\r\n", \
							"COLUMN","SELF(H)","WORD0(H)","WORD1(H)","DATA(H)","NEXT(H)");	
			pos = *printStr + curLen;
		}
		for(i=0;i <numOfRxDesc[queueIdx];i++) {
			swRxDesc = firstSwRxDesc + i;
			rxDesc = swRxDesc->rxDesc;
			if(rxDesc) {
				if((curLen + 44) > printSize) return;
				if(pos) {
					curLen += sprintf(pos,"[%03d]: %08X %08X %08X %08X %08X\r\n", i,rxDesc, \
							prvCpssDrvHwByteSwap(devNum,(volatile unsigned int)rxDesc->word1), \
							prvCpssDrvHwByteSwap(devNum,(volatile unsigned int)rxDesc->word2), \
							prvCpssDrvHwByteSwap(devNum,(volatile unsigned int)rxDesc->buffPointer), \
							prvCpssDrvHwByteSwap(devNum,(volatile unsigned int)rxDesc->nextDescPointer));
					pos = *printStr + curLen;
				}
			}
		}
		
	}

	if((1 == direction) ||(2 == direction)) { /* for Tx SDMA */
		txDescList  = PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList;
		
		if(0 == numOfTxDesc[queueIdx]) {
			numOfTxDesc[queueIdx] = txDescList[queueIdx].freeDescNum;
		}

		firstSwTxDesc = txDescList[queueIdx].swTxDescBlock;
		if(NULL == firstSwTxDesc) {
			if((curLen + 44) > printSize) return;
			
			if(directOut)
				fprintf(stderr,"Tx descriptor null for queue %d on device %d\n",queueIdx,devNum);
			else 
				curLen += sprintf(pos,"Tx descriptor null for queue %d on device %d\n",queueIdx,devNum);
			return;
		}

		if((curLen + 100) > printSize) return;
		
		if(pos) {
			curLen += sprintf(pos ,"----------------<<<Tx-Q%d-%d free>>>----------------\r\n", \
							queueIdx, txDescList[queueIdx].freeDescNum);
			pos = *printStr + curLen;
			curLen += sprintf(pos,"%-7s%-9s%-9s%-9s%-9s%-9s\r\n", \
							"COLUMN","SELF(H)","WORD0(H)","WORD1(H)","DATA(H)","NEXT(H)");		
			pos = *printStr + curLen;
		}

		for(i=0;i <numOfTxDesc[queueIdx];i++) {
			swTxDesc = firstSwTxDesc + i;
			txDesc = swTxDesc->txDesc;
			if(txDesc) {
				if((curLen + 44) > printSize) return;
				if(pos) {
					curLen += sprintf(pos,"[%03d]: %08X %08X %08X %08X %08X\r\n", i,txDesc, \
							prvCpssDrvHwByteSwap(devNum,(volatile unsigned int)txDesc->word1), \
							prvCpssDrvHwByteSwap(devNum,(volatile unsigned int)txDesc->word2), \
							prvCpssDrvHwByteSwap(devNum,(volatile unsigned int)txDesc->buffPointer), \
							prvCpssDrvHwByteSwap(devNum,(volatile unsigned int)txDesc->nextDescPointer));
					pos = *printStr + curLen;
				}
			}
		}
	}

	return;
}
#endif

