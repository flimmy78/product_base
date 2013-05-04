/*******************************************************************************
Copyright (C) Autelan Technology

This software file is owned and distributed by Autelan Technology 
********************************************************************************

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************
* nbm_eth_port.c
*
*
* CREATOR:
*		zhubo@autelan.com
*
* DESCRIPTION:
*		APIs used in NBM for broad eth -port  process.
*
* DATE:
*		06/26/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.59 $	
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <errno.h>
#include <linux/ethtool.h>
#include <linux/types.h>

#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "nbm_util.h"
#include "nbm_cpld.h" /*for some reg's addr*/
#include "nbm_eth_port.h"
#include "nbm_log.h"
#include "time.h"
#include "cvm/ethernet-ioctl.h"

static const char *eth_name_ax7000[] = 
{
	"eth0-1", "eth0-2", "eth0-3", "eth0-4"
};
static const char *eth_name_ax7000_I[] = 
{
	"eth1-1", "eth1-2", "eth1-3", "eth1-4"
};


static const char *eth_name_5612i[] = {
	"eth1-9", "eth1-10", "eth1-11", "eth1-12"
};

static const char *eth_name_ax5612e[] = 
{
	"eth1-1", "eth1-2", "eth1-3", "eth1-4", "eth1-5", "eth1-6", "eth1-7","eth1-8", 
};
static const char *eth_name_ax5608[] = 
{
	"eth1-1", "eth1-2", "eth1-3", "eth1-4",
};


static const unsigned char phy_ax5000[] = {1, 2};
static const unsigned char phy_ax5612e[] = {0, 1, 2, 3};
static const unsigned char phy_ax7000[] = {0, 1, 2, 3};

/*
 * 0x1140 means: Enable Auto-Negotiation Process, Full-duplex, 100 Mbps
 */
static unsigned short phy_ctl_copper[] = {0x1140, 0x1140, 0x1140, 0x1140};
static unsigned short phy_ctl_fiber[] = {0x1140, 0x1140, 0x1140, 0x1140};

int nbm_get_ethport_mru(unsigned char portNum);

/**
 * Perform an MII read. Called by the generic MII routines
 *
 * @param phy_id   The MII phy id, just the portNum
 * @param location Register location to read
 * @return Result from the read or zero on failure
 */
static int nbm_oct_mdio_read(int port, int location)
{
	phy_read_write_t phy_read_value;
	int retval = 0;
	int fd = -1;

	memset(&phy_read_value, 0, sizeof(phy_read_write_t));
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		if( (port >= 0) && (port <= 3) )		/* Cavium Eth-port */
		{
			phy_read_value.phy_addr = phy_ax5612e[port];
		}
		else if( (port >= 4) && (port <= 7) )		/* 82571 Eth-port */
		{			
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
		}
	}
	else if( PRODUCT_AX7000 )
	{
		phy_read_value.phy_addr = phy_ax7000[port];
	}
	else 
	{
		/*code optimize:  Out-of-bounds access
		zhangcl@autelan.com 2013-1-17*/
		if((port >= 0) && (port <= 1))
		{
			phy_read_value.phy_addr = phy_ax5000[port];
		}
		else
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n",port);
			return NPD_FAIL;
		}
	}
	
	phy_read_value.location = location;

	fd = open(DEV_NAME, 0);
	if(fd == -1)
	{
		syslog_ax_nbm_eth_port_err("open Dev(%s) error!", DEV_NAME);
		return NPD_FAIL;
	}
	
	retval = ioctl(fd, CVM_IOC_PHY_READ, &phy_read_value);
	if(retval == -1)
	{
		syslog_ax_nbm_eth_port_err("read failed\n");
		close(fd);
		return NPD_FAIL;
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
static int nbm_oct_mdio_write(int port, int location, uint16_t value)
{
	phy_read_write_t phy_write_value;

	int retval = 0;
	int fd = -1;

	memset(&phy_write_value, 0, sizeof(phy_read_write_t));
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		phy_write_value.phy_addr = phy_ax5612e[port];
	}
	else if( PRODUCT_AX7000 )
	{
		phy_write_value.phy_addr = phy_ax7000[port];
	}
	else
	{
		/*code optimize:  Out-of-bounds access
		zhangcl@autelan.com 2013-1-17*/
		if((port >= 0) && (port <= 1))
		{
			phy_write_value.phy_addr = phy_ax5000[port];
		}
		else
		{
			syslog_ax_nbm_eth_port_err("Input port %d Error !\n",port);
			return NPD_FAIL;
		}
	}
	
	phy_write_value.location = location;
	phy_write_value.write_reg_val = value;
	
	fd = open(DEV_NAME, 0);
	if(fd == -1)
	{
		syslog_ax_nbm_eth_port_err("open Dev %s error!", DEV_NAME);
		return NPD_FAIL;
	}
	
	retval = ioctl(fd, CVM_IOC_PHY_WRITE, &phy_write_value);
	if(retval == -1)
	{
		syslog_ax_nbm_eth_port_err("read failed\n");
		close(fd);
		return NPD_FAIL;
	}
	close(fd);
	
	return NPD_SUCCESS;
}

static int nbm_oct_check_media_and_change_page(unsigned char port)
{
	unsigned short reg_val;
	int media;
	
	/*
	 * read reg 26 bit10~bit11 to determine which media is selected: copper or fiber 
	 * 11:10	Autoselect preferred media
	 *	00 = No Preference for Media
	 *	01 = Preferred Fiber Medium
	 *	10 = Preferred Copper Medium
	 *	11 = Reserved
	 */
	reg_val = nbm_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2);
	media = ((reg_val >> 10) & 0x3);
	if(media == 0x0)			/* No Preference for Media */
	{
		/*
		 * set page for copper meidia
		 */
		nbm_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 0);			
	}
	else if(media == 0x1)		/* Preferred Fiber */
	{		
		/* 
		 * set page for fiber media 
		 */
		nbm_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 1);
	}
	else if(media == 0x2)	/* Preferred Copper */
	{
		/*
		 * set page for copper meidia
		 */
		nbm_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 0);			
	}
	else 					/* Reserved */
	{
		nbm_oct_mdio_write(port, MV_88E1145_PHY_EXT_ADDR, 0);			
		media = 0;
	}

	return media;
}

int nbm_eth_interface_down(int fd, struct ifreq *ifr)
{
	syslog_ax_nbm_eth_port_dbg("interface %s will be down\n", ifr->ifr_name);
	
    if (ioctl(fd, SIOCGIFFLAGS, ifr) < 0) 
	{
		syslog_ax_nbm_eth_port_err("ioctl error! %s (%d)\n", __FUNCTION__, __LINE__);
		return NPD_FAIL;
    }
	
    ifr->ifr_flags &= (~IFF_UP);
    if (ioctl(fd, SIOCSIFFLAGS, ifr) < 0) 
	{
		syslog_ax_nbm_eth_port_err("SIOCSIFFLAGS");
		return NPD_FAIL;
    }

	return NPD_SUCCESS;
}

int nbm_eth_interface_up(int fd, struct ifreq *ifr)
{
	syslog_ax_nbm_eth_port_err("interface %s will be up\n", ifr->ifr_name);

	if (ioctl(fd, SIOCGIFFLAGS, ifr) < 0) 
	{
		syslog_ax_nbm_eth_port_err("ioctl error! %s (%d)\n", __FUNCTION__, __LINE__);
		return NPD_FAIL;
    }
	
    ifr->ifr_flags |= (IFF_UP | IFF_RUNNING);
    if (ioctl(fd, SIOCSIFFLAGS, ifr) < 0) 
	{
		syslog_ax_nbm_eth_port_err("SIOCSIFFLAGS");
		return NPD_FAIL;
    }
	
	return NPD_SUCCESS;
}

int nbm_e1000e_set_eth_port_media(unsigned int port, unsigned int media)
{
	int fd;
	int err;
	struct ethtool_regs *regs;
	struct ifreq ifr;
	
	syslog_ax_nbm_eth_port_dbg("Will chang media type to %s media == %d\n",
					(media == 2 ? "Copper":(media == 1 ? "Fiber":"error media type")), media);
	
	memset(&ifr, 0, sizeof(ifr));
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5612e[port]);
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		/*code optimize:  Out-of-bounds access
		zhangcl@autelan.com 2013-1-17*/
		if(port<=3)
		{
			strcpy(ifr.ifr_name, eth_name_ax5608[port]);
		}
		else
		{
			syslog_ax_nbm_eth_port_err("Input port %d Error !\n",port);
			return NPD_FAIL;
		}
	}
	
	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) 
	{
		syslog_ax_nbm_eth_port_err("Open socket failed!\n");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	if(media == 1)		/* will change to fiber */
	{
		nbm_eth_interface_down(fd, &ifr);			
	}
	
	regs = calloc(1, sizeof(*regs) + 4);
	if (!regs) 
	{
		syslog_ax_nbm_eth_port_err("calloc memory failed!\n");
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}	
		
	regs->cmd = 0x0000003C;	/* THTOOL_CMEDIA */
	regs->len = media; 		/* Here, we use the len field as the media type */
	ifr.ifr_data = regs;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if (err < 0) 
	{
		syslog_ax_nbm_eth_port_err("Cannot read register err == %d\n", err);
		free(regs);
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	
#define MEDIA_CHANGE_SUCCESS		0
#define MEDIA_NO_NEED_TO_CHANGE		1
#define MEDIA_CHANGE_ERROR			2
	
	switch(*((unsigned int *)((unsigned char *)regs + (sizeof(*regs)))))
	{
		case MEDIA_CHANGE_SUCCESS:
		case MEDIA_NO_NEED_TO_CHANGE:	
			syslog_ax_nbm_eth_port_dbg("Chang media type to %s successful.\n",
				(media == 2 ? "Copper":(media == 1 ? "Fiber":"error media type")));
			err=  NPD_SUCCESS;
			break;
		case MEDIA_CHANGE_ERROR:			
			syslog_ax_nbm_eth_port_err("Change media type error!\n");
			err = ETHPORT_RETURN_CODE_ERR_HW;
			break;
		default:			
			syslog_ax_nbm_eth_port_err("Change media type error!\n");
			err = ETHPORT_RETURN_CODE_ERR_HW;
	}
		
	free(regs);

	if(media == 2)
	{
		nbm_eth_interface_down(fd, &ifr);	
	}
	
	nbm_eth_interface_up(fd, &ifr);

	close(fd);
	
	return err;
}

/*******************************************************************************
* nbm_set_eth_port_trans_media
*
* DESCRIPTION:
*      Set media auto-selection preferred media(or say media priority)
*	 irrelevant when port PHY type is not combo(or say Media Auto-Selection Interface)
*
* INPUTS:
*       port   - physical port number on board
*       media   - preferred auto-selection interface type (copper or fiber)
*			0 - none    COMBO_PHY_MEDIA_PREFER_NONE
*			1 - fiber media interface  COMBO_PHY_MEDIA_PREFER_FIBER
*			2 - copper media interface  COMBO_PHY_MEDIA_PREFER_COPPER
*
* RETURNS:
*      0	- on success
*      1  	- on hardware error or parameters error 
*      6 	- for non-combo ports not support this operation
*******************************************************************************/
int nbm_oct_set_eth_port_media
(
	unsigned int port,
	unsigned int media
)
{
	int read_value;
	int status;
	cvmx_mdio_phy1145_reg26  reg_value;

	if (media == 0x1)			/* select fiber */
	{
		/* read phy register 27 and set bit0~bit3 */
		read_value = nbm_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_STATUS);

		/* set mode 0011b rgmii to fiber */
		reg_value.u16 = read_value & 0xfff0;		
		reg_value.u16 |= 0x3;
		
		/* disable fiber/copper auto-selection */
		reg_value.u16 |= (1<<15);
		status = nbm_oct_mdio_write(port, MV_88E1145_PHY_EXT_SPEC_STATUS, reg_value.u16);
		if (0 != status)
		{
			syslog_ax_nbm_eth_port_err("set board port(%d) media to fiber error\n", port);
			return ETHPORT_RETURN_CODE_ERR_HW;
		}
		
		/* software reset to take effect disable fiber/copper auto-selection */
		read_value = nbm_oct_mdio_read(port, 0x00);
		reg_value.u16 = read_value & 0xffff;
		reg_value.u16 |= (1<<15);

		status = nbm_oct_mdio_write(port, 0x00, reg_value.u16);
		if (0 != status)
		{
			syslog_ax_nbm_eth_port_err("fiber media select failed:software reset error\n");
			return ETHPORT_RETURN_CODE_ERR_HW;
		}
		
		/* set register 26 Autoselect preferred media:select fiber */
		read_value = nbm_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2);
		reg_value.u16 = read_value & 0xffff;
		reg_value.u16 |= (1<<10);
		reg_value.u16 &= (~(1<<11));
		status = nbm_oct_mdio_write(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2, reg_value.u16);
		if (0 != status)
		{
			syslog_ax_nbm_eth_port_err("preferred select failed:set board port(%d) media to fiber error\n", port);
			return ETHPORT_RETURN_CODE_ERR_HW;
		}
		
	}	
	else if (media == 0x2)			/* select copper */
	{
		/* read phy register 27 and set bit0~bit3 */
		read_value = nbm_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_STATUS);
		reg_value.u16 = read_value & 0xfff0;
		
		/* set mode 1011b rgmii/modified mii to copper */
		reg_value.u16 |= 0xb;
		
		/* disable fiber/copper auto-selection */
		reg_value.u16 |= (1<<15);
		
		status = nbm_oct_mdio_write(port, MV_88E1145_PHY_EXT_SPEC_STATUS, reg_value.u16);
		if (0 != status)
		{
			syslog_ax_nbm_eth_port_err("set board port(%d) media to copper error\n", port);
			return ETHPORT_RETURN_CODE_ERR_HW;
		}
		
		/* software reset to take effect disable fiber/copper auto-selection */
		read_value = nbm_oct_mdio_read(port, 0x00);
		reg_value.u16 = read_value & 0xffff;
		reg_value.u16 |= (1<<15);

		status = nbm_oct_mdio_write(port, 0x00, reg_value.u16);
		if (0 != status)
		{
			syslog_ax_nbm_eth_port_err("copper media select failed:software reset error\n");
			return ETHPORT_RETURN_CODE_ERR_HW;
		}

		/* set register 26 Autoselect preferred media: select copper */
		read_value = nbm_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2);
		reg_value.u16 = read_value & 0xffff;
		reg_value.u16 |= (1<<11);
		reg_value.u16 &= (~(1<<10));
		status = nbm_oct_mdio_write(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2, reg_value.u16);
		if (0 != status)
		{
			syslog_ax_nbm_eth_port_err("set board port(%d) media to fiber error\n", port);
			return ETHPORT_RETURN_CODE_ERR_HW;
		}
	}	
	else		/* other media */		
	{
		syslog_ax_nbm_eth_port_err("Error:set board port(%d) media to a unknown media\n", port);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	/* reset to take effect the setting */
	read_value = nbm_oct_mdio_read(port, 0x00);
	reg_value.u16 = read_value & 0xffff;
	reg_value.u16 |= (1<<15);
	status = nbm_oct_mdio_write(port, 0x00, reg_value.u16);
	if (0 != status)
	{
		syslog_ax_nbm_eth_port_err("set board port(%d) media to %s error\n", port, (media == 0x1 ? "fiber" : (media == 0x2 ? "copper" : "unknown")));
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	return 0; /* success */
}


int nbm_set_eth_port_trans_media(unsigned int port, unsigned int media)
{
	int ret;
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		/*code optimize:  Unsigned compared against 0
		zhangcl@autelan.com 2013-1-17*/
		if(port > 7)
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if(port <= 3)		/* Cavium Eth-port */
		{				
			ret = ETHPORT_RETURN_CODE_UNSUPPORT;
		}
		else if( (port >= 4) && (port <= 7) )		/* 82571 Eth-port */
		{
			ret = nbm_e1000e_set_eth_port_media(port, media);
		}
	}
	else if(PRODUCT_TYPE_5608 == nbm_get_product_type())
	{
		if( port > 3)
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3)		/* 82571 Eth-port */
		{
			ret = nbm_e1000e_set_eth_port_media(port, media);
		}
	}
	else if( PRODUCT_AX7000 )
	{
		if(port > 3)
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else
		{			
			ret = nbm_oct_set_eth_port_media(port, media);
		}
	}
	else
	{
		ret = ETHPORT_RETURN_CODE_ERR_PRODUCT_TYPE;
	}

	return ret;
}


