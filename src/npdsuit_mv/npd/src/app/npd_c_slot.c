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
* npd_c_slot.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for chassis slot related routine.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.59 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/*
  Chassis slot management 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_product.h"
#include "npd_log.h"
#include "nbm/nbm_api.h"

#include "npd_c_slot.h"

#include "npd_eth_port.h"

struct chassis_slot_s **chassis_slots = NULL;

/*
  NOTE list module type data according to npd_sysdef.h and product definition
*/
struct module_type_data_s module_type_mib[] = {
/* ext_slot_count,  eth_port_1no, eth_port_count	*/
	{0,0,0},   	/*  MODULE_ID_NONE*/
	{1,1,4},   	/*  MODULE_ID_AX7_CRSMU*/
	{0,1,6},	/* MODULE_ID_AX7_6GTX*/
	{0,1,6},    /* MODULE_ID_AX7_6GE_SFP*/
	{0,1,1},	/* MODULE_ID_AX7_XFP*/
	{0,1,6},	/* MODULE_ID_AX7_6GTX_POE*/
	{0,1,12},	/* MODULE_ID_AX5_5612*/
	{2,1,24},	/* MODULE_ID_AU4_4626*/
	{0,1,24},	/* MODULE_ID_AU4_4524*/
	{0,1,24},	/* MODULE_ID_AU4_4524_POE*/
	{0,1,24},   /* MODULE_ID_AU3_3524*/
	{0,1,28},   /* MODULE_ID_AU3_3028*/
	{0,1,52},   /* MODULE_ID_AU3_3052*/
	{0,1,28},   /* MODULE_ID_AU3_3528 */
	{0,1,28},   /* MODULE_ID_AU2_2528 */
	{0,1,12},	/* MODULE_ID_AX5_5612I*/
	{0,1,6},	/* MODULE_ID_AX7I_GTX */
	{0,1,2},	/* MODULE_ID_AX7I_XG_CONNECT  */
	{0,1,1},    /* MODULE_ID_SUBCARD_TYPE_A0*/
	{0,1,1},    /* MODULE_ID_SUBCARD_TYPE_A1*/
	{0,1,2},    /* MODULE_ID_SUBCARD_TYPE_B*/	
	{0,1,8},	/* MODULE_ID_AX5_5612E*/
	{0,1,4},	/* MODULE_ID_AX5_5608*/
	{0,1,28},	/* MODULE_ID_AX71_2X12G12S, zhangdi*/
    {0,1,24},	/* MODULE_ID_AX81_SMU */
	{0,0,0}		/* MODULE_ID_MAX*/
};

struct module_type_data_s *module_type_data_old
(
	unsigned int module_id
)
{
	switch(module_id) {
		case MODULE_ID_NONE:
		default:
			return &module_type_mib[0];
		case MODULE_ID_AX7_CRSMU:
		case MODULE_ID_AX7I_CRSMU:		
			return &module_type_mib[1];
		case MODULE_ID_AX7_6GTX:
			return &module_type_mib[2];
		case MODULE_ID_AX7_6GE_SFP:
			return &module_type_mib[3];
		case MODULE_ID_AX7_XFP:
			return &module_type_mib[4];
		case MODULE_ID_AX7_6GTX_POE:
			return &module_type_mib[5];
		case MODULE_ID_AX5_5612:
			return &module_type_mib[6];
		case MODULE_ID_AU4_4626:
			return &module_type_mib[7];
		case MODULE_ID_AU4_4524:
			return &module_type_mib[8];
		case MODULE_ID_AU4_4524_POE:
			return &module_type_mib[9];
		case MODULE_ID_AU3_3524:
			return &module_type_mib[10];
		case MODULE_ID_AU3_3028:
			return &module_type_mib[11];
		case MODULE_ID_AU3_3052:
			return &module_type_mib[12];
		case MODULE_ID_AU3_3528:
			return &module_type_mib[13];
		case MODULE_ID_AU2_2528:
			return &module_type_mib[14];
		case MODULE_ID_AX5_5612I:
			return &module_type_mib[15];
		case MODULE_ID_AX7I_GTX:
			return &module_type_mib[16];
		case MODULE_ID_AX7I_XG_CONNECT:
			return &module_type_mib[17];
		case MODULE_ID_SUBCARD_TYPE_A0:
			return &module_type_mib[18];
		case MODULE_ID_SUBCARD_TYPE_A1:
			return &module_type_mib[19];			
		case MODULE_ID_SUBCARD_TYPE_B:
			return &module_type_mib[20];
		case MODULE_ID_AX5_5612E:
			return &module_type_mib[21];
		case MODULE_ID_AX5_5608:
			return &module_type_mib[22];
		case MODULE_ID_AX71_2X12G12S:
			return &module_type_mib[23];	
        case MODULE_ID_AX81_SMU:
			return &module_type_mib[24];	
	}
}

