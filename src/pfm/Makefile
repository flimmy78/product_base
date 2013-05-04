OCTEON_ROOT = /opt/CN/OCTEON-SDK
#CAVIUM_INCLUDES := -I ${SUBDIRS}/config -I ${OCTEON_ROOT}/target/include -I ${OCTEON_ROOT}/executive
CAVIUM_INCLUDES := -I ${OCTEON_ROOT}/target/include -I ${OCTEON_ROOT}/gpl-executive -I ../cavium-ethernet -I ../rpa/src/kmod -I ../cavium-ethernet
#kernel_source := ${OCTEON_ROOT}/linux/kernel_2.6/linux/

kernel_source := ${KERNEL_ROOT} 

deamon_file := pfm.c


# Common flags to be passed for driver compilation
EXTRA_CFLAGS += -Winline -Wall ${CAVIUM_INCLUDES} -DOCTEON_MODEL=${OCTEON_MODEL}

kern: all
obj-m         :=  pfm_kern.o
#pfm-objs			:=	pfm_kern.o

#pfm_kern.o: $(pfm-objs)
#	$(LD) -r $^ -o $@

#config/cvmx-config.h: config/executive-config.h ${OCTEON_ROOT}/executive/cvmx-resources.config
#	cvmx-config config/executive-config.h

#all: config/cvmx-config.h
all:
	@echo "Make pfm_kern module"
	$(MAKE) -C $(kernel_source) SUBDIRS=`pwd`  modules;

deamon:
	@echo "Make pfm_deamon"
	$(CC) ${DBUS_INCLUDE} ${DBUS_LIBS} -L${BUILDROOTDIR}/usr/lib -lpthread ${deamon_file} -o pfm_deamon
clean:
	@echo "Make pfm clean"
	rm -rf .*.cmd *.mod.c *.o *.ko pfm_deamon .tmp_versions

