#ifdef __cplusplus
extern "C"
{
#endif
#include <errno.h>
#include <sys/mman.h> 
#include "board/netlink.h"
#include "sem/product.h"
#include "board/board_define.h"
#include "sysdef/npd_sysdef.h"
#include "npd_log.h"
#include "npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "hmd/hmdpub.h"
#include "npd_board.h"
#include "npd_vlan.h"
#include "npd_trunk.h"
#include "npd_intf.h"
#include "npd_dynamic_trunk.h"
#include "sysdef/returncode.h"
#include <cvm/autelan_product.h>   /* for product_info */
#include <fcntl.h>

int sock_n_fd = 0;
struct msghdr n_msg;
struct nlmsghdr *n_nlh = NULL;
struct sockaddr_nl n_src_addr, n_dest_addr;
struct iovec n_iov;

int sock_ks_fd = 0;
struct msghdr ks_msg;
struct nlmsghdr *ks_nlh = NULL;
struct sockaddr_nl ks_src_addr, ks_dest_addr;
struct iovec ks_iov;

extern struct global_ethport_s **global_ethport;
extern struct global_ethport_s *start_fp[MAX_SLOT_COUNT];
extern char g_dis_dev[MAX_ASIC_NUM];
extern dst_trunk_s *g_dst_trunk;
/* to define the master info zhangdi@autelan.com 2012-03-23 */
extern BoardInfo board_info;
extern char init_mac[];
int npd_netlink_send(char *msgBuf, int msgLen)
{
	memcpy(NLMSG_DATA(n_nlh), msgBuf, msgLen);
	
	nl_msg_head_t *head = (nl_msg_head_t*)NLMSG_DATA(n_nlh);
	
	syslog_ax_board_dbg("Netlink npd(%d) send\n", head->pid);

	if(sendmsg(sock_n_fd, &n_msg, 0) < 0)
	{
        syslog_ax_board_dbg("Failed npd netlink send : %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

int npd_netlink_init(void)
{
	/* Initialize data field */
	memset(&n_src_addr, 0, sizeof(n_src_addr));
	memset(&n_dest_addr, 0, sizeof(n_dest_addr));
	memset(&n_iov, 0, sizeof(n_iov));
	memset(&n_msg, 0, sizeof(n_msg));
	
	/* Create netlink socket use NETLINK_DISTRIBUTED(18) */
	if ((sock_n_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_DISTRIBUTED)) < 0) {
		syslog_ax_board_dbg("Failed socket\n");
		return -1;
	}

	/* Fill in src_addr */
	n_src_addr.nl_family = AF_NETLINK;
	n_src_addr.nl_pid = getpid();
	/* Focus */
	n_src_addr.nl_groups = 1;

	if (bind(sock_n_fd, (struct sockaddr*)&n_src_addr, sizeof(n_src_addr)) < 0) {
		syslog_ax_board_dbg("Failed bind\n");
		return -1;
	}

	/* Fill in dest_addr */
	n_dest_addr.nl_pid = 0;
	n_dest_addr.nl_family = AF_NETLINK;
	/* Focus */
	n_dest_addr.nl_groups = 1;

	/* Initialize buffer */
	if((n_nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD))) == NULL) {
		syslog_ax_board_dbg("Failed malloc\n");
		return -1;
	}

	memset(n_nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	n_iov.iov_base = (void *)n_nlh;
	n_iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);
	n_msg.msg_name = (void *)&n_dest_addr;
	n_msg.msg_namelen = sizeof(n_dest_addr);
	n_msg.msg_iov = &n_iov;
	n_msg.msg_iovlen = 1;
	
	return 0;
}

int npd_netlink_init_for_ksem(void)
{
	/* Initialize data field */
	memset(&ks_src_addr, 0, sizeof(ks_src_addr));
	memset(&ks_dest_addr, 0, sizeof(ks_dest_addr));
	memset(&ks_iov, 0, sizeof(ks_iov));
	memset(&ks_msg, 0, sizeof(ks_msg));
	
	/* Create netlink socket use NETLINK_KSEM(19) */
	if ((sock_ks_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_KSEM)) < 0) {
		syslog_ax_board_dbg("Failed socket\n");
		return -1;
	}

	/* Fill in src_addr */
	ks_src_addr.nl_family = AF_NETLINK;
	ks_src_addr.nl_pid = getpid();
	/* Focus */
	ks_src_addr.nl_groups = 1;

	if (bind(sock_ks_fd, (struct sockaddr*)&ks_src_addr, sizeof(ks_src_addr)) < 0) {
		syslog_ax_board_dbg("Failed bind\n");
		return -1;
	}

	/* Fill in dest_addr */
	ks_dest_addr.nl_pid = 0;
	ks_dest_addr.nl_family = AF_NETLINK;
	/* Focus */
	ks_dest_addr.nl_groups = 1;

	/* Initialize buffer */
	if((ks_nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD))) == NULL) {
		syslog_ax_board_dbg("Failed malloc\n");
		return -1;
	}

	memset(ks_nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	ks_iov.iov_base = (void *)ks_nlh;
	ks_iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);
	ks_msg.msg_name = (void *)&ks_dest_addr;
	ks_msg.msg_namelen = sizeof(ks_dest_addr);
	ks_msg.msg_iov = &ks_iov;
	ks_msg.msg_iovlen = 1;
	
	return 0;
}