static char *product_id_desc[] = {	\
	"PRODUCT_NONE",	/* PRODUCT_ID_NONE*/
	"PRODUCT_AX7K",	/* PRODUCT_ID_AX7K*/
	"PRODUCT_AX5K",	/* PRODUCT_ID_AX5K*/
	"PRODUCT_AU4K",	/* PRODUCT_ID_AU4K*/
	"PRODUCT_AU3K",	/* PRODUCT_ID_AU3K*/
	"PRODUCT_AU3K_BCM", /* PRODUCT_ID_AU3K_BCM*/
	"PRODUCT_AU3K_BCAT", /* PRODUCT_ID_AU3K_BCAT */
	"PRODUCT_AU2K_TCAT", /* PRODUCT_ID_AU2K_TCAT */
	"PRODUCT_AX5K_I",	/* PRODUCT_ID_AX5K_I*/
	"PRODUCT_AX7Ki", /* PRODUCT_ID_AX7K_I */
	"PRODUCT_AX5K_E",	/* PRODUCT_ID_AX5K_E*/
	"PRODUCT_AX5608",
	"PRODUCT_AX8610",	/* PRODUCT_ID_AX8600 */
	"PRODUCT_ERR"	/* PRODUCT_ID_MAX*/
};

char *product_id_str
(
	unsigned int productId
)
{
	switch(productId) {
		case PRODUCT_ID_NONE:
		default:
			return product_id_desc[0];
		case PRODUCT_ID_AX7K:
			return product_id_desc[1];
		case PRODUCT_ID_AX5K:
			return product_id_desc[2];
		case PRODUCT_ID_AU4K:
			return product_id_desc[3];
		case PRODUCT_ID_AU3K:
			return product_id_desc[4];
		case PRODUCT_ID_AU3K_BCM:
			return product_id_desc[5];
		case PRODUCT_ID_AU3K_BCAT:
			return product_id_desc[6];
		case PRODUCT_ID_AU2K_TCAT:
			return product_id_desc[7];
		case PRODUCT_ID_AX5K_I:
			return product_id_desc[8];
		case PRODUCT_ID_AX7K_I:
			return product_id_desc[9];
		case PRODUCT_ID_AX5K_E:
			return product_id_desc[10];
		case PRODUCT_ID_AX5608:
			return product_id_desc[11];
        case PRODUCT_ID_AX8600:
			return product_id_desc[12];

	}
}

