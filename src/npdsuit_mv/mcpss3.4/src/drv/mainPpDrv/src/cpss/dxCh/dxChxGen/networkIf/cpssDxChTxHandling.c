/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTxHandling.c
*
* DESCRIPTION:
*       This file implements all needed functions for sending packets of upper
*       layer to the cpu port of the DXCH device (PP's Tx Queues).
*
* FILE REVISION NUMBER:
*       $Revision: 1.3 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>

#include <cpss/generic/networkIf/cpssGenNetIfMii.h>
#include <cpss/generic/networkIf/private/prvCpssGenNetworkIfMii.h>

/*******************************************************************************
* External usage environment parameters
*******************************************************************************/

/*******************************************************************************
* Internal usage environment parameters
*******************************************************************************/

/*******************************************************************************
* dxChNetIfSdmaTxPacketSend
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from the different
*       core Tx functions. Prepares them for the transmit operation, and
*       enqueues the prepared descriptors to the PP's tx queues. -- SDMA relate
*       function activates Tx SDMA , function doesn't wait for event of
*       "Tx buffer queue" from PP
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum       - Device number.
*       pcktParamsPtr- The internal packet params to be set into the packet
*                      descriptors.
*       buffList     - The packet data buffers list.
*       buffLenList  - A list of the buffers len in buffList.
*       numOfBufsPtr - Length of buffList.
*       synchronicSend - sent the packets synchronic (not return until the
*                        buffers can be free)
*                       GT_TRUE - send synchronic
*                       GT_FALSE - send asynchronous (not wait for buffers to be
*                                  free)
*        txDescListPtr - The Tx desc. list control structure
*
* OUTPUTS:
*        txDescListPtr - The Tx desc. list control structure
*        txDescListUpdatedPtr - (pointer to) a parameter indicating if the
*                               txDescList was updated and data structure need
*                               to be restored
*
* RETURNS:
*       GT_OK          - on success, or
*       GT_NO_RESOURCE - if there is not enough free elements in the fifo
*                        associated with the Event Request Handle.
*       GT_EMPTY       - if there are not enough descriptors to do the sending.
*       GT_ABORTED     - if command aborted (during shutDown operation)
*       GT_HW_ERROR    - when  synchronicSend = GT_TRUE and after transmission
*                        the last descriptor own bit wasn't changed for long time.
*       GT_BAD_PARAM   - the data buffer is longer than allowed.
*                        buffer data can occupied up to the maximum
*                        number of descriptors defined.
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_FAIL otherwise.
*
* COMMENTS:
*           Note: Each buffer should be at least 8 bytes long.
*           first buffer must be at least 24 bytes for tagged packet ,
*           20 for untagged packet
*
*           support both :
*           ===== regular DSA tag  ====
*           ===== extended DSA tag ====
*
*******************************************************************************/
static GT_STATUS dxChNetIfSdmaTxPacketSend
(
    IN GT_U8                             devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC      *pcktParamsPtr,
    IN GT_U8                            *buffList[],
    IN GT_U32                            buffLenList[],
    IN GT_U32                           *numOfBufsPtr,
    IN GT_BOOL                           synchronicSend,
    INOUT PRV_CPSS_TX_DESC_LIST_STC     *txDescListPtr,
    OUT GT_BOOL                         *txDescListUpdatedPtr
)
{

	osPrintfErr("\t\t***********buffList[0]:%p \n",buffList[0]);
	osPrintfErr("\t\t***********txDescListPtr->swTxDescBlock:%p \n",txDescListPtr->swTxDescBlock);
	osPrintfErr("\t\t***********txDescListPtr->next2Feed:%p \n",txDescListPtr->next2Feed);
	osPrintfErr("\t\t***********txDescListPtr->freeDescNum:%p \n",txDescListPtr->freeDescNum);
	osPrintfErr("\t\t***********txDescListPtr->poolId:%p \n",txDescListPtr->poolId);


	
    GT_STATUS rc;
    GT_U8 txQueue;              /* The tx queue to send this packet to. */
    PRV_CPSS_TX_DESC_STC *firstDesc;  /* The first descriptor of the          */
                                /* currently transmitted packet.        */
    PRV_CPSS_SW_TX_DESC_STC  *currSwDesc;  /* The current handled descriptor of*/
                                /* the currently transmitted packet.    */
    #ifndef __AX_PLATFORM__
    PRV_CPSS_SW_TX_DESC_STC *curSwDescPrint,*swDescPrnPtr;/* currently transmitted packet descriptors saved for print */
    unsigned int    tmpTxDmaCdpVal = {0};/* descriptor value read from txDmaCdp register */
    #endif
    PRV_CPSS_TX_DESC_STC  tmpTxDesc;  /* Temporary Tx descriptor used for     */
                                /* preparing the real descriptor data.  */
    PRV_CPSS_TX_DESC_STC  tmpFirstTxDesc; /* Temporary Tx descriptor used for */
                                /* preparing the real first descriptor  */
                                /* data.                                */
    GT_U32      descWord1;          /* The first word of the Tx descriptor. */
    GT_U32      tmpBuffPtr;         /* Holds the real buffer pointer.       */
    GT_U32      txCmdMask;          /* Mask for Tx command register.        */
    GT_U32      i;
    GT_U8       numOfAddedBuffs;
    GT_U32      dataOffset;
    GT_U32      dsaTagNumOfBytes = 8;/* extended DSA tag size */
    GT_U32      numOfBufs;
    GT_32       key;
    GT_U32      portGroupId;/* port group Id for multi-port-groups device support */
    GT_U32      packetLen = 0;  /* packet length including the DSA tag, */
                                /* not including padding if needed */
    GT_U32      byteBlockMask;  /* mask used to calculate if padding needed due to */
                                /* troublesome packet length */
    
    #ifndef __AX_PLATFORM__
    unsigned char printBuf[64] = {0},*curPtr = printBuf;
    int length = 0;
    #endif
    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    /* Set to 0, in the function body will be updated to real number of used buffers. */
    numOfBufs = *numOfBufsPtr;
    *numOfBufsPtr = 0;

    /* Set tx parameters */
    txQueue     = pcktParamsPtr->sdmaInfo.txQueue;

    /* the descriptors not changed yet */
    *txDescListUpdatedPtr = GT_FALSE;

    if(pcktParamsPtr->dsaParam.commonParams.dsaTagType ==
       CPSS_DXCH_NET_DSA_TYPE_REGULAR_E)
    {
        dsaTagNumOfBytes = 4;
    }

    /* Get the Data after Source MAC Address and Destination MAC Address */
    if( pcktParamsPtr->packetIsTagged == GT_TRUE )
    {
		#if 0
        dataOffset = PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS + /*12*/
                     PRV_CPSS_DXCH_ETHPRT_TAGGED_PCKT_FLAG_LEN_CNS;/* 4 */
        #endif
		/* from 275 */
        dataOffset = PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS; /*12*/
		
	}
    else
    {
        dataOffset = PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS; /*12*/
    }

    /* Number of buffers added - 1 for the DSA Tag, 1 for split data after */
    /* the Source MAC Address and Destination MAC Address */
    numOfAddedBuffs = 2;
#if 0
    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E))
    {
        /* calc packet length */
        packetLen = dsaTagNumOfBytes + PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS - dataOffset;
        for( i = 0 ; i < numOfBufs ; i++ )
        {
            packetLen += buffLenList[i];
        }
    
        /* add 8 byte padding (another buffer is added, numOfAddedBuffs = 3) should */
        /* be done only if packets length is <byteblock>*n+k (1<=k<=8) (including   */
        /* DSA tag), where n and k are integers and <byteblock> is 256 for xCat */
        /* and 512 for DxChLion,                                  */
        /* otherwise (numOfAddedBuffs = 2) and previous flow is preserved.          */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            byteBlockMask = 0x1FF;
        }
        else /* xCat A1 */
        {
            byteBlockMask = 0xFF;
        }
        
        if( (1 <= (packetLen & byteBlockMask)) && (8 >= (packetLen & byteBlockMask)) )
        {
            numOfAddedBuffs = 3;
        }
    }

    if((numOfBufs + numOfAddedBuffs) > txDescListPtr->maxDescNum)
    {
        return GT_BAD_PARAM;
    }