void npd_netlink_recv_thread(void)
{
    int ret=0, i = 0;
	int product_no = 0;
	fd_set read_fds;
	int maxfd = 0;
	unsigned short vlanId = 0, slotId = 0,trunkId = 0;
	int devnum= 0,virportNum = 0,action_type = 0;
	int slot_no = 0,port_no = 0;
	unsigned char endis = 0;

	/* tell my thread id */
	npd_init_tell_whoami("Npd_netlink_recv", 0);	

    if(npd_netlink_init() < 0)
    {
        syslog_ax_board_dbg("Failed npd_netlink_init\n");
	}
	
	if(npd_netlink_init_for_ksem() < 0)
	{
        syslog_ax_board_dbg("Failed npd_netlink_init_for_ksem\n");
	}
	
	while( 1 )
	{
		FD_ZERO(&read_fds);
		FD_SET(sock_n_fd, &read_fds);
		FD_SET(sock_ks_fd, &read_fds);
        maxfd = sock_n_fd > sock_ks_fd ? sock_n_fd : sock_ks_fd;
		
		ret = select(maxfd+1, &read_fds, NULL, NULL, NULL);
		if(ret < 0)
		{
            syslog_ax_board_dbg("Failed select\n");
		}
		else if(ret == 0)
		{

		}
		else
		{
			if(FD_ISSET(sock_n_fd, &read_fds))
			{
                if(recvmsg(sock_n_fd, &n_msg, 0) < 0)
                {
                    syslog_ax_board_dbg("Failed npd netlink recv : %s\n", strerror(errno));
        		}
        		
        		nl_msg_head_t* head = (nl_msg_head_t*)NLMSG_DATA(n_nlh);

                syslog_ax_board_dbg("Npd netlink object(%d) recvfrom sem pid:%d\n",head->object, head->pid);

        		if(head->object == NPD_MODULE || head->object == COMMON_MODULE)
        		{
                    char *chTemp = NLMSG_DATA(n_nlh) + sizeof(nl_msg_head_t);            
                    
                    for(i = 0; i < head->count; i++)
                    {
            			if(chTemp)
            			{
                			netlink_msg_t *nl_msg= (netlink_msg_t*)chTemp;	
                			syslog_ax_board_dbg("Npd netlink msgType(%d)\n", nl_msg->msgType);	
                			switch(nl_msg->msgType)
                			{
                				case SYSTEM_STATE_NOTIFIER_EVENT:
            					    npd_syslog_dbg("##### from sem : SYSTEM_STATE_NOTIFIER_EVENT \n");                					
                					if(nl_msg->msgData.productInfo.action_type == SYSTEM_READY)
                					{
                						npd_syslog_info("##### System ready.\n");
                                        /* Init the stage 2 of NPD */
										ret = npd_init_second_stage();
										if (0 != ret)
                                        {
                                            syslog_ax_board_err("npd_init_second_stage on MCB error !\n");
                                        }																					
										/* Add for check port status in trunk of AX81-SMU */
										if(BOARD_TYPE == BOARD_TYPE_AX81_SMU || BOARD_TYPE == BOARD_TYPE_AX81_SMUE)
										{
											ret = npd_update_smu_trunk_info();
											if (0 != ret)
                                            {
                                                syslog_ax_board_err("npd_update_smu_trunk_info on MCB error !\n");
                                            }
                                        }

                					}
									else if(nl_msg->msgData.productInfo.action_type == SYSTEM_RUNNING)
									{
                						npd_syslog_info("##### System running.\n");
										/* 1. Enable asic */
										if(BOARD_TYPE != BOARD_TYPE_AX71_CRSMU)     /* jump ax71-crsmu, because it's already enable */
										{
                                            nam_enable_device(0,1);   /* enable asic 0 */
                                            if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
                                            {
												/* can`t enable asic 1 for 4X&12X together wangchong@autelan 20120920
                                        	    nam_enable_device(1,1);   	
												*/
                                            }
                    						npd_syslog_info("Enable asic after system-running OK.\n");											
										}
										
										/* 2. Set aw.state aftet system running */
										ret = npd_init_tell_system_state_end();
										if(0!=ret)
										{
                       						npd_syslog_info("System running set aw.state Error.\n");															
										}
										else
										{
                       						npd_syslog_info("System running set aw.state OK.\n");																					
										}									
									}									
                					break;
                				case BOARD_STATE_NOTIFIER_EVENT:
            					    npd_syslog_info("##### from sem : BOARD_STATE_NOTIFIER_EVENT \n");									
            						if(nl_msg->msgData.boardInfo.action_type == BOARD_INSERTED)
            						{
                					    npd_syslog_info("Board inserted slot %d\n",nl_msg->msgData.boardInfo.slotid);
										
										if(1 == IS_MASTER_NPD)
										{
											/* Add the port on the slot to default vlan */
                                            ret = npd_update_slotx_vlan_info(nl_msg->msgData.boardInfo.slotid, BOARD_INSERTED);
											if (0 != ret)
                                            {
                                                syslog_ax_board_err("update default vlan on MCB error !\n");
                                            }
                                    		else
                                    		{
                                    		    syslog_ax_board_dbg("update default vlan on MCB OK !\n");
                                    		}
											
											/* Send the all vlan info to the insert slot */
    										if(1 == IS_ACTIVE_MASTER_NPD)
    										{
                								for(vlanId = 1; vlanId < 4094; vlanId++)
                								{
                                                    if(g_vlanlist[vlanId-1].vlanStat)
                                                    {
                										syslog_ax_board_dbg("vlanId = %d, slot = %d\n", vlanId, nl_msg->msgData.boardInfo.slotid);
                                                        npd_asic_vlan_notifier(vlanId, nl_msg->msgData.boardInfo.slotid);
                                                        /* sleep for a while, to avoid sem netlink to npd error on dist slot! */
														usleep(50000);  /* delay 50 ms is ok, zhangdi@autelan.com 2012-07-05 */
													}
                								}
    										}
										}
										else if(SLOT_ID == nl_msg->msgData.boardInfo.slotid)
										{
											/* 1. Enable asic if the board is hot-insert */
    										if(BOARD_TYPE != BOARD_TYPE_AX71_CRSMU)     /* jump ax71-crsmu*/
    										{
                                                nam_enable_device(0,1);   /* enable asic 0 */
                                                if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
                                                {
                                            	    /* can`t enable asic 1 for 4X&12X together wangchong@autelan 20120920
                                        	        nam_enable_device(1,1);   	
												    */
                                                }
                        						npd_syslog_info("Enable asic for board-insert OK.\n");												
    										}
											
                                            /* 2. Init the stage 2 of NPD on local board */
    										ret = npd_init_second_stage();
    										if (0 != ret)
                                            {
                                                syslog_ax_board_err("npd_init_second_stage error!\n");
                                            }
											
    										/* 3. Set aw.state if board is hot-insert */
    										ret = npd_init_tell_system_state_end();
    										if(0!=ret)
    										{
                           						npd_syslog_info("Hot insert set aw.state Error.\n");											
    										}
    										else
    										{
                           						npd_syslog_info("Hot insert set aw.state OK.\n");
    										}											
											
										}
										else
										{
											/* other board, do nothing */
										}
            						}
            						else if(nl_msg->msgData.boardInfo.action_type == BOARD_REMOVED)
            						{
										/* all the other board will receive the msg */
                					    npd_syslog_info("Board remove slot %d\n",nl_msg->msgData.boardInfo.slotid);
										ret = npd_update_slotx_vlan_info(nl_msg->msgData.boardInfo.slotid, BOARD_REMOVED);
										if (0 != ret)
                                        {
                                            syslog_ax_board_err("update vlan on local board error !\n");
                                        }
                                		else
                                		{
                                		    syslog_ax_board_dbg("update vlan on local board OK !\n");
                                		}
            						}
									else if(nl_msg->msgData.boardInfo.action_type == BOARD_INSERTED_AND_REMOVED)
									{
										/* who receive?? */
                					    npd_syslog_info("Board remove & insert slot %d\n",nl_msg->msgData.boardInfo.slotid);
										
									}
									else
									{
										/* other msg, hot remove-insert */
									}
        							break;		
        					    case ASIC_VLAN_NOTIFIER_ENVET:
            					    npd_syslog_info("###### from sem : ASIC_VLAN_NOTIFIER_ENVET vid = %d \n",nl_msg->msgData.vlanInfo.vlan_id);

									/* receive vlan info & creat vlan */
        							vlanId = nl_msg->msgData.vlanInfo.vlan_id;
									slotId = nl_msg->msgData.vlanInfo.slot_id;
									/* jump the vlan-1, because it has be create at npd stage1 */
									if(vlanId != 1)
									{
                                        npd_asic_creat_vlan(vlanId, nl_msg->msgData.vlanInfo.vlan_name);
									}
  									syslog_ax_board_dbg("vlanId = %d, slot = %d\n", vlanId, slotId);
                                  
        							for(i = 0; i < CHASSIS_SLOT_COUNT; i++)
        							{
										/* if the vlan have bond_info */
        								if(nl_msg->msgData.vlanInfo.bond_slot[i] != 0 )
        								{
        									syslog_ax_board_dbg("bond_info = %d,\n", i+1);
        									npd_asic_bond_vlan_to_slot(vlanId, i+1,nl_msg->msgData.vlanInfo.bond_slot[i]);
        								}
										/* If the board is master, then update all vlan info */
                                        if(1 == IS_MASTER_NPD)
                                        {
                                        	g_vlanlist[vlanId-1].untagPortBmp[i].high_bmp = nl_msg->msgData.vlanInfo.untagPortBmp[i].high_bmp;
                                        	g_vlanlist[vlanId-1].untagPortBmp[i].low_bmp = nl_msg->msgData.vlanInfo.untagPortBmp[i].low_bmp;
                                        	g_vlanlist[vlanId-1].tagPortBmp[i].high_bmp =nl_msg->msgData.vlanInfo.tagPortBmp[i].high_bmp;
                                        	g_vlanlist[vlanId-1].tagPortBmp[i].low_bmp =nl_msg->msgData.vlanInfo.tagPortBmp[i].low_bmp;	
                                        }
        							}

                                	ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
                                	if( ret!=0 )
                                    {
                                        syslog_ax_board_err("msync shm_vlan failed \n" );   
                                    }
									
        							break;
								#if 0
								/* recieve msg from sem! for bug: CHINAMOBILE-203*/	
                				case ACTIVE_STDBY_SWITCH_EVENT:
            					    npd_syslog_info("############ from sem to npd: ACTIVE_STDBY_SWITCH_EVENT ############\n");
									/* enable cscd port of Standby-MCB */
                                    if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
                                    {
                                    	if(1 != is_active_master)    /* if local board is not Active-MCB */
                                    	{
                                         	ret = npd_enable_cscd_port();
                                            if (0 != ret)
                                            {
                                                syslog_ax_board_dbg("enable cscd port on Standby-MCB error !\n");
                                            }
                                    		else
                                    		{
                                    			is_active_master = 1;
                                    	        syslog_ax_board_dbg("enable cscd port on Standby-MCB OK !\n");
                                    		}																				
                                    	}
                                    }
            						break;
                            	#endif									
                 				case ACTIVE_STDBY_SWITCH_OCCUR_EVENT:   /* switch start */
            					    npd_syslog_info("###### from sem : ACTIVE_STDBY_SWITCH_OCCUR_EVENT \n");
            						/* enable cscd port of Standby-MCB on AX8610 & AX8603 */
                                    if((PRODUCT_TYPE==AUTELAN_PRODUCT_AX8610)||(PRODUCT_TYPE==AUTELAN_PRODUCT_AX8606)\
										|| (PRODUCT_TYPE == AUTELAN_PRODUCT_AX8800))
                                    {
										/* Disable cscd port if local board is Active-MCB */
                                    	if(1 == IS_ACTIVE_MASTER_NPD)
                                    	{
                                         	ret = npd_disable_cscd_port();
                                            if (0 != ret)
                                            {
                                                syslog_ax_board_dbg("disable cscd port on Active-MCB error !\n");
                                            }
                                    		else
                                    		{
												/* Change to be standby master */
                                    			board_info.is_active_master = 0;
                                    	        syslog_ax_board_dbg("disable cscd port on Active-MCB OK !\n");
                                    		}																				
                                    	}
										else
                                    	{
                                            /* Enable the cscd port on Standby-MCB */											
                                         	ret = npd_enable_cscd_port();
                                            if (0 != ret)
                                            {
                                                syslog_ax_board_dbg("enable cscd port on Standby-MCB error !\n");
                                            }
                                    		else
                                    		{
												/* Change to be active master */
                                    			board_info.is_active_master = 1;
                                    	        syslog_ax_board_dbg("enable cscd port on Standby-MCB OK !\n");
                                    		}																				
                                    	}
                                    }
									else
									{
										/* On AX7605i & AX8603 do nothing */
									}
            					    npd_syslog_info("npd make CMD switch change OK !\n");									
            						break;                               
                				case ACTIVE_STDBY_SWITCH_COMPLETE_EVENT:    /* switch OK, update the master info */
            					    npd_syslog_info("##### from sem : ACTIVE_STDBY_SWITCH_COMPLETE_EVENT \n");
									/* no use */
            						break;
								case ASIC_ETHPORT_UPDATE_EVENT:
            					    npd_syslog_info("##### from sem : ASIC_ETHPORT_UPDATE_EVENT \n");									
									devnum = nl_msg->msgData.portInfo.devNum;
									virportNum = nl_msg->msgData.portInfo.virportNum;
									action_type = nl_msg->msgData.portInfo.action_type;
									trunkId = nl_msg->msgData.portInfo.trunkId;
									slot_no = nl_msg->msgData.portInfo.slot_id;
									port_no = nl_msg->msgData.portInfo.port_no;
									if((trunkId > 0) && (trunkId < 110))
									{
										if((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I))
										{
											if(action_type == 1)
												endis = 1;
											else
												endis = 0;
											if(CSCD_TYPE == 1)/*only is CSCD system can endis trunk member*/
											{
												ret = nam_asic_trunk_port_endis_for_distributed(0,devnum,virportNum,trunkId,endis);
												if(ret != 0)
												{
													syslog_ax_board_err("nam_asic_trunk_port_endis_for_distributed fail ! ret %d\n",ret);
												}
												if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
												{
													ret = nam_asic_trunk_port_endis_for_distributed(1,devnum,virportNum,trunkId,endis);
													if(ret != 0)
													{
														syslog_ax_board_err("nam_asic_trunk_port_endis_for_distributed fail ! \n");
													}
												}
											}

											if((CSCD_TYPE == 1) || (IS_MASTER_NPD == 1))/*if is CSCD system or master active update trunk info*/
											{
												for(i=0;i<g_dst_trunk[trunkId-1].portLog;i++)
												{
													if((g_dst_trunk[trunkId-1].portmap[i].slot == slot_no) && (g_dst_trunk[trunkId-1].portmap[i].port == port_no))
													{
														g_dst_trunk[trunkId-1].portmap[i].enable = endis;
															break;
													}
												}

												for(i=0;i<g_dst_trunk[trunkId-1].portLog;i++)/*update trunk state*/
												{
													if(g_dst_trunk[trunkId-1].portmap[i].enable)
													{
														g_dst_trunk[trunkId-1].tLinkState = TRUNK_STATE_UP_E;
														break;
													}
													else
													{
														g_dst_trunk[trunkId-1].tLinkState = TRUNK_STATE_DOWN_E;
													}
												}
												ret = msync(g_dst_trunk, sizeof(dst_trunk_s)*127, MS_SYNC);
												if( ret != 0 )
											    {
											        syslog_ax_board_dbg("msync shm_trunk failed \n" );
											    }
												else
												{
													syslog_ax_board_dbg("update g_dst_trunk info successfully!\n");
												}
											}
										}
									}
								
									syslog_ax_board_dbg("#####  devnum = %d,virportNum = %d,action_type = %d,trunkId = %d \n",devnum,virportNum,action_type,trunkId);
									break;
								case ASIC_DYNAMIC_TRUNK_NOTIFIER_EVENT:								
									devnum = nl_msg->msgData.portInfo.devNum;
									virportNum = nl_msg->msgData.portInfo.virportNum;
									action_type = nl_msg->msgData.portInfo.action_type;
									trunkId = nl_msg->msgData.portInfo.trunkId;
									slot_no = nl_msg->msgData.portInfo.slot_id;
									port_no = nl_msg->msgData.portInfo.port_no;
						
		
									if((trunkId > 118) && (trunkId < 127))
									{
										if(action_type == 1)
												endis = 1;
											else
												endis = 0;
										/*for dynamic trunk*/
										if((CSCD_TYPE == 1) && (SLOT_ID != slot_no))/*only is CSCD system can endis trunk member*/
										{
										
											ret = nam_asic_trunk_map_table_update(0,trunkId,devnum,virportNum,endis,endis);
											if(ret != 0)
											{
												syslog_ax_board_err("nam_asic_trunk_port_endis_for_distributed fail ! ret %d\n",ret);
											}
											if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
											{
												ret = nam_asic_trunk_map_table_update(1,trunkId,devnum,virportNum,endis,endis);
												if(ret != 0)
												{
													syslog_ax_board_err("nam_asic_trunk_port_endis_for_distributed fail ! \n");
												}
											}
											
											
										}
									}
									syslog_ax_board_dbg("#####  devnum = %d,virportNum = %d,action_type = %d,trunkId = %d \n",devnum,virportNum,action_type,trunkId);
									npd_syslog_info("LACP:  eth-port %d/%d %s Link Aggregate 1.\n",slot_no,port_no,endis?"joined":"left");	
									break;
                				default:
                					syslog_ax_board_dbg("Error:npd recv an error message type\n");
                					break;
                			}

                			chTemp = chTemp + sizeof(netlink_msg_t);
            			}
                    }
        		}
			}
			else if(FD_ISSET(sock_ks_fd, &read_fds))
			{
                if(recvmsg(sock_ks_fd, &ks_msg, 0) <= 0)
                {
                    syslog_ax_board_dbg("Failed npd netlink recv : %s\n", strerror(errno));
        		}
    
        		nl_msg_head_t* head = (nl_msg_head_t*)NLMSG_DATA(ks_nlh);
                netlink_msg_t* nl_msg= (netlink_msg_t*)(NLMSG_DATA(ks_nlh) + sizeof(nl_msg_head_t));	
                syslog_ax_board_dbg("Npd netlink object(%d) recvfrom ksem pid: %d\n",head->object, head->pid);
                if(head->object == NPD_MODULE || head->object == COMMON_MODULE)
                {
           			syslog_ax_board_dbg("Npd netlink msgType(%d)\n", nl_msg->msgType);						
                    switch(nl_msg->msgType)
                    {
        				case BOARD_STATE_NOTIFIER_EVENT:
    						if(nl_msg->msgData.boardInfo.action_type == BOARD_INSERTED)
    						{
        					    syslog_ax_board_dbg("Board inserting\n");
    						}
    						else if(nl_msg->msgData.boardInfo.action_type == BOARD_REMOVED)
    						{
                                syslog_ax_board_dbg("Board removing\n");
    						}
    						break;
        				case ACTIVE_STDBY_SWITCH_EVENT:
							#if 0   /* here is no use, change to ACTIVE_STDBY_SWITCH_OCCUR_EVENT zhangdi 2012-03-23 */
        					npd_syslog_info("############ from ksem to npd: ACTIVE_STDBY_SWITCH_EVENT ############\n");
    						/* enable cscd port of Standby-MCB */
                            if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
                            {
                            	if(1 != is_active_master)    /* if local board is not Active-MCB */
                            	{
                                 	ret = npd_enable_cscd_port();
                                    if (0 != ret)
                                    {
                                        syslog_ax_board_dbg("enable cscd port on Standby-MCB error !\n");
                                    }
                            		else
                            		{
                            			is_active_master = 1;
                            	        syslog_ax_board_dbg("enable cscd port on Standby-MCB OK !\n");
                            		}																				
                            	}
                            }
							#endif
    						break;							
        				default:
        					syslog_ax_board_dbg("Error:npd recv an error message type\n");
        					break;
            		}	
                }
			}
		}
	}
}

