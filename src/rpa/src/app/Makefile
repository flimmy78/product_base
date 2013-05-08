
CC = mips-linux-gnu-gcc

LDFLAGS = -lpthread -ldl
LDFLAGS += -L/opt/mipslibs/usr/lib 
LDFLAGS += -lsctp

CFLAGS = -Wall -Werror -g -O0 -I. 
CFLAGS += -I$(ACCAPI_PB_DIR)
CFLAGS += -I/opt/mipslibs/usr/include
#CFLAGS += -I../../../accapi
CFLAGS += -I/opt/mipslibs/usr/include/libxml2

LIBS = -L/opt/mipslibs/usr/lib -L./  -lm -L/opt/mipslibs/usr/lib -lxml2 -lz

DBUS_INCLUDE=-I/opt/mipslibs/usr/include/dbus-1.0 -I/opt/mipslibs/usr/lib/dbus-1.0/include
DBUS_LIBS=-ldbus-1


RM = /bin/rm -f 

# list of generated object files for AC. 
RPA_OBJS = rpa-daemon.o
 
RPA_SRCS = $(RPA_OBJS:.o=.c) 

RPA_DEPS := $(RPA_OBJS:.o=.d)

# program executables. 
RPA_DAEMON = rpa-daemon

.PHONY: deps clean clean_libs libs

# top-level rule, to compile everything. 
all: $(RPA_DAEMON) 

$(RPA_DAEMON): $(RPA_OBJS) 
	$(CC) $(RPA_OBJS) $(LDFLAGS) $(LDLIBS)  $(DBUS_LIBS) $(LIBS) -o $(RPA_DAEMON) 
%.o: %.c
	$(CC) -c $(CFLAGS) $(DBUS_INCLUDE) $(INCLUDES) -o $*.o $< -D_GNU_SOURCE
clean: 
	$(RM) $(RPA_DAEMON) $(RPA_OBJS) $(RPA_DEPS) 

clean_deps:
	$(RPA_DEPS) 
	
deps: $(RPA_SRC) 
	$(CC) -MD -E $(RPA_SRCS) $(CFLAGS) >/dev/null

-include $(RPA_DEPS)
