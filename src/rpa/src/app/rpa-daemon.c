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
* rpa_daemon.c
*
*
* CREATOR:
*		caojia@autelan.com
*
* DESCRIPTION:
*		Daemon process of RPA module
*
* DATE:
*		4/12/2012	
*
*  FILE REVISION NUMBER:
*  		
*******************************************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <errno.h>
#include <pthread.h>
#include <syslog.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <dbus/dbus.h>

#include "board/netlink.h"
#include "dbus/rpa/rpa_dbus_def.h"
#include "rpa-daemon.h"
	
DBusConnection *rpa_daemon_dbus_connection = NULL;

pthread_t rpa_daemon_dbus_function_thread;
pthread_t rpa_daemon_netlink_function_thread;

int sock_rpa_fd = 0;
struct msghdr rpa_msg;
struct nlmsghdr *rpa_nlh = NULL;
struct sockaddr_nl rpa_src_addr, rpa_dest_addr;
struct iovec rpa_iov;

unsigned int broadcast_mask = 0x0;
unsigned int default_broadcast_mask = 0x0;
int broadcast_mask_update_flag;

void rpa_daemon_syslog_emerg(char *format,...)
	
{
	char buf[2048] = {0};
	sprintf(buf,"RPA_DAEMON ");
	va_list ptr;
	va_start(ptr,format);
	vsprintf(buf+8,format,ptr);
	va_end(ptr);
	syslog(LOG_EMERG,buf);
	
}
void rpa_daemon_syslog_alert(char *format,...)
	
{
	char buf[2048] = {0};
	sprintf(buf,"RPA_DAEMON ");
	va_list ptr;
	va_start(ptr,format);
	vsprintf(buf+8,format,ptr);
	va_end(ptr);
	syslog(LOG_ALERT,buf);	
}
void rpa_daemon_syslog_crit(char *format,...)
	
{
	char buf[2048] = {0};
	sprintf(buf,"RPA_DAEMON ");
	va_list ptr;
	va_start(ptr,format);
	vsprintf(buf+8,format,ptr);
	
	va_end(ptr);
	syslog(LOG_CRIT,buf);
}
void rpa_daemon_syslog_err(char *format,...)
	
{
	char buf[2048] = {0};
	sprintf(buf,"RPA_DAEMON ");
	va_list ptr;
	va_start(ptr,format);
	vsprintf(buf+8,format,ptr);
	va_end(ptr);
	syslog(LOG_ERR,buf);
}
void rpa_daemon_syslog_warning(char *format,...)
	
{
	char buf[2048] = {0};
	sprintf(buf,"RPA_DAEMON ");
	va_list ptr;
	va_start(ptr,format);
	vsprintf(buf+8,format,ptr);
	va_end(ptr);
	syslog(LOG_WARNING,buf);
}
void rpa_daemon_syslog_notice(char *format,...)
	
{
	char buf[2048] = {0};
	sprintf(buf,"RPA_DAEMON ");
	va_list ptr;
	va_start(ptr,format);
	vsprintf(buf+8,format,ptr);
	va_end(ptr);
	syslog(LOG_NOTICE,buf);
}

void rpa_daemon_syslog_info(char *format,...)
	
{
	char buf[2048] = {0};
	sprintf(buf,"RPA_DAEMON ");
	va_list ptr;
	va_start(ptr,format);
	vsprintf(buf+8,format,ptr);
	va_end(ptr);
	syslog(LOG_INFO,buf);
}
void rpa_daemon_syslog_debug(char *format,...)
	
{
	char buf[2048] = {0};
	sprintf(buf,"RPA_DAEMON ");
	va_list ptr;
	va_start(ptr,format);
	vsprintf(buf+8,format,ptr);
	va_end(ptr);
	syslog(LOG_DEBUG,buf);
}

void rpa_daemon_init_syslog(void)
{
	openlog("rpa_daemon", LOG_PID, LOG_DAEMON);
}

