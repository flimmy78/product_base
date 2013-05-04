/**
  * Soft emulated I2C bus.
  * author: Autelan Tech.
  * codeby: baolc
  * 2008-07-15
  */
#include <net/sctp/command.h>
#include <asm/delay.h>
#include <linux/init.h>  
#include <linux/string.h>

#include "ax_soft_i2c.h"
#include "bmk_main.h"

#define _AX_SOFT_I2C_DEBUG_




#ifdef _AX_SOFT_I2C_DEBUG_
    #define  ASI_DEBUG  printk
#else
    #define  ASI_DEBUG  
#endif

extern unsigned char _ctype[];

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define _D	0x04	/* digit */
#define _X	0x40	/* hex digit */
#define _L	0x02	/* lower */

#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isdigit(c)	((__ismask(c)&(_D)) != 0)
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)
#define toupper(c)  __toupper(c)
#define islower(c)	((__ismask(c)&(_L)) != 0)

//#define ASI_DEBUG_LINE  //printk



typedef enum 
{
	AX_LOW = 0,
	AX_HIGH = 1,
} AX_HIGH_OR_LOW;

typedef struct cmd_tbl_s cmd_tbl_t;

/**
  * Typedef for manipulate CPLD reg's data value.
  */
  /*
typedef union
{
	uint8_t  u8;
	struct 
	{
		uint8_t  other_fields_1_7    : 7;
		uint8_t  lsb                 : 1;
	} s;
} ax_soft_i2c_cpld_reg_8bit;
*/
/**
 *
 */
typedef union
{
	uint8_t  u8;
	struct 
	{
		uint8_t  other_fields_1_2	 : 2;
		uint8_t  lsb6	 			 : 1;
		uint8_t  lsb5	 			 : 1;
		uint8_t  lsb4	 			 : 1;
		uint8_t  lsb3	 			 : 1;
		uint8_t  lsb2	 			 : 1;
		uint8_t  lsb				 : 1;
	} s;
} ax_soft_i2c_cpld_reg_8bit;

static	uint64_t	base_address64 = (1ull << 63);




/*************************SOFT I2C TOOLS***************************/
/**
  * Soft i2c Delay.
  */
/*
static void AX_SOFT_I2C_DELAY(int cycles)
{
	int i;
	while ((--cycles) > 0)
		i++;
}
*/
static void AX_SOFT_I2C_DELAY(int us)
{
	udelay(us);
}

static inline unsigned char __toupper(unsigned char c)
{
	if (islower(c))
		c -= 'a'-'A';
	return c;
}


/* octeon_write64_byte and octeon_read64_byte are only used by the debugger stub.
** The debugger will generate KSEG0 addresses that are not in the 64 bit compatibility
** space, so we detect that and fix it up.  This should probably be addressed in the 
** debugger itself, as this fixup makes some valid 64 bit address inaccessible.
*/
#define DO_COMPAT_ADDR_FIX
void octeon_write64_byte(uint64_t csr_addr, uint8_t val)
{

    volatile uint32_t addr_low  = csr_addr & 0xffffffff;
    volatile uint32_t addr_high = csr_addr  >> 32;
    
    if (!addr_high && (addr_low & 0x80000000))
    {
#ifdef DO_COMPAT_ADDR_FIX
        addr_high = ~0;
#endif
#if 0
        char tmp_str[500];
        sprintf(tmp_str, "fixed read64_byte at low  addr: 0x%x\n", addr_low);
        simprintf(tmp_str);
        sprintf(tmp_str, "fixed read64_byte at high addr: 0x%x\n", addr_high);
        simprintf(tmp_str);
#endif

    }

    asm volatile (
      "  .set push                         \n"
      "  .set mips64                       \n"
      "  .set noreorder                    \n"
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[addrh], 32                 \n"
      "  dsll  %[addrl], 32          \n"
      "  dsrl  %[addrl], 32          \n"
      "  daddu %[addrh], %[addrh], %[addrl]   \n"
      /* Combined value is in addrh */
      "  sb %[val], 0(%[addrh])   \n"
      "  .set pop                          \n"
      : 
      :[val] "r" (val), [addrh] "r" (addr_high), [addrl] "r" (addr_low)
	  : "memory");
}


static uint8_t octeon_read64_byte(uint64_t csr_addr)
{
    
    uint8_t val;

    uint32_t addr_low  = csr_addr & 0xffffffff;
    uint32_t addr_high = csr_addr  >> 32;
    if (!addr_high && (addr_low & 0x80000000))
    {
#ifdef DO_COMPAT_ADDR_FIX
        addr_high = ~0;
#endif
#if 0
        char tmp_str[500];
        sprintf(tmp_str, "fixed read64_byte at low  addr: 0x%x\n", addr_low);
        simprintf(tmp_str);
        sprintf(tmp_str, "fixed read64_byte at high addr: 0x%x\n", addr_high);
        simprintf(tmp_str);
#endif

    }
    asm volatile (
                  "  .set push                         \n"
                  "  .set mips64                       \n"
                  "  .set noreorder                    \n"
                  /* Standard twin 32 bit -> 64 bit construction */
                  "  dsll  %[addrh], 32                 \n"
                  "  dsll  %[addrl], 32          \n"
                  "  dsrl  %[addrl], 32          \n"
                  "  daddu %[addrh], %[addrh], %[addrl]   \n"
                  /* Combined value is in addrh */
                  "  lb    %[val], 0(%[addrh])   \n"
                  "  .set pop                          \n"
                  :[val] "=&r" (val) 
                  : [addrh] "r" (addr_high), [addrl] "r" (addr_low)
                  : "memory"
                 );

    return(val);

}
								//arg null 10
unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
{
	unsigned long result = 0,value;

	if (*cp == '0') 
	{
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) 
		{
			base = 16;
			cp++;
		}
		if (!base) 
		{
			base = 8;
		}
	}
	if (!base) 
	{
		base = 10;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
	    ? toupper(*cp) : *cp)-'A'+10) < base) 
	{
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}


