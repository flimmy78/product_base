# 
# This file intends for make software system 

export MAKE=make

export PATH=/usr/local/bin:/usr/bin:/usr/sbin:/sbin:/bin
# Add new branch for mcpss3.4
MCPSSPATH=mcpss3.4
CPSS_FLAG=CPSS_3_4	
export AUTEWAREFS_DIR=${PWD}/auteware

ifdef xcat
export COMPILER_PREFIX=arm-none-linux-gnueabi
export XCATBUILDROOTDIR=/opt/xcat/buildroot/sysroot
export XCATBUSYROOTDIR=/opt/xcat/busyroot/sysroot
export BUILDROOTDIR=${XCATBUILDROOTDIR}
export BUSYROOTDIR=${XCATBUSYROOTDIR}
export BUILDDIR=xcatbuild
export PATH:=/opt/arm-none-linux-gnueabi/bin/:${PATH}
export ROOTFS_DIR=${PWD}/xcat-rootfs
export XCAT_ROOTFS=${ROOTFS_DIR}/files
export KERNEL_ROOT=${PWD}/src/xcat-kernel2.6.22.18
export ROOTFS_KMOD_DIR=${XCAT_ROOTFS}/lib/modules/2.6.22.18
export CPU_ARCH_FLAG=arm
DBUS_INCLUDE=-I${AUTEWAREFS_DIR}/files/opt/lib -I${BUILDROOTDIR}/usr/include/dbus-1.0 -I${BUILDROOTDIR}/usr/lib/dbus-1.0/include
export DBUS_INCLUDE
DBUS_LIBS=-ldbus-1
export DBUS_LIBS
DHCP_CONFIG_HOSTCC=arm-none-linux-gnueabi
else
MIPS_ADDITION_LIB_ROOT_PATH=/opt/mipslibs
export MIPS_ADDITION_LIB_ROOT_PATH
DBUS_INCLUDE=-I/opt/mipslibs/usr/include/dbus-1.0 -I/opt/mipslibs/usr/lib/dbus-1.0/include
export DBUS_INCLUDE
DBUS_LIBS=-L${AUTEWAREFS_DIR}/files/opt/lib -ldbus-1
export DBUS_LIBS
#export CFLAGS+=-I${MIPS_ADDITION_LIB_ROOT_PATH}/usr/include 
DHCP_CONFIG_HOSTCC=mips-linux-gnu

ifdef octeon
export OCT_ROOTFS_DIR=/data/home/chenbin/cvshome/busyroot/sysroot
else
export COMPILER_PREFIX=mips-linux-gnu
export BUILDROOTDIR=${MIPS_ADDITION_LIB_ROOT_PATH}
export BUILDDIR=mipsbuild
export DBUS_EXEC_PREFIX=${PWD}/rootfs/files/usr
export DBUS_PATH:=$(shell if [ -d src/dbus-1.4.0 ] ; then echo "`pwd`/src/dbus-1.4.0" ; else echo ${OCT_ROOTFS_DIR} ; fi)

export ROOTFS_DIR:=$(shell if [ -d rootfs ] ; then echo "`pwd`/rootfs" ; else echo ${OCT_ROOTFS_DIR} ; fi)
export OCT_ROOTFS_DIR=${ROOTFS_DIR}
endif

ifndef NEWKERN
$(info NEWKERN not defined)
export NEWKERN=$(shell \
if [ -d src/kernel2.6.21.7cn3 ] && [ ! -d src/kernel2.6.16.26cn ] && [ ! -d src/kernel2.6.21.7oct1.8.1 ] && [ ! -d src/kernel2.6.32.13cn ] ; then echo "1"; \
elif  [ ! -d src/kernel2.6.21.7cn3 ] && [ -d src/kernel2.6.16.26cn ] && [ ! -d src/kernel2.6.21.7oct1.8.1 ] && [ ! -d src/kernel2.6.32.13cn ] ; then echo "0"; \
elif  [ ! -d src/kernel2.6.21.7cn3 ] && [ ! -d src/kernel2.6.16.26cn ] && [ -d src/kernel2.6.21.7oct1.8.1 ] && [ ! -d src/kernel2.6.32.13cn ] ; then echo "2"; \
elif  [ ! -d src/kernel2.6.21.7cn3 ] && [ ! -d src/kernel2.6.16.26cn ] && [ ! -d src/kernel2.6.21.7oct1.8.1 ] && [ -d src/kernel2.6.32.13cn ] ; then echo "3"; \
elif  [ -d src/kernel2.6.32.27cn ] ; \
then echo "4"; fi)
endif

ifeq (${NEWKERN},4)
export OCTEON_ROOT=/opt/CN/SDK2.2
export KERNEL_ROOT:=$(shell if [ -d src/kernel2.6.32.27cn ] ; then echo "`pwd`/src/kernel2.6.32.27cn" ; else echo ${OCT_KERNEL_ROOT} ; fi)
export ROOTFS_KMOD_DIR=${ROOTFS_DIR}/files/lib/modules/2.6.32.27-Cavium-Octeon
else ifeq (${NEWKERN},3)
export OCTEON_ROOT=/opt/CN/SDK2.0
export KERNEL_ROOT:=$(shell if [ -d src/kernel2.6.32.13cn ] ; then echo "`pwd`/src/kernel2.6.32.13cn" ; else echo ${OCT_KERNEL_ROOT} ; fi)
export ROOTFS_KMOD_DIR=${ROOTFS_DIR}/files/lib/modules/2.6.21.7-Cavium-Octeon
else ifeq (${NEWKERN},2)
export OCTEON_ROOT=/opt/CN/SDK1.8.1
export KERNEL_ROOT:=$(shell if [ -d src/kernel2.6.21.7oct1.8.1 ] ; then echo "`pwd`/src/kernel2.6.21.7oct1.8.1" ; else echo ${OCT_KERNEL_ROOT} ; fi)
export ROOTFS_KMOD_DIR=${ROOTFS_DIR}/files/lib/modules/2.6.21.7-Cavium-Octeon
else ifeq (${NEWKERN},1)
export OCTEON_ROOT=/opt/CN/SDK1.7.3
export KERNEL_ROOT:=$(shell if [ -d src/kernel2.6.21.7cn3 ] ; then echo "`pwd`/src/kernel2.6.21.7cn3" ; else echo ${OCT_KERNEL_ROOT} ; fi)
export ROOTFS_KMOD_DIR=${ROOTFS_DIR}/files/lib/modules/2.6.21.7-Cavium-Octeon
export CFLAGS += 
else 
export OCTEON_ROOT=/opt/CN/OCTEON-SDK
export KERNEL_ROOT:=$(shell if [ -d src/kernel2.6.16.26cn ] ; then echo "`pwd`/src/kernel2.6.16.26cn" ; else echo ${OCT_KERNEL_ROOT} ; fi)
export ROOTFS_KMOD_DIR=${ROOTFS_DIR}/files/lib/modules/2.6.16.26-Cavium-Octeon
export CFLAGS += -g
endif

export KERNEL_DIR=${KERNEL_ROOT}
export OCT_KERNEL_ROOT=${KERNEL_ROOT}
#export OCTEON_CPPFLAGS_GLOBAL_ADD= -DUSE_RUNTIME_MODEL_CHECKS=1 -DCVMX_ENABLE_PARAMETER_CHECKING=0 -DCVMX_ENABLE_CSR_ADDRESS_CHECKING=0 -DCVMX_ENABLE_POW_CHECKS=0

export PATH:=${OCTEON_ROOT}/tools/bin:${OCTEON_ROOT}/host/bin:${PATH}

export OCTEON_MODEL=OCTEON_CN38XX_PASS3
export XML_LIB_PATH = ${PWD}/rootfs/files/usr/lib

$(info OCTEON_ROOT = ${OCTEON_ROOT})
$(info KERNEL_ROOT = ${KERNEL_ROOT})

endif

export AR=${COMPILER_PREFIX}-ar
export RANLIB=${COMPILER_PREFIX}-ranlib
export OBJDUMP=${COMPILER_PREFIX}-objdump
export STRIP=${COMPILER_PREFIX}-strip
export SIZE=${COMPILER_PREFIX}-size
export CC=${COMPILER_PREFIX}-gcc
export CXX=${COMPILER_PREFIX}-g++
export LD=${COMPILER_PREFIX}-ld
export NM=${COMPILER_PREFIX}-nm




export BUILD_DIR=${shell pwd}
export EXPORT_OPT_DIR=${BUILD_DIR}/auteware/files/opt
export LIB_EXPORT_DIR=${BUILD_DIR}/auteware/files/opt/lib
export BIN_EXPORT_DIR=${BUILD_DIR}/auteware/files/opt/bin
#export ETC_EXPORT_DIR=${BUILD_DIR}/auteware/files/opt/etc
export WWW_EXPORT_DIR=${BUILD_DIR}/auteware/files/opt/www

export KMOD_EXPORT_DIR=${BUILD_DIR}/auteware/files/kmod

export TOPSRC_DIR=${BUILD_DIR}/src



export MOTDFILE=${AUTEWAREFS_DIR}/files/etc/motd
export BUILDERFILE=${ROOTFS_DIR}/files/etc/version/builder
export VERFILE=${ROOTFS_DIR}/files/etc/version/version
export GITHASH=${ROOTFS_DIR}/files/etc/version/githash
export NAMFILE=${ROOTFS_DIR}/files/etc/version/name
export PRODUCTFILE=./products
export BUILDNOFILE=./buildno_v2.0
export AWNAME=$(shell echo "AW`cat ${VERFILE}`.`cat ${BUILDNOFILE}`.`cat ${PRODUCTFILE}`")
export IMGDIR=imgdir

version=$(shell echo "`cat ${VERFILE}`")
ifeq ($(version), 2.0.18sp7)
export AWVERSION=VERSION_18SP7
else ifeq ($(version), 2.0.18sp8)
export AWVERSION=VERSION_18SP8
endif

$(shell git log | head -n 1 | awk '{ print $$2 }' > ${GITHASH})

$(shell \
	if [ -d .git ] ; then \
		cat ${VERFILE} > ${IMGDIR} ; \
	elif [ ! -e ${IMGDIR} ] ; then \
		echo "`date +%Y%m%d`" > ${IMGDIR};  \
	elif [ `cat ${IMGDIR}` != `cat ${VERFILE}` ]; then \
		echo "`date +%Y%m%d`" > ${IMGDIR}; \
	fi )
export DESTDIR=$(shell echo "/srv/tftp/`cat ${IMGDIR}`")

$(info =========================================================================)
$(info ROOTFS_DIR = ${ROOTFS_DIR})
$(info PATH is ${PATH} )
$(info CFLAGS=${CFLAGS} )
$(info DESTDIR is ${DESTDIR})
$(info VERSION is $(AWVERSION))
$(info =========================================================================)

#export IBUS_MOD="src/ibus"
export ACCAPI_DIR=${TOPSRC_DIR}/dg/accapi
export ACCAPI_PB_DIR=${TOPSRC_DIR}/accapi_pb
export BASHTOOLS_DIR=src/dg/bashtools
export NPDSUIT_MV_MOD=src/npdsuit_mv
export SEM_MOD=src/sem
#export XCAT_BM_MOD="src/xcatbm"
export NPDSUIT_BCM_MOD=src/npdsuit_bcm
export NPDSUIT_XCAT_MOD=src/npdsuit_xcat
export KSEM_MOD=src/ksem
export QUAGGA_MOD=src/dg/quagga
export DCLI_MOD=src/dg/dcli
export BM_KMOD=src/bm
export KES_MOD=src/kes/
export OCTETH_KMOD=src/cavium-ethernet
export RPA_MOD=src/rpa
export PFM_MOD=src/pfm
export SFD_MOD=src/dg/sfd
export E1000E_KMOD=src/e1000e/src
export OCTEON_BOOTDIR="src/oct-nic"
export DBA_MOD="src/dg/dba/"
export PPPOE_KERNEL_MOD="src/dg/pppoe/kernel"
export PPPOE_MOD="src/dg/pppoe"
export DPI_KMOD="src/intercept-example"
export CAVIUM_RATELIMIT_APP="src/cavium_ratelimit/app"
export CAVIUM_RATELIMIT_MOD="src/cavium_ratelimit/kmod"
export WCPSS_MOD="src/dg/wcpss"
export IUH_MOD="src/dg/ibus/app/ranapproxy/iuh"
export IU_MOD="src/dg/ibus/app/ranapproxy/iu"
export SCCP_LIB_MOD="src/dg/ibus/lib/sccp/lib"
export WTPVERFILE=${WCPSS_MOD}/src/res/wtpcompatible*
export ASD_MOD="src/dg/asd"
export STPSUIT_MOD="src/dg/stpsuit"
export HAD_MOD="src/dg/had"
export HMD_MOD="src/dg/hmd"
export BSD_MOD="src/dg/bsd"
export WBMD_MOD="src/dg/wbmd"
export HBIP_MOD="src/dg/hbip"
export IGMP_MOD="src/dg/igmp-snooping"
export DLDP_MOD="src/dg/dldp"
export CCGI_MOD="src/dg/AuteCS"
export SRVM_MOD="src/dg/service_management"
export CHKPWD_MOD="src/dg/checkpasswd-pam"
export IPTABLES_MOD=src/dg/iptables
export EBTABLES_MOD="src/dg/ebtables"
export CAPTIVE_MOD="src/dg/captive_portal"
export EAG_MOD="src/dg/eag"
export BCM_MOD=${NPDSUIT_BCM_MOD}/bcmd
export BCM_TOPDIR=${TOPSRC_DIR}/npdsuit_bcm/bcmd
export SNMP_ROOTDIR=src/dg/net-snmp
export SNMPMIBS_DIR=${SNMP_ROOTDIR}/mibs
export SNMPMANAGE_MOD=${SNMP_ROOTDIR}/manage
export TRAP_HELPER_MOD=${SNMP_ROOTDIR}/trap-helper
export SUBAGENT_MOD=${SNMP_ROOTDIR}/subagent
export ACSAMPLE_MOD=${SNMP_ROOTDIR}/sample
export DRP_MOD=${SNMP_ROOTDIR}/drp
export LIBNM_MOD="src/dg/lib/nm"
export DHCP_MOD="src/dg/dhcp4/dhcp-4.0.2b3"
export DHCP6_MOD="src/dg/dhcp4/dhcp-4.1.1"
export DCCN_MOD="src/dccnetlink"
export DCCN_TOPSRC=${TOPSRC_DIR}/dccnetlink
export PIMD_MOD="src/dg/pimd"
export DHCPSNP_MOD="src/dg/dhcp-snooping"
export RADIUSD_MOD="src/dg/radiusd"
export IPFWD_LEARN_KMOD="src/fast-forward/control_plane/ipfwd_learn"
export SE_AGENT_MOD="src/fast-forward/control_plane/se_agent"
export FASTFWD_MOD="src/fast-forward/data_plane/fast-fwd"
export DBUS_MOD="src/dg/dbus-1.4.0"
export SYSLOG_MOD="src/dg/syslog-ng"

