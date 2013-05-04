#
#Makefile for the sem kernel module
#

KERNEL_DIR = ../kernel2.6.32.27cn

default:all

obj-m      := kernel-sem.o

kernel-sem-objs   := ksem.o 
kernel-sem-objs   += ksem_common.o 

kernel-sem.o : $(kernel-sem-objs)
	$(LD) -r $^ -o $@
all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=`pwd` modules
	
clean:
	rm -rf *.o *.ko *.mod.c