/**
  * Set SCL line 0 or 1.
  */
static void AX_SOFT_I2C_SCL(AX_HIGH_OR_LOW high_or_low, uint64_t cpld_base_addr, int is_delay)
{
	ax_soft_i2c_cpld_reg_8bit csr_value;
	csr_value.u8 = octeon_read64_byte(base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_SCL); //NOTE: this call convert 32bit address to 64bit address!!
	//ASI_DEBUG_LINE("scl line cpld addr: 0x%px, high-or-low: %d\n", base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_SCL, high_or_low);
	csr_value.s.lsb = high_or_low;
	octeon_write64_byte(base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_SCL, csr_value.u8); //NOTE: this call convert 32bit address to 64bit address!!
	if (is_delay)
		AX_SOFT_I2C_DELAY(100);
}

static void AX_SOFT_I2C_SCL_8BIT(uint8_t port_number, AX_HIGH_OR_LOW high_or_low, uint64_t cpld_base_addr, int is_delay)
{
	ax_soft_i2c_cpld_reg_8bit csr_value;
	csr_value.u8 = octeon_read64_byte(base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_SCL); //NOTE: this call convert 32bit address to 64bit address!!
	//ASI_DEBUG_LINE("scl line cpld addr: 0x%px, high-or-low: %d\n", base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_SCL, high_or_low);
	//csr_value.s.lsb = high_or_low;
	csr_value.u8 = 0xff;
	switch (port_number)
	{
		case 1:
			csr_value.s.lsb = high_or_low;
			break;
		case 2:
			csr_value.s.lsb2 = high_or_low;
			break;
		case 3:
			csr_value.s.lsb3 = high_or_low;
			break;
		case 4:
			csr_value.s.lsb4 = high_or_low;
			break;
		case 5:
			csr_value.s.lsb5 = high_or_low;
			break;
		case 6:
			csr_value.s.lsb6 = high_or_low;
			break;
		default:
			break;
	}
	octeon_write64_byte(base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_SCL, csr_value.u8); //NOTE: this call convert 32bit address to 64bit address!!
	if (is_delay)
		AX_SOFT_I2C_DELAY(100);
}


/**
  * Set SDA line 0 or 1.
  */
static void AX_SOFT_I2C_SDA(AX_HIGH_OR_LOW high_or_low, uint64_t cpld_base_addr, int is_delay)
{
	ax_soft_i2c_cpld_reg_8bit csr_value;
	csr_value.u8 = octeon_read64_byte(base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_SDA); //NOTE: this call convert 32bit address to 64bit address!!
	//ASI_DEBUG_LINE("sda line cpld addr: 0x%qx, high-or-low: %d\n", base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_SDA, high_or_low);
	csr_value.s.lsb = high_or_low;
	octeon_write64_byte(base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_SDA, csr_value.u8); //NOTE: this call convert 32bit address to 64bit address!!
	if (is_delay)
		AX_SOFT_I2C_DELAY(100);
}


/**
  * Set DIR line 0 or 1. Used for CPLD to emulate SDA's data direction.
  * DIR=0 for CPLD->EEPROM (CPU write)
  * DIR=1 for EEPROM->CPLD (CPU read)
  */
static void AX_SOFT_I2C_DIR(AX_HIGH_OR_LOW high_or_low, uint64_t cpld_base_addr, int is_delay)
{
	ax_soft_i2c_cpld_reg_8bit csr_value;
	csr_value.u8 = octeon_read64_byte(base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_DIR); //NOTE: this call convert 32bit address to 64bit address!!
	csr_value.s.lsb = high_or_low;
	octeon_write64_byte(base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_DIR, csr_value.u8); //NOTE: this call convert 32bit address to 64bit address!!
	if (is_delay)
		AX_SOFT_I2C_DELAY(50);
}


/**
  * Read from SCL line.
  */
static AX_HIGH_OR_LOW AX_SOFT_I2C_READ_FROM_SCL(uint64_t cpld_base_addr)
{
	ax_soft_i2c_cpld_reg_8bit csr_value;
	csr_value.u8 = octeon_read64_byte(base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_SCL); //NOTE: this call convert 32bit address to 64bit address!!
	return csr_value.s.lsb;
}


/**
  * Read from SDA line.
  */
static AX_HIGH_OR_LOW AX_SOFT_I2C_READ_FROM_SDA(uint64_t cpld_base_addr)
{
	ax_soft_i2c_cpld_reg_8bit csr_value;
	csr_value.u8 = octeon_read64_byte(base_address64 + cpld_base_addr + AX_SOFT_I2C_CPLD_OFFSET_SDA); //NOTE: this call convert 32bit address to 64bit address!!
	return csr_value.s.lsb;
}

/**
  * Convert slot_num to CPLD base addr. Addr is actually 32bit currently, it's under the boot bus address region.
  */
static uint64_t AX_SOFT_I2C_CONVERT_SLOT_NUM_TO_CPLD_ADDR(int slot_num)
{
	switch (slot_num)
	{
	case 1:
		return AX_SOFT_I2C_SLOT1_CPLD_BASE_ADDR;
		break;
	case 2:
		return AX_SOFT_I2C_SLOT2_CPLD_BASE_ADDR;
		break;
	case 3:
		return AX_SOFT_I2C_SLOT3_CPLD_BASE_ADDR;
		break;
	case 4:
		return AX_SOFT_I2C_SLOT4_CPLD_BASE_ADDR;
		break;
	default:
		return AX_SOFT_I2C_SLOT1_CPLD_BASE_ADDR; /* default use slot1's cpld */
		break;
	}
}



/*************************SOFT I2C MANIPULATE***************************/
/**
  * I2C start.
  */
