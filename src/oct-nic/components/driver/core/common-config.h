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
			fau CVM_FAU_SE_COEXIST_FLAG
                        size = 8
                        description = "se coexist flag";				

			fau CVM_FAU_INIT_STATE_FLAG
                        size = 8
                        description = "init state flag";
                        
      fau CVM_FAU_SE_CORE_MASK
                        size = 8
                        description = "se core mask";						
	    }
#endif
#endif  /* __COMMON_CONFIG_H__ */

