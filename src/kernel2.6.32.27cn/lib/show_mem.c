/*
 * Generic show_mem() implementation
 *
 * Copyright (C) 2008 Johannes Weiner <hannes@saeurebad.de>
 * All code subject to the GPL version 2.
 */

#include <linux/mm.h>
#include <linux/nmi.h>
#include <linux/quicklist.h>

extern int (*kes_mem_print_handle)(const char * fmt, ...);
extern int (*kes_mem_dump_handle)(char *buff, int size);
extern void (*print_current_time_handle)(void);

void show_mem(void)
{
	pg_data_t *pgdat;
	unsigned long total = 0, reserved = 0, shared = 0,
		nonshared = 0, highmem = 0;

	printk(KERN_INFO "Mem-Info:\n");
	if (kes_mem_print_handle && print_current_time_handle) {
		print_current_time_handle();
		kes_mem_print_handle("Mem-Info:\n");
	}
	show_free_areas();

	for_each_online_pgdat(pgdat) {
		unsigned long i, flags;

		pgdat_resize_lock(pgdat, &flags);
		for (i = 0; i < pgdat->node_spanned_pages; i++) {
			struct page *page;
			unsigned long pfn = pgdat->node_start_pfn + i;

			if (unlikely(!(i % MAX_ORDER_NR_PAGES)))
				touch_nmi_watchdog();

			if (!pfn_valid(pfn))
				continue;

			page = pfn_to_page(pfn);

			if (PageHighMem(page))
				highmem++;

			if (PageReserved(page))
				reserved++;
			else if (page_count(page) == 1)
				nonshared++;
			else if (page_count(page) > 1)
				shared += page_count(page) - 1;

			total++;
		}
		pgdat_resize_unlock(pgdat, &flags);
	}

	printk(KERN_INFO "%lu pages RAM\n", total);
	if (kes_mem_print_handle) {
		kes_mem_print_handle("%lu pages RAM\n", total);
	}
#ifdef CONFIG_HIGHMEM
	printk(KERN_INFO "%lu pages HighMem\n", highmem);
	if (kes_mem_print_handle) {
		kes_mem_print_handle("%lu pages HighMem\n", highmem);
	}
#endif
	printk(KERN_INFO "%lu pages reserved\n", reserved);
	printk(KERN_INFO "%lu pages shared\n", shared);
	printk(KERN_INFO "%lu pages non-shared\n", nonshared);
	if (kes_mem_print_handle) {
		kes_mem_print_handle("%lu pages reserved\n", reserved);
		kes_mem_print_handle("%lu pages shared\n", shared);
		kes_mem_print_handle("%lu pages non-shared\n", nonshared);
	}
#ifdef CONFIG_QUICKLIST
	printk(KERN_INFO "%lu pages in pagetable cache\n",
		quicklist_total_size());
	if (kes_mem_print_handle) {
		kes_mem_print_handle("%lu pages in pagetable cache\n",
							quicklist_total_size());
	}
#endif
}