static void ax_soft_i2c_start(uint64_t cpld_base_addr)
{
	AX_SOFT_I2C_DIR(0, cpld_base_addr, 1); //DIR=0, cpu write
	//AX_SOFT_I2C_SCL(0, cpld_base_addr, 1); //SCL=0, SDA could be changed
	AX_SOFT_I2C_SDA(1, cpld_base_addr, 1); //SDA=1
	AX_SOFT_I2C_SCL(1, cpld_base_addr, 1); //SCL=1
	AX_SOFT_I2C_SDA(0, cpld_base_addr, 1); //SDA=0, sda jump to low, i2c start
	AX_SOFT_I2C_SCL(0, cpld_base_addr, 1); //SCL=0
}
static void ax_soft_i2c_start_8bit(uint64_t cpld_base_addr, uint8_t port_number)
{
	AX_SOFT_I2C_DIR(0, cpld_base_addr, 1); //DIR=0, cpu write
	//AX_SOFT_I2C_SCL(0, cpld_base_addr, 1); //SCL=0, SDA could be changed
	AX_SOFT_I2C_SDA(1, cpld_base_addr, 1); //SDA=1
	AX_SOFT_I2C_SCL_8BIT(port_number, 1, cpld_base_addr, 1); //SCL=1
	AX_SOFT_I2C_SDA(0, cpld_base_addr, 1); //SDA=0, sda jump to low, i2c start
	AX_SOFT_I2C_SCL_8BIT(port_number, 0, cpld_base_addr, 1); //SCL=0
}
#define ax_soft_i2c_restart_8bit ax_soft_i2c_start_8bit
#define ax_soft_i2c_restart  ax_soft_i2c_start //restart is the same as start

/**
  * I2C stop.
  */
static void ax_soft_i2c_stop(uint64_t cpld_base_addr)
{
	AX_SOFT_I2C_DIR(0, cpld_base_addr, 1); //DIR=0, cpu write
	AX_SOFT_I2C_SCL(0, cpld_base_addr, 1); //SCL=0, SDA could be changed
	AX_SOFT_I2C_SDA(0, cpld_base_addr, 1); //SDA=0
	AX_SOFT_I2C_SCL(1, cpld_base_addr, 1); //SCL=1
	AX_SOFT_I2C_SDA(1, cpld_base_addr, 1); //SDA=1, sda jump to high, i2c stop
}

static void ax_soft_i2c_stop_8bit(uint64_t cpld_base_addr, uint8_t port_number)
{
	AX_SOFT_I2C_DIR(0, cpld_base_addr, 1); //DIR=0, cpu write
	AX_SOFT_I2C_SCL_8BIT(port_number, 0, cpld_base_addr, 1); //SCL=0, SDA could be changed
	AX_SOFT_I2C_SDA(0, cpld_base_addr, 1); //SDA=0
	AX_SOFT_I2C_SCL_8BIT(port_number, 1, cpld_base_addr, 1); //SCL=1
	AX_SOFT_I2C_SDA(1, cpld_base_addr, 1); //SDA=1, sda jump to high, i2c stop
}


/**
  * Soft I2C send byte as master-sender.
  * @return: AX_SOFT_I2C_OK(0) for success. AX_SOFT_I2C_FAIL(-1) for failed.
  */
static int ax_soft_i2c_send_byte(uint8_t data, uint64_t cpld_base_addr)
{
	int i;
	//send byte data
	AX_SOFT_I2C_SCL(0, cpld_base_addr, 1); //SCL=0, SDA chould be changed
	AX_SOFT_I2C_DIR(0, cpld_base_addr, 1); //DIR=0, cpu write
	printk("be about to sent 0x%x\n", data);
	for (i=7; i>=0; i--)
	{
		AX_SOFT_I2C_SCL(0, cpld_base_addr, 1); //SCL=0, SDA chould be changed
		if (data & (0x1<<i))
		{
			AX_SOFT_I2C_SDA(1, cpld_base_addr, 1); //bit is high
		}
		else
		{
			AX_SOFT_I2C_SDA(0, cpld_base_addr, 1); //bit is low
		}
		AX_SOFT_I2C_SCL(1, cpld_base_addr, 1); //SCL=1, SDA data valid
	}
	//receive ACK
	AX_SOFT_I2C_SCL(0, cpld_base_addr, 0); //SCL=0, SDA chould be changed, no delay
	AX_SOFT_I2C_DIR(1, cpld_base_addr, 1); //DIR=1, cpu read
	AX_SOFT_I2C_SCL(1, cpld_base_addr, 1); //SCL=1, SDA data valid
	if (AX_SOFT_I2C_READ_FROM_SDA(cpld_base_addr) != 0)
	{
		AX_SOFT_I2C_SCL(0, cpld_base_addr, 0); //SCL=0
		return AX_SOFT_I2C_FAIL; //ACK=1, failed
	}
	else
	{
		AX_SOFT_I2C_SCL(0, cpld_base_addr, 0); //SCL=0
		return AX_SOFT_I2C_OK; //ACK=0, success
	}
}

static int ax_soft_i2c_send_byte_8bit(uint8_t data, uint64_t cpld_base_addr, uint8_t port_number)
{
	int i;
	//send byte data
	AX_SOFT_I2C_SCL_8BIT(port_number, 0, cpld_base_addr, 1); //SCL=0, SDA chould be changed
	AX_SOFT_I2C_DIR(0, cpld_base_addr, 1); //DIR=0, cpu write
	for (i=7; i>=0; i--)
	{
		AX_SOFT_I2C_SCL_8BIT(port_number, 0, cpld_base_addr, 1); //SCL=0, SDA chould be changed
		if (data & (0x1<<i))
		{
			AX_SOFT_I2C_SDA(1, cpld_base_addr, 1); //bit is high
		}
		else
		{
			AX_SOFT_I2C_SDA(0, cpld_base_addr, 1); //bit is low
		}
		AX_SOFT_I2C_SCL_8BIT(port_number, 1, cpld_base_addr, 1); //SCL=1, SDA data valid
	}
	//receive ACK
	AX_SOFT_I2C_SCL_8BIT(port_number, 0, cpld_base_addr, 0); //SCL=0, SDA chould be changed, no delay
	AX_SOFT_I2C_DIR(1, cpld_base_addr, 1); //DIR=1, cpu read
	AX_SOFT_I2C_SCL_8BIT(port_number, 1, cpld_base_addr, 1); //SCL=1, SDA data valid
	if (AX_SOFT_I2C_READ_FROM_SDA(cpld_base_addr) != 0)
	{
		AX_SOFT_I2C_SCL_8BIT(port_number, 0, cpld_base_addr, 0); //SCL=0
		return AX_SOFT_I2C_FAIL; //ACK=1, failed
	}
	else
	{
		AX_SOFT_I2C_SCL_8BIT(port_number, 0, cpld_base_addr, 0); //SCL=0
		return AX_SOFT_I2C_OK; //ACK=0, success
	}
}


