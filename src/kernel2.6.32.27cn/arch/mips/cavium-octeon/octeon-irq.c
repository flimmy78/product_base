/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004-2010 Cavium Networks
 */

#include <linux/bitops.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/percpu.h>
#include <linux/smp.h>
#include <linux/of_irq.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-ciu2-defs.h>

DEFINE_SPINLOCK(octeon_irq_ciu0_spinlock);
DEFINE_SPINLOCK(octeon_irq_ciu1_spinlock);

static DEFINE_PER_CPU(volatile unsigned long, octeon_irq_ciu0_en_mirror);
static DEFINE_PER_CPU(volatile unsigned long, octeon_irq_ciu1_en_mirror);

static __read_mostly u8 octeon_irq_ciu_to_irq[8][64];

union octeon_ciu_chip_data {
	void *p;
	unsigned long l;
	struct {
		unsigned int line:6;
		unsigned int bit:6;
	} s;
};

static void __init octeon_irq_set_ciu_mapping(int irq, int line, int bit,
					      struct irq_chip *chip, irq_flow_handler_t handler)
{
	union octeon_ciu_chip_data cd;

	set_irq_chip_and_handler(irq, chip, handler);

	cd.l = 0;
	cd.s.line = line;
	cd.s.bit = bit;

	set_irq_chip_data(irq, cd.p);
	octeon_irq_ciu_to_irq[line][bit] = irq;
}

static unsigned int octeon_irq_gpio_mapping(struct device_node *controller,
					    const u32 *intspec,
					    unsigned int intsize)
{
	struct of_irq oirq;
	int i;
	unsigned int irq = 0;
	unsigned int type;
	unsigned int ciu = 0, bit = 0;
	unsigned int pin = be32_to_cpup(intspec);
	unsigned int trigger = be32_to_cpup(intspec + 1);
	bool set_edge_handler = false;

	if (pin >= 16)
		goto err;
	i = of_irq_map_one(controller, 0, &oirq);
	if (i)
		goto err;
	if (oirq.size != 2)
		goto err_put;

	ciu = oirq.specifier[0];
	bit = oirq.specifier[1] + pin;

	if (ciu >= 8 || bit >= 64)
		goto err_put;

	irq = octeon_irq_ciu_to_irq[ciu][bit];
	if (!irq)
		goto err_put;

	switch (trigger & 0xf) {
	case 1:
		type = IRQ_TYPE_EDGE_RISING;
		set_edge_handler = true;
		break;
	case 2:
		type = IRQ_TYPE_EDGE_FALLING;
		set_edge_handler = true;
		break;
	case 4:
		type = IRQ_TYPE_LEVEL_HIGH;
		break;
	case 8:
		type = IRQ_TYPE_LEVEL_LOW;
		break;
	default:
		pr_err("Error: Invalid irq trigger specification: %x\n",
		       trigger);
		type = IRQ_TYPE_LEVEL_LOW;
		break;
	}

	set_irq_type(irq, type);

	if (set_edge_handler)
		__set_irq_handler(irq, handle_edge_irq, 0, NULL);

err_put:
	of_node_put(oirq.controller);
err:
	return irq;
}

/*
 * irq_create_of_mapping - Hook to resolve OF irq specifier into a Linux irq#
 *
 * Octeon irq maps are a pair of indexes.  The first selects either
 * ciu0 or ciu1, the second is the bit within the ciu register.
 */
unsigned int irq_create_of_mapping(struct device_node *controller,
				   const u32 *intspec, unsigned int intsize)
{
	unsigned int irq = 0;
	unsigned int ciu, bit;

	if (of_device_is_compatible(controller, "cavium,octeon-3860-gpio"))
		return octeon_irq_gpio_mapping(controller, intspec, intsize);

	ciu = be32_to_cpup(intspec);
	bit = be32_to_cpup(intspec + 1);

	if (ciu < 8 && bit < 64)
		irq = octeon_irq_ciu_to_irq[ciu][bit];

	return irq;
}
EXPORT_SYMBOL(irq_create_of_mapping);

int octeon_coreid_for_cpu(int cpu)
{
#ifdef CONFIG_SMP
	return cpu_logical_map(cpu);
#else
	return cvmx_get_core_num();
#endif
}

static int octeon_cpu_for_coreid(int coreid)
{
#ifdef CONFIG_SMP
	return cpu_number_map(coreid);
#else
	return smp_processor_id();
#endif
}

static void octeon_irq_core_ack(unsigned int irq)
{
	unsigned int bit = irq - OCTEON_IRQ_SW0;
	/*
	 * We don't need to disable IRQs to make these atomic since
	 * they are already disabled earlier in the low level
	 * interrupt code.
	 */
	clear_c0_status(0x100 << bit);
	/* The two user interrupts must be cleared manually. */
	if (bit < 2)
		clear_c0_cause(0x100 << bit);
}

static void octeon_irq_core_eoi(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	unsigned int bit = irq - OCTEON_IRQ_SW0;
	/*
	 * If an IRQ is being processed while we are disabling it the
	 * handler will attempt to unmask the interrupt after it has
	 * been disabled.
	 */
	if ((unlikely(desc->status & IRQ_DISABLED)))
		return;
	/*
	 * We don't need to disable IRQs to make these atomic since
	 * they are already disabled earlier in the low level
	 * interrupt code.
	 */
	set_c0_status(0x100 << bit);
}

static void octeon_irq_core_enable(unsigned int irq)
{
	unsigned long flags;
	unsigned int bit = irq - OCTEON_IRQ_SW0;

	/*
	 * We need to disable interrupts to make sure our updates are
	 * atomic.
	 */
	local_irq_save(flags);
	set_c0_status(0x100 << bit);
	local_irq_restore(flags);
}

static void octeon_irq_core_disable_local(unsigned int irq)
{
	unsigned long flags;
	unsigned int bit = irq - OCTEON_IRQ_SW0;
	/*
	 * We need to disable interrupts to make sure our updates are
	 * atomic.
	 */
	local_irq_save(flags);
	clear_c0_status(0x100 << bit);
	local_irq_restore(flags);
}

static void octeon_irq_core_disable(unsigned int irq)
{
#ifdef CONFIG_SMP
	on_each_cpu((void (*)(void *)) octeon_irq_core_disable_local,
		    (void *) (long) irq, 1);
#else
	octeon_irq_core_disable_local(irq);
#endif
}

static struct irq_chip octeon_irq_chip_core = {
	.name = "Core",
	.enable = octeon_irq_core_enable,
	.disable = octeon_irq_core_disable,
	.ack = octeon_irq_core_ack,
	.eoi = octeon_irq_core_eoi,
};

static int next_cpu_for_irq(struct irq_desc *desc)
{

#ifdef CONFIG_SMP
	int cpu;
	int weight = cpumask_weight(desc->affinity);

	if (weight > 1) {
		cpu = smp_processor_id();
		for (;;) {
			cpu = cpumask_next(cpu, desc->affinity);
			if (cpu >= nr_cpu_ids) {
				cpu = -1;
				continue;
			} else if (cpumask_test_cpu(cpu, cpu_online_mask)) {
				break;
			}
		}
	} else if (weight == 1) {
		cpu = cpumask_first(desc->affinity);
	} else {
		cpu = smp_processor_id();
	}
	return cpu;
#else
	return smp_processor_id();
#endif
}

