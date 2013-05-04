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
* nam_dldp.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		NAM module implement for device link detection protocol
*
* DATE:
*		05/04/2009	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.3 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************
*	head files														*
**********************************************************/
#include "nam_dldp.h"

/*********************************************************
*	global variable define											*
**********************************************************/

/*********************************************************
*	extern variable												*
**********************************************************/

/*********************************************************
*	functions define												*
**********************************************************/

/**********************************************************************************
 *	nam_dldp_set_vlan_filter
 * 
 *  DESCRIPTION:
 *		set vlan filter, trap NON IPv4 BCast
 *
 *  INPUT:
 *		unsigned short vlanId
 *		unsigned char enable
 *  
 *  OUTPUT:
 * 	 	NULL
 *
 *  RETURN:
 * 		NAM_ERROR		- set fail
 *	 	NAM_OK			- set ok
 *
 **********************************************************************************/
unsigned int nam_dldp_set_vlan_filter
(
	unsigned short vlanId,
	unsigned char enable
)
{
	unsigned ret = NAM_OK;
	unsigned char devNum = 0;

	#ifdef DRV_LIB_CPSS
	if (enable) {
		/*Trap NON IPv4 BCast*/
		cpssDxChBrgVlanUnkUnregFilterSet(devNum, vlanId,
									CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E,
									CPSS_PACKET_CMD_TRAP_TO_CPU_E);
	}else {
		/* no Trap NON IPv4 BCast*/
		cpssDxChBrgVlanUnkUnregFilterSet(devNum, vlanId,
									CPSS_DXCH_BRG_VLAN_PACKET_UNREG_NON_IPV4_BCAST_E,
									CPSS_PACKET_CMD_FORWARD_E);
	}
	#endif

	return ret;
}


#ifdef __cplusplus
}
#endif







