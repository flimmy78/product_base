#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/types.h> 
#include <linux/fs.h>
#include <linux/slab.h>
#include <net/netlink.h>
#include <net/sock.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#include <asm/octeon/cvmx.h>
#include "asm/octeon/cvmx-app-init.h"
#include "ksem.h"

unsigned long flag_bitmap = 0;
int preState = 0;
extern struct sock *nl_sk_ksem;
extern unsigned long product_id;
extern unsigned long board_id;
extern int ksem_debug;

DEFINE_RWLOCK(octeon_gpio_cfg_rwlock);
DEFINE_RWLOCK(cpld_int_mask_rwlock);

/**
 * Do a twsi read from a 7 bit device address using an (optional) internal address.
 * Up to 8 bytes can be read at a time.
 *
 * @param twsi_id   which Octeon TWSI bus to use
 * @param dev_addr  Device address (7 bit)
 * @param internal_addr
 *                  Internal address.  Can be 0, 1 or 2 bytes in width
 * @param num_bytes Number of data bytes to read
 * @param ia_width_bytes
 *                  Internal address size in bytes (0, 1, or 2)
 * @param data      Pointer argument where the read data is returned.
 *
 * @return read data returned in 'data' argument
 *         Number of bytes read on success
 *         -1 on failure
 */
int cvmx_twsix_read_ia(int twsi_id, uint8_t dev_addr, uint16_t internal_addr, int num_bytes, int ia_width_bytes, uint64_t *data)
{
	cvmx_mio_twsx_sw_twsi_t sw_twsi_val;
	cvmx_mio_twsx_sw_twsi_ext_t twsi_ext;
	int timeout = 10;
	
	if (num_bytes < 1 || num_bytes > 8 || !data || ia_width_bytes < 0 || ia_width_bytes > 2)
		return -1;
	twsi_ext.u64 = 0;
	sw_twsi_val.u64 = 0;
	sw_twsi_val.s.v = 1;
	sw_twsi_val.s.r = 1;
	sw_twsi_val.s.sovr = 1;
	sw_twsi_val.s.size = num_bytes - 1;
	sw_twsi_val.s.a = dev_addr;

	if (ia_width_bytes > 0) {
		sw_twsi_val.s.op = 1;
		sw_twsi_val.s.ia = (internal_addr >> 3) & 0x1f;
		sw_twsi_val.s.eop_ia = internal_addr & 0x7;
	}
	if (ia_width_bytes == 2) {
		sw_twsi_val.s.eia = 1;
		twsi_ext.s.ia = internal_addr >> 8;
		cvmx_write_csr(CVMX_MIO_TWSX_SW_TWSI_EXT(twsi_id), twsi_ext.u64);
	}
	
    cvmx_write_csr(CVMX_MIO_TWSX_SW_TWSI(twsi_id), sw_twsi_val.u64);
	
	do {
		udelay(1000);
		if (timeout <= 0){		
			ksemDBG("Timeout : waiting for sw_twsi_val.s.v to be cleared\n");
		}          
	}while (((cvmx_mio_twsx_sw_twsi_t)(sw_twsi_val.u64 = cvmx_read_csr(CVMX_MIO_TWSX_SW_TWSI(twsi_id)))).s.v && timeout--);

	if (!sw_twsi_val.s.r)
	{
		return -1;
	}

	*data = (sw_twsi_val.s.d & (0xFFFFFFFF >> (32 - num_bytes*8)));
	if (num_bytes > 4) {
		twsi_ext.u64 = cvmx_read_csr(CVMX_MIO_TWSX_SW_TWSI_EXT(twsi_id));
		*data |= ((unsigned long long)(twsi_ext.s.d & (0xFFFFFFFF >> (32 - num_bytes*8))) << 32);
	}
	return num_bytes;	
}


/* code optmize: Unsigned compared against 0 (NO_EFFECT). wangchao@autelan.com 2013-01-17 */
/*uint64_t*/
int i2c_read8(int twsi_id, uint8_t dev_addr, uint16_t internal_addr)
{
	uint64_t data;
	int ia_width_bytes;
	
	if(internal_addr>>8)
		ia_width_bytes = 2;
	else
		ia_width_bytes = 1;
		
    if(cvmx_twsix_read_ia(twsi_id, dev_addr, internal_addr, 1, ia_width_bytes, &data) < 0)
		return -1;
	return data & 0xff;
}

