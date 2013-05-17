#ifndef _FWD_PURE_IP_H_
#define _FWD_PURE_IP_H_

#include "acl.h"

extern inline rule_item_t * acl_table_pure_ip_lookup(cvm_common_ip_hdr_t *ip, cvmx_spinlock_t  **first_lock);

extern inline uint32_t pure_ip_get(cvm_common_ip_hdr_t **true_ip, cvm_common_tcp_hdr_t **true_th,cvmx_wqe_t* work);

extern int32_t acl_self_learn_pure_ip_rule(rule_param_t *rule_para, capwap_cache_t *cw_cache);


#endif
