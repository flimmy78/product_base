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
* npd_dbm.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		Distributed Board Management module
*
* DATE:
*		5/21/2010	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.7 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sysdef/npd_sysdef.h>
#include <util/npd_list.h>
#include <npd/nam/npd_amapi.h>
#include <npd/nbm/npd_bmapi.h>
#include "npd_c_slot.h"
#include <net/if_arp.h>
#include <sys/wait.h>

#include "npd_log.h"
#include "npd_product.h"
#include "npd_main.h"
#include "dccnetlink.h"
#include "dccn_lib.h"
#include "npd_dbm.h"

/*pthread type*/
pthread_t thread_dccn_recvskb;

struct dccn_handle dch_t = { .fd = -1 };
struct dccn_handle *dch_tp = &dch_t;
int dbmFd = -1;

/**********************************************************************************
 *  npd_dbm_init
 *
 *	DESCRIPTION:
 * 		Initialize DBM module
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
void npd_dbm_init()
{
#define NPD_DBM_REQUEST_MOD_LINE	"/sbin/insmod lib/modules/2.6.16.26-Cavium-Octeon/misc/dccn.ko dif=%s"
#define NPD_DBM_TEST_MOD_LINE		"test -d /sys/module/dccn"
#define NPD_DBM_REQUEST_MOD_LEN	256
	unsigned char state = 0xFF;
	int result = NPD_SUCCESS, local_slot = -1;
	char request[NPD_DBM_REQUEST_MOD_LEN] = {0};
	int rval = 0,status =0;
	local_slot = LOCAL_CHASSIS_SLOT_NO;

	/* DBM only take effect for dual master device */
	if(PRODUCT_ID_AX7K_I != PRODUCT_ID) {
		syslog_ax_dbm_warning("dbm init skip this product %s!\n", product_id_str(PRODUCT_ID));
		return;
	}
	
	/* check peer board is on slot or not */
	result = nbm_query_peerboard_status(&state);
	if(NPD_SUCCESS != result) {
		syslog_ax_dbm_err("dbm init query peer board status error %d!\n", result);
		return;
	}

	/*if(state) {
		syslog_ax_dbm_err("dbm init found peer board isn't on slot, skip!\n");
		return;
	}*/

	/* initialize cascading ports : cscd0/cscd1 */
	result = npd_dbm_cscd_intf_init();
	if(NPD_SUCCESS != result) {
		syslog_ax_dbm_err("dbm init cscd interface error %d!\n", result);
		return;
	}

	/* request DCCN.ko module */
	sprintf(request, NPD_DBM_REQUEST_MOD_LINE,  "cscd0");
	system(request);
	
	/* TODO: here we should check if module inserted or not */
#if 0 
	if((dbmFd = open("/dev/dccnChk",O_RDWR)) < 0){
		syslog_ax_dbm_err("dbm init open dccnChk error,dbmFd %d!\n", dbmFd);
		return;
	}
#endif
	sprintf(request, NPD_DBM_TEST_MOD_LINE);
	rval = system(request);
	status = WEXITSTATUS(rval);
	if(0 != status){
		syslog_ax_dbm_err("dbm init test dccn insmod error,status %d,rval %d!\n", status,rval);
		return;
	}
	else{
		syslog_ax_dbm_err("dbm init test dccn insmod ok,status %d,rval %d!\n", status,rval);
	}

	/* create new thread to execute master/slave election */
	nam_thread_create("dbm", (void *)npd_dbm_dccn_thread,  NULL, NPD_FALSE, NPD_FALSE);
}

