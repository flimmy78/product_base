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
* npd_trunk.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for TRUNK module.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.85 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  /*getpagesize(  ) */  
#include <sys/ipc.h>   
#include <sys/shm.h>
#include <dbus/dbus.h>


#include <fcntl.h>
#include <sys/mman.h>  /* for mmap() */
#include <errno.h>

#include "npd_log.h"
#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_product.h"
#include "npd_c_slot.h"
#include "npd_vlan.h"
#include "npd_trunk.h"
#include "npd_eth_port.h"
#include "sysdef/returncode.h"
#include "npd_board.h"

#include <sys/mman.h>  /* for mmap() MS_SYNC */
#include "board/board_define.h"   /* MASTER_BOARD */

extern char g_dis_dev[MAX_ASIC_NUM];
extern dst_trunk_s* g_dst_trunk;
struct trunk_s  ** g_trunks = NULL;

char *trkLBalanc[LOAD_BANLC_MAX] = {
	 "based-mac",
	 "based-port",
	 "based-ip",
	 "based-L4",
	 "mac+ip",
	 "mac+L4",
	 "ip+L4",
	 "mac+ip+L4"
};

unsigned int g_loadBalanc = LOAD_BANLC_MAC_IP_L4;
unsigned int npd_dst_trunk_valid(unsigned short trunkId,char *trunkName);
unsigned int npd_dst_trunk_invalid(unsigned short trunkId);


void npd_init_trunks
(
	void
)
{
	syslog_ax_trunk_err("npd init trunk count %d\n",NPD_TRUNK_NUMBER_MAX);
	g_trunks = malloc(sizeof(struct trunk_s*)*(NPD_TRUNK_NUMBER_MAX));
	if(NULL == g_trunks)
	{
		 syslog_ax_trunk_dbg("memory alloc error for trunk init!!\n");
		return;
	}
	
	memset(g_trunks , 0, sizeof(struct trunk_s*)*NPD_TRUNK_NUMBER_MAX);
	
	return;
}
/* check a trunk node has exists in the list or not*/
/* we use TRUNKiD as a index for searching for the node*/
/**********************************************************************************
 *  npd_check_trunk_exist
 *
 *	DESCRIPTION:
 * 		Check out if specified TRUNK has been created or not
 * 		TRUNK ID used as an index to find trunk structure.
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUNK_BADPARAM  - if parameters given are wrong
 *		NPD_TRUNK_EXISTS	- if trunk has been created before 
 *		NPD_TRUNK_NOTEXISTS	- if trunk doesn't exist,or hasn't been created before
 *
 **********************************************************************************/
unsigned int npd_check_trunk_exist
(
	unsigned short trunkId
)
{
	/*trunkId should be in range of (0,119]*/
	if((trunkId == 0)||(trunkId > NPD_TRUNK_NUMBER_MAX))
		return TRUNK_RETURN_CODE_BADPARAM;  /*MACRO =12*/

	if(NULL == g_trunks[trunkId-1]) {
		return TRUNK_RETURN_CODE_TRUNK_NOTEXISTS; /*MACRO =24*/
	}
	else
		return TRUNK_RETURN_CODE_TRUNK_EXISTS; /*MACRO =23*/
}

/**********************************************************************************
 *  npd_find_trunk
 *
 *	DESCRIPTION:
 * 		Check out if specified trunk has been created or not
 * 		TRUNK ID used as an index to find trunk structure.
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL  - if parameters given are wrong
 *		trunkNode	- if trunk has been created before 
 *		
 **********************************************************************************/
struct trunk_s *npd_find_trunk
(
	unsigned short trunkId
)
{
	struct trunk_s *trunkNode = NULL;
	/*trunkId should be in range of (0,119]*/
	if((trunkId == 0)||(trunkId> NPD_TRUNK_NUMBER_MAX))
		trunkNode = NULL;

	trunkNode = g_trunks[trunkId-1];

	return trunkNode;
}

/**********************************************************************************
 *  npd_find_trunk_by_name
 *
 *	DESCRIPTION:
 * 		Check out if specified trunk has been created or not
 * 		name is used to compare with each trunk exists.
 *
 *	INPUT:
 *		name - trunk name
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		pointer to trunk found.
 *
 **********************************************************************************/
struct trunk_s* npd_find_trunk_by_name
(
	char *name
)
{
	struct trunk_s* node = NULL;

	int iter = 0, found = 0;

	for(iter = 0;iter < NPD_TRUNK_NUMBER_MAX; iter++) {
		node = g_trunks[iter];
		if(NULL == node)
			continue;	/*trunk null*/
		else if('\0' == node->trunkName[0]) {
			continue;	/*trunk name null*/
		}
		else if(strcmp(name,(char*)node->trunkName)) {			
			continue;	/*trunk name not match*/
		}
		else {
			found = 1;
			break;
		}
	}

	if(found)
		return node;
	else
		return NULL;
}
/**********************************************************************************
 *  npd_check_trunk_status
 *
 *	DESCRIPTION:
 * 		Check out if specified trunk is UP or DOWN
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		TRUNK_STATE_UP_E  - if trunk is up
 *		TRUNK_STATE_DOWN_E	- if trunk is down
 *		-NPD_TRUNK_NOTEXISTS	- if trunk doesn't exist,or hasn't been created before
 *
 **********************************************************************************/
int npd_check_trunk_status
(
	unsigned short trunkId
)
{
	struct trunk_s 	*trunkEntity = NULL;
	unsigned int ret = TRUNK_RETURN_CODE_ERR_NONE;

	/* first check vlan exists or not*/
	ret = npd_check_trunk_exist(trunkId);
	if(TRUNK_RETURN_CODE_TRUNK_NOTEXISTS == ret) {
		return TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
	}
	else if(TRUNK_RETURN_CODE_BADPARAM == ret) {
		return TRUNK_RETURN_CODE_BADPARAM;
	}
	else if(TRUNK_RETURN_CODE_TRUNK_EXISTS == ret) {
		/*seconde check trunk state*/
		trunkEntity = g_trunks[trunkId-1];
		if(TRUNK_STATE_UP_E == trunkEntity->tLinkState) {
			ret = TRUNK_STATE_UP_E;
		}
		else {
			ret = TRUNK_STATE_DOWN_E;
		}
	}

	return ret;
}

/**********************************************************************************
 *  npd_create_trunk
 *
 *	DESCRIPTION:
 * 		Create trunk node by TRUNK ID
 * 		Trunk node should be have not been created before.
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		pointer to trunk being created.
 *
 **********************************************************************************/
struct trunk_s* npd_create_trunk
(
	unsigned short trunkId
)
{
	struct trunk_s* node = NULL;
	
	node = (struct trunk_s *)malloc(sizeof(struct trunk_s));
	if(NULL == node) {
		 syslog_ax_trunk_err("null memory allocated!\n");
		return NULL;
	}
	memset(node,0,sizeof(struct trunk_s));

	node->trunkId = trunkId;
	return node;
}
/**********************************************************************************
 *  npd_create_trunk_by_name
 *
 *	DESCRIPTION:
 * 		Create trunk node by TRUNK ID
 * 		Vlan node should be have not been created before.
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		pointer to trunk being created.
 *
 **********************************************************************************/
struct trunk_s* npd_create_trunk_by_name
(
	char *name
)
{
	struct trunk_s* node = NULL;

	if(NULL == name || NPD_TRUNK_IFNAME_SIZE < strlen(name)){
		return NULL;
	}
		node = (struct trunk_s *)malloc(sizeof(struct trunk_s));
		if(NULL == node) {
			 syslog_ax_trunk_err("error:null memory allocated!\n");
			return NULL;
		}
		memset(node,0,sizeof(struct trunk_s));
		memcpy(node->trunkName , name ,NPD_TRUNK_IFNAME_SIZE );
				
	return node;
}

/**********************************************************************************
 *  npd_delete_trunk_by_vid
 *
 *	DESCRIPTION:
 * 		Delete trunk node by TRUNK ID
 * 		Vlan node should be have been created before.
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUNK_ERR_NONE - if no error occurred
 *		NPD_TRUNK_NOTEXISTS - if trunk does not exists
 *		
 *
 **********************************************************************************/
unsigned int npd_delete_trunk
(
	unsigned short trunkId
)
{
	struct trunk_s* node = NULL;
	int retval = TRUNK_RETURN_CODE_ERR_NONE;

	node = npd_find_trunk(trunkId);

	if (NULL == node) {
	 	retval = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
	}
	else {
		
        /* add for update the g_vlanlist[], add trunk port to default vlan, for smu */
		syslog_ax_trunk_dbg("add trunk %d port to vlan-1\n",trunkId);
		retval = npd_trunk_list_delete(trunkId);
        if(retval != TRUNK_RETURN_CODE_ERR_NONE)
        {			
		    syslog_ax_trunk_err("delete trunk %d node error\n",trunkId);
			return retval;
        }
		/* add end */
		
		syslog_ax_trunk_dbg("free trunk %d node\n",trunkId);
		free(node);
		npd_trunk_flushout_global(trunkId);
	}
	return retval;
}

/**********************************************************************************
 *  npd_trunk_fillin_global
 *
 *	DESCRIPTION:
 * 		fill in trunk info to global trunk array for specified trunk
 *
 *	INPUT:
 *		trunkId - trunk id
 *		trunkNode - trunk structure pointer
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUNK_ERR_NONE - if no error occurred.
 *		NPD_TRUNK_BADPARAM - if trunk id is out of range.
 *
 **********************************************************************************/
unsigned int npd_trunk_fillin_global
(
	unsigned short trunkId,
	struct trunk_s	*trunkNode
)
{
	if(NULL == trunkNode){
		return TRUNK_RETURN_CODE_BADPARAM;
	}
	
	/*trunkId should be in range of (0,119]*/
	if((trunkId == 0)||(trunkId > NPD_TRUNK_NUMBER_MAX))
		return TRUNK_RETURN_CODE_BADPARAM;

	g_trunks[trunkId- 1] = trunkNode;

	return TRUNK_RETURN_CODE_ERR_NONE;
}
/**********************************************************************************
 *  npd_trunk_flushout_global
 *
 *	DESCRIPTION:
 * 		flush out trunk info from global trunk array for specified trunk
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUNK_ERR_NONE - if no error occurred.
 *		NPD_TRUNK_BADPARAM - if trunk id is out of range.
 *
 **********************************************************************************/

unsigned int npd_trunk_flushout_global
(
	unsigned short trunkId
)
{
	/*trunkId should be in range of (0,119]*/
	if((trunkId == 0)||(trunkId > NPD_TRUNK_NUMBER_MAX))
		return TRUNK_RETURN_CODE_BADPARAM;

	g_trunks[trunkId - 1] = NULL;

	return TRUNK_RETURN_CODE_ERR_NONE;
}
/**********************************************************************************
 *  npd_trunk_check_port_membership
 *
 *	DESCRIPTION:
 * 		check out if a port is a port member of specified TRUNK
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of trunk
 *		NPD_FALSE - if port is not a member of trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_check_port_membership
(
	unsigned short trunkId,
	unsigned int   eth_index
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	struct trunk_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}
	
	portList = trunkNode->portList;

	if(NULL == portList) {
		return NPD_FALSE;
	}
	else if(0 != portList->count) {
		syslog_ax_trunk_dbg("check port %d in trunk %d count %d\n",eth_index,trunkId, portList->count);
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_port_list_node_s,list);
			if(NULL != node && eth_index == node->eth_global_index) {
				syslog_ax_trunk_dbg("npd trunk check port found %#0x in trunk %d.\n",eth_index,trunkId);
				return NPD_TRUE;
			}
		}
	}

	return NPD_FALSE;
}

/**********************************************************************************
 *  npd_trunk_check_vlan_allowship
 *
 *	DESCRIPTION:
 * 		check out if a vlan is allow for specified TRUNK
 *
 *	INPUT:
 *		trunkId - trunk id
 *		vid - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if vlan is allow for trunk
 *		NPD_FALSE - if vlan is not allow for trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_check_vlan_allowship
(
	unsigned short trunkId,
	unsigned short vid
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_vlan_list_s *alVlanList = NULL;
	struct trunk_vlan_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}
	
	alVlanList = trunkNode->allowVlanList;

	if(NULL == alVlanList) {
		return NPD_FALSE;
	}
	else if(0 != alVlanList->count) {
		syslog_ax_trunk_dbg("npd_trunk:: check vlan %d in trunk %d count %d\n",vid,trunkId, alVlanList->count);
		list = &(alVlanList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_vlan_list_node_s,list);
			if(NULL != node && vid == node->vlanId) {
				syslog_ax_trunk_dbg("npd trunk check vlan found %d in trunk %d.\n",vid,trunkId);
				return NPD_TRUE;
			}
		}
	}

	return NPD_FALSE;
}


/**********************************************************************************
 *  npd_trunk_member_port_index_get_all
 *
 *	DESCRIPTION:
 * 		check out if a port is a port member of specified trunk
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *		mbr_count - trunk port member counts
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of trunk
 *		NPD_FALSE - if port is not a member of trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_member_port_index_get_all
(
	unsigned short	trunkId,
	unsigned int	eth_g_index[],
	unsigned int	*mbr_count
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	struct trunk_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	unsigned int mbrCnt = 0;
	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}

	portList = trunkNode->portList;

	if(NULL == portList) {
		/* for error on smu:[% Bad parameter,there exists no member in trunk 10] */
		*mbr_count = 0;
		return NPD_TRUE;
	}
	else if(0 != portList->count) {
		/*NPD_DEBUG(("npd trunk check port:: trunk %d port count %d",trunkId,portList->count));*/
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_port_list_node_s,list);
			if(NULL != node) {
				eth_g_index[mbrCnt] = node->eth_global_index;
				mbrCnt++;
			}
		}
	}
	if(mbrCnt >0){
		*mbr_count = mbrCnt;
		return NPD_TRUE;
	}
	return NPD_FALSE;
}

/**********************************************************************************
 *  npd_trunk_endis_member_port_index_get
 *
 *	DESCRIPTION:
 * 		check out if a port is aport enable/disable member of specified trunk
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *		enDis
 *		mbr_count - trunk port member counts
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of trunk
 *		NPD_FALSE - if port is not a member of trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_endis_member_port_index_get
(
	unsigned short	trunkId,
	unsigned int	eth_g_index[],
	unsigned char 	enDis,
	unsigned int	*mbr_count
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	struct trunk_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	unsigned int mbrCnt = 0;
	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}

	portList = trunkNode->portList;

	if(NULL == portList) {
		return NPD_FALSE;
	}
	else if(0 != portList->count) {
		syslog_ax_trunk_dbg("check trunk %d %s portBmp \n",trunkId, enDis?"En":"Dis");
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_port_list_node_s,list);
			if(NULL != node && enDis == node->enDisFlag) {
				syslog_ax_trunk_dbg("trunk %d port %d enDisFlag %d \n",trunkId, node->eth_global_index,node->enDisFlag);
				eth_g_index[mbrCnt] = node->eth_global_index;
				mbrCnt++;
			}
		}
	}
	if(mbrCnt >0){
		*mbr_count = mbrCnt;
		return NPD_TRUE;
	}
	return NPD_FALSE;
}

/**********************************************************************************
 *  npd_trunk_allow_vlan_id_get
 *
 *	DESCRIPTION:
 * 		check out if a vlan is allowed for specified trunk
 *
 *	INPUT:
 *		trunkId - vlan id
 *		vid - vlan id
 *		vlanCount - allowed vlan counts
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_allow_vlan_id_get
(
	unsigned short	trunkId,
	unsigned short	vid[],
	unsigned int	*v_count
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_vlan_list_s *allowVlanList = NULL;
	struct trunk_vlan_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	unsigned int mbrCnt = 0;
	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}

	allowVlanList = trunkNode->allowVlanList;

	if(NULL == allowVlanList) {
		return NPD_FALSE;
	}
	else if(0 != allowVlanList->count) {
		syslog_ax_trunk_dbg("npd trunk check vlan:: trunk %d vlan count %d\n",trunkId,allowVlanList->count);
		list = &(allowVlanList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_vlan_list_node_s,list);
			if(NULL != node){
				vid[mbrCnt] = node->vlanId;
				mbrCnt++;
			}
		}
	}
	if(mbrCnt>0){
		*v_count = mbrCnt;
		/*NPD_DEBUG(("npd_trunk_allow_vlan_id_get::return TRUE."));*/
		return NPD_TRUE;
	}
	return NPD_FALSE;
}


/**********************************************************************************
 *  npd_trunk_allow_vlan_tagmode_get
 *
 *	DESCRIPTION:
 * 		check out if a vlan is allowed for specified trunk
 *
 *	INPUT:
 *		trunkId - vlan id
 *		tag - tag/untag
 *		vlanCount - allowed vlan counts
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_allow_vlan_tagmode_get
(
	unsigned short	trunkId,
	unsigned char	tag[],
	unsigned int	*v_count
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_vlan_list_s *allowVlanList = NULL;
	struct trunk_vlan_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	unsigned int mbrCnt = 0;
	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}

	allowVlanList = trunkNode->allowVlanList;

	if(NULL == allowVlanList) {
		return NPD_FALSE;
	}
	else if(0 != allowVlanList->count) {
		syslog_ax_trunk_dbg("trunk %d vlan count %d\n",trunkId,allowVlanList->count);
		list = &(allowVlanList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_vlan_list_node_s,list);
			if(NULL != node){
				tag[mbrCnt] = node->uTagM;
				mbrCnt++;
			}
		}
	}
	if(mbrCnt>0){
		*v_count = mbrCnt;
		return NPD_TRUE;
	}
	return NPD_FALSE;
}

/**********************************************************************************
 *  npd_trunk_master_port_config
 *
 *	DESCRIPTION:
 * 		config a port tobe a master port member of specified TRUNK
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of trunk
 *		NPD_FALSE - if port is not a member of trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_master_port_config
(
	unsigned short trunkId,
	unsigned int   eth_index
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	struct trunk_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}
	
	portList = trunkNode->portList;

	if(NULL == portList) {
		return NPD_FALSE;
	}
	else if(0 != portList->count) {
		syslog_ax_trunk_dbg("check port %d in trunk %d count %d\n",eth_index,trunkId, portList->count);
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_port_list_node_s,list);			
			if(NULL != node ){
				if(NPD_TRUE == node->masterPortFlag &&
					eth_index != node->eth_global_index) {
					node->masterPortFlag = NPD_FALSE;
					syslog_ax_trunk_dbg("trunk find original master port %#0x in trunk %d.\n",eth_index,trunkId);
				}
				else if(/*NPD_TRUE != node->masterPortFlag &&*/
					eth_index == node->eth_global_index) {
					node->masterPortFlag = NPD_TRUE;
				}
			}
		}
		return NPD_TRUE;
	}

	return NPD_FALSE;
}


/**********************************************************************************
 *  npd_trunk_check_port_endis
 *
 *	DESCRIPTION:
 * 		check a port is enalbe or disable port member of specified TRUNK
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *		en_dis - enable or disable
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is an enable member of trunk
 *		NPD_FALSE - if port is a disable member of trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_check_port_endis
(
	unsigned short trunkId,
	unsigned int   eth_index,
	unsigned char  *en_dis
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	struct trunk_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}
	
	portList = trunkNode->portList;

	if(NULL == portList) {
		return NPD_FALSE;
	}
	else if(0 != portList->count) {
		syslog_ax_trunk_dbg("check port %d in trunk %d count %d\n",eth_index,trunkId, portList->count);
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_port_list_node_s,list);
			if(NULL != node && eth_index == node->eth_global_index) {
				*en_dis =node->enDisFlag ;
				syslog_ax_trunk_dbg("port %d trunk %d enDisFlag %d\n",eth_index,trunkId,node->enDisFlag);
				return NPD_TRUE;
			}
		}
	}
	return NPD_FALSE;
}

/**********************************************************************************
 *  npd_trunk_port_set_endis
 *
 *	DESCRIPTION:
 * 		config a port tobe enalbe or disable port member of specified TRUNK
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *		en_dis - enable or disable
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is an enable member of trunk
 *		NPD_FALSE - if port is a disable member of trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_port_set_endis
(
	unsigned short trunkId,
	unsigned int   eth_index,
	unsigned char  en_dis
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	struct trunk_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}
	
	portList = trunkNode->portList;

	if(NULL == portList) {
		return NPD_FALSE;
	}
	else if(0 != portList->count) {
		syslog_ax_trunk_dbg("check port %d in trunk %d count %d\n",eth_index,trunkId, portList->count);
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_port_list_node_s,list);
			if(NULL != node && eth_index == node->eth_global_index) {
				node->enDisFlag = en_dis;
				syslog_ax_trunk_dbg("port %d trunk %d enDisFlag %d\n",eth_index,trunkId, node->enDisFlag);
				if(NPD_SUCCESS != npd_modify_trunk_status_by_portdown(trunkId)){
					syslog_ax_trunk_err("trunkId %d error\n",trunkId);
				}				
				return NPD_TRUE;
			}
		}
	}
	return NPD_FALSE;
}

/***************************************************
 * npd_trunk_load_balanc_set
 *
 * params :
 *		trunkId
 *		loadBalancMode
 *
 ***************************************************/
 unsigned int npd_trunk_load_balanc_set(unsigned short trunkId,unsigned int lbMode)
 {
 	struct trunk_s* trunkNode = NULL;
	unsigned long ret = 0;

	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode ){
		return TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
	}

	if(lbMode == trunkNode->loadBanlcMode){
		return TRUNK_RETURN_CODE_LOAD_BANLC_CONFLIT;
	}

	trunkNode->loadBanlcMode = lbMode;

	g_dst_trunk[trunkId-1].loadBanlcMode = lbMode;

	ret = msync(g_dst_trunk, sizeof(dst_trunk_s)*127, MS_SYNC);
	if( ret!=0 )
    {
        syslog_ax_vlan_err("msync shm_trunk failed \n" ); 
		return TRUNK_RETURN_CODE_ERR_GENERAL;
    }		
	
	return TRUNK_RETURN_CODE_ERR_NONE;
 }
