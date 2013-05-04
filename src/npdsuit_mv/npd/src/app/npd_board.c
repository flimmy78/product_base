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
* npd_board.c
*
*
* CREATOR:
*		luoxun@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for board related routine.
*
* DATE:
*		03/21/2011	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.20 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "npd_log.h"
#include "npd_board.h"

/* for __WITH_NPD_CSCD zhangdi@autelan.com 2012-07-10 */
#include <config/npd_config.h>   

/* for asic board */
#include "npd_board_ax81_smu.h"
#include "npd_board_ax81_ac12c.h"
#include "npd_board_ax81_12x.h"
#include "npd_board_ax81_ac8c.h"
#include "npd_board_ax71_2x12g12s.h"
#include "npd_board_ax81_1x12g12s.h"
#include "npd_board_ax81_ac4x.h"
#include "npd_board_axxx_virtualboard.h"
#include "npd_vlan.h"
#include "npd_trunk.h"

/***********************************************************************/
BoardInfo board_info;
asic_board_t* asic_board = NULL;
vlan_list_t* g_vlanlist = NULL;
vlan_list_t* g_vlanback = NULL;
dst_trunk_s* g_dst_trunk = NULL;
char g_dis_dev[MAX_ASIC_NUM] = {0,0}; /*global distributed dev num arry[i] i represent asic num,and the {0,0} represent devNum*/
char init_mac[] = "001122AABBCC";
unsigned char npd_cscd_type = 0;    /* CSCD_TYPE refer to */

asic_board_t* g_asic_board[]=
{
	[BOARD_TYPE_AX81_SMU] = &ax81_smu_board_t, /*&ax81_smu_board_t,*/
	[BOARD_TYPE_AX81_AC12C] = &ax81_ac12c_board_t, 
	[BOARD_TYPE_AX81_AC8C] = &ax81_ac8c_board_t,
	[BOARD_TYPE_AX81_12X] = &ax81_12x_board_t,
	[BOARD_TYPE_AX71_2X12G12S] = &ax71_2x12g12s_board_t,
	[BOARD_TYPE_AX81_1X12G12S] = &ax81_1x12g12s_board_t,	
	[BOARD_TYPE_AX81_AC4X] = &ax81_ac4x_board_t,
	[BOARD_TYPE_AXXX_VIRTUALBOARD] = &axxx_virtualboard_board_t,
	[BOARD_TYPE_MAX] = NULL
};

int npd_board_init_set_system_static_FDB( int slot_id)
{
	int rc = -1;
	unsigned char   macAddr1[6] = {0x00,0x1f,0x64,0xff,0xff,0xf0};
	unsigned char   macAddr2[6] = {0x00,0x1f,0x64,0xff,0xff,0xf1};
	unsigned char   macAddr3[6] = {0x00,0x1f,0x64,0xff,0xff,0xf2};
	unsigned short	vlanId = 4094;
    if(slot_id == 1)
    {   
        rc= nam_static_fdb_entry_mac_vlan_port_set_distributed(macAddr3,4094,0,12);
        if (rc != 0) {
    	    npd_syslog_err("npd_dbus_fdb_config_mac_vlan_port:: fdb entry  macAddr3ERROR. \n");
			return NPD_FAIL;
        }
		rc= nam_static_fdb_entry_mac_vlan_port_set_distributed(macAddr2,4094,1,24);
        if (rc != 0) {
    	    npd_syslog_err("npd_dbus_fdb_config_mac_vlan_port:: fdb entry macAddr2 ERROR. \n");
			return NPD_FAIL;
        }

    }
    else if(slot_id == 2)
	{   
        rc= nam_static_fdb_entry_mac_vlan_port_set_distributed(macAddr3,4094,2,12);
        if (rc != 0) {
    	    npd_syslog_err("npd_dbus_fdb_config_mac_vlan_port:: fdb entry macAddr3 ERROR. \n");
			return NPD_FAIL;

        }
		rc= nam_static_fdb_entry_mac_vlan_port_set_distributed(macAddr1,4094,3,24);
        if (rc != 0) {
    	    npd_syslog_err("npd_dbus_fdb_config_mac_vlan_port:: fdb entry macAddr1 ERROR. \n");
			return NPD_FAIL;
        }
    }
    else
    {
	npd_syslog_dbg("Slot id error.\n");		
    }
    return NPD_TRUE;	
}
/**********************************************************************
 * get_num_from_file
 *
 *  DESCRIPTION:
 *          Get number from proc file.
 *
 *  INPUT:
 *          filename    - the path of proc file
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          number
 *          DATA_INVALID
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
int get_num_from_file(const char *filename)
{
    FILE *fp = NULL;
    int data = DATA_INVALID;
	int ret = 0;
    
    fp = fopen(filename, "r");
    if (fp)
    {
        ret = fscanf(fp, "%d", &data);
		if(ret !=1)	/* code optimize: Unchecked return value from library  houxx@autelan.com  2013-1-19 */
		{
			syslog_ax_board_dbg("catch data error!\n");
			fclose(fp);			
			return data;
		}		
    }
	else
	{
        syslog_ax_board_dbg("%s open error!\n", filename);
		return data; 
	}
	
	fclose(fp);
    syslog_ax_board_dbg("read %s return:%d \n",filename,data);
    return data;
}

