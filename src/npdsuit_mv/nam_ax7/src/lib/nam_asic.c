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
* nam_asic.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM for ASIC driver.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.35 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "nam_utilus.h"
#include "nam_asic.h"
#include "nam_log.h"
#ifdef DRV_LIB_BCM
#include "bcm/stat.h"
#endif

#ifdef DRV_LIB_CPSS
extern unsigned long appDemoPpConfigDevAmount;/*Number of PP's in the system*/
extern unsigned long systemInitialized;/*marvell PP has completed initialization or not: 1-true,0-false */
extern unsigned int asicType[128];
#endif

#ifdef DRV_LIB_BCM
extern int soc_ndev;
extern unsigned long systemInitialized;/*BCM asic has completed initialization or not: 1-true,0-false */
#endif

int nam_asic_get_asic_type(int devNum)
{
	int asictype = 0;
	/*code optimize: Out-of-bounds read
	zhangcl@autelan.com 2013-1-21*/
	if((devNum < 0) || (devNum > 127))
		return -1;
#ifdef DRV_LIB_CPSS
	asictype = asicType[devNum];
#endif
	return asictype;
}
int nam_asic_set_cfc
(
	CPU_FC_PACKET_TYPE packettype,
	unsigned int time,
	unsigned int rate
)
{
	int ret = 0,i = 0;
	unsigned char devNum = 0;

	CPU_FC_PACKET_TYPE pTypeArr[CPU_FC_TYPE_MAX_E + 1] = {CPU_FC_TYPE_MAX_E};
	int pTypeNumber = 0;
#ifdef DRV_LIB_CPSS
	switch(packettype) {
		default:
			pTypeArr[pTypeNumber++] = packettype;
			break;
		case CPU_FC_TYPE_ALL_E:
			for(i = CPU_FC_TYPE_ARP_E; i < CPU_FC_TYPE_ALL_E; i++) {
				pTypeArr[pTypeNumber++] = i;
			}
			break;
		case CPU_FC_TYPE_OAM_E:
			pTypeArr[pTypeNumber++] = CPU_FC_TYPE_TELNET_E;
			pTypeArr[pTypeNumber++] = CPU_FC_TYPE_SSH_E;
			pTypeArr[pTypeNumber++] = CPU_FC_TYPE_HTTP_E;
			pTypeArr[pTypeNumber++] = CPU_FC_TYPE_HTTPS_E;
			pTypeArr[pTypeNumber++] = CPU_FC_TYPE_SNMP_E;
			break;
		case CPU_FC_TYPE_WIRELESS_E:
			pTypeArr[pTypeNumber++] = CPU_FC_TYPE_CAPWAP_CTRL_E;
			pTypeArr[pTypeNumber++] = CPU_FC_TYPE_CAPWAP_DATA_E;
			break;
	}

	for(i = 0; i < pTypeNumber; i++) {
		ret = cpssDxChNetIfCpufcRateLimiterSet(devNum,pTypeArr[i],time,rate);
		if(0 != ret) {
			syslog_ax_nam_asic_dbg("batch set cpu flow control packet type %d time %d rate %d error %d\r\n", \
						pTypeArr[i], time, rate, ret);
		}
	}
#endif
	return ret;
}

/**********************************************************************************
 * nam_asic_get_instance_num
 * 
 * get asic instance number
 *
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		number of asic instance
 *		
 **********************************************************************************/
unsigned long nam_asic_get_instance_num
(
	void
)
{
#ifdef DRV_LIB_CPSS
	return appDemoPpConfigDevAmount;
#endif
#ifdef DRV_LIB_BCM
	return soc_ndev;
#endif
}

/**********************************************************************************
 * nam_asic_get_cpu_interface_type
 * 
 * get asic CPU interface type :using SDMA or using Ethernet MAC
 *
 *
 *	INPUT:
 *		devNum - device number
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - using SDMA (PCI interface)
 *		1 - using Ethernet MAC (SMI interface)
 *		
 **********************************************************************************/
unsigned long nam_asic_get_cpu_interface_type
(
	unsigned char devNum
)
{
	/* TODO: currently use PCI interface, maybe changed to SMI interface*/
	return 0;
}

