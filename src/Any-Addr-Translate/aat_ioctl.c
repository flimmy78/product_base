#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/if.h>
#include <net/if_inet6.h>
#include <linux/vmalloc.h>
#include "aat.h"
#define STA_HASH(sta) (sta[2]+sta[3]+sta[4]+sta[5])
#define AAT_STA_VIP_HASH(vip)  (vip % HASH_SIZE)

struct sta_info *aat_get_stav2(struct aat_info *if_aat, unsigned char *sta, unsigned vrrid)
{
	struct sta_info *s = NULL;

	s = if_aat->sta_hash[STA_HASH(sta)];

    while (s != NULL)
    {
    	if (memcmp(s->stamac, sta, 6) == 0
    	&& s->vrrid == vrrid)
    	{
    		return s;
    	}

		s = s->hnext;
    }
	
    return s;
	
}

struct sta_info *aat_get_sta(struct aat_info *if_aat, unsigned char *sta)
{
	struct sta_info *s = NULL;

	s = if_aat->sta_hash[STA_HASH(sta)];
	while (s != NULL
		   && memcmp(s->stamac, sta, 6) != 0)
	{
		s = s->hnext;
	}
	
	return s;
}
/* get sta info by vip */
struct sta_info *aat_get_sta_by_vip
(
	struct aat_info *if_aat,
	unsigned int sta_vip
)
{
	struct sta_info *s = NULL;

	if (NULL == if_aat)
	{
		return NULL;
	}

	s = if_aat->sta_vip_hash[AAT_STA_VIP_HASH(sta_vip)];
	while (s != NULL
		   && s->staip != sta_vip)
	{
		s = s->vip_hnext;
	}
	
	return s;
}


static void if_sta_list_del(struct aat_info *if_aat, struct sta_info *sta)
{
	struct sta_info *tmp = NULL;

	if (if_aat->sta_list == sta)
	{
		if_aat->sta_list = sta->next;
		return;
	}

	tmp = if_aat->sta_list;
	while (tmp != NULL && tmp->next != sta)
	{
		tmp = tmp->next;
	}
	
	if (tmp != NULL)
	{
		tmp->next = sta->next;
	}
}


void if_sta_hash_add(struct aat_info *if_aat, struct sta_info *sta)
{
	sta->hnext = if_aat->sta_hash[STA_HASH(sta->stamac)];
	if_aat->sta_hash[STA_HASH(sta->stamac)] = sta;
}


static void if_sta_hash_del(struct aat_info *if_aat, struct sta_info *sta)
{
	struct sta_info *s = NULL;

	s = if_aat->sta_hash[STA_HASH(sta->stamac)];
	if (s == NULL)
	{
		return;
	}
	
	if (memcmp(s->stamac, sta->stamac, 6) == 0
		&& s->vrrid == sta->vrrid)
	{
		if_aat->sta_hash[STA_HASH(sta->stamac)] = s->hnext;
		return;
	}


    while (s->hnext != NULL)
    {
        if (memcmp(s->hnext->stamac, sta->stamac, ETH_ALEN) == 0
           && s->vrrid == sta->vrrid)
        {
        	s->hnext = s->hnext->hnext;
        	return ;
    	}
    	s = s->hnext; 
    }
    return ;


	
}

void if_sta_vip_hash_add
(
	struct aat_info *if_aat,
	struct sta_info *sta
)
{
	if (NULL == if_aat || NULL == sta)
	{
		return ;
	}
	
	sta->vip_hnext = if_aat->sta_vip_hash[AAT_STA_VIP_HASH(sta->staip)];
	if_aat->sta_vip_hash[AAT_STA_VIP_HASH(sta->staip)] = sta;
}


static void if_sta_vip_hash_del
(
	struct aat_info *if_aat,
	struct sta_info *sta
)
{
	struct sta_info *s = NULL;

	if (NULL == if_aat || NULL == sta)
	{
		return;
	}

	s = if_aat->sta_vip_hash[AAT_STA_VIP_HASH(sta->staip)];
	if (s == NULL)
	{
		return;
	}

	/* found sta is first node in vip_hash */
	if (s->staip == sta->staip)
	{
		if_aat->sta_vip_hash[AAT_STA_VIP_HASH(sta->staip)] = s->vip_hnext;
		return;
	}

	/* not first node in vip_hash */
	while (s->vip_hnext != NULL
		   && (s->vip_hnext->staip != sta->staip))
	{
		s = s->vip_hnext;
	}
	
	if (s->vip_hnext != NULL)
	{
		/* found the sta, and remove it from vip_hash */
		s->vip_hnext = s->vip_hnext->vip_hnext;
	}
}

struct sta_info* aat_add_sta(struct io_info * ioinfo)
{
	if(aat_debug >= AAT_DEBUG){
		printk("%s,%d,ioinfo->stamac:%2X:%2X:%2X:%2X:%2X:%2X.\n",__func__,__LINE__,ioinfo->stamac[0],ioinfo->stamac[1],ioinfo->stamac[2],ioinfo->stamac[3],ioinfo->stamac[4],ioinfo->stamac[5]);
		printk("%s,%d,ioinfo->acmac:%2X:%2X:%2X:%2X:%2X:%2X.\n",__func__,__LINE__,ioinfo->acmac[0],ioinfo->acmac[1],ioinfo->acmac[2],ioinfo->acmac[3],ioinfo->acmac[4],ioinfo->acmac[5]);
		printk("%s,%d,ioinfo->staip %d.%d.%d.%d.\n", \
				__func__,__LINE__,((ioinfo->staip) >> 24) & 0xFF, ((ioinfo->staip) >> 16) & 0xFF,((ioinfo->staip) >> 8) & 0xFF, (ioinfo->staip) & 0xFF);
	}
	
