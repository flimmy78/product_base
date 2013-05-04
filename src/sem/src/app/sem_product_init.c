#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <linux/tipc.h>
#include <fcntl.h>
#include <unistd.h>

#include "sem_common.h"
#include "sem/sem_tipc.h"
#include "sysdef/sem_sysdef.h"
#include "sem/product.h"

#include "product/board/board_feature.h"
#include "product/product_feature.h"

//#include "product/product_ax8610.h"
//#include "product/product_ax7605i.h"


#define MAX_PRODUCT_NUM		10



extern product_fix_param_t dt_ax7605i_fix_param;
extern product_fix_param_t dt_ax8610_fix_param;
extern product_fix_param_t dt_ax8606_fix_param;
extern product_fix_param_t dt_ax8603_fix_param;
extern product_fix_param_t dt_ax8800_fix_param;
extern product_fix_param_t dt_single_board_fix_param;

extern int sem_read_cpld(int reg, int *read_value);
extern int sem_write_cpld(int reg, int write_value);
extern int get_product_serial(int *product_serial);
extern int get_board_type(int *board_type);
extern int product_dbm_file_create(product_info_syn_t *product);

product_fix_param_t **product_type_array = NULL;
board_fix_param_t **module_basic_info = NULL;

product_fix_param_t *product = NULL;
product_info_syn_t product_info_syn;


product_fix_param_t *dt_series_product_param_arr[MAX_PRODUCT_NUM] =
{
	[PRODUCT_AX7605I] = &dt_ax7605i_fix_param,
	[PRODUCT_AX8610] = &dt_ax8610_fix_param,
	[PRODUCT_AX8606] = &dt_ax8606_fix_param,
	[PRODUCT_AX8603] = &dt_ax8603_fix_param,
	[PRODUCT_AX8800] = &dt_ax8800_fix_param,
	[PRODUCT_MAX_NUM] = NULL
	//[PRODUCT_AX8610] = &dt_ax8610_fix_param
};

int get_product_serial(int *product_serial)
{
	int ret = 0;
	ret = sem_read_cpld(CPLD_PRODUCT_SERIAL_REG, product_serial);

	if (ret)
	{
		sem_syslog_dbg("sem get cpld reg %d failed\n", CPLD_PRODUCT_SERIAL_REG);
		return ret;
	}

	*product_serial = *product_serial & CPLD_PRODUCT_SERIAL_MASK;
	return ret;
}

int dt_product_hw_type_get(int *product_id)
{
	int ret = 0;
	int reg_data = 0;
	ret = sem_read_cpld(CPLD_PRODUCT_TYPE_REG, &reg_data);
	if (ret)
	{
		sem_syslog_dbg("sem read cpld reg %d failed\n", CPLD_PRODUCT_TYPE_REG);
		return ret;
	}
	sem_syslog_dbg("*product_id = 0x%x\n", reg_data);
	*product_id = (reg_data>>2) & CPLD_PRODUCT_TYPE_MASK;

	//huxuefeng modified for adjusting hardware requirement
	/*if (*product_id == 0x7)
	{
		*product_id = XXX_YYY_AX8610;
	}
	*/
	if (*product_id == 0x3)
	{
		*product_id = XXX_YYY_AX7605I;
	}
	
    return ret;
}

unsigned long dt_local_module_hw_code_get()
{
	return 0;
}

/*
 *read cpld reg 4 to get the pcb version
 *
 */
unsigned long dt_board_hw_version_get()
{
	return 0;
}

int dt_board_hw_slotno_get()
{
	return 0;	
}

int read_product_sysinfo()
{
	return 0;
}

int set_product(int product_type)
{
	int i;
	for (i=0; i<MAX_PRODUCT_NUM && dt_series_product_param_arr[i]; i++)
	{
		if (product_type == dt_series_product_param_arr[i]->product_type)
		{
			product = dt_series_product_param_arr[i];
			sem_syslog_dbg("find correct product:%s\n", product->name);
			return 0;
		}
	}
	sem_syslog_dbg("not find the correct product\n");
	product = NULL;
	return -1;
}



