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
* npd_rstp_common.c
*
*
*CREATOR:
*	zhubo@autelan.com
*
*DESCRIPTION:
*	APIs used in NPD for rstp process.
*
*DATE:
*	03/01/2008
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.54 $		
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
/*user part */
#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd/nam/npd_amapi.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd_product.h"

#include "npd_c_slot.h"
#include "npd_eth_port.h"
#include "npd_log.h"
#include "npd_vlan.h"
#include "npd_rstp_common.h"

/*include header files begin */

/*MACRO definition begin */
/*MACRO definition end */

/*local variables definition begin */
struct	sockaddr_un 	npd_addr;       /*local addr*/	
struct	sockaddr_un		rstp_addr;   	/*remote addr*/
int		ser_fd;
/*local variables definition end */

/*external variables reference begin */
/*external variables reference end */

/*external function declarations begin*/
/*external function declarations begin*/


/*******************************************************************************
 * npd_rstp_sock_init
 *
 * DESCRIPTION:
 *   	init socket between npd and stp socket
 *
 * INPUTS:
 * 	NONE
 *
 * OUTPUTS:
 *    	
 *
 * RETURNS:
 * 	TRUE   	- on success
 * 	FALSE	- otherwise
 *
 *
 ********************************************************************************/
int	npd_rstp_sock_init()
{
	memset(&rstp_addr,0,sizeof(rstp_addr));
	memset(&npd_addr,0,sizeof(npd_addr));

	/* tell my thread id*/
	npd_init_tell_whoami((unsigned char *)"RstpSock",0);
	
	if((ser_fd = socket(AF_LOCAL,SOCK_DGRAM,0)) == -1)
	{
		syslog_ax_rstp_err("create npd to rstp socket fail\n");
		return -1;
	}
	rstp_addr.sun_family = AF_LOCAL;
	strcpy(rstp_addr.sun_path, "/tmp/NpdCmd_CLIENT");
	
	npd_addr.sun_family = AF_LOCAL;
	strcpy(npd_addr.sun_path,"/tmp/NpdCmd_SERVER");
				

    unlink(npd_addr.sun_path);

	if(bind(ser_fd, (struct sockaddr *)&npd_addr, sizeof(npd_addr)) == -1) 
	{
		syslog_ax_rstp_err("npd to rstp socket created but failed when bind\n");
		return -1;
	}

	/* code optimize: Unchecked return value. zhangdi@autelan.com 2013-01-18 */
	if(chmod(npd_addr.sun_path, 0777) == -1)
	{
		syslog_ax_rstp_err("npd chmod(npd_addr.sun_path, 0777) failed.\n");
		return -1;		
	}

	npd_read_stp_ports_state();
			
	return 0;	
	
}

/********************/
/********************/
 int npd_cmd_write_ro_rstp
(
	NPD_CMD_STC		*npdCmdPtr,
	unsigned int	cmdLen
)
{
	int	rc,byteSend = 0;

	while(cmdLen != byteSend)
	{
		rc = sendto(ser_fd,npdCmdPtr,cmdLen,MSG_DONTWAIT,
							(struct sockaddr *)&rstp_addr, sizeof(rstp_addr));
		if(rc < 0)
		{
			if(errno == EINTR)
			{
				npd_syslog_err("sendto fail \n");
				continue; 
			}
			else
			{
				 syslog_ax_rstp_err("npd send command to rstp write fail\n");
				return -1;
			}
		}
		byteSend += rc;
	}
	/*printf("sendto :fd %d\n",ser_fd);		*/					
	/*printf("sendto :byteSend %d,originCmdbytes %d\n",byteSend,cmdLen);*/

	return byteSend;
	
}

int npd_mstp_enable_port(unsigned int port_index)
{
		NPD_CMD_STC	 npdCmdPtr ={
		.type = PORT_ENABLE_E,
		.length  = sizeof(NPD_RSTP_INTF_OP_PARAM_T),
		.value.cmdIntf.vid = 0,
		.value.cmdIntf.portIdx = port_index
	};
	
	if(( npd_cmd_write_ro_rstp(&npdCmdPtr,sizeof(NPD_CMD_STC)))!=-1){
		syslog_ax_rstp_dbg("send success\n");
		return 0;
	}
	else{
		syslog_ax_rstp_err("enable ERROR\n");
		return -1;
	}
		
}

int npd_mstp_disable_port(unsigned int port_index)
{
		NPD_CMD_STC	 npdCmdPtr ={
		.type = PORT_DISABLE_E,
		.length  = sizeof(NPD_RSTP_INTF_OP_PARAM_T),
		.value.cmdIntf.vid = 0,
		.value.cmdIntf.portIdx = port_index
	};
	
	if(( npd_cmd_write_ro_rstp(&npdCmdPtr,sizeof(NPD_CMD_STC)))!=-1){
		syslog_ax_rstp_dbg("send success\n");
		return 0;
	}
	else{
		syslog_ax_rstp_err("disable ERROR\n");
		return -1;
	}
		
}

int npd_mstp_add_port(unsigned short vid,unsigned int index,unsigned int iswan)
{
	/*printf("npd_rstp_common206 :: add port[%d] in vlan[%d]\n",index,vid);*/
	NPD_CMD_STC	 npdCmdPtr ={
		.type = INTERFACE_ADD_E,
		.length  = sizeof(NPD_RSTP_INTF_OP_PARAM_T),
		.value.cmdIntf.vid = vid,
		.value.cmdIntf.portIdx = index,
		.value.cmdIntf.isWan = iswan
	};
	
	if(( npd_cmd_write_ro_rstp(&npdCmdPtr,sizeof(NPD_CMD_STC)))!=-1){
		syslog_ax_rstp_dbg("send success\n");
		return 0;
	}
	else{
		syslog_ax_rstp_err("add ERROR\n");
		return -1;
	}
		
}

int npd_mstp_del_port(unsigned short vid,unsigned int index)
{
	/*printf("npd_rstp_common227 :: del port[%d] in vlan[%D]\n",index,vid);*/
	NPD_CMD_STC	 npdCmdPtr ={
		.type = INTERFACE_DEL_E,
		.length  = sizeof(NPD_RSTP_INTF_OP_PARAM_T),
		.value.cmdIntf.vid = vid,
		.value.cmdIntf.portIdx = index
	};
	
	if(( npd_cmd_write_ro_rstp(&npdCmdPtr,sizeof(NPD_CMD_STC)))!=-1){
		syslog_ax_rstp_dbg("send success\n");
		return 0;
	}
	else{
		syslog_ax_rstp_err("del ERROR\n");
		return -1;
	}
		
}

int npd_mstp_add_vlan_on_mst(unsigned short vid,unsigned int untagbmp[],unsigned int tagbmp[])
{
	NPD_CMD_STC	 npdCmdPtr ={
		.type = VLAN_ADD_ON_MST_E,
		.length  = sizeof(NPD_MSTP_VLAN_OP_PARAM_T),
		.value.cmdVlan.vid = vid
	};
	npdCmdPtr.value.cmdVlan.untagbmp[0] = untagbmp[0];
	npdCmdPtr.value.cmdVlan.untagbmp[1] = untagbmp[1];
	npdCmdPtr.value.cmdVlan.tagbmp[0] = tagbmp[0];
	npdCmdPtr.value.cmdVlan.tagbmp[1] = tagbmp[1];

	if(( npd_cmd_write_ro_rstp(&npdCmdPtr,sizeof(NPD_CMD_STC)))!=-1){
		syslog_ax_rstp_dbg("send success\n");
		return 0;
	}
	else{
		syslog_ax_rstp_err("add vlan ERROR\n");
		return -1;
	}
}