void ksem_netlink_rcv(struct sk_buff *skb)
{
	return ;
}

/**
 * send netlink message to user-space
 *
 * @param sendBuf: message content
 * @param msgLen : message lenth
 * modify:      <jialh@autelan.com>
 */
void sem_kern_netlink_notifier(char *sendBuf, int msgLen)     
{   
    struct sk_buff *skb;   
    struct nlmsghdr *nlh;  
	int size = NLMSG_SPACE(msgLen);
	
    if(!sendBuf || !nl_sk_ksem){   
        return;   
    }   

    /* Allocate a new sk_buffer */  
    skb = alloc_skb(size, GFP_ATOMIC);   
    if(!skb){   
        ksemERR("netlink: alloc_skb Error.\n");   
        return;   
    }   

    /* Initialize the header of netlink message */  
    nlh = NLMSG_PUT(skb, 0, 0, 0, size-sizeof(*nlh));   
  
    NETLINK_CB(skb).pid = 0;   
    NETLINK_CB(skb).dst_group = 1;   
    
    memcpy(NLMSG_DATA(nlh), sendBuf, msgLen);
	
    /* send message by broadcast */  
    netlink_broadcast(nl_sk_ksem, skb, 0, 1, GFP_ATOMIC);   
	
    return; 
	
nlmsg_failure: 
    if(skb)
        kfree_skb(skb);
    return;
}

void check_on_board_state_for_ax7605i()
{
	cvmx_gpio_rx_dat_t gpio_rx_data;
	unsigned long board_on_mask;
	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;	
	int msgLen = 0;
	char sendBuf[512] = {0};
	unsigned long local_board_state, master_slot_id;

	head = (nl_msg_head_t*)sendBuf;
	nl_msg = (netlink_msg_t*)(sendBuf + sizeof(nl_msg_head_t));

	head->type = OVERALL_UNIT;
	head->object = COMMON_MODULE;
	head->count = 1;
	head->pid = 0;
	
    msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t); 	
	gpio_rx_data.u64 = cvmx_read_csr(GPIO_RX_DAT);
	board_on_mask = gpio_rx_data.s.dat & 7;
    local_board_state = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_LOCAL_BOARD_STATE_REG);
	master_slot_id    = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_MASTER_SLOT_ID_REG);
/*
	ksemDBG("board_on_mask     = %#lx\n", board_on_mask);
    ksemDBG("local_board_state = %#lx\n", local_board_state);
	ksemDBG("master_slot_id    = %#lx\n", master_slot_id);
*/	
	/* Focus on remote master board state */
	if(test_bit(0, &board_on_mask)) {
		if(!test_bit(AX7605I_REMOTE_STATE, &flag_bitmap)) {
			#if 0
    		if((local_board_state&1) == preState) {
        		nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
				nl_msg->msgData.boardInfo.action_type = BOARD_REMOVED;
    			if(test_bit(0, &master_slot_id))
    			    nl_msg->msgData.boardInfo.slotid = 1;
    			else
    				nl_msg->msgData.boardInfo.slotid = 2;
				
				ksemERR("Remote Master Board Removed\n");
				sem_kern_netlink_notifier(sendBuf, msgLen);
    		}else {
    		    preState = local_board_state&1;
    		    nl_msg->msgType = ACTIVE_STDBY_SWITCH_EVENT;
    			nl_msg->msgData.swInfo.action_type = HOTPLUG_MODE;
    			if(test_bit(0, &master_slot_id))
    			    nl_msg->msgData.swInfo.active_slot_id = 2;
    			else
    				nl_msg->msgData.swInfo.active_slot_id = 1;
    			
    			ksemERR("Active_MCB and Standby_MCB switch. Now active_master_slot_id is [%d]\n", nl_msg->msgData.boardInfo.slotid);
                sem_kern_netlink_notifier(sendBuf, msgLen);
			}
			#endif
			/* On product AX7605i the local board active-standby state register is not useable */

			nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
			nl_msg->msgData.boardInfo.action_type = BOARD_REMOVED;
			if(test_bit(0, &master_slot_id))
			    nl_msg->msgData.boardInfo.slotid = 1;
			else
				nl_msg->msgData.boardInfo.slotid = 2;
			
			ksemERR("Remote Master Board Removed\n");
			sem_kern_netlink_notifier(sendBuf, msgLen);
	
			flag_bitmap |= 1 << AX7605I_REMOTE_STATE;	
		}
	}else {  
		if(test_bit(AX7605I_REMOTE_STATE, &flag_bitmap)) {
			ksemERR("Remote Master Board Inserted\n");
			
			nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
			nl_msg->msgData.boardInfo.action_type = BOARD_INSERTED;
			if(test_bit(0, &master_slot_id))
			    nl_msg->msgData.boardInfo.slotid = 1;
			else
				nl_msg->msgData.boardInfo.slotid = 2;
			
			sem_kern_netlink_notifier(sendBuf, msgLen);
			
			flag_bitmap &= ~(1 << AX7605I_REMOTE_STATE);	
		}
	}
	
    /* Focus on slave board state */
	if(test_bit(2, &board_on_mask)) {
		if(!test_bit(AX7605I_SLAVE_STATE, &flag_bitmap)) {
			ksemERR("Switch Board Removed\n");
			
			nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
			nl_msg->msgData.boardInfo.action_type = BOARD_REMOVED;
			nl_msg->msgData.boardInfo.slotid = 3;
			sem_kern_netlink_notifier(sendBuf, msgLen);
			
			flag_bitmap |= 1 << AX7605I_SLAVE_STATE;	
		}
	}else {  
		if(test_bit(AX7605I_SLAVE_STATE, &flag_bitmap)) {
			ksemERR("Switch Board Inserted\n");
			
			nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
			nl_msg->msgData.boardInfo.action_type = BOARD_INSERTED;
			nl_msg->msgData.boardInfo.slotid = 3;
            sem_kern_netlink_notifier(sendBuf, msgLen);
			
			flag_bitmap &= ~(1 << AX7605I_SLAVE_STATE);	
		}
	}
}


