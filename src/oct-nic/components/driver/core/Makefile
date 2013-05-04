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
# Makefile for creating symbolic links to Octeon BASE core driver header files
#



PHONY: links


links:
	@for i in *.h; do ln -sf ../../components/driver/core/"$${i}" $(OCTNIC_ROOT)/target/include; done

clean:
	@for i in *.h; do rm -f $(OCTNIC_ROOT)/target/include/"$${i}"; done





# $Id: Makefile 42868 2009-05-19 23:57:52Z panicker $ 
