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
* npd_igmp_snp_com.c
*
*
*CREATOR:
*		zhubo@autelan.com
*
* DESCRIPTION:
*   	APIs used in NPD for igmp snooping process.
*
* DATE:
*  		03/01/2008	
*
*  FILE REVISION NUMBER:
*       $Revision: 1.43 $
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*include header files begin */
/*kernel part */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/select.h>
#include "util/npd_list.h"

/*user part */
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_product.h"
#include "npd_vlan.h"
#include "npd_log.h"
#include "npd_igmp_snp_com.h"
#include "sysdef/returncode.h"
#include "npd_mld_snp_com.h"
/*include header files begin */

/*MACRO definition begin */
#define	IGMP_SNOOP_NPD_MSG_SOCK	"/tmp/igmp_snp_npdmng.sock"		/*command¢ssocket*/
#define	IGMP_MSG_MAX_SIZE		(sizeof(struct igmp_msg_npd) > sizeof(struct mld_msg_npd)) ?  \
								sizeof(struct igmp_msg_npd) : sizeof(struct mld_msg_npd)	
								/*modify by yangxs for mld*/
#define CPSS_IP_PROTOCOL_IPV4_E 0
#define CPSS_BRG_IPM_GV_E		1
/*MACRO definition end */


/*external variables reference begin */
/*external variables reference end */

/*external function declarations begin*/

/*external function declarations begin*/

/*local variables definition begin */

/*l2mc_list	*l2mc_member_list = NULL;*/
unsigned char igmpSnpEnable = 0;
unsigned char mldSnpEnable = 0;
mc_fdb_list_head 	*mcFdbListHead = NULL;
groupBasedVlan v2McGroupArry[NPD_IGMP_SNP_GROUP_NUM_MAX] ;
int		igmp_fd = 0;
unsigned char transMac[MACADDRLENGTH] = {0};

/*local variables definition end */
struct	sockaddr_un 	npd_igmpsnp_addr;   /*local addr*/	
struct	sockaddr_un		igmpsnp_addr;   	/*remote addr*/

/*******************************************************************************
 * npd_igmp_snp_sock_init
 *
 * DESCRIPTION:
 *   		create socket communication with igmpsnp
 *
 * INPUTS:
 * 		null
 *
 * OUTPUTS:
 *    	ser_sock - socket number
 *
 * RETURNS:
 * 		IGMPSNP_RETURN_CODE_OK   - create successfully 
 * 		IGMPSNP_RETURN_CODE_ERROR	- create socket or bind error
 *
 * COMMENTS:
 *      
 **
 ********************************************************************************/
int	npd_igmp_snp_sock_init(int *ser_sock)
{
	int ret = -1;
	memset(&igmpsnp_addr,0,sizeof(igmpsnp_addr));
	memset(&npd_igmpsnp_addr,0,sizeof(npd_igmpsnp_addr));

	if((*ser_sock = socket(AF_LOCAL,SOCK_DGRAM,0)) == -1)
	{
		npd_syslog_err("create npd to igmp socket fail\n");
		return IGMPSNP_RETURN_CODE_ERROR;
	}
	
	npd_igmpsnp_addr.sun_family = AF_LOCAL;
	strcpy(npd_igmpsnp_addr.sun_path,"/tmp/npd2igmpSnp_server");

	igmpsnp_addr.sun_family = AF_LOCAL;
	strcpy(igmpsnp_addr.sun_path, "/tmp/npd2igmpSnp_client");

    unlink(npd_igmpsnp_addr.sun_path);

	if(bind(*ser_sock , (struct sockaddr *)&npd_igmpsnp_addr, sizeof(npd_igmpsnp_addr)) == -1) 
	{
		npd_syslog_err("npd to igmp snooping socket created but failed when bind\n");
		close(*ser_sock);	/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
		return IGMPSNP_RETURN_CODE_ERROR;
	}

	ret = chmod(npd_igmpsnp_addr.sun_path, 0777);
	if(-1 == ret)	/* code optimize: Unchecked return value from library  houxx@autelan.com  2013-1-18 */
	{
		return IGMPSNP_RETURN_CODE_ERROR;
	}
	return IGMPSNP_RETURN_CODE_OK;	
	
}

/*******************************************************************************
 * npd_cmd_sendto_igmpsnp
 *
 * DESCRIPTION:
 *   		npd commad send to igmpsnp 
 *
 * INPUTS:
 * 		mngCmd - npd commad message
 *
 * OUTPUTS:
 *    	null
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - send successfully		
 *		IGMPSNP_RETURN_CODE_ERROR - send fail when use sendto 
 *		IGMPSNP_RETURN_CODE_ALLOC_MEM_NULL - malloc memory fail
 *
 * COMMENTS:
 *      
 **
 ********************************************************************************/

 int npd_cmd_sendto_igmpsnp
(
	struct npd_dev_event_cmd* mngCmd	
)
{
	syslog_ax_igmp_dbg("npd msg sendto igmp-snoop\n");
	struct npd_mng_igmp*	devMsg = NULL;
	dev_notify_msg*		notify_format = NULL;
	unsigned int		cmdLen = sizeof(struct npd_mng_igmp);
	int	rc,byteSend = 0;
	
	devMsg = (struct npd_mng_igmp* )malloc(sizeof(struct npd_mng_igmp));
	if(NULL == devMsg){
		syslog_ax_igmp_dbg("devMsg memory allocation Fail!\n");
		return IGMPSNP_RETURN_CODE_ALLOC_MEM_NULL;
	}
	memset(devMsg,0,sizeof(struct npd_mng_igmp));	/* code optimize: Wrong sizeof argument houxx@autelan.com  2013-1-18 */
	
	notify_format = &(devMsg->npd_dev_mng);
	devMsg->nlh.nlmsg_len = sizeof(struct npd_mng_igmp);
	syslog_ax_igmp_dbg("IGMP Snooping devMsg->nlh.nlmsg_len %d\n,sizeof(struct nlmsghdr) %d\n, sizeof(struct dev_notify_mng) %d.\n",
								devMsg->nlh.nlmsg_len,sizeof(struct nlmsghdr),sizeof(dev_notify_msg));
	devMsg->nlh.nlmsg_type = IGMP_SNP_TYPE_DEVICE_EVENT;
	devMsg->nlh.nlmsg_flags = IGMP_SNP_FLAG_ADDR_MOD;/*can be ignored*/
	notify_format->event = mngCmd->event_type;/*port down/up,EVENT_DEV_UNREGISTER..*/
	notify_format->vlan_id = mngCmd->vid;
	notify_format->ifindex = mngCmd->port_index;

	if(NPD_IGMPSNP_EVENT_DEV_SYS_MAC_NOTI == notify_format->event){
		memcpy(notify_format->sys_mac,transMac,MACADDRLENGTH);
		syslog_ax_igmp_dbg("show system Mac address: %s.\n",PRODUCT_MAC_ADDRESS);
	}
	
	while(cmdLen != byteSend)
	{
		rc = sendto(igmp_fd,( char* )devMsg,sizeof(struct npd_mng_igmp),MSG_DONTWAIT,
							(struct sockaddr *)&igmpsnp_addr, sizeof(igmpsnp_addr));
		if(rc < 0)
		{
			if(errno == EINTR)/*send() be interrupted.*/
			{
				npd_syslog_dbg("sendto");
				continue;
			}
			else if(errno == EACCES)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == EWOULDBLOCK)/*send()*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == EBADF)
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == ECONNRESET)
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == EDESTADDRREQ)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == EFAULT)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == EINVAL)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == EISCONN)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == EMSGSIZE)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == ENOBUFS)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == ENOMEM)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == ENOTCONN)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == ENOTSOCK)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == EOPNOTSUPP)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else if(errno == EPIPE)/*send() permission is denied on the destination socket file*/
			{
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				break;
			}
			else {
				npd_syslog_dbg("%d:%s\n", errno, strerror(errno));
				syslog_ax_igmp_dbg("IgmpSnpCmd Write To IGMP::Write Fail.\n");
				free(devMsg);
				devMsg = NULL;
				return IGMPSNP_RETURN_CODE_ERROR;
			}
		}
		byteSend += rc;
	}
	if(byteSend == cmdLen) {
		rc = IGMPSNP_RETURN_CODE_OK;	
	}
	free(devMsg);
	devMsg = NULL;
	return rc;
}

/*******************************************************************************
 * npd_igmpsnp_recv_info
 *
 * DESCRIPTION:
 *   		use recvfrom to receive information from igmp 
 *
 * INPUTS:
 *		infoLen - the receive max size
 *
 * OUTPUTS:
 * 		msg - pointer to igmp message get from igmp
 *		len - actual receive the data size
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - complete the receive
 *
 * COMMENTS:
 *     
 **
 ********************************************************************************/
