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
*cpssDxChPacketRxHandling.c
*
*MODIFY:
*	by wujh@autelan.com   on 11/14/2007 revision <0.0.1..>
*       this file includs the functions will perform in appDemoEvHndler task.
*CREATOR:
*	wujh@autelan.com
*
*DESCRIPTION:
*<some description about this file>
      In this file The functions prepare for packet Rx from Adapter to The Virtual Network Interface  .
*
*DATE:
*	11/14/2007	
*	
*******************************************************************************/
#ifdef __cplusplus
extern "C" 
{
#endif
/*include header files begin */
/*kernel part */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/un.h>
#include <sys/select.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>

/*include header files end */
extern void npd_route_nexthop_show_table
(
	void
);
extern void npd_arp_snooping_show_table
(
	void
);
extern unsigned int npd_route_nexthop_iteminfo_get
(
	unsigned char devNum,
	unsigned int tblIndex
);

/* add for cpssDxChPacketRxGetEthIndexViaDSA() zhangdi 2011-05-25*/
extern unsigned int (*npd_get_global_index_by_devport)
(
	unsigned int 		devNum,
	unsigned int 		portNum,
	unsigned int		*eth_g_index
);
/* add end */

void cpssDxChPacketRxPrintPacketBuffer(unsigned char *buffer,unsigned long buffLen)
{
	unsigned int i;
	unsigned char lineBuffer[64] = {0}, *bufPtr = NULL;
	unsigned int curLen = 0;

	if(!buffer)
		return;
	
	osPrintfPktRx("................................................\n");
	bufPtr = lineBuffer;
	curLen = 0;
	for(i = 0;i < buffLen ; i++)
	{
		curLen += sprintf(bufPtr,"%02x ",buffer[i]);
		bufPtr = lineBuffer + curLen;
		
		if(0==(i+1)%16) {
			osPrintfPktRx("%s\n",lineBuffer);
			memset(lineBuffer,0,sizeof(lineBuffer));
			curLen = 0;
			bufPtr = lineBuffer;
		}
	}
	
	if((buffLen%16)!=0)
	{
		osPrintfPktRx("%s\n",lineBuffer);
	}
	
	osPrintfPktRx("................................................\n");
}

void cpssDxChPacketTxPrintPacketBuffer(unsigned char *buffer,unsigned long buffLen)
{
	unsigned int i;
	unsigned char lineBuffer[64] = {0}, *bufPtr = NULL;
	unsigned int curLen = 0;

	if(!buffer)
		return;
	
	osPrintfPktTx("------------------------------------------------\n");
	bufPtr = lineBuffer;
	curLen = 0;
	
	for(i = 0;i < buffLen ; i++)
	{
		curLen += sprintf(bufPtr,"%02x ",buffer[i]);
		bufPtr = lineBuffer + curLen;
		
		if(0==(i+1)%16) {
			osPrintfPktTx("%s\n",lineBuffer);
			memset(lineBuffer,0,sizeof(lineBuffer));
			curLen = 0;
			bufPtr = lineBuffer;
		}
	}
	
	if((buffLen%16)!=0)
	{
		osPrintfPktTx("%s\n",lineBuffer);
	}
	
	osPrintfPktTx("------------------------------------------------\n");
}

/*******************************************************************************
 * cpssDxChPacketRxGetEthIndexViaDSA
 *
 * DESCRIPTION:
 *		 This function parse DSA tag and get global ethernet port index
 *
 * INPUTS:
 *		 rxParams - Rx packet params.
 *
 * OUTPUTS:
 *		type	 - interface type
 *		 intfId  -  interface ID such as ethernet port global index or trunk id or vid or vidx
 *
 * RETURNS:
 *		GT_OK	-	if no error occurred
 *		GT_FAIL	-	if error occurred
 *
 * COMMENTS:
 *		 This function is call after packet received.
 *
 *******************************************************************************/
GT_STATUS cpssDxChPacketRxGetEthIndexViaDSA
(
     IN   unsigned char					device,
     IN   CPSS_DXCH_NET_RX_PARAMS_STC   *rxParams,
     OUT  CPSS_INTERFACE_TYPE_ENT 		*type,
     OUT  unsigned int					*intfId
)
{
	CPSS_DXCH_NET_DSA_CMD_ENT 			dsaType;
	CPSS_DXCH_NET_DSA_COMMON_STC 		*commonParams;
	CPSS_DXCH_NET_DSA_TO_CPU_STC   		*toCpu;
	CPSS_DXCH_NET_DSA_FROM_CPU_STC   	*fromCpu;
	CPSS_DXCH_NET_DSA_TO_ANALYZER_STC	*toAnalyzer;
	CPSS_DXCH_NET_DSA_FORWARD_STC    	*forward;
	CPSS_INTERFACE_TYPE_ENT		intfType;
	GT_U8			devNum;
	GT_U16 			portOrTrunkId = 0xFFFF;
	GT_U16			vid = 0, vidx = 0;
	unsigned int 	eth_g_index = 0;
	
	dsaType 			= rxParams->dsaParam.dsaType;
	commonParams 		= &(rxParams->dsaParam.commonParams);
	intfType 			= CPSS_INTERFACE_PORT_E;

	vid = commonParams->vid;

	switch (dsaType) 
	{
		case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
		{
			toCpu = &(rxParams->dsaParam.dsaInfo.toCpu);
			if(toCpu->srcIsTrunk != GT_TRUE)
			{
				portOrTrunkId	= toCpu->interface.portNum;
			}
			else 
			{
				portOrTrunkId = toCpu->interface.srcTrunkId;
				intfType 			= CPSS_INTERFACE_TRUNK_E;					
			}
			devNum = toCpu->devNum;
			
			break;
		}
		case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
		{
			toAnalyzer 		= &(rxParams->dsaParam.dsaInfo.toAnalyzer);
			portOrTrunkId	= toAnalyzer->devPort.portNum;
			
			break;
		}
		case CPSS_DXCH_NET_DSA_CMD_FORWARD_E :
		{
			forward = &(rxParams->dsaParam.dsaInfo.forward);
			if(forward->srcIsTrunk != GT_TRUE)
			{
				portOrTrunkId 	= forward->source.portNum;
			}
			else
			{
				portOrTrunkId 	= forward->source.trunkId;
				intfType 		= CPSS_INTERFACE_TRUNK_E;		
			}
			devNum = forward->srcDev;
			
			break;
		}
		case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E :
		{
			fromCpu 		= &(rxParams->dsaParam.dsaInfo.fromCpu);
			intfType 		= fromCpu->dstInterface.type;
			portOrTrunkId 	= fromCpu->dstInterface.devPort.portNum;
			devNum 			= fromCpu->dstInterface.devPort.devNum;

			break;
		}
		default:
			break;
	} /* end switch(..) */

	#if 0
	cpssDxChVirtPort2SlotPort(devNum,portOrTrunkId,&slot,&portOnSlot);
	return (intfType << 29)|(devNum << 24) | portOrTrunkId;
	#else
	*type = intfType;
	if(CPSS_INTERFACE_PORT_E == intfType) { 	/* port */
		/* change devNum( 71-2x is 4) in packet to device (0). zhangdi@autelan.com 2012-05-17 */
		/*if(0 != npd_get_global_index_by_devport(devNum,portOrTrunkId,&eth_g_index))*/
		if(0 != npd_get_global_index_by_devport(device,portOrTrunkId,&eth_g_index))
			return GT_FAIL;
		else	
			*intfId = eth_g_index;
	}
	else if(CPSS_INTERFACE_TRUNK_E == intfType) { /* trunk */
		*intfId = portOrTrunkId;
	}
	else if(CPSS_INTERFACE_VIDX_E == intfType) {  /* VIDX */
		*intfId = vidx;
	}
	else if(CPSS_INTERFACE_VID_E == intfType) {  /* VID */
		*intfId = vid;
	}
	return GT_OK;
	#endif
}

GT_STATUS show_arp()
{
	npd_arp_snooping_show_table();
	return 0;
}
GT_STATUS show_at()
{
	npd_route_nexthop_show_table();
	return 0;
}

static char showHwAtHelpStr[] = {
	"Usage:show_hw_at <device> <table index>\n"
	"<device>\tDevice number:0 for 98Dx275,1 for 98Dx804\n"
	"<table index>\tHardware table index range [0-4096)\n"
};

extern GT_U32 appDemoPpConfigDevAmount;

GT_STATUS show_hw_at
(
	unsigned char devNum,
	unsigned int tblIndex
)
{
	GT_STATUS ret = GT_OK;
	if((devNum >= appDemoPpConfigDevAmount) || 
		(tblIndex >= 4096)) {
		osPrintf("%s",showHwAtHelpStr);
		return GT_FAIL;
	}
	ret = npd_route_nexthop_iteminfo_get(devNum,tblIndex);

	return ret;
}

GT_STATUS show_rtdrv_entry(unsigned int DIP,unsigned int masklen)
{
	GT_STATUS ret = GT_OK;
	unsigned int Dip=DIP,Masklen=masklen;
	int index = 0;
	CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT defUcNextHop;

	cpssOsMemSet(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
	osPrintf("------ --------------- ----- ----------\n");
	osPrintf("%-6s %-15s %-5s %-10s\n","ITEM","Dest IP","MASK","NEXTHOP");
	if(!cpssDxChIpLpmIpv4UcPrefixSearch(0, 0, DIP, masklen,&defUcNextHop))
		osPrintf("%-6d %-3d.%-3d.%-3d.%-3d %-5d %#0x\n",index,(Dip>>24)&0xFF,(Dip>>16)&0xFF,	\
						(Dip>>8)&0xFF,Dip & 0xFF,masklen,defUcNextHop.ipLttEntry.routeEntryBaseIndex);
	osPrintf("------ --------------- ----- ----------\n");
	osPrintf("there is total %d entry\n",index);

	return ret;
}

GT_STATUS show_rtdrv()
{
	GT_STATUS ret = GT_OK;
	unsigned int Dip=0,masklen=0;
	int index = 0;
	CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT defUcNextHop;

	osPrintf("------ --------------- ----- ----------\n");
	osPrintf("%-6s %-15s %-5s %-10s\n","ITEM","Dest IP","MASK","NEXTHOP");
	while(GT_OK == cpssDxChIpLpmIpv4UcPrefixGetNext(0,0,&Dip,&masklen,&defUcNextHop))
	{
		index ++;
		osPrintf("%-6d %-3d.%-3d.%-3d.%-3d %-5d %#0x\n",index,(Dip>>24)&0xFF,(Dip>>16)&0xFF,	\
						(Dip>>8)&0xFF,Dip & 0xFF,masklen,defUcNextHop.ipLttEntry.routeEntryBaseIndex);
	}
	osPrintf("------ --------------- ----- ----------\n");
	osPrintf("there is totle %d entry\n",index);

	return ret;
}

GT_STATUS npd_get_port_pvid
(
	unsigned char devNum,
	unsigned char portNum
)
{
		unsigned short vlanId = 0;
		GT_STATUS ret = GT_OK;

		ret = cpssDxChBrgVlanPortVidGet(devNum,portNum,&vlanId);
		osPrintf("dev %d port %d pvid %d\n",devNum,portNum,vlanId);

		return ret;
}
#ifdef __cplusplus
}
#endif
