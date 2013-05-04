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
* nam_mirror.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM by MIRROR module.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.22 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "dbus/npd/npd_dbus_def.h"
#include "nam_log.h"
#include "nam_mirror.h"
#include "sysdef/returncode.h"

#ifdef DRV_LIB_CPSS
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#endif

void nam_mirror_init
(
	void
)
{
#ifdef DRV_LIB_CPSS_3_4
	unsigned char	devIdx = 0, appDemoDevAmount = 0;
	int ret = 0, enable = 1, globalindex = 1;
	appDemoDevAmount = nam_asic_get_instance_num();
	nam_syslog_dbg("nam mirror init! \n");
	for(devIdx=0; devIdx < appDemoDevAmount; devIdx++) {
	/*
		ret = cpssDxChMirrRxStatMirrorToAnalyzerEnable(0, 1);
		if (ret) {
			nam_syslog_err(" enable mirror Analyzer rx fail ret is %d\n", ret);
		}
		ret = cpssDxChMirrTxStatMirrorToAnalyzerEnable(0, 1);
		if (ret) {
			nam_syslog_err(" enable mirror Analyzer tx fail ret is %d\n", ret);
		}
		*/

		ret = cpssDxChMirrorToAnalyzerForwardingModeSet(devIdx, CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_E);
		if (ret) {
			nam_syslog_err("mirror to analyer forwaiding mode set fail ret is %d\n", ret);
		}
		ret = cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet(devIdx, enable, globalindex);
		if (ret) {
			nam_syslog_err("mirror rx global analyer forwaiding mode set fail ret is %d\n", ret);
		}
		ret = cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet(devIdx, enable, 2);
		if (ret) {
			nam_syslog_err("mirror tx global analyer forwaiding mode set fail ret is %d\n", ret);
		}
	}
#endif	
}

unsigned int nam_mirror_analyzer_port_set
(
	unsigned int profile,
	unsigned char analyzerDev,
	unsigned char analyzerPort,
	unsigned int    direct
	
)
{
	 unsigned int       ret=MIRROR_RETURN_CODE_ERROR;
	 unsigned char 		dev = 0;
     unsigned char      analyzer_port = 0,analyzer_dev = 0;
	 unsigned int       devnum;
	 unsigned char      asic_num = 0;

#ifdef DRV_LIB_CPSS
	#ifdef DRV_LIB_CPSS_3_4
	CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   interfaceptr;
	memset(&interfaceptr, 0, sizeof(CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC));

	interfaceptr.interface.type = CPSS_INTERFACE_PORT_E;/*CPSS_INTERFACE_PORT_E*/

	ret = nam_get_distributed_devnum(asic_num,&devnum);
	if(ret != MIRROR_RETURN_CODE_SUCCESS)
	{
		nam_syslog_dbg("get distributed dev num failure !!! \n");
	}
	interfaceptr.interface.devPort.devNum = devnum;
	interfaceptr.interface.devPort.portNum = analyzerPort;

	if(SYSTEM_TYPE == IS_DISTRIBUTED)
	{
		ret = cpssDxChMirrorAnalyzerInterfaceSet(dev, profile, &interfaceptr);
		if (ret) 
		{
			nam_syslog_dbg("mirror analyer Interface set fail ret is %d\n", ret);
		}
	}
	else
	{	
		if(MIRROR_INGRESS_E== direct) {
			/*set ingress analyzer dev port*/
			ret = cpssDxChMirrorRxAnalyzerPortSet(dev,analyzerPort,analyzerDev);
			if(ret!=0) {
				nam_syslog_err("set rx mirror analyzer port fail! %d\n",ret);
			}
			/*check egress analyzer port ,is the same as the ingress port, delete it*/
			else{
				ret = cpssDxChMirrorTxAnalyzerPortGet(dev,&analyzer_port,&analyzer_dev);
				if((analyzerPort == analyzer_port)&&(analyzerDev == analyzer_dev)){
					nam_syslog_dbg("change the port egress to ingress!\n");
					/*set to default value*/
					cpssDxChMirrorTxAnalyzerPortSet(dev,0,62);
				}
			}
			cpssDxChMirrorRxAnalyzerPortGet(dev,&analyzer_port,&analyzer_dev);
			nam_syslog_dbg("dev %d ,&analyzer_port %d ,&analyzer_dev %d \n",
							dev, analyzer_port, analyzer_dev);
		}
		else if(MIRROR_EGRESS_E== direct) {
			/*set egress analyzer dev port */	
			ret = cpssDxChMirrorTxAnalyzerPortSet(dev,analyzerPort,analyzerDev);
			if(0 != ret) {
				nam_syslog_err("set tx mirror analyzer port fail %d \n",ret);
			}
			/*check ingress analyzer port ,is the same as theegress port, delete it*/
			else{
				ret = cpssDxChMirrorRxAnalyzerPortGet(dev,&analyzer_port,&analyzer_dev);
				if((analyzerPort == analyzer_port)&&(analyzerDev == analyzer_dev)){
					nam_syslog_dbg("change the port ingress to egress!\n");
					/*set to default value*/
					cpssDxChMirrorRxAnalyzerPortSet(dev,0,62);
				}
			}
		}
		else if(MIRROR_BIDIRECTION_E == direct){/*if is bidirection*/
		   ret = cpssDxChMirrorRxAnalyzerPortSet(dev,analyzerPort,analyzerDev);
		   if(ret!=0) {
				nam_syslog_err("set rx mirror analyzer port fail! %d\n",ret);
			}
		   else {
	           ret = cpssDxChMirrorTxAnalyzerPortSet(dev,analyzerPort,analyzerDev);
			   if(0 != ret) {
				   nam_syslog_err("set tx mirror analyzer port fail %d \n",ret);
			   }
		   	}
		}
		else {
			nam_syslog_err("input direct error %d\n ",direct);
			ret = MIRROR_RETURN_CODE_INPUT_DIRECT_ERROR;
		}
	}
	#endif	
#endif
#ifdef DRV_LIB_BCM
    return 0;
#endif 

	return ret;
}

