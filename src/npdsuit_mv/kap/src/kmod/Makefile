#cross-compile environment settings
DIST_HOME       = ${KERNEL_ROOT}
#XCOMP_PREFIX    = mips64-octeon-linux-gnu-
#XCOMP_SUFFIEX   = mips64-octeon-linux-gnu/sys-root
#G               = g
#CC              = cc
#CC              := ${XCOMP_PREFIX}${G}${CC}
#full_path       := ${shell which ${CC}}
#XCOMP_ROOT_PATH := ${shell dirname ${full_path}}#in *-gcc path
#XCOMP_ROOT_PATH := ${XCOMP_ROOT_PATH}/../mips64-octeon-linux-gnu/sys-root

# user-defined environment settings
ifndef SUBDIRS
SUBDIRS         =$(shell pwd)
endif
export SUBDIRS

CURRENT         = kap
# USER_BASE       = ${SUBDIRS}/../../..
# SRC_BASE        = $(USER_BASE)/tun
USE_GNUEABI     = 1

#compiler flags setting
KBUILD_CFLAGS += -D__KERNEL__ -Dlinux -D__linux__ -Dunix -DEMBED -DLINUX -O2
KBUILD_CFLAGS  +=  -Winline -Wall -D__linux_make

#KBUILD_CFLAGS += -nostdinc -msoft-float
#KBUILD_CFLAGS += -Iinclude -I$(SRC_BASE)/drv/mainPpDrv/h -I$(SRC_BASE)/drv/common/h
#KBUILD_CFLAGS += -I$(XCOMP_ROOT_PATH)/usr/include
KBUILD_CFLAGS += -I$(DIST_HOME)/include
KBUILD_CFLAGS += -I$(SUBDIRS)/include

CAVIUM_INCLUDES := -I ${SUBDIRS}/config -I ${OCTEON_ROOT}/gpl-executive

EXTRA_FLAGS += KBUILD_CFLAGS ${CAVIUM_INCLUDES}

TARGET = kapDrv

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m := ${TARGET}.o
${TARGET}-objs := kap.o

else

default:
	$(MAKE) V=1 -C $(DIST_HOME) M=$(shell pwd) modules
	@ cp $(TARGET).ko $(KMOD_EXPORT_DIR)/
	@ rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
	@ echo "make kapDrv.ko completed."
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(KBUILD_CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