/**********************************************************************
 *  get_board_mac_from_sn
 *
 *  DESCRIPTION:
 *          genrate mac from sn.
 *
 *  INPUT:
 *          filename    - the path of proc file
 *
 *  OUTPUT:
 *          mac - the genrate mac from sn
 *
 *  RETURN:
 *          ret - NPD_FAIL NPD_SUCCESS
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
int get_board_mac_from_sn(char *board_mac)
{
    FILE *fp = NULL;
	/* 02020125 C       120   119 00027 + NULL = 21 chars */
	/* ERP factory hardware-version moth board-serial number */
    char board_sn[21] = {0};
    const char *filename = "/devinfo/module_sn";
	int board_sn_lenth = 20;
    int ret = NPD_FAIL;
	int i = 0;
    fp = fopen(filename, "r");
    if (fp)
    {
        if(fgets(board_sn,board_sn_lenth+1,fp)!=NULL)
		{
            syslog_ax_board_dbg("read sn %s :%s \n",filename,board_sn);
    		/* we use [0125] + [119 00027}  */
    		board_mac[0] = board_sn[4]; 
    		board_mac[1] = board_sn[5]; 
    		board_mac[2] = board_sn[6]; 
    		board_mac[3] = board_sn[7]; 
    		for(i;i<8;i++)
    		{
        		board_mac[4+i] = board_sn[12+i]; 			
    		}
            syslog_ax_board_dbg("genrate mac : %s \n",board_mac);
        }
		else
		{
            syslog_ax_board_dbg("fgets return NULL !\n");
		}
		fclose(fp);
		ret = NPD_SUCCESS;
    }
	else
	{
        syslog_ax_board_dbg("open file %s error!\n", filename);
	}
    return ret;
}
/**********************************************************************
 * npd_get_boardinfo
 *
 *  DESCRIPTION:
 *          Initialize boardinfo.
 *
 *  INPUT:
 *          p_boardinfo    - the pointer point to BoardInfo struct.
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          None
 *
 *  COMMENTS:
 *          None
 *
 **********************************************************************/
static void npd_get_boardinfo(BoardInfo *p_boardinfo)
{
	p_boardinfo->system_type = get_num_from_file("/proc/product_info/distributed_product");
	p_boardinfo->product_type = get_num_from_file("/proc/product_info/product_type");	
	p_boardinfo->board_type = get_num_from_file("/proc/product_info/board_type_num");
    p_boardinfo->slot_id = get_num_from_file("/proc/product_info/board_slot_id");

    /* init the master info, and they are defined after system-ready */
    p_boardinfo->is_master = -1;
    p_boardinfo->is_active_master = -1;
	
    /* for no distributed system,set board_type to -1,so it can not run into the branch of distributed */
	if(p_boardinfo->system_type == NO_DISTRIBUTED)
	{
		/*for board type on 5612 is 0, the same with AX81_SMU,so change it. */
		p_boardinfo->board_type = -1;
	    syslog_ax_board_dbg(" make BOARD_TYPE to -1 on box ! \n");
	
	}
        
	/* for AX71_CRSMU,must use the old code */
	if(p_boardinfo->board_type == BOARD_TYPE_AX71_CRSMU)
	{
		/*just for the ax71-crsmu must use the old code in branch NO_DISTRIBUTED*/
		p_boardinfo->system_type = NO_DISTRIBUTED;
	    syslog_ax_board_dbg(" make system_type to NO_DISTRIBUTED on ax71-crsmu ! \n");
		
	}

#ifdef __WITH_NPD_CSCD
    npd_cscd_type = 1;
#endif
	/*zhangcl@autelan.com added for cscd system*/
	if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX7605I)
	{
		npd_cscd_type = 1;
	}
    syslog_ax_board_dbg("get CSCD_TYPE :%d\n",CSCD_TYPE);

    syslog_ax_board_dbg("get system_type :%d\n",p_boardinfo->system_type);
    syslog_ax_board_dbg("get product_type :%d\n",p_boardinfo->product_type);
    syslog_ax_board_dbg("get board_type :%d\n",p_boardinfo->board_type);
    syslog_ax_board_dbg("get slot_id :%d\n",p_boardinfo->slot_id);	
}




/**********************************************************************
 * npd_get_virtual_boardinfo
 *
 *  DESCRIPTION:
 *          Initialize virtual board`s boardinfo.ep a new unknow board
*           or 5612i work on aw2.x
 *
 *  INPUT:
 *          p_boardinfo    - the pointer point to BoardInfo struct.
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          None
 *
 *  COMMENTS:
 *          wangchong@autelan.com 2012-7-27
 *
 **********************************************************************/

void npd_get_virtual_boardinfo(BoardInfo *p_boardinfo)
{
    if (p_boardinfo->board_type == BOARD_TYPE_AX81_SMU || \
		p_boardinfo->board_type == BOARD_TYPE_AX81_AC12C || \
		p_boardinfo->board_type == BOARD_TYPE_AX81_AC8C || \
		p_boardinfo->board_type == BOARD_TYPE_AX81_12X || \
		p_boardinfo->board_type == BOARD_TYPE_AX71_2X12G12S || \
		p_boardinfo->board_type == BOARD_TYPE_AX81_1X12G12S || \
		p_boardinfo->board_type == BOARD_TYPE_AX81_AC4X || \
		p_boardinfo->board_type == BOARD_TYPE_AX71_CRSMU )
    {
        /* do nothing */
	}
	else
	{
    	p_boardinfo->system_type = IS_DISTRIBUTED;
	    p_boardinfo->product_type = 2;  /*2 is 8610*/
	    p_boardinfo->slot_id = 1; /*xie si*/
	    p_boardinfo->board_type = BOARD_TYPE_AXXX_VIRTUALBOARD;    
        syslog_ax_board_dbg("*************in Virtual branch for a virtual board***********");
    	
        syslog_ax_board_dbg("virtual_board get system_type :%d\n",p_boardinfo->system_type);
        syslog_ax_board_dbg("virtual_board get product_type :%d\n",p_boardinfo->product_type);
        syslog_ax_board_dbg("virtual_board get board_type :%d\n",p_boardinfo->board_type);
        syslog_ax_board_dbg("virtual_board get slot_id :%d\n",p_boardinfo->slot_id);
	}
}




/**********************************************************************
 * npd_init_boardinfo
 *
 *  DESCRIPTION:
 *          Initialize global struct 'board_info'
 *
 *  INPUT:
 *          None
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          None
 *
 *  COMMENTS:
 *          Called only by npd main()
 *
 **********************************************************************/
