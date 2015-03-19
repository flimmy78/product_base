/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssDxChPhySmi.c
*
* DESCRIPTION:
*       API implementation for port Core Serial Management Interface facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.6 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <cpss/generic/phy/private/prvCpssGenPhySmi.h>
#include <cpss/dxCh/dxChxGen/phy/private/prvCpssDxChPhySmi.h>
#include <sysdef/npd_sysdef.h>

/***************Private functions ********************************************/
#define CPSS_DX_PHY_ADDR_REG_OFFSET     0x0800000
#define CPSS_DX_SMI_MNG_CNTRL_OFFSET    0x1000000

#define CPSS_DX_SMI_RETRY_COUNTER_CNS       1000


static GT_VOID vctObjInit
(
    IN  GT_U8     devNum
);

static GT_VOID smiObjInit
(
    IN  GT_U8     devNum
);

static GT_STATUS prvCpssDxChPhyPortSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyReg,
    IN  GT_U16    data
);

static GT_STATUS prvCpssDxChPhySmi0NumOfPortsGet
(
  IN  GT_U8     devNum,
  OUT GT_U8     *smi0PortsNumPtr
);


#ifndef __AX_PLATFORM__
extern unsigned int auteware_product_id;

/*******************************************************************************
*  marvell virtual(logical, or MAC) port map to  PHY port 
*  first column is virtual port #
*  second column is PHY port #
**/

/* this MACRO is for AXAX71-2X-12G12S product */
#define AX71_2X12G12S_MAX_SLOTCARD_NUMBER   1 
#define AX71_2X12G12S_MAX_PORT_PER_SLOTCARD 26

static unsigned char prvCpssDxChPhySmiVirPort2PhyPortAx71[] = {
	/* {virtual port ,phy port} */
	/* XSMI */
	{1 ,0x18},/* PORT1 */
	{2 ,0x19},/* PORT2 */
	/* SMI0 */
	{3 ,0x5 },/* PORT3 */
	{4 ,0x4 },/* PORT4 */
	{5 ,0x7 },/* PORT5 */
	{6 ,0x6 },/* PORT6 */
	{7 ,0x9 },/* PORT7 */
	{8 ,0x8 },/* PORT8 */
	{9 ,0xB },/* PORT9 */
	{10,0xA },/* PORT10 */
	{11,0xD },/* PORT11 */
	{12,0xC },/* PORT12 */
	{13,0xF },/* PORT13 */
	{14,0xE},/* PORT14 */
	/* SMI1 */
	{15,0x7 },/* PORT15 */
	{16,0x6 },/* PORT16 */
	{17,0x5 },/* PORT17 */
	{18,0x4 },/* PORT18 */
	{19,0xB },/* PORT19 */
	{20,0xA },/* PORT20 */
	{21,0x9 },/* PORT21 */
	{22,0x8 },/* PORT22 */
	{23,0xF },/* PORT23 */
	{24,0xE },/* PORT24 */
	{25,0xD },/* PORT25 */
	{26,0xC },/* PORT26 */
};

/*******************************************************************************
*  marvell virtual(logical, or MAC) port map to  PHY port 
*  first column is virtual port #
*  second column is PHY port #
**/

/* this MACRO is for AX7005 product */
#define AX7_MAX_SLOTCARD_NUMBER   4 
#define AX7_MAX_PORT_PER_SLOTCARD 6

static unsigned char prvCpssDxChPhySmiVirPort2PhyPortAx7[][2] = {
	/* {virtual port ,phy port} */
	/* SLOT 1 SMI1 */
	{23,0x0 },/* PORT1 */
	{22,0x1 },/* PORT2 */
	{21,0x2 },/* PORT3 */
	{20,0x3 },/* PORT4 */
	{19,0x4 },/* PORT5 */
	{18,0x5 },/* PORT6 */
	/* SLOT 2 SMI0 */
	{11,0x0 },/* PORT1 */
	{10,0x1 },/* PORT2 */
	{9 ,0x2 },/* PORT3 */
	{8 ,0x3 },/* PORT4 */
	{7 ,0x4 },/* PORT5 */
	{6 ,0x5 },/* PORT6 */
	/* SLOT 3 SMI1 */	
	{17,0x10},/* PORT1 */
	{16,0x11},/* PORT2 */
	{15,0x12},/* PORT3 */
	{14,0x13},/* PORT4 */
	{13,0x14},/* PORT5 */
	{12,0x15},/* PORT6 */
	/* SLOT 4 SMI0 */	
	{5 ,0x10},/* PORT1 */
	{4 ,0x11},/* PORT2 */
	{3 ,0x12},/* PORT3 */
	{2 ,0x13},/* PORT4 */
	{1 ,0x14},/* PORT5 */
	{0 ,0x15},/* PORT6 */
};

/*******************************************************************************
*  marvell virtual(logical, or MAC) port map to  PHY port 
*  first column is virtual port #
*  second column is PHY port #
**/

/* this MACRO is for AU4XXX product */
#define AU4_MAX_SLOTCARD_NUMBER   1 
#define AU4_MAX_PORT_PER_SLOTCARD 24

static unsigned char prvCpssDxChPhySmiVirPort2PhyPortAu4[][2] = {
	/* {virtual port ,phy port} */
	/* SMI0 */
	{0 ,0x0 },/* PORT1  */
	{1 ,0x1 },/* PORT2  */
	{2 ,0x2 },/* PORT3  */
	{3 ,0x3 },/* PORT4  */
	{4 ,0x4 },/* PORT5  */
	{5 ,0x5 },/* PORT6  */
	{6 ,0x6 },/* PORT7  */
	{7 ,0x7 },/* PORT8  */
	{8 ,0x8 },/* PORT9  */
	{9 ,0x9 },/* PORT10 */
	{10,0xA },/* PORT11 */
	{11,0xB },/* PORT12 */
	/* SMI1 */	
	{12,0x0 },/* PORT13 */
	{13,0x1 },/* PORT14 */
	{14,0x2 },/* PORT15 */
	{15,0x3 },/* PORT16 */
	{16,0x4 },/* PORT17 */
	{17,0x5 },/* PORT18 */
	{18,0x6 },/* PORT19 */
	{19,0x7 },/* PORT20 */
	{20,0x8 },/* PORT21 */
	{21,0x9 },/* PORT22 */
	{22,0xA },/* PORT23 */
	{23,0xB },/* PORT24 */
};

/*******************************************************************************
*  marvell virtual(logical, or MAC) port map to  PHY port 
*  first column is virtual port #
*  second column is PHY port #
**/

/* this MACRO is for AX5612i product */
#define AX5I_MAX_SLOTCARD_NUMBER   1 
#define AX5I_MAX_PORT_PER_SLOTCARD 24

static unsigned char prvCpssDxChPhySmiVirPort2PhyPortAx5i[][2] = {
	/* {virtual port ,phy port} */
	/* SMI0 */
	{0 ,0x4 },/* PORT1  */
	{1 ,0x5 },/* PORT2  */
	{2 ,0x6 },/* PORT3  */
	{3 ,0x7 },/* PORT4  */
	{4 ,0x8 },/* PORT5  */
	{5 ,0x9 },/* PORT6  */
	{6 ,0xA },/* PORT7  */
	{7 ,0xB },/* PORT8  */
	{8 ,0x0 },/* PORT9 <null port> */
	{9 ,0x0 },/* PORT10 <null port> */
	{10,0x0 },/* PORT11 <null port> */
	{11,0x0 },/* PORT12 <null port> */

	/* SMI1 */
	{12 ,0x0 },/* PORT1 <null port> */
	{13 ,0x0 },/* PORT2 <null port> */
	{14 ,0x0 },/* PORT3 <null port> */
	{15 ,0x0 },/* PORT4 <null port> */
	{16 ,0x0 },/* PORT5 <null port> */
	{17 ,0x0 },/* PORT6 <null port> */
	{18 ,0x0 },/* PORT7 <null port> */
	{19 ,0x0 },/* PORT8 <null port> */
	{20 ,0x0 },/* PORT9 <null port> */
	{21 ,0x0 },/* PORT10 <null port> */
	{22,0x0 },/* PORT11 <null port> */
	{23,0x0 },/* PORT12 <null port> */
};
unsigned int ax81_12x_xg_port_to_phyid[12] = {
    3,2,1,0,7,6,5,4,11,10,9,8
    };
/*******************************************************************************
* cpssDxChPhySmiFindPhyPortByVirtPort
*
* DESCRIPTION:
*       find out real PHY port number via virtual port number
*       virtual port number is in range of 0...numberOfPorts of device.
*       this function only caculate GE port,e.g. for 98DX275 range is 0..23
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum     - physical device number
*       virtPort      - virtual port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       PHY port # 		- on success
*       0xFFFF		 	- on wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
unsigned short cpssDxChPhySmiFindPhyPortByVirtPort(IN GT_U8 devNum, IN unsigned char virtPort)
{
	unsigned int portNum = 0;
	unsigned short rc = 0xFF;
	int i = 0,max_port = 0;
	unsigned char (*aPtr)[2] = NULL;
	
	portNum = PRV_CPSS_PP_MAC(devNum)->numOfPorts;
	
	if((PRODUCT_ID_AX7K_I == auteware_product_id) || /* PRODUCT_ID_AX7K_I_ALPHA */
		(PRODUCT_ID_AX7K == auteware_product_id)) { /* PRODUCT_ID_AX7K */
		max_port = AX7_MAX_SLOTCARD_NUMBER * AX7_MAX_PORT_PER_SLOTCARD;
		aPtr = prvCpssDxChPhySmiVirPort2PhyPortAx7;
	}
	else if((PRODUCT_ID_AX5K == auteware_product_id)||  /* PRODUCT_ID_AX5K */
			 (PRODUCT_ID_AU4K == auteware_product_id)||  /* PRODUCT_ID_AU4K */
			 (PRODUCT_ID_AU3K == auteware_product_id)){ /* PRODUCT_ID_AU3K */
		max_port = AU4_MAX_SLOTCARD_NUMBER * AU4_MAX_PORT_PER_SLOTCARD;
		aPtr = prvCpssDxChPhySmiVirPort2PhyPortAu4;
	}
	else if(PRODUCT_ID_AX5K_I == auteware_product_id) {/* PRODUCT_ID_AX5K_I */
		max_port = AX5I_MAX_SLOTCARD_NUMBER * AX5I_MAX_PORT_PER_SLOTCARD;
		aPtr = prvCpssDxChPhySmiVirPort2PhyPortAx5i;
	}

	if((virtPort > portNum)||(virtPort >= max_port)) {
		rc = 0xFF;
	}
	else  {
		for(i=0 ;i < max_port ;i++,aPtr++) {
			if(virtPort == (*aPtr)[0]) {
				rc = (*aPtr)[1];
				break;
			}
		}
	}

	return rc;
}

