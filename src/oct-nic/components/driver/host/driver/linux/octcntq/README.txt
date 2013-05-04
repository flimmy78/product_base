Linux Driver for OCTEON (CNTQ Module - Version 2.2.0)
=======================================================

	Table of contents:

	1. What's in this release?
	2. System Requirements
	3. Installing the RPM
	4. Compiling the PCI CNTQ Driver Module
	5. Compiling the core driver for CNTQ module
	6. Loading the CNTQ module
	7. Testing the CNTQ module
	


	1. What's in this release?
	--------------------------
	
	This release provides the sources for the CNTQ module of  OCTEON  PCI
	driver loadable on a Linux Host System. Also included are sources for
	a sample  OCTEON core application that can be loaded on  OCTEON cores
	to test the PCI driver.

	This release adds support for CN66XX Pass1.

	Please refer to the release notes for list of changes in this release.

	The CNTQ module uses  the  interfaces provided by the OCTEON PCI BASE
	driver.	Refer to the  README.txt file at OCTEON-SDK/components/driver
	to know more about the BASE driver usage and compilation instructions.

	Documentation for the sources is provided in HTML format at
	OCTEON-SDK/components/driver/docs/html/index.html.
	
	
	
	2. System Requirements
	-----------------------

	The  CNTQ  module for OCTEON PCI driver is used when OCTEON is in PCI
	target mode. The  driver  can  be  loaded on any system running Linux
	kernel version 2.6.18 to 2.6.32. The  CNTQ  driver  uses the services
	provided by OCTEON base driver for communication with OCTEON.

	The BASE driver has to be loaded before the CNTQ module can be loaded.

	The driver has been tested on SL 5.5 (2.6.18), SL 6.0 (2.6.32) & OCTEON
	SDK 2.1 LINUX.
	
	
	
	
	
	
	
	3. Installing the RPM
	---------------------
	
	The OCTEON PCI CNTQ RPM requires the following packages from Cavium
	Networks to be installed on the system:
	1) OCTEON SDK version 2.2.0
	2) OCTEON-COMPONENTS-COMMON version 2.2.0
	3) OCTEON-PCI-BASE version 2.2.0
  

	Install the RPM
	cmd> rpm -i OCTEON-PCI-CNTQ-2.2.0-84.i386.rpm

	The path to source and documentation is printed at end of installation.

	After installation, the source files for the driver are available at 
	1. OCTEON-SDK/components/driver/host/driver/linux/octcntq (host driver)
	2. OCTEON-SDK/components/driver/core (core driver).
	The core application included with the package would get installed at
	OCTEON-SDK/applications/pci-core-app/cntq.
	
	


	4. Compiling the PCI CNTQ Driver Module
	---------------------------------------

	The CNTQ module can be compiled by doing "make" in 
	OCTEON-SDK/components/driver/host/driver/linux/octcntq directory.

	The CNTQ module is called octcntq.ko. A symbolic link to the object
	file will be added from OCTEON-SDK/components/driver/bin.

	TIP: To know more about CNTQ in the OCTEON driver refer to the 
	     documentation at OCTEON-SDK/components/driver/docs/cntq.


	NOTE: You  will  see warnings for undefined symbols at the end of
	      compilation. These  symbols  are defined in the base driver
	      available  from  the OCTEON-PCI-BASE package. These symbols
	      are resolved when you load the module after the base driver
	      is loaded.



	5. Compiling the core driver for CNTQ module
	--------------------------------------------
           
	The  core  pci driver, like  the simple executive files, are compiled
	into libraries  that  get  linked  to the core application at compile
	time. You do not have to do a separate make for the core driver.
                                                                                             
	This package includes a sample simple executive application installed
	at    OCTEON-SDK/applications/pci-core-app/cntq. Compiling  the  core
	application is done by running 'make' in the above directory.

	The core application Makefile includes the  CNTQ core driver makefile
	fragment  (core-drv-cntq.mk). The  make  process  compiles the driver
	sources and creates	a library which gets linked with the application.
	
	cvmcs-cntq.bin is the original application binary. cvmcs-cntq.strip is 
	thei stripped version of the application.

	In order to use the PCIe port #1, change the OCTEON_PCIE_PORT to 1 in
	OCTEON-SDK/components/driver/cvm-driver-defs.h file.
    
		NOTE: Before you can compiling the core driver sources, you should
		      source the "env-setup" script located at
		      /usr/local/Cavium_Networks/OCTEON-SDK to setup the
		      environment variables for building OCTEON	applications.




	6. Loading the CNTQ module
	--------------------------

	The following steps should be followed to load the CNTQ module:

	1. Load the base driver module.
	cmd> insmod octeon_drv.ko

	2. Load  the core application.
	NOTE: The  oct-pci-boot command below should  be run only if the card
	      is configured for pci-boot mode.
	cmd> oct-pci-boot
	cmd> oct-pci-load  0 ./cvmcs-cntq.strip
	cmd> oct-pci-bootcmd "bootoct 0 coremask=fff"

	NOTE: To load the target application on multiple OCTEON devices, prefix
	    the oct-pci-* commands with OCTEON_PCI_DEVICE=<device number>.
	    For e.g., to load the application on OCTEON device 1, the
	    commands would be,

    cmd> OCTEON_PCI_DEVICE=1 oct-pci-load  0 ./cvmcs-cntq.strip
    cmd> OCTEON_PCI_DEVICE=1 oct-pci-bootcmd "bootoct 0 coremask=f"

	3. Load the CNTQ module.
	cmd> insmod octcntq.ko




	7. Testing the CNTQ module
	--------------------------

	The  RPM  package provides an additional test program for testing the
	DDOQ functionality. The  sources  for this test program are available
	from OCTEON-SDK/components/driver/host/test/kernel/ddoq_test.
	The program is a loadable kernel module and can be compiled by doing
	"make" in the above directory. The test program can be configured by
	modifying  parameters in octeon_ddoq_test.c. The program creates two
	kernel threads -  one to create DDOQ's and the  other to delete them.
	The threads that creates DDOQ's starts first. The  program deletes a
	DDOQ after waiting for  DT_CREATE_WAIT_TIME  ticks from the time the
	DDOQ  was  created. The  maximum  number  of  DDOQ's  that can exist
	(created but not deleted yet) is determined by the MAX_DDOQS value.

	To test DDOQ functionality, follow the steps in section (6) above and
	then load the ddoq_test module as shown below:

	Load the test kernel module.
	cmd> insmod ddoq_test.ko

	The test program will now be creating and deleting DDOQ's at a rate 
	determined by the parameters discussed above. The test program will
	also display the count of  DDOQ's  created and deleted at intervals
	determined by DISPLAY_INTERVAL in octeon_ddoq_test.h.

	You can monitor the test statistics by running the following command
	cmd> ./oct_stats 0 -icdr

	By default,  the test program is configures to send test packets on
	OCTEON device 0. If  you need to test on a different device, change
	the  OCTEON_ID value in octeon_ddoq_test.h.


	You can send test packets to a DDOQ by enabling the CVMCS_TEST_DDOQ
	flag in the  applications/pci-core-app/cntq/cvmcs-cntq-test.h file.
	When the flag is enabled, the core application can send test packets
	to a DDOQ with id's in the range 36 to (36 + MAX_TEST_DDOQS). The
	ddoq_test kernel module must have the DDOQ_PKT_INPUT_TEST flag
	enabled to prevent the test from deleting the DDOQ's on which the
	core application will send packets.





