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
* tipc_api.c
*
*
* CREATOR:
*		zhanwei@autelan.com
*
* DESCRIPTION:
*		TIPC APIs (using zebra's thread machansim).
*
* DATE:
*		05/05/2010	
*UPDATE:
*  FILE REVISION NUMBER:
*  		$Revision: 1.00 $	
*******************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/syscall.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/tipc.h>
#include <pthread.h>
#include <sched.h>
#include "quagga/thread.h"
#include "tipc_api.h"

struct thread_master *master;
osal_thread_master_list * osal_thread_master_list_head = NULL;
npd_tipc_socket_list * tipc_client_socket_list_head = NULL;
npd_tipc_socket_list * tipc_server_socket_list_head = NULL;

static int tipc_mutex_inited = 0;

static pthread_mutex_t tipc_client_lock;
static pthread_mutex_t tipc_server_lock;
static pthread_mutex_t tipc_master_lock;


int tipc_mutex_init()
{
    pthread_mutexattr_t client_attr;
    pthread_mutexattr_t server_attr;
    pthread_mutexattr_t master_attr;
	if(tipc_mutex_inited)
		return 0;
	tipc_mutex_inited = 1;
    pthread_mutexattr_init(&client_attr);
    pthread_mutexattr_settype(&client_attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&tipc_client_lock, &client_attr);
	
    pthread_mutexattr_init(&server_attr);
    pthread_mutexattr_settype(&server_attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&tipc_server_lock, &server_attr);
	
    pthread_mutexattr_init(&master_attr);
    pthread_mutexattr_settype(&master_attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&tipc_master_lock, &master_attr);
    return 0;
}

void osal_thread_tell_whoami
(
	unsigned int service_type
)
{
	int fd = 0;
	pid_t self = 0;
	unsigned char pidBuf[64] = {0};

	self = getpid();
	
	fd = open("/var/run/thread_master.pid",O_CREAT|O_RDWR|O_APPEND, 0777);
	if(fd < 0)
	{
		printf("open file error when thread master service %d tell its pid %d\r\n",	\
					service_type,self);
		return;
	}
	sprintf(pidBuf,"%d(%d) - threadMaster%d\r\n",self, syscall(SYS_gettid), service_type);
	write(fd,pidBuf,strlen(pidBuf));

	close(fd);
	
	return;
}

int osal_thread_create( char* name,
                           void (*pfnFunc)( void * ),
                           unsigned long ulStartPriority,
                           unsigned long ulStackSize,
                           void * pArgs)
{
	int status = 0;
    pthread_attr_t attr;
    pthread_t tid;
    struct sched_param param;
    int newprio=20;
	int policy = 0;
	size_t old_stack_size = 0;
    
    status = pthread_attr_init(&attr);
	if(status != 0)
	{
		printf("Attr init failed at func %s line %d.\r\n", __func__, __LINE__);
		return status;
	}
    status = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	if(status != 0)
	{
		printf("Attr set scope failed at func %s line %d.\r\n", __func__, __LINE__);
		return status;
	}
	status = pthread_attr_setstacksize(&attr, ulStackSize);
	if(status != 0)
	{
		printf("Attr set stack size failed at func %s line %d.\r\n", __func__, __LINE__);
		return status;
	}
	status = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	if(status != 0)
	{
		printf("Attr get sched policy failed at func %s line %d.\r\n", __func__, __LINE__);
		return status;
	}
	
    status = pthread_attr_getschedparam(&attr, &param);
	if(status != 0)
	{
		printf("Attr get sched param failed at func %s line %d.\r\n", __func__, __LINE__);
		return status;
	}
    param.sched_priority=ulStartPriority;             
    status = pthread_attr_setschedparam(&attr, &param);
	if(status != 0)
	{
		printf("Attr set sched param failed at func %s line %d.\r\n", __func__, __LINE__);
		printf("Max thread priority is %d, input priority is %d.\r\n", sched_get_priority_max(SCHED_FIFO), ulStartPriority);
		return status;
	}
    status = pthread_create(&tid, &attr, (void *)pfnFunc, pArgs);
	if(status != 0)
	{
		printf("Pthread create failed at func %s line %d.\r\n", __func__, __LINE__);
		return status;
	}
	return status;
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

int osal_thread_read_buffer_length_set(int service_type, int len)
{
    struct thread_master *master = NULL;
    struct thread *t;
    struct thread *next;
    struct thread_list *list;
	osal_thread_session * read_session;
	
	master = osal_thread_master_pool_find(service_type);
	if(NULL == master)
	{
		return -1;
	}
	
	if(len <= 0 || len > TIPC_MAX_USER_MSG_SIZE)
	{
		len = TIPC_MAX_USER_MSG_SIZE;
	}
	
	list = &master->read;
    for (t = list->head; t; t = next)
    {
        next = t->next;
		read_session = THREAD_ARG(t);
		if(read_session)
		{
			read_session->max_length = len;
		}
    }
	return 0;
}

void osal_thread_cancel_by_sock(int service_type, int sock)
{
    struct thread_master *master = NULL;
    struct thread *thread;
    struct thread *next;
    struct thread_list *list;
	
	master = osal_thread_master_pool_find(service_type);
	if(NULL == master)
	{
		return;
	}
    list = &master->read;
    assert (list);
    
    for (thread = list->head; thread; thread = next)
    {
      next = thread->next;
      if(THREAD_FD(thread) == sock)
      {
          if (FD_ISSET (THREAD_FD (thread), &master->readfd))
          {
              FD_CLR(THREAD_FD (thread), &master->readfd);
              osal_thread_list_delete (list, thread);
          }
      }
    }
    list = &master->write;
    for (thread = list->head; thread; thread = next)
    {
      next = thread->next;
      if(THREAD_FD (thread) == sock)
      {
          if (FD_ISSET (THREAD_FD (thread), &master->writefd))
          {
              FD_CLR(THREAD_FD (thread), &master->writefd);
              osal_thread_list_delete (list, thread);
          }
      }
    }
}
void osal_release_master_resource(int service_type)
{
    struct thread_master *master = NULL;
    struct thread *t;
    struct thread *next;
    struct thread_list *list;
	osal_thread_session * read_session;
	osal_thread_session * server_session;
	osal_thread_session * timer_session;
	int sock = -1;
	
	master = osal_thread_master_pool_find(service_type);
	if(NULL == master)
	{
		return;
	}
	osal_thread_master_pool_del(service_type);
	list = &master->read;
    for (t = list->head; t; t = next)
    {
        next = t->next;
		sock = THREAD_FD(t);
		read_session = THREAD_ARG(t);
		if(read_session)
		{
    		if(read_session->read_handler)
    		{
    			(*read_session->read_handler)(sock, NULL, -1, read_session->private_data);
				read_session->read_handler = NULL;
    		}
			if(read_session->private_data)
			{
				free(read_session->private_data);
				read_session->private_data = NULL;
			}
			free(read_session);
			THREAD_ARG(t) = NULL;
		}
		if(sock >= 0)
		{
		    close(sock);
			THREAD_FD(t) = -1;
		}
    }
	list = &master->write;
    for (t = list->head; t; t = next)
    {
        next = t->next;
		server_session = THREAD_ARG(t);
		if(server_session)
		{
    		if(server_session->write_handler)
    		{
    			(*server_session->write_handler)(sock, NULL, -1, read_session->private_data);
				server_session->write_handler = NULL;
    		}
			if(server_session->private_data)
			{
				free(server_session->private_data);
				server_session->private_data = NULL;
			}
			free(server_session);
			THREAD_ARG(t) = NULL;
		}
    }
	list = &master->timer;
    for (t = list->head; t; t = next)
    {
        next = t->next;
	    timer_session = THREAD_ARG(t);
		if(timer_session)
		{
			if(timer_session->private_data)
			{
				free(timer_session->private_data);
				timer_session->private_data = NULL;
			}
			free(timer_session);
			THREAD_ARG(t) = NULL;
		}
    }
	thread_master_free(master);
}

/*为了让select超时*/
int do_nothing(struct thread *thread)
{
	thread_add_background(thread->master, do_nothing, 0, 1000);
	return 0;
}

void osal_thread_master_run(int service_type)
{
    struct thread thread;
	struct thread_master * thread_master;
	thread_master = osal_thread_master_pool_find(service_type);
	if(NULL == thread_master)
	{
		assert(0);
		return;
	}
	
	if(osal_thread_master_running_get(service_type) == 1)
	{
		return;
	}
	osal_thread_master_running_set(service_type, 1);
	osal_thread_tell_whoami(service_type);
	thread_add_background(thread_master, do_nothing, 0, 1000);
    while (thread_fetch(thread_master, &thread))
    {
        thread_call(&thread);
    }
	osal_thread_master_running_set(service_type, 0);
}

struct thread_master * osal_thread_master_init(int service_type, int service_priority)
{
	struct thread_master * osal_thread_master = NULL;
	tipc_mutex_init();
	osal_thread_master = osal_thread_master_pool_find(service_type);
	if(osal_thread_master)
	{
		if(service_priority)
		{
			osal_thread_create(NULL, osal_thread_master_run, service_priority, 0x20000, (void *)service_type);
		}
		return osal_thread_master;
	}
	
	osal_thread_master = thread_master_create();
    if(osal_thread_master == NULL)
    {
    	return NULL;
    }
	osal_thread_master_pool_add(osal_thread_master, service_type);
	if(service_priority)
	{
		osal_thread_create(NULL, osal_thread_master_run, service_priority, 0x20000, (void *)service_type);
	}
	return osal_thread_master;
}

static int osal_thread_server_accept (struct thread *thread)
{
	osal_thread_session * server_session = NULL;
	osal_thread_session * read_session = NULL;
	int sock = THREAD_FD (thread);
	int new_socket = 0;
	uint remote_slot = 0;
	uint service_type = 0;
	server_session = THREAD_ARG(thread);
	read_session = malloc(sizeof(osal_thread_session));
	if(read_session && server_session)
	{
		memcpy(read_session, server_session, sizeof(osal_thread_session));
	}
	osal_thread_event(thread->master, OSAL_SERVER_CTRL, sock, server_session);
    if(server_session->accept_handler)
    {
		new_socket = (*server_session->accept_handler)(sock, (void*)&(read_session->private_data));
		if(new_socket < 0)
		{
		    return TIPC_FAIL;
		}
        osal_thread_event(thread->master, OSAL_THREAD_READ, new_socket, read_session);
    }
	return TIPC_SUCCESS;
}

static int osal_thread_read (struct thread *thread)
{
	char * read_buffer = NULL;
	int len = 0;
	int buff_len = 0;
	int ret = TIPC_FAIL;
	osal_thread_session * read_session;
	int sock = THREAD_FD(thread);
	read_session = THREAD_ARG(thread);
	buff_len = read_session->max_length;
	if(buff_len <= 0 || buff_len > TIPC_MAX_USER_MSG_SIZE)
	{
		buff_len = TIPC_MAX_USER_MSG_SIZE;
	}
	
	read_buffer = malloc(buff_len);
	if(read_buffer == NULL)
	{
		if(read_session->private_data)
		{
			free(read_session->private_data);
			read_session->private_data = NULL;
		}
		if(read_session)
		{
			free(read_session);
			THREAD_ARG(thread) = NULL;
		}
		return TIPC_FAIL;
	}
	memset(read_buffer, 0, buff_len);
	if((len = read(sock, read_buffer, buff_len)) <= 0)
	{
		if(read_session)
		{
    		if(read_session->read_handler)
    		{
    			ret = (*read_session->read_handler)(sock, read_buffer, len, read_session->private_data);
				read_session->read_handler = NULL;
    		}
    		if(read_session->private_data)
    		{
    			free(read_session->private_data);
				read_session->private_data = NULL;
    		}
			if(read_session->read_thread)
			{
		        thread_cancel(read_session->read_thread);
				read_session->read_thread = NULL;
			}
			free(read_session);
			THREAD_ARG(thread) = NULL;
		}
        if(ret != TIPC_BUFFER_OWNED)
            free(read_buffer);
		if(sock >= 0)
		{
		    close(sock);
			THREAD_FD(thread) = -1;
		}
		return TIPC_FAIL;
	}
	if(read_session)
	{
    	if(read_session->read_handler)
    	{
    	    ret = (*read_session->read_handler)(sock, read_buffer, len, read_session->private_data);
    	}
		if(read_session->repeat_flag)
		{
		    osal_thread_event(thread->master, OSAL_THREAD_READ, sock, read_session);
		}
		else
		{
    		if(read_session->private_data)
    		{
    			free(read_session->private_data);
    		}
			free(read_session);
		}
	}
    if(ret != TIPC_BUFFER_OWNED)
        free(read_buffer);
	return ret;
}
/*注意:这个函数不推荐使用*/
static int osal_thread_write (struct thread *thread)
{
	int ret = TIPC_FAIL;
	osal_thread_session * server_session;
	int sock = THREAD_FD(thread);
	server_session = THREAD_ARG(thread);
	if(server_session == NULL)
	{
		return TIPC_FAIL;
	}
	
	if(server_session->write_handler)
	{
	    ret = (*server_session->write_handler)(sock, 0, 0, server_session->private_data);
	}
	
    if(server_session->private_data)
    {
    	free(server_session->private_data);
    }
	free(server_session);
	return ret;
}

static int osal_thread_timer (struct thread *thread)
{
	int ret = TIPC_FAIL;
	osal_thread_session * timer_session;
	timer_session = THREAD_ARG(thread);

	if(timer_session)
	{
    	if(timer_session->timer_handler)
    	{
    	    ret = (*timer_session->timer_handler)(timer_session->private_data);
    	}
		if(timer_session->repeat_flag)
		{
		    osal_thread_event(thread->master, OSAL_THREAD_TIMER, 0, timer_session);
		}
		else
		{
        	if(timer_session->private_data)
        	{
        		free(timer_session->private_data);
        	}
			free(timer_session);
		}
	}
	return ret;
}

void osal_thread_event(struct thread_master * osal_thread_master, enum osal_thread_evnet osal_event, int fd, osal_thread_session * osal_session)
{
	switch(osal_event)
	{
		case OSAL_SERVER_CTRL:
			thread_add_read(osal_thread_master, osal_thread_server_accept, osal_session, fd);
			break;
		case OSAL_THREAD_READ:
			osal_session->read_thread = thread_add_read (osal_thread_master, osal_thread_read, osal_session, fd);
			break;
		case OSAL_THREAD_WRITE:
			osal_session->write_thread = thread_add_write (osal_thread_master, osal_thread_write, osal_session, fd);
			break;
		case OSAL_THREAD_TIMER:
			osal_session->timer_thread = thread_add_timer(osal_thread_master, osal_thread_timer, osal_session, osal_session->timer_val);
			break;
	}
}

int osal_thread_register_fd(int fd, int service_type, enum osal_thread_evnet osal_event, osal_thread_handler func, void * private_data, int repeat_flag)
{
	struct thread_master * osal_thread_master = NULL;
	osal_thread_session * osal_session = NULL;
	osal_thread_master = osal_thread_master_init(service_type, 0);
	if(osal_thread_master == NULL)
	{
		return -1;
	}
	
	osal_session = malloc(sizeof(osal_thread_session));
	if(osal_session == NULL)
	{
		return -1;
	}
	memset(osal_session, 0, sizeof(osal_thread_session));
	osal_session->service_type = service_type;
	osal_session->private_data = private_data;
	osal_session->repeat_flag = repeat_flag;
	
	switch(osal_event)
	{
		case OSAL_THREAD_READ:
			osal_session->read_handler = func;
			osal_session->read_thread = thread_add_read(osal_thread_master, osal_thread_read, osal_session, fd);
			break;
		case OSAL_THREAD_WRITE:
			osal_session->write_handler = func;
			osal_session->write_thread = thread_add_write(osal_thread_master, osal_thread_write, osal_session, fd);
			break;
		default:
			return -1;
	}
	return 0;
}

int osal_register_read_fd(int fd, int service_type, osal_thread_handler func, void * private_data, int repeat_flag)
{
	return osal_thread_register_fd(fd, service_type, OSAL_THREAD_READ, func, private_data, repeat_flag);
}

int osal_register_write_fd(int fd, int service_type, osal_thread_handler func, void * private_data)
{
	return osal_thread_register_fd(fd, service_type, OSAL_THREAD_WRITE, func, private_data, 0);
}

int osal_register_server_ctrl_fd(int fd, int service_type, int (*server_ctrl_handler)(int, void **), int(*read_hander)(int, char *, int, void *), void * private_data)
{
	struct thread_master * osal_thread_master = NULL;
	osal_thread_session * osal_session = NULL;
	osal_thread_master = osal_thread_master_init(service_type, 0);
	if(osal_thread_master == NULL)
	{
		return -1;
	}
	
	osal_session = malloc(sizeof(osal_thread_session));
	if(osal_session == NULL)
	{
		return -1;
	}
	memset(osal_session, 0, sizeof(osal_thread_session));
	osal_session->service_type = service_type;
	osal_session->private_data = private_data;
	osal_session->repeat_flag = 1;
	
    osal_session->accept_handler = server_ctrl_handler;
    osal_session->read_handler = read_hander;
	thread_add_read(osal_thread_master, osal_thread_server_accept, osal_session, fd);
	return 0;
}

int osal_register_timer(int timeout, int service_type, int (*func)(void *), void * private_data, int repeat_flag)
{
	struct thread_master * osal_thread_master = NULL;
	osal_thread_session * osal_session = NULL;
	osal_thread_master = osal_thread_master_init(service_type, 0);
	if(osal_thread_master == NULL)
	{
		return -1;
	}
	
	osal_session = malloc(sizeof(osal_thread_session));
	if(osal_session == NULL)
	{
		return -1;
	}
	memset(osal_session, 0, sizeof(osal_thread_session));
	osal_session->service_type = service_type;
	osal_session->private_data = private_data;
	osal_session->repeat_flag = repeat_flag;
	osal_session->timer_val = timeout;
	
    osal_session->timer_handler = func;
	osal_session->timer_thread = thread_add_timer(osal_thread_master, osal_thread_timer, osal_session, osal_session->timer_val);

	return 0;
}

uint tipc_get_own_node()
{
    struct sockaddr_tipc addr = {0};
    socklen_t sz = sizeof(addr);
	int sd = socket (AF_TIPC, SOCK_RDM, 0);
    if (0 > getsockname(sd,(struct sockaddr*)&addr,&sz))
	{
        printf("(PID:%d)Failed to get sock address for own\r\n", getpid());
	    return 0;
    }
	close(sd);
    return addr.addr.id.node;
}

uint tipc_get_peer_node(int sd)
{
    struct sockaddr_tipc addr = {0};
    socklen_t sz = sizeof(addr);
    if (0 > getpeername(sd,(struct sockaddr*)&addr,&sz))
	{
        printf("(PID:%d)Failed to get sock address for peer\r\n", getpid());
	    return 0;
    }
    return addr.addr.id.node;
}

uint tipc_socket_service_type(int sd)
{
    struct sockaddr_tipc addr = {0};
    socklen_t sz = sizeof(addr);
    if (0 > getpeername(sd,(struct sockaddr*)&addr,&sz))
	{
        printf("(PID:%d)Failed to get sock address for service type\r\n", getpid());
		return 0;
    }
    return addr.addr.name.name.type;
}

int tipc_server_accept (int sock, void ** data)
{
	tipc_thread_session * new_tipc_session = NULL;
	int new_socket = 0;
	uint remote_slot = 0;
	uint service_type = 0;
	new_socket = accept(sock, 0, 0);
	if (new_socket < 0 )
	{
		printf("Accept a data socket from ctrl socket %d failed: %s.\r\n", sock, safe_strerror(errno));
		return TIPC_FAIL;
	}
	remote_slot = tipc_get_peer_node(new_socket);
	if(remote_slot == 0)
	{
		close(new_socket);
		return TIPC_FAIL;
	}
    new_tipc_session = malloc(sizeof(tipc_thread_session));
    if(new_tipc_session == NULL)
    {
    	close(new_socket);
    	return TIPC_FAIL;
    }
	memset(new_tipc_session, 0, sizeof(tipc_thread_session));
	if(*data)
	{
		memcpy(new_tipc_session, *data, sizeof(tipc_thread_session));
	}
	
	new_tipc_session->local_node = tipc_get_own_node(new_socket);
	new_tipc_session->remote_node = remote_slot;
	remote_slot = NODE_TO_SLOT(remote_slot);/*FIXME......Zhanwei*/
	if(new_tipc_session->service_type)
	{
		service_type = new_tipc_session->service_type;
	}
	else
	{
    	service_type = tipc_socket_service_type(new_socket);
    	if(service_type == 0)
    	{
    		free(new_tipc_session);
    		close(new_socket);
    		return TIPC_FAIL;
    	}
	}
	*data = (void *)(new_tipc_session);
	tipc_server_socket_pool_add(new_socket, service_type, remote_slot);
	if(new_tipc_session->accept_handler)
	{
	    (*new_tipc_session->accept_handler)(sock, new_socket, &new_tipc_session->private_data);
	}
	return new_socket;
}

int connect_to_service(int service_type, int instance)
{
	int sock = 0;
	int imp = TIPC_CRITICAL_IMPORTANCE;
	struct sockaddr_tipc server_addr = {0};
	
	server_addr.family = AF_TIPC;
	server_addr.addrtype = TIPC_ADDR_NAME;
	server_addr.addr.name.name.type = service_type;
	server_addr.addr.name.name.instance = instance;
	server_addr.addr.name.domain = 0;
    
	sock = socket(AF_TIPC, SOCK_SEQPACKET, 0);
	if(sock < 0)
	{
		printf("Create socket for TIPC STREAM failed.\r\n");
		return TIPC_FAIL;
	}
    setsockopt(sock, SOL_TIPC, TIPC_IMPORTANCE, &imp, sizeof(imp));
	if(connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_tipc)) != 0)
	{
		printf("Connect to service %d at instance %d failed. error code :%s \r\n", service_type, instance, strerror(errno));
		close(sock);
		return TIPC_FAIL;
	}
	return sock;
}

