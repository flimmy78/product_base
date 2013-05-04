kernel_source := ../../kernel2.6.32.27cn

CAVIUM_INCLUDES := -I ${SUBDIRS}/config -I ${OCTEON_ROOT}/target/include -I ${OCTEON_ROOT}/executive -I $(kernel_source)/include/linux

# Common flags to be passed for driver compilation
EXTRA_CFLAGS += -Winline -Wall ${CAVIUM_INCLUDES} -DOCTEON_MODEL=${OCTEON_MODEL} -D__SMP__

default: all
obj-m         :=  cavium_ratelimit.o

cavium_ratelimit: $(cavium_ratelimit-objs)

config/cvmx-config.h: config/executive-config.h ${OCTEON_ROOT}/executive/cvmx-resources.config
	cvmx-config config/executive-config.h

all: config/cvmx-config.h
	$(MAKE) -C $(kernel_source) SUBDIRS=`pwd`  modules;

clean:
	rm -rf .*.cmd *.mod.c *.o *.ko config/cvmx-config.h .tmp_versions

