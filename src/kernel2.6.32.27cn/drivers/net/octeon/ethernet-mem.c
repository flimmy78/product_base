/**********************************************************************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
 *
 * Copyright (c) 2003-2010 Cavium Networks
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * or visit http://www.gnu.org/licenses/.
 *
 * This file may also be available under a different license from Cavium.
 * Contact Cavium Networks for more information
**********************************************************************/
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/netdevice.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-fpa.h>

#include "ethernet-defines.h"
#include "ethernet-util.h"

#if USE_32BIT_SHARED
#include <asm/octeon/cvmx-bootmem.h>
#endif

static struct kmem_cache *cvm_oct_kmem_128;
static struct kmem_cache *cvm_oct_kmem_1024;
extern int use_skbuffs_in_hw_flag;


/**
 * cvm_oct_fill_hw_skbuff - fill the supplied hardware pool with skbuffs
 * @pool:     Pool to allocate an skbuff for
 * @size:     Size of the buffer needed for the pool
 * @elements: Number of buffers to allocate
 *
 * Returns the actual number of buffers allocated.
 */
static int cvm_oct_fill_hw_skbuff(int pool, int size, int elements)
{
	int freed = elements;
/*	int padding = 128 + sizeof(void *) + NET_SKB_PAD - 1;*/
	int padding = 256 + sizeof(void *) + NET_SKB_PAD - 1;
	while (freed) {
		int extra_reserve;
		unsigned char *desired_data;
		struct sk_buff *skb = alloc_skb(size + padding, GFP_ATOMIC);
		if (unlikely(skb == NULL)) {
			pr_warning("Failed to allocate skb for hardware pool %d\n", pool);
			break;
		}
		desired_data = (unsigned char *)((unsigned long)(skb->data + padding) & ~0x7fUl);
		extra_reserve = desired_data - skb->data;
		skb_reserve(skb, extra_reserve);
		*(struct sk_buff **)(skb->data - NET_SKB_PAD - sizeof(void *)) = skb;
		cvmx_fpa_free(skb->data, pool, DONT_WRITEBACK(size / 128));
		freed--;
	}
	return elements - freed;
}

/**
 * cvm_oct_free_hw_skbuff- free hardware pool skbuffs
 * @pool:     Pool to allocate an skbuff for
 * @size:     Size of the buffer needed for the pool
 * @elements: Number of buffers to allocate
 */
static void cvm_oct_free_hw_skbuff(int pool, int size, int elements)
{
	char *memory;

	do {
		memory = cvmx_fpa_alloc(pool);
		if (memory) {
			struct sk_buff *skb = *cvm_oct_packet_to_skb(memory);
			elements--;
			dev_kfree_skb(skb);
		}
	} while (memory);

	if (elements < 0)
		pr_warning("Freeing of pool %u had too many skbuffs (%d)\n", pool, elements);
	else if (elements > 0)
		pr_warning("Freeing of pool %u is missing %d skbuffs\n", pool, elements);
}

/**
 * cvm_oct_fill_hw_memory - fill a hardware pool with memory.
 * @pool:     Pool to populate
 * @size:     Size of each buffer in the pool
 * @elements: Number of buffers to allocate
 *
 * Returns the actual number of buffers allocated.
 */
static int cvm_oct_fill_hw_memory(int pool, int size, int elements)
{
	char *memory;
	int freed = elements;

	if (USE_32BIT_SHARED) {
		extern u64 octeon_reserve32_memory;
		memory = cvmx_bootmem_alloc_range(elements*size, 128, octeon_reserve32_memory,
			    octeon_reserve32_memory + (CONFIG_CAVIUM_RESERVE32<<20) - 1);

		if (memory == NULL)
			panic("Unable to allocate %u bytes for FPA pool %d\n", elements*size, pool);

		printk("Memory range %p - %p reserved for hardware\n",
			memory, memory + elements*size - 1);

		while (freed) {
			cvmx_fpa_free(memory, pool, 0);
			memory += size;
			freed--;
		}
		return elements - freed;
	}

	while (freed) {
		if (size == 128) {
			memory = kmem_cache_alloc(cvm_oct_kmem_128, GFP_KERNEL);
			if (unlikely(memory == NULL)) {
				pr_warning("Unable to allocate %u bytes for FPA pool %d\n",
					elements * size, pool);
				break;
			}
		} else if (size == 1024) {
			memory = kmem_cache_alloc(cvm_oct_kmem_1024, GFP_KERNEL);
			if (unlikely(memory == NULL)) {
				pr_warning("Unable to allocate %u bytes for FPA pool %d\n",
					elements * size, pool);
				break;
			}
		} else {
			BUG();
		}
		cvmx_fpa_free(memory, pool, 0);
		freed--;
	}
	return elements - freed;
}

