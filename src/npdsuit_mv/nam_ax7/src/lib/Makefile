# Network Platform Daemon ASIC Management Library
#
#CC  = mips-linux-gnu-gcc
CFLAGS = -g -Wall #-ansi

ifeq ($(DRV_LIB_FLAG),CPSS)
CFLAGS += -DDRV_LIB_CPSS -ansi
INCLUDES += -I$(TOPSRC_DIR)/npdsuit_mv/mcpss/src/drv/mainPpDrv/h
INCLUDES += -I$(TOPSRC_DIR)/npdsuit_mv/mcpss/src/drv/common/h
INCLUDES += -I$(TOPSRC_DIR)/npdsuit_mv/mcpss/src/drv/cpssEnabler/mainOs/h
endif

ifeq ($(DRV_LIB_FLAG),CPSS_XCAT)
#DLE_HOST is not need in our CPU zhangdi 2011-03-11
#CFLAGS += -DDRV_LIB_CPSS -DDRV_LIB_CPSS_XCAT -DLE_HOST
CFLAGS += -DDRV_LIB_CPSS -DDRV_LIB_CPSS_XCAT
INCLUDES += -I$(TOPSRC_DIR)/npdsuit_mv/mcpss3.4/src/drv/mainPpDrv/h
INCLUDES += -I$(TOPSRC_DIR)/npdsuit_mv/mcpss3.4/src/drv/common/h
INCLUDES += -I$(TOPSRC_DIR)/npdsuit_mv/mcpss3.4/src/drv/cpssEnabler/mainOs/h
endif

ifeq ($(DRV_LIB_FLAG),CPSS_3_4)
#DLE_HOST is not need in our CPU zhangdi 2011-03-11
#CFLAGS += -DDRV_LIB_CPSS -DDRV_LIB_CPSS_XCAT -DLE_HOST
CFLAGS += -DDRV_LIB_CPSS -DDRV_LIB_CPSS_3_4
INCLUDES += -I$(TOPSRC_DIR)/npdsuit_mv/mcpss3.4/src/drv/mainPpDrv/h
INCLUDES += -I$(TOPSRC_DIR)/npdsuit_mv/mcpss3.4/src/drv/common/h
INCLUDES += -I$(TOPSRC_DIR)/npdsuit_mv/mcpss3.4/src/drv/cpssEnabler/mainOs/h
endif

ifeq ($(DRV_LIB_FLAG),BCM)
CFLAGS += -DDRV_LIB_BCM -ansi
INCLUDES += -I$(BCM_TOPDIR)/include

# add for L3 include 
CFLAGS += -DINCLUDE_L3
# add for BCM_DRV_SHLIB
CFLAGS += -D__BCM_DRV_SHLIB__
endif

# define socket type for RSTP
CFLAGS += -DRSTP_SOCKET_DGRAM

# define socket type for IGMP Snooping
CFLAGS += -DIGMP_SNOOP_SOCKET_STREAM

LDFLAGS = -g -shared -Wl,-soname,$(NPD_AM_SHLIB_SONAME)



INCLUDES += -I$(ACCAPI_DIR) 

NPD_AM_OBJS = nam_eth.o nam_qos.o nam_asic.o nam_vlan.o nam_trunk.o nam_dynamic_trunk.o nam_arp.o nam_utilus.o nam_fdb.o nam_intf.o nam_pvlan.o nam_acl.o nam_log.o nam_mirror.o nam_rx.o nam_tx.o nam_stp.o nam_route.o nam_diag.o nam_tunnel.o nam_dldp.o nam_wifi.o nam_prot_vlan.o nam_hw_diag.o

#ifeq ($(DRV_LIB_FLAG),BCM)
#NPD_AM_OBJS += nam_stp.o
#endif

NPD_AM_SHLIB_MAJOR = 0
NPD_AM_SHLIB_MINOR = 1


NPD_AM_SHLIB_LINKERNAME = libnam.so
NPD_AM_SHLIB_SONAME = $(NPD_AM_SHLIB_LINKERNAME).$(NPD_AM_SHLIB_MAJOR)
NPD_AM_SHLIB_REALNAME = $(NPD_AM_SHLIB_SONAME).$(NPD_AM_SHLIB_MINOR)

.PHONY: all
all: $(NPD_AM_SHLIB_REALNAME)

$(NPD_AM_SHLIB_REALNAME): $(NPD_AM_OBJS)
	rm -rf $(LIB_EXPORT_DIR)/$(NPD_AM_SHLIB_REALNAME) $(LIB_EXPORT_DIR)/$(NPD_AM_SHLIB_SONAME) $(LIB_EXPORT_DIR)/$(NPD_AM_SHLIB_LINKERNAME)
	$(CC) $(LDFLAGS) -o $@ $^
	mv $(NPD_AM_SHLIB_REALNAME) $(LIB_EXPORT_DIR)/
	ln -s $(NPD_AM_SHLIB_REALNAME)  $(LIB_EXPORT_DIR)/$(NPD_AM_SHLIB_SONAME)
	ln -s $(NPD_AM_SHLIB_SONAME)  $(LIB_EXPORT_DIR)/$(NPD_AM_SHLIB_LINKERNAME)

%.o: %.c
	$(RM) $@
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $*.o $<

.PHONY: clean
clean:
	rm -rf *~ $(NAM_ROOT_PATH)/export/lib/$(NPD_AM_SHLIB_REALNAME) $(NAM_ROOT_PATH)/export/lib/$(NPD_AM_SHLIB_SONAME) $(NAM_ROOT_PATH)/export/lib/$(NPD_AM_SHLIB_LINKERNAME) $(NPD_AM_OBJS)

