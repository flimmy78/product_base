/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAppDemoQT2025PhyConfig.c
*
* DESCRIPTION:
*       Initialization functions for the Lion board.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#ifndef ASIC_SIMULATION
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <appDemo/sysHwConfig/gtAppDemoQT2025PhyConfig.h>
#include <appDemo/sysHwConfig/gtAppDemoLioncubConfig.h>

/* structures definition for the use of Dx Cheetah Prestera core SW */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/*******************************************************************************
 * Global variables
 ******************************************************************************/

GT_U32 *xsmiAddrPtr = NULL;

/*******************************************************************************
* qt2025PhyCfgPhaseX
*
* DESCRIPTION:
*       Init PHY for Lion 48 - init done by 7 different phases
*
* INPUTS:
*       dev - device number
*       phase - phase number (1 to 7)
*       phyDataPhase - pointer to phy init structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS qt2025PhyCfgPhaseX(
    GT_U8 dev,
    GT_U32 phase,
    GT_U32 array_size,
    GT_QT2025_FW *phyDataPhase
)
{
    GT_U8  phyAddr, portGroup, port, localPortNum;
    GT_U32 i;
    GT_U16 rxLossVal, txLossVal;

    if(xsmiAddrPtr == NULL)
        return GT_NOT_INITIALIZED;

    for (portGroup = 0; portGroup < 4; portGroup++)
    {
        /* check if current core connected to lioncub */
        if (lioncubConnected(dev, portGroup) == GT_FALSE)
        {
            continue;
        }

        for (localPortNum = 0; localPortNum <= 11; localPortNum++) 
        {
            port = localPortNum + (16*portGroup);
            phyAddr = xsmiAddrPtr[port];

            for (i = 0; i < array_size; i++)
            {
                /* set rx/tx loss for phase 1 only */
                if ((phase == 1) && (phyDataPhase[i].regAddr == 0x27))
                {
                    rxLossVal = reg27RxTxLossArray_Lion_B0[localPortNum].rxLoss;
                    txLossVal = reg27RxTxLossArray_Lion_B0[localPortNum].txLoss;
                    GT_U16_QT2025_SET_FIELD(phyDataPhase[i].data,4,3,rxLossVal);
                    GT_U16_QT2025_SET_FIELD(phyDataPhase[i].data,1,3,txLossVal);
                }

                /* write to Phy via XSMI */
                if (cpssDxChPhyPort10GSmiRegisterWrite(dev, port, phyAddr, GT_TRUE, phyDataPhase[i].regAddr, 
                                                        phyDataPhase[i].devAddr, phyDataPhase[i].data) != GT_OK)
                {
                    return GT_FAIL;
                }
            }

            /* Second phase will broadcast data to all PHYs together so we need to perform it one time only */
            if (phase == 2)
            {
                break;
            }
        }
    }
    return GT_OK;
}

/*******************************************************************************
* qt2025PhyConfig
*
* DESCRIPTION:
*       Init PHY for Lion 48
*
* INPUTS:
*       dev - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
 GT_STATUS qt2025PhyConfig
(
    GT_U8  dev,
    GT_U32 *xsmiAddrArrayPtr
)
{
    GT_U32  phase1_array_size, phase2_array_size, phase3_array_size;
    GT_U8   phyAddr, portGroup, port, localPortNum;
    GT_U16  data;

    if((xsmiAddrPtr = xsmiAddrArrayPtr) == NULL)
        return GT_NOT_INITIALIZED;

    phase1_array_size = sizeof(phyDataPhase1)/sizeof(GT_QT2025_FW);
    phase2_array_size = sizeof(phyDataPhase2)/sizeof(GT_QT2025_FW);
    phase3_array_size = sizeof(phyDataPhase3)/sizeof(GT_QT2025_FW);

    /* phase 1 */
    if (qt2025PhyCfgPhaseX(dev, 1, phase1_array_size, phyDataPhase1) != GT_OK)
    {
        return GT_FAIL;
    }

    /* wait 1 mSec */
    osTimerWkAfter(0);

    /* special phase to enable broardcast */
    if (qt2025PhyCfgPhaseX(dev, 1, phase1_array_size, phyDataPhase1a) != GT_OK)
    {
        return GT_FAIL;
    }

    /* wait 1 mSec */
    osTimerWkAfter(0);

    /* phase 2 */
    if (qt2025PhyCfgPhaseX(dev, 2, phase2_array_size, phyDataPhase2) != GT_OK)
    {
        return GT_FAIL;
    }

    /* wait 600 mSec */
    osTimerWkAfter(600);

    /* phase 3 */
    if (qt2025PhyCfgPhaseX(dev, 3, phase3_array_size, phyDataPhase3) != GT_OK)
    {
        return GT_FAIL;
    }

    /* wait additional 3000 mSec before FW stabilized */
    osTimerWkAfter(3000);

    /* Check download status */
    for (portGroup = 0; portGroup < 4; portGroup++)
    {
        if(CPSS_98CX8203_CNS == PRV_CPSS_PP_MAC(dev)->devType)
        {
            if( (2 == portGroup) || (3 == portGroup) )
                continue;
        }

        for (localPortNum = 0; localPortNum <= 11; localPortNum++) 
        {
            port = localPortNum + (16*portGroup);
            phyAddr = xsmiAddrPtr[port];

            /* Read Phy via XSMI */
            if (cpssDxChPhyPort10GSmiRegisterRead(dev, port, phyAddr, GT_TRUE, 0xd7fd, 3, &data) != GT_OK)
            {
                return GT_FAIL;
            }

            if ((data == 0xF0) || (data == 0) || (data == 0x10)) /* (Checksum fails) */
            {
                osPrintf("\nFW Checksum error on port %d portGroup %d (phyAddr=0x%x) - value is 0x%x.", 
                    localPortNum, portGroup, phyAddr, data);         
            }
        }
    }

    return GT_OK;
}

#endif /* ASIC_SIMULATION */
