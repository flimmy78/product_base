
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/syscalls.h>

#include <asm/octeon/cvmx.h>
#include "bmk_fpga_burn.h"

extern void *vmalloc(unsigned long size);
extern void vfree(const void *addr);
extern ssize_t bm_sysfs_read(struct file *filp, char *buf, size_t count);
extern struct file* bm_sysfs_open(const char *filename, int flags, int mode);

  /***********************************************************************
   *
   * Function:     check_done_bit_ok
   *
   * Description:  This function takes monitors the CPLD Input Register
   *               by checking the status of the DONE bit in that Register.
   *               By doing so, it monitors the Xilinx Virtex device's DONE
   *               Pin to see if configuration bitstream has been properly
   *               loaded
   *
   * Parameters:
   *      void
   *
   *
   * Returns:
   *      void
   *
   * Called By:
   *      fpga_online_burning()
   *
   * History:
   *      3-October-2001  MN/MP  - Created
   *
   ***********************************************************************/

void check_done_bit_ok(void)
{

	uint32_t data16;
    	uint32_t done;
    	uint32_t init;
    	uint32_t extra_cclk;
	int i =1;

    	done = 0;
    	init = 1;

    	//printk(KERN_ERR"Checking fpga state,waiting\n");
    	while ( (done==0) && (init==1) && (i<=30) )		//( (done==0) && (init==1) )
    	{
    		//printk(KERN_ERR"***check done bit*** \n");

		    mdelay(3000);
        	//Apply additional CCLK pulse until DONE=1
        	cvmx_write64_uint8(CPLD_Cclk_Reg, CPLD_CCLK_NO );  //CCLK=0, DATA=1
        	cvmx_write64_uint8(CPLD_Cclk_Reg, CPLD_CCLK_SET );  //CCLK=1, DATA=1

        	//Read CPLD Input Register:
        	data16 = cvmx_read64_uint8(CPLD_Init_Reg);

        	//Check the DONE bit of the CPLD Input Register:
        	done = (data16 & CPLD_DONE_BIT) ? 1 : 0;

        	//Check the INIT bit of the CPLD Input Register
        	init = (data16 & CPLD_INIT_BIT) ? 1 : 0;
				
		//printk(KERN_ERR"***done=%d   init=%d   i=%d*** \n",done, init,i);
		    i=i+1;
		
        		//Note: FPGA user design should drive INIT High or make INIT HIGHZ
        		//      to ensure no false INIT error detected by this code.
    	}

    if (done == 1)
	{                     //If DONE Pin is High, display message
	    for(extra_cclk=0;extra_cclk<8;extra_cclk++)
		{
			//Apply additional CCLK pulse to ensure end-of-startup
	        cvmx_write64_uint8(CPLD_Cclk_Reg, CPLD_CCLK_NO );  //CCLK=0, DATA=1
	        cvmx_write64_uint8(CPLD_Cclk_Reg, CPLD_CCLK_SET );  //CCLK=1, DATA=1
	        //Note:  FPGA user design should keep CCLK and DATA HIGHZ to avoid
	        //       conflict with this code.
	    }
		printk(KERN_INFO"LOAD FPGA:Successful! DONE is High!\n");
    }
    else
	{//done==0 and init==0

		printk(KERN_ERR"LOAD FPGA:Configuration Failed, DONE is Low, Init is Low\n");
    }
	
}


  /***********************************************************************
   *
   * Function:     shift_data_out
   *
   * Description:  This function takes a 16-bit configuration word,
   *               and serializes it, MSB first, LSB Last
   *               This function is also responsible for strobing the
   *               Configuration Clock (CCLK)
   *
   * Parameters:
   *      data16 = 16 bit configuration word previously stored in flash
   *               memory
   *
   * Returns:
   *      void
   *
   * Called By:
   *      __fpga_online_burning()
   *
   * History:
   *      3-October-2001  MN/MP  - Created
   *
   ***********************************************************************/

void
shift_data_out( char data16 )
{
	uint32_t data_out;
   	uint32_t SCLK_LOW;
    	uint32_t SCLK_HIGH;

    	SCLK_LOW  = 0x0000;
    	SCLK_HIGH = 0x0002;

	//printk("\n\n**0x%x**\n\n",&data16);

// --  Lower Byte:

    data_out = (data16 & 0x0001) ? 1 : 0;
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_LOW  | data_out );  //1th Bit, and Bring SCLK Low
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_HIGH | data_out );  //SCLK High

    data_out = (data16 & 0x0002) ? 1 : 0;
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_LOW  | data_out );  //2th Bit, and Bring SCLK Low
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_HIGH | data_out );  //SCLK High

    data_out = (data16 & 0x0004) ? 1 : 0;
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_LOW  | data_out );  //3th Bit, and Bring SCLK Low
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_HIGH | data_out );  //SCLK High

    data_out = (data16 & 0x0008) ? 1 : 0;
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_LOW  | data_out );  //4th Bit, and Bring SCLK Low
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_HIGH | data_out );  //SCLK High

    data_out = (data16 & 0x0010) ? 1 : 0;
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_LOW  | data_out );  //5th Bit, and Bring SCLK Low
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_HIGH | data_out );  //SCLK High

    data_out = (data16 & 0x0020) ? 1 : 0;
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_LOW  | data_out );  //6th Bit, and Bring SCLK Low
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_HIGH | data_out );  //SCLK High

    data_out = (data16 & 0x0040) ? 1 : 0;
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_LOW  | data_out );  //7th Bit, and Bring SCLK Low
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_HIGH | data_out );  //SCLK High

    data_out = (data16 & 0x0080) ? 1 : 0;
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_LOW  | data_out );  //8th Bit, and Bring SCLK Low
    cvmx_write64_uint8(CPLD_Cclk_Reg, SCLK_HIGH | data_out );  //SCLK High


}