int phy_88X2140_config
(
    unsigned char devNum,
    unsigned char portNum
)
{
    int ret = 0;
    GT_U8 phyId = ax81_12x_xg_port_to_phyid[12];
	
    ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xF002 ,31,0x8102 );
	usleep( 10 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0AA ,30,0x027D );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0A0 ,30,0x000B );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0A1 ,30,0x0204 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0A6 ,30,0x0D09 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0A7 ,30,0x0904 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0A5 ,30,0x0409 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0F1 ,30,0x7E81 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0AE ,30,0x0080 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0A2 ,30,0x9FFF );
	usleep( 2 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0A2 ,30,0x1FFF );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xF002 ,31,0x8102 );
	usleep( 10 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xF06A ,4,0x2000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xF06A ,4,0x3000 );
	usleep( 5 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xC000 ,30,0x8C00 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xC000 ,30,0x9C00 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xC000 ,30,0xDC00 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xC000 ,30,0xFC00 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE000 ,30,0x7F00 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD070 ,30,0x0010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD078 ,30,0x0010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD080 ,30,0x0010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD088 ,30,0x0010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD090 ,30,0x0010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD098 ,30,0x0010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD073 ,30,0x0D89 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD075 ,30,0xF277 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD07B ,30,0x203A );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD07D ,30,0xDFC6 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD040 ,30,0x4020 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0B5 ,30,0x0224 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0B0 ,30,0x0000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0B1 ,30,0x0204 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD08A ,30,0x0140 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD08B ,30,0x0834 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD08C ,30,0x013F );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD08D ,30,0xF7CC );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD082 ,30,0x0140 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD083 ,30,0x0834 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD084 ,30,0x013F );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD085 ,30,0xF7CC );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD092 ,30,0x0140 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD093 ,30,0x1388 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD094 ,30,0x013F );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD095 ,30,0xEC78 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD09A ,30,0x0140 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD09B ,30,0x1388 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD09C ,30,0x013F );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD09D ,30,0xEC78 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD01C ,30,0x1800 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xC030 ,30,0x0209 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD070 ,30,0x8010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD078 ,30,0x8010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD080 ,30,0x8010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD088 ,30,0x8010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD090 ,30,0x8010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD098 ,30,0x8010 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD040 ,30,0x4020 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD042 ,30,0x0A6C );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD04F ,30,0xC200 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD04F ,30,0xC20F );
	usleep( 1 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD049 ,30,0x0001 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD049 ,30,0x0011 );
	usleep( 5 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD049 ,30,0x0002 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD049 ,30,0x0012 );
	usleep( 5 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD049 ,30,0x0004 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD049 ,30,0x0014 );
	usleep( 5 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD049 ,30,0x0008 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD049 ,30,0x0018 );
	usleep( 5 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD049 ,30,0x0000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD04F ,30,0xC20F );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD012 ,30,0x0058 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD011 ,30,0x12C4 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0AE ,30,0x0080 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0BC ,30,0x0080 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xC205 ,30,0xF0F4 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xD0E0 ,30,0x0318 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE016 ,30,0x0B05 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE000 ,30,0x7F00 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE005 ,30,0x28A6 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE005 ,30,0x28A7 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE016 ,30,0x0B05 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE016 ,30,0x0A05 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE011 ,30,0x2380 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE02E ,30,0x00FF );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE01D ,30,0x0000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE01C ,30,0x0000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE011 ,30,0x2380 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE002 ,30,0x677A );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE02F ,30,0x2045 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE02F ,30,0x20A5 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE010 ,30,0x2940 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE040 ,30,0x0003 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE040 ,30,0x000B );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE02C ,30,0x007F );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE001 ,30,0x380A );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE001 ,30,0x383C );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE03C ,30,0x003C );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE001 ,30,0x383C );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE002 ,30,0x697A );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE02B ,30,0x1900 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE01D ,30,0x0000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE01C ,30,0x0000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE02F ,30,0x20A5 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE029 ,30,0x1900 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE02A ,30,0x0000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE028 ,30,0x0000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE025 ,30,0x0012 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE042 ,30,0x2000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE043 ,30,0x0006 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE004 ,30,0x0A0C );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE023 ,30,0x1820 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE00A ,30,0x0FFF );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE00B ,30,0x0FFF );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE00C ,30,0x0FFF );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE008 ,30,0x06DB );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE009 ,30,0x0249 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE006 ,30,0x06DB );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE007 ,30,0x06DB );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE003 ,30,0x0404 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE024 ,30,0x1014 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE005 ,30,0x29F7 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE00F ,30,0x009C );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE00F ,30,0x0098 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE00D ,30,0xBF00 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE00E ,30,0x1500 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE01E ,30,0x0002 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE01F ,30,0x000A );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE01A ,30,0x0000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE01B ,30,0x0000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE016 ,30,0x0A05 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE016 ,30,0x0205 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE016 ,30,0x0005 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE027 ,30,0x8001 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE00D ,30,0x9F00 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE042 ,30,0x0400 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE052 ,30,0x0820 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE053 ,30,0x0820 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE048 ,30,0x0300 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE000 ,30,0x7F01 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE000 ,30,0x7F00 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE048 ,30,0x0200 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE000 ,30,0x7F01 );
	usleep( 5 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE000 ,30,0x7F00 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xE000 ,30,0x7F02 );
	usleep( 750 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xC005 ,30,0x0018 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xC005 ,30,0x0000 );
	usleep( 5 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xF06A ,4,0x2000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xF06A ,4,0x0000 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xC005 ,30,0x0001 );
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1,0xC005 ,30,0x0000);

	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum,portNum, phyId, 1, 0xF012, 31, 0x0CB0);
    /*LED0 configuration*/
    ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum,portNum, phyId, 1, 0xF020, 31, 0x0100);
    /*LED1 configuration*/
    ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum,portNum, phyId, 1, 0xF021, 31, 0x0060);

	    return GT_OK;
}
int phy_88x2140_init(GT_U8 devNum, GT_U8 portNum)
{
    int ret = 0;
	
    ret = gtAppDemoLionPortModeSpeedSet(devNum, portNum, 11, 3);
    if (0 != ret)
    {
        return ret;
    }
    ret = phy_88X2140_config(devNum, portNum);
	
   
	return GT_OK;
}
int phy_88x2140_enable_set(GT_U8 devNum, GT_U8 portNum, GT_U32 enable)
{

	GT_STATUS ret= 0;
	GT_U8 phyId;
    GT_U16 power, mask, state;
	phyId = ax81_12x_xg_port_to_phyid[12];

	ret = cpssDxChPhyPort10GSmiRegisterRead(devNum, portNum, phyId, 1, 0x0000, 1, &power);
    mask = 1<<11;
	enable = (enable)? 0:(1<<11);
	power &= ~mask;
	power |= enable;
	
	ret = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId, 1, 0x0000, 1, power);

	ret = cpssDxChPhyPort10GSmiRegisterRead(devNum, portNum, phyId, 1, 0x0001, 1, &state);
	ret = cpssDxChPhyPort10GSmiRegisterRead(devNum, portNum, phyId, 1, 0x0001, 1, &state);
	state &= 0x4;
	if((state == 0)&&(enable == 0))
	{
	    ret = phy_88x2140_init(devNum, portNum); 
	}

	return GT_OK;
}
#endif


/*******************************************************************************
* cpssDxChPhyPortSmiInit
*
* DESCRIPTION:
*       Initialiaze the SMI control register port, Check all GE ports and
*       activate the errata initialization fix.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiInit
(
    IN GT_U8 devNum
)
{
    GT_U8       i;
    GT_U32      fieldOffset;        /* Phy address reg. field offset.   */
    GT_U32      regAddrOffset;      /* Register address offset.         */
    GT_U32      value = 0;          /* value to read from  the register */
    GT_U32      regAddr;            /* register address                 */
    GT_U8       numOfPorts;         /* number of ports                  */
    GT_U8       smi0portsNum;       /* number of ports which their phys are controled by SMI0  */
    GT_STATUS   rc;                 /* return code                      */
	GT_U16		 phyPort;		     /* PHY port number */
	unsigned int writeValue = 0;	 /* value to write to PHY registers */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    numOfPorts = PRV_CPSS_PP_MAC(devNum)->numOfPorts;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr;

    rc = prvCpssDxChPhySmi0NumOfPortsGet(devNum, &smi0portsNum);
    if (rc != GT_OK)
    {
        return rc;
    }
    printf("%s %s %d---numOfPorts  =%d\n", __FILE__, __FUNCTION__, __LINE__, numOfPorts);/*xcat debug*/
    printf("%s %s %d---smi0portsNum=%d\n", __FILE__, __FUNCTION__, __LINE__, smi0portsNum);/*xcat debug*/
    printf("%s %s %d---smiControl=0x%x\n", __FILE__, __FUNCTION__, __LINE__, PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl);/*xcat debug*/
    printf("%s %s %d---regAddr   =0x%x\n", __FILE__, __FUNCTION__, __LINE__, regAddr);/*xcat debug*/

    /* loop over all GE and FE ports */
    for(i = 0; i < numOfPorts; i++)
    {
        /* skip not existed ports */
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, i))
            continue;

        /* skip XG ports and stack ports of XCAT2 */
        if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[i].portType >= PRV_CPSS_PORT_XG_E)
            continue;

        /* Bind SMI controller to port only if was not already done */
        if( PRV_CPSS_PHY_SMI_NO_CTRL_ADDR_CNS ==
            PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,i) )
        {
            if(i < smi0portsNum)
            {
                /* use SMI#0 controller to access port's PHY*/
                PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,i) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl;
            }
            else if(i < PRV_CPSS_DXCH_SMI_PPU_PORTS_NUM_CNS)
            {
                /* use SMI#1 controller to access port's PHY*/
                PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,i) =
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl +
                    CPSS_DX_SMI_MNG_CNTRL_OFFSET_CNS;
            }
        }

        /* read all PHY Address Registers */
        if(i < PRV_CPSS_DXCH_SMI_PPU_PORTS_NUM_CNS)
        {
            /* calc the reg and bit offset for the port */
            regAddrOffset = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,i)
                                 / 6) * CPSS_DX_PHY_ADDR_REG_OFFSET_CNS;
            fieldOffset   = (i % 6) * 5;
            printf("\n%s %s %d---i=%d\n", __FILE__, __FUNCTION__, __LINE__, i);/*xcat debug*/
            printf("%s %s %d---PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,i)=%d\n", 
                __FILE__, __FUNCTION__, __LINE__, 
                PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,i));/*xcat debug*/
            printf("%s %s %d---regAddrOffset=0x%x\n", __FILE__, __FUNCTION__, __LINE__, regAddrOffset);/*xcat debug*/
            printf("%s %s %d---fieldOffset  =0x%x\n", __FILE__, __FUNCTION__, __LINE__, fieldOffset);/*xcat debug*/

			if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E){

				phyPort = cpssDxChPhySmiFindPhyPortByVirtPort(devNum,i);
				if(0xFF == phyPort)
				{
					osPrintf("cpssDxChPhyPortSmiInit::find phy port number for dev %#x port %d error phyPort %#x.\n",devNum,i,phyPort);
					return GT_FAIL;
				}
				else
				{
					#ifdef AX_FULL_DEBUG
					osPrintf("cpssDxChPhyPortSmiInit::find phy port number %d for %d\r\n",phyPort,i);
					#endif
					writeValue |= (phyPort << fieldOffset);
				}

	            if (((i+1) % 6) == 0)
	            {
	            	/* first write all PHY Addresses to PHY Address Register */
					if(prvCpssDrvHwPpWriteRegister(
							devNum,regAddr + regAddrOffset,writeValue) != GT_OK)
					{
						osPrintf("cpssDxChPhyPortSmiInit::write PHY address %#0x to register %#0x error\n", \
								writeValue,regAddr+regAddrOffset);
						return GT_HW_ERROR;		
					}
					#ifdef AX_FULL_DEBUG
					osPrintf("cpssDxChPhyPortSmiInit::write PHY address %#0x to register %#0x ok\n", \
							writeValue,regAddr+regAddrOffset);
					#endif
	                /* read all PHY Address Registers */
	                if (prvCpssDrvHwPpReadRegister(
	                        devNum, regAddr + regAddrOffset, &value) != GT_OK)
	                {
	                    return GT_HW_ERROR;
	                }

					/* reset PHY Address value to write */
					writeValue = 0;
					value = 0;
	        	}
				
				/* init PHY Addresses Array */
				PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,i) = phyPort;
			}
			else {
				if ((i % 6) == 0)
	            {	            
	                /* read all PHY Address Registers */
	                if (prvCpssDrvHwPpReadRegister(
	                        devNum, regAddr + regAddrOffset, &value) != GT_OK)
	                {
	                    return GT_HW_ERROR;
	                }

	        	}
            /* init PHY Addresses Array */
            PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,i) =
                (GT_U16)((value >> (fieldOffset)) & 0x1f);
			}
		
    	}

        /* implement workarounds of FE/GE PHYs */
        if (prvCpssGenPhyErrataInitFix(devNum,
                                 i,
                                 &cpssDxChPhyPortSmiRegisterRead,
                                 &prvCpssDxChPhyPortSmiRegisterWrite ) != GT_OK)
        {
            return GT_FAIL;
        }
    }
    /* Initialize PHY virtual functions */
    vctObjInit(devNum);

    /* Initialize SMI service virtual functions */
    smiObjInit(devNum);

    /* Enable PHY Auto-Negotiation */
	#ifndef __AX_PLATFORM__
	for(i = 0; i < numOfPorts; i++)
	{	
		PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
						devNum, i, GT_TRUE);
	}
	#endif

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyPortSmiRegisterRead
*
* DESCRIPTION:
*       Read specified SMI Register on a specified port on specified device.
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
*       phyReg    - SMI register (value 0..31)
*
* OUTPUTS:
*       dataPtr   - (pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - Flex port SMI or PHY address were not set
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiRegisterRead
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyReg,
    OUT GT_U16    *dataPtr
)
{
    GT_U32 value = 0;           /* value to write into the register */
    GT_U32 regAddr;             /* register address                 */
    GT_U16 portAddr;            /* port address                     */
    GT_U32 copm;
    volatile GT_U32 retryCnt;   /* retry counter for read success   */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    if(phyReg >= BIT_5)
    {
        /* only 5 bits for each of those parameters */
        return GT_OUT_OF_RANGE;
    }

    /* XG ports are not supported */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType >= PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* if port SMI or PHY address were not set access to phy registers through SMI */
    /* is prohibited. */
    if (PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portNum) ==  PRV_CPSS_PHY_SMI_NO_PORT_ADDR_CNS ||
        PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portNum) == PRV_CPSS_PHY_SMI_NO_CTRL_ADDR_CNS)
    {
        return GT_NOT_INITIALIZED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    portAddr = PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portNum);
    value = ((portAddr & 0x1F) << 16) | ((phyReg & 0x1F) << 21) | (1 << 26);

    regAddr = PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portNum);

    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId, regAddr, value) != GT_OK)
        return GT_HW_ERROR;

    /* check if read operation has finished */
    retryCnt = CPSS_DX_SMI_RETRY_COUNTER_CNS;
    do
    {
        if (prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &value) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        copm = (value >> 27) & 0x1;
        retryCnt--;
    }while (!copm && retryCnt != 0);

    if (0 == retryCnt)
        return GT_HW_ERROR;

    *dataPtr = (GT_U16) (value & 0xFFFF);
    return GT_OK;
}
/*******************************************************************************
* cpssDxChPhyPortSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyReg      - The new phy address, (value 0...31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - Flex port SMI or PHY address were not set
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyReg,
    IN  GT_U16    data
)
{
    GT_STATUS rc;                /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(phyReg >= BIT_5)
    {
        /* only 5 bits for each of those parameters */
        return GT_OUT_OF_RANGE;
    }

    /* XG ports are not supported */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType >= PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* if port SMIor PHY address were not set access to phy registers through SMI */
    /* is prohibited. */
    if (PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portNum) ==  PRV_CPSS_PHY_SMI_NO_PORT_ADDR_CNS ||
        PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portNum) == PRV_CPSS_PHY_SMI_NO_CTRL_ADDR_CNS)
    {
        return GT_NOT_INITIALIZED;
    }

    /* check for PHY WA erratum */
    rc = prvCpssPhyRegWriteErrataFix(devNum, portNum, phyReg, data,
                                     &prvCpssDxChPhyPortSmiRegisterWrite);
    if(rc != GT_OK)
        return rc;

    rc = prvCpssDxChPhyPortSmiRegisterWrite(devNum, portNum, phyReg, data);

    return rc;

}
/*******************************************************************************
* prvCpssDxChPhyPortSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyReg      - The new phy address, (value 0...31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED - for XG ports
*       GT_OUT_OF_RANGE  - phyAddr bigger then 31
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhyPortSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyReg,
    IN  GT_U16    data
)
{
    GT_U32 value = 0;           /* value to write into the register */
    GT_U32 regAddr;             /* register address                 */
    GT_U16 portAddr;            /* port address                     */
    GT_U32 busy;
    volatile GT_U32 retryCnt;   /* retry counter for busy SMI reg   */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(phyReg >= BIT_5)
    {
        /* only 5 bits for each of those parameters */
        return GT_OUT_OF_RANGE;
    }

    /* XG ports are not supported */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType >= PRV_CPSS_PORT_XG_E)
    {
        return GT_NOT_SUPPORTED;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    portAddr = PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portNum);
    value = data | ((portAddr & 0x1F) << 16) | ((phyReg & 0x1F) << 21);

    regAddr = PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portNum);

    #ifdef __AX_PLATFORM_
	osPrintf("SmiPhyPortWrite dev %d port %d reg %#0x portAddr %#0x ", \
				devNum,portNum,phyReg,portAddr);
    #endif

    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId, regAddr, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }   
    #ifdef __AX_PLATFORM_
	osPrintf("write reg %#0x value %#0x ok\r\n",regAddr,value);
    #endif

    /* check if smi register is not busy */
    retryCnt = CPSS_DX_SMI_RETRY_COUNTER_CNS;
    do
    {
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 28, 1, &busy) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        retryCnt--;
    }while (busy && retryCnt != 0);

    
    #ifdef __AX_PLATFORM_
	osPrintf("SmiPhyPortWrite complish %s retry %d\r\n", \
				(0==retryCnt)?"fail":"ok",retryCnt);
    #endif

    if (0 == retryCnt)
        return GT_HW_ERROR;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyPort10GSmiRegisterRead