/**********************************************************************************
 *  npd_trunk_master_port_get
 *
 *	DESCRIPTION:
 * 		find out if a port is a master port member of specified TRUNK
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of trunk
 *		NPD_FALSE - if port is not a member of trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_master_port_get
(
	unsigned short trunkId,
	unsigned int   *eth_index
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	struct trunk_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}
	
	portList = trunkNode->portList;

	if(NULL == portList) {
		return NPD_FALSE;
	}
	else if(0 != portList->count) {
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_port_list_node_s,list);
			if( NULL != node &&	NPD_TRUE == node->masterPortFlag ){
				*eth_index = node->eth_global_index;
				syslog_ax_trunk_dbg("get master port %d in trunk %d.\n",node->eth_global_index,trunkId);
				return NPD_TRUE;
			}
		}
	}
	return NPD_FALSE;
}

/**********************************************************************************
 *  npd_trunk_status_get
 *
 *	DESCRIPTION:
 * 		get trunk status:state up or state down
 *
 *	INPUT:
 *		trunkId - trunk id		
 *	
 *	OUTPUT:
 *		status - trunk status
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_status_get
(
	IN unsigned int trunkId,
	OUT unsigned int *status
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;

	trunkNode = npd_find_trunk(trunkId);
	if(trunkNode){
		portList = trunkNode->portList;
		if(NULL != portList) {
			if(TRUNK_STATE_DOWN_E == trunkNode->tLinkState) {
				*status = TRUNK_STATE_DOWN_E;
			}
			else if(TRUNK_STATE_UP_E == trunkNode->tLinkState) {
				*status = TRUNK_STATE_UP_E;
			}
		}
	}
	else{
		syslog_ax_trunk_dbg("can not find trunk %d by trunkID\n",trunkId);
		return NPD_FAIL;
	}
	syslog_ax_trunk_dbg("get trunk %d status %d\n",trunkId,*status);
	return NPD_SUCCESS;
}


/**********************************************************************************
 *  npd_trunk_port_list_addto
 *
 *	DESCRIPTION:
 * 		add port to trunk port list
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of trunk
 *		NPD_FALSE - if port is not a member of trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_port_list_addto
(
	unsigned short trunkId,
	unsigned int   eth_g_index
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	struct trunk_port_list_node_s *node = NULL;
	unsigned int ret = 0,port_link_state = 0;

	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		syslog_ax_trunk_dbg("find trunk %d error!\n",trunkId);
		return NPD_FALSE;
	}
	
	portList = trunkNode->portList;
	
	if(NULL == portList) { /* create port list */
		syslog_ax_trunk_dbg("trunk %d add first port %#d.\n",trunkId,eth_g_index);
		portList = (trunk_port_list_s *)malloc(sizeof(trunk_port_list_s));
		if(NULL == portList) {
			syslog_ax_trunk_err("trunk %d add first port %#0x memory error\n",	\
					trunkId,eth_g_index);
			return NPD_FAIL;
		}
		else {
			memset(portList, 0, sizeof(trunk_port_list_s));
			INIT_LIST_HEAD(&(portList->list));
			trunkNode->portList = portList;
		}
	}
	node = (struct trunk_port_list_node_s *)malloc(sizeof(struct trunk_port_list_node_s));
	if(NULL == node) {
		syslog_ax_trunk_err("trunk %d add port %#0x to list error,null memory allocation\n", \
					trunkId, eth_g_index);
		return NPD_FALSE;
	}
	memset(node,0,sizeof(struct trunk_port_list_node_s));
	node->eth_global_index = eth_g_index;
	list_add(&(node->list),&(portList->list));
	portList->count++;
	
	ret = npd_get_port_link_status(eth_g_index,&port_link_state);
	if(NPD_SUCCESS != ret) {
		return NPD_FALSE;
	}
	else {
		if(ETH_ATTR_LINKUP == port_link_state) {
			node->enDisFlag = NPD_TRUE;
			if(TRUNK_STATE_DOWN_E == trunkNode->tLinkState) {
				trunkNode->tLinkState = TRUNK_STATE_UP_E;
			}
		}
		else {
			node->enDisFlag = NPD_FALSE;
			if(TRUNK_STATE_DOWN_E == trunkNode->tLinkState) {
				trunkNode->tLinkState = TRUNK_STATE_DOWN_E;
			}
		}
	}
	/*zhangcl@autelan.com 2012-09-15 do not need master port any more*/
	#if 0
	if(1 == portList->count){
		node->masterPortFlag = NPD_TRUE;
	}
	else 
		node->masterPortFlag = NPD_FALSE;
	#endif
	syslog_ax_trunk_dbg("trunk %d current port node count %d.\n",trunkId ,portList->count);

	return NPD_TRUE;
}

/**********************************************************************************
 *  npd_trunk_port_list_delfrom
 *
 *	DESCRIPTION:
 * 		delete a port from trunk port list
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port list
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of trunk
 *		NPD_FALSE - if port is not a member of trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_port_list_delfrom
(
	unsigned short trunkId,
	unsigned int   eth_g_index
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	struct trunk_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	unsigned int delflag = 0;
	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}
	
	portList = trunkNode->portList;
	
	if(NULL == portList) {
		 syslog_ax_trunk_err("delete port %#0x from trunk %d null port list err!\n",	\
						eth_g_index, trunkId);
		return NPD_FALSE;
	}
	else {	
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_port_list_node_s,list);
			if(NULL != node && eth_g_index == node->eth_global_index) {
				list_del(&(node->list));
				delflag = 1;
				break;
			}
		}
		if(NULL != node && 1 == delflag){
			free(node);
			node = NULL;
			if(portList->count > 0) {
				portList->count--;
			}
		}
		 syslog_ax_trunk_dbg("npd trunk %d current port node count %d\n",	\
						trunkId,  portList->count);
		if(0 == portList->count) {
			free(portList);
			portList = NULL;
			trunkNode->portList = NULL;
		}
	}
	
	return NPD_TRUE;
}


/**********************************************************************************
 *  npd_trunk_vlan_list_addto
 *
 *	DESCRIPTION:
 * 		add vlan to trunk's vlan list
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if vlan is allowed for trunk
 *		NPD_FALSE - if vlan is not allowed for trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_vlan_list_addto
(
	unsigned short trunkId,
	unsigned short   vid,
	unsigned char  tagMode
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_vlan_list_s *allowVlanList = NULL;
	struct trunk_vlan_list_node_s *node = NULL;

	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		syslog_ax_trunk_dbg("find trunk %d Error!\n",trunkId);
		return NPD_FALSE;
	}
	
	allowVlanList = trunkNode->allowVlanList;
	
	if(NULL == allowVlanList) { /* create port list */
		/*NPD_DEBUG(("npd trunk %d add first vlan %d",trunkId,vid));*/
		allowVlanList = (trunk_vlan_list_s *)malloc(sizeof(trunk_vlan_list_s));
		if(NULL == allowVlanList) {
			syslog_ax_trunk_dbg("npd trunk %d add first vlan %d memory err\n",\
														trunkId,vid);
			return NPD_FAIL;
		}
		else {
			memset(allowVlanList, 0, sizeof(trunk_port_list_s));
			INIT_LIST_HEAD(&(allowVlanList->list));
			trunkNode->allowVlanList = allowVlanList;
		}
	}

	node = (struct trunk_vlan_list_node_s *)malloc(sizeof(struct trunk_vlan_list_node_s));
	if(NULL == node) {
		syslog_ax_trunk_err("npd trunk %d add vlan %d to list error:null memory alloc\n", \
					trunkId, vid);
		return NPD_FALSE;
	}
	memset(node,0,sizeof(struct trunk_vlan_list_node_s));
	node->vlanId = vid;
	node->uTagM = tagMode;
	list_add(&(node->list),&(allowVlanList->list));
	allowVlanList->count++;

	/*NPD_DEBUG(("npd trunk %d current vlan node count %d",trunkId ,allowVlanList->count));*/

	return NPD_TRUE;
}


/**********************************************************************************
 *  npd_trunk_vlan_list_delfrom
 *
 *	DESCRIPTION:
 * 		delete a vlan from trunk's vlan list
 *
 *	INPUT:
 *		trunkId - trunk id
 *		vid - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if vlan is allow for trunk
 *		NPD_FALSE - if port is not allow for trunk
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_vlan_list_delfrom
(
	unsigned short trunkId,
	unsigned short vid,
	unsigned char tagMode
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_vlan_list_s *allowVlanList = NULL;
	struct trunk_vlan_list_node_s *node = NULL;
	struct list_head  *list = NULL, *pos = NULL;
	unsigned int delflag = 0;

	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		return NPD_FALSE;
	}
	
	allowVlanList = trunkNode->allowVlanList;
	if(NULL == allowVlanList) {
		syslog_ax_trunk_err("npd del vlan %d from trunk %d null vlan list err!\n",\
						vid, trunkId);
		return NPD_FALSE;
	}
	else {	
		list = &(allowVlanList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct trunk_vlan_list_node_s,list);
			if(NULL != node && vid == node->vlanId ) {
				if(tagMode != node->uTagM){
					return TRUNK_RETURN_CODE_VLAN_TAGMODE_ERR;
				}
				list_del(&(node->list));
				delflag = 1;
				break;
			}
		}
		if(NULL != node && 1 == delflag){
			free(node);
			node = NULL;
			if(allowVlanList->count > 0) {
				allowVlanList->count--;
			}
		}
		syslog_ax_trunk_dbg("npd trunk %d current vlan node count %d\n",	\
						trunkId,  allowVlanList->count);
		if(0 == allowVlanList->count) {
			if(NULL != allowVlanList){
				free(allowVlanList);
				allowVlanList = NULL;
				trunkNode->allowVlanList = NULL;
			}
		}
	}
	
	return NPD_TRUE;
}


/**********************************************************************************
 *
 * change trunk status down if all ports down in the trunk
 *
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS
 *		NPD_FAIL
 *		
 **********************************************************************************/
 
int npd_modify_trunk_status_by_portdown
(
	unsigned short trunkId
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	struct trunk_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	int mbrCount = 0;
	unsigned int isLast = NPD_FALSE;

	trunkNode = npd_find_trunk(trunkId);
	if(trunkNode){
		/*check ports*/
		portList = trunkNode->portList;
		if(NULL != portList) {
			syslog_ax_trunk_dbg("trunk port into trunk portList count is %d\n",portList->count);
			list = &(portList->list);
			__list_for_each(pos,list) {
				node = list_entry(pos,struct trunk_port_list_node_s,list);
				if(node) {
					mbrCount++;
					if(ETH_ATTR_LINKUP == npd_check_eth_port_status(node->eth_global_index) 
						&& NPD_TRUE == node->enDisFlag){
						isLast = NPD_TRUE;
						break;						
					}
				}
			}
		}
		if(!isLast){
			trunkNode->tLinkState = TRUNK_STATE_DOWN_E;
			syslog_ax_trunk_dbg("trunk all port members are down,trunk %d is down\n",trunkId);
		}
		else
			trunkNode->tLinkState = TRUNK_STATE_UP_E;
		
		if(TRUNK_STATE_UP_E == trunkNode->tLinkState){
		    npd_static_arp_validate_by_trunk(trunkId);
		}
		else if(TRUNK_STATE_DOWN_E == trunkNode->tLinkState){
			npd_static_arp_invalidate_by_trunk(trunkId);
		}
	}
	else{
		syslog_ax_trunk_dbg("can not find trunk %d by trunkID\n",trunkId);
		return NPD_FAIL;
	}
	return NPD_SUCCESS;
}

/**********************************************************************************
 *
 * change trunk port memebers and fdb entry by port link status
 *
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - port global index
 *		event - port link status
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS
 *		NPD_FAIL
 *		
 *
 **********************************************************************************/

int npd_trunk_port_link_change
(
	unsigned int eth_g_index,
	struct port_based_trunk_s *portTrunk,
	enum ETH_PORT_NOTIFIER_ENT event
)
{
	struct trunk_s* node = NULL;
	unsigned short trkid = portTrunk->trunkId;
	unsigned int ret = 0;
	unsigned char tlinkstate = TRUNK_STATE_DOWN_E;
	
	if(ETH_PORT_NOTIFIER_LINKDOWN_E == event){
		/* TODO:check trunk status*/
		syslog_ax_trunk_dbg("port %d down, trunkId is %d\n",eth_g_index,trkid);
		if(NPD_SUCCESS != npd_modify_trunk_status_by_portdown(trkid)){
			syslog_ax_trunk_err("trunkId %d error\n",trkid);
			/*return NPD_FAIL;*/
		}
		/* TODO:disable the port member trunk*/
		if(TRUNK_RETURN_CODE_ERR_NONE != npd_trunk_port_endis(trkid,eth_g_index, NPD_FALSE)){
			syslog_ax_trunk_err("set trunk port disable error, port_index %d\n",eth_g_index);
		}
		#if 0
		if(nam_fdb_table_delete_entry_with_port(eth_g_index)){
			syslog_ax_trunk_err("delete fdb table entry error, port_index %d\n",eth_g_index);
			/*return NPD_FAIL;*/
		}
		#endif
		node = npd_find_trunk(trkid);
		if((NULL != node)&&(TRUNK_STATE_DOWN_E == node->tLinkState)){
			tlinkstate = TRUNK_STATE_DOWN_E;
			npd_syslog_dbg("clear fdb trunk %d\n",trkid);
			if(nam_fdb_table_delete_entry_with_trunk(trkid)){
				syslog_ax_trunk_err("delete fdb table entry error, trunk %d\n",trkid);
				/*return NPD_FAIL;*/
			}
		}
	}
	else if(ETH_PORT_NOTIFIER_LINKUP_E == event){
		/* TODO:check trunk status*/
		syslog_ax_trunk_dbg("eth-port %d up, trunkId is %d\n",eth_g_index,trkid);
		node = npd_find_trunk(trkid);
		if(node){
			if(TRUNK_STATE_DOWN_E == node->tLinkState){	
				node->tLinkState = TRUNK_STATE_UP_E;
				syslog_ax_trunk_dbg("port %d id up,trunk %d is up\n",eth_g_index,trkid);
			}
		}
		tlinkstate = TRUNK_STATE_UP_E;
		/* TODO:enable the port member trunk*/
		if(TRUNK_RETURN_CODE_ERR_NONE != npd_trunk_port_endis(trkid,eth_g_index, NPD_TRUE)){
			syslog_ax_trunk_err("set trunk port enable error, port_index %d\n",eth_g_index);
		}
	}

	if(g_dst_trunk[trkid-1].tLinkState != tlinkstate)
	{
		g_dst_trunk[trkid-1].tLinkState = tlinkstate;
		ret = msync(g_dst_trunk, sizeof(dst_trunk_s)*127, MS_SYNC);
		if( ret!=0 )
	    {
	        syslog_ax_trunk_err("msync shm_trunk failed \n" ); 
			return TRUNK_RETURN_CODE_ERR_GENERAL;
	    }
	}
	
	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_trunk_port_member_add
 *
 *	DESCRIPTION:
 * 		add port to trunk .
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUNK_ERR_NONE  - if no error occurred
 *		NPD_TRUNK_PORT_EXISTS - if port is already a member of trunk
 *		...
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_port_member_add
(
	unsigned short trunkId,
	unsigned int   eth_index
)
{
	unsigned int retval = TRUNK_RETURN_CODE_ERR_NONE;

	if(NPD_TRUE == npd_trunk_check_port_membership(trunkId, eth_index))
	{	
		retval = TRUNK_RETURN_CODE_PORT_EXISTS;
	}
	else {
		/*add port to trunk_port_list*/
		retval = npd_trunk_port_list_addto(trunkId, eth_index);
		if(NPD_TRUE == retval) {
			retval = TRUNK_RETURN_CODE_ERR_NONE;
		}
		else 
			retval = TRUNK_RETURN_CODE_ERR_GENERAL;

		/*lable the eth_port a trunkId tag.*/
		retval = npd_eth_port_set_ptrunkid(eth_index,trunkId);
		if(NPD_SUCCESS != retval){
			retval = TRUNK_RETURN_CODE_SET_TRUNKID_ERR;
		}
		else {
			retval = TRUNK_RETURN_CODE_ERR_NONE;
		}
	}
	return retval;
}
/**********************************************************************************
 *  npd_trunk_port_member_del
 *
 *	DESCRIPTION:
 * 		delete port from trunk .
 *
 *	INPUT:
 *		trunkId - trunk id
 *		eth_g_index - global ethernet port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUNK_ERR_NONE  - if no error occurred
 *		NPD_TRUNK_PORT_NOTEXISTS - if port is not a member of trunk
 *		...
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_port_member_del
(
	unsigned short trunkId,
	unsigned int   eth_index
)
{
	int retval = TRUNK_RETURN_CODE_ERR_NONE;

	if(NPD_FALSE == npd_trunk_check_port_membership(trunkId, eth_index)){
		retval = TRUNK_RETURN_CODE_PORT_NOTEXISTS;
	}
	else {
		npd_trunk_port_list_delfrom(trunkId, eth_index);
		retval = npd_eth_port_clear_ptrunkid(eth_index);
		if(NPD_SUCCESS != retval){
			syslog_ax_trunk_dbg("clear port %d pTrunkId error,return %d.\n",eth_index,retval);
		}
		syslog_ax_trunk_dbg("clear port %d pTrunkId ok,return %d.\n",eth_index,retval);
		if(NPD_SUCCESS != npd_modify_trunk_status_by_portdown(trunkId)){
			syslog_ax_trunk_err("trunkId %d error\n",trunkId);
		}				
	}
	return retval;
}
unsigned int npd_trunk_hold_member_bmp
(
	unsigned short trunkId,
	unsigned int lastMbrBmp0,
	unsigned int lastMbrBmp1
)
{
	unsigned int ret = 0;
	struct trunk_s	*trunkNode = NULL;

	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
		return ret;
	}
	trunkNode->portsMbrBmp.mbrBmp0 = lastMbrBmp0;
	trunkNode->portsMbrBmp.mbrBmp1 = lastMbrBmp1;
	return TRUNK_RETURN_CODE_ERR_NONE;
	
}
/**********************************************************************************
 *
 * create trunk by TRUNK ID on both SW and HW side
 *
 *
 *	INPUT:
 *		trunk ID - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUNK_ERR_GENERAL - if error occurred when create trunk in SW side 
 *		NPD_TRUNK_ERR_HW - if error occurred when create trunk in HW side
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_activate
(
	unsigned short trunkId,
	unsigned char* name
)
{
	unsigned int	ret = TRUNK_RETURN_CODE_ERR_NONE;
	struct trunk_s	*trunkNode = NULL ;
	unsigned int    trkLinkStat = TRUNK_STATE_DOWN_E;
	if(NULL == name){
		return TRUNK_RETURN_CODE_ERR_GENERAL;
	}
	ret = npd_check_trunk_exist(trunkId);
	if(ret == TRUNK_RETURN_CODE_TRUNK_EXISTS) {
		/* syslog_ax_trunk_dbg(("Npd_trunk:: trunk %d exist",trunkId));*/
		trunkNode = npd_find_trunk(trunkId);
		if (NULL == trunkNode) { 
			 syslog_ax_trunk_dbg("npd_trunk_find error.\n");
			ret = TRUNK_RETURN_CODE_ERR_GENERAL; 
		}
		else {
			ret = TRUNK_RETURN_CODE_TRUNK_EXISTS;
		}
		return ret;   /*NPD return HERE!!! steps into next CMD node*/
	}
	else {
		trunkNode = npd_find_trunk_by_name((char*)name);
		if(NULL != trunkNode) {
			 syslog_ax_trunk_dbg("trunk name conflict.\n");
			return TRUNK_RETURN_CODE_NAME_CONFLICT;
		}
		/* syslog_ax_trunk_dbg(("Npd_trunk::config trunk NOT exist.\n"));*/
		trunkNode = npd_create_trunk_by_name((char*)name);/*create trunk on Sw.*/
		if (NULL == trunkNode) { 
			 syslog_ax_trunk_dbg("trunk create error.\n");
			return TRUNK_RETURN_CODE_ERR_GENERAL;  
		}
		else 
		{
			/*create trunk on SW ok.*/
			trunkNode->trunkId= trunkId;
			trunkNode->loadBanlcMode = g_loadBalanc;
			trunkNode->tLinkState = trkLinkStat;
			ret = npd_trunk_fillin_global(trunkId,trunkNode);
			if(TRUNK_RETURN_CODE_ERR_NONE != ret) {
				free(trunkNode);
				return TRUNK_RETURN_CODE_ERR_GENERAL;
			}
			ret = npd_vlan_trunk_entity_add(1,trunkId,NPD_FALSE);
			if (ret != VLAN_RETURN_CODE_ERR_NONE) {
				 syslog_ax_trunk_dbg("trunk %d add to default vlan 1 error ret %d \n",trunkId,ret);
				return TRUNK_RETURN_CODE_ERR_GENERAL;
			}
			syslog_ax_trunk_dbg("trunkNode %p,trunknode->ports %d\n",trunkNode,trunkNode->portList);
			ret = nam_asic_trunk_entry_active(trunkId);
			if (ret != 0) {
				 syslog_ax_trunk_dbg("create trunk %d hardware error ret %d \n",trunkId,ret);
				npd_delete_trunk(trunkId);
				ret = TRUNK_RETURN_CODE_ERR_HW;
				return ret;
			}
		}
	}
	return TRUNK_RETURN_CODE_ERR_NONE;
}
unsigned int npd_trunk_port_del_for_dynamic_trunk
(
	unsigned short	trunkId,
	unsigned int	eth_index
)
{
	unsigned int i = 0, ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned char devNum = 0, virPortNum = 0, isTagged = 0;
	unsigned int cretMbrBmp0 = 0, cretMbrBmp1 = 0;
	static unsigned short vid[NPD_MAX_VLAN_ID] = {0};
	unsigned int m_eth_index = 0, vCount = 0;
	
	ret = npd_get_devport_by_global_index(eth_index,&devNum,&virPortNum);
	if (ret != 0) 
	{
		 syslog_ax_trunk_err("global_index %d convert to devPort error ret %d\n",eth_index,ret);
		ret = TRUNK_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)){
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			 syslog_ax_trunk_dbg("Legal devNum %d,virPortNum %d\n",devNum,virPortNum);
			ret = TRUNK_RETURN_CODE_ERR_NONE;
		} 
		else {
			 syslog_ax_trunk_err("virPortNum error.\n");
			ret = TRUNK_RETURN_CODE_BADPARAM;
			return ret;
		}
	} 
	else{
		 syslog_ax_trunk_err("devNum error.\n");
		ret = TRUNK_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*if NOT Steps into the (trunk-config CMD node),trunk exists or not Test is wanted.*/
	/*if Steps into (trunk-config CMD node), it does NOT need test trunk node exist or not,because*/
	ret = npd_check_trunk_exist(trunkId);
	if (TRUNK_RETURN_CODE_TRUNK_EXISTS != ret) {
		 syslog_ax_trunk_err("trunk %d not exist.\n",trunkId);
		return TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;/*can NOT directed Return.*/
	}
	
	/*here MUST check the ETH-port is already member of the TRUNK or NOT*/
	ret = npd_trunk_check_port_membership(trunkId,eth_index);
	if (NPD_FALSE == ret) {
		 syslog_ax_trunk_err("Port %d was not the member of trunk % d\n",virPortNum,trunkId);
		return TRUNK_RETURN_CODE_PORT_NOTEXISTS;
	}

	/*check the port is master port or NOT of the trunk*/
	#if 0
	ret = npd_trunk_master_port_get(trunkId, &m_eth_index);
	if((NPD_TRUE == ret) && (m_eth_index == eth_index)){
		return TRUNK_RETURN_CODE_DEL_MASTER_PORT;
	}
	#endif
	
	if (TRUNK_CONFIG_SUCCESS == nam_asic_trunk_get_port_member_bmp(trunkId, &cretMbrBmp0, &cretMbrBmp1)){
		npd_trunk_hold_member_bmp(trunkId,cretMbrBmp0,cretMbrBmp1);
	}
	ret = npd_trunk_port_member_del(trunkId,eth_index);
	if(TRUNK_RETURN_CODE_ERR_NONE != ret) {
		 syslog_ax_trunk_err("delete port %d from trunk %d software error ret %d\n",eth_index,trunkId,ret);
		return TRUNK_RETURN_CODE_ERR_GENERAL;
	}
	ret = nam_asic_trunk_ports_del(devNum, trunkId,virPortNum);
	if (0 != ret) {
		 syslog_ax_trunk_err("delete port %d from trunk %d hardware error ret %d\n",\
																	virPortNum,trunkId,ret);
		ret = TRUNK_RETURN_CODE_ERR_HW;
	}
	
	/*deleting port from trunk,it'll delete port from vlans that the trunk allowed!
	 *At the same time, add the port to Default vlan 1.*/
	ret = npd_trunk_allow_vlan_id_get(trunkId,vid,&vCount);
	if(NPD_TRUE == ret ){
		for(i=0;i<vCount;i++){
			if(NPD_TRUE == npd_vlan_check_contain_port(vid[i],eth_index, &isTagged)){
				if(!((DEFAULT_VLAN_ID == vid[i])&&(FALSE == isTagged))){
					ret = npd_vlan_interface_port_del_for_dynamic_trunk(vid[i],eth_index, isTagged);
					syslog_ax_trunk_dbg("delete port %d from vlan %d ret %d.\n",\
																			eth_index,vid[i],ret);
					if(VLAN_RETURN_CODE_PORT_NOTEXISTS != ret && VLAN_RETURN_CODE_ERR_NONE != ret){
						ret = TRUNK_RETURN_CODE_MEMBER_DEL_ERR;
						return ret;
					}
				}
			}
		}
		/*syslog_ax_trunk_dbg("npd_trunk_port_add_del::return %d.",ret);*/
		ret = TRUNK_RETURN_CODE_ERR_NONE;
	}

	/*return to Default vlan 1.*/
	npd_vlan_interface_port_add_for_dynamic_trunk(DEFAULT_VLAN_ID, eth_index,NPD_FALSE);syslog_ax_trunk_dbg("add port to default vlan \n");
	return ret;
}