char *module_id_desc[] = {		\
	"MODULE_NONE",			/* MODULE_ID_NONE*/
	"MODULE_AX7_CRSMU",		/* MODULE_ID_AX7_CRSMU*/
	"MODULE_AX7_6GTX",		/* MODULE_ID_AX7_6GTX*/
	"MODULE_AX7_6GE_SFP",	/* MODULE_ID_AX7_6GE_SFP*/
	"MODULE_AX7_XFP",		/* MODULE_ID_AX7_XFP*/
	"MODULE_AX7_6GTX_PoE",	/* MODULE_ID_AX7_6GTX_POE*/
	"MODULE_AX5_5612",		/* MODULE_ID_AX5_5612*/
	"MODULE_AU4_4626",		/* MODULE_ID_AU4_4626*/
	"MODULE_AU4_4524",		/* MODULE_ID_AU4_4524*/
	"MODULE_AU4_4524_PoE",	/* MODULE_ID_AU4_4524_POE*/
	"MODULE_AU3_3524",		/* MODULE_ID_AU3_3524*/
	"MODULE_AU3_3028",		/* MOUDLE_ID_AU3_3028*/
	"MODULE_AU3_3052",		/* MOUDLE_ID_AU3_3052*/
	"MODULE_AU3_3528",		/* MODULE_ID_AU3_3528 */
	"MODULE_AU2_2528",		/* MODULE_ID_AU2_2528 */
	"MODULE_AX5_5612I",		/* MODULE_ID_AX5_5612I*/
	"MODULE_AX7I_CRSMU", /* MODULE_ID_AX7I_CRSMU */
	"MODULE_AX7I_GTX", /* MODULE_ID_AX7I_GTX */
	"MODULE_AX7I XCONN", /* MODULE_ID_AX7I_XG_CONNECT */
	"MODULE_AX5_SUBCARD",   /* MODULE_ID_SUBCARD_TYPE_A0*/
	"MODULE_AX5I_SUBCARD",  /* MODULE_ID_SUBCARD_TYPE_A1*/
	"MODULE_AU4_SUBCARD",   /* MODULE_ID_SUBCARD_TYPE_B */	
	"MODULE_AX5_5612E",		/* MODULE_ID_AX5_5612E*/
	"MODULE_ID_AX5_5608",	/* MODULE_ID_AX5_5608*/
	"MODULE_ID_AX71_2X12G12S",	/* MODULE_ID_AX71_2X12G12S*/
	"MODULE_ID_AX81_SMU",	/* MODULE_ID_AX81_SMU */
	"MODULE_MY_LOCAL_BOARD",	/* MODULE_ID_DISTRIBUTED */
	"MODULE_ERR"			/* MODULE_ID_MAX*/
};

char *module_id_str
(
	unsigned int module_id	
)
{
	switch(module_id) {
		case MODULE_ID_NONE:
		default:
			return module_id_desc[0];
		case MODULE_ID_AX7_CRSMU:
			return module_id_desc[1];
		case MODULE_ID_AX7_6GTX:
			return module_id_desc[2];
		case MODULE_ID_AX7_6GE_SFP:
			return module_id_desc[3];
		case MODULE_ID_AX7_XFP:
			return module_id_desc[4];
		case MODULE_ID_AX7_6GTX_POE:
			return module_id_desc[5];
		case MODULE_ID_AX5_5612:
			return module_id_desc[6];
		case MODULE_ID_AU4_4626:
			return module_id_desc[7];
		case MODULE_ID_AU4_4524:
			return module_id_desc[8];
		case MODULE_ID_AU4_4524_POE:
			return module_id_desc[9];
		case MODULE_ID_AU3_3524:
			return module_id_desc[10];
		case MODULE_ID_AU3_3028:
			return module_id_desc[11];
		case MODULE_ID_AU3_3052:
			return module_id_desc[12];
		case MODULE_ID_AU3_3528:
			return module_id_desc[13];
		case MODULE_ID_AU2_2528:
			return module_id_desc[14];
		case MODULE_ID_AX5_5612I:
			return module_id_desc[15];
		case MODULE_ID_AX7I_CRSMU:
			return module_id_desc[16];
		case MODULE_ID_AX7I_GTX:
			return module_id_desc[17];
		case MODULE_ID_AX7I_XG_CONNECT:
			return module_id_desc[18];		
		case MODULE_ID_SUBCARD_TYPE_A0:
			return module_id_desc[19];
		case MODULE_ID_SUBCARD_TYPE_A1:
			return module_id_desc[20];
		case MODULE_ID_SUBCARD_TYPE_B:
			return module_id_desc[21];
		case MODULE_ID_AX5_5612E:
			return module_id_desc[22];
		case MODULE_ID_AX5_5608:
			return module_id_desc[23];
		case MODULE_ID_AX71_2X12G12S:
			return module_id_desc[24];
        case MODULE_ID_AX81_SMU:
			return module_id_desc[25];
        case MODULE_ID_DISTRIBUTED:
			return module_id_desc[26];				
	}
}

