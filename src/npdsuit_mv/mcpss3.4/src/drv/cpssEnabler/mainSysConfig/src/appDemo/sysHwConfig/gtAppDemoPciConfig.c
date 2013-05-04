/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtAppDemoPciConfig.c
*
* DESCRIPTION:
*       Prestera Devices pci initialization & detection module.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.3 $
*
*******************************************************************************/

#include <appDemo/sysHwConfig/gtAppDemoPciConfig.h>

#include <appDemo/sysHwConfig/gtAppDemoSmiConfig.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#include <gtExtDrv/drivers/gtSmiHwCtrl.h>
#include <sysdef/npd_sysdef.h>

#ifdef SHARED_MEMORY
#   include <gtOs/gtOsSharedPp.h>
#endif

#if defined(_FreeBSD)
    #include <gtExtDrv/drivers/gtDmaDrv.h>
#endif
    
#if defined (_linux) || defined (_FreeBSD)

#if (OS_TARGET) == FreeBSD && (OS_TARGET_RELEASE) == 4
    #include <sys/types.h>
#endif

#include <sys/mman.h>

#if (OS_TARGET) == FreeBSD
#define PCIR_BARS        0x10
#define PCIR_BAR(x)     (PCIR_BARS + (x) * 4)
#endif

#endif
/*******************************************************************************
* internal definitions
*******************************************************************************/
#ifdef PRESTERA_DEBUG
#define APP_INIT_DEBUG
#endif

#ifdef APP_INIT_DEBUG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

#ifndef __AX_PLATFORM__
#define DBG_INFO(x)     osPrintfEvt x
#endif

#if defined(_linux)
#include <cpss/generic/cpssTypes.h>
#define CONFIG_SIZE (4 * 1024)
#define REGS_SIZE (64 * 1024 * 1024)
#define PP_SIZE (CONFIG_SIZE + REGS_SIZE)
#define NUM_PPS 2
/* What about simulation */
#if !defined(ASIC_SIMULATION)
extern GT_32 gtPpFd;
#endif
#endif

/*******************************************************************************
* Internal usage environment parameters
*******************************************************************************/

#define MAX_DEV_IDS_CNS  256

static GT_PCI_DEV_VENDOR_ID device[MAX_DEV_IDS_CNS] =
{
    {0,0}/* --> must be last 'Non CPSS' device */


    /************************************************************/
    /* entries here will be filled in runtime from the CPSS DB  */
    /* see function initDevArray()                              */
    /* NOTE : this function will arange the Dx devices as last  */
    /* devices to support VB with ExMx/ExMxPm device with the Dx*/
    /* device(s)                                                */
    /***********************************************************/

};

static GT_STATUS initDevArray(void);

#define END_OF_TABLE    0xFFFFFFFF
/* DB to hold the device types that the CPSS support */
extern const struct {
    CPSS_PP_FAMILY_TYPE_ENT     devFamily;
    GT_U8                       numOfPorts;
    const CPSS_PP_DEVICE_TYPE   *devTypeArray;
    const CPSS_PORTS_BMP_STC    *defaultPortsBmpPtr;
    const /*CORES_INFO_STC*/void *coresInfoPtr;
    const GT_U32                *activeCoresBmpPtr;
}cpssSupportedTypes[];

#ifndef PRESTERA_NO_HW

#if defined(_linux) && !defined(ASIC_SIMULATION)
/*
 * static struct prvPciDeviceQuirks prvPciDeviceQuirks[]
 *
 * Quirks can be added to GT_PCI_DEV_VENDOR_ID structure
 */
PRV_PCI_DEVICE_QUIRKS_ARRAY_MAC

static struct prvPciDeviceQuirks*
prvPciDeviceGetQuirks(unsigned vendorId, unsigned devId)
{
    int k;
    GT_U32  pciId;

    pciId = (devId << 16) | vendorId;

    for (k = 0; prvPciDeviceQuirks[k].pciId != 0xffffffff; k++)
    {
        if (prvPciDeviceQuirks[k].pciId == pciId)
            break;
    }
    return &(prvPciDeviceQuirks[k]);
}
#endif /* _linux */

