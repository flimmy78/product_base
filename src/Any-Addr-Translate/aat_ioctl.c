#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/if.h>
#include <net/if_inet6.h>
#include <linux/vmalloc.h>
#include "aat.h"
#include "aat_ioctl.h"

struct sta_info *aat_get_stav2(struct aat_info *if_aat, unsigned char *sta, unsigned vrrid)
{
	struct sta_info *s = NULL;

	s = if_aat->sta_hash[STA_HASH(sta)];
		
	while(s != NULL)
	{
		if (0 == memcmp(s->stamac, sta, MAC_LEN) 
			&& (s->vrrid == vrrid))
		{
			return s;
		}
		
		s = s->hnext;
	}

	return NULL;
}

struct sta_info *aat_get_sta(struct aat_info *if_aat, unsigned char *sta)
{
	struct sta_info *s = NULL;

	s = if_aat->sta_hash[STA_HASH(sta)];
	
	while (s != NULL 
		&& memcmp(s->stamac, sta, MAC_LEN) != 0)
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
	struct sta_info *tmp = NULL, *pprev = NULL;
#if 0
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
#else

	if (!if_aat || !sta)
	{
		printk(KERN_ERR"%s-%d list %p sta %p\n", __func__, __LINE__, if_aat, sta);
		return ;
	}
		
	tmp = if_aat->sta_list;
	pprev = NULL;
	while (tmp != NULL)
	{
		if (0 == memcmp(tmp->stamac, sta->stamac, MAC_LEN) 
			&& (tmp->vrrid == sta->vrrid))
		{
			if(NULL == pprev)/* first node in the list */
			{
				if_aat->sta_list = tmp->next;
			}
			else 
			{
				pprev->next = tmp->next; /* node escape */
			}
			return ;
		}
		else 
		{
			pprev = tmp;
			tmp = tmp->next;
		}
	}	
#endif
	return;
}


void if_sta_hash_add(struct aat_info *if_aat, struct sta_info *sta)
{	 
	sta->hnext = if_aat->sta_hash[STA_HASH(sta->stamac)];
	
	if_aat->sta_hash[STA_HASH(sta->stamac)] = sta;

	return;
}


static void if_sta_hash_del(struct aat_info *if_aat, struct sta_info *sta)
{
	struct sta_info *s = NULL, *pprev = NULL;
	unsigned int index = 0;

	if (!if_aat || !sta)
	{
		printk(KERN_ERR"%s-%d list %p sta %p\n", __func__, __LINE__, if_aat, sta);
		return ;
	}

	index = STA_HASH(sta->stamac);
	s = if_aat->sta_hash[index];
	if (s == NULL)
	{
		return;
	}

#if 0	
	if (0 == memcmp(s->stamac, sta->stamac, MAC_LEN)
		&& s->vrrid == sta->vrrid)
	{
		if_aat->sta_hash[STA_HASH(sta->stamac)] = s->hnext;
		return;
	}
		
	while(s->hnext != NULL)
	{
		if (0 == memcmp(s->hnext->stamac, sta->stamac, MAC_LEN)
			&& s->hnext->vrrid == sta->vrrid)
		{
			s->hnext = s->hnext->hnext;
			return;
		}

		s = s->hnext;
	}
#else

	pprev = NULL;
	while (NULL != s)
	{
		if ((0 == memcmp(s->stamac, sta->stamac, MAC_LEN))
			&& (s->vrrid == sta->vrrid))
		{
			if(NULL == pprev) 
			{
				if_aat->sta_hash[index] = s->hnext;
			}
			else 
			{
				pprev->hnext = s->hnext;
			}
			return;
		}
		else 
		{
			pprev = s;
			s = s->hnext;
		}
	}

#endif

	return;
}

