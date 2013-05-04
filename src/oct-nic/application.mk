#/***********************license start***************
# * Copyright (c) 2003-2007  Cavium Networks (support@cavium.com). All rights
# * reserved.
# *
# *
# * Redistribution and use in source and binary forms, with or without
# * modification, are permitted provided that the following conditions are
# * met:
# *
# *     * Redistributions of source code must retain the above copyright
# *       notice, this list of conditions and the following disclaimer.
# *
# *     * Redistributions in binary form must reproduce the above
# *       copyright notice, this list of conditions and the following
# *       disclaimer in the documentation and/or other materials provided
# *       with the distribution.
# *
# *     * Neither the name of Cavium Networks nor the names of
# *       its contributors may be used to endorse or promote products
# *       derived from this software without specific prior written
# *       permission.
# *
# * TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
# * AND WITH ALL FAULTS AND CAVIUM NETWORKS MAKES NO PROMISES, REPRESENTATIONS
# * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
# * RESPECT TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
# * REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
# * DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
# * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
# * PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET
# * POSSESSION OR CORRESPONDENCE TO DESCRIPTION.  THE ENTIRE RISK ARISING OUT
# * OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
# *
# *
# * For any questions regarding licensing please contact marketing@caviumnetworks.com
# *
# ***********************license end**************************************/
#
#  application Makefile fragment
#
#  $Id: application.mk 63986 2011-08-25 22:53:25Z vsubbiah $
#

#  target to create object directory

dir := $(OCTEON_ROOT)/executive/libfdt
include $(dir)/libfdt.mk

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

#  applications object suffix rule

$(OBJ_DIR)/%.o: %.c
	$(COMPILE)

$(OBJ_DIR)/%.o: %.S
	$(ASSEMBLE)

#  application config check and rules

CVMX_CONFIG = config/cvmx-config.h
CVMX_OTHER_CONFIGS := config/*-config.h
CVMX_OTHER_CONFIGS := $(shell echo $(CVMX_OTHER_CONFIGS) | sed 's/config\/cvmx-config.h//')

$(CVMX_CONFIG): $(CVMX_OTHER_CONFIGS)
	cvmx-config $(CVMX_OTHER_CONFIGS)

#  application object dependencies

-include $(OBJS:.o=.d)

#  special rule to re-compile if important environment variables change
MATCH=${findstring DUSE_RUNTIME_MODEL_CHECKS=1, ${OCTEON_CPPFLAGS_GLOBAL_ADD}}
ifeq (${MATCH}, DUSE_RUNTIME_MODEL_CHECKS=1)
#  We are using runtime model detection, so use "runtime" as model to avoid
#    a re-compile if only OCTEON_MODEL used for simulation changes
MADE_WITH_OCTEON_MODEL = "runtime"
else
MADE_WITH_OCTEON_MODEL = $(OCTEON_MODEL)
endif
#  set special filename for target and change any spaces in it to commas
MADE_WITH = $(shell echo "$(OBJ_DIR)/made_with-OCTEON_MODEL=$(MADE_WITH_OCTEON_MODEL)=-ISA=${OCTEON_ISA}=-OCTEON_CPPFLAGS_GLOBAL_ADD=$(OCTEON_CPPFLAGS_GLOBAL_ADD)=." | sed 's/\ /,/g')

MADE_WITH_ALL = $(OBJ_DIR)/made_with-*

$(MADE_WITH):
	mkdir -p $(OBJ_DIR)
	rm -f $(MADE_WITH_ALL)
	touch "$(MADE_WITH)"

$(TARGET) $(OBJS) $(CLEAN_LIST): $(MADE_WITH) ${CVMX_CONFIG}

#  application build target

# The user decides if the build will create the cdb files for use with
# the EDB debugger or the stripped binaries used to save space and
# reduce download time by defining the EXTRA_CVMX_APPLICATION_TARGETS
# environment variable.
#
# $ setenv EXTRA_CVMX_APPLICATION_TARGETS '$(TARGET).stp $(TARGET).cdb'
#
# If the variable is not defined the build works in the usual way.

#   The user decides if the build will create archives by defining the
#   CVMX_ARCHIVE_DIRECTORY environment variable.
#   
#   $ setenv CVMX_ARCHIVE_DIRECTORY $HOME/archive
#   
#   If the variable is not defined the build works in the usual way.
#   Building will create a unique subdirectory for each OCTEON_MODEL
#   configuration.

ARCHIVE_FILES = $(TARGET) $(EXTRA_CVMX_APPLICATION_TARGETS)

archive: $(ARCHIVE_FILES)
	-@ if [ "${CVMX_ARCHIVE_DIRECTORY}" != "" ] ; then \
		mkdir -p ${CVMX_ARCHIVE_DIRECTORY}/${OCTEON_MODEL} ; \
		cp -f $(ARCHIVE_FILES) ${CVMX_ARCHIVE_DIRECTORY}/${OCTEON_MODEL}/ ; \
		chmod -R a+r ${CVMX_ARCHIVE_DIRECTORY}/${OCTEON_MODEL} ; \
	 fi ; \

application-target: $(TARGET) $(EXTRA_CVMX_APPLICATION_TARGETS) archive


ifdef TARGET_SIM
$(TARGET):
	echo "TARGET_SIM is deprecated use OCTEON_TARGET=cvmx_64/cvmx_n32/linux_64/linux_n32"
	exit 0
else
$(TARGET): $(CVMX_CONFIG) $(OBJ_DIR) $(OBJS) $(LIBS_LIST)
	$(CC) $(OBJS) $(LDFLAGS_PATH) $(LIBS_LIST) $(LDFLAGS_GLOBAL) -o $@
endif


$(TARGET).stp: $(TARGET)
	$(STRIP) -o $(TARGET).stp $(TARGET)


$(TARGET).cdb: $(TARGET)
#	Use cdbtrans to generate $(TARGET).cdb for use by the EDB source
#	debugger.  Hint: The user should set the environment variable
#	CDBTRANS_OPTIONS to pass personalized options into cdbtrans.  If
#	there exists a Windows path (through Samba, etc) to the source files
#	where they were built on Linux the user may use the -x option to
#	transform the source file paths from unix paths to equivalent
#	Windows paths.  This allows the EJTAG EDB debugger running on
#	Windows to display the source files from where they were built on
#	Linux.  Also note that multiple uses of the -x option are allowed.
#	Example:
#	$ setenv CDBTRANS_OPTIONS  "-x /nfs/user/linux x:\linux"

#	Insure that cdbtrans exists in the user's path before running it.
ifeq (linux_64,$(findstring linux_64,$(OCTEON_TARGET)))
	!(which cdbtrans64 > /dev/null 2>&1) || (cdbtrans64 -o $(TARGET) $(TARGET) ${CDBTRANS_LINUX_OPTIONS})
else
	!(which cdbtrans > /dev/null 2>&1) || (cdbtrans -o $(TARGET) $(TARGET) ${CDBTRANS_OPTIONS})
endif
