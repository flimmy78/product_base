#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/tipc.h>

#define SERVER_TYPE  18888
#define SERVER_INST  17

void wait_for_server(__u32 name_type, __u32 name_instance, int wait)
{
	struct sockaddr_tipc topsrv;
	struct tipc_subscr subscr;
	struct tipc_event event;

	int sd = socket(AF_TIPC, SOCK_SEQPACKET, 0);
	memset(&topsrv, 0, sizeof(topsrv));
	topsrv.family = AF_TIPC;
	topsrv.addrtype = TIPC_ADDR_NAME;
	topsrv.addr.name.name.type = TIPC_TOP_SRV;
	topsrv.addr.name.name.instance = TIPC_TOP_SRV;

	/* Connect to topology server */

	if (0 > connect(sd, (struct sockaddr *)&topsrv, sizeof(topsrv))) {
		perror("Client: failed to connect to topology server");
		exit(1);
	}

	subscr.seq.type = htonl(name_type);
	subscr.seq.lower = htonl(name_instance);
	subscr.seq.upper = htonl(name_instance);
	subscr.timeout = htonl(wait);
	subscr.filter = htonl(TIPC_SUB_SERVICE);

	if (send(sd, &subscr, sizeof(subscr), 0) != sizeof(subscr)) {
		perror("Client: failed to send subscription");
		exit(1);
	}
	/* Now wait for the subscription to fire */

	if (recv(sd, &event, sizeof(event), 0) != sizeof(event)) {
		perror("Client: failed to receive event");
		exit(1);
	}
	if (event.event != htonl(TIPC_PUBLISHED)) {
		printf("Client: server {%u,%u} not published within %u [s]\n",
		       name_type, name_instance, wait/1000);
		exit(1);
	}

	close(sd);
}

int atoi(const char *str)
{
    unsigned int value=0;
    int b_plus=1;

    switch(*str)
    {
    case '+':
        str++;
        break;
    case '-':
        b_plus=0;
        str++;
        break;
    default:
        break;
    }

    while('\0' != *str)
    {
        value = (value*10)+(*str-'0');
        str++;
    }
    if(!b_plus)
        value=-value;
    return value;
}

struct tipc_test_str {
	unsigned int total_counter;
	unsigned int packet_id;
	char checkcode[9];
};

int main(int argc, char **argv)
{
	int sd;
	if (argc < 4)
	{
		printf("please type as: tipc_client buf_size usleeptime num\n");
		return 0;
	}
	unsigned int buf_size = atoi(argv[1]);
	unsigned int send_interval = atoi(argv[2]);
	unsigned int repeat_counter = atoi(argv[3]);
	struct sockaddr_tipc server_addr;
	char *buf;
	int i;
	int len;
	struct tipc_test_str *test_buf_header;
	buf = malloc(buf_size);
	test_buf_header = (struct tipc_test_str *)buf;
	memset(buf, 0, buf_size);
	test_buf_header->total_counter = repeat_counter;
	char *data;
	int counter = 0;

	data = (char*)(buf+sizeof(struct tipc_test_str));
	len = sizeof(struct tipc_test_str);
	for (i = 0; i<buf_size-len; i++)
		data[i] = 'a' + i%22;
	printf("****** TIPC client hello world program started ******\n");

	wait_for_server(SERVER_TYPE, SERVER_INST, 10000);

	sd = socket(AF_TIPC, SOCK_RDM, 0);

	server_addr.family = AF_TIPC;
	server_addr.addrtype = TIPC_ADDR_NAME;
	server_addr.addr.name.name.type = SERVER_TYPE;
	server_addr.addr.name.name.instance = SERVER_INST;
	server_addr.addr.name.domain = 0;
	while (counter<repeat_counter)
	{		
		test_buf_header->packet_id = ++counter;
		/*if (counter%5 == 0)
		{
			continue;
		}*/
		//memset(test_buf_header->checkcode, 0, 9);
		//make_XOR_checkcode(buf, chkcode);
		//strcpy(test_buf_header->checkcode, chkcode);
		//printf("test_buf_header->checkcode = %s, chkcode = %s\n", test_buf_header->checkcode, chkcode);
		if (0 > sendto(sd, buf, buf_size, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)))
		{
			printf("\nClient: failed to send packet %d\n", counter);
			exit(1);
		}
		printf(".");
		usleep(send_interval);
	}
	printf("\nclient send %u packet of size %u interval between packet is %uus\n", counter, buf_size, send_interval);
	printf("\n****** TIPC client hello program finished ******\n");
	return 0;
}