/**********************************************************************************
 * nam_asic_set_cpufc_queue_quota
 * 
 * DESCRIPTION:
 *	This method set CPU port queue quota.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *		quota - quota assigned to the queue
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
unsigned long nam_asic_set_cpufc_queue_quota
(
	unsigned int queue,
	unsigned int quota
)
{
	int devNum = 0, devCnt = 0;
	unsigned long retVal = 0;
	
	devCnt = nam_asic_get_instance_num();
	
#ifdef DRV_LIB_CPSS
	for(devNum = 0; devNum < devCnt; devNum++) {
		retVal = cpssDxChSdmaRxQueueQuotaSet(devNum,queue,quota);
		if(retVal) {
			syslog_ax_nam_asic_err("set dev %d queue %d cpufc quota %d error %d\n",	\
					devNum, queue,quota, retVal);
		}
		
		syslog_ax_nam_asic_dbg("set dev %d queue %d cpufc quota %d \n", \
				devNum, queue,quota);
	}
#endif	
	return retVal;
}

/**********************************************************************************
 * nam_asic_get_cpufc_queue_quota
 * 
 * DESCRIPTION:
 *	This method get CPU port queue quota.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *	
 *	OUTPUT:
 *		quota - quota assigned to the queue
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
unsigned long nam_asic_get_cpufc_queue_quota
(
	unsigned int queue,
	unsigned int *quota
)
{
	int devNum = 0;
	unsigned long retVal = 0;
	unsigned int value;
#ifdef DRV_LIB_CPSS

	/* here we assume only get asic device 0 */
	retVal = cpssDxChSdmaRxQueueQuotaGet(devNum,queue,&value);
	if(retVal) {
		syslog_ax_nam_asic_err("get dev %d queue %d cpufc quota error %d\n",	\
				devNum, queue,retVal);
		value = 0;
	}
	syslog_ax_nam_asic_err("get dev %d queue %d cpufc quota %d\n",	\
			devNum, queue,value);

	*quota = value;
#endif
	return retVal;
}

/**********************************************************************************
 * nam_asic_set_cpufc_queue_shaper
 * 
 * DESCRIPTION:
 *	This method set CPU port queue shaper value.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *		shaper - shaper value assigned to the queue (unit is PPS)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
unsigned long nam_asic_set_cpufc_queue_shaper
(
	unsigned int queue,
	unsigned int shaper
)
{
	int devNum = 0, devCnt = 0;
	unsigned long retVal = 0;
	unsigned long tbMtu = 0;
	unsigned int bpsBase = 0;
	unsigned long value = 0;
#ifdef DRV_LIB_CPSS

	/* convert shaper value from PPS to bps*/
	retVal = cpssDrvPpHwRegFieldGet(0,0x01800004,3,2,&tbMtu);
	if(retVal) {
		syslog_ax_nam_asic_err("get token bucket MTU error %d when set cpu queue %d shaper\n", retVal,queue);
	}
	else {
		syslog_ax_nam_asic_err("get token bucket MTU %d when set cpu queue %d shaper\n", tbMtu,queue);
		/* 2 for 10KB, 1 for 2KB, 0 for 1.5KB*/
		bpsBase = (0x2 == tbMtu) ? (10 * 1000 * 8) : 	\
				  (0x1 == tbMtu) ? (2 * 1000 * 8) : (1.5 * 1000 * 8);
	}

	
	devCnt = nam_asic_get_instance_num();

	for(devNum = 0; devNum < devCnt; devNum++) {
		value = (bpsBase * shaper)/1000;
		retVal = cpssDxChPortTxQShaperEnableSet(devNum,CPSS_CPU_PORT_NUM_CNS,queue,1);
		if(retVal) {
			syslog_ax_nam_asic_err("set dev %d CPU port queue %d shaper %d enable error %d\n",	\
					devNum, queue,shaper, retVal);
		}
		
		syslog_ax_nam_asic_dbg("set dev %d CPU port queue %d shaper<%d,%d>\n",	\
					devNum, queue,shaper, value);
		retVal = cpssDxChPortTxQShaperProfileSet(devNum,CPSS_CPU_PORT_NUM_CNS,queue,0x1, &value);
		if(retVal) {
			syslog_ax_nam_asic_err("set dev %d CPU port queue %d shaper<%d,%d> error %d\n",	\
					devNum, queue,shaper, value, retVal);
		}
		else {
			syslog_ax_nam_asic_dbg("set dev %d CPU port queue %d shaper %d real %d\n",	\
					devNum, queue,shaper, value);
		}
	}
