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
* cli_tunnel_server.c
*
*
* CREATOR:
*		zhanwei@autelan.com
*
* DESCRIPTION:
*		CLI tunnel server(using TIPC APIs and PTY).
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
#include <stropts.h>
#include "quagga/thread.h"
#include "tipc_api.h"

#define GETPTY_BUFSIZE 32
#define TIPC_EXEV_NAME "/bin/bash"

typedef struct TIPC_LOGIN_SESSION_LIST {
	pid_t pid;
	int ptyfd;
	int sockfd_read;
	int sockfd_write;
#define TS_BUF1(session) ((unsigned char*)(session + 1))
#define TS_BUF2(session) (((unsigned char*)(session + 1)) + BUFSIZE)
	int rdidx1, wridx1, size1;
	int rdidx2, wridx2, size2;
    struct TIPC_LOGIN_SESSION_LIST *next;
}tipc_login_session_list;

tipc_login_session_list *tipc_server_session_list_head = NULL;

enum { BUFSIZE = (4 * 1024 - sizeof(tipc_login_session_list)) / 2 };

char exev_name[32] = {0};

int tipc_login_server_handler(int sock, char * buff, int len, void *private_data);
int tipc_login_server_pty_handler(int ptyfd, char *buff, int len, void* private_data);
int tipc_login_accept_callback(int ctrl_sock, int sock, void **private_data);
tipc_login_session_list *make_tipc_login_session(int sock);
static void free_tipc_session(tipc_login_session_list *ts);
tipc_login_session_list* tipc_session_list_find(int sock);
int tipc_session_list_add(tipc_login_session_list *session);
void tipc_session_list_del(tipc_login_session_list* session);
void osal_thread_cancel_by_sockfd(int service_type, int sock);
pid_t startslave (char path_name[32], int *fdm)  
{
	const char *login_argv[2];
    pid_t pid;
    int master = -1;
	char pts_name[GETPTY_BUFSIZE];
    pid = forkpty (&master, pts_name, NULL, NULL);  
    if (pid < 0)  
    { 
        return -1;
    }  
  
    if (pid == 0)  
    { 
	    login_argv[0] = path_name;
	    login_argv[1] = NULL;

	    execvp(path_name, (char **)login_argv);
     }  
  	*fdm = master;
    return pid;  
}

struct thread_master * thread_master_login;

int main(int argc, char* argv[], char* dummy[])
{
	int ret = -1;
	int service_type = CLI_TUNNEL_SERVICE;
	int slot_test = 1;
	slot_test = tipc_get_own_node();
	slot_test &= 0xF;
    strcpy(exev_name, TIPC_EXEV_NAME);
    
	if(argc == 2)
	{
        strcpy(exev_name,argv[1]);
	}
    else if(argc == 3)
    {
        strcpy(exev_name,argv[1]);
        slot_test = atoi(argv[2]);
    }
	ret = tipc_server_init(service_type, slot_test, tipc_login_server_handler, tipc_login_accept_callback, NULL);
    osal_thread_master_run(service_type);
}

int tipc_login_server_handler(int sock, char * buff, int len, void *data)
{
    tipc_login_session_list * session;
    int fd;
    session = tipc_session_list_find(sock);
    if(session)
    {
        if(len <=0)
        {
			sleep(1);
            tipc_session_list_del(session);
            osal_thread_cancel_by_sockfd(CLI_TUNNEL_SERVICE, session->ptyfd);
            close(session->ptyfd);
            return TIPC_FAIL;
        }
        fd = session->ptyfd;
        return write(fd, buff, len);
    }
    else
        return TIPC_FAIL;
}

/* Delete a thread from the list. */
static struct thread *
osal_thread_list_delete (struct thread_list *list, struct thread *thread)
{
  if (thread->next)
    thread->next->prev = thread->prev;
  else
    list->tail = thread->prev;
  if (thread->prev)
    thread->prev->next = thread->next;
  else
    list->head = thread->next;
  thread->next = thread->prev = NULL;
  list->count--;
  return thread;
}

void osal_thread_cancel_by_sockfd(int service_type, int sock)
{
    struct thread_master *thread_master = NULL;
    struct thread *thread;
    struct thread *next;
    struct thread_list *list;
	
	thread_master = osal_thread_master_pool_find(service_type);
	if(NULL == thread_master)
	{
        printf("Can not find service %d thread master.\r\n", service_type);
		return;
	}
    list = &thread_master->read;
    if(list == NULL)
	{
        printf("Can not find read list.\r\n");
		return;
	}
    for (thread = list->head; thread; thread = next)
    {
      next = thread->next;
      if(THREAD_FD(thread) == sock)
      {
          if (FD_ISSET (THREAD_FD (thread), &thread_master->readfd))
          {
              FD_CLR(THREAD_FD (thread), &thread_master->readfd);
              osal_thread_list_delete (list, thread);
          }
      }
    }
    list = &thread_master->write;
    if(list == NULL)
	{
        printf("Can not find write list.\r\n");
		return;
	}
    for (thread = list->head; thread; thread = next)
    {
      next = thread->next;
      if(THREAD_FD (thread) == sock)
      {
          if (FD_ISSET (THREAD_FD (thread), &thread_master->writefd))
          {
              FD_CLR(THREAD_FD (thread), &thread_master->writefd);
              osal_thread_list_delete (list, thread);
          }
      }
    }
}