void check_on_hwState_for_ax7605i()
{
	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;	
	unsigned long hwState;
	int msgLen = 0;
	char sendBuf[512] = {0};
	
	hwState = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_HW_STATE_REG);
    /*
	ksemDBG("hwState    = %#lx\n", hwState);
	*/
	head = (nl_msg_head_t*)sendBuf;
	nl_msg = (netlink_msg_t*)(sendBuf + sizeof(nl_msg_head_t));

    head->type = OVERALL_UNIT;
    head->object = SEM_MODULE;
	head->count = 1;
	head->pid = 0;
	
    msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t); 
	
	/* Check on hardware state */
	if(!test_bit(3, &hwState)) {
		/* check on power supply voltage */
		if(!test_bit(AX7605I_VCC_STATE, &flag_bitmap)) {
			ksemERR("VCC Alarm\n");
			nl_msg->msgData.powerInfo.action_type = POWER_REMOVED;					
			sem_kern_netlink_notifier(sendBuf, msgLen);
			flag_bitmap |= 1 << AX7605I_VCC_STATE;
		}
	}else {
	    flag_bitmap &= ~(1 << AX7605I_VCC_STATE);
	}
	
	/* Focus on whether the power module 1 is OK */
	if(!test_bit(1, &hwState)) {
		if(!test_bit(AX7605I_POWER1_STATE, &flag_bitmap)) {
		    ksemERR("Power Module 1 Off\n");
			
			nl_msg->msgType = POWER_STATE_NOTIFIER_EVENT;
			nl_msg->msgData.powerInfo.power_id = 1;
			nl_msg->msgData.powerInfo.action_type = POWER_OFF;					
			sem_kern_netlink_notifier(sendBuf, msgLen);
			flag_bitmap |= 1 << AX7605I_POWER1_STATE;
		}
	}else {
	    if(test_bit(AX7605I_POWER1_STATE, &flag_bitmap)) {
    	    ksemERR("Power Module 1 On\n");
    		
    		nl_msg->msgType = POWER_STATE_NOTIFIER_EVENT;
    		nl_msg->msgData.powerInfo.power_id = 1;
    		nl_msg->msgData.powerInfo.action_type = POWER_ON;					
    		sem_kern_netlink_notifier(sendBuf, msgLen);
    	    flag_bitmap &= ~(1 << AX7605I_POWER1_STATE);
	    }
	}
	
    /* Focus on whether the power module 2 is OK */
	if(!test_bit(2, &hwState)) {
		if(!test_bit(AX7605I_POWER2_STATE, &flag_bitmap)) {
			ksemERR("Power Module 2 Off\n");
			
			nl_msg->msgType = POWER_STATE_NOTIFIER_EVENT;
			nl_msg->msgData.powerInfo.power_id = 2;
			nl_msg->msgData.powerInfo.action_type = POWER_OFF;	
            sem_kern_netlink_notifier(sendBuf, msgLen);
			flag_bitmap |= 1 << AX7605I_POWER2_STATE;
		}
	}else {
	    if(test_bit(AX7605I_POWER2_STATE, &flag_bitmap)) {
    		ksemERR("Power Module 2 On\n");
    		
    		nl_msg->msgType = POWER_STATE_NOTIFIER_EVENT;
    		nl_msg->msgData.powerInfo.power_id = 2;
    		nl_msg->msgData.powerInfo.action_type = POWER_ON;	
            sem_kern_netlink_notifier(sendBuf, msgLen);
    	    flag_bitmap &= ~(1 << AX7605I_POWER2_STATE);
	    }
	}
	
	/* Focus on whether the fan module is OK */		
	if(test_bit(4, &hwState)) {
		if(!test_bit(AX7605I_FAN_STATE, &flag_bitmap)) {
			ksemERR("Fan Module Alarm\n");
			flag_bitmap |= 1 << AX7605I_FAN_STATE;
		}
	}else {
	    flag_bitmap &= ~(1 << AX7605I_FAN_STATE);
	}    
}