unsigned int npd_trunk_port_add_for_dynamic_trunk
(
	unsigned short trunkId, 
	unsigned int eth_index
)
{
	unsigned int i = 0,j = 0,ret = 0;
	unsigned char devNum = 0, virPortNum = 0, trunkTagMode = NPD_FALSE;
	unsigned int cretMbrBmp0 = 0, cretMbrBmp1 = 0, ifIndex = 0;
	static unsigned short vid[NPD_MAX_VLAN_ID] = {0};
	unsigned int vCount = 0;
	unsigned char enDis = 0;
	unsigned int port_link_state = 0;
    unsigned int slot_index = 0, slot_no = 0;
    unsigned int port_index = 0, local_port_no = 0;
	unsigned int  fdblimit = 0;
	
	syslog_ax_trunk_dbg("trunkId %d,global_index %d\n",trunkId,eth_index);
	ret = npd_get_devport_by_global_index(eth_index,&devNum,&virPortNum);
	if (ret != 0) 
	{
		syslog_ax_trunk_err("global_index %d convert to devPort error.\n",eth_index);
		return TRUNK_RETURN_CODE_BADPARAM;
	}
		slot_index= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_index);
		slot_no= CHASSIS_SLOT_INDEX2NO(slot_index);
		port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_index);
		local_port_no= ETH_LOCAL_INDEX2NO(slot_index,port_index);
	/*test devNum virPortNum is legal or not*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = TRUNK_RETURN_CODE_ERR_NONE;
		} 
		else {
			ret = TRUNK_RETURN_CODE_BADPARAM;
			return ret;
		}
	}
	else
	{
		ret = TRUNK_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*if NOT Steps into the (trunk-config CMD node),trunk exists or not Test is wanted.*/
	/*if Steps into (trunk-config CMD node), it does NOT need test trunk node exist or not.*/
	ret = npd_check_trunk_exist(trunkId);
	if (TRUNK_RETURN_CODE_TRUNK_EXISTS != ret) {
		syslog_ax_trunk_err("trunk %d not exist.\n",trunkId);
		return TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;/*can NOT directed Return NPD_TRUNK_BADPARAM.*/
	}
	
	/*here MUST check the ETH-port has already member of this TRUNK or not*/
	ret = npd_trunk_check_port_membership(trunkId,eth_index);
	if (NPD_TRUE == ret) {
		syslog_ax_trunk_err("port %d has already been the member of trunk % d\n",virPortNum,trunkId);
		return TRUNK_RETURN_CODE_PORT_EXISTS;
	}
	
	/*here MUST check the ETH-port has already member of other TRUNK or not*/
	for(i = 0; i <= NPD_MAX_TRUNK_ID; i++) {
		if(TRUNK_RETURN_CODE_TRUNK_EXISTS == npd_check_trunk_exist(i)) {
			syslog_ax_trunk_dbg("trunk %d valid.\n",i);
			if(NPD_TRUE == npd_trunk_check_port_membership(i,eth_index)) {
				syslog_ax_trunk_err("port %d has already been the member of trunk % d\n",virPortNum,i);
				return TRUNK_RETURN_CODE_MEMBERSHIP_CONFICT;
			}
		}
	}

	/*here must chect the port which add to trunk whether have the same configuration as master port*/
	ret = npd_eth_port_configuration_check(eth_index,trunkId);
	if(NPD_TRUE == ret) {
		syslog_ax_trunk_err("port %d has not the same configuration as master port in trunk %d\n",virPortNum,trunkId);
		return TRUNK_RETURN_CODE_PORT_CONFIG_DIFFER;
	}
	if(MAX_STATIC_TRUNK_ID >= trunkId){
		ret = npd_eth_port_interface_check(eth_index,&ifIndex);
		if((NPD_TRUE == ret) && (ifIndex != ~0UI)) {
			return TRUNK_RETURN_CODE_PORT_L3_INTFG;
		}
	}
	{
				
         /*judge port fdb protect exit*/
		if(npd_fdb_number_port_set_check(slot_no,local_port_no,&fdblimit)){
        		/*port add in trunk  port fdb protect delete */
			if(fdblimit>0) {
				npd_fdb_set_port_number(slot_no,local_port_no,0);
			} 
		}
		/*add port to trunk_port_list and set port trunkId*/
		ret = npd_trunk_port_member_add(trunkId, eth_index);
		if(TRUNK_RETURN_CODE_ERR_NONE != ret) { 
			return ret; 
		}
		/*trunk member port escape from vlans --vlan range(2-4094)*/
		npd_port_vlan_free(eth_index);

		/*add trunk port to default vlan 1 as untagged port member */
		ret = npd_vlan_interface_port_add_for_dynamic_trunk(DEFAULT_VLAN_ID, eth_index, NPD_FALSE);
		if(VLAN_RETURN_CODE_ERR_NONE != ret && VLAN_RETURN_CODE_PORT_EXISTS != ret && NPD_TRUE != ret) /*NPD_TRUE -- vlan is L3 interface*/
		{	
			syslog_ax_trunk_err("port %d add to vlan 1 error. ret %d\n",eth_index,ret);

			/*delete port from portlist if add error*/
			npd_trunk_port_member_del(trunkId,eth_index);
			ret = TRUNK_RETURN_CODE_MEMBER_ADD_ERR;
			return ret ;
		}

		/* check the trunk has already allowed vlans,and add new trunk member port to be a member of
		* all these vlans.
		*/
		ret = npd_trunk_allow_vlan_id_get(trunkId,vid,&vCount);
		if(NPD_TRUE == ret ) {
			for(i=0;i<vCount;i++) {
				npd_vlan_check_contain_trunk(vid[i],trunkId,&trunkTagMode);
				syslog_ax_trunk_dbg("get vlan count %d!\n",vCount);
				syslog_ax_trunk_dbg("trunk %d add to vlan %d tag mode %d\n",trunkId,vid[i],trunkTagMode);
				ret = npd_vlan_interface_port_add_for_dynamic_trunk(vid[i],eth_index, trunkTagMode);
				if(VLAN_RETURN_CODE_PORT_EXISTS != ret && VLAN_RETURN_CODE_ERR_NONE != ret && NPD_TRUE != ret) /*NPD_TRUE --vlan L3 interface*/
				{
					syslog_ax_trunk_err("port %d add to vlan %d error,ret %d.\n",eth_index,vid[i],ret);
					return TRUNK_RETURN_CODE_MEMBER_ADD_ERR;
				}
			}
		}
		
		ret = npd_get_port_link_status(eth_index,&port_link_state);
		if(NPD_SUCCESS != ret) {
			return NPD_FALSE;
		}
		else {
			if(ETH_ATTR_LINKUP == port_link_state) {
				enDis = NPD_TRUE;
			}
			else {
				enDis = NPD_FALSE;	
			}
		}		
		ret = nam_asic_trunk_ports_add(devNum, trunkId, virPortNum,enDis);
		if (TRUNK_PORT_EXISTS_GTDB == ret || TRUNK_CONFIG_SUCCESS == ret) {
			if (TRUNK_CONFIG_SUCCESS == nam_asic_trunk_get_port_member_bmp(trunkId, &cretMbrBmp0, &cretMbrBmp1)){
				npd_trunk_hold_member_bmp(trunkId,cretMbrBmp0,cretMbrBmp1);		
				syslog_ax_trunk_dbg("trunk %d 's portMbrBmp0 = %0x,portMbrBmp1 = %0x\n",trunkId,\
										cretMbrBmp0,		cretMbrBmp1);
				ret = TRUNK_RETURN_CODE_ERR_NONE;
			}
		}
		else if (TRUNK_PORT_MBRS_FULL == ret) {
			/*delete port from trunk portlist,nam Hw trunkDB,allowed vlan portlist*/
			npd_trunk_port_del_for_dynamic_trunk(trunkId,eth_index);
			ret = TRUNK_RETURN_CODE_PORT_MBRS_FULL;
			return ret;
		}
		else {	
			/*delete port from trunk portlist,nam Hw trunkDB,allowed vlan portlist*/
			npd_trunk_port_del_for_dynamic_trunk(trunkId, eth_index);
			ret = TRUNK_RETURN_CODE_ERR_HW; 
		}
	}
	return ret;
}

unsigned int npd_trunk_port_add
(
	unsigned short trunkId, 
	unsigned int eth_index
)
{
	unsigned int i = 0,j = 0,ret = 0;
	unsigned char devNum = 0, virPortNum = 0, trunkTagMode = NPD_FALSE;
	unsigned int cretMbrBmp0 = 0, cretMbrBmp1 = 0, ifIndex = 0;
	static unsigned short vid[NPD_MAX_VLAN_ID] = {0};
	unsigned int vCount = 0;
	unsigned char enDis = 0;
	unsigned int port_link_state = 0;
    unsigned int slot_index = 0, slot_no = 0;
    unsigned int port_index = 0, local_port_no = 0;
	unsigned int  fdblimit = 0;
	
	syslog_ax_trunk_dbg("trunkId %d,global_index %d\n",trunkId,eth_index);
	ret = npd_get_devport_by_global_index(eth_index,&devNum,&virPortNum);
	if (ret != 0) 
	{
		syslog_ax_trunk_err("global_index %d convert to devPort error.\n",eth_index);
		return TRUNK_RETURN_CODE_BADPARAM;
	}
		slot_index= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_index);
		slot_no= CHASSIS_SLOT_INDEX2NO(slot_index);
		port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_index);
		local_port_no= ETH_LOCAL_INDEX2NO(slot_index,port_index);
	/*test devNum virPortNum is legal or not*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = TRUNK_RETURN_CODE_ERR_NONE;
		} 
		else {
			ret = TRUNK_RETURN_CODE_BADPARAM;
			return ret;
		}
	}
	else
	{
		ret = TRUNK_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*if NOT Steps into the (trunk-config CMD node),trunk exists or not Test is wanted.*/
	/*if Steps into (trunk-config CMD node), it does NOT need test trunk node exist or not.*/
	ret = npd_check_trunk_exist(trunkId);
	if (TRUNK_RETURN_CODE_TRUNK_EXISTS != ret) {
		syslog_ax_trunk_err("trunk %d not exist.\n",trunkId);
		return TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;/*can NOT directed Return NPD_TRUNK_BADPARAM.*/
	}
	
	/*here MUST check the ETH-port has already member of this TRUNK or not*/
	ret = npd_trunk_check_port_membership(trunkId,eth_index);
	if (NPD_TRUE == ret) {
		syslog_ax_trunk_err("port %d has already been the member of trunk % d\n",virPortNum,trunkId);
		return TRUNK_RETURN_CODE_PORT_EXISTS;
	}
	
	/*here MUST check the ETH-port has already member of other TRUNK or not*/
	for(i = 0; i <= NPD_MAX_TRUNK_ID; i++) {
		if(TRUNK_RETURN_CODE_TRUNK_EXISTS == npd_check_trunk_exist(i)) {
			syslog_ax_trunk_dbg("trunk %d valid.\n",i);
			if(NPD_TRUE == npd_trunk_check_port_membership(i,eth_index)) {
				syslog_ax_trunk_err("port %d has already been the member of trunk % d\n",virPortNum,i);
				return TRUNK_RETURN_CODE_MEMBERSHIP_CONFICT;
			}
		}
	}

	/*here must chect the port which add to trunk whether have the same configuration as master port*/
	ret = npd_eth_port_configuration_check(eth_index,trunkId);
	if(NPD_TRUE == ret) {
		syslog_ax_trunk_err("port %d has not the same configuration as master port in trunk %d\n",virPortNum,trunkId);
		return TRUNK_RETURN_CODE_PORT_CONFIG_DIFFER;
	}
	if(MAX_STATIC_TRUNK_ID >= trunkId){
		ret = npd_eth_port_interface_check(eth_index,&ifIndex);
		if((NPD_TRUE == ret) && (ifIndex != ~0UI)) {
			return TRUNK_RETURN_CODE_PORT_L3_INTFG;
		}
	}
	{
				
         /*judge port fdb protect exit*/
		if(npd_fdb_number_port_set_check(slot_no,local_port_no,&fdblimit)){
        		/*port add in trunk  port fdb protect delete */
			if(fdblimit>0) {
				npd_fdb_set_port_number(slot_no,local_port_no,0);
			} 
		}
		/*add port to trunk_port_list and set port trunkId*/
		ret = npd_trunk_port_member_add(trunkId, eth_index);
		if(TRUNK_RETURN_CODE_ERR_NONE != ret) { 
			return ret; 
		}
		/*trunk member port escape from vlans --vlan range(2-4094)*/
		npd_port_vlan_free(eth_index);

		/*add trunk port to default vlan 1 as untagged port member */
		ret = npd_vlan_interface_port_add(DEFAULT_VLAN_ID, eth_index, NPD_FALSE);
		if(VLAN_RETURN_CODE_ERR_NONE != ret && VLAN_RETURN_CODE_PORT_EXISTS != ret && NPD_TRUE != ret) /*NPD_TRUE -- vlan is L3 interface*/
		{	
			syslog_ax_trunk_err("port %d add to vlan 1 error. ret %d\n",eth_index,ret);

			/*delete port from portlist if add error*/
			npd_trunk_port_member_del(trunkId,eth_index);
			ret = TRUNK_RETURN_CODE_MEMBER_ADD_ERR;
			return ret ;
		}

		/* check the trunk has already allowed vlans,and add new trunk member port to be a member of
		* all these vlans.
		*/
		ret = npd_trunk_allow_vlan_id_get(trunkId,vid,&vCount);
		if(NPD_TRUE == ret ) {
			for(i=0;i<vCount;i++) {
				npd_vlan_check_contain_trunk(vid[i],trunkId,&trunkTagMode);
				syslog_ax_trunk_dbg("get vlan count %d!\n",vCount);
				syslog_ax_trunk_dbg("trunk %d add to vlan %d tag mode %d\n",trunkId,vid[i],trunkTagMode);
				ret = npd_vlan_interface_port_add(vid[i],eth_index, trunkTagMode);
				if(VLAN_RETURN_CODE_PORT_EXISTS != ret && VLAN_RETURN_CODE_ERR_NONE != ret && NPD_TRUE != ret) /*NPD_TRUE --vlan L3 interface*/
				{
					syslog_ax_trunk_err("port %d add to vlan %d error,ret %d.\n",eth_index,vid[i],ret);
					return TRUNK_RETURN_CODE_MEMBER_ADD_ERR;
				}
			}
		}
		
		ret = npd_get_port_link_status(eth_index,&port_link_state);
		if(NPD_SUCCESS != ret) {
			return NPD_FALSE;
		}
		else {
			if(ETH_ATTR_LINKUP == port_link_state) {
				enDis = NPD_TRUE;
			}
			else {
				enDis = NPD_FALSE;	
			}
		}		
		ret = nam_asic_trunk_ports_add(devNum, trunkId, virPortNum,enDis);
		if (TRUNK_PORT_EXISTS_GTDB == ret || TRUNK_CONFIG_SUCCESS == ret) {
			if (TRUNK_CONFIG_SUCCESS == nam_asic_trunk_get_port_member_bmp(trunkId, &cretMbrBmp0, &cretMbrBmp1)){
				npd_trunk_hold_member_bmp(trunkId,cretMbrBmp0,cretMbrBmp1);		
				syslog_ax_trunk_dbg("trunk %d 's portMbrBmp0 = %0x,portMbrBmp1 = %0x\n",trunkId,\
										cretMbrBmp0,		cretMbrBmp1);
				ret = TRUNK_RETURN_CODE_ERR_NONE;
			}
		}
		else if (TRUNK_PORT_MBRS_FULL == ret) {
			/*delete port from trunk portlist,nam Hw trunkDB,allowed vlan portlist*/
			npd_trunk_port_del(trunkId,eth_index);
			ret = TRUNK_RETURN_CODE_PORT_MBRS_FULL;
			return ret;
		}
		else {	
			/*delete port from trunk portlist,nam Hw trunkDB,allowed vlan portlist*/
			npd_trunk_port_del(trunkId, eth_index);
			ret = TRUNK_RETURN_CODE_ERR_HW; 
		}
	}
	return ret;
}

unsigned int npd_trunk_port_del
(
	unsigned short	trunkId,
	unsigned int	eth_index
)
{
	unsigned int i = 0, ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned char devNum = 0, virPortNum = 0, isTagged = 0;
	unsigned int cretMbrBmp0 = 0, cretMbrBmp1 = 0;
	static unsigned short vid[NPD_MAX_VLAN_ID] = {0};
	unsigned int m_eth_index = 0, vCount = 0;
	
	ret = npd_get_devport_by_global_index(eth_index,&devNum,&virPortNum);
	if (ret != 0) 
	{
		 syslog_ax_trunk_err("global_index %d convert to devPort error ret %d\n",eth_index,ret);
		ret = TRUNK_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)){
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			 syslog_ax_trunk_dbg("Legal devNum %d,virPortNum %d\n",devNum,virPortNum);
			ret = TRUNK_RETURN_CODE_ERR_NONE;
		} 
		else {
			 syslog_ax_trunk_err("virPortNum error.\n");
			ret = TRUNK_RETURN_CODE_BADPARAM;
			return ret;
		}
	} 
	else{
		 syslog_ax_trunk_err("devNum error.\n");
		ret = TRUNK_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*if NOT Steps into the (trunk-config CMD node),trunk exists or not Test is wanted.*/
	/*if Steps into (trunk-config CMD node), it does NOT need test trunk node exist or not,because*/
	ret = npd_check_trunk_exist(trunkId);
	if (TRUNK_RETURN_CODE_TRUNK_EXISTS != ret) {
		 syslog_ax_trunk_err("trunk %d not exist.\n",trunkId);
		return TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;/*can NOT directed Return.*/
	}
	
	/*here MUST check the ETH-port is already member of the TRUNK or NOT*/
	ret = npd_trunk_check_port_membership(trunkId,eth_index);
	if (NPD_FALSE == ret) {
		 syslog_ax_trunk_err("Port %d was not the member of trunk % d\n",virPortNum,trunkId);
		return TRUNK_RETURN_CODE_PORT_NOTEXISTS;
	}

	/*zhangcl@autelan.com delete master port command 2013/1/15*/
	/*check the port is master port or NOT of the trunk*/
	#if 0
	ret = npd_trunk_master_port_get(trunkId, &m_eth_index);
	if((NPD_TRUE == ret) && (m_eth_index == eth_index)){
		return TRUNK_RETURN_CODE_DEL_MASTER_PORT;
	}
	#endif
	
	if (TRUNK_CONFIG_SUCCESS == nam_asic_trunk_get_port_member_bmp(trunkId, &cretMbrBmp0, &cretMbrBmp1)){
		npd_trunk_hold_member_bmp(trunkId,cretMbrBmp0,cretMbrBmp1);
	}
	ret = npd_trunk_port_member_del(trunkId,eth_index);
	if(TRUNK_RETURN_CODE_ERR_NONE != ret) {
		 syslog_ax_trunk_err("delete port %d from trunk %d software error ret %d\n",eth_index,trunkId,ret);
		return TRUNK_RETURN_CODE_ERR_GENERAL;
	}
	ret = nam_asic_trunk_ports_del(devNum, trunkId,virPortNum);
	if (0 != ret) {
		 syslog_ax_trunk_err("delete port %d from trunk %d hardware error ret %d\n",\
																	virPortNum,trunkId,ret);
		ret = TRUNK_RETURN_CODE_ERR_HW;
	}
	
	/*deleting port from trunk,it'll delete port from vlans that the trunk allowed!
	 *At the same time, add the port to Default vlan 1.*/
	ret = npd_trunk_allow_vlan_id_get(trunkId,vid,&vCount);
	if(NPD_TRUE == ret ){
		for(i=0;i<vCount;i++){
			if(NPD_TRUE == npd_vlan_check_contain_port(vid[i],eth_index, &isTagged)){
				if(!((DEFAULT_VLAN_ID == vid[i])&&(FALSE == isTagged))){
					ret = npd_vlan_interface_port_del(vid[i],eth_index, isTagged);
					syslog_ax_trunk_dbg("delete port %d from vlan %d ret %d.\n",\
																			eth_index,vid[i],ret);
					if(VLAN_RETURN_CODE_PORT_NOTEXISTS != ret && VLAN_RETURN_CODE_ERR_NONE != ret){
						ret = TRUNK_RETURN_CODE_MEMBER_DEL_ERR;
						return ret;
					}
				}
			}
		}
		/*syslog_ax_trunk_dbg("npd_trunk_port_add_del::return %d.",ret);*/
		ret = TRUNK_RETURN_CODE_ERR_NONE;
	}

	/*return to Default vlan 1.*/
	npd_vlan_interface_port_add(DEFAULT_VLAN_ID, eth_index,NPD_FALSE);syslog_ax_trunk_dbg("add port to default vlan \n");
	return ret;
}



unsigned int npd_trunk_master_port_set
(
	unsigned short trunkId, 
	unsigned int eth_index
)
{
	unsigned int ifIndex = 0, eth_index_orgl = 0, ret = 0;
	/*if NOT Steps into the (trunk-config CMD node),trunk exists or not Test is wanted.*/
	/*if Steps into (trunk-config CMD node), it does NOT need test trunk node exist or not,because*/
	ret = npd_check_trunk_exist(trunkId);
	if (TRUNK_RETURN_CODE_TRUNK_EXISTS != ret) {
		syslog_ax_trunk_dbg("npd_trunk_add_del:trunk %d NOT exist.\n",trunkId);
		return TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
	}
	/*here MUST check the ETH-port is already member of the TRUNK or NOT*/
	ret = npd_trunk_check_port_membership(trunkId,eth_index);
	if (NPD_TRUE != ret) {
		syslog_ax_trunk_dbg("port %d is not the member of trunk % d\n",eth_index,trunkId);
		return TRUNK_RETURN_CODE_PORT_NOTEXISTS;
	}
	/*check ETH_port is L3 interface or NOT*/
	if(MAX_STATIC_TRUNK_ID >= trunkId){
		ret = npd_eth_port_interface_check(eth_index,&ifIndex);
		if((NPD_TRUE == ret) && (ifIndex != ~0UI)) {
			return TRUNK_RETURN_CODE_PORT_L3_INTFG;
		}
	}
	{
		/*find the original Master port*/
		ret = npd_trunk_master_port_get(trunkId, &eth_index_orgl);
		if(eth_index != eth_index_orgl){
			if(NPD_TRUE == npd_trunk_master_port_config(trunkId, eth_index)){
				return TRUNK_RETURN_CODE_ERR_NONE;
			}
		}
	}
	
	return ret;
}

int npd_trunk_port_endis
(
	unsigned short trunkId,
	unsigned int eth_g_idx,
	unsigned char enDis
)
{
	unsigned int ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned char devNum = 0, virPortNum = 0, stat = NPD_FALSE;

	ret = npd_get_devport_by_global_index(eth_g_idx,&devNum,&virPortNum);
	if (ret != 0) 
	{
		 syslog_ax_trunk_dbg("global index %d convert to devPort error\n",eth_g_idx);
		return TRUNK_RETURN_CODE_BADPARAM;
	}
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = TRUNK_RETURN_CODE_ERR_NONE;
		} 
		else {
			 syslog_ax_trunk_dbg("virPortNum %d error\n",virPortNum);
			ret = TRUNK_RETURN_CODE_BADPARAM;
			return ret;
		}
	}

	ret = npd_check_trunk_exist(trunkId);
	if(TRUNK_RETURN_CODE_TRUNK_NOTEXISTS == ret){
		return ret;
	}

	ret = npd_trunk_check_port_membership(trunkId,eth_g_idx);
	if(NPD_FALSE == ret){
		return TRUNK_RETURN_CODE_PORT_NOTEXISTS;
	}
	npd_trunk_check_port_endis(trunkId,eth_g_idx,&stat);
	if(1 == enDis && 1 == stat){
		syslog_ax_trunk_dbg("the trunk %d port %d has already enable\n",trunkId,eth_g_idx);
		/*return NPD_TRUNK_PORT_ENABLE;*/
	}
	else if (0 == enDis && 0 == stat){
		syslog_ax_trunk_dbg("the trunk %d port %d has already disable\n",trunkId,eth_g_idx);
		/*return NPD_TRUNK_PORT_NOTENABLE;	*/
	}
	else{
		ret = npd_trunk_port_set_endis(trunkId,eth_g_idx,enDis);
		if(NPD_TRUE == ret){
			ret = nam_asic_trunk_port_endis(devNum,virPortNum,trunkId,enDis);
			if(0 != ret){
				syslog_ax_trunk_dbg("devNum %d,virPortNum %d,trunId %d, enDis %d,ret %d,error\n",devNum,virPortNum,trunkId,enDis,ret);
				enDis = ~enDis;
				npd_trunk_port_set_endis(trunkId,eth_g_idx,enDis);
				return TRUNK_RETURN_CODE_ERR_HW;
			}
		}
		else{
			syslog_ax_trunk_dbg("trunkId %d,eth_g_index %d,enDis %d,ret =%d,error\n",trunkId,eth_g_idx,enDis,ret);
			return TRUNK_RETURN_CODE_PORT_NOTEXISTS;
		}
	}
	return TRUNK_RETURN_CODE_ERR_NONE;
}