int npd_mstp_del_vlan_on_mst(unsigned short vid)
{
	NPD_CMD_STC	 npdCmdPtr ={
		.type = VLAN_DEL_ON_MST_E,
		.length  = sizeof(NPD_MSTP_VLAN_OP_PARAM_T),
		.value.cmdVlan.vid = vid
	};
	
	npdCmdPtr.value.cmdVlan.untagbmp[0] = 0;
	npdCmdPtr.value.cmdVlan.untagbmp[1] =0;
	npdCmdPtr.value.cmdVlan.tagbmp[0] = 0;
	npdCmdPtr.value.cmdVlan.tagbmp[1] = 0;

	if(( npd_cmd_write_ro_rstp(&npdCmdPtr,sizeof(NPD_CMD_STC)))!=-1){
		syslog_ax_rstp_dbg("send success\n");
		return 0;
	}
	else{
		syslog_ax_rstp_err("del vlan ERROR\n");
		return -1;
	}
	
}
/***************************
*Function used to get port stp state
*Input:
*	vid - vlan id
* 	port_index - eth-port index
*Output:
*	state - port rstp state
*
***************************/
int npd_mstp_get_port_state
(
    unsigned short vid,
    unsigned int port_index,
    NAM_RSTP_PORT_STATE_E*  state 
)
{
	struct stp_info_s* stport = NULL;
    struct eth_port_s 	*ethPort = NULL;
	unsigned char devNum = 0,portNum = 0;
	unsigned short stpId = 0,mstid = 0;
	unsigned int stpstate = 0;
	int ret = 0;

	/* for interface vlan on AX81-SMU, zhangdi@autelan.com 2011-07-21 */	
    if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
    {
		/* jump 'ethPort = npd_get_port_by_index(port_index)' */
    	ret =  npd_get_devport_by_global_index(port_index,&devNum,&portNum);
    	if(0 == ret) {
    		ret = nam_vlan_stpid_get(devNum,vid,&stpId);/*when bind,mstid == stpId*/
    		if(0 == ret){
    			ret = nam_stp_state_get(devNum,portNum,stpId,&stpstate);
    			if(0 == ret) {
                    *state = stpstate;
                    return NPD_SUCCESS;
    			}
    			else {
                   return NPD_FAIL;
    			}	
    		}
    		else {
              return NPD_FAIL;
    		}	
    	}
		return NPD_FAIL;	
    }
	
	ethPort = npd_get_port_by_index(port_index);
	if(NULL == ethPort) {
		npd_syslog_dbg("get eth-port %#x stp state but port null\n",port_index);
		return NPD_FAIL;
	}
	if(ethPort->funcs.funcs_run_bitmap & (1 << ETH_PORT_FUNC_DOT1W)){
		stport = (struct stp_info_s *)(ethPort->funcs.func_data[ETH_PORT_FUNC_DOT1W]);
		if(NULL == stport) {/*the port is not enable in stp */
			*state = NAM_STP_PORT_STATE_DISABLE_E;
           	return NPD_SUCCESS;
		}
	}
	else{/*dot1w not in run bitmap,do not need check port stp state*/
		*state = NAM_STP_PORT_STATE_DISABLE_E;
        return NPD_SUCCESS;
	}
	ret =  npd_get_devport_by_global_index(port_index,&devNum,&portNum);
	if(0 == ret) {
		ret = nam_vlan_stpid_get(devNum,vid,&stpId);/*when bind,mstid == stpId*/
		if(0 == ret){
			ret = nam_stp_state_get(devNum,portNum,stpId,&stpstate);
			if(0 == ret) {
                *state = stpstate;
                return NPD_SUCCESS;
			}
			else {
               return NPD_FAIL;
			}	
		}
		else {
          return NPD_FAIL;
		}	
	}
	return NPD_FAIL;
}
#if 0
int npd_rstp_add_port(unsigned int index)
{
	
	NPD_CMD_STC	 npdCmdPtr ={
		.type = INTERFACE_ADD_E,
		.length  = sizeof(NPD_RSTP_INTF_OP_PARAM_T),
		.value.cmdIntf.portIdx = index
	};
	
	if(( npd_cmd_write_ro_rstp(&npdCmdPtr,sizeof(NPD_CMD_STC)))!=-1){
		 syslog_ax_rstp_dbg("send success\n");
		return 0;
	}
	else{
		 syslog_ax_rstp_err("add ERROR\n");
		return -1;
	}
		
}

int npd_rstp_del_port(unsigned int index)
{
	NPD_CMD_STC	 npdCmdPtr ={
		.type = INTERFACE_DEL_E,
		.length  = sizeof(NPD_RSTP_INTF_OP_PARAM_T),
		.value.cmdIntf.portIdx = index
	};
	
	if(( npd_cmd_write_ro_rstp(&npdCmdPtr,sizeof(NPD_CMD_STC)))!=-1){
		 syslog_ax_rstp_dbg("send success\n");
		return 0;
	}
	else{
		 syslog_ax_rstp_err("del ERROR\n");
		return -1;
	}
		
}
#endif

/*******************************************************************************
 * npd_rstp_link_change
 *
 * DESCRIPTION:
 *   	when link up or down ,send info to stp
 *
 * INPUTS:
 * 	NONE
 *
 * OUTPUTS:
 *    	
 *
 * RETURNS:
 * 	TRUE   	- on success
 * 	FALSE	- otherwise
 *
 *
 ********************************************************************************/
int npd_rstp_link_change
(
	unsigned int eth_g_index,
	struct stp_info_s	*stpInfo,/*void	*	paramPtr,*/
	enum ETH_PORT_NOTIFIER_ENT	event
)
{
	unsigned char devNum = 0,portNum = 0;
	unsigned char stp_enable = 0;
	unsigned int duplex_mode= 0, attr_map = 0;
	int ret = 0;
	unsigned int speed = 0, isWAN = 0;
	/*get the link speed
    npd_get_port_speed(eth_g_index,&speed)*/;
	/*get the duplex mode
	npd_get_port_duplex_mode(eth_g_index,&duplex_mode)*/;

	ret = npd_get_port_attrmap_by_gindex(eth_g_index, &isWAN, &attr_map);
	if (!ret) {
		speed = (attr_map & ETH_ATTR_SPEED_MASK);
		duplex_mode = (attr_map & ETH_ATTR_DUPLEX);
	}

	
	NPD_CMD_STC		npdCmd = {
		
		.type = LINK_CHANGE_EVENT_E,
		.length = sizeof(NPD_RSTP_INTF_OP_PARAM_T),
		.value.cmdLink.portIdx = eth_g_index,
		.value.cmdLink.event = event,
		.value.cmdLink.speed = speed,
		.value.cmdLink.duplex_mode = duplex_mode
	};
    
	/*stpInfo = (struct stp_info_s*)paramPtr;*/
	if(NULL != stpInfo && stpInfo->stpEnable){

		 syslog_ax_rstp_dbg("npd_rstp_link_change :: link state %d, port %d\n",event,npdCmd.value.cmdLink.portIdx);
		npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);

		if(ETH_PORT_NOTIFIER_LINKUP_E == event)
			stp_enable = 1;
		else
			stp_enable = 0;

		/*config asic*/
		 syslog_ax_rstp_dbg("npd_rstp_link_change :: link state %d, port %d config asic\n",event,eth_g_index);
		ret = nam_asic_stp_port_enable(devNum, portNum,stp_enable);
		if(0 != ret){
			 syslog_ax_rstp_err("npd_rstp_link_change:: nam_asic_stp_port_enable ERROR\n");
			return ret;
		}

		/*get the port_speed*/
	    /* npd_get_port_speed(eth_g_index,&speed);*/
       /*  npdCmd.value.cmdLink.speed = speed;*/
       /*npdCmd.value.cmdLink.speed = stpInfo->pathcost[mstid];*/
      
		/*config protocol*/
		 syslog_ax_rstp_dbg("npd_rstp_link_change :: link state %d, port %d ,speed %d,duplex_mode %d config protocol\n",event,eth_g_index,speed, duplex_mode);
		if(( npd_cmd_write_ro_rstp(&npdCmd,sizeof(NPD_CMD_STC)))!=-1){
			 syslog_ax_rstp_dbg("send Success\n");
			return NPD_SUCCESS;
		}
		else{
			 syslog_ax_rstp_err("del ERROR\n");
			return NPD_FAIL;
		}

	}
	 
	return 0;

}

/*******************************************************************************
 * npd_read_stp_ports_state
 *
 * DESCRIPTION:
 *   	receive info from stp
 *
 * INPUTS:
 * 	NONE
 *
 * OUTPUTS:
 *    	
 *
 * RETURNS:
 * 	TRUE   	- on success
 * 	FALSE	- otherwise
 *
 *
 ********************************************************************************/
int npd_rstp_recv_info
(
	NPD_CMD_STC	*stpInfoPtr,
	unsigned int  infoLen
)
{
	int len = 0; 
	unsigned int addrLen = sizeof(rstp_addr);
	while(1)
	{
		len = recvfrom(ser_fd, stpInfoPtr, infoLen, 0,(struct sockaddr *)&rstp_addr, &addrLen);
		if(len < 0 && errno == EINTR) 
		{
			continue;
		}
		break;
	}

	return len;
}

/*******************************************************************************
 * npd_read_stp_ports_state
 *
 * DESCRIPTION:
 *   	receive info from stp
 *
 * INPUTS:
 * 	NONE
 *
 * OUTPUTS:
 *    	
 *
 * RETURNS:
 * 	TRUE   	- on success
 * 	FALSE	- otherwise
 *
 *
 ********************************************************************************/
int npd_read_stp_ports_state()
{
 	fd_set stpfds; 
	int numfds,rc = 0;
	
	while(1) {
		numfds = -1;
    	FD_ZERO(&stpfds);

		FD_SET(ser_fd,&stpfds);


		numfds = ser_fd + 1;
    	/*printf ("wait %ld-%ld\n", (long) tv.tv_sec, (long) tv.tv_usec);*/
    	rc = select(numfds, &stpfds, NULL, NULL, NULL);
    	if(rc < 0)
		{             
			/* Error*/
      		if (EINTR == errno) continue; /* don't break */
      		 syslog_ax_rstp_err("FATAL_MODE:select failed: %s\n", strerror(errno));
      		return -2;
    	}

    	if(FD_ISSET(ser_fd, &stpfds))
		{
      		npd_read_socket();
    	}

	}

	return NPD_SUCCESS;	
}