/*
 * check on Slave board state: inserted or removed
 * @param intState1 : from interrupt state register 1 on AX81_CRSMU
 */
void check_on_intState1_for_ax86xx(unsigned long intState1)
{
	int i;
	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;
	int msgLen = 0;
	char sendBuf[512] = {0};
	unsigned long slaveState;
	int effectiveSlotNum = 0;
	
	if (product_id == PRODUCT_AX8800)
	{
		effectiveSlotNum = 12;
	}
	else if (product_id == PRODUCT_AX8610)
	{
		effectiveSlotNum = 8;
	}
	else if (product_id == PRODUCT_AX8606)
	{
		effectiveSlotNum = 4;
	}
	else if (product_id == PRODUCT_AX8603)
	{	
		;
	}
	else
	{
		return ;
	}

	head = (nl_msg_head_t*)sendBuf;
	nl_msg = (netlink_msg_t*)(sendBuf + sizeof(nl_msg_head_t));

	head->type = OVERALL_UNIT;
	head->object = COMMON_MODULE;
	head->count = 1;
	head->pid = 0;
	
    msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t); 

    slaveState  = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_TYPE_REG1);
	ksemDBG("slaveState = %#lx\n", slaveState);
	
	nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
	if (product_id == PRODUCT_AX8603)
	{
		nl_msg->msgData.boardInfo.slotid = 3;
		if(test_bit(0, &slaveState)) {
			ksemERR("Slot [3] Borad Removed\n");
			nl_msg->msgData.boardInfo.action_type = BOARD_REMOVED;	
		}else {
			ksemERR("Slot [3] Borad Inserted\n");
			nl_msg->msgData.boardInfo.action_type = BOARD_INSERTED;
		}
		sem_kern_netlink_notifier(sendBuf, msgLen); 
		return ;
	}
    for(i = 0; i < effectiveSlotNum; i++) 
	{
        if(test_bit(i, &intState1)) {
            if(i < effectiveSlotNum/2) {
				nl_msg->msgData.boardInfo.slotid = i+1;
			    if(test_bit(i, &slaveState)) {
					ksemERR("Slot [%d] Borad Removed\n", i+1);
					nl_msg->msgData.boardInfo.action_type = BOARD_REMOVED;	
			    }else {
			        ksemERR("Slot [%d] Borad Inserted\n", i+1);
					nl_msg->msgData.boardInfo.action_type = BOARD_INSERTED;
			    }
				sem_kern_netlink_notifier(sendBuf, msgLen);	
            }else {
                nl_msg->msgData.boardInfo.slotid = i+3;
				if(test_bit(i, &slaveState)) {
					ksemERR("Slot [%d] Borad Removed\n", i+3);
				    nl_msg->msgData.boardInfo.action_type = BOARD_REMOVED;
				}else {
				    ksemERR("Slot [%d] Borad Inserted\n", i+3);
					nl_msg->msgData.boardInfo.action_type = BOARD_INSERTED;
				}
				sem_kern_netlink_notifier(sendBuf, msgLen);	
			}		
		}
	}	
	return ;
}

