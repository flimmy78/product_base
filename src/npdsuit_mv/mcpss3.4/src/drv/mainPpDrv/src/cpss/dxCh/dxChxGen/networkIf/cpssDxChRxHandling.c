/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChRxHandling.c
*
* DESCRIPTION:
*       This file implements functions to allow application to receive RX
*       packets from PP , and to free the resources when Rx packet's buffers not
*       needed.
*
* FILE REVISION NUMBER:
*       $Revision: 1.3 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>

#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfMii.h>

/*******************************************************************************
* External usage environment parameters
*******************************************************************************/

/*******************************************************************************
* Internal usage environment parameters
*******************************************************************************/

/*******************************************************************************
* dxChFirstRxBuffOffsetGet
*
* DESCRIPTION:
*       get the offset from the start of the first Rx buffer to the actual start
*       of this Rx buffer
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number through which these buffers where
*                     received.
*       firstRxBuffPtr     - (pointer to)start of the Rx buffer
*
* OUTPUTS:
*       offsetToActualStartPtr -  (pointer to) offset to the actual start of the
*                                 Rx buffer
*
* RETURNS:
*       none
*
* COMMENTS:
*       the actual pointer to the start of the buffer is :
*       (firstRxBuffPtr - (*offsetToActualStartPtr))
*
*       called from cpssDxChNetIfRxBufFree(...)
*
*******************************************************************************/
static void dxChFirstRxBuffOffsetGet
(
    IN  GT_U8           devNum,
    IN  GT_U8           *firstRxBuffPtr,
    OUT GT_U32          *offsetToActualStartPtr
)
{
    /* in cheetah we always have at least 4 bytes on the first Rx buffer that
       are are not used .
       those 4 bytes (or 8 bytes) exist because the cheetah receive every packet
       with Extended DSA tag (8 bytes). and if the packet came untagged then
       we remove 8 bytes , and if the packet came tagged we remove only 4 bytes
       */

    /* so when function cpssDxChNetIfRxBufFree(...) need to free this first
       buffer it must know the actual start of the buffer .
    */

    /* so in this function we need to know if the packet was originally
       tagged/untagged
    */

    if(PRV_CPSS_DXCH_RX_ORIG_PCKT_TAGGED_GET_MAC(firstRxBuffPtr,
        PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum)->netIfCfg.rxBufInfo.headerOffset))
    {
    	/* no 802.1q tag append for tagged packet when we receive it, 
    	  * so we move back just as untagged packets do.
    	  * by qinhs@autelan.com */
    	#if 0
        /* packet was originally tagged */
        *offsetToActualStartPtr =
            PRV_CPSS_DXCH_DIFF_EXT_DSA_TAG_2_TAGGED_CNS;/* 4 */
		#else 
        *offsetToActualStartPtr =
            PRV_CPSS_DXCH_DIFF_EXT_DSA_TAG_2_UNTAGGED_CNS;/* 8 */
		#endif
    }
    else
    {
        /* packet was originally untagged */
        *offsetToActualStartPtr =
            PRV_CPSS_DXCH_DIFF_EXT_DSA_TAG_2_UNTAGGED_CNS;/*8*/
    }

    return;
}

