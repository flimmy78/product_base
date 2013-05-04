#*******************************************************************************
#                   Copyright 2002, GALILEO TECHNOLOGY, LTD.                   *
# THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
# NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
# OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
# DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
# THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
# IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
#                                                                              *
# MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
# MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
# SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
# (MJKK), GALILEO TECHNOLOGY LTD. (GTL) ,GALILEO TECHNOLOGY, INC. (GTI). and   *
# RADLAN COMPUTER COMMUNICATIONS LTD. (RND).
#*******************************************************************************
# Makefile for Linux and uClinux
#
# DESCRIPTION:
#       This file contains rules for building PSS for
#           Linux and uClinux on the specified board
#           using the tool chain environment. (refer to LSP manual.)
#
#*******************************************************************************

######### CPU settings #################
export CPU	= MIPS
export CPU_TOOL	= mips
export OS_RUN	= linux
##

######### prestera select ##############
PP_FAMILY	= DX
export PRESTERA_FAMILY=$(PP_FAMILY)_FAMILY
export USER_MAKE=presteraTopMake
export USE_GNUEABI=TRUE
export LINUX_CPSS_TARGET=CPSS_ENABLER
export CHX_FAMILY=EXISTS
export EXDXMX_DRIVER=EXISTS
export GT_PCI=EXISTS
export LINUX_SMI=0
export MANAGEMENT_BUS=GT_MNG_PCI
##

######### linux setting ################
export KVERSION=2
export KPATCHLEVEL=6
export KRELEASE=2.6.12.5
OS_TARGET_RELEASE=${shell uname -r}
# export OS_TARGET_RELEASE=${OS_TARGET_RELEASE:0:1}
export OS_TARGET_RELEASE=2 # shell code failed last line,so give the result directly.
##

######### user environment #############
export LINUX_DIST_PATH=${OCT_KERNEL_ROOT}
export DIST_HOME=${LINUX_DIST_PATH}
export LINUX_BASE=${LINUX_DIST_PATH}
export USER_BASE=${shell pwd}
export OS_TARGET=Linux
OBJ_BASE=$(USER_BASE)/../../OBJ
IMG_BASE=$(USER_BASE)/../../IMG
#EXPORT_BASE	= $(USER_BASE)/../../export
EXPORT_BASE=$(LIB_EXPORT_DIR)
CPSS_DRV_SHLIB_MAJOR = 0
CPSS_DRV_SHLIB_MINOR = 1
CPSS_DRV_SHLIB_LINKNAME	= libmcpss.so
CPSS_DRV_SHLIB_SONAME   = $(CPSS_DRV_SHLIB_LINKNAME).$(CPSS_DRV_SHLIB_MAJOR)
CPSS_DRV_SHLIB_REALNAME = $(CPSS_DRV_SHLIB_SONAME).$(CPSS_DRV_SHLIB_MINOR) 
##

######### compiler setting #############
## gcc
#COMPILER=mips64-octeon-linux-gnu-gcc
#COMP_PREFIX=mips64-octeon-linux-gnu
#XCOMP_ROOT_PATH_SUFFIX=mips64-octeon-linux-gnu/sys-root
COMPILER=mips-linux-gnu-gcc
COMP_PREFIX=mips-linux-gnu
#XCOMP_ROOT_PATH_SUFFIX=mips-linux-gnu/sys-root  # gnu mips has librt.a under lib
XCOMP_ROOT_PATH_SUFFIX=mips-linux-gnu

export GCC_VER=${shell $(COMPILER) -v 2>&1 | awk '/gcc version/ {print $3}'}

## path
full_path=${shell which $(COMPILER)}
export COMPILER_ROOT=${shell dirname $(full_path)}
XCOMP_ROOT_PATH=${shell dirname $(COMPILER_ROOT)}
export XCOMP_INC_PATH=$(XCOMP_ROOT_PATH)
export XCOMP_ROOT_PATH:=$(XCOMP_ROOT_PATH)/$(XCOMP_ROOT_PATH_SUFFIX)
##

## utils
#unset CC_ARCH
export BIN_BS=128
export BIN_SKIP=1
export AUTO_INIT_SYS=ENABLE
export TOOL_FAMILY=gnu
#unset CPSS_CONFIG
export USER_HOME=$(USER_BASE)