int npd_asic_creat_vlan(unsigned short vlanId, char *vlanName)
{
	int ret = 0;
	unsigned int untagbmp[2] = {0};
	unsigned int tagbmp[2] = {0};
	
	syslog_ax_board_dbg("npd_asic_creat_vlan: vlanId = %d, vlanName = %s!\n", vlanId, vlanName);
    if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
		ret = ETHPORT_RETURN_CODE_NO_SUCH_VLAN;
	}
	/* Check out if specified vlan has been created.  bug:AXSSZFI-277 */
	else if(VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(vlanId))
	{
		ret = VLAN_RETURN_CODE_VLAN_EXISTS;
	}		
	else
	{
		/* call npd_vlan_interface_active, to do somthing as BELOW:*/
		ret = npd_vlan_activate(vlanId,vlanName);
		if (VLAN_RETURN_CODE_ERR_NONE != ret) {
			syslog_ax_board_err("npd_vlan_activate vlan:%d error !\n",vlanId);
		}
		else   /* vlan active OK */
		{
            #if 1   /* we do not need mstp */
			npd_mstp_add_vlan_on_mst(vlanId,untagbmp,tagbmp);
			#endif
			
			/* add for distributed OS, add cscd port to all vlan */
			ret = npd_vlan_cscd_add(vlanId);
			if (VLAN_RETURN_CODE_ERR_NONE != ret) {
				syslog_ax_board_err("npd_vlan_add_cscd to vlan %d error !\n",vlanId);
				ret = VLAN_RETURN_CODE_ERR_GENERAL;
			}
			else    /* cscd port add OK */
			{
    	        /* we need update the g_vlanlist[] on local-board */		
    			ret = npd_vlanlist_valid(vlanId,vlanName);
    			if (VLAN_RETURN_CODE_ERR_NONE != ret) {
    				syslog_ax_board_err("npd set npd_vlanlist_valid error !\n");
    				ret = VLAN_RETURN_CODE_ERR_GENERAL;
    			}
    			else    /* vlanlist valid OK */
    			{
        			/* update g_vlans[] on AX81-SMU, for L3-vlan */
        			if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
        			{
        			    g_vlans[vlanId-1]->brgVlan.state = VLAN_STATE_UP_E;
        			}
    			}
			}
		}
	}

    ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
	if( ret!=0 )
    {
        syslog_ax_board_err("msync shm_vlan failed \n" );   
    }

	return ret;
}

