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

#ifndef __CVM_COMMON_HOST_H__
#define __CVM_COMMON_HOST_H__


#if defined(__KERNEL__) && defined(linux)
/* for backlog implementaion */
struct cvm_common_tcp_backlog
   {
    /* backlog, as defined in listen call */
    uint32_t   backlog_value;

    /* no of descriptors which have arrived (callback called)
       and not yet accepted by application */
    uint32_t   backlog_used;
   };
#endif


#endif /* __CVM_COMMON_HOST_H__ */