int npd_igmpsnp_recv_info
(
	char *msg,
	unsigned int  infoLen,
	int *len
)
{
	unsigned int addrLen = sizeof(igmpsnp_addr);
	while(1)
	{
		*len = recvfrom(igmp_fd, (char*)msg, infoLen, 0,(struct sockaddr *)&igmpsnp_addr, &addrLen);
		if(*len < 0 && errno == EINTR) 
		{
			continue;
		}
		break;
	}
	return IGMPSNP_RETURN_CODE_OK;
}
#if 0
/**********************************************************************************
 *
 * init global l2mc_list structure:NPD_LMAC_MCNUMBER_MAX pointers point to detailde 
 * l2mc_list structure.
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_init_l2mc_entry_list(void) {
	//syslog_ax_igmp_dbg("npd init l2mc entry count %d",NPD_LMAC_MCNUMBER_MAX);
	
	l2mc_member_list = malloc(sizeof(l2mc_list*)*(NPD_LMAC_MCNUMBER_MAX);
	
	if(NULL == l2mc_member_list) {
		NPD_ERROR("memory alloc error for l2mc entry init!!");
		return;
	}
	memset(l2mc_member_list,0,NPD_LMAC_MCNUMBER_MAX);
	return;
}

int creatservsock_dgram(char *path)
{
	int sock,len,oldmask;
	struct sockaddr_un serv;
	
	if(!path)
		return -1;
	
	unlink(path);
	oldmask =umask(0077);
	if((sock = socket(AF_UNIX,SOCK_DGRAM,0))<0)
	{
		printf("Creat socket failed.\r\n");
		return -1;
	}
	
	memset(&serv,0,sizeof(struct sockaddr_un));
	serv.sun_family = AF_UNIX;
	strncpy(serv.sun_path,path,strlen(path));
	len = strlen(serv.sun_path) + sizeof(serv.sun_family);
	if(bind(sock,(struct sockaddr*)&serv,len)<0)
	{
		printf("Bind failed.\r\n");
		close(sock);
		return -1;
	}
	umask(oldmask);
	return sock;
}
int creatservsock_stream(char *path)
{
	int sock,len;
	struct sockaddr_un serv;
	
	if( !path )
		return -1;
	
	unlink(path);
	
	if((sock = socket(AF_UNIX,SOCK_STREAM,0))<0)
	{
		printf("Creat socket failed.\r\n");
		return -1;
	}
	syslog_ax_igmp_dbg("create socket success,sock_fd = %d.",sock);

	memset(&serv,0,sizeof(struct sockaddr_un));
	serv.sun_family = AF_UNIX;
	strncpy(serv.sun_path,path,strlen(path));
	len = sizeof(serv.sun_family)+strlen(serv.sun_path);
	
	if( bind(sock,(struct sockaddr*)&serv,len)<0)
	{
		printf("Bind failed.\r\n");
		close(sock);
		return -1;
	}
	syslog_ax_igmp_dbg("server socket bind to the path :%s OK!",path);
	
	if( listen(sock,5)<0 )
	{
		printf("Listen failed.");
		close(sock);
		return -1;
	}
	syslog_ax_igmp_dbg("server listening ...");
	return sock;
}

int read_npdsocket(int fd,void *buffer,int length) 
{ 
 int bytes_left; 
 int bytes_read; 
 char *ptr; 
   
 bytes_left=length; 
 while(bytes_left>0) 
 { 
   bytes_read=read(fd,ptr,bytes_read); 
   if(bytes_read<=0) 
   { 
     if(errno==EINTR) 
        bytes_read=0; 
     else 
	 {
		exit(1);
		//return(-1); 
	 }
   } 
   else if(bytes_read==0) 
       break; 
    bytes_left-=bytes_read; 
    ptr+=bytes_read; 
 } 
 return(length-bytes_left); 
} 

/********************/
/*EVENT_DEV_UP,EVENT_DEV_DOWN,EVENT_DEV_UNREGISTER*/
/********************/
int npd_msg_sendto_igmpsnp
(
	struct npd_dev_event_cmd* mngCmd	
)
{
	syslog_ax_igmp_dbg("enter npd msg sendto igmpsnp...");
	struct igmp_skb*	devMsg = NULL;
	dev_notify_msg*		notify_format = NULL;
	unsigned int		cmdLen = sizeof(struct igmp_skb);
	int	rc,byteSend = 0;
	int tmpfd = igmp_fd;
	devMsg = (struct igmp_skb* )malloc(sizeof(struct igmp_skb));
	if(NULL == devMsg){
		printf("devMsg memory allocation Fail!\r\n");
		return 0;
	}

	notify_format = (dev_notify_msg*)devMsg->mngbuf;
	devMsg->nlh.nlmsg_len = sizeof(struct igmp_skb);
	devMsg->nlh.nlmsg_type = IGMP_SNP_TYPE_DEVICE_EVENT;
	devMsg->nlh.nlmsg_flags = IGMP_SNP_FLAG_ADDR_MOD;/*can be ignored*/
	notify_format->event = mngCmd->event_type;/*port down/up,EVENT_DEV_UNREGISTER..*/
	notify_format->vlan_id = mngCmd->vid;
	notify_format->ifindex = mngCmd->port_index;

	//memcpy(devMsg->mngbuf,notify_format,sizeof(dev_notify_msg);
	/*
		byteSend = write(igmp_fd,( char* )devMsg,sizeof(struct igmp_skb);
		if(0 == byteSend){
			free(devMsg);
			close(igmp_fd);
		}
	*/
	while(cmdLen != byteSend)
	{
		/*
		rc = sendto(igmp_fd,devMsg,sizeof(struct igmp_skb),MSG_DONTWAIT,
							(struct sockaddr *)&client, sizeof(client);
		*/
		syslog_ax_igmp_dbg("send devMsg via socket");
		rc = send(tmpfd,(const void*)devMsg,sizeof(struct igmp_skb),0);
		if(rc < 0)
		{
			if(errno == EINTR)/*send() be interrupted.*/
			{
				continue;
			}
			else if(errno == EACCES)/*send() permission is denied on the destination socket file*/
			{
				printf("send() permision denied.\n");
				break;
			}
			else if(errno == EWOULDBLOCK)/*send()*/
			{
				break;
			}
			else if(errno == EBADF)
			{
				break;
			}
			else if(errno == ECONNRESET)
			{
				break;
			}
			else if(errno == EDESTADDRREQ)/*send() permission is denied on the destination socket file*/
			{
				break;
			}
			else if(errno == EFAULT)/*send() permission is denied on the destination socket file*/
			{
				break;
			}
			else if(errno == EINVAL)/*send() permission is denied on the destination socket file*/
			{
				break;
			}
			else if(errno == EISCONN)/*send() permission is denied on the destination socket file*/
			{
				break;
			}
			else if(errno == EMSGSIZE)/*send() permission is denied on the destination socket file*/
			{
				break;
			}
			else if(errno == ENOBUFS)/*send() permission is denied on the destination socket file*/
			{
				break;
			}
			else if(errno == ENOMEM)/*send() permission is denied on the destination socket file*/
			{
				break;
			}
			else if(errno == ENOTCONN)/*send() permission is denied on the destination socket file*/
			{
				break;
			}
			else if(errno == ENOTSOCK)/*send() permission is denied on the destination socket file*/
			{
				break;
			}
			else if(errno == EOPNOTSUPP)/*send() permission is denied on the destination socket file*/
			{
				break;
			}
			else if(errno == EPIPE)/*send() permission is denied on the destination socket file*/
			{
				break;
			}
			else
			{
				printf("rstp socket write fail.\n");
				break;//return -1;/*send fail unknow error*/
			}
		}
		byteSend += rc;
	}
	printf("send :fd %d\n",tmpfd);							
	printf("send :byteSend %d,originCmdbytes %d\n",byteSend,cmdLen);
	if(byteSend == cmdLen) {
		rc = 0;	
	}
	free(devMsg);
	printf("close igmp_fd .\n");							
	//close(igmp_fd);/*really need close,should NOT*/
	return rc;
	
}
#endif
/*******************************************************************************
 * npd_check_igmp_snp_status
 *
 * DESCRIPTION:
 *   		check the global status is enable or disable
 *
 * INPUTS:
 *		null
 *
 * OUTPUTS:
 *    	status - output the status
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - get the status successfully
 *
 * COMMENTS:
 *   
 **
 ********************************************************************************/

int npd_check_igmp_snp_status(unsigned char *status)
{
	syslog_ax_vlan_dbg("npd check igmp snp status: %d.",igmpSnpEnable);	
	*status = igmpSnpEnable;
	return IGMPSNP_RETURN_CODE_OK;
}

/*******************************************************************************
 * npd_check_igmp_snp_vlan_status
 *
 * DESCRIPTION:
 *   		check the vlan status is enable or disable
 *
 * INPUTS:
 *		vlanId - vlan id
 *
 * OUTPUTS:
 *    	status - output the port status
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - get the status successfully
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - the vlan not exist
 *
 * COMMENTS:
 *   
 **
 ********************************************************************************/

int npd_check_igmp_snp_vlan_status(unsigned short vlanId,unsigned char *status)
{
	struct vlan_s *vlanNode = NULL;
	vlanNode = npd_find_vlan_by_vid(vlanId);
	if((NULL == vlanNode)||(vlanNode->isAutoCreated)){
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}
	*status =vlanNode->igmpSnpEnDis;

	return IGMPSNP_RETURN_CODE_OK;
}
/*******************************************************************************
 * npd_check_igmp_snp_vlanMbr_status
 *
 * DESCRIPTION:
 *   		check the igmp port status is enable or disable
 *
 * INPUTS:
 *		vlanId - vlan id
 *		eth_g_index - ether global index ,which we can get portNode
 *
 * OUTPUTS:
 *    	status - output the port status
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - get the status successfully
 *		IGMPSNP_RETURN_CODE_PORT_NOT_EXIST - the port not exist
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - the vlan not exist
 *		IGMPSNP_RETURN_CODE_ERROR_SW - software error
 *
 * COMMENTS:
 *   
 **
 ********************************************************************************/

int npd_check_igmp_snp_vlanMbr_status
(
	unsigned short vlanId,
	unsigned int eth_g_index,
	unsigned char *status
)
{
	struct vlan_s *vlanNode = NULL;
	unsigned char isTagged = NPD_FALSE;
	unsigned int ret = IGMPSNP_RETURN_CODE_ERROR;
	unsigned char enDisFlag = 0;
	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode){
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}
	ret = npd_vlan_check_contain_port(vlanId,eth_g_index, &isTagged);
	if(NPD_TRUE == ret){
		ret = npd_vlan_port_igmp_endis_flag_check(vlanId,eth_g_index,isTagged,&enDisFlag);
		if(IGMPSNP_RETURN_CODE_OK == ret){
			*status = enDisFlag;
			return IGMPSNP_RETURN_CODE_OK;
		}
	}
	else {
		return IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;
	}
	*status = 0xff;
	return IGMPSNP_RETURN_CODE_ERROR_SW;
}
/*******************************************************************************
 * npd_check_igmp_snp_port_status
 *
 * DESCRIPTION:
 *   		check the igmp port status from bitmap
 *
 * INPUTS:
 * 		eth_g_index - ether global index ,which we can get portNode
 *
 * OUTPUTS:
 *    	status - output the port status
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - get the status successfully
 *		IGMPSNP_RETURN_CODE_PORT_NOT_EXIST - the port not exist
 *
 * COMMENTS:
 *   
 **
 ********************************************************************************/

int npd_check_igmp_snp_port_status(unsigned int eth_g_index,unsigned char* status)
{
	struct eth_port_s	*portNode = NULL;
	portNode = npd_get_port_by_index(eth_g_index);
	if( NULL == portNode ) {
		return IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;
	}
	*status = 0x01 & (portNode->funcs.funcs_run_bitmap >> ETH_PORT_FUNC_IGMP_SNP);
	return IGMPSNP_RETURN_CODE_OK;
}
/**********************************************************************************
 *  npd_igmp_snp_l2mc_vlan_exist
 *
 * DESCRIPTION:
 * 		check the grouparry contain the vlan by vlanid
 *
 * INPUT:
 *		vlanId - vlan id
 *	
 * OUTPUT:
 *		null
 *
 *  RETURN:
 *		IGMPSNP_RETURN_CODE_OK - can find vlan success
 *		IGMPSNP_RETURN_CODE_MC_VLAN_NOT_EXIST - can not find the vlan
 *
 **********************************************************************************/

int npd_igmp_snp_l2mc_vlan_exist
(
	unsigned short vlanId
)
{
	unsigned int ret = IGMPSNP_RETURN_CODE_MC_VLAN_NOT_EXIST;
	if( 1 == v2McGroupArry[vlanId].vidBit){
		ret = IGMPSNP_RETURN_CODE_OK;
	}
	return ret;
}
/**********************************************************************************
 *  npd_igmp_snp_l2mc_group_exist
 *
 * DESCRIPTION:
 * 		check the grouparry contain the group by vidx
 *
 * INPUT:
 *		vlanId - vlan id
 * 		vidx - the vlan index about group
 *	
 * OUTPUT:
 *		null
 *
 *  RETURN:
 *		IGMPSNP_RETURN_CODE_OK - can find group success
 *		IGMPSNP_RETURN_CODE_GROUP_NOTEXIST - can not find the group
 *
 **********************************************************************************/

int npd_igmp_snp_l2mc_group_exist
(
	unsigned short vlanId, 
	unsigned short vidx
)
{
	unsigned int ret = IGMPSNP_RETURN_CODE_GROUP_NOTEXIST;
	
	if(1 == v2McGroupArry[vlanId].mbrArray[vidx]){
		ret = IGMPSNP_RETURN_CODE_OK;
	}
	return ret;
}
/**********************************************************************************
 *  npd_l2mc_entry_mbr_exist
 *
 * DESCRIPTION:
 * 		check the fdb entry  contain the group by vidx
 *
 * INPUT:
 *		vidx - the vlan index about group
 *	
 * OUTPUT:
 *		null
 *
 *  RETURN:
 *		IGMPSNP_RETURN_CODE_OK - can find group address success
 *		IGMPSNP_RETURN_CODE_ERROR - can not find the group address
 *
 **********************************************************************************/

int npd_l2mc_entry_mbr_exist(unsigned short vidx)
{
	unsigned long ret = IGMPSNP_RETURN_CODE_OK;
	unsigned char devNum =0;
	CPSS_PORTS_BMP_STC portBitmapPtr;
	portBitmapPtr.ports[0] = 0;
	portBitmapPtr.ports[1] = 0;
	ret = nam_mc_entry_read(devNum, vidx, &portBitmapPtr);
	if ((0 == portBitmapPtr.ports[0]) && (0 == portBitmapPtr.ports[1])){
		return IGMPSNP_RETURN_CODE_OK;/*l2mc entry index by Vidx is empty*/
	}
	else
		return IGMPSNP_RETURN_CODE_ERROR;
}

