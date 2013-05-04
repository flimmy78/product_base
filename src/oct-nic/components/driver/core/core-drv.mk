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
#  Core Driver  Makefile fragment
#

#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY := $(OBJ_DIR)/libcvm-pci-drv.a 

OBJS_$(d)  := 	$(OBJ_DIR)/cvm-pci-pko.o	\
		$(OBJ_DIR)/cvm-drv.o 		 \
		$(OBJ_DIR)/cvm-drv-debug.o	 \
		$(OBJ_DIR)/cvm-drv-reqresp.o \
		$(OBJ_DIR)/cvm-pci-loadstore.o \
		$(OBJ_DIR)/cvm-cn63xx.o \
		$(OBJ_DIR)/cvm-cn68xx.o \
		$(OBJ_DIR)/cvm-pci-dma.o


ifeq ($(findstring CN56XX_PEER_TO_PEER,$(OCTDRVFLAGS)), CN56XX_PEER_TO_PEER)
OBJS_$(d) +=  $(OBJ_DIR)/cn56xx_ep_comm.o 
endif




$(OBJS_$(d)):  CFLAGS_LOCAL := -I$(d) -I$(d)/../common  -O2 -g

#  standard component Makefile rules

DEPS_$(d)   :=  $(OBJS_$(d):.o=.d)

LIBS_LIST   :=  $(LIBS_LIST) $(LIBRARY)

CLEAN_LIST  :=  $(CLEAN_LIST) $(OBJS_$(d)) $(DEPS_$(d)) $(LIBRARY)

-include $(DEPS_$(d))

$(LIBRARY): $(OBJS_$(d))
	$(AR) -r $@ $^

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

$(OBJ_DIR)/%.o:	$(d)/%.S
	$(COMPILE)

#  standard component Makefile footer

d   :=  $(dirstack_$(sp))
sp  :=  $(basename $(sp))


# $Id: core-drv.mk 60689 2011-05-25 00:00:17Z panicker $
