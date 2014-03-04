#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <pthread.h>
#include <string.h>
#include <syslog.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <fcntl.h>
#include <sys/wait.h>	


#include "dbus/dbus.h"



#include "pfm.h"



#define PFM_DBUS_BUSNAME				"pfm.daemon"
#define PFM_DBUS_OBJPATH				"/pfm/daemon"
#define PFM_DBUS_INTERFACE				"pfm.daemon"
#define PFM_DBUS_METHOD_PFM_TABLE 	"pfm_maintain_table"
#define PFM_DBUS_METHOD_PFM_DEAL_SERVICE 	"pfm_deal_service"
	
DBusConnection *pfm_dbus_connection = NULL;
int product_serial = 0;
	
	

ifindex_dest_ip_tree* forwarding_table[MAX_PROTOCOL][MAX_PORT];
int 				  sd=0 ;//netlink socket



/************************************************************
*
*
*					sys_log_func
*
*
*************************************************************/

typedef enum {
  PFM_SYSTEM_LOG_INFO,
  PFM_SYSTEM_LOG_SECURITY,
  PFM_SYSTEM_LOG_FATAL
} PfmSystemLogSeverity;


netlink_transport_info pfm_table[64] = {0};


void 
pfm_init_system_log (void)
{
  openlog ("pfm_deamon", LOG_PID, LOG_DAEMON);
}


void
pfm_system_log (PfmSystemLogSeverity severity, const char *msg, ...)
{
  int flags;
  va_list args;

  va_start (args, msg);

  
  switch (severity)
    {
      case PFM_SYSTEM_LOG_INFO:
        flags =  LOG_DAEMON | LOG_NOTICE;
        break;
      case PFM_SYSTEM_LOG_SECURITY:
        flags = LOG_AUTH | LOG_NOTICE;
        break;
      case PFM_SYSTEM_LOG_FATAL:
        flags = LOG_DAEMON|LOG_CRIT;
		break;
      default:
        return;
    }

  vsyslog (flags, msg, args);

  if (severity == PFM_SYSTEM_LOG_FATAL)
  	vprintf(msg,args);

  va_end (args);
}

#if 0
void
_pfm_system_logv (PfmSystemLogSeverity severity, const char *msg, va_list args)
{
  int flags;
  switch (severity)
    {
      case PFM_SYSTEM_LOG_INFO:
        flags =  LOG_DAEMON | LOG_NOTICE;
        break;
      case PFM_SYSTEM_LOG_SECURITY:
        flags = LOG_AUTH | LOG_NOTICE;
        break;
      case PFM_SYSTEM_LOG_FATAL:
        flags = LOG_DAEMON|LOG_CRIT;
      default:
        return;
    }

  vsyslog (flags, msg, args);

  if (severity == PFM_SYSTEM_LOG_FATAL)
  	vprintf("%s",msg,args);
    exit (1);
}
#endif
/*************************************************************
*
*
*
*                      data structure func
*
*
*
**************************************************************/

/*********************show func*******************/

int list_all_ifindex_dest_ip_tree(ifindex_dest_ip_tree* root)
{
	if(NULL != root)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"ifindex is %d\n",root -> ifindex);
		list_all_ifindex_dest_ip_tree(GET_LIFT_CHILD_IFINDEX(root));
		list_all_ifindex_dest_ip_tree(GET_RIGHT_CHILD_IFINDEX(root));
	}else{

		return 0 ;

	}
	return 0 ;
}




int list_all_dest_ip_tree(dest_ip_tree* root)
{
	if(NULL != root)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"ip addr is %d\n",root -> dest_ip);
	/*	pfm_system_log(PFM_SYSTEM_LOG_INFO,"slot_id is %d\n",root -> forward_slot);*/
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"\n");
		list_all_dest_ip_tree(GET_LIFT_CHILD_DEST_IP(root));
		list_all_dest_ip_tree(GET_RIGHT_CHILD_DEST_IP(root));
	}else{

		return 0 ;

	}
	return 0 ;
}

int list_all_elem(ifindex_dest_ip_tree* root)
{
	if(NULL != root)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"\n\n====================================\n");
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"ifindex is %d\n",root -> ifindex);
		list_all_dest_ip_tree(root -> ip_tree);
		list_all_elem(GET_LIFT_CHILD_IFINDEX(root));
		list_all_elem(GET_RIGHT_CHILD_IFINDEX(root));
	}else{

		return 0;

	}
	return 0;
}


/********************test func***********************/


void test_forwarding_table_init_state()
{
	int i,j;
	for(i=0;i<MAX_PROTOCOL;i++)
	{
		for(j=0;j<MAX_PORT;j++)
		{
			if(NULL  != forwarding_table[i][j])
				pfm_system_log(PFM_SYSTEM_LOG_INFO,"forwarding_table[%d][%d] is not NULL\n",i,j);
		}
	}
}



/**********************init func*********************/



int init_tree_elem(tree_elem** root,tree_elem* parent)
{
	if(NULL != (*root))
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"init_tree_elem ,root is not NULL\n");
		return -1;
	}
/*
	if(NULL == parent)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"init_tree_elem ,parent is NULL\n");
		return -1;
	}
*/
	(*root) = (tree_elem*)malloc(TREE_ELEM_LEN);

	if(NULL == (*root))
	{
		pfm_system_log(PFM_SYSTEM_LOG_FATAL,"init_tree_elem,malloc error\n");
		return -1;
	}
	memset((*root),0,TREE_ELEM_LEN);

	
	(*root) -> lc 		= NULL;
	(*root) -> rc		= NULL;
	(*root) -> parent	= parent;
	(*root) -> deep		= 0;

	return 0;
}
	

int init_dest_ip_tree(dest_ip_tree** root,\
						_u32		 ip_addr,\
						int 		 slot_id,\
					  dest_ip_tree*	 parent)
{
	if(NULL != (*root))
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"init_dest_ip_tree is not NULL\n");
		return -1;
	}
	
	(*root) = (dest_ip_tree*)malloc(DEST_IP_TREE_LEN);
	
	if(-1 == init_tree_elem(&((*root) -> tree),(tree_elem*)(parent)))
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"in init_dest_ip_tree,init_tree_elem \n");
		return -1;
	}
	
	(*root) -> dest_ip 			= ip_addr;
	(*root) -> forward_slot 	= slot_id;

	return 0;
}


int init_ifindex_dest_ip_tree(ifindex_dest_ip_tree** 	root,\
								int 					ifindex,\
								int 					forward_opt,\
								int 					forward_opt_para,\
								_u32 					ip_addr,\
								int 					slot_id,\
								ifindex_dest_ip_tree*	parent)
{
	if(NULL != (*root))
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"init_ifindex_dest_ip_tree ,root is not NULL \n");
		return -1 ;
	}
	
	(*root) = malloc(IFINDEX_DEST_IP_TREE_LEN);

	if(NULL == (*root))
	{
		pfm_system_log(PFM_SYSTEM_LOG_FATAL,"init_ifindex_dest_ip_tree , malloc error\n");
		return -1;
	}
	memset((*root),0,IFINDEX_DEST_IP_TREE_LEN);

	
	if(-1 == init_tree_elem(&((*root) -> tree),(tree_elem*)(parent)))
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"in init_ifindex_dest_ip_tree ,tree_elem_new return NULL \n");
		return -1;
	}
	if(-1 == init_dest_ip_tree(&((*root) -> ip_tree),ip_addr,slot_id,NULL))
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"in init_ifindex_dest_ip_tree ,init_dest_ip_tree error\n");
		return -1;
	}
	
	(*root) -> ifindex 			= 	ifindex;
	(*root) -> forward_opt		=	forward_opt;
	(*root) -> forward_opt_para	=	forward_opt_para;

	return 0;
}


void forwarding_table_init() /* modify by dingkang coverity cid:11342*/
{	
	
	memset(forwarding_table,0,sizeof(forwarding_table) );

	test_forwarding_table_init_state();


}


/******************insert func***********************/



int insert_dest_ip_tree(dest_ip_tree** 	root,\
					_u32 			ip_addr,\
					int 			slot_id,\
					dest_ip_tree*	parent)
{
	if(NULL == (*root))
	{
		if(-1 == init_dest_ip_tree(root,ip_addr,slot_id,parent))
		{
			pfm_system_log(PFM_SYSTEM_LOG_FATAL,"inster_dest_ip_tree,init_dest_ip_tree() error\n");
			return -1;
		}
		
	}else{

		if(ip_addr == (*root) -> dest_ip)
		{
			pfm_system_log(PFM_SYSTEM_LOG_SECURITY,"has same dest_ip");
			return -1;
		}else if(ip_addr > (*root) -> dest_ip){

			if(-1 == insert_dest_ip_tree((dest_ip_tree**)(&(((*root) -> tree -> rc))),\
										ip_addr,\
										slot_id,\
										(*root)))
			{
				pfm_system_log(PFM_SYSTEM_LOG_FATAL,"inster_dest_ip_tree error\n");
				return -1;
			}
			
			return 0;
			
		}else if(ip_addr < (*root) -> dest_ip){
		
			if(-1 == insert_dest_ip_tree((dest_ip_tree**)(&(((*root) -> tree -> lc))),\
										ip_addr,\
										slot_id,\
										(*root)))
			{
				pfm_system_log(PFM_SYSTEM_LOG_FATAL,"inster_dest_ip_tree error\n");
				return -1;
			}
			
			return 0;
			
		}
	}
	/* no problem coverity cid:11343 */
}
			
			