#if 0  
/*only need vidx for index a fdb entry*/
/*once found a fdb entry with the same groupIP & vidx,do nothing*/
unsigned int npd_igmp_get_groupaddr_by_vlanid_vidx
(
	unsigned short vid,
	unsigned short vidx,
	unsigned int *groupAddr
)
{
	
	mc_fdb_list *pstFdbEntry = NULL;
	unsigned int groupIp = 0;
	syslog_ax_igmp_dbg("Enter :npd_igmp_get_groupaddr_by_vlanid_vidx...");
	if(mcFdbListHead->stListHead)
	{
		pstFdbEntry = mcFdbListHead->stListHead;
		while(pstFdbEntry)
		{
			if ((pstFdbEntry->vidx == vidx)&&(pstFdbEntry->vlanId == vid))
			{
				groupIp = pstFdbEntry->groupIp;
				break;
			}
			pstFdbEntry = pstFdbEntry->next;
			if( pstFdbEntry == mcFdbListHead->stListHead )
			{	/*can not find*/
				groupIp = 0;
				break;
			}
		}
	}
	else /*NO FDB entry create*/
	{
		syslog_ax_igmp_dbg("No fdb Entry Created!");
		*groupAddr = 0;
		return NPD_MC_FDB_ENTRY_ERROR_GENERAL;
	}
	*groupAddr = groupIp;
	return NPD_MC_FDB_ENTRY_SUCCESS;
}

/*only need vidx for index a fdb entry*/
/*once found a fdb entry with the same groupIP & vidx,do nothing*/
unsigned int npd_mc_fdb_entry_insert
(
	unsigned short vid,
	unsigned long groupIp,
	unsigned short vidx,
	unsigned int *entryExist
)
{
	
	mc_fdb_list * pstFdbEntry = NULL;
	mc_fdb_list * fdbTemp = NULL;
	int lCount = 0;
	unsigned int EntryExist = 0;
	syslog_ax_igmp_dbg("Enter :npd_Mc_Fdb_Entry_Insert...vid %d,vidx %d,groupIp 0x%x.",vid,vidx,groupIp);
	if(mcFdbListHead->stListHead)
	{
		syslog_ax_igmp_dbg("insert fdb entry :vid %d,vidx %d,groupIp 0x%x.",vid,vidx,groupIp);
		fdbTemp = mcFdbListHead->stListHead;
		while(fdbTemp)
		{
			if ( lCount >= NPD_IGMP_SNP_GROUP_NUM_MAX + 1 )
			{
				return NPD_MC_FDB_ENTRY_ERROR_OUT_MAX;
			}
			lCount++;
			if ((fdbTemp->groupIp == groupIp)&&(fdbTemp->vidx == vidx)
					&&(fdbTemp->vlanId == vid))
			{
				EntryExist = 1;
				break;
			}
			fdbTemp =fdbTemp->next;
			if( fdbTemp == mcFdbListHead->stListHead )	/*can not find*/
				break;
		}
		if ( EntryExist != 1 )
		{
			if ( mcFdbListHead->ulListNodeCount >= NPD_IGMP_SNP_GROUP_NUM_MAX + 1 )
			{
				return NPD_MC_FDB_ENTRY_ERROR_OUT_MAX;
			}
			pstFdbEntry = (mc_fdb_list*)malloc( sizeof( mc_fdb_list ));
			if ( pstFdbEntry == NULL )
			{
				return NPD_MC_FDB_ENTRY_ERROR_GENERAL;
			}
			memset(pstFdbEntry,0,sizeof(mc_fdb_list));
			pstFdbEntry->vlanId = vid;
			pstFdbEntry->groupIp = groupIp;
			pstFdbEntry->vidx = vidx;
			pstFdbEntry->next = mcFdbListHead->stListHead;
			pstFdbEntry->prev = mcFdbListHead->stListHead->prev;
			if(mcFdbListHead->stListHead->prev){
				(mcFdbListHead->stListHead->prev)->next = pstFdbEntry;
			}
			mcFdbListHead->stListHead = pstFdbEntry;
			mcFdbListHead->ulListNodeCount++;
			syslog_ax_igmp_dbg(" Mc_Fdb_Entry_Insert: insert new entry.");
		}
	}
	else /*first FDB entry*/
	{	
		syslog_ax_igmp_dbg("add first fdb entry");
		pstFdbEntry = (mc_fdb_list*)malloc( sizeof( mc_fdb_list ));
		if ( pstFdbEntry == NULL )
		{
			return NPD_MC_FDB_ENTRY_ERROR_GENERAL;
		}
		memset(pstFdbEntry,0,sizeof(mc_fdb_list));
		pstFdbEntry->next = pstFdbEntry;
		pstFdbEntry->prev = pstFdbEntry;
		pstFdbEntry->vlanId = vid;
		pstFdbEntry->groupIp = groupIp;
		pstFdbEntry->vidx = vidx;
		mcFdbListHead->stListHead = pstFdbEntry;
		mcFdbListHead->ulListNodeCount = 1;
	}
	*entryExist = EntryExist;
	return NPD_MC_FDB_ENTRY_SUCCESS;
}

/*delete one Node in Mc FDB entry list*/
/*one Mc group owned only one FDB entry*/
unsigned int npd_mc_fdb_entry_delete
(
	unsigned short vid,
	unsigned long groupIp,
	unsigned short vidx,
	unsigned int *entryExist
)
{

	mc_fdb_list *pstFdbEntry = NULL;
	mc_fdb_list *pstTemp = NULL;
	mc_fdb_list *curFdbEntry = NULL;
	int lCount = 0;
	unsigned int EntryExist = 0;

	syslog_ax_igmp_dbg("Enter :npd_Mc_Fdb_Entry_delete...vid %d,vidx %d,groupIp 0x%x.",vid,vidx,groupIp);
	pstTemp = mcFdbListHead->stListHead;

	while(pstTemp)
	{
		if (lCount >= NPD_IGMP_SNP_GROUP_NUM_MAX + 1 )
		{
			return NPD_MC_FDB_ENTRY_ERROR_GENERAL;
		}
		lCount++;

		if ((pstTemp ->groupIp == groupIp)&&(pstTemp->vidx == vidx)
				&&(pstTemp->vlanId == vid))
		{
			EntryExist = 1;
			pstFdbEntry = pstTemp;
			break;
		}
		curFdbEntry = pstTemp;
		pstTemp = pstTemp->next;
		if( pstTemp == mcFdbListHead->stListHead )	/*can not find*/
			break;
	}
	if ( EntryExist == 1 )
	{
		if ( pstFdbEntry == NULL )
		{
			return NPD_MC_FDB_ENTRY_ERROR_GENERAL;
		}

		(pstFdbEntry->prev)->next = pstFdbEntry->next;
		(pstFdbEntry->next)->prev = pstFdbEntry->prev;
		free( pstFdbEntry );
		pstFdbEntry = NULL;
		mcFdbListHead->ulListNodeCount--;
		if(0== mcFdbListHead->ulListNodeCount)
			mcFdbListHead->stListHead = NULL;
		syslog_ax_igmp_dbg(" Mc_Fdb_Entry_Delete: remove entry.");
	}
	*entryExist = EntryExist;
	return NPD_MC_FDB_ENTRY_SUCCESS;
}

/*delete Mc FDB entry list*/
/*one Mc group owned only one FDB entry*/
unsigned int npd_mc_fdb_entry_delete_all
(
	unsigned short vid[],
	unsigned int groupIp[],
	unsigned short vidx[],
	unsigned int *fdbCnt
)
{

	mc_fdb_list *pstTemp = NULL;
	mc_fdb_list *fdbTemp = NULL;
	int lCount = 0;
	pstTemp = mcFdbListHead->stListHead;
	while(pstTemp)
	{

		if (lCount >= NPD_IGMP_SNP_GROUP_NUM_MAX + 1 )
		{
			return NPD_MC_FDB_ENTRY_ERROR_GENERAL;
		}

		if ( pstTemp == NULL )
		{
			return NPD_MC_FDB_ENTRY_ERROR_GENERAL;
		}
		if(NULL !=pstTemp->prev)
		(pstTemp->prev)->next = pstTemp->next;
		(pstTemp->next)->prev = pstTemp->prev;

		vid[lCount] = pstTemp->vlanId;
		groupIp[lCount]= pstTemp->groupIp;
		vidx[lCount] = pstTemp->vidx;
		lCount++;

		mcFdbListHead->ulListNodeCount--;
		if(0== mcFdbListHead->ulListNodeCount)
			mcFdbListHead->stListHead = NULL;

		/**/
		fdbTemp = pstTemp;
		pstTemp = pstTemp->next;
		free( fdbTemp );
		if( pstTemp == mcFdbListHead->stListHead )	/*can not find*/
			break;
	}

	*fdbCnt = lCount;
	return NPD_MC_FDB_ENTRY_SUCCESS;
}
#endif
/**********************************************************************************
 *  npd_igmp_check_contain_fdb
 *
 * DESCRIPTION:
 * 		check the fdb entry  contain the group by vidx
 *
 * INPUT:
 *		vlanId - vlan id
 *		vidx - the vlan index about group
 *	
 * OUTPUT:
 *		null

 *
 *  RETURN:
 *		IGMPSNP_RETURN_CODE_OK - can find ip address success
 *		IGMPSNP_RETURN_CODE_ERROR - can not find the group address
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - can find vlannode by vlanid
 *		IGMPSNP_RETURN_CODE_NULL_PTR - the fdblist is null
 *
 **********************************************************************************/
unsigned int npd_igmp_check_contain_fdb
(
	unsigned short vlanId,
	unsigned short vidx
	/*unsigned int	groupAddr*/
)
{
	struct vlan_s *vlanNode = NULL;
	igmp_snooping_list_s *fdbList = NULL;
	struct igmp_snooping_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}

	fdbList = vlanNode->igmpList;
	if(NULL == fdbList) {
		return IGMPSNP_RETURN_CODE_NULL_PTR;
	}
	else if(0 != fdbList->count) {
		syslog_ax_vlan_dbg("npd vlan check igmp fdb entry:: vlan %d fdb entry count %d.",vlanId,fdbList->count);
		list = &(fdbList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct igmp_snooping_list_node_s,list);
			if(vidx == node->vidx )/*&& groupAddr == node->groupIp) {*/
			{	syslog_ax_vlan_dbg("npd vlan check igmp fdb entry: vidx %d groupAddr 0x%x,"\
									"groupIpV6 %4x:%4x:%4x:%4x:%4x:%4x:%4x:%4x, in vlan %d.",\
									vidx,node->groupIp,ADDRSHOWV6(node->grouIpV6),vlanId);
				return IGMPSNP_RETURN_CODE_OK;
			}
		}
	}
	syslog_ax_vlan_dbg("vidx %d NOT exist of vlan %d!",vidx,vlanId);
	return IGMPSNP_RETURN_CODE_ERROR;
}
/**********************************************************************************
 *  npd_igmp_get_groupip_by_vlan_vidx
 *
 * DESCRIPTION:
 * 		get group ip address by vlanId
 *
 * INPUT:
 *		vlanId - vlan id
 *		vidx - the vlan index about group
 *	
 * OUTPUT:
 *		groupAddr - group Ip address

 *
 *  RETURN:
 *		IGMPSNP_RETURN_CODE_OK - get the ip address success
 *		IGMPSNP_RETURN_CODE_ERROR - can find the group address
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - can find vlannode by vlanid
 *		IGMPSNP_RETURN_CODE_NULL_PTR - the fdblist is null
 *
 **********************************************************************************/

