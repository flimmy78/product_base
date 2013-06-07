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
#include "cvm/ethernet-ioctl.h"
#include <dbus/sem/sem_dbus_def.h>

#include "sysdef/sem_sysdef.h"
#include "sysdef/returncode.h"
#include "sem/sem_tipc.h"
#include "../../sem_eth_port.h"
#include "sem_common.h"
#include "../product_feature.h"
#include "board_feature.h"
#include "sem_eth_port_dbus.h"
#include "sem_dbus.h"
#include "config/auteware_config.h"


extern product_fix_param_t *product;
extern board_fix_param_t *local_board;


int sem_bm_phyQT2025_read(int regaddr, unsigned int location)
{
	int fd;
	int ret;
	readphy_QT RD_QT;
	
  	RD_QT.regaddr = regaddr;
  	RD_QT.location = location;
    RD_QT.val = 0;
	
    fd = open(SEM_BM_FILE_PATH, 0);
	if (fd<0)
	{
		sem_syslog_dbg("cann't open /dev/bm0, file %s is not opened\n", SEM_BM_FILE_PATH);
		return -1;
	}
	ret = ioctl(fd, BM_IOC_READ_QT2025, &RD_QT);
    //sem_syslog_dbg("QT 3.d7fd = %x\n",RD_QT.val);
	if (ret == -1)
	{
		sem_syslog_dbg("ioctl operation fail.\n");
		close(fd);
		return -1;
	}	
	ret = close(fd);
	if(ret == -1)
	{
        sem_syslog_dbg("close the SEM_BM_FILE_PATH fail!\n");
		return -1;
	}
	if(-1 == RD_QT.val)
	{
        sem_syslog_dbg("read phy QT2025 register fail!\n");
	}
	return RD_QT.val;
    
}
/**
 * Perform an MII read. Called by the generic MII routines
 *
 * @param phy_id   The MII phy id, just the portNum
 * @param location Register location to read
 * @return Result from the read or zero on failure
 */