void npd_init_boardinfo(void)
{
    npd_get_boardinfo(&board_info);
	
	/*wangchong@autelan for virtual-board, 2012-07-28 */
	npd_get_virtual_boardinfo(&board_info);
	
	if(SYSTEM_TYPE==IS_DISTRIBUTED)
	{
        asic_board = g_asic_board[BOARD_TYPE];   /* get local board */
    	asic_board->slot_id = SLOT_ID;
	}	
}

int npd_cscd_port_config(void)
{
	int ret = NPD_FAIL;
	int i=0;

	for(i=0; i< asic_board->asic_cscd_port_cnt; i++)
	{
	
		ret = force_port_to_cscd_mode(asic_board->asic_cscd_ports[i].dev_num,asic_board->asic_cscd_ports[i].port_num);
		if(ret != NPD_SUCCESS)
		{
			syslog_ax_board_err("config dev %d port %d to cscd port fail !\n",asic_board->asic_cscd_ports[i].dev_num,asic_board->asic_cscd_ports[i].port_num);
			return NPD_FAIL;
		}
		syslog_ax_board_dbg("config dev %d port %d to cscd mode successfully\n",asic_board->asic_cscd_ports[i].dev_num,asic_board->asic_cscd_ports[i].port_num);
	}
	
	return ret;
}


int npd_set_dev_map_table(void)
{
    int ret = NPD_FAIL;

	switch(PRODUCT_TYPE)
	{
		case AUTELAN_PRODUCT_AX7605I:
            switch(BOARD_TYPE)
            {
                case BOARD_TYPE_AX71_2X12G12S:
                    nam_asic_redistribute_devnum(0, 4);
					g_dis_dev[0] = 4;/*set the devNum*/
                    nam_asic_set_device_map_table(0, 0, 26);
                    nam_asic_set_device_map_table(0, 1, 26);
                    nam_asic_set_device_map_table(0, 2, 27);
                    nam_asic_set_device_map_table(0, 3, 27);
                    break;
                case BOARD_TYPE_AX71_CRSMU:
                    if (1 == SLOT_ID)
                    {
						g_dis_dev[0] = 0;/*set the devNum*/
						g_dis_dev[1] = 1;/*set the devNum*/
                        nam_asic_set_device_map_table(0, 1, 26);
                        nam_asic_set_device_map_table(0, 4, 25);
                        nam_asic_set_device_map_table(1, 0, 27);
                        nam_asic_set_device_map_table(1, 4, 27);
        				
                        ret = nam_pvlan_cscd_port_config_spi(1,24,1,26);
                        if(NPD_SUCCESS != ret) {
                            syslog_ax_board_err("cscd port config spi error\n");
                        }

                        ret = nam_pvlan_cscd_port_config_spi(0,12,1,26);
                        if(NPD_SUCCESS != ret) {
                            syslog_ax_board_err("cscd port config spi error\n");
                        }				
                        syslog_ax_board_dbg("npd_board_init_set_system_static_FDB. slot id:%d\n",SLOT_ID);
        				npd_board_init_set_system_static_FDB(SLOT_ID);

                    }
                    else if(2 == SLOT_ID)
                    {
                        nam_asic_redistribute_devnum(0, 2);
                        nam_asic_redistribute_devnum(1, 3);
						g_dis_dev[0] = 2;/*set the devNum*/
						g_dis_dev[1] = 3;/*set the devNum*/
                        nam_asic_set_device_map_table(0, 3, 26);
                        nam_asic_set_device_map_table(0, 4, 25);
                        nam_asic_set_device_map_table(1, 2, 27);
                        nam_asic_set_device_map_table(1, 4, 27);
        				
                        ret = nam_pvlan_cscd_port_config_spi(1,24,3,26);
                        if(NPD_SUCCESS != ret) {
                            syslog_ax_board_err("cscd port config spi error\n");
                        }

                        ret = nam_pvlan_cscd_port_config_spi(0,12,3,26);
                        if(NPD_SUCCESS != ret) {
                            syslog_ax_board_err("cscd port config spi error\n");
                        }
                        syslog_ax_board_dbg("npd_board_init_set_system_static_FDB. slot id:%d\n",SLOT_ID);
        				npd_board_init_set_system_static_FDB(SLOT_ID); 
                    }
                    /* Ingress Port<n> Bridge Configuration Register0, field <UcLocalEn>  */
                    ret = nam_port_unicast_local_en(1, 24, 1);
                    if(NPD_SUCCESS != ret) {
                        syslog_ax_board_err("set nam_port_unicast_local_en(1, 24, 1) error\n");
                    }			
                    break;
                default:
                    syslog_ax_board_warning("We are not interested in BOARD_TYPE %d, exit set_dev_map_table()\n", BOARD_TYPE);
                    break;
					
            }
			break;
        default:
            syslog_ax_board_warning("We are not interested in PRODUCT_TYPE %d, exit set_dev_map_table()\n", PRODUCT_TYPE);
            break;
	}
	syslog_ax_board_dbg("set dev map table OK \n");
    return NPD_SUCCESS;
}

