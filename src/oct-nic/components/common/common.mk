#
#  OCTEON TOOLKITS                                                         
#  Copyright (c) 2007 Cavium Networks. All rights reserved.
#
#  This file, which is part of the OCTEON TOOLKIT from Cavium Networks,
#  contains proprietary and confidential information of Cavium Networks
#  and in some cases its suppliers.
#
#  Any licensed reproduction, distribution, modification, or other use of
#  this file or confidential information embodied in this file is subject
#  to your license agreement with Cavium Networks. The applicable license
#  terms can be found by contacting Cavium Networks or the appropriate
#  representative within your company.
#
#  All other use and disclosure is prohibited.
#
#  Contact Cavium Networks at info@caviumnetworks.com for more information.
#

#
#  Common Makefile fragment
#

#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

LIBRARY := $(OBJ_DIR)/libcvm-common.a

OBJS_$(d)  :=  $(OBJ_DIR)/cvm-common.o $(OBJ_DIR)/cvm-common-utils.o 

$(OBJS_$(d)):  CFLAGS_LOCAL := -I$(d) -g $(CFLAGS_LOCAL)

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
