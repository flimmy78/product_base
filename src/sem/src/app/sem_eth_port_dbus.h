#ifndef SEM_ETH_PORT_DBUS_H
#define SEM_ETH_PORT_DBUS_H

extern board_fix_param_t *local_board;
extern product_fix_param_t *product;
#define SLOT_COUNT product->slotcount

#define AX81_1X12G12S_BOARD_CODE	0x5
#define AX71_1X12G12S_BOARD_CODE	0x5

#define MAX_IFNAME_LEN 20

#define LOCAL_PORT_INDEX(port)      ((port) - 1)   
#define	SLOT_PORT_COMBINATION(slot, port)	(((((slot) & 0x1f) - 1)<<6) + ((port) - 1))
#define	SLOT_PORT_ANALYSIS_SLOT(combination, slot) 	(slot = (((combination)>>6) & 0x1f) + 1)
#define	SLOT_PORT_ANALYSIS_PORT(combination, port) 	(port = ((combination) & 0x3f) + 1)

#define LOCAL_PORT_ISLEGAL(portno) (((portno) >= 1) && ((portno) <= local_board->port_num_on_panel))
#define LOCAL_SLOT_ISLEGAL(slotno) (((slotno) >= 1) && ((slotno) <= product->slotcount))

#define PRODUCT_SLOT_START_NO 0
#define BOARD_PORT_START_NO 1

#define PRODUCT_SLOTNO_ISLEGAL(slot_no) ((slot_no > 0) && ((slot_no) < (product->slotcount)) && ((slot_no) == local_board->slot_id + 1))
#define ETH_LOCA_BOARD_PORTNO_ISLEGAL(slot_no,port_no) (PRODUCT_SLOTNO_ISLEGAL(slot_no) &&(((port_no) >= BOARD_PORT_START_NO) && ((port_no) <= (local_board->port_num_on_panel))))
#endif
