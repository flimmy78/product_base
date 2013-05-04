/* eag_stamsg.h */

#ifndef _EAG_STAMSG_H
#define _EAG_STAMSG_H

#include <stdint.h>
#include "eag_def.h"
#include "wcpss/waw.h"
#include "session.h"
#include "eag_conf.h"
#include "appconn.h"
#include "eag_captive.h"

#define STAMSG_SOCK_PATH_FMT   	"/var/run/wcpss/portal_sta_us%d_%d"
#define STAMSG_ASD_SOCK_PATH_FMT   	"/var/run/wcpss/asd_table%d_%d"

eag_stamsg_t *
eag_stamsg_new(uint8_t hansi_type, 
		uint8_t hansi_id);

int
eag_stamsg_free(eag_stamsg_t *stamsg);

int
eag_stamsg_start(eag_stamsg_t *stamsg);

int
eag_stamsg_stop(eag_stamsg_t *stamsg);

int
eag_stamsg_send(eag_stamsg_t *stamsg,
		struct appsession *session, Operate Op);

int
eag_stamsg_set_thread_master(eag_stamsg_t *stamsg,
		eag_thread_master_t *master);

int
eag_stamsg_set_eagins(eag_stamsg_t *stamsg,
		eag_ins_t *eagins);

int
eag_stamsg_set_portal(eag_stamsg_t *stamsg,
		eag_portal_t *portal);

int
eag_stamsg_set_eagdbus(eag_stamsg_t *stamsg,
		eag_dbus_t *eagdbus);

int
eag_stamsg_set_appdb(eag_stamsg_t *stamsg,
		appconn_db_t *appdb);

int
eag_stamsg_set_captive(eag_stamsg_t *stamsg,
		eag_captive_t *captive);

int
eag_stamsg_set_macauth(eag_stamsg_t *stamsg,
		eag_macauth_t *macauth);

int
eag_stamsg_set_portal_conf(eag_stamsg_t *stamsg,
		struct portal_conf *portalconf);

int
eag_stamsg_set_nasid_conf(eag_stamsg_t *stamsg,
		struct nasid_conf *nasidconf);

int
eag_stamsg_set_nasportid_conf(eag_stamsg_t *stamsg,
		struct nasportid_conf *nasportidconf);

int
eag_stamsg_set_eagstat(eag_stamsg_t *stamsg,
		eag_statistics_t *eagstat);

int
eag_stamsg_set_eaghansi(eag_stamsg_t *stamsg,
		eag_hansi_t *eaghansi);


#endif        /* _EAG_STAMSG_H */