#endif	
	return retVal;
}

/**********************************************************************************
 * nam_asic_set_cpufc_port_shaper
 * 
 * DESCRIPTION:
 *	This method set CPU port shaper value.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *		shaper - shaper value assigned to the queue (unit is PPS)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
unsigned long nam_asic_set_cpufc_port_shaper
(
	unsigned int shaper
)
{
	int devNum = 0, devCnt = 0;
	unsigned long retVal = 0;
	unsigned long tbMtu = 0;
	unsigned int bpsBase = 0;
	unsigned long value = 0;
#ifdef DRV_LIB_CPSS

	/* convert shaper value from PPS to bps*/
	retVal = cpssDrvPpHwRegFieldGet(0,0x01800004,3,2,&tbMtu);
	if(retVal) {
		syslog_ax_nam_asic_err("get token bucket MTU error %d when set cpu shaper\n", retVal);
	}
	else {
		syslog_ax_nam_asic_err("get token bucket MTU %d when set cpu shaper\n", tbMtu);
		/* 2 for 10KB, 1 for 2KB, 0 for 1.5KB*/
		bpsBase = (0x2 == tbMtu) ? (10 * 1000 * 8) : 	\
				  (0x1 == tbMtu) ? (2 * 1000 * 8) : (1.5 * 1000 * 8);
	}

	
	devCnt = nam_asic_get_instance_num();

	for(devNum = 0; devNum < devCnt; devNum++) {
		value = (bpsBase * shaper)/1000;
		retVal = cpssDxChPortTxShaperEnableSet(devNum,CPSS_CPU_PORT_NUM_CNS,1);
		if(retVal) {
			syslog_ax_nam_asic_err("set dev %d CPU port shaper enable error %d\n",	\
					devNum, shaper, retVal);
		}
		
		syslog_ax_nam_asic_dbg("set dev %d CPU port shaper<%d,%d>\n",devNum, shaper, value);
		retVal = cpssDxChPortTxShaperProfileSet(devNum,CPSS_CPU_PORT_NUM_CNS,0x1, &value);
		if(retVal) {
			syslog_ax_nam_asic_err("set dev %d CPU port shaper<%d,%d> error %d\n",	\
					devNum, shaper, value, retVal);
		}
		else {
			syslog_ax_nam_asic_dbg("set dev %d CPU port shaper %d real %d\n",	\
					devNum, shaper, value);
		}
	}
#endif	
	return retVal;
}

/**********************************************************************************
 * nam_asic_show_cpu_sdma_info
 * 
 * DESCRIPTION:
 *	This method show CPU sdma channel descriptor info
 *
 *
 *	INPUT:
 *		devNum - asic device number
 *		queue - queue number
 *		direction - cpu SDMA channel direction: Rx or Tx or Both
 *		showSize - show buffer size
 *	
 *	OUTPUT:
 *		outStr - output string buffer
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
unsigned long nam_asic_show_cpu_sdma_info
(
	unsigned char devNum,
	unsigned char queue,
	unsigned char direction,
	unsigned char **outStr,
	unsigned int showSize
)
{
#ifdef DRV_LIB_CPSS

	cpssDxChNetIfSdmaPrint(devNum,queue,direction,outStr,showSize);
#endif
	return 0;
}

/**********************************************************************************
 * nam_asic_get_cpu_sdma_mib
 * 
 * DESCRIPTION:
 *	This method get CPU sdma channel Rx packets/octets and resource error counter
 *
 *
 *	INPUT:
 *		devNum - asic device number
 *	
 *	OUTPUT:
 *		stats - cpu port dma channel mib info
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		other - if error found.
 *		
 **********************************************************************************/
