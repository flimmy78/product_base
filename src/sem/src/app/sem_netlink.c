#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <errno.h>
#include <sys/mman.h> 
#include <linux/tipc.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <sys/sysinfo.h>    /* for sysinfo() */

#include "sem_common.h"
#include "sem/sem_tipc.h"
#include "product/board/board_feature.h"
#include "product/product_feature.h"
#include "sysdef/sem_sysdef.h"
#include "board/board_common.h"
#include "sem_eth_port_dbus.h"
#include "board/netlink.h"
#include "sem_dbus.h"


#define LINK_UP             1
#define LINK_DOWN           0


int sock_s_fd = 0;
struct msghdr s_msg;
struct nlmsghdr *s_nlh = NULL;
struct sockaddr_nl s_src_addr, s_dest_addr;
struct iovec s_iov;

int sock_ks_fd = 0;
struct msghdr ks_msg;
struct nlmsghdr *ks_nlh = NULL;
struct sockaddr_nl ks_src_addr, ks_dest_addr;
struct iovec ks_iov;

int sock_ko_fd = 0;
struct msghdr ko_msg;
struct nlmsghdr *ko_nlh = NULL;
struct sockaddr_nl ko_src_addr, ko_dest_addr;
struct iovec ko_iov;

int sock_rpa_fd = 0;
struct msghdr rpa_msg;
struct nlmsghdr *rpa_nlh = NULL;
struct sockaddr_nl rpa_src_addr, rpa_dest_addr;
struct iovec rpa_iov;

extern board_fix_param_t *local_board;
extern product_fix_param_t *product;
extern struct global_ethport_s **global_ethport;
extern struct global_ethport_s *start_fp[MAX_SLOT_COUNT];
extern thread_index_sd_t thread_id_arr[MAX_SLOT_NUM];
extern int g_recv_sd;
extern int switch_flag;
extern pthread_mutex_t switch_lock;

extern int sem_tipc_send(unsigned int slot_id, int type, char*msgBuf, int len);