int nbm_e1000e_get_eth_port_media(unsigned int port, unsigned int* media)
{	
	int fd;
	int err;
	struct ethtool_drvinfo drvinfo;	
	struct ethtool_regs *regs;
	struct ifreq ifr;
	
	memset(&ifr, 0, sizeof(ifr));
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5612e[port]);
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		/*code optimize:  Out-of-bounds access
		zhangcl@autelan.com 2013-1-17*/
		if(port <=3)
		{
			strcpy(ifr.ifr_name, eth_name_ax5608[port]);
		}
		else
		{
			syslog_ax_nbm_eth_port_err("Input port %d Error !\n",port);
			return NPD_FAIL;
		}
	}
	
	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) 
	{
		syslog_ax_nbm_eth_port_err("Open socket failed!\n");
		return 1;
	}
	
	regs = calloc(1, sizeof(*regs));
	if (!regs) 
	{
		syslog_ax_nbm_eth_port_err("calloc memory failed!\n");
		close(fd);
		return 1;
	}
	
	regs->cmd = 0x0000003E;		/* ETHTOOL_GMEDIA */

	ifr.ifr_data = regs;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if (err < 0) 
	{
		syslog_ax_nbm_eth_port_err("ioctl failed!\n");		
		close(fd);
		return 1;
	}

	*media = regs->len;
	
	syslog_ax_nbm_eth_port_dbg("port == %d media == %s\n", port, (*media) ? "fiber" : "copper");

	free(regs);	
	close(fd);

	return NPD_SUCCESS;	
}



/*******************************************************************************
* nbm_get_eth_port_trans_media
*
* DESCRIPTION:
*      Get media auto-selection preferred media(or say media priority)
*	 irrelevant when port PHY type is not combo(or say Media Auto-Selection Interface)
*
* INPUTS:
*       port   - physical port number
*
* OUTPUTS:
*       media   - preferred auto-selection interface type (copper or fiber)
*			0 - none  COMBO_PHY_MEDIA_PREFER_NONE
*			1 - fiber media interface  COMBO_PHY_MEDIA_PREFER_FIBER
*			2 - copper media interface  COMBO_PHY_MEDIA_PREFER_COPPER
*
* RETURNS:
*      0	- on success
*      1  	- on hardware error or parameters error 
*      6 	- for non-combo ports not support this operation
*******************************************************************************/
int nbm_oct_get_eth_port_media
(
	unsigned int port,
	unsigned int* media
)
{
	int read_value;
	cvmx_mdio_phy1145_reg26  reg_value;
	

	read_value = nbm_oct_mdio_read(port, MV_88E1145_PHY_EXT_SPEC_CNTRL2);

    reg_value.u16 = read_value & 0xffff;
	*media = reg_value.s.preferred_media; /* return value */
	
	/* if media is not fiber or copper */
	if (*media != 0x1 && *media != 0x2)
	{
		syslog_ax_nbm_eth_port_dbg("get board port(%d) media auto-select prefer media error\n", port);
		return ETHPORT_RETURN_CODE_ERR_HW; /* read failed */
	}
	
	return 0;
}

int nbm_get_eth_port_trans_media(unsigned int port, unsigned int *media)
{
	int ret = NPD_SUCCESS;
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		/*code optimize:  Unsigned compared against 0
		zhangcl@autelan.com 2013-1-17*/
		if( port > 7)
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3 )		/* Cavium Eth-port */
		{				
			*media = 2;								/* Copper */
		}
		else if( (port >= 4) && (port <= 7) )		/* 82571 Eth-port */
		{
			if(nbm_e1000e_get_eth_port_media(port, media))
				ret = ETHPORT_RETURN_CODE_ERR_HW;
		}
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		if( port > 3 )
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3 )		/* 82571 Eth-port */
		{
			if(nbm_e1000e_get_eth_port_media(port, media))
				ret = ETHPORT_RETURN_CODE_ERR_HW;
		}
	}
	else if( PRODUCT_AX7000 )
	{
		if (port > 3)
		{
			syslog_ax_nbm_eth_port_err("Port number is Error!\n");
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else 
		{
			if(nbm_oct_get_eth_port_media(port, media))
				ret = ETHPORT_RETURN_CODE_ERR_GENERAL;
		}
	}
	else
	{
		ret = ETHPORT_RETURN_CODE_ERR_PRODUCT_TYPE;
	}

	return ret;
}

static int nbm_e1000e_get_ethport_enable(unsigned char port, unsigned long *enable)
{
	int fd;
	int err;
	struct ethtool_drvinfo drvinfo;	
	struct ethtool_regs *regs;
	struct ifreq ifr;
	
	memset(&ifr, 0, sizeof(ifr));
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5612e[port]);
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5608[port]);
	}
	
	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) 
	{
		syslog_ax_nbm_eth_port_err("Open socket failed!\n");
		return NPD_FAIL;
	}
	
	regs = calloc(1, sizeof(*regs));
	if (!regs)
	{
		syslog_ax_nbm_eth_port_err("calloc failed!\n");
		close(fd);
		return NPD_FAIL;
	}
	
	regs->cmd = 0x00000040;		/* ETHTOOL_GADMIN */

	ifr.ifr_data = regs;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if (err < 0)
	{
		syslog_ax_nbm_eth_port_err("ioctl failed!\n");
		close(fd);
		return NPD_FAIL;
	}

	*enable = regs->len;

	free(regs);
	close(fd);

	return NPD_SUCCESS;
}

int nbm_e1000e_ethtool_get_settings(int fd, unsigned char port, struct eth_port_s *portInfo)
{
	int err;
	struct ethtool_cmd ecmd;
	struct ethtool_value edata;
	struct ifreq ifr;
	
	memset(&ifr, 0, sizeof(ifr));
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5612e[port]);
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5608[port]);
	}

	ecmd.cmd = 0x00000001;	/* ETHTOOL_GSET */
	ifr.ifr_data = &ecmd;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if (err == 0) 
	{
		int speed = ecmd.speed;		
		if (speed == 0 || speed == (unsigned short)(-1) || speed == (unsigned int)(-1))
		{			
			syslog_ax_nbm_eth_port_err("Get port %d speed failed!\n", port);
		}
		else
		{
			portInfo->attr_bitmap &= ~ETH_ATTR_SPEED_MASK;
			if(speed == 10)
			{
				portInfo->attr_bitmap |= ETH_ATTR_SPEED_10M << ETH_SPEED_BIT;
			}
			else if(speed == 100)
			{
				portInfo->attr_bitmap |= ETH_ATTR_SPEED_100M << ETH_SPEED_BIT;
			}			
			else if (speed == 1000) 
			{
				portInfo->attr_bitmap |= ETH_ATTR_SPEED_1000M << ETH_SPEED_BIT;
			}			
		}
		
		switch (ecmd.duplex)
		{			
			case 0x00: 		/* DUPLEX_HALF */
				portInfo->attr_bitmap |= ETH_ATTR_DUPLEX_HALF << ETH_DUPLEX_BIT;
				break;
			case 0x01:		/* DUPLEX_FULL */
				portInfo->attr_bitmap |= ETH_ATTR_DUPLEX_FULL << ETH_DUPLEX_BIT;
				break;
			default:
				break;
		};

		switch (ecmd.port) 
		{
			case 0x00:		/* PORT_TP: Twisted Pair */
				portInfo->port_type = ETH_GTX;		/* GE copper */	
				portInfo->attr_bitmap |= (1 << ETH_PREFERRED_COPPER_MEDIA_BIT);
				break;
			case 0x03:		/* PORT_FIBRE */
				portInfo->port_type = ETH_GE_FIBER;	/* GE fiber */
				portInfo->attr_bitmap |= ((1 << ETH_PREFERRED_FIBER_MEDIA_BIT));
				break;
			default:
				break;
		};

		if(ecmd.autoneg)		/* autoneg enable */
		{
			portInfo->attr_bitmap |= ETH_ATTR_ON << ETH_AUTONEG_BIT;
			portInfo->attr_bitmap |= ETH_ATTR_ON << ETH_AUTONEG_CTRL_BIT;
			portInfo->attr_bitmap |= ETH_ATTR_AUTONEG_DUPLEX_ENABLE << ETH_AUTONEG_DUPLEX_BIT;
			portInfo->attr_bitmap |= ETH_ATTR_AUTONEG_SPEEDON << ETH_AUTONEG_SPEED_BIT;
		}
		else
		{
			portInfo->attr_bitmap &= ~(ETH_ATTR_ON << ETH_AUTONEG_BIT);
			portInfo->attr_bitmap &= ~(ETH_ATTR_ON << ETH_AUTONEG_CTRL_BIT);
			portInfo->attr_bitmap &= ~(ETH_ATTR_AUTONEG_DUPLEX_ENABLE << ETH_AUTONEG_DUPLEX_BIT);
			portInfo->attr_bitmap &= ~(ETH_ATTR_AUTONEG_SPEEDON << ETH_AUTONEG_SPEED_BIT);
		}

	} 
	else 
	{		
		syslog_ax_nbm_eth_port_err("Open socket failed!\n", port);
	}

	edata.cmd = 0x0000000A;		/* ETHTOOL_GLINK */
	ifr.ifr_data = &edata;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if (err == 0) 
	{
		if(edata.data)
		{		
			portInfo->attr_bitmap |= ETH_ATTR_LINKUP << ETH_LINK_STATUS_BIT;
		}
		else
		{
			portInfo->attr_bitmap &= ~(ETH_ATTR_LINKUP << ETH_LINK_STATUS_BIT);
		}
	} 
	else 
	{
		syslog_ax_nbm_eth_port_err("Open socket failed!\n", port);
	}

	syslog_ax_nbm_eth_port_dbg("portInfo->attr_bitmap == 0x%x\n", portInfo->attr_bitmap);

	return NPD_SUCCESS;
}

