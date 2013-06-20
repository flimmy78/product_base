#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/tipc.h>
#include "sem_common.h"
#include "sem/sem_tipc.h"
#include "product/product_feature.h"
#include "product/product_ax7605i.h"
#include "product/product_ax8610.h"
#include "product/product_ax8606.h"
#include "product/product_ax8603.h"
#include "product/board/board_feature.h"
#include "config/auteware_config.h"


extern int sem_read_cpld(int reg, int *read_value);
extern int sem_write_cpld(int reg, int write_value);
extern product_fix_param_t *product;
extern product_info_syn_t product_info_syn;

extern int product_dbm_file_create(product_info_syn_t *product);

board_fix_param_t *local_board = NULL;
board_info_syn_t board_info_to_syn;
int update_mac_enable = 1;
int update_sn_enable = 1;

#if 0
int get_product_serial(int *product_serial)
{
	int ret = 0;
	int reg_data = 0;
	ret = sem_read_cpld(CPLD_PRODUCT_SERIAL_REG, &reg_data);
	if (ret)
	{
		return ret;
	}
	*product_serial = reg_data & CPLD_PRODUCT_SERIAL_MASK;
	return ret;
}
#endif
int get_board_type(int *board_type)
{
	int ret = 0;
	int reg_data = 0;
	ret = sem_read_cpld(CPLD_SINGAL_BOARD_TYPE_REG, &reg_data);

	if (ret)
	{
		return ret;
	}
	*board_type = reg_data & CPLD_SINGAL_BOARD_TYPE_MASK;
	return ret;	
}

#if 0
int get_slot_id(int *slot_id)
{
	int ret = 0;
	int reg_data = 0;
	ret = sem_read_cpld(0x28, &reg_data);

	if (ret)
	{
		return ret;
	}
	*slot_id = reg_data & SLOT_ID_MASK;
	
	return ret;	
}
#endif

