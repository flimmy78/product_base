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
* nam_pvlan.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		API used in NAM for private VLAN module.
*
* DATE:
*		05/12/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.14 $	
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
#include "npd/nbm/npd_bmapi.h"
#include "nam_pvlan.h"
#include "nam_log.h"
static unsigned int 			vlanid=0;
static CPSS_PORTS_BMP_STC		port;
#if 0

static NAM_BOOL 				globle_pvlan=NAM_PVE_FALSE;
#endif
int nam_pvlan_create(unsigned int pvlanid)
{
    int              		ret = PVE_CONFIG_ERR;
	unsigned char    		dev = 0;
	
	vlanid=pvlanid;
	memset(&port,0,sizeof(CPSS_PORTS_BMP_STC));
#if 0
	ret=cpssDxChBrgSrcIdPortDefaultSrcIdSet(devNum1,portNum1,0);
	if( ret!=0){
			printf("cpssDxChBrgSrcIdPortDefaultSrcIdSet erro is :%d\n",ret);
			return ret;
			}
#endif
#ifdef DRV_LIB_CPSS

	ret=cpssDxChBrgSrcIdGlobalUcastEgressFilterSet(dev,NAM_PVE_TRUE);
	if( ret!=0){
		 syslog_ax_nam_pvlan_err("cpssDxChBrgSrcIdGlobalUcastEgressFilterSet erro is :%d\n",ret);
		return ret;
		}

	ret=cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet(dev,CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E);
	if( ret!=0){
		 syslog_ax_nam_pvlan_err("cpssDxChBrgSrcIdGlobalSrcIdAssignModeSet erro is :%d\n",ret);
		return ret;
		}
	port.ports[0]=0xfffffff;
	ret=cpssDxChBrgSrcIdGroupEntrySet(dev,vlanid,NAM_PVE_FALSE,&port);
	if( ret!=0){
			 syslog_ax_nam_pvlan_err("cpssDxChBrgSrcIdGroupEntrySet erro is :%d\n",ret);
			return ret;
			}
#endif

	return ret;
}

int nam_pvlan_add_port(unsigned int eth_g_index1)
{
    int              		ret=PVE_CONFIG_ERR;
	unsigned char 			devNum1,portNum1;
#ifdef DRV_LIB_CPSS	
	npd_get_devport_by_global_index(eth_g_index1,&devNum1, &portNum1);
	NAM_CONVERT_BACK_DEV_PORT_MAC(devNum1, portNum1);
	ret=cpssDxChBrgSrcIdGroupPortDelete(devNum1,vlanid,portNum1);
	if( ret!=0){
			 syslog_ax_nam_pvlan_err("cpssDxChBrgSrcIdGroupEntrySet erro is :%d\n",ret);
			return ret;
			}
#endif
	return ret;
}

int nam_pvlan_delete_port(unsigned int eth_g_index1)
{
    int              		ret=PVE_CONFIG_ERR;
	unsigned char 			devNum1,portNum1;
#ifdef DRV_LIB_CPSS	
	npd_get_devport_by_global_index(eth_g_index1,&devNum1, &portNum1);
	NAM_CONVERT_BACK_DEV_PORT_MAC(devNum1, portNum1);
	ret=cpssDxChBrgSrcIdGroupPortAdd(devNum1,vlanid,portNum1);
	if( ret!=0){
			 syslog_ax_nam_pvlan_err("cpssDxChBrgSrcIdGroupEntrySet erro is :%d\n",ret);
			return ret;
			}
#endif
	return ret;
}

