#ifdef __cplusplus
extern "C"
{
#endif


#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/tipc.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>

#include "../sem_common.h"
#include "sem/sem_tipc.h"

#include "../product/product_feature.h"
#include "../product/board/board_feature.h"



extern board_fix_param_t *local_board;
extern product_fix_param_t *product;
extern int active_mcb_state;


int wait_for_server(struct tipc_name* name,int wait)
{
        struct sockaddr_tipc topsrv;
        struct tipc_subscr subscr = {{name->type,name->instance,name->instance},
                                     wait,TIPC_SUB_SERVICE,{}};
        struct tipc_event event;

        int sd = socket (AF_TIPC, SOCK_SEQPACKET,0);
		if (sd < 0)
		{
			sem_syslog_warning("socket create failed:sd = %d\n", sd);
			return 1;	
		}

        memset(&topsrv,0,sizeof(topsrv));
	  	topsrv.family = AF_TIPC;
        topsrv.addrtype = TIPC_ADDR_NAME;
        topsrv.addr.name.name.type = TIPC_TOP_SRV;
        topsrv.addr.name.name.instance = TIPC_TOP_SRV;

        /* Connect to topology server: */

        if (0 > connect(sd,(struct sockaddr*)&topsrv,sizeof(topsrv))){
                sem_syslog_dbg("			Failed to connect to topology server\n");
				close(sd);
				return 1;
        }
        if (send(sd,&subscr,sizeof(subscr),0) != sizeof(subscr)){
                sem_syslog_dbg("			Failed to send subscription\n");
				close(sd);
                return 1;
        }
        /* Now wait for the subscription to fire: */
        if (recv(sd,&event,sizeof(event),0) != sizeof(event)){
                sem_syslog_dbg("			Failed to receive event\n");
				close(sd);
               	return 1;
        }
        if (event.event != TIPC_PUBLISHED){
                sem_syslog_dbg("			Server %u,%u not published within %u [s]\n",
                       name->type,name->instance,wait/1000);
				close(sd);
                return 1;
        }
        close(sd);
		return 0;
}


/************************************************************
function:  int tipc_SetServerAddr(struct sockaddr_tipc *sockaddr, __u32 lower, __u32 upper)
describ:   setting option of tipc address
in:        unsigned char addrtype     :
	       __u32 lower                : this a  limit(lower ~ upper) that Support server
	       __u32 upper                :

out:       struct sockaddr_tipc *sockaddr : whitch tipc address that somebody want set
           return 0 : setting success
date:      2011-02-26
modify:    2011-03-07
           change tipc_setaddr to  tipc_SetServerAddr <zhangdx@autelan.com>
*************************************************************/
int tipc_set_recv_addr(struct sockaddr_tipc *sockaddr, __u32 lower, __u32 upper)
{	
    if(sockaddr == NULL)
    {
      return 1;
    }
	sockaddr->family = AF_TIPC;
    sockaddr->addrtype = TIPC_ADDR_NAMESEQ;
	sockaddr->addr.nameseq.type = TIPC_SEM_TYPE_LOW;
    sockaddr->addr.nameseq.lower = lower;
    sockaddr->addr.nameseq.upper = upper;
	sockaddr->scope = TIPC_CLUSTER_SCOPE;
    return 0;
}

int tipc_set_send_addr(struct sockaddr_tipc *sockaddr, __u32 instance)
{
	if (sockaddr == NULL)
	{
		return 1;	
	}
	
	sockaddr->family = AF_TIPC;
	sockaddr->addrtype = TIPC_ADDR_NAME;
	sockaddr->addr.name.name.type = TIPC_SEM_TYPE_LOW;
	sockaddr->addr.name.name.instance = instance;
	sockaddr->addr.name.domain = 0;
	return 0;
}
/************************************************************
function:  int tipc_SetClientAddr(struct sockaddr_tipc *sockaddr,  __u32 instance)
describ:   setting option of tipc address
in:        unsigned char addrtype     :
	       __u32 instance                : this a  limit(lower ~ upper) that Support server             :
out:       struct sockaddr_tipc *sockaddr : whitch tipc address that somebody want set
           return 0 : setting success
date:      2011-02-26
modify:    2011-03-07
           change tipc_setaddr to  tipc_SetClientAddr <zhangdx@autelan.com>
*************************************************************/
int tipc_set_client_addr(struct sockaddr_tipc *sockaddr,  __u32 instance)
{	
  if(sockaddr == NULL)
  {
  	return 1;
  }

  sockaddr->family = AF_TIPC;
  sockaddr->addrtype = TIPC_ADDR_NAME;
  sockaddr->addr.name.name.type = TIPC_SEM_TYPE_LOW;
  sockaddr->addr.name.name.instance = instance;
  sockaddr->addr.name.domain = 0;
  return 0;
}

/*
 *
 *return :0 for success;1 for failed
 */
int tipc_client_connect_to_server(int sd, struct sockaddr_tipc sockaddr)
{
    if(0 != wait_for_server(&sockaddr.addr.name.name,10))
	{
		sem_syslog_dbg("		failed wait_for_server \n");
		return 1;
	}

    if (connect(sd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) != 0)
    {
        sem_syslog_dbg("		Connect failed");
        return 1;
    }
    return 0;
}

int tipc_send(int sd, char *chBuf, int len)
{	
	int ret;

	//sem_syslog_dbg("ready to send\n");
	ret = send(sd, chBuf, len, 0);
	//sem_syslog_dbg("send ret is %d\n", ret);
	if ( ret <= 0 )
	{
       //sem_syslog_dbg("Server : Failed to send response\n");
       return 1;
    }

    return 0;
}

int tipc_recv(int sd, char *chBuf, int *buf_len)
{
	//sem_syslog_dbg("recv before:\n");
	int sz = recv(sd, chBuf, *buf_len, 0);
	//sem_syslog_dbg("recv done.\n");
	if (sz <= 0)
	{
		return 1;
	}

	*buf_len = sz;
	
	return 0;
}


/*
 *
 *return val:0 link on;1 link failed;-1 error
 */
int active_link_test(int test_count)
{	
	int ret ;
	char *link_request_pdu, *link_echo_pdu;
	int len;
	int flag = 0;
	int temp_test_count;
	fd_set rdfds;
	unsigned int time_out = 5;
	sem_pdu_head_t * head;
	sem_tlv_t *tlv_head;
	
	board_info_syn_t *board_info_head;
	struct timeval wait_time;
	wait_time.tv_sec = 0;
	wait_time.tv_usec = 1000000;
	int temp_sd;
	
    char send_buf[SEM_TIPC_SEND_BUF_LEN] = {0};
	char recv_buf[SEM_TIPC_RECV_BUF_LEN];

	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = local_board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_CONNECT_REQUEST;
	tlv_head->length = 33;

	int addr_len;

	while (test_count)
	{
		if (sendto(g_send_sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_link_test_t), 0, (struct sockaddr*)&g_send_sock_addr, sizeof(struct sockaddr_tipc)) < 0)
		{
			sem_syslog_dbg("	link request:send failed\n");
			continue;
		}
		else
		{
			sem_syslog_dbg("	link request:send succeed\n");
			while (time_out)
			{
				FD_ZERO(&rdfds);
				FD_SET(g_recv_sd, &rdfds);

				ret = select((g_recv_sd)+1, &rdfds, NULL, NULL, &wait_time);

				if (ret < 0)
				{
				}
				else if (ret == 0)
				{
				}
				else
				{
					if (recvfrom(g_recv_sd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&g_recv_sock_addr, &addr_len) <= 0)
					{
						sem_syslog_warning("recv wrong mesg\n");
						break;
					}
					else
					{
						head = (sem_pdu_head_t *)recv_buf;
						tlv_head = (sem_tlv_t *)(recv_buf + sizeof(sem_pdu_head_t));

						switch (tlv_head->type)
						{
							case SEM_CONNECT_CONFIRM:
								sem_syslog_dbg("recv connnect confirm msg\n");
								flag = 1;
								break;
							default :
								sem_syslog_dbg("recv message type %d\n", tlv_head->type);
								flag = 0;
								break;
						}
					}
				}

				FD_CLR(g_recv_sd, &rdfds);
				
				if (flag)
					break;
				
				time_out--;
				//sleep(1);
			}
		}

		if (flag)
		{
			break;	
		}
		test_count--;
	}
	
	if (test_count == 0)
	{
		sem_syslog_warning("	master is not exist\n");
		return 1;
	}
	else
	{
		active_mcb_state = IS_ALIVING;
		return 0;
	}
}

#ifdef __cplusplus
}
#endif