unsigned int nam_mirror_analyzer_port_set_for_bcm
(
    unsigned char srcDev,
    unsigned char srcPort,
    unsigned char analyzerDev,
	unsigned char analyzerPort,
	unsigned int  direct
	
)
{
     unsigned int ret = 0;
	 unsigned int tmpdestdev = 0;
	 unsigned int tmpdestport = 0;
	 unsigned int flags = 0;
#ifdef DRV_LIB_BCM
	if(MIRROR_INGRESS_E== direct) {
		/*set ingress analyzer dev port*/
		ret = bcm_mirror_port_set((int)srcDev,(int)srcPort,(int)analyzerDev,(int)analyzerPort,2);
		if(ret!=0) {
			nam_syslog_err("set rx mirror analyzer port fail! %d\n",ret);
		}
	}
	else if(MIRROR_EGRESS_E== direct) {
		/*set egress analyzer dev port */				
		ret = bcm_mirror_port_set((int)srcDev,(int)srcPort,(int)analyzerDev,(int)analyzerPort,4);
		if(0 != ret) {
			nam_syslog_err("set tx mirror analyzer port fail %d \n",ret);
		}
	}
	else if(MIRROR_BIDIRECTION_E == direct){/*if is bidirection*/
		ret = bcm_mirror_port_set((int)srcDev,(int)srcPort,(int)analyzerDev,(int)analyzerPort,6);
	    if(ret!=0) {
			nam_syslog_err("set rxtx mirror analyzer port fail! %d\n",ret);
		}
	}
	else if(MIRROR_DIRECT_DEL_E == direct){
        ret = bcm_mirror_port_set((int)srcDev,(int)srcPort,(int)analyzerDev,(int)analyzerPort,0);
	    if(ret!=0) {
			nam_syslog_err("del rxtx mirror analyzer port fail! %d\n",ret);
		}
	}
	else {
		nam_syslog_err("input direct error %d\n ",direct);
		ret = MIRROR_RETURN_CODE_INPUT_DIRECT_ERROR;
	}
	bcm_mirror_port_get((int)srcDev,(int)srcPort,&tmpdestdev,&tmpdestport,&flags);
	nam_syslog_dbg(" bcm mirror port set destdev %d,destport %d,flags %d direct %d \n",tmpdestdev,tmpdestport,flags,direct);
#endif
#ifdef DRV_LIB_CPSS
    return 0;
#endif
    return ret;
}