#endif

    /* check available buffers in pool */
    if (cpssBmPoolBufFreeCntGet(txDescListPtr->poolId) < 1)
    {
        return GT_NO_RESOURCE;
    }

    /* Check if there are enough descriptors for this packet. */
    if((numOfBufs + numOfAddedBuffs) > txDescListPtr->freeDescNum)
    {
        if(PRV_CPSS_PP_MAC(devNum)->useIsr == GT_FALSE)
        {
            /* the device was NOT bound to the ISR ,
               and the EVENTs are not handled by the cpss */

            /* return 'Empty' to tell the application , to take care of the
               descriptors , because we have not enough descriptor to do the 'TX'
            */
            /* note : we must not call the
                prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle , because this
                function wants to use the EVENTs mechanism of the CPSS, by
                releasing the descriptors but also sending the 'cookie' info to
                the events queue handling
            */

            /* Application should call function cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet
               to release the descriptors ,and get the 'cookie' indication */
            return GT_EMPTY;
        }
        else /* the device was bound to the ISR */
        {
            /* Call prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle to free buffers.
            Must be under interrupt disabled */
            cpssOsSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &key);
            rc = prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle(devNum, txQueue);
            cpssOsSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &key);
            if(rc != GT_OK)
                return rc;
            /* Check again if there are enough descriptors for this packet. */
            if((numOfBufs + numOfAddedBuffs) > txDescListPtr->freeDescNum)
                return GT_EMPTY;
        }
    }

    /* the descriptors start to change here */
    *txDescListUpdatedPtr = GT_TRUE;

    /* Set descriptor parameters */
    currSwDesc              = txDescListPtr->next2Feed;
    #ifndef __AX_PLATFORM__
	curSwDescPrint			= currSwDesc;
    #endif
    firstDesc               = currSwDesc->txDesc;
    /* Set the cookie in the userData - usually the packet pointer received by
       the application */
    currSwDesc->userData = pcktParamsPtr->cookie;
    currSwDesc->evReqHndl= pcktParamsPtr->sdmaInfo.evReqHndl;

    /***********************************************/
    /* start with the original first buffer's data */
    /***********************************************/
    descWord1 = 0;

    if(pcktParamsPtr->sdmaInfo.recalcCrc == GT_TRUE)
    {
        descWord1 |= (1 << 12);
    }

    /* Set the first descriptor parameters. */
    TX_DESC_RESET_MAC(&tmpFirstTxDesc);

    tmpFirstTxDesc.word1 = descWord1;

    /* Set bit for first buffer of a frame for Temporary Tx descriptor */
    TX_DESC_SET_FIRST_BIT_MAC(&tmpFirstTxDesc,1);
    /* Add Source MAC Address and Destination MAC Address */
    TX_DESC_SET_BYTE_CNT_MAC(&tmpFirstTxDesc,PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS); /*12*/

    /* update the packet header to the first descriptor */
    cpssOsVirt2Phy((GT_U32)buffList[0],&tmpBuffPtr);
    tmpFirstTxDesc.buffPointer = prvCpssDrvHwByteSwap(devNum, tmpBuffPtr);

	osPrintfErr("\t\t***********buffList[0]:%p \n",buffList[0]);
	osPrintfErr("\t\t***********tmpBuffPtr:%p \n",tmpBuffPtr);
	osPrintfErr("\t\t***********tmpFirstTxDesc.buffPointer:%p \n",tmpFirstTxDesc.buffPointer);

    
    #ifndef __AX_PLATFORM__
	for(i=0;i<PRV_CPSS_DXCH_ETHPRT_HDR_MAC_SIZE_CNS;i++)
	{
		length += sprintf(curPtr,"%02x ",buffList[0][i]);
		curPtr = printBuf + length;
	}
	osPrintfPktTx("MAC %s\n",printBuf);
    #endif

    /***************/
    /* Add DSA Tag */
    /***************/
    currSwDesc = currSwDesc->swNextDesc;

	osPrintfErr("\t\t****222222******currSwDesc:%p \n",currSwDesc);


    TX_DESC_RESET_MAC(&tmpTxDesc);
    tmpTxDesc.word1 = descWord1;

    /* Use Short buffer in this case*/
    /* Build the DSA Tag info into the short buffer of the descriptor */
    rc = cpssDxChNetIfDsaTagBuild(devNum,
                                     &pcktParamsPtr->dsaParam,
                                     &currSwDesc->shortBuffer[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    cpssOsVirt2Phy((GT_U32)(currSwDesc->shortBuffer),&tmpBuffPtr);
    tmpTxDesc.buffPointer = prvCpssDrvHwByteSwap(devNum, tmpBuffPtr);
	osPrintfErr("\t\t***********tmpTxDesc.buffPointer:0x%p \n",tmpTxDesc.buffPointer);

    currSwDesc->userData = pcktParamsPtr->cookie;
    /* Set byte counter to DSA Tag size */
    TX_DESC_SET_BYTE_CNT_MAC(&tmpTxDesc, dsaTagNumOfBytes);
    /* Set ownership bit - owned by DMA */
    TX_DESC_SET_OWN_BIT_MAC(&tmpTxDesc,TX_DESC_DMA_OWN);

    tmpTxDesc.word1 = prvCpssDrvHwByteSwap(devNum, tmpTxDesc.word1);
    tmpTxDesc.word2 = prvCpssDrvHwByteSwap(devNum, tmpTxDesc.word2);
    TX_DESC_COPY_MAC(currSwDesc->txDesc,&tmpTxDesc);

	osPrintfErr("\t\t**22222222*********currSwDesc->txDesc:0x%p \n",currSwDesc->txDesc);


    
    #ifndef __AX_PLATFORM__
    memset(printBuf,0,64);
    curPtr = printBuf;
    length = 0;
    for(i=0;i<dsaTagNumOfBytes;i++)
    {
        length += sprintf(curPtr,"%02x ",currSwDesc->shortBuffer[i]);
        curPtr = printBuf + length;
    }
    osPrintfPktTx("DSA %s\n",printBuf);
    #endif

    /**************************************************/
    /* continue with the original first buffer's data */
    /**************************************************/
    currSwDesc = currSwDesc->swNextDesc;

	osPrintfErr("\t\t*33333********* currSwDesc:%p \n",currSwDesc);


    TX_DESC_RESET_MAC(&tmpTxDesc);
    tmpTxDesc.word1 = descWord1;

    /* Get the Data */
    cpssOsVirt2Phy((GT_U32)( ((GT_U8*)buffList[0]) + dataOffset ),&tmpBuffPtr);
    tmpTxDesc.buffPointer = prvCpssDrvHwByteSwap(devNum, tmpBuffPtr);
	
    currSwDesc->userData = pcktParamsPtr->cookie;
    /* Set byte counter to buffer size minus first descriptor */
    #if 0
    //TX_DESC_SET_BYTE_CNT_MAC(&tmpTxDesc, ( buffLenList[0] - dataOffset + 4 ));
    /* for the 4 bytes XCAT not send, we add the length of buffLenList, zhangdi */
    TX_DESC_SET_BYTE_CNT_MAC(&tmpTxDesc, ( buffLenList[0] - dataOffset /*+ 4*/ ));
    #endif
    /* 
        * Unknown reason lead to 4 bytes lost in Lion DMA transmit. 
        *                                                   luoxun@autelan.com
        */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        TX_DESC_SET_BYTE_CNT_MAC(&tmpTxDesc, ( buffLenList[0] - dataOffset + 4 ));
    }
    else
    {
        TX_DESC_SET_BYTE_CNT_MAC(&tmpTxDesc, ( buffLenList[0] - dataOffset ));
    }
    
    /* Set ownership bit - owned by DMA */
    TX_DESC_SET_OWN_BIT_MAC(&tmpTxDesc,TX_DESC_DMA_OWN);

    /* in case last descriptor don't swap
       need to set: Enable Interrupt bit, Last buffer of frame bit */
    if(numOfBufs != 1)
    {

		osPrintfErr("\t\t********** numOfBufs != 1 \n");
	
        tmpTxDesc.word1 = prvCpssDrvHwByteSwap(devNum, tmpTxDesc.word1);
        tmpTxDesc.word2 = prvCpssDrvHwByteSwap(devNum, tmpTxDesc.word2);
        TX_DESC_COPY_MAC(currSwDesc->txDesc,&tmpTxDesc);

        /*******************************************/
        /* continue with the original next buffers */
        /*******************************************/


        /* Add rest of the buffers (starting from second buffer) */
        for(i = 1; i < numOfBufs; i++)
        {

		    osPrintfErr("\t\t********** rest of the buffers \n");	
            currSwDesc = currSwDesc->swNextDesc;

            TX_DESC_RESET_MAC(&tmpTxDesc);
            tmpTxDesc.word1 = descWord1;

            /* Check if the buffers length is larger than (TX_SHORT_BUFF_SIZE)  */
            if(buffLenList[i] > TX_SHORT_BUFF_SIZE)
            {
                cpssOsVirt2Phy((GT_U32)(buffList[i]),&tmpBuffPtr);
            }
            else
            {
                cpssOsMemCpy(currSwDesc->shortBuffer,buffList[i],buffLenList[i]);
                cpssOsVirt2Phy((GT_U32)(currSwDesc->shortBuffer),&tmpBuffPtr);
            }

            tmpTxDesc.buffPointer = prvCpssDrvHwByteSwap(devNum, tmpBuffPtr);
            currSwDesc->userData = pcktParamsPtr->cookie;
            TX_DESC_SET_BYTE_CNT_MAC(&tmpTxDesc,buffLenList[i]);
            TX_DESC_SET_OWN_BIT_MAC(&tmpTxDesc,TX_DESC_DMA_OWN);

            /* in case last descriptor don't swap  */
            /* need to set: Enable Interrupt bit, Last buffer of frame bit */
            /* In case PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E is */
            /* to be done (3 == numOfAddedBuffs) the last buffer in the */
            /* received buffer list is not the last one.                */ 
            if((i != (numOfBufs - 1)) || (3 == numOfAddedBuffs) )
            {
                tmpTxDesc.word1 = prvCpssDrvHwByteSwap(devNum, tmpTxDesc.word1);
                tmpTxDesc.word2 = prvCpssDrvHwByteSwap(devNum, tmpTxDesc.word2);
                TX_DESC_COPY_MAC(currSwDesc->txDesc,&tmpTxDesc);
            }
        }
    }

    /* padded 8 bytes for PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E */
    if( 3 == numOfAddedBuffs )
    {
        if(numOfBufs == 1)
        {
            /* swapping the previous descriptor in case only one buffer was */
            /* supplied to the routine */
            tmpTxDesc.word1 = prvCpssDrvHwByteSwap(devNum, tmpTxDesc.word1);
            tmpTxDesc.word2 = prvCpssDrvHwByteSwap(devNum, tmpTxDesc.word2);
            TX_DESC_COPY_MAC(currSwDesc->txDesc,&tmpTxDesc);
        }

        currSwDesc = currSwDesc->swNextDesc;

        TX_DESC_RESET_MAC(&tmpTxDesc);
        tmpTxDesc.word1 = descWord1;

        /* padding with zeros */
        cpssOsMemSet(currSwDesc->shortBuffer, 0x0, 8);

        cpssOsVirt2Phy((GT_U32)(currSwDesc->shortBuffer),&tmpBuffPtr);
        tmpTxDesc.buffPointer = prvCpssDrvHwByteSwap(devNum, tmpBuffPtr);
        currSwDesc->userData = pcktParamsPtr->cookie;
        TX_DESC_SET_BYTE_CNT_MAC(&tmpTxDesc, 8);
        TX_DESC_SET_OWN_BIT_MAC(&tmpTxDesc,TX_DESC_DMA_OWN);

        /* last descriptor don't swap */
    }

    /**************************************/
    /* Set the last descriptor parameters */
    /**************************************/
    /* Set the bit for Last buffer of a frame */
    TX_DESC_SET_LAST_BIT_MAC(&tmpTxDesc,1);

    /* check if caller want to get the event invocation ,
       NOTE : we must not invoke when sending "synchronic" */
    if((pcktParamsPtr->sdmaInfo.invokeTxBufferQueueEvent == GT_TRUE) &&
       (synchronicSend == GT_FALSE))
    {
        /* Set the bit for Enable Interrupt */
        TX_DESC_SET_INT_BIT_MAC(&tmpTxDesc,1);
    }
    else
        /* Set the bit for Disable Interrupt */
        TX_DESC_SET_INT_BIT_MAC(&tmpTxDesc,0);

    tmpTxDesc.word1 = prvCpssDrvHwByteSwap(devNum, tmpTxDesc.word1);
    tmpTxDesc.word2 = prvCpssDrvHwByteSwap(devNum, tmpTxDesc.word2);
    TX_DESC_COPY_MAC(currSwDesc->txDesc,&tmpTxDesc);
	osPrintfErr("\t\t***33333***last descriptor********currSwDesc->txDesc:%p \n",currSwDesc->txDesc);
    
//#ifdef __AX_PLATFORM__
#ifndef __AX_PLATFORM__
    osPrintfErr("dxChNetIfSdmaTxPacketSend::last desc %#0x info\n",&tmpTxDesc);
    osPrintfErr("word1 %#0x word2 %#0x bufPtr %#0x nextDesc %#0x\n", \
            (volatile unsigned int)tmpTxDesc.word1, \
            (volatile unsigned int)tmpTxDesc.word2, \
            (volatile unsigned int)tmpTxDesc.buffPointer, \
            (volatile unsigned int)tmpTxDesc.nextDescPointer);
    osPrintfErr("::::::::::::::::::::::::PACKET::::::::::::::::::::::::\n");
	char packet[128] = {0};
	int outlen = 0;
    #ifndef __AX_PLATFORM__
    for(i=0;i<(buffLenList[0]-dataOffset);i++)
    {
        outlen += sprintf(packet+outlen, "%02x ",(((GT_U8*)buffList[0])+dataOffset)[i]);
		if(outlen >= 124){
			osPrintfErr("%s", packet);
			outlen = 0;
		}
    }
    #else	
    for(i=0;i<(buffLenList[0]);i++)
    {
        outlen += sprintf(packet+outlen, "%02x ",buffList[0][i]);
        if((i+1)%16==0)
            outlen += sprintf(packet+outlen, "\n");
		if(outlen >= 124){
			osPrintfErr("%s", packet);
			outlen = 0;
		}
    }
    #endif
    osPrintfErr("\n::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
#endif

    txDescListPtr->freeDescNum -= (numOfBufs + numOfAddedBuffs);
    txDescListPtr->next2Feed    = currSwDesc->swNextDesc;
    /* Set the sctual number of used buffers. */
    *numOfBufsPtr = numOfBufs + numOfAddedBuffs;

    /* Make sure that all previous operations where */
    /* executed before changing the own bit of the  */
    /* first descriptor.                            */
    GT_SYNC;


	osPrintfErr("\n***********start transmitting !!!!!!********** \n");

    /* Set the first descriptor own bit to start transmitting.  */
    TX_DESC_SET_OWN_BIT_MAC(&tmpFirstTxDesc,TX_DESC_DMA_OWN);
    tmpFirstTxDesc.word1    = prvCpssDrvHwByteSwap(devNum, tmpFirstTxDesc.word1);
    tmpFirstTxDesc.word2    = prvCpssDrvHwByteSwap(devNum, tmpFirstTxDesc.word2);
    TX_DESC_COPY_MAC(firstDesc,&tmpFirstTxDesc);



	osPrintfErr("\t\t***********firstDesc:%p \n",firstDesc);
	osPrintfErr("\t\t***********firstDesc->buffPointer:%p \n",firstDesc->buffPointer);
	osPrintfErr("\t\t***********firstDesc->nextDescPointer:%p \n",firstDesc->nextDescPointer);
	osPrintfErr("\t\t***********firstDesc->word1:%p \n",firstDesc->word1);
	osPrintfErr("\t\t***********firstDesc->word2:%p \n",firstDesc->word2);


    /* The Enable DMA operation should be done only */
    /* AFTER all desc. operations where completed.  */
    GT_SYNC;
    
    #ifdef __AX_PLATFORM__
    osPrintf("dxChNetIfSdmaTxPacketSend::before enable Tx DMA,txDesc %#0x info\n",curSwDescPrint->txDesc);
    osPrintf("===================================================\n");
    i = 0;
    swDescPrnPtr = curSwDescPrint;
    while(i<3)
    {
        osPrintf("word1 %#0x word2 %#0x bufPtr %#0x nextDescPtr %#0x owner is %s\n", \
                (volatile unsigned int)swDescPrnPtr->txDesc->word1, \
                (volatile unsigned int)swDescPrnPtr->txDesc->word2, \
                (volatile unsigned int)swDescPrnPtr->txDesc->buffPointer, \
                (volatile unsigned int)swDescPrnPtr->txDesc->nextDescPointer,   \
                TX_DESC_GET_OWN_BIT(devNum,swDescPrnPtr->txDesc) == TX_DESC_DMA_OWN)?"DMA":"CPU");
        swDescPrnPtr = swDescPrnPtr->swNextDesc;
        i++;
    }
    
    osPrintf("reg cdp[%d] value %#0x\n", txQueue,tmpTxDmaCdpVal);
    osPrintf("reg value %#0x TxENQ %#0x TxDISQ %#0x\n", \
                            tmpTxDmaCdpVal, tmpTxDmaCdpVal & 0xFF,(tmpTxDmaCdpVal>>8) & 0xFF);
    osPrintf("===================================================\n");
    #endif

    /* Enable the Tx DMA.   */
    txCmdMask = 1 << txQueue;
	osPrintfErr("\t\t***********txCmdMask:0x%x ******\n",txCmdMask);	
    rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txQCmdReg,txCmdMask);
    if(rc != GT_OK)
        return rc;

    if(synchronicSend == GT_TRUE)
    {
        GT_U32 loopIndex = (numOfBufs + numOfAddedBuffs) * 500;

        /* Wait until PP sent the packet. Or HW error if while block
                   run more then loopIndex times */
        /*luoxun --There is no need to combinate.*/
        while (loopIndex && (TX_DESC_GET_OWN_BIT_MAC(devNum,currSwDesc->txDesc) == TX_DESC_DMA_OWN))
        {
#ifdef ASIC_SIMULATION
            /* do some sleep allow the simulation process the packet */
            cpssOsTimerWkAfter(1);
#endif /*ASIC_SIMULATION*/
            loopIndex--;
        }

        if(loopIndex == 0)
        {
			/*zhangdi add osPrintf() have bug*/
		     osPrintfErr("Tx DMA on device %d send packet sync %d timeout,txDesc:0x%p\n", devNum,synchronicSend,currSwDesc->txDesc);	
	        /* end */
			return GT_HW_ERROR;
        }

    }

    osPrintfErr("***************jump out the loop !!!!!!!!!\n", devNum,synchronicSend,currSwDesc->txDesc);	


    #if 1   /* Add for debug */
	tmpTxDmaCdpVal = 0;
	/* read descriptor value from register */
	prvCpssDrvHwPpReadRegister(devNum,PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txDmaCdp[txQueue], \
											(unsigned int *)&tmpTxDmaCdpVal);

    osPrintfErr("reg txDmaCdp[%d] value %#0x\n", txQueue,tmpTxDmaCdpVal); 
	
	tmpTxDmaCdpVal = 0;
	/* read Tx SDMA Queue Command Register */
	prvCpssDrvHwPpReadRegister(devNum,PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txQCmdReg, \
											(unsigned int *)&tmpTxDmaCdpVal);

	/* next2feed descriptor value */
	swDescPrnPtr = txDescListPtr->next2Feed;


	osPrintfErr("dxChNetIfSdmaTxPacketSend::after tx done,txDesc %#0x info\n", \
					curSwDescPrint->txDesc);
	osPrintfErr("===================================================\n");
	i = 0;
	swDescPrnPtr = curSwDescPrint;
	while(i<3)
	{
		osPrintfErr("word1 %#0x word2 %#0x bufPtr %#0x nextDescPtr %#0x owner is %s\n", \
				(volatile unsigned int)swDescPrnPtr->txDesc->word1, \
				(volatile unsigned int)swDescPrnPtr->txDesc->word2, \
				(volatile unsigned int)swDescPrnPtr->txDesc->buffPointer, \
				(volatile unsigned int)swDescPrnPtr->txDesc->nextDescPointer, \
				(TX_DESC_GET_OWN_BIT_MAC(devNum,swDescPrnPtr->txDesc) == TX_DESC_DMA_OWN)?"DMA":"CPU");
		swDescPrnPtr = swDescPrnPtr->swNextDesc;
		i++;
	}
	osPrintfErr("reg value %#0x TxENQ %#0x TxDISQ %#0x\n", \
							tmpTxDmaCdpVal,  tmpTxDmaCdpVal & 0xFF, (tmpTxDmaCdpVal>>8) & 0xFF);

	osPrintfErr("next desc:word1 %#0x word2 %#0x bufPtr %#0x nextDescPtr %#0x owner is %s\n", \
			(volatile unsigned int)swDescPrnPtr->txDesc->word1, \
			(volatile unsigned int)swDescPrnPtr->txDesc->word2, \
			(volatile unsigned int)swDescPrnPtr->txDesc->buffPointer, \
			(volatile unsigned int)swDescPrnPtr->txDesc->nextDescPointer, \
		(TX_DESC_GET_OWN_BIT_MAC(devNum,swDescPrnPtr->txDesc) == TX_DESC_DMA_OWN)?"DMA":"CPU");
	osPrintfErr("===================================================\n");
    #endif

	#if 0
    #ifdef __AX_PLATFORM__
    osPrintf("dxChNetIfSdmaTxPacketSend::after tx done,txDesc %#0x info\n", \
                    curSwDescPrint->txDesc);
    osPrintf("===================================================\n");
    i = 0;
    swDescPrnPtr = curSwDescPrint;
    while(i<3)
    {
        osPrintf("word1 %#0x word2 %#0x bufPtr %#0x nextDescPtr %#0x owner is %s\n", \
                (volatile unsigned int)swDescPrnPtr->txDesc->word1, \
                (volatile unsigned int)swDescPrnPtr->txDesc->word2, \
                (volatile unsigned int)swDescPrnPtr->txDesc->buffPointer, \
                (volatile unsigned int)swDescPrnPtr->txDesc->nextDescPointer, \
                (TX_DESC_GET_OWN_BIT(devNum,swDescPrnPtr->txDesc) == TX_DESC_DMA_OWN)?"DMA":"CPU");
        swDescPrnPtr = swDescPrnPtr->swNextDesc;
        i++;
    }
    osPrintf("reg cdp[%d] value %#0x\n", txQueue,tmpTxDmaCdpVal);
    osPrintf("reg value %#0x TxENQ %#0x TxDISQ %#0x\n", \
                            tmpTxDmaCdpVal,  tmpTxDmaCdpVal & 0xFF, (tmpTxDmaCdpVal>>8) & 0xFF);
    osPrintf("next desc:word1 %#0x word2 %#0x bufPtr %#0x nextDescPtr %#0x owner is %s\n", \
            (volatile unsigned int)swDescPrnPtr->txDesc->word1, \
            (volatile unsigned int)swDescPrnPtr->txDesc->word2, \
            (volatile unsigned int)swDescPrnPtr->txDesc->buffPointer, \
            (volatile unsigned int)swDescPrnPtr->txDesc->nextDescPointer, \
        (TX_DESC_GET_OWN_BIT(devNum,swDescPrnPtr->txDesc) == TX_DESC_DMA_OWN)?"DMA":"CPU");
    osPrintf("===================================================\n");
    #endif
    #endif

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxPacketSend
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from Application .
*       Prepares them for the transmit operation, and enqueues the prepared
*       descriptors to the PP's tx queues.  -- SDMA relate.
*       function activates Tx SDMA , function doesn't wait for event of
*       "Tx buffer queue" from PP
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum       - Device number.
*       pcktParamsPtr- The internal packet params to be set into the packet
*                      descriptors.
*       buffList     - The packet data buffers list.
*       buffLenList  - A list of the buffers len in buffList.
*       numOfBufs    - Length of buffList.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success, or
*       GT_NO_RESOURCE - if there is not enough free elements in the fifo
*                        associated with the Event Request Handle.
*       GT_EMPTY       - if there are not enough descriptors to do the sending.
*       GT_BAD_PARAM   - on bad DSA params or the data buffer is longer
*                        than allowed. Buffer data can occupied up to the
*                        maximum number of descriptors defined.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL otherwise.
*
* COMMENTS:
*    1. Support both regular DSA tag and extended DSA tag.
*    2. Each buffer must be at least 8 bytes long. First buffer must be at 
*       least 24 bytes for tagged packet, 20 for untagged packet.
*    3. If returned status is GT_NO_RESOURCE then the application should free
*       the elements in the fifo (associated with the Handle) by calling
*       cpssDxChNetIfTxBufferQueueGet, and send the packet again.
*    4. If returned status is GT_EMPTY and CPSS handling the events
*       of the device then the application should wait and try to send the 
*       packet again.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
{
    GT_STATUS rc = GT_OK;
    GT_U8                      txQueue;   /*The tx queue to send this packet to*/
    PRV_CPSS_TX_DESC_LIST_STC *txDescList;/*The Tx desc. list control structure*/
    GT_BOOL   txDescListUpdated;/* do we need to restore descriptors -->
                        dummy parameter not used for 'asynch sending'*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(pcktParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(buffList);
    CPSS_NULL_PTR_CHECK_MAC(buffLenList);
    PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(devNum);

    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);


    if(!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        /*check that the DB of networkIf library was initialized*/
        PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);
    
        txQueue     = pcktParamsPtr->sdmaInfo.txQueue;
        txDescList  = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);
    
        /* asynchronous sending */
        rc = dxChNetIfSdmaTxPacketSend(devNum, pcktParamsPtr, buffList, buffLenList,
                                       &numOfBufs, GT_FALSE, txDescList ,
                                       &txDescListUpdated);
    }
    else
    {
        rc = prvCpssGenNetIfMiiTxModeSet(PRV_CPSS_GEN_NETIF_MII_TX_MODE_ASYNCH_E);
        if (rc != GT_OK)
            return rc;

        rc = prvCpssDxChNetIfMiiPacketTx(devNum, pcktParamsPtr, buffList,
                                         buffLenList, numOfBufs);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfSdmaSyncTxPacketSend
*
*       This function receives packet buffers & parameters from Applications .
*       Prepares them for the transmit operation, and enqueues the prepared
*       descriptors to the PP's tx queues.
*       After transmition end all transmitted packets descriptors are freed.
*        -- SDMA relate.
*       function activates Tx SDMA , function wait for PP to finish processing
*       the buffers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum       - Device number.
*       pcktParamsPtr- The internal packet params to be set into the packet
*                      descriptors.
*       buffList     - The packet data buffers list.
*       buffLenList  - A list of the buffers len in buffList.
*       numOfBufs    - Length of buffList.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success, or
*       GT_NO_RESOURCE - if there is not enough free elements in the fifo
*                        associated with the Event Request Handle.
*       GT_EMPTY       - if there are not enough descriptors to do the sending.
*       GT_HW_ERROR    - when after transmission last descriptor own bit wasn't
*                        changed for long time.
*       GT_BAD_PARAM   - on bad DSA params or the data buffer is longer
*                        than allowed. Buffer data can occupied up to the
*                        maximum number of descriptors defined.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL otherwise.
*
* COMMENTS:
*    1. Support both regular DSA tag and extended DSA tag.
*    2. Each buffer must be at least 8 bytes long. First buffer must be at 
*       least 24 bytes for tagged packet, 20 for untagged packet.
*    3. If returned status is GT_NO_RESOURCE then the application should free
*       the elements in the fifo (associated with the Handle) by calling
*       cpssDxChNetIfTxBufferQueueGet, and send the packet again.
*    4. If returned status is GT_EMPTY and CPSS handling the events
*       of the device then the application should wait and try to send the 
*       packet again.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaSyncTxPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
)
{
    GT_STATUS rc0 = GT_OK;
    GT_STATUS rc1 = GT_OK;
    GT_BOOL   txDescListUpdated;/* do we need to restore descriptors */
    GT_U8 txQueue;              /* The tx queue to send this packet to. */
    PRV_CPSS_TX_DESC_LIST_STC *txDescList;   /* The Tx desc. list control structure. */
    PRV_CPSS_SW_TX_DESC_STC    *firstDescPtr;
    GT_U32                      numOfUsedDesc;

    GT_U32                      phyNext2Feed; /* The physicat address of the next2Feed*/
                                              /* field.                               */
    PRV_CPSS_DXCH_PP_REGS_ADDR_STC *regsAddr;
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(pcktParamsPtr);
    CPSS_NULL_PTR_CHECK_MAC(buffList);
    CPSS_NULL_PTR_CHECK_MAC(buffLenList);
    PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(devNum);

    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);


    if(!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        /*check that the DB of networkIf library was initialized*/
        PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);
    
        /* we will use the 'First active port group' , to represent the whole device.
           that way we allow application to give SDMA memory to single port group instead
           of split it between all active port groups
        */
        portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
    
        txQueue     = pcktParamsPtr->sdmaInfo.txQueue;
        txDescList  = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);

	osPrintfErr("\t\t***********intCtrl.txDescList[txQueue]:%p \n",&(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[0]));
	osPrintfErr("\t\t***********intCtrl.txDescList[txQueue]:%p \n",&(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[1]));
	osPrintfErr("\t\t***********intCtrl.txDescList[txQueue]:%p \n",&(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[2]));
	osPrintfErr("\t\t***********intCtrl.txDescList[txQueue]:%p \n",&(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[3]));
	osPrintfErr("\t\t***********intCtrl.txDescList[txQueue]:%p \n",&(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[4]));
	osPrintfErr("\t\t***********intCtrl.txDescList[txQueue]:%p \n",&(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[5]));
	osPrintfErr("\t\t***********intCtrl.txDescList[txQueue]:%p \n",&(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[6]));
	osPrintfErr("\t\t***********intCtrl.txDescList[txQueue]:%p \n",&(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[7]));


	
    
        /* dxChNetIfSdmaTxPacketSend changes txDescList->next2Feed, keep it here
        and return it back after cleaning. */
        firstDescPtr = txDescList->next2Feed;
    
        numOfUsedDesc = numOfBufs;
    
        /* synchronic sending */
        rc0 =  dxChNetIfSdmaTxPacketSend(devNum,pcktParamsPtr,buffList,buffLenList,
                                        &numOfUsedDesc, GT_TRUE, txDescList ,
                                        &txDescListUpdated);
    
        if (txDescListUpdated == GT_TRUE)
        {
            /* Don't check rc here, need to restore data structure*/
    
            /* Restore next to feed descriptor. */
            txDescList->next2Feed = firstDescPtr;
            txDescList->freeDescNum += numOfUsedDesc;
    
            /* Returned the buffers to application. Return the state of descriptors as
             it was before calling internalTxPacketSend. Set txDescList->next2Feed
             to point to the same SW_DESC before calling internalTxPacketSend
             Then return the status to the caller (even in case of error). */
            while(numOfUsedDesc > 0)
            {
                firstDescPtr->txDesc->word1 = 0x0;
                firstDescPtr->txDesc->word2 = 0x0;
                firstDescPtr = firstDescPtr->swNextDesc;
                numOfUsedDesc--;
            }
    
            /* Set/Restore Tx Current Desc Pointer Register. */
            regsAddr    = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum);
            cpssOsVirt2Phy((GT_U32)(txDescList->next2Feed->txDesc),&phyNext2Feed);
            rc1 = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regsAddr->sdmaRegs.txDmaCdp[txQueue],phyNext2Feed);
            if(rc1 != GT_OK)
                return rc1;
        }
    }
    else
    {

        rc0 = prvCpssGenNetIfMiiTxModeSet(PRV_CPSS_GEN_NETIF_MII_TX_MODE_SYNCH_E);
        if (rc0 != GT_OK)
            return rc0;

        rc0 = prvCpssDxChNetIfMiiPacketTx(devNum, pcktParamsPtr, buffList,
                                         buffLenList, numOfBufs);
    }
   	osPrintfErr("**********cpssDxChNetIfSdmaSyncTxPacketSend() over***\n\n");

    return rc0;
}

