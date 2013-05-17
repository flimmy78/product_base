#include "fwd_pure_ip.h"

/**
 * Description:
 *  Lookup rule by sip dip  
 *
 * Parameter:
 *  ip: Internal IP header
 *
 * Return:
 *  rule:find rule 
 *  NUll: not find rule 
 *
 */
inline rule_item_t  * acl_table_pure_ip_lookup(cvm_common_ip_hdr_t *ip, cvmx_spinlock_t  **first_lock)
{    
	rule_item_t  *rule  = NULL;
	rule_item_t  *free_rule  = NULL; /*the first free bucket position*/
	rule_item_t  *aging_cur_rule = NULL;
	rule_item_t  *head_rule = NULL;
	rule_item_t  *aging_pre_rule = NULL;
	cvmx_spinlock_t  *head_lock = NULL;

	if (ip == NULL)
	{
		FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_WARNING,
				"acl_table_lookup: NULL Pointer !...\r\n");	
		return NULL;		
	}

	cvmx_fau_atomic_add64(CVM_FAU_TOTAL_ACL_LOOKUP, 1);
	FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_DEBUG,
			"acl_table_lookup: packet-fiveTuple----dip=%d.%d.%d.%d, sip=%d.%d.%d.%d.  \r\n",
			IP_FMT(ip->ip_dst),IP_FMT(ip->ip_src));

	/*look up ACL Table and get the bucket*/
	cvm_two_tupe_hash_lookup(ip->ip_dst, ip->ip_src);
	rule = CASTPTR(rule_item_t, cvmx_scratch_read64(CVM_SCR_ACL_CACHE_PTR));

	head_rule = rule;
	aging_cur_rule = rule;
	aging_pre_rule = rule;

	head_lock = &rule->lock;
	*first_lock = head_lock;
	cvmx_spinlock_lock(head_lock);

    /* debug trace.add by zhaohan */
    if((head_rule->valid_entries == acl_bucket_max_entries)&&(head_rule->next == NULL))
    {
        printf("Should never come to here file%s, line %d.\n",__FILE__, __LINE__);
    }

	/*if the first bucket is empty and there are no more buckets, then insert current flow*/
	if(head_rule->rules.rule_state == RULE_IS_EMPTY)
	{
		if(head_rule->valid_entries == 0)
		{
			head_rule->rules.time_stamp = get_sec(); 
			head_rule->rules.dip = ip->ip_dst;
			head_rule->rules.sip = ip->ip_src;
			head_rule->rules.rule_state = RULE_IS_LEARNING;
			head_rule->rules.action_type = FLOW_ACTION_TOLINUX;
			head_rule->valid_entries++;
			cvmx_fau_atomic_add64(CVM_FAU_ACL_REG, 1);
			cvmx_spinlock_unlock(head_lock);
		
#ifdef USER_TABLE_FUNCTION	
			set_acl_mask(head_rule);
#endif
            
			return rule;
		}
		else /*first bucket is empty but with other buckets*/
		{
			free_rule = head_rule; /*record current free bucket position*/

			if(head_rule->next != NULL)
			{
				aging_cur_rule = head_rule->next;
				rule = head_rule->next;
			}
			else
			{
				FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_MUST_PRINT,
						"acl_table_lookup: Should never come to here file%s, line %d, rule=0x%p, num=0x%d,next=0x%p.\r\n",__FILE__, __LINE__,rule,rule->valid_entries,rule->next);	
				cvmx_spinlock_unlock(head_lock);
				return NULL;
			}
		}
	}

	/*aging the current rule first then compare the rule with the key,loop*/			 
	while(1)
	{
		if(rule == NULL)
		{
			FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_MUST_PRINT,
					"acl_table_lookup: Should never come to here file%s, line %d.\n",__FILE__, __LINE__);
			cvmx_spinlock_unlock(head_lock);
			return NULL;
		}

		FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_DEBUG,
				"acl_table_lookup: current Rule info: dip=%d.%d.%d.%d, sip=%d.%d.%d.%d \r\n",
				IP_FMT(rule->rules.dip),IP_FMT(rule->rules.sip));

		/************************aging first*****************************/
		if((acl_aging_check(&(rule->rules)) > 0) && (rule->rules.rule_state != RULE_IS_STATIC))
		{   
			atomic_add64_nosync(&(rule_cnt_info.rule_age_cnt)); /*rule need to be aging*/
			if(acl_delete_rule_by_aging(head_rule,aging_pre_rule ,aging_cur_rule)==RETURN_OK)
			{
				if(aging_cur_rule == head_rule)     /* add by zhaohan */
				{
					free_rule = head_rule;
				}
				if(aging_pre_rule !=NULL)
				{
					rule=aging_pre_rule->next;
					aging_cur_rule=aging_pre_rule->next;

					if(rule==NULL)/*after age, there is no more bucket*/
					{
						if(free_rule == NULL)
						{
							aging_pre_rule->next = (rule_item_t *)cvmx_malloc(rule_arena, sizeof(rule_item_t));
							if(aging_pre_rule->next == NULL)
							{
								FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, 
										FASTFWD_COMMON_DBG_LVL_ERROR,
										"InsertFastRule: Memory not available for adding rules line=%d\n",__LINE__);
								cvmx_fau_atomic_add64(CVM_FAU_ALLOC_RULE_FAIL, 1);			
								cvmx_spinlock_unlock(head_lock);
								return NULL;
							}	
							memset(aging_pre_rule->next, 0, sizeof(rule_item_t));
							free_rule = aging_pre_rule->next;
							atomic_add64_nosync(&(rule_cnt_info.dynamic_rule_cnt));  /* add by zhaohan */
						}

						free_rule->rules.time_stamp = get_sec(); 
						free_rule->rules.dip = ip->ip_dst;
						free_rule->rules.sip = ip->ip_src;
						free_rule->rules.rule_state = RULE_IS_LEARNING;
						free_rule->rules.action_type = FLOW_ACTION_TOLINUX;
						head_rule->valid_entries++; /* ---------- modify by zhaohan ----------- */
						cvmx_fau_atomic_add64(CVM_FAU_ACL_REG, 1);
						cvmx_spinlock_unlock(head_lock);

#ifdef USER_TABLE_FUNCTION	
            			set_acl_mask(head_rule);
#endif

						return free_rule;
					}
					else
						continue;
				}
				else
				{
					FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_MUST_PRINT,
							"acl_table_lookup: Should never come to here file%s, line %d.\n",__FILE__, __LINE__);
					cvmx_spinlock_unlock(head_lock);
					return NULL;
				}
			}
			else
			{
				FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_MUST_PRINT,
						"acl_table_lookup: Should never come to here file%s, line %d.\r\n",__FILE__, __LINE__);
				cvmx_spinlock_unlock(head_lock);
				return NULL;
			}
		}
		/************************aging end*****************************/
		if((rule->rules.dip == ip->ip_dst) && (rule->rules.sip == ip->ip_src)) 
		{		
			FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_DEBUG,
					"acl_table_lookup: Find the rule=0x%p\n",rule);

			if(rule->rules.rule_state== RULE_IS_STATIC) 
			{
				cvmx_fau_atomic_add64(CVM_FAU_FLOWTABLE_HIT_PACKETS, 1);
				cvmx_spinlock_unlock(head_lock);
				return rule;
			}

			/*是否打开周期性强制老化*/
			if(acl_aging_enable == FUNC_ENABLE)
				rule->rules.time_stamp = get_sec(); 

			if(rule->rules.rule_state == RULE_IS_LEARNED)
			{
				cvmx_fau_atomic_add64(CVM_FAU_FLOWTABLE_HIT_PACKETS, 1);		
				cvmx_spinlock_unlock(head_lock);
				return rule;
			}
			else if(rule->rules.rule_state == RULE_IS_LEARNING)
			{
				cvmx_fau_atomic_add64(CVM_FAU_ACL_REG, 1);
				cvmx_spinlock_unlock(head_lock);
				return rule;
			}
			else
			{
				FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_MUST_PRINT,
						"acl_table_lookup: Should never come to here file%s, line %d, rule_state=%d.\r\n",__FILE__, __LINE__,rule->rules.rule_state);	
				cvmx_spinlock_unlock(head_lock);
				return NULL;
			}

		}
		else
		{
			if(rule->next != NULL)
			{
				aging_pre_rule=rule;
				rule = rule->next;
				aging_cur_rule=rule;
			}
			else
			{
				FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_DEBUG,
						"Need to malloc new rule\n");

				if(free_rule == NULL)
				{
					/* if more than 8 node at one list, replace the first un-static dynamic rule, add by zhaohan */
					if(head_rule->valid_entries >= acl_bucket_max_entries)
					{
						rule_item_t* replace_rule = head_rule->next;
						rule_item_t* pre_rule = head_rule;
						uint64_t *ptmp;
						uint32_t tmp = 0;  

                        /* bug trace */
                        if(replace_rule == NULL)
                        {
                            printf("replace_rule == NULL! should not be here!\n");
                            printf("head_rule->valid_entries = %d\n", head_rule->valid_entries);
                            printf("head_rule->rules.rule_state=%d\n", head_rule->rules.rule_state);
                            printf("header rule info: dip=%d.%d.%d.%d, sip=%d.%d.%d.%d.  \r\n",
                                            IP_FMT(head_rule->rules.dip),IP_FMT(head_rule->rules.sip));
                            return NULL;
                        }

						/* find the first none-static rule */
						while(replace_rule->rules.rule_state == RULE_IS_STATIC)
						{
							if(NULL == replace_rule->next)
							{
								cvmx_spinlock_unlock(head_lock);
								return NULL;
							}
							pre_rule = replace_rule;
							replace_rule = replace_rule->next;
						}

						/* remove & clear replace rule */
						if(replace_rule->next != NULL)
						{
    						pre_rule->next = replace_rule->next;  /* there is a bug when acl_bucket_max_entries = 2 */
                            rule->next = replace_rule;  
                        }
						
						ptmp = (uint64_t*)replace_rule;
						while(tmp < sizeof(rule_item_t)/8) /*144 Bytes*//*没有考虑capwap表项删除*/
						{
							*ptmp = 0;
							ptmp = ptmp + 1;
							tmp++;
						}

						/* add replace_rule to tail */						
						replace_rule->next = NULL;
                		/* debug trace.add by zhaohan */
                        if((head_rule->valid_entries == acl_bucket_max_entries)&&(head_rule->next == NULL))
                        {
                            printf("Should never come to here file%s, line %d.\n",__FILE__, __LINE__);
                        }
						replace_rule->rules.time_stamp = get_sec();
						replace_rule->rules.dip = ip->ip_dst;
						replace_rule->rules.sip = ip->ip_src;
						replace_rule->rules.rule_state = RULE_IS_LEARNING;
						replace_rule->rules.action_type = FLOW_ACTION_TOLINUX;
						cvmx_spinlock_unlock(head_lock);
						cvmx_fau_atomic_add64(CVM_FAU_MAX_RULE_ENTRIES, 1);
						cvmx_fau_atomic_add64(CVM_FAU_ACL_REG, 1);
						return replace_rule;
					}

					rule->next = (rule_item_t *)cvmx_malloc(rule_arena, sizeof(rule_item_t));
					if(rule->next == NULL)
					{
						FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_ERROR, 
						"InsertFastRule: Memory not available for adding rules\n");
						cvmx_fau_atomic_add64(CVM_FAU_ALLOC_RULE_FAIL, 1);	
						cvmx_spinlock_unlock(head_lock);
						return NULL;
					}	
					memset(rule->next, 0, sizeof(rule_item_t));
					free_rule = rule->next;
					atomic_add64_nosync(&(rule_cnt_info.dynamic_rule_cnt));  // add by zhaohan
				}

				free_rule->rules.time_stamp = get_sec(); 
				free_rule->rules.dip = ip->ip_dst;
				free_rule->rules.sip = ip->ip_src;
				free_rule->rules.rule_state = RULE_IS_LEARNING;
				free_rule->rules.action_type = FLOW_ACTION_TOLINUX;
				head_rule->valid_entries++;
				cvmx_spinlock_unlock(head_lock);
				cvmx_fau_atomic_add64(CVM_FAU_ACL_REG, 1);
				return free_rule;

			}
		}
	}
}

