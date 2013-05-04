Linux PCI Driver for OCTEON processors (Base Module - Version 2.2.0)
=======================================================================

	Table of contents:

	1.  What's in this release?
	2.  System Requirements
	3.  Installing the RPM
	4.  Compiling the host driver
	5.  Compiling the core driver
	6.  OCTEON Host Driver Utilities
	7.  Loading PCI driver and core application
	8.  Configuring the Host Driver
	9.  Test Programs
	10. Additional documentation





	1. What's in this release?
	==========================
	
	This package provides the sources for the base module  of  OCTEON  PCI
	driver loadable on a Linux Host System. Also included  are sources for
	a sample core application that can be loaded on  OCTEON  cores to test
	the PCI driver.
	
	This release adds support for CN66XX.

	Documentation for the sources is provided in HTML format at
	OCTEON-SDK/components/driver/docs/html/index.html  once the source RPM
	is installed. Also refer to additional documents listed at the end of
	this file.

	Please refer to the release notes for a list of changes in this
	release.




	2. System Requirements
	======================

	The  OCTEON  PCI  Host driver  is  meant for use when  OCTEON is in PCI
	target mode. The driver can be loaded on a  system running 32 or 64-bit 
	Linux kernel version 2.6.18 upto 2.6.32.

	The OCTEON PCI driver typically uses around 14 Mbytes of  free memory,
	a portion of which requires contiguous physical memory.
	
	The driver has been tested on SL 5.5 (2.6.18), SL 6.0 (2.6.32) & OCTEON
	SDK 2.2 LINUX.





	3. Installing the RPM
	=====================
	
	The  OCTEON  PCI driver RPM  requires Cavium Networks  OCTEON SDK with 
	version 2.2.0 installed on the Linux host. It also requires version
	2.2.0 of OCTEON-COMPONENTS-COMMON package.


	Install the RPM
	cmd> rpm -i OCTEON-PCI-BASE-2.2.0-82.i386.rpm

	The path to source and documentation is printed at end of installation.
	After installation, the source file are available starting at 
	OCTEON-SDK/components/driver. The core  application  included with the
	package would get installed at OCTEON-SDK/applications/pci-core-app.
	

	NOTE: The driver sources are best read with a tabstop of 4.






	4. Compiling the Host Driver
	============================

	The driver.mk file in OCTEON-SDK/components/driver directory  controls
	important driver and target application compilation options.

	a) HOST_IS_OCTEON
	   - Enable this flag to compile the driver for OCTEON host running linux.

	b) CVMCS_DMA_IC
	   - Enable this option to use DMA coalesing interrupts for responses from
	     the sample Octeon application.

	c) DISABLE_PCIE14425_ERRATAFIX
	   - The fix for PCI lockup errata seen on CN56xx is turned off by default.
         Comment this option to turn it on.

	NOTE: The host driver and target application must be recompiled
	       after changing any settings in the driver.mk file.



	A top-level Makefile is provided at OCTEON-SDK/components/driver which
	can  be  used  to  build the host driver and other components. In this
	directory,
	*  Doing "make" will compile the host driver, api library, and host
	   utilities.
	   cmd> make 


			-- OR --


	You can proceed step-by-step.
	*  Build the host driver.
	   cmd> make host_drv

	*  Build the host utility binaries.
	   cmd> make utils



	Additionally you can do the following,
	*  Create the symbolic links required for host driver compilation.
	   cmd> make links
	   The required links were created when the rpm is installed. The make
	   rule above can be used if the links were deleted at any time.

	*  Clean binary and object files for host driver and core applications.
	   cmd> make clean

	TIP: The host driver Makefile is actually located at
	     OCTEON-SDK/components/driver/host/driver/linux. 

	At the end of make, all binaries (and the driver module) files have a
	symbolic link to them from OCTEON-SDK/components/driver/bin.
	To load the driver, go to OCTEON-SDK/components/driver/bin and run,
	cmd> "insmod octeon_drv.ko"


	To compile for OCTEON in host mode enable the "HOST_IS_OCTEON" flag in
	driver.mk  in  components/driver  and  recompile  the sources as above.
	Note  that  you  will  need  OCTEON-LINUX package installed to compile
	the driver for OCTEON in host mode.


	OCTEON device file
	------------------
	To  run  user  applications that interact with OCTEON PCI host driver,
	you will need to create a character device file with major number 127.
	This can be done with the following command: 
	cmd>   mknod /dev/octeon_device c 127 0

	TIP: To  know  more about request/response  mechanism using the OCTEON
	     driver refer to the documentation at 
	     OCTEON-SDK/components/driver/docs/html.





	5. Compiling the core driver
	============================

	The core pci driver, like the simple executive files, are compiled into
	libraries which get linked to the core application at compile time. You
	do not have to compile the core driver separately.

	This package includes a sample simple executive application installed at
	OCTEON-SDK/applications/pci-core-app/base.
	You can do "make" in this directory to compile the core application. The
	core application Makefile includes the core driver makefile fragment
	(core-drv.mk). The make process compiles the driver sources and creates
	a library which gets linked with the application.
	
	cvmcs is the original application binary. cvmcs.strip is the binary
	without symbol information.

	NOTE: Before you can start compiling the sources, you should source the
	      "env-setup" script -
	       located at /usr/local/Cavium_Networks/OCTEON-SDK.
	      This will setup the environment variables for a proper make.

	      For, e.g., to prepare for a OCTEON CN56XX board, do
	      cmd> source env_setup OCTEON_CN56XX









	6. OCTEON Host Driver Utilities
	===============================

	To make use of the utilities, you will  need  to build the api library
	first. The driver api  (liboctapi)  can  be  built  by doing a make at
	OCTEON-SDK/components/driver/host/api or
	"make host_api" at OCTEON-SDK/components/driver.

	You can build the utilities by
	* Doing "make utils" at OCTEON-SDK/components/driver

			-- OR --

	* Doing "make" at OCTEON-SDK/components/driver/host/utils.
	
	The utilities provided are 
	1. oct_stats    - to print driver queue statistics.
	2. oct_dbg      - to reset OCTEON & to read/write OCTEON registers and
	                  core memory.

	All utilities have symbolic links from OCTEON-SDK/components/driver/bin.
	
	The "oct_stats" utility requires the curses library to be installed on
	your system. If you don't have curses library or don't want to use it,
	comment the ENABLE_CURSES line in the Makefile and recompile oct_stats.

	NOTE: Remember  to  create the OCTEON device file before you use these
	      utilities (mknod /dev/octeon_device c 127 0)





	7. Loading PCI driver and core application
	==========================================

	This section shows how to load the PCI driver and core application. It
	assumes  that  the  driver, utilities  and  core application have been
	compiled  by  following the directions in the previous section and all
	have symbolic links from the driver/bin directory.

	In  the  driver/bin directory, let's assume we want to load the sample
	PCI  core  application - cvmcs.strip  - on  OCTEON device number 0 and
	run it on 4 cores.


	Load the driver and application using the steps below:

	Step 1: Load the driver

		cmd> insmod octeon_drv.ko

	Step 2A: For non PCI-boot boards, go to Step 2B. For  PCI-Boot boards,
		an  additional  step  is required. You would  need  to compile the
		bootloader  for your board. Refer  to  the  SDK  documentation  or
		contact  Cavium Support  for  instructions  to  prepare bootloader
		for your board. To  perform  PCI  boot, we  use  the  oct-pci-boot
		utility from SDK in the OCTEON-SDK/host/pci directory.

		cmd> oct-pci-boot <boot image file>

		This will load the bootloader on OCTEON.

	Step 2B: Load the core application on OCTEON and run on 4 cores.
	         (coremask will be 0xf).

		cmd> oct-pci-load  0 ./cvmcs.strip
		cmd> oct-pci-bootcmd "bootoct 0 coremask=f"


	NOTE: To load the target application on multiple OCTEON devices, prefix
	    the oct-pci-* commands with OCTEON_PCI_DEVICE=<device number>.
	    For e.g., to load the application on OCTEON device 1, the
	    commands would be,

	    cmd> OCTEON_PCI_DEVICE=1 oct-pci-load  0 ./cvmcs.strip
	    cmd> OCTEON_PCI_DEVICE=1 oct-pci-bootcmd "bootoct 0 coremask=f"








	8. Configuring the Host Driver
	==============================
	

	Enabling Debug Information
	--------------------------

		The driver can provide additional debug information that can be useful
		in  debugging  the  flow  of packets through it. To enable these debug
		messages, change the COMPILEFOR line in the Makefile at
		OCTEON-SDK/components/driver/host/driver/linux as shown below:

		COMPILEFOR = DEBUG

		and recompile the driver sources.

		The  level  of  debugging can also be changed to a value between 0 and 4
		at compile time by modifying the CAVIUM_DEBUG flag in the above Makefile
		or at run time when debug is enabled by writing to
		/proc/Octeon0/debug_level.



	Maximum OCTEON device count
	---------------------------

		The driver supports up to 4 devices by default.
		This value can be changed by modifying the MAX_OCTEON_DEVICES value in 
		OCTEON-SDK/components/driver/host/include/octeon_config.h.


	Changing per-device configuration
	---------------------------------

		The driver uses default settings for each type of OCTEON device it finds
		in the system. These settings can be found in 
		OCTEON-SDK/components/driver/host/include/oct_config_data.h.


	Multi-core Output queue processing
	----------------------------------
	
		The driver can be configured to process the packets on each output queue
		on  a  separate  CPU. This is turned on by enabling the USE_DROQ_THREADS
		flag in OCTEON-SDK/components/driver/host/driver/osi/octeon_droq.h.







	9. Test Programs
	=================

	The PCI driver base package includes test programs to test the driver.
	They are:
		a) oct_req   - a user space program to test request/response.
		b) req_resp  - a kernel space program to test request/response.
		c) droq_test - a kernel space program to test output queues.

	More  documentation  on  usage, compilation  and running the tests is
	available at OCTEON-SDK/components/driver/docs/html/index.html. Click
	on the section titled "OCTEON PCI test programs".






	10. Additional Documentation
	============================
	1. OCTEON-SDK/components/driver/docs/html/index.html
		The start page to access documentation for the driver sources 
		including the data structures & API's provided by the driver.

	2. OCTEON-SDK/components/driver/OCT-PCI-BASE-GUIDE-2.2.pdf
		A  guide to the architecture of the driver and in-depth description
		of the data flow management. This document covers both the user and
		kernel space API.


	