unsigned int npd_eth_port_configuration_check
(
	unsigned int eth_g_idx,
	unsigned short trunkId
)
{
	struct trunk_s *trunkNode = NULL;
	trunk_port_list_s *portList = NULL;
	/*struct trunk_port_list_node_s *node = NULL;*/
	/*struct list_head  *list = NULL,*pos = NULL;*/	
	unsigned int ret = NPD_FALSE,port_link_state = 0;
	unsigned int m_eth_index = 0,m_speed = 0,m_fc = 0,speed = 0,fc = 0;
	unsigned char devNum = 0,portNum = 0,m_devNum = 0,m_portNum = 0;
	npd_get_devport_by_global_index(eth_g_idx,&devNum,&portNum);
	
	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		syslog_ax_trunk_dbg("find trunk %d error!\n",trunkId);
		return NPD_FALSE;
	}
	
	portList = trunkNode->portList;
	
	if(NULL != portList) {
		if(0 != portList->count) {
			ret = npd_trunk_master_port_get(trunkId, &m_eth_index);
			if(NPD_TRUE != ret){
				return NPD_FALSE;
			}
		}

		npd_get_devport_by_global_index(m_eth_index,&m_devNum,&m_portNum);
		
		/*get the configuration of master port and the port which want to add to trunk*/

		/*get the speed	of master port	*/
		nam_get_port_config_speed(m_devNum,m_portNum,&m_speed);
		/*get the flow control of master port	*/ 
		nam_get_port_config_flowcontrol(m_devNum,m_portNum,&m_fc);
		/*syslog_ax_trunk_dbg("the master port(%d,%d) : speed %d flow-control %d\n",m_devNum,m_portNum,m_speed,m_fc);*/
		/*get the speed	of the port which want to add to trunk	*/
		nam_get_port_config_speed(devNum,portNum,&speed);
		/*get the flow control of the port which want to add to trunk	*/ 
		nam_get_port_config_flowcontrol(devNum,portNum,&fc);
		/*/syslog_ax_trunk_dbg("the added port(%d,%d) : speed %d flow-control %d\n",devNum,portNum,speed,fc);*/

		if(m_speed == speed && m_fc == fc) {
			ret = NPD_FALSE;
		}
		else
			ret = NPD_TRUE;
	}

	return ret;
}

#if 0
/************************
 *
 *
 *
 ************************/
int npd_trunk_port_en_dis
(
	unsigned short trunkId,
	unsigned int eth_g_idx,
	unsigned char enDis
)
{
	unsigned int ret = NPD_SUCCESS;
	unsigned char devNum = 0,virPortNum = 0,stat = NPD_FALSE;

	ret = npd_get_devport_by_global_index(eth_g_idx,&devNum,&virPortNum);
	if (ret != 0) 
	{
		 syslog_ax_trunk_dbg("global index %d convert to devPort error\n",eth_g_idx);
		return NPD_TRUNK_BADPARAM;
	}
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = NPD_TRUNK_ERR_NONE;
		} 
		else {
			 syslog_ax_trunk_dbg("virPortNum %d error\n",virPortNum);
			ret = NPD_TRUNK_BADPARAM;
			return ret;
		}
	}

	ret = npd_check_trunk_exist(trunkId);
	if(NPD_TRUNK_NOTEXISTS == ret){
		return ret;
	}

	ret = npd_trunk_check_port_membership(trunkId,eth_g_idx);
	if(NPD_FALSE == ret){
		return NPD_TRUNK_PORT_NOTEXISTS;
	}

	if(ETH_ATTR_LINKUP == npd_check_eth_port_status(eth_g_idx)) {
		npd_trunk_check_port_endis(trunkId,eth_g_idx,&stat);
		if(1 == enDis && 1 == stat){
			syslog_ax_trunk_dbg("the trunk %d port %d has already enable\n",trunkId,eth_g_idx);
			return NPD_TRUNK_PORT_ENABLE;
		}
		else if (0 == enDis && 0 == stat){
			syslog_ax_trunk_dbg("the trunk %d port %d has already disable\n",trunkId,eth_g_idx);
			return NPD_TRUNK_PORT_NOTENABLE;	
		}
		else{
			ret = npd_trunk_port_set_endis(trunkId,eth_g_idx,enDis);
			if(NPD_TRUE == ret){
				ret = nam_asic_trunk_port_endis(devNum,virPortNum,trunkId,enDis);
				if(0 != ret){
					syslog_ax_trunk_dbg("devNum %d,virPortNum %d,trunId %d, enDis %d,ret %d,error\n",devNum,virPortNum,trunkId,enDis,ret);
					enDis = ~enDis;
					npd_trunk_port_set_endis(trunkId,eth_g_idx,enDis);
					return NPD_TRUNK_ERR_HW;
				}
			}
			else{
				syslog_ax_trunk_dbg("trunkId %d,eth_g_index %d,enDis %d,ret =%d,error\n",trunkId,eth_g_idx,enDis,ret);
				return NPD_TRUNK_PORT_NOTEXISTS;
			}
		}
	}
	else
		return NPD_TRUNK_PORT_LINK_DOWN;
	return NPD_SUCCESS;
}
/* delete trunk node*/
/* by RE-init the struct trunk_s to be ZERO*/
/**********************************************************************************
 *
 * delete trunk by TRUNK ID on both SW and HW side
 *
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUNK_ERR_GENERAL - if error occurred when create trunk in SW side 
 *		NPD_TRUNK_ERR_HW - if error occurred when create trunk in HW side
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_destroy_node
(
	unsigned short trunkId
)
{
	unsigned int	i,j,ret = NPD_TRUNK_ERR_NONE;
	unsigned char	tag=0;
	unsigned int mbrCount = 0,m_eth_index =0,eth_g_index[8] = {0};
	ret = npd_check_trunk_exist(trunkId);
	if(ret == NPD_TRUNK_NOTEXISTS)
	{ 
		syslog_ax_trunk_dbg("This Vlan inteface does NOT exist.\n");
	 	return ret;  
	}
	else 
	{
		for(i =2;i<NPD_VLAN_NUMBER_MAX;i++) {
			if (NPD_VLAN_EXISTS == npd_check_vlan_exist(i)) {
				NPD_DEBUG(("check trunk %d is member of vlan %d or NOT.",eth_g_index,i));
				if (NPD_TRUE == npd_vlan_check_contain_trunk(i,trunkId,&tag)){
					ret = npd_vlan_trunk_entity_del(i, trunkId);
					/*get trunk port member index,*/
					if(NPD_TRUE == npd_trunk_member_port_index_get(trunkId, eth_g_index,&mbrCount)){
						for(j=0;j<mbrCount;j++){
							/*get the master port of trunk.*/
							npd_trunk_master_port_get(trunkId, &m_eth_index);

							/*delete these ports from the vlan,except master port*/
							if(m_eth_index!=eth_g_index[j]){
								npd_vlan_interface_port_del(i, eth_g_index[j], tag);
							}
							/*clear eth_port trunkId,including master port*/
							npd_eth_port_clear_ptrunkid(eth_g_index[j]);

							/*delete these ports from trunk.*/
							npd_trunk_port_del(trunkId,eth_g_index[j]);
						}
					}
				}
				else {
				 syslog_ax_trunk_dbg("trunk %d is NOT member of vlan %d",trunkId,i);
					continue;
				}
			}
			else { continue;}
		}
		
		
		ret = nam_asic_trunk_delete(trunkId);
		if (0 != ret) {
			 syslog_ax_trunk_dbg("Npd_trunk::nam_asic_trunk_entry_delete:: trunkId %d ERROR. \n",trunkId);
			ret = NPD_TRUNK_ERR_HW;
			return ret;
			/*ret = NPD_DBUS_ERROR; in this case dbus message can not reply*/ 
		}

		/*to update the trunk info data struct in sw */
		/*trunk active op ,need to modify the vid_list only*/
		ret = npd_delete_trunk(trunkId);
		if (0 != ret) {
			 syslog_ax_trunk_dbg("Npd_trunk::npd_delete_trunk:trunkId %d Error",trunkId);
			return ret;
		}
		
	}

	return ret;
}
#endif
/* delete trunk node*/
/* by RE-set the struct trunk_s to be ZERO*/
/**********************************************************************************
 *
 *  delete trunk by TRUNK ID on both SW and HW side
 *
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUNK_ERR_GENERAL - if error occurred when create trunk in SW side 
 *		NPD_TRUNK_ERR_HW - if error occurred when create trunk in HW side
 *		
 *
 **********************************************************************************/
unsigned int npd_trunk_destroy_node
(
	unsigned short trunkId
)
{
	unsigned int	i = 0,ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned char	devNum = 0,virPortNum = 0,tagMode = NPD_FALSE,isTagged = 0;
	unsigned int 	mbrCount = 0,vlan_count = 0,m_eth_index =0,eth_g_index[8] = {0};
	static unsigned short 	vid[NPD_MAX_VLAN_ID] = {0};
	ret = npd_check_trunk_exist(trunkId);
	if(TRUNK_RETURN_CODE_TRUNK_NOTEXISTS == ret)
	{ 
		syslog_ax_trunk_dbg("trunk %d does not exist.\n",trunkId);
	 	return ret;  
	}
	else {
		/*first disable trunk qinq in special vlan*/
		if((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I))
		{
			for(i=0;i<4094;i++)
			{
				if(g_vlanlist[i].tagTrkBmp[trunkId-1] == 1)
				{
					ret = npd_vlan_trunk_qinq_disable(trunkId,i+1);
					if(ret != TRUNK_RETURN_CODE_ERR_NONE)
					{
						syslog_ax_trunk_err("disable vlan trunk qinq fail !\n");
					}
				}
			}
		}
		
		/*delete master port.*/
		ret = npd_trunk_master_port_get(trunkId,&m_eth_index);
		if(NPD_TRUE == ret) {
			syslog_ax_trunk_dbg("trunk %d master port %d.\n",trunkId,m_eth_index);
			npd_trunk_port_member_del(trunkId,m_eth_index);

			ret = npd_get_devport_by_global_index(m_eth_index,&devNum,&virPortNum);
			if (ret != 0) {
				syslog_ax_trunk_dbg("global_index %d convert to devPort Error.\n",m_eth_index);
				ret = TRUNK_RETURN_CODE_BADPARAM;
				return ret;
			}
			else {
				nam_asic_trunk_ports_del(devNum,trunkId,virPortNum);
				npd_vlan_interface_port_add(1,m_eth_index,NPD_FALSE);
			}
			
			/*deleting port from trunk,it'll delete port from vlans that the trunk allowed!
			 *At the same time, add the port to Default vlan 1.*/
			ret = npd_trunk_allow_vlan_id_get(trunkId,vid,&vlan_count);
			if(NPD_TRUE == ret) {
				for(i=0;i<vlan_count;i++) {
					if(NPD_TRUE == npd_vlan_check_contain_port(vid[i],m_eth_index, &isTagged) && 1 != vid[i]) {
						ret = npd_vlan_interface_port_del(vid[i],m_eth_index, isTagged);
						syslog_ax_trunk_dbg("delete master port %d from vlan %d ret %d.\n",\
															    m_eth_index,vid[i],ret);
						if(VLAN_RETURN_CODE_PORT_NOTEXISTS != ret && VLAN_RETURN_CODE_ERR_NONE != ret) {
							ret = TRUNK_RETURN_CODE_MEMBER_DEL_ERR;
							syslog_ax_trunk_dbg("delete master port %d from vlan %d error, ret %d\n", \
																m_eth_index,vid[i],ret);
						}
					}
				}
				ret = TRUNK_RETURN_CODE_ERR_NONE;
			}		
		}
		
		/*delete NON-master port*/
		ret = npd_trunk_member_port_index_get_all(trunkId,eth_g_index,&mbrCount);
		if(NPD_TRUE == ret) {
			for (i=0;i<mbrCount;i++) {
				if(m_eth_index != eth_g_index[i]) {
					ret = npd_trunk_port_del(trunkId,eth_g_index[i]);
					if(TRUNK_RETURN_CODE_ERR_NONE != ret){
						syslog_ax_trunk_dbg("trunk %d delete port %d error,ret %d.\n",trunkId,eth_g_index[i],ret);
						return TRUNK_RETURN_CODE_MEMBER_DEL_ERR;
					}
				}
			}
		}
		
		/*delete trunk allow vlan list*/
		ret = npd_trunk_allow_vlan_id_get(trunkId,vid,&vlan_count);
		if(NPD_TRUE == ret) {
			for(i=0;i<vlan_count;i++) {
				npd_vlan_check_contain_trunk(vid[i],trunkId,&tagMode);
				npd_trunk_vlan_list_delfrom(trunkId,vid[i],tagMode);
				npd_vlan_trunk_member_del(vid[i],trunkId,tagMode);
			}
		}
		
		/*delete trunk entity*/
		ret = nam_asic_trunk_delete(trunkId);
		if (0 != ret) {
			 syslog_ax_trunk_dbg("delete trunk %d hardware error,ret %d.\n",trunkId,ret);
			ret = TRUNK_RETURN_CODE_ERR_HW;
			return ret;
		}
		
		/*to update the trunk info data struct in sw */
		/*trunk active op ,need to modify the vid_list only*/
		ret = npd_delete_trunk(trunkId);
		if (TRUNK_RETURN_CODE_ERR_NONE != ret) {
			 syslog_ax_trunk_dbg("delete trunkId %d software error,ret %d.\n",trunkId,ret);
			return ret;
		}

		if((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I))
		{
			ret = npd_dst_trunk_invalid(trunkId);
			if(TRUNK_RETURN_CODE_ERR_NONE != ret)
			{
				syslog_ax_trunk_dbg("set distributed trunk %d invalid fail !\n",trunkId);
			}
		}
	}
	return ret;
}

int npd_trunk_allow_vlan
(
	unsigned short trunkId,
	unsigned int vlan_count,
	unsigned short vid[],
	unsigned char tagMode
)
{
	unsigned char  isTagged = 0, hasAllow = NPD_FALSE;
	unsigned int i = 0, j = 0, mbr_count = 0, eth_g_index[24] = {0};
	unsigned int  ret = TRUNK_RETURN_CODE_ERR_NONE;

	/*check if vlan has already allowed by this trunk*/
	for(i=0;i<vlan_count;i++){
		ret = npd_vlan_check_contain_trunk(vid[i], trunkId, &isTagged);
		if(NPD_TRUE == ret ){
			syslog_ax_trunk_err("vlan %d has trunk %d member already.\n",vid[i],trunkId);
			return VLAN_RETURN_CODE_TRUNK_EXISTS;
		}
	}

	#if 0	
	/*check if vlan has already allowed by other trunks as untag member)*/
	if(0 == tagMode){
		for(i=1; i<NPD_MAX_TRUNK_ID; i++){
			ret = npd_check_trunk_exist(i);
			if(NPD_TRUNK_EXISTS == ret && i!=trunkId){
				for(j=0;j<vlan_count;j++){
					ret = npd_vlan_check_trunk_membership(vid[j], i, NPD_FALSE);
					if(NPD_TRUE == ret ){
						return NPD_VLAN_TRUNK_CONFLICT;
					}
				}
			}
			else 
				continue;
		}
	}	
	#endif
	
	/*check this trunk is untag member of other vlans*/
	for(i=0;i<vlan_count;i++) {
		if(0 == tagMode && 1 != vid[i]) {
			for(i=2;i<NPD_VLAN_NUMBER_MAX ;i++) {
				for(j=0;j<vlan_count;j++) {
					if(VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i) && i != vid[j]) {
						ret = npd_vlan_check_trunk_membership(i, trunkId, NPD_FALSE);
						if(NPD_TRUE == ret ) {
							syslog_ax_vlan_dbg("Trunk %d is untagged member of vlan %d\n",trunkId,i);
							return VLAN_RETURN_CODE_TRUNK_CONFLICT;
						}
					}
				}
			}

			if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId,eth_g_index,&mbr_count)) {
				/*if trunk add to vlan as untagged, delete the trunk from default vlan 1*/
				npd_vlan_trunk_member_del(1, trunkId,tagMode);
				npd_trunk_vlan_list_delfrom(trunkId,1,tagMode);			

				for(i = 0; i < mbr_count; i ++) {
					syslog_ax_vlan_dbg("delete trunk %d port %d from vlan %d %s.\n",
										trunkId,eth_g_index[i],1,(tagMode)?"tag":"untag");
					npd_vlan_interface_port_del(1, eth_g_index[i], tagMode);
				}
			}
		}
	}
	
	/*modify trunk_vlan_list and vlan_trunk_list and add all port member of the trunk to vlan.*/
	if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId,eth_g_index,&mbr_count)){
		for(i=0;i<vlan_count;i++) {
			ret = npd_trunk_check_vlan_allowship(trunkId,vid[i]);
			if(NPD_TRUE == ret) {
				syslog_ax_trunk_dbg("trunk %d has allow vlan %d!\n",trunkId,vid[i]);
				hasAllow = NPD_TRUE;
				continue;
			}
			else {
				/*add vlanNode to trunk_vlan_list_node*/
				npd_trunk_vlan_list_addto(trunkId,vid[i],tagMode);

				/*add trunkNode to vlan_trunk_list_node*/
				npd_vlan_trunk_member_add(vid[i],trunkId,tagMode);

				/*add trunk member port to vlan*/
				for(j=0;j<mbr_count;j++) {
					ret = npd_vlan_check_contain_port(vid[i],eth_g_index[j], &isTagged);
					if(NPD_TRUE == ret && tagMode == isTagged){
						continue;
					}
					else if(NPD_TRUE == ret && tagMode != isTagged) {
						ret = npd_vlan_interface_port_del(vid[i],eth_g_index[j],isTagged);
						ret = npd_vlan_interface_port_add(vid[i],eth_g_index[j],tagMode);
						if(VLAN_RETURN_CODE_ERR_NONE != ret && VLAN_RETURN_CODE_PORT_EXISTS != ret && NPD_TRUE != ret){
							syslog_ax_trunk_dbg("port %d add to vlan %d error, tagMode %d. ret = %#x.\n",eth_g_index[j], tagMode, vid[i],ret);
							/*if add error,delete the node add before*/
							npd_trunk_vlan_list_delfrom(trunkId,vid[i],tagMode);

							npd_vlan_trunk_member_del(vid[i],trunkId,tagMode);
							return TRUNK_RETURN_CODE_ALLOW_ERR;
						}
					}
					else if(NPD_FALSE == ret) {
						ret = npd_vlan_interface_port_add(vid[i],eth_g_index[j],tagMode);
						if(VLAN_RETURN_CODE_ERR_NONE != ret && VLAN_RETURN_CODE_PORT_EXISTS != ret && NPD_TRUE != ret){
							syslog_ax_trunk_dbg("port %d add to vlan %d error. ret = %#x.\n",eth_g_index[j],vid[i],ret);
							/*if add error,delete the node add before*/
							npd_trunk_vlan_list_delfrom(trunkId,vid[i],tagMode);

							npd_vlan_trunk_member_del(vid[i],trunkId,tagMode);
							return TRUNK_RETURN_CODE_ALLOW_ERR;
						}
					}
				}
			}
		}
	}
	else {
		/*trunk has not any port member add to vlan*/
		return TRUNK_RETURN_CODE_NO_MEMBER;
	}

	if(NPD_TRUE == hasAllow){
		/*trunk has already been the member of this vlan*/
		return TRUNK_RETURN_CODE_ALLOW_VLAN;
	}
	return TRUNK_RETURN_CODE_ERR_NONE;
}


int npd_trunk_refuse_vlan
(
	unsigned short trunkId,
	unsigned int vlan_count,
	unsigned short vid[],
	unsigned char tagMode
)
{
	unsigned char  isTagged = 0,notAllow = NPD_FALSE;
	unsigned int i,j,mbr_count = 0,eth_g_index[24] = {0};
	unsigned int  ret = TRUNK_RETURN_CODE_ERR_NONE;

	/*every vlan enterd by DCLI exist*/
	if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId,eth_g_index,&mbr_count)){
		#if 0
		for(j= 0;j<mbr_count;j++){
			ret = npd_eth_port_interface_check(eth_g_index[j],&ifIndex);
			if((NPD_TRUE == ret) && (ifIndex != ~0UI)){
				return NPD_VLAN_PORT_L3_INTF;
			}
		}
		#endif
		for(i=0;i<vlan_count;i++) {
			ret = npd_trunk_check_vlan_allowship(trunkId, vid[i]);
			if(NPD_FALSE == ret) {
				syslog_ax_trunk_err("trunk %d has refused vlan %d!\n",trunkId,vid[i]);
				notAllow = NPD_TRUE;
				continue;
			}
			else {
				/*delete trunk_vlan_list node*/
				if(TRUNK_RETURN_CODE_VLAN_TAGMODE_ERR == npd_trunk_vlan_list_delfrom(trunkId,vid[i],tagMode)){
					return TRUNK_RETURN_CODE_VLAN_TAGMODE_ERR;
				}

				/*delete trunkNode from vlan trunklist*/
				/*if(NPD_VLAN_EXISTS == npd_check_vlan_exist(vid[i])) {*/
				npd_vlan_trunk_member_del(vid[i],trunkId,tagMode);

				for(j= 0;j<mbr_count;j++) {
					/*delete trunk member port to vlan*/
					ret = npd_vlan_check_contain_port(vid[i],eth_g_index[j], &isTagged);
					if(NPD_TRUE == ret ) {
						ret = npd_vlan_interface_port_del(vid[i], eth_g_index[j],isTagged);
						if(VLAN_RETURN_CODE_ERR_NONE != ret) { 
							return TRUNK_RETURN_CODE_REFUSE_ERR;
						}
					}
				}
				/*add trunk to default vlan 1 as untagged if refuse this trunk as untagged*/
				if(NPD_FALSE == tagMode) {
					if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId,eth_g_index,&mbr_count)) {
						npd_vlan_trunk_member_add(1, trunkId,tagMode);
						npd_trunk_vlan_list_addto(trunkId,1, tagMode);		
						for(i = 0; i<mbr_count;i++) {
							syslog_ax_vlan_dbg("add trunk %d port %#x to vlan %d %s\n",
												trunkId,eth_g_index[i],1,(tagMode)?"tag":"untag");
							npd_vlan_interface_port_add(1, eth_g_index[i], tagMode);
						}
					}
				}
				/*flush all fdb item according vlan_trunk*/
				nam_fdb_table_delete_entry_with_vlan_trunk(vid[i],trunkId);
			}
		}
	}
	if(NPD_TRUE == notAllow)
		return TRUNK_RETURN_CODE_NOTALLOW_VLAN;
	return TRUNK_RETURN_CODE_ERR_NONE;
}


/******************************************************
 *  show trunk port membership by read Software record.
 *
 *****************************************************/
