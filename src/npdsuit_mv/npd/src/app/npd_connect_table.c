/*******************************************************************************
Copyright (C) Autelan Technology

This software file is owned and distributed by Autelan Technology 
********************************************************************************

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************
* npd_vlan.c
*
*
* CREATOR:
*		wangchong@autelan.com
*
* DESCRIPTION:
*		API used in NPD for CONNECT TABLE.
*
* DATE:
*		05/21/2013	
*
*  FILE REVISION NUMBER:
*  		$Revision: 0.01 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

#include <unistd.h>  /*getpagesize(  ) */  
#include <sys/ipc.h>   
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>

#include <fcntl.h>
#include <sys/mman.h>  /* for mmap() */

#define MY_SHM_VLANLIST_ID 54321  

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_log.h"
#include "npd_product.h"
#include "npd_eth_port.h"
#include "npd_c_slot.h"
#include "npd_connect_table.h"		/* sw data structure in npd*/
#include "npd_intf.h"
#include "npd_trunk.h"
#include "npd_igmp_snp_com.h"
#include "npd_rstp_common.h"
#include "npd_pvlan.h"
#include "npd_fdb.h"
#include "npd_dhcp_snp_com.h"
#include "npd_dhcp_snp_options.h"
#include "sysdef/returncode.h"
#include "npd_mld_snp_com.h"
#include "cvm/autelan_product.h"

#include "board/netlink.h"   /* for netlink msg */
#include "npd_connect_12c_config.h"
#include "npd_connect_12x_config.h"
#include "npd_connect_1x12g12s_config.h"
#include "npd_connect_2x12g12s_config.h"
#include "npd_connect_4x_config.h"
#include "npd_connect_8c_config.h"


extern BoardInfo board_info;



void npd_init_distributed_connect_table(void)
{
	int fd = -1;
	int length = 0;
	char* file_path = "/dbm/shm/connect_table/shm_conntable";
	
    syslog_ax_conntable_dbg("\n============== init distributed connect_table: ===============\n");
    /* creat /dbm/shm/connect_table*/
	system("mkdir -p /dbm/shm/connect_table/");	
    syslog_ax_conntable_dbg("file_path = %s\n", file_path);
	fd = open(file_path, O_CREAT | O_RDWR, 00755);
	
	if(fd < 0)
    {
        syslog_ax_conntable_err("Failed to open! %s\n", strerror(errno));
        return -1;
    }

	length = lseek(fd, sizeof(struct asic_board_cscd_bport_s)*14, SEEK_SET);
    write(fd,"",1);

    g_bportlist = (asic_board_cscd_bport_t *)mmap(NULL, sizeof(struct asic_board_cscd_bport_s)*14, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

    syslog_ax_conntable_dbg("mmap addr for shm_vlan: g_vlanlist = 0x%x\n", g_bportlist);

    if (MAP_FAILED == g_bportlist)
    {
        syslog_ax_conntable_dbg("Failed to mmap for g_vlanlist[]! %s\n", strerror(errno));
		close(fd);	/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
        return -1;
    }
	memset(g_bportlist, 0, sizeof(struct asic_board_cscd_bport_s)*14);			

	close(fd);	

}

void npd_init_connect_table(void) 
{
    /* if this is distributed system, goto npd_init_distributed_vlan() */
    if((SYSTEM_TYPE == IS_DISTRIBUTED)||(PRODUCT_ID == PRODUCT_ID_AX7K_I))
	{
		syslog_ax_conntable_dbg("npd_init_connect_table() is NULL for distributed system !\n");		
		return;
		/*do nothing*/
	}
	else
	{	
        /*do nothing*/
	}
}



int npd_get_default_connect_table(void)
{
	char slot_id = 0;
	char buf[32];
    int board_type,i,ret;
	int slotNum = get_num_from_file(SEM_SLOT_COUNT_PATH);

	if(PRODUCT_TYPE==AUTELAN_PRODUCT_AX8610)
	{
        for(slot_id=0;slot_id<8;slot_id++)
        {
			sprintf(buf, "/dbm/product/slot/slot%d/board_code", ((slot_id<AX8610_SLOT_HALF?slot_id:slot_id+2)+1));
			board_type =get_num_from_file(buf);
            switch(board_type)
            {

                case   	AX81_AC8C_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_AC8C;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8610_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 2;
					g_bportlist[slot_id].asic_to_cpu_ports = 1;
					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_8c_8610_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_8c_8610_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_8c_8610_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_8c_8610_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_8c_8610_cscd_bports[slot_id][i].trunk_id;
					}
					break;                 
					case   	AX81_AC12C_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_AC12C;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8610_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 2;
					g_bportlist[slot_id].asic_to_cpu_ports = 2;
					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_12c_8610_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_12c_8610_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_12c_8610_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_12c_8610_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_12c_8610_cscd_bports[slot_id][i].trunk_id;
					}
					break;                
				case   	AX81_2X12G12S_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_AC12C;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8610_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 2;
					g_bportlist[slot_id].asic_to_cpu_ports = 0;
					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_2x12g12s_8610_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_2x12g12s_8610_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_2x12g12s_8610_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_2x12g12s_8610_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_2x12g12s_8610_cscd_bports[slot_id][i].trunk_id;
					}
					break;
				case   	AX81_1X12G12S_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_1X12G12S;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8610_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 2;
					g_bportlist[slot_id].asic_to_cpu_ports = 2;
					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_1x12g12s_8610_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_1x12g12s_8610_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_1x12g12s_8610_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_1x12g12s_8610_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_1x12g12s_8610_cscd_bports[slot_id][i].trunk_id;
					}
					break;                 
                case   	AX81_12X_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_AC_12X;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8610_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 12;
					g_bportlist[slot_id].asic_to_cpu_ports = 0;
					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_12x_8610_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_12x_8610_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_12x_8610_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_12x_8610_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_12x_8610_cscd_bports[slot_id][i].trunk_id;

					}
					break;
                case   	AX81_AC4X_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_AC_4X;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8610_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 10;
					g_bportlist[slot_id].asic_to_cpu_ports = 4;

					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_4x_8610_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_4x_8610_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_4x_8610_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_4x_8610_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_4x_8610_cscd_bports[slot_id][i].trunk_id;			
					}
	                break;	
					
                case   	AX81_CRSMU_BOARD_CODE:
                case	AX81_CRSMUE_BOARD_CODE:					
				default:
						g_bportlist[slot_id].board_type = -1;
					break;
			}
				
		}
	}


	
