#ifndef NPD_TEST
#define NPD_TEST 1


#define AX81_CRSMU_BOARD_CODE	 0x0
#define AX81_AC12C_BOARD_CODE 	 0x1
#define AX81_AC8C_BOARD_CODE 	 0x2
#define AX81_2X12G12S_BOARD_CODE 0x4
#define AX81_1X12G12S_BOARD_CODE 0x5
#define AX81_12X_BOARD_CODE	 0x3
#define AX81_AC4X_BOARD_CODE	 0x6
#define AX81_CRSMUE_BOARD_CODE 0x7



#define SEM_SLOT_COUNT_PATH         "/dbm/product/slotcount"

#define AX8606_SLOT_HALF 2
#define AX8610_SLOT_HALF 4
#define AX8800_SLOT_HALF 6


/*
typedef struct asic_cscd_bport_s{
    char cscd_port;
	char master;
	char asic_id;
	char bport;
	char trunk_id;
}asic_cscd_bport_t;

typedef struct asic_board_cscd_bport_s {
    char board_type;
	char slot_id;
	char slot_num;
	char asic_cscd_port_cnt;
	char asic_to_cpu_ports;
	asic_cscd_bport_t asic_cscd_bports[16];
}asic_board_cscd_bport_t;

extern asic_board_cscd_bport_t * g_bportlist;
*/



#if 0

typedef enum {
    PRODUCT_TYPE_8610 = 0,
    PRODUCT_TYPE_8603 = 1,
    PRODUCT_TYPE_8800 = 2,
    PRODUCT_TYPE_MAX
}

		

typedef enum {
    BOARD_TYPE_AX81_SMU = 0,
    BOARD_TYPE_AX81_AC12C = 1,   
    BOARD_TYPE_AX81_AC8C = 2,   
    BOARD_TYPE_AX81_12X = 3,   
    BOARD_TYPE_AX71_2X12G12S = 4, /*include AX81_2X12G12S and AX71_2X12G12S*/
    BOARD_TYPE_AX81_1X12G12S = 5,
    BOARD_TYPE_AX81_AC4X = 6,
    BOARD_TYPE_AX71_CRSMU = 0x80,
    BOARD_TYPE_AXXX_VIRTUALBOARD=0x81,
    BOARD_TYPE_MAX
}

#endif



#endif
