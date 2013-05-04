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
# Makefile for PCI driver kernel application to demonstrate
# 'request/response' using Octeon PCI Input queues.
#


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
EXTRA_CFLAGS += -I.    -I$(SUBDIRS)/../../../../common \
		-I$(SUBDIRS)/../../../include  -I$(SUBDIRS) \
		-D__SMP__ -O2 -Winline -Wall -finline-functions

EXTRA_CFLAGS += $(OCTDRVFLAGS)





#
# PCI Kernel application object files
#
OBJS = octeon_req_resp.o
OBJNAME = req_resp

default: all 





ifeq ($(LINUX_VERSION), 2.6) 

# The driver object file
obj-m         :=  $(OBJNAME).o
$(OBJNAME)-y  :=  $(OBJS)


all:
	$(MAKE) -C $(kernel_source) SUBDIRS=`pwd` modules; 
	@rm -f $(BINDIR) 2> /dev/null; mkdir -p $(BINDIR);
	@rm -f $(BINDIR)/$(OBJNAME).ko; 
	@ln -s $(PWD)/$(OBJNAME).ko $(BINDIR) ; 

clean:
	rm -rf $(BINDIR)/$(OBJNAME).ko *.ko .*.cmd *.mod.* *.o *~ .tmp_versions

else   

all: $(OBJNAME).o 
	@rm -f $(BINDIR) 2> /dev/null; mkdir -p $(BINDIR);
	@rm -f $(BINDIR)/$(OBJNAME).o; 
	@ln -s $(PWD)/$(OBJNAME).o $(BINDIR) ; 

$(OBJNAME).o: $(OBJS)
	$(LD) -r $^ -o $@

%.o : %.c 
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c  $^

clean:
	rm -rf $(BINDIR)/$(OBJNAME).o .*.cmd *.mod.* *.o *~ .tmp_versions

endif



# $Id: Makefile 56301 2011-01-15 00:41:25Z skoteshwar $