/*******************************************************************************
* gtPresteraGetPciDev
*
* DESCRIPTION:
*       This routine search for Prestera Devices Over the PCI.
*
* INPUTS:
*       first - whether to bring the first device, if GT_FALSE return the next
*               device.
*
* OUTPUTS:
*       pciInfo - the next device PCI info.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*       GT_NO_MORE - no more prestera devices.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtPresteraGetPciDev
(
    IN  GT_BOOL     first,
    OUT GT_PCI_INFO *pciInfo
)
{
    GT_U32  busNo;                  /* pci bus number */
    GT_U32  deviceNo;               /* PCI device number */
    GT_U32  funcNo;                 /* PCI function number */
    GT_U32  baseAddr;               /* PCI base address */
    GT_U32  internalBase;           /* Internal Pci base    */
    /* zhangdi copy from mv_cpss */
	GT_U32	dmaBase;				/* DMA base address access by CPU */
	unsigned int deviceDmaBase;		/* DMA base address access by device */
    /* end */

    printf("gtPresteraGetPciDev()\n");
	DBG_INFO(("******************************************\n"));
	DBG_INFO(("************** gtPresteraGetPciDev *******\n"));
	DBG_INFO(("******************************************\n"));

	
#if defined(_linux) && !defined(ASIC_SIMULATION)
    struct prvPciDeviceQuirks* quirks;
#endif
    static GT_U32 deviceIdx = 0;    /* device id index */
    static GT_U32 instance = 0;     /* device instance */
    static GT_U32 numOfDevices = 0;/* total number of devices in system */
    GT_U8   i;
    GT_STATUS ret;                  /* function return value */

    /* check parameters */
    if(pciInfo == NULL)
    {
        return GT_BAD_PARAM;
    }
    /* wheck whether first call */
    if(first == GT_TRUE)
    {
        deviceIdx = 0;
        instance = 0;
        numOfDevices = 0;

        ret = initDevArray();
        if(ret != GT_OK)
        {
            return ret;
        }
    }

    /*call the BSP PCI facility to get all Prestera devices */
    for(;device[deviceIdx].vendorId != 0;)
    {     
        #ifdef __AX_PLATFORM__
    	if(numOfDevices>0)
			return GT_NO_MORE;
        #endif
        DBG_INFO(("search the PCI devices 0x%04x\n",device[deviceIdx].devId));
        if(extDrvPciFindDev(device[deviceIdx].vendorId, device[deviceIdx].devId,
                            instance++, &busNo, &deviceNo, &funcNo) != GT_OK)
        {
            DBG_INFO(("move to the next device\n"));
            instance = 0; /* first instance for that device type */
            deviceIdx++;
            continue;
        }
        numOfDevices++;
        DBG_INFO(("found Prestera device\n"));
        DBG_INFO(("numOfDevices = %d\n", numOfDevices));
        DBG_INFO(("deviceIdx = %d\n", deviceIdx));
        /* get device BAR 0 */
        ret = extDrvPciConfigReadReg(busNo,deviceNo,funcNo,0x14,&baseAddr);
        if(ret != GT_OK)
        {
            DBG_INFO(("Failed In PCI configuration\n"));
            return GT_FAIL;
        }

        /* Get internal base address    */
        ret = extDrvPciConfigReadReg(busNo,deviceNo,funcNo,0x10,&internalBase);
        if(ret != GT_OK)
        {
            DBG_INFO(("Failed In PCI configuration\n"));

            return GT_FAIL;
        }

        pciInfo->pciDevVendorId = device[deviceIdx];

#if defined(_linux) && !defined(ASIC_SIMULATION)
        DBG_INFO(("prvPciDeviceGetQuirks(0x%04x, 0x%04x)\n", 
                    device[deviceIdx].vendorId, device[deviceIdx].devId));
        /* map config space */
        quirks = prvPciDeviceGetQuirks(device[deviceIdx].vendorId,
                device[deviceIdx].devId);

        baseAddr = ASIC_CFG_VIRT_ADDR + (numOfDevices - 1) * CONFIG_SIZE;
        baseAddr = (GT_U32) mmap((void*)(baseAddr + quirks->configOffset),
                                quirks->configSize,
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_FIXED,
                                gtPpFd,
                                quirks->configOffset);
        DBG_INFO(("ASIC_CFG_VIRT_ADDR baseAddr = 0x%08x\n", baseAddr));

        if (MAP_FAILED == (void*)baseAddr)
        {
            return GT_FAIL;
        }

#ifdef SHARED_MEMORY
        DBG_INFO(("SHARED_MEMORY\n"));
        /* We should note mapped section to restore mapping for all non-first clients */
        ret = shrMemRegisterPpMmapSection
        (
            (GT_VOID *)baseAddr,
            quirks->configSize,
            quirks->configOffset
        );
        if (GT_OK != ret) return ret;
#endif

        /* map register space */
        baseAddr = (GT_U32) mmap((void*)(ASIC_REG_VIRT_ADDR + ((numOfDevices - 1) << 26)),
                                REGS_SIZE,
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_FIXED,
                                gtPpFd,
                                0);
        DBG_INFO(("ASIC_REG_VIRT_ADDR baseAddr = 0x%08x\n", baseAddr));

        if (MAP_FAILED == (void*)baseAddr)
        {
            return GT_FAIL;
        }

#ifdef SHARED_MEMORY
        /* We should note mapped section to restore mapping for all non-first clients */
        ret = shrMemRegisterPpMmapSection
        (
            (GT_VOID *)baseAddr,
            REGS_SIZE,
            0
        );
        if (GT_OK != ret) return ret;
#endif

        /* config address BAR0 */
        pciInfo->internalPciBase = ASIC_CFG_VIRT_ADDR + (numOfDevices - 1) * CONFIG_SIZE;
        /* registers address BAR1 */
        pciInfo->pciBaseAddr     = ASIC_REG_VIRT_ADDR + ((numOfDevices - 1) << 26);
#else
    #if defined(_FreeBSD) && !defined(ASIC_SIMULATION)
        /* config address BAR0 */
        extDrvPciConfigReadReg(busNo, deviceNo, funcNo, PCIR_BAR(0), &(pciInfo->internalPciBase));
        pciInfo->internalPciBase &= (GT_U32)(~0x3f);
        extDrvPhy2Virt(pciInfo->internalPciBase, &(pciInfo->internalPciBase));

        /* registers address BAR2 (there is no bar1) */
        extDrvPciConfigReadReg(busNo,deviceNo,funcNo, PCIR_BAR(2), &(pciInfo->pciBaseAddr));
        pciInfo->pciBaseAddr &= (GT_U32)(~0x3f);
        extDrvPhy2Virt(pciInfo->pciBaseAddr, &(pciInfo->pciBaseAddr));
    #else
        osPhy2Virt(baseAddr, &(pciInfo->pciBaseAddr));
        osPhy2Virt(internalBase, &(pciInfo->internalPciBase));
    #endif
#endif
        pciInfo->pciIdSel   = deviceNo;
        pciInfo->pciBusNum  = busNo;
        pciInfo->funcNo     = funcNo;

        /* zhangdi copy form mv_cpss */
		/* Get DMA base address for both CPU and device access */
		ret = extDrvGetDeviceDmaBase(pciInfo->pciBusNum,pciInfo->pciIdSel,pciInfo->funcNo,&dmaBase,&deviceDmaBase);
        if(ret != GT_OK)
        {
            DBG_INFO(("Failed In get DMA base configuration\n"));
            return GT_FAIL;
        }
		pciInfo->dmaBase 		= dmaBase;
		pciInfo->deviceDmaBase	= deviceDmaBase;
		DBG_INFO(("DMA base:0x%p device DMA base:0x%p\r\n",dmaBase,deviceDmaBase));
        /* copy end */
		
        /* Get the Pci header info  */
        for(i = 0; i < 64; i += 4)
        {
            ret = extDrvPciConfigReadReg(busNo,deviceNo,funcNo,i,
                                         &(pciInfo->pciHeaderInfo[i / 4]));
            if(ret != GT_OK)
            {
                DBG_INFO(("Failed In PCI configuration\n"));

                return GT_FAIL;
            }
        }
        return GT_OK;
    }
    return GT_NO_MORE;
}


