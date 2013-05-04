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
* nam_eth.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NAM for ETHERNET PORT routine.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.105 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <memory.h>

#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "nam_vlan.h"
#include "nam_eth.h"
#include "nam_asic.h"
#include "nam_log.h"

#ifdef DRV_LIB_BCM
#include <bcm/port.h>
#include <bcm/stat.h>
#include <soc/mcm/memregs.h>
#include <bcm/rate.h>
#endif

#ifdef DEBUG_NAM_INFO
unsigned int nam_info_debug = 1;
#else
unsigned int nam_info_debug = 0;
#endif

#define STR(a)	#a
#define NAM_INFO_DETAIL(x) if(nam_info_debug) printf x

char* nam_err_msg[] = {				\
	STR(NAM_ERR_NONE),					\
	STR(NAM_ERR_SLOT_OUT_OF_RANGE),		\
	STR(NAM_ERR_PORT_OUT_OF_RANGE),		\
	STR(NAM_ERR_MODULE_NOT_SUPPORT),	\
	STR(NAM_ERR_DEVICE_NUMBER),			\
	STR(NAM_ERR_PORT_ON_DEVICE),		\
	STR(NAM_ERR_NO_MATCH),				\
	STR(NAM_ERR_MAX)					\
};

/* MODULE_ID_AX7_6GTX panel port to virtual port map array */
unsigned int ax7_6gtx_port_mapArr[NAM_AX7_MAX_SLOTNO][NAM_AX7_MAX_PORT_PERSLOT] = {
	/*slot 1:port 23~18*/
	{0x17, 0x16, 0x15, 0x14, 0x13, 0x12},
	/*slot 2:port 11~6*/
	{0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06},
	/*slot 3:port 17~12*/
	{0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c},
	/*slot 4:port 5~0*/
	{0x05, 0x04, 0x03, 0x02, 0x01, 0x00}
};

/* MODULE_ID_AX7_XFP panel port to virtual port map array */
struct xg_port_info_s ax7_xfp_port_mapArr[NAM_AX7_MAX_SLOTNO][NAM_AX7_MAX_XG_PORT_PERSLOT] = { \
	/*slot 1:port 24 on 98Dx804*/
	{{0x1,0x18}},
	/*slot 2:port 24 on 98Dx275*/
	{{0x0,0x18}},
	/*slot 1:port 25 on 98Dx804*/
	{{0x1,0x19}},
	/*slot 2:port 25 on 98Dx275*/
	{{0x0,0x19}},
};
/* MODULE_ID_AX7_6GE_SFP panel port to virtual port map array */
unsigned int ax7_6ge_sfp_port_mapArr[NAM_AX7_MAX_SLOTNO][NAM_AX7_MAX_PORT_PERSLOT] = {
	/*slot 1:port 23~18*/
	{0x17, 0x16, 0x15, 0x14, 0x13, 0x12},
	/*slot 2:port 11~6*/
	{0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06},
	/*slot 3:port 17~12*/
	{0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c},
	/*slot 4:port 5~0*/
	{0x05, 0x04, 0x03, 0x02, 0x01, 0x00}
};

/* MODULE_ID_AX7I_GTX panle port to virtual port map array */
unsigned int ax7i_alpha_gtx_port_mapArr[NAM_AX7i_alpha_MAX_SLOTNO][NAM_AX7i_alpha_MAX_PORT_PERSLOT] = {
	/* port 23~18  */
	{0x17, 0x16, 0x15, 0x14, 0x13, 0x12}
};

/* MODULE_ID_AX7I_XG_CONNECT panle port to virtual port map array */
struct xg_port_info_s ax7i_alpha_xg_conn_port_mapArr[NAM_AX7i_alpha_MAX_SLOTNO][NAM_AX7i_alpha_MAX_INTCONN_XPORT] = {
	/* port 24 & 25 on 98Dx804*/
	{{0x1, 0x18},{0x1, 0x19}},
};

/* MODULE_ID_AX5_5612 panel port to virtual port map array */
unsigned int ax5_5612_port_mapArr[NAM_AX5_MAX_SLOTNO][NAM_AX5_MAX_PORT_PERSLOT] = {
	{
		0xc ,0xd ,0xe ,0xf ,0x10,0x11,
		0x12,0x13,0x14,0x15,0x16,0x17
	}
};

/* MODULE_ID_AX5_5612I panel port to virtual port map array */
unsigned int ax5_5612i_port_mapArr[NAM_AX5I_MAX_SLOTNO][NAM_AX5I_MAX_PORT_PERSLOT] = {
	{
		0x0 ,0x1 ,0x2 ,0x3 ,0x4 ,0x5 ,
		0x6 ,0x7
	}
};

/* MODULE_ID_AX5_5612I xg port  */

struct xg_port_info_s ax5i_xfp_port_mapArr[NAM_AX5I_MAX_SLOTNO][NAM_AX5I_MAX_XG_PORT_PERSLOT] = {
	/* port 25 on 98Dx275*/
	{{0x0, 0x19}}
};

/* MODULE_ID_AX5_5612E panel port to virtual port map array */
unsigned int ax5_5612e_port_mapArr[NAM_AX5E_MAX_SLOTNO][NAM_AX5E_MAX_PORT_PERSLOT] = {
	{
		0x0 ,0x1 ,0x2 ,0x3 ,0x4 ,0x5 ,
		0x6 ,0x7
	}
};

/* MODULE_ID_AX5_5608_E panel port to virtual port map array */
unsigned int ax5_5608e_port_mapArr[NAM_AX5E_MAX_SLOTNO][NAM_AX5E_MAX_PORT_PERSLOT] = {
	{
		0x0 ,0x1 ,0x2 ,0x3 ,0x4 ,0x5 ,
		0x6 ,0x7
	}
};

/* MODULE_ID_AU4_4626 panel port to virtual port map array */
unsigned int au4_4626_port_mapArr[NAM_AU4_MAX_SLOTNO][NAM_AU4_MAX_PORT_PERSLOT] = {
	{
		0x0 ,0x1 ,0x2 ,0x3 ,0x4 ,0x5 ,
		0x6 ,0x7 ,0x8 ,0x9 ,0xa ,0xb ,
		0xc ,0xd ,0xe ,0xf ,0x10,0x11,
		0x12,0x13,0x14,0x15,0x16,0x17
	}
};

/* MODULE_ID_AU3_3524 panel port to virtual port map array */
unsigned int au3_3524_port_mapArr[NAM_AU3_MAX_SLOTNO][NAM_AU3_MAX_PORT_PERSLOT] = {
	{
		0x0 ,0x1 ,0x2 ,0x3 ,0x4 ,0x5 ,
		0x6 ,0x7 ,0x8 ,0x9 ,0xa ,0xb ,
		0xc ,0xd ,0xe ,0xf ,0x10,0x11,
		0x12,0x13,0x14,0x15,0x16,0x17
	}
};

/* MODULE_ID_AU3_3524 panel port to virtual port map array */
gen_asic_port_info_s au3_3052_port_mapArr[NAM_AU3_BCM_MAX_SLOTNO][NAM_AU3_BCM_MAX_PORT_PERSLOT] = {
	{
		{0,0x6} ,{0,0x7} ,{0,0x8} ,{0,0x9} ,{0,0xa} ,{0,0xb} ,
		{0,0xc} ,{0,0xd} ,{0,0xe} ,{0,0xf} ,{0,0x10},{0,0x11},
		{0,0x12},{0,0x13},{0,0x14},{0,0x15},{0,0x16},{0,0x17},
		{0,0x18},{0,0x19},{0,0x1a},{0,0x1b},{0,0x1c},{0,0x1d},
		{1,0x6} ,{1,0x7} ,{1,0x8} ,{1,0x9} ,{1,0xa} ,{1,0xb} ,
		{1,0xc} ,{1,0xd} ,{1,0xe} ,{1,0xf} ,{1,0x10},{1,0x11},
		{1,0x12},{1,0x13},{1,0x14},{1,0x15},{1,0x16},{1,0x17},
		{1,0x18},{1,0x19},{1,0x1a},{1,0x1b},{1,0x1c},{1,0x1d},
		{0,0x1},{1,0x1},{0,0x2},{1,0x2}
	}
};

/* MODULE_ID_AU3_3528 panel port to virtual port map array */
unsigned int au3_3528_port_mapArr[NAM_AU3528_MAX_SLOTNO][NAM_AU3528_MAX_PORT_PERSLOT] = {
	{
		0x18,0x19,0x1,0x0,0x3,0x2,0x5,0x4,0x7,0x6,
		0x9,0x8,0xb,0xa,0xe,0xf,0xc,0xd,0x12,0x13,
		0x10,0x11,0x16,0x17,0x14,0x15,0x1A,0x1B
	}
};

/* MODULE_ID_AU2_2528 panel port to virtual port map array */
unsigned int au2_2528_port_mapArr[NAM_AU2528_MAX_SLOTNO][NAM_AU2528_MAX_PORT_PERSLOT] = {
	{
		0x0 ,0x1 ,0x2 ,0x3 ,0x4 ,0x5 ,
		0x6 ,0x7 ,0x8 ,0x9 ,0xa ,0xb ,
		0xc ,0xd ,0xe ,0xf ,0x10,0x11,
		0x12,0x13,0x14,0x15,0x16,0x17,
		0x18,0x19,0x1A,0x1B
	}
};

/* MODULE_ID_AU4_4626 panel port to virtual port map array */
unsigned int au4_4626_subcard_port_mapArr[NAM_AU4626_SUBCARD_MAX_SLOTNO][NAM_AU4626_SUBCARD_MAX_PORT_PERSLOT] = {
	{
		0x18,0x19
	}
};

/* MODULE_ID_AX71_2X12G12S panel port to virtual port map array */
unsigned int ax7605i_port_mapArr[NAM_AX7605i_MAX_SLOTNO][NAM_AX7605i_MAX_PORT_PERSLOT] = {
	/*slot 1:port 0-1 -> 0-4 cavium */
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},    /* need modify !!!!!!!!!!!! zhangdi*/
	/*slot 2:port 1-1 -> 1-4 cavium */
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},    /* need modify !!!!!!!!!!!! zhangdi*/
	/*slot 3:port 2-1 -> 2-28 XCAT*/
	{
		0x18,0x19,0x1,0x0,0x3,0x2,0x5,0x4,0x7,0x6,
		0x9,0x8,0xb,0xa,0xe,0xf,0xc,0xd,0x12,0x13,
		0x10,0x11,0x16,0x17,0x14,0x15,0x1A,0x1B
	}
};

/**********************************************************************************
 *
 * get PP device number and virtual port info
 *
 *	INPUT:
 *		slotNo - physical slot number
 *		localPortNo - port number on slot (e.g. 6/1 has slot 6,port 1)
 *	
 *	OUTPUT:
 *		devNum - PP device number
 *		virtPortNo - virtual port number in PP device
 *
 * 	RETURN:
 *		NAM_ERR_NONE -  if no error.
 *		NAM_ERR_SLOT_OUT_OF_RANGE - if slot number out of range (error)
 *		NAM_ERR_PORT_OUT_OF_RANGE - if port number out of range (error)
 *		NAM_ERR_MODULE_NOT_SUPPORT - if module not support this operation.
 *		
 *
 **********************************************************************************/
unsigned int nam_get_devport_by_slotport
(
	IN 		unsigned int slotNo,
	IN 		unsigned int localPortNo,
	IN		unsigned int moduleType,
	OUT		unsigned int* devNum,
	OUT 	unsigned int* virtPortNo
)
{	
	unsigned int	retVal = NAM_ERR_NONE;
	struct xg_port_info_s xgport ;
	gen_asic_port_info_s asicport ;
	unsigned int slot_l = 0,slot_h = 0,start = 0, end = 0;
    if(SYSTEM_TYPE == IS_DISTRIBUTED) 
    {
	    *virtPortNo	= asic_board->asic_eth_ports[localPortNo - asic_board->asic_port_start_no].port_num;
	    *devNum = asic_board->asic_eth_ports[localPortNo - asic_board->asic_port_start_no].dev_num;
		syslog_ax_nam_eth_dbg("nam convert %d-%d to devport (%d,%d) OK \n",slotNo,localPortNo,*devNum,*virtPortNo);
		return NAM_ERR_NONE;
	}
	
	/* minimum slot number*/
	slot_l = 1;
	if(MODULE_ID_SUBCARD_TYPE_A1 == moduleType) {
		slot_l = 0;
	}
	/* maximum slot number*/
	slot_h = (MODULE_ID_AX7_6GTX == moduleType) ? 4 :		\
			 (MODULE_ID_AX7_6GE_SFP == moduleType) ? 4 :	\
			 (MODULE_ID_AX7_XFP == moduleType) ? 4 :    	\
			 (MODULE_ID_AX7_6GTX_POE == moduleType) ? 4 :	\
			 (MODULE_ID_AX7I_GTX == moduleType) ? 1 : \
			 (MODULE_ID_AX7I_XG_CONNECT == moduleType) ? 4 : \
			 (MODULE_ID_AU4_4626 == moduleType) ? 1 :		\
			 (MODULE_ID_AU4_4524 == moduleType) ? 1 :	   \
			 (MODULE_ID_AU4_4524_POE == moduleType) ? 1 :	   \
			 (MODULE_ID_AU3_3524 == moduleType) ? 1:  \
			 (MODULE_ID_AU3_3052 == moduleType) ? 1:  \
			 (MODULE_ID_AU3_3528 == moduleType) ? 1:  \
			 (MODULE_ID_AU2_2528 == moduleType) ? 1:  \
			 (MODULE_ID_AX5_5612 == moduleType) ? 1:  \
			 (MODULE_ID_AX5_5612I == moduleType) ? 1: \
			 (MODULE_ID_SUBCARD_TYPE_A1 == moduleType) ? 0: \
			 (MODULE_ID_AX5_5612E == moduleType) ? 1: \
			 (MODULE_ID_AX5_5608 == moduleType) ? 1: \
			 (MODULE_ID_AX71_2X12G12S == moduleType) ? 3:0;
	/* start port number on module*/
	start = 1;
	/* end port number on module*/
	end = (MODULE_ID_AX7_6GTX == moduleType) ? 6 :			\
			(MODULE_ID_AX7_6GE_SFP == moduleType) ? 6 :	\
			(MODULE_ID_AX7_XFP == moduleType) ? 1 : 		\
			(MODULE_ID_AX7_6GTX_POE == moduleType) ? 6 :   \
			(MODULE_ID_AX7I_GTX == moduleType) ? 6 : \
			(MODULE_ID_AX7I_XG_CONNECT == moduleType) ? 2 : \
			(MODULE_ID_AU4_4626 == moduleType) ? 24 :		\
			(MODULE_ID_AU4_4524 == moduleType) ? 24 :		\
			(MODULE_ID_AU4_4524_POE == moduleType) ? 24 :	\
			(MODULE_ID_AU3_3524 == moduleType) ? 24:    \
			(MODULE_ID_AX5_5612 == moduleType) ? 12 : \
			(MODULE_ID_AX5_5612I == moduleType) ? 8 : \
			(MODULE_ID_AX5_5612E == moduleType) ? 8 : \
			(MODULE_ID_AX5_5608 == moduleType) ? 8 : \
			(MODULE_ID_AU3_3528 == moduleType) ? 28:  \
			(MODULE_ID_AU2_2528 == moduleType) ? 28:  \
			(MODULE_ID_SUBCARD_TYPE_A1 == moduleType) ? 1: \
			(MODULE_ID_AU3_3052 == moduleType) ? 52 : \
	        (MODULE_ID_AX71_2X12G12S == moduleType) ? 192:0;   /* slot-2:64*2 -> 64*3 */
	
	if((!NAM_SLOTNO_ISLEGAL(slotNo,slot_l,slot_h))&&(MODULE_ID_AU3_3528 !=moduleType))
	{
		retVal = NAM_ERR_SLOT_OUT_OF_RANGE;
	}
	else if(!NAM_PORTNO_ISLEGAL(localPortNo,start,end))
	{
		retVal = NAM_ERR_PORT_OUT_OF_RANGE;
	}
	else {
		switch(moduleType)
		{
			default:
				retVal = NAM_ERR_MODULE_NOT_SUPPORT;
				break;
			case MODULE_ID_AX7_CRSMU:
			case MODULE_ID_AX7I_CRSMU:
				retVal = NAM_ERR_MODULE_NOT_SUPPORT;
				break;
			case MODULE_ID_AX7_6GTX:
				*devNum 	= 0;
				*virtPortNo	= ax7_6gtx_port_mapArr[slotNo-1][localPortNo-1];
				break;
			case MODULE_ID_AX7_6GE_SFP:
				*devNum 	= 0;
				*virtPortNo	= ax7_6ge_sfp_port_mapArr[slotNo-1][localPortNo-1];
				break;
			case MODULE_ID_AX7_XFP:
				memset(&xgport, 0, sizeof(struct xg_port_info_s));
				xgport = ax7_xfp_port_mapArr[slotNo-1][localPortNo-1];
				*devNum = xgport.devNum;
				*virtPortNo = xgport.portNum;
				break;
			case MODULE_ID_AX7I_GTX:
				*devNum = 0;
				*virtPortNo = ax7i_alpha_gtx_port_mapArr[slotNo-1][localPortNo-1];
				break;
			case MODULE_ID_AX7I_XG_CONNECT:
				memset(&xgport, 0, sizeof(struct xg_port_info_s));
				xgport = ax7i_alpha_xg_conn_port_mapArr[slotNo-3][localPortNo-1];
				*devNum = xgport.devNum;
				*virtPortNo = xgport.portNum;
				break;
			case MODULE_ID_AX5_5612:
				*devNum 	= 0;
				*virtPortNo	= ax5_5612_port_mapArr[slotNo-1][localPortNo-1];				
				break;				
			case MODULE_ID_AX5_5612I:
				*devNum 	= 0;
				*virtPortNo	= ax5_5612i_port_mapArr[slotNo-1][localPortNo-1];
				break;
			case MODULE_ID_AX5_5612E:
				*devNum 	= 0;
				*virtPortNo = ax5_5612e_port_mapArr[slotNo-1][localPortNo-1];
				break;
			case MODULE_ID_AX5_5608:
				*devNum 	= 0;
				*virtPortNo = ax5_5608e_port_mapArr[slotNo-1][localPortNo-1];
				break;
			case MODULE_ID_AU4_4626:
			case MODULE_ID_AU4_4524:
			case MODULE_ID_AU4_4524_POE:
				*devNum 	= 0;
				*virtPortNo	= au4_4626_port_mapArr[slotNo-1][localPortNo-1];				
				break;
			case MODULE_ID_AU3_3524:
				*devNum 	= 0;
				*virtPortNo = au3_3524_port_mapArr[slotNo-1][localPortNo-1];
				break;
			case MODULE_ID_AU3_3052:
				memset(&asicport, 0, sizeof(gen_asic_port_info_s));
				asicport = au3_3052_port_mapArr[slotNo-1][localPortNo-1];
				*devNum = asicport.devNum;
				*virtPortNo = asicport.portNum;
				syslog_ax_nam_eth_dbg("AU3052 port panel %d/%d to asic (%d,%d)\n", \
								slotNo, localPortNo, *devNum, *virtPortNo);
				break;
			case MODULE_ID_AU3_3528:
				*devNum 	= 0;
				if(slotNo == 2)
					*virtPortNo	= au3_3528_port_mapArr[slotNo-2][localPortNo-1];
				break;
			case MODULE_ID_AU2_2528:
				*devNum 	= 0;
				*virtPortNo	= au2_2528_port_mapArr[slotNo-1][localPortNo-1];
				break;
			case MODULE_ID_SUBCARD_TYPE_A0:
				*devNum 	= 0;
				*virtPortNo = 0x18;
				break;
			case MODULE_ID_SUBCARD_TYPE_A1:
				*devNum 	= 0;
				*virtPortNo = 0x19;
				break;
			case MODULE_ID_SUBCARD_TYPE_B:
				*devNum 	= 0;
				*virtPortNo	= au4_4626_subcard_port_mapArr[slotNo][localPortNo-1];
				break;
			case MODULE_ID_AX71_2X12G12S:
				*devNum 	= 0;
				/*code optimize:  Out-of-bounds read
				zhangcl@autelan.com 2013-1-17*/
				if(slotNo  == 3)
				{
					break;
				}
				*virtPortNo	= ax7605i_port_mapArr[slotNo][localPortNo-1];
				break;
				
		}
	}
	return retVal;
}