int tipc_login_server_pty_handler(int ptyfd, char *buff, int len, void* private_data)
{
    int sockfd;
    tipc_login_session_list * session = (tipc_login_session_list *)private_data;
    sockfd = session->sockfd_read;
    if(len < 0)
    {
    	waitpid(session->pid, NULL, 0);
        tipc_session_list_del(session);
        osal_thread_cancel_by_sockfd(CLI_TUNNEL_SERVICE, sockfd);
        close(sockfd);
        return 0;
    }
	while(len > 0)
	{
		write(sockfd, buff, (len > MAX_TIPC_SEND_LEN?MAX_TIPC_SEND_LEN:len));
		len -= MAX_TIPC_SEND_LEN;
	}
	return 0;
}

int tipc_login_accept_callback(int ctrl_sock, int sock, void **data)
{
    tipc_login_session_list *psession;
    psession = make_tipc_login_session(sock);
    return 0;
}

tipc_login_session_list *make_tipc_login_session(int sock) 
{
	const char *login_argv[2];
	struct termios termbuf;
	int fdm, fds;
	pid_t pid;
	tipc_login_session_list *psession = malloc(sizeof(tipc_login_session_list) + BUFSIZE * 2);
    bzero(psession,sizeof(psession));
    pid = startslave(exev_name, &fdm);
    if(fdm < 0)
    {
        printf("Can not get pty.\r\n");
        return NULL;
    }
	psession->pid = pid;
	psession->ptyfd = fdm;
	psession->sockfd_read = sock;
	fcntl(sock,F_SETFL,fcntl(sock,F_GETFL,0) | O_NDELAY);
	psession->sockfd_write = sock;
    tipc_session_list_add(psession);
    osal_register_read_fd(fdm, CLI_TUNNEL_SERVICE, tipc_login_server_pty_handler, psession, 1);
	return psession;
}


static void free_tipc_session(tipc_login_session_list *ts)
{
}


tipc_login_session_list* tipc_session_list_find(int sock)
{
	tipc_login_session_list *tipc_session = NULL;
	tipc_session = tipc_server_session_list_head;
	while(tipc_session)
	{
		if(tipc_session->sockfd_read == sock)
		{
			return tipc_session;
		}
		tipc_session = tipc_session->next;
	}
	return NULL;
}

int tipc_session_list_add(tipc_login_session_list *session)
{
	/*tipc_login_session_list *tipc_session = NULL;
	tipc_login_session_list *new_tipc_session = NULL;
	if(tipc_server_session_list_head == NULL)
	{
        tipc_server_session_list_head = session;
	}
	else
	{
		tipc_session = tipc_server_session_list_head;
		while(tipc_session->next != NULL)
		{
			tipc_session = tipc_session->next;
		}
		tipc_session->next = session;
	}*/
	session->next = tipc_server_session_list_head;
	tipc_server_session_list_head = session;
	return TIPC_SUCCESS;
}

void tipc_session_list_del(tipc_login_session_list* session)
{
	/*tipc_login_session_list *tipc_session = NULL;
	tipc_login_session_list *last_tipc_session = NULL;
	if(tipc_server_session_list_head)
	{
		if(tipc_server_session_list_head == session)
		{
			tipc_session = tipc_server_session_list_head;
			tipc_server_session_list_head = tipc_server_session_list_head->next;
			free_tipc_session(tipc_session);
		}
		else
		{
			last_tipc_session = tipc_server_session_list_head;
			tipc_session = last_tipc_session->next;
			while(tipc_session)
			{
				if(tipc_session == session)
				{
					last_tipc_session->next = tipc_session->next;
					free_tipc_session(tipc_session);
					break;
				}
				last_tipc_session = tipc_session;
				tipc_session = last_tipc_session->next;
			}
		}
	}*/
	tipc_login_session_list* pre;
	if (session == tipc_server_session_list_head)
	{
		tipc_server_session_list_head = session->next;
	}
	for (pre = tipc_server_session_list_head; pre; pre = pre->next)
		if (pre->next == session)
		{
			pre->next = session->next;
		}
}