int tipc_service_monitor (int sock, char *buffer, int len, void * data)
{
    struct tipc_event event;
	tipc_thread_session * tipc_session = (tipc_thread_session *)data;
	int new_socket = 0;
	int i = 0;
	int ret = 0;
	
    if(len <= 0)
    {
		return TIPC_FAIL;
    }
	memcpy(&event, buffer, sizeof(event));
    switch(event.event)
    {
		case TIPC_PUBLISHED:
			for(i = event.found_lower; i <= event.found_upper; i++)
			{
				if(tipc_get_own_node() == event.port.node)
				{
					return TIPC_FAIL;
				}
				new_socket = tipc_client_socket_pool_find(event.s.seq.type, i);
				if(new_socket > 0)
				{
					tipc_client_socket_pool_del(event.s.seq.type, i);
					shutdown(new_socket, SHUT_RDWR);
					/*SERVICE MONITOR单独起一个thread后，这个地方可能会有问题*/
					osal_thread_cancel_by_sock(tipc_session->service_type, new_socket);
					close(new_socket);
				}
				
				new_socket = connect_to_service(event.s.seq.type, i);
				if(new_socket > 0)
				{
				    ret = tipc_client_socket_pool_add(new_socket, event.s.seq.type, i);
					if(ret < 0)
					{
						return TIPC_FAIL;
					}
    				if(tipc_session)
    				{
    					if(tipc_session->monitor_handler)
    					{
    						ret = (*tipc_session->monitor_handler)(event.event, event.s.seq.type, i);
    					}
    				}
				}
			}
			return TIPC_SUCCESS;
		case TIPC_WITHDRAWN:
			for(i = event.found_lower; i <= event.found_upper; i++)
			{
				if(tipc_get_own_node() == i)
				{
					return TIPC_FAIL;
				}
				new_socket = tipc_client_socket_pool_find(event.s.seq.type, i);
				if(new_socket > 0)
				{
    				tipc_client_socket_pool_del(event.s.seq.type, i);
					shutdown(new_socket, SHUT_RDWR);
					/*SERVICE MONITOR单独起一个线程后，这个地方可能会有问题
					tipc_session->service_type是运行在不同于MONITOR的另外一个线程中
					数据需要互斥，当然这种情况只出现在既调用了tipc_client_init
					又用了异步收发的情况*/
					osal_thread_cancel_by_sock(tipc_session->service_type, new_socket);
					close(new_socket);
				}
    	        if(tipc_session)
    			{
    				if(tipc_session->monitor_handler)
    				{
    					ret = (*tipc_session->monitor_handler)(event.event, event.s.seq.type, i);
    				}
    			}
			}
			return TIPC_SUCCESS;
		case TIPC_SUBSCR_TIMEOUT:
			break;
		default:
			break;
    }
	return ret;
}