/**
  * Soft I2C receive byte as master-receiver.
  * @param  is_last_byte:  if is last byte, then should send NO_ACK.
  * @return: AX_SOFT_I2C_OK(0) for success. AX_SOFT_I2C_FAIL(-1) for failed.
  */
static void ax_soft_i2c_receive_byte(uint8_t* data, int is_last_byte, uint64_t cpld_base_addr)
{
	int i;
	uint8_t tmp = 0;
	//receive data byte
	AX_SOFT_I2C_SCL(0, cpld_base_addr, 0); //SCL=0, SDA chould be changed, no delay
	AX_SOFT_I2C_DIR(1, cpld_base_addr, 1); //DIR=1, cpu read
	for (i=7; i>=0; i--)
	{
		AX_SOFT_I2C_SCL(1, cpld_base_addr, 1); //SCL=1, SDA data valid, receive data from slave
		tmp |= (AX_SOFT_I2C_READ_FROM_SDA(cpld_base_addr) << i); //read from SDA
		AX_SOFT_I2C_SCL(0, cpld_base_addr, 1); //SCL=0, SDA chould be changed
	}
	*data = tmp;
	//send ACK or NO_ACK
	//SCL already=0, SDA chould be changed
	AX_SOFT_I2C_DIR(0, cpld_base_addr, 1); //DIR=0, cpu write
	if (is_last_byte)
		AX_SOFT_I2C_SDA(1, cpld_base_addr, 1); //is last byte, SDA=1, send NO_ACK
    else
		AX_SOFT_I2C_SDA(0, cpld_base_addr, 1); //is not last byte, SDA=0, send ACK
	AX_SOFT_I2C_SCL(1, cpld_base_addr, 1); //SCL=1, SDA data valid, send ACK or NO_ACK
	AX_SOFT_I2C_SCL(0, cpld_base_addr, 1); //SCL=0
	return;
}

static void ax_soft_i2c_receive_byte_8bit(uint8_t *data, int is_last_byte, uint64_t cpld_base_addr, uint8_t port_number)
{
	int i;
	uint8_t tmp = 0;
	//receive data byte
	AX_SOFT_I2C_SCL_8BIT(port_number, 0, cpld_base_addr, 0); //SCL=0, SDA chould be changed, no delay
	AX_SOFT_I2C_DIR(1, cpld_base_addr, 1); //DIR=1, cpu read
	for (i=7; i>=0; i--)
	{
		AX_SOFT_I2C_SCL_8BIT(port_number, 1, cpld_base_addr, 1); //SCL=1, SDA data valid, receive data from slave
		tmp |= (AX_SOFT_I2C_READ_FROM_SDA(cpld_base_addr) << i); //read from SDA
		AX_SOFT_I2C_SCL_8BIT(port_number, 0, cpld_base_addr, 1); //SCL=0, SDA chould be changed
	}
	*data = tmp;
	//send ACK or NO_ACK
	//SCL already=0, SDA chould be changed
	AX_SOFT_I2C_DIR(0, cpld_base_addr, 1); //DIR=0, cpu write
	if (is_last_byte)
		AX_SOFT_I2C_SDA(1, cpld_base_addr, 1); //is last byte, SDA=1, send NO_ACK
    else
		AX_SOFT_I2C_SDA(0, cpld_base_addr, 1); //is not last byte, SDA=0, send ACK
	AX_SOFT_I2C_SCL_8BIT(port_number, 1, cpld_base_addr, 1); //SCL=1, SDA data valid, send ACK or NO_ACK
	AX_SOFT_I2C_SCL_8BIT(port_number, 0, cpld_base_addr, 1); //SCL=0
	return;
}


/*************************SOFT I2C FUNCTIONS for i2c***************************/
/**
  * Function for write bytes to i2c.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * @param    slave_dev_addr_7bit:  slave device's 7 bit addr. ONLY  low 7 bit valid!
  * @param    data: data to be write to i2c.
  * @param    data_len: data length.
  * @return: AX_SOFT_I2C_OK(0) for success. AX_SOFT_I2C_FAIL(-1) for failed.
  */
