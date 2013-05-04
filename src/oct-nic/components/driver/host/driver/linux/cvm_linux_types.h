/*
 *
 * OCTEON SDK
 *
 * Copyright (c) 2011 Cavium Networks. All rights reserved.
 *
 * This file, which is part of the OCTEON SDK which also includes the
 * OCTEON SDK Package from Cavium Networks, contains proprietary and
 * confidential information of Cavium Networks and in some cases its
 * suppliers. 
 *
 * Any licensed reproduction, distribution, modification, or other use of
 * this file or the confidential information or patented inventions
 * embodied in this file is subject to your license agreement with Cavium
 * Networks. Unless you and Cavium Networks have agreed otherwise in
 * writing, the applicable license terms "OCTEON SDK License Type 5" can be found 
 * under the directory: $OCTEON_ROOT/components/driver/licenses/
 *
 * All other use and disclosure is prohibited.
 *
 * Contact Cavium Networks at info@caviumnetworks.com for more information.
 *
 */

#ifndef  __CVM_LINUX_TYPES_H__
#define  __CVM_LINUX_TYPES_H__



#define   __CVM_FILE__                   __FILE__
#define   __CVM_FUNCTION__               __FUNCTION__
#define   __CVM_LINE__                   __LINE__
#define   __CAVIUM_MEM_ATOMIC            GFP_ATOMIC
#define   __CAVIUM_MEM_GENERAL           GFP_KERNEL
#define   CAVIUM_PAGE_SIZE               PAGE_SIZE
#define   CAVIUM_MAX_CONTIG_PAGES        512
#define   OCTEON_MAX_ALLOC_RETRIES       1


#define   cavium_flush_write()           wmb()
#define   cavium_get_cpu_count()         num_online_cpus()

#define   cavium_get_cpu_counter()       get_cycles()
#define   cavium_jiffies                 jiffies
#define   CAVIUM_TICKS_PER_SEC           HZ
#define   cavium_mdelay(tmsecs)          mdelay(tmsecs)
#define   cavium_udelay(tusecs)          udelay(tusecs)
#define   cavium_timeout(tjiffies)       schedule_timeout(tjiffies)


#define   cavium_malloc_dma(size, flags) kmalloc((size),(flags))
#define   cavium_free_dma(pbuf)	         kfree((pbuf))
#define   cavium_alloc_virt(size)        vmalloc((size))
#define   cavium_free_virt(ptr)          vfree((ptr))
#define   cavium_get_order(size)         get_order((size))


#define   cavium_memcpy(dest, src, size) memcpy((dest), (src), (size))
#define   cavium_memset(buf, val, size)  memset((buf), (val), (size))


#define   cavium_sleep(wc)               interruptible_sleep_on(wc)
#define   cavium_wakeup(wc)              wake_up_interruptible((wc))
#define   cavium_schedule()              schedule()


#define   cavium_atomic_set(ptr, val)    atomic_set((ptr), (val))
#define   cavium_atomic_read(ptr)        atomic_read((ptr))
#define   cavium_atomic_inc(ptr)         atomic_inc((ptr))
#define   cavium_atomic_add(val, ptr)    atomic_add((val), (ptr))
#define   cavium_atomic_dec(ptr)         atomic_dec((ptr))
#define   cavium_atomic_sub(val, ptr)    atomic_sub((val), (ptr))

#define   cavium_sema_init(sema, count)  sema_init((sema),(count))
#define   cavium_sema_down(sema)         down((sema))
#define   cavium_sema_up(sema)           up((sema))


#define   OCTEON_READ32(addr)            readl(addr)
#define   OCTEON_WRITE32(addr, val)      writel((val),(addr))
#define   OCTEON_READ16(addr)            readw(addr)
#define   OCTEON_WRITE16(addr, val)      writew((val),(addr))
#define   OCTEON_READ8(addr)             readb(addr)
#define   OCTEON_WRITE8(addr, val)       writeb((val),(addr))
#ifdef    readq
#define   OCTEON_READ64(addr)            readq(addr)
#endif
#ifdef    writeq
#define   OCTEON_WRITE64(addr, val)      writeq((val),(addr))
#endif


#define   CAVIUM_PCI_DMA_FROMDEVICE      PCI_DMA_FROMDEVICE
#define   CAVIUM_PCI_DMA_TODEVICE        PCI_DMA_TODEVICE