int get_board_index(int product_serial, int product_type, int board_type, int *index)
{
	switch (product_type)
	{
		case XXX_YYY_AX7605I:
			if (board_type == 4)
			{
				*index = BOARD_TYPE_AX71_2S12G12S;
				return 0;
			}
			else if (board_type == 5)
			{
				*index = BOARD_TYPE_AX71_1X12G12S;
				return 0;
			}
			else if (board_type == 0x80)
			{
				*index = BOARD_TYPE_AX71_CRSMU;
				return 0;
			}
			else
			{
				*index = -1;
				return -1;
			}
			break;
		case XXX_YYY_AX8610:
			if (board_type == 0x0)
			{
				*index = BOARD_TYPE_AX81_SMU;
				return 0;
			}
			else if (board_type == 0x1)
			{
				*index = BOARD_TYPE_AX81_AC12C;
				return 0;
			}
			else if (board_type == 0x4)
			{
				*index = BOARD_TYPE_AX81_2X12G12S;
				return 0;
			}
			else if (board_type == 0x2)
			{
				*index = BOARD_TYPE_AX81_AC8C;
				return 0;
			}
			else if (board_type == 0x5)
			{
				*index = BOARD_TYPE_AX81_1X12G12S;
				return 0;
			}
			else if (board_type == 0x3)
			{
				*index = BOARD_TYPE_AX81_12X;
				return 0;
			}
			else if (board_type == 0x6)
			{
				*index = BOARD_TYPE_AX81_AC4X;
				return 0;
			}
			else
			{
				*index = -1;
				return -1;
			}
			break;
		case XXX_YYY_AX8603:
			if (board_type == 0x1) {
				*index = BOARD_TYPE_AX81_AC12C;
				return 0;
			} else if (board_type == 0x2) {
				*index = BOARD_TYPE_AX81_AC8C;
				return 0;
			} else if (board_type == 0x4) {
				*index = BOARD_TYPE_AX81_2X12G12S;
				return 0;
			} else if (board_type == 0x3) {
				*index = BOARD_TYPE_AX81_12X;
				return 0;
			} else if (board_type == 0x5) {
				*index = BOARD_TYPE_AX81_1X12G12S;
				return 0;
			} else if (board_type == 0x6) {
				*index = BOARD_TYPE_AX81_AC4X;
				return 0;
			} else {
				*index = -1;
				return -1;
			}
			break;
		case XXX_YYY_AX8606:
			if (board_type == 0x0)
			{
				*index = BOARD_TYPE_AX81_SMU;
				return 0;
			}
			else if (board_type == 0x1)
			{
				*index = BOARD_TYPE_AX81_AC12C;
				return 0;
			}
			else if (board_type == 0x4)
			{
				*index = BOARD_TYPE_AX81_2X12G12S;
				return 0;
			}
			else if (board_type == 0x2)
			{
				*index = BOARD_TYPE_AX81_AC8C;
				return 0;
			}
			else if (board_type == 0x5)
			{
				*index = BOARD_TYPE_AX81_1X12G12S;
				return 0;
			}
			else if (board_type == 0x3)
			{
				*index = BOARD_TYPE_AX81_12X;
				return 0;
			}
			else if (board_type == 0x6)
			{
				*index = BOARD_TYPE_AX81_AC4X;
				return 0;
			}
			else
			{
				*index = -1;
				return -1;
			}
			break;
		case XXX_YYY_AX8800:
			if (board_type == 0x0)
			{
				*index = BOARD_TYPE_AX81_SMU;
				return 0;
			}
			else if (board_type == 0x1)
			{
				*index = BOARD_TYPE_AX81_AC12C;
				return 0;
			}
			else if (board_type == 0x4)
			{
				*index = BOARD_TYPE_AX81_2X12G12S;
				return 0;
			}
			else if (board_type == 0x2)
			{
				*index = BOARD_TYPE_AX81_AC8C;
				return 0;
			}
			else if (board_type == 0x5)
			{
				*index = BOARD_TYPE_AX81_1X12G12S;
				return 0;
			}
			else if (board_type == 0x6)
			{
				*index = BOARD_TYPE_AX81_AC4X;
				return 0;
			}
			else if (board_type == 0x3)
			{
				*index = BOARD_TYPE_AX81_12X;
				return 0;
			}
			else
			{
				*index = -1;
				return -1;
			}
			break;
		case XXX_YYY_SINGLE_BOARD:
			if (board_type == 0x1)
			{
				*index = BOARD_TYPE_AX81_AC12C;
				return 0;
			}
			else if (board_type == 0x2)
			{
				*index = BOARD_TYPE_AX81_AC8C;
				return 0;
			}
			else if (board_type == 0x3)
			{
				*index = BOARD_TYPE_AX81_12X;
				return 0;
			}
			else if (board_type == 0x4)
			{
				*index = BOARD_TYPE_AX81_2X12G12S;
				return 0;
			}
			else if (board_type == 0x5)
			{
				*index = BOARD_TYPE_AX81_1X12G12S;
				return 0;
			}
			else
			{
				*index = -1;
				return -1;
			}
			break;
		default:
			*index = -1;
			return -1;
	}
}


int set_local_board(void)
{
	int ret;
	int product_serial;
	int board_type;
	int index;
	
	/*
	ret = get_product_serial(&product_serial);

	if (ret)
	{
		sem_syslog_dbg ("get product serial failed\n");
		return -1;
	}
	*/

	ret = get_board_type(&board_type);

	if (ret)
	{
		sem_syslog_dbg("get board type failed\n");
		return -1;
	}

	ret = get_board_index(product_serial, product->product_type, board_type, &index);

	if (ret)
	{
		sem_syslog_dbg("get board index failed:index = %d product->product_type = %d board_type = %d\n", index, product->product_type, board_type);
		return -1;
	}

	if (index < 0)
	{
		sem_syslog_dbg("not find the correct board type\n");
		return -1;
	}
	local_board = *(product->support_board_arr+index);

	#ifndef __AP_MAX_COUNTER
	sem_syslog_dbg("board->board_ap_counter = %d\n",local_board->board_ap_counter);
	#else
	local_board->board_ap_counter = (__AP_MAX_COUNTER > local_board->board_ap_max_counter) ? local_board->board_ap_max_counter : __AP_MAX_COUNTER;
	sem_syslog_dbg("define __AP_MAX_COUNTER = %d\n",__AP_MAX_COUNTER);
	#endif

	/*the first parameter is useless*/
	ret = product->get_slot_id(local_board->is_master, &local_board->slot_id);

	if (ret)
	{
		local_board->slot_id = UNKONWN_SLOT_ID;
		return -1;
	}

	if (product->product_type == XXX_YYY_AX8603 && 
		(local_board->board_type == BOARD_TYPE_AX81_AC8C || local_board->board_type == BOARD_TYPE_AX81_AC12C || 
		local_board->board_type == BOARD_TYPE_AX81_1X12G12S || local_board->board_type == BOARD_TYPE_AX81_AC4X)) {
		for (ret=0; ret<product->master_slot_count; ret++) {
			if (product->master_slot_id[ret] == local_board->slot_id) {
				local_board->is_master = IS_MASTER;
				local_board->function_type = MASTER_BOARD|AC_BOARD;
				break;
			}
		}	
	}
	if (local_board->board_self_init())
	{
		sem_syslog_dbg("board self init failed\n");
		return -1;
	}
	sem_syslog_dbg("*********************ok***********************************\n");
	sem_syslog_dbg("Local Board : %s\n", local_board->name);
	return 0;
}