int ax_soft_i2c_write_bytes(int slot_num, uint8_t slave_dev_addr_7bit, uint8_t* data, int data_len)
{
	int i;
	uint64_t cpld_base_addr;
	uint8_t first_byte = 0; //high 7 bit is addr, LSB=0 indicates write operation
	first_byte |= (slave_dev_addr_7bit << 1);
	first_byte &= ~((uint8_t)0x1); //LSB=0, write operation
	
	if (data == NULL || data_len <= 0 || slot_num < 1 || slot_num > 4)
	{
		ASI_DEBUG("ax_soft_i2c_write_bytes: param invalid!\n");
		return AX_SOFT_I2C_FAIL;
	}
	
	cpld_base_addr = AX_SOFT_I2C_CONVERT_SLOT_NUM_TO_CPLD_ADDR(slot_num);
	ASI_DEBUG("ax_soft_i2c_write_bytes: now begin to write data to i2c, slot num is %d, dev addr is %#x, data length is %d...\n", slot_num, slave_dev_addr_7bit, data_len);
	ax_soft_i2c_start(cpld_base_addr); //start i2c
    if (ax_soft_i2c_send_byte(first_byte, cpld_base_addr) != AX_SOFT_I2C_OK) //write dev_addr and op code
    {
		ASI_DEBUG("ax_soft_i2c_write_bytes: write first byte failed, no ACK...\n");
		return AX_SOFT_I2C_FAIL;
    }
	
	for (i=0; i<data_len; i++)
	{
		if (ax_soft_i2c_send_byte(*(data+i), cpld_base_addr) != AX_SOFT_I2C_OK)
		{
			ASI_DEBUG("ax_soft_i2c_write_bytes: write data byte%d(%#x) failed, no ACK...\n", i, *(data+i));
			return AX_SOFT_I2C_FAIL;
		}
		ASI_DEBUG("ax_soft_i2c_write_bytes: write data byte%d(%#x) ok...\n", i, *(data+i));
	}
	
	ax_soft_i2c_stop(cpld_base_addr); //stop i2c
	AX_SOFT_I2C_DELAY(9000);
	return AX_SOFT_I2C_OK;
}

int ax_soft_i2c_write_bytes_8bit(int slot_num, uint8_t slave_dev_addr_7bit, uint8_t* data, int data_len, uint8_t port_number)
{
	int i;
	uint64_t cpld_base_addr;
	uint8_t first_byte = 0; //high 7 bit is addr, LSB=0 indicates write operation

	//printk("slot_num = %x\n", slot_num);
	//printk("slave_dev_addr_7bit = %x\n", slave_dev_addr_7bit);
	//printk("internal_addr = %x\n", *data);
	//printk("port_number = %x\n", port_number);
	//printk("AX_SOFT_I2C_CPLD_OFFSET_SCL = %x\n", AX_SOFT_I2C_CPLD_OFFSET_SCL);

	first_byte |= (slave_dev_addr_7bit << 1);
	first_byte &= ~((uint8_t)0x1); //LSB=0, write operation
	
	if (data == NULL || data_len <= 0 || slot_num < 1 || slot_num > 4)
	{
		ASI_DEBUG("ax_soft_i2c_write_bytes_8bit: param invalid!\n");
		return AX_SOFT_I2C_FAIL;
	}
	cpld_base_addr = AX_SOFT_I2C_CONVERT_SLOT_NUM_TO_CPLD_ADDR(slot_num);
	ASI_DEBUG("ax_soft_i2c_write_bytes_8bit: now begin to write data to i2c, slot num is %d, dev addr is %#x, data length is %d...\n", slot_num, slave_dev_addr_7bit, data_len);
	ax_soft_i2c_start_8bit(cpld_base_addr, port_number); //start i2c
    if (ax_soft_i2c_send_byte_8bit(first_byte, cpld_base_addr, port_number) != AX_SOFT_I2C_OK) //write dev_addr and op code
    {
		ASI_DEBUG("ax_soft_i2c_write_bytes_8bit: write first byte failed, no ACK...\n");
		return AX_SOFT_I2C_FAIL;
    }
	for (i=0; i<data_len; i++)
	{
		if (ax_soft_i2c_send_byte_8bit(*(data+i), cpld_base_addr, port_number) != AX_SOFT_I2C_OK)
		{
			ASI_DEBUG("ax_soft_i2c_write_bytes_8bit: write data byte%d(%#x) failed, no ACK...\n", i, *(data+i));
			return AX_SOFT_I2C_FAIL;
		}
		ASI_DEBUG("ax_soft_i2c_write_bytes_8bit: write data byte%d(%#x) ok...\n", i, *(data+i));
	}
	ax_soft_i2c_stop_8bit(cpld_base_addr, port_number); //stop i2c
	return AX_SOFT_I2C_OK;
}


/**
  * Function for read bytes from i2c.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * @param    slave_dev_addr:  slave device's 7 bit addr. ONLY  low 7 bit valid!
  * @param    data: buffer to be put data which is read from i2c.
  * @param    data_len: the data length to be read.
  * @return: AX_SOFT_I2C_OK(0) for success. AX_SOFT_I2C_FAIL(-1) for failed.
  */
int ax_soft_i2c_read_bytes(int slot_num, uint8_t slave_dev_addr_7bit, uint8_t* data, int data_len)
{
	int i;
	uint64_t cpld_base_addr;
	uint8_t first_byte = 0; //high 7 bit is addr, LSB=1 indicates read operation
	first_byte |= (slave_dev_addr_7bit << 1);
	first_byte |= ((uint8_t)0x1); //LSB=1, read operation
	if (data == NULL || data_len <= 0 || slot_num < 1 || slot_num > 4)
	{
		ASI_DEBUG("ax_soft_i2c_read_bytes: param invalid!\n");
		return AX_SOFT_I2C_FAIL;
	}
	cpld_base_addr = AX_SOFT_I2C_CONVERT_SLOT_NUM_TO_CPLD_ADDR(slot_num);
	ASI_DEBUG("ax_soft_i2c_read_bytes: now begin to read data from i2c, slot num is %d, dev addr is %#x, data length is %d...\n", slot_num, slave_dev_addr_7bit, data_len);
	ax_soft_i2c_start(cpld_base_addr); //start i2c
    if (ax_soft_i2c_send_byte(first_byte, cpld_base_addr) != AX_SOFT_I2C_OK) //write dev_addr and op code
    {
		ASI_DEBUG("ax_soft_i2c_read_bytes: write first byte failed, no ACK...\n");
		return AX_SOFT_I2C_FAIL;
    }
	for (i=0; i<data_len-1; i++) //receive (data_len-1)  bytes, the last byte is sent later
	{
		ax_soft_i2c_receive_byte(data+i, 0, cpld_base_addr);
		ASI_DEBUG("ax_soft_i2c_read_bytes: get byte %d, %#x\n", i, *(data+i));
	}
	ax_soft_i2c_receive_byte(data+data_len-1, 1, cpld_base_addr); //receive the last byte, send no ACK
	ax_soft_i2c_stop(cpld_base_addr); //stop i2c
	return AX_SOFT_I2C_OK;
}

