/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004-2007 Cavium Networks
 * Copyright (C) 2008 Wind River Systems
 */
#include <linux/init.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/serial.h>
#include <linux/smp.h>
#include <linux/types.h>
#include <linux/string.h>	/* for memset */
#include <linux/tty.h>
#include <linux/time.h>
#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <linux/of_fdt.h>
#include <linux/libfdt.h>
#ifdef CONFIG_BLK_DEV_INITRD
#include <linux/initrd.h>
#endif
#include <linux/autelan_product.h>

#include <asm/processor.h>
#include <asm/reboot.h>
#include <asm/smp-ops.h>
#include <asm/system.h>
#include <asm/irq_cpu.h>
#include <asm/mipsregs.h>
#include <asm/bootinfo.h>
#include <asm/sections.h>
#include <asm/time.h>
#include <asm/traps.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/octeon-boot-info.h>

#include <asm/octeon/cvmx-clock.h>
#include <asm/octeon/cvmx-error.h>
#include <asm/octeon/cvmx-debug.h>
#include <asm/octeon/cvmx-pcie.h>
#include <asm/octeon/cvmx-sso-defs.h>

#ifdef CONFIG_CAVIUM_DECODE_RSL
extern int __cvmx_interrupt_ecc_report_single_bit_errors;
#endif

extern struct plat_smp_ops octeon_smp_ops;

extern void pci_console_init(const char *arg);

static unsigned long long MAX_MEMORY = 4096ull << 20;

struct octeon_boot_descriptor *octeon_boot_desc_ptr;

struct cvmx_bootinfo *octeon_bootinfo;
EXPORT_SYMBOL(octeon_bootinfo);
product_info_t autelan_product_info;
EXPORT_SYMBOL(autelan_product_info);

#if CONFIG_CAVIUM_RESERVE32
uint64_t octeon_reserve32_memory;
EXPORT_SYMBOL(octeon_reserve32_memory);
#endif

static int octeon_uart;

extern asmlinkage void handle_int(void);
extern asmlinkage void plat_irq_dispatch(void);

/* Any mdio busses that are configured for this kernel. */
struct mii_bus *octeon_mdiobuses[4];
EXPORT_SYMBOL(octeon_mdiobuses);

/*
 * If set to a non-zero value, the bootloader entry point for
 * HOTPLUG_CPU and other tricks.
 */
u64 octeon_bootloader_entry_addr;
EXPORT_SYMBOL(octeon_bootloader_entry_addr);


/* If an initrd named block is specified, its name goes here. */
static char __initdata rd_name[64];

int octeon_serial_uartdisable_mask;

#define SDK_VERSION "2.2"

/**
 * Return non zero if we are currently running in the Octeon simulator
 *
 * Returns
 */
int octeon_is_simulation(void)
{
	return octeon_bootinfo->board_type == CVMX_BOARD_TYPE_SIM;
}
EXPORT_SYMBOL(octeon_is_simulation);

/**
 * Return true if Octeon is in PCI Host mode. This means
 * Linux can control the PCI bus.
 *
 * Returns Non zero if Octeon in host mode.
 */
int octeon_is_pci_host(void)
{
#ifdef CONFIG_PCI
	return octeon_bootinfo->config_flags & CVMX_BOOTINFO_CFG_FLAG_PCI_HOST;
#else
	return 0;
#endif
}

/**
 * Get the clock rate of Octeon
 *
 * Returns Clock rate in HZ
 */
uint64_t octeon_get_clock_rate(void)
{
	struct cvmx_sysinfo *sysinfo = cvmx_sysinfo_get();

	return sysinfo->cpu_clock_hz;
}
EXPORT_SYMBOL(octeon_get_clock_rate);

/*
 * Get the IO clock rate of Octeon2
 *
 * Returns IO Clock rate in HZ
 */
uint64_t octeon_get_io_clock_rate(void)
{
	return cvmx_clock_get_rate(CVMX_CLOCK_SCLK);
}
EXPORT_SYMBOL(octeon_get_io_clock_rate);

/**
 * Write to the LCD display connected to the bootbus. This display
 * exists on most Cavium evaluation boards. If it doesn't exist, then
 * this function doesn't do anything.
 *
 * @s:      String to write
 */
void octeon_write_lcd(const char *s)
{
	if (octeon_bootinfo->led_display_base_addr) {
		void __iomem *lcd_address =
			ioremap_nocache(octeon_bootinfo->led_display_base_addr,
					8);
		int i;
		for (i = 0; i < 8; i++, s++) {
			if (*s)
				iowrite8(*s, lcd_address + i);
			else
				iowrite8(' ', lcd_address + i);
		}
		iounmap(lcd_address);
	}
}

/**
 * Return the console uart passed by the bootloader
 *
 * Returns uart   (0 or 1)
 */
static int octeon_get_boot_uart(void)
{
	int uart;
#ifdef CONFIG_CAVIUM_OCTEON_2ND_KERNEL
	uart = 1;
#else
	uart = (octeon_boot_desc_ptr->flags & OCTEON_BL_FLAG_CONSOLE_UART1) ?
		1 : 0;
#endif
	return uart;
}

/**
 * Get the coremask Linux was booted on.
 *
 * Returns Core mask
 */
int octeon_get_boot_coremask(void)
{
	return octeon_boot_desc_ptr->core_mask;
}

/**
 * Check the hardware BIST results for a CPU
 */
void octeon_check_cpu_bist(void)
{
	const int coreid = cvmx_get_core_num();
	unsigned long long mask;
	unsigned long long bist_val;

	/* Check BIST results for COP0 registers */
	mask = 0x1f00000000ull;
	bist_val = read_octeon_c0_icacheerr();
	if (bist_val & mask)
		pr_err("Core%d BIST Failure: CacheErr(icache) = 0x%llx\n",
		       coreid, bist_val);

	bist_val = read_octeon_c0_dcacheerr();
	if (bist_val & 1)
		pr_err("Core%d L1 Dcache parity error: "
		       "CacheErr(dcache) = 0x%llx\n",
		       coreid, bist_val);

	mask = 0xfc00000000000000ull;
	bist_val = read_c0_cvmmemctl();
	if (bist_val & mask)
		pr_err("Core%d BIST Failure: COP0_CVM_MEM_CTL = 0x%llx\n",
		       coreid, bist_val);

	write_octeon_c0_dcacheerr(0);
}

