#ifdef __cplusplus
extern "C"
{
#endif


#include <linux/tipc.h>
#include <linux/ioctl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <dbus/sem/sem_dbus_def.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "cvm/ethernet-ioctl.h"

#include "sem/product.h"
#include "sysdef/sem_sysdef.h"
#include "../../sem_eth_port.h"
#include "../../sem_common.h"
#include "../../sem_dbus.h"
#include "sysdef/returncode.h"
#include "board_feature.h"
#include "ax81_smu.h"
#include "../product_ax8610.h"
#include "sem/sem_tipc.h"
#include "../product_feature.h"


static unsigned short phy_ctl_copper[] = {0x1140, 0x1140};
static unsigned short phy_ctl_fiber[] = {0x1140, 0x1140};

extern product_fix_param_t *product;
extern product_info_syn_t product_info_syn;
extern board_fix_param_t *local_board;

extern int sem_oct_mdio_read(int port, int location);
extern int sem_oct_mdio_write(int port, int location, uint16_t value);
extern int get_ctl_fd(void);
extern int set_mtu(const char *dev, int mtu);
extern int get_mtu(const char *dev, unsigned int *mtu);
extern int set_ethport_mtu(int slot, int port, unsigned int mtu_size);
extern int get_ethport_mtu(int slot, int port, unsigned int *mtu_size);


static int update_local_board_state(board_fix_param_t *board)
{
	FILE *fd;
	char buf[200];
	char buf1[200];
	
	fd = fopen("/dbm/local_board/is_active_master", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/is_active_master failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->is_active_master);
	fclose(fd);

	fd = fopen("/dbm/local_board/is_use_default_master", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/is_use_default_master failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->is_use_default_master);
	fclose(fd);

	fd = fopen("/dbm/local_board/board_state", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_state failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->board_state);
	fclose(fd);

	sprintf(buf, "/dbm/product/slot/slot%d", board->slot_id+1);
	strcpy(buf1, buf);
	strcat(buf1, "/board_state");
	fd = fopen(buf1, "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("update %s error\n", buf1);
		return -1;
	}
	fprintf(fd, "%d\n", board->board_state);
	fclose(fd);

	return 0;
}




static int syn_board_info(int sd, board_info_syn_t *board_info)
{
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	board_info_syn_t *board_info_head;
	char send_buf[SEM_TIPC_SEND_BUF_LEN] = {0};

	sem_syslog_dbg("syn board info local_board->slot_id = %d\n", board_info->slot_id);

	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = board_info->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_BOARD_INFO_SYN;
	tlv_head->length = 33;

	board_info_head = (board_info_syn_t *)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
	memcpy(board_info_head, board_info, sizeof(board_info_syn_t));
	
	if (sendto(g_send_sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+sizeof(board_info_syn_t), 0, (struct sockaddr*)&g_send_sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_dbg("	sync board info failed\n");
		return 1;
	}
	else
	{
		sem_syslog_dbg("	sync board info succeed\n");
	}
	
	return 0;
}

static int local_board_dbm_file_create(board_fix_param_t *board)
{
	FILE *fd;
	system("mkdir -p /dbm/local_board/");

	//system("touch /dbm/local_board/name");

	fd = fopen("/dbm/local_board/name", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/name failed\n");
		return -1;
	}
	fprintf(fd, "%s\n", board->name);
	fclose(fd);

	fd = fopen("/dbm/local_board/board_code", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_code failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->board_code);
	fclose(fd);
	
	fd = fopen("/dbm/local_board/slot_id", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/slot_id failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->slot_id + 1);
	fclose(fd);
	
	fd = fopen("/dbm/local_board/is_master", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/is_master failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->is_master);
	fclose(fd);

	fd = fopen("/dbm/local_board/is_active_master", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/is_active_master failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->is_active_master);
	fclose(fd);

	fd = fopen("/dbm/local_board/is_use_default_master", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/is_use_default_master failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->is_use_default_master);
	fclose(fd);


	fd = fopen("/dbm/local_board/port_num_on_panel", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/port_num_on_panel failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->port_num_on_panel);
	fclose(fd);

	fd = fopen("/dbm/local_board/port_num_on_panel", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/port_num_on_panel failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->port_num_on_panel);
	fclose(fd);

	fd = fopen("/dbm/local_board/obc_port_num", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/obc_port_num failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->obc_port_num);
	fclose(fd);

	fd = fopen("/dbm/local_board/cscd_port_num", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/cscd_port_num failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->cscd_port_num);
	fclose(fd);

	fd = fopen("/dbm/local_board/function_type", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/function_type failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->function_type);
	fclose(fd);

	fd = fopen("/dbm/local_board/board_state", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_state failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->board_state);
	fclose(fd);
	
	fd = fopen("/dbm/local_board/board_id", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_id failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->board_id);
	fclose(fd);
	return 0;
}

/*
 *board to reboot itself's cpu
 *
 *
 */
static void reboot_self(void)
{
	int i=0;
	int ret=0;
	if (local_board->is_active_master) {
		ret =sem_send_signal_to_trap_reboot();
		if(ret){
            sem_syslog_warning("sem_send_signal_to_trap_reboot failed");
		}
        system ("echo 1 > /mnt/sem_trap.flag");
    	ret = sem_sor_exec("cp","sem_trap.flag",50);
        if(ret){
        sem_syslog_warning("sor.sh rm sem_trap.flag 50 failed");
        }
        system ("rm /mnt/sem_trap.flag");
	}
	system ("dcli_reboot.sh");

	#if 0
	sem_write_cpld(CPLD_HW_RESET_CONTROL_REG1, 0xff);
	#if 1
    struct timeval tv;
    tv.tv_sec = 1; //1s
    tv.tv_usec = 1;//1us  	
	int rc = select(0, NULL, NULL, NULL, &tv);
	#else
	sleep(1);
	#endif
	
	sem_write_cpld(CPLD_HW_RESET_CONTROL_REG1, 0xfe);
	#endif	
}


/*
 *
 *board ax81_smu hardware reset self
 *
 *
 */
 
int ax81_smu_hard_reset(void)
{
	int ret;
	sem_syslog_warning("board %s on slot %d self reset\n", local_board->name, local_board->slot_id);
	ret = sem_write_cpld(CPLD_HW_SELFT_RESET_REG, CPLD_SELF_RESET_DATA);
	return ret;
}

/**
 * Perform an MII read. Called by the generic MII routines
 *
 * @param phy_id   The MII phy id, just the portNum
 * @param location Register location to read
 * @return Result from the read or zero on failure
 */
static int sem_oct_mii_read(int port, int location)
{
	phy_read_write_t phy_read_value;
	int retval = 0;
	int fd = -1;

	memset(&phy_read_value, 0, sizeof(phy_read_write_t));
	
	phy_read_value.phy_addr = port;
	phy_read_value.location = location;

	fd = open(DEV_NAME, 0);
	if(fd == -1)
	{
		sem_syslog_dbg("open Dev(%s) error!", DEV_NAME);
		return -1;
	}
	
	retval = ioctl(fd, CVM_IOC_PHY_READ, &phy_read_value);
	
	if(retval == -1)
	{
		sem_syslog_dbg("read failed\n");
		close(fd);
		return -1;
	}
	close(fd);

	return phy_read_value.read_reg_val;
}

/**
 * Perform an MII write. 
 *
 * @param phy_id     The MII phy id, just the portNum
 * @param location   Register location to write
 * @return   -1 for failure. 0 for success.
 * add by baolc, 2008-07-10
 */
static int sem_oct_mii_write(int port, int location, uint16_t value)
{
	phy_read_write_t phy_write_value;

	int retval = 0;
	int fd = -1;

	memset(&phy_write_value, 0, sizeof(phy_read_write_t));
	
	phy_write_value.phy_addr = port;
	phy_write_value.location = location;
	phy_write_value.write_reg_val = value;
	
	fd = open(DEV_NAME, 0);
	if(fd == -1)
	{
		sem_syslog_dbg("open Dev %s error!", DEV_NAME);
		return -1;
	}
	
	retval = ioctl(fd, CVM_IOC_PHY_WRITE, &phy_write_value);
	if(retval == -1)
	{
		sem_syslog_dbg("read failed\n");
		close(fd);
		return -1;
	}
	close(fd);
	
	return 0;
}

static int miisw_read(unsigned int devaddr, unsigned int regaddr)
{
	smi_cmd_88e6185_reg_t smi_cmd;
	int return_value;
	smi_cmd.u16 = 0;
	smi_cmd.s.smi_busy = 1;
	smi_cmd.s.smi_mode = 1; 
	smi_cmd.s.smi_op = 0;   /* 0 = read */
	smi_cmd.s.dev_addr = devaddr;
	smi_cmd.s.reg_addr = regaddr;

	if (sem_oct_mii_write(MV_88E6185_AX52XX_SGMII_ADDR, MV_88E6185_SMI_COMMAND_REG, smi_cmd.u16))
 	{
		return 1;
 	}
	usleep(1000);
	
    if (!(return_value = sem_oct_mii_read(MV_88E6185_AX52XX_SGMII_ADDR, MV_88E6185_SMI_DATA_REG)))
    {
		return 1;
    }
	usleep(1000);
	
 
	return return_value;
}


static int miisw_write(unsigned int devaddr, unsigned int regaddr, unsigned short value)
{
	smi_cmd_88e6185_reg_t smi_cmd;
	
	smi_cmd.u16 = 0;
	smi_cmd.s.smi_busy = 1;
	smi_cmd.s.smi_mode = 1; 
	smi_cmd.s.smi_op = 1;   /* 1 = write */
	smi_cmd.s.dev_addr = devaddr;
	smi_cmd.s.reg_addr = regaddr;
	
    if (sem_oct_mii_write(MV_88E6185_AX52XX_SGMII_ADDR, MV_88E6185_SMI_DATA_REG, value))
    {
		return 1;
    }
	usleep(1000);
	
 	if (sem_oct_mii_write(MV_88E6185_AX52XX_SGMII_ADDR, MV_88E6185_SMI_COMMAND_REG, smi_cmd.u16))
 	{
		return 1;
 	}
	usleep(1000);
 
	return 0;
}

/*
 *board self special initialize
 *
 *ret val: 0 for success 1 for failed.
 */
int ax81_smu_init(void)
{
	int port = 0;
	unsigned short data = 0;
	int value;

	//data = 0x200;
	//data = 0x3fff;
	//for (port = 0x10; port < 0x19; port++)
	//{
	//	miisw_write(port, MV_88E6185_PORT_BASED_VLAN_MAP, data);
	//}
    data = 0x2FE;
	miisw_write(0x10, MV_88E6185_PORT_BASED_VLAN_MAP, data);
    data = 0x2FD;
	miisw_write(0x11, MV_88E6185_PORT_BASED_VLAN_MAP, data);
    data = 0x2FB;
	miisw_write(0x12, MV_88E6185_PORT_BASED_VLAN_MAP, data);
    data = 0x2F7;
	miisw_write(0x13, MV_88E6185_PORT_BASED_VLAN_MAP, data);
    data = 0x2EF;
	miisw_write(0x14, MV_88E6185_PORT_BASED_VLAN_MAP, data);
    data = 0x2DF;
	miisw_write(0x15, MV_88E6185_PORT_BASED_VLAN_MAP, data);
    data = 0x2BF;
	miisw_write(0x16, MV_88E6185_PORT_BASED_VLAN_MAP, data);
    data = 0x27F;
	miisw_write(0x17, MV_88E6185_PORT_BASED_VLAN_MAP, data);
    data = 0x200;
	miisw_write(0x18, MV_88E6185_PORT_BASED_VLAN_MAP, data);

	data = 0x1ff;
	miisw_write(0x19, MV_88E6185_PORT_BASED_VLAN_MAP, data);

	//change 88e6185 mtu
	value = miisw_read(0x1b, 0x4);
	sem_syslog_dbg("value = %x\n", value);
	miisw_write(0x1b, 0x4, value | (1 << 10));
	value = miisw_read(0x1b, 0x4);
	sem_syslog_dbg("value = %x\n", value);

	return 0;
}

/*
 *function:It is used for boards except active master board to register current board state to active master board.
 *sem may block here
 *
 */
static int board_register(board_fix_param_t *board, int sd, board_info_syn_t *board_info)
{
	sem_pdu_head_t *head;
	sem_tlv_t *tlv_head;
	board_info_syn_t *board_info_head;
	
	char send_buf[SEM_TIPC_SEND_BUF_LEN] = {0};

	sem_syslog_dbg("register board state local_board->slot_id = %d\n", board->slot_id+1);
	sem_syslog_dbg("sd=%d\n", sd);

	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = board->slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_NON_ACTIVE_BOARD_REGISTER;
	tlv_head->length = 33;

	board_info_head = (board_info_syn_t *)(send_buf + sizeof(sem_pdu_head_t) + sizeof(sem_tlv_t));
	memcpy(board_info_head, board_info, sizeof(board_info_syn_t));

	if (sendto(g_send_sd, send_buf, sizeof(sem_pdu_head_t)+sizeof(sem_tlv_t)+sizeof(board_info_syn_t), 0, (struct sockaddr*)&g_send_sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_dbg("	board send register message failed\n");
		return 1;
	}
	else
	{
		sem_syslog_dbg("	board send register message succeed\n");
	}
	
	return 0;
}

static int get_product_port_name(int slot, int port, char *eth_port_name)
{
	sprintf(eth_port_name, "mng%d-%d", slot, port);
	sem_syslog_dbg("\tport name is: %s\n", eth_port_name);
	return 0;
}


static int sem_oct_check_media_and_change_page(unsigned char port, int *media)
{
	int reg_val;
	int ret;
	/*
	 * read reg 26 bit10~bit11 to determine which media is selected: copper or fiber 
	 * 11:10	Autoselect preferred media
	 *	00 = No Preference for Media
	 *	01 = Preferred Fiber Medium
	 *	10 = Preferred Copper Medium
	 *	11 = Reserved
	 */
	reg_val = sem_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2);
	*media = ((reg_val >> 10) & 0x3);
	if(*media == 0x0)			/* No Preference for Media */
	{
		/*
		 * set page for copper meidia
		 */
		sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 0);			
	}
	else if(*media == 0x1)		/* Preferred Fiber */
	{		
		/* 
		 * set page for fiber media 
		 */
		sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 1);
	}
	else if(*media == 0x2)	/* Preferred Copper */
	{
		/*
		 * set page for copper meidia
		 */
		sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 0);			
	}
	else 					/* Reserved */
	{
		sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 0);			
		*media = 0;
	}

	return SEM_COMMAND_SUCCESS;
}