######### Debug level selector #########
DEBUG_AND_OPTIMIZE_LEVEL	= -O0 -g
##DEBUG_AND_OPTIMIZE_LEVEL	= -O2

ifeq (linux, $(OS_RUN))
#ifeq ($(CPU_TOOL), $(HOST))
#PREFIX=
#POSTFIX=
#endif

ifneq ($(CPU_TOOL), $(HOST))

#ifeq ($(CPU), PPC603)
#PREFIX=ppc_82xx-
#POSTFIX=
#endif

ifeq ($(CPU), PPC604)
PREFIX=ppc_7xx-
POSTFIX=
endif

ifeq ($(CPU), PPC85XX)
PREFIX=ppc_85xx-
POSTFIX=
endif

ifeq (arm, $(CPU_TOOL))
ifdef USE_GNUEABI
PREFIX=arm-none-linux-gnueabi-
else
PREFIX=arm-linux-
endif
POSTFIX=
endif

ifeq (mips, $(CPU_TOOL))
#ifdef USE_GNUEABI
#PREFIX=mips64-octeon-linux-gnu-
PREFIX=mips-linux-gnu-
#else
#PREFIX=arm-linux-
#endif
POSTFIX=
endif

endif
G=g
endif

TOOL	= gnu

ifeq (arm, $(CPU_TOOL))
CPU_DIR = ARM$(ARM_CPU)
else
CPU_DIR = $(CPU)
endif

AS      =$(PREFIX)as$(POSTFIX)
CC      =$(PREFIX)$(G)cc$(POSTFIX)
AR      =$(PREFIX)ar$(POSTFIX)
LD      =$(PREFIX)ld$(POSTFIX)
CPP     =$(PREFIX)cpp$(POSTFIX)
NM      =$(PREFIX)nm$(POSTFIX)
STRIP   =$(PREFIX)strip$(POSTFIX)
RM      =rm
MAKE    =gmake
TOUCH   =touch
COPY    =cp
MKDIR   =mkdir
CD	=cd

EXTRA_DEFINE	= -Wcomment                     \
		-DLE                          \
		-D_LINUX


## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.
## Should update the kernel
VENDOR		= Marvell
HOME_ROMFS	= $(DIST_HOME)/romfs
HOME_ZIMAGE	= $(DIST_HOME)/$(KERNELVER)/arch/armnommu/boot
HOME_INITRD     = $(DIST_HOME)/rootfs

##########################################################################
EXTRA_DEFINE	+= -D$(BOARD_TYPE)

RELEASE		= application

#specify extra components & include path
EXTRA_INCLUDE =
LIB_EXTRA =
MACH_EXTRA =

#specify components for cpss as lib 
CPSS_LIB = cpssDrvLib.a
COMMON_LIBS_LB = 
MACH_EXTRA_LB =
LIB_EXTRA_LB =


ifndef COMPILATION_ROOT
COMPILATION_ROOT = $(USER_BASE)
endif

#Convert DOS path to Unix
COMPILATION_ROOT_FIX = $(subst \,/,$(COMPILATION_ROOT))

#Move libs and objs to OBJ directory
COMPILATION_ROOT_FIX := $(COMPILATION_ROOT_FIX)/../../OBJ