static void octeon_irq_ciu_enable(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	int cpu = next_cpu_for_irq(desc);
	int coreid = octeon_coreid_for_cpu(cpu);
	volatile unsigned long *pen;
	unsigned long flags;
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;

	if (cd.s.line == 0) {
		spin_lock_irqsave(&octeon_irq_ciu0_spinlock, flags);
		pen = &per_cpu(octeon_irq_ciu0_en_mirror, cpu);
		set_bit(cd.s.bit, pen);
		cvmx_write_csr(CVMX_CIU_INTX_EN0(coreid * 2), *pen);
		spin_unlock_irqrestore(&octeon_irq_ciu0_spinlock, flags);
	} else {
		spin_lock_irqsave(&octeon_irq_ciu1_spinlock, flags);
		pen = &per_cpu(octeon_irq_ciu1_en_mirror, cpu);
		set_bit(cd.s.bit, pen);
		cvmx_write_csr(CVMX_CIU_INTX_EN1(coreid * 2 + 1), *pen);
		spin_unlock_irqrestore(&octeon_irq_ciu1_spinlock, flags);
	}
}

static void octeon_irq_ciu_enable_local(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	int coreid = cvmx_get_core_num();
	volatile unsigned long *pen;
	unsigned long flags;
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;

	if (cd.s.line == 0) {
		spin_lock_irqsave(&octeon_irq_ciu0_spinlock, flags);
		pen = &__get_cpu_var(octeon_irq_ciu0_en_mirror);
		set_bit(cd.s.bit, pen);
		cvmx_write_csr(CVMX_CIU_INTX_EN0(coreid * 2), *pen);
		spin_unlock_irqrestore(&octeon_irq_ciu0_spinlock, flags);
	} else {
		spin_lock_irqsave(&octeon_irq_ciu1_spinlock, flags);
		pen = &__get_cpu_var(octeon_irq_ciu1_en_mirror);
		set_bit(cd.s.bit, pen);
		cvmx_write_csr(CVMX_CIU_INTX_EN1(coreid * 2 + 1), *pen);
		spin_unlock_irqrestore(&octeon_irq_ciu1_spinlock, flags);
	}
}

static void octeon_irq_ciu_disable(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	unsigned long flags;
	volatile unsigned long *pen;
	int cpu;
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;

	if (cd.s.line == 0) {
		spin_lock_irqsave(&octeon_irq_ciu0_spinlock, flags);
		for_each_online_cpu(cpu) {
			int coreid = octeon_coreid_for_cpu(cpu);
			pen = &per_cpu(octeon_irq_ciu0_en_mirror, cpu);
			clear_bit(cd.s.bit, pen);
			cvmx_write_csr(CVMX_CIU_INTX_EN0(coreid * 2), *pen);
		}
		spin_unlock_irqrestore(&octeon_irq_ciu0_spinlock, flags);
	} else {
		spin_lock_irqsave(&octeon_irq_ciu1_spinlock, flags);
		for_each_online_cpu(cpu) {
			int coreid = octeon_coreid_for_cpu(cpu);
			pen = &per_cpu(octeon_irq_ciu1_en_mirror, cpu);
			clear_bit(cd.s.bit, pen);
			cvmx_write_csr(CVMX_CIU_INTX_EN1(coreid * 2 + 1), *pen);
		}
		spin_unlock_irqrestore(&octeon_irq_ciu1_spinlock, flags);
	}
}

/*
 * Enable the irq on the next core in the affinity set for chips that
 * have the EN*_W1{S,C} registers.
 */
static void octeon_irq_ciu_enable_v2(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	u64 mask;
	int cpu = next_cpu_for_irq(desc);
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;
	mask = 1ull << (cd.s.bit);

	/*
	 * Called under the desc lock, so these should never get out
	 * of sync.
	 */
	if (cd.s.line == 0) {
		int index = octeon_coreid_for_cpu(cpu) * 2;
		set_bit(cd.s.bit, &per_cpu(octeon_irq_ciu0_en_mirror, cpu));
		cvmx_write_csr(CVMX_CIU_INTX_EN0_W1S(index), mask);
	} else {
		int index = octeon_coreid_for_cpu(cpu) * 2 + 1;
		set_bit(cd.s.bit, &per_cpu(octeon_irq_ciu1_en_mirror, cpu));
		cvmx_write_csr(CVMX_CIU_INTX_EN1_W1S(index), mask);
	}
}

/*
 * Enable the irq on the current CPU for chips that
 * have the EN*_W1{S,C} registers.
 */
static void octeon_irq_ciu_enable_local_v2(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	u64 mask;
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;
	mask = 1ull << (cd.s.bit);

	if (cd.s.line == 0) {
		int index = cvmx_get_core_num() * 2;
		set_bit(cd.s.bit, &__get_cpu_var(octeon_irq_ciu0_en_mirror));
		cvmx_write_csr(CVMX_CIU_INTX_EN0_W1S(index), mask);
	} else {
		int index = cvmx_get_core_num() * 2 + 1;
		set_bit(cd.s.bit, &__get_cpu_var(octeon_irq_ciu1_en_mirror));
		cvmx_write_csr(CVMX_CIU_INTX_EN1_W1S(index), mask);
	}
}

static void octeon_irq_ciu_enable_check_v2(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	if ((desc->status & IRQ_DISABLED) == 0)
		octeon_irq_ciu_enable_v2(irq);
}

static void octeon_irq_ciu_enable_local_check_v2(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	if ((desc->status & IRQ_DISABLED) == 0)
		octeon_irq_ciu_enable_local_v2(irq);
}

static void octeon_irq_ciu_disable_local_v2(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	u64 mask;
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;
	mask = 1ull << (cd.s.bit);

	if (cd.s.line == 0) {
		int index = cvmx_get_core_num() * 2;
		clear_bit(cd.s.bit, &__get_cpu_var(octeon_irq_ciu0_en_mirror));
		cvmx_write_csr(CVMX_CIU_INTX_EN0_W1C(index), mask);
	} else {
		int index = cvmx_get_core_num() * 2 + 1;
		clear_bit(cd.s.bit, &__get_cpu_var(octeon_irq_ciu1_en_mirror));
		cvmx_write_csr(CVMX_CIU_INTX_EN1_W1C(index), mask);
	}
}

/*
 * Write to the W1C bit in CVMX_CIU_INTX_SUM0 to clear the irq.
 */
static void octeon_irq_ciu_ack(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	u64 mask;
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;
	mask = 1ull << (cd.s.bit);

	if (cd.s.line == 0) {
		int index = cvmx_get_core_num() * 2;
		cvmx_write_csr(CVMX_CIU_INTX_SUM0(index), mask);
	} else {
		cvmx_write_csr(CVMX_CIU_INT_SUM1, mask);
	}
}

/*
 * Disable the irq on the all cores for chips that have the EN*_W1{S,C}
 * registers.
 */
