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
# This file provides system-wide defaults to compile the driver sources.
#
# IMPORTANT: Do not leave spaces at the end of directory paths.
#



# Enable this flag if the driver and applications will run on an Octeon
# in PCI Host mode.
HOST_IS_OCTEON=1


ifeq ($(HOST_IS_OCTEON), 1)
# The compiler needs to be changed only for the host sources.
# No changes are made if the core application includes this file.
ifneq ($(findstring OCTEON_CORE_DRIVER,$(COMPILE)), OCTEON_CORE_DRIVER)
kernel_source := $(KERNEL_ROOT)
CC=mips64-octeon-linux-gnu-gcc
AR=mips64-octeon-linux-gnu-ar
endif
else
kernel_source := /lib/modules/$(shell uname -r)/build
ENABLE_CURSES=1
endif



# The driver sources are assumed to be in this directory.
# Modify it if you installed the sources in a different directory.
DRIVER_ROOT := $(OCTNIC_ROOT)/components/driver

BINDIR := $(DRIVER_ROOT)/bin



#Enable the DMA Interrupt Coalescing
#OCTDRVFLAGS += -DCVMCS_DMA_IC
OCTDRVFLAGS  += -DUSE_DDOQ_THREADS

#Enable this flag to turn on peer to peer communication for CN56XX
#OCTDRVFLAGS  += -DCN56XX_PEER_TO_PEER

OCTDRVFLAGS  += -DDISABLE_PCIE14425_ERRATAFIX

# $Id$ 
