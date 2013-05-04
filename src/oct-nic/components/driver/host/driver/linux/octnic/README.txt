Linux PCI Driver for OCTEON (NIC Module - Version 2.2.0)
===========================================================

	Table of contents:

	1. What's in this release?
	2. System Requirements
	3. Installing the RPM
	4. Compiling the sources
		a. Top level Makefile
		b. Compiling the PCI NIC Driver Module
		c. Compiling the core driver for NIC module
	6. Loading the OCTEON NIC module
	7. EtherPCI
	8. Enabling additional features in the driver





	1. What's in this release?
	==========================
	
	This release  provides  the  sources for the NIC module of  OCTEON  PCI
	driver loadable on  a Linux  Host System. Also included are sources for
	a sample  OCTEON  core  application  that can be loaded on OCTEON cores
	to  use  the  PCI NIC  module. The  NIC driver has support for multiple
	OCTEON devices, hot-swap capability and link status monitoring.

	This release adds support for CN66XX.

	Please refer to the release notes for list of changes in this release.

	The  NIC  driver  uses  the  interfaces provided by the OCTEON PCI BASE
	driver.	Refer  to  the  README.txt file at OCTEON-SDK/components/driver
	to	know more about the BASE driver usage and compilation instructions.





	2. System Requirements
	======================

	The NIC module  for OCTEON PCI Host driver is meant for use when OCTEON
	is  used  in  PCI  target  mode. The  driver can be loaded on any Linux
	system running kernel versions between 2.6.18 to 2.6.32. The NIC driver
	uses the services provided by OCTEON base driver for communication with
	OCTEON.	The  base  driver has to be loaded before the NIC module can be
	loaded.

	The driver has been tested on SL 5.5 (2.6.18), SL 6.0 (2.6.32) & OCTEON
	SDK 2.1 LINUX.
	
	
	
	
	
	3. Installing the RPM
	=====================
	
	The OCTEON PCI NIC RPM requires the following packages from Cavium
	Networks to be installed on the system:
		1) OCTEON SDK version 2.2.0
		2) OCTEON-COMPONENTS-COMMON version 2.2.0
		3) OCTEON-PCI-BASE version 2.2.0
  

	Install the RPM
	cmd> rpm -i OCTEON-PCI-NIC-2.2.0-83.i386.rpm

	The path to source and documentation is printed at end of installation.


	4. Sources
	==========

	After installation, the source files for the driver are available at 
		a) OCTEON-SDK/components/driver/host/driver/linux/octnic (host driver)
		b) OCTEON-SDK/components/driver/core (core driver).
		c) The core application included with the package would get
		   installed at OCTEON-SDK/applications/pci-core-app/nic.


	
	
	5. Compiling the sources
	========================

		a) Top level Makefile
		---------------------

		The top level Makefile at OCTEON-SDK/components/driver can be used
		to  compile  the host driver and OCTEON application using a single
		command at OCTEON-SDK/components/driver.
		cmd> make -s nic

		The next sections describe the build process if you need to build
		each object in the NIC package separately.

		IMPORTANT: Compile the BASE driver module first before proceeding
		           to step (b).
		           The base driver can be built by doing make at 
		           OCTEON-SDK/components/driver/host/driver/linux.



		b) Compiling the PCI NIC Driver Module
		--------------------------------------

		The NIC module can be compiled by doing "make" at 
		OCTEON-SDK/components/driver/host/driver/linux/octnic.

		The NIC module is available at octnic.ko in the above directory. A
		symbolic link to the object file will be added from 
		OCTEON-SDK/components/driver/bin.

		NOTE: On older kernels, you will see warnings for undefined symbols
		      at the end of compilation. These  symbols  are defined in the
		      base driver available from the  OCTEON-PCI-BASE  package. The
		      kernel  takes  care  of making these symbols available to the
		      NIC module once the BASE module has been loaded.



		c) Compiling the core driver for NIC module
		-------------------------------------------

		NOTE: Before you can compiling the core driver sources, you should
		      source the "env-setup" script located at
		      /usr/local/Cavium_Networks/OCTEON-SDK to setup the
		      environment variables for building OCTEON	applications.
			
			For, e.g., to prepare for a OCTEON CN56XX board, do
			cmd> source env_setup OCTEON_CN56XX

		The core pci driver, like the simple executive files, are compiled
		into libraries which get linked to the core application at compile
		time. You do not have to do a separate make for the core driver.

		This  package  includes a sample simple executive (SE) application
		installed at OCTEON-SDK/applications/pci-core-app/nic. Compile the
		core application by running 'make' in the above	directory.

		The  core  application  Makefile includes the core driver makefile
		fragment file - core-drv.mk. The  make process compiles the driver
		sources and creates	a library which gets linked with the
		application.
	
		cvmcs-nic.bin is the original application binary. cvmcs-nic.strip
		is the stripped version (no symbol information) of the application.






	6. Loading the OCTEON NIC module
	================================

	The  OCTEON  base driver module and core application  should be loaded
	before  the  NIC driver module can be loaded.  Follow  the steps below
	to load the NIC module.

	1. Load the base driver module.
	cmd> insmod octeon_drv.ko


	2. Load the core application.
	NOTE: The  oct-pci-boot command below should  be  run only if the card
	      is configured for pci-boot mode.
	cmd> oct-pci-boot
	cmd> oct-pci-load  0x20000000  ./cvmcs-nic.strip
	cmd> oct-pci-bootcmd "bootoct 0x20000000 coremask=fff"

	NOTE: To load the target application on multiple OCTEON devices, prefix
	    the oct-pci-* commands with OCTEON_PCI_DEVICE=<device number>.
	    For e.g., to load the application on OCTEON device 1, the
	    commands would be,

    cmd> OCTEON_PCI_DEVICE=1 oct-pci-load  0x20000000  ./cvmcs-nic.strip
    cmd> OCTEON_PCI_DEVICE=1 oct-pci-bootcmd "bootoct 0x20000000 coremask=f"


	3. Load the NIC module.
	cmd> insmod octnic.ko

	After  the nic module is loaded, the ethernet interfaces on all OCTEON
	devices  are  available as network devices on the host. The interfaces
	are  named  in  the order they are detected by the driver. For e.g. if
	there  are  2 OCTEON devices, one with 2 XAUI interfaces and the other
	with  4 SGMII interfaces, the OCTEON network devices on the host would
	be  named oct0 and oct1 (for the interfaces from the first OCTEON) and
	oct2 to oct5 (for the interfaces from the second OCTEON).


	4. Configure the interface.
	cmd> ifconfig oct0 192.168.16.101

	The OCTEON interfaces are now ready for use.

	You  can  also  monitor  packet  receipt on the OCTEON ports using the
	oct_stats utility. oct_stats is available from
	OCTEON-SDK/components/driver/host/utils. A symbolic link is available
	from OCTEON-SDK/components/driver/bin.

	To run oct_stats, do
	cmd> oct_stats 0






	7. EtherPCI
	===========

	EtherPCI  provides  a  mechanism  of  using the PCI ports of OCTEON as
	ethernet interfaces. It requires  OCTEON  target to run SDK Linux with
	the  octeon-ethernet driver modified to enable use of the PCI ports as
	ethernet  interfaces.  It  also requires the host BASE and NIC drivers
	to  be compiled with the  ETHERPCI flag enabled to register the OCTEON
	PCI ports as ethernet interfaces on the host.

	More detailed description and compilation instructions can be found at
	$OCTEON_ROOT/components/driver/host/driver/linux/octnic/EtherPCI/README.txt





	8. Enabling additional features in the driver
	=============================================

	Multi-core Rx processing
	------------------------

	The driver support Rx processing on multiple cores. This feature can be
	enabled	by recompiling the base & nic driver modules after uncommenting
	the following line in
	$OCTEON_ROOT/components/driver/host/driver/osi/octeon_droq.h
	
	#define USE_DROQ_THREADS

	You should also enable the use of multiple output queues in the OCTEON
	SE NIC application by turning on the USE_MULTIPLE_OQ option in
	applications/pci-core-app/nic/cvmcs-nic.h.

	This feature helps boost Rx packet processing performance by splitting
	the work across multiple CPU's. Enabling this feature only if you have
	multiple CPUs and expect heavy traffic to arrive on all ports of an
	OCTEON NIC card.

	Note that the BASE driver, NIC driver and  SE NIC application needs to
	be recompiled after this change.


	NAPI
	----
	
	NOTE: NAPI performance is not optimal yet. We are investigating this and
	      a patch would be released at a later date.

	NAPI is supported by the NIC driver. To enable Rx processing in NAPI
	mode, open components/driver/host/driver/linux/octnic/octeon_network.h
	and set the OCT_NIC_USE_NAPI flag to 1.

	You should also enable the use of multiple output queues in the OCTEON
	SE NIC application by turning on the USE_MULTIPLE_OQ option in
	applications/pci-core-app/nic/cvmcs-nic.h.

	The NIC driver and  SE NIC application needs to be recompiled after
	this change.