/**
 * Reboot Octeon
 *
 * @command: Command to pass to the bootloader. Currently ignored.
 */
static void octeon_restart(char *command)
{
	/* Disable all watchdogs before soft reset. They don't get cleared */
#ifdef CONFIG_SMP
	int cpu;
	for_each_online_cpu(cpu)
		cvmx_write_csr(CVMX_CIU_WDOGX(cpu_logical_map(cpu)), 0);
#else
	cvmx_write_csr(CVMX_CIU_WDOGX(cvmx_get_core_num()), 0);
#endif

	mb();
	while (1)
		cvmx_write_csr(CVMX_CIU_SOFT_RST, 1);
}


/**
 * Permanently stop a core.
 *
 * @arg: Ignored.
 */
static void octeon_kill_core(void *arg)
{
	mb();
	if (octeon_is_simulation()) {
		/* The simulator needs the watchdog to stop for dead cores */
		cvmx_write_csr(CVMX_CIU_WDOGX(cvmx_get_core_num()), 0);
		/* A break instruction causes the simulator stop a core */
		asm volatile ("sync\nbreak");
	}
}


/**
 * Halt the system
 */
static void octeon_halt(void)
{
	smp_call_function(octeon_kill_core, NULL, 0);

	switch (octeon_bootinfo->board_type) {
	case CVMX_BOARD_TYPE_NAO38:
		/* Driving a 1 to GPIO 12 shuts off this board */
		cvmx_write_csr(CVMX_GPIO_BIT_CFGX(12), 1);
		cvmx_write_csr(CVMX_GPIO_TX_SET, 0x1000);
		break;
	default:
		octeon_write_lcd("PowerOff");
		break;
	}

	octeon_kill_core(NULL);
}

/**
 * Handle all the error condition interrupts that might occur.
 *
 */
#ifdef CONFIG_CAVIUM_DECODE_RSL
static irqreturn_t octeon_rml_interrupt(int cpl, void *dev_id)
{
	if (cvmx_error_poll())
		return IRQ_HANDLED;
	else
		return IRQ_NONE;
}
#endif

/**
 * Return a string representing the system type
 *
 * Returns
 */
const char *octeon_board_type_string(void)
{
	static char name[80];
	sprintf(name, "%s (%s)",
		cvmx_board_type_to_string(octeon_bootinfo->board_type),
		octeon_model_get_string(read_c0_prid()));
	return name;
}

const char *get_system_type(void)
	__attribute__ ((alias("octeon_board_type_string")));

void octeon_user_io_init(void)
{
	union octeon_cvmemctl cvmmemctl;
	union cvmx_iob_fau_timeout fau_timeout;

	/* Get the current settings for CP0_CVMMEMCTL_REG */
	cvmmemctl.u64 = read_c0_cvmmemctl();
	/* R/W If set, marked write-buffer entries time out the same
	 * as as other entries; if clear, marked write-buffer entries
	 * use the maximum timeout. */
	cvmmemctl.s.dismarkwblongto = 1;
	/* R/W If set, a merged store does not clear the write-buffer
	 * entry timeout state. */
	cvmmemctl.s.dismrgclrwbto = 0;
	/* R/W Two bits that are the MSBs of the resultant CVMSEG LM
	 * word location for an IOBDMA. The other 8 bits come from the
	 * SCRADDR field of the IOBDMA. */
	cvmmemctl.s.iobdmascrmsb = 0;
	/* R/W If set, SYNCWS and SYNCS only order marked stores; if
	 * clear, SYNCWS and SYNCS only order unmarked
	 * stores. SYNCWSMARKED has no effect when DISSYNCWS is
	 * set. */
	cvmmemctl.s.syncwsmarked = 0;
	/* R/W If set, SYNCWS acts as SYNCW and SYNCS acts as SYNC. */
	cvmmemctl.s.dissyncws = 0;
	/* R/W If set, no stall happens on write buffer full. */
	if (OCTEON_IS_MODEL(OCTEON_CN38XX_PASS2))
		cvmmemctl.s.diswbfst = 1;
	else
		cvmmemctl.s.diswbfst = 0;
	/* R/W If set (and SX set), supervisor-level loads/stores can
	 * use XKPHYS addresses with <48>==0 */
	cvmmemctl.s.xkmemenas = 0;

	/* R/W If set (and UX set), user-level loads/stores can use
	 * XKPHYS addresses with VA<48>==0 */
#ifdef CONFIG_CAVIUM_OCTEON_USER_MEM
	cvmmemctl.s.xkmemenau = 1;
#else
	cvmmemctl.s.xkmemenau = 0;
#endif

	/* R/W If set (and SX set), supervisor-level loads/stores can
	 * use XKPHYS addresses with VA<48>==1 */
	cvmmemctl.s.xkioenas = 0;

	/* R/W If set (and UX set), user-level loads/stores can use
	 * XKPHYS addresses with VA<48>==1 */
#ifdef CONFIG_CAVIUM_OCTEON_USER_IO
	cvmmemctl.s.xkioenau = 1;
#else
	cvmmemctl.s.xkioenau = 0;
#endif

	/* R/W If set, all stores act as SYNCW (NOMERGE must be set
	 * when this is set) RW, reset to 0. */
	cvmmemctl.s.allsyncw = 0;

	/* R/W If set, no stores merge, and all stores reach the
	 * coherent bus in order. */
	cvmmemctl.s.nomerge = 0;
	/* R/W Selects the bit in the counter used for DID time-outs 0
	 * = 231, 1 = 230, 2 = 229, 3 = 214. Actual time-out is
	 * between 1x and 2x this interval. For example, with
	 * DIDTTO=3, expiration interval is between 16K and 32K. */
	cvmmemctl.s.didtto = 0;
	/* R/W If set, the (mem) CSR clock never turns off. */
	cvmmemctl.s.csrckalwys = 0;
	/* R/W If set, mclk never turns off. */
	cvmmemctl.s.mclkalwys = 0;
	/* R/W Selects the bit in the counter used for write buffer
	 * flush time-outs (WBFLT+11) is the bit position in an
	 * internal counter used to determine expiration. The write
	 * buffer expires between 1x and 2x this interval. For
	 * example, with WBFLT = 0, a write buffer expires between 2K
	 * and 4K cycles after the write buffer entry is allocated. */
	cvmmemctl.s.wbfltime = 0;
	/* R/W If set, do not put Istream in the L2 cache. */
	cvmmemctl.s.istrnol2 = 0;
	/*
	 * R/W The write buffer threshold. As per erratum Core-14752
	 * for CN63XX, a sc/scd might fail if the write buffer is
	 * full.  Lowering WBTHRESH greatly lowers the chances of the
	 * write buffer ever being full and triggering the erratum.
	 */
	if (OCTEON_IS_MODEL(OCTEON_CN63XX_PASS1_X))
		cvmmemctl.s.wbthresh = 4;
	else
		cvmmemctl.s.wbthresh = 10;
	/* R/W If set, CVMSEG is available for loads/stores in
	 * kernel/debug mode. */
#if CONFIG_CAVIUM_OCTEON_CVMSEG_SIZE > 0
	cvmmemctl.s.cvmsegenak = 1;
#else
	cvmmemctl.s.cvmsegenak = 0;
#endif
	/* R/W If set, CVMSEG is available for loads/stores in
	 * supervisor mode. */
	cvmmemctl.s.cvmsegenas = 0;
	/* R/W If set, CVMSEG is available for loads/stores in user
	 * mode. */
	cvmmemctl.s.cvmsegenau = 0;

	write_c0_cvmmemctl(cvmmemctl.u64);

	/* Setup of CVMSEG is done in kernel-entry-init.h */
	if (smp_processor_id() == 0)
		pr_notice("CVMSEG size: %d cache lines (%d bytes)\n",
			  CONFIG_CAVIUM_OCTEON_CVMSEG_SIZE,
			  CONFIG_CAVIUM_OCTEON_CVMSEG_SIZE * 128);

	/* Set a default for the hardware timeouts */
	fau_timeout.u64 = 0;
	fau_timeout.s.tout_val = 0xfff;
	/* Disable tagwait FAU timeout */
	fau_timeout.s.tout_enb = 0;
	cvmx_write_csr(CVMX_IOB_FAU_TIMEOUT, fau_timeout.u64);

	if (OCTEON_IS_MODEL(OCTEON_CN68XX)) {
		union cvmx_sso_nw_tim nm_tim;
		nm_tim.u64 = 0;
		/* 4096 cycles */
		nm_tim.s.nw_tim = 3;
		cvmx_write_csr(CVMX_SSO_NW_TIM, nm_tim.u64);
	} else {
		union cvmx_pow_nw_tim nm_tim;
		nm_tim.u64 = 0;
		/* 4096 cycles */
		nm_tim.s.nw_tim = 3;
		cvmx_write_csr(CVMX_POW_NW_TIM, nm_tim.u64);
	}
	write_octeon_c0_icacheerr(0);
	write_c0_derraddr1(0);
}

