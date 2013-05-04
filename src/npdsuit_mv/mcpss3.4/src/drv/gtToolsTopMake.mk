


####################################################################
#   GNU Definitions
####################################################################

ifeq  ($(findstring gnu,$(TOOL)),gnu)

ifeq (workbench, $(TOOL_ENV))
  TGT_DIR = $(WIND_BASE)/target
  include $(TGT_DIR)/h/make/defs.bsp
else
 AS      =$(PREFIX)as$(POSTFIX)
 CC      =$(PREFIX)$(G)cc$(POSTFIX)
 AR      =$(PREFIX)ar$(POSTFIX)
 LD      =$(PREFIX)ld$(POSTFIX)
 CPP     =$(PREFIX)cpp$(POSTFIX)
 NM      =$(PREFIX)nm$(POSTFIX)
endif

RM      =rm
MAKE    =gmake
TOUCH   =touch
COPY    =cp
MKDIR   =mkdir


#ifeq (ppc, $(CPU_TOOL))
#CPU_FAMILY = PPC_CPU
#LDFLAGS = -EB -r -X -N

# CPU depended compilation flags (default)
#CPU_FLAGS = -meabi -mstrict-align

#ifeq (PPC603, $(CPU))
#CPU_FLAGS = -mstrict-align
#endif

#ifeq (PPC860, $(CPU))
#CPU_FLAGS = -mstrict-align
#endif

#ifneq (3, $(GCCVER))
#CPU_FLAGS += -mlongcall
#endif
#endif

ifeq (mips, $(CPU_TOOL))
#CPU_FLAGS = -mlong-calls -mips3 -mgp32 -mfp32 -non_shared -G 0
#CPU_FLAGS = -mlong-calls -mips3 -mabi=64 -mgp64 -mfp64 -non_shared -G 0# for octeon mips64
CPU_FLAGS = -mlong-calls -mips3 -mabi=32 -mgp32 -mfp32 # for gnu mips 
CPU_FAMILY = MIPS_CPU
LDFLAGS = -EB -r -X -N
# LDFLAGS = -r -X -N
endif

#ifeq (arm, $(CPU_TOOL))
##LDFLAGS = -EL -r -X -N
#LDFLAGS = -r -X -N
#CPU_FLAGS = -DARMEL -DCPU_$(ARM_CPU) -DARMMMU=ARMMMU_$(ARM_MMU) -DARMCACHE=ARMCACHE_$(ARM_CACHE) -DLE
#CPU_FAMILY = CPU_ARM
##Do we compile for thumb ?
#ifeq  ($(findstring _T,$(CPU)),_T)
## definitions for Thumb mode
#CPU_FLAGS += -mthumb -mthumb-interwork
#endif
#endif

#ifeq (intel, $(CPU_TOOL))
#LDFLAGS = -EL -r -X -N
#CPU_FAMILY = INTEL_CPU
#endif

#Add a C definition to distiguish between different OS (Added for FreeBSD)
ifdef OS_TARGET
#CFLAGS1 = -B$(COMPILER_ROOT) -Wall $(CC_ARCH) -DCPU=$(CPU) -D_$(OS_RUN) -DOS_TARGET=$(OS_TARGET) -DOS_TARGET_RELEASE=$(OS_TARGET_RELEASE) -D$(CPU_FAMILY) $(DEBUG_TYPE) $(CPSS_FLAG)
CFLAGS1 = -Wall $(CC_ARCH) -DCPU=$(CPU) -D_$(OS_RUN) -DOS_TARGET=$(OS_TARGET) -DOS_TARGET_RELEASE=$(OS_TARGET_RELEASE) -D$(CPU_FAMILY) $(DEBUG_TYPE) $(CPSS_FLAG)
else
CFLAGS1 = -B$(COMPILER_ROOT) -Wall $(CC_ARCH) -DCPU=$(CPU) -D_$(OS_RUN) -DOS_TARGET=$(OS_RUN) -D$(CPU_FAMILY) $(DEBUG_TYPE)
endif

#Add SDMA Rx packet via interrupt
#CFLAGS1 += -DCPSS_PACKET_RX_INT

#Add RSTP socket type:stream
#CFLAGS1 += -DRSTP_SOCK_STREAM
CFLAGS1 += -DRSTP_SOCK_DGRAM

#Add IGMP Snooping socket type:stream
CFLAGS1 += -DIGMP_SNOOP_SOCKET_STREAM

#Add icmp echo reply send from user space
#CFLAGS1 += -D__APP_ICMP_REPLY__

#Add Galtis shell supported
#CFLAGS1 += -DAX_WITH_GALTIS

#Add version full debug option
#CFLAGS1 += -DAX_FULL_DEBUG