int npd_trunk_get_mbr_bmp_via_sw
(
	unsigned short trunkId,
	unsigned int *mbrBmp_sp,
	unsigned int *disMbrBmp_sp
)
{
	unsigned int i,ret = NPD_SUCCESS;
	unsigned int mbrCont = 0;
	unsigned int mbr_index[64] = {0},dis_mbr_idx[64] = {0};
	unsigned int MbrBmpTmpSP = 0;
	unsigned int slotNo = 0,localPortNo = 0,slot_index = 0;

	ret = npd_trunk_endis_member_port_index_get(trunkId,mbr_index,NPD_TRUE,&mbrCont);
	if(NPD_TRUE == ret){
		for(i = 0;i<mbrCont;i++){
			/*npd_get_devport_by_global_index(unmbr_index[i], &devNum, &portNum);*//*no use:0604*/
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(mbr_index[i]);
			slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
			localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(mbr_index[i]);
			localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
			MbrBmpTmpSP |= 1<<((slotNo-1)*8+localPortNo);
			syslog_ax_trunk_dbg("trunk %d enable portBmp 0x%x \n",trunkId, MbrBmpTmpSP);
		}
		*mbrBmp_sp =MbrBmpTmpSP;
	}

	if(0 == MbrBmpTmpSP) ret = NPD_FAIL;
	else { ret = NPD_SUCCESS; }
		
	MbrBmpTmpSP = 0;
	ret = npd_trunk_endis_member_port_index_get(trunkId,dis_mbr_idx,NPD_FALSE,&mbrCont);
	if(NPD_TRUE == ret){
		for(i = 0;i<mbrCont;i++){
			/*npd_get_devport_by_global_index(unmbr_index[i], &devNum, &portNum);*//*no use:0604*/
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(dis_mbr_idx[i]);
			slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
			localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(dis_mbr_idx[i]);
			localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
			MbrBmpTmpSP |= 1<<((slotNo-1)*8+localPortNo);
			syslog_ax_trunk_dbg("trunk %d disable portBmp 0x%x \n",trunkId, MbrBmpTmpSP);
		}
		*disMbrBmp_sp =MbrBmpTmpSP;
	}
	if(0 == MbrBmpTmpSP && NPD_SUCCESS != ret)
		return NPD_FAIL;
	return NPD_SUCCESS;
}


/******************************************************
 *  show trunk port membership by read Software record.
 *
 *****************************************************/
int npd_trunk_get_mbr_bmp_via_sw_v1
(
	unsigned short trunkId,
	PORT_MEMBER_BMP *mbrBmp_sp,
	PORT_MEMBER_BMP *disMbrBmp_sp
)
{
	unsigned int i,ret = NPD_SUCCESS;
	unsigned int mbrCont = 0;
	unsigned int mbr_index[64] = {0},dis_mbr_idx[64] = {0};
	PORT_MEMBER_BMP MbrBmpTmpSP;
	unsigned int slotNo = 0,localPortNo = 0,slot_index = 0;
    memset(&MbrBmpTmpSP,0,sizeof(PORT_MEMBER_BMP));
	ret = npd_trunk_endis_member_port_index_get(trunkId,mbr_index,NPD_TRUE,&mbrCont);
	if(NPD_TRUE == ret){
		for(i = 0;i<mbrCont;i++){
			/*npd_get_devport_by_global_index(unmbr_index[i], &devNum, &portNum);*//*no use:0604*/
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(mbr_index[i]);
			slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
			localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(mbr_index[i]);
			localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
			if(IS_DISTRIBUTED == SYSTEM_TYPE){
				MbrBmpTmpSP.portMbr[localPortNo/32]|=1<<(localPortNo%32);
			}
			else
			{
				MbrBmpTmpSP.portMbr[localPortNo/32]|= 1<<((slotNo-1)*8+(localPortNo%32));
			}
			syslog_ax_trunk_dbg("trunk %d enable portBmp[0] 0x%x \n",trunkId, MbrBmpTmpSP.portMbr[0]);
		}
		mbrBmp_sp->portMbr[0] = MbrBmpTmpSP.portMbr[0];
		mbrBmp_sp->portMbr[1] = MbrBmpTmpSP.portMbr[1];
	}

	if((0 == MbrBmpTmpSP.portMbr[0]) &&(0 == MbrBmpTmpSP.portMbr[1]))ret = NPD_FAIL;
	else { ret = NPD_SUCCESS; }
		
	memset(&MbrBmpTmpSP,0,sizeof(PORT_MEMBER_BMP));
	ret = npd_trunk_endis_member_port_index_get(trunkId,dis_mbr_idx,NPD_FALSE,&mbrCont);
	if(NPD_TRUE == ret){
		for(i = 0;i<mbrCont;i++){
			/*npd_get_devport_by_global_index(unmbr_index[i], &devNum, &portNum);*//*no use:0604*/
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(dis_mbr_idx[i]);
			slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
			localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(dis_mbr_idx[i]);
			localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
			if(IS_DISTRIBUTED == SYSTEM_TYPE){
				MbrBmpTmpSP.portMbr[localPortNo/32]|=1<<(localPortNo%32);
			}
			else
			{
				MbrBmpTmpSP.portMbr[localPortNo/32]|= 1<<((slotNo-1)*8+(localPortNo%32));
			}
			syslog_ax_trunk_dbg("trunk %d disable portBmp[0] 0x%x \n",trunkId, MbrBmpTmpSP.portMbr[0]);
		}
		disMbrBmp_sp->portMbr[0] = MbrBmpTmpSP.portMbr[0];
		disMbrBmp_sp->portMbr[1] = MbrBmpTmpSP.portMbr[1];
	}
	if((0 == MbrBmpTmpSP.portMbr[0])&&(0 == MbrBmpTmpSP.portMbr[1])&& NPD_SUCCESS != ret)
		return NPD_FAIL;
	return NPD_SUCCESS;
}

void npd_save_trunk_cfg(char* buf,int bufLen)
{
	char *showStr = buf,*cursor = NULL;
	int totalLen = 0;
	int i = 0;
	struct trunk_s* node = NULL;
	struct trunk_port_list_node_s *portNode = NULL;
	trunk_port_list_s* portList = NULL;
	struct list_head *pos = NULL,*list  = NULL;
	unsigned int eth_g_index = 0;
	unsigned char slot_index = 0,slot_no = 0,local_port_no = 0;
	int enDis =0;
	
	cursor = showStr;

	for(i = 1; i <= MAX_STATIC_TRUNK_ID; i++) {
		char tmpBuf[2048] = {0},*tmpPtr = NULL;
		int length = 0;
		tmpPtr = tmpBuf;
		
		if(TRUNK_RETURN_CODE_TRUNK_EXISTS == npd_check_trunk_exist(i)) {
			node = npd_find_trunk(i);
			if(NULL != node) {
				 syslog_ax_trunk_dbg("find trunk %d in system.\n",i);
				if((length + 30) < 2048) {
					length += sprintf(tmpPtr,"create trunk %d %s\n",i,node->trunkName);
					 syslog_ax_trunk_dbg("%s\n",tmpPtr);
					tmpPtr = tmpBuf+length;
				}

				if((length + 40) < 2048) {
					if(node->loadBanlcMode)
					length += sprintf(tmpPtr,"config trunk load-balance %s\n",trkLBalanc[node->loadBanlcMode]);
					 syslog_ax_trunk_dbg("%s\n",tmpPtr);
					tmpPtr = tmpBuf+length;
				}

				portList = node->portList;
				if(NULL != portList) {
					if(0 != portList->count) {
						if((length + 19) < 2048) {
							length += sprintf(tmpPtr,"config trunk %d\n",i);
							 syslog_ax_trunk_dbg("%s\n\n",tmpPtr);
							tmpPtr = tmpBuf + length;
						}
						
						list = &(portList->list);
						if(NULL !=list){
							__list_for_each(pos,list) {
								portNode = list_entry(pos,struct trunk_port_list_node_s,list);
								if(portNode) {
									eth_g_index = portNode->eth_global_index;
									slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
									slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
									local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
									local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_no);
									if((length + 14 )< 2048) {
										length += sprintf(tmpPtr,"  add port %d/%d\n",slot_no,local_port_no);
										 syslog_ax_trunk_dbg("%s",tmpPtr);
										tmpPtr = tmpBuf + length;
									}
									if((length + 21 )< 2048) {
										enDis = portNode->enDisFlag;
										length += sprintf(tmpPtr,"  set port %d/%d %s\n",slot_no,local_port_no,enDis?"enable":"disable");
										 syslog_ax_trunk_dbg("%s",tmpPtr);
										tmpPtr = tmpBuf + length;
									}
									/*zhangcl@autelan.com 2012-09-15 do not need master port any more*/
									#if 0
									if(NPD_TRUE == portNode->masterPortFlag ){
										if((length + 21 )< 2048) {
											length += sprintf(tmpPtr,"  set master-port %d/%d\n",slot_no,local_port_no);
											 syslog_ax_trunk_dbg("%s",tmpPtr);
											tmpPtr = tmpBuf + length;
										}
									}
									#endif
								}
							}
						}
						if((length + 4) < 2048) { /* exit port config(add,masterport,enDis,) */
							length += sprintf(tmpPtr,"exit\n");
							 syslog_ax_trunk_dbg("%s\n\n",tmpPtr);
							tmpPtr = tmpBuf + length;
						}
					}
				} /*endof trunk port config saving*/
				#if 0 /*trunk saving before vlan, so allow vlans NOT exist here.*/
				/*cause no port trunk can NOT allow vlan, so saving allowvlanL 
				do need enter trunk config CMD node "config trunk i"*/
				allowVlanL = node->allowVlanList;
				if(NULL != allowVlanL){
					if(0 != allowVlanL->count){
						list = &(allowVlanL->list);
						if(NULL !=list){
							__list_for_each(pos,list) {
								vlanNodeL = list_entry(pos,struct trunk_port_list_node_s,list);
								if(vlanNodeL) {
									if((length + 14 )< 2048) {
										length += sprintf(tmpPtr,"  allow vlan %d %s\n",vlanNodeL->vlanId,(vlanNodeL->uTagM)?"tag":"untag");
										 syslog_ax_trunk_dbg("%s",tmpPtr);
										tmpPtr = tmpBuf + length;
									}
								}
							}
						}
						if((length + 4) < 2048) { /* exit port config(add,masterport,enDis,) */
							length += sprintf(tmpPtr,"exit\n",i);
							 syslog_ax_trunk_dbg("%s\n\n",tmpPtr);
							tmpPtr = tmpBuf + length;
						}
					}
				}
				#endif 
				/* syslog_ax_trunk_dbg((""));*/
				if((totalLen +length + 5 ) > bufLen) { /* 5 exit */
					 syslog_ax_trunk_err("save trunk buffer full\n");
					break;
				}
				 syslog_ax_trunk_dbg("tmpBuf Length = %d.\n",length);
				totalLen += sprintf(cursor,"%s",tmpBuf);
				cursor = showStr + totalLen;
				/*
				totalLen += sprintf(cursor,"exit\n");
				 syslog_ax_trunk_dbg("%s",cursor);
				cursor = showStr + totalLen;
				*/
			}
		} /*trunk i not exist.*/
	}/* end of (for loop)*/
}

void npd_save_dst_trunk_cfg(char *buf,int bufLen)
{
	char *showStr = buf,*cursor = NULL;
	int totalLen = 0;
	int i = 0,j=0;
	unsigned char slot_no = 0,local_port_no = 0;
	
	cursor = showStr;
	for(i = 1; i <= MAX_STATIC_TRUNK_ID; i++) 
	{
		char tmpBuf[2048] = {0},*tmpPtr = NULL;
		int length = 0;
		tmpPtr = tmpBuf;


		if(g_dst_trunk[i-1].trunkId)
		{
			if((length + 30) < 2048) 
			{
				length += sprintf(tmpPtr,"create trunk %d %s\n",g_dst_trunk[i-1].trunkId,g_dst_trunk[i-1].trunkName);
				syslog_ax_trunk_dbg("%s\n",tmpPtr);
				tmpPtr = tmpBuf+length;
			}

			if((length + 40) < 2048) 
			{
					if(g_dst_trunk[i-1].loadBanlcMode && (LOAD_BANLC_MAC_IP_L4 != g_dst_trunk[i-1].loadBanlcMode))
					length += sprintf(tmpPtr,"config trunk load-balance %s\n",trkLBalanc[g_dst_trunk[i-1].loadBanlcMode]);
					syslog_ax_trunk_dbg("%s\n",tmpPtr);
					tmpPtr = tmpBuf+length;
			}

			if(0 != g_dst_trunk[i-1].portLog)
			{
				if((length + 19) < 2048) {
					length += sprintf(tmpPtr,"config trunk %d\n",i);
					 syslog_ax_trunk_dbg("%s\n\n",tmpPtr);
					tmpPtr = tmpBuf + length;
				}
        		for(j=0; j<g_dst_trunk[i-1].portLog; j++)
        		{
					slot_no = g_dst_trunk[i-1].portmap[j].slot;
					local_port_no = g_dst_trunk[i-1].portmap[j].port;
					if((length + 14 )< 2048) {
						length += sprintf(tmpPtr,"  add port %d/%d\n",slot_no,local_port_no);
						 syslog_ax_trunk_dbg("%s",tmpPtr);
						tmpPtr = tmpBuf + length;
					}
					#if 0
					if(NPD_TRUE == g_dst_trunk[i-1].portmap[j].master){
						if((length + 21 )< 2048) {
							length += sprintf(tmpPtr,"  set master-port %d/%d\n",slot_no,local_port_no);
							 syslog_ax_trunk_dbg("%s",tmpPtr);
							tmpPtr = tmpBuf + length;
						}
					}
					#endif
				}
				if((length + 4) < 2048) { /* exit port config(add,masterport,enDis,) */
					length += sprintf(tmpPtr," exit\n");
					 syslog_ax_trunk_dbg("%s\n\n",tmpPtr);
					tmpPtr = tmpBuf + length;
				}					
			}

			/* syslog_ax_trunk_dbg((""));*/
			if((totalLen +length + 5 ) > bufLen) { /* 5 exit */
				 syslog_ax_trunk_err("save trunk buffer full\n");
				break;
			}
			 syslog_ax_trunk_dbg("tmpBuf Length = %d.\n",length);
			totalLen += sprintf(cursor,"%s",tmpBuf);
			cursor = showStr + totalLen;
		}
	}/* end of (for loop)*/
}

/* save trunk config on smu */
void npd_save_trunk_cfg_smu(char* buf,int bufLen)
{
	char *showStr = buf,*cursor = NULL;
	int totalLen = 0;
	int i = 0,j=0;
	unsigned char slot_no = 0,local_port_no = 0;
	
	cursor = showStr;

	for(i = 1; i <= MAX_STATIC_TRUNK_ID; i++) 
	{
		char tmpBuf[2048] = {0},*tmpPtr = NULL;
		int length = 0;
		tmpPtr = tmpBuf;


		if(g_dst_trunk[i-1].trunkId)
		{
			if((length + 30) < 2048) 
			{
				length += sprintf(tmpPtr,"create trunk %d %s\n",g_dst_trunk[i-1].trunkId,g_dst_trunk[i-1].trunkName);
				syslog_ax_trunk_dbg("%s\n",tmpPtr);
				tmpPtr = tmpBuf+length;
			}

			if((length + 40) < 2048) 
			{
					if(g_dst_trunk[i-1].loadBanlcMode)
					length += sprintf(tmpPtr,"config trunk load-balance %s\n",trkLBalanc[g_dst_trunk[i-1].loadBanlcMode]);
					syslog_ax_trunk_dbg("%s\n",tmpPtr);
					tmpPtr = tmpBuf+length;
			}

			if(0 != g_dst_trunk[i-1].portLog)
			{
				if((length + 19) < 2048) {
					length += sprintf(tmpPtr,"config trunk %d\n",i);
					 syslog_ax_trunk_dbg("%s\n\n",tmpPtr);
					tmpPtr = tmpBuf + length;
				}
        		for(j=0; g_dst_trunk[i-1].portLog; j++)
        		{
					slot_no = g_dst_trunk[i-1].portmap[j].slot;
					local_port_no = g_dst_trunk[i-1].portmap[j].port;
					if((length + 14 )< 2048) {
						length += sprintf(tmpPtr,"  add port %d/%d\n",slot_no,local_port_no);
						 syslog_ax_trunk_dbg("%s",tmpPtr);
						tmpPtr = tmpBuf + length;
					}
					if(NPD_TRUE == g_dst_trunk[i-1].portmap[j].master){
						if((length + 21 )< 2048) {
							length += sprintf(tmpPtr,"  set master-port %d/%d\n",slot_no,local_port_no);
							 syslog_ax_trunk_dbg("%s",tmpPtr);
							tmpPtr = tmpBuf + length;
						}
					}
				}
				if((length + 4) < 2048) { /* exit port config(add,masterport,enDis,) */
					length += sprintf(tmpPtr,"exit\n");
					 syslog_ax_trunk_dbg("%s\n\n",tmpPtr);
					tmpPtr = tmpBuf + length;
				}					
			}

			/* syslog_ax_trunk_dbg((""));*/
			if((totalLen +length + 5 ) > bufLen) { /* 5 exit */
				 syslog_ax_trunk_err("save trunk buffer full\n");
				break;
			}
			 syslog_ax_trunk_dbg("tmpBuf Length = %d.\n",length);
			totalLen += sprintf(cursor,"%s",tmpBuf);
			cursor = showStr + totalLen;
		}
	}/* end of (for loop)*/
}

/*****************************************************************************************
 *		NPD dbus operation
 *
 *****************************************************************************************/
/*create trunk entity with trunkId,on Both Hw &Sw.*/
DBusMessage * npd_dbus_trunk_create_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	unsigned short	trunkId = 0;
	unsigned char*	name = NULL;
	unsigned int	ret = TRUNK_RETURN_CODE_ERR_NONE;
	DBusError err;

	
	if(!((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT16,&trunkId,
								 DBUS_TYPE_STRING,&name,
								 DBUS_TYPE_INVALID))) {
			 syslog_ax_trunk_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		 syslog_ax_trunk_dbg("create trunk %d,trunk name %s.\n",trunkId,name);
		if(!strncmp("dytrunk", name, 7)){
			ret = TRUNK_RETURN_CODE_NAME_CONFLICT;
		}
		else{
			ret = npd_trunk_activate(trunkId,name);
		}
	}
	if((ret == TRUNK_RETURN_CODE_ERR_NONE) && ((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I)))
	{
		ret = npd_dst_trunk_valid(trunkId,name);
	}
	
	if(TRUNK_RETURN_CODE_ERR_NONE == ret)
	{
		unsigned short vlanId = 1;
		unsigned char isAdd = 1,isTag=0;
		/* add trunk untag to default vlan */
        ret = npd_vlanlist_add_del_trunk(vlanId,trunkId,isAdd,isTag);
		if(0 != ret)
		{
    		syslog_ax_trunk_err("update trunk %d info to vlan %d error!!!\n",trunkId,vlanId);			
		    ret = TRUNK_RETURN_CODE_ERR_HW;			
		}
		else
		{
		    ret = TRUNK_RETURN_CODE_ERR_NONE;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
	return reply;
	
}

/*enter trunk configure node*/
DBusMessage * npd_dbus_trunk_config_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned short	trunkId = 0;
	unsigned int	ret = 0,op_ret = TRUNK_RETURN_CODE_ERR_NONE;
	DBusError err;
	
	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_UINT16,&trunkId,
			DBUS_TYPE_INVALID))) {
			 syslog_ax_trunk_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		 syslog_ax_trunk_dbg("get trunk Id %d from Dcli.\n",trunkId);
		/* call npd_check_trunk_exist, return tha trunk exist or NOT:*/
		ret = npd_check_trunk_exist(trunkId);
		if (TRUNK_RETURN_CODE_BADPARAM == ret ) {
			op_ret = TRUNK_RETURN_CODE_ERR_GENERAL;   /* exchange with the following one */
		}
		else if (TRUNK_RETURN_CODE_TRUNK_NOTEXISTS == ret){
			op_ret = ETHPORT_RETURN_CODE_NO_SUCH_TRUNK;
		}
		else if(TRUNK_RETURN_CODE_TRUNK_EXISTS == ret) {     /*23*/
			op_ret = TRUNK_RETURN_CODE_ERR_NONE;      
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &op_ret);
	return reply;
}

/*enter trunk configure node*/
DBusMessage * npd_dbus_trunk_config_by_name
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned short	trunkId = 0;
	char*	trunkName = NULL;
	struct trunk_s*	trunkNode = NULL;
	unsigned int	op_ret = TRUNK_RETURN_CODE_ERR_NONE;
	DBusError err;
	
	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_STRING,&trunkName,
			DBUS_TYPE_INVALID))) {
			 syslog_ax_trunk_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		 syslog_ax_trunk_dbg("get vlan name %s\n",trunkName);
		trunkNode = npd_find_trunk_by_name(trunkName);
		if(NULL == trunkNode) {
			op_ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
		}
		else {
			trunkId = trunkNode->trunkId;
			 syslog_ax_trunk_dbg("trunk %s 's Id %d\n",trunkName,trunkId);
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &op_ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT16,
									 &trunkId);
	
	return reply;
	
}

