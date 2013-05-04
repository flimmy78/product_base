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
# Makefile for PCI driver user-space test program 
#


include $(OCTNIC_ROOT)/components/driver/driver.mk


#
# Compilation flags
#
INCLUDEFILES = 	-I$(OCTNIC_ROOT)/components/driver/host/include \
				-I$(OCTNIC_ROOT)/components/driver/common

CFLAGS = -Wall $(INCLUDEFILES) -O2
LDFLAGS = -L$(BINDIR) -loctapi

ifeq ($(HOST_IS_OCTEON),1)
LDFLAGS += --static
endif



TARGET = oct_req 


all: $(TARGET)  links

links:
	@rm -f $(BINDIR) 2> /dev/null; mkdir -p $(BINDIR);
	@ln -sf $(PWD)/oct_req $(BINDIR)/oct_req

%: Makefile %.c %.h ../api/liboctapi.a
	$(CC) $(CFLAGS) -g $(@F).c -o $@ $(LDFLAGS)

../api/liboctapi.a:
	cd ../api; make all;

clean:
	rm -f *.o *~ $(BINDIR)/$(TARGET) $(TARGET)


# $Id: Makefile 42681 2009-05-08 18:11:54Z panicker $