*
* DESCRIPTION:
*       Read specified SMI Register and PHY device of specified port
*       on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       phyId   - ID of external PHY (value of 0..31).
*       useExternalPhy - boolean variable, defines if to use external PHY
*                       NOTE: for DxCh3 and above only GT_TRUE supported !
*       phyReg  - SMI register, the register of PHY to read from
*       phyDev  - the PHY device to read from (value of 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If useExternalPhy flag is GT_FALSE, the phyId is being ignored
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPort10GSmiRegisterRead
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyId,
    IN  GT_BOOL   useExternalPhy,
    IN  GT_U16    phyReg,
    IN  GT_U8     phyDev,
    OUT GT_U16    *dataPtr
)
{
    GT_U32 value = 0;           /* value to write into the register */
    GT_U32 regAddr;             /* register address                 */
    GT_U32 copm;                /* variable to compare              */
    GT_U32 busy;                /* XSMI Busy bit                    */
    volatile GT_U32 retryCnt;   /* retry counter for read success   */
    GT_U32 phyConfigRegVal = 0;
    GT_U32 phyConfigRegAddr = 0;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    if(GT_FALSE == useExternalPhy)
    {
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
             CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);
    }

    if(phyDev >= BIT_5 || phyId >= BIT_5)
    {
        return GT_OUT_OF_RANGE;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* check if XSMI is not busy */
    retryCnt = CPSS_DX_SMI_RETRY_COUNTER_CNS;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeControl;
    do
    {
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 30, 1, &busy) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        retryCnt--;
    }while (busy && retryCnt != 0);

    if (0 == retryCnt)
        return GT_HW_ERROR;

    /* Switching between internal and external PHY*/
    if(GT_FALSE == useExternalPhy)
    {
        /* write to XAUI configuration register1. */
        /* this register must be initialized before writing to the PHY */
        phyConfigRegAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GePhyConfig1[portNum];

        /* store the register content */
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, phyConfigRegAddr,
                                      0, 6, &phyConfigRegVal) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        value = phyDev & 0x1F ;
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, phyConfigRegAddr,
                                      0, 6, value) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }


    /* write the address to be used in the read operation */
    value = phyReg;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeAddr;

    if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, 0, 16, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* write address and read opCode to Control-Register */
    /* set port address */
    if(PRV_CPSS_PP_MAC(devNum)->numOfPorts == 1)
    {
        /* One Port devices */
        value |= (((useExternalPhy) ?
                   phyId : PRV_CPSS_PHY_XENPAK_DEV_ADDRESS_CNS) << 16);
    }
    else
    {
        /* Use external PHY if useExternalPhy == TRUE */
        value |= (((useExternalPhy) ?
                    phyId :
                    PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum))
                        << 16);
    }

    /* set device address */
    value |= (phyDev << 21);

    /* set read opcode */
    value |= (((GT_U32)7) << 26);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeControl;
    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId, regAddr, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* check if read operation has finished */
    retryCnt = CPSS_DX_SMI_RETRY_COUNTER_CNS;
    do
    {
        if (prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId, regAddr, &value) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        copm = (value >> 29) & 0x1;
        retryCnt--;
    }while (!copm && retryCnt != 0);

    if (0 == retryCnt)
        return GT_HW_ERROR;

    *dataPtr = (GT_U16) (value & 0xFFFF);

    /* write the original value of XAUI configuration register1. */
    if( GT_FALSE == useExternalPhy)
    {
       if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, phyConfigRegAddr,
                                     0, 6, phyConfigRegVal) != GT_OK)
        {
            return GT_HW_ERROR;
        }

    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyPort10GSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified SMI Register and PHY device of
*       specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyId       - ID of external PHY (value of 0...31).
*       useExternalPhy - Boolean variable, defines if to use external PHY
*                       NOTE: for DxCh3 and above only GT_TRUE supported !
*       phyReg      - SMI register, the register of PHY to read from
*       phyDev      - the PHY device to read from (value of 0..31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If useExternalPhy flag is GT_FALSE, the phyId is being ignored
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPort10GSmiRegisterWrite
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  GT_U8     phyId,
    IN  GT_BOOL   useExternalPhy,
    IN  GT_U16    phyReg,
    IN  GT_U8     phyDev,
    IN  GT_U16    data
)
{
    GT_U32 value = 0;           /* value to write into the register */
    GT_U32 regAddr;             /* register address                 */
    GT_U32 busy;
    volatile GT_U32 retryCnt;   /* retry counter for busy SMI reg   */
    GT_U32 phyConfigRegVal = 0;
    GT_U32 phyConfigRegAddr = 0;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if(GT_FALSE == useExternalPhy)
    {
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
             CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E);
    }

    if(phyDev >= BIT_5 || phyId >= BIT_5)
    {
        return GT_OUT_OF_RANGE;
    }

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeControl;

    /* check if smi register is not busy */
    retryCnt = CPSS_DX_SMI_RETRY_COUNTER_CNS;
    do
    {
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 30, 1, &busy) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        retryCnt--;
    }while (busy && retryCnt != 0);

    if (0 == retryCnt)
        return GT_HW_ERROR;

    if(GT_FALSE == useExternalPhy)
    {
        /* write to XAUI configuration register1. */
        /* this register must be initialized before writing to the PHY */
        phyConfigRegAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GePhyConfig1[portNum];

        /* store the register content */
        if (prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, phyConfigRegAddr,
                                      0, 6, &phyConfigRegVal) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        value = phyDev & 0x1F ;
        if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, phyConfigRegAddr, 0, 6, value) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }

    /* write address */
    value = phyReg;
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeAddr;

    if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, 0, 16, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* set data */
    value = data;

    /* set port address */
    if(PRV_CPSS_PP_MAC(devNum)->numOfPorts == 1)
    {
        /* One Port devices */
        value |= (((useExternalPhy) ?
                   phyId : PRV_CPSS_PHY_XENPAK_PORT_ADDRESS_CNS) << 16);
    }
    else
        value |= (((useExternalPhy) ?
                    phyId :
                    PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum))
                         << 16);


    /* set device address */
    value |= (phyDev << 21);

    /* set write opcode */
    value |= (5 << 26);
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smi10GeControl;

    /* Write the value */
    if (prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId, regAddr, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* write the original value of XAUI configuration register1. */
    if( GT_FALSE == useExternalPhy)
    {
       if (prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, phyConfigRegAddr,
                                     0, 6, phyConfigRegVal) != GT_OK)
        {
            return GT_HW_ERROR;
        }

    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyAutonegSmiGet
*
* DESCRIPTION:
*       This function gets the auto negotiation status between the PP and PHY.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number.
*       portNum      - port number: applicable ports from 0 till 23
*
* OUTPUTS:
*       enabledPtr   - (Pointer to) the auto negotiation process state between
*                      PP and Phy:
*                      0 = Auto-Negotiation process is preformed.
*                      1 = Auto-Negotiation process is not preforme
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Although the device ignores the information read from the PHY
*       registers, it keeps polling those registers.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyAutonegSmiGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   portNum,
    OUT GT_BOOL *enabledPtr
)
{
    GT_U32      regAddr;            /* register address     */
    GT_U32      autonegEn;          /* autoneg enabled flag */
    GT_U8       localPort;/* local port - support multi-port-groups device */
    GT_U8       smi0portsNum;
                        /* number of ports which their phys are controled by SMI0  */
    GT_STATUS   rc;                 /* return code          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enabledPtr);

    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType >= PRV_CPSS_PORT_XG_E)
    {
        return GT_BAD_PARAM;
    }

    rc = prvCpssDxChPhySmi0NumOfPortsGet(devNum, &smi0portsNum);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* for multi-port-group device : since this config is not really per port ,
        we will read the info from first active port group , and will set it to all port groups */
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if (localPort < smi0portsNum)
    {
        regAddr = 0x4004034;
    }
    else if (localPort < PRV_CPSS_DXCH_SMI_PPU_PORTS_NUM_CNS)
    {
        regAddr = 0x5004034;
    }
    else
    {
        return GT_BAD_PARAM;
    }

    /* Read the SMI AutoNeg enabled bit */
    if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 7, 1, &autonegEn) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    *enabledPtr = (autonegEn == 0) ? GT_TRUE : GT_FALSE;
    return GT_OK;
}


