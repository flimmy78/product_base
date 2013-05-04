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


/*! \file cvm-cntq-resources.h
    \brief Core Driver: FPA Pool for DDOQ descriptor ring.
*/                                                                                        

#ifndef __CVM_CNTQ_RESOURCES_CONFIG__
#define __CVM_CNTQ_RESOURCES_CONFIG__
                                                                                                                                                             
/* Content below this point is only used by the cvmx-config tool, and is
** not used by any C files as CAVIUM_COMPONENT_REQUIREMENT is never
defined.
*/
#ifdef CAVIUM_COMPONENT_REQUIREMENT
	/* global resource requirement */
	cvmxconfig
	{
		fpa CVM_FPA_DDOQ_POOL
		size        = 64
		protected   = 1
		description = "Buffer pool for ddoq";
	}
#endif

#endif

/* $Id: cvm-cntq-resources.h 42868 2009-05-19 23:57:52Z panicker $ */