#define   free_recv_buffer(skb)          dev_kfree_skb_any((skb))
#define   recv_buf_put(skb, len)         skb_put((skb), (len))
#define   recv_buf_reserve(skb, len)     skb_reserve((ptr), len)
#define   recv_buffer_push(skb, len)     skb_push((skb), (len))
#define   recv_buffer_pull(skb, len)     skb_pull((skb), (len))

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
#define   cvm_ip_hdr(skb)                (ip_hdr((skb)))
#define   cvm_ip6_hdr(skb)               (ipv6_hdr((skb)))
#else
#define   cvm_ip_hdr(skb)                ((skb)->nh.iph)
#define   cvm_ip6_hdr(skb)               ((skb)->nh.ipv6h)
#endif


#define   cavium_spin_lock_init(lock)               spin_lock_init((lock))
#define   cavium_spin_lock(lock)                    spin_lock((lock))
#define   cavium_spin_unlock(lock)                  spin_unlock((lock))
#define   cavium_spin_lock_softirqsave(lock)        spin_lock_bh(lock)
#define   cavium_spin_unlock_softirqrestore(lock)   spin_unlock_bh(lock)
#define   cavium_spin_lock_irqsave(lock, flags)     spin_lock_irqsave(lock, flags)
#define   cavium_spin_unlock_irqrestore(lock,flags) spin_unlock_irqrestore(lock, flags)


#define   cavium_init_wait_channel(wc_ptr)          init_waitqueue_head(wc_ptr)
#define   cavium_init_wait_entry(we_ptr, task)      init_waitqueue_entry(we_ptr, task)
#define   cavium_add_to_waitq(wq_ptr, we_ptr)       add_wait_queue(wq_ptr, we_ptr)
#define   cavium_remove_from_waitq(wq_ptr, we_ptr)  remove_wait_queue(wq_ptr, we_ptr)


#define   cavium_alloc_pages(flags,order)           __get_free_pages((flags), (order))
#define   cavium_free_pages(ppages,order)           free_pages ((unsigned long)(ppages),(order))


#define   cavium_copy_in(dest, src, size)           copy_from_user((dest), (src), (size))
#define   cavium_copy_out(dest, src, size)          copy_to_user((dest), (src), (size))
#define   cavium_access_ok(flag, addr, size)        access_ok((flag), (addr), (size))

#define   cavium_get_random_bytes(ptr, len)         get_random_bytes((ptr), (len))


#define   cavium_check_timeout(kerntime, chk_time)  time_after((kerntime), (unsigned long)(chk_time))


#define   cavium_tasklet_init(ptask, pfn, parg)     tasklet_init((ptask), (pfn), (parg))
#define   cavium_tasklet_schedule(ptask)            tasklet_schedule((ptask))
#define   cavium_tasklet_kill(ptask)                tasklet_kill((ptask))



#define OCTEON_READ_PCI_CONFIG(dev, offset, pvalue)      \
          pci_read_config_dword((dev)->pci_dev, (offset),(pvalue))

#define OCTEON_WRITE_PCI_CONFIG(dev, offset, value)      \
          pci_write_config_dword((dev)->pci_dev, (offset),(value))









#define    cvm_intr_return_t             irqreturn_t
#define    CVM_INTR_HANDLED              IRQ_HANDLED
#define    CVM_INTR_NONE                 IRQ_NONE

#define    CVM_MOD_INC_USE_COUNT         try_module_get(THIS_MODULE)
#define    CVM_MOD_DEC_USE_COUNT         module_put(THIS_MODULE)
#define    CVM_MOD_IN_USE                module_refcount(THIS_MODULE)



#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
#define    CVM_SHARED_INTR         SA_SHIRQ
#else
#define    CVM_SHARED_INTR         IRQF_SHARED
#endif



typedef  pid_t               cavium_pid_t;
typedef  struct task_struct  cavium_ostask_t;
typedef  spinlock_t          cavium_spinlock_t;
typedef  struct semaphore    cavium_semaphore_t;
typedef  struct tasklet      cavium_tasklet_t;
typedef  atomic_t            cavium_atomic_t;
typedef  wait_queue_head_t   cavium_wait_channel;
typedef  wait_queue_t        cavium_wait_entry;
typedef  struct sk_buff      cavium_netbuf_t;
typedef  struct pci_dev      cavium_pci_device_t;
typedef  struct page         cavium_page_t;





#endif



/* $Id: cvm_linux_types.h 48846 2010-04-30 19:53:17Z panicker $ */
