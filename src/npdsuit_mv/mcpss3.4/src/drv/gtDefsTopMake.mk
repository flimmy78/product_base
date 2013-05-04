####################################################################
#   Common Definitions
####################################################################

_empty_=

ifdef TOOL
        ifeq ($(TOOL),)
                TOOL = gnu
        endif
else
        TOOL = gnu
endif

ifeq ($(TOOL),visual)
FI=/I $(_empty_)
FD=/D
FO=/Fo
FC=/c
FG=/Zi
OBJ=obj
else
FI=-I
FD=-D
FO=-o
FC=-c
FG=-g
OBJ=o
endif


export PROJ_BASE = $(USER_BASE)/$(PROJ_NAME)
SW_ROOT = $(subst \,/,$(USER_BASE))

# the PROJ_MAKE used in build rules in gtTargetsTopMake.mk
# it is the default value that mathces for all libraries
# except the Dune library
# For the dune library it is overridden in gtTopMake file

export PROJ_MAKE = $(SW_ROOT)/$(PROJ_NAME)/gtTopMake

.PHONY: do_it_all
do_it_all: all


.SUFFIXES:
.SUFFIXES: .c .dep .$(OBJ) .h

LIB = lib


#ifeq (VXWORKS, $(OS_RUN))
# VX_ROOT = $(subst \,/,$(WIND_BASE))
# OS_INCLUDE= -I$(VX_ROOT)/target/h -I$(VX_ROOT)/target/config/all -I$(PROJ_BSP)
# ifneq (workbench, $(TOOL_ENV))
#   OS_DEFINE = -fno-for-scope
# endif
# COMPILER_ROOT = $(VX_ROOT)/host/x86-win32/lib/gcc-lib/
# OS_ROOT        = $(VX_ROOT)
# GNULIBROOT= $(VX_ROOT)/host/$(WIND_HOST_TYPE)
# PREFIX=
# POSTFIX=$(CPU_TOOL)
# G=
#endif


#ifeq (linux, $(OS_RUN))
#ifeq (1,$(LINUX_SIM))
# OS_INCLUDE = -I$(LINUX_BASE)/include -I$(SW_ROOT) -I$(SW_ROOT)/simulation/h -I$(SW_ROOT)/cpssEnabler/mainOs/h/linuxStubs -I$(SW_ROOT)/cpssEnabler/mainOs/src/gtOs/linux/v2linux -I$(SW_ROOT)/cpssEnabler/mainOs/h -I$(SW_ROOT)/mainDrv/h  -I$(SW_ROOT)/simulation/h/common/ROS
# OS_DEFINE = -DASIC_SIMULATION -DLINUX_SIM
#endif
OS_ROOT = $(LINUX_BASE)
GNULIBROOT      = $(LINUX_BASE)
#ifeq ($(CPU_TOOL), $(HOST))
# PREFIX=
# POSTFIX=
#endif
#ifneq ($(CPU_TOOL), $(HOST))
# ifeq ($(CPU), PPC85XX)
#  PREFIX=ppc_85xx-
#  POSTFIX=
# endif
# ifeq ($(CPU), PPC603)
#  PREFIX=ppc_82xx-
#  POSTFIX=
# endif
#
# ifeq ($(CPU), PPC604)
#  PREFIX=ppc_7xx-
#  POSTFIX=
# endif

#ifeq ($(CPU), ARMARCH5)
# ifdef USE_GNUEABI
#   PREFIX=arm-none-linux-gnueabi-
#   OS_DEFINE+=-DLINUX_NPTL
# else
#  PREFIX=arm-linux-
# endif
#  POSTFIX=
#endif

#ifeq ($(CPU), MIPS)
 ifdef USE_GNUEABI
   #PREFIX=mips64-octeon-linux-gnu-
   PREFIX=mips-linux-gnu-
   OS_DEFINE+=-DLINUX_NPTL
 else
  PREFIX=
 endif
  POSTFIX=
#endif

#endif
G=g
#endif


#ifeq (uClinux, $(OS_RUN))
# OS_DEFINE = -D$(CPU) -fno-for-scope
# OS_ROOT = $(LINUX_BASE)/arm-elf/lib
# GNULIBROOT     = $(LINUX_BASE)
# PREFIX=arm-elf-
# POSTFIX=
# G=g
#endif

