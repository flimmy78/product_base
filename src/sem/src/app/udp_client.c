/* client.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERV_PORT 8000

struct tipc_test_str {
	unsigned int total_counter;
	unsigned int packet_id;
	char checkcode[9];
};

int main(int argc, char *argv[])
{
	struct sockaddr_in servaddr;
	char *buf;
	int i;
	int len;
	int sockfd, n;
	char dip[20];
	int counter = 0;
	if (argc != 5) {
		fputs("please type as: udp_client slot buf_size usleeptime num\n", stderr);
		exit(1);
	}
	unsigned int buf_size = atoi(argv[2]);
	unsigned int send_interval = atoi(argv[3]);
	unsigned int repeat_counter = atoi(argv[4]);
	struct tipc_test_str *test_buf_header;
	buf = malloc(buf_size);
	test_buf_header = (struct tipc_test_str *)buf;
	memset(buf, 0, buf_size);
	test_buf_header->total_counter = repeat_counter;
	char *data;
	
	data = (char*)(buf+sizeof(struct tipc_test_str));
	len = sizeof(struct tipc_test_str);
	for (i = 0; i<buf_size-len; i++)
		data[i] = 'a' + i%22;
	printf("****** UDP client hello world program started ******\n");
    
	sprintf(dip, "169.254.2.%d", atoi(argv[1]));
    
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, dip, &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);
    
	while (counter<repeat_counter)
	{		
		test_buf_header->packet_id = ++counter;
		if (0 > sendto(sockfd, buf, buf_size, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)))
		{
			printf("\nClient: failed to send packet %d\n", counter);
			exit(1);
		}
		printf(".");
		usleep(send_interval);
	}
	printf("\nclient send %u packet of size %u interval between packet is %uus\n", counter, buf_size, send_interval);
	printf("\n****** UDP client hello program finished ******\n");
	
	return 0;
}

