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
* npd_engine.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for ASIC
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.17 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dbus/dbus.h>
#include "sysdef/npd_sysdef.h"
#include "nam/nam_main_api.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_log.h"
#include "npd_engine.h"

CPU_PORT_MIB_INFO_S **g_cpu_port_mib = NULL;
extern struct product_s productinfo;

int npd_fw_engines_init(void) {	
	if(!((productinfo.capbmp) & FUNCTION_ASIC_VLAUE)){
		npd_syslog_dbg("do not support ASIC init!\n");
		return NPD_FAIL;   /* code optimize: Missing return statement. zhangdi@autelan.com 2013-01-18 */
	}
	else if(((productinfo.capbmp) & FUNCTION_ASIC_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ASIC_VLAUE)){
		npd_syslog_dbg("do not support ASIC init!\n");
		return NPD_FAIL;
	}
	else{
		nam_asic_init(productinfo.product_id);
	}
	
	return NPD_SUCCESS;
}

int npd_fw_engine_initialization_check(void) {

	/*if(PRODUCT_ID_AX5K_E == productinfo.product_id){
		syslog_ax_engine_event("PRODUCT_ID_AX5K_E not support ASIC init check...\n");
		return;
	}	*/

	if(!((productinfo.capbmp) & FUNCTION_ASIC_VLAUE)){
		npd_syslog_dbg("do not support ASIC init check...!\n");
		return NPD_FAIL;
	}
	else if(((productinfo.capbmp) & FUNCTION_ASIC_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ASIC_VLAUE)){
		npd_syslog_dbg("do not support ASIC init check...!\n");
		return NPD_FAIL;
	}
	else{
	unsigned long numOfAsic = 0, i = 0;
	CPU_PORT_MIB_INFO_S *mibPtr = NULL;

	nam_thread_create("BoardInitCheck",(void *)nam_asic_init_completion_check,NULL,NPD_FALSE,NPD_TRUE);

	/* after all asic initialization done, check asic instance*/
	numOfAsic = nam_asic_get_instance_num();

	g_cpu_port_mib = (CPU_PORT_MIB_INFO_S **)malloc(numOfAsic * sizeof(CPU_PORT_MIB_INFO_S *));
	if(NULL == g_cpu_port_mib) {
		syslog_ax_engine_err("init forwarding engine cpu port mib error\n");
		return NPD_FAIL;
	}
	memset(g_cpu_port_mib, 0, numOfAsic*sizeof(CPU_PORT_MIB_INFO_S*));
	
	for(i = 0; i < numOfAsic; i++) {
		mibPtr = (CPU_PORT_MIB_INFO_S *)malloc(sizeof(CPU_PORT_MIB_INFO_S));
		if(NULL == mibPtr) {
			syslog_ax_engine_err("init forwarding device %d cpu port mib info error\n", i);
			return NPD_FAIL;
		}
		memset(mibPtr, 0, sizeof(CPU_PORT_MIB_INFO_S));
		g_cpu_port_mib[i] = mibPtr;
	}
	return NPD_SUCCESS;
	}
}

DBusMessage * npd_dbus_cpufc_set_queue_quota
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned int queue = 0;
	unsigned int quota = 0;
	unsigned int retVal = 0;
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&queue,
		DBUS_TYPE_UINT32,&quota,
		DBUS_TYPE_INVALID))) {
		syslog_ax_engine_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_engine_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	retVal = nam_asic_set_cpufc_queue_quota(queue,quota);	

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &retVal);	
	return reply;
}

DBusMessage * npd_dbus_cpufc_get_queue_quota
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int i = 0, fullQuota[8] = {0};
	unsigned int retVal = 0;
	
	dbus_error_init(&err);

	for(i = 0; i < 8; i++) {
		retVal = nam_asic_get_cpufc_queue_quota(i,&fullQuota[i]);
	}
	
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
					DBUS_TYPE_UINT32,&retVal);
	for(i = 0; i < 8; i++) {
		dbus_message_iter_append_basic (&iter,
						DBUS_TYPE_UINT32,&fullQuota[i]);
	}
	
	return reply;	
}