int insert_ifindex_dest_ip_tree(ifindex_dest_ip_tree** 	root,\
								int 					ifindex,\
								int 					forward_opt,\
								int 					forward_opt_para,\
								_u32 					ip_addr,\
								int 					slot_id,\
								ifindex_dest_ip_tree*	parent)
{
	if(NULL == (*root))
	{
		
		if(-1 == init_ifindex_dest_ip_tree( root,\
											ifindex,\
											forward_opt,\
											forward_opt_para,\
											ip_addr,\
											slot_id,\
											parent))
		{
			pfm_system_log(PFM_SYSTEM_LOG_FATAL,"In inster_ifindex_dest_ip_tree ,init_ifindex_dest_ip_tree () error\n");
			return -1;
		}
		return 0;
	}else{

		if(ifindex == ((*root)->ifindex))
		{
			if(-1 == insert_dest_ip_tree(&((*root) -> ip_tree),ip_addr,slot_id,NULL))
			{
				pfm_system_log(PFM_SYSTEM_LOG_FATAL,"In inster-ifindex_dest_ip_tree, inster_dest_ip_tree error\n");
				return -1;
			}
			return 0;
		}
		else if(ifindex > ((*root) -> ifindex)){

			if(-1 == insert_ifindex_dest_ip_tree((ifindex_dest_ip_tree**)(&((*root) -> tree -> rc)),\
													ifindex,\
													forward_opt,\
													forward_opt_para,\
													ip_addr,\
													slot_id,\
													(*root)))
			{
				pfm_system_log(PFM_SYSTEM_LOG_INFO,"inster_ifindex_dest_ip_tree error\n");
				return -1;
			}
			return 0;
		}
		else if(ifindex < ((*root) -> ifindex)){

			if(-1 == insert_ifindex_dest_ip_tree((ifindex_dest_ip_tree**)(&((*root) -> tree -> lc)),\
													ifindex,\
													forward_opt,\
													forward_opt_para,\
													ip_addr,\
													slot_id,\
													(*root)))
			{
				pfm_system_log(PFM_SYSTEM_LOG_INFO,"inster-ifindex_dest_ip_tree error\n");
				return -1;
			}
		return 0;
		}
	}
	/* no problem coverity cid:11344 */
}
		
	

		
		
/**********************search func************************/



int look_for_ipaddr_at_dest_ip_tree(dest_ip_tree* root,int ip_addr)
{
	if(NULL == root)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"look for ip_add ,root was NULL \n");
		return 0;
	}

	if(ip_addr == root -> dest_ip)
	{
		return (root -> forward_slot);
	}

	if(ip_addr > root -> dest_ip)
	{
		return (look_for_ipaddr_at_dest_ip_tree(GET_RIGHT_CHILD_DEST_IP(root),ip_addr));
	}

	if(ip_addr < root -> dest_ip)
	{
		return (look_for_ipaddr_at_dest_ip_tree(GET_LIFT_CHILD_DEST_IP(root),ip_addr));
	}
/* no problem coverity cid:11346 */
}

/*for extend */

int hook_of_forward_opt(ifindex_dest_ip_tree* root)
{
	return NOTHING;
}



int look_for_forward_slot_at_ifindex_tree(ifindex_dest_ip_tree* root,\
										int ifindex,\
										int ip_addr)
{
	if(NULL == root)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"look for ifindex, root was NULL \n");
		return 0;
	}

	if(ifindex == root -> ifindex)
	{
		int i;
		int forward_ip;
		i = hook_of_forward_opt(root);
		if(NOTHING != i)
		{
			return i;

		}

		forward_ip = look_for_ipaddr_at_dest_ip_tree(root -> ip_tree,ip_addr);

		return forward_ip;
	}else if(ifindex > root -> ifindex){
	int temp;
	temp = look_for_forward_slot_at_ifindex_tree(GET_RIGHT_CHILD_IFINDEX(root),\
										ifindex,\
										ip_addr);
	return temp;

	}else if(ifindex < root -> ifindex){
	int temp;
	temp = look_for_forward_slot_at_ifindex_tree(GET_LIFT_CHILD_IFINDEX(root),\
										ifindex,\
										ip_addr);
	return temp;

	}
	/* no problem coverity cid:11345 */
}





/**************************************************************************
*
*
*
*                           netlink_func
*
*
*
***************************************************************************/
	int show_netlink_transport_info(netlink_transport_info* data)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"=============get netlink info=================\n");
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info opt  		%d \n",data -> opt);
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info opt_para  	%d \n",data -> opt_para);
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info protocol  	%d \n",data -> protocol);
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info ifindex  	%s \n",data -> ifindex);
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info src_port  	%d \n",data -> src_port);
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info dest_port  	%d \n",data -> dest_port);
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info src_ipaddr  	%s \n",data -> src_ipaddr);
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info dest_ipaddr  %s \n",data -> dest_ipaddr);
/*		pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info src_ipmask 	%d \n",data -> src_ipmask);*/
/*		pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info dest_ipmask  %d \n",data -> dest_ipmask);*/
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info slot  		%d \n",data -> forward_slot);
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"=============netlink info over===============\n");
		return 0;
	}

		



int init_netlink()
{
	struct sockaddr_nl saddr;
	sd = socket(AF_NETLINK, SOCK_RAW,30);
	if(-1 == sd)
	{
		pfm_system_log(PFM_SYSTEM_LOG_FATAL,"creat netlink socket error\n");
		perror("socket");
		return -1;
	}
	memset(&saddr,0,sizeof(struct sockaddr_nl)); /*init saddr because coverity cid:14322*/
	saddr.nl_family = AF_NETLINK;
	saddr.nl_pid	= getpid();
	saddr.nl_groups	= 0;

/*nl_pad field no use coverity cid:14322*/
	if(0 != bind(sd,(struct sockaddr*)&saddr, sizeof(saddr)) )
	{
		pfm_system_log(PFM_SYSTEM_LOG_FATAL,"bind saddr error\n");
		
		perror("bind:");
		return -1;
	}
	if (fcntl(sd, F_SETFL, O_NONBLOCK) < 0) 
	{
	  	pfm_system_log(PFM_SYSTEM_LOG_FATAL,"set_nonblocking(%d) failed\n", sd);
	 	return -1;
	}
	return 1;
}

int send_message(void* data,int len)
{
	struct nlmsghdr* 	nlhdr = NULL;
	struct msghdr 		msg;
	struct iovec		iov;
	struct sockaddr_nl	daddr;
	int 				ret;
	nlhdr = (struct nlmsghdr*)malloc(NLMSG_SPACE(len)); /* no problem coverity cid:13908 */

	if(NULL == nlhdr)
	{
		pfm_system_log(PFM_SYSTEM_LOG_FATAL,"send_message malloc error\n");
		return -1;
	}

	memcpy(NLMSG_DATA(nlhdr), data, len);
	memset(&msg, 0 ,sizeof(struct msghdr));
	
/*	show_netlink_transport_info(NLMSG_DATA(nlhdr));*/

	/*set nlhdr*/
	nlhdr->nlmsg_len = NLMSG_LENGTH(len);
	nlhdr->nlmsg_pid = getpid();  
	nlhdr->nlmsg_flags = 0;

	/*set daddr*/
	
	daddr.nl_family = AF_NETLINK;
	daddr.nl_pid = 0;
	daddr.nl_groups = 0;

	/*set message*/
    iov.iov_base = (void *)nlhdr;
    iov.iov_len = nlhdr->nlmsg_len;
    msg.msg_name = (void *)&daddr;
    msg.msg_namelen = sizeof(daddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

	/*send message*/
	if(sd==0) /*modfiy by dingkang*/
	{
		/*if(NULL != nlhdr)
		{
			free(nlhdr);
			nlhdr=NULL;
			return -1;
		}*/ /*this part of code can not reached coverity cid:10793*/
		return -1;
	}
	
    ret = sendmsg(sd, &msg, 0);

	pfm_system_log(PFM_SYSTEM_LOG_INFO,"sendmsg return is %d\n",ret);
	
	if (ret == -1) {
			perror("sendmsg error:");
			
	/*		if(NULL != nlhdr)
			{
				free(nlhdr);
				nlhdr=NULL;
				return -1;
			} */ /*this part of code no use*/
			return -1;
	}
	
	if(NULL != nlhdr)
	{
		free(nlhdr);
	}
	return ret;
}


int send_netlink_transport_info(int 	opt,\
								int 	opt_para,\
								_u16	protocol,\
								_u16	src_port,\
								_u16	dest_port,\
								/*struct in6_addr src_ipaddr,\
								struct in6_addr dest_ipaddr,\*/
								unsigned int src_ipaddr,
								unsigned int dest_ipaddr,
								unsigned int src_ipmask,\
								unsigned int dest_ipmask,\
								int		ifindex,\
								int		forward_slot )
{
	netlink_transport_info data;

	data.opt 	 			 = opt;
	data.opt_para 			 = opt_para;
	data.protocol 			 = protocol;
	data.src_port	 		 = src_port;
	data.dest_port	 		 = dest_port;
	data.src_ipaddr 		 = src_ipaddr;
	data.dest_ipaddr 		 = dest_ipaddr;
	data.src_ipmask 		 = src_ipmask;
	data.dest_ipmask 		 = dest_ipmask;
	data.ifindex	 		 = ifindex;
	data.forward_slot		 = forward_slot;

	pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info opt == %d!!!!!!!!!!! \n",data.opt);
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"netlink_transport_info protocol == %d!!!!!!!!!!! \n",data.protocol);

	if(-1 == send_message((void*)&data,sizeof(data)))
	{
		pfm_system_log(PFM_SYSTEM_LOG_FATAL,"send message error\n");
		return -1;
	}
	/*show_netlink_transport_info(&data);*/
	return 0;
}