int npd_asic_bond_vlan_to_slot(unsigned short vlanId, unsigned short slotId,unsigned int bond_info)
{
	unsigned int ret = VLAN_RETURN_CODE_ERR_NONE;
	int cpu_no=0,cpu_port_no=0,i=0;
	
	syslog_ax_board_dbg("npd_asic_bond_vlan_to_slot!\n"); 
	if(slotId == SLOT_ID)  /* is on local board */
	{
        if(g_vlanlist[vlanId-1].vlanStat == 0)
        {
    	    syslog_ax_board_err("vlan %d is not exists ");	
    		ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
        }
    	else
    	{
        	/*syslog_ax_board_dbg("Real %s vlan %d to slot: %d \n",(isbond==1)?"bond":"unbond",vlanId,slotId);*/
			if(g_vlanlist[vlanId-1].bond_slot[slotId-1] == 0)
			{
                for(i = 0;i<MAX_CPU_PORT_NUM;i++)
                {
    				if(((bond_info>>i)&0x1)==0x1)
    				{
    					cpu_no = i/8; 
    					cpu_port_no = i%8;
            		    /*ret = npd_vlan_to_cpu_port_add(vlanId);*/
        				ret = npd_vlan_to_cpu_port_add(vlanId,cpu_no,cpu_port_no);
        				if(ret==VLAN_RETURN_CODE_ERR_NONE)
        				{
        				    g_vlanlist[vlanId-1].bond_slot[slotId-1] |= (unsigned int)(0x1<<1);
        				}					
    				}    				
                }				
			}
			else
			{
				/* this vlan have already bonded, can not bond */
				ret = VLAN_RETURN_CODE_ERR_HW;
			}
    	}
	}
	else   /* not my board,only update the vlan_slot[] */
	{
        if(g_vlanlist[vlanId-1].vlanStat == 0)
        {
    	    syslog_ax_board_err("vlan %d is not exists on my slot:%d ",slotId);	
    		ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
        }
    	else
    	{
        	/*syslog_ax_board_dbg("Not-Real only update: %s vlan %d to slot %d\n",(isbond==1)?"bond":"unbond",vlanId,slotId);*/
			g_vlanlist[vlanId-1].bond_slot[slotId-1] = bond_info;	
    	}		
	}

    ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
	if( ret!=0 )
    {
        syslog_ax_board_err("msync shm_vlan failed \n" );   
    }

	return ret;
}


