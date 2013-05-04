/* server.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define SERV_PORT 8000

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


int main(void)
{
	struct sockaddr_in servaddr, cliaddr;
	socklen_t cliaddr_len = sizeof(cliaddr);
	int listenfd, connfd;
	int i, n;
	int ret;
	char inbuf[4096] = {'\0'};
	struct timeval wait_time;
	wait_time.tv_sec = 0;
	wait_time.tv_usec = 0;
	struct tipc_test_str *test_buf_header = (struct tipc_test_str *)inbuf;
	fd_set rdfds;
	unsigned int packet_lost = 0;
	unsigned int packet_counter = 0;
	printf("****** UDP server hello world program started ******\n");

	listenfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	if (test_thread_create(&thread_timeout, (void *)test_timeout_thread, NULL, 0) != 1)
    {
    	printf("create timeout thread failed\n");
    }
    
	ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(ret<0)
	{
		printf("bind error\n");
	}

	while (1)
	{
		FD_ZERO(&rdfds);
		FD_SET(listenfd, &rdfds);
		ret = select((listenfd)+1, &rdfds, NULL, NULL, &wait_time);

		if (ret < 0)
		{
			printf("ret < 0\n");
		}
		else if (ret == 0)
		{
		}
		else
		{
			if (0 >= recvfrom(listenfd, inbuf, 4096, 0, (struct sockaddr *)&cliaddr, &cliaddr_len))
		    {
				perror("Server: unexpected message");
				continue;
			}
			packet_counter++;
			is_timeout = 3;
			start = 1;
			//printf("packet_counter = %d\n", packet_counter);
			//printf("test_buf_header->total_counter = %d, test_buf_header->packet_id = %d\n", test_buf_header->total_counter, test_buf_header->packet_id);
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
	}
	printf("client send %u packets, receive %d packets\n", test_buf_header->total_counter, test_buf_header->total_counter-packet_lost);
	printf("lost packets %d\n", packet_lost);
	printf("\n****** UDP server hello program finished ******\n");
	return 0;
}

