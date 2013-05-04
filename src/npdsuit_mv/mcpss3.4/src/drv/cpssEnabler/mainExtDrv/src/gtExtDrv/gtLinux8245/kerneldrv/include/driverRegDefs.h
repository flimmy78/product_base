/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
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
* gtDriverDefs.h
*
* DESCRIPTION:
*       Includes driver level structs needed for implementing HW related
*       functions, and interrupt handling. And common definitions for driver
*       level communication.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __gtDriverStructsh
#define __gtDriverStructsh

#include "drvPpRegs.h"
#include "presteraIrq.h"
#include "eventsCauseTypes.h"
/*
 * Typedef: enumeration GT_DEVICE
 *
 * Description: Defines the different device type that may exist in system.
 *
 * Fields:
 *  Tango group:
 *      GT_98MX620  -   10 * 1G
 *      GT_98MX630  -   1 * 10G
 *      GT_98EX120  -   10 * 1G
 *      GT_98EX130  -   1 * 10G
 *
 *  Twist group:
 *      GT_98MX610B     - 48 + 4 DSSMII full feature
 *      GT_98MX620B     - 10 * 1G full feature
 *      GT_98MX610BS    - 48 + 4 SSMII full feature
 *      GT_98EX110BS    - 48 + 4 SSMII no external rams
 *      GT_98EX111BS    - 48 + 4 SSMII no ns, ws
 *      GT_98EX112BS    - 48 + 4 SSMII no ws, no df
 *      GT_98EX110B     - 48 + 4 DSSMII no external rams
 *      GT_98EX111B     - 48 + 4 DSSMII no ns, ws
 *      GT_98EX112B     - 48 + 4 DSSMII no ws, no df
 *      GT_98EX120B     - 12G no external rams
 *      GT_98EX120B_    - 12G no external rams (TC enabled)
 *      GT_98EX121B     - 12G no ns, no ws
 *      GT_98EX122B     - 12G no ws, no df
 *      GT_98EX128B     - 12G  with extrenal memories
 *      GT_98EX129B     - 12G  with ws, and fd
 *
 *  Lambada group: OBSOLETE
 *      GT_98DX240  -  24 * 1G DX feature set deprecated
 *      GT_98DX245  -  24 * 1G NO Management
 *      GT_98EX242  -  24 * 1G
 *
 *  Twist D group:
 *      GT_98EX100D  - 24+4
 *      GT_98EX110D  - 48+4 SSMII
 *      GT_98EX115D  - 48+4 SSMII with external narrow sram
 *      GT_98EX110DS - 48+4 DSSMII
 *      GT_98EX115DS - 48+4 DSSMII with external narrow sram
 *      GT_98EX120D  - 12G
 *      GT_98EX125D  - 12G with external narrow sram
 *      GT_98EX130D  - 1 * 10G (XG)
 *      GT_98EX135D  - 1 * 10G (XG) with external sram
 *
 *  Samba group:
 *      GT_98MX615   - 48+4 SSMII   with external narrow sram
 *      GT_98MX625   - 12G          with external narrow sram
 *      GT_98MX635   - 1 * 10G (XG) with external sram
 *
 *
 *  Salsa group:
 *      GT_98DX240   - 24G  Mngment Dev
 *      GT_98DX160   - 16G  Mngment Dev
 *      GT_98DX120   - 12G  Mngment Dev
 *      GT_98DX241   - 24G  NO Mngment Dev
 *      GT_98DX161   - 16G  NO Mngment Dev
 *      GT_98DX121   - 12G  NO Mngment Dev
 *
 *  Cheetah group:
 *
 *     GT_98DX270   - 24G + 3 10G MngmentDev
 *     GT_98DX260   - 24G + 2 10G MngmentDev
 *     GT_98DX250   - 24G         MngmentDev
 *
 *  Tiger group:
 *     GT_98EX116   - 48+4 SSMII with external narrow sram
 *     GT_98EX126   - 12G with external narrow sram
 *     GT_98EX136   - 1X10G with external narrow sram
 * 
 */
