SHELL = /bin/bash

CCFLAGS = -D_DISTRIBUTION_ -D_VERSION_18SP8_
DBUS_LIBS = -ldbus-1


INCLUDES = -I$(QUAGGA_DIR)/${BUILDDIR} -I$(QUAGGA_DIR)/debsrc/quagga-0.99.5 -I$(QUAGGA_DIR)/debsrc/quagga-0.99.5/lib 
INCLUDES += -I$(ACCAPI_DIR) -I$(BUILDROOTDIR)/usr/include -I$(BUILDROOTDIR)/usr/include/dbus-1.0 -I$(BUILDROOTDIR)/usr/lib/dbus-1.0/include
INCLUDES += -I../../../../dg/dcli/src/lib/
INCLUDES += -I../../../../sem/src/dcli/


DRPLIB_PATH = -L$(LIB_EXPORT_DIR)
DRPLIB_FLAGS = -ldrpintf
LIB_DCLI = $(LIB_EXPORT_DIR)/libdcli.so.0.1
LDFLAGS = -shared -Wl,-soname,$@
#-lxml2 -ldcliiuhcli -ldcliiuhmapi -ldclipub -lrt...... -lrt   -lnm -lz -lpam

LD_PATH = -L${ROOTFS_DIR}/files/lib -L$(BUILDROOTDIR)/usr/lib 
DCLI_MAIN_PATH = ../../../../dg/dcli/src/lib/


DCLI_SHLIB_MAJOR = 0
DCLI_SHLIB_MINOR = 1
DCLI_OBJS = dcli_diag.o dcli_vlan.o dcli_trunk.o dcli_dynamic_trunk.o dcli_fdb.o \
			dcli_qos.o dcli_acl.o dcli_stp.o dcli_common_stp.o dcli_routesyn.o dcli_tunnel.o\
			dcli_pvlan.o dcli_prot_vlan.o dcli_igmp_snp.o dcli_mld_snp.o dcli_common_mld_snp.o\
			dcli_common_igmp_snp.o dcli_mirror.o
DCLI_SHLIB_LINKERNAME = libdcli_npd.so
DCLI_SHLIB_SONAME = $(DCLI_SHLIB_LINKERNAME).$(DCLI_SHLIB_MAJOR)
DCLI_SHLIB_REALNAME = $(DCLI_SHLIB_SONAME).$(DCLI_SHLIB_MINOR)


.PHONY: all
all: libdcli_main  $(DCLI_SHLIB_REALNAME)

libdcli_main:
	$(MAKE) -C ${DCLI_MAIN_PATH}
	
$(DCLI_SHLIB_REALNAME): $(DCLI_OBJS)
	$(CC) ${LDFLAGS} $(LD_PATH) -o $@ $^ $(DBUS_LIBS) $(CCFLAGS) $(LIB_DCLI)
	mv $(DCLI_SHLIB_REALNAME) $(LIB_EXPORT_DIR)/
	ln -sf $(DCLI_SHLIB_REALNAME) $(LIB_EXPORT_DIR)/$(DCLI_SHLIB_SONAME) 
	ln -sf $(DCLI_SHLIB_SONAME) $(LIB_EXPORT_DIR)/$(DCLI_SHLIB_LINKERNAME) 	
	
				 

%.o: %.c
	${RM} $@
	$(CC) -c $(CCFLAGS) $(INCLUDES)  -o $*.o $<	

# $(COMFLAGS) $(CFLAGS) $(CGIFLAGS) $(XML_LIBS) $(XMLFLAGS) $(EAGFLAGS)

.PHONY: clean
clean: 
	$(RM) *.o $(DCLI_SHLIB_LINKERNAME) $(DCLI_SHLIB_SONAME) $(DCLI_SHLIB_REALNAME)


	
	
