#ifdef __cplusplus
extern "C"
{
#endif


#include<stdio.h>
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
#include <unistd.h>
#include <pthread.h>
#include <linux/tipc.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>

#define NPD_COMM_TYPE_SLOT_ZERO_LOW       1200
#define NPD_COMM_TYPE_SLOT_ZERO_UP        1200
#define NPD_COMM_TYPE_SLOT_ZERO_INSTANCE  1200

#define NPD_COMM_TYPE_SLOT_ONE_LOW        1250
#define NPD_COMM_TYPE_SLOT_ONE_UP         1250
#define NPD_COMM_TYPE_SLOT_ONE_INSTANCE   1250

#define NPD_COMM_TYPE_SLOT_TWO_LOW        1300
#define NPD_COMM_TYPE_SLOT_TWO_UP         1300
#define NPD_COMM_TYPE_SLOT_TWO_INSTANCE   1300

#define NPD_TIPC_RECV_BUF_LEN   512

#define NPD_SERVER_STAT 100
#define NPD_CLIETN_STAT 200

int gServerTypeLow;
int gServerTypeUP;
int gClientInstance;
int gNpdStat;


typedef struct
{
    int slot;
    int opcode;
}npd_cmd_t;
#define RECV_LEN 1000
int npd_send(char *send_buf, int send_buf_len, char *recv_buf, int *recv_buf_len);

main()
{
    char send_buf[RECV_LEN] = {0};
    char recv_buf[RECV_LEN] = {0};
    int send_len = 0;
    int recv_len = RECV_LEN;
	
	printf("send_buf:%s\n",send_buf);  
	npd_cmd_t *cmd = (npd_cmd_t*)send_buf; 
	send_len += sizeof(npd_cmd_t);
	printf("send_len: %d \n", send_len);
	int i = 0;
	cmd->slot = 0;
	while(1)
	{		
		printf("cmd->slot: %d \n", cmd->slot);
		cmd->opcode = i;
		printf("cmd->opcode: %d \n", cmd->opcode);
		if(i < 3)
		{
			i++;
		}
		else
		{
			i = 0;
		}
		recv_len = RECV_LEN;
        npd_send(send_buf, send_len, recv_buf, &recv_len);
        printf("recv message len: %d \n", recv_len);
        printf("recv message : %s \n", recv_buf);
	    sleep(2);
	}
}



int npd_send(char *send_buf, int send_buf_len, char *recv_buf, int *recv_buf_len)
{
    printf("	file: %s, func: %s , line: %d\n", __FILE__, __FUNCTION__,__LINE__);
    int client_sock = -1;
    struct sockaddr_tipc client_sockaddr;
    client_sock = socket (AF_TIPC, SOCK_STREAM, 0);
    if (client_sock < 0)
    {   printf("	file: %s, func: %s , line: %d\n", __FILE__, __FUNCTION__,__LINE__);
        return ;
    }

    npd_cmd_t *cmd = (npd_cmd_t*)send_buf;

    switch(cmd->slot)
    {    
           case 0 :printf("	file: %s, func: %s , line: %d\n", __FILE__, __FUNCTION__,__LINE__);
                   gClientInstance= NPD_COMM_TYPE_SLOT_ZERO_INSTANCE;
                   break;
           case 1 :printf("	file: %s, func: %s , line: %d\n", __FILE__, __FUNCTION__,__LINE__);
                   gClientInstance = NPD_COMM_TYPE_SLOT_ONE_INSTANCE;
                   break;
           case 2 :printf("	file: %s, func: %s , line: %d\n", __FILE__, __FUNCTION__,__LINE__);
                   gClientInstance = NPD_COMM_TYPE_SLOT_TWO_INSTANCE;
                   break;
           default:
                   break;
        }

    if (tipc_set_client_addr(&client_sockaddr, gClientInstance))
    {   printf("	file: %s, func: %s , line: %d\n", __FILE__, __FUNCTION__,__LINE__);
        return ;
    }

    if(!tipc_client_connect_to_server(client_sock, client_sockaddr))
    {
	  printf("	file: %s, func: %s , line: %d\n", __FILE__, __FUNCTION__,__LINE__);
      if (!tipc_send(client_sock, send_buf, send_buf_len))
      {                
          if(tipc_recv(client_sock, recv_buf, recv_buf_len))
          {
			 printf("tipc_recv failed file: %s, func: %s , line: %d\n", __FILE__, __FUNCTION__,__LINE__);
          }
          close(client_sock);
          return 0;
      }
      else
      {
          printf("	send failed, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
          return -1;
      }
    }
    else
    {
          printf("	send failed\n, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
          return -1;
    }
    printf("	file: %s, func: %s , line: %d\n", __FILE__, __FUNCTION__,__LINE__);
    close(client_sock);
    return 0;

}


#ifdef __cplusplus
}
#endif