unsigned long nam_asic_get_cpu_sdma_mib
(
	unsigned char devNum,
	CPU_PORT_DMA_CHANNEL_MIB_S **stats
)
{
	unsigned long result = NAM_OK;
#ifdef DRV_LIB_CPSS	
	result = cpssDxChNetIfSdmaRxMibGet(devNum,stats);
#endif
#ifdef DRV_LIB_BCM	
	int ret = NAM_OK;
	int unit = 0;
	bcm_port_t port;
	bcm_stat_val_t type;
	unsigned int value = 0;

	unit = devNum;
	port = 0;	/* CPU_PORT */

	/* Rx packet count */
	type = snmpIfInUcastPkts;
	ret = bcm_stat_get32(unit, port, type, &value);
	if (ret != NAM_OK) {
		syslog_ax_nam_asic_err("get device %d CPU port %d sdma %s counter error, ret %d\n",
				unit, port, "snmpIfInUcastPkts", ret);
		return ret;
	}
	(*stats)->rxInPkts = value;

	/* Rx byte count */
	value = 0;
	type = snmpIfInOctets;
	ret = bcm_stat_get32(unit, port, type, &value);
	if (ret != NAM_OK) {
		syslog_ax_nam_asic_err("get device %d CPU port %d sdma %s counter error, ret %d\n",
				unit, port, "snmpIfInOctets", ret);
		return ret;
	}
	(*stats)->rxInOctets = value;

	/* Rx resource error count */
	value = 0;
	type = snmpIfInErrors;
	(*stats)->rxErrCnt = value;

	/* clear counter */
	ret = bcm_stat_clear(unit, port);
	if (ret != NAM_OK) {
		syslog_ax_nam_asic_err("clear device %d CPU port %d sdma counter error, ret %d\n",
				unit, port, ret);
		return ret;
	}

	result = ret;
#endif

	return result;
}

/**********************************************************************************
 * nam_asic_get_cpu_mac_mib
 * 
 * DESCRIPTION:
 *	This method get CPU port Ethernet MAC mib counter
 *
 *
 *	INPUT:
 *		devNum - asic device number
 *	
 *	OUTPUT:
 *		stats - cpu port ethernet mac mib info
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		other - if error found.
 *		
 **********************************************************************************/
unsigned long nam_asic_get_cpu_mac_mib
(
	unsigned char devNum,
	CPU_PORT_MAC_MIB_S *stats
)
{
	unsigned long result = NAM_OK;
#ifdef DRV_LIB_CPSS

	CPSS_PORT_MAC_COUNTER_SET_STC cpuMacMib;
	memset(&cpuMacMib, 0,sizeof(CPSS_PORT_MAC_COUNTER_SET_STC));
	result = cpssDxChPortMacCountersOnPortGet(devNum,CPSS_CPU_PORT_NUM_CNS,&cpuMacMib);
	if(NAM_OK != result) {
		syslog_ax_nam_asic_err("get device %d cpu mac mib counter error %d\n",devNum, result);
	}
	else {
		/* Rx*/
		stats->rx.goodPkts[0] 	= cpuMacMib.goodPktsRcv.l[0];
		stats->rx.goodPkts[1] 	= cpuMacMib.goodOctetsRcv.l[1];
		stats->rx.badPkts[0]	= cpuMacMib.badPktsRcv.l[0];
		stats->rx.badPkts[1]	= cpuMacMib.badPktsRcv.l[1];
		stats->rx.goodOctets[0]	= cpuMacMib.goodOctetsRcv.l[0];
		stats->rx.goodOctets[1]	= cpuMacMib.goodOctetsRcv.l[1];
		stats->rx.badOctets[0]	= cpuMacMib.badOctetsRcv.l[0];
		stats->rx.badOctets[1]	= cpuMacMib.badOctetsRcv.l[1];
		stats->rx.internalDrop[0] = cpuMacMib.dropEvents.l[0];
		stats->rx.internalDrop[1] = cpuMacMib.dropEvents.l[1];
		
		/* Tx*/
		stats->tx.goodPkts[0]	= cpuMacMib.goodPktsSent.l[0];
		stats->tx.goodPkts[1]	= cpuMacMib.goodPktsSent.l[1];
		stats->tx.goodOctets[0]	= cpuMacMib.goodOctetsSent.l[0];
		stats->tx.goodOctets[1]	= cpuMacMib.goodOctetsSent.l[1];
		stats->tx.macErrPkts[0]	= cpuMacMib.macTransmitErr.l[0];
		stats->tx.macErrPkts[1]	= cpuMacMib.macTransmitErr.l[1];
	}
#endif	
	return result;
}