/*******************************************************************************
* dxChNetIfSdmaRxBufFreeWithSize
*
* DESCRIPTION:
*       Frees a list of buffers, that where previously passed to the upper layer
*       in an Rx event. -- SDMA relate
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum      - The device number through which these buffers where
*                     received.
*       rxQueue     - The Rx queue number through which these buffers where
*                     received.
*       rxBuffList  - List of Rx buffers to be freed.
*       rxBuffSizeList  - List of Rx Buffers sizes , to set to the free
*                     descriptor
*                     if this parameter is NULL --> we ignore it.
*       buffListLen - Length of rxBufList.
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
static GT_STATUS dxChNetIfSdmaRxBufFreeWithSize
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   rxBuffSizeList[],
    IN GT_U32   buffListLen
)
{
    GT_STATUS rc = GT_OK;
    PRV_CPSS_RX_DESC_LIST_STC       *rxDescList;
    GT_U32                          rxSdmaRegMask;
    PRV_CPSS_RX_DESC_STC            *rxDesc;
    GT_U32                          tmpData;
    GT_U32                          i;
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */
    PRV_CPSS_SW_RX_DESC_STC         *first2Return;

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    rxDescList = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList[rxQueue]);

    first2Return = rxDescList->next2Return;
    for(i = 0; i < buffListLen; i++)
    {
        /* If rxDescList->rxDescList->next2Return equal to rxDescList->rxDescList->next2Receive
          and rxDescList->freeDescNum is bigger then 0 this means that all descriptors
          in the ring pointing to buffer, therefore we cannot add new buufer to the ring.
          This may indicate a problem in the application, that tries to free the buffers
          more then once before receiving it from PP.
         */
        if((rxDescList->freeDescNum > 0) &&
           (rxDescList->next2Return == rxDescList->next2Receive) )
            return GT_FULL;

        rxDescList->freeDescNum++;

        cpssOsVirt2Phy((GT_U32)(rxBuffList[i]),&tmpData);
        tmpData = prvCpssDrvHwByteSwap(devNum, tmpData);
        rxDesc = rxDescList->next2Return->rxDesc;
        rxDesc->buffPointer = tmpData;

        if(i != 0)
        {/* first descriptor will be freed last */
            GT_SYNC;
            RX_DESC_RESET_MAC(devNum,rxDescList->next2Return->rxDesc);
            if(rxBuffSizeList)
            {
                RX_DESC_SET_BUFF_SIZE_FIELD_MAC(devNum,rxDescList->next2Return->rxDesc,rxBuffSizeList[i]);
            }
            GT_SYNC;
        }
        rxDescList->next2Return = rxDescList->next2Return->swNextDesc;
    }

    /* return first descriptor in chain to sdma ownership, it's done last to prevent multiple resource errors,
     * when jumbo packets arrive to cpu on high speed and cpu frees descriptors one by one in simple sequence
     * sdma immediately uses them and reaches cpu owned descriptor and again causes resource error
     */
    GT_SYNC;
    RX_DESC_RESET_MAC(devNum, first2Return->rxDesc);

    if(rxBuffSizeList)
    {
        RX_DESC_SET_BUFF_SIZE_FIELD_MAC(devNum, first2Return->rxDesc, rxBuffSizeList[0]);
    }
    GT_SYNC;

    if(rxDescList->forbidQEn == GT_FALSE)
    {
        /* Enable the Rx SDMA   */
        rxSdmaRegMask = (1 << rxQueue);
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,
                                         PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.rxQCmdReg,
                                         rxSdmaRegMask);
        if(rc != GT_OK)
            return rc;

    }

    return GT_OK;
}
/*******************************************************************************
* cpssDxChNetIfRxBufFree
*
* DESCRIPTION:
*       Frees a list of buffers, that where previously passed to the upper layer
*       in an Rx event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number through which these buffers where
*                     received.
*       rxQueue     - The Rx queue number through which these buffers where
*                     received (0..7).
*       rxBuffList  - List of Rx buffers to be freed.
*       buffListLen - Length of rxBufList.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfRxBufFree
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   buffListLen
)
{
    GT_U32                  firstBuffOffset = 0;/* offset to the actual start of
                                                   the FIRST buffer */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(rxBuffList);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(rxQueue);
    if(buffListLen == 0)
    {
        return GT_BAD_PARAM;
    }

    /*check that the DB of networkIf library was initialized*/
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);

    if((PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
        && !PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        if(PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum))
        {
            /* get the offset before the start of the first buffer pointer to the
               actual start of the first buffer */
            dxChFirstRxBuffOffsetGet(devNum,rxBuffList[0],&firstBuffOffset);

            /* update the buffer pointer to the actual start of the buffer */
            rxBuffList[0] -= firstBuffOffset;

            /* SDMA buffer free */
            return dxChNetIfSdmaRxBufFreeWithSize(devNum,rxQueue,rxBuffList,NULL,buffListLen);
        }
        else
        {
            /* call the BSP to free the buffers */
            return GT_NOT_IMPLEMENTED;
        }
    }
    else if((PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
            && PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        return prvCpssGenNetIfMiiRxBufFree(devNum, rxQueue, rxBuffList, buffListLen);
    }
    else /* PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E, PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_NONE_E */
        return GT_BAD_STATE;

}

/*******************************************************************************
* cpssDxChNetIfRxBufFreeWithSize
*
* DESCRIPTION:
*       Frees a list of buffers, that where previously passed to the upper layer
*       in an Rx event.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number through which these buffers where
*                     received.
*       rxQueue     - The Rx queue number through which these buffers where
*                     received (0..7).
*       rxBuffList  - List of Rx buffers to be freed.
*       rxBuffSizeList  - List of Rx Buffers sizes.
*       buffListLen - Length of rxBuffList and rxBuffSize lists.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfRxBufFreeWithSize
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   rxBuffSizeList[],
    IN GT_U32   buffListLen
)
{
    GT_U32                  firstBuffOffset = 0;/* offset to the actual start of
                                                   the FIRST buffer */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(rxBuffList);
    CPSS_NULL_PTR_CHECK_MAC(rxBuffSizeList);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(rxQueue);
    if(buffListLen == 0)
    {
        return GT_BAD_PARAM;
    }

    /*check that the DB of networkIf library was initialized*/
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);

    /* get the offset before the start of the first buffer pointer to the
       actual start of the first buffer */
    dxChFirstRxBuffOffsetGet(devNum,rxBuffList[0],&firstBuffOffset);

    /* update the buffer pointer to the actual start of the buffer */
    rxBuffList[0] -= firstBuffOffset;

    if((PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
        && !PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        if(PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum))
        {
            /* SDMA buffer free */
            return dxChNetIfSdmaRxBufFreeWithSize(devNum,rxQueue,rxBuffList,rxBuffSizeList,buffListLen);
        }
        else
        {
            /* No Such interface with BSP */
            return GT_NOT_IMPLEMENTED;
        }
    }
    else /* PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E, PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_NONE_E */
        return GT_BAD_STATE;
}


/*******************************************************************************
* dxChNetIfRxBufFree
*
* DESCRIPTION:
*       Frees a list of buffers, are NOT yet added Tagged/untagged offset
*       emulation.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number through which these buffers where
*                     received.
*       rxQueue     - The Rx queue number through which these buffers where
*                     received.
*       rxBuffList  - List of Rx buffers to be freed.
*       buffListLen - Length of rxBufList.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       Internal function -- not API to the Application
*
*******************************************************************************/
static void dxChNetIfRxBufFree
(
    IN GT_U8    devNum,
    IN GT_U8    rxQueue,
    IN GT_U8    *rxBuffList[],
    IN GT_U32   buffListLen
)
{
    /* this code is done for the cpssDxChNetIfRxBufFree(...) to do correct
       action */

    rxBuffList[0] += PRV_CPSS_DXCH_DIFF_EXT_DSA_TAG_2_UNTAGGED_CNS; /*8*/

    /* emulate untagged packet */
    PRV_CPSS_DXCH_RX_ORIG_PCKT_UNTAGGED_SET_MAC(rxBuffList[0],
        PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum)->netIfCfg.rxBufInfo.headerOffset);

    /* call the engine to free the buffers */
    cpssDxChNetIfRxBufFree(devNum,rxQueue,rxBuffList,buffListLen);
}