### Set  the CPSS libs ###
PP_LIBS=$(wildcard $(COMPILATION_ROOT_FIX)/mainPpDrv/libs/$(CPU_DIR)/mainPp*.lib)
PP_LIBS+=$(wildcard $(COMPILATION_ROOT_FIX)/mainPpDrv/libs/$(CPU_DIR)/*BrgFdbHash.lib)
PP_LIBS+=$(wildcard $(COMPILATION_ROOT_FIX)/mainPpDrv/libs/$(CPU_DIR)/*GenIpLpmEngine.lib)


UTF_LIB=$(wildcard $(COMPILATION_ROOT_FIX)/mainUT/libs/$(CPU_DIR)/mainU*.lib)

UTIL_FILES = $(wildcard $(COMPILATION_ROOT_FIX)/mainExtUtils/libs/$(CPU_DIR)/mainExtUtils.o)

ifeq ($(NO_PP_USED),NOT_USED)
	PP_LIBS =
endif

### handle the COMMON files ###
COMMON_LIBS = $(COMPILATION_ROOT_FIX)/common/libs/$(CPU_DIR)/common.o

### handle the FA files ###
ifeq (PRESTERA, $(FA_VENDOR))
	FA_FILES = $(COMPILATION_ROOT_FIX)/mainFaDrv/libs/$(CPU_DIR)/mainFaDrv.o
endif

### handle the XBAR files ###
ifeq (DUNE, $(XBAR_VENDOR))
	XBAR_FILES = $(COMPILATION_ROOT_FIX)/mainSandDrv/libs/$(CPU_DIR)/mainSandDrv.o
endif
ifeq (PRESTERA, $(XBAR_VENDOR))
	XBAR_FILES = $(COMPILATION_ROOT_FIX)/mainXbarDrv/libs/$(CPU_DIR)/mainXbarDrv.o
endif

GALTIS_FILES = $(COMPILATION_ROOT_FIX)/mainGaltisWrapper/libs/$(CPU_DIR)/mainGaltisWrapper.o

#For simulation only
SIM_LIB = $(wildcard $(COMPILATION_ROOT_FIX)/simulation/libs/$(CPU_DIR)/*.lib)
ifeq (EXISTS, $(PM_FAMILY))
ifeq (FreeBSD, $(OS_TARGET))
	SIM_LIB += $(USER_BASE)/simulation/libs/GM/FreeBsd/$(OS_TARGET_RELEASE)/gm98EX24X.a
else
	SIM_LIB += $(USER_BASE)/simulation/libs/GM/Linux/gm98EX24X.a
#IDT TCAM simulation libs
	SIM_LIB += $(USER_BASE)/simulation/libs/GM/Linux/libidtplatform.a
	SIM_LIB += $(USER_BASE)/simulation/libs/GM/Linux/libidtresource.a
	SIM_LIB += $(USER_BASE)/simulation/libs/GM/Linux/libidtutility.a
	SIM_LIB += $(USER_BASE)/simulation/libs/GM/Linux/libnsa_socket_driver.a
endif
endif

# DO NOT CHANGE libraries order
# IT IS important for successfull link
LIB_EXTRA += $(UTF_LIB) $(PP_LIBS)
LIB_EXTRA_LB += $(UTF_LIB) $(COMPILATION_ROOT_FIX)/$(CPSS_LIB)


#add simulation libraries - must be 3 times for link pass
ifeq (1, $(LINUX_SIM))
LIB_EXTRA += $(SIM_LIB)
LIB_EXTRA += $(SIM_LIB)
LIB_EXTRA += $(SIM_LIB)
endif

#currently nothing to add as "extra libraries"
MACH_EXTRA += $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/cpssEnabler.o \
	$(COMMON_LIBS) \
	$(UTIL_FILES) \
	$(GALTIS_FILES) 

#specify files for cpss as lib 
MACH_EXTRA_LB += $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/cpssEnabler.o \
	$(UTIL_FILES) \
	$(GALTIS_FILES) 


ifneq (DX_FAMILY, $(PRESTERA_FAMILY))
ifneq (SX_FAMILY, $(PRESTERA_FAMILY))
MACH_EXTRA += $(XBAR_FILES) \
				$(FA_FILES)
endif
endif

CFLAGS	= $(DEBUG_AND_OPTIMIZE_LEVEL)

#Set linker flags
ifeq (uClinux, $(OS_RUN))
#NO DYNAMIC LIBRARIES IN uClinux.
LDFLAGS = -lpthread -Wl,-elf2flt="-s 65535" -L/$(DIST_HOME)/uClibc/lib
else
ifeq (linux, $(OS_RUN))
ifeq (FreeBSD, $(OS_TARGET))
ifeq (4, $(OS_TARGET_RELEASE))
LDFLAGS = -L$(XCOMP_ROOT_PATH)/usr/lib -pthread -rdynamic
else
LDFLAGS = -L$(XCOMP_ROOT_PATH)/usr/lib -lpthread -rdynamic
endif
else
#LDFLAGS = -L$(XCOMP_ROOT_PATH)/usr/lib -lpthread -ldl -rdynamic # for octeon mips64
LDFLAGS = -L$(XCOMP_ROOT_PATH)/lib -lpthread -ldl -rdynamic # for gnu mips
endif
ifeq (1, $(LINUX_SIM))
LDFLAGS += -lstdc++
ifneq (FreeBSD, $(OS_TARGET))
LDFLAGS += -lrt
endif
else
#LDFLAGS += $(XCOMP_ROOT_PATH)/usr/lib/librt.a #for octeon mips64
LDFLAGS += -lrt # for gnu mips
endif
endif
endif

# define object files get to cpss lib
OBJ_FILES_01 = $(COMPILATION_ROOT_FIX)/common/objTemp/$(CPU_DIR)/*.o
OBJ_FILES_02 = $(COMPILATION_ROOT_FIX)/mainPpDrv/objTemp/$(CPU_DIR)/*.o
OBJ_FILES_03 = $(COMPILATION_ROOT_FIX)/cpssEnabler/objTemp/$(CPU_DIR)/*.o
OBJ_FILES_04 = $(COMPILATION_ROOT_FIX)/mainGaltisWrapper/objTemp/$(CPU_DIR)/*.o

LDFLAGS += -shared -Wl,-soname,$(CPSS_DRV_SHLIB_SONAME) # for gnu mips shared library

CPSS_DRV_SHLIB_OBJS  = $(OBJ_FILES_01)
CPSS_DRV_SHLIB_OBJS += $(OBJ_FILES_02)
CPSS_DRV_SHLIB_OBJS += $(OBJ_FILES_03)
CPSS_DRV_SHLIB_OBJS += $(OBJ_FILES_04)

# compile object as shared library  by default 
CPSS_FLAG = -D__CPSS_DRV_SHLIB__
#CPSS_FLAG += -DLINUX_SIM

ifeq (${TARGET},app)
CPSS_FLAG = -D__CPSS_DRV_APP__
endif

# Make cpss driver as shared library
lib: prelib cpss $(CPSS_DRV_SHLIB_REALNAME)
	

$(CPSS_DRV_SHLIB_REALNAME):$(CPSS_DRV_SHLIB_OBJS)
	$(RM) -rf $(EXPORT_BASE)/$(CPSS_DRV_SHLIB_REALNAME) $(EXPORT_BASE)/$(CPSS_DRV_SHLIB_SONAME) $(EXPORT_BASE)/$(CPSS_DRV_SHLIB_LINKNAME)
	@$(CC) $(LDFLAGS) $(CPSS_FLAG) -o $(EXPORT_BASE)/$@ $^
	ln -s $(CPSS_DRV_SHLIB_REALNAME) $(EXPORT_BASE)/$(CPSS_DRV_SHLIB_SONAME)
	ln -s $(CPSS_DRV_SHLIB_SONAME) $(EXPORT_BASE)/$(CPSS_DRV_SHLIB_LINKNAME)

$(CPSS_DRV_SHLIB_OBJS):
	

.PHONY:prelib

prelib:
	@ echo "begin libCpssDrv.so"
	@ $(MKDIR) -p $(EXPORT_BASE)

#Set build all rules
all : cpss  application

#Build cpss lib and appDemo
all_lb: cpss cpss_lib appDemo_lb

#build cpss lib only
cpss_lb: cpss cpss_lib

#Build CPSS
cpss :
	echo " PREFIX - ${PREFIX}"
#	$(MAKE) -sf presteraTopMake full --no-print-directory
	$(MAKE) -sf presteraTopMake CFLAG_A=${CPSS_FLAG} full --no-print-directory
#	$(MAKE) -f presteraTopMake CFLAG_A=${CPSS_FLAG} full

cpss_lib:
	$(AR) crus $(COMPILATION_ROOT_FIX)/$(CPSS_LIB) $(OBJ_FILES_02)
	$(AR) rcs $(COMPILATION_ROOT_FIX)/$(CPSS_LIB) $(OBJ_FILES_01)

#Build appDemo
ifneq (1, $(LINUX_SIM))
application : appDemo
appDemo :
	@ echo "begin appDemo"
	@ $(MKDIR) -p $(IMG_BASE)
	@ $(MKDIR) -p $(IMG_BASE)/drv
	cd  ${COMPILATION_ROOT_FIX}
	@ rm -f ${COMPILATION_ROOT_FIX}/appDemo
	$(CC) -o ${COMPILATION_ROOT_FIX}/appDemo $(CFLAGS) $(MACH_EXTRA) $(LIB_EXTRA) $(LDFLAGS) 
	@ echo -e "cp ${COMPILATION_ROOT_FIX}/appDemo $(IMG_BASE)/drv/appDemo"
	@ cp -f ${COMPILATION_ROOT_FIX}/appDemo $(IMG_BASE)/drv/appDemo
	@ rm -f ${COMPILATION_ROOT_FIX}/appDemo
	@ echo -e "\t"$(STRIP)" "$(IMG_BASE)"/drv/appDemo"
#	@ $(STRIP) $(IMG_BASE)/drv/appDemo
	@ chmod 777 $(IMG_BASE)/drv/appDemo
	@ echo "end appDemo"	

#build appDemo using cpss lib
appDemo_lb :
	echo " begin appDemo_lb"
	cd  ${COMPILATION_ROOT_FIX}
	rm -f ${COMPILATION_ROOT_FIX}/appDemo
	$(CC) -o ${COMPILATION_ROOT_FIX}/appDemo $(CFLAGS) $(MACH_EXTRA_LB) $(LIB_EXTRA_LB) $(LDFLAGS) 
	echo -e "cp ${COMPILATION_ROOT_FIX}/appDemo $(HOME_INITRD)/usr/bin/appDemo"
	cp ${COMPILATION_ROOT_FIX}/appDemo $(HOME_INITRD)/usr/bin/appDemo
	echo -e "\t"$(STRIP)" "$(HOME_INITRD)"/usr/bin/appDemo"
	$(STRIP) $(HOME_INITRD)/usr/bin/appDemo
	chmod 777 $(HOME_INITRD)/usr/bin/appDemo
else
application : appDemoSim
appDemoSim :
	$(CC) $(CFLAGS) $(MACH_EXTRA) $(LIB_EXTRA) $(LDFLAGS) $< -o $@
	echo "appDemoSim created"
endif


%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean_all:  clean
clean:
	rm -f $(IMG_BASE)/drv/appDemo*
	rm -f $(IMG_BASE)/drv/*.gdb
	-find $(OBJ_BASE) -name '*~' -exec rm -f {} \;
	-find $(OBJ_BASE) -name '*.o' -exec rm -f {} \;
	-find $(OBJ_BASE) -name '*.lib' -exec rm -f {} \;
	-find $(OBJ_BASE) -name '*.dep' -exec rm -f {} \;
	-find $(EXPORT_BASE) -name 'libmcpss*' -exec rm -f {} \;
#	rm -f appDemo*
#	rm -f *.gdb
#	find . -name '*~' -exec rm -f {} \;
#	find . -name '*.o' -exec rm -f {} \;
#	find . -name '*.lib' -exec rm -f {} \;
#	find . -name '*.dep' -exec rm -f {} \;

unix:
	find . -name '*.c' -exec dos2unix -q {} \;
	find . -name '*.cpp' -exec dos2unix -q {} \;
	find . -name '*.h' -exec dos2unix -q {} \;
	find . -name '*.C' -exec dos2unix -q {} \;
	find . -name '*.CPP' -exec dos2unix -q {} \;
	find . -name '*.H' -exec dos2unix -q {} \;
	find . -name Makefile -exec dos2unix -q {} \;
	find . -name '*.mk' -exec dos2unix -q {} \;
	find . -name presteraTopMake -exec dos2unix -q {} \;
	find . -name gtTopMake -exec dos2unix -q {} \;
	find . -name gtBuild -exec dos2unix -q {} \;

dos:
	find . -name '*.c' -exec unix2dos -q {} \;
	find . -name '*.cpp' -exec unix2dos -q {} \;
	find . -name '*.h' -exec unix2dos -q {} \;
	find . -name '*.C' -exec unix2dos -q {} \;
	find . -name '*.CPP' -exec unix2dos -q {} \;
	find . -name '*.H' -exec unix2dos -q {} \;
	find . -name Makefile -exec unix2dos -q {} \;
	find . -name '*.mk' -exec unix2dos -q {} \;
	find . -name presteraTopMake -exec unix2dos -q {} \;
	find . -name gtTopMake -exec unix2dos -q {} \;
	find . -name gtBuild -exec unix2dos -q {} \;