DBusMessage * npd_dbus_trunk_update_map_table
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;

	unsigned short	trunkId = 0;	
	unsigned int 	ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned char isAdd = 0,actdevNum = 0,virportNum = 0,endis = 0;
	unsigned long i = 0,numMembersPtr = 0;
	
	DBusError err;	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isAdd,
							DBUS_TYPE_UINT16,&trunkId,
							DBUS_TYPE_BYTE,&actdevNum,
							DBUS_TYPE_BYTE,&virportNum,
							DBUS_TYPE_BYTE,&endis,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	
	syslog_ax_trunk_dbg("isAdd = %d,trunkId = %d,actdevNum = %d,virportNum = %d,endis = %d\n",isAdd,trunkId,actdevNum,virportNum,endis);
	if((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I))
	{
		ret = nam_asic_trunk_map_table_update(0,trunkId,actdevNum,virportNum,isAdd,endis);
		if(TRUNK_RETURN_CODE_ERR_NONE != ret) 
		{
			syslog_ax_trunk_err("nam_asic_trunk_map_table_update failed ! ret %d\n",ret);
		}
		if(BOARD_TYPE_AX71_CRSMU == BOARD_TYPE)
		{
			ret = nam_asic_trunk_map_table_update(1,trunkId,actdevNum,virportNum,isAdd,endis);
			if(TRUNK_RETURN_CODE_ERR_NONE != ret) 
			{
				syslog_ax_trunk_err("nam_asic_trunk_map_table_update faile !\n");
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}



DBusMessage * npd_dbus_trunk_add_delete_port_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;

	unsigned char	isAdd = 0;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index = 0, local_port_index = 0;
	unsigned char is_same_board_port = 0,cscd_type = CSCD_TYPE;
	unsigned short	trunkId = 0;	
	unsigned int	eth_g_index = 0;
	unsigned int 	ret = TRUNK_RETURN_CODE_ERR_NONE;
	enum product_id_e productId = PRODUCT_ID;
	unsigned long i = 0,j = 0,numMembersPtr = 0;
	unsigned char actdevNum = 0,portNum = 0,endis = 0,devNum = 0;
	unsigned int devnum = 0,port_link_state = 0;
	int trunk_state = 0;
	unsigned char portLog = 0;
	int vlanId = 0;
	
	DBusError err;	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isAdd,
							DBUS_TYPE_BYTE,&slot_no,
							DBUS_TYPE_BYTE,&local_port_no,
							DBUS_TYPE_BYTE,&is_same_board_port,
							DBUS_TYPE_UINT16,&trunkId,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}	
	if((CSCD_TYPE != 1) && (is_same_board_port == 0) && (isAdd))
	{
		syslog_ax_trunk_err("System is not CSCD mode,do not allow to add port from different boards !\n");
		ret = TRUNK_RETURN_CODE_BASE;
	}
	else
	{
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				local_port_index = ETH_LOCAL_NO2INDEX(slot_index, local_port_no);			
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,local_port_index);
				if(npd_eth_port_rgmii_type_check(slot_no,local_port_no)) {
					syslog_ax_trunk_dbg("port(%d %d) index %d do not allow adding to trunk.\n",slot_no,    \
														local_port_no,eth_g_index);
					ret = TRUNK_RETURN_CODE_UNSUPPORT;
				}
				if(TRUNK_RETURN_CODE_ERR_NONE == ret) {
					if(isAdd){
						ret = npd_trunk_port_add(trunkId,eth_g_index);
						if(TRUNK_RETURN_CODE_ERR_NONE == ret) {
							ret = TRUNK_RETURN_CODE_ERR_NONE;
							/*delete fdb port entries*/
							nam_fdb_table_delete_entry_with_port(eth_g_index);
						}
					}
					else{
						ret = npd_trunk_port_del(trunkId,eth_g_index);
						if(TRUNK_RETURN_CODE_ERR_NONE == ret) {
							ret = TRUNK_RETURN_CODE_ERR_NONE;
							/*check trunk link state*/
							trunk_state = npd_check_trunk_status(trunkId);
							if(TRUNK_STATE_DOWN_E == trunk_state){/*delete all trunk fdb items*/
		                       nam_fdb_table_delete_entry_with_trunk(trunkId);
							}
						}
					}
				}
			}
			else {
				ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
			}
		}
		else {
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		if(((IS_DISTRIBUTED == SYSTEM_TYPE) || (PRODUCT_ID == PRODUCT_ID_AX7K_I)) && (ret == TRUNK_RETURN_CODE_ERR_NONE))
		{
			ret = npd_get_port_link_status(eth_g_index,&port_link_state);
			if(TRUNK_RETURN_CODE_ERR_NONE != ret) 
			{
				syslog_ax_trunk_err("get port link status FAIL !\n");
			}
			else
			{
				if(ETH_ATTR_LINKUP == port_link_state) {
					endis = NPD_TRUE;
				}
				else {
					endis = NPD_FALSE;	
				}
			}
			ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
			if (ret != TRUNK_RETURN_CODE_ERR_NONE) 
			{
				syslog_ax_trunk_err("global_index %d convert to devPort error.\n",eth_g_index);
			}
			actdevNum = g_dis_dev[0];
			syslog_ax_trunk_dbg("actdevNum = %d\n",actdevNum);
		}

		/*update global trunk array*/

		if(ret == TRUNK_RETURN_CODE_ERR_NONE)
		{
			portLog = g_dst_trunk[trunkId-1].portLog;
			if(isAdd)/*add port*/
			{
				g_dst_trunk[trunkId-1].portLog  = portLog + 1;
				g_dst_trunk[trunkId-1].portmap[portLog].enable = endis;
				g_dst_trunk[trunkId-1].portmap[portLog].slot = slot_no;
				g_dst_trunk[trunkId-1].portmap[portLog].port = local_port_no;
				g_dst_trunk[trunkId-1].dst_port_map[portLog].devnum = actdevNum;
				g_dst_trunk[trunkId-1].dst_port_map[portLog].virportnum = portNum;

				if(g_dst_trunk[trunkId-1].portLog == 1)/*if is the first port set master port*/
				{
					g_dst_trunk[trunkId-1].portmap[portLog].master = 1;
				}
			}
			else/*delete port*/
			{
				for(vlanId=1;vlanId<=4094;vlanId++)
				{
					if(g_vlanlist[vlanId-1].tagTrkBmp[trunkId-1] == 1)/*trunk is tag of vlan*/
					{
						if((local_port_no <= 32)&&((g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp)&(1<<(local_port_no-1))))/*if trunk port is enable for qinq*/
						{
							/*disable the specific port on the specific slot*/
							ret = npd_vlan_qinq_endis(vlanId, eth_g_index, 0);
							if(ret != NPD_SUCCESS)
							{
								npd_syslog_err("set slot %d port %d qinq disable FAIL !",slot_no,local_port_no);
							}
							else
							{
								npd_syslog_dbg("set vlan %d port %d-%d qinq disable SUCCESS\n",vlanId,slot_no,local_port_no);
								g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
							}
							
						}
						else if((local_port_no>32) && ((g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp)&(1<<(local_port_no-33))))
						{
							ret = npd_vlan_qinq_endis(vlanId, eth_g_index, 0);
							if(ret != NPD_SUCCESS)
							{
								npd_syslog_err("set slot %d port %d qinq disable FAIL !",slot_no,local_port_no);
							}
							else
							{
								npd_syslog_dbg("set vlan %d port %d-%d qinq disable SUCCESS\n",vlanId,slot_no,local_port_no);
								g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
							}
						}
					}
				}
				g_dst_trunk[trunkId-1].portLog = portLog - 1;
				for(i=0;i<portLog;i++)
				{
					if((g_dst_trunk[trunkId-1].portmap[i].slot == slot_no) && ((g_dst_trunk[trunkId-1].portmap[i].port == local_port_no)))
					{
						for(j=i;j<TRUNK_MEMBER_NUM_MAX-1;j++)
						{
							g_dst_trunk[trunkId-1].portmap[j].enable = g_dst_trunk[trunkId-1].portmap[j+1].enable;
							g_dst_trunk[trunkId-1].portmap[j].slot = g_dst_trunk[trunkId-1].portmap[j+1].slot;
							g_dst_trunk[trunkId-1].portmap[j].port = g_dst_trunk[trunkId-1].portmap[j+1].port;
							g_dst_trunk[trunkId-1].dst_port_map[j].devnum = g_dst_trunk[trunkId-1].dst_port_map[j+1].devnum;
							g_dst_trunk[trunkId-1].dst_port_map[j].virportnum = g_dst_trunk[trunkId-1].dst_port_map[j+1].virportnum;
						}
						g_dst_trunk[trunkId-1].portmap[TRUNK_MEMBER_NUM_MAX-1].enable = 0;
						g_dst_trunk[trunkId-1].portmap[TRUNK_MEMBER_NUM_MAX-1].slot = 0;
						g_dst_trunk[trunkId-1].portmap[TRUNK_MEMBER_NUM_MAX-1].port = 0;
						g_dst_trunk[trunkId-1].dst_port_map[TRUNK_MEMBER_NUM_MAX-1].devnum = 0;
						g_dst_trunk[trunkId-1].dst_port_map[TRUNK_MEMBER_NUM_MAX-1].virportnum = 0;
						break;
					}
				}
			}

			for(i=0;i<g_dst_trunk[trunkId-1].portLog;i++)/*update trunk state*/
			{
				if(g_dst_trunk[trunkId-1].portmap[i].enable)
				{
					g_dst_trunk[trunkId-1].tLinkState = TRUNK_STATE_UP_E;
					break;
				}
				else
				{
					g_dst_trunk[trunkId-1].tLinkState = TRUNK_STATE_DOWN_E;
				}
			}
			

			ret = msync(g_dst_trunk, sizeof(dst_trunk_s)*127, MS_SYNC);
			if( ret!=0 )
		    {
		        syslog_ax_trunk_err("msync shm_trunk failed \n" ); 
				ret = TRUNK_RETURN_CODE_ERR_GENERAL;
		    }
			else
			{
				ret = TRUNK_RETURN_CODE_ERR_NONE;

				/* add for update the g_vlanlist[] of distributed */
		        if(isAdd==1)
		        {   
		            /* remove the port from all vlan */
		    		for (vlanId=1;vlanId<=4094;vlanId++)
		            {
		                if(g_vlanlist[vlanId-1].vlanStat == 1)
		        		{			
		                    if(local_port_no>32)
		                    {
		            			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));		/* remove from all vlan */		
		            			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));			
		                    }
		            		else
		            		{
		            			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));		/* remove from all vlan */			
		            			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
		            		}
		                }
		    		}
		        }
		    	else
		    	{

		            if(local_port_no>32)
		            {
		    			g_vlanlist[0].untagPortBmp[slot_no-1].high_bmp |= (unsigned int)((1<<(local_port_no-33)));		/* add to default vlan  */		
		            }
		    		else
		    		{
		    	        g_vlanlist[0].untagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));		/* add to default vlan */	
		    		}
		    	}

		    	ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
		    	if( ret!=0 )
		        {
		            syslog_ax_trunk_err("msync shm_vlan failed \n" );
		    		ret = TRUNK_RETURN_CODE_ERR_GENERAL;
		        }
		    	else
		    	{
		    		ret = TRUNK_RETURN_CODE_ERR_NONE;
		    	}
			}
			syslog_ax_trunk_dbg("set trunk %d invalid ok\n",trunkId);
		}
		syslog_ax_trunk_dbg("devNum = %d,actdevNum = %d,portNum = %d,endis = %d\n",devNum,actdevNum,portNum,endis);
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_BYTE,&actdevNum);
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_BYTE,&portNum);
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_BYTE,&endis);
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_BYTE,&cscd_type);
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&ret);

	return reply;
}
DBusMessage * npd_dbus_trunk_delete_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned short	trunkId = 0;
	struct trunk_s	*trunkNode = NULL;
	unsigned int	ret = TRUNK_RETURN_CODE_ERR_NONE;
	DBusError err;
	
	 syslog_ax_trunk_dbg("Entering delete trunk one!\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&trunkId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	trunkNode = npd_find_trunk(trunkId);
	if(NULL == trunkNode) {
		ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
		reply = dbus_message_new_method_return(msg);
		
		dbus_message_iter_init_append (reply, &iter);
		
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32, 
										 &ret);
		return reply;
		
	}
	syslog_ax_trunk_dbg("delete trunk entry %d.\n",trunkId);
	ret = npd_trunk_destroy_node(trunkId);
	if(TRUNK_RETURN_CODE_ERR_NONE == ret){ /*no use if struct*/
		ret = TRUNK_RETURN_CODE_ERR_NONE;
		/*delete trunk fdb entries*/
		nam_fdb_table_delete_entry_with_trunk(trunkId);
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
	
	return reply;
	
}


DBusMessage * npd_dbus_trunk_master_port_set
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index = 0, local_port_index = 0;
	unsigned short	trunkId = 0;	

	unsigned int	eth_g_index = 0;
	unsigned int 	ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned int i = 0;
	DBusError err;
	/*enum module_id_e module_type;*/
	
	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_BYTE,&slot_no,
								DBUS_TYPE_BYTE,&local_port_no,
								DBUS_TYPE_UINT16,&trunkId,
								DBUS_TYPE_INVALID))) {
			syslog_ax_trunk_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				local_port_index = ETH_LOCAL_NO2INDEX(slot_index, local_port_no);
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,local_port_index);

				ret = npd_trunk_master_port_set(trunkId,eth_g_index);
			}
			else {
				syslog_ax_trunk_err("legal slot %d\n",slot_no);			
				ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
			}
		}
		else {
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
	}
	if(ret == TRUNK_RETURN_CODE_ERR_NONE)/*update the g_dst_trunk */
	{
		for(i=0;i<g_dst_trunk[trunkId-1].portLog;i++)
		{
			if((g_dst_trunk[i].portmap[i].slot == slot_no) && ((g_dst_trunk[i].portmap[i].port == local_port_no)))
			{
				g_dst_trunk[i].portmap[i].master = 1;
			}
			else
			{
				g_dst_trunk[i].portmap[i].master = 0;
			}
		}

		ret = msync(g_dst_trunk, sizeof(dst_trunk_s)*127, MS_SYNC);
		if( ret!=0 )
	    {
	        syslog_ax_trunk_err("msync shm_trunk failed \n" ); 
	    }		
		syslog_ax_trunk_dbg("update g_dst_trunk[] success !\n");
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}

/*******************************
 *
 *
 ******************************/

DBusMessage * npd_dbus_trunk_port_endis
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index = 0, local_port_index = 0, en_dis=0;
	unsigned short	trunkId = 0;	

	unsigned int	eth_g_index=0, ret = TRUNK_RETURN_CODE_ERR_NONE;

	DBusError err;
	/*enum module_id_e module_type;*/
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&slot_no,
							DBUS_TYPE_BYTE,&local_port_no,
							DBUS_TYPE_UINT16,&trunkId,
							DBUS_TYPE_BYTE,&en_dis,
							DBUS_TYPE_INVALID))) {
		syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
			slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
			local_port_index = ETH_LOCAL_NO2INDEX(slot_index, local_port_no);
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,local_port_index);

			ret = npd_trunk_port_endis(trunkId,eth_g_index,en_dis);
			if(TRUNK_RETURN_CODE_ERR_NONE == ret) {
				ret = TRUNK_RETURN_CODE_ERR_NONE;
			}
		}
		else {
			syslog_ax_trunk_err("legal slot %d\n",slot_no);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
	}
	else {
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}


/*******************************
 *
 *
 ******************************/

DBusMessage * npd_dbus_trunk_load_banlc_config
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int	loadBanlc = LOAD_BANLC_MAX;
	unsigned short	trunkId = 0;	
	unsigned int	ret = TRUNK_RETURN_CODE_ERR_NONE,iret = 0;
	unsigned int devCnt;
	unsigned char devNum = 0;
	DBusError err;
	
	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE))
	{
		npd_syslog_dbg("do not support trunk!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TRUNK_VLAUE))
	{
		npd_syslog_dbg("do not support trunk!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else 
	{
		if (!(dbus_message_get_args ( msg, &err,
								/*DBUS_TYPE_UINT16,&trunkId,*/
								DBUS_TYPE_UINT32,&loadBanlc,
								DBUS_TYPE_INVALID))) 
		{
			syslog_ax_trunk_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) 
			{
				syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		syslog_ax_trunk_dbg("get parameter,loadBalanc %d.\n",loadBanlc);
		devCnt = nam_asic_get_instance_num();
		for(devNum = 0; devNum < devCnt; devNum++)
		{
			ret = nam_asic_trunk_load_balanc_set(devNum,0,loadBanlc);
			if(ret != TRUNK_RETURN_CODE_ERR_NONE)
			{
				syslog_ax_trunk_err("set trunk load balance failed !\n");
			}
		}

		if(ret == TRUNK_RETURN_CODE_ERR_NONE)
		{
			g_loadBalanc = loadBanlc;
			for(trunkId = 1;trunkId <128;trunkId++)
			{	
				iret = npd_check_trunk_exist(trunkId);
				if(TRUNK_RETURN_CODE_TRUNK_EXISTS == iret)
				{
					iret = npd_trunk_load_balanc_set(trunkId,loadBanlc);
					if(iret != TRUNK_RETURN_CODE_ERR_NONE)
					{
						syslog_ax_trunk_err("set trunk load balance on SW failed\n");
					}
				}
				else
				{
					continue;
				}
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}

/*allow/refuse on vlan list*/
DBusMessage * npd_dbus_trunk_allow_refuse_vlan
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char  isAllow = 0, tagMode = 0;
	unsigned short trunkId = 0;
	static unsigned short vid[NPD_MAX_VLAN_ID] = {0}, nonVid[NPD_MAX_VLAN_ID] = {0};
	unsigned int i = 0, vCount = 0, nonVCount = 0;
	unsigned int ret = TRUNK_RETURN_CODE_ERR_NONE;
	
	syslog_ax_trunk_dbg("Entering trunk allow/refuse vlans..\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isAllow,
							DBUS_TYPE_BYTE,&tagMode,
							DBUS_TYPE_UINT32,&vCount,
							DBUS_TYPE_UINT16,&vid,
							DBUS_TYPE_UINT16,&trunkId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	for(i=0;i<vCount;i++){
		ret = npd_check_vlan_exist(vid[i]);
		if(VLAN_RETURN_CODE_VLAN_EXISTS != ret){
			nonVid[nonVCount++] = vid[i];
			continue;
		}
	}
	if(nonVCount>0) {
		ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;

		reply = dbus_message_new_method_return(msg);
		
		dbus_message_iter_init_append (reply, &iter);
		
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32, 
										 &ret);

		return reply;
	}
	#if 0
	for (i=0;i<vCount;i++){
		ret = npd_vlan_interface_check(vid[i], &ifIndex);
		if(NPD_TRUE == ret && ifIndex !=(~0UI) ) {
			nonVid[nonVCount++] = vid[i];
			continue;
		}
	}
	if(nonVCount > 0){
		ret = NPD_VLAN_L3_INTF;

		reply = dbus_message_new_method_return(msg);
		
		dbus_message_iter_init_append (reply, &iter);
		
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_UINT32, 
										 &ret);
		return reply;
	}
	#endif
	/*no need to check trunk existing or NOT*/
	{
		if(isAllow)
			ret = npd_trunk_allow_vlan(trunkId,vCount,vid,tagMode);
		else
			ret = npd_trunk_refuse_vlan(trunkId,vCount,vid,tagMode);
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
	return reply;
	
}
DBusMessage * npd_dbus_trunk_delete_by_name
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned short	trunkId =0;
	char*			trunkName = NULL;
	struct trunk_s*	trunkNode = NULL;
	unsigned int	ret = TRUNK_RETURN_CODE_ERR_NONE;
	DBusError err;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_STRING,&trunkName,
		DBUS_TYPE_INVALID))) {
		syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	if(!strncmp("dytrunk", trunkName, 7)){
		ret = TRUNK_RETURN_CODE_NAME_CONFLICT;
	}
	else{
		trunkNode = npd_find_trunk_by_name(trunkName);
		if(NULL == trunkNode) {
			ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
		}
	}
	if(TRUNK_RETURN_CODE_ERR_NONE == ret){
		trunkId = trunkNode->trunkId;
		ret = npd_trunk_destroy_node(trunkId);
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
	
	return reply;
}

/*show a special trunk member slot_port*/
DBusMessage * npd_dbus_trunk_show_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply = NULL;
	DBusMessageIter	iter;
	DBusError err;
	unsigned short	trunkId =0;
	char*			trunkName = NULL;
	struct trunk_s	*trunkNode;
	unsigned int 	m_eth_index = 0, slot_index = 0, port_index = 0, op_ret = 0 ;
	unsigned char 	mstFlag = 0;
	unsigned int 	mSlotNo = 0, mPortNo = 0;
	enum product_id_e productId = PRODUCT_ID;
	unsigned int 	mbrBmp_sp = 0, disMbrBmp_sp = 0, loadBalanc = 0x0f;
	unsigned int 	trkLinkStat = 0;

	trunkName = (char*)malloc(NPD_TRUNK_IFNAME_SIZE+1);
	if(NULL == trunkName) {
		return reply;
	}
	memset(trunkName,0,NPD_TRUNK_IFNAME_SIZE+1);

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&trunkId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		free(trunkName);
		trunkName = NULL;
		return NULL;
	}
	 syslog_ax_trunk_dbg("show trunk %d. \n",trunkId);
	if(TRUNK_RETURN_CODE_TRUNK_NOTEXISTS == npd_check_trunk_exist(trunkId)) {
		op_ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
		 syslog_ax_trunk_dbg("trunkId =%d NOT exists.\n",trunkId);
	}
	else {
		trunkNode = npd_find_trunk(trunkId);
		if(NULL == trunkNode) {
			syslog_ax_trunk_dbg("find trunk on Sofeware error!\n");
    		free(trunkName);
    		trunkName = NULL;
    		return NULL;			 
		}
		/* trunkName */
		 syslog_ax_trunk_dbg("find trunkNode %p\n",trunkNode);
		memcpy(trunkName,trunkNode->trunkName, ALIAS_NAME_SIZE);
		loadBalanc = trunkNode->loadBanlcMode;
		trkLinkStat = trunkNode->tLinkState;
		/*find out trunk ports*/
		 syslog_ax_trunk_dbg("trunkName %s,load-balance %d.\n",trunkName,loadBalanc);
		if(0 != trunkNode->portList){
			op_ret = npd_trunk_get_mbr_bmp_via_sw(trunkId,&mbrBmp_sp,&disMbrBmp_sp);
			if(NPD_SUCCESS != op_ret ) {
				syslog_ax_trunk_err("get trunk %d port bitmap error\n",trunkId);
				op_ret = TRUNK_RETURN_CODE_ERR_GENERAL;
			}
		}
		/*find out allowd vlan*/

		/*get trunk master port*/
		op_ret = npd_trunk_master_port_get(trunkId,&m_eth_index);
		if(NPD_TRUE == op_ret){
			mstFlag = 1;
			/*NPD_DEBUG(("trunk %d master port index = %d.",trunkId,m_eth_index));*/
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
			mSlotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
			port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
			mPortNo = ETH_LOCAL_INDEX2NO(slot_index, port_index);
			/*NPD_DEBUG(("trunk %d master port :slot = %d,port = %d.",trunkId,mSlotNo,mPortNo));*/
			op_ret = TRUNK_RETURN_CODE_ERR_NONE;
		}
		
	}

	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&op_ret);
	/*syslog_ax_trunk_dbg("dbus_message_iter_append_basic:return op_ret %d\n",op_ret);*/
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_STRING,&trunkName);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&productId);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_BYTE,&mstFlag);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mSlotNo);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mPortNo);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mbrBmp_sp);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&disMbrBmp_sp);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&loadBalanc);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&trkLinkStat);	
	free(trunkName);
	trunkName = NULL;
	return reply;
}

/*show a special trunk member slot_port*/
DBusMessage * npd_dbus_trunk_show_one_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply = NULL;
	DBusMessageIter	iter;
	DBusError err;
	unsigned short	trunkId =0;
	char*			trunkName = NULL;
	struct trunk_s	*trunkNode;
	unsigned int 	m_eth_index = 0, slot_index = 0, port_index = 0, op_ret = 0 ;
	unsigned char 	mstFlag = 0;
	unsigned int 	mSlotNo = 0, mPortNo = 0;
	enum product_id_e productId = PRODUCT_ID;
	unsigned int 	loadBalanc = 0x0f;
	unsigned int 	trkLinkStat = 0;
    PORT_MEMBER_BMP mbrBmp_sp,disMbrBmp_sp;
	unsigned int slot_id;
	slot_id = SLOT_ID;
	memset(&mbrBmp_sp,0,sizeof(PORT_MEMBER_BMP));
	memset(&disMbrBmp_sp,0,sizeof(PORT_MEMBER_BMP));
	trunkName = (char*)malloc(NPD_TRUNK_IFNAME_SIZE+1);
	if(NULL == trunkName) {
		return reply;
	}
	memset(trunkName,0,NPD_TRUNK_IFNAME_SIZE+1);

	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_UINT16,&trunkId,
			DBUS_TYPE_INVALID))) {
			 syslog_ax_trunk_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			free(trunkName);	
			trunkName = NULL;
			return NULL;
		}
		 syslog_ax_trunk_dbg("show trunk %d. \n",trunkId);
		if(TRUNK_RETURN_CODE_TRUNK_NOTEXISTS == npd_check_trunk_exist(trunkId)) {
			op_ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
			 syslog_ax_trunk_dbg("trunkId =%d NOT exists.\n",trunkId);
		}
		else {
			trunkNode = npd_find_trunk(trunkId);
			if(NULL == trunkNode) {
				syslog_ax_trunk_dbg("find trunk on Sw Error!\n");
    			free(trunkName);	/* code optimize: Dereference after null check. zhangdi@autelan.com 2013-01-18 */
    			trunkName = NULL;
    			return NULL;				 
			}
			/* trunkName */
			 syslog_ax_trunk_dbg("find trunkNode %p\n",trunkNode);
			memcpy(trunkName,trunkNode->trunkName, ALIAS_NAME_SIZE);
			loadBalanc = trunkNode->loadBanlcMode;
			trkLinkStat = trunkNode->tLinkState;
			/*find out trunk ports*/
			 syslog_ax_trunk_dbg("trunkName %s,load-balance %d.\n",trunkName,loadBalanc);
			if(0 != trunkNode->portList){
				op_ret = npd_trunk_get_mbr_bmp_via_sw_v1(trunkId,&mbrBmp_sp,&disMbrBmp_sp);
				if(NPD_SUCCESS != op_ret ) {
					syslog_ax_trunk_err("get trunk %d port bitmap error\n",trunkId);
					op_ret = TRUNK_RETURN_CODE_ERR_GENERAL;
				}
			}
			/*find out allowd vlan*/

			/*get trunk master port*/
			op_ret = npd_trunk_master_port_get(trunkId,&m_eth_index);
			if(NPD_TRUE == op_ret){
				mstFlag = 1;
				/*NPD_DEBUG(("trunk %d master port index = %d.",trunkId,m_eth_index));*/
				slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
				mSlotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
				port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
				mPortNo = ETH_LOCAL_INDEX2NO(slot_index, port_index);
				/*NPD_DEBUG(("trunk %d master port :slot = %d,port = %d.",trunkId,mSlotNo,mPortNo));*/
				op_ret = TRUNK_RETURN_CODE_ERR_NONE;
			}
			
		}
	}

	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&op_ret);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_STRING,&trunkName);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&productId);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&slot_id);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_BYTE,&mstFlag);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mSlotNo);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mPortNo);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mbrBmp_sp.portMbr[0]);
	dbus_message_iter_append_basic(&iter,
							    DBUS_TYPE_UINT32,&mbrBmp_sp.portMbr[1]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&disMbrBmp_sp.portMbr[0]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&disMbrBmp_sp.portMbr[1]);	
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&loadBalanc);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&trkLinkStat);	
	free(trunkName);
	trunkName = NULL;
	return reply;
}
/*show a special trunk member slot_port*/
DBusMessage * npd_dbus_trunk_show_by_name
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply;
	DBusMessageIter	iter;
	DBusError err;
	unsigned short	trunkId = 0;
	char*			trunkName = NULL;
	struct trunk_s* trunkNode = NULL;
	unsigned int	op_ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned int 	slot_index = 0, port_index = 0, m_eth_index = 0;
	unsigned char 	mstFlag = 0;
	unsigned int 	mSlotNo = 0, mPortNo = 0;
	enum product_id_e productId = PRODUCT_ID;
	unsigned int 	mbrBmp_sp = 0, disMbrBmp_sp = 0, loadBalanc = 0x0f;
	unsigned int 	trkLinkStat = 0;
	
	 syslog_ax_trunk_dbg("Entering show trunk port members!\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_STRING,&trunkName,
								DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	 syslog_ax_trunk_dbg("show trunk %s. \n",trunkName);
	trunkNode = npd_find_trunk_by_name(trunkName);
	if(NULL == trunkNode){
		op_ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
	}
	else {
		trunkId = trunkNode->trunkId;
		loadBalanc = trunkNode->loadBanlcMode;
		trkLinkStat = trunkNode->tLinkState;
		 syslog_ax_trunk_dbg("trunkNode->trunkId = %d\n",trunkId);
		if(0 != trunkNode->portList){
			op_ret = npd_trunk_get_mbr_bmp_via_sw(trunkId,&mbrBmp_sp,&disMbrBmp_sp);
			if(NPD_SUCCESS != op_ret ) {
				syslog_ax_trunk_err("get trunk %d port bitmap error\n",trunkId);
				op_ret = TRUNK_RETURN_CODE_ERR_GENERAL;
			}
		}
		
		/*get trunk master port*/
		op_ret = npd_trunk_master_port_get(trunkId,&m_eth_index);
		if( NPD_TRUE == op_ret){
			mstFlag = 1;
			/*NPD_DEBUG(("trunk %d master port index = %d.",trunkId,m_eth_index));*/
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
			mSlotNo= CHASSIS_SLOT_INDEX2NO(slot_index);
			port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
			mPortNo = ETH_LOCAL_INDEX2NO(slot_index, port_index);
			/*NPD_DEBUG(("trunk %d master port :slot = %d,port = %d.",mSlotNo,mPortNo));*/
			op_ret = TRUNK_RETURN_CODE_ERR_NONE;
		}
	}

	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&op_ret);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT16,&trunkId);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&productId);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_BYTE,&mstFlag);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mSlotNo);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mPortNo);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mbrBmp_sp);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&disMbrBmp_sp);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&loadBalanc);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&trkLinkStat);	
	return reply;
}

