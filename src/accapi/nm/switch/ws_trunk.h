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
* ws_trunk.h
*
*
* CREATOR:
* autelan.software.Network Dep. team
* zhouym@autelan.com
*
* DESCRIPTION:
* function for web
*
*
***************************************************************************/
#ifndef _WS_TRUNK_H
#define _WS_TRUNK_H



#define ALIAS_NAME_SIZE 		0x15
#define ALIAS_NAME_LEN_ERROR	0x1
#define ALIAS_NAME_HEAD_ERROR	(ALIAS_NAME_LEN_ERROR+1)
#define ALIAS_NAME_BODY_ERROR	(ALIAS_NAME_LEN_ERROR+2)
#define VID_MAX_NUM  0xFFF 
#define VID_SPLIT_DASH 		'-'
#define VID_SPLIT_SLASH		','
#define VID_LLEGAL(vid)     ((vid)>0&&(vid)<VID_MAX_NUM)
#define NPD_VLAN_ERR_NONE			NPD_SUCCESS


#define TRUNK_PORT_EXISTS_GTDB	0xf		//port already exists in trunkDB
#define TRUNK_PORT_MBRS_FULL	(TRUNK_PORT_EXISTS_GTDB+1)
#define NPD_TRUNK_ERR_NONE		NPD_SUCCESS
#define NPD_TRUNK_ERR_GENERAL	(NPD_TRUNK_ERR_NONE + 20)		//general failure
#define NPD_TRUNK_BADPARAM			(NPD_TRUNK_ERR_NONE + 22)  		//bad parameters
#define NPD_TRUNK_EXISTS			(NPD_TRUNK_ERR_NONE + 23)		//vlan have been created already
#define NPD_TRUNK_NOTEXISTS		(NPD_TRUNK_ERR_NONE + 24)			//vlan does not exists
#define NPD_TRUNK_ERR_HW			(NPD_TRUNK_ERR_NONE + 26) 		//vlan error when operation on HW
#define NPD_TRUNK_PORT_EXISTS		(NPD_TRUNK_ERR_NONE + 27)		//port already exists in vlan
#define NPD_TRUNK_PORT_NOTEXISTS	(NPD_TRUNK_ERR_NONE + 28)		// port is not a member of vlan
#define NPD_TRUNK_PORT_MEMBERSHIP_CONFLICT	(NPD_TRUNK_ERR_NONE + 29)		//port can NOT be Untag member of different vlans. 
#define NPD_TRUNK_NAME_CONFLICT		(NPD_TRUNK_ERR_NONE + 30)	
#define NPD_TRUNK_MEMBERSHIP_CONFICT		(NPD_TRUNK_ERR_NONE + 31)
#define NPD_TRUNK_ALLOW_ERR				(NPD_TRUNK_ERR_NONE +32)
#define NPD_TRUNK_REFUSE_ERR				(NPD_TRUNK_ERR_NONE +33)
#define NPD_TRUNK_MEMBER_ADD_ERR	(NPD_TRUNK_ERR_NONE + 34)
#define NPD_TRUNK_MEMBER_DEL_ERR	(NPD_TRUNK_ERR_NONE + 35)
#define NPD_TRUNK_GET_ALLOWVLAN_ERR	(NPD_TRUNK_ERR_NONE + 36)
#define NPD_TRUNK_NO_MEMBER			(NPD_TRUNK_ERR_NONE + 37)
#define NPD_TRUNK_SET_TRUNKID_ERR	(NPD_TRUNK_ERR_NONE + 38)
#define NPD_TRUNK_DEL_MASTER_PORT	(NPD_TRUNK_ERR_NONE + 39)		//master port NOT allowd to delete.
#define NPD_TRUNK_PORT_ENABLE		(NPD_TRUNK_ERR_NONE + 40)		//port enalbe in trunk
#define NPD_TRUNK_PORT_NOTENABLE	(NPD_TRUNK_ERR_NONE + 41)		// port disable in trunk
#define NPD_TRUNK_ALLOW_VLAN		(NPD_TRUNK_ERR_NONE + 42)
#define NPD_TRUNK_NOTALLOW_VLAN		(NPD_TRUNK_ERR_NONE + 43)
#define NPD_TRUNK_LOAD_BANLC_CONFLIT (NPD_TRUNK_ERR_NONE + 44)		//trunk load balance mode same to original
#define NPD_TRUNK_VLAN_TAGMODE_ERR (NPD_TRUNK_ERR_NONE + 45)		
#define NPD_TRUNK_PORT_L3_INTF		(NPD_TRUNK_ERR_NONE + 61)
#define NPD_VLAN_TRUNK_CONFLICT		(NPD_VLAN_ERR_NONE + 65)  // vlan can not add more than one trunk 


#define	DEFAULT_TRUNK_ID			0x0								//NOT member of any trunk


#define ETH_PORT_NUM_MAX		0x6
#define EXTENDED_SLOT_NUM_MAX	0x4
#define TRUNK_MEMBER_NUM_MAX		0x8
#define TRUNK_CONFIG_SUCCESS	0x0
#define	TRUNK_CONFIG_FAIL		0xff

#define SLOT_PORT_SPLIT_DASH 		'-'
#define SLOT_PORT_SPLIT_SLASH		'/'

/*src/npdsuite/npd/src/app/npd_vlan.h*/
#define NPD_VLAN_NOTEXISTS		(NPD_VLAN_ERR_NONE + 15)		//vlan does not exists
#define NPD_VLAN_L3_INTF			(NPD_VLAN_ERR_NONE + 20)		//vlan is L3 interface
#define NPD_VLAN_TRUNK_EXISTS		(NPD_VLAN_ERR_NONE + 63)