/**
 * Description:
 *  Find true_ip ,true_th 
 *
 * Parameter:
 *  true_ip:  IP header
 *  true_th:  tcp_header 
 *  work:  wqe pointer
 *  
 * Return:
 *  FLOW_ACTION_DROP: need drop
 *  FLOW_ACTION_TOLINUX: need to linux process 
 *  FLOW_ACTION_ETH_FORWARD: get true_ip need call acl_table_pure_ip_lookup
 *  true_ip: Internal IP header
 *  true_th: tcp_header 
 *
 */
inline uint32_t pure_ip_get(cvm_common_ip_hdr_t **true_ip, cvm_common_tcp_hdr_t **true_th,cvmx_wqe_t* work)
{
	cvm_common_udp_hdr_t *uh = NULL;
	cvm_common_ip_hdr_t *ip = NULL;

	ip = *true_ip;
	
	if (CVM_COMMON_IPPROTO_UDP == ip->ip_p) 
	{
		uint32_t len;
		int32_t tmp;
		uh = ( cvm_common_udp_hdr_t*)((uint32_t *)ip + ip->ip_hl);
        
		/* destination port of 0 is illegal, based on RFC768. */
		/* port dont need judge */
		
		if (uh->uh_dport == 0)
		{
			cvmx_fau_atomic_add64(CVM_FAU_UDP_BAD_DPORT, 1);
			return FLOW_ACTION_DROP;
		}
		
		/*
		 * Make data length reflect UDP length.
		 * If not enough data to reflect UDP length, drop.
		 */
		len = cvm_common_ntohs((uint16_t)uh->uh_ulen);

		if (ip->ip_len != len) 
		{
			if (len > ip->ip_len || len < sizeof(cvm_common_udp_hdr_t)) 
			{
				FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_MAIN, FASTFWD_COMMON_DBG_LVL_ERROR,
						"The UDP length is ERROR: udp len=%d, ip->ip_len=%d\r\n",len,ip->ip_len);		  

				cvmx_fau_atomic_add64(CVM_FAU_UDP_BAD_LEN, 1);
				return  FLOW_ACTION_DROP;			
			}
		}

		if ((tmp = rx_udp_decap(uh)) == PACKET_TYPE_UNKNOW)
		{
			FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_MAIN, FASTFWD_COMMON_DBG_LVL_DEBUG,
					"UDP is special CAPWAP frame, trap to linux.\n");
			cvmx_fau_atomic_add64(CVM_FAU_UDP_TO_LINUX, 1);
			return  FLOW_ACTION_TOLINUX;			
		}

		CVM_WQE_SET_UNUSED(work, tmp);	

		if (PACKET_TYPE_CAPWAP_802_3 == tmp)
		{
			if (cw_802_3_decap(uh, true_ip, true_th) != 0) 
			{
				FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_MAIN, FASTFWD_COMMON_DBG_LVL_ERROR,
						"Warning]: recv capwap 802.3 packet, decap capwap failed\n");
				return FLOW_ACTION_TOLINUX;			
			}
		}
		else if (PACKET_TYPE_CAPWAP_802_11 == tmp)
		{
			if (cw_802_11_decap(uh, true_ip, true_th) != 0) 
			{
				FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_MAIN, FASTFWD_COMMON_DBG_LVL_ERROR,
						"Warning]: recv capwap packet, decap capwap failed\n");
				return FLOW_ACTION_TOLINUX;			
			}
		}	
	}
	else
	{
		CVM_WQE_SET_UNUSED(work, PACKET_TYPE_ETH_IP);
	}
	
	return FLOW_ACTION_ETH_FORWARD;
}