int npd_get_system_environment_state(NPD_ENVI* envi_state)
{
  	return nbm_get_system_environment_state(envi_state);
}
int npd_system_shutdown_enable(unsigned short isenable)
{
	return nbm_set_system_shutdwon_enable(isenable);
}

/**********************************************************************************
 * npd_init_one_chassis_slot
 *
 * init chassis slot global info structure,dedicated slot given by chssis_slot_index
 *
 *	INPUT:
 *		chassis_slot_index - chassis slot index to initialize
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/

void npd_init_one_chassis_slot
(
	int chassis_slot_index
) 
{
	unsigned char ret = 0, card_type = 0, status = 0;
	unsigned int i = 0;
	
	if ((LOCAL_CHASSIS_SLOT_INDEX) == chassis_slot_index) {
		chassis_slots[chassis_slot_index]->module.state = nbm_get_board_state(chassis_slot_index);
		chassis_slots[chassis_slot_index]->module.id = nbm_probe_chassis_module_id(CHASSIS_SLOT_INDEX2NO(chassis_slot_index));
		chassis_slots[chassis_slot_index]->module.hw_version = productinfo.local_module_hw_version;
		
		if (PRODUCT_IS_BOX) {
			chassis_slots[chassis_slot_index]->module.id = nbm_probe_chassis_module_id(chassis_slot_index);
		/* read mainboard serial no and product sysinfo*/
			if(PRODUCT_ID_AX5K == PRODUCT_ID) {
				syslog_ax_c_slot_dbg("npd init slot index %d product %s module %s\n",chassis_slot_index,		\
						product_id_str(PRODUCT_ID_AX5K), module_id_str(chassis_slots[chassis_slot_index]->module.id));
			}
			else if(PRODUCT_ID_AX5K_I == PRODUCT_ID) {
				syslog_ax_c_slot_dbg("npd init slot index %d product %s module %s\n",chassis_slot_index,		\
						product_id_str(PRODUCT_ID_AX5K_I), module_id_str(chassis_slots[chassis_slot_index]->module.id));
			}
			else if(PRODUCT_ID_AX5K_E == PRODUCT_ID) {
				syslog_ax_c_slot_dbg("npd init slot index %d product %s module %s\n",chassis_slot_index,		\
						product_id_str(PRODUCT_ID_AX5K_E), module_id_str(chassis_slots[chassis_slot_index]->module.id));
			}
			else if(PRODUCT_ID_AU4K == PRODUCT_ID) {
				syslog_ax_c_slot_dbg("npd init slot index %d product %s module %s\n",chassis_slot_index,		\
						product_id_str(PRODUCT_ID_AU4K), module_id_str(chassis_slots[chassis_slot_index]->module.id));
			}
			else if(PRODUCT_ID_AU3K == PRODUCT_ID) {
				syslog_ax_c_slot_dbg("npd init slot index %d product %s module %s\n",chassis_slot_index,		\
						product_id_str(PRODUCT_ID_AU3K), module_id_str(chassis_slots[chassis_slot_index]->module.id));
			}
			else if(PRODUCT_ID_AU3K_BCAT == PRODUCT_ID) {
				syslog_ax_c_slot_dbg("npd init slot index %d product %s module %s\n",chassis_slot_index,		\
						product_id_str(PRODUCT_ID_AU3K_BCAT), module_id_str(chassis_slots[chassis_slot_index]->module.id));
			}
			else if(PRODUCT_ID_AX5608 == PRODUCT_ID) {
				syslog_ax_c_slot_dbg("npd init slot index %d product %s module %s\n",chassis_slot_index,		\
						product_id_str(PRODUCT_ID_AX5608), module_id_str(chassis_slots[chassis_slot_index]->module.id));
			}
			nbm_read_mainboard_sysinfo(&productinfo, &(chassis_slots[chassis_slot_index]->module));
		} 
		else {
			/* only read slot serial no and module name */
			nbm_read_chassis_module_sysinfo(CHASSIS_SLOT_INDEX2NO(chassis_slot_index),&(chassis_slots[chassis_slot_index]->module));
		}		
	}
	/* init 4626, 5612, 5612i subcard slot */
	else if(PRODUCT_IS_BOX) {
		syslog_ax_c_slot_dbg("in the slot %d for 4626, 5612, 5612i\n",chassis_slot_index);
		for(i = 0; i < CHASSIS_SUBCARD_COUNT; i ++) {
			ret = npd_get_subcard_port_type(i+1, &card_type);
			syslog_ax_c_slot_dbg("slot %d port %d to get subcard port type ret %d\n",chassis_slot_index,i,ret);			
			/* if ret = 0, subcard is on its slot */
			if(0 == ret) {
				chassis_slots[chassis_slot_index]->module.state = MODULE_STAT_RUNNING;			
				chassis_slots[chassis_slot_index]->module.id = nbm_get_subcard_module_id(PRODUCT_ID,i);	
				chassis_slots[chassis_slot_index]->module.hw_version = productinfo.local_module_hw_version;
				ret = nbm_cpld_driven_phy(PRODUCT_ID, card_type);
				break;
			}
		}
		syslog_ax_eth_port_dbg("chassis_slots[%d]->module.id %d\n",chassis_slot_index,chassis_slots[chassis_slot_index]->module.id);		
	}	
	else {
		chassis_slots[chassis_slot_index]->module.id = nbm_probe_chassis_module_id(CHASSIS_SLOT_INDEX2NO(chassis_slot_index));
		if ( MODULE_ID_NONE != chassis_slots[chassis_slot_index]->module.id) {
			chassis_slots[chassis_slot_index]->module.state = nbm_get_board_state(chassis_slot_index);
			chassis_slots[chassis_slot_index]->module.hw_version = nbm_probe_chassis_module_hw_version(CHASSIS_SLOT_INDEX2NO(chassis_slot_index));
			nbm_read_chassis_module_sysinfo(CHASSIS_SLOT_INDEX2NO(chassis_slot_index),&(chassis_slots[chassis_slot_index]->module));
		}
		else {
			npd_reset_one_chassis_slot(chassis_slot_index);
		}

		if(PRODUCT_ID_AX7K_I == PRODUCT_ID) {
			ret = nbm_query_peerboard_status(&status);
#if 0
			if((NPD_SUCCESS == ret) && (AX7i_XG_CONNECT_SLOT_NUM != chassis_slot_index) &&(AX7i_SLAVE_SLOT_NUM != chassis_slot_index)){
#endif
			if((NPD_SUCCESS == ret) &&(AX7i_SLAVE_SLOT_NUM != chassis_slot_index)){
				if(1 == status){
					syslog_ax_eth_port_dbg("query the peer board slot %d isn't on slot\n", chassis_slot_index );
					chassis_slots[chassis_slot_index]->module.id = MODULE_ID_NONE;
					chassis_slots[chassis_slot_index]->module.state = MODULE_STAT_NONE;
				}
				else if(0 == status){
					syslog_ax_eth_port_dbg("query the peer board slot %d is on slot,ret %d, status %d.\n",chassis_slot_index,ret,status);					
				}
				else{
					syslog_ax_eth_port_err("query the peer board slot %d error!ret %d, status %d.\n",chassis_slot_index,ret,status);
				}
			}
		}
	}
	return;
}