int nam_pvlan_cpu_port(unsigned int flag)
{
    int              		ret=PVE_CONFIG_ERR;
	unsigned char 			devNum1,portNum1;
	
	devNum1=0;
	portNum1=63;
#ifdef DRV_LIB_CPSS

	if(flag==1)
	{
		ret=cpssDxChBrgSrcIdGroupPortDelete(devNum1,vlanid,portNum1);
		if( ret!=0){
				 syslog_ax_nam_pvlan_err("nam_pvlan_cpu_port erro is :%d\n",ret);
				return ret;
				}
	}
	else 
	{
		ret=cpssDxChBrgSrcIdGroupPortAdd(devNum1,vlanid,portNum1);
		if( ret!=0){
				 syslog_ax_nam_pvlan_err("nam_pvlan_cpu_port erro is :%d\n",ret);
				return ret;
			}
	}
#endif
	return ret;
}

int nam_pvlan_enable
(
	void
)
{	
	int ret = 0;
	unsigned char devNum = 0;
#ifdef DRV_LIB_CPSS

	ret=cpssDxChBrgPrvEdgeVlanEnable(devNum,NAM_PVE_TRUE);
	if(0 != ret) {
		 syslog_ax_nam_pvlan_err("cpssDxChBrgPrvEdgeVlanEnable1 erro is :%d\n",ret);
	}
#endif
	return ret;
}

int nam_pvlan_port_config(unsigned int eth_g_index1,unsigned int eth_g_index2)
{
	int             	ret=PVE_CONFIG_ERR;
	unsigned char 		devNum1,portNum1,devNum2,portNum2;
	
	npd_get_devport_by_global_index(eth_g_index1,&devNum1, &portNum1);
	
	npd_get_devport_by_global_index(eth_g_index2,&devNum2, &portNum2);
#ifdef DRV_LIB_CPSS

	NAM_CONVERT_BACK_DEV_PORT_MAC(devNum1, portNum1);

#if 0
	 syslog_ax_nam_pvlan_dbg("devnum = %d,  portnumber = %d\n",devNum1,portNum1);
	if(globle_pvlan == NAM_FALSE){
		ret=cpssDxChBrgPrvEdgeVlanEnable(devNum1,NAM_TRUE);
		if( ret!=0){
			
			return ret;
		}
		globle_pvlan = NAM_TRUE;
	}
#endif

	ret=cpssDxChBrgPrvEdgeVlanPortEnable(devNum1,portNum1,NAM_PVE_TRUE,portNum2,devNum2,NAM_PVE_FALSE);
	if( ret!=0){
		 syslog_ax_nam_pvlan_err("cpssDxChBrgPrvEdgeVlanEnable erro is :%d\n",ret);
		return ret;
	}
#endif	
	return ret;
}

int nam_pvlan_cscd_port_config_spi
(
	unsigned char devNum1,
	unsigned char portNum1,
	unsigned char dev,
	unsigned char port
)
{
	int	ret=PVE_CONFIG_ERR;
	
#ifdef DRV_LIB_CPSS	
	NAM_CONVERT_BACK_DEV_PORT_MAC(devNum1, portNum1);

#if 0
	if(globle_pvlan == NAM_FALSE){
		ret=cpssDxChBrgPrvEdgeVlanEnable(devNum1,NAM_TRUE);
		if( ret!=0){
			 syslog_ax_nam_pvlan_err("cpssDxChBrgPrvEdgeVlanEnable1 erro is :%d\n",ret);
			return ret;
			}
		globle_pvlan = NAM_TRUE;
	}
#endif

	ret=cpssDxChBrgPrvEdgeVlanPortEnable(devNum1,portNum1,NAM_PVE_TRUE,port,dev,NAM_PVE_FALSE);
	if( ret!=0){
		 syslog_ax_nam_pvlan_err("cpssDxChBrgPrvEdgeVlanEnable erro is :%d\n",ret);
		return ret;
	}
	cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet(devNum1,portNum1,NAM_PVE_TRUE);
#endif

	return ret;
}


