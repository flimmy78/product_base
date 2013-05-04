#include <linux/string.h>
#include <linux/slab.h>
#include <asm/delay.h>			/* udelay needed */
#include <linux/delay.h>		/* mdelay, ndelay, msleep needed */
#include <linux/unistd.h>
#include <linux/module.h>		/* For EXPORT_SYMBOL */
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <linux/autelan_product.h>

#include <asm/octeon/cvmx.h>
#include <asm/octeon/cvmx-app-init.h>

#include <sysdef/npd_sysdef.h>
#include "bmk_main.h"
#include "bmk_operation_boot_env.h"

#undef DEBUG
#ifdef DEBUG 
#define debug(args...) printk(args)
#else
#define debug(args...)
#endif

char *mac_filename_my="/devinfo/mac"; 
char *base_mac_filename = "/devinfo/base_mac"; 
flash_info_t info;

mac_addr_stored_t stored_mac;

extern char local_slot_num;
extern int is_distributed;

extern cvmx_bootinfo_t *octeon_bootinfo;
extern product_info_t autelan_product_info;
extern int do_cpld_reg_read(bm_op_cpld_args *);
int get_flash_type(flash_info_t *);
int do_get_boot_env (char *name, char *val);
int do_save_boot_env(char *name,char *value);
void delete_env(char *env_base_addr,char *env_offset);
int crc32_for_boot_env(unsigned int crc, char *buf,int len);
void flash_sect_erase(int sect_number);
int flash_buffer_write(int sect_number,char *addr,int buffer_size);


/*
 * Name:	do_get_or_save_boot_env
 *
 * Parameters:
 * 		name:	the env variable name. 
 * 				In u-boot it is a char type pointer, so its long is not restricted. 
 * 				But here it is restricted in 50.
 * 				It can't be empty. When it is empty, return -1.
 * 				
 * 		value:	the env variable value. 
 * 				In u-boot it is also a char type pointer and its long is not restricted. 
 * 				But here it is restrictd in 100.
 * 				when operation is GET_BOOT_ENV, it will store the env value
 * 				when operation is SAVE_BOOT_ENV, it bring the value in this function
 * 				NOTE: 	when the value's lenth is 0, this function will delete the env.
 *
 * 		operation:	0 is geting a env;
 * 					1 is saving a env
 *
 * return:	when the name is NUll or the operation is valid return -1;
 * 			Others return the value retuned from the do_get_boot_env and do_save_boot_env function
 */
int do_get_or_save_boot_env(char *name,char *value,int operation)
{
	if(strlen(name) == 0){
		debug("the name parameter is NULL\n");
		return -1;
	}

	if(operation == GET_BOOT_ENV){		
		return do_get_boot_env(name,value);
	}else if(operation == SAVE_BOOT_ENV){
		return do_save_boot_env(name,value);
	}else{
		debug("the parameter operation is invalid\n");
		return -1;
	}	
}


/*
 * s1 is either a simple 'name', or a 'name=value' pair.
 * s2 is a 'name=value' pair.
 * If the names match, return the value of s2, else NULL.
 */
char *envmatch (char * s1, char * s2)
{
	while (*s1 == *s2++)
		if (*s1++ == '=')
			return (s2);
	if (*s1 == '\0' && *(s2 - 1) == '=')
		return (s2);
	return (NULL);
}

/*
* name:		the env variable name. In u-boot it is a char type pointer, so its long is not restricted. But here it is restricted in 50.
* val: 		the env variable value. In u-boot it is also a char type pointer and its long is not restricted. But here it is restrictd in 100.
*			when finding the env, this function will put the env value in the parameter value.
*
* return:	when don't finding the env return -1
*			when finding the env return 0, at the same time put the env value in the parameter value
*/
int do_get_boot_env (char *name, char *val)
{
	char *env, *nxt, *tmp;
	char *env_base_addr;
	int i;
		
	env_base_addr = (char *)(FLASH_BASE_ADDR + 7*64*1024);/*gxd modify 16*/

	for (env = env_base_addr + 4; *env; env = nxt + 1) 
	{
		for (nxt = env; *nxt; ++nxt) 
		{
			if (nxt >= env_base_addr + ENV_SPACE_SIZE) 
			{
				debug("environment not terminated\n");
				return -1;
			}
		}
		
		tmp = envmatch(name, env);
		if(!tmp)
			continue;
		
		for(i = 0; *tmp; i++)
		{
            val[i] = *tmp;
			tmp++;
		}
		
		return 0;
	}
	return -1;
}


