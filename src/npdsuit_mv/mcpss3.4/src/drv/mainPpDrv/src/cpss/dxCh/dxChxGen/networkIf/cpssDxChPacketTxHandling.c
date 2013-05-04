/*******************************************************************************
Copyright (C) Autelan Technology

This software file is owned and distributed by Autelan Technology 
********************************************************************************

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************
*cpssDxChPacketTxHandling.c
*
*MODIFY:
*	by wujh@autelan.com   on 11/14/2007 revision <0.0.1..>
*       this file includs the functions will perform in appDemoEvHndler task.
*CREATOR:
*	wujh@autelan.com
*
*DESCRIPTION:
*<some description about this file>
*      In this file The functions prepare for packet Rx from Adapter to 
*      The Virtual Network Interface  .
*
*DATE:
*	11/14/2007	
*	
*******************************************************************************/
#ifdef __cplusplus
extern "C" 
{
#endif

/*kernel part*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <linux/list.h>
#include <sys/types.h>
#include <sys/socket.h>

/*#include <netinet/in.h>*/
/*#include <arpa/inet.h>*/


#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>


/*user part */
#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/os/gtOs/cpssOsMem.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChPacketHandling.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/interrupts/private/prvCpssGenIntDefs.h>

extern int osPrintfDbg
(
	const char* format, 
	...
);
#if 1
extern unsigned int (*npd_get_global_index_by_devport)
(
	unsigned int 		devNum,
	unsigned int 		portNum,
	unsigned int		*eth_g_index
);
extern unsigned int (*npd_get_devport_by_global_index)
(
	unsigned int 		eth_g_index,
	unsigned char 		*devNum,
	unsigned char 		*portNum
);
#else
extern unsigned int npd_get_global_index_by_devport
(
	unsigned int 		devNum,
	unsigned int 		portNum,
	unsigned int		*eth_g_index
);
extern unsigned int npd_get_devport_by_global_index
(
	unsigned int 		eth_g_index,
	unsigned char 		*devNum,
	unsigned char 		*portNum
);
#endif
extern  int npd_vlan_check_contain_port
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  *isTagged
);
#if 0
extern GT_BOOL	cpssDxChPacketTypeIsARP
(       
	IN GT_U8  *packetBuff
);

extern GT_BOOL	cpssDxChPacketTypeIsMc
(       
	IN GT_U8  *packetBuff
);

extern GT_BOOL	cpssDxChPacketTypeIsDhcp
(       
	IN GT_U8  *packetBuff
);

extern 	int                 adptVirRxFd;
extern 	int                 adptRstpRxFd;
extern 	int 								adptRstpRxClientFd;
extern 	GT_BOOL							rstpSocketCreated;
extern	struct sockaddr_un	clieAddr;
extern	socklen_t           clieAddrLen;


#ifndef __IGMP_SNP_SOCK_STREAM__
extern 	int                 adptIgmpSnpRxFd;
extern 	int 				adptIgmpSnpRxCltFd;
extern 	GT_BOOL				igmpSnpSockCreated;
extern	struct sockaddr_un	igmpSnpClieAddr;
extern	socklen_t           igmpSnpClieAddrLen;
#endif
#endif

extern CPSS_OS_CACHE_DMA_MALLOC_FUNC  cpssOsCacheDmaMalloc;
extern CPSS_OS_CACHE_DMA_FREE_FUNC    cpssOsCacheDmaFree;


GT_BOOL	cpssDxChVirtNetIfCheckPortTagged
(
		IN GT_U8 devNum,
		IN GT_U16 vlanId,
		IN GT_U32	portNum
)
{
		return GT_FALSE;
}

unsigned int cpssDxChVirtNetIfMaskId
(
	IN CPSS_INTERFACE_TYPE_ENT intfType,
	IN GT_U8	devNum,
	IN GT_U16 portOrTrunkId
)
{
		unsigned int interfaceId = 0;
		GT_BOOL		isIllegal = GT_FALSE;

		/* check interface type */
		if((CPSS_INTERFACE_PORT_E > intfType) ||
				(CPSS_INTERFACE_VID_E	< intfType))
				isIllegal = GT_TRUE;

		/* check device exists or not */
		PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
		
		/* check if port or trunk id is illegal */
		switch(intfType)
		{
				case CPSS_INTERFACE_PORT_E:
					if(portOrTrunkId > 27)
						isIllegal = GT_TRUE;
					break;
				case CPSS_INTERFACE_TRUNK_E:
					if(portOrTrunkId > 255)
						isIllegal = GT_TRUE;
					break;
				case CPSS_INTERFACE_VID_E:
					if((portOrTrunkId > 4094)||(portOrTrunkId < 1))
						isIllegal = GT_TRUE;
					break;
				case CPSS_INTERFACE_VIDX_E:
					if(portOrTrunkId > 255)
						isIllegal = GT_TRUE;
					break;
				default:
					break;
		}

		if(isIllegal == GT_TRUE)
		{
				return 0xFFFF;
		}

		interfaceId = ((intfType << 29) | (devNum << 24) | (portOrTrunkId)); 

		return interfaceId;
}