/*******************************************************************************
* dxChNetIfRxPacketGet
*
* DESCRIPTION:
*       This function returns packets from PP destined to the CPU port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       numOfBuffPtr- Num of buffs in packetBuffsArrPtr.
*
* OUTPUTS:
*       numOfBuffPtr        - Num of used buffs in packetBuffsArrPtr.
*       packetBuffsArrPtr   - (pointer to)The received packet buffers list.
*       buffLenArr          - List of buffer lengths for packetBuffsArrPtr.
*       rxParamsPtr  - (Pointer to)information parameters of received packets
*
* RETURNS:
*       GT_NO_MORE  - no more packets on the device/queue
*       GT_OK       - packet got with no error.
*       GT_FAIL     - failed to get the packet
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED - on not supported DSA tag type.
*       GT_BAD_PARAM - wrong devNum,portNum,queueIdx.
*       GT_DSA_PARSING_ERROR - DSA tag parsing error.
*
* COMMENTS:
*       It is recommended to call cpssDxChNetIfRxBufFree for this queue
*       i.e. return the buffer to their original queue.
*
*
*       common function for both for the :
*           1) parse DMA packet
*           2) parse Eth port packet
*
*******************************************************************************/
static GT_STATUS dxChNetIfRxPacketGet
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    INOUT GT_U32                            *numOfBuffPtr,
    OUT GT_U8                               *packetBuffsArrPtr[],
    OUT GT_U32                              buffLenArr[],
    OUT CPSS_DXCH_NET_RX_PARAMS_STC         *rxParamsPtr
)
{
    GT_STATUS   rc;         /* Return Code                                  */
    GT_U32      i;          /* Iterator                                     */
    GT_U32      byte2Remove;/* Bytes to remove in order to remove DSA tag   */
    GT_U32      pcktOffset; /* Offset from the start of the buffer          */
    GT_U8       *buffPtr;   /* temporary buffer ptr                         */
    GT_BOOL     notSupported;/* not supported state                         */
    GT_U32      hdrOffset;   /* Packet header offset                        */
    GT_U8       *dsaTagPtr;  /* Extended DSA tag                            */
    GT_BOOL     srcOrDstIsTagged;/* send application the packet tagged or untagged */
    GT_U32      dsaOffset;/* offset of the DSA tag from start of packet */

    notSupported    = GT_FALSE;
    hdrOffset       = PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum)->netIfCfg.rxBufInfo.headerOffset;

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.prePendTwoBytesHeader == GT_TRUE)
    {
        /* the PP will put the start of the Ethernet header of the packet , in 2
           bytes offset , to allow the IP header of the packet to be 4 bytes
           aligned */

        dsaOffset = PRV_CPSS_DSA_TAG_PCKT_OFFSET_CNS + /* 12 */
                    PRV_CPSS_DXCH_IP_ALIGN_OFFSET_CNS; /* 2 */
    }
    else
    {
        dsaOffset = PRV_CPSS_DSA_TAG_PCKT_OFFSET_CNS; /* 12 */
    }

    /* get the 8 bytes of the extended DSA tag from the first buffer */
    dsaTagPtr = &(packetBuffsArrPtr[0][dsaOffset]);/*offset 12*/

    rc = cpssDxChNetIfDsaTagParse(devNum, dsaTagPtr , &rxParamsPtr->dsaParam);

    if(rc != GT_OK)
    {
        dxChNetIfRxBufFree(devNum, queueIdx,packetBuffsArrPtr,*numOfBuffPtr);
        return rc;
    }

    switch(rxParamsPtr->dsaParam.dsaType)
    {
        case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
            srcOrDstIsTagged = rxParamsPtr->dsaParam.dsaInfo.toCpu.isTagged;
            break;
        case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
            if(rxParamsPtr->dsaParam.dsaInfo.fromCpu.dstInterface.type == CPSS_INTERFACE_PORT_E)
            {
                srcOrDstIsTagged = rxParamsPtr->dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged;
            }
            else
            {
                srcOrDstIsTagged = GT_TRUE;
            }
            break;
        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            srcOrDstIsTagged = rxParamsPtr->dsaParam.dsaInfo.toAnalyzer.isTagged;
            break;
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
            srcOrDstIsTagged = rxParamsPtr->dsaParam.dsaInfo.forward.srcIsTagged;
            break;
        default:
            dxChNetIfRxBufFree(devNum, queueIdx,packetBuffsArrPtr,*numOfBuffPtr);
            return GT_NOT_SUPPORTED;
    }

    /**************************************************/
    /* remove the extended DSA tag from buffer[0] */
    /**************************************************/
    if(GT_TRUE == srcOrDstIsTagged)
    {
    	#if 0 /* no 802.1q tag append for tagged packet, by qinhs@autelan.com */
        /* packet was tagged originally */
        byte2Remove = PRV_CPSS_DXCH_ETHPRT_TAGGED_PCKT_FLAG_LEN_CNS ;/* 4 */
        /* after the vlan tag */
        pcktOffset = dsaOffset + 4;/*16 or 18 */

        /* restore the "vlan tag style" to the needed bytes */
        buffPtr = (GT_U8 *)&packetBuffsArrPtr[0][dsaOffset];

        /* set 0x8100 */
        buffPtr[0] = (GT_U8)PRV_CPSS_VLAN_ETHR_TYPE_MSB_CNS;
        buffPtr[1] = (GT_U8)PRV_CPSS_VLAN_ETHR_TYPE_LSB_CNS;

        /* reset the cfiBit value */
        buffPtr[2] &= ~(1<<4);

        /* set the cfiBit -- according to info */
        buffPtr[2] |= ((rxParamsPtr->dsaParam.commonParams.cfiBit == 1) ? (1 << 4) : 0);
		#else
        byte2Remove = PRV_CPSS_DXCH_DIFF_EXT_DSA_TAG_2_UNTAGGED_CNS;/* 8 */

        /* after the mac addresses  */
        pcktOffset = dsaOffset;/*12 or 14 */
		#endif
		
    }
    else
    {
        byte2Remove = PRV_CPSS_DXCH_DIFF_EXT_DSA_TAG_2_UNTAGGED_CNS;/* 8 */

        /* after the mac addresses  */
        pcktOffset = dsaOffset;/*12 or 14 */
    }

    /* move the start of the data on the buffer */
    buffPtr = (GT_U8 *)&packetBuffsArrPtr[0][pcktOffset-1];

    for(i = pcktOffset ; i; i-- , buffPtr--)
    {
        buffPtr[byte2Remove] = buffPtr[0];
    }

    /***************************************************************************
    *  from this point the "free buffers" in case of error must call function  *
    *  cpssDxChNetIfRxBufFree(...) and not to dxChNetIfRxBufFree(...)      *
    ***************************************************************************/


    packetBuffsArrPtr[0] = (buffPtr + 1) + byte2Remove;
    buffLenArr[0]     =  buffLenArr[0]   - byte2Remove;

    /* Next code is for internal use :
       to know how to retrieve actual pointer to the buffer when free the buffer

       Note : this action of emulation tagged/untagged must be done after the
       packetBuffsArrPtr[0] is stable (the pointer is not changed any more)
    */
    if(GT_TRUE == srcOrDstIsTagged)
    {
        /* emulate tagged packet */
        PRV_CPSS_DXCH_RX_ORIG_PCKT_TAGGED_SET_MAC(packetBuffsArrPtr[0], hdrOffset);
    }
    else
    {
        /* emulate untagged packet */
        PRV_CPSS_DXCH_RX_ORIG_PCKT_UNTAGGED_SET_MAC(packetBuffsArrPtr[0], hdrOffset);
    }
    return GT_OK;
}