/*******************************************************************************
* cpssDxChPhyAutonegSmiSet
*
* DESCRIPTION:
*       This function sets the auto negotiation process, between
*       the PP and PHY, to enable/disable.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number: applicable ports from 0 till 23
*       enable      - 0 = Auto-Negotiation process is preformed.
*                     1 = Auto-Negotiation process is not preforme
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Although the device ignores the information read from the PHY
*       registers, it keeps polling those registers.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyAutonegSmiSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;            /* register address             */
    GT_U32      autonegEn;          /* autoneg enabled flag         */
    GT_U8       localPort;/* local port - support multi-port-groups device */
    GT_U8       smi0portsNum;
                        /* number of ports which their phys are controled by SMI0  */
    GT_STATUS   rc;                 /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType >= PRV_CPSS_PORT_XG_E)
    {
        return GT_BAD_PARAM;
    }

    rc = prvCpssDxChPhySmi0NumOfPortsGet(devNum, &smi0portsNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* for multi-port-group device : since this config is not really per port ,
        we will read the info from first active port group , and will set it to all port groups */
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if (localPort < smi0portsNum)
    {
        regAddr = 0x4004034;
    }
    else if (localPort < PRV_CPSS_DXCH_SMI_PPU_PORTS_NUM_CNS)
    {
        regAddr = 0x5004034;
    }
    else
    {
        return GT_BAD_PARAM;
    }
    autonegEn = (enable == GT_TRUE) ? 0 : 1;

    /* Write the SMI AutoNeg enabled bit */
    if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 7, 1, autonegEn) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhySmiAutoMediaSelectSet
*
* DESCRIPTION:
*       This function gets a bit per port bitmap (of 6 ports)
*       indicating whether this port is connected to a dual-media PHY,
*       such as Marvell 88E1112, that is able to perform Auto-media select
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number: applicable ports from 0 till 23
*       autoMediaSelect - Sets 6 bits.
*                         0 = Port is not connected to dual-media PHY
*                         1 = Port is connected to dual-media PHY
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhySmiAutoMediaSelectSet
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   autoMediaSelect

)
{
    GT_U32      regAddr;            /* register address     */
    GT_U32      value = 0;          /* autoneg enabled flag */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType >= PRV_CPSS_PORT_XG_E)
    {
        return GT_BAD_PARAM;
    }

    if(autoMediaSelect >= BIT_6)
    {
        return GT_OUT_OF_RANGE;
    }

    /* for multi-port-group device : since this config is not really per port ,
        we will read the info from first active port group , and will set it to all port groups */
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if (localPort <= 5)
    {
        regAddr = 0x4004034;
    }
    else if (localPort <= 11)
    {
        regAddr = 0x4804034;
    }
    else if (localPort <= 17)
    {
        regAddr = 0x5004034;
    }
    else if (localPort <= 23)
    {
        regAddr = 0x5804034;
    }
    else
    {
        return GT_BAD_PARAM;
    }

    value = autoMediaSelect & 0x3F ;
    if (prvCpssDrvHwPpSetRegField(devNum, regAddr,0, 6, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    
    #ifndef __AX_PLATFORM__
	if(prvCpssDrvHwPpGetRegField(devNum,regAddr,0,6,&value) != GT_OK)
	{
		return GT_HW_ERROR;
	}
	value |= (autoMediaSelect & 0x3F);
    if (prvCpssDrvHwPpSetRegField(devNum, regAddr,0, 6, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }	
    #else
    value = autoMediaSelect & 0x3F ;
    if (prvCpssDrvHwPpSetRegField(devNum, regAddr,0, 6, value) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    #endif

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhySmiAutoMediaSelectGet
*
* DESCRIPTION:
*       This function gets a bit per port bitmap (of 6 ports)
*       indicating whether this port is connected to a dual-media PHY,
*       such as Marvell 88E1112, that is able to perform Auto-media select
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number:applicable ports from 0 till 23
*
* OUTPUTS:
*       autoMediaSelectPtr - (Pointer to) port state bitMap of 6 bits:
*                            0 = Port is not connected to dual-media PHY
*                            1 = Port is connected to dual-media PHY
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhySmiAutoMediaSelectGet
(

    IN GT_U8    devNum,
    IN GT_U8    portNum,
    OUT GT_U32   *autoMediaSelectPtr
)
{
    GT_U32      regAddr;            /* register address     */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(autoMediaSelectPtr);

    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType >= PRV_CPSS_PORT_XG_E)
    {
        return GT_BAD_PARAM;
    }

    /* for multi-port-group device : since this config is not really per port ,
        we will read the info from first active port group , and will set it to all port groups */
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    if (localPort <= 5)
    {
        regAddr = 0x4004034;
    }
    else if (localPort <= 11)
    {
        regAddr = 0x4804034;
    }
    else if (localPort <= 17)
    {
        regAddr = 0x5004034;
    }
    else if (localPort <= 23)
    {
        regAddr = 0x5804034;
    }
    else
    {
        return GT_BAD_PARAM;
    }

    if (prvCpssDrvHwPpGetRegField(devNum, regAddr,0, 6, autoMediaSelectPtr) != GT_OK)
    {
        return GT_HW_ERROR;
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChPhySmiCtrlRegWrite
*
* DESCRIPTION:
*       Write to SMI control register
*
* INPUTS:
*       devNum      - The PP to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       smiInterface - SMI interface.
*       value        - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_BAD_PARAM - wrong smiIf
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhySmiCtrlRegWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN  CPSS_PHY_SMI_INTERFACE_ENT  smiInterface,
    IN GT_U32   value
)
{
    GT_U32 regAddr;

    switch(smiInterface)
    {
        case CPSS_PHY_SMI_INTERFACE_0_E:
            /* use SMI#0 controller to access port's PHY*/
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl;
            break;
        case CPSS_PHY_SMI_INTERFACE_1_E:
            /* use SMI#1 controller to access port's PHY*/
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl +
                        CPSS_DX_SMI_MNG_CNTRL_OFFSET_CNS;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr, value);
}

/*******************************************************************************
* prvCpssDxChPhySmiCtrlRegRead
*
* DESCRIPTION:
*       Read to SMI control register
*
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       smiInterface    - SMI interface.
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_BAD_PARAM - wrong smiIf
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhySmiCtrlRegRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN  CPSS_PHY_SMI_INTERFACE_ENT  smiInterface,
    OUT GT_U32  *dataPtr
)
{
    GT_U32 regAddr;

    switch(smiInterface)
    {
        case CPSS_PHY_SMI_INTERFACE_0_E:
            /* use SMI#0 controller to access port's PHY*/
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl;
            break;
        case CPSS_PHY_SMI_INTERFACE_1_E:
            /* use SMI#1 controller to access port's PHY*/
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl +
                        CPSS_DX_SMI_MNG_CNTRL_OFFSET_CNS;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, dataPtr);
}

/******************************************************************************
* smiObjInit
*
* DESCRIPTION:
*       Initialise SMI service function pointers : SMI Ctrl Reg. Read/Write
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - the device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       none.
*
* COMMENTS:
*******************************************************************************/
static GT_VOID smiObjInit
(
    IN  GT_U8     devNum
)
{
    /* assign relevant virtual functions */
    PRV_CPSS_PP_MAC(devNum)->phyInfo.genSmiBindFunc.
                          cpssPhySmiCtrlRegRead = prvCpssDxChPhySmiCtrlRegRead;

    PRV_CPSS_PP_MAC(devNum)->phyInfo.genSmiBindFunc.
                          cpssPhySmiCtrlRegWrite = prvCpssDxChPhySmiCtrlRegWrite;

    return;
}

/******************************************************************************
* vctObjInit
* DESCRIPTION:
*       Initialise all function pointers : Read/Write SMI, AutoNegotiation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - the device number
* OUTPUTS:
*       None.
*
* RETURNS:
*       none.
* COMMENTS:
*******************************************************************************/

static GT_VOID vctObjInit
(
    IN  GT_U8     devNum
)
{
    /* assign relevant virtual functions */
    PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.
                           cpssPhyAutoNegotiationGet = cpssDxChPhyAutonegSmiGet;

    PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.
                           cpssPhyAutoNegotiationSet = cpssDxChPhyAutonegSmiSet;

    PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.
                          cpssPhyRegisterRead = cpssDxChPhyPortSmiRegisterRead;

    PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.
                          cpssPhyRegisterWrite =cpssDxChPhyPortSmiRegisterWrite;


    return ;

}


/*******************************************************************************
* prvCpssDxChPhySmi0NumOfPortsGet
*
* DESCRIPTION:
*       Get number of Auto Poll ports for PHY SMI 0.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*
* OUTPUTS:
*       smi0PortsNumPtr - number of auto poll ports for SMI 0.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on fail
*       GT_BAD_PARAM     - on wrong devNum or portNum.
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChPhySmi0NumOfPortsGet
(
  IN  GT_U8     devNum,
  OUT GT_U8     *smi0PortsNumPtr
)
{
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT       smi0ports;
                        /* PHY SMI 0 AUTO_POLL mode */
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT       smi1ports;
                        /* PHY SMI 1 AUTO_POLL mode */
    GT_STATUS       rc;     /* return code  */


    /* 2 SMI Master controllers exists - SMI0 and SMI1.                                       */
    /* For xCat FE devices due to the use of 3 octal phys, SMI0 is responsible for ports 0-15 */
    /* and SMI1 for ports 16-23. For other devices the is equal SMI0 and SMI1 for ports 0-11  */
    /* and 12-23 respectively.                                                                */

    /* Check device type */
    if ((PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum) &&
            (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E))
            || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        /* xCat A1 devices */

        /* Get Auto Poll configuration */
        rc = cpssDxChPhyAutoPollNumOfPortsGet(devNum, &smi0ports, &smi1ports);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* convert CPSS value to physical number of ports */
        switch (smi0ports)
        {
            case CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E:
                *smi0PortsNumPtr = 8;
                break;
            case CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_12_E:
                *smi0PortsNumPtr = 12;
                break;
            case CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_16_E:
                *smi0PortsNumPtr = 16;
                break;
            default:
                return GT_FAIL;
        }
    }
    else
    {
        switch(PRV_CPSS_PP_MAC(devNum)->devType)
        {
            case PRV_CPSS_DXCH_XCAT_FE_DEVICES_CASES_MAC:
                *smi0PortsNumPtr = 16;
                break;
            default:
                *smi0PortsNumPtr = 12;
                break;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyXsmiMdcDivisionFactorSet
*
* DESCRIPTION:
*       This function sets MDC frequency for Master XSMI Interface
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       divisionFactor  - Division factor of the core clock to get the MDC
*                                           (Serial Management Interface Clock).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or divisionFactor
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyXsmiMdcDivisionFactorSet
(
    IN GT_U8 devNum,
    IN CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactor
)
{
    GT_U32      regAddr;  /* register address        */
    GT_U32      divSel;   /* division selector value */
    GT_U32      fastMdcEn = 0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        switch(divisionFactor)
        {
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E:
                fastMdcEn = 1;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E:
                fastMdcEn = 0;
                break;
            default:
                return GT_BAD_PARAM;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 12, 1, fastMdcEn) != GT_OK)
        {
            return GT_HW_ERROR;
        }

    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        regAddr = 0x6E4000C; /* XSMI Configuration Register */
        switch(divisionFactor)
        {
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E:
                divSel = 0;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E:
                divSel = 1;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E:
                divSel = 2;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E:
                divSel = 3;
                break;
            default:
                return GT_BAD_PARAM;
        }
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 2, divSel) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }
    else
    {
        return GT_NOT_SUPPORTED;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhyXsmiMdcDivisionFactorGet
*
* DESCRIPTION:
*       This function gets MDC frequency for Master XSMI Interface
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       divisionFactorPtr  - (Pointer to) Division factor of the core clock
*                           to get the MDC (Serial Management Interface Clock).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or divisionFactor
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyXsmiMdcDivisionFactorGet
(
    IN  GT_U8 devNum,
    OUT CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT *divisionFactorPtr
)
{
    GT_U32      regAddr;  /* register address        */
    GT_U32      divSel;   /* division selector value */
    GT_U32      fastMdcEn;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(divisionFactorPtr);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.hgXsmiiCountersCtr;
        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 12, 1, &fastMdcEn) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        if(fastMdcEn == 0)
            *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E;
        else
            *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E;
    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        regAddr = 0x6E4000C; /* XSMI Configuration Register */
        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 2, &divSel) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        switch(divSel)
        {
            case 0:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E;
                break;
            case 1:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E;
                break;
            case 2:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E;
                break;
            case 3:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_8_E;
                break;
            default:
                return GT_HW_ERROR;
        }
    }
    else
    {
        return GT_NOT_SUPPORTED;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhySmiMdcDivisionFactorSet
*
* DESCRIPTION:
*       This function sets Fast MDC Division Selector
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number:applicable ports from 0 till 23
*       divisionFactor  - Division factor of the core clock to get the MDC
*                                           (Serial Management Interface Clock).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or divisionFactor
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhySmiMdcDivisionFactorSet
(
    IN GT_U8 devNum,
    IN GT_U8 portNum,
    IN CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT divisionFactor
)
{
    GT_U32      regAddr;  /* register address        */
    GT_U32      divSel;   /* division selector value */
    GT_U32      fastMdcEn;/* fast MDC enable */
    GT_U8       smi0portsNum;
                        /* number of ports which their phys are controled by SMI0  */
    GT_STATUS rc;       /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);


    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        rc = prvCpssDxChPhySmi0NumOfPortsGet(devNum, &smi0portsNum);
        if (rc != GT_OK)
        {
            return rc;
        }

        fastMdcEn = 1;
        switch(divisionFactor)
        {
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E:
                divSel = 0;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E:
                divSel = 1;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E:
                divSel = 2;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E:
                fastMdcEn = 0;
                divSel = 0; /* to avoid VC warning */
                break;
            default:
                return GT_BAD_PARAM;
        }

        if(portNum < smi0portsNum)
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr;
        else
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr + 0x1000000;

        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 30, 1, fastMdcEn) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        if(fastMdcEn != 0)
        {
            if(portNum < smi0portsNum)
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
            else
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms1MiscConfig;
            if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 15, 2, divSel) != GT_OK)
            {
                return GT_HW_ERROR;
            }
        }

    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        switch(divisionFactor)
        {
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E:
                divSel = 0;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E:
                divSel = 1;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E:
                divSel = 2;
                break;
            case CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E:
                divSel = 3;
                break;
            default:
                return GT_BAD_PARAM;
        }
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
        if (prvCpssDrvHwPpSetRegField(devNum, regAddr, 15, 2, divSel) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }
    else
    {
        return GT_NOT_SUPPORTED;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPhySmiMdcDivisionFactorGet
*
* DESCRIPTION:
*       This function gets Fast MDC Division Selector
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number:applicable ports from 0 till 23
*
* OUTPUTS:
*       divisionFactorPtr  - (Pointer to) Division factor of the core clock
*                           to get the MDC (Serial Management Interface Clock).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or divisionFactor
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhySmiMdcDivisionFactorGet
(
    IN  GT_U8 devNum,
    IN  GT_U8 portNum,
    OUT CPSS_PHY_SMI_MDC_DIVISION_FACTOR_ENT *divisionFactorPtr
)
{
    GT_U32      regAddr;  /* register address        */
    GT_U32      divSel;   /* division selector value */
    GT_U32      fastMdcEn;/* fast MDC enable */
    GT_U8       smi0portsNum;
                        /* number of ports which their phys are controled by SMI0  */
    GT_STATUS rc;       /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(divisionFactorPtr);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
        || (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E))
    {
        PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

        rc = prvCpssDxChPhySmi0NumOfPortsGet(devNum, &smi0portsNum);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(portNum < smi0portsNum)
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr;
        else
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr + 0x1000000;

        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 30, 1, &fastMdcEn) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        if(fastMdcEn == 0)
        {
            *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E;
            return GT_OK;
        }

        if(portNum < smi0portsNum)
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
        else
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms1MiscConfig;
        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 15, 2, &divSel) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        switch(divSel)
        {
            case 0:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_16_E;
                break;
            case 1:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E;
                break;
            case 2:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E;
                break;
            default:
                return GT_BAD_PARAM;
        }

    }
    else if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
        if (prvCpssDrvHwPpGetRegField(devNum, regAddr, 15, 2, &divSel) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        switch(divSel)
        {
            case 0:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_32_E;
                break;
            case 1:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_64_E;
                break;
            case 2:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_128_E;
                break;
            case 3:
                *divisionFactorPtr = CPSS_PHY_SMI_MDC_DIVISION_FACTOR_256_E;
                break;
            default:
                return GT_BAD_PARAM;
        }
    }
    else
    {
        return GT_NOT_SUPPORTED;
    }

    return GT_OK;
}