/**
 * cvm_oct_free_hw_memory - Free memory allocated by cvm_oct_fill_hw_memory
 * @pool:     FPA pool to free
 * @size:     Size of each buffer in the pool
 * @elements: Number of buffers that should be in the pool
 */
static void cvm_oct_free_hw_memory(int pool, int size, int elements)
{
	char *fpa;
	if (USE_32BIT_SHARED) {
		printk("Warning: 32 shared memory is not freeable\n");
		return;
	}
	do {
		fpa = cvmx_fpa_alloc(pool);
		if (fpa) {
			elements--;
			if (size == 128)
				kmem_cache_free(cvm_oct_kmem_128, fpa);
			else if (size == 1024)
				kmem_cache_free(cvm_oct_kmem_1024, fpa);
			else
				BUG();
		}
	} while (fpa);

	if (elements < 0)
		pr_warning("Freeing of pool %u had too many buffers (%d)\n",
			pool, elements);
	else if (elements > 0)
		pr_warning("Warning: Freeing of pool %u is missing %d buffers\n",
			pool, elements);
}

int cvm_oct_mem_fill_fpa(int pool, int size, int elements)
{
	int freed;

	//if (USE_SKBUFFS_IN_HW && pool == CVMX_FPA_PACKET_POOL)
	if (use_skbuffs_in_hw_flag && pool == CVMX_FPA_PACKET_POOL)
		freed = cvm_oct_fill_hw_skbuff(pool, size, elements);
	else
		freed = cvm_oct_fill_hw_memory(pool, size, elements);
	return freed;
}
EXPORT_SYMBOL(cvm_oct_mem_fill_fpa);

void cvm_oct_mem_empty_fpa(int pool, int size, int elements)
{
	//if (USE_SKBUFFS_IN_HW && pool == CVMX_FPA_PACKET_POOL)
	if (use_skbuffs_in_hw_flag && pool == CVMX_FPA_PACKET_POOL)
		cvm_oct_free_hw_skbuff(pool, size, elements);
	else
		cvm_oct_free_hw_memory(pool, size, elements);
}
EXPORT_SYMBOL(cvm_oct_mem_empty_fpa);

void cvm_oct_mem_uninit(void)
{
	if (USE_32BIT_SHARED)
		return;

	if (cvm_oct_kmem_128)
		kmem_cache_destroy(cvm_oct_kmem_128);
	if (cvm_oct_kmem_1024)
		kmem_cache_destroy(cvm_oct_kmem_1024);

	cvm_oct_kmem_128 = NULL;
	cvm_oct_kmem_1024 = NULL;
}
EXPORT_SYMBOL(cvm_oct_mem_uninit);

void cvm_oct_mem_cleanup(void)
{
	if (USE_32BIT_SHARED)
		return;

	if (cvm_oct_kmem_128)
		kmem_cache_shrink(cvm_oct_kmem_128);
	if (cvm_oct_kmem_1024)
		kmem_cache_shrink(cvm_oct_kmem_1024);
}
EXPORT_SYMBOL(cvm_oct_mem_cleanup);

static int __init cvm_oct_mem_init(void)
{
	int r = 0;

	if (USE_32BIT_SHARED)
		goto out;

	cvm_oct_kmem_128 = kmem_cache_create("octeon_ethernet-128", 128, 128, 0, NULL);
	if (!cvm_oct_kmem_128) {
		r = -ENOMEM;
		goto out;
	}

	cvm_oct_kmem_1024 = kmem_cache_create("octeon_ethernet-1024", 1024, 128, 0, NULL);
	if (!cvm_oct_kmem_1024) {
		r = -ENOMEM;
		goto err;
	}
	goto out;

err:
	kmem_cache_destroy(cvm_oct_kmem_128);
	cvm_oct_kmem_128 = NULL;
out:
	return r;
}
arch_initcall(cvm_oct_mem_init);