int nam_pvlan_port_config_spi(unsigned int eth_g_index,unsigned char dev,unsigned char port)
{
	int             	ret=PVE_CONFIG_ERR;
	unsigned char 		devNum1,portNum1;
	
	npd_get_devport_by_global_index(eth_g_index,&devNum1, &portNum1);
#ifdef DRV_LIB_CPSS	
	NAM_CONVERT_BACK_DEV_PORT_MAC(devNum1, portNum1);

#if 0
	if(globle_pvlan == NAM_FALSE){
		ret=cpssDxChBrgPrvEdgeVlanEnable(devNum1,NAM_TRUE);
		if( ret!=0){
			 syslog_ax_nam_pvlan_err("cpssDxChBrgPrvEdgeVlanEnable1 erro is :%d\n",ret);
			return ret;
			}
		globle_pvlan = NAM_TRUE;
	}
#endif

	ret=cpssDxChBrgPrvEdgeVlanPortEnable(devNum1,portNum1,NAM_PVE_TRUE,port,dev,NAM_PVE_FALSE);
	if( ret!=0){
		 syslog_ax_nam_pvlan_err("cpssDxChBrgPrvEdgeVlanEnable erro is :%d\n",ret);
		return ret;
	}
	cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet(devNum1,portNum1,NAM_PVE_TRUE);
#endif

	return ret;
}



int nam_pvlan_port_delete(unsigned int eth_g_index1)
{
	int             	ret=PVE_CONFIG_ERR;
	unsigned char 		devNum1,portNum1,devNum2,portNum2;
	
	npd_get_devport_by_global_index(eth_g_index1,&devNum1, &portNum1);
	
	devNum2=devNum1;
	portNum2=portNum1;
#ifdef DRV_LIB_CPSS

	NAM_CONVERT_BACK_DEV_PORT_MAC(devNum1, portNum1);
	
	 syslog_ax_nam_pvlan_dbg("devnum = %d,  portnumber = %d\n",devNum1,portNum1);
	
	ret=cpssDxChBrgPrvEdgeVlanPortEnable(devNum1,portNum1,NAM_PVE_FALSE,portNum2,devNum2,NAM_PVE_FALSE);
	if( ret!=0){
		 syslog_ax_nam_pvlan_err("cpssDxChBrgPrvEdgeVlanEnable erro is :%d\n",ret);
		return ret;
	}


	ret=cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet (devNum1,portNum1,NAM_PVE_FALSE);
	if( ret!=0){
		 syslog_ax_nam_pvlan_err("cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet erro is :%d\n",ret);
		return ret;
	}
#endif
	return ret;
}


int nam_pvlan_trunk_config(unsigned int eth_g_index1,unsigned char trunkid)
{
	int             	ret=PVE_CONFIG_ERR;
	unsigned char 		devNum1,portNum1;
	unsigned char  		devtrunk=0;      
	
	npd_get_devport_by_global_index(eth_g_index1,&devNum1, &portNum1);
	
#ifdef DRV_LIB_CPSS

	NAM_CONVERT_BACK_DEV_PORT_MAC(devNum1, portNum1);

#if 0
	if(globle_pvlan==NAM_FALSE){
		ret=cpssDxChBrgPrvEdgeVlanEnable(devNum1,NAM_TRUE);
		if( ret!=0){
			 syslog_ax_nam_pvlan_err("cpssDxChBrgPrvEdgeVlanEnable1 erro is :%d\n",ret);
			return ret;
			}
		globle_pvlan=NAM_TRUE;
	}
#endif

	ret=cpssDxChBrgPrvEdgeVlanPortEnable(devNum1,portNum1,NAM_PVE_TRUE,devtrunk,trunkid,NAM_PVE_TRUE);
	if( ret!=0){
		 syslog_ax_nam_pvlan_err("cpssDxChBrgPrvEdgeVlanEnable erro is :%d\n",ret);
		return ret;
		}
#endif

	return ret;
}