/* Disable port25 of 804 */
int tmp_function(void)
{
    unsigned char	devNum  = 1; /*804*/
	unsigned char	portNum = 25;
	unsigned long	portAttr = 0;
    
    return nam_set_ethport_enable(devNum, portNum, portAttr);
}
/* board init for ax71-crsmu */
int npd_init_ax71_crsmu(void)
{
    unsigned char	devNum  = 1; /*804*/
	unsigned char	portNum = 25;
	unsigned long	portAttr = 0;
	unsigned long   mtuIndex = 0;
    int ret = RET_FAIL;

    /* Set Asic PVE enable */
    ret = nam_pvlan_set_pve_enable(0, 1);
    if (NPD_SUCCESS != ret)
    {
        syslog_ax_board_err("Set 275 PVE enable fail, return\n");
		return RET_FAIL;
    }

    ret = nam_pvlan_set_pve_enable(1, 1);
    if (NPD_SUCCESS != ret)
    {
        syslog_ax_board_err("Set 804 PVE enable fail, return\n");
		return RET_FAIL;
    }

    #if 0  /* for AX81-AC8C test on AX7605i */
	/* for AX81-AC8C test on AX7605i, set port 24,25 on 275 to 10G mode, vlan 1 */
	ret = force_port_to_xg_mode(0,25);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("force port to 10G mode error !\n");
        return RET_FAIL;
    }
	ret = force_port_to_xg_mode(0,24);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("force port to 10G mode error !\n");
        return RET_FAIL;
    }
	syslog_ax_board_dbg("force port (0,24) (0,25) to XG mode OK!\n");	
    #endif
	
    /* Disable port25 of 804 */	
    ret = nam_set_ethport_enable(devNum, portNum, portAttr);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("disable port 25 fail, return %d\n",ret);
        return RET_FAIL;
    }
	
	/* set mtu of port25,port26 on 275, to 1548 */
	ret = nam_set_ethport_mru(0,25,1548);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
        return RET_FAIL;
    }	
	ret = nam_set_ethport_mru(0,26,1548);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
        return RET_FAIL;
    }
	
	/* set mtu of port24,25,26,27 on 804, to 1548 */	
	ret = nam_set_ethport_mru(1,24,1548);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
        return RET_FAIL;
    }
	ret = nam_set_ethport_mru(1,25,1548);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
        return RET_FAIL;
    }	
	ret = nam_set_ethport_mru(1,26,1548);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
        return RET_FAIL;
    }
	ret = nam_set_ethport_mru(1,27,1548);
    if (RET_SUCCESS != ret)
    {
        syslog_ax_board_err("nam set mtu fail, return %d\n",ret);
        return RET_FAIL;
    }	

	/* set vlan mtu of 275, VLAN MRU Profile Configuration Reg-ister */
    devNum  = 0; 
    for(mtuIndex = 0; mtuIndex < 8; mtuIndex++)
    {
        ret = nam_config_vlan_mtu(devNum,mtuIndex,1548);
        if (RET_SUCCESS != ret)
        {
            syslog_ax_board_err("nam set vlan mtu fail, return %d\n",ret);
            return RET_FAIL;
        }
    }
	
	/* set vlan mtu of 804, VLAN MRU Profile Configuration Reg-ister */    
    devNum  = 1; 
    for(mtuIndex = 0; mtuIndex < 8; mtuIndex++)
    {
        ret = nam_config_vlan_mtu(devNum,mtuIndex,1548);
        if (RET_SUCCESS != ret)
        {
            syslog_ax_board_err("nam set vlan mtu fail, return %d\n",ret);
            return RET_FAIL;
        }
    }	
	return RET_SUCCESS;	/* code optimize: Missing return statement houxx@autelan.com  2013-1-19 */
}

/**********************************************************************
 * npd_init_board
 *
 *  DESCRIPTION:
 *          Call different init function for different board.
 *
 *  INPUT:
 *          None
 *
 *  OUTPUT:
 *          None
 *
 *  RETURN:
 *          None
 *
 *  COMMENTS:
 *          Called only by npd main()
 *
 **********************************************************************/