#if defined(CONFIG_CAVIUM_OCTEON_USER_MEM_PER_PROCESS) || \
	defined(CONFIG_CAVIUM_OCTEON_USER_IO_PER_PROCESS)

void cavium_prepare_arch_switch(struct task_struct *next)
{
	struct task_struct *group_leader = next->group_leader;
	union octeon_cvmemctl cvmmemctl;
	cvmmemctl.u64 = read_c0_cvmmemctl();

#if defined(CONFIG_CAVIUM_OCTEON_USER_MEM_PER_PROCESS)
	cvmmemctl.s.xkmemenau = test_tsk_thread_flag(group_leader, TIF_XKPHYS_MEM_EN) ? 1 : 0;
#endif

#if defined(CONFIG_CAVIUM_OCTEON_USER_IO_PER_PROCESS)
	cvmmemctl.s.xkioenau = test_tsk_thread_flag(group_leader, TIF_XKPHYS_IO_EN) ? 1 : 0;
#endif
	write_c0_cvmmemctl(cvmmemctl.u64);
}
#else
static void cavium_prepare_arch_switch(struct task_struct *next)
{
}
#endif

static struct task_struct *xkphys_get_task(pid_t pid)
{
	struct task_struct *task, *group_leader;

	rcu_read_lock();
	task = find_task_by_vpid(pid);
	if (!task) {
		read_unlock(&tasklist_lock);
		return NULL;
	}
	group_leader = task->group_leader;
	get_task_struct(group_leader);

	rcu_read_unlock();
	return group_leader;
}

int xkphys_usermem_read(long pid)
{
	struct task_struct *task;
	int io, mem;

	task = xkphys_get_task(pid);
	if (!task)
		return -ESRCH;
#if defined(CONFIG_CAVIUM_OCTEON_USER_IO)
	io = 1;
#elif defined(CONFIG_CAVIUM_OCTEON_USER_IO_PER_PROCESS)
	io = test_tsk_thread_flag(task, TIF_XKPHYS_IO_EN);
#else
	io = 0;
#endif

#if defined(CONFIG_CAVIUM_OCTEON_USER_MEM)
	mem = 1;
#elif defined(CONFIG_CAVIUM_OCTEON_USER_MEM_PER_PROCESS)
	mem = test_tsk_thread_flag(task, TIF_XKPHYS_MEM_EN);
#else
	mem = 0;
#endif
	put_task_struct(task);
	return (io ? 2 : 0) | (mem ? 1 : 0);
}

/* the caller must hold RCU read lock */
int is_task_and_current_same(struct task_struct *t)
{
	const struct cred *cred = current_cred(), *tcred;

	tcred = __task_cred(t);
	if ((cred->euid ^ tcred->suid) &&
	    (cred->euid ^ tcred->uid) &&
	    (cred->uid  ^ tcred->suid) &&
	    (cred->uid  ^ tcred->uid)) {
		return 0;
	}
	return 1;
}

