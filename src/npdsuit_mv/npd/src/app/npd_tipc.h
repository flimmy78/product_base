#ifndef __NPD_TIPC_H_
#define __NPD_TIPC_H_

#define TIPC_NPD_TYPE	100
#define __u32 	unsigned int
#if 1
extern int tipc_set_server_addr(struct sockaddr_tipc *sockaddr, __u32 lower, __u32 upper);
extern int tipc_set_client_addr(struct sockaddr_tipc *sockaddr,  __u32 instance);

extern int tipc_client_connect_to_server(int sd, struct sockaddr_tipc sockaddr);
extern int tipc_send(int sd, char *chBuf, int len);
extern int tipc_recv(int sd, char *chBuf, int *buf_len);
#endif
#endif