void npd_init_board(void)
{
	int ret= NPD_FAIL;
    switch(BOARD_TYPE)
    {
        case BOARD_TYPE_AX71_2X12G12S:
            npd_init_ax71_2x12g12s();
            syslog_ax_board_dbg("AX71_2X12G12S init finished.\n");
            break; 
		case BOARD_TYPE_AX81_1X12G12S:
            npd_init_ax81_1x12g12s();
            syslog_ax_board_dbg("AX81_1X12G12S init finished.\n");
            break;				
        case BOARD_TYPE_AX81_AC12C:
            npd_init_ax81_ac12c();
            syslog_ax_board_dbg("AX81_AC12C init finished.\n");
            break;
        case BOARD_TYPE_AX81_AC8C:
            npd_init_ax81_ac8c();
            syslog_ax_board_dbg("AX81_AC8C init finished.\n");
            break;			
        case BOARD_TYPE_AX71_CRSMU:
			npd_init_ax71_crsmu();
            syslog_ax_board_dbg("AX71_CRSMU init finished.\n");
            break;
        case BOARD_TYPE_AX81_SMU:
			npd_init_ax81_smu();
            syslog_ax_board_dbg("AX81_SMU init finished.\n");
            break;
        case BOARD_TYPE_AX81_12X:
			npd_init_ax81_12x();
            syslog_ax_board_dbg("AX81_12X init finished.\n");
            break;
		case BOARD_TYPE_AX81_AC4X:
		    npd_init_ax81_ac4x();
		    syslog_ax_board_dbg("AX81_AC4X init finished.\n");
			break;
		case BOARD_TYPE_AXXX_VIRTUALBOARD:
		    npd_init_axxx_virtualboard();
		    syslog_ax_board_dbg("AXXX_VIRTUALBOARD init finished.\n");
			break;
        default:
            syslog_ax_board_warning("We are not interested in BOARD_TYPE %d, exit npd_init_board()\n", BOARD_TYPE);
            break;
    }
	/* for different page needed in board AX71-2X12G12S */
    phy_page_set(BOARD_TYPE);
	
    /* init distributed vlan */
    ret = npd_init_distributed_vlan();
	if(ret != NPD_SUCCESS)
	{
		npd_syslog_err("npd_init_distributed_vlan error !!!\n");
	}		

    /* init distributed trunk */    
	ret = npd_init_distributed_trunk();
	if(ret != NPD_SUCCESS)
	{
		npd_syslog_err("npd_init_distributed_trunk error !!!\n");
	}
	/*set default fdb aging time to 300s*/
	ret = nam_fdb_table_agingtime_set(300);
	if(ret != NPD_SUCCESS)
	{
		npd_syslog_err("set default agingtime err !!! \n");
	}
	
	/* init per-board, except AX71-CRSMU */
	if(SYSTEM_TYPE == IS_DISTRIBUTED)
	{
        /* set cscd port TX premble to 4 bytes */		
        ret = npd_set_cscd_port_preamble_tx(4);
		if(ret != NPD_SUCCESS)
		{
			npd_syslog_err("npd set cscd port tx premble to 4 bytes fail !\n");
		}		
		
        /* set cscd port TX ipg to 8 bytes */
        ret = npd_set_cscd_port_ipg(0);
		if(ret != NPD_SUCCESS)
		{
			npd_syslog_err("npd set cscd port tx ipg to 8 bytes fail !\n");
		}		
    	/* init for QINQ */		
		ret = npd_vlan_qinq_init(0);
		if(ret != NPD_SUCCESS)
		{
			npd_syslog_err("npd vlan init qinq fail !\n");
		}
	}
	/* Set dev map for AX7605i */
	if(PRODUCT_TYPE == AUTELAN_PRODUCT_AX7605I)
	{
		ret = npd_set_dev_map_table();
		if(ret != NPD_SUCCESS)
		{
			npd_syslog_err("npd_set_dev_map_table error !!!\n");
			return ret;
		}
		else
		{
			npd_syslog_dbg("set dev map table successfully !\n");
		}		
	}
	else if(CSCD_TYPE == 1)
	{
		npd_syslog_dbg("entry cscd port set !!!\n");
		ret = npd_cscd_port_config();
		if(ret != NPD_SUCCESS)
		{
			npd_syslog_err("cscd port config fail !\n");
			return ret;
		}
		else
		{
			npd_syslog_dbg("set cscd port successfully !\n");
		}
		if(asic_board->distributed_system_dev_map_table_set)/*judge if the pointer is NULL*/
		{
			ret = asic_board->distributed_system_dev_map_table_set();
			if(ret != NPD_SUCCESS)
			{
				npd_syslog_err("set dev map table fail !\n");
				return ret;
			}
			else
			{
				npd_syslog_dbg("set dev map table successfully !\n");
			}
		}
	}
	/* Trunk config for muti-cscd port board, except AX71-CRSMU */
	if(SYSTEM_TYPE == IS_DISTRIBUTED)
	{
    	if(asic_board->board_cscd_port_trunk_config)/*judge if the pointer is NULL*/
    	{
    		ret = asic_board->board_cscd_port_trunk_config();
    		if(ret != NPD_SUCCESS)
    		{
    			npd_syslog_err("config cscd port to trunk fail !\n");
    			return ret;
    		}
    		else
    		{
    			npd_syslog_dbg("config cscd port to trunk successfully !\n");
    		}
    	}
	}
	/* enable device for AX71-CRSMU, for traffic on obc0. */	
	if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
	{
        nam_enable_device(0,1);   /* enable asic 0, 98DX275 */
	    nam_enable_device(1,1);   /* enable asic 1, 98DX804 */
        syslog_ax_board_dbg("...Enable asic 275 & 804 for AX71-CRSMU OK!\n");		
	}
	/* for virtual board */
	if(BOARD_TYPE == BOARD_TYPE_AXXX_VIRTUALBOARD)
	{
        nam_enable_device(0,1);   /* enable asic 0 */
	    nam_enable_device(1,1);   /* enable asic 1 */
        syslog_ax_board_dbg("...virtual board enable asic done.\n");		
	}	
		
}

struct product_type_data_s my_board_product;
struct module_type_data_s my_board_module;
struct module_type_data_s temp_board_module;

int npd_init_productinfo_distributed(void)
{	
	memset((void*)(&productinfo),0,sizeof(struct product_s));
	productinfo.capbmp= asic_board->capbmp;   /* for function init */
	productinfo.runbmp= asic_board->runbmp;
    productinfo.local_chassis_slot_no= asic_board->slot_id;
    syslog_ax_board_dbg("productinfo.capbmp:0x%x\n",productinfo.capbmp);	
    syslog_ax_board_dbg("productinfo.local_chassis_slot_no:%d\n",productinfo.local_chassis_slot_no);	
	
	/* read from eeprom */
	memset((void*)(&productinfo.sysinfo),0,sizeof(struct product_sysinfo_s));	
	productinfo.sysinfo.basemac= init_mac;
    productinfo.sysinfo.name= "old";
    productinfo.sysinfo.sn= "000";
    productinfo.sysinfo.sw_name= "none";

    /* for product_type_data_distributed() return my board_product */
	memset((void*)(&my_board_product),0,sizeof(struct product_type_data_s));	
    my_board_product.chassis_slot_count = DISTRIBUTED_SLOT_MAX;    /* this should be dynamic, CHASSIS_SLOT_COUNT */
    my_board_product.chassis_slot_start_no = DISTRIBUTED_SLOT_START_NO;   /* CHASSIS_SLOT_START_NO */

    /* for module_type_data_distributed() return my module */
	memset((void*)(&my_board_module),0,sizeof(struct module_type_data_s));	
    my_board_module.eth_port_count = asic_board->asic_port_cnt_visable;   /* ETH_LOCAL_PORT_COUNT */
    my_board_module.eth_port_start_no = asic_board->asic_port_start_no;   /* ETH_LOCAL_PORT_START_NO */

	/* return not my module */
	memset((void*)(&temp_board_module),0,sizeof(struct module_type_data_s));
    syslog_ax_board_dbg("npd_init_productinfo_distributed ...OK\n");	
	return 0;
}