/**********************************************************************************
 *  npd_dbm_cscd_intf_init
 *
 *	DESCRIPTION:
 * 		Initialize cascading ports related interface(s), which are needed by DCCN module
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
int npd_dbm_cscd_intf_init(void)
{
	int result = NPD_SUCCESS;
	unsigned char	slot,port = 0;
	unsigned int  ipadd =0;
	unsigned char ipaddr[4] = {0};

	result = npd_set_cscd_port_promi_mode(port);
	if(NPD_SUCCESS != result) {
		syslog_ax_dbm_err("npd dbm init cscd interface error %d!\n", result);
		return result;
	}
	result = npd_set_cscd_port_promi_mode(1);
#if 0 
	slot = LOCAL_CHASSIS_SLOT_NO;
	if(0 == slot){
		ipaddr[0] = 169;
		ipaddr[1] = 254;
		ipaddr[2] = 0;
		ipaddr[3] = 1;
	}else if(1 == slot){
		ipaddr[0] = 169;
		ipaddr[1] = 254;
		ipaddr[2] = 0;
		ipaddr[3] = 2;
	}
		ipadd |= ipaddr[0];
		ipadd = (ipadd << 8) | ipaddr[1];
		ipadd = (ipadd << 8) | ipaddr[2];
		ipadd = (ipadd << 8) | ipaddr[3];
	
	syslog_ax_dbm_dbg("set cscd port ipaddr %d:%d:%d:%d! ipadd %x.\n", \
							ipaddr[0], ipaddr[1], ipaddr[2],ipaddr[3],ipadd);

	result = npd_set_cscd_port_ipaddr(ipadd,port);
	if(NPD_SUCCESS != result){
		syslog_ax_dbm_err("set cscd port ipaddr error!\n");
		return result;
	}
	#endif

	return result;
}

/**********************************************************************************
 *  npd_dbm_dccn_thread
 *
 *	DESCRIPTION:
 * 		Main routine for thread which executes DCC netlink communication and 
 * 	performs board role election(Master/Slave).
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
int npd_dbm_dccn_thread(void)
{
	int result = NPD_SUCCESS;

	syslog_ax_dbm_dbg("npd dbm send and receive broadcast message.\n");

	int ret =0;
	int len = 0;
	int groups = 0;
	struct dccn_sendarg send_uarg;
	struct dccn_sendarg *uargp = NULL;
	struct dccn_msg_endpoint localer;
	
	memset(&send_uarg, 0, sizeof(struct dccn_sendarg));
	memset(&localer, 0, sizeof(struct dccn_msg_endpoint));

	char buff[]= "hello world!!";	/* set buff content*/
	len = sizeof(buff);

	/*dccn open process*/
	syslog_ax_dbm_dbg("npd dbm entry open process.\n");
	localer.slotno = LOCAL_CHASSIS_SLOT_NO;
	localer.mod = LOCAL_MODULE_ID;
	groups = 1;
	DCCN_OPEN(dch_tp,localer, groups);
	
	/*rcv msg cyclical*/
	syslog_ax_dbm_dbg("npd dbm entry rcv msg cyclical.\n");
	ret = pthread_create(&thread_dccn_recvskb,NULL,(void *)npd_dbm_create_dccn_recvskb_thread,NULL);
	if( 0 != ret )
	{
		syslog_ax_dbm_err("npd dbm Create dccn packet skb thread fail.\n");
		result = NPD_FAIL;
		return result;
	}

	send_uarg.dch = dch_tp;
	send_uarg.buff = buff;
	send_uarg.type = DCCN_MULTICAST;
	send_uarg.len = len;
	send_uarg.groups = 1;
	send_uarg.localslot = LOCAL_CHASSIS_SLOT_NO;
	send_uarg.remoteslot = LOCAL_CHASSIS_SLOT_NO?0:1;
	send_uarg.localmod = LOCAL_MODULE_ID;
	send_uarg.remotemod = LOCAL_MODULE_ID;
	uargp = &send_uarg;
	
	syslog_ax_dbm_dbg("npd dbm entry send msg cyclical.\n");
	while(1){
		/*send msg cyclical*/
		DCCN_MESG_SEND(uargp);

		sleep(2);
	}

	/*end the dccn to run close*/	
	syslog_ax_dbm_dbg("npd dbm entry close dccn.\n");
	DCCN_CLOSE(dch_tp);

	return result;
}

/**********************************************************************************
 *  npd_dbm_create_dccn_recvskb_thread
 *
 *	DESCRIPTION:
 * 		Main routine for thread which receive packet via DCC netlink communication and 
 * 	handle the packet.
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
static void *npd_dbm_create_dccn_recvskb_thread(void)
{
	while(1){
			DCCN_MESG_RCV(dch_tp, npd_dbm_skb_handle, NULL);
/*			if < 0){
				dccn_syslog_err("dccn error when user dccn mesg rcv.\n");
			}
			else{
					dccn_syslog_event("dccn mesg rcv successfully!\n");
			}
*/	}
}

/**********************************************************************************
 *  npd_dbm_skb_handle
 *
 *	DESCRIPTION:
 * 		Main routine for handle message received via DCC netlink communication.
 *
 *	INPUT:
 *		addr - netlink address
 *		dccnhdr - DCC Netlink header point to message buffer
 *		len - message buffer size
 *		arg - appendix
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
int npd_dbm_skb_handle
(
	const struct sockaddr_nl *addr,
	struct dccn_msghdr *dccnhdr,
	int len,
	void *arg
)
{
	unsigned char *data = NULL;
	data = (unsigned char *)dccnhdr;
	
	if(addr->nl_groups == 0){
		syslog_ax_dbm_dbg(" Npd dbm received unicast message payload: %dB\n", len);
		DCCN_PRINT_NLHR(data, len);
		return 0;
	}
	else{
		syslog_ax_dbm_dbg(" Npd dbm received broadcast message payload: %dB\n", len);
		DCCN_PRINT_NLHR(data, len);
		return 0;
	}
}

#ifdef __cplusplus
}
#endif