static void octeon_irq_ciu_disable_all_v2(unsigned int irq)
{
	int cpu;
	struct irq_desc *desc = irq_to_desc(irq);
	u64 mask;
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;
	mask = 1ull << (cd.s.bit);

	if (cd.s.line == 0) {
		for_each_online_cpu(cpu) {
			int index = octeon_coreid_for_cpu(cpu) * 2;
			clear_bit(cd.s.bit, &per_cpu(octeon_irq_ciu0_en_mirror, cpu));
			cvmx_write_csr(CVMX_CIU_INTX_EN0_W1C(index), mask);
		}
	} else {
		for_each_online_cpu(cpu) {
			int index = octeon_coreid_for_cpu(cpu) * 2 + 1;
			clear_bit(cd.s.bit, &per_cpu(octeon_irq_ciu1_en_mirror, cpu));
			cvmx_write_csr(CVMX_CIU_INTX_EN1_W1C(index), mask);
		}
	}
}

#ifdef CONFIG_SMP
static int octeon_irq_ciu_set_affinity(unsigned int irq, const struct cpumask *dest)
{
	int cpu;
	struct irq_desc *desc = irq_to_desc(irq);
	int enable_one = (desc->status & IRQ_DISABLED) == 0;
	unsigned long flags;
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;

	/*
	 * For non-v2 CIU, we will allow only single CPU affinity.
	 * This removes the need to do locking in the .ack/.eoi
	 * functions.
	 */
	if (cpumask_weight(dest) != 1)
		return -EINVAL;

	if (desc->status & IRQ_DISABLED)
		return 0;

	if (cd.s.line == 0) {
		spin_lock_irqsave(&octeon_irq_ciu0_spinlock, flags);
		for_each_online_cpu(cpu) {
			int coreid = octeon_coreid_for_cpu(cpu);
			volatile unsigned long *pen = &per_cpu(octeon_irq_ciu0_en_mirror, cpu);

			if (cpumask_test_cpu(cpu, dest) && enable_one) {
				enable_one = 0;
				set_bit(cd.s.bit, pen);
			} else {
				clear_bit(cd.s.bit, pen);
			}
			cvmx_write_csr(CVMX_CIU_INTX_EN0(coreid * 2), *pen);
		}
		spin_unlock_irqrestore(&octeon_irq_ciu0_spinlock, flags);
	} else {
		spin_lock_irqsave(&octeon_irq_ciu1_spinlock, flags);
		for_each_online_cpu(cpu) {
			int coreid = octeon_coreid_for_cpu(cpu);
			volatile unsigned long *pen = &per_cpu(octeon_irq_ciu1_en_mirror, cpu);

			if (cpumask_test_cpu(cpu, dest) && enable_one) {
				enable_one = 0;
				set_bit(cd.s.bit, pen);
			} else {
				clear_bit(cd.s.bit, pen);
			}
			cvmx_write_csr(CVMX_CIU_INTX_EN1(coreid * 2 + 1), *pen);
		}
		spin_unlock_irqrestore(&octeon_irq_ciu1_spinlock, flags);
	}
	return 0;
}

/*
 * Set affinity for the irq for chips that have the EN*_W1{S,C}
 * registers.
 */
static int octeon_irq_ciu_set_affinity_v2(unsigned int irq,
					   const struct cpumask *dest)
{
	int cpu;
	struct irq_desc *desc = irq_to_desc(irq);
	int enable_one = (desc->status & IRQ_DISABLED) == 0;
	u64 mask;
	union octeon_ciu_chip_data cd;

	if (desc->status & IRQ_DISABLED)
		return 0;

	cd.p = desc->chip_data;
	mask = 1ull << cd.s.bit;

	if (cd.s.line == 0) {
		for_each_online_cpu(cpu) {
			volatile unsigned long *pen = &per_cpu(octeon_irq_ciu0_en_mirror, cpu);
			int index = octeon_coreid_for_cpu(cpu) * 2;
			if (cpumask_test_cpu(cpu, dest) && enable_one) {
				enable_one = 0;
				set_bit(cd.s.bit, pen);
				cvmx_write_csr(CVMX_CIU_INTX_EN0_W1S(index), mask);
			} else {
				clear_bit(cd.s.bit, pen);
				cvmx_write_csr(CVMX_CIU_INTX_EN0_W1C(index), mask);
			}
		}
	} else {
		for_each_online_cpu(cpu) {
			volatile unsigned long *pen = &per_cpu(octeon_irq_ciu1_en_mirror, cpu);
			int index = octeon_coreid_for_cpu(cpu) * 2 + 1;
			if (cpumask_test_cpu(cpu, dest) && enable_one) {
				enable_one = 0;
				set_bit(cd.s.bit, pen);
				cvmx_write_csr(CVMX_CIU_INTX_EN1_W1S(index), mask);
			} else {
				clear_bit(cd.s.bit, pen);
				cvmx_write_csr(CVMX_CIU_INTX_EN1_W1C(index), mask);
			}
		}
	}
	return 0;
}
#endif

/*
 * The v1 CIU code already masks things, so supply a dummy version to
 * the core chip code.
 */
static void octeon_irq_dummy_mask(unsigned int irq)
{
	return;
}

/*
 * Newer octeon chips have support for lockless CIU operation.
 */
static struct irq_chip octeon_irq_chip_ciu_v2 = {
	.name = "CIU",
	.enable = octeon_irq_ciu_enable_v2,
	.disable = octeon_irq_ciu_disable_all_v2,
	.mask = octeon_irq_ciu_disable_local_v2,
	.unmask = octeon_irq_ciu_enable_check_v2,
#ifdef CONFIG_SMP
	.set_affinity = octeon_irq_ciu_set_affinity_v2,
#endif
};

static struct irq_chip octeon_irq_chip_ciu_edge_v2 = {
	.name = "CIU-E",
	.enable = octeon_irq_ciu_enable_v2,
	.disable = octeon_irq_ciu_disable_all_v2,
	.ack = octeon_irq_ciu_ack,
	.mask = octeon_irq_ciu_disable_local_v2,
	.unmask = octeon_irq_ciu_enable_check_v2,
#ifdef CONFIG_SMP
	.set_affinity = octeon_irq_ciu_set_affinity_v2,
#endif
};

static struct irq_chip octeon_irq_chip_ciu = {
	.name = "CIU",
	.enable = octeon_irq_ciu_enable,
	.disable = octeon_irq_ciu_disable,
	.mask = octeon_irq_dummy_mask,
#ifdef CONFIG_SMP
	.set_affinity = octeon_irq_ciu_set_affinity,
#endif
};

static struct irq_chip octeon_irq_chip_ciu_edge = {
	.name = "CIU-E",
	.enable = octeon_irq_ciu_enable,
	.disable = octeon_irq_ciu_disable,
	.mask = octeon_irq_dummy_mask,
	.ack = octeon_irq_ciu_ack,
#ifdef CONFIG_SMP
	.set_affinity = octeon_irq_ciu_set_affinity,
#endif
};

/* The mbox versions don't do any affinity or round-robin. */
static struct irq_chip octeon_irq_chip_ciu_mbox_v2 = {
	.name = "CIU-M",
	.enable = octeon_irq_ciu_enable_local_v2,
	.disable = octeon_irq_ciu_disable_all_v2,
	.ack = octeon_irq_ciu_disable_local_v2,
	.eoi = octeon_irq_ciu_enable_local_check_v2,
};

static struct irq_chip octeon_irq_chip_ciu_mbox = {
	.name = "CIU-M",
	.enable = octeon_irq_ciu_enable_local,
	.disable = octeon_irq_ciu_disable,
};

/*
 * Watchdog interrupts are special.  They are associated with a single
 * core, so we hardwire the affinity to that core.
 */
