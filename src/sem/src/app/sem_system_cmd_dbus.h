#ifndef SEM_SYSTEM_CMD_DBUS_H
#define SEM_SYSTEM_CMD_DBUS_H

#define RT_TABLE_DATABASE_FILE "/etc/iproute2/rt_tables"
#define RTFIELD_DATABASE_FILE  "/etc/iproute2/rt_dsfield"

#define IS_RTB_ID(_rtb_id) (_rtb_id>=1 && _rtb_id<=32)?1:0
#define RTB_NAME(_buf,_rtb_id) sprintf(_buf,"_rp%u",_rtb_id)

#define CMD_STR_LEN 256

#define rt_tables_entry_name(_rtb_id )  dcli_rp_table[_rtb_id]

struct rp_rule_node{

struct rp_rule_node *next;
struct rp_rule_node *prev;
unsigned char type;
char tablename[64];
char ifname[32];
char prefix[32];
int tos;
int sn;
};
struct rp_ip_route_node{

struct rp_ip_route_node *next;
struct rp_ip_route_node *prev;
unsigned char type;
char tablename[64];
char dst[32];
char prefix[32];
};


#endif

