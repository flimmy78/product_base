OBJS = npd_tipc.o  main.o #npd_communication.o

LDFLAGS += -L../pub  -lpthread -L$(MIPS_ADDITION_LIB_ROOT_PATH)/usr/lib 
CFLAGS += -DCW_NO_DTLS -I. -g -I../pub -DWRITE_STD_OUTPUT -DCW_DEBUGGING -D_GNU_SOURCE
CFLAGS += -I$(ACCAPI_DIR)
CFLAGS += -I/opt/mipslibs/usr/include/libxml2

CC = mips-linux-gnu-gcc

test_tipc:$(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(DBUS_LIBS) $(OBJS) -o test_tipc
	#cp test_tipc ~/
	
main.o:main.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) main.c -o main.o

.PHONY:clean
clean:
	rm test_tipc main.o