typedef enum
{
    /*GT_98MX620A = 0x000011AB,
    GT_98MX630A = 0x001011AB,
    GT_98EX120A = 0x000B11AB,
    GT_98EX130A = 0x001B11AB,*/

    GT_98MX610B  = 0x007011AB,
    GT_98MX620B  = 0x004011AB,
    GT_98MX610BS = 0x006011AB,
    GT_98EX110BS = 0x006711AB,
    GT_98EX111BS = 0x006311AB,
    GT_98EX112BS = 0x006511AB,
    GT_98EX110B  = 0x007711AB,
    GT_98EX111B  = 0x007311AB,
    GT_98EX112B  = 0x007511AB,
    GT_98EX120B  = 0x005711AB,
    GT_98EX120B_ = 0x005611AB,
    GT_98EX121B  = 0x005311AB,
    GT_98EX122B  = 0x005511AB,
    GT_98EX128B  = 0x005011AB,
    GT_98EX129B  = 0x005211AB,

    /*
    GT_98DX245   = 0x008E11AB,
    GT_98EX242   = 0x008111AB,
    */
    GT_98EX100D  = 0x00EF11AB,    
    GT_98EX110D  = 0x00E711AB,
    GT_98EX115D  = 0x00E111AB,
    GT_98EX110DS = 0x00F711AB,
    GT_98EX115DS = 0x00F111AB,
    GT_98EX120D  = 0x00D711AB,
    GT_98EX125D  = 0x00D111AB,
    GT_98EX130D  = 0x01D711AB,
    GT_98EX135D  = 0x01D111AB,

    /* samba devices */
    GT_98MX615   = 0x01A011AB,
    GT_98MX625   = 0x018011AB,
    GT_98MX625V0 = 0x019011AB, 
    GT_98MX635   = 0x01D011AB,
    /* Salsa devices  */
    GT_98DX240   = 0x0D0411AB,
    GT_98DX160   = 0x0D0511AB,
    GT_98DX120   = 0x0D0611AB,
    GT_98DX241   = 0x0D0011AB,
    GT_98DX161   = 0x0D0111AB,
    GT_98DX121   = 0x0D0211AB,
    /* Salsa 2 devices  */
    GT_98DX240_2 = 0x0D1411AB,
    GT_98DX160_2 = 0x0D1511AB,
    GT_98DX120_2 = 0x0D1611AB,
    GT_98DX241_2 = 0x0D1011AB,
    GT_98DX161_2 = 0x0D1111AB,
    GT_98DX121_2 = 0x0D1211AB,
    /* Sapphire device */
    GT_88E6183   = 0x01A311AB,
    /* Ruby device */
    GT_88E6093   = 0x009311AB,
    /* Cheetah devices */
    GT_98DX270 = 0xD80411AB,
    GT_98DX260 = 0xD80C11AB,
    GT_98DX250 = 0xD81411AB,
    /* Tiger devices */
    GT_98EX116 = 0x012111AB,
    GT_98EX126 = 0x011111AB,
    GT_98EX136 = 0x015111AB,
    
    GT_98EX116DI = 0x012511AB,
    GT_98EX126DI = 0x011511AB,
    GT_98EX136DI = 0x015511AB
    
}GT_DEVICE;

/* Maximum interrupts number in system per PP   */
#define     PP_INTERRUPT_MAX_NUM        GT_NUM_OF_INT_CAUSE

/* Number of interrupt queues */
#define     NUM_OF_INT_QUEUES           8

/* The interrupt-queues default policy for all queues */
#define     INT_QUEUES_POLICY           GT_WRR_PRIO
#define     INT_QUEUES_WEIGHT           10


/*
 * Typedef: struct GT_GPP_INT_INFO
 *
 * Description: Holds Gpp interrupt binding information.
 *
 * Fields:
 *      gppIsrPtr   - The function pointer to be invoked in Gpp interrupt
 *                    reception.
 *      cookie      - A user defined parameter to be passed to the isr, in
 *                    addition to the gppId parameter.
 *
 */
typedef struct
{
    ISR_FUNCP    gppIsrPtr;
    void        *cookie;
}GPP_INT_INFO;


/*
 * Typedef: struct STRUCT_INT_CTRL
 *
 * Description: Includes interrupt control and handling data.
 *
 * Fields:
 *      intNodesPool    - An array containing all interrupt nodes to be enqueued
 *                        / dequeued in the interrupt queues.
 *      intCauseType    - Interrupt cause registers type.
 *      isrFuncPtr      - A pointer to an ISR to be called when this device
 *                        should be scanned for interrupts.
 *      intScanRoot     - The interrupt information hierarchy tree root.
 *      intMaskShadow   - Holds the interrupt mask registers shadow.
 *      accessB4StartInit - Whether the coresponding interrupt mask register
 *                          may be accessed before start init or not.
 *      numOfIntBits    - Number of interrupt bits defined for this device.
 *      gppIsrFunc      - Array of pointers to the different Gpps interrupt
 *                        service routines.
 *      intVecNum       - The interrupt vector this device is connected throw.
 *      isrConnectionId - Id for access to the isr connection table.
 *      intTaskParams   - Interrupt task parameters.
 *
 */