unsigned int npd_igmp_get_groupip_by_vlan_vidx
(
	unsigned short vlanId,
	unsigned short vidx,
	unsigned int	*groupAddr,
	unsigned short **groupAddrv6,
	unsigned int	*ret
)
{
	struct vlan_s *vlanNode = NULL;
	igmp_snooping_list_s *fdbList = NULL;
	struct igmp_snooping_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	unsigned int iptemp =0x0;

	syslog_ax_vlan_dbg("#####GET groupIp by vidx %d in vlan %d.\n",vidx,vlanId);
	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}

	fdbList = vlanNode->igmpList;
	if(NULL == fdbList) {
		return IGMPSNP_RETURN_CODE_NULL_PTR;
	}
	else if(0 != fdbList->count) {
		syslog_ax_vlan_dbg("npd vlan check igmp fdb entry:: vlan %d fdb entry count %d.",vlanId,fdbList->count);
		list = &(fdbList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct igmp_snooping_list_node_s,list);
			if(vidx == node->vidx )/*&& groupAddr == node->groupIp) {*/
			{	
				syslog_ax_vlan_dbg("npd vlan check igmp fdb entry: vidx %d groupIp 0x%x in vlan %d.",vidx,node->groupIp,vlanId);
				/*when it's the v6 vidx,the groupIp is 0*/
				if(0 == node->groupIp){
					iptemp |= node->grouIpV6[6];
					iptemp = iptemp << 16;
					iptemp |= node->grouIpV6[7];
					*groupAddr = iptemp;

					npd_mld_copy_ipv6addr(node->grouIpV6,groupAddrv6);
					
					*ret = NPD_TRUE;
				}
				else {
					*groupAddr = node->groupIp;
					*ret = NPD_FALSE;
				}
				return IGMPSNP_RETURN_CODE_OK;
			}
		}
	}
	syslog_ax_vlan_dbg("vidx %d NOT exist of vlan %d!",vidx,vlanId);
	return IGMPSNP_RETURN_CODE_ERROR;
}
/**********************************************************************************
 *  npd_igmp_fdb_entry_addto
 *
 * DESCRIPTION:
 * 		add a fdb entry to vlanNode list
 *
 * INPUT:
 *		vid - vlan id
 *		groupIp - group Ip address
 *		vidx - the vlan index about group
 *	
 * OUTPUT:
 *		NULL
 *
 *  RETURN:
 *		IGMPSNP_RETURN_CODE_OK - add success
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - can not find vlannode by vid
 *		IGMPSNP_RETURN_CODE_ALLOC_MEM_NULL - melloc memery fail
 *
 **********************************************************************************/
unsigned int npd_igmp_fdb_entry_addto
(
	unsigned short vid,
	unsigned int groupIp,
	unsigned short vidx
)
{
	syslog_ax_vlan_dbg("vlanId %d,vidx %d, groupIp 0x%x.\n",vid,vidx,groupIp);
	struct vlan_s *vlanNode = NULL;
	igmp_snooping_list_s *fdbList = NULL;
	struct igmp_snooping_list_node_s *node = NULL;

	vlanNode = npd_find_vlan_by_vid(vid);
	if(NULL == vlanNode){
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}
	
	fdbList = vlanNode->igmpList;
	
	if(NULL == fdbList) { /* create port list */
		/* syslog_ax_vlan_dbg("npd vlan %d add first %s port %#0x",vlanId,isTagged ? "tag":"untag",eth_g_index);*/
		fdbList = (igmp_snooping_list_s *)malloc(sizeof(igmp_snooping_list_s));
		if(NULL == fdbList) {
			 syslog_ax_vlan_err("npd vlan %d add igmpfdb entry with vidx %d memory err",	
					vid,vidx);
			return IGMPSNP_RETURN_CODE_ALLOC_MEM_NULL;
		}
		else {
			memset(fdbList, 0, sizeof(igmp_snooping_list_s));
			INIT_LIST_HEAD(&(fdbList->list));
				vlanNode->igmpList = fdbList;
		}
	}

	node = (struct igmp_snooping_list_node_s *)malloc(sizeof(struct igmp_snooping_list_node_s));
	if(NULL == node) {
		 syslog_ax_vlan_err("npd vlan %d add igmp entry vidx %d groupId 0x%x to %s list error:null memory alloc", 
					vid, vidx,groupIp);
		return IGMPSNP_RETURN_CODE_ALLOC_MEM_NULL;
	}
	memset(node,0,sizeof(struct igmp_snooping_list_node_s));
	node->vidx = vidx;
	node->groupIp = groupIp;
	list_add(&(node->list),&(fdbList->list));
	fdbList->count++;
	syslog_ax_vlan_dbg("######vlan %d current igmpList node count %d,\n#####igmpList Node->vidx %d,\n######node->groupIp 0x%x.\n",\
								vid,fdbList->count,node->vidx,node->groupIp);

	return IGMPSNP_RETURN_CODE_OK;
}

/**********************************************************************************
 *  npd_igmp_fdb_entry_delfrom
 *
 * DESCRIPTION:
 * 		delete a fdb entry from vlanNode 
 *
 * INPUT:
 *		vlanId - vlan id
 *		groupIp - group Ip address
 *		vidx - the vlan index about group
 *	
 * OUTPUT:
 *		NULL
 *
 *  RETURN:
 *		IGMPSNP_RETURN_CODE_OK - delete success
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - can find vlannode by vid
 *		IGMPSNP_RETURN_CODE_NULL_PTR - the fdblist is null
 *
 **********************************************************************************/
unsigned int npd_igmp_fdb_entry_delfrom
(
	unsigned short vid,
	unsigned short vidx
)
{
	struct vlan_s *vlanNode = NULL;
	igmp_snooping_list_s *fdbList = NULL;
	struct igmp_snooping_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	unsigned int delflag = 0;
	vlanNode = npd_find_vlan_by_vid(vid);
	if(NULL == vlanNode) {
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}

	fdbList = vlanNode->igmpList;
	
	if(NULL == fdbList) {
		 syslog_ax_vlan_err("npd del vidx %d from vlan %d null fdbList err!",vidx,vidx);
		return IGMPSNP_RETURN_CODE_NULL_PTR;
	}
	else {	
		list = &(fdbList->list);
		if(NULL ==list){
			syslog_ax_vlan_dbg("No fdb entry node in vlan,NULL ==fdbList->list !");
			return IGMPSNP_RETURN_CODE_NULL_PTR;
		}
		__list_for_each(pos,list) {
			node = list_entry(pos,struct igmp_snooping_list_node_s,list);
			if( NULL != node && vidx == node->vidx)/* && groupIp == node->groupIp) */
			{
				list_del(&(node->list));
				delflag= 1;
				break;
			}
		}
		if(NULL != node && 1 == delflag){
			free(node);
			node = NULL;
			if(fdbList->count > 0) {
				fdbList->count--;
			}
		}
		/*
		 syslog_ax_vlan_dbg("npd vlan %d current %s port node count %d",	\
						vlanId, isTagged ? "tag":"untag",portList->count);
		*/
		if(0 == fdbList->count) {
			if(NULL != fdbList){
				free(fdbList);
				fdbList = NULL;
			}
			vlanNode->igmpList = NULL;
		}
	}

	return IGMPSNP_RETURN_CODE_OK;
}

unsigned int npd_igmp_clear_fdb_list
(	
	unsigned short vid[],
	unsigned int groupAddr[],
	unsigned short vidxArry[],
	unsigned int *fdbCount)
{
	unsigned short vlanId,vidx,tmpFdbCnt = 0;
	unsigned int ret = IGMPSNP_RETURN_CODE_OK;
	unsigned int npdret = 0;
	struct vlan_s *vlanNode = NULL;
	unsigned short groupAddrv6[SIZE_OF_IPV6_ADDR];
	unsigned short *pnpdaddr = NULL;
	
	for(vlanId = 2;vlanId<NPD_VLAN_NUMBER_MAX;vlanId++)
	{
		if (VLAN_RETURN_CODE_VLAN_EXISTS==npd_check_vlan_exist(vlanId))
		{
			if(NULL != (vlanNode = npd_find_vlan_by_vid(vlanId)))
			{
				if(NPD_TRUE == vlanNode->igmpSnpEnDis)
				{
					for(vidx = 1;vidx < NPD_IGMP_SNP_GROUP_NUM_MAX;vidx++)
					{
						if(IGMPSNP_RETURN_CODE_OK == npd_igmp_check_contain_fdb(vlanId,vidx))
						{
								pnpdaddr = groupAddrv6;
								if(IGMPSNP_RETURN_CODE_OK == npd_igmp_get_groupip_by_vlan_vidx(vlanId,vidx,&groupAddr[tmpFdbCnt],&pnpdaddr,&npdret))
								{
									ret = npd_igmp_fdb_entry_delfrom(vlanId,vidx);
									if(IGMPSNP_RETURN_CODE_OK != ret)
									{
										continue;	/* code optimize:  Unchecked return value  houxx@autelan.com  2013-1-18 */ 
									}
									syslog_ax_igmp_dbg("vid %d, vidx %d, tmpFdbCnt %d\n", vlanId, vidx, tmpFdbCnt);
									vid[tmpFdbCnt] = vlanId;
									vidxArry[tmpFdbCnt] = vidx;
									tmpFdbCnt++;
								}
						}
						else
						{
							ret = IGMPSNP_RETURN_CODE_GROUP_NOTEXIST;
							continue;
						}
					}
				}
				else 
				{
					ret=IGMPSNP_RETURN_CODE_NOT_SUPPORT_IGMP_SNP;
					continue;
				}
			}
			else 
			{
				ret = IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
				continue;
			}
		}
		else 
		{
			ret = IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
			continue;
		}
	}
	
	*fdbCount = tmpFdbCnt;
	if(0 == tmpFdbCnt)	
		return ret;
	else 
		return IGMPSNP_RETURN_CODE_OK;
}

/*******************************************************************************
 * npd_igmp_clear_fdb_list_new
 *
 * DESCRIPTION:
 *   		when set igmp disable ,delete all FDB entries by  vlanId
 *
 * INPUTS:
 * 		null
 *
 * OUTPUTS:
 *    	null
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_NOT_SUPPORT_IGMP_SNP - the vlan not support igmp snooping yet
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - can find the vlan by vlanId
 *		IGMPSNP_RETURN_CODE_OK - delete the FDB entries success
 *
 * COMMENTS:
 *   
 **
 ********************************************************************************/
unsigned int npd_igmp_clear_fdb_list_new()
{
	unsigned short vlanId = 0;
	unsigned int ret = IGMPSNP_RETURN_CODE_OK;
	struct vlan_s *vlanNode = NULL;
	
	for (vlanId = 2; vlanId < NPD_IGMP_SNP_GROUP_NUM_MAX; vlanId++)	/* code optimize: Out-of-bounds access houxx@autelan.com  2013-1-18 */
	{
		if (VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(vlanId)) 
		{
			if( NULL != (vlanNode = npd_find_vlan_by_vid(vlanId))) 
			{
				if (NPD_TRUE == vlanNode->igmpSnpEnDis) 
				{
					ret = npd_vlan_igmp_snp_endis_config(NPD_FALSE, vlanId);
				}
				else 
				{
					ret= IGMPSNP_RETURN_CODE_NOT_SUPPORT_IGMP_SNP;
					continue;
				}
			}
			else
			{
				ret = IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
				continue;
			}
		}
		else
		{
			ret = IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
			continue;
		}
	}
	
	return ret;
}
/*******************************************************************************
 * npd_l2mc_entry_mbr_add
 *
 * DESCRIPTION:
 *	vidx- L2mc entry add member, A bit in bitmap set to 1. 
 *	when the mcgroup membr is the first port,this is create new l2mc entry.
 *
 * INPUTS:
 * 		vlanId - vlan id
 *		vidx - vlan's vidx
 *		eth_g_index - the index we  can device number and port number
 *		group_ip - the multiple group address
 *
 * OUTPUTS:
 *    	null
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - layer 2 entry add success
 *		IGMPSNP_RETURN_CODE_ERROR -  get the devNum and portNum error or add layer 2 entry error
 *
 * COMMENTS:
 *    
 **
 ********************************************************************************/