int tipc_thread_read(int sock, char* buff, int len, void* data)
{
	int ret = -1;
	tipc_thread_session *tipc_session = (tipc_thread_session *)data;
	
	if(tipc_session)
	{
		if(len < 0)
		{
			if(tipc_session->server_or_client == TIPC_SERVER_MODE)
			{
				tipc_session->server_or_client = 0;
			    tipc_server_socket_pool_del(tipc_session->service_type, NODE_TO_SLOT(tipc_session->remote_node));
			}
			/*不要在此返回，在上层用户程序里继续处理异常退出情况*/
		}
		if(tipc_session->read_handler)
		{
			ret = (*tipc_session->read_handler)(sock, buff, len, tipc_session->private_data);
			if(len < 0)
			{
				if(tipc_session->private_data)
				{
					free(tipc_session->private_data);
					tipc_session->private_data = NULL;
				}
				tipc_session->read_handler = NULL;
			}
		    return ret;
		}
	}
	return TIPC_FAIL;
}

int tipc_thread_asyc_write(int sock, char* dummy0, int dummy1, void* data)
{
    int ret = TIPC_FAIL;
    char *buff;
    int len = 0;
	tipc_thread_session *tipc_session = (tipc_thread_session *)data;
	
	if(tipc_session)
	{
	    len = tipc_session->len;
	    buff = tipc_session->buff;
	     	    
		if(len < 0 || tipc_session->buff == NULL)
		{
		    return TIPC_FAIL;
		}
		if(tipc_session->write_handler)
		{
			ret = (*tipc_session->write_handler)(sock, buff, len, NULL);
		}
		free(tipc_session->buff);
		tipc_session->buff = NULL;
	}
	return ret;
}