/*
* name:		the env variable name. In u-boot it is a char type pointer, so its long is not restricted. But here it is restricted in 50.
* value: 		the env variable value. In u-boot it is also a char type pointer and its long is not restricted. But here it is restrictd in 100.
*			NOTE: when the value's lenth is 0, this function will delete the env.
*
* return:		when don't finding the env return -1
*			when finding the env return 0, at the same time put the env value in the parameter value
*/
int do_save_boot_env(char *name,char *value)
{	
	int len;
	int env_offset;
	char *env_buff;
	char *env_addr;
	char *env_addr_temp;	
	char *flash_env_base_addr;
	/*gxd*/
    int env_start_sector ;
	int sector_size = info.size/info.sector_count;
	int env_end_sector ;
	int i;
	char *ptr ;

	int valuenumb;
	unsigned char storage_flag = 1;
	unsigned long long cpld_version;

	/*get env sector boundary*/
	switch(info.flash_id){
		case (FLASH_MAN_SST|FLASH_SST040): 
			env_start_sector = 112;
			env_end_sector = 113;
			break;
		case (FLASH_MAN_AMD|FLASH_AM040):
			env_start_sector = 7;
			env_end_sector = 7;
			break;
		default:
			debug("do_save_boot_env : unkown flash ID\n");
			return -1;
	}

	if(!strcmp(name,"bootcmd"))
	{
       	cpld_version = cvmx_read64_uint8(CPLD1_BASE_ADDR+CPLD_VERSION_REG+ (1ull << 63));
		storage_flag = cpld_version > 4 ? 1 : 0;   /* 1:CF Card, 0:SD Card, nie@autelan.com on 2012.8.31 */

		valuenumb = (int)simple_strtoul(value, NULL, 10);

		switch(valuenumb)
		{
		
           	case 1:   /* AX7605i AX71_SMU board (don't take fast forward) */
   				if (storage_flag)
   				    sprintf(value,"fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xffff");
   				else
   				    sprintf(value,"fatload mmc 0 100000 $(bootfile);bootm 100000 coremask=0xffff");

           		break;
			case 2:   /* AX7605i AX71_SMU board (take fast forward) */
				if(storage_flag)
				{
   				    sprintf(value,"fatload ide 0 100000 $(sefile);bootoct 100000 coremask=0xff00;fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xff mem=1790");
				}
				else
				{
   				    sprintf(value,"fatload mmc 0 100000 $(sefile);bootoct 100000 coremask=0xff00;fatload mmc 0 100000 $(bootfile);bootm 100000 coremask=0xff mem=1790");
				}
						
           		break;
           	case 3:   /* AX7605i AX71_2x12g12s board */
   				sprintf(value,"fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0x3");
           			
           		break;
		    case 4:   /* AX8610 AX71_1x12g12s board(Don't take fast forward) */
   				sprintf(value,"fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xfff");

   				break;
           	case 5:  /* AX8610 AX71_1x12g12s board(take fast forward) */
   				sprintf(value,"fatload ide 0 100000 $(sefile);bootoct 100000 coremask=0xf00;fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xff mem=3072");
            			
           		break;
           	case 6:   /* AX8610 AX81_SMU board */
   				sprintf(value,"fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xf");
            			
           		break;
			case 7:   /*AX81_AC8C/AX81_AC12C board(Don't take fast forward)*/
   				sprintf(value,"fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xfff");
            			
           		break;
           	case 8:   /*AX8610 AX81_AC8C/AX81_AC12C board(take fast forward)*/
   				sprintf(value,"fatload ide 0 100000 $(sefile);bootoct 100000 coremask=0xf00;fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xff mem=3072");
            			
           		break;
			case 9:  /*AX8610 AX81_AC4x board(Don't take fast forward)*/
   				sprintf(value,"fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xfff");
            			
           		break;
           	case 10:  /* AX8610 AX81_AC4x board(take fast forward) */
   				sprintf(value,"fatload ide 0 100000 $(sefile);bootoct 100000 coremask=0xf00;fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xff mem=3072");
            			
           		break;			
           	case 11:   /* AX8610 AX81_12x board */ 
   				sprintf(value,"fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xf");
            			
           		break;
			case 12:   /* AX8610 AX81_1x12g12s board(Don't take fast forward) */
   				sprintf(value,"fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xfff");

   				break;
           	case 13:  /* AX8610 AX81_1x12g12s board(take fast forward) */
   				sprintf(value,"fatload ide 0 100000 $(sefile);bootoct 100000 coremask=0xf00;fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0xff mem=3072");
            			
           		break;           		
           	case 14:   /* AX8610 AX81_2x12g12s board */
   				sprintf(value,"fatload ide 0 100000 $(bootfile);bootm 100000 coremask=0x3");
            			
           		break;

           	default: 
				break;
           }
	}
		
	debug("env env_start_sector : %d \t env_end_sector : %d\n",
					env_start_sector,env_end_sector);
	debug("env name == %s\tenv value == %s\n",name,value);
	flash_env_base_addr = (char *)(FLASH_BASE_ADDR + 7*64*1024);/*gxd modify to 16*/
	env_buff = (char *)kmalloc(ENV_SPACE_SIZE,GFP_KERNEL);	
	if(env_buff == NULL){
		printk("kmalloc for env_buff failed\n");
		return -1;
	}
	/*cp env to ram from flash*/
	ptr = env_buff;
	env_addr = env_buff + 4;
	memcpy(env_buff,flash_env_base_addr,ENV_SPACE_SIZE);
	debug("original env checksum crc == 0x%x\n",crc32_for_boot_env(0,env_addr,ENV_SPACE_SIZE-4));
	/*
	* find the env name, and delete it 
	*/
	len = strlen(name);
	for(env_offset = 4,env_addr_temp = env_addr;env_offset < ENV_SPACE_SIZE;){
		if((strncmp(name,env_addr_temp,len) == 0) && *(env_addr_temp + len) == '='){
			debug("find the env name\n");
			delete_env(env_addr,env_addr_temp);
			break;
		}
		env_offset ++;
		env_addr_temp ++;
	}

	if(env_offset >= ENV_SPACE_SIZE){
		debug("don't match the env name string\n");
		if(strlen(value)== 0){
			kfree(env_buff);   
			env_buff = NULL;
			return 0;
		}
	}
	/*
	* Delete only
	*/
	if(strlen(value) == 0){
		debug("the value == NULL, so delete it\n");
		*(int *)env_buff = crc32_for_boot_env(0,env_addr,ENV_SPACE_SIZE-4);
		debug("the new checksum crc == 0x%x\n",*(int *)env_buff);
		/*gxd modified*/
		for(i=env_start_sector; i<env_end_sector+1;i++){	
			flash_sect_erase(i);
			flash_buffer_write(i,ptr,sector_size);
			ptr += sector_size;
			debug("i : %d\n",i);
		}
		
		kfree(env_buff);   
	    env_buff = NULL;
		return 0;
	}
	
	/*
	 * Append new definition at the end
	 */
	for (env_addr_temp=env_addr; *env_addr_temp || *(env_addr_temp+1); ++env_addr_temp)
		;
	/*
	*there is at least one env.
	*/
	if (env_addr_temp > env_addr)
		++env_addr_temp;		// after this sentence the env points the second '\0'

	if((strlen(name)+strlen(value)+3)>(ENV_SPACE_SIZE - 4 - (env_addr_temp-env_addr))){
		printk("Error! Overflow! the env %s is deleted!",name);
		kfree(env_buff);   
	    env_buff = NULL;
		return -1;
	}

	while ((*env_addr_temp= *name++) != '\0')
		env_addr_temp++;
	
	*env_addr_temp = '=';

	while ((*++env_addr_temp = *value++) != '\0')
			;

	*++env_addr_temp = '\0';

	*(int *)env_buff = crc32_for_boot_env(0,env_addr,ENV_SPACE_SIZE-4);
	debug("the new checksum crc == 0x%x\n",*(int *)env_buff);
	/*write env to flash from ram*/
	for(i=env_start_sector; i<env_end_sector+1;i++){	
		flash_sect_erase(i);
		if( info.flash_id == (FLASH_MAN_AMD|FLASH_AM040) )
		{
		   /* This is for AMD,because our env < 8K,
		    * So, we need not write AMD_sector_size(64K).
		    * And write ENV_SPACE_SIZE instead .
		    * If not,so long time we must wait !
		    */
			flash_buffer_write(i,ptr,ENV_SPACE_SIZE);	
		}
		else
		{
		   /*
		    * For SST,sector_size(4K) < 8K,
		    * so we need write twice !
		    * As follow: ptr += sector_size;
		    */
		    flash_buffer_write(i,ptr,sector_size);
		}
		ptr += sector_size;
		debug("i : %d\n",i);
	}
	
        kfree(env_buff);   
	    env_buff = NULL;	
	    return 0;
}