/*******************************************************************************
* gtPresteraIsPexSwitchOnBoard
*
* DESCRIPTION:
*       This routine search for Prestera Devices Over the PCI.
*
* INPUTS:
*
* OUTPUTS:
*       pexOnBoardPtr - GT_TRUE  - pex switch is on board
*                       GT_FALSE - pex switch is absent
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*       GT_NO_MORE - no more prestera devices.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtPresteraIsPexSwitchOnBoard
(
    OUT  GT_BOOL   *pexOnBoardPtr
)
{
    GT_U32  busNo;                        /* pci bus number */
    GT_U32  deviceNo;                     /* PCI device number */
    GT_U32  funcNo;                       /* PCI function number */
    GT_U32  instance = 0;                 /* device instance */
    GT_U16  pexVendorId = 0x111d;         /* IDT vendor id */
    GT_U16  pexSwitchDeviceId = 0x802b;   /*PEX switch device id */

    if(extDrvPciFindDev(pexVendorId,
                        pexSwitchDeviceId,
                        instance++,
                        &busNo,
                        &deviceNo,
                        &funcNo) != GT_OK)
        *pexOnBoardPtr = GT_FALSE;
    else
        *pexOnBoardPtr = GT_TRUE;

    return GT_OK;

}

/*******************************************************************************
* gtPresteraGetPciIntVec
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intVec - PCI interrupt vector.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtPresteraGetPciIntVec
(
    IN  GT_PCI_INT  pciInt,
    OUT void        **intVec
)
{
    GT_STATUS retVal;

    /* check parameters */
    if(intVec == NULL)
    {
        return GT_BAD_PARAM;
    }
    /* get the PCI interrupt vector */
    retVal = extDrvGetPciIntVec(pciInt, intVec);
    /* check whether a valid value */
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed in extDrvGetPciIntVec\n"));
        return retVal;
    }
    return GT_OK;
}

