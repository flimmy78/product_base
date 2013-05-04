#ifndef __NPD_DBM_H__
#define __NPD_DBM_H__

/**********************************************************************************
 * nbm_query_localboard_slotno
 *
 * 	This function read CPLD register to get which chassis slot the current board is inserted in 
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		slotno - current board slot number.
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
extern int nbm_query_localboard_slotno
(
	unsigned char *slotno
);

/**********************************************************************************
 * nbm_query_peerboard_status
 *
 * 	This function read CPLD register to get peer board on-slot status
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		state - peer board on-slot status.
 *				0 - peer board is on slot
 *				1 - peer board isn't on slot
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *
 **********************************************************************************/
extern int nbm_query_peerboard_status
(
	unsigned char *state
);

/**********************************************************************************
 * nbm_reset_peerboard
 *
 * 	This function write CPLD register to reset peer board.
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *
 *	RETURN:
 *		0 - if no error occur.
 *		NPD_FAIL - if error occurred.
 *	NOTE:
 *		To invoke a reset procedure, write 0 and delay 10 ms and then write 1 to
 *	the control register, illustrated as follows:
 *		WR 0 --> delay 10 ms --> WR 1
 *
 **********************************************************************************/
extern int nbm_reset_peerboard
(
	void
);

/**********************************************************************************
 *	external function declearation
 **********************************************************************************/

/**********************************************************************************
 *  npd_dbm_init
 *
 *	DESCRIPTION:
 * 		Initialize DBM module
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
 **********************************************************************************/
void npd_dbm_init();

/**********************************************************************************
 *  npd_dbm_cscd_intf_init
 *
 *	DESCRIPTION:
 * 		Initialize cascading ports related interface(s), which are needed by DCCN module
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
 **********************************************************************************/
int npd_dbm_cscd_intf_init(void);

/**********************************************************************************
 *  npd_dbm_dccn_thread
 *
 *	DESCRIPTION:
 * 		Main routine for thread which executes DCC netlink communication and 
 * 	performs board role election(Master/Slave).
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
 **********************************************************************************/
int npd_dbm_dccn_thread(void);

/**********************************************************************************
 *  npd_dbm_create_dccn_recvskb_thread
 *
 *	DESCRIPTION:
 * 		Main routine for thread which receive packet via DCC netlink communication and 
 * 	handle the packet.
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
 **********************************************************************************/
static void *npd_dbm_create_dccn_recvskb_thread(void);

/**********************************************************************************
 *  npd_dbm_skb_handle
 *
 *	DESCRIPTION:
 * 		Main routine for handle message received via DCC netlink communication.
 *
 *	INPUT:
 *		addr - netlink address
 *		dccnhdr - DCC Netlink header point to message buffer
 *		len - message buffer size
 *		arg - appendix
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
int npd_dbm_skb_handle
(
	const struct sockaddr_nl *addr,
	struct dccn_msghdr *dccnhdr,
	int len,
	void *arg
);

extern int npd_set_cscd_port_promi_mode(unsigned char port);
extern int npd_set_cscd_port_ipaddr(unsigned int ipaddr, unsigned char port);
#endif