int recv_netlink_transport_info()
{
	struct nlmsghdr *msg = NULL;
	struct sockaddr_nl daddr;
	int socklen = 0, rcvlen = 0, msglen = 0;
	int i;
	msglen = NLMSG_SPACE(sizeof(netlink_transport_info));
	msg = (struct nlmsghdr *)malloc(msglen);
	socklen = sizeof(struct sockaddr_nl);

	daddr.nl_family = AF_NETLINK;
	daddr.nl_pid = 0;
	daddr.nl_groups = 0;

	for(i=0;i<RECORD_MAX;i++) 
	{
		rcvlen = recvfrom(sd, msg, msglen,
							0, (struct sockaddr *)&daddr, &socklen);
		if(rcvlen != msglen) 
		{
	
			pfm_system_log(PFM_SYSTEM_LOG_FATAL,
						"recv a broken netlink message, ignored\n");
					
			if(msg)
				free(msg);
			return -1;
			
			/*continue;*/
		}
/*
		pfm_system_log(PFM_SYSTEM_LOG_INFO,
						"recv a netlink message, length = %u,i=%d\n",
						rcvlen,i);
*/		
		pfm_table[i]= *(netlink_transport_info *)NLMSG_DATA(msg);
		if(pfm_table[i].opt == -1)
		{
			pfm_system_log(PFM_SYSTEM_LOG_INFO,
						"recv num = %u\n",
						i);
			break;
		}
#if 0
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"+++++++++++++++++++proto is %d ++++++++++++++++++\n",pfm_table[i].opt);
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"opt is \t\t\t :%d\n",pfm_table[i].opt_para);
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"slot is \t\t :%d\n",pfm_table[i].forward_slot);
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"proto is \t\t :%d\n",pfm_table[i].protocol);
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"ifindex is \t\t :%d\n",pfm_table[i].ifindex);
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"d_port is \t\t :%d\n",pfm_table[i].dest_port);
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"s_port is \t\t :%d\n",pfm_table[i].src_port);
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"d_addr is \t\t :%u\n",pfm_table[i].dest_ipaddr);
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"d_mask is \t\t :%d\n",pfm_table[i].dest_ipmask);
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"s_addr is \t\t :%u\n",pfm_table[i].src_ipaddr);
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"s_mask is \t\t :%d\n",pfm_table[i].src_ipmask);
			
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"count is \t\t :%d\n",pfm_table[i].forward_opt);
			
#endif
	}

	if(msg)
		free(msg);
	return i;
}

/***************************************************************
*
*
*					delete data structure func
*
*
***************************************************************/


ifindex_dest_ip_tree* look_for_ifindex_in_ifindex_tree(ifindex_dest_ip_tree* root,int ifindex)
{
	if(NULL == root)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"loot_for_ifindex_in_ifindex_tree,can not find ifindex\n");
		return NULL;
	}

	if(ifindex == root -> ifindex)
	{
		return root;
	}

	if(ifindex > root -> ifindex)
	{
		return look_for_ifindex_in_ifindex_tree(GET_RIGHT_CHILD_IFINDEX(root),ifindex);
	}
		if(ifindex < root -> ifindex)
	{
		return look_for_ifindex_in_ifindex_tree(GET_LIFT_CHILD_IFINDEX(root),ifindex);
	}
	return NULL;
}

dest_ip_tree* look_for_dest_ip_in_dest_ip_tree(dest_ip_tree* root,_u32 dest_ip)
{

	if(NULL == root)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"look_for_dest_ip_in_dest_ip_tree,can not find dest_ip\n");
		return NULL;
	}

	if(dest_ip == root -> dest_ip)
	{
		return root;
	}

	if(dest_ip > root -> dest_ip)
	{
		return look_for_dest_ip_in_dest_ip_tree(GET_RIGHT_CHILD_DEST_IP(root),dest_ip);
	}

	if(dest_ip < root -> dest_ip)
	{
		return look_for_dest_ip_in_dest_ip_tree(GET_LIFT_CHILD_DEST_IP(root),dest_ip);
	}
	return NULL;
}
#if 0
int del_dest_ip_elem(dest_ip_tree* root,ifindex_dest_ip_tree* root_ifindex)
{
	int ret;
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"found dest_ip is %d\n",(root) -> dest_ip);
	if(((root) -> tree -> parent == NULL) && \
		((root) -> tree -> lc == NULL) && \
		((root) -> tree -> rc == NULL))
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"dest_ip_elem is only one elem\n");
		free((root) -> tree);
		free(root);
		ret = 1;
		return ret;

	}else{

		ret = 0;
	}

	if((root) -> tree -> lc == NULL)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"dest_ip_elem lc is NULL\n");
		dest_ip_tree* temp;
		temp = (root) -> tree -> parent;
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"parent is %d\n",temp -> dest_ip);
		if(GET_LIFT_CHILD_DEST_IP(temp) != NULL && \
			(GET_LIFT_CHILD_DEST_IP(temp) -> dest_ip == (root) -> dest_ip))
		{
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"del lc\n");
			(temp) -> tree -> lc = GET_RIGHT_CHILD_DEST_IP(root);

		}else{
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"del rc\n");

			(temp) -> tree -> rc = GET_RIGHT_CHILD_DEST_IP(root);
		}
				
			

		free(root -> tree);
		free(root);

		return ret;
	}else if((root) -> tree -> rc == NULL){

		pfm_system_log(PFM_SYSTEM_LOG_INFO,"dest_ip_elem rc is NULL \n");
		dest_ip_tree* temp;

		temp = (root) -> tree -> parent;
		if(GET_LIFT_CHILD_DEST_IP(temp) != NULL && \
			(GET_LIFT_CHILD_DEST_IP(temp) -> dest_ip == (root) -> dest_ip))
		{
			
			(temp) -> tree -> lc = GET_LIFT_CHILD_DEST_IP(root);

		}else{

			(temp) -> tree -> rc = GET_LIFT_CHILD_DEST_IP(root);
		}

		free(temp -> tree);
		free(temp);

		return (ret);
	}else{

		pfm_system_log(PFM_SYSTEM_LOG_INFO,"dest_ip_elem rc and lc both no NULL\n");
		dest_ip_tree* temp;
		dest_ip_tree* temp2;


		temp 	= root;
		temp2	= GET_RIGHT_CHILD_DEST_IP(root);
		if(GET_PARENT_DEST_IP(root) == NULL)
		{
			ifindex_dest_ip_tree* temp3;
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"root is we will del\n");
			temp3 	= root_ifindex;
			temp3 -> ip_tree	= GET_RIGHT_CHILD_DEST_IP(root);
		
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"root ifindex is %d\n",temp3 -> ifindex);
		}else{
			dest_ip_tree* temp3;
			if(GET_LIFT_CHILD_DEST_IP(temp3) != NULL && \
				(GET_LIFT_CHILD_DEST_IP(temp3) -> dest_ip == (root) -> dest_ip))
			{
				
				(temp3) -> tree -> lc = GET_RIGHT_CHILD_DEST_IP(root);

			}else{

				(temp3) -> tree -> rc = GET_RIGHT_CHILD_DEST_IP(root);
			}
		}


		
		while(temp2 -> tree -> lc != NULL)
		{
			temp2 = (dest_ip_tree*)(temp2 -> tree -> lc);
		}
		temp2 -> tree -> lc = temp -> tree -> lc;

		free(temp -> tree);
		free(temp);
		return ret;
	}
	return ret;
}


