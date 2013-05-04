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

#include "cvm-pci-loadstore.h"

CVMX_SHARED volatile uint64_t pci_mem_subdid_base;


static void
cn3xxx_setup_pci_load_store()
{
	cvmx_npi_mem_access_subidx_t mem_access;

	pci_mem_subdid_base = CVMX_ADDR_DID(CVMX_FULL_DID(CVMX_OCT_DID_PCI, 3));

	mem_access.u64 = 0;
	mem_access.s.esr = 1;   /**< Endian-Swap on read. */
	mem_access.s.esw = 1;   /**< Endian-Swap on write. */
	cvmx_write_csr(CVMX_NPI_MEM_ACCESS_SUBID3, mem_access.u64);
}



static void
cn56xx_setup_pci_load_store()
{
	int i;
	cvmx_npei_mem_access_subidx_t mem_access;

	pci_mem_subdid_base = CVMX_ADDR_DID(CVMX_FULL_DID(CVMX_OCT_DID_PCI, 3));

	mem_access.u64 = 0;
	mem_access.s.esr = 1;   /**< Endian-Swap on read. */
	mem_access.s.esw = 1;   /**< Endian-Swap on write. */

	for(i = 0; i < 4; i++) {
		mem_access.s.ba = i;
		cvmx_write_csr(CVMX_PEXP_NPEI_MEM_ACCESS_SUBIDX(12 + i), mem_access.u64);
	}
}





static void
cn63xx_setup_pci_load_store()
{
	int                            i, pcie_port = 0;
	cvmx_sli_mem_access_subidx_t   mem_access;

	pci_mem_subdid_base = 0x80011b0000000000ULL;

	mem_access.u64    = 0;
	mem_access.s.port = pcie_port; /* Port the request is sent to. */
	mem_access.s.esr  = 1;     /* Endian-swap for Reads. */
	mem_access.s.esw  = 1;     /* Endian-swap for Writes. */
    
    /* Setup mem access 12-15 for port 0, 16-19 for port 1, supplying 36 bits of address space */
    for (i=12 + pcie_port*4; i<16 + pcie_port*4; i++)
    {
        cvmx_write_csr(CVMX_PEXP_SLI_MEM_ACCESS_SUBIDX(i), mem_access.u64);

	if( OCTEON_IS_MODEL(OCTEON_CN63XX) ) {
        	mem_access.cn63xx.ba += 1; /* Set each SUBID to extend the addressable range */
	}

	if( OCTEON_IS_MODEL(OCTEON_CN66XX) ) {
        	mem_access.cn66xx.ba += 1; /* Set each SUBID to extend the addressable range */
	}
    }
}





static void
cn68xx_setup_pci_load_store()
{
	int                            i, pcie_port = OCTEON_PCIE_PORT;
	cvmx_sli_mem_access_subidx_t   mem_access;

	pci_mem_subdid_base = 0x80011b0000000000ULL;

	mem_access.u64    = 0;
	mem_access.s.port = pcie_port; /* Port the request is sent to. */
	mem_access.s.esr  = 1;     /* Endian-swap for Reads. */
	mem_access.s.esw  = 1;     /* Endian-swap for Writes. */
 
   
	/* Setup mem access 12-15 for port 0, 16-19 for port 1, supplying 36 bits of address space */
	//for (i=12 + pcie_port*4; i<16 + pcie_port*4; i++)
	for (i=12; i<16; i++)
	{
		cvmx_write_csr(CVMX_PEXP_SLI_MEM_ACCESS_SUBIDX(i), mem_access.u64);
		mem_access.cn68xx.ba += 1; /* Set each SUBID to extend the addressable range */
	}
}








void
cvm_setup_pci_load_store()
{
	printf("%s: Setting up registers for PCI load/store ops\n", __FUNCTION__);

	if(OCTEON_IS_MODEL(OCTEON_CN68XX))
		cn68xx_setup_pci_load_store();

	if( OCTEON_IS_MODEL(OCTEON_CN63XX) || OCTEON_IS_MODEL(OCTEON_CN66XX) )
		cn63xx_setup_pci_load_store();

	if(OCTEON_IS_MODEL(OCTEON_CN56XX))
		cn56xx_setup_pci_load_store();

	if(OCTEON_IS_MODEL(OCTEON_CN58XX) || OCTEON_IS_MODEL(OCTEON_CN38XX))
		cn3xxx_setup_pci_load_store();

	printf("[ DRV ] subdid base is 0x%lx\n", pci_mem_subdid_base);
}




uint8_t cvm_pci_mem_readb (unsigned long addr)
{
	CVMX_SYNCW;
	DBG_PRINT(DBG_FLOW, "%s: Read 8B from 0x%lx\n", __FUNCTION__, addr);
	return cvmx_read64_uint8(pci_mem_subdid_base + addr);
}



void cvm_pci_mem_writeb (unsigned long addr, uint8_t val8)
{
	CVMX_SYNCW;
	cvmx_write64_uint8(pci_mem_subdid_base + addr, val8);
	CVMX_SYNCW;
}




uint32_t cvm_pci_mem_readl (unsigned long addr)
{
	uint32_t   val32;

	CVMX_SYNCW;
	DBG_PRINT(DBG_FLOW,"\n %s: Going for read from 0x%016lx\n",
	          __FUNCTION__, (pci_mem_subdid_base + addr));
	val32 = cvmx_read64_uint32(pci_mem_subdid_base + addr);
	DBG_PRINT(DBG_FLOW,"%s: read from 0x%016lx: 0x%08x\n",
	       __FUNCTION__, (pci_mem_subdid_base + addr), val32);
	return val32;
}





void cvm_pci_mem_writel (unsigned long addr, uint32_t val32)
{
	CVMX_SYNCW;
	DBG_PRINT(DBG_FLOW,"%s: write : 0x%08x to addr 0x%016lx\n",
	       __FUNCTION__, val32, (pci_mem_subdid_base + addr) );
	cvmx_write64_uint32(pci_mem_subdid_base + addr, val32);
	CVMX_SYNCW;
}





uint64_t cvm_pci_mem_readll (unsigned long addr)
{
	uint64_t   val64;

	CVMX_SYNCW;
	val64 = cvmx_read64_uint64(pci_mem_subdid_base + addr);
	DBG_PRINT(DBG_FLOW,"%s: read from 0x%016lx: 0x%016lx\n",
	       __FUNCTION__, (pci_mem_subdid_base + addr), val64);
	return val64;
}



void cvm_pci_mem_writell (unsigned long addr, uint64_t val64)
{
	CVMX_SYNCW;

	DBG_PRINT(DBG_FLOW,"%s: write : 0x%016lx to addr 0x%016lx\n",
	       __FUNCTION__, val64, (pci_mem_subdid_base + addr) );
	cvmx_write64_uint64(pci_mem_subdid_base + addr, val64);
	CVMX_SYNCW;
}




void
cvm_pci_read_mem(unsigned long addr, uint8_t  *localbuf, uint32_t  size)
{
	uint8_t   *buf8 = localbuf;

	while(size) {
		*buf8 = cvm_pci_mem_readb(addr);
		printf("Read %x from %lx into %p\n", *buf8, addr, buf8);
		buf8++;
		addr++; size--;
	}
	CVMX_SYNCW;
}



/* $Id: cvm-pci-loadstore.c 66890 2011-11-09 03:09:07Z mchalla $ */
