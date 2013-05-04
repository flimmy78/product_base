/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpConInit.c
*
* DESCRIPTION:
*       Low level driver initialization of PPs, and declarations of global
*       variables
*
* FILE REVISION NUMBER:
*       $Revision: 1.2 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>

extern GT_U16   prvCpssDrvDebugDeviceId[PRV_CPSS_MAX_PP_DEVICES_CNS];

#ifdef ASIC_SIMULATION
    /* Simulation H files */
    #include <asicSimulation/SInit/sinit.h>
#endif /*ASIC_SIMULATION*/

/* address of SDMA configuration register addresses */
#define SDMA_CFG_REG_ADDR       0x2800

/* DX address of PCI Pad Calibration Configuration register addresses */
#define DX_PCI_PAD_CALIB_REG_ADDR       0x0108

/* DX address of PCI Status and Command Register addresses */
#define DX_PCI_STATUS_CMD_REG_ADDR       0x04

/* DX address of the device Id and the revision id register address */
#define DEVICE_ID_AND_REV_REG_ADDR    0x4c

/* DX address of the vendor id register address */
#define VENDOR_ID_REG_ADDR           0x50

/* EXMX address of the class code and the revision id register address */
#define EXMX_PCI_CLASS_CODE_AND_REV_REG_ADDR    0x08

/* PORT_GROUPS_INFO_STC - info about the port groups
        NOTE: assumption that all port groups of the device are the same !
    numOfPortGroups - number of port groups
    portsBmp        - the actual ports bmp of each port group
                        for example: Lion 0xFFF (12 ports)
    maxNumPorts      - number of ports in range
                        for example: Lion 16 ports (12 out of 16 in each port group)
*/
typedef struct {
    GT_U32  numOfPortGroups;
    GT_U32  portsBmp;
    GT_U32  maxNumPorts;
} PORT_GROUPS_INFO_STC;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#define BMP_CONTINUES_PORTS_MAC(x)  ((1<<(x)) - 1)

/* bmp of 52 ports 0..51 -- physical ports (not virtual) */
#define BMP_PORTS_MORE_32_MAC(portsBmpPtr,numPorts) \
    portsBmpPtr->ports[0] = 0xFFFFFFFF;    \
    portsBmpPtr->ports[1] = BMP_CONTINUES_PORTS_MAC(numPorts - 32)

/* bmp of less then 32 ports -- physical ports (not virtual) */
#define BMP_PORTS_LESS_32_MAC(portsBmpPtr,numPorts)    \
    portsBmpPtr->ports[0] = BMP_CONTINUES_PORTS_MAC(numPorts);  \
    portsBmpPtr->ports[1] = 0

/* macro to set range of ports in bmp --
   NOTE :  it must be (startPort + numPorts) <= 31
*/
#define PORT_RANGE_MAC(startPort,numPorts)\
    (BMP_CONTINUES_PORTS_MAC(numPorts) << startPort)

/* macro to set range of port groups in bmp --
   NOTE :  it must be (startPortGroup + numPortGroups) <= 31
*/
#define PORT_GROUPS_RANGE_MAC(startPortGroup,numPortGroups)\
    (BMP_CONTINUES_PORTS_MAC(numPorts) << startPort)