static int undistributed_board_dbm_file_create(board_fix_param_t *board)
{
	FILE *fd;
	system("mkdir -p /dbm/local_board/");

	//system("touch /dbm/local_board/name");

	fd = fopen("/dbm/local_board/name", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/name failed\n");
		return -1;
	}
	fprintf(fd, "%s\n", board->name);
	fclose(fd);

	fd = fopen("/dbm/local_board/board_code", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_code failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->board_code);
	fclose(fd);
	
	fd = fopen("/dbm/local_board/slot_id", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/slot_id failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->slot_id);
	fclose(fd);
	
	fd = fopen("/dbm/local_board/is_master", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/is_master failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->is_master);
	fclose(fd);

	fd = fopen("/dbm/local_board/is_active_master", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/is_active_master failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->is_active_master);
	fclose(fd);

	fd = fopen("/dbm/local_board/is_use_default_master", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/is_use_default_master failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->is_use_default_master);
	fclose(fd);


	fd = fopen("/dbm/local_board/port_num_on_panel", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/port_num_on_panel failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->port_num_on_panel);
	fclose(fd);

	fd = fopen("/dbm/local_board/port_num_on_panel", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/port_num_on_panel failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->port_num_on_panel);
	fclose(fd);

	fd = fopen("/dbm/local_board/obc_port_num", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/obc_port_num failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->obc_port_num);
	fclose(fd);

	fd = fopen("/dbm/local_board/cscd_port_num", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/cscd_port_num failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->cscd_port_num);
	fclose(fd);

	fd = fopen("/dbm/local_board/function_type", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/function_type failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->function_type);
	fclose(fd);

	fd = fopen("/dbm/local_board/board_state", "w+");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /dbm/local_board/board_state failed\n");
		return -1;
	}
	fprintf(fd, "%d\n", board->board_state);
	fclose(fd);
	return 0;
}

