#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <linux/tipc.h>
#include <pthread.h>

#define SERVER_TYPE  18888
#define SERVER_INST  17
#define RECV_BUFF_SIZE 

struct tipc_test_str {
	unsigned int total_counter;
	unsigned int packet_id;
	char checkcode[9];
};

int test_thread_create(pthread_t *new_thread, void * (*thread_fun)(void *), void *arg, int is_detached)
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
		printf("new_thread is null\n");
		return 0;
	}
	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, s);

	if (pthread_create(new_thread, &attr, thread_fun, arg) != 0)
	{
		printf("thread create failed\n");
		return 0;
	}

	return 1;	
}



int exit_flag = 0;
int is_timeout = 3;
int start = 0;

void test_timeout_thread(void)
{	
	while (1) {
		if (!is_timeout&&start) {
			exit_flag = 1;
		}
		sleep(1);
		//printf("is_timeout =%d exit_flag=%d\n", is_timeout, exit_flag);	
		is_timeout--;
	}
}

pthread_t thread_timeout;

int main(int argc, char **argv)
{
	struct sockaddr_tipc server_addr;
	struct sockaddr_tipc client_addr;
	socklen_t alen = sizeof(client_addr);
	int sd;
	int ret;
	char inbuf[4096] = {'\0'};
	unsigned int packet_lost = 0;
	unsigned int packet_counter = 0;
	struct timeval wait_time;
	wait_time.tv_sec = 0;
	wait_time.tv_usec = 0;
	struct tipc_test_str *test_buf_header = (struct tipc_test_str *)inbuf;
	fd_set rdfds;

	printf("****** TIPC server hello world program started ******\n");

	server_addr.family = AF_TIPC;
	server_addr.addrtype = TIPC_ADDR_NAMESEQ;
	server_addr.addr.nameseq.type = SERVER_TYPE;
	server_addr.addr.nameseq.lower = SERVER_INST;
	server_addr.addr.nameseq.upper = SERVER_INST;
	server_addr.scope = TIPC_ZONE_SCOPE;

	sd = socket(AF_TIPC, SOCK_RDM, 0);
	if (0 != bind(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
		printf("Server: failed to bind port name\n");
		exit(1);
	}

	if (test_thread_create(&thread_timeout, (void *)test_timeout_thread, NULL, 0) != 1)
    {
    	printf("create timeout thread failed\n");
    }

	while (1)
	{
		FD_ZERO(&rdfds);
		FD_SET(sd, &rdfds);
		ret = select((sd)+1, &rdfds, NULL, NULL, &wait_time);

		if (ret < 0)
		{
			printf("ret < 0\n");
		}
		else if (ret == 0)
		{
		}
		else
		{		
			if (0 >= recvfrom(sd, inbuf, 4096, 0, (struct sockaddr *)&client_addr, &alen))
		    {
				perror("Server: unexpected message");
				continue;
			}
			packet_counter++;
			is_timeout = 3;
			start = 1;
			/*tipcbuf = (struct tipc_test_str *)inbuf;
			memcpy(input_str, inbuf, 4096);
			inputstr = (struct tipc_test_str *)input_str;
			memset(inputstr->checkcode, 0, 9);
			make_XOR_checkcode(input_str, chkcode);*/
			//printf("packet_counter = %d\n", packet_counter);
			//printf("tipcbuf->checkcode = %s, chkcode = %s\n", tipcbuf->checkcode, chkcode);
			while(test_buf_header->packet_id>packet_counter)
			{
				printf("lost packet %d\n", packet_counter++);
				packet_lost++;
			}
		}

		if (exit_flag) {
			while(test_buf_header->total_counter>packet_counter)
			{
				printf("lost packet %d\n", ++packet_counter);
				packet_lost++;
			}
			printf("main exit_flag = %d\n", exit_flag);
		}
		if ((exit_flag||test_buf_header->total_counter == packet_counter) && start) {
			break;
		}
		//usleep(10);
		//printf("Server: Message received: %s \n", inbuf);
	}
	/*if (packet_counter != test_buf_header->total_counter)
		printf("client may send %u packets,but only receive %u packets\n", test_buf_header->total_counter, packet_counter);
	else
		printf("client send %u packets, receive all packets\n", test_buf_header->total_counter);*/
	printf("client send %u packets, receive %d packets\n", test_buf_header->total_counter, test_buf_header->total_counter-packet_lost);
	printf("lost packets %d\n", packet_lost);
	printf("\n****** TIPC server hello program finished ******\n");
	return 0;
}