/**********************************************************************************
 *
 *
 **********************************************************************************/
static int read_eth_port_info(int slot, int port, struct eth_port_s *port_info)
{
	int retval = 0;
	int fd = 0;
	unsigned short reg_value = 0;
	unsigned short ctl_value = 0;
	unsigned long tmp = 0;
	int type = 0;
	int ret = 0, media = 0;
	
	phy_reg_17_dat fiber_stat;
	phy_reg_17_dat copper_stat;
	cvmx_mdio_phy1145_reg0_1 fiber_ctl;
	cvmx_mdio_phy1145_reg0_0 copper_ctl;

	/* some param for read FC and BP status, add by baolc */
	bm_op_rbit64_args  frm_ctl_ioctl_args = {0};
	bm_op_rbit64_args  ovr_bp_ioctl_args = {0};
	bm_op_rbit64_args  bp_page_cnt_args = {0};
	cvmx_gmxx_rxx_frm_ctl_t  frm_ctl_reg;
	cvmx_gmxx_tx_ovr_bp_t	 ovr_bp_reg;
	cvmx_ipd_portx_bp_page_cnt_t  bp_page_cnt_reg;
	
	/* preferred_copper:0 preferred select is not copper or yes */
	int preferred_copper;
	
	/* preferred_fiber:0 preferred select is not fiber or yes */
	int preferred_fiber;
	
	if (port < 0 || port >= local_board->port_num_on_panel || !port_info)
	{
		return SEM_WRONG_PARAM;
	}

	if (local_board->port_arr[port].cpu_or_asic != CPU)
	{
		return SEM_COMMAND_NOT_SUPPORT;
	}
	

	sem_syslog_dbg("\tport %d media is copper\n", port);
	
	/* preferred copper not fiber */
	preferred_copper = 1;
	preferred_fiber = 0;
	
	/* get copper state of duplex ,speed and link */
	copper_stat.u16 = sem_oct_mdio_read(port, MV_88E1145_PHY_SPEC_STATUS);

	sem_syslog_dbg("\tcopper_stat.u16 = %#x\n", copper_stat.u16);
	/* get control state */
	copper_ctl.u16 = sem_oct_mdio_read(port, 0);	
	reg_value = copper_stat.u16;
	ctl_value = copper_ctl.u16;
	if(copper_ctl.u16 == 0xffff)
	{
		copper_ctl.u16 = phy_ctl_copper[port];
	}
	else
	{
		phy_ctl_copper[port] = copper_ctl.u16;
	}
	

	tmp |= ((1 << ETH_PREFERRED_COPPER_MEDIA_BIT));

	if(copper_ctl.s.auto_negotiaton_en || fiber_ctl.s.auto_negotiaton_en) 
	{
		sem_syslog_dbg("\tAUTONEG enable\n"); 
		tmp |= ETH_ATTR_ON << ETH_AUTONEG_SPEED_BIT;
		tmp |= ETH_ATTR_ON << ETH_AUTONEG_DUPLEX_BIT; 
		tmp |= ETH_ATTR_ON << ETH_AUTONEG_CTRL_BIT;
		tmp |= ETH_ATTR_ON << ETH_AUTONEG_BIT;
	}
	else
	{
		sem_syslog_dbg("\tAUTONEG disable\n"); 
		tmp |= ETH_ATTR_OFF << ETH_AUTONEG_SPEED_BIT;
		tmp |= ETH_ATTR_OFF << ETH_AUTONEG_DUPLEX_BIT; 
		tmp |= ETH_ATTR_OFF << ETH_AUTONEG_CTRL_BIT;
		tmp |= ETH_ATTR_OFF << ETH_AUTONEG_BIT;
	}
	/* get MTU */
	ret = get_ethport_mtu(slot, port+1, &port_info->mtu);
	
	
	/* speed */
	sem_syslog_dbg("\treg17's value = 0x%x\n", reg_value);
	/* 10M copper */
	if ((((reg_value & 0xc000)>>14) == 0) && preferred_copper) 
	{
		sem_syslog_dbg("\tcopper 10M\n");
		tmp |= ETH_ATTR_SPEED_10M << ETH_SPEED_BIT;
		port_info->port_type = ETH_GTX;
	}
	/*100M copper*/
	else if ((((reg_value&0xc000)>>14) == 1) && preferred_copper) 
	{
		sem_syslog_dbg("\tcopper 100M\n");
		tmp |= ETH_ATTR_SPEED_100M << ETH_SPEED_BIT;
		port_info->port_type = ETH_GTX;
	}
	/*1000M copper or fiber*/
	else if (((reg_value&0xc000)>>14) == 2) 
	{
		tmp |= ETH_ATTR_SPEED_1000M << ETH_SPEED_BIT;
		/*for 1000M firber*/
		if(preferred_fiber) 
		{		
			port_info->port_type = ETH_GE_SFP;
			sem_syslog_dbg("\tfiber 1000M\n");
		}
		/*for 1000M copper*/
		else
		{
			port_info->port_type = ETH_GTX;
			sem_syslog_dbg("\tcopper 1000M\n");
		}
	}
	else 
	{
		sem_syslog_dbg("Error:wrong speed\n");	  
	}
	
	if(preferred_copper)				/* copper */
	{
		/*link status*/
		if (copper_stat.s.link) {
			tmp |= ETH_ATTR_LINKUP << ETH_LINK_STATUS_BIT;
		}
		else {
			tmp |= ETH_ATTR_LINKDOWN << ETH_LINK_STATUS_BIT;
		}

		/*duplex*/
		if(copper_stat.s.duplex == SEM_DUPLEX_FULL)
		{
			sem_syslog_dbg("\tfull duplex mode\n");
			tmp |= ETH_ATTR_DUPLEX_FULL << ETH_DUPLEX_BIT;
		}
		else
		{
			sem_syslog_dbg("\thalf duplex mode\n");
			tmp |= ETH_ATTR_DUPLEX_HALF << ETH_DUPLEX_BIT;
		}

		/*admin status*/
		if(copper_ctl.s.power_down)
		{
			tmp |= ETH_ATTR_DISABLE << ETH_ADMIN_STATUS_BIT;
		}
		else
		{
			tmp |= ETH_ATTR_ENABLE << ETH_ADMIN_STATUS_BIT; 		
		}
	}	
	else									/* fiber */
	{
		/*link status*/
		if (fiber_stat.s.link) 
		{
			tmp |= ETH_ATTR_LINKUP << ETH_LINK_STATUS_BIT;
		}
		else 
		{
			tmp |= ETH_ATTR_LINKDOWN << ETH_LINK_STATUS_BIT;
		}

		/*duplex*/
		if(fiber_stat.s.duplex == SEM_DUPLEX_FULL)
		{	
			sem_syslog_dbg("\tfull duplex mode\n");
			tmp |= ETH_ATTR_DUPLEX_FULL << ETH_DUPLEX_BIT;
		}
		else
		{
			sem_syslog_dbg("\thalf duplex mode\n");
			tmp |= ETH_ATTR_DUPLEX_HALF << ETH_DUPLEX_BIT;
		}


		/*admin status*/
		if(fiber_ctl.s.power_down)
		{
			tmp |= ETH_ATTR_DISABLE << ETH_ADMIN_STATUS_BIT;
		}
		else
		{
			tmp |= ETH_ATTR_ENABLE << ETH_ADMIN_STATUS_BIT;
		}
	}
	
	/* read FC status and BP status, add by baolc, 2008-07-04 */
	memset(&frm_ctl_ioctl_args, 0, sizeof(bm_op_rbit64_args));
	memset(&ovr_bp_ioctl_args, 0, sizeof(bm_op_rbit64_args));
	memset(&bp_page_cnt_args, 0, sizeof(bm_op_rbit64_args));

	frm_ctl_ioctl_args.regAddr = CVMX_GMXX_RXX_FRM_CTL(port, 0);
	ovr_bp_ioctl_args.regAddr = CVMX_GMXX_TX_OVR_BP(0);
	bp_page_cnt_args.regAddr = CVMX_IPD_PORTX_BP_PAGE_CNT(local_board->port_arr[port].ipd_port_id);


	fd = open("/dev/bm0",0);
	if(fd < 0) {
		sem_syslog_dbg("open dev %s error(%d) when read eth port info!\n","/dev/bm0",fd);
		return SEM_COMMAND_FAILED;
	}

	retval = ioctl(fd, BM_IOC_BIT64_REG_STATE, &frm_ctl_ioctl_args);
	if(retval == -1){
		sem_syslog_dbg("get port frm_ctl error! portNum: %d\n",port);
		close(fd);
		return SEM_COMMAND_FAILED;
	}

	retval = ioctl(fd, BM_IOC_BIT64_REG_STATE, &ovr_bp_ioctl_args);
	if(retval == -1){
		sem_syslog_dbg("get ovr_bp error! portNum: %d\n", port);
		close(fd);
		return SEM_COMMAND_FAILED;
	}
	
	retval = ioctl(fd, BM_IOC_BIT64_REG_STATE, &bp_page_cnt_args);
	if(retval == -1){
		sem_syslog_dbg("get bp_page_cnt error! portNum: %d\n", port);
		close(fd);
		return SEM_COMMAND_FAILED;
	}

	frm_ctl_reg.u64 = frm_ctl_ioctl_args.regData;
	ovr_bp_reg.u64 = ovr_bp_ioctl_args.regData;
	bp_page_cnt_reg.u64 = bp_page_cnt_args.regData;
	
	close(fd);
	
	/* FC and BP status, add by baolc */
	if (frm_ctl_reg.s.ctl_bck == 1
		&& frm_ctl_reg.s.ctl_drp == 1
		&& (ovr_bp_reg.s.en & (0x1<<port)) != 0 
		&& (ovr_bp_reg.s.bp & (0x1<<port)) != 0)
	{
		/* FC is enable */
		sem_syslog_dbg("\tport FC status is enabled! portNum: %d\n", port);
		tmp |= (ETH_ATTR_ENABLE << ETH_FLOWCTRL_BIT);
	}
	else if (frm_ctl_reg.s.ctl_bck == 1
		&& frm_ctl_reg.s.ctl_drp == 0
		&& (ovr_bp_reg.s.en & (0x1<<port)) != 0 
		&& (ovr_bp_reg.s.bp & (0x1<<port)) == 0)
	{
		/* FC is disable, do nothing */
		sem_syslog_dbg("\tport FC status is disabled! portNum: %d\n", port);
	}
	else 
	{
		/* error status */
		sem_syslog_dbg("\tport FC status incorrect! FC is disabled! portNum: %d\n", port);
	}

	if (bp_page_cnt_reg.s.bp_enb == 1)
	{
		/* BP is enable */
		sem_syslog_dbg("\tport BP status is enabled! portNum: %d\n", port);
		tmp |= (ETH_ATTR_ENABLE << ETH_BACKPRESSURE_BIT);
	}
	else
	{
		/* BP is disable, do nothing */
		sem_syslog_dbg("\tport BP status is disabled! portNum: %d\n", port);
	}
	
	/* out param */
	port_info->attr_bitmap = tmp;
	return SEM_COMMAND_SUCCESS;
}


