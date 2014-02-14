#ifndef SEM_INTERFACE_H
#define SEM_INTERFACE_H

extern int
sem_dbus_show_slot_id(DBusConnection *dcli_dbus_connection, int *slot_id);

extern int 
sem_dbus_show_one_slot_info(DBusConnection *dcli_dbus_connection,
										int* slot_id,
										int* board_code,
							   			unsigned int* function_type,
										int* master,
										int* active_master,
										char** name);


extern int
sem_dbus_show_all_slot_info(DBusConnection* dcli_dbus_connection, 
					  sem_interface_board_info_t* board_info,
										    int *slot_count,
	 					 		   unsigned int *board_on_mask);

extern int
sem_dbus_get_product_info(DBusConnection* dcli_dbus_connection, 
					  sem_interface_product_info_t* product_info);

extern int 
sem_dbus_send_file(DBusConnection* dcli_dbus_connection);



#endif