void get_product_name(char *name)
{
	int product_serial;
	int board_type;

	get_product_serial(&product_serial);
	get_board_type(&board_type);

	if(product_serial == 3)
	{
		if (board_type == 0)
		{
			strcpy(name, "AU3524");
		}
		else if (board_type == 1)
		{
			strcpy(name, "AU3052");
		}
		else if (board_type == 2)
		{
			strcpy(name, "AU3028");
		}
		else if (board_type == 3)
		{
			strcpy(name, "AU3052P");
		}
		else if (board_type == 4)
		{
			strcpy(name, "AU3028P");
		}
		else if (board_type == 5)
		{
			strcpy(name, "AU3524P");
		}
		else
		{
			strcpy(name, "UNKNOWN NAME");
		}
	}
	else if (product_serial == 4)
	{
		if (board_type == 1)
		{
			strcpy(name, "AU4624");
		}
		else if (board_type == 2)
		{
			strcpy(name, "AU4524");
		}
		else if (board_type == 3)
		{
			strcpy(name, "AU4626P");
		}
		else if (board_type == 4)
		{
			strcpy(name, "AU4524P");
		}
		else
		{
			strcpy(name, "UNKNOWN NAME");
		}
	}
	else if (product_serial == 5)
	{
		if (board_type == 0)
		{
			strcpy(name, "AX_5612");
		}
		else if (board_type == 3)
		{
			strcpy(name, "AX_5612I");
		}
		else if (board_type == 0x61)
		{
			strcpy(name, "AX_5612E");
		}
		else if (board_type == 0x62)
		{
			strcpy(name, "AX_5608");
		}
		else
		{
			strcpy(name, "UNKNOWN NAME");
		}
	}
	else if (product_serial == 7)
	{
		if (board_type == 0x80)
		{
			strcpy(name, "AX_7605");
		}
		else
		{
			strcpy(name, "UNKNOWN NAME");
		}
	}
	else
	{
		sem_syslog_dbg("product serial = %d  module = %d\n", product_serial, board_type);
		/*wrong product id*/
		strcpy(name, "UNKNOWN NAME");
	}
}

int product_init_in_single_board_mode()
{
	int fd;
	char c;
	fd = open(PRODUCT_INIT_IN_SINGLE_BOARD_MODE, O_RDONLY);
	if(fd < 0)
	{
		sem_syslog_dbg("file %s open failed\n", PRODUCT_INIT_IN_SINGLE_BOARD_MODE);
		return 0;
	}
	read(fd, &c, 1);
	close(fd);
	if(c == '1')
		return 1;
	
	return 0;
}

int product_single_board_init(int product_series, int product_type)
{
	int ret;
	product_fix_param_t *temp_product;
	int i;
	
	if (set_product(product_type)) {
		
	}
	else {
		temp_product = product;
		product = &dt_single_board_fix_param;

		product->slotcount = temp_product->slotcount;
		product->fan_num = temp_product->fan_num;
		product->board_on_mask = temp_product->board_on_mask;
		
		product->product_serial = product_series;
		product_info_syn.board_on_mask = product->board_on_mask;
		product_info_syn.default_master_slot_id = product->default_master_slot_id;
		product_info_syn.fan_num = product->fan_num;
		product_info_syn.is_distributed = product->is_distributed;
		product_info_syn.master_slot_count = product->master_slot_count;

		for (i = 0; i < product->slotcount; i++)
		{
			product->product_slot_board_info[i].board_code = -1;
			product->product_slot_board_info[i].board_type = -1;
			product->product_slot_board_info[i].slot_id = -1;
			strcpy(product->product_slot_board_info[i].name, "unkown");
			product->product_slot_board_info[i].is_master = 0;
			product->product_slot_board_info[i].is_active_master = 0;
			product->product_slot_board_info[i].is_use_default_master = 0;
			product->product_slot_board_info[i].function_type = UNKNOWN_BOARD;
			product->product_slot_board_info[i].board_state = BOARD_EMPTY;
			product->product_slot_board_info[i].asic_start_no = -1;
			product->product_slot_board_info[i].asic_port_num = -1;
		}

		for (i = 0; i < MAX_MASTER_SLOT_NUM; i++)
		{
			product_info_syn.master_slot_id[i] = product->master_slot_id[i];
		}
		
		product_info_syn.more_than_one_master_board_on = product->more_than_one_master_board_on;
		strcpy(product_info_syn.name, product->name);
		product_info_syn.product_type = product->product_type;
		product_info_syn.product_serial = product->product_serial;
		product_info_syn.slotcount = product->slotcount;
		product_info_syn.product_state = product->product_state;
		for (i = 0; i < MAX_SUPPORT_BOARD_NUM; i++)
		{
			product_info_syn.product_slot_board_info[i] = product->product_slot_board_info[i];
		}
		strcpy(product_info_syn.sn, product->sn);

		product->product_dbm_file_create(&product_info_syn);
	}

	return 0;
}


