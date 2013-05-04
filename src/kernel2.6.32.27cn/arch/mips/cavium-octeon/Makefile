#
# Makefile for the Cavium Octeon specific kernel interface routines
# under Linux.
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2005-2009 Cavium Networks
#

CFLAGS_octeon-platform.o = -I$(src)/../../../scripts/dtc/libfdt
CFLAGS_setup.o = -I$(src)/../../../scripts/dtc/libfdt

obj-y := setup.o serial.o octeon-platform.o octeon-irq.o csrc-octeon.o
obj-y += dma-octeon.o octeon_info.o
obj-y += octeon-memcpy.o perf_counters.o

#obj-$(CONFIG_MTD)		      += flash_setup.o
obj-$(CONFIG_SMP)                     += smp.o
obj-y				      += pci-console.o
obj-$(CONFIG_CAVIUM_OCTEON_WATCHDOG)  += octeon-wdt.o
octeon-wdt-objs := octeon-wdt-main.o octeon-wdt-nmi.o
obj-$(CONFIG_CAVIUM_OCTEON_TRA)       += octeon-tra.o
obj-$(CONFIG_CAVIUM_OCTEON_NAND)      += octeon-nand.o
obj-$(CONFIG_CAVIUM_OCTEON_RAPIDIO)   += octeon-rapidio.o

EXTRA_CFLAGS += -Werror

DTS_FILES = octeon_3xxx.dts octeon_68xx.dts
DTB_FILES = $(patsubst %.dts, %.dtb, $(DTS_FILES))

obj-y += $(patsubst %.dts, %.dtb.o, $(DTS_FILES))

# DTC
# ---------------------------------------------------------------------------

# Generate an assembly file to wrap the output of the device tree compiler
quiet_cmd_dt_S_dtb= DTB    $@
cmd_dt_S_dtb=						\
(							\
	echo '.section .dtb.init.rodata,"a"';		\
	echo '.balign 8';		\
	echo '.global __dtb_$(*F)_begin';		\
	echo '__dtb_$(*F)_begin:';			\
	echo '.incbin "$<" ';				\
	echo '__dtb_$(*F)_end:';			\
	echo '.global __dtb_$(*F)_end';			\
	echo '.balign 8'; 		\
) > $@

$(obj)/%.dtb.S: $(obj)/%.dtb
	$(call cmd,dt_S_dtb)

quiet_cmd_dtc = DTC     $@
cmd_dtc = $(objtree)/scripts/dtc/dtc -O dtb -o $@ -b 0 $(DTC_FLAGS) $<


$(obj)/%.dtb: $(src)/%.dts
	$(call cmd,dtc)

# Let's keep the .dtb files around in case we want to look at them.
.SECONDARY:  $(addprefix $(obj)/, $(DTB_FILES))

clean-files += $(DTB_FILES) $(patsubst %.dtb, %.dtb.S, $(DTB_FILES))