int ax_soft_i2c_read_bytes_8bit(int slot_num, uint8_t slave_dev_addr_7bit, uint8_t* data, int data_len, uint8_t port_number)
{
	int i;
	uint64_t cpld_base_addr;
	uint8_t first_byte = 0; //high 7 bit is addr, LSB=1 indicates read operation
	first_byte |= (slave_dev_addr_7bit << 1);
	first_byte |= ((uint8_t)0x1); //LSB=1, read operation
	if (data == NULL || data_len <= 0 || slot_num < 1 || slot_num > 4)
	{
		ASI_DEBUG("ax_soft_i2c_read_bytes_8bit: param invalid!\n");
		return AX_SOFT_I2C_FAIL;
	}
	cpld_base_addr = AX_SOFT_I2C_CONVERT_SLOT_NUM_TO_CPLD_ADDR(slot_num);
	ASI_DEBUG("ax_soft_i2c_read_bytes_8bit: now begin to read data from i2c, slot num is %d, dev addr is %#x, data length is %d...\n", slot_num, slave_dev_addr_7bit, data_len);
	ax_soft_i2c_start_8bit(cpld_base_addr, port_number); //start i2c
    if (ax_soft_i2c_send_byte_8bit(first_byte, cpld_base_addr, port_number) != AX_SOFT_I2C_OK) //write dev_addr and op code
    {
		ASI_DEBUG("ax_soft_i2c_read_bytes_8bit: write first byte failed, no ACK...\n");
		return AX_SOFT_I2C_FAIL;
    }
	for (i=0; i<data_len-1; i++) //receive (data_len-1)  bytes, the last byte is sent later
	{
		ax_soft_i2c_receive_byte_8bit(data+i, 0, cpld_base_addr, port_number);
		ASI_DEBUG("ax_soft_i2c_read_bytes: get byte %d, %#x\n", i, *(data+i));
	}
	ax_soft_i2c_receive_byte_8bit(data+data_len-1, 1, cpld_base_addr, port_number); //receive the last byte, send no ACK
	ax_soft_i2c_stop_8bit(cpld_base_addr, port_number); //stop i2c
	return AX_SOFT_I2C_OK;
}


/*************************SOFT I2C FUNCTIONS api for read/write through i2c bus***************************/
/**
  * Set internal 16 bit addr in slave device. For some eeproms which have more than 8 bit's internal addresses.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_set_addr16().
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_set_addr16(int slot_num, uint8_t slave_dev_addr, uint16_t internal_addr)
{
	uint8_t  tmp[2];
	tmp[0] = (internal_addr >> 8) & 0xffff; //internal addr's high 8 bit
	tmp[1] = (internal_addr) & 0xffff; //internal addr's low 8 bit
	return ax_soft_i2c_write_bytes(slot_num, slave_dev_addr, tmp, 2);
}


/**
  * Set internal 8 bit addr in slave device. For devices which have 8 bit internal addresses.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_set_addr8().
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_set_addr8(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr)
{
	return ax_soft_i2c_write_bytes(slot_num, slave_dev_addr, &internal_addr, 1);
}

int ax_soft_i2c_set_addr8_8bit(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr, uint8_t port_number)
{
	return ax_soft_i2c_write_bytes_8bit(slot_num, slave_dev_addr, &internal_addr, 1, port_number);
}


/**
  * Read 8 bit data from the current internal addr in slave device. The cur addr should be set before this operation.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_read8_cur_addr().
  * @param: data: output.
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_read8_cur_addr(int slot_num, uint8_t slave_dev_addr, uint8_t* data)
{
	return ax_soft_i2c_read_bytes(slot_num, slave_dev_addr, data, 1);
}
int ax_soft_i2c_read8_cur_addr_8bit(int slot_num, uint8_t slave_dev_addr, uint8_t* data, uint8_t port_number)
{
	return ax_soft_i2c_read_bytes_8bit(slot_num, slave_dev_addr, data, 1, port_number);
}

/**
  * Read 8 bit data from a random 16 bit internal addr in slave device. 
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_read16().
  * @param: data: output.
  * @return: 0 for success. -1 for failure.
  */
//description: 
//firstly set the address secondly read data from current address
int ax_soft_i2c_read16(int slot_num, uint8_t slave_dev_addr, uint16_t internal_addr, uint16_t* data)
{
	if (ax_soft_i2c_set_addr16(slot_num, slave_dev_addr, internal_addr) != 0)
		return -1;
	return ax_soft_i2c_read_bytes(slot_num, slave_dev_addr, (uint8_t *)data, 2);
}



/**
  * Read 8 bit data from a random 8 bit internal addr in slave device. 
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_read8().
  * @param: data: output.
  * @return: 0 for success. -1 for failure.
  */
//description: 
//firstly set the address secondly read data from current address

int ax_soft_i2c_read8(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr, uint8_t* data)
{
	if (ax_soft_i2c_set_addr8(slot_num, slave_dev_addr, internal_addr) != 0)
		return -1;
	return ax_soft_i2c_read8_cur_addr(slot_num, slave_dev_addr, data);
}
int ax_soft_i2c_read8_8bit(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr, uint8_t* data, int port_number, int buf_len)
{
	char tmp;
	int i;
	if (slot_num < 1 || slot_num > 4)
	{
		printk("slot_number = %x is out of range\n", slot_num);
		return -1;
	}
	if (port_number < 1 || port_number > 6)
	{
		printk("port_number = %x is out of range\n", port_number);
		return -1;
	}
	if (ax_soft_i2c_set_addr8_8bit(slot_num, slave_dev_addr, internal_addr, port_number) != 0)
		return -1;
	for (i=0; i<buf_len; i++)
	{
		if (ax_soft_i2c_read8_cur_addr_8bit(slot_num, slave_dev_addr, &tmp, port_number) == AX_SOFT_I2C_OK)
		{
			data[i] = tmp & 0xff;
		}
		else
		{
			return 	AX_SOFT_I2C_FAIL;
		}
	}
	return AX_SOFT_I2C_OK;
}