/*
 * 1.check on master board state: inserted or removed
 * 2.check on fan state
 * @param intState2 : from interrupt state register 2 on AX81_CRSMU
 */
void check_on_intState2_for_ax86xx(unsigned long intState2)
{
	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;
	int msgLen = 0;
	char sendBuf[512] = {0};
    unsigned long remoteState, fanState;
	unsigned long master_slot_id, local_board_state;
	int default_active_slot_id, default_standby_slot_id;

	if (product_id == PRODUCT_AX8800)
	{
		default_active_slot_id = 7;
		default_standby_slot_id = 8;
	}
	else if (product_id == PRODUCT_AX8610)
	{
		default_active_slot_id = 5;
		default_standby_slot_id = 6;
	}
	else if (product_id == PRODUCT_AX8606)
	{
		default_active_slot_id = 3;
		default_standby_slot_id = 4;
	}
	else if (product_id == PRODUCT_AX8603)
	{
		default_active_slot_id = 1;
		default_standby_slot_id = 2;
	}
	else
	{
		return ;
	}

	head = (nl_msg_head_t*)sendBuf;
	nl_msg = (netlink_msg_t*)(sendBuf + sizeof(nl_msg_head_t));

	head->type = OVERALL_UNIT;
	head->object = COMMON_MODULE;
	head->count = 1;
	head->pid = 0;
	
    msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t); 

	remoteState = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_REMOTE_MASTER_STATE_REG);
	fanState    = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_TYPE_REG2);
	local_board_state = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_LOCAL_BOARD_STATE_REG);
	master_slot_id    = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_MASTER_SLOT_ID_REG);	

	ksemDBG("remoteState       = %#lx\n", remoteState);
    ksemDBG("local_board_state = %#lx\n", local_board_state);
	ksemDBG("master_slot_id    = %#lx\n", master_slot_id);
	ksemDBG("fanState          = %#lx\n", fanState);

	if (product_id == PRODUCT_AX8610 || product_id == PRODUCT_AX8603 || product_id == PRODUCT_AX8800){
		if(test_bit(0, &intState2)) {
			#if 0
			if((local_board_state&1) == preState) {
	    		nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
				if(test_bit(0, &master_slot_id))
				    nl_msg->msgData.boardInfo.slotid = default_active_slot_id;
				else
					nl_msg->msgData.boardInfo.slotid = default_standby_slot_id;	
	            if(test_bit(0, &remoteState)){
					nl_msg->msgData.boardInfo.action_type = BOARD_REMOVED;
					ksemERR("Slot [%d] Borad Removed\n", nl_msg->msgData.boardInfo.slotid);
	            }else {
					nl_msg->msgData.boardInfo.action_type = BOARD_INSERTED;
					ksemERR("Slot [%d] Borad Inserted\n", nl_msg->msgData.boardInfo.slotid);
	            }
			}else {
			    preState = local_board_state&1;
			    nl_msg->msgType = ACTIVE_STDBY_SWITCH_EVENT;
				nl_msg->msgData.swInfo.action_type = HOTPLUG_MODE;
				if(test_bit(0, &master_slot_id))
				    nl_msg->msgData.swInfo.active_slot_id = default_standby_slot_id;
				else
					nl_msg->msgData.swInfo.active_slot_id = default_active_slot_id;
				
				ksemERR("Active_MCB and Standby_MCB switch. Now active_master_slot_id is [%d]\n", nl_msg->msgData.boardInfo.slotid);
			}
			sem_kern_netlink_notifier(sendBuf, msgLen);	
			#endif
			nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
			if(test_bit(0, &master_slot_id))
				nl_msg->msgData.boardInfo.slotid = default_active_slot_id;
			else
				nl_msg->msgData.boardInfo.slotid = default_standby_slot_id;
			if(test_bit(0, &remoteState)){
				nl_msg->msgData.boardInfo.action_type = BOARD_REMOVED;
				ksemERR("Slot [%d] Borad Removed\n", nl_msg->msgData.boardInfo.slotid);
			}else {
				nl_msg->msgData.boardInfo.action_type = BOARD_INSERTED;
				ksemERR("Slot [%d] Borad Inserted\n", nl_msg->msgData.boardInfo.slotid);
			}
			sem_kern_netlink_notifier(sendBuf, msgLen);
		}
	}
	else if (product_id == PRODUCT_AX8606){
		if(test_bit(0, &intState2)){
			nl_msg->msgType = BOARD_STATE_NOTIFIER_EVENT;
			if(test_bit(0, &master_slot_id))
			    nl_msg->msgData.boardInfo.slotid = default_active_slot_id;
			else
				nl_msg->msgData.boardInfo.slotid = default_standby_slot_id;	
	        if(test_bit(0, &remoteState)){
				nl_msg->msgData.boardInfo.action_type = BOARD_REMOVED;
				ksemERR("Slot [%d] Borad Removed\n", nl_msg->msgData.boardInfo.slotid);
	        }else {
				nl_msg->msgData.boardInfo.action_type = BOARD_INSERTED;
				ksemERR("Slot [%d] Borad Inserted\n", nl_msg->msgData.boardInfo.slotid);
	        }
		}
		sem_kern_netlink_notifier(sendBuf, msgLen);	
	}
	
	if(test_bit(1, &intState2)) {
        head->type = LOCAL_BOARD;
		nl_msg->msgType = FAN_STATE_NOTIFIER_EVENT;
		
		if(test_bit(1, &fanState)) {
			ksemERR("Fan Module Removed\n");
			nl_msg->msgData.fanInfo.action_type = FAN_REMOVED;
        }else {
			ksemERR("Fan Module Inserted\n");
			nl_msg->msgData.fanInfo.action_type = FAN_INSERTED;
        }
		sem_kern_netlink_notifier(sendBuf, msgLen);	
	}
	
	if(test_bit(2, &intState2)) {
	    head->type = LOCAL_BOARD;
	    nl_msg->msgType = FAN_STATE_NOTIFIER_EVENT;
		
		if(!test_bit(2, &fanState)) {
			ksemERR("Fan Module Alarm\n");
			nl_msg->msgData.fanInfo.action_type = FAN_ALARM;
			sem_kern_netlink_notifier(sendBuf, msgLen);	
        }else {
			/*ksemERR("Fan Module Normal\n");*/
			nl_msg->msgData.fanInfo.action_type = FAN_NORMAL;
        }          
	}	
}