int del_ifindex_tree_elem(ifindex_dest_ip_tree* root,ifindex_dest_ip_tree** forwarding_table)
{
	#if 1
	
	int ret;
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"found ifindex is %d\n",(root) -> ifindex);
	if(((root) -> tree -> parent == NULL) && \
		((root) -> tree -> lc == NULL) && \
		((root) -> tree -> rc == NULL))
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"ifindex_elem is only one elem\n");
		free((root) -> tree);
		free(root);
		ret = 1;
		return ret;

	}else{

		ret = 0;
	}

	if((root) -> tree -> lc == NULL)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"ifindex_elem lc is NULL\n");
		ifindex_dest_ip_tree* temp;
		temp = (root) -> tree -> parent;
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"parent is %d\n",temp -> ifindex);
		if(GET_LIFT_CHILD_IFINDEX(temp) != NULL && \
			(GET_LIFT_CHILD_IFINDEX(temp) -> ifindex == (root) -> ifindex))
		{
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"del lc\n");
			(temp) -> tree -> lc = GET_RIGHT_CHILD_IFINDEX(root);

		}else{
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"del rc\n");

			(temp) -> tree -> rc = GET_RIGHT_CHILD_IFINDEX(root);
		}
				
			

		free(root -> tree);
		free(root);

		return ret;
	}else if((root) -> tree -> rc == NULL){

		pfm_system_log(PFM_SYSTEM_LOG_INFO,"ifindex_elem rc is NULL \n");
		ifindex_dest_ip_tree* temp;

		temp = (root) -> tree -> parent;
		if(GET_LIFT_CHILD_IFINDEX(temp) != NULL && \
			(GET_LIFT_CHILD_IFINDEX(temp) -> ifindex == (root) -> ifindex))
		{
			
			(temp) -> tree -> lc = GET_LIFT_CHILD_IFINDEX(root);

		}else{

			(temp) -> tree -> rc = GET_LIFT_CHILD_IFINDEX(root);
		}

		free(temp -> tree);
		free(temp);

		return (ret);
	}else{

		pfm_system_log(PFM_SYSTEM_LOG_INFO,"ifindex_elem rc and lc both no NULL\n");
		ifindex_dest_ip_tree* temp;
		ifindex_dest_ip_tree* temp2;


		temp 	= root;
		temp2	= GET_RIGHT_CHILD_IFINDEX(root);
		if(GET_PARENT_IFINDEX(root) == NULL)
		{
			ifindex_dest_ip_tree** temp3;
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"root is we will del\n");
			temp3 	= forwarding_table;
			(*temp3)= GET_RIGHT_CHILD_IFINDEX(root);
		
		}else{
			ifindex_dest_ip_tree* temp3;
			if(GET_LIFT_CHILD_IFINDEX(temp3) != NULL && \
				(GET_LIFT_CHILD_IFINDEX(temp3) -> ifindex == (root) -> ifindex))
			{
				
				(temp3) -> tree -> lc = GET_RIGHT_CHILD_IFINDEX(root);

			}else{

				(temp3) -> tree -> rc = GET_RIGHT_CHILD_IFINDEX(root);
			}
		}


		
		while(temp2 -> tree -> lc != NULL)
		{
			temp2 = (ifindex_dest_ip_tree*)(temp2 -> tree -> lc);
		}
		temp2 -> tree -> lc = temp -> tree -> lc;

		free(temp -> tree);
		free(temp);
		return ret;
	}
	return ret;
	#else
	int ret;
	if(((*root) -> tree -> parent == NULL) && \
		((*root) -> tree -> rc == NULL) && \
		((*root) -> tree -> lc == NULL))
	{
		
		free((*root) -> tree);
		free(*root);
		ret = 1;
		return ret;
		
	}else{
	
		ret = 0;
		
	}

	
	if((*root) -> tree -> lc == NULL)
	{
		ifindex_dest_ip_tree* temp;
		temp = (*root);
		(*root) = (ifindex_dest_ip_tree*)((*root) -> tree -> rc);

		free(temp -> tree);
		free(temp);

		return ret;
	}else if((*root) -> tree -> rc == NULL){

		ifindex_dest_ip_tree* temp;
		temp = (*root);
		(*root) = (ifindex_dest_ip_tree*)((*root) -> tree -> lc);

		free(temp -> tree);
		free(temp);

		return (ret);
	}else{

		ifindex_dest_ip_tree* temp;
		ifindex_dest_ip_tree* temp2;
		temp = temp2 = (*root);
		(*root) = (ifindex_dest_ip_tree*)((*root) -> tree -> rc);
		while(temp2 -> tree -> lc != NULL)
		{
			temp2 = (ifindex_dest_ip_tree*)(temp2 -> tree -> lc);
		}
		temp2 -> tree -> lc = temp -> tree -> lc;

		free(temp -> tree);
		free(temp);
		return ret;
	}
	return ret;
	#endif
}
#endif
int hook_of_del_opt(ifindex_dest_ip_tree** root,\
					int opt,\
					int opt_para)
{
	return 0;
}

/*
int del_dest_ip_in_ifindex_tree(ifindex_dest_ip_tree** root,\
								int ifindex,\
								int	opt,\
								int opt_para,\
								_u32 dest_ip)
{
	ifindex_dest_ip_tree *temp_ifindex;
	dest_ip_tree *temp_dest_ip;
//	dest_ip_tree **temp;
	
	if(NULL == (*root))
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"del_dest_ip_in_ifindex_tree, root is NULL\n");
		return -1;
	}
	//look_for_ifindex_in_ifindex_tree
	#if 1

	temp_ifindex = look_for_ifindex_in_ifindex_tree((*root),ifindex);

	
	if(NULL == temp_ifindex)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"del_dest_ip_in_ifindex_tree, can not find ifindex in tree");
		return -1;
	}
	#else

	for(temp_ifindex = (*root);temp_ifindex -> ifindex != ifindex;)
	{
		if(ifindex > temp_ifindex)
		{
			temp_ifindex = GET_LIFT_CHILD_IFINDEX(temp_ifindex);
		}
		if(ifindex < temp_ifindex)
		{
			temp_ifindex = GET_RIGHT_CHILD_IFINDEX(temp_ifindex);
		}
		if(NULL == temp_ifindex)
		{
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"can not find ifindex\n");
			return 0;
		}
	}


	#endif
	

	if(hook_of_del_opt((temp_ifindex),opt,opt_para))
	{
		return 0;
	}
//lookfor dest_ip
	#if 1
	temp_dest_ip = look_for_dest_ip_in_dest_ip_tree((temp_ifindex) -> ip_tree,dest_ip);
	if(NULL == (temp_dest_ip))
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"del_dest_ip_in_ifindex_tree, can not find dest_ip in tree");
		return -1;
	}
	#else
	
	for(temp_dest_ip = (temp_ifindex -> ip_tree);temp_dest_ip -> dest_ip != dest_ip;)
	{
		if(dest_ip > temp_dest_ip -> dest_ip)
		{
			temp_dest_ip = GET_LIFT_CHILD_IFINDEX(temp_dest_ip);
		}
		if(dest_ip < temp_dest_ip -> dest_ip)
		{
			temp_ifindex = GET_RIGHT_CHILD_IFINDEX(temp_ifindex);
		}
		if(NULL == temp_ifindex)
		{
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"can not find ifindex\n");
			return 0;
		}
	}
	#endif
	//temp = &temp_dest_ip;
	if(del_dest_ip_elem(temp_dest_ip,temp_ifindex))
	{	
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"dest_ip_tree is NULL ,we will del ifindex tree\n");

		if(del_ifindex_tree_elem(temp_ifindex,root))
		{
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"ifindex_tree_elem is NULL, we will make forwarding table is null\n");
			(*root) = NULL;
		}else{
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"ifindex_tree_elem is not null,del it\n");
		}
	}else{

		pfm_system_log(PFM_SYSTEM_LOG_INFO,"dest_ip_tree is not null ,del it\n");
	}
	return 0;
}
#endif
*/
/***********************************************************************************
*
*
*								dbus_func
*
*
***********************************************************************************/
static int ifname2ifindex_by_ioctl(const char *dev)
#if 0
{
	struct ifreq ifr;
	int fd=0;
	int err;

	memset(&ifr,0,sizeof(ifr));
	strncpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd<=0)  //add by dingkang coverity cid:11469
		return 0;
	err = ioctl(fd, SIOCGIFINDEX, &ifr);
	if (err) 
	{
		close(fd);
		return 0;
	}
	close(fd);
	return ifr.ifr_ifindex;
}
#endif

{
	struct ifreq ifr;
	int fd;
	int err;
	char dev_temp[16];
	memset(dev_temp,0,16);
	if((!strncmp(dev,"ve",2)) && (strlen(dev) <= 3)) /*dev such as ve2*/
	{
		int slot=0;
		if(1!=sscanf(dev,"ve%d",&slot))
		{
			return -1;
		}
		sprintf(dev_temp,"ve0%df1",slot);
	}else{
		sprintf(dev_temp,"%s",dev);
	}
	printf("%s\n",dev_temp);

	memset(&ifr,0,sizeof(ifr));
	strncpy(ifr.ifr_name, dev_temp, sizeof(ifr.ifr_name));
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd<=0)  /*add by dingkang coverity cid:11469*/
		return 0;
	err = ioctl(fd, SIOCGIFINDEX, &ifr);
	if (err) 
	{
		close(fd);
		return 0;
	}
	close(fd);
	return ifr.ifr_ifindex;
}




#if 1
	int pfm_dbus_thread_main()
	{
		
		pfm_dbus_init();
		while(dbus_connection_read_write_dispatch(pfm_dbus_connection,-1)){
			;
		}
		return -1;
	}

#if 0
int parse_ipv4_and_ipv6_address(char* ip_str, struct in6_addr* ipaddr, unsigned int *ipmask, int opt)
{
	char* save = malloc(strlen(ip_str) +1);
	char* token = NULL;
	char* sep = "/";
	int ret;
	
	memset(save, 0, strlen(save) + 1);
	memcpy(save, ip_str, strlen(save));
	
	token = strtok(save, sep);
	pfm_system_log(PFM_SYSTEM_LOG_FATAL,"token == %s\n", token);
	opt = strlen(save);
	if (opt == 0 || opt == 1 ) {
		
		ret = inet_pton(AF_INET, token, (ipaddr->s6_addr32)[0]);
		if (ret <=  0){
			pfm_system_log(PFM_SYSTEM_LOG_FATAL,"the ipaddress is NULL or invalid\n");
			return ret;
		}
	} else if (opt == 11 || opt == 12) {
	
		ret = inet_pton(AF_INET6, token, ipaddr->s6_addr32);
		if (ret <= 0){
			pfm_system_log(PFM_SYSTEM_LOG_FATAL,"the ipaddress is NULL or invalid\n");
			return ret;
		}		
	}
	

	token = strtok(NULL, sep);
	*ipmask = atoi(token);
	if (*ipmask < 0 ||*ipmask > 24){
		pfm_system_log(PFM_SYSTEM_LOG_FATAL,"the ipmask is invalid\n");
		return -1;
	}
		
	return 0;
}

#endif

