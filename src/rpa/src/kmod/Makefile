#OCTEON_ROOT = /opt/CN/OCTEON-SDK
CAVIUM_INCLUDES := -I ${OCTEON_ROOT}/executive -I ${KERNEL_ROOT}/drivers/net/octeon/
kernel_source := ../../../kernel2.6.32.27cn

# Common flags to be passed for driver compilation
EXTRA_CFLAGS += -Winline -Werror ${CAVIUM_INCLUDES} -DOCTEON_MODEL=${OCTEON_MODEL}

default: all
obj-m         :=  rpa-ethernet.o
rpa-ethernet-objs := rpa.o
rpa-ethernet-objs += rpa-ioctl.o
rpa-ethernet-objs += rpa-rx.o
rpa-ethernet-objs += rpa-tx.o

rpa-ethernet.o: $(rpa-ethernet-objs)
	$(LD) -r $^ -o $@

all: 
	$(MAKE) -C $(kernel_source) SUBDIRS=`pwd`  modules;

clean:
	rm -rf .*.cmd *.mod.c *.o *.ko config/cvmx-config.h .tmp_versions

