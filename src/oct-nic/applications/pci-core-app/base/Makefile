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
# PCI BASE Octeon simple exec application Makefile
#

# The driver files are kept in this location.
DRIVER_ROOT = $(OCTNIC_ROOT)/components/driver
OCTEON_MODEL=OCTEON_CN56XX
#OCTEON_CPPFLAGS_GLOBAL_ADD= -DUSE_RUNTIME_MODEL_CHECKS=1 -DCVMX_ENABLE_PARAMETER_CHECKING=0 -DCVMX_ENABLE_CSR_ADDRESS_CHECKING=0 -DCVMX_ENABLE_POW_CHECKS=0

# The application specific files are located here.
APP_ROOT = $(PWD)

# The application binary is copied here after compilation.
BINDIR = $(DRIVER_ROOT)/bin

#  application specification
TARGET        =  cvmcs



#  default target
default: $(TARGET) 
	cp $^ $^.strip;
	mipsisa64-octeon-elf-strip $^.strip;
	@ln -fs $(PWD)/$^.strip $(BINDIR)/$^.strip;


#  standard common Makefile fragment
include $(OCTNIC_ROOT)/common.mk

#  global debug setting for compile
#DEBUG_FLAGS = -DOCTEON_DEBUG_LEVEL=6


COMPILE += $(DEBUG_FLAGS)

COMPILE += -DSDKVER="\"`oct-version | sed -n "s/.* version \(.*\)/\1/p"`\""
 


# This flag masks out code not required for core driver compilation
# from the common routines. Keep this enabled at all times.
COMPILE += -DOCTEON_CORE_DRIVER


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

#  include needed simple executive Makefile fragments
dir := $(OCTEON_ROOT)/executive
include $(dir)/cvmx.mk





#Action for making cvmcs
OBJS +=  $(OBJ_DIR)/cvmcs.o $(OBJ_DIR)/cvmcs-data-loop.o 


CFLAGS_LOCAL = -O2 -g -I$(OCTNIC_ROOT)/applications/pci-core-app/common \
				-I$(OCTNIC_ROOT)/applications/pci-core-app/test \
				-I$(DRIVER_ROOT)/common -W -Wall -Wno-unused-parameter 

include $(OCTNIC_ROOT)/application.mk



#  clean target
clean:
	rm -rf $(TARGET) $(CVMX_CONFIG) $(OBJ_DIR)  *~  *.o *.d $(TARGET).strip


# $Id: Makefile 53787 2010-10-08 22:10:24Z panicker $ 
