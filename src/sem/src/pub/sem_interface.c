#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <dbus/dbus.h>
#include <dbus/sem/sem_dbus_def.h>
#include <sys/wait.h>
#include "dcli_main.h"
#include "../dcli/dcli_sem.h"

int 
sem_dbus_show_slot_id(DBusConnection *dcli_dbus_connection, int *slot_id)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;	

	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME, 
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE, 
										 SEM_DBUS_SHOW_SLOT_ID);

	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);
	
	dbus_message_unref(query);
	
	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		}
		return SEM_DBUS_ERROR;		
	}else{
		dbus_message_iter_init(reply,&iter);
		dbus_message_iter_get_basic(&iter, slot_id);
	}
	
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;
}



int 
sem_dbus_show_one_slot_info(DBusConnection *dcli_dbus_connection,
											int* slot_id,
											int* board_code,
											unsigned int* function_type,
										    int* master,
										    int* active_master,
										    char** name)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME, 
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE, 
										 SME_DBUS_SHOW_SLOT_N_INFO);	

    dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_INT32,slot_id,
							 DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);

	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		}
		return SEM_DBUS_ERROR;		
		
	}else{
	
    	dbus_message_iter_init(reply,&iter);
    
    	dbus_message_iter_get_basic(&iter, board_code);
    	dbus_message_iter_next(&iter);
    
    	dbus_message_iter_get_basic(&iter, function_type);
    	dbus_message_iter_next(&iter);
    	
    	dbus_message_iter_get_basic(&iter, master);
    	dbus_message_iter_next(&iter);
    
    	dbus_message_iter_get_basic(&iter, active_master);
    	dbus_message_iter_next(&iter);
    	
    	dbus_message_iter_get_basic(&iter, name);
	}
	
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;
	
}



int
sem_dbus_show_all_slot_info(DBusConnection* dcli_dbus_connection, 
					  sem_interface_board_info_t* board_info,
										    int *slot_count,
	 					 		   unsigned int *board_on_mask)
{ 
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	int i;

	int board_code;
	char *name;
	int is_master;
	int is_active_master;
	unsigned int function_type;
	int board_state;

	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME, 
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE,
										 SEM_DBUS_SHOW_SLOT_INFO);
	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);
	
	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		}
		return SEM_DBUS_ERROR;		
		
    }else{
    
    	dbus_message_iter_init(reply,&iter);
    	
    	dbus_message_iter_get_basic(&iter, slot_count);	
    	dbus_message_iter_next(&iter);
    	dbus_message_iter_get_basic(&iter, board_on_mask);	
    	dbus_message_iter_next(&iter);
    	
    	for (i=0; i<*slot_count; i++)
    	{	
    		dbus_message_iter_get_basic(&iter, &board_state);	
    		dbus_message_iter_next(&iter);
    		dbus_message_iter_get_basic(&iter, &board_code);
    		dbus_message_iter_next(&iter);
    		dbus_message_iter_get_basic(&iter, &function_type);
    		dbus_message_iter_next(&iter);
    		
    		dbus_message_iter_get_basic(&iter, &is_master);
    		dbus_message_iter_next(&iter);
    		dbus_message_iter_get_basic(&iter, &is_active_master);
    		dbus_message_iter_next(&iter);

    		dbus_message_iter_get_basic(&iter, &name);

			board_info[i].board_state = board_state;
			board_info[i].board_code = board_code;
			board_info[i].function_type = function_type;
			board_info[i].is_master = is_master;
			board_info[i].is_active_master= is_active_master;
			board_info[i].name = name;

    		if (i < *slot_count-1)
    		{
    			dbus_message_iter_next(&iter);
    		}
    		
    	}
   
    }


	dbus_message_unref(reply);
	return SEM_DBUS_SUCCESS;
}