/**********************************************************************************
 * npd_init_chassis_slots
 *
 * global entrance to initialize all chassis slots
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void (*npd_init_chassis_slots)(void);
 
void npd_init_chassis_slots_old
(
	void
) 
{
	int i;

	syslog_ax_c_slot_dbg("chassis slot count is %d\n",CHASSIS_SLOT_COUNT);
	chassis_slots = malloc(sizeof(struct chassis_slot_s *) *(CHASSIS_SLOT_COUNT));
		
	memset((void *)chassis_slots,0,CHASSIS_SLOT_COUNT);
	for (i = 0; i < CHASSIS_SLOT_COUNT; i++) {
		chassis_slots[i] = malloc(sizeof(struct chassis_slot_s));
		memset((void*)(chassis_slots[i]),0,sizeof(struct chassis_slot_s));
        if(BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)
        {
        	if (LOCAL_CHASSIS_SLOT_INDEX == i) 
			{
        		chassis_slots[i]->module.state = nbm_get_board_state(i);
        		chassis_slots[i]->module.id = MODULE_ID_AX71_2X12G12S;
        		chassis_slots[i]->module.hw_version = productinfo.local_module_hw_version;
        		productinfo.sysinfo.basemac = "001122334455";
				/* for :get system mac error when rx packet! */
				/* for :show hw config error */
        		chassis_slots[i]->module.modname 	= "AX71_2X12G12S";
        		chassis_slots[i]->module.sn 		= "not init";
        		chassis_slots[i]->module.ext_slot_data = NULL;
				syslog_ax_c_slot_dbg("slot %d module %s\n",i,chassis_slots[i]->module.modname);	
		
        	}
			else
			{
        		chassis_slots[i]->module.id 		= MODULE_ID_NONE;
        		chassis_slots[i]->module.state 		= MODULE_STAT_NONE;
        		chassis_slots[i]->module.hw_version = 0;
        		chassis_slots[i]->module.modname 	= "none";
        		chassis_slots[i]->module.sn 		= "null";
        		chassis_slots[i]->module.ext_slot_data = NULL;			
				syslog_ax_c_slot_dbg("slot %d module %s\n",i,module_id_str(MODULE_ID_NONE));	
				/* the SMU slot, do nothing */
			}		
        }
		else
		{
		    npd_init_one_chassis_slot(i);
        }
		syslog_ax_c_slot_dbg("init slot %d module %s\n",i,module_id_str(MODULE_TYPE_ON_SLOT_INDEX(i)));	
	}
	return;
}

