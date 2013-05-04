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


/*! \file  oct_config_data.h 
    \brief Host Driver: Default Configuration data for Octeon devices.
*/


/* This file contains default configuration data for Octeon devices. There is
   one definition each for
   - CN38XX/58XX
   - CN56XX
   - CN63XX 
   - CN66XX and
   - CN68XX devices.
   The configuration data follows the structures definitions in
   include/octeon_config.h.
*/

#ifndef  __OCT_CONFIG_DATA_H__
#define  __OCT_CONFIG_DATA_H__


cn3xxx_config_t  default_cn3xxx_conf =
{
	/* num_iqs; num_oqs; pending_list_size; */
	{
	4,
	4,
	4096,
	},

	/* Input Queue configuration */
	/* num_descs; instr_type; db_min; db_timeout; */
	{
	{1024, 32, 1, 1} ,
	{1024, 32, 1, 1}  ,
	{1024, 32, 1, 1}  ,
	{1024, 32, 1, 1}
	},

	/* Output Queue configuration */
	/* num_descs; buf_size; info_ptr; pkts_per_intr; intr_pkt; intr_time;
	   refill_threshold*/
	{
	{1024, 2048, 1, 64, 64, 100, 64} ,
	{1024, 2048, 1, 64, 64, 100, 64} ,
	{1024, 2048, 1, 64, 64, 100, 64} ,
	{1024, 2048, 1, 64, 64, 100, 64} ,
	}

};




cn56xx_config_t  default_cn56xx_conf =
{
	/* num_iqs; num_oqs; pending_list_size; */
	{
	4,
	4,
	4096,
	},

	/* Input Queue configuration */
	/* num_descs; instr_type; db_min; db_timeout; */
	{
	{1024, 32, 1, 1} ,
	{1024, 32, 1, 1}  ,
	{1024, 32, 1, 1}  ,
	{1024, 32, 1, 1}
	},

	/* Output Queue configuration */
	/* num_descs; info_ptr; pkts_per_intr; refill_threshold*/
	{
	{1024, 1, 64, 64} ,
	{1024,  1, 64, 64} ,
	{1024,  1, 64, 64} ,
	{1024,  1, 64, 64} ,
	},

	/* oq_buf_size; oq_intr_pkt; oq_intr_time; */
	1536,
	64,
	100,

#ifdef CVMCS_DMA_IC
	/* dma_intr_pkt; dma_intr_time; */
	64,
	1000,
#endif

};


cn63xx_config_t  default_cn63xx_conf =
{
	/* num_iqs; num_oqs; pending_list_size; */
	{
	4,
	4,
	4096,
	},

	/* Input Queue configuration */
	/* num_descs; instr_type; db_min; db_timeout; */
	{
	{1024, 32, 1, 1} ,
	{1024, 32, 1, 1}  ,
	{1024, 32, 1, 1}  ,
	{1024, 32, 1, 1}
	},

	/* Output Queue configuration */
	/* num_descs; info_ptr; bufsize, pkts_per_intr; refill_threshold*/
	{
	{1024, 1, 1536, 128, 128} ,
	{1024,  1, 1536, 128, 128} ,
	{1024,  1, 1536, 128, 128} ,
	{1024,  1, 1536, 128, 128} ,
	},

	/* oq_intr_pkt; oq_intr_time; */
	64,
	100,

#ifdef CVMCS_DMA_IC
	/* dma_intr_pkt; dma_intr_time; */
	64,
	1000,
#endif

};




cn66xx_config_t  default_cn66xx_conf =
{
	/* num_iqs; num_oqs; pending_list_size; */
	{
	4,
	4,
	4096,
	},

	/* Input Queue configuration */
	/* num_descs; instr_type; db_min; db_timeout; */
	{
	{1024, 32, 1, 1} ,
	{1024, 32, 1, 1}  ,
	{1024, 32, 1, 1}  ,
	{1024, 32, 1, 1}
	},

	/* Output Queue configuration */
	/* num_descs; info_ptr; bufsize, pkts_per_intr; refill_threshold*/
	{
	{1024, 1, 1536, 128, 128} ,
	{1024,  1, 1536, 128, 128} ,
	{1024,  1, 1536, 128, 128} ,
	{1024,  1, 1536, 128, 128} ,
	},

	/* oq_intr_pkt; oq_intr_time; */
	64,
	100,

#ifdef CVMCS_DMA_IC
	/* dma_intr_pkt; dma_intr_time; */
	64,
	1000,
#endif

};




cn68xx_config_t  default_cn68xx_conf =
{
	/* num_iqs; num_oqs; pending_list_size; */
	{
	4,
	4,
	4096,
	},

	/* Input Queue configuration */
	/* num_descs; instr_type; db_min; db_timeout; */
	{
	{1024, 32, 1, 1} ,
	{1024, 32, 1, 1}  ,
	{1024, 32, 1, 1}  ,
	{1024, 32, 1, 1}
	},

	/* Output Queue configuration */
	/* num_descs; info_ptr; bufsize, pkts_per_intr; refill_threshold*/
	{
	{1024, 1, 1536, 128, 128} ,
	{1024,  1, 1536, 128, 128} ,
	{1024,  1, 1536, 128, 128} ,
	{1024,  1, 1536, 128, 128} ,
	},

	/* oq_intr_pkt; oq_intr_time; */
	64,
	100,

#ifdef CVMCS_DMA_IC
	/* dma_intr_pkt; dma_intr_time; */
	64,
	1000,
#endif

};

#endif

