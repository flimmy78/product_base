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
* nam_diag.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		NAM module implement for ASIC diagnosis configuration.
*
* DATE:
*		03/24/2009	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.26 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "nam_utilus.h"
#include "nam_log.h"
#include "nam_acl.h"
#include "nam_diag.h"
#include "sysdef/returncode.h"

#ifdef DRV_LIB_CPSS
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/generic/phy/private/prvCpssGenPhySmi.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiag.h>
#endif
#ifdef DRV_LIB_BCM
#include <sal/appl/pci.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/ll.h>
#include <bcm/port.h>
#include <soc/mcm/memregs.h>
#include <sal/core/libc.h>
#include <bcm/cosq.h>
#include <bcm/types.h>
#endif
/*typedef int func(int, int, int *);*/
/*

#define	mac_read	READ_COMMAND_CONFIGr

	mac_read[1] = READ_GPORT_CONFIGr;
	mac_read[2] = READ_MAC_0r;
	mac_read[3] = READ_MAC_1r;
	mac_read[4] = READ_FRM_LENGTHr;
	mac_read[5] = READ_PAUSE_QUANTr;
	mac_read[6] = READ_SFD_OFFSETr;
	mac_read[7] = READ_MAC_MODEr;
	mac_read[8] = READ_TAG_0r;
	mac_read[9] = READ_TAG_1r;
	mac_read[10] = READ_TX_IPG_LENGTHr;
	mac_read[11] = READ_PAUSE_CONTROLr;
	mac_read[12] = READ_IPG_HD_BKP_CNTLr;
	mac_read[13] = READ_FLUSH_CONTROLr;
	mac_read[14] = READ_RXFIFO_STATr;
	mac_read[15] = READ_TXFIFO_STATr;
	mac_read[16] = READ_GPORT_RSV_MASKr;
	mac_read[17] = READ_GPORT_STAT_UPDATE_MASKr;
	mac_read[18] = READ_GPORT_TPIDr;
	mac_read[19] = READ_GPORT_SOP_S1r;
	mac_read[20] = READ_GPORT_SOP_S0r;
	mac_read[21] = READ_GPORT_SOP_S3r;
	mac_read[22] = READ_GPORT_SOP_S4r;
	mac_read[23] = READ_GPORT_MAC_CRS_SELr;
*/
/*
int (*mac_read[])(int, int, int *) = {
	READ_COMMAND_CONFIGr,
	READ_GPORT_CONFIGr,
	READ_MAC_0r,
	READ_MAC_1r,
	READ_FRM_LENGTHr,
	READ_PAUSE_QUANTr,
	READ_SFD_OFFSETr,
	READ_MAC_MODEr,
	READ_TAG_0r,
	READ_TAG_1r,
	READ_TX_IPG_LENGTHr,
	READ_PAUSE_CONTROLr,
	READ_IPG_HD_BKP_CNTLr,
	READ_FLUSH_CONTROLr,
	READ_RXFIFO_STATr,
	READ_TXFIFO_STATr,
	READ_GPORT_RSV_MASKr,
	READ_GPORT_STAT_UPDATE_MASKr,
	READ_GPORT_TPIDr,
	READ_GPORT_SOP_S1r,
	READ_GPORT_SOP_S0r,
	READ_GPORT_SOP_S3r,
	READ_GPORT_SOP_S4r,
	READ_GPORT_MAC_CRS_SELr
};

int (*mac_write[])(int, int, int) = {
	WRITE_COMMAND_CONFIGr,
	WRITE_GPORT_CONFIGr,
	WRITE_MAC_0r,
	WRITE_MAC_1r,
	WRITE_FRM_LENGTHr,
	WRITE_PAUSE_QUANTr,
	WRITE_SFD_OFFSETr,
	WRITE_MAC_MODEr,
	WRITE_TAG_0r,
	WRITE_TAG_1r,
	WRITE_TX_IPG_LENGTHr,
	WRITE_PAUSE_CONTROLr,
	WRITE_IPG_HD_BKP_CNTLr,
	WRITE_FLUSH_CONTROLr,
	WRITE_RXFIFO_STATr,
	WRITE_TXFIFO_STATr,
	WRITE_GPORT_RSV_MASKr,
	WRITE_GPORT_STAT_UPDATE_MASKr,
	WRITE_GPORT_TPIDr,
	WRITE_GPORT_SOP_S1r,
	WRITE_GPORT_SOP_S0r,
	WRITE_GPORT_SOP_S3r,
	WRITE_GPORT_SOP_S4r,
	WRITE_GPORT_MAC_CRS_SELr
};
*/
/*int (*mac_write[2])(1, 2, 3);*/
#ifdef DRV_LIB_BCM
#define BCM_RAPTOR_PHY_ADDR_DEFAULT(_unit, _port, _phy_addr, _phy_addr_int)	\
    do {                                                                	\
       /*                                                               							\
        * MDIO address and MDIO bus select for External Phy             				\
        * port 0  - 0  CPU (_port) no external MDIO address             					\
        * port 1  - 2  MDIO address (undetermined)                    					\
        * port 3  - 3  No internal MDIO                                 						\
        * port 4  - 5  MDIO address (undetermined)                    					\
        * port 6  - 29 MDIO address 1,24 (0x1 - 0x18)                   					\
        * port 30 - 53 MDIO address 1,24 (0x41 - 0x58 with Bus sel set) 			\
        */                                                              		\
        _phy_addr = (_port > 5) ? (_port - 5) : (0);						\
        if((0x0 == _unit)&&(0x1 == _port))									\
			_phy_addr = 0x19;												\
        if((0x1 == _unit)&&(0x1 == _port))									\
			_phy_addr = 0x1A;												\
      /*                                                                							\
       * MDIO address and MDIO bus select for Internal Phy (Serdes)     				\
       * port 0  - 0  CPU (_port) no internal MDIO address              					\
       * port 1  - 2  MDIO address 1,2 (0x81 - 0x82 with Intenal sel set) 			\
       * port 3  - 3  No internal MDIO                                    						\
       * port 4  - 5  MDIO address 4, 5 (0x84 - 0x85 with Intenal sel set)			\
       * port 6  - 29 MDIO address 6,29 (0x86 - 0x9D with Intenal sel set)			\
       * port 30 - 53 MDIO address 0,23 (0xc0 - 0xd7 with Intenal sel set)			\
       */                                                               		\
        _phy_addr_int = (_port) +                                    		\
                           (((_port) > 29) ? (0xc0 - 30) : (0x80));     	\
     } while (0)
#endif
int nam_asic_phy_read
(
	unsigned int opDevice,
	unsigned char opPort,
	unsigned char opRegaddr,
	unsigned short *regValue
)
{
	unsigned long ret = 0;
	unsigned short value = 0;
	
#ifdef DRV_LIB_BCM
	unsigned short phy_addr = 0, phy_addr_int = 0;
	BCM_RAPTOR_PHY_ADDR_DEFAULT(opDevice, opPort, phy_addr, phy_addr_int);
	ret = soc_miim_read(opDevice, phy_addr, opRegaddr, &value);
#endif
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPhyPortSmiRegisterRead(opDevice, opPort, opRegaddr, &value);
#endif
	/*code optimize:	Unsigned compared against 0
	zhangcl@autelan.com 2013-1-17*/

	if (ret != DIAG_RETURN_CODE_SUCCESS) 
	{
		nam_syslog_dbg("ERROR: read port(%d,%d) phy register %d failed\n",
							  opDevice, opPort, opRegaddr);
		return DIAG_RETURN_CODE_ERROR;
	}
	nam_syslog_dbg("port(%d,%d) phy register %d value %#x\n",
							  opDevice, opPort, opRegaddr, value);
	*regValue = value;

	return DIAG_RETURN_CODE_SUCCESS;
}

int nam_asic_phy_write
(
	unsigned int opDevice,
	unsigned char opPort,
	unsigned char opRegaddr,
	unsigned short regValue
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_BCM
	unsigned short phy_addr = 0, phy_addr_int = 0;
	BCM_RAPTOR_PHY_ADDR_DEFAULT(opDevice, opPort, phy_addr, phy_addr_int);
	ret = soc_miim_write(opDevice, phy_addr, opRegaddr, regValue);
#endif
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPhyPortSmiRegisterWrite(opDevice, opPort, opRegaddr, regValue);
#endif
	/*code optimize:	Unsigned compared against 0
	zhangcl@autelan.com 2013-1-17*/

	if (ret != DIAG_RETURN_CODE_SUCCESS) 
	{
		nam_syslog_dbg("ERROR: write port(%d,%d) phy register %d value %#x failed\n",
							  opDevice, opPort, opRegaddr, regValue);
		return DIAG_RETURN_CODE_ERROR;
	}

	/* test read back */
	unsigned short value = 0;
#ifdef DRV_LIB_BCM
	ret = soc_miim_read(opDevice, phy_addr, opRegaddr, &value);	
#endif
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPhyPortSmiRegisterRead(opDevice, opPort, opRegaddr, &value);
#endif
	/*code optimize:	Unsigned compared against 0
	zhangcl@autelan.com 2013-1-17*/

	if (ret != DIAG_RETURN_CODE_SUCCESS) 
	{
		nam_syslog_dbg("ERROR: read back port(%d,%d) phy register %d failed\n",
							  opDevice, opPort, opRegaddr);
		return DIAG_RETURN_CODE_ERROR;
	}
	nam_syslog_dbg("port(%d,%d) phy register %d write %#x read back %#x\n",
							  opDevice, opPort, opRegaddr, regValue, value);

	return DIAG_RETURN_CODE_SUCCESS;
}