/*	if(PRODUCT_TYPE==AUTELAN_PRODUCT_AX8606)
*/
    if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8800)
    {
        for(slot_id=0;slot_id<8;slot_id++)
        {
			sprintf(buf, "/dbm/product/slot/slot%d/board_code", ((slot_id<AX8800_SLOT_HALF?slot_id:slot_id+2)+1));
			board_type =get_num_from_file(buf);
            switch(board_type)
            {

                case   	AX81_AC8C_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_AC8C;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8800_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 2;
					g_bportlist[slot_id].asic_to_cpu_ports = 1;
					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_8c_8800_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_8c_8800_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_8c_8800_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_8c_8800_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_8c_8800_cscd_bports[slot_id][i].trunk_id;
					}
					break;                 
					case   	AX81_AC12C_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_AC12C;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8800_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 2;
					g_bportlist[slot_id].asic_to_cpu_ports = 2;
					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_12c_8800_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_12c_8800_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_12c_8800_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_12c_8800_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_12c_8800_cscd_bports[slot_id][i].trunk_id;
					}
					break;                
				case   	AX81_2X12G12S_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_AC12C;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8800_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 2;
					g_bportlist[slot_id].asic_to_cpu_ports = 0;
					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_2x12g12s_8800_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_2x12g12s_8800_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_2x12g12s_8800_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_2x12g12s_8800_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_2x12g12s_8800_cscd_bports[slot_id][i].trunk_id;
					}
					break;
				case   	AX81_1X12G12S_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_1X12G12S;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8800_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 2;
					g_bportlist[slot_id].asic_to_cpu_ports = 2;
					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_1x12g12s_8800_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_1x12g12s_8800_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_1x12g12s_8800_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_1x12g12s_8800_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_1x12g12s_8800_cscd_bports[slot_id][i].trunk_id;
					}
					break;                 
                case   	AX81_12X_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_AC_12X;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8800_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 8;
					g_bportlist[slot_id].asic_to_cpu_ports = 0;
					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_12x_8800_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_12x_8800_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_12x_8800_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_12x_8800_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_12x_8800_cscd_bports[slot_id][i].trunk_id;
					}
					break;
                case   	AX81_AC4X_BOARD_CODE:
					g_bportlist[slot_id].board_type = AUTELAN_BOARD_AX81_AC_4X;
				    g_bportlist[slot_id].slot_id = (slot_id<=AX8800_SLOT_HALF?slot_id:slot_id+2)+1;
					g_bportlist[slot_id].slot_num = slotNum;
					g_bportlist[slot_id].asic_cscd_port_cnt = 8;
					g_bportlist[slot_id].asic_to_cpu_ports = 4;

					for(i=0;i<(g_bportlist[slot_id].asic_cscd_port_cnt+g_bportlist[slot_id].asic_to_cpu_ports);i++)
					{
						g_bportlist[slot_id].asic_cscd_bports[i].cscd_port = asic_4x_8800_cscd_bports[slot_id][i].cscd_port;
						g_bportlist[slot_id].asic_cscd_bports[i].master = asic_4x_8800_cscd_bports[slot_id][i].master;
						g_bportlist[slot_id].asic_cscd_bports[i].asic_id = asic_4x_8800_cscd_bports[slot_id][i].asic_id;
						g_bportlist[slot_id].asic_cscd_bports[i].bport = asic_4x_8800_cscd_bports[slot_id][i].bport;
						g_bportlist[slot_id].asic_cscd_bports[i].trunk_id = asic_4x_8800_cscd_bports[slot_id][i].trunk_id;
					}
	                break;	
					
                case   	AX81_CRSMU_BOARD_CODE:
                case	AX81_CRSMUE_BOARD_CODE:					
				default:
						g_bportlist[slot_id].board_type = -1;
					break;
			}
				
		}
	}

    ret = msync(g_bportlist, sizeof(struct asic_board_cscd_bport_s)*14, MS_SYNC);
	if( ret != 0 )
    {
        syslog_ax_conntable_err("msync shm_conntable failed \n" );
		return VLAN_RETURN_CODE_ERR_GENERAL;
    }	
    syslog_ax_conntable_dbg("init g_bportlist[] OK on SMU !\n");	
    return 0;
}