int xkphys_usermem_write(long pid, int value)
{
	struct task_struct *task, *group_leader;
	int permission_ok = 0;

#if defined(CONFIG_CAVIUM_OCTEON_USER_IO)
	if ((value & 2) == 0)
		return -EINVAL;
#elif ! defined(CONFIG_CAVIUM_OCTEON_USER_IO_PER_PROCESS)
	if (value & 2)
		return -EINVAL;
#endif
#if defined(CONFIG_CAVIUM_OCTEON_USER_MEM)
	if ((value & 1) == 0)
		return -EINVAL;
#elif !defined(CONFIG_CAVIUM_OCTEON_USER_MEM_PER_PROCESS)
	if (value & 1)
		return -EINVAL;
#endif

	task = xkphys_get_task(pid);
	group_leader = task->group_leader;

	if (!task)
		return -ESRCH;

	rcu_read_lock();
	/* Allow XKPHYS disable of other tasks from the current user*/
	if (value == 0 && is_task_and_current_same(task))
		permission_ok = 1;
	rcu_read_unlock();

	if (capable(CAP_SYS_RAWIO))
		permission_ok = 1;

	if (!permission_ok) {
		put_task_struct(task);
		return -EPERM;
	}

	if (value & 1)
		set_tsk_thread_flag(group_leader, TIF_XKPHYS_MEM_EN);
	else
		clear_tsk_thread_flag(group_leader, TIF_XKPHYS_MEM_EN);

	if (value & 2)
		set_tsk_thread_flag(group_leader, TIF_XKPHYS_IO_EN);
	else
		clear_tsk_thread_flag(group_leader, TIF_XKPHYS_IO_EN);

	preempt_disable();

	/* If we are adjusting ourselves, make the change effective 
           immediatly.  */
	if (group_leader == current->group_leader)
		cavium_prepare_arch_switch(current);

	preempt_enable();

	put_task_struct(task);
	return 0;
}

/*
 * Add Octeon specific bus error handler, as write buffer parity errors
 * trigger bus errors.  These are fatal since the copy in the write buffer
 * is the only copy of the data.
 */
static int octeon2_be_handler(struct pt_regs *regs, int is_fixup)
{
	u64 dcache_err;
	u64 wbfperr_mask = 1ULL << 1;
	dcache_err = read_octeon_c0_dcacheerr();
	if (dcache_err & wbfperr_mask) {
		unsigned int coreid = cvmx_get_core_num();

		pr_err("Core%u: Write buffer parity error:\n", coreid);
		pr_err("CacheErr (Dcache) == %llx\n", dcache_err);

		write_octeon_c0_dcacheerr(wbfperr_mask);
		return MIPS_BE_FATAL;
	}
	if (is_fixup)
		return MIPS_BE_FIXUP;
	else
		return MIPS_BE_FATAL;
}

/**
 * Early entry point for arch setup
 */