/*******************************************************************************
* nam_vct_phy_enable
*
* DESCRIPTION:
*       enable port phy vct 
*
* INPUTS:
*       opDevice   - physical device number
*       opPort	   - physical port number
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*       DIAG_RETURN_CODE_SUCCESS          - on success
*       DIAG_RETURN_CODE_ERROR			- on fail.
*
*******************************************************************************/
int nam_vct_phy_enable
(
	unsigned int opDevice,
	unsigned char opPort
)
{
	unsigned int ret = 0;
	unsigned short value = 0, page = 0;
	
#ifdef DRV_LIB_BCM
	unsigned short phy_addr = 0, phy_addr_int = 0;
	unsigned int times = 0;
	BCM_RAPTOR_PHY_ADDR_DEFAULT(opDevice, opPort, phy_addr, phy_addr_int);
	ret = soc_miim_write(opDevice, phy_addr, 0x1f, 0x008b);
	ret = soc_miim_read(opDevice,  phy_addr, 0x14, &value);
	while(value & (1<<6)) 
	{
		sleep(1);
		times++;
		if (times > 5) 
		{
			return DIAG_RETURN_CODE_ERROR;
		}
		ret = soc_miim_read(opDevice,  phy_addr, 0x14, &value);
	}	
	ret = soc_miim_write(opDevice, phy_addr, 0x14, 0x0000);
	ret = soc_miim_write(opDevice, phy_addr, 0x13, 0x0000);
	ret = soc_miim_write(opDevice, phy_addr, 0x14, 0x0200);
	ret = soc_miim_write(opDevice, phy_addr, 0x13, 0x4824);
	ret = soc_miim_write(opDevice, phy_addr, 0x13, 0x4000);
	ret = soc_miim_write(opDevice, phy_addr, 0x13, 0x0500);
	ret = soc_miim_write(opDevice, phy_addr, 0x13, 0xc404);
	ret = soc_miim_write(opDevice, phy_addr, 0x13, 0x0100);
	ret = soc_miim_write(opDevice, phy_addr, 0x13, 0x004c);
	ret = soc_miim_write(opDevice, phy_addr, 0x13, 0x8006);
	ret = soc_miim_write(opDevice, phy_addr, 0x14, 0x0000);
#endif
#ifdef DRV_LIB_CPSS
	unsigned short phyType = 0; 		  /* PHY type */
	unsigned long isMarvellPhy = 0;	  /* indicator whether the PHY is a Marvell PHY */
	/* get PHY type. Don't check return status. It may be not GT_OK if PHY
	   not connected to a port */
	cpssGenPhyTypeGet(opDevice, opPort, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);
	
	/* currently support for Marvell PHYs   ...result = GT_NOT_SUPPORTED; */
	if (isMarvellPhy) 
	{
		/* for PHY 1240 support combo ports need check , is set ok */
		if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1240) 
		{
			ret = nam_asic_phy_read(opDevice, opPort, 0x16, &page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 0x5);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("write device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x17, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			value |= 0x8000;
			ret = nam_asic_phy_write(opDevice, opPort, 0x17, value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("write device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("write device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
		}
		/* for PHY 1112 support combo ports */
		else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) 
		{	
			ret = nam_asic_phy_read(opDevice, opPort, 0x16, &page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 0x5);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("write device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x17, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			value |= 0x8000;
			ret = nam_asic_phy_write(opDevice, opPort, 0x10, value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("write device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			/* need check vct is happen or not*/
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("write device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
		}
		/*for 7k 1145*/
		else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E114X) 
		{		
			ret = nam_asic_phy_read(opDevice, opPort, 0x16, &page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 0x5);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("write device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x1c, 0x8000);	
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("write device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("write device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
		}
		/*for xcat*/
#ifdef DRV_LIB_CPSS_3_4		
		else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340) 
		{
			ret = nam_asic_phy_read(opDevice, opPort, 0x16, &page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 0x7);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("write device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x15, 0x8400);	
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret =nam_asic_phy_write(opDevice, opPort, 0x16, page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
		}
#endif		
		else 
		{
			return DIAG_RETURN_CODE_ERROR;
		}
	}
#endif	
	/*code optimize:  Logically dead code
	zhangcl@autelan.com 2013-1-17*/

	return ret;
}

/*******************************************************************************
* nam_vct_phy_enable
*
* DESCRIPTION:
*       enable port phy vct 
*
* INPUTS:
*       opDevice   - physical device number
*       opPort	   - physical port number
*
* OUTPUTS:
*       state   - vct state
*       len	    - copper len
*
* RETURNS:
*       DIAG_RETURN_CODE_SUCCESS          - on success
*       DIAG_RETURN_CODE_ERROR			- on fail.
*
*******************************************************************************/
int nam_vct_phy_read
(
	unsigned int opDevice,
	unsigned char opPort,
	unsigned short *state,
	unsigned int *len
)
{
	unsigned int ret = 0, i = 0, tmp = 0;
	unsigned short value = 0, page = 0;
	
#ifdef DRV_LIB_BCM
	unsigned short phy_addr = 0, phy_addr_int = 0;
	BCM_RAPTOR_PHY_ADDR_DEFAULT(opDevice, opPort, phy_addr, phy_addr_int);
	/*add value err test later*/
	*state = 0xff00;
	ret = soc_miim_read(opDevice, phy_addr, 0x13, &value);
	while(i < 4) 
	{
		tmp = value>>(10+i) & 3;
		/*normal*/
		if (!tmp) 
		{
			*state |= ~(0xf<<(i*4));
		}
		/*short*/
		else if (2 == tmp) 
		{
			*state |= 0x1<<(i*4);
		}
		/*open*/
		else if (1 == tmp) 
		{
			*state |= 0x2<<(i*4);
		}
		/*fail*/
		else 
		{
			*state |= 0xf<<(i*4);
		}			
		i += 2;
	}
	ret = soc_miim_read(opDevice, phy_addr, 0x13, &value);
	*len = value * 4 / 5;

	soc_miim_write(opDevice, phy_addr, 0x1f, 0x000b);
#endif
#ifdef DRV_LIB_CPSS
	unsigned short phyType = 0; 		  /* PHY type */
	unsigned long isMarvellPhy = 0;	  /* indicator whether the PHY is a Marvell PHY */	
	/* get PHY type. Don't check return status. It may be not GT_OK if PHY
	   not connected to a port */
	cpssGenPhyTypeGet(opDevice, opPort, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);
	
	/* currently support for Marvell PHYs   ...result = GT_NOT_SUPPORTED; */
	if (isMarvellPhy) 
	{
		/* for PHY 1240 support combo ports need check , is set ok */
		if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1240) 
		{
			ret = nam_asic_phy_read(opDevice, opPort, 0x16, &page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 5);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x10, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*len |= tmp & 0xff;
			ret = nam_asic_phy_read(opDevice, opPort, 0x11, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*len |= tmp<<8 & 0xff00;
			ret = nam_asic_phy_read(opDevice, opPort, 0x12, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*len |= tmp<<16 & 0xff0000;
			ret = nam_asic_phy_read(opDevice, opPort, 0x13, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*len |= tmp<<24 & 0xff000000;			
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
		}
		/* for PHY 1112 support combo ports */
		else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) 
		{
			*state = 0xcccc;
			ret = nam_asic_phy_read(opDevice, opPort, 0x16, &page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 5);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x10, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*state |= tmp>>13 & 0x3;
			*len |= tmp & 0xff;
			ret = nam_asic_phy_read(opDevice, opPort, 0x11, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*state |= tmp>>9 & 0x30;
			*len |= tmp<<8 & 0xff00;
			ret = nam_asic_phy_read(opDevice, opPort, 0x12, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*state |= tmp>>5 & 0x300;
			*len |= tmp<<16 & 0xff0000;
			ret = nam_asic_phy_read(opDevice, opPort, 0x13, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*state |= tmp>>1 & 0x3000;
			*len |= tmp<<24 & 0xff000000;			
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
		}
		/*for 7k 1145*/
		else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E114X) 
		{
			*state = 0xcccc;
			ret = nam_asic_phy_read(opDevice, opPort, 0x16, &page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 0);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x10, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*state |= tmp>>13 & 0x3;
			*len |= tmp & 0xff;
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 1);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x11, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*state |= tmp>>9 & 0x30;
			*len |= tmp<<8 & 0xff00;
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 2);if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x12, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*state |= tmp>>5 & 0x300;
			*len |= tmp<<16 & 0xff0000;
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 3);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x13, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*state |= tmp>>1 & 0x3000;
			*len |= tmp<<24 & 0xff000000;			
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
		}
		/*for xcat*/
#ifdef DRV_LIB_CPSS_3_4		
		else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340) 
		{
			/*set phy page to 7*/
			ret = nam_asic_phy_read(opDevice, opPort, 0x16, &page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 0x7);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x14, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			while(i < 4) 
			{
				tmp = value>>(i*4) & 3;
				/*normal*/
				if (1 == tmp) 
				{
					*state |= ~(0xf<<(i*4));
				}
				/*short*/
				else if (3 == tmp) 
				{
					*state |= 0x1<<(i*4);
				}
				/*open*/
				else if (2 == tmp) 
				{
					*state |= 0x2<<(i*4);
				}
				/*fail*/
				else 
				{
					*state |= 0xf<<(i*4);
				}			
				i++;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x10, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*len |= tmp & 0xff;
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 1);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x11, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*len |= tmp<<8 & 0xff00;
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 2);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x12, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*len |= tmp<<16 & 0xff0000;
			ret = nam_asic_phy_write(opDevice, opPort, 0x16, 3);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			ret = nam_asic_phy_read(opDevice, opPort, 0x13, &value);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
			tmp = value;
			*len |= tmp<<24 & 0xff000000;	

			ret = nam_asic_phy_write(opDevice, opPort, 0x16, page);
			if(ret != DIAG_RETURN_CODE_SUCCESS)
			{
				nam_syslog_err("read device %d,port %d failed\n",opDevice,opPort);
				return DIAG_RETURN_CODE_ERROR;
			}
		}
#endif		
		else 
		{
			return DIAG_RETURN_CODE_ERROR;
		}
	}	
#endif
	/*code optimize:  Logically dead code
	zhangcl@autelan.com 2013-1-17*/

	return DIAG_RETURN_CODE_SUCCESS;
}

/*******************************************************************************
* nam_asic_auxi_phy_read
*
* DESCRIPTION:
*       Read specified 10G SMI Register and PHY device of specified port
*       on specified device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       phyId   - ID of external 10G PHY (value of 0..31).
*       useExternalPhy - boolean variable, defines if to use external 10G PHY
*       phyReg  - 10G SMI register, the register of PHY to read from
*       phyDev  - the PHY device to read from (value of 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       DIAG_RETURN_CODE_SUCCESS          - on success
*       DIAG_RETURN_CODE_ERROR		- on fail.
*
*******************************************************************************/
unsigned int nam_asic_auxi_phy_read
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char phyId,
	unsigned char useExternalPhy,
	unsigned short phyReg,
	unsigned char phyDev,
	unsigned short *dataPtr
)
{
	unsigned int ret = 0;
	unsigned short value = 0;
	
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPhyPort10GSmiRegisterRead(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, &value);
#endif
	if (ret != 0) {
		nam_syslog_err("ERROR: read auxi-phy device %d port %d phyID %d %s Regaddr %#x phyDve %d, ret %x.\n", 
				devNum, portNum, phyId,
				useExternalPhy ? "external":"internal",
				phyReg, phyDev, ret);
		return DIAG_RETURN_CODE_ERROR;
	}
	nam_syslog_dbg("read auxi-phy device %d port %d phyID %d %s Regaddr %#x phyDve %d value %#x.\n", 
			devNum, portNum, phyId,
			useExternalPhy ? "external":"internal",
			phyReg, phyDev, value);

	*dataPtr = value;

	return DIAG_RETURN_CODE_SUCCESS;
}

/*******************************************************************************
* nam_asic_auxi_phy_write
*
* DESCRIPTION:
*       Write value to a specified 10G SMI Register and PHY device of
*       specified port on specified device.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyId       - ID of external 10G PHY (value of 0...31).
*       useExternalPhy - Boolean variable, defines if to use external 10G PHY
*       phyReg      - 10G SMI register, the register of PHY to read from
*       phyDev      - the PHY device to read from (value of 0..31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       DIAG_RETURN_CODE_SUCCESS            - on success
*       DIAG_RETURN_CODE_ERROR		- on fail.
*
*******************************************************************************/
int nam_asic_auxi_phy_write
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char phyId,
	unsigned char useExternalPhy,
	unsigned short phyReg,
	unsigned char phyDev,
	unsigned short dataPtr
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, dataPtr);
#endif
	/*code optimize: 	Unsigned compared against 0
	zhangcl@autelan.com 2013-1-17*/
	if (ret != DIAG_RETURN_CODE_SUCCESS) 
	{
		nam_syslog_err("ERROR: write auxi-phy device %d port %d phyID %d %s Regaddr %#x phyDve %d, ret %x.\n", 
				devNum, portNum, phyId,
				useExternalPhy ? "external":"internal",
				phyReg, phyDev, ret);
		return DIAG_RETURN_CODE_ERROR;
	}

	/* test read back */
	unsigned short value = 0;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPhyPort10GSmiRegisterRead(devNum, portNum, phyId, useExternalPhy, phyReg, phyDev, &value);
#endif
	if (ret != 0) 
	{
		nam_syslog_err("ERROR: read auxi-phy device %d port %d phyID %d %s Regaddr %#x phyDve %d, ret %x.\n", 
				devNum, portNum, phyId,
				useExternalPhy ? "external":"internal",
				phyReg, phyDev, ret);
		return DIAG_RETURN_CODE_ERROR;
	}
	nam_syslog_dbg("read auxi-phy device %d port %d phyID %d %s Regaddr %#x phyDve %d value %#x.\n", 
			devNum, portNum, phyId,
			useExternalPhy ? "external":"internal",
			phyReg, phyDev, value);

	return DIAG_RETURN_CODE_SUCCESS;
}

/*******************************************************************************
* nam_asic_show_mib_ge
*
* DESCRIPTION:
*       show mib of ge port
*
* INPUTS:
*       NULL
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*       DIAG_RETURN_CODE_SUCCESS          - on success
*       DIAG_RETURN_CODE_ERROR		- on fail.
*
*******************************************************************************/
unsigned int nam_asic_show_mib_ge
(
	void
)
{
	unsigned int ret = 0;
	
#ifdef DRV_LIB_CPSS
	ret = geportmibreadall();
#endif
	if (ret != 0) {
		nam_syslog_err("show mib ge error %x\n",  ret);
		return DIAG_RETURN_CODE_ERROR;
	}

	return DIAG_RETURN_CODE_SUCCESS;
}

/*******************************************************************************
* nam_asic_show_mib_xg
*
* DESCRIPTION:
*       show mib of xg port
*
* INPUTS:
*       NULL
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*       DIAG_RETURN_CODE_SUCCESS          - on success
*       DIAG_RETURN_CODE_ERROR		- on fail.
*
*******************************************************************************/
unsigned int nam_asic_show_mib_xg
(
	void
)
{
	unsigned int ret = 0;
	
#ifdef DRV_LIB_CPSS
	ret = xgportmibreadall();
#endif
	if (ret != 0) {
		nam_syslog_err("show mib xg error %x\n",  ret);
		return DIAG_RETURN_CODE_ERROR;
	}

	return DIAG_RETURN_CODE_SUCCESS;
}

int nam_asic_pci_read
(
	unsigned int opDevice,
	unsigned int opRegaddr,
	unsigned int *regValue
)
{
	unsigned int ret = 0;
	unsigned int value = 0;
	
#ifdef DRV_LIB_BCM
	value = soc_pci_read(opDevice, opRegaddr);
	if (ret < 0) {
		nam_syslog_dbg("ERROR: unit %d pci addr 0x%#x: soc_miim_read failed\n",
							  opDevice);
		return DIAG_RETURN_CODE_ERROR;
	}
	nam_syslog_dbg("unit %d pci addr 0x%#x value 0x%#x\n",
							  opDevice, opRegaddr, value);
	*regValue = value;
#endif
	
	return DIAG_RETURN_CODE_SUCCESS;
}

int nam_asic_pci_write
(
	unsigned int opDevice,
	unsigned int opRegaddr,
	unsigned int regValue
)
{
	unsigned int ret = 0;

#ifdef DRV_LIB_BCM
	ret = soc_pci_write(opDevice, opRegaddr, regValue);
	if (ret < 0) {
		nam_syslog_dbg("ERROR: unit %d mii addr 0x%#x: soc_miim_write failed\n",
							  opDevice);
		return DIAG_RETURN_CODE_ERROR;
	}
	nam_syslog_dbg("phy write unit %d mii addr %#x value %#x read back %#x\n",
							  opDevice, regValue, ret);
#endif
	
	return DIAG_RETURN_CODE_SUCCESS;
}

int nam_board_cpu_read_reg
(
	unsigned int regAddr,
	unsigned int *regValue
)
{
	int ret = 0;
	unsigned int data = 0;

#ifdef DRV_LIB_CPSS
#ifdef DRV_LIB_CPSS_3_4
	ret = extDrvBoardCpuReadReg(regAddr, &data);
	if(ret) {
		nam_syslog_err("ERROR: read cpu register %#x data %#x\n", regAddr, data);
		return ret;
	}

	nam_syslog_dbg("diag nam read cpu register %#x data %#x ok\n", regAddr, data);
	*regValue = data;
#endif
#endif
	return DIAG_RETURN_CODE_SUCCESS;
}

int nam_board_cpu_write_reg
(
	unsigned int regAddr,
	unsigned int regValue
)
{
	int ret = 0;
#ifdef DRV_LIB_CPSS
#ifdef DRV_LIB_CPSS_3_4
	ret = extDrvBoardCpuWriteReg(regAddr, regValue);
	if(ret) {
		nam_syslog_err("ERROR: write cpu register %#x value %#x\n", regAddr, regValue);
		return ret;
	}
	nam_syslog_dbg("diag write cpu register %#x value %#x ok\n", regAddr, regValue);
#endif
#endif
	return DIAG_RETURN_CODE_SUCCESS;
}

int nam_field_dump_br(int unit, int gnum, char *strbuff)
{	
#ifdef DRV_LIB_BCM	
	int msgid;	
	key_t key;
	char *str;
	char *showStr = NULL,*cursor = NULL;
	int totalLen = 0;
	int ret = 0;
	int i = 0;
	
	showStr = (char*)malloc(102400);
	if(NULL == showStr) {
		npd_syslog_dbg("Malloc failed!\n");
		return NULL;
	}
	memset(showStr,0,102400);	
	cursor = showStr;
	
	str = (char *)malloc(1000);
	memset(str, 0, 1000);
	
	key = ftok("/etc/hosts", 's');	
	if (key==-1) {		
		nam_syslog_dbg("ftok()");		
		/*exit(1);	*/
	}	
	msgid=msgget(key, IPC_CREAT|0600);
	if (msgid==-1) {		
		npd_syslog_dbg("msgget()");		
		/*exit(1);*/	
	}
	nam_syslog_dbg("dump unit %d, group %d\n", unit, gnum);
	bcm_field_group_dump(unit, gnum);
	
	while (1) {	
		ret = msgrcv(msgid, str, 1000, 0, 0);
		if (ret==-1) {
			if (errno==EINTR)   {
				continue;
			}	
			npd_syslog_dbg("msgrcv()");	
			break;	
		}
		nam_syslog_dbg("run time is %d", i);
		if (0 == strcmp(str, "endofgroup")) {
			break;
		}
		nam_syslog_dbg("rcver str is %s, len is %d \n", str, strlen(str));
		strcat(showStr, str);
		memset(str, 0, 1000);
		i++;
	}
	strncpy(strbuff, showStr, strlen(showStr));
	
	msgctl(msgid, IPC_RMID, NULL);
	nam_syslog_dbg("dump unit 0, group 1 \n");
#endif

	return DIAG_RETURN_CODE_SUCCESS;
}

int nam_cosq_dump_br(int unit, int port, char *strbuff)
{
#ifdef DRV_LIB_BCM
	bcm_cos_queue_t	cosq;
	int		        weights[BCM_COS_COUNT];
	int			delay = 0;
	int 			i = 0, r = 0;
	int			numq, mode = 0;
	char		*mode_name;
	char		*weight_str = "packets";
	int 			weight_max;
	soc_port_t          p;
	bcm_pbmp_t temp_pbmp;
	char *showStr = NULL,*cursor = NULL;
	int totalLen = 0;
	unsigned int kbits_sec_min, kbits_sec_max, bw_flags;
	unsigned int          rate = 0,  burst = 0;
	int count = 64;
	int srccp, mapcp, prio, cng;
	int cfi, int_prio, color;
	  
	showStr = (char*)malloc(102400);
	if(NULL == showStr) {
		nam_syslog_dbg("Malloc failed!\n");
		return NULL;
	}
	memset(showStr,0,102400);	
	cursor = showStr;

	p = port;
	memset(weights, 0, BCM_COS_COUNT * sizeof(int));
/*	
	bcm_cosq_sched_get(unit, &mode, weights, &delay);
	
	if (r == BCM_E_UNAVAIL) {
	r = 0;
	mode = -1;
	} else if (r < 0) {
	goto bcm_err;
	}
*/
	SOC_PBMP_PORT_ADD(temp_pbmp, p);

	r = bcm_cosq_port_sched_get(unit, temp_pbmp, &mode, weights, &delay);
	if (r < 0) {
	    return DIAG_RETURN_CODE_ERROR;
	}

	if ((r = bcm_cosq_config_get(unit, &numq)) < 0) {
	    return DIAG_RETURN_CODE_ERROR;
	}
	numq = 8;
	if ((r = bcm_cosq_sched_weight_max_get(unit,
										   mode, &weight_max)) < 0) {
		return DIAG_RETURN_CODE_ERROR;
	}
					 
	totalLen += sprintf(cursor, "\n  Port %s COS configuration:\n", SOC_PORT_NAME(unit, p));
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, " ------------------------------\n");
	cursor = showStr + totalLen;

	switch (mode) {
	case BCM_COSQ_STRICT:
	    mode_name = "strict";
	    break;
	case BCM_COSQ_ROUND_ROBIN:
	    mode_name = "simple round-robin";
	    break;
	case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
	    mode_name = "weighted round-robin";
	    break;
	case BCM_COSQ_WEIGHTED_FAIR_QUEUING:
	    mode_name = "weighted fair queuing";
	    break;
	case BCM_COSQ_BOUNDED_DELAY:
	    mode_name = "weighted round-robin with bounded delay";
	    break;
	case BCM_COSQ_DEFICIT_ROUND_ROBIN:
	    mode_name = "deficit round-robin";
	    weight_str = "Kbytes";
	    break;
	default:
	    mode_name = NULL;
	    break;
	}

	if ((r = bcm_cosq_sched_weight_max_get(unit,
	                                       mode, &weight_max)) < 0) {
		return DIAG_RETURN_CODE_ERROR;
	}

	totalLen += sprintf(cursor, "  Config (max queues): %d\n", numq);
	cursor = showStr + totalLen;
	if (mode_name) {
		totalLen += sprintf(cursor,"  Schedule mode: %s\n", mode_name);
		cursor = showStr + totalLen;
	}

	if (mode >= 0 && mode != BCM_COSQ_STRICT) {
	totalLen += sprintf(cursor,"  Weighting (in %s):\n", weight_str);
	cursor = showStr + totalLen;

	for (cosq = 0; cosq < numq; cosq++) {
		if (weight_max == BCM_COSQ_WEIGHT_UNLIMITED) {
			totalLen += sprintf(cursor,"    COSQ %d = %u %s\n",
			       cosq, (uint32) weights[cosq], weight_str);
			cursor = showStr + totalLen;
		} 
		else {
			totalLen += sprintf(cursor,"    COSQ %d = %d %s\n",
			       cosq, weights[cosq], weight_str);
			cursor = showStr + totalLen;
		}
	}
	/*
	BCM_GPORT_LOCAL_SET(gport, p);
	if (bcm_cosq_gport_sched_get(unit, gport, 8, &mode, &weights[0]) == 0) {
	printk("    COSQ 8 = %d %s (queue for output of S1 scheduler)\n",
	   weights[0], weight_str);
	}
	*/
	}
	
	totalLen += sprintf(cursor, " ------------------------------\n");
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor,"  Priority to queue mappings:\n");
	cursor = showStr + totalLen;

	for (prio = 0; prio < 8; prio++) {
		if ((r = bcm_cosq_port_mapping_get(unit, p, prio, &cosq)) < 0) {
			return DIAG_RETURN_CODE_ERROR;
		}
		totalLen += sprintf(cursor,"    PRIO %d ==> COSQ %d\n", prio, cosq);
		cursor = showStr + totalLen;
	}

	/* Only newer XGS3 chips support more than 8 priorities */
	/*
	for (prio = 8; prio < 16; prio++) {
	    if (bcm_cosq_port_mapping_get(unit, p, prio, &cosq) < 0) {
	        break;
	    }
	    totalLen = sprintf(cursor,"    PRIO %d ==> COSQ %d\n", prio, cosq);
	    cursor = showStr + totalLen;
	}
*/
	if (mode == BCM_COSQ_BOUNDED_DELAY) {
	    totalLen += sprintf(cursor, "  Bounded delay: %d usec\n", delay);
	    cursor = showStr + totalLen;
	}
	
	totalLen += sprintf(cursor, " ------------------------------\n");
	cursor = showStr + totalLen;	
	prio = 0;
	for (i = 0; i < count; i++) {
		r = bcm_port_dscp_map_get(unit, port, srccp + i, &mapcp,
										   &prio);
		if (r < 0) {
			nam_syslog_dbg("ERROR: dscp map get failed %d\n", r);
			return DIAG_RETURN_CODE_ERROR;
		}
		if (srccp + i != mapcp || count == 1) {
			totalLen += sprintf(cursor, " srcdscp=%d ==> mapeddscp=%d prio=%d cng=%d\n",
			   srccp + i, mapcp,
			   prio & BCM_PRIO_MASK,
			   (prio & BCM_PRIO_DROP_FIRST) ? 1 : 0);
			cursor = showStr + totalLen;
		}
	}
	
	totalLen += sprintf(cursor, " ------------------------------\n");
	cursor = showStr + totalLen;	
	prio = 0;	
	for (prio = BCM_PRIO_MIN; prio <= BCM_PRIO_MAX; prio++) {
		for (cfi = 0; cfi <= 1; cfi++) {
		    if ((r = bcm_port_vlan_priority_map_get(unit, port, 
		              prio, cfi, &int_prio, &color)) < 0) {
		         return DIAG_RETURN_CODE_ERROR;
		    }
		    totalLen += sprintf(cursor, " Packet Prio=%d, CFI=%d, Internal Prio=%d, "
		           "Color=%d\n",
		            prio, cfi, int_prio, color);
		    cursor = showStr + totalLen;
		} 
	}
	
	totalLen += sprintf(cursor, " ------------------------------\n");
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, " COSQ bandwith configuration:\n");
	cursor = showStr + totalLen;
	
	r = bcm_port_rate_egress_get(unit, p, &rate, &burst);
	if (r < 0) {
		return DIAG_RETURN_CODE_ERROR;
	}
	if (rate) { 
		totalLen += sprintf(cursor, "\t Egress meter:  %8d kbps %8d kbits max burst.\n",
			   rate, burst);
		cursor = showStr + totalLen;
	}
	totalLen += sprintf(cursor, " ------------------------------\n");
	cursor = showStr + totalLen;

	totalLen += sprintf(cursor, " port | q | KbpsMin  | KbpsMax  | Flags\n");
	cursor = showStr + totalLen;

	totalLen += sprintf(cursor, " -----+---+----------+----------+-------\n");
	cursor = showStr + totalLen;
	for (cosq = 0; cosq < numq; cosq++) {
	    if ((r = bcm_cosq_port_bandwidth_get(unit, p, cosq, 
	               &kbits_sec_min, &kbits_sec_max, &bw_flags)) < 0) {
	        return DIAG_RETURN_CODE_ERROR;
	    }
	    totalLen += sprintf(cursor, " %4s | %d | %8d | %8d | %6d\n",
	           SOC_PORT_NAME(unit, p), cosq, kbits_sec_min,
	           kbits_sec_max, bw_flags);
	    cursor = showStr + totalLen;
	}
	totalLen += sprintf(cursor, " -----+---+----------+----------+-------\n");
	cursor = showStr + totalLen;
	
	strncpy(strbuff, showStr, strlen(showStr));
	nam_syslog_dbg("showstr is %s len is %d\n", showStr, strlen(showStr));
	free(showStr);
