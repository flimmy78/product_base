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
# Makefile for request/response test program for Octeon PCI driver
#



include $(OCTNIC_ROOT)/components/driver/driver.mk


#
# Compilation flags
#
INCLUDEFILES =  -I$(OCTNIC_ROOT)/components/driver/host/include \
                -I$(OCTNIC_ROOT)/components/driver/common       \
                -I$(OCTNIC_ROOT)/components/driver/bin

CFLAGS = -Wall $(INCLUDEFILES) -O0
LDFLAGS = -L$(BINDIR) -loctapi

ifeq ($(HOST_IS_OCTEON),1)
LDFLAGS += --static
endif

OBJS = $(OCTNIC_ROOT)/components/driver/common/oct_test_list.uo  \
		octeon_req_perf.uo parse_args.uo setup_requests.uo       \
		alltests.uo


oct_test: $(OBJS)
	$(CC) $(CFLAGS) -g $^ -o $@ $(LDFLAGS) 
	ln -sf $(PWD)/$@ $(OCTNIC_ROOT)/components/driver/bin

%.uo:  %.c
	$(CC) $(CFLAGS) -g -c $^ -o $@

clean:
	rm -f  $(OBJS) *~ oct_test

# $Id: Makefile 60697 2011-05-25 01:02:18Z panicker $