int nam_pvlan_control_config(unsigned int eth_g_index1,int flag)
{
	int             	ret=PVE_CONFIG_ERR;
	unsigned char 		devNum1,portNum1;
	NAM_BOOL             enable;
	npd_get_devport_by_global_index(eth_g_index1,&devNum1, &portNum1);
	if(flag==1)
		{enable=NAM_PVE_TRUE;}
	else 
		{enable=NAM_PVE_FALSE;}
#ifdef DRV_LIB_CPSS

	ret=cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet (devNum1,portNum1,enable);
	if( ret!=0){
		 syslog_ax_nam_pvlan_err("cpssDxChBrgPrvEdgeVlanPortControlPktsToUplinkSet erro is :%d\n",ret);
		return ret;
		}
#endif
	return ret;
}


/**********************************************************************
 * nam_pvlan_set_pve_enable
 *
 *  DESCRIPTION:
 *          Set Asic PVE enable/disable.
 *          0x02040000  [30]
 *
 *  INPUT:
 *          devnum:  [0~31]
 *          enable:   [0~1]
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          NAM_OK
 *          NAM_ERR
 *
 *  AUTHOR:
 *          luoxun@autelan.com
 *
 **********************************************************************/
unsigned long nam_pvlan_set_pve_enable(unsigned char devnum, boolean enable)
{
    if (devnum>31)  /* code optimize: Unsigned compared against 0. zhangdi@autelan.com 3013-01-18 */
        return NAM_ERR;

    #ifdef DRV_LIB_CPSS
    return prvCpssDrvHwPpSetRegField(devnum, 
                                     0x02040000,
                                     30, 1, enable);
    #endif
}


/**********************************************************************
 * nam_pvlan_set_port_pve
 *
 *  DESCRIPTION:
 *          Set Asic port PVE to a dest.
 *
 *  INPUT:
 *          devnum:       [0~31]
 *          portnum:      [0~127]
 *          dest_device: [0~31]
 *          dest_port:    [0~127]
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          NAM_OK
 *          NAM_ERR
 *
 *  AUTHOR:
 *          luoxun@autelan.com
 *
 **********************************************************************/
unsigned long nam_pvlan_set_port_pve(unsigned char devnum,
                                           unsigned char portnum,
                                           unsigned char dest_device,
                                           unsigned char dest_port)
{
    unsigned long ret = NAM_OK;
    
    if ( (devnum>31)
        ||(portnum>127)
        ||(dest_device>31)
        ||(dest_port>127))
        return NAM_ERR;

    #ifdef DRV_LIB_CPSS
    /* Set port PVE enable. */
    ret = prvCpssDrvHwPpSetRegField(devnum, 
                                    (0x02000000 + portnum*0x1000),
                                     23, 1, 1);
    if (NAM_OK != ret)
        return ret;

    /* Set dest port. */
    ret = prvCpssDrvHwPpSetRegField(devnum, 
                                    (0x02000000 + portnum*0x1000),
                                     25, 7, dest_port);
    if (NAM_OK != ret)
        return ret;

    /* Set dest device. */
    ret = prvCpssDrvHwPpSetRegField(devnum, 
                                    (0x02000010 + portnum*0x1000),
                                     0, 5, dest_device);
    if (NAM_OK != ret)
        return ret;

    /* Enable forwarding of all packets, including control packets to PVlan Uplink. */
    ret = prvCpssDrvHwPpSetRegField(devnum, 
                                    (0x02000000 + portnum*0x1000),
                                     22, 1, 1);
    #endif
    
    if (NAM_OK != ret)
        return ret;

    return ret;
}
unsigned long nam_pvlan_disable_port_pve(unsigned char devnum,
                                           unsigned char portnum,
                                           unsigned char dest_device,
                                           unsigned char dest_port)
{
    unsigned long ret = NAM_OK;
    
    if ( (devnum>31)
        ||(portnum>127)
        ||(dest_device>31)
        ||(dest_port>127))
        return NAM_ERR;

    /* Set port PVE disable. */

    #ifdef DRV_LIB_CPSS
    ret = prvCpssDrvHwPpSetRegField(devnum, 
                                    (0x02000000 + portnum*0x1000),
                                     23, 1, 0);
    #endif
    if (NAM_OK != ret)
        return ret;

    return ret;
}



#ifdef __cplusplus
}
#endif