#endif

	return DIAG_RETURN_CODE_SUCCESS;		
}

int nam_read_reg_br(int unit, int reg_num, int port, unsigned int *dataret)
{
	int ret = 0;
#ifdef DRV_LIB_BCM
	if (65535 == port) {
		port = -10;
	}
	ret = soc_reg32_read(unit, soc_reg_addr(unit, reg_num, port, 0), dataret);
	if (ret < 0) {
		nam_syslog_dbg("ERROR: soc_reg32_read failed addr 0x%#x\n", reg_num);
	}
#endif

	return ret;
}

int nam_write_reg_br(int unit, int reg_num, int port, unsigned int data)
{
	int ret = 0;
#ifdef DRV_LIB_BCM
	if (65535 == port) {
		port = -10;
	}
	ret = soc_reg32_write(unit, soc_reg_addr(unit, reg_num, port, 0), data);
	if (ret < 0) {
		nam_syslog_dbg("ERROR: soc_reg32_write failed addr 0x%#x\n", reg_num);
	}
#endif

	return ret;
}

int nam_read_reg(int unit, unsigned int portGroupId, unsigned int regaddr, unsigned int *dataret)
{
	int ret = 0;
	unsigned int value = 0;
#ifdef DRV_LIB_BCM
	ret = soc_reg32_read(unit, regaddr, &value);
	if (ret < 0) {
		nam_syslog_dbg("ERROR: soc_reg32_read failed addr 0x%#x\n", regaddr);
	}
#endif

#ifdef DRV_LIB_CPSS
#ifdef DRV_LIB_CPSS_3_4
	ret = cpssDrvPpHwRegisterRead(unit, portGroupId, regaddr, &value);
#else
	ret = cpssDrvPpHwRegisterRead(unit, regaddr, &value);
#endif
	if (0 != ret) {
		nam_syslog_dbg("ERROR: read device %d register %#x failed ret is %d\n", 
				unit, regaddr, ret);
		return 1;
	}
#endif

	*dataret = value;
	return ret;
}