int tipc_client_sock_init(int service_type, int *rtn_sock)
{
    int sd;
    int ret = -1;
    struct tipc_subscr subscr = {{service_type,0x1001001,0x1010010},
                                     FOREVER,
                                     TIPC_SUB_SERVICE,
                                     {2,2,2,2,2,2,2,2}};
    struct sockaddr_tipc topsrv = {0};
    memset(&topsrv, 0, sizeof(topsrv));
	topsrv.family = AF_TIPC;
    topsrv.addrtype = TIPC_ADDR_NAME;
    topsrv.addr.name.name.type = TIPC_TOP_SRV;
    topsrv.addr.name.name.instance = TIPC_TOP_SRV;

	sd = socket (AF_TIPC, SOCK_SEQPACKET,0);
    assert(sd >= 0);

	subscr.seq.type = service_type;
	
    if (0 > connect(sd,(struct sockaddr*)&topsrv,sizeof(topsrv)))
	{
        printf("failed to connect to topology server");
		close(sd);
        return TIPC_FAIL;
    }
	
    if(send(sd,&subscr,sizeof(subscr),0) != sizeof(subscr))
	{
        printf("failed to send subscription");
		close(sd);
		return TIPC_FAIL;
    }
    *rtn_sock = sd;
    return TIPC_SUCCESS;
}