/*******************************************************************************
* dxChNetIfSdmaRxResErrPacketHandle
*
* DESCRIPTION:
*       This function free the descriptors of packets from PP destined to the
*       CPU port with Rx Resource Error. -- SDMA relate
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*      None
*
* RETURNS:
*      GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#if 0   /* used by npd, cpss1.3 */
static GT_STATUS dxChNetIfSdmaRxResErrPacketHandle
#endif
GT_STATUS dxChNetIfSdmaRxResErrPacketHandle

(
    IN GT_U8                               devNum,
    IN GT_U8                               queueIdx
)
{
    GT_STATUS rc = GT_OK;
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC      *regsAddr;/* alias to register address*/
    PRV_CPSS_SW_RX_DESC_STC   *swRxDesc;    /* A pointer to a Rx descriptor   */
    PRV_CPSS_SW_RX_DESC_STC   *swN2fRxDesc; /* A pointer to the Next 2        */
                                            /* free Rx descriptor.            */
    GT_U32                    rxSdmaRegMask;/* Rx SDMA register mask          */
    PRV_CPSS_RX_DESC_LIST_STC        *rxDescList;    /* alias to queue descriptor list   */
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */
    #ifndef __AX_PLATFORM__ /* by qinhs@autelan.com 2009-3-28 for illegal cdp value check */
	GT_BOOL		cdpValErr = GT_FALSE; /* Indicates if Current Descriptor Pointer has wrong value */
	unsigned int	curCdpData = 0, curCdpVirtAddr = 0; /* Current Descriptor Pointer value */
    GT_BOOL                   errExists;     /* resource error indication flag*/
    #endif

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    regsAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);

    rxDescList = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList[queueIdx]);

    if (GT_FALSE == rxDescList->forbidQEn)
    {
        /* Disable the corresponding Queue. */
        rxSdmaRegMask = (1 << (queueIdx + 8));
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regsAddr->sdmaRegs.rxQCmdReg, rxSdmaRegMask);
        if(rc != GT_OK)
            return rc;

        /* Check that the SDMA is disabled. */
        /* wait for bit 0 to clear */
        rc = prvCpssDxChPortGroupBusyWait(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg,queueIdx,GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        rxDescList->forbidQEn = GT_TRUE;
    }

    /* set the SW descriptor shadow pointers */
    swRxDesc        = rxDescList->next2Receive;
    swN2fRxDesc     = rxDescList->next2Return;

    /* copy the Rx descriptor to SW shadow */
    swRxDesc->shadowRxDesc.word1 = prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->word1);
    swRxDesc->shadowRxDesc.word2 = prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->word2);

    
    #ifndef __AX_PLATFORM__/* by qinhs@autelan.com 2009-3-28 for illegal cdp value check */
	/* get the current descriptor in HW */
	prvCpssDrvHwPpReadRegister(devNum, regsAddr->sdmaRegs.rxDmaCdp[queueIdx], &curCdpData);
    #if 0
    if(GT_OK != cpssOsPhy2Virt(devNum,(unsigned int)curCdpData, &curCdpVirtAddr))
    #endif
    /* luoxun -- cpssOsPhy2Virt() has only two arguments in cpss3.4, remove 'devNum' for compiling.*/
	if(GT_OK != cpssOsPhy2Virt((unsigned int)curCdpData, &curCdpVirtAddr))
	{
		osPrintfErr("device %d queue %d rx res error event found dma address @%#x illegal, need recovery\r\n",
				devNum, queueIdx, curCdpData);
		cdpValErr = GT_TRUE;
	}
    #endif

    /* Reset all descriptors with set resource error bit. */
    while (RX_DESC_GET_REC_ERR_BIT(&(swRxDesc->shadowRxDesc)) == 1 ||
           (RX_DESC_GET_OWN_BIT(&(swRxDesc->shadowRxDesc)) == RX_DESC_CPU_OWN &&
            RX_DESC_GET_FIRST_BIT(&(swRxDesc->shadowRxDesc)) != 1))
    {
        swN2fRxDesc->rxDesc->buffPointer = swRxDesc->rxDesc->buffPointer;

        GT_SYNC;

        RX_DESC_RESET_MAC(devNum,swN2fRxDesc->rxDesc);

        GT_SYNC;

        swN2fRxDesc = swN2fRxDesc->swNextDesc;

        swRxDesc = swRxDesc->swNextDesc;
        swRxDesc->shadowRxDesc.word1 = prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->word1);
        swRxDesc->shadowRxDesc.word2 = prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->word2);
    }
    
    #ifndef __AX_PLATFORM__ /* by qinhs@autelan.com 2008-11-24 for DEBUG */
