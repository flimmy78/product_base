# Network Platform Daemon 
#
#CC  = mips-linux-gnu-gcc

CFLAGS = -g -Wall  #-ansi
#LDFLAGS = -g -L$(LIB_EXPORT_DIR) -L$(MIPS_ADDITION_LIB_ROOT_PATH)/usr/lib 
LDFLAGS = -g -L$(LIB_EXPORT_DIR) -L$(BUILDROOTDIR)/usr/lib
LDLIBS = -lnam -lnbm -lpthread -lsqlite3
# add for dhcp snooping db
LDLIBS += -lsqlite3

# ASIC driver type Broadcom
ifeq ($(DRV_LIB_FLAG),BCM)
CFLAGS += -ansi
LDLIBS += -lmbcm
endif

# ASIC driver type Marvell
ifeq ($(DRV_LIB_FLAG),CPSS_3_4)
CFLAGS +=  -ansi
LDLIBS += -lmcpss
endif

# debug for mcpss=1 in root makefile, zhangdi
ifeq ($(DRV_LIB_FLAG),CPSS)
CFLAGS +=  -ansi
LDLIBS += -lmcpss
endif

ifeq ($(DRV_LIB_FLAG),CPSS_XCAT)
CFLAGS += -DCPU_ARM_XCAT
LDFLAGS += -L/opt/arm-none-linux-gnueabi/arm-none-linux-gnueabi/sys-root/usr/lib 
LDLIBS += -lmcpss -lrt
endif

#CFLAGS += -DDBUS_API_SUBJECT_TO_CHANGE
INCLUDES += -I$(ACCAPI_PB_DIR)
INCLUDES += -I$(DCCN_TOPSRC)/src/include

# add for dhcp snooping db
INCLUDES += -I/opt/mipslibs/usr/include

#DBUS_INCLUDES = $(shell pkg-config --cflags dbus-1)
#DBUS_LIBS = $(shell pkg-config --libs dbus-1)

C_FLAGS = -g -Wall  #-ansi
C_FLAGS += -lpthread
C_FLAGS += -I$(ACCAPI_PB_DIR)
C_FLAGS += -I/opt/mipslibs/usr/include/libxml2

NPD_OBJS = npd_main.o npd_dbus.o npd_log.o npd_product.o npd_c_slot.o npd_e_slot.o npd_vlan.o npd_trunk.o \
			npd_eth_port.o npd_engine.o npd_rstp_common.o npd_acl_drv.o npd_acl.o npd_qos.o npd_vlan.o npd_trunk.o npd_dynamic_trunk.o \
			npd_intf.o npd_hash.o npd_fdb.o npd_arpsnooping.o npd_route.o npd_igmp_snp_com.o npd_pvlan.o npd_mirror.o npd_diag.o npd_tunnel.o \
			npd_dldp_common.o npd_vrrp.o npd_dhcp_snp_com.o npd_dhcp_snp_if.o npd_dhcp_snp_options.o npd_dhcp_snp_pkt.o npd_dhcp_snp_tbl.o \
			npd_dhcp_snp_sqlite.o npd_mld_snp_com.o npd_prot_vlan.o npd_board.o npd_board_ax71_2x12g12s.o npd_board_ax81_ac12c.o \
			npd_board_ax81_1x12g12s.o npd_board_ax81_ac8c.o npd_board_ax81_smu.o npd_hw_diag.o npd_board_ax81_12x.o npd_board_ax81_smue.o \
			npd_board_ax81_ac4x.o npd_board_axxx_virtualboard.o
#do not nedd npd_dbm.o(not dir dccnetlink)
NPD_OBJS_EX = npd_tipc.o npd_communication.o npd_netlink.o

.PHONY: all
all: npd 

npd: $(NPD_OBJS) $(NPD_OBJS_EX)
	$(CC) $(LDFLAGS) $(LDLIBS) $(DBUS_LIBS) -o $@ $^
	mv npd $(BIN_EXPORT_DIR)/

%.o: %.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) $(INCLUDES) -o $*.o $<

npd_tipc.o: npd_tipc.c
	$(CC) -c $(C_FLAGS) $(DBUS_INCLUDE) -o $*.o $<

npd_netlink.o: npd_netlink.c
	$(CC) -c $(C_FLAGS) $(DBUS_INCLUDE) -o $*.o $<

npd_communication.o: npd_communication.c
	$(CC) -c $(C_FLAGS) $(DBUS_INCLUDE) -o $*.o $<

.PHONY: clean
clean:
	rm -rf *~ $(NPD_OBJS) $(NPD_OBJS_EX) npd

