#
#
# OCTEON SDK
#
# Copyright (c) 2011 Cavium Networks. All rights reserved.
#
# This file, which is part of the OCTEON SDK which also includes the
# OCTEON SDK Package from Cavium Networks, contains proprietary and
# confidential information of Cavium Networks and in some cases its
# suppliers. 
#
# Any licensed reproduction, distribution, modification, or other use of
# this file or the confidential information or patented inventions
# embodied in this file is subject to your license agreement with Cavium
# Networks. Unless you and Cavium Networks have agreed otherwise in
# writing, the applicable license terms "OCTEON SDK License Type 5" can be found 
# under the directory: $OCTEON_ROOT/components/driver/licenses/
#
# All other use and disclosure is prohibited.
#
# Contact Cavium Networks at info@caviumnetworks.com for more information.
#
################################################################################


#
# Makefile for Linux Octeon PCI driver
#


#
# Compilation Flags.
# Select one or more attributes for the driver from the list below. 
#
#  COMPILEFOR = DEBUG BUFPOOL ETHERPCI
#
#COMPILEFOR = DEBUG


#
# Setup path to sources and linux version
#

# Give the complete path to driver.mk here.
# Modify path if required.
include $(OCTNIC_ROOT)/components/driver/driver.mk

#
# Setup compilation flags here
#
WARNINGS = -Wall -Wuninitialized -Wunused-function -Werror-implicit-function-declaration

INCLUDE += -I.    -I$(SUBDIRS)/../../../common    -I$(SUBDIRS)/../osi    \
	-I$(SUBDIRS)/../../include  -I$(SUBDIRS)/../test -I$(SUBDIRS)

EXTRA_CFLAGS +=	-D__SMP__ -O2 -finline-functions $(INCLUDE) $(WARNINGS)

#EXTRA_CFLAGS += -DFAST_PATH_DROQ_DISPATCH

EXTRA_CFLAGS += ${OCTDRVFLAGS}


#EXTRA_CFLAGS += -DENABLE_PCIE_2G4G_FIX

#
# Driver object files
#
default: all 

OBJS += octeon_linux.o            \
        octeon_main.o             \
	octeon_poll.o             \
	octeon_ioctl.o            \
	cavium_proc.o             \
	../osi/request_manager.o  \
	../osi/response_manager.o \
	../osi/pending_list.o     \
	../osi/octeon_device.o    \
	../osi/cn3xxx_device.o    \
	../osi/cn56xx_device.o    \
	../osi/cn63xx_device.o    \
	../osi/cn66xx_device.o    \
	../osi/cn68xx_device.o    \
	../osi/octeon_errata.o    \
	../osi/octeon_mem_ops.o   \
	../osi/octeon_droq.o

ifeq ($(findstring DEBUG,$(COMPILEFOR)), DEBUG)
# Creates debug levels.
# The default debug level is 0 when only critical messages are printed.
# Only errors are printed if this flag is not defined.
EXTRA_CFLAGS += -DCAVIUM_DEBUG=0
OBJS += ../osi/octeon_debug.o
endif


ifeq ($(findstring BUFPOOL,$(COMPILEFOR)), BUFPOOL)
# Enable this flag to use preallocated buffer pools for dynamic memory
# allocation.
EXTRA_CFLAGS += -DUSE_BUFFER_POOL
OBJS += ../osi/buffer_pool.o
endif


ifeq ($(findstring ETHERPCI,$(COMPILEFOR)), ETHERPCI)
# For Ethernet over PCI
EXTRA_CFLAGS += -DETHERPCI
endif


# The driver object file
obj-m         :=  octeon_drv.o
octeon_drv-y  :=  $(OBJS)


all:
	@echo "the source kernel sorece is $(kernel_source)"
	@echo "the octeon_root dir  is $(OCTNIC_ROOT)"
	$(MAKE) -C $(kernel_source) SUBDIRS=`pwd`  modules; 
	@rm -f $(BINDIR) 2> /dev/null; mkdir -p $(BINDIR);
	@rm -f $(BINDIR)/octeon_drv.ko; 
	@ln -s $(PWD)/octeon_drv.ko $(BINDIR) ; 


clean:
	@rm -rf $(BINDIR)/octeon_drv.ko *.ko .*.cmd *.mod.* *.o *~ .tmp_versions
	@rm -f ../osi/.*.cmd ../osi/*.o  ../osi/*~
	@rm -f ../test/.*.cmd ../test/*.o ../test/*~ *.ko 

# $Id: Makefile 66446 2011-10-25 02:31:59Z mchalla $
