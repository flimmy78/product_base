/*******************************************************************************
Copyright (C) Autelan Technology

This software file is owned and distributed by Autelan Technology 
********************************************************************************
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************
* cli_tunnel_client.c
*
*
* CREATOR:
*		zhanwei@autelan.com
*
* DESCRIPTION:
*		App used logining to shell on other CPUs(using TIPC APIs).
*
* DATE:
*		06/15/2010	
*UPDATE:
*  FILE REVISION NUMBER:
*  		$Revision: 1.00 $	
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/tipc.h>
#include "tipc_api.h"

struct tsession
{
    int sockfd_read;
    int sockfd_write;
    int fd_in;
    int fd_out;
};

struct termios bktty;
struct tsession *session = NULL;
int term_backed = 0;
int console_read(int fd, char *buffer, int len, void *private_data);

static void release_resouce()
{
    int fd;

    if (term_backed == 1)
    {
        fd = fileno(stdin);
        tcsetattr(fd,TCSANOW,&bktty);
        term_backed = 0;
    }

    osal_release_master_resource(CLI_TUNNEL_SERVICE);
    exit(0);
}

int client_async_recv_handle(int sock, char *buff , int len, void *private_data)
{
    int fd;

    if (len <= 0)
    {
        release_resouce();
        return TIPC_FAIL;
    }

    if (NULL == buff)
    {
        return TIPC_FAIL;
    }

    printf("%s", buff);
    fflush(stdout);
    return TIPC_SUCCESS;
}

int main(int argc, char* argv[], char* dummy[])
{
    int ret = -1;
    int service_type = CLI_TUNNEL_SERVICE;
    int slot_test = 1;
    int sock,fd;

    if (argc == 2)
    {
        slot_test = atoi(argv[1]);
    }

    //printf("Start client: %d:%d ... \n", service_type, SLOT_TO_INSTANCE(slot_test));
    sock = connect_to_service(service_type, SLOT_TO_INSTANCE(slot_test));

    if (sock <= 0)
    {
        printf("failed to connect to server %d,instance %d\r\n",service_type, SLOT_TO_INSTANCE(slot_test));
        return TIPC_FAIL;
    }
    ret = osal_register_read_fd(sock, service_type, client_async_recv_handle, NULL, 1);

    if (ret == TIPC_FAIL)
    {
        if (session)
        {
            free(session);
        }

        close(sock);
        return TIPC_FAIL;
    }

    fd = client_set_fd_parameter(sock, service_type, SLOT_TO_INSTANCE(slot_test));
    ret = osal_register_read_fd(fd, service_type, console_read, session, 1);

    if (ret == TIPC_FAIL)
    {
        if (session)
        {
            free(session);
        }

        close(sock);
        return TIPC_FAIL;
    }
    signal(SIGTERM, release_resouce);
    signal(SIGINT, release_resouce);
    signal(SIGQUIT, release_resouce);
    signal(SIGSEGV, release_resouce);
    osal_thread_master_run(service_type);
    return TIPC_SUCCESS;
}

int console_read(int fd, char *buffer, int len, void *private_data)
{
    int sock;
    int ret = -1;
    struct tsession * read_session = (struct tsession *)private_data;
	if(len < 0)
	{
		return -1;
	}
    sock = read_session->sockfd_read;
    ret = write(sock, buffer, len);
    if (ret == -1)
    {
        return TIPC_FAIL;
    }
    return TIPC_SUCCESS;
}

int client_set_fd_parameter(int sock, int service_type, int instance)
{
    int fd;
    struct termios savetty;

    if (sock <= 0)
    {
        return -1;
    }

    session = malloc(sizeof(struct tsession));

    if (session == NULL)
    {
        printf("No enough memory.\n");
    }
    else
    {
        memset(session, 0, sizeof(struct tsession));
    }

    fd = fileno(stdin);

    if (tcgetattr(fd,&savetty) < 0)
    {
        term_backed = 0;
        //printf("get the parameters associated with the terminal failed!\n");
        //exit(1);
    }
    else
    {
        memcpy(&bktty,&savetty,sizeof(savetty));
        term_backed = 1;
        /* 设置raw mode ，只传输数据，不处理*/
        savetty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
        savetty.c_oflag &= ~OPOST;   /*Output*/
    
        if (tcsetattr(fd,TCSANOW,&savetty) < 0)
        {
            printf("set the parameters associated with the terminal failed!\n");
            //exit(1);
        }
    }

    session->fd_in = fd;
    session->fd_out = fileno(stdout);
    session->sockfd_read = sock;
    return fd;
}