#Add driver log to standout option
#CFLAGS1 += -DAX_LOG_STDOUT

#Add pci channel performance test control
#CFLAGS1 += -D__AX_PCI_TEST__

#CFLAGS2 = -ansi -pedantic -fvolatile -fno-builtin -funroll-loops
ifeq (1,$(LINUX_SIM))
CFLAGS2 = -ansi  -fno-builtin -funroll-loops
else
CFLAGS2 = -fno-builtin -funroll-loops
# GnuEabi compiler don't supprt -fvolatile...
ifndef USE_GNUEABI
CFLAGS2 += -fvolatile
endif
endif

ifeq (PPC603, $(CPU))
CFLAGS_OPT = -O0 
else
ifeq (PPC860, $(CPU))
CFLAGS_OPT = -O0 
else
CFLAGS_OPT = -O2 
endif
endif

ifeq (mainCmd, $(PROJ_NAME))
  CFLAGS_OPT = -O0 
endif
ifeq (mainAppDemo, $(PROJ_NAME))
  CFLAGS_OPT = -O0 
endif

ifeq (2,$(OPT_LEVEL))
  CFLAGS_OPT = -O2 
endif
ifeq (1,$(OPT_LEVEL))
  CFLAGS_OPT = -O1 
endif
ifeq (0,$(OPT_LEVEL))
  CFLAGS_OPT = -O0 
endif

BUILD_MAP_FILE= -Map 

endif

####################################################################
#   DIAB Definitions
####################################################################

ifeq  ($(findstring diab,$(TOOL)),diab)

TGT_DIR=$(WIND_BASE)/target

include $(TGT_DIR)/h/make/defs.bsp

ifeq (ppc, $(CPU_TOOL))
CPU_FAMILY = PPC_CPU
LDFLAGS    = $(CC_ARCH_DIAB) -X -r 
endif

ifeq (mips, $(CPU_TOOL))
CPU_FAMILY = MIPS_CPU
endif

ifeq (arm, $(CPU_TOOL))
CPU_FAMILY = CPU_ARM
endif

ifeq (intel, $(CPU_TOOL))
CPU_FAMILY = INTEL_CPU
endif

#Add a C definition to distiguish between different OS (Added for FreeBSD)
ifdef OS_TARGET
CFLAGS1 = $(CC_ARCH_DIAB) -DCPU=$(CPU) -D_$(OS_RUN) -DOS_TARGET=$(OS_TARGET) -D$(CPU_FAMILY)  $(DEBUG_TYPE)
else
CFLAGS1 = $(CC_ARCH_DIAB) -DCPU=$(CPU) -D_$(OS_RUN) -DOS_TARGET=$(OS_RUN) -D$(CPU_FAMILY)  $(DEBUG_TYPE)
endif

ifeq (1,$(LINUX_SIM))
CFLAGS2 = -ansi  -fno-builtin -funroll-loops
else
CFLAGS2 = -ansi -fvolatile -fno-builtin -funroll-loops
endif

#CFLAGS2 = -Xenum-is-best -Xeieio
ifeq (ppc, $(CPU_TOOL))
#CFLAGS2 = -Xeieio
# suppress warning 1244 constant out of range
CFLAGS2 = -ei1244
endif
#CFLAGS_OPT = 

BUILD_MAP_FILE= -m2 > 

RM      =rm
MAKE    =gmake
TOUCH   =touch
COPY    =cp
MKDIR   =mkdir



endif


####################################################################
#   Visual C++ Definitions
####################################################################

ifeq (visual, $(TOOL))

TGT_DIR=$(WIND_BASE)\target

ifeq (intel, $(CPU_TOOL))
CPU_FAMILY = INTEL_CPU
endif

CFLAGS1 = /nologo /W3 /DCPU=$(CPU) /D_$(OS_RUN) /D$(CPU_FAMILY) /DWIN32 /D_WIN32 /DNDEBUG /D_MBCS /D_VISUALC /DIMAGE_HELP_SHELL /D_CONSOLE /D_LIB  $(DEBUG_TYPE)
CFLAGS2 =

ifeq (6, $(VC_VER))
CFLAGS_OPT = /Od /Gm /GX /GZ /ZI /MTd
endif

ifeq (8, $(VC_VER))
CFLAGS_OPT = /Od /Gm /EHsc /RTC1 /ZI /MTd
endif

ifeq (simulation, $(PROJ_NAME))
CFLAGS_OPT += /Zp1
endif

LDFLAGS = 

BUILD_MAP_FILE=/Fm

AS      =
CC      =cl
AR      =lib
LD      =link
CPP     =cl
NM      =
RM      =rm
MAKE    =gmake
TOUCH   =touch
COPY    =copy
MKDIR   =mkdir
MD      =makedepend

endif
