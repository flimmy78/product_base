/**
  * Soft emulated I2C bus.
  * author: Autelan Tech.
  * codeby: baolc
  * 2008-07-15
  */
#ifndef _AX_SOFT_I2C_H_
#define _AX_SOFT_I2C_H_

#include "bmk_main.h"

#define  AX_SOFT_I2C_OK     0
#define  AX_SOFT_I2C_FAIL  -1


#define OCTEON_CPLD4_BASE_ADDR  0x1d040000
#define OCTEON_CPLD5_BASE_ADDR  0x1d050000 
#define OCTEON_CPLD6_BASE_ADDR  0x1d060000
#define OCTEON_CPLD7_BASE_ADDR  0x1d070000


#define  AX_SOFT_I2C_SLOT1_CPLD_BASE_ADDR  OCTEON_CPLD4_BASE_ADDR  /* CPLD's addr on slot1 board, 0x1d040000 */
#define  AX_SOFT_I2C_SLOT2_CPLD_BASE_ADDR  OCTEON_CPLD5_BASE_ADDR
#define  AX_SOFT_I2C_SLOT3_CPLD_BASE_ADDR  OCTEON_CPLD6_BASE_ADDR
#define  AX_SOFT_I2C_SLOT4_CPLD_BASE_ADDR  OCTEON_CPLD7_BASE_ADDR

//#define internal_address_8bit 1   //switch of 8bit read and write
#define internal_address_16bit 1  //switch of 16bit read and write

#ifdef internal_address_16bit
#define  AX_SOFT_I2C_CPLD_OFFSET_SCL  0x26  //bit0
#define  AX_SOFT_I2C_CPLD_OFFSET_DIR  0x27  //bit0, DIR=1: eeprom->cpld, DIR=0: cpld->eeprom
#define  AX_SOFT_I2C_CPLD_OFFSET_SDA  0x28  //bit0
#endif

#ifdef internal_address_8bit
#define  AX_SOFT_I2C_CPLD_OFFSET_SCL  0x23  //bit0
#define  AX_SOFT_I2C_CPLD_OFFSET_DIR  0x24  //bit0, DIR=1: eeprom->cpld, DIR=0: cpld->eeprom
#define  AX_SOFT_I2C_CPLD_OFFSET_SDA  0x25  //bit0
#endif

#if defined(CONFIG_SPI) || !defined(CFG_I2C_EEPROM_ADDR)
# define CFG_DEF_EEPROM_ADDR 0
#else
# define CFG_DEF_EEPROM_ADDR CFG_I2C_EEPROM_ADDR
#endif /* CONFIG_SPI || !defined(CFG_I2C_EEPROM_ADDR) */






/*************************SOFT I2C FUNCTIONS for i2c***************************/
/**
  * Function for write bytes to i2c.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * @param    slave_dev_addr_7bit:  slave device's 7 bit addr. ONLY  low 7 bit valid!
  * @param    data: data to be write to i2c.
  * @param    data_len: data length.
  * @return: AX_SOFT_I2C_OK(0) for success. AX_SOFT_I2C_FAIL(-1) for failed.
  */
int ax_soft_i2c_write_bytes(int slot_num, uint8_t slave_dev_addr_7bit, uint8_t* data, int data_len);

/**
  * Function for read bytes from i2c.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * @param    slave_dev_addr:  slave device's 7 bit addr. ONLY  low 7 bit valid!
  * @param    data: buffer to be put data which is read from i2c.
  * @param    data_len: the data length to be read.
  * @return: AX_SOFT_I2C_OK(0) for success. AX_SOFT_I2C_FAIL(-1) for failed.
  */
int ax_soft_i2c_read_bytes(int slot_num, uint8_t slave_dev_addr_7bit, uint8_t* data, int data_len);



