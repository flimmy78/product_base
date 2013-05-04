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
* npd_e_slot.c
*
*
* CREATOR:
*		chenb@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for chassis extended slots.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.15 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/*
  Extend slot management

*/
#include <stdio.h>

#include "sysdef/npd_sysdef.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_product.h"
#include "npd_log.h"
#include "npd_c_slot.h"
#include "npd_e_slot.h"

/* add by jinpc@autelan.com */
/* this MACRO is for AX7K product */
#define AX7_MAX_XG_PORT_NUM				4			/* number of xg-ports	*/
static unsigned char npdXGPhySmiAX7[][2] = {
	/* devNum,  portNum*/
	{0, 0x18},
	{0, 0x19},
	{1, 0x18},
	{1, 0x19},
};

/* this MACRO is for AX5612 product */
#define AX5_MAX_XG_PORT_NUM				1			/* number of xg-ports	*/
static unsigned char npdXGPhySmiAX5[][2] = {
	/* devNum,  portNum*/
	{0, 0x18},
	{0, 0x19},
};

/* this MACRO is for AX5612i product */
#define AX5I_MAX_XG_PORT_NUM				1			/* number of xg-ports	*/
static unsigned char npdXGPhySmiAX5I[][2] = {
	/* devNum,  portNum*/
	{0, 0x19},
};

/* this MACRO is for AU4K product */
#define AU4_MAX_XG_PORT_NUM				2			/* number of xg-ports	*/
static unsigned char npdXGPhySmiAU4[][2] = {
	/* devNum,  portNum*/
	{0, 0x18},
	{0, 0x19},
};

/* this MACRO is for AU3K product */
#define AU3_MAX_XG_PORT_NUM				2			/* number of xg-ports	*/
static unsigned char npdXGPhySmiAU3[][2] = {
	/* devNum,  portNum*/
	{0, 0x18},
	{0, 0x19},
};

void npd_init_ext_slot(int slot_index) {
/*
Should be filled with real probe functions.
      with funcs like nbm_probe_ext_slot.
*/
	
     return;
}

void npd_init_extend_slot(void) {
    int i;
	
	for (i = 0; i < CHASSIS_SLOT_COUNT; i++) {
		if (EXT_SLOT_COUNT(i) > 0) {
			syslog_ax_e_slot_dbg("slot %d has %d extend slots",i,EXT_SLOT_COUNT(i)) ;
        	npd_init_ext_slot(i);
		}
	}
	
	return;
}

/*******************************************************************************
* npd_init_eslot_port
*
* DESCRIPTION:
*       set LED interface for XG-Port on extend slot
*
* INPUTS:
*       NULL
*
* OUTPUTS:
*	   NULL
*
* RETURNS:
*	   NULL
*
*******************************************************************************/
void npd_init_eslot_port
(
	void
) 
{
	unsigned int ret = 0;
	unsigned int i = 0;
	unsigned int max_port = 0;
	unsigned char (*portPtr)[2] = NULL;
	unsigned char devNum = 0;
	unsigned char portNum = 0;

	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID){
		syslog_ax_e_slot_dbg("product %#x not support extern slot.\n", PRODUCT_ID);
		return;
	}	*/

	if(!((productinfo.capbmp) & FUNCTION_ESLOT_VLAUE)){
		npd_syslog_dbg("do not support extern slot.!\n");
		return;
	}
	else if(((productinfo.capbmp) & FUNCTION_ESLOT_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ESLOT_VLAUE)){
		npd_syslog_dbg("do not support extern slot!\n");
		return;
	}
	else{
	switch (PRODUCT_ID)
	{
		case PRODUCT_ID_AX7K:
		case PRODUCT_ID_AX7K_I:
		{
			max_port = AX7_MAX_XG_PORT_NUM;
			portPtr = npdXGPhySmiAX7;
			break;
		}
		case PRODUCT_ID_AX5K:
		{
			max_port = AX5_MAX_XG_PORT_NUM;
			portPtr = npdXGPhySmiAX5;
			break;
		}
		case PRODUCT_ID_AX5K_I:
		{
			max_port = AX5I_MAX_XG_PORT_NUM;
			portPtr = npdXGPhySmiAX5I;
			break;
		}
		case PRODUCT_ID_AU4K:
		{
			max_port = AU4_MAX_XG_PORT_NUM;
			portPtr = npdXGPhySmiAU4;
			break;
		}
		case PRODUCT_ID_AU3K:
		{
			max_port = AU3_MAX_XG_PORT_NUM;
			portPtr = npdXGPhySmiAU3;
			break;
		}
		default :
		{
			syslog_ax_e_slot_dbg("product %#x not support XG ports.\n", PRODUCT_ID);
			return ;
		}
	}

	for (i = 0; i < max_port; i++, portPtr++)
	{
		devNum = (*portPtr)[0];
		portNum = (*portPtr)[1];
		syslog_ax_e_slot_dbg("product %#x set eslot port(%d,%d)\n",
								PRODUCT_ID, devNum, portNum);
		/* set external Phy LED interface*/
		ret = nam_set_xgport_led_on_external_phy(devNum, portNum);
		if (NPD_SUCCESS != ret) {
			syslog_ax_e_slot_dbg("set eslot port(%d,%d) LED on external phy error %d\n",
					devNum, portNum, ret);
		}
	}

	return ;
	}
}

/*******************************************************************************
* npd_eslot_check_subcard_module
*
* DESCRIPTION:
* 	This function checks if the module given has subcard or subcard module.
*
* INPUTS:
*       module_type - product module type
*
* OUTPUTS:
*	   NULL
*
* RETURNS:
*	   0 - neither subcard module nor module with subcard, or
*		   module type is unrecgonized.
*	   1 - either subcard module or module with subcard.
*
*******************************************************************************/
int npd_eslot_check_subcard_module
(
	enum module_id_e  module_type
)
{
	int ret = 0;

	switch(module_type) {
		default:
			break;
		case MODULE_ID_SUBCARD_TYPE_A0:
		case MODULE_ID_SUBCARD_TYPE_A1:
		case MODULE_ID_SUBCARD_TYPE_B:
			ret = 1;
			break;
	}

	return ret;
}

#ifdef __cplusplus
}
#endif