int npd_asic_ehtport_update_notifier(unsigned int eth_g_index)
{	
	char msgbuf[512] = {0};
	int msgLen;
	int i = 0;
	unsigned long ret = 0,member = 0;
	unsigned short trunkId = 0;
	unsigned int port_link_state = 0,actdevNum = 0;
	unsigned char devnum = 0,portNum = 0;
	unsigned int slot_no = 0,port_no = 0;
    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg = (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));
	slot_no = CHASSIS_SLOT_INDEX2NO(SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index));
	port_no = ETH_LOCAL_INDEX2NO(SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index),ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index));
	
	if((BOARD_TYPE != BOARD_TYPE_AX71_CRSMU) && (BOARD_TYPE != BOARD_TYPE_AX81_SMU))
	{
		ret = npd_get_port_link_status(eth_g_index,&port_link_state);
		if(0 != ret) 
		{
			syslog_ax_board_err("get port link status FAIL !\n");
		}
		ret = npd_get_devport_by_global_index(eth_g_index,&devnum,&portNum);
		if (ret != 0) 
		{
			syslog_ax_board_err("global_index %d convert to devPort error.\n",eth_g_index);
		}
		syslog_ax_board_dbg("devNum = %d ,portNum = %d\n",devnum,portNum);
		ret = nam_trunk_port_trunkId_get(devnum,portNum,&member,&trunkId);
		if(ret != 0)
		{
			syslog_ax_board_err("get trunk ID err !\n");
		}
		if((member == 0) || (trunkId > 110))    /* code optimize: no need check 0. zhangdi@autelan.com 3013-01-18 */
		{
			trunkId = 0;/*if port is not a member of trunk ,set the trunk Id to zero*/
		}
		
		syslog_ax_eth_port_dbg("member = %d,trunkId = %d\n",member,trunkId);
	}	
    eth_g_index = eth_g_index + asic_board->asic_port_start_no-1;
	
	head->pid = getpid();
	head->count = 1;
    head->type = LOCAL_BOARD;
	head->object = SEM_MODULE;

    nl_msg->msgType = ASIC_ETHPORT_UPDATE_EVENT;
	nl_msg->msgData.portInfo.action_type = port_link_state;
	nl_msg->msgData.portInfo.slot_id = slot_no;
	nl_msg->msgData.portInfo.port_no = port_no;
	nl_msg->msgData.portInfo.eth_l_index = global_ethport[eth_g_index]->local_port_no-1;
	nl_msg->msgData.portInfo.devNum = g_dis_dev[0];
	nl_msg->msgData.portInfo.virportNum = portNum;
	nl_msg->msgData.portInfo.trunkId = trunkId;

    msgLen = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
	npd_netlink_send(msgbuf, msgLen);
	
	return 0;
}
int npd_asic_dynamic_trunk_notifier(unsigned int eth_g_index,unsigned int endis)
{	
	char msgbuf[512] = {0};
	int msgLen;
	int i = 0,index = 0;;
	unsigned long ret = 0,member = 0;
	unsigned short trunkId = 0;
	unsigned int port_link_state = 0,actdevNum = 0;
	unsigned char devnum = 0,portNum = 0;
	unsigned int slot_no = 0,port_no = 0;
	unsigned int dy_slot_no = 0,dy_port_no = 0;
    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg = (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));
	slot_no = CHASSIS_SLOT_INDEX2NO(SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index));
	port_no = ETH_LOCAL_INDEX2NO(SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index),ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index));
	
	if((BOARD_TYPE != BOARD_TYPE_AX71_CRSMU) && (BOARD_TYPE != BOARD_TYPE_AX81_SMU))
	{
		port_link_state = endis;
		ret = npd_get_devport_by_global_index(eth_g_index,&devnum,&portNum);
		if (ret != 0) 
		{
			syslog_ax_board_err("global_index %d convert to devPort error.\n",eth_g_index);
		}
		syslog_ax_board_dbg("devNum = %d ,portNum = %d\n",devnum,portNum);
		ret = nam_trunk_port_trunkId_get(devnum,portNum,&member,&trunkId);
		if(ret != 0)
		{
			syslog_ax_board_err("get trunk ID err !\n");
		}
		if((member == 0) || (trunkId > 110))    /* code optimize: no need check 0. zhangdi@autelan.com 3013-01-18 */
		{
			trunkId = 0;/*if port is not a member of trunk ,set the trunk Id to zero*/
		}
		

		if(DYNAMIC_TRUNK_MEMBER_IS_NOT_EMPTY(1))
		{
			for(index = 0;index < MAX_DYNAMIC_TRUNK_MEMBER;index++)
			{
				if((dynamic_trunk_member[1][index]>>16)&0xff)
				{/*is need add*/
						dy_slot_no = (dynamic_trunk_member[1][index]>>8)&0xff;/*get slot no*/
						dy_port_no = dynamic_trunk_member[1][index]&0xff;/*get port no*/
						if((dy_slot_no == slot_no) && (dy_port_no == port_no))
						{
							trunkId = 119;
							break;
						}
				}
			}
		}
		syslog_ax_eth_port_dbg("member = %d,trunkId = %d\n",member,trunkId);
	}	
    eth_g_index = eth_g_index + asic_board->asic_port_start_no-1;
	
	head->pid = getpid();
	head->count = 1;
    head->type = LOCAL_BOARD;
	head->object = SEM_MODULE;

    nl_msg->msgType = ASIC_DYNAMIC_TRUNK_NOTIFIER_EVENT;
	nl_msg->msgData.portInfo.action_type = port_link_state;
	nl_msg->msgData.portInfo.slot_id = slot_no;
	nl_msg->msgData.portInfo.port_no = port_no;
	nl_msg->msgData.portInfo.eth_l_index = global_ethport[eth_g_index]->local_port_no-1;
	nl_msg->msgData.portInfo.devNum = g_dis_dev[0];
	nl_msg->msgData.portInfo.virportNum = portNum;
	nl_msg->msgData.portInfo.trunkId = trunkId;

    msgLen = sizeof(nl_msg_head_t) + head->count*sizeof(netlink_msg_t);
	npd_netlink_send(msgbuf, msgLen);
	
	return 0;
}


