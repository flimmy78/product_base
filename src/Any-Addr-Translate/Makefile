#DS_INCLUDES := -I ${SUBDIRS}/config -I ${OCTEON_ROOT}/gpl-executive
AAT_INCLUDES := -I ${OCTEON_ROOT}/gpl-executive

#kernel_source :=/opt/CN/kernel2.6.32.27cn
kernel_source := ${OCT_KERNEL_ROOT}
#kernel_source :=../../../../kernel2.6.32.27cn

# Common flags to be passed for driver compilation
EXTRA_CFLAGS += -Winline -Wall ${AAT_INCLUDES}

default: all
obj-m         :=  any-addr-translate.o

any-addr-translate-objs := aat.o
any-addr-translate-objs += aat_ioctl.o
any-addr-translate-objs += aat_rx.o
any-addr-translate-objs += aat_tx.o

any-addr-translate.o: $(any-addr-translate-objs)
	$(LD) -r $^ -o $@


all: 
	$(MAKE) -C $(kernel_source) SUBDIRS=`pwd`  modules;

clean:
	rm -rf .*.cmd *.mod.c *.o *.ko  .tmp_versions



