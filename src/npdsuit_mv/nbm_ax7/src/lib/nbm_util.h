#ifndef __NBM_UTIL_H__
#define __NBM_UTIL_H__

/*
NOTICE
	These cpu arch, mainboard, backplane ids are very hardware related,
	they will be converted to product/module ids which are defined in npd_sysdef.h

*/

enum cpu_arch_id_e {
	CPU_ARCH_ID_OCTEON,
	CPU_ARCH_ID_MIPS,
	CPU_ARCH_ID_ARM,
	CPU_ARCH_ID_PPC,
	CPU_ARCH_ID_UNKNOW
};

/*
  Each cpu arch will have a mainboard type id definition, range from 0 to 255.
  It could be the mainboard of box product, or mainboard of master or slave cards of a chassis product.
*/
enum octeon_mainboard_id_e {
	OCTEON_MAINBOARD_ID_AX7_CRSMU,       /* 0*/
	OCTEON_MAINBOARD_ID_AX5K,
	OCTEON_MAINBOARD_ID_AU4K,
	OCTEON_MAINBOARD_ID_AU3K,
	OCTEON_MAINBOARD_ID_AX2K,
	XCAT_MAINBOARD_ID_AU1K, /* for BobCat soc */
	OCTEON_MAINBOARD_ID_AX8600,	
	OCTEON_MAINBOARD_ID_UNKNOW     /* MAX value*/
};


/*
    If one mainboard id is a module of chasssis product,
    it should have a list of backplane on which it could work.
*/

enum ax7_backplane_id_e {
	BACKPLANE_TYPE_NONE, 				/* could be used for box product*/
	BACKPLANE_TYPE_ID_AX7605_V1,        /* 0 */
	BACKPLANE_TYPE_ID_AX7605I,			/* 1 */
	BACKPLANE_TYPE_ID_AX8610,
	BACKPLANE_TYPE_ID_UNKNOW      /* max value*/
};


enum ax7_iocard_type_id_e {
	CARD_TYPE_ID_AX7_6GTX,        /* 0*/
	CARD_TYPE_ID_UNKNOW     /* MAX value*/
};

/**********************************************************************************
 * nbm_init_system_localboard_slotno
 * 
 * 	This function initialize local board chassis slot number,
 *	Currently only product PRODUCT_ID_AX7K_I need this operation.
 *
 *	INPUT:
 *		product_id - product id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred.
 *		NPD_FAIL	 - if error occurred.
 *		
 **********************************************************************************/
int nbm_init_system_localboard_slotno
(
	enum product_id_e product_id
);
#endif