PREREQ_DCLI=$(shell if [ -d ${DCLI_MOD} ] ; then echo "dcli"; fi )
#$(warning Dcli is $(PREREQ_DCLI))
CLEAN_DCLI=$(subst dcli,cleandcli,${PREREQ_DCLI})
#$(warning Clean Dcli is ${CLEAN_DCLI} or ${PREREQ_DCLI})
PREREQ_ASD=$(shell if [ -d ${ASD_MOD} ] ; then echo "asd"; fi)
CLEAN_ASD=$(subst asd,cleanasd,${PREREQ_ASD})

ifneq (${NEWKERN},1)
PREREQ_OCTETHKMOD=$(shell if [ -d ${OCTETH_KMOD} ] ; then echo "cavium-ethernet"; fi)
CLEAN_OCTETHKMOD=$(subst cavium-ethernet,cleancavium-ethernet,${PREREQ_OCTETHKMOD})
else ifneq (${NEWKERN},3)
PREREQ_OCTETHKMOD=$(shell if [ -d ${OCTETH_KMOD} ] ; then echo "cavium-ethernet"; fi)
CLEAN_OCTETHKMOD=$(subst cavium-ethernet,cleancavium-ethernet,${PREREQ_OCTETHKMOD})
else ifneq (${NEWKERN},4)
PREREQ_OCTETHKMOD=$(shell if [ -d ${OCTETH_KMOD} ] ; then echo "cavium-ethernet"; fi)
CLEAN_OCTETHKMOD=$(subst cavium-ethernet,cleancavium-ethernet,${PREREQ_OCTETHKMOD})
else 
MAKE_KERNEL_BEFORE_MODULES=kernel
endif
PREREQ_RPA=$(shell if [ -d ${RPA_MOD} ] ; then echo "rpa"; fi)
CLEAN_RPA=$(subst rpa,cleanrpa,${PREREQ_RPA})
PREREQ_PFM=$(shell if [ -d ${PFM_MOD} ] ; then echo "pfm_deamon"; fi)
CLEAN_PFM=$(subst pfm,cleanpfm,${PREREQ_PFM})
PREREQ_SFD=$(shell if [ -d ${SFD_MOD} ] ; then echo "sfd_daemon"; fi)
CLEAN_SFD=$(subst sfd,cleansfd,${PREREQ_SFD})

PREREQ_BM=$(shell if [ -d ${BM_KMOD} ] ; then echo "bm"; fi)
CLEAN_BM=$(subst bm,cleanbm,${PREREQ_BM})
PREREQ_SEM=$(shell if [ -d ${SEM_MOD} ] ; then echo "sem"; fi)
CLEAN_SEM=$(subst sem,cleansem,${PREREQ_SEM})
PREREQ_SNMP=$(shell if [ -d ${SNMP_ROOTDIR} ] ; then echo "snmp"; fi)
CLEAN_SNMP=$(subst snmp,cleansnmp,${PREREQ_SNMP})

PREREQ_STP=$(shell if [ -d ${STPSUIT_MOD} ] ; then echo "stpsuit"; fi)
CLEAN_STP=$(subst stpsuit,cleanstpsuit,${PREREQ_STP})
PREREQ_HAD=$(shell if [ -d ${VRRP_MOD} ] ; then echo "had"; fi)
CLEAN_HAD=$(subst had,cleanhad,${PREREQ_HAD})
PREREQ_IUH=$(shell if [ -d ${IUH_MOD} ] ; then echo "iuh"; fi)
CLEAN_IUH=$(subst iuh,cleaniuh,${PREREQ_IUH})
PREREQ_RANAPPROXY=$(shell if [ -d ${IU_MOD} ] ; then echo "ranapproxy"; fi)
CLEAN_RANAPPROXY=$(subst ranapproxy,cleanranapproxy,${PREREQ_RANAPPROXY})
PREREQ_HMD=$(shell if [ -d ${HMD_MOD} ] ; then echo "hmd"; fi)
CLEAN_HMD=$(subst hmd,cleanhmd,${PREREQ_HMD})
PREREQ_WBMD=$(shell if [ -d ${WBMD_MOD} ] ; then echo "wbmd"; fi)
CLEAN_WBMD=$(subst wbmd,cleanwbmd,${PREREQ_WBMD})
PREREQ_BSD=$(shell if [ -d ${BSD_MOD} ] ; then echo "bsd"; fi)
CLEAN_BSD=$(subst bsd,cleanbsd,${PREREQ_BSD})
PREREQ_HBIP=$(shell if [ -d ${HBIP_MOD} ] ; then echo "hbip"; fi)
CLEAN_HBIP=$(subst hbip,cleanhbip,${PREREQ_HBIP})
PREREQ_IGMP=$(shell if [ -d ${IGMP_MOD} ] ; then echo "igmp"; fi)
CLEAN_IGMP=$(subst igmp,cleanigmp,${PREREQ_IGMP})
PREREQ_DLDP=$(shell if [ -d ${DLDP_MOD} ] ; then echo "dldp"; fi)
CLEAN_DLDP=$(subst dldp,cleandldp,${PREREQ_DLDP})
PREREQ_DCCN=$(shell if [ -d ${DCCN_MOD} ] ; then echo "dccn"; fi)
CLEAN_DCCN=$(subst dccn,cleandccn,${PREREQ_DCCN})
PREREQ_DCCNKMOD=$(shell if [ -d ${DCCN_MOD} ] ; then echo "dccnkmod"; fi)
CLEAN_DCCNKMOD=$(shell if [ -d ${DCCN_MOD} ] ; then echo "cleandccnkmod"; fi)
PREREQ_CCGI=$(shell if [ -d ${CCGI_MOD} ] ; then echo "ccgi"; fi)
CLEAN_CCGI=$(subst ccgi,cleanccgi,${PREREQ_CCGI})
PREREQ_SRVM=$(shell if [ -d ${SRVM_MOD} ] ; then echo "srvm"; fi)
CLEAN_SRVM=$(subst srvm,cleansrvm,${PREREQ_SRVM})
PREREQ_LIBNM=$(shell if [ -d ${LIBNM_MOD} ] ; then echo "libnm"; fi)
CLEAN_LIBNM=$(subst libnm,cleanlibnm,${CLEAN_LIBNM})



PREREQ_CHKPWD=$(shell if [ -d ${CHKPWD_MOD} ] ; then echo "chkpwd"; fi)
CLEAN_CHKPWD=$(subst chkpwd,cleanchkpwd,${PREREQ_CHKPWD})
PREREQ_NPDSUIT_MV=$(shell if [ -d ${NPDSUIT_MV_MOD} ] ; then echo "npdsuit_mv"; fi)
CLEAN_NPDSUIT_MV=$(subst npdsuit_mv,cleannpdsuit_mv,${PREREQ_NPDSUIT_MV})
PREREQ_QUAGGA=$(shell if [ -d ${QUAGGA_MOD} ] ; then echo "quagga"; fi)
CLEAN_QUAGGA=$(subst quagga,cleanquagga,${PREREQ_QUAGGA})
PREREQ_PIMD=$(shell if [ -d ${PIMD_MOD} ] ; then echo "pimd"; fi)
CLEAN_PIMD=$(subst pimd,cleanpimd,${PREREQ_PIMD})
PREREQ_WCPSS=$(shell if [ -d ${WCPSS_MOD} ] ; then echo "wcpss"; fi)
CLEAN_WCPSS=$(subst wcpss,cleanwcpss,${PREREQ_WCPSS})
PREREQ_WIFIKMOD=$(shell if [ -d ${WCPSS_MOD} ] ; then echo "wifikmod"; fi)
CLEAN_WIFIKMOD=$(shell if [ -d ${WCPSS_MOD} ] ; then echo "cleanwifikmod"; fi)

PREREQ_IPTABLES=$(shell if [ -d ${IPTABLES_MOD} ] ; then echo "iptables"; fi)
CLEAN_IPTABLES=$(subst iptables,cleaniptables,${PREREQ_IPTABLES})

PREREQ_EBTABLES=$(shell if [ -d ${EBTABLES_MOD} ] ; then echo "ebtables"; fi)
CLEAN_EBTABLES=$(subst ebtables,cleanebtables,${PREREQ_EBTABLES})

PREREQ_CAPTIVE=$(shell if [ -d ${CAPTIVE_MOD} ] ; then echo "captive"; fi)
CLEAN_CAPTIVE=$(subst captive,cleancaptive,${PREREQ_CAPTIVE})

PREREQ_EAG=$(shell if [ -d ${EAG_MOD} ] ; then echo "eag"; fi)
CLEAN_EAG=$(subst eag,cleaneag,${PREREQ_EAG})

PREREQ_PPPOE=$(shell if [ -d ${PPPOE_MOD} ] ; then echo "pppoe"; fi)
CLEAN_PPPOE=$(subst pppoe,cleanpppoe,${PREREQ_PPPOE})

PREREQ_BCM_NPDSUIT=$(shell if [ -d ${NPDSUIT_BCM_MOD} ] ; then echo "npdsuit_bcm"; fi)
CLEAN_NPDSUIT_BCM=$(subst npdsuit_bcm,cleanbcm_npdsuit,${ PREREQ_BCM_NPDSUIT})
CLEAN_BCMKMOD=$(shell if [ -d ${NPDSUIT_BCM_MOD} ] ; then echo "cleanbcmkmod"; fi) 
CLEAN_MVKMOD=$(shell if [ -d ${NPDSUIT_MV_MOD} ] ; then echo "cleanmvkmod"; fi) 

PREREQ_DHCP=$(shell if [ -d ${DHCP_MOD} ] ; then echo "dhcp"; fi)
CLEAN_DHCP=$(subst dhcp,cleandhcp,${PREREQ_DHCP})
PREREQ_IPFWD_LEARN=$(shell if [ -d ${IPFWD_LEARN_KMOD} ] ; then echo "ipfwd_learn"; fi)
CLEAN_IPFWD_LEARN=$(subst ipfwd_learn,cleanipfwd_learn,${PREREQ_IPFWD_LEARN})
PREREQ_SE_AGENT=$(shell if [ -d ${SE_AGENT_MOD} ] ; then echo "se_agent"; fi)
CLEAN_SE_AGENT=$(subst se_agent,cleanse_agent,${PREREQ_SE_AGENT})
PREREQ_FASTFWD=$(shell if [ -d ${FASTFWD_MOD} ] ; then echo "fast-fwd"; fi)
CLEAN_FASTFWD=$(subst fastfwd,cleanfast-fwd,${PREREQ_FASTFWD})
PREREQ_DHCPSNP=$(shell if [ -d ${DHCPSNP_MOD} ] ; then echo "dhcpsnp"; fi)
CLEAN_DHCPSNP=$(subst dhcpsnp,cleandhcpsnp,${PREREQ_DHCPSNP})

PREREQ_DHCP6=$(shell if [ -d ${DHCP6_MOD} ] ; then echo "dhcp6"; fi)
CLEAN_DHCP6=$(subst dhcp6,cleandhcp6,${PREREQ_DHCP6})

PREREQ_RADIUSD=$(shell if [ -d ${RADIUSD_MOD} ] ; then echo "radiusd"; fi)
CLEAN_RADIUSD=$(subst radiusd,cleanradiusd,${PREREQ_RADIUSD})
PREREQ_CAVIUM_RATELIMIT=$(shell if [ -d ${CAVIUM_RATELIMIT_MOD} ] ; then echo "cavium_ratelimit"; fi)
CLEAN_CAVIUM_RATELIMIT=$(subst cavium-ratelimit,cleancavium-ratelimit,${PREREQ_CAVIUM_RATELIMIT})
PREREQ_DBUS=$(shell if [ -d ${DBUS_MOD} ] ; then echo "dbus"; fi)
CLEAN_DBUS=$(subst dbus,cleandbus,${PREREQ_DBUS})

PREREQ_SYSLOG=$(shell if [ -d ${SYSLOG_MOD} ] ; then echo "syslog-ng"; fi)
CLEAN_SYSLOG=$(subst syslog-ng,cleansyslog-ng,${PREREQ_SYSLOG})

ifdef xcat
export CPSS_DIR=$(shell pwd)/${NPDSUIT_XCAT_MOD}/xcat_mcpss/src/drv
endif