#if 0
	if(errExists) 
		osPrintfEvt("device %d cpu queue %d rx resource error actually found\n",devNum, queueIdx);
#endif
    #endif

    /* update the Rx desc list if error was detected */
    rxDescList->next2Receive = swRxDesc;
    rxDescList->next2Return  = swN2fRxDesc;

    /* Enable the Rx SDMA only if there are free descriptors in the Rx queue. */
    if (rxDescList->freeDescNum > 0)
    {
        rxSdmaRegMask = 1 << queueIdx;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regsAddr->sdmaRegs.rxQCmdReg, rxSdmaRegMask);
        if(rc != GT_OK)
            return rc;
    }

    rxDescList->forbidQEn = GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaRxPacketGet
*
* DESCRIPTION:
*       This function returns packets from PP destined to the CPU port.
*        -- SDMA relate.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received (0..7).
*       numOfBuffPtr- Num of buffs in packetBuffsArrPtr.
*
* OUTPUTS:
*       numOfBuffPtr        - Num of used buffs in packetBuffsArrPtr.
*       packetBuffsArrPtr   - (pointer to)The received packet buffers list.
*       buffLenArr          - List of buffer lengths for packetBuffsArrPtr.
*       rxParamsPtr  - (Pointer to)information parameters of received packets
*
* RETURNS:
*       GT_NO_MORE               - no more packets on the device/queue
*       GT_OK                    - packet got with no error.
*       GT_FAIL                  - failed to get the packet
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong devNum,portNum,queueIdx.
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_DSA_PARSING_ERROR     - DSA tag parsing error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It is recommended to call cpssDxChNetIfRxBufFree for this queue
*       i.e. return the buffer to their original queue.
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaRxPacketGet
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    INOUT GT_U32                            *numOfBuffPtr,
    OUT GT_U8                               *packetBuffsArrPtr[],
    OUT GT_U32                              buffLenArr[],
    OUT CPSS_DXCH_NET_RX_PARAMS_STC         *rxParamsPtr
)
{


    osPrintfDbg("\n************* get get get !!!!!  **************\n");	
    GT_U32              i;              /* iterator                         */
    GT_STATUS           rc;             /* return code                      */
    PRV_CPSS_DXCH_MODULE_CONFIG_STC *moduleCfgPtr; /* pointer to module configuration */
    PRV_CPSS_RX_DESC_LIST_STC   *rxDescList;
    PRV_CPSS_SW_RX_DESC_STC   *firstRxDesc;   /* First Rx descriptor        */
    PRV_CPSS_SW_RX_DESC_STC   *swRxDesc;      /* Rx descriptor              */
    GT_U32              descNum;        /* Num of desc this packet occupies */
    GT_U32              packetLen;      /* Length of packet in bytes        */
    GT_U32              bufferSize;     /* Size of a single buffer.         */
    GT_U32              numOfBufs;      /* Number of buffers in packet      */
    GT_U32              temp;           /* temporary word                   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queueIdx);
    CPSS_NULL_PTR_CHECK_MAC(numOfBuffPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetBuffsArrPtr);
    CPSS_NULL_PTR_CHECK_MAC(buffLenArr);
    CPSS_NULL_PTR_CHECK_MAC(rxParamsPtr);

    /*check that the DB of networkIf library was initialized*/
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);

    moduleCfgPtr = PRV_CPSS_DXCH_DEV_MODULE_CFG_MAC(devNum);

    if((PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
        && !PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {	
        firstRxDesc = NULL;
        rxDescList = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList[queueIdx]);

        if(rxDescList->freeDescNum == 0)
        {
            *numOfBuffPtr = 0;
		    osPrintfErr("no free descriptor for packet from device %d queue %d\n",devNum,queueIdx);
            return GT_NO_MORE;
        }

        swRxDesc = rxDescList->next2Receive;

        swRxDesc->shadowRxDesc.word1 = prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->word1);
        swRxDesc->shadowRxDesc.word2 = prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->word2);

        /* Handle resource error if happened. */
        if ( RX_DESC_GET_REC_ERR_BIT(&(swRxDesc->shadowRxDesc)) == 1 ||
             ( RX_DESC_GET_OWN_BIT(&(swRxDesc->shadowRxDesc)) == RX_DESC_CPU_OWN &&
               RX_DESC_GET_FIRST_BIT(&(swRxDesc->shadowRxDesc)) != 1))
        {
            /* handle SDMA error */
            dxChNetIfSdmaRxResErrPacketHandle(devNum,queueIdx);

            /* update the shadow pointer , due to SDMA error */
            swRxDesc = rxDescList->next2Receive;

            swRxDesc->shadowRxDesc.word1 = prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->word1);
            swRxDesc->shadowRxDesc.word2 = prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->word2);
        }

        /* No more Packets to process, return */
        if(RX_DESC_GET_OWN_BIT(&(swRxDesc->shadowRxDesc)) != RX_DESC_CPU_OWN)
        {
            return GT_NO_MORE;
        }

        if(RX_DESC_GET_FIRST_BIT(&(swRxDesc->shadowRxDesc)) == 0x0)
        {
		    osPrintfErr("packet from device %d queue %d with wrong descriptor[%#0x]: no first bit\r\n", \
					devNum,queueIdx,swRxDesc->shadowRxDesc.word1);
            return (GT_STATUS)GT_ERROR;
        }

        descNum     = 1;
        firstRxDesc = swRxDesc;

        if(RX_DESC_GET_REC_ERR_BIT(&(swRxDesc->shadowRxDesc)) == 1)
        {
		    osPrintfErr("packet from device %d queue %d with wrong descriptor [%#0x]: error bit set\r\n", \
					devNum,queueIdx,swRxDesc->shadowRxDesc.word1,swRxDesc->shadowRxDesc.word2, \
					swRxDesc->shadowRxDesc.buffPointer,swRxDesc->shadowRxDesc.nextDescPointer);
            return GT_FAIL;
        }

        /* Get the packet's descriptors.        */
        while(RX_DESC_GET_LAST_BIT(&(swRxDesc->shadowRxDesc)) == 0)
        {
            swRxDesc = swRxDesc->swNextDesc;
            swRxDesc->shadowRxDesc.word1 = prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->word1);
            swRxDesc->shadowRxDesc.word2 = prvCpssDrvHwByteSwap(devNum, swRxDesc->rxDesc->word2);
		    osPrintfErr("packet from device %d queue %d descriptor%d [%#0x %#0x %#0x %#0x]\n", \
					devNum,queueIdx,descNum,swRxDesc->shadowRxDesc.word1,swRxDesc->shadowRxDesc.word2, \
					swRxDesc->shadowRxDesc.buffPointer,swRxDesc->shadowRxDesc.nextDescPointer);
            descNum++;

            if(RX_DESC_GET_REC_ERR_BIT(&(swRxDesc->shadowRxDesc)) == 1)
            {
                return GT_FAIL;
            }
        }

        rxDescList->next2Receive = swRxDesc->swNextDesc;
        rxDescList->freeDescNum -= descNum;

        /* Filling the src port group lport were the frame was received */
        swRxDesc = firstRxDesc;
        bufferSize  = RX_DESC_GET_BUFF_SIZE_FIELD_MAC(&(swRxDesc->shadowRxDesc));
        packetLen   = RX_DESC_GET_BYTE_COUNT_FIELD(&(swRxDesc->shadowRxDesc));

        numOfBufs   = 0;

        /* Validate that the packet array length is big enough. */
        if (descNum > *numOfBuffPtr)
        {
            /* Free all buffers -- free buffers one by one */
            for(i = 0; i < descNum ; i++)
            {
                temp = swRxDesc->rxDesc->buffPointer;
                cpssOsPhy2Virt(prvCpssDrvHwByteSwap( devNum, temp), (GT_U32*)(&(packetBuffsArrPtr[0])) );
                swRxDesc = swRxDesc->swNextDesc;
                dxChNetIfRxBufFree(devNum, queueIdx,&packetBuffsArrPtr[0], 1);
            }
            #ifndef __AX_PLATFORM__ /* TBD:free all buffer,so no need to send to kernel ::by qinhs*/
    		*numOfBuffPtr = 0;
    		#endif
    		osPrintfErr("packet from device %d queue %d but descriptor run out in sw,free buffer\r\n",devNum,queueIdx);

            return GT_BAD_SIZE;
        }

        /* received packet strip out 4-Byte CRC appended */
    	#ifndef __AX_PLATFORM__ /* by qinhs@autelan.com 2009-4-17 */
    	if(packetLen > 4) {
    		packetLen -= 4; 
    	}
    	#endif

        /* build the array of buffers , and array of buffer length*/
        for(i = 0; i < descNum ; i++)
        {
            if(packetLen > bufferSize)
            {
                buffLenArr[i] = bufferSize;
            }
            else
            {
                buffLenArr[i] = packetLen;
            }
            packetLen -= buffLenArr[i];

            temp = swRxDesc->rxDesc->buffPointer;
            cpssOsPhy2Virt(prvCpssDrvHwByteSwap(devNum, temp), (GT_U32*)(&(packetBuffsArrPtr[i])) );
            /* Invalidate data cache for cached buffer */
            if(GT_TRUE == moduleCfgPtr->netIfCfg.rxBufInfo.buffersInCachedMem)
            {
                /* invalidate data cache */
                cpssExtDrvMgmtCacheInvalidate(CPSS_MGMT_DATA_CACHE_E, packetBuffsArrPtr[i], buffLenArr[i]);
            }

            swRxDesc  = swRxDesc->swNextDesc;
        }

        numOfBufs += descNum;

        if(buffLenArr[descNum - 1] == 0)
        {
            dxChNetIfRxBufFree(devNum,queueIdx,&(packetBuffsArrPtr[descNum - 1]), 1);
            numOfBufs--;
        }

        /* update the actual number of buffers in packet */
        *numOfBuffPtr = numOfBufs;

        return dxChNetIfRxPacketGet(devNum, queueIdx, numOfBuffPtr, packetBuffsArrPtr, buffLenArr,
                    rxParamsPtr);
    }
    else if((PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
            && PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        rc = prvCpssDxChNetIfMiiRxGet(devNum, queueIdx, numOfBuffPtr, packetBuffsArrPtr,
                                             buffLenArr, rxParamsPtr);
        if (rc == GT_OK)
        {
            /* In SDMA CRC is not calculated, */
            /* instead 0x55555555 constant is used. */
            /* This code emulates SDMA behavior. */
            temp = buffLenArr[*numOfBuffPtr - 1];
            packetBuffsArrPtr[*numOfBuffPtr - 1][temp-1] = 0x55;
            packetBuffsArrPtr[*numOfBuffPtr - 1][temp-2] = 0x55;
            packetBuffsArrPtr[*numOfBuffPtr - 1][temp-3] = 0x55;
            packetBuffsArrPtr[*numOfBuffPtr - 1][temp-4] = 0x55;
        }
        return rc;
    }

    /* should not arrive here */
    return GT_BAD_STATE;
}