void delete_env(char *env_base_addr,char *env_offset)
{
	char *nxt;
	char *env;
	char *env_data;

	env = env_offset;
	env_data = env_base_addr;

	for(nxt=env;*nxt != '\0';nxt ++)
		;

	/* Now the state of the parameters is
	* env_data:		the addr of env_buff, that is, it points the first env;
	* env:			the addr of one env that is matched above;
	* nxt:			it points the end ('\0') of the env that the env parameter points. Note that it don't points the next env;
	*/

	if (*++nxt == '\0') {		//the last one of the env
		debug("the last one of the env\n");		
		if (env > env_data) {	//the last one but not the first one of the env
			debug("not the first one of the env\n");			
			env--;				//Make preparations for the sentence "*++env = '\0';" which is following the if... else sentence.
								//after this sentence the env points the end ('\0')of the previous env 
			
		} else {				//the last one and also the first one of the env
			debug("the first one of the env\n");		
			*env = '\0';			
		}
		
	} else {					//not the last one of the env
		//the for loop complete the function of deleting an env by making the following env forward when the env is in the middle position.
		for (;;) {
				
			*env = *nxt++;		//because the nxt is added by 1 in the if sentence, now it points the next env
			if ((*env == '\0') && (*nxt == '\0')) //when there are two '\o's, the env_buff is ended.
				break;
			++env;
		}			
	}
	*++env = '\0';				//there are two '\0's, the env_buff is end

}