/**********************************************************************************
 *
 * get PP device number and virtual port info
 *
 *	INPUT:
 *		devNum - PP device number
 *		virtPortNo - virtual port number in PP device
 *		productId - prodcut id
 *	
 *	OUTPUT:
 *		slotNo - physical slot number
 *		localPortNo - port number on slot (e.g. 6/1 has slot 6,port 1)
 *
 * 	RETURN:
 *		NAM_ERR_NONE -  if no error.
 *		NAM_ERR_SLOT_OUT_OF_RANGE - if slot number out of range (error)
 *		NAM_ERR_PORT_OUT_OF_RANGE - if port number out of range (error)
 *		NAM_ERR_MODULE_NOT_SUPPORT - if module not support this operation.
 *	
 *	6GTX sub card port map (all port on device<0> 98Dx275):	
 * 	+----------------+----------------+
 *	| slot3 (0x11-0x0c) | slot4 (0x05-0x00) |
 * 	+----------------+----------------+
 *	| slot1 (0x17-0x12) | slot2 (0x0b-0x06) |
 * 	+----------------+----------------+
 *	|			     CRSMU			    |
 * 	+----------------+----------------+
 *
 *	XFP sub card port map (XG port on both device<0> 98Dx275 and device<1> 98Dx804 ):	
 * 	+----------------+----------------+
 *	| 	slot3 (1,25)	  | 	slot4 (0,25)	    |
 * 	+----------------+----------------+
 *	| 	slot1 (1,24)	  | 	slot2 (0,24)	    |
 * 	+----------------+----------------+
 *	|			     CRSMU			    |
 * 	+----------------+----------------+
 *
 **********************************************************************************/
unsigned int nam_get_slotport_by_devport
(
	IN		unsigned int devNum,
	IN 		unsigned int virtPortNo,
	IN		unsigned int productId,
	OUT 	unsigned int* slotNo,
	OUT 	unsigned int* localPortNo
)
{	
	unsigned int	retVal = NAM_ERR_NO_MATCH;
	int i = 0,j = 0, slot_max = 0,port_count = 0;
	struct xg_port_info_s xgport;
	gen_asic_port_info_s geport;
	unsigned char found = 0;
	unsigned int ge_port_max = 0,port_max = 0,dev_max = 0;

    if(SYSTEM_TYPE == IS_DISTRIBUTED) 
    {
		for(i=0;i<asic_board->asic_port_cnt;i++)
		{
			if((devNum == asic_board->asic_eth_ports[i].dev_num)&&(virtPortNo == asic_board->asic_eth_ports[i].port_num))
			{
			    *localPortNo = i + asic_board->asic_port_start_no;			
		        *slotNo = asic_board->slot_id;
				retVal = NAM_ERR_NONE;
				syslog_ax_nam_eth_dbg("nam convert dev_port(%d,%d) to %d-%d OK\n",devNum,virtPortNo,*slotNo,*localPortNo);
			}
		}
		if(retVal != NAM_ERR_NONE)
		{
			syslog_ax_nam_eth_err("nam convert dev_port(%d,%d) to slot-port ERROR !\n",devNum,virtPortNo);
		}
		return retVal;
    }

	if(PRODUCT_ID_AX7K == productId) 
	{
		slot_max = NAM_AX7_MAX_SLOTNO; /* max slot number */
		port_count = NAM_AX7_MAX_PORT_PERSLOT; /* portCnt per slot*/
		dev_max = 2;/* maximum device count*/
		ge_port_max = 23;/* include port number 23*/
		port_max = 25;/* include port number 25*/
	}
	else if(PRODUCT_ID_AX7K_I == productId) 
	{
		if(BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)
		{
    		slot_max = NAM_AX7605i_MAX_SLOTNO; /* max slot number */
    		port_count = NAM_AX7605i_MAX_PORT_PERSLOT; /* portCnt per slot*/
    		dev_max = 1;/* maximum device count£¬temp to 1*/
    		ge_port_max = 27;/* include port number 23*/
    		port_max = 27;/* include port number 25*/			
		}
		else
		{
    		slot_max = NAM_AX7i_alpha_MAX_SLOTNO; /* max slot number */
    		port_count = NAM_AX7i_alpha_MAX_PORT_PERSLOT; /* portCnt per slot*/
    		dev_max = 1;/* maximum device count*/
    		ge_port_max = 23;/* include port number 23*/
    		port_max = 25;/* include port number 25*/
		}
	}
	else if(PRODUCT_ID_AX5K == productId) 
	{
		slot_max = NAM_AX5_MAX_SLOTNO; /* max slot number */
		port_count = NAM_AX5_MAX_PORT_PERSLOT; /* portCnt per slot*/
		dev_max = 1;/* maximum device count*/
		ge_port_max = 23;/* include port number 23*/
		port_max = 23;/* include port number 23*/
	}
	else if(PRODUCT_ID_AX5K_I == productId) 
	{
		slot_max = NAM_AX5I_MAX_SLOTNO; /* max slot number */
		port_count = NAM_AX5I_MAX_PORT_PERSLOT; /* portCnt per slot*/
		dev_max = 1;/* maximum device count*/
		ge_port_max = 7;/* include port number 7*/
		port_max = 25;/* include port number 25*/
	}
	else if(PRODUCT_ID_AX5K_E == productId || PRODUCT_ID_AX5608 == productId) 
	{
		slot_max = NAM_AX5E_MAX_SLOTNO; /* max slot number */
		port_count = NAM_AX5E_MAX_PORT_PERSLOT; /* portCnt per slot*/
		dev_max = 1;/* maximum device count*/
		ge_port_max = 7;/* include port number 23*/
		port_max = 7;/* include port number 23*/
	}
	else if(PRODUCT_ID_AU4K == productId) 
	{
		slot_max = NAM_AU4_MAX_SLOTNO; /* max slot number */
		port_count = NAM_AU4_MAX_PORT_PERSLOT; /* portCnt per slot*/
		dev_max = 1;/* maximum device count*/
		ge_port_max = 23; /* include port number 23*/
		port_max = 23; /* include port number 23*/
	}
	else if(PRODUCT_ID_AU3K == productId) 
	{
		slot_max = NAM_AU3_MAX_SLOTNO; /* max slot number */
		port_count = NAM_AU3_MAX_PORT_PERSLOT; /* portCnt per slot*/
		dev_max = 1;/* maximum device count*/
		ge_port_max = 23; /* include port number 23*/
		port_max = 23; /* include port number 23	*/	
	}
	else if(PRODUCT_ID_AU3K_BCM== productId) 
	{
		slot_max = NAM_AU3_BCM_MAX_SLOTNO; /* max slot number */
		port_count = NAM_AU3_BCM_MAX_PORT_PERSLOT; /* portCnt per slot*/
		dev_max = 2;/* maximum device count*/
		ge_port_max = 52; /* include port number 52 */
		port_max = 52; /* include port number 52		*/
	}
	else if((PRODUCT_ID_AU3K_BCAT == productId) || 
			 (PRODUCT_ID_AU2K_TCAT == productId)) 
	{
		slot_max = NAM_AU3528_MAX_SLOTNO; /* max slot number */
		port_count = NAM_AU3528_MAX_PORT_PERSLOT; /* portCnt per slot */
		dev_max = 1;/* maximum device count */
		ge_port_max = 28;/* include port number 23 */
		port_max = 28;/* include port number 27 */
	}
	
	if(devNum > dev_max) 
	{
		return NAM_ERR_DEVICE_NUMBER;
	}
	else if( virtPortNo > port_max) 
	{
		return NAM_ERR_PORT_ON_DEVICE;
	}
	else if(PRODUCT_ID_AU3K_BCM == productId) 
	{/* for product AU3052 speicial handle*/
		memset(&geport, 0, sizeof(gen_asic_port_info_s));
		for(i=0; i < slot_max; i++) 
		{
			for(j = 0; j < port_count; j++ ) 
			{
				geport = au3_3052_port_mapArr[i][j];
				if((devNum == geport.devNum)&& (virtPortNo == geport.portNum)) 
				{
					*slotNo = i + 1;
					*localPortNo = j + 1;
					found = 1;
				}
				/*code optimize:  Logically dead code
				zhangcl@autelan.com 2013-1-17*/
				if(found) 
					break;
			}
		}
	}
	else if((0 == devNum)&&(virtPortNo <= ge_port_max)) 
	{ /* for GE ports*/
		for(i=0; i < slot_max; i++) 
		{
			if(found) 
				break;
			for(j = 0; j < port_count; j++ ) 
			{
				if(found) 
					break;
				if(BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)
				{
					if((PRODUCT_ID_AX7K_I == productId) && (virtPortNo == ax7605i_port_mapArr[i][j]))
					{
					  	*slotNo = i;
    					*localPortNo = j+1;
    					found = 1;
						continue;
					}			
				}
				else
				{
    				if(((PRODUCT_ID_AX7K == productId) && (virtPortNo == ax7_6gtx_port_mapArr[i][j])) ||
    					((PRODUCT_ID_AX7K_I == productId) && (virtPortNo == ax7i_alpha_gtx_port_mapArr[i][j])) ||
    					((PRODUCT_ID_AX5K == productId) && (virtPortNo == ax5_5612_port_mapArr[i][j])) ||
    					((PRODUCT_ID_AX5K_I== productId) && (virtPortNo == ax5_5612i_port_mapArr[i][j])) ||
    					((PRODUCT_ID_AX5K_E== productId) && (virtPortNo == ax5_5612e_port_mapArr[i][j])) ||
    					((PRODUCT_ID_AX5608== productId) && (virtPortNo == ax5_5608e_port_mapArr[i][j])) ||
    					((PRODUCT_ID_AU4K == productId) && (virtPortNo == au4_4626_port_mapArr[i][j])) ||
    					((PRODUCT_ID_AU3K == productId) && (virtPortNo == au3_3524_port_mapArr[i][j])) ||
    					((PRODUCT_ID_AU3K_BCAT == productId) && (virtPortNo == au3_3528_port_mapArr[i][j])) ||
    					((PRODUCT_ID_AU2K_TCAT == productId) && (virtPortNo == au2_2528_port_mapArr[i][j]))
    					)
    				{
    					if(PRODUCT_ID_AU3K_BCAT == productId)
    				    {
    				        *slotNo = i+2;
    				    }				
    					else
    					{
    					    *slotNo = i+1;
    					}
    					*localPortNo = j+1;
    					found = 1;
    				}
				}
			}
		}
	}
	else if(PRODUCT_ID_AX7K == productId) { /*special treate for XG ports*/
		memset(&xgport, 0,sizeof(struct xg_port_info_s));
		for(i=0; i < NAM_AX7_MAX_SLOTNO; i++) 
		{
			for(j = 0; j < NAM_AX7_MAX_XG_PORT_PERSLOT; j++ ) 
			{
				xgport = ax7_xfp_port_mapArr[i][j];
				if((virtPortNo == xgport.portNum) && (devNum == xgport.devNum)) 
				{
						*slotNo = i + 1;
						*localPortNo = j + 1;
						found = 1;
				}
				/*code optimize:  Logically dead code
				zhangcl@autelan.com 2013-1-17*/
				if(found) 
					break;
			}
		}
	}
	else if(PRODUCT_ID_AX7K_I == productId) { /* XG port on 7605i */
		if(BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)
		{
			NULL;   /*zhangdi add for debug*/
		}
		else
		{
    		memset(&xgport, 0,sizeof(struct xg_port_info_s));
    		
    		for(i=0; i < NAM_AX7i_alpha_MAX_SLOTNO; i++) 
			{
    			for(j=0; j < NAM_AX7i_alpha_MAX_INTCONN_XPORT; j++) 
				{
    				xgport = ax7i_alpha_xg_conn_port_mapArr[i][j];
    				if((virtPortNo == xgport.portNum) && (devNum == xgport.devNum)) 
					{
    						*slotNo = 4;
    						*localPortNo = j + 1;
    						found = 1;
    				}
					/*code optimize:  Logically dead code
					zhangcl@autelan.com 2013-1-17*/
					if(found) 
						break;
    			}
    		}
		}
	}

	else if(PRODUCT_ID_AX5K_I == productId) 
	{ /*special treate for 5612i XG ports*/
		syslog_ax_nam_eth_dbg("get PRODUCT_ID_AX5K_I's XG ports\n");
		memset(&xgport, 0,sizeof(struct xg_port_info_s));
		for(i=0; i < NAM_AX5I_MAX_SLOTNO; i++) 
		{
			for(j = 0; j < NAM_AX5I_MAX_XG_PORT_PERSLOT; j++ ) 
			{
				xgport = ax5i_xfp_port_mapArr[i][j];
				if((virtPortNo == xgport.portNum) && (devNum == xgport.devNum)) 
				{
						*slotNo = 0;
						*localPortNo = j + 1;
						found = 1;
				}
				/*code optimize:  Logically dead code
				zhangcl@autelan.com 2013-1-17*/
				if(found) 
					break;
			}
		}
	}

	else 
	{
		/* currently unsupported types*/
	}

	if(found) 
		retVal = NAM_ERR_NONE;
		
	return retVal;
}
/*********************
* port Type
*
*
**********************/
int nam_set_ethport_type
(
	unsigned char devNum,
	unsigned char portNum,
	CPSS_DXCH_PORT_TYPE_E portType
)
{
	unsigned long ret = 0; 
		
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortTypeSet(devNum,portNum,portType);
	if(0 != ret) {
		return NPD_FAIL;
	}
#endif
	return NPD_SUCCESS;
}

/*nam_set_ethport_ipg: set port IPG(Inter-packet-Gap) 
 *	for GE port only
 */

/**********************************************************************
 *  nam_set_ethport_ipg
 *
 *  DESCRIPTION:
 *         Config ipg of GE port & XG port.
 *
 *  INPUT:   devNum - asic no
 *           portNum - prot no   
 *           value - the ipg of this port
 *  OUTPUT: none
 *  RETURN:
 *          0--OK  
 *         -1--ERROR
 *
 *  COMMENTS:
 *          zhangdi@autelan.com 2011-10-15
 **********************************************************************/ 
