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
# Makefile for Linux Octeon NIC driver
#


#
# Compilation Flags.
# Select one or more attributes for the driver from the list below.
#
# COMPILEFOR = DEBUG BUFPOOL ETHERPCI
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
WARNINGS = -Wall -Wuninitialized -Wunused-function

INCLUDE += -I$(SUBDIRS)/..  -I$(SUBDIRS)/../../../../common  \
	 -I$(SUBDIRS)/../../osi  -I$(SUBDIRS)/../../../include  -I$(SUBDIRS)

EXTRA_CFLAGS +=	-D__SMP__ -O2 -finline-functions $(INCLUDE) $(WARNINGS)

EXTRA_CFLAGS += ${OCTDRVFLAGS}


KBUILD_EXTRA_SYMBOLS := $(SUBDIRS)/../Module.symvers


#
# Driver object files
#

OBJS += octeon_netmain.o   \
        octeon_network.o   \
        ../../osi/octeon_nic.o


default: all 






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



ifeq ($(findstring ETHERPCI,$(COMPILEFOR)), ETHERPCI)
# For Ethernet over PCI
EXTRA_CFLAGS += -DETHERPCI
endif



# The driver object file
obj-m         :=  octnic.o
octnic-y  :=  $(OBJS)

all:
	$(MAKE) -C $(kernel_source) SUBDIRS=`pwd`  modules; 
	@rm -f $(BINDIR) 2> /dev/null; mkdir -p $(BINDIR);
	@rm -f $(BINDIR)/octnic.ko; 
	@ln -s `pwd`/octnic.ko $(BINDIR) ; 


clean:
	rm -rf $(OBJS) $(BINDIR)/octnic.ko *.ko .*.cmd *.mod.* *.o *~ .tmp_versions



# $Id: Makefile 67742 2011-12-06 02:11:48Z panicker $ 