int rpa_daemon_netlink_msg_send(char *msgBuf, int len)
{
	memcpy(NLMSG_DATA(rpa_nlh), msgBuf, len);
	
	nl_msg_head_t *head = (nl_msg_head_t*)NLMSG_DATA(rpa_nlh);

	rpa_daemon_syslog_debug("\tNetlink sem pid(%d) send to rpa(%d)\n", head->pid, head->object);
	
	if(sendmsg(sock_rpa_fd, &rpa_msg, 0) < 0) {
		rpa_daemon_syslog_debug("Failed sem netlink send to rpa : %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int rpa_daemon_netlink_init_to_rpa(void)
{	
	/* Initialize data field */
	memset(&rpa_src_addr, 0, sizeof(rpa_src_addr));
	memset(&rpa_dest_addr, 0, sizeof(rpa_dest_addr));
	memset(&rpa_iov, 0, sizeof(rpa_iov));
	memset(&rpa_msg, 0, sizeof(rpa_msg));
	
	/* Create netlink socket use NETLINK_RPA(25) */
	if ((sock_rpa_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_RPA)) < 0) {
		rpa_daemon_syslog_debug("Failed socket : %s\n", strerror(errno));
		return -1;
	}

	/* Fill in src_addr */
	rpa_src_addr.nl_family = AF_NETLINK;
	rpa_src_addr.nl_pid = getpid();
	/* Focus */
	rpa_src_addr.nl_groups = 1;

	if (bind(sock_rpa_fd, (struct sockaddr*)&rpa_src_addr, sizeof(rpa_src_addr)) < 0) {
		rpa_daemon_syslog_debug("Failed bind : %s\n", strerror(errno));
		return -1;
	}

	/* Fill in dest_addr */
	rpa_dest_addr.nl_pid = 0;
	rpa_dest_addr.nl_family = AF_NETLINK;
	/* Focus */
	rpa_dest_addr.nl_groups = 1;

	/* Initialize buffer */
	if((rpa_nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD))) == NULL) {
		rpa_daemon_syslog_debug("Failed malloc\n");
		return -1;
	}

	memset(rpa_nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	rpa_nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	rpa_nlh->nlmsg_pid = getpid();
	rpa_nlh->nlmsg_flags = 0;
	rpa_iov.iov_base = (void *)rpa_nlh;
	rpa_iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);
	rpa_msg.msg_name = (void *)&rpa_dest_addr;
	rpa_msg.msg_namelen = sizeof(rpa_dest_addr);
	rpa_msg.msg_iov = &rpa_iov;
	rpa_msg.msg_iovlen = 1;

    return 0;
}


int rpa_daemon_thread_create(pthread_t *new_thread, void * (*thread_rtn)(void *), int detachstate, void *arg)
{
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, detachstate);

	return pthread_create(new_thread, &attr, thread_rtn, arg);	
}

DBusMessage *rpa_daemon_set_broadcast_mask(DBusConnection *connection, DBusMessage *message, void *user_data)
{
	DBusMessage *reply = NULL;
	DBusMessageIter iter;
	DBusError err;
	int ret = -1;
	int timeout = 10;
	unsigned int mask;

	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;
	int msgLen = 0;
	char sendBuf[512] = {0};

	dbus_error_init(&err);

	if (!(dbus_message_get_args(message, &err,
								DBUS_TYPE_UINT32, &mask,
								DBUS_TYPE_INVALID)))
	{
		if (dbus_error_is_set(&err)) 
		{
			rpa_daemon_syslog_debug("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		ret = -1;
		reply = dbus_message_new_method_return(message);
	
		dbus_message_iter_init_append (reply, &iter);
	
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, &ret);
		return reply;
	}

	rpa_daemon_syslog_err("Mask : %#x\n", mask);

	broadcast_mask_update_flag = 0;
	head = (nl_msg_head_t*)sendBuf;
	nl_msg = (netlink_msg_t*)(sendBuf + sizeof(nl_msg_head_t));

	head->type = LOCAL_BOARD;
	head->object = COMMON_MODULE;
	head->count = 1;
	head->pid = getpid();

	nl_msg->msgType = RPA_BROADCAST_MASK_ACTION_EVENT;
	nl_msg->msgData.broadcast_mask_action.action_type = 1;
	nl_msg->msgData.broadcast_mask_action.mask = mask;

	msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t);

	rpa_daemon_netlink_msg_send(sendBuf, msgLen);

	while (timeout--) {
		if (broadcast_mask_update_flag) {
			ret = 0;
			break;
		}
		usleep(300000);
	}

	reply = dbus_message_new_method_return(message);
	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &ret);
	return reply;
}

