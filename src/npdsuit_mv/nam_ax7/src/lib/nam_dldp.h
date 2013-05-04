#ifndef __NAM_DLDP_H__
#define __NAM_DLDP_H__

/*********************************************************
*	head files														*
**********************************************************/
#include <stdio.h>
#include <string.h>
#include <strings.h>
	
#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "npd/nam/npd_amapi.h"

#include "nam_vlan.h"
#include "nam_log.h"


/*********************************************************
*	macro define													*
**********************************************************/


/*********************************************************
*	struct define													*
**********************************************************/


/*********************************************************
*	function declare												*
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
);


/*********************************************************
*	extern Functions												*
**********************************************************/


#endif



