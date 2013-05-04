/***********************************************************************

  OCTEON TOOLKITS                                                         
  Copyright (c) 2007 Cavium Networks. All rights reserved.

  This file, which is part of the OCTEON TOOLKIT from Cavium Networks,
  contains proprietary and confidential information of Cavium Networks
  and in some cases its suppliers.

  Any licensed reproduction, distribution, modification, or other use of
  this file or confidential information embodied in this file is subject
  to your license agreement with Cavium Networks. The applicable license
  terms can be found by contacting Cavium Networks or the appropriate
  representative within your company.

  All other use and disclosure is prohibited.

  Contact Cavium Networks at info@caviumnetworks.com for more information.

 ************************************************************************/ 
 
#ifndef __COMMON_CONFIG_H__
#define __COMMON_CONFIG_H__



/* Content below this point is only used by the cvmx-config tool, and is
 * not used by any C files as CAVIUM_COMPONENT_REQUIREMENT is never defined.
 */

#ifdef CAVIUM_COMPONENT_REQUIREMENT
 
        /* global resource requirement */
 
        cvmxconfig
        {
            fau CVM_FAU_REG_FPA_OOB_COUNT
                        size = 4
                        count = 8
		        description = "FPA out of buffer counters";


                scratch CVM_SCR_MBUFF_INFO_PTR
                        size = 8
                        iobdma = true
                        permanent = true
                        description = "MBuff information";

#ifdef OUT_SWP_IN_SCRATCH // {

		scratch CVMX_SCR_OUT_SWP
		        size = 8
		        iobdma = true
		        permanent = true
		        description = "Work queue entrys";

		scratch CVMX_SCR_OUT_SWP_TAIL
		        size = 8
		        iobdma = true
		        permanent = true
		        description = "Work queue entrys";

#endif // OUT_SWP_IN_SCRATCH }


                fau CVM_FAU_REG_POOL_0_USE_COUNT
                        size = 4
                        description = "pool 0 use count";

                fau CVM_FAU_REG_POOL_1_USE_COUNT
                        size = 4
                        description = "pool 1 use count";

                fau CVM_FAU_REG_POOL_2_USE_COUNT
                        size = 4
                        description = "pool 2 use count";

                fau CVM_FAU_REG_POOL_3_USE_COUNT
                        size = 4
                        description = "pool 3 use count";

                fau CVM_FAU_REG_POOL_4_USE_COUNT
                        size = 4
                        description = "pool 4 use count";

                fau CVM_FAU_REG_POOL_5_USE_COUNT
                        size = 4
                        description = "pool 5 use count";

                fau CVM_FAU_REG_POOL_6_USE_COUNT
                        size = 4
                        description = "pool 6 use count";

                fau CVM_FAU_REG_POOL_7_USE_COUNT
                        size = 4
                        description = "pool 7 use count";


	}
#endif
#endif  /* __COMMON_CONFIG_H__ */
