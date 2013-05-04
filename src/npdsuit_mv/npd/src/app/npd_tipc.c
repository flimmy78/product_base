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

#include "npd_tipc.h"

#if 1

int wait_for_server(struct tipc_name* name,int wait)
{
        struct sockaddr_tipc topsrv;
        struct tipc_subscr subscr = {{name->type,name->instance,name->instance},
                                     wait,TIPC_SUB_SERVICE,{}};
        struct tipc_event event;

        int sd = socket (AF_TIPC, SOCK_SEQPACKET,0);
        assert(sd > 0);

        memset(&topsrv,0,sizeof(topsrv));
	  	topsrv.family = AF_TIPC;
        topsrv.addrtype = TIPC_ADDR_NAME;
        topsrv.addr.name.name.type = TIPC_TOP_SRV;
        topsrv.addr.name.name.instance = TIPC_TOP_SRV;

        /* Connect to topology server: */

        if (0 > connect(sd,(struct sockaddr*)&topsrv,sizeof(topsrv))){
                printf("			Failed to connect to topology server, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
				close(sd);
                return 1;
        }
        if (send(sd,&subscr,sizeof(subscr),0) != sizeof(subscr)){
                printf("			Failed to send subscription, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
				close(sd);
                return 1;
        }
        /* Now wait for the subscription to fire: */
        if (recv(sd,&event,sizeof(event),0) != sizeof(event)){
                printf("			Failed to receive event, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
				close(sd);
            	return 1;
        }
        if (event.event != TIPC_PUBLISHED){
                printf("			Server %u,%u not published within %u [s]\n",
                       name->type,name->instance,wait/1000);
				close(sd);      /* code optimize: Resource "sd" is not freed. zhangdi@autelan.com 2013-01-18 */
                return 1;
        }
        close(sd);
		return 0;
}


/****************************************************************************************
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
*****************************************************************************************/
int tipc_set_server_addr(struct sockaddr_tipc *sockaddr, __u32 lower, __u32 upper)
{	
    if(sockaddr == NULL)
    {
      return 1;
    }
	sockaddr->family = AF_TIPC;
    sockaddr->addrtype = TIPC_ADDR_NAMESEQ;
        sockaddr->addr.nameseq.type = TIPC_NPD_TYPE;
    sockaddr->addr.nameseq.lower = lower;
    sockaddr->addr.nameseq.upper = upper;
	sockaddr->scope = TIPC_CLUSTER_SCOPE;
    return 0;
}

/****************************************************************************************
function:  int tipc_SetClientAddr(struct sockaddr_tipc *sockaddr,  __u32 instance)
describ:   setting option of tipc address
in:        unsigned char addrtype     :
	       __u32 instance                : this a  limit(lower ~ upper) that Support server             :
out:       struct sockaddr_tipc *sockaddr : whitch tipc address that somebody want set
           return 0 : setting success
date:      2011-02-26
modify:    2011-03-07
           change tipc_setaddr to  tipc_SetClientAddr <zhangdx@autelan.com>
*****************************************************************************************/
int tipc_set_client_addr(struct sockaddr_tipc *sockaddr,  __u32 instance)
{	
  if(sockaddr == NULL)
  {
  	return 1;
  }

  sockaddr->family = AF_TIPC;
  sockaddr->addrtype = TIPC_ADDR_NAME;
  sockaddr->addr.name.name.type = TIPC_NPD_TYPE;
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
    if(0 != wait_for_server(&sockaddr.addr.name.name,1000))
	{
                printf("		failed wait_for_server , file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
		return 1;
	}

    if (connect(sd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) != 0)
    {
        printf("		Connect failed, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
        return 1;
    }
    return 0;
}

int tipc_send(int sd, char *chBuf, int len)
{	
	int ret;
	ret = send(sd, chBuf, len, 0);
	if ( ret <= 0 )
	{
           printf("Server : Failed to send response, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
           return 1;
        }

    return 0;
}

int tipc_recv(int sd, char *chBuf, int *buf_len)
{
	int sz = recv(sd, chBuf, buf_len, 0);
	if (sz <= 0)
	{
		return 1;
	}

	*buf_len = sz;
	
	return 0;
}

#endif

#ifdef __cplusplus
}
#endif