int nam_write_reg(int unit, unsigned int portGroupId, unsigned int regaddr, unsigned int data)
{
	int ret = 0;
#ifdef DRV_LIB_BCM

	ret = soc_reg32_write(unit, regaddr, data);
	if (ret < 0) {
		nam_syslog_dbg("ERROR: soc_reg32_write failed addr 0x%#x\n", regaddr);
	}
#endif

#ifdef DRV_LIB_CPSS
#ifdef DRV_LIB_CPSS_3_4
	ret = cpssDrvPpHwRegisterWrite(unit, portGroupId, regaddr, data);
#else
	ret = cpssDrvPpHwRegisterWrite(unit, regaddr, data);
#endif	
	if (0 != ret) {
		nam_syslog_dbg("ERROR: write device %d register %#x data %#x failed ret is %d\n",
				unit, regaddr, data, ret);
		return 1;
	}
#endif

	return ret;
}

int nam_cscd_port_prbs_test(unsigned int opDevice, unsigned int *Gpmem, unsigned int *Gpres)
{
	#define LANE_NUM_SUM 4
	#define MAX_GROUP_NUM 4
	#define MAX_GPMEM_NUM 16
	int i = 0, j = 0;
	int gp_num = 0, gp_mem_num = 0;
	int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned long locked = 0;
	unsigned long errorCnt = 0;
	unsigned long patternCnt = 0;
	unsigned long enable = 0;
    unsigned int devNum = opDevice;
	unsigned int portNum = 0;
	unsigned int transmit_mode = 0;
    unsigned int result = 0;
#ifdef DRV_LIB_CPSS
    for(i = 0; i < MAX_GROUP_NUM; i++)
	{
		nam_syslog_dbg("prbs test get dev %d group %d Gpmembitmap: 0x%x Gpresbitmap 0x%x.\n",devNum,i,Gpmem[i],Gpres[i]);
	}
    for(gp_num = 0; gp_num < MAX_GROUP_NUM; gp_num++)
    {
        for(gp_mem_num = 0; gp_mem_num < MAX_GPMEM_NUM; gp_mem_num++)
        {
			if((Gpmem[gp_num] & (1 << gp_mem_num)) != 0)
			{
				portNum = gp_num * MAX_GPMEM_NUM + gp_mem_num;
				nam_syslog_dbg("prbs test dev %d group %d port %d.\n",opDevice,gp_num,portNum);
        	    for(j = 0;j < LANE_NUM_SUM;j++)
            	{
                    ret = cpssDxChDiagPrbsSerdesTransmitModeSet(devNum, portNum, j, CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E);
                    if (DIAG_RETURN_CODE_SUCCESS != ret) 
                    {
                        nam_syslog_dbg("port%d lane%d PrbsSerdesTransmitModeSet ret 0x%x\n",portNum, j,ret);
                        return DIAG_RETURN_CODE_ERROR;
                    }
            		ret = cpssDxChDiagPrbsSerdesTransmitModeGet(devNum, portNum, j, &transmit_mode);
            		if ((DIAG_RETURN_CODE_SUCCESS != ret) || (CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E != transmit_mode)) 
                    {
                        nam_syslog_dbg("port%d lane%d PrbsSerdesTransmitModeSet ret 0x%x,transmit_mode %d\n",portNum, j,ret,transmit_mode);
                        return DIAG_RETURN_CODE_ERROR;
                    }
                    ret = cpssDxChDiagPrbsSerdesTestEnableSet(devNum, portNum, j, 1);
                    if (DIAG_RETURN_CODE_SUCCESS != ret) 
                    {
                        nam_syslog_dbg("port%d lane%d PrbsSerdesTestEnableSet ret 0x%x\n",portNum, j,ret);
                        return DIAG_RETURN_CODE_ERROR;
                    }
            		ret = cpssDxChDiagPrbsSerdesTestEnableGet(devNum, portNum, j,&enable);
            		if ((DIAG_RETURN_CODE_SUCCESS != ret) || (enable != 1)) 
                    {
                        nam_syslog_dbg("port%d lane%d PrbsSerdesTestEnableSet ret 0x%x, enable %d\n",portNum, j,ret,enable);
                        return DIAG_RETURN_CODE_ERROR;
                    }
        	    }
			}
        }
    }
	sleep(10);
	for(gp_num = 0; gp_num < MAX_GROUP_NUM; gp_num++)
    {
        for(gp_mem_num = 0; gp_mem_num < MAX_GPMEM_NUM; gp_mem_num++)
        {
			if((Gpmem[gp_num] & (1 << gp_mem_num)) != 0)
			{
				portNum = gp_num * MAX_GPMEM_NUM + gp_mem_num;
				result = 0;
            	for(j = 0;j < LANE_NUM_SUM;j++)
            	{
                    ret = cpssDxChDiagPrbsSerdesStatusGet(devNum, portNum, j, &locked, &errorCnt, &patternCnt);
                    if (DIAG_RETURN_CODE_SUCCESS != ret) 
                    {
                        nam_syslog_dbg("port%d lane%d PrbsSerdesStatusGet ret 0x%x\n",portNum,j,ret);
                        return DIAG_RETURN_CODE_ERROR;
                    }
            		nam_syslog_dbg("Port%d lane%d :\n",portNum,j);
            		nam_syslog_dbg("locked %u ,errorCnt %u, patternCnt %u\n",locked,errorCnt,patternCnt);
            		if(locked == 0)
            		{
            			result = 1;
            			nam_syslog_dbg("Port%d lane%d status Error!\n",portNum,j);
            		}
            		else
            		{
            			if(errorCnt > 50)
                		{
                			result = 1;
            				nam_syslog_dbg("Port%d lane%d status Error!\n",portNum,j);
                		}
            			else
            			{
            				nam_syslog_dbg("Port%d lane%d status OK!\n",portNum,j);
            			}
            		}
            		ret = cpssDxChDiagPrbsSerdesTestEnableSet(devNum, portNum, j, 0);
                    if (DIAG_RETURN_CODE_SUCCESS != ret) 
                    {
                        nam_syslog_dbg("port%d lane%d PrbsSerdesTransmitModeSet ret 0x%x\n",portNum, j,ret);
                        return DIAG_RETURN_CODE_ERROR;
                    }
            	}
				if(result == 1)
				{
    			    Gpres[gp_num] |= (1 << gp_mem_num);
				}
			}
        }
	}
#endif
	return DIAG_RETURN_CODE_SUCCESS;
}
int nam_test_prbs(unsigned int opDevice, unsigned int opPort, unsigned int *result)
{
	int j = 0;
	int ret = DIAG_RETURN_CODE_SUCCESS;
	unsigned long locked = 0;
	unsigned long errorCnt = 0;
	unsigned long patternCnt = 0;
	unsigned long enable = 0;
    unsigned int devNum = opDevice;
	unsigned int portNum = opPort;
	unsigned int transmit_mode = 0;
	#define LANE_NUM_SUM 4
	
#ifdef DRV_LIB_CPSS
	for(j = 0;j < LANE_NUM_SUM;j++)
	{
        /*phyTestReg0,phyTestDataReg5*/
        ret = cpssDxChDiagPrbsSerdesTransmitModeSet(devNum, portNum, j, CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E);
        if (DIAG_RETURN_CODE_SUCCESS != ret) 
        {
            nam_syslog_dbg("port%d lane%d PrbsSerdesTransmitModeSet ret 0x%x\n",portNum, j,ret);
            return DIAG_RETURN_CODE_ERROR;
        }
		ret = cpssDxChDiagPrbsSerdesTransmitModeGet(devNum, portNum, j, &transmit_mode);
		if ((DIAG_RETURN_CODE_SUCCESS != ret) || (CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E != transmit_mode)) 
        {
            nam_syslog_dbg("port%d lane%d PrbsSerdesTransmitModeSet ret 0x%x,transmit_mode %d\n",portNum, j,ret,transmit_mode);
            return DIAG_RETURN_CODE_ERROR;
        }
        /*phyTestReg0*/
        ret = cpssDxChDiagPrbsSerdesTestEnableSet(devNum, portNum, j, 1);
        if (DIAG_RETURN_CODE_SUCCESS != ret) 
        {
            nam_syslog_dbg("port%d lane%d PrbsSerdesTestEnableSet ret 0x%x\n",portNum, j,ret);
            return DIAG_RETURN_CODE_ERROR;
        }
		ret = cpssDxChDiagPrbsSerdesTestEnableGet(devNum, portNum, j,&enable);
		if ((DIAG_RETURN_CODE_SUCCESS != ret) || (enable != 1)) 
        {
            nam_syslog_dbg("port%d lane%d PrbsSerdesTestEnableSet ret 0x%x, enable %d\n",portNum, j,ret,enable);
            return DIAG_RETURN_CODE_ERROR;
        }
	}
	
	sleep(10);
	*result = 0;
	for(j = 0;j < LANE_NUM_SUM;j++)
	{
		/*phyTestReg1,phyTestPrbsErrCntReg,phyTestPrbsCntReg*/
        ret = cpssDxChDiagPrbsSerdesStatusGet(devNum, portNum, j, &locked, &errorCnt, &patternCnt);
        if (DIAG_RETURN_CODE_SUCCESS != ret) 
        {
            nam_syslog_dbg("port%d lane%d PrbsSerdesStatusGet ret 0x%x\n",portNum,j,ret);
            return DIAG_RETURN_CODE_ERROR;
        }
		nam_syslog_dbg("Port%d lane%d :\n",portNum,j);
		nam_syslog_dbg("locked %u ,errorCnt %u, patternCnt %u\n",locked,errorCnt,patternCnt);
		if(locked == 0)
		{
			*result = 1;
			nam_syslog_dbg("Port%d status Error!\n",portNum);
		}
		else
		{
			if(errorCnt > 50)
    		{
    			*result = 1;
				nam_syslog_dbg("Port%d status Error!\n",portNum);
    		}
			else
			{
				nam_syslog_dbg("Port%d status Error!\n",portNum);
			}
		}
		ret = cpssDxChDiagPrbsSerdesTestEnableSet(devNum, portNum, j, 0);
        if (DIAG_RETURN_CODE_SUCCESS != ret) 
        {
            nam_syslog_dbg("port%d lane%d PrbsSerdesTransmitModeSet ret 0x%x\n",portNum, j,ret);
            return DIAG_RETURN_CODE_ERROR;
        }
	}
#endif

	return DIAG_RETURN_CODE_SUCCESS;
}