static void octeon_irq_ciu_wd_enable(unsigned int irq)
{
	unsigned long flags;
	int coreid = irq - OCTEON_IRQ_WDOG0;	/* Bit 0-63 of EN1 */

	volatile unsigned long *pen;
	int cpu = octeon_cpu_for_coreid(coreid);

	spin_lock_irqsave(&octeon_irq_ciu1_spinlock, flags);
	pen = &per_cpu(octeon_irq_ciu1_en_mirror, cpu);
	set_bit(coreid, pen);
	cvmx_write_csr(CVMX_CIU_INTX_EN1(coreid * 2 + 1), *pen);
	spin_unlock_irqrestore(&octeon_irq_ciu1_spinlock, flags);
}

/*
 * Watchdog interrupts are special.  They are associated with a single
 * core, so we hardwire the affinity to that core.
 */
static void octeon_irq_ciu1_wd_enable_v2(unsigned int irq)
{
	int coreid = irq - OCTEON_IRQ_WDOG0;
	int cpu = octeon_cpu_for_coreid(coreid);

	set_bit(coreid, &per_cpu(octeon_irq_ciu1_en_mirror, cpu));
	cvmx_write_csr(CVMX_CIU_INTX_EN1_W1S(coreid * 2 + 1), 1ull << coreid);
}


static struct irq_chip octeon_irq_chip_ciu_wd_v2 = {
	.name = "CIU-W",
	.enable = octeon_irq_ciu1_wd_enable_v2,
	.disable = octeon_irq_ciu_disable_all_v2,
	.mask = octeon_irq_ciu_disable_local_v2,
	.unmask = octeon_irq_ciu_enable_local_check_v2,
};

static struct irq_chip octeon_irq_chip_ciu_wd = {
	.name = "CIU-W",
	.enable = octeon_irq_ciu_wd_enable,
	.disable = octeon_irq_ciu_disable,
	.mask = octeon_irq_dummy_mask,
};


static void octeon_irq_gpio_setup(unsigned int irq)
{
	union cvmx_gpio_bit_cfgx cfg;
	struct irq_desc *desc = irq_to_desc(irq);
	int bit = irq - OCTEON_IRQ_GPIO0;
        unsigned int t = desc->status;

	cfg.u64 = 0;
	cfg.s.int_en = 1;
	cfg.s.int_type = (t & (IRQ_TYPE_EDGE_RISING | IRQ_TYPE_EDGE_FALLING)) != 0;
	cfg.s.rx_xor = (t & (IRQ_TYPE_LEVEL_LOW | IRQ_TYPE_EDGE_FALLING)) != 0;

	/* 1 uS glitch filter*/
	cfg.s.fil_cnt = 7;
	cfg.s.fil_sel = 3;

	cvmx_write_csr(CVMX_GPIO_BIT_CFGX(bit), cfg.u64);
}

static void octeon_irq_ciu_enable_gpio_v2(unsigned int irq)
{
	octeon_irq_gpio_setup(irq);
	octeon_irq_ciu_enable_v2(irq);
}

static void octeon_irq_ciu_enable_gpio(unsigned int irq)
{
	octeon_irq_gpio_setup(irq);
	octeon_irq_ciu_enable(irq);
}

static int octeon_irq_ciu_gpio_set_type(unsigned int irq, unsigned int t)
{
	struct irq_desc *desc = irq_to_desc(irq);
	unsigned int current_type = desc->status & IRQ_TYPE_SENSE_MASK;

	/* If the type has been set, don't change it */
	if (current_type && current_type != t)
		return -EINVAL;

	desc->status &= ~IRQ_TYPE_SENSE_MASK;
	desc->status |= t & IRQ_TYPE_SENSE_MASK;
	return 0;
}

static void octeon_irq_ciu_disable_gpio_v2(unsigned int irq)
{
	int bit = irq - OCTEON_IRQ_GPIO0;
	cvmx_write_csr(CVMX_GPIO_BIT_CFGX(bit), 0);

	octeon_irq_ciu_disable_all_v2(irq);
}

static void octeon_irq_ciu_disable_gpio(unsigned int irq)
{
	int bit = irq - OCTEON_IRQ_GPIO0;
	cvmx_write_csr(CVMX_GPIO_BIT_CFGX(bit), 0);

	octeon_irq_ciu_disable(irq);
}

static void octeon_irq_ciu_gpio_ack(unsigned int irq)
{
	int bit = irq - OCTEON_IRQ_GPIO0;
	u64 mask = 1ull << bit;

	cvmx_write_csr(CVMX_GPIO_INT_CLR, mask);
}

static struct irq_chip octeon_irq_chip_ciu_gpio_v2 = {
	.name = "CIU-GPIO",
	.enable = octeon_irq_ciu_enable_gpio_v2,
	.disable = octeon_irq_ciu_disable_gpio_v2,
	.ack = octeon_irq_ciu_gpio_ack,
	.mask = octeon_irq_ciu_disable_local_v2,
	.unmask = octeon_irq_ciu_enable_v2,
	.set_type = octeon_irq_ciu_gpio_set_type,
#ifdef CONFIG_SMP
	.set_affinity = octeon_irq_ciu_set_affinity_v2,
#endif
};

static struct irq_chip octeon_irq_chip_ciu_gpio = {
	.name = "CIU-GPIO",
	.enable = octeon_irq_ciu_enable_gpio,
	.disable = octeon_irq_ciu_disable_gpio,
	.mask = octeon_irq_dummy_mask,
	.ack = octeon_irq_ciu_gpio_ack,
	.set_type = octeon_irq_ciu_gpio_set_type,
#ifdef CONFIG_SMP
	.set_affinity = octeon_irq_ciu_set_affinity,
#endif
};

static void octeon_irq_ip2_v1(void)
{
	const unsigned long core_id = cvmx_get_core_num();
	u64 ciu_sum = cvmx_read_csr(CVMX_CIU_INTX_SUM0(core_id * 2));

	ciu_sum &= __get_cpu_var(octeon_irq_ciu0_en_mirror);
	clear_c0_status(STATUSF_IP2);
	if (likely(ciu_sum)) {
		int bit = fls64(ciu_sum) - 1;
		int irq = octeon_irq_ciu_to_irq[0][bit];
		if (likely(irq))
			do_IRQ(irq);
		else
			spurious_interrupt();
	} else {
		spurious_interrupt();
	}
	set_c0_status(STATUSF_IP2);
}

static void octeon_irq_ip2_v2(void)
{
	const unsigned long core_id = cvmx_get_core_num();
	u64 ciu_sum = cvmx_read_csr(CVMX_CIU_INTX_SUM0(core_id * 2));

	ciu_sum &= __get_cpu_var(octeon_irq_ciu0_en_mirror);
	if (likely(ciu_sum)) {
		int bit = fls64(ciu_sum) - 1;
		int irq = octeon_irq_ciu_to_irq[0][bit];
		if (likely(irq))
			do_IRQ(irq);
		else
			spurious_interrupt();
	} else {
		spurious_interrupt();
	}
}
static void octeon_irq_ip3_v1(void)
{
	u64 ciu_sum = cvmx_read_csr(CVMX_CIU_INT_SUM1);

	ciu_sum &= __get_cpu_var(octeon_irq_ciu1_en_mirror);
	clear_c0_status(STATUSF_IP3);
	if (likely(ciu_sum)) {
		int bit = fls64(ciu_sum) - 1;
		int irq = octeon_irq_ciu_to_irq[1][bit];
		if (likely(irq))
			do_IRQ(irq);
		else
			spurious_interrupt();
	} else {
		spurious_interrupt();
	}
	set_c0_status(STATUSF_IP3);
}

