#ifndef __NPD_BOARD_AX81_12X_H__
#define __NPD_BOARD_AX81_12X_H__

#define LION_DEFAULT_MTU 1548
int phy_88X2140_config(int devNum, int portNum);
extern int npd_init_ax81_12x(void);
extern asic_board_t ax81_12x_board_t; 
#endif