int npd_asic_vlan_notifier(unsigned int vlanId, unsigned int slotId)
{	
	int ret = -1;
	int msgLen = 0;
	int i = 0;

	char msgbuf[512] = {0};
    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg = (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));

	head->pid = getpid();
	head->count = 1;
    head->type = CROSS_BOARD;
	head->object = COMMON_MODULE;

    /* init vlan netlink msg */
    nl_msg->msgType = ASIC_VLAN_NOTIFIER_ENVET;
	nl_msg->msgData.vlanInfo.vlan_id = vlanId;
    nl_msg->msgData.vlanInfo.slot_id = slotId;
	
	for(i = 0; i < CHASSIS_SLOT_COUNT; i++)
	{
		nl_msg->msgData.vlanInfo.bond_slot[i] = g_vlanlist[vlanId-1].bond_slot[i];
    	nl_msg->msgData.vlanInfo.untagPortBmp[i].high_bmp = g_vlanlist[vlanId-1].untagPortBmp[i].high_bmp;
    	nl_msg->msgData.vlanInfo.untagPortBmp[i].low_bmp = g_vlanlist[vlanId-1].untagPortBmp[i].low_bmp;
    	nl_msg->msgData.vlanInfo.tagPortBmp[i].high_bmp = g_vlanlist[vlanId-1].tagPortBmp[i].high_bmp;
    	nl_msg->msgData.vlanInfo.tagPortBmp[i].low_bmp = g_vlanlist[vlanId-1].tagPortBmp[i].low_bmp;
	}

	
	strcpy(nl_msg->msgData.vlanInfo.vlan_name, g_vlanlist[vlanId-1].vlanName);
	
    msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t);
	ret = npd_netlink_send(msgbuf, msgLen);	
	if(0!=ret)
	{
		syslog_ax_board_err("npd_asic_vlanId_notifier error !\n");
	}	
	return ret;
}