/*BE CAREFUL THIS IS BYTESWAPPED!*/

 /***********************************************************************
  *
  * Function:     __fpga_online_burning
  *
  * Description:  Initiates fpga online burning Configuration.
  *               Calls shift_data_out() to output serial data
  *
  * Parameters:
  *   flash_start_addr = Address value where Virtex bitstream begins
  *   flash_end_addr   = Address value where Virtex bitstream ends
  *
  * Returns:
  *   void
  *
  * Called By:
  *   fpga_online_burning
  *
  * History:
  *   3-October-2001  MN/MP  - Created
  *
  ***********************************************************************/


int __fpga_online_burning(struct file *fd)
{
	uint32_t data16;

	int result;
	int len = 2;
	int i = 1;
	int test_program = 1;
	uint32_t tmp_file_chars;
	uint64_t cfg1_original_value = 0x0;
	uint64_t tim1_original_value = 0x0;
	uint64_t cfg1_value = 0x0;

	int flag = 0;

#if 1

	unsigned char * temp_graph_file_buff;

	temp_graph_file_buff = vmalloc(GRAPH_MAX_SIZE);

	if(temp_graph_file_buff == NULL)
	{
		printk(KERN_ERR "LOAD FPGA:Malloc error for fpga burning  file.\n");
		return -1;
	}

#endif

     /**************************************************************************
       *Toggle Program Pin by Toggling Program_OE bit
       *This is accomplished by writing to the Program Register in the CPLD
       *
       *NOTE: The Program_OE bit should be driven high to bring the Virtex
       *     	   Program Pin low. Likewise, it should be driven low to bring the Virtex Program Pin to High-Z
       *
       ***************************************************************************/
#if 1
	 //printk(KERN_ERR"CPLD program bit writing low and high!\n");
	 cvmx_write64_uint8(CPLD_Program_Reg, CPLD_PROG_LOW); //PROGRAM_OE LOW
	 mdelay(200);


    /************************************************************************
      *Bring Program High-Z
      *(Drive Program_OE bit low to bring Virtex Program Pin to High-Z
      ************************************************************************/

    //Program_OE bit Low brings the Virtex Program Pin to High Z:
    //printk(KERN_ERR"CPLD program bit writing high !\n");
    cvmx_write64_uint8(CPLD_Program_Reg, CPLD_PROG_HIGH);
	//mdelay(200);
#endif

#if 0
	 while(test_program <= 200)
	{
	 	cvmx_write64_uint8(CPLD_Program_Reg, CPLD_PROG_LOW); //PROGRAM_OE LOW
	 	mdelay(2);
	 	cvmx_write64_uint8(CPLD_Program_Reg, CPLD_PROG_HIGH); //PROGRAM_OE HIGH
	 	mdelay(2);	 
		test_program++;
		printk(KERN_ERR"low to high =%d!\n",test_program);
	}
#endif

    /********************************************************************
      *Check for /INIT after Program is High Z:
      ********************************************************************/

	//printk(KERN_ERR"Check Status of INIT !\n");
	
    data16 = 0;  //Initialize data16 variable before entering While Loop

    while(data16 == 0)
	{
		data16 = cvmx_read64_uint8(CPLD_Init_Reg);    //Read Init Register
        data16 = data16 & CPLD_INIT_BIT;           //Check Status of /INIT
	}

    /* Begin fpga online burning Configuration */
	//printk(KERN_ERR "write data to fpga\n");

#if 1
	//unsigned long test_time;
	//test_time = jiffies;
	//printk(KERN_ERR "start read file time is [%u]\n",test_time);

    /*Accelerate fpga burning*/
    
    cfg1_original_value = cvmx_read64_uint64(MIO_BOOT_REG_CFG1_ADDR);
    cfg1_value = cfg1_original_value | MIO_BOOT_REG_CFG1_VALUE;
	
    tim1_original_value = cvmx_read64_uint64(MIO_BOOT_REG_TIM1_ADDR);

    cvmx_write64_uint64(MIO_BOOT_REG_CFG1_ADDR, cfg1_value);
	cvmx_write64_uint64(MIO_BOOT_REG_TIM1_ADDR, MIO_BOOT_REG_TIM1_VALUE);
	
	printk(KERN_ERR "LOAD FPGA:burn fpga logic,please wait for few seconds...\n");
	result = bm_sysfs_read(fd,(char *)temp_graph_file_buff,GRAPH_MAX_SIZE);
	while(result > 0 )
	{
		/*printk(KERN_ERR "result = %d flag = %d\n", result, ++flag);*/
		for(i=0; i<result; i++)
		{
			shift_data_out(*(temp_graph_file_buff+ i));
		}
	
		result = bm_sysfs_read(fd,(char *)temp_graph_file_buff,GRAPH_MAX_SIZE);
	}
	
	//printk(KERN_ERR "read file time is [%u]\n",(jiffies - test_time));

    cvmx_write64_uint64(MIO_BOOT_REG_CFG1_ADDR, cfg1_original_value);
	cvmx_write64_uint64(MIO_BOOT_REG_TIM1_ADDR, tim1_original_value);

	/*end accelerate fpga burning*/

#endif
	
	printk(KERN_ERR"LOAD FPGA:write data to fpga done \n");
	vfree(temp_graph_file_buff);
    return 0;

}