int nam_set_ethport_ipg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char  value
)
{
	unsigned long ret = 0;
	unsigned int ipgBase = 0;	
#ifdef DRV_LIB_CPSS

    /* modify to the new api, zhangdi@autelan.com 2011-10-15 */
	ret = cpssDxChPortIpgSet(devNum,portNum,value);
	if(0 != ret && 0x10 != ret) { /* GT_NOT_SUPPORTED (0x10) */
		syslog_ax_nam_eth_dbg("set port(%d,%d) ipg %d err %d.\n",devNum, portNum, value ,ret);
		return NPD_FAIL;
	}
    if(0x10 == ret)     /* GT_NOT_SUPPORTED (0x10) */
	{ 
		if(value == 8)
		{
            ipgBase = CPSS_DXCH_XG_PORT_IPG_BASE_BYTE_MIN_E;    /* CPSS_PORT_XG_FIXED_IPG_8_BYTES_E */
		}
		else if(value == 12)
		{
			ipgBase = CPSS_DXCH_XG_PORT_IPG_BASE_BYTE_MAX_E;    /* CPSS_PORT_XG_FIXED_IPG_8_BYTES_E */
		}
		else
		{
			return 0x10;
		}
		/* set XG port IPG*/
		ret = nam_set_xg_port_ipg(devNum,portNum,CPSS_DXCH_XG_PORT_IPG_FIXED_IPG_LAN_MODE_E,ipgBase);
    	if(0 != ret)
		{
    		syslog_ax_nam_eth_dbg("set XG port(%d,%d) ipg %d err %d.\n",devNum, portNum, value ,ret);
    		return NPD_FAIL;
    	}			
		return ret;
    }
#endif
	return NPD_SUCCESS;
}
#if 0
int nam_get_ethport_ipg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char  *portIPG
)
{
	unsigned long ret =0;
		
#ifdef DRV_LIB_CPSS
	if(0 == devNum){
#ifdef DRV_LIB_CPSS_3_4
#ifndef CPSS_CH2_E
#define CPSS_CH2_E 0x4
#endif
	if(!cpssDxChFamilyCheck(devNum, CPSS_CH2_E)){
		if(portNum <= 0x1B) {/*for 0-27 GE-Port*/
			syslog_ax_nam_eth_dbg("get port(%d %d) ipg\n",devNum, portNum);
			ret = cpssDxChPortIPGGet( devNum, portNum, portIPG);
			if(0 != ret) {
				syslog_ax_nam_eth_err("get port(%d %d) ipg error, ret %d\n",devNum, portNum, ret);
				return NPD_FAIL;
			}
		}
		else {
			syslog_ax_nam_eth_dbg(" NOT support Xg port.");
			return ret;
		}
	}
	else{
		if(portNum <= 0x17) {/*for 0-23 GE-Port on dev 275*/
			syslog_ax_nam_eth_dbg("get port(%d %d) ipg\n",devNum, portNum);
			ret = cpssDxChPortIPGGet( devNum, portNum, portIPG);
			if(0 != ret) {
				syslog_ax_nam_eth_err("get port(%d %d) ipg error, ret %d\n",devNum, portNum, ret);
				return NPD_FAIL;
			}
		}
		else {
			syslog_ax_nam_eth_dbg(" NOT support Xg port.");
			return ret;
		}
	}
#else
		if(portNum <= 0x17) {/*for 0-23 GE-Port on dev 275*/
			ret = cpssDxChPortIPGGet( devNum, portNum, portIPG);
			if(0 != ret) {
				return NPD_FAIL;
			}
		}
		else {
			syslog_ax_nam_eth_dbg(" NOT support Xg port.");
			return ret;
		}
#endif		
	}	
#endif
	return NPD_SUCCESS;
}
#else

/**********************************************************************
 *  nam_get_ethport_ipg
 *
 *  DESCRIPTION:
 *         Get ipg of GE port & XG port.
 *
 *  INPUT:   devNum - asic no
 *           portNum - prot no   
 *  OUTPUT:  portIPG - the ipg of this port
 *  RETURN:
 *          0--OK  
 *         -1--ERROR
 *
 *  COMMENTS:
 *          Called in npd_netlink.c  zhangdi@autelan.com 2012-04-09
 **********************************************************************/
int nam_get_ethport_ipg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned int *portIPG
)
{
	unsigned long ret = NPD_FAIL;
		
#ifdef DRV_LIB_CPSS
    /* modify to the new api */
	ret = cpssDxChPortIpgGet(devNum,portNum,portIPG);
	if(0 == ret) 
	{
		syslog_ax_nam_eth_dbg("get port(%d,%d) ipg %d OK.\n",devNum, portNum, *portIPG);
		return NPD_SUCCESS;
	}	
    else if(0x10 == ret)     /* GT_NOT_SUPPORTED XG (0x10) */
	{ 
		
		/* Get XG port IPG*/
		ret = cpssDxChPortIpgBaseGet(devNum,portNum,portIPG);
    	if(0 != ret)
		{
    		syslog_ax_nam_eth_dbg("get XG port(%d,%d) ipg err %d.\n",devNum, portNum, ret);
    		return NPD_FAIL;
    	}
        *portIPG = (*portIPG == 0) ? 8 :12;
		syslog_ax_nam_eth_dbg("get XG port(%d,%d) ipg %d OK.\n",devNum, portNum, *portIPG);	
		return NPD_SUCCESS;		
    }
	else
	{
		syslog_ax_nam_eth_dbg("get port(%d,%d) ipg err %d.\n",devNum, portNum, ret);
		return NPD_FAIL;		
	}
#endif
}
#endif

int nam_set_port_buff_mode
(
	unsigned char devNum,
    CPSS_DXCH_PORT_BUFFERS_MODE_ENT  bufferMode
)
{
	unsigned long ret = 0;
		
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortBuffersModeSet(devNum,bufferMode);
	if(0 != ret) {
		syslog_ax_nam_eth_dbg("cpssDxChPortBuffersModeSet:: dev %d,ret %d.",devNum,ret);
		return NPD_FAIL;
	}
#endif
	return NPD_SUCCESS;
}

/* set global buffer mode  */
unsigned int nam_set_port_global_buffer_mode(unsigned char enable)
{			
	unsigned char 	devIdx;
	unsigned int	ret = 0,appDemoDevAmount;
		
#ifdef DRV_LIB_CPSS
	appDemoDevAmount = nam_asic_get_instance_num(); 

	syslog_ax_nam_acl_dbg("param is %d\n",enable);
	
	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++)
	{							
		ret=nam_set_port_buff_mode(devIdx,enable);
		if(ret!=0)
			syslog_ax_nam_acl_err("IngressPolicyEnable dev %d ret %x Error!",devIdx,ret);
	}
#endif
    return ret;
}

/****************************
 *
 *	XG Port: Fix LAN IPG Mode;
 *			 extra IPG	= 0;		
 *			 IPGBase	= 8 byte
 ****************************/
int nam_set_xg_port_ipg
(
	unsigned char devNum,
	unsigned char portNum,
	CPSS_DXCH_NETWORK_XGPORT_IPG_MODE_ENT ipgMode,
	CPSS_DXCH_NETWORK_XGPORT_IPG_BASE_ENT ipgBase
)
{
	unsigned long ret = 0;
		
#ifdef DRV_LIB_CPSS
	ret = cpssDxChXGPortIPGModeSet(devNum,portNum,ipgMode);
	if(0 != ret) {
		syslog_ax_nam_eth_dbg("cpssDxChXGPortIPGModeSet:: dev %d,ret %d.",devNum,ret);
		return NPD_FAIL;
	}

	ret = cpssDxChPortIpgBaseSet(devNum,portNum,ipgBase);
	if(0 != ret) {
		syslog_ax_nam_eth_dbg("cpssDxChXGPortIPGBaseSet:: dev %d,ret %d.",devNum,ret);
		return NPD_FAIL;
	}
#endif
	return NPD_SUCCESS;
}

/* for access external PHY*/
int nam_set_xsmi_ac_timing
(
	unsigned char devNum
)
{
	unsigned long ret = 0;
		
#ifdef DRV_LIB_CPSS
	ret = cpssDxChXsmiAcTimingSet(devNum);
	if(0 != ret) {
		syslog_ax_nam_eth_dbg("cpssDxChXsmiAcTimingSet:: dev %d,ret %d.",devNum,ret);
		return NPD_FAIL;
	}
#endif
	return NPD_SUCCESS;
}

int nam_set_xgport_led_on_external_phy
(
	unsigned char devNum,
	unsigned char portNum
)
{
	unsigned long ret = 0;
		
#ifdef DRV_LIB_CPSS
	ret = cpssDxChXgPortExternalPhyLedIntfSet(devNum,portNum);
	if(0 != ret) {
		syslog_ax_nam_eth_dbg("set XG port(%d,%d) led on external phy ret %d\n",devNum,portNum,ret);
		return NPD_FAIL;
	}
#endif
	return NPD_SUCCESS;
}
/*********************
* port Enable :
*	Both for GE_Port &XG-Port
*
*********************/
int nam_set_ethport_enable
(
	unsigned char	devNum,
	unsigned char	portNum,
	unsigned long	portAttr
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS	
	unsigned long state = 0;
	ret = cpssDxChPortEnableSet(devNum,portNum,portAttr);
	if(0 != ret) {
		nam_syslog_err("set port(%d,%d) admin %s error %d!\n",  \
						devNum, portNum, portAttr ? "enable":"disable", ret);
		return NPD_FAIL;
	}
	/*portAttr is 0 set phy link down state,portAttr is 2 set phy link auto state*/
	state = portAttr ? 2 : 0; 
	ret = cpssDxChPhyPortLinkForceSet(devNum, portNum, state);
	if(0!=ret) {
		nam_syslog_err("set port(%d,%d) phy status(%d) error %d!\n", devNum, portNum, state, ret);
		return NPD_FAIL;
	}
	nam_syslog_dbg("set port(%d,%d) phy status(%d) success!\n", devNum, portNum, state);

	/* force link down MAC if admin state disabled, otherwise no force link down */
	state = portAttr ? 0 : 1;
	ret = cpssDxChPortForceLinkDownEnableSet(devNum, portNum, state);
	if(NPD_SUCCESS != ret) {
		nam_syslog_err("set port(%d,%d) %sforce link down error %d!\n", devNum, portNum, state ? "":"no ", ret);
		return NPD_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	ret = bcm_port_enable_set(devNum,portNum,portAttr);
	if(0 != ret) {
		return NPD_FAIL;
	}	
#endif	
	return NPD_SUCCESS;
}

/*********************
*
*
*
*********************/
unsigned int nam_get_port_en_dis
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long* portAttr
) 
{
	unsigned long	ret;
 	unsigned long   state;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortEnableGet(devNum, portNum, &state);
	/*NAM_INFO_DETAIL(("cpssDxChPortEnableGet ev %d port %d state %ld ret %#0lx\n",devNum,portNum,state,ret));	*/
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}

	if (1 == state) {
		*portAttr = ETH_ATTR_ENABLE ;
	}
	else {
		*portAttr = ETH_ATTR_DISABLE;
	}
#endif

#ifdef DRV_LIB_BCM
	ret = bcm_port_enable_get(devNum, portNum, &state);
	/*NAM_INFO_DETAIL(("cpssDxChPortEnableGet:dev %d port %d state %ld ret %#0lx\n",devNum,portNum,state,ret));*/
	
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}

	if (1 == state) {
		*portAttr = ETH_ATTR_ENABLE ;
	}
	else {
		*portAttr = ETH_ATTR_DISABLE;
	}
#endif
	return PORT_STATE_GET_SUCCESS;
}

/*********************
*port Link UP
*
*
*********************/
int nam_set_ethport_force_linkup
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
)
{
	unsigned long ret = NPD_SUCCESS;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortForceLinkPassEnableSet(devNum, portNum, state);
	if(0!=ret) {
		ret = NPD_FAIL;
	}
	ret = cpssDxChPortForceLinkDownEnableSet(devNum, portNum, 0);
	if(0!=ret) {
		ret = NPD_FAIL;
	}
	ret = cpssDxChPhyPortLinkForceSet(devNum, portNum, 1);
	if(0!=ret) {
		ret = NPD_FAIL;
	}
#endif
	return ret;	
}

int nam_set_ethport_force_linkdown
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
)
{
	unsigned long ret = NPD_SUCCESS;
	
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortForceLinkDownEnableSet(devNum, portNum, state);
	if(0!=ret) {
		ret = NPD_FAIL;
	}
	ret = cpssDxChPortForceLinkPassEnableSet(devNum, portNum, 0);
	if(0!=ret) {
		ret = NPD_FAIL;
	}	
	ret = cpssDxChPhyPortLinkForceSet(devNum, portNum, 0);
	if(0!=ret) {
		ret = NPD_FAIL;
	}
#endif
	return ret;	
}

int nam_set_ethport_force_auto
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
)
{
	unsigned long ret = NPD_SUCCESS;

#ifdef DRV_LIB_CPSS	
	ret = cpssDxChPortForceLinkDownEnableSet(devNum, portNum, state);
	if(0!=ret) {
		ret = NPD_FAIL;
	}
	ret = cpssDxChPortForceLinkPassEnableSet(devNum, portNum, state);
	if(0!=ret) {
		ret = NPD_FAIL;
	}
	ret = cpssDxChPhyPortLinkForceSet(devNum, portNum, 2);
	if(0!=ret) {
		ret = NPD_FAIL;
	}
#endif
	return ret;	
}


unsigned int nam_get_port_link_state
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long* portAttr
)
{
	unsigned long	ret;
	unsigned long  isLink;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortLinkStatusGet(devNum, portNum, &isLink);
	/*NAM_INFO_DETAIL(("cpssDxChPortLinkStatusGet:dev %d port %d link %ld ret %#0lx\n",devNum,portNum,isLink,ret));*/

	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}

	/* map to MACRO CONST*/
	if (1 == isLink) {
		*portAttr = ETH_ATTR_LINKUP  ;
	}
	else {
		*portAttr = ETH_ATTR_LINKDOWN ;
	}
#endif

#ifdef DRV_LIB_BCM
	ret = bcm_port_link_status_get(devNum, portNum, &isLink);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}

	/* map to MACRO CONST*/
	if (1 == isLink) {
		*portAttr = ETH_ATTR_LINKUP  ;
	}
	else {
		*portAttr = ETH_ATTR_LINKDOWN ;
	}
#endif	
	return PORT_STATE_GET_SUCCESS;
}

/******************************************************
*port AN: 
*	port auto-neg including: 
*		duplex-mode,speed & flow control
*Use:
*	Enable/Disable Auto-Neg Just on Duplex-Mode
*
*Attension:
*	Operation On XG-Port will return GT_NOT_SUPPORTED ,
*	when set tobe Enable.
*	XG-Port can not support Duplex-Mode Auto-Neg.
********************************************************/
/*
*Set Duplex-Mode AN Support or NOT
*/
int nam_set_ethport_duplex_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
)
{
	unsigned long ret = NPD_SUCCESS;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortDuplexAutoNegEnableSet(devNum, portNum,state);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
	/*printf("Duplex AN %s",state? "En":"Dis");*/
#endif

#ifdef DRV_LIB_BCM
	return ETHPORT_RETURN_CODE_NOT_SUPPORT;
#endif
	return NPD_SUCCESS;
}

/*
*Get Duplex-Mode AN Support or NOT
*/
int nam_get_ethport_duplex_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long *state
)
{
	unsigned long ret = NPD_SUCCESS;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortDuplexAutoNegEnableGet(devNum, portNum,state);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
	/*printf("Duplex AN %s",*state? "En":"Dis");*/
#endif
	return NPD_SUCCESS;
}

int nam_set_fc_autoneg_sa
(
    unsigned char devNum,
    unsigned char portNum,
    GT_ETHERADDR  *macPtr
)

{
   int ret;
#ifdef DRV_LIB_CPSS
   unsigned char macSaLsb;
   int i;

   macSaLsb = macPtr->arEther[5];
   
   ret = cpssDxChPortMacSaLsbSet(devNum,portNum,macSaLsb);
   syslog_ax_nam_eth_dbg("cpssDxChPortMacSaLsbSet::%#x\n",macSaLsb);
   ret = cpssDxChPortMacSaBaseSet (devNum,macPtr);
   for(i=5;i>=0;i--)
   {
      syslog_ax_nam_eth_dbg("cpssDxChPortMacSaBaseSet::%#x\n",macPtr->arEther[i]);
   }
 #endif
   return 0;
}

/******************************************************
*port AN: 
*	port auto-neg including: 
*		duplex-mode,speed & flow control
*Use:
*	Enable/Disable Auto-Neg Just on Flow-Control
*
*Attension:
*	Operation On XG-Port will return GT_NOT_SUPPORTED ,
*	when set tobe Enable.
*	XG-Port can not support Auto-Neg for FC.
********************************************************/
/*
*Set Flow_control AN Support or NOT
*/
int nam_set_ethport_fc_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state,
	unsigned long pauseAdvertise
)
{
	unsigned long ret = NPD_SUCCESS;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortFlowCntrlAutoNegEnableSet(devNum, portNum,state,pauseAdvertise);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	return ETHPORT_RETURN_CODE_NOT_SUPPORT;
#endif
	return NPD_SUCCESS;
}
/*
*Get Flow_control AN Support or NOT
*/
int nam_get_ethport_fc_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long *state
)
{
	unsigned long ret = NPD_SUCCESS;
	unsigned long pauseAdvertise = 0;

#ifdef DRV_LIB_CPSS
#ifdef DRV_LIB_CPSS_3_4
	ret = cpssDxChPortFlowCntrlAutoNegEnableGet(devNum, portNum,state, &pauseAdvertise);
#else
	ret = cpssDxChPortFlowCntrlAutoNegEnableGet(devNum, portNum,state);
#endif
#endif
	if(0 != ret) {
		ret = NPD_FAIL;
	}
	/*printf("FCtrol AN %s",*state? "En":"Dis");*/
	return NPD_SUCCESS;
}

