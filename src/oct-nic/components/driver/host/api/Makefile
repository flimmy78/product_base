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
# Makefile for Octeon PCI user API library
#


include ../../driver.mk

INCLUDEDIR = ../include
COMMONDIR  = ../../common
CFLAGS =  -Wall -I$(INCLUDEDIR) -I$(COMMONDIR) -O2 -g


all: liboctapi.a links

links:
	@rm -f $(BINDIR) 2> /dev/null; mkdir -p $(BINDIR);
	@ln -fs $(PWD)/octeon_user.h $(INCLUDEDIR)/octeon_user.h
	@ln -sf $(PWD)/liboctapi.a $(BINDIR)

liboctapi.a: octeon_user.o
	$(AR) -r liboctapi.a octeon_user.o
	@ln -sf $(PWD)/liboctapi.a $(BINDIR)

octeon_user.o : octeon_user.c $(INCLUDEDIR)/cavium_defs.h
	@ln -fs $(PWD)/octeon_user.h $(INCLUDEDIR)/octeon_user.h
	$(CC) $(CFLAGS) -c octeon_user.c

clean:
	rm -f *.o *~  $(BINDIR)/liboctapi.a liboctapi.a

# $Id: Makefile 42868 2009-05-19 23:57:52Z panicker $