int tipc_client_init(int service_type, int (*monitor_handler)(int, int, int))
{
    int sd;
    int ret = -1;
	int monitor_service_type = SERVICE_MONITOR_TYPE;
	tipc_thread_session *tipc_session = NULL;
	struct thread_master* tipc_thread_master = NULL;
    struct tipc_subscr subscr = {{service_type,0x1001001,0x1010010},
                                     FOREVER,
                                     TIPC_SUB_SERVICE,
                                     {2,2,2,2,2,2,2,2}};
    struct sockaddr_tipc topsrv = {0};
    tipc_mutex_init();
    memset(&topsrv, 0, sizeof(topsrv));
	topsrv.family = AF_TIPC;
    topsrv.addrtype = TIPC_ADDR_NAME;
    topsrv.addr.name.name.type = TIPC_TOP_SRV;
    topsrv.addr.name.name.instance = TIPC_TOP_SRV;

	sd = socket (AF_TIPC, SOCK_SEQPACKET,0);
    assert(sd >= 0);

	subscr.seq.type = service_type;
	
    if (0 > connect(sd,(struct sockaddr*)&topsrv,sizeof(topsrv)))
	{
        printf("failed to connect to topology server");
		close(sd);
        return TIPC_FAIL;
    }
	/*这个thread master是给客户端异步收发的时候用的*/
	tipc_thread_master = osal_thread_master_init(service_type, 0);
	if(tipc_thread_master)
	{
	}
	else
	{
		close(sd);
		return TIPC_FAIL;
	}
	
	tipc_session = malloc(sizeof(tipc_thread_session));
	if(tipc_session == NULL)
	{
		close(sd);
		return TIPC_FAIL;
	}
	tipc_session->service_type = service_type;
	tipc_session->monitor_handler = monitor_handler;
	tipc_session->server_or_client = TIPC_CLIENT_MODE;
	
    if(send(sd,&subscr,sizeof(subscr),0) != sizeof(subscr))
	{
        printf("failed to send subscription");
		if(tipc_session)
		{
			free(tipc_session);
		}
		close(sd);
		return TIPC_FAIL;
    }
	ret = osal_register_read_fd(sd, SERVICE_MONITOR_TYPE, tipc_service_monitor, tipc_session, 1);
	if(ret == TIPC_FAIL)
	{
		if(tipc_session)
		{
			free(tipc_session);
		}
		close(sd);
		return TIPC_FAIL;
	}
	ret = osal_thread_create("service_monitor", osal_thread_master_run, 60, 0x4000, monitor_service_type);
	return TIPC_SUCCESS;
}

int tipc_server_sock_init(int service_type, int local_slot, int *rtn_sock)
{
	int ret = -1;
	struct sockaddr_tipc server_addr = {0};
	tipc_thread_session *tipc_session = NULL;
	int sock = 0;
	struct thread_master* tipc_thread_master = NULL;
	sock = socket (AF_TIPC, SOCK_SEQPACKET, 0);
	if (sock < 0)
	{
		printf("Server: unable to create socket\r\n");
		return TIPC_FAIL;
	}
	server_addr.family = AF_TIPC;
	server_addr.addrtype = TIPC_ADDR_NAMESEQ;
	server_addr.addr.nameseq.type = service_type;
	server_addr.addr.nameseq.lower = SLOT_TO_INSTANCE(local_slot);
	server_addr.addr.nameseq.upper = SLOT_TO_INSTANCE(local_slot);
	server_addr.scope = TIPC_ZONE_SCOPE;

	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)))
	{
		printf("Server: failed to bind port name\n");
		close(sock);
		return TIPC_FAIL;
	}

	if (0 != listen(sock, 0))
	{
		printf("Server: failed to listen\r\n");
		close(sock);
		return TIPC_FAIL;
	}    

    *rtn_sock = sock;
    return TIPC_SUCCESS;
}




int tipc_server_init(int service_type, int local_slot, int (*user_read_handler)(int, char *, int, void *), int (*accept_callback)(int,int, void **), void *private_data)
{
	int ret = -1;
	struct sockaddr_tipc server_addr = {0};
	tipc_thread_session *tipc_session = NULL;
	int sock = 0;
	struct thread_master* tipc_thread_master = NULL;
    tipc_mutex_init();
	sock = socket (AF_TIPC, SOCK_SEQPACKET, 0);
	if (sock < 0)
	{
		printf("Server: unable to create socket\r\n");
		return TIPC_FAIL;
	}
	server_addr.family = AF_TIPC;
	server_addr.addrtype = TIPC_ADDR_NAMESEQ;
	server_addr.addr.nameseq.type = service_type;
	server_addr.addr.nameseq.lower = SLOT_TO_INSTANCE(local_slot);
	server_addr.addr.nameseq.upper = SLOT_TO_INSTANCE(local_slot);
	server_addr.scope = TIPC_ZONE_SCOPE;

	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)))
	{
		printf("Server: failed to bind port name\n");
		close(sock);
		return TIPC_FAIL;
	}

	if (0 != listen(sock, 0))
	{
		printf("Server: failed to listen\r\n");
		close(sock);
		return TIPC_FAIL;
	}
	tipc_thread_master = osal_thread_master_init(service_type, 0);
	if(tipc_thread_master)
	{
	}
	else
	{
		close(sock);
		return TIPC_FAIL;
	}
	
	tipc_session = malloc(sizeof(tipc_thread_session));
	if(tipc_session == NULL)
	{
		close(sock);
		return TIPC_FAIL;
	}
	tipc_session->service_type = service_type;
	tipc_session->instance = SLOT_TO_INSTANCE(local_slot);
	tipc_session->server_or_client = TIPC_SERVER_MODE;
	tipc_session->read_handler = user_read_handler;
	tipc_session->accept_handler = accept_callback;
	tipc_session->private_data = private_data;
	ret = osal_register_server_ctrl_fd(sock, service_type, tipc_server_accept, tipc_thread_read, tipc_session);
	if(ret == TIPC_FAIL)
	{
		if(tipc_session)
		{
			free(tipc_session);
		}
		close(sock);
		return TIPC_FAIL;
	}
	return TIPC_SUCCESS;
}
/**/
int tipc_client_sync_send(int service_type, int dest_slot, char *buffer, int len)
{
	int sock = 0;
	int i = 0;
	int ret = TIPC_FAIL;
	if(dest_slot == BROADCAST_SLOT)
	{
		for(i = 0; i < MAX_SLOT_NUM; i++)
		{
	        sock = tipc_client_socket_pool_find(service_type, SLOT_TO_INSTANCE(i));
			if(sock > 0)
			{
                pthread_mutex_lock(&tipc_client_lock);
				ret = send(sock, buffer, len, 0);
				pthread_mutex_unlock(&tipc_client_lock);
			}
		}
		return ret;
	}
	else
	{
	    sock = tipc_client_socket_pool_find(service_type, SLOT_TO_INSTANCE(dest_slot));
	    if(sock > 0)
		{
			pthread_mutex_lock(&tipc_client_lock);
			ret = send(sock, buffer, len, 0);
			pthread_mutex_unlock(&tipc_client_lock);
			if(ret > 0)
			{
				return TIPC_SUCCESS;
			}
			else
			{
				printf("Send %d byte(s) failed\r\n", len);
				return TIPC_FAIL;
			}
		}
		else
		{
			return TIPC_FAIL;
		}
	}
}