#ifndef __AX_PLATFORM__
/*******************************************************************************
* cpssDxChPhyPortSmiPreferredMediaSet
*
* DESCRIPTION:
*      Set media auto-selection preferred media(or say media priority)
*	 irrelevant when port PHY type is not combo(or say Media Auto-Selection Interface)
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       autoMediaType   - preferred auto-selection interface type (copper or fiber)
*			0 - none
*			1 - fiber media interface
*			2 - copper media interface
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*       GT_OK (0)  					- on success
*       GT_BAD_PARAM (4)    	   	- on wrong devNum or portNum.
*       GT_NOT_SUPPORTED (0x10)  	- for non-combo ports
*       GT_HW_ERROR (0x10017)     	- on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiPreferredMediaSet
(
	IN GT_U8	devNum,
	IN GT_U8	portNum,
	IN GT_U8    autoMediaType
)
{
	GT_STATUS  result = GT_OK;
	GT_U16 phyType; 		  /* PHY type */
	GT_BOOL isMarvellPhy;	  /* indicator whether the PHY is a Marvell PHY */
	GT_U16 data = 0,old_page = 0;
	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
	PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

	/* No Media Auto-Selection on XG port */
	if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray->portType == PRV_CPSS_PORT_XG_E) {
		return GT_NOT_SUPPORTED;
	}

	
	/* get PHY type. Don't check return status. It may be not GT_OK if PHY
	   not connected to a port */
	cpssGenPhyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);

	result = GT_NOT_SUPPORTED;
	/* currently support for Marvell PHYs */
	if (isMarvellPhy) {
		/* for PHY 1112 support combo ports */
		if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) {
			/*
			 * Disable PHY Auto-Negotiation
			 */
			PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
							devNum, portNum, GT_FALSE);

			/*
			 * MAC Specific Control Register 1
			 *	set Auto-Selection Preferred Media
			 *	page 2 register 16 bit[11:10] value as follows:
			 *		00 - no preference for media
			 *		01 - prefer fiber medium
			 *		10 - prefer copper medium
			 *		11 - reserved
			 */
			/* step 1: select page 2 */
			/* backup old page value */
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,&old_page);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 backup page read PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,result);
				goto restore_page;
			}
			
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,2);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,2,result);
				goto restore_page;
			}
			/* step 2: read PHY register 16 */
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,16,&data);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 read PHY:dev %d port %d reg %d error %d\r\n", \
							devNum,portNum,16,result);
				goto restore_page;
			}
			data &= 0xF07F;
			if(1 == autoMediaType) { /* fiber media */
				data |= (0x1 << 10); 
				/*data |= (0x7 << 7);  mode Fiber(1000Base-X) only */
				data |= (0x3 << 7); /* [7-9] 011 - auto Copper/1000Base-X */
			}
			else if(2 == autoMediaType) {/* copper media */
				data |= (0x2 << 10);
				/*data |= (0x5 << 7);  mode Copper only */
				data |= (0x3 << 7);  /*[7-9] 011 - auto Copper/1000Base-X */
			}
			else {/* prefer media none */
				data |= (0x3 << 7); /* [7-9] 011 - auto Copper/1000Base-X */
			}
			
			osPrintfDbg("phy 1112 port(%d,%d) media %d write reg data %#x\n",	\
							devNum,portNum,autoMediaType,data);
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,16,data);
			if(GT_OK != result) {
				osPrintf("phy 1112 write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
							devNum,portNum,16,data,result);
				goto restore_page;
			}
			

			/* step 3 reset PHY : set 0_0.15 or 0_1.15 or 0_2.15 [1] */
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0,result);
				goto restore_page;
			}

			data = 0;
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,PRV_CPSS_PHY_CNTRL,&data);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 read PHY:dev %d port %d reg %d error %d\r\n", \
							devNum,portNum,PRV_CPSS_PHY_CNTRL,result);
				goto restore_page;
			}
			data |= (1<<15);
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_PHY_CNTRL,data);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_PHY_CNTRL,data,result);
				goto restore_page;
			}
			osPrintfDbg("phy 1112 port(%d,%d) reset phy with data %#x after preferred media %d\n",	\
							devNum,portNum,data,autoMediaType);
			
			goto restore_page;
		}
	}

	if(GT_NOT_SUPPORTED == result) {
		/*return GT_OK;*/
		return GT_NOT_SUPPORTED;
	}

restore_page:
	/* restore old page selection */
	result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page);
	if(GT_OK != result) {
		osPrintf("phy 1112 restore page write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,result);
	}			

	/*
	 * Enable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);
	
	return result;
}


/*******************************************************************************
* cpssDxChPhyPortSmiPreferredMediaGet
*
* DESCRIPTION:
*      Get media auto-selection preferred media(or say media priority)
*	 irrelevant when port PHY type is not combo(or say Media Auto-Selection Interface)
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       autoMediaType   - preferred auto-selection interface type (copper or fiber)
*			0 - none
*			1 - fiber media interface
*			2 - copper media interface
*
* RETURNS:
*       GT_OK (0)  					- on success
*       GT_BAD_PARAM (4)    	   	- on wrong devNum or portNum.
*	  GT_BAD_PTR (5)				- null pointer error
*       GT_NOT_SUPPORTED (0x10)  	- for non-combo ports
*       GT_HW_ERROR (0x10017)     	- on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiPreferredMediaGet
(
	IN GT_U8	devNum,
	IN GT_U8	portNum,
	OUT GT_U8	*autoMediaType
)
{
	GT_STATUS  result = GT_OK;
    GT_U16 phyType;           /* PHY type */
    GT_BOOL isMarvellPhy;     /* indicator whether the PHY is a Marvell PHY */
	GT_U16 data = 0,old_page = 0;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(autoMediaType);

    /* No Media Auto-Selection on XG port */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray->portType == PRV_CPSS_PORT_XG_E) {
        return GT_NOT_SUPPORTED;
    }

    /* get PHY type. Don't check return status. It may be not GT_OK if PHY
       not connected to a port */
    cpssGenPhyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);

	result = GT_NOT_SUPPORTED;
    /* currently support for Marvell PHYs */
    if (isMarvellPhy) {
        /* for PHY 1112 support combo ports */
        if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) {
			/*
			 * Disable PHY Auto-Negotiation
			 */
			PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
							devNum, portNum, GT_FALSE);

			/*
			 * MAC Specific Control Register 1
			 *	get Auto-Selection Preferred Media
			 * 	page 2 register 16 bit[11:10] value as follows:
			 *		00 - no preference for media
			 *		01 - prefer fiber medium
			 *		10 - prefer copper medium
			 *		11 - reserved
			 */
			/* step 1: select page 2 */
			/* backup old page value */
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,&old_page);
			if(GT_OK != result) {
				osPrintf("phy 1112 backup page read PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,result);
				goto restore_page;
			}
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,2);
			if(GT_OK != result) {
				osPrintf("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,2,result);
				goto restore_page;
			}
			/* step 2: read PHY register 16 */
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,16,&data);
			if(GT_OK != result) {
				osPrintf("phy 1112 read PHY:dev %d port %d reg %d error %d\r\n", \
							devNum,portNum,16,result);
				goto restore_page;
			}
			data &= 0xC00; /* reserve bit[11:10] */
			data >>= 10;
			*autoMediaType = (0x2 == data) ? 2 :	\
								((0x1 == data) ? 1 : 0);

			osPrintf("%s :: dev %d , port %d data %02x ,media %d\n",__func__,devNum,portNum,data,*autoMediaType);
		}
    }

	if(GT_NOT_SUPPORTED == result) {
		*autoMediaType = 0; 
		return GT_OK;
	}