.SILENT: default
.PHONY: default
default:
	echo "==========================================================="
	echo "Following targets are supported:"
	echo
	echo "==CVS Source code management targets"
	echo 
	echo "updatesrc		-Update cvs source code of apps"
	echo "update			-Update cvs source code of apps and buildtools"
	echo "archives		-Compress all source code and build an archive" 
	echo
	echo "==Application&related kernel modules targets"
	echo 
	echo "pubapps			-Compile all userspace applications and related kernel modules."
	echo "dcli			-Make dcli module."
	echo "npdsuit bcm or mv		-Make npd suit."
	echo "wcpss			-Make wcpss."
	echo "asd			-Make asd."
	echo "pfm			-Make pfm"
	echo "cavium-ethernet		-Make cavium-ethernet."
	echo "cavium-ratelimt		-Make cavium-ratelimt."
	echo "ipfwd_learn		-Make ipfwd_learn."
	echo "se_agent			-Make se_agent."
	echo "fast-fwd			-Make fast-fwd."
	echo "rpa		        -Make rpa"
	echo "sem		        -Make sem"
	echo "sfd			-Make sfd"
	echo "stpsuit			-Make rstp and mstp."
	echo "had			-Make HA daemon."
	echo "iuh			-Make iuh daemon."
	echo "ranapproxy		-Make ranapproxy daemon."
	echo "hmd			-Make HMD daemon."
	echo "wbmd                      -Make WBMD daemon."
	echo "bsd			-Make BSD daemon."
	echo "ccgi			-Make AuteCS ccgi."
	echo "dba			-Make DHCP Broadcast Agent."
	echo
	echo "==Cleanning targets"
	echo 
	echo "cleanapps		-Clean apps."
	echo "cleandcli		-Clean dcli."
	echo "cleannpdsuit		-Clean npdsuit."
	echo "cleanwcpss		-Clean wcpss."
	echo "cleanrpa		    -Clean rpa."
	echo "cleansem			-Clean sem."
	echo "cleansfd		-Clean sfd."
	echo "cleanasd		-Clean asd."
	echo "cleanpfm		-Clean pfm."
	echo "cleancavium-ethernet	-Clean Cavium-ethernet."
	echo "cleancavium-ratelimit	-Clean Cavium-ratelimt."
	echo "cleanse_agent		-Clean se_agent."
	echo "cleanipfwd_learn		-Clean ipfwd_learn."
	echo "cleanfast-fwd		-Clean fast-fwd."
	echo "cleanstpsuit		-Clean stpsuit."
	echo "cleanhad			-Clean HA daemon."
	echo "cleaniuh			-Clean iuh daemon."
	echo "cleanranapproxy		-Clean ranapproxy daemon."
	echo "cleanhmd			-Clean HMD daemon."
	echo "cleanwbmd                 -Clean WBMD daemon."
	echo "cleanbsd			-Clean BSD daemon."
	echo "cleanccgi		-Clean ccgi."
	echo "cleanbcm_npdsuit 		-Clean npdsuit bcm utilities."
	echo "cleandba	 		-Clean DHCP Broadcast Agent."
	echo 
	echo "==System level targets"
	echo 
	echo "x7x5img			-Make x7x5 img with kernel2.6.16.26"
	echo "x3img			-Make x3 img with kernel2.6.16.26"
	echo "x3img2			-Make x3 img with kernel2.6.21.7cn3"
	echo "awimgs			-Make x7x5 and x3img x3img2 within one build"
	echo
	echo "For other possible targets, please try: make TABKEY"
	echo "==========================================================="


updatesrc:
	@if [ -d .git ] ; then \
		echo "You are using a git project, please use git pull  to update src instead."; \
		exit 1 ; \
	fi
	cd src && cvs update -Ad
ifndef xcat
	cd ${ROOTFS_DIR} && git pull
endif
	cd ${KERNEL_ROOT} && git pull

update: updatesrc
	@if [ -d .git ] ; then \
		echo "You are using a git project, please use git pull  to update src instead."; \
		exit 1 ; \
	fi
	cvs update -Ad

versioncheck:
	@echo "Checking which gcc in path is in use..."
	@which mips64-octeon-linux-gnu-gcc
	@echo "Checking which gcc version is in use..."
	@mips64-octeon-linux-gnu-gcc -v

ifneq (${PREREQ_WCPSS},)
export DCLI_HAS_WCPSS=-D_D_WCPSS_ 
else
unexport DCLI_HAS_WCPSS
endif
CFLAGS += ${DCLI_HAS_WCPSS}

#$(info DCLI_HAS_WCPSS = ${DCLI_HAS_WCPSS})

define checkquagga
	if [ -d ${QUAGGA_MOD} ]; \
	then \
		echo "Use private quagga header files." ;\
		export QUAGGA_DIR=${BUILD_DIR}/${QUAGGA_MOD} ;\
	elif [ -n "${QUAGGA_DIR}" ] ; \
	then \
		echo "No private quagga found, using public quagga header files env";  \
		echo "Public QUAGGA_DIR is ${QUAGGA_DIR}"; \
	else \
		echo "No local or public quagga found. exit..."; \
		exit 1; \
	fi 
endef

dcli:dcli-pub libnm eag drp
	@echo "Building dcli module..."
	@$(checkquagga) && $(MAKE) -C ${DCLI_MOD}/src/lib
	
dcli-pub:
	@echo "Building dclipub module..."
	$(MAKE) -C ${DCLI_MOD}/src/pub
	
ccgi: eag drp libnm dcli-pub
	@echo "Building ccgi ..."
	$(MAKE) -C ${CCGI_MOD}/cgic205 OLDEAG=$(OLDEAG)
