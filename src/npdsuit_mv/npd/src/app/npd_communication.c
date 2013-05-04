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

#include <unistd.h>
#include <pthread.h>
#include <linux/tipc.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
/* for g_eth_ports */
#if 1
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "util/npd_list.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd/nam/npd_amapi.h"
#include "nam/nam_eth_api.h"
#include "dbus/npd/npd_dbus_def.h"
#include "cvm/ethernet-ioctl.h"
#include "npd_product.h"
#include "npd_log.h"
#include "npd_c_slot.h"
#include "npd_eth_port.h"
extern struct eth_port_s **g_eth_ports;
#endif
#include "npd_tipc.h"
#include "npd_communication.h"

#define MAX_CONNECT_CLIENT 512
thread_index_sd_t thread_id_arr[MAX_CONNECT_CLIENT];

    int npd_thread_create(pthread_t *new_thread, void * (*thread_fun)(void *), void *arg, int is_detached)
    {
            pthread_attr_t attr;
            int s ;

            if (is_detached)
            {
                    s = PTHREAD_CREATE_DETACHED;
            }
            else
            {
                    s = PTHREAD_CREATE_JOINABLE;
            }

            if (!new_thread)
            {
                    return -1;
            }
            pthread_attr_init(&attr);

            pthread_attr_setdetachstate(&attr, s);

            if (pthread_create(new_thread, &attr, thread_fun, arg) != 0)
            {
                    return -1;
            } 
            return 0;
    }

    int find_free_index(void)
    {
		    /*printf("	int find_free_index() begin\n");*/
			int iCount;
            for (iCount=0; iCount<MAX_CONNECT_CLIENT; iCount++)
            {
                    if (thread_id_arr[iCount].thread_index == -1)
                    {
                        /*printf("	int find_free_index() return %d;\n",iCount);*/
                            return iCount;
                    }
            }
            /*printf("	int find_free_index() return -1;\n");*/
            return -1;
    }

    void npd_thread_recv(void *arg)
    {
            if (!arg)
            {
                printf("	arg is null, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
                return;
            }

            thread_index_sd_t *temp_index_sd;
            char recv_buf[NPD_TIPC_RECV_BUF_LEN];
            int recv_len = NPD_TIPC_RECV_BUF_LEN;

            temp_index_sd = (thread_index_sd_t *)arg; 

			struct eth_port_s ** temp = NULL;
			struct eth_port_s port[30]; 
            while(1)
            {
                int recv_len = NPD_TIPC_RECV_BUF_LEN;
                if (tipc_recv(temp_index_sd->sd, recv_buf, &recv_len) == 0)
                {
					/*
                    printf("recved a message recv_len = %d\n", recv_len);
                    printf("message : %s\n", recv_buf);
					*/
					
					npd_cmd_t *cmd = (npd_cmd_t*)recv_buf;
           			int i=0;
        			int g_port_index= 0;
        			int port_count=0;
                 
					switch(cmd->opcode)
					{
						case 0:     /* to display eth-port list */
						{
							npd_get_eth_ports_status();	

							port_count = ETH_LOCAL_PORT_COUNT(LOCAL_CHASSIS_SLOT_NO);
							
							int send_len=(sizeof(struct eth_port_s))*port_count;
							g_port_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(cmd->slot,i);
														
							for(i=0;i<port_count;i++)
							{
								port[i].attr_bitmap=g_eth_ports[g_port_index]->attr_bitmap;
    							port[i].mtu=g_eth_ports[g_port_index]->mtu;
    							port[i].counters=g_eth_ports[g_port_index]->counters;
    							port[i].funcs=g_eth_ports[g_port_index]->funcs;  	
    							port[i].ipg=g_eth_ports[g_port_index]->ipg;							
    							port[i].lastLinkChange=g_eth_ports[g_port_index]->lastLinkChange;							
    							port[i].port_type=g_eth_ports[g_port_index]->port_type;
								g_port_index++;
							}
							if(!tipc_send(temp_index_sd->sd, (char *)port, send_len))
							{
								close(temp_index_sd->sd);
								temp_index_sd->sd = -1;
								temp_index_sd->thread_index = -1;
								return;
							}
							else
							{
						        printf("server reply info failed !, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
							}
							break;
						}
						case 1:
					        memset(recv_buf, 'y',recv_len);
                            tipc_send(temp_index_sd->sd, recv_buf, recv_len);
						    break;

						case 2 :
					        memset(recv_buf, 'z',recv_len);
                            tipc_send(temp_index_sd->sd, recv_buf, recv_len);
						    break;

						default:
							break;
					}
                }
                else
                {
                    return;
                }
            }
    }


    int  npd_communication_init()
    {
		/*printf("	int  npd_communication_init()\n");*/
        pthread_t npd_server_thread;
        pthread_t npd_client_thread;
		
		int SlotID = -1;
		int iCount;
		int ret = 0;

        FILE *fp = NULL;
		fp = fopen("/proc/board/slot_id", "r");
		if(fp != NULL)
		{
			ret = fscanf(fp, "%d", &SlotID);
			fclose(fp);
			if(1 != ret)		/* code optimize: unchecked value houxx@autelan.com  2013-1-19 */
			{
				printf("get slot id error,can not gat slot id\n");
				return -1;
			}
		}
		else
		{
			printf("get slot id error , file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
			return -1;
		}
			
		if(SlotID >= 0 && SlotID <= 4)
		{
			printf("SlotID = %d\n", SlotID);			
		}
		else
		{
			printf("slot id value error , file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
			return -1;
		}

	
        for (iCount = 0; iCount < MAX_CONNECT_CLIENT; iCount++)
        {
            thread_id_arr[iCount].thread_index = -1;
            thread_id_arr[iCount].sd = -1;
        }        
		
        gNpdStat = NPD_SERVER_STAT;

        if(gNpdStat == NPD_SERVER_STAT)
        {
			npd_thread_create(&npd_server_thread, (void *)npd_creat_server, (void*)SlotID, 1);
			pthread_join(npd_server_thread, NULL);
        }
    	else if(gNpdStat == NPD_CLIETN_STAT)
    	{
			npd_thread_create(&npd_client_thread, (void *)npd_creat_client, (void*)SlotID, 1);
			pthread_join(npd_client_thread, NULL);
    	}      
		return 0;		/* code optimize: Missing return statement houxx@autelan.com  2013-1-19 */
		
    }

    void npd_creat_server(void* arg)
    {
		/*printf("	void npd_creat_server\n");*/
        int server_sock = -1;
        int fd = -1;
        int index = -1;
		int iSlotID = -1;
        struct sockaddr_tipc server_sockaddr;
        server_sock = socket (AF_TIPC, SOCK_STREAM, 0);
        if (server_sock < 0)
        {
			    printf("failed : server_sock < 0 , file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
                return ;
        }
        
		iSlotID = (int)arg;
		if(iSlotID < 0 || iSlotID > 4)
		{
			printf("slotid error value = %d, file: %s, function: %s , line : %d\n", iSlotID, __FILE__, __FUNCTION__, __LINE__);
			close(server_sock);/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
			return;
		}

    	switch(iSlotID)
	    {
			case 0 :
				gServerTypeLow = NPD_COMM_TYPE_SLOT_ZERO_LOW;
				gServerTypeUP  = NPD_COMM_TYPE_SLOT_ZERO_UP;
				break;
			case 1 :
				gServerTypeLow = NPD_COMM_TYPE_SLOT_ONE_LOW;
				gServerTypeUP  = NPD_COMM_TYPE_SLOT_ONE_UP;					
				break;
			case 2 :
				gServerTypeLow = NPD_COMM_TYPE_SLOT_TWO_LOW;
				gServerTypeUP  = NPD_COMM_TYPE_SLOT_TWO_UP;					
				break;	
			default:
				break;
		}


        if (tipc_set_server_addr(&server_sockaddr, gServerTypeLow, gServerTypeUP))
        {
                return ;
        }

        if (0 != bind (server_sock, (struct sockaddr*)&server_sockaddr, sizeof(struct sockaddr)))
                {
                        printf("	bind failed, return, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
						close(server_sock);/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
                        return;
                }

                if (0 != listen (server_sock, 0))
                {
                        printf("	Server: Failed to listen, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
						close(server_sock);/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
						return;
                }

                while (1)
                {                       
                        if ((fd = accept(server_sock, 0, 0)) >= 0)
                        {

                                index = find_free_index();
                                if (index < 0)
                                {
                                    continue;
                                }
                                thread_id_arr[index].sd = fd;
                                thread_id_arr[index].thread_index = index;
                                if (!npd_thread_create(&thread_id_arr[index].thread_index, npd_thread_recv, (void *)(&thread_id_arr[index]), 0))
                                {
                                       printf("	create recv thread failed, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
                                }
                        }
                }
                return ;

    }

    void npd_creat_client(void* arg)
    {
		/*printf("	void npd_creat_client\n");*/
        int client_sock = -1;
        struct sockaddr_tipc client_sockaddr;
        char send_buf[20] = {0};
		int recv_len;
        client_sock = socket (AF_TIPC, SOCK_STREAM, 0);
        if (client_sock < 0)
        {
                return ;
        }

        int iSlotID = 0;//*(int *)arg;

    	switch(iSlotID)
	    {
			case 0 :
				gClientInstance= NPD_COMM_TYPE_SLOT_ZERO_INSTANCE;
				break;
			case 1 :
				gClientInstance = NPD_COMM_TYPE_SLOT_ONE_INSTANCE;				
				break;
			case 2 :
				gClientInstance = NPD_COMM_TYPE_SLOT_TWO_INSTANCE;				
				break;	
			default:
				break;
		}

        if (tipc_set_client_addr(&client_sockaddr, gClientInstance))
        {
                return ;
        }

        if(!tipc_client_connect_to_server(client_sock, client_sockaddr))
        {
            npd_cmd_t *cmd = (npd_cmd_t*)send_buf;
			
            while (1)
            {         
				int i;
				for(i = 0; i < 3; i++)
				{
					cmd->opcode= i;
                    if (!tipc_send(client_sock, send_buf, 20))
                    {
						tipc_recv(client_sock, send_buf, &recv_len);
						/*printf("recv : %s", send_buf);*/
                        sleep(1);
                    }
                    else
                    {
                            printf("	send failed, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
                    }

                    sleep(1);
				}
            }
        }
		close(client_sock);		/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
    }

	int npd_send(char *send_buf, int send_buf_len, char *recv_buf, int *recv_buf_len)
    {
		/*printf("	void npd_send\n");*/
        int client_sock = -1;
        struct sockaddr_tipc client_sockaddr;
        client_sock = socket (AF_TIPC, SOCK_STREAM, 0);
        if (client_sock < 0)
        {			/* code optimize: Missing return statement houxx@autelan.com  2013-1-19 */
                return -1;
        }

        npd_cmd_t *cmd = (npd_cmd_t*)send_buf;

    	switch(cmd->slot)
	    {
			case 0 :
				gClientInstance= NPD_COMM_TYPE_SLOT_ZERO_INSTANCE;
				break;
			case 1 :
				gClientInstance = NPD_COMM_TYPE_SLOT_ONE_INSTANCE;				
				break;
			case 2 :
				gClientInstance = NPD_COMM_TYPE_SLOT_TWO_INSTANCE;				
				break;	
			default:
				break;
		}

        if (tipc_set_client_addr(&client_sockaddr, gClientInstance))
        {
                return -1;
        }

        if(!tipc_client_connect_to_server(client_sock, client_sockaddr))
        {
              if (!tipc_send(client_sock, send_buf, send_buf_len))
              {
				 tipc_recv(client_sock, recv_buf, recv_buf_len);
				 close(client_sock);
				 return 0;
              }
              else
              {			  	 
                 printf("	send failed, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
				 close(client_sock);/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
				 return -1;
              }
        }
		else
		{
			printf("	send failed\n, file: %s, function: %s , line : %d\n",__FILE__, __FUNCTION__, __LINE__);
			close(client_sock);	/* code optimize: recourse leak houxx@autelan.com  2013-1-19 */
			return -1;
		}

		/*	close(client_sock);
			return 0; */ /* code optimize: Structurally dead code houxx@autelan.com  2013-1-19 */

    }

#ifdef __cplusplus
}
#endif