/******************************************************
*port AN: 
*	port auto-neg including: 
*		duplex-mode,speed & flow control
*Use:
*	Enable/Disable Auto-Neg Just on Speed
*
*Attension:
*	Operation On XG-Port will return GT_NOT_SUPPORTED ,
*	when set tobe Enable.
*	XG-Port can not support Auto-Neg on Speed.
********************************************************/
/*
*Set Speed AN Support or NOT
*/
int nam_set_ethport_speed_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
)
{
	unsigned long ret = NPD_SUCCESS;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortSpeedAutoNegEnableSet(devNum, portNum, state);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
	/*printf("Speed AN %s",state? "En":"Dis");*/
#endif

#ifdef DRV_LIB_BCM
	return ETHPORT_RETURN_CODE_NOT_SUPPORT;
#endif
	return NPD_SUCCESS;
}

/*
*Get Speed AN Support or NOT
*/
int nam_get_ethport_speed_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long *state
)
{
	unsigned long ret = NPD_SUCCESS;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortSpeedAutoNegEnableGet(devNum, portNum, state);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
	/*printf("Speed AN %s",*state? "En":"Dis");*/
#endif
	return NPD_SUCCESS;
}


/*
* Set Port In-Band AutoNeg
*/
int nam_set_ethport_inband_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortInbandAutoNegEnableSet(devNum,portNum,state);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
	/*printf("In Band AN %s",*state? "En":"Dis");*/
#endif
	return NPD_SUCCESS;
}

/*
* Set port In-Band AutoNeg Mode
*/
int nam_set_ethport_inband_an_mode
(
	unsigned char devNum,
	unsigned char portNum,
	CPSS_DXCH_PORT_INBAND_AN_MODE anMode
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortInbandAutoNegModeSet(devNum,portNum,anMode);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
#endif	
	return NPD_SUCCESS;
}

unsigned int nam_set_bcm_ethport_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
)
{
	unsigned long ret = 0;
#ifdef DRV_LIB_BCM	
	ret = bcm_port_autoneg_set(devNum,portNum,state);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}	
#endif
	return PORT_STATE_GET_SUCCESS;
}

unsigned int nam_get_port_autoneg_state
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned long* portAttr
)
{
	unsigned long 	ret;
	unsigned long   anDone;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortAutoNegStatusGet(devNum, portNum,&anDone);
	/*NAM_INFO_DETAIL(("cpssDxChPortAutoNegStatusGet:dev %d port %d auto-Nego %ld ret %#0lx\n",devNum,portNum,anDone,ret));*/
	if (ret != 0) {

		return PORT_STATE_GET_FAIL;
	}

	/*map to MACRO CONST*/
	if(1 == anDone) {
		*portAttr = ETH_ATTR_AUTONEG_DONE  ;
	}
	else {
		*portAttr = ETH_ATTR_AUTONEG_NOT_DONE ;
	}
#endif

#ifdef DRV_LIB_BCM	
	ret = bcm_port_autoneg_get(devNum, portNum, &anDone);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}

	/*map to MACRO CONST*/
	if(1 == anDone) {
		*portAttr = ETH_ATTR_AUTONEG_DONE  ;
	}
	else {
		*portAttr = ETH_ATTR_AUTONEG_NOT_DONE ;
	}
#endif	
	return PORT_STATE_GET_SUCCESS;
}

/*********************************************************
*port Duplex
*	Both for GE-Port & XG-Port
*Atension:
*	XG-Port can only work On FULL.
*   Set Half duplex on XG-Port will return GT_NOT_SUPPORTED
**********************************************************/
int nam_set_ethport_duplex_mode
(
	unsigned char devNum, 
	unsigned char portNum, 
	PORT_DUPLEX_ENT dMode
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortDuplexModeSet(devNum, portNum, dMode);
	if(0!=ret){
		ret = NPD_FAIL;
	}
	ret = cpssDxChPhyPortDuplexSet( devNum, portNum, dMode);
	if(0!= ret) {
		ret = NPD_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	if(PORT_FULL_DUPLEX_E == dMode) {
		ret = bcm_port_duplex_set(devNum, portNum, 1);	
		if(0!=ret){
			ret = NPD_FAIL;
		}		
	}
	else if(PORT_HALF_DUPLEX_E == dMode) {
		ret = bcm_port_duplex_set(devNum, portNum, 0);	
		if(0!=ret){
			ret = NPD_FAIL;
		}
	}
#endif	
	return NPD_SUCCESS;
}
unsigned int nam_get_port_duplex_mode
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int *portAttr
)
{
	unsigned long		ret;
	PORT_DUPLEX_ENT 	duplexMode;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortDuplexModeGet(devNum, portNum, &duplexMode);
	/*NAM_INFO_DETAIL(("cpssDxChPortDuplexModeGet:dev %d port %d duplex %d ret %#0lx\n",devNum,portNum,duplexMode,ret));*/
	if (ret != 0) {

		return PORT_STATE_GET_FAIL;
	}

	/*map to MACRO CONST*/
	if (0 == duplexMode) { /*0:Duplex_Full, 1:Duplex_half*/
		*portAttr = ETH_ATTR_DUPLEX_FULL  ;
	}
	else {
		*portAttr = ETH_ATTR_DUPLEX_HALF;
	}
#endif

#ifdef DRV_LIB_BCM
	ret = bcm_port_duplex_get(devNum, portNum, &duplexMode);
	syslog_ax_nam_eth_dbg("get port(%d,%d) duplexMode %d \n",devNum,portNum,duplexMode);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	if (0 == duplexMode) { /*0:Duplex_half, 1:Duplex_full*/
		*portAttr = ETH_ATTR_DUPLEX_HALF;
	}
	else {
		*portAttr = ETH_ATTR_DUPLEX_FULL;
	}
#endif	
	return PORT_STATE_GET_SUCCESS;
}

/******************************
* Port Speed
*	Both for GE-Port & XG-Port
*
******************************/
int nam_set_ethport_speed
(
	unsigned char devNum, 
	unsigned char portNum, 
	PORT_SPEED_ENT speed
)
{
	unsigned long ret = NPD_SUCCESS;
	
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortSpeedSet( devNum, portNum, speed);
	if(0!= ret) {
		ret = NPD_FAIL;
	}
	ret = cpssDxChPhyPortSpeedSet( devNum, portNum, speed);
	if(0!= ret) {
		ret = NPD_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM

	if (PORT_SPEED_10_E == speed) {
		ret = bcm_port_speed_set(devNum, portNum, 10);	
		if(0!= ret) {
			ret = NPD_FAIL;
		}
	}
	else if (PORT_SPEED_100_E == speed) {
		ret = bcm_port_speed_set(devNum, portNum, 100);	
		if(0!= ret) {
			ret = NPD_FAIL;
		}
	}
	else if (PORT_SPEED_1000_E == speed) {
		ret = bcm_port_speed_set(devNum, portNum, 1000);	
		if(0!= ret) {
			ret = NPD_FAIL;
		}
	}	
#endif	
	return ret;
}
unsigned int nam_get_port_speed
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int* portAttr
)
{
	unsigned long	ret;
	PORT_SPEED_ENT	speed;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortSpeedGet(devNum, portNum, &speed);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*map to ENT */
	*portAttr = (unsigned int)speed;
#endif

#ifdef DRV_LIB_BCM
	unsigned int bcm_speed = 0;
	ret = bcm_port_speed_get(devNum, portNum, &bcm_speed);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*map to ENT */
	if(100 == bcm_speed){
		speed = PORT_SPEED_100_E;
	}
	else if(10 == bcm_speed){
		speed = PORT_SPEED_10_E;
	}
	else if(1000 == bcm_speed){
		speed = PORT_SPEED_1000_E;
	}
	*portAttr = (unsigned int)speed;
#endif	
	return PORT_STATE_GET_SUCCESS;
}

/********************************
*port Flow-Control
*	Both for GE-Port & XG-Port
*
*********************************/
int nam_set_ethport_flowCtrl
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long state
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortFlowControlEnableSet(devNum, portNum,state);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	unsigned int fc_Tx = 0,fc_Rx = 0;
	fc_Tx = state;
	fc_Rx = state;
	syslog_ax_nam_eth_dbg("port(%d,%d) fc_TxEn %d,fc_RxEn %d\n",devNum,portNum,fc_Tx,fc_Rx);
	ret = bcm_port_pause_set(devNum,portNum,fc_Tx,fc_Rx);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
#endif

	return NPD_SUCCESS;
}
int nam_set_ethport_PeriodFCtrl
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long enable
)
{
	unsigned long ret = 0;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortPeriodicFcEnableSet(devNum, portNum, enable);
	if(0!=ret) {
		ret = NPD_FAIL;
	}
#endif	
	return NPD_SUCCESS;
}

unsigned int nam_get_port_flowCtrl_state
(
	unsigned char devNum,
	unsigned char portNum, 
	unsigned long* portAttr
)
{
	unsigned long	ret = 0;

#ifdef DRV_LIB_CPSS
	unsigned long	fcEn = 0;

	ret = cpssDxChPortFlowControlEnableGet(devNum, portNum, &fcEn);
	/*NAM_INFO_DETAIL(("cpssDxChPortFlowControlEnableGet:dev %d port %d flowControl %ld ret %#0lx\n",devNum,portNum,fcEn,ret));*/
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*map to MACRO const*/
	if (1 == fcEn) {
		*portAttr = ETH_ATTR_FC_ENABLE;
	}
	else {
		*portAttr = ETH_ATTR_FC_DISABLE;
	}
#endif

#ifdef DRV_LIB_BCM
	unsigned int fc_TxEn = 0,fc_RxEn = 0;
	ret = bcm_port_pause_get(devNum,portNum,&fc_TxEn,&fc_RxEn);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
	syslog_ax_nam_eth_dbg("port(%d,%d) fc_TxEn %d,fc_RxEn %d\n",devNum,portNum,fc_TxEn,fc_RxEn);
	if(1 == fc_TxEn && 1 == fc_RxEn) {
		*portAttr = ETH_ATTR_FC_ENABLE;
	}
	else if(0 == fc_TxEn && 0 == fc_RxEn){
		*portAttr = ETH_ATTR_FC_DISABLE;
	}
#endif
	return PORT_STATE_GET_SUCCESS;
}

/*********************
*port BackPres
*
*
*********************/
int nam_set_ethport_backPres
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long state
)
{
	unsigned long ret = 0;
	unsigned int value = 0;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortBackPressureEnableSet(devNum, portNum, state);

	if(0!=ret) {
		ret = NPD_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	ret = READ_IPG_HD_BKP_CNTLr(devNum,portNum,&value);
	if(0 != ret) {
		syslog_ax_nam_eth_err("read port(%d %d) register error ret %d.\n",devNum,portNum,ret);
		ret = NPD_FAIL;
	}
	/*value |= ((state ? 1 : 0) << 0);*/
	/*(bit 0:HD_FC_ENA) When set, enables back-pressure in half-duplex mode.*/
	if(1 == state) {
		value |= 0x1;
	}
	else if(0 == state) {
		value &= 0x3E;
	}
	ret = WRITE_IPG_HD_BKP_CNTLr(devNum,portNum,value);
	if(0 != ret) {
		syslog_ax_nam_eth_err("write port(%d %d) register error ret %d.\n",devNum,portNum,ret);
		ret = NPD_FAIL;
	}
#endif
	return ret;
}
unsigned int nam_get_port_backPres_state
(
	unsigned char devNum,
	unsigned char portNum, 
	unsigned long* portAttr
)
{
	unsigned long	ret = 0;
	unsigned long	bpEn = 0;

#ifdef DRV_LIB_CPSS

	ret = cpssDxChPortBackPressureEnableGet(devNum, portNum, &bpEn);
	/*NAM_INFO_DETAIL(("cpssDxChPortBackPressureEnableGet:dev %d port %d backPressure %ld ret %#0lx\n",devNum,portNum,bpEn,ret));*/
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}

	/*map to MACRO const*/
	if (1 == bpEn) {
		*portAttr = ETH_ATTR_BP_ENABLE;
	}
	else {
		*portAttr = ETH_ATTR_BP_DISABLE;
	}
#endif

#ifdef DRV_LIB_BCM
	unsigned int value = 0 ;

	ret = READ_IPG_HD_BKP_CNTLr(devNum,portNum,&value);
	if(0 != ret) {
		syslog_ax_nam_eth_err("read port(%d %d) register error ret %d.\n",devNum,portNum,ret);
	}
	bpEn = (value & 1) ? 1 : 0;
	if (1 == bpEn) {
		*portAttr = ETH_ATTR_BP_ENABLE;
	}
	else {
		*portAttr = ETH_ATTR_BP_DISABLE;
	}
#endif
	return PORT_STATE_GET_SUCCESS;
}

/*********************
*
*
*
*********************/
int nam_set_ethport_mru
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned int mruSize
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortMruSet(devNum, portNum, mruSize);
	if(0!=ret){
		ret =NPD_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	ret = bcm_port_frame_max_set(devNum,portNum,mruSize);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
#endif
	return NPD_SUCCESS;
}
unsigned int nam_get_port_mru
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned int  *portMru
)
{
	unsigned long	ret = 0;
	unsigned int	mru;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortMruGet(devNum, portNum, &mru);

	/*NAM_INFO_DETAIL(("cpssDxChPortMruGet:dev %d port %d mru %d ret %ld\n",devNum,portNum,mru,ret));*/
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
#ifdef DRV_LIB_CPSS_3_4
	*portMru = (unsigned int)mru;
#else
	*portMru = (unsigned int)mru*2;
#endif

#endif

#ifdef DRV_LIB_BCM
	ret = bcm_port_frame_max_get(devNum, portNum, &mru);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
	*portMru = (unsigned int)mru;
#endif

	return PORT_STATE_GET_SUCCESS;
}

unsigned int nam_set_ethport_led_intf
(
	unsigned char devNum, 
	unsigned char portNum, 
	unsigned long enable
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortLedIntfSet(devNum,portNum,enable);
#endif
	return ret;
}
unsigned int nam_asic_port_pkt_statistic
(
	unsigned char devNum, 
	unsigned char portNum, 
	struct npd_port_counter *portPktCount,
	struct port_oct_s		*portOctCount
)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChBrgGEPortPktStatistic(devNum, portNum, portPktCount,portOctCount);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	unsigned long long value;

	/*Receive and Transmit 64 Byte Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsPkts64Octets,&value);
	portOctCount->b64oct = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive and Transmit 65 to 127 Byte Frame Counter*/	
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsPkts65to127Octets,&value);
	portOctCount->b64oct127 = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive and Transmit 128 to 255 Byte Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsPkts128to255Octets,&value);
	portOctCount->b128oct255 = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive and Transmit 256 to 511 Byte Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsPkts256to511Octets,&value);
	portOctCount->b256oct511 = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive and Transmit 512 to 1023 Byte Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsPkts512to1023Octets,&value);
	portOctCount->b512oct21023 = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive and Transmit 1024 to 1518 Byte Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsPkts1024to1518Octets,&value);
	portOctCount->b1024oct2max = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive and Transmit Oversized Frame Counter or Receive Jabber Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsOversizePkts,&value);
	portPktCount->rx.overSizepkt = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive Jabber Frame Counter*/	
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsJabbers,&value);
	portPktCount->rx.jabber = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Transmit Total Collision Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsCollisions,&value);
	portOctCount->collision = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive FCS Error Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsCRCAlignErrors,&value);
	portPktCount->rx.errorframe = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Transmit frame Counter except Transmit Total Collision Counter,Oversize Packet Counter,Fragment Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsTXNoErrors,&value);
	portPktCount->tx.goodbytesl = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive frame Counter except Receive FCS Error Frame Counter,Unsupported Opcode Frame Counter,Oversized Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsRXNoErrors,&value);
	portPktCount->rx.goodbytesl = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Transmit Late Collision Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpDot3StatsLateCollisions,&value);
	portOctCount->late_collision = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Transmit Excessive Collision Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpDot3StatsExcessiveCollisions,&value);
	portPktCount->tx.excessiveCollision = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive Fragment Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsFragments,&value);
	portPktCount->rx.fragments = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive Undersize Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsUndersizePkts,&value);
	portPktCount->rx.underSizepkt = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*eceive Unicast Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpIfHCInUcastPkts,&value);
	portPktCount->rx.uncastframes = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive Multicast Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpIfHCInMulticastPkts,&value);
	portPktCount->rx.mcastframes = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive Broadcast Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpIfHCInBroadcastPkts,&value);
	portPktCount->rx.bcastframes = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Transmit frame Counter except Transmit Multicast Frame Counter,Broadcast Frame Counter,Jabber Counter,FCS Error Counter,Control Frame Counter,Oversize Packet Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpIfHCOutUcastPkts,&value);
	portPktCount->tx.uncastframe = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Transmit Multicast Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpIfHCOutMulticastPkts,&value);
	portPktCount->tx.mcastframe = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Transmit Broadcast Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpIfHCOutBroadcastPckts,&value);
	portPktCount->tx.bcastframe = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}	
	/*Transmit Single Deferral Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpDot3StatsDeferredTransmissions,&value);
	portPktCount->tx.sent_deferred = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive Pause Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpDot3InPauseFrames,&value);
	portPktCount->rx.fcframe = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Transmit Pause Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpDot3OutPauseFrames,&value);
	portPktCount->rx.fcframe = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive Alignment Error Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpDot3StatsAlignmentErrors,&value);
	portPktCount->rx.fifooverruns = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}	
	/*Transmit Multiple Collision Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpDot3StatsMultipleCollisionFrames,&value);
	portPktCount->tx.sentMutiple = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*Receive Undersize Frame Counter,Fragment Counter,Oversized Frame Counter,Jabber Frame Counter,FCS Error Frame Counter*/
	ret = bcm_stat_get(devNum,portNum,snmpIfInErrors,&value);
	portPktCount->rx.badbytes = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	/*CRC errors + alignment errors*/
	ret = bcm_stat_get(devNum,portNum,snmpEtherStatsCRCAlignErrors,&value);
	portPktCount->tx.crcerror_fifooverrun = value;
	if(ret != 0) {
		return PORT_STATE_GET_FAIL;
	}	