#define DCLI_IPMASK_STRING_MAXLEN	(strlen("255.255.255.255/32"))
#define DCLI_IPMASK_STRING_MINLEN	(strlen("0.0.0.0/0"))
#define DCLI_IP_STRING_MAXLEN	(strlen("255.255.255.255"))
#define DCLI_IP_STRING_MINLEN   (strlen("0.0.0.0"))

unsigned long ip2ulong(char *str)
{
	char *sep=".";
	char *token = NULL;
	unsigned long ip_long[4]; 
	unsigned long ip = 0;
	int i = 1;
	
	token=strtok(str,sep);
	if(NULL != token){
	    ip_long[0] = strtoul(token,NULL,10);
	}
	while((token!=NULL)&&(i<4))
	{
		token=strtok(NULL,sep);
		if(NULL != token){
		    ip_long[i] = strtoul(token,NULL,10);
		}
		i++;
	}

	ip=(ip_long[0]<<24)+(ip_long[1]<<16)+(ip_long[2]<<8)+ip_long[3];

	return ip;
}


int str2ulong(char *str,unsigned int *Value)
{
	char *endptr = NULL;
	char c = 0;
	int ret = 0;
	if (NULL == str) return -1;

	ret = checkPoint(str);
	if(ret == 1){
		return -1;
	}

	c = str[0];
	if((strlen(str) > 1)&&('0' == c)){
		/* string(not single "0") should not start with '0'*/
		return -1;
	}		
	*Value= strtoul(str,&endptr,10);
	if('\0' != endptr[0]){
		return -1;
	}
	return 0;	

}

int parse_ip_check(char * str)
	{
	
		char *sep=".";
		char *token = NULL;
		unsigned long ip_long[4] = {0}; 
		int i = 0;
		int pointCount=0;
		char ipAddr[DCLI_IP_STRING_MAXLEN+1]={0};
		if(str==NULL||strlen(str)>DCLI_IP_STRING_MAXLEN || \
			strlen(str) < DCLI_IP_STRING_MINLEN ){
			return -1;
		}
		if((str[0] > '9')||(str[0] < '1')){
			return -1;
		}
		for(i=0;i<strlen(str);i++){
			ipAddr[i]=str[i];
			if('.' == str[i]){
                pointCount++;
				if((i == strlen(str)-1)||('0' > str[i+1])||(str[i+1] > '9')){
					return -1;
				}
			}
			if((str[i]>'9'||str[i]<'0')&&str[i]!='.'&&str[i]!='\0'){
				return -1;
			}
		}
		if(3 != pointCount){
            return -1;
		}
		token=strtok(ipAddr,sep);
		if((NULL == token)||(strcmp(token,"")==0)||(strlen(token) < 1)||\
			((strlen(token) > 1) && ('0' == token[0]))){ 
			return -1;
		}/*coverity modify for CID 10049 */
		if(NULL != token){
		    ip_long[0] = strtoul(token,NULL,10);
		}
	/*	else {
			return -1;
		}
		*//*coverity modify for CID 10543 */
		i=1;
		while((token!=NULL)&&(i<4))
		{
			token=strtok(NULL,sep);
			if((NULL == token)||(strcmp(token,"")==0)||(strlen(token) < 1)|| \ 
				((strlen(token) > 1) && ('0' == token[0]))){
				return -1;
			}
			if(NULL != token){
			    ip_long[i] = strtoul(token,NULL,10);
			}
			else {
				return -1;
			}
			i++;
		}
		for(i=0;i<4;i++){
            if(ip_long[i]>255){
                return -1;
			}
		}
		return 0;
		
}



int checkPoint(char *ptr_)
{
	int ret = 0;
	char* ptr=NULL;
	if(NULL == ptr_) /*modify by dingkang coverity cid:11560 #1*/
		return 1;
	else
		ptr=ptr_;
	
	while(*ptr != '\0') {
		if(((*ptr) < '0')||((*ptr) > '9')){
			ret = 1;
	 		break;
		}
		ptr++;
	}
	return ret;
}


int ip_str2ulong(char *str,unsigned int *Value)
{
	char *endptr = NULL;
	char c = 0;
	int ret = 0;
	if (NULL == str) 
		return -1;

	ret = checkPoint(str);
	if(ret == 1){
		return -1;
	}

	c = str[0];
	if((strlen(str) > 1)&&('0' == c)){
		/* string(not single "0") should not start with '0'*/
		return -1;
	}		
	*Value= strtoul(str,&endptr,10);
	if('\0' != endptr[0]){
		return -1;
	}
	return 0;	
}



int	ip_address_format2ulong(char ** buf,unsigned long *ipAddress,unsigned int *mask,int opt)
{
	char *strcp = NULL;
	char *split = "/";
	char *token1 = NULL,*token2 = NULL;
	int Val = 0,ret = 0;
	unsigned int ipMask = 0;
	int i = 0, length = 0;
	int splitCount=0;
	char * str = NULL;
	if(opt == 10 || opt == 11)
	{
		*ipAddress = 0;
		*mask = 0;
		return 0;
	}
	
	if(NULL == buf||NULL == *buf){ /*add by dingkang coverity cid:13862 #1 */
			return -1;
		}

	
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"in ip_address_format2ulong buf is %s\n\n",*buf);

	if (!strcmp (*buf, "all")) {
		*ipAddress = 0;
		*mask = 0;
		return 0;
	}
/*	
	if(NULL == buf||NULL == *buf){
        return -1;
	}	*/
	str = *buf;
	if(NULL == str)
		{
		return -1 ;
		}
	length = strlen(str);
	if( length > DCLI_IPMASK_STRING_MAXLEN ||  \
		length < DCLI_IP_STRING_MINLEN ){
		return -1;
	}
	if((str[0] > '9')||(str[0] < '1')){
		return -1;
	}
	for(i = 0; i < length; i++){
		if('/' == str[i]){
            splitCount++;
			if((i == length - 1)||('0' > str[i+1])||(str[i+1] > '9')){
                return -1;
			}
		}
		if((str[i] > '9'||str[i]<'0') &&  \
			str[i] != '.' &&  \
			str[i] != '/' &&  \
			str[i] != '\0'
		){
			return -1;
		}
	}
	if(1 != splitCount){

	strcp=(char *)malloc(50*sizeof(char));
	if (NULL == strcp) {
		return -1;
	}
		memset(strcp,'\0',50);
	strcpy(strcp,str);	
	strcat(strcp,"/0");
	
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"str is :%s\n",strcp);
	}else
	{
		
		strcp=(char *)malloc(50*sizeof(char));
		if (NULL == strcp) {
			return -1;
		}
			memset(strcp,'\0',50);
	strcpy(strcp,str);	
	}


	token1 = strtok(strcp,split);
	token2 = token1;/*token2 is ip string*/
	if(NULL != token1) {
		token1 = strtok(NULL,split);/*token1 is mask string*/
		ret = str2ulong(token1,&ipMask);
		if (-1 == ret) {
			free(strcp);
			strcp = NULL;
			return -1;
		}
		if(ipMask>32) /*modify by dingkang coverity cid:11584 #1*/
		{
			free(strcp);
			strcp = NULL;
			return -1;	
		}
		else
		{
			*mask = ipMask;
		}
	}
	
	if(0 != parse_ip_check(token2)){
		free(strcp);
		strcp = NULL;
		return -1;
	}
    *ipAddress = ip2ulong(token2);	
	free(strcp);
	strcp = NULL;
	return 0;
}


int is_subinterface_qinq(char* ifname)
{
	if(NULL == ifname)
		return 0;
	int ifname_num=0;
	int ret=0;
	ifname_num=strlen(ifname);
	for(ifname_num--;ifname_num>=0;ifname_num--)
	{
		if(ifname[ifname_num]=='.')
		{
			ret++;
		}
	}

	return ret;
}

char* get_interface_of_subinterface(char* ifname)
{
	char* interface=NULL;
	int i=0;
	int ifname_num=0;
	
	if(NULL == ifname)
		return NULL;
	ifname_num=strlen(ifname); /*add by dingkang coverity cid:13861*/
	interface=malloc(32);
	if(interface==NULL)
		return NULL;
	memset(interface,0,32);
	
	for(i=0;i<ifname_num;i++)
	{
		if(ifname[i] != '.')
		{
			interface[i]=ifname[i];
		}else{
			return interface;
		}
	}
	return interface;
}
char* get_vidch_of_subinterface(char* ifname)
{
	char* interface=NULL;
	int i=0;
	int temp=0;/*init it coverity cid:14321*/
	if(NULL == ifname)
		return NULL;
	int ifname_num=strlen(ifname);
	interface=malloc(32);
	if(interface==NULL)
		return NULL;
	memset(interface,0,32);
	
	for(i=0;i<ifname_num;i++)
	{
		if(ifname[i] != '.')
		{
			temp++;
		}else{
		goto out;	
		}
	}
out:
	memcpy(interface,&ifname[i+1],(ifname_num-(i+1)));/*modify by dingkang coverity cid:10849*/
	return interface;
}