unsigned int nam_set_port_mode(unsigned devNum, unsigned portNum, unsigned int portMode)
{
	int ret = DIAG_RETURN_CODE_SUCCESS;
	int interfacemode = 0;
	int lanesbmp = 0;
	int portspeed = 0;
	
	#ifdef DRV_LIB_CPSS_3_4      /* jump BCM & CPSS*/
	if(portMode == 0)/*xaui*/
	{
		interfacemode = 4;
		lanesbmp = 0xf;
		portspeed =3;
	}
	else if(portMode == 1)/*rxaui*/
	{
		interfacemode = 11;
		lanesbmp = 0x3;
		portspeed =3;
	}
	else if(portMode == 2)/*dxaui*/
	{
		interfacemode = 4;
		lanesbmp = 0xf;
		portspeed =8;
	}
	else
	{
		return DIAG_RETURN_CODE_ERROR;
	}
    ret = cpssDxChPortInterfaceModeSet(devNum,portNum,interfacemode);
    if (DIAG_RETURN_CODE_SUCCESS != ret) 
    {
        return DIAG_RETURN_CODE_ERROR;
    }
    ret = cpssDxChPortSpeedSet(devNum,portNum,portspeed);
    if (DIAG_RETURN_CODE_SUCCESS != ret) 
    {
        return DIAG_RETURN_CODE_ERROR;
    }
    ret = cpssDxChPortSerdesPowerStatusSet(devNum,portNum,2,lanesbmp,1);
    if (DIAG_RETURN_CODE_SUCCESS != ret) 
    {
        return DIAG_RETURN_CODE_ERROR;
    } 
	#endif
	return ret;
}