void npd_init_chassis_slots_distributed
(
	void
) 
{
	int i;
	syslog_ax_c_slot_dbg("chassis slot count is %d\n",CHASSIS_SLOT_COUNT);
	chassis_slots = malloc(sizeof(struct chassis_slot_s *) *(CHASSIS_SLOT_COUNT));
		
	memset((void *)chassis_slots,0,CHASSIS_SLOT_COUNT);
	for (i = 0; i < CHASSIS_SLOT_COUNT; i++) {
		chassis_slots[i] = malloc(sizeof(struct chassis_slot_s));
		memset((void*)(chassis_slots[i]),0,sizeof(struct chassis_slot_s));
		if(LOCAL_CHASSIS_SLOT_INDEX == i)
		{
    		chassis_slots[i]->module.id 		= asic_board->id; /*0xF0F0F0F0*/
    		chassis_slots[i]->module.state 		= asic_board->state;
    		chassis_slots[i]->module.hw_version = asic_board->hw_version;
    		chassis_slots[i]->module.modname 	= asic_board->board_name;
    		chassis_slots[i]->module.sn 		= asic_board->sn;
    		chassis_slots[i]->module.ext_slot_data = asic_board->ext_slot_data;
    		syslog_ax_c_slot_dbg("init slot %d %s  OK\n",CHASSIS_SLOT_INDEX2NO(i),module_id_str(chassis_slots[i]->module.id));
    		syslog_ax_c_slot_dbg("board name: %s \n",chassis_slots[i]->module.modname);
		}
		/* reset one slot */
        else
		{
    		chassis_slots[i]->module.id 		= MODULE_ID_NONE;
    		chassis_slots[i]->module.state 		= MODULE_STAT_DISABLED;
    		chassis_slots[i]->module.hw_version = 0;
    		chassis_slots[i]->module.modname 	= "null";
    		chassis_slots[i]->module.sn 		= "null";
    		chassis_slots[i]->module.ext_slot_data = NULL;
    	}
	}
	return;
}

struct product_type_data_s *product_type_data_distributed
(
	unsigned int productId
)
{
    return &my_board_product;
}

struct module_type_data_s *module_type_data_distributed
(
	unsigned int module_id
)
{
	if(module_id==MODULE_ID_DISTRIBUTED)
	{		
        return &my_board_module;
	}
	else
	{	
        return &temp_board_module;		
	}
}

unsigned int npd_get_global_index_by_devport_distributed
(
	IN 		unsigned char devNum,
	IN 		unsigned char portNum,
	OUT		unsigned int *eth_g_index
)
{
	int i=0;
	unsigned int ret = NPD_FAIL;	
	
	unsigned int slotNo,localPortNo;
    for(i =0;i<asic_board->asic_port_cnt;i++)
	{
		if((devNum == asic_board->asic_eth_ports[i].dev_num)&&(portNum == asic_board->asic_eth_ports[i].port_num))
		{
	        localPortNo = i + asic_board->asic_port_start_no;
        	slotNo = asic_board->slot_id;
			ret = NPD_SUCCESS;
		}
	}
	if(ret == NPD_SUCCESS)
	{
    	*eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotNo,localPortNo);	
    	syslog_ax_c_slot_dbg("\nnpd convert devport (%d,%d) to %d-%d,eth_g_index:%d OK.\n",devNum,portNum,slotNo,localPortNo,*eth_g_index);
	}
	else
	{
		ret = NPD_FAIL;
		syslog_ax_c_slot_err("\nnpd convert devport (%d,%d) to eth_g_index ERROR!\n",devNum,portNum);
	}
	return ret;
}

unsigned int npd_get_devport_by_global_index_distributed
(
	IN	unsigned int eth_g_index,
	OUT	unsigned char *devNum,
	OUT	unsigned char *portNum
)
{
	unsigned int ret = NPD_FAIL;    /* 0XFFFFFFFF */		
	int port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	if((port_index>=0)&&(port_index<asic_board->asic_port_cnt))
	{
        *portNum = asic_board->asic_eth_ports[port_index].port_num;
    	*devNum = asic_board->asic_eth_ports[port_index].dev_num;	
    	syslog_ax_c_slot_dbg("npd convert index %#x to devport (%d,%d) OK.\n",eth_g_index,*devNum,*portNum);
		ret = NPD_SUCCESS;
	}
	else
	{
    	syslog_ax_c_slot_dbg("npd convert index %#x to port_index %d , not asic port!\n",eth_g_index,port_index);		
		ret = NPD_FAIL;
	}
	return ret;	
}

int npd_asic_func_init(int system_type)
{
	if(system_type ==IS_DISTRIBUTED)
	{
        npd_init_productinfo = npd_init_productinfo_distributed;
		npd_init_chassis_slots = npd_init_chassis_slots_distributed;
		product_type_data = product_type_data_distributed;
		module_type_data = module_type_data_distributed;
		npd_get_global_index_by_devport = npd_get_global_index_by_devport_distributed;
		npd_get_devport_by_global_index = npd_get_devport_by_global_index_distributed;
	}
	else
	{
		npd_init_productinfo= npd_init_productinfo_old;
		npd_init_chassis_slots = npd_init_chassis_slots_old;
	
		product_type_data = product_type_data_old;
		module_type_data = module_type_data_old;
		npd_get_global_index_by_devport = npd_get_global_index_by_devport_old;
		npd_get_devport_by_global_index = npd_get_devport_by_global_index_old;	
	}
	syslog_ax_board_dbg("init asic function OK!\n");	
	return 0;
}