#ifeq (win32, $(OS_RUN))
# OS_DEFINE = /D$(CPU) /DASIC_SIMULATION /D_VISUALC
#ifeq (6, $(VC_VER))
# OS_INCLUDE = /I "C:/Program Files/Microsoft Visual Studio/VC98/Include" /I "$(SW_ROOT)/simulation/h" /I "$(SW_ROOT)/simulation/h" /I "$(SW_ROOT)/simulation/h/common/ROS" /I "$(SW_ROOT)/simulation/h/common/SHOST"
# COMPILER_ROOT = C:/Program Files/Microsoft Visual Studio/VC98/Bin
# OS_ROOT = "C:/Program Files/Microsoft Visual Studio/VC98/Lib"
#endif
#ifeq (8, $(VC_VER))
# OS_INCLUDE = /I "C:/Program Files/Microsoft Visual Studio 8/VC/include" /I "$(SW_ROOT)/simulation/h" /I "$(SW_ROOT)/simulation/h" /I "$(SW_ROOT)/simulation/h/common/ROS" /I "$(SW_ROOT)/simulation/h/common/SHOST"
# COMPILER_ROOT = C:/Program Files/Microsoft Visual Studio 8/VC/Bin
# OS_ROOT = "C:/Program Files/Microsoft Visual Studio 8/VC/Lib"
#endif
# GNULIBROOT	=
# PREFIX=
# POSTFIX=
# G=
#endif


ifndef COMPILATION_ROOT
COMPILATION_ROOT = $(SW_ROOT)
endif

COMPILATION_ROOT_FIX = $(subst \,/,$(COMPILATION_ROOT))

#Move libs and objs to OBJ directory
COMPILATION_ROOT_FIX := $(COMPILATION_ROOT_FIX)/../../OBJ

PROJ_BSP = $(subst \,/,$(PROJ_BSP_DIR))

FPGA_SW_ROOT = $(subst \,/,$(PROJ_FPGA_DIR))


ifeq ($(TOOL),visual)
XW_ROOT = $(subst /,\,$(SW_ROOT))
FPGA_SW_ROOT = $(subst /,\,$(PROJ_FPGA_DIR))
BASE_PATH       = /I "$(XW_ROOT)\$(PROJ_NAME)\h"
PP_DRV_PATH     = /I "$(XW_ROOT)\mainPpDrv\h"
XBAR_DRV_PATH   = /I "$(XW_ROOT)\mainXbarDrv\h"
FA_DRV_PATH     = /I "$(XW_ROOT)\mainFaDrv\h"
DUNE_DRV_PATH   = /I "$(XW_ROOT)\mainSandDrv"
COMMON_PATH     = /I "$(XW_ROOT)\common\h"
EXT_UTILS_PATH	= /I "$(XW_ROOT)\mainExtUtils\h"
UTFTOOL_PATH    = /I "$(SW_ROOT)\mainUT\utfTool\h"
FPGA_PATH       = /I "$(FPGA_SW_ROOT)\mainRhodesDrv\h"
FPGA_COMMON_PATH= /I "$(FPGA_SW_ROOT)\common\h"

else
BASE_PATH       = -I$(SW_ROOT)/$(PROJ_NAME)/h
PP_DRV_PATH     = -I$(SW_ROOT)/mainPpDrv/h
XBAR_DRV_PATH   = -I$(SW_ROOT)/mainXbarDrv/h
FA_DRV_PATH     = -I$(SW_ROOT)/mainFaDrv/h
DUNE_DRV_PATH   = -I$(SW_ROOT)/mainSandDrv
COMMON_PATH     = -I$(SW_ROOT)/common/h -I$(ACCAPI_DIR)
#COMMON_PATH     = -I$(SW_ROOT)/common/h
EXT_UTILS_PATH	= -I$(SW_ROOT)/mainExtUtils/h
UTFTOOL_PATH    = -I$(SW_ROOT)/mainUT/utfTool/h
FPGA_PATH       = -I$(FPGA_SW_ROOT)/mainRhodesDrv/h
FPGA_COMMON_PATH= -I$(FPGA_SW_ROOT)/common/h
CMD_PATH			=	-I$(SW_ROOT)/cpssEnabler/mainCmd/h
EXT_DRV_PATH		=	-I$(SW_ROOT)/cpssEnabler/mainExtDrv/h
OS_PATH			=	-I$(SW_ROOT)/cpssEnabler/mainOs/h
EXT_MAC_PATH		=	-I$(SW_ROOT)/cpssEnabler/mainExtMac/h
SYSCONFIG_PATH	=	-I$(SW_ROOT)/cpssEnabler/mainSysConfig/h
GALTIS_PATH		=	-I$(SW_ROOT)/mainGaltisWrapper/h

endif



ifeq (arm, $(CPU_TOOL))
CPU_DIR = ARM$(ARM_CPU)
else
CPU_DIR = $(CPU)
endif

LIB_DIR = $(COMPILATION_ROOT_FIX)/$(PROJ_NAME)/libs/$(CPU_DIR)