void cpssDxChCreateNewTagPacket
(
	IN 		unsigned char* insertStr,
	IN 		unsigned char* srcStr,
	INOUT unsigned char* destStr,
	IN			unsigned int		srcLen,
	IN 		GT_BOOL 		 enable
)
{
	unsigned char* tmp = NULL;
	
	if(!insertStr || !srcStr || !destStr)
		return ;

	if(GT_FALSE == enable)
		memmove(destStr,srcStr,srcLen);
	else {
		memmove(destStr,srcStr,12);
		tmp = destStr + 12;
		memcpy(tmp,insertStr,4);
	}
	
	return ;
}

GT_STATUS cpssDxChAddVlanHeader
(
	IN unsigned char devNum,
	IN unsigned char portNum,
	IN unsigned short vid,
	IN unsigned char* packetBuf,
	IN unsigned int 	  packetLen	
)
{
	unsigned char isTag = GT_FALSE;
	unsigned int eth_g_index = 0;
	unsigned char* src = NULL ,*dest = NULL;
	unsigned char vlanHeader[4] = {0x81,00,00,00};
	int rc = 0;

	dest = packetBuf - 4; /*when malloc , reserve 4 bytes*/
	src = packetBuf;
	
	rc = npd_get_global_index_by_devport(devNum,portNum,&eth_g_index);
	if(0 != rc)
		return GT_FAIL;
	else if(0 != (rc = npd_vlan_check_contain_port(vid,eth_g_index,&isTag))) {
		return GT_FALSE;
	}
	else if(GT_FALSE == isTag) {
		cpssDxChCreateNewTagPacket(vlanHeader,src,dest,packetLen,GT_FALSE);
		return GT_OK;
	}
	else if(GT_TRUE == isTag) {
		((unsigned short*)vlanHeader)[1] = vid;
		cpssDxChCreateNewTagPacket(vlanHeader,src,dest,packetLen,GT_TRUE);
		return GT_OK;
	}

	return GT_OK;
}

GT_STATUS cpssDxChNetIfTxPacketQueueSet
(
	enum CPSS_PACKET_TYPE_ENT packetType,
	unsigned char *queueIdx
)
{
	unsigned char queue = 0;

	switch (packetType) {
		/* queue 7 */
		case CPSS_PACKET_TYPE_BPDU_E:
			queue = CPU_TX_BPDU_QUEUE;
			break;
		/* queue 6 */
		case CPSS_PACKET_TYPE_IP_TCP_TELNET_E:
			queue = CPU_TX_TELNET_QUEUE;
			break;
		case CPSS_PACKET_TYPE_IP_TCP_SSH_E:
			queue = CPU_TX_SSH_QUEUE;
			break;
		/* queue 5 */
		/* queue 4 */
		/* queue 3 */
		/* queue 2 */
		case CPSS_PACKET_TYPE_IP_IGMP_E:
		case CPSS_PACKET_TYPE_IP6_MLD_E:
			queue = CPU_TX_IGMP_QUEUE;
			break;
		/* queue 1 */
		case CPSS_PACKET_TYPE_ARP_E:
			queue = CPU_TX_ARP_BC_QUEUE;
			break;
		/* queue 0 */
		case CPSS_PACKET_TYPE_GVRP_E:
			queue = CPU_TX_DEFAULT_QUEUE;
			break;
		case CPSS_PACKET_TYPE_IPv4_E:
			queue = CPU_TX_IPv4_QUEUE;
			break;
		case CPSS_PACKET_TYPE_IP_ICMP_E:
			queue = CPU_TX_ICMP_QUEUE;
			break;
		case CPSS_PACKET_TYPE_IP_TCP_E:
			queue = CPU_TX_TCP_QUEUE;
			break;
		case CPSS_PACKET_TYPE_IP_UDP_E:
			queue = CPU_TX_UDP_QUEUE;
			break;
		case CPSS_PACKET_TYPE_DLDP_E:
			queue =CPU_TX_DEFAULT_QUEUE;
			break;
		case CPSS_PACKET_TYPE_OTHER:
			queue = CPU_TX_DEFAULT_QUEUE;
			break;
		default :
			queue = CPU_TX_DEFAULT_QUEUE;
			break;

	}

	
	*queueIdx = queue;
	
	return GT_OK;
}