/**********************************************************************************
 * npd_reset_one_chassis_slot
 *
 * reset chassis slot global info structure,dedicated slot given by chssis_slot_index
 *
 *	INPUT:
 *		chassis_slot_index - chassis slot index to initialize
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/

void npd_reset_one_chassis_slot
(
	int chassis_slot_index
) 
{
	struct chassis_slot_s *slotInfo = NULL;

	slotInfo = chassis_slots[chassis_slot_index];
	
	if(slotInfo != NULL) {
		memset(slotInfo,0,sizeof(struct chassis_slot_s));
		slotInfo->module.id 		= MODULE_ID_NONE;
		slotInfo->module.state 		= MODULE_STAT_NONE;
		slotInfo->module.hw_version = 0;
		slotInfo->module.modname 	= "null";
		slotInfo->module.sn 		= "null";
		slotInfo->module.ext_slot_data = NULL;
	}
	else {
		syslog_ax_c_slot_err("reset chassis slot %d by info null\n",chassis_slot_index);
	}

	return;
}

/****************************************************************************************
 * npd_get_global_index_by_devport 
 *
 * get ethernet global index from PP device number and virtual port info
 *
 *	INPUT:
 *		devNum - PP device number
 *		portNum - PP port number (or say as virtual port number)
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		global eth index
 *    
 *  	NOTE:
 *		this routine is the only interface provided to others get global eth index via device and port number by NPD
 *		because in NPD layer we use global index or slot port to stands for an ether port, no device number and 
 *		port number(view from PP side); And in NAM layer we use global index or device port number to stands for
 *		an ether port, no slot port number(view from product side) provided.
 ****************************************************************************************/
