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
*  dev_gop.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		Net device characteristic extension provided by Autelan for private usage.
*
* DATE:
*		Mar. 23/2010
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.1 $
*******************************************************************************/
#include "dev_gop.h"

#define NETDEV_HASHBITS	8
/*
static struct hlist_head dev_gindex_head[1<<NETDEV_HASHBITS];
*/
static inline struct hlist_head *dev_gindex_hash
(
	struct net * net,
	int gindex
)
{
	return &net->dev_gindex_head[gindex & ((1<<NETDEV_HASHBITS)-1)];
}

/**
 *	dev_gindex_init - initialize gindex hash chain
 */
 /*
void dev_gindex_hash_init()
{
	int i  = 0;

	for (i = 0; i < ARRAY_SIZE(dev_gindex_head); i++)
		INIT_HLIST_HEAD(&dev_gindex_head[i]);
}
*/
/**
 *	dev_gindex_init - initialize gindex hash chain
 */
void dev_gindex_hash_remove
(
	struct net_device* dev
)
{
	if(!dev) {
		printk(KERN_INFO"remove device from gindex hash chain null pointer error!\n");
		return;
	}

	hlist_del(&dev->gindex_hlist);
	printk(KERN_DEBUG"remove device %s %d from gindex hash!\n", dev->name, dev->gindex);
	return;
}

/**
 *	dev_gindex_add_head - add device to gindex head chain
 */
void dev_gindex_hash_add_head
(
	struct net * net,
	struct net_device* dev
)
{
	if(!dev) {
		printk(KERN_INFO"add device to gindex hash chain null pointer error!\n");
		return;
	}

	hlist_add_head(&dev->gindex_hlist, dev_gindex_hash(net, dev->gindex));
	printk(KERN_DEBUG"add device %s %d to gindex hash!\n", dev->name, dev->gindex);
	return;
}

/**
 *	__dev_get_by_gindex - find a device by its global index
 *	@gindex: global index of device
 *
 *	Search for an interface by global index. Returns %NULL if the device
 *	is not found or a pointer to the device. The device has not
 *	had its reference counter increased so the caller must be careful
 *	about locking. The caller must hold either the RTNL semaphore
 *	or @dev_base_lock.
 */

struct net_device *__dev_get_by_gindex
(
	struct net * net,
	int gindex
)
{
	struct hlist_node *p;

	hlist_for_each(p, dev_gindex_hash(net, gindex)) {
		struct net_device *dev
			= hlist_entry(p, struct net_device, gindex_hlist);
		if (dev->gindex == gindex)
			return dev;
	}
	return NULL;
}


/**
 *	dev_get_by_gindex - find a device by its global index
 *	@gindex: global index of device
 *
 *	Search for an interface by global index. Returns NULL if the device
 *	is not found or a pointer to the device. The device returned has
 *	had a reference added and the pointer is safe until the user calls
 *	dev_put to indicate they have finished with it.
 */

struct net_device *dev_get_by_gindex
(
	struct net * net,
	int gindex
)
{
	struct net_device *dev;

	read_lock(&dev_base_lock);
	dev = __dev_get_by_gindex(net,gindex);
	if (dev)
		dev_hold(dev);
	read_unlock(&dev_base_lock);
	return dev;
}

/**
 *	dev_new_gindex	-	allocate a global ifindex
 *
 *	Returns a suitable unique value for a new device global interface
 *	number.  The caller must hold the rtnl semaphore or the
 *	dev_base_lock to be sure it remains unique.
 */
int dev_new_gindex(struct net *net)
{
	static int gindex;
	for (;;) {
		if (++gindex <= 0)
			gindex = 1;
		if (!__dev_get_by_gindex(net, gindex))
			return gindex;
	}
}

/**
 *	dev_gop_update_index	-	update a global ifindex
 *
 *	Returns a suitable unique value for a new device global interface
 *	number.  The caller must hold the rtnl semaphore or the
 *	dev_base_lock to be sure it remains unique.
 */
int dev_update_gindex
(
	struct net * net,
	char *name,
	int gindex
)
{
	struct net_device *dev = NULL, *odev = NULL;

	if(!name) {
		printk(KERN_INFO"update gindex null name!\n");
		return -EINVAL;
	}
	else if(strlen(name) > IFNAMSIZ) {
		printk(KERN_INFO"update %s gindex name truncate to %d\n", name, IFNAMSIZ);
	}

	/* get net device via name */
	if(!(dev = dev_get_by_name(net,name))) {
		printk(KERN_INFO"update %s gindex %d no such device!\n", name , gindex);
		return -ENODEV;
	}
	else if(gindex == dev->gindex) {
		dev_put(dev);
		printk(KERN_DEBUG"update %s gindex %d no change!\n", name , gindex);
		return 0;
	}
	else {
		dev_put(dev);
	}

	/* check if gindex already in use */
	if((odev = dev_get_by_gindex(net, gindex))) {
		dev_put(odev);
		printk(KERN_INFO"update %s gindex %d in use!\n", name , gindex);
		return -EGINDEX_ALREADY_INUSE;
	}

	/* udpate gindex for the device */
	printk(KERN_DEBUG"device %s gindex change %d -> %d\n", name, dev->gindex, gindex);
	write_lock_bh(&dev_base_lock);
	dev_gindex_hash_remove(dev);
	dev->gindex = gindex;
	dev_gindex_hash_add_head(net,dev);
	write_unlock_bh(&dev_base_lock);

	return 0;
}


/**
 *	dev_get_gindex	-   get global ifindex for the device
 *
 *	Returns global index value for the device.  The caller
 *    must hold the rtnl semaphore or the dev_base_lock
  *   to be sure it remains unique.
 */
int dev_get_gindex
(
	struct net * net,
	char *name,
	int *gindex
)
{
	struct net_device *dev = NULL;

	if(!name ||!gindex) {
		printk(KERN_INFO"get gindex with null name or null ptr!\n");
		return -EINVAL;
	}

	/* get net device via name */
	if(!(dev = dev_get_by_name(net, name))) {
		printk(KERN_INFO"get %s gindex but no such device!\n", name);
		return -ENODEV;
	}
	else {
		dev_put(dev);
	}
	/* fetch gindex for the device */
	printk(KERN_DEBUG"device %s gindex %d\n", name, dev->gindex);
	*gindex = dev->gindex;

	return 0;
}