int nam_dump_tab_br(int unit, int reg_num, int blk, int index, char *dataret)
{
	int ret = 0;
#ifdef DRV_LIB_BCM
/*int i = 0;*/
	if (65535 == blk) {
		blk = -1;
	}
	ret = soc_mem_read(unit, reg_num, blk, index, dataret);
	nam_syslog_dbg("dataret is %s \n", dataret);
/*	for (i = 0; i < 80; i++) {
		nam_syslog_dbg("dataret[%d] is %02x,  %d", i, (dataret[i] & 0xFF), dataret[i]);
	}*/
	if (ret < 0) {
		nam_syslog_dbg("ERROR: soc_reg32_read failed addr 0x%#x\n", reg_num);
	}
#endif

	return ret;
}

int nam_acl_show_ma(unsigned long ruleIndex, unsigned int rulesize, char *strbuff)
{
#ifdef DRV_LIB_CPSS
	unsigned char						devIdx = 0;
	int				ruleSize;
	unsigned long						GetIndex;
	unsigned long						bitPos=189;
	unsigned int						maskPtr[12] = {0};
	unsigned int						patternPtr[12] = {0};
	unsigned int						actionptr[3] = {0}; 
	char *showStr = NULL,*cursor = NULL;
	int totalLen = 0;
	
	showStr = (char*)malloc(102400);
	if(NULL == showStr) {
		nam_syslog_dbg("Malloc failed!\n");
		return NULL;
	}
	memset(showStr,0,102400);	
	cursor = showStr;

	if (rulesize) { 
		ruleSize=CPSS_PCL_RULE_SIZE_EXT_E;
	}
	else {
		ruleSize=CPSS_PCL_RULE_SIZE_STD_E;
	}
		
	GetIndex=cpssDxChPclRuleGet(devIdx, ruleSize, ruleIndex, maskPtr, patternPtr, actionptr);
	if(GetIndex!=0) {
		nam_syslog_dbg("cpssDxChPclRuleGet ERROR!\n");
	}

	totalLen += sprintf(cursor, "devIdx : %d ,rulesize %d , ruleIndex %ld :\n",devIdx,ruleSize,ruleIndex);
	cursor = showStr + totalLen;

	totalLen += sprintf(cursor, "pattern info");
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s %-8s %-8s\n","ITEM","RULE","MASK");
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "==================== ========== ==========\n");
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","Valid",patternPtr[0]&0x1,maskPtr[0]&0x1);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","PCL-ID(Group num)",(patternPtr[0]>>1)&0x3FF,(maskPtr[0]>>1)&0x3FF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","SrcPort",(patternPtr[0]>>11)&0x3F,(maskPtr[0]>>11)&0x3F);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","IsTagged",(patternPtr[0]>>17)&0x01,(maskPtr[0]>>17)&0x01);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","VID",(patternPtr[0]>>18)&0x0FFF,(maskPtr[0]>>18)&0x0FFF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","UP",((patternPtr[0]>>30)&0x03)|((patternPtr[1]<<2)&0x04), \
		((maskPtr[0]>>30)&0x03)|((maskPtr[1]<<2)&0x04));
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","QoSProfile",(patternPtr[1]>>1)& 0x7F,(maskPtr[1]>>1)& 0x7F);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","IsIP",(patternPtr[1]>>9)&0x01,(maskPtr[1]>>9)&0x01);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","is UDB Valid",(patternPtr[188/32]>>(188 % 32))&0x01, \
		(maskPtr[188/32]>>(188 % 32))&0x01);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","isL2Valid",(patternPtr[bitPos/32]>>(bitPos % 32))&0x01, \
		(maskPtr[bitPos/32]>>(bitPos % 32))&0x01);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","IsIPV6",(patternPtr[1]>>8)&0x01,(maskPtr[1]>>8)&0x01);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","IpProtocol",(patternPtr[1]>>10)&0x00FF,(maskPtr[1]>>10)&0x00FF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","DSCP",(patternPtr[1]>>18)&0x3F,(maskPtr[1]>>18)&0x3F);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","L4Valid",(patternPtr[1]>>24)&0x01,(maskPtr[1]>>24)&0x01);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","L4HeaderByte3/5",((patternPtr[1]>>25)&0x7F)|((patternPtr[2]<<7)&0x0080),	\
		((maskPtr[1]>>25)&0x7F)|((maskPtr[2]<<7)&0x0080));
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","L4HeaderByte2/4",(patternPtr[2]>>1)&0x00FF,(maskPtr[2]>>1)&0x00FF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","L4HeaderByte13",(patternPtr[2]>>9)&0xFF,(maskPtr[2]>>9)&0xFF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","L4HeaderByte1",(patternPtr[2]>>17)&0xFF,(maskPtr[2]>>17)&0xFF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","L4HeaderByte0",((patternPtr[2]>>25)&0x7F)|((patternPtr[3]<<7)&0x0080),	\
		((maskPtr[2]>>25)&0x7F)|((maskPtr[3]<<7)&0x0080));
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","IPHeaderOK",(patternPtr[11]>>31)&0x01,(maskPtr[11]>>31)&0x01);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","etherType/DsapSsap",(patternPtr[5]>>2)&0xFFFF, \
		(maskPtr[5]>>2)&0xFFFF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","l2Encap",(patternPtr[5]>>1)&0x1, \
		(maskPtr[5]>>1)&0x1);	
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","IPv4Fragmented",(patternPtr[5]>>18)&0x01,(maskPtr[5]>>18)&0x01);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","SIP",((patternPtr[3]>>1)&0x7FFFFFFF)|((patternPtr[4]&0x1)<<31), \
		((maskPtr[3]>>1)&0x7FFFFFFF)|((maskPtr[4]&0x1)<<31));
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%#-8x %#-8x\n","DIP",((patternPtr[4]>>1)&0x7FFFFFFF)|((patternPtr[5]&0x1)<<31), \
		((maskPtr[4]>>1)&0x7FFFFFFF)|((maskPtr[5]&0x1)<<31));
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s: %x:%x:%x:%x:%x:%x    %x:%x:%x:%x:%x:%x\n","MAC DA",
			((patternPtr[10]>>3)&0xFF),(((patternPtr[10]<<5)&0xE0)|((patternPtr[9]>>27)&0x1F)),
			((patternPtr[9]>>19)&0xFF), 
			((patternPtr[9]>>11)&0xFF),((patternPtr[9]>>3)&0xFF),
			 (((patternPtr[9]<<5)&0xE0)|((patternPtr[8]>>27)&0x1F)),
		    ((maskPtr[10]>>3)&0xFF),(((maskPtr[10]<<5)&0xE0)|((maskPtr[9]>>27)&0x1F)),
			((maskPtr[9]>>19)&0xFF), 
			((maskPtr[9]>>11)&0xFF),((maskPtr[9]>>3)&0xFF),
			 (((maskPtr[9]<<5)&0xE0)|((maskPtr[8]>>27)&0x1F)));
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%x:%x:%x:%x:%x:%x  %x:%x:%x:%x:%x:%x\n","MAC SA", 
	        ((patternPtr[8]>>19)&0xFF),((patternPtr[8]>>11)&0xFF), 
			((patternPtr[8]>>3)&0xFF), 
		   (((patternPtr[8]<<5)&0xE0)|((patternPtr[7]>>27)&0x1F)),
			((patternPtr[7]>>19)&0xFF),((patternPtr[7]>>11)&0xFF),

		    ((maskPtr[8]>>19)&0xFF),((maskPtr[8]>>11)&0xFF), 
			((maskPtr[8]>>3)&0xFF), 
		   (((maskPtr[8]<<5)&0xE0)|((maskPtr[7]>>27)&0x1F)),
			((maskPtr[7]>>19)&0xFF),((maskPtr[7]>>11)&0xFF));
	cursor = showStr + totalLen;
	/*
	nam_syslog_dbg("%-20s:%#-8x %#-8x\n","UDB0",(patternPtr[11]>>3)&0xFF,(maskPtr[11]>>3)&0xFF);		
	nam_syslog_dbg("%-20s:%#-8x %#-8x\n","UDB1",(patternPtr[11]>>11)&0xFF,(maskPtr[11]>>11)&0xFF);
	nam_syslog_dbg("%-20s:%#-8x %#-8x\n","UDB2",(patternPtr[11]>>19)&0xFF,(maskPtr[11]>>19)&0xFF);
	nam_syslog_dbg("%-20s:%#-8x %#-8x\n","UDB3",(patternPtr[10]>>11)&0xFF,(maskPtr[10]>>11)&0xFF);
	nam_syslog_dbg("%-20s:%#-8x %#-8x\n","UDB4",(patternPtr[10]>>19)&0xFF,(maskPtr[10]>>19)&0xFF);
	nam_syslog_dbg("%-20s:%#-8x %#-8x\n","UDB5",((patternPtr[10]>>27)&0x1F)|((patternPtr[11]<<5)&0xE0),((maskPtr[10]>>27)&0x1F)|((maskPtr[11]<<5)&0xE0));
		
*/
	totalLen += sprintf(cursor, "==========================================\n");
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "Action Entry %ld word 0\n",ruleIndex);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","FwdCmd",actionptr[0]&0x7);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PCECPU_CODE",(actionptr[0]>>3)&0xFF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","MirorToIngressAnalyzerPort",(actionptr[0]>>11)&0x1);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PCEQoSPrecedence",(actionptr[0]>>12)&0x01);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PCEQoSProfileMarkingEn",(actionptr[0]>>13)&0x01);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PCEQoSProfile",(actionptr[0]>>14)&0x7F);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PCEModifyDSCP",(actionptr[0]>>21)&0x03);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PCEModifyUP",(actionptr[0]>>23)&0x03);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","MatchCounterEn",(actionptr[0]>>25)&0x1);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","Match CounterIndex",(actionptr[0]>>26)&0x1F);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","Reserved",(actionptr[0]>>31)&0x1);
	cursor = showStr + totalLen;

	totalLen += sprintf(cursor, "Action Entry %ld word 1\n",ruleIndex);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PCERedirectCmd",actionptr[1]&0x3);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PCEEgressInterface/PCERounterLTTIndex",(actionptr[1]>>2)&0x1FFF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PCEVIDPrecedece",(actionptr[1]>>15)&0x1);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","EnNestedVLAN",(actionptr[1]>>16)&0x3);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PCEVLANCmd",(actionptr[1]>>17)&0x3);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PCE_VID",(actionptr[1]>>19)&0x0FFF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","Reserved",(actionptr[1]>>31)&0x1);
	cursor = showStr + totalLen;

	totalLen += sprintf(cursor, "Action Entry %ld word 2\n",ruleIndex);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","Reserved",(actionptr[2]>>22)&0x03FF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","Reserved",(actionptr[2]>>21)&0x1);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","TunnelPtr",(actionptr[2]>>11)&0x03FF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","TunnelStart",(actionptr[2]>>10)&0x1);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","VNTL2Echo",(actionptr[2]>>9)&0x1);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PolicerIndex",(actionptr[2]>>1)&0xFF);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "%-20s:%-x\n","PolicerEn",actionptr[2]&0x1);
	cursor = showStr + totalLen;
	totalLen += sprintf(cursor, "==========================================\n");
	cursor = showStr + totalLen;
	
	strncpy(strbuff, showStr, strlen(showStr));
	free(showStr);