int nbm_e1000e_read_eth_port_info(unsigned char port, struct eth_port_s *portInfo)
{
	int fd;
	unsigned long enable;

	portInfo->attr_bitmap = 0;
	
	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) 
	{
		syslog_ax_nbm_eth_port_err("Open socket failed!\n");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}	

    portInfo->attr_bitmap |= ETH_ATTR_AUTONEG;
	nbm_e1000e_get_ethport_enable(port, &enable);	
	if(enable)
	{
		portInfo->attr_bitmap |= ETH_ATTR_ENABLE << ETH_ADMIN_STATUS_BIT; 		
	}
	else
	{
		portInfo->attr_bitmap |= ETH_ATTR_DISABLE << ETH_ADMIN_STATUS_BIT;
	}	
	syslog_ax_nbm_eth_port_dbg("port %d admin %s\n", port, (enable ? "enable":"disable"));

	nbm_e1000e_ethtool_get_settings(fd, port, portInfo);
		
	/* get MTU */
	portInfo->mtu = nbm_get_ethport_mru(port);

	close(fd);

	return NPD_SUCCESS;
}


/**********************************************************************************
 * nbm_read_eth_port_info
 *
 * read ethernet port from cavium-ethernet driver
 *
 *	INPUT:
 *		portNum - port number
 *		portInfo - ethernet port info structure.
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred.
 *		NPD_FAIL - if error occurred.
 *
 *	NOTE:
 *		Here port number is viewed as a port index, so 0 is legal.
 *
 **********************************************************************************/
int nbm_oct_read_eth_port_info
(
	unsigned char 		portNum,
	struct eth_port_s 	*portInfo
)
{
	int retval = NPD_SUCCESS;
	int fd = 0;
	unsigned short reg_value = 0;
	unsigned short ctl_value = 0;
	unsigned long tmp = 0;
	int type = 0;
	int ret = 0,media = 0;
	
	phy_reg_17_dat fiber_stat;
	phy_reg_17_dat copper_stat;
	cvmx_mdio_phy1145_reg0_1 fiber_ctl;
	cvmx_mdio_phy1145_reg0_0 copper_ctl;

	/* some param for read FC and BP status, add by baolc */
    bm_op_rbit64_args  frm_ctl_ioctl_args = {0};
    bm_op_rbit64_args  ovr_bp_ioctl_args = {0};
	bm_op_rbit64_args  bp_page_cnt_args = {0};;
    cvmx_gmxx_rxx_frm_ctl_t  frm_ctl_reg;
    cvmx_gmxx_tx_ovr_bp_t    ovr_bp_reg;
    cvmx_ipd_portx_bp_page_cnt_t  bp_page_cnt_reg;
	
	/* preferred_copper:0 preferred select is not copper or yes */
	int preferred_copper;
	
	/* preferred_fiber:0 preferred select is not fiber or yes */
	int preferred_fiber;
	

	media = nbm_oct_check_media_and_change_page(portNum);
	if (media == 0x0 || media == 0x2)
	{
		syslog_ax_nbm_eth_port_dbg("port %d media is copper\n", portNum);
		
		/* preferred copper not fiber */
		preferred_copper = 1;
		preferred_fiber = 0;
		
		/* get copper state of duplex ,speed and link */
		copper_stat.u16= nbm_oct_mdio_read(portNum, MV_88E1145_PHY_SPEC_STATUS);
		
		/* get control state */
		copper_ctl.u16 = nbm_oct_mdio_read(portNum, 0);	
		reg_value = copper_stat.u16;
		ctl_value = copper_ctl.u16;
		if(copper_ctl.u16 == 0xffff)
		{
			copper_ctl.u16 = phy_ctl_copper[portNum];
		}
		else
		{
			phy_ctl_copper[portNum] = copper_ctl.u16;
		}

	}
	else if (media == 0x1)			/* fiber */
	{
		syslog_ax_nbm_eth_port_dbg("port %d preferred media is fiber\n", portNum);

		/* preferred fiber not copper */
		preferred_fiber = 1;
		preferred_copper = 0;		
		
		/* get fiber state of duplex ,speed and link */
		fiber_stat.u16 = nbm_oct_mdio_read(portNum, MV_88E1145_PHY_SPEC_STATUS);

		/* get control state */
		fiber_ctl.u16 = nbm_oct_mdio_read(portNum, 0);
		
		reg_value = fiber_stat.u16;
		ctl_value = fiber_ctl.u16;
		if(fiber_ctl.u16 == 0xffff)
		{
			fiber_ctl.u16 = phy_ctl_fiber[portNum];
		}
		else
		{
			phy_ctl_fiber[portNum] = fiber_ctl.u16;
		}
	}	
	else						/* not preferred copper or fiber */
	{
		syslog_ax_nbm_eth_port_dbg("Error:select a wrong media.\n");
		portInfo->attr_bitmap = 0;
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	if (preferred_copper)
	{
		tmp |= ((1 << ETH_PREFERRED_COPPER_MEDIA_BIT));
	}
	else
	{
		tmp |= ((1 << ETH_PREFERRED_FIBER_MEDIA_BIT));	
	}

	if(copper_ctl.s.auto_negotiaton_en || fiber_ctl.s.auto_negotiaton_en) 
    {
		syslog_ax_nbm_eth_port_err("AUTONEG enable\n"); 
	    tmp |= ETH_ATTR_ON << ETH_AUTONEG_SPEED_BIT;
	    tmp |= ETH_ATTR_ON << ETH_AUTONEG_DUPLEX_BIT; 
        tmp |= ETH_ATTR_ON << ETH_AUTONEG_CTRL_BIT;
	    tmp |= ETH_ATTR_ON << ETH_AUTONEG_BIT;
    }
	else
	{
		syslog_ax_nbm_eth_port_err("AUTONEG disable\n"); 
        tmp |= ETH_ATTR_OFF << ETH_AUTONEG_SPEED_BIT;
	    tmp |= ETH_ATTR_OFF << ETH_AUTONEG_DUPLEX_BIT; 
        tmp |= ETH_ATTR_OFF << ETH_AUTONEG_CTRL_BIT;
	    tmp |= ETH_ATTR_OFF << ETH_AUTONEG_BIT;
	}
	/* get MTU */
	portInfo->mtu = nbm_get_ethport_mru(portNum);


	/* get port type */
	if(PRODUCT_AX7000)
	{
		type = nbm_oct_mdio_read(portNum, MV_88E1145_PHY_EXT_SPEC_STATUS);
	}
	else
	{
		if(AX51_SFP_TYPE == cvm_query_card_type(portNum))
		{
			type = 1ul << 13;
		}
	}
	
	
	/* speed */
	syslog_ax_nbm_eth_port_err("reg17's value = 0x%x\n", reg_value);
	/* 10M copper */
    if ((((reg_value & 0xc000)>>14) == 0) && preferred_copper) 
	{
		syslog_ax_nbm_eth_port_err("copper 10M\n");
    	tmp |= ETH_ATTR_SPEED_10M << ETH_SPEED_BIT;
		portInfo->port_type = ETH_GTX;
    }
	/*100M copper*/
    else if ((((reg_value&0xc000)>>14) == 1) && preferred_copper) 
	{
		syslog_ax_nbm_eth_port_err("copper 100M\n");
    	tmp |= ETH_ATTR_SPEED_100M << ETH_SPEED_BIT;
		portInfo->port_type = ETH_GTX;
    }
	/*1000M copper or fiber*/
    else if (((reg_value&0xc000)>>14) == 2) 
	{
    	tmp |= ETH_ATTR_SPEED_1000M << ETH_SPEED_BIT;
		/*for 1000M firber*/
		if(preferred_fiber) 
		{	  	
		  	portInfo->port_type = ETH_GE_SFP;
			syslog_ax_nbm_eth_port_err("fiber 1000M\n");
		}
		/*for 1000M copper*/
		else
		{
			portInfo->port_type = ETH_GTX;
			syslog_ax_nbm_eth_port_err("copper 1000M\n");
		}
    }
    else 
	{
		syslog_ax_nbm_eth_port_err("Error:wrong speed\n");    
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
		if(copper_stat.s.duplex == NPD_DUPLEX_FULL)
		{
			syslog_ax_nbm_eth_port_dbg(" full duplex mode\n");
			tmp |= ETH_ATTR_DUPLEX_FULL << ETH_DUPLEX_BIT;
		}
		else
		{
			syslog_ax_nbm_eth_port_dbg(" half duplex mode\n");
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
		if(fiber_stat.s.duplex == NPD_DUPLEX_FULL)
		{	
			syslog_ax_nbm_eth_port_dbg(" full duplex mode\n");
			tmp |= ETH_ATTR_DUPLEX_FULL << ETH_DUPLEX_BIT;
		}
		else
		{
			syslog_ax_nbm_eth_port_dbg(" half duplex mode\n");
			tmp |= ETH_ATTR_DUPLEX_HALF << ETH_DUPLEX_BIT;
		}


		/*admin status*/
		if(fiber_ctl.s.power_down)
		{
			tmp |= ETH_ATTR_DISABLE << ETH_ADMIN_STATUS_BIT;
			syslog_ax_nbm_eth_port_dbg("fiber_stat = ETH_ATTR_DISABLE");
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
	if(PRODUCT_AX7000)
	{
		frm_ctl_ioctl_args.regAddr = CVMX_GMXX_RXX_FRM_CTL(portNum, 1);
	    ovr_bp_ioctl_args.regAddr = CVMX_GMXX_TX_OVR_BP(1);
		bp_page_cnt_args.regAddr = CVMX_IPD_PORTX_BP_PAGE_CNT(portNum + 16);
	}
	else
	{
		frm_ctl_ioctl_args.regAddr = CVMX_GMXX_RXX_FRM_CTL(portNum, 0);
	    ovr_bp_ioctl_args.regAddr = CVMX_GMXX_TX_OVR_BP(0);
		bp_page_cnt_args.regAddr = CVMX_IPD_PORTX_BP_PAGE_CNT(portNum);
	}
	
	fd = open("/dev/bm0",0);
	if(fd < 0) {
		syslog_ax_nbm_eth_port_err("open dev %s error(%d) when read eth port info!\n","/dev/bm0",fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	retval = ioctl(fd, BM_IOC_BIT64_REG_STATE, &frm_ctl_ioctl_args);
	if(retval == -1){
		syslog_ax_nbm_eth_port_err("get port frm_ctl error! portNum: %d\n",portNum);
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	retval = ioctl(fd, BM_IOC_BIT64_REG_STATE, &ovr_bp_ioctl_args);
	if(retval == -1){
		syslog_ax_nbm_eth_port_err("get ovr_bp error! portNum: %d\n", portNum);
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	retval = ioctl(fd, BM_IOC_BIT64_REG_STATE, &bp_page_cnt_args);
	if(retval == -1){
		syslog_ax_nbm_eth_port_err("get bp_page_cnt error! portNum: %d\n", portNum);
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	frm_ctl_reg.u64 = frm_ctl_ioctl_args.regData;
    ovr_bp_reg.u64 = ovr_bp_ioctl_args.regData;
    bp_page_cnt_reg.u64 = bp_page_cnt_args.regData;
	
	close(fd);
	
    /* FC and BP status, add by baolc */
    if (frm_ctl_reg.s.ctl_bck == 1
		&& frm_ctl_reg.s.ctl_drp == 1
		&& (ovr_bp_reg.s.en & (0x1<<portNum)) != 0 
		&& (ovr_bp_reg.s.bp & (0x1<<portNum)) != 0)
    {
		/* FC is enable */
		/*syslog_ax_nbm_eth_port_dbg("port FC status is enabled! portNum: %d\n", portNum);*/
		tmp |= (ETH_ATTR_ENABLE << ETH_FLOWCTRL_BIT);
    }
	else if (frm_ctl_reg.s.ctl_bck == 1
		&& frm_ctl_reg.s.ctl_drp == 0
		&& (ovr_bp_reg.s.en & (0x1<<portNum)) != 0 
		&& (ovr_bp_reg.s.bp & (0x1<<portNum)) == 0)
	{
		/* FC is disable, do nothing */
		/*syslog_ax_nbm_eth_port_dbg("port FC status is disabled! portNum: %d\n", portNum);*/
	}
	else 
	{
		/* error status */
		/*syslog_ax_nbm_eth_port_dbg("port FC status incorrect! FC is disabled! portNum: %d\n", portNum);*/
	}

	if (bp_page_cnt_reg.s.bp_enb == 1)
	{
		/* BP is enable */
		/*syslog_ax_nbm_eth_port_dbg("port BP status is enabled! portNum: %d\n", portNum);*/
		tmp |= (ETH_ATTR_ENABLE << ETH_BACKPRESSURE_BIT);
	}
	else
	{
		/* BP is disable, do nothing */
		/*syslog_ax_nbm_eth_port_dbg("port BP status is disabled! portNum: %d\n", portNum);*/
	}
	
	/* out param */
	portInfo->attr_bitmap = tmp;
	return retval;
}

int nbm_read_eth_port_info(unsigned char port, struct eth_port_s *port_info)
{
	int ret;
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		/*code optimize:  Unsigned compared against 0
		zhangcl@autelan.com 2013-1-17*/
		if( port > 7 )
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3 )		/* Cavium Eth-port */
		{				
			ret = nbm_oct_read_eth_port_info(port, port_info);
		}
		else if( (port >= 4) && (port <= 7) )		/* 82571 Eth-port */
		{
			ret = nbm_e1000e_read_eth_port_info(port, port_info);
		}
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		if(port > 3 )
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3 )		/* 82571 Eth-port */
		{
			ret = nbm_e1000e_read_eth_port_info(port, port_info);
		}
	}
	else if( PRODUCT_AX7000 )
	{
		if ( port > 3)
		{
			syslog_ax_nbm_eth_port_err("Port number is Error!\n");
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else
		{
			ret = nbm_oct_read_eth_port_info(port, port_info);
		}
	}
	else
	{
		ret = ETHPORT_RETURN_CODE_ERR_PRODUCT_TYPE;
		syslog_ax_nbm_eth_port_err("Product type is Error\n");
	}

	return ret;
}

/**
  * Use ioctl to get board port's pkt statistic.For private use.
  * @return: PORT_STATE_GET_FAIL for failed. PORT_STATE_GET_SUCCESS for success.
  * codeby: baolc, 2008-07-04
  */
/*  // TODO:  Have to modify kernel interface.*/
  
static unsigned int nbm_board_port_pkt_statistic_do
(
	unsigned char portNum, 
	int clear,
	struct npd_port_counter *portPktCount,
	struct port_oct_s		*portOctCount
)
{
	int retval;
	int fd = -1;
	bm_op_read_eth_port_stats_args  read_port_stats_args;
	if (portNum > 3)
	{
		syslog_ax_nbm_eth_port_err("main board port number out of range!\n");
		return PORT_STATE_GET_FAIL;
	}
	if (portPktCount == NULL || portOctCount == NULL)
	{
		syslog_ax_nbm_eth_port_err("board eth-port statistics counter is null pointer.\n");
		return PORT_STATE_GET_FAIL;
	}

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_nbm_eth_port_err("open dev %s error(%d) when set port admin status!\n", NPD_BM_FILE_PATH, fd);
			return PORT_STATE_GET_FAIL;
		}
		g_bm_fd = fd;
	}

    memset(&read_port_stats_args, 0, sizeof(bm_op_read_eth_port_stats_args));
    read_port_stats_args.portNum = portNum;
	read_port_stats_args.clear = clear;
	memcpy(&read_port_stats_args.port_counter, portPktCount, sizeof(struct npd_port_counter));
	memcpy(&read_port_stats_args.port_oct_count, portOctCount, sizeof(struct port_oct_s));
    retval = ioctl(g_bm_fd, BM_IOC_BOARD_ETH_PORT_STATS_READ, &read_port_stats_args); /*read CSRs*/
	if (retval == -1)
	{
		syslog_ax_nbm_eth_port_err("read eth-port %d statistics error!\n", portNum);
		return PORT_STATE_GET_FAIL;
	}
    memcpy(portPktCount, &read_port_stats_args.port_counter, sizeof(struct npd_port_counter));
    memcpy(portOctCount, &read_port_stats_args.port_oct_count, sizeof(struct port_oct_s));

	return PORT_STATE_GET_SUCCESS;
}

/**
  * Get board port's pkt statistic.
  * @return: PORT_STATE_GET_FAIL(0xff) for failed. PORT_STATE_GET_SUCCESS(0x00) for success.
  * codeby: baolc, 2008-07-04
  */
unsigned int nbm_board_port_pkt_statistic
(
	unsigned char portNum, 
	struct npd_port_counter *portPktCount,
	struct port_oct_s		*portOtcCount
)
{
	return nbm_board_port_pkt_statistic_do(portNum, 1, portPktCount,portOtcCount); /* clear is 0, not clear stats CSRs */ /* clear use 1, because software accumulate the counter, modify by baolc, 2008-07-11*/
}


/**
  * Clear board port's pkt statistic.
  * @return: PORT_STATE_GET_FAIL for failed. PORT_STATE_GET_SUCCESS for success.
  * codeby: baolc, 2008-07-04
  */
unsigned int nbm_board_clear_port_pkt_stat
(
	unsigned char portNum
)
{
	struct npd_port_counter tmp_counter;
	struct port_oct_s tmp_oct;
	return nbm_board_port_pkt_statistic_do(portNum, 1, &tmp_counter,&tmp_oct); /* clear is 1, clear stats CSRs */
}

static int nbm_e1000e_set_ethport_enable(unsigned char port, unsigned long enable)
{
	int fd;
	int err;
	struct ethtool_drvinfo drvinfo;	
	struct ethtool_regs *regs;
	struct ifreq ifr;
	
	memset(&ifr, 0, sizeof(ifr));
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5612e[port]);
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		/*code optimize:  Out-of-bounds access
		zhangcl@autelan.com 2013-1-17*/
		if(port <= 3)
		{
			strcpy(ifr.ifr_name, eth_name_ax5608[port]);
		}
		else
		{
			syslog_ax_nbm_eth_port_err("Input port %d Error !\n",port);
			return NPD_FAIL;
		}
	}
	
	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) 
	{
		syslog_ax_nbm_eth_port_err("Open socket failed!\n");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	regs = calloc(1, sizeof(*regs));
	if (!regs) 
	{
		syslog_ax_nbm_eth_port_err("calloc failed!\n");
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	regs->cmd = 0x0000003D;		/* ETHTOOL_PORT_ADMIN */
	regs->len = enable;			/* Here, we use the len field as the enable state: 0 disable; 1 enable */

	ifr.ifr_data = regs;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if (err < 0) 
	{
		syslog_ax_nbm_eth_port_err("ioctl failed!\n");
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	free(regs);
	close(fd);

	return NPD_SUCCESS;
}

/***************************************************************************/
/*  Functions for control ports' attributes on mainboard.                  */
/***************************************************************************/

/**
  * Enable or disable port./Set port admin status.
  * @param portNum: The port number in main board. 0~3.
  * @param portAttr: ETH_ATTR_ENABLE for enable, ETH_ATTR_DISABLE for disable.
  * @return: NPD_FAIL for failed, NPD_SUCCESS for success.
  * codeby: baolc, 2008-06-30
  */
int nbm_oct_set_ethport_enable
(
	unsigned char	portNum,
	unsigned long	portAttr
)
{
	cvmx_mdio_phy1145_reg0_0 copper_ctl;
	cvmx_mdio_phy1145_reg0_1 fiber_ctl;
	
	copper_ctl.u16 = 0;
	fiber_ctl.u16 = 0;

	/*
	 * Selects copper banks of registers 0, 1, 4, 5, 6, 7, 8, 17, 18, 19.
	 */
	nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 0);
	copper_ctl.u16 = nbm_oct_mdio_read(portNum, 0);

	/*
	 * Selects fiber banks of registers 0, 1, 4, 5, 6, 7, 8, 17, 18, 19.
	 */
	nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 1);
	fiber_ctl.u16 = nbm_oct_mdio_read(portNum, 0);
	
	if(copper_ctl.u16 == 0xffff)		/* that is , -1 */
	{
		syslog_ax_nbm_eth_port_dbg("ERROR copper_ctl = 0x%x\n", phy_ctl_copper[portNum]);
		copper_ctl.u16 = phy_ctl_copper[portNum];
	}
	else
	{
		phy_ctl_copper[portNum] = copper_ctl.u16;
	}
	
	if(fiber_ctl.u16 == 0xffff)				/* that is , -1 */
	{
		syslog_ax_nbm_eth_port_dbg("ERROR fiber_ctl = 0x%x\n", phy_ctl_fiber[portNum]);
		fiber_ctl.u16 = phy_ctl_fiber[portNum];
	}
	else
	{
		phy_ctl_fiber[portNum] = fiber_ctl.u16;
	}

	if(portAttr)						/* ETH_ATTR_ENABLE */
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
	nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 0);
	nbm_oct_mdio_write(portNum, 0, copper_ctl.u16);
	
	/*
	 * Selects fiber banks of registers 0, 1, 4, 5, 6, 7, 8, 17, 18, 19.
	 */
	nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 1);
	nbm_oct_mdio_write(portNum, 0, fiber_ctl.u16);

	return NPD_SUCCESS;
}

int nbm_set_ethport_enable(unsigned char	port, unsigned long enable)
{
	int ret;
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		if( port > 7 )
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3)		/* Cavium Eth-port */
		{			
			ret = nbm_oct_set_ethport_enable(port, enable);
		}
		else if( (port >= 4) && (port <= 7) )		/* 82571 Eth-port */
		{			
			ret = nbm_e1000e_set_ethport_enable(port, enable);
		}
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		if( port > 3 )
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3 )		/* 82571 Eth-port */
		{			
			ret = nbm_e1000e_set_ethport_enable(port, enable);
		}
	}
	else if( PRODUCT_AX7000 )
	{
		if (port > 3)
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else
		{
			ret = nbm_oct_set_ethport_enable(port, enable);
		}
	}
	else
	{
		ret = ETHPORT_RETURN_CODE_ERR_PRODUCT_TYPE;
		syslog_ax_nbm_eth_port_err("Product type is Error\n");
	}

	return ret;
}