/*
    For equipment test
*/
unsigned int equipment_test(void)
{
	int ret = 0;
	unsigned int vid = 4000;
	unsigned int devnum = 0;   /* no use,just for printf */

	if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
	{
		/* AX7605i equipment test */
		if(2==SLOT_ID)
		{
    		/* 275 */
    		devnum = 0;
    		vid = 1000;
            create_vlan(devnum,vid);
            nam_asic_vlan_entry_ports_add(devnum, vid, 25,1);
            nam_asic_vlan_entry_ports_add(devnum, vid, 26,1);
    		/* 804 */
    		devnum = 1;
    		vid = 1000;
            create_vlan(devnum,vid);
            nam_asic_vlan_entry_ports_add(devnum, vid, 27,1);
            nam_asic_vlan_entry_ports_add(devnum, vid, 24,0);
			/* set port 24 PVLAN disable */
			nam_pvlan_disable_port_pve(devnum,24,0,0);
    		/* disable port 25 26 on 804 */
		    nam_set_ethport_enable(devnum,25,0);
		    nam_set_ethport_enable(devnum,26,0);			
		}
		else if(1==SLOT_ID)
		{
			#if 0   /* for AX81-2X12G12S & AX81-AC8C test*/
            ret = force_port_to_xg_mode(0,25);
            if (ret != 0)
            {
                return ret;
            }		
        	nam_syslog_dbg("275 set port 25 to xg mode OK!\n");
			#else
    		/* 275 */
    		devnum = 0;
    		vid = 2000;
            create_vlan(devnum,vid);
            nam_asic_vlan_entry_ports_add(devnum, vid, 25,1);
            nam_asic_vlan_entry_ports_add(devnum, vid, 26,1);
    		/* 804 */
    		devnum = 1;
    		vid = 2000;
            create_vlan(devnum,vid);
            nam_asic_vlan_entry_ports_add(devnum, vid, 27,1);
            nam_asic_vlan_entry_ports_add(devnum, vid, 24,0);
		    /* set port 24 PVLAN disable */
			nam_pvlan_disable_port_pve(devnum,24,0,0);
    		/* disable port 25 26 on 804*/
		    nam_set_ethport_enable(devnum,25,0);
		    nam_set_ethport_enable(devnum,26,0);
			#endif
			
		}
		else
		{
    		syslog_ax_board_dbg("the board do not need ge_xg_test!\n");			
		}		
		
	}
	else if(SYSTEM_TYPE == IS_DISTRIBUTED)
	{
		asic_board->my_equipment_test();
	}
	else
	{
		syslog_ax_board_dbg("the board do not need ge_xg_test!\n");
	}	
	return 0;	
}

/* Disable cscd port on Standby MCB */
int npd_disable_cscd_port(void)
{
    int ret = -1;
	int i = 0;
	int devnum = 0;
	int portnum = 0;
	int eth_g_index = 0;
	if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
	{
        ret = nam_set_ethport_enable(0, 25, 0);
        if (RET_SUCCESS != ret)
        {
            syslog_ax_board_err("disable port (%d,%d) fail !\n",0, 25);
            return RET_FAIL;
        }
        syslog_ax_board_dbg("disable port (%d,%d) OK\n",0, 25);	
	}
	else
	{
        for(i=0; i < asic_board->asic_cscd_port_cnt; i++)
        {
        	devnum = asic_board->asic_cscd_ports[i].dev_num;
        	portnum = asic_board->asic_cscd_ports[i].port_num;

			/* 1.disable cscd port */
            ret = nam_set_ethport_enable(devnum,portnum, 0);
            if (RET_SUCCESS != ret)
            {
                syslog_ax_board_err("disable port (%d,%d) fail !\n",devnum,portnum);
                return RET_FAIL;
            }
            syslog_ax_board_dbg("disable port (%d,%d) OK\n",devnum, portnum);						

			/* 2.delete the FDB of cscd port */
        	ret = npd_get_global_index_by_devport(devnum,portnum,&eth_g_index);
			if(0 != ret)
			{
                syslog_ax_board_err ("Fail to get gloable index when delete fdb one!/n");
                return RET_FAIL;	 
			}
			
            ret =nam_fdb_table_delete_entry_with_port(eth_g_index);
            if (ret != 0)
            {
                syslog_ax_board_err("delete fdb of port (%d,%d) fail !\n",devnum,portnum);
                return RET_FAIL;
            }
            syslog_ax_board_dbg("delete fdb of port  (%d,%d) OK\n",devnum, portnum);
		}
	}	
    return ret;
}

/* Enable cscd port on Active MCB */
int npd_enable_cscd_port(void)
{
    int ret = -1;
	int i = 0;
	int devnum = 0;
	int portnum = 0;
	if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
	{
        ret = nam_set_ethport_enable(0, 25, 1);
        if (RET_SUCCESS != ret)
        {
            syslog_ax_board_err("enable port (%d,%d) fail !\n",0, 25);
            return RET_FAIL;
        }
        syslog_ax_board_dbg("enable port (%d,%d) OK\n",0, 25);	
	}
	else
	{
        for(i=0; i < asic_board->asic_cscd_port_cnt; i++)
        {
        	devnum = asic_board->asic_cscd_ports[i].dev_num;
        	portnum = asic_board->asic_cscd_ports[i].port_num;
			
            ret = nam_set_ethport_enable(devnum,portnum, 1);
            if (RET_SUCCESS != ret)
            {
                syslog_ax_board_err("enable port (%d,%d) fail !\n",devnum,portnum);
                return RET_FAIL;
            }
            syslog_ax_board_dbg("enable port (%d,%d) OK\n",devnum, portnum);						
		}
	}	
    return ret;
}
/* Set the port with preamble length for Tx to 4 bytes of cscd port */
int npd_set_cscd_port_preamble_tx(unsigned char length)
{
	unsigned char devnum = 0;
	unsigned char portnum = 0;
	unsigned char direction = 1;  /* TX */
    int ret = -1;
	int i = 0;	
    for(i=0; i < asic_board->asic_cscd_port_cnt; i++)
    {
    	devnum = asic_board->asic_cscd_ports[i].dev_num;
    	portnum = asic_board->asic_cscd_ports[i].port_num;
		
        ret = nam_set_port_preamble_length(devnum,portnum,direction,length);
        if (0 != ret)
        {
            syslog_ax_board_err("set port (%d,%d) Tx preamble to 4 bytes error !\n",devnum,portnum);
            return RET_FAIL;
        }
        syslog_ax_board_err("set port (%d,%d) Tx preamble to 4 bytes OK!\n",devnum,portnum);		
	}
    return ret;
}