static const CPSS_PORTS_BMP_STC portsBmp24_25_27 = {{(PORT_RANGE_MAC(24,2) |
                                                      PORT_RANGE_MAC(27,1)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to23_27 = {{(PORT_RANGE_MAC(0,24) |
                                                      PORT_RANGE_MAC(27,1)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmpCh3_8_Xg = {{(PORT_RANGE_MAC(24,4) |
                                                      PORT_RANGE_MAC(12,1) |
                                                      PORT_RANGE_MAC(10,1) |
                                                      PORT_RANGE_MAC(4 ,1) |
                                                      PORT_RANGE_MAC(0 ,1)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmpCh3Xg =    {{(PORT_RANGE_MAC(24,4) |
                                                      PORT_RANGE_MAC(22,1) |
                                                      PORT_RANGE_MAC(16,1) |
                                                      PORT_RANGE_MAC(12,1) |
                                                      PORT_RANGE_MAC(10,1) |
                                                      PORT_RANGE_MAC(4 ,1) |
                                                      PORT_RANGE_MAC(0 ,1)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to3_24_25 = {{(PORT_RANGE_MAC(0,4) |
                                                        PORT_RANGE_MAC(24,2)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to7_24_25 = {{(PORT_RANGE_MAC(0,8) |
                                                        PORT_RANGE_MAC(24,2)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to15_24_25 = {{(PORT_RANGE_MAC(0,16) |
                                                         PORT_RANGE_MAC(24,2)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to15_24to27 = {{(PORT_RANGE_MAC(0,16) |
                                                          PORT_RANGE_MAC(24,4)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to12_24to27 = {{(PORT_RANGE_MAC(0,12) |
                                                          PORT_RANGE_MAC(24,4)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to7_24to27 = {{(PORT_RANGE_MAC(0,8) |
                                                        PORT_RANGE_MAC(24,4)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to5_24to27 = {{(PORT_RANGE_MAC(0,6) |
                                                        PORT_RANGE_MAC(24,4)) ,0}};

#if 0
static const CPSS_PORTS_BMP_STC portsBmp_0to11_16to27_32to43_48to59 =
    {{(PORT_RANGE_MAC(0,12) /*0..11 */| PORT_RANGE_MAC(16,12))/*16..27*/ ,
      (PORT_RANGE_MAC(0,12) /*32..43*/| PORT_RANGE_MAC(16,12))/*48..59*/ }};

static const CPSS_PORTS_BMP_STC portsBmp_0to11_32to43_48to59 =
    {{(PORT_RANGE_MAC(0,12) /*0..11 */                      ),
      (PORT_RANGE_MAC(0,12) /*32..43*/| PORT_RANGE_MAC(16,12))/*48..59*/ }};

static const CPSS_PORTS_BMP_STC portsBmp_0to11_16to27_48to59 =
    {{(PORT_RANGE_MAC(0,12) /*0..11 */| PORT_RANGE_MAC(16,12))/*16..27*/ ,
      (                                 PORT_RANGE_MAC(16,12))/*48..59*/ }};

static const CPSS_PORTS_BMP_STC portsBmp_0to11_48to59 =
    {{(PORT_RANGE_MAC(0,12) /*0..11 */                      )/*16..27*/ ,
      (                                 PORT_RANGE_MAC(16,12))/*48..59*/ }};

static const CPSS_PORTS_BMP_STC portsBmp_16to27_32to43_48to59 =
    {{(                                 PORT_RANGE_MAC(16,12))/*16..27*/ ,
      (PORT_RANGE_MAC(0,12) /*32..43*/| PORT_RANGE_MAC(16,12))/*48..59*/ }};

static const CPSS_PORTS_BMP_STC portsBmp_16to27_48to59 =
    {{(                                 PORT_RANGE_MAC(16,12))/*16..27*/ ,
      (                                 PORT_RANGE_MAC(16,12))/*48..59*/ }};

static const CPSS_PORTS_BMP_STC portsBmp_32to43_48to59 =
    {{(0                                                     ) ,
      (PORT_RANGE_MAC(0,12) /*32..43*/| PORT_RANGE_MAC(16,12))/*48..59*/ }};

static const CPSS_PORTS_BMP_STC portsBmp_48to59 =
    {{(0                                                     ) ,
      (                                 PORT_RANGE_MAC(16,12))/*48..59*/ }};
#endif /*0*/


/*****************************************************************************
* Global
******************************************************************************/
/* pointer to the object that control ExMxDx */
static PRV_CPSS_DRV_GEN_OBJ_STC drvGenExMxDxObj;
PRV_CPSS_DRV_GEN_OBJ_STC *prvCpssDrvGenExMxDxObjPtr = &drvGenExMxDxObj;

/* pointer to the object that control PCI */
PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfPciPtr = NULL;

/* pointer to the object that control PCI -- for HA standby */
PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfPciHaStandbyPtr = NULL;

/* pointer to the object that control SMI */
PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfSmiPtr = NULL;

/* pointer to the object that control TWSI */
PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfTwsiPtr = NULL;

/* pointer to the object that control PEX */
/*PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfPexPtr = NULL;*/

/*****************************************************************************
* Externals
******************************************************************************/


/*****************************************************************************
* Forward declarations
******************************************************************************/

/*****************************************************************************
* Local variables
******************************************************************************/
#define END_OF_TABLE    0xFFFFFFFF
/*tiger*/
const static CPSS_PP_DEVICE_TYPE tgXGlegalDevTypes[] =
{   PRV_CPSS_TIGER_1_DEVICES_MAC , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE tg12legalDevTypes[] =
{   PRV_CPSS_TIGER_12_DEVICES_MAC , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE tg52legalDevTypes[] =
{   PRV_CPSS_TIGER_52_DEVICES_MAC , END_OF_TABLE};
/*samba*/
const static  CPSS_PP_DEVICE_TYPE sambaXGlegalDevTypes[] =
{   PRV_CPSS_SAMBA_1_DEVICES_MAC  , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE samba12legalDevTypes[] =
{   PRV_CPSS_SAMBA_12_DEVICES_MAC , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE samba52legalDevTypes[] =
{   PRV_CPSS_SAMBA_52_DEVICES_MAC , END_OF_TABLE};
/*twistd*/
const static  CPSS_PP_DEVICE_TYPE twistdXGlegalDevTypes[] =
{   PRV_CPSS_TWISTD_1_DEVICES_MAC , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE twistd12legalDevTypes[] =
{   PRV_CPSS_TWISTD_12_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE twistd52legalDevTypes[] =
{   PRV_CPSS_TWISTD_52_DEVICES_MAC, END_OF_TABLE };
/*twistc*/
const static  CPSS_PP_DEVICE_TYPE twistc10legalDevTypes[] =
{   PRV_CPSS_TWISTC_10_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE twistc12legalDevTypes[] =
{   PRV_CPSS_TWISTC_12_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE twistc52legalDevTypes[] =
{   PRV_CPSS_TWISTC_52_DEVICES_MAC, END_OF_TABLE };

/* salsa */
const static  CPSS_PP_DEVICE_TYPE salsa24legalDevTypes[] =
{   PRV_CPSS_SALSA_24_DEVICES_MAC , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE salsa16legalDevTypes[] =
{   PRV_CPSS_SALSA_16_DEVICES_MAC , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE salsa12legalDevTypes[] =
{   PRV_CPSS_SALSA_12_DEVICES_MAC , END_OF_TABLE};
/* cheetah */
const static  CPSS_PP_DEVICE_TYPE ch_27legalDevTypes[] =
{   PRV_CPSS_CH_27_DEVICES_MAC , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE ch_26legalDevTypes[] =
{   PRV_CPSS_CH_26_DEVICES_MAC , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE ch_24legalDevTypes[] =
{   PRV_CPSS_CH_24_DEVICES_MAC , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE ch_25legalDevTypes[] =
{   PRV_CPSS_CH_25_DEVICES_MAC , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE ch_16legalDevTypes[] =
{   PRV_CPSS_CH_16_DEVICES_MAC , END_OF_TABLE};
const static  CPSS_PP_DEVICE_TYPE ch_10legalDevTypes[] =
{   PRV_CPSS_CH_10_DEVICES_MAC , END_OF_TABLE};
/* cheetah-2 */
const static  CPSS_PP_DEVICE_TYPE ch2_28legalDevTypes[] =
{   PRV_CPSS_CH2_28_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch2_27legalDevTypes[] =
{   PRV_CPSS_CH2_27_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch2_26legalDevTypes[] =
{   PRV_CPSS_CH2_26_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch2_24legalDevTypes[] =
{   PRV_CPSS_CH2_24_DEVICES_MAC, END_OF_TABLE };
/* cheetah-3 */
const static  CPSS_PP_DEVICE_TYPE ch3_28legalDevTypes[] =
{   PRV_CPSS_CH3_28_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch3_26legalDevTypes[] =
{   PRV_CPSS_CH3_26_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch3_24legalDevTypes[] =
{   PRV_CPSS_CH3_24_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch3_6legalDevTypes[] =
{   PRV_CPSS_CH3_6_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch3_10legalDevTypes[] =
{   PRV_CPSS_CH3_10_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch3_18legalDevTypes[] =
{   PRV_CPSS_CH3_18_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch3_20legalDevTypes[] =
{   PRV_CPSS_CH3_20_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch3_24_2legalDevTypes[] =
{   PRV_CPSS_CH3_24_2_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch3_24_4legalDevTypes[] =
{   PRV_CPSS_CH3_24_4_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch3_8_XGlegalDevTypes[] =
{   PRV_CPSS_CH3_8_XG_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE ch3_XGlegalDevTypes[] =
{   PRV_CPSS_CH3_XG_DEVICES_MAC, END_OF_TABLE };
/* Puma */
const static  CPSS_PP_DEVICE_TYPE puma_26legalDevTypes[] =
{   PRV_CPSS_PUMA_26_DEVICES_MAC, END_OF_TABLE };
const static  CPSS_PP_DEVICE_TYPE puma_24legalDevTypes[] =
{   PRV_CPSS_PUMA_24_DEVICES_MAC, END_OF_TABLE };
/****************/
/* xCat devices */
/****************/
const static  CPSS_PP_DEVICE_TYPE xcat_24_4legalDevTypes[] =
{   PRV_CPSS_XCAT_24GE_4STACK_PORTS_DEVICES ,
    PRV_CPSS_XCAT_24FE_4STACK_PORTS_DEVICES ,
    END_OF_TABLE };

const static  CPSS_PP_DEVICE_TYPE xcat_24_2legalDevTypes[] =
{   PRV_CPSS_XCAT_24GE_2STACK_PORTS_DEVICES ,
    END_OF_TABLE };

const static  CPSS_PP_DEVICE_TYPE xcat_24_0legalDevTypes[] =
{   PRV_CPSS_XCAT_24GE_NO_STACK_PORTS_DEVICES ,
    END_OF_TABLE };

const static  CPSS_PP_DEVICE_TYPE xcat_16_4legalDevTypes[] =
{   PRV_CPSS_XCAT_16GE_4STACK_PORTS_DEVICES,
    END_OF_TABLE };

const static  CPSS_PP_DEVICE_TYPE xcat_16_2legalDevTypes[] =
{   PRV_CPSS_XCAT_16GE_2STACK_PORTS_DEVICES ,
    END_OF_TABLE };

/* xcat device with 12+2*/
const static  CPSS_PP_DEVICE_TYPE xcat_12_2legalDevTypes[] =
{   PRV_CPSS_XCAT_12GE_2STACK_PORTS_26_27_DEVICES , /*where 2 XG are 26,27*/
    END_OF_TABLE };


const static  CPSS_PP_DEVICE_TYPE xcat_8_4legalDevTypes[] =
{   PRV_CPSS_XCAT_8GE_4STACK_PORTS_DEVICES,
    PRV_CPSS_XCAT_8FE_4STACK_PORTS_DEVICES,
    END_OF_TABLE };

const static  CPSS_PP_DEVICE_TYPE xcat_8_2legalDevTypes[] =
{   PRV_CPSS_XCAT_8GE_2STACK_PORTS_DEVICES ,
    PRV_CPSS_XCAT_8FE_2STACK_PORTS_DEVICES ,
    END_OF_TABLE };
/****************/
/* lion devices */
/****************/
/* lion with port groups 0,1,2,3 */
const static  CPSS_PP_DEVICE_TYPE lion_port_groups0123legalDevTypes[] = {CPSS_LION_PORT_GROUPS_0123_CNS , END_OF_TABLE };
/* lion with port groups 0,1 */
const static  CPSS_PP_DEVICE_TYPE lion_port_groups01__legalDevTypes[] = {CPSS_LION_PORT_GROUPS_01___CNS , END_OF_TABLE };

#define LION_NUM_PORT_GROUPS_4_CNS    4

const static PORT_GROUPS_INFO_STC lionPortGroups =
{
    LION_NUM_PORT_GROUPS_4_CNS /*numOfPortGroups*/ ,
    PORT_RANGE_MAC(0,12) /*portsBmp*/   ,
    16 /*maxNumPorts*/
};

/* macro to set any of 4 port groups as active(exists) in a bitmap */
#define SET_4_PORT_GROUPS_MAC(portGroup0Exists,portGroup1Exists,portGroup2Exists,portGroup3Exists) \
    ((portGroup3Exists) << 3) | ((portGroup2Exists) << 2) |                        \
    ((portGroup1Exists) << 1) | (portGroup0Exists)

/* bmp of lion port groups , for 'known' devices */
static const GT_U32 portGroups01__bmp = SET_4_PORT_GROUPS_MAC(1,1,0,0);
static const GT_U32 portGroups0123bmp = SET_4_PORT_GROUPS_MAC(1,1,1,1);

/* number of ports in the lion device according to the last active port group */
#define LION_NUM_PORTS_FROM_LAST_PORT_GROUP_MAC(lastPortGroup) (((lastPortGroup + 1) * 16)-4)

/******************/
/* xCat2 devices */
/******************/
const static  CPSS_PP_DEVICE_TYPE xcat2_8_2legalDevTypes[] =
{   PRV_CPSS_XCAT2_8FE_2STACK_PORTS_DEVICES,
    PRV_CPSS_XCAT2_8GE_2STACK_PORTS_DEVICES,
    END_OF_TABLE};

const static  CPSS_PP_DEVICE_TYPE xcat2_8_4legalDevTypes[] =
{   PRV_CPSS_XCAT2_8FE_4STACK_PORTS_DEVICES,
    PRV_CPSS_XCAT2_8GE_4STACK_PORTS_DEVICES,
    END_OF_TABLE};

const static  CPSS_PP_DEVICE_TYPE xcat2_16_2legalDevTypes[] =
{   PRV_CPSS_XCAT2_16GE_2STACK_PORTS_DEVICES,
    END_OF_TABLE};

const static  CPSS_PP_DEVICE_TYPE xcat2_16_4legalDevTypes[] =
{
    PRV_CPSS_XCAT2_16FE_4STACK_PORTS_DEVICES,
    PRV_CPSS_XCAT2_16GE_4STACK_PORTS_DEVICES,
    END_OF_TABLE};

const static  CPSS_PP_DEVICE_TYPE xcat2_24_2legalDevTypes[] =
{   PRV_CPSS_XCAT2_24GE_2STACK_PORTS_DEVICES,
    END_OF_TABLE};

const static  CPSS_PP_DEVICE_TYPE xcat2_24_4legalDevTypes[] =
{   PRV_CPSS_XCAT2_24FE_4STACK_PORTS_DEVICES,
    PRV_CPSS_XCAT2_24GE_4STACK_PORTS_DEVICES,
    END_OF_TABLE};

/* alias to make line shorter */
#define TWISTC_CNS          CPSS_PP_FAMILY_TWISTC_E
#define TWISTD_CNS          CPSS_PP_FAMILY_TWISTD_E
#define SAMBA_CNS           CPSS_PP_FAMILY_SAMBA_E
#define TIGER_CNS           CPSS_PP_FAMILY_TIGER_E
#define SALSA_CNS           CPSS_PP_FAMILY_SALSA_E
#define CHEETAH_CNS         CPSS_PP_FAMILY_CHEETAH_E
#define CHEETAH2_CNS        CPSS_PP_FAMILY_CHEETAH2_E
#define CHEETAH3_CNS        CPSS_PP_FAMILY_CHEETAH3_E
#define XCAT_CNS            CPSS_PP_FAMILY_DXCH_XCAT_E
#define PUMA_CNS            CPSS_PP_FAMILY_PUMA_E
#define LION_CNS            CPSS_PP_FAMILY_DXCH_LION_E
#define XCAT2_CNS          CPSS_PP_FAMILY_DXCH_XCAT2_E


/* cpssSupportedTypes :
    purpose :
        DB to hold the device types that the CPSS support

fields :
    devFamily - device family (cheetah/salsa/tiger/../puma)
    numOfPorts - number of ports in the device
    devTypeArray - array of devices for the device family that has same number
                   of ports
    defaultPortsBmpPtr - pointer to special ports bitmap.
                         if this is NULL , that means that the default bitmap
                         defined by the number of ports that considered to be
                         sequential for 0 to the numOfPorts
    portGroupInfoPtr - port groups info.
                    NULL --> meaning 'non multi port groups' device
                    otherwise hold info about the port groups of the device
    activePortGroupsBmpPtr - active port groups bitmap.
                    relevant only when portGroupInfoPtr != NULL
                    can't be NULL when portGroupInfoPtr != NULL
*/
const struct {
    CPSS_PP_FAMILY_TYPE_ENT     devFamily;
    GT_U8                       numOfPorts;
    const CPSS_PP_DEVICE_TYPE   *devTypeArray;
    const CPSS_PORTS_BMP_STC    *defaultPortsBmpPtr;
    const PORT_GROUPS_INFO_STC  *portGroupInfoPtr;
    const GT_U32                *activePortGroupsBmpPtr;
}cpssSupportedTypes[] =
{
    {TWISTC_CNS   ,52 ,twistc52legalDevTypes ,NULL,NULL,NULL},
    {TWISTC_CNS   ,12 ,twistc12legalDevTypes ,NULL,NULL,NULL},
    {TWISTC_CNS   ,10 ,twistc10legalDevTypes ,NULL,NULL,NULL},

    {TWISTD_CNS   ,52 , twistd52legalDevTypes,NULL,NULL,NULL},
    {TWISTD_CNS   ,12 , twistd12legalDevTypes,NULL,NULL,NULL},
    {TWISTD_CNS   , 1 , twistdXGlegalDevTypes,NULL,NULL,NULL},

    {SAMBA_CNS    ,52 ,samba52legalDevTypes, NULL,NULL,NULL},
    {SAMBA_CNS    ,12 ,samba12legalDevTypes, NULL,NULL,NULL},
    {SAMBA_CNS    , 1 ,sambaXGlegalDevTypes, NULL,NULL,NULL},

    {TIGER_CNS    ,52 ,tg52legalDevTypes, NULL,NULL,NULL},
    {TIGER_CNS    ,12 ,tg12legalDevTypes, NULL,NULL,NULL},
    {TIGER_CNS    , 1 ,tgXGlegalDevTypes, NULL,NULL,NULL},

    {SALSA_CNS    ,12 ,salsa12legalDevTypes, NULL,NULL,NULL},
    {SALSA_CNS    ,16 ,salsa16legalDevTypes, NULL,NULL,NULL},
    {SALSA_CNS    ,24 ,salsa24legalDevTypes, NULL,NULL,NULL},

    {CHEETAH_CNS  ,27 ,ch_27legalDevTypes, NULL,NULL,NULL},
    {CHEETAH_CNS  ,26 ,ch_26legalDevTypes, NULL,NULL,NULL},
    {CHEETAH_CNS  ,24 ,ch_24legalDevTypes, NULL,NULL,NULL},
    {CHEETAH_CNS  ,25 ,ch_25legalDevTypes, NULL,NULL,NULL},
    {CHEETAH_CNS  ,16 ,ch_16legalDevTypes, NULL,NULL,NULL},
    {CHEETAH_CNS  ,10 ,ch_10legalDevTypes, NULL,NULL,NULL},
    
    {CHEETAH2_CNS ,27 ,ch2_27legalDevTypes, NULL,NULL,NULL},
    {CHEETAH2_CNS ,28 ,ch2_28legalDevTypes, NULL,NULL,NULL},
    {CHEETAH2_CNS ,26 ,ch2_26legalDevTypes, NULL,NULL,NULL},
    {CHEETAH2_CNS ,24 ,ch2_24legalDevTypes, NULL,NULL,NULL},

    {CHEETAH3_CNS ,28 ,ch3_28legalDevTypes, NULL,NULL,NULL},
    {CHEETAH3_CNS ,26 ,ch3_26legalDevTypes, NULL,NULL,NULL},
    {CHEETAH3_CNS ,24 ,ch3_24legalDevTypes, NULL,NULL,NULL},
    {CHEETAH3_CNS ,26 ,ch3_6legalDevTypes,  &portsBmp0to3_24_25,NULL,NULL},
    {CHEETAH3_CNS ,26 ,ch3_10legalDevTypes, &portsBmp0to7_24_25,NULL,NULL},
    {CHEETAH3_CNS ,26 ,ch3_18legalDevTypes, &portsBmp0to15_24_25,NULL,NULL},
    {CHEETAH3_CNS ,28 ,ch3_20legalDevTypes, &portsBmp0to15_24to27,NULL,NULL},
    {CHEETAH3_CNS ,26 ,ch3_24_2legalDevTypes, NULL,NULL,NULL},
    {CHEETAH3_CNS ,28 ,ch3_24_4legalDevTypes, NULL,NULL,NULL},
    {CHEETAH3_CNS ,28 ,ch3_8_XGlegalDevTypes, &portsBmpCh3_8_Xg,NULL,NULL},
    {CHEETAH3_CNS ,28 ,ch3_XGlegalDevTypes, &portsBmpCh3Xg,NULL,NULL},

    /* xCat devices  - 24network+0,2,4stack */
    {XCAT_CNS     ,24+4 ,xcat_24_4legalDevTypes, NULL,NULL,NULL},/*ports 0..27 */
    {XCAT_CNS     ,24+2 ,xcat_24_2legalDevTypes, NULL,NULL,NULL},/*ports 0..25 */
    {XCAT_CNS     ,24+0 ,xcat_24_0legalDevTypes, NULL,NULL,NULL},/*ports 0..23 */

    /* xCat devices  - 16network+0,2,4stack */
    {XCAT_CNS     ,24+4 ,xcat_16_4legalDevTypes, &portsBmp0to15_24to27,NULL,NULL}, /*ports 0..15,24..27 */
    {XCAT_CNS     ,24+2 ,xcat_16_2legalDevTypes, &portsBmp0to15_24to27,NULL,NULL}, /*ports 0..15,24..25 */

    /* xCat devices  - 12network+0,2,4stack */
    {XCAT_CNS     ,24+4 ,xcat_12_2legalDevTypes, &portsBmp0to12_24to27,NULL,NULL}, /*ports 0..11,24..27 */

    /* xCat devices  - 8network+2,4stack */
    {XCAT_CNS     ,24+4 ,xcat_8_4legalDevTypes, &portsBmp0to7_24to27,NULL,NULL}, /*ports 0..7,24..27 */
    {XCAT_CNS     ,24+2 ,xcat_8_2legalDevTypes, &portsBmp0to7_24to27,NULL,NULL}, /*ports 0..7,24..25 */

    /* puma devices */
    {PUMA_CNS     ,28 ,puma_26legalDevTypes, &portsBmp24_25_27,NULL,NULL},
    {PUMA_CNS     ,28 ,puma_24legalDevTypes, &portsBmp0to23_27,NULL,NULL},

    /* Lion devices */                                             /* active port groups */
    {LION_CNS ,LION_NUM_PORTS_FROM_LAST_PORT_GROUP_MAC(3) ,/*0,1,2,3*/lion_port_groups0123legalDevTypes, NULL ,&lionPortGroups,&portGroups0123bmp},
    {LION_CNS ,LION_NUM_PORTS_FROM_LAST_PORT_GROUP_MAC(3) ,/*0,1,   */lion_port_groups01__legalDevTypes, NULL ,&lionPortGroups,&portGroups01__bmp},

    /* xCat2 devices  - 8network, 2stack */
    {XCAT2_CNS     ,24+2 ,xcat2_8_2legalDevTypes, &portsBmp0to7_24_25,NULL,NULL},

    /* xCat2 devices  - 8network, 4stack */
    {XCAT2_CNS     ,24+4 ,xcat2_8_4legalDevTypes, &portsBmp0to7_24to27,NULL,NULL},

    /* xCat2 devices  - 16network, 2stack */
    {XCAT2_CNS     ,24+2 ,xcat2_16_2legalDevTypes, &portsBmp0to15_24_25,NULL,NULL},

    /* xCat2 devices  - 16network, 4stack */
    {XCAT2_CNS     ,24+4 ,xcat2_16_4legalDevTypes, &portsBmp0to15_24to27,NULL,NULL},

    /* xCat2 devices  - 24network, 2stack */
    {XCAT2_CNS     ,24+2 ,xcat2_24_2legalDevTypes, NULL,NULL,NULL},

    /* xCat2 devices  - 24network, 4stack */
    {XCAT2_CNS     ,24+4 ,xcat2_24_4legalDevTypes, NULL,NULL,NULL},

    /* End of list      */
    {(CPSS_PP_FAMILY_TYPE_ENT)END_OF_TABLE   ,0 , NULL,NULL}
};

/* cpssSpecialDevicesBmp -
 * Purpose : DB to hold the devices with special ports BMP
 *
 * NOTE : devices that his port are sequential for 0 to the numOfPorts
 *        not need to be in this Array !!!
 *
 * fields :
 *  devType - device type that has special ports bmp (that is different from
 *            those of other devices of his family with the same numOfPort)
 *  existingPorts - the special ports bmp of the device
 *
*/
const static struct {
    CPSS_PP_DEVICE_TYPE         devType;
    CPSS_PORTS_BMP_STC          existingPorts;
}cpssSpecialDevicesBmp[] =
{
    /* 12 Giga ports (0.11) and XG 24,25 */
    {CPSS_98DX145_CNS , {{PORT_RANGE_MAC(0,12) | PORT_RANGE_MAC(24,2) ,0}}},
    /* 12 Giga ports (0.11) and XG 24 */
    {CPSS_98DX133_CNS , {{PORT_RANGE_MAC(0,12) | PORT_RANGE_MAC(24,1) ,0}}},
    /* No Giga ports and 3XG 24..26 */
    {CPSS_98DX803_CNS , {{PORT_RANGE_MAC(24,3) ,0}}},
    /* No Giga ports and 4XG 24..27 */
    {CPSS_98DX804_CNS , {{PORT_RANGE_MAC(24,4) ,0}}},

    /* End of list      */
    {END_OF_TABLE   ,{{0,0}} }
};



#ifdef ASIC_SIMULATION
typedef enum{
    BIG_ENDIAN_E = 1, /* HOST is 'Big endian' */
    LITTLE_ENDIAN_E   /* HOST is 'Little endian' */
}ENDIAN_ENT;

/*******************************************************************************
* checkHostEndian
*
* DESCRIPTION:
*       This function check if the HOST is 'Big endian' or 'Little endian'
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       one of ENDIAN_ENT -  BIG_ENDIAN_E /  LITTLE_ENDIAN_E
*
* COMMENTS:
*
*******************************************************************************/
static ENDIAN_ENT  checkHostEndian(void)
{
    GT_U32  wordMem = 0x11223344;
    GT_U8  *charMemPtr = (GT_U8*)(&wordMem);

    if(charMemPtr[0] == 0x11)
    {
        /* 'Big endian' - the bits 24..31 sit in byte0 */
        return BIG_ENDIAN_E;
    }

    /* 'Little endian' - the bits 24..31 sit in byte3 */
    return LITTLE_ENDIAN_E;
}
#endif /*ASIC_SIMULATION*/

/*******************************************************************************
* phase1Part1Init
*
* DESCRIPTION:
*       This function is called by prvCpssDrvHwPpPhase1Init()
*       the function bind the PCI/SMI/TWSI driver functions to the driver object
*       functions for this device.
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - bad management interface value
*       GT_NOT_IMPLEMENTED - the needed driver was not compiled correctly
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS phase1Part1Init
(
    IN  GT_U8                devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr
)
{
    GT_STATUS rc;

    /********************************************/
    /* bind the BUS part to the ExMxDx BUS part */
    /********************************************/


    /* set rc as "not implemented" to suggest that the specific SMI/PCI/TWSI is
       not part of the image although needed in the image */
    rc = GT_NOT_IMPLEMENTED;
    /* driver object bind */
    if(ppInInfoPtr->mngInterfaceType == CPSS_CHANNEL_PCI_E)
    {
#if defined GT_PCI  /*|| defined GT_PEX*/
        rc = GT_OK;
        if(ppInInfoPtr->ppHAState == CPSS_SYS_HA_MODE_ACTIVE_E)
        {
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciPtr;
        }
        else
        {
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciHaStandbyPtr;
        }
#endif
    }
    /* SMI */
    else if (ppInInfoPtr->mngInterfaceType == CPSS_CHANNEL_SMI_E)
    {
#ifdef GT_SMI
        rc = GT_OK;
        prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfSmiPtr;
#endif
    }
 /* TWSI */
    else if (ppInInfoPtr->mngInterfaceType == CPSS_CHANNEL_TWSI_E)
    {
#ifdef GT_I2C
        rc = GT_OK;
        prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfTwsiPtr;
#endif
    }
    else if(ppInInfoPtr->mngInterfaceType == CPSS_CHANNEL_PEX_E)
    {
#if defined GT_PCI  /*|| defined GT_PEX*/
        rc = GT_OK;
        if(ppInInfoPtr->ppHAState == CPSS_SYS_HA_MODE_ACTIVE_E)
        {
            /* TBD - prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPexPtr; */
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciPtr;
        }
        else
        {
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciHaStandbyPtr;
        }
#endif
    }
    else
    {
        /* unknown interface type */
        rc = GT_BAD_PARAM;
    }
    return rc;
}

/*******************************************************************************
* phase1Part2Init
*
* DESCRIPTION:
*       This function is called by prvCpssDrvHwPpPhase1Init()
*       This function allocate the memory for the DB of the driver for this
*       device , and set some of it's values.
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_BAD_STATE - the driver is not in state that ready for initialization
*                      for the device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS phase1Part2Init
(
    IN  GT_U8                devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr
)
{
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32      portGroupId;/* the port group Id */
    GT_BOOL     activePortGroupFound = GT_FALSE; /*indication that there is active port group */

    if(prvCpssDrvPpConfig[devNum] != NULL)
    {
        /* the device already exists ?! */
        /* we can't override it ! , the device must be empty before we
           initialize it */
        /* check that "remove device" was done properly */
        return GT_BAD_STATE;
    }

    /* allocate the device a DB */
    ppConfigPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRIVER_PP_CONFIG_STC));
    if(ppConfigPtr == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* set default values */
    cpssOsMemSet(ppConfigPtr,0,sizeof(PRV_CPSS_DRIVER_PP_CONFIG_STC));
    /* bind the memory to the global DB */
    prvCpssDrvPpConfig[devNum] = ppConfigPtr;

    /* set value */
    ppConfigPtr->devNum = devNum;
    ppConfigPtr->drvHwIfSupportedBmp = PRV_CPSS_HW_IF_BMP_MAC(ppInInfoPtr->mngInterfaceType);
    if(ppInInfoPtr->mngInterfaceType == CPSS_CHANNEL_PEX_E)
    {
        /* in most cases we will treat the "PCI" and "PEX" the same */
        ppConfigPtr->drvHwIfSupportedBmp |= PRV_CPSS_HW_IF_BMP_MAC(CPSS_CHANNEL_PCI_E);
    }

    for(portGroupId = 0; portGroupId < ppInInfoPtr->numOfPortGroups; portGroupId++)
    {
        ppConfigPtr->hwCtrl[portGroupId].baseAddr = ppInInfoPtr->busBaseAddr[portGroupId];
        ppConfigPtr->hwCtrl[portGroupId].internalPciBase = ppInInfoPtr->internalPciBase[portGroupId];

        if(ppConfigPtr->hwCtrl[portGroupId].baseAddr != CPSS_PARAM_NOT_USED_CNS)
        {
            /* active port group */
            activePortGroupFound = GT_TRUE;
        }
    }

    if(activePortGroupFound == GT_FALSE)
    {
        /* at least one port group need to be active */
        return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* phase1Part3Init
*
* DESCRIPTION:
*       This function is called by prvCpssDrvHwPpPhase1Init()
*       This function call driverHwCntlInit to initialize the HW control.
*       The function get the deviceType from HW , revision id , doByteSwap
*
*       Note : for standby HA -- the doByteSwap must be figured when the system
*              changed to HA active
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       portGroupId      - port group Id.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS phase1Part3Init
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32  readDeviceId ,readVendorId;/*deviceId,vendor Id as read from HW */
    GT_U32  tempDevId = 0;/* the device type built from device id , vendor id */
    GT_U32  regData;     /* register data */
    GT_U32  ii;
    GT_U32  address;/* address to access */

    ppConfigPtr = prvCpssDrvPpConfig[devNum];

    ppConfigPtr->hwCtrl[portGroupId].doByteSwap = GT_FALSE;

    if(0 == PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum))
    {
        ppConfigPtr->hwCtrl[portGroupId].internalPciBase = 0; /* don't care in non-PCI */

        /* The "do byte swap" relate only to the PCI bus , since on PCI we read
           4 BYTES and don't know if was read correctly

           so for non-PCI we set doByteSwap = GT_FALSE
        */

        /* Init Hw Cntl parameters. --
           must be done prior to any read/write registers ...*/
        rc = prvCpssDrvHwCntlInit(devNum,portGroupId,
                         ppConfigPtr->hwCtrl[portGroupId].baseAddr,
                         ppConfigPtr->hwCtrl[portGroupId].internalPciBase,
                         ppConfigPtr->hwCtrl[portGroupId].doByteSwap,
                         GT_FALSE);/* isDiag */
        if (rc!= GT_OK)
            return rc;

        /* read DeviceID */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId ,DEVICE_ID_AND_REV_REG_ADDR, 4, 16, &readDeviceId);
        if (rc!= GT_OK)
            return rc;
        /* read VendorID */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId ,VENDOR_ID_REG_ADDR, 0, 16, &readVendorId) ;
        if (rc!= GT_OK)
            return rc;

        tempDevId = (readDeviceId << 16) | readVendorId ;
    }
    else  /* PCI/PEX compatible */
    {
        /* Init Hw Cntl parameters. The value of doByteSwap is unknown
         * Assume only prvCpssDrvHwPpReadInternalPciReg() will be used till
         * correct value of doByteSwap known */
        rc = prvCpssDrvHwCntlInit(devNum,portGroupId,
                         ppConfigPtr->hwCtrl[portGroupId].baseAddr,
                         ppConfigPtr->hwCtrl[portGroupId].internalPciBase,
                         ppConfigPtr->hwCtrl[portGroupId].doByteSwap,
                         GT_FALSE);/* isDiag */
        if (rc!= GT_OK)
            return rc;

        /* Read from the PCI channel */
        if(ppInInfoPtr->ppHAState == CPSS_SYS_HA_MODE_ACTIVE_E)
        {
            switch(ppInInfoPtr->devIdLocationType)
            {
                case PRV_CPSS_DRV_DEV_ID_LOCATION_PCI_CONFIG_CYCLE_ADDR_0x00000000_E:
                case PRV_CPSS_DRV_DEV_ID_LOCATION_PEX_CONFIGURATION_HEADER_ADDR_0X00070000_E:
                    if(ppInInfoPtr->devIdLocationType ==
                        PRV_CPSS_DRV_DEV_ID_LOCATION_PEX_CONFIGURATION_HEADER_ADDR_0X00070000_E)
                    {
                        address = 0x70000;
                    }
                    else
                    {
                        address = 0;
                    }

                    /* perform the direct PCI/PEX memory read access */
                    rc = prvCpssDrvHwPpPortGroupReadInternalPciReg(devNum, portGroupId,address, &tempDevId);
                    if (rc!= GT_OK)
                        return rc;

                    /* The following determines the need for byte swapping for the device.
                       If we were unable to read the deviceID correctly, we should swap the
                       bytes*/
                    if((tempDevId & 0xFFFF) != MARVELL_VENDOR_ID)
                    {
                        ppConfigPtr->hwCtrl[portGroupId].doByteSwap = GT_TRUE;
                        tempDevId   = BYTESWAP_MAC(tempDevId);
                    }
                    break;

                case PRV_CPSS_DRV_DEV_ID_LOCATION_DEVICE_ADDR_0x0000004C_E:
                /* try to read register 0x50 to get the 0x11AB (MARVELL_VENDOR_ID) */

                    /* read VendorID */
                    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,VENDOR_ID_REG_ADDR, &readVendorId) ;
                    if (rc!= GT_OK)
                        return rc;

                    /* The following determines the need for byte swapping for the device.
                       If we were unable to read the deviceID correctly, we should swap the
                       bytes*/
                    if((readVendorId & 0xFFFF) != MARVELL_VENDOR_ID)
                    {
                        ppConfigPtr->hwCtrl[portGroupId].doByteSwap = GT_TRUE;
                        readVendorId   = BYTESWAP_MAC(readVendorId);

                        if((readVendorId & 0xFFFF) != MARVELL_VENDOR_ID)
                        {
                            /* not expected value for the register */
                            return GT_HW_ERROR;
                        }
                    }

                    /* now that the we tuned the doByteSwap we can read other registers */

                    /* read DeviceID */
                    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,DEVICE_ID_AND_REV_REG_ADDR, 4,16, &readDeviceId);
                    if (rc!= GT_OK)
                        return rc;

                    tempDevId = (readDeviceId << 16) | readVendorId ;

                    break;
                default:
                    return GT_NOT_SUPPORTED;
            }/*switch(ppInInfoPtr->devIdLocationType) */

            if(PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,PRV_CPSS_DRV_ERRATA_PCI_SLAVE_UNABLE_TO_ABORT_TRANSACTION_E) == GT_TRUE)
            {
                /* Initialize the PCI Timer and Retry register with 0xFF.   */
                /* Perform a read modify write on the register.             */
                rc = prvCpssDrvHwPpPortGroupReadInternalPciReg(devNum,portGroupId,0x104,&regData);
                if (rc!= GT_OK)
                    return rc;

                regData = (regData & 0xFFFF0000) | 0xFFFF;  /*set both timer values to maximum */

                rc = prvCpssDrvHwPpPortGroupWriteInternalPciReg(devNum,portGroupId,0x104,regData);
                if (rc!= GT_OK)
                    return rc;
            }
        }
        /* now doByteSwap has correct value */
    }/* PCI/PEX compatible */

    if(ppInInfoPtr->ppHAState == CPSS_SYS_HA_MODE_STANDBY_E)
    {
        /* get the info that the high availability holds for that device */
        tempDevId  = sysGenGlobalInfo.cpssPpHaDevData[devNum]->deviceId;
    }

    /* device type was stored in the internal DB */
    if (prvCpssDrvDebugDeviceId[devNum] != 0x0)
    {
        tempDevId = ((prvCpssDrvDebugDeviceId[devNum] << 16) | MARVELL_VENDOR_ID);
    }

    /* temporary setting that may change if the CPSS decide to address this
       device with other device type ! */
    ppConfigPtr->devType = tempDevId;

    /* Init the Gpp Isrs pointers array.    */
    for(ii = 0; ii < CPSS_EVENT_GPP_MAX_NUM_E; ii++)
    {
        ppConfigPtr->intCtrl.gppIsrFuncs[ii].cookie     = NULL;
        ppConfigPtr->intCtrl.gppIsrFuncs[ii].gppIsrPtr  = NULL;
    }

    return GT_OK;
}

/*******************************************************************************
* deviceTypeInfoGet
*
* DESCRIPTION:
*       This function sets the device info :
*       number of ports ,
*       deviceType ,
*       and return bmp of ports , for a given device.
*
* INPUTS:
*       devNum      - The Pp's device number.
*
* OUTPUTS:
*       existingPortsPtr - (pointer to)the BMP of existing ports
* RETURNS:
*       GT_OK on success,
*       GT_BAD_STATE - the 'multi port groups' support of the device has differences
*                      between the application and the cpssDrv DB
*       GT_NOT_FOUND if the given pciDevType is illegal.
*       GT_NOT_SUPPORTED - not properly supported device in DB
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS deviceTypeInfoGet
(
    IN  GT_U8       devNum,
    OUT CPSS_PORTS_BMP_STC  *existingPortsPtr
)
{
    GT_U32   ii;/* index in cpssSupportedTypes */
    GT_U32   jj;/* index in cpssSupportedTypes[ii].devTypeArray */
    GT_U32   kk;/* index in cpssSpecialDevicesBmp */
    GT_BOOL found = GT_FALSE;
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32  port;/*port iterator*/
    GT_U32  portInCurrentPortGroup;/*port number in current port group*/
    CPSS_PORTS_BMP_STC portsInPortGroupBmp;/*bitmap of the local ports of the port group */

    ppConfigPtr = prvCpssDrvPpConfig[devNum];

    /* get the info about our device */
    ii = 0;
    while(cpssSupportedTypes[ii].devFamily != END_OF_TABLE)
    {
        jj = 0;
        while(cpssSupportedTypes[ii].devTypeArray[jj] != END_OF_TABLE)
        {
            if(ppConfigPtr->devType == cpssSupportedTypes[ii].devTypeArray[jj])
            {
                found = GT_TRUE;
                break;
            }
            jj++;
        }

        if(found == GT_TRUE)
        {
            break;
        }
        ii++;
    }

    if(found == GT_FALSE)
    {
        return GT_NOT_FOUND;
    }

    /* do 'multi port groups' device info check */
    if(cpssSupportedTypes[ii].portGroupInfoPtr)
    {
        /*************************************************/
        /* the recognized device is 'multi port groups' device */
        /*************************************************/

        /* check that application stated that the device is 'multi port groups' device */
        if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.isMultiPortGroupDevice == GT_FALSE)
        {
            /* application not aware to the 'multi port groups' device ?! */
            return GT_BAD_STATE;
        }

        if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.numOfPortGroups >
            cpssSupportedTypes[ii].portGroupInfoPtr->numOfPortGroups)
        {
            /* application think more port groups then really exists */
            return GT_BAD_STATE;
        }

        if(cpssSupportedTypes[ii].activePortGroupsBmpPtr == NULL)
        {
            /* the DB of the device not hold the active port groups bitmap */
            /* internal DB error ! */
            return GT_BAD_STATE;
        }


        if(*cpssSupportedTypes[ii].activePortGroupsBmpPtr !=
           prvCpssDrvPpConfig[devNum]->portGroupsInfo.activePortGroupsBmp)
        {
            /* application think different active port groups then actually exists */
            return GT_BAD_STATE;
        }

    }
    else if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.isMultiPortGroupDevice == GT_TRUE)
    {
        /* application stated that this is 'multi port groups' device */
        /* but according to DB this is 'non multi port groups' device */
        return GT_BAD_STATE;
    }


    ppConfigPtr->numOfPorts = cpssSupportedTypes[ii].numOfPorts;
    ppConfigPtr->devFamily = cpssSupportedTypes[ii].devFamily;

    if(cpssSupportedTypes[ii].defaultPortsBmpPtr == NULL)
    {
        if(cpssSupportedTypes[ii].portGroupInfoPtr == NULL)
        {
            /* no special default for the ports BMP , use continuous ports bmp */
            if(cpssSupportedTypes[ii].numOfPorts < 32)
            {
                BMP_PORTS_LESS_32_MAC(existingPortsPtr,
                                cpssSupportedTypes[ii].numOfPorts);
            }
            else
            {
                BMP_PORTS_MORE_32_MAC(existingPortsPtr,
                                cpssSupportedTypes[ii].numOfPorts);
            }
        }
        else
        {
            /* 'multi port groups' device */
            /* by default we build it's bmp according to the port groups info */

            portInCurrentPortGroup = 0;
            portsInPortGroupBmp.ports[0] = cpssSupportedTypes[ii].portGroupInfoPtr->portsBmp;
            portsInPortGroupBmp.ports[1] = 0;
            existingPortsPtr->ports[0] = 0;
            existingPortsPtr->ports[1] = 0;

            for(port = 0 ; port < cpssSupportedTypes[ii].numOfPorts ; port++,portInCurrentPortGroup++)
            {
                if(portInCurrentPortGroup == cpssSupportedTypes[ii].portGroupInfoPtr->maxNumPorts)
                {
                    portInCurrentPortGroup = 0;
                }

                if(ppConfigPtr->hwCtrl[((port >> 4) & 0x3)/*portGroupId*/].baseAddr == CPSS_PARAM_NOT_USED_CNS)
                {
                    /* non active port group */
                    continue;
                }

                if(CPSS_PORTS_BMP_IS_PORT_SET_MAC((&portsInPortGroupBmp),portInCurrentPortGroup))
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(existingPortsPtr,port);
                }
            }
        }
    }
    else
    {
        /* use special default for the ports BMP */
        *existingPortsPtr = *cpssSupportedTypes[ii].defaultPortsBmpPtr;
    }

    /****************************************************************/
    /* add here specific devices BMP of ports that are not standard */
    /****************************************************************/
    kk = 0;
    while(cpssSpecialDevicesBmp[kk].devType != END_OF_TABLE)
    {
        if(cpssSpecialDevicesBmp[kk].devType == ppConfigPtr->devType)
        {
            *existingPortsPtr = cpssSpecialDevicesBmp[kk].existingPorts;
            break;
        }
        kk++;
    }

    /* clear from the existing ports the ports beyond the 'Number of ports' */
    if(cpssSupportedTypes[ii].numOfPorts < 32)
    {
        existingPortsPtr->ports[0] &= BMP_CONTINUES_PORTS_MAC(cpssSupportedTypes[ii].numOfPorts);
        existingPortsPtr->ports[1] = 0;/* clear bmp[1] */
    }
    else
    {
        existingPortsPtr->ports[1] &= BMP_CONTINUES_PORTS_MAC(cpssSupportedTypes[ii].numOfPorts-32);
    }

    return GT_OK;
}

/*******************************************************************************
* phase1Part4Init
*
* DESCRIPTION:
*       This function is called by prvCpssDrvHwPpPhase1Init()
*       This function do PCI and DMA calibration .
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       revisionIdPtr - (pointer to)the revision of the PP
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS phase1Part4Init
(
    IN  GT_U8       devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr,
    OUT GT_U8       *revisionIdPtr
)
{
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32  tempData = 0;/* data from the register */
    GT_STATUS rc = GT_OK;

    ppConfigPtr = prvCpssDrvPpConfig[devNum];

    /* set Cheetah's PCI settings */
    if((PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum))
               &&
       (ppConfigPtr->devFamily == CPSS_PP_FAMILY_CHEETAH_E ||
        ppConfigPtr->devFamily == CPSS_PP_FAMILY_CHEETAH2_E))
    {
         /* PCI Pad Calibration Configuration -
         Bits [3:0, NDrive => 0x0 (Fingers) , The value set to l_io_pad_n_drive.
         Bits [7:4], Pdrive => 0x4 (Fingers), The value set to l_io_pad_p_drive.
         */
         prvCpssDrvHwPpWriteInternalPciReg(devNum,DX_PCI_PAD_CALIB_REG_ADDR,0x40);

         /* PCI Status and Command Register:
            Bit 0, IOSpaceEn, => Enable (Internal)
            Bit 1, MemorySpaceEn, => Enable
            Bit 2, BusMasterEn, => Enable
            Bit 4, MemWrInv, => Enable  */
         prvCpssDrvHwPpWriteInternalPciReg(devNum,DX_PCI_STATUS_CMD_REG_ADDR,0x02000017);
    }

    /* Set the Rx / Tx glue byte swapping configuration     */
    if (PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum))
    {
        /* SDMA LE/BE should works the same since the CPU get the data as raw
           byte */
        /* Enable byte swap, Enable word swap  */
        prvCpssDrvHwPpSetRegField(devNum,SDMA_CFG_REG_ADDR,6,2,3);

        prvCpssDrvHwPpSetRegField(devNum,SDMA_CFG_REG_ADDR,23,2,0);

#ifdef ASIC_SIMULATION
        /*
            this section is needed for the distributed simulation.
            when the read/write DMA from Asic are done via socket.

            and because both descriptors and frames are on the DMA memory,
            we need to set the way that Device put frame on the socket.

            because the DMA parser on the CPU side will set the data from the
            socket the same way for descriptors (that are little endian format)
            and the frames .
        */
        if(sasicgSimulationRoleIsApplication == GT_TRUE)
        {
            GT_U32  swapBytes;
            /* override the setting of SDMA swap/not swap bytes */
            if(BIG_ENDIAN_E == checkHostEndian())
            {
                /* when PP read/write frame form/to DMA it should do it non-swapped
                    I.E the frame should be placed on socket 'Big endean'
                 */
                swapBytes = 0;
            }
            else
            {
                /* when PP read/write frame form/to DMA it should do it swapped
                    I.E the frame should be placed on socket 'Little endean'
                 */
                swapBytes = 1;
            }

            prvCpssDrvHwPpSetRegField(devNum,SDMA_CFG_REG_ADDR, 6,1,swapBytes);
            prvCpssDrvHwPpSetRegField(devNum,SDMA_CFG_REG_ADDR,23,1,swapBytes);
        }
#endif /*ASIC_SIMULATION*/
    }

    
#ifndef __AX_PLATFORM__
	rc = prvCpssDrvHwPpSetRegField(devNum, SDMA_CFG_REG_ADDR,8,8,0xFF);
    #if 0   /*xcat debug 20110114, osPrintf() kill npd.*/
	osPrintf("phase1Part4Init::write 0xff reg 0x2800 bit[15:8] return %d\n",rc);
    #endif
    printf("%s %s %d: osPrintf() kill npd.\n", __FILE__, __FUNCTION__, __LINE__);/*xcat debug*/
#endif

    /* get the revision id of the PP */
    if(!CPSS_IS_EXMX_FAMILY_MAC(ppConfigPtr->devFamily))
    {
        /* RevisionID read */
        prvCpssDrvHwPpGetRegField(devNum,
                        DEVICE_ID_AND_REV_REG_ADDR, 0, 4, &tempData) ;
    }
    else
    {
        /* get revision id from the PCI registers */
        prvCpssDrvHwPpReadInternalPciReg(devNum,
                        EXMX_PCI_CLASS_CODE_AND_REV_REG_ADDR,&tempData);
        tempData &= 0xFF;/* 8 bits of the register */
    }

    if(ppInInfoPtr->ppHAState == CPSS_SYS_HA_MODE_ACTIVE_E)
    {
        *revisionIdPtr = (GT_U8)tempData;
    }
    else
    {
        *revisionIdPtr = sysGenGlobalInfo.cpssPpHaDevData[devNum]->revision;
    }

    /* !!! PATCH:
       TwistD-XG behaves as TwistD revision 1, but the value written in PCI
       status register is 0. Thus, to  initialize correctly functionalities
       such as L2 learning, FDB hash function and so on, the revision number
       of TwistD-XG must not be 0.
     */
    if((ppConfigPtr->devFamily == CPSS_PP_FAMILY_TWISTD_E) &&/* Twist-D*/
       (ppConfigPtr->numOfPorts == 1) &&                    /* XG */
       (*revisionIdPtr == 0))                               /* rev 0 */
    {
        *revisionIdPtr = 1;                               /* modify to rev 1 */
    }

    return GT_OK;
}
/*******************************************************************************
* prvCpssDrvHwPpPhase1Init
*
* DESCRIPTION:
*       This function is called by cpssExMxHwPpPhase1Init() or other
*       cpss "phase 1" family functions, in order to enable PP Hw access,
*       the device number provided to this function may
*       be changed when calling prvCpssDrvPpHwPhase2Init().
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       ppOutInfoPtr  - (pointer to)the info that driver return to caller.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       function also bound to the "pci-standby" for HA
*
*******************************************************************************/
static GT_STATUS driverDxExMxHwPpPhase1Init
(
    IN  GT_U8                devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr,
    OUT PRV_CPSS_DRV_PP_PHASE_1_OUTPUT_INFO_STC *ppOutInfoPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32      portGroupId;/* the port group Id */

    /* the function bind the PCI/SMI/TWSI driver functions to the driver object
       functions for this device.*/
    rc = phase1Part1Init(devNum,ppInInfoPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* NOTE :
        since the function of phase1Part2Init(...) will allocate dynamic memory
        we need to free it and restore NULL state if error occur in this current
        function driverDxExMxHwPpPhase1Init(...) after calling
        phase1Part2Init(...)

        see the use of label exit_cleanly_lbl
    */


   /* This function allocate the memory for the DB of the driver for this
      device , and set some of it's values.*/
    rc = phase1Part2Init(devNum,ppInInfoPtr);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    if (ppInInfoPtr->mngInterfaceType != CPSS_CHANNEL_PEX_E)
    {
        /* relevant only for PCI devices */
        /* all PCI devices need it */
        PRV_CPSS_DRV_ERRATA_SET_MAC(
            devNum,
            PRV_CPSS_DRV_ERRATA_PCI_SLAVE_UNABLE_TO_ABORT_TRANSACTION_E);
    }


    ppConfigPtr = prvCpssDrvPpConfig[devNum];

    ppConfigPtr->portGroupsInfo.isMultiPortGroupDevice =
        (ppInInfoPtr->numOfPortGroups == 1)
            ? GT_FALSE : GT_TRUE;

    for(portGroupId = 0; portGroupId < ppInInfoPtr->numOfPortGroups; portGroupId++)
    {
        if(ppConfigPtr->hwCtrl[portGroupId].baseAddr == CPSS_PARAM_NOT_USED_CNS)
        {
            /* non active port group */
            continue;
        }

        /* This function call driverHwCntlInit to initialize the HW control.
           The function get the deviceType from HW , revision id , doByteSwap */
        rc = phase1Part3Init(devNum,portGroupId,ppInInfoPtr);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }

    /* starting from this point the DB of : prvCpssDrvPpConfig[devNum]->portGroupsInfo
       is ready because initialized in the prvCpssDrvHwCntlInit(..) that called
       from phase1Part3Init(..)

       so we can start use the macros for loops on port groups (supports also 'non multi port group' device):
       PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC
       PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC
    */


    /* This function sets the device info : number of ports , deviceType ,
      and return bmp of ports , for a given device. */
    rc = deviceTypeInfoGet(devNum,
                           &ppOutInfoPtr->existingPorts);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    ppOutInfoPtr->devType = ppConfigPtr->devType;
    ppOutInfoPtr->numOfPorts = ppConfigPtr->numOfPorts;
    ppOutInfoPtr->devFamily = ppConfigPtr->devFamily;

    /*only summary interrupts are unmasked by default */
    /* according to the interrupt scan tree. */
    ppConfigPtr->intCtrl.needInterruptMaskDefaultValueInit = GT_TRUE;
            /* bind the ISR callback routine done before signalling app. */
    ppConfigPtr->intCtrl.bindTheIsrCbRoutineToFdbTrigEnded = GT_FALSE;

    switch(ppOutInfoPtr->devFamily)
    {
        case CPSS_PP_FAMILY_SALSA_E:
            #ifdef SAL_FAMILY
            prvCpssDrvPpIntDefSalsaInit();

            ppConfigPtr->intCtrl.intSumMaskRegAddr = 0;/* relevant only for PCI !!! */
            break;
            #else
            rc = GT_NOT_SUPPORTED;
            goto exit_cleanly_lbl;
            #endif

        case CPSS_PP_FAMILY_CHEETAH_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the cheetah devices.*/
                prvCpssDrvPpIntDefCheetahInit();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_CHEETAH2_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the cheetah-2 devices.*/
                prvCpssDrvPpIntDefCheetah2Init();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_CHEETAH3_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the cheetah-3 devices.*/
                prvCpssDrvPpIntDefCheetah3Init();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the xCat devices.*/
                prvCpssDrvPpIntDefDxChXcatInit();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */

        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the xCat2 devices.*/
                prvCpssDrvPpIntDefDxChXcat2Init();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_DXCH_LION_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the Lion devices.*/
                prvCpssDrvPpIntDefDxChLionInit();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */

            ppConfigPtr->hwCtrl[portGroupId].dummyReadAfterWriteRegAddr = 0;/*not used when
            not need PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E*/

            /*only summary interrupts are unmasked by default */
            /* according to the interrupt scan tree. */
            ppConfigPtr->intCtrl.needInterruptMaskDefaultValueInit = GT_FALSE;

            if (ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
                /* xCat2 does not have PEX switch interface */
                ppConfigPtr->intCtrl.intSumMaskRegAddr = 0xFFFFFFFF;
            }
            else if (ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            {
                /* Cheetah3 */
                ppConfigPtr->intCtrl.intSumMaskRegAddr = 0x00071910;/* PEX */
            }
            else if (ppOutInfoPtr->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
                /* xCat.. */
                ppConfigPtr->intCtrl.intSumMaskRegAddr = 0x000F1910;/* PEX */
            }
            else
            {
                /* Cheetah and Cheetah2 */
                ppConfigPtr->intCtrl.intSumMaskRegAddr = 0x118; /* PCI */
            }
            break;

        case CPSS_PP_FAMILY_TWISTD_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTD_E)
            {
            #if defined(EX_FAMILY) || defined(MX_FAMILY)
                PRV_CPSS_DRV_ERRATA_SET_MAC(devNum,PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E);
                /*Interrupts initialization for the Twist-D devices.*/
                prvCpssDrvPpIntDefTwistDInit();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_TWISTC_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTC_E)
            {
            #if defined(EX_FAMILY) || defined(MX_FAMILY)
                PRV_CPSS_DRV_ERRATA_SET_MAC(devNum,PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E);
                /*Interrupts initialization for the Twist-C devices.*/
                prvCpssDrvPpIntDefTwistCInit(GT_TRUE);
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_TIGER_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_TIGER_E)
            {
            #ifdef TG_FAMILY
                /*Interrupts initialization for the Tiger devices.*/
                prvCpssDrvPpIntDefTigerInit();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_SAMBA_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_SAMBA_E)
            {
                PRV_CPSS_DRV_ERRATA_SET_MAC(devNum,PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E);
                /*prvCpssDrvPpIntDefSambaInit();*/
            }

            ppConfigPtr->hwCtrl[portGroupId].dummyReadAfterWriteRegAddr = 0x0380001C;/*relevant to PCI */

            ppConfigPtr->intCtrl.intSumMaskRegAddr = 0x118;
            break;
        case CPSS_PP_FAMILY_PUMA_E:
        #ifdef EXMXPM_FAMILY
            /*Interrupts initialization for the Puma devices.*/
            prvCpssDrvPpIntDefPumaInit();

            ppConfigPtr->hwCtrl[portGroupId].dummyReadAfterWriteRegAddr = 0;/*not used when
            not need PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E*/

            /*only summary interrupts are unmasked by default */
            /* according to the interrupt scan tree. */
            ppConfigPtr->intCtrl.needInterruptMaskDefaultValueInit = GT_FALSE;
            /* bind the ISR callback routine done before signalling app. */
            ppConfigPtr->intCtrl.intSumMaskRegAddr = 0x00071910; /*regsAddr.PEX.PEXInterrupt.PEXInterruptMask*/
            break;
        #else
            rc = GT_NOT_SUPPORTED;
            goto exit_cleanly_lbl;
        #endif
        default:
            rc = GT_NOT_SUPPORTED;
            goto exit_cleanly_lbl;
    }
    prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].doReadAfterWrite =
        PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E);

    if (ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
#ifdef PRV_CPSS_DRV_ERRATA_REFERENCE_CNS  /* macro never defined --> for references purpose only  */
    PRV_CPSS_DXCH_LION_RM_SDMA_ACTIVATION_WA_E /*Lion RM#2701*/
#endif /*PRV_CPSS_DRV_ERRATA_REFERENCE_CNS*/
        /* Lion RM#2701: SDMA activation */
        /* the code must be before calling phase1Part4Init(...) because attempt
           to access register 0x2800 will cause the PEX to hang */
        rc = prvCpssDrvHwPpSetRegField(devNum,0x58,20,1,1);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }

    rc = phase1Part4Init(devNum,ppInInfoPtr,&ppOutInfoPtr->revision);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

exit_cleanly_lbl:
    if(rc != GT_OK)
    {
        if(rc != GT_BAD_STATE && /* another device is in this memory --
                                    don't free the memory */
           prvCpssDrvPpConfig[devNum])
        {
            /* release the allocation */
            cpssOsFree(prvCpssDrvPpConfig[devNum]);
            prvCpssDrvPpConfig[devNum] = NULL;
        }
    }

    return rc;
}

/*******************************************************************************
* driverDxExMxHwPpRamBurstConfigSet
*
* DESCRIPTION:
*       Sets the Ram burst information for a given device.
*
* INPUTS:
*       devNum          - The Pp's device number.
*       ramBurstInfoPtr - A list of Ram burst information for this device.
*       burstInfoLen    - Number of valid entries in ramBurstInfo.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_ALREADY_EXIST - the ram already initialized
*
* COMMENTS:
*
*       function also bound to the "pci-standby" for HA
*
*******************************************************************************/
static GT_STATUS driverDxExMxHwPpRamBurstConfigSet
(
    IN  GT_U8               devNum,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfoPtr,
    IN  GT_U8               burstInfoLen
)
{
    GT_U32  portGroupId;

    if(CPSS_IS_DXCH_FAMILY_MAC(prvCpssDrvPpConfig[devNum]->devFamily) ||
       CPSS_IS_DXSAL_FAMILY_MAC(prvCpssDrvPpConfig[devNum]->devFamily) )
    {
        /* no more to do */
        return GT_OK;
    }

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo != NULL ||
           prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfoLen != 0 )
        {
            return GT_BAD_PARAM;
        }

        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo =
            cpssOsMalloc(sizeof(PRV_CPSS_DRV_RAM_BURST_INFO_STC)* burstInfoLen);
        if(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        cpssOsMemCpy(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo,ramBurstInfoPtr,
                 sizeof(PRV_CPSS_DRV_RAM_BURST_INFO_STC) * burstInfoLen);

        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfoLen = burstInfoLen;
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* driverExMxDxHwPpHaModeSet
*
* DESCRIPTION:
*       function to set CPU High Availability mode of operation.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       devNum       - the device number.
*       mode - active or standby
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS driverExMxDxHwPpHaModeSet
(
    IN  GT_U8   devNum,
    IN  CPSS_SYS_HA_MODE_ENT mode
)
{
    GT_STATUS   rc;
    GT_U32  tempDevId;/* the device type built from device id , vendor id */
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32      portGroupId;/* the port group Id */

    if(0 == PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum))
    {
        /* nothing to update -- not supported option yet */
        return GT_OK;
    }

    ppConfigPtr =  prvCpssDrvPpConfig[devNum];

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(mode == CPSS_SYS_HA_MODE_ACTIVE_E)
        {
            /* update the pointer to the functions object -- to use the PCI */
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciPtr;

            /* we need to set the doByteSwap for the device */
            tempDevId = *((volatile GT_U32*)ppConfigPtr->hwCtrl[portGroupId].internalPciBase);

            /* update the byte swap */
            ppConfigPtr->hwCtrl[portGroupId].doByteSwap =
                ((tempDevId & 0xFFFF) != MARVELL_VENDOR_ID) ? GT_TRUE : GT_FALSE;
        }
        else
        {
            /* update the pointer to the functions object -- to emulate the PCI */
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciHaStandbyPtr;
        }

        /* we need to re-initialize the HW control driver --
           because we changed the OBJ of the prvCpssDrvHwCntlInit */
        /* Init Hw Cntl parameters. Now we know the value of doByteSwap */
        rc = prvCpssDrvHwCntlInit(devNum,portGroupId,
                         ppConfigPtr->hwCtrl[portGroupId].baseAddr,
                         ppConfigPtr->hwCtrl[portGroupId].internalPciBase,
                         ppConfigPtr->hwCtrl[portGroupId].doByteSwap, /* doByteSwap */
                         GT_FALSE);/* isDiag */
        if (rc!= GT_OK)
            return rc;
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvDxExMxInitObject
*
* DESCRIPTION:
*      This function creates and initializes ExMxDx device driver object
*
* INPUTS:
*       devNum          - The PP device number to read from.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_ALREADY_EXIST - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvDxExMxInitObject
(
     IN GT_U8   devNum
)
{
    GT_STATUS rc;

    /****************************************************/
    /* bind the generic part to the ExMxDx generic part */
    /****************************************************/
    prvCpssDrvPpObjConfig[devNum]->genPtr = prvCpssDrvGenExMxDxObjPtr;
    /* the bus part will be initialized in "phase 1" -
       when we get the parameter */
    prvCpssDrvPpObjConfig[devNum]->busPtr = NULL;

    /* driver object initialization */
    prvCpssDrvGenExMxDxObjPtr->drvHwPpCfgPhase1Init = driverDxExMxHwPpPhase1Init;
    prvCpssDrvGenExMxDxObjPtr->drvHwPpSetRamBurstConfig = driverDxExMxHwPpRamBurstConfigSet;
    prvCpssDrvGenExMxDxObjPtr->drvHwPpHaModeSet = driverExMxDxHwPpHaModeSet;
    prvCpssDrvGenExMxDxObjPtr->drvHwPpInterruptsTreeGet = prvCpssDrvExMxDxHwPpInterruptsTreeGet;
    rc = prvCpssDrvDxExMxEventsInitObject();

    if(rc != GT_OK)
        return rc;

    rc = prvCpssDrvDxExMxIntInitObject();

    return rc;
}


