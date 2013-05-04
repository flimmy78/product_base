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
* npd_vrrp.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for virtual router redundancy protocol.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.3 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd/nam/npd_amapi.h"
#include "npd_product.h"
#include "npd_log.h"
#include "nbm/nbm_api.h"
#include "npd_c_slot.h"
#include "npd_main.h"
#include "npd_eth_port.h"


/*******************************************************************************
* npd_dbus_vrrp_get_sysmac
*
* DESCRIPTION:
*      Get the sys mac.
*
* INPUTS:
*      None
*
* OUTPUTS:
*      char* sysmac
*
* RETURNS:
*       reply.
*
* COMMENTS:
*       
*
*******************************************************************************/
DBusMessage * npd_dbus_vrrp_get_sysmac(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter iter;
	unsigned int	ifindex = 0;
	unsigned int	ret = NPD_SUCCESS;
	unsigned char  macAddr[6] = {0};
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&ifindex,
									DBUS_TYPE_INVALID))) {
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_product_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		ret = NPD_FAIL;
	}
    ret = npd_system_get_basemac(macAddr,6);
	if(0 != ret){
       npd_syslog_err("Can not get sysmac!\n");
	   reply = dbus_message_new_method_return(msg);
	   dbus_message_append_args(reply,
								 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);	   
	}
	else{
       npd_syslog_dbg("npd get sysmac: %02x:%02x:%02x:%02x:%02x:%02x\n",macAddr[0],\
	   	               macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
	   reply = dbus_message_new_method_return(msg);
	   dbus_message_iter_init_append (reply, &iter);   
	   dbus_message_iter_append_basic(&iter,
									 DBUS_TYPE_UINT32,&ret);
	   dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_BYTE,&macAddr[0]);
	   dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_BYTE,&macAddr[1]);
	   dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_BYTE,&macAddr[2]);
	   dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_BYTE,&macAddr[3]);
	   dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_BYTE,&macAddr[4]);
	   dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_BYTE,&macAddr[5]);
	}	
	return reply;	
}

#ifdef __cplusplus
}
#endif