/*******************************************************************************
* gtPresteraGetPciIntMask
*
* DESCRIPTION:
*       This routine return the PCI interrupt mask to enable/disable interrupts.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intMask - PCI interrupt mask.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtPresteraGetPciIntMask
(
    IN  GT_PCI_INT  pciInt,
    OUT GT_U32      *intMask
)
{
    GT_STATUS retVal;

    /* check parameters */
    if(intMask == NULL)
    {
        return GT_BAD_PARAM;
    }
    /* get the PCI interrupt vector */
    retVal = extDrvGetIntMask(pciInt, intMask);
    /* check whether a valid value */
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed in extDrvGetPciIntVec\n"));
        return retVal;
    }
    return GT_OK;
}

#else


/*******************************************************************************
* gtPresteraGetPciDev
*
* DESCRIPTION:
*       This routine search for Prestera Devices Over the PCI.
*
* INPUTS:
*       first - whether to bring the first device, if GT_FALSE return the next
*               device.
*
* OUTPUTS:
*       pciInfo - the next device PCI info.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*       GT_NO_MORE - no more prestera devices.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtPresteraGetPciDev
(
    IN  GT_BOOL     first,
    OUT GT_PCI_INFO *pciInfo
)
{

    static GT_U32 i;
    if(first == GT_TRUE)
    {
        i = 0;
    }
    if(i >= 2)
    {
        return GT_NO_MORE;
    }
    i++;
    pciInfo->pciDevVendorId.devId = device[0].devId;
    pciInfo->pciDevVendorId.vendorId = device[0].vendorId;
    pciInfo->pciBaseAddr = 0x10000000 + (i * 0x10000000);
    pciInfo->internalPciBase = (GT_U32)osMalloc(4);
    (* ((GT_U32 *)pciInfo->internalPciBase) ) = device[0].vendorId | (device[0].devId << 16);

    pciInfo->pciIdSel = 6 + i;
    pciInfo->pciBusNum = 0;
    return GT_OK;
}



/*******************************************************************************
* gtPresteraGetPciIntVec
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intVec - PCI interrupt vector.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtPresteraGetPciIntVec
(
    IN  GT_PCI_INT  pciInt,
    OUT void        **intVec
)
{

    /* check parameters */
    if(intVec == NULL)
    {
        return GT_BAD_PARAM;
    }
    /* get the PCI interrupt vector */
    *intVec = (void*)pciInt;
    return GT_OK;
}

/*******************************************************************************
* gtPresteraGetPciIntMask
*
* DESCRIPTION:
*       This routine return the PCI interrupt mask to enable/disable interrupts.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intMask - PCI interrupt mask.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtPresteraGetPciIntMask
(
    IN  GT_PCI_INT  pciInt,
    OUT GT_U32      *intMask
)
{
    GT_STATUS retVal;

    /* check parameters */
    if(intMask == NULL)
    {
        return GT_BAD_PARAM;
    }
    *intMask = 0;
    return GT_OK;
}

#endif

/*******************************************************************************
* gtPresteraGetDefIntrLine
*
* DESCRIPTION:
*       This routine return the default PCI interrupt line on board.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       GT_PCI_INT - PCI interrupt line.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_PCI_INT  gtPresteraGetDefIntrLine(void)
{
    return GT_PCI_INT_D;
}

#define IS_DEVICE_CAN_BE_VB_MUX_DEVICE_MAC(devId)    \
    ((((devId) >> 12) == 0xD) ? 1 :    /*dxCh devices*/ \
     (((devId) >> 8)  == 0x0D) ? 1 :   /*dxSal devices*/\
     0)

#define IS_XCAT_DEV_CPU_ENABLED_MAC(devId)    \
        ((((devId) & 0x2) == 0) ? GT_TRUE : GT_FALSE)

