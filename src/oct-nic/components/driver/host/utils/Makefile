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
# Makefile for PCI driver host utilities 
#



include ../../driver.mk

#
# The utilities to be compiled.
#
TARGETS = $(BINDIR)/liboctapi oct_dbg oct_stats 

all: $(TARGETS) links



#
# Compilation flags
#
CFLAGS = -Wall -I../include -I../../common -O2 

ifeq ($(ENABLE_CURSES),1)
CFLAGS += -DUSE_CURSES -lcurses
endif

LDFLAGS = -L$(BINDIR) -loctapi

ifeq ($(HOST_IS_OCTEON),1)
LDFLAGS += --static
endif


links: 
	@rm -f $(BINDIR) 2> /dev/null; mkdir -p $(BINDIR);
	@ln -sf $(PWD)/oct_dbg $(BINDIR);
	@ln -sf $(PWD)/oct_stats $(BINDIR);


$(BINDIR)/liboctapi:
	cd ../api; make;

%:  %.c 
	$(CC) $(CFLAGS) -g $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.o  *~ $(TARGETS);
	cd $(BINDIR); rm -f $(TARGETS);


# $Id: Makefile 42868 2009-05-19 23:57:52Z panicker $