int get_ifindex_of_subinterface(char* ifname)
{
	char *vid=NULL;
	char *interface=NULL;
	int ret=-1;
	int vid1=0;
	int vid2=0;
	
	if(NULL == ifname)
		return 0;
		
	if(!strncmp(ifname,"all",strlen("all")))
	{
		return 0;
	}
	
	vid=get_vidch_of_subinterface(ifname);
	if(NULL == vid)/*modify by dingkang coverity cid:13504*/
		return -1;
	ret=is_subinterface_qinq(ifname);
	interface=get_interface_of_subinterface(ifname);
	if(NULL == interface){
		free(vid);
		return -1;
	}
	switch(ret)
	{
		case 0:
		/*	if(product_serial == 7)
			if(0)
			if(
				ret=ifname2ifindex_by_ioctl("obc0");
			else*/
				ret=ifname2ifindex_by_ioctl(interface);
			goto out;
		case 1:
			sscanf(vid,"%d",&vid1);
			if(product_serial == 7)
				ret=ifname2ifindex_by_ioctl("obc0");
			else
				ret=ifname2ifindex_by_ioctl(interface);
			vid1=vid1<<8;
			ret+=vid1;
			goto out;
		case 2:
			sscanf(vid,"%d.%d",&vid2,&vid1);
			vid2=vid2<<8;
			vid1=vid1<<(12+8);
			if(product_serial == 7)
				ret=ifname2ifindex_by_ioctl("obc0");
			else
				ret=ifname2ifindex_by_ioctl(interface);
			ret=ret+vid1+vid2;
			goto out;
		default:
			ret=0;
	}
	out:
		free(vid);
		free(interface);
		return ret;
	
}


#if 1
static int service_ssh_enable(void)
{
	char cmd[128];
	int ret;
	memset(cmd,0,128);
	sprintf(cmd,"sudo /etc/init.d/ssh restart > /dev/null 2> /dev/null");
	ret = system(cmd);
	if(WEXITSTATUS(ret)== 0)
	{
		return 0;
	}
	else
	{
		pfm_system_log(PFM_SYSTEM_LOG_FATAL,"SSH,some thing is Wrong when enable.\n");
		return 1;
	}
}


static int service_ssh_disable(void)
{
	
	char cmd[128];
	int ret;
	memset(cmd,0,128);
	sprintf(cmd,"ps -ef | grep \"sshd:\" | grep -v \"sh -c ps -ef | grep\" | grep -v \"grep sshd:\"");
	ret = system(cmd);
	if(WEXITSTATUS(ret)== 0)
	{
		pfm_system_log(PFM_SYSTEM_LOG_INFO,"SSH can not be shut down because someone has logged into the system using it. If you want, please use the 'kick user' command to kick the user off first.\n");
		return 2;
	}
	memset(cmd,0,128);
	sprintf(cmd,"sudo /etc/init.d/ssh stop > /dev/null 2> /dev/null");
	ret = system(cmd);
	if(WEXITSTATUS(ret)== 0)
	{
		return 0;
	}else{
		pfm_system_log(PFM_SYSTEM_LOG_FATAL,"SSH,some thing is Wrong when disable.\n");
		return 1;
	}
	
}


static int service_telnet_enable(void)
{
   pfm_system_log(PFM_SYSTEM_LOG_INFO,"To enable inetd for Telnet.\n");
  /* system("pkill inetd;sudo inetd >/dev/null 2>/dev/null");*/
   char cmd[128];
   int ret;
   memset(cmd,0,128);
   sprintf(cmd,"pkill inetd;sudo inetd >/dev/null 2>/dev/null");
   system(cmd);
   return 0;
}

static int service_telnet_dsiable(void)
{
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"To disable inetd for Telnet.\n");
	/*system("pkill inetd 2>/dev/null");*/
	char cmd[128];
	memset(cmd,0,128);
	sprintf(cmd,"pkill inetd;pkill telnet 2>/dev/null");
	system(cmd);
	return 0;
}

#endif