void __init prom_init(void)
{
	struct cvmx_sysinfo *sysinfo;
	struct linux_app_boot_info *labi;
	int i;
	int argc;
#if CONFIG_CAVIUM_RESERVE32
	int64_t addr = -1;
#endif
	/*
	 * The bootloader passes a pointer to the boot descriptor in
	 * $a3, this is available as fw_arg3.
	 */
	octeon_boot_desc_ptr = (struct octeon_boot_descriptor *)fw_arg3;

	octeon_feature_init();

	octeon_bootinfo = phys_to_virt(octeon_boot_desc_ptr->cvmx_desc_vaddr);
	cvmx_bootmem_init((u64)phys_to_virt(octeon_bootinfo->phy_mem_desc_addr));


	sysinfo = cvmx_sysinfo_get();
	memset(sysinfo, 0, sizeof(*sysinfo));
	sysinfo->system_dram_size = octeon_bootinfo->dram_size << 20;
	sysinfo->phy_mem_desc_addr = (u64)phys_to_virt(octeon_bootinfo->phy_mem_desc_addr);
	sysinfo->core_mask = octeon_bootinfo->core_mask;
	sysinfo->exception_base_addr = octeon_bootinfo->exception_base_addr;
	sysinfo->cpu_clock_hz = octeon_bootinfo->eclock_hz;
	sysinfo->dram_data_rate_hz = octeon_bootinfo->dclock_hz * 2;
	sysinfo->board_type = octeon_bootinfo->board_type;
	sysinfo->board_rev_major = octeon_bootinfo->board_rev_major;
	sysinfo->board_rev_minor = octeon_bootinfo->board_rev_minor;
	memcpy(sysinfo->mac_addr_base, octeon_bootinfo->mac_addr_base,
	       sizeof(sysinfo->mac_addr_base));
	sysinfo->mac_addr_count = octeon_bootinfo->mac_addr_count;
	memcpy(sysinfo->board_serial_number,
	       octeon_bootinfo->board_serial_number,
	       sizeof(sysinfo->board_serial_number));
	sysinfo->compact_flash_common_base_addr =
		octeon_bootinfo->compact_flash_common_base_addr;
	sysinfo->compact_flash_attribute_base_addr =
		octeon_bootinfo->compact_flash_attribute_base_addr;
	sysinfo->led_display_base_addr = octeon_bootinfo->led_display_base_addr;
	sysinfo->dfa_ref_clock_hz = octeon_bootinfo->dfa_ref_clock_hz;
	sysinfo->bootloader_config_flags = octeon_bootinfo->config_flags;

#ifdef CONFIG_CAVIUM_DECODE_RSL
	cvmx_error_initialize(CVMX_ERROR_FLAGS_ECC_SINGLE_BIT | CVMX_ERROR_FLAGS_CORRECTABLE);
#endif

	/*
	 * Only enable the LED controller if we're running on a CN38XX, CN58XX,
	 * or CN56XX. The CN30XX and CN31XX don't have an LED controller.
	 */
	if (!octeon_is_simulation() &&
	    octeon_has_feature(OCTEON_FEATURE_LED_CONTROLLER)) {
		cvmx_write_csr(CVMX_LED_EN, 0);
		cvmx_write_csr(CVMX_LED_PRT, 0);
		cvmx_write_csr(CVMX_LED_DBG, 0);
		cvmx_write_csr(CVMX_LED_PRT_FMT, 0);
		cvmx_write_csr(CVMX_LED_UDD_CNTX(0), 32);
		cvmx_write_csr(CVMX_LED_UDD_CNTX(1), 32);
		cvmx_write_csr(CVMX_LED_UDD_DATX(0), 0);
		cvmx_write_csr(CVMX_LED_UDD_DATX(1), 0);
		cvmx_write_csr(CVMX_LED_EN, 1);
	}
#if CONFIG_CAVIUM_RESERVE32
	/*
	 * We need to temporarily allocate all memory in the reserve32
	 * region. This makes sure the kernel doesn't allocate this
	 * memory when it is getting memory from the
	 * bootloader. Later, after the memory allocations are
	 * complete, the reserve32 will be freed.
	 *
	 * Allocate memory for RESERVED32 aligned on 2MB boundary. This
	 * is in case we later use hugetlb entries with it.
	 */
	addr = cvmx_bootmem_phy_named_block_alloc(CONFIG_CAVIUM_RESERVE32 << 20,
						0, 0, 2 << 20,
						"CAVIUM_RESERVE32", 0);
	if (addr < 0)
		pr_err("Failed to allocate CAVIUM_RESERVE32 memory area\n");
	else
		octeon_reserve32_memory = addr;
#endif

	octeon_check_cpu_bist();

	octeon_uart = octeon_get_boot_uart();

#ifdef CONFIG_SMP
	octeon_write_lcd("LinuxSMP");
#else
	octeon_write_lcd("Linux");
#endif

#ifdef CONFIG_CAVIUM_GDB
	cvmx_debug_init ();
#endif

	octeon_setup_delays();

	/*
	 * BIST should always be enabled when doing a soft reset. L2
	 * Cache locking for instance is not cleared unless BIST is
	 * enabled.  Unfortunately due to a chip errata G-200 for
	 * Cn38XX and CN31XX, BIST msut be disabled on these parts.
	 */
	if (OCTEON_IS_MODEL(OCTEON_CN38XX_PASS2) ||
	    OCTEON_IS_MODEL(OCTEON_CN31XX))
		cvmx_write_csr(CVMX_CIU_SOFT_BIST, 0);
	else
		cvmx_write_csr(CVMX_CIU_SOFT_BIST, 1);

	/* Default to 64MB in the simulator to speed things up */
	if (octeon_is_simulation())
		MAX_MEMORY = 64ull << 20;

	arcs_cmdline[0] = 0;
	argc = octeon_boot_desc_ptr->argc;
	for (i = 0; i < argc; i++) {
		const char *arg = phys_to_virt(octeon_boot_desc_ptr->argv[i]);
		if ((strncmp(arg, "MEM=", 4) == 0) ||
		    (strncmp(arg, "mem=", 4) == 0)) {
			sscanf(arg + 4, "%llu", &MAX_MEMORY);
			MAX_MEMORY <<= 20;
			if (MAX_MEMORY == 0)
				MAX_MEMORY = 32ull << 30;
		} else if (strncmp(arg, "rd_name=", 8) == 0) {
			strncpy(rd_name, arg + 8, sizeof(rd_name));
			rd_name[sizeof(rd_name) - 1] = 0;
			goto append_arg;
		} else if (strncmp(arg, "uartdisable=", 12) == 0) {
			const char *str = arg + 12;
			const char *comma;

			while (str && *str) {
				comma = strchr(str, ',');
				if (comma == NULL || (comma - str) == 2) {
					switch (*str) {
					case '0':
						octeon_serial_uartdisable_mask |= 1;
						break;
					case '1':
						octeon_serial_uartdisable_mask |= 2;
						break;
					case '2':
						octeon_serial_uartdisable_mask |= 4;
						break;
					default:
						goto err;
					}
				} else {
err:
					pr_warning("Invalid uartdisable= parameter\n");
					break;
				}
				str = comma;
				if (str && *str)
					str++;
			}
			goto append_arg;
		}
#ifdef CONFIG_CAVIUM_REPORT_SINGLE_BIT_ECC
		else if (strcmp(arg, "ecc_verbose") == 0) {
			__cvmx_interrupt_ecc_report_single_bit_errors = 1;
			pr_notice("Reporting of single bit ECC errors is "
				  "turned on\n");
			goto append_arg;
		}
#endif
		else {
append_arg:
			if (strlen(arcs_cmdline) + strlen(arg) + 1 < sizeof(arcs_cmdline) - 1) {
				strcat(arcs_cmdline, " ");
				strcat(arcs_cmdline, arg);
			}
		}
	}

#ifdef CONFIG_CAVIUM_GDB
	octeon_serial_uartdisable_mask |= (1 << OCTEON_DEBUG_UART);
#endif

	if (octeon_serial_uartdisable_mask & (1 << octeon_uart))
		pr_warning("WARNING: Boot uart (%d) has been disabled!\n", octeon_uart);

	if (strstr(arcs_cmdline, "console=pci"))
		pci_console_init(strstr(arcs_cmdline, "console=pci") + 8);

	if (strstr(arcs_cmdline, "console=") == NULL) {
		if (octeon_uart == 1 && !(octeon_serial_uartdisable_mask & 1))
			strcat(arcs_cmdline, " console=ttyS1,115200");
		else
			strcat(arcs_cmdline, " console=ttyS0,115200");
	}

	if (octeon_is_simulation()) {
		/*
		 * The simulator uses a mtdram device pre filled with
		 * the filesystem. Also specify the calibration delay
		 * to avoid calculating it every time.
		 */
		strcat(arcs_cmdline, " rw root=1f00 slram=root,0x40000000,+1073741824");
	}

	mips_hpt_frequency = octeon_get_clock_rate();

	octeon_init_cvmcount();

	_machine_restart = octeon_restart;
	_machine_halt = octeon_halt;

	octeon_user_io_init();
	register_smp_ops(&octeon_smp_ops);

	if (OCTEON_IS_MODEL(OCTEON_CN6XXX))
		board_be_handler = octeon2_be_handler;


	labi = phys_to_virt(LABI_ADDR_IN_BOOTLOADER);
	if (labi->labi_signature == LABI_SIGNATURE)
		octeon_bootloader_entry_addr = labi->InitTLBStart_addr;

	pr_info("Cavium Networks SDK-" SDK_VERSION "\n");
}