int product_init(void)
{
	int ret;
	int product_type;
	int product_serial;
	
	char name[PRODUCT_NAME_LEN];
	/*open /dev/bm0 to get ready the fd for reading and writing cpld reg*/	
	ret = open_fd();
	if (ret)
	{
		sem_syslog_dbg("Error:fd open failed,cann't get cpld register infomation\n");
		return -1;
	}

	ret = dt_product_hw_type_get(&product_type);

	if (ret)
	{
		sem_syslog_dbg("get product type failed\n");
	}

	ret = get_product_serial(&product_serial);
	
	if (ret)
	{
		sem_syslog_dbg("get product type failed\n");
	}

	if (product_init_in_single_board_mode()) {
		product_single_board_init(product_serial, product_type);
		return 0;
	}

	if (set_product(product_type))
	{
		#if 0
		if (product_type != XXX_YYY_AX7605I && product_type != XXX_YYY_AX8610)
		{
		#endif
		
		sem_syslog_dbg("the product is not distributed\n");
		get_product_name(name);
		product_info_syn.board_on_mask = 0x1;
		product_info_syn.default_master_slot_id = 1;
		product_info_syn.fan_num = 1;
		product_info_syn.is_distributed = 0;
		product_info_syn.master_slot_count = 1;
		product_info_syn.master_slot_id[0] = 1;
		product_info_syn.more_than_one_master_board_on = 0;
		product_info_syn.active_master_slot_id = 1;
		strcpy(product_info_syn.name, name);
		product_info_syn.product_slot_board_info[1].board_code = 1;
		product_info_syn.product_slot_board_info[1].board_type = 1;
		product_info_syn.product_slot_board_info[1].slot_id = 1;
		strcpy(product_info_syn.product_slot_board_info[1].name, name);
		product_info_syn.product_slot_board_info[1].is_master = 1;
		product_info_syn.product_slot_board_info[1].is_active_master = 1;
		product_info_syn.product_slot_board_info[1].is_use_default_master = 1;
		product_info_syn.product_slot_board_info[1].function_type = MASTER_BOARD|AC_BOARD|SWITCH_BOARD|BASE_BOARD|NAT_BOARD; 
		product_info_syn.product_slot_board_info[1].board_state = BOARD_READY;
		product_info_syn.product_type = product_type;
		product_info_syn.product_serial = product_serial;
		product_info_syn.slotcount = 1;
		product_info_syn.product_state = SYSTEM_RUNNING;
		product_dbm_file_create(&product_info_syn);
		return 0;
			
		#if 0
		}
		else
		{
			sem_syslog_dbg("this product is unknown product ,product_type is 0x%x\n", product_type);
			return -1;
		}
		#endif
		
	}

	product->product_serial = product_serial;
	/*get product sysinfo*/
	product->get_product_sys_info(&product->sys_info);
	//product->get_board_on_mask(&product->board_on_mask);

	product_info_syn.board_on_mask = product->board_on_mask;
	product_info_syn.default_master_slot_id = product->default_master_slot_id;
	product_info_syn.fan_num = product->fan_num;
	product_info_syn.is_distributed = product->is_distributed;
	product_info_syn.master_slot_count = product->master_slot_count;
	//product_info_syn.product_state= PRODUCT_INITIAL;
	for (ret=0; ret<product->slotcount; ret++)
	{
		product->product_slot_board_info[ret].board_code = -1;
		product->product_slot_board_info[ret].board_type = -1;
		product->product_slot_board_info[ret].slot_id = -1;
		strcpy(product->product_slot_board_info[ret].name, "unkown");
		product->product_slot_board_info[ret].is_master = 0;
		product->product_slot_board_info[ret].is_active_master = 0;
		product->product_slot_board_info[ret].is_use_default_master = 0;
		product->product_slot_board_info[ret].function_type = UNKNOWN_BOARD;
		product->product_slot_board_info[ret].board_state = BOARD_EMPTY;
		product->product_slot_board_info[ret].asic_start_no = -1;
		product->product_slot_board_info[ret].asic_port_num = -1;
	}

	for (ret = 0; ret < MAX_MASTER_SLOT_NUM; ret++)
	{
		product_info_syn.master_slot_id[ret] = product->master_slot_id[ret];
	}
	
	product_info_syn.more_than_one_master_board_on = product->more_than_one_master_board_on;
	strcpy(product_info_syn.name, product->name);
	product_info_syn.product_type = product->product_type;
	product_info_syn.product_serial = product->product_serial;
	product_info_syn.slotcount = product->slotcount;
	product_info_syn.product_state = product->product_state;
	for (ret=0; ret<MAX_SUPPORT_BOARD_NUM; ret++)
	{
		product_info_syn.product_slot_board_info[ret] = product->product_slot_board_info[ret];
	}
	strcpy(product_info_syn.sn, product->sn);

	product->product_dbm_file_create(&product_info_syn);
	return 0;
}


#ifdef __cplusplus
}
#endif