unsigned int npd_get_global_index_by_devport_old
(
	IN 		unsigned char devNum,
	IN 		unsigned char portNum,
	OUT		unsigned int *eth_g_index
)
{	
	unsigned int 	slotNo,localPortNo;
	unsigned int	retVal = 0;
	if(((PRODUCT_ID_AX7K == PRODUCT_ID)&&(2 == devNum))||\
		((PRODUCT_ID_AX7K != PRODUCT_ID)&&(PRODUCT_ID_AX7K_I != PRODUCT_ID)&&\
		(PRODUCT_ID_AU3K_BCM != PRODUCT_ID)&&(1 == devNum))){
		slotNo = 0;
		localPortNo = portNum;
		if(portNum > 8){
			slotNo = 1;
		}
	}
	else{
		/* there is no asic port on panel on AX71-CRSMU(7605i), so add this for error of devNum ==1,zhangdi 20110815 */
		if(PRODUCT_ID_AX7K_I == PRODUCT_ID)
		{
			syslog_ax_c_slot_err("need not get panel port from asic port(%d,%d) on AX71-CRSMU.\n",devNum, portNum);
			return NPD_FAIL;						
		}
		retVal = nam_get_slotport_by_devport(devNum,portNum,PRODUCT_ID,&slotNo,&localPortNo);
		if(retVal != 0) {
			*eth_g_index = ~0;
			syslog_ax_c_slot_err("get panel port from asic port(%d,%d) product id 0x%x error %s",devNum, portNum,PRODUCT_ID, nam_err_msg[retVal]);
			return retVal;
		}
	}

	*eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotNo,localPortNo);

	/*syslog_ax_c_slot_dbg("get global index %#x from asic port(%d,%d)\r\n", */
	/*				*eth_g_index, devNum,portNum);*/
	return retVal;
}

/****************************************************************************************
 * npd_get_devport_by_global_index 
 *
 * get PP device number and virtual port info from ethernet global index
 *
 *	INPUT:
 *		eth_g_index - ether global index
 *
 *	OUTPUT:
 *		devNum - PP device number
 *		portNum - PP port number (or say as virtual port number)
 *
 *	RETURN:
 *		NPD_SUCCESS - if no error occur
 *		NAM_ERR_SLOT_OUT_OF_RANGE - if slot number given is out of range
 *		NAM_ERR_PORT_OUT_OF_RANGE - if port number given is out of range
 *		NAM_ERR_MODULE_NOT_SUPPORT - if module type of specified slot is unsupported
 *    
 *  	NOTE:
 *		this routine is the only interface provided to others get device and port number via global eth index by NPD
 *		because in NPD layer we use global index or slot port to stands for an ether port, no device number and 
 *		port number(view from PP side); And in NAM layer we use global index or device port number to stands for
 *		an ether port, no slot port number(view from product side) provided.
 ****************************************************************************************/

