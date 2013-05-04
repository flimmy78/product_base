# -*-makefile-*-
# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y


# This Makefile has been simplified as much as possible, by putting all
# generic material, independent of this specific directory, into
# ../Rules.make. Read that file for details
OS_INCLUDE = -I$(LINUX_BASE)/include
COMPILER_ROOT = $(LINUX_BASE)/lib/gcc-lib
OS_ROOT = $(LINUX_BASE)
GNULIBROOT	= $(LINUX_BASE)
ifeq ($(CPU_TOOL), $(HOST))
PREFIX=
POSTFIX=
endif
ifneq ($(CPU_TOOL), $(HOST))
ifeq ($(CPU), PPC603)
PREFIX=ppc_82xx-
POSTFIX=
endif
ifeq ($(CPU), PPC604)
PREFIX=ppc_7xx-
POSTFIX=
endif
endif
G=g

CC      =$(PREFIX)$(G)cc$(POSTFIX)
AR      =$(PREFIX)ar$(POSTFIX)
LD      =$(PREFIX)ld$(POSTFIX)

HOME_ROMFS	= $(DIST_HOME)/rootfs
MODULES_DIR	= $(HOME_ROMFS)/lib/modules
KERNEL_VER	= linux-2.4.x

CFLAGS = -DMODULE -D__KERNEL__ -Dlinux -D__linux__ -Dunix -D__linux__ -DEMBED -DLINUX -O2
CFLAGS += -Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common
CFLAGS += -pipe -fno-builtin -D__linux__make
CFLAGS += -nostdinc 
CFLAGS += -msoft-float 
CFLAGS += -I./include -I$(DIST_HOME)/include
#CFLAGS += -I./include -I/work/linux-2.4.26/include/
#CFLAGS += -I./include -I/home/shlomop/PowerPC/linux/include/ 
#CFLAGS += -I$(DIST_HOME)/$(KERNEL_VER)/include -I$(DIST_HOME)/$(KERNEL_VER)/include/linux
#CFLAGS += -I/usr/local/lib/gcc-lib/powerpc-unknown-linux-gnu/$(GCC_VER)/include 
#CFLAGS += -I/usr/local/arm-elf/include/
CFLAGS += -I$(XCOMP_INC_PATH)/usr/lib/gcc-lib/ppc-linux/$(GCC_VER)/include
TARGET = mvPpDrv
OBJS = $(TARGET).o
SRC = prestera.c presterairq.c

# Kernel module should share PP device for multiple users with shared memory
ifeq (1,$(SHARED_MEMORY))
	CFLAGS += -DSHARED_MEMORY
endif

all : clean $(OBJS)

mvPpDrv.o: prestera.o presterairq.o
	$(LD) -r prestera.o presterairq.o -o $@
	cp $@ $(MODULES_DIR)


%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean_all:  clean

install:
	install -d $(INSTALLDIR)
	install -c $(TARGET) $(INSTALLDIR)

clean:
	rm -f *.o *~ core .depend mvPpDrv.o $(MODULES_DIR)/mvPpDrv.*

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > $@


ifeq (.depend,$(wildcard .depend))
include .depend
endif

