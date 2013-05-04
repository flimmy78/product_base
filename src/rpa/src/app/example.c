#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEV_NAME "/dev/rpa0"

#define ETH_LEN		   16
/**
 * Command number for ioctl.
 */
#define RPA_IOC_MAGIC 245
#define RPA_IOC_MAXNR 0x16

#define RPA_IOC_REG_IF		_IOWR(RPA_IOC_MAGIC, 1, struct rpa_interface_info_s) // read values
#define RPA_IOC_UNREG_IF    _IOWR(RPA_IOC_MAGIC, 2, struct rpa_interface_info_s) // read values
#define RPA_IOC_INDEX_TABLE_ADD _IOWR(RPA_IOC_MAGIC, 3, struct cvm_rpa_dev_index_info)
#define RPA_IOC_INDEX_TABLE_DEL _IOWR(RPA_IOC_MAGIC, 4, struct cvm_rpa_dev_index_info)
#define RPA_IOC_INDEX_TABLE_MDF _IOWR(RPA_IOC_MAGIC, 5, struct cvm_rpa_dev_index_info)
#define RPA_IOC_IF_CHANGE_MTU _IOWR(RPA_IOC_MAGIC, 6, struct rpa_intf_states_update_info_s)
#define RPA_IOC_NETIF_CARRIER_SET _IOWR(RPA_IOC_MAGIC, 7, struct rpa_netif_carrier_ioc_s)
#define RPA_IOC_IF_MDF		_IOWR(RPA_IOC_MAGIC, 8, struct rpa_interface_info_s)


typedef struct rpa_interface_info_s
{
	unsigned char  if_name[ETH_LEN];
	unsigned char  mac_addr[6];
	unsigned int mtu;
	unsigned int   type;
	unsigned short slotNum;
	unsigned short netdevNum;
	unsigned short devNum;
	unsigned short dev_portNum;
}rpa_interface_info_t;

typedef struct cvm_rpa_dev_index_info
{
	int slotNum;
	int netdevNum;
	int flag;
	int ifindex;
}index_table_info;

typedef struct rpa_intf_states_update_info_s
{
	int ifindex;
	int mtu;
	int flags;
}rpa_intf_states_update_info_t;

typedef struct rpa_netif_carrier_ioc_s
{
	char if_name[ETH_LEN];
	int carrier_flag;
}rpa_netif_carrier_ioc_t;

