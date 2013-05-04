#ifndef __NPD_BOARD_H__
#define __NPD_BOARD_H__

/* Global H file for asic board */
#include <sysdef/npd_sysdef.h>   /* for board_info */
#include <cvm/autelan_product.h>   /* for product_info */

#include "util/npd_list.h"      /* for "list" in npd_amapi.h  */
#include "npd/nbm/npd_bmapi.h"  /* need by npd_product.h & */
#include "npd/nam/npd_amapi.h"  /* for product_s & product_sysinfo_s */
#include "npd_product.h"    /* for FUNCTION_ACL & npd_init_productinfo() */

#include "npd_c_slot.h"   /* for module_type_data() */
#include "npd_eth_port.h"   /* for module_type_data() */

#define RET_SUCCESS 0
#define RET_FAIL   -1
#define ENABLE  1
#define DISABLE 0
#define DATA_INVALID -1

/* define for per board */
#define DISTRIBUTED_SLOT_MAX 16     /* change to 16 ,zhangdi@autelan.com 2012-03-26 */
#define DISTRIBUTED_SLOT_START_NO 1  /* start form 1-1 */
#define DISTRIBUTED_PORT_START_NO 1  /* start from 1-1,not 1-0 */

#define EXTERNAL_MTU    1538
#define INTERNAL_MTU    1548
#define ASIC_CAPBMP ((1<<FUNCTION_ACL)|   \
	               (1<<FUNCTION_QOS)|   \
	               (1<<FUNCTION_TUNNEL)|   \
	               (1<<FUNCTION_PVLAN)|   \
	               (1<<FUNCTION_IGMP)|   \
	               (1<<FUNCTION_MLD)|   \
	               (1<<FUNCTION_DLDP)|   \
	               (1<<FUNCTION_STP)|   \
	               (1<<FUNCTION_VLAN)|   \
	               (1<<FUNCTION_INTERFACE)|   \
	               (1<<FUNCTION_MIRROR)|   \
	               (1<<FUNCTION_FDB)|   \
	               (1<<FUNCTION_TRUNK)|   \
	               (1<<FUNCTION_ARP)|   \
	               (1<<FUNCTION_ESLOT)|   \
	               (1<<FUNCTION_ASIC)|   \
	               (1<<FUNCTION_OAM)|   \
	               (1<<FUNCTION_ROUTE))


#define ASIC_RUNBMP ASIC_CAPBMP

typedef enum {
	ASIC_TYPE_CPSS,     /* XCAT & CHEECH2 */
	ASIC_TYPE_BCM
};

/* function */
void npd_init_boardinfo(void);
int npd_init_productinfo_distributed(void);
int npd_asic_func_init(int);
int get_num_from_file(const char *filename);
int get_board_mac_from_sn(char *board_mac);
int npd_disable_cscd_port(void);
int npd_enable_cscd_port(void);
int npd_set_cscd_port_preamble_tx(unsigned char length);
unsigned int equipment_test(void);
int npd_set_dev_map_table(void);
int npd_set_cscd_port_ipg(unsigned char ipg_type);
int npd_cscd_port_config(void);

#endif