/*
 * 1.check on power module state: inserted or removed
 * @param intState3 : from interrupt state register 3 on AX81_CRSMU
 */
void check_on_intState3_for_ax86xx(unsigned long intState3)
{
	int i;
	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;
	int msgLen = 0;
	char sendBuf[512] = {0};
	unsigned long powerState;
	int power_no = 4;

	if (product_id == PRODUCT_AX8603)
		power_no = 2;
	head = (nl_msg_head_t*)sendBuf;
	nl_msg = (netlink_msg_t*)(sendBuf + sizeof(nl_msg_head_t));

	head->type = LOCAL_BOARD;
	head->object = SEM_MODULE;
	head->count = 1;
	head->pid = 0;
	
    msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t); 

    powerState  = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_TYPE_REG3);
	ksemDBG("powerState = %#lx\n", powerState);
	nl_msg->msgType = POWER_STATE_NOTIFIER_EVENT;
    for(i = 0; i < power_no; i++)
    {
		nl_msg->msgData.powerInfo.power_id = i + 1;
        if(test_bit(i, &intState3)) {
			if(test_bit(i, &powerState)) {
                /*WangChao change: The AX8603 is contraily to others products*/
				if(product_id == PRODUCT_AX8603){
                   	ksemERR("Power Module [%d] Inserted\n", i + 1);
    			    nl_msg->msgData.powerInfo.action_type = POWER_INSERTED; 
				}else {
    			    ksemERR("Power Module [%d] Removed\n", i + 1);
    		    	nl_msg->msgData.powerInfo.action_type = POWER_REMOVED;
				}
				sem_kern_netlink_notifier(sendBuf, msgLen);	
				
            }else {
                /*WangChao change: The AX8603 is contraily to others products*/
				if(product_id == PRODUCT_AX8603){
    		    	ksemERR("Power Module [%d] Removed\n", i + 1);
    		    	nl_msg->msgData.powerInfo.action_type = POWER_REMOVED;
				}else {
    		    	ksemERR("Power Module [%d] Inserted\n", i + 1);
    		    	nl_msg->msgData.powerInfo.action_type = POWER_INSERTED;
				}
				sem_kern_netlink_notifier(sendBuf, msgLen);	
            }
        }
	}	
}