	struct sta_info * tmp = NULL;
	tmp = aat_get_stav2(&allif, ioinfo->stamac, ioinfo->vrrid);
	if(tmp != NULL)
	{
		if(aat_debug >= AAT_DEBUG){
			printk("%s,%d,tmp->stamac:%2X:%2X:%2X:%2X:%2X:%2X.\n",__func__,__LINE__,tmp->stamac[0],tmp->stamac[1],tmp->stamac[2],tmp->stamac[3],tmp->stamac[4],tmp->stamac[5]);
			printk("%s,%d,tmp->acmac:%2X:%2X:%2X:%2X:%2X:%2X.\n",__func__,__LINE__,tmp->acmac[0],tmp->acmac[1],tmp->acmac[2],tmp->acmac[3],tmp->acmac[4],tmp->acmac[5]);
			printk("%s,%d,tmp->staip %d.%d.%d.%d.\n", \
					__func__,__LINE__,((tmp->staip) >> 24) & 0xFF, ((tmp->staip) >> 16) & 0xFF,((tmp->staip) >> 8) & 0xFF, (tmp->staip) & 0xFF);
		}
		memcpy(tmp->acmac, ioinfo->acmac, MAC_LEN);
		memcpy(tmp->ifname, ioinfo->ifname, ETH_LEN);
		memcpy(tmp->in_ifname, ioinfo->in_ifname, ETH_LEN);
		
		/* here, sta vip maybe changed, so
		 * 1. remove sta old vip from vip_hash */
		if_sta_vip_hash_del(&allif, tmp);
		
		tmp->staip = ioinfo->staip;
		/* 2. add sta new vip to vip_hash */
		if_sta_vip_hash_add(&allif, tmp);
		
		return tmp;		
	}
	
	tmp =(struct sta_info*)vmalloc(sizeof(struct sta_info));
	memset(tmp, 0, sizeof(struct sta_info));
	tmp->next = allif.sta_list;
	allif.sta_list = tmp;
	memcpy(tmp->stamac, ioinfo->stamac, MAC_LEN);	
	memcpy(tmp->acmac, ioinfo->acmac, MAC_LEN);
	if(aat_debug >= AAT_DEBUG){
		printk("%s,%d,tmp->stamac:%2X:%2X:%2X:%2X:%2X:%2X.\n",__func__,__LINE__,tmp->stamac[0],tmp->stamac[1],tmp->stamac[2],tmp->stamac[3],tmp->stamac[4],tmp->stamac[5]);
		printk("%s,%d,tmp->acmac:%2X:%2X:%2X:%2X:%2X:%2X.\n",__func__,__LINE__,tmp->acmac[0],tmp->acmac[1],tmp->acmac[2],tmp->acmac[3],tmp->acmac[4],tmp->acmac[5]);
	}
	memcpy(tmp->ifname, ioinfo->ifname, ETH_LEN);
	memcpy(tmp->in_ifname, ioinfo->in_ifname, ETH_LEN);
	tmp->staip = ioinfo->staip;
	tmp->vrrid = ioinfo->vrrid;
	if(aat_debug >= AAT_DEBUG)
	{
		printk("%s,%d,tmp->staip %d.%d.%d.%d.\n", \
			__func__,__LINE__,((tmp->staip) >> 24) & 0xFF, ((tmp->staip) >> 16) & 0xFF,((tmp->staip) >> 8) & 0xFF, (tmp->staip) & 0xFF);
	}
	if_sta_hash_add(&allif, tmp);
	if_sta_vip_hash_add(&allif, tmp);
	
	return tmp;
}
int aat_del_sta(struct io_info * ioinfo)
{
	struct sta_info * tmp = NULL;
	
	tmp = aat_get_stav2(&allif, ioinfo->stamac, ioinfo->vrrid);
	if(NULL == tmp)
	{
		return 0;
	}
	if(aat_debug >= AAT_DEBUG)
	{
		printk("%s,%d,tmp->stamac:%2X:%2X:%2X:%2X:%2X:%2X.\n",__func__,__LINE__,tmp->stamac[0],tmp->stamac[1],tmp->stamac[2],tmp->stamac[3],tmp->stamac[4],tmp->stamac[5]);
		printk("%s,%d,tmp->staip %d.%d.%d.%d.\n", \
			__func__,__LINE__,((tmp->staip) >> 24) & 0xFF, ((tmp->staip) >> 16) & 0xFF,((tmp->staip) >> 8) & 0xFF, (tmp->staip) & 0xFF);
	}
	if_sta_list_del(&allif, tmp);
	if_sta_hash_del(&allif, tmp);
	if_sta_vip_hash_del(&allif, tmp);
	
	vfree(tmp);
	tmp = NULL;
	
	return 0;
}

int aat_clean_hansi_stas(struct sta_info **stalist, unsigned int vrrid)
{
	struct sta_info **tmp = NULL;

	if (NULL == stalist)
	{
		return -1;
	}

    for (tmp = stalist; *tmp; )
    {
        struct sta_info *entry = *tmp;
        if (entry->vrrid == vrrid)
        {
			if (aat_debug >= AAT_DEBUG)
			{
				printk("clean HANSI %d STA %2X:%2X:%2X:%2X:%2X:%2X.\n",
						entry->vrrid,
						entry->stamac[0], entry->stamac[1], entry->stamac[2],
						entry->stamac[3], entry->stamac[4], entry->stamac[5]);
			}
            *tmp = entry->next;
            if_sta_hash_del(&allif, entry);
			vfree(entry);
			entry = NULL;
        }
        else
        {
            tmp = &entry->next;
        }
    }
 
    return 0;
}