int sem_netlink_send(char *msgBuf, int len)
{
	memcpy(NLMSG_DATA(s_nlh), msgBuf, len);
	
	nl_msg_head_t *head = (nl_msg_head_t*)NLMSG_DATA(s_nlh);
	
    sem_syslog_dbg("\tNetlink sem pid(%d) send to module(%d)\n", head->pid, head->object);
	
	if(sendmsg(sock_s_fd, &s_msg, 0) < 0)
	{
        sem_syslog_dbg("Failed sem netlink send : %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int sem_netlink_send_to_rpa(char *msgBuf, int len)
{
	memcpy(NLMSG_DATA(rpa_nlh), msgBuf, len);
	
	nl_msg_head_t *head = (nl_msg_head_t*)NLMSG_DATA(rpa_nlh);

	sem_syslog_dbg("\tNetlink sem pid(%d) send to rpa(%d)\n", head->pid, head->object);
	
	if(sendmsg(sock_rpa_fd, &rpa_msg, 0) < 0) {
		sem_syslog_dbg("Failed sem netlink send to rpa : %s\n", strerror(errno));
		return -1;
	}

	return 0;
}


void product_state_notifier(int action_type)
{
    char chBuf[512];
	int i;
    nl_msg_head_t *head = (nl_msg_head_t*)chBuf;
    netlink_msg_t *nl_msg= (netlink_msg_t*)(chBuf + sizeof(nl_msg_head_t));			
    head->pid = getpid();		
    head->type = OVERALL_UNIT;
	head->object = COMMON_MODULE;
	head->count = 1;
	
	nl_msg->msgType = SYSTEM_STATE_NOTIFIER_EVENT;
    nl_msg->msgData.productInfo.action_type = action_type;
    nl_msg->msgData.productInfo.slotid = local_board->slot_id;

	sem_syslog_dbg("\tNetlink sem send prouct ready\n");
	
    int len = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
    sem_netlink_send(chBuf, len);
	
	for(i = 0; i < product->slotcount;i++)
	{
       if(i != product->active_master_slot_id)
       {
            sem_tipc_send(i, SEM_NETLINK_MSG, chBuf, len);
	   }
	}
	
	return;
}

void sem_eth_port_state_notifier(asic_port_update_t port_info)
{
	char msgbuf[512] = {0};
	int msgLen;
	int i;
    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg = (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));
	
	head->pid = getpid();
	head->count = 1;
    head->type = OVERALL_UNIT;
	head->object = NPD_MODULE;

    nl_msg->msgType = ASIC_ETHPORT_UPDATE_EVENT;
	nl_msg->msgData.portInfo.action_type = port_info.action_type;
	nl_msg->msgData.portInfo.devNum = port_info.devNum;
	nl_msg->msgData.portInfo.virportNum = port_info.virportNum;
	nl_msg->msgData.portInfo.trunkId = port_info.trunkId;
	nl_msg->msgData.portInfo.slot_id = port_info.slot_id;
	nl_msg->msgData.portInfo.port_no = port_info.port_no;

    msgLen = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
	sem_netlink_send(msgbuf, msgLen);

	sem_syslog_dbg("\tNetlink sem eth_port state\n");
	
	for(i = 0; i < product->slotcount;i++)
	{
       if(i != product->active_master_slot_id)
       {
            sem_tipc_send(i, SEM_NETLINK_MSG, msgbuf, msgLen);
	   }
	}
	
	return;
}

void sem_dynamic_trunk_notifier(asic_port_update_t port_info)
{
	char msgbuf[512] = {0};
	int msgLen;
	int i;
    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg = (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));
	
	head->pid = getpid();
	head->count = 1;
    head->type = OVERALL_UNIT;
	head->object = NPD_MODULE;

    nl_msg->msgType = ASIC_DYNAMIC_TRUNK_NOTIFIER_EVENT;
	nl_msg->msgData.portInfo.action_type = port_info.action_type;
	nl_msg->msgData.portInfo.devNum = port_info.devNum;
	nl_msg->msgData.portInfo.virportNum = port_info.virportNum;
	nl_msg->msgData.portInfo.trunkId = port_info.trunkId;
	nl_msg->msgData.portInfo.slot_id = port_info.slot_id;
	nl_msg->msgData.portInfo.port_no = port_info.port_no;

    msgLen = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
	sem_netlink_send(msgbuf, msgLen);

	sem_syslog_dbg("\tNetlink sem eth_port state\n");
	
	for(i = 0; i < product->slotcount;i++)
	{
       if(i != product->active_master_slot_id)
       {
            sem_tipc_send(i, SEM_NETLINK_MSG, msgbuf, msgLen);
	   }
	}
	
	return;
}



void sem_eth_port_state_send_to_active_mcb(asic_port_update_t port_info)
{
	char msgbuf[512] = {0};
	int msgLen;
	int i;
    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg = (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));
	
	head->pid = getpid();
	head->count = 1;
    head->type = OVERALL_UNIT;
	head->object = NPD_MODULE;

    nl_msg->msgType = ASIC_ETHPORT_UPDATE_EVENT;
	nl_msg->msgData.portInfo.action_type = port_info.action_type;
	nl_msg->msgData.portInfo.devNum = port_info.devNum;
	nl_msg->msgData.portInfo.virportNum = port_info.virportNum;
	nl_msg->msgData.portInfo.trunkId = port_info.trunkId;
	nl_msg->msgData.portInfo.slot_id = port_info.slot_id;
	nl_msg->msgData.portInfo.port_no = port_info.port_no;
	

    msgLen = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);

	sem_syslog_dbg("\tNetlink sem eth_port state\n");
	
	sem_tipc_send(product->active_master_slot_id, SEM_NETLINK_MSG_BROADCAST, msgbuf, msgLen);
	
	return;
}


void sem_dynamic_trunk_info_send_to_active_mcb(asic_port_update_t port_info)
{
	char msgbuf[512] = {0};
	int msgLen;
	int i;
    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg = (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));
	
	head->pid = getpid();
	head->count = 1;
    head->type = OVERALL_UNIT;
	head->object = NPD_MODULE;

    nl_msg->msgType = ASIC_DYNAMIC_TRUNK_NOTIFIER_EVENT;
	nl_msg->msgData.portInfo.action_type = port_info.action_type;
	nl_msg->msgData.portInfo.devNum = port_info.devNum;
	nl_msg->msgData.portInfo.virportNum = port_info.virportNum;
	nl_msg->msgData.portInfo.trunkId = port_info.trunkId;
	nl_msg->msgData.portInfo.slot_id = port_info.slot_id;
	nl_msg->msgData.portInfo.port_no = port_info.port_no;
	

    msgLen = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);

	sem_syslog_dbg("\tNetlink sem eth_port state\n");
	
	sem_tipc_send(product->active_master_slot_id, SEM_NETLINK_MSG_BROADCAST, msgbuf, msgLen);
	
	return;
}

