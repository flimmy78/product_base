/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortCtrl.c
*
* DESCRIPTION:
*       CPSS implementation for Port configuration and control facility.
*
*       The following APIs can run now "preliminary stage" and "post stage" callbacks
*       if the bind with port MAC object pointer will be run:
*
*        - cpssDxChPortSpeedSet;                        
*        - cpssDxChPortSpeedGet;                        
*        - cpssDxChPortDuplexAutoNegEnableSet;               
*        - cpssDxChPortDuplexAutoNegEnableGet;           
*        - cpssDxChPortFlowCntrlAutoNegEnableSet;       
*        - cpssDxChPortFlowCntrlAutoNegEnableGet;      
*        - cpssDxChPortSpeedAutoNegEnableSet;               
*        - cpssDxChPortSpeedAutoNegEnableGet;          
*        - cpssDxChPortFlowControlEnableSet;            
*        - cpssDxChPortFlowControlEnableGet;           
*        - cpssDxChPortPeriodicFcEnableSet;             
*        - cpssDxChPortPeriodicFcEnableGet;            
*        - cpssDxChPortBackPressureEnableSet;             
*        - cpssDxChPortBackPressureEnableGet;           
*        - cpssDxChPortLinkStatusGet;                   
*        - cpssDxChPortDuplexModeSet;                 
*        - cpssDxChPortDuplexModeGet;                   
*        - cpssDxChPortEnableSet;                       
*        - cpssDxChPortEnableGet;                      
*        - cpssDxChPortExcessiveCollisionDropEnableSet; 
*        - cpssDxChPortExcessiveCollisionDropEnableGet; 
*        - cpssDxChPortPaddingEnableSet;                
*        - cpssDxChPortPaddingEnableGet;               
*        - cpssDxChPortPreambleLengthSet;                 
*        - cpssDxChPortPreambleLengthGet;               
*        - cpssDxChPortCrcCheckEnableSet;              
*        - cpssDxChPortCrcCheckEnableGet;             
*        - cpssDxChPortMruSet;                          
*        - cpssDxChPortMruGet;                         
*
* FILE REVISION NUMBER:
*       $Revision: 1.2 $
*******************************************************************************/

/* macro needed to support the call to PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC */
/* this define must come before include files */
#define PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_SUPPORTED_FLAG_CNS

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPort.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>

#include <cpss/generic/extMac/cpssExtMacDrv.h> 
#include <sysdef/npd_sysdef.h>

#ifndef __AX_PLATFORM__
/* add by jinpc@autelan.com */
extern unsigned int auteware_product_id;

/* this MACRO is for AX7K product */
#define AX7_MAX_XG_PORT_NUMBER			4			/* number of xg-ports	*/
#define AX7_MAX_XG_PORT_REG_NUMBER		1			/* number of registers	*/

static unsigned char prvCpssDxChXGPhySmiAX7[][3] = {
	/* devNum,  portNum,  phyId*/
	{0, 0x18, 0x2},
	{0, 0x19, 0x0},
	{1, 0x18, 0x0},
	{1, 0x19, 0x2},
};
static unsigned short prvCpssDxChXGPhySmiRegAX7[][3] = {
	/* phyReg, phyDev,  regValue */
	{0x8304, 0x3   , 0x4455},
};

/* this MACRO is for AX5612 product */
#define AX5_MAX_XG_PORT_NUMBER			1			/* number of xg-ports	*/
#define AX5_MAX_XG_PORT_REG_NUMBER		2			/* number of registers	*/

static unsigned char prvCpssDxChXGPhySmiAX5[][3] = {
	/* devNum,  portNum,  phyId*/
	{0, 0x18, 0X0},
	{0, 0x19, 0X1},
};
static unsigned short prvCpssDxChXGPhySmiRegAX5[][3] = {
	/* phyReg, phyDev,  regValue */
	{0xD006, 0x1   , 0x9   },
	{0xD008, 0x1   , 0xa   },
};

/* this MACRO is for AX5612i product */
#define AX5I_MAX_XG_PORT_NUMBER			1			/* number of xg-ports	*/
#define AX5I_MAX_XG_PORT_REG_NUMBER		2			/* number of registers	*/

static unsigned char prvCpssDxChXGPhySmiAX5I[][3] = {
	/* devNum,  portNum,  phyId*/
	{0, 0x19, 0X0},
};
static unsigned short prvCpssDxChXGPhySmiRegAX5I[][3] = {
	/* phyReg, phyDev,  regValue */
	{0xD006, 0x1   , 0x9   },
	{0xD008, 0x1   , 0xa   },
};

/* this MACRO is for AU4K product */
#define AU4_MAX_XG_PORT_NUMBER			2			/* number of xg-ports	*/
#define AU4_MAX_XG_PORT_REG_NUMBER		2			/* number of registers	*/

static unsigned char prvCpssDxChXGPhySmiAU4[][3] = {
	/* devNum,  portNum,  phyId*/
	{0, 0x18, 0x0},
	{0, 0x19, 0x1},
};
static unsigned short prvCpssDxChXGPhySmiRegAU4[][3] = {
	/* phyReg, phyDev,  regValue */
	{0xD006, 0x1   , 0x9   },
	{0xD008, 0x1   , 0xa   },
};

/* this MACRO is for AU3K product */
#define AU3_MAX_XG_PORT_NUMBER			2			/* number of xg-ports	*/
#define AU3_MAX_XG_PORT_REG_NUMBER		2			/* number of registers	*/

static unsigned char prvCpssDxChXGPhySmiAU3[][3] = {
	/* devNum, portNum, phyId*/
	{0, 0x18, 0x0},
	{0, 0x19, 0x1},
};
static unsigned short prvCpssDxChXGPhySmiRegAU3[][3] = {
	/* phyReg, phyDev, regValue */
	{0xD006, 0x1   , 0x9   },
	{0xD008, 0x1   , 0xa   },
};
#endif


static GT_STATUS prvPortEnableSet(GT_U8 devNum, GT_U8 portNum,
                                  GT_U32 portState);
static GT_STATUS prvPortDisableSet(GT_U8 devNum, GT_U8 portNum,
                                   GT_U32* portState);
static GT_STATUS getXgPortSpeed(GT_U8 dev, GT_U8 port,
                                CPSS_PORT_SPEED_ENT *speed);
static GT_STATUS setXgPortSpeed(GT_U8 dev, GT_U8 port,
                                CPSS_PORT_SPEED_ENT speed);
static GT_STATUS setSgmiiPortSpeed(GT_U8 devNum,GT_U8 portNum,
                                   CPSS_PORT_SPEED_ENT speed);
static GT_BOOL isPortInXgMode(GT_U8 dev, GT_U8 port);

static GT_STATUS prvCpssDxChPortGigInterfaceModeSet(GT_U8 devNum, GT_U8 portNum,
                                                    CPSS_PORT_INTERFACE_MODE_ENT ifMode);

static GT_STATUS prvCpssDxCh3PortSerdesPowerUpBitMapGet(GT_U8 devNum,
                                                        GT_U32 *networkSerdesPowerUpBmpPtr);

static GT_STATUS setFlexLinkPortSpeed(GT_U8 dev,GT_U8 port,CPSS_PORT_SPEED_ENT speed);
static GT_STATUS setFlexLinkPortSpeedLpSerdes(GT_U8 dev,GT_U8 port,CPSS_PORT_SPEED_ENT speed);

static GT_STATUS prvCpssDxCh3PortGroupSerdesPowerStatusSet(GT_U8 devNum,
                                                           GT_U32 portSerdesGroup,
                                                           GT_BOOL    powerUp);
static GT_STATUS prvCpssDxChLpCheckSerdesInitStatus(GT_U8   devNum,
                                                    GT_U32  portGroupId,
                                                    GT_U32  serdesNum);
static GT_STATUS prvCpssDxChLpGetFirstInitSerdes(GT_U8  devNum,
                                                 GT_U32 portGroupId,
                                                 GT_U32 startSerdes,
                                                 GT_U32 *initSerdesNumPtr);

static GT_STATUS prvCpssDxChPortSerdesResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U32    startSerdes,
    IN  GT_U32    serdesesNum,
    IN  GT_BOOL   state
);

static GT_STATUS setNetworkPortSpeedLpSerdes(IN GT_U8 devNum, IN GT_U8 portNum,
                                             IN CPSS_PORT_SPEED_ENT speed);

static GT_STATUS prvGetLpSerdesSpeed
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               serdesNum,
    OUT PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT  *serdesSpeedPtr
);

static GT_STATUS prvCpssDxChPortLionSerdesSpeedSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    IN  PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed
);

/* macro for HW wait time for configuration */
#ifndef ASIC_SIMULATION
    #define HW_WAIT_MILLISECONDS_MAC(miliSec) cpssOsTimerWkAfter(miliSec)
#else /*ASIC_SIMULATION*/
/* the simulation NOT need those 'sleeps' needed in HW */
    #define HW_WAIT_MILLISECONDS_MAC(miliSec)
#endif /*ASIC_SIMULATION*/


/* #define SERDES_CFG_DBG */
#ifdef SERDES_CFG_DBG
#define SERDES_DBG_DUMP_MAC(x)     cpssOsPrintf x
#else
#define SERDES_DBG_DUMP_MAC(x)
#endif

/* #define AUTODETECT_DBG */
#ifdef AUTODETECT_DBG
#define AUTODETECT_DBG_PRINT_MAC(x)     cpssOsPrintf x
#else
#define AUTODETECT_DBG_PRINT_MAC(x)
#endif

/* matrix [register][PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT] = reg value */
/* Values for Short Reach 125MHz SerDes RefClk */
GT_U32 lpSerdesConfig[PRV_CPSS_DXCH_PORT_SERDES_REG_NUM_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E] =
/* Note: 5G used by network ports in QSGMII mode */
/* 1.25G  3.125G  3.75G   6.25G     5G    4.25G    2.5      reserved */
{
 {0x800A, 0x800A, 0x800A, 0x800A, 0x800A, 0x800A, 0x800A, 0x0000 }, /* PLL/INTP Register1 */
 {0x6614, 0x5515, 0x4413, 0x4415, 0x4414, 0x4413, 0x5514, 0x0000 }, /* PLL/INTP Register2 */
 {0x8150, 0x8164, 0x813c, 0x8164, 0x8150, 0x8144, 0x8150, 0x0000 }, /* PLL/INTP Register3 */
 {0xA150, 0xA164, 0xA13c, 0xA164, 0xA150, 0xA144, 0xA150, 0x0000 }, /* PLL/INTP Register3 */
 {0xBAAB, 0xFBAA, 0x99AC, 0xFBAA, 0xBAAB, 0x99AC, 0xBAAB, 0x0000 }, /* PLL/INTP Register4 */
 {0x8B2C, 0x8720, 0x872c, 0x8720, 0x882c, 0x872C, 0x882c, 0x0000 }, /* PLL/INTP Register5 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Analog Reserved Register */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register 1*/
 {0xc011, 0xc011, 0xc011, 0xc011, 0xC011, 0xc011, 0xc011, 0x0000 }, /* Calibration Register 2*/
 {0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x0000 }, /* Calibration Register 3*/
 {0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0000 }, /* Calibration Register 5*/
 {0x8060, 0x8060, 0xA060, 0xA060, 0xA060, 0xA060, 0x8060, 0x0000 }, /* Transmitter Register0 */
 {0x8060, 0x8060, 0xA060, 0xA060, 0xA060, 0xA060, 0x8060, 0x0000 }, /* Transmitter Register0 duplicated for Lion compatibility */
 {0x7704, 0x7704, 0x7704, 0x7704, 0x7704, 0x7704, 0x7704, 0x0000 }, /* Transmitter Register1 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Transmitter Register2 */
 {0x905A, 0x905A, 0x905A, 0x90AA, 0x90AA, 0x905A, 0x905A, 0x0000 }, /* Reciever Register1 */
 {0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0000 }, /* Reciever Register2 */
 {0x0243, 0x0243, 0x0243, 0x0243, 0x0243, 0x0243, 0x0243, 0x0000 }, /* FFE Register0 */
 {0x423F, 0x423F, 0x403F, 0x423F, 0x423F, 0x423F, 0x403F, 0x0000 }, /* SLC Register       */
 {0x5554, 0x5554, 0x5554, 0x5554, 0x5554, 0x5554, 0x5554, 0x0000 }, /* Reference Register1 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Reset Register0 */
 {0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x0000 }, /* Calibration Register3*/
 {0xE0C0, 0xE0C0, 0xE0C0, 0xE0C0, 0xE0C0, 0xE0C0, 0xE0C0, 0x0000 }, /* Calibration Register7*/
 {0x8040, 0x8040, 0x8040, 0x8040, 0x8040, 0x8040, 0x8040, 0x0000 }, /* Reset Register0 */
 {0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x0000 }, /* Reset Register0 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register0 */
 {0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x0000 }, /* Calibration Register0 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }  /* Calibration Register0 */
};

/* xCat2 LP SerDes Power Up Sequence for Short Reach RefClock =125.0MHz */
GT_U32 lpSerdesConfig_xCat2[PRV_CPSS_DXCH_PORT_SERDES_REG_NUM_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E] =
/* Note: 5G used by network ports in QSGMII mode */
/* 1.25G  3.125G  reserved reserved  5G  reserved reserved reserved */
{
 {0x800A, 0x800A, 0x0000, 0x0000, 0x800A, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register1 */
 {0x6614, 0x5515, 0x0000, 0x0000, 0x4414, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register2 */
 {0xA150, 0xA164, 0x0000, 0x0000, 0xA150, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register3 */
 {0xA150, 0xA164, 0x0000, 0x0000, 0xA150, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register3 duplicated for compatibility */
 {0xBAAB, 0xFBAA, 0x0000, 0x0000, 0xBAAB, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register4 */
 {0x8B2C, 0x8720, 0x0000, 0x0000, 0x882c, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register5 */
 {0x2000, 0x2000, 0x0000, 0x0000, 0x2000, 0x0000, 0x0000, 0x0000 }, /* Analog Reserved Register */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register 1*/
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register 2*/
 {0x4000, 0x4000, 0x0000, 0x0000, 0x4000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register 3*/
 {0x0018, 0x0018, 0x0000, 0x0000, 0x0018, 0x0000, 0x0000, 0x0000 }, /* Calibration Register 5*/
 {0x80C0, 0x80C0, 0x0000, 0x0000, 0xA0C0, 0x0000, 0x0000, 0x0000 }, /* Transmitter Register0 */
 {0x8060, 0x8060, 0x0000, 0x0000, 0xA060, 0x0000, 0x0000, 0x0000 }, /* Transmitter Register0 */
 {0x7F2D, 0x7F2D, 0x0000, 0x0000, 0x750C, 0x0000, 0x0000, 0x0000 }, /* Transmitter Register1 */
 {0x0900, 0x0900, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Transmitter Register2 */
 {0x905A, 0x905A, 0x0000, 0x0000, 0x90AA, 0x0000, 0x0000, 0x0000 }, /* Reciever Register1 */
 {0x0800, 0x0800, 0x0000, 0x0000, 0x0800, 0x0000, 0x0000, 0x0000 }, /* Reciever Register2 */
 {0x037F, 0x037F, 0x0000, 0x0000, 0x0371, 0x0000, 0x0000, 0x0000 }, /* FFE Register0 */
 {0x423F, 0x423F, 0x0000, 0x0000, 0x423F, 0x0000, 0x0000, 0x0000 }, /* SLC Register       */
 {0x5555, 0x5555, 0x0000, 0x0000, 0x5555, 0x0000, 0x0000, 0x0000 }, /* Reference Register1 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Reset Register0 */
 {0x4000, 0x4000, 0x0000, 0x0000, 0x4000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register3*/
 {0xE0C0, 0xE0C0, 0x0000, 0x0000, 0xE0C0, 0x0000, 0x0000, 0x0000 }, /* Calibration Register7*/
 {0x8040, 0x8040, 0x0000, 0x0000, 0x8040, 0x0000, 0x0000, 0x0000 }, /* Reset Register0 */
 {0x8000, 0x8000, 0x0000, 0x0000, 0x8000, 0x0000, 0x0000, 0x0000 }, /* Reset Register0 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register0 */
 {0x8000, 0x8000, 0x0000, 0x0000, 0x8000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register0 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }  /* Calibration Register0 */
};

/* Values for 156.25MHz SerDes RefClk */
GT_U32 lpSerdesConfig_Lion_156Clk[PRV_CPSS_DXCH_PORT_SERDES_REG_NUM_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E] =
/* 1.25G  3.125G reserved 6.25G  reserved reserved reserved */
{
 {0x800A, 0x800A, 0x0000, 0x800A, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register1 */         /* 0  */
 {0x6614, 0x5515, 0x0000, 0x4415, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register2 */         /* 1  */
 {0x8140, 0x8150, 0x0000, 0x8150, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register3 */         /* 2  */
 {0xA140, 0xA150, 0x0000, 0xA150, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register3 */         /* 3  */
 {0xBAAB, 0xFBAA, 0x0000, 0xFBAA, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register4 */         /* 4  */
 {0x8B2C, 0x8720, 0x0000, 0x8720, 0x0000, 0x0000, 0x0000 }, /* PLL/INTP Register5 */         /* 5  */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Analog Reserved Register */   /* 6  */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register 1*/      /* 7  */
 {0xC011, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register 2*/      /* 8  */
 {0x4000, 0x4000, 0x0000, 0x4000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register 3*/      /* 9  */
 {0x0018, 0x0018, 0x0000, 0x0018, 0x0000, 0x0000, 0x0000 }, /* Calibration Register 5*/      /* 10 */
 {0x8060, 0x8060, 0x0000, 0xA060, 0x0000, 0x0000, 0x0000 }, /* Transmitter Register0 */      /* 11 */
 {0x8060, 0x8060, 0x0000, 0xA060, 0x0000, 0x0000, 0x0000 }, /* Transmitter Register0 */      /* 12 duplicated for Lion compatibility */
 {0x770A, 0x3A09, 0x0000, 0x7F2D, 0x0000, 0x0000, 0x0000 }, /* Transmitter Register1 */      /* 13 */
 {0x0000, 0x0000, 0x0000, 0x0100, 0x0000, 0x0000, 0x0000 }, /* Transmitter Register2 */      /* 14 */
 {0x905A, 0x905A, 0x0000, 0x90AA, 0x0000, 0x0000, 0x0000 }, /* Reciever Register1 */         /* 15 */
 {0x0800, 0x0800, 0x0000, 0x0800, 0x0000, 0x0000, 0x0000 }, /* Reciever Register2 */         /* 16 */
 {0x0336, 0x0336, 0x0000, 0x036F, 0x0000, 0x0000, 0x0000 }, /* FFE Register0 */              /* 17 */
 {0x423F, 0x423F, 0x0000, 0x423F, 0x0000, 0x0000, 0x0000 }, /* SLC Register       */         /* 18 */
 {0x5554, 0x5554, 0x0000, 0x5554, 0x0000, 0x0000, 0x0000 }, /* Reference Register1 */        /* 19 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Reset Register0 */            /* 20 */
 {0x4000, 0x4000, 0x0000, 0x4000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register3*/       /* 21 */
 {0xE0C0, 0xE0C0, 0x0000, 0xE0C0, 0x0000, 0x0000, 0x0000 }, /* Calibration Register7*/       /* 22 */
 {0x8040, 0x8040, 0x0000, 0x8040, 0x0000, 0x0000, 0x0000 }, /* Reset Register0 */            /* 23 */
 {0x8000, 0x8000, 0x0000, 0x8000, 0x0000, 0x0000, 0x0000 }, /* Reset Register0 */            /* 24 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register0 */      /* 25 */
 {0x8000, 0x8000, 0x0000, 0x8000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register0 */      /* 26 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }  /* Calibration Register0 */      /* 27 */
};                                                                                  /* 28 */

/* Values for Lion B0 156.25MHz SerDes RefClk Long Reach */
GT_U32 lpSerdesConfig_Lion_B0_156Clk_LR[PRV_CPSS_DXCH_PORT_SERDES_REG_NUM_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E] =
/* 1.25G  3.125G reserved 6.25G    5G   reserved reserved 5.156G */
{
 {0x800A, 0x800A, 0x0000, 0x800A, 0x800A, 0x0000, 0x0000, 0x800A }, /* PLL/INTP Register1 */         /* 0  */
 {0x6614, 0x5515, 0x0000, 0x4415, 0x4414, 0x0000, 0x0000, 0x4414 }, /* PLL/INTP Register2 */         /* 1  */
 {0xA140, 0xA150, 0x0000, 0xA150, 0xA140, 0x0000, 0x0000, 0xA142 }, /* PLL/INTP Register3 */         /* 2 for backword compatibility */
 {0xA140, 0xA150, 0x0000, 0xA150, 0xA140, 0x0000, 0x0000, 0xA142 }, /* PLL/INTP Register3 */         /* 3  */
 {0xBAAB, 0xFBAA, 0x0000, 0xFBAA, 0xBAAB, 0x0000, 0x0000, 0xBAAB }, /* PLL/INTP Register4 */         /* 4  */
 {0x8B2C, 0x8720, 0x0000, 0x8720, 0x882C, 0x0000, 0x0000, 0x882C }, /* PLL/INTP Register5 */         /* 5  */
 {0x2000, 0x2000, 0x0000, 0x2000, 0x2000, 0x0000, 0x0000, 0x2000 }, /* Analog Reserved Register */   /* 6  */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register 1*/      /* 7  */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register 2*/      /* 8  */
 {0x4000, 0x4000, 0x0000, 0x4000, 0x4000, 0x0000, 0x0000, 0x4000 }, /* Calibration Register 3*/      /* 9  */
 {0x0018, 0x0018, 0x0000, 0x0018, 0x0018, 0x0000, 0x0000, 0x0018 }, /* Calibration Register 5*/      /* 10 */
 {0x80C0, 0x80C0, 0x0000, 0xA0C0, 0xA0C0, 0x0000, 0x0000, 0xA0C0 }, /* Transmitter Register0 */      /* 11 */
 {0x8060, 0x8060, 0x0000, 0xA060, 0xA060, 0x0000, 0x0000, 0xA060 }, /* Transmitter Register0 */      /* 12 */
 {0x770A, 0x3A09, 0x0000, 0x7F2D, 0x7F2D, 0x0000, 0x0000, 0x7F2D }, /* Transmitter Register1 */      /* 13 */
 {0x0000, 0x0000, 0x0000, 0x0100, 0x0100, 0x0000, 0x0000, 0x0100 }, /* Transmitter Register2 */      /* 14 */
 {0x905A, 0x905A, 0x0000, 0x90AA, 0x90AA, 0x0000, 0x0000, 0x90AA }, /* Reciever Register1 */         /* 15 */
 {0x0800, 0x0800, 0x0000, 0x0800, 0x0800, 0x0000, 0x0000, 0x0800 }, /* Reciever Register2 */         /* 16 */
 {0x0336, 0x0336, 0x0000, 0x036F, 0x036F, 0x0000, 0x0000, 0x036F }, /* FFE Register0 */              /* 17 */
 {0x423F, 0x423F, 0x0000, 0x423F, 0x423F, 0x0000, 0x0000, 0x423F }, /* SLC Register       */         /* 18 */
 {0x5555, 0x5555, 0x0000, 0x5555, 0x5555, 0x0000, 0x0000, 0x5555 }, /* Reference Register1 */        /* 19 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Reset Register0 */            /* 20 */
 {0x4000, 0x4000, 0x0000, 0x4000, 0x4000, 0x0000, 0x0000, 0x4000 }, /* Calibration Register3*/       /* 21 */
 {0xE0C0, 0xE0C0, 0x0000, 0xE0C0, 0xE0C0, 0x0000, 0x0000, 0xE0C0 }, /* Calibration Register7*/       /* 22 */
 {0x8040, 0x8040, 0x0000, 0x8040, 0x8040, 0x0000, 0x0000, 0x8040 }, /* Reset Register0 */            /* 23 */
 {0x8000, 0x8000, 0x0000, 0x8000, 0x8000, 0x0000, 0x0000, 0x8000 }, /* Reset Register0 */            /* 24 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }, /* Calibration Register0 */      /* 25 */
 {0x8000, 0x8000, 0x0000, 0x8000, 0x8000, 0x0000, 0x0000, 0x8000 }, /* Calibration Register0 */      /* 26 */
 {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }  /* Calibration Register0 */      /* 27 */
};                                                                                  /* 28 */

/* matrix [register][] = reg value */
/* Values for 125MHz SerDes RefClk */
/* This matrix containes External Config Register values for
   preinit and postinit sequences. the left column values are intended for
   FLEX port in SGMII mode, the right column for all others. */
GT_U32 lpSerdesExtConfig[PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E][2] =
{
/* SGMII   other */
/*on Flex        */
/*   port        */
 {0x0000,  0x0000 }, /* External Config Register2 */
 {0xBE80,  0x3E80 }, /* External Config Register1 */
 {0x0008,  0x0008 }, /* External Config Register2 */
 {0x0018,  0x0018},  /* External Config Register2 */
 {0x0008,  0x0008},  /* External Config Register2 */
 {0x0028,  0x0028}   /* External Config Register2 */
};

GT_U32 lpSerdesExtConfig_xCat2_125Clk[PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E][2] =
{
/* 1.25G   other */
 {0x0000,  0x0000}, /* External Config Register2 */
 {0xBE80,  0x4E00}, /* External Config Register1 */
 {0x0008,  0x0008}, /* External Config Register2 */
 {0x0018,  0x0018}, /* External Config Register2 */
 {0x0008,  0x0008}, /* External Config Register2 */
 {0x0028,  0x0028}  /* External Config Register2 */
};

/* Values for 156.25MHz SerDes RefClk */
GT_U32 lpSerdesExtConfig_Lion_156Clk[PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E][2] =
{
/* 1.25G   other */
 {0x0000,  0x0000}, /* External Config Register2 */
 {0x4E00,  0x4E00}, /* External Config Register1 */
 {0x0008,  0x0008}, /* External Config Register2 */
 {0x0018,  0x0018}, /* External Config Register2 */
 {0x0008,  0x0008}, /* External Config Register2 */
 {0x0028,  0x0048}  /* External Config Register2 */
};

/* In this case first column 1.25G and 5.156; second - all other */
GT_U32 lpSerdesExtConfig_Lion_B0_156Clk_LR[PRV_CPSS_DXCH_PORT_SERDES_EXT_REG_NUM_E][2] =
{
 {0x0000,  0x0000}, /* External Config Register2 */
 {0xCE00,  0x4E00}, /* External Config Register1 */
 {0x0008,  0x0008}, /* External Config Register2 */
 {0x0018,  0x0018}, /* External Config Register2 */
 {0x0008,  0x0008}, /* External Config Register2 */
 {0x013F,  0x013F}  /* SERDES Miscellaneous */
};

static GT_STATUS prvCpssDxChXgmii20GDetect
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
);

static GT_STATUS prvCpssDxChXgmii10GDetect
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
);

static GT_STATUS prvCpssDxChRxauiDetect
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
);

static GT_STATUS prvCpssDxCh1000BaseXDetect
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
);

static GT_STATUS prvCpssDxChSGMII1GDetect
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
);

/* array of functions for specific port mode/speed test 
    NOTE !!! must be compatible with defaultPortModeSpeedOptionsArray
*/
static PRV_CPSS_PORT_MODE_SPEED_TEST_FUN portModeSpeedTestFunctionsArray[] = 
{
    prvCpssDxChXgmii20GDetect,
    prvCpssDxChXgmii10GDetect,
    prvCpssDxChRxauiDetect,
    prvCpssDxCh1000BaseXDetect,
    prvCpssDxChSGMII1GDetect
};

/*******************************************************************************
* serDesConfig
*
* DESCRIPTION:
*       returns two dimantional array values from device specific arrays
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       lpSerdesConfigArrPtr  - pointer to SERDES config array
*       lpSerdesExtConfigArrPtr - pointer to SERDES extended config array
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_SUPPORTED  - if serdes reference clock is not 156.25
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS serDesConfig
(
    IN  GT_U8 devNum,
    OUT GT_U32 (**lpSerdesConfigArrPtr)[PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E],
    OUT GT_U32 (**lpSerdesExtConfigArrPtr)[2]
)
{
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->port.serdesRefClock != CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E)
            return GT_NOT_SUPPORTED;
    }
    
    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_LION_E:
            *lpSerdesConfigArrPtr = lpSerdesConfig_Lion_B0_156Clk_LR;
            *lpSerdesExtConfigArrPtr = lpSerdesExtConfig_Lion_B0_156Clk_LR;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            *lpSerdesConfigArrPtr = lpSerdesConfig_xCat2;
            *lpSerdesExtConfigArrPtr = lpSerdesExtConfig_xCat2_125Clk;
            break;
        default:
            *lpSerdesConfigArrPtr = lpSerdesConfig;
            *lpSerdesExtConfigArrPtr = lpSerdesExtConfig;
    }

    return GT_OK;
}

/*******************************************************************************
* getNumberOfSerdesLanesForPortLocal
*
* DESCRIPTION:
*       Get number of first serdes and quantity of serdeses occupied by given
*       port in given interface mode
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - port media interface mode
*
* OUTPUTS:
*       startSerdesPtr      - first used serdes number
*       numOfSerdesLanesPtr - quantity of serdeses occupied
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - if port doesn't support given interface mode
*       GT_NOT_SUPPORTED         - wrong device family
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getNumberOfSerdesLanesForPortLocal
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    OUT GT_U8                           *startSerdesPtr,
    OUT GT_U8                           *numOfSerdesLanesPtr
)
{
    GT_U32 localPort;

    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {/* xCat2 stack ports have only one SERDES.
        xCat2 each 4 network ports use same SERDES. */
        *startSerdesPtr = (GT_U8)((portNum > 23) ? ((portNum - 24) + 6) : localPort / 4);
        *numOfSerdesLanesPtr = 1;
        return GT_OK;
    }
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {/* for xCat flex links, for xCat network ports see below */
        *startSerdesPtr = (GT_U8)((PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE) ?
                                (localPort%24)*4 + 6 : localPort/4);
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        *startSerdesPtr = (GT_U8)(localPort*2);
    }
    else
    {
        return GT_NOT_SUPPORTED;
    }

    switch(ifMode)
    {
        /* for modes using just part of lanes posessed by port must return all
         * lanes, because they may be swapped - resposibility of application
         * to know which lanes to use and provide laneBmp in cpssDxChPortSerdesPowerStatusSet
         */
        case CPSS_PORT_INTERFACE_MODE_QSGMII_E:
        case CPSS_PORT_INTERFACE_MODE_SGMII_E:
        case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
        case CPSS_PORT_INTERFACE_MODE_QX_E:
        case CPSS_PORT_INTERFACE_MODE_100BASE_FX_E:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
                if(PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
                    *numOfSerdesLanesPtr = 4;
                else
                    *numOfSerdesLanesPtr = 1;
            }
            else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
                *numOfSerdesLanesPtr = 1;
            else
                return GT_NOT_SUPPORTED;

            break;

        case CPSS_PORT_INTERFACE_MODE_HX_E:
        case CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E:
            *numOfSerdesLanesPtr = 2;
            break;
        case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
            /* RXAUI on FlexLink ports on XCAT uses serdes 0 and 2, thus we return all 4*/
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E &&
               PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
            {
                *numOfSerdesLanesPtr = 4;
            }
            else
            {
                *numOfSerdesLanesPtr = 2;
            }

            break;

        case CPSS_PORT_INTERFACE_MODE_XGMII_E: /* XAUI */
            *numOfSerdesLanesPtr = 4;
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {/* if neighbor port is in special mode - don't touch its serdeses or mac */
                if((localPort%2) == 0)
                {
                    if(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum+1) == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E)
                        *numOfSerdesLanesPtr = 2;
                }
            }

            break;

        case CPSS_PORT_INTERFACE_MODE_XLG_E: /* 40G */
            *numOfSerdesLanesPtr = 8;

            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {/* in Lion use serdes of ports 8-11, while configuring XLG on port 10 */
                *startSerdesPtr = 8*2;
            }
            else
                return GT_NOT_SUPPORTED;

            break;

        case CPSS_PORT_INTERFACE_MODE_NA_E:
            *numOfSerdesLanesPtr = 0;
            *startSerdesPtr = 0;
            break;

        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* getNumberOfSerdesLanesForPort
*
* DESCRIPTION:
*       Get number of first serdes and quantity of serdeses occupied by given
*       port in given interface mode
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - port media interface mode
*
* OUTPUTS:
*       startSerdesPtr         - first used serdes number
*       numOfSerdesLanesPtr    - quantity of serdeses occupied
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - if port doesn't support given interface mode
*       GT_NOT_SUPPORTED         - wrong device family
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getNumberOfSerdesLanesForPort
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    OUT GT_U8                           *startSerdesPtr,
    OUT GT_U8                           *numOfSerdesLanesPtr
)
{
    GT_U32 localPort;

    if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E) &&
       (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E)&& 
       (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E))
        return GT_NOT_SUPPORTED;

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
        if((ifMode == CPSS_PORT_INTERFACE_MODE_NA_E) ||
            ((ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (localPort%2 != 0)) ||
            ((ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E) && (localPort != 10)))
            return GT_NOT_SUPPORTED;
    }

    return getNumberOfSerdesLanesForPortLocal(devNum,portNum,ifMode,startSerdesPtr,numOfSerdesLanesPtr);
}
/*******************************************************************************
* prvCpssDxChPortNumberOfSerdesLanesGet
*
* DESCRIPTION:
*       Get number of first SERDES and quantity of SERDESes occupied by given
*       port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       firstSerdesPtr         - (pointer to) first used SERDES number
*       numOfSerdesLanesPtr    - (pointer to) quantity of SERDESes occupied
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_SUPPORTED         - wrong device family
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortNumberOfSerdesLanesGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT GT_U32                          *firstSerdesPtr,
    OUT GT_U32                          *numOfSerdesLanesPtr
)
{
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* port media interface mode */
    GT_STATUS                       rc ;    /* return code */
    GT_U8                           firstSerdes; /* first SERDES */
    GT_U8                           numOfSerdesLanes; /* number of SERDESes */

    /* get interface type */
    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* get first SERDES and number of SERDES lanes */
    rc = getNumberOfSerdesLanesForPort(devNum, portNum, ifMode,
                                       &firstSerdes,   &numOfSerdesLanes);
    if (rc != GT_OK)
    {
        return rc;
    }

    *firstSerdesPtr = firstSerdes;
    *numOfSerdesLanesPtr = numOfSerdesLanes;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacSaLsbSet
*
* DESCRIPTION:
*       Set the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       macSaLsb    - The ls byte of the MAC SA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       see srcAddrMiddle and srcAddrHigh as related parameters.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaLsbSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U8    macSaLsb
)
{
    GT_U32          regAddr;         /* register address */
    GT_U32          value;           /* value to write into the register */
    GT_U32          portState;       /* current port state (enable/disable) */
    GT_U32          fieldOffset;     /* start to write register at this bit */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    value = macSaLsb;

    /* disable port */
    if (prvPortDisableSet(devNum,portNum, &portState) != GT_OK)
        return GT_HW_ERROR;

#ifdef __AX_PLATFORM__
		for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
		{
#else
		portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
			if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
				continue;
#else
				return GT_OK;
#endif

  
        if (portMacType >= PRV_CPSS_PORT_XG_E) 
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
            fieldOffset = 0;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
            fieldOffset = 7;
        }

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        { 
            /* set new value */
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum,
                    PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                        regAddr, fieldOffset, 8, value) != GT_OK)
                return GT_HW_ERROR;
        }  
#ifdef __AX_PLATFORM__
    }
#endif
    /* write original port state */
    if (prvPortEnableSet(devNum, portNum, portState) != GT_OK)
        return GT_HW_ERROR;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacSaLsbGet
*
* DESCRIPTION:
*       Get the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       macSaLsbPtr - (pointer to) The ls byte of the MAC SA
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       see srcAddrMiddle and srcAddrHigh as related parameters.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaLsbGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U8    *macSaLsbPtr
)
{
    GT_U32          regAddr;         /* register address */
    GT_U32          value;
    GT_U32          fieldOffset;     /* start to write register at this bit */
    GT_STATUS       rc = GT_OK;
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(macSaLsbPtr);

   /* cpssDxChPortMacSaLsbSet updates all available MAC's - here enough to read */
    /* one of them */
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if (portMacType >= PRV_CPSS_PORT_XG_E) 
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
        fieldOffset = 0;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
        fieldOffset = 7;
    }

    rc =prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                    regAddr, fieldOffset, 8, &value);
    if(rc != GT_OK)
        return rc;

    *macSaLsbPtr = (GT_U8)value;

    return rc;
}

/*******************************************************************************
* prvCpssDxChPortEnableSet        
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable port,
*                  GT_FALSE: disable port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
{
    GT_U32 regAddr;
    GT_U32 value;
    
    value = (enable == GT_TRUE) ? 1 : 0;
    
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum, portNum,
                        PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum), &regAddr);

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, 
        PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
        regAddr, 0, 1, value);
}

/*******************************************************************************
* cpssDxChPortEnableSet
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable port,
*                  GT_FALSE: disable port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
{
   
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;

    GT_BOOL targetEnable = enable;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPortEnableSetFunc(devNum,portNum,
                                  targetEnable, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* switch MAC port enable configuration */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortEnableSet(devNum,portNum,targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPortEnableSetFunc(devNum,portNum,
                                  targetEnable, 
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortEnableGet         
*
* DESCRIPTION:
*       Get the Enable/Disable status of a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       statePtr - Pointer to the Get Enable/disable state of the port.
*                  GT_TRUE for enabled port, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *statePtr
)
{
    GT_U32 regAddr;
    GT_U32 value;

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

    /* store value of port state */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 0, 1, &value) != GT_OK)
        return GT_HW_ERROR;

    *statePtr = (value == 1) ? GT_TRUE : GT_FALSE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status of a specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       statePtr - Pointer to the Get Enable/disable state of the port.
*                  GT_TRUE for enabled port, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortEnableGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT  GT_BOOL   *statePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPortEnableGetFunc(devNum,portNum,
                                  statePtr, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get port enable status from switch MAC */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortEnableGet(devNum,portNum,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPortEnableGetFunc(devNum,portNum,
                                  statePtr, 
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortDuplexModeSet        
*
* DESCRIPTION:
*       Set the port to enable and the port mode to half- or full-duplex mode.
*       When port is disabled it does not transmit pending packets and
*       drops packets received from the line.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       dMode    - 0 = The port operates in half-duplex mode.
*                  1 = The port operates in full-duplex mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or dMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported duplex mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The <SetFullDx> bit is meaningless when <ANDuplex> is set.
*       For CPU port, this field must be set to reflect this port Duplex mode.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortDuplexModeSet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    IN  CPSS_PORT_DUPLEX_ENT  dMode
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 portState;       /* current port state (enable/disable) */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 duplexModeRegAddr;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC (devNum,portNum);
    if (dMode != CPSS_PORT_FULL_DUPLEX_E &&
        dMode != CPSS_PORT_HALF_DUPLEX_E)
    {
        return GT_BAD_PARAM;
    }

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
    /* can not be changed for 10 Gb and up interface */
    if((portNum != CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
    {
        if (dMode == CPSS_PORT_FULL_DUPLEX_E)
        {
            return GT_OK;
        }
        else
        {
            return GT_NOT_SUPPORTED;
        }
    }

    /* in devices of Lion family half-duplex not supported in any port mode/speed */
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) &&
        (dMode == CPSS_PORT_HALF_DUPLEX_E))
        return GT_NOT_SUPPORTED;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    value = (dMode == CPSS_PORT_FULL_DUPLEX_E) ? 1 : 0;
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);

    /* store value of port state */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &portState) != GT_OK)
        return GT_HW_ERROR;

    /* disable port if we need */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
            return GT_HW_ERROR;
    }

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portNum,&duplexModeRegAddr);

    offset = 12;

    /* set new duplex mode */
    if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                  duplexModeRegAddr,
                                  offset,
                                  1,
                                  value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* enable port if we need */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 1) != GT_OK)
            return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortDuplexModeSet
*
* DESCRIPTION:
*       Set the port to enable and the port mode to half- or full-duplex mode.
*       When port is disabled it does not transmit pending packets and
*       drops packets received from the line.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       dMode    - 0 = The port operates in half-duplex mode.
*                  1 = The port operates in full-duplex mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or dMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported duplex mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The <SetFullDx> bit is meaningless when <ANDuplex> is set.
*       For CPU port, this field must be set to reflect this port Duplex mode.
*
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexModeSet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    IN  CPSS_PORT_DUPLEX_ENT  dMode
)
{
    CPSS_PORT_DUPLEX_ENT  targetdMode = dMode;   /* mode value - set by callback */
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC (devNum,portNum);
    if (dMode != CPSS_PORT_FULL_DUPLEX_E &&
        dMode != CPSS_PORT_HALF_DUPLEX_E)
    {
        return GT_BAD_PARAM;
    }

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacDuplexSetFunc(devNum,portNum,
                                  targetdMode, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetdMode);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* switch MAC configuration */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortDuplexModeSet(devNum,portNum,targetdMode);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run */
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacDuplexSetFunc(devNum,portNum,
                                  targetdMode, 
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetdMode);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortDuplexModeGet     
*
* DESCRIPTION:
*       Gets duplex mode for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       dModePtr - duplex mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortDuplexModeGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_DUPLEX_ENT  *dModePtr
)
{
    GT_U32 value;
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 duplexModeRegAddr;
    GT_STATUS rc;
    CPSS_PORT_SPEED_ENT speed;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(dModePtr);

     /* can not be changed for 1G and above */
    if(portNum != CPSS_CPU_PORT_NUM_CNS)
    {
        if((rc = cpssDxChPortSpeedGet(devNum, portNum, &speed)) != GT_OK)
            return rc;

        if(speed >= CPSS_PORT_SPEED_1000_E)
        {
            *dModePtr = CPSS_PORT_FULL_DUPLEX_E;
            return GT_OK;
        }
    }

    PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portNum,&duplexModeRegAddr);

    offset = 3;

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                                  duplexModeRegAddr,
                                  offset,
                                  1,
                                  &value) != GT_OK)
        return GT_HW_ERROR;

    *dModePtr = (value == 1) ? CPSS_PORT_FULL_DUPLEX_E : CPSS_PORT_HALF_DUPLEX_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortDuplexModeGet
*
* DESCRIPTION:
*       Gets duplex mode for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       dModePtr - duplex mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexModeGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_DUPLEX_ENT  *dModePtr
)
{
    GT_STATUS rc = GT_OK;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(dModePtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacDuplexGetFunc(devNum,portNum,
                                  dModePtr, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,dModePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get value from switch MAC */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortDuplexModeGet(devNum,portNum,dModePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacDuplexGetFunc(devNum,portNum,
                                  dModePtr, 
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,dModePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxCh3SerdesSpeedModeGet
*
* DESCRIPTION:
*       Gets speed mode for specified port serdes on specified device.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       serdesSpeed_2500_EnPtr - pointer to serdes speed mode.
*                                GT_TRUE  - serdes speed is 2.5 gig.
*                                GT_FALSE - other serdes speed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3SerdesSpeedModeGet
(
    IN  GT_U8          devNum,
    IN  GT_U8          portNum,
    OUT GT_BOOL        *serdesSpeed_2500_EnPtr
)
{
    GT_U32                regAddr;       /* register address */
    GT_U32                regValue;      /* register value */

    /* Get SERDES speed configuration */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesSpeed1[0];

    if (prvCpssDrvHwPpPortGroupReadRegister(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, &regValue) != GT_OK)
    {
        return GT_HW_ERROR;
    }


    if(regValue == PRV_SERDES_SPEED_2500_REG_VAL_CNS)
    {
        *serdesSpeed_2500_EnPtr = GT_TRUE;
    }
    else
    {
        *serdesSpeed_2500_EnPtr = GT_FALSE;
    }

    return GT_OK;

}
/*******************************************************************************
* prvCpssDxCh3PortSerdesSet
*
* DESCRIPTION:
*       Sets SERDES speed configurations for specified port on specified device.
*       The ports that are members of same SERDES group MUST work with same speed.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3PortSerdesSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_U32                regAddr;       /* register address */
    GT_U32                regValue;      /* register value */
    CPSS_PORT_SPEED_ENT   existSpeed;    /* existed speed */
    GT_BOOL               oldPowerUp;    /* serdes lanes old power up state */
    GT_U32                serdesGroup;   /* SERDES group */
    GT_U8                 checkPortNum;  /* checkd port in serdes group */
    GT_BOOL         serdesSpeed_2500_En; /* GT_TRUE  - serdes speed is 2.5 gig.
                                            GT_FALSE - other serdes speed */
    GT_U32                i;
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  portSerdesGroup;  /* port group number */
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    rc = cpssDxChPortSpeedGet(devNum, portNum, &existSpeed);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Set SERDES when the port speed was changed from 2.5 gig or to 2.5 gig. */
    if((existSpeed == speed) ||
      ((speed != CPSS_PORT_SPEED_2500_E)&& (existSpeed != CPSS_PORT_SPEED_2500_E)))
    {
        return GT_OK;
    }

    serdesGroup = portNum / 4;
    for(i = 0; i < 4; i++)
    {
        checkPortNum = (GT_U8)(i + serdesGroup * 4);

        /* check speed setting for all group members those already
           "SERDES Powered UP" except of the port for setting */
        if(checkPortNum == portNum)
        {
            continue;
        }
       /* Get power up or down state to port and serdes */
        rc = prvCpssDxCh3PortSerdesPowerUpDownGet(devNum,
                                                  checkPortNum,
                                                  &oldPowerUp);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(oldPowerUp == GT_TRUE)
        {
            /* Get SERDES speed mode (2.5 gig / 1 gig ) */
            rc = prvCpssDxCh3SerdesSpeedModeGet(devNum, checkPortNum,
                                                &serdesSpeed_2500_En);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Speed of the group member is different from speed for setting */
            /* Members of same SERDES group MUST work with same speed.*/

            if(serdesSpeed_2500_En == GT_TRUE)
            {
                if(speed != CPSS_PORT_SPEED_2500_E)
                {
                    return GT_BAD_STATE;
                }
            }
            else
            {
                if(speed == CPSS_PORT_SPEED_2500_E)
                {
                    return GT_BAD_STATE;
                }

            }

        }
    }

    /* Configure serdes lanes power Down */
    rc = cpssDxChPortSerdesGroupGet(devNum, portNum, &portSerdesGroup);
    if(rc != GT_OK)
    {
        return rc;
    }
    rc = prvCpssDxCh3PortGroupSerdesPowerStatusSet(devNum, portSerdesGroup,
                                                   GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* NP ref clock configurations */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesSpeed1[0];

    if(speed == CPSS_PORT_SPEED_2500_E)
    {
        regValue = PRV_SERDES_SPEED_2500_REG_VAL_CNS;
    }
    else
    {
        regValue = PRV_SERDES_SPEED_1000_REG_VAL_CNS;
    }

    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Setting electrical parameters */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesSpeed2[0];

    if(speed == CPSS_PORT_SPEED_2500_E)
    {
        regValue = 0x000001F7;
    }
    else
    {
        regValue = 0x00000036;
    }

    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
            regAddr, regValue) != GT_OK)
    {
        return GT_HW_ERROR;
    }


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesReservConf[0];

    if(speed == CPSS_PORT_SPEED_2500_E)
    {
        regValue = 0x0000F0F0;
    }
    else
    {
        regValue = 0x0000F1F0;
    }

    /* bit 12 is for metal fix enabling */
    if (prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, 
                                               0xFFFFEFFF, regValue) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* NP SERDES electrical configurations */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesIvrefConf1[0];

    if(speed == CPSS_PORT_SPEED_2500_E)
    {
        regValue = 0x0000575A;
    }
    else
    {
        regValue = 0x00005555;
    }

    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue) != GT_OK)
    {
        return GT_HW_ERROR;
    }


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesIvrefConf2[0];

    if(speed == CPSS_PORT_SPEED_2500_E)
    {
        regValue = 0x00000003;
    }
    else
    {
        regValue = 0x00000001;
    }

    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue) != GT_OK)
    {
        return GT_HW_ERROR;
    }


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesMiscConf[0];

    if(speed == CPSS_PORT_SPEED_2500_E)
    {
        regValue = 0x00000008;
    }
    else
    {
        regValue = 0x00000009;
    }

    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    return GT_OK;

}

/*******************************************************************************
*   prvCpssDxChPortSpeedSet
* 
* 
* DESCRIPTION:
*       Sets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported speed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting.
*                                  (For DxCh3, not XG ports only.)
*
* COMMENTS:
*       1. If the port is enabled then the function disables the port before
*          the operation and re-enables it at the end.
*       2. For Flex-Link ports the interface mode should be
*          configured before port's speed, see cpssDxChPortInterfaceModeSet.
*          The following table summarizes all options:
*  Interface   |  Interface Mode                       |  Port Speed
*  ------------------------------------------------------------------------
*  1G SGMII    | CPSS_PORT_INTERFACE_MODE_SGMII_E      | CPSS_PORT_SPEED_1000_E
*  1000 BASE-X | CPSS_PORT_INTERFACE_MODE_1000BASE_X_E | CPSS_PORT_SPEED_1000_E
*  100 BAS-FX  | CPSS_PORT_INTERFACE_MODE_100BASE_FX_E | CPSS_PORT_SPEED_100_E
*  2.5G SGMII  | CPSS_PORT_INTERFACE_MODE_SGMII_E      | CPSS_PORT_SPEED_2500_E
*  QX          | CPSS_PORT_INTERFACE_MODE_QX_E         | CPSS_PORT_SPEED_2500_E
*  DQX         | CPSS_PORT_INTERFACE_MODE_QX_E         | CPSS_PORT_SPEED_5000_E
*  HX          | CPSS_PORT_INTERFACE_MODE_HX_E         | CPSS_PORT_SPEED_5000_E
*  DHX         | CPSS_PORT_INTERFACE_MODE_HX_E         | CPSS_PORT_SPEED_10000_E
*  XAUI        | CPSS_PORT_INTERFACE_MODE_XGMII_E      | CPSS_PORT_SPEED_10000_E
*  HGS         | CPSS_PORT_INTERFACE_MODE_XGMII_E      | CPSS_PORT_SPEED_12000_E
*  RXAUI       | CPSS_PORT_INTERFACE_MODE_RXAUI_E      | CPSS_PORT_SPEED_10000_E
*
*       3.This API also checks all SERDES per port initialization. If serdes was
*         not initialized, proper init will be done
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc = GT_OK;

    /* check input parameter */
    if (speed < CPSS_PORT_SPEED_10_E || speed >= CPSS_PORT_SPEED_NA_E)
    {
        return GT_BAD_PARAM;
    }

    if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
    {
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                if(((speed == CPSS_PORT_SPEED_40000_E) &&
                     PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_40G_MAC_NOT_SUPPORTED_WA_E))
                    || (((speed == CPSS_PORT_SPEED_20000_E) || (speed == CPSS_PORT_SPEED_16000_E)) &&
                     PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_20G_MAC_NOT_SUPPORTED_WA_E))
                    || (((speed <= CPSS_PORT_SPEED_1000_E) || (speed == CPSS_PORT_SPEED_2500_E)) &&
                     PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_1G_SPEED_NOT_SUPPORTED_WA_E)))
                {
                    return GT_NOT_SUPPORTED;
                }
            }

            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                if ((speed > CPSS_PORT_SPEED_1000_E) && (speed != CPSS_PORT_SPEED_2500_E))
                    return GT_NOT_SUPPORTED;
            }

            return setFlexLinkPortSpeedLpSerdes(devNum,portNum,speed);
        }
        else
        {
            return setFlexLinkPortSpeed(devNum,portNum,speed);
        }
    }

    switch (speed)
    {
        case CPSS_PORT_SPEED_10_E:
        case CPSS_PORT_SPEED_100_E:
        case CPSS_PORT_SPEED_1000_E:
            /* speed can not be changed for 10 Gb interface */
            if((portNum != CPSS_CPU_PORT_NUM_CNS) &&
               (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_XG_E))
                 return GT_NOT_SUPPORTED;

            if((PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E) &&
                (speed == CPSS_PORT_SPEED_1000_E))
                return GT_NOT_SUPPORTED;

            break;

        case CPSS_PORT_SPEED_2500_E:
            /* The speed 2.5 Gbps supported on DxCh3 not for XG ports */
            /* and supported on DxCh HX devices for XG ports */
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            {
                if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_XG_E)
                {
                    return GT_NOT_SUPPORTED;
                }
            }
            else
            {
                /* network ports of xcat only support sgmii 2.5G speed */
                if ((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E)
                    || (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E))
                {
                    return GT_NOT_SUPPORTED;
                }
            }

            break;

        case CPSS_PORT_SPEED_12000_E:
        case CPSS_PORT_SPEED_5000_E:
        case CPSS_PORT_SPEED_10000_E:
        case CPSS_PORT_SPEED_16000_E:
        case CPSS_PORT_SPEED_20000_E:
        case CPSS_PORT_SPEED_40000_E:
            /* The speed 12 Gbps isn't supported for Dx Cheetah 3 */
            /* but supported on DXCH xCat devices */
            if((speed == CPSS_PORT_SPEED_12000_E) &&
                (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E))
            {
                return GT_NOT_SUPPORTED;
            }

            /* supported for XG ports only */
            if((portNum == CPSS_CPU_PORT_NUM_CNS) ||
               (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) != PRV_CPSS_PORT_XG_E))
                return GT_NOT_SUPPORTED;
            break;

        default:
            return GT_BAD_PARAM;
    }

    if((portNum != CPSS_CPU_PORT_NUM_CNS) &&
       (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_XG_E))
    {
        return setXgPortSpeed (devNum, portNum, speed);
    }

    /* Set SERDES */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        if(portNum != CPSS_CPU_PORT_NUM_CNS)
        {
            rc = prvCpssDxCh3PortSerdesSet(devNum, portNum, speed);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return setSgmiiPortSpeed(devNum,portNum,speed);
}

/*******************************************************************************
* cpssDxChPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported speed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting.
*                                  (For DxCh3, not XG ports only.)
*
* COMMENTS:
*       1. If the port is enabled then the function disables the port before
*          the operation and re-enables it at the end.
*       2. For Flex-Link ports the interface mode should be
*          configured before port's speed, see cpssDxChPortInterfaceModeSet.
*          The following table summarizes all options:
*  Interface   |  Interface Mode                       |  Port Speed
*  ------------------------------------------------------------------------
*  1G SGMII    | CPSS_PORT_INTERFACE_MODE_SGMII_E      | CPSS_PORT_SPEED_1000_E
*  1000 BASE-X | CPSS_PORT_INTERFACE_MODE_1000BASE_X_E | CPSS_PORT_SPEED_1000_E
*  100 BAS-FX  | CPSS_PORT_INTERFACE_MODE_100BASE_FX_E | CPSS_PORT_SPEED_100_E
*  2.5G SGMII  | CPSS_PORT_INTERFACE_MODE_SGMII_E      | CPSS_PORT_SPEED_2500_E
*  QX          | CPSS_PORT_INTERFACE_MODE_QX_E         | CPSS_PORT_SPEED_2500_E
*  DQX         | CPSS_PORT_INTERFACE_MODE_QX_E         | CPSS_PORT_SPEED_5000_E
*  HX          | CPSS_PORT_INTERFACE_MODE_HX_E         | CPSS_PORT_SPEED_5000_E
*  DHX         | CPSS_PORT_INTERFACE_MODE_HX_E         | CPSS_PORT_SPEED_10000_E
*  XAUI        | CPSS_PORT_INTERFACE_MODE_XGMII_E      | CPSS_PORT_SPEED_10000_E
*  HGS         | CPSS_PORT_INTERFACE_MODE_XGMII_E      | CPSS_PORT_SPEED_12000_E
*  RXAUI       | CPSS_PORT_INTERFACE_MODE_RXAUI_E      | CPSS_PORT_SPEED_10000_E
*
*       3.This API also checks all SERDES per port initialization. If serdes was
*         not initialized, proper init will be done
*
*       4.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    CPSS_PORT_SPEED_ENT  origSpeed = speed;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port callback pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* run switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
         /* Get PHY MAC object ptr 
       if it is CPU port or the pointer is NULL (no bind) - return NULL */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* run callback*/
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacSpeedSetFunc(devNum,portNum,
                                  origSpeed,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&origSpeed);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* doPpMacConfig value defined by callback; if it is TRUE -
       run prvCpssDxChPortSpeedSet that means do all required 
       confiruration actions for switch MAC */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortSpeedSet(devNum,portNum,origSpeed);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* if required the post stage callback can run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacSpeedSetFunc(devNum,portNum,
                                       origSpeed,CPSS_MACDRV_STAGE_POST_E,
                                       &doPpMacConfig,&origSpeed);
        if(rc!=GT_OK)
            {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortSpeedGet     
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on no initialized SERDES per port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        This API also checks if at least one serdes per port was initialized.
*        In case there was no initialized SERDES per port GT_NOT_INITIALIZED is
*        returned.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_U32    value;
    GT_U32    regAddr;              /* register address */
    GT_BOOL   serdesSpeed_2500_En;  /* GT_TRUE  - serdes speed is 2.5 gig.
                                       GT_FALSE - other serdes speed */
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* XG/HX/Flex-Link port */
    if(((portNum != CPSS_CPU_PORT_NUM_CNS) &&
       (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)) ||
        PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
    {
        return getXgPortSpeed (devNum, portNum, speedPtr);
    }

    /* if we reached this point, then we are in 10/100/1000 interface */
    PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portNum,&regAddr);

    /* read GMII Speed */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 1, 1, &value) != GT_OK)
        return GT_HW_ERROR;

    /* Port speed 1 gig or 2.5 gig according to the value. */
    /* Determine speed according to SERDES settings for DxCh3 devices. */
    if (value == 1)
    {
        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
        {
            if(portNum == CPSS_CPU_PORT_NUM_CNS)
            {
                *speedPtr = CPSS_PORT_SPEED_1000_E;
                return GT_OK;
            }

            /* Get SERDES speed mode (2.5 gig / 1 gig ) */
            rc = prvCpssDxCh3SerdesSpeedModeGet(devNum, portNum,
                                                &serdesSpeed_2500_En);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(serdesSpeed_2500_En == GT_TRUE)
            {
                *speedPtr = CPSS_PORT_SPEED_2500_E;
            }
            else
            {
                *speedPtr = CPSS_PORT_SPEED_1000_E;
            }
        }
        else if ((PRV_CPSS_PP_MAC(devNum)->devFamily > CPSS_PP_FAMILY_CHEETAH3_E) &&
                 (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_GE_E))
        {
            GT_U32 localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
            GT_U32 startSerdes;
            PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed;

            if(portNum == CPSS_CPU_PORT_NUM_CNS)
            {
                *speedPtr = CPSS_PORT_SPEED_1000_E;
                return GT_OK;
            }

            startSerdes = localPort/4;
            rc = prvGetLpSerdesSpeed(devNum,portGroupId,startSerdes,&serdesSpeed);
            if (rc != GT_OK)
            {
                return rc;
            }
            /* can't determine serdes speed */
            if (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
            {
                return GT_FAIL;
            }
            else
            {
                switch(serdesSpeed)
                {
                    case PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E: /* sgmii */
                    case PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_E:    /* qsgmii */
                        *speedPtr = CPSS_PORT_SPEED_1000_E;
                    break;

                    case PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E: /* sgmii 2.5G */
                        *speedPtr = CPSS_PORT_SPEED_2500_E;
                    break;

                    default:
                        return GT_NOT_SUPPORTED;
                }
            }
        }
        else /* CH2 */
        {
            *speedPtr = CPSS_PORT_SPEED_1000_E;
        }
    }
    else
    {
        /* read MII Speed */
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 2, 1, &value) != GT_OK)
            return GT_HW_ERROR;

        if (value == 1)
        {
            *speedPtr = CPSS_PORT_SPEED_100_E;
        }
        else
        {
            *speedPtr = CPSS_PORT_SPEED_10_E;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on no initialized SERDES per port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        1.This API also checks if at least one serdes per port was initialized.
*        In case there was no initialized SERDES per port GT_NOT_INITIALIZED is
*        returned.
*
*       2.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_U8                 portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port callback pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(speedPtr);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);


    /* run MACPHY callback */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacSpeedGetFunc(devNum,portNum,
                                  speedPtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,speedPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* if callback set doPpMacConfig as TRUE - run prvCpssDxChPortSpeedGet - switch MAC configuration code */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortSpeedGet(devNum,portNum,speedPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* run "post stage" callback */
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacSpeedGetFunc(devNum,portNum,
                                  speedPtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,speedPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortDuplexAutoNegEnableSet     
*
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for duplex mode on specified port on
*       specified device.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the duplex auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortDuplexAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 portState;       /* current port state (enable/disable) */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 dupledAutoNegRegAddr;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    /* can not be changed for 10 Gb and up interface */
    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        /* for disable Auto-Negotiation for duplex mode */
        if (state == GT_FALSE)
        {
            return GT_OK;
        }
        else
        {
            return GT_NOT_SUPPORTED;
        }
    }

    /* in devices of Lion family half-duplex not supported in any port mode/speed,
        so disable duplex auto-neg. to prevent unpredictable behaviour */
    if ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) &&
        (state == GT_TRUE))
        return GT_NOT_SUPPORTED;

    value = (state == GT_TRUE) ? 1 : 0;

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);

    /* store value of port state */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &portState) != GT_OK)
        return GT_HW_ERROR;

    /* disable port if we need */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
            return GT_HW_ERROR;
    }

    dupledAutoNegRegAddr= PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                          perPortRegs[portNum].autoNegCtrl;
    offset= 13;

    /* set Auto-Negotiation for duplex mode */
    if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,dupledAutoNegRegAddr,
                                  offset,
                                  1,
                                  value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* enable port if we need */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 1) != GT_OK)
            return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortDuplexAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for duplex mode on specified port on
*       specified device.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.CPU port not supports the duplex auto negotiation
*
*       2.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;
    GT_BOOL targetPortState = state;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* run MACPHY callback  */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacDuplexANSetFunc(devNum,portNum,
                                  targetPortState,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetPortState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* if required run prvCpssDxChPortDuplexAutoNegEnableSet - 
       switch MAC configuration code */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortDuplexAutoNegEnableSet(devNum,portNum,targetPortState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

       /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacDuplexANSetFunc(devNum,portNum,
                                  targetPortState,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetPortState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortDuplexAutoNegEnableGet    
*
* DESCRIPTION:
*       Get enable/disable status of an Auto-Negotiation for MAC duplex mode
*       per port.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the duplex auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortDuplexAutoNegEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL    *statePtr
)
{
    GT_U32 value;
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 dupledAutoNegRegAddr;

    /* can not be changed for 10 Gb interface */
    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        *statePtr = GT_FALSE;
        return GT_OK;
    }
    dupledAutoNegRegAddr= PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                          perPortRegs[portNum].autoNegCtrl;

    offset= 13;

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            dupledAutoNegRegAddr, offset, 1,
                                  &value) != GT_OK)
        return GT_HW_ERROR;


    *statePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortDuplexAutoNegEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of an Auto-Negotiation for MAC duplex mode
*       per port.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. CPU port not supports the duplex auto negotiation
*
*       2. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortDuplexAutoNegEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL    *statePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port callback pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;                       /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* run MACPHY callback */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacDuplexANGetFunc(devNum,portNum,
                                  statePtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
           /*
     *  the function returns the switch MAC value if callback not bound or callback set 
     *  doPpMacConfig = GT_TRUE; in this case prvCpssDxChPortDuplexAutoNegEnableGet 
     *  will run  
     */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortDuplexAutoNegEnableGet(devNum,portNum,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* post callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacDuplexANGetFunc(devNum,portNum,
                                  statePtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortFlowCntrlAutoNegEnableSet    
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for Flow Control on
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*       state          - GT_TRUE for enable Auto-Negotiation for Flow Control
*                        GT_FALSE otherwise
*       pauseAdvertise - Advertise symmetric flow control support in
*                        Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the flow control auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortFlowCntrlAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state,
    IN  GT_BOOL   pauseAdvertise
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 stateValue;      /* value to write into the register */
    GT_U32 pauseAdvValue;   /* value to write into the register */
    GT_U32 portState;       /* current port state (enable/disable) */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 fcAutoNegRegAddr;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    /* can not be changed for 10 Gb interface */
    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        /* for disable Auto-Negotiation for duplex mode */
        if ((state == GT_FALSE) && (pauseAdvertise == GT_FALSE))
        {
            return GT_OK;
        }
        else
        {
            return GT_NOT_SUPPORTED;
        }
    }

    stateValue = (state == GT_TRUE) ? 1 : 0;
    pauseAdvValue = (pauseAdvertise == GT_TRUE) ? 1 : 0;
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);

    /* store value of port state */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &portState) != GT_OK)
        return GT_HW_ERROR;

    /* disable port if we need */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
            return GT_HW_ERROR;
    }

    fcAutoNegRegAddr= PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                      perPortRegs[portNum].autoNegCtrl;

    /* set Auto-Negotiation */
    offset= 11;

    /* advertise for Flow Control */
    if(prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,fcAutoNegRegAddr, offset, 1, stateValue) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* advertise for Flow Control */
    offset= 9;
    if(prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,fcAutoNegRegAddr, offset, 1, pauseAdvValue) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    /* enable port if we need */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 1) != GT_OK)
            return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortFlowCntrlAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for Flow Control on
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*       state          - GT_TRUE for enable Auto-Negotiation for Flow Control
*                        GT_FALSE otherwise
*       pauseAdvertise - Advertise symmetric flow control support in
*                        Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.CPU port not supports the flow control auto negotiation
*
*       2. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowCntrlAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state,
    IN  GT_BOOL   pauseAdvertise
)
  {
    GT_BOOL   targetState = state;
    GT_BOOL   targetPauseAdvertise = pauseAdvertise;

    CPSS_MACDRV_OBJ_STC * portMacObjPtr;      /* port PHYMAC object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;          /* do switch mac code indicator */                   
    

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);


    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* run MACPHY callback */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacFlowCntlANSetFunc(devNum,portNum,
                                     targetState,targetPauseAdvertise, 
                                     CPSS_MACDRV_STAGE_PRE_E,
                                     &doPpMacConfig,&targetState,
                                     &targetPauseAdvertise);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

         /* run prvCpssDxChPortDuplexAutoNegEnableGet - switch MAC configuration code */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortFlowCntrlAutoNegEnableSet(devNum,portNum,targetState,
                                     targetPauseAdvertise);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacFlowCntlANSetFunc(devNum,portNum,
                                  state,pauseAdvertise, 
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetState,
                                  &targetPauseAdvertise);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortFlowCntrlAutoNegEnableGet    
*
* DESCRIPTION:
*       Get Auto-Negotiation enable/disable state for Flow Control per port
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       statePtr       -   GT_TRUE for enable Auto-Negotiation for Flow Control
*                          GT_FALSE otherwise
*       pauseAdvertisePtr - Advertise symmetric flow control support in
*                           Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the flow control auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortFlowCntrlAutoNegEnableGet
(
    IN   GT_U8       devNum,
    IN   GT_U8       portNum,
    OUT  GT_BOOL     *statePtr,
    OUT  GT_BOOL     *pauseAdvertisePtr
)
{
    GT_U32 value;
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 fcAutoNegRegAddr;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        *statePtr = GT_FALSE;
        *pauseAdvertisePtr = GT_FALSE;
        return GT_OK;
    }
    fcAutoNegRegAddr= PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                      perPortRegs[portNum].autoNegCtrl;

    offset= 11;
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,fcAutoNegRegAddr, offset, 1,
                                  &value) != GT_OK)
        return GT_HW_ERROR;

    *statePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    offset= 9;
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,fcAutoNegRegAddr, offset, 1,
                                  &value) != GT_OK)
        return GT_HW_ERROR;

    *pauseAdvertisePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortFlowCntrlAutoNegEnableGet
*
* DESCRIPTION:
*       Get Auto-Negotiation enable/disable state for Flow Control per port
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       statePtr       -   GT_TRUE for enable Auto-Negotiation for Flow Control
*                          GT_FALSE otherwise
*       pauseAdvertisePtr - Advertise symmetric flow control support in
*                           Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the flow control auto negotiation
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowCntrlAutoNegEnableGet
(
    IN   GT_U8       devNum,
    IN   GT_U8       portNum,
    OUT  GT_BOOL     *statePtr,
    OUT  GT_BOOL     *pauseAdvertisePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;       /* port PHYMAC object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;           /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);
    CPSS_NULL_PTR_CHECK_MAC(pauseAdvertisePtr);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacFlowCntlANGetFunc(devNum,portNum,
                                  statePtr,pauseAdvertisePtr, 
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,statePtr,
                                  pauseAdvertisePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /*   if switch MAC value should be returned prvCpssDxChPortFlowCntrlAutoNegEnableGet 
     *   will run 
     */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortFlowCntrlAutoNegEnableGet(devNum,portNum,statePtr,
                                                   pauseAdvertisePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post sage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacFlowCntlANGetFunc(devNum,portNum,
                                                    statePtr,pauseAdvertisePtr, 
                                                    CPSS_MACDRV_STAGE_POST_E,
                                                    &doPpMacConfig,statePtr,
                                                    pauseAdvertisePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}
/*******************************************************************************
* prvCpssDxChPortSpeedAutoNegEnableSet  
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation of interface speed on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the speed auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSpeedAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 portState;       /* current port state (enable/disable) */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 speedAutoNegRegAddr;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if((portMacType >= PRV_CPSS_PORT_XG_E) || ((portNum) == CPSS_CPU_PORT_NUM_CNS))
    {
        /* for disable Auto-Negotiation for speed mode */
        if (state == GT_FALSE)
        {
            return GT_OK;
        }
        else
        {
            return GT_NOT_SUPPORTED;
        }
    }

    value = (state == GT_TRUE) ? 1 : 0;

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);

    /* store value of port state */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &portState) != GT_OK)
        return GT_HW_ERROR;

    /* disable port if we need */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
            return GT_HW_ERROR;
    }

    speedAutoNegRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                          perPortRegs[portNum].autoNegCtrl;
    offset = 7;

    /* set Auto-Negotiation for speed */
    if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,
                                  speedAutoNegRegAddr,
                                  offset,
                                  1,
                                  value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* enable port if we need */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 1) != GT_OK)
            return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSpeedAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation of interface speed on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*       2.CPU port not supports the speed auto negotiation
*
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedAutoNegEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
{
    GT_BOOL targetState = state; 
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;            /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);


    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* run MACPHY callback */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacSpeedANSetFunc(devNum,portNum,
                                  targetState,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set switch MAC configuration */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortSpeedAutoNegEnableSet(devNum,portNum,state);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacSpeedANSetFunc(devNum,portNum,
                                  targetState,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortSpeedAutoNegEnableGet    
*
*
* DESCRIPTION:
*
*   Get status  of  Auto-Negotiation enable on specified port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enabled Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the speed auto negotiation
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSpeedAutoNegEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT  GT_BOOL  *statePtr
)
{
    GT_U32 value;
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 speedAutoNegRegAddr;

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        *statePtr = GT_FALSE;
        return GT_OK;
    }
    speedAutoNegRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                          perPortRegs[portNum].autoNegCtrl;

    offset= 7;

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            speedAutoNegRegAddr, offset, 1,
                                  &value) != GT_OK)
        return GT_HW_ERROR;


    *statePtr = (value == 1) ? GT_TRUE : GT_FALSE;


    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSpeedAutoNegEnableGet
*
* DESCRIPTION:
*
*   Get status  of  Auto-Negotiation enable on specified port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       statePtr - GT_TRUE for enabled Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*       2. CPU port not supports the speed auto negotiation
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortSpeedAutoNegEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT  GT_BOOL  *statePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port callback pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;                       /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* MACPHY callback run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacSpeedANGetFunc(devNum,portNum,
                                  statePtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get the value from switch MAC */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortSpeedAutoNegEnableGet(devNum,portNum,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacSpeedANGetFunc(devNum,portNum,
                                  statePtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortAutoNegStatusGet
*
* DESCRIPTION:
*       Gets Link Status of specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       isLinkUpPtr - GT_TRUE for link up, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       use by npd - cpss1.3
*
*******************************************************************************/
GT_STATUS cpssDxChPortAutoNegStatusGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *isAutoNegPtr
)
{
    GT_U32 regAddr;         /* register address */
    unsigned int value;           /* value to write into the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(isAutoNegPtr);

    PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portNum,&regAddr);

    if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 11, 1, &value) != GT_OK)
        return GT_HW_ERROR;
	#ifdef AX_FULL_DEBUG
	osPrintf("cpssDxChPortAutoNegStatusGet:dev %d port %d val %d\n",devNum,portNum,value);
	#endif

    if (value == 1)
        *isAutoNegPtr = GT_TRUE;
    else
        *isAutoNegPtr = GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortFlowControlEnableSet    
*
* DESCRIPTION:
*       Enable/disable receiving and transmission of 802.3x Flow Control frames
*       in full duplex on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*                  Note: only XG ports can be configured in all 4 options,
*                        Tri-Speed and FE ports may use only first two.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, state or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortFlowControlEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_FLOW_CONTROL_ENT  state
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 portState;       /* current port state (enable/disable) */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 length;          /* number of bits to be written to the register */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    /* disable port if we need */
    if (prvPortDisableSet(devNum,portNum, &portState) != GT_OK)
        return GT_HW_ERROR;

#ifdef __AX_PLATFORM__
	for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
	{
#else
	portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
		if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
			continue;
#else
			return GT_OK;
#endif


        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            switch (state)
            {
                case CPSS_PORT_FLOW_CONTROL_DISABLE_E:
                    value = 0;
                    break;
                case CPSS_PORT_FLOW_CONTROL_RX_TX_E:
                    value = 3;
                    break;
                case CPSS_PORT_FLOW_CONTROL_RX_ONLY_E:
                    value = 1;
                    break;
                case CPSS_PORT_FLOW_CONTROL_TX_ONLY_E:
                    value = 2;
                    break;
                default:
                    return GT_BAD_PARAM;
            }

            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);

            offset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 7 : 8;
            length = 2;
        }
        else
        {
            switch (state)
            {
                case CPSS_PORT_FLOW_CONTROL_DISABLE_E:
                    value = 0;
                    break;
                case CPSS_PORT_FLOW_CONTROL_RX_TX_E:
                    value = 1;
                    break;
                default:
                    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum)<PRV_CPSS_PORT_XG_E)
                        return GT_BAD_PARAM;
                    else
#ifdef __AX_PLATFORM__
                        continue;
#else
						return GT_OK;
#endif
            }
    
            PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portNum,&regAddr);
    
            offset = 8;
            length = 1;
        }

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                    regAddr, offset, length, value) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
#ifdef __AX_PLATFORM__
    }
#endif

    /* enable port if we need */
    if (prvPortEnableSet(devNum,portNum, portState) != GT_OK)
        return GT_HW_ERROR;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable receiving and transmission of 802.3x Flow Control frames
*       in full duplex on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*                  Note: only XG ports can be configured in all 4 options,
*                        Tri-Speed and FE ports may use only first two.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, state or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlEnableSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_FLOW_CONTROL_ENT  state
)
{
    CPSS_PORT_FLOW_CONTROL_ENT  targetState = state; 
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port PHYMAC object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacFlowCntlSetFunc(devNum,portNum,
                                  state,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* switch MAC configuration*/
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortFlowControlEnableSet(devNum,portNum,targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

       /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacFlowCntlSetFunc(devNum,portNum,
                                  targetState,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortFlowControlEnableGet        
*
* DESCRIPTION:
*       Get status of 802.3x Flow Control on specific logical port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       statePtr - Pointer to Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortFlowControlEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_FLOW_CONTROL_ENT      *statePtr
)
{
    GT_U32  value;
    GT_U32  regAddr;
    GT_U32  fieldOffset;
    GT_U32  fieldLength;
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    fieldLength = 2;

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
        fieldOffset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 7 : 8;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portNum,&regAddr);
        fieldOffset = 4;
    }


    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
        regAddr, fieldOffset, fieldLength, &value) != GT_OK)
        return GT_HW_ERROR;

    switch (value)
    {
        case 0:
            *statePtr = CPSS_PORT_FLOW_CONTROL_DISABLE_E;
            break;
        case 1:
            *statePtr = CPSS_PORT_FLOW_CONTROL_RX_ONLY_E;
            break;
        case 2:
            *statePtr = CPSS_PORT_FLOW_CONTROL_TX_ONLY_E;
            break;
        case 3:
            *statePtr = CPSS_PORT_FLOW_CONTROL_RX_TX_E;
            break;
        default:
            /* no chance getting here */
            return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortFlowControlEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x Flow Control on specific logical port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       statePtr - Pointer to Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlEnableGet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    OUT CPSS_PORT_FLOW_CONTROL_ENT      *statePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port PHYMAC object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacFlowCntlGetFunc(devNum,portNum,
                                  statePtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get vavue from switch MAC */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortFlowControlEnableGet(devNum,portNum,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacFlowCntlGetFunc(devNum,portNum,
                                  statePtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,statePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPeriodicFcEnableSet    
*
* DESCRIPTION:
*       Enable/Disable transmits of periodic 802.3x flow control.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       enable   - If GT_TRUE, enable transmit.
*                  If GT_FALSE, disable transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPeriodicFcEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
{
    GT_U32 regAddr, regAddrXoff; /* register address               */
    GT_U32 value;           /* value to write into the register    */
    GT_U32 portState;       /* current port state (enable/disable) */
    GT_U32 offset;          /* bit number inside register          */
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    value = (enable == GT_TRUE) ? 1 : 0;

    /* disable port if we need */
    if (prvPortDisableSet(devNum,portNum, &portState) != GT_OK)
        return GT_HW_ERROR;

#ifdef __AX_PLATFORM__
			for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
			{
#else
			portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
				if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
					continue;
#else
					return GT_OK;
#endif


        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
            offset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 10 : 11;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
            offset = 1;
        }

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, offset, 1, value) != GT_OK)
                return GT_HW_ERROR;
        }

        if((portNum != CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddrXoff);
                if(regAddrXoff != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
                {
                    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddrXoff, offset, 1, value) != GT_OK)
                        return GT_HW_ERROR;
                }
            }
        }
#ifdef __AX_PLATFORM__
    }
#endif
    /* enable port if we need */
    if (prvPortEnableSet(devNum,portNum, portState) != GT_OK)
        return GT_HW_ERROR;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPeriodicFcEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmits of periodic 802.3x flow control.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       enable   - If GT_TRUE, enable transmit.
*                  If GT_FALSE, disable transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPeriodicFcEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
{
    GT_BOOL targetEnable = enable; 
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;            /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPeriodFlowCntlSetFunc(devNum,portNum,
                                  targetEnable,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set switch MAC configuration */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortPeriodicFcEnableSet(devNum,portNum,targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPeriodFlowCntlSetFunc(devNum,portNum,
                                    targetEnable,CPSS_MACDRV_STAGE_POST_E,
                                    &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPeriodicFcEnableGet         
*
* DESCRIPTION:
*       Get status of periodic 802.3x flow control transmition.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - Pointer to the periodic 802.3x flow control
*                    tramsition state.
*                    If GT_TRUE, enable transmit.
*                    If GT_FALSE, disable transmit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPeriodicFcEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32 regAddr, regAddrXoff; /* register address       */
    GT_U32 value, valueXoff;     /* value read from register    */
    GT_U32 offset;          /* bit number inside register  */
    GT_STATUS rc;
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum, portNum, portMacType, &regAddrXoff);
            offset = 10;
        }
        else
        {
            offset = 11;
        }
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum, portNum, portMacType, &regAddr);
        offset = 1;
    }

    /* get state */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, offset, 1, &value);
    if (rc != GT_OK)
    {
        return GT_HW_ERROR;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, offset, 1, &valueXoff);
        if (rc != GT_OK)
        {
            return GT_HW_ERROR;
        }
        *enablePtr = ((value == 1) || (valueXoff == 1)) ? GT_TRUE : GT_FALSE;
    }
    else
    {
        *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPeriodicFcEnableGet
*
* DESCRIPTION:
*       Get status of periodic 802.3x flow control transmition.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - Pointer to the periodic 802.3x flow control
*                    tramsition state.
*                    If GT_TRUE, enable transmit.
*                    If GT_FALSE, disable transmit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPeriodicFcEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPeriodFlowCntlGetFunc(devNum,portNum,
                                  enablePtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get value from switch MAC */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortPeriodicFcEnableGet(devNum,portNum,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPeriodFlowCntlGetFunc(devNum,portNum,
                                    enablePtr,CPSS_MACDRV_STAGE_POST_E,
                                    &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortBackPressureEnableSet   
*
* DESCRIPTION:
*       Enable/disable of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for enable Back Pressure, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For XGMII (10 Gbps) ports feature is not supported.
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBackPressureEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
{
    GT_U32 value;           /* value to write into the register */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 backPressureRegAddr;

    /* can not be changed for 10 Gb interface */
    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    value = (state == GT_TRUE) ? 1 : 0;

    /* set BackPressure Enable bit */
    /* Serial Parameters Register is one for 6 ports set */

    offset = 4;
    PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portNum,&backPressureRegAddr);

    if (prvCpssDrvHwPpPortGroupSetRegField(devNum,PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                                  backPressureRegAddr,
                                  offset,
                                  1,
                                  value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortBackPressureEnableSet
*
* DESCRIPTION:
*       Enable/disable of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for enable Back Pressure, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*       2. For XGMII (10 Gbps) ports feature is not supported.
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortBackPressureEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
{
    GT_BOOL targetState = state;

    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacBackPrSetFunc(devNum,portNum,
                                  targetState,CPSS_MACDRV_STAGE_PRE_E,
                                       &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set switch MAC configuration */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortBackPressureEnableSet(devNum,portNum,targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacBackPrSetFunc(devNum,portNum,
                                  targetState,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetState);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortBackPressureEnableGet      
*
* DESCRIPTION:
*       Gets the state of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - pointer to back pressure enable/disable state:
*                       - GT_TRUE to enable Back Pressure
*                       - GT_FALSE to disable Back Pressure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For XGMII (10 Gbps) ports feature is not supported.
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortBackPressureEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
)
{
    GT_U32 value;           /* value to write into the register */
    GT_U32 regAddr;         /* register address            */

    /* can not be changed for 10 Gb interface */
    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* get BackPressure Enable bit */
    /* Serial Parameters Register is one for 6 ports set */
    PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portNum,&regAddr);

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 4, 1, &value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortBackPressureEnableGet
*
* DESCRIPTION:
*       Gets the state of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - pointer to back pressure enable/disable state:
*                       - GT_TRUE to enable Back Pressure
*                       - GT_FALSE to disable Back Pressure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*       For XGMII (10 Gbps) ports feature is not supported.
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortBackPressureEnableGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *enablePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacBackPrGetFunc(devNum,portNum,
                                  enablePtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get value from switch MAC */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortBackPressureEnableGet(devNum,portNum,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    
    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacBackPrGetFunc(devNum,portNum,
                                  enablePtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}


/*******************************************************************************
* prvCpssDxChPortLinkStatusGet         
*
* DESCRIPTION:
*       Gets Link Status of specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       isLinkUpPtr - GT_TRUE for link up, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortLinkStatusGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *isLinkUpPtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */

    PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portNum,&regAddr);

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 0, 1, &value) != GT_OK)
        return GT_HW_ERROR;

    if (value == 1)
        *isLinkUpPtr = GT_TRUE;
    else
        *isLinkUpPtr = GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortLinkStatusGet
*
* DESCRIPTION:
*       Gets Link Status of specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       isLinkUpPtr - GT_TRUE for link up, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortLinkStatusGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT GT_BOOL   *isLinkUpPtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(isLinkUpPtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPortlinkGetFunc(devNum,portNum,
                                  isLinkUpPtr,CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,isLinkUpPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get value from switch MAC */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortLinkStatusGet(devNum,portNum,isLinkUpPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPortlinkGetFunc(devNum,portNum,
                                  isLinkUpPtr,CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,isLinkUpPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}


/*******************************************************************************
* cpssDxChPortInterfaceModeSet
*
* DESCRIPTION:
*       Sets Interface mode on a specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - Interface mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     1. For CPU port the interface may be one of GMII, RGMII, MII and MII
*        PHY. RGMII and MII PHY port interface modes are not supported for
*        DxCh3 CPU port.
*     2. For Tri-Speed ports the interface may be SGMII or 1000 BASE-X.
*     3. For Flex-Link ports the interface may be SGMII, 1000 BASE-X,
*        100 BASE-FX, QX, HX or XGMII.
*        Depends on capability of the specific device. The port's speed
*        should be set after configuring Interface Mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortInterfaceModeSet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_U32     regAddr;        /* register address */
    GT_U32     value;          /* value to write into the register */
    GT_U32     resetState = 0; /* reset state */
    GT_STATUS  rc;             /* return code     */
    GT_U32     portGroupId;    /*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT    currentMacMode;
    PRV_CPSS_PORT_TYPE_ENT    newMacMode;
    GT_U32  localPort;
    CPSS_PORT_INTERFACE_MODE_ENT ifModePrv;
    GT_U8   startSerdes = 0, numOfSerdesLanes = 0;
    GT_U8   globalPort, i;
    GT_BOOL dontResetSerdes = GT_FALSE; /* don't reset serdeses they are already configured as required */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if((ifMode >= CPSS_PORT_INTERFACE_MODE_NA_E) || (ifMode < CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E))
        return GT_BAD_PARAM;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* the Interface can be changed only for Flex-Link port */
    if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
    {
        /* check if interface mode supported by given port */
        if(supportedPortsModes[PRV_CPSS_DXCH_FLEX_PORT_TYPE_MAC(devNum,portNum)][ifMode] != GT_TRUE)
        {
            return GT_NOT_SUPPORTED;
        }

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_40G_MAC_NOT_SUPPORTED_WA_E) &&
                 PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_20G_MAC_NOT_SUPPORTED_WA_E) &&
                 PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_1G_SPEED_NOT_SUPPORTED_WA_E))
            {
                if((ifMode != CPSS_PORT_INTERFACE_MODE_XGMII_E /* XAUI */)
                    && (ifMode != CPSS_PORT_INTERFACE_MODE_RXAUI_E))
                {
                    return GT_NOT_SUPPORTED;
                }
            }

            if(PRV_CPSS_PP_MAC(devNum)->revision >= 2)
            { /* check if B0 supports requested mode */
                switch (ifMode)
                {
                    case CPSS_PORT_INTERFACE_MODE_SGMII_E:
                    case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
                    case CPSS_PORT_INTERFACE_MODE_XGMII_E:
                    case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
                    case CPSS_PORT_INTERFACE_MODE_XLG_E:
                    case CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E:
                    break;

                    default:
                        return GT_NOT_SUPPORTED;
                }
            }
        }

        switch (ifMode)
        {
            case CPSS_PORT_INTERFACE_MODE_SGMII_E:
            case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
            case CPSS_PORT_INTERFACE_MODE_100BASE_FX_E:
                newMacMode = PRV_CPSS_PORT_GE_E;
                break;
            case CPSS_PORT_INTERFACE_MODE_QX_E:
            case CPSS_PORT_INTERFACE_MODE_HX_E:
            case CPSS_PORT_INTERFACE_MODE_XGMII_E:
            case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
            case CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E:
                newMacMode = PRV_CPSS_PORT_XG_E;
                break;
            case CPSS_PORT_INTERFACE_MODE_XLG_E:
                newMacMode = PRV_CPSS_PORT_XLG_E;
                break;
            default:
                return GT_NOT_SUPPORTED;
        }

        currentMacMode = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

        rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifModePrv);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* may be scenario when serdeses of odd port used for XGMII by even port,
         * now even port changed mode to lets say RXAUI and we want to change mode
         * of odd port too, lets fake here old port's mode to some mode using just
         * own serdeses of current port.
         */
        if(ifModePrv == CPSS_PORT_INTERFACE_MODE_NA_E)
            ifModePrv = CPSS_PORT_INTERFACE_MODE_RXAUI_E;

        /* can't define XLG mode if one of ports which serdeses will be occupied by XLG port is special */
        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) && (ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E))
        {
            if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&startSerdes,&numOfSerdesLanes)) != GT_OK)
                return rc;

            for(i = (GT_U8)(startSerdes/2); i*2 < startSerdes+numOfSerdesLanes; i++)
            {
                globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,i);
                if((globalPort != portNum)
                    && (PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,globalPort) == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E))
                {
                    return GT_BAD_STATE;
                }
            }
        }

        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) && (ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E))
        {
            if((localPort == 8) || (localPort == 9) || (localPort == 11))
            {/* can't define special mode on ports which serdeses occupied by XLG port */
                globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,10);
                if(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,globalPort) == CPSS_PORT_INTERFACE_MODE_XLG_E)
                {
                    return GT_BAD_STATE;
                }
            }

            if(localPort%2 != 0)
            {/* if neighbor even port occupies more then 2 serdes then special mode could be defined only
              * if neighbor is XGMII */
                if((rc = getNumberOfSerdesLanesForPortLocal(devNum,(GT_U8)(portNum-1),
                                                        PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum-1),
                                                        &startSerdes,&numOfSerdesLanes)) != GT_OK)
                    return rc;
                if((numOfSerdesLanes > 2)
                    && (PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum-1) != CPSS_PORT_INTERFACE_MODE_XGMII_E))
                    return GT_BAD_STATE;
            }
        }

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {/* for Lion reset mac, xpcs, serdes before changing port mode and speed,
          * so unreset will be in serdesPowerStatusSet */

            if((rc = getNumberOfSerdesLanesForPortLocal(devNum,portNum,ifModePrv, /* reset here accordingly to old ifMode */
                                                    &startSerdes,&numOfSerdesLanes)) != GT_OK)
                return rc;

            for(i = (GT_U8)(startSerdes/2); i*2 < startSerdes+numOfSerdesLanes; i++)
            {
                globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,i);
                if((rc = prvCpssDxChPortMacResetStateSet(devNum, globalPort, GT_TRUE)) != GT_OK)
                    return rc;

                if(globalPort != portNum)
                {
                    /* invalidate current interface mode in DB */
                    PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, globalPort) = CPSS_PORT_INTERFACE_MODE_NA_E;
                }
            }

            if((ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E) && (localPort%2 != 0))
            {
                if(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum-1) == CPSS_PORT_INTERFACE_MODE_XGMII_E)
                    dontResetSerdes = GT_TRUE;
            }

            if(dontResetSerdes != GT_TRUE)
            {
                if((rc = prvCpssDxChPortSerdesResetStateSet(devNum,portNum,startSerdes,numOfSerdesLanes,GT_TRUE)) != GT_OK)
                    return rc;
            }
        }

        if (newMacMode != currentMacMode)
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            globalRegs.sampledAtResetReg;
                value = (newMacMode == PRV_CPSS_PORT_GE_E) ? 0 : 1;
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                        portNum + 2, 1, value) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
            }

            /* change XG Port MAC Control Register3 for xCat before prvCpssDxChXcatHwRegAddrPortMacUpdate;
                in Lion B0 for 1G MAC this field has different role */
            if ((newMacMode == PRV_CPSS_PORT_GE_E) && 
                (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E))
            {
                /* Set GigMAC in MacModeSelect */
                PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                        13, 3, 0) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
            }
        }

        PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType = newMacMode;

        /* update addresses of mac registers accordingly to used MAC 1G/XG/XLG */
        rc = prvCpssDxChXcatHwRegAddrPortMacUpdate(devNum, portNum, ifMode);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {/* Set MAC type in MacModeSelect */
            /* In Lion MAC Mode is selected by the XG port.
                This field is not applicable for 40G ports and for 1G
                it has some other purpose */
            regAddr = 0x0880001C + localPort * 0x400;
            switch (ifMode)
            {
                case CPSS_PORT_INTERFACE_MODE_SGMII_E:
                case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
                case CPSS_PORT_INTERFACE_MODE_100BASE_FX_E:
                    value = 0;
                    break;
                case CPSS_PORT_INTERFACE_MODE_QX_E:
                case CPSS_PORT_INTERFACE_MODE_HX_E:
                case CPSS_PORT_INTERFACE_MODE_XGMII_E: /* both 10G and 20G */
                case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
                case CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E:
                    value = 1;
                    break;
                case CPSS_PORT_INTERFACE_MODE_XLG_E:
                    value = 3;
                    break;
                default:
                    return GT_NOT_SUPPORTED;
            }
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 13, 3, value) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            HW_WAIT_MILLISECONDS_MAC(20);
        }

        if(GT_TRUE == dontResetSerdes)
        {/* other configurations done in scope of even neighbor port */
            if((rc = prvCpssDxChPortMacResetStateSet(devNum, portNum, GT_FALSE)) != GT_OK)
                return rc;

            /* save new interface mode in DB */
            PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum) = ifMode;

            return GT_OK;
        }

        if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
           /* Call serdes reset again, because in new mode could be used more SerDes's then in old one
                and type of MAC may change */

            if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&startSerdes,&numOfSerdesLanes)) != GT_OK)
                return rc;

            for(i = (GT_U8)(startSerdes/2); i*2 < startSerdes+numOfSerdesLanes; i++)
            {
                globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,i);
                if((rc = prvCpssDxChPortMacResetStateSet(devNum, globalPort, GT_TRUE)) != GT_OK)
                    return rc;

                if(globalPort != portNum)
                {
                    /* invalidate current interface mode in DB */
                    PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, globalPort) = CPSS_PORT_INTERFACE_MODE_NA_E;
                }
            }

            /* Set ports to XGMII/RXAUI mode no matter what mode really used what matters
                is number of used SerDeses */
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                if(ifMode == CPSS_PORT_INTERFACE_MODE_RXAUI_E)
                {
                    value = BIT_MASK_MAC(numOfSerdesLanes);
                }
                else
                {
                    value = 0;
                }

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, startSerdes, numOfSerdesLanes,
                                                        value) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
            }

            /* must update ifMode in DB here, because prvCpssDxChPortSerdesResetStateSet uses it */
            PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum) = ifMode;

            /* run this reset after interface mode configuration, because it defines number of serdeses
                to reset accordingly to defined mode */
            if((rc = prvCpssDxChPortSerdesResetStateSet(devNum,portNum,startSerdes,numOfSerdesLanes,GT_TRUE)) != GT_OK)
                return rc;

            HW_WAIT_MILLISECONDS_MAC(10);
        }

        if ((newMacMode == PRV_CPSS_PORT_XG_E) || (newMacMode == PRV_CPSS_PORT_XLG_E))
        {
            switch (ifMode)
            {
                case CPSS_PORT_INTERFACE_MODE_QX_E:
                    value = 0;
                    break;
                case CPSS_PORT_INTERFACE_MODE_HX_E:
                    value = 1;
                    break;
                case CPSS_PORT_INTERFACE_MODE_XGMII_E:
                case CPSS_PORT_INTERFACE_MODE_RXAUI_E:
                case CPSS_PORT_INTERFACE_MODE_XLG_E:
                case CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E:
                    /* RXAUI and XAUI both use 4 lanes, but RXAUI uses 2 SerDes while XAUI uses 4 */
                    value = 2;
                    break;
                default:
                    return GT_BAD_PARAM;
            }

            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
                && (PRV_CPSS_PP_MAC(devNum)->revision > 2)
                && ((ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E)||(ifMode == CPSS_PORT_INTERFACE_MODE_RXAUI_E)))
            {/* define mediaInterface for xCat A2 */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                        localPort, 1,
                                                        (ifMode == CPSS_PORT_INTERFACE_MODE_RXAUI_E) ? 1 : 0)
                                                        != GT_OK)
                {
                    return GT_HW_ERROR;
                }
            }

            /* Get LaneActive in Global Configuration0 register*/
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].xgGlobalConfReg0;

            /* xCat Note:LaneActive may be changed only when <PCSReset> is HIGH.
               Lion Note: This field may be changed only when PCSReset_ is asserted */
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &resetState) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            if(resetState == 0)
            {
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 1) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
                HW_WAIT_MILLISECONDS_MAC(20);
            }

            /* set LaneActive */
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 5, 2, value) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            HW_WAIT_MILLISECONDS_MAC(20);

            /* restore PSC Reset state */
            if(resetState == 0)
            {
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
                HW_WAIT_MILLISECONDS_MAC(20);
            }

            if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
            {/* xcat haven't 20G and 40G, so here it's simple */
                /* Set MAC type in MacModeSelect */
                PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 13, 3, 1) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
                HW_WAIT_MILLISECONDS_MAC(20);
            }

            /* save new interface mode in DB */
            PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum) = ifMode;

            return GT_OK;
        }
        else /* Gig Mac */
        {
            /* 100 BASE-FX supported only on FlexLink ports */
            if (ifMode == CPSS_PORT_INTERFACE_MODE_100BASE_FX_E)
            {
                GT_U32 portState;

                PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

                /* store value of port state */
                if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &portState) != GT_OK)
                    return GT_HW_ERROR;

                /* disable port if we need */
                if (portState == 1)
                {
                    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
                        return GT_HW_ERROR;
                }

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                            perPortRegs[portNum].Config100Fx;
                /* 100FX PCS Sigdet polarity */
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                       13, 1, 1) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
                /* desired 100-FX PHY address in MDC/MDIO transaction */
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                       8, 5, portNum+4) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
                /* 100FX PCS use sigdet */
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                       5, 1, 1) != GT_OK)
                {
                    return GT_HW_ERROR;
                }

                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                            perPortRegs[portNum].autoNegCtrl;
                /* AnDuplexEn => 0x0, SetFullDuplex => 0x1, AnFcEn => 0x0 */
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                       11, 3, 2) != GT_OK)
                {
                    return GT_HW_ERROR;
                }

                /* PauseAdv => 0x0, SetFcEn => 0x0, AnSpeedEn => 0x0, SetGMIISpeed => 0x0,
                    SetMIISpeed => 0x1 */
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                       5, 5, 1) != GT_OK)
                {
                    return GT_HW_ERROR;
                }

                /* PcsEn field in Port MAC Control Register2*/
                PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                       3, 1, 0) != GT_OK)
                {
                    return GT_HW_ERROR;
                }

                /* 100FX enable field in 100FX Configuration Register */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                            perPortRegs[portNum].Config100Fx;
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                       7, 1, 1) != GT_OK)
                {
                    return GT_HW_ERROR;
                }

                /* enable port if we need */
                if (portState == 1)
                {
                    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

                    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 1) != GT_OK)
                        return GT_HW_ERROR;
                }

                /* save new interface mode in DB */
                PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum) = ifMode;

                return GT_OK;
            }
            else
            {
                if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
                {
                    /* 100FX enable field in 100FX Configuration Register */
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                                perPortRegs[portNum].Config100Fx;
                    if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                           7, 1, 0) != GT_OK)
                    {
                        return GT_HW_ERROR;
                    }
                }

                /* PcsEn field in Port MAC Control Register2*/
                PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,
                                                       3, 1, 1) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
            }
        }
    }
    else
    {
        /* not FlexLink port */
        if((portNum != CPSS_CPU_PORT_NUM_CNS) &&
           (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_XG_E))
        {
            if(ifMode != CPSS_PORT_INTERFACE_MODE_XGMII_E)
            {
                return GT_BAD_PARAM;
            }

            /* save new interface mode in DB */
            PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum) = ifMode;

            return GT_OK;
        }

        if((PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
            && ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
                || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)))
        {
            if(ifMode != CPSS_PORT_INTERFACE_MODE_MII_E)
            {
                return GT_BAD_PARAM;
            }

            /* save new interface mode in DB */
            PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum) = ifMode;

            return GT_OK;
        }
    }

    return prvCpssDxChPortGigInterfaceModeSet(devNum,portNum,ifMode);
}

/*******************************************************************************
* prvCpssDxChPortInterfaceModeGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - wrong media interface mode value received
*       GT_BAD_STATE             - if XGMII(XAUI) mode used and odd portNum provided
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortInterfaceModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* register value*/
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 localPort;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* the Interface for XG port can be changed only for Flex-Link port */
    if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                globalRegs.sampledAtResetReg;
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr,
                                              portNum + 2, 1, &value) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
        else
        {
            /* in available Lion boxes port 10 capable for 40G support
             * it's implemented by taking over serdeses of ports 8,9,10,11
             * so when internal function asking about ports 8,9,11 while
             * port 10 is XLG - return XLG
             */
            if (localPort == 8 || localPort == 9 || localPort == 11)
            {
                GT_U8    tempPort;

                tempPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,10);
                if (PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,tempPort) == CPSS_PORT_INTERFACE_MODE_XLG_E)
                {
                    *ifModePtr = CPSS_PORT_INTERFACE_MODE_XLG_E;
                    return GT_OK;
                }
            }

            /* Get MAC type in MacModeSelect */
            /* In Lion MAC Mode is selected by the XG port.
               This field is not applicable for 40G ports and for 1G
               it has some other purpose, so use here hardcoded register address */
            regAddr = 0x0880001C + localPort * 0x400;
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 13, 3, &value) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        if (value == 0) /* SGMII MODE */
        {
            if(! PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
            {
                /* get PortType from Port MAC Control Register0*/
                PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

                if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr,
                                                       1, 1, &value) != GT_OK)
                {
                    return GT_HW_ERROR;
                }

                *ifModePtr = (value == 0) ? CPSS_PORT_INTERFACE_MODE_SGMII_E :
                                                CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;

                return GT_OK;
            }

            /* get 100FX enable from 100FX Configuration Register */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                    perPortRegs[portNum].Config100Fx;
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr,
                                                   7, 1, &value) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            if (value == 1)
            {
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_100BASE_FX_E;
            }
            else
            {
                /* get PortType from Port MAC Control Register0*/
                PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

                if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr,
                                                       1, 1, &value) != GT_OK)
                {
                    return GT_HW_ERROR;
                }

                *ifModePtr = (value == 0) ? CPSS_PORT_INTERFACE_MODE_SGMII_E :
                                                CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
            }
        }
        else
        {
            if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                switch(value)
                {
                    case 1:
                        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
                        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                                localPort*2, 1, &value) != GT_OK)
                        {
                            return GT_HW_ERROR;
                        }

                        if (localPort % 2 == 0)
                        {
                            *ifModePtr = (value == 1) ? CPSS_PORT_INTERFACE_MODE_RXAUI_E :
                                                        CPSS_PORT_INTERFACE_MODE_XGMII_E;
                        }
                        else
                        {
                            if(value == 1)
                                *ifModePtr = CPSS_PORT_INTERFACE_MODE_RXAUI_E;
                            else
                            {
                                if(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum) == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E)
                                {
                                    *ifModePtr = CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E;
                                    return GT_OK;
                                }
                                else
                                {
                                    *ifModePtr = CPSS_PORT_INTERFACE_MODE_XGMII_E;
                                    return GT_BAD_STATE;
                                }
                            }
                        }

                        break;

                    case 2:
                        *ifModePtr = CPSS_PORT_INTERFACE_MODE_XGMII_E;
                        break;

                    case 3:
                        *ifModePtr = CPSS_PORT_INTERFACE_MODE_XLG_E;
                        break;

                    /* TBD
                    case 7:
                        *ifModePtr = CPSS_PORT_INTERFACE_MODE_HGL_E;
                        break;
                    */

                    default:
                        return GT_NOT_SUPPORTED;
                }
                return GT_OK;
            }

            /* Get LaneActive in Global Configuration0 register*/
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].xgGlobalConfReg0;
            if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 5, 2, &value) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            switch (value)
            {
                case 0:
                    *ifModePtr = CPSS_PORT_INTERFACE_MODE_QX_E;
                    break;
                case 1:
                    *ifModePtr = CPSS_PORT_INTERFACE_MODE_HX_E;
                     break;
                case 2:
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl2;
                    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                            portNum, 1, &value) != GT_OK)
                    {
                        return GT_HW_ERROR;
                    }

                     *ifModePtr = (value == 0) ? CPSS_PORT_INTERFACE_MODE_XGMII_E :
                                                        CPSS_PORT_INTERFACE_MODE_RXAUI_E;
                     break;
                default:
                     return GT_FAIL;
            }
        }
        return GT_OK;
    }

    /* irrelevant for 10 Gb interface */
    if((portNum != CPSS_CPU_PORT_NUM_CNS) &&
       (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_XG_E))
    {
        *ifModePtr = CPSS_PORT_INTERFACE_MODE_XGMII_E;
        return GT_OK;
    }

    if((PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
            && ((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
                || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)))
    {
        *ifModePtr = CPSS_PORT_INTERFACE_MODE_MII_E;
        return GT_OK;
    }

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg;

        if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 1, 2, &value) != GT_OK)
            return GT_HW_ERROR;

        switch(value)
        {
            case 0:
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_MII_E;
                break;
            case 1:
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_GMII_E;
                break;
            case 2:
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E;
                break;
            case 3:
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_MII_PHY_E;
                break;
            default:
                break;
        }
        return GT_OK;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr, 1, 1, &value) != GT_OK)
        return GT_HW_ERROR;

    if (value == 1)
    {
        *ifModePtr = CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
    }
    else
    {
        if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
            || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;
            if(prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 15, 6, &value) != GT_OK)
                return GT_HW_ERROR;

            if(value & (1<<(portNum/4)))
            {
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_SGMII_E;
            }
            else
            {
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_QSGMII_E;
            }
        }
        else
        {
            *ifModePtr = CPSS_PORT_INTERFACE_MODE_SGMII_E;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInterfaceModeGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - wrong media interface mode value received
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortInterfaceModeGet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
)
{
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 localPort;
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(ifModePtr);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        /* in available Lion boxes port 10 capable for 40G support
         * it's implemented by taking over serdeses of ports 8,9,10,11
         * so when port 10 configured to XLG mode ports 8,9,11 not available
         */
        if (localPort == 8 || localPort == 9 || localPort == 11)
        {
            GT_U8                          tempPort;

            tempPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,10);
            if (PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,tempPort) == CPSS_PORT_INTERFACE_MODE_XLG_E)
            {
                *ifModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;
                return GT_OK;
            }
        }

        *ifModePtr = PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum);
        return GT_OK;
    }

    rc = prvCpssDxChPortInterfaceModeGet(devNum,portNum,ifModePtr);
    if(rc == GT_BAD_STATE)
    {
        *ifModePtr = CPSS_PORT_INTERFACE_MODE_NA_E;
        return GT_OK;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChPortForceLinkPassEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Pass on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link pass, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortForceLinkPassEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 portStateRegAddr;
    GT_U32 value;           /* value to write into the register */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 portState;       /* current port state (enable/disable) */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&portStateRegAddr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* store value of port state */
    if(prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,portStateRegAddr, 0, 1, &portState) != GT_OK)
        return GT_HW_ERROR;

    /* disable port if we need */
    if (portState == 1)
    {
        if(prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,portStateRegAddr, 0, 1, 0) != GT_OK)
            return GT_HW_ERROR;
    }

#ifdef __AX_PLATFORM__
			for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
			{
#else
			portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
				if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
					continue;
#else
					return GT_OK;
#endif


        /* for 10 Gb interface and up */
        if (portMacType >= PRV_CPSS_PORT_XG_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
            offset = 3;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portNum,&regAddr);
            offset = 1;
        }
    
        value = (state == GT_TRUE) ? 1 : 0;
    
        /* set force link pass bit */
        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if(prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, offset, 1, value) != GT_OK)
                return GT_HW_ERROR;
        }
#ifdef __AX_PLATFORM__
	}
#endif

    /* enable port if we need */
    if(portState == 1)
    {
        if(prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,portStateRegAddr, 0, 1, 1) != GT_OK)
            return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortForceLinkDownEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Down on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link down, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortForceLinkDownEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
{
    GT_U32        portEnRegAddr;        /* register address */
    GT_U32        forceLinkDownRegAddr; /* register address */
    GT_U32        value;                /* value to write into the register */
    GT_U32        forceLinkDownOffset;  /* bit number inside register       */
    GT_U32        portState;            /* current port state (enable/disable) */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&portEnRegAddr);

    /* store value of port state */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,portEnRegAddr, 0, 1, &portState) != GT_OK)
        return GT_HW_ERROR;

    /* disable port if we need */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,portEnRegAddr, 0, 1, 0) != GT_OK)
            return GT_HW_ERROR;
    }

    value = (state == GT_TRUE) ? 1 : 0;

#ifdef __AX_PLATFORM__
			for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
			{
#else
			portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
				if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
					continue;
#else
					return GT_OK;
#endif


        /* for 10 Gb interface and up*/
        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&forceLinkDownRegAddr);
            forceLinkDownOffset = 2;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portNum,&forceLinkDownRegAddr);
            forceLinkDownOffset = 0;
        }        
     
        /* set force link down bit */
        if(forceLinkDownRegAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, forceLinkDownRegAddr, 
                                                   forceLinkDownOffset, 1, value) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
#ifdef __AX_PLATFORM__
    }
#endif

    /* enable port if we need */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,portEnRegAddr, 0, 1, 1) != GT_OK)
            return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortMruSet       
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       mruSize    - max receive packet size in bytes. (0..16382)
*                    value must be even
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                                   odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 16382
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMruSet
(
    IN  GT_U8  devNum,
    IN  GT_U8  portNum,
    IN  GT_U32 mruSize
)
{
    GT_U32 regAddr;             /* register address */
    GT_U32 value;               /* value to write into the register */
    GT_U32 offset;              /* bit number inside register       */
    GT_U32 portGroupId;         /* port group id */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* The resolution of this field is 2 bytes and
     * the default value is 1522 bytes => 0x2f9
     */
    if(mruSize & 1)
    {
        return GT_BAD_PARAM;
    }

    value = mruSize/2;

    if(value >= BIT_13)
    {
        /* 13 bits in HW */
        return GT_OUT_OF_RANGE;
    }

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
#ifdef __AX_PLATFORM__
    for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
    {
#else
	portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
        if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
			continue;
#else
	    	return GT_OK;
#endif

        if(portMacType < PRV_CPSS_PORT_XG_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
            offset=2;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
            offset = 0;
        }

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, offset, 13, value) != GT_OK)
                return GT_HW_ERROR;
        }
#ifdef __AX_PLATFORM__
    }
#endif

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMruSet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       mruSize    - max receive packet size in bytes. (0..16382)
*                    value must be even
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                                   odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 16382
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortMruSet
(
    IN  GT_U8  devNum,
    IN  GT_U8  portNum,
    IN  GT_U32 mruSize
)
{
    /***********************************************
    * to solve jumbo packet problem: 
    *  - set MRU value on PHY MACs as maximum - 0x3FFF
    *  - switch MAC MRU value set as require in API
    ************************************************/
    GT_U32 targetMruSize = 0x3FFF;

    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacMRUSetFunc(devNum,portNum,
                                  targetMruSize,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,
                                  &targetMruSize);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set MRU size value for switch MAC */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortMruSet(devNum,portNum,mruSize);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacMRUSetFunc(devNum,portNum,
                                  targetMruSize,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,
                                  &targetMruSize);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortMruGet      
*
* DESCRIPTION:
*       Gets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       mruSizePtr - (pointer to) max receive packet size in bytes. (0..16382)
*                    value must be even
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                                   odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 16382
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       
*******************************************************************************/
static GT_STATUS prvCpssDxChPortMruGet
(
    IN   GT_U8  devNum,
    IN   GT_U8  portNum,
    OUT  GT_U32 *mruSizePtr
)
{
    GT_U32 value;           /* value to read from the register  */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 regAddr;         /* register address                 */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    /* cpssDxChPortMruSet updates all available MAC's - here enough to read */
    /* one of them */
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
        offset = 0;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
        offset = 2;
    }

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, offset, 13, &value) != GT_OK)
        return GT_HW_ERROR;

    *mruSizePtr = value * 2;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortMruGet
*
* DESCRIPTION:
*       Gets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       mruSizePtr - (pointer to) max receive packet size in bytes. (0..16382)
*                    value must be even
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or
*                                   odd value of mruSize
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - mruSize > 16382
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       
*******************************************************************************/
GT_STATUS cpssDxChPortMruGet
(
    IN   GT_U8  devNum,
    IN   GT_U8  portNum,
    OUT  GT_U32 *mruSizePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   
    GT_U32  targetMruSize;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(mruSizePtr);

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacMRUGetFunc(devNum,portNum,
                                  mruSizePtr,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,
                                  &targetMruSize);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set MRU size value for switch MAC */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortMruGet(devNum,portNum,mruSizePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacMRUGetFunc(devNum,portNum,
                                  mruSizePtr,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,
                                  &targetMruSize);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;

}

/*******************************************************************************
* prvCpssDxChPortCrcCheckEnableSet     
*
* DESCRIPTION:
*       Enable/Disable 32-bit the CRC checking.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       enable     - If GT_TRUE, enable CRC checking
*                    If GT_FALSE, disable CRC checking
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortCrcCheckEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 portState;       /* current port state (enable/disable) */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    value = (enable == GT_TRUE) ? 1 : 0;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* disable port if needed */
    if (prvPortDisableSet(devNum, portNum, &portState) != GT_OK)
        return GT_HW_ERROR;

#ifdef __AX_PLATFORM__
			 for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
			 {
#else
			 portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
				 if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
					 continue;
#else
					 return GT_OK;
#endif
    
        if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
            offset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 9 : 10;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
            offset = 0;
        }    

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, offset, 1, value) != GT_OK)
                return GT_HW_ERROR;
        }
#ifdef __AX_PLATFORM__
     }
#endif

    /* restore original port state if changed */
    if (prvPortEnableSet(devNum, portNum, portState) != GT_OK)
        return GT_HW_ERROR;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCrcCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable 32-bit the CRC checking.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       enable     - If GT_TRUE, enable CRC checking
*                    If GT_FALSE, disable CRC checking
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortCrcCheckEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
)
{
    GT_BOOL     targetEnable = enable;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacCRCCheckSetFunc(devNum,portNum,
                                  targetEnable,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,
                                  &targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set Port Crc Check Enable value for switch MAC */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortCrcCheckEnableSet(devNum,portNum,targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacCRCCheckSetFunc(devNum,portNum,
                                  targetEnable,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,
                                  &targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortCrcCheckEnableGet    
*
* DESCRIPTION:
*       Get CRC checking (Enable/Disable) state for received packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr - Pointer to the CRS checking state :
*                       GT_TRUE  - CRC checking is enable,
*                       GT_FALSE - CRC checking is disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortCrcCheckEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 offset;          /* bit number inside register       */
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
        offset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 9 : 10;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
        offset = 0;
    }

    /* get state */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, offset, 1, &value);
    if (rc != GT_OK)
    {
        return GT_HW_ERROR;
    }
    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortCrcCheckEnableGet
*
* DESCRIPTION:
*       Get CRC checking (Enable/Disable) state for received packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr - Pointer to the CRS checking state :
*                       GT_TRUE  - CRC checking is enable,
*                       GT_FALSE - CRC checking is disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS cpssDxChPortCrcCheckEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacCRCCheckGetFunc(devNum,portNum,
                                  enablePtr,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,
                                  enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get Port CRC Check Enable value from switch MAC  */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortCrcCheckEnableGet(devNum,portNum,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacCRCCheckGetFunc(devNum,portNum,
                                          enablePtr,
                                          CPSS_MACDRV_STAGE_POST_E,
                                          &doPpMacConfig,
                                          enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* cpssDxChPortXGmiiModeSet
*
* DESCRIPTION:
*       Sets XGMII mode on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       mode  - XGMII mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXGmiiModeSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_XGMII_MODE_ENT    mode
)
{
    GT_U32 value;           /* value to write into the register */
    GT_U32 offset; /* field offset */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32          regAddr;         /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    switch(mode)
    {
        case CPSS_PORT_XGMII_LAN_E:
            value = 0;
            break;
        case CPSS_PORT_XGMII_WAN_E:
            value = 1;
            break;
        case CPSS_PORT_XGMII_FIXED_E:
            value = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        offset = 5;
    }
    else
    {
        offset = 6;
    }

    for(portMacType = PRV_CPSS_PORT_XG_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            /* set xgmii mode */
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, offset, 2, value) != GT_OK)
                return GT_HW_ERROR;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortXGmiiModeGet
*
* DESCRIPTION:
*       Gets XGMII mode on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
* OUTPUTS:
*       modePtr  - Pointer to XGMII mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXGmiiModeGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    OUT CPSS_PORT_XGMII_MODE_ENT    *modePtr
)
{
    GT_U32 value;           /* value read from the register */
    GT_U32 offset; /* field offset */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32          regAddr;         /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        offset = 5;
    }
    else
    {
        offset = 6;
    }

    /* get xgmii mode */
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,
            PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, offset, 2, &value) != GT_OK)
        return GT_HW_ERROR;

    switch(value)
    {
        case 0:
            *modePtr = CPSS_PORT_XGMII_LAN_E;
            break;
        case 1:
            *modePtr = CPSS_PORT_XGMII_WAN_E;
            break;
        case 2:
            *modePtr = CPSS_PORT_XGMII_FIXED_E;
            break;
        default:
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortIpgBaseSet
*
* DESCRIPTION:
*       Sets IPG base for fixed IPG mode on XG ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       ipgBase  - IPG base
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or ipgBase
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when XG Port is in CPSS_PORT_XGMII_FIXED_E mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgBaseSet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    IN  CPSS_PORT_XG_FIXED_IPG_ENT  ipgBase
)
{
    GT_U32 value;   /* value to write into the register */
    GT_U32 offset;  /* bit number inside register       */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32          regAddr;         /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    offset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 9 : 10;

    switch(ipgBase)
    {
        case CPSS_PORT_XG_FIXED_IPG_12_BYTES_E: value = 0;
                                                break;
        case CPSS_PORT_XG_FIXED_IPG_8_BYTES_E:  value = 1;
                                                break;
        default: return GT_BAD_PARAM;
    }

     for(portMacType = PRV_CPSS_PORT_XG_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
     {
         PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);

         if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
         {
             /* set IPG base */
             if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, offset, 1, value) != GT_OK)
                 return GT_HW_ERROR;
         }
     }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortIpgBaseGet
*
* DESCRIPTION:
*       Gets IPG base for fixed IPG mode on XG ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
* OUTPUTS:
*       ipgBasePtr  - pointer to IPG base
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       Relevant only when XG Port is in CPSS_PORT_XGMII_FIXED_E mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgBaseGet
(
    IN  GT_U8                       devNum,
    IN  GT_U8                       portNum,
    OUT CPSS_PORT_XG_FIXED_IPG_ENT  *ipgBasePtr
)
{
    GT_U32 value;   /* value read from the register */
    GT_U32 offset;  /* bit number inside register   */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32          regAddr;         /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(ipgBasePtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    offset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 9 : 10;
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);

    /* set xgmii mode */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,
                PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, offset, 1, &value) != GT_OK)
        return GT_HW_ERROR;

    *ipgBasePtr = (value == 0) ? CPSS_PORT_XG_FIXED_IPG_12_BYTES_E :
                                 CPSS_PORT_XG_FIXED_IPG_8_BYTES_E;

    return GT_OK;
}

/****************************************************************************
* cpssDxChPortIpgSet
*
* DESCRIPTION:
*       Sets the Inter-Packet Gap (IPG) interval of a tri-speed physical port.
*       Using this API may be required to enable wire-speed in traffic paths
*       that include cascading ports, where it may not be feasible to reduce
*       the preamble length.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*       ipg         - IPG in bytes, acceptable range:
*                     [0..511] for DxCh3 and above devices.
*                     [0..15] for DxCh1 and DxCh2 devices.
*                     Default HW value is 12 bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on wrong port type
*       GT_OUT_OF_RANGE  - ipg value out of range
*       GT_BAD_PARAM     - on bad parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U32  ipg
)
{
    GT_U32 regAddr; /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* set */
	/* modify to support XCAT, zhangdi@autelan.com 2011-10-15 */
    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum)||PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        if(ipg >= BIT_9)
            return GT_OUT_OF_RANGE;

        PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,portMacType,&regAddr);
    	osPrintf("set ipg:%d\n",ipg);
    	osPrintf("set regAddr:0x%x\n",regAddr);
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 6, 9, ipg);
    }
    else
    {
        if(ipg >= BIT_4)
            return GT_OUT_OF_RANGE;

        if(portNum != CPSS_CPU_PORT_NUM_CNS)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                                                    perPortRegs[portNum].serialParameters;
        }
        else /* CPU port */
        {
             regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                                                    cpuPortRegs.serialParameters;
        }

        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 4, ipg);
    }
}

/****************************************************************************
* cpssDxChPortIpgGet
*
* DESCRIPTION:
*       Gets the Inter-Packet Gap (IPG) interval of a tri-speed physical port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*
* OUTPUTS:
*       ipgPtr      - (pointer to) IPG value in bytes
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on wrong port type
*       GT_BAD_PARAM     - on bad parameter
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIpgGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT  GT_U32  *ipgPtr
)
{
    GT_U32 regAddr; /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(ipgPtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* get */
    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,portMacType,&regAddr);

        return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 6, 9, ipgPtr);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portNum,&regAddr);

        return prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 4, ipgPtr);
    }
}

/*******************************************************************************
* cpssDxChPortExtraIpgSet
*
* DESCRIPTION:
*       Sets the number of 32-bit words to add to the 12-byte IPG.
*       Hence, 12+4*ExtraIPG is the basis for the entire IPG calculation.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       number   -  number of words
*                   for DxCh,DxCh2  : number range 0..3
*                   DxCh3 and above : number range 0..127
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - the number is out of range
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortExtraIpgSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_U8       number
)
{
    GT_U32 value;           /* value to write into the register */
    GT_U32  regAddr;    /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 offset;      /* bit number inside register       */
    GT_U32 sizeOfValue; /* number of bits to write to the register       */
    GT_STATUS rc;       /*the return value*/
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    value = number;

    /* set */
    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {
        if(value >= BIT_7)
        {
            return GT_OUT_OF_RANGE;
        }
        sizeOfValue=7;
        offset=6;
    }
    else
    {
        if(value >= BIT_2)
        {
            return GT_OUT_OF_RANGE;
        }
        sizeOfValue=2;
        offset=4; 
    }

    for(portMacType = PRV_CPSS_PORT_XG_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
    {
        if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
        {
           PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,portMacType,&regAddr);
        }
        else
        {
           PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
        }

      if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
      {
          rc= prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,offset, sizeOfValue, value);

          if (rc!=GT_OK) 
          {
              return rc;
          }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortExtraIpgGet
*
* DESCRIPTION:
*       Gets the number of 32-bit words to add to the 12-byte IPG.
*       Hence, 12+4*ExtraIPG is the basis for the entire IPG calculation.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       numberPtr -  pointer to number of words
*                   for DxCh,DxCh2  : number range 0..3
*                   DxCh3 and above : number range 0..127
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - the number is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortExtraIpgGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_U8       *numberPtr
)
{
    GT_U32 value;           /* value to write into the register */
    GT_U32  regAddr;
    GT_STATUS rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(numberPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {

        PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,portMacType,&regAddr);
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,6, 7, &value);
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,4, 2, &value);
    }
    if (rc != GT_OK)
        return rc;

    *numberPtr = (GT_U8)value;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortXgmiiLocalFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected local
*       fault messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
* OUTPUTS:
*       isLocalFaultPtr - GT_TRUE, if bit set or
*                         GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgmiiLocalFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *isLocalFaultPtr
)
{
    GT_U32 value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(isLocalFaultPtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                perPortRegs[portNum].macStatus,
                2, 1, &value) != GT_OK)
        return GT_HW_ERROR;

    *isLocalFaultPtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortXgmiiRemoteFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected remote
*       fault messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number
*
* OUTPUTS:
*       isRemoteFaultPtr  - GT_TRUE, if bit set or
*                           GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgmiiRemoteFaultGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *isRemoteFaultPtr
)
{
    GT_U32 value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(isRemoteFaultPtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum,PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                perPortRegs[portNum].macStatus,
                1, 1, &value) != GT_OK)
        return GT_HW_ERROR;

    *isRemoteFaultPtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacStatusGet
*
* DESCRIPTION:
*       Reads bits that indicate different problems on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portMacStatusPtr  - info about port MAC
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacStatusGet
(
    IN  GT_U8                      devNum,
    IN  GT_U8                      portNum,
    OUT CPSS_PORT_MAC_STATUS_STC   *portMacStatusPtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(portMacStatusPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].macStatus;

    if (prvCpssDrvHwPpPortGroupReadRegister(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, &value) != GT_OK)
        return GT_HW_ERROR;

    /*  10 Gb interface */
    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        portMacStatusPtr->isPortRxPause  =    (value & 0x40) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortTxPause  =    (value & 0x80) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortBackPres =    GT_FALSE;
        portMacStatusPtr->isPortBufFull  =    (value & 0x100) ? GT_TRUE : GT_FALSE;/* Bit#8 */
        portMacStatusPtr->isPortSyncFail =    (value & 0x1) ? GT_FALSE : GT_TRUE; /* value 0 -> SyncFail */
        portMacStatusPtr->isPortHiErrorRate = GT_FALSE;
        portMacStatusPtr->isPortAnDone =      GT_FALSE;
        portMacStatusPtr->isPortFatalError =  (value & 0x6) ? GT_TRUE : GT_FALSE; /* Bit#1 Or Bit#2 */
    }
    else
    {
        portMacStatusPtr->isPortRxPause = (value & 0x40) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortTxPause = (value & 0x80) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortBackPres = (value & 0x100) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortBufFull = (value & 0x200) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortSyncFail = (value & 0x400) ? GT_TRUE : GT_FALSE;
        portMacStatusPtr->isPortAnDone = (value & 0x800) ? GT_TRUE : GT_FALSE;

        portMacStatusPtr->isPortHiErrorRate = GT_FALSE;
        portMacStatusPtr->isPortFatalError = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInternalLoopbackEnableSet
*
* DESCRIPTION:
*       Set the internal Loopback state in the packet processor MAC port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       LoopBack is enabled only on XG devices.
*       For port 25 in DX269 (XG/HX port) it's the application responsibility to
*       reconfigure the loopback mode after switching XG/HX mode.
*
*******************************************************************************/
GT_STATUS cpssDxChPortInternalLoopbackEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN  GT_BOOL     enable
)
{
    GT_U32 regAddr;               /* register address                    */
    GT_U32 value;                 /* value to write into the register    */
    GT_U32 portState;             /* current port state (enable/disable) */
    GT_U32 fieldOffset;           /* bit field offset */
    GT_U32 fieldLength;           /* number of bits to be written to register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* disable port if we need */
    if (prvPortDisableSet(devNum,portNum, &portState) != GT_OK)
        return GT_HW_ERROR;
 
#ifdef __AX_PLATFORM__
	 for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
	 {
#else
	 portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
		 if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
			 continue;
#else
			 return GT_OK;
#endif

    
        if(portMacType >= PRV_CPSS_PORT_XG_E)
        /* Set MAC loopback mode */
        {
            value = (enable == GT_TRUE) ? 1 : 0;
            if (isPortInXgMode (devNum, portNum) == GT_FALSE)  /* HX port */
            {
                if( (25 != portNum) && (26 != portNum) )
#ifdef __AX_PLATFORM__
								 continue;
#else
								 return GT_OK;
#endif


                /* First reset the HX port PCS */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs->
                    hxPortConfig0[portNum-25];
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 11, 1, 1) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
    
                /* Now set the MACLoopBackEn field */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs->
                    hxPortConfig1[portNum-25];
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 7, 1, value) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
    
                /* Finally set the HX PCS bit to "not reset" */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs->
                    hxPortConfig0[portNum-25];
                if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 11, 1, 0) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
#ifdef __AX_PLATFORM__
							 continue;
#else
							 return GT_OK;
#endif

            }
    
            fieldOffset = 13;
            fieldLength = 1;
            value = (enable == GT_TRUE) ? 1 : 0;
        }
        else
        /* Set loopback on Serial and PSC Block. */
        {
            fieldOffset = 5;
            fieldLength = 2;
            value = (enable == GT_TRUE) ? 3 : 0;
        }

        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, fieldOffset, fieldLength,
                                                                       value) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
#ifdef __AX_PLATFORM__
     }
#endif

    /* enable port if we need */
    if (prvPortEnableSet(devNum,portNum, portState) != GT_OK)
        return GT_HW_ERROR;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInternalLoopbackEnableGet
*
* DESCRIPTION:
*       Get Internal Loopback
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       enablePtr  - Pointer to the Loopback state.
*                    If GT_TRUE, loopback is enabled
*                    If GT_FALSE, loopback is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not relevant for the CPU port.
*
*******************************************************************************/
GT_STATUS cpssDxChPortInternalLoopbackEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 portState;       /* current port state (enable/disable) */
    GT_STATUS status;
    GT_U32 fieldOffset;           /* bit field offset */
    PRV_CPSS_PORT_TYPE_ENT portMacType;


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);

    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        fieldOffset = 13;
    }
    else
    {
        fieldOffset = 5;
    }

    /* store value of port state */
    status = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, fieldOffset, 1, &portState);
    if (status != GT_OK)
    {
        return GT_HW_ERROR;
    }
    *enablePtr = (portState == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInbandAutoNegEnableSet
*
* DESCRIPTION:
*       Configure Auto-Negotiation mode of MAC for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                 GT_FALSE - Auto-Negotiation works in out-of-band via
*                            the device's Master SMI interface mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - The feature is not supported the port/device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not supported for CPU port of DX devices and not supported for XG ports.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*       The change of the Auto-Negotiation causes temporary change of the link
*       to down and up for ports with link up.
*
*       Not relevant for the CPU port
*
*******************************************************************************/
GT_STATUS cpssDxChPortInbandAutoNegEnableSet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    IN  GT_BOOL  enable
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 portState;       /* current port state (enabled/disabled) */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        /* Inband auto-negotiation is not supported on XG ports */
        return GT_NOT_SUPPORTED;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);


    /* store value of port state */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &portState) != GT_OK)
        return GT_HW_ERROR;

    /* Disable port if it's enabled */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
            return GT_HW_ERROR;
    }

    value = (enable == GT_TRUE) ? 1 : 0;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
              perPortRegs[portNum].autoNegCtrl;

    /* Set inband auto-negotiation */
    if (prvCpssDrvHwPpPortGroupSetRegField (devNum, portGroupId,regAddr, 2, 1, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Restore original port state */
    if (portState == 1)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 1) != GT_OK)
            return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInbandAutoNegEnableGet
*
* DESCRIPTION:
*       Gets Auto-Negotiation mode of MAC for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                    GT_FALSE - Auto-Negotiation works in out-of-band via
*                            the device's Master SMI interface mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - The feature is not supported the port/device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not supported for CPU port of DX devices and not supported for XG ports.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*
*       Not relevant for the CPU port
*
*******************************************************************************/
GT_STATUS cpssDxChPortInbandAutoNegEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        /* Inband auto-negotiation is not supported on XG ports */
        return GT_NOT_SUPPORTED;
    }
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
              perPortRegs[portNum].autoNegCtrl;

    /* store value of port state */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 2, 1, &value);
    if (rc != GT_OK)
    {
        return GT_HW_ERROR;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortAttributesOnPortGet
*
* DESCRIPTION:
*       Gets port attributes for particular logical port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (or CPU port)
*
* OUTPUTS:
*       portAttributSetArrayPtr - Pointer to attributes values array.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortAttributesOnPortGet
(
    IN    GT_U8                     devNum,
    IN    GT_U8                     portNum,
    OUT   CPSS_PORT_ATTRIBUTES_STC  *portAttributSetArrayPtr
)
{
    GT_BOOL              portLinkUp;      /* port attributes */
    CPSS_PORT_SPEED_ENT  portSpeed;       /* port attributes */
    CPSS_PORT_DUPLEX_ENT portDuplexity;   /* port attributes */
    GT_STATUS            rc;              /* return code     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(portAttributSetArrayPtr);

    /* get port attributes */

    rc = cpssDxChPortLinkStatusGet(devNum, portNum, &portLinkUp);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortSpeedGet(devNum, portNum, &portSpeed);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortDuplexModeGet(devNum ,portNum ,&portDuplexity);
    if (rc != GT_OK)
    {
        return rc;
    }

    portAttributSetArrayPtr->portLinkUp     = portLinkUp;
    portAttributSetArrayPtr->portSpeed      = portSpeed;
    portAttributSetArrayPtr->portDuplexity  = portDuplexity;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortPreambleLengthSet      
*
* DESCRIPTION:
*       Set the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction and "both directions"
*                      options (GE ports support only Tx direction)
*       length       - length of preamble in bytes
*                      support only values of 4,8
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPreambleLengthSet(
    IN GT_U8                    devNum,
    IN GT_U8                    portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
)
{
    GT_U32  regAddr;/* register address to set */
    GT_U32  offset = 0;/* start offset in register to set */
    GT_U32  fieldLen;/* number of bits to set */
    GT_U32  value;/* value to set to bits */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */
    GT_STATUS rc = GT_OK;
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if((length != 4) && (length != 8))
    {
        return GT_BAD_PARAM;
    }

    /* XG and up accept all directions , GE and down only accepts TX*/
    if (((direction != CPSS_PORT_DIRECTION_TX_E) &&  (portMacType < PRV_CPSS_PORT_XG_E)) ||
        (direction > CPSS_PORT_DIRECTION_BOTH_E) ||   (direction < CPSS_PORT_DIRECTION_RX_E) )
    {
        return GT_BAD_PARAM;
    }

    fieldLen = 1;/* 1 bit to set*/

    /** change all mac units,  */
#ifdef __AX_PLATFORM__
	for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
	{
#else
	portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
		if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
			continue;
#else
			return GT_OK;
#endif


        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            value = (length == 8) ? 1 : 0;

            switch(direction)
            {
                case CPSS_PORT_DIRECTION_RX_E:
                    offset = 0;/* rx bit */
                    break;
                case CPSS_PORT_DIRECTION_TX_E:
                    offset = 1;/* tx bit */
                    break;
                case CPSS_PORT_DIRECTION_BOTH_E:
                    fieldLen = 2;/* 2 bits to set*/
                    offset = 0;/* start from rx bit */
                    value |= (value << 1);/* set 2 bits instead of 1 ,  1 => 11 , 0 =>00 */
                    break;
            }
            /* MAC Control 0 */
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
            offset += (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 11 : 12;
        }
        else
        {
            if (direction != CPSS_PORT_DIRECTION_RX_E)  /* change only TX (even for BOTH) */
            {
                value = (length == 8) ? 0 : 1;
                offset = 15;

                /* MAC Control 1 */
                PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
            }
            else
            {
#ifdef __AX_PLATFORM__
				continue;
#else
				return GT_OK;
#endif

            }
        }

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, offset, fieldLen, value);

            if (rc != GT_OK) 
            {
                return rc;
            }
        }
#ifdef __AX_PLATFORM__
    }
#endif

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPreambleLengthSet
*
* DESCRIPTION:
*       Set the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction and "both directions"
*                      options (GE ports support only Tx direction)
*       length       - length of preamble in bytes
*                      support only values of 4,8
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPreambleLengthSet(
    IN GT_U8                    devNum,
    IN GT_U8                    portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
)
{
    CPSS_PORT_DIRECTION_ENT  targetDirection = direction;
     GT_U32                   targetLength = length;

    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if((length != 4) && (length != 8))
    {
        return GT_BAD_PARAM;
    }

    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPreambleLengthSetFunc(devNum,portNum,
                                  targetDirection,targetLength,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetDirection,
                                  &targetLength);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
      /* set switch MAC Port Preamble Length */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortPreambleLengthSet(devNum,portNum,
                                              targetDirection,targetLength);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPreambleLengthSetFunc(devNum,portNum,
                                  targetDirection,targetLength,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetDirection,
                                  &targetLength);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPreambleLengthGet       
*
* DESCRIPTION:
*       Get the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction
*                      GE ports support only Tx direction.
*
*
* OUTPUTS:
*       lengthPtr    - pointer to preamble length in bytes :
*                      supported length values are : 4,8.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPreambleLengthGet(
    IN  GT_U8                    devNum,
    IN  GT_U8                    portNum,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
    OUT GT_U32                   *lengthPtr
)
{
    GT_U32  regAddr;/* register address to set */
    GT_U32  offset;/* start offset in register to set */
    GT_U32  value;/* value to set to bits */
    GT_BOOL XGport = GT_FALSE;
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portNum != CPSS_CPU_PORT_NUM_CNS && portMacType >= PRV_CPSS_PORT_XG_E)
    {
        XGport = GT_TRUE;
        /* MAC Control 0 */
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);

        switch(direction)
        {
        case CPSS_PORT_DIRECTION_RX_E:
            offset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 11 : 12;/* rx bit */
            break;
        case CPSS_PORT_DIRECTION_TX_E:
            offset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 12 : 13;/* tx bit */
            break;
        default:
            return GT_BAD_PARAM;
        }
    }
    else
    {
        switch(direction)
        {
            case CPSS_PORT_DIRECTION_TX_E:
                break;
            default:
                return GT_BAD_PARAM;
        }
        /* MAC Control 1 */
        PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);
        offset = 15;
    }

    if(prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, offset, 1, &value) != GT_OK)
        return GT_HW_ERROR;


    if (value == 0)
    {
        *lengthPtr = (XGport == GT_TRUE) ? 4 : 8;
    }
    else /* value == 1*/
    {
        *lengthPtr = (XGport == GT_TRUE) ? 8 : 4;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPreambleLengthGet
*
* DESCRIPTION:
*       Get the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction
*                      GE ports support only Tx direction.
*
*
* OUTPUTS:
*       lengthPtr    - pointer to preamble length in bytes :
*                      supported length values are : 4,8.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPreambleLengthGet(
    IN  GT_U8                    devNum,
    IN  GT_U8                    portNum,
    IN  CPSS_PORT_DIRECTION_ENT  direction,
      OUT GT_U32                   *lengthPtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   
    CPSS_PORT_DIRECTION_ENT  targetDirection = direction;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(lengthPtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPreambleLengthGetFunc(devNum,portNum,
                                  direction,lengthPtr,
                                  CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetDirection,
                                  lengthPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get switch MAC Port Preamble Length value  */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortPreambleLengthGet(devNum,portNum,targetDirection,lengthPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPreambleLengthGetFunc(devNum,portNum,
                                  targetDirection,lengthPtr,
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetDirection,
                                  lengthPtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}


/*******************************************************************************
* cpssDxChPortMacSaBaseSet
*
* DESCRIPTION:
*       Sets the Source addresses of all of the device's ports.
*       This addresses is used as the MAC SA for Flow Control Packets
*       transmitted by the device.In addition this address is used for
*       reception of Flow Control packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       macPtr - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaBaseSet
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *macPtr
)
{
    GT_U32  macMiddle;          /* Holds bits 8 - 15 of mac     */
    GT_U32  macHigh;            /* Holds bits 16 - 47 of mac    */
    GT_STATUS rc;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(macPtr);

    macMiddle   =  macPtr->arEther[4];
    macHigh     = (macPtr->arEther[3]        |
                  (macPtr->arEther[2] << 8)  |
                  (macPtr->arEther[1] << 16) |
                  (macPtr->arEther[0] << 24));

    rc = prvCpssDrvHwPpSetRegField(devNum,
         PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.srcAddrMiddle,0,8,macMiddle);
    if( rc != GT_OK )
        return rc;

    return prvCpssDrvHwPpWriteRegister(devNum,
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.srcAddrHigh,macHigh);
}

/*******************************************************************************
* cpssDxChPortMacSaBaseGet
*
* DESCRIPTION:
*       Sets the Source addresses of all of the device's ports.
*       This addresses is used as the MAC SA for Flow Control Packets
*       transmitted by the device.In addition this address is used for
*       reception of Flow Control packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       macPtr - (pointer to)The system Mac address.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacSaBaseGet
(
    IN  GT_U8           devNum,
    OUT GT_ETHERADDR    *macPtr
)
{
    GT_STATUS status = GT_OK;
    GT_U32  macMiddle;          /* Holds bits 8 - 15 of mac     */
    GT_U32  macHigh;            /* Holds bits 16 - 47 of mac    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(macPtr);

    status = prvCpssDrvHwPpGetRegField(devNum,
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.srcAddrMiddle,0,8,&macMiddle);
    if(status != GT_OK)
        return status;

    status = prvCpssDrvHwPpReadRegister(devNum,
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.srcAddrHigh,&macHigh);
    if(status != GT_OK)
        return status;

    macPtr->arEther[3] = (GT_U8)(macHigh & 0xFF);
    macPtr->arEther[2] = (GT_U8)((macHigh & 0xFF00) >> 8);
    macPtr->arEther[1] = (GT_U8)((macHigh & 0xFF0000) >> 16);
    macPtr->arEther[0] = (GT_U8)((macHigh & 0xFF000000) >> 24);
    macPtr->arEther[4] = (GT_U8)macMiddle;

    return GT_OK;
}



/***********internal functions*************************************************/

/*******************************************************************************
* prvPortDisableSet
*
* DESCRIPTION:
*       Disable a specified port on specified device.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portStatePtr - (pointer to) the state (en/dis) of port before calling this function
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvPortDisableSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    OUT GT_U32  *portStatePtr
)
{
    GT_U32 regAddr;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* store value of port state */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, portStatePtr) != GT_OK)
        return GT_HW_ERROR;

    /* disable port if we need */
    if (*portStatePtr == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
            return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* prvPortEnableSet
*
* DESCRIPTION:
*       Enable a specified port on specified device.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       portState - the state (en/dis) of port before calling the port disable
*                   function
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvPortEnableSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U32 portState
)
{
    GT_U32 regAddr;
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    /* enable port if we need */
    if (portState == 1)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);

        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 0, 1, 1) != GT_OK)
            return GT_HW_ERROR;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3PortXgPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified XG port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       speed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3PortXgPortSpeedSet
(
    IN  GT_U8                     dev,
    IN  GT_U8                     port,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_BOOL oldPowerUp;             /* XAUI serdes lanes old power up state */
    GT_U32  serdesSpeedRegAddr;     /* Serdes Speed register address */
    GT_U32  fieldValue;             /* register field value */
    GT_U32  lane;                   /* iterator */
    GT_STATUS rc;                   /* return status */

    switch (speed)
    {

        case CPSS_PORT_SPEED_16000_E:
        case CPSS_PORT_SPEED_10000_E:
            /* DxCh3 XG ports < 24 support 10G only */
            if ((speed == CPSS_PORT_SPEED_16000_E) && (port < 24))
            {
                return GT_NOT_SUPPORTED;
            }
            if(speed == CPSS_PORT_SPEED_10000_E)
            {
                /* the speed for ports < 24 cannot be changed*/
                if(port < 24)
                {
                    return GT_OK;
                }

                fieldValue = 0x8;
            }
            else
            {
                fieldValue = 0xE;
            }

            /* Configure XAUI serdes lanes power Down */
            rc = prvCpssDxCh3PortSerdesPowerUpDownSet(dev, port, GT_FALSE,
                                                      &oldPowerUp);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Configure XAUI speeds */

            for(lane = 0; lane < 4; lane++)
            {
                serdesSpeedRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                          macRegs.perPortRegs[port].serdesSpeed1[lane];
                rc = prvCpssDrvHwPpSetRegField(dev, serdesSpeedRegAddr,
                                                 2, 7, fieldValue);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            if(oldPowerUp == GT_TRUE)
            {
               return prvCpssDxCh3PortSerdesPowerUpDownSet(dev, port, GT_TRUE,
                                                           &oldPowerUp);
            }

            break;
        default:
            return GT_NOT_SUPPORTED;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3PortXgPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified XG port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - the port speed can't be determined.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3PortXgPortSpeedGet
(
    IN  GT_U8                      dev,
    IN  GT_U8                      port,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_U32 regAddr;          /* register address */
    GT_U32 fieldValue;       /* register field value */
    GT_STATUS rc;            /* return status */


    /* DxCh3 XG ports < 24 support 10G only */
    if(port < 24)
    {
        *speedPtr = CPSS_PORT_SPEED_10000_E;
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
              macRegs.perPortRegs[port].serdesSpeed1[0];

    rc = prvCpssDrvHwPpGetRegField(dev, regAddr, 2, 7, &fieldValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch(fieldValue)
    {
        case 0x8:
            *speedPtr = CPSS_PORT_SPEED_10000_E;
            break;
        case 0xE:
            *speedPtr = CPSS_PORT_SPEED_16000_E;
            break;
        default:
            *speedPtr = CPSS_PORT_SPEED_NA_E;
            break;
    }
    return GT_OK;
}

/*******************************************************************************
* setXgPortSpeed
*
* DESCRIPTION:
*       Sets speed for specified XG/HX port on specified device.
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       speed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS setXgPortSpeed
(
    IN  GT_U8                dev,
    IN  GT_U8                port,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_U32 regAddr;
    GT_U32 regValue;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);

    switch (speed)
    {
        case CPSS_PORT_SPEED_2500_E:
        case CPSS_PORT_SPEED_5000_E:
            if (PRV_CPSS_DXCH_XG_PORT_TYPE_MAC(dev,port) ==
                PRV_CPSS_XG_PORT_XG_ONLY_E)
            {
                return GT_NOT_SUPPORTED;
            }
            else if ((PRV_CPSS_DXCH_XG_PORT_TYPE_MAC(dev,port) ==
                      PRV_CPSS_XG_PORT_XG_HX_QX_E) &&
                     (isPortInXgMode (dev, port) == GT_TRUE))
            {
                /* change interface to HX */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                    macRegs.hxPortGlobalConfig;
                if (prvCpssDrvHwPpPortGroupSetRegField(dev, portGroupId,regAddr, 15, 1, 0) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
            }

            if( (25 != port) && (26 != port) )
            {
                return GT_FAIL;
            }
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->macRegs.perPortRegs->
                hxPortConfig0[port-25];
            regValue = (speed == CPSS_PORT_SPEED_2500_E) ? 0 : 1;
            /* Reset the HX port PCS */
            if (prvCpssDrvHwPpPortGroupSetRegField(dev, portGroupId,regAddr, 11, 1, 1) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            /* Set HX port mode according to the required speed */
            if (prvCpssDrvHwPpPortGroupSetRegField(dev, portGroupId,regAddr, 5, 1, regValue) !=
                GT_OK)
            {
                return GT_HW_ERROR;
            }
            /* Set the HX port PCS to 0 */
            if (prvCpssDrvHwPpPortGroupSetRegField(dev, portGroupId,regAddr, 11, 1, 0) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            break;

        case CPSS_PORT_SPEED_10000_E:
        case CPSS_PORT_SPEED_12000_E:
        case CPSS_PORT_SPEED_16000_E:
            if((PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E) &&
                ((speed == CPSS_PORT_SPEED_10000_E) ||
                    (speed == CPSS_PORT_SPEED_16000_E)))
            {
                    return prvCpssDxCh3PortXgPortSpeedSet(dev, port, speed);
            }
            /* Port speed 16G is supported for DxCh3 */
            if(speed == CPSS_PORT_SPEED_16000_E)
            {
                return GT_NOT_SUPPORTED;
            }

            if (PRV_CPSS_DXCH_XG_PORT_TYPE_MAC(dev,port) ==
                PRV_CPSS_XG_PORT_HX_QX_ONLY_E)
            {
                return GT_NOT_SUPPORTED;
            }
            else if (PRV_CPSS_DXCH_XG_PORT_TYPE_MAC(dev,port) ==
                     PRV_CPSS_XG_PORT_XG_HX_QX_E)
            {
                if (isPortInXgMode (dev, port) == GT_FALSE)
                {
                    /* change interface to XG */
                    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                        macRegs.hxPortGlobalConfig;
                    if (prvCpssDrvHwPpPortGroupSetRegField(dev, portGroupId,regAddr, 15, 1, 1) !=
                        GT_OK)
                    {
                        return GT_HW_ERROR;
                    }
                }
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                smiRegs.smi10GePhyConfig0[port];

            regValue = (speed == CPSS_PORT_SPEED_10000_E) ? 0 : 1;

            /* Reset the XAUI PHY */
            if (prvCpssDrvHwPpPortGroupSetRegField(dev, portGroupId,regAddr, 0, 1, 0) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            /* Set HyperG.Stack port speed */
            if (prvCpssDrvHwPpPortGroupSetRegField(dev, portGroupId,regAddr, 6, 2, regValue) !=
                GT_OK)
            {
                return GT_HW_ERROR;
            }
            /* Set the XAUI PHY to 1 */
            if (prvCpssDrvHwPpPortGroupSetRegField(dev, portGroupId,regAddr, 0, 1, 1) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            break;

        default:
            return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* setSgmiiPortSpeed
*
* DESCRIPTION:
*       Sets speed for specified .
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS setSgmiiPortSpeed
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS rc = GT_OK;
    GT_U32    regAddr;          /* register address */
    GT_U32    portState;        /* current port state (enable/disable) */
    GT_U32    gmiiOffset;
    GT_U32    miiOffset;
    GT_U32    portSpeedRegAddr;
    GT_U32    portGroupId;/*the port group Id - support multi-port-groups device */
    GT_BOOL   npSerdesSupported = GT_TRUE;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

    /* store value of port state */
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &portState) != GT_OK)
        return GT_HW_ERROR;

    /* disable port if we need */
    if (portState == 1)
    {
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0) != GT_OK)
            return GT_HW_ERROR;
    }

    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portNum,&portSpeedRegAddr);

    gmiiOffset = 6;
    miiOffset  = 5;

    switch (speed)
    {
        case CPSS_PORT_SPEED_10_E:
            if(prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,portSpeedRegAddr, gmiiOffset, 1, 0) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,portSpeedRegAddr, miiOffset, 1, 0);
            break;

        case CPSS_PORT_SPEED_100_E:
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,portSpeedRegAddr, gmiiOffset, 1, 0) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,portSpeedRegAddr, miiOffset, 1, 1);
            break;

        case CPSS_PORT_SPEED_1000_E:
        /* relevant for CH3 and XCAT A1,2 XCAT2 devices */
        case CPSS_PORT_SPEED_2500_E:

            if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E) 
                && (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT2_E))
            {
                npSerdesSupported = GT_FALSE;
            }
            if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E) 
                && (PRV_CPSS_PP_MAC(devNum)->revision == 0))
            {
                npSerdesSupported = GT_FALSE;
            }
            if((portNum == CPSS_CPU_PORT_NUM_CNS)
                || (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
                || (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E))
            {
                npSerdesSupported = GT_FALSE;
            }

            if(npSerdesSupported)
            {
                if((rc = setNetworkPortSpeedLpSerdes(devNum,portNum,speed)) != GT_OK)
                    return rc;
            }

            /* enable/disable XCAT specific for SGMII 2.5G on stack ports configurations */
            if(((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
                || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
                    && (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;
                if(speed == CPSS_PORT_SPEED_2500_E)
                {
                    if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                                22, 1, 1)) != GT_OK)
                        return rc;
            
                    if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                                14, 1, 0)) != GT_OK)
                        return rc;

                    /* set inband autoneg mode 1000Based-X, although ifMode is SGMII */
                    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum, portNum, 
                                                    PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);
                    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 0, 1, 0);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                else
                {
                    if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                                22, 1, 0)) != GT_OK)
                        return rc;
            
                    if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                                14, 1, 1)) != GT_OK)
                        return rc;
                }
            }

            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,portSpeedRegAddr, gmiiOffset, 1, 1);

            break;

        default:
            break;
    }

    /* enable port if we need */
    if (portState == 1)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 1) != GT_OK)
            return GT_HW_ERROR;
    }

    return rc;
}


/*******************************************************************************
* prvCpssDxChPortGigInterfaceModeSet
*
* DESCRIPTION:
*       Sets Interface mode on gig port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - Interface mode.For CPU port the interface may be one of
*                   GMII, RGMII, MII and MII PHY. For Tri-Speed ports
*                   the interface may be SGMII or 1000 Base X.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - RGMII and MII PHY port interface modes
*                           are not supported for DxCh3 CPU port.
*
* COMMENTS:
*     None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortGigInterfaceModeSet
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_U32     regAddr;              /* register address */
    GT_U32     forceLinkDownRegAddr; /* register address */
    GT_U32     macCnrl2RegAddr;      /* register address */
    GT_U32     value;          /* value to write into the register */
    GT_U32     useIntClkForEn = 0; /* Must be set for CPU port according to ifMode */
    GT_U32     fieldLength;    /* The number of bits to be written to register.*/
    GT_U32     linkDownStatus; /* Port link down status */
    GT_STATUS  rc;             /* return code     */
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        switch(ifMode)
        {
            case CPSS_PORT_INTERFACE_MODE_MII_E:
                value = 0;
                useIntClkForEn = 0;
                break;
            case CPSS_PORT_INTERFACE_MODE_GMII_E:
                value = 1;
                useIntClkForEn = 0;
                break;
            case CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E:
            case CPSS_PORT_INTERFACE_MODE_MII_PHY_E:
                /* RGMII and MII PHY are not supported for DxCh3 CPU Port */
                if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
                {
                    return GT_NOT_SUPPORTED;
                }
                if(ifMode == CPSS_PORT_INTERFACE_MODE_REDUCED_GMII_E)
                {
                    value = 2;
                    useIntClkForEn = 1;
                }
                /* CPSS_PORT_INTERFACE_MODE_MII_PHY_E */
                else
                {
                    value = 3;
                    useIntClkForEn = 1;
                }
                break;
            default:
                return GT_BAD_PARAM;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.cpuPortCtrlReg;
        fieldLength = 2;
    }
    else
    {
        switch(ifMode)
        {
            case CPSS_PORT_INTERFACE_MODE_QSGMII_E:
            case CPSS_PORT_INTERFACE_MODE_SGMII_E:
                value = 0;
                break;
            case CPSS_PORT_INTERFACE_MODE_1000BASE_X_E:
                value = 1;
                break;
            default:
                return GT_NOT_SUPPORTED;
        }

        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);
        fieldLength = 1;
    }

    /* Get force link down status */
    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum, portNum,
                                             &forceLinkDownRegAddr);
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,forceLinkDownRegAddr, 0, 1,
                                   &linkDownStatus);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(linkDownStatus == 0)
    {
        /* Set force link down */
        rc = cpssDxChPortForceLinkDownEnableSet(devNum,portNum,GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

    }

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&macCnrl2RegAddr);
        /* Must be set for CPU port according to ifMode */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,macCnrl2RegAddr, 4, 1,
                                       useIntClkForEn);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Set interface mode */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 1, fieldLength, value);
    if(rc != GT_OK)
    {
        return rc;
    }

    if((ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) || 
        (ifMode == CPSS_PORT_INTERFACE_MODE_QSGMII_E) ||
            (ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E))
    {
        /* qsgmii also uses 1000BaseX inband auto-neg mode */
        value = (ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) ? 1 : 0;
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum, portNum, PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&macCnrl2RegAddr);
        /* set inband autoneg mode accordingly to new ifMode */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, macCnrl2RegAddr, 0, 1, value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if((GT_TRUE != PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum, portNum))
        && PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;
        if(ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E)
        {
            /* Enable SGMII mode in Network Port */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,15+portNum/4,1,1);
            if(rc != GT_OK)
            {
                return rc;
            }
            /* for SGMII 1G doesn't matter if network_ports_dp_sync_fifo_bypass enabled or disabled */
        }
        else if(ifMode == CPSS_PORT_INTERFACE_MODE_QSGMII_E)
        {
            /* clear network_ports_sgmii_en for port */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,15+portNum/4,1,0);
            if(rc != GT_OK)
            {
                return rc;
            }
            /* enable network_ports_dp_sync_fifo_bypass */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,13,1,1);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        HW_WAIT_MILLISECONDS_MAC(10);
    }

    if(linkDownStatus == 0)
    {
        /* Set force link down to previous status */
        rc = cpssDxChPortForceLinkDownEnableSet(devNum,portNum,GT_FALSE);
    }

    /* save new interface mode in DB */
    if(portNum != CPSS_CPU_PORT_NUM_CNS)
        PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum, portNum) = ifMode;

    return rc;
}

/*******************************************************************************
* prvGetLpSerdesSpeed
*
* DESCRIPTION:
*       Gets LP serdes speed.
*
* INPUTS:
*       devNum - physical device number
*       portGroupId - port group Id, support multi-port-groups device
*       serdesNum - serdes number
*
* OUTPUTS:
*       serdesSpeedPtr - (pointer to) serdes speed
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong speed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvGetLpSerdesSpeed
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               serdesNum,
    OUT PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT  *serdesSpeedPtr
)
{
    GT_U32 regAddr;
    GT_U32 regValue,regValue2;
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed;
    GT_STATUS rc = GT_OK;
    GT_U32 firstInitializedSerdes;
    GT_U32 (*lpSerdesConfigArr)[PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E];
    GT_U32 (*lpSerdesExtConfigArr)[2];

    rc = serDesConfig(devNum,&lpSerdesConfigArr,&lpSerdesExtConfigArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChLpGetFirstInitSerdes(devNum,portGroupId,serdesNum,&firstInitializedSerdes);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* get PLL/INTP Register2 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        serdesConfig[firstInitializedSerdes].pllIntpReg2;
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &regValue);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* get PLL/INTP Register3 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        serdesConfig[firstInitializedSerdes].pllIntpReg3;
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &regValue2);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (serdesSpeed=PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E;
          serdesSpeed < PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E; serdesSpeed++)
    {
        if ((regValue == lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E][serdesSpeed]) &&
            ((regValue2 & 0xFFF)  == (lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_0_E][serdesSpeed] & 0xFFF)))
            break;
    }

#ifdef ASIC_SIMULATION
    if(serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {/* RXAUI */
            serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_6_25_E;
        }
        else
        {
            /*XGMII*/
            serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E;
        }
    }
#endif /*ASIC_SIMULATION*/

    *serdesSpeedPtr = serdesSpeed;
    return rc;
}

/*******************************************************************************
* setFlexLinkPortSpeedLpSerdes
*
* DESCRIPTION:
*       Sets speed for flex link port on specified device.
*
* INPUTS:
*       devNum - physical device number
*       portNum - physical port number
*       speed  - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong speed
*       GT_BAD_STATE     - if serdes speed of neighbor port defined as special isn't
*                           equal to required for even port while it needs to use
*                           serdeses of special port for traffic
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS setFlexLinkPortSpeedLpSerdes
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_U32 regAddr;
    GT_U32 regValue;
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;
    GT_STATUS rc;
    GT_U32 i;
    GT_U32 startSerdes = 0, serdesesNum = 0;
    GT_U8  tmpStartSerdes = 0, tmpSerdesesNum = 0;
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E;
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeedPrev;
    GT_U32      portGroupId;     /*the port group Id - support multi-port-groups device */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    GT_U32      globalPort;
    GT_U32 (*lpSerdesConfigArr)[PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E];
    GT_U32 (*lpSerdesExtConfigArr)[2];
    GT_U32   fieldOffset;
    
    rc = serDesConfig(devNum,&lpSerdesConfigArr,&lpSerdesExtConfigArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        return GT_NOT_SUPPORTED;
    }

    serdesSpeed = serdesFrequency[ifMode][speed];
    if(serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&tmpStartSerdes,&tmpSerdesesNum)) != GT_OK)
        return rc;
    else
    {
        /* to ensure correct assignment in BE mode */
        startSerdes = ((GT_U32)tmpStartSerdes) & 0xFF;
        serdesesNum = ((GT_U32)tmpSerdesesNum) & 0xFF;
    }

    /* check if all serdes in port are initialized and make proper initialization
       if it is needed */
    rc = prvCpssDxChLpCheckAndInitNeededSerdes(devNum,portGroupId,startSerdes,serdesesNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {/* if neighbor port defined as special port need check if serdes frequency defined on
      * his serdeses equal to required for even port */
        if((ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (serdesesNum == 2))
        {/* number of serdeses less then needed for this mode i.e. neighbor is special port */
            rc = prvGetLpSerdesSpeed(devNum,portGroupId,startSerdes+2,&serdesSpeedPrev);
            if (rc != GT_OK)
            {
                return rc;
            }
            if(serdesSpeedPrev != serdesSpeed)
                return GT_BAD_STATE;
        }

        /* if special mode required and port is odd, check that even port configuration compatible to required */
        if((ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E) && ((localPort%2) != 0))
        {
            globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,localPort-1);
            if(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,globalPort) == CPSS_PORT_INTERFACE_MODE_XGMII_E)
            {
                rc = prvGetLpSerdesSpeed(devNum,portGroupId,startSerdes-2,&serdesSpeedPrev);
                if (rc != GT_OK)
                {
                    return rc;
                }
                if(serdesSpeedPrev != serdesSpeed)
                    return GT_BAD_STATE;
                else
                    return GT_OK; /* needed speed already configured */
            }
        }
    }

    rc = prvGetLpSerdesSpeed(devNum,portGroupId,startSerdes,&serdesSpeedPrev);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* perform power down only if serdes speed changed */
    if (serdesSpeedPrev != serdesSpeed)
    {
        /* power down all serdes */
        for (i = startSerdes; i < startSerdes + serdesesNum;i++)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                serdesConfig[i].powerReg;
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* set PLL/INTP Register2 and PLL/INTP Register3*/
        for (i = startSerdes; i < startSerdes + serdesesNum;i++)
        {
            regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E][serdesSpeed];
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                serdesConfig[i].pllIntpReg2;
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            HW_WAIT_MILLISECONDS_MAC(20);

            regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_0_E][serdesSpeed];
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                serdesConfig[i].pllIntpReg3;
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            HW_WAIT_MILLISECONDS_MAC(20);
        }
    }

    if (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_GE_E)
    {
        rc = setSgmiiPortSpeed(devNum,portNum,speed);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        switch(speed)
        {
            case     CPSS_PORT_SPEED_10_E:
            case     CPSS_PORT_SPEED_100_E:
            case     CPSS_PORT_SPEED_1000_E:
            case     CPSS_PORT_SPEED_2500_E:
            case     CPSS_PORT_SPEED_5000_E:
            case     CPSS_PORT_SPEED_10000_E:
                regValue = 0;
                break;

            case     CPSS_PORT_SPEED_16000_E:
            case     CPSS_PORT_SPEED_20000_E:
                regValue = 0x15;
                break;

            case     CPSS_PORT_SPEED_40000_E:
                regValue = 0x17;
                break;

            default:
                return GT_NOT_SUPPORTED;
        }
        i = (localPort < 6) ? 0 : 1;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.txdmaThresholdOverrideEn[i];
        fieldOffset = (localPort%6)*5;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,fieldOffset,5,regValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(GT_FALSE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_LION_XLG_WS_SUPPORT_WA_E))
            return GT_OK;

        /* Metal_fix[31] - Enable Strict Priority to port10 (40Gbps)
            Metal_fix[30] - Port0 is 20 Gpbs
            Metal_fix[29] - Port2 is 20 Gpbs
            Metal_fix[28] - Port4 is 20 Gpbs
            Metal_fix[27] - Port6 is 20 Gpbs
            Metal_fix[26] - Port8 is 20 Gpbs
            Metal_fix[25] - Port10 is 20 Gpbs
            Metal_fix[24] - Enable fix
            NOTE: - When one of the bits Metal_fix[31:25] is SET, these 2 other configurations must be set as well:
            1. Metal_fix[24]
            2. Scheduler/Scheduler Configuration/Scheduler Configuration/DeficitEnable 
        */
        if(localPort%2 != 0)
            return GT_OK;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.global.debug.metalFix;
        if((speed == CPSS_PORT_SPEED_20000_E) || (speed == CPSS_PORT_SPEED_16000_E))
        {            
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,24,1,1);
            if (rc != GT_OK)
            {
                return rc;
            }
            if(localPort == 10)
            {/* disable metal fix for 40G */
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,31,1,0);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,(30-(localPort/2)),1,1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else if(speed == CPSS_PORT_SPEED_40000_E)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,24,1,1);
            if (rc != GT_OK)
            {
                return rc;
            }
            /* disable metal fix for 20G on ports 8 and 10 */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,25,2,0);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,31,1,1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            /* disable metal fix for 20G */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,(30-(localPort/2)),1,0);
            if (rc != GT_OK)
            {
                return rc;
            }

            if(localPort == 10)
            {/* disable metal fix for 40G */
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,31,1,0);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }

            /* check if metal fix can be disabled */
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,25,7,&regValue);
            if (rc != GT_OK)
            {
                return rc;
            }
            
            if(regValue == 0)
            {
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,24,1,1);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }

    }

    return GT_OK;
}

/*******************************************************************************
* setNetworkPortSpeedLpSerdes
*
* DESCRIPTION:
*       Sets speed SerDes of network port on specified device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       speed   - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong speed
*
* COMMENTS:
*       Applicable for xcat a2 devices
*
*******************************************************************************/
static GT_STATUS setNetworkPortSpeedLpSerdes
(
    IN  GT_U8                devNum,
    IN  GT_U8                portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_U32 regAddr;
    GT_U32 regValue;
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;
    GT_STATUS rc;
    GT_U32 i;
    GT_U32 serdesesNum;
    GT_U32 startSerdes;
    GT_U8  tmpStartSerdes, tmpSerdesesNum;
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E;
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeedPrev;
    GT_U32      portGroupId;     /*the port group Id - support multi-port-groups device */
    GT_U8       localPort;  /* local port - support multi-port-groups device */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    CPSS_TBD_BOOKMARK
    #if 0
    serdesRefClock = PRV_CPSS_DXCH_PP_MAC(devNum)->port.serdesRefClock;
    if ((serdesRefClock != PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_SINGLE_ENDED_E) &&
        (serdesRefClock != PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_DIFF_E))
    {
        return GT_NOT_SUPPORTED;
    }
    #endif

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        && (speed == CPSS_PORT_SPEED_2500_E))
    {/* 2.5G not supportd on network ports of XCAT2 */
        return GT_NOT_SUPPORTED;
    }

    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if((ifMode != CPSS_PORT_INTERFACE_MODE_SGMII_E) &&
        (ifMode != CPSS_PORT_INTERFACE_MODE_QSGMII_E))
    {
        return GT_NOT_SUPPORTED;
    }

    if(ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E)
    {
        if (speed == CPSS_PORT_SPEED_1000_E)
        {
            serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E;
        }
        else if (speed == CPSS_PORT_SPEED_2500_E)
        {
            serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E;
        }
    }
    else
    {
        serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_E;
    }

    if (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if((rc = getNumberOfSerdesLanesForPortLocal(devNum, portNum, ifMode, &tmpStartSerdes,
                                                &tmpSerdesesNum)) != GT_OK)
    {
        return rc;
    }
    else
    {
        /* to ensure correct assignment in BE mode */
        startSerdes = ((GT_U32)tmpStartSerdes) & 0xFF;
        serdesesNum = ((GT_U32)tmpSerdesesNum) & 0xFF;
    }

    /* check if serdeses not in reset and open for read, otherwise
     * make default init to enable configuration continue */
    rc = prvCpssDxChLpCheckAndInitNeededSerdes(devNum,portGroupId,startSerdes,serdesesNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvGetLpSerdesSpeed(devNum,portGroupId,startSerdes,&serdesSpeedPrev);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* perform power down only if serdes speed changed */
    if (serdesSpeedPrev != serdesSpeed)
    {
        /* power down all serdes */
        for (i = startSerdes; i < startSerdes + serdesesNum;i++)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                serdesConfig[i].powerReg;
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* set PLL/INTP Register2 and PLL/INTP Register3*/
        for (i = startSerdes; i < startSerdes + serdesesNum;i++)
        {
            regValue = lpSerdesConfig[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E][serdesSpeed];
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                serdesConfig[i].pllIntpReg2;
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
            if (rc != GT_OK)
            {
                return rc;
            }
            regValue = lpSerdesConfig[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_0_E][serdesSpeed];
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                serdesConfig[i].pllIntpReg3;
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E)
    {
        /* enable/disable XCAT specific for SGMII 2.5G on network ports configurations */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;
        if(serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E)
        {/* 2.5G port speed */
            if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                        21, 1, 1)) != GT_OK)
                return rc;

            /* enable network_ports_dp_sync_fifo_bypass */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,13,1,0);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {/* 1G port speed */
            if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                        21, 1, 0)) != GT_OK)
                return rc;
        }
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x\n", regAddr));            
    }

    return GT_OK;
}

/*******************************************************************************
* setFlexLinkPortSpeed
*
* DESCRIPTION:
*       Sets speed for flex link port on specified device.
*
* INPUTS:
*       devNum - physical device number
*       port   - physical port number
*       speed  - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong speed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS setFlexLinkPortSpeed
(
    IN  GT_U8                devNum,
    IN  GT_U8                port,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_U32 regAddr;
    GT_U32 regValue;
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;
    GT_STATUS rc;
    GT_U32 serdesSpeedConf = 0;
    GT_U32 lanes[4] = {0,1,2,3};
    GT_U32 laneNum = 4;
    GT_U32 serdesesNum = 4;
    GT_U32 i;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_ENT serdesRefClock;

    if(speed > CPSS_PORT_SPEED_5000_E)
    {
        return GT_BAD_PARAM;
    }
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

    serdesRefClock = PRV_CPSS_DXCH_PP_MAC(devNum)->port.serdesRefClock;
    rc = cpssDxChPortInterfaceModeGet(devNum,port,&ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if ((speed == CPSS_PORT_SPEED_1000_E ||
         speed == CPSS_PORT_SPEED_100_E ||
         speed == CPSS_PORT_SPEED_10_E)
        && ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E)
    {
        switch (serdesRefClock)
        {
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_25_SINGLE_ENDED_E:
                serdesSpeedConf = 0x205C;
                break;
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_SINGLE_ENDED_E:
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_DIFF_E:
                serdesSpeedConf = 0x200C;
                break;
            default:
                serdesSpeedConf = 0;
        }
    }
    else if ((speed == CPSS_PORT_SPEED_2500_E && ifMode == CPSS_PORT_INTERFACE_MODE_QX_E) ||
             (speed == CPSS_PORT_SPEED_2500_E && ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) ||
             (speed == CPSS_PORT_SPEED_5000_E && ifMode == CPSS_PORT_INTERFACE_MODE_HX_E) ||
             (speed == CPSS_PORT_SPEED_10000_E && ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) ||
             (speed == CPSS_PORT_SPEED_10000_E && ifMode == CPSS_PORT_INTERFACE_MODE_RXAUI_E))
    {
        switch (serdesRefClock)
        {
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_SINGLE_ENDED_E:
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_DIFF_E:
                serdesSpeedConf = 0x245E;
                break;
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_SINGLE_ENDED_E:
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_DIFF_E:
                serdesSpeedConf = 0x2421;
                break;
            default:
                serdesSpeedConf = 0;
        }
    }
    else if ((speed == CPSS_PORT_SPEED_5000_E && ifMode == CPSS_PORT_INTERFACE_MODE_QX_E) ||
             (speed == CPSS_PORT_SPEED_10000_E && ifMode == CPSS_PORT_INTERFACE_MODE_HX_E))
    {
        switch (serdesRefClock)
        {
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_SINGLE_ENDED_E:
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_DIFF_E:
                serdesSpeedConf = 0x245D;
                break;
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_SINGLE_ENDED_E:
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_DIFF_E:
                serdesSpeedConf = 0x2420;
                break;
            default:
                serdesSpeedConf = 0;
        }
    }
    else if (speed == CPSS_PORT_SPEED_12000_E && ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E)
    {
        switch (serdesRefClock)
        {
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_25_SINGLE_ENDED_E:
                serdesSpeedConf = 0x255C;
                break;
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_SINGLE_ENDED_E:
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_DIFF_E:
                serdesSpeedConf = 0x2435;
                break;
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_SINGLE_ENDED_E:
            case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_DIFF_E:
                serdesSpeedConf = 0x2410;
            default:
               break;
        }
    }

    if (serdesSpeedConf == 0)
    {
        return GT_NOT_SUPPORTED;
    }

    if (ifMode == CPSS_PORT_INTERFACE_MODE_QX_E || ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E)
    {
        lanes[1] = 3;
        laneNum = 2;
    }

    /* Configure serdes lanes power Down */
    for(i = 0; i < serdesesNum; i++)
    {
        /* PowerUp Register 1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[port].serdesPowerUp1[i];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* PowerUp Register 2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[port].serdesPowerUp2[i];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    for (i = 0;i < laneNum; i++)
    {
        /* SerDes Speed reg1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[port].serdesSpeed1[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, serdesSpeedConf);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* SerDes Speed reg2 */
        regValue = (ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) ? 0x66 : 0xe6;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[port].serdesSpeed2[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* SerDes Speed reg3 */
        regValue = (ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) ? 0x32 : 0x3;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[port].serdesSpeed3[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Tx Conf reg1 */
        regValue = (ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) ? 0xEAA : 0x27BE;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[port].serdesTxConfReg1[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Rx Conf reg1 */
        regValue = (ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) ? 0xF38 : 0xFC4;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[port].serdesRxConfReg1[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* DTL conf reg2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[port].serdesDtlConfReg2[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x2800);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* DTL conf reg3 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[port].serdesDtlConfReg3[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x2A9);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Reserv Conf Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[port].serdesReservConf[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0xf070);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* IVREF Conf Register 1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[port].serdesIvrefConf1[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x557B);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Configure serdes lanes power Up */
    for (i = 0;i < laneNum; i++)
    {
        /* PowerUp Register 1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[port].serdesPowerUp1[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x44);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* PowerUp Register 2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[port].serdesPowerUp2[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x44);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* Wait at least 140 usec */
    HW_WAIT_MILLISECONDS_MAC(14);

    /* SP analog De-Assert reset reg #1 */
    for (i = 0;i < laneNum; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[port].serdesReset[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x1110);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Misc conf reg */
    for (i = 0;i < laneNum; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[port].serdesMiscConf[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x4);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* SP analog De-Assert reset reg #2 */
    for (i = 0;i < laneNum; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[port].serdesReset[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x1010);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* SP analog De-Assert reset reg #3 */
    for (i = 0;i < laneNum; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[port].serdesReset[lanes[i]];
        if (lanes[i] == 1 || lanes[i] == 2)
        {
            regValue = 0x1010;
        }
        else
        {
            regValue = 0x10;
        }
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* SP analog De-Assert reset reg #3 */
    for (i = 0;i < laneNum; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[port].serdesReset[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x00);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* DeAssert Reset TxSyncFIFO */
    for (i = 0;i < laneNum; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[port].serdesMiscConf[lanes[i]];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0xc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    if (ifMode != CPSS_PORT_INTERFACE_MODE_SGMII_E)
    {
        /* De-Assert XPCS reset */
        for (i = 0;i < laneNum; i++)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[port].xgGlobalConfReg0;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr, 0, 3, 0x7);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        HW_WAIT_MILLISECONDS_MAC(10);

        /* De-assert mac reset */
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,port,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,port),&regAddr);


        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr, 1, 1, 1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* De-assert mac reset */
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,port,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,port),&regAddr);

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr, 6, 1, 0);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = setSgmiiPortSpeed(devNum,port,speed);
        if (rc != GT_OK)
        {
            return rc;
        }

    }

    return GT_OK;

}
/* matrix [serdes speed][serdes ref clock] = reg value */
static GT_U32 speedRegValue[4][3] =
{{0x205C,0x200C,0   }, /* 1.25G */
 {0     ,0x245E,0x2421}, /* 3.125G */
 {0     ,0x245D,0x2420}, /* 6.25G */
 {0x255C,0x2435,0x2410}  /* 3.75G */
};

/*******************************************************************************
* getXgPortSpeed
*
* DESCRIPTION:
*       Gets speed for specified XG/HX port on specified device.
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       speedPtr - (pointer to) actual port speed
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getXgPortSpeed
(
    IN  GT_U8                 dev,
    IN  GT_U8                 port,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_U32 regAddr; /* register address */
    GT_U32 regValue; /* register value */
    CPSS_PORT_INTERFACE_MODE_ENT ifMode; /* port interface mode */
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed; /* serdes speed/frequency */
    GT_STATUS rc; /* return code */
    GT_U32 serdes; /* first serdes of port */
    GT_U32  portGroupId;     /* the port group Id - support multi-port-groups device      */
    GT_U32  refClockId; /* SERDES reference clock index in speedRegValue */
    GT_U8   tmpStartSerdes, tmpSerdesesNum; /* first serdes of port; number of serdes occupied by port */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);

    if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(dev,port) == GT_TRUE)
    {
        if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
        {
            rc = cpssDxChPortInterfaceModeGet(dev,port,&ifMode);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* NA interface */
            if (ifMode ==CPSS_PORT_INTERFACE_MODE_NA_E)
            {
                *speedPtr = CPSS_PORT_SPEED_NA_E;
                return GT_OK;
            }

            if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                if(PRV_CPSS_DXCH_ERRATA_GET_MAC(dev, PRV_CPSS_DXCH_LION_40G_MAC_NOT_SUPPORTED_WA_E)
                    && PRV_CPSS_DXCH_ERRATA_GET_MAC(dev, PRV_CPSS_DXCH_LION_20G_MAC_NOT_SUPPORTED_WA_E)
                    && PRV_CPSS_DXCH_ERRATA_GET_MAC(dev, PRV_CPSS_DXCH_LION_1G_SPEED_NOT_SUPPORTED_WA_E))
                { /* with errata above just 10G supported */
                    *speedPtr = CPSS_PORT_SPEED_10000_E;
                    return GT_OK;
                }
            }

            if((rc = getNumberOfSerdesLanesForPortLocal(dev, port, ifMode, &tmpStartSerdes,
                                                        &tmpSerdesesNum)) != GT_OK)
            {
                return rc;
            }
            else
            {
                /* to ensure correct assignment in BE mode */
                serdes = ((GT_U32)tmpStartSerdes) & 0xFF;
            }

            rc = prvGetLpSerdesSpeed(dev,portGroupId,serdes,&serdesSpeed);
            if (rc != GT_OK)
            {
                if (rc == GT_NOT_SUPPORTED)
                {/* if port/serdes doesn't support speed parameter don't fail the function */
                    *speedPtr = CPSS_PORT_SPEED_NA_E;
                    return GT_OK;
                }
                else
                    return rc;
            }
        }
        else /* xCat A0 */
        {
            switch (PRV_CPSS_DXCH_PP_MAC(dev)->port.serdesRefClock)
            {
                case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_25_SINGLE_ENDED_E:
                    refClockId = 0;
                    break;
                case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_SINGLE_ENDED_E:
                case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_125_DIFF_E:
                    refClockId = 1;
                    break;
                case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_SINGLE_ENDED_E:
                case PRV_CPSS_DXCH_PORT_SERDES_REF_CLOCK_156_25_DIFF_E:
                    refClockId = 2;
                    break;
                default:
                    return GT_NOT_SUPPORTED;
            }
            /* Get SERDES speed configuration */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                            macRegs.perPortRegs[port].serdesSpeed1[0];
            if (prvCpssDrvHwPpPortGroupReadRegister(dev, portGroupId,regAddr, &regValue) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            if (regValue == speedRegValue[0][refClockId])
            {
                serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E;
            }
            else if (regValue == speedRegValue[1][refClockId])
            {
                serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E;
            }
            else if (regValue == speedRegValue[2][refClockId])
            {
                serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_6_25_E;
            }
            else if (regValue == speedRegValue[3][refClockId])
            {
                serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_75_E;
            }
            else
            {
                serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E;
            }
        }
        /* Get ifMode */
        rc = cpssDxChPortInterfaceModeGet(dev,port,&ifMode);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* NA interface */
        if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
        {
            *speedPtr = CPSS_PORT_SPEED_NA_E;
        }
        /* 1G SGMII */
        else if ((ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E ||
             ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E ||
             ifMode == CPSS_PORT_INTERFACE_MODE_100BASE_FX_E)
            && serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E)
        {
            /* if we reached this point, then we are in 10/100/1000 interface */
            PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(dev,port,&regAddr);

            rc = prvCpssDrvHwPpPortGroupGetRegField(dev, portGroupId,regAddr, 1, 2, &regValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            if ((regValue & 0x1) == 1)
            {
                *speedPtr = CPSS_PORT_SPEED_1000_E;
            }
            else
            {
                *speedPtr = ((regValue >> 1) == 1) ? CPSS_PORT_SPEED_100_E:
                    CPSS_PORT_SPEED_10_E;
            }
        }
        /* 2.5G SGMII or QX */
        else if ((ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E ||
                  ifMode == CPSS_PORT_INTERFACE_MODE_QX_E ) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E))
        {
            *speedPtr = CPSS_PORT_SPEED_2500_E;
        }
        /* DQX or HX */
        else if (((ifMode == CPSS_PORT_INTERFACE_MODE_QX_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_6_25_E)) ||
                 ((ifMode == CPSS_PORT_INTERFACE_MODE_HX_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E)))
        {
            *speedPtr = CPSS_PORT_SPEED_5000_E;
        }
        /* DHX or XAUI or RXAUI */
        else if (((ifMode == CPSS_PORT_INTERFACE_MODE_HX_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_6_25_E)) ||
                 ((ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E)) ||
                 ((ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E)) ||
                 ((ifMode == CPSS_PORT_INTERFACE_MODE_RXAUI_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_6_25_E)))
        {
            *speedPtr = CPSS_PORT_SPEED_10000_E;
        }
        /* HGS */
        else if ((ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_75_E))
        {
            *speedPtr = CPSS_PORT_SPEED_12000_E;
        }
        else if ((ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_4_25_E))
        {
            *speedPtr = CPSS_PORT_SPEED_13600_E;
        }
        else if (((ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_6_25_E)) ||
                 ((ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_6_25_E)))
        {
            *speedPtr = CPSS_PORT_SPEED_20000_E;
        }
        else if ((ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_156_E))
        {
            *speedPtr = CPSS_PORT_SPEED_40000_E;
        }
        else if ((ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_E))
        {
            *speedPtr = CPSS_PORT_SPEED_16000_E;
        }
        else
        {
#if (defined ASIC_SIMULATION) &&  (!(defined RTOS_ON_SIM))
            /* allow simulation not to have all the SERDESs specific values */
            *speedPtr = CPSS_PORT_SPEED_10000_E;
#else /*(defined ASIC_SIMULATION) &&  (!(defined RTOS_ON_SIM))*/
            return GT_FAIL;
#endif /*(defined ASIC_SIMULATION) &&  (!(defined RTOS_ON_SIM))*/
        }

        return GT_OK;

    }

    /* 10 Gb interface - can be either 10 or 12 Gb */
    if (isPortInXgMode (dev, port))
    {
        if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(dev))
        {
            return prvCpssDxCh3PortXgPortSpeedGet(dev, port, speedPtr);
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
            smiRegs.smi10GePhyConfig0[port];
        if (prvCpssDrvHwPpPortGroupGetRegField(dev, portGroupId,regAddr, 6, 2, &regValue) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        if (regValue == 0)
        {
            *speedPtr = CPSS_PORT_SPEED_10000_E;
        }
        else if (regValue == 1)
        {
            *speedPtr = CPSS_PORT_SPEED_12000_E;
        }
        else /* 2 and 3 are reserved values */
        {
            return GT_FAIL;
        }
        return GT_OK;
    }
    else
    {
        if( (25 != port) && (26 != port) )
        {
            return GT_FAIL;
        }
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->macRegs.perPortRegs->
            hxPortConfig0[port-25];
        if (prvCpssDrvHwPpPortGroupGetRegField(dev, portGroupId,regAddr, 5, 1, &regValue) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        if (regValue == 0)
        {
            *speedPtr = CPSS_PORT_SPEED_2500_E;
        }
        else
        {
            *speedPtr = CPSS_PORT_SPEED_5000_E;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* isPortInXgMode
*
* DESCRIPTION:
*       Returns mode for specified XG/HX port on specified device.
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE  - The port is in XG mode.
*       GT_FALSE - The port is in HX/QX mode.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL isPortInXgMode
(
    IN  GT_U8  dev,
    IN  GT_U8  port
)
{
    GT_STATUS rc;           /* return code      */
    GT_U32    regAddr;      /* register address */
    GT_U32    regValue;     /* register content */

    if (PRV_CPSS_DXCH_XG_PORT_TYPE_MAC(dev,port) == PRV_CPSS_XG_PORT_HX_QX_ONLY_E)
        return GT_FALSE;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->macRegs.hxPortGlobalConfig;
    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        return GT_TRUE;

    /* at this point the port can be either XG or HX/QX - read port25Mode field
       to get the mode */
    rc = prvCpssDrvHwPpPortGroupGetRegField(
        dev, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port),
        regAddr, 15, 1, &regValue);
    if (rc != GT_OK)
    {
        return GT_FALSE;
    }

    return BIT2BOOL_MAC(regValue);
}

/*******************************************************************************
* cpssDxChPortSerdesComboModeSet
*
* DESCRIPTION:
*       Set Serdes mode. If Serdes mode is CPSS_DXCH_PORT_SERDES_MODE_COMBO_E,
*       Combo Serdes are active for the port, else regular Serdes are active for
*       the port.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number, values 20..23.
*       mode     - serdes mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum, mode
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesComboModeSet
(
    IN  GT_U8                           devNum,
    IN  GT_U8                           portNum,
    IN  CPSS_DXCH_PORT_SERDES_MODE_ENT  mode
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType == PRV_CPSS_PORT_FE_E)
    {
        /* no serdes support for FE ports */
        return GT_NOT_SUPPORTED;
    }

    if((portNum > 23) || (portNum < 20))
    {
        return GT_BAD_PARAM;
    }

    switch(mode)
    {
        case CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E:
            value = 0;
            break;
        case CPSS_DXCH_PORT_SERDES_MODE_COMBO_E:
            value = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,portMacType,&regAddr);

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 15, 1, value);

}

/*******************************************************************************
* cpssDxChPortSerdesComboModeGet
*
* DESCRIPTION:
*       Get Serdes mode. If Serdes mode is CPSS_DXCH_PORT_SERDES_MODE_COMBO_E,
*       Combo Serdes are active for the port, else regular Serdes are active for
*       the port.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number, values 20..23.
*
* OUTPUTS:
*       modePtr   -  pointer to Serdes mode.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesComboModeGet
(
    IN  GT_U8                             devNum,
    IN  GT_U8                             portNum,
    OUT CPSS_DXCH_PORT_SERDES_MODE_ENT    *modePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_STATUS   rc;      /* return status */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType == PRV_CPSS_PORT_FE_E)
    {
        /* no serdes support for FE ports */
        return GT_NOT_SUPPORTED;
    }

    if((portNum > 23) || (portNum < 20))
    {
        return GT_BAD_PARAM;
    }


    PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum,portMacType,&regAddr);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
        regAddr, 15, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (value == 0) ? CPSS_DXCH_PORT_SERDES_MODE_REGULAR_E :
                              CPSS_DXCH_PORT_SERDES_MODE_COMBO_E;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortSerdesConfigSet
*
* DESCRIPTION:
*       Set SERDES configuration per port
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number.
*       serdesCfgPtr    - pointer to port serdes configuration.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on SERDES parameters out of range
*       GT_HW_ERROR     - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This API also checks all SERDES per port initialization. If serdes was
*       not initialized, proper init will be done
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesConfigSet
(
    IN GT_U8  devNum,
    IN GT_U8  portNum,
    IN CPSS_DXCH_PORT_SERDES_CONFIG_STC  *serdesCfgPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register value */
    GT_STATUS   rc;         /* return code */
    GT_U32      lane;       /* Serdes lane index  */
    GT_U32      startSerdes = 0, serdesesNum = 0;
    GT_U8       tmpStartSerdes = 0, tmpSerdesesNum = 0;
    GT_U32      portGroupId;     /*the port group Id - support multi-port-groups device */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(serdesCfgPtr);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
    {
        /* no serdes support for FE ports */
        return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    /* Valid values range 0-15 */
    if(serdesCfgPtr->txEmphAmp >= BIT_4)
    {
        /* 4 bits in hardware */
        return GT_OUT_OF_RANGE;
    }

    /* Valid values range 0-31 */
    if (serdesCfgPtr->txAmp >= BIT_5)
    {
        /* 5 bits in hardware */
        return GT_OUT_OF_RANGE;
    }

    if((rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode)) != GT_OK)
        return rc;

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        serdesesNum = 4;
        /* Build SERDEX configuration fields */
        regValue =(serdesCfgPtr->txAmp << 1) |
                  (BOOL2BIT_MAC(serdesCfgPtr->txEmphEn) << 7) |
                  (serdesCfgPtr->txEmphAmp << 10);

        if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portNum) == PRV_CPSS_PORT_XG_E)
        {
            for(lane = 0; lane < serdesesNum; lane++)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[portNum].serdesTxConfReg1[lane];
                /* Set Serdes Lanes configuration bits for XG ports*/
                rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, 0x3CBE, regValue);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        else
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].serdesTxConfReg1[0];
            /* Set Serdes configuration bits for GE ports */
            rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, 0x3CBE, regValue);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }
    else /* xCat and above */
    {
        if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&tmpStartSerdes,&tmpSerdesesNum)) != GT_OK)
            return rc;
        else
        {
            /* to ensure correct assignment in BE mode */
            startSerdes = ((GT_U32)tmpStartSerdes) & 0xFF;
            serdesesNum = ((GT_U32)tmpSerdesesNum) & 0xFF;
        }

        if (serdesCfgPtr->txAmpAdj >= BIT_2 ||
            serdesCfgPtr->ffeCapacitorSelect >= BIT_4 ||
            serdesCfgPtr->ffeResistorSelect >= BIT_3 ||
            serdesCfgPtr->ffeSignalSwingControl >= BIT_2)
        {
            return GT_OUT_OF_RANGE;
        }
        
        /* check if all serdes in port are initialized and make proper initialization
           if it is needed */
        rc = prvCpssDxChLpCheckAndInitNeededSerdes(devNum,portGroupId,startSerdes,serdesesNum);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* set Transmitter Register1 */
        regValue = serdesCfgPtr->txEmphAmp |
                  (serdesCfgPtr->txAmp << 8) |
                  (serdesCfgPtr->txAmpAdj << 13);

        for (lane = startSerdes; lane < (startSerdes + serdesesNum); lane++)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        serdesConfig[lane].transmitterReg1;
            rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, 0x7F0F, regValue);
            if(rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        /* set FFE Register 0 */
        regValue = (serdesCfgPtr->ffeCapacitorSelect |
                  (serdesCfgPtr->ffeResistorSelect << 4) |
                  (serdesCfgPtr->ffeSignalSwingControl << 8));
        for (lane = startSerdes; lane < (startSerdes + serdesesNum); lane++)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    serdesConfig[lane].ffeReg;
            rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, 0x37F, regValue);
            if(rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        regValue = BOOL2BIT_MAC(serdesCfgPtr->txEmphEn);
        for (lane = startSerdes; lane < (startSerdes + serdesesNum); lane++)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        serdesConfig[lane].transmitterReg0;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 15, 1, regValue);
            if(rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSerdesConfigGet
*
* DESCRIPTION:
*       Get SERDES configuration per port.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number.
*
* OUTPUTS:
*       serdesCfgPtr    - pointer to port serdes configuration.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*       GT_NOT_INITIALIZED - on no initialized SERDES per port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        This API also checks if at least one serdes per port was initialized.
*        In case there was no initialized SERDES per port GT_NOT_INITIALIZED is
*        returned.
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesConfigGet
(
    IN GT_U8  devNum,
    IN GT_U8  portNum,
    OUT CPSS_DXCH_PORT_SERDES_CONFIG_STC  *serdesCfgPtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register value */
    GT_STATUS   rc;         /* return code */
    GT_U32      serdes;     /* serdes number */
    GT_U32      portGroupId;     /*the port group Id - support multi-port-groups device */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    GT_U32      firstInitializedSerdes;
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;    /* port interface mode */
    GT_U8       tmpStartSerdes = 0, tmpSerdesesNum = 0; /* temporary variables */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(serdesCfgPtr);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
    {
        /* no serdes support for FE ports */
        return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].serdesTxConfReg1[0];

        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* Tx Driver output amplitude */
        serdesCfgPtr->txAmp = (regValue >> 1) & 0x1f;
        /* Pre- or de-emphasis enable signal */
        serdesCfgPtr->txEmphEn = BIT2BOOL_MAC(((regValue >> 7) & 0x1));
        /* Emphasis level control bits */
        serdesCfgPtr->txEmphAmp = (regValue >> 10) & 0xf;
    }
    else /* xCat and above */
    {
        if((rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode)) != GT_OK)
            return rc;
    
        if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
        {
            return GT_NOT_SUPPORTED;
        }

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            if(((ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E) && (localPort%2 != 0)) ||
                ((ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E) && (localPort != 10)))
                return GT_NOT_SUPPORTED;
        }

        if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&tmpStartSerdes,&tmpSerdesesNum)) != GT_OK)
            return rc;
        else
        {
            /* to ensure correct assignment in BE mode */
            serdes = ((GT_U32)tmpStartSerdes) & 0xFF;
        }

        rc = prvCpssDxChLpGetFirstInitSerdes(devNum,portGroupId,serdes,&firstInitializedSerdes);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* get Transmitter Register 1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[firstInitializedSerdes].transmitterReg1;

        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
        serdesCfgPtr->txEmphAmp = regValue & 0xf;
        serdesCfgPtr->txAmp = (regValue >> 8) & 0x1f;
        serdesCfgPtr->txAmpAdj = (regValue >> 13) & 0x3;

        /* get FFE Register 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[firstInitializedSerdes].ffeReg;

        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
        serdesCfgPtr->ffeCapacitorSelect = regValue & 0xf;
        serdesCfgPtr->ffeResistorSelect = (regValue >> 4) & 0x7;
        serdesCfgPtr->ffeSignalSwingControl = (regValue >> 8) & 0x3;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    serdesConfig[firstInitializedSerdes].transmitterReg0;
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 15, 1, &regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
        serdesCfgPtr->txEmphEn = BIT2BOOL_MAC(regValue);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortPaddingEnableSet        
*
* DESCRIPTION:
*       Enable/Disable padding of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - Pad short packet in Tx.
*                - GT_FALSE - No padding in short packets.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPaddingEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_U32      offset;  /* field offset */
    PRV_CPSS_PORT_TYPE_ENT portMacType;
    GT_STATUS rc;

    value = (enable == GT_TRUE) ? 0 : 1;
#ifdef __AX_PLATFORM__
			for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
			{
#else
			portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
				if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
					continue;
#else
					return GT_OK;
#endif


        if(portMacType >= PRV_CPSS_PORT_XG_E)
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
            offset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 13 : 14;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
            offset = 5;
        }

       if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
       {
            rc= prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                                                   regAddr, offset, 1, value);
            if (rc!=GT_OK) 
            {
                return rc;
            }
       }
#ifdef __AX_PLATFORM__
    }
#endif

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPaddingEnableSet
*
* DESCRIPTION:
*       Enable/Disable padding of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - Pad short packet in Tx.
*                - GT_FALSE - No padding in short packets.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPaddingEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
)
{
    GT_BOOL    targetEnable = enable;
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;            /* do switch mac code indicator */                   
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPaddingEnableSetFunc(devNum,portNum,
                                  targetEnable, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* set switch MAC Padding Enable configuration */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortPaddingEnableSet(devNum,portNum,targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPaddingEnableSetFunc(devNum,portNum,
                                  targetEnable, 
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortPaddingEnableGet       
*
* DESCRIPTION:
*       Gets padding status of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr   - pointer to packet padding status.
*                     - GT_TRUE  - Pad short packet in Tx.
*                     - GT_FALSE - No padding in short packets.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortPaddingEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_U32      offset;  /* field offset */
    GT_STATUS   rc;      /* return status */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    if(portMacType < PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
        offset = 5;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            offset = 13;
        }
        else
        {
            offset = 14;
        }
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, offset, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortPaddingEnableGet
*
* DESCRIPTION:
*       Gets padding status of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr   - pointer to packet padding status.
*                     - GT_TRUE  - Pad short packet in Tx.
*                     - GT_FALSE - No padding in short packets.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortPaddingEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_STATUS   rc = GT_OK;                      /* return status */
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    /* Get PHY MAC object ptr */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacPaddingEnableGetFunc(devNum,portNum,
                                  enablePtr, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,enablePtr);
          if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* get Port Padding Enable value from switch MAC*/
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortPaddingEnableGet(devNum,portNum,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacPaddingEnableGetFunc(devNum,portNum,
                                  enablePtr, 
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortExcessiveCollisionDropEnableSet   
*
* DESCRIPTION:
*       Enable/Disable excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - if the number of collisions on the same packet
*                             is 16 the packet is dropped.
*                - GT_FALSE - A collided packet will be retransmitted by device
*                             until it is transmitted without collisions,
*                             regardless of the number of collisions on the packet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       The setting is not relevant in full duplex mode
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortExcessiveCollisionDropEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);

    value = (enable == GT_TRUE) ? 0 : 1;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 4, 1, value);

}

/*******************************************************************************
* cpssDxChPortExcessiveCollisionDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - if the number of collisions on the same packet
*                             is 16 the packet is dropped.
*                - GT_FALSE - A collided packet will be retransmitted by device
*                             until it is transmitted without collisions,
*                             regardless of the number of collisions on the packet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       The setting is not relevant in full duplex mode
*       Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*
*******************************************************************************/
GT_STATUS cpssDxChPortExcessiveCollisionDropEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   
    GT_BOOL    targetEnable = enable;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacExcessiveCollisionDropSetFunc(devNum,portNum,
                                  targetEnable, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
         
    /* set switch MAC Collision Drop configuration */
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortExcessiveCollisionDropEnableSet(devNum,portNum,targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacExcessiveCollisionDropSetFunc(devNum,portNum,
                                  targetEnable, 
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,&targetEnable);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}


/*******************************************************************************
* prvCpssDxChPortExcessiveCollisionDropEnableGet   
*
* DESCRIPTION:
*       Gets status of excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr - pointer to status of excessive collision packets drop.
*                   - GT_TRUE  - if the number of collisions on the same packet
*                               is 16 the packet is dropped.
*                   - GT_FALSE - A collided packet will be retransmitted by
*                                device until it is transmitted
*                                without collisions, regardless of the number
*                                of collisions on the packet.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Not relevant in full duplex mode
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortExcessiveCollisionDropEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_STATUS   rc;      /* return status */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PORT_MAC_CTRL1_REG_MAC(devNum,portNum,portMacType,&regAddr);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 4, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortExcessiveCollisionDropEnableGet
*
* DESCRIPTION:
*       Gets status of excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr - pointer to status of excessive collision packets drop.
*                   - GT_TRUE  - if the number of collisions on the same packet
*                               is 16 the packet is dropped.
*                   - GT_FALSE - A collided packet will be retransmitted by
*                                device until it is transmitted
*                                without collisions, regardless of the number
*                                of collisions on the packet.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       1. Added callback bind option used to run PHYMAC configuration functions
*       Callbacks can be run before switch MAC configuration and after
*       First callback can set doPpMacConfig TRUE or FALSE. If the return value 
*       is TRUE the switch MAC will be configured
*
*       2. Not relevant in full duplex mode
*
*******************************************************************************/
GT_STATUS cpssDxChPortExcessiveCollisionDropEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
)
{
    CPSS_MACDRV_OBJ_STC * portMacObjPtr;         /* port object pointer */
    GT_STATUS rc = GT_OK;
    GT_BOOL doPpMacConfig = GT_TRUE;             /* do switch mac code indicator */                   

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Get PHY MAC object pnt */
    portMacObjPtr = PRV_CPSS_PHY_MAC_OBJ(devNum,portNum);

    /* check if MACPHY callback should run */
    if (portMacObjPtr != NULL) {
        rc = portMacObjPtr->macDrvMacExcessiveCollisionDropGetFunc(devNum,portNum,
                                  enablePtr, CPSS_MACDRV_STAGE_PRE_E,
                                  &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    
    /* Get Port Excessive Collision Drop value from switch MAC*/
    if (doPpMacConfig == GT_TRUE) 
    {
        rc = prvCpssDxChPortExcessiveCollisionDropEnableGet(devNum,portNum,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }

    /* "post stage" callback run*/
    if (portMacObjPtr != NULL)  {
        rc = portMacObjPtr->macDrvMacExcessiveCollisionDropGetFunc(devNum,portNum,
                                  enablePtr, 
                                  CPSS_MACDRV_STAGE_POST_E,
                                  &doPpMacConfig,enablePtr);
        if(rc!=GT_OK)
        {
            return rc;
        }
    }
    return rc;
}

/******************************************************************************
* prvCpssDxChPortSerdesPowerUpDownSet
*
* DESCRIPTION:
*       Sets power state of SERDES port lanes according to port capabilities.
*       XG / XAUI ports: All 4 (TX or RX) lanes are set simultanuously.
*       HX / QX ports: lanes 0,1 or TX and RX may be set separately.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       dev       - physical device number
*       port      - physical port number
*       direction - may be either CPSS_PORT_DIRECTION_RX_E,
*                                 CPSS_PORT_DIRECTION_TX_E, or
*                                 CPSS_PORT_DIRECTION_BOTH_E.
*       lanesBmp  - bitmap of SERDES lanes (bit 0-> lane 0, etc.)
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_FAIL          - on error
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*
* COMMENTS:
*     none
*
******************************************************************************/
static GT_STATUS prvCpssDxChPortSerdesPowerUpDownSet
(
    IN  GT_U8                   dev,
    IN  GT_U8                   port,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  lanesBmp,
    IN  GT_BOOL                 powerUp
)
{
    PRV_CPSS_PORT_TYPE_OPTIONS_ENT xgType;
    GT_STATUS       status;
    GT_U32          regAddr;
    GT_U32          regAddrReset;
    GT_U32          data;
    GT_U16          data16;
    GT_U8           i;
    GT_U32          pwrDn = (powerUp == GT_TRUE) ? 0:1;
    GT_U32          maskRst = 0;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(dev, port) < PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);

    /* Determine port type */
    xgType = PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portTypeOptions;
    if ( xgType == PRV_CPSS_XG_PORT_XG_HX_QX_E)
    {
        /* Resolve abiguity of multi-mode port */
        xgType = isPortInXgMode( dev, port) ? PRV_CPSS_XG_PORT_XG_ONLY_E :
                                                PRV_CPSS_XG_PORT_HX_QX_ONLY_E;
    }

    /* select the proper sequence */
    switch ( xgType)
    {
        case PRV_CPSS_XG_PORT_XG_ONLY_E:
            /* A group of 4 lanes is controlled by the same bit */
            if ( lanesBmp == 0xf)
            {
                regAddr = 0x8000;
                status = cpssDxChPhyPort10GSmiRegisterRead(dev, port, 0, GT_FALSE,
                                                           (GT_U16)regAddr, 5, &data16);
                if ( status != GT_OK)
                    return status;

                data = data16;
                switch ( direction)
                {
                    case CPSS_PORT_DIRECTION_RX_E:
                        U32_SET_FIELD_MAC( data, 7 , 1, pwrDn); /* RX */
                        break;
                    case CPSS_PORT_DIRECTION_BOTH_E:
                        U32_SET_FIELD_MAC( data, 7 , 1, pwrDn); /* RX */
                        U32_SET_FIELD_MAC( data, 11, 1, pwrDn); /* TX */
                        break;
                    case CPSS_PORT_DIRECTION_TX_E:
                        U32_SET_FIELD_MAC( data, 11, 1, pwrDn); /* TX */
                        break;
                    default:
                        status = GT_BAD_PARAM;
                        return status;
                }
                /* update register */
                data16 = (GT_U16)data;
                status = cpssDxChPhyPort10GSmiRegisterWrite(dev, port , 0, GT_FALSE,
                                                            (GT_U16)regAddr, 5, data16);
            }
            else if ( lanesBmp == 0)
            {
                /* no lane affacted */
                status = GT_OK;
            }
            else
            {
                /* HW cannot control lanes individually */
                status = GT_NOT_SUPPORTED;
            }
            break;

        case PRV_CPSS_XG_PORT_HX_QX_ONLY_E:
            /* Controlling lanes individually */
            if ( lanesBmp & (~3) )
            {
                /* invalid arguments */
                status = GT_BAD_PARAM;
            }
            else if ( lanesBmp == 0)
            {
                /* no lane affacted */
                status = GT_OK;
            }
            else
            {
                /* Read HXPort25 or HXPort26 register, port must be >= 25 */
                if( (25 != port) && (26 != port) )
                {
                    return GT_FAIL;
                }
                regAddrReset = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->
                                        macRegs.perPortRegs->hxPortConfig0[port-25];
                regAddr = regAddrReset + 0x60;
                status = prvCpssDrvHwPpPortGroupReadRegister( dev, portGroupId,regAddr, &data);
                if ( status != GT_OK)
                    return status;

                /* Bit manipulation for lanes */
                for ( i = 0; i < 2; ++i)
                {
                    if ( lanesBmp & (1 << i) )
                    {
                        switch ( direction)
                        {
                            case CPSS_PORT_DIRECTION_RX_E:
                                U32_SET_FIELD_MAC( data, i, 1, pwrDn);  /* RX */
                                break;
                            case CPSS_PORT_DIRECTION_BOTH_E:
                                U32_SET_FIELD_MAC( data, i, 1, pwrDn);  /* RX */
                                U32_SET_FIELD_MAC( data, i + 2, 1, pwrDn);  /* TX */
                                break;
                            case CPSS_PORT_DIRECTION_TX_E:
                                U32_SET_FIELD_MAC( data, i + 2, 1, pwrDn);  /* TX */
                                break;
                            default:
                                status = GT_BAD_PARAM;
                                return status;
                        }
                    }
                    /* Reset lane if power down both directions, else put it out of reset */
                    if ( (data & (0x5 << i)) == (0x5ul << i))
                    {
                        data |= (0x10 << i);
                    }
                    else
                    {
                        data &= ~(0x10 << i);
                        maskRst |= (0x10 << i);
                    }
                }
                /* Power up/down sequences */
                if ( pwrDn)
                {
                    /* power down affected lanes and put them in reset state */
                    status = prvCpssDrvHwPpPortGroupWriteRegister( dev, portGroupId,regAddr, data);
                    if ( status != GT_OK)
                        return status;

                    /* Put port into reset state if all lanes are powered down */
                    if ( (data & 0xf) == 0xf)
                        status = prvCpssDrvHwPpPortGroupSetRegField( dev, portGroupId,regAddrReset, 11 , 1, 1);
                }
                else
                {
                    /* Power UP: put the port out of reset, or verify it is out of reset */
                    status = prvCpssDrvHwPpPortGroupSetRegField( dev, portGroupId,regAddrReset, 11 , 1, 0);
                    if ( status != GT_OK)
                        return status;

                    /* Force reset on lanes that are being powered up */
                    status = prvCpssDrvHwPpPortGroupWriteRegister( dev, portGroupId,regAddr, data | maskRst);
                    if ( status != GT_OK)
                        return status;

                    /* apply the new lanes state */
                    status = prvCpssDrvHwPpPortGroupWriteRegister( dev, portGroupId,regAddr, data);
                }
            }
            break;
        default:
            status = GT_FAIL;
            break;
    }

    return status;
}

/*******************************************************************************
* prvCpssDxCh3PortSerdesPowerUpDownGet
*
* DESCRIPTION:
*       Get power up or down state to port and serdes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       powerUpPtr     - (pointer to) power up state.
*                           GT_TRUE  - power up
*                           GT_FALSE - power down
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3PortSerdesPowerUpDownGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL   *powerUpPtr
)
{
    GT_U32      regAddr, regAddr2;       /* register address */
    GT_U32      regData;                 /* register data    */
    GT_U32      regMask;                 /* register mask    */
    GT_U32      i;                       /* iterator */
    GT_U32      startSerdes; /* first SERDES number */
    GT_U32      serdesesNum; /* number of SERDESs in a port  */
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8       tmpStartSerdes = 0, tmpSerdesesNum = 0;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        /* Get current state */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[portNum].serdesPowerUp1[0];
        regAddr2 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[portNum].serdesPowerUp2[0];
        /* SP or XG*/

        if( PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType == PRV_CPSS_PORT_XG_E )
        {
            regMask = 0xFF;
        }
        else /* Giga port */
        {
            regMask = 0x11 << (portNum%4) ;
        }

        rc = prvCpssDrvHwPpPortGroupReadRegBitMask(devNum, portGroupId,regAddr, regMask, &regData);
        if(rc != GT_OK)
        {
            return rc;
        }

        *powerUpPtr = GT_FALSE;
        if( regData == regMask )
        {
            rc = prvCpssDrvHwPpPortGroupReadRegBitMask(devNum, portGroupId,regAddr2, regMask, &regData);
            if(rc != GT_OK)
            {
                return rc;
            }

            if( regData == regMask )
            {
                *powerUpPtr = GT_TRUE;
            }
        }
    }
    else
    {   /* xCat and above */

        if((rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode)) != GT_OK)
            return rc;
    
        if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
        {
            /* SERDES was not initialized yet */
            *powerUpPtr = GT_FALSE;
            return GT_OK;
        }

        if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&tmpStartSerdes,&tmpSerdesesNum)) != GT_OK)
            return rc;
        else
        {
            /* to ensure correct assignment in BE mode */
            startSerdes = ((GT_U32)tmpStartSerdes) & 0xFF;
            serdesesNum = ((GT_U32)tmpSerdesesNum) & 0xFF;
        }

        /* check if all serdes in port are initialized */
        for (i = startSerdes; i < startSerdes + serdesesNum;i++)
        {
            rc = prvCpssDxChLpCheckSerdesInitStatus(devNum, portGroupId, i);
            if ( (rc != GT_OK) && (rc != GT_NOT_INITIALIZED))
            {
                return rc;
            }
            if (rc == GT_NOT_INITIALIZED)
            {
                /* SERDES was not initialized yet */
                *powerUpPtr = GT_FALSE;
                return GT_OK;
            }
        }

        /* check Power state of SERDES */
        for (i = startSerdes; i < startSerdes + serdesesNum;i++)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                serdesConfig[i].powerReg;
            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &regData);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* check that at least one SERDES is UP */
            if (regData != 0)
            {
                /* SERDES is UP */
                *powerUpPtr = GT_TRUE;
                return GT_OK;
            }
        }

        /* SERDES is DOWN */
        *powerUpPtr = GT_FALSE;
    }

    return GT_OK;
}
/*******************************************************************************
* prvCpssDxCh3PortSerdesPowerUpDownSet
*
* DESCRIPTION:
*       Set power up or down state to port and serdes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       powerUp  - power up state:
*                  GT_TRUE  - power up
*                  GT_FALSE - power down
*
* OUTPUTS:
*       oldPowerUpPtr     - (pointer to) old power up state.
*                           GT_TRUE  - power up
*                           GT_FALSE - power down
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3PortSerdesPowerUpDownSet
(
    IN GT_U8         devNum,
    IN GT_U8         portNum,
    IN GT_BOOL       powerUp,
    OUT GT_BOOL   *oldPowerUpPtr
)
{
    GT_U32      regAddr, regAddr2;       /* register address */
    GT_U32      regData;                 /* register data    */
    GT_U32      regMask;                 /* register mask    */
    GT_U32      lane;                    /* lane number      */
    GT_U32      i;                       /* loop index       */
    GT_STATUS   rc;                     /* return status    */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32      networkSerdesPowerUpBmp;  /* bitmap of network SERDES to be power UP */

    CPSS_NULL_PTR_CHECK_MAC(oldPowerUpPtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum, portNum) != PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* Get current state */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].serdesPowerUp1[0];
    regAddr2 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[portNum].serdesPowerUp2[0];
    /* SP or XG*/

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    regMask = 0xFF;
    lane = 4;

    rc = prvCpssDxCh3PortSerdesPowerUpDownGet(devNum, portNum, oldPowerUpPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* the current state is the same as the requested one, return */
    if( powerUp == *oldPowerUpPtr )
    {
        return GT_OK;
    }

    if( powerUp == GT_FALSE )
    {
        regData = 0x0;
    }
    else
    {
        regData = 0xFF;
    }

    /* PowerUp Register 1 */
    rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, regMask, regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* PowerUp Register 2 */
    rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr2, regMask, regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    if( powerUp == GT_FALSE )
        return GT_OK;

    if( portNum < 24 )
    {
        /* Wait at least 10 usec */
        HW_WAIT_MILLISECONDS_MAC(1);

        /* Analog Reset */
        regMask = 0xFFF;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesReset[0];

        if (prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, regMask, 0xFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* NP electrical configurations, NP SERDES powerup */
        for( i = 0 ; i < lane ; i++ )
        {
            /* Calibration Conf register, start automatic impedance calibration and PLL calibration */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesCalibConf1[i];

            if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x00003333) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        /* Wait at least 5 msec */
        HW_WAIT_MILLISECONDS_MAC(5);


        regData = 4;

        for( i = 0 ; i < lane ; i++ )
        {

            /* Reset TxSyncFIFO */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesMiscConf[i];

            if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regData) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        /* Rx\DTL --> Normal operation */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesReset[0];

        if (prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, regMask, 0x0) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        regData = 0xC;

        for( i = 0 ; i < lane ; i++ )
        {
            /* DeAssert Reset TxSyncFIFO */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesMiscConf[i];

            if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regData) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        for( i = 0 ; i < lane ; i++ )
        {
            /* RX Conf. Register - FFE_RES_SEL=4, FFE_CAP_SEL=0, RESET_INTP_EXT_RX=1 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesRxConfReg1[i];

            if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x00004C22) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        for( i = 0 ; i < lane ; i++ )
        {
            /* RX Conf. Register - FFE_RES_SEL=4, FFE_CAP_SEL=0, RESET_INTP_EXT_RX=0 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesRxConfReg1[i];

            if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x00000C22) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        rc = prvCpssDxCh3PortSerdesPowerUpBitMapGet(devNum, &networkSerdesPowerUpBmp);
        if(rc != GT_OK)
        {
            return rc;
        }

        return prvCpssDxCh3HwPpPllUnlockWorkaround(devNum, networkSerdesPowerUpBmp, GT_TRUE);
    }
    else /* SP */
    {
        /* SP SERDES Powerup */

        /* Wait at least 10 usec */
        HW_WAIT_MILLISECONDS_MAC(1);

        /* Analog Reset, Reset calibration logic */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesReset[0];

        if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x0000FFF0) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        for( lane = 0 ; lane < 4 ; lane++ )
        {
            /* Start PLL Calibration */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[portNum].serdesCalibConf1[lane];

            if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x00000400) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        for( lane = 0 ; lane < 4 ; lane++ )
        {
                /* Start DLL Calibration */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    macRegs.perPortRegs[portNum].serdesCalibConf2[lane];

                if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x00000001) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
        }

        for( lane = 0 ; lane < 4 ; lane++ )
        {
                /* Start Impedance Calibration */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    macRegs.perPortRegs[portNum].serdesCalibConf1[lane];

                if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x00000401) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
        }

        for( lane = 0 ; lane < 4 ; lane++ )
        {
                /* Start Process Calibration */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    macRegs.perPortRegs[portNum].serdesProcessMon[lane];

                if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x00001B61) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
        }

        for( lane = 0 ; lane < 4 ; lane++ )
        {
                /* Start Sampler Calibration */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    macRegs.perPortRegs[portNum].serdesCalibConf2[lane];

                if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x00000041) != GT_OK)
                {
                    return GT_HW_ERROR;
                }
        }

        for( lane = 0 ; lane < 4 ; lane++ )
        {
            /* Reset TxSyncFIFO; Sample Tx Data at Posedge */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                macRegs.perPortRegs[portNum].serdesMiscConf[lane];

            if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x00000004) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

        /* Wait at least 10 usec */
        HW_WAIT_MILLISECONDS_MAC(1);

        /* Rx/DTL --> Normal operation, per Group (each group contains 4 ports) */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesReset[0];

        if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x00000000) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Wait at least 10 msec */
        HW_WAIT_MILLISECONDS_MAC(10);

        /* DeAssert Reset TxSyncFIFO; Sample Tx Data at Posedge */
        for( lane = 0 ; lane < 4 ; lane++ )
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesMiscConf[lane];

            if (prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0x0000000C) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortLionSerdesSpeedSet
*
* DESCRIPTION:
*       Configure Lion SerDes to specific frequency
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       serdesSpeed - speed on which serdes mist work
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortLionSerdesSpeedSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum,
    IN  PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed
)
{
    GT_U32 portGroupId, regAddr, localPort, regValue, regMask, i, globalPort;
    GT_U32 startSerdes = 0, serdesesNum = 0;
    GT_U8  tmpStartSerdes = 0, tmpSerdesesNum = 0;
    GT_STATUS rc;
    GT_U32 extCfgIdx = 1;
    GT_U32 ffeRegValue,transmitReg1Value;
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;
    GT_U32 (*lpSerdesConfigArr)[PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E];
    GT_U32 (*lpSerdesExtConfigArr)[2];
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT  serdesSpeedPrev;

    rc = serDesConfig(devNum,&lpSerdesConfigArr,&lpSerdesExtConfigArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if((rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode)) != GT_OK)
        return rc;

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&tmpStartSerdes,&tmpSerdesesNum)) != GT_OK)
        return rc;
    else
    {
        /* to ensure correct assignment in BE mode */
        startSerdes = ((GT_U32)tmpStartSerdes) & 0xFF;
        serdesesNum = ((GT_U32)tmpSerdesesNum) & 0xFF;
    }

    /* if serdeses already configured as needed exit and if their configuration not compatible error */
    if((ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E) && ((portNum%2) != 0))
    {
        if(PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum-1) == CPSS_PORT_INTERFACE_MODE_XGMII_E)
        {
            rc = prvGetLpSerdesSpeed(devNum,portGroupId,startSerdes-2,&serdesSpeedPrev);
            if (rc != GT_OK)
            {
                return rc;
            }
            if(serdesSpeedPrev != serdesSpeed)
                return GT_BAD_STATE;
            else
                goto unreset_serdes_and_xpcs; /* needed speed already configured */
        }
    }    

    /* save Transmitter Register1 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes].transmitterReg1;
    if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &transmitReg1Value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* save FFE Register0 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[startSerdes].ffeReg;
    if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &ffeRegValue) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* In Lion ports_num per port group = serdes_num / 2 */
    PRV_PER_PORT_LOOP_MAC
    {/* Disable port and assert port reset */
        globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,localPort);
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,globalPort,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,globalPort),&regAddr);
        if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) != PRV_CPSS_PORT_GE_E)
        {
            /* unset PortEn and macReset bits */
            if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                        0, 2, 0)) != GT_OK)
                return rc;
            SERDES_DBG_DUMP_MAC(("regAddr=0x%08x\n", regAddr));
        }
        else
        {
            /* unset PortEn bit */
            if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                        0, 1, 0)) != GT_OK)
                return rc;
            SERDES_DBG_DUMP_MAC(("regAddr=0x%08x\n", regAddr));

            PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,globalPort,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,globalPort),&regAddr);
            /* set MACResetn bit */
            if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                        6, 1, 1)) != GT_OK)
                return rc;
            SERDES_DBG_DUMP_MAC(("regAddr=0x%08x\n", regAddr));
        }
    }

    HW_WAIT_MILLISECONDS_MAC(20);

    if((serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E)
            || (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_GE_E))
    {/* Set mode to 10bit <SDSel10bitMode> */
        SERDES_DBG_DUMP_MAC(("extCfgIdx = 0;\n"));
        extCfgIdx = 0;
    }

    /* Pre Configuration for all serdeses, */
    /* enables access to serdes registers */
    PRV_PER_SERDES_LOOP_MAC
    {
        rc = prvCpssDxChLpPreInitSerdesSequence(devNum, portGroupId, i,
                lpSerdesExtConfigArr[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_E][extCfgIdx]);
        if (rc != GT_OK)
            return rc;
    }

    HW_WAIT_MILLISECONDS_MAC(20);

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].pllIntpReg1;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG1_E    ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].pllIntpReg2;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E    ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].pllIntpReg3;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_1_E  ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].pllIntpReg4;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG4_E    ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].pllIntpReg5;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG5_E    ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].analogReservReg;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_ANALOG_REG0_E      ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].calibrationReg1;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG1_E ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].calibrationReg2;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG2_E ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].calibrationReg3;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG3_0_E][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].calibrationReg5;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG5_E ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].transmitterReg0;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_1_E  ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }
    
    HW_WAIT_MILLISECONDS_MAC(10); /* 5ms in AN */

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].transmitterReg0;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_E    ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].transmitterReg1;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,transmitReg1Value);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,transmitReg1Value=0x%08x\n", regAddr, transmitReg1Value));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].transmitterReg2;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG2_E    ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].receiverReg1;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG1_E    ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].receiverReg2;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG2_E    ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].ffeReg;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,ffeRegValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,ffeRegValue=0x%08x\n", regAddr, ffeRegValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].slcReg;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_SLC_REG_E          ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].referenceReg;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_REFERENCE_REG1_E   ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].resetReg;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_0_E     ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    HW_WAIT_MILLISECONDS_MAC(20);

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].calibrationReg3;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG3_1_E][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].calibrationReg7;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG7_E ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].resetReg;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_1_E     ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].resetReg;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_2_E     ][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    HW_WAIT_MILLISECONDS_MAC(20);

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].calibrationReg0;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_0_E][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].calibrationReg0;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_1_E][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].calibrationReg0;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_2_E][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }
    HW_WAIT_MILLISECONDS_MAC(20);

    /*--- End of SerDes Configuration --- */

unreset_serdes_and_xpcs:

        regMask = FIELD_MASK_NOT_MAC(11,1);

    PRV_PER_SERDES_LOOP_MAC
    {
        /* Set RxInit to 0x1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].serdesExternalReg2;
        regValue = lpSerdesExtConfigArr[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_2_E][extCfgIdx];
        rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr,regMask,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    PRV_PER_SERDES_LOOP_MAC
    {
        /* Set RxInit to 0x0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].serdesExternalReg2;
        regValue = lpSerdesExtConfigArr[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_3_E][extCfgIdx];
        rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr,regMask,regValue);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, regValue));
    }

    /* Set RxInit to 0x1 */
    rc = prvCpssDxChPortSerdesResetStateSet(devNum,portNum,startSerdes,serdesesNum,GT_FALSE);
    if (rc != GT_OK)
        return rc;

    PRV_PER_PORT_LOOP_MAC
    {
        /* XPCS Configurations */
        globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,localPort);
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[globalPort].xgGlobalConfReg0;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr, 0x46);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, 0x46));
    }

    if (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_156_E)
    {/* XLG PCS reset disable */
        regAddr     = 0x8803214; /* PCS40G Common Control */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,9,1,0);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x\n", regAddr));
    }
    else
    {/* De assert PCS Reset */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].xgGlobalConfReg0;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr, 0x47);
        if (rc != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regValue=0x%08x\n", regAddr, 0x47));
    }

    HW_WAIT_MILLISECONDS_MAC(20);

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChPortLionSerdesPowerUpSet
*
* DESCRIPTION:
*       Set power up state to Lion port and serdes.
*
* APPLICABLE DEVICES: Lion
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortLionSerdesPowerUpSet
(
    IN  GT_U8                               devNum,
    IN  GT_U8                               portNum
)
{
    GT_U32 portGroupId, regAddr;
    GT_STATUS rc;
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    /* Enable port and De-assert port reset - just for really working port */
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
    if(portMacType != PRV_CPSS_PORT_GE_E)
    { /* if not sgmii or 1000baseX */

        /* set PortEn bit */
        if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                    0, 2, 0x3)) != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x\n", regAddr));
    }
    else
    {
        /* set PortEn bit */
        if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                    0, 1, 1)) != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x\n", regAddr));

        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
        /* set MACResetn bit */
        if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                    6, 1, 0)) != GT_OK)
            return rc;
        SERDES_DBG_DUMP_MAC(("regAddr=0x%08x\n", regAddr));
    }

    HW_WAIT_MILLISECONDS_MAC(20);

    if((rc = cpssDxChPortSerdesResetStateSet(devNum, portNum, GT_FALSE)) != GT_OK)
        return rc;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortLpSerdesPowerUpDownSet
*
* DESCRIPTION:
*       Set power up or down state to port and serdes.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       lanesBmp - lanes bitmap
*       powerUp  - power up state:
*                  GT_TRUE  - power up
*                  GT_FALSE - power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortLpSerdesPowerUpDownSet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    IN  GT_U32                  lanesBmp,
    IN  GT_BOOL                 powerUp
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      regValue;   /* register value   */
    GT_U32      i;          /* loop index       */
    GT_STATUS   rc;         /* return status    */
    GT_U32      portGroupId;     /*the port group Id - support multi-port-groups device */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    GT_U32      startSerdes = 0, serdesesNum = 0;
    GT_U8       tmpStartSerdes = 0, tmpSerdesesNum = 0;
    GT_U32      ffeRegValue,transmitReg1Value;
    GT_U32      serdesExternalRegVal;
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;
    GT_U32 (*lpSerdesConfigArr)[PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E];
    GT_U32 (*lpSerdesExtConfigArr)[2];

    rc = serDesConfig(devNum,&lpSerdesConfigArr,&lpSerdesExtConfigArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if((rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode)) != GT_OK)
        return rc;

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&tmpStartSerdes,&tmpSerdesesNum)) != GT_OK)
        return rc;
    else
    {
        /* to ensure correct assignment in BE mode */
        startSerdes = ((GT_U32)tmpStartSerdes) & 0xFF;
        serdesesNum = ((GT_U32)tmpSerdesesNum) & 0xFF;
    }

    serdesExternalRegVal = lpSerdesExtConfigArr[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_E][1];
    if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
    {
        if (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_GE_E)
        {
            serdesExternalRegVal = lpSerdesExtConfigArr[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_E][0];
            SERDES_DBG_DUMP_MAC(("prvCpssDxChPortLpSerdesPowerUpDownSet:Set serdesExternalRegVal for SGMII=0x%x;portNum=%d\n",
                            serdesExternalRegVal, portNum));
        }
    }

    /* check if all serdes in port are initialized and make proper initialization
       if it is needed */
    rc = prvCpssDxChLpCheckAndInitNeededSerdes(devNum,portGroupId,startSerdes,serdesesNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(powerUp == GT_FALSE)
    {/* no need to configure power register for up, because then we assert serdes reset
      * and this register receives appropriate value automatically
      * NOTE: serdes power down not necessary causes link down - the only sign that it's
      *         down is - no traffic
      */

        for (i = startSerdes; i < startSerdes + serdesesNum;i++)
        {
            /* skip unset lanes */
            if (!((0x1 << (i - startSerdes)) & lanesBmp))
                continue;

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                serdesConfig[i].powerReg;
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, 0);
            if (rc != GT_OK)
            {
                return rc;
            }
            SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regVal==0x%08x\n", regAddr, 0));
            HW_WAIT_MILLISECONDS_MAC(10);
        }
        return GT_OK;
    }

    if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
    {
        rc = prvGetLpSerdesSpeed(devNum,portGroupId,startSerdes,&serdesSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* can't determine serdes speed */
        if (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
        {
            return GT_FAIL;
        }
    }
    else /* NP */
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.extendedGlobalControl;
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 15, 6, &regValue);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(regValue & (1<<(portNum/4)))
        {/* sgmii 1G works on 1.25, 2.5G on 3.125 */
            rc = prvGetLpSerdesSpeed(devNum,portGroupId,startSerdes,&serdesSpeed);
            if (rc != GT_OK)
            {
                return rc;
            }
            /* can't determine serdes speed */
            if (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
            {
                return GT_FAIL;
            }

            /* for 2,5G port speed with 3,125G serdes speed use here value from 1.25 */
            serdesExternalRegVal = lpSerdesExtConfigArr[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_E][0];
        }
        else /* qsgmii */
        {
            serdesSpeed = PRV_CPSS_DXCH_PORT_SERDES_SPEED_5_E;
        }
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        rc = prvCpssDxChPortLionSerdesSpeedSet(devNum,portNum,serdesSpeed);
        if(rc != GT_OK)
            return rc;

        return prvCpssDxChPortLionSerdesPowerUpSet(devNum, portNum);
    }

    /* save Transmitter Register1 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        serdesConfig[startSerdes].transmitterReg1;
    if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &transmitReg1Value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* save FFE Register0 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        serdesConfig[startSerdes].ffeReg;
    if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &ffeRegValue) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Pre Configuration */
    for (i = startSerdes; i < startSerdes + serdesesNum;i++)
    {
        /* skip unset lanes */
        if (!((0x1 << (i - startSerdes)) & lanesBmp))
            continue;
        SERDES_DBG_DUMP_MAC(("prvCpssDxChPortLpSerdesPowerUpDownSet:serdesExternalRegVal=0x%x;portNum=%d,serdes=%d\n",
                        serdesExternalRegVal, portNum, i));
        rc = prvCpssDxChLpPreInitSerdesSequence(devNum,portGroupId,i,serdesExternalRegVal);
        if (rc != GT_OK)
            return rc;
    }
    HW_WAIT_MILLISECONDS_MAC(5);

    /* Configuration */
    for (i = startSerdes; i < startSerdes + serdesesNum;i++)
    {
        /* skip unset lanes */
        if (!((0x1 << (i - startSerdes)) & lanesBmp))
            continue;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].pllIntpReg1;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG1_E][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E][serdesSpeed];

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].pllIntpReg2;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_0_E][serdesSpeed];

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].pllIntpReg3;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

    }
    HW_WAIT_MILLISECONDS_MAC(1);

    for (i = startSerdes; i < startSerdes + serdesesNum;i++)
    {
        /* skip unset lanes */
        if (!((0x1 << (i - startSerdes)) & lanesBmp))
            continue;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_1_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].pllIntpReg3;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG4_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].pllIntpReg4;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG5_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].pllIntpReg5;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_ANALOG_REG0_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].analogReservReg;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG1_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].calibrationReg1;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG2_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].calibrationReg2;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG3_0_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].calibrationReg3;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG5_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].calibrationReg5;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].transmitterReg0;
            regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_1_E  ][serdesSpeed];
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,regValue);
            if (rc != GT_OK)
                return rc;
        
            HW_WAIT_MILLISECONDS_MAC(10); /* 5ms in AN */
        }

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].transmitterReg0;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].transmitterReg1;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, transmitReg1Value);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG2_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].transmitterReg2;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG1_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].receiverReg1;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_RECEIVER_REG2_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].receiverReg2;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].ffeReg;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, ffeRegValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_SLC_REG_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].slcReg;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_REFERENCE_REG1_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].referenceReg;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_0_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].resetReg;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG3_1_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].calibrationReg3;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG7_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].calibrationReg7;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_1_E][serdesSpeed];
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].resetReg;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_RESET_REG0_2_E][serdesSpeed];


        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].resetReg;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

    }

    HW_WAIT_MILLISECONDS_MAC(5);

    for (i = startSerdes; i < startSerdes + serdesesNum;i++)
    {
        /* skip unset lanes */
        if (!((0x1 << (i - startSerdes)) & lanesBmp))
            continue;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_0_E][serdesSpeed];

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].calibrationReg0;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;
        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_1_E][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG0_2_E][serdesSpeed];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;
    }

    HW_WAIT_MILLISECONDS_MAC(1);

    /* Post Configuration */
    for (i = startSerdes; i < startSerdes + serdesesNum;i++)
    {
        /* skip unset lanes */
        if (!((0x1 << (i - startSerdes)) & lanesBmp))
            continue;
        regValue = lpSerdesExtConfig[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_2_E][1];

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].serdesExternalReg2;

        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesExtConfig[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_3_E][1];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;

        regValue = lpSerdesExtConfig[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_4_E][1];
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,regAddr, regValue);
        if (rc != GT_OK)
            return rc;
    }

    if (PRV_CPSS_DXCH_IS_FLEX_LINK_MAC(devNum,portNum) == GT_TRUE)
    {
        if (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
        {
            /* De-Assert XPCS reset */
            for (i = 0;i < serdesesNum; i++)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[portNum].xgGlobalConfReg0;
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr, 0, 3, 0x7);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            /* De-assert mac reset */
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);


            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr, 1, 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            /* De-assert mac reset */
            PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr, 6, 1, 0);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPortXgLanesSwapEnableSet
*
* DESCRIPTION:
*       Enable/Disable swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*
*                - GT_FALSE - Normal operation (no swapping)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortXgLanesSwapEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
)
{

    GT_U32      regAddr;                 /* register address */
    GT_U32      value;                   /* register field value */
    GT_U32      xgGlobalConfRegAddr;     /* XG Global Conf. Reg. 0 address */
    GT_U32      resetState;              /* reset state */
    GT_U32      lane;                    /* iterator */
    GT_STATUS   rc;                      /* return status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* Configure Reset PCS */
    xgGlobalConfRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                  macRegs.perPortRegs[portNum].xgGlobalConfReg0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,xgGlobalConfRegAddr, 0, 1, &resetState);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(resetState != 0)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,xgGlobalConfRegAddr, 0, 1, 0);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    for(lane = 0; lane < 4; lane++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
              macRegs.perPortRegs[portNum].laneConfig1[lane];

        /* Swap Lanes */
        if(enable == GT_TRUE)
        {
            value = (3 - lane) | ((3 - lane) << 3);
        }
        else
        {
            value = lane | (lane << 3);
        }

        /* Configure RxSwpSel / TxSwpSel */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 8, 6, value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* restore PSC Reset state */
    if(resetState != 0)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,xgGlobalConfRegAddr, 0, 1, resetState);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh3PortXgLanesSwapEnableGet
*
* DESCRIPTION:
*       Gets status of swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*
*
* OUTPUTS:
*       enablePtr - pointer to status of swapping XAUI PHY SERDES Lanes.
*                    - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*                    - GT_FALSE - Normal operation (no swapping)
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3PortXgLanesSwapEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
)
{

    GT_U32      regAddr;                 /* register address */
    GT_U32      value;                   /* register field value */
    GT_STATUS   rc;                      /* return status */


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
          macRegs.perPortRegs[portNum].laneConfig1[0];

    /* status of swapping XAUI PHY SERDES Lanes */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 8, 6, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;


}

/*******************************************************************************
* cpssDxChPortXgLanesSwapEnableSet
*
* DESCRIPTION:
*       Enable/Disable swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*
*                - GT_FALSE - Normal operation (no swapping)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgLanesSwapEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
)
{
    GT_U32      regAddr;           /* register address */
    GT_U32      value;             /* register field value */
    GT_U32      resetState;        /* reset state */
    GT_STATUS   rc;                /* return status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        /* supported only on XG ports */
        return GT_BAD_PARAM;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    if(PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_CHEETAH3_E)
    {
        return prvCpssDxChPortXgLanesSwapEnableSet(devNum, portNum, enable);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            smiRegs.smi10GePhyConfig0[portNum];

        /* Get Reset state */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 0, 1, &resetState);

        if(resetState == 1)
        {
            /* Reset XAUI PHY */
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 0, 1, 0);
            if(rc != GT_OK)
            {
                return rc;
            }

        }

        value = resetState | (((enable == GT_TRUE) ? 1 : 0) << 4);

                        /* Set XAUI PHY and Lanes Swap bits */
        return prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId,regAddr, 0x11, value);
    }

}


/*******************************************************************************
* cpssDxChPortXgLanesSwapEnableGet
*
* DESCRIPTION:
*       Gets status of swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*
*
* OUTPUTS:
*       enablePtr - pointer to status of swapping XAUI PHY SERDES Lanes.
*                    - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*                    - GT_FALSE - Normal operation (no swapping)
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgLanesSwapEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_STATUS   rc;      /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        /* supported only on XG ports */
        return GT_BAD_PARAM;
    }

    if(PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))
    {
        return prvCpssDxCh3PortXgLanesSwapEnableGet(devNum, portNum, enablePtr);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            smiRegs.smi10GePhyConfig0[portNum];


        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 4, 1, &value);

        if(rc != GT_OK)
        {
            return rc;
        }

        *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortXgPscLanesSwapSet
*
* DESCRIPTION:
*       Set swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*       rxSerdesLaneArr - array for binding port Rx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*       txSerdesLaneArr - array for binding port Tx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - wrong SERDES lane
*       GT_BAD_VALUE    - multiple connection detected
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgPscLanesSwapSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS],
    IN GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS]
)
{
    GT_U32      regAddr;                 /* register address */
    GT_U32      value;                   /* register field value */
    GT_U32      xgGlobalConfRegAddr;     /* XG Global Conf. Reg. 0 address */
    GT_U32      resetState;              /* reset state */
    GT_U32      lane;                    /* iterator */
    GT_U32      rxLaneBitMap,txLaneBitMap;/* auxilary bit maps to check multiple conn. */
    GT_STATUS   rc;                      /* return status */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(rxSerdesLaneArr);
    CPSS_NULL_PTR_CHECK_MAC(txSerdesLaneArr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        /* supported only on XG ports */
        return GT_BAD_PARAM;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    rxLaneBitMap = 0x0;
    txLaneBitMap = 0x0;

    for( lane = 0 ; lane < CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ; lane++ )
    {
        if(rxSerdesLaneArr[lane] >= CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ||
           txSerdesLaneArr[lane] >= CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS)
        {
            return GT_OUT_OF_RANGE;
        }

        rxLaneBitMap |= ( 1 << rxSerdesLaneArr[lane]);
        txLaneBitMap |= ( 1 << txSerdesLaneArr[lane]);
    }

    if( (rxLaneBitMap != (( 1 << CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ) - 1)) ||
        (txLaneBitMap != (( 1 << CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ) - 1)))
    {
        return GT_BAD_VALUE;
    }

    /* Configure Reset PCS */
    xgGlobalConfRegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                  macRegs.perPortRegs[portNum].xgGlobalConfReg0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,xgGlobalConfRegAddr, 0, 1, &resetState);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(resetState != 0)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,xgGlobalConfRegAddr, 0, 1, 0);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    for( lane = 0 ; lane < CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ; lane++ )
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
              macRegs.perPortRegs[portNum].laneConfig1[lane];

        value = txSerdesLaneArr[lane] + (rxSerdesLaneArr[lane] << 3);

        /* Configure RxSwpSel / TxSwpSel */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, 8, 6, value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* restore PSC Reset state */
    if(resetState != 0)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,xgGlobalConfRegAddr, 0, 1, resetState);
        if(rc != GT_OK)
        {
            return rc;
        }
        HW_WAIT_MILLISECONDS_MAC(10);
    }

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortXgPscLanesSwapGet
*
* DESCRIPTION:
*       Get swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*
* OUTPUTS:
*       rxSerdesLaneArr - array for binding port Rx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*       txSerdesLaneArr - array for binding port Tx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXgPscLanesSwapGet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    OUT GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS],
    OUT GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS]
)
{
    GT_U32      regAddr;                 /* register address */
    GT_U32      value;                   /* register field value */
    GT_U32      lane;                    /* iterator */
    GT_STATUS   rc;                      /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(rxSerdesLaneArr);
    CPSS_NULL_PTR_CHECK_MAC(txSerdesLaneArr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) < PRV_CPSS_PORT_XG_E)
    {
        /* supported only on XG ports */
        return GT_BAD_PARAM;
    }

    for( lane = 0 ; lane < CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS ; lane++ )
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
              macRegs.perPortRegs[portNum].laneConfig1[lane];

        /* Get RxSwpSel / TxSwpSel */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
                regAddr, 8, 6, &value);
        if(rc != GT_OK)
        {
            return rc;
        }

        txSerdesLaneArr[lane] = value & 0x7;
        rxSerdesLaneArr[lane] = ( value >> 3 ) & 0x7;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInBandAutoNegBypassEnableSet
*
* DESCRIPTION:
*       Enable/Disable Auto-Negotiation by pass.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Auto-Negotiation can't be bypassed.
*                - GT_FALSE - Auto-Negotiation is bypassed.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Supported for ports in 1000Base-X mode only.
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssDxChPortInbandAutoNegEnableSet.)
*
*******************************************************************************/
GT_STATUS cpssDxChPortInBandAutoNegBypassEnableSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    IN  GT_BOOL    enable
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        /* not supported on XG ports */
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        macRegs.perPortRegs[portNum].autoNegCtrl;

    value = (enable == GT_TRUE) ? 1 : 0;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 3, 1, value);

}

/*******************************************************************************
* cpssDxChPortInBandAutoNegBypassEnableGet
*
* DESCRIPTION:
*       Gets Auto-Negotiation by pass status.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*
* OUTPUTS:
*       enablePtr - pointer to Auto-Negotiation by pass status.
*                   - GT_TRUE  - Auto-Negotiation can't be bypassed.
*                   - GT_FALSE - Auto-Negotiation is bypassed.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Supported for ports in 1000Base-X mode only.
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssDxChPortInbandAutoNegEnableSet.)
*
*******************************************************************************/
GT_STATUS cpssDxChPortInBandAutoNegBypassEnableGet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum,
    OUT GT_BOOL    *enablePtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register field value */
    GT_STATUS   rc;      /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) >= PRV_CPSS_PORT_XG_E)
    {
        /* not supported on XG ports */
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        macRegs.perPortRegs[portNum].autoNegCtrl;

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 3, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortMacErrorIndicationPortSet
*
* DESCRIPTION:
*       Sets port monitored for MAC errors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port monitored for MAC errors.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       MAC source addresses and the error type can be read
*       for the port. See cpssDxChPortMacErrorIndicationGet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacErrorIndicationPortSet
(
    IN  GT_U8      devNum,
    IN  GT_U8      portNum
)
{
    GT_U32      regAddr; /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.macErrorIndicationPortConfReg;

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum),
            regAddr, 0, 6, portNum);


}

/*******************************************************************************
* cpssDxChPortMacErrorIndicationPortGet
*
* DESCRIPTION:
*       Gets port monitored for MAC errors.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       portNumPtr  - pointer to port monitored for MAC errors.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       MAC source addresses and the error type can be read
*       for the port. See cpssDxChPortMacErrorIndicationGet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacErrorIndicationPortGet
(
    IN  GT_U8      devNum,
    OUT GT_U8      *portNumPtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register value */
    GT_STATUS   rc;      /* return status */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(portNumPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.macErrorIndicationPortConfReg;

    rc = prvCpssDrvHwPpGetRegField(devNum,
            regAddr, 0, 6, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    *portNumPtr = (GT_U8)value;

    return GT_OK;

}

/*******************************************************************************
* cpssDxChPortMacErrorIndicationGet
*
* DESCRIPTION:
*       Gets MAC source addresses and the error type for the monitored port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       typePtr     - pointer to MAC error type.
*       macPtr      - pointer to MAC SA of the last packet received
*                     with  a MAC error on MAC error indication port.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*      Port monitored for MAC errors should be set.
*      See cpssDxChPortMacErrorIndicationPortSet.
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacErrorIndicationGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT   *typePtr,
    OUT GT_ETHERADDR                        *macPtr
)
{
    GT_U32      regAddr; /* register address */
    GT_U32      value;   /* register value */
    GT_STATUS   rc;      /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);


    CPSS_NULL_PTR_CHECK_MAC(macPtr);
    CPSS_NULL_PTR_CHECK_MAC(typePtr);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.macErrorIndicationStatusReg[0];

    rc = prvCpssDrvHwPpReadRegister(devNum,regAddr, &value);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* MAC SA of the last packet received [31:0] */
    macPtr->arEther[5] = (GT_U8)(value & 0xFF);
    macPtr->arEther[4] = (GT_U8)((value >> 8) & 0xFF);
    macPtr->arEther[3] = (GT_U8)((value >> 16) & 0xFF);
    macPtr->arEther[2] = (GT_U8)((value >> 24) & 0xFF);

    /* MAC SA of the last packet received [47:32] */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.macErrorIndicationStatusReg[1];

    rc = prvCpssDrvHwPpGetRegField(devNum,regAddr, 0, 19, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    macPtr->arEther[1] = (GT_U8)(value & 0xFF);
    macPtr->arEther[0] = (GT_U8)((value >> 8) & 0xFF);

    switch((value >> 16) & 7)
    {
        case 0:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_JABBER_E;
            break;
        case 1:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_FRAGMENTS_E;
            break;
        case 2:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_UNDERSIZE_E;
            break;
        case 3:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_OVERSIZE_E;
            break;
        case 4:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_RX_E;
            break;
        case 5:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_CRC_E;
            break;
        case 6:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_OVERRUN_E;
            break;
        case 7:
            *typePtr = CPSS_DXCH_PORT_MAC_ERROR_NO_E;
            break;

    }

    return GT_OK;
}

/******************************************************************************
* cpssDxChPortSerdesPowerStatusSet
*
* DESCRIPTION:
*       Sets power state of SERDES port lanes according to port capabilities.
*       XG / XAUI ports: All 4 (TX or RX) lanes are set simultanuously.
*       HX / QX ports: lanes 0,1 or TX and RX may be set separately.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       direction - may be either CPSS_PORT_DIRECTION_RX_E,
*                                 CPSS_PORT_DIRECTION_TX_E, or
*                                 CPSS_PORT_DIRECTION_BOTH_E.
*       lanesBmp  - bitmap of SERDES lanes (bit 0-> lane 0, etc.)
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_FAIL          - on error
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     DxCh3 and above devices supports only CPSS_PORT_DIRECTION_BOTH_E.
*
******************************************************************************/
GT_STATUS cpssDxChPortSerdesPowerStatusSet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  lanesBmp,
    IN  GT_BOOL                 powerUp
)
{
    GT_STATUS rc;           /* return code */
    GT_BOOL   prevState;    /* previous power state */

    /* Check if device is active and it is a SERDES port */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
    {
        /* no serdes support for FE ports */
        return GT_NOT_SUPPORTED;
    }

    if (PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E)
    {
        rc = prvCpssDxChPortSerdesPowerUpDownSet(devNum, portNum, direction, lanesBmp, powerUp);
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
    {
        if( direction != CPSS_PORT_DIRECTION_BOTH_E )
        {
            return GT_NOT_SUPPORTED;
        }

        rc = prvCpssDxCh3PortSerdesPowerUpDownSet(devNum, portNum, powerUp, &prevState);
    }
    else
    {
        if( direction != CPSS_PORT_DIRECTION_BOTH_E )
        {
            return GT_NOT_SUPPORTED;
        }
        rc = prvCpssDxChPortLpSerdesPowerUpDownSet(devNum, portNum, lanesBmp, powerUp);
    }

    return rc;
}

/******************************************************************************
* prvDxChGroupSerdesPowerUpDownSet
*
* DESCRIPTION:
*      Set Power Up/Down configuration.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum     - physical device number
*       portSerdesGroup  - port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                        0..3        |      0
*                        4..7        |      1
*                        8..11       |      2
*                        12..15      |      3
*                        16..19      |      4
*                        20..23      |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*                DxCh3 XG devices:
*                        0           |      0
*                        4           |      1
*                        10          |      2
*                        12          |      3
*                        16          |      4
*                        22          |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               xCat FE devices
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_FAIL          - on error
*
* COMMENTS:
*       None.
*
******************************************************************************/
static GT_STATUS prvDxChGroupSerdesPowerUpDownSet
(
    IN  GT_U8      devNum,
    IN  GT_U32     portSerdesGroup,
    IN  GT_BOOL    powerUp
)
{
    GT_U32    firstPortInGroup; /* port number*/
    GT_U32    regValue;
    GT_U32    regAddr; /* register address */
    GT_STATUS rc;

    regValue = (powerUp == GT_TRUE) ? 0xFF : 0;

    /*  The function is used for 1G/2.5G ports only
       and every 4 adjencent port belong to same group. */
    firstPortInGroup = portSerdesGroup * 4;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[firstPortInGroup].serdesPowerUp1[0];
    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                macRegs.perPortRegs[firstPortInGroup].serdesPowerUp2[0];
    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);

    return rc;
}

/******************************************************************************
* prvCpssDxChPortFromSerdesGroupGet
*
* DESCRIPTION:
*       Get port number from SERDES port group.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       portSerdesGroup  - port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                        0..3        |      0
*                        4..7        |      1
*                        8..11       |      2
*                        12..15      |      3
*                        16..19      |      4
*                        20..23      |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*                DxCh3 XG devices:
*                        0           |      0
*                        4           |      1
*                        10          |      2
*                        12          |      3
*                        16          |      4
*                        22          |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               xCat FE devices
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*
* OUTPUTS:
*       portNumPtr    - (pointer to)physical port number
*                       that is member in the group.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on wrong portSerdesGroup
*     GT_BAD_PTR       - on NULL pointer
*
* COMMENTS:
*     The port is choosen for CH3 XG device but it OK for Gig ports too.
*
*
******************************************************************************/
static GT_STATUS prvCpssDxChPortFromSerdesGroupGet
(
    IN  GT_U32   portSerdesGroup,
    OUT GT_U32   *portNumPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(portNumPtr);
    switch(portSerdesGroup)
    {
        case 0:
            *portNumPtr = 0;
            break;
        case 1:
            *portNumPtr = 4;
            break;
        case 2:
            *portNumPtr = 10;
            break;
        case 3:
            *portNumPtr = 12;
            break;
        case 4:
            *portNumPtr = 16;
            break;
        case 5:
            *portNumPtr = 22;
            break;
        case 6:
        case 7:
        case 8:
        case 9:
            *portNumPtr = portSerdesGroup + 18;
            break;
        default:
            return GT_BAD_PARAM;
    }
    return GT_OK;
}
/******************************************************************************
* prvCpssDxCh3PortGroupSerdesPowerStatusSet
*
* DESCRIPTION:
*       Sets power state of SERDES port lanes per group
*       according to port capabilities.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum     - physical device number
*       portSerdesGroup  - port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                        0..3        |      0
*                        4..7        |      1
*                        8..11       |      2
*                        12..15      |      3
*                        16..19      |      4
*                        20..23      |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*                DxCh3 XG devices:
*                        0           |      0
*                        4           |      1
*                        10          |      2
*                        12          |      3
*                        16          |      4
*                        22          |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               xCat FE devices
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on wrong devNum, portSerdesGroup
*     GT_FAIL          - on error
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Get SERDES port group according to port by calling to
*     cpssDxChPortSerdesGroupGet.
*
******************************************************************************/
static GT_STATUS prvCpssDxCh3PortGroupSerdesPowerStatusSet
(
    IN  GT_U8      devNum,
    IN  GT_U32     portSerdesGroup,
    IN  GT_BOOL    powerUp
)
{
    GT_U32    portNum; /* port number*/
    GT_U32    i;
    GT_U32    regValue;
    GT_STATUS rc;
    GT_BOOL   prevState;    /* previous power state */
    GT_U32    regAddr; /* register address */
    GT_U32    firstPortInGroup; /* first port in SERDES group */
    /* GT_TRUE - port lane speed 2.5 G  - XAUI port or 2.5 G port */
    /* GT_FALSE - port lane speed is not 2.5 G - 1G port */
    GT_BOOL     portLaneSpeed_2500_En = GT_FALSE;
    GT_BOOL     tempPortLaneSpeed_2500_En = GT_FALSE;
    GT_U32      networkSerdesPowerUpBmp;  /* bitmap of network SERDES to be power UP */

    if(portSerdesGroup > 9)
    {
        return GT_BAD_PARAM;
    }

    /* check if one of ports in the group is XG */
    rc = prvCpssDxChPortFromSerdesGroupGet(portSerdesGroup, &portNum);
    if(rc != GT_OK)
    {
        return rc;
    }
    if(PRV_CPSS_PP_MAC(devNum)->
       phyPortInfoArray[portNum].portType == PRV_CPSS_PORT_XG_E)
    {
        return prvCpssDxCh3PortSerdesPowerUpDownSet(devNum, (GT_U8)portNum,
                                                    powerUp, &prevState);
    }

    else
    {
        /*  Every 4 adjencent port belong to same group. */
        firstPortInGroup = portSerdesGroup * 4;
    }
    /*  Power Down */
    if(powerUp == GT_FALSE)
    {
        return prvDxChGroupSerdesPowerUpDownSet(devNum, portSerdesGroup, powerUp);
    }

    /* check if the port is in 2.5 gig mode */
    for(i = 0; i < 4; i++)
    {
        portNum = firstPortInGroup + i;

        rc = prvCpssDxCh3SerdesSpeedModeGet(devNum, (GT_U8)portNum,
                                        &portLaneSpeed_2500_En);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(i > 0)
        {
            /*  All 4 ports in a group should have a same speed */
            if(tempPortLaneSpeed_2500_En != portLaneSpeed_2500_En)
            {
                return GT_BAD_STATE;
            }
        }
        else
        {
            tempPortLaneSpeed_2500_En = portLaneSpeed_2500_En;
        }
    }

    if(portLaneSpeed_2500_En == GT_TRUE)
    {
        regValue = PRV_SERDES_SPEED_2500_REG_VAL_CNS;
    }
    else
    {
        regValue = PRV_SERDES_SPEED_1000_REG_VAL_CNS;
    }
    for(i = 0; i < 4; i++)
    {
        portNum = firstPortInGroup + i;
        /* Set SERDES speed configuration */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesSpeed1[0];
        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* NP SERDES common register, change NP clock to differential mode
      is configured in the hwPpCheetah3PortsAndSerdesRegsDefault */
    if(portLaneSpeed_2500_En == GT_TRUE)
    {
        regValue = 0x000001F7;
    }
    else
    {
        regValue = 0x00000036;
    }
    for(i = 0; i < 4; i++)
    {
        portNum = firstPortInGroup + i;
        /* Setting electrical parameters */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesSpeed2[0];
        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(portLaneSpeed_2500_En == GT_TRUE)
    {
        regValue = 0x0000F0F0;
    }
    else
    {
        regValue = 0x0000F1F0;
    }
    for(i = 0; i < 4; i++)
    {
        portNum = firstPortInGroup + i;
        /* Reserv Conf Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesReservConf[0];
        /* bit 12 is for metal fix enabling */
        rc = prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, 
                                           0xFFFFEFFF, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    for(i = 0; i < 4; i++)
    {
        portNum = firstPortInGroup + i;
        /* Change TxSyncEn to 1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesTxSyncConf[0];

        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000003);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    for(i = 0; i < 4; i++)
    {
        portNum = firstPortInGroup + i;
        /* RX Conf. Register - FFE_RES_SEL=4, FFE_CAP_SEL=0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesRxConfReg1[0];

        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000C22);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    if(portLaneSpeed_2500_En == GT_TRUE)
    {
        for(i = 0; i < 4; i++)
        {
            portNum = firstPortInGroup + i;
            /* Tx Conf reg2 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesTxConfReg2[0];
            rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000002);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(portLaneSpeed_2500_En == GT_TRUE)
    {
        regValue = 0x0000575A;
    }
    else
    {
        regValue = 0x00005555;
    }
    for(i = 0; i < 4; i++)
    {
        portNum = firstPortInGroup + i;
        /* IVREF Conf Register1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesIvrefConf1[0];

        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(portLaneSpeed_2500_En == GT_TRUE)
    {
        regValue = 0x00000003;
    }
    else
    {
        regValue = 0x00000001;
    }
    for(i = 0; i < 4; i++)
    {

        portNum = firstPortInGroup + i;
        /* IVREF Conf Register2 - Vddr10Pll=2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesIvrefConf2[0];

        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(portLaneSpeed_2500_En == GT_TRUE)
    {
        regValue = 0x00000008;
    }
    else
    {
        regValue = 0x00000009;
    }
    for(i = 0; i < 4; i++)
    {

        portNum = firstPortInGroup + i;
        /* Misc Conf Register - CLK_DET_EN=0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesMiscConf[0];

        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Set PowerUp Registers */
    rc = prvDxChGroupSerdesPowerUpDownSet(devNum, portSerdesGroup, powerUp);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Wait at least 10 usec */
    HW_WAIT_MILLISECONDS_MAC(1);

    /* Analog Reset */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[firstPortInGroup].serdesReset[0];
    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000FF0);
    if(rc != GT_OK)
    {
        return rc;
    }

    for(i = 0; i < 4; i++)
    {
        portNum = firstPortInGroup + i;
        /* Calibration Conf register, start automatic impedance calibration and PLL calibration */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    macRegs.perPortRegs[portNum].serdesCalibConf1[0];

        rc = prvCpssDrvHwPpWriteRegister(devNum,regAddr, 0x00003333);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* Wait at least 5 msec */
    HW_WAIT_MILLISECONDS_MAC(5);

    if(portLaneSpeed_2500_En == GT_TRUE)
    {
        regValue = 0x00000004;
    }
    else
    {
        regValue = 0x00000005;
    }

    for(i = 0; i < 4; i++)
    {
        portNum = firstPortInGroup + i;
        /* Reset TxSyncFIFO; Sample Tx Data at Posedge */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesMiscConf[0];

        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Rx/DTL --> Normal operation, per Group (each group contains 4 ports) */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[firstPortInGroup].serdesReset[0];

    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000000);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(portLaneSpeed_2500_En == GT_TRUE)
    {
        regValue = 0xC;
    }
    else
    {
        regValue = 0xD;
    }
    for(i = 0; i < 4; i++)
    {
        portNum = firstPortInGroup + i;

        /* DeAssert Reset TxSyncFIFO */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                        macRegs.perPortRegs[portNum].serdesMiscConf[0];

        rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(portLaneSpeed_2500_En == GT_TRUE)
    {
        for(i = 0; i < 4; i++)
        {
            portNum = firstPortInGroup + i;
            /* RX Conf. Register - FFE_RES_SEL=4, FFE_CAP_SEL=0, RESET_INTP_EXT_RX=1 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesRxConfReg1[0];

            rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00004C22);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        for(i = 0; i < 4; i++)
        {
            portNum = firstPortInGroup + i;
            /* RX Conf. Register - FFE_RES_SEL=4, FFE_CAP_SEL=0, RESET_INTP_EXT_RX=0 */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            macRegs.perPortRegs[portNum].serdesRxConfReg1[0];

            rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x00000C22);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    rc = prvCpssDxCh3PortSerdesPowerUpBitMapGet(devNum, &networkSerdesPowerUpBmp);
    if(rc != GT_OK)
    {
        return rc;
    }

    return prvCpssDxCh3HwPpPllUnlockWorkaround(devNum, networkSerdesPowerUpBmp, GT_FALSE);
}

/*******************************************************************************
* prvCpssDxCh3PortSerdesPowerUpBitMapGet
*
* DESCRIPTION:
*       Create bitmap of the network SERDES which are powered up.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum                     - device number
*
* OUTPUTS:
*       networkSerdesPowerUpBmpPtr - (pointer to) bitmap of network SERDES
*                                     which are powered up
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*       For GE devices the SERDES bitmap contains one-to-one mapping with the
*       ports.
*       For XG devices the output bitmap will include only powered up ports
*       (due to the use of PRV_CPSS_PHY_PORT_IS_EXIST_MAC), but this complies
*       with the algorithm using this bitmap.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh3PortSerdesPowerUpBitMapGet
(
    IN GT_U8    devNum,
    OUT GT_U32   *networkSerdesPowerUpBmpPtr
)
{
    GT_STATUS rc; /* return code */
    GT_U32 i;     /* loop index */
    GT_BOOL portPowerUp; /* port power up indication */

    *networkSerdesPowerUpBmpPtr = 0x0;
    for( i = 0 ; i < 24 ; i++)
    {
        if(PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, i))
        {
            rc = prvCpssDxCh3PortSerdesPowerUpDownGet(devNum,
                                                      (GT_U8)i,
                                                      &portPowerUp);
            if(rc != GT_OK)
            {
                return rc;
            }
            *networkSerdesPowerUpBmpPtr |= ( portPowerUp == GT_TRUE ) ?
                                            ( 1 << i ) : 0 ;
        }
    }

    return GT_OK;
}

/******************************************************************************
* cpssDxChPortGroupSerdesPowerStatusSet
*
* DESCRIPTION:
*       Sets power state of SERDES port lanes per group
*       according to port capabilities.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum     - physical device number
*       portSerdesGroup  - port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                        0..3        |      0
*                        4..7        |      1
*                        8..11       |      2
*                        12..15      |      3
*                        16..19      |      4
*                        20..23      |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*                DxCh3 XG devices:
*                        0           |      0
*                        4           |      1
*                        10          |      2
*                        12          |      3
*                        16          |      4
*                        22          |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               xCat FE devices
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               Lion devices: Port == SERDES Group
*
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on wrong devNum, portSerdesGroup
*     GT_FAIL          - on error
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Get SERDES port group according to port by calling to
*     cpssDxChPortSerdesGroupGet.
*
******************************************************************************/
GT_STATUS cpssDxChPortGroupSerdesPowerStatusSet
(
    IN  GT_U8      devNum,
    IN  GT_U32     portSerdesGroup,
    IN  GT_BOOL    powerUp
)
{
    GT_U8 portNum;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portSerdesGroup);
        return prvCpssDxChPortLpSerdesPowerUpDownSet(devNum,(GT_U8)portSerdesGroup,0xF,powerUp);
    }
    else if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        if(portSerdesGroup < 6) /* network ports */
        {
            portNum = (GT_U8)(portSerdesGroup * 4);

            if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
            {
                /* no serdes support for FE ports */
                return GT_NOT_SUPPORTED;
            }
        }
        else if (portSerdesGroup < 10) /* Flex Link ports */
        {
            portNum = (GT_U8)(portSerdesGroup + 18);
        }
        else
        {
            return GT_BAD_PARAM;
        }

        return prvCpssDxChPortLpSerdesPowerUpDownSet(devNum,portNum,0xF,powerUp);
    }

    return prvCpssDxCh3PortGroupSerdesPowerStatusSet(devNum, portSerdesGroup,
                                                     powerUp);
}
/******************************************************************************
* cpssDxChPortSerdesGroupGet
*
* DESCRIPTION:
*       Get SERDES port group that may be used in per SERDES group APIs.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portSerdesGroupPtr  - Pointer to port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                        0..3        |      0
*                        4..7        |      1
*                        8..11       |      2
*                        12..15      |      3
*                        16..19      |      4
*                        20..23      |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*                DxCh3 XG devices:
*                        0           |      0
*                        4           |      1
*                        10          |      2
*                        12          |      3
*                        16          |      4
*                        22          |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               xCat FE devices
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               Lion devices: Port == SERDES Group
*
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on wrong devNum, portNum
*     GT_FAIL          - on error
*     GT_BAD_PTR       - on NULL pointer
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
*
******************************************************************************/
GT_STATUS cpssDxChPortSerdesGroupGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    portNum,
    OUT GT_U32   *portSerdesGroupPtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(portSerdesGroupPtr);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        *portSerdesGroupPtr = portNum;
    }
    else /* DxCh3, xCat */
    {
        if(portNum < 24)
        {
            *portSerdesGroupPtr = portNum / 4;
        }
        else
        {
            *portSerdesGroupPtr = portNum - 18;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSerdesCalibrationStartSet
*
* DESCRIPTION:
*       Start calibration on all SERDES of given port.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported port type
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesCalibrationStartSet
(
    IN GT_U8  devNum,
    IN GT_U8  portNum
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      i;          /* loop index       */
    GT_U32      portGroupId;     /*the port group Id - support multi-port-groups device */
    GT_U8       localPort;  /* local port - support multi-port-groups device */
    GT_STATUS   rc = GT_OK; /* return status    */
    GT_U32      startSerdes = 0, serdesesNum = 0;
    GT_U8       tmpStartSerdes = 0, tmpSerdesesNum = 0;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;

    /* Check if device is active and it is a SERDES port */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
    {
        /* no serdes support for FE ports */
        return GT_NOT_SUPPORTED;
    }

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if((rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode)) != GT_OK)
        return rc;

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&tmpStartSerdes,&tmpSerdesesNum)) != GT_OK)
        return rc;
    else
    {
        /* to ensure correct assignment in BE mode */
        startSerdes = ((GT_U32)tmpStartSerdes) & 0xFF;
        serdesesNum = ((GT_U32)tmpSerdesesNum) & 0xFF;
    }

    for (i = startSerdes; i < startSerdes + serdesesNum;i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[i].calibrationReg0;
        /* activate calibration start */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, 15, 1, 1);
        if (rc != GT_OK)
            return rc;
        /* de - activate calibration start */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, 15, 1, 0);
        if (rc != GT_OK)
            return rc;

    }
    return rc;
}


/*******************************************************************************
* cpssDxChPortFlowControlModeSet
*
* DESCRIPTION:
*       Sets Flow Control mode on given port.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number, CPU port number.
*       fcMode     - flow control mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For Lion and above:
*       This function also configures insertion of DSA tag for PFC frames.
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlModeSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  CPSS_DXCH_PORT_FC_MODE_ENT fcMode
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 fieldOffset;/* the start bit number in the register */
    GT_U32 value, pfcValue;      /* register value */
    GT_STATUS rc;      /* function return value */
    CPSS_CSCD_PORT_TYPE_ENT portCscdType;
    GT_BOOL enablePfcCascade;
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    switch (fcMode)
    {
        case CPSS_DXCH_PORT_FC_MODE_802_3X_E:
            value = 0;
            break;
        case CPSS_DXCH_PORT_FC_MODE_PFC_E:
            value = 1;
            break;
        case CPSS_DXCH_PORT_FC_MODE_LL_FC_E:
            value = 2;
            break;
        default:
            return GT_BAD_PARAM;
    }
  
    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);


#ifdef __AX_PLATFORM__
			for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
			{
#else
			portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
				if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
					continue;
#else
					return GT_OK;
#endif


        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if (portMacType >= PRV_CPSS_PORT_XG_E) 
            {
                fieldOffset = 14;
            }
            else
            {
                fieldOffset = 1;
            }
    

            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, fieldOffset, 2, value);
            if (rc != GT_OK)
            {
                return rc;
            }
    
            if (fcMode != CPSS_DXCH_PORT_FC_MODE_802_3X_E)
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.
                    pfcRegs.pfcSrcPortFcMode;
                regAddr += (portNum/32)*4;
                pfcValue = (fcMode == CPSS_DXCH_PORT_FC_MODE_PFC_E) ? 0 : 1;
                rc = prvCpssDxChHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, portNum%32, 1, pfcValue);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
#ifdef __AX_PLATFORM__
    }
#endif

    /* for cascad port and pfc mode, enable insertion of DSA tag to PFC packets */
    if (fcMode == CPSS_DXCH_PORT_FC_MODE_PFC_E)
    {
        /* get cascade port configuration */
        rc = cpssDxChCscdPortTypeGet(devNum,portNum,&portCscdType);
        if (rc != GT_OK)
        {
            return rc;
        }
        enablePfcCascade = (portCscdType == CPSS_CSCD_PORT_NETWORK_E) ?
                            GT_FALSE : GT_TRUE;
    }
    else
    {
        enablePfcCascade = GT_FALSE;
    }

   return prvCpssDxChPortPfcCascadeEnableSet(devNum,portNum,enablePfcCascade);
}


/*******************************************************************************
* cpssDxChPortFlowControlModeGet
*
* DESCRIPTION:
*       Gets Flow Control mode on given port.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number, CPU port number.
*
* OUTPUTS:
*       fcModePtr  - flow control mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortFlowControlModeGet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    OUT CPSS_DXCH_PORT_FC_MODE_ENT   *fcModePtr
)
{

    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 fieldOffset;/* the start bit number in the register */
    GT_U32 value;      /* register value */
    GT_STATUS rc;
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(fcModePtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        fieldOffset = 14;
    }
    else
    {
        fieldOffset = 1;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, fieldOffset, 2, &value);
    if (rc != GT_OK)
    {
        return rc;
    }
    *fcModePtr = (CPSS_DXCH_PORT_FC_MODE_ENT)value;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacResetStateSet
*
* DESCRIPTION:
*       Set MAC Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state    - Reset state
*                  GT_TRUE   - Port MAC is under Reset
*                  GT_FALSE - Port MAC is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortMacResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
{
    return prvCpssDxChPortMacResetStateSet(devNum, portNum, state);
}

/*******************************************************************************
* prvCpssDxChPortSerdesResetStateSet
*
* DESCRIPTION:
*       Set SERDES Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       startSerdes - first SERDES number
*       serdesesNum - number of SERDESes
*       state  - Reset state
*                GT_TRUE  - Port SERDES is under Reset
*                GT_FALSE - Port SERDES is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPortSerdesResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U32    startSerdes,
    IN  GT_U32    serdesesNum,
    IN  GT_BOOL   state
)
{
    GT_U32    regValue;       /* register field value */
    GT_U32    regAddr;        /* register address */
    GT_U32    fieldOffset;    /* register field offset */
    GT_U32    i;              /* iterator */
    GT_U32    portGroupId;    /*the port group Id - support multi-port-groups device */
    GT_U32    localPort;
    GT_STATUS rc;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        regValue = (state == GT_TRUE) ? 1 : 0;
        fieldOffset = 5;
    }
    else
    {
        regValue = (state == GT_TRUE) ? 0 : 1;
        fieldOffset = 3; /* sd_reset_in */
    }

    for (i = startSerdes; i < startSerdes + serdesesNum; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[i].serdesExternalReg2;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, fieldOffset, 1, regValue);
        if(rc !=GT_OK)
        {
            return rc;
        }
    }

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        if((CPSS_PORT_INTERFACE_MODE_XGMII_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum))
            && (CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum+1))
            && (state == GT_TRUE))
        {   /* each bit in Serdes miscellaneous reg. resets all lanes of port accordingly to current
             * interface mode 4 for XAUI, 2 for RXAUI etc., so lets fake here 2 serdes mode
             */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, startSerdes, serdesesNum,
                                                    BIT_MASK_MAC(serdesesNum)) != GT_OK)
            {
                return GT_HW_ERROR;
            }
            HW_WAIT_MILLISECONDS_MAC(20);
        }

        /* Set Serdes reset latency */
        for (i = startSerdes; i < startSerdes + serdesesNum; i++)
        {
            regAddr = 0x9800010 + 0x400*i;
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, 8, 1, 1);
            if(rc !=GT_OK)
            {
                return rc;
            }
        }

        /* sd_reset using Serdes miscellaneous register */
        regAddr = (localPort < 6) ? 0x9800010 : 0x9803010;
        /* if number of lanes is 8 we are in XLG mode - use bit 6 */
        fieldOffset = (serdesesNum < 8) ? (localPort % 6) : 6;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, fieldOffset, 1, regValue);
        if(rc !=GT_OK)
        {
            return rc;
        }

        HW_WAIT_MILLISECONDS_MAC(20);

        if((CPSS_PORT_INTERFACE_MODE_XGMII_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum))
            && (CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E == PRV_CPSS_DXCH_PORT_IFMODE_MAC(devNum,portNum+1))
            && (state == GT_TRUE))
        {/* restore original XAUI media interface mode */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, startSerdes, serdesesNum,
                                                    0) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }
    }

    HW_WAIT_MILLISECONDS_MAC(20);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortSerdesResetStateSet
*
* DESCRIPTION:
*       Set SERDES Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state  - Reset state
*                GT_TRUE   - Port SERDES is under Reset
*                GT_FALSE - Port SERDES is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortSerdesResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
{
    GT_U32    startSerdes = 0, serdesesNum = 0;
    GT_U8     tmpStartSerdes = 0, tmpSerdesesNum = 0;
    GT_STATUS rc;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_FE_E)
    {
        /* no serdes support for FE ports */
        return GT_NOT_SUPPORTED;
    }

    if((rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode)) != GT_OK)
        return rc;

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        return GT_NOT_SUPPORTED;
    }

    if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&tmpStartSerdes,&tmpSerdesesNum)) != GT_OK)
        return rc;
    else
    {
        /* to ensure correct assignment in BE mode */
        startSerdes = ((GT_U32)tmpStartSerdes) & 0xFF;
        serdesesNum = ((GT_U32)tmpSerdesesNum) & 0xFF;
    }

    return prvCpssDxChPortSerdesResetStateSet(devNum,portNum,startSerdes,serdesesNum,state);
}

/*******************************************************************************
* prvCpssDxChLpPreInitSerdesSequence
*
* DESCRIPTION:
*       Make pre-init sequence for given serdes.
*
* INPUTS:
*       devNum              - physical device number
*       portGroupId         - ports group number
*       serdesNum           - given SERDES number
*       serdesExtCnfg1Value - value for Serdes External Cnfg 1 register .
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_INITIALIZED - on not initialized serdes
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpPreInitSerdesSequence
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               serdesNum,
    IN  GT_U32               serdesExtCnfg1Value
)
{
    GT_U32  regAddr, regMask = 0xFFFFFFFF;
    GT_STATUS rc = GT_OK;
    GT_U32 extCfgReg2_0, extCfgReg2_1;

    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->port.serdesRefClock == CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E)
        {
            extCfgReg2_0 = lpSerdesExtConfig_Lion_156Clk[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_0_E][1];
            extCfgReg2_1 = lpSerdesExtConfig_Lion_156Clk[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_1_E][1];
        }
        else
            return GT_NOT_SUPPORTED;

        regMask = FIELD_MASK_NOT_MAC(11,1);
    }
    else /* xcat */
    {
        extCfgReg2_0 = lpSerdesExtConfig[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_0_E][1];
        extCfgReg2_1 = lpSerdesExtConfig[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG2_1_E][1];
    }

    /* Activate SerDes Reset */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg2;
    rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId, regAddr, regMask, extCfgReg2_0);
    if (rc != GT_OK)
        return rc;

    HW_WAIT_MILLISECONDS_MAC(10);

    SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regVal==0x%08x\n", regAddr, extCfgReg2_0));
    SERDES_DBG_DUMP_MAC(("SLEEP 10 msec\n"));

    /* Configuring reference clock accordingly to current interface mode */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg1;
    rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr, serdesExtCnfg1Value);
    if (rc != GT_OK)
        return rc;

    HW_WAIT_MILLISECONDS_MAC(10);

    SERDES_DBG_DUMP_MAC(("prvCpssDxChLpPreInitSerdesSequence:regAddr=0x%08x,serdesExtCnfg1Value==0x%08x\n",
                        regAddr, serdesExtCnfg1Value));
    SERDES_DBG_DUMP_MAC(("SLEEP 10 msec\n"));

    /* Deactivate SerDes Reset - must be done otherwise device stuck when try to access sersed under reset */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg2;
    rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId, regAddr, regMask, extCfgReg2_1);

    HW_WAIT_MILLISECONDS_MAC(10);

    SERDES_DBG_DUMP_MAC(("regAddr=0x%08x,regVal==0x%08x\n", regAddr, extCfgReg2_1));
    SERDES_DBG_DUMP_MAC(("SLEEP 10 msec\n"));

    return rc;
}


/*******************************************************************************
* prvCpssDxChLpCheckSerdesInitStatus
*
* DESCRIPTION:
*       Check LP SERDES initialization status.
*
* INPUTS:
*       devNum        - physical device number
*       portGroupId   - ports group number
*       serdesNum     - SERDES number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_INITIALIZED - on not initialized serdes
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLpCheckSerdesInitStatus
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               serdesNum
)
{
    GT_U32 regAddr;
    GT_STATUS rc = GT_OK;
    GT_U32 fieldData = 0;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        serdesConfig[serdesNum].serdesExternalReg2;
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 3, 1, &fieldData);
    if (rc != GT_OK)
        return rc;
    if (fieldData == 0)
    {
        /* serdes is in reset and its registers are not available */
        rc = GT_NOT_INITIALIZED;
    }

    return rc;
}



/*******************************************************************************
* prvCpssDxChLpSerdesInit
*
* DESCRIPTION:
*       Make serdes initialization and enable access to serdes internal registers
*
* INPUTS:
*       devNum             - physical device number
*       portGroupId        - ports group number
*       serdesNum2Init     - SERDES number that should be initialized
*       serdesNum2CopyInit - previously initialized SERDES number to take init params from
*       isDefaultInit      - GT_TRUE:  use default params for SERDES init
*                            GT_FALSE: use parameters of serdesNum2CopyInit for init
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_INITIALIZED - on not initialized serdes
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLpSerdesInit
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               serdesNum2Init,
    IN  GT_U32               serdesNum2CopyInit,
    IN  GT_BOOL              isDefaultInit
)
{
    /* for ports in RXAUI/HGS/XGMII mode - set serdes speed correspondingly */
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode; /* port inetrface mode */
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed; /* serdes speed/frequency */
    GT_U32 regAddr; /* register address */
    GT_U32 transmitterReg1Value = 0; /* value of TX1 reg */
    GT_U32 ffeRegValue = 0; /* value of FFE reg */
    GT_U32 pllIntpReg2Value = 0; /* value of PLLIntp2 reg */
    GT_U32 pllIntpReg3Value = 0; /* value of PLLIntp3 reg */
    GT_STATUS rc = GT_OK;           /* return code */
    GT_U32 xgMode = 0;          /* interface mode of XG port */
    GT_U32 (*lpSerdesConfigArr)[PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E];
    GT_U32 (*lpSerdesExtConfigArr)[2];

    rc = serDesConfig(devNum,&lpSerdesConfigArr,&lpSerdesExtConfigArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (isDefaultInit == GT_TRUE)
    {
        /* default values will be used */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            serdesSpeed             = serdesFrequency[CPSS_PORT_INTERFACE_MODE_RXAUI_E][CPSS_PORT_SPEED_10000_E];

            transmitterReg1Value    = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_E][serdesSpeed];
            ffeRegValue             = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_E][serdesSpeed];
            pllIntpReg2Value        = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E][serdesSpeed];
            pllIntpReg3Value        = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_1_E][serdesSpeed];
        }
        else
        {
            transmitterReg1Value = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_E][0];
            ffeRegValue = lpSerdesConfigArr[PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_E][0];
            /* set default pllIntpReg2 and pllIntpReg3 for flexLink ports serdes */
            if (serdesNum2Init >= 6)
            {
                /* get flexLink ports mode */
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.sampledAtResetReg;
                if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId,regAddr,26,4,&xgMode) != GT_OK)
                {
                    return GT_HW_ERROR;
                }

                /* for ports in RXAUI/HGS/XGMII mode set serdes speed correspondingly */
                if ((xgMode >> (serdesNum2Init-6) / 4) & 0x1)
                {
                    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;
                    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT serdesSpeed;

                    rc = cpssDxChPortInterfaceModeGet(devNum,(GT_U8)(24+((serdesNum2Init-6)/4)),&ifMode);
                    /* if no interface set -> do nothing */
                    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
                    {
                        return GT_OK;
                    }
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                    serdesSpeed = (ifMode == CPSS_PORT_INTERFACE_MODE_RXAUI_E)
                                    ? PRV_CPSS_DXCH_PORT_SERDES_SPEED_6_25_E : PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E;

                    pllIntpReg2Value = lpSerdesConfig[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E][serdesSpeed];
                    pllIntpReg3Value = lpSerdesConfig[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_1_E][serdesSpeed];
                }
                else
                {   /* in current implementation although flex links support sgmii 2.5G - it's not default mode */
                    pllIntpReg2Value = lpSerdesConfig[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E];
                    pllIntpReg3Value = lpSerdesConfig[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_1_E][PRV_CPSS_DXCH_PORT_SERDES_SPEED_1_25_E];
                }
            }
            else
            {
                rc = cpssDxChPortInterfaceModeGet(devNum,(GT_U8)(24+((serdesNum2Init-6)/4)),&ifMode);
                /* if no interface set -> do nothing */
                if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
                {
                    return GT_OK;
                }
                if (rc != GT_OK)
                {
                    return rc;
                }
                serdesSpeed = (ifMode == CPSS_PORT_INTERFACE_MODE_RXAUI_E)
                                ? PRV_CPSS_DXCH_PORT_SERDES_SPEED_6_25_E : PRV_CPSS_DXCH_PORT_SERDES_SPEED_3_125_E;

                pllIntpReg2Value = lpSerdesConfig[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_E][serdesSpeed];
                pllIntpReg3Value = lpSerdesConfig[PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_1_E][serdesSpeed];
            }
        }
    }
    else
    {
        /* init parameters is taken from another already initialized serdes */
       /* copy transmitterReg1 register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[serdesNum2CopyInit].transmitterReg1;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &transmitterReg1Value);
        if (rc != GT_OK)
            return rc;

        /* copy FFE Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[serdesNum2CopyInit].ffeReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &ffeRegValue);
        if (rc != GT_OK)
            return rc;

        /* copy  pllIntpReg2 Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[serdesNum2CopyInit].pllIntpReg2;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &pllIntpReg2Value);
        if (rc != GT_OK)
            return rc;

        /* copy  pllIntpReg3 Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            serdesConfig[serdesNum2CopyInit].pllIntpReg3;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &pllIntpReg3Value);
        if (rc != GT_OK)
            return rc;
    }

    /* Pre Configuration for serdesNum2Init, */
    /* enables access to serdes registers */
    SERDES_DBG_DUMP_MAC(("prvCpssDxChLpSerdesInit->\n"));
    rc = prvCpssDxChLpPreInitSerdesSequence(devNum, portGroupId, serdesNum2Init,
                                            lpSerdesExtConfig[PRV_CPSS_DXCH_PORT_SERDES_EXT_CFG_REG1_E][1]);
    if (rc != GT_OK)
        return rc;

    HW_WAIT_MILLISECONDS_MAC(5);

    /* set transmitterReg1 register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        serdesConfig[serdesNum2Init].transmitterReg1;
    rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr, transmitterReg1Value);
    if (rc != GT_OK)
        return rc;

    /* set ffeReg register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        serdesConfig[serdesNum2Init].ffeReg;
    rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr, ffeRegValue);
    if (rc != GT_OK)
        return rc;
    /* set  pllIntpReg2 and pllIntpReg3 for flexLink ports serdes */
    if ((serdesNum2Init >= 6) || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        serdesConfig[serdesNum2Init].pllIntpReg2;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr, pllIntpReg2Value);
        if (rc != GT_OK)
            return rc;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        serdesConfig[serdesNum2Init].pllIntpReg3;
        rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr, pllIntpReg3Value);
        if (rc != GT_OK)
            return rc;
    }
    return rc;
}


/*******************************************************************************
* prvCpssDxChLpCheckAndInitNeededSerdes
*
* DESCRIPTION:
*       Make serdes initialization and enable access to serdes internal registers
*
* INPUTS:
*       devNum             - physical device number
*       portGroupId        - ports group number
*       startSerdes        - start SERDES number
*       serdesesNum        - number of  SERDES per port
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_INITIALIZED - on not initialized serdes
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpCheckAndInitNeededSerdes
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               startSerdes,
    IN  GT_U32               serdesesNum
)
{
    GT_BOOL serdesInitMap[PRV_CPSS_DXCH_MAX_SERDES_NUM_PER_PORT_CNS] =
            {GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE,GT_FALSE};
    GT_U32      initializedSerdesNum = 0xff;
    GT_U32      initialSerdeCounter = 0;
    GT_U32      i;
    GT_STATUS   rc = GT_OK;

    /* now check if all serdeses of given port are initialized */
    for (i = startSerdes; i < startSerdes + serdesesNum;i++)
    {
        rc = prvCpssDxChLpCheckSerdesInitStatus(devNum,portGroupId,i);
        if ( (rc != GT_OK) && (rc != GT_NOT_INITIALIZED))
        {
            return rc;
        }
        if (rc != GT_NOT_INITIALIZED)
        {
            if(PRV_CPSS_DXCH_MAX_SERDES_NUM_PER_PORT_CNS <= (i - startSerdes))
                return GT_NOT_SUPPORTED;
            else
                serdesInitMap[i - startSerdes]= GT_TRUE;

            /* save  initialized serdes num */
            initializedSerdesNum = i;
            initialSerdeCounter++;
        }
    }
    if (initialSerdeCounter != serdesesNum)
    {
        /* not all serdeses are initialized */
        for (i = 0; i < serdesesNum; i++)
        {
            if(serdesInitMap[i] == GT_TRUE)
                continue;
            if (initializedSerdesNum != 0xFF)
            {
                /* there is at least one initialized, so take params from it */
                rc = prvCpssDxChLpSerdesInit(devNum,portGroupId,i+startSerdes,initializedSerdesNum,GT_FALSE);
            }
            else
            {
                /* default params */
                rc = prvCpssDxChLpSerdesInit(devNum,portGroupId,i+startSerdes,initializedSerdesNum,GT_TRUE);
            }
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    return rc;
}



/*******************************************************************************
* prvCpssDxChLpGetFirstInitSerdes
*
* DESCRIPTION:
*       Get first initialize serdes per port
*
* INPUTS:
*       devNum             - physical device number
*       portGroupId        - ports group number
*       startSerdes        - start SERDES number
*
*
* OUTPUTS:
*       initSerdesNumPtr   - pointer to first initialize serdes number per port
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_INITIALIZED - on not initialized serdes
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLpGetFirstInitSerdes
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               startSerdes,
    OUT GT_U32               *initSerdesNumPtr
)
{
    GT_U32 serdesNumPerPort; /* number of serdeses used by current port */
    GT_U32 i;   /* counter */
    GT_U32 firstInitializedSerdes =0xff; /* number of first initialized serdes of current port */
    GT_STATUS   rc = GT_OK; /* return code */
    GT_U8 globalPort;   /* port number in box (not in core) */
    GT_U8 tmpStartSerdes = 0, tmpSerdesesNum = 0; /* dummy values for types translation */
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;    /* port interface mode */
    GT_U32 localPort;

    /* check if given serdes is initialized */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        localPort = startSerdes/2;
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        localPort = (startSerdes < 6) ? startSerdes*4 /* network ports connected by QSGMII four ports on one serdes */ :
                                            24+(startSerdes-6); /* stack ports of xcat2 has just one serdes */
    else /* CPSS_PP_FAMILY_DXCH_XCAT_E */
        localPort = (startSerdes < 6) ? startSerdes*4 /* network ports connected by QSGMII four ports on one serdes */ :
                                            24+((startSerdes-6)/4); /* stack ports of xcat has 4 serdes each */
    
    globalPort = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,portGroupId,localPort);
    rc = prvCpssDxChPortInterfaceModeGet(devNum,globalPort,&ifMode);
    if((rc != GT_OK) && (rc != GT_BAD_STATE))
        return rc;

    if((rc = getNumberOfSerdesLanesForPortLocal(devNum, globalPort, ifMode, &tmpStartSerdes,
                                                &tmpSerdesesNum)) != GT_OK)
    {
        return rc;
    }
    else
    {
        /* to ensure correct assignment in BE mode */
        serdesNumPerPort = ((GT_U32)tmpSerdesesNum) & 0xFF;
    }

    for (i = tmpStartSerdes; i < tmpStartSerdes + serdesNumPerPort; i++)
    {
        rc = prvCpssDxChLpCheckSerdesInitStatus(devNum,portGroupId,i);
        if ((rc != GT_OK)&& (rc != GT_NOT_INITIALIZED))
        {
            return rc;
        }
        if(rc == GT_OK)
        {
            firstInitializedSerdes = i;
            break;
        }
    }
    if (firstInitializedSerdes != 0xff)
    {
        *initSerdesNumPtr = firstInitializedSerdes;
        rc = GT_OK;
    }
    else
    {
        rc = GT_NOT_INITIALIZED;
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChPortMacResetStateSet
*
* DESCRIPTION:
*       Set MAC Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state    - Reset state
*               GT_TRUE   - Port MAC is under Reset
*               GT_FALSE - Port MAC is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   state
)
{
    GT_STATUS rc;
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    /* for 10 Gb interface */
    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
        {
            /* Configure Reset PCS */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                          macRegs.perPortRegs[portNum].xgGlobalConfReg0;

            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                        0, 1, (GT_U32)(!state));
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
        offset = 1;
        value = (state == GT_TRUE) ? 0 : 1;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,portMacType,&regAddr);
        offset = 6;
        value = (state == GT_TRUE) ? 1 : 0;
    }

    /* set MACResetn bit */
    if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                offset, 1, value)) != GT_OK)
        return rc;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXpcsLinkStatus
*
* DESCRIPTION:
*      The function checks XPCS of GE or XG MAC has link synchronized
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       geXpcsStatusPtr - pointer to GE XPCS MAC status result: 
*         GT_TRUE - synchronized,
*         GT_FALSE - not synchronized
*       xgXpcsStatusPtr - pointer to XG XPCS MAC status result: 
*         GT_TRUE - synchronized,
*         GT_FALSE - not synchronized
*                                       
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXpcsLinkStatus
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *geXpcsStatusPtr,
    OUT GT_BOOL *xgXpcsStatusPtr
)
{
    GT_STATUS rc;
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into the register */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    CPSS_NULL_PTR_CHECK_MAC(geXpcsStatusPtr);
    CPSS_NULL_PTR_CHECK_MAC(xgXpcsStatusPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    PRV_CPSS_DXCH_PORT_STATUS_CTRL_REG_MAC(devNum,portNum,&regAddr);    
    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxChXpcsLinkStatus:PORT_STATUS_CTRL_REG addr=0x%x\n", regAddr));
    /* get SynkOk bit */
    if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                14, 1, &value)) != GT_OK)
        return rc;
    AUTODETECT_DBG_PRINT_MAC(("fieldValue=%d\n", value));
    *geXpcsStatusPtr = (value == 1) ? GT_TRUE : GT_FALSE;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].xgXpcsGlobalStatus;
    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxChXpcsLinkStatus:xgXpcsGlobalStatus addr=0x%x\n", regAddr));
    /* get LinkUP bit */
    if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                0, 1, &value)) != GT_OK)
        return rc;
    AUTODETECT_DBG_PRINT_MAC(("fieldValue=%d\n", value));
    *xgXpcsStatusPtr = (value == 1) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChSerdesSignalDetect
*
* DESCRIPTION:
*      The function checks if signal detected on required serdeses
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       numOfSerdesToCheck - how many serdes must be tested to report success
*
* OUTPUTS:
*       statusPtr - pointer to signal detect status result: 
*         GT_TRUE - signal detected on all required serdeses
*         GT_FALSE - otherwise
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChSerdesSignalDetect
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_U8   numOfSerdesToCheck,
    OUT GT_BOOL *statusPtr
)
{
    GT_STATUS rc;
    GT_U8   serdesNum, i;
    GT_U32  regAddr;         /* register address */
    GT_U32  serdesReset, value; /* value of field */
    GT_U32  portGroupId;     /* the port group Id - support multi-port-groups device */
    GT_U32  localPort;       /* the local port number - support multi-port-groups device */

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    serdesNum = (GT_U8)(localPort*2);
    for(i = 0; i < numOfSerdesToCheck; i++)
    {
        /* Deactivate SerDes Reset - must be done otherwise device stuck when try to access sersed under reset */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum+i].serdesExternalReg2;
        if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 3, 1, &serdesReset)) != GT_OK)
            return rc;

        if(serdesReset != 1)
        {
            if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 3, 1, 1)) != GT_OK)
                return rc;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum+i].receiverReg0;
        if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 11, 1, &value)) != GT_OK)
            return rc;

        if(serdesReset != 1)
        {/* Activate SerDes Reset - don't leave not connected serdeses active  */
            if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 3, 1, 0)) != GT_OK)
                return rc;
        }

        /* if on serdes no signal detected stop */
        if(value == 1)
        {
            *statusPtr = GT_FALSE;
            return GT_OK;
        }            
    }

    *statusPtr = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXpcsLanesSyncStatus
*
* DESCRIPTION:
*      The function checks if XG XPCS lanes RX synchronized
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       statusPtr - pointer to XE XPCS lanes RX sync status result: 
*         GT_TRUE - synchronized,
*         GT_FALSE - not synchronized
*                                       
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXpcsLanesSyncStatus
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
)
{
    GT_STATUS   rc;
    GT_U8       laneNum;
    GT_U32      regAddr;     /* register address */
    GT_U32      value;       /* value to write into the register */
    GT_U32      portGroupId; /*the port group Id - support multi-port-groups device */

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    *statusPtr = GT_TRUE;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    for(laneNum = 0; laneNum < PRV_CPSS_DXCH_NUM_LANES_CNS; laneNum++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].laneStatus[laneNum];
        /* get lane status SyncOK bit */
        if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                    4, 1, &value)) != GT_OK)
            return rc;

        if(value == 0)
        {
            *statusPtr = GT_FALSE;
            break;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXpcsLanesDisparityErrStatus
*
* DESCRIPTION:
*      Read value of disparity error counter
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       statusPtr - disparity error happend on one of first 4 XPCS lanes
*         GT_TRUE - happend,
*         GT_FALSE - not happend
*                                       
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       There are 6 XPCS lanes, but currently supported port interface modes use
*       just 4.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXpcsLanesDisparityErrStatus
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
)
{
    GT_STATUS   rc;
    GT_U8       laneNum;
    GT_U32      regAddr;     /* register address */
    GT_U32      value;       /* value to write into the register */
    GT_U32      portGroupId; /*the port group Id - support multi-port-groups device */

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    *statusPtr = GT_FALSE;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    for(laneNum = 0; laneNum < PRV_CPSS_DXCH_NUM_LANES_CNS; laneNum++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            macRegs.perPortRegs[portNum].disparityErrorCounter[laneNum];
        /* read counter */
        if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                    0, 16, &value)) != GT_OK)
            return rc;

        if(value != 0)
        {
            *statusPtr = GT_TRUE;
            break;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLocalSerdesLanesFree
*
* DESCRIPTION:
*      This functions checks if even port isn't occupaing serdes lanes of given 
*       odd port and if it does the tries to release them
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       serdesStatusPtr - local serdes are:
*         GT_TRUE - free,
*         GT_FALSE - occupied without possibility to free automatically
*
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLocalSerdesLanesFree
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *serdesStatusPtr
)
{
    GT_STATUS rc;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;
    GT_U32  macModeSelect, mediaIf;
    GT_U32  regAddr;
    GT_U32  portGroupId, localPort;
    GT_U32  startSerdes;
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    CPSS_NULL_PTR_CHECK_MAC(serdesStatusPtr);    

    *serdesStatusPtr = GT_FALSE;

    if(portNum%2 == 0)
    {
        *serdesStatusPtr = GT_TRUE;
        return GT_OK;
    }

     /* if on even port defined XAUI occupying serdeses of neighbour odd port 
      * no sence to check here RXAUI, but don't return error let 
      * application to use default sequence for all ports */
    rc = cpssDxChPortInterfaceModeGet(devNum,(GT_U8)(portNum-1),&ifMode);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("portNum-1 ifMode=%d\n", ifMode));
    if(CPSS_PORT_INTERFACE_MODE_XGMII_E == ifMode)
        return GT_OK;
   
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum-1);

    /* read current media interface of even port */   
    startSerdes = (GT_U8)(localPort*2);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.mediaInterface;
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, startSerdes, 1,
                                            &mediaIf) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* read current MAC mode of even port */
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum-1);
    PRV_CPSS_DXCH_PORT_MAC_CTRL3_REG_MAC(devNum,portNum-1,portMacType,&regAddr);
    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                            13, 3, &macModeSelect) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* if on even port mac mode GE or media interface not XAUI - serdes lanes free */
    if((0 == macModeSelect) || (1 == mediaIf))
    {
        *serdesStatusPtr = GT_TRUE;
        return GT_OK;
    }

    /* if application hasn't configured yet even port we can free local lanes
     * automatically */
    if(CPSS_PORT_INTERFACE_MODE_NA_E == ifMode)
    {
        /* change current MAC mode of even port to GE (using 1 serdes lane) */
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                13, 3, 0) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        *serdesStatusPtr = GT_TRUE;
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChSerdesConfig
*
* DESCRIPTION:
*      Configures serdes to frequency predefined by port data speed configuration
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       ifMode  - interface mode of current port (to save read it from DB)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChSerdesConfig
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
)
{
    GT_STATUS rc;
    GT_U32  portGroupId;
    GT_U32  startSerdes;
    GT_U8   tmpStartSerdes = 0, tmpSerdesesNum = 0;
    PRV_CPSS_DXCH_PORT_SERDES_SPEED_ENT  serdesSpeed;

    if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&tmpStartSerdes,&tmpSerdesesNum)) != GT_OK)
        return rc;
    else
    {
        /* to ensure correct assignment in BE mode */
        startSerdes = ((GT_U32)tmpStartSerdes) & 0xFF;
    }

    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxChSerdesConfig:tmpStartSerdes=%d,tmpSerdesesNum=%d\n", 
                                tmpStartSerdes, tmpSerdesesNum));

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    rc = prvGetLpSerdesSpeed(devNum,portGroupId,startSerdes,&serdesSpeed);
    if (rc != GT_OK)
    {
        return rc;
    }
    AUTODETECT_DBG_PRINT_MAC(("serdesSpeed=%d\n", serdesSpeed));

    /* can't determine serdes speed */
    if (serdesSpeed == PRV_CPSS_DXCH_PORT_SERDES_SPEED_NA_E)
    {
        return GT_FAIL;
    }

    return prvCpssDxChPortLionSerdesSpeedSet(devNum,portNum,serdesSpeed);
}

/*******************************************************************************
* prvCpssDxChXgmii20GDetect
*
* DESCRIPTION:
*      The function checks if XGMII (XAUI) interface mode and 20G data speed 
*       compatible with given port interface
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       statusPtr - pointer to test result: 
*         GT_TRUE - suggested mode/speed pair compatible with given port interface,
*         GT_FALSE - otherwise
*                                       
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXgmii20GDetect
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
)
{
    GT_STATUS rc;
    GT_BOOL geXpcsStatus, xgXpcsStatus, xpcsLanesSyncStatus, 
            serdesSignalDetected, disparityErrorDiscovered;
    GT_U8   numOfSerdesToCheck = 4;
    CPSS_PORT_INTERFACE_MODE_ENT oddPortIfMode;
    CPSS_PORT_SPEED_ENT oddPortSpeed;

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);    

    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxChXgmii20GDetect\n"));

    /* init status to false - save few lines of code */
    *statusPtr = GT_FALSE;

    if(portNum%2 != 0)
    {/* no sence to check XAUI on odd port, but don't return error let 
      * application to use default sequence for all ports */
        return GT_OK;
    }
    else
    {
        if((rc = cpssDxChPortInterfaceModeGet(devNum, (GT_U8)(portNum+1), &oddPortIfMode)) != GT_OK)
            return rc;
        if(CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E == oddPortIfMode)
        {
            if((rc = cpssDxChPortSpeedGet(devNum, (GT_U8)(portNum+1), &oddPortSpeed)) != GT_OK)
                return rc;
            if(CPSS_PORT_SPEED_20000_E != oddPortSpeed)
                return GT_OK;
        }
    }

    /* port+1 sync ? */
    rc = prvCpssDxChXpcsLinkStatus(devNum, (GT_U8)(portNum+1), &geXpcsStatus, &xgXpcsStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("neighbor port %d sync ? geXpcsStatus=%d,xgXpcsStatus=%d\n", 
                                portNum+1, geXpcsStatus, xgXpcsStatus));

    /* if neigbour odd port already synchronized current port defenetly 
     * not in XAUI mode - exit */
    if(geXpcsStatus || xgXpcsStatus)
        return GT_OK;

    rc = prvCpssDxChSerdesSignalDetect(devNum, portNum, numOfSerdesToCheck, &serdesSignalDetected);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("serdesSignalDetected=%d\n", serdesSignalDetected));

    if(!serdesSignalDetected)
        return GT_OK;

    if((rc = cpssDxChPortInterfaceModeSet(devNum, portNum, CPSS_PORT_INTERFACE_MODE_XGMII_E)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortSpeedAutoNegEnableSet(devNum, portNum, GT_FALSE)) != GT_OK)
        return rc;
    
    if((rc = cpssDxChPortSpeedSet(devNum, portNum, CPSS_PORT_SPEED_20000_E)) != GT_OK)
        return rc;

    if((rc = prvCpssDxChSerdesConfig(devNum, portNum, CPSS_PORT_INTERFACE_MODE_XGMII_E)) != GT_OK)
        return rc;
    
    if((rc = cpssDxChPortDuplexAutoNegEnableSet(devNum, portNum, GT_FALSE)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortDuplexModeSet(devNum, portNum, CPSS_PORT_FULL_DUPLEX_E)) != GT_OK)
        return rc;

    /* Sync OK ? */
    rc = prvCpssDxChXpcsLanesSyncStatus(devNum, portNum, &xpcsLanesSyncStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("xpcsLanesSyncStatus=%d\n", xpcsLanesSyncStatus));

    if(GT_FALSE == xpcsLanesSyncStatus)
        return GT_OK;

    /* disparity errors ? */
    rc = prvCpssDxChXpcsLanesDisparityErrStatus(devNum,portNum,&disparityErrorDiscovered);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("disparityErrorDiscovered=%d\n", disparityErrorDiscovered));

    if(GT_TRUE == disparityErrorDiscovered)
        return GT_OK;

    /* xpcs link ? */
    rc = prvCpssDxChXpcsLinkStatus(devNum, portNum, &geXpcsStatus, &xgXpcsStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("xpcs link ? geXpcsStatus=%d,xgXpcsStatus=%d\n", 
                                geXpcsStatus, xgXpcsStatus));

    if(GT_FALSE == xgXpcsStatus)
        return GT_OK;

    if((rc = prvCpssDxChPortLionSerdesPowerUpSet(devNum, portNum)) != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxChXgmii20GDetect success\n"));

    *statusPtr = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChXgmii10GDetect
*
* DESCRIPTION:
*      The function checks if XGMII (XAUI) interface mode and 10G data speed 
*       compatible with given port interface
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       statusPtr - pointer to test result: 
*         GT_TRUE - suggested mode/speed pair compatible with given port interface,
*         GT_FALSE - otherwise
*                                       
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChXgmii10GDetect
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
)
{
    GT_STATUS rc;
    GT_BOOL geXpcsStatus, xgXpcsStatus, xpcsLanesSyncStatus, 
            serdesSignalDetected;
    GT_U8   numOfSerdesToCheck = 4;
    CPSS_PORT_INTERFACE_MODE_ENT oddPortIfMode;
    CPSS_PORT_SPEED_ENT oddPortSpeed;

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxChXgmii10GDetect\n"));

    /* init status to false - save few lines of code */
    *statusPtr = GT_FALSE;

    if(portNum%2 != 0)
    {/* no sence to check XAUI on odd port, but don't return error let 
      * application to use default sequence for all ports */
        return GT_OK;
    }
    else
    {
        if((rc = cpssDxChPortInterfaceModeGet(devNum, (GT_U8)(portNum+1), &oddPortIfMode)) != GT_OK)
            return rc;
        if(CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E == oddPortIfMode)
        {
            if((rc = cpssDxChPortSpeedGet(devNum, (GT_U8)(portNum+1), &oddPortSpeed)) != GT_OK)
                return rc;
            if(CPSS_PORT_SPEED_10000_E != oddPortSpeed)
                return GT_OK;
        }
    }

    /* port+1 sync ? */
    rc = prvCpssDxChXpcsLinkStatus(devNum, (GT_U8)(portNum+1), &geXpcsStatus, &xgXpcsStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("neighbor port %d sync ? geXpcsStatus=%d,xgXpcsStatus=%d\n", 
                                portNum+1, geXpcsStatus, xgXpcsStatus));

    /* if neigbour odd port already synchronized current port defenetly 
     * not in XAUI mode - exit */
    if(geXpcsStatus || xgXpcsStatus)
        return GT_OK;

    rc = prvCpssDxChSerdesSignalDetect(devNum, portNum, numOfSerdesToCheck, &serdesSignalDetected);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("serdesSignalDetected=%d\n", serdesSignalDetected));

    if(!serdesSignalDetected)
        return GT_OK;

    if((rc = cpssDxChPortInterfaceModeSet(devNum, portNum, CPSS_PORT_INTERFACE_MODE_XGMII_E)) != GT_OK)
        return rc;
    
    if((rc = cpssDxChPortSpeedSet(devNum, portNum, CPSS_PORT_SPEED_10000_E)) != GT_OK)
        return rc;

    if((rc = prvCpssDxChSerdesConfig(devNum, portNum, CPSS_PORT_INTERFACE_MODE_XGMII_E)) != GT_OK)
        return rc;

    /* Sync OK ? */
    rc = prvCpssDxChXpcsLanesSyncStatus(devNum, portNum, &xpcsLanesSyncStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("xpcsLanesSyncStatus=%d\n", xpcsLanesSyncStatus));

    if(GT_FALSE == xpcsLanesSyncStatus)
        return GT_OK;

    /* xpcs link ? */
    rc = prvCpssDxChXpcsLinkStatus(devNum, portNum, &geXpcsStatus, &xgXpcsStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("xpcs link ? geXpcsStatus=%d,xgXpcsStatus=%d\n", 
                                geXpcsStatus, xgXpcsStatus));

    if(GT_FALSE == xgXpcsStatus)
        return GT_OK;

    if((rc = prvCpssDxChPortLionSerdesPowerUpSet(devNum, portNum)) != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxChXgmii10GDetect success\n"));

    *statusPtr = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChRxauiDetect
*
* DESCRIPTION:
*      The function checks if RXAUI interface mode and 10G data speed 
*       compatible with given port interface
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       statusPtr - pointer to test result: 
*         GT_TRUE - suggested mode/speed pair compatible with given port interface,
*         GT_FALSE - otherwise
*                                       
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChRxauiDetect
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
)
{
    GT_STATUS rc;
    GT_BOOL geXpcsStatus, xgXpcsStatus, xpcsLanesSyncStatus, 
            serdesSignalDetected, serdesStatus;
    GT_U8   numOfSerdesToCheck = 2;

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);    

    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxChRxauiDetect\n"));

    /* init status to false - save few lines of code */
    *statusPtr = GT_FALSE;

    rc = prvCpssDxChLocalSerdesLanesFree(devNum,portNum,&serdesStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("serdesStatus=%d\n", serdesStatus));
    if(!serdesStatus)
        return GT_OK;

    rc = prvCpssDxChSerdesSignalDetect(devNum, portNum, numOfSerdesToCheck, &serdesSignalDetected);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("serdesSignalDetected=%d\n", serdesSignalDetected));
    if(!serdesSignalDetected)
        return GT_OK;

    if((rc = cpssDxChPortInterfaceModeSet(devNum, portNum, CPSS_PORT_INTERFACE_MODE_RXAUI_E)) != GT_OK)
        return rc;
    
    if((rc = cpssDxChPortSpeedSet(devNum, portNum, CPSS_PORT_SPEED_10000_E)) != GT_OK)
        return rc;

    if((rc = prvCpssDxChSerdesConfig(devNum, portNum, CPSS_PORT_INTERFACE_MODE_RXAUI_E)) != GT_OK)
        return rc;

    /* Sync OK ? */
    rc = prvCpssDxChXpcsLanesSyncStatus(devNum, portNum, &xpcsLanesSyncStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("xpcsLanesSyncStatus=%d\n", xpcsLanesSyncStatus));
    if(GT_FALSE == xpcsLanesSyncStatus)
        return GT_OK;

    /* xpcs link ? */
    rc = prvCpssDxChXpcsLinkStatus(devNum, portNum, &geXpcsStatus, &xgXpcsStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("xpcs link ? geXpcsStatus=%d,xgXpcsStatus=%d\n", geXpcsStatus, xgXpcsStatus));
    if(GT_FALSE == xgXpcsStatus)
        return GT_OK;

    if((rc = prvCpssDxChPortLionSerdesPowerUpSet(devNum, portNum)) != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxChRxauiDetect success\n"));

    *statusPtr = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxCh1000BaseXDetect
*
* DESCRIPTION:
*      The function checks if 1000BaseX interface mode and 1G data speed 
*       compatible with given port interface
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       statusPtr - pointer to test result: 
*         GT_TRUE - suggested mode/speed pair compatible with given port interface,
*         GT_FALSE - otherwise
*                                       
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxCh1000BaseXDetect
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
)
{
    GT_STATUS rc;
    GT_BOOL geXpcsStatus, xgXpcsStatus, serdesSignalDetected, serdesStatus;
    GT_U8   numOfSerdesToCheck = 1;
    GT_U32 portGroupId, regAddr;

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxCh1000BaseXDetect\n"));

    /* init status to false - save few lines of code */
    *statusPtr = GT_FALSE;

    rc = prvCpssDxChLocalSerdesLanesFree(devNum,portNum,&serdesStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("serdesStatus=%d\n", serdesStatus));
    if(!serdesStatus)
        return GT_OK;

    rc = prvCpssDxChSerdesSignalDetect(devNum, portNum, numOfSerdesToCheck, &serdesSignalDetected);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("serdesSignalDetected=%d\n", serdesSignalDetected));
    if(!serdesSignalDetected)
        return GT_OK;

    if((rc = cpssDxChPortInterfaceModeSet(devNum, portNum, CPSS_PORT_INTERFACE_MODE_1000BASE_X_E)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortSpeedAutoNegEnableSet(devNum, portNum, GT_FALSE)) != GT_OK)
        return rc;
    
    if((rc = cpssDxChPortSpeedSet(devNum, portNum, CPSS_PORT_SPEED_1000_E)) != GT_OK)
        return rc;

    if((rc = prvCpssDxChSerdesConfig(devNum, portNum, CPSS_PORT_INTERFACE_MODE_1000BASE_X_E)) != GT_OK)
        return rc;

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_PORT_GE_E,&regAddr);
    /* set MACResetn bit */
    if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                6, 1, 0)) != GT_OK)
        return rc;

    /* xpcs link ? */
    rc = prvCpssDxChXpcsLinkStatus(devNum, portNum, &geXpcsStatus, &xgXpcsStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("xpcs link ? geXpcsStatus=%d,xgXpcsStatus=%d\n", geXpcsStatus, xgXpcsStatus));
    if(GT_FALSE == geXpcsStatus)
        return GT_OK;

    if((rc = prvCpssDxChPortLionSerdesPowerUpSet(devNum, portNum)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortInBandAutoNegBypassEnableSet(devNum, portNum, GT_TRUE)) != GT_OK)
    {
        return rc;
    }

    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxCh1000BaseXDetect success\n"));

    *statusPtr = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChSGMII1GDetect
*
* DESCRIPTION:
*      The function checks if SGMII interface mode and 1G data speed 
*       compatible with given port interface
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       statusPtr - pointer to test result: 
*         GT_TRUE - suggested mode/speed pair compatible with given port interface,
*         GT_FALSE - otherwise
*                                       
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChSGMII1GDetect
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *statusPtr
)
{
    GT_STATUS rc;
    GT_BOOL geXpcsStatus, xgXpcsStatus, serdesSignalDetected, serdesStatus;
    GT_U8   numOfSerdesToCheck = 1;
    GT_U32  portGroupId, regAddr;

    CPSS_NULL_PTR_CHECK_MAC(statusPtr);

    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxChSGMII1GDetect\n"));

    /* init status to false - save few lines of code */
    *statusPtr = GT_FALSE;

    rc = prvCpssDxChLocalSerdesLanesFree(devNum,portNum,&serdesStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("serdesStatus=%d\n", serdesStatus));
    if(!serdesStatus)
        return GT_OK;

    rc = prvCpssDxChSerdesSignalDetect(devNum, portNum, numOfSerdesToCheck, &serdesSignalDetected);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("serdesSignalDetected=%d\n", serdesSignalDetected));
    if(!serdesSignalDetected)
        return GT_OK;

    if((rc = cpssDxChPortInterfaceModeSet(devNum, portNum, CPSS_PORT_INTERFACE_MODE_SGMII_E)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortSpeedAutoNegEnableSet(devNum, portNum, GT_FALSE)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortSpeedSet(devNum, portNum, CPSS_PORT_SPEED_1000_E)) != GT_OK)
        return rc;

    if((rc = prvCpssDxChSerdesConfig(devNum, portNum, CPSS_PORT_INTERFACE_MODE_SGMII_E)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortDuplexAutoNegEnableSet(devNum,portNum,GT_FALSE)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortDuplexModeSet(devNum, portNum, CPSS_PORT_FULL_DUPLEX_E)) != GT_OK)
        return rc;

    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_PORT_GE_E,&regAddr);
    /* set MACResetn bit */
    if((rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                6, 1, 0)) != GT_OK)
        return rc;

    /* xpcs link ? */
    rc = prvCpssDxChXpcsLinkStatus(devNum, portNum, &geXpcsStatus, &xgXpcsStatus);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("xpcs link ? geXpcsStatus=%d,xgXpcsStatus=%d\n", geXpcsStatus, xgXpcsStatus));
    if(GT_FALSE == geXpcsStatus)
        return GT_OK;

    if((rc = prvCpssDxChPortLionSerdesPowerUpSet(devNum, portNum)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortInbandAutoNegEnableSet(devNum, portNum, GT_TRUE)) != GT_OK)
    {
        return rc;
    }

    if((rc = cpssDxChPortInBandAutoNegBypassEnableSet(devNum, portNum, GT_FALSE)) != GT_OK)
    {
        return rc;
    }

    AUTODETECT_DBG_PRINT_MAC(("prvCpssDxChSGMII1GDetect success\n"));

    *statusPtr = GT_TRUE;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortModeSpeedAutoDetectAndConfig
*
* DESCRIPTION:
*       Autodetect and configure interface mode and speed of a given port 
*       If the process succeeded the port gets configuration ready for link, 
*       otherwise (none of provided by application options valid) the port 
*       is left in reset mode.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (CPU port not supported)
*       portModeSpeedOptionsArrayPtr ?array of port mode and speed options 
*                                      preferred for application, 
*                                   NULL ?if any of supported options acceptable
*       optionsArrayLen ?length of options array (must be 0 if 
*                                     portModeSpeedOptionsArrayPtr == NULL)
*
*
* OUTPUTS:
*       currentModePtr ?if succeeded ifMode and speed that were configured 
*                        on the port, 
*                        otherwise previous ifMode and speed restored
*
* RETURNS:
*       GT_OK   - if process completed without errors (no matter if mode 
*                                                       configured or not)
*       GT_BAD_PARAM             - on wrong port number or device,
*                                   optionsArrayLen more then possible options
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         ?ifMode/speed pair not supported
*       GT_HW_ERROR              - HW error
*
* COMMENTS:
*       At this moment supported:
*           CPSS_PORT_INTERFACE_MODE_RXAUI_E         10G
*           CPSS_PORT_INTERFACE_MODE_XGMII_E (XAUI)  10G, 20G
*           CPSS_PORT_INTERFACE_MODE_1000BASE_X_E    1G
*           CPSS_PORT_INTERFACE_MODE_SGMII_E         1G
*
*       There is no way to distinguish between SGMII and 1000baseX
*       until the port is connected to an external device. Application
*       can remove not needed mode from portModeSpeedOptionsArrayPtr
*
*******************************************************************************/
GT_STATUS cpssDxChPortModeSpeedAutoDetectAndConfig
(
    IN   GT_U8                    devNum,
    IN   GT_U8                    portNum,
    IN   CPSS_PORT_MODE_SPEED_STC *portModeSpeedOptionsArrayPtr,
    IN   GT_U8                    optionsArrayLen,
    OUT  CPSS_PORT_MODE_SPEED_STC *currentModePtr
)
{
    GT_STATUS rc;
    CPSS_PORT_MODE_SPEED_STC *optionsPtr;
    GT_U8 option, optionsNum, maxOptionsNum, i;
    GT_BOOL status = GT_FALSE;
    GT_U8   numOfSerdesLanes;
    CPSS_PORT_INTERFACE_MODE_ENT originalIfMode, ifMode;
    CPSS_PORT_SPEED_ENT          originalSpeed;
    GT_BOOL   originalSerdesPowerUpState;
    GT_U32 localPort, startSerdes;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(currentModePtr);

    maxOptionsNum = sizeof(defaultPortModeSpeedOptionsArray)/
                                    sizeof(CPSS_PORT_MODE_SPEED_STC);

    if(optionsArrayLen > maxOptionsNum)
    {
        return GT_BAD_PARAM;
    }        

    /* save original states of ifMode, speed, SerdesPowerUp */
    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&originalIfMode);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChPortSpeedGet(devNum,portNum,&originalSpeed);
    if(rc != GT_OK)
        return rc;

    rc = prvCpssDxCh3PortSerdesPowerUpDownGet(devNum, portNum, 
                                                &originalSerdesPowerUpState);
    if(rc != GT_OK)
        return rc;

    AUTODETECT_DBG_PRINT_MAC(("original states:ifMode=%d,speed=%d,serdesPowerUp=%d\n", 
                            originalIfMode, originalSpeed, originalSerdesPowerUpState));
    if(portModeSpeedOptionsArrayPtr != NULL)
    {
        optionsPtr = portModeSpeedOptionsArrayPtr; 
        optionsNum = optionsArrayLen;
    }
    else
    {
        optionsPtr = defaultPortModeSpeedOptionsArray; 
        optionsNum = maxOptionsNum;
    }

    for(option = 0; option < optionsNum; option++)
    {
        if(portModeSpeedOptionsArrayPtr != NULL)
        {
            for(i = 0; i < maxOptionsNum; i++)
            {
                if((optionsPtr[option].ifMode == defaultPortModeSpeedOptionsArray[i].ifMode)
                    && (optionsPtr[option].speed == defaultPortModeSpeedOptionsArray[i].speed))
                {
                    break;
                }
            }

            if(i == maxOptionsNum)
                return GT_NOT_SUPPORTED;
        }
        else
        {
            i = option;
        }

        AUTODETECT_DBG_PRINT_MAC(("test pair:ifMode=%d,speed=%d\n", 
                                    defaultPortModeSpeedOptionsArray[i].ifMode, 
                                    defaultPortModeSpeedOptionsArray[i].speed));

        if((rc = portModeSpeedTestFunctionsArray[i](devNum, portNum, &status)) != GT_OK)
            return rc;

        if(GT_TRUE == status)
        {
            currentModePtr->ifMode = defaultPortModeSpeedOptionsArray[i].ifMode;
            currentModePtr->speed = defaultPortModeSpeedOptionsArray[i].speed;
            break;
        }            
        else /* reset port units */
        {
            if(portNum%2 != 0)
            {/* if on even port defined XAUI occupying serdeses of neighbour odd port 
              * don't reset serdeses of its neighbour odd port */
                rc = cpssDxChPortInterfaceModeGet(devNum,(GT_U8)(portNum-1),&ifMode);
                if(rc != GT_OK)
                    return rc;
                if(CPSS_PORT_INTERFACE_MODE_XGMII_E == ifMode)
                    goto skipSerdesReset;
            }
            
            /* convert the 'Physical port' to local port -- supporting multi-port-groups device */
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);
            startSerdes = localPort*2;
            numOfSerdesLanes = 2;/* always reset only local port */
            if((rc = prvCpssDxChPortSerdesResetStateSet(devNum, portNum, startSerdes, 
                                                        numOfSerdesLanes, GT_TRUE)) != GT_OK)
                return rc;
skipSerdesReset:
            if((rc = prvCpssDxChPortMacResetStateSet(devNum, portNum, GT_TRUE)) != GT_OK)
                return rc;

        }
    }

    /* If API fails to establish link, port mode, speed and Serdes power 
        set to original values */
    if(GT_FALSE == status)
    {
        if(originalIfMode == CPSS_PORT_INTERFACE_MODE_NA_E)
        {
            currentModePtr->ifMode = CPSS_PORT_INTERFACE_MODE_NA_E;
        }
        else
        {
            rc = cpssDxChPortInterfaceModeSet(devNum,portNum,originalIfMode);
            if(rc != GT_OK)
                return rc;
            currentModePtr->ifMode = originalIfMode;
        }

        if(originalSpeed == CPSS_PORT_SPEED_NA_E)
        {
            currentModePtr->speed = CPSS_PORT_SPEED_NA_E;
        }
        else
        {
            rc = cpssDxChPortSpeedSet(devNum,portNum,originalSpeed);
            if(rc != GT_OK)
                return rc;
            currentModePtr->speed = originalSpeed;
        }

        if(GT_TRUE == originalSerdesPowerUpState)
        {
            if((originalIfMode == CPSS_PORT_INTERFACE_MODE_XGMII_E)
                && (portNum%2 == 0))
            {
                numOfSerdesLanes = 4;
            }
            else
            {
                numOfSerdesLanes = 2;
            }
            if((rc = cpssDxChPortSerdesPowerStatusSet(devNum, portNum, CPSS_PORT_DIRECTION_BOTH_E,
                                                        numOfSerdesLanes, GT_TRUE)) != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortForward802_3xEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of 802.3x Flow Control frames to the ingress 
*       pipeline of a specified port. Processing of  802.3x Flow Control frames 
*       is done like regular data frames if forwarding enabled.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*       enable   - GT_TRUE:  forward 802.3x frames to the ingress pipe,
*                  GT_FALSE: do not forward 802.3x frames to the ingress pipe.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       A packet is considered a valid Flow Control packet (i.e., it may be used 
*       to halt the port’s packet transmission if it is an XOFF packet, or to 
*       resume the port’s packets transmission, if it is an XON packet) if all of 
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is 00-01
*       - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortForward802_3xEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL enable
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */
    GT_U32 offset;     /* bit number inside register       */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* type of mac unit of port */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType >= PRV_CPSS_PORT_XG_E)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    value = BOOL2BIT_MAC(enable);

#ifdef __AX_PLATFORM__
			for(portMacType = PRV_CPSS_PORT_GE_E; portMacType < PRV_CPSS_PORT_NOT_APPLICABLE_E; portMacType++)
			{
#else
			portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
#endif
				if((portNum == CPSS_CPU_PORT_NUM_CNS) && (portMacType >= PRV_CPSS_PORT_XG_E))
#ifdef __AX_PLATFORM__
					continue;
#else
					return GT_OK;
#endif


        if(portMacType < PRV_CPSS_PORT_XG_E)
        {
            PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portNum,&regAddr);
            offset = 3;
        }
        else
        {
            PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
            offset = 4;
        }

        if(regAddr != PRV_CPSS_SW_PTR_ENTRY_UNUSED)
        {
            if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, offset, 13, value) != GT_OK)
                return GT_HW_ERROR;
        }
#ifdef __AX_PLATFORM__
    }
#endif

    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortForward802_3xEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x frames forwarding on a specified port 
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*
* OUTPUTS:
*       enablePtr - status of 802.3x frames forwarding
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       A packet is considered a valid Flow Control packet (i.e., it may be used 
*       to halt the port’s packet transmission if it is an XOFF packet, or to 
*       resume the port’s packets transmission, if it is an XON packet) if all of 
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is 00-01
*       - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortForward802_3xEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */
    GT_STATUS rc;      /* return code */
    GT_U32 offset;     /* bit number inside register       */
    PRV_CPSS_PORT_TYPE_ENT portMacType; /* type of mac unit of port */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);
    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            return GT_NOT_SUPPORTED;

        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&regAddr);
        offset = 4;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_SERIAL_PARAM_CTRL_REG_MAC(devNum,portNum,&regAddr);
        offset = 3;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, offset, 1, &value)) != GT_OK)
        return rc;
    
    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortForwardUnknownMacControlFramesEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of unknown MAC control frames to the ingress 
*       pipeline of a specified port. Processing of unknown MAC control frames 
*       is done like regular data frames if forwarding enabled.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*       enable   - GT_TRUE:  forward unknown MAC control frames to the ingress pipe,
*                  GT_FALSE: do not forward unknown MAC control frames to 
*                               the ingress pipe.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       A packet is considered as an unknown MAC control frame if all of 
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is not 00-01 and not 01-01
*           OR
*         Packet’s MAC DA is not 01-80-C2-00-00-01 and not the port’s configured 
*           MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortForwardUnknownMacControlFramesEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    IN  GT_BOOL enable
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    value = BOOL2BIT_MAC(enable);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].serialParameters1;
    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 2, 1, value);
}


/*******************************************************************************
* cpssDxChPortForwardUnknownMacControlFramesEnableGet
*
* DESCRIPTION:
*       Get current status of unknown MAC control frames 
*           forwarding on a specified port 
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number (CPU port not supported)
*
* OUTPUTS:
*       enablePtr   - status of unknown MAC control frames forwarding
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       A packet is considered as an unknown MAC control frame if all of 
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is not 00-01 and not 01-01
*           OR
*         Packet’s MAC DA is not 01-80-C2-00-00-01 and not the port’s configured 
*           MAC Address
*
*******************************************************************************/
GT_STATUS cpssDxChPortForwardUnknownMacControlFramesEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32 regAddr;    /* register address */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 value;      /* register value */
    GT_STATUS rc;      /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].serialParameters1;
    if((rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 2, 1, &value)) != GT_OK)
        return rc;
    
    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}


/* --------------------- debug functions --------------------------------- */


/*******************************************************************************
* debugCpssDxChPortModeSpeedAutoDetectAndConfig
*
* DESCRIPTION:
*       Simple wrapper for port mode and speed auto-detection function
*
* APPLICABLE DEVICES:  
*       Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (not CPU port)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS debugCpssDxChPortModeSpeedAutoDetectAndConfig
(
    IN   GT_U8                    devNum,
    IN   GT_U8                    portNum
)
{
    GT_STATUS rc;
    CPSS_PORT_MODE_SPEED_STC *portModeSpeedOptionsArrayPtr = NULL;
    GT_U8                    optionsArrayLen = 0;
    CPSS_PORT_MODE_SPEED_STC currentMode;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    rc = cpssDxChPortModeSpeedAutoDetectAndConfig(devNum,portNum,
            portModeSpeedOptionsArrayPtr,optionsArrayLen,&currentMode);

    cpssOsPrintf("rc=%d,ifMode=%d,speed=%d\n", rc, currentMode.ifMode, currentMode.speed);

    return rc;
}

/*******************************************************************************
* debugCpssDxChPortSgmii_1G_AutoDetectAndConfig
*
* DESCRIPTION:
*       Simple wrapper for port mode and speed auto-detection function
*       with priority for SGMII 1G mode, needed because at the moment we 
*       can't distinguish between 1000BaseX and SGMII
*
* APPLICABLE DEVICES:  
*       Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (not CPU port)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS debugCpssDxChPortSgmii_1G_AutoDetectAndConfig
(
    IN   GT_U8                    devNum,
    IN   GT_U8                    portNum
)
{
    GT_STATUS rc;    
    GT_U8     optionsArrayLen = 5;
    CPSS_PORT_MODE_SPEED_STC currentMode;
    CPSS_PORT_MODE_SPEED_STC testPortModeSpeedOptionsArray[] = 
    {
        {CPSS_PORT_INTERFACE_MODE_XGMII_E,      CPSS_PORT_SPEED_20000_E},
        {CPSS_PORT_INTERFACE_MODE_XGMII_E,      CPSS_PORT_SPEED_10000_E},
        {CPSS_PORT_INTERFACE_MODE_RXAUI_E,      CPSS_PORT_SPEED_10000_E},    
        {CPSS_PORT_INTERFACE_MODE_SGMII_E,      CPSS_PORT_SPEED_1000_E},
        {CPSS_PORT_INTERFACE_MODE_1000BASE_X_E, CPSS_PORT_SPEED_1000_E}
    };

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    rc = cpssDxChPortModeSpeedAutoDetectAndConfig(devNum,portNum,
            testPortModeSpeedOptionsArray,optionsArrayLen,&currentMode);

    cpssOsPrintf("rc=%d,ifMode=%d,speed=%d\n", rc, currentMode.ifMode, currentMode.speed);

    return rc;
}


/*******************************************************************************
* debugCpssDxChPortLpSerdesDump
*
* DESCRIPTION:
*       Dump Port Serdes Registers
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS debugCpssDxChPortLpSerdesDump
(
    IN  GT_U8  devNum,
    IN  GT_U8  portNum
)
{
    GT_U32 regValue;
    GT_U32 regAddr;
    GT_U32 serdesNum;   /* SERDES number */
    GT_U32 serdesLast;
    GT_STATUS rc;
    GT_U32  portGroupId;    /*the port group Id - support multi-port-groups device */
    GT_U32  localPort;
    GT_U8   tmpStartSerdes;
    GT_U8   tmpSerdesesNum;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    rc = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        cpssOsPrintf("Error:Interface mode not defined\n");
        return GT_NOT_SUPPORTED;
    }

    if((rc = getNumberOfSerdesLanesForPort(devNum,portNum,ifMode,&tmpStartSerdes,&tmpSerdesesNum)) != GT_OK)
        return rc;
    else
    {
        /* to ensure correct assignment in BE mode */
        serdesNum = ((GT_U32)tmpStartSerdes) & 0xFF;
        serdesLast = serdesNum + (((GT_U32)tmpSerdesesNum) & 0xFF);
    }

    for( ; serdesNum < serdesLast ; serdesNum++)
    {
        cpssOsPrintf("\n\r");
        cpssOsPrintf("********************************************************\n");
        cpssOsPrintf("Dump registers for port = %d,  serdesNum = %d           \n",portNum,serdesNum);
        cpssOsPrintf("********************************************************\n");

        /* Serdes External Configuration 1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg1;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Serdes External Configuration 2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg2;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* PLL/INTP Register 1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].pllIntpReg1;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* PLL/INTP Register 2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].pllIntpReg2;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* PLL/INTP Register 3 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].pllIntpReg3;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* PLL/INTP Register 4 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].pllIntpReg4;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* PLL/INTP Register 5 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].pllIntpReg5;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

         /* Analog Reserved Register 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].analogReservReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

           /* Calibration Register 1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].calibrationReg1;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Calibration Register 2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].calibrationReg2;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Calibration Register 3 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].calibrationReg3;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Calibration Register 5 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].calibrationReg5;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

         /* Transmitter Register 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].transmitterReg0;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Transmitter Register 1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].transmitterReg1;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Transmitter Register 2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].transmitterReg2;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

         /* Receiver Register 1 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].receiverReg1;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Receiver Register 2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].receiverReg2;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* FFE Register 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].ffeReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* SLC Register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].slcReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Reference Register 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].referenceReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Reset Register 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].resetReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

         /* Calibration Register 3 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].calibrationReg3;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Calibration Register 7 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].calibrationReg7;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Reset Register 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].resetReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Calibration Register 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].calibrationReg0;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Serdes External Configuration 2 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].serdesExternalReg2;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        cpssOsPrintf("**********************************************************************************\n");
        cpssOsPrintf("Dump Power Register 0, Digital Interface Register 0, Standalone Control Register 0\n");
        cpssOsPrintf("**********************************************************************************\n");
        /* Power Register 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].powerReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);


        /* Digital Interface Register 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].digitalIfReg0;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);

        /* Standalone Control Register 0 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->serdesConfig[serdesNum].standaloneCntrlReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId , regAddr,&regValue);
        if (rc != GT_OK)
           return rc;
        cpssOsPrintf("devNum = %d, portGroupId = %d , regAddr = 0x%X, data = 0x%X\n",devNum, portGroupId , regAddr,regValue);


    }/* for */

    return GT_OK;
}


#ifndef __AX_PLATFORM__
/*******************************************************************************
* cpssDxChPortTypeSet
*
* DESCRIPTION:
*       Sets the port type for specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*       portType    - port type maybe SGMII or 1000BASEX
*                    
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortTypeSet
(
    IN  GT_U8  devNum,
    IN  GT_U8  portNum,
    IN  CPSS_DXCH_PORT_TYPE_E  portType
)
{
    GT_U32 regAddr;             /* register address */
    unsigned int fieldValue;          /* value to write into the register */
    GT_U32 offset;              /* bit number inside register       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    #if 0   /* luoxun -- cpss1.3 */
	/* GE port */
    if((portNum != CPSS_CPU_PORT_NUM_CNS) &&
       (PRV_CPSS_DXCH_PORT_TYPE(devNum,portNum) == PRV_CPSS_PORT_GE_E))
    {
		PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,&regAddr);
    }
    #endif
    /* GE port */
    if((portNum != CPSS_CPU_PORT_NUM_CNS) &&
       (PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum) == PRV_CPSS_PORT_GE_E))
    {
		PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum), &regAddr);
    }
	/* CPU port or XG ports */
    else
    {
		return GT_OK;
    }

	/* set bit1 in Port<n> MAC Control Register0 */
	offset = 1;

	/* 
      	  * 0:port in SGMII mode
      	  * 1:port in 1000BASE-X mode
   	  */
   	if(PORT_TYPE_SGMII == portType)
		fieldValue = 0;
	else if(PORT_TYPE_1000BASEX == portType)
		fieldValue = 1;
	else
		fieldValue = 0;/* default mode SGMII !!!*/
	
    if (prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 1, fieldValue) != GT_OK)
        return GT_HW_ERROR;

    return GT_OK;
}
/*******************************************************************************
* cpssDxChPortGEMacResetEnableSet
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   	- physical device number
*       portNum  	- physical port number, CPU port number
*       enable 	   	- GT_TRUE:  enable port MAC reset,
*                 	 	   GT_FALSE: disable port MAC reset.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortGEMacResetEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
{
    GT_U32 regAddr;
    unsigned int value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

	/*
	  * 0-Port MAC is not reset
	  * 1-Port MAC is reset
	  */
    value = (enable == GT_TRUE) ? 1 : 0;

    #if 0   /* luoxun -- cpss1.3 */
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,&regAddr);
    #endif
    PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);
    
	#if 0
	osPrintf("cpssDxChPortMacResetEnableSet::dev %d port %d %s reg %#0x\r\n", \
				devNum,portNum,(enable==GT_TRUE)?"enable":"disable",regAddr);
	#endif
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 6, 1, value);
}
/*******************************************************************************
* cpssDxChPortXGMacResetEnableSet
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum	   	- physical device number
*       portNum	- physical port number, CPU port number
*       enable 	   	- GT_TRUE:  enable port MAC reset,
*                 	 	   GT_FALSE: disable port MAC reset.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXGMacResetEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
{
    GT_U32 regAddr;
    unsigned int value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

	/*
	  * 0-MAC is under reset
	  * 1-Normal operation
	  */
    value = (enable == GT_FALSE) ? 1 : 0;

    #if 0   /* luoxun -- cpss1.3 */
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,&regAddr);
    #endif
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);
	#if 0
	osPrintf("cpssDxChPortMacResetEnableSet::dev %d port %d %s reg %#0x\r\n", \
				devNum,portNum,(enable==GT_TRUE)?"enable":"disable",regAddr);
	#endif
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 1, 1, value);
}

/*******************************************************************************
* cpssDxChPortInbandAutoNegModeSet
*
* DESCRIPTION:
*       Enable/disable In-Band Auto-Negotiation
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       mode       - SGMII or 1000BASE-X
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortInbandAutoNegModeSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  CPSS_DXCH_PORT_INBAND_AN_MODE mode
)
{
    GT_U32 regAddr;         /* register address */
    unsigned int value;           /* value to write into the register */
    unsigned int portState;       /* current port state (enable/disable) */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 length;          /* number of bits to be written to the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    #if 0   /* luoxun -- cpss1.3 */
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,&regAddr);
    #endif
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);


    /* disable port if we need */
    if (prvPortDisableSet(devNum,portNum, &portState) != GT_OK)
        return GT_HW_ERROR;


    if((portNum != CPSS_CPU_PORT_NUM_CNS)&&
       (PRV_CPSS_DXCH_PORT_TYPE(devNum,portNum) == PRV_CPSS_PORT_XG_E))
    {
    #if 0
        /* bit 8 - RxFcEn, bit 9 - TxFcEn */
        value = (state == GT_TRUE) ? 3 : 0;

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                  perPortRegs[portNum].macCtrl;
        offset = 8;
        length = 2;
	#endif
		return GT_OK;
    }
    else
    {
        value = (mode == PORT_INBAND_AN_SGMII_MODE) ? 1 : 0;

        #if 0   /* luoxun -- cpss1.3*/
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,&regAddr);
        #endif
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

        offset = 0;
        length = 1;
    }

    if (prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, length, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* enable port if we need */
    if (prvPortEnableSet(devNum,portNum, portState) != GT_OK)
        return GT_HW_ERROR;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortInvalidCrcModeSet
*
* DESCRIPTION:
*       Set if packets received from [devNum,portNum] contains 4 bytes of CRC or not.
*
* APPLICABLE DEVICES:  All DxCh2 device
*
* INPUTS:
*       devNum           - physical device number
*       portNum          - port number
*       crcMode          - if contains 4B of CRC or not
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - device type wrong
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortInvalidCrcModeSet
(
    IN GT_U8                             	devNum,
    IN GT_U8                             	portNum,
    IN CPSS_DXCH_PORT_INVALID_CRC_MODE_ENT  crcMode
)
{
    GT_U32 regAddr;                 /* register address */
    GT_STATUS status;               /* return status */
    unsigned int value;             /* write register's value */
    GT_U32 fieldOffset;   			/* register field offset to write to */

    PRV_CPSS_DXCH2_DEV_CHECK_MAC(devNum);
    PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if((crcMode!= CPSS_DXCH_PORT_CONTAIN_4B_CRC_MODE_E)&&
       	(crcMode != CPSS_DXCH_PORT_NOT_CONTAIN_4B_CRC_MODE_E))
    {
        return GT_BAD_PARAM;
    }

    #if 0   /* luoxun -- cpss1.3 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
				              addrSpace.invalidCrcModeConfigReg;
    #endif
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
				              haRegs.invalidCrcModeConfigReg[0];

    value = (crcMode == CPSS_DXCH_PORT_NOT_CONTAIN_4B_CRC_MODE_E) ? 1 : 0;

	if(CPSS_CPU_PORT_NUM_CNS == portNum)
		fieldOffset = 31;
	else
		fieldOffset = portNum;

	status = prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, 1, value);
	if(GT_OK != status)
	{
		osPrintfErr("set dev %d port %d invalid CRC mode error %d\n",devNum,portNum,status);
		return status;
	}
    return status;
}

/*******************************************************************************
* cpssDxChPortIPGSet
*
* DESCRIPTION:
*     Sets the number of IPG on GE-Port, value is in 8 byte resolution
*	  IPG: Minimal Inter-Packet Gap
*	  Default value is 96 Byte(value 0xC)
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       value   -  IPG value resolution in 8 Byte
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortIPGSet
(
    IN  GT_U8       devNum,
    IN  GT_U8       portNum,
    IN	GT_U8		value
)
{
	GT_U32 regAddr ;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(PRV_CPSS_DXCH_PORT_TYPE(devNum,portNum) != PRV_CPSS_PORT_GE_E)
    {
        return GT_NOT_SUPPORTED;
    }
	regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].serialParameters;

    return prvCpssDrvHwPpSetRegField(devNum,
                                     PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.
                                     perPortRegs[portNum].serialParameters,
                                     0, 4, value);
}
GT_STATUS cpssDxChPortIPGGet
(
   IN  GT_U8       devNum,
   IN  GT_U8       portNum,
   OUT GT_U8	   *ipgsize
)
{
    GT_U32 regAddr;             /* register address */
    unsigned int value;         /* value to read the register */
    GT_U32 offset;              /* bit number inside register       */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);


   
   if(PRV_CPSS_DXCH_PORT_TYPE(devNum,portNum) != PRV_CPSS_PORT_GE_E)
	  {
		  return GT_NOT_SUPPORTED;
	  }
	  regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].serialParameters;
      offset = 0;

  
    if (prvCpssDrvHwPpGetRegField(devNum, regAddr, offset, 4, &value) != GT_OK)
        return GT_HW_ERROR;

    if(value >= BIT_4)
    {
        /* 4 bits in HW */
        return GT_OUT_OF_RANGE;
    }
     *ipgsize = value;
    return GT_OK;
}


/*******************************************************************************
* cpssDxChPortLedIntfSet
*
* DESCRIPTION:
*       set led interface end&start value on a specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable port,
*                  GT_FALSE: disable port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortLedIntfSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_BOOL   enable
)
{
	GT_STATUS ret;
    GT_U32 regAddr;
    GT_U32 fieldoffset ;          /* bit number inside register       */
    GT_U32 length = 8;          /* number of bits to be written to the register */
    unsigned int intfIndex = 0,valueEnd = 0,valueStart = 0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

	if(enable){
		/*modify the hw reset value to belows*/
		valueEnd = 0x3B;
		valueStart = 0x24;
	}

	/*interface0*/
	PRV_CPSS_DXCH_PORT_LED_INTF_CTRL_REG(devNum,portNum,0,&regAddr);
	ret = prvCpssDrvHwPpSetRegField(devNum,\
									regAddr,\
									0,\
									8,\
									valueStart);
    ret |= prvCpssDrvHwPpSetRegField(devNum,\
									 regAddr,\
									 8,\
									 8,\
									 valueEnd);
	/*interface1*/
	PRV_CPSS_DXCH_PORT_LED_INTF_CTRL_REG(devNum,portNum,1,&regAddr);
	ret |= prvCpssDrvHwPpSetRegField(devNum,\
									regAddr,\
									0,\
									8,\
									valueStart);
    ret |= prvCpssDrvHwPpSetRegField(devNum,\
									 regAddr,\
									 8,\
									 8,\
									 valueEnd);

	return ret;
}

GT_STATUS cpssDxChXsmiAcTimingSet
(
	IN GT_U8	devNum
)
{
	GT_STATUS ret;
    GT_U32 regAddr;
	unsigned int value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

	PRV_CPSS_DXCH_XSMI_AC_TIMING_REG(devNum,&regAddr);
	if(NULL == regAddr){
		return GT_INIT_ERROR;
	}
	
	value = XSMI_XMDIO_DELAY_VALUE;
	
	return prvCpssDrvHwPpSetRegField(devNum,\
									regAddr,\
									0,\
									2,\
									value);
}

/********************************
 * Set XG port: IPG Mode-- Fixed IPG LAN Mode
 *				IPG Base-- 8 Bytes
 *********************************/
GT_STATUS cpssDxChXGPortIPGModeSet
(
	IN GT_U8	devNum,
	IN GT_U8	portNum,
	IN CPSS_DXCH_NETWORK_XGPORT_IPG_MODE_ENT ipgMode
)
{
    GT_U32 regAddr;         /* register address */
    unsigned int value;           /* value to write into the register */
    unsigned int portState;       /* current port state (enable/disable) */
    GT_U32 portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32 offset;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_PHYSICAL_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    offset = (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) ? 5 : 6;

	/* set XG port IPG Mode :Fixed IPG LAN Mode*/
    if((portNum != CPSS_CPU_PORT_NUM_CNS)&&(PRV_CPSS_DXCH_PORT_TYPE(devNum,portNum) == PRV_CPSS_PORT_XG_E))
    {
        /* bit 6, bit 7 - IPG Mode;set to Fixed IPG LAN */
		switch(ipgMode){
			case CPSS_DXCH_XG_PORT_IPG_LAN_MODE_E:
				value = 0;
			break;
			case CPSS_DXCH_XG_PORT_IPG_WAN_MODE_E:
				value = 1;
			break;
			case CPSS_DXCH_XG_PORT_IPG_FIXED_IPG_LAN_MODE_E:
				value = 2;
			break;
			default :
				return GT_HW_ERROR;
		}
    }
    else
    {
        return GT_HW_ERROR;
    }
	
    if(prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,regAddr,offset, 2, value) != GT_OK)	
    {
        return GT_HW_ERROR;
    }
    return GT_OK;
}


GT_STATUS cpssDxChXGPortIPGBaseSet
(
	IN GT_U8	devNum,
	IN GT_U8	portNum,
	IN CPSS_DXCH_NETWORK_XGPORT_IPG_BASE_ENT ipgBase
)
{
    GT_U32 regAddr;         /* register address */
    unsigned int value;           /* value to write into the register */
    unsigned int portState;       /* current port state (enable/disable) */
    GT_U32 offset;          /* bit number inside register       */
    GT_U32 length;          /* number of bits to be written to the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    #if 0   /* luoxun -- cpss1.3 */
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,&regAddr);
    #endif
    PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);


    /* disable port if we need */
    if (prvPortDisableSet(devNum,portNum, &portState) != GT_OK)
        return GT_HW_ERROR;

	/*set XG Port IPGBase : 8byte*/
    if((portNum != CPSS_CPU_PORT_NUM_CNS)&&
       (PRV_CPSS_DXCH_PORT_TYPE(devNum,portNum) == PRV_CPSS_PORT_XG_E))
    {
        /* bit 10 - IPGBase */
		switch(ipgBase){
			case CPSS_DXCH_XG_PORT_IPG_BASE_BYTE_MIN_E:
				value = 1;
			break;
			case CPSS_DXCH_XG_PORT_IPG_BASE_BYTE_MAX_E:
				value = 0;
			break;
			default:
				return GT_HW_ERROR;
		}

        #if 0   /* luoxun -- cpss1.3 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.perPortRegs[portNum].macCtrl2;
        #endif
        /*regAddr = 0;*/
        PRV_CPSS_DXCH_PORT_MAC_CTRL2_REG_MAC(devNum,portNum,PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum),&regAddr);
        offset = 10;
        length = 1;
    }
    else
    {
        return GT_HW_ERROR;
    }

    if (prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, length, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }
	
    /* enable port if we need */
    if (prvPortEnableSet(devNum,portNum, portState) != GT_OK)
        return GT_HW_ERROR;

    return GT_OK;
}

/**********************************************
 *
 *	set LED interface for XG-Port on 
 *	external Phy :marvell88x2210
 *
 *********************************************/
GT_STATUS cpssDxChXgPortExternalPhyLedIntfSet
(
	GT_U8	devNum,
	GT_U8	portNum
)
{
	GT_STATUS ret = GT_NOT_SUPPORTED;
	GT_U8	phyId = 0;
	GT_U16	phyReg = 0x0;
	GT_U8	phyDev = 0x0;
	GT_U16	data = 0x0;
	GT_U32 i = 0;
	GT_U32 j = 0;
	GT_U32 max_port = 0;
	GT_U32 max_reg = 0;
	unsigned char (*portPtr)[3] = NULL;
	unsigned short (*regPtr)[3] = NULL;
	unsigned short (*tmp_regPtr)[3] = NULL;

	osPrintf("PRODUCT_ID %#x set XG port(%d,%d) LED on external phy\n",
				auteware_product_id,	devNum,portNum);

	switch (auteware_product_id)
	{
		case PRODUCT_ID_AX7K:
		case PRODUCT_ID_AX7K_I:
		{
			max_port = AX7_MAX_XG_PORT_NUMBER;
			max_reg = AX7_MAX_XG_PORT_REG_NUMBER;
			portPtr = prvCpssDxChXGPhySmiAX7;
			regPtr = prvCpssDxChXGPhySmiRegAX7;
			break;
		}
		case PRODUCT_ID_AX5K:
		{
			max_port = AX5_MAX_XG_PORT_NUMBER;
			max_reg = AX5_MAX_XG_PORT_REG_NUMBER;
			portPtr = prvCpssDxChXGPhySmiAX5;
			regPtr = prvCpssDxChXGPhySmiRegAX5;
			break;
		}
		case PRODUCT_ID_AX5K_I:
		{
			max_port = AX5I_MAX_XG_PORT_NUMBER;
			max_reg = AX5I_MAX_XG_PORT_REG_NUMBER;
			portPtr = prvCpssDxChXGPhySmiAX5I;
			regPtr = prvCpssDxChXGPhySmiRegAX5I;
			break;
		}
		case PRODUCT_ID_AU4K:
		{
			max_port = AU4_MAX_XG_PORT_NUMBER;
			max_reg = AU4_MAX_XG_PORT_REG_NUMBER;
			portPtr = prvCpssDxChXGPhySmiAU4;
			regPtr = prvCpssDxChXGPhySmiRegAU4;
			break;
		}
		case PRODUCT_ID_AU3K:
		{
			max_port = AU3_MAX_XG_PORT_NUMBER;
			max_reg = AU3_MAX_XG_PORT_REG_NUMBER;
			portPtr = prvCpssDxChXGPhySmiAU3;
			regPtr = prvCpssDxChXGPhySmiRegAU3;
			break;
		}
		default :
		{
			osPrintfErr("dev %d port %d NOT support external PHY.\n", devNum, portNum);
			return GT_NOT_SUPPORTED;
		}
	}

	for (i = 0; i < max_port; i++, portPtr++)
	{
		phyId = 0x0;
		tmp_regPtr = regPtr;			/* at first reg position per loop */
		if (((*portPtr)[0] == devNum) && ((*portPtr)[1] == portNum))
		{
			phyId = (*portPtr)[2];
			for (j = 0; j < max_reg; j++, tmp_regPtr++)
			{
				phyReg = (*tmp_regPtr)[0];
				phyDev = (*tmp_regPtr)[1];
				data = (*tmp_regPtr)[2];

				osPrintf("set dev %d port %d external XG-phyId %#x phyDev %#x phyReg %#x data %#x\n",
						devNum, portNum, phyId, phyDev, phyReg, data);
				ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, GT_TRUE,
																phyReg, phyDev, data);
				if (GT_OK != ret) {
					osPrintfErr("dev %d port %d external XG-phyId %#x phyDev %#x phyReg %#x data %#x write fail, ret %#x.\n",
								devNum, portNum, phyId, phyDev, phyReg, data, ret);
					return ret;
				}
			}
		}
		else {
			/*
			ret = GT_NOT_SUPPORTED;
			continue;
			*/
			;
		}
	}

	return ret;
}

/*******************************************************************************
* cpssDxChPortConfigSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortConfigSpeedGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
)
{
    GT_U32 regAddr;					/* register address */
    unsigned int value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(speedPtr);

	PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portNum,&regAddr);

    /* read speed of configuration register */
    if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 6, 1, &value) != GT_OK)
        return GT_HW_ERROR;
	if(1 == value) {
        *speedPtr = CPSS_PORT_SPEED_1000_E;	
	}
	else {
        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 5, 1, &value) != GT_OK)
            return GT_HW_ERROR;

        if (value == 1)
        {
            *speedPtr = CPSS_PORT_SPEED_100_E;
        }
        else
        {
            *speedPtr = CPSS_PORT_SPEED_10_E;
        }
	}
    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortConfigFlowControlGet
*
* DESCRIPTION:
*       Gets flow-control for specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortConfigFlowControlGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    OUT GT_BOOL   *statePtr
)
{
    GT_U32 regAddr;					/* register address */
    unsigned int value;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

	PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portNum,&regAddr);

    /* read flow-control of configuration register */
    if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 8, 1, &value) != GT_OK)
        return GT_HW_ERROR;
	if(1 == value) {
        *statePtr = GT_TRUE;	
	}
	else {
        *statePtr = GT_FALSE;
	}
    return GT_OK;
}

#endif