/*show a special trunk member slot_port*/
DBusMessage * npd_dbus_trunk_show_by_name_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply;
	DBusMessageIter	iter;
	DBusError err;
	unsigned short	trunkId = 0;
	char*			trunkName = NULL;
	struct trunk_s* trunkNode = NULL;
	unsigned int	op_ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned int 	slot_index = 0, port_index = 0, m_eth_index = 0;
	unsigned char 	mstFlag = 0;
	unsigned int 	mSlotNo = 0, mPortNo = 0;
	enum product_id_e productId = PRODUCT_ID;
	unsigned int 	loadBalanc = 0x0f;
	unsigned int 	trkLinkStat = 0;
	PORT_MEMBER_BMP mbrBmp_sp,disMbrBmp_sp;
	unsigned int slot_id;
	slot_id = SLOT_ID;
	memset(&mbrBmp_sp,0,sizeof(PORT_MEMBER_BMP));
	memset(&disMbrBmp_sp,0,sizeof(PORT_MEMBER_BMP));
	 syslog_ax_trunk_dbg("Entering show trunk port members!\n");
	
	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_STRING,&trunkName,
									DBUS_TYPE_INVALID))) {
			 syslog_ax_trunk_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		
		
		 syslog_ax_trunk_dbg("show trunk %s. \n",trunkName);
		trunkNode = npd_find_trunk_by_name(trunkName);
		if(!strncmp("dytrunk", trunkName, 7)){
			op_ret = TRUNK_RETURN_CODE_NAME_CONFLICT;
		}
		else if(NULL == trunkNode){
			op_ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
		}
		else {
			trunkId = trunkNode->trunkId;
			loadBalanc = trunkNode->loadBanlcMode;
			trkLinkStat = trunkNode->tLinkState;
			 syslog_ax_trunk_dbg("trunkNode->trunkId = %d\n",trunkId);
			if(0 != trunkNode->portList){
				op_ret = npd_trunk_get_mbr_bmp_via_sw_v1(trunkId,&mbrBmp_sp,&disMbrBmp_sp);
				if(NPD_SUCCESS != op_ret ) {
					syslog_ax_trunk_err("get trunk %d port bitmap error\n",trunkId);
					op_ret = TRUNK_RETURN_CODE_ERR_GENERAL;
				}
			}
			
			/*get trunk master port*/
			op_ret = npd_trunk_master_port_get(trunkId,&m_eth_index);
			if( NPD_TRUE == op_ret){
				mstFlag = 1;
				/*NPD_DEBUG(("trunk %d master port index = %d.",trunkId,m_eth_index));*/
				slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
				mSlotNo= CHASSIS_SLOT_INDEX2NO(slot_index);
				port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
				mPortNo = ETH_LOCAL_INDEX2NO(slot_index, port_index);
				/*NPD_DEBUG(("trunk %d master port :slot = %d,port = %d.",mSlotNo,mPortNo));*/
				op_ret = TRUNK_RETURN_CODE_ERR_NONE;
			}
		}
	}

	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&op_ret);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT16,&trunkId);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&productId);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&slot_id);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_BYTE,&mstFlag);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mSlotNo);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mPortNo);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mbrBmp_sp.portMbr[0]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&mbrBmp_sp.portMbr[1]);	
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&disMbrBmp_sp.portMbr[0]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&disMbrBmp_sp.portMbr[1]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&loadBalanc);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&trkLinkStat);	
	return reply;
}


/*original*/
DBusMessage * npd_dbus_trunk_show_trunklist_port_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	DBusError err;
	actTrunkList	*acttrunkList[CHASSIS_TRUNK_RANGE_MAX];
	unsigned int	trunk_count = 0;
	unsigned int	i = 0, ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned short	trunkId = 0;
	char*			trunkName = NULL;
	struct trunk_s*	trunkNode = NULL;
	unsigned int slot_index = 0, port_index = 0, m_eth_index = 0;	
	enum product_id_e productId = PRODUCT_ID;
	unsigned int mbrBmp_sp = 0;
	unsigned int disMbrBmp_sp = 0;

	dbus_error_init(&err);

	for (trunkId=1;trunkId<=CHASSIS_TRUNK_RANGE_MAX;trunkId++){
		ret = npd_check_trunk_exist(trunkId);
		if(TRUNK_RETURN_CODE_TRUNK_NOTEXISTS ==ret) {
			ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
			continue;
		}
		else if (TRUNK_RETURN_CODE_TRUNK_EXISTS == ret) {
			/*trunk_count++;*/
			trunkNode = npd_find_trunk(trunkId);
			trunkName = (char*)(trunkNode->trunkName);
			 syslog_ax_trunk_dbg("To show trunk %s\n",trunkName);
			acttrunkList[trunk_count] = (actTrunkList* )malloc(sizeof(actTrunkList));
			memset(acttrunkList[trunk_count],0,sizeof(actTrunkList));
			acttrunkList[trunk_count]->trunkId = trunkId;
			acttrunkList[trunk_count]->trunkName = (char*)(trunkNode->trunkName);
			acttrunkList[trunk_count]->loadBalanc = trunkNode->loadBanlcMode;
			acttrunkList[trunk_count]->tLinkStatus = trunkNode->tLinkState;
			if(0 != trunkNode->portList) {
				ret = npd_trunk_get_mbr_bmp_via_sw(trunkId,\
										&(mbrBmp_sp),\
										&(disMbrBmp_sp));
			if(NPD_SUCCESS != ret ) {
				syslog_ax_trunk_err("get trunk %d port bitmap error ret %d\n",trunkId,ret);
				ret = TRUNK_RETURN_CODE_ERR_GENERAL;
			}
				/*get trunk master port*/
				ret = npd_trunk_master_port_get(trunkId,&m_eth_index);
				if(NPD_TRUE == ret){
					acttrunkList[trunk_count]->masterFlag = 1; 
					slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
					port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
					acttrunkList[trunk_count]->mSlotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
					acttrunkList[trunk_count]->mPortno = ETH_LOCAL_INDEX2NO(slot_index, port_index);
					/* NPD_DEBUG(("trunk %d master port :slot = %d,port = %d.",trunkId,\
								acttrunkList[trunk_count]->mSlotNo,\
								acttrunkList[trunk_count]->mPortno));*/
					ret = TRUNK_RETURN_CODE_ERR_NONE;
				}
			}
			trunk_count++;
		}
	}
	if (trunk_count > 0) {
		ret = TRUNK_RETURN_CODE_ERR_NONE;
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	/* Total active trunk count*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &trunk_count);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);
	/*open outer container*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING  /*begin*/
											DBUS_TYPE_UINT16_AS_STRING /*trunkId*/
											DBUS_TYPE_STRING_AS_STRING /*trunkname*/
											DBUS_TYPE_BYTE_AS_STRING	/*masterFlag*/
											DBUS_TYPE_UINT32_AS_STRING   /*master slotNo*/
											DBUS_TYPE_UINT32_AS_STRING   /*master portNo*/
										   	DBUS_TYPE_UINT32_AS_STRING /*mbrBmp_sp*/
										   	DBUS_TYPE_UINT32_AS_STRING /*disMbrBmp_sp*/
										   	DBUS_TYPE_UINT32_AS_STRING /*loadBalance*/
										    DBUS_TYPE_UINT32_AS_STRING /*trkLinkStat*/
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	for (i = 0; i < trunk_count; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		 syslog_ax_trunk_dbg("acttrunkList[%d] trunkId=%d, masterPortFlag =%d,masterPort =%d/%d,portBmp0=%#0x\n",i,	\
															acttrunkList[i]->trunkId,\
															acttrunkList[i]->masterFlag,\
															acttrunkList[i]->mSlotNo,\
															acttrunkList[i]->mPortno,\
															mbrBmp_sp);
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT16,
				  &(acttrunkList[i]->trunkId));
		dbus_message_iter_append_basic 
				(&iter_struct,
				 DBUS_TYPE_STRING,
				 &(acttrunkList[i]->trunkName));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(acttrunkList[i]->masterFlag));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->mSlotNo));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->mPortno));
		
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(mbrBmp_sp));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(disMbrBmp_sp));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->loadBalanc));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->tLinkStatus));		

		dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}

/*original*/
DBusMessage * npd_dbus_trunk_show_trunklist_port_member_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	DBusError err;
	actTrunkList	*acttrunkList[MAX_STATIC_TRUNK_ID] = {NULL};
	unsigned int	trunk_count = 0;
	unsigned int	i = 0, ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned short	trunkId = 0;
	unsigned int slot_id = 0;
	char*			trunkName = NULL;
	struct trunk_s*	trunkNode = NULL;
	unsigned int slot_index = 0, port_index = 0, m_eth_index = 0;	
	enum product_id_e productId = PRODUCT_ID;
	slot_id = SLOT_ID;

	dbus_error_init(&err);

	for (trunkId=1;trunkId<= MAX_STATIC_TRUNK_ID;trunkId++){
		ret = npd_check_trunk_exist(trunkId);
		if(TRUNK_RETURN_CODE_TRUNK_NOTEXISTS ==ret) {
			ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
			continue;
		}
		else if (TRUNK_RETURN_CODE_TRUNK_EXISTS == ret) {
			/*trunk_count++;*/
			trunkNode = npd_find_trunk(trunkId);
			trunkName = (char*)(trunkNode->trunkName);
			 syslog_ax_trunk_dbg("To show trunk %s\n",trunkName);
			acttrunkList[trunk_count] = (actTrunkList* )malloc(sizeof(actTrunkList));
			memset(acttrunkList[trunk_count],0,sizeof(actTrunkList));
			acttrunkList[trunk_count]->trunkId = trunkId;
			acttrunkList[trunk_count]->trunkName = (char*)(trunkNode->trunkName);
			acttrunkList[trunk_count]->loadBalanc = trunkNode->loadBanlcMode;
			acttrunkList[trunk_count]->tLinkStatus = trunkNode->tLinkState;
			if(0 != trunkNode->portList) {
				ret = npd_trunk_get_mbr_bmp_via_sw_v1(trunkId,\
										&(acttrunkList[trunk_count]->mbrBmp_sp),\
										&(acttrunkList[trunk_count]->disMbrBmp_sp));
			if(NPD_SUCCESS != ret ) {
				syslog_ax_trunk_err("get trunk %d port bitmap error ret %d\n",trunkId,ret);
				ret = TRUNK_RETURN_CODE_ERR_GENERAL;
			}
				/*get trunk master port*/
				ret = npd_trunk_master_port_get(trunkId,&m_eth_index);
				if(NPD_TRUE == ret){
					acttrunkList[trunk_count]->masterFlag = 1; 
					slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
					port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(m_eth_index);
					acttrunkList[trunk_count]->mSlotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
					acttrunkList[trunk_count]->mPortno = ETH_LOCAL_INDEX2NO(slot_index, port_index);
					/* NPD_DEBUG(("trunk %d master port :slot = %d,port = %d.",trunkId,\
								acttrunkList[trunk_count]->mSlotNo,\
								acttrunkList[trunk_count]->mPortno));*/
					ret = TRUNK_RETURN_CODE_ERR_NONE;
				}
			}
			trunk_count++;
		}
	}
	if (trunk_count > 0) {
		ret = TRUNK_RETURN_CODE_ERR_NONE;
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	/* Total active trunk count*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &trunk_count);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &slot_id);
	/*open outer container*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING  /*begin*/
											DBUS_TYPE_UINT16_AS_STRING /*trunkId*/
											DBUS_TYPE_STRING_AS_STRING /*trunkname*/
											DBUS_TYPE_BYTE_AS_STRING	/*masterFlag*/
											DBUS_TYPE_UINT32_AS_STRING   /*master slotNo*/
											DBUS_TYPE_UINT32_AS_STRING   /*master portNo*/
										   	DBUS_TYPE_UINT32_AS_STRING /*mbrBmp_sp*/
										   	DBUS_TYPE_UINT32_AS_STRING /*MbrBmp_sp*/
											DBUS_TYPE_UINT32_AS_STRING /*DISbrBmp_sp*/
											DBUS_TYPE_UINT32_AS_STRING /*disMbrBmp_sp*/
										   	DBUS_TYPE_UINT32_AS_STRING /*loadBalance*/
										    DBUS_TYPE_UINT32_AS_STRING /*trkLinkStat*/
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	for (i = 0; i < trunk_count; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		 syslog_ax_trunk_dbg("acttrunkList[%d] trunkId=%d, masterPortFlag =%d,masterPort =%d/%d,portBmp0=%#0x\n",i,	\
															acttrunkList[i]->trunkId,\
															acttrunkList[i]->masterFlag,\
															acttrunkList[i]->mSlotNo,\
															acttrunkList[i]->mPortno,\
															acttrunkList[i]->mbrBmp_sp.portMbr[0]);
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT16,
				  &(acttrunkList[i]->trunkId));
		dbus_message_iter_append_basic 
				(&iter_struct,
				 DBUS_TYPE_STRING,
				 &(acttrunkList[i]->trunkName));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_BYTE,
				  &(acttrunkList[i]->masterFlag));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->mSlotNo));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->mPortno));
		
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->mbrBmp_sp.portMbr[0]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->mbrBmp_sp.portMbr[1]));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->disMbrBmp_sp.portMbr[0]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->disMbrBmp_sp.portMbr[1]));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->loadBalanc));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(acttrunkList[i]->tLinkStatus));		

		dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}


DBusMessage * npd_dbus_trunk_show_vlanlist
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	DBusError err;
	unsigned int	i = 0, op_ret = TRUNK_RETURN_CODE_ERR_NONE, vCount = 0;
	struct vlan_s	*vlanNode = NULL;
	struct trunk_s  *trunkNode =NULL;
	unsigned short trunkId = 0;
	static unsigned short vid[NPD_MAX_VLAN_ID] = {0};
	static unsigned char tag[NPD_MAX_VLAN_ID] = {0};
	char *trunkName = NULL;

	static trunkAllowVlan *tAllowVlan[CHASSIS_VLAN_RANGE_MAX];

	trunkName = (char *)malloc(ALIAS_NAME_SIZE+1);
	if(NULL == trunkName) {
		return NULL;
	}
	memset(trunkName,0,ALIAS_NAME_SIZE+1);
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&trunkId,
		DBUS_TYPE_INVALID))) {
		syslog_ax_trunk_dbg("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_trunk_dbg("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		free(trunkName);
		trunkName = NULL;
		return NULL;
	}

	if(TRUNK_RETURN_CODE_TRUNK_NOTEXISTS == npd_check_trunk_exist(trunkId)) {
		op_ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
		syslog_ax_trunk_dbg("trunk %d not exists.\n",trunkId);
	}
	else 
	{
		trunkNode = npd_find_trunk(trunkId);
		/*trunkName = (char*)(trunkNode->trunkName);*/
		memcpy(trunkName,trunkNode->trunkName, ALIAS_NAME_SIZE);
		if (NULL != trunkNode->allowVlanList) {
			syslog_ax_trunk_dbg("get vlan in trunk if trunk have vlan in it.\n");
			op_ret = npd_trunk_allow_vlan_id_get(trunkId,vid,&vCount);
			if( NPD_TRUE != op_ret ) {
				syslog_ax_trunk_err("get trunk %d allow vlanlist error\n",trunkId);
				op_ret = TRUNK_RETURN_CODE_GET_ALLOWVLAN_ERR;
			}
			op_ret = npd_trunk_allow_vlan_tagmode_get(trunkId,tag,&vCount);
			if( NPD_TRUE != op_ret ) {
				syslog_ax_trunk_err("get trunk %d allow vlanlist error\n",trunkId);
				op_ret = TRUNK_RETURN_CODE_GET_ALLOWVLAN_ERR;
			}
		}
	}
	if(vCount>0){
		for(i=0;i<vCount;i++){
			tAllowVlan[i] = (trunkAllowVlan *)malloc(sizeof(trunkAllowVlan));
			memset(tAllowVlan[i],0,sizeof(trunkAllowVlan));
			tAllowVlan[i]->vlanId = vid[i];
			tAllowVlan[i]->tagMode = tag[i];
			vlanNode = npd_find_vlan_by_vid(vid[i]);
			tAllowVlan[i]->vlanName = vlanNode->vlanName;
			/*NPD_DEBUG(("trunk %d allows vlan %d,vlanName %s.",trunkId,vid[i],vlanNode->vlanName));*/
		}
		op_ret = TRUNK_RETURN_CODE_ERR_NONE;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &op_ret);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_STRING,
									&trunkName);
	
	/* Total active vlan count*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &vCount);

	/*open outer container*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING  /*begin*/
										   	DBUS_TYPE_UINT16_AS_STRING /*vlanId*/
										   	DBUS_TYPE_STRING_AS_STRING /*vlanName*/
										   	DBUS_TYPE_BYTE_AS_STRING /*tag*/
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	for (i = 0; i < vCount; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		/*
		NPD_DEBUG(("inner inter i = %d, vid = %d.vlanname %s.",i,\
											tAllowVlan[i]->vlanId,\
											tAllowVlan[i]->vlanName));
		*/
		dbus_message_iter_append_basic( &iter_struct,
									  DBUS_TYPE_UINT16,
									  &(tAllowVlan[i]->vlanId));

		dbus_message_iter_append_basic(&iter_struct,
									 DBUS_TYPE_STRING,
									 &(tAllowVlan[i]->vlanName));

		dbus_message_iter_append_basic(&iter_struct,
									 DBUS_TYPE_BYTE,
									 &(tAllowVlan[i]->tagMode));

		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(tAllowVlan[i]);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	free(trunkName);
	trunkName = NULL;
	return reply;
	
}



DBusMessage* npd_dbus_trunk_show_arp(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter  iter,iter_array;
	unsigned int i = 0, j = 0, k = 0;
	unsigned int arpCount = 0;
	unsigned int ipaddr= 0;
	unsigned char mac[6] = {0};
	unsigned char isTrunk = 0, isTagged = 0, isStatic = 0;
    unsigned char isValid = 0;
	unsigned short trunkId = 0;
	unsigned short vid = 0, vidx = 0;
	unsigned int ret = TRUNK_RETURN_CODE_ERR_NONE;
	struct arp_snooping_item_s ** arpInfo = NULL;
	struct arp_snooping_item_s * tmpArp = NULL;
	DBusError err;

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&trunkId,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_eth_port_dbg("show arp info in trunk %d\n",trunkId);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	/* Total slot count*/

	if(TRUNK_RETURN_CODE_TRUNK_EXISTS != npd_check_trunk_exist(trunkId)){
		ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
	}
	else{
		pthread_mutex_lock(&arpHashMutex);
		arpCount = npd_arp_snooping_get_num_bytrunk(trunkId);
		if(0 < arpCount) {
			arpInfo = (struct arp_snooping_item_s **)malloc(arpCount * (sizeof(struct arp_snooping_item_s *)));
			if(NULL == arpInfo) {
				ret = TRUNK_RETURN_CODE_ERR_GENERAL;
			}
			else {
				memset(arpInfo,0,arpCount * (sizeof(struct arp_snooping_item_s *)));
				npd_arp_snooping_find_item_bytrunk(trunkId,arpInfo,arpCount);
				for(k = 0;k< arpCount;k++){
					tmpArp = (struct arp_snooping_item_s *)malloc(sizeof(struct arp_snooping_item_s));
					memset(tmpArp,0,sizeof(struct arp_snooping_item_s));
					memcpy(tmpArp,arpInfo[k],sizeof(struct arp_snooping_item_s));
					arpInfo[k] = tmpArp;
					tmpArp = NULL;
				}
			}
		}
		
		pthread_mutex_unlock(&arpHashMutex);
	}

	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32,
									&ret);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &arpCount);
	if(TRUNK_RETURN_CODE_ERR_NONE == ret) {
		if(arpCount > 0) {
			dbus_message_iter_open_container (&iter,
											   DBUS_TYPE_ARRAY,
											   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_UINT32_AS_STRING   /* ip addr*/
														
														DBUS_TYPE_BYTE_AS_STRING       /* mac*/
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														
														DBUS_TYPE_BYTE_AS_STRING		/*isTrunk*/
														DBUS_TYPE_BYTE_AS_STRING		/*trunk id*/
														DBUS_TYPE_UINT16_AS_STRING	/*vid*/
														DBUS_TYPE_UINT16_AS_STRING	/*vidx*/
														DBUS_TYPE_BYTE_AS_STRING		/*isTagged*/
														DBUS_TYPE_BYTE_AS_STRING		/*isStatic*/
											   DBUS_STRUCT_END_CHAR_AS_STRING,
											   &iter_array);
			
			
			for (i = 0; i < arpCount; i++ ) {
				
				DBusMessageIter iter_struct;
				dbus_message_iter_open_container (&iter_array,
												   DBUS_TYPE_STRUCT,
												   NULL,
												   &iter_struct);
				
				if(NULL != arpInfo[i]) {
					ipaddr = arpInfo[i]->ipAddr;
					memcpy(mac,arpInfo[i]->mac,6);
					isTrunk = arpInfo[i]->dstIntf.isTrunk;
					trunkId = arpInfo[i]->dstIntf.intf.trunk.trunkId;
					vid = arpInfo[i]->vid;
					vidx = arpInfo[i]->vidx;
					isTagged = arpInfo[i]->isTagged;
					isStatic = arpInfo[i]->isStatic;
                    isValid = arpInfo[i]->isValid;


#if 0
					printf("%-8s %-15s %-17s %-4s %-6s %-6s %-4s\n",	\
						"IFINDEX","IP ADDRESS","MAC ADDRESS","VID","TAG","DEVICE","PORT");
					printf("-------- --------------- ----------------- ---- ------ ------ ----\n");

					printf("%-8x %-3d.%-3d.%-3d.%-3d %02x:%02x:%02x:%02x:%02x:%02x %-4d %-6s %-6d %-4d\n",	\
					arpInfo[i]->ifIndex,(ipaddr>>24)&0xFF,(ipaddr>>16)&0xFF,	\
					(ipaddr>>8)&0xFF,ipaddr&0xFF,mac[0],mac[1],	\
					mac[2],mac[3],mac[4],mac[5],vid,	\
					isTagged ? "TRUE":"FALSE",arpInfo[i]->dstIntf.intf.port.devNum,	\
					arpInfo[i]->dstIntf.intf.port.portNum);
#endif
                    free(arpInfo[i]);
                    arpInfo[i] = NULL;
				}
				else {
					ipaddr = 0;
					memset(mac,0,6);
					isTrunk = 0;
					trunkId = 0;
					vid = 0;
					vidx = 0;
					isTagged = 3;
					isStatic = 0;
                    isValid = 0;
				}
                
				dbus_message_iter_append_basic
							(&iter_struct,
							  DBUS_TYPE_BYTE,
							  &(isValid));   /*isValid */
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT32,
						  &(ipaddr));  /* ip addr*/
				
				for (j = 0; j < 6; j++ ) {
					
					dbus_message_iter_append_basic
							(&iter_struct,
							  DBUS_TYPE_BYTE,
							  &(mac[j]));  /* mac*/
				}

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isTrunk)); /*is trunkId*/
						  
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(trunkId)); /*trunkId*/
						  
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT16,
						  &(vid)); /*vid*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT16,
						  &(vidx)); /*vidx*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isTagged)); /*isTagged*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isStatic)); /*isStatic*/
					
				dbus_message_iter_close_container (&iter_array, &iter_struct);
			}
				
			dbus_message_iter_close_container (&iter, &iter_array);
			free(arpInfo);
			arpInfo = NULL;
		}
	
	}

	return reply;
}