nbm_set_ethport_enable_MAC(unsigned char portNum,unsigned long portAttr)
{
	int  retval;
	int  fd = -1;
	bm_op_args	prt_cfg_args;
	/* No need to manipulate ASX csr. by baolc, 2008-07-09
	bm_op_args	prt_asx_rx_args;
	bm_op_args	prt_asx_tx_args;
	*/
	cvmx_gmxx_prtx_cfg_t  cfg_reg_value;
	/* No need to manipulate ASX csr.
	cvmx_asxx_rx_prt_en_t asx_rx;
	cvmx_asxx_tx_prt_en_t asx_tx;
	   */
	
	if (portNum > 3)
	{
		syslog_ax_nbm_eth_port_err("main board port number out of range!\n");
		return ETHPORT_RETURN_CODE_NO_SUCH_PORT;
	}
	
	if (portAttr != ETH_ATTR_DISABLE)
	{
		portAttr = ETH_ATTR_ENABLE;
	}

	if(g_bm_fd < 0) 
	{
		fd = open(NPD_BM_FILE_PATH, 0);
		if(fd < 0)
		{
			syslog_ax_nbm_eth_port_err("open dev %s error(%d) when set port admin status!\n", NPD_BM_FILE_PATH, fd);
			return ETHPORT_RETURN_CODE_ERR_HW;
		}
		g_bm_fd = fd;
	}
	
	/*read out the reg's value*/
	memset(&prt_cfg_args, 0, sizeof(prt_cfg_args));
	prt_cfg_args.op_addr = CVMX_GMXX_PRTX_CFG(portNum, 1); /*register's addr*/
	prt_cfg_args.op_len = 64;
	retval = ioctl(g_bm_fd, BM_IOC_G_, &prt_cfg_args); /*read reg*/
	if (retval == -1 || prt_cfg_args.op_ret != 0)
	{
		syslog_ax_nbm_eth_port_err("read port config reg error!\n");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	/*No need to manipulate ASX csr.
	memset(&prt_asx_rx_args, 0, sizeof(prt_asx_rx_args));
	prt_asx_rx_args.op_addr = CVMX_ASXX_RX_PRT_EN(1);
	prt_asx_rx_args.op_len = 64;
	retval = ioctl(fd, BM_IOC_G_, &prt_asx_rx_args); //read reg
	if (retval == -1 || prt_asx_rx_args.op_ret != 0)
	{
		syslog_ax_nbm_eth_port_err("read port asx rx reg error!");
		close(fd);
		return NPD_FAIL;
	}

	memset(&prt_asx_tx_args, 0, sizeof(prt_asx_tx_args));
	prt_asx_tx_args.op_addr = CVMX_ASXX_TX_PRT_EN(1);
	prt_asx_tx_args.op_len = 64;
	retval = ioctl(fd, BM_IOC_G_, &prt_asx_tx_args); //read reg
	if (retval == -1 || prt_asx_tx_args.op_ret != 0)
	{
		syslog_ax_nbm_eth_port_err("read port asx tx reg error!");
		close(fd);
		return NPD_FAIL;
	}
	*/

	/*modify the value*/
	memset(&cfg_reg_value, 0, sizeof(cfg_reg_value));
	cfg_reg_value.u64 = prt_cfg_args.op_value;
	/*syslog_ax_nbm_eth_port_dbg("main board port %d 's CVMX_GMXX_PRTX_CFG[EN] value: %#x.", portNum, cfg_reg_value.s.en);*/
	cfg_reg_value.s.en = portAttr; /*modify the current value*/
	prt_cfg_args.op_value = cfg_reg_value.u64; /*set the value*/

	/*No need to manipulate ASX csr.
	asx_rx.u64 = 0;
	asx_tx.u64 = 0;
	asx_rx.u64 = prt_asx_rx_args.op_value;
	asx_tx.u64 = prt_asx_tx_args.op_value;
	syslog_ax_nbm_eth_port_dbg("main board port %d 's CVMX_ASXX_RX_PRT_EN[PRT_EN] value: %#x.", portNum, asx_rx.s.prt_en);
	syslog_ax_nbm_eth_port_dbg("main board port %d 's CVMX_ASXX_TX_PRT_EN[PRT_EN] value: %#x.", portNum, asx_tx.s.prt_en);
	if (portAttr == ETH_ATTR_DISABLE)
	{
		asx_rx.s.prt_en &= ~(0x1<<portNum);
		asx_tx.s.prt_en &= ~(0x1<<portNum);
	}
	else
	{
		asx_rx.s.prt_en |= (0x1<<portNum);
		asx_tx.s.prt_en |= (0x1<<portNum);
	}
	
	syslog_ax_nbm_eth_port_dbg("main board port %d 's CVMX_ASXX_RX_PRT_EN[PRT_EN]'s new value: %#x.", portNum, asx_rx.s.prt_en);
	syslog_ax_nbm_eth_port_dbg("main board port %d 's CVMX_ASXX_TX_PRT_EN[PRT_EN]'s new value: %#x.", portNum, asx_tx.s.prt_en);
	prt_asx_rx_args.op_value = asx_rx.u64;
	prt_asx_tx_args.op_value = asx_tx.u64;
	*/

	/*write the value back to register*/
	/*No need to manipulate ASX csr.*//*
	if (portAttr == ETH_ATTR_ENABLE) *//* only enable, not disable, modify by baolc, 2008-07-08 
	{
		//ASX tx
		retval = ioctl(fd, BM_IOC_X_, &prt_asx_tx_args);
		if(retval == -1 || prt_asx_tx_args.op_ret != 0) {
			syslog_ax_nbm_eth_port_err("write port asx tx reg error!");
			close(fd);
			return NPD_FAIL;
		}
		//ASX rx
		retval = ioctl(fd, BM_IOC_X_, &prt_asx_rx_args);
		if(retval == -1 || prt_asx_rx_args.op_ret != 0) {
			syslog_ax_nbm_eth_port_err("write port asx rx reg error!");
			close(fd);
			return NPD_FAIL;
		}
	}
	*/
	
	/*syslog_ax_nbm_eth_port_dbg("main board port %d 's CVMX_GMXX_PRTX_CFG[EN]'s new value: %#x.", portNum, cfg_reg_value.s.en);*/
	retval = ioctl(g_bm_fd, BM_IOC_X_, &prt_cfg_args);
	if(retval == -1 || prt_cfg_args.op_ret != 0) {
		syslog_ax_nbm_eth_port_err("write port config reg error!\n");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	return NPD_SUCCESS;
}

/*
	Get Main board eth port up down change times.
	@param portNum: 0~3
	@uptimes : pointer to uptimes
	@downtimes:pointer to downtimes
	@return :NPD_SUCCESS/NPD_FAIL
	
*/
int nbm_get_ethport_updown_times(
	unsigned char portNum, 
	int *uptimes,
	int *downtimes
)
{
	int ret;
	int fd;	

	cvm_port_updown_times updown_times;
	memset(&updown_times,0,sizeof(cvm_port_updown_times));
	
	if (portNum > 3)
	{
		syslog_ax_nbm_eth_port_err("main board port number out of range,portNum = %d!",portNum);
		return ETHPORT_RETURN_CODE_NO_SUCH_PORT;
	}

	fd = open(DEV_NAME, 0);
	if(fd == -1)
	{
		syslog_ax_nbm_eth_port_err("open Dev(%s) error!", DEV_NAME);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	if(PRODUCT_AX7000)
	{
		updown_times.pip_port = portNum + 16 ;
	}
	else
	{
		updown_times.pip_port = portNum;
	}


	ret = ioctl(fd,CVM_IOC_GET_UPDOWN_TIMES,&updown_times);
	if(ret == -1)
	{
		syslog_ax_nbm_eth_port_err("Get port up down times not success.");
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	close(fd);
	*uptimes = updown_times.up_times;
	*downtimes = updown_times.down_times;
	
	syslog_ax_nbm_eth_port_dbg("Get port updown times Port == %d,\t up_times == %d down_times == %d\n",updown_times.pip_port, updown_times.up_times, updown_times.down_times);

	return NPD_SUCCESS;


}

/*
	Get Main board eth port link timestamp.
	@param portNum: 0~3
	@return :last link changed timestamp in seconds.
	
*/
unsigned long nbm_get_link_timestamp(unsigned char portNum)
{
	int ret;
	int fd;	

	cvm_link_timestamp link_stamps;
	memset(&link_stamps,0,sizeof(cvm_link_timestamp));
	
	if (portNum > 3)
	{
		syslog_ax_nbm_eth_port_err("main board port number out of range,portNum = %d!",portNum);
		return ETHPORT_RETURN_CODE_NO_SUCH_PORT;
	}

	fd = open(DEV_NAME, 0);
	if(fd == -1)
	{
		syslog_ax_nbm_eth_port_err("open Dev(%s) error!", DEV_NAME);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	if(PRODUCT_AX7000)
	{
		link_stamps.pip_port = portNum + 16 ;
	}
	else
	{
		link_stamps.pip_port = portNum;
	}


	ret = ioctl(fd,CVM_IOC_ETH_LINK_TIMESTAMP,&link_stamps);
	if(ret == -1)
	{
		syslog_ax_nbm_eth_port_err("Get port time stamps not success.");
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	close(fd);

	
	syslog_ax_nbm_eth_port_dbg("Get port Linkstamps == 0x%016llx.\n",link_stamps.timestamp);
	

	return (unsigned long)(link_stamps.timestamp);


}

static int get_ctl_fd(void)
{
	int s_errno;
	int fd = -1;

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd >= 0)
	{
		return fd;
	}
	
	s_errno = errno;
	fd = socket(PF_PACKET, SOCK_DGRAM, 0);
	if (fd >= 0)
	{
		return fd;
	}
	
	fd = socket(PF_INET6, SOCK_DGRAM, 0);
	if (fd >= 0)
	{
		return fd;
	}
	
	errno = s_errno;
	syslog_ax_nbm_eth_port_err("Cannot create control socket\n");
	return -1;
}

static int set_mtu(const char *dev, int mtu)
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
		syslog_ax_nbm_eth_port_err("SIOCSIFMTU IOCTL error!\n");
		close(s);
		return -1;
	}
	close(s);

	return 0; 
}

static int get_mtu(const char *dev)
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
	if (ioctl(s, SIOCGIFMTU, &ifr) < 0) {
		syslog_ax_nbm_eth_port_err("SIOCGIFMTU IOCTL error!\n");
		close(s);
		return -1;
	}
	close(s);

	return 	ifr.ifr_mtu; 
}

static char *nbm_get_product_port_name(unsigned char port)
{
	/*code optimize:  Uninitialized pointer read
	zhangcl@autelan.com 2013-1-18*/
	char *eth_name = NULL;
	
	if( 7 == ((nbm_get_product_type() >> 8) & 0xF) )
	{
		if (port > 3)
		{
			syslog_ax_nbm_eth_port_err("main board port number out of range!\n");
			return ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else
		{
			/* we need not set the mtu of ax71-smu,zhangdi 2011-06-11 */
			#if 0
			FILE *fp = NULL;
			char path[256] = {0};
			char slot_id = '0';
			snprintf(path,256,"/proc/board/slot_id");
			fp = fopen(path,"r");
			if(fp){
				fgets(&slot_id,2,fp);
				fclose(fp);
			}
			syslog_ax_nbm_eth_port_dbg("get slot no from is /proc/board/slot_id %c\n",slot_id);
			if(slot_id == '1'){
				eth_name = eth_name_ax7000_I[port];
			}
			else{
				eth_name = eth_name_ax7000[port];
			}
			#endif
			/* if eth board is ax71-smu, here return wrong name,don't care */
			eth_name = eth_name_ax7000[port];
		}
	}
	else if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		if (port > 7)
		{
			syslog_ax_nbm_eth_port_err("main board port number out of range!\n");
			return ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else
		{
			eth_name = eth_name_ax5612e[port];
		}			
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		if (port > 3)
		{
			syslog_ax_nbm_eth_port_err("main board port number out of range!\n");
			return ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else
		{
			eth_name = eth_name_ax5608[port];
		}			
	}
	else if( PRODUCT_TYPE_5612I == nbm_get_product_type() )
	{
		if (port > 3)
		{
			syslog_ax_nbm_eth_port_err("main board port number out of range!\n");
			return ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else
		{
			eth_name = eth_name_5612i[port];
		}			
	}
	else
	{
		syslog_ax_nbm_eth_port_err("Product type is Error\n");
	}
	
	return eth_name;
}

/**
  * Set port's MTU.
  * @param portNum: The port number in main board. 0~3.
  * @param mruSize: The size of mtu.
  * @return: NPD_FAIL for failed, NPD_SUCCESS for success.
  * Note: Should disable the port before changing the MTU.
  * codeby: baolc, 2008-06-30
  */
int nbm_set_ethport_mru
(
	unsigned char portNum, 
	unsigned int mruSize
)
{
	unsigned int mtu = 0;
	char *eth_name;

	mtu = mruSize - 18;	
	eth_name = nbm_get_product_port_name( portNum );
	
	if (set_mtu(eth_name, mtu) == 0)
	{
		return NPD_SUCCESS;
	}
	else 
	{
		return ETHPORT_RETURN_CODE_ERR_GENERAL;
	}	
}


/**
  * Get port's MTU.
  * @param portNum: The port number in main board. 0~3.
  * @param mruSize: The size of mtu.
  * @return: NPD_FAIL for failed,MTU for success.
  * Note: Should disable the port before changing the MTU.
  * codeby: baolc, 2008-06-30
  */
int nbm_get_ethport_mru
(
	unsigned char portNum
)
{
	int mtu;
	char *eth_name;	
	
	eth_name = nbm_get_product_port_name( portNum );
	mtu = get_mtu(eth_name);
	mtu = mtu + 18;
	
	return mtu;
}

/**
  * Enable/Disable the ports' flow control. (Including the response to PAUSE frame and the transmition of PAUSE frame.)
  * @param portNum: The port number in main board. 0~3.
  * @param state: ETH_ATTR_ENABLE for enable. ETH_ATTR_DISABLE for disable.
  * @return: NPD_FAIL for failed, NPD_SUCCESS for success.
  * Note: Should disable the port before changing the status of FC.
  * codeby: baolc, 2008-07-02
  */

/* // TODO: because the port disable and enable is modified by wang jiankun use PHY register, so the this function can not work perpuros.  */
int nbm_set_ethport_flowCtrl
( 
	unsigned char portNum, 
	unsigned long state
)
{
    int retval;
	int fd = -1;
    bm_op_args  frm_ctl_ioctl_args;
    bm_op_args  ovr_bp_ioctl_args;
    
    /*GMXn_RXn_FRM_CTL[CTL_BCK/CTL_DRP]  controls the response to PAUSE frame*/
    /*GMXn_TX_OVR_BP[EN/BP]   controls the transmition of PAUSE frame*/
    cvmx_gmxx_rxx_frm_ctl_t  frm_ctl_reg;
    cvmx_gmxx_tx_ovr_bp_t    ovr_bp_reg;

	return ETHPORT_RETURN_CODE_UNSUPPORT;
	/*code optimize:  Structurally dead code
	zhangcl@autelan.com 2013-1-17*/
	#if 0
    if (portNum > 3)
	{
		syslog_ax_nbm_eth_port_err("main board port number out of range!\n");
		return NPD_FAIL;
	}

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_nbm_eth_port_err("open dev %s error(%d) when set port admin status!\n", NPD_BM_FILE_PATH, fd);
			return NPD_FAIL;
		}
		g_bm_fd = fd;
	}

	/*read out the frm_ctl_reg's value*/
	memset(&frm_ctl_ioctl_args, 0, sizeof(frm_ctl_ioctl_args));
	if(PRODUCT_AX7000)
	{
		frm_ctl_ioctl_args.op_addr = CVMX_GMXX_RXX_FRM_CTL(portNum, 1); /*register's addr*/
	}
	else
	{
		frm_ctl_ioctl_args.op_addr = CVMX_GMXX_RXX_FRM_CTL(portNum, 0); /*register's addr*/
	}
	frm_ctl_ioctl_args.op_len = 64;
	retval = ioctl(g_bm_fd, BM_IOC_G_, &frm_ctl_ioctl_args); /*read reg*/
	if (retval == -1 || frm_ctl_ioctl_args.op_ret != 0)
	{
		syslog_ax_nbm_eth_port_err("read port's MTU config reg error!\n");
		return NPD_FAIL;
	}
	
	/*change frm_ctl's value*/
	memset(&frm_ctl_reg, 0, sizeof(frm_ctl_reg));
    frm_ctl_reg.u64 = frm_ctl_ioctl_args.op_value;
    /*syslog_ax_nbm_eth_port_dbg("current flow control response status: gmx_rx_frm_ctl[CTL_BCK/CTL_DRP] is %d, %d!", frm_ctl_reg.s.ctl_bck, frm_ctl_reg.s.ctl_drp);*/
    if (state == ETH_ATTR_ENABLE)
    {
        /*enable FC response*/
        frm_ctl_reg.s.ctl_bck = 1;
        frm_ctl_reg.s.ctl_drp = 1;
    }
    else if (state == ETH_ATTR_DISABLE)
    {
        /*disable FC response*/
        frm_ctl_reg.s.ctl_bck = 0;
        frm_ctl_reg.s.ctl_drp = 1;
    }
    frm_ctl_ioctl_args.op_value = frm_ctl_reg.u64;
    
    /*read out ovr_bp_reg's value*/
    memset(&ovr_bp_ioctl_args, 0, sizeof(ovr_bp_ioctl_args));
	if(PRODUCT_AX7000)
	{
		ovr_bp_ioctl_args.op_addr = CVMX_GMXX_TX_OVR_BP(1); /*register's addr*/
	}
	else
	{
		ovr_bp_ioctl_args.op_addr = CVMX_GMXX_TX_OVR_BP(0); /*register's addr*/
	}
	ovr_bp_ioctl_args.op_len = 64;
	retval = ioctl(g_bm_fd, BM_IOC_G_, &ovr_bp_ioctl_args); /*read reg*/
	if (retval == -1 || ovr_bp_ioctl_args.op_ret != 0)
	{
		syslog_ax_nbm_eth_port_err("read port's MTU config reg error!\n");
		return NPD_FAIL;
	}
	
	/*change ovr_bp's value*/
	memset(&ovr_bp_reg, 0, sizeof(ovr_bp_reg));
    ovr_bp_reg.u64 = ovr_bp_ioctl_args.op_value;
   /*syslog_ax_nbm_eth_port_dbg("current flow control tx status: ovr_bp_reg[EN] is %#x, ovr_bp_reg[BP] is %#x!", ovr_bp_reg.s.en, ovr_bp_reg.s.bp);*/
    if (state == ETH_ATTR_ENABLE)
    {
        /*enable FC tx, EN=1, BP=1*/
        ovr_bp_reg.s.en |= (0x1 << portNum); /*portNum's bit set to 1*/
        ovr_bp_reg.s.bp |= (0x1 << portNum); /*portNum's bit set to 1*/
    }
    else /*if (state == ETH_ATTR_DISABLE)*/
    {
        /*disable FC tx, EN=1, BP=0*/
        ovr_bp_reg.s.en |= (0x1 << portNum); /*portNum's bit set to 1*/
        ovr_bp_reg.s.bp &= ~(0x1 << portNum); /*portNum's bit clear to 0*/
    }
    ovr_bp_ioctl_args.op_value = ovr_bp_reg.u64;
    
    /*disable the port before write the register*/
	if (nbm_set_ethport_enable_MAC(portNum, ETH_ATTR_DISABLE) != NPD_SUCCESS) 
	{
	    syslog_ax_nbm_eth_port_err("can't change port's FC because can't disable the port!\n");
		return NPD_FAIL;
	}
	
	/*write the registers*/
	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_nbm_eth_port_err("open dev %s error(%d) when set port admin status!\n", NPD_BM_FILE_PATH, fd);
			return NPD_FAIL;
		}
		g_bm_fd = fd;
	}

	retval = ioctl(g_bm_fd, BM_IOC_X_, &frm_ctl_ioctl_args);
	if(retval == -1 || frm_ctl_ioctl_args.op_ret != 0) {
		syslog_ax_nbm_eth_port_err("write port FC frm_ctl_reg error!\n");
		return NPD_FAIL;
	}
	retval = ioctl(g_bm_fd, BM_IOC_X_, &ovr_bp_ioctl_args);
	if(retval == -1 || ovr_bp_ioctl_args.op_ret != 0) {
		syslog_ax_nbm_eth_port_err("write port FC ovr_bp error!\n");
		return NPD_FAIL;
	}
	
	/*enable the port after write the register*/
	if (nbm_set_ethport_enable_MAC(portNum, ETH_ATTR_ENABLE) != NPD_SUCCESS) 
	{
	    syslog_ax_nbm_eth_port_err("FC status already changed, but the port can't be enabled!\n");
		return NPD_FAIL;
	}
    
    return NPD_SUCCESS;
	#endif
}


/**
  * Enable/Disable the ports' backpressure.
  * @param portNum: The port number in main board. 0~3.
  * @param state: ETH_ATTR_ENABLE for enable. ETH_ATTR_DISABLE for disable.
  * @return: NPD_FAIL for failed, NPD_SUCCESS for success.
  * Note: No need to disable the port before changing backpressure status. Just change it directly.
  * codeby: baolc, 2008-07-02
  */
int nbm_set_ethport_backPres
(
	unsigned char portNum, 
	unsigned long state
)
{
    int retval;
    int fd = -1;
    bm_op_args  ioctl_args;
    
    /*IPD_PORTn_BP_PAGE_CNT[BP_ENB]  controls the backpressure enable/disable.*/
    cvmx_ipd_portx_bp_page_cnt_t  bp_page_cnt_reg;
    if (portNum > 3)
	{
		syslog_ax_nbm_eth_port_err("main board port number out of range!\n");
		return ETHPORT_RETURN_CODE_NO_SUCH_PORT;
	}

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_nbm_eth_port_err("open dev %s error(%d) when set port admin status!\n", NPD_BM_FILE_PATH, fd);
			return ETHPORT_RETURN_CODE_ERR_HW;
		}
		g_bm_fd = fd;
	}
  
	/*read out the bp_page_cnt_reg's value*/
	memset(&ioctl_args, 0, sizeof(ioctl_args));
	if(PRODUCT_AX7000)
	{
		ioctl_args.op_addr = CVMX_IPD_PORTX_BP_PAGE_CNT(portNum + 16); /*register's addr, RGMII's IPD port is 16~19*/
	}
	else
	{
		ioctl_args.op_addr = CVMX_IPD_PORTX_BP_PAGE_CNT(portNum); /*register's addr, RGMII's IPD port is  0~2*/
	}
	ioctl_args.op_len = 64;
	retval = ioctl(g_bm_fd, BM_IOC_G_, &ioctl_args); /*read reg*/
	if (retval == -1 || ioctl_args.op_ret != 0)
	{
		syslog_ax_nbm_eth_port_err("read port's BP config reg(bp_page_cnt_reg) error!\n");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	/*change bp_page_cnt_reg's value*/
	memset(&bp_page_cnt_reg, 0, sizeof(bp_page_cnt_reg));
    bp_page_cnt_reg.u64 = ioctl_args.op_value;
    /*syslog_ax_nbm_eth_port_dbg("current IPD BP status for port %d: IPD_PORTn_BP_PAGE_CNT[BP_ENB]=%d!", portNum, bp_page_cnt_reg.s.bp_enb);*/
    if (state == ETH_ATTR_ENABLE)
    {
        /*enable BP*/
        bp_page_cnt_reg.s.bp_enb = 1;
    }
    else /*if (state == ETH_ATTR_DISABLE)*/
    {
        /*disable BP*/
        bp_page_cnt_reg.s.bp_enb = 0;
    }
    ioctl_args.op_value = bp_page_cnt_reg.u64;
    
    /*disable the port before write the register*/
	if (nbm_set_ethport_enable_MAC(portNum, ETH_ATTR_DISABLE) != NPD_SUCCESS) 
	{
	   syslog_ax_nbm_eth_port_err("can't change port's FC because can't disable the port!\n");
		return ETHPORT_RETURN_CODE_UNSUPPORT;
	}
	
    /*write the register*/

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_nbm_eth_port_err("open dev %s error(%d) when set port admin status!\n", NPD_BM_FILE_PATH, fd);
			return ETHPORT_RETURN_CODE_ERR_HW;
		}
		g_bm_fd = fd;
	}

	retval = ioctl(g_bm_fd, BM_IOC_X_, &ioctl_args);
	if(retval == -1 || ioctl_args.op_ret != 0) {
		syslog_ax_nbm_eth_port_err("write port BP bp_page_cnt_reg error!\n");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	/*enable the port after write the register*/
	if (nbm_set_ethport_enable_MAC(portNum, ETH_ATTR_ENABLE) != NPD_SUCCESS) 
	{
	   syslog_ax_nbm_eth_port_err("FC status already changed, but the port can't be enabled!\n");
		return ETHPORT_RETURN_CODE_UNSUPPORT;
	}
	
    return NPD_SUCCESS;
}


