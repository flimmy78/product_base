#ifndef __NPD_DBUS_H__
#define __NPD_DBUS_H__

int npd_dbus_init(void);
void * npd_dbus_thread_main(void *arg);
void *npd_dbus_restart_thread();
int npd_dbus_reinit(void);

#endif