/*
 * check on power state: power off or power on
 */
void check_on_power_down_for_ax86xx(void)
{
	/*uint64_t*/
	long  val;	  /* code optmize: Unsigned compared against 0 (NO_EFFECT). wangchao@autelan.com 2013-01-17 */
	nl_msg_head_t *head = NULL;
	netlink_msg_t *nl_msg = NULL;
	int msgLen = 0;
	char sendBuf[512] = {0};
	unsigned long powerState;
	uint8_t dev_addr;
	int i, flag;

	head = (nl_msg_head_t*)sendBuf;
	nl_msg = (netlink_msg_t*)(sendBuf + sizeof(nl_msg_head_t));
	
	head->type = LOCAL_BOARD;
	head->object = SEM_MODULE;
	head->count = 1;
	head->pid = 0;
	
    msgLen = sizeof(nl_msg_head_t) + sizeof(netlink_msg_t); 
	
    powerState  = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_TYPE_REG3);
	dev_addr = I2C_POWER1_STATE_ADDR;
	flag = AX8610_POWER1_STATE;

	nl_msg->msgType = POWER_STATE_NOTIFIER_EVENT;
	for(i = 0; i < 4; i++,dev_addr++,flag++)
	{
        if(!test_bit(i, &powerState)) {
			/* Check on power supply state from eeprom, addr: 0xD0 */
            val = i2c_read8(1, dev_addr , I2C_POWER_STATE_REG);
			if(val < 0) {
				ksemDBG("Error read power module eeprom\n");/*avoid hardware problem temporarily*/
				return;
			}
			
			nl_msg->msgData.powerInfo.power_id = i + 1;
			
			/* Bit0 : 12V_OK, Bit1: 3V3SB_OK */
        	if(!test_bit(0, (unsigned long*)&val) || !test_bit(1, (unsigned long*)&val)) {
				if(!test_bit(flag, &flag_bitmap)) {
                    ksemERR("Power Module [%d] Off\n", i+1);
            		nl_msg->msgData.powerInfo.action_type = POWER_OFF;
            		sem_kern_netlink_notifier(sendBuf, msgLen);	
					flag_bitmap |= 1 << flag;
				}
        	}else {
        	    if(test_bit(flag, &flag_bitmap)) {
                    ksemERR("Power Module [%d] On\n", i+1);
            		nl_msg->msgData.powerInfo.action_type = POWER_ON;
            		sem_kern_netlink_notifier(sendBuf, msgLen);	
            	    flag_bitmap &= ~(1 << flag);
        	    }
        	}
        }
	}	
}