void flash_write_cmd(int sect_number,int addr_offset,int value)
{
	char *addr;
	int sector_size ;
	if (info.sector_count)
		sector_size = info.size/info.sector_count;
	else
		sector_size = 0;
	addr = (char *)(FLASH_BASE_ADDR + sect_number*sector_size + addr_offset);
	*addr = value;	
	
}


void flash_reset (void)
{
	flash_write_cmd (0, 0x5555, 0xaa);
	flash_write_cmd (0, 0x2aaa, 0x55);
	flash_write_cmd (0, 0x5555, 0xf0);
	udelay(1000);	   
}


void flash_sect_erase(int sect_number)
{
	switch(info.flash_id){
		case (FLASH_MAN_SST|FLASH_SST040):
			flash_write_cmd (0,0x5555,0xaa);
			flash_write_cmd (0,0x2aaa,0x55);
			flash_write_cmd (0,0x5555,0x80);
			flash_write_cmd (0,0x5555,0xaa);
			flash_write_cmd (0,0x2aaa,0x55);
			flash_write_cmd (sect_number,0,0x30);
			msleep(25);
			break;
		case (FLASH_MAN_AMD|FLASH_AM040):
			flash_write_cmd (0, 0x555, 0xaa);
			flash_write_cmd (0, 0x2aa, 0x55);
			flash_write_cmd (0, 0x555, 0x80);
			flash_write_cmd (0, 0x555, 0xaa);
			flash_write_cmd (0, 0x2aa, 0x55);
			flash_write_cmd (sect_number,0,0x30);
			msleep(1000);
			break;
		default:
			printk("unkown flash ID\n");
			return;
			
	}
	debug("erase flash sect:%d\n",sect_number);
}


int flash_buffer_write(int sect_number,char *addr,int buffer_size)
{
	int offset;
	char *env_buff = addr;
	int sector_size;
	
	if (addr == NULL || buffer_size == 0)
		return 0;
	
	for(offset = 0;offset < buffer_size;offset++){
		switch(info.flash_id){
			case (FLASH_MAN_AMD|FLASH_AM040):
				flash_write_cmd (0, 0x555, 0xaa);
				flash_write_cmd (0, 0x2aa, 0x55);
				flash_write_cmd (0, 0x555, 0xa0);
				flash_write_cmd (sect_number,offset,*env_buff);	
				udelay(100);
				break;
			case (FLASH_MAN_SST|FLASH_SST040):
				flash_write_cmd (0,0x5555, 0xaa);
				flash_write_cmd (0,0x2aaa, 0x55);
				flash_write_cmd (0,0x5555, 0xa0);
				flash_write_cmd (sect_number,offset,*env_buff);	
				udelay(100);
				break;
			default:
				printk("unkown flash ID\n");
				return -1;
				
		}
		/*
		 * wangjiankun noted on Apr 10, 2009:
		 * for au3052 command "set boot_img IMG_NAME" failure,
		 * increasing the delay time from 20 to 30, but the datasheet
		 * specifies the max time is 20
		 */
		 udelay(30);
		sector_size = info.size/info.sector_count;
		if(env_buff[0] != *((char *)(FLASH_BASE_ADDR + sect_number*sector_size + offset))){
			printk("bm :buffer write error:dest:0x%lx\tsrc:0x%p\n",(FLASH_BASE_ADDR + sect_number * sector_size + offset), env_buff);
			printk("value is 0x%x \t expect is 0x%x\n",*((char *)(FLASH_BASE_ADDR + sect_number*sector_size + offset)),env_buff[0]);
			flash_reset();
			return -1;
		}
		
		env_buff++;
	}
	return 0;
}


