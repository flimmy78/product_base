#include <linux/kernel.h>

#define _NO_VERSION_

#include <linux/module.h>

#include <linux/version.h>

#if CONFIG_MODEREVISION
#define MODREVISION
#include <linux/modeversions.h>
#endif

cleanum_module()
{
    printk("Bye world from kernel\n");
    return 0;
}