DBusMessage *rpa_daemon_get_broadcast_mask(DBusConnection *connection, DBusMessage *message, void *user_data)
{
	DBusMessage *reply = NULL;
	DBusMessageIter iter;
	DBusError err;
	int ret = -1;
	int timeout = 10;
	unsigned int mask = 0x0;
	int is_default = 0;

	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;
	int msgLen = 0;
	char sendBuf[512] = {0};

	dbus_error_init(&err);

	head = (nl_msg_head_t*)sendBuf;
	nl_msg = (netlink_msg_t*)(sendBuf + sizeof(nl_msg_head_t));

	broadcast_mask_update_flag = 0;
	head->type = LOCAL_BOARD;
	head->object = COMMON_MODULE;
	head->count = 1;
	head->pid = getpid();

	nl_msg->msgType = RPA_BROADCAST_MASK_ACTION_EVENT;
	nl_msg->msgData.broadcast_mask_action.action_type = 0;
	nl_msg->msgData.broadcast_mask_action.mask = mask;

	msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t);

	rpa_daemon_netlink_msg_send(sendBuf, msgLen);

	while (timeout--) {
		if (broadcast_mask_update_flag) {
			mask = broadcast_mask;
			if (broadcast_mask == default_broadcast_mask) {
				is_default = 1;
			}
			ret = 0;
			break;
		}
		usleep(300000);
	}

	reply = dbus_message_new_method_return(message);
	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &ret);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &mask);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &is_default);
	return reply;
}

DBusMessage *rpa_daemon_show_dev_index_table(DBusConnection *connection, DBusMessage *message, void *user_data)
{
	DBusMessage *reply = NULL;
	DBusMessageIter iter;
	DBusError err;
	int ret = 0;

	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;
	int msgLen = 0;
	char sendBuf[512] = {0};

	dbus_error_init(&err);

	head = (nl_msg_head_t*)sendBuf;
	nl_msg = (netlink_msg_t*)(sendBuf + sizeof(nl_msg_head_t));

	head->type = LOCAL_BOARD;
	head->object = COMMON_MODULE;
	head->count = 1;
	head->pid = getpid();

	nl_msg->msgType = RPA_DEBUG_CMD_EVENT;
	nl_msg->msgData.debug_cmd_type = RPA_SHOW_DEV_INDEX_TABLE;

	msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t);

	rpa_daemon_netlink_msg_send(sendBuf, msgLen);

	reply = dbus_message_new_method_return(message);
	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32, &ret);
	return reply;
}

static DBusHandlerResult rpa_daemon_dbus_msg_handler(
	DBusConnection *connection, 
	DBusMessage *message, 
	void *user_data)
{
	DBusMessage *reply = NULL;

	if (strcmp(dbus_message_get_path(message), RPA_DAEMON_DBUS_OBJPATH) == 0) {
		if (dbus_message_is_method_call(message, RPA_DAEMON_DBUS_INTERFACE, RPA_DAEMON_DBUS_SET_BROADCAST_MASK)) {
			reply = rpa_daemon_set_broadcast_mask(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message, RPA_DAEMON_DBUS_INTERFACE, RPA_DAEMON_DBUS_SHOW_BROADCAST_MASK)) {
			reply = rpa_daemon_get_broadcast_mask(connection, message, user_data);
		}
		else if (dbus_message_is_method_call(message, RPA_DAEMON_DBUS_INTERFACE, RPA_DAEMON_DBUS_SHOW_DEV_INDEX_TABLE)) {
			reply = rpa_daemon_show_dev_index_table(connection, message, user_data);
		}
		else {	
			rpa_daemon_syslog_debug("Unknow obj path %s", dbus_message_get_path(message));
		}
	}

	if (reply) {
		dbus_connection_send (connection, reply, NULL);
		dbus_connection_flush(connection);
		dbus_message_unref (reply);
	}

	return DBUS_HANDLER_RESULT_HANDLED ;
}

DBusHandlerResult rpa_daemon_dbus_filter_function(
	DBusConnection * connection, 
	DBusMessage * message, 
	void *user_data  )
{
	if (dbus_message_is_signal(message, DBUS_INTERFACE_LOCAL, "Disconnected") && 
		strcmp (dbus_message_get_path(message), DBUS_PATH_LOCAL) == 0) {
		/* this is a local message; e.g. from libdbus in this process */

		dbus_connection_unref (rpa_daemon_dbus_connection);
		rpa_daemon_dbus_connection = NULL;

		/*g_timeout_add (3000, reinit_dbus, NULL);*/
	} 
	else if (dbus_message_is_signal (message, 
							DBUS_INTERFACE_DBUS, 
							"NameOwnerChanged")) {
		;
	} 
	else {
		return 1;
	}
	
	return DBUS_HANDLER_RESULT_HANDLED;
}


