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

/*! \file  cn68xx_device.h
    \brief Host Driver: Routines that perform CN68XX specific operations.
*/



#ifndef  __CN68XX_DEVICE_H__
#define  __CN68XX_DEVICE_H__


#include "cn68xx_regs.h"


/* Register address and configuration for a CN68XX device. */
typedef struct {

	uint8_t            *intr_sum_reg64;

	uint8_t            *intr_enb_reg64;

	uint64_t            intr_mask64;

	cn68xx_config_t    *conf;

} octeon_cn68xx_t;




void  cn68xx_setup_global_output_regs(octeon_device_t  *oct);

void  cn68xx_check_config_space_error_regs(octeon_device_t  *oct);

int   setup_cn68xx_octeon_device(octeon_device_t  *oct);

int   validate_cn68xx_config_info(cn68xx_config_t  *conf68xx);


uint32_t cn68xx_get_oq_ticks(octeon_device_t  *oct, uint32_t  time_intr_in_us);

uint32_t cn68xx_core_clock(octeon_device_t  *oct);

#endif

/* $Id$ */