/**
  * Write 16 bit data to slave device.  When internal addr is 16 bit should use this function.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_write16().
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_write16(int slot_num, uint8_t slave_dev_addr, uint16_t internal_addr, uint16_t data)
{
	uint8_t  tmp[4];
	tmp[0] = (internal_addr >> 8) & 0xff; //internal addr's high 8 bit
	tmp[1] = (internal_addr) & 0xff; //internal addr's low 8 bit
	tmp[2] = (data >> 8) & 0xff; //data's high 8 bit
    tmp[3] = (data) & 0xff; //data's low 8 bit
	return ax_soft_i2c_write_bytes(slot_num, slave_dev_addr, tmp, 4);
}


/**
  * Write 8 bit data to slave device. When internal addr is 8 bit should use this function.
  * @param    slot_num: children cards' slot number(currently 1~4). (This decided the CPLD address which is used to emulate the soft i2c.)
  * Note: similar to octeon_twsi_write8().
  * @return: 0 for success. -1 for failure.
  */
int ax_soft_i2c_write8(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr, uint8_t data)
{
	uint8_t  tmp[2];
	tmp[0] = (internal_addr) & 0xff; //internal addr 8 bit
	tmp[1] = (data) & 0xff; //data 8 bit
	return ax_soft_i2c_write_bytes(slot_num, slave_dev_addr, tmp, 2);
}

int ax_soft_i2c_write8_8bit(int slot_num, uint8_t slave_dev_addr, uint8_t internal_addr, uint8_t *data, uint8_t port_number, int buf_len)
{
	uint8_t  tmp[2];
	int i, j=0;
	for (i=internal_addr; i<(internal_addr+buf_len); i++)
	{
		tmp[0] = (internal_addr) & 0xff; //internal addr 8 bit
		tmp[1] = (data[j]) & 0xff;  //data 8 bit
		if (ax_soft_i2c_write_bytes_8bit(slot_num, slave_dev_addr, tmp, 2, port_number) == AX_SOFT_I2C_OK)
		{
			j++;
			continue;
		}
		else
		{
			return 	AX_SOFT_I2C_FAIL;
		}
	}
	return AX_SOFT_I2C_OK;
}


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
int  ax_soft_i2c_read16_eeprom(int slot_num, uint8_t chip_addr, uint16_t internal_addr, uint8_t *buffer_in_dram, int len)
{
    uint16_t tmp_data;
    if (!buffer_in_dram|| len <= 0)
        return (-1);
	/*  commented by baolc, 2008-08-22, change another read mode
    if (ax_soft_i2c_set_addr16(slot_num, chip_addr, internal_addr) != 0)
        return (-1);

    while (len--)
    {
        if (ax_soft_i2c_read8_cur_addr(slot_num, chip_addr, &tmp_data) != 0)
			return (-1);
        *buffer_in_dram++ = (uint8_t)(tmp_data & 0xff);
    }
    */

    /* this is another read mode */
	while (len--)
    {
        if (ax_soft_i2c_read16(slot_num, chip_addr, internal_addr, &tmp_data) != 0)
			return (-1);
        *buffer_in_dram++ = (uint8_t)(tmp_data & 0xff);
		internal_addr++;
    }

    return(0);
}


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
int  ax_soft_i2c_read8_eeprom(int slot_num, uint8_t chip_addr, uint8_t internal_addr, uint8_t *buffer_in_dram, int len)
{
    //uint8_t tmp_data;
    if (!buffer_in_dram|| len <= 0)
        return (-1);

	/*  commented by baolc, 2008-08-22, change another read mode 
    if (ax_soft_i2c_set_addr8(slot_num, chip_addr, internal_addr) != 0)
        return (-1);

    while (len--)
    {
        if (ax_soft_i2c_read8_cur_addr(slot_num, chip_addr, &tmp_data) != 0)
			return (-1);
        *buffer_in_dram++ = (uint8_t)(tmp_data & 0xff);
    }
   */

    /* this is another read mode 
	while (len--)
    {
        if (ax_soft_i2c_read8(slot_num, chip_addr, internal_addr, &tmp_data) != 0)
			return (-1);
        *buffer_in_dram++ = (uint8_t)(tmp_data & 0xff);
		internal_addr++;
    }
	*/


	/* yet another read mode */
	if (ax_soft_i2c_set_addr8(slot_num, chip_addr, internal_addr) != 0)
        return (-1);
	if (ax_soft_i2c_read_bytes(slot_num, chip_addr, buffer_in_dram, len) != 0)
		return (-1);

    return(0);
}



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
int  ax_soft_i2c_write16_eeprom(int slot_num, uint8_t chip_addr, uint16_t internal_addr, uint8_t *buffer_in_dram, int len)
{
    if (!buffer_in_dram|| !len)
        return (-1);

    while (len >= 2)
    {
        if (ax_soft_i2c_write16(slot_num, chip_addr, internal_addr, *((uint16_t *)buffer_in_dram)) != 0)
            return (-1);
		printk(" %x ", *((uint16_t *)buffer_in_dram));
        len -= 2;
        internal_addr +=2;
        buffer_in_dram += 2;
    }

    /* Handle single (or last) byte case */
    if (len == 1)
    {
        uint16_t tmp_16bit_data;
		uint8_t tmp_next_byte;

        /* Read 8 bits at the next byte */
		if (ax_soft_i2c_set_addr16(slot_num, chip_addr, internal_addr+1) != 0)
	        return (-1);
        if (ax_soft_i2c_read8_cur_addr(slot_num, chip_addr, &tmp_next_byte) != 0)
			return (-1);

		tmp_16bit_data = (tmp_next_byte & 0xff); //low 8 bits is the next byte in eeprom(old data)
        tmp_16bit_data |= (*buffer_in_dram) << 8; //high 8 bits is the data to be written into eeprom

        if (ax_soft_i2c_write16(slot_num, chip_addr, internal_addr, tmp_16bit_data) != 0)
            return (-1);
    }

    return(0);
}


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
int  ax_soft_i2c_write8_eeprom(int slot_num, uint8_t chip_addr, uint8_t internal_addr, uint8_t *buffer_in_dram, int len)
{
    if (!buffer_in_dram|| !len)
        return (-1);

    while (len >= 1)
    {
        if (ax_soft_i2c_write8(slot_num, chip_addr, internal_addr, *buffer_in_dram) != 0)
            return (-1);
        len--;
        internal_addr++;
        buffer_in_dram++;
    }

    return(0);
}




