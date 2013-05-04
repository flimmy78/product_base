#include <asm/octeon/cvmx.h>


/*-------------------------------------------------------------------*/
#define AX_EEPROM_SYSINFO_MAX_SIZE         (0x01ff)
#define AX_EEPROM_SYSINFO_INIT_VAL         ((uint16_t)0x0000)
/*-------------------------------------------------------------------*/

uint64_t _ax_octeon_get_cycles(void)
{
    uint32_t tmp_low, tmp_hi;

    asm volatile (
               "   .set push                  \n"
               "   .set mips64                  \n"
               "   .set noreorder               \n"
               "   dmfc0 %[tmpl], $9, 6         \n"
               "   dadd   %[tmph], %[tmpl], $0   \n"
               "   dsrl  %[tmph], 32            \n"
               "   dsll  %[tmpl], 32            \n"
               "   dsrl  %[tmpl], 32            \n"
               "   .set pop                 \n"
                  : [tmpl] "=&r" (tmp_low), [tmph] "=&r" (tmp_hi) : );

    return(((uint64_t)tmp_hi << 32) + tmp_low);
}

void _ax_octeon_delay_cycles(uint64_t cycles)
{
    uint64_t start = _ax_octeon_get_cycles();
    while (start + cycles > _ax_octeon_get_cycles())
        ;
}


int _ax_octeon_twsi_write8(uint8_t dev_addr, uint8_t addr, uint8_t data)
{

    uint64_t val, val_1;
	int timeout=0x8000000;
    val_1= 0x8000000000000000ull | ( 0x0ull << 57) | ( 0x1ull << 55) | ( 0x1ull << 52) | (((uint64_t)dev_addr) << 40) | (((uint64_t)addr) << 8) | data ;
    cvmx_write_csr(CVMX_MIO_TWSX_SW_TWSI(0), val_1 ); // tell twsii to do the read
    while ((timeout--) && (cvmx_read_csr(CVMX_MIO_TWSX_SW_TWSI(0))&0x8000000000000000ull));
	if (timeout < 0)
	{
		printk("_ax_octeon_twsi_write8 TIME_OUT.\n");
		return -1;
	}
    val = cvmx_read_csr(CVMX_MIO_TWSX_SW_TWSI(0));
    if (!(val & 0x0100000000000000ull)) {
	printk("twsii write (2byte) failed\n");
        return -1;
    }

    /* or poll for read to not fail */
    //octeon_delay_cycles(40*1000000);
    return(0);
}

int _ax_octeon_twsi_write16(uint8_t dev_addr, uint16_t addr, uint16_t data)
{

    uint64_t val;
	int timeout=0x8000000;
    cvmx_write_csr(CVMX_MIO_TWSX_SW_TWSI(0),0x8000000000000000ull | ( 0x8ull << 57) | (((uint64_t)dev_addr) << 40) | (addr << 16) | data ); // tell twsii to do the read
    while ((timeout--) && (cvmx_read_csr(CVMX_MIO_TWSX_SW_TWSI(0))&0x8000000000000000ull));
	if (timeout < 0)
	{
		printk("_ax_octeon_twsi_write16 TIME_OUT.\n");
		return -1;
	}
    val = cvmx_read_csr(CVMX_MIO_TWSX_SW_TWSI(0));
    if (!(val & 0x0100000000000000ull)) {
        //      printf("twsii write (4byte) failed\n");
        return -1;
    }

    /* or poll for read to not fail */
    _ax_octeon_delay_cycles(40*1000000);
    return(0);
}

static int _ax_octeon_twsi_set_addr16(uint8_t dev_addr, uint16_t addr)
{

    /* 16 bit internal address ONLY */
    uint64_t val;
	int timeout=0x8000000;
    cvmx_write_csr(CVMX_MIO_TWSX_SW_TWSI(0),0x8000000000000000ull | ( 0x1ull << 57) | (((uint64_t)dev_addr) << 40) | ((((uint64_t)addr) >> 8) << 32) | (addr & 0xff)); // tell twsii to do the read
    while ((timeout--) && (cvmx_read_csr(CVMX_MIO_TWSX_SW_TWSI(0))&0x8000000000000000ull));
	if (timeout < 0)
	{
		printk("_ax_octeon_twsi_set_addr16 TIME_OUT.\n");
		return -1;
	}
    val = cvmx_read_csr(CVMX_MIO_TWSX_SW_TWSI(0));
    if (!(val & 0x0100000000000000ull)) {
        return -1;
    }

    return(0);
}


/**
  * Read 8bit from cur i2c addr.
  */
static int _ax_octeon_twsi_read8_cur_addr(uint8_t dev_addr)
{

    uint64_t val;
	int tmr=0x8000000;
    cvmx_write_csr(CVMX_MIO_TWSX_SW_TWSI(0),0x8100000000000000ull | ( 0x0ull << 57) | (((uint64_t)dev_addr) << 40) ); // tell twsii to do the read
    while ((tmr--) && (cvmx_read_csr(CVMX_MIO_TWSX_SW_TWSI(0))&0x8000000000000000ull));
	if (tmr < 0)
	{
		printk("_ax_octeon_twsi_read8_cur_addr TIME_OUT.\n");
		return -1;
	}
	
    val = cvmx_read_csr(CVMX_MIO_TWSX_SW_TWSI(0));
    if (!(val & 0x0100000000000000ull))
    {
        return -1;
    }
    else
    {
        return(uint8_t)(val & 0xFF);
    }
}


int _ax_octeon_twsi_read16_cur_addr(uint8_t dev_addr)
{
    int tmp;
    uint16_t val;
    tmp = _ax_octeon_twsi_read8_cur_addr(dev_addr);
    if (tmp < 0)
        return(-1);
    val = (tmp & 0xff) << 8;
    tmp = _ax_octeon_twsi_read8_cur_addr(dev_addr);
    if (tmp < 0)
        return(-1);
    val |= (tmp & 0xff);

    return((int)val);

}


int _ax_octeon_twsi_read16(uint8_t dev_addr, uint16_t addr)
{
    if (_ax_octeon_twsi_set_addr16(dev_addr, addr))
        return(-1);

    return(_ax_octeon_twsi_read16_cur_addr(dev_addr));
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
 * @param chip   chip address
 * @param addr   internal address (only 16 bit addresses supported)
 * @param alen   address length, must be 2
 * @param buffer memory buffer pointer
 * @param len    number of bytes to write
 * 
 * @return 0 on Success
 *         1 on Failure
 */
int  _ax_i2c_write(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len)
{

    if (alen != 2 || !buffer || !len)
        return(1);
    while (len >= 2){
        if (_ax_octeon_twsi_write16(chip, addr, *((uint16_t *)buffer)) < 0)
            return(1);
        len -= 2;
        addr +=2;
        buffer += 2;
    }
    /* Handle single (or last) byte case */
    if (len == 1){
        int tmp;
        /* Read 16 bits at addr */
        tmp = _ax_octeon_twsi_read16(chip, addr);
        if (tmp < 0)
            return(1);
        tmp &= 0xff; 
        tmp |= (*buffer) << 8;
        if (_ax_octeon_twsi_write16(chip, addr, (uint16_t)tmp) < 0)
            return(1);
    }
    return(0);
}

int bmk_ax_i2c_write(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len)
{
	return _ax_i2c_write(chip,addr,alen,buffer,len);
}