#	@echo "Do not copy to ramfs"
	cp ${CCGI_MOD}/cgic205/*.cgi ${WWW_EXPORT_DIR}/ccgi-bin
	cp -P ${CCGI_MOD}/cgic205/libcgic.so* ${LIB_EXPORT_DIR}
	cp -RP ${CCGI_MOD}/htdocs ${WWW_EXPORT_DIR}/

cvm-rate: preparedirs 
	@echo "Building cvm-rate module..."
	$(MAKE) -C src/cavium_ratelimit/app
#	cp ${CAVIUM_RATELIMIT_APP}/cvm_rate ${ROOTFS_DIR}/files/opt/bin/

snmp: eag drp libnm trap-helper acmanage acsample
	@echo "Building snmp extentions ..."
	$(MAKE) -C ${SUBAGENT_MOD} subagent_plugin.so
	cp ${SUBAGENT_MOD}/subagent_plugin.so ${LIB_EXPORT_DIR}
	rm -rf ${ROOTFS_DIR}/files/usr/sbin/snmpd
	rm -rf ${ROOTFS_DIR}/files/usr/bin/snmptrap
	cp ${SNMP_ROOTDIR}/engine/bin/snmpd ${ROOTFS_DIR}/files/usr/sbin/
	chmod +x ${ROOTFS_DIR}/files/usr/sbin/snmpd
	cp ${SNMP_ROOTDIR}/engine/bin/snmptrap ${ROOTFS_DIR}/files/usr/bin/
	chmod +x ${ROOTFS_DIR}/files/usr/bin/snmptrap
	rm -rf ${ROOTFS_DIR}/files/usr/lib/libnetsnmp*
	cp ${SNMP_ROOTDIR}/engine/lib/libnetsnmpagent.so.20.0.0 ${ROOTFS_DIR}/files/usr/lib/
	ln -s libnetsnmpagent.so.20.0.0 ${ROOTFS_DIR}/files/usr/lib/libnetsnmpagent.so.20
	ln -s libnetsnmpagent.so.20 ${ROOTFS_DIR}/files/usr/lib/libnetsnmpagent.so
	cp ${SNMP_ROOTDIR}/engine/lib/libnetsnmphelpers.so.20.0.0 ${ROOTFS_DIR}/files/usr/lib/
	ln -s libnetsnmphelpers.so.20.0.0 ${ROOTFS_DIR}/files/usr/lib/libnetsnmphelpers.so.20
	ln -s libnetsnmphelpers.so.20 ${ROOTFS_DIR}/files/usr/lib/libnetsnmphelpers.so
	cp ${SNMP_ROOTDIR}/engine/lib/libnetsnmpmibs.so.20.0.0 ${ROOTFS_DIR}/files/usr/lib/
	ln -s libnetsnmpmibs.so.20.0.0 ${ROOTFS_DIR}/files/usr/lib/libnetsnmpmibs.so.20
	ln -s libnetsnmpmibs.so.20 ${ROOTFS_DIR}/files/usr/lib/libnetsnmpmibs.so
	cp ${SNMP_ROOTDIR}/engine/lib/libnetsnmptrapd.so.20.0.0 ${ROOTFS_DIR}/files/usr/lib/
	ln -s libnetsnmptrapd.so.20.0.0 ${ROOTFS_DIR}/files/usr/lib/libnetsnmptrapd.so.20
	ln -s libnetsnmptrapd.so.20 ${ROOTFS_DIR}/files/usr/lib/libnetsnmptrapd.so
	cp ${SNMP_ROOTDIR}/engine/lib/libnetsnmp.so.20.0.0 ${ROOTFS_DIR}/files/usr/lib/
	ln -s libnetsnmp.so.20.0.0 ${ROOTFS_DIR}/files/usr/lib/libnetsnmp.so.20
	ln -s libnetsnmp.so.20 ${ROOTFS_DIR}/files/usr/lib/libnetsnmp.so
#	cp -rf ${SNMPMIBS_DIR}/* ${ROOTFS_DIR}/files/usr/share/snmp/mibs/
#	$(MAKE) -C ${SUBAGENT_MOD} 
#	cp ${SUBAGENT_MOD}/subagent ${BIN_EXPORT_DIR}

acmanage:eag drp libnm
	@echo "Buliding acmanage..."
	$(MAKE) -C $(SNMPMANAGE_MOD) acmanage
	cp $(SNMPMANAGE_MOD)/acmanage $(BIN_EXPORT_DIR)
	cp ${SNMPMANAGE_MOD}/acmanaged ${AUTEWAREFS_DIR}/files/etc/init.d/
	-chmod 755 ${AUTEWAREFS_DIR}/files/etc/init.d/acmanaged
	-ln -sf ../init.d/acmanaged  ${AUTEWAREFS_DIR}/files/etc/rc2.d/S75acmanaged

srvm:
	@echo "Building service management ..."
	$(MAKE) -C ${SRVM_MOD}/app
	cp ${SRVM_MOD}/app/srvload ${BIN_EXPORT_DIR}
	cp ${SRVM_MOD}/app/srvsave ${BIN_EXPORT_DIR}
	cp ${SRVM_MOD}/app/srvcmd ${BIN_EXPORT_DIR}

drp:
	@echo "Building drp..."
	$(MAKE) -C ${DRP_MOD}/src
	cp ${DRP_MOD}/src/drp $(BIN_EXPORT_DIR)
	cp ${DRP_MOD}/res/drpd ${AUTEWAREFS_DIR}/files/etc/init.d/drpd
	-chmod 755 ${AUTEWAREFS_DIR}/files/etc/init.d/drpd
	-ln -sf ../init.d/drpd  ${AUTEWAREFS_DIR}/files/etc/rc2.d/S70drpd
	
acsample: libnm dcli-pub acmanage
	@echo "Building acsample..."
	$(MAKE) -C ${ACSAMPLE_MOD}/
	cp ${ACSAMPLE_MOD}/acsample $(BIN_EXPORT_DIR)

trap-helper: libnm dcli-pub
	@echo "Building trap-helper..."
	$(MAKE) -C ${TRAP_HELPER_MOD}/ trap-helper
	cp ${TRAP_HELPER_MOD}/trap-helper $(BIN_EXPORT_DIR)

libnm: dcli-pub eag drp
	@echo "Building libnm..."
	$(MAKE) -C $(LIBNM_MOD) libnm.so


asd:dcli
	@echo "Building asd..."
	$(MAKE) -C ${ASD_MOD}/src/app
	@echo "Building dcli_asd..."
	$(MAKE) -C ${ASD_MOD}/src/dcli/
	cp ${ASD_MOD}/src/app/asd $(BIN_EXPORT_DIR)
	cp -P ${ASD_MOD}/src/app/wapi/libw* $(LIB_EXPORT_DIR)
	chmod 755 $(LIB_EXPORT_DIR)/libwssl.so*
	chmod 755 $(LIB_EXPORT_DIR)/libwcrypto.so*
	ln -sf libwssl.so.0.9.8 $(LIB_EXPORT_DIR)/libwssl.so
	ln -sf libwcrypto.so.0.9.8 $(LIB_EXPORT_DIR)/libwcrypto.so

wcpss: wid wsm
	echo "Finished making wcpss."
	cp ${WTPVERFILE} ${ROOTFS_DIR}/files/etc/version/

wsm: wcpsspub_ac
	@echo "Building wsm ..."
	$(MAKE) -C ${WCPSS_MOD}/src/app/wsm
	cp ${WCPSS_MOD}/src/app/wsm/wsm $(BIN_EXPORT_DIR)

wid: wcpsspub_ac dcli
	@echo "Building wid_dcli ..."
	@$(checkquagga) && $(MAKE) -C ${WCPSS_MOD}/src/dcli/
	@echo "Building wid ..."
	$(MAKE) -C ${WCPSS_MOD}/src/app/wid
	cp ${WCPSS_MOD}/src/app/wid/wid $(BIN_EXPORT_DIR)
	
iuh: preparedirs 
	@echo "Building iuh ..."
	$(MAKE) -C ${IUH_MOD}
	cp ${IUH_MOD}/iuh $(BIN_EXPORT_DIR)
ranapproxy: preparedirs
	@echo "Building ranapproxy ..."
	$(MAKE) -C ${IU_MOD}/
	cp ${IU_MOD}/ranapproxy $(BIN_EXPORT_DIR)
	cp ${SCCP_LIB_MOD}/lib* $(LIB_EXPORT_DIR)

wcpsspub_ac:
	@echo "Building wcpss public lib for AC side..."
	$(MAKE) -C ${WCPSS_MOD}/src/app/pub
	

snmpliblink:
	$(MAKE) -C ${SNMP_ROOTDIR}/subagent makelink

quagga: snmpliblink
	@echo "Building quagga suit ..."
	if [ ! -d ${QUAGGA_MOD}/${BUILDDIR} ] ; then \
		pushd ${QUAGGA_MOD} ; \
		./configpkg ${BUILDDIR}; \
		popd ; \
	fi 	
	$(MAKE) -C ${QUAGGA_MOD}/${BUILDDIR}
	cp ${QUAGGA_MOD}/${BUILDDIR}/vtysh/.libs/vtysh $(BIN_EXPORT_DIR) 
	cp ${QUAGGA_MOD}/${BUILDDIR}/zebra/.libs/zebra $(BIN_EXPORT_DIR)/rtmd
	cp ${QUAGGA_MOD}/${BUILDDIR}/ripd/.libs/ripd $(BIN_EXPORT_DIR) 
	cp ${QUAGGA_MOD}/${BUILDDIR}/ospfd/.libs/ospfd $(BIN_EXPORT_DIR)
#	cp ${QUAGGA_MOD}/${BUILDDIR}/ospf6d/.libs/ospf6d $(BIN_EXPORT_DIR)
#	cp ${QUAGGA_MOD}/${BUILDDIR}/isisd/.libs/isisd $(BIN_EXPORT_DIR)
#	cp ${QUAGGA_MOD}/${BUILDDIR}/ripngd/.libs/ripngd $(BIN_EXPORT_DIR)
	cp ${QUAGGA_MOD}/${BUILDDIR}/lib/.libs/librtm.so.0.0.0 $(LIB_EXPORT_DIR) 
	ln -sf librtm.so.0.0.0 $(LIB_EXPORT_DIR)/librtm.so
	ln -sf librtm.so.0.0.0 $(LIB_EXPORT_DIR)/librtm.so.0
	ln -sf librtm.so.0.0.0 $(LIB_EXPORT_DIR)/librtm.so.0.0
	cp ${QUAGGA_MOD}/${BUILDDIR}/ospfd/.libs/libospf.so.0.0.0 $(LIB_EXPORT_DIR)
	ln -sf libospf.so.0.0.0 $(LIB_EXPORT_DIR)/libospf.so
	ln -sf libospf.so.0.0.0 $(LIB_EXPORT_DIR)/libospf.so.0
#	cp ${QUAGGA_MOD}/debsrc/quagga-0.99.5/ripd/ripd.conf.sample $(ETC_EXPORT_DIR)/ripd.conf
#	cp ${QUAGGA_MOD}/debsrc/quagga-0.99.5/ospfd/ospfd.conf.sample $(ETC_EXPORT_DIR)/ospfd.conf
#	cp ${QUAGGA_MOD}/debsrc/quagga-0.99.5/zebra/zebra.conf.sample $(ETC_EXPORT_DIR)/zebra.conf

syslog-ng:
	@echo "Building syslog-ng suit ..."
	$(MAKE) -C ${SYSLOG_MOD}
	if [ 1 ] ; then \
		pushd ${SYSLOG_MOD} ; \
		test -d ${ROOTFS_DIR}/files/sbin/ || mkdir ${ROOTFS_DIR}/files/sbin/ ; \
		cp src/syslog-ng ${ROOTFS_DIR}/files/sbin/syslog-ng ; \
		#test -d $(ROOTFS_DIR)/files/etc/syslog-ng/ || mkdir $(ROOTFS_DIR)/files/etc/syslog-ng/ ; \
		#cp conf/syslog-ng.conf ${ROOTFS_DIR}/files/etc/syslog-ng/syslog-ng.conf ; \
		popd ; \
	fi

npdsuit_mv: bm mcpss npd
	@echo "Moving npdsuit mv files into rootfs"
#	mv ${KMOD_EXPORT_DIR}/kapDrv.ko ${ROOTFS_KMOD_DIR}/misc/
#	mv ${KMOD_EXPORT_DIR}/mvPpDrv.ko ${ROOTFS_KMOD_DIR}/misc/
#	mv ${BIN_EXPORT_DIR}/npd ${ROOTFS_DIR}/files/opt/bin/
#	mv ${LIB_EXPORT_DIR}/libmcpss* ${ROOTFS_DIR}/files/opt/lib/
#	mv ${LIB_EXPORT_DIR}/libnam* ${ROOTFS_DIR}/files/opt/lib/
#	mv ${LIB_EXPORT_DIR}/libnbm* ${ROOTFS_DIR}/files/opt/lib/
	cp  ${BM_KMOD}/res/QT2x25D_module_firmware_v2_0_3_0_MDIO.bin  ${AUTEWAREFS_DIR}/files/opt/bin/

npdsuit_bcm: bmbcm kapbcm bcmd npdbcm
	@echo "Moving npdsuit mv files into rootfs"
	mv ${KMOD_EXPORT_DIR}/kapDrv.ko ${ROOTFS_KMOD_DIR}/misc/
	mv ${KMOD_EXPORT_DIR}/linux-kernel-bde.ko ${ROOTFS_KMOD_DIR}/misc/
	mv ${KMOD_EXPORT_DIR}/linux-user-bde.ko ${ROOTFS_KMOD_DIR}/misc/
	mv ${BIN_EXPORT_DIR}/npd ${AUTEWAREFS_DIR}/files/opt/bin/
	mv ${LIB_EXPORT_DIR}/libmbcm* ${AUTEWAREFS_DIR}/files/opt/lib/
	mv ${LIB_EXPORT_DIR}/libnam* ${AUTEWAREFS_DIR}/files/opt/lib/
	mv ${LIB_EXPORT_DIR}/libnbm* ${AUTEWAREFS_DIR}/files/opt/lib/

patchnpd:
	@echo "Patching npd...."
	if [ -d ${BUILD_DIR}/release/patchnpd ] ; then 	\
	echo "${BUILD_DIR}/release/patchnpd is exist" ; \
	else  					\
		mkdir ${BUILD_DIR}/release/patchnpd ; \
	fi
	touch ${BUILD_DIR}/release/patchnpd/install
	echo "#! /bin/bash" > ${BUILD_DIR}/release/patchnpd/install
	echo "sudo cp /mnt/patch/patchnpd/npd /opt/bin" >> ${BUILD_DIR}/release/patchnpd/install
#	echo "sudo cp /mnt/patch/patchnpd/libmcpss.so.0.1 /opt/lib" >> ${BUILD_DIR}/release/patchnpd/install
#	echo "sudo cp /mnt/patch/patchnpd/libnam.so.0.1 /opt/lib" >> ${BUILD_DIR}/release/patchnpd/install
	cp ${AUTEWAREFS_DIR}/files/opt/bin/npd ${BUILD_DIR}/release/patchnpd/
#	cp ${AUTEWAREFS_DIR}/files/opt/lib/libmcpss.so.0.1 ${BUILD_DIR}/release/patchnpd/
#	cp ${AUTEWAREFS_DIR}/files/opt/lib/libnam.so.0.1 ${BUILD_DIR}/release/patchnpd/
	cd ${BUILD_DIR}/release;tar cvfj ${BUILD_DIR}/release/patchnpd.sp patchnpd

nam:
	@echo "Building nam module..."
	$(MAKE) DRV_LIB_FLAG=${CPSS_FLAG} -C ${NPDSUIT_MV_MOD}/nam_ax7/src/lib

nambcm:
	@echo "Building nam module......"
	$(MAKE) DRV_LIB_FLAG=BCM -C ${NPDSUIT_BCM_MOD}/nam_ax7/src/lib

nbm:
	@echo "Building nbm module..."
	$(MAKE) DRV_LIB_FLAG=${CPSS_FLAG} -C ${NPDSUIT_MV_MOD}/nbm_ax7/src/lib

nbmbcm:
	@echo "Building nbm module......"
	$(MAKE) DRV_LIB_FLAG=BCM -C ${NPDSUIT_BCM_MOD}/nbm_ax7/src/lib

mcpss: mvkmod kap
	@echo "Building mcpss..."
	$(MAKE) -C ${NPDSUIT_MV_MOD}/${MCPSSPATH}/src/drv

sempub:
	@echo "Building sem_pub"
	$(MAKE) -C ${SEM_MOD}/src/pub/

sem:sempub
	@echo "Building sem ..."
	$(MAKE) -C ${SEM_MOD}/src/app/
	@echo "Building dcli_sem..."
	@$(checkquagga) && $(MAKE) -C ${SEM_MOD}/src/dcli/
#	cp ${BIN_EXPORT_DIR}/sem ${AUTEWAREFS_DIR}/files/opt/bin/

npd: bm nam nbm 
	@echo "Building dcli_npd..."
	@$(checkquagga) && $(MAKE) -C ${NPDSUIT_MV_MOD}/npd/src/dcli/	
	@echo "Building npd..."
	$(MAKE) DRV_LIB_FLAG=${CPSS_FLAG} -C ${NPDSUIT_MV_MOD}/npd/src/app

npdbcm: nambcm nbmbcm
	@echo "Building npd......"
	$(MAKE) DRV_LIB_FLAG=BCM -C ${NPDSUIT_BCM_MOD}/npd/src/app

oct_nic: kmod
	@echo "Building oct pcie..."
	$(MAKE) -C $(OCTEON_BOOTDIR)/components/driver
	cp ${OCTEON_BOOTDIR}/components/driver/bin/octeon_drv.ko ${ROOTFS_KMOD_DIR}/misc/
	cp ${OCTEON_BOOTDIR}/components/driver/bin/octnic.ko ${ROOTFS_KMOD_DIR}/misc/

bcmd: kbdemod ubdemod
	@echo "Building bcm driver ..."
	$(MAKE) -C $(BCM_MOD)/systems/linux/user/nsx64
	#cp $(BCM_MOD)/systems/linux/user/nsx64/bcm.user $(BIN_EXPORT_DIR)/
	cp -RP $(BCM_TOPDIR)/rc ${AUTEWAREFS_DIR}/files/

stpsuit:
	@echo "Building stp suit..."
	$(MAKE) -C ${STPSUIT_MOD}/src
	cp ${STPSUIT_MOD}/src/OBJ/bridge ${BIN_EXPORT_DIR}/

had:
	@echo "Building HA daemon..."
	$(MAKE) -C ${HAD_MOD}/src/lib
	cp $(HAD_MOD)/src/lib/obj/had ${BIN_EXPORT_DIR}/
hmd: preparedirs
	@echo "Building HMD daemon..."
	$(MAKE) -C ${HMD_MOD}
	cp $(HMD_MOD)/hmd ${BIN_EXPORT_DIR}/
wbmd: preparedirs
	@echo "Building WBMD daemon..."
	$(MAKE) -C ${WBMD_MOD}
	cp $(WBMD_MOD)/wbmd ${BIN_EXPORT_DIR}/
bsd: 
	@echo "Building BSD daemon..."
	$(MAKE) -C ${BSD_MOD}/src
	cp $(BSD_MOD)/src/bsd ${BIN_EXPORT_DIR}/

captive:
	@echo "Building captive portal ..."
	-chmod 755 ${CAPTIVE_MOD}/device/bash/*.sh
	[ -d ${AUTEWAREFS_DIR}/files/usr/bin ] || mkdir -p ${AUTEWAREFS_DIR}/files/usr/bin
	cp -rf ${CAPTIVE_MOD}/device/bash/*.sh ${AUTEWAREFS_DIR}/files/usr/bin/

eag:captive iptables
	echo "Building eag ..." 
	$(MAKE) -C ${EAG_MOD}/pkg/eag
	cp ${EAG_MOD}/res/eag/eag ${AUTEWAREFS_DIR}/files/etc/init.d/
	-chmod 755 ${AUTEWAREFS_DIR}/files/etc/init.d/eag
	-ln -sf ../init.d/eag  ${AUTEWAREFS_DIR}/files/etc/rc2.d/S73eag
#	cp ${EAG_MOD}/res/eag/iptables ${AUTEWAREFS_DIR}/files/etc/init.d/
#	-chmod 755 ${AUTEWAREFS_DIR}/files/etc/init.d/iptables
#	-ln -sf ../init.d/iptables  ${AUTEWAREFS_DIR}/files/etc/rc2.d/S72iptables
	cp ${EAG_MOD}/res/eag/pdcd ${AUTEWAREFS_DIR}/files/etc/init.d/
	-chmod 755 ${AUTEWAREFS_DIR}/files/etc/init.d/pdcd
	-ln -sf ../init.d/pdcd  ${AUTEWAREFS_DIR}/files/etc/rc2.d/S72pdcd
	cp ${EAG_MOD}/res/eag/rdcd ${AUTEWAREFS_DIR}/files/etc/init.d/
	-chmod 755 ${AUTEWAREFS_DIR}/files/etc/init.d/rdcd
	-ln -sf ../init.d/rdcd  ${AUTEWAREFS_DIR}/files/etc/rc2.d/S72rdcd
	cp ${EAG_MOD}/res/eag/eag_modules ${AUTEWAREFS_DIR}/files/etc/init.d/
	-chmod 755 ${AUTEWAREFS_DIR}/files/etc/init.d/eag_modules	
	-rm -rf ${AUTEWAREFS_DIR}/files/opt/eag/www
	-mkdir -p ${AUTEWAREFS_DIR}/files/opt/eag/
	cp -RP ${EAG_MOD}/res/eag/www ${AUTEWAREFS_DIR}/files/opt/eag/
	-rm -rf ${AUTEWAREFS_DIR}/files/opt/eag/www/CVS
	cp -RP ${EAG_MOD}/res/eag/wisprp ${AUTEWAREFS_DIR}/files/opt/eag/www/
	-rm -rf ${AUTEWAREFS_DIR}/files/opt/eag/www/wisprp/CVS
	cp ${EAG_MOD}/res/eag/radius_errorcode_profile_CMCC ${AUTEWAREFS_DIR}/files/opt/eag/
	-chmod 644 ${AUTEWAREFS_DIR}/files/opt/eag/radius_errorcode_profile_CMCC
	
pppoe: preparedirs
	echo "Building pppoe ..."
	$(MAKE) -C ${PPPOE_MOD}
	cp ${PPPOE_MOD}/pppoed $(BIN_EXPORT_DIR)
	
dhcp: preparedirs
	@echo "Building dhcp ..."
	@echo $(DHCP_CONFIG_HOSTCC)
	if [ ! -f ${DHCP_MOD}/Makefile ] ; then 		\
		pushd ${DHCP_MOD} ; 				\
		chmod +x ./configure;				\
		./configure --host=$(DHCP_CONFIG_HOSTCC) --with-srv-lease-file=/var/lib/dhcp4/dhcpd.leases; \
		popd ; 						\
	fi
	$(MAKE) -C ${DHCP_MOD}
	cp ${DHCP_MOD}/server/dhcpd ${AUTEWAREFS_DIR}/files/opt/bin
	cp ${DHCP_MOD}/relay/dhcrelay ${AUTEWAREFS_DIR}/files/opt/bin

dhcp6: preparedirs
	@echo "Building dhcp6 ..."
	@echo $(DHCP_CONFIG_HOSTCC)
	if [ ! -f ${DHCP6_MOD}/Makefile ] ; then 		\
		pushd ${DHCP6_MOD} ; 				\
		chmod +x ./configure;				\
		./configure --host=$(DHCP_CONFIG_HOSTCC) --with-srv-lease-file=/var/lib/dhcp4/dhcp6d.leases; \
		popd ; 						\
	fi
	$(MAKE) -C ${DHCP6_MOD}
	cp ${DHCP6_MOD}/server/dhcpd ${AUTEWAREFS_DIR}/files/opt/bin/dhcp6d
	cp ${DHCP6_MOD}/relay/dhcrelay ${AUTEWAREFS_DIR}/files/opt/bin/dhcrelay6

igmp: preparedirs
	@echo "Building igmp snooping ..."
	$(MAKE) -C ${IGMP_MOD}/
	cp ${IGMP_MOD}/igmp_snoop ${BIN_EXPORT_DIR}/



link-kernel-se-files:
ifeq (${NEWKERN},3)
	./copy-se2kernel.sh copy ${KERNEL_ROOT} 
endif
ifeq (${NEWKERN},4)
	./copy-se2kernel.sh copy ${KERNEL_ROOT}
endif

	@echo ${PATH}

revert-kernel-se-files:
ifeq (${NEWKERN},3)
	find  ${KERNEL_ROOT}/arch/mips/include/asm/octeon -type l -delete
	find  ${KERNEL_ROOT}/arch/mips/cavium-octeon/executive -type l -delete
#	(cd kernel_2.6/linux-svn; svn revert -R arch/mips/include/asm/octeon arch/mips/cavium-octeon/executive )
endif
ifeq (${NEWKERN},4)
	find  ${KERNEL_ROOT}/arch/mips/include/asm/octeon -type l -delete
	find  ${KERNEL_ROOT}/arch/mips/cavium-octeon/executive -type l -delete
#       (cd kernel_2.6/linux-svn; svn revert -R arch/mips/include/asm/octeon arch/mips/cavium-octeon/executive )
endif
	@echo ${PATH}
libdhcpsnp: preparedirs
	@echo "Building dhcpsnp library..."
	$(MAKE) -C ${DHCPSNP_MOD}/src/lib
se_agent: preparedirs oct_nic
	@echo "Building se_agent..."
	@echo $(ACCAPI_DIR)
	$(MAKE) -C ${SE_AGENT_MOD} OCTEON_TARGET=linux_64	
	cp ${SE_AGENT_MOD}/se_agent ${AUTEWAREFS_DIR}/files/opt/bin

fast-fwd:
	@echo "Building fast fwd..."
	$(MAKE) -C ${FASTFWD_MOD}	
	#$(MAKE) -C ${FASTFWD_MOD} PREFIX=_standalone FWDMODE_STANDALONE=1	
	#$(MAKE) -C ${FASTFWD_MOD} strip PREFIX=_standalone	
	cp ${FASTFWD_MOD}/fastfwd ${AUTEWAREFS_DIR}/files/usr/bin/
	$(MAKE) -C ${FASTFWD_MOD} PREFIX=_68
	cp ${FASTFWD_MOD}/fastfwd_68 ${AUTEWAREFS_DIR}/files/usr/bin/

dhcpsnp: libdhcpsnp
	@echo "Building dhcpsnp module..."
	$(MAKE) -C ${DHCPSNP_MOD}/src/app
	
cleandhcpsnp:
	@echo "Cleaning dhcpsnp module..."
	$(MAKE) -C ${DHCPSNP_MOD}/src/lib clean
	$(MAKE) -C ${DHCPSNP_MOD}/src/app clean
libsubdirs: preparedirs
#	[ -d ${AUTEWAREFS_DIR}/files/opt/lib/xtables ] || mkdir ${AUTEWAREFS_DIR}/files/opt/lib/xtables
	[ -d ${AUTEWAREFS_DIR}/files/opt/lib/iptables ] || mkdir ${AUTEWAREFS_DIR}/files/opt/lib/iptables
#	[ -d ${AUTEWAREFS_DIR}/files/opt/lib/ipset ] || mkdir ${AUTEWAREFS_DIR}/files/opt/lib/ipset

export IPTABLES_DIR=${IPTABLES_MOD}"/debsrc/iptables-1.4.20/"

ipt:
	${IPTABLES_MOD}/configpkg
	$(MAKE) -C ${IPTABLES_DIR}

iptables: libsubdirs ipset libmnl ipp2p
	@echo "Building iptables ..."
	${IPTABLES_MOD}/configpkg
	$(MAKE) -C ${IPTABLES_DIR}
	cp ${IPTABLES_DIR}/iptables/.libs/xtables-multi ${AUTEWAREFS_DIR}/files/opt/bin/	
	(cd ${AUTEWAREFS_DIR}/files/opt/bin/;\
	ln -sf xtables-multi iptables;\
	ln -sf xtables-multi iptables-save;\
	ln -sf xtables-multi iptables-restore;\
	ln -sf xtables-multi ip6tables;\
	ln -sf xtables-multi ip6tables-save;\
	ln -sf xtables-multi ip6tables-restore;\
	cd -)
	cp ${IPTABLES_DIR}/libiptc/.libs/libip4tc.so.0.1.0 ${AUTEWAREFS_DIR}/files/opt/lib/
	cp ${IPTABLES_DIR}/libiptc/.libs/libip6tc.so.0.1.0 ${AUTEWAREFS_DIR}/files/opt/lib/
	cp ${IPTABLES_DIR}/libiptc/.libs/libiptc.so.0.0.0 ${AUTEWAREFS_DIR}/files/opt/lib/
	cp ${IPTABLES_DIR}/libxtables/.libs/libxtables.so.10.0.0 ${AUTEWAREFS_DIR}/files/opt/lib/
	(cd ${AUTEWAREFS_DIR}/files/opt/lib/;\
	ln -sf libip4tc.so.0.1.0 libip4tc.so;\
	ln -sf libip4tc.so.0.1.0 libip4tc.so.0;\
	ln -sf libip6tc.so.0.1.0 libip6tc.so;\
	ln -sf libip6tc.so.0.1.0 libip6tc.so.0;\
	ln -sf libiptc.so.0.0.0 libiptc.so;\
	ln -sf libiptc.so.0.0.0 libiptc.so.0;\
	ln -sf libxtables.so.10.0.0 libxtables.so;\
	ln -sf libxtables.so.10.0.0 libxtables.so.10;\
	cd -)
	cp ${IPTABLES_DIR}/extensions/*.so ${AUTEWAREFS_DIR}/files/opt/lib/iptables/	

${KERNEL_ROOT}/.config: 
	cp ${KERNEL_ROOT}/octeon.config ${KERNEL_ROOT}/.config

kernel: ${KERNEL_ROOT}/.config
	@echo "Making kernel ..."
	-rm ${KERNEL_ROOT}/usr/initramfs_data.cpio*
ifdef xcat
	make -C ${KERNEL_ROOT} uImage -j 20
else
	make -C ${KERNEL_ROOT}
endif

kmod: ${KERNEL_ROOT}/.config ${MAKE_KERNEL_BEFORE_MODULES}
	@echo "Making kernel built-in modules ..."
	make -C ${KERNEL_ROOT} modules
	[ -d ${ROOTFS_KMOD_DIR} ] || mkdir -p ${ROOTFS_KMOD_DIR}
	[ -d ${KMOD_EXPORT_DIR} ] || mkdir -p ${KMOD_EXPORT_DIR}
	[ -d ${ROOTFS_KMOD_DIR}/kernel ] || mkdir -p ${ROOTFS_KMOD_DIR}/kernel
	[ -d ${ROOTFS_KMOD_DIR}/misc ] || mkdir -p ${ROOTFS_KMOD_DIR}/misc
libmnl:
	@echo "Building libmnl ..."
	${IPTABLES_MOD}/external/config_mnl	
	$(MAKE) -C ${IPTABLES_MOD}/external/libmnl-1.0.1
	cp ${IPTABLES_MOD}/external/libmnl-1.0.1/src/.libs/libmnl.so.0.0.1 ${AUTEWAREFS_DIR}/files/opt/lib/
	(cd ${AUTEWAREFS_DIR}/files/opt/lib/;\
	ln -sf libmnl.so.0.0.1 libmnl.so;\
	ln -sf libmnl.so.0.0.1 libmnl.so.0;\
	cd -)
ipset_config:
	${IPTABLES_MOD}/external/config_ipset

ipset: libmnl kmod libsubdirs 
	@echo "Building ipset ..."
#	${IPTABLES_MOD}/external/config_ipset	
	$(MAKE) -C ${IPTABLES_MOD}/external/ipset-6.19/
	$(MAKE) -C ${IPTABLES_MOD}/external/ipset-6.19/ modules
	cp ${IPTABLES_MOD}/external/ipset-6.19/src/.libs/ipset ${AUTEWAREFS_DIR}/files/opt/bin/
	cp ${IPTABLES_MOD}/external/ipset-6.19/lib/.libs/libipset.so.3.1.0 ${AUTEWAREFS_DIR}/files/opt/lib/
	(cd ${AUTEWAREFS_DIR}/files/opt/lib/;\
	ln -sf libipset.so.3.1.0 libipset.so;\
	ln -sf libipset.so.3.1.0 libipset.so.3;\
	cd -)
	cp ${IPTABLES_MOD}/external/ipset-6.19/kernel/net/netfilter/ipset/*.ko ${KMOD_EXPORT_DIR}/
	cp ${IPTABLES_MOD}/external/ipset-6.19/kernel/net/netfilter/*.ko ${KMOD_EXPORT_DIR}/ 
#	cp ${IPTABLES_MOD}/external/ipset-4.5/kernel/*.ko ${KMOD_EXPORT_DIR}/
#	cp ${IPTABLES_MOD}/external/ipset-4.5/*.so  ${AUTEWAREFS_DIR}/files/opt/lib/ipset/
#	cp ${IPTABLES_MOD}/external/ipset-4.5/ipset ${AUTEWAREFS_DIR}/files/opt/bin/
ipp2p: kmod
	@echo "Building iptables external modules -- ipp2p ..."
	$(MAKE) -C ${IPTABLES_MOD}/external/ipp2p-0.99.15/
	cp ${IPTABLES_MOD}/external/ipp2p-0.99.15/*.ko ${KMOD_EXPORT_DIR}
	cp ${IPTABLES_MOD}/external/ipp2p-0.99.15/*.so ${AUTEWAREFS_DIR}/files/opt/lib/iptables/

bm: kmod
	@echo "Building bm..."
	$(MAKE) -C ${BM_KMOD}/src/kmod
	$(MAKE) -C ${BM_KMOD}/src/app
	cp ${BM_KMOD}/src/app/bmutil ${BIN_EXPORT_DIR}/
	cp ${KMOD_EXPORT_DIR}/bm.ko ${ROOTFS_KMOD_DIR}/misc/

kes: kmod
	@echo "Building kes driver kernel module."
	$(MAKE) -C ${KES_MOD}
	cp ${KES_MOD}/kes.ko ${KMOD_EXPORT_DIR}

pfm_deamon:
	@echo "Building pfm_deamon ..."
	$(MAKE) -C ${PFM_MOD} deamon
	cp ${PFM_MOD}/pfm_deamon ${BIN_EXPORT_DIR}

pfm_kern: kmod
	@echo "Building pfm driver kernel module."
	$(MAKE) -C $(PFM_MOD) kern
	cp $(PFM_MOD)/*.ko ${KMOD_EXPORT_DIR}/
	#mips-linux-gnu-gcc ${DBUS_INCLUDE} ${DBUS_LIBS} -L${BUILDROOTDIR}/usr/lib -lpthread ${PFM_MOD}/pfm.c -o ${PFM_MOD}/pfm_deamon
	#-cp ${PFM_MOD}/pfm_deamon ${BIN_EXPORT_DIR}
	#$(MAKE) -C $(PFM_MOD) deamon

pfm: pfm_kern pfm_deamon

dbus:
	@echo "Building DBus ..."
	$(MAKE) -C ${DBUS_MOD}
	-cp ${DBUS_MOD}/bus/dbus-daemon ${BIN_EXPORT_DIR}
	-cp ${DBUS_MOD}/dbus/.libs/libdbus-1.so* ${LIB_EXPORT_DIR} -d
	-rm ${PWD}/rootfs/files/usr/lib/libdbus-1.so*
	-rm ${PWD}/rootfs/files/usr/bin/dbus-daemon
	#cp ${DBUS_MOD}/tools/dbus-cleanup-sockets ${PWD}/rootfs/files/usr/bin
	#cp ${DBUS_MOD}/tools/dbus-launch ${PWD}/rootfs/files/usr/bin
	#cp ${DBUS_MOD}/tools/dbus-monitor ${PWD}/rootfs/files/usr/bin
	#cp ${DBUS_MOD}/tools/dbus-send ${PWD}/rootfs/files/usr/bin
	#cp ${DBUS_MOD}/tools/dbus-uuidgen ${PWD}/rootfs/files/usr/bin	
dbus_tipc_demo:
	@echo "Building DBus ..."
	mips-linux-gnu-gcc ${DBUS_INCLUDE} ${DBUS_LIBS} -L${BUILDROOTDIR}/usr/lib ${DBUS_MOD}/dbus_tipc_demo.c -o ${DBUS_MOD}/dbus_tipc_demo
	-cp ${DBUS_MOD}/dbus_tipc_demo ${BIN_EXPORT_DIR}


bmbcm: kmod
	@echo "Building bm for bcm..."
	$(MAKE) -C ${BM_KMOD}/src/kmod
	$(MAKE) -C ${BM_KMOD}/src/app
	cp ${BM_KMOD}/src/app/bmutil ${BIN_EXPORT_DIR}/
	cp ${KMOD_EXPORT_DIR}/bm.ko ${ROOTFS_KMOD_DIR}/misc/

ksem: kmod
	@echo "Building kernel sem driver module."
	$(MAKE) -C $(KSEM_MOD)
	[ -d ${KMOD_EXPORT_DIR} ] || mkdir -p ${KMOD_EXPORT_DIR}
	cp $(KSEM_MOD)/*.ko ${KMOD_EXPORT_DIR}/
	cp ${KMOD_EXPORT_DIR}/kernel-sem.ko ${ROOTFS_KMOD_DIR}/misc/
mvkmod: kmod 
	@echo "Building marvell kernel module ..."
	$(MAKE) -C ${NPDSUIT_MV_MOD}/${MCPSSPATH}/src/kmod/mvPpDrv

kap: kmod
	@echo "Building kernel asic pipe kernel module ..."
	$(MAKE) -C ${NPDSUIT_MV_MOD}/kap/src/kmod

kapbcm: kmod
	@echo "Building kernel asic pipe kernel module ..."
	$(MAKE) -C ${NPDSUIT_BCM_MOD}/kap/src/kmod
ipfwd_learn: kmod
	@echo "Building ipfwd_learn kernel module."
	$(MAKE) -C ${IPFWD_LEARN_KMOD}
	cp ${IPFWD_LEARN_KMOD}/ipfwd_learn_coexist.ko  ${KMOD_EXPORT_DIR}/
	@echo "Building ipfwd_learn_standalone kernel module."
	$(MAKE) -C ${IPFWD_LEARN_KMOD} IPFWD_LEARN_MODE_STANDALONE=1
	cp ${IPFWD_LEARN_KMOD}/ipfwd_learn_standalone.ko  ${KMOD_EXPORT_DIR}/

rpa_kmod: kmod
	@echo "Building rpa driver kernel module."
	$(MAKE) -C ${RPA_MOD}/src/kmod
	cp ${RPA_MOD}/src/kmod/*.ko ${KMOD_EXPORT_DIR}/

rpa_daemon:
	@echo "Building rpa daemon application."
	$(MAKE) -C ${RPA_MOD}/src/app all
	cp ${RPA_MOD}/src/app/rpa-daemon ${BIN_EXPORT_DIR}

rpa: rpa_kmod rpa_daemon
cavium-ratelimit: kmod cvm-rate
	@echo "Building cavium-ratelimit kernel module."
	$(MAKE) -C ${CAVIUM_RATELIMIT_MOD}
	cp ${CAVIUM_RATELIMIT_MOD}/cavium_ratelimit.ko ${KMOD_EXPORT_DIR}/
sfd_daemon:
	@echo "Building sfd_daemon ..."
	$(MAKE) -C ${SFD_MOD} daemon
	cp ${SFD_MOD}/sfd_daemon ${BIN_EXPORT_DIR}

sfd_kern: kmod
	@echo "Building sfd driver kernel module."
	$(MAKE) -C $(SFD_MOD) kern
	cp $(SFD_MOD)/*.ko ${KMOD_EXPORT_DIR}/

sfd: sfd_kern sfd_daemon
	if [ 1 ] ; then \
		pushd ${SFD_MOD} ; \
		popd ; \
	fi

wifikmod: kmod
	@echo "Building wifi-ehternet ..."
	$(MAKE) -C ${WCPSS_MOD}/src/kmod/wifi-ethernet/
	cp ${WCPSS_MOD}/src/kmod/wifi-ethernet/wifi-ethernet.ko ${KMOD_EXPORT_DIR}/
dba: kmod
	@echo "Building DBA kernel module."
	$(MAKE) -C ${DBA_MOD}
	cp ${DBA_MOD}/dba.ko ${KMOD_EXPORT_DIR}/
kpppoe: kmod
	@echo "Building pppoe kernel module."
	$(MAKE) -C ${PPPOE_KERNEL_MOD}
	cp ${PPPOE_KERNEL_MOD}/*.ko ${KMOD_EXPORT_DIR}/


ifeq (${EXP},1)
define UPDATEBUILDNO
	make updatebuildno
endef
else
define UPDATEBUILDNO
	echo "Skip update buildno"
endef
endif

updatebuildno:
	@if [ -d .git ] ; then \
		git pull ; \
	else \
		cvs update -A -C ${BUILDNOFILE} ; \
		if [ ! -f ${BUILDNOFILE} ] ; then \
			echo "###################################" ; \
			echo "${BUILDNOFILE} is missing, " ; \
			echo "Maybe buildnofile has been changed, please update buildtools and try again." ; \
			exit 1 ; \
		fi \
	fi
	@echo $$((`cat ${BUILDNOFILE}` +1 )) > ${BUILDNOFILE}
	@if [ -d .git ] ; then \
		git add ${BUILDNOFILE} ; \
		git commit -m "Increased buildno to `cat ${BUILDNOFILE}`" ; \
		git push ; \
	else \
		cvs commit -m "Build `cat ${BUILDNOFILE}` was performed." ${BUILDNOFILE} ; \
	fi

motd:
	@if [ -f  allbuildnoflag ] ; then \
		alreadyupdated=`cat allbuildnoflag` ; \
		if [ "$$alreadyupdated" = "0" ] ; then \
			${UPDATEBUILDNO} ; \
			echo 1 > allbuildnoflag ; \
		fi \
	else \
		${UPDATEBUILDNO} ; \
	fi
	> ${MOTDFILE}
#	@echo "`cat ${NAMFILE}` " > ${MOTDFILE}
#	@echo "`cat ${NAMFILE}` `cat ${VERFILE}` build `cat ${BUILDNOFILE}` `cat ${PRODUCTFILE}` `date`" > ${MOTDFILE}
	@echo "`whoami`@`hostname`:`tty`" > ${BUILDERFILE}
	cp ${BUILDNOFILE} ${ROOTFS_DIR}/files/etc/version/buildno
	cp ${PRODUCTFILE} ${ROOTFS_DIR}/files/etc/version/
	cat ${MOTDFILE}

bashtools:
	[ -d ${AUTEWAREFS_DIR}/files/usr ] || mkdir ${AUTEWAREFS_DIR}/files/usr
	[ -d ${AUTEWAREFS_DIR}/files/usr/bin ] || mkdir ${AUTEWAREFS_DIR}/files/usr/bin
	[ -d ${AUTEWAREFS_DIR}/files/usr/bin/cgi-bin ] || mkdir ${AUTEWAREFS_DIR}/files/usr/bin/cgi-bin
	[ -d ${AUTEWAREFS_DIR}/files/opt ] || mkdir ${AUTEWAREFS_DIR}/files/opt
	[ -d ${AUTEWAREFS_DIR}/files/opt/awk ] || mkdir ${AUTEWAREFS_DIR}/files/opt/awk
	[ -d ${AUTEWAREFS_DIR}/files/opt/services ] || mkdir ${AUTEWAREFS_DIR}/files/opt/services
	[ -d ${AUTEWAREFS_DIR}/files/opt/services/init ] || mkdir ${AUTEWAREFS_DIR}/files/opt/services/init


	-cp -rf ${BASHTOOLS_DIR}/*.sh ${AUTEWAREFS_DIR}/files/usr/bin/
	-cp -rf ${BASHTOOLS_DIR}/*.awk ${AUTEWAREFS_DIR}/files/opt/awk/
	-cp -rf ${SRVM_MOD}/res/*.sh ${AUTEWAREFS_DIR}/files/usr/bin/
	-rm -rf ${AUTEWAREFS_DIR}/files/opt/services
	-cp -rf ${BASHTOOLS_DIR}/services ${AUTEWAREFS_DIR}/files/opt/
	[ -d ${AUTEWAREFS_DIR}/files/usr ] || mkdir ${AUTEWAREFS_DIR}/files/usr
	[ -d ${AUTEWAREFS_DIR}/files/usr/bin ] || mkdir ${AUTEWAREFS_DIR}/files/usr/bin
	[ -d ${AUTEWAREFS_DIR}/files/usr/bin/cgi-bin ] || mkdir ${AUTEWAREFS_DIR}/files/usr/bin/cgi-bin
	-cp -rf ${BASHTOOLS_DIR}/cgi-bin/* ${AUTEWAREFS_DIR}/files/usr/bin/cgi-bin
	-chmod 755 ${AUTEWAREFS_DIR}/files/usr/bin/cgi-bin/*
	-find ${AUTEWAREFS_DIR}/files/opt -name CVS | xargs rm -rf
	-chmod 755 ${AUTEWAREFS_DIR}/files/usr/bin/*
	-chmod 755 ${AUTEWAREFS_DIR}/files/opt/services/init/*
	if [ "x$(OLDEAG)" == "x1" ]; then\
		cp -rf ${BASHTOOLS_DIR}/services/init/eag_init_old ${ROOTFS_DIR}/files/opt/services/init/eag_init;\
	fi



pubkmod: ${PREREQ_IPTABLES} ${PREREQ_WIFIKMOD} ${PREREQ_IPFWD_LEARN}
	@echo "Copying kernel modules ..."
	-cd ${KERNEL_ROOT} && find . -name "*.ko" | xargs cp --parents --target-directory="${ROOTFS_KMOD_DIR}/kernel";
	-cp -RP auteware/files/kmod/* ${ROOTFS_KMOD_DIR}/misc




preparedirs: prepare_config
	[ -d ${ROOTFS_DIR}/files/dev ] || mkdir -p ${ROOTFS_DIR}/files/dev
	[ -e ${ROOTFS_DIR}/files/dev/console ] || mknod ${ROOTFS_DIR}/files/dev/console c 5 1 
	[ -e ${ROOTFS_DIR}/files/dev/null ] || mknod ${ROOTFS_DIR}/files/dev/null c 1 3 
	[ -e ${ROOTFS_DIR}/files/dev/ttyS0 ] || mknod ${ROOTFS_DIR}/files/dev/ttyS0 c 4 64 
	[ -d ${ROOTFS_DIR}/files/dev/shm ] || mkdir -p ${ROOTFS_DIR}/files/dev/shm
	[ -d ${ROOTFS_DIR}/files/dev/pts ] || mkdir -p ${ROOTFS_DIR}/files/dev/pts
	[ -d ${AUTEWAREFS_DIR}/files/opt ] || mkdir -p ${AUTEWAREFS_DIR}/files/opt
	[ -d ${AUTEWAREFS_DIR}/files/opt/bin ] || mkdir -p ${AUTEWAREFS_DIR}/files/opt/bin
	[ -d ${AUTEWAREFS_DIR}/files/opt/lib ] || mkdir -p ${AUTEWAREFS_DIR}/files/opt/lib
	[ -d ${AUTEWAREFS_DIR}/files/opt/etc ] || mkdir -p ${AUTEWAREFS_DIR}/files/opt/etc
	[ -d ${AUTEWAREFS_DIR}/files/opt/awk ] || mkdir -p ${AUTEWAREFS_DIR}/files/opt/awk
	[ -d ${ROOTFS_DIR}/files/lib/init/rw ] || mkdir -p ${ROOTFS_DIR}/files/lib/init/rw
	[ -d ${ROOTFS_DIR}/files/var ] || mkdir -p ${ROOTFS_DIR}/files/var
	[ -d ${ROOTFS_DIR}/files/var/run ] || mkdir -p ${ROOTFS_DIR}/files/var/run
	[ -d ${ROOTFS_DIR}/files/var/lib ] || mkdir -p ${ROOTFS_DIR}/files/var/lib
	[ -d ${ROOTFS_DIR}/files/root ] || mkdir -p ${ROOTFS_DIR}/files/root
	[ -d export ] || mkdir export
	[ -d auteware/files/opt ] || mkdir auteware/fileopt
	[ -d auteware/files/opt/bin ] || mkdir auteware/files/opt/bin
	[ -d auteware/files/opt/lib ] || mkdir auteware/files/opt/lib
	[ -d auteware/files/opt/lib/iptables ] || mkdir auteware/files/opt/lib/iptables
#	[ -d auteware/files/opt/lib/ipset ] || mkdir auteware/files/opt/lib/ipset
	[ -d auteware/files/opt/awk ] || mkdir auteware/files/opt/awk	
	[ -d auteware/files/opt/www ] || mkdir auteware/files/opt/www
	[ -d auteware/files/opt/www/ccgi-bin ] || mkdir auteware/files/opt/www/ccgi-bin
	[ -d auteware/files/kmod ] || mkdir auteware/files/kmod

prepare_config:
	@echo " prepare configs for private" 
	@echo "/* Automatically generated  header file.*/" > ${ACCAPI_PB_DIR}/config/auteware_config.h
	@echo "/* Automatically generated  header file for NPD.*/" > ${ACCAPI_PB_DIR}/config/npd_config.h
	@echo "/* Automatically soft link form accapi_pb to accapi */"
	./link-accapi.sh link
