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
#include "cavium_sysdep.h"
#include "octeon_hw.h"
#include "octeon_device.h"

void
cn68xx_vendor_message_fix(octeon_device_t *oct)
{
	uint32_t regval = 0;

	/* Set M_VEND1_DRP and M_VEND0_DRP bits */
	OCTEON_READ_PCI_CONFIG(oct, CN68XX_CONFIG_PCIE_FLTMSK, &regval);
	regval |= 0x3;
	OCTEON_WRITE_PCI_CONFIG(oct, CN68XX_CONFIG_PCIE_FLTMSK, regval);

}

void
cn63xx_errata_sli14401_fix(octeon_device_t  *oct)
{
	/* We don't which port is being used at this point. We'll apply the fix
	   on both ports. */

	/* Enable Port0  */
	octeon_write_csr64(oct, CN63XX_SLI_CTL_PORT0, (1 << 17));

	/* Enable Port1  */
	octeon_write_csr64(oct, CN63XX_SLI_CTL_PORT1, (1 << 17));

	/* Write back bit fields to their reset value */
	octeon_write_csr64(oct, CN63XX_SLI_CTL_PORT0, 0xE400);
	octeon_write_csr64(oct, CN63XX_SLI_CTL_PORT1, 0xE400);
}


int
check_bp_on(octeon_device_t   *oct)
{
#if !defined(DISABLE_PCIE14425_ERRATAFIX)
	if( (oct->chip_id == OCTEON_CN63XX) || (oct->chip_id == OCTEON_CN66XX) )
		return octeon_read_csr64(oct, CN63XX_SLI_SCRATCH1);
	
	if(oct->chip_id == OCTEON_CN56XX)
		return octeon_read_csr64(oct, CN56XX_NPEI_SCRATCH);
#endif
	return 0;

}


#if defined(ENABLE_PCIE_2G4G_FIX)
#define    CVM_2G   ((unsigned long)(2UL * 1024 * 1024 * 1024))
#define    CVM_4G   ((unsigned long)(4UL * 1024 * 1024 * 1024))

struct buflist {
	cavium_list_t   list;
	void           *buf;
};

static void *
__get_droq_2g4g_buf(octeon_device_t  *oct, int size)
{
	cavium_list_t    head;
	unsigned long    physbuf;
	uint8_t         *buf, *foundbuf = NULL;
	int              count = 1000000;

	CAVIUM_INIT_LIST_HEAD(&head);

	while(count--) {
		buf = (uint8_t *)cavium_alloc_pages(__CAVIUM_MEM_GENERAL, cavium_get_order(size));
		physbuf = (unsigned long)octeon_pci_map_single(oct->pci_dev, buf, 16, CAVIUM_PCI_DMA_FROMDEVICE);
		if(buf == NULL)
			break;
		octeon_pci_unmap_single(oct->pci_dev, physbuf, 16, CAVIUM_PCI_DMA_FROMDEVICE);
		if(physbuf && ((physbuf < (CVM_2G - size)) || (physbuf > CVM_4G))) {
			cavium_print_msg("Found buffer block @ %p phys: %lx\n", buf, physbuf);
			foundbuf = buf;
			break;
		} else {
			struct buflist *node = (struct buflist *)buf;
			node->buf = buf;
			cavium_list_add_tail(&node->list, &head);
		}
	}

	{
		cavium_list_t  *node, *node1;
		cavium_list_for_each_safe(node, node1, &head) {
			cavium_list_del(node);
			buf = ((struct buflist *)node)->buf;
			cavium_free_pages(buf, cavium_get_order(size));
		}
	}

	return foundbuf;
}


uint8_t *
__get_droq_buffer_block(octeon_device_t  *oct, octeon_droq_t  *droq)
{
	return __get_droq_2g4g_buf(oct, droq->max_count * droq->buffer_size);
}


uint8_t *
__get_droq_info_block(octeon_device_t  *oct, octeon_droq_t  *droq)
{
	return __get_droq_2g4g_buf(oct, droq->max_count * OCT_DROQ_INFO_SIZE);
}



void
__free_droq_buffer_block(octeon_droq_t  *droq)
{
	cavium_free_pages((unsigned long)droq->buffer_block, cavium_get_order(droq->buffer_size * droq->max_count));
}



void
__free_droq_info_block(octeon_droq_t  *droq)
{
	cavium_free_pages((unsigned long)droq->info_list, cavium_get_order(OCT_DROQ_INFO_SIZE * droq->max_count));
}
#endif

/* $Id$ */