static int set_port_admin_status(unsigned int port, unsigned int status)
{
	cvmx_mdio_phy1145_reg0_0 copper_ctl;
	cvmx_mdio_phy1145_reg0_1 fiber_ctl;
	int ret;
	
	if (status != 0 && status != 1)
	{
		sem_syslog_dbg("bad state %d\n", status);
		return SEM_WRONG_PARAM;
	}

	copper_ctl.u16 = 0;
	fiber_ctl.u16 = 0;

	/*
	 * Selects copper banks of registers 0, 1, 4, 5, 6, 7, 8, 17, 18, 19.
	 */
	sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 0);
	copper_ctl.u16 = sem_oct_mdio_read(port, 0);

	/*
	 * Selects fiber banks of registers 0, 1, 4, 5, 6, 7, 8, 17, 18, 19.
	 */
	sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 1);
	fiber_ctl.u16 = sem_oct_mdio_read(port, 0);
	
	if(copper_ctl.u16 == 0xffff)		/* that is , -1 */
	{
		sem_syslog_dbg("ERROR copper_ctl = 0x%x\n", phy_ctl_copper[port]);
		copper_ctl.u16 = phy_ctl_copper[port];
	}
	else
	{
		phy_ctl_copper[port] = copper_ctl.u16;
	}
	
	if(fiber_ctl.u16 == 0xffff)				/* that is , -1 */
	{
		sem_syslog_dbg("ERROR fiber_ctl = 0x%x\n", phy_ctl_fiber[port]);
		fiber_ctl.u16 = phy_ctl_fiber[port];
	}
	else
	{
		phy_ctl_fiber[port] = fiber_ctl.u16;
	}

	if(status)						/* ETH_ATTR_ENABLE */
	{
		copper_ctl.s.power_down = 0;	/* 0 = Normal operation */
		copper_ctl.s.reset = 1;			/* 1 = PHY reset */
		fiber_ctl.s.power_down = 0;
		fiber_ctl.s.reset = 1;
	}
	else								/* ETH_ATTR_DISABLE */
	{
		copper_ctl.s.power_down = 1;	/* 1 = Power down */
		copper_ctl.s.reset = 0;			/* 0 = Normal operation */
		fiber_ctl.s.power_down = 1;
		fiber_ctl.s.reset = 0;
	}
	
	/*
	 * Selects copper banks of registers 0, 1, 4, 5, 6, 7, 8, 17, 18, 19.
	 */
	sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 0);
	sem_oct_mdio_write(port, 0, copper_ctl.u16);
	
	/*
	 * Selects fiber banks of registers 0, 1, 4, 5, 6, 7, 8, 17, 18, 19.
	 */
	sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 1);
	sem_oct_mdio_write(port, 0, fiber_ctl.u16);

	return SEM_COMMAND_SUCCESS;
}