int
sem_dbus_get_product_info(DBusConnection* dcli_dbus_connection, 
					  sem_interface_product_info_t* product_info)
{ 
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;
	int i;
	int product_type, slotcount, master_slot_count, default_master_slot_id;
	int more_than_one_master_board_on, is_distributed;
	int fan_num, master_slot_id[SEM_DBUS_MAX_MASTER_SLOT_NUM];
	unsigned int board_on_mask;
	char *name;	

	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME, 
										 SEM_DBUS_OBJPATH,
									     SEM_DBUS_INTERFACE,
									     SEM_DBUS_SHOW_PRODUCT_INFO);
	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);
	
	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		}
		return SEM_DBUS_ERROR;		
		
    }else{
    
    	dbus_message_iter_init(reply,&iter);
    
    	dbus_message_iter_get_basic(&iter, &board_on_mask);
    	dbus_message_iter_next(&iter);
    	
    	dbus_message_iter_get_basic(&iter, &default_master_slot_id);
    	dbus_message_iter_next(&iter);
    
    	dbus_message_iter_get_basic(&iter, &fan_num);
    	dbus_message_iter_next(&iter);
    
    	dbus_message_iter_get_basic(&iter, &is_distributed);
    	dbus_message_iter_next(&iter);
    
    	dbus_message_iter_get_basic(&iter, &master_slot_count);
    	dbus_message_iter_next(&iter);
    
    	for (i=0; i<SEM_DBUS_MAX_MASTER_SLOT_NUM; i++)
    	{
    		dbus_message_iter_get_basic(&iter, &master_slot_id[i]);
    		dbus_message_iter_next(&iter);
    	}
    	
    	dbus_message_iter_get_basic(&iter, &more_than_one_master_board_on);
    	dbus_message_iter_next(&iter);
    
    	dbus_message_iter_get_basic(&iter, &name);
    	dbus_message_iter_next(&iter);
    
    	dbus_message_iter_get_basic(&iter, &product_type);
    	dbus_message_iter_next(&iter);
    
    	dbus_message_iter_get_basic(&iter, &slotcount);
    	dbus_message_iter_next(&iter);

		product_info->board_on_mask = board_on_mask;
		product_info->default_master_slot_id = default_master_slot_id;
		product_info->fan_num = fan_num;
		product_info->is_distributed = is_distributed;
		product_info->master_slot_count = master_slot_count;

    	for (i=0; i<SEM_DBUS_MAX_MASTER_SLOT_NUM; i++)
    	{
    		product_info->master_slot_id[i] =  master_slot_id[i];
    	}
		product_info->more_than_one_master_board_on = more_than_one_master_board_on;
		strcpy(product_info->name, name);
		product_info->product_type = product_type;
		product_info->slotcount = slotcount;
			 
    }

	dbus_message_unref(reply);
	return SEM_DBUS_SUCCESS;
}



int
sem_dbus_config_48GE(DBusConnection* dcli_dbus_connection, char** name)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME, 
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE, 
										 SEM_DBUS_CONF_48GE);
    dbus_error_init(&err);


	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);

	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		}
		return SEM_DBUS_ERROR;		
		
	}else{
	
		dbus_message_iter_init(reply,&iter);
		dbus_message_iter_get_basic(&iter,name);
	}
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;
	
}


int
sem_dbus_get_board_info(DBusConnection* dcli_dbus_connection, sem_interface_board_info_t *board_info)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	//int board_code;
	//char *name;
	//int slot_id;
	//int is_master, is_active_master;
	//unsigned int port_num_on_panel, function_type, obc_port_num, cscd_port_num;

	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME, 
										 SEM_DBUS_OBJPATH,
									     SEM_DBUS_INTERFACE,
									     SEM_DBUS_SHOW_BOARD_INFO);
    dbus_error_init(&err);


	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);

	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		}
		return SEM_DBUS_ERROR;		
		
	}else{
	
     	dbus_message_iter_init(reply,&iter);
     
     	dbus_message_iter_get_basic(&iter, &board_info->board_code);
     	dbus_message_iter_next(&iter);
     
     	dbus_message_iter_get_basic(&iter, &board_info->cscd_port_num);
     	dbus_message_iter_next(&iter);
     	
     	dbus_message_iter_get_basic(&iter, &board_info->function_type);
     	dbus_message_iter_next(&iter);
     	
     	dbus_message_iter_get_basic(&iter, &board_info->is_active_master);
     	dbus_message_iter_next(&iter);
     
     	dbus_message_iter_get_basic(&iter, &board_info->is_master);
     	dbus_message_iter_next(&iter);
     
     	dbus_message_iter_get_basic(&iter, &board_info->name);
     	dbus_message_iter_next(&iter);
     
     	dbus_message_iter_get_basic(&iter, &board_info->obc_port_num);
     	dbus_message_iter_next(&iter);
     
     	dbus_message_iter_get_basic(&iter, &board_info->port_num_on_panel);
     	dbus_message_iter_next(&iter);
     
     	dbus_message_iter_get_basic(&iter, &board_info->slot_id);
	}
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;
	
}