/**
 * Description:
 *  Self learn by sip dip hash 
 *
 * Parameter:
 *  rule_para:  rule info
 *  cw_cache:  capwap info
 *  
 * Return:
 *  RETURN_ERROR: self learn error
 *  return RETURN_OK: self learn ok
 *
 */
int32_t acl_self_learn_pure_ip_rule(rule_param_t *rule_para, capwap_cache_t *cw_cache)
{
	rule_item_t  *rule ;
	rule_item_t  *head_rule=NULL ;
	rule_item_t  *free_rule  = NULL; /*the first free bucket position*/
	cvmx_spinlock_t          *head_lock = NULL;

	if((NULL == rule_para) || (NULL == cw_cache))
	{
		FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_WARNING,
				"acl_self_learn_rule: Invalid argument\n");
		return RETURN_ERROR;
	}

	FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_DEBUG,
			"acl_self_learn_rule: packet-fiveTuple----dip=%d.%d.%d.%d, sip=%d.%d.%d.%d. l2 type=0x%x, tag=0x%x \r\n",
			IP_FMT(rule_para->dip), IP_FMT(rule_para->sip), rule_para->out_ether_type,rule_para->out_tag);

    /* get user table idx, store to rule */
    fwd_get_user_idx(rule_para);

	/*look up ACL Table and get the bucket*/
	cvm_two_tupe_hash_lookup(rule_para->dip, rule_para->sip);
	rule = CASTPTR(rule_item_t, cvmx_scratch_read64(CVM_SCR_ACL_CACHE_PTR));

	head_rule = rule;
	cvmx_spinlock_lock(&rule->lock);
	head_lock = &rule->lock;


	/*if the first bucket is empty and there are no more buckets, then insert current flow*/
	if(head_rule->rules.rule_state == RULE_IS_EMPTY)
	{
		if(head_rule->valid_entries == 0)
		{
			/*表项为空，没有对应状态切换的表项*/
			FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_DEBUG,
					"acl_self_learn_rule: the acl table have no rule that need to be learned.\r\n");   
			cvmx_spinlock_unlock(head_lock);
			return RETURN_ERROR;
		}
		else /*first bucket is empty but with other buckets*/
		{
			free_rule = head_rule; /*record current free bucket position*/

			if(head_rule->next != NULL)
			{
				rule = head_rule->next;
			}
			else
			{
				FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_MUST_PRINT,
						"acl_self_learn_rule: Should never come to here file%s, line %d, rule=0x%p, num=0x%d,next=0x%p.\r\n",__FILE__, __LINE__,rule,rule->valid_entries,rule->next);    
				cvmx_spinlock_unlock(head_lock);
				return RETURN_ERROR;
			}
		}
	}

	while(1)
	{
		FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_DEBUG,
				"acl_self_learn_rule: lookup the table, rule=0x%p.\r\n",rule);   

		if ((rule->rules.dip == rule_para->dip) && 
				(rule->rules.sip == rule_para->sip))             
		{
			if(rule->rules.rule_state == RULE_IS_LEARNING)
			{
				/*add by sunjc@autelan.com*/
				if(rule->rules.packet_wait > 0)
				{
					rule->rules.packet_wait--;
					CVMX_SYNC;
				}
				/*找到对应表项，学习*/
				if(acl_fill_rule(&(rule->rules), rule_para, cw_cache) == RETURN_ERROR)
				{
					cvmx_spinlock_unlock(head_lock);
					return RETURN_ERROR;
				}
#ifdef USER_TABLE_FUNCTION	
				set_acl_mask(head_rule);
#endif
				cvmx_spinlock_unlock(head_lock);
				return RETURN_OK;
			}
			else if(rule->rules.rule_state == RULE_IS_LEARNED)
			{
				/*add by sunjc@autelan.com*/
				if(rule->rules.packet_wait > 0)
				{
					rule->rules.packet_wait--;
					CVMX_SYNC;
				}
#if 0
				/*找到匹配项，根据策略决定属于更新还是重复配置*/
				if( rule->rules.XXXX == XXXXX)
				{
					/*策略不同，属于策略更新*/
				}
				else
				{
					/*学习的表项与以学习的表项冲突*/
					FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_WARNING, 
							"acl_self_learn_rule:conflict with the existing self learn rule 0x%p! \r\n",rule);
					cvmx_spinlock_unlock(head_lock);
					return RETURN_ERROR;    
				}
#endif
				if(acl_fill_rule(&(rule->rules), rule_para, cw_cache) == RETURN_ERROR)
				{
					cvmx_spinlock_unlock(head_lock);
					return RETURN_ERROR;
				}
				
//#ifdef USER_TABLE_FUNCTION	
				//set_acl_mask(head_rule);
//#endif
				cvmx_spinlock_unlock(head_lock);                
				return RETURN_OK;
			}
			else
			{
				/*add by sunjc@autelan.com*/
				if(rule->rules.packet_wait > 0)
				{
					rule->rules.packet_wait--;   
					CVMX_SYNC;
				}
				/*自学习的表项与静态表项冲突*/
				FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_ERROR, 
						"acl_insert_static_rule:conflict with the static rule 0x%p! \r\n",rule);
				cvmx_spinlock_unlock(head_lock);
				return RETURN_ERROR;    
			}
		}

		if(rule->next != NULL)
			rule = rule->next;
		else
			break;
	}

	/*表项为空，没有对应状态切换的表项*/
	FASTFWD_COMMON_DBG_MSG(FASTFWD_COMMON_MOUDLE_FLOWTABLE, FASTFWD_COMMON_DBG_LVL_DEBUG,
			"acl_insert_eth_rule: the acl table have no rule that need to be learned.\r\n"); 
	cvmx_spinlock_unlock(head_lock);

	return RETURN_ERROR;  

}