void if_sta_vip_hash_add
(
	struct aat_info *if_aat,
	struct sta_info *sta
)
{
	unsigned int index = 0;

	if (NULL == if_aat || NULL == sta)
	{
		return ;
	}

	index = AAT_STA_VIP_HASH(sta->staip);
	
	sta->vip_hnext = if_aat->sta_vip_hash[index];
	
	if_aat->sta_vip_hash[index] = sta;

	return;
}


static void if_sta_vip_hash_del
(
	struct aat_info *if_aat,
	struct sta_info *sta
)
{
	struct sta_info *s = NULL , *pprev = NULL;
	unsigned int index = 0;
	
	if (NULL == if_aat || NULL == sta)
	{
		return;
	}

	index = AAT_STA_VIP_HASH(sta->staip);

	s = if_aat->sta_vip_hash[index];
	if (s == NULL)
	{
		return;
	}
#if 0
	/* found sta is first node in vip_hash */
	if (s->staip == sta->staip)
	{
		if (s != sta)
		{
			printk("%s %d s %p vip "IPFORMAT" no match sta %p vip "IPFORMAT"\n",
							__func__, __LINE__, s, format_ip(s->staip), sta, format_ip(sta->staip));
		}
		#if 0
		else
		{
			printk("%s %d s %p vip "IPFORMAT" match sta %p vip "IPFORMAT"\n",
							__func__, __LINE__, s, format_ip(s->staip), sta, format_ip(sta->staip));
		}
		#endif
		
		if_aat->sta_vip_hash[index] = s->vip_hnext;
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
		if (s->vip_hnext != sta)
		{
			printk("%s %d s %p vip "IPFORMAT" no match sta %p vip "IPFORMAT"\n",
							__func__, __LINE__, s->vip_hnext, format_ip(s->vip_hnext->staip), sta, format_ip(sta->staip));
		}
		#if 0
		else
		{
			printk("%s %d s %p vip "IPFORMAT" match sta %p vip "IPFORMAT"\n",
							__func__, __LINE__, s->vip_hnext, format_ip(s->vip_hnext->staip), sta, format_ip(sta->staip));
		}
		#endif
		
		/* found the sta, and remove it from vip_hash */
		s->vip_hnext = s->vip_hnext->vip_hnext;
	}
#else
	pprev = NULL;
	while (NULL != s)
	{
		if (IS_INVALID_PTR((unsigned long)s))
		{
			if(0 == IS_INVALID_PTR((unsigned long)pprev) && pprev) {
				printk(KERN_ERR"%s-%d invalid addr %p pprev %p(%p %p %p "IPFORMAT" "MACFORMAT" index %d (hash arg "IPFORMAT")\n",
							__func__, __LINE__,
							s, pprev,
							pprev->next,pprev->hnext,pprev->vip_hnext,
							format_ip(pprev->staip), format_mac(pprev->stamac),
							index, format_ip(sta->staip));
				#if 1
				if (0 == IS_INVALID_PTR((unsigned long)pprev->next) && pprev->next)
				{
					printk("%s-%d pprev->next (%p %p %p "MACFORMAT" "IPFORMAT")\n",
							__func__, __LINE__,
							pprev->next->next, pprev->next->hnext, pprev->next->vip_hnext,
							format_mac(pprev->next->stamac), format_ip(pprev->next->staip));
				}
				if (0 == IS_INVALID_PTR((unsigned long)pprev->hnext) && pprev->hnext)
				{
					printk("%s-%d pprev->hnext (%p %p %p "MACFORMAT" "IPFORMAT")\n",
							__func__, __LINE__,
							pprev->hnext->next, pprev->hnext->hnext, pprev->hnext->vip_hnext,
							format_mac(pprev->hnext->stamac), format_ip(pprev->hnext->staip));
				}
				#endif
			}
			else {
				printk(KERN_ERR"%s-%d invalid addr %p pprev %p\n", __func__, __LINE__, s, pprev);
			}
			return;
		}
		
		if (0 == memcmp(s->stamac, sta->stamac, MAC_LEN)
			&& (s->staip == sta->staip)
			&& (s->vrrid == sta->vrrid))
		{
			if(NULL == pprev) 
			{
				if_aat->sta_vip_hash[index] = s->vip_hnext;
			}
			else 
			{
				pprev->vip_hnext = s->vip_hnext;
			}
			if ((s->vip_hnext != sta->vip_hnext) || (s != sta))
			{
				printk(KERN_ERR"%s-%d sta "MACFORMAT" "IPFORMAT" vrid %d hash node mismatch %p(%p) %p(%p)\n",
					__func__, __LINE__,format_mac(sta->stamac), format_ip(sta->staip), sta->vrrid, \
					s, s->vip_hnext, sta, sta->vip_hnext);
			}
			return;
		}
		else 
		{
			if((s->staip == sta->staip)&&(s->vrrid == sta->vrrid))
			{
				printk(KERN_ERR"%s-%d sta "MACFORMAT" "IPFORMAT" vrid %d mac mismatch "MACFORMAT"\n",
					__func__, __LINE__,format_mac(sta->stamac),format_ip(sta->staip), sta->vrrid,format_mac(s->stamac));
			}
			pprev = s;
			s = s->vip_hnext;
		}
	}

	printk(KERN_ERR"%s-%d no found "MACFORMAT" "IPFORMAT"\n",
			__func__, __LINE__, format_mac(sta->stamac), format_ip(sta->staip));
#endif
	return;
}

