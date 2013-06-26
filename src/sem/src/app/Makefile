OBJS = sem_netlink.o tipc/sem_tipc.o product/board/board_common.o product/board/ax71_crsmu.o product/board/ax81_smu.o product/board/ax81_smue.o\
	   product/board/ax71_2x12g12s.o product/board/ax81_ac12c.o product/board/ax81_ac8c.o product/board/ax81_2x12g12s.o \
	   product/board/ax81_12x.o product/board/ax81_ac4x.o\
	   product/board/ax81_1x12g12s.o product/board/ax71_1x12g12s.o product/product_ax7605i.o product/product_ax8610.o product/product_ax8606.o  \
	   product/product_ax8603.o product/product_ax8800.o product/product_single_board.o\
	   sem_common.o sem_log.o sem_board_init.o sem_product_init.o sem_dbus.o sem_main.o sem_eth_port.o sem_eth_port_dbus.o \
	   sem_system_cmd_dbus.o sem_intf.o sem_fpga.o

LDFLAGS += -L../pub  -lpthread -L$(MIPS_ADDITION_LIB_ROOT_PATH)/usr/lib

#LDFLAGS += -lpthread
CFLAGS += -DCW_NO_DTLS -I. -g -Werror -I../pub -DWRITE_STD_OUTPUT -DCW_DEBUGGING -D_GNU_SOURCE
CFLAGS += -I$(ACCAPI_PB_DIR)
CFLAGS += -I/opt/mipslibs/usr/include/libxml2

CC = mips-linux-gnu-gcc

sem:$(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(DBUS_LIBS) $(OBJS) -o sem
	cp sem $(BIN_EXPORT_DIR)/
	$(CC) reset_if_statistics.c -o reset_if_statistics
	cp reset_if_statistics $(BIN_EXPORT_DIR)
	#cp sem ~/
	mips-linux-gnu-gcc config_slot_x/tipc_api.c -I ./config_slot_x -shared -fPIC -L$(ROOTFS_DIR)/files/lib -lpthread -lzebra -o config_slot_x/libtipc_api.so
	cp config_slot_x/libtipc_api.so $(ROOTFS_DIR)/files/lib/
	mips-linux-gnu-gcc config_slot_x/cli_tunnel_server.c -I ./config_slot_x -L$(ROOTFS_DIR)/files/lib -lzebra -ltipc_api -lnpdlib -lutil -o config_slot_x/cli_tunnel_server
	cp config_slot_x/cli_tunnel_server $(ROOTFS_DIR)/files/usr/bin/
	mips-linux-gnu-gcc config_slot_x/cli_tunnel_client.c -I ./config_slot_x -L$(ROOTFS_DIR)/files/lib -lzebra -ltipc_api -lnpdlib -lutil -o config_slot_x/cli_tunnel_client
	cp config_slot_x/cli_tunnel_client $(ROOTFS_DIR)/files/usr/bin/

sem_dbus.o:sem_dbus.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) sem_dbus.c -o sem_dbus.o

sem_eth_port_dbus.o:sem_eth_port_dbus.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) sem_eth_port_dbus.c -o sem_eth_port_dbus.o

sem_system_cmd_dbus.o:sem_system_cmd_dbus.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) sem_system_cmd_dbus.c -o sem_system_cmd_dbus.o
	
sem_fpga.o:sem_fpga.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) sem_fpga.c -o sem_fpga.o

sem_intf.o:sem_intf.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) sem_intf.c -o sem_intf.o

sem_log.o:sem_log.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) sem_log.c -o sem_log.o

sem_common.o:sem_common.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) sem_common.c -o sem_common.o

sem_board_init.o:sem_board_init.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) sem_board_init.c -o sem_board_init.o

sem_product_init.o:sem_product_init.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) sem_product_init.c -o sem_product_init.o

sem_main.o:sem_main.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) sem_main.c -o sem_main.o

.PHONY:clean
clean:
	rm sem *.o product/*.o product/board/*.o tipc/*.o
	rm config_slot_x/cli_tunnel_server config_slot_x/cli_tunnel_client config_slot_x/libtipc_api.so
	rm reset_if_statistics
