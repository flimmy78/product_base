/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntInterrupts.c
*
* DESCRIPTION:
*       This file includes initialization function for the interrupts module,
*       and low level interrupt handling (interrupt bits scanning).
*
* FILE REVISION NUMBER:
*       $Revision: 1.2 $
*
*******************************************************************************/
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>

/*******************************************************************************
* internal definitions
*******************************************************************************/

#ifdef DRV_INTERRUPTS_DBG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

#if 0   /* luoxun -- cpss1.3 */
extern unsigned int  cheetah2MaskRegMapArr[];
extern unsigned int  cheetah2MaskRegMapArrLen;
#endif
extern unsigned int  cheetah2MaskRegMapArr[];
extern unsigned int  cheetah2MaskRegMapArrLen;

extern unsigned int  cheetah2IntCauseRegMapArr[];
extern unsigned int  cheetah2IntCauseRegMapArrLen;

extern int osPrintf(const char * format,...);

unsigned int prvCpssDrvInterruptCuaseRegGet(unsigned int maskRegAddr)
{
	int i = 0,found = 0;
	unsigned int maskRegArrLen = cheetah2MaskRegMapArrLen;

	for(;i<maskRegArrLen;i++)
	{
		if(maskRegAddr == cheetah2MaskRegMapArr[i])
		{
			found = 1;
			break;
		}
	}
	if(1 == found)
	{
		#ifdef AX_FULL_DEBUG
		osPrintf("find INT cause reg %#0x by mask %#0x\r\n",maskRegAddr,cheetah2IntCauseRegMapArr[i]);
		#endif
		return cheetah2IntCauseRegMapArr[i];
	}
}

/*******************************************************************************
 * External usage variables
 ******************************************************************************/

/*******************************************************************************
* prvCpssDrvInterruptMaskSet
*
* DESCRIPTION:
*       This function masks/unmasks a given interrupt bit in the relevant
*       interrupt mask register.
*
* INPUTS:
*       evNode  - The interrupt node representing the interrupt to be
*                 unmasked.
*       operation - The operation to perform, mask or unmask the interrupt
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptMaskSet
(
    IN PRV_CPSS_DRV_EV_REQ_NODE_STC              *evNode,
    IN CPSS_EVENT_MASK_SET_ENT     operation
)
{
    GT_U32  maskIdx;
    GT_U32  *intMaskShadow;
    GT_U8   devNum = evNode->devNum;  /* devNum */
    GT_U32  portGroupId = evNode->portGroupId;  /* portGroupId */

    intMaskShadow = prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intMaskShadow;

    maskIdx = evNode->intCause >> 5;

    if (CPSS_EVENT_MASK_E == operation)
    {
        /* mask the interrupt */
        intMaskShadow[maskIdx] &= ~(evNode->intBitMask);
    }
    else
    {
        /* unmask the interrupt */
        intMaskShadow[maskIdx] |= (evNode->intBitMask);
    }

    if((evNode->intMaskReg !=
        prvCpssDrvPpConfig[devNum]->intCtrl.intSumMaskRegAddr) ||
        (0 == PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum)))
    {
        #ifdef AX_FULL_DEBUG
    	osPrintf("prvCpssDrvInterruptMaskSet::dev %d reg %#0x shadown %#0x\r\n", \
					evNode->devNum,
                    evNode->intMaskReg,
                    intMaskShadow[maskIdx]);
		#endif
		/*
		 * 	by qinhs@autelan.com on Jul. 29 2008
		 *	Currently mask global interrupts when system starting-up,
		 *	When system started-up unmask the interrupts by signal
		 */
		#ifndef __AX_PLATFORM__
		if((0x00000034 == evNode->intMaskReg) ||
			(0x00000118 == evNode->intMaskReg)) {
			return GT_OK;
		}
		#endif
        
        return prvCpssDrvHwPpPortGroupWriteRegister(
                    devNum,
                    portGroupId,
                    evNode->intMaskReg,
                    intMaskShadow[maskIdx]);
    }
    else
    {
        
        #ifdef AX_FULL_DEBUG
    	osPrintf("prvCpssDrvInterruptMaskSet::internal dev %d reg %#0x shadown %#0x\r\n", \
					evNode->devNum,
                    evNode->intMaskReg,
                    intMaskShadow[maskIdx]);
        #endif
        return prvCpssDrvHwPpPortGroupWriteInternalPciReg(
                    devNum,
                    portGroupId,
                    evNode->intMaskReg,
                    intMaskShadow[maskIdx]);
    }
}


#ifndef __AX_PLATFORM__
/*******************************************************************************
* cpssIntCauseRead
*
* DESCRIPTION:
*       This function read all interrupt cause register.
*
* INPUTS:
*       devNum  - The device number of which PP to read
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssIntCauseRead
(
	IN GT_U8 devNum
)
{
	GT_U32 regAddr;
	unsigned int value,i;
	GT_STATUS rc;
	#define PRINT_EQUAL "=========="

	osPrintf("%sREAD ALL INTERRUPT CAUSE REGISTER(dev%d)%s\r\n",PRINT_EQUAL,devNum,PRINT_EQUAL);
	
	for(i = 0;i < cheetah2IntCauseRegMapArrLen;i++)
	{
		regAddr = cheetah2IntCauseRegMapArr[i];
		value = 0;
		rc = prvCpssDrvHwPpReadRegister(devNum,regAddr,&value);
		if(GT_OK != rc)
		{
			osPrintf("cpssIntCauseRead::dev %d reg %#0x value %#0x return %d error\r\n", \
						devNum,regAddr,value,rc);
		}
		else
			osPrintf("%#-08x:%#-8x ok\r\n",regAddr,value);
	}

	return rc;
}
/*******************************************************************************
* cpssIntMaskRead
*
* DESCRIPTION:
*       This function read all interrupt mask register.
*
* INPUTS:
*       devNum  - The device number of which PP to read
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssIntMaskRead
(
	IN GT_U8 devNum
)
{
	GT_U32 regAddr;
	unsigned int value,i;
	GT_STATUS rc;
	#define PRINT_EQUAL "=========="

	osPrintf("%sREAD ALL INTERRUPT MASK REGISTER(dev%d)%s\r\n",PRINT_EQUAL,devNum,PRINT_EQUAL);
	
	for(i = 0;i < cheetah2MaskRegMapArrLen;i++)
	{
		regAddr = cheetah2MaskRegMapArr[i];
		value = 0;
		rc = prvCpssDrvHwPpReadRegister(devNum,regAddr,&value);
		if(GT_OK != rc)
		{
			osPrintf("cpssIntMaskRead::dev %d reg %#0x value %#0x return %d error\r\n", \
						devNum,regAddr,value,rc);
		}
		else
			osPrintf("%#-08x:%#-8x ok\r\n",regAddr,value);
	}

	return rc;
}
/*******************************************************************************
* cpssIntReadAll
*
* DESCRIPTION:
*       This function read all interrupt mask register.
*
* INPUTS:
*       devNum  - The device number of which PP to read
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssIntReadAll
(
	IN GT_U8 devNum
)
{
	GT_STATUS rc;
	rc = cpssIntCauseRead(devNum);
	if(GT_OK != rc)
	{
		osPrintf("cpssIntReadAll::dev %d return %d\r\n",devNum,rc);
	}
	rc = cpssIntMaskRead(devNum);
	if(GT_OK != rc)
	{
		osPrintf("cpssIntReadAll::dev %d return %d\r\n",devNum,rc);
	}

	return rc;	
}


#endif