SCRAP_DIR = $(COMPILATION_ROOT_FIX)/$(PROJ_NAME)/objTemp/$(CPU_DIR)

QAC_OUT_DIR = $(COMPILATION_ROOT_FIX)/$(PROJ_NAME)/Qac/$(CPU_DIR)

ARFLAGS =  crus

#
#  Target name
#
ARCH_NAME = $(PROJ_NAME)
EXE_NAME  = temp

vpath %.$(OBJ)  $(SCRAP_DIR)
vpath %.dep  $(SCRAP_DIR)
vpath %.c.err  $(QAC_OUT_DIR)
vpath %.rep  $(QAC_OUT_DIR)

unexport LIB_NAME
ifndef LIB_NAME
LIB_NAME = $(notdir $(CURDIR)).lib
endif

unexport EXE_OBJ_LIST
ifndef EXE_OBJ_LIST
EXE_OBJ_LIST = $(wildcard $(SCRAP_DIR)/*.$(OBJ))
endif

unexport SUBDIRS
unexport KERNELSUBDIRS
ifndef SUBDIRS
 SUBDIRS = $(patsubst  %/,%,$(dir $(wildcard */gtBuild)))
 KERNELSUBDIRS_TMP = $(patsubst  %/,%,$(dir $(wildcard */Makefile)))
 KERNELSUBDIRS = $(filter-out $(SUBDIRS) , $(KERNELSUBDIRS_TMP))
endif
ifeq ($(SUBDIRS) ,NONE)
 SUBDIRS =
 KERNELSUBDIRS =
endif

unexport EXCLUDE_SUBDIRS

SUBDIRS_FIX = $(subst \,/,$(SUBDIRS))
EXCLUDE_SUBDIRS_FIX = $(subst \,/,$(EXCLUDE_SUBDIRS))
KERNELSUBDIRS_FIX = $(subst \,/,$(KERNELSUBDIRS))

unexport C_FILE_LIST
ifndef C_FILE_LIST
C_FILE_LIST = $(wildcard *.c)
endif

ifeq ($(C_EXCLUDE_FILE_LIST) ,ALL)
C_EXCLUDE_FILE_LIST = $(wildcard *.c)
endif

C_FILE_LIST_FIX = $(foreach file,$(C_FILE_LIST),$(dir $(file))$(notdir $(file)))
C_EXCLUDE_FILE_LIST_FIX = $(foreach file,$(C_EXCLUDE_FILE_LIST),$(dir $(file))$(notdir $(file)))

C_FILE_LIST_AFTER_EXCLUDE = $(filter-out $(C_EXCLUDE_FILE_LIST_FIX),$(C_FILE_LIST_FIX))

SUBDIRS_AFTER_EXCLUDE = $(filter-out $(EXCLUDE_SUBDIRS_FIX),$(SUBDIRS_FIX))

vpath %.c $(patsubst %,%:,$(dir $(filter %.c,$(C_FILE_LIST_AFTER_EXCLUDE))))


OBJ_FILE_LIST = $(notdir $(C_FILE_LIST_AFTER_EXCLUDE:.c=.$(OBJ)))
DEP_FILE_LIST = $(notdir $(C_FILE_LIST_AFTER_EXCLUDE:.c=.dep))
QAC_FILE_LIST = $(notdir $(C_FILE_LIST_AFTER_EXCLUDE:.c=.c.err))
QAC_REP_FILE_LIST = $(notdir $(C_FILE_LIST_AFTER_EXCLUDE:.c=.rep))

DEP_FILES = $(addprefix $(SCRAP_DIR)/,$(DEP_FILE_LIST))
OBJ_FILES = $(addprefix $(SCRAP_DIR)/,$(OBJ_FILE_LIST))

unexport LIB_FILE_LIST
ifndef LIB_FILE_LIST
LIB_FILE_LIST = $(wildcard $(LIB_DIR)/*.lib)
endif

unexport EXE_OBJ_LIST
ifndef EXE_OBJ_LIST
EXE_OBJ_LIST = $(wildcard $(SCRAP_DIR)/*.$(OBJ))
endif

ifneq (intel, $(CPU_TOOL)) 
ifneq (,$(DEP_FILES))
ifneq (dep,$(MAKECMDGOALS))
ifneq (clean,$(MAKECMDGOALS))
include $(DEP_FILES)
endif
endif
endif
endif

DO_LOOP = for i in $(SUBDIRS_AFTER_EXCLUDE); do $(MAKE) -f gtBuild -C $$i $@; done
DO_KERNELLOOP = for i in $(KERNELSUBDIRS_FIX); do $(MAKE) -f Makefile  -C $$i; done