#ifdef CONFIG_CAVIUM_OCTEON_LOCK_L2
static int __init octeon_l2_cache_lock(void)
{
	int is_octeon2 = (current_cpu_type() == CPU_CAVIUM_OCTEON2);

	if ((is_octeon2 && (cvmx_read_csr(CVMX_MIO_FUS_DAT3) & (3ull << 32)))
	    || (!is_octeon2 && (cvmx_read_csr(CVMX_L2D_FUS3) & (3ull << 34)))) {
		pr_info("Skipping L2 locking due to reduced L2 cache size\n");
	} else {
		u32 __maybe_unused my_ebase = read_c0_ebase() & 0x3ffff000;
		unsigned int __maybe_unused len = 0;
		unsigned int __maybe_unused len2 = 0;
#ifdef CONFIG_CAVIUM_OCTEON_LOCK_L2_TLB
		/* TLB refill */
		len = 0x100;
		pr_info("L2 lock: TLB refill %d bytes\n", len);
		cvmx_l2c_lock_mem_region(my_ebase, len);
#endif
#ifdef CONFIG_CAVIUM_OCTEON_LOCK_L2_EXCEPTION
		/* General exception */
		len = 0x80;
		pr_info("L2 lock: General exception %d bytes\n", len);
		cvmx_l2c_lock_mem_region(my_ebase + 0x180, len);
#endif
#ifdef CONFIG_CAVIUM_OCTEON_LOCK_L2_LOW_LEVEL_INTERRUPT
		/* Interrupt handler */
		len = 0x80;
		pr_info("L2 lock: low-level interrupt %d bytes\n", len);
		cvmx_l2c_lock_mem_region(my_ebase + 0x200, len);
#endif
#ifdef CONFIG_CAVIUM_OCTEON_LOCK_L2_INTERRUPT
		len = 0x100;
		len2 = 0x180;
		pr_info("L2 lock: interrupt %d bytes\n", len + len2);
		cvmx_l2c_lock_mem_region(__pa_symbol(handle_int), len);
		cvmx_l2c_lock_mem_region(__pa_symbol(plat_irq_dispatch), len2);
#endif
#ifdef CONFIG_CAVIUM_OCTEON_LOCK_L2_MEMCPY
		len = 0x480;
		pr_info("L2 lock: memcpy %d bytes\n", len);
		cvmx_l2c_lock_mem_region(__pa_symbol(memcpy), len);
#endif
	}
	return 0;
}
late_initcall(octeon_l2_cache_lock);
#endif

/* Exclude a single page from the regions obtained in plat_mem_setup. */
static __init void memory_exclude_page(u64 addr, u64 *mem, u64 *size)
{
	if (addr > *mem && addr < *mem + *size) {
		u64 inc = addr - *mem;
		add_memory_region(*mem, inc, BOOT_MEM_RAM);
		*mem += inc;
		*size -= inc;
	}

	if (addr == *mem && *size > PAGE_SIZE) {
		*mem += PAGE_SIZE;
		*size -= PAGE_SIZE;
	}
}

void __init plat_mem_setup(void)
{
	uint64_t mem_alloc_size;
	uint64_t total = 0;
	int64_t memory;

#ifdef CONFIG_BLK_DEV_INITRD
	const cvmx_bootmem_named_block_desc_t *initrd_block;

	if (rd_name[0] &&
	    (initrd_block = cvmx_bootmem_find_named_block(rd_name)) != NULL) {
		initrd_start = initrd_block->base_addr + PAGE_OFFSET;
		initrd_end = initrd_start + initrd_block->size;
		add_memory_region(initrd_block->base_addr, initrd_block->size,
			BOOT_MEM_INIT_RAM);
		initrd_in_reserved = 1;
		total += initrd_block->size;
	}
#endif

	/* First add the init memory we will be returning.  */
	memory = __pa_symbol(&__init_begin) & PAGE_MASK;
	mem_alloc_size = (__pa_symbol(&__init_end) & PAGE_MASK) - memory;
	if (mem_alloc_size > 0) {
		add_memory_region(memory, mem_alloc_size, BOOT_MEM_INIT_RAM);
		total += mem_alloc_size;
	}

	/*
	 * The Mips memory init uses the first memory location for
	 * some memory vectors. When SPARSEMEM is in use, it doesn't
	 * verify that the size is big enough for the final
	 * vectors. Making the smallest chuck 4MB seems to be enough
	 * to consistantly work.
	 */
	mem_alloc_size = 4 << 20;
	if (mem_alloc_size > MAX_MEMORY)
		mem_alloc_size = MAX_MEMORY;

	/*
	 * When allocating memory, we want incrementing addresses from
	 * bootmem_alloc so the code in add_memory_region can merge
	 * regions next to each other.
	 */
	cvmx_bootmem_lock();
	while ((boot_mem_map.nr_map < BOOT_MEM_MAP_MAX)
		&& (total < MAX_MEMORY)) {
#if defined(CONFIG_64BIT) || defined(CONFIG_64BIT_PHYS_ADDR)
		memory = cvmx_bootmem_phy_alloc(mem_alloc_size,
						__pa_symbol(&__init_end), -1,
						0x100000,
						CVMX_BOOTMEM_FLAG_NO_LOCKING);
#elif defined(CONFIG_HIGHMEM)
		memory = cvmx_bootmem_phy_alloc(mem_alloc_size, 0, 1ull << 31,
						0x100000,
						CVMX_BOOTMEM_FLAG_NO_LOCKING);
#else
		memory = cvmx_bootmem_phy_alloc(mem_alloc_size, 0, 512 << 20,
						0x100000,
						CVMX_BOOTMEM_FLAG_NO_LOCKING);
#endif
		if (memory >= 0) {
			u64 size = mem_alloc_size;

			/*
			 * exclude a page at the beginning and end of
			 * the 256MB PCIe 'hole' so the kernel will not
			 * try to allocate multi-page buffers that
			 * span the discontinuity.
			 */
			memory_exclude_page(CVMX_PCIE_BAR1_PHYS_BASE,
					    &memory, &size);
			memory_exclude_page(CVMX_PCIE_BAR1_PHYS_BASE +
					    CVMX_PCIE_BAR1_PHYS_SIZE,
					    &memory, &size);

			/*
			 * This function automatically merges address
			 * regions next to each other if they are
			 * received in incrementing order.
			 */
			if (size)
				add_memory_region(memory, size, BOOT_MEM_RAM);
			total += mem_alloc_size;
		} else {
			break;
		}
	}
	cvmx_bootmem_unlock();

#if CONFIG_CAVIUM_RESERVE32
	/*
	 * Now that we've allocated the kernel memory it is safe to
	 * free the reserved region. We free it here so that builtin
	 * drivers can use the memory.
	 */
	if (octeon_reserve32_memory)
		cvmx_bootmem_free_named("CAVIUM_RESERVE32");
#endif /* CONFIG_CAVIUM_RESERVE32 */

	if (total == 0)
		panic("Unable to allocate memory from "
		      "cvmx_bootmem_phy_alloc\n");
}


