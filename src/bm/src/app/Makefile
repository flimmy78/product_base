CC = mips-linux-gnu-gcc
AR = mips-linux-gnu-ar
RANLIB = mips-linux-gnu-ranlib
KMOD_DIR = ../kmod
INCLUDES = -I. -I ${KMOD_DIR} -I${ACCAPI_DIR}
CFLAGS = -g -Wall -D__LINUX__ $(INCLUDES)

#Set LANG to POSIX to display traditional simple english instead of other languages, avoid decode error in non UTF8 application.
LANG=POSIX

all: clean bmutil
#	mv bmutil ../../exec

clean:
	rm -rf bmutil *.o *~
#	rm -f ../../exec/bmutil

bmutil:	bmutil.o
	$(CC)  -o $@ $?
