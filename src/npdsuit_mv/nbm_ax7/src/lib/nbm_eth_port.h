#ifndef __NBM_ETH_PORT__
#define __NBM_ETH_PORT__


/*
 * Define 88E1145 Register name
 */
#define MV_88E1145_PHY_SPEC_STATUS			0x11
#define MV_88E1145_PHY_EXT_ADDR 			0x16
#define MV_88E1145_PHY_EXT_SPEC_CNTRL2		0x1A
#define MV_88E1145_PHY_EXT_SPEC_STATUS		0x1B

/*
 * For Dbus
 */
#define	NPD_DBUS_ERROR_UNSUPPORT	6

#define ETH_GE_FIBER        0x1
#define ETH_GTX_COPPER      0x2

#define PORT_STATE_GET_SUCCESS	0x0
#define PORT_STATE_GET_FAIL		0xFF


#define CPLD_PHY_LED_CTL 	0x27
#define CPLD_PRODUCT_CTL 	0x1
#define CPLD_MODULE_CTL		0x3
#define CPLD_CARD_TYPE(x) 	(0x24+x)
#define CPLD_CARD_ON 		0x10
#define AX51_XFP_TYPE 		0x1
#define AX51_GTX_TYPE 		0x2
#define AX51_SFP_TYPE		0x3


typedef enum {
	COMBO_PHY_MEDIA_LINKS_COPPER = 0,
	COMBO_PHY_MEDIA_LINKS_FIBER
}COMBO_PHY_LINKS_MEDIA;

typedef enum {
	NPD_DUPLEX_HALF = 0,
	NPD_DUPLEX_FULL
}COMBO_PHY_DUPLEX;






#define  CVMX_POW_WQ_INT_THRX(offset)      (0x8001670000000080ull+(((offset)&15)*8))

/**
  * Enums from nam_eth.h.
  */
typedef enum {
	PORT_FULL_DUPLEX_E,
	PORT_HALF_DUPLEX_E
} PORT_DUPLEX_ENT;

typedef enum {
	PORT_SPEED_10_E,
	PORT_SPEED_100_E,
	PORT_SPEED_1000_E,
	PORT_SPEED_10000_E,
	PORT_SPEED_12000_E,
	PORT_SPEED_2500_E,
	PORT_SPEED_5000_E
} PORT_SPEED_ENT;



int nbm_set_eth_port_trans_media
(
	unsigned int port,
	unsigned int media
);
int nbm_get_eth_port_trans_media
(
	unsigned int port,
	unsigned int* media
);
int nbm_read_eth_port_info
(
	unsigned char 		portNum,
	struct eth_port_s 	*portInfo
);



/**
  * Get board ports' pkt statistic.
  * @return: PORT_STATE_GET_FAIL for failed. PORT_STATE_GET_SUCCESS for success.
  * codeby: baolc, 2008-07-04
  */
unsigned int nbm_board_port_pkt_statistic
(
	unsigned char portNum, 
	struct npd_port_counter *portPktCount,
	struct port_oct_s		*portOtcCount
);

/**
  * Clear board port's pkt statistic.
  * @return: PORT_STATE_GET_FAIL for failed. PORT_STATE_GET_SUCCESS for success.
  * codeby: baolc, 2008-07-04
  */
unsigned int nbm_board_clear_port_pkt_stat
(
	unsigned char portNum
);




/**
  * Enable or disable port./Set port admin status.
  * @param portNum: The port number in main board. 0~3.
  * @param portAttr: ETH_ATTR_ENABLE for enable, ETH_ATTR_DISABLE for disable.
  * @return: NPD_FAIL for failed, NPD_SUCCESS for success.
  * codeby: baolc, 2008-06-30
  */
int nbm_set_ethport_enable
(
	unsigned char	portNum,
	unsigned long	portAttr
);


/**
  * Set port's MTU. (MTU is also named MRU.)
  * @param portNum: The port number in main board. 0~3.
  * @param mruSize: The size of mtu.
  * @return: NPD_FAIL for failed, NPD_SUCCESS for success.
  * codeby: baolc, 2008-06-30
  */
int nbm_set_ethport_mru
(
	unsigned char portNum, 
	unsigned int mruSize
);


int nbm_set_ethport_flowCtrl
(
	unsigned char portNum, 
	unsigned long state
);

int nbm_set_ethport_backPres
(
	unsigned char portNum, 
	unsigned long state
);


/*unsupported port configuration */
int nbm_set_ethport_force_linkup
(
	unsigned char portNum,
	unsigned long state
);
int nbm_set_ethport_force_linkdown
(
	unsigned char portNum,
	unsigned long state
);
int nbm_set_ethport_duplex_mode
( 
	unsigned char portNum, 
	PORT_DUPLEX_ENT	dMode
);
int nbm_set_ethport_speed
(
	unsigned char portNum, 
	PORT_SPEED_ENT speed
);

/*
int nbm_set_ethport_PeriodFCtrl
(
	unsigned char portNum, 
	unsigned long enable
);
*/

int nbm_set_ethport_duplex_autoneg
(
	unsigned char portNum,
	unsigned long state
);
int nbm_set_ethport_fc_autoneg
(
	unsigned char portNum,
	unsigned long state,
	unsigned long pauseAdvertise
);
int nbm_set_ethport_speed_autoneg
(
	unsigned char portNum,
	unsigned long state
);
int nbm_set_ethport_POW_Int_enable
(
	void
);
int nbm_get_ethport_updown_times
(
	unsigned char portNum, 
	int *uptimes,
	int *downtimes
);

unsigned long nbm_get_link_timestamp(
	unsigned char portNum
	);

/*
 * return
 *	0xX7XX ( PRODUCT_TYPE_AX7000 ) :	auteX 7000 product
 *	0x0561 ( PRODUCT_TYPE_5612E ):	5612e
 *	0x0503 ( PRODUCT_TYPE_5612I ):	5612I
 */
unsigned int nbm_get_product_type(void);
#define PRODUCT_AX7000	( (7 == ((nbm_get_product_type() >> 8) & 0xF)) || \
	(PRODUCT_TYPE_5612I == nbm_get_product_type()) )

/*
	CPLD reg 0x10 
	bit: [7]		[6]		[5]		[4]		[3]		[2]		[1]		[0]
		XFP1	XFP0	CARD1	CARD0	SFP3	SFP2	SFP1	SFP0
		BIT[4] ==0	CARD0 is ON.
		BIT[4] ==1	CARD0 is OFF.
*/

unsigned char nbm_cvm_query_card_on(int card_Num);

/*
	CARD0 REG 0X24  BIT[3:2]
	CARD1 REG 0X25  BIT[3:2]
	01 ----- AX51-XFP
	10 ----- AX51-GTX
	11 ----- AX51-SFP
*/
unsigned char cvm_query_card_type(int card_Num);

#endif

