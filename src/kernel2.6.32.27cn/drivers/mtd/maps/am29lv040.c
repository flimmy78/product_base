/*
 * sst39vf040 AM29LV040B, MTD map driver for SST39Lf040 chip
 *
 * physical Adress 1fc00000
 *
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>

#include <asm/octeon/octeon.h>


#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/partitions.h>
#endif

#define WINDOW_ADDR 0x1fc00000      /* physical properties of flash */
//#define WINDOW_ADDR 0x00000000      /* physical properties of flash */
#define WINDOW_SIZE 0x80000
#define BUSWIDTH    1

#define FLASH_BLOCKSIZE_MAIN	0x10000 /* 64K */
#define FLASH_NUMBLOCKS_MAIN	8

/* can be "cfi_probe", "jedec_probe", "map_rom", NULL }; */
#define PROBETYPES {  "cfi_probe","jedec_probe", NULL }

#define MSG_PREFIX "flash-NOR:"   /* prefix for our printk()'s */
#define MTDID      "FLASH-nor"    /* for mtd:parts= partitioning */

static struct mtd_info *mymtd;

struct map_info flashnor_map = {
	.name = "flash",
	.size = WINDOW_SIZE,
	.bankwidth = BUSWIDTH,
	.phys = WINDOW_ADDR,
};

#ifdef CONFIG_MTD_PARTITIONS   /*make menucondif option choose*/

/*
 * MTD partitioning stuff, SST39LF040  partitioning
 */

static struct mtd_partition static_partitions[ ] =
{
	{
		.name = "u-boot",
		.size = 0x70000,
		.offset = 0x00,
		.mask_flags = 0,
	},
	{
		.name = "ENV",
		.size = 0x10000,
		.offset = 0x70000,
	}
};


//static const char *probes[] = { "RedBoot", "cmdlinepart", NULL };


static const char *probes[] = { NULL };

#endif

static int                   mtd_parts_nb = 0;
static struct mtd_partition *mtd_parts    = 0;

static int __init init_flashnor(void)
{
	static const char *rom_probe_types[] = PROBETYPES;
	const char **type;
	const char *part_type = 0;

	/*
	 * Read the bootbus region 0 setup to determine the base
	 * address of the flash.
	*/

	union cvmx_mio_boot_reg_cfgx region_cfg;
	region_cfg.u64 = cvmx_read_csr(CVMX_MIO_BOOT_REG_CFGX(0));
	if (region_cfg.s.en) {

       	printk(KERN_NOTICE MSG_PREFIX "0x%08x at 0x%08x\n",
	       WINDOW_SIZE, WINDOW_ADDR);
	flashnor_map.virt = ioremap(WINDOW_ADDR, WINDOW_SIZE);
//	printk("iormap addrs ---------->0x%16x\n",SST39LF040nor_map.virt);
	if (!flashnor_map.virt) {
		printk(MSG_PREFIX "failed to ioremap\n");
		return -EIO;
	}

	simple_map_init(&flashnor_map);

	mymtd = 0;
	type = rom_probe_types;
	for(; !mymtd && *type; type++) {
		mymtd = do_map_probe(*type, &flashnor_map);
//		mymtd = do_map_probe("jedec_probe", &SST39LF040nor_map);
	}
	if (mymtd) {
		mymtd->owner = THIS_MODULE;
		mymtd->erasesize = FLASH_BLOCKSIZE_MAIN;
	if(!mymtd){
		iounmap((void *)flashnor_map.virt);
		printk("failde to proble\n");
	}
#ifdef CONFIG_MTD_PARTITIONS
//		mtd_parts_nb = parse_mtd_partitions(mymtd, probes, &mtd_parts, MTDID);
	       	mtd_parts_nb = parse_mtd_partitions(mymtd, probes, &mtd_parts, 0);
		if (mtd_parts_nb > 0)
		  part_type = "detected";

		if (mtd_parts_nb == 0)
		{
			mtd_parts = static_partitions;
			mtd_parts_nb = ARRAY_SIZE(static_partitions);
			part_type = "static";
		}
	
#endif
		add_mtd_device(mymtd);
		if (mtd_parts_nb == 0)
		  printk(KERN_NOTICE MSG_PREFIX "no partition info available\n");
		else
		{
			printk(KERN_NOTICE MSG_PREFIX
			       "using %s partition definition\n", part_type);
			add_mtd_partitions(mymtd, mtd_parts, mtd_parts_nb);
		}
		return 0;
	}

		}

	iounmap((void *)flashnor_map.virt);
	return -ENXIO;
 }

static void __exit cleanup_flashnor(void)
{
	if (mymtd) {
		del_mtd_device(mymtd);
		map_destroy(mymtd);
	}
	if (flashnor_map.virt) {
		iounmap((void *)flashnor_map.virt);
		flashnor_map.virt = 0;
	}
}

module_init(init_flashnor);
module_exit(cleanup_flashnor);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AUTELAN");
MODULE_DESCRIPTION("Generic configurable MTD map driver");
