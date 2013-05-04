#CC  = mips-linux-gnu-gcc

CFLAGS = -g -Wall #-ansi

LDFLAGS = -g -L$(LIB_EXPORT_DIR) -L$(BUILDROOTDIR)/usr/lib

LDLIBS = -lpthread -lsqlite3


INCLUDES += -I$(ACCAPI_DIR)

INCLUDES += -I/opt/mipslibs/usr/include


CVM_RATELIMIT_OBJS = cvm_rate_limit_dbus.o


.PHONY: all
all: cvm_rate 

cvm_rate: $(CVM_RATELIMIT_OBJS)
	$(CC) $(LDFLAGS) $(LDLIBS) $(DBUS_LIBS) -o $@ $^ 
	#cp cvm_rate $(BIN_EXPORT_DIR)
	cp cvm_rate $(BIN_EXPORT_DIR)/


%.o: %.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) $(INCLUDES) -o $*.o $<

	

.PHONY: clean
clean:
	rm -rf *~ $(CVM_RATELIMIT_OBJS) cvm_rate

