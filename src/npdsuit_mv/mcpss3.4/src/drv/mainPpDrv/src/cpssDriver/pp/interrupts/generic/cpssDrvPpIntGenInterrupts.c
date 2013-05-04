/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntGenInterrupts.c
*
* DESCRIPTION:
*       This file includes initialization function for the interrupts module,
*       and low level interrupt handling (interrupt bits scanning).
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>


/*******************************************************************************
* internal definitions
*******************************************************************************/

#ifdef DRV_INTERRUPTS_DBG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

/*******************************************************************************
 * External usage variables
 ******************************************************************************/

/*******************************************************************************
* prvCpssDrvInterruptDeviceRemove
*
* DESCRIPTION:
*       This function removes a list of devices after Hot removal operation.
*
* INPUTS:
*       devNum   - device Number to be removed.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptDeviceRemove
(
    IN  GT_U8   devNum
)
{
    GT_STATUS       rc;         /* Function return value.           */
    GT_U32          portGroupId;

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /* Delete the node from the linked-list.    */
        rc = prvCpssDrvInterruptDisconnect(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intVecNum,
                            prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].isrConnectionId);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)


    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvInterruptPpSR
*
* DESCRIPTION:
*       This is the Packet Processor interrupt service routine, it scans the
*       interrupts and enqueues each of them to the interrupt queues structure.
*
* INPUTS:
*       cookie  - (devNum) The PP device number to scan.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       1 - if interrupts where received from the given device, or
*       0 - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_U8 prvCpssDrvInterruptPpSR
(
    IN void *cookie
)
{
    GT_U8   devNum;         /* The Pp device number to scan.            */
    PRV_CPSS_DRV_INT_CTRL_STC *intCtrlPtr;
    GT_U32   portGroupId;            /* The port group ID number to scan .
                                   relevant to devices with multi-port-groups.
                                   for 'non multi-port-groups' device it will be
                                   PRV_CPSS_DRV_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS */
    PRV_CPSS_DRV_INT_ISR_COOKIE_STC        *isrCookieInfoPtr;/*pointer to format of the cookie of interrupt*/

    isrCookieInfoPtr = cookie;
    devNum = isrCookieInfoPtr->devNum;
    portGroupId = isrCookieInfoPtr->portGroupId;

    intCtrlPtr = &prvCpssDrvPpConfig[devNum]->intCtrl;

    return prvCpssDrvIntEvReqDrvnScan(devNum,portGroupId,
                   intCtrlPtr->portGroupInfo[portGroupId].intNodesPool,
                   intCtrlPtr->portGroupInfo[portGroupId].intMaskShadow,
                   intCtrlPtr->portGroupInfo[portGroupId].intScanRoot);
}




/********************************************************************************
* !!!!!!!!!!!!!!!!!!!!! FOR DEBUG PURPOSES ONLY !!!!!!!!!!!!!!!!!!!!!!!!!!
********************************************************************************/
/*#define DRV_INTERRUPTS_DBG*/
#ifdef DRV_INTERRUPTS_DBG

extern void     lkAddr (unsigned int addr);