unsigned int crc_table[256] = {
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419,
  0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4,
  0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07,
  0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
  0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856,
  0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
  0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
  0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
  0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
  0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a,
  0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599,
  0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190,
  0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
  0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e,
  0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
  0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed,
  0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
  0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3,
  0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
  0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
  0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5,
  0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010,
  0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17,
  0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
  0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
  0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
  0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344,
  0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
  0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a,
  0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
  0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1,
  0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c,
  0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
  0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe,
  0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31,
  0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c,
  0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
  0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b,
  0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
  0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1,
  0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
  0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
  0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7,
  0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66,
  0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
  0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8,
  0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b,
  0x2d02ef8d
  };

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

int crc32_for_boot_env(unsigned int crc, char *buf,int len)
{
    crc = crc ^ 0xffffffff;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) do {
      DO1(buf);
    } while (--len);
    return crc ^ 0xffffffff;
}
/*added by gxd for geting mac from devinfo and saving bootinfo*/

struct file* bm_sysfs_open(const char *filename, int flags, int mode)
{
    return filp_open(filename, flags, mode);
}

ssize_t bm_sysfs_read(struct file *filp, char *buf, size_t count)
{
    return filp->f_op->read(filp, buf, count, &filp->f_pos);
}

int cvm_read_from_file(char *filename,char *buff,int len)
{
	struct file *filp;
	mm_segment_t old_fs = get_fs();
	long error;
	if((filename == NULL) || (buff == NULL))
		return -1;
	set_fs(KERNEL_DS);
	filp = bm_sysfs_open(filename, O_RDONLY, 0);
	error = PTR_ERR(filp);
	if (IS_ERR(filp))
		{
		set_fs(old_fs);
		return -1;
		}
	    if(bm_sysfs_read(filp, buff, len) == len)
			{
		    filp_close(filp, NULL);
	    	set_fs(old_fs);
			return 0;
	    	}
		else
			{
		printk(KERN_INFO"Can not READ out MAC info.\n");
		    filp_close(filp, NULL);
	    	set_fs(old_fs);
			return -1;
		}
	return 0;	
		}

 int bm_cvm_get_mac_addr(unsigned char *mac_addr)
 {
 	int j;
	char tmp_mac_chars[13];
	unsigned char temp_2char[3] = {0x00, 0x00, 0x00};
    memset(tmp_mac_chars, 0x00, 13);
	if (0 == cvm_read_from_file(base_mac_filename,tmp_mac_chars,12)) {
		printk(KERN_INFO "read Base MAC string is %s.\n",tmp_mac_chars);
		if(0 == simple_strtoul(tmp_mac_chars, 0, 16))
			return -2;
	}
	else if(0 == cvm_read_from_file(mac_filename_my,tmp_mac_chars,12)){
		printk(KERN_INFO "read MAC string is %s.\n",tmp_mac_chars);
		if(0 == simple_strtoul(tmp_mac_chars, 0, 16))
			return -2;
	}
	else{
		return -1;
	}
	for (j=0; j<12; j+=2)
	{
		memcpy(temp_2char, tmp_mac_chars+j, 2);
		mac_addr[j/2] = (unsigned char)simple_strtoul((char *)temp_2char, 0, 16);
//		printk("%02x:",mac_addr[j/2]);
	}
//	printk("\n");
	return 0;
 }
 
 int get_mac_from_devinfo(void)
 {
 	unsigned char mac_addr[64];
	int rt;
	memset(mac_addr,0x00, sizeof(mac_addr));
	rt = bm_cvm_get_mac_addr(mac_addr);
	if(-1 == rt)
		return -1;
	if(-2 == rt)
		return -2;
	memcpy(octeon_bootinfo->mac_addr_base,mac_addr,sizeof(octeon_bootinfo->mac_addr_base));
	#if 0
	int i;
	for(i=0;i<sizeof(octeon_bootinfo->mac_addr_base);i++){
	printk("bm : octeon_bootinfo.mac_addr_base:%x\n",
			octeon_bootinfo->mac_addr_base[i]);
	}
	#endif 
	return 0;
 }

/*2009-5-21 9:13:10*/


//	module_init(oct_init);
//	module_exit(oct_cleanup);