static void octeon_irq_ip3_v2(void)
{
	u64 ciu_sum = cvmx_read_csr(CVMX_CIU_INT_SUM1);

	ciu_sum &= __get_cpu_var(octeon_irq_ciu1_en_mirror);
	if (likely(ciu_sum)) {
		int bit = fls64(ciu_sum) - 1;
		int irq = octeon_irq_ciu_to_irq[1][bit];
		if (likely(irq))
			do_IRQ(irq);
		else
			spurious_interrupt();
	} else {
		spurious_interrupt();
	}
}

static bool octeon_irq_use_ip4;

static void __cpuinit octeon_irq_local_enable_ip4(void *arg)
{
	set_c0_status(STATUSF_IP4);
}

static void octeon_irq_ip4_mask(void)
{
	clear_c0_status(STATUSF_IP4);
	spurious_interrupt();
}

static void (*octeon_irq_ip2)(void);
static void (*octeon_irq_ip3)(void);
static void (*octeon_irq_ip4)(void);

void (*octeon_irq_setup_secondary)(void);

void __cpuinit octeon_irq_set_ip4_handler(octeon_irq_ip4_handler_t h)
{
	octeon_irq_ip4 = h;
	octeon_irq_use_ip4 = true;
	on_each_cpu(octeon_irq_local_enable_ip4, NULL, 1);
}

static void octeon_irq_percpu_enable(void)
{
	int i;

	/* Enable the percpu interrupts on this core */
	for (i = OCTEON_IRQ_SW0; i < OCTEON_IRQ_LAST; i++) {
		struct irq_desc *desc = irq_to_desc(i);
		if (desc && desc->handle_irq ==  handle_percpu_irq &&
			desc->chip && desc->chip->enable) {
			unsigned long flags;
			spin_lock_irqsave(&desc->lock, flags);
			if ((desc->status & IRQ_DISABLED) == 0)
				desc->chip->enable(i);
			spin_unlock_irqrestore(&desc->lock, flags);
		}
	}

}

static void octeon_irq_init_ciu_percpu(void)
{
	int coreid = cvmx_get_core_num();
	/*
	 * Disable All CIU Interrupts. The ones we need will be
	 * enabled later.  Read the SUM register so we know the write
	 * completed.
	 */
	cvmx_write_csr(CVMX_CIU_INTX_EN0((coreid * 2)), 0);
	cvmx_write_csr(CVMX_CIU_INTX_EN0((coreid * 2 + 1)), 0);
	cvmx_write_csr(CVMX_CIU_INTX_EN1((coreid * 2)), 0);
	cvmx_write_csr(CVMX_CIU_INTX_EN1((coreid * 2 + 1)), 0);
	cvmx_read_csr(CVMX_CIU_INTX_SUM0((coreid * 2)));
}

static void octeon_irq_init_ciu2_percpu(void)
{
	u64 regx, ipx;
	int coreid = cvmx_get_core_num();
	u64 base = CVMX_CIU2_EN_PPX_IP2_WRKQ(coreid);

	/*
	 * Disable All CIU2 Interrupts. The ones we need will be
	 * enabled later.  Read the SUM register so we know the write
	 * completed.
	 *
	 * There are 9 registers and 3 IPX levels with strides 0x1000
	 * and 0x200 respectivly.  Use loops to clear them.
	 */
	for (regx = 0; regx <= 0x8000; regx += 0x1000) {
		for (ipx = 0; ipx <= 0x400; ipx += 0x200)
			cvmx_write_csr(base + regx + ipx, 0);
	}

	cvmx_read_csr(CVMX_CIU2_SUM_PPX_IP2(coreid));
}

static void octeon_irq_setup_secondary_ciu(void)
{

	__get_cpu_var(octeon_irq_ciu0_en_mirror) = 0;
	__get_cpu_var(octeon_irq_ciu1_en_mirror) = 0;

	octeon_irq_init_ciu_percpu();
	octeon_irq_percpu_enable();

	/* Enable the CIU lines */
	set_c0_status(STATUSF_IP3 | STATUSF_IP2);
	if (octeon_irq_use_ip4)
		set_c0_status(STATUSF_IP4);
	else
		clear_c0_status(STATUSF_IP4);
}

static void octeon_irq_setup_secondary_ciu2(void)
{
	octeon_irq_init_ciu2_percpu();
	octeon_irq_percpu_enable();

	/* Enable the CIU lines */
	set_c0_status(STATUSF_IP3 | STATUSF_IP2);
	if (octeon_irq_use_ip4)
		set_c0_status(STATUSF_IP4);
	else
		clear_c0_status(STATUSF_IP4);
}