void prvCpssDrvSubPrintIntScan(PRV_CPSS_DRV_INTERRUPT_SCAN_STC  *pIntScanNode)
{
    GT_U8 i;

    cpssOsPrintf("isGpp = %d.\n",pIntScanNode->isGpp);
    if(pIntScanNode->isGpp == GT_TRUE)
    {
        /* This is a Gpp interrupt, call the attached function. */
        return;
    }

    /* Read the interrupt cause register.           */
    cpssOsPrintf("pIntScanNode->causeRegAddr = 0x%x.\n",pIntScanNode->causeRegAddr);
    cpssOsPrintf("pIntScanNode->pRegReadFunc = 0x%x:\n",pIntScanNode->pRegReadFunc);
    /*lkAddr(pIntScanNode->pRegReadFunc);*/

    /* Scan local interrupts (non-summary bits).    */
    cpssOsPrintf("pIntScanNode->nonSumBitMask = 0x%x.\n",pIntScanNode->nonSumBitMask);
    cpssOsPrintf("startIdx = %d, endIdx = %d.\n",pIntScanNode->startIdx,
             pIntScanNode->endIdx);

    /* Unmask the received interrupt bits if needed */
    cpssOsPrintf("pIntScanNode->maskRcvIntrEn = 0x%x.\n",pIntScanNode->maskRcvIntrEn);
    cpssOsPrintf("pIntScanNode->maskRegAddr = 0x%x.\n",pIntScanNode->maskRegAddr);
    cpssOsPrintf("pIntScanNode->pRegWriteFunc = 0x%x.\n",pIntScanNode->pRegWriteFunc);
    /*lkAddr(pIntScanNode->pRegWriteFunc);*/

    /* Clear the interrupts (if needed).            */
    cpssOsPrintf("pIntScanNode->rwBitMask = 0x%x.\n",pIntScanNode->rwBitMask);

    cpssOsPrintf("pIntScanNode->subIntrListLen = %d.\n",pIntScanNode->subIntrListLen);
    for(i = 0; i < pIntScanNode->subIntrListLen; i++)
    {
        cpssOsPrintf("----------------------------\n");
        cpssOsPrintf("pIntScanNode->subIntrScan[i]->bitNum = %d.\n",
                 pIntScanNode->subIntrScan[i]->bitNum);
        prvCpssDrvSubPrintIntScan(pIntScanNode->subIntrScan[i]);
    }

    return;
}

void prvCpssDrvPrintIntScan(GT_U8 devNum)
{
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC *pIntScanNode;
    GT_U32  portGroupId = 0;

    pIntScanNode = prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intScanRoot;

    prvCpssDrvSubPrintIntScan(pIntScanNode);
    return;
}

#endif /*DRV_INTERRUPTS_DBG*/

#ifndef __AX_PLATFORM__
/*******************************************************************************
* cpssDrvIntScanPortLinkStatusMaskShadowSet
*
* DESCRIPTION:
*       This function scans a given port's interrupt mask register, and performs 
*   interrupt mask/unmask operation.
*
* INPUTS:
*       devNum         - The device number to scan.
*	  portNum	  - The port number on device
*	  enable 		  - flag controls mask/unmask port LinkStatusChange interrupt
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - Setup mask shadow value successfully.
*	  GT_FAIL - Port's mask register not found in the scan array.
*
* COMMENTS:
*       This routine is used on CH2 devices' ports need mask/unmask link event
*
*******************************************************************************/
GT_STATUS cpssDrvIntScanPortLinkStatusMaskShadowSet
(
	IN GT_U8 devNum,
	IN GT_U8 portNum,
	IN GT_BOOL enable
)
{
	PRV_CPSS_DRV_INTERRUPT_SCAN_STC  *intScanStackPtr[PRV_CPSS_DRV_SCAN_STACK_SIZE_CNS]; /* scan stack		   */
	PRV_CPSS_DRV_INTERRUPT_SCAN_STC  *currIntScanPtr = NULL;/* The current pointer in scan stack */
	PRV_CPSS_DRV_INTERRUPT_SCAN_STC  **intStackTopPtr = NULL; /* The top of scan stack pointer	  */
	GT_U32 	i = 0; /* Iterator 						 */
    GT_BOOL  regFound = GT_FALSE; /* A flag for register addr search   */
    PRV_CPSS_DRV_INT_CTRL_STC *intCtrlPtr = NULL; /* interrupt control block */
	unsigned int *intMaskShadowPtr = NULL; /* interrupt mask shadow value */
	unsigned int *maskShadowValPtr = NULL;	/* specified interrupt mask shadow */
	GT_U32 causeRegAddr = 0,maskRegAddr = 0;

    intCtrlPtr = &prvCpssDrvPpConfig[devNum]->intCtrl;
    #if 0   /* luoxun -- cpss1.3 */
	intMaskShadowPtr = intCtrlPtr->intMaskShadow;
    #endif
    intMaskShadowPtr = intCtrlPtr->portGroupInfo[0].intMaskShadow;

	/* calculate link status interrupt mask regsiter for the port */
	causeRegAddr = 0x0A800020 + portNum * 0x100;
	maskRegAddr	 = 0x0A800024 + portNum * 0x100;

	/* push the first scan element into the stack */
    #if 0   /* luoxun -- cpss1.3 */
	intScanStackPtr[0] = intCtrlPtr->intScanRoot;
    #endif
    intScanStackPtr[0] = intCtrlPtr->portGroupInfo[0].intScanRoot;
	intStackTopPtr	   = &intScanStackPtr[0];

	while (intStackTopPtr >= &intScanStackPtr[0])
	{
		/* pop the interrupt scan node */
		currIntScanPtr = *intStackTopPtr;
		intStackTopPtr--;

		/* search for the register address in current scan node */
		if((maskRegAddr == currIntScanPtr->maskRegAddr) &&
			(causeRegAddr == currIntScanPtr->causeRegAddr))
		{
			maskShadowValPtr = &intMaskShadowPtr[currIntScanPtr->startIdx >> 5];
			if(enable) {/* unmask the interrupt */
				*maskShadowValPtr |= (1 << 1);/* LinkStatusChange INT bit1 */
			}
			else {
				*maskShadowValPtr &= ~(1<<1);
			}

            #if 0   /* luoxun -- cpss1.3 */
			currIntScanPtr->pRegWriteFunc(devNum, 
							currIntScanPtr->maskRegAddr, *maskShadowValPtr);
            #endif
            currIntScanPtr->pRegWriteFunc(devNum, 0,
							currIntScanPtr->maskRegAddr, *maskShadowValPtr);
			osPrintfEvt("port(%d,%d) interrupt %s link status (mask %#x shadow %#x)\n", \
					devNum, portNum, enable ? "unmask":"mask", maskRegAddr,*maskShadowValPtr);
			regFound = GT_TRUE;
			return GT_OK;
		}

		if (0 == currIntScanPtr->subIntrListLen)
		{
			/* scan element does not have sub interrupts */
			continue;
		}
		else
		{
			regFound = GT_FALSE;

			/* search for sub scan nodes : add sub node to scan stack */
            for(i = 0; i < currIntScanPtr->subIntrListLen; i++)
            {
                intStackTopPtr++;
                if (intStackTopPtr > &intScanStackPtr[PRV_CPSS_DRV_SCAN_STACK_LAST_ENTRY_CNS])
                {
                    osPrintfErr("port(%d,%d) %s link status int scan stack overflow!\n", \
							devNum, portNum ,enable ? "mask":"unmask");
                    return 0;
                }
                *intStackTopPtr = currIntScanPtr->subIntrScan[i];
            }
		}
	}

	if(GT_FALSE == regFound) 
		return GT_FAIL;
	else 
		return GT_OK; 
}