#if 0
asic_board_cscd_bport_t* asic_cscd_bbports[]=
{
	[BOARD_TYPE_AX81_AC12C] = &ax81_ac12c_cscd_bport_t, 
	[BOARD_TYPE_AX81_AC8C] ........
};

int asic_cscd_8610_getbport();
{
   	asic_cscd_bbports.board_type = npd_get_board_type();

	switch(board_type)
	{
        case(BOARD_TYPE_AX81_AC12C)
			asic_cscd_bbports.asic_cscd_port_cnt=2;
		    asic_board_cscd_bbports->asic_cscd_bports = asic_12c_8610_cscd_bports;
		    asic_board_cscd_bbports->asic_cscd_bports = asic_12c_8610_cscd_bports;
			malloc((slot_num-2)*asic_cscd_port_cnt*sizeof(asic_12c_8610_cscd_bports));
			
			break;
		case(BOARD_TYPE_AX81_12X)
			.............

	}
}


void main()
{
    asic_board_cscd_bbports_t asic_cscd_bbports = g_asic_board_cscd_bport[];	

	if(autelan_product == PRODUCT_TYPE_8610)
	{
		asic_cscd_bbports.slot_num = 10;

    	for(i=0,i<slot_num,i++)
    	{
            if(slot_in[i+1]==empty)
    			continue;
    		if(((i+1)==(slot_num/2)) || ((i+1)==(slot_num/2+1)))
    			continue;

			asic_cscd_bbports.slot_id = i+1;
			asic_cscd_8610_getbport(&asic_cscd_bbports);
				
    		
    	}
	}
	if(autelan_product == PRODUCT_TYPE_8603)
	{
        ..........
	}

/*假设３槽为12c,有两个背板口，分别为0,1。设备为8610,12c上第二个背板口对应的bport信息为*/	
	asic_board_cscd_bbports[BOARD_TYPE_AX81_AC12C]->asic_12c_8610_cscd_bports[(3-1)*2+1]/*(slot_id-1)*cscd_num+port_id*/
    {
    .cscd_port = 26;/*参见npd_12c_config.c上的asic_12c_8610_cscd_bports大数组*/
	.master = 1;    /*slot5*/
	.asic_id = 0;   /*sum上的lion0*/
	.bport = 24;    /*对应的sum上的lion上的背板口id*/
	}
	
}
#endif








