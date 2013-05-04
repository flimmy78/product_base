                             EtherPCI
                  SDK Version:2.2.0 Driver Version: 2.2.0
                   -----------------------------------------




Section 1: Overview
===================

  EtherPCI is a mechanism that enables  the  use of PCI ports on an Octeon
  target device as ethernet interfaces.  It  requires Octeon  to run Linux
  with a modified ethernet driver and  also  requires the host PCI and NIC
  modules (available  from  OCTEON-PCI-BASE and  OCTEON-PCI-NIC  packages)
  to run in ETHERPCI mode.


  The  modified  host PCI driver and NIC module will provide four ethernet
  interfaces on host  system  (oct0  to  oct3).  Any packets sent on these
  interfaces  will  be  sent  across PCI to  the Octeon PCI target running
  linux.


  The  modified octeon-ethernet driver running on Octeon also creates four
  ethernet  interfaces  (pci0  to  pci3)  for communicating with the host.
  Any  packet  sent on oct0 from the host will arrive on pci0 interface on
  Octeon while packets sent on oct1 will arrive on pci1.


  A  patch provided in this package provides the required  changes for the
  Octeon  Linux  octeon-ethernet  driver that should  be run on the target 
  Octeon device. The Octeon  PCI driver and NIC module running on the host
  can be  modified to run in ETHERPCI mode by a simple recompilation after
  changing flags in their respective Makefiles. 


  



Section 2: The package
======================

  The patch,  documentation  and  install scripts are available at 
  $OCTEON_ROOT/components/driver/host/driver/linux/octnic/EtherPCI
  directory once the OCTEON-PCI-NIC package is installed.

  The directory structure of the contents are shown below:

  EtherPCI
     README.txt  - This README file.
     etherpci    - a script to install/uninstall the patch. 
     patch.cvm-eth.sdk-2.2.0.txt - patch for octeon-ethernet driver.


  OCTEON_ROOT is the installed location of the Cavium Networks Octeon SDK,
  typically found at /usr/local/Cavium_Networks/OCTEON-SDK.







Section 3: Required Software Packages
=====================================

  Before you proceed, ensure that you have the following 4 packages
  installed on your system:

  1. OCTEON-SDK      Version 2.2.0
  2. OCTEON-LINUX    Version 2.2.0
  3. OCTEON-PCI-BASE Version 2.2.0
  4. OCTEON-PCI-NIC  Version 2.2.0

  The steps described below assume that all 4 packages are installed.
  The  OCTEON-PCI-BASE  and  OCTEON-PCI-NIC have built-in support for
  EtherPCI.
  They need to be recompiled after enabling the ETHERPCI flag.








Section 4: Installing Patch
===========================


  Install patch using the etherpci script (recommended)
  -----------------------------------------------------

  Run the etherpci script provided in the EtherPCI directory.
  To install the patches, execute the following command
  cmd> ./etherpci install

  After installation of patches, proceed to "Step 2" to compile the
  sources.


                            -- OR --


  Install patch manually
  ----------------------

  a) Go to $OCTEON_ROOT/linux/kernel_2.6/linux/drivers/net/octeon

  b) Apply patch by executing the following command
     cmd> patch -p0 < $OCTEON_ROOT/components/driver/host/driver/linux/octnic/EtherPCI/patch.cvm-eth.sdk-2.2.0.txt









Section 5: Compiling sources
============================

  a) Compile the Octeon Linux kernel 
     cmd> cd $OCTEON_ROOT/linux
     cmd> make kernel
          --OR--
     cmd> make kernel-deb


  b) Transfer the linux image that includes the new octeon-ethernet driver
     to  the  target  octeon using the normal method used to load linux on
     your Octeon target board. More details on loading linux on Octeon can
     be found in the Octeon SDK documentation.


  c) Compile the PCI base driver
     cmd> cd $OCTEON_ROOT/components/driver/host/driver/linux
     cmd> Add the ETHERPCI flag to the COMPILEFOR line in the Makefile
          COMPILEFOR = ETHERPCI
     cmd> make


  d) Compile the PCI NIC module
     cmd> cd $OCTEON_ROOT/components/driver/host/driver/linux/octnic
     cmd> Add the ETHERPCI flag to the COMPILEFOR line in the Makefile
          COMPILEFOR = ETHERPCI
     cmd> make


  At the  end of this step, the base driver (called octeon_drv.ko) and the
  NIC  module  (called octnic.ko)  can  be accessed by symbolic links from 
  $OCTEON_ROOT/components/driver/bin








Section 6: Communication across PCI
===================================

  This section  assumes that an Octeon board is plugged into a PCI slot in
  the system. 

  a) Start the host PCI driver and NIC module on the host system.
     cmd> cd $OCTEON_ROOT/components/driver/bin
     cmd> insmod octeon_drv.ko
     cmd> insmod octnic.ko

  You should see four Octeon interfaces on your system, labelled "oct0" to
  "oct3".


  b) Load linux on the Octeon board in the normal way.
     When  the  octeon-ethernet  module comes up, it will register the PCI
     ports as regular network interfaces named "pci0" to "pci3". These are
     the four interfaces that can be used to communicate with the host.
 


                               IMPORTANT
                               ---------
    Make sure that the octeon-ethernet driver  is loaded on the target AND
    the nic driver is loaded on the host BEFORE you configure IP addresses
    on any of the interfaces.




  Configure the interfaces
  --------------------------

  a) On the host, configure the interfaces oct0 and oct1,
     cmd> ifconfig oct0 192.168.11.101
     cmd> ifconfig oct1 192.168.12.101

  b) On octeon, configure the interfaces pci0 and pci1,
     cmd> ifconfig pci0 192.168.11.102
     cmd> ifconfig pci1 192.168.12.102

  c) You are now ready to communicate across PCI using these interfaces.








Section 7: Uninstalling Patch
=============================


  Uninstall patch using the etherpci script (recommended)
  -------------------------------------------------------

  Run the etherpci script provided in the EtherPCI directory  to uninstall
  the patches,

  cmd> ./etherpci uninstall



            -- OR --


  Uninstall patch manually
  ------------------------

  a) Go to $OCTEON_ROOT/linux/kernel_2.6/linux/drivers/net/octeon

  b) Remove patch as follows
     cmd> patch -p0 -R < $OCTEON_ROOT/components/driver/host/driver/linux/octnic/EtherPCI/patch.cvm-eth.sdk-2.2.0.txt