/**********************************************************************************
 * nam_asic_get_port_mac_mib
 * 
 * DESCRIPTION:
 *	This method get port Ethernet MAC mib counter
 *
 *
 *	INPUT:
 *		devNum - asic device number
 *		portNum - asic port number
 *	
 *	OUTPUT:
 *		stats - cpu port ethernet mac mib info
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		other - if error found.
 *		
 **********************************************************************************/
unsigned long nam_asic_get_port_mac_mib
(
	unsigned char devNum,
	unsigned char portNum,
	ASIC_PORT_MAC_MIB_DIAG_S *stats
)
{
	unsigned long result = NAM_OK;
#ifdef DRV_LIB_CPSS

	CPSS_PORT_MAC_COUNTER_SET_STC cpuMacMib;
	memset(&cpuMacMib, 0,sizeof(CPSS_PORT_MAC_COUNTER_SET_STC));
	result = cpssDxChPortMacCountersOnPortGet(devNum,portNum,&cpuMacMib);
	if(NAM_OK != result)
	{
		syslog_ax_nam_asic_err("get device %d cpu mac mib counter error %d\n",devNum, result);
	}
	else
	{
		stats->goodOctetsRcv[0] = cpuMacMib.goodOctetsRcv.l[0];
		stats->goodOctetsRcv[1] = cpuMacMib.goodOctetsRcv.l[1];
		
		stats->badOctetsRcv[0] = cpuMacMib.badOctetsRcv.l[0];
		stats->badOctetsRcv[1] = cpuMacMib.badOctetsRcv.l[1];
		
		stats->macTransmitErr[0] = cpuMacMib.macTransmitErr.l[0];
		stats->macTransmitErr[1] = cpuMacMib.macTransmitErr.l[1];
		
		stats->goodPktsRcv[0] = cpuMacMib.goodPktsRcv.l[0];
		stats->goodPktsRcv[1] = cpuMacMib.goodPktsRcv.l[1];
		
		stats->badPktsRcv[0] = cpuMacMib.badPktsRcv.l[0];
		stats->badPktsRcv[1] = cpuMacMib.badPktsRcv.l[1];
		
		stats->brdcPktsRcv[0] = cpuMacMib.brdcPktsRcv.l[0];
		stats->brdcPktsRcv[1] = cpuMacMib.brdcPktsRcv.l[1];
		
		stats->mcPktsRcv[0] = cpuMacMib.mcPktsRcv.l[0];
		stats->mcPktsRcv[1] = cpuMacMib.mcPktsRcv.l[1];
		
		stats->pkts64Octets[0] = cpuMacMib.pkts64Octets.l[0];
		stats->pkts64Octets[1] = cpuMacMib.pkts64Octets.l[1];
		
		stats->pkts65to127Octets[0] = cpuMacMib.pkts65to127Octets.l[0];
		stats->pkts65to127Octets[1] = cpuMacMib.pkts65to127Octets.l[1];
		
		stats->pkts128to255Octets[0] = cpuMacMib.pkts128to255Octets.l[0];
		stats->pkts128to255Octets[1] = cpuMacMib.pkts128to255Octets.l[1];

		stats->pkts256to511Octets[0] = cpuMacMib.pkts256to511Octets.l[0];
		stats->pkts256to511Octets[1] = cpuMacMib.pkts256to511Octets.l[1];
		
		stats->pkts512to1023Octets[0] = cpuMacMib.pkts512to1023Octets.l[0];
		stats->pkts512to1023Octets[1] = cpuMacMib.pkts512to1023Octets.l[1];
		
		stats->pkts1024tomaxOoctets[0] = cpuMacMib.pkts1024tomaxOoctets.l[0];
		stats->pkts1024tomaxOoctets[1] = cpuMacMib.pkts1024tomaxOoctets.l[1];
		
		stats->goodOctetsSent[0] = cpuMacMib.goodOctetsSent.l[0];
		stats->goodOctetsSent[1] = cpuMacMib.goodOctetsSent.l[1];
		
		stats->goodPktsSent[0] = cpuMacMib.goodPktsSent.l[0];
		stats->goodPktsSent[1] = cpuMacMib.goodPktsSent.l[1];
		
		stats->excessiveCollisions[0] = cpuMacMib.excessiveCollisions.l[0];
		stats->excessiveCollisions[1] = cpuMacMib.excessiveCollisions.l[1];
		
		stats->mcPktsSent[0] = cpuMacMib.mcPktsSent.l[0];
		stats->mcPktsSent[1] = cpuMacMib.mcPktsSent.l[1];
		
		stats->brdcPktsSent[0] = cpuMacMib.brdcPktsSent.l[0];
		stats->brdcPktsSent[1] = cpuMacMib.brdcPktsSent.l[1];
		
		stats->unrecogMacCntrRcv[0] = cpuMacMib.unrecogMacCntrRcv.l[0];
		stats->unrecogMacCntrRcv[1] = cpuMacMib.unrecogMacCntrRcv.l[1];
		
		stats->fcSent[0] = cpuMacMib.fcSent.l[0];
		stats->fcSent[1] = cpuMacMib.fcSent.l[1];	

		stats->goodFcRcv[0] = cpuMacMib.goodFcRcv.l[0];
		stats->goodFcRcv[1] = cpuMacMib.goodFcRcv.l[1];
		
		stats->dropEvents[0] = cpuMacMib.dropEvents.l[0];
		stats->dropEvents[1] = cpuMacMib.dropEvents.l[1];
		
		stats->undersizePkts[0] = cpuMacMib.undersizePkts.l[0];
		stats->undersizePkts[1] = cpuMacMib.undersizePkts.l[1];
		
		stats->fragmentsPkts[0] = cpuMacMib.fragmentsPkts.l[0];
		stats->fragmentsPkts[1] = cpuMacMib.fragmentsPkts.l[1];
		
		stats->oversizePkts[0] = cpuMacMib.oversizePkts.l[0];
		stats->oversizePkts[1] = cpuMacMib.oversizePkts.l[1];
		
		stats->jabberPkts[0] = cpuMacMib.jabberPkts.l[0];
		stats->jabberPkts[1] = cpuMacMib.jabberPkts.l[1];
		
		stats->macRcvError[0] = cpuMacMib.macRcvError.l[0];
		stats->macRcvError[1] = cpuMacMib.macRcvError.l[1];	

		stats->badCrc[0] = cpuMacMib.badCrc.l[0];
		stats->badCrc[1] = cpuMacMib.badCrc.l[1];
		
		stats->collisions[0] = cpuMacMib.collisions.l[0];
		stats->collisions[1] = cpuMacMib.collisions.l[1];
		
		stats->lateCollisions[0] = cpuMacMib.lateCollisions.l[0];
		stats->lateCollisions[1] = cpuMacMib.lateCollisions.l[1];
		
		stats->badFcRcv[0] = cpuMacMib.badFcRcv.l[0];
		stats->badFcRcv[1] = cpuMacMib.badFcRcv.l[1];
		
		stats->ucPktsRcv[0] = cpuMacMib.ucPktsRcv.l[0];
		stats->ucPktsRcv[1] = cpuMacMib.ucPktsRcv.l[1];
		
		stats->ucPktsSent[0] = cpuMacMib.ucPktsSent.l[0];
		stats->ucPktsSent[1] = cpuMacMib.ucPktsSent.l[1];
		
		stats->multiplePktsSent[0] = cpuMacMib.multiplePktsSent.l[0];
		stats->multiplePktsSent[1] = cpuMacMib.multiplePktsSent.l[1];	

		stats->deferredPktsSent[0] = cpuMacMib.deferredPktsSent.l[0];
		stats->deferredPktsSent[1] = cpuMacMib.deferredPktsSent.l[1];			
	}
#endif	
	return result;
}