#endif
	return PORT_STATE_GET_SUCCESS;

}
unsigned int nam_asic_xg_port_pkt_statistic
(   
    unsigned char devNum,
    unsigned char portNum,
	struct npd_port_counter *portPktCount,
	struct port_oct_s		*portOctCount

)
{
	unsigned long ret = 0;

#ifdef DRV_LIB_CPSS
#ifdef DRV_LIB_CPSS_3_4

	  ret = cpssDxChBrgXGPortPktStatistic(devNum,portNum,portPktCount,portOctCount);
	  if (ret != 0) {
		  return PORT_STATE_GET_FAIL;
	  }
	  
#endif
     
#endif
     return PORT_STATE_GET_SUCCESS;
}
unsigned int nam_asic_clear_port_pkt_stat
(
	unsigned char dev,
	unsigned char port
)
{
	int ret = 0;

#ifdef DRV_LIB_CPSS
	struct npd_port_counter portPktCount;
	struct port_oct_s		portOctCount;
	memset(&portPktCount,0,sizeof(struct npd_port_counter));
	memset(&portOctCount,0,sizeof(struct port_oct_s));
	ret = cpssDxChBrgGEPortPktStatistic(dev, port, &portPktCount,&portOctCount);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	ret = bcm_stat_clear(dev,port);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
#endif
	return PORT_STATE_GET_SUCCESS;
}
/* add this from aw1.3 */
unsigned int nam_asic_clear_xg_port_pkt_stat
(
	unsigned char dev,
	unsigned char port
)
{
	int ret = 0;

#ifdef DRV_LIB_CPSS
	struct npd_port_counter portPktCount;
	struct port_oct_s		portOctCount;
	memset(&portPktCount,0,sizeof(struct npd_port_counter));
	memset(&portOctCount,0,sizeof(struct port_oct_s));
	ret = cpssDxChBrgXGPortPktStatistic(dev, port, &portPktCount,&portOctCount);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
#endif

#ifdef DRV_LIB_BCM
	ret = bcm_stat_clear(dev,port);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
#endif
	return PORT_STATE_GET_SUCCESS;
}

/*******************************************************************************
* nam_set_eth_port_trans_media
*
* DESCRIPTION:
*      Set media auto-selection preferred media(or say media priority)
*	 irrelevant when port PHY type is not combo(or say Media Auto-Selection Interface)
*
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       media   - preferred auto-selection interface type (copper or fiber)
*			0 - none
*			1 - fiber media interface
*			2 - copper media interface
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*      0	- on success
*      1  	- on hardware error or parameters error 
*      6 	- for non-combo ports not support this operation
*
* COMMENTS:
*       None.
*
*******************************************************************************/
int nam_set_eth_port_trans_media
(
	unsigned char dev,
	unsigned char port,
	unsigned int  media
)
{

	int ret = 0,status = 0;

#ifdef DRV_LIB_CPSS
#ifdef DRV_LIB_CPSS_3_4

    if((BOARD_TYPE == BOARD_TYPE_AX81_AC12C)||(BOARD_TYPE == BOARD_TYPE_AX81_AC8C))
    {
        /* add for ac12c & ac12g12s */
        status = cpssDxChPhyPortSmiMediaSet(dev,port,media);
		if(0 != status) 
		{
		    syslog_ax_nam_eth_dbg("Set dev:%d port:%d page_media:%d OK \n", \
				dev,port,media);
		}
		ret = (0 == status) ? NAM_ERR_NONE : 	\
		  (0x10 == status) ? NAM_ERR_UNSUPPORT : NAM_ERR_GENERNAL;		
    }
	else if(BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)
	{
	    syslog_ax_nam_eth_dbg("Do not support media mode set on AX71_2X12G12S\n");
		ret = NAM_ERR_UNSUPPORT;
	}
	else if(BOARD_TYPE == BOARD_TYPE_AX81_1X12G12S)
	{
	    syslog_ax_nam_eth_dbg("Do not support media mode set on AX81_1X12G12S\n");
		ret = NAM_ERR_UNSUPPORT;
	}	
	else
#endif
	{
    	status = cpssDxChPhyPortSmiPreferredMediaSet(dev,port,media);
    	if(0 != status) {
    		syslog_ax_nam_eth_dbg("set port(%d,%d) media auto-select prefer media %s error %d", \
    				dev,port,(media == 0) ? "none":((media == 1) ?"fiber" : "copper"),status);
    	}

    	/*printf("set port(%d,%d) media auto-select prefer media %s error %d", 
    			dev,port,(media == 0) ? "none":((media == 1) ?"fiber" : "copper"),status);*/
    	/*
    	 * convert return value for NPD 
    	 *	0 - no error
    	 *	1 - general error such as device or port out of range and etc.
    	 *  	6 - operation not supported
    	 */
    	ret = (0 == status) ? NAM_ERR_NONE : 	\
    		  (0x10 == status) ? NAM_ERR_UNSUPPORT : NAM_ERR_GENERNAL;
	}
#endif

#ifdef DRV_LIB_BCM
	bcm_phy_config_t c_config,f_config;

	memset(&c_config,0,sizeof(bcm_phy_config_t));
	memset(&f_config,0,sizeof(bcm_phy_config_t));	

	/*only combo port can config media preferred*/
	if(1 == port) {
		if(1 == media) {
			ret = bcm_port_medium_config_get(dev,port,BCM_PORT_MEDIUM_COPPER,&c_config);
			c_config.preferred = 0;
			ret = bcm_port_medium_config_set(dev,port,BCM_PORT_MEDIUM_COPPER,&c_config);
			
			ret = bcm_port_medium_config_get(dev,port,BCM_PORT_MEDIUM_FIBER,&f_config);
			if(f_config.preferred == c_config.preferred) {
				syslog_ax_nam_eth_dbg("config error only one medium should be preferred\n");
			}
			syslog_ax_nam_eth_dbg("modify media %d preferred : %d\n",media,c_config.preferred);
			syslog_ax_nam_eth_dbg("modify media %d preferred : %d\n",media,f_config.preferred);
		}
		else if(2 == media) {
			ret = bcm_port_medium_config_get(dev,port,BCM_PORT_MEDIUM_COPPER,&c_config);
			c_config.preferred = 1;
			ret = bcm_port_medium_config_set(dev,port,BCM_PORT_MEDIUM_COPPER,&c_config);

			ret = bcm_port_medium_config_get(dev,port,BCM_PORT_MEDIUM_FIBER,&f_config);
			if(f_config.preferred == c_config.preferred) {
				syslog_ax_nam_eth_dbg("config error only one medium should be preferred\n");
			}
			syslog_ax_nam_eth_dbg("modify media %d preferred : %d\n",media,c_config.preferred);
			syslog_ax_nam_eth_dbg("modify media %d preferred : %d\n",media,f_config.preferred);
		}	
		else 
			ret = ETHPORT_RETURN_CODE_NOT_SUPPORT;
	}
	else {
		ret = NAM_ERR_UNSUPPORT;
	}
#endif
	return ret;
}

/*******************************************************************************
* nam_get_eth_port_trans_media
*
* DESCRIPTION:
*      Get media auto-selection preferred media(or say media priority)
*	 irrelevant when port PHY type is not combo(or say Media Auto-Selection Interface)
*
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       media   - preferred auto-selection interface type (copper or fiber)
*			0 - none
*			1 - fiber media interface
*			2 - copper media interface
*
* RETURNS:
*      0	- on success
*      1  	- on hardware error or parameters error 
*      6 	- for non-combo ports not support this operation
*
* COMMENTS:
*       None.
*
*******************************************************************************/
int nam_get_eth_port_trans_media
(
	unsigned char dev,
	unsigned char port,
	unsigned int  *media
)
{
	int ret = 0,status = 0;

#ifdef DRV_LIB_CPSS
	unsigned char mediaType = 0;

	status = cpssDxChPhyPortSmiPreferredMediaGet(dev,port,&mediaType);
	if(0 != status) {
		syslog_ax_nam_eth_dbg("get port(%d,%d) media auto-select prefer media error %d\n",dev,port,status);
	}

	*media = (1 == mediaType) ? 1 :		\
			  (2 == mediaType) ? 2 : 0;
	syslog_ax_nam_eth_dbg("get media value %d\n",*media);
	/*
	 * convert return value for NPD 
	 *	0 - no error
	 *	1 - general error such as device or port out of range and etc.
	 *  	6 - operation not supported
	 */
	ret = (0 == status) ? NAM_ERR_NONE : 	\
		  (0x10 == status) ? 6 : 1;
#endif

#ifdef DRV_LIB_BCM
	bcm_phy_config_t config;
	unsigned char mediaType = 2;

	memset(&config,0,sizeof(bcm_phy_config_t));

	if(1 == port) {
		ret = bcm_port_medium_config_get(dev,port,BCM_PORT_MEDIUM_FIBER,&config);
		if(0 != ret) {
			syslog_ax_nam_eth_dbg("get copper port(%d,%d) media error %d\n",dev,port,ret);
		}		
		syslog_ax_nam_eth_dbg("get copper media preferred %d\n",config.preferred);
		mediaType = config.preferred;
		#if 0
		ret = bcm_port_medium_config_get(dev,port,BCM_PORT_MEDIUM_FIBER,&f_config);
		if(0 != ret) {
			syslog_ax_nam_eth_dbg("get fiber port(%d,%d) media error %d\n",dev,port,ret);
		}
		syslog_ax_nam_eth_dbg("get fiber media %d preferred %d",medium,f_config.preferred);
		f_mediaType = f_config.preferred;	
		if(c_mediaType == f_mediaType) {
			syslog_ax_nam_eth_dbg("config error only one medium should be preferred\n");
		}
		#endif
	}
	*media = (1 == mediaType) ? 1 :		\
		  (0 == mediaType) ? 2 : 0;
	syslog_ax_nam_eth_dbg("get port mediaType %d media value %d\n",mediaType,*media);
#endif
	return ret;
}

/*********************
*	get eth port basic attr,including port ON-OFF,DISABLE-ENABLE,LINKUP-LINKDOWN
*
*
*
*********************/

int nam_read_eth_port_info
(
	unsigned int module_type, 
	unsigned char slotno, 
	unsigned char portno, 
	struct eth_port_s *ethport
) 
{
	unsigned int devNum = 0,virtPortNum = 0;
	unsigned int ret = NAM_ERR_NONE;
	unsigned long enDis = 0,isLink = 0,anDone = 0,fcEn = 0,backPsEn = 0,duplx_an = 0,speed_an = 0,fc_an = 0,ipg_vl = 1;
	unsigned int f_h_duplex = 0,currentSpeed = 0,portMru = 0,media = 0;
	unsigned int tmpBM = 0;
    unsigned char port_ipg = 0xc;
	unsigned int eth_g_index;
	struct eth_port_s portInfo;
	
	memset(&portInfo, 0,sizeof(struct eth_port_s));
   /* eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,portno);*/
   /*code optimize:  Dereference after null check
	zhangcl@autelan.com 2013-1-17*/
   if(NULL != ethport)
   {
		portInfo.port_type = ethport->port_type;
   }
   else
   {
   		return NAM_ERR_GENERNAL;
   }
	
	ret = nam_get_devport_by_slotport(slotno,portno,module_type,&devNum,&virtPortNum);
	
	if(NAM_ERR_NONE != ret) 
	{
		syslog_ax_nam_eth_err("get %d/%d device port err %s\n", \
						slotno,portno,nam_err_msg[ret]);
		return ret;
	}
	syslog_ax_nam_eth_err("get devNum %d virtPortNum %d device port.\n", devNum,virtPortNum);
	ret = npd_get_global_index_by_devport(devNum,virtPortNum,&eth_g_index);
    if(0 != ret)
	{
        return ret;
	}
	/* Admin status */
	ret = nam_get_port_en_dis (devNum,virtPortNum,&enDis);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) admin state fail ret %#0x.\n",devNum,virtPortNum,ret);
	}
	tmpBM |= ((enDis << ETH_ADMIN_STATUS_BIT) & ETH_ATTR_ADMIN_STATUS);

	/* Link status */
	ret = nam_get_port_link_state(devNum,virtPortNum,&isLink);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) link state fail ret %#0x.\n",devNum,virtPortNum,ret);
	}
	tmpBM |= ((isLink << ETH_LINK_STATUS_BIT) & ETH_ATTR_LINK_STATUS);

	/* Auto-Negotiation Status */
	ret = nam_get_port_autoneg_state(devNum,virtPortNum,&anDone);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) auto-nego state fail ret %#0x.\n",devNum,virtPortNum,ret);
	}
	tmpBM |= ((anDone << ETH_AUTONEG_BIT) & ETH_ATTR_AUTONEG);

	/*add by qily@autelan.com 2009-03-04*/
	if (MODULE_ID_AU3_3052 == module_type)
	{
		tmpBM |= ((anDone << ETH_AUTONEG_DUPLEX_BIT) & ETH_ATTR_AUTONEG_DUPLEX);
		tmpBM |= ((anDone << ETH_AUTONEG_FLOWCTRL_BIT) & ETH_ATTR_AUTONEG_FLOWCTRL);
		tmpBM |= ((anDone << ETH_AUTONEG_SPEED_BIT) & ETH_ATTR_AUTONEG_SPEED);
	}

	ret = nam_get_port_duplex_mode(devNum,virtPortNum,&f_h_duplex);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) duplex state fail ret %#0x.\n",devNum,virtPortNum,ret);
	}
	tmpBM |= ((f_h_duplex << ETH_DUPLEX_BIT ) & ETH_ATTR_DUPLEX);
	ret = nam_get_port_flowCtrl_state(devNum,virtPortNum,&fcEn);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) flow control state fail ret %#0x.\n",devNum,virtPortNum,ret);
	}
	if(portInfo.port_type == ETH_GE_SFP)
	{
		fcEn = ETH_ATTR_FC_ENABLE;
		tmpBM |= ((fcEn << ETH_FLOWCTRL_BIT) & ETH_ATTR_FLOWCTRL);
	}
	else
	{
		tmpBM |= ((fcEn << ETH_FLOWCTRL_BIT) & ETH_ATTR_FLOWCTRL);
	}
	if(MODULE_ID_AX7_XFP != module_type &&
		MODULE_ID_AX7I_XG_CONNECT != module_type && 
		MODULE_ID_SUBCARD_TYPE_A1 != module_type )/*add by wujh -- when module_type == AX7_XFP,get BP'll kill prestera module from kernal*/
	{	
		ret = nam_get_port_backPres_state(devNum,virtPortNum,&backPsEn);
		if(ret != 0) 
		{
			syslog_ax_nam_eth_err("get port(%d,%d) back pressure state fail ret %#0x.\n",devNum,virtPortNum,ret);
		}
		tmpBM |= ((backPsEn << ETH_BACKPRESSURE_BIT) & ETH_ATTR_BACKPRESSURE);
	}

	ret = nam_get_ethport_duplex_autoneg(devNum,virtPortNum, &duplx_an);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) duplex auto-nego state fail ret %#0x.\n",devNum,virtPortNum,ret);
	}
	if(portInfo.port_type == ETH_GE_SFP)
	{

	    duplx_an = ETH_ATTR_AUTONEG_DUPLEX_DISABLE; 
	    tmpBM |= ((duplx_an << ETH_AUTONEG_DUPLEX_BIT) & ETH_ATTR_AUTONEG_DUPLEX);
	}
	else 
	{
	    tmpBM |= ((duplx_an << ETH_AUTONEG_DUPLEX_BIT) & ETH_ATTR_AUTONEG_DUPLEX);
	}	
	ret = nam_get_ethport_fc_autoneg(devNum, virtPortNum, &fc_an);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) flow control auto-nego state fail ret %#0x.\n",devNum,virtPortNum,ret);
	}
    if(portInfo.port_type == ETH_GE_SFP)
	{

	    fc_an = ETH_ATTR_AUTONEG_FCON; 
	    tmpBM |= ((fc_an << ETH_AUTONEG_FLOWCTRL_BIT) & ETH_ATTR_AUTONEG_FLOWCTRL);
	}
	else 
	{
	    tmpBM |= ((fc_an << ETH_AUTONEG_FLOWCTRL_BIT) & ETH_ATTR_AUTONEG_FLOWCTRL);
	}
	ret = nam_get_ethport_speed_autoneg(devNum,virtPortNum, &speed_an);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) speed auto-nego state fail ret %#0x.\n",devNum,virtPortNum,ret);
	}
	if(portInfo.port_type == ETH_GE_SFP)
	{

	    speed_an = ETH_ATTR_AUTONEG_SPEEDDOWN; 
	    tmpBM |= ((speed_an << ETH_AUTONEG_SPEED_BIT) & ETH_ATTR_AUTONEG_SPEED);
	}
	else 
	{
	    tmpBM |= ((speed_an << ETH_AUTONEG_SPEED_BIT) & ETH_ATTR_AUTONEG_SPEED);
	}
	
	ret = nam_get_eth_port_trans_media(devNum,virtPortNum,&media);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) combo media fail ret %#0x.\n",devNum,virtPortNum,ret);
	}
	if(COMBO_PHY_MEDIA_PREFER_COPPER == media) 
	{
		tmpBM |= ((1 << ETH_PREFERRED_COPPER_MEDIA_BIT) & ETH_ATTR_PREFERRED_COPPER_MEDIA);
	}
	else if(COMBO_PHY_MEDIA_PREFER_FIBER == media) 
	{
		tmpBM |= ((1 << ETH_PREFERRED_FIBER_MEDIA_BIT) & ETH_ATTR_PREFERRED_FIBER_MEDIA);
	}
	
	ret = nam_get_port_speed(devNum, virtPortNum,&currentSpeed);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) speed fail ret %#0x.\n",devNum,virtPortNum,ret);
	}
	tmpBM |= ((currentSpeed << ETH_SPEED_BIT) & ETH_ATTR_SPEED_MASK);/*bit12~15 represent 16 kinds of speed*/

	/* reserve eth-port auto-negotiation configuration by qinhs@autelan.com 10/22/2008	*/
	tmpBM |= (ETH_ATTR_AUTONEG_CTRL & ethport->attr_bitmap);

	ethport->attr_bitmap = tmpBM;
	
	ret = nam_get_port_mru(devNum,virtPortNum, &portMru);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) mtu fail ret %#0x.\n",devNum,virtPortNum,ret);
	}
	ethport->mtu = portMru;

	ret = nam_get_ethport_ipg(devNum,virtPortNum,&port_ipg);
	if(ret != 0) 
	{
		syslog_ax_nam_eth_err("get port(%d,%d) ipg fail ret %d.\n",devNum,virtPortNum,ret);
	}
	ethport->ipg = port_ipg;	
	return NPD_SUCCESS;
}