/*****************************SOFT I2C PROBE***********************************/
int ax_soft_i2c_probe_eeprom(struct soft_i2c_read_write_16bit *args)
{
	int i = 0;
	uint8_t tmp;
	printk("Valid chip addresses on soft i2c bus: ");
	for (i=0; i<128; i++)
	{
		if (ax_soft_i2c_set_addr8(args->slot_num, i, 0))
	        continue;

	    if (ax_soft_i2c_read8_cur_addr(args->slot_num, i, &tmp) < 0)
			continue;
		else
			printk("valid slave_addr %#x ", i); // probed OK 
			//*((unsigned char *)args->valid_bus_addr + i) = 1;
	}
	return 0;
}

/* code optmize: Big parameter passed by value (PASS_BY_VALUE)
 wangchao@autelan.com 2013-01-17 */
int ax_soft_i2c_probe_eeprom_cmd(struct soft_i2c_read_write_16bit *args  /*before change:args*/)
{
	//int slot_num;
	//slot_num = (int)simple_strtoul(args, NULL, 10);
	//slot_num = args->read_from_user;
	//printk("slot_num = %d\n", slot_num);
	ax_soft_i2c_probe_eeprom(args); /*before change:&args*/
	return 0;
}


//U_BOOT_CMD(
//	ax_soft_i2c_probe, 2, 0, ax_soft_i2c_probe_eeprom_cmd,
//	"ax_soft_i2c_probe  -probe chips on a soft i2c bus\n",
//	"slot_num\n"
//	"   -slot_num(should be 1~4) is the slot number of the chidren card whose i2c bus will be read/write\n"
//);




/*****************************DEBUG cmd for SOFT I2C****************************/
int ax_test_soft_i2c_line_scl_sda(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	AX_HIGH_OR_LOW res = 0;
	int slot_num = 1;
	uint64_t cpld_base_addr = 0;
	if (argc == 2)
	{
		slot_num = (int)simple_strtoul(argv[1], NULL, 10);
	}else{
		//printk("Usage:\n%s\n", cmdtp->usage);
		return -1;
    }
	cpld_base_addr = AX_SOFT_I2C_CONVERT_SLOT_NUM_TO_CPLD_ADDR(slot_num);
	printk("cpld base addr: 0x%lx\n", (long unsigned int)cpld_base_addr);
	
	AX_SOFT_I2C_SCL(1, cpld_base_addr, 1);
	res = AX_SOFT_I2C_READ_FROM_SCL(cpld_base_addr);
	printk("write scl 1...read scl got %d\n", res);

	AX_SOFT_I2C_SCL(0, cpld_base_addr, 1);
	res = AX_SOFT_I2C_READ_FROM_SCL(cpld_base_addr);
	printk("write scl 0...read scl got %d\n", res);
	return 0;
}



int ax_soft_i2c_write_byte_test(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	//int rcode = 0;
	int slot_num;
	uint8_t test_data;
    uint8_t slave_dev_addr = CFG_DEF_EEPROM_ADDR; //default use this addr
    
    if (argc == 4)
    {
		slot_num = (int)simple_strtoul(argv[1], NULL, 10);
		slave_dev_addr = (uint8_t)simple_strtoul(argv[2], NULL, 16);
		test_data = (uint8_t)simple_strtoul(argv[3], NULL, 16);
		ASI_DEBUG("slave_dev_addr is %#x, test data is %#x\n", slave_dev_addr, test_data);
    }else{
		//printk("Usage:\n%s\n", cmdtp->usage);
		return -1;
    }

	if (ax_soft_i2c_write_bytes(slot_num, slave_dev_addr, &test_data, 1) == AX_SOFT_I2C_FAIL)
		ASI_DEBUG("ax_soft_i2c test cmd: ax_soft_i2c_write_bytes return failure\n");

	return 0;
}




#if 0 //some cmds just for test

U_BOOT_CMD(
	ax_test_soft_i2c_line_scl_sda,	2,	0,	ax_test_soft_i2c_line_scl_sda,
	"ax_test_soft_i2c_line_scl_sda  - test soft i2c's line of cpld\n",
 	"  slot_num \n"
 	"   -slot_num(should be 1~4) is the slot number of the chidren card whose i2c bus will be read/write\n"
);


U_BOOT_CMD(
	ax_test_soft_i2c_write_byte,	4,	0,	ax_soft_i2c_write_byte_test,
	"ax_test_soft_i2c_write_byte  - write one byte through soft i2c for test soft i2c's signals\n",
 	"  slot_num  dev_addr  hex_data\n"
 	"   -slot_num(should be 1~4) is the slot number of the chidren card whose i2c bus will be read/write\n"
 	"	-dev_addr is the address of a slave device on i2c bus\n"
 	"	-hex_data is a test byte's hex value\n"
);



#endif /* (CONFIG_COMMANDS) */




