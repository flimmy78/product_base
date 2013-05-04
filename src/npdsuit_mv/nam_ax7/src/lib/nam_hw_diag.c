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
* nam_npd_test.c
*
*
* CREATOR:
*		zhangdi@autelan.com
*
* DESCRIPTION:
*		APIs used by npd test.
*
* DATE:
*		06/01/2011	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.1 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "npd/nam/npd_amapi.h"
#include "nam_vlan.h"
#include "nam_eth.h"
#include "nam_log.h"


/* for port test */
/* create vlan by vid */
int create_vlan(int devnum,int vid)
{
    #ifndef DRV_LIB_BCM      /* jump BCM */

    /* For auto-learning to be enabled, it must be enabled both on the ingress port and the packet VLAN. */
	unsigned int autolearn = 1;     /* Auto-Learn Disable in vlan entry */
	int asictype = 0;

	/*vlan entry ValidBit set 1*/
	if(0 != cpssDxChBrgVlanEntryValidate(devnum,vid))
	{
		nam_syslog_err("ValidBit set ERROR\n");
		return NPD_FAIL;
	}
	/* autolearnDisable set 0 */
	if(0 != cpssDxChBrgVlanLearningStateSet(devnum,vid,autolearn))
	{
		nam_syslog_err("autolearnDisable set ERROR\n");
		return NPD_FAIL;
	}
	/* NewSrcAddrIsNotSecurityBreach: yes */
	if(0 != cpssDxChBrgVlanNASecurEnable(devnum,vid,0))
	{
		nam_syslog_err("NewSrcAddrIsNotSecurityBreach set ERROR\n");			
		return NPD_FAIL;
	}
	/* NA message to CPU disable*/
	asictype = nam_asic_get_asic_type(devnum);
	if((asictype < ASIC_TYPE_START_EXMX_E) || (asictype > ASIC_TYPE_LAST_E))
	{
		nam_syslog_err("get asic type fail !\n");
	}
	if(asictype == ASIC_TYPE_DXCH_LION_E)
	{
		if(0 != cpssDxChBrgVlanNaToCpuEnable(devnum,vid,1))
		{
			nam_syslog_err("NA message set ERROR\n");			
			return NPD_FAIL;
		}
	}
	else
	{
		if(0 != cpssDxChBrgVlanNaToCpuEnable(devnum,vid,0))
		{
			nam_syslog_err("NA message set ERROR\n");			
			return NPD_FAIL;
		}
	}
	nam_syslog_dbg("Create vlan %d on device %d OK\n",vid,devnum);	
    #endif
	return NPD_SUCCESS;	
}

int phy_page_set(int board_type)
{
	int	status = -1; /* return value */
	int port_num;
	int dev_num =0;
	
	#ifdef DRV_LIB_CPSS_3_4      /* jump BCM & CPSS*/	
	if((BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)||(BOARD_TYPE == BOARD_TYPE_AX81_1X12G12S))
	{   
		dev_num = 0;		
		for(port_num=0;port_num<=23;port_num++)
		{
			if(port_num<=11)
			{   /* copper mode */
            	status = cpssDxChPhyPortSmiRegisterWrite(dev_num,port_num, 22,0x0000);
            	if(status != 0)
            	{
            		nam_syslog_err("PHY1340 write :dev %d port %d page reg to 0 error\r\n", \
            					dev_num,port_num);
            		return status;
            	}
			}
			else
			{   /* fiber mode */
            	status = cpssDxChPhyPortSmiRegisterWrite(dev_num,port_num, 22,0x0001);
            	if(status != 0)
            	{
            		nam_syslog_err("PHY1340 write :dev %d port %d page reg to 1 error\r\n", \
            					dev_num,port_num);
            		return status;
            	}				
			}			
		}
		nam_syslog_dbg("AX71_2X12G12S / AX81_1X12G12S  set phy page OK!\n");
    }
	else
	{
  		nam_syslog_dbg("Do not need set page reg on this board !\n");		
	}
	#else
	status =0;
	#endif
	return status;
}
int force_port_to_cscd_mode_for_lion(int devnum,int portnum)
{
	int rc = -1;

	#ifdef DRV_LIB_CPSS_3_4
	rc = cpssDxChCscdPortTypeSet(devnum,portnum,1);/*CPSS_CSCD_PORT_DSA_MODE_EXTEND_E*/
	if (rc != 0)
    {
		nam_syslog_err("cpssDxChCscdPortTypeSet  dev %d,port %d to cscd fail !rc = %d\n",devnum,portnum,rc);
        return rc;
    }	
	/*lion  support speed at 12G */
	rc = gtAppDemoLionPortModeSpeedSet(devnum,portnum,4,3);  /*3: CPSS_PORT_SPEED_10000_E */
    if (rc != 0)
    {
		nam_syslog_err("gtAppDemoLionPortModeSpeedSet fail ! rc = %d\n",rc);
        return rc;
    }
	rc = cpssDxChPortSerdesPowerStatusSet(devnum,portnum,2,0xf,1);
    if (rc != 0)
    {
		nam_syslog_err("cpssDxChPortSerdesPowerStatusSet fail ! rc = %d\n",rc);
        return rc;
    }
	#endif
	return rc;
}