/*******************************************************************************
* prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle
*
* DESCRIPTION:
*       This routine frees all transmitted packets descriptors. In addition, all
*       user relevant data in Tx End FIFO.  -- SDMA relate.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum      - The device number the packet was transmitted from
*       queueIdx    - The queue the packet was transmitted upon.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - the CPSS does not handle the ISR for the device
*                                  so function must not be called
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Invoked by ISR routine !!
*       the function is called internally in the CPSS from the driver as a call
*       back , see bind in function hwPpPhase1Part1(...)
*
*******************************************************************************/
GT_STATUS prvCpssDxChNetIfSdmaTxBuffQueueEvIsrHandle
(
    IN GT_U8                devNum,
    IN GT_U8                queueIdx
)
{
    PRV_CPSS_TX_DESC_LIST_STC       *txDescListPtr;
    /* Counts the number of descriptors already freed. */
    GT_U32                          freeDescNum;
    PRV_CPSS_SW_TX_DESC_STC         *descPtr;
    /* Points to the first sw desc. to be sent to the callback function.*/
    PRV_CPSS_SW_TX_DESC_STC         *firstTxDescPtr;
    /* Number of descriptors this packet occupies to be sent to the callback
       function.*/
    GT_U32                          descNum;
    PRV_CPSS_TX_DESC_STC            tmpTxDesc;
    PRV_CPSS_SW_TX_FREE_DATA_STC    *txFreeDataPtr;
    PRV_CPSS_DRV_EVENT_HNDL_STC     *evHndlPtr;
    PRV_CPSS_TX_BUF_QUEUE_FIFO_STC  *fifoPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queueIdx);

    /*check that the DB of networkIf library was initialized*/
    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);

    txDescListPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[queueIdx]);

    if(PRV_CPSS_PP_MAC(devNum)->useIsr == GT_FALSE)
    {
        /* the CPSS not handling the ISR for the device so function must not be called */
        return GT_BAD_STATE;
    }

    descPtr = txDescListPtr->next2Free;

    tmpTxDesc.word1 = descPtr->txDesc->word1;

	osPrintfErr("tx buffer queue ISR called by device %d queue %d\n",devNum,queueIdx);
    /* No more descriptors to release.  */
    while (TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) != TX_DESC_DMA_OWN)
    {
        if (TX_DESC_GET_FIRST_BIT_MAC(devNum,&tmpTxDesc) == 0)
        {
            /* It may happen if all the Tx descriptors are free - this is legal case
               In this case - should not return GT_ERROR */
            if(txDescListPtr->freeDescNum == txDescListPtr->maxDescNum)
            {
                return GT_OK;
            }

            /* should not happen */
            return (GT_STATUS)GT_ERROR;
        }

        descNum     = 1;
        firstTxDescPtr = descPtr;
        evHndlPtr   = (PRV_CPSS_DRV_EVENT_HNDL_STC*)descPtr->evReqHndl;

        /* Get the packet's descriptors.        */
        while (TX_DESC_GET_LAST_BIT_MAC(devNum,&tmpTxDesc) == 0x0)
        {
            if (TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) == TX_DESC_DMA_OWN)
            {
                return GT_OK;
            }
            descPtr = descPtr->swNextDesc;
            tmpTxDesc.word1 = descPtr->txDesc->word1;
            descNum++;
        }

        if (TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) == TX_DESC_DMA_OWN)
        {
            return GT_OK;
        }

        /* for not control packets/master/stand alone/device that doesn't need trap2cpu WA */
        freeDescNum = descNum;

        if ((txFreeDataPtr = cpssBmPoolBufGet(txDescListPtr->poolId)) == NULL)
        {
            return GT_NO_RESOURCE;
        }

        /* save the Tx End data in FIFO node */
        txFreeDataPtr->devNum   = devNum;
        txFreeDataPtr->queueNum = queueIdx;
        txFreeDataPtr->userData = firstTxDescPtr->userData;
        txFreeDataPtr->nextPtr  = NULL;

        /* insert the new Tx end node into FIFO */
        fifoPtr = (PRV_CPSS_TX_BUF_QUEUE_FIFO_STC*)evHndlPtr->extDataPtr;

        if (NULL == fifoPtr->tailPtr)
        {
            fifoPtr->tailPtr = txFreeDataPtr;
            fifoPtr->headPtr = txFreeDataPtr;

            /* notify the user process ,or        */
            /* call the application's CB function */
            /* only for first packet in fifo.     */
            prvCpssDrvEvReqNotify(evHndlPtr);
        }
        else
        {
            fifoPtr->tailPtr->nextPtr = txFreeDataPtr;
            fifoPtr->tailPtr = txFreeDataPtr;
        }

        while(freeDescNum > 0)
        {
            firstTxDescPtr->txDesc->word1 = 0x0;

            firstTxDescPtr = firstTxDescPtr->swNextDesc;
            freeDescNum--;
        }

        txDescListPtr->freeDescNum += descNum;
        txDescListPtr->next2Free = firstTxDescPtr;
        if(txDescListPtr->next2Free == txDescListPtr->next2Feed)
            break;

        /* update the desc ponter for next iteration in while loop */
        descPtr = firstTxDescPtr;
        tmpTxDesc.word1 = descPtr->txDesc->word1;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxQueueEnable
*
* DESCRIPTION:
*       Enable/Disable the specified traffic class queue for TX
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
*       GT_BAD_STATE             - on CPU port is not SDMA
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxQueueEnable
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           queue,
    IN  GT_BOOL                         enable
)
{
    GT_U32  regAddr;/* The register address to write to.    */
    GT_U32  data;
    GT_U32  portGroupId;/* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);

    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queue);

    if(PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E &&
        !PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        /* we will use the 'First active port group' , to represent the whole device.
           that way we allow application to give SDMA memory to single port group instead
           of split it between all active port groups
        */
        portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);
    
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txQCmdReg;
    
        if (enable == GT_TRUE)
        {
            data = (1 << queue);
        }
        else
        {
            data = (1 << (8 + queue));
        }
    
        return prvCpssDrvHwPpPortGroupWriteRegister (devNum, portGroupId,regAddr, data);
    }
    else if (PRV_CPSS_PP_MAC(devNum)->cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E &&
             PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        return GT_OK;
    }

    return GT_BAD_STATE;
}

/*******************************************************************************
* cpssDxChNetIfTxBufferQueueGet
*
* DESCRIPTION:
*       This routine returns the caller the TxEnd parameters for a transmitted
*       packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       hndl        - Handle got from gtEventBind.
*
* OUTPUTS:
*       devPtr      - The device the packet was transmitted from.
*       cookiePtr   - The user cookie handed on the transmit request.
*       queuePtr    - The queue from which this packet was transmitted
*       statusPtr   - GT_OK if packet transmission was successful, GT_FAIL on
*                     packet reject.
*
* RETURNS:
*       GT_OK on success, or
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NO_MORE               - no more packets
*       GT_BAD_STATE             - the CPSS not handling the ISR for the device
*                                  so function must not be called
*       GT_NOT_INITIALIZED       - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfTxBufferQueueGet
(
    IN  GT_U32               hndl,
    OUT GT_U8               *devPtr,
    OUT GT_PTR              *cookiePtr,
    OUT GT_U8               *queuePtr,
    OUT GT_STATUS           *statusPtr
)
{
    PRV_CPSS_DRV_EVENT_HNDL_STC     *evHndlPtr;   /* The event handle pointer */
    PRV_CPSS_TX_BUF_QUEUE_FIFO_STC  *fifoPtr;     /* The Handle Tx-End FIFO   */
    GT_U32                          intKey;       /* The interrupt key        */
    PRV_CPSS_SW_TX_FREE_DATA_STC    *txEndDataPtr;/* The Tx shadow data pointer*/

    CPSS_NULL_PTR_CHECK_MAC(devPtr);
    CPSS_NULL_PTR_CHECK_MAC(cookiePtr);
    CPSS_NULL_PTR_CHECK_MAC(queuePtr);
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    evHndlPtr = (PRV_CPSS_DRV_EVENT_HNDL_STC*)hndl;

    /* validate event handle data */
    if (NULL == evHndlPtr || NULL == evHndlPtr->extDataPtr)
    {
        return GT_FAIL;
    }

    fifoPtr = (PRV_CPSS_TX_BUF_QUEUE_FIFO_STC*)evHndlPtr->extDataPtr;

    if (NULL == fifoPtr->headPtr)
    {
        /* FIFO is empty */
        return GT_NO_MORE;
    }

    /* lock section from ISR preemption */
    intKey = 0;
    cpssOsTaskLock();
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E , (GT_32*)&intKey);

    txEndDataPtr = fifoPtr->headPtr;
    fifoPtr->headPtr  = txEndDataPtr->nextPtr;

    if (NULL == fifoPtr->headPtr)
    {
        /* last element in FIFO */
        fifoPtr->tailPtr = NULL;
    }

    /* extract data from FIFO element */
    *queuePtr   = txEndDataPtr->queueNum;
    *devPtr     = txEndDataPtr->devNum;
    *cookiePtr  = txEndDataPtr->userData;
    *statusPtr  = GT_OK;

    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E , (GT_32*)&intKey);
    cpssOsTaskUnLock();

    if(!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(*devPtr))
    {       
        PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(*devPtr);
    
        /*check that the DB of networkIf library was initialized*/
        PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(*devPtr);
    
        if(PRV_CPSS_PP_MAC(*devPtr)->useIsr == GT_FALSE)
        {
            /* this check can be done only after we have the 'device number' ,
               and now we can check that :
               the CPSS not handling the ISR for the device so function must not
               be called */
            return GT_BAD_STATE;
        }
    
        /* return the FIFO element buffer to pool */
        if (cpssBmPoolBufFree(PRV_CPSS_PP_MAC(*devPtr)->intCtrl.txDescList[*queuePtr].poolId,
                          txEndDataPtr) != GT_OK)
        {
            return GT_FAIL;
        }
    }
    else
    {
        /* return the packet header buffer to pool */
        if (prvCpssGenNetIfMiiPoolFreeBuf(PRV_CPSS_GEN_NETIF_MII_TX_POOL_E,
                                            txEndDataPtr) != GT_OK)
        {
            return GT_FAIL;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet
*
* DESCRIPTION:
*       This routine frees all transmitted packets descriptors. In addition, all
*       user relevant data in Tx End FIFO.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum     - The device number the packet was transmitted from
*       txQueue    - The queue the packet was transmitted upon (0..7)
*
* OUTPUTS:
*       cookiePtr   - (pointer to) the cookie attached to packet that was send
*                     from this queue
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - on bad device or queue
*       GT_BAD_PTR - on NULL pointer
*       GT_NO_MORE - no more packet cookies to get
*       GT_ERROR - the Tx descriptor is corrupted
*       GT_NOT_INITIALIZED - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*  1. returns the caller the cookie attached to the frame that was sent from the
*     device on the specific queue.
*  2. put the descriptors of this sent packet back to the 'Free descriptors' list
*       --> SDMA relate
*  3. this function should be used only when the ISR of the CPSS is not in use,
*     meaning that the intVecNum (given in function cpssDxChHwPpPhase1Init) was
*    set to CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS.
*
*   NOTE: the caller is responsible to synchronize the calls to 'Tx send' and
*   this function , since both deal with the 'Tx descriptors'.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxBufferQueueDescFreeAndCookieGet
(
    IN GT_U8                devNum,
    IN GT_U8                txQueue,
    OUT GT_PTR             *cookiePtr
)
{
    PRV_CPSS_TX_DESC_LIST_STC       *txDescListPtr;
    /* Counts the number of descriptors already freed. */
    GT_U32                          freeDescNum;
    PRV_CPSS_SW_TX_DESC_STC         *descPtr;
    /* Points to the first sw desc. to be sent to the callback function.*/
    PRV_CPSS_SW_TX_DESC_STC         *firstTxDescPtr;
    /* Number of descriptors this packet occupies to be sent to the callback
       function.*/
    GT_U32                          descNum;
    PRV_CPSS_TX_DESC_STC            tmpTxDesc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(cookiePtr);
    PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(devNum);

    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);


    if(!PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
    {
        PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);
    
        if(txQueue >= 8)
        {
            return GT_BAD_PARAM;
        }
    
        /* the Tx descriptors for this Queue */
        txDescListPtr  = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);
    
        descPtr = txDescListPtr->next2Free;
    
        tmpTxDesc.word1 = descPtr->txDesc->word1;
    
        /* No more descriptors to release.  */
        if(TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) == TX_DESC_DMA_OWN)
        {
            return GT_NO_MORE;
        }
    
        if(TX_DESC_GET_FIRST_BIT_MAC(devNum,&tmpTxDesc) == 0)
        {
            /*should not happen*/
            return (GT_STATUS)GT_ERROR;
        }
    
        descNum     = 1;
        firstTxDescPtr = descPtr;
    
        /* Get the packet's descriptors.        */
        while(TX_DESC_GET_LAST_BIT_MAC(devNum,&tmpTxDesc) == 0x0)
        {
            if(TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) == TX_DESC_DMA_OWN)
            {
                /*should not happen*/
                return GT_NO_MORE;
            }
            descPtr = descPtr->swNextDesc;
            tmpTxDesc.word1 = descPtr->txDesc->word1;
            descNum++;
        }
    
        if(TX_DESC_GET_OWN_BIT_MAC(devNum,&tmpTxDesc) == TX_DESC_DMA_OWN)
        {
            /*should not happen*/
            return GT_NO_MORE;
        }
    
        freeDescNum = descNum;
    
        /* we have a cookie to return to caller */
        *cookiePtr = firstTxDescPtr->userData;
    
        while(freeDescNum > 0)
        {
            firstTxDescPtr->txDesc->word1 = 0x0;
    
            firstTxDescPtr = firstTxDescPtr->swNextDesc;
            freeDescNum--;
        }
    
        txDescListPtr->freeDescNum += descNum;
        txDescListPtr->next2Free = firstTxDescPtr;
    }
    else
    {
        /* MII doesn't use descriptors */
        *cookiePtr = 0;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet
*
* DESCRIPTION:
*       This function return the number of free Tx descriptors for given
*       device and txQueue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond.
*
* INPUTS:
*       devNum  - Device number.
*       txQueue - Tx queue number.
*
* OUTPUTS:
*       numberOfFreeTxDescriptorsPtr - pointer to number of free
*                                         descriptors for the given queue.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - on bad device or queue
*       GT_BAD_PTR - on NULL pointer
*       GT_NOT_INITIALIZED - the library was not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaTxFreeDescripotrsNumberGet
(
    IN GT_U8                devNum,
    IN GT_U8                txQueue,
    OUT GT_U32             *numberOfFreeTxDescriptorsPtr
)
{
    PRV_CPSS_TX_DESC_LIST_STC       *txDescListPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(numberOfFreeTxDescriptorsPtr);
    PRV_CPSS_DXCH_SDMA_USED_CHECK_MAC(devNum);

    /* Diamond Cut have not SDMA registers */

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_DIAMOND_E);


    PRV_CPSS_DXCH_NETIF_INITIALIZED_CHECK_MAC(devNum);

    if(txQueue >= 8)
    {
        return GT_BAD_PARAM;
    }

    /* the Tx descriptors for this Queue */
    txDescListPtr  = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.txDescList[txQueue]);

    *numberOfFreeTxDescriptorsPtr = txDescListPtr->freeDescNum;

    return GT_OK;
}