int npd_l2mc_entry_mbr_add
(
	unsigned short vlanId,
	unsigned short vidx,
	unsigned int   eth_g_index,
	unsigned int   group_ip
)
{
	unsigned long ret = IGMPSNP_RETURN_CODE_OK;
	unsigned char devNum,portNum;
	/*l2mc_list	  *mcGroupMbrNode;*/
	
	syslog_ax_igmp_event("add multicast entry to layer2\n");
	/*add the port from vidx entry*/
	ret = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != ret) {
		ret = IGMPSNP_RETURN_CODE_ERROR;
	}
	else {
		ret = nam_asic_l2mc_member_add(devNum,vidx,portNum,group_ip, vlanId);
		if(0 == ret){
			syslog_ax_igmp_dbg("##################IGMP SNOOPING NOTIFY MESSAGE##################\n");
			syslog_ax_igmp_dbg("NPD_IGMP :nam_asic_l2mc_member_add :devNum %d,vidx %d,portNum %d!\n",\
							devNum,vidx,portNum);
			v2McGroupArry[vlanId].vidBit = 1;
			v2McGroupArry[vlanId].mbrArray[vidx] = 1;/*set 1 :indicates the vidx entry valid*/
			syslog_ax_igmp_dbg("set v2McGroupArry[%d].vidBit,v2McGroupArry[%d].mbrArray[%d] to 1\n",\
							vlanId,vlanId,vidx);
			syslog_ax_igmp_dbg("##################IGMP SNOOPING NOTIFY MESSAGE##################\n");
            ret = IGMPSNP_RETURN_CODE_OK;
		}
		else{
			syslog_ax_igmp_err("NPD_IGMP :nam asic l2mc member add :devNum %d,vidx %d,portNum %d ERROR!",\
							devNum,vidx,portNum);
            ret = IGMPSNP_RETURN_CODE_ERROR;
			}
	}
	return ret;
}
/**************************************
 *
 *	vidx- L2mc entry delete member
 *	a bit in bitmap clear to 0.
 *
 *************************************/
int npd_l2mc_entry_mbr_del
(
	unsigned short vlanId,
	unsigned short vidx, 
	unsigned int eth_g_index,
	unsigned int group_ip
)
{
	unsigned long ret = IGMPSNP_RETURN_CODE_OK;
	unsigned char mbrLeft = 1;/*group has member left still*/
	unsigned char devNum,portNum;
	
	/*delete the port from vidx entry*/
	ret = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	syslog_ax_igmp_dbg("npd_l2mc_entry_mbr_del:: vid %d,vidx %d,eth_g_index %d <-->devNum %d, portNum %d.\n",
							vlanId,vidx,eth_g_index,devNum,portNum);
	if(NPD_SUCCESS != ret) {
		ret = IGMPSNP_RETURN_CODE_ERROR;
	}
	else {
		ret = nam_asic_l2mc_member_del(devNum,vidx,portNum,group_ip,vlanId,&mbrLeft);
		if((0 == ret) && (0 ==mbrLeft)){
			v2McGroupArry[vlanId].vidBit = 1;
			v2McGroupArry[vlanId].mbrArray[vidx] = 0;/*set 0 :the mbrLeft is 0,indicates the vidx entry invalid*/
			ret = IGMPSNP_RETURN_CODE_OK;
		}
	}
	return ret;
}
/*******************************************************************************
 * npd_l2mc_entry_delete
 *
 * DESCRIPTION:
 *   		vidx- L2mc entry delete ,multigroup destroy.
 *
 * INPUTS:
 * 		vlanId - vlan id
 *		vidx - vlan's vidx
 *		group_ip - the multiple group address
 *
 * OUTPUTS:
 *    	null
 *
 * RETURNS:
 *		ret - teturns from nam_asic_l2mc_group_del
 *
 * COMMENTS:
 *    
 **
 ********************************************************************************/
int npd_l2mc_entry_delete
(
	unsigned short vlanId,
	unsigned short vidx,
	unsigned int   group_ip
)
{
	unsigned long ret ;
	unsigned char devNum = 0;
	
	/*delete the port from vidx entry*/
	ret = nam_asic_l2mc_group_del(devNum,vidx,group_ip, vlanId);
	if(0 == ret){
		v2McGroupArry[vlanId].vidBit = 1;
		v2McGroupArry[vlanId].mbrArray[vidx] = 0;/*set 0 :indicates the vidx entry invalid*/
	}
	return ret;
}

int npd_l2mc_entry_delete_all(void)
{
	unsigned long ret = 0;	/* code optimize: Uninitialized scalar variable houxx@autelan.com  2013-1-18 */
	unsigned int i = 0,vidx = 0;
	
	/*delete the port from vidx entry*/
	for(i=0;i<NPD_IGMP_SNP_GROUP_NUM_MAX; i++){
		if(IGMPSNP_RETURN_CODE_MC_VLAN_NOT_EXIST == npd_igmp_snp_l2mc_vlan_exist(i)){continue;}
		else 
		{
			for(vidx = 0;vidx<NPD_IGMP_SNP_GROUP_NUM_MAX; vidx++)
			{
				if(NPD_GROUP_NOTEXIST == npd_igmp_snp_l2mc_group_exist(i,vidx))
					continue;
				else
					/*ret = nam_asic_l2mc_group_del(devNum, vidx);*/
					continue;
			}
		}
	}
	if(0 == ret){
		memset(v2McGroupArry,0,sizeof(groupBasedVlan)*NPD_IGMP_SNP_GROUP_NUM_MAX);
	}
	return ret;
}
/*******************************************************************************
 * npd_default_vlan_port_mbr_igmp_enable
 *
 * DESCRIPTION:
 *   		config all member of default vlan IGMP enable
 *
 * INPUTS:
 * 		vlanId - the default vlan id is 1.
 *		enDis - config the default vlan enable or disable
 *
 * OUTPUTS:
 *    	null
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_ERROR - set the enDis flag error
 *
 * COMMENTS:
 *      
 **
 ********************************************************************************/
int npd_default_vlan_port_mbr_igmp_enable
(
	unsigned short vlanId,
	unsigned int enDis 
)
{
	unsigned int i,mbr_count = 0,ret = IGMPSNP_RETURN_CODE_ERROR;
	unsigned int default_vlan_mbr[MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT];
	unsigned char enable = NPD_TRUE;
	
	if (enDis == 1) {
		enable = NPD_TRUE;
	}else if (enDis == 0) {
		enable = NPD_FALSE;
	}else {
		syslog_ax_igmp_dbg("default vlan %d enable/disable all ports igmp-snooping error.\n", vlanId);
		return IGMPSNP_RETURN_CODE_ERROR;
	}

	ret = npd_vlan_member_port_index_get(vlanId, NPD_FALSE, default_vlan_mbr, &mbr_count);
	syslog_ax_igmp_dbg("default vlan %d untag port count %d.\n",vlanId, mbr_count);
	if(NPD_TRUE == ret)
	{
		for(i=0;i<mbr_count;i++)
		{
			usleep(1000);
			ret = npd_vlan_port_igmp_snp_endis_config(enable,\
												vlanId,\
												default_vlan_mbr[i],\
												NPD_FALSE);
			if(IGMPSNP_RETURN_CODE_OK != ret ){
				syslog_ax_igmp_err("default vlan 1 port %d %s igmp Error,ret %d.\n",
								default_vlan_mbr[i],
								enable ? "enable" : "disable",
								ret);
			}
		}
	}
	return ret;		/* code optimize:Missing return statement houxx@autelan.com  2013-1-18 */
}
/*******************************************************************************
 * npd_igmp_snp_recvmsg_proc
 *
 * DESCRIPTION:
 *		 config dev according to running result of igmp snoop protocol
 *
 * INPUTS:
 * 		igmp_notify - notify message
 *
 * OUTPUTS:
 *    	null
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - config success
 *
 * COMMENTS:
 *  
 **
 ********************************************************************************/
