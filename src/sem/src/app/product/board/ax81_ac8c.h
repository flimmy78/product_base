#ifndef __AX81_AC8C_H_
#define __AX81_AC8C_H_


#define VSC_VSC8558_MODE_CTL          0
#define VSC_VSC8558_MODE_STATUS       1
#define VSC_VSC8558_EXT_CTL_SET1      0x17
#define VSC_VSC8558_AUX_CTL_STATUS    0x1c
#define VSC_VSC8558_LED_MODE_SELECT   0x1d

typedef union
{
	uint16_t  u16;
	struct
	{
		uint16_t autoneg_complete		:1;
		uint16_t autoneg_disabled		:1; 
		uint16_t mdi_indication     	:1;
		uint16_t cd_swap		        :1;
		uint16_t a_polarity			    :1;
		uint16_t b_polarity				:1;
		uint16_t c_polarity	            :1;
		uint16_t d_polarity		        :1;
		uint16_t link_timeout1			:1;
		uint16_t actiphy_mode_en		:1;
		uint16_t duplex_status			:1;
		uint16_t speed_status			:2;
		uint16_t link_timeout0			:1;
		uint16_t reserved				:2;
	} s;
} vsc8558_aux_ctl_stat_t; /* 16 bit reg */


/* Mode Control Register */
typedef union
{
	uint16_t  u16;
	struct
	{
		uint16_t reset					:1;
		uint16_t loop_back		      	:1; 
		uint16_t speedselect_bit13     	:1;
		uint16_t autoneg_en		        :1;
		uint16_t power_down			    :1;
		uint16_t isolate				:1;
		uint16_t restart_copper_auto	:1;
		uint16_t duplex_mode		    :1;
		uint16_t collision_test			:1;
		uint16_t speedselect_bit6		:1;
		uint16_t reservered				:6;
		
	} s;
} vsc8558_mode_ctl_t; /* 16 bit reg */


#define AX81_AC8C_ETHPORT_DEFAULT_ATTR \
	{ETH_ATTR_ENABLE,1518,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
	ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
	ETH_ATTR_ON,ETH_ATTR_OFF,COMBO_PHY_MEDIA_PREFER_COPPER}



#endif