static int set_port_preferr_media(unsigned int port, unsigned int media)
{
	int read_value;
	int status;
	cvmx_mdio_phy1145_reg26  reg_value;

	if (media == 0x1)			/* select fiber */
	{
		/* read phy register 27 and set bit0~bit3 */
		read_value = sem_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_STATUS);
		
		/* set mode 0011b rgmii to fiber */
		reg_value.u16 = read_value & 0xfff0;		
		reg_value.u16 |= 0x3;
		
		/* disable fiber/copper auto-selection */
		reg_value.u16 |= (1<<15);
		status = sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_SPEC_STATUS, reg_value.u16);
		if (0 != status)
		{
			sem_syslog_dbg("set board port(%d) media to fiber error\n", port);
			return SEM_COMMAND_FAILED;
		}
		
		/* software reset to take effect disable fiber/copper auto-selection */
		read_value = sem_oct_mdio_read(port, 0x00);
		reg_value.u16 = read_value & 0xffff;
		reg_value.u16 |= (1<<15);

		status = sem_oct_mdio_write(port, 0x00, reg_value.u16);
		if (0 != status)
		{
			sem_syslog_dbg("fiber media select failed:software reset error\n");
			return SEM_COMMAND_FAILED;
		}
		
		/* set register 26 Autoselect preferred media:select fiber */
		read_value = sem_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2);
		reg_value.u16 = read_value & 0xffff;
		reg_value.u16 |= (1<<10);
		reg_value.u16 &= (~(1<<11));
		status = sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2, reg_value.u16);
		if (0 != status)
		{
			sem_syslog_dbg("preferred select failed:set board port(%d) media to fiber error\n", port);
			return SEM_COMMAND_FAILED;
		}	
	}	
	else if (media == 0x2)			/* select copper */
	{
		/* read phy register 27 and set bit0~bit3 */
		read_value = sem_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_STATUS);
		reg_value.u16 = read_value & 0xfff0;
		
		/* set mode 1011b rgmii/modified mii to copper */
		reg_value.u16 |= 0xb;
		
		/* disable fiber/copper auto-selection */
		reg_value.u16 |= (1<<15);
		
		status = sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_SPEC_STATUS, reg_value.u16);
		if (0 != status)
		{
			sem_syslog_dbg("set board port(%d) media to copper error\n", port);
			return SEM_COMMAND_FAILED;
		}
		
		/* software reset to take effect disable fiber/copper auto-selection */
		read_value = sem_oct_mdio_read(port, 0x00);
		reg_value.u16 = read_value & 0xffff;
		reg_value.u16 |= (1<<15);

		status = sem_oct_mdio_write(port, 0x00, reg_value.u16);
		if (0 != status)
		{
			sem_syslog_dbg("copper media select failed:software reset error\n");
			return SEM_COMMAND_FAILED;
		}

		/* set register 26 Autoselect preferred media: select copper */
		read_value = sem_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2);
		reg_value.u16 = read_value & 0xffff;
		reg_value.u16 |= (1<<11);
		reg_value.u16 &= (~(1<<10));
		status = sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2, reg_value.u16);
		if (0 != status)
		{
			sem_syslog_dbg("set board port(%d) media to fiber error\n", port);
			return SEM_COMMAND_FAILED;
		}
	}	
	else		/* other media */		
	{
		sem_syslog_dbg("Error:set board port(%d) media to a unknown media\n", port);
		return SEM_WRONG_PARAM;
	}
		
	/* reset to take effect the setting */
	read_value = sem_oct_mdio_read(port, 0x00);
	reg_value.u16 = read_value & 0xffff;
	reg_value.u16 |= (1<<15);
	status = sem_oct_mdio_write(port, 0x00, reg_value.u16);
	if (0 != status)
	{
		sem_syslog_dbg("set board port(%d) media to %s error\n", port, (media == 0x1 ? "fiber" : (media == 0x2 ? "copper" : "unknown")));
		return SEM_COMMAND_FAILED;
	}
	
	return SEM_COMMAND_SUCCESS; /* success */
}