int mcb_active_standby_switch_notifier(active_mcb_switch_type_t switch_type, int msg_type, 
	int active_master_slot_id, active_mcb_switch_netlink_broadcast_domin_t broadcast_domain)
{
	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;
	int msgLen = 0;
	char sendBuf[512] = {0};
	int i;

	if (msg_type != ACTIVE_STDBY_SWITCH_OCCUR_EVENT && msg_type != ACTIVE_STDBY_SWITCH_COMPLETE_EVENT) {
		sem_syslog_dbg("Unsupported msg type.\n");
		return 0;
	}

	head = (nl_msg_head_t*)sendBuf;
	nl_msg = (netlink_msg_t*)(sendBuf + sizeof(nl_msg_head_t));

	head->type = OVERALL_UNIT;
	head->object = COMMON_MODULE;
	head->count = 1;
	head->pid = getpid();

	nl_msg->msgType = msg_type;
	nl_msg->msgData.swInfo.action_type = switch_type;
	nl_msg->msgData.swInfo.active_slot_id = active_master_slot_id;

	msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t);

	/* if need broadcast on local board */
	if (LOCAL_BROADCAST == broadcast_domain) {
		sem_syslog_dbg("broadcast switch message on LOCAL board, current active mcb_slotid : %d\n", active_master_slot_id);
		sem_netlink_send(sendBuf, msgLen);
	}
	else if (OTHER_BOARD_BROADCAST == broadcast_domain) {
		sem_syslog_dbg("broadcast switch message on other boards, current active mcb_slotid : %d\n", active_master_slot_id);
		for(i = 0; i < product->slotcount; i++) {
			if(i != product->active_master_slot_id && (product->board_on_mask & (1 << i)))
				sem_tipc_send(i, SEM_NETLINK_MSG, sendBuf, msgLen);
		}
	}
	else if (ALL_BROADCAST == broadcast_domain) {
		sem_syslog_dbg("broadcast switch message on all boards on product, current active mcb_slotid : %d\n", active_master_slot_id);
		sem_netlink_send(sendBuf, msgLen);
		for(i = 0; i < product->slotcount; i++) {
			if(i != product->active_master_slot_id && (product->board_on_mask & (1 << i)))
				sem_tipc_send(i, SEM_NETLINK_MSG, sendBuf, msgLen);
		}
	}
	else {
		sem_syslog_dbg("mcb_active_standby_switch_notifier unknown broadcast domain\n");
	}

	return 0;
}

int base_mac_addr_notifier(void *addr)
{
	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;
	int msgLen = 0;
	char sendBuf[512] = {0};
	int i;

	head = (nl_msg_head_t*)sendBuf;
	nl_msg = (netlink_msg_t*)(sendBuf + sizeof(nl_msg_head_t));

	head->type = OVERALL_UNIT;
	head->object = COMMON_MODULE;
	head->count = 1;
	head->pid = getpid();

	nl_msg->msgType = BASE_MAC_ADDRESS_CHANGE_EVENT;
	memcpy(nl_msg->msgData.base_mac_addr, addr, 6);

	msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t);

	sem_syslog_dbg("Netlink send base mac address update.\n");

	sem_netlink_send_to_rpa(sendBuf, msgLen);

	return 0;
}

int sem_netlink_init(void)
{	
	/* Initialize data field */
	memset(&s_src_addr, 0, sizeof(s_src_addr));
	memset(&s_dest_addr, 0, sizeof(s_dest_addr));
	memset(&s_iov, 0, sizeof(s_iov));
	memset(&s_msg, 0, sizeof(s_msg));
	
	/* Create netlink socket use NETLINK_DISTRIBUTED(18) */
	if ((sock_s_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_DISTRIBUTED)) < 0) {
		sem_syslog_dbg("Failed socket : %s\n", strerror(errno));
		return -1;
	}

	/* Fill in src_addr */
	s_src_addr.nl_family = AF_NETLINK;
	s_src_addr.nl_pid = getpid();
	/* Focus */
	s_src_addr.nl_groups = 1;

	if (bind(sock_s_fd, (struct sockaddr*)&s_src_addr, sizeof(s_src_addr)) < 0) {
		sem_syslog_dbg("Failed bind : %s\n", strerror(errno));
		return -1;
	}

	/* Fill in dest_addr */
	s_dest_addr.nl_pid = 0;
	s_dest_addr.nl_family = AF_NETLINK;
	/* Focus */
	s_dest_addr.nl_groups = 1;

	/* Initialize buffer */
	if((s_nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD))) == NULL) {
		sem_syslog_dbg("Failed malloc\n");
		return -1;
	}

	memset(s_nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	s_nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	s_nlh->nlmsg_pid = getpid();
	s_nlh->nlmsg_flags = 0;
	s_iov.iov_base = (void *)s_nlh;
	s_iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);
	s_msg.msg_name = (void *)&s_dest_addr;
	s_msg.msg_namelen = sizeof(s_dest_addr);
	s_msg.msg_iov = &s_iov;
	s_msg.msg_iovlen = 1;

    return 0;
}

