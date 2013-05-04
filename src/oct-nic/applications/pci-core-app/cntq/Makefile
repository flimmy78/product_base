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
# PCI CNTQ Octeon simple exec application Makefile
#

# The driver files are kept in this location.
DRIVER_ROOT = $(OCTNIC_ROOT)/components/driver

# The application specific files are located here.
APP_ROOT = $(PWD)

# The application binary is copied here after compilation.
BINDIR = $(DRIVER_ROOT)/bin

# Application specification
# This is the default.
TARGET        =  cvmcs-cntq


#  default target
default: $(TARGET) 
	cp $^ $^.strip;
	mipsisa64-octeon-elf-strip $^.strip;
	@ln -fs $(PWD)/$^.strip $(BINDIR)/$^.strip;

#  standard common Makefile fragment
include $(OCTNIC_ROOT)/common.mk


#  global debug setting for compile
#DEBUG_FLAGS = -DOCTEON_DEBUG_LEVEL=6


COMPILE += -DOCTEON_CORE_DRIVER -O2 -g $(DEBUG_FLAGS) 

COMPILE += -Wall -Wa,-a=$@.list -Wno-unused-parameter -O2 -g -fno-strict-aliasing $(DEBUG_FLAGS) $(CORE_MASKS) $(CC_FLAGS) $(INCLUDE_DIRS) $(DRIVER_DEFINES)


dir := $(OCTNIC_ROOT)/components/driver
include $(dir)/driver.mk

COMPILE += $(OCTDRVFLAGS)

dir := $(OCTNIC_ROOT)/applications/pci-core-app/common
include $(dir)/cvmcs-common.mk

dir := $(OCTNIC_ROOT)/applications/pci-core-app/test
include $(dir)/cvmcs-test.mk

#  include needed components common sanity code fragments
dir := $(OCTNIC_ROOT)/components/common
include $(dir)/common.mk

#  include needed Core driver Makefile fragments
dir := $(DRIVER_ROOT)/core
include $(dir)/core-drv.mk

#  include needed Core driver Makefile fragments
dir := $(DRIVER_ROOT)/core
include $(dir)/core-drv-cntq.mk





#  include needed simple executive Makefile fragments
dir := $(OCTEON_ROOT)/executive
include $(dir)/cvmx.mk





#Action for making cvmcs-cntq
OBJS += $(OBJ_DIR)/cvmcs-cntq-main.o $(OBJ_DIR)/cvmcs-cntq-test.o


CFLAGS_LOCAL = -I$(OCTNIC_ROOT)/applications/pci-core-app/common \
                -I$(OCTNIC_ROOT)/applications/pci-core-app/test \
                -I$(DRIVER_ROOT)/common -W -Wall -Wno-unused-parameter


include $(OCTNIC_ROOT)/application.mk



#  clean target
clean:
	rm -rf $(TARGET) $(CVMX_CONFIG) $(OBJ_DIR) *.o *.d *~ $(TARGET).strip




# $Id: Makefile 42872 2009-05-20 00:06:26Z panicker $
