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
#include "cvmx.h"
#include "cvmx-helper.h"
#include "cvmx-pip.h"


static inline void
__print_regs_in_range(unsigned long long  start,
                      unsigned long long  end,
                      int                 offset,
                      char               *str)
{
	uint64_t  reg = start;

	while(reg <= end) {
		printf("%s[0x%016lx]:  0x%016lx\n", str, reg,
			 cvmx_read_csr(CVMX_ADD_IO_SEG(reg)));
		reg += offset;
	}
}







void
dump_cn63xx_pem_regs(int pcieport)
{
	unsigned long long  base = 0x00011800C0000000ULL;

	printf("\n ---- Dumping CN63xx PEM registers for PCIe port %d\n", pcieport);

	if(pcieport > 1) {
		printf("Invalid pcie port %d passed to %s\n", pcieport, __FUNCTION__);
		return;
	}

	base += (pcieport * 1000000ULL);

	__print_regs_in_range(base, base + 0x18, 0x8, "PEM");

	__print_regs_in_range(base + 0x20, base + 0x20, 0x8, "PEM");

	__print_regs_in_range(base + 0x38, base + 0x130, 0x8, "PEM");

	__print_regs_in_range(base + 0x408, base + 0x420, 0x8, "PEM");
}



void
cvm_dump_cn63xx_regs(void)
{
	dump_cn63xx_pem_regs(0);
}





