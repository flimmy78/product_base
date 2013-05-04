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

/*!  \file octcntq_proc.h
     \brief Host CNTQ Driver: Routines that handle /proc interface for Octeon
                              CNTQ driver.
*/



#ifndef  __OCTCNTQ_PROC_H__
#define  __OCTCNTQ_PROC_H__


int
octcntq_init_proc(int octeon_id);


void
octcntq_delete_proc(int octeon_id);



int
octcntq_proc_read_statsb(int octeon_id, oct_stats_t  *stats);


int
octcntq_proc_read_stats(int octeon_id, char   *page);


#endif


/* $Id: octcntq_proc.h 42868 2009-05-19 23:57:52Z panicker $ */

