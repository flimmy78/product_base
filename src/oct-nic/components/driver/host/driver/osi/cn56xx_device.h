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

/*! \file  cn56xx_device.h
    \brief Host Driver: Routines that perform CN56XX specific operations.
*/



#ifndef  __CN56XX_DEVICE_H__
#define  __CN56XX_DEVICE_H__


#include "cn56xx_regs.h"



/* Register address and configuration for a CN56XX device. */
typedef struct {

	/** PCI interrupt summary register for CN56xx */
	uint8_t           *intr_sum_reg64;

	/** Additional PCI interrupt summary register for CN56xx */
	uint8_t           *intr_a_sum_reg64;

	/** PCI interrupt enable register for CN56xx */
	uint8_t           *intr_enb_reg64;

	/** Additional PCI interrupt enable register for CN56xx */
	uint8_t           *intr_a_enb_reg64;

	/** The PCI interrupt mask used by interrupt handler for CN56xx */
	uint64_t           intr_mask64;

	cn56xx_config_t   *conf;

} octeon_cn56xx_t;


int   setup_cn56xx_octeon_device(octeon_device_t  *oct);

int   validate_cn56xx_config_info(cn56xx_config_t  *cn56xx);


void  cn56xx_setup_global_output_regs(octeon_device_t  *oct);

void  cn56xx_check_config_space_error_regs(octeon_device_t  *oct);




int       cn56xx_send_peer_to_peer_map(int oct_id);

uint32_t  cn56xx_pass2_core_clock(octeon_device_t  *oct);

#endif

/* $Id: cn56xx_device.h 61122 2011-06-07 00:12:54Z panicker $ */
