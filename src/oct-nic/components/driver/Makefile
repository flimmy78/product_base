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
# Top-level Makefile for Linux Octeon PCI driver packages
#



export OCTNIC_ROOT=$(shell pwd)/../..
export OCTEON_CPPFLAGS_GLOBAL_ADD= -DUSE_RUNTIME_MODEL_CHECKS=1 -DCVMX_ENABLE_PARAMETER_CHECKING=0 -DCVMX_ENABLE_CSR_ADDRESS_CHECKING=0 -DCVMX_ENABLE_POW_CHECKS=0
DRIVER_ROOT = $(PWD)
#OCTEON_ROOT=/home/wangchong/aw2.1/src/oct-nic
#OCTEON_MODEL=OCTEON_CN56XX
#KERNEL_ROOT=/home/wangchong/aw2.1/src/kernel2.6.32.27cn

all: check host_drv host_api utils links host_nic
#all: check host_drv host_api utils links tests host_cntq


check:
ifndef OCTEON_ROOT
	@echo "####################################################"
	@echo "              OCTEON_ROOT not defined"     
	@echo " Run the env-setup script in SDK root directory"
	@echo " octeon-models.txt list the Octeon Models supported"
	@echo "####################################################"
	@exit 2
endif

host_drv: 
	@make check;
	@echo "the source kernel is $(KERNEL_ROOT)"
	cd $(DRIVER_ROOT)/host/driver/linux; make all;

host_api:
	@make check;
	cd $(DRIVER_ROOT)/host/api; make all; 

utils:
	@make check;
	cd $(DRIVER_ROOT)/host/utils; make all;


tests:
	@make check;
	cd $(DRIVER_ROOT)/host/test; make all;
	cd $(DRIVER_ROOT)/host/test/kernel/req_resp; make all;
	cd $(DRIVER_ROOT)/host/test/kernel/droq_test; make all;
	
clean:
	@make check
	cd $(DRIVER_ROOT)/host/driver/linux; make clean; 
	cd $(DRIVER_ROOT)/host/api; make clean; 
	cd $(DRIVER_ROOT)/host/utils; make clean

test_clean:
	@make check;
	cd $(DRIVER_ROOT)/host/test; make clean;
	cd $(DRIVER_ROOT)/host/test/kernel/req_resp; make clean;
	cd $(DRIVER_ROOT)/host/test/kernel/droq_test; make clean;

links:
	@make check;
	cd $(DRIVER_ROOT)/host/api; make links;
	cd $(DRIVER_ROOT)/core; make links;


host_nic: 
	@make check;
	@if [ -d $(DRIVER_ROOT)/host/driver/linux/octnic ]; then cd $(DRIVER_ROOT)/host/driver/linux/octnic; make; else echo -e "\nOcteon NIC Host Driver not installed\n"; fi;

nic_app:
	@if [ -d $(DRIVER_ROOT)/../../applications/pci-core-app/nic ]; then cd $(DRIVER_ROOT)/../../applications/pci-core-app/nic; make; else echo -e "\nOcteon NIC application not installed\n"; fi;


host_cntq: 
	@make check;
	@if [ -d $(DRIVER_ROOT)/host/driver/linux/octcntq ]; then cd $(DRIVER_ROOT)/host/driver/linux/octcntq; make; else echo -e "\nOcteon CNTQ Host Driver not installed\n"; fi;

cntq_app:
	@if [ -d $(DRIVER_ROOT)/../../applications/pci-core-app/cntq ]; then cd $(DRIVER_ROOT)/../../applications/pci-core-app/cntq; make; else echo -e "\nOcteon CNTQ application not installed\n"; fi;


base:
	@make host_drv;
	cd $(DRIVER_ROOT)/../../applications/pci-core-app/base; make;

nic:
	@make host_drv; make host_nic; make nic_app;
	
cntq:
	@make host_drv; make host_cntq; make cntq_app;
	
app_clean:
	@if [ -d $(DRIVER_ROOT)/../../applications/pci-core-app/base ]; then cd $(DRIVER_ROOT)/../../applications/pci-core-app/base; make clean; fi;
	@if [ -d $(DRIVER_ROOT)/../../applications/pci-core-app/nic ]; then cd $(DRIVER_ROOT)/../../applications/pci-core-app/nic; make clean; fi;
	@if [ -d $(DRIVER_ROOT)/../../applications/pci-core-app/cntq ]; then cd $(DRIVER_ROOT)/../../applications/pci-core-app/cntq; make clean; fi;

host_clean:
	@if [ -d $(DRIVER_ROOT)/host/driver/linux/octnic ]; then cd $(DRIVER_ROOT)/host/driver/linux/octnic; make clean; fi;
	@if [ -d $(DRIVER_ROOT)/host/driver/linux/octcntq ]; then cd $(DRIVER_ROOT)/host/driver/linux/octcntq; make clean; fi;
	
all_clean:
	make clean;
	make host_clean;
	make test_clean;
	make app_clean;

# $Id: Makefile 62414 2011-07-15 23:06:22Z panicker $