int prom_putchar(char c)
{
	uint64_t lsrval;

	/* Spin until there is room */
	do {
		lsrval = cvmx_read_csr(CVMX_MIO_UARTX_LSR(octeon_uart));
	} while ((lsrval & 0x20) == 0);

	/* Write the byte */
	cvmx_write_csr(CVMX_MIO_UARTX_THR(octeon_uart), c & 0xffull);
	return 1;
}
EXPORT_SYMBOL(prom_putchar);

void prom_free_prom_memory(void)
{
	if (OCTEON_IS_MODEL(OCTEON_CN63XX_PASS1_X)) {
		/* Check for presence of Core-14449 fix.  */
		u32 insn;
		u32 *foo;

		foo = &insn;

		asm volatile("# before" : : : "memory");
		prefetch(foo);
		asm volatile(
			".set push\n\t"
			".set noreorder\n\t"
			"bal 1f\n\t"
			"nop\n"
			"1:\tlw %0,-12($31)\n\t"
			".set pop\n\t"
			: "=r" (insn) : : "$31", "memory");

		if ((insn >> 26) != 0x33)
			panic("No PREF instruction at Core-14449 probe point.\n");

		if (((insn >> 16) & 0x1f) != 28)
			panic("Core-14449 WAR not in place (%04x).\n"
				"Please build kernel with proper options (CONFIG_CAVIUM_OCTEON2).\n", insn);
	}
#ifdef CONFIG_CAVIUM_DECODE_RSL
	/* Add an interrupt handler for general failures. */
	if (OCTEON_IS_MODEL(OCTEON_CN68XX)) {
		if (request_irq(OCTEON_IRQ_NAND, octeon_rml_interrupt, IRQF_SHARED,
					"NAND RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_NAND)\n");
		}
		if (request_irq(OCTEON_IRQ_MIO, octeon_rml_interrupt, IRQF_SHARED,
					"MIO RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_MIO)\n");
		}
		if (request_irq(OCTEON_IRQ_FPA, octeon_rml_interrupt, IRQF_SHARED,
					"FPA RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_FPA)\n");
		}
#if 0
		if (request_irq(OCTEON_IRQ_POW, octeon_rml_interrupt, IRQF_SHARED,
					"POW RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_POW)\n");
		}
#endif
		if (request_irq(OCTEON_IRQ_L2C, octeon_rml_interrupt, IRQF_SHARED,
					"L2C RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_L2C)\n");
		}
		if (request_irq(OCTEON_IRQ_IPD, octeon_rml_interrupt, IRQF_SHARED,
					"IPD RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_IPD)\n");
		}
		if (request_irq(OCTEON_IRQ_PKO, octeon_rml_interrupt, IRQF_SHARED,
					"PKO RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_PKO)\n");
		}
		if (request_irq(OCTEON_IRQ_ZIP, octeon_rml_interrupt, IRQF_SHARED,
					"ZIP RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_ZIP)\n");
		}
		if (request_irq(OCTEON_IRQ_RAD, octeon_rml_interrupt, IRQF_SHARED,
					"RAD RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_RAD)\n");
		}
		if (request_irq(OCTEON_IRQ_KEY, octeon_rml_interrupt, IRQF_SHARED,
					"KEY RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_KEY)\n");
		}
		if (request_irq(OCTEON_IRQ_DFA, octeon_rml_interrupt, IRQF_SHARED,
					"DFA RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_DFA)\n");
		}
		if (request_irq(OCTEON_IRQ_DPI, octeon_rml_interrupt, IRQF_SHARED,
					"DPI RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_DPI)\n");
		}
		if (request_irq(OCTEON_IRQ_AGL, octeon_rml_interrupt, IRQF_SHARED,
					"AGL RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_AGL)\n");
		}
	} else {
		if (request_irq(OCTEON_IRQ_RML, octeon_rml_interrupt, IRQF_SHARED,
					"RML/RSL", octeon_rml_interrupt)) {
			panic("Unable to request_irq(OCTEON_IRQ_RML)\n");
		}
	}
#endif
}

int octeon_prune_device_tree(void);

extern const char __dtb_octeon_3xxx_begin;
extern const char __dtb_octeon_3xxx_end;
extern const char __dtb_octeon_68xx_begin;
extern const char __dtb_octeon_68xx_end;
void __init device_tree_init(void)
{
	int dt_size;
	struct boot_param_header *fdt;
	bool do_prune;

	if (octeon_bootinfo->minor_version >= 3 && octeon_bootinfo->fdt_addr) {
		fdt = phys_to_virt(octeon_bootinfo->fdt_addr);
		if (fdt_check_header(fdt))
			panic("Corrupt Device Tree passed to kernel.");
		dt_size = be32_to_cpu(fdt->totalsize);
		do_prune = false;
	} else if (OCTEON_IS_MODEL(OCTEON_CN68XX)) {
		fdt = (struct boot_param_header *)&__dtb_octeon_68xx_begin;
		dt_size = &__dtb_octeon_68xx_end - &__dtb_octeon_68xx_begin;
		do_prune = true;
	} else {
		fdt = (struct boot_param_header *)&__dtb_octeon_3xxx_begin;
		dt_size = &__dtb_octeon_3xxx_end - &__dtb_octeon_3xxx_begin;
		do_prune = true;
	}

	/* Copy the default tree from init memory. */
	initial_boot_params = early_init_dt_alloc_memory_arch(dt_size, 8);
	if (initial_boot_params == NULL)
		panic("Could not allocate initial_boot_params\n");
	memcpy(initial_boot_params, fdt, dt_size);

	if (do_prune) {
		octeon_prune_device_tree();
		pr_info("Using internal Device Tree.\n");
	} else {
		pr_info("Using passed Device Tree.\n");
	}
	unflatten_device_tree();
}