static int get_eth_port_trans_media(unsigned int port, unsigned int *media)
{
	int read_value;
	cvmx_mdio_phy1145_reg26  reg_value;
	int ret;
	
	read_value = sem_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2);

	reg_value.u16 = read_value & 0xffff;
	*media = reg_value.s.preferred_media; /* return value */
	
	/* if media is not fiber or copper */
	if (*media != 0x1 && *media != 0x2)
	{
		sem_syslog_dbg("get board port(%d) media auto-select prefer media error\n", port);
		return -1; /* read failed */
	}
	
	return 0;
}

static int set_port_autoneg_status(unsigned int port, unsigned int state)
{
	cvmx_mdio_phy1145_reg0_0 copper_ctl;
	cvmx_mdio_phy1145_reg0_1 fiber_ctl;
	fiber_ctl.u16 = 0;
	copper_ctl.u16 = 0;
	int media = 0;	
	int ret;
		
	if (state != 0 && state != 1)
	{
		sem_syslog_dbg("bad stat %d\n", state);
		return SEM_WRONG_PARAM;
	}

	/* get preferred media ; fiber:media = 1; copper:media = 2 */
	if (get_eth_port_trans_media(port, &media))
	{
		sem_syslog_dbg("port:%d get preferred media failed\n", port);
		return SEM_COMMAND_FAILED;
	}

	if (media == 0x1)			/* preferred fiber */
	{
		sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 1);
		fiber_ctl.u16 = sem_oct_mdio_read(port, 0);

		if(fiber_ctl.u16 == 0xffff)
		{
			fiber_ctl.u16 = phy_ctl_fiber[port];
		}
		else
		{
			phy_ctl_fiber[port] = fiber_ctl.u16;
		}

		if(state == 0)		/* disable fiber autonegotiation and set full duplex mode 1000M */
		{
			fiber_ctl.s.auto_negotiaton_en = 0;
			fiber_ctl.s.speedselect_bit13 = 0;
			fiber_ctl.s.speedselect_bit6 = 1;
			fiber_ctl.s.fiber_duplex_mod = 1;
			fiber_ctl.s.reset = 1;
			sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 1);
			sem_oct_mdio_write(port,0,fiber_ctl.u16);
			sem_syslog_dbg("\tPHY1145 AUNEG disable!");
			return SEM_COMMAND_SUCCESS;
		}		
		else if(state == 1) /* enable fiber autonegotiation */
		{
			fiber_ctl.s.auto_negotiaton_en = 1;
			fiber_ctl.s.reset = 1;
			sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 1);
			sem_oct_mdio_write(port,0,fiber_ctl.u16);
			sem_syslog_dbg("\tPHY1145 AUNEG enable!");
			return SEM_COMMAND_SUCCESS;
		}
	}	
	else if (media == 0x2 || media == 0x0)		/* preferred copper */
	{
		/* set page and read.*/
		sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 0);
		copper_ctl.u16 = sem_oct_mdio_read(port, 0);
		if(copper_ctl.u16 == 0xffff)
		{
			copper_ctl.u16 = phy_ctl_copper[port];
		}
		else
		{
			phy_ctl_copper[port] = copper_ctl.u16;
		}

		/*disable copper autonegotiation and set full duplex mode 100M*/
		if(state == 0)
		{
			copper_ctl.s.auto_negotiaton_en = 0;			
			copper_ctl.s.speedselect_bit13 = 0;
			copper_ctl.s.speedselect_bit6 = 1;
			copper_ctl.s.copper_duplex_mod = 1;
			copper_ctl.s.reset = 1;
			sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 0);
			sem_oct_mdio_write(port,0,copper_ctl.u16);
			sem_syslog_dbg("\tPHY1145 AUNEG disable!");
			return SEM_COMMAND_SUCCESS;
		}
		/*enable copper autonegotiation*/
		else if(state == 1)
		{
			copper_ctl.s.auto_negotiaton_en = 1;
			copper_ctl.s.reset = 1;
			sem_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 0);
			sem_oct_mdio_write(port,0,copper_ctl.u16);
			sem_syslog_dbg("\tPHY1145 AUNEG enable!");
			return SEM_COMMAND_SUCCESS;
		}
	}	
	else
	{
		sem_syslog_dbg("port:%d get wrong preferred media\n", port);
		return SEM_COMMAND_FAILED;	
	}
}

