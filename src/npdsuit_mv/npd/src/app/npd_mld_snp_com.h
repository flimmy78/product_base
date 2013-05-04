#ifndef __NPD_MLD_SNP_COM_H__
#define __NPD_MLD_SNP_COM_H__

/* include header file begin */
/* kernel or sys part */
#include <sys/socket.h>
#include <linux/netlink.h>
/* user or app part */
/* include header file end */

#define SIZE_OF_IPV6_ADDR 8

#define ISMLD 1
#define ISIGMP 2

typedef struct mld_notify_mod_npd_s {
	unsigned long	mod_type;
	long	vlan_id;
	unsigned long	ifindex;
	unsigned short	groupadd[SIZE_OF_IPV6_ADDR];
	unsigned long	reserve;
}mld_notify_mod_npd;

struct mld_msg_npd {
	struct nlmsghdr nlh;
	mld_notify_mod_npd mld_noti_npd;
};

#define ADDRSHOWV6(addr) \
	(addr)[0], \
	(addr)[1], \
	(addr)[2], \
	(addr)[3], \
	(addr)[4], \
	(addr)[5], \
	(addr)[6], \
	(addr)[7]


enum mldmodaddtype
{
	MLD_ADDR_ADD,
	MLD_ADDR_DEL,
	MLD_ADDR_RMV,
	MLD_SYS_SET,
	MLD_TRUNK_UPDATE
};


#define MLD_SYS_SET_INIT	1
#define MLD_SYS_SET_STOP	2



/*IGMP Snoop kernel message type*/
#define	MLD_SNP_TYPE_MIN				0
#define	MLD_SNP_TYPE_NOTIFY_MSG		4	/*notify message*/
#define	MLD_SNP_TYPE_PACKET_MSG		5	/*Packet message*/
#define MLD_SNP_TYPE_DEVICE_EVENT		6	/*device message*/
#define	MLD_SNP_TYPE_MAX				9

/*IGMP Snoop message flag*/
#define	MLD_SNP_FLAG_MIN				0
#define	MLD_SNP_FLAG_PKT_UNKNOWN	4	/*Unknown packet*/
#define	MLD_SNP_FLAG_PKT_MLD		5	/*MLD,PIM packet*/
#define	MLD_SNP_FLAG_ADDR_MOD		6	/*notify information for modify address*/
#define	MLD_SNP_FLAG_MAX				9

/*local function declarations */
unsigned int npd_mld_v6addr_equal_0(unsigned short * group);
unsigned int npd_mld_ntohs(unsigned short ** group_ip);
unsigned int npd_mld_htons(unsigned short ** group_ip);
unsigned int npd_mld_copy_ipv6addr(unsigned short *ugroup,unsigned short **destugroup);
unsigned int npd_mld_compare_ipv6addr(unsigned short *addr1, unsigned short *addr2);
unsigned int npd_mld_fdb_entry_addto(unsigned short vid,unsigned short *groupIp,unsigned short vidx);
unsigned int npd_mld_snp_recvmsg_proc(mld_notify_mod_npd * mld_notify);
int npd_check_mld_snp_status(unsigned char *status);

/*local function declarations end*/
extern unsigned char mldSnpEnable;

extern int npd_igmp_sysmac_notifer(void);
extern int nam_static_fdb_entry_indirect_delete_for_mld(unsigned int dip,unsigned short vid);
extern int nam_static_fdb_entry_indirect_set_for_mld(unsigned int dip,unsigned short vidx,unsigned short vlanId);
extern unsigned int npd_igmp_clear_fdb_list_new();
extern int npd_vlan_igmp_snp_endis_config(unsigned char	isAdd, unsigned short vid);
extern int npd_default_vlan_port_mbr_igmp_enable(unsigned short vlanId, unsigned int enDis);


#endif