int tipc_server_sync_send(int service_type, int dest_slot, char *buffer, int len)
{
	int sock = 0;
	int i = 0;
	int ret = TIPC_FAIL;
	if(dest_slot == BROADCAST_SLOT)
	{
		for(i = 0; i < MAX_SLOT_NUM; i++)
		{
	        sock = tipc_server_socket_pool_find(service_type, SLOT_TO_INSTANCE(i));
			if(sock > 0)
			{
				pthread_mutex_lock(&tipc_server_lock);
				ret = send(sock, buffer, len, 0);
				pthread_mutex_unlock(&tipc_server_lock);
			}
		}
		return TIPC_SUCCESS;
	}
	else
	{
	    sock = tipc_server_socket_pool_find(service_type, SLOT_TO_INSTANCE(dest_slot));
	    if(sock > 0)
		{
			pthread_mutex_lock(&tipc_server_lock);
			ret = send(sock, buffer, len, 0);
			pthread_mutex_unlock(&tipc_server_lock);
			if(ret > 0)
			{
				return TIPC_SUCCESS;
			}
			else
			{
				printf("Send %d byte(s) failed\r\n", len);
				return TIPC_FAIL;
			}
		}
		else
		{
			return TIPC_FAIL;
		}
	}
}
int tipc_client_sync_recv(int service_type, int dest_slot, char *buffer, int timeout)
{
	int sock = 0;
	int len = TIPC_FAIL;
	int num_sock = 0;
  	struct timeval    tv; 
  	struct timeval    now, earliest;
	fd_set rfds;
	FD_ZERO(&rfds);
	if(dest_slot == BROADCAST_SLOT)
	{
		/*not support*/
		return TIPC_FAIL;
	}
	else
	{
	    sock = tipc_client_socket_pool_find(service_type, SLOT_TO_INSTANCE(dest_slot));
	    if(sock > 0)
		{
	        FD_ZERO(&rfds);
		    FD_SET(sock, &rfds);
			num_sock = sock+1;
            gettimeofday(&earliest, 0);
            earliest.tv_sec+=timeout;/*总共3秒延时*/
            tv.tv_sec = 0;
            tv.tv_usec = 500000;

			while(1)
			{
            	gettimeofday(&now, 0);
                if(now.tv_sec > earliest.tv_sec)
            	{
            		return TIPC_RX_TIMEOUT;
            	}
            	if(select(num_sock, &rfds, NULL, NULL, &tv) < 0)
            	{
            		return TIPC_FAIL;
            	}
				if(FD_ISSET(sock, &rfds))
                {
    				len = recv(sock, buffer, TIPC_MAX_USER_MSG_SIZE, 0);
    				if(len <=0)
    				{
    					printf("Recv from board[%d] socket[%d] error:%s.\r\n", dest_slot, sock, strerror(errno));
						FD_CLR(sock, &rfds);
    				}
    				else
    				{
    					if(len == TIPC_MAX_USER_MSG_SIZE)
    					{
    						printf("Recv data length warning.\r\n");
    					}
    				}
					return len;
		        }
			}
		}
		else
		{
			return TIPC_FAIL;
		}
	}
	return TIPC_FAIL;
}
int tipc_sync_recv(int sock, char *buffer,int dest_slot, int timeout)
{
	int len = TIPC_FAIL;
  	struct timeval    tv;
  	struct timeval    now, earliest;
	fd_set rfds;
	FD_ZERO(&rfds);

	//printf("Receiving packet from socket : %d\r\n", sock);
    if(sock > 0)
	{
        FD_ZERO(&rfds);
	    FD_SET(sock, &rfds);
        gettimeofday(&earliest, 0);
        earliest.tv_sec+=timeout;/*总共3秒延时*/
        tv.tv_sec = 0;
        tv.tv_usec = 500000;

		while(1)
		{
        	gettimeofday(&now, 0);
            if(now.tv_sec > earliest.tv_sec)
        	{
        		return TIPC_RX_TIMEOUT;
        	}
        	if(select(FD_SETSIZE, &rfds, NULL, NULL, &tv) < 0)
        	{
				printf("Select error:%s\r\n", strerror(errno));
        		return TIPC_FAIL;
        	}
			if(FD_ISSET(sock, &rfds))
            {
				len = read(sock, buffer, TIPC_MAX_USER_MSG_SIZE);
				if(len <= 0)
				{
					printf("Recv from board[%d] socket[%d] error.\r\n", dest_slot, sock);
					FD_CLR(sock, &rfds);
				}
				else
				{
					if(len == TIPC_MAX_USER_MSG_SIZE)
					{
						printf("Recv data length warning.\r\n");
					}
				}
				return len;
	        }
		}
	}
	else
	{
		return TIPC_FAIL;
	}
	return TIPC_FAIL;
}

int tipc_client_async_send(int service_type, int dest_slot, char *buffer, int len)
{
	int sock = 0;
	int i = 0;
	int ret = TIPC_FAIL;
	if(dest_slot == BROADCAST_SLOT)
	{
		for(i = 0; i < MAX_SLOT_NUM; i++)
		{
	        sock = tipc_client_socket_pool_find(service_type, SLOT_TO_INSTANCE(i));
			if(sock > 0)
			{
                pthread_mutex_lock(&tipc_client_lock);
				fcntl(sock, F_SETFL, O_NONBLOCK);
				ret = send(sock, buffer, len, 0);
				pthread_mutex_unlock(&tipc_client_lock);
			}
		}
		return ret;
	}
	else
	{
	    sock = tipc_client_socket_pool_find(service_type, SLOT_TO_INSTANCE(dest_slot));
	    if(sock > 0)
		{
			pthread_mutex_lock(&tipc_client_lock);
			fcntl(sock, F_SETFL, O_NONBLOCK);
			ret = send(sock, buffer, len, 0);
			pthread_mutex_unlock(&tipc_client_lock);
			if(ret > 0)
			{
				return TIPC_SUCCESS;
			}
			else
			{
				printf("Send %d byte(s) failed\r\n", len);
				return TIPC_FAIL;
			}
		}
		else
		{
			return TIPC_FAIL;
		}
	}
}