/***************************************************************************/
/*  The following functions are the unsupported configuration of port  on  main board. */
/***************************************************************************/

int nbm_set_ethport_force_linkup
(
	unsigned char portNum,
	unsigned long state
)
{
	return ETHPORT_RETURN_CODE_UNSUPPORT;
}


int nbm_set_ethport_force_linkdown
(
	unsigned char portNum,
	unsigned long state
)
{
	return ETHPORT_RETURN_CODE_UNSUPPORT;
}

static int nbm_e1000e_set_ethport_duplex_mode
(
	unsigned char port,
	PORT_DUPLEX_ENT	mode
)
{
	int err;
	int fd;	
	struct ifreq ifr;
	struct ethtool_cmd ecmd;
	unsigned int media = 0;

	/* Setup our control structures. */
	memset(&ifr, 0, sizeof(ifr));
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5612e[port]);
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5608[port]);
	}

    /* Unsupported eth-port configuration On Fiber/SerDes mode */
    if (nbm_get_eth_port_trans_media(port, &media))
	{
		syslog_ax_nbm_eth_port_err("port:%d get preferred media failed\n", port);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
    if(ETH_GE_FIBER == media)
    {
		syslog_ax_nbm_eth_port_err("Ethernet Controller-82571 only support 1000M+Full duplex for TBI mode\n");
		return ETHPORT_RETURN_CODE_ETH_GE_SFP;
    }
	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		syslog_ax_nbm_eth_port_err("Open socket failed!\n");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	ecmd.cmd = 0x00000001;	/* ETHTOOL_GSET */
	ifr.ifr_data = &ecmd;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if(err)
	{		
		syslog_ax_nbm_eth_port_err("ioctl failed!\n");
		/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-18*/
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	/* Auto-negotiation is enable */
    if(AUTONEG_ENABLE == ecmd.autoneg)
    {
		syslog_ax_nbm_eth_port_dbg("auto-negotiation is enable, can't set duplex mode\n");
		/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-18*/
		close(fd);
		return ETHPORT_RETURN_CODE_DUPLEX_NODE;
	}
	switch(mode)
	{
		case PORT_FULL_DUPLEX_E:
			ecmd.duplex = 1;
			break;
		case PORT_HALF_DUPLEX_E:
			if(ecmd.speed == 1000)
			{
				/*code optimize:  Resource leak
				zhangcl@autelan.com 2013-1-18*/
				close(fd);
				return ETHPORT_RETURN_CODE_DUPLEX_NODE;
			}
			ecmd.duplex = 0;
			break;
		default:
    		return ETHPORT_RETURN_CODE_ERR_GENERAL;
	}
	
	/* Try to perform the update. */
	ecmd.cmd = 0x00000002;		/* ETHTOOL_SSET */
	ifr.ifr_data = &ecmd;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if (err < 0)
	{
		/*code optimize:  Double closed
		zhangcl@autelan.com 2013-1-17*/
		#if 0
		close(fd);
		#endif
		syslog_ax_nbm_eth_port_err("Cannot set new settings");
	}

	close(fd);
	return NPD_SUCCESS;
}


/*
 * set duplex mode according to register 26 bit10~bit11 selected
 */
int nbm_oct_set_ethport_duplex_mode
(
	unsigned char portNum, 
	PORT_DUPLEX_ENT	dMode
)
{
	int media;
	unsigned short reg_val = 0;
	cvmx_mdio_phy1145_reg0_0 copper_ctl;
	cvmx_mdio_phy1145_reg0_1 fiber_ctl;
	
	fiber_ctl.u16 = 0;
	copper_ctl.u16 = 0;
	
	/*is speed auto-negotiation is disable*/
	reg_val = nbm_oct_mdio_read(portNum, 0x00);
	/*auto-negotiation is enable*/
	if (reg_val & 0x1000)
	{
		syslog_ax_nbm_eth_port_dbg("auto-negotiation is enable, can't set duplex mode\n");
		return ETHPORT_RETURN_CODE_DUPLEX_NODE;
	}

	media = nbm_oct_check_media_and_change_page(portNum);	
	switch (media)
	{		
		case 0x1:		/* fiber */
			fiber_ctl.u16 = nbm_oct_mdio_read(portNum, 0x00);
			fiber_ctl.s.reset = 1;
			switch (dMode)
			{
				case PORT_FULL_DUPLEX_E:
					fiber_ctl.s.fiber_duplex_mod = 1;
					break;
				case PORT_HALF_DUPLEX_E:
					fiber_ctl.s.fiber_duplex_mod = 0;
					break;
				default:
					return ETHPORT_RETURN_CODE_ERR_GENERAL;
			}
			nbm_oct_mdio_write(portNum, 0, fiber_ctl.u16);
			break;
		case 0x0:
		case 0x2:		/* copper */
			copper_ctl.u16 = nbm_oct_mdio_read(portNum, 0x00);
			copper_ctl.s.reset = 1;
			switch (dMode)
			{
				case PORT_FULL_DUPLEX_E:
					copper_ctl.s.copper_duplex_mod = 1;
					break;
				case PORT_HALF_DUPLEX_E:
					copper_ctl.s.copper_duplex_mod = 0;
					break;
				default:
					return ETHPORT_RETURN_CODE_ERR_GENERAL;
			}
			nbm_oct_mdio_write(portNum, 0, copper_ctl.u16);
			break;		
		default:		/* error media */
			return ETHPORT_RETURN_CODE_ERR_HW;
		}
	
	return 0;
}
int nbm_set_ethport_duplex_mode
(
	unsigned char port, 
	PORT_DUPLEX_ENT	mode
)
{
	int ret;
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		/*code optimize:  Unsigned compared against 0
		zhangcl@autelan.com 2013-1-17*/
		if( port > 7)
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3 )		/* Cavium Eth-port */
		{			
			ret = nbm_oct_set_ethport_duplex_mode(port, mode);
		}
		else if( (port >= 4) && (port <= 7) )		/* 82571 Eth-port */
		{			
			ret = nbm_e1000e_set_ethport_duplex_mode(port, mode);
		}
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		if(port > 3 )
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3 )		/* 82571 Eth-port */
		{			
			ret = nbm_e1000e_set_ethport_duplex_mode(port, mode);
		}
	}
	else if( PRODUCT_AX7000 )
	{
		if (port > 3)
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else
		{
			ret = nbm_oct_set_ethport_duplex_mode(port, mode);
		}
	}
	else
	{
		ret = ETHPORT_RETURN_CODE_ERR_PRODUCT_TYPE;
		syslog_ax_nbm_eth_port_err("Product type is Error\n");
	}

	return ret;
}