int main(int argc, char **argv)
{
	int cmd = 0;
	int fd, ret = 0;
	int i = 0;
	int slot, netdevNum, devnum, devport;
	int type, flag, ifindex, mtu, carrier_flag;
    struct rpa_interface_info_s rpa_info;
	index_table_info dev_index_info;
	rpa_intf_states_update_info_t intf_states_update_info;
	rpa_netif_carrier_ioc_t rpa_netif_carrier_info;
	char ifname[16];
	unsigned char mac[6] = {0x00, 0x1f, 0x64, 0x11, 0x02, 0xc0};
	

	printf("select cmd : \n");
	printf("1, RPA_IOC_REG_IF\n");
	printf("2, RPA_IOC_UNREG_IF\n");
	printf("3, RPA_IOC_INDEX_TABLE_ADD\n");
	printf("4, RPA_IOC_INDEX_TABLE_DEL\n");
	printf("5, RPA_IOC_INDEX_TABLE_MDF\n");
	printf("6, RPA_IOC_IF_CHANGE_MTU\n");
	printf("7, RPA_IOC_NETIF_CARRIER_SET\n");
	printf("8, RPA_IOC_IF_MDF\n");
	printf("input cmd : ");
	scanf("%d", &cmd);

	switch (cmd)
	{
	case 1:
		printf("input if_name : ");
		scanf("%s", ifname);
		printf("input mtu : ");
		scanf("%d", &mtu);
		printf("input slot number : ");
		scanf("%d", &slot);
		printf("input netdev number : ");
		scanf("%d", &netdevNum);
		printf("input type : ");
		scanf("%d", &type);
		if (type == 0)
		{
			printf("input device number : ");
			scanf("%d", &devnum);
			printf("input device netdev number : ");
			scanf("%d", &devport);
			rpa_info.devNum = devnum;
			rpa_info.dev_portNum = devport;
		}

		strncpy(rpa_info.if_name, ifname, ETH_LEN);
		memcpy(rpa_info.mac_addr, mac, 6);
		rpa_info.mtu = mtu;
		rpa_info.slotNum = slot;
		rpa_info.netdevNum = netdevNum;
		rpa_info.type = type;
		fd = open(DEV_NAME, O_RDWR);
		if(fd == -1){
			printf("open error!\n");
			return -1;
		}
		
		ret = ioctl(fd, RPA_IOC_REG_IF, &rpa_info);
		if(ret == -1){
			printf("IOCTL error!\n");
			return -1;
		}	
		break;
	case 2:
		printf("input slot number : ");
		scanf("%d", &slot);
		printf("input netdev number : ");
		scanf("%d", &netdevNum);

		rpa_info.slotNum = slot;
		rpa_info.netdevNum = netdevNum;
		fd = open(DEV_NAME, O_RDWR);
		if(fd == -1){
			printf("open error!\n");
			return -1;
		}
		
		ret = ioctl(fd, RPA_IOC_UNREG_IF, &rpa_info);
		if(ret == -1){
			printf("IOCTL error!\n");
			return -1;
		}	
		break;
	case 3:
		printf("input slot number : ");
		scanf("%d", &slot);
		printf("input netdev number : ");
		scanf("%d", &netdevNum);
		printf("input flag : ");
		scanf("%d", &flag);
		printf("input ifindex : ");
		scanf("%d", &ifindex);

		dev_index_info.slotNum = slot;
		dev_index_info.netdevNum =netdevNum;
		dev_index_info.flag = flag;
		dev_index_info.ifindex = ifindex;
		fd = open(DEV_NAME, O_RDWR);
		if(fd == -1){
			printf("open error!\n");
			return -1;
		}
		
		ret = ioctl(fd, RPA_IOC_INDEX_TABLE_ADD, &dev_index_info);
		if(ret == -1){
			printf("IOCTL error!\n");
			return -1;
		}	
		break;
	case 4:
		printf("input slot number : ");
		scanf("%d", &slot);
		printf("input netdev number : ");
		scanf("%d", &netdevNum);

		dev_index_info.slotNum = slot;
		dev_index_info.netdevNum =netdevNum;
		fd = open(DEV_NAME, O_RDWR);
		if(fd == -1){
			printf("open error!\n");
			return -1;
		}
		
		ret = ioctl(fd, RPA_IOC_INDEX_TABLE_DEL, &dev_index_info);
		if(ret == -1){
			printf("IOCTL error!\n");
			return -1;
		}	
		break;
	case 5:
		printf("input slot number : ");
		scanf("%d", &slot);
		printf("input netdev number : ");
		scanf("%d", &netdevNum);
		printf("input flag : ");
		scanf("%d", &flag);
		printf("input ifindex : ");
		scanf("%d", &ifindex);

		dev_index_info.slotNum = slot;
		dev_index_info.netdevNum =netdevNum;
		dev_index_info.flag = flag;
		dev_index_info.ifindex = ifindex;
		fd = open(DEV_NAME, O_RDWR);
		if(fd == -1){
			printf("open error!\n");
			return -1;
		}
		
		ret = ioctl(fd, RPA_IOC_INDEX_TABLE_MDF, &dev_index_info);
		if(ret == -1){
			printf("IOCTL error!\n");
			return -1;
		}	
		break;
	case 6:
		printf("input ifindex : ");
		scanf("%d", &ifindex);
		printf("input new mtu : ");
		scanf("%d", &mtu);
		intf_states_update_info.ifindex = ifindex;
		intf_states_update_info.mtu = mtu;
		fd = open(DEV_NAME, O_RDWR);
		if(fd == -1){
			printf("open error!\n");
			return -1;
		}
		
		ret = ioctl(fd, RPA_IOC_IF_CHANGE_MTU, &intf_states_update_info);
		if(ret == -1){
			printf("IOCTL error!\n");
			return -1;
		}	
		break;
	case 7:
		printf("input if_name : ");
		scanf("%s", ifname);
		printf("input carrier_flag : ");
		scanf("%d", &carrier_flag);
		strncpy(rpa_netif_carrier_info.if_name, ifname, ETH_LEN);
		rpa_netif_carrier_info.carrier_flag = carrier_flag;
		fd = open(DEV_NAME, O_RDWR);
		if(fd == -1){
			printf("open error!\n");
			return -1;
		}
		
		ret = ioctl(fd, RPA_IOC_NETIF_CARRIER_SET, &rpa_netif_carrier_info);
		if(ret == -1){
			printf("IOCTL error!\n");
			return -1;
		}	
		break;
	case 8:
		printf("input if_name : ");
		scanf("%s", ifname);
		printf("input slot number : ");
		scanf("%d", &slot);
		printf("input netdev number : ");
		scanf("%d", &netdevNum);
		strncpy(rpa_info.if_name, ifname, ETH_LEN);
		rpa_info.slotNum = slot;
		rpa_info.netdevNum = netdevNum;
		fd = open(DEV_NAME, O_RDWR);
		if(fd == -1){
			printf("open error!\n");
			return -1;
		}
		
		ret = ioctl(fd, RPA_IOC_IF_MDF, &rpa_info);
		if(ret == -1){
			printf("IOCTL error!\n");
			return -1;
		}	
		break;
	default:
		printf("error cmd number!\n");
		return -1;
	}

	return ret;
}