int npd_read_socket()
{
	NPD_CMD_STC	stpInfo;
	unsigned int  infoLen = sizeof(NPD_CMD_STC);
	unsigned char devNum = 0,portNum = 0;
	unsigned short stpId = 0;
	int ret = 0,i = 0;
	NAM_RSTP_PORT_STATE_E  state = 0;
	
	memset(&stpInfo,0,sizeof(NPD_CMD_STC));

	if(npd_rstp_recv_info(&stpInfo,infoLen)>0) {
		if(STP_GET_SYS_MAC_E == stpInfo.type){
			syslog_ax_rstp_dbg("The npd receive mac request from rstp and call send func\n");
	        npd_system_get_basemac(stpInfo.value.cmdFdb.MacDa,6);
			
		    syslog_ax_rstp_dbg("The mac is %02x:%02x:%02x:%02x:%02x:%02x\n",
								stpInfo.value.cmdFdb.MacDa[0],
								stpInfo.value.cmdFdb.MacDa[1],
								stpInfo.value.cmdFdb.MacDa[2],
								stpInfo.value.cmdFdb.MacDa[3],
								stpInfo.value.cmdFdb.MacDa[4],
								stpInfo.value.cmdFdb.MacDa[5]);

		    if(npd_cmd_write_ro_rstp(&stpInfo,sizeof(NPD_CMD_STC))!= -1){
	            syslog_ax_rstp_dbg("npd mac write to rstp successful\n");
		    }
		    else{
	             return NPD_FAIL;
		    }
		} 
		else if(STP_STATE_UPDATE_E == stpInfo.type) {
			ret =  npd_get_devport_by_global_index(stpInfo.value.cmdStp.portIdx,&devNum,&portNum);
            if(NPD_SUCCESS != ret){
               return NPD_FAIL;
			}
			stpId = stpInfo.value.cmdStp.mstid;
			switch(stpInfo.value.cmdStp.portState) {
				case NAM_STP_PORT_STATE_DISCARD_E:
					/*set port state*/
					/*nam_asic_stp_port_enable(devNum,portNum,0);*/
					ret = nam_stp_state_set(devNum,portNum,stpId,NAM_STP_PORT_STATE_DISCARD_E);
					if( 0 != ret){
                       syslog_ax_rstp_dbg("STP_STATE_UPDATE_E:: Set the stp port state error!\n");
					}
					/*delete the fdb entries*/
					ret = nam_fdb_table_delete_entry_with_port( stpInfo.value.cmdStp.portIdx);
					if(0 != ret){
						syslog_ax_rstp_dbg("STP_STATE_UPDATE_E::FDB entry delete error!\n");
					}
					/*delete arp entries*/
					npd_arp_clear_by_port(stpInfo.value.cmdStp.portIdx);
					syslog_ax_rstp_dbg("STP_STATE_UPDATE_E:: DISCARDING,write port %d block state ret[%d],stpId[%d]\n",stpInfo.value.cmdStp.portIdx,ret,stpId);
					break;
				case NAM_STP_PORT_STATE_LEARN_E:
					ret = nam_stp_state_set(devNum,portNum,stpId,NAM_STP_PORT_STATE_LEARN_E);
					/*nam_fdb_enable_port_auto_learn_set(stpInfo.value.cmdStp.portIdx,1);*/
					syslog_ax_rstp_dbg("STP_STATE_UPDATE_E:: LEARNING,return port %d ret [%d],stpId[%d]\n",stpInfo.value.cmdStp.portIdx,ret,stpId);
					break;
				case NAM_STP_PORT_STATE_FORWARD_E:
					ret = nam_stp_state_set(devNum,portNum,stpId,NAM_STP_PORT_STATE_FORWARD_E);
					/*nam_fdb_enable_port_auto_learn_set(stpInfo.value.cmdStp.portIdx,1);*/
					syslog_ax_rstp_dbg("STP_STATE_UPDATE_E:: FORWARDING,return port %d ret [%d],stpId[%d]\n",stpInfo.value.cmdStp.portIdx,ret,stpId);
					break;
				case NAM_STP_PORT_STATE_DISABLE_E:
					ret = nam_stp_state_set(devNum,portNum,stpId,NAM_STP_PORT_STATE_DISABLE_E);
					/*nam_fdb_enable_port_auto_learn_set(stpInfo.value.cmdStp.portIdx,1);*/
					syslog_ax_rstp_dbg("STP_STATE_UPDATE_E:: DISABLE,return port %d ret [%d],stpId[%d]\n",stpInfo.value.cmdStp.portIdx,ret,stpId);
					break;
				default:
					break;
			}
		}
		else if(STP_RECEIVE_TCN_E == stpInfo.type){
            stpId = stpInfo.value.cmdStp.mstid;
			npd_syslog_dbg("npd_rstp_common::STP_RECEIVE_TCN_E mstid %d,port %d\n",stpInfo.value.cmdStp.mstid,stpInfo.value.cmdStp.portIdx);
			ret = nam_fdb_table_delete_entry_with_port( stpInfo.value.cmdStp.portIdx);
			if(0 != ret){
				syslog_ax_rstp_err("npd_read_socket::FDB entry delete error!\n");
			}
			/*delete arp entries*/
			npd_arp_clear_by_port(stpInfo.value.cmdStp.portIdx);
		}
	}
	return 0;
}


/*******************************************************************************
 * npd_stp_vlan_stpid_bind_init
 *
 * DESCRIPTION:
 *   	init vlan's stpid to 0
 *
 * INPUTS:
 * 	NONE
 *
 * OUTPUTS:
 *    	
 *
 * RETURNS:
 * 	TRUE   	- on success
 * 	FALSE	- otherwise
 *
 *
 ********************************************************************************/
int npd_stp_vlan_stpid_bind_init(void)
{
	unsigned char devNum = 0;
	unsigned short i = 0;
	unsigned int stpId = 0,ret = 0;
	
	for(i = 0; i < 4096; i++) {
		ret =  npd_check_vlan_exist(i);
		if(VLAN_RETURN_CODE_VLAN_EXISTS == ret ) {

			ret = nam_stp_vlan_bind_stg(devNum, i, stpId);
			if(ret != 0) {
				 syslog_ax_rstp_dbg("npd_common_rstp379>>npd_stp_vlan_bind_to_stpid::Brg Vlan Bind to STP Error.");
				continue;
			}
		}
	}

	return ret;
}

/*******************************************************************************
 * npd_mstp_vlan_stpid_bind
 *
 * DESCRIPTION:
 *   	vlan's stpid is mstid,while mstp running
 *
 * INPUTS:
 * 	NONE
 *
 * OUTPUTS:
 *    	
 *
 * RETURNS:
 * 	TRUE   	- on success
 * 	FALSE	- otherwise
 *
 *
 ********************************************************************************/
int npd_mstp_vlan_stpid_bind
(
	unsigned short vid,
	unsigned int 	mstid	
)
{
	int ret = NPD_DBUS_SUCCESS;
	unsigned char devNum = 0;

	ret =  npd_check_vlan_exist(vid);
	if(VLAN_RETURN_CODE_VLAN_EXISTS != ret ) {
		syslog_ax_rstp_err("the vlan %d not exsit\n");
		ret = NPD_DBUS_ERROR;
		return ret;
	}
   
	else if(0 != nam_stp_vlan_bind_stg(devNum, vid, mstid)) {
		syslog_ax_rstp_err("npd_mstp_vlan_bind_to_stpid::Brg Vlan%d Bind to STPid %d Error\n",vid,mstid);
		ret = NPD_DBUS_ERROR;
		return ret;
	}

	return NPD_DBUS_SUCCESS;
}

/*******************************************************************************
 * npd_dbus_stp_get_broad_id
 *
 * DESCRIPTION:
 *   	if stp cli need productId to opreate,this functiong will be used
 *
 * INPUTS:
 * 	NONE
 *
 * OUTPUTS:
 *    	
 *
 * RETURNS:
 * 	TRUE   	- on success
 * 	FALSE	- otherwise
 *
 *
 ********************************************************************************/

DBusMessage * npd_dbus_stp_get_broad_id(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	enum product_id_e productId = PRODUCT_ID;
	syslog_ax_rstp_dbg("get productId %d\n",productId);
	
	int ret = 0;

	dbus_error_init(&err);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);
	return reply;
	
}

/*******************************************************************************
 * npd_dbus_get_broad_stp_function
 *
 * DESCRIPTION:
 *   	stp support or not
 *
 * INPUTS:
 * 	NONE
 *
 * OUTPUTS:
 *    	
 *
 * RETURNS:
 * 	TRUE   	- on success
 * 	FALSE	- otherwise
 *
 *
 ********************************************************************************/