int
sem_dbus_config_tipc(DBusConnection* dcli_dbus_connection, int slot_id, int num, int usleep_tm)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	
	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME, 
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE,
										 SEM_DBUS_CONF_TIPC);
    dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_INT32,&slot_id,
							 DBUS_TYPE_INT32,&num,
							 DBUS_TYPE_INT32,&usleep_tm,
							 DBUS_TYPE_INVALID);	


	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);

	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		}
		return SEM_DBUS_ERROR;		
		
	}
	
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;

	
}

int
sem_dbus_send_file(DBusConnection* dcli_dbus_connection)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE,
										 SEM_DBUS_SEND_FILE);
	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);
	
	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		}
		return SEM_DBUS_ERROR;		
		
	}
		
	dbus_message_unref(reply);

	return SEM_DBUS_SUCCESS;

}


int
sem_dbus_interface_mcb_active_standby_switch(DBusConnection*dcli_dbus_connection, int* ret_value)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;
	
	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE,
										 SEM_DBUS_MCB_ACTIVE_STANDBY_SWITCH);
    dbus_error_init(&err);


	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, 60000, &err);


	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		}
		return SEM_DBUS_ERROR;		
		
	}else {
		dbus_message_get_args (reply, &err,
					DBUS_TYPE_UINT32,ret_value,
					DBUS_TYPE_INVALID);
	}
	
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;
}


int 
sem_dbus_interface_disable_keep_alive(DBusConnection*dcli_dbus_connection, int time, int* ret_value)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;
	
	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
								         SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE, 
										 SEM_DBUS_DISABLE_KEEP_ALIVE_TEMPORARILY);

    dbus_error_init(&err);
	
	dbus_message_append_args(query,
							DBUS_TYPE_UINT32, &time,
							DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);


	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		}
		return SEM_DBUS_ERROR;		
		
	}else {
		dbus_message_get_args (reply, &err,
					DBUS_TYPE_UINT32,ret_value,
					DBUS_TYPE_INVALID);
	}
	
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;
}

int
sem_dbus_debug_sem(DBusConnection *dcli_dbus_connection, int flag, int *re_value, int *ret)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
									     SEM_DBUS_OBJPATH,
									     SEM_DBUS_INTERFACE,
									     SEM_DBUS_INTERFACE_METHOD_SYSTEM_DEBUG_STATE);
    dbus_error_init(&err);

	dbus_message_append_args(query,
							DBUS_TYPE_UINT32,&flag,
							DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);

	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		} 
		return SEM_DBUS_ERROR;		
		
	} else {
        dbus_message_get_args (reply, &err,
        									DBUS_TYPE_UINT32,re_value,
        									DBUS_TYPE_UINT32,ret,
        									DBUS_TYPE_INVALID);	
		
	}
	
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;

	
}


int
sem_dbus_no_debug_sem(DBusConnection *dcli_dbus_connection, int flag, int *re_value, int *ret)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE,
										 SEM_DBUS_INTERFACE_METHOD_SYSTEM_UNDEBUG_STATE);

    dbus_error_init(&err);

	dbus_message_append_args(query,
							DBUS_TYPE_UINT32,&flag,
							DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);

	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		} 
		return SEM_DBUS_ERROR;		
		
	} else {
        dbus_message_get_args (reply, &err,
        									DBUS_TYPE_UINT32,re_value,
        									DBUS_TYPE_UINT32,ret,
        									DBUS_TYPE_INVALID);	
		
	}
	
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;
}
	

int
sem_dbus_set_88E6185(DBusConnection *dcli_dbus_connection, unsigned int dev_addr, 
					 unsigned int reg_addr, unsigned short *val, unsigned int phy_addr, int *ret_value)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE,
										 SEM_DBUS_SET_6185);

    dbus_error_init(&err);

	dbus_message_append_args(query,
				 		DBUS_TYPE_UINT32, &dev_addr,
				 		DBUS_TYPE_UINT32, &reg_addr,
				 		DBUS_TYPE_UINT16, val,
				 		DBUS_TYPE_UINT32, &phy_addr,
				 		DBUS_TYPE_INVALID);
	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);

	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		} 
		return SEM_DBUS_ERROR;		
		
	} else {
		dbus_message_iter_init(reply,&iter);
		dbus_message_iter_get_basic(&iter,ret_value);	
	}
	
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;
}