int tipc_client_async_recv(int service_type, int dest_slot, int (*user_recv_handler)(int, char *, int, void *), int repeat_flag)
{
	int sock = 0;
	int i = 0;
	int ret=TIPC_FAIL;
	tipc_thread_session *tipc_session = NULL;
	if(dest_slot == BROADCAST_SLOT)
	{
		for(i = 0; i < MAX_SLOT_NUM; i++)
		{
	        sock = tipc_client_socket_pool_find(service_type, SLOT_TO_INSTANCE(i));
			if(sock > 0)
			{
				tipc_session = malloc(sizeof(osal_thread_session));
				if(tipc_session)
				{
	                tipc_session->service_type = service_type;
	                tipc_session->server_or_client = TIPC_CLIENT_MODE;
					osal_thread_register_fd(sock, service_type, OSAL_THREAD_READ, user_recv_handler, tipc_session, repeat_flag);
				}
			}
		}
		return TIPC_SUCCESS;
	}
	else
	{
	    sock = tipc_client_socket_pool_find(service_type, SLOT_TO_INSTANCE(dest_slot));
	    if(sock > 0)
		{
			tipc_session = malloc(sizeof(osal_thread_session));
			if(tipc_session)
			{
                tipc_session->service_type = service_type;
                tipc_session->server_or_client = TIPC_CLIENT_MODE;
				ret=osal_thread_register_fd(sock, service_type, OSAL_THREAD_READ, user_recv_handler, tipc_session, repeat_flag);
				if(ret==TIPC_FAIL)
				{
				     printf("cannot register user_recv_handler \r\n");
				}
			}
			return TIPC_SUCCESS;
		}
		else
		{
			return TIPC_FAIL;
		}
	}
}

/*find  函数可能在不同于add 和del函数的master主线程中进行，所以应该做互斥*/
int tipc_client_socket_pool_find(int service_type, int instance)
{
	int sock = -1;
	npd_tipc_socket_list *tipc_socket = NULL;
    pthread_mutex_lock(&tipc_client_lock);
	tipc_socket = tipc_client_socket_list_head;
	while(tipc_socket)
	{
		if(tipc_socket->service_type == service_type && tipc_socket->instance == instance)
		{
			sock = tipc_socket->sock;
			break;
		}
		tipc_socket = tipc_socket->next;
	}
    pthread_mutex_unlock(&tipc_client_lock);
	return sock;
}

int tipc_client_socket_pool_add(int sock, int service_type, int instance)
{
	npd_tipc_socket_list *tipc_socket = NULL;
	npd_tipc_socket_list *new_tipc_socket = NULL;
    pthread_mutex_lock(&tipc_client_lock);
	if(tipc_client_socket_list_head == NULL)
	{
		tipc_client_socket_list_head = malloc(sizeof(npd_tipc_socket_list));
		if(tipc_client_socket_list_head)
		{
			tipc_client_socket_list_head->service_type = service_type;
			tipc_client_socket_list_head->instance = instance;
			tipc_client_socket_list_head->sock = sock;
			tipc_client_socket_list_head->next = NULL;
            pthread_mutex_unlock(&tipc_client_lock);
			return TIPC_SUCCESS;
		}
		else
		{
            pthread_mutex_unlock(&tipc_client_lock);
			return TIPC_FAIL;
		}
	}
	else
	{
		tipc_socket = tipc_client_socket_list_head;
		while(tipc_socket->next != NULL)
		{
			tipc_socket = tipc_socket->next;
		}
		
		new_tipc_socket = malloc(sizeof(npd_tipc_socket_list));
		if(new_tipc_socket)
		{
			new_tipc_socket->service_type = service_type;
			new_tipc_socket->instance = instance;
			new_tipc_socket->sock = sock;
			new_tipc_socket->next = NULL;
			tipc_socket->next = new_tipc_socket;
            pthread_mutex_unlock(&tipc_client_lock);
			return TIPC_SUCCESS;
		}
		else
		{
            pthread_mutex_unlock(&tipc_client_lock);
			return TIPC_FAIL;
		}
	}
    pthread_mutex_unlock(&tipc_client_lock);
	return TIPC_SUCCESS;
}

void tipc_client_socket_pool_del(int service_type, int instance)
{
	npd_tipc_socket_list *tipc_socket = NULL;
	npd_tipc_socket_list *last_tipc_socket = NULL;
    pthread_mutex_lock(&tipc_client_lock);
	if(tipc_client_socket_list_head)
	{
		if(tipc_client_socket_list_head->service_type == service_type && tipc_client_socket_list_head->instance == instance)
		{
			tipc_socket = tipc_client_socket_list_head;
			tipc_client_socket_list_head = tipc_client_socket_list_head->next;
			free(tipc_socket);
		}
		else
		{
			last_tipc_socket = tipc_client_socket_list_head;
			tipc_socket = last_tipc_socket->next;
			while(tipc_socket)
			{
				if(tipc_socket->service_type == service_type && tipc_socket->instance == instance)
				{
					last_tipc_socket->next = tipc_socket->next;
					free(tipc_socket);
					break;
				}
				last_tipc_socket = tipc_socket;
				tipc_socket = last_tipc_socket->next;
			}
		}
	}
    pthread_mutex_unlock(&tipc_client_lock);
}
/*FIXME......参数不够，同一槽位上可能有多个客户端连接*/
int tipc_server_socket_pool_find(int service_type, int instance)
{
	int sock = -1;
	npd_tipc_socket_list *tipc_socket = NULL;
    pthread_mutex_lock(&tipc_server_lock);
	tipc_socket = tipc_server_socket_list_head;
	while(tipc_socket)
	{
		if(tipc_socket->service_type == service_type && tipc_socket->instance == instance)
		{
			sock = tipc_socket->sock;
			break;
		}
		tipc_socket = tipc_socket->next;
	}
    pthread_mutex_unlock(&tipc_server_lock);
	return sock;
}

int tipc_server_socket_pool_add(int sock, int service_type, int instance)
{
	npd_tipc_socket_list *tipc_socket = NULL;
	npd_tipc_socket_list *new_tipc_socket = NULL;
    pthread_mutex_lock(&tipc_server_lock);
	if(tipc_server_socket_list_head == NULL)
	{
		tipc_server_socket_list_head = malloc(sizeof(npd_tipc_socket_list));
		if(tipc_server_socket_list_head)
		{
			tipc_server_socket_list_head->service_type = service_type;
			tipc_server_socket_list_head->instance = instance;
			tipc_server_socket_list_head->sock = sock;
			tipc_server_socket_list_head->next = NULL;
			pthread_mutex_unlock(&tipc_server_lock);
			return TIPC_SUCCESS;
		}
		else
		{
			pthread_mutex_unlock(&tipc_server_lock);
			return TIPC_FAIL;
		}
	}
	else
	{
		tipc_socket = tipc_server_socket_list_head;
		while(tipc_socket->next != NULL)
		{
			tipc_socket = tipc_socket->next;
		}
		
		new_tipc_socket = malloc(sizeof(npd_tipc_socket_list));
		if(new_tipc_socket)
		{
			new_tipc_socket->service_type = service_type;
			new_tipc_socket->instance = instance;
			new_tipc_socket->sock = sock;
			new_tipc_socket->next = NULL;
			tipc_socket->next = new_tipc_socket;
			pthread_mutex_unlock(&tipc_server_lock);
			return TIPC_SUCCESS;
		}
		else
		{
			pthread_mutex_unlock(&tipc_server_lock);
			return TIPC_FAIL;
		}
	}
	pthread_mutex_unlock(&tipc_server_lock);
	return TIPC_SUCCESS;
}