/**********************************************************************************
 * npd_dbus_cpufc_set_queue_shaper
 * 
 * DESCRIPTION:
 *	This method set cpu port flow control shaper value at port queue level
 *
 *
 *	INPUT:
 *		queue - queue number
 *		shaper - shaper value assigned to the queue(in unit of PPS)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
DBusMessage * npd_dbus_cpufc_set_queue_shaper
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned int queue = 0;
	unsigned int shaper = 0;
	unsigned int retVal = 0;
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&queue,
		DBUS_TYPE_UINT32,&shaper,
		DBUS_TYPE_INVALID))) {
		syslog_ax_engine_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_engine_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	retVal = nam_asic_set_cpufc_queue_shaper(queue,shaper);	

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &retVal);	
	return reply;
}

/**********************************************************************************
 * npd_dbus_cpufc_set_queue_shaper
 * 
 * DESCRIPTION:
 *	This method set cpu port flow control shaper value at port level
 *
 *
 *	INPUT:
 *		shaper - shaper value assigned to the queue(in unit of PPS)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
DBusMessage * npd_dbus_cpufc_set_port_shaper
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned int shaper = 0;
	unsigned int retVal = 0;
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&shaper,
		DBUS_TYPE_INVALID))) {
		syslog_ax_engine_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_engine_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	retVal = nam_asic_set_cpufc_port_shaper(shaper);	

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &retVal);	
	return reply;
}

/**********************************************************************************
 * npd_dbus_cpu_show_sdma_info
 * 
 * DESCRIPTION:
 *	This method show cpu port SDMA Rx/Tx channel descriptor configuration.
 *
 *
 *	INPUT:
 *		queueIdx - CPU queue number.
 *		direction - Rx or Tx or Both direction.
 *	
 *	OUTPUT:
 *		showStr - show result as string
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
DBusMessage * npd_dbus_cpu_show_sdma_info
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char queue = 0, direction = 0;
	unsigned char *showStr = NULL;
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&queue,
							DBUS_TYPE_BYTE,&direction,
							DBUS_TYPE_INVALID))) {
		syslog_ax_engine_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_engine_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	showStr = (unsigned char *)malloc(NPD_ENGINE_CPU_PERQUEUE_SHOWSIZE);
	if(NULL == showStr) {
		syslog_ax_engine_err("show queue %d info get memory error\n",queue);
		return NULL;
	}
	memset(showStr, 0, NPD_ENGINE_CPU_PERQUEUE_SHOWSIZE);
	
	nam_asic_show_cpu_sdma_info(0,queue,direction, &showStr,NPD_ENGINE_CPU_PERQUEUE_SHOWSIZE);	

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr);	
	free(showStr);
	showStr = NULL;
	return reply;
}

/**********************************************************************************
 * npd_dbus_cpu_show_sdma_mib
 * 
 * DESCRIPTION:
 *	This method get cpu port SDMA Rx/Tx channel mib information
 *
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
DBusMessage * npd_dbus_cpu_show_sdma_mib
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	CPU_PORT_DMA_CHANNEL_MIB_S *cpuPortMib = NULL, *tmpDMPtr = NULL;
	CPU_PORT_MAC_MIB_S cpuMacMib, *tmpMMPtr = NULL;
	unsigned char i = 0, j = 0, dbusItemPci = 0, dbusItemSmi = 0, maxItem = 0;
	unsigned long result = NPD_OK, ppNum = 0, cpuIntfType = 0, reserved = 0;

	memset(&cpuMacMib, 0,sizeof(CPU_PORT_MAC_MIB_S));
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_INVALID))) {
		syslog_ax_engine_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_engine_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	ppNum = nam_asic_get_instance_num();
	cpuPortMib = (CPU_PORT_DMA_CHANNEL_MIB_S *)malloc( \
						NPD_ENGINE_CPU_CHANNEL_NUM * sizeof(CPU_PORT_DMA_CHANNEL_MIB_S));
	if(NULL == cpuPortMib) {
		syslog_ax_engine_err("fetch memory error when get cpu dma channel mib\n");
		return NULL;
	}
	memset(cpuPortMib, 0, sizeof(CPU_PORT_DMA_CHANNEL_MIB_S)*NPD_ENGINE_CPU_CHANNEL_NUM);

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
							DBUS_TYPE_UINT32,&ppNum);
	for(i = 0; i < ppNum; i++) {
		cpuIntfType = nam_asic_get_cpu_interface_type(i);

		dbus_message_iter_append_basic (&iter,
										DBUS_TYPE_UINT32,&cpuIntfType);
		/* using SDMA (PCI interface)*/
		if(0x0 == cpuIntfType) {
			result = nam_asic_get_cpu_sdma_mib(i,&cpuPortMib);	
			if(NPD_OK != result){
				syslog_ax_engine_err("get device %d cpu dma channel mib error %d\n", i, result);
			}

			/* append dbus item*/
			for(j = 0; j < NPD_ENGINE_CPU_CHANNEL_NUM; j++) {
				tmpDMPtr = &(g_cpu_port_mib[i]->dmaMib[j]);
				tmpDMPtr->rxInPkts += cpuPortMib[j].rxInPkts;
				tmpDMPtr->rxInOctets += cpuPortMib[j].rxInOctets;
				tmpDMPtr->rxErrCnt += cpuPortMib[j].rxErrCnt;

				syslog_ax_engine_dbg("device%d dma%d rx pkts %d(%d) octets %d(%d) errors %d(%d)\n", \
							i, j, cpuPortMib[j].rxInPkts,tmpDMPtr->rxInPkts, \
							cpuPortMib[j].rxInOctets, tmpDMPtr->rxInOctets, \
							cpuPortMib[j].rxErrCnt, tmpDMPtr->rxErrCnt);
				dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&(tmpDMPtr->rxInPkts));
				dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&(tmpDMPtr->rxInOctets));
				dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&(tmpDMPtr->rxErrCnt));				
				dbusItemPci += 3;				
			}
		}
		/* using Ethernet MAC (SMI interface)*/
		else {
			result = nam_asic_get_cpu_mac_mib(i,&cpuMacMib); 
			{
				memcpy(&(g_cpu_port_mib[i]->etherMacMib),&cpuMacMib,sizeof(CPU_PORT_MAC_MIB_S));
			}
			if(NPD_OK != result){
				syslog_ax_engine_err("get device %d cpu ethernet mac mib error %d\n", i, result);
			}

			tmpMMPtr = &(g_cpu_port_mib[i]->etherMacMib);
			tmpMMPtr->rx.goodPkts[0] += cpuMacMib.rx.goodPkts[0];
			tmpMMPtr->rx.goodPkts[1] += cpuMacMib.rx.goodPkts[1];
			tmpMMPtr->rx.badPkts[0] += cpuMacMib.rx.badPkts[0];
			tmpMMPtr->rx.badPkts[1] += cpuMacMib.rx.badPkts[1];
			tmpMMPtr->rx.goodOctets[0] += cpuMacMib.rx.goodOctets[0];
			tmpMMPtr->rx.goodOctets[1] += cpuMacMib.rx.goodOctets[1];
			tmpMMPtr->rx.badOctets[0] += cpuMacMib.rx.badOctets[0];
			tmpMMPtr->rx.badOctets[1] += cpuMacMib.rx.badOctets[1];
			tmpMMPtr->rx.internalDrop[0] += cpuMacMib.rx.internalDrop[0];
			tmpMMPtr->rx.internalDrop[1] += cpuMacMib.rx.internalDrop[1];
			tmpMMPtr->tx.goodPkts[0] += cpuMacMib.tx.goodPkts[0];
			tmpMMPtr->tx.goodPkts[1] += cpuMacMib.tx.goodPkts[1];
			tmpMMPtr->tx.goodOctets[0] += cpuMacMib.tx.goodOctets[0];
			tmpMMPtr->tx.goodOctets[1] += cpuMacMib.tx.goodOctets[1];
			tmpMMPtr->tx.macErrPkts[0] += cpuMacMib.tx.macErrPkts[0];
			tmpMMPtr->tx.macErrPkts[1] += cpuMacMib.tx.macErrPkts[1];

			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->rx.goodPkts[0]));
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->rx.goodPkts[1]));
			dbusItemSmi += 2;
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->rx.badPkts[0]));
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->rx.badPkts[1]));				
			dbusItemSmi += 2;
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->rx.goodOctets[0]));
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->rx.goodOctets[1]));				
			dbusItemSmi += 2;
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->rx.badOctets[0]));
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->rx.badOctets[1]));				
			dbusItemSmi += 2;
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->rx.internalDrop[0]));
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->rx.internalDrop[1]));				
			dbusItemSmi += 2;
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->tx.goodPkts[0]));
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->tx.goodPkts[1]));				
			dbusItemSmi += 2;
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->tx.goodOctets[0]));
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->tx.goodOctets[1]));				
			dbusItemSmi += 2;
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->tx.macErrPkts[0]));
			dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,&(tmpMMPtr->tx.macErrPkts[1]));				
			dbusItemSmi += 2;
			
		}
		/*	!!!!attention !!!
		  * two different management interface type must
		  *  transfer the same amount of counter value , which is now as follow:
		  * MAX ( 8 * CPU_PORT_DMA_CHANNEL_MIB_S , CPU_PORT_MAC_MIB_S)
		  */
		syslog_ax_engine_dbg("show cpu port mib get %s mib counter %d items\n", \
						cpuIntfType ? "MAC":"SDMA", cpuIntfType ? dbusItemSmi : dbusItemPci);
		/* using cpu ethernet mac mib has less items*/	
		if(0x0 != cpuIntfType) {
			maxItem = (NPD_ENGINE_CPU_CHANNEL_NUM * sizeof(CPU_PORT_DMA_CHANNEL_MIB_S))/sizeof(unsigned int);
			for(i = dbusItemSmi; i <= maxItem; i++) {
				dbus_message_iter_append_basic (&iter,
										DBUS_TYPE_UINT32,&reserved);		
			}
		}
	}

	free(cpuPortMib);
	cpuPortMib = NULL;
	return reply;
}

/**********************************************************************************
 * npd_dbus_cpu_clear_sdma_mib
 * 
 * DESCRIPTION:
 *	This method clear cpu port SDMA Rx/Tx channel mib information
 *
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
DBusMessage * npd_dbus_cpu_clear_sdma_mib
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	DBusError err;
	int i = 0;
	unsigned long result = NPD_OK, ppNum = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_INVALID))) {
		syslog_ax_engine_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_engine_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	ppNum = nam_asic_get_instance_num();

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	for(i = 0; i < ppNum; i++) {
		memset(&(g_cpu_port_mib[i]->dmaMib[0]), 0,	\
			NPD_ENGINE_CPU_CHANNEL_NUM * sizeof(CPU_PORT_DMA_CHANNEL_MIB_S));
	}
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &result);	

	return reply;
}


#ifdef __cplusplus
}
#endif