int sem_netlink_init_for_ksem(void)
{	
	/* Initialize data field */
	memset(&ks_src_addr, 0, sizeof(ks_src_addr));
	memset(&ks_dest_addr, 0, sizeof(ks_dest_addr));
	memset(&ks_iov, 0, sizeof(ks_iov));
	memset(&ks_msg, 0, sizeof(ks_msg));
	
	/* Create netlink socket use NETLINK_KSEM(19) */
	if ((sock_ks_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_KSEM)) < 0) {
		sem_syslog_dbg("Failed socket : %s\n", strerror(errno));
		return -1;
	}

	/* Fill in src_addr */
	ks_src_addr.nl_family = AF_NETLINK;
	ks_src_addr.nl_pid = getpid();
	/* Focus */
	ks_src_addr.nl_groups = 1;

	if (bind(sock_ks_fd, (struct sockaddr*)&ks_src_addr, sizeof(ks_src_addr)) < 0) {
		sem_syslog_dbg("Failed bind : %s\n", strerror(errno));
		return -1;
	}

	/* Fill in dest_addr */
	ks_dest_addr.nl_pid = 0;
	ks_dest_addr.nl_family = AF_NETLINK;
	/* Focus */
	ks_dest_addr.nl_groups = 1;

	/* Initialize buffer */
	if((ks_nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD))) == NULL) {
		sem_syslog_dbg("Failed malloc\n");
		return -1;
	}

	memset(ks_nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	ks_iov.iov_base = (void *)ks_nlh;
	ks_iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);
	ks_msg.msg_name = (void *)&ks_dest_addr;
	ks_msg.msg_namelen = sizeof(ks_dest_addr);
	ks_msg.msg_iov = &ks_iov;
	ks_msg.msg_iovlen = 1;

    return 0;
}

int sem_netlink_init_for_octeon(void)
{	
	/* Initialize data field */
	memset(&ko_src_addr, 0, sizeof(ko_src_addr));
	memset(&ko_dest_addr, 0, sizeof(ko_dest_addr));
	memset(&ko_iov, 0, sizeof(ko_iov));
	memset(&ko_msg, 0, sizeof(ko_msg));
	
	/* Create netlink socket use NETLINK_OCTEON(17) */
	if ((sock_ko_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_OCTEON)) < 0) {
		sem_syslog_dbg("Failed socket : %s\n", strerror(errno));
		return -1;
	}

	/* Fill in src_addr */
	ko_src_addr.nl_family = AF_NETLINK;
	ko_src_addr.nl_pid = getpid();
	/* Focus */
	ko_src_addr.nl_groups = 1;

	if (bind(sock_ko_fd, (struct sockaddr*)&ko_src_addr, sizeof(ko_src_addr)) < 0) {
		sem_syslog_dbg("Failed bind : %s\n", strerror(errno));
		return -1;
	}

	/* Fill in dest_addr */
	ko_dest_addr.nl_pid = 0;
	ko_dest_addr.nl_family = AF_NETLINK;
	/* Focus */
	ko_dest_addr.nl_groups = 1;

	/* Initialize buffer */
	if((ko_nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD))) == NULL) {
		sem_syslog_dbg("Failed malloc\n");
		return -1;
	}

	memset(ko_nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	ko_iov.iov_base = (void *)ko_nlh;
	ko_iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);
	ko_msg.msg_name = (void *)&ko_dest_addr;
	ko_msg.msg_namelen = sizeof(ko_dest_addr);
	ko_msg.msg_iov = &ko_iov;
	ko_msg.msg_iovlen = 1;

    return 0;
}