void tipc_server_socket_pool_del(int service_type, int instance)
{
	npd_tipc_socket_list *tipc_socket = NULL;
	npd_tipc_socket_list *last_tipc_socket = NULL;
    pthread_mutex_lock(&tipc_server_lock);
	if(tipc_server_socket_list_head)
	{
		if(tipc_server_socket_list_head->service_type == service_type && tipc_server_socket_list_head->instance == instance)
		{
			tipc_socket = tipc_server_socket_list_head;
			tipc_server_socket_list_head = tipc_server_socket_list_head->next;
			free(tipc_socket);
		}
		else
		{
			last_tipc_socket = tipc_server_socket_list_head;
			tipc_socket = last_tipc_socket->next;
			while(tipc_socket)
			{
				if(tipc_socket->service_type == service_type && tipc_socket->instance == instance)
				{
					last_tipc_socket->next = tipc_socket->next;
					free(tipc_socket);
					break;
				}
				last_tipc_socket = tipc_socket;
				tipc_socket = last_tipc_socket->next;
			}
		}
	}
	pthread_mutex_unlock(&tipc_server_lock);
}

struct thread_master *osal_thread_master_pool_find(int service_type)
{
	struct thread_master * osal_thread_master = NULL;
	osal_thread_master_list *current_thread_master_list = NULL;

    pthread_mutex_lock(&tipc_master_lock);
	current_thread_master_list = osal_thread_master_list_head;
	while(current_thread_master_list)
	{
		if(current_thread_master_list->service_type == service_type)
		{
			osal_thread_master = current_thread_master_list->osal_thread_master;
			break;
		}
		current_thread_master_list = current_thread_master_list->next;
	}
    pthread_mutex_unlock(&tipc_master_lock);
	return osal_thread_master;
}

int osal_thread_master_running_get(int service_type)
{
	struct thread_master * osal_thread_master = NULL;
	osal_thread_master_list *current_thread_master_list = NULL;
	pid_t self = 0;
	self = getpid();
    pthread_mutex_lock(&tipc_master_lock);
	current_thread_master_list = osal_thread_master_list_head;
	while(current_thread_master_list)
	{
		if(current_thread_master_list->service_type == service_type)
		{
            pthread_mutex_unlock(&tipc_master_lock);
			return current_thread_master_list->running;
		}
		current_thread_master_list = current_thread_master_list->next;
	}
    pthread_mutex_unlock(&tipc_master_lock);
	return 0;
}

void osal_thread_master_running_set(int service_type, int running)
{
	struct thread_master * osal_thread_master = NULL;
	osal_thread_master_list *current_thread_master_list = NULL;
	pid_t self = 0;
	self = getpid();
    pthread_mutex_lock(&tipc_master_lock);
	current_thread_master_list = osal_thread_master_list_head;
	while(current_thread_master_list)
	{
		if(current_thread_master_list->service_type == service_type)
		{
			current_thread_master_list->running = running;
            pthread_mutex_unlock(&tipc_master_lock);
			return;
		}
		current_thread_master_list = current_thread_master_list->next;
	}
    pthread_mutex_unlock(&tipc_master_lock);
	return;
}

int osal_thread_master_pool_add(struct thread_master * osal_thread_master, int service_type)
{
	osal_thread_master_list *current_thread_master_list = NULL;
	osal_thread_master_list *new_osal_thread_master = NULL;
	pid_t self = 0;
	self = getpid();
    pthread_mutex_lock(&tipc_master_lock);
	if(osal_thread_master_list_head == NULL)
	{
		osal_thread_master_list_head = malloc(sizeof(npd_tipc_socket_list));
		if(osal_thread_master_list_head)
		{
			memset(osal_thread_master_list_head, 0, sizeof(npd_tipc_socket_list));
			osal_thread_master_list_head->service_type = service_type;
			osal_thread_master_list_head->osal_thread_master = osal_thread_master;
			osal_thread_master_list_head->next = NULL;
            pthread_mutex_unlock(&tipc_master_lock);
			return TIPC_SUCCESS;
		}
		else
		{
			pthread_mutex_unlock(&tipc_master_lock);
			return TIPC_FAIL;
		}
	}
	else
	{
		current_thread_master_list = osal_thread_master_list_head;
		while(current_thread_master_list->next != NULL)
		{
			current_thread_master_list = current_thread_master_list->next;
		}
		
		new_osal_thread_master = malloc(sizeof(npd_tipc_socket_list));
		if(new_osal_thread_master)
		{
			memset(new_osal_thread_master, 0, sizeof(npd_tipc_socket_list));
			new_osal_thread_master->service_type = service_type;
			new_osal_thread_master->osal_thread_master = osal_thread_master;
			new_osal_thread_master->next = NULL;
			current_thread_master_list->next = new_osal_thread_master;
			pthread_mutex_unlock(&tipc_master_lock);
			return TIPC_SUCCESS;
		}
		else
		{
			pthread_mutex_unlock(&tipc_master_lock);
			return TIPC_FAIL;
		}
	}
	pthread_mutex_unlock(&tipc_master_lock);
	return TIPC_SUCCESS;
}

void osal_thread_master_pool_del(int service_type)
{
	osal_thread_master_list *current_thread_master_list = NULL;
	osal_thread_master_list *last_osal_thread_master = NULL;
    pthread_mutex_lock(&tipc_master_lock);
	if(osal_thread_master_list_head)
	{
		if(osal_thread_master_list_head->service_type == service_type)
		{
			current_thread_master_list = osal_thread_master_list_head;
			osal_thread_master_list_head = osal_thread_master_list_head->next;
			free(current_thread_master_list);
		}
		else
		{
			last_osal_thread_master = osal_thread_master_list_head;
			current_thread_master_list = last_osal_thread_master->next;
			while(current_thread_master_list)
			{
				if(current_thread_master_list->service_type == service_type)
				{
					last_osal_thread_master->next = current_thread_master_list->next;
					free(current_thread_master_list);
					break;
				}
				last_osal_thread_master = current_thread_master_list;
				current_thread_master_list = last_osal_thread_master->next;
			}
		}
	}
	pthread_mutex_unlock(&tipc_master_lock);
}

