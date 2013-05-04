/*
 *
 * OCTEON SDK
 *
 * Copyright (c) 2011 Cavium Networks. All rights reserved.
 *
 * This file, which is part of the OCTEON SDK which also includes the
 * OCTEON SDK Package from Cavium Networks, contains proprietary and
 * confidential information of Cavium Networks and in some cases its
 * suppliers. 
 *
 * Any licensed reproduction, distribution, modification, or other use of
 * this file or the confidential information or patented inventions
 * embodied in this file is subject to your license agreement with Cavium
 * Networks. Unless you and Cavium Networks have agreed otherwise in
 * writing, the applicable license terms "OCTEON SDK License Type 5" can be found 
 * under the directory: $OCTEON_ROOT/components/driver/licenses/
 *
 * All other use and disclosure is prohibited.
 *
 * Contact Cavium Networks at info@caviumnetworks.com for more information.
 *
 */

/*! \file  cvm-pci-loadstore.h
    \brief Core Driver: Simple executive API for generating memory-space
	                    read/writes operations on the PCI Bus.
*/



#ifndef  __CN56XX_LOADSTORE_H__
#define  __CN56XX_LOADSTORE_H__

#include "cvmx.h"
#include "cvm-driver-defs.h"


#define ENDIAN_SWAP_8_BYTE(_i) \
  ((((((uint64_t)(_i)) >>  0) & (uint64_t)0xff) << 56) | \
   (((((uint64_t)(_i)) >>  8) & (uint64_t)0xff) << 48) | \
   (((((uint64_t)(_i)) >> 16) & (uint64_t)0xff) << 40) | \
   (((((uint64_t)(_i)) >> 24) & (uint64_t)0xff) << 32) | \
   (((((uint64_t)(_i)) >> 32) & (uint64_t)0xff) << 24) | \
   (((((uint64_t)(_i)) >> 40) & (uint64_t)0xff) << 16) | \
   (((((uint64_t)(_i)) >> 48) & (uint64_t)0xff) <<  8) | \
   (((((uint64_t)(_i)) >> 56) & (uint64_t)0xff) <<  0))


#define ENDIAN_SWAP_4_BYTE(_i) \
    ((((uint32_t)(_i)) & 0xff000000) >> 24) | \
   ((((uint32_t)(_i)) & 0x00ff0000) >>  8) | \
   ((((uint32_t)(_i)) & 0x0000ff00) <<  8) | \
   ((((uint32_t)(_i)) & 0x000000ff) << 24)




void       cvm_setup_pci_load_store(void);

void       cvm_pci_mem_writeb (unsigned long addr, uint8_t val8);
uint8_t    cvm_pci_mem_readb (unsigned long addr);

uint32_t   cvm_pci_mem_readl (unsigned long addr);
void       cvm_pci_mem_writel (unsigned long addr, uint32_t  val32);

uint64_t   cvm_pci_mem_readll (unsigned long addr);
void       cvm_pci_mem_writell (unsigned long addr, uint64_t  val64);

void
cvm_pci_read_mem(unsigned long addr, uint8_t  *localbuf, uint32_t  size);


#endif

/* $Id: cvm-pci-loadstore.h 43039 2009-06-03 00:51:33Z panicker $ */