/*******************************************************************************
* cpssDxChNetIfPrePendTwoBytesHeaderSet
*
* DESCRIPTION:
*       Enables/Disable pre-pending a two-byte header to all packets forwarded
*       to the CPU (via the CPU port or the PCI interface).
*       This two-byte header is used to align the IPv4 header to 32 bits.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum    - device number
*        enable    - GT_TRUE  - Two-byte header is pre-pended to packets
*                               forwarded to the CPU.
*                    GT_FALSE - Two-byte header is not pre-pended to packets
*                               forward to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPrePendTwoBytesHeaderSet
(
    IN  GT_U8                           devNum,
    IN  GT_BOOL                         enable
)
{
    GT_STATUS   rc;
    GT_U32  regAddr;         /* The register address to write to.    */
    GT_U32  data;
    GT_U32  bitIndex;/* bit index*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

/* This WA should not be performed in simulation */
#ifndef ASIC_SIMULATION
    if (!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
#endif
    {
    data = (enable == GT_TRUE) ? 1 : 0;
    if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.cscdHeadrInsrtConf[0];
        bitIndex = 28;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.hdrAltGlobalConfig;
        bitIndex = 15;
    }

    rc = prvCpssDrvHwPpSetRegField(devNum,regAddr,bitIndex,1,data);
    }
