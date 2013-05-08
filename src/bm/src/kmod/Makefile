
obj-m	:= bm.o
bm-y := bmk_main.o bmk_read_eeprom.o bmk_eth_port_stats.o bmk_operation_boot_env.o ax_soft_i2c.o bmk_soft_read_eeprom.o bmk_write_eeprom.o bmk_fpga_burn.o

MODULE    := bm
KVERS     := $(shell uname -r)
#KDIR      := /lib/modules/$(KVERS)/build
KDIR      := ${OCT_KERNEL_ROOT}
INCLUDE   := -isystem $(KDIR)/include -I {OCTEON_ROOT}/executive
MODCFLAGS := -DMODULE -D__KERNEL__ -Wall ${INCLUDE}

EXTRA_CFLAGS += -I${ACCAPI_PB_DIR}

all: 
	$(MAKE) V=1 -C ${KDIR} M=`pwd` modules
	mv *.ko $(KMOD_EXPORT_DIR)
	cp ../../res/erd_full_version.bin  ${AUTEWAREFS_DIR}/files/opt/bin/

clean:
	#rm -f ../../exec/*.ko
	rm -f *.o
	rm -f *.mod.o
	rm -f *.mod.c
	rm -f *.*~
	rm -rf .tmp_versions
	rm -rf .bm*
	rm -rf Module.symvers

