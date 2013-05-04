#ifndef __NAM_NPD_TEST_H__
#define __NAM_NPD_TEST_H__
int create_vlan(int devnum,int vid);
int phy_page_set(int board_type);
int force_to_xg_mode();
int force_port_to_xg_mode(int devNum,int portNum);
int	nam_change_mode_xaui_to_rxaui(int devnum, int portnum);
int nam_reconfigure_lionserdesconfig(int devnum, int portnum,int rxSerdesLaneArr[],int txSerdesLaneArr[]);

#endif