static void __init octeon_irq_init_ciu(void)
{
	unsigned int i;
	struct irq_chip *chip;
	struct irq_chip *chip_edge;
	struct irq_chip *chip_mbox;
	struct irq_chip *chip_wd;
	struct irq_chip *chip_gpio;

	octeon_irq_init_ciu_percpu();
	octeon_irq_setup_secondary = octeon_irq_setup_secondary_ciu;

	if (OCTEON_IS_MODEL(OCTEON_CN58XX_PASS2_X) ||
	    OCTEON_IS_MODEL(OCTEON_CN56XX_PASS2_X) ||
	    OCTEON_IS_MODEL(OCTEON_CN52XX_PASS2_X) ||
	    OCTEON_IS_MODEL(OCTEON_CN6XXX)) {
		octeon_irq_ip2 = octeon_irq_ip2_v2;
		octeon_irq_ip3 = octeon_irq_ip3_v2;
		chip = &octeon_irq_chip_ciu_v2;
		chip_edge = &octeon_irq_chip_ciu_edge_v2;
		chip_mbox = &octeon_irq_chip_ciu_mbox_v2;
		chip_wd = &octeon_irq_chip_ciu_wd_v2;
		chip_gpio = &octeon_irq_chip_ciu_gpio_v2;
	} else {
		octeon_irq_ip2 = octeon_irq_ip2_v1;
		octeon_irq_ip3 = octeon_irq_ip3_v1;
		chip = &octeon_irq_chip_ciu;
		chip_edge = &octeon_irq_chip_ciu_edge;
		chip_mbox = &octeon_irq_chip_ciu_mbox;
		chip_wd = &octeon_irq_chip_ciu_wd;
		chip_gpio = &octeon_irq_chip_ciu_gpio;
	}
	octeon_irq_ip4 = octeon_irq_ip4_mask;

	/* Mips internal */
	set_irq_chip_and_handler(OCTEON_IRQ_SW0, &octeon_irq_chip_core, handle_percpu_irq);
	set_irq_chip_and_handler(OCTEON_IRQ_SW1, &octeon_irq_chip_core, handle_percpu_irq);
	set_irq_chip_and_handler(OCTEON_IRQ_5, &octeon_irq_chip_core, handle_percpu_irq);
	set_irq_chip_and_handler(OCTEON_IRQ_PERF, &octeon_irq_chip_core, handle_percpu_irq);
	set_irq_chip_and_handler(OCTEON_IRQ_TIMER, &octeon_irq_chip_core, handle_percpu_irq);

	/* CIU_0 */
	for (i = 0; i < 16; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_WORKQ0, 0, i + 0, chip, handle_level_irq);
	for (i = 0; i < 16; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_GPIO0, 0, i + 16, chip_gpio, handle_level_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_MBOX0, 0, 32, chip_mbox, handle_percpu_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_MBOX1, 0, 33, chip_mbox, handle_percpu_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_UART0, 0, 34, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_UART1, 0, 35, chip, handle_level_irq);

	for (i = 0; i < 4; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_PCI_INT0, 0, i + 36, chip, handle_level_irq);
	for (i = 0; i < 4; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_PCI_MSI0, 0, i + 40, chip, handle_level_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_TWSI, 0, 45, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_RML, 0, 46, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_TRACE0, 0, 47, chip, handle_level_irq);

	for (i = 0; i < 2; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_GMX_DRP0, 0, i + 48, chip_edge, handle_edge_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_IPD_DRP, 0, 50, chip_edge, handle_edge_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_KEY_ZERO, 0, 51, chip_edge, handle_edge_irq);

	for (i = 0; i < 4; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_TIMER0, 0, i + 52, chip_edge, handle_edge_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_USB0, 0, 56, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_PCM, 0, 57, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_MPI, 0, 58, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_TWSI2, 0, 59, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_POWIQ, 0, 60, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_IPDPPTHR, 0, 61, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_MII0, 0, 62, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_BOOTDMA, 0, 63, chip, handle_level_irq);

	/* CIU_1 */
	for (i = 0; i < 16; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_WDOG0, 1, i + 0, chip_wd, handle_level_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_UART2, 1, 16, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_USB1, 1, 17, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_MII1, 1, 18, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_NAND, 1, 19, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_MIO, 1, 20, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_IOB, 1, 21, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_FPA, 1, 22, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_POW, 1, 23, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_L2C, 1, 24, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_IPD, 1, 25, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_PIP, 1, 26, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_PKO, 1, 27, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_ZIP, 1, 28, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_TIM, 1, 29, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_RAD, 1, 30, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_KEY, 1, 31, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_DFA, 1, 32, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_USBCTL, 1, 33, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_SLI, 1, 34, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_DPI, 1, 35, chip, handle_level_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_AGX0, 1, 36, chip, handle_level_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_AGL, 1, 46, chip, handle_level_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_PTP, 1, 47, chip_edge, handle_edge_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_PEM0, 1, 48, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_PEM1, 1, 49, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_SRIO0, 1, 50, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_SRIO1, 1, 51, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_LMC0, 1, 52, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_DFM, 1, 56, chip, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_RST, 1, 63, chip, handle_level_irq);

	/* Enable the CIU lines */
	set_c0_status(STATUSF_IP3 | STATUSF_IP2);
	clear_c0_status(STATUSF_IP4);
}

/*
 * Watchdog interrupts are special.  They are associated with a single
 * core, so we hardwire the affinity to that core.
 */
static void octeon_irq_ciu2_wd_enable(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	u64 mask;
	u64 en_addr;

	int coreid = irq - OCTEON_IRQ_WDOG0;
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;
	mask = 1ull << (cd.s.bit);

	en_addr = CVMX_CIU2_EN_PPX_IP2_WRKQ_W1S(coreid) + (0x1000ull * cd.s.line);
	cvmx_write_csr(en_addr, mask);

}

static void octeon_irq_ciu2_enable(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	u64 mask;
	u64 en_addr;
	int cpu = next_cpu_for_irq(desc);
	int coreid = octeon_coreid_for_cpu(cpu);
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;
	mask = 1ull << (cd.s.bit);

	en_addr = CVMX_CIU2_EN_PPX_IP2_WRKQ_W1S(coreid) + (0x1000ull * cd.s.line);
	cvmx_write_csr(en_addr, mask);

}

static void octeon_irq_ciu2_enable_check(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	if ((desc->status & IRQ_DISABLED) == 0)
		octeon_irq_ciu2_enable(irq);
}

static void octeon_irq_ciu2_enable_local(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	u64 mask;
	u64 en_addr;
	int coreid = cvmx_get_core_num();
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;
	mask = 1ull << (cd.s.bit);

	en_addr = CVMX_CIU2_EN_PPX_IP2_WRKQ_W1S(coreid) + (0x1000ull * cd.s.line);
	cvmx_write_csr(en_addr, mask);

}

static void octeon_irq_ciu2_enable_local_check(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	if ((desc->status & IRQ_DISABLED) == 0)
		octeon_irq_ciu2_enable_local(irq);
}

static void octeon_irq_ciu2_disable_local(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	u64 mask;
	u64 en_addr;
	int coreid = cvmx_get_core_num();
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;
	mask = 1ull << (cd.s.bit);

	en_addr = CVMX_CIU2_EN_PPX_IP2_WRKQ_W1C(coreid) + (0x1000ull * cd.s.line);
	cvmx_write_csr(en_addr, mask);

}

static void octeon_irq_ciu2_ack(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	u64 mask;
	u64 en_addr;
	int coreid = cvmx_get_core_num();
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;
	mask = 1ull << (cd.s.bit);

	en_addr = CVMX_CIU2_RAW_PPX_IP2_WRKQ(coreid) + (0x1000ull * cd.s.line);
	cvmx_write_csr(en_addr, mask);

}

static void octeon_irq_ciu2_disable_all(unsigned int irq)
{
	int cpu;
	struct irq_desc *desc = irq_to_desc(irq);
	u64 mask;
	union octeon_ciu_chip_data cd;

	cd.p = desc->chip_data;
	mask = 1ull << (cd.s.bit);

	for_each_online_cpu(cpu) {
		u64 en_addr = CVMX_CIU2_EN_PPX_IP2_WRKQ_W1C(octeon_coreid_for_cpu(cpu)) + (0x1000ull * cd.s.line);
		cvmx_write_csr(en_addr, mask);
	}
}

static void octeon_irq_ciu2_mbox_disable_all(unsigned int irq)
{
	int cpu;
	u64 mask;

	mask = 1ull << (irq - OCTEON_IRQ_MBOX0);

	for_each_online_cpu(cpu) {
		u64 en_addr = CVMX_CIU2_EN_PPX_IP3_MBOX_W1C(octeon_coreid_for_cpu(cpu));
		cvmx_write_csr(en_addr, mask);
	}
}

static void octeon_irq_ciu2_mbox_enable_local(unsigned int irq)
{
	u64 mask;
	u64 en_addr;
	int coreid = cvmx_get_core_num();

	mask = 1ull << (irq - OCTEON_IRQ_MBOX0);
	en_addr = CVMX_CIU2_EN_PPX_IP3_MBOX_W1S(coreid);
	cvmx_write_csr(en_addr, mask);
}

static void octeon_irq_ciu2_mbox_enable_local_check(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);
	if ((desc->status & IRQ_DISABLED) == 0)
		octeon_irq_ciu2_mbox_enable_local(irq);
}

static void octeon_irq_ciu2_mbox_disable_local(unsigned int irq)
{
	u64 mask;
	u64 en_addr;
	int coreid = cvmx_get_core_num();

	mask = 1ull << (irq - OCTEON_IRQ_MBOX0);
	en_addr = CVMX_CIU2_EN_PPX_IP3_MBOX_W1C(coreid);
	cvmx_write_csr(en_addr, mask);
}