DBusMessage *
pfm_parse_service_message(DBusConnection	*conn,\
				DBusMessage  *msg,\
   				void		*user_data  )
{
		DBusMessage 		*reply = NULL;
		DBusMessageIter iter;
		DBusMessageIter iter_array;
		DBusMessageIter iter_struct;
		DBusError				err;
		
		
		unsigned int opt_ret = 0;

		char *service=NULL;
		int slot = -1;
		char *operation = NULL;

		dbus_error_init(&err);
		if(!(dbus_message_get_args(msg,&err,
							DBUS_TYPE_STRING, &service,
							DBUS_TYPE_INT32, &slot,
							DBUS_TYPE_STRING, &operation,
							DBUS_TYPE_INVALID)))
 
		
		{
			pfm_system_log(PFM_SYSTEM_LOG_FATAL,"Unable to get input args\n");
			if (dbus_error_is_set(&err)){
				pfm_system_log(PFM_SYSTEM_LOG_FATAL,"%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		}
		
		#if 1
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"PFM recv data from DCLI are :\n");
			pfm_system_log(PFM_SYSTEM_LOG_INFO," service   is [%s] ....\n",service);
			pfm_system_log(PFM_SYSTEM_LOG_INFO," slot      is [%d] ....\n",slot);
			pfm_system_log(PFM_SYSTEM_LOG_INFO," operation is [%s] ....\n",operation);
        #endif

		if(strncmp(service,"telnet",6)==0)
		 {
            /*deal telnet service*/
			if(strncmp(operation,"enable",6)==0)
			 {
			   opt_ret = service_telnet_enable();
			  }
			else if(strncmp(operation,"disable",7)==0)
			 {
			   opt_ret = service_telnet_dsiable();
			   }
			else
			{
			  pfm_system_log(PFM_SYSTEM_LOG_INFO," Unkown serivce telnet operation[%s]!\n",operation);
			  opt_ret = 1;
			  goto done;
			  }
		   }
		else if(strncmp(service,"ssh",3)==0)
		 {
			/*deal ssh service*/
			if(strncmp(operation,"enable",6)==0)
			 {
			   opt_ret = service_ssh_enable();
			  }
			else if(strncmp(operation,"disable",7)==0)
			 {
			   opt_ret = service_ssh_disable();
			   }
			else
			{
			  pfm_system_log(PFM_SYSTEM_LOG_INFO," Unkown serivce ssh operation[%s]!\n",operation);
			  opt_ret = 1;
			  goto done;
			  }
		   }
		else
		 {
			pfm_system_log(PFM_SYSTEM_LOG_INFO," Unkown serivce [%s]!\n",service);
			opt_ret = 1;
			goto done;
		 
			}
			
		
		done:
		reply = dbus_message_new_method_return(msg);
		dbus_message_iter_init_append(reply,&iter);
		dbus_message_iter_append_basic(&iter,DBUS_TYPE_UINT32, &opt_ret);
		
		return reply;
		
}	


DBusMessage *
pfm_parse_message(DBusConnection	*conn,\
				DBusMessage  *msg,\
   				void		*user_data  )
{
		DBusMessage 		*reply = NULL;
		DBusMessageIter iter;
		DBusMessageIter iter_array;
		DBusMessageIter iter_struct;
		DBusError				err;
		
		char* ifname;
		int ifindex=0;/*init it coverity cid:14324*/
		unsigned int src_port, dest_port;
		char*  src_ipaddr = NULL;
		char* dest_ipaddr = NULL;
		unsigned int s_ipmask = 0, d_ipmask = 0;
		unsigned short protocol;/**tcp : 6, udp :17**/
		int slot;
		/*struct in6_addr s_ipaddr ;
		struct in6_addr d_ipaddr ;*/
		int  s_ipaddr=0;/*init it coverity cid:14325*/
		int  d_ipaddr=0;/*init it coverity cid:14323*/
		int opt;
		int forward_opt;
		int forward_opt_para;
		int opt_para;
		int i;
		unsigned int opt_ret = 1;
		int ret;
	
		/*showstr = (unsigned char*)malloc(1024);
		  memset(showstr,0,1024);*/
		dbus_error_init(&err);
		if(!(dbus_message_get_args(msg,&err,
							DBUS_TYPE_INT32, &opt,
							DBUS_TYPE_INT32, &opt_para,
							DBUS_TYPE_UINT16, &protocol,
							DBUS_TYPE_STRING, &ifname,
							DBUS_TYPE_UINT32, &src_port,
							DBUS_TYPE_UINT32, &dest_port,
							DBUS_TYPE_STRING, &src_ipaddr,							
							DBUS_TYPE_STRING, &dest_ipaddr,
							DBUS_TYPE_INT32, &slot,
							DBUS_TYPE_INVALID)))
 
		
		{
			pfm_system_log(PFM_SYSTEM_LOG_FATAL,"Unable to get input args\n");
			if (dbus_error_is_set(&err)){
				pfm_system_log(PFM_SYSTEM_LOG_FATAL,"%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		}
/*		{
			memcpy(&src_ipaddr,&src_ipaddr1,sizeof(src_ipaddr1));
			memcpy(&src_ipaddr + sizeof(src_ipaddr1),&src_ipaddr2,sizeof(src_ipaddr2));
			memcpy(&dest_ipaddr,&dest_ipaddr1,sizeof(dest_ipaddr1));
			memcpy(&dest_ipaddr + sizeof(dest_ipaddr1),&dest_ipaddr2,sizeof(dest_ipaddr2));
		}*/
		
		#if 1
			pfm_system_log(PFM_SYSTEM_LOG_INFO,"PFM recv data from DCLI are :\n");
			pfm_system_log(PFM_SYSTEM_LOG_INFO," opt is %d ....\n",opt);
			pfm_system_log(PFM_SYSTEM_LOG_INFO," protocol is %u ....\n",protocol);
			pfm_system_log(PFM_SYSTEM_LOG_INFO," ifindex is %s ....\n",ifname);
			pfm_system_log(PFM_SYSTEM_LOG_INFO," port is %u ....\n",dest_port);
			pfm_system_log(PFM_SYSTEM_LOG_INFO," slot is %d ....\n",slot);
			pfm_system_log(PFM_SYSTEM_LOG_INFO," opt_para is %d ....\n",opt_para);
			pfm_system_log(PFM_SYSTEM_LOG_INFO," ipaddr is %s ....\n",src_ipaddr);
			pfm_system_log(PFM_SYSTEM_LOG_INFO," ipaddr is %s ....\n",dest_ipaddr);
        #endif

	/*	memset(s_ipaddr, 0, sizeof(s_ipaddr));
		memset(d_ipaddr, 0, sizeof(d_ipaddr));		*/
		
        if(opt == ADD || opt == DEL || opt == 11 || opt ==10)
        {
				char *src_ipaddr_temp = NULL;
				char *dest_ipaddr_temp = NULL;
				char *ifindex_temp_ = NULL;
        		char ve_num[16] = "";
        		int vlan_id = 0;
				
				if(src_ipaddr == NULL || dest_ipaddr == NULL || ifname == NULL)
					goto error;
				if((strlen(src_ipaddr) > 256) || (strlen(dest_ipaddr) > 256) ||(strlen(ifname) > 256))
				{
					pfm_system_log(PFM_SYSTEM_LOG_INFO,"ip addr and ifname strlen too long\n");
					goto error;
				}

				src_ipaddr_temp=malloc(strlen(src_ipaddr)+1);
				if(NULL == src_ipaddr_temp) /*modify by dingkang coverity cid:13505*/
				{
					pfm_system_log(PFM_SYSTEM_LOG_INFO,"malloc error\n");
					goto error;
				}
				dest_ipaddr_temp=malloc(strlen(dest_ipaddr)+1);
				
				if(NULL == dest_ipaddr_temp)/*modify by dingkang coverity cid:13506*/
				{
					pfm_system_log(PFM_SYSTEM_LOG_INFO,"malloc error\n");
					free(src_ipaddr_temp);
					goto error;
				}
				ifindex_temp_=malloc(strlen(ifname)+1);

				if(NULL == ifindex_temp_)/*modify by dingkang coverity cid:13507*/
				{
					pfm_system_log(PFM_SYSTEM_LOG_INFO,"malloc error\n");
					free(src_ipaddr_temp);
					free(dest_ipaddr_temp);
					goto error;
				}

				memset(src_ipaddr_temp,0,strlen(src_ipaddr)+1);
				memset(dest_ipaddr_temp,0,strlen(dest_ipaddr)+1);
				memset(ifindex_temp_,0,strlen(ifname)+1);

				strcpy(src_ipaddr_temp,src_ipaddr);
				strcpy(dest_ipaddr_temp,dest_ipaddr);
				strcpy(ifindex_temp_,ifname);

				
				if(0 != ip_address_format2ulong((char **)&src_ipaddr_temp,(unsigned long *)&s_ipaddr,&s_ipmask,opt))/**check the ip address is correct **/
            	{
					pfm_system_log(PFM_SYSTEM_LOG_INFO,"ip address is wrong\n");
					
					free(src_ipaddr_temp);
					free(dest_ipaddr_temp);
					free(ifindex_temp_);
					goto error;
              	}
            	if(0 != ip_address_format2ulong((char **)&dest_ipaddr_temp,(unsigned long *)&d_ipaddr,&d_ipmask,opt))/**check the ip address is correct **/
            	{
					pfm_system_log(PFM_SYSTEM_LOG_INFO,"ip addresss is wrong\n");
					
					free(src_ipaddr_temp);
					free(dest_ipaddr_temp);
					free(ifindex_temp_);
					goto error;

              	}
              	/*
				pfm_system_log(PFM_SYSTEM_LOG_INFO,"########################\n");
				*/

			#if 0
				ret = parse_ipv4_and_ipv6_address(src_ipaddr, &s_ipaddr, &s_ipmask, opt);
				if (ret < 0) {
					pfm_system_log(PFM_SYSTEM_LOG_FATAL,"init_tree_elem,malloc error\n");
					return -1;
				}
				
				ret = parse_ipv4_and_ipv6_address(dest_ipaddr, &d_ipaddr, &d_ipmask, opt);
				if (ret < 0) {
					pfm_system_log(PFM_SYSTEM_LOG_FATAL,"init_tree_elem,malloc error\n");
					return -1;
				}	
			#endif	

				
        		/*if dev is ve subinterface ,ifindex = (ifindex of obc0) * 4096 + vid*/
			/*
        		if(sscanf(ifname,"ve%s.%d",ve_num,&vlan_id))
        		{
        			if(product_serial == 7)
        			{
        				int ifindex_temp = 0 ;
        				ifindex_temp = ifname2ifindex_by_ioctl("obc0");
        				if(ifindex_temp > 1040000)
        				{
        					opt_ret = -1;
        					goto error;
        				}
        				ifindex = ifindex_temp * 4096;
        				ifindex += vlan_id;
        			}
        			else if(product_serial == 8)
        			{
        				int ifindex_temp = 0;
        				char temp[128];
        				memset(temp,0,128);
        				sprintf(temp,"ve%s",ve_num);
						fprintf(stderr,"temp is :%s func:%s line:%d\n",temp,__func__,__LINE__);
        				ifindex_temp = ifname2ifindex_by_ioctl(temp);
						
						fprintf(stderr,"ifindex_temp is :%d func:%s line:%d\n",ifindex_temp,__func__,__LINE__);
        				if(ifindex_temp > 1040000)
        				{
        					opt_ret = -1;
        					goto error;
        				}
        				ifindex = ifindex_temp * 4096;
        				ifindex += vlan_id;
        			}
        			
        		}else{
					if (!strcmp (ifindex_temp_, "all")) {
						ifindex = 0;
					}else{
      		  			ifindex = ifname2ifindex_by_ioctl(ifname);
					}
        		}

				*/

				
				free(src_ipaddr_temp);
				free(dest_ipaddr_temp);
				free(ifindex_temp_);
				ifindex=get_ifindex_of_subinterface(ifname);
				if(-1 ==ifindex)	
					goto error;
				/*
				if(opt == 11 || opt ==10)
				{
					protocol += 0xff;
				}
        			
        		*/
        			opt_ret = send_netlink_transport_info(opt,opt_para,protocol,src_port,dest_port,
        												 s_ipaddr,d_ipaddr,s_ipmask,d_ipmask,
        												  ifindex,slot);
/*					pfm_system_log(PFM_SYSTEM_LOG_INFO,"################    opt_ret == %d ifindex:%d\n", opt_ret,ifindex);*/
        }
        else if(opt == 5)
        {
        		opt_ret = send_netlink_transport_info(opt,opt_para,protocol,src_port,dest_port,
        										 s_ipaddr,d_ipaddr,s_ipmask,d_ipmask,
        										  ifindex,slot);
        
        		ret = recv_netlink_transport_info();
        		if(ret < 0)
					goto err;
        			
        		reply = dbus_message_new_method_return(msg);
        		dbus_message_iter_init_append(reply,&iter);
        		dbus_message_iter_append_basic(&iter,DBUS_TYPE_INT32, &ret);
				dbus_message_iter_open_container (&iter,
										DBUS_TYPE_ARRAY,
										DBUS_STRUCT_BEGIN_CHAR_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING  /*opt*/
												DBUS_TYPE_UINT32_AS_STRING	/*opt_para*/	
												DBUS_TYPE_UINT32_AS_STRING  /*src_port	*/											
												DBUS_TYPE_UINT32_AS_STRING  /*dest_port*/
												DBUS_TYPE_UINT32_AS_STRING  /*protocol*/
												DBUS_TYPE_UINT32_AS_STRING	/*protocol*/
												DBUS_TYPE_UINT32_AS_STRING  /*src_ipaddr*/
												DBUS_TYPE_UINT32_AS_STRING	/*dest_ipaddr*/
												DBUS_TYPE_UINT32_AS_STRING	/*forward_opt*/
												DBUS_TYPE_UINT32_AS_STRING	/*forward_slot*/
												DBUS_TYPE_UINT32_AS_STRING	/*src_ipmask*/
												DBUS_TYPE_UINT32_AS_STRING	/*dest_ipmask*/
										DBUS_STRUCT_END_CHAR_AS_STRING,
										&iter_array);

				for(i=0;i<ret;i++)
				{		
					dbus_message_iter_open_container (&iter_array,DBUS_TYPE_STRUCT,NULL,&iter_struct);
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32,&(pfm_table[i].opt));
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32,&(pfm_table[i].opt_para));
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32,&(pfm_table[i].src_port));								  
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32, &(pfm_table[i].dest_port));
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32,&(pfm_table[i].protocol)); 
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32,&(pfm_table[i].ifindex)); 
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32,&(pfm_table[i].src_ipaddr));
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32,&(pfm_table[i].dest_ipaddr));																  
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32,&(pfm_table[i].forward_opt));
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32,&(pfm_table[i].forward_slot));				
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32,&(pfm_table[i].src_ipmask));
					dbus_message_iter_append_basic(&iter_struct,DBUS_TYPE_UINT32,&(pfm_table[i].dest_ipmask));
					dbus_message_iter_close_container(&iter_array,&iter_struct);
				}
			
				dbus_message_iter_close_container(&iter,&iter_array);
		       return reply;
		       err:
        			reply = dbus_message_new_method_return(msg);
        			dbus_message_iter_init_append(reply,&iter);
        			dbus_message_iter_append_basic(&iter,DBUS_TYPE_INT32, &ret);
        			return reply;
        }
		else
		{
        		opt_ret = send_netlink_transport_info(opt,opt_para,protocol,src_port,dest_port,
        										 s_ipaddr,d_ipaddr,s_ipmask,d_ipmask,
        										  ifindex,slot);
        
        	
        	/*opt_ret = send_netlink_transport_info(opt,opt_para,protocol,port,ifname,forward_opt,forward_opt_para,ipaddr,slot);*/
        
        
        
        }
        error:
        			reply = dbus_message_new_method_return(msg);
        			dbus_message_iter_init_append(reply,&iter);
        			dbus_message_iter_append_basic(&iter,
        											DBUS_TYPE_UINT32, &opt_ret);
        			return reply;
}	
	
