# cross-compile environment settings
DIST_HOME 	= ${OCT_KERNEL_ROOT}
XCOMP_PREFIX	= mips64-octeon-linux-gnu-
XCOMP_SUFFIEX	= mips64-octeon-linux-gnu/sys-root
G		= g
CC		= cc
CC		:= ${XCOMP_PREFIX}${G}${CC}
full_path	:= ${shell which ${CC}}
XCOMP_ROOT_PATH	:= ${shell dirname ${full_path}}#in *-gcc path
XCOMP_ROOT_PATH := ${XCOMP_ROOT_PATH}/../mips64-octeon-linux-gnu/sys-root

# user-defined environment settings
ifndef SUBDIRS
SUBDIRS		=${shell pwd}
endif
USER_BASE	= ${SUBDIRS}/../../..
SRC_BASE	= $(USER_BASE)/src
USE_GNUEABI	= 1
HOME_ROMFS	= $(DIST_HOME)/rootfs
MODULES_DIR	= $(HOME_ROMFS)/lib/modules

# compiler flags setting
KBUILD_CFLAGS += -D__KERNEL__ -Dlinux -D__linux__ -Dunix -DEMBED -DLINUX -O2
#CFLAGS += -Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing
#CFLAGS += -fno-common -pipe -fno-builtin -D__linux__make
KBUILD_CFLAGS  +=  -Winline -Wall -D__linux_make

# GnuEabi don't know some options like -mapcs-32 or -mshort-load-bytes
ifdef USE_GNUEABI
#CFLAGS += -mlittle-endian -mabi=apcs-gnu -march=armv4 -D__LINUX_ARM_ARCH__=5
else
KBUILD_CFLAGS += -mapcs-32 -mshort-load-bytes
endif

#CFLAGS += -nostdinc -msoft-float
KBUILD_CFLAGS += -Iinclude -I$(SRC_BASE)/drv/mainPpDrv/h -I$(SRC_BASE)/drv/common/h
KBUILD_CFLAGS += -I$(DIST_HOME)/include
KBUILD_CFLAGS += -I$(XCOMP_ROOT_PATH)/usr/include
#for use #include "sysdef/npd_sysdef.h" 
KBUILD_CFLAGS += -I$(ACCAPI_DIR)

EXTRA_FLAGS += KBUILD_CFLAGS

TARGET = mvPpDrv

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m := ${TARGET}.o
${TARGET}-objs := prestera.o presteraIrq.o

else

default:
	$(MAKE) -C $(DIST_HOME) V=1 SUBDIRS=$(shell pwd) modules	
	@ cp $(TARGET).ko $(KMOD_EXPORT_DIR)/
	@ rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
	@ echo "make mvPpDrv.ko completed."
endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(KBUILD_CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