int rpa_daemon_dbus_init(void)
{
	DBusError dbus_error;
	DBusObjectPathVTable rpa_daemon_dbus_vtable = {NULL, &rpa_daemon_dbus_msg_handler, NULL, NULL, NULL, NULL};

	dbus_error_init (&dbus_error);
	rpa_daemon_dbus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
	if (rpa_daemon_dbus_connection == NULL) {
		rpa_daemon_syslog_warning("Initial rpa-daemon dbus dbus_bus_get() failed : %s", dbus_error.message);
		return -1;
	}

	if (!dbus_connection_register_fallback(rpa_daemon_dbus_connection, RPA_DAEMON_DBUS_OBJPATH, &rpa_daemon_dbus_vtable, NULL)) {
		rpa_daemon_syslog_warning("Register rpa_daemon dbus handlers (fallback dhcpsnp) failed.\n");
		return -1;
	}
		
	dbus_bus_request_name(rpa_daemon_dbus_connection, RPA_DAEMON_DBUS_BUSNAME, 0, &dbus_error);

	if (dbus_error_is_set(&dbus_error)) {
		rpa_daemon_syslog_warning("Rpa-daemon dbus request bus name failed : %s\n", dbus_error.message);
		return -1;
	}

	dbus_connection_add_filter (rpa_daemon_dbus_connection, rpa_daemon_dbus_filter_function, NULL, NULL);

	dbus_bus_add_match (rpa_daemon_dbus_connection,
				"type='signal'"
				",interface='"DBUS_INTERFACE_DBUS"'"
				",sender='"DBUS_SERVICE_DBUS"'"
				",member='NameOwnerChanged'",
				NULL);
	return 0;
}

void rpa_daemon_dbus_function(void)
{
	rpa_daemon_syslog_debug("Rpa daemon thread %s(%d)\n", __FUNCTION__, getpid());
	if (rpa_daemon_dbus_init()) {
		rpa_daemon_syslog_warning("Initialize %s failed.\n", __func__);
		exit(-1);
	}

	while (1) {
		if (dbus_connection_read_write_dispatch(rpa_daemon_dbus_connection, -1))
		{
			;
		}
	}
}

void rpa_daemon_netlink_function(void)
{
	rpa_daemon_syslog_debug("Rpa daemon thread %s(%d)\n", __FUNCTION__, getpid());
	if (rpa_daemon_netlink_init_to_rpa()) {
		rpa_daemon_syslog_warning("Netlink initial in rpa-daemon failed.\n");
	}

	while (1) {
		if(recvmsg(sock_rpa_fd, &rpa_msg, 0) <= 0) {
			rpa_daemon_syslog_warning("Failed sem netlink recv : %s\n", strerror(errno));
		}
		
		//nl_msg_head_t *head = (nl_msg_head_t*)NLMSG_DATA(rpa_nlh);
		netlink_msg_t *nl_msg = (netlink_msg_t *)(NLMSG_DATA(rpa_nlh) + sizeof(nl_msg_head_t));

		switch (nl_msg->msgType) {
			case RPA_BROADCAST_MASK_ACTION_EVENT:
				rpa_daemon_syslog_debug("RPA module the broadcast mask : %#x .\n", nl_msg->msgData.broadcast_mask_action.mask);
				if (nl_msg->msgData.broadcast_mask_action.action_type == MASK_NOTIFY) {
					broadcast_mask_update_flag = 1;
					broadcast_mask = nl_msg->msgData.broadcast_mask_action.mask;
				}
				else if (nl_msg->msgData.broadcast_mask_action.action_type == DEFAULT_MASK_NOTIFY) {
					default_broadcast_mask = nl_msg->msgData.broadcast_mask_action.mask;
				}
				break;
			default:
				rpa_daemon_syslog_debug("Receive an unknown message type.\n");
				break;
		}
	}
}

void rpa_daemon_keep_running(void)
{
	while(1) {
		usleep(1000);
	}
}

int main(int argc, char **argv)
{
	rpa_daemon_init_syslog();

	rpa_daemon_syslog_debug("Rpa daemon thread %s(%d)\n", __FUNCTION__, getpid());

	if (rpa_daemon_thread_create(&rpa_daemon_dbus_function_thread, 
		(void *)rpa_daemon_dbus_function, PTHREAD_CREATE_DETACHED, NULL)) {
		rpa_daemon_syslog_warning("rpa_daemon_dbus_function_thread create failed.\n");
	}

	if (rpa_daemon_thread_create(&rpa_daemon_netlink_function_thread, 
		(void *)rpa_daemon_netlink_function, PTHREAD_CREATE_DETACHED, NULL)) {
		rpa_daemon_syslog_warning("rpa_daemon_netlink_function_thread create failed.\n");
	}

	rpa_daemon_keep_running();

	return 0;
}

#ifdef __cplusplus
}
#endif