/* init the device[] array                                                    */
/* NOTES :                                                                    */
/* 1.This function will arange the Dx devices as last                         */
/* devices to support VB with ExMx/ExMxPm device with the Dx                  */
/* device(s)                                                                  */
/* 2.The XCAT devices with CPU enabled will be last.                          */
/* This is done to support existing tests for B2B XCAT boards                 */
static GT_STATUS initDevArray(void)
{
    GT_U32 ii,jj,kk;
    GT_U32  devId;
    static GT_BOOL  initWasDone = GT_FALSE;
    static GT_STATUS    firstStatus = GT_OK;

    if(initWasDone == GT_FALSE)
    {
        initWasDone = GT_TRUE;
    }
    else
    {
        return firstStatus;
    }

    /* loop on devices that are 'Non CPSS' devices , and we need to look for the end of them */
    for(ii = 0 ; ii < MAX_DEV_IDS_CNS;ii++)
    {
        if(device[ii].vendorId == 0)
        {
            break;
        }
    }

    if(ii == MAX_DEV_IDS_CNS)
    {
        firstStatus = GT_FULL;
        return firstStatus;
    }

    /* now ii is the index to start the adding to device[] of the cpss devices */


    /***********************************************************************/
    /*on first loop add only devices that can't be use as MUX device in VB */
    /***********************************************************************/

    jj=0;
    while(cpssSupportedTypes[jj].devFamily != END_OF_TABLE)
    {
        kk = 0;
        while(cpssSupportedTypes[jj].devTypeArray[kk] != END_OF_TABLE)
        {
            devId = (cpssSupportedTypes[jj].devTypeArray[kk] >> 16);
            if(IS_DEVICE_CAN_BE_VB_MUX_DEVICE_MAC(devId))
            {
                /* the device can be mux so we skip it on the first loop of devices */
                /* we will add those devices on the 'Second loop' */
                /* XCAT devices can be mux */
                kk++;

                /* don't increment ii here */

                continue;
            }

            device[ii].vendorId = (GT_U16)(cpssSupportedTypes[jj].devTypeArray[kk] & 0xFFFF);
            device[ii].devId    = (GT_U16) devId;

            ii++;
            if(ii == MAX_DEV_IDS_CNS)
            {
                firstStatus = GT_FULL;
                return firstStatus;
            }

            kk++;
        }
        jj++;
    }

    /**********************************************************************/
    /*on second loop add only devices that CAN be use as MUX device in VB */
    /**********************************************************************/
    jj=0;
    while(cpssSupportedTypes[jj].devFamily != END_OF_TABLE)
    {
        kk = 0;
        while(cpssSupportedTypes[jj].devTypeArray[kk] != END_OF_TABLE)
        {
            devId = (cpssSupportedTypes[jj].devTypeArray[kk] >> 16);
            if(!IS_DEVICE_CAN_BE_VB_MUX_DEVICE_MAC(devId) ||
               (cpssSupportedTypes[jj].devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E &&
                IS_XCAT_DEV_CPU_ENABLED_MAC(devId)))

            {
                /* the device CAN'T be mux or XCAT dev with CPU enabled,*/
                /* so we skip it on the second loop of devices */
                kk++;

                /* don't increment ii here */

                continue;
            }
            device[ii].vendorId = (GT_U16)(cpssSupportedTypes[jj].devTypeArray[kk] & 0xFFFF);
            device[ii].devId    = (GT_U16) devId;

            ii++;
            if(ii == MAX_DEV_IDS_CNS)
            {
                firstStatus = GT_FULL;
                return firstStatus;
            }

            kk++;
        }
        jj++;
    }

    /**********************************************************************/
    /*on third loop add XCAT devices with CPU enabled                     */
    /**********************************************************************/
    jj=0;
    while(cpssSupportedTypes[jj].devFamily != END_OF_TABLE)
    {
        kk = 0;
        if (cpssSupportedTypes[jj].devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            jj++;
            continue;
        }
        while(cpssSupportedTypes[jj].devTypeArray[kk] != END_OF_TABLE)
        {
            devId = (cpssSupportedTypes[jj].devTypeArray[kk] >> 16);
            if (!IS_XCAT_DEV_CPU_ENABLED_MAC(devId))
            {
                kk++;
                continue;
            }
            device[ii].vendorId = (GT_U16)(cpssSupportedTypes[jj].devTypeArray[kk] & 0xFFFF);
            device[ii].devId    = (GT_U16) devId;

            ii++;
            if(ii == MAX_DEV_IDS_CNS)
            {
                firstStatus = GT_FULL;
                return firstStatus;
            }

            kk++;
        }
        jj++;
    }

    /* put here ONLY devices that are 'unknown' to the CPSS */
    device[ii].vendorId =  GT_PCI_VENDOR_ID;
    device[ii++].devId    = 0xF950;

    device[ii].vendorId = 0;/* set new 'end of array' */

    firstStatus = GT_OK;
    return firstStatus;
}