struct sta_info* aat_add_sta(struct io_info * ioinfo)
{
	struct sta_info *tmp = NULL;

	if(aat_debug > AAT_INFO)
	{
		printk("%s-%d cmd %#x %u sta %02X:%02X:%02X:%02X:%02X:%02X ac %02X:%02X:%02X:%02X:%02X:%02X %u.%u.%u.%u %s %d\n",
			__func__, __LINE__, (unsigned int)AAT_IOC_ADD_STA, ioinfo->seq_num,
			ioinfo->stamac[0], ioinfo->stamac[1],ioinfo->stamac[2],
			ioinfo->stamac[3], ioinfo->stamac[4], ioinfo->stamac[5],
			ioinfo->acmac[0], ioinfo->acmac[1],ioinfo->acmac[2],
			ioinfo->acmac[3], ioinfo->acmac[4], ioinfo->acmac[5], \
			(ioinfo->staip>>24)&0xFF,(ioinfo->staip>>16)&0xFF, \
			(ioinfo->staip>>8)&0xFF,(ioinfo->staip)&0xFF,
			ioinfo->ifname, ioinfo->vrrid);
	}

	tmp = aat_get_stav2(&allif, ioinfo->stamac, ioinfo->vrrid);
	if (tmp != NULL)
	{
		#if 1
		memcpy(tmp->acmac, ioinfo->acmac, MAC_LEN);
		
		if (strcmp(tmp->ifname, ioinfo->ifname))
		{
			printk("%s-%d  cmd %#x %u old sta %02X:%02X:%02X:%02X:%02X:%02X intf change %s  to %s \n",
				__func__, __LINE__, (unsigned int)AAT_IOC_ADD_STA, ioinfo->seq_num,
				tmp->stamac[0], tmp->stamac[1], tmp->stamac[2],tmp->stamac[3], tmp->stamac[4], tmp->stamac[5],
				tmp->ifname, ioinfo->ifname);			
			tmp->realsip = 0;
		}
		else 
		{
			printk("%s-%d  cmd %#x %u old sta %02X:%02X:%02X:%02X:%02X:%02X update\n",
				__func__, __LINE__, (unsigned int)AAT_IOC_ADD_STA, ioinfo->seq_num,
				tmp->stamac[0], tmp->stamac[1], tmp->stamac[2],
				tmp->stamac[3], tmp->stamac[4], tmp->stamac[5]);
		}
		memcpy(tmp->ifname, ioinfo->ifname, ETH_LEN);
		//memcpy(tmp->in_ifname, ioinfo->in_ifname, ETH_LEN);

		if (tmp->staip != ioinfo->staip)
		{
			if (aat_debug >= AAT_DEBUG)
			{
				printk("%s-%d  cmd %#x %u old sta %02X:%02X:%02X:%02X:%02X:%02X update vip "IPFORMAT" to "IPFORMAT"\n",
					__func__, __LINE__, (unsigned int)AAT_IOC_ADD_STA, ioinfo->seq_num,
					tmp->stamac[0], tmp->stamac[1], tmp->stamac[2],
					tmp->stamac[3], tmp->stamac[4], tmp->stamac[5],
					format_ip(tmp->staip), format_ip(ioinfo->staip));
				printk("%s-%d  cmd %#x %u old sta %02X:%02X:%02X:%02X:%02X:%02X update... %p(%p %p %p)\n",
					__func__, __LINE__, (unsigned int)AAT_IOC_ADD_STA, ioinfo->seq_num,
					tmp->stamac[0], tmp->stamac[1], tmp->stamac[2],
					tmp->stamac[3], tmp->stamac[4], tmp->stamac[5],
					tmp, tmp->next, tmp->hnext, tmp->vip_hnext);
			}

			/* here, sta vip maybe changed, so
			 * 1. remove sta old vip from vip_hash */
			if_sta_vip_hash_del(&allif, tmp);
			tmp->vip_hnext = NULL;
			
			tmp->staip = ioinfo->staip;
			/* 2. add sta new vip to vip_hash */
			if_sta_vip_hash_add(&allif, tmp);

			if (aat_debug >= AAT_DEBUG)
			{
				printk("%s-%d  cmd %#x %u old sta %02X:%02X:%02X:%02X:%02X:%02X update done %p(%p %p %p)\n",
					__func__, __LINE__, (unsigned int)AAT_IOC_ADD_STA, ioinfo->seq_num,
					tmp->stamac[0], tmp->stamac[1], tmp->stamac[2],
					tmp->stamac[3], tmp->stamac[4], tmp->stamac[5],
					tmp, tmp->next, tmp->hnext, tmp->vip_hnext);
			}
		}
		
		return tmp;		
		#else
		printk("%s-%d  cmd %#x %u free old sta %02X:%02X:%02X:%02X:%02X:%02X\n",
				__func__, __LINE__, (unsigned int)AAT_IOC_ADD_STA, ioinfo->seq_num,
				tmp->stamac[0], tmp->stamac[1], tmp->stamac[2],
				tmp->stamac[3], tmp->stamac[4], tmp->stamac[5]);
		if_sta_list_del(&allif, tmp);
		if_sta_hash_del(&allif, tmp);
		if_sta_vip_hash_del(&allif, tmp);
		
		kfree(tmp);
		tmp = NULL;

		#endif
	}
	
