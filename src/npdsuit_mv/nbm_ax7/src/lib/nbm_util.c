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
* nbm_util.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NBM for utilities.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.28 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*
  NPD call Board Management api to get hardware board & backplane information.
*/

#include "stdio.h"
#include <sys/stat.h>
#include <fcntl.h>
#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "nbm_util.h"
#include "nbm_cpld.h"
#include "nbm_eeprom.h"
#include "nbm_log.h"

#define NBM_SYSTEM_LOCALBOARD_SLOTNO_FILE "/var/run/current.slot"

void nbm_convert_board_product_id
(
	struct product_s *product_info,
	int board_id, 
	int backplane_id
)
{
	unsigned char local_slot = 0;
	/*
	return product id and module id which is readable over dbus message
	*/
	switch (backplane_id) {
		case BACKPLANE_TYPE_NONE: /* for box product*/
			switch (board_id) {
				case OCTEON_MAINBOARD_ID_AX5K:
					product_info->product_id = PRODUCT_ID_AX5K;
					product_info->local_module_id = nbm_probe_chassis_module_id(0);
					product_info->local_chassis_slot_no = 1;
					if (MODULE_ID_AX5_5612 == product_info->local_module_id) {
						product_info->product_id = PRODUCT_ID_AX5K;
					}
					else if (MODULE_ID_AX5_5612I == product_info->local_module_id) {
						product_info->product_id = PRODUCT_ID_AX5K_I;
					}
					else if (MODULE_ID_AX5_5612E == product_info->local_module_id) {
						product_info->product_id = PRODUCT_ID_AX5K_E;
					}
					else if (MODULE_ID_AX5_5608 == product_info->local_module_id) {
						product_info->product_id = PRODUCT_ID_AX5608;
					}
					
					syslog_ax_util_dbg("AX5K converted product id %#x local slot number %d module id %#x\n", \
								product_info->product_id, product_info->local_chassis_slot_no, product_info->local_module_id);
					return;
				case OCTEON_MAINBOARD_ID_AU4K:
					product_info->product_id = PRODUCT_ID_AU4K;
					product_info->local_module_id = nbm_probe_chassis_module_id(0);
					product_info->local_chassis_slot_no = 1;
					return;
				case OCTEON_MAINBOARD_ID_AU3K:
					product_info->product_id = PRODUCT_ID_AU3K;
					product_info->local_module_id = nbm_probe_chassis_module_id(0);
					product_info->local_chassis_slot_no = 1;
					if(MODULE_ID_AU3_3028 == product_info->local_module_id ||
						MODULE_ID_AU3_3052 == product_info->local_module_id) {
						product_info->product_id = PRODUCT_ID_AU3K_BCM;
					}
					syslog_ax_util_dbg("converted product id %#x local slot number %d module id %#x\n", \
								product_info->product_id, product_info->local_chassis_slot_no, product_info->local_module_id);
					return;
				case OCTEON_MAINBOARD_ID_AX2K:
					return;
				#ifdef DIAG_XCAT
				case XCAT_MAINBOARD_ID_AU1K:
					product_info->product_id = PRODUCT_ID_AU3K_BCAT;
					product_info->local_module_id = MODULE_ID_AU3_3528;
					product_info->local_chassis_slot_no = 2;
					syslog_ax_util_dbg("converted product id %#x local slot number %d module id %#x\n", \
								product_info->product_id, product_info->local_chassis_slot_no, product_info->local_module_id);
					return;
				#endif
				default:
					return;
					
			}
			
			break;
			
			case BACKPLANE_TYPE_ID_AX7605_V1: /* for ax7k chassis product*/
				switch (board_id) {
					case OCTEON_MAINBOARD_ID_AX7_CRSMU:
						product_info->product_id  = PRODUCT_ID_AX7K;
						product_info->local_module_id = MODULE_ID_AX7_CRSMU;
						product_info->local_chassis_slot_no = 0;
						return;
					default:
						return;
					
				}
				break;
			case BACKPLANE_TYPE_ID_AX7605I: /* for AX7605i dual master board chassis product */
				switch(board_id) {
					case OCTEON_MAINBOARD_ID_AX7_CRSMU:
						product_info->product_id  = PRODUCT_ID_AX7K_I;
						product_info->local_module_id = MODULE_ID_AX7I_CRSMU;
						if(!nbm_query_localboard_slotno(&local_slot)) {
							product_info->local_chassis_slot_no = local_slot;
							syslog_ax_util_dbg("converted product id %#x local slot number %d module id %#x\n", \
										product_info->product_id, product_info->local_chassis_slot_no, product_info->local_module_id);
						}
						return;
					case OCTEON_MAINBOARD_ID_UNKNOW:
						product_info->product_id  = PRODUCT_ID_AX7K_I;
						/* zhangdi add for debug!!!!! */
						product_info->local_module_id = MODULE_ID_AX71_2X12G12S;						
						if(!nbm_query_localboard_slotno(&local_slot)) {
							product_info->local_chassis_slot_no = local_slot;
                            #if 1
							product_info->local_chassis_slot_no = 2;  /* add for debug, read from cpld is 3 on 7605i*/
                            #endif
							syslog_ax_util_dbg("converted product id %#x local slot number %d module id %#x\n", \
										product_info->product_id, product_info->local_chassis_slot_no, product_info->local_module_id);
						}
						return;						
					default:
						return;
					
				}
				break;
            case BACKPLANE_TYPE_ID_AX8610: /* for AX8610 */
				switch(board_id) {
					case OCTEON_MAINBOARD_ID_AX8600:
						product_info->product_id  = PRODUCT_ID_AX8600;
						product_info->local_module_id = MODULE_ID_AX81_SMU;
						if(!nbm_query_localboard_slotno(&local_slot)) {
							product_info->local_chassis_slot_no = local_slot;
							syslog_ax_util_dbg("converted product id %#x local slot number %d module id %#x\n", \
										product_info->product_id, product_info->local_chassis_slot_no, product_info->local_module_id);
						}
						return;
					case OCTEON_MAINBOARD_ID_UNKNOW:
						product_info->product_id  = PRODUCT_ID_AX8600;
						product_info->local_module_id = MODULE_ID_AX71_2X12G12S;						
						if(!nbm_query_localboard_slotno(&local_slot)) {
							product_info->local_chassis_slot_no = local_slot;
                            #if 1
							product_info->local_chassis_slot_no = 2;  /* add for debug, read from cpld is 3 on 7605i*/
                            #endif
							syslog_ax_util_dbg("converted product id %#x local slot number %d module id %#x\n", \
										product_info->product_id, product_info->local_chassis_slot_no, product_info->local_module_id);
						}
						return;						
					default:
						return;
					
				}
				break;
		default:
			break;
	}
	return;
}

