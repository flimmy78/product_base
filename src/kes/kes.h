#ifndef _KES_H 
#define _KES_H

#define KES_MEM_BLOCK_NAME "__kes_mem"
#define KES_PROC_NAME "kes_proc"
#define KES_FLAG_NAME "kes_flag"
#define KES_MEM_BLOCK_SIZE (1 * 1024 * 1024)
#define HALF_KES_MEM_BLOCK_SIZE (512 * 1024)
#define QUARTER_KES_MEM_BLOCK_SIZE (256 * 1024)

#define CVT_BUF_MAX 1023
#define KES_MEM_SHOW_LEN (4 * 1024)
#define KES_MEM_HEADER_LEN 32
#define KES_PEOTECT_LEN 16
#define KES_MAGIC_LEN 8
#define KES_ISENABLE_LEN 8

#define KES_POS_LEN ((HALF_KES_MEM_BLOCK_SIZE)/(KES_MEM_SHOW_LEN))
#define FWD_POS_LEN ((QUARTER_KES_MEM_BLOCK_SIZE)/(KES_MEM_SHOW_LEN))
#define FWD_PROC_NAME "fastfwd_exception_log_proc"


typedef struct
{
	unsigned char protect[KES_PEOTECT_LEN];
	unsigned char magic[KES_MAGIC_LEN];
	unsigned char isenable[KES_ISENABLE_LEN];
}kes_mem_header_type;


#define NVRAM_MEM_SIZE 32 * 1024
#define NVRAM_MEM_HEADER_LEN 32
#define NVRAM_MEM_FLAG_LEN 4
#define NVRAM_MEM_TIME_LEN 28

typedef struct nvram_mem_header_s
{
	unsigned char flag[NVRAM_MEM_FLAG_LEN];
	unsigned char time[NVRAM_MEM_TIME_LEN];
}nvram_mem_header_t;


extern int dump_msg_to_kes_mem(char *buff, int size );
extern int print_msg_to_kes_mem(const char * fmt, ...);
extern int print_msg_to_nvram(const char * fmt,...);

#endif