#ifndef ASIC_SIMULATION
    else
    {
        /* In case of SDMA emulation do not prepend on PP side, */
        /* this will disable DSA tag recognition on CPU side */
        /* Make the IP alignment on CPU side */
        rc = cpssExtDrvEthPrePendTwoBytesHeaderSet(enable);
    }
#endif
    if(rc == GT_OK)
    {
        /* save info to the DB */
        PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.prePendTwoBytesHeader = enable;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfPrePendTwoBytesHeaderGet
*
* DESCRIPTION:
*       Get the Enable/Disable status of pre-pending a two-byte header to all
*       packets forwarded to the CPU (via the CPU port or the PCI interface).
*       This two-byte header is used to align the IPv4 header to 32 bits.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum    - device number
*
* OUTPUTS:
*        enablePtr -  (pointer to) GT_TRUE  - Two-byte header is pre-pended
*                                  to packets forwarded to the CPU.
*                    GT_FALSE - Two-byte header is not pre-pended to packets
*                               forward to the CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPrePendTwoBytesHeaderGet
(
    IN  GT_U8        devNum,
    OUT  GT_BOOL    *enablePtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    /* get info from the DB */
    *enablePtr = PRV_CPSS_DXCH_PP_MAC(devNum)->netIf.prePendTwoBytesHeader;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaRxCountersGet
*
* DESCRIPTION:
*       For each packet processor, get the Rx packet counters from its SDMA
*       packet interface.  Return the aggregate counter values for the given
*       traffic class queue. -- SDMA relate
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*     devNum    - device number
*     queueIdx  - traffic class queue (0..7)
*
* OUTPUTS:
*     rxCountersPtr   - (pointer to) rx counters on this queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on bad parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Counters are reset on every read.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaRxCountersGet
(
    IN    GT_U8      devNum,
    IN    GT_U8      queueIdx,
    OUT   CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC *rxCountersPtr
)
{
    GT_U32  regAddr;        /* The register address to write to.    */
    GT_U32  data;           /* Data read from the register.         */
    GT_STATUS rc = GT_OK;
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(rxCountersPtr);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queueIdx);

    if((PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
        && !PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);


        /* we will use the 'First active port group' , to represent the whole device.
           that way we allow application to give SDMA memory to single port group instead
           of split it between all active port groups
        */
        portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.rxDmaPcktCnt[queueIdx];
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,&data);
        if(rc != GT_OK)
            return rc;
        rxCountersPtr->rxInPkts = data;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.rxDmaByteCnt[queueIdx];
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,&data);
        if(rc != GT_OK)
            return rc;
        rxCountersPtr->rxInOctets = data;
    }
    else if((PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
            && PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        rc = prvCpssGenNetIfMiiRxQueueCountersGet(queueIdx,
                                                  &(rxCountersPtr->rxInPkts),
                                                  &(rxCountersPtr->rxInOctets));
        if (rc != GT_OK)
        {
            return rc;
        }

        /* emulate ROC */
        return prvCpssGenNetIfMiiRxQueueCountersClear(queueIdx);

    }
    else /* PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E, PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_NONE_E */
        return GT_BAD_STATE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaRxErrorCountGet
*
* DESCRIPTION:
*       Returns the total number of Rx resource errors that occurred on a given
*       Rx queue . -- SDMA relate
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       rxErrCountPtr  - (pointer to) The total number of Rx resource errors on
*                        the device for all the queues.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The counters returned by this function reflects the number of Rx errors
*       that occurred since the last call to this function.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaRxErrorCountGet
(
    IN GT_U8    devNum,
    OUT CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC  *rxErrCountPtr
)
{
    GT_U32      regAddr;    /* The register address to read from.   */
    GT_U32      data;       /* Data read from register.             */
    GT_U32      ii;
    GT_STATUS   rc = GT_OK;
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(rxErrCountPtr);

    if((PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
        && !PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);


        /* we will use the 'First active port group' , to represent the whole device.
           that way we allow application to give SDMA memory to single port group instead
           of split it between all active port groups
        */
        portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

        /***************************************************************************
           the registers are "Clear on read" (Read only Clear - ROC)
           but the  counters are only 8 bits , and 4 queue counters in single
           register  -> so read for one counter will reset the values for the other
           3 counters in the register --> that is why the API return all 8 queue
           counters in single action !
        ***************************************************************************/

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.rxDmaResErrCnt[0];
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &data);
        if(rc != GT_OK)
            return rc;
        /* set the counters of Queues 0..3 */
        for(ii = 0 ; ii < (CPSS_TC_RANGE_CNS / 2) ; ii++)
        {
            rxErrCountPtr->counterArray[ii] = U32_GET_FIELD_MAC(data,8*ii,8);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.rxDmaResErrCnt[1];
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &data);
        if(rc != GT_OK)
            return rc;
        /* set the counters of Queues 4..7 */
        for(ii = 0 ; ii < (CPSS_TC_RANGE_CNS / 2) ; ii++)
        {
            rxErrCountPtr->counterArray[(ii + 4)] = U32_GET_FIELD_MAC(data,8*ii,8);
        }
    }
    else if((PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
            && PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        for(ii = 0 ; ii < CPSS_TC_RANGE_CNS; ii++)
            rxErrCountPtr->counterArray[ii] = 0;
    }
    else /* PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E, PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_NONE_E */
        return GT_BAD_STATE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaRxQueueEnable
*
* DESCRIPTION:
*       Enable/Disable the specified traffic class queue for RX
*       on all packet processors in the system. -- SDMA relate
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*        devNum    - device number
*        queue     - traffic class queue (0..7)
*        enable    - GT_TRUE, enable queue
*                    GT_FALSE, disable queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaRxQueueEnable
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    IN  GT_BOOL                         enable
)
{
    GT_U32  regAddr;        /* The register address to write to.    */
    GT_U32  data;
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queue);

    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);


    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.rxQCmdReg;

    if (enable == GT_TRUE)
    {
        data = (1 << queue);
    }
    else
    {
        data = (1 << (8 + queue));
    }

    return prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, data);
}


