#ifndef __AAT_H__
#define __AAT_H__

#include <linux/netdevice.h>

#define AAT_DEBUG    9
#define AAT_INFO  5
#define AAT_WARNING  3
#define AAT_ERROR    1

#define MAC_LEN		6
#define ETH_LEN		16

#define AAT_OK 	(0)
#define AAT_ERR	(-1)

#define AAT_NEED_DROP_PACKET 2
#define AAT_NORMAL	0

#define STA_HASH(sta) ((sta[2]+sta[3]+sta[4]+sta[5])%HASH_SIZE)
#define AAT_STA_VIP_HASH(vip)  (vip % HASH_SIZE)

#define IPFORMAT	"%u.%u.%u.%u"
#define MACFORMAT	"%02x:%02x:%02x:%02x:%02x:%02x"
#define format_ip(x)	\
 (x>>24)&0xFF,(x>>16)&0xFF,(x>>8)&0xFF,x&0xFF
#define format_mac(x)	\
	x[0],x[1],x[2],x[3],x[4],x[5]


#define PTR_MIN	0x1
#define PTR_MAX 0xffff
#define PTR_KADD 0xA80000
#define IS_INVALID_PTR(p)	\
	(((p >> 40) != PTR_KADD) || (p >= PTR_MIN && p <= PTR_MAX))
//	(((p >> 40) == PTR_KADD) && p >= PTR_MIN && p <= PTR_MAX)

/**
  * Command number for ioctl.
  */
struct io_info
{
	unsigned char stamac[MAC_LEN];
	unsigned char acmac[MAC_LEN];
	unsigned char ifname[ETH_LEN];	
	unsigned char in_ifname[ETH_LEN];	
	unsigned int staip;
	unsigned int vrrid;
	unsigned int seq_num;
};

/* for ipfwd_learn get sta info */
struct aat_sta_info
{
	unsigned char stamac[MAC_LEN];   /* sta mac address */
	unsigned char acmac[MAC_LEN];    /* reserved */
	unsigned char ifname[ETH_LEN];   /* reserved */
	unsigned int staip;              /* sta virtal ip address */
	unsigned int realsip;            /* sta real ip address */
};

typedef enum AAT_STA_GET_TYPE_S
{
	AAT_STA_GET_TYPE_MAC = 0,       /* ipfwd_learn get sta info by MAC */
	AAT_STA_GET_TYPE_VIP = 1,       /* ipfwd_learn get sta info by VIP */
}AAT_STA_GET_TYPE_T;

#define AAT_IOC_MAGIC 249
#define AAT_IOC_MAXNR 0x16

#define AAT_IOC_ADD_STA		_IOWR(AAT_IOC_MAGIC, 1, struct io_info) // read values
#define AAT_IOC_DEL_STA		_IOWR(AAT_IOC_MAGIC, 2, struct io_info) // read values
#define AAT_IOC_GET_STA_IP	_IOWR(AAT_IOC_MAGIC, 3, struct io_info)
/* use vrrid clean stas for HMD */
#define AAT_IOC_CLEAN_STAS	_IOWR(AAT_IOC_MAGIC, 4, struct io_info) 


#define HASH_SIZE		1024

extern int aat_debug;
struct sta_info {
	struct sta_info *next; /* next entry in sta list */
	struct sta_info *hnext; /* next entry in hash table list */
	struct sta_info *vip_hnext; /* next entry in hash table list */
	unsigned char stamac[MAC_LEN];
	unsigned char acmac[MAC_LEN];
	unsigned char ifname[ETH_LEN];	
	unsigned char in_ifname[ETH_LEN];	
	unsigned int staip;
	unsigned char realdmac[MAC_LEN];
	unsigned int realsip;	
	unsigned int vrrid;
};
struct aat_info{
	char ifname[ETH_LEN];
	struct sta_info *sta_list; /* STA info list head */
	struct sta_info *sta_hash[HASH_SIZE];        /* STA MAC hash */
	struct sta_info *sta_vip_hash[HASH_SIZE];    /* STA virtual IP hash */
};

void netlink_with_asd(void);
extern struct aat_info allif;

#endif