ifeq (${WEB},1)
	@echo "#define __WITH_AUTEWARE_WEB 1" >> ${ACCAPI_PB_DIR}/config/auteware_config.h
endif

ifeq (${AP_MAX_FLAG},1)
	@echo "#define __AP_MAX_COUNTER 2048" >> ${ACCAPI_PB_DIR}/config/auteware_config.h
endif
ifeq (${AP_MAX_FLAG},2)
	@echo "#define __AP_MAX_COUNTER 4096" >> ${ACCAPI_PB_DIR}/config/auteware_config.h
endif
	
ifeq (${CSCD},1)
	@echo "#define __WITH_NPD_CSCD 1" >> ${ACCAPI_PB_DIR}/config/npd_config.h
endif

pubapps: preparedirs  ${PREREQ_DBUS} ${PREREQ_PFM} ${PREREQ_SFD} ${PREREQ_QUAGGA} ${PREREQ_DCLI} ${PREREQ_HBIP}  ${PREREQ_HMD} ${PREREQ_BSD} ${PREREQ_WCPSS} ${PREREQ_ASD} ${PREREQ_HAD} ${PREREQ_IUH} ${PREREQ_RANAPPROXY} ${PREREQ_SRVM} ${PREREQ_SNMP} ${PREREQ_DHCP} ${PREREQ_DHCP6} ${PREREQ_DHCPSNP} ${PREREQ_IGMP} ${PREREQ_SE_AGENT} ${PREREQ_PPPOE} ${PREREQ_SYSLOG} bashtools 
	@echo "Copying pubapps files ..."
	#cp -RP auteware/files/opt/bin ${ROOTFS_DIR}/files/opt
	#cp -RP auteware/files/opt/lib ${ROOTFS_DIR}/files/opt

