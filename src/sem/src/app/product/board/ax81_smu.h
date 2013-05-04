#ifndef __AX81_CRSMU_H_
#define __AX81_CRSMU_H_



//extern board_fix_param_t ax71_crsmu;
#define AX81_CRSMU_ETHPORT_DEFAULT_ATTR \
	{ETH_ATTR_ENABLE,1518,PORT_FULL_DUPLEX_E,PORT_SPEED_1000_E,	\
	ETH_ATTR_DISABLE,ETH_ATTR_DISABLE,ETH_ATTR_ON,ETH_ATTR_ON,	\
	ETH_ATTR_ON,ETH_ATTR_OFF,COMBO_PHY_MEDIA_PREFER_COPPER}

/* Mavell 88E6185 register definations */
typedef union
{
    unsigned short int u16;
    struct smi_cmd_s
    {
		unsigned int smi_busy	: 1;
		unsigned int reserved	: 2;
        unsigned int smi_mode   : 1;       
        unsigned int smi_op     : 2;       /* SMI Opcode 1=write 2=read */
		unsigned int dev_addr	: 5;       /* Device Address */
		unsigned int reg_addr	: 5;	   /* PHY Register Offset */
    } s;
} smi_cmd_88e6185_reg_t;


#define MV_88E6185_SMI_COMMAND_REG           0x00
#define MV_88E6185_SMI_DATA_REG              0x01

#define MV_88E6185_AX52XX_SGMII_ADDR         0x14

#define MV_88E6185_PORT_STATUS_REG           0x00
#define MV_88E6185_PSC_CONCTRL_REG           0x01
#define MV_88E6185_CTRL_FORCE_SPEED_1000     0x0002
#define MV_88E6185_CTRL_FORCE_DUPLEX         0x0004
#define MV_88E6185_PORT_BASED_VLAN_MAP		 0X0006
#define MV_88E6185_CTRL_DUPLEX_FULL          0x0008
#define MV_88E6185_CTRL_FORCE_LINK           0x0010
#define MV_88E6185_CTRL_FORCE_LINKUP         0x0020
#define MV_88E6185_CTRL_AUTONEG_EN           0x0400
#define MV_88E6185_CTRL_ANBYPASS_EN          0x0800

#endif