/*************************SOFT I2C FUNCTIONS api for read/write through i2c bus***************************/
/**
  * Set internal 16 bit addr in slave device. For some eeproms which have more than 8 bit's internal addresses.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_set_addr16().
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_set_addr16(int slot_num, uint8_t slave_dev_addr, uint16_t internal_addr);

/**
  * Set internal 8 bit addr in slave device. For devices which have 8 bit internal addresses.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_set_addr8().
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_set_addr8(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr);
int ax_soft_i2c_set_addr8_8bit(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr, uint8_t port_number);

/**
  * Read 8 bit data from the current internal addr in slave device. The cur addr should be set before this operation.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_read8_cur_addr().
  * @param: data: output.
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_read8_cur_addr(int slot_num, uint8_t slave_dev_addr, uint8_t* data);

/**
  * Read 8 bit data from a random 16 bit internal addr in slave device. 
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_read16().
  * @param: data: output.
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_read16(int slot_num, uint8_t slave_dev_addr, uint16_t internal_addr, uint16_t* data);


/**
  * Read 8 bit data from a random 8 bit internal addr in slave device. 
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_read8().
  * @param: data: output.
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_read8(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr, uint8_t* data);
int ax_soft_i2c_read8_8bit(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr, uint8_t* data, int port_number, int buf_len);



/**
  * Write 16 bit data to slave device.  When internal addr is 16 bit should use this function.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_write16().
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_write16(int slot_num, uint8_t slave_dev_addr, uint16_t internal_addr, uint16_t data);

/**
  * Write 8 bit data to slave device. When internal addr is 8 bit should use this function.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_write8().
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_write8(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr, uint8_t data);
int ax_soft_i2c_write8_8bit(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr, uint8_t *data, uint8_t port_number, int buf_len);


/*************************SOFT I2C FUNCTIONS api for read/write EEPROM(only support EEPROM which has 16bit internal address)***************************/
/**
 * Reads bytes from eeprom and copies to DRAM.
 * Only supports address size of 2 (16 bit internal address.)
 * 
 * @param slot_num  boart's slot number
 * @param chip_addr   chip address
 * @param internal_addr   internal address (only 16 bit addresses supported)
 * @param buffer_in_dram memory buffer pointer
 * @param len    number of bytes to read
 * 
 * @return 0 on Success
 *         -1 on Failure
 *
 * NOTE: similar to i2c_read() function.
 */
int  ax_soft_i2c_read16_eeprom(int slot_num, uint8_t chip_addr, uint16_t internal_addr, uint8_t *buffer_in_dram, int len);


/**
 * Reads bytes from eeprom and copies to DRAM.
 * Only supports address size of 1 (8 bit internal address.)
 * 
 * @param slot_num  boart's slot number
 * @param chip_addr   chip address
 * @param internal_addr   internal address (only 8 bit addresses supported)
 * @param buffer_in_dram memory buffer pointer
 * @param len    number of bytes to read
 * 
 * @return 0 on Success
 *         -1 on Failure
 *
 * NOTE: similar to i2c_read() function.
 */
int  ax_soft_i2c_read8_eeprom(int slot_num, uint8_t chip_addr, uint8_t internal_addr, uint8_t *buffer_in_dram, int len);



/**
 * Reads bytes from memory and copies to eeprom.
 * Only supports address size of 2 (16 bit internal address.)
 * 
 * We can only write two bytes at a time to the eeprom, so in some cases
 * we need to to a read/modify/write.
 * 
 * Note: can't do single byte write to last address in EEPROM
 * 
 * @param slot_num  board's slot number
 * @param chip_addr   chip address
 * @param internal_addr   internal address (only 16 bit addresses supported)
 * @param buffer_in_dram memory buffer pointer
 * @param len    number of bytes to write
 * 
 * @return 0 on Success
 *         -1 on Failure
 * 
 * NOTE: similar to i2c_write() function.
 */
int  ax_soft_i2c_write16_eeprom(int slot_num, uint8_t chip_addr, uint16_t internal_addr, uint8_t *buffer_in_dram, int len);


/**
 * Reads bytes from memory and copies to eeprom.
 * Only supports address size of 1 (8 bit internal address.)
 * 
 * We can only write two bytes at a time to the eeprom, so in some cases
 * we need to to a read/modify/write.
 * 
 * Note: can't do single byte write to last address in EEPROM
 * 
 * @param slot_num  board's slot number
 * @param chip_addr   chip address
 * @param internal_addr   internal address (only 8 bit addresses supported)
 * @param buffer_in_dram memory buffer pointer
 * @param len    number of bytes to write
 * 
 * @return 0 on Success
 *         -1 on Failure
 *
 * NOTE: similar to i2c_write() function.
 */
int  ax_soft_i2c_write8_eeprom(int slot_num, uint8_t chip_addr, uint8_t internal_addr, uint8_t *buffer_in_dram, int len);


#endif