int nbm_set_ethport_ipg
(
	unsigned char portNum, 
	unsigned char port_ipg 
)
{
	return NPD_DBUS_ERROR_UNSUPPORT;
}

static int nbm_e1000e_set_speed(unsigned char port, PORT_SPEED_ENT speed)
{
	int err;
	int fd;	
	struct ifreq ifr;
	struct ethtool_cmd ecmd;
	unsigned int media = 0;

	/* Setup our control structures. */
	memset(&ifr, 0, sizeof(ifr));
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5612e[port]);
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5608[port]);
	}

	/* Unsupported eth-port configuration On Fiber/SerDes mode */
    if (nbm_get_eth_port_trans_media(port, &media))
	{
		syslog_ax_nbm_eth_port_err("port:%d get preferred media failed\n", port);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	if(ETH_GE_FIBER == media)
	{
		syslog_ax_nbm_eth_port_err("Ethernet Controller-82571 only support 1000M+Full duplex for TBI mode\n");
		return ETHPORT_RETURN_CODE_ETH_GE_SFP;
	}
	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		syslog_ax_nbm_eth_port_err("Open socket failed!\n");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	ecmd.cmd = 0x00000001;	/* ETHTOOL_GSET */
	ifr.ifr_data = &ecmd;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if(err)
	{		
		syslog_ax_nbm_eth_port_err("ioctl failed!\n");
		/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-18*/
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
    /* Auto-negotiation is enable */
    if(AUTONEG_ENABLE == ecmd.autoneg)
    {
		syslog_ax_nbm_eth_port_err("auto-negotiation is enable, can't set speed\n");
		/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-18*/
		close(fd);
		return ETHPORT_RETURN_CODE_SPEED_NODE;
	}
	switch(speed)
	{
		case PORT_SPEED_10_E:
			ecmd.speed = 10;
			break;
		case PORT_SPEED_100_E:
			ecmd.speed = 100;
			break;
		case PORT_SPEED_1000_E:
			ecmd.speed = 1000;
			break;
		default:
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-18*/
			close(fd);
			return ETHPORT_RETURN_CODE_ERR_GENERAL; 
	}
	
	/* Try to perform the update. */
	ecmd.cmd = 0x00000002;		/* ETHTOOL_SSET */
	ifr.ifr_data = &ecmd;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if (err < 0)
	{
		/*code optimize:  Double closed
		zhangcl@autelan.com 2013-1-17*/
		#if 0
		close(fd);
		#endif
		syslog_ax_nbm_eth_port_err("Cannot set new settings");
	}

	close(fd);
	return NPD_SUCCESS;
}

static int nbm_oct_set_ethport_speed
(
	unsigned char port, 
	PORT_SPEED_ENT speed
)
{
	int media;
	unsigned short reg_val = 0;
	cvmx_mdio_phy1145_reg0_0 copper_ctl;
	cvmx_mdio_phy1145_reg0_1 fiber_ctl;
	fiber_ctl.u16 = 0;
	copper_ctl.u16 = 0;

	/* is speed auto-negotiation is disable */
	reg_val = nbm_oct_mdio_read(port, 0x00);	
	if (reg_val & 0x1000)		/* auto-negotiation is enable */
	{
		syslog_ax_nbm_eth_port_dbg("auto-negotiation is enable, can't set speed\n");
		return ETHPORT_RETURN_CODE_SPEED_NODE;
	}

	media = nbm_oct_check_media_and_change_page(port);
	switch(media)
	{
		case 0x1:			/* fiber */
			/* fiber only support 1000M */
			if (speed != PORT_SPEED_1000_E)
			{
				syslog_ax_nbm_eth_port_dbg("fiber speed alaways is 1000M\n");
				return ETHPORT_RETURN_CODE_ETH_GE_SFP;
			}
			
			fiber_ctl.u16 = nbm_oct_mdio_read(port, 0x00);
			fiber_ctl.s.auto_negotiaton_en = 0;
			fiber_ctl.s.reset = 1;
			fiber_ctl.s.speedselect_bit13 = 0;
			fiber_ctl.s.speedselect_bit6 = 1;
			nbm_oct_mdio_write(port, 0, fiber_ctl.u16);
			break;
		case 0x0:
		case 0x2:		/* copper */
			copper_ctl.u16 = nbm_oct_mdio_read(port, 0x00);
			copper_ctl.s.auto_negotiaton_en = 0;
			copper_ctl.s.reset = 1;
			
			/* copper can support 10/100/1000 */
			switch (speed)
			{
				case PORT_SPEED_10_E:
					copper_ctl.s.speedselect_bit13 = 0;
					copper_ctl.s.speedselect_bit6 = 0;
					break;
				case PORT_SPEED_100_E:
					copper_ctl.s.speedselect_bit13 = 1;
					copper_ctl.s.speedselect_bit6 = 0;
					break;
				case PORT_SPEED_1000_E:
					copper_ctl.s.speedselect_bit13 = 0;
					copper_ctl.s.speedselect_bit6 = 1;
					break;
				default:
					return ETHPORT_RETURN_CODE_ERR_GENERAL;	
			}
			nbm_oct_mdio_write(port, 0, copper_ctl.u16);
			break;
		default:
			syslog_ax_nbm_eth_port_dbg("media type error!\n");
			return ETHPORT_RETURN_CODE_ERR_HW;
	}

	return NPD_SUCCESS;
}

int nbm_set_ethport_speed(unsigned char port, PORT_SPEED_ENT speed)
{
	int ret;
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		/*code optimize:  Unsigned compared against 0
		zhangcl@autelan.com 2013-1-17*/
		if( port > 7)
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3 )		/* Cavium Eth-port */
		{				
			ret = nbm_oct_set_ethport_speed(port, speed);
		}
		else if( (port >= 4) && (port <= 7) )		/* 82571 Eth-port */
		{
			ret = nbm_e1000e_set_speed(port, speed);
		}
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		if( port > 3 )
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3)		/* 82571 Eth-port */
		{
			ret = nbm_e1000e_set_speed(port, speed);
		}
	}
	else if ( PRODUCT_AX7000 )
	{
		if (port > 3)
		{
			syslog_ax_nbm_eth_port_err("Port number is Error!\n");
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else
		{
			ret = nbm_oct_set_ethport_speed(port, speed);
		}
	}
	else
	{
		ret = ETHPORT_RETURN_CODE_ERR_PRODUCT_TYPE;
		syslog_ax_nbm_eth_port_err("Product type is Error\n");
	}

	return ret;
}

