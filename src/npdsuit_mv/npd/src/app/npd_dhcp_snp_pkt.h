#ifndef __NPD_DHCP_SNP_PKT_H__
#define __NPD_DHCP_SNP_PKT_H__


/*********************************************************
*	macro define													*
**********************************************************/
#define NPD_DHCP_SNP_REQUEST_TIMEOUT                 (60)


/*********************************************************
*	struct define													*
**********************************************************/


/*********************************************************
*	function declare												*
**********************************************************/


/**********************************************************************************
 *npd_dhcp_snp_get_item_from_pkt()
 *
 *	DESCRIPTION:
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex
 *		NPD_DHCP_MESSAGE_T *packet
 *
 *	OUTPUTS:
 *		NPD_DHCP_SNP_USER_ITEM_T *user
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK
 *		DHCP_SNP_RETURN_CODE_ERROR
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL
 *
 ***********************************************************************************/
unsigned int npd_dhcp_snp_get_item_from_pkt
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *packet,
	NPD_DHCP_SNP_USER_ITEM_T *user
);

/**********************************************************************************
 *npd_dhcp_snp_discovery_process()
 *
 *	DESCRIPTION:
 *		destroy DHCP Snooping packet receive
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, parameter is null
 ***********************************************************************************/
unsigned int npd_dhcp_snp_discovery_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
);

/**********************************************************************************
 *npd_dhcp_snp_offer_process()
 *
 *	DESCRIPTION:
 *		destroy DHCP Snooping packet receive
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 ***********************************************************************************/
unsigned int npd_dhcp_snp_offer_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
);

/**********************************************************************************
 *npd_dhcp_snp_request_process()
 *
 *	DESCRIPTION:
 *		destroy DHCP Snooping packet receive
 *
 *	INPUTS:
  *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, parameter is null
 ***********************************************************************************/
unsigned int npd_dhcp_snp_request_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
);

/**********************************************************************************
 *npd_dhcp_snp_ack_process()
 *
 *	DESCRIPTION:
 *		destroy DHCP Snooping packet receive
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, parameter is null
 ***********************************************************************************/
unsigned int npd_dhcp_snp_ack_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
);

/**********************************************************************************
 *npd_dhcp_snp_nack_process()
 *
 *	DESCRIPTION:
 *		destroy DHCP Snooping packet receive
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, parameter is null
 ***********************************************************************************/
unsigned int npd_dhcp_snoop_nack_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
);

/**********************************************************************************
 *npd_dhcp_snp_release_process()
 *
 *	DESCRIPTION:
 *		release DHCP Snooping packet receive
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, parameter is null
 ***********************************************************************************/
unsigned int npd_dhcp_snp_release_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
);

unsigned short npd_dhcp_snp_checksum
(
	void *addr,
	int count
);

/*********************************************************
*	extern Functions												*
**********************************************************/

#endif