int fpga_online_burning(struct file *fd )
{
	int ret;
	uint64_t system_state_reg_value = 0x0;
	uint64_t fpga_version_reg_value = 0x0;

	ret = __fpga_online_burning(fd);
    if(ret)
    {
        printk(KERN_ERR "LOAD FPGA:__fpga_online_burning failed.\n");
		return -1;
	}
  /***********************************************************************
    * Monitor the DONE bit in the CPLD Input Register to see if
    * configuration successful
    ***********************************************************************/

  	check_done_bit_ok();

	fpga_version_reg_value = (uint64_t) cvmx_read64_uint16(FPGA_VERSION_REG_ADDR);
	printk(KERN_ERR "\n***** FPGA Version is [0x%x] *****\n", fpga_version_reg_value);
	
    system_state_reg_value = (uint64_t) cvmx_read64_uint16(SYSTEM_STATE_REG_ADDR);
	printk(KERN_ERR "\n***** System State Register Value is [0x%x] *****\n", system_state_reg_value);
	
	//cvmx_write64_uint16(SYSTEM_CONTROL_REGISTER, SYSTEM_CONTROL_REGISTER_WORK_VALUE);
	//mdelay(200);
	
	if(system_state_reg_value != 0xffff)
	{
        printk(KERN_ERR "\n*****FPGA initialization unfinished *****\n");
		return -1;
	}
	
	cvmx_write64_uint16(SYSTEM_CONTROL_REGISTER, SYSTEM_CONTROL_REGISTER_WORK_VALUE);
	printk(KERN_ERR "\n***** System control Register Value is [0x%x] *****\n", SYSTEM_CONTROL_REGISTER_WORK_VALUE);

	//mdelay(200);
	
    //printk(KERN_ERR "\n*****LOAD FPGA OK *****\n");
    return 0;
}

int read_bin_to_fpga(char *filename)
{

	//int fd;
	int ret;
	struct file *fd = NULL;
	mm_segment_t old_fs= get_fs();
	printk(KERN_INFO"LOAD FPGA:read_bin_to_fpga start.\n");
#if 0
    int result;
    //int i=0;
	int len = 2;
    char tmp_file_chars[2];
#endif

	if(filename == NULL) 
	{
		printk(KERN_ERR"LOAD FPGA:FPGA burning filename error!\n");	
		return -1;
	}
	set_fs(KERNEL_DS);
	fd = bm_sysfs_open(filename, O_RDONLY, 0);
	
	//if (fd>=0)
	if (!IS_ERR(fd))
	{
#if 0
		result = bm_sysfs_read(fd, tmp_file_chars, len);

		while((result>0)&&(i<256))
		{
			printk("0x%x	         0x%x",tmp_file_chars[0],tmp_file_chars[1]);
                		//if( i++ %16 == 0 )
                    		//printk("\n");
		  	result = bm_sysfs_read(fd, tmp_file_chars, 2);
		}
#endif		
		printk(KERN_INFO"LOAD FPGA:FPGA online burning function start !\n");	

		ret = fpga_online_burning(fd);
        if(ret)
        {
            printk(KERN_ERR"LOAD FPGA:fpga_online_burning failed!\n");
			sys_close(fd);
		    set_fs(old_fs);		
			return -1;
		}
		sys_close(fd);
		set_fs(old_fs);		
		return 0;
	}
	else
	{
        printk(KERN_ERR, "LOAD FPGA:Open file:%s error!\n", filename);
		set_fs(old_fs);
		return -1;
	}

}