restore_page:	
	/* restore old page selection */
	result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page);
	if(GT_OK != result) {
		osPrintf("phy 1112 restore page write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,result);
	}			

	/*
	 * Enable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);
	
	return result;
}

/***********************************
 *get link up port phy Media type 
 * IN params:
 *			devNum,
 *			portNum
 * OUT params:
 *			phyMediaType
 *
 * Return :
 *		result -- GT_OK 
 *		result -- GT_NOT_SUPPORT
 *		...
 *		...
 *Commends :
 *		phyMediaType-- 0: fiber linkup
 *					-- 1: copper linkup
 **********************************/
GT_STATUS cpssDxChPhyPortMediaTypeGet
(
	IN GT_U8	devNum,
	IN GT_U8	portNum,
	OUT GT_U8	*phyMediaType
)
{
	GT_STATUS  result = GT_OK;
	GT_U16 phyType; 		  /* PHY type */
	GT_BOOL isMarvellPhy;	  /* indicator whether the PHY is a Marvell PHY */
	GT_U16 data = 0, tempVal = 0, old_page = 0;
	GT_U8 autoMediaType = 0; /* 0 - prefer non; 1 - prefer fiber; 2 - prefer copper */
	GT_U8 fiberState = 0, copperState = 0, fiberStateWithAn =0 , copperStateWithAn = 0;
	
	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
	PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

	/* No Media Auto-Selection on XG port */
	if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray->portType == PRV_CPSS_PORT_XG_E) {
		return GT_OK;
	}
	
	/* get PHY type. Don't check return status. It may be not GT_OK if PHY
	   not connected to a port */
	cpssGenPhyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);

	result = GT_NOT_SUPPORTED;
	/* currently support for Marvell PHYs */
	if (isMarvellPhy){
		/* for PHY 1112 support combo ports */
		if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) {
			/*
			 * Disable PHY Auto-Negotiation
			 */
			PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
							devNum, portNum, GT_FALSE);

			/*
			 * read copper and fiber status register to check which is link up
			 *	copper status register at page 0 register 1 bit[2] for link up or down
			 *	fiber status register at page 1 register 1 bit[2] for link up or down
			 *
			 * if both of media link up and we will take advantage of Fiber link as default
			 * so we check fiber link first
			 */
			/* backup old page value */
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,&old_page);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 backup page read PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,result);
				goto restore_page;
			}
			
			/* phase 0 step 1: read preferred media info */
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,2);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,2,result);
				goto restore_page;
			}
			/* phase 0 step 2: read PHY register 16 */
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,16,&data);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 read PHY:dev %d port %d reg %d error %d\r\n", \
							devNum,portNum,16,result);
				goto restore_page;
			}
			data &= 0xC00; /* reserve bit[11:10] */
			data >>= 10;
			autoMediaType = (0x2 == data) ? 2 : \
								((0x1 == data) ? 1 : 0);
			/* phase 1	step 1: select page 1 */
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1,result);
				goto restore_page;
			}
			/* phase 1 step 2: read PHY register 1 */
			data = 0;
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,PRV_CPSS_PHY_STATUS,&data);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 read PHY:dev %d port %d reg %d error %d\r\n", \
							devNum,portNum,PRV_CPSS_PHY_STATUS,result);
				goto restore_page;
			}
			/* bit2: fiber link up 
			 *	bit5: fiber Auto-Nego Completed
			 *	both condition match means fiber is link up
			 */
			if(0x24 == (data & 0x24)) {
				fiberStateWithAn = 1;
			}
			else if(0x4 == (data & 0x4)) {
				fiberState = 1;
			}
			
			/* phase 2 step 1: select page 0 */
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0,result);
				goto restore_page;
			}
			/* phase 2 step 2: read PHY register 1 */
			data = 0;
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,PRV_CPSS_PHY_STATUS,&data);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 read PHY:dev %d port %d reg %d error %d\r\n", \
							devNum,portNum,PRV_CPSS_PHY_STATUS,result);
				goto restore_page;
			}
			/* bit2:copper link up 
			  * bit5:copper Auto-Nego Completed
			  * both condition match means copper link up
			  */
			if(0x24 == (data & 0x24)) { 
				copperStateWithAn = 1;
			}			
			else if(0x4 == (data & 0x4)) {
				copperState = 1;
			}
 

			/* arbitrate which media should UP */
			/* 
			  * when combo Auto-Media work under fiber mode, set value 0x7488
			  * when combo Auto-Media work under copper mode, set value 0x8817
			  */

			if((1 == autoMediaType) && fiberStateWithAn) {
				tempVal = 0x7488;
				*phyMediaType = 0;
			}
			else if((2 == autoMediaType) && copperStateWithAn) {
				tempVal = 0x8817;
				*phyMediaType = 1;
			}
			else if(fiberStateWithAn) {
				tempVal = 0x7488;
				*phyMediaType = 0;
			}
			else if(copperStateWithAn) {
				tempVal = 0x8817;
				*phyMediaType = 1;
			}
			else if(fiberState && (1 == old_page)) {
				tempVal = 0x7488;
				*phyMediaType = 0;
			}
			else if(copperState && (0 == old_page)) {
				tempVal = 0x8817;
				*phyMediaType = 1;
			}			
			else {
				goto restore_page;
			}
			
			/* LOS INIT STATUS[1:0] function control
			  *  bit[15,12]:0111: LOS control
			  *  bit[11:8]:0100: INIT control
			  *  bit[7:4]:0001: STATUS[1] control
			  *  bit[3:0]:0111: STATUS[0] control
			  *
			  * write register 22 value 3:select page 3
			  */
			/* phase 1 step 3 */
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,3);
			if(result != GT_OK) {		
				osPrintfErr("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,3,result);
				goto restore_page;
			}
			/* 
			  * Set LED interface
			  */
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum, 16,tempVal);
			if(result != GT_OK) {
				osPrintfErr("phy 1112 write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,16,tempVal,result);
				goto restore_page;
			}
		}
	} 
	if( GT_NOT_SUPPORTED == result ) return GT_OK;
	
restore_page:
	/* restore old page selection */
	result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page);
	if(GT_OK != result) {
		osPrintfErr("phy 1112 restore page write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,result);
		return result;
	}	
	/*
	 * Enable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);

	return result;

}

/*******************************************************************************
* cpssDxChPhyPortMediaChange
*
* DESCRIPTION:
*      control PHY LED when different type of media pluged in
*	 irrelevant when port PHY type is not combo(or say Media Auto-Selection Interface)
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*	NULL
*
* RETURNS:
*       GT_OK (0)  					- on success
*       GT_BAD_PARAM (4)    	   	- on wrong devNum or portNum.
*	  GT_BAD_PTR (5)				- null pointer error
*       GT_NOT_SUPPORTED (0x10)  	- for non-combo ports
*       GT_HW_ERROR (0x10017)     	- on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortMediaChange
(
	IN GT_U8	devNum,
	IN GT_U8	portNum
)
{
	GT_STATUS  result = GT_OK;
	GT_U16 phyType; 		  /* PHY type */
	GT_BOOL isMarvellPhy;	  /* indicator whether the PHY is a Marvell PHY */
	GT_U16 data = 0, tempVal = 0, old_page = 0;

	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
	PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);
	/* No Media Auto-Selection on XG port */
	if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray->portType == PRV_CPSS_PORT_XG_E) {
		return GT_OK;
	}

	/* get PHY type. Don't check return status. It may be not GT_OK if PHY
	   not connected to a port */
	cpssGenPhyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);

	result = GT_NOT_SUPPORTED;
	/* currently support for Marvell PHYs */
	if (isMarvellPhy) {
		/* for PHY 1112 support combo ports */
		if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) {
			/*
			 * Disable PHY Auto-Negotiation
			 */
			PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
							devNum, portNum, GT_FALSE);
			
			/*
			 * read copper and fiber status register to check which is link up
			 * 	copper status register at page 0 register 1 bit[2] for link up or down
			 * 	fiber status register at page 1 register 1 bit[2] for link up or down
			 *
			 * if both of media link up and we will take advantage of Fiber link as default
			 * so we check fiber link first
			 */
			/* phase 1  step 1: select page 1 */
			/* backup old page value */
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,&old_page);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 backup page read PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,result);
				goto restore_page;
			}
			
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1,result);
				goto restore_page;
			}
			/* phase 1 step 2: read PHY register 1 */
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,PRV_CPSS_PHY_STATUS,&data);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 read PHY:dev %d port %d reg %d error %d\r\n", \
							devNum,portNum,PRV_CPSS_PHY_STATUS,result);
				goto restore_page;
			}
			/* bit2: fiber link up 
			 *  bit5: fiber Auto-Nego Completed
			 *  both condition match means fiber is link up
			 */
			if(0x4 == (data & 0x4)) { 
				/* LOS INIT STATUS[1:0] function control
				  *  bit[15,12]:0111: LOS control
				  *  bit[11:8]:0100: INIT control
				  *  bit[7:4]:0001: STATUS[1] control
				  *  bit[3:0]:0111: STATUS[0] control
				  *
				  * write register 22 value 3:select page 3
				  */
				/* phase 1 step 3 */
				tempVal = 0;
				result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,3);
				if(result != GT_OK) {		
					osPrintfErr("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
								devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,3,result);
					goto restore_page;
				}
				/* 
				  * when combo Auto-Media work under fiber mode, set value 0x7488
				  */
				tempVal = 0x7488;
				result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum, 16,tempVal);
				if(result != GT_OK) {
					osPrintfErr("phy 1112 write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
								devNum,portNum,16,tempVal,result);
					goto restore_page;
				}
				/* take advantage of Fiber link, so do not check copper now */
				goto restore_page;
			}
			
			/* phase 2 step 1: select page 0 */
			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0,result);
				goto restore_page;
			}
			/* phase 2 step 2: read PHY register 1 */
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,PRV_CPSS_PHY_STATUS,&data);
			if(GT_OK != result) {
				osPrintfErr("phy 1112 read PHY:dev %d port %d reg %d error %d\r\n", \
							devNum,portNum,PRV_CPSS_PHY_STATUS,result);
				goto restore_page;
			}
			/* bit2:copper link up 
			  * bit5:copper Auto-Nego Completed
			  *	both condition match means copper link up
			  */
			if(0x4 == (data & 0x4)) { 
				/* LOS INIT STATUS[1:0] function control
				  *  bit[15,12]:0111: LOS control
				  *  bit[11:8]:0100: INIT control
				  *  bit[7:4]:0001: STATUS[1] control
				  *  bit[3:0]:0111: STATUS[0] control
				  *
				  * write register 22 value 3:select page 3
				  */
				/* phase 2 step 3 */
				tempVal = 0;
				result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,3);
				if(result != GT_OK) {		
					osPrintfErr("phy 1112 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
								devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,3,result);
					goto restore_page;
				}
				/* 
				  * when combo Auto-Media work under copper mode, set value 0x7817
				  */
				tempVal = 0x7817;
				result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum, 16,tempVal);
				if(result != GT_OK) {
					osPrintfErr("phy 1112 write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
								devNum,portNum,16,tempVal,result);
					goto restore_page;
				}
			}
			
		}
	}

	if( GT_NOT_SUPPORTED == result )	return GT_OK;