/*
* for master board to init mac files
* add by caojia 20110730
*/
static int init_local_mac_info(void)
{
	int fd;
	FILE *fp;
	int ret;
	int i = 0;
	char tmp_mac_buffer[13];
	unsigned char temp_buffer[3] = {0x00, 0x00, 0x00};
	unsigned char mac_addr[6];
	char output_buffer[13];
	char buf[1024];
	char *name;

	/* copy base mac file to other positions */
	system("/etc/init.d/product_mac_info_copy");

	/* get base mac from file, and write local mac to local mac files */
	fd = open("/devinfo/base_mac", O_RDONLY);
	if (fd < 0)
	{
		sem_syslog_dbg("open /devinfo/base_mac failed.\n");
		return -1;
	}

	memset(tmp_mac_buffer, 0x00, 13);
	ret = read(fd, tmp_mac_buffer, 12);

	sem_syslog_dbg("Base Mac : ");
	for (i=0; i<12; i+=2)
	{
		memcpy(temp_buffer, tmp_mac_buffer+i, 2);
		mac_addr[i/2] = (unsigned char)strtoul((char *)temp_buffer, 0, 16);
		sem_syslog_dbg_without_time("%x", mac_addr[i/2]);
	}
	sem_syslog_dbg_without_time("\n");

	close(fd);

	for (i = 0; i < 6; i++)
	{
		product->base_mac_addr[i] = mac_addr[i];
		product_info_syn.base_mac_addr[i] = mac_addr[i];
	}

	mac_addr[5] += local_board->slot_id;

	fp = fopen("/devinfo/local_mac", "w+");
	if (fp == NULL)
	{
		sem_syslog_dbg("open /devinfo/local_mac failed.\n");
		return -1;
	}

	fprintf(fp, "%02X%02X%02X%02X%02X%02X\n", mac_addr[0], mac_addr[1], mac_addr[2],\
		mac_addr[3], mac_addr[4], mac_addr[5]);

	fclose(fp);

	system("cp /devinfo/local_mac /dbm/local_board/mac");

	/* change mac addr of local interfaces */

	/* open /proc/net/dev */
	fp = fopen("/proc/net/dev", "r");
	if (fp == NULL)
	{
		sem_syslog_dbg("Can't open proc file dev.\n");
		sem_syslog_dbg("Update mac address of interfaces failed.\n");
		return -1;
	}

	/* Drop header lines. */
	fgets (buf, 1024, fp);
	fgets (buf, 1024, fp);

	while (fgets(buf, 1024, fp) != NULL)
	{
		ret = get_interface_name(buf, &name);
		/* filter the obc interface */
		if ((!strncmp(name, "obc", 3)) || (!strncmp(name, "lo", 2)) ||\
			(!strncmp(name, "sit", 3)) || (!strncmp(name, "pimreg", 6)))
		{
			continue;
		}
		sem_syslog_dbg("device name : %s\n", name);
		ret = set_mac_addr(name, mac_addr);
	}

	fclose(fp);
	

	return 0;
}