typedef struct
{
    INT_NODE            *intNodesPool;
    GT_INT_CAUSE_TYPE   intCauseType;
    ISR_FUNCP           isrFuncPtr;
    INTERRUPT_SCAN      *intScanRoot;

    unsigned int        intMaskShadow[PP_INTERRUPT_MAX_NUM / 32];
    INT_BOOL            accessB4StartInit[PP_INTERRUPT_MAX_NUM / 32];
    unsigned int        numOfIntBits;

    GPP_INT_INFO        gppIsrFuncs[GT_GPP_MAX_NUM];

    unsigned int        intVecNum;
    unsigned int        isrConnectionId;
    INT_TASK_PARAMS     *intTaskParams;
}STRUCT_INT_CTRL, *STRUCT_INT_CTRL_PTR;

/*
 * Typedef: struct GT_RAM_BURST_INFO
 *
 * Description: Holds information regarding the bursts that should be performed
 *              when access the different Pp's Rams.
 *
 * Fields:
 *      ramBase         - Base address of the Ram.
 *      ramBaseMask     - Mask to be used to check if a given address falls into
 *                        this Ram.
 *      burstSize       - Number of words to be written in each burst.
 *      addrResolution  - Resolution of the addresses to be used when addressing
 *                        this Ram (see comment).
 *
 * Comment:
 *       1.  addrResolution: This is the parameter by which the Ram offset is
 *                           multiplied when accessing it by burst, if this
 *                           is different than 1, then a block write / read from
 *                           Ram should be devided into several bursts in size
 *                           'burstSize'.
 *
 */
typedef struct
{
    unsigned int ramBase;
    unsigned int ramBaseMask;
    unsigned char burstSize;
    unsigned char addrResolution;
}RAM_BURST_INFO;


/* Maximal size of a memory block to be read / written in   */
/* burst (in 4-bytes words).                                */
#define MAX_BURST_BLOCK_SIZE    256
#define MAX_BURST_EXTRA_ALLIGN  16


/*
 * typedef: struct STRUCT_PP_HW
 *
 * Description:
 *
 * Hardware access related data - For performing Read/Write acceses to PP's .
 *
 * Fields:
 *
 *      baseAddr            - Base address to which PP is mapped on PCI.
 *                            in case of SMI it would be smiIdSel.
 *      addrCompletShadow[4]- Shadow for the address completion register
 *                            index 0 if fixed 0, index 1 - used by ISR
 *                            functions
 *      hwComplSem          - binary semaphor to enable mutual exclusion use of
 *                            address Completion register use.
 *      compIdx             - last Completion Index used (index 0,1 are
 *                            reserved)
 *      internalPciBase     - Base address to which the internal pci registers
 *                            are mapped to.
 *      ramBurstInfo        - Holds the ram burst parameters for this device.
 *      ramBurstInfoLen     - Number of valid entries in ramBurstInfo[].
 *      lastWriteAddr       - Address of the last 2 addresses that a write
 *                            operation was performed on.
 *
 * Comments:
 *
 */
typedef struct
{
    unsigned int  baseAddr;
    unsigned char addrCompletShadow[4];
    void *        hwComplSem;
    unsigned char compIdx;
    unsigned int  internalPciBase;
    RAM_BURST_INFO   *ramBurstInfo;
    unsigned char  ramBurstInfoLen;
    unsigned char  lastWriteAddr[2];

}STRUCT_PP_HW,*STRUCT_PP_HW_PTR;

/*
 * Typedef: struct DRIVER_PP_CONFIG
 *
 * Description: Includes all driver level configuration info, for HW access,
 *              interrupt handling.
 *
 * Fields:
 *      devNum     - The PP's device number.
 *      devType    - The PP's device type.
 *      numOfPorts - number of ports in device
 *      regsId  - Identifies the register's family to which this device belongs
 *                to, two devices with the same registers set addresses, will
 *                have the same regsId field.
 *      regsAddr - A struct including PP registers addresses for register access
 *                 to different types of PPs.
 *      hwCtrl  - A struct containing control fields for the hw access
 *                functions.
 *      intCtrl - Interrupt control struct.
 *
 */
typedef struct
{
    unsigned char devNum;
    GT_DEVICE     devType;
    unsigned char numOfPorts;
    
    REGS_FAMILY      regsId;
    PP_REGS_ADDR_PTR regsAddr;
    
    STRUCT_PP_HW  hwCtrl;
    
    STRUCT_INT_CTRL intCtrl;

}DRIVER_PP_CONFIG, *DRIVER_PP_CONFIG_PTR;

#endif /* __gtDriverStructsh */