/************************************************
*Get the ingress analyzer port and egress analyzer port
*OUT: analyzerrxDev & analyzerrxPort: ingress analyzer port
*        analyzertxDev & analyzertxPort: egress analyzer port
*NOTICE: here set the dev as default dev 0
************************************************/
unsigned int nam_mirror_analyzer_port_get
(
    unsigned int profile,
	unsigned char *analyzerrxDev,
	unsigned char *analyzerrxPort,
	unsigned char *analyzertxDev,
	unsigned char *analyzertxPort
	
)
{
	 unsigned int       ret=MIRROR_RETURN_CODE_ERROR;
	 unsigned char 		dev = 0;
	 unsigned char  rxPort = 0,rxDev = 0,txPort = 0,txDev = 0;
#ifdef DRV_LIB_CPSS
	#ifdef DRV_LIB_CPSS_3_4
	CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC  interfacerx,interfacetx;
	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
		ret=cpssDxChMirrorAnalyzerInterfaceGet(dev, profile, &interfacerx);
		if(ret!=0) 
		{
			nam_syslog_err("Get rx mirror analyzer port fail! ret %d\n",ret);
		}
		*analyzerrxDev = interfacerx.interface.devPort.devNum;
	    *analyzerrxPort = interfacerx.interface.devPort.portNum;
	    *analyzertxDev = interfacerx.interface.devPort.devNum;
		*analyzertxPort = interfacerx.interface.devPort.portNum;
	}
	else
	{
	     ret = cpssDxChMirrorRxAnalyzerPortGet(dev,&rxPort,&rxDev);
	     if(ret!=0) {
			nam_syslog_err("Get rx mirror analyzer port fail! ret %d\n",ret);
		 }
		 else {
	       ret = cpssDxChMirrorTxAnalyzerPortGet(dev,&txPort,&txDev);
		   if(0 != ret) {
			   nam_syslog_err("Get tx mirror analyzer port fail! ret %d \n",ret);
		   }
		   else {
	            *analyzerrxDev = rxDev;
			    *analyzerrxPort = rxPort;
			    *analyzertxDev = txDev;
				*analyzertxPort = txPort;
		   }
	    }
	}
	#endif
#endif
#ifdef DRV_LIB_BCM
    return 0;
#endif
	return ret;
}

unsigned int nam_mirror_port_set
(
	unsigned char dev,
	unsigned char port,
	unsigned int enable,
	unsigned int direct,
	unsigned int profile
)
{
	int ret = 0;
#ifdef DRV_LIB_CPSS
	#ifdef DRV_LIB_CPSS_3_4
	if(SYSTEM_TYPE == IS_DISTRIBUTED)
	{
		if(MIRROR_INGRESS_E == direct) {
		/*sprintf("nam mirror port set Rx %d/ %d , en %d, dir %d\n",dev,port,enable,direct);*/
		ret = cpssDxChMirrorRxPortSet(dev,port,enable, profile);
		if(0 != ret) {
			nam_syslog_err("nam mirror port set Rx error %d\n",ret);
		}
		}
		else if(MIRROR_EGRESS_E == direct) {
			/*printf("nam mirror port set Tx %d/ %d , en %d, dir %d\n",dev,port,enable,direct);*/
			ret = cpssDxChMirrorTxPortSet(dev,port,enable, profile);
			if(0 != ret) {
				nam_syslog_err("nam mirror port set Tx error %d\n",ret);
			}
		}
		else if(MIRROR_BIDIRECTION_E == direct){
		    ret = cpssDxChMirrorRxPortSet(dev,port,enable, profile);
			if(0 != ret) {
				nam_syslog_err("nam mirror port set Rx error %d\n",ret);
			}
			else{
				ret = cpssDxChMirrorTxPortSet(dev,port,enable, profile);
				if(0 != ret) {
					nam_syslog_err("nam mirror port set Tx error %d\n",ret);
				}
			}
		}
		else {
			ret = MIRROR_RETURN_CODE_ERROR;
		}
	}
	else
	{
		if(MIRROR_INGRESS_E == direct) {
			/*sprintf("nam mirror port set Rx %d/ %d , en %d, dir %d\n",dev,port,enable,direct);*/
			ret = cpssDxChMirrorRxPortSet(dev,port,enable, profile);
			if(0 != ret) {
				nam_syslog_err("nam mirror port set Rx error %d\n",ret);
			}
		}
		else if(MIRROR_EGRESS_E == direct) {
			/*printf("nam mirror port set Tx %d/ %d , en %d, dir %d\n",dev,port,enable,direct);*/
			ret = cpssDxChMirrorTxPortSet(dev,port,enable, profile);
			if(0 != ret) {
				nam_syslog_err("nam mirror port set Tx error %d\n",ret, profile);
			}
		}
		else if(MIRROR_BIDIRECTION_E == direct){
		    ret = cpssDxChMirrorRxPortSet(dev,port,enable, profile);
			if(0 != ret) {
				nam_syslog_err("nam mirror port set Rx error %d\n",ret);
			}
			else{
				ret = cpssDxChMirrorTxPortSet(dev,port,enable, profile);
				if(0 != ret) {
					nam_syslog_err("nam mirror port set Tx error %d\n",ret);
				}
			}
		}
		else {
			ret = MIRROR_RETURN_CODE_ERROR;	
		}
	}
	#endif
#endif
#ifdef DRV_LIB_BCM
    return 0;
#endif
	return ret;
	
}