/*

	EXPORT SYMBOL for kernel module call to get MAC.

*/
int mac_addr_get(unsigned char *mac)
{
	int i;
	if ((stored_mac.store_flag & 0x1) == 0x0)
	{
		if(0 == bm_cvm_get_mac_addr(stored_mac.mac))
		{
			if (is_distributed && (!(local_slot_num < 0)))
			{
				stored_mac.mac[5] += (local_slot_num - 0x1);
			}
			//stored_mac.store_flag = (stored_mac.store_flag | 0x1); //cancel to prevent error mac by caojia
			printk(KERN_INFO "Get system MAC from devinfo OK!\n");
		}
		else
		{
			memcpy(stored_mac.mac, octeon_bootinfo->mac_addr_base, MAC_ADDRESS_LEN);
			//stored_mac.store_flag = (stored_mac.store_flag | 0x1); //cancel to let interface get right board mac by caojia
			printk(KERN_INFO "Get system MAC from bootinfo OK!\n");
		}

		
		printk(KERN_INFO "Got MAC addr is:");
		for(i=0; i<MAC_ADDRESS_LEN-1; i++)
		{
			printk(KERN_INFO "%02X:", stored_mac.mac[i]);
		}
		printk(KERN_INFO "%02X.\n", stored_mac.mac[MAC_ADDRESS_LEN-1]);			
	}

	memcpy(mac, stored_mac.mac, MAC_ADDRESS_LEN);

	return 1;	
}

/*
	Used for IOCTL calling from userspace. 

	Get MAC address from system.
	The devinfo priority is high than bootinfo.
	MAC in bootinfo is from EEPROM.

*/
int bmk_get_sys_mac(sys_mac_add * mac_addr)
{

	mac_addr_get(mac_addr->mac_add);
	return 1;
}

int do_get_product_type(sys_product_type_t *product_type)
{
	product_type->product_num = autelan_product_info.product_series_num;
	product_type->module_num = autelan_product_info.board_type_num;
	
	return 1;
}

/*gxd*/
#define BOOTROMSIZE (448*1024)
#define READLEN 65536
#define PRODUCT_CN_LEN 8
#define AX_PRODUCT_TYPE_CPLD_REG		0x01

#define AX7K_CPLD_PRODUCT_TYPE_CODE	0x7
#define AX5K_CPLD_PRODUCT_TYPE_CODE	0x5
#define AU4K_CPLD_PRODUCT_TYPE_CODE 	0x4
#define AU3K_CPLD_PRODUCT_TYPE_CODE 	0x3


unsigned char flash_read_uchar (uint sect, uint offset)
{
	unsigned char *cp;
	int sector_size;
	
	if (info.sector_count)
		sector_size = info.size/info.sector_count;
	else
		sector_size = 0;
	cp = (unsigned char *)(FLASH_BASE_ADDR + sect*sector_size + offset);
	return (cp[0]);
}