struct port_profile
{
  unsigned int slot;
  unsigned int port;
  unsigned int tmpVal;
  struct port_profile *next;
};

struct vlan_profile
{
  unsigned short vlanId;
  char vlanName[NPD_TRUNK_IFNAME_SIZE];
  unsigned char	tagMode;
  struct vlan_profile *next;
};

#if 0
typedef struct {
	unsigned int portMbr[2];
}PORT_MEMBER_BMP;

#endif


struct trunk_profile
{
  unsigned short trunkId;
  unsigned char	trunkName[NPD_TRUNK_IFNAME_SIZE];
  unsigned char	loadBalanc[20];
  unsigned char masterFlag;
  unsigned int mSlotNo;
  unsigned int mPortNo;
  unsigned char tLinkState;
  unsigned int port_Cont;
  unsigned int vlan_Cont;
  PORT_MEMBER_BMP	mbrBmp_sp;  
  PORT_MEMBER_BMP disMbrBmp_sp;
  struct port_profile *portHead;
  struct vlan_profile *vlanHead;
  struct trunk_port_bmp	portsMbrBmp;
  trunk_port_list_s	*portList;
  trunk_vlan_list_s	*allowVlanList;
  struct trunk_profile *next;  
};


extern void Free_trunk_head( struct trunk_profile *head);
extern void Free_trunk_one( struct trunk_profile *trunk);
extern void Free_vlanlist_trunk_head( struct trunk_profile *head);
extern int show_trunk_list(struct trunk_profile *trunk_head, int *trunk_num);   /*ʧ�ܷ���0���ɹ�����1������-1��ʾno trunk*/
																			      /*����-2��ʾError occurs in Read trunk Entry in HW.������-3��ʾOp on trunk portlist Fail.*/
extern int show_trunk_byid(int id,struct trunk_profile *trunk);  /*����Ҫ��ʾ��trunk��id�ţ�����trunk��������Ϣ*/
																	 /*����0��ʾʧ�ܣ�,����1��ʾ�ɹ�������-1��ʾtrunk ID�Ƿ�*/
																	 /*����-2��ʾtrunk not Exists������-3��ʾerror*/
extern int show_trunk_vlanlist(int trunk_id,struct trunk_profile *trunk_head); /*ʧ�ܷ���0���ɹ�����1������-1��ʾIllegal trunk Id������-2��ʾtrunk not exists*/
																				  /*����-3��ʾError: Op on Hw Fail������-4��ʾError: Op on Getting trunk allow vlanlist*/
extern int create_trunk(char * id, char *trunk_name);   	/*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾtrunk name can  not be list*/
														/*����-2��ʾtrunk name too long������-3��ʾtrunk name begins with an illegal char*/
														/*����-4��ʾtrunk name contains illegal char������-5��ʾtrunk id illeagal*/
														/*����-6��ʾtrunkID Already Exists������-7��ʾtrunkName Already Exists*/
														/*����-8��ʾerror*/
extern int delete_trunk_byid(int id);   /*����0��ʾ ɾ��ʧ�ܣ�����1��ʾɾ���ɹ�������-1��ʾTrunk id Illegal������-2��ʾtrunk not exists������-3��ʾ����*/
extern int add_delete_trunk_port(int id,char *flag,char *slot_port);  /*flag="add"��"delete"��slot_port��ʽ"1-1,1-2"��slot ��Χ1-4��port��Χ1-6��ÿ��trunk���add 8��port*/
                                                                          /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾport not exists*/
                                                                          /*����-2��ʾtrunk not exists������-3��ʾ port was Already the member of this trunk*/
                                                                          /*����-4��ʾport was not member of this trunk������-5��ʾ trunk port member id FULL*/
                                                                          /*����-6��ʾThis port is a member of other trunk������-7��ʾ This port is L3 interface*/
                                                                          /*����-8��ʾThis is the master port of trunk������-9��ʾerror*/
extern int set_master_port(int id,char *slot_port);  /*slot_port��ʽ"1-1,1-2"��slot ��Χ1-4��port��Χ1-6*/
													    /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnknow portno format.������-2��ʾport not exists*/
														/*����-3��ʾtrunk not exists������-4��ʾ port was not member of this trunk*/
														/*����-5��ʾThis port is L3 interface������-6��ʾ error*/
extern int allow_refuse_vlan(int id,char *flag,char *vlan_id,char *mod);  /*flag="allow"��"refuse"��vlan_id��Χ1-4094*��mod="tag"��"untag"*/
                                                                             /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnknow vlan format.������-2��ʾvlan not exists*/
                                                                             /*����-3��ʾvlan is L3 interface������-4��ʾ vlan Already allow in trunk*/
                                                                             /*����-5��ʾvlan Already allow in other trunk������-6��ʾ There exists no member in trunk*/
                                                                             /*����-7��ʾVlan NOT allow in trunk������-8��ʾtagMode error in vlan������-9��ʾerror*/
extern int set_port_state(int id,char *slot_port,char *state);  /*slot_port��ʽ"1-1,1-2"��slot ��Χ1-4��port��Χ1-6��ÿ��trunk���add 8��port��state="enable"��"disable"*/
																  /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾUnknow portno format.������-2��ʾport was not member of this trunk*/
																  /*����-3��ʾport already enable������-4��ʾ port not enable������-5��ʾerror*/
extern int set_load_balance(char *mode);	/*mode�б�:based-port|based-mac|based-ip|based-L4|mac+ip|mac+L4|ip+L4|mac+ip+L4*/
		          					        /*����0��ʾʧ�ܣ�����1��ʾ�ɹ�������-1��ʾthere no trunk exist*/
										    /*����-2��ʾload-balance Mode same to corrent mode*/

#endif