unsigned int nam_asic_port_brg_bypass_en
(
	unsigned char devNum,
	unsigned char virPortNum,
	unsigned long state  
)
{
	unsigned long ret ;

#ifdef DRV_LIB_CPSS	
	/*code optimize:  Uninitialized scalar variable table type
	zhangcl@autelan.com 2013-1-17*/
	PRV_CPSS_DXCH_TABLE_ENT	tableType = PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E;
	
	unsigned int fieldWordNum = 0;
	unsigned int fieldOffset = 21;
	unsigned int fieldLength = 1;
	unsigned long fieldValuePtr = state;
	ret = prvCpssDxChWriteTableEntryField(devNum, \
										tableType, \
										virPortNum, \
										fieldWordNum,\
										fieldOffset,  \
										fieldLength,   \
										fieldValuePtr);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
	syslog_ax_nam_eth_dbg("port %d bridge bypass enable!\n",virPortNum);
#endif
	return NPD_SUCCESS;
}

int nam_port_phy_port_media_type
(	
	enum module_id_e	 moduleType,
	unsigned int slotno,
	unsigned int portno,
	enum eth_port_type_e 	*portMedia	
)
{
	unsigned int ret = 0;

#ifdef DRV_LIB_CPSS
	unsigned int devNum =0,portNum = 0;
	unsigned char portPhyType = 0xff;

	ret = nam_get_devport_by_slotport(slotno, portno,moduleType, &devNum, &portNum);
	if(NAM_ERR_NONE == ret ){
		/*get combo phy media type and set corresponding LED*/
		ret = cpssDxChPhyPortMediaTypeGet((unsigned char)devNum,(unsigned char)portNum,&portPhyType);
		if(0 !=ret ){
			syslog_ax_nam_eth_err("dev %d port %d PhyMediaType get fail.\n",devNum,portNum);
			return NAM_ERR_MAX;
		}
		else if( PHY_MEDIA_FIBBER == portPhyType){
			*portMedia = ETH_GE_SFP;
		}
		else if( PHY_MEDIA_COPPER == portPhyType){
			*portMedia = ETH_GTX;
		}
	}
#endif

#ifdef DRV_LIB_BCM
	unsigned int devNum =0,portNum = 0;
	int portPhyType = 0xff;

	ret = nam_get_devport_by_slotport(slotno, portno,moduleType, &devNum, &portNum);
	if(NAM_ERR_NONE == ret ){
		/*get combo phy media type and set corresponding LED*/
		ret = bcm_port_medium_get(devNum,portNum,&portPhyType);
		if(0 != ret ){
			syslog_ax_nam_eth_err("dev %d port %d PhyMediaType get fail.\n",devNum,portNum);
			return NAM_ERR_MAX;
		}
		else if(BCM_PORT_MEDIUM_FIBER == portPhyType){
			*portMedia = ETH_GE_SFP;
		}
		else if(BCM_PORT_MEDIUM_COPPER == portPhyType){
			*portMedia = ETH_GTX;
		}
		syslog_ax_nam_eth_dbg("get port(%d,%d) mediaType : %d\n",devNum,portNum,portPhyType);
	}	
#endif
	return NAM_ERR_NONE;
}
/*
*Set Phy Autoneg
*/
int nam_set_phy_ethport_autoneg
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long state
)
{
	unsigned long ret = NPD_SUCCESS;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPhyPortAutonegSet(devNum, portNum, state);
	syslog_ax_nam_eth_dbg("cpssDxChPhyPortAutonegSet::devNum:%d.portNum:%d.state:%d.\n",devNum, portNum, state);
	if(0 != ret) {
		ret = NPD_FAIL;
	}	
#endif
	return ret;
}

unsigned int nam_get_port_config_speed
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int* portAttr
)
{
#ifdef DRV_LIB_CPSS
	unsigned long	ret;
	PORT_SPEED_ENT	speed;

	ret = cpssDxChPortConfigSpeedGet(devNum, portNum, &speed);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	*portAttr = (unsigned int)speed;
#endif

#ifdef DRV_LIB_BCM
	unsigned int bcm_speed = 0;
	unsigned int value = 0;

	PORT_SPEED_ENT	speed;
	int ret = 0;
	ret = READ_COMMAND_CONFIGr(devNum,portNum,&value);
	if(0 != ret) {
		syslog_ax_nam_eth_err("read port(%d %d) register READ_COMMAND_CONFIG error ret %d.\n",devNum,portNum,ret);
		ret = NPD_FAIL;
	}
	bcm_speed = (value & 0x0000000c) >> 2;
	if(PORT_SPEED_100_E == bcm_speed){
		speed = PORT_SPEED_100_E;
	}
	else if(PORT_SPEED_10_E == bcm_speed){
		speed = PORT_SPEED_10_E;
	}
	else if(PORT_SPEED_1000_E == bcm_speed){
		speed = PORT_SPEED_1000_E;
	}
	*portAttr = (unsigned int)speed;		
#endif
	return PORT_STATE_GET_SUCCESS;
}

unsigned int nam_get_port_config_flowcontrol
(
	unsigned char devNum, 
	unsigned char portNum,
	unsigned int* portAttr
)
{
#ifdef DRV_LIB_CPSS
	unsigned long	ret;
	unsigned long	fcEn = 0;

	ret = cpssDxChPortConfigFlowControlGet(devNum, portNum, &fcEn);
	if (ret != 0) {
		return PORT_STATE_GET_FAIL;
	}
	
	if (1 == fcEn) {
		*portAttr = ETH_ATTR_FC_ENABLE;
	}
	else {
		*portAttr = ETH_ATTR_FC_DISABLE;
	}
#endif	

#ifdef DRV_LIB_BCM	
	unsigned int fc_TxEn = 0,fc_RxEn = 0;
	unsigned int value = 0;
	int ret = 0;
	ret = READ_MAC_MODEr(devNum,portNum,&value);
	if(0 != ret) {
		syslog_ax_nam_eth_err("read port(%d %d) register READ_MAC_MODE error ret %d.\n",devNum,portNum,ret);
		ret = NPD_FAIL;
	}
	fc_TxEn = (value & 0x10) >> 4;
	fc_RxEn = (value & 0x8) >> 3;
	/*syslog_ax_nam_eth_dbg("read from register 0port(%d %d) fc_TxEn %d,fc_RxEn %d value %0x\n",devNum,portNum,fc_TxEn,fc_RxEn,value);*/
	if(1 == fc_TxEn && 1 == fc_RxEn) {
		*portAttr = ETH_ATTR_FC_ENABLE;
	}
	else if(0 == fc_TxEn && 0 == fc_RxEn){
		*portAttr = ETH_ATTR_FC_DISABLE;
	}	
#endif
	return PORT_STATE_GET_SUCCESS;
}

/*
*Set Phy Reboot
*/
int nam_set_phy_reboot
(
	unsigned char devNum,
	unsigned char portNum
)
{
	unsigned long ret = NPD_SUCCESS;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPhyPortRebootSet(devNum, portNum);
	syslog_ax_nam_eth_dbg("phy reset asic port(%d,%d)\n",devNum, portNum);
	if(0 != ret) {
		ret = NPD_FAIL;
	}
#endif
	return ret;
}

/**********************************************************************************
 * nam_ethport_mask_link_interrupt_set
 * 
 * DESCRIPTION:
 *	This method set CPU port shaper value.	 
 *
 *
 *	INPUT:
 *		devNum - DXCH device number
 *		portNum - port number on the DXCH device
 *		enable - enable/disable flag(0 for disable interrupt and 1 for enable interrupt)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		status - return value from cpss driver layer.
 *		
 **********************************************************************************/
unsigned long nam_ethport_mask_link_interrupt_set
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned long enable
)
{
	unsigned long status = 0;

#ifdef DRV_LIB_CPSS
	status = cpssDxChPortLinkStatusIntMaskSet(devNum,portNum,enable);
#endif
	return status;
}

 int nam_asic_config_counter_drop_statistic
( 
	   unsigned int  reason
)
{
#ifdef DRV_LIB_CPSS
	unsigned char 	devIdx = 0;
	unsigned int	ret =0,appDemoDevAmount = 0;
	unsigned int    op_ret = 0,i = 0;
	unsigned int    rem = 0;
    unsigned int  counter = 0;
	
	appDemoDevAmount = nam_asic_get_instance_num(); 
	syslog_ax_nam_eth_dbg("nam_get_dropmode:");

	for(devIdx=0;devIdx < appDemoDevAmount;devIdx++) {		
        syslog_ax_nam_eth_dbg("The devNum is %d,reason %d,counter %d\n",devIdx,reason,counter);
        syslog_ax_nam_eth_dbg("The devNum is %d\n",devIdx);
    
		 /* call cpss api function */
		op_ret = cpssDxChBrgCntDropCntrModeSet(devIdx,reason);
	    if (op_ret !=  0) {
			return NPD_FAIL;
	    }
		rem = cpssDxChBrgCntDropCntrSet(devIdx, counter);
		if(rem!=0) {
           return NPD_FAIL; 
		} 	  
    }
    return 0;
	/*code optimize:  Structurally dead code
	zhangcl@autelan.com 2013-1-17*/
	#if 0
	op_ret=cpssDxChBrgCntDropCntrModeSet(devIdx,reason);
	if (op_ret !=  0) {
		return NPD_FAIL;
	}
	rem = cpssDxChBrgCntDropCntrSet(devIdx, counter);
	syslog_ax_nam_eth_dbg("The return value of  is %d\n",rem); 
	if(rem!=0) {
        return NPD_FAIL; 
    } 
		
        /* *counter=dropcnt;*/
	/* cpssDxChBrgCntDropCntrSet(devIdx, dropcnt);	 */ 
	#endif
#endif
	return 0;
}

 int nam_asic_counter_drop_statistic
( 
	unsigned int *drop_Mode,
	unsigned int *drop_Count
)
{

	unsigned char 	devIdx = 0;
	unsigned int	ret = 0,appDemoDevAmount;
	unsigned int    dropcnt =0;
	unsigned int    dropMode =0;

#ifdef DRV_LIB_CPSS
	appDemoDevAmount = nam_asic_get_instance_num(); 
	syslog_ax_nam_eth_dbg("nam_get_dropmode:\n");

	for(devIdx=0; devIdx < appDemoDevAmount; devIdx++) {			  
		/* call cpss api function */
		ret = cpssDxChBrgCntDropCntrModeGet(devIdx,&dropMode);
	    if (ret != 0) {
		    return PORT_STATE_GET_FAIL;
	    }
	    *drop_Mode = dropMode;				  
		syslog_ax_nam_eth_dbg("nam_get_dropmode:drop_Mode %d\n",*drop_Mode );	

	    ret = cpssDxChBrgCntDropCntrGet(devIdx,&dropcnt);
		syslog_ax_nam_eth_dbg("nam_get_dropmode:%d\n",*drop_Mode );
		
	    ret = cpssDxChBrgCntDropCntrGet(devIdx,&dropcnt);
		if(ret!=0) {
	        return NPD_FAIL; 
		} 
	    *drop_Count = dropcnt;
		syslog_ax_nam_eth_dbg("nam_get_dropcount:drop_Count %d\n",*drop_Count);
	}
#endif
  	return ret;		  
}

 int nam_asic_config_port_egress_counter
(
	unsigned int devNum,
	unsigned int portNum
)
{

	unsigned int ret = 0;

#ifdef DRV_LIB_CPSS
#define CntrSetNum 1 
    ret=cpssDxChPortEgressCntrModeSet(devNum,CntrSetNum,EGRESS_CNT_PORT_E,portNum,0,0,0);
    syslog_ax_nam_eth_dbg("The return value of Egress counter is %d\n",ret);
	if (ret !=  0) {
		return NPD_FAIL;
	}
#endif	 
    return ret;
}



 int nam_asic_config_vlan_port_egress_counter
(
	unsigned int devNum,
	unsigned int portNum,
	unsigned int vlanId
)
{
	unsigned int ret = 0;

#ifdef DRV_LIB_CPSS

    ret=cpssDxChPortEgressCntrModeSet(devNum,0,EGRESS_CNT_PORT_E|EGRESS_CNT_VLAN_E,portNum,vlanId,0,0);
    syslog_ax_nam_eth_dbg("The return value of port egress mode value is %d\n",ret);
	if (ret !=  0) {
		return NPD_FAIL;		 
	}
#endif
	return ret;

}

 int nam_asic_config_vlan_egress_counter
(
	unsigned int vlanId
)
{
	unsigned int ret=0;

#ifdef DRV_LIB_CPSS
	unsigned char devIdx=0;

	syslog_ax_nam_eth_dbg("nam_get_dropmode:");

	ret=cpssDxChPortEgressCntrModeSet(devIdx,0,EGRESS_CNT_VLAN_E,0,vlanId,0,0);
	syslog_ax_nam_eth_dbg("The return value of port egress count value is %d\n",ret);

	if (ret !=  0) {
		return NPD_FAIL;		 
	}
#endif
	return ret;
}

