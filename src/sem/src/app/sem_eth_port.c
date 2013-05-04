#ifdef __cplusplus
extern "C"
{
#endif
#include <errno.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/mman.h> 
#include <sys/types.h>   
#include <asm/types.h>   
#include <linux/netlink.h>   
#include <linux/socket.h> 
#include <linux/tipc.h>
#include <sys/sysinfo.h>    /* for sysinfo() */

#include "sysdef/sem_sysdef.h"
#include "sysdef/returncode.h"
#include "sem_common.h"
#include "sem/sem_tipc.h"
#include "product/product_feature.h"
#include "product/board/board_feature.h"
#include "cvm/ethernet-ioctl.h"
#include "sem_eth_port.h"
#include "sem_eth_port_dbus.h"
#include "sem_common.h"
#include "sem_dbus.h"

extern int g_recv_sd;
extern product_fix_param_t *product;
extern board_fix_param_t *local_board;
struct eth_port_s **g_eth_ports = NULL;
struct global_ethport_s **global_ethport = NULL;
struct global_ethport_s *start_fp[MAX_SLOT_COUNT];
extern thread_index_sd_t thread_id_arr[MAX_SLOT_NUM];

/*
	attr_bitmap-->
		 bit 0 : admin state
	     bit 1 : link state   //only this is available.;
	     bit 2 : autoneg state
	     bit 3 : duplex mode
	     bit 4 : flow control
	     bit 5 : backpressure
	     bit 6 : autoneg-speed
	     bit 7 : autoneg-duplex
	     bit 8 : autoneg-fc 
	     bit 9 : autoneg ctrl
	     bit 12-15 : speed
	     bit 16-19 : pluggable port status
	     bit 24-27 : basic functions
	     bit 28-29 : preferred media
*/
void poll_QT2025_link_status()
{
	unsigned int temp = 0;
	int val;
	int i = 0;
	int active_master_slot_id = product->active_master_slot_id;
    char file_path[64];
	
	sem_syslog_warning("thread %s, thread id is %d\n", __FUNCTION__, getpid());
	
    val = sem_bm_phyQT2025_read(0x3, 0xd7fd);
	if(val == -1)
	{
        sem_syslog_dbg("\noperate sem_bm_phyQT2025_read wrong!\n",val);		
	}

    if(val == 0x70)
    {
        start_fp[local_board->slot_id][24].attr_bitmap = 0x3003;// bit 0 for admin_status;bit 1 for link status;bit 12/13 = 11 for 10G
    }
    else
    {
        start_fp[local_board->slot_id][24].attr_bitmap = 0x3001;//3001
    }
	temp = val;
    while(1)
    {
        sleep(1);
        val = sem_bm_phyQT2025_read(0x3, 0xd7fd);
		if(val == -1)
		{
	        sem_syslog_dbg("\noperate sem_bm_phyQT2025_read wrong!\n",val);		
		}
    
	    if(val == 0x70)
	    {
            start_fp[local_board->slot_id][24].attr_bitmap = 0x3003;//3003 bit 0 for admin_status;bit 1 for link status;bit 12/13 = 11 for 10G
	    }
	    else
	    {
            start_fp[local_board->slot_id][24].attr_bitmap = 0x3001;//3001
	    }

		if(temp == val)
		{
			//sem_syslog_warning("==\n");
			continue;
		}
		else
		{
			//sem_syslog_warning("!=\n");
			temp = val;
		}
		
		msync(start_fp[local_board->slot_id], sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM, MS_ASYNC);
        if(local_board->slot_id != active_master_slot_id)
        {
            sprintf(file_path, "/dbm/shm/ethport/shm%d", local_board->slot_id+1);
            product->sync_send_file(active_master_slot_id, file_path, 0);
        }


	}
}

int sem_read_eth_port_info(int slot , int port, struct eth_port_s *port_info)
{
	int ret;

	sem_syslog_dbg("\tcall sem_read_eth_port_info:slot=%d port=%d\n", slot, port);

	ret = local_board->read_eth_port_info(slot, port, port_info);

	return ret;
}

int sem_set_ethport_attr(unsigned int eth_l_index)
{
	unsigned int attr_bitmap;
	unsigned int slot_id = local_board->slot_id;
	unsigned int admin_state, autoNeg, speed, duplex_mode, media_type, mtu;
	unsigned int eth_g_index = 0, local_port_no = 0, local_slot_no = 0;
	int ret;
    
	local_slot_no = start_fp[slot_id][eth_l_index].slot_no;
	local_port_no = start_fp[slot_id][eth_l_index].local_port_no;
	eth_l_index = local_port_no - 1;
	
	attr_bitmap = start_fp[slot_id][eth_l_index].attr_bitmap;
    admin_state = (attr_bitmap & ETH_ATTR_ADMIN_STATUS) >> ETH_ADMIN_STATUS_BIT;
	autoNeg = (attr_bitmap & ETH_ATTR_AUTONEG) >> ETH_AUTONEG_BIT;
    speed = (attr_bitmap & ETH_ATTR_SPEED_MASK) >> ETH_SPEED_BIT;
	duplex_mode = (attr_bitmap & ETH_ATTR_DUPLEX) >> ETH_DUPLEX_BIT;
	media_type = (attr_bitmap & ETH_ATTR_MEDIA_MASK) >> ETH_ATTR_MEDIA_BIT;

	mtu = start_fp[slot_id][eth_l_index].mtu;
	
	sem_syslog_dbg("\tsem set port attr: eth_l_index %d, admin_state %s.\n", 
		eth_l_index, admin_state ? "ON" : "OFF");
	
	ret = local_board->set_port_admin_status(eth_l_index, admin_state);
	if(SEM_SUCCESS != ret) {
		sem_syslog_dbg("set port(%d,%d) admin status error %d\n ",local_slot_no, local_port_no, ret);
	}	

    sem_syslog_dbg("\tsem set port attr: eth_l_index %d, preferred media %d.\n", \
		eth_l_index, media_type);		
	ret = local_board->set_port_preferr_media(eth_l_index, media_type);
	if(SEM_SUCCESS != ret) {
		sem_syslog_dbg("set port(%d,%d) preferred media error %d\n ",local_slot_no, local_port_no, ret);
	}	
	
    sem_syslog_dbg("\tsem set port attr: eth_l_index %d, autoNeg status %s.\n", 
		eth_l_index, autoNeg ? "ON" : "OFF");		
	ret = local_board->set_port_autoneg_status(eth_l_index, autoNeg);
	if(SEM_SUCCESS != ret) {
		sem_syslog_dbg("set port(%d,%d) autoneg status error %d\n ",local_slot_no, local_port_no, ret);
	}		

    sem_syslog_dbg("\tsem set port attr: eth_l_index %d, mtu %d.\n", eth_l_index, mtu);		
	ret = local_board->set_ethport_mtu(local_slot_no, local_port_no, mtu);
    if(SEM_SUCCESS != ret) {
		sem_syslog_dbg("set port(%d,%d) mtu error %d\n ",local_slot_no, local_port_no, ret);
	}	
	
	return ret;
}



int sem_set_ethport_default_attr(unsigned int eth_l_index)
{
	unsigned int attr_bitmap;
	unsigned int eth_g_index = 0, local_port_no = 0, local_slot_no = 0;
	int ret;

	local_slot_no = local_board->slot_id + 1;
	local_port_no = eth_l_index + 1;
	eth_g_index = SLOT_PORT_COMBINATION(local_slot_no, local_port_no);
	
	sem_syslog_dbg("\tsem set port attr default: eth_l_index %d, admin_state %s.\n", 
		eth_g_index, local_board->port_arr[eth_l_index].port_attribute.admin_state ? "ON" : "OFF");

	ret = local_board->set_port_admin_status(eth_l_index, local_board->port_arr[eth_l_index].port_attribute.admin_state);
	if(SEM_SUCCESS != ret) {
		sem_syslog_dbg("set port(%d,%d) admin status error %d\n ",local_slot_no, local_port_no, ret);
	}	

    sem_syslog_dbg("\tsem set port attr default: eth_l_index %d, preferred media %d.\n", \
		eth_g_index, local_board->port_arr[eth_l_index].port_attribute.mediaPrefer);		
	ret = local_board->set_port_preferr_media(eth_l_index, local_board->port_arr[eth_l_index].port_attribute.mediaPrefer);
	if(SEM_SUCCESS != ret) {
		sem_syslog_dbg("set port(%d,%d) preferred media error %d\n ",local_slot_no, local_port_no, ret);
	}
	
    sem_syslog_dbg("\tsem set port attr default: eth_l_index %d, autoNeg status %s.\n", 
		eth_g_index, local_board->port_arr[eth_l_index].port_attribute.autoNego ? "ON" : "OFF");		
	ret = local_board->set_port_autoneg_status(eth_l_index, local_board->port_arr[eth_l_index].port_attribute.autoNego);
	if(SEM_SUCCESS != ret) {
		sem_syslog_dbg("set port(%d,%d) autoneg status error %d\n ",local_slot_no, local_port_no, ret);
	}	
	
#if 0	
    sem_syslog_dbg("sem set port attr default: eth_l_index %d, speed %d.\n", 
		eth_g_index, local_board->port_arr[eth_l_index].port_attribute.speed);
	ret = local_board->set_ethport_speed(eth_l_index, local_board->port_arr[eth_l_index].port_attribute.speed);
	if(SEM_SUCCESS != ret) {
		sem_syslog_dbg("set port(%d,%d) speed error %d\n ",local_slot_no, local_port_no, ret);
	}	
	/* duplex mode full unsupport*/
	ret = nbm_set_ethport_duplex_mode(eth_l_index,ethport_attr_default(moduleId)->duplex);
	/* flow-control enable*/
	attr_bitmap |= (ETH_PORT_FC_STATE_DEFAULT << ETH_FLOWCTRL_BIT);
	ret |= nbm_set_ethport_flowCtrl(eth_l_index,ETH_ATTR_DISABLE);
	/* back-pressure disable*/
	attr_bitmap |= (ETH_PORT_BP_STATE_DEFAULT << ETH_BACKPRESSURE_BIT);	
	ret |= nbm_set_ethport_backPres(eth_l_index,ethport_attr_default(moduleId)->bp);
#endif

    sem_syslog_dbg("\tsem set port attr default: eth_l_index %d, mtu %d.\n", \
		eth_g_index, local_board->port_arr[eth_l_index].port_attribute.mtu);		
	ret = local_board->set_ethport_mtu(local_slot_no, local_port_no, local_board->port_arr[eth_l_index].port_attribute.mtu);
    if(SEM_SUCCESS != ret) {
		sem_syslog_dbg("set port(%d,%d) mtu error %d\n ",local_slot_no, local_port_no, ret);
	}	
	
	return ret;
}

void sem_create_eth_port(unsigned int eth_l_index) 
{
    unsigned int eth_g_index = 0, local_port_no = 0, local_slot_no = 0;
	int ret = 0;
	struct timeval tnow;
	struct timezone tzone;
	unsigned int attr_bitmap;
	struct eth_port_s port_info;
	struct eth_port_s *eth_ports = NULL;
	unsigned int slot_id = local_board->slot_id;
    struct sysinfo info = {0};      /* for mobile test */

	memset(&tnow, 0, sizeof(struct timeval));
	memset(&tzone, 0, sizeof(struct timezone));

    local_slot_no = local_board->slot_id + 1;
	local_port_no = eth_l_index + 1;
	eth_g_index = SLOT_PORT_COMBINATION(local_slot_no, local_port_no);
	
	sem_syslog_dbg("\tlocal port %d global index %#x\n",local_port_no, eth_g_index);
/*	
	if(global_ethport[eth_g_index]) 
	{
		sem_syslog_dbg("port %d/%d hot plugin, skip reconstuct\n", local_slot_no, local_port_no);
		ret = sem_set_ethport_default_attr(eth_l_index); 
		return;
	}
*/	
	if (!((product->product_type == XXX_YYY_AX8610 || \
		product->product_type == XXX_YYY_AX8800 || \
		product->product_type == XXX_YYY_AX8606) && \
		((local_slot_no == (product->master_slot_id[0] + 1)) || \
		(local_slot_no == (product->master_slot_id[1] + 1)))))
	{
        sem_set_ethport_default_attr(eth_l_index);
    }

	ret = sem_read_eth_port_info(local_slot_no, eth_l_index, &port_info);
	if(ret < 0)
	{
        sem_syslog_dbg("Failed to read port%d-%d info \n", local_slot_no, local_port_no);
	}

	if(g_eth_ports[eth_g_index]) {
		sem_syslog_dbg("port %d/%d hot plugin, skip reconstuct\n", local_slot_no, local_port_no);
	}else {
    	eth_ports = malloc(sizeof(struct eth_port_s));
    	if(NULL == eth_ports)
    	{
    		sem_syslog_dbg("memory alloc error when create ether port %d/%d!!!\n", local_slot_no, local_port_no);
    		return;
    	}
    	memset((void *)eth_ports,0,sizeof(struct eth_port_s));

		sysinfo(&info);     /* fro mobile test */
    	gettimeofday (&tnow, &tzone);
    	eth_ports->lastLinkChange = tnow.tv_sec;
	}
	g_eth_ports[eth_g_index] = eth_ports;
	
    start_fp[slot_id][eth_l_index].isValid = VALID_ETHPORT;
	start_fp[slot_id][eth_l_index].cpu_or_asic = CPU;
	start_fp[slot_id][eth_l_index].eth_g_index = eth_g_index;
	start_fp[slot_id][eth_l_index].slot_no = local_slot_no;
	start_fp[slot_id][eth_l_index].local_port_no = local_port_no;
	start_fp[slot_id][eth_l_index].local_port_index = eth_l_index;
	start_fp[slot_id][eth_l_index].port_type = local_board->port_arr[eth_l_index].port_type;
	start_fp[slot_id][eth_l_index].mtu = port_info.mtu;
	start_fp[slot_id][eth_l_index].attr_bitmap = port_info.attr_bitmap;		
	start_fp[slot_id][eth_l_index].linkchanged = 0; /* for mobile test */
	start_fp[slot_id][eth_l_index].lastLinkChange = info.uptime; /* for mobile test */
	start_fp[slot_id][eth_l_index].port_default_attr.admin_state = local_board->port_arr[eth_l_index].port_attribute.admin_state;
	start_fp[slot_id][eth_l_index].port_default_attr.autoNego = local_board->port_arr[eth_l_index].port_attribute.autoNego;
	start_fp[slot_id][eth_l_index].port_default_attr.bp = local_board->port_arr[eth_l_index].port_attribute.bp;
	start_fp[slot_id][eth_l_index].port_default_attr.duplex = local_board->port_arr[eth_l_index].port_attribute.duplex;
	start_fp[slot_id][eth_l_index].port_default_attr.duplex_an = local_board->port_arr[eth_l_index].port_attribute.duplex_an;
	start_fp[slot_id][eth_l_index].port_default_attr.fc = local_board->port_arr[eth_l_index].port_attribute.fc;
	start_fp[slot_id][eth_l_index].port_default_attr.fc_an = local_board->port_arr[eth_l_index].port_attribute.fc_an;
	start_fp[slot_id][eth_l_index].port_default_attr.mediaPrefer = local_board->port_arr[eth_l_index].port_attribute.mediaPrefer;
	start_fp[slot_id][eth_l_index].port_default_attr.mtu = local_board->port_arr[eth_l_index].port_attribute.mtu;
	start_fp[slot_id][eth_l_index].port_default_attr.speed = local_board->port_arr[eth_l_index].port_attribute.speed;
	start_fp[slot_id][eth_l_index].port_default_attr.speed_an = local_board->port_arr[eth_l_index].port_attribute.speed_an;

	return;
}

int sem_ethports_init()
{
    int fd = -1, sd = -1;
	char file_path[64];
	int i, j;
	off_t length;
	
    sem_syslog_dbg("\t-------------------------sem_ethports_init start--------------------------\n");

	g_eth_ports = malloc(sizeof(struct eth_port_s *)*(GLOBAL_ETHPORTS_MAXNUM));
	if(NULL == g_eth_ports)
	{
		sem_syslog_dbg("memory alloc g_eth_ports error for eth port init!!!\n");
		return 1;
	}
	
	memset(g_eth_ports,0,sizeof(struct eth_port_s *)*(GLOBAL_ETHPORTS_MAXNUM));


    global_ethport = (struct global_ethport_s **)malloc(sizeof(struct global_ethport_s*)*GLOBAL_ETHPORTS_MAXNUM);
	if(NULL == global_ethport)
	{
		sem_syslog_dbg("memory alloc global_ethport error for eth port init!!!\n");
		return 1;
	}
	sem_syslog_dbg("\tmemory alloc sizeof(struct global_ethport_s) = %d\n", sizeof(struct global_ethport_s));
	memset(global_ethport, 0, sizeof(struct global_ethport_s*)*GLOBAL_ETHPORTS_MAXNUM);

	
    for(i = 0; i < SLOT_COUNT; i++)
    {
		system("mkdir -p /dbm/shm/ethport/");
    	sprintf(file_path, "/dbm/shm/ethport/shm%d", i+1);
		
		fd = open(file_path, O_CREAT | O_RDWR, 00755);
		
    	if(fd < 0)
        {
            sem_syslog_dbg("Failed to open! %s\n", strerror(errno));
            return 1;
        }

		length = lseek(fd, sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM-1, SEEK_SET);
        write(fd,"",1);

        start_fp[i] = (struct global_ethport_s *)mmap(NULL, sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM,
    		PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
		if(start_fp[i] == MAP_FAILED)
		{
			// TODO: if faile should munmap already mmap success start_fp[] and close the opened sd
            sem_syslog_dbg("Failed to mmap for slot%d! %s\n", i+1, strerror(errno));
			close(fd);
			return 1;
		}
		
		//memset(start_fp[i], 0, sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM);

		for(j = 0; j < BOARD_GLOBAL_ETHPORTS_MAXNUM; j++)
		{
	        global_ethport[i*BOARD_GLOBAL_ETHPORTS_MAXNUM + j] = &start_fp[i][j];
		}

		close(fd);
    }
	
	sem_syslog_dbg("\tCreating eth-port on slot%d.\r\n", local_board->slot_id + 1);
	for (j = 0; j < local_board->port_num_on_panel; j++)
	{		
		if (local_board->port_arr[j].cpu_or_asic == CPU)
	    {
		    sem_create_eth_port(j);
		}
	}
	
    for(i = 0; i < SLOT_COUNT; i++)
    {
        msync(start_fp[i], sizeof(struct global_ethport_s)*BOARD_GLOBAL_ETHPORTS_MAXNUM, MS_ASYNC);
    }
	
	sem_syslog_dbg("\t-------------------------sem_ethports_init end--------------------------\n");
	
    return 0;
}


#ifdef __cplusplus
}
#endif