DBusMessage * npd_dbus_get_broad_stp_function(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char IsSupport = 0;
	unsigned ret = NPD_FAIL;

	enum product_id_e productId = PRODUCT_ID;

	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){/*5612e and 5608 is not support*/
		/*npd_syslog_dbg("do not support stp!\n");
		IsSupport = 1;
	}*/
	/*if(!((productinfo.capbmp) & FUNCTION_STP_VLAUE)){
		npd_syslog_dbg("do not support stp!\n");
		IsSupport = 1;
	}
	else if(((productinfo.capbmp) & FUNCTION_STP_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_STP_VLAUE)){
		npd_syslog_dbg("do not support stp!\n");
		IsSupport = 1;
	}*/
	ret = NPD_SUCCESS;
	
	dbus_error_init(&err);

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &IsSupport);
	return reply;
	
}

DBusMessage * npd_dbus_ethports_interface_config_stp(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	slot_no,local_port_no,slot_index;
	unsigned int 	eth_g_index = 0;
	int ret = 0;
	unsigned int	stp_enable;
	unsigned int	devNum;
	unsigned int	virPortNo;
	DBusError err;
	enum module_id_e module_type;
	unsigned int mode = STP_MODE;
	NPD_RECV_INFO  stpInfo;
	memset(&stpInfo,0,sizeof(stpInfo));
	
	syslog_ax_rstp_dbg("Entering config ethport stp!!!!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_UINT32,&mode,
		DBUS_TYPE_UINT32,&stp_enable,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_rstp_dbg("index %#0x %s",eth_g_index,stp_enable ? "enable":"disable");
	ret = NPD_DBUS_ERROR_NO_SUCH_PORT;

	slot_index 		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	slot_no			= CHASSIS_SLOT_INDEX2NO(slot_index);
	local_port_no	= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	local_port_no 	= ETH_LOCAL_INDEX2NO(slot_index,local_port_no);
	
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
			ret = NPD_DBUS_SUCCESS;
		}
		else{
			 syslog_ax_rstp_err("illegal port %d",local_port_no);
		}
	}
	else{
		 syslog_ax_rstp_err("illegal slot %d",slot_no);
	}

	/* here config eth port <slot/port> stp state */
	/* !!!! */
	module_type = MODULE_TYPE_ON_SLOT_INDEX(slot_index);

	ret = nam_get_devport_by_slotport(slot_no, local_port_no, module_type, &devNum, &virPortNo);
	if(ret != 0){
		 syslog_ax_rstp_err("Npd_dbus::nam_get_devport_by_slotport::get devport Error. port maybe Master Control Port");
	}
	else {
		syslog_ax_rstp_dbg("nam_get_devport_by_slotport::slot_no %d,local_port_no %d,devNum %d,virPortNo %d",slot_no,local_port_no,devNum,virPortNo);

		if(NPD_SUCCESS != npd_eth_port_config_stp(eth_g_index,mode,stp_enable)) {
			syslog_ax_rstp_err("npd eth port %#x config stp fail",eth_g_index);
		}

		if(stp_enable) {
			 /* according to user_data*/
			 ret = nam_asic_stp_port_enable((unsigned char)devNum,virPortNo,stp_enable);
			  syslog_ax_rstp_dbg("Npd_dbus::npd_dbus_ethports_interface_config_stp::ret %x.",ret);
			 /*if((ret = npd_rstp_add_port(eth_g_index))<0)
				  syslog_ax_rstp_err(("call npd_rstp_add_port error"));
			 else{
				  syslog_ax_rstp_dbg(("call npd_rstp_add_port ok"));
				ret = npd_rstp_recv_info(&stpInfo,sizeof(stpInfo));
				if(ret <= 0){
					 syslog_ax_rstp_err(("recv err info\n"));
					ret = NPD_SUCCESS;
				}
				else{
					ret = stpInfo.ret;
					 syslog_ax_rstp_dbg(("recv succeed ret %02x\n",ret));
				}
			 }*/

		}
		else{
			 ret = nam_asic_stp_port_enable((unsigned int)devNum, virPortNo, stp_enable);
			 /*if((ret = npd_rstp_del_port(eth_g_index))<0)
				  syslog_ax_rstp_err(("call npd_rstp_del_port error"));
			 else{
				  syslog_ax_rstp_dbg(("call npd_rstp_del_port ok"));
				ret = npd_rstp_recv_info(&stpInfo,sizeof(stpInfo));
				if(ret <= 0){
					 syslog_ax_rstp_err(("recv err info\n"));
					ret = NPD_SUCCESS;
				}
				else{
					ret = stpInfo.ret;
					 syslog_ax_rstp_dbg(("recv succeed ret %02x\n",ret));
				}
				
			 }*/
		}
	}
		
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_INT32,
									 &ret);
	

	return reply;
}

DBusMessage *npd_dbus_stp_set_stpid_for_vlan(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusError err;
	unsigned short vid = 0;
	unsigned int mstid = 0;
	int ret = NPD_SUCCESS;
	unsigned int	port_index = 0, slot_index =0, eth_g_index = 0, iswan = 0,local_port_no = 0;
	unsigned int i = 0 ,j = 0, slotno = 0,portno = 0;
	enum module_id_e module_type = MODULE_ID_NONE;

#define SYSFS_PATH_MAX 256
		FILE *f = NULL;
		char path[SYSFS_PATH_MAX] = {0};

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&vid,
		DBUS_TYPE_UINT32,&mstid,
		DBUS_TYPE_INVALID))) {
		syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	syslog_ax_rstp_dbg("bind stpid %d to vlanid %d\n",mstid,vid);
	/* used for asic port  */
	ret = npd_mstp_vlan_stpid_bind(vid,mstid);

	/* used for cpu port  */
	for (i = 0; i < CHASSIS_SLOT_COUNT; i++ ) {
		for (j = 0; j < ETH_LOCAL_PORT_COUNT(i); j++) {
			port_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i, j);
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(port_index);
			slotno= CHASSIS_SLOT_INDEX2NO(slot_index);
			local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(port_index);
			local_port_no = ETH_LOCAL_INDEX2NO(slot_index, local_port_no);
			module_type = MODULE_TYPE_ON_SLOT_INDEX(slot_index);
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno, local_port_no);
			syslog_ax_rstp_dbg("port_index 0x%x slot_index %d slotno %d, localPortNo %d eth_g_index 0x%x\n",port_index,slot_index,slotno,local_port_no,eth_g_index);

		if(npd_eslot_check_subcard_module(module_type) && \
			((AX51_GTX == npd_check_subcard_type(PRODUCT_ID, local_port_no)) || \
			 (AX51_SFP == npd_check_subcard_type(PRODUCT_ID, local_port_no)))) {
			iswan = 1;
		}
		else if(!PRODUCT_IS_BOX && (AX7_CRSMU_SLOT_NUM == slot_index)) { /* read CRSMU RGMII ethernet port info*/
			iswan = 1;
		}
		else if((MODULE_ID_AX5_5612I == module_type) && 
			(local_port_no >= 9) && (local_port_no <= 12)) {				
			iswan = 1;
		}			
		else if(MODULE_ID_AX5_5612E == module_type  || MODULE_ID_AX5_5608 == module_type){
			iswan = 1;
		}

		if(1 == iswan){
			syslog_ax_rstp_dbg("port_index %d slot_index %d localPortNo %d \n",port_index,slot_index,local_port_no);
			snprintf(path, SYSFS_PATH_MAX, "/sys/class/net/eth%d-%d.%d/brport", slotno, local_port_no,vid);
			f = fopen(path, "r");
			if (f) {
				fclose(f);
				ret = npd_mstp_add_port(vid,eth_g_index,iswan);
			}
			else{
				syslog_ax_rstp_dbg("/sys/class/net/eth%d-%d.%d not in br \n",slotno, local_port_no,vid);
			}
		}
		
		}
	}
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply; 
}