int npd_asic_vlan_sync(unsigned int act,unsigned int slot)
{	
	int ret = -1;
	int msgLen = 0;

	char msgbuf[512] = {0};
    nl_msg_head_t *head = (nl_msg_head_t *)msgbuf;
    netlink_msg_t *nl_msg = (netlink_msg_t *)(msgbuf + sizeof(nl_msg_head_t));

	head->pid = getpid();
	head->count = 1;
    head->type = CROSS_BOARD;
	head->object = SEM_MODULE;

    /* init vlan netlink msg */
    nl_msg->msgType = ASIC_VLAN_SYNC_ENVET;
	nl_msg->msgData.vlanInfo.action_type = act;
	nl_msg->msgData.vlanInfo.slot_id = slot;

    msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t);
	ret = npd_netlink_send(msgbuf, msgLen);
	if(0!=ret)
	{
		syslog_ax_board_dbg("npd_netlink_send error !\n");
	}	
	return ret;
}
extern int adptVirRxFd;

int npd_reset_intf_mac(void)
{
	struct if_cfg_struct pif;
    int i = 0;
	int fd = -1;
	unsigned char temp_buffer[3] = {0x00, 0x00, 0x00};
	unsigned char mac_addr[6];	
	char tmp_mac_buffer[13];
	int ret = -1;
	
    memset(&pif, 0, sizeof(struct if_cfg_struct));

	fd = open("/devinfo/local_mac", O_RDONLY);
	
    if (fd < 0) {
        syslog_ax_netlink_info("open /devinfo/local_mac fail\n");
        return -1;
    }

	memset(tmp_mac_buffer, 0x00, 13);
	ret = read(fd, tmp_mac_buffer, 12);
	syslog_ax_netlink_info("npd_set_intf_mac read length %d\n",ret);
	for (i=0; i<12; i+=2)
	{
		memcpy(temp_buffer, tmp_mac_buffer+i, 2);
		mac_addr[i/2] = (unsigned char)strtoul((char *)temp_buffer, 0, 16);
		syslog_ax_netlink_info("npd_get_intf_mac %x", mac_addr[i/2]);
	}

	close(fd);
	
    memcpy(pif.mac_addr , mac_addr, 6);
    if (!adptVirRxFd || adptVirRxFd < 0) {
        syslog_ax_intf_err("npd_intf_get_intf_mac::KAP fd %d invalided\n", adptVirRxFd);
        return INTERFACE_RETURN_CODE_FD_ERROR;
    }
    else {
        if (0 > ioctl(adptVirRxFd, KAPRESETMAC, &pif)) {
            if (ENODEV != errno) {
                syslog_ax_intf_err("KAP ioctl %#x get L3 mac fail,errno %d\n", KAPRESETMAC, errno);
                return INTERFACE_RETURN_CODE_IOCTL_ERROR;
            }
            else {
                return INTERFACE_RETURN_CODE_SUCCESS;
            }
        }
        else {
            return INTERFACE_RETURN_CODE_SUCCESS;
        }
    }
}
/**********************************************************************
 *  npd_init_second_stage
 *
 *  DESCRIPTION:
 *         Config for cscd port, and init the distributed vlan info.
 *
 *  INPUT:   None
 *  OUTPUT:  None
 *  RETURN:
 *          0--OK  
 *         -1--ERROR
 *
 *  COMMENTS:
 *          Called in npd_netlink.c  zhangdi@autelan.com 2012-03-23
 **********************************************************************/
int npd_init_second_stage(void)
{
	int ret = -1;
	ret = npd_reset_intf_mac();	
	if(0 != ret)
	{
		syslog_ax_netlink_info("npd_get_intf_mac error!\n");
	}
	else
	{
		syslog_ax_netlink_info("npd_get_intf_mac OK!\n");
	}
    /* Reinit the local board MAC, generate from local board sn. zhangdi@autelan.com 2012-05-22 */
	ret = get_board_mac_from_sn(init_mac);
    if (ret==NPD_FAIL)
    {
        syslog_ax_netlink_info("get_board_mac_from_sn failed !\n");
    }
	else
	{
        syslog_ax_netlink_info("get_board_mac_from_sn: %s OK.\n",productinfo.sysinfo.basemac);
	}
	
    /* Init the master info for macro IS_MASTER_NPD & IS_ACTIVE_MASTER_NPD */
	board_info.is_active_master = get_num_from_file("/dbm/local_board/is_active_master");
	board_info.is_master = get_num_from_file("/dbm/local_board/is_master");
	if((board_info.is_active_master<0)||(board_info.is_master<0))
    {
		syslog_ax_netlink_info("get_num_from_file of master info error !\n");
		return -1;
    }

   	if(1 == IS_MASTER_NPD)    /* if local board is Master, update the default vlan info. */
   	{		    	
		/* 1.Disable the cscd port of Stanby-MCB on AX8610 & AX8606, need not on AX8603. */
		if((PRODUCT_TYPE==AUTELAN_PRODUCT_AX8610)||(PRODUCT_TYPE==AUTELAN_PRODUCT_AX8606)\
			||(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8800))
    	{
        	if(0 == IS_ACTIVE_MASTER_NPD)    /* if local board is not Active-MCB */
        	{
           		ret = npd_disable_cscd_port();
                if (0 != ret)
                {
                    syslog_ax_board_dbg("disable cscd port on Standby-MCB error !\n");
    				return ret;
                }
    			else
    			{
    		        syslog_ax_board_dbg("disable cscd port on Standby-MCB OK !\n");
    			}
    		}
    	}
		else    /* AX7605i & AX8603 */
		{
			/* do nothing */
		}

    	/* 2.Init the g_vlanlist[] of other board on SMU(active & standby). */									
        ret = npd_get_default_vlan_smu();
        if (0 != ret)
        {
            syslog_ax_board_dbg("init default vlan on MCB error !\n");
    		return ret;
        }		
   	}
	else    /* on not Master board */
	{
    	/* We can set the different config(exp: cscd) for different board on stage 2. */
    	#if 0
    	if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
    	{
    		/* for later use */
    	}
    	#endif
        syslog_ax_board_dbg("I am not Master, my stage-2 config is NULL!\n");		
	}
    npd_syslog_info("======  npd_init_second_stage OK! ====\n");	
	return ret;
}