unsigned int npd_igmp_snp_recvmsg_proc(struct igmp_notify_mod_npd* igmp_notify)
{
	unsigned int iter,ret = 0;
	unsigned short vlanId = 0;
	unsigned int eth_g_index = 0;
	unsigned int group_ip = 0;
	unsigned short vidx = 0;
	unsigned int igmpSysSet =0;/*fdb_entry_exist = 0;*/

	vlanId = igmp_notify->vlan_id;
	eth_g_index = (unsigned int)igmp_notify->ifindex;
	group_ip = (unsigned int)igmp_notify->groupadd ;
	vidx = igmp_notify->reserve;
	
	syslog_ax_igmp_dbg("npd receive message from igmp_snp:get igmpMsg:type %d, vlan_id %d, eth_g_index %d,group_ip 0x%x, %s %d.\n",
						igmp_notify->mod_type, vlanId, eth_g_index, group_ip,
						(IGMP_SYS_SET ==igmp_notify->mod_type)?"reserve":"vidx",
						igmp_notify->reserve);
#if 0
	int fdbCont = 0;
	unsigned short vid[NPD_IGMP_SNP_GROUP_NUM_MAX] = {0};
	unsigned int groupip[NPD_IGMP_SNP_GROUP_NUM_MAX] = {0};
	unsigned short vidxArray[NPD_IGMP_SNP_GROUP_NUM_MAX] = {0};
#endif
	switch(igmp_notify->mod_type)
	{
		case IGMP_ADDR_ADD:
		{



			/*FDB Entry operation,if fdb already exists,do nothing on Sw.*/
			if(IGMPSNP_RETURN_CODE_OK != npd_igmp_check_contain_fdb(vlanId,vidx)){
				ret = npd_igmp_fdb_entry_addto(vlanId,group_ip,vidx);
				if(IGMPSNP_RETURN_CODE_OK == ret ){
					/*the FDB entry is a New one,Here to config on HW*/
					syslog_ax_igmp_dbg("Here to set FDB entry On HW,group_ip 0x%x,vidx %d,vlan %d!\n",group_ip,vidx,vlanId);
					ret = nam_static_fdb_entry_indirect_set_for_igmp(group_ip,vidx,vlanId);
					syslog_ax_igmp_dbg("Set FDB entry on HW,ret =%d.\n",ret);
				}
			}

			/*vidx = igmp_notify->reserve;*/
			/*eth_g_index is a new member of vidx*/
			ret = npd_l2mc_entry_mbr_add(vlanId,vidx,eth_g_index, group_ip);
			
			syslog_ax_igmp_dbg("npd_l2mc_entry_mbr_add ret %d!\n", ret);
			break;
		}
		case IGMP_ADDR_DEL:
		{
			/*vidx = igmp_notify->reserve;*/
			ret = npd_l2mc_entry_mbr_del(vlanId,vidx,eth_g_index,group_ip);
			if(IGMPSNP_RETURN_CODE_OK != npd_l2mc_entry_mbr_exist(vidx))
			{
				/*L2mc GROUP empty!!*/
				/*delete l2mc entry*/
				npd_l2mc_entry_delete(vlanId,vidx,group_ip);

				/*delete fdb entry*/
				if(IGMPSNP_RETURN_CODE_OK != npd_igmp_check_contain_fdb(vlanId,vidx)){
					ret = npd_igmp_fdb_entry_delfrom(vlanId,vidx);
					if(IGMPSNP_RETURN_CODE_OK == ret ){
						syslog_ax_igmp_dbg("Here to delete FDB entry On HW,group_ip 0x%x,vidx %d,vlan %d!",group_ip,vidx,vlanId);
						nam_static_fdb_entry_indirect_delete_for_igmp(group_ip,vlanId);
					}
				}
			}
			/*npd_show_vidx_member(vlanId,vidx);*/
			break;
		}
		case IGMP_ADDR_RMV:
		{
			/*vidx = igmp_notify->reserve;*/
			if(IGMPSNP_RETURN_CODE_OK == npd_igmp_check_contain_fdb(vlanId,vidx)){
				ret = npd_igmp_fdb_entry_delfrom(vlanId,vidx);
				if(IGMPSNP_RETURN_CODE_OK == ret ){
					/*Here to delete FDB entry on HW*/
					syslog_ax_igmp_dbg("Here to delete FDB entry On HW,group_ip 0x%x,vidx %d,vlan %d!",group_ip,vidx,vlanId);
					nam_static_fdb_entry_indirect_delete_for_igmp(group_ip,vlanId);
					ret = npd_l2mc_entry_delete(vlanId,vidx,group_ip);
				}
			}
			break;
		}
		case IGMP_SYS_SET:
		{
			igmpSysSet = igmp_notify->reserve;
			if( IGMP_SYS_SET_INIT == igmpSysSet)
			{
				/*set igmp snooping Enable Global Flag */
				if(!igmpSnpEnable){igmpSnpEnable = 1;}
				syslog_ax_igmp_dbg("Set IGMP Snooping Enable Global Flag: igmpSnpEnable %d.\n",igmpSnpEnable);
				/*transfer Product system MAC address to igmp snooping*/
				ret = npd_igmp_sysmac_notifer();
				if(IGMPSNP_RETURN_CODE_OK != ret){
					syslog_ax_igmp_dbg("npd sysmac notifier error!\n");
				}

				/*add DEFAULT vlan,and all port member of default vlan -- 08/09/23*/
				/*set igmp trap enable on Default vlan*/
				ret = npd_vlan_igmp_snp_endis_config(NPD_TRUE,1);

				/*add default vlan port member to igmp snooping*/
				npd_default_vlan_port_mbr_igmp_enable(1,NPD_TRUE);
			}
			else if(IGMP_SYS_SET_STOP == igmpSysSet)
			{
				/*clear igmp snooping Enable Global Flag */
				igmpSnpEnable = 0;
				syslog_ax_igmp_dbg("Clear IGMP Snooping Enable Global Flag: igmpSnpEnable %d.\n",igmpSnpEnable);
#if 0
				/*igmp Disable:delete all FDB entries which interfaceType = Vidx,
				  Vidx membership entries,*/
				ret = npd_igmp_clear_fdb_list(vid,groupip,vidxArray,&fdbCont);
				syslog_ax_igmp_dbg("disable_error:npd_igmp_clear_fdb_list: fdbCont = %d, ret = %d.\n", fdbCont, ret);
				if(NPD_SUCCESS == ret ){
					for(iter = 0;iter<fdbCont;iter++){
						nam_static_fdb_entry_indirect_delete_for_igmp(groupip[iter],vid[iter]);
						npd_l2mc_entry_delete(vid[iter], vidxArray[iter]);
					}
				}

				/*clear vlan igmp enable flag ;port igmp enable & route port flag*/
				npd_vlan_portmbr_igmp_flags_clear();
#endif
				/*igmp Disable:delete all FDB entries which interfaceType = Vidx,
				  Vidx membership entries,*/
				ret = npd_igmp_clear_fdb_list_new();
				syslog_ax_igmp_dbg("npd_igmp_clear_fdb_list_new completed");

				/*disable igmp snooping on DEFAULT vlan --08/09/23*/
				ret = npd_vlan_igmp_snp_endis_config(NPD_FALSE, 1);

				/*disable default vlan port member to igmp snooping */
				npd_default_vlan_port_mbr_igmp_enable(1,NPD_FALSE);
				/*ret = npd_l2mc_entry_delete_all();*/
			}
			break;	
		}
		default :
		{
			syslog_ax_igmp_dbg("npd can NOT proccess the running result of Protocol");
			break;
		}
	}
	return IGMPSNP_RETURN_CODE_OK;
}
 /*******************************************************************************
  * npd_igmp_sysmac_notifer
  *
  * DESCRIPTION:
  * 	 	transfer Product system MAC address to igmp snooping	
  *
  * INPUTS:
  *  		null
  *
  * OUTPUTS:
  * 	 	null
  *
  * RETURNS:
  *		IGMPSNP_RETURN_CODE_OK - the mac address is ok and send ok
  *		IGMPSNP_RETURN_CODE_OUT_RANGE - the mac address out of range
  *		IGMPSNP_RETURN_CODE_ERROR - send commad to igmp error
  *
  * COMMENTS:
  * 	 
  **
  ********************************************************************************/
 int npd_igmp_sysmac_notifer(void)
 {
 	unsigned int i,ret = IGMPSNP_RETURN_CODE_OK;
	struct npd_dev_event_cmd npdMngMsg;
	unsigned char macByte = 0,sysMac[13] = {0};
	npdMngMsg.event_type = NPD_IGMPSNP_EVENT_DEV_SYS_MAC_NOTI;
	npdMngMsg.vid = 0;
	npdMngMsg.port_index = 0;

	memcpy(sysMac, (unsigned char*)PRODUCT_MAC_ADDRESS,13);
	for(i = 0;i < 12;i++) {
		if(('0' <= sysMac[i])&&(sysMac[i] <= '9')) {
			macByte = sysMac[i] - '0';
		}
		else if(('a' <= sysMac[i])&&(sysMac[i] <= 'f')) {
			macByte = sysMac[i] - 'a' + 10;/* wujh@autelan.com 08/09/24*/
		}
		else if(('A' <= sysMac[i])&&(sysMac[i] <= 'F')) {
			macByte = sysMac[i] - 'A' + 10;/* wujh@autelan.com 08/09/24*/
		}
		else {
			syslog_ax_igmp_dbg("Bad macaddr range!\n");
			return IGMPSNP_RETURN_CODE_OUT_RANGE;
		}
		
		if(0 == (i%2)) {
			transMac[i/2] = macByte;
		}
		else {
			transMac[i/2] <<= 4;
			transMac[i/2] |= macByte;
		}
	}

	syslog_ax_igmp_dbg("Packet etherHeader sMac =%02x:%02x:%02x:%02x:%02x:%02x.\n",\
		transMac[0],transMac[1],transMac[2],transMac[3],transMac[4],transMac[5]);
	
	syslog_ax_igmp_dbg("Enter::npd_igmp_sysmac_notifer...\n");
	ret = npd_cmd_sendto_igmpsnp(&npdMngMsg);
	if(IGMPSNP_RETURN_CODE_OK != ret ){
		syslog_ax_igmp_dbg("npd mng msg write fail.\n");
		return IGMPSNP_RETURN_CODE_ERROR;
	}
	return IGMPSNP_RETURN_CODE_OK;
 }
 /*******************************************************************************
  * npd_igmp_port_link_change
  *
  * DESCRIPTION:
  *		when the igmp port link state change ,send the commad to igmp_snooping
  *
  * INPUTS:
  *  		eth_g_index - ether global index include device and port number
  *		event - the port notifier event 
  *
  * OUTPUTS:
  * 	 	null
  *
  * RETURNS:
  *		ret - get the returns from npd_cmd_sendto_igmpsnp.
  *
  * COMMENTS:
  * 	 
  **
  ********************************************************************************/
int npd_igmp_port_link_change
(
	unsigned int eth_g_index,	
	enum ETH_PORT_NOTIFIER_ENT event
)
{
	unsigned int ret = IGMPSNP_RETURN_CODE_OK;
	struct npd_dev_event_cmd npdMngMsg;

	if( ETH_PORT_NOTIFIER_LINKUP_E == event) {
		npdMngMsg.event_type = NPD_IGMPSNP_EVENT_DEV_UP;
		npdMngMsg.port_index = eth_g_index;
		npdMngMsg.vid = 0;
	}
	if( ETH_PORT_NOTIFIER_LINKDOWN_E == event){
		npdMngMsg.event_type = NPD_IGMPSNP_EVENT_DEV_DOWN;
		npdMngMsg.port_index = eth_g_index;
		npdMngMsg.vid = 0;
	}

	ret = npd_cmd_sendto_igmpsnp(&npdMngMsg);

	return ret;
}
/**********************************************************************************
*igmp_snp_init()
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*	
*DESCRIPTION:
*	IGMP SNOOP init Global igmp Snooping Enable Flag 
*	and FDB entry listhead and l2mc GroupArray
*
***********************************************************************************/
void igmp_snp_init(void){

	/*init Global igmp Snooping Enable Flag*/
	igmpSnpEnable = 0;

	/*init FDB entry listhead*/
	mcFdbListHead = (mc_fdb_list_head*)malloc(sizeof(mc_fdb_list_head));
	mcFdbListHead->stListHead = NULL;
	mcFdbListHead->ulListNodeCount = 0;

	/*init l2mc GroupArray*/
	memset(v2McGroupArry,0,NPD_IGMP_SNP_GROUP_NUM_MAX*sizeof(groupBasedVlan));

}

/**********************************************************************************
*create_msg_thread_dgram()
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*	
*DESCRIPTION:
*	IGMP SNOOP command message handle thread
*
***********************************************************************************/
void *npd_igmp_snp_mng_thread_dgram(void)
{
	int ret;
	int sock = NPD_IGMPSNP_FD;
	int recv_len = 0;
	struct igmp_msg_npd *igmp_msg = NULL;
	struct mld_msg_npd *mld_msg = NULL;
	struct igmp_notify_mod_npd *notify_msg = NULL;
	struct mld_notify_mod_npd *mld_notify_msg = NULL;
	#if 1
	char buf[IGMP_MSG_MAX_SIZE];
	#else
	char buf[1024] = {0};
	#endif
	
	fd_set rfds;

	recv_len = IGMP_MSG_MAX_SIZE;
	/* tell my thread id*/
	npd_init_tell_whoami("IgmpSnpSock",0);

	igmp_snp_init();

	/*create socket communication*/
	if(IGMPSNP_RETURN_CODE_OK != npd_igmp_snp_sock_init(&sock))
	{
		syslog_ax_igmp_err("create igmp snooping manage thread socket failed.\r\n");
		return NULL;
	}
	igmp_fd = sock ;
	syslog_ax_igmp_dbg("create igmp snooping manage thread socket fd %d ok\n",sock);

	while(1)
	{
		memset(buf,0,sizeof(char)*IGMP_MSG_MAX_SIZE);
		FD_ZERO(&rfds);
		FD_SET(igmp_fd,&rfds);
		switch(select(igmp_fd+1,&rfds,NULL,NULL,NULL))
		{
			case -1:
				break;
			case 0:
				break;
			default:
				if(FD_ISSET(igmp_fd,&rfds))
				{
					ret = npd_igmpsnp_recv_info(buf,IGMP_MSG_MAX_SIZE,&recv_len);					
					if( 0 == recv_len )
						break;
					/* distinguish the igmp type and mld type */
					mld_msg = (struct mld_msg_npd* )buf;
					if(MLD_SNP_TYPE_NOTIFY_MSG == mld_msg->nlh.nlmsg_type){
						if(MLD_SNP_FLAG_ADDR_MOD == mld_msg->nlh.nlmsg_flags){
							mld_notify_msg = (struct mld_notify_mod_npd *) &(mld_msg->mld_noti_npd);
							syslog_ax_igmp_dbg("recv info from mld ok,entry mld recvmsg proc.\n");
							ret = npd_mld_snp_recvmsg_proc(mld_notify_msg);
						}
					}
					else if(IGMP_SNP_TYPE_NOTIFY_MSG == mld_msg->nlh.nlmsg_type){
						igmp_msg = (struct igmp_msg_npd* )buf;						
						if(IGMP_SNP_FLAG_ADDR_MOD == igmp_msg->nlh.nlmsg_flags){
							notify_msg = (struct igmp_notify_mod_npd*) &(igmp_msg->igmp_noti_npd);
							syslog_ax_igmp_dbg("recv info from igmp ok,entry igmp recvmsg proc.\n");
							ret = npd_igmp_snp_recvmsg_proc(notify_msg);
						}
					}
					break;
				}
		}
		if(0 == recv_len)
			break;
	}
	close(igmp_fd);
}