#endif	
	return DIAG_RETURN_CODE_SUCCESS;
}

int nam_policer_global_show_ma(char devNum, char *strbuff)
{
#ifdef DRV_LIB_CPSS

	unsigned int      hwData, data;     /* data to write to register */
	unsigned int      regAddr;    /* register address */
	char *showStr = NULL,*cursor = NULL;
	int totalLen = 0;
	
	showStr = (char*)malloc(102400);
	if(NULL == showStr) {
		nam_syslog_dbg("Malloc failed!\n");
		return NULL;
	}
	memset(showStr,0,102400);	
	cursor = showStr;
	/*
	regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.txQCmdReg;
	regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->trafficClass.policerGlobalConfigReg;
	*/
	regAddr = 0x0C000000;
	hwData = readreg(devNum, regAddr);
	
	totalLen += sprintf(cursor, "%-20s:%-x\n", "Policers Global Configuration Register", hwData);
	cursor = showStr + totalLen;
	
	totalLen += sprintf(cursor, "%s\n", "DSAtagCompEn");
	cursor = showStr + totalLen;
	if ((hwData & 1<<9)>>9) {		
		totalLen += sprintf(cursor, "%s\n", "The byte count used for policing packets received on the cascading port does not include the added DSA tag as follows");
		cursor = showStr + totalLen;
	}
	else {
		totalLen += sprintf(cursor, "%s\n", "The byte count used for policing packets received on the cascading port includes the added DSA tag");
		cursor = showStr + totalLen;		
	}

	data = ((hwData & 1<<8)>>8) + ((hwData & 1<<7)>>7);
	totalLen += sprintf(cursor, "%s\n", "TunnelTermPolicingMode");
	cursor = showStr + totalLen;
	if (3 == data) {
		totalLen += sprintf(cursor, "%s\n", "Layer1 counting (including preamble+IPG+CRC)");
		cursor = showStr + totalLen;
	}
	else if (2 == data) {
		totalLen += sprintf(cursor, "%s\n", "Layer2 counting (including L2+L3+ header+CRC)");
		cursor = showStr + totalLen;		
	}
	else if (1 == data) {
		totalLen += sprintf(cursor, "%s\n", "Passenger packet counting");
		cursor = showStr + totalLen;		
	} 
	else if (0 == data) {
		totalLen += sprintf(cursor, "%s\n", "Reserved");
		cursor = showStr + totalLen;		
	} 
	
	totalLen += sprintf(cursor, "%s\n", "Policer Mode");
	cursor = showStr + totalLen;
	if ((hwData & 1<<6)>>6) {		
		totalLen += sprintf(cursor, "%s\n", "Loose Rate Limiter");
		cursor = showStr + totalLen;
	}
	else {
		totalLen += sprintf(cursor, "%s\n", "Strict Rate Limiter");
		cursor = showStr + totalLen;		
	}
	
	data = ((hwData & 1<<5)>>5) + ((hwData & 1<<4)>>4);
	totalLen += sprintf(cursor, "%s\n", "PolicerMRU");
	cursor = showStr + totalLen;
	if (3 == data) {
		totalLen += sprintf(cursor, "%s\n", "Reserved");
		cursor = showStr + totalLen;
	}
	else if (2 == data) {
		totalLen += sprintf(cursor, "%s\n", "10KB");
		cursor = showStr + totalLen;		
	}
	else if (1 == data) {
		totalLen += sprintf(cursor, "%s\n", "2KB");
		cursor = showStr + totalLen;		
	} 
	else if (0 == data) {
		totalLen += sprintf(cursor, "%s\n", "1.5KB");
		cursor = showStr + totalLen;		
	} 
	
	totalLen += sprintf(cursor, "%s\n", "Policer Drop Mode");
	cursor = showStr + totalLen;
	if ((hwData & 1<<2)>>2) {		
		totalLen += sprintf(cursor, "%s\n", "Hard Drop");
		cursor = showStr + totalLen;
	}
	else {
		totalLen += sprintf(cursor, "%s\n", "Soft Drop ");
		cursor = showStr + totalLen;		
	}

	data = ((hwData & 1<<1)>>1) + ((hwData & 1<<0)>>0);
	totalLen += sprintf(cursor, "%s\n", "Policing mode");
	cursor = showStr + totalLen;
	if (3 == data) {
		totalLen += sprintf(cursor, "%s\n", "Reserved");
		cursor = showStr + totalLen;
	}
	else if (2 == data) {
		totalLen += sprintf(cursor, "%s\n", "Layer3 counting (includes L3+packet without CRC)");
		cursor = showStr + totalLen;		
	}
	else if (1 == data) {
		totalLen += sprintf(cursor, "%s\n", "Layer2 counting (including L2+L3+ header+CRC)");
		cursor = showStr + totalLen;		
	} 
	else if (0 == data) {
		totalLen += sprintf(cursor, "%s\n", "Layer1 counting (including preamble+IPG+CRC)");
		cursor = showStr + totalLen;		
	}
	
	strncpy(strbuff, showStr, strlen(showStr));
	free(showStr);
#endif	
	return DIAG_RETURN_CODE_SUCCESS;
}

