#ifndef __SEM_COMMON_H_
#define __SEM_COMMON_H_

#include <bits/pthreadtypes.h>

#define SEM_DEBUG

//#undef SEM_DEBUG

#ifdef SEM_DEBUG
#define SEM_DEBUG_PRINTF printf
#else
#define SEM_DEBUG_PRINTF
#endif
#define CF_SPACE_BUF_LEN	1024

#define MAX_SLOT_COUNT				20
#define RE_CONNECT_TIMEOUT			60

#define MD5_PATH "/home/admin/md5"
#define TESTFILE_PATH "/home/admin/testfile"

/*software version sync */
#define SOFTWARE_SYNC_TIMEOUT		100
enum version_syn_state
{
	IS_SYNCING,
	SYNC_DONE,
	SYNC_FAILED,
	SYNC_SUCCESS,
	SYNC_NO_NEED
};

typedef struct bm_op_cpld_read {
	unsigned char 	slot;		//which slot's CPLD to read from
	unsigned int	regAddr;	//CPLD register address
	unsigned char	regData;	//CPLD register value
}bm_op_cpld_args;

typedef struct bm_op_args_s {
	unsigned long long op_addr;
	unsigned long long op_value;  // ignore on read in arg, fill read value on write return value
	unsigned short op_byteorder; // destination byte order. default is bigendiana.
	unsigned short op_len;
	unsigned short op_ret; // 0 for success, other value indicate different failure.
} bm_op_args;


#define GET_BOOT_ENV	0
#define SAVE_BOOT_ENV 	1


#define SEM_FREE_SD		-1
#define FREE_THREAD_ID	0
#define USELESS_SLOT_ID	0xff

#define IS_DEAD			0
#define IS_ALIVING		!IS_DEAD

typedef struct boot_env
{
	char name[64];
	char value[128];
	int operation;
}boot_env_t;

typedef struct 
{
	pthread_t thread_index;
	int sd;
	int slot_id;
	int keep_alive_test_sd;
	int is_aliving;
}thread_index_sd_t;

typedef struct
{
	int slot_id;
	int version_sync_state;
	int version_sync_timeout;
}version_sync_t;

typedef struct bm_op_bit64_read {
	unsigned long long 	regAddr;	/*64-bit register address*/
	unsigned long long	regData;	/*64-bit register data*/
}bm_op_rbit64_args;

#define BM_IOC_MAGIC 			0xEC
#define BM_IOC_G_  				_IOWR(BM_IOC_MAGIC,1,bm_op_args) // read values
#define BM_IOC_X_ 				_IOWR(BM_IOC_MAGIC,2,bm_op_args) // write and readout wrote value
#define BM_IOC_CPLD_READ		_IOWR(BM_IOC_MAGIC,3,bm_op_cpld_args) 
#define BM_IOC_BIT64_REG_STATE 	_IOWR(BM_IOC_MAGIC,4,bm_op_rbit64_args)
#define BM_IOC_CPLD_WRITE   	_IOWR(BM_IOC_MAGIC,5,bm_op_cpld_args)
#define BM_IOC_ENV_EXCH  		_IOWR(BM_IOC_MAGIC, 10,boot_env_t)
#define BM_GET_POWER_STATE      _IOWR(BM_IOC_MAGIC, 19, sys_power_state_t)
#define BM_IOC_LOAD_QT2025_FIRMWARE     _IOWR(BM_IOC_MAGIC, 21, int)    /*cofigure SMI0 apply to IEEE802.3-2005 clause 45
                                                                        and load firmware for phy QT2025*/
#define BM_IOC_PORT_admin_status     _IOWR(BM_IOC_MAGIC, 22, int)
#define BM_IOC_READ_QT2025     _IOWR(BM_IOC_MAGIC, 23, int)
#define BM_IOC_FPGA_W 				_IOWR(BM_IOC_MAGIC,24,bm_op_args)//write fpga reg no delay


#define SEM_BM_FILE_PATH	"/dev/bm0"
#define SEM_INIT_STAGE_FILE_PATH "/var/run/sem.state"
#define ABORT_SYNC_VERSION_PATH "/dbm/parameters/abort_sync_version"
#define ABORT_CHECK_PRODUCT_STATE_REBOOT_PATH "/dbm/parameters/abort_check_product_state_reboot"

int sem_read_cpld(int reg, int *read_value);
int sem_write_cpld(int reg, int write_value);
int open_fd();
void close_fd();
void sem_init_completed(void);
extern int abort_sync_verion(void);
extern int snapshot_before_reset(void);

#if 0
/*cpld universial reg define*/
#define CPLD_TEST_REG						0x0
#define CPLD_PRODUCT_SERIAL_REG				0x1
#define CPLD_PRODUCT_TYPE_REG				0x2
#define	CPLD_SINGAL_BOARD_TYPE_REG			0x3
#define CPLD_PCB_VERSION_REG				0x4
#define CPLD_VERSION_REG					0x5
#define CPLD_HW_RESET_CONTROL_REG1			0x6
#define CPLD_HW_RESET_CONTROL_REG2			0x7
#define CPLD_SYS_SELFT_RESET_REG			0x8
#define CPLD_INTERRUPT_MASK_REG1			0x9
#define CPLD_INTERRUPT_MASK_REG2			0xa
#define CPLD_INTERRUPT_STATE_REG1			0xb
#define CPLD_INTERRUPT_STATE_REG2			0xc
#define CPLD_WATCHDOG_ENABLE_REG			0xd
#define CPLD_WATCHDOG_OVERFLOW_TIME_REG		0xe
#define CPLD_WATCHDOG_RESET_REG				0xf

#define CPLD_MASTER_BOARD_SLOTID_REG		0x28
#define CPLD_SLOT_ID_REG					0x28
#define CPLD_SLAVE_BOARD_SLOTID_REG			0x2e
#define CPLD_REMOTE_ON_STATE_REG			0x29
#define CPLD_REMOTE_MASTER_STATE_REG		0x37
#define CPLD_LOCAL_MASTER_STATE_REG			0x38

/*cpld universial reg mask*/
#define CPLD_PRODUCT_SERIAL_MASK 			0xf	//reg1
#define CPLD_PRODUCT_TYPE_MASK				0xff	//reg2
#define CPLD_SINGAL_BOARD_TYPE_MASK			0xff //reg3
#define SLOT_ID_MASK						0xf	//reg28

#endif
#define	SEM_THREAD_CREATE_SUCCESS		1
#define	SEM_THREAD_CREATE_FAILED		0


#define IS_DETACHED		1


#define MAX_SD_NUM	20



typedef struct
{
	int sd;
	struct sockaddr_tipc sock_addr;
	int slot;
}sd_sockaddr_tipc_t;


extern int g_send_sd;
extern int g_recv_sd;
extern struct sockaddr_tipc g_send_sock_addr;
extern struct sockaddr_tipc g_recv_sock_addr;

extern sd_sockaddr_tipc_t g_send_sd_arr[];
extern sd_sockaddr_tipc_t g_recv_sd_arr[];

extern int dbm_effective_flag;

extern void sem_send_signal_to_trap(void);
extern int sem_sor_exec(char* cmd,char* filename,int time);
#endif
