/*
 *	Forwarding database
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/rculist.h>
#include <linux/spinlock.h>
#include <linux/times.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/jhash.h>
#include <linux/random.h>
#include <asm/atomic.h>
#include <asm/unaligned.h>
#include "br_private.h"

static struct kmem_cache *br_fdb_cache __read_mostly;
static int fdb_insert(struct net_bridge *br, struct net_bridge_port *source,
		      const unsigned char *addr);

static u32 fdb_salt __read_mostly;

int __init br_fdb_init(void)
{
	br_fdb_cache = kmem_cache_create("bridge_fdb_cache",
					 sizeof(struct net_bridge_fdb_entry),
					 0,
					 SLAB_HWCACHE_ALIGN, NULL);
	if (!br_fdb_cache)
		return -ENOMEM;

	get_random_bytes(&fdb_salt, sizeof(fdb_salt));
	return 0;
}

void br_fdb_fini(void)
{
	kmem_cache_destroy(br_fdb_cache);
}


/* if topology_changing then use forward_delay (default 15 sec)
 * otherwise keep longer (default 5 minutes)
 */
static inline unsigned long hold_time(const struct net_bridge *br)
{
	return br->topology_change ? br->forward_delay : br->ageing_time;
}

static inline int has_expired(const struct net_bridge *br,
				  const struct net_bridge_fdb_entry *fdb)
{
	return !fdb->is_static
		&& time_before_eq(fdb->ageing_timer + hold_time(br), jiffies);
}

static inline int br_mac_hash(const unsigned char *mac)
{
	/* use 1 byte of OUI cnd 3 bytes of NIC */
	u32 key = get_unaligned((u32 *)(mac + 2));
	return jhash_1word(key, fdb_salt) & (BR_HASH_SIZE - 1);
}

static void fdb_rcu_free(struct rcu_head *head)
{
	struct net_bridge_fdb_entry *ent
		= container_of(head, struct net_bridge_fdb_entry, rcu);
	kmem_cache_free(br_fdb_cache, ent);
}

static inline void fdb_delete(struct net_bridge_fdb_entry *f)
{
	hlist_del_rcu(&f->hlist);
	call_rcu(&f->rcu, fdb_rcu_free);
}

void br_fdb_changeaddr(struct net_bridge_port *p, const unsigned char *newaddr)
{
	struct net_bridge *br = p->br;
	int i;

	spin_lock_bh(&br->hash_lock);

	/* Search all chains since old address/hash is unknown */
	for (i = 0; i < BR_HASH_SIZE; i++) {
		struct hlist_node *h;
		hlist_for_each(h, &br->hash[i]) {
			struct net_bridge_fdb_entry *f;

			f = hlist_entry(h, struct net_bridge_fdb_entry, hlist);
			if (f->dst == p && f->is_local) {
				/* maybe another port has same hw addr? */
				struct net_bridge_port *op;
				list_for_each_entry(op, &br->port_list, list) {
					if (op != p &&
					    !compare_ether_addr(op->dev->dev_addr,
								f->addr.addr)) {
						f->dst = op;
						goto insert;
					}
				}

				/* delete old one */
				fdb_delete(f);
				goto insert;
			}
		}
	}
 insert:
	/* insert new address,  may fail if invalid address or dup. */
	fdb_insert(br, p, newaddr);

	spin_unlock_bh(&br->hash_lock);
}

void br_fdb_cleanup(unsigned long _data)
{
	struct net_bridge *br = (struct net_bridge *)_data;
	unsigned long delay = hold_time(br);
	unsigned long next_timer = jiffies + br->forward_delay;
	int i;

	spin_lock_bh(&br->hash_lock);
	for (i = 0; i < BR_HASH_SIZE; i++) {
		struct net_bridge_fdb_entry *f;
		struct hlist_node *h, *n;

		hlist_for_each_entry_safe(f, h, n, &br->hash[i], hlist) {
			unsigned long this_timer;
			if (f->is_static)
				continue;
			this_timer = f->ageing_timer + delay;
			if (time_before_eq(this_timer, jiffies))
				fdb_delete(f);
			else if (time_before(this_timer, next_timer))
				next_timer = this_timer;
		}
	}
	spin_unlock_bh(&br->hash_lock);

	/* Add HZ/4 to ensure we round the jiffies upwards to be after the next
	 * timer, otherwise we might round down and will have no-op run. */
	mod_timer(&br->gc_timer, round_jiffies(next_timer + HZ/4));
}

