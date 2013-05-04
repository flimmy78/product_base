#ifndef __NAM_DIAG_H__
#define __NAM_DIAG_H__
/*
extern unsigned int
soc_pci_write
(
	unsigned int unit,
	unsigned int addr,
	unsigned int data
);

extern unsigned int
soc_pci_read
(
	unsigned int unit,
	unsigned int addr
);
*/
typedef struct unimac_data_s {
	int rw;
	int type;
	int unit;
	int port;
	unsigned int data;
}unimac_data_t;

typedef enum{
	COOMAND_CONFIG,	/*0*/
	GPORT_CONFIG,
	MAC_0,
	MAC_1,
	FRM_LENGTH,
	PAUSE_QUNAT,
	SFD_OFFSET,
	MAC_MODE,
	TAG_0,
	TAG_1,
	TX_IPG_LENGTH,
	PAUSE_CONTROL,
	IPG_HD_BKP_CNTL,
	FLUSH_CONTROL,
	RXFIFO_STAT,
	TXFIFO_STAT,
	GPORT_RSV_MASK,
	GPORT_STAT_UPDATE_MASK,
	GPORT_TPID,
	GPORT_SOP_S1,
	GPORT_SOP_S0,
	GPORT_SOP_S3,
	GPORT_SOP_S4,
	GPORT_MAC_CRS_SEL
}unimac_type;

int nam_asic_phy_read
(
	unsigned int opDevice,
	unsigned char opPort,
	unsigned char opRegaddr,
	unsigned short *regValue
);

int nam_asic_phy_write
(
	unsigned int opDevice,
	unsigned char opPort,
	unsigned char opRegaddr,
	unsigned short regValue
);

/*******************************************************************************
* nam_asic_auxi_phy_read
*
* DESCRIPTION:
*       Read specified 10G SMI Register and PHY device of specified port
*       on specified device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       phyId   - ID of external 10G PHY (value of 0..31).
*       useExternalPhy - boolean variable, defines if to use external 10G PHY
*       phyReg  - 10G SMI register, the register of PHY to read from
*       phyDev  - the PHY device to read from (value of 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       DIAG_RETURN_CODE_SUCCESS          - on success
*       DIAG_RETURN_CODE_ERROR		- on fail.
*
*******************************************************************************/
unsigned int nam_asic_auxi_phy_read
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char phyId,
	unsigned char useExternalPhy,
	unsigned short phyReg,
	unsigned char phyDev,
	unsigned short *dataPtr
);

/*******************************************************************************
* nam_asic_auxi_phy_write
*
* DESCRIPTION:
*       Write value to a specified 10G SMI Register and PHY device of
*       specified port on specified device.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyId       - ID of external 10G PHY (value of 0...31).
*       useExternalPhy - Boolean variable, defines if to use external 10G PHY
*       phyReg      - 10G SMI register, the register of PHY to read from
*       phyDev      - the PHY device to read from (value of 0..31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       DIAG_RETURN_CODE_SUCCESS            - on success
*       DIAG_RETURN_CODE_ERROR		- on fail.
*
*******************************************************************************/
int nam_asic_auxi_phy_write
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char phyId,
	unsigned char useExternalPhy,
	unsigned short phyReg,
	unsigned char phyDev,
	unsigned short dataPtr
);

int nam_asic_pci_read
(
	unsigned int opDevice,
	unsigned int opRegaddr,
	unsigned int *regValue
);

int nam_asic_pci_write
(
	unsigned int opDevice,
	unsigned int opRegaddr,
	unsigned int regValue
);

int nam_board_cpu_read_reg
(
	unsigned int regAddr,
	unsigned int *regValue
);

int nam_board_cpu_write_reg
(
	unsigned int regAddr,
	unsigned int regValue
);

int nam_unimac_data_check_br
(
	unimac_data_t* unimac_data, 
	unsigned int * dataret
);

int nam_vct_phy_enable
(
	unsigned int opDevice,
	unsigned char opPort
);

int nam_vct_phy_read
(
	unsigned int opDevice,
	unsigned char opPort,
	unsigned short *state,
	unsigned int *len
);

#ifdef DRV_LIB_CPSS
#define IN
#define OUT
/*******************************************************************************
* cpssDxChPhyPortSmiRegisterRead
*
* DESCRIPTION:
*       Read specified SMI Register on a specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       phyReg    - SMI register (value 0..31)
*
* OUTPUTS:
*       dataPtr   - (pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - Flex port SMI or PHY address were not set
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned long cpssDxChPhyPortSmiRegisterRead
(
    IN  unsigned char     devNum,
    IN  unsigned char     portNum,
    IN  unsigned char     phyReg,
    OUT unsigned short    *dataPtr
);

/*******************************************************************************
* cpssDxChPhyPortSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyReg      - The new phy address, (value 0...31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - Flex port SMI or PHY address were not set
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned long cpssDxChPhyPortSmiRegisterWrite
(
    IN  unsigned char     devNum,
    IN  unsigned char     portNum,
    IN  unsigned char     phyReg,
    IN  unsigned short    data
);


#ifdef DRV_LIB_CPSS_3_4
/*******************************************************************************
* cpssDrvPpHwRegisterRead
*
* DESCRIPTION:
*       Read a register value from the given PP.
*
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*
* OUTPUTS:
*       data - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned long cpssDrvPpHwRegisterRead
(
    IN unsigned char   devNum,
    IN unsigned long   portGroupId,
    IN unsigned long   regAddr,
    OUT unsigned long  *data
);
/*******************************************************************************
* cpssDrvPpHwRegisterWrite
*
* DESCRIPTION:
*       Write to a PP's given register.
*
* INPUTS:
*       devNum  - The PP to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to write to.
*       data    - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned long cpssDrvPpHwRegisterWrite
(
    IN unsigned char   devNum,
    IN unsigned long   portGroupId,
    IN unsigned long   regAddr,
    IN unsigned long   value
);

/*******************************************************************************
* extDrvPciConfigWriteReg
*
* DESCRIPTION:
*       This routine read register to the CPU configuration register.
*
* INPUTS:
*       regAddr  - Register offset of CPU Register set
*
* OUTPUTS:
*       data     - data read from.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
extern int extDrvBoardCpuReadReg
(
    IN  unsigned int  regAddr,
    IN  unsigned int  *data
);

/*******************************************************************************
* extDrvBoardCpuWriteReg
*
* DESCRIPTION:
*       This routine write register from the CPU configuration register.
*
* INPUTS:
*       regAddr  - Register offset of CPU Register set
*       data     - data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
extern int extDrvBoardCpuWriteReg
(
    IN  unsigned int  regAddr,
    IN  unsigned int  data
);

#else
/*******************************************************************************
* cpssDrvPpHwRegisterRead
*
* DESCRIPTION:
*       Read a register value from the given PP.
*
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*
* OUTPUTS:
*       data - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned long cpssDrvPpHwRegisterRead
(
    IN unsigned char   devNum,
    IN unsigned long   regAddr,
    OUT unsigned long  *data
);
/*******************************************************************************
* cpssDrvPpHwRegisterWrite
*
* DESCRIPTION:
*       Write to a PP's given register.
*
* INPUTS:
*       devNum  - The PP to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to write to.
*       data    - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
extern unsigned long cpssDrvPpHwRegisterWrite
(
    IN unsigned char   devNum,
    IN unsigned long   regAddr,
    IN unsigned long   value
);
#endif
#endif
#endif