/*******************************************************************************
* cpssDrvIntScanCpuRxErrMaskShadowSet
*
* DESCRIPTION:
*       This function scans a given CPU queue's interrupt mask register, and performs 
*   interrupt mask/unmask operation.
*
* INPUTS:
*       devNum         - The device number to scan.
*	  queueIdx	  - The cpu queue number on device
*	  enable 		  - flag controls mask/unmask cpu queue's rx error interrupt
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - Setup mask shadow value successfully.
*	  GT_FAIL - Port's mask register not found in the scan array.
*
* COMMENTS:
*       This routine is used on CH2 devices' ports need mask/unmask receive error event
*
*******************************************************************************/
GT_STATUS cpssDrvIntScanCpuRxErrMaskShadowSet
(
	IN GT_U8 devNum,
	IN GT_U8 queueIdx,
	IN GT_BOOL enable
)
{
	PRV_CPSS_DRV_INTERRUPT_SCAN_STC  *intScanStackPtr[PRV_CPSS_DRV_SCAN_STACK_SIZE_CNS]; /* scan stack		   */
	PRV_CPSS_DRV_INTERRUPT_SCAN_STC  *currIntScanPtr = NULL;/* The current pointer in scan stack */
	PRV_CPSS_DRV_INTERRUPT_SCAN_STC  **intStackTopPtr = NULL; /* The top of scan stack pointer	  */
	GT_U32 	i = 0; /* Iterator 						 */
    GT_BOOL  regFound = GT_FALSE; /* A flag for register addr search   */
    PRV_CPSS_DRV_INT_CTRL_STC *intCtrlPtr = NULL; /* interrupt control block */
	unsigned int *intMaskShadowPtr = NULL; /* interrupt mask shadow value */
	unsigned int *maskShadowValPtr = NULL;	/* specified interrupt mask shadow */
	GT_U32 causeRegAddr = 0,maskRegAddr = 0;
	static GT_U8 queueRxErrMask[NUM_OF_RX_QUEUES] = {0};/* Rx error event mask state: 0 - unmask, 1 -mask */

	if(queueIdx >= NUM_OF_RX_QUEUES) return GT_FAIL;

	/* check if already unmasked when enable, or 
	  *	already masked when disable
	  */
	if((!enable && queueRxErrMask[queueIdx])||
		(enable && !queueRxErrMask[queueIdx])) 
	{
		return GT_OK;
	}
	else {
		queueRxErrMask[queueIdx] = (enable ? 0:1);
	}

    intCtrlPtr = &prvCpssDrvPpConfig[devNum]->intCtrl;
    #if 0   /* luoxun -- cpss1.3 */
	intMaskShadowPtr = intCtrlPtr->intMaskShadow;
    #endif
    intMaskShadowPtr = intCtrlPtr->portGroupInfo[0].intMaskShadow;

	/* interrupt mask regsiter for the cpu queue */
	causeRegAddr = 0x0000280C;
	maskRegAddr	 = 0x00002814;

	/* push the first scan element into the stack */
    #if 0   /* luoxun -- cpss1.3 */
	intScanStackPtr[0] = intCtrlPtr->intScanRoot;
    #endif
    intScanStackPtr[0] = intCtrlPtr->portGroupInfo[0].intScanRoot;
	intStackTopPtr	   = &intScanStackPtr[0];

	while (intStackTopPtr >= &intScanStackPtr[0])
	{
		/* pop the interrupt scan node */
		currIntScanPtr = *intStackTopPtr;
		intStackTopPtr--;

		/* search for the register address in current scan node */
		if((maskRegAddr == currIntScanPtr->maskRegAddr) &&
			(causeRegAddr == currIntScanPtr->causeRegAddr))
		{
			maskShadowValPtr = &intMaskShadowPtr[currIntScanPtr->startIdx >> 5];
			if((enable && ((*maskShadowValPtr)>>(queueIdx + 11) & 0x1)) ||
				(!enable && !((*maskShadowValPtr)>>(queueIdx + 11) & 0x1)))
			{
				return GT_OK;
			}
			if(enable) {/* unmask the interrupt */
				*maskShadowValPtr |= (1 <<(queueIdx + 11));/* LinkStatusChange INT bit1 */
			}
			else {
				*maskShadowValPtr &= ~(1<<(queueIdx + 11));
			}

            #if 0   /* luoxun -- cpss1.3 */
			currIntScanPtr->pRegWriteFunc(devNum, 
							currIntScanPtr->maskRegAddr, *maskShadowValPtr);
            #endif
            currIntScanPtr->pRegWriteFunc(devNum, 0,
							currIntScanPtr->maskRegAddr, *maskShadowValPtr);
			osPrintfEvt("device %d cpu queue %d interrupt %s rx error (mask %#x shadow %#x)\n", \
					devNum, queueIdx, enable ? "mask":"unmask", maskRegAddr,*maskShadowValPtr);
			regFound = GT_TRUE;
			return GT_OK;
		}

		if (0 == currIntScanPtr->subIntrListLen)
		{
			/* scan element does not have sub interrupts */
			continue;
		}
		else
		{
			regFound = GT_FALSE;

			/* search for sub scan nodes : add sub node to scan stack */
            for(i = 0; i < currIntScanPtr->subIntrListLen; i++)
            {
                intStackTopPtr++;
                if (intStackTopPtr > &intScanStackPtr[PRV_CPSS_DRV_SCAN_STACK_LAST_ENTRY_CNS])
                {
                    osPrintfErr("device %d cpu queue %d %s rx error int scan stack overflow!\n", \
							devNum, queueIdx ,enable ? "mask":"unmask");
                    return 0;
                }
                *intStackTopPtr = currIntScanPtr->subIntrScan[i];
            }
		}
	}

	if(GT_FALSE == regFound) 
		return GT_FAIL;
	else 
		return GT_OK; 
}

#endif