DBusMessage * npd_dbus_stp_set_port_pathcost(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusError err;
	struct eth_port_s* node = NULL;
	struct stp_info_s *stpInfo = NULL;
	unsigned int eth_g_index = 0,value = 0,mstid = 0;
	int ret = NPD_SUCCESS;

	 syslog_ax_rstp_dbg("Entering stp set port pathcost!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&mstid,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_UINT32,&value,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	node = npd_get_port_by_index(eth_g_index);
	/*NPD_DEBUG(("node %p, stpInfo %p", node, node->funcs.func_data[ETH_PORT_FUNC_DOT1W]));*/
	
	if(NULL == node) {
		ret = NPD_FAIL;
	}
	else {
		if(NULL == (stpInfo = node->funcs.func_data[ETH_PORT_FUNC_DOT1W])){
			ret = NPD_FAIL;

		}
		else {
		     stpInfo->pathcost[mstid] = value;
			 stpInfo->mstid[mstid] = mstid;
			if(mstid > 0)
				 stpInfo->mode = MST_MODE;
		}
	}
	

	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply; 
}

DBusMessage * npd_dbus_stp_set_port_prio(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusError err;
	struct eth_port_s* node = NULL;
	struct stp_info_s *stpInfo = NULL;
	unsigned int eth_g_index = 0,value = 0,mstid = 0;
	int ret = NPD_SUCCESS;

	 syslog_ax_rstp_dbg("Entering stp set port priority!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
				DBUS_TYPE_UINT32,&mstid,
				DBUS_TYPE_UINT32,&eth_g_index,
				DBUS_TYPE_UINT32,&value,
				DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	node = npd_get_port_by_index(eth_g_index);
	if(NULL == node) {
		ret = NPD_FAIL;
	}
	else {
		if(NULL == (stpInfo = node->funcs.func_data[ETH_PORT_FUNC_DOT1W]))
			ret = NPD_FAIL;
		else {
			stpInfo->mstid[mstid] = mstid;
			stpInfo->prio[mstid]= value;
			if( mstid > 0)
			   stpInfo->mode = MST_MODE;
		}
	}

	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply; 
}
DBusMessage * npd_dbus_stp_set_port_nonstp(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusError err;
	struct eth_port_s* node = NULL;
	struct stp_info_s *stpInfo = NULL;
	unsigned int eth_g_index = 0,value = 0;
	int ret = NPD_SUCCESS;

	 syslog_ax_rstp_dbg("Entering stp set port nodstp!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_UINT32,&value,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	node = npd_get_port_by_index(eth_g_index);
	if(NULL == node) {
		ret = NPD_FAIL;
	}
	else {
		if(NULL == (stpInfo = node->funcs.func_data[ETH_PORT_FUNC_DOT1W]))
			ret = NPD_FAIL;
		else
			stpInfo->nonstp = value;
	}

	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply; 
}

DBusMessage * npd_dbus_stp_set_port_p2p(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusError err;
	struct eth_port_s* node = NULL;
	struct stp_info_s *stpInfo = NULL;
	unsigned int eth_g_index = 0,value = 0;
	int ret = NPD_SUCCESS;

	 syslog_ax_rstp_dbg("Entering stp set port p2p!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_UINT32,&value,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	node = npd_get_port_by_index(eth_g_index);
	if(NULL == node) {
		ret = NPD_FAIL;
	}
	else {
		if(NULL == (stpInfo = node->funcs.func_data[ETH_PORT_FUNC_DOT1W]))
			ret = NPD_FAIL;
		else
			stpInfo->p2p = value;
	}

	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply; 
}

DBusMessage * npd_dbus_stp_set_port_edge(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusError err;
	struct eth_port_s* node = NULL;
	struct stp_info_s *stpInfo = NULL;
	unsigned int eth_g_index = 0,value = 0;
	int ret = NPD_SUCCESS;

	 syslog_ax_rstp_dbg("Entering stp set port edge!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_UINT32,&value,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	node = npd_get_port_by_index(eth_g_index);
	if(NULL == node) {
		ret = NPD_FAIL;
	}
	else {
		if(NULL == (stpInfo = node->funcs.func_data[ETH_PORT_FUNC_DOT1W]))
			ret = NPD_FAIL;
		else
			stpInfo->edge= value;
	}

	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply; 
}

int npd_save_stp_mode_cfg
(
	struct stp_info_s* 	stpInfo,
	unsigned char			slot,
	unsigned char			port,
	char * 						buf,
	unsigned int* 			buflen
)
{
	char* tmpPtr = buf;
	unsigned length = 0;

	if(1 == stpInfo->stpEnable){
		length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d enable\n",slot,port);
		/*printf("stpInfo->stpEnable %d\n", stpInfo->stpEnable);*/
		tmpPtr = buf+length;
		
		if(DEF_PORT_PRIO != stpInfo->prio[MIN_MST_ID]) {
			length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d priority %d\n",slot,port,stpInfo->prio[MIN_MST_ID]);
			/*printf("stpInfo->priority %ld\n", stpInfo->prio[MIN_MST_ID]);*/
			tmpPtr = buf+length;
		}
		if(ADMIN_PORT_PATH_COST_AUTO != stpInfo->pathcost[MIN_MST_ID]){
			length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d path-cost %d\n",slot,port,stpInfo->pathcost[MIN_MST_ID]);
			tmpPtr = buf+length;
		}

		if(DEF_ADMIN_EDGE != stpInfo->edge) {
			length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d edge no\n",slot,port);
			tmpPtr = buf+length;
		}

		if(DEF_P2P != stpInfo->p2p) {
			length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d p2p %s\n",slot,port,stpInfo->prio ? "no" : "yes");
			tmpPtr = buf+length;
		}		
		if(DEF_ADMIN_NON_STP != stpInfo->nonstp) {
			length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d none-stp yes\n",slot,port);
			tmpPtr = buf+length;
		}
	
	}
	*buflen = length;
	return 0;
}

#if 1
int npd_save_mstp_mode_cfg
(
	struct stp_info_s* 	stpInfo,
	unsigned char			slot,
	unsigned char			port,
	char * 						buf,
	unsigned int* 			buflen
)
{
	char* tmpPtr = buf;
	unsigned length = 0;
	unsigned int  j = 0;
	
	if(1 == stpInfo->stpEnable){
		length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d enable\n",slot,port);
		/*printf("stpInfo->stpEnable %d\n", stpInfo->stpEnable);*/
		tmpPtr = buf+length;
		for(j = 0;j < 64; j++){
			if( j == stpInfo->mstid[j]){
				if(DEF_PORT_PRIO != stpInfo->prio[j]) {
					length += sprintf(tmpPtr,"config spanning-tree %d eth-port %d/%d priority %d\n",j,slot,port,stpInfo->prio[j]);
					tmpPtr = buf+length;
				}
				if(ADMIN_PORT_PATH_COST_AUTO != stpInfo->pathcost[j]){
					length += sprintf(tmpPtr,"config spanning-tree %d eth-port %d/%d path-cost %d\n",j,slot,port,stpInfo->pathcost[j]);
					tmpPtr = buf+length;
				}
			}
		}
  
		if(DEF_ADMIN_EDGE != stpInfo->edge) {
			length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d edge no\n",slot,port);
			tmpPtr = buf+length;
		}

		if(DEF_P2P != stpInfo->p2p) {
			length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d p2p %s\n",slot,port,stpInfo->prio ? "no" : "yes");
			tmpPtr = buf+length;
		}		
		if(DEF_ADMIN_NON_STP != stpInfo->nonstp) {
			length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d none-stp yes\n",slot,port);
			tmpPtr = buf+length;
		}
	}

	*buflen = length;
	return 0;
}
#endif

#if 0
int npd_save_mstp_mode_cfg
(
	struct stp_info_s* 	stpInfo, 
	unsigned char			slot,
	unsigned char			port,
	char * 						buf,
	unsigned int* 			buflen
)
{
	char* tmpPtr = buf;
	unsigned length = 0;
	int i;
	
	if(1 == stpInfo->stpEnable) {
		length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d enable\n",slot,port);
		tmpPtr = buf+length;
		
		for(i = 0; i < MAX_MST_ID; i++) {
			if(i ==0 || stpInfo->mstid[i] != 0) {
				if(DEF_PORT_PRIO != stpInfo->prio[i]) {
					length += sprintf(tmpPtr,"config spanning-tree %d eth-port %d/%d priority %d\n",stpInfo->mstid[i],slot,port,stpInfo->prio[i]);
					tmpPtr = buf+length;
				}

				if(ADMIN_PORT_PATH_COST_AUTO != stpInfo->pathcost[i]) {
					length += sprintf(tmpPtr,"config spanning-tree %d eth-port %d/%d path-cost %d\n",stpInfo->mstid[i],slot,port,stpInfo->pathcost[i]);
					tmpPtr = buf+length;
				}
			}
		}

		if(DEF_ADMIN_EDGE != stpInfo->edge) {
			length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d edge no\n",slot,port);
			tmpPtr = buf+length;
		}

		if(DEF_P2P != stpInfo->p2p) {
			length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d p2p %s\n",slot,port,stpInfo->prio ? "no" : "yes");
			tmpPtr = buf+length;
		}

		if(DEF_ADMIN_NON_STP != stpInfo->nonstp) {
			length += sprintf(tmpPtr,"config spanning-tree eth-port %d/%d none-stp yes\n",slot,port);
			tmpPtr = buf+length;
		}
	}

	*buflen = length;
	return 0;
}
#endif

DBusMessage * npd_dbus_stp_show_running_cfg(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;

	char *showStr = NULL,*cursor = NULL;
	int totalLen = 0;
	unsigned char slot_index = 0;
	unsigned int slot_no = 0,local_port_no = 0;
	unsigned char local_port_count = 0;
	unsigned int loop = 0;
	int flag = 0;
	
	showStr = (char*)malloc(NPD_STP_RUNNING_CFG_MEM);
	if(NULL == showStr) {
		 syslog_ax_rstp_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_STP_RUNNING_CFG_MEM);
	cursor = showStr;
	/*printf("###ethport stp  show running config\n");*/
	if(!PRODUCT_IS_BOX) {/* chassis product */
		slot_no = 1; /* slot 0:CRSMU port not add to default vlan*/
		loop = CHASSIS_SLOT_COUNT;
		flag = 1;
	}
	else { /* box product */
		slot_no = CHASSIS_SLOT_START_NO; /* as default we assume box product has subcard */
		loop = CHASSIS_SLOT_COUNT;
		flag = 0;
		/* !!!excetion!!!
		 * box product without subcard
		 * override loop count */
		if(CHASSIS_SLOT_START_NO == CHASSIS_SLOT_COUNT) {
			loop = CHASSIS_SLOT_COUNT+1;
		}
	}
	for (; slot_no < loop; slot_no++ ) {
		
		slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
	    local_port_count = ETH_LOCAL_PORT_COUNT(slot_index);
		local_port_no= ETH_LOCAL_PORT_START_NO(slot_index);
		
		for (; local_port_no <= local_port_count; local_port_no++ ) {

			struct eth_port_s* portInfo = NULL;
			struct stp_info_s* stpInfo = NULL;
			char tmpBuf[2048] = {0};
			unsigned int length = 0;
			unsigned int eth_g_index = 0;
			
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
			printf("npd >> show running cfg: slot_no: %u, local_port_no: %u, eth_g_index: %u\n", \
					slot_no, local_port_no, eth_g_index);
			syslog_ax_rstp_dbg("npd >> show running cfg: slot_no: %u, local_port_no: %u, eth_g_index: %u\n", \
					slot_no, local_port_no, eth_g_index);
			/*printf("###eth_g_index %d\n", eth_g_index);*/
			
			portInfo = npd_get_port_by_index(eth_g_index);
			if(NULL == portInfo) 
				continue;
			else if(NULL == (stpInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1W]))
				/*printf("port_index [%d] init\n",eth_g_index);*/
				continue;
			else {
				/*printf("##Coming here now 1180\n");*/
				switch (stpInfo->mode) {
					case STP_MODE :
						npd_save_stp_mode_cfg(stpInfo,slot_no,local_port_no,tmpBuf,&length);
						if((totalLen + length) > NPD_STP_RUNNING_CFG_MEM) { /* 20 for enter node; 5 for exit node*/
							syslog_ax_rstp_err("show ethport buffer full");
							if(flag){
								slot_no = CHASSIS_SLOT_COUNT;
							}
							else{
								slot_no = CHASSIS_SLOT_COUNT + 1;
								
							}
							local_port_no = local_port_count;
							break;
						}

						totalLen += sprintf(cursor,"%s",tmpBuf);
						cursor = showStr + totalLen;
						break;

					case MST_MODE :
						npd_save_mstp_mode_cfg(stpInfo,slot_no,local_port_no,tmpBuf,&length);
						if((totalLen + length) > NPD_STP_RUNNING_CFG_MEM) { /* 20 for enter node; 5 for exit node*/
							syslog_ax_rstp_dbg("show ethport buffer full");
							if(flag){
								slot_no = CHASSIS_SLOT_COUNT;
							}
							else{
								slot_no = CHASSIS_SLOT_COUNT + 1;
								
							}
							local_port_no = local_port_count;
							break;
						}

						totalLen += sprintf(cursor,"%s",tmpBuf);
						cursor = showStr + totalLen;
						break;

					default :
						syslog_ax_rstp_err("stp_running_cfg :: stp mode error %d\n",stpInfo->mode);
						break;
				}
			}	
		}			
	}		

	if((totalLen + 4) > NPD_STP_RUNNING_CFG_MEM) {
	    free(showStr);
	    showStr = NULL;
		return NULL;
	}
	else {
		totalLen += sprintf(cursor,"exit\n");
		cursor = showStr + totalLen;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr);	
	free(showStr);
	showStr = NULL;
	return reply;
}

DBusMessage * npd_dbus_stp_all_vlans_bind_to_stpid(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	int ret = NPD_SUCCESS;
	unsigned int enable = 0;


	syslog_ax_rstp_dbg("Entering stp set stp struct!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args (msg,&err,
						DBUS_TYPE_UINT32,&enable,
						DBUS_TYPE_INVALID))) {
		syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if(1 == enable){
			ret = npd_stp_vlan_stpid_bind_init();
		    syslog_ax_rstp_dbg("npd_stp_vlan_stpid_bind_init value is %d\n",ret);
		}
	else if(0 == enable)
		npd_stp_struct_init();

	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage * npd_dbus_stp_get_port_link_state(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	unsigned int lk = 0;
	int ret = NPD_SUCCESS;
	unsigned int eth_g_index = 0;
	struct eth_port_s* portInfo = NULL;
	unsigned int attr_map = 0, isWAN = 0;

	
	 syslog_ax_rstp_dbg("Entering get stp port link state!\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
syslog_ax_rstp_dbg("npd_dbus_stp_get_port_link_state eth_g_index is %d\n",eth_g_index);
/*
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL != portInfo) {
		npd_get_port_link_status(eth_g_index,&lk);
		portInfo->attr_bitmap |= (lk << ETH_LINK_STATUS_BIT) & ETH_ATTR_LINK_STATUS;
		#if 0
		if(portInfo->attr_bitmap & ((ETH_ATTR_LINKUP << ETH_LINK_STATUS_BIT) & ETH_ATTR_LINK_STATUS)) 
			lk = 1;
		else
			lk = 0;
		#endif
	}
*/
	ret = npd_get_port_attrmap_by_gindex(eth_g_index, &isWAN, &attr_map);
	if (!ret) {
		if(attr_map & ETH_ATTR_LINK_STATUS) {
			lk = 1;
		}
		else {
			lk = 0;
		}
	}
syslog_ax_rstp_dbg("npd_dbus_stp_get_port_link_state eth_g_index is %d, lk is %d\n",eth_g_index, lk);
	
	/*printf("npd_dbus 6338 >> lkstae %s\n",lk ? "up" : "down");*/
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_UINT32,&lk,
							 DBUS_TYPE_UINT32,&isWAN,
							 DBUS_TYPE_INVALID);
	
	return reply;
}


DBusMessage * npd_dbus_stp_get_port_speed(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;

	int ret = NPD_SUCCESS;
	unsigned int eth_g_index = 0;
	unsigned int speed = 0, attr_map = 0, isWAN = 0;

	
	 syslog_ax_rstp_dbg("Entering get stp port link state!\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	/*
    npd_get_port_speed(eth_g_index,&speed);
*/
	ret = npd_get_port_attrmap_by_gindex(eth_g_index, &isWAN, &attr_map);
	if (!ret) {
		speed = (attr_map & ETH_ATTR_SPEED_MASK);
	}
syslog_ax_rstp_dbg("npd_dbus_stp_get_port_speed eth_g_index is %d, speed is %d\n",eth_g_index, speed);

	/*printf("npd_dbus 6338 >> lkstae %s\n",lk ? "up" : "down");*/
	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INT32,&speed,
							 DBUS_TYPE_INVALID);
	
	return reply;
}

DBusMessage * npd_dbus_stp_get_port_duplex_mode(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;

	int ret = NPD_SUCCESS;
	unsigned int eth_g_index = 0;
	unsigned int duplex_mode = 0, attr_map = 0, isWAN = 0;

	
	 syslog_ax_rstp_dbg("Entering get stp port link state!\n");

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	/*
    npd_get_port_duplex_mode(eth_g_index,&duplex_mode);
*/
	ret = npd_get_port_attrmap_by_gindex(eth_g_index, &isWAN, &attr_map);
	if (!ret) {
		duplex_mode = (attr_map & ETH_ATTR_DUPLEX);
	}

	reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INT32,&duplex_mode,
							 DBUS_TYPE_INVALID);
	
	return reply;
}


DBusMessage * npd_dbus_rstp_get_one_port_index(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	slot_no,port_no;
	unsigned int 	eth_g_index = 0;
	int ret;
	DBusError err;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&slot_no,
		DBUS_TYPE_BYTE,&port_no,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	
    if(slot_no == 0xEE)              /* slot = 0xEE for stp get slot NUM*/
    {
		slot_no = SLOT_ID;		
    }

	
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) {
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
			ret = NPD_DBUS_SUCCESS;
		}
	}
		
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &eth_g_index);
	return reply;
}