unsigned int nam_asic_show_egress_counter
(
    NAM_PORT_EGRESS_CNTR_STC	*egrCntrPtr
)
{  
	unsigned int	op_ret = 0;

#ifdef DRV_LIB_CPSS
	unsigned char	devIdx = 0;
#define COUNTER_SET0 0
#define COUNTER_SET1 1		  
	op_ret= cpssDxChPortEgressCntrsGet(devIdx,COUNTER_SET1,egrCntrPtr);
	syslog_ax_nam_eth_dbg("The show egress return value of is %d\n",op_ret);
	if (op_ret != 0) {
	    return PORT_STATE_GET_FAIL;
	}

#endif
	return op_ret;	
}
unsigned int nam_asic_show_ingress_counter
(
    BRIDGE_HOST_CNTR_STC *hostGroupCntPtr,
    GT_ETHERADDR *DmacAddr,
    GT_ETHERADDR *SmacAddr,
    unsigned int *matrixCntSaDaPktsPtr 
)
{  
	unsigned int  op_ret = 0;
	GT_ETHERADDR *smac = NULL;
	GT_ETHERADDR *dmac = NULL;
 

	smac = (GT_ETHERADDR*)malloc(sizeof(GT_ETHERADDR));
	dmac = (GT_ETHERADDR*)malloc(sizeof(GT_ETHERADDR));
	memset(smac,0,sizeof(GT_ETHERADDR));
	memset(dmac,0,sizeof(GT_ETHERADDR));

#ifdef DRV_LIB_CPSS
	unsigned int	ret;  
   	unsigned char	devIdx = 0;
	op_ret= cpssDxChBrgCntMacDaSaGet(devIdx,SmacAddr,DmacAddr);
	if (op_ret != 0) 
	{
		/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-17*/
		free(smac);
		free(dmac);
	    return HOST_DMAC_SMAC_GET_ERROR;
	}
	op_ret = cpssDxChBrgCntHostGroupCntrsGet(devIdx,hostGroupCntPtr);
	if (op_ret !=0)
	{
		/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-17*/
		free(smac);
		free(dmac);
        return HOST_STATE_GET_FAIL;
	}
	if((memcmp(smac->arEther,SmacAddr->arEther,6)!=0)&&((memcmp(dmac->arEther,DmacAddr->arEther,6) !=0)))
	{
        op_ret = cpssDxChBrgCntMatrixGroupCntrsGet(devIdx,matrixCntSaDaPktsPtr);
		if (op_ret !=0)
		{	
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(smac);
			free(dmac);
	        return HOST_STATE_GET_FAIL;
		}
	}
#endif
	return op_ret;	
}


unsigned int nam_ingress_host_counter_dmac_set(
    GT_ETHERADDR *DmacAddr
)

{
	unsigned char devIdx = 0;
	unsigned int ret = 0;
	GT_ETHERADDR SmacAddr;
	GT_ETHERADDR *smac = NULL;
	GT_ETHERADDR *dmac = NULL;

	
	smac = (GT_ETHERADDR*)malloc(sizeof(GT_ETHERADDR));
	dmac = (GT_ETHERADDR*)malloc(sizeof(GT_ETHERADDR));
	memset(smac,0,sizeof(GT_ETHERADDR));
	memset(dmac,0,sizeof(GT_ETHERADDR));
	memset(&SmacAddr,0,sizeof(GT_ETHERADDR));

  
#ifdef DRV_LIB_CPSS
    ret= cpssDxChBrgCntMacDaSaGet(devIdx,smac,dmac);
	if(0 != ret)
   	{
		syslog_ax_nam_eth_err("the return value host counter set%d ",ret);
		/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-17*/
		free(smac);
		free(dmac);
	   	return NAM_CONFIG_HOST_COUNTER_ERR;
   	}
    if(memcmp(smac->arEther,SmacAddr.arEther,6)==0)
	{
	   	ret = cpssDxChBrgCntMacDaSaSet(devIdx,&SmacAddr,DmacAddr);
	   	if (ret != 0) 
	   	{
			/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(smac);
			free(dmac);
			return NAM_CONFIG_HOST_COUNTER_ERR;
	   	}
   	}
   	else
	{
		ret = cpssDxChBrgCntMacDaSaSet(devIdx,smac,DmacAddr);/*save old config*/
	   	if (ret != 0) 
	   	{
	   		/*code optimize:  Resource leak
			zhangcl@autelan.com 2013-1-17*/
			free(smac);
			free(dmac);
	      	return NAM_CONFIG_HOST_COUNTER_ERR;
	   	}
   	}
#endif
    free(smac);
    free(dmac);
	return ret;
}
 
unsigned int nam_ingress_host_counter_smac_set(
    GT_ETHERADDR *SmacAddr
)

{
	unsigned char devIdx = 0;
	unsigned int ret = 0;
	GT_ETHERADDR DmacAddr;
	GT_ETHERADDR *smac = NULL;
	GT_ETHERADDR *dmac = NULL;

	smac = (GT_ETHERADDR*)malloc(sizeof(GT_ETHERADDR));
	dmac = (GT_ETHERADDR*)malloc(sizeof(GT_ETHERADDR));
	memset(smac,0,sizeof(GT_ETHERADDR));
	memset(dmac,0,sizeof(GT_ETHERADDR));	
	memset(&DmacAddr,0,sizeof(GT_ETHERADDR));
  
#ifdef DRV_LIB_CPSS
    ret= cpssDxChBrgCntMacDaSaGet(devIdx,smac,dmac);
   	if(0!= ret)
	{
   		syslog_ax_nam_eth_err("the return value host get %d ",ret);
	  	/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-17*/
		free(smac);
		free(dmac);
    	return NAM_CONFIG_HOST_COUNTER_ERR;
   	}
    if(memcmp(dmac->arEther,DmacAddr.arEther,6) ==0)
	{
		ret = cpssDxChBrgCntMacDaSaSet(devIdx,SmacAddr,&DmacAddr);
		if (ret != 0) 
		{
			free(smac);
			free(dmac);
		    return NAM_CONFIG_HOST_COUNTER_ERR;
		}
    }
	else
	{
		ret = cpssDxChBrgCntMacDaSaSet(devIdx,SmacAddr,dmac);
		if (ret != 0) 
		{	
			free(smac);
			free(dmac);
		    return NAM_CONFIG_HOST_COUNTER_ERR;
		}
	}
#endif
    free(smac);
    free(dmac);
	return ret;
}
unsigned int nam_ingress_host_counter_set(
	GT_ETHERADDR *DmacAddr,
    GT_ETHERADDR *SmacAddr
)

{
	unsigned char devNum = 0;
	unsigned int ret = 0;
	
  
#ifdef DRV_LIB_CPSS

	ret = cpssDxChBrgCntMacDaSaSet(devNum,SmacAddr,DmacAddr);
	if (ret != 0) {
	    return NAM_CONFIG_HOST_COUNTER_ERR;
	}
#endif
	return ret;
}
/* 
 * call cpss function cpssDxChBrgVlanLearningStateSet and cpssDxChBrgVlanNASecurEnable
 * if both return 0,then return 0; else return1
 *
 */
unsigned int nam_learning_state_set_na_secur_enable(
	unsigned char dev,
	unsigned short vid,
	unsigned int flag
)
{
#ifdef DRV_LIB_CPSS

    	if(0 != cpssDxChBrgVlanLearningStateSet(dev,vid,flag))
		    return 1;
		else if(0 != cpssDxChBrgVlanNASecurEnable(dev,vid,flag))
		    return 1;
#endif
		return 0;
}

/**********************************************************************************
 *  nam_set_promi_attr
 *
 *	DESCRIPTION:
 * 		set promiscuous port attribute.
 *
 *	INPUT:
 *		dev   - device NO.
 *		port  - port NO.
 *		enable - en/disable device some attribute
 *          vid  - the vlan id
 *          flag  - state
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *			NAM_OK
 *			NAM_ERR
 *
 **********************************************************************************/

int nam_set_promi_attr_on_pvid
(
	unsigned char dev,
	unsigned char port,
	unsigned int enable,
	unsigned short vid,
	unsigned int flag
)
{
#ifdef DRV_LIB_CPSS

	/*Enable/disable Ingress Filtering for specific port*/
	if(0 != cpssDxChBrgVlanPortIngFltEnable(dev,port,enable))
		return NAM_ERR;

	/*
	 *		 untag port in vlan
	 *  	 Sets state of VLAN based learning to specified VLAN on specified device.
	*/
	if(0 != cpssDxChBrgVlanLearningStateSet(dev,vid,flag))
		return NAM_ERR;

	/*
	*      This function enables/disables per VLAN the generation of
	*      security breach event for packets with unknown Source MAC addresses.
	*/
	if(0 != cpssDxChBrgVlanNASecurEnable(dev,vid,flag))
		return NAM_ERR;
#endif
    return NAM_OK;

}


int nam_set_promi_attr_on_all_vlan
(
	unsigned int product_id,
	unsigned int eth_g_index,
	unsigned char dev,
	unsigned int flag
)
{
#ifdef DRV_LIB_CPSS

	/*
	  * Disable all egress filter along the SPI path
	  */
	if(0 != cpssDxChBrgVlanEgressFilteringEnable(dev,flag))
		return NAM_ERR;
	if((PRODUCT_ID_AX5K == product_id)||(PRODUCT_ID_AX5K_I== product_id)){
		/*
		 * Disable ingress vlan check on SPI path 
		 */
		  if (0 != cpssDxChBrgGenVlanValidCheckEnable(PROMISCUOUS_AU5612_TARGET_DEV, flag))
				return NAM_ERR;
	}
	else if ((PRODUCT_ID_AX7K_I == product_id) ||
			  (PRODUCT_ID_AX7K == product_id)){
		if(0 != cpssDxChBrgVlanEgressFilteringEnable(PROMISCUOUS_TARGET_DEV_NUM,flag))
			return NAM_ERR;
		
		/*
		 * Disable ingress vlan check on SPI path 
		 */
		  if (0 != cpssDxChBrgGenVlanValidCheckEnable(PROMISCUOUS_TARGET_DEV_NUM, flag))
				return NAM_ERR;
	}
	else{
		nam_syslog_err(" %s: not support promis mode\n",__func__);
		return NAM_ERR;
	}

#endif  
	return NAM_OK;
		
}

int nam_egress_filter_enable
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned long enable
){
	int ret = 0;
#ifdef DRV_LIB_CPSS
	if(0 != (ret = cpssDxChBrgVlanEgressFilteringEnable(devNum,enable))) {
			return ret;
	}
	if(0 != (ret = cpssDxChBrgPortEgrFltUregMcastEnable(devNum,portNum,enable))) {
			return ret;
	}
#endif
	return 0;
}

/*******************************************************************
*FUNC:nam_eth_port_storm_control_bps_set, set port storm control base on bit per 
*         second
*PARAM:
*       unsigned int -eth_g_index, global index
*
    unsigned int - cntype,         storm control type
*       unsigned int - cnvalue,       storm control value
*       unsigned int - portType,    0 means that is one FE port,1 means that is one GE port
*RETURN:
*       NAM_OK--successful
*       NAM_ERR  -- failed
*********************************************************************/

int nam_eth_port_sc_pps_set
(
    unsigned int eth_g_index,
    unsigned int cntype,
    unsigned int cnvalue,
    unsigned int portType
)
{
   unsigned int rv = 0;
   unsigned char dev = 0,port = 0;
   rv = npd_get_devport_by_global_index(eth_g_index,&dev,&port);
   if(0 != rv){
       syslog_ax_nam_eth_err("parse eth_g_index(%#x) failed for ret %d\n",eth_g_index,rv);
	   return rv;
   }
   nam_syslog_dbg("nam storm control: eth_g_index(%#x),cntype %d,cnvalue %d\n",eth_g_index,cntype,cnvalue);
#ifdef DRV_LIB_BCM
   int ret = 0;
   int flags = 0,pps = 0,rvflg = 0;
   switch(cntype){
      case PORT_STORM_CONTROL_STREAM_DLF:
	  	flags = BCM_RATE_DLF;
		if(((cnvalue == 1488100)&&(PORT_GE_TYPE_E == portType))|| ((cnvalue == 148810)&&(PORT_FE_TYPE_E == portType))){
			syslog_ax_nam_eth_dbg("Disable rate limit for top limit value set!\n");
			flags = 0;
		}
        ret = bcm_rate_dlfbc_set(dev,cnvalue,flags,port);
		if(0 != ret){         
		   syslog_ax_nam_eth_err("Failed to set rate: g_port %#x flag %#x,value %d,ret %d\n",eth_g_index,flags,cnvalue,ret);
           return ret;
		}
		else{
           ret = bcm_rate_dlfbc_get(dev,&pps,&rvflg,port);
		   if(0==ret){
			   syslog_ax_nam_eth_dbg("Success to set rate: g_port %#x flag %#x,value %d,ret %d\n",eth_g_index,rvflg,pps,ret);
		   }
		   else{
               syslog_ax_nam_eth_err("Failed to get rate: g_port %#x flag %#x,value %d,ret %d\n",eth_g_index,flags,cnvalue,ret);
               return ret;
		   }
		}
	  break;
      case PORT_STORM_CONTROL_STREAM_BCAST:
	  	flags = BCM_RATE_BCAST; 
		if(((cnvalue == 1488100)&&(PORT_GE_TYPE_E == portType))|| ((cnvalue == 148810)&&(PORT_FE_TYPE_E == portType))){
			syslog_ax_nam_eth_dbg("Disable rate limit for top limit value set!\n");
			flags = 0;
		}		
        ret = bcm_rate_bcast_set(dev,cnvalue,flags,port);
		if(0 != ret){         
		   syslog_ax_nam_eth_err("Failed to set rate: g_port %#x flag %#x,value %d,ret %d\n",eth_g_index,flags,cnvalue,ret);
           return ret;
		}
		else{
           ret = bcm_rate_bcast_get(dev,&pps,&rvflg,port);
		   if(0==ret){
			   syslog_ax_nam_eth_dbg("Success to set rate: g_port %#x flag %#x,value %d,ret %d\n",eth_g_index,rvflg,pps,ret);
		   }
		   else{
               syslog_ax_nam_eth_err("Failed to get rate: g_port %#x flag %#x,value %d,ret %d\n",eth_g_index,flags,cnvalue,ret);
               return ret;
		   }
		}		
	  break;
	  case PORT_STORM_CONTROL_STREAM_MCAST:
	  	flags = BCM_RATE_MCAST;	  
		if(((cnvalue == 1488100)&&(PORT_GE_TYPE_E == portType))|| ((cnvalue == 148810)&&(PORT_FE_TYPE_E == portType))){
			syslog_ax_nam_eth_dbg("Disable rate limit for top limit value set!\n");
			flags = 0;
		}		
        ret = bcm_rate_mcast_set(dev,cnvalue,flags,port);
		if(0 != ret){         
		   syslog_ax_nam_eth_err("Failed to set rate: g_port %#x flag %#x,value %d,ret %d\n",eth_g_index,flags,cnvalue,ret);
           return ret;
		}
		else{
           ret = bcm_rate_mcast_get(dev,&pps,&rvflg,port);
		   if(0 == ret){
			   syslog_ax_nam_eth_dbg("Success to set rate: g_port %#x flag %#x,value %d,ret %d\n",eth_g_index,rvflg,pps,ret);
		   }
		   else{
               syslog_ax_nam_eth_err("Failed to get rate: g_port %#x flag %#x,value %d,ret %d\n",eth_g_index,flags,cnvalue,ret);
               return ret;
		   }
		}	
	  break;
	  default:
		syslog_ax_nam_eth_err("Bad value input!\n");
	  break;
   }
   return NAM_OK;
#endif

#ifdef DRV_LIB_CPSS
   unsigned int ret = 0;
   unsigned int portSpeed = 0,unit = 0;
   NAM_BRG_RATE_LIMIT_PORT_T rateCfg,getCfg;
   memset(&rateCfg,0,sizeof(NAM_BRG_RATE_LIMIT_PORT_T));
   memset(&getCfg,0,sizeof(NAM_BRG_RATE_LIMIT_PORT_T));


   ret = cpssDxChBrgGenPortRateLimitGet(dev,port,&getCfg);
   if(NAM_OK != ret){
      syslog_ax_nam_eth_err("get port %d rate limit failed,ret %d\n",port,ret);
	  return NAM_ERR;
   }
   else{
	   syslog_ax_nam_eth_err("get port %d rate limit %d,multicast %s,dlf %s,broadcast %s\n",port,getCfg.rateLimit,\
		  getCfg.enableMc ? "enable":"disable",getCfg.enableUcUnk ? "enable":"disable",getCfg.enableBc ? "enable":"disable");

   }
   memcpy(&rateCfg,&getCfg,sizeof(NAM_BRG_RATE_LIMIT_PORT_T));
   ret = nam_get_port_speed(dev,port,&portSpeed);
   if(PORT_STATE_GET_SUCCESS == ret){
   	   if(PORT_SPEED_10000_E == portSpeed){
	   	   unit= 1000*64;/*granularity is 25.6 for 10Gbps*/
	   }
       else if((PORT_SPEED_1000_E == portSpeed)||((0 == portSpeed)&&(PORT_GE_TYPE_E == portType))){
	   	   unit = 100*64;		   
	    }
	   else if((PORT_SPEED_100_E == portSpeed)||((0 == portSpeed)&&(PORT_FE_TYPE_E == portType))){
	   	   unit =  10*64;
	   }
	   else if(PORT_SPEED_10_E == portSpeed){
		   unit =  1*64;
	   } 
	   syslog_ax_nam_eth_dbg("unit is %d\n",unit);
	   rateCfg.rateLimit =	(cnvalue+(unit -1))/unit;
   }
   else{
       return NAM_ERR;
   }
   switch(cntype){
		 case PORT_STORM_CONTROL_STREAM_DLF:
		   rateCfg.enableUcUnk = 1;
		   if(((cnvalue == ETH_PORT_SPEED_1000_PPS)&&((PORT_SPEED_1000_E == portSpeed)||((0 == portSpeed)&&(PORT_GE_TYPE_E == portType))))|| \
		   	   ((cnvalue == ETH_PORT_SPEED_100_PPS)&&((PORT_SPEED_100_E == portSpeed)||((0 == portSpeed)&&(PORT_FE_TYPE_E == portType))))){
			   syslog_ax_nam_eth_dbg("Disable rate limit for top limit value set!\n");
			   rateCfg.enableUcUnk = 0;
			   if((65535 != getCfg.rateLimit )&&((getCfg.enableMc)||(getCfg.enableBc)))
			   	   rateCfg.rateLimit = getCfg.rateLimit;
			   else
			       rateCfg.rateLimit = 65535;
		   }
		   break;
		 case PORT_STORM_CONTROL_STREAM_BCAST:
		   rateCfg.enableBc = 1;
		   if(((cnvalue == ETH_PORT_SPEED_1000_PPS)&&((PORT_SPEED_1000_E == portSpeed)||((0 == portSpeed)&&(PORT_GE_TYPE_E == portType))))|| \
		   	   ((cnvalue == ETH_PORT_SPEED_100_PPS)&&((PORT_SPEED_100_E == portSpeed)||((0 == portSpeed)&&(PORT_FE_TYPE_E == portType))))){
			   syslog_ax_nam_eth_dbg("Disable rate limit for top limit value set!\n");
			   rateCfg.enableBc = 0;
			   if((65535 != getCfg.rateLimit )&&((getCfg.enableMc)||(getCfg.enableUcUnk)))
			   	   rateCfg.rateLimit = getCfg.rateLimit;
			   else
			       rateCfg.rateLimit = 65535;
		   }	      
		   break;
		 case PORT_STORM_CONTROL_STREAM_MCAST:
		   rateCfg.enableMc = 1;
		   if(((cnvalue == ETH_PORT_SPEED_1000_PPS)&&((PORT_SPEED_1000_E == portSpeed)||((0 == portSpeed)&&(PORT_GE_TYPE_E == portType))))|| \
		   	   ((cnvalue == ETH_PORT_SPEED_100_PPS)&&((PORT_SPEED_100_E == portSpeed)||((0 == portSpeed)&&(PORT_FE_TYPE_E == portType))))){
			   syslog_ax_nam_eth_dbg("Disable rate limit for top limit value set!\n");
			   rateCfg.enableMc = 0;
			   if((65535 != getCfg.rateLimit )&&((getCfg.enableBc)||(getCfg.enableUcUnk)))
			   	   rateCfg.rateLimit = getCfg.rateLimit;
			   else
			       rateCfg.rateLimit = 65535;
		   }	      
		   break;
		 default:
		   syslog_ax_nam_eth_err("Bad value input!\n");
		   break;
	}

   ret = cpssDxChBrgGenPortRateLimitSet(dev,port,&rateCfg);
   if(NAM_OK != ret){
      syslog_ax_nam_eth_err("set port %d rate limit failed,ret %d\n",port,ret);
	  return NAM_ERR;
   }
   else{
   	  cpssDxChBrgGenPortRateLimitGet(dev,port,&getCfg);
      syslog_ax_nam_eth_dbg("set port %d rate limit success,enableMc %d,enabledlf %d,enablebc %d,limit %d\n",port,rateCfg.enableMc,rateCfg.enableUcUnk,rateCfg.enableBc,getCfg.rateLimit);
   }
   return NAM_OK;
#endif
}
/*******************************************************************
*FUNC:nam_eth_port_storm_control_bps_set, set port storm control base on bit per 
*         second
*PARAM:
*       unsigned int -eth_g_index, global index
*
    unsigned int - cntype,         storm control type
*       unsigned int - cnvalue,       storm control value
*       unsigned int - portType,    0 means that is one FE port,1 means that is one GE port
*RETURN:
*       NAM_OK--successful
*       NAM_ERR  -- failed
*********************************************************************/

