/** 
@mainpage Octeon Common Component

@section common_contents Contents

    - @ref introduction
    - @ref fpa_buffer_monitoring

@section introduction 1 Introduction

 OCTEON common component provides support to other 
 compoentns including TCP, IP, ENET, PCI driver etc. One of
 the service provided by common component is to monitor the 
 invalid behaviour of the software  which may result in FPA 
 unit corruption. This feature is also  known as FPA Buffer 
 Integrity Monitoring.


@section fpa_buffer_monitoring 2 FPA Buffer Integrity Monitoring
 Octeon FPA unit does not perform any intelligent operations from 
 preventing the software corrupting its pools. Software can do a
 number of operations which can result in FPA pool corruption 
 including:

    - freeing a NULL pointer to FPA pool
    - freeing a garbage pointer to FPA pool
    - freeing the same pointer twice
    - freeing a pointer to wrong FPA pool

 The FPA buffer integrity monitoring feature provides software support
 to catch all these error conditions.

 Below is a list of various component common macros and functions which
 are used by this feature:

     - cvm_common_fpa_add_pool_info()
     - cvm_common_fpa_display_all_pool_info()
     - CVM_COMMON_CHECK_FPA_POOL()
     - CVM_COMMON_WHICH_FPA_POOL()
     - CVM_COMMON_INIT_FPA_CHECKS()
     - CVM_COMMON_MARK_FPA_BUFFER_ALLOC()
     - CVM_COMMON_MARK_FPA_BUFFER_FREE()
     - CVM_COMMON_CHECK_BUFFER_BOUNDRIES()
     - cvm_common_alloc_fpa_buffer_sync()
     - cvm_common_alloc_fpa_buffer()
     - cvm_common_free_fpa_buffer()
     - cvm_common_mbuf_free()
     - cvm_common_packet_free()
     - cvm_common_discard_swp()
   
*/