#ifdef CONFIG_SMP
static int octeon_irq_ciu2_set_affinity(unsigned int irq,
					const struct cpumask *dest)
{
	int cpu;
	struct irq_desc *desc = irq_to_desc(irq);
	int enable_one = (desc->status & IRQ_DISABLED) == 0;
	u64 mask;
	union octeon_ciu_chip_data cd;

	if (desc->status & IRQ_DISABLED)
		return 0;

	cd.p = desc->chip_data;
	mask = 1ull << cd.s.bit;

	for_each_online_cpu(cpu) {
		u64 en_addr;
		if (cpumask_test_cpu(cpu, dest) && enable_one) {
			enable_one = 0;
			en_addr = CVMX_CIU2_EN_PPX_IP2_WRKQ_W1S(octeon_coreid_for_cpu(cpu)) + (0x1000ull * cd.s.line);
		} else {
			en_addr = CVMX_CIU2_EN_PPX_IP2_WRKQ_W1C(octeon_coreid_for_cpu(cpu)) + (0x1000ull * cd.s.line);
		}
		cvmx_write_csr(en_addr, mask);
	}

	return 0;
}
#endif

static void octeon_irq_ciu2_enable_gpio(unsigned int irq)
{
	octeon_irq_gpio_setup(irq);
	octeon_irq_ciu2_enable(irq);
}

static void octeon_irq_ciu2_disable_gpio(unsigned int irq)
{
	int bit = irq - OCTEON_IRQ_GPIO0;
	cvmx_write_csr(CVMX_GPIO_BIT_CFGX(bit), 0);

	octeon_irq_ciu2_disable_all(irq);
}

static struct irq_chip octeon_irq_chip_ciu2 = {
	.name = "CIU2",
	.enable = octeon_irq_ciu2_enable,
	.disable = octeon_irq_ciu2_disable_all,
	.mask = octeon_irq_ciu2_disable_local,
	.unmask = octeon_irq_ciu2_enable_check,
#ifdef CONFIG_SMP
	.set_affinity = octeon_irq_ciu2_set_affinity,
#endif
};

static struct irq_chip octeon_irq_chip_ciu2_edge = {
	.name = "CIU2-E",
	.enable = octeon_irq_ciu2_enable,
	.disable = octeon_irq_ciu2_disable_all,
	.ack = octeon_irq_ciu2_ack,
	.mask = octeon_irq_ciu2_disable_local,
	.unmask = octeon_irq_ciu2_enable_check,
#ifdef CONFIG_SMP
	.set_affinity = octeon_irq_ciu2_set_affinity,
#endif
};

static struct irq_chip octeon_irq_chip_ciu2_mbox = {
	.name = "CIU2-M",
	.enable = octeon_irq_ciu2_mbox_enable_local,
	.disable = octeon_irq_ciu2_mbox_disable_all,
	.ack = octeon_irq_ciu2_mbox_disable_local,
	.eoi = octeon_irq_ciu2_mbox_enable_local_check,
};

static struct irq_chip octeon_irq_chip_ciu2_wd = {
	.name = "CIU2-W",
	.enable = octeon_irq_ciu2_wd_enable,
	.disable = octeon_irq_ciu2_disable_all,
	.mask = octeon_irq_ciu2_disable_local,
	.unmask = octeon_irq_ciu2_enable_local_check,
};

static struct irq_chip octeon_irq_chip_ciu2_gpio = {
	.name = "CIU-GPIO",
	.enable = octeon_irq_ciu2_enable_gpio,
	.disable = octeon_irq_ciu2_disable_gpio,
	.ack = octeon_irq_ciu_gpio_ack,
	.mask = octeon_irq_ciu2_disable_local,
	.unmask = octeon_irq_ciu2_enable_check,
	.set_type = octeon_irq_ciu_gpio_set_type,
#ifdef CONFIG_SMP
	.set_affinity = octeon_irq_ciu2_set_affinity,
#endif
};

static void octeon_irq_ciu2(void)
{
	int line;
	int bit;
	int irq;
	u64 src_reg, src, sum;
	const unsigned long core_id = cvmx_get_core_num();

	sum = cvmx_read_csr(CVMX_CIU2_SUM_PPX_IP2(core_id)) & 0xfful;

	if (unlikely(!sum))
		goto spurious;

	line = fls64(sum) - 1;
	src_reg = CVMX_CIU2_SRC_PPX_IP2_WRKQ(core_id) + (0x1000 * line);
	src = cvmx_read_csr(src_reg);

	if (unlikely(!src))
		goto spurious;

	bit = fls64(src) - 1;
	irq = octeon_irq_ciu_to_irq[line][bit];
	if (unlikely(!irq))
		goto spurious;

	do_IRQ(irq);
	goto out;

spurious:
	spurious_interrupt();
out:
	/* CN68XX pass 1.x has an errata that accessing the ACK registers
		can stop interrupts from propagating */
	if (OCTEON_IS_MODEL(OCTEON_CN68XX_PASS1_X))
		cvmx_read_csr(CVMX_CIU2_INTR_CIU_READY);
	else
		cvmx_read_csr(CVMX_CIU2_ACK_PPX_IP2(core_id));
	return;
}

static void octeon_irq_ciu2_mbox(void)
{
	int line;

	const unsigned long core_id = cvmx_get_core_num();
	u64 sum = cvmx_read_csr(CVMX_CIU2_SUM_PPX_IP3(core_id)) >> 60;

	if (unlikely(!sum))
		goto spurious;

	line = fls64(sum) - 1;

	do_IRQ(OCTEON_IRQ_MBOX0 + line);
	goto out;

spurious:
	spurious_interrupt();
out:
	/* CN68XX pass 1.x has an errata that accessing the ACK registers
		can stop interrupts from propagating */
	if (OCTEON_IS_MODEL(OCTEON_CN68XX_PASS1_X))
		cvmx_read_csr(CVMX_CIU2_INTR_CIU_READY);
	else
		cvmx_read_csr(CVMX_CIU2_ACK_PPX_IP3(core_id));
	return;
}