#define UNDISTRIBUTED_BOARD_CODE	7
int board_init(void)
{
	int ret;

	if (!product_info_syn.is_distributed)
	{
		sem_syslog_dbg("undistributed board init\n");
		local_board = (board_fix_param_t *)malloc(sizeof(board_fix_param_t));
		if (!local_board)
		{
			sem_syslog_warning("malloc for local_board failed\n");
			return -1;
		}
		
		local_board->board_code = UNDISTRIBUTED_BOARD_CODE;
		local_board->board_type = BOARD_TYPE_AX81_MAX;
		local_board->board_state = BOARD_READY;
		local_board->cscd_port_num = -1;
		local_board->function_type = MASTER_BOARD|AC_BOARD|SWITCH_BOARD|BASE_BOARD|NAT_BOARD;
		local_board->is_active_master = 1;
		local_board->is_master = 1;
		local_board->is_use_default_master = 1;
		strcpy(local_board->name, product_info_syn.name);
		local_board->obc_port_num = -1;
		local_board->port_num_on_panel = -1;
		local_board->asic_port_num = -1;
		local_board->asic_start_no = 1;
		local_board->slot_id = 1;
		undistributed_board_dbm_file_create(local_board);
		return 0;
	}

	if (set_local_board())
	{
		sem_syslog_dbg("set local board failed.\n");
		return -1;
	}


	//for OEM name	
	FILE *fd;
	char name[MAX_BOARD_NAME_LEN];
	
	fd = fopen("/devinfo/c5a001", "r");
	if (fd == NULL)
	{
		sem_syslog_dbg("open /devinfo/c5a001 failed\n");
	}
	else
	{
		if(!fscanf(fd, "%s\n", name))
		sem_syslog_dbg("fscanf error\n");
		fclose(fd);

		if ((strncmp(name, "MASTER", 6) == 0) || (strlen(name) == 0))
		{
			sem_syslog_dbg("board use default name\n");
		}
		else
		{
			sem_syslog_dbg("board use OEM name\n");
			if (strlen(name) >= MAX_BOARD_NAME_LEN)
			{
				sem_syslog_warning("OEM name is too long\n");
				strncpy(local_board->name, name, MAX_BOARD_NAME_LEN-1);
			}
			else
			{
				sem_syslog_dbg("name is %s local_board->name is %s strlen(name) = %d\n", name, local_board->name, strlen(name));
				strcpy(local_board->name, name);
			}
		}
	}

	local_board->board_state = BOARD_INITIALIZING;
	local_board->is_active_master = NOT_ACTIVE_MASTER;
	/*init board info to registe to active master board*/
	board_info_to_syn.board_code = local_board->board_code;
	board_info_to_syn.board_type = local_board->board_type;
	board_info_to_syn.function_type = local_board->function_type;
	board_info_to_syn.is_master = local_board->is_master;
	board_info_to_syn.slot_id = local_board->slot_id;
	strcpy(board_info_to_syn.name, local_board->name);
	board_info_to_syn.board_state = local_board->board_state;
	/*this two items will be changed when master_active_or_stby_select done if*/
	board_info_to_syn.is_active_master = local_board->is_active_master;
	board_info_to_syn.asic_start_no = local_board->asic_start_no;
	board_info_to_syn.asic_port_num = local_board->asic_port_num;
    board_info_to_syn.board_ap_counter = local_board->board_ap_counter;
	//board_info_to_syn.is_use_default_master = local_board->is_use_default_master;// TODO: is needed? 

	product->product_slot_board_info[local_board->slot_id].board_code = local_board->board_code;
	product->product_slot_board_info[local_board->slot_id].board_type = local_board->board_type;
	product->product_slot_board_info[local_board->slot_id].function_type = local_board->function_type;
	product->product_slot_board_info[local_board->slot_id].is_master = local_board->is_master;
	strcpy(product->product_slot_board_info[local_board->slot_id].name, local_board->name);
	product->product_slot_board_info[local_board->slot_id].board_state = local_board->board_state;
	product->product_slot_board_info[local_board->slot_id].asic_start_no = local_board->asic_start_no;
	product->product_slot_board_info[local_board->slot_id].asic_port_num = local_board->asic_port_num;
	product->product_slot_board_info[local_board->slot_id].board_ap_counter = local_board->board_ap_counter;
	local_board->local_board_dbm_file_create(local_board);

	product_info_syn.product_slot_board_info[local_board->slot_id].board_code = product->product_slot_board_info[local_board->slot_id].board_code;
	product_info_syn.product_slot_board_info[local_board->slot_id].board_type = product->product_slot_board_info[local_board->slot_id].board_type;
	product_info_syn.product_slot_board_info[local_board->slot_id].function_type = product->product_slot_board_info[local_board->slot_id].function_type;
	product_info_syn.product_slot_board_info[local_board->slot_id].is_master = product->product_slot_board_info[local_board->slot_id].is_master;
	strcpy(product_info_syn.product_slot_board_info[local_board->slot_id].name, product->product_slot_board_info[local_board->slot_id].name);
	product_info_syn.product_slot_board_info[local_board->slot_id].board_state = product->product_slot_board_info[local_board->slot_id].board_state;
    product_info_syn.product_slot_board_info[local_board->slot_id].asic_start_no = product->product_slot_board_info[local_board->slot_id].asic_start_no;
    product_info_syn.product_slot_board_info[local_board->slot_id].asic_start_no = product->product_slot_board_info[local_board->slot_id].asic_port_num;
    product_info_syn.product_slot_board_info[local_board->slot_id].board_ap_counter = product->product_slot_board_info[local_board->slot_id].board_ap_counter;
	if (local_board->is_master)
	{
		//sem_syslog_dbg("111111111111111\n");
		product->get_board_on_mask(&product->board_on_mask, local_board->slot_id, product->default_master_slot_id);
		product->update_product_state();
		//sem_syslog_dbg("mask=0x%x\n", product->board_on_mask);
	}
	product_info_syn.board_on_mask = product->board_on_mask;
	product_info_syn.product_state = product->product_state;
	
	local_board->init_local_mac_info();
	local_board->init_local_sn_info();
	product->product_dbm_file_create(&product_info_syn);
	
	return 0;
}


#ifdef __cplusplus
}
#endif