static DBusHandlerResult 
pfm_dbus_demon_message_handler (DBusConnection *connection, 
											DBusMessage *message, 
											void *user_data)
{
		DBusMessage 	*reply = NULL;
	
		if(strcmp(dbus_message_get_path(message), PFM_DBUS_OBJPATH) == 0) {	
			if (dbus_message_is_method_call(message,						/**gjd: get message**/
											PFM_DBUS_INTERFACE,
											PFM_DBUS_METHOD_PFM_TABLE))
			{
				reply = pfm_parse_message(connection, message, user_data);/**gjd: parse the message**/
			}
			else if (dbus_message_is_method_call(message,						/**gjd: get message**/
												PFM_DBUS_INTERFACE,
												PFM_DBUS_METHOD_PFM_DEAL_SERVICE))
				{
					reply = pfm_parse_service_message(connection, message, user_data);/**gjd: parse the message**/
				}
		
		if (reply) {
			dbus_connection_send (connection, reply, NULL);
			dbus_connection_flush(connection); /* TODO	  Maybe we should let main loop process the flush*/
			dbus_message_unref (reply);
		}
	
	/*	dbus_message_unref(message); //TODO who should unref the incoming message? */
	}
		
		return DBUS_HANDLER_RESULT_HANDLED ;/*modify by dingkang coverity cid:11347*/
}
	
DBusHandlerResult 
demo_dbus_filter_function (DBusConnection * connection,
								DBusMessage * message, 
								void *user_data  )
{
	if (dbus_message_is_signal (message, DBUS_INTERFACE_LOCAL, "Disconnected") &&
			   strcmp (dbus_message_get_path (message), DBUS_PATH_LOCAL) == 0) {
	
			/* this is a local message; e.g. from libdbus in this process */
	
			dbus_connection_unref (pfm_dbus_connection);
			pfm_dbus_connection = NULL;
	
			/*g_timeout_add (3000, reinit_dbus, NULL);*/
	
		} else if (dbus_message_is_signal (message,
					  DBUS_INTERFACE_DBUS,
					  "NameOwnerChanged")) {
			;
		} else
			return TRUE;
	
		return DBUS_HANDLER_RESULT_HANDLED;
}
	
int pfm_dbus_init(void)
{
		DBusError dbus_error;
		DBusObjectPathVTable	demo_vtable = {NULL, &pfm_dbus_demon_message_handler, NULL, NULL, NULL, NULL};
		
		dbus_error_init (&dbus_error);
		pfm_dbus_connection = dbus_bus_get (DBUS_BUS_SYSTEM, &dbus_error);
		if (pfm_dbus_connection == NULL) {
			pfm_system_log(PFM_SYSTEM_LOG_FATAL,"init dbus dbus_bus_get(): %s", dbus_error.message);
			return -1;
		}
	
		if (!dbus_connection_register_fallback (pfm_dbus_connection, PFM_DBUS_OBJPATH, &demo_vtable, NULL)) {
			pfm_system_log(PFM_SYSTEM_LOG_FATAL,"can't register D-BUS handlers (fallback dhcpsnp). cannot continue.\n");
			return -1;
			
		}
			
		dbus_bus_request_name (pfm_dbus_connection, PFM_DBUS_BUSNAME,
					   0, &dbus_error);
		
		
		if (dbus_error_is_set (&dbus_error)) {
			pfm_system_log(PFM_SYSTEM_LOG_FATAL,"dbus request bus name error: %s\n",
					dbus_error.message);
			return -1;
		}
	
		dbus_connection_add_filter (pfm_dbus_connection, demo_dbus_filter_function, NULL, NULL);
	
		dbus_bus_add_match (pfm_dbus_connection,
					"type='signal'"
					",interface='"DBUS_INTERFACE_DBUS"'"
					",sender='"DBUS_SERVICE_DBUS"'"
					",member='NameOwnerChanged'",
					NULL);
		return 0;
}

#endif
int init_product_info()
{
	FILE *fd;
	int i=0;

	fd = fopen("/dbm/product/product_serial", "r");
	if (fd == NULL)
	{
		return -1;
	}
	if(1 != fscanf(fd, "%d", &product_serial))
	{
		product_serial=8;
		fclose(fd);/*coverity modify for CID 17193 */
		return -1;
	}
	fclose(fd);
	return 0;
}
	

int main(void)
	
{
	#if 0
	forwarding_table_init();
	insert_ifindex_dest_ip_tree(&(forwarding_table[0][0]),2,0,0,2,1,forwarding_table[0][0]);
		insert_ifindex_dest_ip_tree(&(forwarding_table[0][0]),3,0,0,3,1,forwarding_table[0][0]);
		insert_ifindex_dest_ip_tree(&(forwarding_table[0][0]),12,0,0,12,1,forwarding_table[0][0]);
		
		insert_ifindex_dest_ip_tree(&(forwarding_table[0][0]),11,0,0,11,1,forwarding_table[0][0]);
		insert_ifindex_dest_ip_tree(&(forwarding_table[0][0]),4,0,0,4,1,forwarding_table[0][0]);
		insert_ifindex_dest_ip_tree(&(forwarding_table[0][0]),5,0,0,5,4,forwarding_table[0][0]);
		insert_ifindex_dest_ip_tree(&(forwarding_table[0][0]),10,0,0,10,4,forwarding_table[0][0]);
		insert_ifindex_dest_ip_tree(&(forwarding_table[0][0]),17,0,0,17,4,forwarding_table[0][0]);
		insert_ifindex_dest_ip_tree(&(forwarding_table[0][0]),1,0,0,1,1,forwarding_table[0][0]);
		insert_ifindex_dest_ip_tree(&(forwarding_table[0][0]),13,0,0,13,1,forwarding_table[0][0]);
		insert_ifindex_dest_ip_tree(&(forwarding_table[0][0]),19,0,0,19,1,forwarding_table[0][0]);

	list_all_elem(forwarding_table[0][0]);

{
	int ifindex = 2;
	int ip_addr = 15;
	int i = -1;
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"look for ifindex %d,ip_add %d\n",ifindex , ip_addr);

	i = look_for_forward_slot_at_ifindex_tree(forwarding_table[0][0],ifindex,ip_addr);
	if(i != 0)
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"get forward_slot is %d\n",i);
	else
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"can't found \n");
	
}

{
	
	int ifindex = 2;
	int ip_addr = 5;
	int i = -1;
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"del ifindex %d,ip_add %d\n",ifindex , ip_addr);

	del_dest_ip_in_ifindex_tree(&(forwarding_table[0][0]),ifindex,0,0,ip_addr);

}

{
	int ifindex = 2;
	int ip_addr = 4;
	int i = -1;
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"look for ifindex %d,ip_add %d\n",ifindex , ip_addr);

	i = look_for_forward_slot_at_ifindex_tree(forwarding_table[0][0],ifindex,ip_addr);
	if(i != 0)
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"get forward_slot is %d\n",i);
	else
	pfm_system_log(PFM_SYSTEM_LOG_INFO,"can't found \n");
	
	list_all_elem(forwarding_table[0][0]);
	
}
#endif

#if 0
{
	int 			ret = 0;
	pthread_t 		*dbus_thread;
	pthread_attr_t	dbus_thread_attr;

	pfm_dbus_init();
	dbus_thread = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_attr_init(&dbus_thread_attr);
	ret = pthread_create(dbus_thread,&dbus_thread_attr,pfm_dbus_thread_main,NULL);

	if(0 != ret)
	{
		perror("pthread_create:");
		return -1;
	}
	pthread_join(dbus_thread,NULL);
}
return 0 ;


#else
	pfm_init_system_log();
	
	if(0 != init_product_info())
	{
		fprintf(stderr,"get product info error\n");
		/*exit -1*/
		return -1; 
	}

	if(-1 != init_netlink())
	{
		pfm_system_log(PFM_SYSTEM_LOG_SECURITY,"init_netlink ok\n");
		send_netlink_transport_info(SET_PFM_PID,getpid(),0,0,0,0,0,0,0,0,0);
	}else{

		pfm_system_log(PFM_SYSTEM_LOG_FATAL,"init_netlink error\n");
		return -1;
	}
/*
	send_netlink_transport_info(0,2,6,22,10,6,4,(_u32)0xc0a80480,9);
	

	send_netlink_transport_info(0,2,6,22,10,6,4,(_u32)0xc0a80481,9);


	send_netlink_transport_info(1,2,6,22,2,6,4,(_u32)0xc0a80481,1);	
*/

	if(-1 == pfm_dbus_init())
	{
		pfm_system_log(PFM_SYSTEM_LOG_FATAL,"pfm_dbus_init error\n");
		return -1;
	}else{
		pfm_system_log(PFM_SYSTEM_LOG_SECURITY,"pfm_dbus_init OK\n");
	}
		while(dbus_connection_read_write_dispatch(pfm_dbus_connection,-1)){
			;
	}
	

	return 0 ;
	#endif
}