#ifndef __AX_PLATFORM__
/*******************************************************************************
* cpssDxChNetIfSdmaRxMibGet
*
* DESCRIPTION:
*       This function get SDMA Rx channel packet/octets and resource error counters.
*       
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*     devNum  - The device that was removed.
*
* OUTPUTS:
*     statistics - all SDMA queues' mib counter info.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - one of the parameters value is wrong
*	  GT_BAD_PTR - output parameter with null pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaRxMibGet
(
	IN GT_U8	devNum,
	OUT CPSS_DXCH_NET_SDMA_RX_MIB_STC **statistics
)
{
    GT_STATUS result = GT_OK;
	GT_U8	queueIdx = 0;
	CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC rxCounter;
	CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC rxErrCounter;	
	CPSS_DXCH_NET_SDMA_RX_MIB_STC *stats = NULL;

	memset(&rxCounter, 0 ,sizeof(CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC));
	memset(&rxErrCounter, 0 ,sizeof(CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC));

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
		
	if(!statistics || !(*statistics)) 
		return GT_BAD_PTR;

	/* read resource errors */
	result = cpssDxChNetIfSdmaRxErrorCountGet(devNum,&rxErrCounter);
	if(GT_OK != result) {
		osPrintfErr("device %d get cpu port resource error counter error %d\n",devNum,result);
	}
	
	for(queueIdx = 0; queueIdx < CPSS_TC_RANGE_CNS; queueIdx++) {
		stats = &((*statistics)[queueIdx]);
		/* read packet/octets counter */
		result = cpssDxChNetIfSdmaRxCountersGet(devNum,queueIdx,&rxCounter);
		if(GT_OK != result) {
			osPrintfErr("device %d get cpu port sdma%d mib counter error %d\n",devNum, queueIdx, result);
		}
		else {
			stats->rxInPkts = rxCounter.rxInPkts;
			stats->rxInOctets = rxCounter.rxInOctets;
			stats->resErrCounter = rxErrCounter.counterArray[queueIdx];
		}
		osPrintfDbg("dma%d rx %#x pkts %#x octets %#x errors\n", \
				queueIdx, stats->rxInPkts, stats->rxInOctets, stats->resErrCounter);
	}
	
	return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaRxMibClear
*
* DESCRIPTION:
*       This function clear SDMA Rx channel packet/octets and resource error counters, as
*  those mib counter registers are reset on every read.
*       
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*     devNum  - The device that was removed.
*
* OUTPUTS:
*     NULL
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - one of the parameters value is wrong
*	  GT_BAD_PTR - output parameter with null pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaRxMibClear
(
	IN GT_U8	devNum
)
{
	GT_STATUS result = GT_OK;
	GT_U8	queueIdx = 0;
	CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC rxCounter;
	CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC rxErrCounter;	

	memset(&rxCounter, 0 ,sizeof(CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC));
	memset(&rxErrCounter, 0 ,sizeof(CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC));

	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

	/* read resource errors */
	result = cpssDxChNetIfSdmaRxErrorCountGet(devNum,&rxErrCounter);
	if(GT_OK != result) {
		osPrintfErr("device %d get cpu port resource error counter error %d\n",devNum,result);
		return result;
	}
	
	for(queueIdx = 0; queueIdx < CPSS_TC_RANGE_CNS; queueIdx++) {
		/* read packet/octets counter */
		result = cpssDxChNetIfSdmaRxCountersGet(devNum,queueIdx,&rxCounter);
		if(GT_OK != result) {
			osPrintfErr("device %d get cpu port sdma%d mib counter error %d\n",devNum, queueIdx, result);
			return result;
		}
		osPrintfDbg("Clear dma%d counter rx [%#x]pkts [%#x]octets [%#x]rxError\n", \
				queueIdx, rxCounter.rxInPkts, rxCounter.rxInOctets, rxErrCounter.counterArray[queueIdx]);
	}
	
	return GT_OK;
}

#endif