/*
  * set autelan product info by reading CPLD.
  * 
  * caojia@autelan.com
  * 12/17/2011
  *
  */
void octeon_autelan_product_init(void)
{
	unsigned char product_series_num = 0x0;
	unsigned char product_type_num = 0x0;
	unsigned char board_type_num = 0x0;
	unsigned char board_slot_id = 0x0;
	unsigned char reg_data = 0x0;

	memset(&autelan_product_info, 0, sizeof(product_info_t));
	
	product_series_num = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_PRODUCT_SERIES_REG + (1ull<<63));
	product_series_num &= 0xf;
	
	reg_data = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_PRODUCT_TYPE_REG + (1ull<<63));
	product_type_num = (reg_data >> 2) & CPLD_PRODUCT_TYPE_MASK;
	
	board_type_num = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_BOARD_TYPE_REG + (1ull<<63));

	board_slot_id = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_BOARD_SLOT_ID_REG + (1ull<<63));
	board_slot_id += 1;

	autelan_product_info.product_series_num = product_series_num;
	autelan_product_info.product_type_num = product_type_num;
	autelan_product_info.board_type_num = board_type_num;
	autelan_product_info.board_slot_id = board_slot_id;
	
	if(product_type_num == 0x1)
	{
		autelan_product_info.product_type = AUTELAN_PRODUCT_AX7605I;
		autelan_product_info.distributed_product = 1;
		if (board_type_num == 0x80)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX71_CRSMU;
		}
		else if (board_type_num == 0x4)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX71_2X12G12S;
		}
		else if (board_type_num == 0x5)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX71_1X12G12S;
		}
		else
		{
			autelan_product_info.board_type = AUTELAN_UNKNOWN_BOARD;
		}
	}
	else if(product_type_num == 0x3)
	{
	
		autelan_product_info.product_type = AUTELAN_PRODUCT_AX7605;
		if (board_type_num == 0x80)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX71_CRSMU;
		}
	
	}
	else if (product_type_num == 0x4)
	{
		autelan_product_info.product_type = AUTELAN_PRODUCT_AX8610;
		autelan_product_info.distributed_product = 1;
		if (board_type_num == 0x0)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_SMU;
		}
		else if (board_type_num == 0x1)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC12C;
		}
		else if (board_type_num == 0x2)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC8C;
		}
		else if (board_type_num == 0x3)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC_12X;
		}
		else if (board_type_num == 0x4)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_2X12G12S;
		}
		else if (board_type_num == 0x5)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_1X12G12S;
		}
		else if (board_type_num == 0x6)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC_4X;
		}
		else if (board_type_num == 0x7)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_SMUE;
		}
		else
		{
			autelan_product_info.board_type = AUTELAN_UNKNOWN_BOARD;
		}
	}
	else if (product_type_num == 0x5)
	{
		autelan_product_info.product_type = AUTELAN_PRODUCT_AX8606;
		autelan_product_info.distributed_product = 1;
		if (board_type_num == 0x0)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_SMU;
			autelan_product_info.board_slot_id -= 2;
		}
		else if (board_type_num == 0x1)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC12C;
		}
		else if (board_type_num == 0x2)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC8C;
		}
		else if (board_type_num == 0x3)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC_12X;
		}
		else if (board_type_num == 0x4)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_2X12G12S;
		}
		else if (board_type_num == 0x5)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_1X12G12S;
		}
		else if (board_type_num == 0x6)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC_4X;
		}
		else if (board_type_num == 0x7)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_SMUE;
		}
		else
		{
			autelan_product_info.board_type = AUTELAN_UNKNOWN_BOARD;
		}
	}
	else if (product_type_num == 0x6)
	{
		autelan_product_info.product_type = AUTELAN_PRODUCT_AX8603;
		autelan_product_info.distributed_product = 1;
		if (board_type_num == 0x0)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_SMU;
		}
		else if (board_type_num == 0x1)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC12C;
		}
		else if (board_type_num == 0x2)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC8C;
		}
		else if (board_type_num == 0x3)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC_12X;
		}
		else if (board_type_num == 0x4)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_2X12G12S;
		}
		else if (board_type_num == 0x5)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_1X12G12S;
		}
		else if (board_type_num == 0x6)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC_4X;
		}
		else
		{
			autelan_product_info.board_type = AUTELAN_UNKNOWN_BOARD;
		}
	}
	else if (product_type_num == 0x7)
	{
		autelan_product_info.product_type = AUTELAN_PRODUCT_AX8800;
		autelan_product_info.distributed_product = 1;
		if (board_type_num == 0x0)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_SMU;
			autelan_product_info.board_slot_id += 2;
		}
		else if (board_type_num == 0x1)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC12C;
		}
		else if (board_type_num == 0x2)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC8C;
		}
		else if (board_type_num == 0x3)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC_12X;
		}
		else if (board_type_num == 0x4)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_2X12G12S;
		}
		else if (board_type_num == 0x5)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_1X12G12S;
		}
		else if (board_type_num == 0x6)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_AC_4X;
		}
		else if (board_type_num == 0x7)
		{
			autelan_product_info.board_type = AUTELAN_BOARD_AX81_SMUE;
			autelan_product_info.board_slot_id += 2;
		}
		else
		{
			autelan_product_info.board_type = AUTELAN_UNKNOWN_BOARD;
		}
	}
	else
	{
		autelan_product_info.product_type = AUTELAN_UNKNOWN_PRODUCT;
		autelan_product_info.board_type = AUTELAN_UNKNOWN_BOARD;
		autelan_product_info.distributed_product = 0;
	}

	pr_info("Product type : %d\n", autelan_product_info.product_type);
	pr_info("Board type : %d\n", autelan_product_info.board_type);
	pr_info("Slot id : %d\n", autelan_product_info.board_slot_id);

	return;
}