/*
* for business board update mac addr to files and interfaces 
* add by caojia , 20110730
*/
static int update_local_mac_info(unsigned char *mac_addr)
{
	int i = 0;
	FILE *fp;
	unsigned char local_mac_addr[6];
	char buf[1024];
	char *name;
	int ret;

	if (local_board->is_active_master)
	{
		return 0;
	}

	/* copy base mac to other positions */
	system("cp /dbm/product/base_mac /devinfo/base_mac");

	/* create local mac files */
	for (i = 0; i < 6; i++)
	{
		if (i == 5)
		{
			local_mac_addr[i] = mac_addr[i] + local_board->slot_id;
		}
		else
		{
			local_mac_addr[i] = mac_addr[i];
		}
	}
	fp = fopen("/devinfo/local_mac", "w+");
	if (fp == NULL)
	{
		sem_syslog_dbg("Open /devinfo/local_mac failed.\n");
	}
	else
	{
		fprintf(fp, "%02X%02X%02X%02X%02X%02X\n", local_mac_addr[0], local_mac_addr[1], local_mac_addr[2],\
			local_mac_addr[3], local_mac_addr[4], local_mac_addr[5]);
		fclose(fp);
	}

	system("cp /devinfo/local_mac /dbm/local_board/mac");

	/* change mac of all interfaces */

	/* open /proc/net/dev */
	fp = fopen("/proc/net/dev", "r");
	if (fp == NULL)
	{
		sem_syslog_dbg("Can't open proc file dev.\n");
		sem_syslog_dbg("Update mac address of interfaces failed.\n");
		return -1;
	}

	/* Drop header lines. */
	fgets (buf, 1024, fp);
	fgets (buf, 1024, fp);

	while (fgets(buf, 1024, fp) != NULL)
	{
		ret = get_interface_name(buf, &name);
		/* filter the obc interface */
		if ((!strncmp(name, "obc", 3)) || (!strncmp(name, "lo", 2)) ||\
			(!strncmp(name, "sit", 3)) || (!strncmp(name, "pimreg", 6)))
		{
			continue;
		}
		sem_syslog_dbg("device name : %s\n", name);
		ret = set_mac_addr(name, local_mac_addr);
	}

	fclose(fp);

	return 0;
}