GT_VOID cpssDxChVirtNetIfTxBuildParam
(
	
	IN unsigned int interfaceId,
	IN unsigned short vid,
	IN GT_BOOL	isTagged,
	IN unsigned char queueIdx,
	IN CPSS_INTERFACE_TYPE_ENT vifType,
	IN unsigned int sendBytes,
	INOUT CPSS_DXCH_NET_TX_PARAMS_STC* txParams
)
{
	CPSS_DXCH_NET_SDMA_TX_PARAMS_STC    *sdmaInfo;
	CPSS_DXCH_NET_DSA_PARAMS_STC        *dsaParam;
	CPSS_DXCH_NET_DSA_TO_CPU_STC       	*toCpu;
	CPSS_DXCH_NET_DSA_FROM_CPU_STC     	*fromCpu;
	CPSS_DXCH_NET_DSA_TO_ANALYZER_STC  	*toAnalyzer;
	CPSS_DXCH_NET_DSA_FORWARD_STC      	*forward;
	GT_U8	ret;
	unsigned char	devNum = 0,portNum = 0;
	unsigned char	hwDev = 0;
	unsigned int	portOrTrunkId = 0;
	char buf[1024] = {0},*bufptr = NULL;
	int bufLen = 0;
	bufptr = buf;

	sdmaInfo = &(txParams->sdmaInfo);
	dsaParam = &(txParams->dsaParam);

	/* parse interfaceId */
	bufLen += sprintf(bufptr,"packet tx build parameter:type(%s) ",	\
				(vifType == CPSS_INTERFACE_PORT_E) ? "PORT" : "VID");
	bufptr = buf + bufLen;
	
	#if 0 /* changed by qinhuasong on 08/03/03 - here interface ID must be eth global index */
	vifType 			= (interfaceId >> 29) & 0x7;
	devNum 				= (interfaceId >> 24) & 0x1F;
	portOrTrunkId = (interfaceId & 0xFFFFFF);
	#else
	if(vifType == CPSS_INTERFACE_PORT_E)	{
		ret = npd_get_devport_by_global_index(interfaceId,&devNum,&portNum);
		if(ret != 0)
		{
			bufLen += sprintf(bufptr,"get device port error %d",ret);
			bufptr = buf + bufLen;
		}
		else
		{
			bufLen += sprintf(bufptr,"port(%d,%d) %s in vlan %d\n",devNum,portNum,isTagged ? "TAG" : "UNTAG",vid);
			bufptr = buf + bufLen;
			portOrTrunkId = (unsigned int)portNum;
		}	
		
	}
	else if(CPSS_INTERFACE_VID_E == vifType) {
		/*when looked up arp entry ,virRePktType must change  CPSS_INTERFACE_VID_E to CPSS_INTERFACE_PORT_E.
		*	then packets were only sended by unicast. by zhubo@autelan.com 2008.7.17 
		*/
		vifType = CPSS_INTERFACE_PORT_E;
		devNum = interfaceId & 0x000000ff;
		portOrTrunkId =  interfaceId >> 8;
		bufLen += sprintf(bufptr,"get port(%d,%d) in vlan %d istag %s, changed CPSS_INTERFACE_PORT_E\n",devNum,portOrTrunkId,vid,isTagged ? "TAG" : "UNTAG");
		bufptr = buf + bufLen;
	}
	else if(CPSS_INTERFACE_TRUNK_E == vifType) {
		devNum = interfaceId & 0x000000ff;
		portOrTrunkId = interfaceId >> 8;
		bufLen += sprintf(bufptr,"get trunk(%d,%d) in vlan %d\n",devNum,portOrTrunkId,vid);
		bufptr = buf + bufLen;
	}

	osPrintfDbg("%s",buf);
	bufptr = buf;
	bufLen = 0;
	#endif
	/* build general Tx params */
	txParams->cookie 			= NULL;
	txParams->packetIsTagged 	= isTagged ? GT_TRUE : GT_FALSE;/* by zhubo@autelan.com 2008.7.24 cpssDxChVirtNetIfCheckPortTagged(0,2,0);*/

	/* build SDMA info */
	sdmaInfo->evReqHndl 		= 0;
	sdmaInfo->recalcCrc 		= GT_TRUE;
	sdmaInfo->txQueue 					= queueIdx,
	sdmaInfo->invokeTxBufferQueueEvent 	= GT_FALSE,

	/* build DSA params */
	dsaParam->dsaType 					= CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E,   /* From_CPU */

	dsaParam->commonParams.dsaTagType 	= CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;/*extended DSA Tag*/
	dsaParam->commonParams.vpt 			= 1;  
	dsaParam->commonParams.cfiBit 		= 1;
	/*modify*/
	dsaParam->commonParams.vid 			= vid;
	/* Get HW device number from reg 0x58  zhangdi 2011-09-09 */
    ret = cpssDxChCfgHwDevNumGet(devNum, &hwDev);
    if (GT_OK != ret)
    {
		osPrintfDbg("Get hwDev error! \n");
        return ret;
    }

	devNum = hwDev;/* let DSA tag 's dev num  equal to hwDev  */
	osPrintfDbg("**************************************\n");
	osPrintfDbg("************ Get hwDev = %d **********\n",hwDev);
	osPrintfDbg("**************************************\n");
	switch(dsaParam->dsaType)
	{
		case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
			toCpu = &(dsaParam->dsaInfo.toCpu);

			toCpu->isEgressPipe 			= GT_FALSE;
			toCpu->isTagged 				= isTagged;
			toCpu->devNum					= devNum;
			toCpu->srcIsTrunk				= (vifType == CPSS_INTERFACE_TRUNK_E) ? GT_TRUE : GT_FALSE;
			toCpu->cpuCode					= 0;
			toCpu->wasTruncated   			= GT_FALSE;
			toCpu->originByteCount			= sendBytes;
			if(toCpu->srcIsTrunk)
				toCpu->interface.srcTrunkId = portOrTrunkId;
			else
				toCpu->interface.portNum	= portOrTrunkId;

			break;
		case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
			fromCpu = &(dsaParam->dsaInfo.fromCpu);

			fromCpu->dstInterface.type = vifType;
			fromCpu->extDestInfo.devPort.dstIsTagged = isTagged ? GT_TRUE : GT_FALSE;
			switch(vifType)
			{
				
				case CPSS_INTERFACE_PORT_E:
					fromCpu->dstInterface.devPort.devNum 	= devNum;
					fromCpu->dstInterface.devPort.portNum	= portOrTrunkId;
					break;
				case CPSS_INTERFACE_VID_E:
					fromCpu->dstInterface.vlanId			= vid;
					fromCpu->dstInterface.type 				= CPSS_INTERFACE_VID_E;
					break;
				case CPSS_INTERFACE_VIDX_E:
					fromCpu->dstInterface.vidx				= vid;
					break;
				case CPSS_INTERFACE_TRUNK_E:
					fromCpu->dstInterface.trunkId 			= portOrTrunkId;
					break;
				default:
					break;
			}
			break;
		case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
			toAnalyzer = &(dsaParam->dsaInfo.toAnalyzer);

			toAnalyzer->devPort.devNum 	= devNum;
			toAnalyzer->devPort.portNum	= portOrTrunkId;
			toAnalyzer->isTagged		= isTagged;
			toAnalyzer->rxSniffer		= GT_TRUE;
			break;
		case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
			forward = &(dsaParam->dsaInfo.forward);
			
			forward->srcIsTagged			= isTagged ? GT_TRUE : GT_FALSE;
			forward->srcDev					= devNum;
			forward->srcIsTrunk				= (vifType == CPSS_INTERFACE_TRUNK_E) ? GT_TRUE : GT_FALSE;
			if(forward->srcIsTrunk)
				forward->source.trunkId 	= portOrTrunkId;
			else
				forward->source.portNum		= portOrTrunkId;
			forward->srcId					= 0;
			forward->egrFilterRegistered	= GT_FALSE;
			forward->wasRouted				= GT_TRUE;
			forward->qosProfileIndex		= 255;
			forward->dstInterface.type 		= vifType;
			switch(vifType)
			{
				
				case CPSS_INTERFACE_PORT_E:
					forward->dstInterface.devPort.devNum 	= devNum;
					forward->dstInterface.devPort.portNum 	= portOrTrunkId;
					break;
				case CPSS_INTERFACE_VID_E:
					forward->dstInterface.vlanId			= vid;
					forward->dstInterface.type 				= CPSS_INTERFACE_VID_E;
					break;
				case CPSS_INTERFACE_VIDX_E:
					forward->dstInterface.vidx				= vid;
					break;
				case CPSS_INTERFACE_TRUNK_E:
					forward->dstInterface.trunkId 			= portOrTrunkId;
					break;
				default:
					break;
			}
			break;
			default:
				break;
	}

	return;
}

#ifdef __cplusplus
}
#endif