int
sem_dbus_show_88E6185(DBusConnection *dcli_dbus_connection, unsigned int dev_addr, 
					 unsigned int reg_addr, unsigned short *val, unsigned int phy_addr)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE,
										 SEM_DBUS_SHOW_6185);

    dbus_error_init(&err);

	dbus_message_append_args(query,
				 		DBUS_TYPE_UINT32, &dev_addr,
				 		DBUS_TYPE_UINT32, &reg_addr,
				 		DBUS_TYPE_UINT32, &phy_addr,
				 		DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block(dcli_dbus_connection, query, -1, &err);

	dbus_message_unref(query);

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		} 
		return SEM_DBUS_ERROR;		
		
	} else {
		dbus_message_iter_init(reply,&iter);
		dbus_message_iter_get_basic(&iter,val);	
	}
	
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;
}



int
sem_dbus_interface_delete_patch(DBusConnection *dcli_dbus_connection, char *patchname, int *ret_value)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	if (dcli_dbus_connection)
	{	
       	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
    										 SEM_DBUS_OBJPATH,
    	    								 SEM_DBUS_INTERFACE,
    	    								 SEM_DBUS_DELETE_PATCH);
        dbus_error_init(&err);
    
    	dbus_message_append_args(query,
    					 		DBUS_TYPE_STRING,&patchname,
    					 		DBUS_TYPE_INVALID);

		reply = dbus_connection_send_with_reply_and_block (dcli_dbus_connection, query, -1, &err);    
		
    	dbus_message_unref(query); 
    
    	if ( NULL == reply ){	
    		if (dbus_error_is_set(&err)){
    			dbus_error_free(&err);
    		} 
    		return SEM_DBUS_ERROR;		
    		
    	} else {
    		dbus_message_get_args ( reply, &err,
    	                     DBUS_TYPE_INT32,ret_value,
    	                     DBUS_TYPE_INVALID);
    	}
    	
    	dbus_message_unref(reply);
	}
	else {
		return SEM_DBUS_NO_CONN;
	}
	
	return SEM_DBUS_SUCCESS;
}





int
sem_dbus_flash_write_boot(DBusConnection *dcli_dbus_connection, char *patchname, int *ret_value)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	if (dcli_dbus_connection)
	{	
		query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
											 SEM_DBUS_OBJPATH,
											 SEM_DBUS_INTERFACE,
											 WRITE_BOOT_TO_FLASH);
        dbus_error_init(&err);
    
    	dbus_message_append_args(query,
    					 		DBUS_TYPE_STRING,&patchname,
    					 		DBUS_TYPE_INVALID);

		reply = dbus_connection_send_with_reply_and_block (dcli_dbus_connection, query, -1, &err);    
		
    	dbus_message_unref(query); 
    
    	if ( NULL == reply ){	
    		if (dbus_error_is_set(&err)){
    			dbus_error_free(&err);
    		} 
    		return SEM_DBUS_ERROR;		
    		
    	} else {
    		dbus_message_get_args ( reply, &err,
    	                     DBUS_TYPE_INT32,ret_value,
    	                     DBUS_TYPE_INVALID);
    	}

		printf("ret_value == %d\n",*ret_value);
    	dbus_message_unref(reply);
	}
	else {
		return SEM_DBUS_NO_CONN;
	}
	
	return SEM_DBUS_SUCCESS;
}


int
sem_dbus_interface_apply_patch(DBusConnection *dcli_dbus_connection, char* patchname, int* ret_value, char** patch_result)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

    //char *patch_result;
	if (dcli_dbus_connection)
	{	
    	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
											 SEM_DBUS_OBJPATH,
    										 SEM_DBUS_INTERFACE,
    										 SEM_DBUS_APPLY_PATCH);

        dbus_error_init(&err);
    
    	dbus_message_append_args(query,
    					 		DBUS_TYPE_STRING, &patchname,
    					 		DBUS_TYPE_INVALID);

		reply = dbus_connection_send_with_reply_and_block (dcli_dbus_connection, query, -1, &err);    
		
    	dbus_message_unref(query); 
    
    	if ( NULL == reply ){	
    		if (dbus_error_is_set(&err)){
    			dbus_error_free(&err);
    		} 
    		return SEM_DBUS_ERROR;		
    		
    	} else {
        	dbus_message_get_args ( reply, &err,
        		                     DBUS_TYPE_INT32,ret_value,
        		                     DBUS_TYPE_STRING,patch_result,
        		                     DBUS_TYPE_INVALID);
    	}
    	dbus_message_unref(reply);
	}
	else {
		return SEM_DBUS_NO_CONN;
	}
	
	return SEM_DBUS_SUCCESS;

}