static int nbm_e1000e_set_autoneg(unsigned char port, unsigned long state)
{
	int err;
	int fd; 
	struct ifreq ifr;
	struct ethtool_cmd ecmd;
	unsigned int media = 0;

	/* Setup our control structures. */
	memset(&ifr, 0, sizeof(ifr));
	
	if( PRODUCT_TYPE_5612E == nbm_get_product_type() )
	{
		strcpy(ifr.ifr_name, eth_name_ax5612e[port]);
	}
	else if( PRODUCT_TYPE_5608 == nbm_get_product_type() )
	{
		/*code optimize:  Out-of-bounds access
		zhangcl@autelan.com 2013-1-17*/
		if(port<=3)
		{
			strcpy(ifr.ifr_name, eth_name_ax5608[port]);
		}
		else
		{
			syslog_ax_nbm_eth_port_err("Input port %d Error !\n",port);
			return NPD_FAIL;
		}
	}

	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) 
	{
		syslog_ax_nbm_eth_port_err("Cannot get control socket");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	ecmd.cmd = 0x00000001;	/* ETHTOOL_GSET */
	ifr.ifr_data = &ecmd;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if(err)
	{		
		syslog_ax_nbm_eth_port_err("ioctl failed!\n");
		/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-18*/
		close(fd);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	ecmd.autoneg = state;
	
	/* Try to perform the update. */
	ecmd.cmd = 0x00000002;		/* ETHTOOL_SSET */
	ifr.ifr_data = &ecmd;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if (err < 0)
	{
		syslog_ax_nbm_eth_port_err("Cannot set new settings");
		/*code optimize:  Double closed
		zhangcl@autelan.com 2013-1-17*/
		#if 0
		close(fd);
		#endif
	}

	close(fd);

	return NPD_SUCCESS;
}

/*
 *return value:0 succeed
 *			 1 failed
 *
 *@portNum:0~3
 *@state:0 disable autonegotiation
 *        1 endable autonegotiation
 *coded by huxuefeng20091104
 */
static int nbm_oct_set_autoneg(unsigned char portNum, unsigned long state)
{
	cvmx_mdio_phy1145_reg0_0 copper_ctl;
	cvmx_mdio_phy1145_reg0_1 fiber_ctl;
	fiber_ctl.u16 = 0;
	copper_ctl.u16 = 0;
	int media = 0;	

	if (portNum > 3)
	{
		syslog_ax_nbm_eth_port_err("bad port number %d\n", portNum);
		return ETHPORT_RETURN_CODE_NO_SUCH_PORT;	
	}

	if (state != 0 && state != 1)
	{
		syslog_ax_nbm_eth_port_err("bad stat %d\n", state);
		return ETHPORT_RETURN_CODE_ERR_GENERAL;
	}
	
	/* get preferred media ; fiber:media = 1; copper:media = 2 */
	if (nbm_get_eth_port_trans_media(portNum, &media))
	{
		syslog_ax_nbm_eth_port_err("port:%d get preferred media failed\n", portNum);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	if (media == 0x1)			/* preferred fiber */
	{
		nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 1);
		fiber_ctl.u16 = nbm_oct_mdio_read(portNum,0);

		if(fiber_ctl.u16 == 0xffff)
		{
			fiber_ctl.u16 = phy_ctl_fiber[portNum];
		}
		else
		{
			phy_ctl_fiber[portNum] = fiber_ctl.u16;
		}
		
		if(state == 0)		/* disable fiber autonegotiation and set full duplex mode 1000M */
		{
			fiber_ctl.s.auto_negotiaton_en = 0;
			fiber_ctl.s.speedselect_bit13 = 0;
			fiber_ctl.s.speedselect_bit6 = 1;
			fiber_ctl.s.fiber_duplex_mod = 1;
			fiber_ctl.s.reset = 1;
			nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 1);
			nbm_oct_mdio_write(portNum,0,fiber_ctl.u16);
			syslog_ax_nbm_eth_port_dbg("PHY1145 AUNEG disable!");
			return 0;
		}		
		else if(state == 1)	/* enable fiber autonegotiation */
		{
			fiber_ctl.s.auto_negotiaton_en = 1;
			fiber_ctl.s.reset = 1;
			nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 1);
			nbm_oct_mdio_write(portNum,0,fiber_ctl.u16);
			syslog_ax_nbm_eth_port_dbg("PHY1145 AUNEG enable!");
			return 0;
		}
	}	
	else if (media == 0x2 || media == 0x0)		/* preferred copper */
	{
		/* set page and read.*/
		nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 0);
		copper_ctl.u16 = nbm_oct_mdio_read(portNum,0);
		if(copper_ctl.u16 == 0xffff)
		{
			copper_ctl.u16 = phy_ctl_copper[portNum];
		}
		else
		{
			phy_ctl_copper[portNum] = copper_ctl.u16;
		}

		/*disable copper autonegotiation and set full duplex mode 100M*/
		if(state == 0)
		{
			copper_ctl.s.auto_negotiaton_en = 0;			
			copper_ctl.s.speedselect_bit13 = 0;
			copper_ctl.s.speedselect_bit6 = 1;
			copper_ctl.s.copper_duplex_mod = 1;
			copper_ctl.s.reset = 1;
			nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 0);
			nbm_oct_mdio_write(portNum,0,copper_ctl.u16);
			syslog_ax_nbm_eth_port_dbg("PHY1145 AUNEG disable!");
			return 0;
		}
		/*enable copper autonegotiation*/
		else if(state == 1)
		{
			copper_ctl.s.auto_negotiaton_en = 1;
			copper_ctl.s.reset = 1;
			nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 0);
			nbm_oct_mdio_write(portNum,0,copper_ctl.u16);
			syslog_ax_nbm_eth_port_dbg("PHY1145 AUNEG enable!");
			return 0;
		}
	}	
	else
	{
		syslog_ax_nbm_eth_port_err("port:%d get wrong preferred media\n", portNum);
		return NPD_DBUS_ERROR_UNSUPPORT;	
	}
}

