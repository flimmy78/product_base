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


/*! \file  cn3xxx_device.h
    \brief Host Driver: Routines that perform CN3XXX or CN58XX specific 
                        operations. 
*/


#ifndef  __CN3XXX_DEVICE_H__
#define  __CN3XXX_DEVICE_H__


#include "cn3xxx_regs.h"



/* Register address and configuration for a CN38XX/CN58XX device. */
typedef struct {

	/** PCI interrupt summary register for CN38xx/CN58xx */
	void              *intr_sum_reg;

	/** PCI interrupt enable register for CN38xx/CN58xx */
	void              *intr_enb_reg;

	/** The PCI interrupt mask used by interrupt handler for CN38xx/CN58xx */
	cavium_atomic_t    intr_mask;

	/** The PCI bus frequency for this device. For CN58XX, this is
	    auto-detected. For CN3XXX, this field takes its value from
	    OCTEON_PCI_BUS_HZ. */
	uint32_t           pci_freq;

	cn3xxx_config_t   *conf;

} octeon_cn3xxx_t;




int
setup_cn38xx_octeon_device(octeon_device_t  *oct);

int
setup_cn58xx_octeon_device(octeon_device_t  *oct);


int
validate_cn3xxx_config_info(cn3xxx_config_t  *cn3xxx);


#endif


/* $Id: cn3xxx_device.h 61122 2011-06-07 00:12:54Z panicker $ */