x7x5web: 
	@echo "Making cgi for x7x5 web ..."
	$(MAKE) -C ${CCGI_MOD}/cgic205
#	@echo "Do not copy ccgi to ramfs ."
	cp ${CCGI_MOD}/cgic205/*.cgi ${WWW_EXPORT_DIR}/ccgi-bin
	cp -P ${CCGI_MOD}/cgic205/libcgic.so* ${LIB_EXPORT_DIR}
	cp -RP ${CCGI_MOD}/htdocs ${WWW_EXPORT_DIR}/
	-find ${WWW_EXPORT_DIR} -name CVS | xargs rm -rf
	@echo "Copying x7x5 web files info rootfs"
#	cp -RP ${WWW_EXPORT_DIR} ${ROOTFS_DIR}/files/opt/
#	cp -RP ${LIB_EXPORT_DIR}/libcgic.so* ${ROOTFS_DIR}/files/opt/lib/
	@echo "Copying x7x5 timezone files info etc"
	-cp -f ${ROOTFS_DIR}/files/usr/share/zoneinfo/Etc/UTC ${ROOTFS_DIR}/files/etc/localtime
	-echo "Etc/UTC" > ${ROOTFS_DIR}/files/etc/timezone

x3web:x7x5web
	rm -rf ${AUTEWAREFS_DIR}/files/opt/www/htdocs/images/7605
	rm -rf ${AUTEWAREFS_DIR}/files/opt/www/htdocs/images/crmsu
	rm -rf ${AUTEWAREFS_DIR}/files/opt/www/htdocs/images/GTX
	rm -rf ${AUTEWAREFS_DIR}/files/opt/www/htdocs/images/new5000
	rm -rf ${AUTEWAREFS_DIR}/files/opt/www/htdocs/images/panel
	rm -rf ${AUTEWAREFS_DIR}/files/opt/www/htdocs/images/SFP
	rm -rf ${AUTEWAREFS_DIR}/files/opt/www/htdocs/images/sys5000
	rm -rf ${AUTEWAREFS_DIR}/files/opt/www/htdocs/images/XFP

reinitweb:
	-rm -rf ${AUTEWAREFS_DIR}/files/opt/www
	-rm -rf ${AUTEWAREFS_DIR}/files/opt/lib/libcgic.so*

splitimg:

	[ -d ${AUTEWAREFS_DIR} ] || mkdir -p ${AUTEWAREFS_DIR}
	[ -d ${AUTEWAREFS_DIR}/files ] || mkdir -p ${AUTEWAREFS_DIR}/files
	[ -d ${AUTEWAREFS_DIR}/files/etc ] || mkdir -p ${AUTEWAREFS_DIR}/files/etc
	[ -d ${ROOTFS_DIR}/files/etc/version ] || mkdir -p ${ROOTFS_DIR}/files/etc/version
	[ -d ${AUTEWAREFS_DIR}/files/etc/init.d ] || mkdir -p ${AUTEWAREFS_DIR}/files/etc/init.d
	[ -d ${AUTEWAREFS_DIR}/files/etc/rc2.d ] || mkdir -p ${AUTEWAREFS_DIR}/files/etc/rc2.d
	[ -d ${AUTEWAREFS_DIR}/files/opt ] || mkdir -p ${AUTEWAREFS_DIR}/files/opt
	#rm -rf ${AUTEWAREFS_DIR}/files/opt/
	#mv -f ${ROOTFS_DIR}/files/opt/ ${AUTEWAREFS_DIR}/files/opt/
	#-cp -f ${BIN_EXPORT_DIR}/* ${AUTEWAREFS_DIR}/files/opt/bin/	
	#-cp -f -u ${ROOTFS_DIR}/files/etc/init.d/* ${AUTEWAREFS_DIR}/files/etc/init.d/
#	cp -rf ${ROOTFS_DIR}/files/etc/init.bk/* ${ROOTFS_DIR}/files/etc/init.d/
	-mv -f -u ${ROOTFS_DIR}/files/etc/rc2.d/* ${AUTEWAREFS_DIR}/files/etc/rc2.d/ 


reinitnpdsuit:
	-rm -rf ${AUTEWAREFS_DIR}/files/opt/bin/npd
	-rm -rf ${AUTEWAREFS_DIR}/files/opt/lib/libmcpss*
	-rm -rf ${AUTEWAREFS_DIR}/files/opt/lib/libmbcm*
	-rm -rf ${AUTEWAREFS_DIR}/files/opt/lib/libnam*
	-rm -rf ${AUTEWAREFS_DIR}/files/opt/lib/libnbm*
#	-rm -rf ${ROOTFS_KMOD_DIR}/misc/bm.ko
#	-rm -rf ${ROOTFS_KMOD_DIR}/misc/kapDrv.ko
#	-rm -rf ${ROOTFS_KMOD_DIR}/misc/mvPpDrv.ko
#	-rm -rf ${ROOTFS_KMOD_DIR}/misc/linux-kernel-bde.ko
#	-rm -rf ${ROOTFS_KMOD_DIR}/misc/linux-user-bde.ko

reinitpubkmod:
	-rm -rf ${KMOD_EXPORT_DIR}/*
	-rm -rf ${AUTEWAREFS_DIR}/files/lib/modules/*


reinitxfiles: reinitnpdsuit reinitpubkmod 
	@echo "Cleaned x specified files..."

compressimg:
	@echo "Copy generated vmlinux file to ${DESTDIR}"
	if [ ! -d ${DESTDIR} ]; then mkdir -p ${DESTDIR} ; chmod 777 ${DESTDIR} ; fi
	cp ${KERNEL_ROOT}/vmlinux ${DESTDIR}/${IMGNAME}.ELF
	ls -lh ${DESTDIR}/${IMGNAME}*
	-rm -rf release
	mkdir release
	cp ${KERNEL_ROOT}/vmlinux release/
	cp -rp ${AUTEWAREFS_DIR} release/
	cp -rp ${FASTFWD_MOD}/fastfwd*  release/
	@echo "Making image for u-boot." 
	./makeimg.sh ${IMGNAME}
	ls -lh release
	cp release/fastfwd ${DESTDIR}/${IMGNAME}.fastfwd.bin
	cp release/AW.IMG ${DESTDIR}/${IMGNAME}.IMG
	pushd ${SNMPMIBS_DIR} && tar cvf ${DESTDIR}/${IMGNAME}.MIB.tar * && popd
	@echo "WWW_DAILYBUILDDIR is ${WWW_DAILYBUILDDIR}"
	@if [ -n "${WWW_DAILYBUILDDIR}" ] ; then  [ -d ${WWW_DAILYBUILDDIR} ] || mkdir ${WWW_DAILYBUILDDIR}; cp release/AW.IMG ${WWW_DAILYBUILDDIR}/${IMGNAME}.IMG ; fi
	ls -lh ${DESTDIR}/${IMGNAME}*

x3img: pubapps 
	@echo "Reinit x specified files ..."
	make reinitxfiles
	@echo "clean mv files"
	-make cleannpdsuit_mv
	@echo "Making npdsuit bcm"
	make npdsuit_bcm
	@echo "Making x3kmod"
	make x3kmod
	@echo "Making web console for x3"
	#make x3web	
	@echo "Preparing product info"
	@echo X3 > ${PRODUCTFILE}
	make motd
	make -C ${ROOTFS_DIR}/pkgs installleastpkgs	
	make -C ${KERNEL_ROOT}
	make IMGNAME="AW`cat ${VERFILE}`.`cat ${BUILDNOFILE}`.`cat ${PRODUCTFILE}`" compressimg
	mv ${KERNEL_ROOT}/vmlinux release/AW.X3.ELF
	
x7x5img: pubapps 
	@echo "Reinit x specified files ..."
	make reinitxfiles
	@echo "Making bm module"
	make bm
	@echo "Making kes module"
	make kes
	@echo "clean bcm files"
	-make cleanbcm_npdsuit
	@echo "Making npdsuit marvell"
	make npdsuit_mv
	make stpsuit
	make kmod
	@echo "making ksem"
	make ksem
	@echo "Making sem"
	make sem
	@echo "Making pfm_kern module"
	make pfm_kern	
	@echo "Making e1000e module"
	make sfd_kern
	@echo "Making sfd_kern module"
#	make e1000e
	@echo "Makeing dba module"
	make dba
	@echo "Making rpa module"
	make kpppoe
	@echo "Making kpppoe module"
	make oct_nic
	@echo "Making oct-nic module"
	make rpa
	@echo "Making cavium-ratelimit module"
	make cavium-ratelimit

	@echo "Making pubkmod"
	make pubkmod
	@echo "Making web console for x7x5"
	make x7x5web
	@echo "Preparing product info"
	@echo X7X5 > ${PRODUCTFILE}
	make fast-fwd
	make motd
	make -C ${ROOTFS_DIR}/pkgs installpkgs	
ifdef mtrace
	make -C ${ROOTFS_DIR}/pkgs inst_mtrace
endif
	make splitimg
	make -C ${KERNEL_ROOT}

ifeq (${EXP},1)
	make IMGNAME="AW`cat ${VERFILE}`.`cat ${BUILDNOFILE}`.`cat ${PRODUCTFILE}`" compressimg
else
	make IMGNAME="AW`cat ${VERFILE}`.`cat ${BUILDNOFILE}`.`cat ${PRODUCTFILE}`.`date +%d%H%M`" compressimg
endif
	mv ${KERNEL_ROOT}/vmlinux release/AW.X7X5.ELF

ifeq (${R},1)
define UPDATEMOTD
	make motd
	make -C ${KERNEL_ROOT}
endef
else
define UPDATEMOTD
	echo "Skip update buildno"
endef
endif

recompressimg:
	${UPDATEMOTD}
	make IMGNAME="AW`cat ${VERFILE}`.`cat ${BUILDNOFILE}`.`cat ${PRODUCTFILE}`" compressimg
	mv ${KERNEL_ROOT}/vmlinux release/AW.X7X5.ELF

allbuildno:
	-rm -rf allbuildnoflag
	@echo 0 > allbuildnoflag

awimgs: allbuildno x3img x7x5img
	-rm -rf allbuildnoflag
	@echo "Finished making images"
	@ls -lh ${DESTDIR}/AW`cat ${VERFILE}`.`cat ${BUILDNOFILE}`*

experiment: 
	@echo "================================================="
	@echo "This target is only for quick debug usage,"
	@echo "which might cause unexpected problems. "
	@echo "Use it when you know what you are doing."
	@echo "================================================="
	@echo "Makeing experimental elf"
	make -C ${KERNEL_ROOT}
	@echo "Copying elf"
	-rm -rf release
	mkdir release
	cp ${KERNEL_ROOT}/vmlinux release/AW.X3.ELF
	cp ${KERNEL_ROOT}/vmlinux release/AW.X7X5.ELF

cleandcli:cleandclipub_ac
	$(MAKE) -C ${DCLI_MOD}/src/lib clean
cleandclipub_ac:
	$(MAKE) -C ${DCLI_MOD}/src/pub clean

cleannpdsuit_mv:
	#$(MAKE) -C ${BM_KMOD}/src/kmod clean
	#$(MAKE) -C ${BM_KMOD}/src/app clean
	$(MAKE) -C ${NPDSUIT_MV_MOD}/nam_ax7/src/lib clean
	$(MAKE) -C ${NPDSUIT_MV_MOD}/nbm_ax7/src/lib clean
	$(MAKE) -C ${NPDSUIT_MV_MOD}/npd/src/app clean
	$(MAKE) -C ${NPDSUIT_MV_MOD}/${MCPSSPATH}/src/drv clean
	$(MAKE) -C ${NPDSUIT_MV_MOD}/mcpss3.4/src/drv clean
	$(MAKE) -C ${NPDSUIT_MV_MOD}/npd/src/dcli/ clean

cleandrp:
	$(MAKE) -C ${DRP_MOD}/src clean
	
cleantrap-helper:cleanlibnm
	$(MAKE) -C ${TRAP_HELPER_MOD}/ clean

cleanacsample:cleanlibnm
	$(MAKE) -C ${ACSAMPLE_MOD} clean

cleanacmanage:cleanlibnm
	$(MAKE) -C $(SNMPMANAGE_MOD) clean

cleansnmp:cleanlibnm cleanacmanage cleantrap-helper cleanacsample
	$(MAKE) -C ${SUBAGENT_MOD} clean

cleanwcpss:
	$(MAKE) -C ${WCPSS_MOD}/src/dcli/ clean
	$(MAKE) -C ${WCPSS_MOD}/src/app/pub clean
	$(MAKE) -C ${WCPSS_MOD}/src/app/wid clean
	$(MAKE) -C ${WCPSS_MOD}/src/app/wsm clean

cleaniuh:
	$(MAKE) -C ${IUH_MOD} clean
cleanranapproxy:
	$(MAKE) -C ${IU_MOD} clean
cleanipfwd_learn:
	echo "Cleanning ipfwd_learn ..."
	$(MAKE) -C ${IPFWD_LEARN_KMOD} clean
cleanrpa:
	echo "Cleanning rpa ..."
	$(MAKE) -C ${RPA_MOD}/src/kmod clean
	$(MAKE) -C ${RPA_MOD}/src/app clean

cleanwifikmod:
	$(MAKE) -C ${WCPSS_MOD}/src/kmod/wifi-ethernet/ clean

cleansfd:
	echo "Cleanning sfd ..."
	$(MAKE) -C $(SFD_MOD) clean
	-rm ${KMOD_EXPORT_DIR}/sfd_kern.ko
	-rm ${BIN_EXPORT_DIR}/sfd_daemon

cleancvm-rate:
	@echo "Cleanning cvm-rate app module ..."
	$(MAKE) -C ${CAVIUM_RATELIMIT_APP} clean
cleancavium-ratelimit: cleancvm-rate
	@echo "Cleanning cavium-ratelimit module ..."
	$(MAKE) -C ${CAVIUM_RATELIMIT_MOD} clean

cleandba:
	@echo "Cleanning dba module ..."
	$(MAKE) -C ${DBA_MOD} clean

cleankpppoe:
	@echo "Cleanning pppoe module ..."
	$(MAKE) -C ${PPPOE_KERNEL_MOD} clean
	
cleanasd:
	@echo "Cleaning dcli_asd..."
	$(MAKE) -C ${ASD_MOD}/src/dcli/ clean
	@echo "Cleaning asd..."
	$(MAKE) -C ${ASD_MOD}/src/app clean

cleanhad:
	echo "Cleaning HAD daemon..."
	${MAKE} -C ${HAD_MOD}/src/lib clean

cleanhmd:
	echo "Cleaning HMD daemon..."
	${MAKE} -C ${HMD_MOD} clean
cleanwbmd:
	echo "Cleaning WBMD daemon..."
	${MAKE} -C ${WBMD_MOD} clean
cleanbsd:
	echo "Cleaning BSD daemon..."
	${MAKE} -C ${BSD_MOD}/src clean


cleanccgi:cleanlibnm
	echo "Cleanning ccgi ..."  
	$(MAKE) -C ${CCGI_MOD}/cgic205 clean 
#	-$(MAKE) -C ${CCGI_MOD}/cgic205/portal clean
	-$(MAKE) -C ${CCGI_MOD}/cgic205/snmp_agent clean
	-$(MAKE) -C $(CCGI_MOD)/cgic205/wireless clean	

cleanlibnm:
	echo "Cleaning libnm ..."
	$(MAKE) -C $(LIBNM_MOD) clean
	
cleansrvm:
	echo "Cleanning srvm ..."
	$(MAKE) -C ${SRVM_MOD}/app clean
	
cleanigmp:
	echo "Cleaning igmp snooping..."
	$(MAKE) -C ${IGMP_MOD}/ clean

cleaniptables:
	echo "Cleanning iptables ..."
	$(MAKE) -C ${IPTABLES_DIR} clean
	$(MAKE) -C ${IPTABLES_MOD}/external/ipset-6.19 clean
	$(MAKE) -C ${IPTABLES_MOD}/external/ipset-6.19 modules_clean
	$(MAKE) -C ${IPTABLES_MOD}/external/ipp2p-0.99.15 clean
	$(MAKE) -C ${IPTABLES_MOD}/external/libmnl-1.0.1 clean
cleancaptive:
	@echo "Cleaning captive portal ..."

cleaneag:
	@echo "Cleaning easy access gateway ..."
	-$(MAKE) -C ${EAG_MOD}/pkg distclean
	-$(MAKE) -C ${EAG_MOD}/pkg/eag clean

cleanpppoe:
	@echo "Cleaning pppoe ..."
	-$(MAKE) -C ${PPPOE_MOD} clean

cleandhcp:
	@echo "Cleanning dhcp ..."
	if [ -f ${DHCP_MOD}/Makefile ] ; then 	\
		pushd ${DHCP_MOD}; 		\
		make clean ; 			\
		rm -rf Makefile ;	\
		popd ; 				\
	else  					\
		echo "clean dhcp but not configured." ; \
	fi

cleandhcp6:
	@echo "Cleanning dhcp6 ..."
	if [ -f ${DHCP6_MOD}/Makefile ] ; then 	\
		pushd ${DHCP6_MOD}; 		\
		make clean ; 			\
		rm -rf Makefile ;	\
		popd ; 				\
	else  					\
		echo "clean dhcp6 but not configured." ; \
	fi
	-rm ${AUTEWAREFS_DIR}/files/opt/bin/dhcp6d
	-rm ${AUTEWAREFS_DIR}/files/opt/bin/dhcrelay6

cleanse_agent:
	@echo "Cleaning se_agent module..."
	$(MAKE) -C ${SE_AGENT_MOD} OCTEON_TARGET=linux_64  clean
	
cleanfast-fwd:
	@echo "Cleanning fast fwd..."
	$(MAKE) -C ${FASTFWD_MOD} clean
	$(MAKE) -C ${FASTFWD_MOD} clean PREFIX=_standalone

cleanbcm_npdsuit: 
	$(MAKE) cleanbcmkmod
	@echo "Cleaning bcm utilities ..."
	$(MAKE) -C $(BCM_MOD)/systems/linux/user/nsx64 clean
	-@rm -rf $(BCM_MOD)/build
	$(MAKE) -C ${NPDSUIT_BCM_MOD}/nam_ax7/src/lib clean
	$(MAKE) -C ${NPDSUIT_BCM_MOD}/nbm_ax7/src/lib clean
	$(MAKE) -C ${NPDSUIT_BCM_MOD}/npd/src/app clean

cleanmvkmod:
	$(MAKE) -C ${NPDSUIT_MV_MOD}/${MCPSSPATH}/src/kmod/mvPpDrv clean
	$(MAKE) -C ${NPDSUIT_MV_MOD}/kap/src/kmod clean

cleanbcmkmod:
	@echo "Cleaning bcm kmod utilities ..."
	$(MAKE) -C $(BCM_MOD)/systems/bde/linux/bdekmod/kernel clean
	$(MAKE) -C $(BCM_MOD)/systems/bde/linux/bdekmod/user clean
	$(MAKE) -C ${NPDSUIT_BCM_MOD}/kap/src/kmod clean
	#$(MAKE) -C ${NPDSUIT_BCM_MOD}/bm/src/kmod clean
	#$(MAKE) -C ${NPDSUIT_BCM_MOD}/bm/src/app clean

cleanstpsuit:		
	echo "Cleaning stp suit..."
	$(MAKE) -C ${STPSUIT_MOD}/src clean

cleansyslog-ng:
	echo "Cleanning syslog-ng suit..."
	$(MAKE) -C ${SYSLOG_MOD}/ clean
	-rm ${ROOTFS_DIR}/files/sbin/syslog-ng
	#-rm -rf ${ROOTFS_DIR}/files/etc/syslog-ng

cleanquagga:
	echo "Cleanning quagga ..."
	#$(MAKE) -C ${QUAGGA_MOD}/${BUILDDIR} clean
	rm -rf ${QUAGGA_MOD}/${BUILDDIR}
cleanksem:
	echo "cleanning ksem ..."
	$(MAKE) -C $(KSEM_MOD) clean
	
cleansem:
	$(MAKE) -C ${SEM_MOD}/src/app/ clean
	$(MAKE) -C ${SEM_MOD}/src/dcli clean
	$(MAKE) -C ${SEM_MOD}/src/pub clean

clean_e1000e:
	@echo "Cleanning e1000e module ... "
	$(MAKE) -C ${E1000E_KMOD} clean
	
cleanoct_nic:
	$(MAKE) -C ${OCTEON_BOOTDIR}/components/driver all_clean
	
cleanbm:
	$(MAKE) -C ${BM_KMOD}/src/kmod clean
	$(MAKE) -C ${BM_KMOD}/src/app clean
cleankes:
	$echo "Cleaning kes module ..."
	$(MAKE) -C ${KES_MOD} clean
cleankernel: revert-kernel-se-files
	$(MAKE) -C ${KERNEL_ROOT} clean

cleanpfm:
	echo "Cleanning pfm ..."
	$(MAKE) -C $(PFM_MOD) clean
	-rm ${KMOD_EXPORT_DIR}/pfm_kern.ko
	-rm ${BIN_EXPORT_DIR}/pfm_deamon
cleandbus:
	$(MAKE) -C ${DBUS_MOD} clean
	-rm ${BIN_EXPORT_DIR}/dbus-daemon
	-rm ${LIB_EXPORT_DIR}/libdbus-1.so*

cleanmvkmod:
	#	$(MAKE) -C ${NPDSUIT_MV_MOD}/mcpss/src/kmod/mvPpDrv clean
#	$(MAKE) -C ${NPDSUIT_MV_MOD}/kap/src/kmod clean
#	$(MAKE) -C ${NPDSUIT_MV_MOD}/bm/src/kmod clean
#	$(MAKE) -C ${NPDSUIT_MV_MOD}/bm/src/app clean


cleankmod: ${CLEAN_BM} ${CLEAN_IPFWD_LEARN}
	echo "Cleaning kernel modules ..."
cleanapps: ${CLEAN_QUAGGA} ${CLEAN_DCLI} ${CLEAN_SEM} ${CLEAN_WCPSS} ${CLEAN_ASD} ${CLEAN_IUH} ${CLEAN_RANAPPROXY} ${CLEAN_CCGI} ${CLEAN_IPTABLES} ${CLEAN_CAPTIVE} ${CLEAN_EAG} ${CLEAN_SNMP} ${CLEAN_SE_AGENT} ${CLEAN_FASTFWD} ${CLEAN_PPPOE} ${CLEAN_CAVIUM_RATELIMIT} ${CLEAN_SYSLOG}
	-rm -rf $(LIB_EXPORT_DIR)/* $(BIN_EXPORT_DIR)/* $(KMOD_EXPORT_DIR)/*

#cleanall: cleanapps cleankernel
cleanall: cleanapps cleankmod cleankernel

buildroot2rootfs: busyroot2rootfs ${BUILDROOTDIR}/.timestamp .buildroot2rootfstimestamp
	[ -d ${ROOTFS_DIR}/files/usr/lib ] || mkdir -p ${ROOTFS_DIR}/files/usr/lib
	-cp -RPf ${BUILDROOTDIR}/usr/lib/* ${ROOTFS_DIR}/files/usr/lib/
	[ -d ${ROOTFS_DIR}/files/usr/bin ] || mkdir -p ${ROOTFS_DIR}/files/usr/bin
	-cp -RPf ${BUILDROOTDIR}/usr/bin/* ${ROOTFS_DIR}/files/usr/bin/
	-[ -d ${ROOTFS_DIR}/usr/sbin ] || mkdir -p ${ROOTFS_DIR}/files/usr/sbin
	-cp -RPf ${BUILDROOTDIR}/usr/sbin/* ${ROOTFS_DIR}/files/usr/sbin/
	-cp -RPf ${BUILDROOTDIR}/etc/* ${ROOTFS_DIR}/files/etc/
	-touch .buildroot2rootfstimestamp

busyroot2rootfs: ${BUSYROOTDIR}/.timestamp .busyroot2rootfstimestamp
	[ -d ${ROOTFS_DIR} ] || mkdir -p ${ROOTFS_DIR}
	[ -d ${ROOTFS_DIR}/files ] || mkdir -p ${ROOTFS_DIR}/files
	-cp -P ${BUSYROOTDIR}/files/*i* ${ROOTFS_DIR}/files/
	[ -d ${ROOTFS_DIR}/files/bin ] || mkdir -p ${ROOTFS_DIR}/files/bin
	-cp -P ${BUSYROOTDIR}/files/bin/* ${ROOTFS_DIR}/files/bin/
	[ -d ${ROOTFS_DIR}/files/etc ] || mkdir -p ${ROOTFS_DIR}/files/etc
	-cp -RP ${BUSYROOTDIR}/files/etc/* ${ROOTFS_DIR}/files/etc/
	[ -d ${ROOTFS_DIR}/files/sbin ] || mkdir -p ${ROOTFS_DIR}/files/sbin
	-cp -P ${BUSYROOTDIR}/files/sbin/* ${ROOTFS_DIR}/files/sbin/
	[ -d ${ROOTFS_DIR}/files/lib ] || mkdir -p ${ROOTFS_DIR}/files/lib
	-cp -P ${BUSYROOTDIR}/files/lib/* ${ROOTFS_DIR}/files/lib/
	[ -d ${ROOTFS_DIR}/files/proc ] || mkdir -p ${ROOTFS_DIR}/files/proc
	[ -d ${ROOTFS_DIR}/files/sys ] || mkdir -p ${ROOTFS_DIR}/files/sys
	[ -d ${ROOTFS_DIR}/files/tmp ] || mkdir -p ${ROOTFS_DIR}/files/tmp
	[ -d ${ROOTFS_DIR}/files/etc/version ] || mkdir -p ${ROOTFS_DIR}/files/etc/version
	[ -d ${ROOTFS_DIR}/files/mnt] || mkdir -p ${ROOTFS_DIR}/files/mnt
	-touch .busyroot2rootfstimestamp

.busyroot2rootfstimestamp:
	@touch $@

.buildroot2rootfstimestamp:
	@touch $@

cleanrootfs:
	rm .busyroot2rootfstimestamp
	rm .buildroot2rootfstimestamp

archives:
	@echo "Cleanning all obj files..."
	@make cleanall
	@echo "Cleanning release files..."
	-@rm -rf release
	@echo "Cleanning export files..."
	-@rm -rf export
	@echo "Cleanning dynamic kernel module files..."
	-@rm -rf rootfs/files/lib/modules
	@echo "Cleanning opt files..."
	-@rm -rf rootfs/files/opt
	@echo "Compress all files..."
	@tar -cvjf ../`date +%Y%m%d%H%M%S`.tar.bz2 ./
	@echo "Make archives done."
	@ls -lah ../