int nam_unimac_data_check_br(unimac_data_t* unimac_data, unsigned int * dataret)
{
#ifdef DRV_LIB_BCM	
	switch (unimac_data->type) {
		case COOMAND_CONFIG:
			if (unimac_data->rw) {
				READ_COMMAND_CONFIGr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_COMMAND_CONFIGr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case GPORT_CONFIG:
			if (unimac_data->rw) {
				READ_GPORT_CONFIGr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_GPORT_CONFIGr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case MAC_0:
			if (unimac_data->rw) {
				READ_MAC_0r(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_MAC_0r(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case MAC_1:
			if (unimac_data->rw) {
				READ_MAC_1r(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_MAC_1r(unimac_data->unit, unimac_data->port, unimac_data->data);
			}		
		break;
		case FRM_LENGTH:
			if (unimac_data->rw) {
				READ_FRM_LENGTHr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_FRM_LENGTHr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case PAUSE_QUNAT:
			if (unimac_data->rw) {
				READ_PAUSE_QUANTr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_PAUSE_QUANTr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case SFD_OFFSET:
			if (unimac_data->rw) {
				READ_SFD_OFFSETr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_SFD_OFFSETr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case MAC_MODE:
			if (unimac_data->rw) {
				READ_MAC_MODEr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_MAC_MODEr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case TAG_0:
			if (unimac_data->rw) {
				READ_TAG_0r(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_TAG_0r(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case TAG_1:
			if (unimac_data->rw) {
				READ_TAG_1r(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_TAG_1r(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case TX_IPG_LENGTH:
			if (unimac_data->rw) {
				READ_TX_IPG_LENGTHr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_TX_IPG_LENGTHr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case PAUSE_CONTROL:
			if (unimac_data->rw) {
				READ_PAUSE_CONTROLr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_PAUSE_CONTROLr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case IPG_HD_BKP_CNTL:
			if (unimac_data->rw) {
				READ_IPG_HD_BKP_CNTLr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_IPG_HD_BKP_CNTLr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case FLUSH_CONTROL:
			if (unimac_data->rw) {
				READ_FLUSH_CONTROLr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_FLUSH_CONTROLr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case RXFIFO_STAT:
			if (unimac_data->rw) {
				READ_RXFIFO_STATr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_RXFIFO_STATr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case TXFIFO_STAT:
			if (unimac_data->rw) {
				READ_TXFIFO_STATr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_TXFIFO_STATr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case GPORT_RSV_MASK:
			if (unimac_data->rw) {
				READ_GPORT_RSV_MASKr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_GPORT_RSV_MASKr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case GPORT_STAT_UPDATE_MASK:
			if (unimac_data->rw) {
				READ_GPORT_STAT_UPDATE_MASKr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_GPORT_STAT_UPDATE_MASKr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case GPORT_TPID:
			if (unimac_data->rw) {
				READ_GPORT_TPIDr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_GPORT_TPIDr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case GPORT_SOP_S1:
			if (unimac_data->rw) {
				READ_GPORT_SOP_S1r(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_GPORT_SOP_S1r(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case GPORT_SOP_S0:
			if (unimac_data->rw) {
				READ_GPORT_SOP_S0r(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_GPORT_SOP_S0r(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case GPORT_SOP_S3:
			if (unimac_data->rw) {
				READ_GPORT_SOP_S3r(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_GPORT_SOP_S3r(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case GPORT_SOP_S4:
			if (unimac_data->rw) {
				READ_GPORT_SOP_S4r(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_GPORT_SOP_S4r(unimac_data->unit, unimac_data->port, unimac_data->data);
			}
		break;
		case GPORT_MAC_CRS_SEL:
			if (unimac_data->rw) {
				READ_GPORT_MAC_CRS_SELr(unimac_data->unit, unimac_data->port, dataret);
			}
			else {
				WRITE_GPORT_MAC_CRS_SELr(unimac_data->unit, unimac_data->port, unimac_data->data);
			}			
		break;
		
		default:
			return DIAG_RETURN_CODE_ERROR;
		break;
	} 
#endif

	return DIAG_RETURN_CODE_SUCCESS;
}

/*
int unimac_check_one(void *data, unimac_read_write *func, int rw)
{
	if (rw) {
		(*func)(data->unit, data->port, data->ptr);
	}
	else {
		(*func)(data->unit, data->port, &(data->ptr));
	}
}

int test()
{
	unsigned char adr[6] = {0};
	unsigned int data = 0;
	bcm_port_pause_addr_get(0, 8,adr);
	READ_FLUSH_CONTROLr(0, 8, &data);
}
*/
/*
mac_uni_speed_get(int unit, soc_port_t port, int *speed)
{
    uint32 		command_config, uni_speed;
    int                 rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(READ_COMMAND_CONFIGr(unit, port, &command_config));

    uni_speed = soc_reg_field_get(unit, COMMAND_CONFIGr, command_config, 
                                  ETH_SPEEDf);

    switch(uni_speed) {
    case 0:			
	*speed = 10;
	break;
    case 1:			
	*speed = 100;
	break;
    case 2:			
	*speed = 1000;
	break;
    case 3:			
	*speed = 2500;
	break;
    default:
        rv = SOC_E_INTERNAL;
    }

    DBG_GE_VERB(("mac_uni_speed_get: unit %d port %s speed=%dMb\n",
               unit, SOC_PORT_NAME(unit, port),
                *speed));
    return(rv);
}
*/
#ifdef __cplusplus
}
#endif
