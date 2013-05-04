/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* appDemoDxChEventHandle.c
*
* DESCRIPTION:
*       this library contains the implementation of the event handling functions
*       for the DXCH device  , and the redirection to the GalTis Agent for
*       extra actions .
*
********************************************************************************
*   ---> this file is compiled only under CHX_FAMILY <---
********************************************************************************
*
* FILE REVISION NUMBER:
*       $Revision: 1.2 $
******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfMii.h>
#include <appDemo/userExit/userEventHandler.h>

#ifdef __AX_PCI_TEST__
unsigned long pci_test_pkt_count = 0;
#endif
#ifndef __AX_FLATFROM__
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChPacketHandling.h>/*for test_icmp--data struct*/
extern void	 *userMutexId;/* signal new packet received */
extern void  *varLockId;/* global variable lock */
extern GT_STATUS cpssDxChNetIfSdmaSyncTxPacketSend
(
    IN GT_U8                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN GT_U8                                    *buffList[],
    IN GT_U32                                   buffLenList[],
    IN GT_U32                                   numOfBufs
);
#endif

/****************************************************************************
* Debug                                                                     *
****************************************************************************/

#define BUFF_LEN    20

#ifdef IMPL_GALTIS
extern GT_STATUS cmdDxChRxPktReceive
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    IN GT_U32                               numOfBuff,
    IN GT_U8                               *packetBuffs[],
    IN GT_U32                               buffLen[],
    IN CPSS_DXCH_NET_RX_PARAMS_STC         *rxParamsPtr
);
#endif /* IMPL_GALTIS */