DBusMessage* npd_dbus_trunk_show_nexthop(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter  iter,iter_array;
	unsigned int i = 0, j = 0;
	unsigned int arpCount = 0;
	unsigned int ipaddr= 0;
	unsigned char mac[6] = {0};
	unsigned char isTrunk = 0, isTagged = 0, isStatic = 0;
	unsigned short trunkId = 0;
	unsigned short vid = 0, vidx = 0;
	unsigned int ret = TRUNK_RETURN_CODE_ERR_NONE;	
	unsigned int *refCntArray = NULL, refCnt = 0;
	struct arp_snooping_item_s * arpInfo = NULL;
	struct arp_snooping_item_s * tmpArp = NULL;
	DBusError err;

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&trunkId,
		DBUS_TYPE_INVALID))) {
		syslog_ax_eth_port_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_eth_port_dbg("show arp info in trunk %d\n",trunkId);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	/* Total slot count*/

	if(TRUNK_RETURN_CODE_TRUNK_EXISTS != npd_check_trunk_exist(trunkId)){
		ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
	}
	else{
		pthread_mutex_lock(&arpHashMutex);
		arpCount = npd_arp_snooping_get_valid_num_bytrunk_for_nexthop(trunkId);  /*add by hanhui for show valid nexthop*/
		if(0 < arpCount) {
			arpInfo = (struct arp_snooping_item_s *)malloc(arpCount * (sizeof(struct arp_snooping_item_s )));
			if(NULL == arpInfo) 
				ret = TRUNK_RETURN_CODE_ERR_GENERAL;
			else {
				/*printf("get nexthop info arpInfo %p\n",arpInfo);*/
				memset(arpInfo,0,arpCount * (sizeof(struct arp_snooping_item_s )));
				refCntArray = (unsigned int *)malloc(sizeof(int) * arpCount);
				if(NULL == refCntArray) {
					ret = TRUNK_RETURN_CODE_ERR_GENERAL;
					free(arpInfo);
					arpInfo = NULL;
				}
				else {
					memset(refCntArray, 0, sizeof(int)*arpCount);
			 		npd_arp_snooping_get_valid_nexthop_item_bytrunk(trunkId,arpInfo,arpCount,refCntArray);   /*add by hanhui for show valid arp*/
				}
			}
		}
		pthread_mutex_unlock(&arpHashMutex);
	}

	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32,
									&ret);
	dbus_message_iter_append_basic (&iter,
								 DBUS_TYPE_UINT32,
								 &arpCount);
	if(TRUNK_RETURN_CODE_ERR_NONE == ret) {
		if(arpCount > 0) {
			dbus_message_iter_open_container (&iter,
											   DBUS_TYPE_ARRAY,
											   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
														DBUS_TYPE_UINT32_AS_STRING   /* ip addr*/
														
														DBUS_TYPE_BYTE_AS_STRING       /* mac*/
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														DBUS_TYPE_BYTE_AS_STRING
														
														DBUS_TYPE_BYTE_AS_STRING		/*isTrunk*/
														DBUS_TYPE_BYTE_AS_STRING		/*trunk id*/
														DBUS_TYPE_UINT16_AS_STRING	/*vid*/
														DBUS_TYPE_UINT16_AS_STRING	/*vidx*/
														DBUS_TYPE_BYTE_AS_STRING		/*isTagged*/
														DBUS_TYPE_BYTE_AS_STRING		/*isStatic*/
														DBUS_TYPE_UINT32_AS_STRING		/*ref count*/
											   DBUS_STRUCT_END_CHAR_AS_STRING,
											   &iter_array);
			
			
			for (i = 0; i < arpCount; i++ ) {
				
				DBusMessageIter iter_struct;
				dbus_message_iter_open_container (&iter_array,
												   DBUS_TYPE_STRUCT,
												   NULL,
												   &iter_struct);
				
				if(0 != arpInfo[i].ifIndex) {
					ipaddr = arpInfo[i].ipAddr;
					memcpy(mac,arpInfo[i].mac,6);
					isTrunk = arpInfo[i].dstIntf.isTrunk;
					trunkId = arpInfo[i].dstIntf.intf.trunk.trunkId;
					vid = arpInfo[i].vid;
					vidx = arpInfo[i].vidx;
					isTagged = arpInfo[i].isTagged;
					isStatic = arpInfo[i].isStatic;
					refCnt = refCntArray[i];


				}
				else {
					ipaddr = 0;
					memset(mac,0,6);
					isTrunk = 0;
					trunkId = 0;
					vid = 0;
					vidx = 0;
					isTagged = 3;
					isStatic = 0;
				}
				
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT32,
						  &(ipaddr));  /* ip addr*/
				
				for (j = 0; j < 6; j++ ) {
					
					dbus_message_iter_append_basic
							(&iter_struct,
							  DBUS_TYPE_BYTE,
							  &(mac[j]));  /* mac*/
				}

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isTrunk)); /*is trunkId*/
						  
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(trunkId)); /*trunkId*/
						  
				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT16,
						  &(vid)); /*vid*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT16,
						  &(vidx)); /*vidx*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isTagged)); /*isTagged*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_BYTE,
						  &(isStatic)); /*isStatic*/

				dbus_message_iter_append_basic
						(&iter_struct,
						  DBUS_TYPE_UINT32,
						  &(refCnt)); 
					
				dbus_message_iter_close_container (&iter_array, &iter_struct);
			}
				
			dbus_message_iter_close_container (&iter, &iter_array);
			free(arpInfo);
			free(refCntArray);
			arpInfo = NULL;
			refCntArray = NULL;
		}
	
	}

	return reply;
}


DBusMessage * npd_dbus_trunk_show_running_config
(	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	char *showStr = NULL;
	unsigned char file_path[64] = {0};
	int function_type = -1;	

	showStr = (char*)malloc(NPD_TRUNK_RUNNING_CFG_MEM);
	if(NULL == showStr) {
		 syslog_ax_trunk_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_TRUNK_RUNNING_CFG_MEM);
	/*save trunk cfg*/

	if((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I))
	{
		npd_save_dst_trunk_cfg(showStr,NPD_TRUNK_RUNNING_CFG_MEM);
	}
	else
	{
		npd_save_trunk_cfg(showStr,NPD_TRUNK_RUNNING_CFG_MEM);
	}
	syslog_ax_trunk_dbg("trunk config:%s\n",showStr);

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr);	
	free(showStr);
	showStr = NULL;
	return reply;
}


DBusMessage * npd_dbus_trunk_clear_arp
(	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned int trunkId = 0;
	unsigned int isStatic = 0;
	unsigned int arpCount = 0;
	unsigned int ret = TRUNK_RETURN_CODE_ERR_NONE;

	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TRUNK_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TRUNK_VLAUE)){
		npd_syslog_dbg("do not support trunk!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_UINT32,&trunkId,
			DBUS_TYPE_UINT32,&isStatic,
			DBUS_TYPE_INVALID))) {
			syslog_ax_eth_port_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				syslog_ax_eth_port_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		if(TRUNK_RETURN_CODE_TRUNK_EXISTS != npd_check_trunk_exist(trunkId)){
			ret = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
		}
		else{
	        if(isStatic){
				arpCount = npd_static_arp_clear_by_trunk(trunkId);
			}
			else {
				arpCount = npd_arp_clear_by_trunk(trunkId);
			}
			syslog_ax_eth_port_dbg("npd clear %s arp on trunk %d total %d item cleared\n",isStatic?"static":"",trunkId,arpCount);
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);	
	return reply;
}

/* add fot distribute trunk */
/* update g_dst_trunk of smu */
DBusMessage * npd_dbus_trunk_add_delete_port_trunk_list
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;

	unsigned char	isAdd = 0;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index = 0, local_port_index = 0,devnum = 0,virportnum = 0,endis = 0;
	unsigned short	trunkId = 0;	
	unsigned int	eth_g_index = 0;
	unsigned int 	ret = TRUNK_RETURN_CODE_ERR_NONE;
	unsigned char   trunk_port_log = 0,portLog = 0;
	unsigned char   master = 0;
	int i = 0,j=0,vlanId=0,tLinkState = 0;
	
	DBusError err;	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isAdd,
							DBUS_TYPE_BYTE,&slot_no,
							DBUS_TYPE_BYTE,&local_port_no,
							DBUS_TYPE_BYTE,&master,	
							DBUS_TYPE_BYTE,&devnum,
							DBUS_TYPE_BYTE,&virportnum,
							DBUS_TYPE_BYTE,&endis,
							DBUS_TYPE_UINT16,&trunkId,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_trunk_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_trunk_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	/*set master port of trunk*/
	if(1 == master)
    {
    	syslog_ax_vlan_dbg("g_trunks: set port %d/%d on trunk %d to master !\n",slot_no,local_port_no,trunkId);
		
		for(i=0; i<g_dst_trunk[trunkId-1].portLog; i++)
		{
            if((slot_no == g_dst_trunk[trunkId-1].portmap[i].slot)&&(local_port_no == g_dst_trunk[trunkId-1].portmap[i].port))
            {
    			g_dst_trunk[trunkId-1].portmap[i].master = 1;     /* set port to master port  */
            }
			else
			{
    			g_dst_trunk[trunkId-1].portmap[i].master = 0;     /* set port to not master port  */				
			}
		}
   		ret = TRUNK_RETURN_CODE_ERR_NONE;
    }
	else
	{
		portLog = g_dst_trunk[trunkId-1].portLog;
		if(isAdd)/*add port*/
		{
			g_dst_trunk[trunkId-1].portLog  = portLog + 1;
			g_dst_trunk[trunkId-1].portmap[portLog].enable = endis;
			g_dst_trunk[trunkId-1].portmap[portLog].slot = slot_no;
			g_dst_trunk[trunkId-1].portmap[portLog].port = local_port_no;
			g_dst_trunk[trunkId-1].dst_port_map[portLog].devnum = devnum;
			g_dst_trunk[trunkId-1].dst_port_map[portLog].virportnum = virportnum;
			if(g_dst_trunk[trunkId-1].portLog == 1)/*set the first port to be master port*/
			{
				g_dst_trunk[trunkId-1].portmap[portLog].master = 1;
			}
		}
		else/*delete port*/
		{
			for(vlanId=1;vlanId<=4094;vlanId++)
			{
				if(g_vlanlist[vlanId-1].tagTrkBmp[trunkId-1] == 1)/*trunk is tag of vlan*/
				{
					if((local_port_no <= 32)&&((g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp)&(1<<(local_port_no-1))))/*if trunk port is enable for qinq*/
					{
						g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
					}
					else if((local_port_no>32) && ((g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp)&(1<<(local_port_no-33))))
					{
						g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
					}
				}
			}
			
			g_dst_trunk[trunkId-1].portLog = portLog - 1;
			for(i=0;i<portLog;i++)
			{
				if((g_dst_trunk[trunkId-1].portmap[i].slot == slot_no) && ((g_dst_trunk[trunkId-1].portmap[i].port == local_port_no)))
				{
					for(j=i;j<TRUNK_MEMBER_NUM_MAX-1;j++)
					{
						g_dst_trunk[trunkId-1].portmap[j].enable = g_dst_trunk[trunkId-1].portmap[j+1].enable;
						g_dst_trunk[trunkId-1].portmap[j].slot = g_dst_trunk[trunkId-1].portmap[j+1].slot;
						g_dst_trunk[trunkId-1].portmap[j].port = g_dst_trunk[trunkId-1].portmap[j+1].port;
						g_dst_trunk[trunkId-1].dst_port_map[j].devnum = g_dst_trunk[trunkId-1].dst_port_map[j+1].devnum;
						g_dst_trunk[trunkId-1].dst_port_map[j].virportnum = g_dst_trunk[trunkId-1].dst_port_map[j+1].virportnum;
					}
					g_dst_trunk[trunkId-1].portmap[TRUNK_MEMBER_NUM_MAX-1].enable = 0;
					g_dst_trunk[trunkId-1].portmap[TRUNK_MEMBER_NUM_MAX-1].slot = 0;
					g_dst_trunk[trunkId-1].portmap[TRUNK_MEMBER_NUM_MAX-1].port = 0;
					g_dst_trunk[trunkId-1].dst_port_map[TRUNK_MEMBER_NUM_MAX-1].devnum = 0;
					g_dst_trunk[trunkId-1].dst_port_map[TRUNK_MEMBER_NUM_MAX-1].virportnum = 0;
					break;
				}
			}
		}

		for(i=0;i<g_dst_trunk[trunkId-1].portLog;i++)/*update trunk state*/
		{
			if(g_dst_trunk[trunkId-1].portmap[i].enable)
			{
				g_dst_trunk[trunkId-1].tLinkState = TRUNK_STATE_UP_E;
				break;
			}
			else
			{
				g_dst_trunk[trunkId-1].tLinkState = TRUNK_STATE_DOWN_E;
			}
		}
		ret = msync(g_dst_trunk, sizeof(dst_trunk_s)*127, MS_SYNC);
		if( ret!=0 )
	    {
	        syslog_ax_trunk_err("msync shm_trunk failed \n" ); 
			ret = TRUNK_RETURN_CODE_ERR_GENERAL;
	    }		
		else
		{
			ret = TRUNK_RETURN_CODE_ERR_NONE;
		}
		syslog_ax_trunk_dbg("set trunk %d invalid ok\n",trunkId);

		/* add for update the g_vlanlist[] of distributed */
        if(isAdd==1)
        {   
            /* remove the port from all vlan */
    		for (vlanId=1;vlanId<=4094;vlanId++)
            {
                if(g_vlanlist[vlanId-1].vlanStat == 1)
        		{			
                    if(local_port_no>32)
                    {
            			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));		/* remove from all vlan */		
            			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));			
                    }
            		else
            		{
            			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));		/* remove from all vlan */			
            			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
            		}
                }
    		}
        }
    	else
    	{

            if(local_port_no>32)
            {
    			g_vlanlist[0].untagPortBmp[slot_no-1].high_bmp |= (unsigned int)((1<<(local_port_no-33)));		/* add to default vlan  */		
            }
    		else
    		{
    	        g_vlanlist[0].untagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));		/* add to default vlan */	
    		}
    	}

    	ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
    	if( ret!=0 )
        {
            syslog_ax_trunk_err("msync shm_vlan failed \n" );
    		ret = TRUNK_RETURN_CODE_ERR_GENERAL;
        }
    	else
    	{
    		ret = TRUNK_RETURN_CODE_ERR_NONE;
    	}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}

/* add for distributed trunk */
int	npd_trunk_list_delete(unsigned char trunkId)
{
	struct trunk_s* node = NULL;
	int retval = TRUNK_RETURN_CODE_ERR_NONE;
    int i = 0,j=0;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index = 0;
	
	node = npd_find_trunk(trunkId);

	if (NULL == node)
	{
	 	retval = TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;
	}
	else 
	{
		for(i=0;i<g_dst_trunk[trunkId-1].portLog;i++)
		{
			slot_no = g_dst_trunk[trunkId-1].portmap[i].slot;
			local_port_no = g_dst_trunk[trunkId-1].portmap[i].port;
			if(local_port_no>32)
            {
    			g_vlanlist[0].untagPortBmp[slot_no-1].high_bmp |= (unsigned int)((1<<(local_port_no-33)));		/* add to default vlan  */
            }
    		else
    		{
    	        g_vlanlist[0].untagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));		/* add to default vlan */	
    		}
		}
	}
	/* remove the trunk from 1-4093 vlan */
	
	for(i=0; i<=4093; i++)
	{
		/*update vlan trunk qinq state */
		for(j=0;j<g_dst_trunk[trunkId-1].portLog;j++)
		{
			slot_no = g_dst_trunk[trunkId-1].portmap[j].slot;
			local_port_no = g_dst_trunk[trunkId-1].portmap[j].port;
			if(g_vlanlist[i].tagTrkBmp[trunkId-1] == 1)/*only if the speical trunk is added to vlan as tag*/
			{
				if(local_port_no > 32)
				{
					if((g_vlanlist[i].qinq[slot_no-1].high_bmp)&(1<<(local_port_no-33)))
					{
						g_vlanlist[i].qinq[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
					}
				}
				else
				{
					if((g_vlanlist[i].qinq[slot_no-1].low_bmp)&(1<<(local_port_no-1)))
					{
						g_vlanlist[i].qinq[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
					}
				}
			}
		}
        g_vlanlist[i].tagTrkBmp[trunkId-1] = 0;		
        g_vlanlist[i].untagTrkBmp[trunkId-1] = 0;					
	}
	
	retval = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
	if( retval!=0 )
    {
        syslog_ax_vlan_err("msync shm_vlan failed \n" );
		retval = TRUNK_RETURN_CODE_ERR_HW;
    }
	else
	{
		retval = TRUNK_RETURN_CODE_ERR_NONE;
	}
	
	return retval;
}

/**********************************************************************************
 *  npd_init_distributed_trunk
 *
 *	DESCRIPTION:
 * 		INIT the distributed trunk structure
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		TRUNK_RETURN_CODE_ERR_NONE	- init success
 *
 **********************************************************************************/



int npd_init_distributed_trunk(void)
{
	int fd = -1;
	int length = 0;
	int ret = -1;
	unsigned char devCnt = 0,devNum = 0;
	char* file_path = "/dbm/shm/trunk/shm_trunk";
    void * mmaped_base = NULL;
    syslog_ax_trunk_dbg("\n============== init distributed trunk: ===============\n");
 
    /* mmap file /dbm/shm/trunk/trunk_vlan to g_dst_trunk */
	system("mkdir -p /dbm/shm/trunk/");	
    syslog_ax_trunk_dbg("file_path = %s\n", file_path);
	fd = open(file_path, O_CREAT | O_RDWR, 00755);
	
	if(fd < 0)
    {
        syslog_ax_trunk_dbg("Failed to open! %s\n", strerror(errno));
        return -1;
    }

	length = lseek(fd, sizeof(dst_trunk_s)*127, SEEK_SET);
    write(fd,"",1);

    g_dst_trunk = (dst_trunk_s *)mmap(NULL, sizeof(dst_trunk_s)*127, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

    syslog_ax_trunk_dbg("mmap addr for shm_trunk dst_trunk = 0x%x\n", g_dst_trunk);

    if (MAP_FAILED == g_dst_trunk)
    {
        syslog_ax_trunk_err("Failed to mmap for g_dst_trunk[]! %s\n", strerror(errno));
    	close(fd);      /* code optimize: Resource leak. zhangdi@autelan.com 2013-01-18 */		
        return -1;
    }
	memset(g_dst_trunk, 0, sizeof(dst_trunk_s)*127);			

	close(fd);
	devCnt = nam_asic_get_instance_num();
	for(devNum = 0; devNum < devCnt; devNum++)
	{
		ret = nam_asic_trunk_load_balanc_set(devNum,0,LOAD_BANLC_MAC_IP_L4);
		if(ret != TRUNK_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_trunk_err("set trunk load balance failed !\n");
		}
	}
	if(ret == TRUNK_RETURN_CODE_ERR_NONE)
	{
		syslog_ax_trunk_dbg("set trunk load balance MAC+IP+L4 successfully !\n");
	}
    syslog_ax_trunk_dbg("=========== init distributed trunk OK ! ==============\n\n");
	
	return TRUNK_RETURN_CODE_ERR_NONE;	
}

/**********************************************************************************
 *  npd_dst_trunk_valid
 *
 *	DESCRIPTION:
 * 		when create trunk,update the params in the global trunk structure
 *
 *	INPUT:
 *		trunkId - trunk  id
 *		trunkName - trunk name
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		TRUNK_RETURN_CODE_ERR_NONE	- update success
 *		TRUNK_RETURN_CODE_ERR_GENERAL  - update fail 
 *
 **********************************************************************************/

unsigned int npd_dst_trunk_valid(unsigned short trunkId,char *trunkName)
{
    int ret =0;
	
	g_dst_trunk[trunkId-1].tLinkState = TRUNK_STATE_DOWN_E;
	g_dst_trunk[trunkId-1].loadBanlcMode = g_loadBalanc;
	g_dst_trunk[trunkId-1].trunkId = trunkId;
	memcpy(g_dst_trunk[trunkId-1].trunkName , trunkName ,NPD_TRUNK_IFNAME_SIZE);    /* vlanName is 12 chars */
	ret = msync(g_dst_trunk, sizeof(dst_trunk_s)*127, MS_SYNC);
	if( ret != TRUNK_RETURN_CODE_ERR_NONE )
    {
        syslog_ax_vlan_err("msync shm_trunk failed \n" );
		return TRUNK_RETURN_CODE_ERR_GENERAL;
    }	
	syslog_ax_vlan_dbg("set vlan %d valid ok\n",trunkId);	
	return ret;
}

/**********************************************************************************
 *  npd_dst_trunk_invalid
 *
 *	DESCRIPTION:
 * 		when delete trunk,update the params in the global trunk structure
 *
 *	INPUT:
 *		trunkId - trunk  id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		TRUNK_RETURN_CODE_ERR_NONE	- update success
 *		TRUNK_RETURN_CODE_ERR_GENERAL  - update fail 
 *
 **********************************************************************************/

unsigned int npd_dst_trunk_invalid(unsigned short trunkId)
{
    int ret =0;

	/*set the trunk id to null*/
	memset(&(g_dst_trunk[trunkId-1]),0,sizeof(dst_trunk_s));
	ret = msync(g_dst_trunk, sizeof(dst_trunk_s)*127, MS_SYNC);
	if( ret != TRUNK_RETURN_CODE_ERR_NONE )
    {
        syslog_ax_vlan_err("msync shm_trunk failed \n" ); 
		return TRUNK_RETURN_CODE_ERR_GENERAL;
    }		
	syslog_ax_vlan_dbg("set trunk %d invalid ok\n",trunkId);	
	return ret;
}


#ifdef __cplusplus
}
#endif