/*
* for master board copy product sn to necessary positions and get local board sn to write to some files
* add by caojia, 20110809
*/
static int init_local_sn_info(void)
{
	int i = 0, ret = 0;
	int fd;
	FILE *fp;
	char tmp_sn_buffer[21];
	unsigned char temp_buffer[3] = {0x00, 0x00, 0x00};
	unsigned char product_sn[10];
	unsigned char board_sn[10];
	ax_module_sysinfo sysinfo;

	/* copy product sn file to other positions */
	system("cp /devinfo/sn /devinfo/product_sn");
	system("cp /devinfo/sn /dbm/product/sn");

	/* to get product board sn */
	fd = open("/devinfo/sn", O_RDONLY);
	if (fd < 0)
	{
		sem_syslog_dbg("open /devinfo/sn failed.\n");
		return -1;
	}

	memset(tmp_sn_buffer, 0x00, 21);
	ret = read(fd, tmp_sn_buffer, 20);

	sem_syslog_dbg("Product Sn : ");
	for (i=0; i<20; i+=2)
	{
		memcpy(temp_buffer, tmp_sn_buffer+i, 2);
		product_sn[i/2] = (unsigned char)strtoul((char *)temp_buffer, 0, 16);
		product->sn[i/2] = product_sn[i/2];
		product_info_syn.sn[i/2] = product_sn[i/2];
		sem_syslog_dbg_without_time("%02x", product_sn[i/2]);
	}
	sem_syslog_dbg_without_time("\n");

	close(fd);

	/* get local board sn */
	fd = open("/dev/bm0", 0);
	if (fd < 0)
	{
		sem_syslog_dbg("Open dev /dev/bm0 failed.\n");
		return -1;
	}

	sysinfo.slot_num = 0;
	sysinfo.product_id = 7;

	ret = ioctl(fd, BM_IOC_READ_MODULE_SYSINFO, &sysinfo);
	close(fd);
	if (ret != 0)
	{
		sem_syslog_dbg("read module info failed.\n");
	}

	sem_syslog_dbg("Module sn read from eeprom : %s\n", sysinfo.ax_sysinfo_module_serial_no);

	sem_syslog_dbg("Module Sn : ");
	for (i=0; i<20; i+=2)
	{
		memcpy(temp_buffer, sysinfo.ax_sysinfo_module_serial_no+i, 2);
		board_sn[i/2] = (unsigned char)strtoul((char *)temp_buffer, 0, 16);
		local_board->sn[i/2] = board_sn[i/2];
		sem_syslog_dbg_without_time("%02x", board_sn[i/2]);
	}
	sem_syslog_dbg_without_time("\n");

	
	fp = fopen("/devinfo/module_sn", "w+");
	if (fp == NULL)
	{
		sem_syslog_dbg("open /devinfo/module_sn failed.\n");
	}
	else
	{
		fprintf(fp, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n", board_sn[0], board_sn[1], board_sn[2],\
			board_sn[3], board_sn[4], board_sn[5], board_sn[6], board_sn[7], board_sn[8], board_sn[9]);

		fclose(fp);
	}

	system("cp /devinfo/module_sn /dbm/local_board/sn");
	
	return 0;
}


static int ax81_smu_show_system_environment(env_state_t *env_state)
{
	int i, ret = 0;
	int fd;
	int reg_data = 0;
	int master_temp1_input, master_temp2_input;
    int slave_temp1_input, slave_temp2_input;
	int intState2, intState3, power_state;
	int fan_rpm, rpmVal;
	sys_power_state_t sys_power_state;

	/* Fan RPM */
    if(sem_read_cpld(CPLD_FAN_RPM, &fan_rpm) < 0)
		return -1;
	switch(fan_rpm & 0xff)
	{
        case 0x01: 
			rpmVal = FAN_125_RPM;break;
        case 0x02: 
			rpmVal = FAN_250_RPM;break;		
        case 0x04: 
			rpmVal = FAN_375_RPM;break;			
        case 0x08: 
			rpmVal = FAN_500_RPM;break;			
        case 0x10: 
			rpmVal = FAN_625_RPM;break;
        case 0x20: 
			rpmVal = FAN_750_RPM;break;		
        case 0x40: 
			rpmVal = FAN_875_RPM;break;			
        default: 
			rpmVal = FAN_1000_RPM;
			break;				
	}
    env_state->fan_rpm = rpmVal;
    sem_syslog_dbg("\tfan_rpm = %#x\n", fan_rpm);
	
	/* Fan state */
    if(sem_read_cpld(CPLD_INTERRUPT_RESULT_REG2, &intState2) < 0)
		return -1;
	/* Fan module remove alse disposed as alarm*/
	env_state->fan_state = (((intState2 >> 2) & 1) & (((intState2 >> 1) & 1) ^ 1));
    sem_syslog_dbg("\tintState2 = %#x\n", intState2);
	
	/* Power supply */
	fd = open("/dev/bm0", 0);
	if (fd < 0)
	{
		sem_syslog_dbg("Open dev /dev/bm0 failed.\n");
		return -1;
	}

	/*wangchao add to support power*/
	if(sem_read_cpld(CPLD_INTERRUPT_RESULT_REG3, &intState3) < 0){
		close(fd);
		return -1;
	}
	memset(&sys_power_state, 0, sizeof(sys_power_state_t));
	for(i = 0; i < 4; i++)
	{
		/*wangchao add: When power module state is inserted, judged the power is "on" or "off"; 
		  when power module is not inserted ,the power state appeared "none" directly*/
    	sys_power_state.power_no = i;
		if (((intState3 >>i) & 1 ) == 1) {
			env_state->power_state &= ~(1<<(i*4));
			sem_syslog_dbg("Power module %d is NONE \n",i);
			continue;
		}
    	ret = ioctl(fd, BM_GET_POWER_STATE, &sys_power_state);
    	if (ret != 0)
    	{
    		sem_syslog_dbg("read power info failed.\n");
    	}
		env_state->power_state |= (sys_power_state.power_supply + 1)<< (i*4) ;
		
	}
	close(fd);
    sem_syslog_dbg("\tpower_state = %#x\n", power_state);
	
	/* CPU temperature */
	if ((get_num_from_file(MASTER_TEMP1_INPUT, &master_temp1_input) >= 0)
			&&(get_num_from_file(MASTER_TEMP2_INPUT, &master_temp2_input) >= 0)){

		env_state->master_inter_temp = master_temp2_input/1000;
		env_state->master_remote_temp = master_temp1_input/1000;
		
	} else if ((get_num_from_file(TMP421_MASTER_TEMP1_INPUT, &master_temp1_input) >= 0)
					&&(get_num_from_file(TMP421_MASTER_TEMP2_INPUT, &master_temp2_input) >= 0)) {

		env_state->master_inter_temp = master_temp2_input/1000;
		env_state->master_remote_temp = master_temp1_input/1000;			
		
	} else {
		
		sem_syslog_dbg("ax81_ac8c read cpu temperature error! \n");
		ret = -1;	
	}
	
	return ret;
}

board_fix_param_t ax81_smu =
{
	.board_code = AX81_CRSMU_BOARD_CODE,
	.board_type = BOARD_TYPE_AX81_SMU,
	.board_id = BOARD_SOFT_ID_AX81_SMU,
	.name = "AX81_SMU",
	.slot_id = UNKONWN_SLOT_ID,
	.is_master = IS_MASTER,
	.cpu_num = SINGLE_CPU,
	.port_num_on_panel = 2,
	.port_arr = {{CPU, ETH_GTX, 0, 0, 2, AX81_CRSMU_ETHPORT_DEFAULT_ATTR}, 
            	 {CPU, ETH_GTX, 1, 1, 1, AX81_CRSMU_ETHPORT_DEFAULT_ATTR}},
	.function_type = MASTER_BOARD,
	.obc_port_num = 1,
	.obc_port = {{CPU, 1, 12, "obc0", OBC_PORT, NOT_CSCD_PORT, 0}},
	.cscd_port_num = 0,
	.board_state = BOARD_INSERTED,
	.master_active_or_backup_state_set = NULL,
	.get_remote_board_master_state = NULL,
	.get_slot_id = NULL,
	.set_initialize_state = NULL,
	.get_initialize_state = NULL,
	.slot_reset = NULL,
	.get_cpld_version = NULL,
	.get_board_type = NULL,
	.get_product_type = NULL,
	.get_product_serial = NULL,
	.cpld_test = NULL,
	.update_local_board_state = update_local_board_state,
	.board_register = board_register,
	.syn_board_info = syn_board_info,
	.local_board_dbm_file_create = local_board_dbm_file_create,
	.reboot_self = reboot_self,
	.hard_self_reset = ax81_smu_hard_reset,
	.board_self_init = ax81_smu_init,
	.get_port_name = get_product_port_name,
	.read_eth_port_info = read_eth_port_info,
	.get_eth_port_trans_media = get_eth_port_trans_media, 
	.set_port_preferr_media = set_port_preferr_media,
	.set_port_admin_status = set_port_admin_status,
	.set_port_autoneg_status = set_port_autoneg_status,
	.set_ethport_mtu = set_ethport_mtu,
	.init_local_mac_info = init_local_mac_info,
	.update_local_mac_info = update_local_mac_info,
	.init_local_sn_info = init_local_sn_info,
	.show_system_environment = ax81_smu_show_system_environment
};


#ifdef __cplusplus
}
#endif