/* Completely flush all dynamic entries in forwarding database.*/
void br_fdb_flush(struct net_bridge *br)
{
	int i;

	spin_lock_bh(&br->hash_lock);
	for (i = 0; i < BR_HASH_SIZE; i++) {
		struct net_bridge_fdb_entry *f;
		struct hlist_node *h, *n;
		hlist_for_each_entry_safe(f, h, n, &br->hash[i], hlist) {
			if (!f->is_static)
				fdb_delete(f);
		}
	}
	spin_unlock_bh(&br->hash_lock);
}

/* Flush all entries refering to a specific port.
 * if do_all is set also flush static entries
 */
void br_fdb_delete_by_port(struct net_bridge *br,
			   const struct net_bridge_port *p,
			   int do_all)
{
	int i;

	spin_lock_bh(&br->hash_lock);
	for (i = 0; i < BR_HASH_SIZE; i++) {
		struct hlist_node *h, *g;

		hlist_for_each_safe(h, g, &br->hash[i]) {
			struct net_bridge_fdb_entry *f
				= hlist_entry(h, struct net_bridge_fdb_entry, hlist);
			if (f->dst != p)
				continue;

			if (f->is_static && !do_all)
				continue;
			/*
			 * if multiple ports all have the same device address
			 * then when one port is deleted, assign
			 * the local entry to other port
			 */
			if (f->is_local) {
				struct net_bridge_port *op;
				list_for_each_entry(op, &br->port_list, list) {
					if (op != p &&
					    !compare_ether_addr(op->dev->dev_addr,
								f->addr.addr)) {
						f->dst = op;
						goto skip_delete;
					}
				}
			}

			fdb_delete(f);
		skip_delete: ;
		}
	}
	spin_unlock_bh(&br->hash_lock);
}

/* No locking or refcounting, assumes caller has no preempt (rcu_read_lock) */
struct net_bridge_fdb_entry *__br_fdb_get(struct net_bridge *br,
					  const unsigned char *addr)
{
	struct hlist_node *h;
	struct net_bridge_fdb_entry *fdb;

	hlist_for_each_entry_rcu(fdb, h, &br->hash[br_mac_hash(addr)], hlist) {
		if (!compare_ether_addr(fdb->addr.addr, addr)) {
			if (unlikely(has_expired(br, fdb)))
				break;
			return fdb;
		}
	}

	return NULL;
}

#if defined(CONFIG_ATM_LANE) || defined(CONFIG_ATM_LANE_MODULE)
/* Interface used by ATM LANE hook to test
 * if an addr is on some other bridge port */
int br_fdb_test_addr(struct net_device *dev, unsigned char *addr)
{
	struct net_bridge_fdb_entry *fdb;
	int ret;

	if (!dev->br_port)
		return 0;

	rcu_read_lock();
	fdb = __br_fdb_get(dev->br_port->br, addr);
	ret = fdb && fdb->dst->dev != dev &&
		fdb->dst->state == BR_STATE_FORWARDING;
	rcu_read_unlock();

	return ret;
}
#endif /* CONFIG_ATM_LANE */

/*
 * Fill buffer with forwarding table records in
 * the API format.
 */