DBusMessage * npd_dbus_rstp_get_all_port_index(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;


	unsigned int loop = 0;
	unsigned int slot_index = 0, eth_g_index = 0;
	unsigned char local_port_count = 0;
	unsigned char slot_no = 0,local_port_no = 0;
	unsigned int portmap = 0;
	unsigned char map_rev = 0;
	int ret = 0;
	
	syslog_ax_rstp_dbg("return slot_no port_no  and port_index!\n");
	
	if(!PRODUCT_IS_BOX) {/* chassis product */
		slot_no = 1; /* slot 0:CRSMU port not add to default vlan*/
		loop = CHASSIS_SLOT_COUNT;
		printf("!PRODUCT_IS_BOX:: slot_no is %d,loop is %d\n",slot_no,loop);
	}
	else { /* box product */
		slot_no = CHASSIS_SLOT_START_NO; /* as default we assume box product has subcard */
		loop = CHASSIS_SLOT_COUNT;
	
		/* !!!excetion!!!
		 * box product without subcard
		 * override loop count */
		if(CHASSIS_SLOT_START_NO == CHASSIS_SLOT_COUNT) {
			loop = CHASSIS_SLOT_COUNT+1;
		}		
		printf("PRODUCT_IS_BOX:: slot_no is %d,loop is %d\n",slot_no,loop);
	}
	for(; slot_no < loop ; slot_no++)
	{
		if(CHASSIS_SLOTNO_ISLEGAL(slot_no)){
			slot_index =  CHASSIS_SLOT_NO2INDEX(slot_no);
			

			local_port_no= ETH_LOCAL_PORT_START_NO(slot_index);
			local_port_count = ETH_LOCAL_PORT_COUNT(slot_index);
			
			for(; local_port_no <= local_port_count; local_port_no++)
			{
				if(0 != local_port_no) {
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no, local_port_no);
					if(PRODUCT_IS_BOX){
		
						portmap |=  1<<((slot_no-1)*8+local_port_no);
					}
					else {
						if (local_port_no % 6)
							map_rev = local_port_no %6;
						else
							map_rev = 6;
					
						portmap |= 1<<(((slot_no -1)%6)*8 + map_rev);
					}
				}
				else{
					continue;
				}
				/*port_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no); ......
				//printf(" \n");
				//printf("RSTP>> local_port_no = %d ; port_index = %d\n",local_port_no,port_index);*/

			}
			printf("NPD >> portmap %u\n", portmap);
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);

	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &portmap);

	/*printf("END\n");*/
	return reply;
}