int nbm_set_autoneg(unsigned char port, unsigned long state)
{
	int ret;
	
	if( PRODUCT_AX7000 )
	{
		if (port > 3)
		{
			syslog_ax_nbm_eth_port_err("main board port number out of range!\n");
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}			
		ret = nbm_oct_set_autoneg(port, state);
	}
	else if(PRODUCT_TYPE_5612E == nbm_get_product_type())
	{
		/*code optimize:  Unsigned compared against 0
		zhangcl@autelan.com 2013-1-17*/
		if( port > 7)
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3)		/* Cavium Eth-port */
		{			
			ret = nbm_oct_set_autoneg(port, state);
		}
		else if( (port >= 4) && (port <= 7) )		/* 82571 Eth-port */
		{
			ret = nbm_e1000e_set_autoneg(port, state);
		}
	}
	else if(PRODUCT_TYPE_5608 == nbm_get_product_type())
	{
		if( port > 3)
		{
			syslog_ax_nbm_eth_port_err("Port number (%d) is Error!\n", port);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
		else if( port <= 3 )		/* 82571 Eth-port */
		{
			ret = nbm_e1000e_set_autoneg(port, state);
		}
	}
	else
	{
		syslog_ax_nbm_eth_port_err("Product type is Error\n");
		ret = ETHPORT_RETURN_CODE_ERR_PRODUCT_TYPE;
	}

	return ret;
}

/*
 *return:6 media can't set duplex mode ,set failed
 *	     0  media duplex mode set succeed
 *@portNum:0~3
 *@state:1 full duplex
 * 		 0 half duplex
 *coded by huxuefeng20091104
 */
int nbm_set_ethport_duplex_autoneg
(
	unsigned char portNum,
	unsigned long state
)
{
	cvmx_mdio_phy1145_reg0_0 copper_ctl;
	cvmx_mdio_phy1145_reg0_1 fiber_ctl;	
	int media = 0;
	
	fiber_ctl.u16 = 0;
	copper_ctl.u16 = 0;

	if (portNum > 3)
	{
		syslog_ax_nbm_eth_port_err("bad port number %d\n", portNum);
		return ETHPORT_RETURN_CODE_NO_SUCH_PORT;	
	}
	/*code optimize:  Logically dead code state
	zhangcl@autelan.com 2013-1-17*/
	if ((state != 0) && (state != 1))
	{
		syslog_ax_nbm_eth_port_err("bad stat %d\n", state);
		return ETHPORT_RETURN_CODE_ERR_GENERAL;
	}
	
	/*get preferred media ; fiber:media = 1; copper:media = 2*/
	if (nbm_get_eth_port_trans_media(portNum, &media))
	{
		syslog_ax_nbm_eth_port_err("port:%d get preferred media failed\n", portNum);
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	/*preferred fiber*/
	if (media == 0x1)
	{
		nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 1);
		fiber_ctl.u16 = nbm_oct_mdio_read(portNum,0);
		
		/*autonegotiation is enable*/
		if (fiber_ctl.s.auto_negotiaton_en)
		{
			syslog_ax_nbm_eth_port_err("port %d autonegotiation is enable, can't set duplex mode\n", portNum);
			return ETHPORT_RETURN_CODE_UNSUPPORT;
		}
		/*set fiber duplex*/
		if (state == 1)
		{
			fiber_ctl.s.fiber_duplex_mod = 1; 
		}
		else
		{
			fiber_ctl.s.fiber_duplex_mod = 0;	
		}
		fiber_ctl.s.reset = 1;
		nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 1);
		nbm_oct_mdio_write(portNum,0,fiber_ctl.u16);
		return 0;
	}
	/*preferred copper*/
	else if (media == 0x2 || media == 0x0)
	{
		nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 0);
		copper_ctl.u16 = nbm_oct_mdio_read(portNum,0);
		/*autonegotiation is enable*/
		if (copper_ctl.s.auto_negotiaton_en)
		{
			syslog_ax_nbm_eth_port_err("port %d autonegotiation is enable, can't set duplex mode\n", portNum);
			return ETHPORT_RETURN_CODE_UNSUPPORT;
		}
		/*set fiber duplex*/
		if (state == 1)
		{
			copper_ctl.s.copper_duplex_mod= 1; 
		}
		else
		{
			copper_ctl.s.copper_duplex_mod = 0;	
		}
		copper_ctl.s.reset = 1;
		nbm_oct_mdio_write(portNum, MV_88E1145_PHY_EXT_ADDR, 0);
		nbm_oct_mdio_write(portNum, 0, copper_ctl.u16);
		return 0;
	}
	else
	{
		syslog_ax_nbm_eth_port_err("port:%d get wrong preferred media\n", portNum);
		return ETHPORT_RETURN_CODE_ERR_HW;	
	}
}

int nbm_set_ethport_fc_autoneg
(
	unsigned char portNum,
	unsigned long state,
	unsigned long pauseAdvertise
)
{
	return ETHPORT_RETURN_CODE_UNSUPPORT;
}
			

int nbm_set_ethport_speed_autoneg
(
	unsigned char portNum,
	unsigned long state
)
{
	return ETHPORT_RETURN_CODE_UNSUPPORT;
}

/*
	set Thx int enable.
	in driver must set disable.
*/
int nbm_set_ethport_POW_Int_enable
(
	void
)
{	
	bm_op_args  pow_int_args;
	int retval;
	int fd = -1;

	if(g_bm_fd < 0) {
		fd = open(NPD_BM_FILE_PATH,0);
		if(fd < 0)
		{
			syslog_ax_nbm_eth_port_err("open dev %s error(%d) when set port admin status!\n", NPD_BM_FILE_PATH, fd);
			return ETHPORT_RETURN_CODE_ERR_HW;
		}
		g_bm_fd = fd;
	}
	
    /*read out the reg's value*/
	memset(&pow_int_args, 0, sizeof(pow_int_args));
	pow_int_args.op_addr = CVMX_POW_WQ_INT_THRX(15); /*register's addr we use group 15 as receive group*/
	pow_int_args.op_len = 64;
	retval = ioctl(g_bm_fd, BM_IOC_G_, &pow_int_args); /*read reg*/
	if (retval == -1 || pow_int_args.op_ret != 0)
	{
		syslog_ax_nbm_eth_port_err("read POW thrx Int config reg error!\n");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}

	/*modify the value
	cvmx_pow_int_thrx.u64 = pow_int_args.op_value;
	syslog_ax_nbm_eth_port_dbg("sushaohua Read out Int reg value is %#0llx",cvmx_pow_int_thrx.u64);
	cvmx_pow_int_thrx.s.tc_en = 1; //modify the value
	*/

	pow_int_args.op_value = 0x1f000000;

	/*write back*/
	retval = ioctl(g_bm_fd, BM_IOC_X_, &pow_int_args);
	if(retval == -1 || pow_int_args.op_ret != 0) {
		syslog_ax_nbm_eth_port_err("write port config reg error!\n");
		return ETHPORT_RETURN_CODE_ERR_HW;
	}
	
	return NPD_SUCCESS;	
}


/*
 * Func Name: nbm_cvm_oct_get_product_type
 * Input:
 *	none
 * Output:
 *	none
 * Return:
 *	Product Type: 7~0 bit is module number; 15~8 bit is product number
 * Description:
 *	Read the CPLD product and module register and 
 *	return ( (product_num << 8)| module_num )
 */
unsigned int nbm_get_product_type(void)
{
	unsigned int product_type = 0;
	unsigned char product_num = 0;
	unsigned char module_num = 0;
	
	nbm_cpld_reg_read(CPLD_PRODUCT_CTL, &product_num);
	nbm_cpld_reg_read(CPLD_MODULE_CTL, &module_num);

	product_type = ( product_num << 8 ) | module_num;

	syslog_ax_nbm_eth_port_dbg("product type == 0x%x\n", product_type);

	return product_type;
}

/*
	CPLD reg 0x10 
	bit: [7]		[6]		[5]		[4]		[3]		[2]		[1]		[0]
		XFP1	XFP0	CARD1	CARD0	SFP3	SFP2	SFP1	SFP0
		BIT[4] ==0	CARD0 is ON.
		BIT[4] ==1	CARD0 is OFF.
*/

unsigned char nbm_cvm_query_card_on(int card_Num)
{
	unsigned char tmp;
	int ret = 0;
	
	if (card_Num > 1)
		return 0xff;
	ret = nbm_cpld_reg_read(CPLD_CARD_ON, &tmp);
	if(0 != ret) {
		syslog_ax_nbm_eth_port_err("read cpld register error. ret %#x\n",ret);
		return 0xff;
	}		
	syslog_ax_nbm_eth_port_dbg("%s = 0x%x\n",__FUNCTION__,tmp);	
	return ((tmp >> (card_Num + 4)) & 0x1);
}



/*
	CARD0 REG 0X24  BIT[3:2]
	CARD1 REG 0X25  BIT[3:2]
	01 ----- AX51-XFP
	10 ----- AX51-GTX
	11 ----- AX51-SFP
*/
unsigned char cvm_query_card_type(int card_Num)
{
	unsigned char tmp;
	int ret = 0;
	if (card_Num > 1) 
		return 0xff;
	ret = nbm_cpld_reg_read(CPLD_CARD_TYPE(card_Num), &tmp);
	if(0 != ret) {
		syslog_ax_nbm_eth_port_err("read cpld register error. ret %#x\n",ret);
		return 0xff;
	}
	syslog_ax_nbm_eth_port_dbg("%s = 0x%x\n",__FUNCTION__,tmp); 
	return ((tmp >> 2) & 0x3);
}

#ifdef __cplusplus
}
#endif