int br_fdb_fillbuf(struct net_bridge *br, void *buf,
		   unsigned long maxnum, unsigned long skip)
{
	struct __fdb_entry *fe = buf;
	int i, num = 0;
	struct hlist_node *h;
	struct net_bridge_fdb_entry *f;

	memset(buf, 0, maxnum*sizeof(struct __fdb_entry));

	rcu_read_lock();
	for (i = 0; i < BR_HASH_SIZE; i++) {
		hlist_for_each_entry_rcu(f, h, &br->hash[i], hlist) {
			if (num >= maxnum)
				goto out;

			if (has_expired(br, f))
				continue;

			if (skip) {
				--skip;
				continue;
			}

			/* convert from internal format to API */
			memcpy(fe->mac_addr, f->addr.addr, ETH_ALEN);

			/* due to ABI compat need to split into hi/lo */
			fe->port_no = f->dst->port_no;
			fe->port_hi = f->dst->port_no >> 8;

			fe->is_local = f->is_local;
			if (!f->is_static)
				fe->ageing_timer_value = jiffies_to_clock_t(jiffies - f->ageing_timer);
			++fe;
			++num;
		}
	}

 out:
	rcu_read_unlock();

	return num;
}

/**** hanhui sdk upgrade ****
*****************************/
int br_fdb_write_get_args
(
	unsigned char * buf,
	unsigned char ** args
);
int br_fdb_write_parse_args
(
	struct net_bridge *br,
	unsigned char ** args,
	unsigned int *ifindex,
	unsigned char * addr,
	unsigned int *isadd
);

int br_fdb_write(struct net_bridge *br, char *buf)
{
	unsigned char *args[4];
	unsigned int ifindex = 0;
	unsigned char addr[ETH_ALEN] = {0};
	unsigned int isadd = 0;
	unsigned char bufbak[128] = {0};
	int ret = 0;
	int len = strlen(buf);
	if(len >= 128){
		len = 127;
	} 
	memcpy(bufbak,buf,len);
	if(br_fdb_write_get_args(bufbak, args)){/* get argments from bufbak to args[] */
		ret = -EINVAL;
	}
	else if(br_fdb_write_parse_args(br, args, &ifindex, addr,&isadd)){/*parse the args get : ifindex,addr and isadd */
		ret = -EINVAL;
	}
	else{
		ret = add_del_static_fdb(br,ifindex,addr,isadd);
	}

	return (0 == ret) ? 1 : (0 > ret) ? ret : -EIO;
}

int br_fdb_write_get_args
(
	unsigned char * buf,
	unsigned char ** args
)
{
	unsigned char * argptr = buf;
	int i = 0;
	char * ptr = NULL;
	for(i = 0;i < 4;i++){
		args[i] = NULL;
	}
	i = 0;
	ptr = strsep(&argptr," \t\n");
	while(ptr && (i < 4)){
		args[i] = ptr; 
		ptr = strsep(&argptr," \t\n");
		i++;
	} 
	if((!args[0])||(!args[1])){
		return -EINVAL;
	}
	return 0;
}

int br_fdb_write_parse_args
(
	struct net_bridge *br,
	unsigned char ** args,
	unsigned int *ifindex,
	unsigned char * addr,
	unsigned int *isadd
)
{
	if(!args[0]) return -EINVAL; 
	struct net_device * dev = dev_get_by_name(dev_net(br->dev),args[0]);
	unsigned char * ptr = NULL;
	int i = 0;
	 
	*isadd = 1;
	if(!dev){
		if(!strcmp("null", args[0])){
			*ifindex = 0;
		}
		else {
			return -EINVAL;
		}
	}else{ 
		*ifindex = dev->ifindex; 
		dev_put(dev);
	}
	if(!args[1]){
		return -EINVAL;
	}
	for(i = 0; i < ETH_ALEN; i++){
		ptr = strsep(&args[1],":");
		if(ptr){
			addr[i] = (unsigned char)simple_strtoul(ptr, NULL, 16);
		}
		else{
			break;
		}
	} 
	if((args[2])&&((!strcmp("del", args[2]))||(!strcmp("delete", args[2])))){
		*isadd = 0;
	}
	else if((args[2])&&(strcmp("", args[2]) && strcmp("add", args[2]))){
		return -EINVAL;
	}
	return 0;
}

