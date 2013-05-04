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

#ifndef __CVM_COMMON_WQE_H__
#define __CVM_COMMON_WQE_H__


/*
 * Software work queue entry
 */
struct cvm_tcp_tcpcb;
struct cvm_ip_ifaddr;

#ifdef WORK_QUEUE_ENTRY_SIZE_128 // {
typedef struct
{
    cvmx_wqe_word0_t        word0;

    cvmx_wqe_word1_t        word1;

    cvmx_pip_wqe_word2_t    word2;

    /**
     * Field access macros
     */
#define wqe_w1_tag       word1.cn68xx.tag
#define wqe_w1_tag_type  word1.cn68xx.tag_type
#define wqe_w1_len       word1.len


    /**
     * Pointer to the first segment of the packet.
     */
    cvmx_buf_ptr_t             packet_ptr;


    uint8_t packet_data[96 - 32];

} cvm_common_hw_wqe_t;


typedef struct cvm_common_wqe {
    cvm_common_hw_wqe_t hw_wqe;   /* 128-48 bytes from hardware */
#else

typedef struct cvm_common_wqe {
    cvmx_wqe_t hw_wqe;   /* 128 bytes from hardware */

#endif // WORK_QUEUE_ENTRY_SIZE_128 }

    /**
     * Field access macros
     */
#define hwqe_w1_tag       hw_wqe.word1.cn68xx.tag
#define hwqe_w1_tag_type  hw_wqe.word1.cn68xx.tag_type
#define hwqe_w1_len       hw_wqe.word1.len

    /* Software usable space */
    struct cvm_common_wqe	*next;
    struct cvm_ip_ifaddr	*ifa;	/* IP address */
    struct cvm_tcp_tcpcb	*tcb;	/* TCP Connection Block */
    uint32_t	ifidx;			/* Interface index */
    uint8_t	l4_offset;		/* Offset of L4 header */
    uint8_t	l4_prot;		/* L4 protocol */
    uint8_t	opprt;			/* Output port for this entry */
    struct {
        uint8_t sc            : 1;    /* set if lookup yields syncache entry */
        uint8_t gth           : 1;    /* set if ptr indicates a gather list */
        uint8_t no_tag_switch : 1;    /* tells PKO routine not to tag switch
                                         before sending a packet */
        uint8_t mc            : 1;    /* tells if packet is multicast */
        uint8_t listen_lookup : 1;    /* tells if listen lookup has been done */
        uint8_t reserved      : 3;    /* Reserved */
    } control;           /* processing control bits */    
#ifdef CVMX_ABI_N32
    uint32_t reserved2[2];
#endif

#ifndef WORK_QUEUE_ENTRY_SIZE_128 // {
    uint64_t	reserved[16 - 4];
#endif
} cvm_common_wqe_t;



/*
 * Wrapper functions to access WQE fields with offsets/legths 
 * varying between OCTEON models.
 */
static inline int
cvm_common_wqe_get_port(cvm_common_wqe_t *wqe)
{
    return cvmx_wqe_get_port((cvmx_wqe_t *)wqe);
}


static inline void
cvm_common_wqe_set_port(cvm_common_wqe_t *wqe, int port)
{
    cvmx_wqe_set_port((cvmx_wqe_t *)wqe, port);
}


static inline int
cvm_common_wqe_get_grp(cvm_common_wqe_t *wqe)
{
    return cvmx_wqe_get_grp((cvmx_wqe_t *)wqe);
}


static inline void
cvm_common_wqe_set_grp(cvm_common_wqe_t *wqe, int grp)
{
    cvmx_wqe_set_grp((cvmx_wqe_t *)wqe, grp);
}


static inline int
cvm_common_wqe_get_qos(cvm_common_wqe_t *wqe)
{
    return cvmx_wqe_get_qos((cvmx_wqe_t *)wqe);
}


static inline void
cvm_common_wqe_set_qos(cvm_common_wqe_t *wqe, int qos)
{
    cvmx_wqe_set_qos((cvmx_wqe_t *)wqe, qos);
}



static inline int
cvm_common_wqe_get_unused(cvm_common_wqe_t *wqe)
{
    return cvmx_wqe_get_unused8((cvmx_wqe_t *)wqe);
}


static inline void
cvm_common_wqe_set_unused(cvm_common_wqe_t *wqe, int v)
{
    cvmx_wqe_set_unused8((cvmx_wqe_t *)wqe, v);
}


#endif /* _CVM_COMMON_INLINE_H__ */