	tmp =(struct sta_info*)kmalloc(sizeof(struct sta_info), GFP_ATOMIC);
	if (NULL == tmp)
	{
		printk("%s-%d kmalloc for sta node fail\n", __func__, __LINE__);
		return NULL;
	}
	memset(tmp, 0, sizeof(struct sta_info));

	if (aat_debug >= AAT_DEBUG)
	{
		printk("%s-%d kmalloc tmp %p(next %p hnext %p vip_hnext %p)\n",
				__func__, __LINE__,
				tmp, tmp->next, tmp->hnext, tmp->vip_hnext);
	}

	tmp->next = NULL;
	tmp->hnext = NULL;
	tmp->vip_hnext = NULL;

	memcpy(tmp->stamac, ioinfo->stamac, MAC_LEN);	
	memcpy(tmp->acmac, ioinfo->acmac, MAC_LEN);

	if(aat_debug > AAT_INFO)
	{
		printk("%s-%d cmd %#x %u new sta %02X:%02X:%02X:%02X:%02X:%02X ac %02X:%02X:%02X:%02X:%02X:%02X\n",
			__func__,__LINE__, (unsigned int)AAT_IOC_ADD_STA, ioinfo->seq_num,
			tmp->stamac[0],tmp->stamac[1],tmp->stamac[2],tmp->stamac[3],tmp->stamac[4],tmp->stamac[5],
			tmp->acmac[0],tmp->acmac[1],tmp->acmac[2],tmp->acmac[3],tmp->acmac[4],tmp->acmac[5]);
	}		
	