int get_flash_type(flash_info_t *info)
{
	unsigned char manu_id;
	unsigned char dev_id;

	memset(info,0,sizeof(flash_info_t));

	/*NOTE:the new version can't read out a right manufacturer id and device id, 
	  so we have to use a fixed value temporarily*/
	manu_id = SST_MANUFACT_FIXED;	
	dev_id = FLASH_SST040_FIXED;
#if 0	
	flash_write_cmd (0, 0x5555, 0xaa);
    flash_write_cmd (0, 0x2aaa, 0x55);
    flash_write_cmd (0, 0x5555, 0x90);

	manu_id=flash_read_uchar(0,0x0);
	dev_id=flash_read_uchar(0,0x01);

	printk("manu_id is 0x%x\n", manu_id);
	
	if (manu_id != (SST_MANUFACT&0x0000FFFF))
	{
		flash_reset();
		
		flash_write_cmd (0, 0x555, 0xaa);
		flash_write_cmd (0, 0x2aa, 0x55);
		flash_write_cmd (0, 0x555, 0x90); 	

		manu_id=flash_read_uchar(0,0x00);
		dev_id=flash_read_uchar(0,0x01);
		printk("manu_id is 0x%x dev_id is 0x%x\n", manu_id, dev_id);
	}
#endif
	debug("bm : manu_id %x \t dev_id :%x\n",manu_id,dev_id);

	switch (manu_id | (manu_id << 16)) {
		case AMD_MANUFACT:
			printk("AMD Manufacture\tFlash ID == 0x%X\n",FLASH_MAN_AMD);
			info->flash_id = FLASH_MAN_AMD;
			break;
		case SST_MANUFACT:
			printk("SST Manufacture\tFlash ID == 0x%X\n",FLASH_MAN_SST);
			info->flash_id = FLASH_MAN_SST;
			break;
		default:
			printk("Unknown flash id \n");
			info->flash_id = FLASH_UNKNOWN;
			flash_reset();
			return FLASH_UNKNOWN;
	}
	switch (dev_id | (dev_id << 16)) {
		case AMD_ID_LV040B:
			printk("Am29LV040B\n");
			info->flash_id += FLASH_AM040;
			info->size = 0x00080000;
			info->sector_count = 8;
			break;			/* => 512 kB		*/
		case SST_ID_VF040:
			printk("39VF040\n");
			info->flash_id += FLASH_SST040;
			info->size = 0x00080000;
			info->sector_count = 128;
			break;			/* => 512 kB		*/	
		default:
			info->flash_id = FLASH_UNKNOWN;
			flash_reset ();
			return (FLASH_UNKNOWN);		/* => no or unknown flash */

	} 
	flash_reset();
	return 0;
}
/*
	verify the bootrom file to be suit with this board
*/
int verify_bootrom(char *pathname)
{
	struct file* filp;
	long error;
	int ret;
	int length;
	static  char buf[READLEN];
	char product[PRODUCT_CN_LEN];
	char *ptr = NULL;
	char *offset = NULL;
	mm_segment_t old_fs;
	bm_op_cpld_args bm_op_cpld = {0};
	bm_op_cpld_args bm_op_cpld_module = {0};
	
	const char product_sample[PRODUCT_TYPE_NUM][PRODUCT_CN_LEN] = {
		{0xff,0xff,0x1B,0x68,0xff,0xff,0x1B,0x68},	/* ax8000 */
		{0xff,0xff,0x1B,0x58,0xff,0xff,0x1B,0x58},	/* ax7000 */
		{0xff,0xff,0x13,0x88,0xff,0xff,0x13,0x88},	/* ax5000 */
		{0xff,0xff,0x0B,0xB8,0xff,0xff,0x0B,0xB8},	/* ax3000 */
		{0xff,0xff,0x0B,0xEC,0xff,0xff,0x0B,0xEC},	/* au3052 */
		{0xFF,0xFF,0x13,0x89,0xFF,0xFF,0x13,0x89}	/* 5612e */
	};
	
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	
	/* open file */
	debug("file name == %s\n", pathname);
	filp = bm_sysfs_open(pathname, O_RDONLY, 0);	
	error = PTR_ERR(filp);
	if (IS_ERR(filp))
	{
		set_fs(old_fs);
		return -1;
	}
	
	/* get product cn from bin */
	do{
		memset(buf, 0, sizeof(buf));
		ret = bm_sysfs_read(filp, buf, READLEN);
	}while(ret == READLEN);
	
	if (ret < 0)
	{
		return -1;
	}
	debug("bm : verify_bootrom ret :%d\n", ret);
	
	/* get the product_id & board type */
	bm_op_cpld.slot = 0;
	bm_op_cpld.regAddr = AX_PRODUCT_TYPE_CPLD_REG;
	do_cpld_reg_read(&bm_op_cpld);

	bm_op_cpld_module.slot = 0;
	bm_op_cpld_module.regAddr = 0x3;
	do_cpld_reg_read(&bm_op_cpld_module);
	debug("bm_op_cpld.regData == 0x%x\n", bm_op_cpld.regData);
	debug("bm_op_cpld_module.regData == 0x%x\n", bm_op_cpld_module.regData);

	if (bm_op_cpld.regData == 0x8 && bm_op_cpld_module.regData == 0x1)		/* 8610 */
	{
		memcpy(product, product_sample[AX8000], PRODUCT_CN_LEN);
	}
	else if(bm_op_cpld.regData == 0x8 && bm_op_cpld_module.regData == 0x4)		/* AX71-2X12G12S(songchao) -CN5220, use u-boot-ax5612e.bin */
	{
		memcpy(product, product_sample[AX5612E], PRODUCT_CN_LEN);
	}	
	else if(bm_op_cpld.regData == 0x5 && bm_op_cpld_module.regData == 0x3)		/* 5612i */
	{
		memcpy(product, product_sample[AX7000], PRODUCT_CN_LEN);
	}
	else if(bm_op_cpld.regData == 0x5 && bm_op_cpld_module.regData == 0x61)	/* 5612e */
	{
		memcpy(product, product_sample[AX5612E], PRODUCT_CN_LEN);
	}
	else if(bm_op_cpld.regData == 0x5 && bm_op_cpld_module.regData == 0x62)	/* 5608 -CN5220, use u-boot-ax5612e.bin */
	{
		memcpy(product, product_sample[AX5612E], PRODUCT_CN_LEN);
	}	
	else
	{
		switch ( bm_op_cpld.regData & 0xF )
		{
			case AX7K_CPLD_PRODUCT_TYPE_CODE :
				memcpy(product, product_sample[AX7000], PRODUCT_CN_LEN);
				break;
			case AX5K_CPLD_PRODUCT_TYPE_CODE :
			case AU4K_CPLD_PRODUCT_TYPE_CODE : 
				if(CVMX_BOARD_TYPE_EBH3100 == octeon_bootinfo->board_type)
				{
					memcpy(product, product_sample[AX5000], PRODUCT_CN_LEN);
				}
				else if(CVMX_BOARD_TYPE_CN3010_EVB_HS5 == octeon_bootinfo->board_type)
				{
					memcpy(product, product_sample[AX3000], PRODUCT_CN_LEN);
				}
				break;
			case AU3K_CPLD_PRODUCT_TYPE_CODE :
				if(CVMX_BOARD_TYPE_EBH3100  == octeon_bootinfo->board_type)
				{
					memcpy(product, product_sample[AX5000], PRODUCT_CN_LEN);
				}
				else if(CVMX_BOARD_TYPE_CN3010_EVB_HS5  == octeon_bootinfo->board_type)
				{
					memcpy(product, product_sample[AU3000], PRODUCT_CN_LEN);
				}
				break;
			default:
				return -2;
		}
	}

	debug("product == 0x%x\n",*((int *)product));
	
	/* verify the bootrom file */
	printk("bm : start to verify_bootrom");
	ptr = buf;
	offset = buf;
	debug("bm : buf %p\n",buf);
	do{
		length = ret - (offset - buf);
		ptr = memchr(offset, 0xFFFF, length);
		debug("bm : length : %d\toffset : %p\tptr %p\n",length,offset,ptr);
		if (ptr)
		{
			if(!memcmp(ptr, product, PRODUCT_CN_LEN))
			{
				printk("bm :.OK\n");
				filp_close(filp, NULL);
				set_fs(old_fs);
				return 0;
			}
			else
			{
				offset = ptr + 1;
			}
			printk(".");
		}	
	}while(ptr);
	printk("bm : .WORNG\n");
	filp_close(filp, NULL);
	set_fs(old_fs);
	return -2;
}