int 
sem_dbus_config_sem_sendto_trap(DBusConnection *dcli_dbus_connection, int num, int* ret_value)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	
	query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE,
										 SEM_DBUS_CONFIG_SEM_TO_TRAP);
    dbus_error_init(&err);

	dbus_message_append_args(query,
		                DBUS_TYPE_UINT32, &num,
						DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block (dcli_dbus_connection, query, -1, &err);    
	
	dbus_message_unref(query); 

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		} 
		return SEM_DBUS_ERROR;		
		
	} else {
		dbus_message_get_args (reply, &err,
					DBUS_TYPE_UINT32,ret_value,
					DBUS_TYPE_INVALID);
	}

	dbus_message_unref(reply);

	
	return SEM_DBUS_SUCCESS;
}

int
sem_dbus_set_if_wan_state(DBusConnection* dcli_dbus_connection, int	wan_state, char* dev_name, int* ret_value)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	
    query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
										 SEM_DBUS_OBJPATH,
       									 SEM_DBUS_INTERFACE,
       									 SEM_DBUS_SET_IF_WAN_STATE);
    dbus_error_init(&err);

	dbus_message_append_args(query,
						DBUS_TYPE_UINT32, &wan_state,
						DBUS_TYPE_STRING, &dev_name,
						DBUS_TYPE_INVALID);

    reply = dbus_connection_send_with_reply_and_block (dcli_dbus_connection, query, 60000, &err);	
	
	dbus_message_unref(query); 

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		} 
		return SEM_DBUS_ERROR;		
		
	} else {
		dbus_message_get_args (reply, &err,
					DBUS_TYPE_UINT32,ret_value,
					DBUS_TYPE_INVALID);
	}

	dbus_message_unref(reply);

	
	return SEM_DBUS_SUCCESS;
}


int
sem_dbus_copy_files(DBusConnection* dcli_dbus_connection, int slot_id, char* src_file_path, \
								char* dst_file_path, int tar_switch, int op, int *ret_value, char** resultMd5)
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	
    query = dbus_message_new_method_call(BSD_DBUS_BUSNAME,
										 BSD_DBUS_OBJPATH,
										 BSD_DBUS_INTERFACE,
										 BSD_COPY_FILES_BETEWEEN_BORADS);
    dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_UINT32,&slot_id,
							 DBUS_TYPE_STRING,&src_file_path,
							 DBUS_TYPE_STRING,&dst_file_path,
							 DBUS_TYPE_UINT32,&tar_switch,
							 DBUS_TYPE_UINT32,&op,
							 DBUS_TYPE_INVALID);

    reply = dbus_connection_send_with_reply_and_block (dcli_dbus_connection, query, 120000, &err);	
	
	dbus_message_unref(query); 

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		} 
		return SEM_DBUS_ERROR;		
		
	} else {
    	dbus_message_get_args ( reply, &err,
    					DBUS_TYPE_UINT32,ret_value,
    					DBUS_TYPE_STRING,resultMd5,
    					DBUS_TYPE_INVALID);
	}
	dbus_message_unref(reply);
	
	return SEM_DBUS_SUCCESS;
}


int 
sem_dbus_interface_set_boot_img(DBusConnection* dcli_dbus_connection,\
										int slot_id, char* version_file, int* ret_value)
												
{
	DBusMessage *query = NULL, *reply = NULL;
	DBusMessageIter iter;
	DBusError err;

	
    query = dbus_sem_msg_new_method_call(SEM_DBUS_BUSNAME,
										 SEM_DBUS_OBJPATH,
										 SEM_DBUS_INTERFACE,
										 SEM_DBUS_SET_BOOT_IMG);
    dbus_error_init(&err);

	dbus_message_append_args(query,
							 DBUS_TYPE_UINT32,&slot_id,
							 DBUS_TYPE_STRING,&version_file,
							 DBUS_TYPE_INVALID);


    reply = dbus_connection_send_with_reply_and_block (dcli_dbus_connection, query, 60000, &err);	
	
	dbus_message_unref(query); 

	if ( NULL == reply ){	
		if (dbus_error_is_set(&err)){
			dbus_error_free(&err);
		} 
		return SEM_DBUS_ERROR;		
		
	} else {
        dbus_message_get_args ( reply, &err,
        						DBUS_TYPE_UINT32,ret_value,
        						DBUS_TYPE_INVALID);
	}

	dbus_message_unref(reply);

	
	return SEM_DBUS_SUCCESS;
}