int force_network_port_to_cscd_mode(int devnum,int portnum,char isCscd)
{
	int rc = -1;
	CPSS_QOS_ENTRY_STC portQosCfg;
#ifdef DRV_LIB_CPSS_3_4
	if(isCscd)
	{
		rc = cpssDxChCscdPortTypeSet(devnum,portnum,1);/*CPSS_CSCD_PORT_DSA_MODE_EXTEND_E*/
	}
	else
	{
		rc = cpssDxChCscdPortTypeSet(devnum,portnum,2);/*CPSS_CSCD_PORT_NETWORK_E*/
	}
	if (rc != 0)
	{
		nam_syslog_err("cpssDxChCscdPortTypeSet  dev %d,port %d to cscd fail !rc = %d\n",devnum,portnum,rc);
		return rc;
	}
#endif
	return rc;
}

int force_port_to_cscd_mode(int devnum,int portnum)
{
	int rc = -1;

	#ifdef DRV_LIB_CPSS_3_4
	rc = cpssDxChCscdPortTypeSet(devnum,portnum,1);/*CPSS_CSCD_PORT_DSA_MODE_EXTEND_E*/
	if (rc != 0)
    {
		nam_syslog_err("cpssDxChCscdPortTypeSet  dev %d,port %d to cscd fail !rc = %d\n",devnum,portnum,rc);
        return rc;
    }

	rc = cpssDxChPortInterfaceModeSet(devnum,portnum,4); /* 4:CPSS_PORT_INTERFACE_MODE_XGMII_E */
    if (rc != 0)
    {
		nam_syslog_err("cpssDxChPortInterfaceModeSet fail ! rc = %d\n",rc);
        return rc;
    }

	rc = cpssDxChPortSpeedSet(devnum,portnum,3);  /*3: CPSS_PORT_SPEED_12000_E */
    if (rc != 0)
    {
		nam_syslog_err("cpssDxChPortSpeedSet fail ! rc = %d\n",rc);
        return rc;
    }
	rc = cpssDxChPortSerdesPowerStatusSet(devnum,portnum,2,0xf,1);
    if (rc != 0)
    {
		nam_syslog_err("cpssDxChPortSerdesPowerStatusSet fail ! rc = %d\n",rc);
        return rc;
    }
	#endif
	return rc;
}

/* force the port to 10G mode  */
int force_port_to_xg_mode(int devNum,int portNum)
{
	int	rc = -1; /* return value */
	
	#ifdef DRV_LIB_CPSS_3_4      /* jump BCM & CPSS*/		
    rc = cpssDxChCscdPortTypeSet(devNum, portNum, 2);  /*2:CPSS_CSCD_PORT_NETWORK_E */
    if (rc != 0)
    {
        return rc;
    }		
    /* set port to HGS mode */
    rc = cpssDxChPortInterfaceModeSet(devNum,portNum,4); /* 4:CPSS_PORT_INTERFACE_MODE_XGMII_E */
    if (rc != 0)
    {
        return rc;
    }

    rc = cpssDxChPortSpeedSet(devNum,portNum,3);  /*3: CPSS_PORT_SPEED_10000_E */
    if (rc != 0)
    {
        return rc;
    }
    /* is is true need  */
    rc = cpssDxChPortSerdesPowerStatusSet(devNum,portNum,2,0xf,1);
    if (rc != 0)
    {
        return rc;
    }
	nam_syslog_dbg("Set port:(%d,%d) to xg mode OK!\n",devNum,portNum);
	return rc;
	
	#else
	return 0;
	#endif	
}

/* just for AX71-2X12G12S on AX8610,we set it to 10G mode, after READY */
int force_to_xg_mode()
{
	int	rc = -1; /* return value */

    rc = force_port_to_xg_mode(0,26);
    if (rc != 0)
    {
        return rc;
    }
    rc = force_port_to_xg_mode(0,27);
    if (rc != 0)
    {
        return rc;
    }		
	nam_syslog_dbg("XCAT set port 26 27 to xg mode OK!\n");
	return rc;
}

/*change 0 throught 11 ports mode from xaui to rxaui*/
int	nam_change_mode_xaui_to_rxaui(int devnum, int portnum)
{
	int ret = 0;
	
    if((ret = gtAppDemoLionPortModeSpeedSet(devnum, portnum, 11,3)) != 0)
    {
		nam_syslog_dbg("cpss set dev %d port %d rxaui mode fail, return %d\n", devnum, portnum, ret);
    	return ret;
    }
   
	return 0;
}
int	nam_change_mode_xaui_to_rdxaui(int devnum, int portnum)
{
	int ret = 0;
    if((ret = gtAppDemoLionPortModeSpeedSet(devnum, portnum, 4, 8)) != 0)
    {
		nam_syslog_dbg("cpss set dev %d port %d rdxaui mode fail, return %d\n", devnum, portnum, ret);
    	return ret;
    }
	return 0;
}
/*Configuration lionSerdesConfig of port*/
int nam_reconfigure_lionserdesconfig(int devnum, int portnum,int rxSerdesLaneArr[4],int txSerdesLaneArr[4])
{
	int ret = 0;
	
	ret = cpssDxChPortXgPscLanesSwapSet(devnum, (GT_U8)portnum, rxSerdesLaneArr, txSerdesLaneArr);
    if(ret != 0)
        return ret;
    return 0;
}

#ifdef __cplusplus
}
#endif