/*******************************************************************************
* RX_PACKET_RECEIVE_CB_FUN
*
* DESCRIPTION:
*       Function called to handle incoming Rx packet in the CPU
*
* APPLICABLE DEVICES: ALL Devices
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       numOfBuffPtr - Num of used buffs in packetBuffs.
*       packetBuffs  - The received packet buffers list.
*       buffLen      - List of buffer lengths for packetBuffs.
*       rxParamsPtr  - (Pointer to)information parameters of received packets
*
* RETURNS:
*       GT_OK                    - no error
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*RX_PACKET_RECEIVE_CB_FUN)
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx,
    IN GT_U32                               numOfBuff,
    IN GT_U8                               *packetBuffs[],
    IN GT_U32                               buffLen[],
    IN void                                 *rxParamsPtr
);

#define MAX_REGISTER    5
static GT_U32                   numRegistrations=0;
static RX_PACKET_RECEIVE_CB_FUN rxPktReceiveCbArray[MAX_REGISTER];

/*******************************************************************************
* appDemoDxChNetRxPacketCbRegister
*
* DESCRIPTION:
*       register a CB function to be called on every RX packet to CPU
*
*       NOTE: function is implemented in the 'AppDemo' (application)
*
* INPUTS:
*       rxPktReceiveCbFun - CB function to be called on every RX packet to CPU
*
* OUTPUTS:
*       None
*
* RETURNS:
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoDxChNetRxPacketCbRegister
(
    IN  RX_PACKET_RECEIVE_CB_FUN  rxPktReceiveCbFun
)
{
    if(numRegistrations >= MAX_REGISTER)
    {
        return GT_FULL;
    }

    rxPktReceiveCbArray[numRegistrations] = rxPktReceiveCbFun;

    numRegistrations++;

    return GT_OK;
}


/*******************************************************************************
* appDemoDxChNetRxPktHandle
*
* DESCRIPTION:
*       application routine to receive frames . -- DXCH function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxChNetRxPktHandle
(
    IN GT_U8                                devNum,
    IN GT_U8                                queueIdx
)
{
    GT_STATUS                           rc;
    GT_U32                              numOfBuff = BUFF_LEN;
    GT_U8*                              packetBuffs[BUFF_LEN];
    GT_U32                              buffLenArr[BUFF_LEN];
    CPSS_DXCH_NET_RX_PARAMS_STC         rxParams;
    GT_U8	i;
	PKT_INFO_PARAM adptpktInfo;
	memset(&rxParams, 0, sizeof(CPSS_DXCH_NET_RX_PARAMS_STC));
	memset(&adptpktInfo,0,sizeof(PKT_INFO_PARAM));

    /* get the packet from the device */
    if(appDemoPpConfigList[devNum].cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
    {
        rc = cpssDxChNetIfSdmaRxPacketGet(devNum, queueIdx,&numOfBuff,
                            packetBuffs,buffLenArr,&rxParams);
    }
    else if(appDemoPpConfigList[devNum].cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E)
    {
        rc = cpssDxChNetIfMiiRxPacketGet(devNum,queueIdx,&numOfBuff,packetBuffs,buffLenArr,&rxParams);
    }
    else
    {
        rc = GT_FAIL;
    }

    if (rc != GT_OK)  /* GT_NO_MORE is good and not an error !!! */
    {
        /* no need to free buffers because not got any */
        return rc;
    }
#ifdef __AX_PCI_TEST__
        pci_test_pkt_count++;
#endif

#ifndef __AX_PCI_TEST__
#ifndef __AX_PLATFORM__
        if(0 == numOfBuff)/* free descriptor number zero or not enough */
        {
            osPrintf("appDemoDxChNetRxPktHandle::number of buffer is 0\n");
            return GT_OK;
        }
	#ifdef __APP_ICMP_REPLY__
        rc = cpssDxChRxIcmpEchoProcess(devNum,queueIdx,numOfBuff,packetBuffs,buffLenArr,&rxParams);
        if(GT_OK == rc)
        {
            osPrintf("appDemoDxChNetRxPktHandle::cpssDxChRxIcmpEchoProcess devNum %d ok\n",devNum);
            /* no return, we should free packet buffers at the end */
        }
        else if(GT_ICMP_ERR != rc)
        {
            osPrintf("appDemoDxChNetRxPktHandle::cpssDxChRxIcmpEchoProcess dev %d rc %d\n",devNum,rc);
            /* no return, we should free packet buffers at the end */
        }
        else /* GT_ICMP_ERR:packet is not icmp type */
        {
	#endif
    
      /*
        * by zhubo@autelan.com 2008.08.06
        * former when send packets to kernel, the buff has been write new data.
        *   now invoke cpssDxChPacketRxAdapterStartReceive directly in order to
        *   avoid packets error.
        *   del signal
        */
        /*osSemWait(varLockId,OS_WAIT_FOREVER);*/
    
        /*adptdevInfo.devNum = devNum;
        adptdevInfo.queueIdx = queueIdx;*/
    
        osMemCpy(&(adptpktInfo.rxRxParamPtr),&(rxParams),sizeof(CPSS_DXCH_NET_RX_PARAMS_STC));
        adptpktInfo.buffsNum = numOfBuff;
        for(i=0;i<numOfBuff;i++)
        {
            adptpktInfo.buffPtr[i]      = packetBuffs[i];
            adptpktInfo.buffLength[i] = buffLenArr[i];			
        }

#if 0   /* add for debug */
		int j=0;
        for(j=0;j<numOfBuff;j++)
        {
    		printf("\n**********2222*****adptpktInfo.buffPtr[i]:0x%x\n",adptpktInfo.buffPtr[j]);
            printf("\n**********2222*****adptpktInfo.buffLength[i]:%d\n",adptpktInfo.buffLength[j]);
        }
		/* for debug */
        printf("\n**********2222*****adptpktInfo.buffsNum:0x%x\n",adptpktInfo.buffsNum);
    	printf("::::::::::::::::::::::::PACKET::::::::::::::::::::::::\n");

    	for(j;j<(adptpktInfo.buffLength[0]);j++)
    	{
    		printf("%02x ",adptpktInfo.buffPtr[0][j]);
    		if((j+1)%16==0)
    			printf("\n");
    	}
    	printf("\n::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
#endif
        nam_packet_rx_handling(devNum,queueIdx,&adptpktInfo);
	#ifdef __APP_ICMP_REPLY__
        }
	#endif
#endif     
#endif


    /* call all the functions that register the hook */
    {
        GT_U32  ii;
        for(ii = 0 ; ii < numRegistrations ; ii++)
        {
            (rxPktReceiveCbArray[ii])(devNum,queueIdx,numOfBuff,
                                    packetBuffs,buffLenArr,&rxParams);
        }
    }

    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
    /* ignore the return code from this function because we want to keep on
       getting RX buffers , regardless to the CMD manager */
    (void) cmdDxChRxPktReceive(devNum,queueIdx,numOfBuff,
                                    packetBuffs,buffLenArr,&rxParams);
#endif /* IMPL_GALTIS */

    if(appDemoPpConfigList[devNum].cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
    {
        /* now you need to free the buffers */
        rc = cpssDxChNetIfRxBufFree(devNum,queueIdx,packetBuffs,numOfBuff);
    }
    else /* PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E */
    {
        rc = cpssDxChNetIfMiiRxBufFree(devNum,queueIdx,packetBuffs,numOfBuff);
    }

    return rc;
}

/*******************************************************************************
* appDemoDxChNetRxPktGet
*
* DESCRIPTION:
*       application routine to get the packets from the queue and 
*       put it in msgQ. -- DXCH function
*
* INPUTS:
*       devNum      - Device number.
*       queueIdx    - The queue from which this packet was received.
*       msgQId      - Message queue Id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxChNetRxPktGet
(
    IN GT_U8            devNum,
    IN GT_U8            queueIdx,
    IN CPSS_OS_MSGQ_ID  msgQId
)
{
    GT_STATUS                 rc;
    APP_DEMO_RX_PACKET_PARAMS rxParams;
    rxParams.numOfBuff = BUFF_LEN;
    cpssOsMutexLock(rxMutex);
    /* get the packet from the device */
    if(appDemoPpConfigList[devNum].cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
    {
        rc = cpssDxChNetIfSdmaRxPacketGet(devNum, queueIdx,&(rxParams.numOfBuff),
                            rxParams.packetBuffs,rxParams.buffLenArr,
                                          &(rxParams.dxChNetRxParams));
    }
    else if(appDemoPpConfigList[devNum].cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E)
    {
        rc = cpssDxChNetIfMiiRxPacketGet(devNum, queueIdx,&(rxParams.numOfBuff),
                            rxParams.packetBuffs,rxParams.buffLenArr,
                                          &(rxParams.dxChNetRxParams));
    }
    else
    {
        rc = GT_FAIL;
    }
    cpssOsMutexUnlock(rxMutex);

    if (rc != GT_OK)
    {
        /* no need to free buffers because not got any */
        return rc;
    }

    rxParams.devNum = devNum;
    rxParams.queue = queueIdx;
    /* put in msgQ */
    rc = cpssOsMsgQSend(msgQId,&rxParams,
                        sizeof(APP_DEMO_RX_PACKET_PARAMS),CPSS_OS_MSGQ_WAIT_FOREVER);
    return rc;
}

/*******************************************************************************
* appDemoDxChNetRxPktTreat
*
* DESCRIPTION:
*       application routine to treat the packets . -- DXCH function
*
* INPUTS:
*       rxParamsPtr - (pointer to) rx paacket params
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxChNetRxPktTreat
(
    IN APP_DEMO_RX_PACKET_PARAMS  *rxParamsPtr
)
{
    GT_STATUS   rc;
    GT_U8       devNum ;
    if (rxParamsPtr == NULL)
    {
        /* debug */
        return GT_BAD_PTR;
    }

    devNum = rxParamsPtr->devNum;

    /* call all the functions that register the hook */
    {
        GT_U32  ii;
        for(ii = 0 ; ii < numRegistrations ; ii++)
        {
            (rxPktReceiveCbArray[ii])(rxParamsPtr->devNum,rxParamsPtr->queue,
                                      rxParamsPtr->numOfBuff,rxParamsPtr->packetBuffs,
                                      rxParamsPtr->buffLenArr,&(rxParamsPtr->dxChNetRxParams));
        }
    }

    /* call GalTis Agent for counting and capturing */
#ifdef IMPL_GALTIS
    /* ignore the return code from this function because we want to keep on
       getting RX buffers , regardless to the CMD manager */
    (void) cmdDxChRxPktReceive(rxParamsPtr->devNum,rxParamsPtr->queue,rxParamsPtr->numOfBuff,
                               rxParamsPtr->packetBuffs,rxParamsPtr->buffLenArr,
                               &(rxParamsPtr->dxChNetRxParams));
#endif /* IMPL_GALTIS */
    cpssOsMutexLock(rxMutex);
    if(appDemoPpConfigList[devNum].cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
    {
        /* now you need to free the buffers */
        rc = cpssDxChNetIfRxBufFree(rxParamsPtr->devNum,rxParamsPtr->queue
                                    ,rxParamsPtr->packetBuffs,rxParamsPtr->numOfBuff);
    }
    else if (appDemoPpConfigList[devNum].cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E)
    {
        rc = cpssDxChNetIfMiiRxBufFree(rxParamsPtr->devNum,rxParamsPtr->queue
                                    ,rxParamsPtr->packetBuffs,rxParamsPtr->numOfBuff);
    }
    else
    {
        rc = GT_FAIL;
    }
    cpssOsMutexUnlock(rxMutex);

    return rc;
}