/*
	Used for IOCTL calling from userspace. 
	Update bootrom with new one
	Called by user in vtysh
	input-bootrom file 's pathname
	output-
	-2 bootrom file type is worng
	-1 bootrom file operation is error
	0  success
*/

int do_update_bootrom(bootrom_file *bootrom)
{
	struct file* filp;
	long error;
	/* code optmize:Unsigned compared against 0 (NO_EFFECT)
	 wangchao@autelan.com 2013-01-17 */
	/*unsigned int ret = 0;*/
	int ret = 0;
	int i,j;
	static unsigned char buf[READLEN];
	unsigned char *ptr;
	mm_segment_t old_fs;
	int sector_size = info.size/info.sector_count;

	if(!bootrom->path_name){
		printk("bm :pathname is NULL\n");
		return -1;
	}
	/*verify_bootrom*/
	if ((ret = verify_bootrom(bootrom->path_name)))
	{
		printk(KERN_ALERT "Verify bootrom error!\n");
		return ret;
	}
	memset(buf,0,sizeof(buf));
	if(FLASH_UNKNOWN == info.flash_id)
		return -1;
	/*open*/
	printk("bm :pathname is %s\n",bootrom->path_name);
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	filp = bm_sysfs_open(bootrom->path_name, O_RDONLY, 0);
	error = PTR_ERR(filp);
	if (IS_ERR(filp))
		{		
		set_fs(old_fs);
		return -1;
	}
	/*erase 0~6 sec of flash*/
	printk("bm :start to erase flash\n");
	for(i=0;i<BOOTROMSIZE/sector_size;i++){
		flash_sect_erase(i);
		printk(".");
	}
	flash_reset();
	printk("OK\n");
	/*cp file to flash*/
	printk("bm :start to read file\n");
	i = 0;
	do{
		ret = bm_sysfs_read(filp,buf,READLEN);
		debug("bm :get %d byte\n",ret);
		if (ret < 0){
			printk("bm :%s read fail at %d.\n",bootrom->path_name,ret);
			set_fs(old_fs);
			return -1;
		}	
		ptr = buf;
		for (j = i;j< i + ret/sector_size ;j++){
			flash_buffer_write(j,ptr,sector_size);
			ptr += sector_size;
		}
		if (ret > 0){
			i += ret/sector_size;
		}
		flash_buffer_write(i,ptr,ret%sector_size);
		printk("bm :.");
	}while(ret && ret == READLEN );
	printk("OK\n");
	filp_close(filp, NULL);
	set_fs(old_fs);
	printk("update bootrom is OK\n");
	return 0;
}

EXPORT_SYMBOL(mac_addr_get);

EXPORT_SYMBOL(do_get_or_save_boot_env);