restore_page:
	/* restore old page selection */
	result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page);
	if(GT_OK != result) {
		osPrintfErr("phy 1112 restore page write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,result);
		return result;
	}
	/*
	 * Enable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);

	return result;
}

/*******************************************************************************
* cpssDxChPhyPortLinkForceSet
*
* DESCRIPTION:
*   This API implements PHY port link status force up/down
*
* INPUTS:
*	  devNum 	- CH2 Device Number
*	  portNum 	- Device Port Number
*	  state 	- Link Force State(UP<1> or DOWN<0> or AUTO<2>)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
GT_STATUS cpssDxChPhyPortLinkForceSet
(
	IN GT_U8  devNum,
	IN GT_U8  portNum,
	IN GT_U32 state
)
{
    GT_U16 phyType = 0;           /* PHY type */
    GT_BOOL isMarvellPhy = 0;     /* indicator whether the PHY is a Marvell PHY */
    GT_STATUS status = GT_OK;     /* returned status */
	GT_STATUS ret = 0;
    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum)) {
        return GT_BAD_PARAM;
    }
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* No errata to fix on XG port */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray->portType == PRV_CPSS_PORT_XG_E) {
        return GT_OK;
    }

    /* get PHY type. Don't check return status. It may be not GT_OK if PHY
       not connected to a port */
    cpssGenPhyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);

	/* Set Link Force for Marvell PHYs */
    if (isMarvellPhy) {
        /* for PHY 1112 */
        if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) {
        	osPrintf("cpssDxChPhyPortLinkForceSet::MV phyType %#0x is 1112\r\n",phyType);
			if(0x1 == state){ /* Force UP */
				status = phy1112LinkForceUp (
										devNum,
										portNum,
										1,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			else if(0x0 == state) { /* Force DOWN */
				status = phy1112LinkForceDown (
										devNum,
										portNum,
										1,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			else { /* No Force Link */
				status = phy1112LinkForceUp (
										devNum,
										portNum,
										0,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
				status = phy1112LinkForceDown (
										devNum,
										portNum,
										0,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortLinkForceSet::dev %d port %d link %s error %d\r\n", \
							devNum,portNum,(0x1 == state) ? "UP" : ((0x0 == state)? "DOWN":"AUTO"));
				return status;
            }
        }

        /* for PHY 114x */
        else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E114X) {
			osPrintf("cpssDxChPhyPortLinkForceSet::MV phyType %#0x is 114x\r\n",phyType);
			if(0x1 == state){ /* Force UP */
				status = phy114xLinkForceUp (
										devNum,
										portNum,
										1,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			else if(0x0 == state) { /* Force DOWN */
				status = phy114xLinkForceDown (
										devNum,
										portNum,
										1,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			else { /* No Force Link */
				status = phy114xLinkForceUp (
										devNum,
										portNum,
										0,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
				status = phy114xLinkForceDown (
										devNum,
										portNum,
										0,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortLinkForceSet::dev %d port %d link %s error %d\r\n", \
							devNum,portNum,(0x1 == state) ? "UP" : ((0x0 == state)? "DOWN":"AUTO"));
				return status;
			}
        }        

        /* for PHY 1240 */
        else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1240) {
			osPrintf("cpssDxChPhyPortLinkForceSet::MV phyType %#0x is 1240\r\n",phyType);
			if(0x1 == state){ /* Force UP */
				status = phy1240LinkForceUp (
										devNum,
										portNum,
										1,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			else if(0x0 == state) { /* Force DOWN */
				status = phy1240LinkForceDown (
										devNum,
										portNum,
										1,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			else { /* No Force Link */
				status = phy1240LinkForceUp (
										devNum,
										portNum,
										0,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
				status = phy1240LinkForceDown (
										devNum,
										portNum,
										0,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortLinkForceSet::dev %d port %d link %s error %d\r\n", \
							devNum,portNum,(0x1 == state) ? "UP" : ((0x0 == state) ?"DOWN":"AUTO"));
				return status;
			}
        }
        /* for PHY 1340 */
        else if ( ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340) \
			        || ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340_X0) ) {
			osPrintf("cpssDxChPhyPortLinkForceSet::MV phyType %#0x is 1340\r\n",phyType);
			if(0x1 == state){ /* Force UP */
				status = phy1340LinkForceUp (
										devNum,
										portNum,
										1,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			else if(0x0 == state) { /* Force DOWN */
				status = phy1340LinkForceDown (
										devNum,
										portNum,
										1,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			else { /* No Force Link */
				status = phy1340LinkForceUp (
										devNum,
										portNum,
										0,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
				status = phy1340LinkForceDown (
										devNum,
										portNum,
										0,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			}
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortLinkForceSet::dev %d port %d link %s error %d\r\n", \
							devNum,portNum,(0x1 == state) ? "UP" : ((0x0 == state) ?"DOWN":"AUTO"));
				return status;
			}
        }		
    }
	return status;
}

/*******************************************************************************
* cpssDxChPhyPortAutonegSet
*
* DESCRIPTION:
*   This API implements PHY port Auto-negotiation enable or disable.
*
* INPUTS:
*	  devNum 	- CH2 Device Number
*	  portNum 	- Device Port Number
*	  state 	- Autoneg State(enable<1> or disable<0>)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
GT_STATUS cpssDxChPhyPortAutonegSet
(
	IN GT_U8  devNum,
	IN GT_U8  portNum,
	IN GT_U32 state
)
{
    GT_U16 phyType = 0;           /* PHY type */
    GT_BOOL isMarvellPhy = 0;     /* indicator whether the PHY is a Marvell PHY */
    GT_STATUS status = GT_OK;     /* returned status */
	GT_STATUS ret = 0;
    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum)) {
        return GT_BAD_PARAM;
    }
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* No errata to fix on XG port */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray->portType == PRV_CPSS_PORT_XG_E) {
        return GT_OK;
    }

    /* get PHY type. Don't check return status. It may be not GT_OK if PHY
       not connected to a port */
    cpssGenPhyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);

	/* Set Link Force for Marvell PHYs */
    if (isMarvellPhy) {
        /* for PHY 1112 */
        if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) {
        	osPrintf("cpssDxChPhyPortAutonegSet::MV phyType %#0x is 1112\r\n",phyType);
			status = phy1112AutoState (
										devNum,
										portNum,
										state,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortAutonegSet::dev %d port %d link %s error %d\r\n", \
							devNum,portNum,(0x1 == state) ? "enable" : "disable");
                return status;
            }			
        }
        /* for PHY 114x */
        else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E114X) {
			osPrintf("cpssDxChPhyPortAutonegSet::MV phyType %#0x is 114x\r\n",phyType);
			status = phy114xAutoState (
										devNum,
										portNum,
										state,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortAutonegSet::dev %d port %d link %s error %d\r\n", \
							devNum,portNum,(0x1 == state) ? "enable" : "disable");
				return status;
			}
        }        
        /* for PHY 1240 */
		else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1240) {
			osPrintf("cpssDxChPhyPortAutonegSet::MV phyType %#0x is 1240\r\n",phyType);
			status = phy1240AutoState (
										devNum,
										portNum,
										state,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			if(status != GT_OK) {
				osPrintf("cpssDxChPhyPortAutonegSet::dev %d port %d link %s error %d\r\n", \
							devNum,portNum,(0x1 == state) ? "enable" : "disable");
				return status;
			}
        }
        /* for PHY 1340 */
        else if ( ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340) \
			        || ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340_X0) ) {
			osPrintf("cpssDxChPhyPortAutonegSet::MV phyType %#0x is 1340\r\n",phyType);
			status = phy1340AutoState (
										devNum,
										portNum,
										state,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
			if(status != GT_OK) {
				osPrintf("cpssDxChPhyPortAutonegSet::dev %d port %d link %s error %d\r\n", \
							devNum,portNum,(0x1 == state) ? "enable" : "disable");
				return status;
			}
        }

		
    }
	return status;
}

/*******************************************************************************
* cpssDxChPhyPortSpeedSet
*
* DESCRIPTION:
*   This API implements PHY port speed set
*
* INPUTS:
*	  devNum 	- CH2 Device Number
*	  portNum 	- Device Port Number
*	  speed 	- Speed 10<0> or Speed 100<1> or Speed 1000<2>
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
GT_STATUS cpssDxChPhyPortSpeedSet
(
	IN GT_U8  devNum,
	IN GT_U8  portNum,
	IN GT_U32 speed
)
{
    GT_U16 phyType = 0;           /* PHY type */
    GT_BOOL isMarvellPhy = 0;     /* indicator whether the PHY is a Marvell PHY */
    GT_STATUS status = GT_OK;     /* returned status */
    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum)) {
        return GT_BAD_PARAM;
    }
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* No errata to fix on XG port */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray->portType == PRV_CPSS_PORT_XG_E) {
        return GT_OK;
    }

    /* get PHY type. Don't check return status. It may be not GT_OK if PHY
       not connected to a port */
    cpssGenPhyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);

	/* Set Link Force for Marvell PHYs */
    if (isMarvellPhy) {	
        /* for PHY 1112 */
        if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) {
        	osPrintf("cpssDxChPhyPortSpeedSet::MV phyType %#0x is 1112\r\n",phyType);
			status = phy1112SpeedSet (
									devNum,
									portNum,
									speed,
									&cpssDxChPhyPortSmiRegisterRead,
									&prvCpssDxChPhyPortSmiRegisterWrite);
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortSpeedSet::dev %d port %d link %s error %d\r\n", \
							devNum,portNum,(0x1 == speed) ? "speed 100" : ((0x0 == speed)? "speed 10":"speed 1000"));
				return status;
            }
        }
        /* for PHY 114x */
        else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E114X) {
			osPrintf("cpssDxChPhyPortSpeedSet::MV phyType %#0x is 114x\r\n",phyType);
			status = phy114xSpeedSet (
									devNum,
									portNum,
									speed,
									&cpssDxChPhyPortSmiRegisterRead,
									&prvCpssDxChPhyPortSmiRegisterWrite);
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortLinkForceSet::dev %d port %d link %s error %d\r\n", \
												devNum,portNum,(0x1 == speed) ? "Speed 100" : ((0x0 == speed) ?"Speed 10":"Speed 1000"));

				return status;
			}
        }        
        /* for PHY 1240 */
        else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1240) {
			osPrintf("cpssDxChPhyPortSpeedSet::MV phyType %#0x is 1240\r\n",phyType);
			status = phy1240SpeedSet (
									devNum,
									portNum,
									speed,
									&cpssDxChPhyPortSmiRegisterRead,
									&prvCpssDxChPhyPortSmiRegisterWrite);
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortLinkForceSet::dev %d port %d link %s error %d\r\n", \
							devNum,portNum,(0x1 == speed) ? "Speed 100" : ((0x0 == speed) ?"Speed 10":"Speed 1000"));
				return status;
			}
        }
        /* for PHY 1340, the same as 1240  */
        else if ( ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340) \
			        || ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340_X0) ) {
			osPrintf("cpssDxChPhyPortSpeedSet::MV phyType %#0x is 1340\r\n",phyType);
			status = phy1340SpeedSet (
									devNum,
									portNum,
									speed,
									&cpssDxChPhyPortSmiRegisterRead,
									&prvCpssDxChPhyPortSmiRegisterWrite);
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortLinkForceSet::dev %d port %d link %s error %d\r\n", \
							devNum,portNum,(0x1 == speed) ? "Speed 100" : ((0x0 == speed) ?"Speed 10":"Speed 1000"));
				return status;
			}
        }
		
    }
	return status;
}
/*******************************************************************************
* cpssDxChPhyPortDuplexSet
*
* DESCRIPTION:
*   This API implements PHY port duplex set
*
* INPUTS:
*	  devNum 	- CH2 Device Number
*	  portNum 	- Device Port Number
*	  duplex 	- full mode<0> or half mode<1>
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
GT_STATUS cpssDxChPhyPortDuplexSet
(
	IN GT_U8  devNum,
	IN GT_U8  portNum,
	IN GT_U32 duplex
)
{
    GT_U16 phyType = 0;           /* PHY type */
    GT_BOOL isMarvellPhy = 0;     /* indicator whether the PHY is a Marvell PHY */
    GT_STATUS status = GT_OK;     /* returned status */
	GT_U32 state = 0;
	
    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum)) {
        return GT_BAD_PARAM;
    }
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* No errata to fix on XG port */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray->portType == PRV_CPSS_PORT_XG_E) {
        return GT_OK;
    }

    /* get PHY type. Don't check return status. It may be not GT_OK if PHY
       not connected to a port */
    cpssGenPhyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);

	/* Set Link Force for Marvell PHYs */
    if (isMarvellPhy) {	
        /* for PHY 1112 */
        if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) {
        	osPrintf("cpssDxChPhyPortDuplexSet::MV phyType %#0x is 1112\r\n",phyType);
			state = (0x1 == duplex) ? 0 : 1; /* state:0 - duplex half; state:1 - duplex full*/
			status = phy1112DuplexSet (
									devNum,
									portNum,
									state,
									&cpssDxChPhyPortSmiRegisterRead,
									&prvCpssDxChPhyPortSmiRegisterWrite);
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortDuplexSet::dev %d port %d link %s\n", \
							devNum,portNum,(0x1 == duplex) ? "half" : "full");
				return status;
            }		
        }
        /* for PHY 114x */
        else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E114X) {
			osPrintf("cpssDxChPhyPortDuplexSet::MV phyType %#0x is 114x\r\n",phyType);
			state = (0x1 == duplex) ? 0 : 1; /* state:0 - duplex half; state:1 - duplex full*/
			status = phy114xDuplexSet (
									devNum,
									portNum,
									state,
									&cpssDxChPhyPortSmiRegisterRead,
									&prvCpssDxChPhyPortSmiRegisterWrite);
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortDuplexSet::dev %d port %d link %s\n", \
							devNum,portNum,(0x1 == duplex) ? "half" : "full");
				return status;
			}
        }        
        /* for PHY 1240 */
        else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1240) {
			osPrintf("cpssDxChPhyPortDuplexSet::MV phyType %#0x is 1240\r\n",phyType);
			state = (0x1 == duplex) ? 0 : 1; /* state:0 - duplex half; state:1 - duplex full*/
			status = phy1240DuplexSet (
									devNum,
									portNum,
									state,
									&cpssDxChPhyPortSmiRegisterRead,
									&prvCpssDxChPhyPortSmiRegisterWrite);
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortDuplexSet::dev %d port %d link %s\n", \
							devNum,portNum,(0x1 == duplex) ? "half" : "full");

				return status;
			}
        }
        /* for PHY 1340 */
        else if ( ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340) \
			        || ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340_X0) ) {
			osPrintf("cpssDxChPhyPortDuplexSet::MV phyType %#0x is 1340\r\n",phyType);
			state = (0x1 == duplex) ? 0 : 1; /* state:0 - duplex half; state:1 - duplex full*/
			status = phy1340DuplexSet (
									devNum,
									portNum,
									state,
									&cpssDxChPhyPortSmiRegisterRead,
									&prvCpssDxChPhyPortSmiRegisterWrite);
			if (status != GT_OK) {
				osPrintf("cpssDxChPhyPortDuplexSet::dev %d port %d link %s\n", \
							devNum,portNum,(0x1 == duplex) ? "half" : "full");

				return status;
			}
        }		
    }

	return status;
}
/*******************************************************************************
* cpssDxChPhyPortRebootSet
*
* DESCRIPTION:
*   This API implements PHY port Auto-negotiation enable or disable.
*
* INPUTS:
*	  devNum 	- CH2 Device Number
*	  portNum 	- Device Port Number
*	  state 	- Autoneg State(enable<1> or disable<0>)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
******************************************************************************/
GT_STATUS cpssDxChPhyPortRebootSet
(
	IN GT_U8  devNum,
	IN GT_U8  portNum
)
{
    GT_U16 phyType = 0;           /* PHY type */
    GT_BOOL isMarvellPhy = 0;     /* indicator whether the PHY is a Marvell PHY */
    GT_STATUS status = GT_OK;     /* returned status */
	GT_STATUS ret = 0;

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum)) {
        return GT_BAD_PARAM;
    }
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* No errata to fix on XG port */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray->portType == PRV_CPSS_PORT_XG_E) {
        return GT_OK;
    }

    /* get PHY type. Don't check return status. It may be not GT_OK if PHY
       not connected to a port */
    cpssGenPhyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);

	/* Set Link Force for Marvell PHYs */
    if (isMarvellPhy) {
        /* for PHY 1112 */
        if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1112) {
        	osPrintf("cpssDxChPhyPortAutonegSet::MV phyType %#0x is 1112\r\n",phyType);
			status = phy1112RebootSet (
										devNum,
										portNum,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
		}
        /* for PHY 114x */
        else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E114X) {
			osPrintf("cpssDxChPhyPortAutonegSet::MV phyType %#0x is 114x\r\n",phyType);
			status = phy114xRebootSet (
										devNum,
										portNum,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
		}        
        /* for PHY 1240 */
		else if ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1240) {
			osPrintf("cpssDxChPhyPortAutonegSet::MV phyType %#0x is 1240\r\n",phyType);
			status = phy1240RebootSet (
										devNum,
										portNum,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
    	}
        /* for PHY 1340 */
        else if ( ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340) \
			        || ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340_X0) ) {
			osPrintf("cpssDxChPhyPortAutonegSet::MV phyType %#0x is 1340\r\n",phyType);
			status = phy1340RebootSet (
										devNum,
										portNum,
										&cpssDxChPhyPortSmiRegisterRead,
										&prvCpssDxChPhyPortSmiRegisterWrite);
    	}
		
		return status;
	}

	return status;      /* code optimize: Missing return statement. zhangdi@autelan.com 2013-01-18 */	
}

