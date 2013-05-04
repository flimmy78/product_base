#define GET_BOOT_ENV	0
#define SAVE_BOOT_ENV 	1


#define FLASH_BASE_ADDR	0x800000001fc00000
#define ENV_SPACE_SIZE 	0x2000

#define CPLD_VERSION_REG				0x5
#define CPLD1_BASE_ADDR 	0x1d010000

typedef struct boot_env
{
	char name[64];
	char value[128];
	int operation;
}boot_env_t;

typedef struct{
	unsigned int store_flag;
	unsigned char mac[6];
}mac_addr_stored_t;


typedef struct sys_mac{
	
	unsigned char mac_add[6];
	unsigned char reserve[2];
}sys_mac_add;

/*gxd*/

typedef struct {
	ulong	size;			/* total bank size in bytes		*/
	ushort	sector_count;		/* number of erase units		*/
	ulong	flash_id;		/* combined device & manufacturer code	*/
} flash_info_t;

#define FLASH_CFI_8BIT		0x01

#define AMD_MANUFACT	0x00010001	/* AMD	   manuf. ID in D23..D16, D7..D0 */
#define SST_MANUFACT	0x00BF00BF	/* SST	   manuf. ID in D23..D16, D7..D0 */
/*--------------------------------------------------------------------------*/
#define FLASH_MAN_AMD	0x00000000	/* AMD					*/
#define FLASH_MAN_SST	0x00100000
#define FLASH_UNKNOWN	0xFFFF		/* unknown flash type			*/
/*--------------------------------------------------------------------------*/
#define AMD_ID_LV040B	0x004F004F		/* 29LV040B ID				*/
#define SST_ID_VF040	0x00D700D7		/*39VF040 ID (512KB=64KB x 8)  */ 
/*--------------------------------------------------------------------------*/
#define FLASH_AM040	0x0001		/* AMD Am29F040B, Am29LV040B		*/
#define FLASH_SST040	0x000E		/* SST 39xF040 ID (512KB = 4Mbit x 8 )	*/

/*NOTE:the new version can't read out a right manufacturer id and device id, 
  so we have to use a fixed value temporarily*/
#define SST_MANUFACT_FIXED	0xBF 
#define FLASH_SST040_FIXED	0xD7

typedef struct bootrom_file
{
	char path_name[4096];
}bootrom_file;

enum product_num{
	AX8000,			/* ax8000 */	
	AX7000,			/* ax7000 */
	AX5000,			/* ax5000 */
	AX3000,			/* ax3000 */
	AU3000,			/* au3052 */
	AX5612E,		/* ax5612e */
	PRODUCT_TYPE_NUM
};


int do_get_or_save_boot_env(char *name,char *value,int operation);
int do_get_product_type(sys_product_type_t *product_type);
int get_mac_from_devinfo(void);
int bmk_get_sys_mac(sys_mac_add * mac_addr);

int do_update_bootrom(bootrom_file *);