int sem_netlink_init_for_rpa(void)
{	
	/* Initialize data field */
	memset(&rpa_src_addr, 0, sizeof(rpa_src_addr));
	memset(&rpa_dest_addr, 0, sizeof(rpa_dest_addr));
	memset(&rpa_iov, 0, sizeof(rpa_iov));
	memset(&rpa_msg, 0, sizeof(rpa_msg));
	
	/* Create netlink socket use NETLINK_OCTEON(17) */
	if ((sock_rpa_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_RPA)) < 0) {
		sem_syslog_dbg("Failed socket : %s\n", strerror(errno));
		return -1;
	}

	/* Fill in src_addr */
	rpa_src_addr.nl_family = AF_NETLINK;
	rpa_src_addr.nl_pid = getpid();
	/* Focus */
	rpa_src_addr.nl_groups = 1;

	if (bind(sock_rpa_fd, (struct sockaddr*)&rpa_src_addr, sizeof(rpa_src_addr)) < 0) {
		sem_syslog_dbg("Failed bind : %s\n", strerror(errno));
		return -1;
	}

	/* Fill in dest_addr */
	rpa_dest_addr.nl_pid = 0;
	rpa_dest_addr.nl_family = AF_NETLINK;
	/* Focus */
	rpa_dest_addr.nl_groups = 1;

	/* Initialize buffer */
	if((rpa_nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD))) == NULL) {
		sem_syslog_dbg("Failed malloc\n");
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

void sem_netlink_recv_thread(void)
{
	char msgbuf[512] = {0};
	int retval = -1;
	int standby_master_slot_id;
	char *chTemp = NULL;
	fd_set read_fds;
	char file_path[64] = {0};
	int slot_id, eth_l_index, action_type;
	int i = 0, len = 0, maxfd = 0;
	asic_port_update_t port_info;

	struct timeval tnow;
	struct timezone tzone;	
	memset(&tnow, 0, sizeof(struct timeval));
	memset(&tzone, 0, sizeof(struct timezone));
	memset(&port_info,0,sizeof(asic_port_update_t));
    struct sysinfo info = {0};      /* for mobile test */

	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());

    if(sem_netlink_init() < 0)
    {
        sem_syslog_dbg("Fail sem_netlink_init\n");
	}  
    if(sem_netlink_init_for_octeon() < 0)
    {
        sem_syslog_dbg("Fail sem_netlink_init_for_octeon\n");
	}
	if(sem_netlink_init_for_ksem() < 0)
	{
        sem_syslog_dbg("Fail sem_netlink_init_for_ksem\n");
	}
	if (sem_netlink_init_for_rpa() < 0)
	{
		sem_syslog_dbg("Fail sem_netlink_init_for_rpa\n");
	}
	
    while( 1 ) 
	{  
        FD_ZERO(&read_fds);
		FD_SET(sock_s_fd, &read_fds);
		FD_SET(sock_ks_fd, &read_fds);
		FD_SET(sock_ko_fd, &read_fds);

		maxfd = sock_s_fd > sock_ks_fd ? sock_s_fd : sock_ks_fd;
		maxfd = maxfd > sock_ko_fd ? maxfd : sock_ko_fd;
		
		retval = select(maxfd+1, &read_fds, NULL, NULL, NULL);

		if(retval < 0)
		{
            sem_syslog_dbg("Fail select\n");
			return;
		}
		else if(retval == 0)
		{

		}
		else
		{
            if(FD_ISSET(sock_ko_fd, &read_fds))
            {
                if(recvmsg(sock_ko_fd, &ko_msg, 0) <= 0)
                {
                    sem_syslog_dbg("Failed sem netlink recv : %s\n", strerror(errno));
				}

                if(product->active_master_slot_id == product->master_slot_id[0])
            		standby_master_slot_id = product->master_slot_id[1];
            	else
            		standby_master_slot_id = product->master_slot_id[0];

        		nl_msg_head_t *head = (nl_msg_head_t*)NLMSG_DATA(ko_nlh);	
                sem_syslog_dbg("\tNetlink type(%d) from kernel\n", head->type);
        		kernel_nlmsg_t *nl_msg = (kernel_nlmsg_t*)(NLMSG_DATA(ko_nlh) + sizeof(nl_msg_head_t));
				
        		switch(head->type)
        		{
        			case OCTEON_ETHPORT_LINK_EVENT:       				
                        sem_syslog_dbg("\tReceive ipgport(%d) update event (Link %s) from kernel\n", 
        					nl_msg->ipgport, nl_msg->action_type == LINK_UP ? "UP" : "DOWN");
						
                    	struct eth_port_s port_info;
            			unsigned int slot_no = local_board->slot_id+1;
            			unsigned int local_port_no = nl_msg->ipgport + 1;
            			unsigned int eth_l_index = SLOT_PORT_COMBINATION(slot_no, local_port_no);
						
            			usleep(1000);
						
            			retval = sem_read_eth_port_info(slot_no, nl_msg->ipgport, &port_info);
            			if(retval < 0)
            			{
                            sem_syslog_dbg("Failed to read port%d-%d info \n", slot_no, local_port_no);
            			}
            			
            		    start_fp[local_board->slot_id][nl_msg->ipgport].attr_bitmap = port_info.attr_bitmap;
            		    start_fp[local_board->slot_id][nl_msg->ipgport].port_type = port_info.port_type;
                        /* add link change info */
                    	gettimeofday (&tnow, &tzone);
                		sysinfo(&info);     /* fro mobile test */
						
						start_fp[local_board->slot_id][nl_msg->ipgport].lastLinkChange = info.uptime; /* for mobile test */
						start_fp[local_board->slot_id][nl_msg->ipgport].linkchanged += 1;						
						
            			msync(start_fp[local_board->slot_id], sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM, MS_SYNC);

        				char chTmp[512] = {0};
                	    memcpy(chTmp, &start_fp[local_board->slot_id][nl_msg->ipgport], sizeof(global_ethport_t));
                				
                        if(local_board->is_active_master)
                        {
                		    sem_tipc_send(standby_master_slot_id, SEM_ETHPORT_INFO_SYN, chTmp, sizeof(global_ethport_t));				
                        }else {
                            sem_tipc_send(product->active_master_slot_id, SEM_ETHPORT_INFO_SYN, chTmp, sizeof(global_ethport_t));
                        }		
        				break;
        			default:
        				sem_syslog_dbg("Error:recv an error message type\n");
        				break;
        		}
            }
			else if(FD_ISSET(sock_ks_fd, &read_fds))
			{
                if(recvmsg(sock_ks_fd, &ks_msg, 0) <= 0)
                {
                    sem_syslog_dbg("Failed sem netlink recv : %s\n", strerror(errno));
				}
				
                if(product->active_master_slot_id == product->master_slot_id[0])
            		standby_master_slot_id = product->master_slot_id[1];
            	else
            		standby_master_slot_id = product->master_slot_id[0];

        		nl_msg_head_t *head = (nl_msg_head_t*)NLMSG_DATA(ks_nlh);
        		netlink_msg_t *nl_msg = (netlink_msg_t *)(NLMSG_DATA(ks_nlh) + sizeof(nl_msg_head_t));
                sem_syslog_dbg("\tNetlink type(%d) from kernel sem\n", head->type);
        		
        		switch(nl_msg->msgType)
        		{
        		#if 1  //use "ACTIVE_STDBY_SWITCH_OCCUR_EVENT" again,yjl modified 2014-2-12 
        			case ACTIVE_STDBY_SWITCH_OCCUR_EVENT:
        				chTemp = NLMSG_DATA(ks_nlh);	
                    	sem_syslog_dbg("\tNetlink Sem MCB Active Standby Switch\n");
						sem_syslog_dbg("\tSlot ID : %d\n", nl_msg->msgData.swInfo.active_slot_id);
                    	
                        int len = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
						mcb_switch_arg_t *mcb_switch_arg;
						mcb_switch_arg = (mcb_switch_arg_t *)malloc(sizeof(mcb_switch_arg_t));
						memset(mcb_switch_arg, 0, sizeof(mcb_switch_arg_t));
						mcb_switch_arg->switch_type = HOTPLUG_MODE;
						mcb_switch_arg->broadcast_domain = OTHER_BOARD_BROADCAST;
						
                    	/* run standby to active switch function, support product 8610 temporarily, caojia added*/
						if (local_board->is_master && (product->product_type == XXX_YYY_AX8610 || \
							product->product_type == XXX_YYY_AX7605I || \
							product->product_type == XXX_YYY_AX8606))
						{
							pthread_mutex_lock(&switch_lock);
							/* set switch flag to disable repetitive standby to active switch in keep alive thread, caojia added*/
							if (switch_flag == 1)
							{
								pthread_mutex_unlock(&switch_lock);
								continue;
							}
							else
							{
								switch_flag = 1;
							}
							pthread_mutex_unlock(&switch_lock);
							
							retval = product->active_stby_switch((void *)mcb_switch_arg);
						}
						else
						{
							sem_syslog_dbg("\tNot meet the conditions for active_MCB and standby_MCB switch\n");
						}
						
        				break;
				#endif
        			case BOARD_STATE_NOTIFIER_EVENT:
                        		if(nl_msg->msgData.boardInfo.action_type == BOARD_INSERTED)
        					sem_syslog_dbg("\tBoard Inserting Slot ID %d\n", nl_msg->msgData.boardInfo.slotid);
        				else if(nl_msg->msgData.boardInfo.action_type == BOARD_REMOVED) {
							sem_syslog_dbg("\tBoard Removing Slot ID %d\n", nl_msg->msgData.boardInfo.slotid);
							
					        #if 0 //here is no use, use "ACTIVE_STDBY_SWITCH_OCCUR_EVENT" again.yjl modified 2014-2-12 
							sem_syslog_dbg("\tActive MCB : %d\n", product->active_master_slot_id);
							if (((nl_msg->msgData.boardInfo.slotid - 1) == product->active_master_slot_id) && \
								local_board->is_master) {
								/* run standby to active switch function, support product 8610 temporarily, caojia added*/
								if (!local_board->is_active_master && (product->product_type == XXX_YYY_AX8610 || \
									product->product_type == XXX_YYY_AX8606 || \
									product->product_type == XXX_YYY_AX8800 || \
									product->product_type == XXX_YYY_AX8603)) {
									pthread_mutex_lock(&switch_lock);
									/* set switch flag to disable repetitive standby to active switch in keep alive thread, caojia added*/
									if (switch_flag == 1)
									{
										pthread_mutex_unlock(&switch_lock);
										continue;
									}
									else
									{
										switch_flag = 1;
									}
									pthread_mutex_unlock(&switch_lock);

										mcb_switch_arg_t mcb_switch_arg;
										mcb_switch_arg.switch_type = HOTPLUG_MODE;
										mcb_switch_arg.broadcast_domain = ALL_BROADCAST;
										
										retval = product->active_stby_switch((void *)(&mcb_switch_arg));
								}
							}
							#endif
							//else { 
								sem_syslog_warning("SEM:slot %d is removed\n", nl_msg->msgData.boardInfo.slotid);
								if (local_board->is_active_master)
									set_dbm_effective_flag(INVALID_DBM_FLAG);
									product->board_removed(nl_msg->msgData.boardInfo.slotid - 1);
									set_dbm_effective_flag(VALID_DBM_FLAG);
							//}
						}
        				else
        					sem_syslog_dbg("Error board state notifier\n");
        				break;
					case POWER_STATE_NOTIFIER_EVENT:
						{
    						unsigned int power_state, power_index;
    						power_state = nl_msg->msgData.powerInfo.action_type;
    						power_index = nl_msg->msgData.powerInfo.power_id;
    						if(nl_msg->msgData.powerInfo.action_type == POWER_ON) {
    							sem_syslog_dbg("\tPower Module [%d] On\n", power_index);
								sem_dbus_power_state_abnormal(power_state, power_index);
    						}else if(nl_msg->msgData.powerInfo.action_type == POWER_OFF) {
    							sem_syslog_dbg("\tPower Module [%d] Off\n", power_index);
    							sem_dbus_power_state_abnormal(power_state, power_index);
							}else if(nl_msg->msgData.powerInfo.action_type == POWER_INSERTED) {
    							sem_syslog_dbg("\tPower Module [%d] Inserted\n", power_index);
    						}else if(nl_msg->msgData.powerInfo.action_type == POWER_REMOVED) {
    							sem_syslog_dbg("\tPower Module [%d] Removed\n", power_index);		
    						}else if(nl_msg->msgData.powerInfo.action_type == VCC_ALARM) {
    						    sem_syslog_dbg("\tVCC Alarm\n");
    						}else
    							sem_syslog_dbg("Error power state notifier\n");
    						break;
					    }
					case FAN_STATE_NOTIFIER_EVENT:
					    if(nl_msg->msgData.fanInfo.action_type == FAN_INSERTED)
							sem_syslog_dbg("\tFan Inserted\n");
						else if(nl_msg->msgData.fanInfo.action_type == FAN_REMOVED) {
							sem_syslog_dbg("\tFan Removed\n");
						}else if(nl_msg->msgData.fanInfo.action_type == FAN_ALARM) {
        		            sem_syslog_dbg("\tFan Alarm\n");
						}else
							sem_syslog_dbg("Error fan state notifier\n");
						break;	
        			default:
        				sem_syslog_dbg("Error:recv an error message type\n");
        				break;                
        		}
			}
			else if(FD_ISSET(sock_s_fd, &read_fds))
			{                    				
                if(recvmsg(sock_s_fd, &s_msg, 0) <= 0)
                {
                    sem_syslog_dbg("Failed sem netlink recv : %s\n", strerror(errno));
				}
				
        		if(product->active_master_slot_id == product->master_slot_id[0])
            		standby_master_slot_id = product->master_slot_id[1];
            	else
            		standby_master_slot_id = product->master_slot_id[0];
        		
        		nl_msg_head_t *head = (nl_msg_head_t*)NLMSG_DATA(s_nlh);         
                sem_syslog_dbg("\tSem netlink module(%d) recvfrom pid(%d)\n", head->object, head->pid);
        		
                chTemp = NLMSG_DATA(s_nlh) + sizeof(nl_msg_head_t);  
                len = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
        		
        		if(head->type == OVERALL_UNIT && \
        			local_board->slot_id == product->active_master_slot_id)
        		{
                	for(i = 0; i < product->slotcount;i++)
                	{
                       if(i != product->active_master_slot_id)
                       {
                            sem_tipc_send(i, SEM_NETLINK_MSG, NLMSG_DATA(s_nlh), len);
                	   }
                	}			
        		}
        		
        	    if(head->object == SEM_MODULE || head->object == COMMON_MODULE)
        		{
                    for(i = 0; i < head->count; i++)
                    {
            			if(chTemp)
            			{
                			netlink_msg_t *nl_msg= (netlink_msg_t*)chTemp;	
                			sem_syslog_dbg("\tSem netlink msgType(%d)\n", nl_msg->msgType);	
                			switch(nl_msg->msgType)
                			{
                				case ASIC_ETHPORT_UPDATE_EVENT:
                                    slot_id = nl_msg->msgData.portInfo.slot_id;
                					eth_l_index = nl_msg->msgData.portInfo.eth_l_index;
									port_info.action_type = nl_msg->msgData.portInfo.action_type;
									port_info.devNum = nl_msg->msgData.portInfo.devNum;
									port_info.virportNum = nl_msg->msgData.portInfo.virportNum;
									port_info.trunkId = nl_msg->msgData.portInfo.trunkId;
									port_info.slot_id = nl_msg->msgData.portInfo.slot_id;
									port_info.port_no = nl_msg->msgData.portInfo.port_no;

                    				sem_syslog_dbg("\tReceive asic port(%d-%d) update event from npd\n", \
                    					slot_id, eth_l_index+1);

                                    char chTmp[512] = {0};
                            	    memcpy(chTmp, &start_fp[local_board->slot_id][eth_l_index], sizeof(global_ethport_t));
                            				
                                    if(local_board->is_active_master)
                                    {
                            		    sem_tipc_send(standby_master_slot_id, SEM_ETHPORT_INFO_SYN, chTmp, sizeof(global_ethport_t));				
                                    }else {
                                        sem_tipc_send(product->active_master_slot_id, SEM_ETHPORT_INFO_SYN, chTmp, sizeof(global_ethport_t));
                                    }	
									if((product->is_distributed) && (port_info.trunkId != 0))/* zhangcl added for setting trunk map table*/
									{
										if (local_board->is_active_master) {
											sem_eth_port_state_notifier(port_info);
										}
										else {
											sem_eth_port_state_send_to_active_mcb(port_info);
										}
									}
                					break;
								case ASIC_DYNAMIC_TRUNK_NOTIFIER_EVENT:
                                    slot_id = nl_msg->msgData.portInfo.slot_id;
                					eth_l_index = nl_msg->msgData.portInfo.eth_l_index;
									port_info.action_type = nl_msg->msgData.portInfo.action_type;
									port_info.devNum = nl_msg->msgData.portInfo.devNum;
									port_info.virportNum = nl_msg->msgData.portInfo.virportNum;
									port_info.trunkId = nl_msg->msgData.portInfo.trunkId;
									port_info.slot_id = nl_msg->msgData.portInfo.slot_id;
									port_info.port_no = nl_msg->msgData.portInfo.port_no;

                    				sem_syslog_dbg("\tReceive dynamic trunk event from npd\n", \
                    					slot_id, eth_l_index+1);

          	
									if((product->is_distributed) && (port_info.trunkId != 0))/* zhangcl added for dynamic trunk*/
									{
										if (local_board->is_active_master) {
											sem_dynamic_trunk_notifier(port_info);
										}
										else {
											sem_dynamic_trunk_info_send_to_active_mcb(port_info);
										}
									}
                					break;
                                case ASIC_VLAN_SYNC_ENVET:
        							action_type = nl_msg->msgData.vlanInfo.action_type;
                                    slot_id = nl_msg->msgData.vlanInfo.slot_id;
        							strcpy(file_path, "/dbm/shm/vlan/shm_vlan");
                                    sem_syslog_dbg("\tActive_MCB send shm_vlan file to:\n");
                                	for(i = 0; (i < product->slotcount) && 
                                		       (thread_id_arr[i].sd >= 0) && 
                                			   ((thread_id_arr[i].slot_id) != (local_board->slot_id)) &&
                                			   (thread_id_arr[i].slot_id != USELESS_SLOT_ID); i++) 
                                    {
        								sem_syslog_dbg("\tthread_id_arr[%d].slot_id = %d, sd = %d\n", i, thread_id_arr[i].slot_id, thread_id_arr[i].sd);
                                        sem_syslog_dbg("\taction_type %d\n", action_type);
        								
                                		if((thread_id_arr[i].slot_id == standby_master_slot_id) && (action_type == 0))
                                		{
                                			sem_syslog_dbg("\tActive_MCB send shm_vlan file to standby_MCB\n");
                                			product->sync_send_file(thread_id_arr[i].sd, file_path, 0);
                            			}
        								else if((thread_id_arr[i].slot_id == slot_id-1) && (action_type == 1))
        								{
                                            sem_syslog_dbg("\tActive_MCB send shm_vlan file to slot%d\n", slot_id);
        									product->sync_send_file(thread_id_arr[i].sd, file_path, 0);
        								}
                            		}								
                					break;
								case ASIC_VLAN_NOTIFIER_ENVET:
                                    if(local_board->is_active_master)
                                    {
                            		    sem_tipc_send(nl_msg->msgData.vlanInfo.slot_id-1, SEM_NETLINK_MSG, NLMSG_DATA(s_nlh), len);			
                                    }	
									break;
                				default:
                					sem_syslog_dbg("Error:sem recv an error message type\n");
                					break;
                			}
                			chTemp = chTemp + sizeof(netlink_msg_t);
            			}
                    }
        		}	
			}
		}
    }
}

#if 0

void netlink_destroy()
{
	memset(&msg, 0, sizeof(msg));
	memset(&src_addr, 0, sizeof(src_addr));
	memset(&dest_addr, 0, sizeof(dest_addr));
	close(nl_sock);
}
#endif


#ifdef __cplusplus
}
#endif
