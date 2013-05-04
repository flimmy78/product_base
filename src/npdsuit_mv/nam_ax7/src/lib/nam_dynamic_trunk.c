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
* nam_dynamic_trunk.c
*
*
* CREATOR:
*		hanhui@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for TRUNK module.
*
* DATE:
*		09/02/2010
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.5 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "npd/nam/npd_amapi.h"
#include "nam_asic.h"
#include "nam_eth.h"
#include "nam_trunk.h"
#include "nam_log.h"
#include "sysdef/returncode.h"
#include "nam_dynamic_trunk.h"
#include "nam_trunk.h"

/*****************************************************************************
 *nam_asic_show_dynamic_trunk_hwinfo
 *
 * DESCRIPTION:
 *			return the enabled and disabled port members in a dynamic trunk
 * INPUT:
 *                trunkId : uint16 -  the dynamic trunkId that want to show
 * OUTPUTS:
 *                numOfEnabledMembersPtr:  (pointer to) the actual num of enabled members in the trunk
 *                                                     (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
 *                enabledMembersArray:       (array of) enabled members of the trunk array was allocated by the caller
 *                numOfDisabledMembersPtr: (pointer to) the actual num of disabled members in the trunk
 *                                                     (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
 *                disabledMembersArray :      (array of) disabled members of the trunk array was allocated by the caller
 * RETURN:
 *			NAM_ERR 
 *			NAM_OK
 * NOTE:
 *
 *****************************************************************************/

int nam_asic_show_dynamic_trunk_hwinfo
(		
		unsigned short trunkId,
		unsigned long *numOfEnabledMembersPtr,
		unsigned long *numOfDisabledMembersPtr,
		CPSS_TRUNK_MEMBER_STC  *enabledMembersArray,
		CPSS_TRUNK_MEMBER_STC  *disabledMembersArray
)
{
		unsigned long ret = TRUNK_RETURN_CODE_ERR_NONE;
		unsigned int  num = 0;
		unsigned char   devNum = 0;
		unsigned long          numOfEnabledMembers=0;
		unsigned long          numOfDisabledMembers=0;
#ifdef DRV_LIB_CPSS       
		numOfEnabledMembers = MAX_DYNAMIC_TRUNK_MEMBER;
        for(num = 0;num<=numOfEnabledMembers;num++)
        	{
             enabledMembersArray[num].devNum = 0;
			 enabledMembersArray[num].portNum = 0;
		}
		
		ret = cpssDxChTrunkDbEnabledMembersGet(devNum,trunkId,&numOfEnabledMembers,enabledMembersArray);		 
		*numOfEnabledMembersPtr = numOfEnabledMembers;
		if(0!=ret)
			{
			npd_syslog_err("nam get enable hardware info error,ret %#x", ret);
            return NAM_ERR;
		}
		
		numOfDisabledMembers = MAX_DYNAMIC_TRUNK_MEMBER;
        for(num = 0;num<=numOfDisabledMembers;num++)
        	{
             disabledMembersArray[num].devNum = 0;
			 disabledMembersArray[num].portNum = 0;
		}
		ret = cpssDxChTrunkDbDisabledMembersGet(devNum,trunkId,&numOfDisabledMembers,disabledMembersArray);
		*numOfDisabledMembersPtr = numOfDisabledMembers;
		if(0!=ret)
			{
			npd_syslog_err("nam get disable hardware info error,ret %#x", ret);
            return NAM_ERR;
		}
#endif	
        return ret;
}


/*********************************************************************
 * nam_dynamic_trunk_lacp_trap_init
 * DESCRIPTION:
 *			init driver for dynamic trunk
 * INPUT:
 *			devNum : uint8  -  the dev num we want to init
 * OUTPUT:
 *			NONE
 * RETURN:
 *			NAM_OK  -  init success
 *			NAM_ERR    -    init failed 
 * NOTE:
 *
 *********************************************************************/
int nam_dynamic_trunk_lacp_trap_init
(
	unsigned char devNum
)
{
	int result = 0;
	
#ifdef DRV_LIB_CPSS
#if 1
	unsigned char ieeeResProtocol = 0x2;/*for lacp*/
	unsigned int ieeeCpuCode = 326;/*CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E, *//* PRV_CPSS_DXCH_NET_DSA_TAG_IEEE_RSRVD_MULTICAST_ADDR_1_E */
#ifdef DRV_LIB_CPSS_3_4
	result = cpssDxChBrgGenIeeeReservedMcastProtCmdSet(devNum,0, ieeeResProtocol,2 /*CPSS_PACKET_CMD_TRAP_TO_CPU_E*/);
#else
	result = cpssDxChBrgGenIeeeReservedMcastProtCmdSet(devNum,ieeeResProtocol,2 /*CPSS_PACKET_CMD_TRAP_TO_CPU_E*/);
#endif
	if(NAM_OK != result) {
		nam_syslog_err("set IEEE reserved MC trap CPU error %d\n",result);
		return NAM_ERR;
	}
	result = cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet(devNum,ieeeResProtocol,ieeeCpuCode);
	if(NAM_OK != result) {
		nam_syslog_err("build IEEE reserved MC trap CPU code error %d\n",result);
		return NAM_ERR;
	}
	result = cpssDxChBrgGenIeeeReservedMcastTrapEnable(devNum,1);
	if(NAM_OK != result) {
		nam_syslog_err("enable bridge trap IEEE reserved multicast packet to CPU error %d\n",result);
		return NAM_ERR;
	}
	nam_syslog_event("lacp trap init success\n");
#endif
#endif
#ifdef DRV_LIB_BCM
#endif
	return NAM_OK;
}

#ifdef __cplusplus
}
#endif

