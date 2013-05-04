CC = mips-linux-gnu-gcc

LDFLAGS += -L../pub -lpthread -L$(MIPS_ADDITION_LIB_ROOT_PATH)/usr/lib 
#-lpthread -ldl

RM = /bin/rm -f

BOARD_OBJS = ax71_2x12g12s.o ax71_crsmu.o ax81_crsmu.o

board : $(BOARD_OBJS)
	$(CC) $(BOARD_OBJS) $(CFLAGS) $(DBUS_LIBS) $(LDFLAGS) -o board 

%.o: %.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) -o $*.o $<

.PHONY: clean

clean: 
	$(RM) board $(BOARD_OBJS)
