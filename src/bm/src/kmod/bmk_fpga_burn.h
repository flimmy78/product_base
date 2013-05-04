
#ifndef __AX86_FPGA_H__
#define __AX86_FPGA_H__

#define uint32_t unsigned long
#define uint64_t unsigned long long

#define GRAPH_MAX_SIZE (100*1024)

 /*Start Accelerate fpga burning*/

 #define SYSTEM_STATE_REG_ADDR      (0x800000001d070006)
 #define FPGA_VERSION_REG_ADDR      (0x800000001d070002)

 #define MIO_BOOT_REG_CFG1_ADDR      (0x8001180000000008)
 #define MIO_BOOT_REG_TIM1_ADDR      (0x8001180000000048)
 
 //#define MIO_BOOT_REG_CFG1_VALUE    0x10080001d01
 
 #define MIO_BOOT_REG_CFG1_VALUE    (0x10000000000)
 #define MIO_BOOT_REG_TIM1_VALUE    (0x820820820820820)
 
 //#define MIO_BOOT_REG_CFG1_ORIGINAL_VALUE    0x80001d01
 //#define MIO_BOOT_REG_TIM1_ORIGINAL_VALUE    0xfffffffffffffff

/*end*/

 /*CPLD Register Addresses*/
 #define CPLD_Program_Reg    (0x800000001d0100c0)
 #define CPLD_Cclk_Reg     (0x800000001d0100c1)
 #define CPLD_Init_Reg         (0x800000001d0100c2)

 /*Program Register*/
#define CPLD_INIT_BIT  (0x01)
#define CPLD_DONE_BIT  (0x02)

#define CPLD_PROG_LOW   (0x00)
#define CPLD_PROG_HIGH   (0x01)

#define CPLD_CCLK_NO  (0x01)
#define CPLD_CCLK_SET  (0x03)

#define SYSTEM_CONTROL_REGISTER (0x800000001d070004)    
#define SYSTEM_CONTROL_REGISTER_WORK_VALUE (0xaf14)

#endif