/*********end****************/


static inline struct net_bridge_fdb_entry *fdb_find(struct hlist_head *head,
						    const unsigned char *addr)
{
	struct hlist_node *h;
	struct net_bridge_fdb_entry *fdb;

	hlist_for_each_entry_rcu(fdb, h, head, hlist) {
		if (!compare_ether_addr(fdb->addr.addr, addr))
			return fdb;
	}
	return NULL;
}

static struct net_bridge_fdb_entry *fdb_create(struct hlist_head *head,
					       struct net_bridge_port *source,
					       const unsigned char *addr,
					       int is_local)
{
	struct net_bridge_fdb_entry *fdb;

	fdb = kmem_cache_alloc(br_fdb_cache, GFP_ATOMIC);
	if (fdb) {
		memcpy(fdb->addr.addr, addr, ETH_ALEN);
		hlist_add_head_rcu(&fdb->hlist, head);

		fdb->dst = source;
		fdb->is_local = is_local;
		fdb->is_static = is_local;
		fdb->ageing_timer = jiffies;
	}
	return fdb;
}

static int fdb_insert(struct net_bridge *br, struct net_bridge_port *source,
		  const unsigned char *addr)
{
	struct hlist_head *head = &br->hash[br_mac_hash(addr)];
	struct net_bridge_fdb_entry *fdb;

	if (!is_valid_ether_addr(addr))
		return -EINVAL;

	fdb = fdb_find(head, addr);
	if (fdb) {
		/* it is okay to have multiple ports with same
		 * address, just use the first one.
		 */
		if (fdb->is_local)
			return 0;

		printk(KERN_WARNING "%s adding interface with same address "
		       "as a received packet\n",
		       source->dev->name);
		fdb_delete(fdb);
	}

	if (!fdb_create(head, source, addr, 1))
		return -ENOMEM;

	return 0;
}

int br_fdb_insert(struct net_bridge *br, struct net_bridge_port *source,
		  const unsigned char *addr)
{
	int ret;

	spin_lock_bh(&br->hash_lock);
	ret = fdb_insert(br, source, addr);
	spin_unlock_bh(&br->hash_lock);
	return ret;
}

/**** hanhui sdk upgrade ****
*****************************/
	
static struct net_bridge_fdb_entry *static_fdb_create(struct hlist_head *head,
						   struct net_bridge_port *source,
						   const unsigned char *addr)
{
	struct net_bridge_fdb_entry *fdb;

	fdb = kmem_cache_alloc(br_fdb_cache, GFP_ATOMIC);
	if (fdb) {
		memcpy(fdb->addr.addr, addr, ETH_ALEN);
		//atomic_set(&fdb->use_count, 1);
		hlist_add_head_rcu(&fdb->hlist, head);

		fdb->dst = source;
		fdb->is_local = 0;
		fdb->is_static = 1;
		fdb->ageing_timer = jiffies;
	}
	return fdb;
}

static int static_fdb_insert(struct net_bridge *br, struct net_bridge_port *source,
		  const unsigned char *addr)
{
	struct hlist_head *head = &br->hash[br_mac_hash(addr)];
	struct net_bridge_fdb_entry *fdb;

	if (!is_valid_ether_addr(addr))
		return -EINVAL;

	fdb = fdb_find(head, addr);
	if (fdb) {
		/* if fdb already exists , do nothing
		 */
		if ((fdb->is_static) && (fdb->dst == source))
			return -EEXIST;

		printk(KERN_WARNING "%s adding interface with same address "
			   "as a received packet\n",
			   source->dev->name);
		fdb_delete(fdb);
	}

	if (!static_fdb_create(head, source, addr))
		return -ENOMEM;

	return 0;
}

