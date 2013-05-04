#ifndef __PRESTERA__
#define __PRESTERA__

#include <linux/types.h>
#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */

#ifdef __KERNEL__

#define PRESTERA_MASK_REG                           0x34
#define PRESTERA_MASK_EVENT                     0x0
#define PRESTERA_UNMASK_EVENT                   0xFFFE




#define MAX_PP_DEVICES              (128)

/* Max number pf ports in PP device */
#define MAX_PP_PORTS_NUM            (64)

/* Max number of Xbars in system  */
#define MAX_XBAR_DEVICES             (128)

/* Max fabric ports in xbar device */
#define MAX_XBAR_FPORTS_NUM          (13)


/* version dependencies have been confined to a separate file */
#include "sysdep.h"
#include "marvell.h"

#ifndef PRESTERA_MAJOR
#define PRESTERA_MAJOR 0   /* dynamic major by default */
#endif

#ifndef PRESTERA_NR_DEVS
#define PRESTERA_NR_DEVS 4    /* scull0 through scull3 */
#endif

#ifndef PRESTERA_P_NR_DEVS
#define PRESTERA_P_NR_DEVS 4  /* scullpipe0 through scullpipe3 */
#endif

#define PCI_INTR_A 1
#define PCI_INTR_B 2
#define PCI_INTR_C 3
#define PCI_INTR_D 4

#define PCI0_INTR_A_VEC 26
#define PCI0_INTR_B_VEC 27
#define PCI0_INTR_C_VEC 30
#define PCI0_INTR_D_VEC 31
/*
 * The bare device is a variable-length region of memory.
 * Use a linked list of indirect blocks.
 *
 * "Scull_Dev->data" points to an array of pointers, each
 * pointer refers to a memory area of SCULL_QUANTUM bytes.
 *
 * The array (quantum-set) is SCULL_QSET long.
 */
#ifndef PRESTERA_QUANTUM
#define PRESTERA_QUANTUM 4000
#endif

#ifndef PRESTERA_QSET
#define PRESTERA_QSET    1000
#endif

#ifdef LINUX_20
#  include <asm/segment.h>
#  define access_ok(t,a,sz)           (verify_area((t),(void *) (a),(sz)) ? 0 : 1)
#  define verify_area_20              verify_area
#  define   copy_to_user(t,f,n)         (memcpy_tofs((t), (f), (n)), 0)
#  define copy_from_user(t,f,n)       (memcpy_fromfs((t), (f), (n)), 0)
#  define   __copy_to_user(t,f,n)       copy_to_user((t), (f), (n))
#  define __copy_from_user(t,f,n)     copy_from_user((t), (f), (n))

#  define PUT_USER(val,add)           (put_user((val),(add)), 0)
#  define __PUT_USER(val,add)         PUT_USER((val),(add))

#  define GET_USER(dest,add)          ((dest)=get_user((add)), 0)
#  define __GET_USER(dest,add)        GET_USER((dest),(add))
#else
#  include <asm/uaccess.h>
#  include <asm/io.h>
#  define verify_aScull_Dev *scull_devices; /* allocated in scull_init_module */
#  define   PUT_USER   put_user
#  define __PUT_USER __put_user
#  define   GET_USER   get_user
#  define __GET_USER __get_user
#endif

#define GT_SYNC __asm__("   eieio")

#define MAX_LEN 64

struct Prestera_Dev
{
    struct Prestera_Dev * next;/* pointer to next dev */
    unsigned long magic;
    unsigned long flags;
    unsigned int dev_num;
    char *wr_p,*rd_p; /* where to read where to write */
    unsigned long irq;
    unsigned long pci_address;
    struct semaphore sem; /* Mutual exclusion semaphore */
    loff_t size; /* prestera mem size */
    unsigned char buffer[MAX_LEN];
};

struct Mem_Region
{
    unsigned long base;
    unsigned long size;
};

struct PP_Dev
{
    struct Mem_Region config; /* Configuration space */
    struct Mem_Region ppregs; /* PP registers space */
};

struct InterruptData
{
    unsigned long           vector; /* the interrupt vector we bind too */
    struct semaphore        sem;    /* the semaphore on which the user waits */
    struct tq_struct        *tq;    /* the tasklet - needit for cleanup */
};
#endif /* __KERNEL__ */

#undef INTERRUPT_1 /* this is for second R&D Phase */

#endif /* __PRESTERA__ */