/**********************************************************************************
 * nam_oam_traffic_setup
 * 
 * DESCRIPTION:
 *	This method set OAM traffic configuration.	 
 *
 *
 *	INPUT:
 *		oamType - OAM traffic type
 *		oamInfo - OAM traffic controller
 *	
 *	OUTPUT:
 *		oamInfo - OAM traffic controller
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
int nam_oam_traffic_setup
(
	enum OAM_TRAFFIC_TYPE_ENT oamType,
	struct oam_ctrl_s *oamInfo
)
{
	unsigned long rc = NAM_OK;
	unsigned long hwTblIndex = ~0UL;
	unsigned char devNum = 0, ppCount = 0;
#ifdef DRV_LIB_CPSS
	ppCount = nam_asic_get_instance_num();
	
	for(; devNum < ppCount; devNum++) {
		rc = cpssDxChNetIfOamTrafficSet(devNum,oamType,oamInfo->dport,&hwTblIndex);
		if(NAM_OK != rc) {
			syslog_ax_nam_asic_err("setup oam traffic type %d port %d error %d\n", \
							oamType,oamInfo->dport,rc);
			return -rc;
		}
	}
	oamInfo->hwTblIdx = hwTblIndex;
#endif
	return rc;
}

/**********************************************************************************
 * nam_asic_legal_device_number
 * 
 * check out device number is legal or not
 *
 *
 *	INPUT:
 *		devNum - device number
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE - if device number is legal
 *		NPD_FALSE - if device number is illegal
 *		
 **********************************************************************************/
