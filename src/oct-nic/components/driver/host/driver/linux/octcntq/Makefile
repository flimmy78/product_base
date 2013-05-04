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
# Makefile for Linux Octeon CNTQ driver
#

#
# Compilation Flags.
# Select one or more attributes for the driver from the list below.
#
#  COMPILEFOR = DEBUG BUFPOOL
#
COMPILEFOR= 


#
# Setup path to sources and linux version
#

# Give the complete path to driver.mk here.
# Modify path if required.
include $(OCTNIC_ROOT)/components/driver/driver.mk

# The version of linux for which the driver will be compiled
LINUX_VERSION := $(shell grep -m 1 PATCHLEVEL $(kernel_source)/Makefile | sed -e 's/.*= \(.*\)/2.\1/g')

 


#
# Linux version specific adjustments
#
ifeq ($(LINUX_VERSION), 2.4)
SUBDIRS := .
CFLAGS += -D__KERNEL__ -DMODULE -DEXPORT_SYMTAB -I$(kernel_source)/include -march=i686
endif


#
# Setup compilation flags here
#
WARNINGS = -Wall -Wuninitialized -Wunused-function

INCLUDE += -I$(SUBDIRS)/..    -I$(SUBDIRS)/../../../../common  \
	  -I$(SUBDIRS)/../../osi  -I$(SUBDIRS)/../../../include  -I$(SUBDIRS)

EXTRA_CFLAGS +=	-D__SMP__ -O2 -finline-functions $(INCLUDE) $(WARNINGS)

EXTRA_CFLAGS += $(OCTDRVFLAGS) 


KBUILD_EXTRA_SYMBOLS := $(SUBDIRS)/../Module.symvers



#
# Driver object files
#
default: all 

OBJS += octcntq_main.o            \
	octcntq_proc.o            \
	../../osi/octeon_cntq.o   \
	../../osi/octeon_ddoq_list.o \
	../../osi/octeon_ddoq.o



ifeq ($(findstring BUFPOOL,$(COMPILEFOR)), BUFPOOL)
# Enable this flag to use preallocated buffer pools for dynamic memory
# allocation.
EXTRA_CFLAGS += -DUSE_BUFFER_POOL
endif


ifeq ($(findstring DEBUG,$(COMPILEFOR)), DEBUG)
# Creates debug levels.
# The default debug level is 0 when only critical messages are printed.
# Only errors are printed if this flag is not defined.
EXTRA_CFLAGS += -DCAVIUM_DEBUG=0
endif





ifeq ($(LINUX_VERSION), 2.6) 

# The driver object file
obj-m         :=  octcntq.o
octcntq-y  :=  $(OBJS)


all:
	$(MAKE) -C $(kernel_source) SUBDIRS=`pwd`  modules; 
	@rm -f $(BINDIR) 2> /dev/null; mkdir -p $(BINDIR);
	@rm -f $(BINDIR)/octcntq.ko; 
	@ln -s `pwd`/octcntq.ko $(BINDIR) ; 


clean:
	rm -rf $(BINDIR)/octcntq.ko $(OBJS) *.ko .*.cmd *.mod.* *.o *~ .tmp_versions

else   

all: octcntq.o 
	$(MAKE) octcntq.o; 
	@rm -f $(BINDIR) 2> /dev/null; mkdir -p $(BINDIR);
	@rm -f $(BINDIR)/octcntq.o; 
	@ln -s `pwd`/octcntq.o $(BINDIR) ; 

octcntq.o: $(OBJS)
	$(LD) -r $^ -o $@

%.o : %.c 
		$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c  $^ -o $@

../../osi/%.o : ../../osi/%.c 
		$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c  $^ -o $@

clean:
	@rm -rf $(BINDIR)/octcntq.o .*.cmd *.mod.* *.o *~ .tmp_versions

endif

# $Id: Makefile 67895 2011-12-09 06:29:17Z mchalla $