	memcpy(tmp->ifname, ioinfo->ifname, ETH_LEN);
	//memcpy(tmp->in_ifname, ioinfo->in_ifname, ETH_LEN);
	tmp->staip = ioinfo->staip;
	tmp->vrrid = ioinfo->vrrid;

	tmp->next = allif.sta_list;
	allif.sta_list = tmp;
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
		printk("%s-%d cmd %#x %u error no sta %02X:%02X:%02X:%02X:%02X:%02X ac %02X:%02X:%02X:%02X:%02X:%02X %u.%u.%u.%u %s %d\n",
					__func__, __LINE__, (unsigned int)AAT_IOC_DEL_STA, ioinfo->seq_num,
					ioinfo->stamac[0], ioinfo->stamac[1],ioinfo->stamac[2],
					ioinfo->stamac[3], ioinfo->stamac[4], ioinfo->stamac[5],
					ioinfo->acmac[0], ioinfo->acmac[1],ioinfo->acmac[2],
					ioinfo->acmac[3], ioinfo->acmac[4], ioinfo->acmac[5],
					(ioinfo->staip>>24)&0xFF,(ioinfo->staip>>16)&0xFF, \
					(ioinfo->staip>>8)&0xFF,(ioinfo->staip)&0xFF,
					ioinfo->ifname, ioinfo->vrrid);
		
		return 0;
	}
	if(aat_debug > AAT_INFO)
	{
		printk("%s-%d cmd %#x %u sta %02X:%02X:%02X:%02X:%02X:%02X ac %02X:%02X:%02X:%02X:%02X:%02X %u.%u.%u.%u %s %s %d ok\n",
					__func__, __LINE__, (unsigned int)AAT_IOC_DEL_STA,ioinfo->seq_num,
					ioinfo->stamac[0], ioinfo->stamac[1],ioinfo->stamac[2],
					ioinfo->stamac[3], ioinfo->stamac[4], ioinfo->stamac[5],
					ioinfo->acmac[0], ioinfo->acmac[1],ioinfo->acmac[2],
					ioinfo->acmac[3], ioinfo->acmac[4], ioinfo->acmac[5],
					(ioinfo->staip>>24)&0xFF,(ioinfo->staip>>16)&0xFF, \
					(ioinfo->staip>>8)&0xFF,(ioinfo->staip)&0xFF,
					ioinfo->ifname, ioinfo->in_ifname, ioinfo->vrrid);
	}
	
	if_sta_list_del(&allif, tmp);
	if_sta_hash_del(&allif, tmp);
	if_sta_vip_hash_del(&allif, tmp);
	
	kfree(tmp);
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
				printk("clean HANSI %d STA %02X:%02X:%02X:%02X:%02X:%02X\n",
						entry->vrrid,
						entry->stamac[0], entry->stamac[1], entry->stamac[2],
						entry->stamac[3], entry->stamac[4], entry->stamac[5]);
			}
			
			*tmp = entry->next;
			
			if_sta_list_del(&allif, entry);
			if_sta_hash_del(&allif, entry);
			if_sta_vip_hash_del(&allif, entry);
			
			kfree(entry);
			entry = NULL;
        }
        else
        {
            tmp = &entry->next;
        }
    }
 
    return 0;
}