int nbm_init_productinfo
(
	struct product_s *product_info
)
{
	
	int mainboard_id,backplan_id;
	unsigned char mainboard_version;
	
	if (NPD_FAIL == nbm_init_cpld()) {
		syslog_ax_util_err("Fatal Erorr, failed init board management registers.\n");
		#ifndef DIAG_XCAT
		return NPD_FAIL;
		#endif
	} ;
	mainboard_id = nbm_query_mainboard_id();
	mainboard_version = nbm_query_mainboard_version();

/* NOTE: read backplane based on board type which could be a box product*/
	switch (mainboard_id) {
		case OCTEON_MAINBOARD_ID_AX7_CRSMU:
			backplan_id = nbm_query_backplane_id();
			break;
		case OCTEON_MAINBOARD_ID_AX5K:
		case OCTEON_MAINBOARD_ID_AU4K:
		case OCTEON_MAINBOARD_ID_AU3K:
		case OCTEON_MAINBOARD_ID_AX2K:
		case XCAT_MAINBOARD_ID_AU1K:
			/* return BACKPLANE_TYPE_NONE for all box product*/	
			backplan_id = BACKPLANE_TYPE_NONE;
			break;
		case OCTEON_MAINBOARD_ID_AX8600:
			backplan_id = BACKPLANE_TYPE_ID_AX8610; /* add for debug */
			break;			
		default:
			backplan_id = BACKPLANE_TYPE_ID_UNKNOW; /* code optimize: Using uninitialized value "backplan_id" */
			break;
	}

	#ifdef DIAG_XCAT
	mainboard_id = XCAT_MAINBOARD_ID_AU1K;
	mainboard_version = 0;
	backplan_id = BACKPLANE_TYPE_NONE;
	#endif
    if(BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S)   /*zhangdi add for debug*/
    {
		/* here we can use BOARD_TYPE to branch */
        /* zhangdi add for debug  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
    	mainboard_id = OCTEON_MAINBOARD_ID_UNKNOW;
		backplan_id = BACKPLANE_TYPE_ID_AX7605I;
    }
	/*
	NOTE convert board_id and backplane_id which is defined based on cpld, into
	product id and module id which is transfered over dbus and applications.
				
	*/
	nbm_convert_board_product_id(product_info,mainboard_id,backplan_id);
	
	product_info->local_module_hw_version = mainboard_version;

	/*
	Read backplane & product eeprom
	*/
	if (backplan_id != BACKPLANE_TYPE_NONE) {
		nbm_read_backplane_sysinfo(product_info);
	}
		
	return NPD_SUCCESS;
}

/**********************************************************************************
 * nbm_init_system_state_info
 * 
 * initialize system state info such as 
 *		FANs state,
 *		POWER state,
 *		system TEMPERATURE
 *		and so on.
 *
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred.
 *		NPD_FAIL	 - if error occurred.
 *		
 **********************************************************************************/
void nbm_init_system_temperature(struct system_state_s *stateInfo)
{
	stateInfo->core.criticalH =  SYSTEM_CORE_HIGH_CRITICAL;
	stateInfo->core.criticalL =  SYSTEM_CORE_LOW_CRITICAL;
	stateInfo->core.flag = 0;
	
	stateInfo->surface.criticalH = SYSTEM_SURFACE_HIGH_CRITICAL;
	stateInfo->surface.criticalL = SYSTEM_SURFACE_LOW_CRITICAL;
	stateInfo->surface.flag = 0;
	
}

/**********************************************************************************
 * nbm_init_system_state_info
 * 
 * initialize system state info such as 
 *		FANs state,
 *		POWER state,
 *		system TEMPERATURE
 *		and so on.
 *
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred.
 *		NPD_FAIL	 - if error occurred.
 *		
 **********************************************************************************/
int nbm_init_system_state_info
(
	struct system_state_s *stateInfo
)
{
	if(NULL == stateInfo) {
		syslog_ax_util_err("nbm init system state info null pointer error.\n");
		return NPD_FAIL;
	}

	stateInfo->fan_state     = nbm_check_fan_state();
	
	stateInfo->power_state = nbm_check_power_state();
	
	nbm_init_system_temperature(stateInfo);
	if(0 != nbm_get_sys_temperature(&(stateInfo->core.current),&(stateInfo->surface.current))){
		 syslog_ax_util_err("nbm init system state info get sys temperature error.\n");
	 }

	return NPD_SUCCESS;
}
/**********************************************************************************
 * nbm_init_system_localboard_slotno
 * 
 * 	This function initialize local board chassis slot number,
 *	Currently only product PRODUCT_ID_AX7K_I need this operation.
 *
 *	INPUT:
 *		product_id - product id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred.
 *		NPD_FAIL	 - if error occurred.
 *		
 **********************************************************************************/
int nbm_init_system_localboard_slotno
(
	enum product_id_e product_id
)
{
	int ret = NPD_SUCCESS, fd = -1;
	unsigned char slotno = 0xFF;
	char writeBuf[4] = {0};

	/* check if product need this operation or not */
	switch(product_id) {
		default:
			nbm_syslog_dbg("nbm init local board slot number but product %#x not support, ignore!\n", product_id);
			return ret;
		case PRODUCT_ID_AX7K_I:
			break;
	}

	/* query local board slot number from CPLD register */
	ret = nbm_query_localboard_slotno(&slotno);
	if(NPD_SUCCESS != ret) {
		nbm_syslog_dbg("nbm init local board slot number error %d\n", ret);
		return ret;
	}

	/* log slot number to file NBM_SYSTEM_LOCALBOARD_SLOTNO_FILE */
	fd = open(NBM_SYSTEM_LOCALBOARD_SLOTNO_FILE, O_RDWR | O_CREAT);
	if(fd < 0) {
		nbm_syslog_err("nbm log local board slot number to %s open error!\n", NBM_SYSTEM_LOCALBOARD_SLOTNO_FILE);
		return NPD_FAIL;
	}
	sprintf(writeBuf,"%d", slotno);
	write(fd, writeBuf, strlen(writeBuf));
	close(fd);
	
	return NPD_SUCCESS;
}
#ifdef __cplusplus
}
#endif