/*******************************************************************************
* cpssDxChPhyPortSmiInit
*
* DESCRIPTION:
*       Initialiaze the SMI control register port, Check all GE ports and
*       activate the errata initialization fix.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number
*	  portNum     - Device port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong devNum
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPhyPortSmiReInit
(
    IN GT_U8 devNum,
    IN GT_U8 portNum
)
{
    GT_U32       fieldOffset;        /* Phy address reg. field offset.   */
    GT_U32       regAddrOffset;      /* Register address offset.         */
    unsigned int value = 0;          /* value to read from  the register */
    GT_U32       regAddr;            /* register address                 */
	GT_U16		 phyPort;		     /* PHY port number */
	GT_U8 		 portCnt = 0;		 /* number of ports                  */
	unsigned int writeValue = 0;	 /* value to write to PHY registers */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    portCnt = PRV_CPSS_PP_MAC(devNum)->numOfPorts;

	if(portNum >= portCnt) {
		osPrintfErr("smi reinit port(%d,%d) with port number out of range(>%d)!\n", devNum, portNum, portCnt);
		return GT_BAD_PARAM;
	}
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiPhyAddr;

    if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum))
        return GT_BAD_PARAM;

    if(portNum < 12)
    {
        PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portNum) =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl;
    }
    else if(portNum < 24)
    {
        PRV_CPSS_PHY_SMI_CTRL_REG_ADDR_MAC(devNum,portNum) =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.smiControl +
            CPSS_DX_SMI_MNG_CNTRL_OFFSET;
    }

    /* read all PHY Address Registers */
    if(PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType == PRV_CPSS_PORT_GE_E)
    {
        /* calc the reg and bit offset for the port */
        regAddrOffset = (portNum / 6) * CPSS_DX_PHY_ADDR_REG_OFFSET;
        fieldOffset   = (portNum % 6) * 5;

	if(prvCpssDrvHwPpReadRegister(
			devNum,regAddr + regAddrOffset, &writeValue) != GT_OK)
	{
		osPrintfErr("smi reinit read PHY address from register %#0x error\n", \
				regAddr+regAddrOffset);
		return GT_HW_ERROR; 	
	}
	
	phyPort = cpssDxChPhySmiFindPhyPortByVirtPort(devNum,portNum);
	if(0xFF == phyPort)
	{
		osPrintfErr("smi reinit find phy port number for (%d,%d) error\n",devNum, portNum);
		return GT_FAIL;
	}
	else
	{
		osPrintfDbg("smi reinit find phy port number %d for (%d,%d)\n",phyPort,devNum,portNum);
		writeValue |= (phyPort << fieldOffset);
	}

    	/* first write all PHY Addresses to PHY Address Register */
	if(prvCpssDrvHwPpWriteRegister(
			devNum,regAddr + regAddrOffset,writeValue) != GT_OK)
	{
		osPrintfErr("smi reinit write PHY address %#0x to register %#0x error\n", \
				writeValue,regAddr+regAddrOffset);
		return GT_HW_ERROR;		
	}

        /* read all PHY Address Registers */
        if (prvCpssDrvHwPpReadRegister(
                devNum, regAddr + regAddrOffset, &value) != GT_OK)
        {
		osPrintfErr("smi reinit dev %d read PHY address from register %#0x error\n", \
				devNum,regAddr+regAddrOffset);
		return GT_HW_ERROR;
        }
	if(value != writeValue)
	{
		osPrintfWarn("smi reinit read after write dev %d reg %#0x data %#0x get %#0x non-consistent value error\n", \
				devNum,regAddr + regAddrOffset, writeValue,value);
	}

	/* init PHY Addresses Array */
        PRV_CPSS_PHY_SMI_PORT_ADDR_MAC(devNum,portNum) = phyPort;
    }

    /* implement workarounds */
    if (prvCpssGenPhyErrataInitFix(devNum,
                             portNum,
                             &cpssDxChPhyPortSmiRegisterRead,
                             &prvCpssDxChPhyPortSmiRegisterWrite ) != GT_OK)
    {
        return GT_FAIL;
    }


	/* Enable PHY Auto-Negotiation */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);

    return GT_OK;
}

#endif

#ifndef __AX_PLATFORM_XCAT__
/***********************************
 * Set port phy Media type 
 * IN params:
 *			devNum,
 *			portNum,
 *          phyMediaType
 *
 * Return :
 *        GT_OK (0)  					- on success
 *        GT_BAD_PARAM (4)    	   	- on wrong devNum or portNum.
 *        GT_NOT_SUPPORTED (0x10)  	- for non-combo ports
 *        GT_HW_ERROR (0x10017)     	- on hardware error
 * Note : zhangdi 2011-04-28
 **********************************/
GT_STATUS cpssDxChPhyPortSmiMediaSet
(
	IN GT_U8	devNum,
	IN GT_U8	portNum,
	IN GT_U8    phyMediaType
)
{
	GT_STATUS  result = GT_OK;
	GT_U16 phyType; 		  /* PHY type */
	GT_BOOL isMarvellPhy;	  /* indicator whether the PHY is a Marvell PHY */
	GT_U16 data = 0,old_page = 0;
	PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
	PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,portNum);

	/* No Media Auto-Selection on XG port !!!!!!!!!!!*/
	if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray->portType == PRV_CPSS_PORT_XG_E) {
		return GT_NOT_SUPPORTED;
	}

	/* get PHY type. Don't check return status. It may be not GT_OK if PHY
	   not connected to a port */
	cpssGenPhyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, &cpssDxChPhyPortSmiRegisterRead);

	result = GT_NOT_SUPPORTED;
	/* currently support for Marvell PHYs */
	if (isMarvellPhy)
	{
		/* for PHY 88E1340 & 88E1340S support combo ports */
		if ( ((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340)||  \
			((phyType & PRV_CPSS_PHY_MODEL_MASK) == PRV_CPSS_DEV_E1340_X0))
		{

			/* backup old page value */
			result = cpssDxChPhyPortSmiRegisterRead(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,&old_page);
			if(GT_OK != result) {
				osPrintfErr("phy 1340 backup page read PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,result);
				goto restore_page;
			}
            if(2 == phyMediaType)    /* PHY_MEDIA_MODE_COPPER */
            {   

				/* Set page to 3,LED control */
    			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0x3);
    			if(GT_OK != result) 
				{
				    osPrintfErr("Set page to 3 error! \n");
				    goto restore_page;
			    }				
				/* Set led[2] led[3] off */
    			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,16,0x8817);
    			if(GT_OK != result) 
				{
				    osPrintfErr("Set led to copper error! \n");
				    goto restore_page;
			    }

				osPrintfDbg("copper: set page to 0 !\n");
    			/* copper: set page to 0 */			
    			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0);
    			if(GT_OK != result) 
				{
				    osPrintfErr("phy 1340 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0,result);
				    goto restore_page;
			    }								
            }
			else if(1 == phyMediaType)    /* PHY_MEDIA_MODE_FIBER */
			{
				/* Set page to 3,LED control */
    			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,0x3);
    			if(GT_OK != result) 
				{
				    osPrintfErr("Set page to 3 error! \n");
				    goto restore_page;
			    }				
				/* Set led[0] led[1] off */
    			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,16,0x0488);
    			if(GT_OK != result) 
				{
				    osPrintfErr("Set led to copper error! \n");
				    goto restore_page;
			    }

				osPrintfDbg("fiber: set page to 1 !\n");
    			/* fiber: set page to 1 */	
    			result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1);
    			if(GT_OK != result) 
				{
				    osPrintfErr("phy 1340 select page write PHY:dev %d port %d reg %d val %#0x error %d\r\n", \
							devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,1,result);
				    goto restore_page;
			    }					
			}
			else
			{
				goto restore_page;
			}
			
		}
	}

	if(GT_OK== result)
	{
		return GT_OK;   /* we should not set page back zhangdi */
	}

restore_page:
	/* restore old page selection */
	result = prvCpssDxChPhyPortSmiRegisterWrite(devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page);
	if(GT_OK != result) 
	{
		osPrintfErr("phy 1340 restore page write PHY:dev %d port %d reg %d data %#0x error %d\r\n", \
					devNum,portNum,PRV_CPSS_GE_PHY_EXT_ADDR,old_page,result);
	}
	
	/*
	 * Enable PHY Auto-Negotiation
	 */
	PRV_CPSS_PP_MAC(devNum)->phyInfo.genVctBindFunc.cpssPhyAutoNegotiationSet( \
					devNum, portNum, GT_TRUE);
	
	return result;
}
#endif