/**********************************************************************
 *  npd_update_slotx_vlan_info
 *
 *  DESCRIPTION:
 *         When board insert/remove, update the vlan info on board.
 *
 *  INPUT:   slot  -  which slot the board on
 *           state -  inser or remove
 *  OUTPUT:  none
 *  RETURN:
 *          0--OK  
 *         -1--ERROR
 *
 *  COMMENTS:
 *          Called in npd_netlink.c  zhangdi@autelan.com 2012-03-23
 **********************************************************************/
int npd_update_slotx_vlan_info(int slot, int state)
{
	int i = 0, ret =0;
    int vid_index = 0;	
	int asic_port_start_no = 0;
	int asic_port_cnt = 0;
	unsigned int ifIndex = ~0UI;	
	char path_start_no[64];
	char path_cnt[64];	
	char vename[21] = {0};
	
	sprintf(path_start_no, "/dbm/product/slot/slot%d/asic_start_no", slot);
	sprintf(path_cnt, "/dbm/product/slot/slot%d/asic_port_num", slot);


    /* If insert, add the asic port on the board to default-vlan on SMU */
	if(state == BOARD_INSERTED)
    {
		/* get the new board info of aisc */
    	asic_port_start_no = get_num_from_file(path_start_no);
    	asic_port_cnt = get_num_from_file(path_cnt);
        if((asic_port_start_no<0)||(asic_port_start_no<0))
        {
    		syslog_ax_netlink_info("get_num_from_file of slot %d error !\n",slot);
    		return -1;
        }
		
    	for (i= asic_port_start_no; i <= (asic_port_cnt + asic_port_start_no -1); i++ )
    	{		
    		if(i<=32)
    		{
    		    g_vlanlist[0].untagPortBmp[slot-1].low_bmp |= (unsigned int)(1<<(i-1));
			}
    		else
    		{
    		    g_vlanlist[0].untagPortBmp[slot-1].high_bmp |= (unsigned int)(1<<(i-33));				
    		}
    	}
	}
    /* If remove, delete the asic port info on all active vlan on SMU */
	else if(state == BOARD_REMOVED)
	{
		for(vid_index =0; vid_index<=4095; vid_index++)
		{
			/* Check if the vlan valid */
            if(VLAN_VALID == g_vlanlist[vid_index].vlanStat)
            {
        		syslog_ax_board_dbg("Update info of vlan %d :\n",vid_index+1);
				/* 1. Clean the port of the remove slot on MCB */
				if(IS_MASTER_NPD==1)
				{
        		    g_vlanlist[vid_index].untagPortBmp[slot-1].low_bmp = 0;
        		    g_vlanlist[vid_index].untagPortBmp[slot-1].high_bmp = 0;
    				g_vlanlist[vid_index].tagPortBmp[slot-1].low_bmp = 0;
        		    g_vlanlist[vid_index].tagPortBmp[slot-1].high_bmp = 0;
            		syslog_ax_board_dbg("MCB remove port of slot %d ok.\n",slot);						
				}
				
				/* 2. Remove the bond info to the remove slot on SMU & other board */
                if(g_vlanlist[vid_index].bond_slot[slot-1] != 0)
                {
					/* On active master, set ve sub-intf down */
					if(IS_ACTIVE_MASTER_NPD==1)
					{
						for(i=0;i<MAX_CPU_PORT_NUM;i++)
						{
							if(((g_vlanlist[vid_index].bond_slot[slot-1])&(unsigned int)(1<<i))!=0)
							{
		    					/* Check if interface exist, then make the interface down */
		               			sprintf(vename,"ve%02d%c%d.%d",slot,((i/8)+1==1)?'f':'s',((i%8)+1),vid_index+1);

		        			    if(0 == npd_intf_ifindex_get_by_ifname(vename, &ifIndex))  /* get ifindex OK */
		        			    {
		    						syslog_ax_vlan_err("Active-MCB Get %s ifindex: %d OK\n",vename,ifIndex);
		                            ret = npd_intf_set_intf_state(vename,0);    /* set ve sub intf down */
		                            if(ret != 0)
		                            {
		                                syslog_ax_vlan_err("Active-MCB set %s DOWN error !\n",vename);
		                            }
		    						else
		    						{
		        					    syslog_ax_vlan_dbg("Active-MCB set %s DOWN OK !\n",vename);
										continue;
		    						}
									usleep(500);  /* wait for a while for set intf state */
		    					}
								else
								{
		    						syslog_ax_vlan_err("Active-MCB Get %s ifindex error !\n",vename);							
								}
							}
						}
					}
					
					/* Set the bond info to null on all other board */
                	g_vlanlist[vid_index].bond_slot[slot-1] = 0;
				    syslog_ax_vlan_dbg("Clean the bond info of vlan %d OK !\n",vid_index+1);					
                }											
            }
		}			
	}

    ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
	if( ret!=0 )
    {
        syslog_ax_board_err("msync shm_vlan failed \n" );   
    }

	return 0;
}

int npd_update_smu_trunk_info()
{
	int ret = 0;
	syslog_ax_board_dbg("Init ax81_smu trunk port status !\n");
	if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8610)
	{
		ret = ax81_smu_trunk_port_status_init_for_lion0(0);
		if(ret != 0)
		{
			syslog_ax_board_dbg("ax81_smu_trunk_port_status_init dev 0 err !\n");
		}
		ret = ax81_smu_trunk_port_status_init_for_lion1(1);
		if(ret != 0)
		{
			syslog_ax_board_dbg("ax81_smu_trunk_port_status_init dev 1 err !\n");
		}
	}
	else if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8606)
	{
		ret = ax81_smu_trunk_port_status_init_for_8606(0);
		if(ret != 0)
		{
			syslog_ax_board_dbg("ax81_smu_trunk_port_status_init_for_8606 dev 0 err !\n");
		}
	}
	else if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8800)
	{
		ret = ax81_smu_trunk_port_status_init_for_8800_lion0(0);
		if(ret != 0)
		{
			syslog_ax_board_dbg("ax81_smu_trunk_port_status_init dev 0 err !\n");
		}
		ret = ax81_smu_trunk_port_status_init_for_8800_lion1(1);
		if(ret != 0)
		{
			syslog_ax_board_dbg("ax81_smu_trunk_port_status_init dev 1 err !\n");
		}
	}
	return ret;
}
#ifdef __cplusplus
}
#endif