unsigned int nam_asic_legal_device_number
(
	unsigned int devNum
)
{
#ifdef DRV_LIB_CPSS

	return (devNum <appDemoPpConfigDevAmount) ? NPD_TRUE : NPD_FALSE;
#endif
}

unsigned int nam_board_after_enable()
{
#ifdef DRV_LIB_CPSS
  return appDemoBoardAfterSysUpIntEnable();
#endif

}


/**********************************************************************************
 * nam_asic_init
 * 
 * asic initialization
 *
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred.
 *		
 **********************************************************************************/
int nam_asic_init(enum product_id_e product_id) {

	if(PRODUCT_ID_AX5K_E == product_id)
	{
		/*code optimize: Missing return statement
		zhangcl@autelan.com 2013-1-21*/
		syslog_ax_nam_asic_dbg("PRODUCT_ID_AX5K_E not support ASIC init...\n");
		return NPD_FALSE;
	}
	
	syslog_ax_nam_asic_dbg("Initilizing ASIC...\n");
#ifdef DRV_LIB_BCM
	nam_thread_create("AsicInit",(void *)bcmUserAppMain,(void *)product_id,NPD_TRUE,NPD_FALSE);
#endif
#ifdef DRV_LIB_CPSS
   nam_thread_create("AsicInit",(void *)cpssUserAppMain,(void *)product_id,NPD_TRUE,NPD_FALSE);
#endif
	return NPD_SUCCESS;	
}

/**********************************************************************************
 * nam_asic_init_completion_check
 * 
 * check out asic initialization process completed or not
 * if initialization process not completed,wait for it.
 *
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred.
 *		
 **********************************************************************************/
int nam_asic_init_completion_check(void) {
	/* tell my thread id*/
	npd_init_tell_whoami((unsigned char *)"BoardInitCheck",0);

#ifdef DRV_LIB_BCM
	while(systemInitialized!=1){
				syslog_ax_nam_asic_dbg("!)@(#*$&%^\n");/*wait until marvell pp initialization done*/
		sleep(1);
	}
#endif
#ifdef DRV_LIB_CPSS
	while(systemInitialized!=CPSS_INITIALIZED){
		syslog_ax_nam_asic_dbg("!)@(#*$&%^\n");/*wait until marvell pp initialization done*/
		sleep(1);
	}
#endif
	return NPD_SUCCESS;
}