static void __init octeon_irq_init_ciu2(void)
{
	unsigned int i;

	octeon_irq_init_ciu2_percpu();
	octeon_irq_setup_secondary = octeon_irq_setup_secondary_ciu2;

	octeon_irq_ip2 = octeon_irq_ciu2;
	octeon_irq_ip3 = octeon_irq_ciu2_mbox;
	octeon_irq_ip4 = octeon_irq_ip4_mask;

	/* Mips internal */
	set_irq_chip_and_handler(OCTEON_IRQ_SW0, &octeon_irq_chip_core, handle_percpu_irq);
	set_irq_chip_and_handler(OCTEON_IRQ_SW1, &octeon_irq_chip_core, handle_percpu_irq);
	set_irq_chip_and_handler(OCTEON_IRQ_5, &octeon_irq_chip_core, handle_percpu_irq);
	set_irq_chip_and_handler(OCTEON_IRQ_PERF, &octeon_irq_chip_core, handle_percpu_irq);
	set_irq_chip_and_handler(OCTEON_IRQ_TIMER, &octeon_irq_chip_core, handle_percpu_irq);

	/* CUI2 */
	for (i = 0; i < 64; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_WORKQ0, 0, i, &octeon_irq_chip_ciu2, handle_level_irq);

	for (i = 0; i < 32; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_WDOG0, 1, i , &octeon_irq_chip_ciu2_wd, handle_level_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_IOB, 2, 0, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_FPA, 2, 4, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_IPD, 2, 5, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_PIP, 2, 6, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_PKO, 2, 7, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_POW, 2, 16, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_ZIP, 2, 24, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_TIM, 2, 28, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_RAD, 2, 29, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_KEY, 2, 30, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_SLI, 2, 32, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_DPI, 2, 33, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_DFA, 2, 40, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_L2C, 2, 48, &octeon_irq_chip_ciu2, handle_level_irq);
	for (i = 0; i < 4; i++)
		octeon_irq_set_ciu_mapping(OCTEON_IRQ_TRACE0 + i, 2, i + 52, &octeon_irq_chip_ciu2, handle_level_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_IPDPPTHR, 3, 0, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_IPD_DRP, 3, 2, &octeon_irq_chip_ciu2_edge, handle_edge_irq);
	for (i = 0; i < 4; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_TIMER0, 3, i + 8, &octeon_irq_chip_ciu2_edge, handle_edge_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_NAND, 3, 16, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_MIO, 3, 17, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_BOOTDMA, 3, 18, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_TWSI, 3, 32, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_TWSI2, 3, 33, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_UART0, 3, 36, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_UART1, 3, 37, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_USBCTL, 3, 40, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_USB0, 3, 44, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_PTP, 3, 48, &octeon_irq_chip_ciu2_edge, handle_edge_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_RST, 3, 63, &octeon_irq_chip_ciu2, handle_level_irq);

	for (i = 0; i < 4; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_PCI_INT0, 4, i, &octeon_irq_chip_ciu2, handle_level_irq);

	if (OCTEON_IS_MODEL(OCTEON_CN68XX_PASS1_X))
		for (i = 0; i < 4; i++)
			octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_PCI_MSI0, 4, i + 8, &octeon_irq_chip_ciu2, handle_level_irq);

	octeon_irq_set_ciu_mapping(OCTEON_IRQ_PEM0, 4, 32, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_PEM1, 4, 33, &octeon_irq_chip_ciu2, handle_level_irq);

	for (i = 0; i < 4; i++)
		octeon_irq_set_ciu_mapping(OCTEON_IRQ_LMC0, 5, i, &octeon_irq_chip_ciu2, handle_level_irq);

	for (i = 0; i < 5; i++)
		octeon_irq_set_ciu_mapping(OCTEON_IRQ_AGX0 + i, 6, i, &octeon_irq_chip_ciu2, handle_level_irq);
	for (i = 0; i < 5; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_GMX_DRP0 + i, 6, i + 8, &octeon_irq_chip_ciu2_edge, handle_edge_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_AGL, 6, 32, &octeon_irq_chip_ciu2, handle_level_irq);
	octeon_irq_set_ciu_mapping(OCTEON_IRQ_MII0, 6, 40, &octeon_irq_chip_ciu2, handle_level_irq);

	for (i = 0; i < 16; i++)
		octeon_irq_set_ciu_mapping(i + OCTEON_IRQ_GPIO0, 7, i, &octeon_irq_chip_ciu2_gpio, handle_level_irq);

	set_irq_chip_and_handler(OCTEON_IRQ_MBOX0, &octeon_irq_chip_ciu2_mbox, handle_percpu_irq);
	set_irq_chip_and_handler(OCTEON_IRQ_MBOX1, &octeon_irq_chip_ciu2_mbox, handle_percpu_irq);
	set_irq_chip_and_handler(OCTEON_IRQ_MBOX2, &octeon_irq_chip_ciu2_mbox, handle_percpu_irq);
	set_irq_chip_and_handler(OCTEON_IRQ_MBOX3, &octeon_irq_chip_ciu2_mbox, handle_percpu_irq);

	/* Enable the CIU lines */
	set_c0_status(STATUSF_IP3 | STATUSF_IP2);
	clear_c0_status(STATUSF_IP4);
}

void __init arch_init_irq(void)
{
#ifdef CONFIG_SMP
	/* Set the default affinity to the boot cpu. */
	cpumask_clear(irq_default_affinity);
	cpumask_set_cpu(smp_processor_id(), irq_default_affinity);
#endif
	if (OCTEON_IS_MODEL(OCTEON_CN68XX))
		octeon_irq_init_ciu2();
	else
		octeon_irq_init_ciu();
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned long cop0_cause;
	unsigned long cop0_status;

	while (1) {
		cop0_cause = read_c0_cause();
		cop0_status = read_c0_status();
		cop0_cause &= cop0_status;
		cop0_cause &= ST0_IM;

		if (unlikely(cop0_cause & STATUSF_IP2))
			octeon_irq_ip2();
		else if (unlikely(cop0_cause & STATUSF_IP3))
			octeon_irq_ip3();
		else if (unlikely(cop0_cause & STATUSF_IP4))
			octeon_irq_ip4();
		else if (likely(cop0_cause))
			do_IRQ(fls(cop0_cause) - 9 + MIPS_CPU_IRQ_BASE);
		else
			break;
	}
}

#ifdef CONFIG_HOTPLUG_CPU

void fixup_irqs(void)
{
	int irq;
	struct irq_desc *desc;
	cpumask_t new_affinity;
	unsigned long flags;
	int do_set_affinity;
	int cpu;

	cpu = smp_processor_id();

	for (irq = OCTEON_IRQ_SW0; irq <= OCTEON_IRQ_TIMER; irq++)
		octeon_irq_core_disable_local(irq);

	for (irq = OCTEON_IRQ_WORKQ0; irq < OCTEON_IRQ_LAST; irq++) {
		desc = irq_to_desc(irq);
		if (desc->status & IRQ_DISABLED)
			continue;

		switch (irq) {
		case OCTEON_IRQ_MBOX0...OCTEON_IRQ_MBOX3:
			/* The eoi function will disable them on this CPU. */
			desc->chip->eoi(irq);
			break;
		case OCTEON_IRQ_WDOG0...OCTEON_IRQ_WDOG31:
			/*
			 * These have special per CPU semantics and
			 * are handled in the watchdog driver.
			 */
			break;
		default:
			spin_lock_irqsave(&desc->lock, flags);
			/*
			 * If this irq has an action, it is in use and
			 * must be migrated if it has affinity to this
			 * cpu.
			 */
			if (desc->action && cpumask_test_cpu(cpu, desc->affinity)) {
				if (cpumask_weight(desc->affinity) > 1) {
					/*
					 * It has multi CPU affinity,
					 * just remove this CPU from
					 * the affinity set.
					 */
					cpumask_copy(&new_affinity, desc->affinity);
					cpumask_clear_cpu(cpu, &new_affinity);
				} else {
					/*
					 * Otherwise, put it on lowest
					 * numbered online CPU.
					 */
					cpumask_clear(&new_affinity);
					cpumask_set_cpu(cpumask_first(cpu_online_mask), &new_affinity);
				}
				do_set_affinity = 1;
			} else {
				do_set_affinity = 0;
			}
			spin_unlock_irqrestore(&desc->lock, flags);

			if (do_set_affinity)
				irq_set_affinity(irq, &new_affinity);

			break;
		}
	}
}

#endif /* CONFIG_HOTPLUG_CPU */