int sem_oct_mdio_read(int port, int location)
{
	phy_read_write_t phy_read_value;
	int retval = 0;
	int fd = -1;

	memset(&phy_read_value, 0, sizeof(phy_read_write_t));
	
	phy_read_value.phy_addr = local_board->port_arr[port].phy_id;
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
int sem_oct_mdio_write(int port, int location, uint16_t value)
{
	phy_read_write_t phy_write_value;

	int retval = 0;
	int fd = -1;

	memset(&phy_write_value, 0, sizeof(phy_read_write_t));
	
	phy_write_value.phy_addr = local_board->port_arr[port].phy_id;
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


int get_ctl_fd(void)
{
	int s_errno;
	int fd = -1;

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd >= 0)
	{
		return fd;
	}
	else
	{
		sem_syslog_warning("get_ctl_fd error1: fd=%d\n", fd);
		return -1;
	}
	
	/*s_errno = errno;
	fd = socket(PF_PACKET, SOCK_DGRAM, 0);
	if (fd >= 0)
	{
		return fd;
	}
	else
	{
		sem_syslog_warning("get_ctl_fd error2: fd=%d\n", fd);
		return -1;
	}
	
	fd = socket(PF_INET6, SOCK_DGRAM, 0);
	if (fd >= 0)
	{
		return fd;
	}
	else
	{
		sem_syslog_warning("get_ctl_fd error3: fd=%d\n", fd);
		return -1;
	}
	
	errno = s_errno;
	//syslog_ax_nbm_eth_port_err("Cannot create control socket\n");
	return -1;*/
}



int set_mtu(const char *dev, int mtu)
{
	struct ifreq ifr;
	int s;

	s = get_ctl_fd();
	if (s < 0)
	{
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, dev, IFNAMSIZ); 
	ifr.ifr_mtu = mtu; 
	if (ioctl(s, SIOCSIFMTU, &ifr) < 0) {
		sem_syslog_dbg("SIOCSIFMTU IOCTL error!\n");
		close(s);
		return -1;
	}
	close(s);

	return SEM_COMMAND_SUCCESS; 
}

int get_mtu(const char *dev, unsigned int *mtu)
{
	struct ifreq ifr;
	int s;

	s = get_ctl_fd();
	if (s < 0)
	{
		sem_syslog_dbg("get_ctl_fd failed\n");
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, dev, IFNAMSIZ); 
	if (ioctl(s, SIOCGIFMTU, &ifr) < 0) {
		sem_syslog_dbg("SIOCGIFMTU IOCTL error!\n");
		close(s);
		return -1;
	}
	close(s);
	*mtu = ifr.ifr_mtu;
	return 	SEM_COMMAND_SUCCESS; 
}

int set_ethport_mtu(int slot, int port, unsigned int mtu_size)
{
	unsigned int mtu = 0;
	int ret;
	char eth_port_name[ETH_PORT_NAME_LEN_MAX];

	mtu = mtu_size - 18;	
	ret = local_board->get_port_name(slot, port, eth_port_name);
	
	if (set_mtu(eth_port_name, mtu) == 0)
	{
		return 0;
	}
	else 
	{
		return SEM_COMMAND_FAILED;
	}	
}


int get_ethport_mtu(int slot, int port, unsigned int *mtu)
{
	int ret;
	char eth_port_name[ETH_PORT_NAME_LEN_MAX];	
	
	ret = local_board->get_port_name(slot, port, eth_port_name);
	ret = get_mtu(eth_port_name, mtu);
	*mtu = *mtu + 18;
	
	return 0;
}

/*
* get interface name from buffer which read from /proc/net/dev
* caojia 20110730
*/
int get_interface_name(char *buf, char **name)
{
	char *end;

	/* Skip white space.  Line will include header spaces. */
	while (*buf == ' ')
	{
		buf++;
	}
	*name = buf;

	/* Cut interface name. */
	end = strrchr (buf, ':');
	*end++ = '\0';

	return 0;
}

int set_mac_addr(char *name, unsigned char *mac_addr)
{
	struct ifreq ifr;
	int s;
	int i = 0;

	s = get_ctl_fd();
	if (s < 0)
	{
		sem_syslog_dbg("get_ctl_fd failed\n");
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_hwaddr.sa_family = 1;
	for (i = 0; i < 6; i++)
	{
		ifr.ifr_hwaddr.sa_data[i] = (char)mac_addr[i];
	}
	if (ioctl(s, SIOCSIFHWADDR, &ifr) < 0) {
		sem_syslog_dbg("SIOCSIFHWADDR IOCTL failed!\n");
		close(s);
		return -1;
	}
	close(s);
	
	return 	0;
}


int hardware_reset_prepare(int sd, int slot_id)
{
	sem_pdu_head_t * head;
	sem_tlv_t *tlv_head;
	
	board_info_syn_t *board_info_head;
    char send_buf[SEM_TIPC_SEND_BUF_LEN];

	snapshot_before_reset();
	memset(send_buf, 0, SEM_TIPC_SEND_BUF_LEN);
	head = (sem_pdu_head_t *)send_buf;
	head->slot_id = slot_id;
	head->version = 11;
	head->length = 22;

	tlv_head = (sem_tlv_t *)(send_buf + sizeof(sem_pdu_head_t));
	tlv_head->type = SEM_RESET_READY;
	tlv_head->length = 33;

	if (sendto(g_send_sd, send_buf, SEM_TIPC_SEND_BUF_LEN, 0, (struct sockaddr*)&g_send_sock_addr, sizeof(struct sockaddr_tipc)) < 0)
	{
		sem_syslog_warning("send reset ready failed\n");
		return 1;
	}
	else
	{
		sem_syslog_warning("send reset ready succeed\n");
		return 0;
	}

	return 0;
}

int board_dbm_file_create(board_fix_param_t *board)
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
	fprintf(fd, "%d\n", board->slot_id+1);
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

	fd = fopen("/dbm/local_board/board_ap_counter", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_ap_counter failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->board_ap_counter);
	fclose(fd);

	return 0;

}


#ifdef __cplusplus
}
#endif