unsigned int npd_get_devport_by_global_index_old
(
	IN	unsigned int eth_g_index,
	OUT	unsigned char *devNum,
	OUT	unsigned char *portNum
)
{
	unsigned int ret = NPD_SUCCESS;
	unsigned int slot_no = 0, slot_index = 0;
	unsigned int local_port_no = 0,port_index = 0;
	enum module_id_e module_type;
	unsigned int devTmp = 0,portTmp = 0;
	
	slot_index 		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	slot_no			= CHASSIS_SLOT_INDEX2NO(slot_index);
	port_index 		= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
		/* 
		 * by qinhs@autelan.com 10/13/2008 
		 * 	slot_no or slot_index must be legal when fetching local_port_no and module_type
		 *	otherwise, exception or crash must happen.
		 */
		local_port_no	= ETH_LOCAL_INDEX2NO(slot_index,port_index);
		module_type 	= MODULE_TYPE_ON_SLOT_INDEX(slot_index);
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
			ret = NPD_SUCCESS;
		}
		else {
			syslog_ax_c_slot_err("illegal port %d\n",local_port_no);
			syslog_ax_c_slot_err("  gindex %#x slot %d port %d module %d\n", \
								eth_g_index, slot_no, local_port_no, module_type);
			return -NPD_FAIL;
		}
	}
	else {
		syslog_ax_c_slot_err("illegal slot %d\n",slot_no);
		return -NPD_FAIL;
	}
	if(TRUE == npd_eth_port_rgmii_type_check(slot_no,local_port_no)){
		*devNum = 1;
		*portNum = local_port_no;
		if(PRODUCT_ID_AX7K == PRODUCT_ID){
			*devNum = 2;
		}
	}
	else {
		ret = nam_get_devport_by_slotport(slot_no, local_port_no, module_type, &devTmp, &portTmp);
		if(ret != 0){
			syslog_ax_c_slot_err("convert index %#x to port %d/%d get devport error %d.\n",eth_g_index,slot_no,local_port_no,ret);
		}
		else {
			*devNum = devTmp;
			*portNum = portTmp;
			/*syslog_ax_c_slot_dbg("convert index %#x to port %d/%d,and then dev %d,port %d", 	\*/
			/*					eth_g_index,slot_no,local_port_no,*devNum,*portNum);*/
		}
	}
	return ret;	
}
/**********************************************************************************
 * npd_init_board_check_thread
 *
 * initialize a thread to check board state:read board state from CPLD and check 
 * if board is running or being pulled out.
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/

void npd_init_board_check_thread
(
	void
)
{
	unsigned char slotCount = CHASSIS_SLOT_COUNT;

	nam_thread_create("BoardStateCheck",(void*)nbm_check_board_state,slotCount,NPD_TRUE,NPD_FALSE);

	return;
}

/**********************************************************************************
 * npd_chassis_slot_hotplugin
 *
 * npd callback functions called when chassis slot is hot plugged in,which reported by board check thread.
 *
 *	INPUT:
 *		chassis_slot_index - chassis slot index(or slot number)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_chassis_slot_hot_plugin
(
	int chassis_slot_index
) 
{
	int i = 0, ret = NPD_SUCCESS;	
	/* first we should re-initialize chassis slot info*/
	npd_init_one_chassis_slot(chassis_slot_index);
	
	/* TODO: some other re-initialize process we should taken here*/	
	nbm_gpio_reset_single_board(chassis_slot_index);

	for (i = 0; i < ETH_LOCAL_PORT_COUNT(chassis_slot_index); i++) 
	{
		ret = npd_eth_port_reinit(chassis_slot_index, i);
		if(NPD_SUCCESS != ret) {
			syslog_ax_c_slot_err("slot hot plugin reinit port %d/%d error %d\n", chassis_slot_index, i, ret);
		}
	}
	
	return;
}

/**********************************************************************************
 * npd_chassis_slot_hot_pullout
 *
 * npd callback functions called when chassis slot is hot pulled out,which reported by board check thread.
 *
 *	INPUT:
 *		chassis_slot_index - chassis slot index(or slot number)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/
void npd_chassis_slot_hot_pullout
(
	int chassis_slot_index
) 
{
	/* first we should re-initialize chassis slot info*/
	npd_reset_one_chassis_slot(chassis_slot_index);
	
	/* TODO: some other clean-up process we should taken here*/

	return;
}
#ifdef __cplusplus
}
#endif
