#ifndef __NPD_E_SLOT_H__
#define __NPD_E_SLOT_H__

void npd_init_extend_slot(void);

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
);

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
);
#endif