unsigned int nam_mirror_vlan_set
(
	unsigned short vid,
	unsigned int enable
)
{
	int ret = 0;
	int dev = 0;

#ifdef DRV_LIB_CPSS 

	ret =  cpssDxChBrgVlanIngressMirrorEnable(dev,vid,enable);
	if(0 != ret) {
		nam_syslog_err("nam mirror vlan set error %d\n",ret);
	}

#endif
#ifdef DRV_LIB_BCM
    return 0;
#endif
	return ret;
}
unsigned int nam_mirror_vlan_set_for_bcm
(
	unsigned short vid,
	unsigned char destDev,
	unsigned char destPort,
	unsigned int enable
)
{
    int ret = 0;
#ifdef DRV_LIB_CPSS
    return 0;
#endif
#ifdef DRV_LIB_BCM
    if(enable){
        ret = bcm_mirror_vlan_set(destDev,destPort,0,vid);
    }
	else {
        ret = bcm_mirror_vlan_set(destDev,destPort,0,0);
	}
	if(0 != ret){
        nam_syslog_err("nam mirror vlan set for bcm error %d, enable %d\n",ret,enable);
	}
#endif
	return ret;
}

unsigned int nam_mirror_fdb_entry_set
(
   unsigned short vlanid,
   unsigned int eth_g_index,
   ETHERADDR *macAddr,
   unsigned int flag
)
{
    unsigned long ret;
	unsigned int hashPtr;
	unsigned long  skip = 0;
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	ETHERADDR  *macPtr = NULL;
    ETHERADDR  *readPtr = NULL;
	unsigned int					 validPtr;
	unsigned int					 skipPtr;
	unsigned int 					 agedPtr;
	unsigned char					 associatedDevNumPtr;
#ifdef DRV_LIB_CPSS 
	CPSS_MAC_ENTRY_EXT_STC       macEntryPtr;
	CPSS_MAC_ENTRY_EXT_STC       readEntryPtr;

	memset(&macEntryPtr, 0 ,sizeof(CPSS_MAC_ENTRY_EXT_STC));
	
    macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macPtr = &(macEntryPtr.key.key.macVlan.macAddr);
    memcpy(macPtr->arEther,macAddr,6);
    macEntryPtr.key.key.macVlan.vlanId = (unsigned short)vlanid;
	ret=cpssDxChBrgFdbHashCalc(  devNum,&macEntryPtr.key, &hashPtr);
	
	if( ret!=0){
		syslog_ax_nam_mirror_err("cpssDxChBrgFdbHashCalc erro is :%ld\n",ret);
		return MIRROR_RETURN_CODE_ERROR;
		}
	ret =cpssDxChBrgFdbMacEntryRead( devNum, hashPtr, &validPtr,\
													 &skipPtr,&agedPtr,&associatedDevNumPtr,&readEntryPtr); 
	
	if( ret!=0){
		syslog_ax_nam_mirror_err("cpssDxChBrgFdbMacEntryRead erro is :%ld\n",ret);
		return MIRROR_RETURN_CODE_ERROR;
		}
	readPtr=&(readEntryPtr.key.key.macVlan.macAddr);	
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
    if(MIRROR_RETURN_CODE_SUCCESS != ret){
		syslog_ax_nam_fdb_err("eth_index %#x get asic port failed for ret %d\n",eth_g_index,ret);
        return MIRROR_RETURN_CODE_ERROR;
	}

	#if 0
	 macEntryPtr.dstInterface.trunkId=0;
	 macEntryPtr.dstInterface.vidx=0;
	 macEntryPtr.dstInterface.vlanId=0;
	#endif	
	if( 1 == flag){/*create the fdb entry*/
        macEntryPtr.isStatic = NAM_TRUE;/* static FDB */
		macEntryPtr.mirrorToRxAnalyzerPortEn = NAM_TRUE;/*mirror to Analyzer port*/
	}
    else if(0 == flag){/*delete the fdb entry*/
		macEntryPtr.isStatic = NAM_FALSE;/* static FDB */
		macEntryPtr.mirrorToRxAnalyzerPortEn = NAM_FALSE;/*mirror to Analyzer port*/
    }
	macEntryPtr.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macEntryPtr.dstInterface.devPort.devNum = devNum;
	macEntryPtr.dstInterface.devPort.portNum = portNum;
    macEntryPtr.daRoute =NAM_FALSE;/* no L3 route */
	macEntryPtr.daCommand = readEntryPtr.daCommand;
	macEntryPtr.saCommand = readEntryPtr.saCommand;
    macEntryPtr.sourceID = 0;
    macEntryPtr.userDefined = 0;
    macEntryPtr.daQosIndex = 0;
    macEntryPtr.saQosIndex = 0;
    macEntryPtr.daSecurityLevel = 0;
    macEntryPtr.saSecurityLevel = 0;
    macEntryPtr.appSpecificCpuCode = NAM_FALSE;

	syslog_ax_nam_mirror_dbg("The mac is %02x:%02x:%02x:%02x:%02x:%02x\n",macEntryPtr.key.key.macVlan.macAddr.arEther[0],
		                         macEntryPtr.key.key.macVlan.macAddr.arEther[1],macEntryPtr.key.key.macVlan.macAddr.arEther[2],
		                         macEntryPtr.key.key.macVlan.macAddr.arEther[3],macEntryPtr.key.key.macVlan.macAddr.arEther[4],
		                         macEntryPtr.key.key.macVlan.macAddr.arEther[5]);
    syslog_ax_nam_mirror_dbg("The dacommand is %d,the sacommand is %d\n",macEntryPtr.daCommand,macEntryPtr.saCommand);
	if(1 == flag){
		ret=cpssDxChBrgFdbMacEntryWrite(devNum, hashPtr, skip, &macEntryPtr);
		if(0 != ret) {
			syslog_ax_nam_fdb_err("cpssDxChBrgStaticFdbMacEntrySet erro! \n");
			syslog_ax_nam_fdb_err("err vlaue is: %ld\n",ret);
			return MIRROR_RETURN_CODE_ERROR;
		}
    }
	else if(0 == flag){
        skip = 1;
		ret=cpssDxChBrgFdbMacEntryWrite(devNum, hashPtr, skip, &macEntryPtr);
		if(0 != ret) {
			syslog_ax_nam_fdb_err("cpssDxChBrgStaticFdbMacEntrySet erro! \n");
			syslog_ax_nam_fdb_err("err vlaue is: %ld\n",ret);
			return MIRROR_RETURN_CODE_ERROR;
		}
	}
#endif
	return MIRROR_RETURN_CODE_SUCCESS;
   
}
unsigned int nam_is_bcm_check()
{
#ifdef DRV_LIB_BCM
    return NAM_TRUE;
#endif
    return NAM_FALSE;
}

unsigned int profile_check(unsigned int profile){
#ifdef DRV_LIB_CPSS
       if(profile > 6){
	   	  return MIRROR_RETURN_CODE_ERROR;
       }
#endif
#ifdef DRV_LIB_BCM
       if((profile > 7)||(profile < 1)){
	   	  return MIRROR_RETURN_CODE_ERROR;
       }
#endif
       return MIRROR_RETURN_CODE_SUCCESS;
}


#ifdef __cplusplus
}
#endif