DBusMessage * npd_dbus_rstp_get_all_port_index_v1(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;


	unsigned int loop = 0;
	unsigned int slot_index = 0, eth_g_index = 0;
	unsigned char local_port_count = 0;
	unsigned char slot_no = 0,local_port_start_no = 0,local_port_no=0;
	PORT_MEMBER_BMP portmap;
	unsigned char map_rev = 0;
	int ret = 0;

	memset(&portmap,0,sizeof(PORT_MEMBER_BMP));
	syslog_ax_rstp_dbg("return slot_no port_no  and port_index!\n");

    if(SYSTEM_TYPE == IS_DISTRIBUTED)
    {

		slot_no = CHASSIS_SLOT_START_NO; /* as default we assume box product has subcard */
		loop = CHASSIS_SLOT_COUNT+CHASSIS_SLOT_START_NO;
		syslog_ax_rstp_dbg("Distributed OS:: slot_no is %d,loop is %d\n",slot_no,loop);
    	for(; slot_no < loop ; slot_no++)
    	{
    		if(CHASSIS_SLOTNO_ISLEGAL(slot_no)){
    			slot_index =  CHASSIS_SLOT_NO2INDEX(slot_no);
    			
    			local_port_start_no= ETH_LOCAL_PORT_START_NO(slot_index);
    			local_port_count = ETH_LOCAL_PORT_COUNT(slot_index);
			    local_port_no = local_port_start_no;
            	syslog_ax_rstp_dbg("local_port_no%d\n",local_port_no);
            	syslog_ax_rstp_dbg("local_port_count:: %d\n",local_port_count);

    			for(; local_port_no<(local_port_count +local_port_start_no); local_port_no++)
    			{
                    #if 1
	                portmap.portMbr[local_port_no/32] |= 1<<((local_port_no-1)%32);

					#else
    				if(0 != local_port_no) {
    					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no, local_port_no);
    					if(PRODUCT_IS_BOX){
    						if(PRODUCT_ID_AX5K_I == PRODUCT_ID) {
    							portmap.portMbr[(slot_no*8 + local_port_no)/32] |= 1 << ((slot_no*8+local_port_no)%32 -1);
    						}
    						else {
    			                portmap.portMbr[local_port_no/32] |= 1<<((slot_no-1)*8+(local_port_no%32));
    							/*portmap |=  1<<((slot_no-1)*8+local_port_no);*/
    						}
    					}
    					else {
    						if (local_port_no % 6)
    							map_rev = local_port_no %6;
    						else
    							map_rev = 6;
    					    portmap.portMbr[0] |= 1<<(((slot_no -1)%6)*8 + map_rev);
    						/*portmap |= 1<<(((slot_no -1)%6)*8 + map_rev);*/
    					}
    				}
    				else{
    					continue;
    				}
    				/*port_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no); ......
    				//printf(" \n");
    				//printf("RSTP>> local_port_no = %d ; port_index = %d\n",local_port_no,port_index);*/
                    #endif
    			}

				syslog_ax_rstp_dbg("1111----NPD >> portmap %#x,%#x\n", portmap.portMbr[0], portmap.portMbr[1]);
    			printf("NPD >> portmap %#x,%#x\n", portmap.portMbr[0], portmap.portMbr[1]);
    		}
    	}

		
    }
	else
	{
    	if(!PRODUCT_IS_BOX) {/* chassis product */
    		slot_no = 1; /* slot 0:CRSMU port not add to default vlan*/
    		loop = CHASSIS_SLOT_COUNT;
    		syslog_ax_rstp_dbg("!PRODUCT_IS_BOX:: slot_no is %d,loop is %d\n",slot_no,loop);
    	}
    	else { /* box product */
    		slot_no = CHASSIS_SLOT_START_NO; /* as default we assume box product has subcard */
    		loop = CHASSIS_SLOT_COUNT;
    	
    		/* !!!excetion!!!
    		 * box product without subcard
    		 * override loop count */
    		if(CHASSIS_SLOT_START_NO == CHASSIS_SLOT_COUNT) {
    			loop = CHASSIS_SLOT_COUNT+1;
    		}
    		syslog_ax_rstp_dbg("PRODUCT_IS_BOX:: slot_no is %d,loop is %d\n",slot_no,loop);
    	}
    	for(; slot_no < loop ; slot_no++)
    	{
    		if(CHASSIS_SLOTNO_ISLEGAL(slot_no)){
    			slot_index =  CHASSIS_SLOT_NO2INDEX(slot_no);
    			

			local_port_no= ETH_LOCAL_PORT_START_NO(slot_index);
			local_port_count = ETH_LOCAL_PORT_COUNT(slot_index);
			
			for(; local_port_no <= local_port_count; local_port_no++)
			{
				if(0 != local_port_no) {
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no, local_port_no);
					if(PRODUCT_IS_BOX){
						if(PRODUCT_ID_AX5K_I == PRODUCT_ID) {
							portmap.portMbr[(slot_no*8 + local_port_no)/32] |= 1 << ((slot_no*8+local_port_no)%32 -1);
						}
						else {
			                portmap.portMbr[local_port_no/32] |= 1<<((slot_no-1)*8+(local_port_no%32));
							/*portmap |=  1<<((slot_no-1)*8+local_port_no);*/
						}
					}
					else {
						if (local_port_no % 6)
							map_rev = local_port_no %6;
						else
							map_rev = 6;
					    portmap.portMbr[0] |= 1<<(((slot_no -1)%6)*8 + map_rev);
						/*portmap |= 1<<(((slot_no -1)%6)*8 + map_rev);*/
					}
				}
				else{
					continue;
				}
				/*port_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no); ......
				//printf(" \n");
				//printf("RSTP>> local_port_no = %d ; port_index = %d\n",local_port_no,port_index);*/

    			}
    			printf("NPD >> portmap %#x,%#x\n", portmap.portMbr[0], portmap.portMbr[1]);
    		}
    	}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);

	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &portmap.portMbr[0]);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &portmap.portMbr[1]);

	/*printf("END\n");*/
	return reply;
}

DBusMessage * npd_dbus_rstp_get_slot_port_from_index(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	
	unsigned int  eth_g_index = 0;
	unsigned char slot_index = 0,local_port_index = 0,slot_no = 0,local_port_no = 0;
	int ret = 0;
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&eth_g_index,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_rstp_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_rstp_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	slot_index		 = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	slot_no 		 = CHASSIS_SLOT_INDEX2NO(slot_index);
	local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	local_port_no 	 = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);


	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &slot_no);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_BYTE,
								 &local_port_no);

	/*printf("END\n");*/
	return reply;
}