int br_static_fdb_insert(struct net_bridge *br, struct net_bridge_port *source,
		  const unsigned char *addr)
{
	int ret;
	if(!source || source->br != br){
		return -EINVAL;
	}
	spin_lock_bh(&br->hash_lock);
	ret = static_fdb_insert(br, source, addr);
	spin_unlock_bh(&br->hash_lock);
	return ret;
}
int add_del_static_fdb
(
	struct net_bridge *br, 
	unsigned int ifindex,
	unsigned char * addr,
	int isadd
)
{
	struct net_device *dev = NULL;
	int ret;
	
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if(isadd || ifindex){
		dev = dev_get_by_index(dev_net(br->dev), ifindex);
		if (dev == NULL)
			return -EINVAL;
	}
	if(ifindex && (!(dev->br_port) || (dev->br_port->br != br))){	
		ret = -EINVAL;
	}
	else if (isadd){
		ret = br_static_fdb_insert(br, dev->br_port,addr);
	}
	else{
		ret = br_static_fdb_delete(br, (ifindex)? dev->br_port : NULL,addr);
	}
	if(dev)
		dev_put(dev);
	return ret;
}


/*
 * this function delete fdb item by mac address unconditionally if exists
 */
 int br_vmac_fdb_delete(struct net_bridge * br,const unsigned char * addr){
 	return br_fdb_delete(br,NULL,addr);
 }
int br_static_fdb_delete(struct net_bridge * br, struct net_bridge_port *source, const unsigned char * addr){
	return br_fdb_delete(br,source,addr);
}

/* delete bridge fdb item by addr ,and  if source is not null match source too */
int br_fdb_delete(struct net_bridge * br, struct net_bridge_port *source,const unsigned char * addr){
	struct hlist_head *head = &br->hash[br_mac_hash(addr)];
	struct net_bridge_fdb_entry *fdb;
	int ret = 0;

	if (!is_valid_ether_addr(addr))
		return -EINVAL;

	spin_lock_bh(&br->hash_lock);
	fdb = fdb_find(head, addr);
	if (fdb) {	
		/* don't consider any port has the same mac with br
		if (fdb->is_local) {
			struct net_bridge_port *op;
			list_for_each_entry(op, &br->port_list, list) {
				if (!compare_ether_addr(op->dev->dev_addr,
							fdb->addr.addr)) {
					fdb->dst = op;
					goto skip_delete;
				}
			}
		}		
		*/
		if(source && (fdb->dst != source)){
			ret = -ENODEV;
			goto skip_delete;
		}
		fdb_delete(fdb);
 	}
	else {
		ret = -ENODEV;
	}
skip_delete:
	spin_unlock_bh(&br->hash_lock);
	return ret;
}

/*******************end*****************/
void br_fdb_update(struct net_bridge *br, struct net_bridge_port *source,
		   const unsigned char *addr)
{
	struct hlist_head *head = &br->hash[br_mac_hash(addr)];
	struct net_bridge_fdb_entry *fdb;

	/* some users want to always flood. */
	if (hold_time(br) == 0)
		return;

	/* ignore packets unless we are using this port */
	if (!(source->state == BR_STATE_LEARNING ||
	      source->state == BR_STATE_FORWARDING))
		return;

	fdb = fdb_find(head, addr);
	if (likely(fdb)) {
		/* attempt to update an entry for a local interface */
		if (unlikely(fdb->is_local)) {
			if (net_ratelimit())
				printk(KERN_WARNING "%s: received packet with "
				       "own address as source address\n",
				       source->dev->name);
		} else {
			/* fastpath: update of existing entry */
			fdb->dst = source;
			fdb->ageing_timer = jiffies;
		}
	} else {
		spin_lock(&br->hash_lock);
		if (!fdb_find(head, addr))
			fdb_create(head, source, addr, 0);
		/* else  we lose race and someone else inserts
		 * it first, don't bother updating
		 */
		spin_unlock(&br->hash_lock);
	}
}