#if 0
/**********************************************************************************
*create_msg_thread()
*INPUTS:
*
*OUTPUTS:
*RETURN VALUE:
*	
*DESCRIPTION:
*	IGMP SNOOP command message handle thread
*
***********************************************************************************/
void *npd_igmp_snp_mng_thread(void)
{
	int ret;
	int sock,accept_sock = 0,len;
	int recv_len = 0,retval =0;
	struct sockaddr_un client;
	struct igmp_skb *msg = NULL;
	struct igmp_notify_mod_pkt *notify_msg =NULL;
	char buf[IGMP_MSG_MAX_SIZE];
	fd_set rfds;

	igmp_snp_init();

	/*create socket communication*/
	if(0 >(sock = creatservsock_stream(IGMP_SNOOP_NPD_MSG_SOCK)))
	{
		printf("create_msg_thread::Create msg socket failed.\r\n");
		return NULL;/*-1 -->NULL*/
	}

	//igmp_fd = sock;/*communication with NPD,be used by send_msg() in igmp_snoop_hw.c*/

	syslog_ax_igmp_dbg("create_igmp_snp_mng_thread::Create msg socket ok,MsgSock fd=%d.",sock);
	memset(&client,0,sizeof(struct sockaddr_un));
	len = sizeof(struct sockaddr_un);

	msg = (struct igmp_skb *)buf;
	while(1)
	{
		if((igmp_fd = accept(sock,(struct sockaddr *)&client,&len))<0)/*accept_sock-->igmp_fd*/
		{
			printf("Accept failed:errno %d [%s].\r\n",errno,strerror(errno));
			close(sock);
			/*need send signal for other threads*/
			return NULL; /*'-1' -->NULL*/
		}
		//igmp_fd = accept_sock;/*communication with NPD,be used by send_msg() in igmp_snoop_hw.c*/
		printf("create_igmp_snp_mng_thread:accept socket ok,accept fd = %d.\r\n",igmp_fd);
		while(1)
		{
			memset(buf,0,sizeof(char)*IGMP_MSG_MAX_SIZE);
			FD_ZERO(&rfds);
			FD_SET(igmp_fd,&rfds);
			switch(select(igmp_fd+1,&rfds,NULL,NULL,NULL))
			{
				case -1:
					break;
				case 0:
					break;
				default:
					if(FD_ISSET(igmp_fd,&rfds))
					{
						/*
						recv_len = read(igmp_fd,msg,IGMP_MSG_MAX_SIZE);
						syslog_ax_igmp_dbg("Npd read msg form IGMP,recv_len %d!",recv_len);
						*/

						/*
						recv_len = read_npdsocket(igmp_fd,msg,IGMP_MSG_MAX_SIZE);
						*/
						recv_len= recv(igmp_fd,msg,IGMP_MSG_MAX_SIZE,0);
						if(recv_len < 0){
							syslog_ax_igmp_dbg("++++++++++++++++++++++++++");
							if(EAGAIN == retval){npd_syslog_err("recv");}
							if(EBADF == retval){npd_syslog_err("recv");}
							if(ECONNREFUSED == retval){npd_syslog_err("recv");}
							if(EFAULT == retval){npd_syslog_err("recv");}
							if(EINTR == retval){npd_syslog_err("recv");}
							if(EINVAL == retval){npd_syslog_err("recv");}
							if(ENOMEM == retval){npd_syslog_err("recv");}
							if(ENOTCONN == retval){npd_syslog_err("recv");}
							if(ENOTSOCK == retval){npd_syslog_err("recv");}
							break;
						}
						if( 0 == recv_len )
							break;
						if(IGMP_SNP_TYPE_NOTIFY_MSG == msg->nlh.nlmsg_type){
							if(IGMP_SNP_FLAG_ADDR_MOD == msg->nlh.nlmsg_flags){
								notify_msg =(struct igmp_notify_mod_pkt*)msg->mngbuf;
								ret = igmp_snp_recvmsg_proc(notify_msg);
							}
						}
						break;
					}
			}
			if(0 == recv_len )
				break;
		}
		close(igmp_fd);
	}
	
}
#endif

/*******************************************************************************
 * npd_vlan_igmp_snp_endis_config
 *
 * DESCRIPTION:
 *   		enable/disable IGMP Snooping in vlan 
 *
 * INPUTS:
 * 		isAdd - enalbe or disable flag 
 *		vid - vlan id
 *
 * OUTPUTS:
 *    	null	
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - config endis success
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - can not find the vlan 
 *		IGMPSNP_RETURN_CODE_ERROR_HW - config hardware error
 *
 * COMMENTS:
 *      
 **
 ********************************************************************************/
int npd_vlan_igmp_snp_endis_config
(
	unsigned char	isAdd,
	unsigned short	vid
)
{
	syslog_ax_igmp_dbg("Enter npd_vlan_igmp_snp_endis_config ...\n");
	unsigned char devNum = 0,portNum = 0;
	unsigned char enDis = isAdd;
	unsigned int vidx = 0, groupAddr = 0, npdret = 0;
	
	struct npd_dev_event_cmd npdMngMsg;
	unsigned int i,eth_g_index[MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT],mbrCnt = 0,ret = IGMPSNP_RETURN_CODE_OK;
	
	struct vlan_s *vlanNode = NULL;
	struct eth_port_s *ethPort = NULL ;
	unsigned int tmpAttrBitmap = 0;
	unsigned short groupAddrv6[SIZE_OF_IPV6_ADDR];
	unsigned short *pnpdaddr = NULL;

	memset(eth_g_index, 0, sizeof(unsigned int) * MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT);
	/*struct igmp_snooping_list_s	*igmpList;*/
	
	/*check vlan exist.*/
	ret = npd_check_vlan_exist(vid);
	syslog_ax_igmp_dbg("npd check vlan exist :return value = %x\n",ret);
	if (VLAN_RETURN_CODE_VLAN_EXISTS != ret) {
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;/*can NOT directed Return NPD_VLAN_BADPARAM.*/
	}
	else {
		/*stamp igmp on vlan struct*/
		vlanNode = npd_find_vlan_by_vid(vid);
		vlanNode->igmpSnpEnDis = enDis;

		/*clear igmp on all-ports member of vlan when enDis is 0*/
		if(!enDis){
			ret = npd_vlan_member_port_index_get(vid, /*vlanId*/
												 0,   /*tagMode*/
												 eth_g_index,
												 &mbrCnt);
			if(NPD_TRUE == ret){
				for(i = 0;i<mbrCnt;i++){
					ethPort = npd_get_port_by_index(eth_g_index[i]);
					if (NULL == ethPort)	/* code optimize: Dereference null return value houxx@autelan.com  2013-1-18 */
					{
						syslog_ax_igmp_dbg("eth port not exist\n");
						return IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;
					}
					tmpAttrBitmap = ((ethPort->funcs.funcs_run_bitmap) >> ETH_PORT_FUNC_IGMP_SNP) & 0x0001;
					if(1 == tmpAttrBitmap)
					{
						ethPort->funcs.funcs_run_bitmap ^= 1<<ETH_PORT_FUNC_IGMP_SNP;
					}				
					/*clear vlan port igmp enDis flag*/
					npd_vlan_port_igmp_endis_flag_set(vid,eth_g_index[i],0,NPD_FALSE);

					/*set igmp trap disable on port(enabled igmp)*/
					npd_get_devport_by_global_index(eth_g_index[i],&devNum,&portNum);
					nam_asic_igmp_trap_set_by_devport(vid, devNum, portNum, 0);
				}
			}
			memset(eth_g_index,0,sizeof(eth_g_index));
			mbrCnt = 0;
			ret = npd_vlan_member_port_index_get(vid, /*vlanId*/
												 1,   /*tagMode*/
												 eth_g_index,
												 &mbrCnt);
			if(NPD_TRUE == ret){
				for(i = 0;i<mbrCnt;i++){
					if (NULL == ethPort)	/* code optimize: Dereference null return value houxx@autelan.com  2013-1-18 */
					{
						syslog_ax_igmp_dbg("eth port not exist\n");
						return IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;
					}

					tmpAttrBitmap = ((ethPort->funcs.funcs_run_bitmap) >> ETH_PORT_FUNC_IGMP_SNP) & 0x0001;
					if(1 == tmpAttrBitmap)
					{
						ethPort->funcs.funcs_run_bitmap ^= 1<<ETH_PORT_FUNC_IGMP_SNP;
					}
					
					/*clear vlan port igmp enDis flag*/
					npd_vlan_port_igmp_endis_flag_set(vid,eth_g_index[i],1,NPD_FALSE);

					/*set igmp trap disable on port(enabled igmp)*/
					npd_get_devport_by_global_index(eth_g_index[i],&devNum,&portNum);
					nam_asic_igmp_trap_set_by_devport(vid, devNum, portNum, 0);
				}
			}

			if (IGMPSNP_RETURN_CODE_OK == npd_igmp_snp_l2mc_vlan_exist(vid))
			{
				for (vidx = 0; vidx < NPD_IGMP_SNP_GROUP_NUM_MAX; vidx++)
				{
					if (IGMPSNP_RETURN_CODE_OK == npd_igmp_snp_l2mc_group_exist(vid, vidx))
					{
						if (IGMPSNP_RETURN_CODE_OK == npd_igmp_check_contain_fdb(vid, vidx))
						{
							pnpdaddr = groupAddrv6;
							ret = npd_igmp_get_groupip_by_vlan_vidx(vid, vidx, &groupAddr, &pnpdaddr, &npdret);
							if (IGMPSNP_RETURN_CODE_OK == ret)
							{
								ret = npd_igmp_fdb_entry_delfrom(vid, vidx);
								if (IGMPSNP_RETURN_CODE_OK == ret)
								{
									/*Here to delete FDB entry on HW*/
									syslog_ax_igmp_dbg("Here to delete FDB entry On HW,groupAddr 0x%x,groupAddrv6 %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x,vidx %d,vlan %d!\n",
														groupAddr, ADDRSHOWV6(groupAddrv6), vidx, vid);
									if(NPD_TRUE == npdret){
										nam_static_fdb_entry_indirect_delete_for_mld(groupAddr, vid);										
									}
									else{ 
										nam_static_fdb_entry_indirect_delete_for_igmp(groupAddr, vid);
									}
									ret = npd_l2mc_entry_delete(vid, vidx, groupAddr);
								}
							}
						}
					}
					else
					{
						continue;
					}
				}
			}		
		}
	}
	#if 0
	/*check vlan status*/
	if(VLAN_STATE_DOWN_E ==npd_check_vlan_status(vid)){
		return 1;
	}
	/*no other than there mcgroup build*/
	/*do some op on list*/
	#endif
	if(enDis){
		npdMngMsg.event_type = NPD_IGMPSNP_EVENT_DEV_REGISTER;
	}
	else{
		npdMngMsg.event_type = NPD_IGMPSNP_EVENT_DEV_UNREGISTER;
	}
	npdMngMsg.vid = vid;
	npdMngMsg.port_index = 0;

	/*stream socket*/
	/*ret = npd_msg_sendto_igmpsnp(&npdMngMsg);*/

	/*dgram socket*/
	ret = npd_cmd_sendto_igmpsnp(&npdMngMsg);
	syslog_ax_igmp_dbg("npd_cmd_sendto_igmpsnp::send() return value = %x\n",ret);
	if(IGMPSNP_RETURN_CODE_OK != ret ){
		syslog_ax_igmp_dbg("npd mng msg write fail.\n");
	}
	/*this kind of event,IGMP snooping has not any according notify message*/
	/*So,here immediately config to Hw.*/
	/*enable IGMP packet Trap to cpu by vid that device assigned to the packet*/
	if(1 == vid){
		/*default vlan set igmp trap by vlan (not on port)*/
		if(0 != nam_asic_igmp_trap_set_by_vid(devNum,vid,enDis)){
			return IGMPSNP_RETURN_CODE_ERROR_HW;
		}
	}

	/*enable/disable IPM bridging :Vlan entry Word0<IPv4 IPMbridgingEn> and <IPv6 IPMbridgingEn>*/

	if( nam_asic_vlan_ipv4_mc_enalbe(devNum,
									vid,
									enDis))										/* <*, G, V> Mode */
	{
		/* force set 1, set fdb match as <*, G, V>mode  IPv4 Mcast entry */
		return IGMPSNP_RETURN_CODE_ERROR_HW;
	}

	/*set asic Hw for vlan*/
	if(enDis){
		/*set Ipv4 Multicast mode: (*,G,V)*/
		if(nam_asic_vlan_ipv4_mcmode_set(devNum, vid)){
			return IGMPSNP_RETURN_CODE_ERROR_HW;
		}

		if(nam_asic_vlan_igmp_enable_drop_unmc(devNum,vid)){/*ipv4 to drop,ipv6 need to mirror to cpu*/
			return IGMPSNP_RETURN_CODE_ERROR_HW;
		}
	}
	else 
	{
		if(nam_asic_vlan_igmp_enable_forword_unmc(devNum,vid)){
			return IGMPSNP_RETURN_CODE_ERROR_HW;
		}
	}
#if 0
	/* 0 == enDis,delete fdb & vidx entries*/
	if(0==enDis){
		if(NPD_GROUP_SUCCESS==npd_igmp_snp_l2mc_vlan_exist(vid)){
			for(vidx=0; vidx<NPD_IGMP_SNP_GROUP_NUM_MAX;vidx++){
				if(NPD_GROUP_SUCCESS==npd_igmp_snp_l2mc_group_exist(vid, vidx)){
					ret = npd_l2mc_entry_delete(vid, vidx);
					if(BRG_MC_FAIL == ret){
						return ret ;
					}
					ret = npd_igmp_get_groupip_by_vlan_vidx(vid,vidx, &groupAddr);
					if(NPD_TRUE == ret){
						nam_static_fdb_entry_indirect_delete_for_igmp(groupAddr,vid);
					}
				}
				else
					continue;
			}
		}
	}
#endif
	return IGMPSNP_RETURN_CODE_OK;
}
/*******************************************************************************
 * npd_vlan_port_igmp_snp_endis_config
 *
 * DESCRIPTION:
 *   		enable IGMP Snooping on port of vlan Member
 *
 * INPUTS:
 * 		enable - enable or disable flag 
 *		vid - vlan id
 *		eth_g_idx - ether global index include device and port number
 *		tagMode - port is tag or untag
 *
 * OUTPUTS:
 *    	null	
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - config endis success
 *		IGMPSNP_RETURN_CODE_ERROR - get devMum and virPortNum error
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - can not find the vlan which include the port
 *		IGMPSNP_RETURN_CODE_NOT_SUPPORT_IGMP_SNP - not support igmp_snooping
 *		IGMPSNP_RETURN_CODE_PORT_NOT_EXIST - port not exist
 *		IGMPSNP_RETURN_CODE_ERROR_HW - config hardware error
 *
 * COMMENTS:
 *      
 **
 ********************************************************************************/