#if 0
DBusMessage * npd_dbus_rstp_get_all_port_index(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter,iter_array;

	int i,j;
	
	unsigned char slot_count = CHASSIS_SLOT_COUNT;
	unsigned char local_port_count;
	unsigned char slot_no;
	unsigned char local_port_no;
	unsigned int    port_index = 0;

	 syslog_ax_rstp_dbg("return slot_no port_no  and port_index!\n");
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &slot_count);
	// syslog_ax_rstp_dbg("RSTP>> slot_count = %d\n",slot_count);
	
	dbus_message_iter_open_container (&iter,
					   DBUS_TYPE_ARRAY,
					   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
					   		DBUS_TYPE_BYTE_AS_STRING
					   		DBUS_TYPE_BYTE_AS_STRING
					   		DBUS_TYPE_ARRAY_AS_STRING
					   			DBUS_STRUCT_BEGIN_CHAR_AS_STRING					
							   		DBUS_TYPE_BYTE_AS_STRING
							   		DBUS_TYPE_UINT32_AS_STRING
							   	DBUS_STRUCT_END_CHAR_AS_STRING
						DBUS_STRUCT_END_CHAR_AS_STRING,
					   &iter_array);

	for(i = 0; i < slot_count ; i++)
	{
		local_port_count =  ETH_LOCAL_PORT_COUNT(i);
		slot_no = CHASSIS_SLOT_INDEX2NO(i);
		// syslog_ax_rstp_dbg(" \n##############################\n");
		// syslog_ax_rstp_dbg("RSTP>> local_port_count = %d ; slot_no = %d\n",local_port_count,slot_no);
		DBusMessageIter iter_struct;
		DBusMessageIter	iter_sub_array;
		
		dbus_message_iter_open_container (&iter_array,
								   DBUS_TYPE_STRUCT,
								   NULL,
								   &iter_struct);

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(slot_no));
		
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(local_port_count));

		dbus_message_iter_open_container (&iter_struct,
										   DBUS_TYPE_ARRAY,
										   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
												   DBUS_TYPE_BYTE_AS_STRING
												   DBUS_TYPE_UINT32_AS_STRING
											DBUS_STRUCT_END_CHAR_AS_STRING,
										   &iter_sub_array);
		
		for(j = 0; j < local_port_count ; j++)
		{

			local_port_no = ETH_LOCAL_INDEX2NO(i,j);
			port_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
			// syslog_ax_rstp_dbg(" \n");
			// syslog_ax_rstp_dbg("RSTP>> local_port_no = %d ; port_index = %d\n",local_port_no,port_index);
			DBusMessageIter iter_sub_struct;

			dbus_message_iter_open_container (&iter_sub_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_sub_struct);

			dbus_message_iter_append_basic
					(&iter_sub_struct,
					  DBUS_TYPE_BYTE,
					  &(local_port_no));

			dbus_message_iter_append_basic
					(&iter_sub_struct,
					  DBUS_TYPE_UINT32,
					  &(port_index));

			dbus_message_iter_close_container (&iter_sub_array, &iter_sub_struct);

		}

		dbus_message_iter_close_container (&iter_struct, &iter_sub_array);

		dbus_message_iter_close_container (&iter_array, &iter_struct);
		
	}

	dbus_message_iter_close_container (&iter, &iter_array);

	// syslog_ax_rstp_dbg("END\n");

	return reply;
}
#endif
int npd_stp_struct_init(){

	
	unsigned char loop = 0;
	unsigned char slot_index = 0;
	unsigned char slot_no = 0;
	unsigned char local_port_no = 0, local_port_count = 0, local_port_start_no=0;
	unsigned int 	eth_g_index = 0;
	
	struct eth_port_s *portInfo = NULL;
	struct stp_info_s *stpInfo = NULL;
	/*memset(portInfo, 0, sizeof(eth_port_s));*/
	/*memset(stpInfo, 0, sizeof(stp_info_s));*/
	
    if(SYSTEM_TYPE == IS_DISTRIBUTED)
    {
		slot_no = CHASSIS_SLOT_START_NO; 
		loop = CHASSIS_SLOT_COUNT + CHASSIS_SLOT_START_NO;
 
    	for(; slot_no < loop; slot_no++)
    	{	
    		slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
    		
    		local_port_start_no = ETH_LOCAL_PORT_START_NO(slot_index);
    		local_port_count =  ETH_LOCAL_PORT_COUNT(slot_index);
    		local_port_no = local_port_start_no;
    		for(; local_port_no < (local_port_count + local_port_start_no); local_port_no++) 
    		{	
    			syslog_ax_rstp_dbg("into sec for..... local port no %d\n",local_port_no);
    			syslog_ax_rstp_dbg("slot %d, port %d \n",slot_no,local_port_no);


    			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
    			portInfo = npd_get_port_by_index(eth_g_index);
    			
    			if(NULL == portInfo)
    			{
    				syslog_ax_rstp_dbg("portInfo == NULL\n");
    				continue;		
    			}
    			
    			stpInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1W];

    			if(NULL == stpInfo)
    			{
    				syslog_ax_rstp_dbg("stpInfo == NULL\n");
    				continue;
    			}
    			else if(NULL != stpInfo)
    			{

    				syslog_ax_rstp_dbg("stpInfo != NULL:: slot %d, port %d port_index %d\n",slot_no,local_port_no,eth_g_index);
					
    			#if 0
    				NPD_DEBUG(("stpInfo != NULL:: slot %d, port %d port_index %d\n",slot_no,local_port_no,eth_g_index));
    				stpInfo->stpEnable = 0;
    				NPD_DEBUG(("stpInfo != NULL:: stpInfo->stpEnable %d\n",stpInfo->stpEnable));
    				for(i = 0; i < MAX_MST_ID; i++) 
    				{
    					stpInfo->prio[i] = DEF_PORT_PRIO;
    					stpInfo->pathcost[i] = ADMIN_PORT_PATH_COST_AUTO;
    					stpInfo->mstid[i] = 0;
    				}
    				NPD_DEBUG(("##stpInfo != NULL:: stpInfo->stpEnable %d\n",stpInfo->stpEnable));

    				stpInfo->edge = DEF_ADMIN_EDGE;
    				stpInfo->p2p = DEF_P2P;
    				stpInfo->nonstp = DEF_ADMIN_NON_STP;
    				stpInfo->mode = 0;

    				stpInfo->npd_stp_port_notifier_func = npd_rstp_link_change;

    				NPD_DEBUG(("##stpInfo != NULL:: stpInfo->stpEnable %d\n",stpInfo->stpEnable));

    			#endif

    				free(stpInfo);
    				stpInfo = NULL;
    				portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1W] = NULL;	
    			}
    		}
    	}

    	return 0;
		
    }
	else
	{
    	syslog_ax_rstp_dbg("Init the port state\n");
    	if(!PRODUCT_IS_BOX) {/* chassis product */
    		slot_no = 1; /* slot 0:CRSMU port not add to default vlan*/
    		loop = CHASSIS_SLOT_COUNT;
    	}
    	else { /* box product */
    		slot_no = CHASSIS_SLOT_START_NO; /* as default we assume box product has subcard */
    		loop = CHASSIS_SLOT_COUNT;
    	
    		/* !!!excetion!!!
    		 * box product without subcard
    		 * override loop count */
    		if(CHASSIS_SLOT_START_NO == CHASSIS_SLOT_COUNT) {
    			loop = CHASSIS_SLOT_COUNT+1;
    		}
    	}

	for(; slot_no < loop; slot_no++)
	{	
		slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
		
		local_port_no = ETH_LOCAL_PORT_START_NO(slot_index);
		local_port_count =  ETH_LOCAL_PORT_COUNT(slot_index);
		
		for(; local_port_no < local_port_count ; local_port_no++) 
		{	
			syslog_ax_rstp_dbg("into sec for..... local port no %d\n",local_port_no);
			syslog_ax_rstp_dbg("slot %d, port %d \n",slot_no,local_port_no);


			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
			portInfo = npd_get_port_by_index(eth_g_index);
			
			if(NULL == portInfo)
			{
				syslog_ax_rstp_dbg("portInfo == NULL\n");
				continue;		
			}
			
			stpInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1W];

			if(NULL == stpInfo)
			{
				syslog_ax_rstp_dbg("stpInfo == NULL\n");
				continue;
			}
			else if(NULL != stpInfo)
			{
			#if 0
				NPD_DEBUG(("stpInfo != NULL:: slot %d, port %d port_index %d\n",slot_no,local_port_no,eth_g_index));
				stpInfo->stpEnable = 0;
				NPD_DEBUG(("stpInfo != NULL:: stpInfo->stpEnable %d\n",stpInfo->stpEnable));
				for(i = 0; i < MAX_MST_ID; i++) 
				{
					stpInfo->prio[i] = DEF_PORT_PRIO;
					stpInfo->pathcost[i] = ADMIN_PORT_PATH_COST_AUTO;
					stpInfo->mstid[i] = 0;
				}
				NPD_DEBUG(("##stpInfo != NULL:: stpInfo->stpEnable %d\n",stpInfo->stpEnable));

				stpInfo->edge = DEF_ADMIN_EDGE;
				stpInfo->p2p = DEF_P2P;
				stpInfo->nonstp = DEF_ADMIN_NON_STP;
				stpInfo->mode = 0;

				stpInfo->npd_stp_port_notifier_func = npd_rstp_link_change;

				NPD_DEBUG(("##stpInfo != NULL:: stpInfo->stpEnable %d\n",stpInfo->stpEnable));

			#endif

				free(stpInfo);
				stpInfo = NULL;
				portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1W] = NULL;	
			}			
		}
	}

    	syslog_ax_rstp_dbg("stpInfo != NULL:: slot %d, port %d port_index %d\n",slot_no,local_port_no,eth_g_index);
    	return 0;
	}
}

#ifdef __cplusplus
}
#endif