int nam_eth_port_sc_bps_set
(
    unsigned int eth_g_index,
    unsigned int cntype,
    unsigned int cnvalue,
    unsigned int portType
)
{
   unsigned int rv = 0;
   unsigned char dev = 0,port = 0;
   rv = npd_get_devport_by_global_index(eth_g_index,&dev,&port);
   if(0 != rv){
       syslog_ax_nam_eth_err("parse eth_g_index(%#x) failed for ret %d\n",eth_g_index,rv);
	   return rv;
   }
   nam_syslog_dbg("nam storm control: eth_g_index(%#x),asic port(%d,%d),storm control type %d,value %d\n",eth_g_index,dev,port,cntype,cnvalue);
#ifdef DRV_LIB_BCM
   int ret = 0;
   int flags = 0,bps = 0,rvbps = 0,rvburst = 0;
   /*cnvalue is base on bytes,convert to kbit mode*/
   bps = ((cnvalue*8)+(1024-1))/(1024);
   switch(cntype){
      case PORT_STORM_CONTROL_STREAM_DLF:
	  	flags = BCM_RATE_DLF;
		break;
      case PORT_STORM_CONTROL_STREAM_BCAST:
	  	flags = BCM_RATE_BCAST; 
		break;
	  case PORT_STORM_CONTROL_STREAM_MCAST:
	  	flags = BCM_RATE_MCAST;
		break;
   }
	if(((cnvalue == 100000000) && (PORT_FE_TYPE_E == portType) ) || ((cnvalue == 1000000000) && (PORT_GE_TYPE_E == portType) )){
		syslog_ax_nam_eth_dbg("Disable rate limit for top limit value set!\n");
		flags |=  BCM_RATE_FULL;
	}
    ret = bcm_rate_bandwidth_set(dev,port,flags,bps, ((16 * MBYTES * BITSINBYTE) / KBITS_IN_API));
	if(0 != ret){         
	   syslog_ax_nam_eth_err("Failed to set rate: g_port %#x flag %#x,value %d,ret %d\n",eth_g_index,flags,cnvalue,ret);
       return NAM_ERR;
	}
	else{
       ret = bcm_rate_bandwidth_get(dev,port,flags,&rvbps,&rvburst);
	   if(0 == ret){
		   syslog_ax_nam_eth_dbg("Success to set rate: g_port %#x flag %#x, bps %d,ret %d\n",eth_g_index,flags,rvbps,ret);
	   }
	   else{
           syslog_ax_nam_eth_err("Failed to get rate: g_port %#x flag %#x,bps %d,ret %d\n",eth_g_index,flags,rvbps,ret);
           return NAM_ERR;
	   }
	}   
   return NAM_OK;
#endif

#ifdef DRV_LIB_CPSS
   unsigned int ret = 0;
   unsigned int portSpeed = 0,unit = 0;
   NAM_BRG_RATE_LIMIT_PORT_T rateCfg,getCfg;
   memset(&rateCfg,0,sizeof(NAM_BRG_RATE_LIMIT_PORT_T));
   memset(&getCfg,0,sizeof(NAM_BRG_RATE_LIMIT_PORT_T));
   ret = cpssDxChBrgGenPortRateLimitGet(dev,port,&getCfg);
   if(NAM_OK != ret){
      syslog_ax_nam_eth_err("get port %d rate limit failed,ret %d\n",port,ret);
	  return NAM_ERR;
   }  
   else{
      syslog_ax_nam_eth_err("get port %d rate limit %d,multicast %s,dlf %s,broadcast %s\n",port,getCfg.rateLimit,\
	  	 getCfg.enableMc ? "enable":"disable",getCfg.enableUcUnk ? "enable":"disable",getCfg.enableBc ? "enable":"disable");
   }
   memcpy(&rateCfg,&getCfg,sizeof(NAM_BRG_RATE_LIMIT_PORT_T));  
   ret = nam_get_port_speed(dev,port,&portSpeed);
   if(PORT_STATE_GET_SUCCESS == ret){
   	   if(PORT_SPEED_10000_E == portSpeed){
	   	   unit = 1000*64;
	   }
       else if((PORT_SPEED_1000_E == portSpeed)||((0 == portSpeed)&&(PORT_GE_TYPE_E == portType))){
		   unit = 100*64;			   
	   }
	   else if((PORT_SPEED_100_E == portSpeed)||((0 == portSpeed)&&(PORT_FE_TYPE_E == portType))){
	       unit =10*64;
	   }
	   else if(PORT_SPEED_10_E == portSpeed){
	       unit = 64;
	   }   
	   rateCfg.rateLimit = (cnvalue + (unit - 1))/unit;
	   syslog_ax_nam_eth_dbg("unit is %d,set value %d\n",unit,rateCfg.rateLimit);
   }   
   switch(cntype){
   	
		 case PORT_STORM_CONTROL_STREAM_DLF:
		   rateCfg.enableUcUnk = 1;
		   if(((cnvalue == ETH_PORT_SPEED_1000_BPS)&&((PORT_SPEED_1000_E == portSpeed)||((0 == portSpeed)&&(PORT_GE_TYPE_E == portType))))|| \
		   	   ((cnvalue == ETH_PORT_SPEED_100_BPS)&&((PORT_SPEED_100_E == portSpeed)||((0 == portSpeed)&&(PORT_FE_TYPE_E == portType))))){
			   syslog_ax_nam_eth_dbg("Disable rate limit for top limit value set!\n");
			   rateCfg.enableUcUnk = 0;
			   if((65535 != getCfg.rateLimit )&&((getCfg.enableMc)||(getCfg.enableBc)))
			   	   rateCfg.rateLimit = getCfg.rateLimit;
			   else
			       rateCfg.rateLimit = 65535;
		   }
		   break;
		 case PORT_STORM_CONTROL_STREAM_BCAST:
		   rateCfg.enableBc = 1;
		   if(((cnvalue == ETH_PORT_SPEED_1000_BPS)&&((PORT_SPEED_1000_E == portSpeed)||((0 == portSpeed)&&(PORT_GE_TYPE_E == portType))))|| \
		   	   ((cnvalue == ETH_PORT_SPEED_100_BPS)&&((PORT_SPEED_100_E == portSpeed)||((0 == portSpeed)&&(PORT_FE_TYPE_E == portType))))){
			   syslog_ax_nam_eth_dbg("Disable rate limit for top limit value set!\n");
			   rateCfg.enableBc = 0;
			   if((65535 != getCfg.rateLimit )&&((getCfg.enableMc)||(getCfg.enableUcUnk)))
			   	   rateCfg.rateLimit = getCfg.rateLimit;
			   else
			       rateCfg.rateLimit = 65535;
		   }	      
		   break;
		 case PORT_STORM_CONTROL_STREAM_MCAST:
		   rateCfg.enableMc = 1;
		   if(((cnvalue == ETH_PORT_SPEED_1000_BPS)&&((PORT_SPEED_1000_E == portSpeed)||((0 == portSpeed)&&(PORT_GE_TYPE_E == portType))))|| \
		   	   ((cnvalue == ETH_PORT_SPEED_100_BPS)&&((PORT_SPEED_100_E == portSpeed)||((0 == portSpeed)&&(PORT_FE_TYPE_E == portType))))){
			   syslog_ax_nam_eth_dbg("Disable rate limit for top limit value set!\n");
			   rateCfg.enableMc = 0;
			   if((65535 != getCfg.rateLimit )&&((getCfg.enableUcUnk)||(getCfg.enableBc)))
			   	   rateCfg.rateLimit = getCfg.rateLimit;
			   else
			       rateCfg.rateLimit = 65535;
		   }	      
		   break;
		 default:
		   syslog_ax_nam_eth_err("Bad value input!\n");
		 break;
	}

   ret = cpssDxChBrgGenPortRateLimitSet(dev,port,&rateCfg);
   if(NAM_OK != ret){
      syslog_ax_nam_eth_err("set port %d rate limit failed,ret %d\n",port,ret);
	  return NAM_ERR;
   }
   else{
   	  cpssDxChBrgGenPortRateLimitGet(dev,port,&getCfg);
      syslog_ax_nam_eth_dbg("set port %d rate limit success,enableMc %d,enabledlf %d,enablebc %d,limit %d\n",port,rateCfg.enableMc,rateCfg.enableUcUnk,rateCfg.enableBc,getCfg.rateLimit);
   }
   return NAM_OK;

#endif
}

unsigned int nam_eth_port_sc_global_set
(
   unsigned char modeType  
)
{
   unsigned int ret = 0,granularity = 0;
   unsigned long i =0,numofPp = nam_asic_get_instance_num( );
   NAM_BRG_RATE_LIMIT_T ratecfg,defaultcfg;
   memset(&ratecfg,0,sizeof(NAM_BRG_RATE_LIMIT_T));

#ifdef DRV_LIB_CPSS
   for( i = 0;i<numofPp;i++){
   	   memset(&defaultcfg,0,sizeof(NAM_BRG_RATE_LIMIT_T));
       ret = cpssDxChBrgGenRateLimitGlobalCfgGet(i,&defaultcfg);
	   if(ETH_PORT_STREAM_PPS_E == modeType){
	       ratecfg.rMode = NAM_RATE_LIMIT_PCKT_BASED_E;
	   }
	   else if(ETH_PORT_STREAM_BPS_E == modeType){
	       ratecfg.rMode = NAM_RATE_LIMIT_BYTE_BASED_E;
	   }
	   ratecfg.dropMode = NAM_DROP_MODE_HARD_E;
	   ratecfg.win1000Mbps = defaultcfg.win1000Mbps;
       ratecfg.win100Mbps = defaultcfg.win100Mbps;
	   ratecfg.win10Gbps = defaultcfg.win10Gbps;
       ratecfg.win10Mbps =defaultcfg.win10Mbps;
	   ret = cpssDxChBrgGenRateLimitGlobalCfgSet(i,&ratecfg);
	   if(NAM_OK != ret){
         syslog_ax_nam_eth_err("dev %d set rate limit global config failed,ret %d!\n",i,ret);
		 return NAM_ERR;
	   }
	   else{
	   	 memset(&ratecfg,0,sizeof(NAM_BRG_RATE_LIMIT_T));
         ret = cpssDxChBrgGenRateLimitGlobalCfgGet(i,&ratecfg);
		 if(0 != ret){
            syslog_ax_nam_eth_err("get global config failed,ret %d\n",ret);
			return NAM_ERR;
		 }
	   }
   }
 #endif
    return NAM_OK;
}

/*******************************************************************************
* nam_eth_port_phy_reinit
*
* DESCRIPTION:
*      Reinit phy smi interface 
*
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*      0	- on success
*      other - on hardware error or parameters error 
*
* COMMENTS:
*       None.
*
*******************************************************************************/
unsigned int nam_eth_port_phy_reinit
(
	unsigned char dev,
	unsigned char port
)
{
	unsigned int status = NPD_SUCCESS;
#ifdef DRV_LIB_CPSS
	status = cpssDxChPhyPortSmiReInit(dev,port);
	if(NPD_SUCCESS != status) {
		syslog_ax_nam_eth_err("port(%d,%d) phy reinit error %d\n", dev, port, status);
		return status;
	}
#endif
	return status;
}

/*******************************************************************************
* nam_port_unicast_local_en
*
* DESCRIPTION:
*       Ingress Port<n> Bridge Configuration Register0, field <UcLocalEn> 
*
* INPUTS:
*       devNum    - physical device number
*       port         - physical port number
*       enable      - 1/0
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*      0	- on success
*      other - on hardware error or parameters error 
*
* AUTHOR:
*       luoxun@autelan.com
*******************************************************************************/
unsigned int nam_port_unicast_local_en
(
	unsigned char devNum,
	unsigned char port,
	boolean       enable
)
{
	unsigned int status = NPD_SUCCESS;
    
    #ifdef DRV_LIB_CPSS
	status = cpssDxChBrgGenUcLocalSwitchingEnable(devNum, port, enable);
	if(NPD_SUCCESS != status) {
		syslog_ax_nam_eth_err("port(%d,%d) cpssDxChBrgGenUcLocalSwitchingEnable error %d\n", devNum, port, status);
		return status;
	}
    #endif

	return status;
}


/*******************************************************************************
* nam_set_port_preamble_length
*
* DESCRIPTION:
*       Set the port with preamble length for Rx or Tx or both directions.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction and "both directions"
*                      options (GE ports support only Tx direction)
*       length       - length of preamble in bytes
*                      support only values of 4,8
*
* OUTPUTS:
*       None.
* RETURNS:
*      0	- on success
*      other - on hardware error or parameters error 
*
* AUTHOR:
*       zhangdi@autelan.com
*******************************************************************************/
int nam_set_port_preamble_length
(
    unsigned char devNum,
    unsigned char portNum,
    unsigned char direction,
    unsigned int length
){
	int ret = 0;
#ifdef DRV_LIB_CPSS
	ret = cpssDxChPortPreambleLengthSet(devNum,portNum,direction,length);
    if(0 != ret)
	{
		return ret;
	}
	syslog_ax_nam_eth_dbg("set port(%d,%d) %s preamble length OK!\n", devNum, portNum, direction?"TX":"RX");	
#endif
	return 0;
}
/*wangchong@autelan.com for 1x12G12s trunk119 port26,27 10G to 1000BAST for test*/
int nam_set_port_mac_mode
(
	unsigned int reg_add_num,
	unsigned int fieldata
){
	int ret = 0;
	unsigned int reg_add;
	if(reg_add_num == 0)
	{
		reg_add = 0x0880681c;
	}
	else
	{
		reg_add = 0x08806c1c;
	}
#ifdef DRV_LIB_CPSS
	ret = prvCpssDrvHwPpSetRegField(0,reg_add,13,3,fieldata);
    if(0 != ret)
	{
		return ret;
	}
	syslog_ax_nam_eth_dbg("set reg_add %d to %x OK!\n", reg_add,fieldata<<13);	
#endif
	return 0;
}
#ifdef __cplusplus
}
#endif