int npd_vlan_port_igmp_snp_endis_config
(
	unsigned char enable,
	unsigned short vid,
	unsigned int eth_g_idx,
	unsigned char tagMode
)
{
	unsigned char devNum = 0,virPortNum = 0;
	unsigned int eth_g_index = eth_g_idx;
	unsigned int ret = IGMPSNP_RETURN_CODE_OK;
	unsigned long enDis = NPD_FALSE;
	struct vlan_s *vlanNode = NULL;
	
	struct eth_port_s *portNode = NULL;
	struct npd_dev_event_cmd npdMngMsg;


	if(NPD_SUCCESS != (ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNum))){
		return IGMPSNP_RETURN_CODE_ERROR;
	}
	syslog_ax_igmp_dbg("%s igmp-snooping on vlan %d dev %d port %d.\n",
						enable ? "enable" : "disable", vid, devNum, virPortNum);
	/*check vlan exist*/
	ret = npd_check_vlan_exist(vid);
	if (VLAN_RETURN_CODE_VLAN_EXISTS != ret) {
		syslog_ax_igmp_dbg("NOT find vlan %d :return value = %d\r\n",vid,ret);
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;/*can NOT directed Return NPD_VLAN_BADPARAM.*/
	}
	else {
		vlanNode = npd_find_vlan_by_vid(vid);
		syslog_ax_igmp_dbg("check vlan already set %d %ssupport IGMP\n",
							vid,vlanNode->igmpSnpEnDis ? "" : "not ");
		/*npd_check_igmp_snp_vlan_status*//*because in npd_vlan_igmp_snp_endis_config() the vlanNode->igmpSnpEnDis had been set.-yangxs*/
		if((0 == vlanNode->igmpSnpEnDis)&& (NPD_TRUE == enable)){
			return IGMPSNP_RETURN_CODE_NOT_SUPPORT_IGMP_SNP;
		}
	}
	/*syslog_ax_igmp_dbg("find exsiting vlan %d support IGMP.",vid);*/

	enDis = enable ; 
	npd_vlan_port_igmp_endis_flag_set(vid,eth_g_idx,tagMode,enDis);
	/*config IGMP Snooping Protocal*/
	if(enDis){
		npdMngMsg.event_type = NPD_IGMPSNP_EVENT_DEV_VLANADDIF;
	}
	else{
		npdMngMsg.event_type = NPD_IGMPSNP_EVENT_DEV_VLANDELIF;
	}
	npdMngMsg.vid = vid;
	npdMngMsg.port_index = eth_g_index;
	syslog_ax_igmp_dbg("event type = %s,vid = %d,port_index = %d\n",	\
						(npdMngMsg.event_type==5) ? "IgmpVlanAddport" : "IgmpVlanDelport",	
						npdMngMsg.vid,	\
						npdMngMsg.port_index);
	/*stream socket*/
	/*ret = npd_msg_sendto_igmpsnp(&npdMngMsg);*/

	/*dgram sock*/
	ret = npd_cmd_sendto_igmpsnp(&npdMngMsg);
	if(IGMPSNP_RETURN_CODE_OK != ret ){
		syslog_ax_igmp_dbg("npd mng msg write fail.\n");
	}
	
	/*set port igmp snoop enable Flag*/
	portNode = npd_get_port_by_index(eth_g_index);
	if(NULL == portNode ) 
	{
		/*never happen, cause port exist or not has been checked before call this func. */
		return IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;
	}
	/*npd_igmp_snp_eth_port_status(eth_g_index,enDis);*/
	if(enDis){
		syslog_ax_igmp_dbg("Before >>port %d funcs run bitmap 0x%x.\n",eth_g_index,portNode->funcs.funcs_run_bitmap);
		portNode->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_IGMP_SNP);
		syslog_ax_igmp_dbg("After >>port %d funcs run bitmap 0x%x.\n",eth_g_index,portNode->funcs.funcs_run_bitmap);
	}
	else{
		portNode->funcs.funcs_run_bitmap ^= 1<<ETH_PORT_FUNC_IGMP_SNP;
	}
	
	/* here config Hw : enable/Disable trap*/
	if(0 != nam_asic_igmp_trap_set_by_devport(vid, devNum,virPortNum,enDis)){
		return IGMPSNP_RETURN_CODE_ERROR_HW;
	}
	
	/*0 == enDis;delete FDB entry, and VIDX multicast entry*/
	/**NOT TODO;cause it'll be excuted by Dev Event::IGMP_ADDR_DEL ****/
	return IGMPSNP_RETURN_CODE_OK;
	
}
/*******************************************************************************
 * npd_mcrouter_port_igmp_snp_endis_config
 *
 * DESCRIPTION:
 *		config mcrouter port for a vlan 
 *
 * INPUTS:
 *		enable - enalbe or disable flag
 *		vid -vlan id 
 *		eth_g_idx - ether global index include device and port number
 *
 * OUTPUTS:
 *    	null
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - config endis success
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - can not find the vlan which include the port
 *		IGMPSNP_RETURN_CODE_PORT_NOT_EXIST - port not exist
 *		IGMPSNP_RETURN_CODE_NOTENABLE_PORT - the port is not enable igmp snooping
 *		IGMPSNP_RETURN_CODE_ROUTE_PORT_EXIST - the route port already exist
 *		IGMPSNP_RETURN_CODE_ROUTE_PORT_NOTEXIST - the route port not exist
 *		IGMPSNP_RETURN_CODE_NOTROUTE_PORT - set port  flag fail in vlan
 *		
 * COMMENTS:
 *     
 **
 ********************************************************************************/
int npd_mcrouter_port_igmp_snp_endis_config
(
	unsigned char enable,
	unsigned short vid,
	unsigned int eth_g_idx
)
{
	unsigned char devNum = 0,virPortNum = 0,tagMode = 0;
	unsigned int eth_g_index = eth_g_idx;
	unsigned int ret = IGMPSNP_RETURN_CODE_OK;
	unsigned char enDis = enable,status = 0;
	struct npd_dev_event_cmd npdMngMsg;


	if(0 != (ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNum))){
		return 1;
	}
	syslog_ax_igmp_dbg("config dev %d,vlan %d,port %d,enDis %d.",devNum,vid,virPortNum,enable);
	/*check vlan exist*/
	ret = npd_check_vlan_exist(vid);
	if (VLAN_RETURN_CODE_VLAN_EXISTS != ret) {
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;/*can NOT directed Return NPD_VLAN_BADPARAM.*/
	}

	syslog_ax_igmp_dbg("find vlan %d exsit.",vid);

	/*check port membership */
	if(NPD_TRUE != npd_vlan_check_contain_port(vid,eth_g_index, &tagMode)){
		return IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;	
	}

	#if 0
	/*check port enabled IGMP or NOT*/
	ret = npd_check_igmp_snp_port_status(eth_g_index,&status);
	if(0 == status ){
		 syslog_ax_vlan_err(" port %d not enabled IGMP Snooping.",eth_g_index);
		return NPD_IGMP_SNP_NOTENABLE_PORT;
	}
	#endif
	ret = npd_check_igmp_snp_vlanMbr_status(vid,eth_g_index,&status);
	if((IGMPSNP_RETURN_CODE_OK == ret) && (NPD_FALSE == status)){
		return IGMPSNP_RETURN_CODE_NOTENABLE_PORT;
	}
	/*existed but to config */
	ret = npd_vlan_check_contain_igmp_routeport(vid, eth_g_index);
	if (IGMPSNP_RETURN_CODE_OK == ret) {/* exist the router port */
		/* this port is routerport, if enDis is enabel, no need to add*/
		if (NPD_TRUE == enDis) {			
			return IGMPSNP_RETURN_CODE_ROUTE_PORT_EXIST;
		}
		/* this port is routerport, and enDis is disable, need to delete */
		else if (NPD_FALSE == enDis) {
			syslog_ax_igmp_dbg("find the router-port %d exsit in vlan %d. Next step delete\n", eth_g_idx, vid);
		}
	}
	else if (IGMPSNP_RETURN_CODE_ERROR == ret) { /* not exist the router port */
		/* the vlan haven't the router-port ,if enDis is enable, need to add */
		if (NPD_TRUE == enDis) {
			syslog_ax_igmp_dbg("not find the router-port %d exsit in the vlan %d. Next step add\n", eth_g_idx, vid);
		}
		/* the vlan haven't the router-port, if enDis is disable, no need to delete */		
		else if (NPD_FALSE == enDis ) {
			return IGMPSNP_RETURN_CODE_ROUTE_PORT_NOTEXIST;
		}
	}
	
	/* add/delete port mcrouter_port Flag*/
	ret = npd_vlan_port_igmp_mcrouter_flag_set(vid,eth_g_index,tagMode,enDis);
	if( IGMPSNP_RETURN_CODE_OK != ret){
		syslog_ax_igmp_dbg("%s port %d vlan %d flag %d fail. ",	\
			(1 == enDis) ? "add":"del",eth_g_index,vid,enDis);
		return IGMPSNP_RETURN_CODE_NOTROUTE_PORT;
	}
	
	/* here config Hw */
	enDis = enable ; 
	if(enDis){
		npdMngMsg.event_type = NPD_IGMPSNP_EVENT_DEV_MCROUTER_PORT_UP;
	}
	else{
		npdMngMsg.event_type = NPD_IGMPSNP_EVENT_DEV_MCROUTER_PORT_DOWN;
	}
	npdMngMsg.vid = vid;
	npdMngMsg.port_index = eth_g_index;
	syslog_ax_igmp_dbg("send msg to igmp snooping %s router port %d to vlan %d\n",\
				( NPD_IGMPSNP_EVENT_DEV_MCROUTER_PORT_UP == npdMngMsg.event_type ) ? "add":"delete",\
				npdMngMsg.port_index,npdMngMsg.vid);

	/*dgram sock*/
	ret = npd_cmd_sendto_igmpsnp(&npdMngMsg);
	if(IGMPSNP_RETURN_CODE_OK != ret ){
		syslog_ax_igmp_dbg("npd mng msg write fail.");
	}
	return IGMPSNP_RETURN_CODE_OK;
	
}

#ifdef __cplusplus
}
#endif