/* Set the ipg of Tx to 8 bytes of cscd port */
int npd_set_cscd_port_ipg(unsigned char ipg_type)
{

	unsigned char devnum = 0;
	unsigned char portnum = 0;
    int ret = -1;
	int i = 0;	
    for(i=0; i < asic_board->asic_cscd_port_cnt; i++)
    {
    	devnum = asic_board->asic_cscd_ports[i].dev_num;
    	portnum = asic_board->asic_cscd_ports[i].port_num;

		ret = nam_set_xg_port_ipg(devnum,portnum,CPSS_DXCH_XG_PORT_IPG_FIXED_IPG_LAN_MODE_E,ipg_type);		
        if (0 != ret)
        {
            syslog_ax_board_err("set XG port (%d,%d) Tx ipg error !\n",devnum,portnum);
            return 1;
        }
        syslog_ax_board_err("set XG port (%d,%d) Tx ipg OK !\n",devnum,portnum);				
	}
    return ret;
}

int npd_board_4x_12c_test(unsigned char devNum,unsigned char endis)
{
	int ret = RET_SUCCESS;
	unsigned char Endis = 0;

	npd_syslog_dbg("endis = %d\n",endis);
	if((BOARD_TYPE == BOARD_TYPE_AX81_12X) || (BOARD_TYPE == BOARD_TYPE_AX81_AC4X))
	{
		if(endis)
		{
			/*delete port num from trunk 117*/
			ret = nam_asic_trunk_ports_del(devNum,117,34);		/* code optimize: Unchecked return value houxx@autelan.com  2013-1-19 */
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" del port 34 to trunk 117 error !\n");
				return RET_FAIL;
			}
			ret = nam_asic_trunk_ports_del(devNum,117,38);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" del port 38 to trunk 117 error !\n");
				return RET_FAIL;
			}
			ret = nam_asic_trunk_ports_del(devNum,117,42);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" del port 42 to trunk 117 error !\n");
				return RET_FAIL;
			}

			/*add port num to trunk 118*/
			ret = nam_asic_trunk_ports_add(devNum,118,34,1);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" add port 34 to trunk 118 error !\n");
				return RET_FAIL;
			}
			ret = nam_asic_trunk_ports_add(devNum,118,38,1);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" add port 38 to trunk 118 error !\n");
				return RET_FAIL;
			}
			ret = nam_asic_trunk_ports_add(devNum,118,42,1);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" add port 42 to trunk 118 error !\n");
				return RET_FAIL;
			}
			ret = nam_asic_trunk_load_balanc_set(devNum,118,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err("set trunk 118 load-balance based dest mac error ! \n");
				return RET_FAIL;
			}
		}
		else
		{
			/*delete port num from trunk 118*/
			ret = nam_asic_trunk_ports_del(devNum,118,34);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" del port 34 to trunk 118 error !\n");
			}
			ret = nam_asic_trunk_ports_del(devNum,118,38);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" del port 38 to trunk 118 error !\n");
			}
			ret = nam_asic_trunk_ports_del(devNum,118,42);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" del port 42 to trunk 118 error !\n");
			}

			/*add port num to trunk 117*/
			ret = nam_asic_trunk_ports_add(devNum,117,34,1);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" add port 34 to trunk 117 error !\n");
			}
			ret = nam_asic_trunk_ports_add(devNum,117,38,1);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" add port 38 to trunk 117 error !\n");
			}
			ret = nam_asic_trunk_ports_add(devNum,117,42,1);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" add port 42 to trunk 117 error !\n");
			}
		}
	}
	else if((BOARD_TYPE == BOARD_TYPE_AX81_AC12C) || (BOARD_TYPE == BOARD_TYPE_AX81_AC8C))
	{
		if(endis)
		{
			ret = nam_asic_trunk_ports_add(devNum,118,25,1);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" add port 25 to trunk 118 error !\n");
				return RET_FAIL;
			}
			ret = nam_asic_trunk_ports_add(devNum,118,26,1);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" add port 26 to trunk 118 error !\n");
				return RET_FAIL;
			}
			
			ret = nam_asic_trunk_load_balanc_set(devNum,118,LOAD_BANLC_MAC_IP_L4); /*set trunk load balance by dest mac */
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err("set trunk 118 load-balance based dest mac error ! \n");
				return RET_FAIL;
			}
		}
		else
		{
			ret = nam_asic_trunk_ports_del(devNum,118,25);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" del port 25 to trunk 118 error !\n");
			}
			ret = nam_asic_trunk_ports_del(devNum,118,26);
			if(ret != RET_SUCCESS)
			{
				npd_syslog_err(" del port 26 to trunk 118 error !\n");
			}
		}
	}
	else if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
	{
		if(!IS_ACTIVE_MASTER_NPD)
		{
			if(endis)
			{
				sleep(1);
				ret = npd_enable_cscd_port();
				if(ret != RET_SUCCESS)
				{
					npd_syslog_err("enable cscd port for test failed \n");
					return RET_FAIL;
				}
			}
			else
			{
				ret = npd_disable_cscd_port();
				if(ret != RET_SUCCESS)
				{
					npd_syslog_err("disable cscd port for test failed \n");
					return RET_FAIL;
				}
			}
		}
	}
	else
	{
		Endis = endis?0:1;
		ret = nam_enable_device(devNum,Endis);
		if(ret != RET_SUCCESS)
		{
			npd_syslog_err("%s asic failed\n",Endis?"Enable":"Disable");
			return RET_FAIL;
		}
	}
	return RET_SUCCESS;
}


#ifdef __cplusplus
}
#endif 