#ifdef DRV_LIB_BCM
unsigned int appDemoBoardAfterSysUpIntEnable
(
	void
)
{
	return 0;
}
#endif
#ifdef DRV_LIB_BCM
unsigned int appDemoBoardSigHandler
(
	int signo
)
{
	return 0;
}
#endif
int nam_asic_log_level_set
(
	int level
)
{
	#ifdef DRV_LIB_CPSS
	return cpss_debug_level_set(level);
	#endif
	#ifdef DRV_LIB_BCM
	return bcm_debug_level_set(level);
	#endif
}

int nam_asic_log_level_clr
(
	int level
)
{
	#ifdef DRV_LIB_CPSS
	return cpss_debug_level_clr(level);
	#endif
	#ifdef DRV_LIB_BCM
	return bcm_debug_level_clr(level);
	#endif
}

/**********************************************************************
 * nam_asic_redistribute_devnum
 *
 *  DESCRIPTION:
 *          Redistribute devnum for asic.
 *          0x00000058[8:4]  OwnDevice
 *
 *  INPUT:
 *          original_devnum
 *          new_devnum
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          NAM_OK
 *          NAM_ERR
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
unsigned long nam_asic_redistribute_devnum(unsigned char original_devnum, unsigned char new_devnum)
{
	/*code optimize: Unsigned compared against 0
	zhangcl@autelan.com 2013-1-21*/
    if ( (original_devnum>31)||(new_devnum>31) )
        return NAM_ERR;

    #ifdef DRV_LIB_CPSS
    return prvCpssDrvHwPpSetRegField(original_devnum,
                                     0x58, 4, 5, new_devnum);
    #endif
}


/**********************************************************************
 * nam_asic_set_device_map_table
 *
 *  DESCRIPTION:
 *          Set Device Map Table Entry Register.
 *          0x01A40004 + n*0x10  [7:1]
 *
 *  INPUT:
 *          devnum:             [0~31]
 *          map_table_dev:   [0~31]
 *          map_table_port:   [0~127]
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          NAM_OK
 *          NAM_ERR
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
unsigned long nam_asic_set_device_map_table(unsigned char devnum, 
                                                      unsigned char map_table_dev,
                                                      unsigned char map_table_port)
{
    unsigned char UplinkIsTrunk = 0;
    /*code optimize: Unsigned compared against 0
	zhangcl@autelan.com 2013-1-21*/
    if ((devnum>31)||(map_table_dev>31)||(map_table_port>127))
        return NAM_ERR;

    #ifdef DRV_LIB_CPSS
    prvCpssDrvHwPpSetRegField(devnum, 
                             (0x01A40004 + map_table_dev*0x10),
                              0, 1, UplinkIsTrunk);
    
    return prvCpssDrvHwPpSetRegField(devnum, 
                                    (0x01A40004 + map_table_dev*0x10),
                                     1, 7, map_table_port);
    #endif
}


/**********************************************************************
 * nam_distribute_srcid_egress_filter_table_set
 *
 *  DESCRIPTION:
 *          Set Source ID Egress Filter Table on each Device.
 *          SrcID(n): 0x01A4000C + n*0x10
 *
 *  INPUT:
 *          devnum:             [0~31]
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          NAM_OK
 *          NAM_ERR
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
unsigned long nam_distribute_srcid_egress_filter_table_set(unsigned char devnum)
{    
	/*code optimize: Unsigned compared against 0
	zhangcl@autelan.com 2013-1-21*/
    if ( devnum>31 )
        return NAM_ERR;

    #ifdef DRV_LIB_CPSS
    prvCpssDrvHwPpSetRegField(devnum, 
                              0x01A4000C,
                              26, 2, 0);
    
    return prvCpssDrvHwPpSetRegField(devnum, 
                              0x01A4002C,
                              26, 2, 0);
    #endif
}



#ifdef __cplusplus
}
#endif