void init_board(void)
{
	unsigned long flags;
	unsigned long local_board_state;
	//cvmx_gpio_bit_cfgx_t gpio_cfg;
	
	switch(product_id)
	{
        case PRODUCT_AX7605I:
			if(board_id == AX71_CRSMU)
			{
	            local_board_state = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_LOCAL_BOARD_STATE_REG);
	        	preState = local_board_state&1;	
			}		
			break;
		case PRODUCT_AX8610:
			if(board_id == AX81_CRSMU || board_id == AX81_CRSMUE)
			{
				#if 0
            	read_lock_irqsave(&octeon_gpio_cfg_rwlock, flags);
                gpio_cfg.u64 = 0;
                gpio_cfg.u64 = cvmx_read64_uint64(GPIO_BIT_CFG5);
            	gpio_cfg.s.int_type = 1; /*rising edge trigged*/
            	gpio_cfg.s.int_en = 1;  /*interrupt enable*/
            	cvmx_write64_uint64(GPIO_BIT_CFG5, gpio_cfg.u64);
            	cvmx_read64_uint64(GPIO_BIT_CFG5);
            	read_unlock_irqrestore(&octeon_gpio_cfg_rwlock, flags);
                #endif	
            	read_lock_irqsave(&cpld_int_mask_rwlock, flags);
    			/* Enable interrupt */
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG1, 0xff);
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG2, 0xff);
            	cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG3, 0xff);
    			/* Clear interrupt pending */
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG1);
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG2);
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG3);

                local_board_state = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_LOCAL_BOARD_STATE_REG);
            	preState = local_board_state&1;
            	
            	read_unlock_irqrestore(&cpld_int_mask_rwlock, flags); 
			}
			break;
		case PRODUCT_AX8606:
			if(board_id == AX81_CRSMU || board_id == AX81_CRSMUE)
			{
				#if 0
            	read_lock_irqsave(&octeon_gpio_cfg_rwlock, flags);
                gpio_cfg.u64 = 0;
                gpio_cfg.u64 = cvmx_read64_uint64(GPIO_BIT_CFG5);
            	gpio_cfg.s.int_type = 1;
            	gpio_cfg.s.int_en = 1;
            	cvmx_write64_uint64(GPIO_BIT_CFG5, gpio_cfg.u64);
            	cvmx_read64_uint64(GPIO_BIT_CFG5);
            	read_unlock_irqrestore(&octeon_gpio_cfg_rwlock, flags);
                #endif	
            	read_lock_irqsave(&cpld_int_mask_rwlock, flags);
    			/* Enable interrupt */
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG1, 0xff);
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG2, 0xff);
            	cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG3, 0xff);
    			/* Clear interrupt pending */
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG1);
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG2);
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG3);

                local_board_state = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_LOCAL_BOARD_STATE_REG);
            	preState = local_board_state&1;
            	
            	read_unlock_irqrestore(&cpld_int_mask_rwlock, flags); 
			}
			break;
		case PRODUCT_AX8800:
			if(board_id == AX81_CRSMU || board_id == AX81_CRSMUE)
			{
				#if 0
            	read_lock_irqsave(&octeon_gpio_cfg_rwlock, flags);
                gpio_cfg.u64 = 0;
                gpio_cfg.u64 = cvmx_read64_uint64(GPIO_BIT_CFG5);
            	gpio_cfg.s.int_type = 1;
            	gpio_cfg.s.int_en = 1;
            	cvmx_write64_uint64(GPIO_BIT_CFG5, gpio_cfg.u64);
            	cvmx_read64_uint64(GPIO_BIT_CFG5);
            	read_unlock_irqrestore(&octeon_gpio_cfg_rwlock, flags);
            	#endif
            	read_lock_irqsave(&cpld_int_mask_rwlock, flags);
    			/* Enable interrupt */
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG1, 0xff);
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG2, 0xff);
            	cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG3, 0xff);
    			/* Clear interrupt pending */
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG1);
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG2);
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG3);

                local_board_state = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_LOCAL_BOARD_STATE_REG);
            	preState = local_board_state&1;
            	
            	read_unlock_irqrestore(&cpld_int_mask_rwlock, flags); 
			}
			break;
		case PRODUCT_AX8603:
			if(board_id == AX81_AC12C || board_id == AX81_1X12G12S)
			{  	
            	read_lock_irqsave(&cpld_int_mask_rwlock, flags);
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG1, 0xff);
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG2, 0xff);
            	cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG3, 0xff);
				
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG1);
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG2);
            	cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG3);
                local_board_state = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_LOCAL_BOARD_STATE_REG);
            	preState = local_board_state&1;
            	printk(KERN_INFO"local_board_state&1 == %x\n",preState);
            	read_unlock_irqrestore(&cpld_int_mask_rwlock, flags); 
			}
			break;
		default:
			ksemERR("Error product type [%#lx]\n", product_id);
			break;
	}
}

void release_board(void)
{
	unsigned long flags;
	switch(product_id)
	{
        case PRODUCT_AX7605I:
			break;
		case PRODUCT_AX8610:
			if(board_id == AX81_CRSMU || board_id == AX81_CRSMUE)
			{
                read_lock_irqsave(&cpld_int_mask_rwlock, flags);	
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG1, 0x0);
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG2, 0x0);
    			cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG3, 0x0);
            	read_unlock_irqrestore(&cpld_int_mask_rwlock, flags);
	        }
			break;
		case PRODUCT_AX8800:
			if(board_id == AX81_CRSMU || board_id == AX81_CRSMUE)
			{
                read_lock_irqsave(&cpld_int_mask_rwlock, flags);	
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG1, 0x0);
                cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG2, 0x0);
    			cvmx_write64_uint8(CPLD_BASE_ADDR + CPLD_INT_MASK_REG3, 0x0);
            	read_unlock_irqrestore(&cpld_int_mask_rwlock, flags);
	        }
			break;
		default:
			ksemERR("Error product type [%#lx]\n", product_id);
			break;
	}
}

