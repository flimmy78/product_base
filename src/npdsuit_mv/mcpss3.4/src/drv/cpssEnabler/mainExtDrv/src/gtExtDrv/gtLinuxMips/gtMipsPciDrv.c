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
* gt88fxx81PciDrv.c
*
* DESCRIPTION:
*       Includes PCI functions wrappers implementation for the 88fxx81 BSP.
*
* DEPENDENCIES:
*       -   System controller.
*       -   BSP.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <gtExtDrv/drivers/gtPciDrv.h>
#include <private/8245/gtCore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

#include "kerneldrv/include/prestera_glob.h"

/*******************************************************************************
* internal definitions
*******************************************************************************/
#ifdef PRESTERA_DEBUG
#define PCI_DRV_DEBUG
#endif

#ifdef PCI_DRV_DEBUG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

extern GT_32 gtPpFd;
extern APP_DEMO_PP_CONFIG appDemoPpConfigList[];

#if 0   /* luoxun -- cpss1.3 */
unsigned int gtInternalRegBaseAddr = 0;
#endif
volatile unsigned int gtInternalRegBaseAddr = 0;


/*******************************************************************************
* extDrvPciConfigWriteReg
*
* DESCRIPTION:
*       This routine write register to the PCI configuration space.
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       regAddr  - Register offset in the configuration space.
*       data     - data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPciConfigWriteReg
(
    IN  GT_U32  busNo,
    IN  GT_U32  devSel,
    IN  GT_U32  funcNo,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    PciConfigReg_STC pciConfReg;
    
    pciConfReg.busNo = busNo;
    pciConfReg.devSel = devSel;
    pciConfReg.funcNo = funcNo;
    pciConfReg.regAddr = regAddr;
    pciConfReg.data = data;
    
    /* call driver function */
    if (ioctl(gtPpFd, PRESTERA_IOC_PCICONFIGWRITEREG, &pciConfReg) < 0)
    {
        return GT_FAIL;
    }
    
    return GT_OK;
}


/*******************************************************************************
* extDrvPciConfigReadReg
*
* DESCRIPTION:
*       This routine read register from the PCI configuration space.
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       regAddr  - Register offset in the configuration space.
*
* OUTPUTS:
*       data     - the read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPciConfigReadReg
(
    IN  GT_U32  busNo,
    IN  GT_U32  devSel,
    IN  GT_U32  funcNo,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    PciConfigReg_STC pciConfReg;
    
    pciConfReg.busNo = busNo;
    pciConfReg.devSel = devSel;
    pciConfReg.funcNo = funcNo;
    pciConfReg.regAddr = regAddr;
    
    /* call driver function */
    if (ioctl(gtPpFd, PRESTERA_IOC_PCICONFIGREADREG, &pciConfReg) < 0)
    {
        return GT_FAIL;
    }
    
    *data = pciConfReg.data;
    
    return GT_OK;
}


/*******************************************************************************
* extDrvPciFindDev
*
* DESCRIPTION:
*       This routine returns the next instance of the given device (defined by
*       vendorId & devId).
*
* INPUTS:
*       vendorId - The device vendor Id.
*       devId    - The device Id.
*       instance - The requested device instance.
*
* OUTPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPciFindDev
(
    IN  GT_U16  vendorId,
    IN  GT_U16  devId,
    IN  GT_U32  instance,
    OUT GT_U32  *busNo,
    OUT GT_U32  *devSel,
    OUT GT_U32  *funcNo
)
{
    GT_PCI_Dev_STC dev;
    
    dev.vendorId = vendorId;
    dev.devId    = devId;
    dev.instance = instance;
    
    /* call driver function */
    if (ioctl(gtPpFd, PRESTERA_IOC_FIND_DEV, &dev) < 0)
    {
        return GT_FAIL;
    }
    
    *busNo  = dev.busNo;
    *devSel = dev.devSel;
    *funcNo = dev.funcNo;
    
    return GT_OK;
}


/*******************************************************************************
* extDrvGetPciIntVec
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
GT_STATUS extDrvGetPciIntVec
(
	#if 0
    IN  GT_PCI_INT  pciInt,
    #else
	IN	unsigned int devIdx,
	#endif
	OUT void        **intVec
)
{
    GT_Intr2Vec int2vec 	= {0};
	GT_PCI_INFO *pciInfo 	= &(appDemoPpConfigList[devIdx].pciInfo);
    
    /* check parameters */
    if(intVec == NULL)
    {
        return GT_BAD_PARAM;
    }
    /* get the PCI interrupt vector */
    /* call driver function */
	#if 0
    int2vec.intrLine = (GT_U32)pciInt;
    int2vec.bus = 0;
    int2vec.device = 0;
    int2vec.vector = 0;
	#else
    int2vec.intrLine	= GT_PCI_INT_A;
    int2vec.busNo 		= pciInfo->pciBusNum;
    int2vec.deviceNo	= pciInfo->pciIdSel;
	int2vec.funcNo		= pciInfo->funcNo;
    int2vec.vector 		= 0;
	#endif
	if (ioctl(gtPpFd, PRESTERA_IOC_GETINTVEC, &int2vec) < 0)
    {
        return GT_FAIL;
    }
    *intVec = (void *) int2vec.vector;
    
    /* check whether a valid value */
    if((*intVec) == NULL)
    {
        DBG_INFO(("Failed in gtPciIntrToVecNum\n"));
        return GT_FAIL;
    }
    return GT_OK;
}


/*******************************************************************************
* extDrvGetIntMask
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
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
*       PCI interrupt mask should be used for interrupt disable/enable.
*
*******************************************************************************/
GT_STATUS extDrvGetIntMask
(
	#if 0
    IN  GT_PCI_INT  pciInt,
	#else
	IN	unsigned int devIdx,
	#endif
    OUT GT_U32      *intMask
)
{
    void        *intVec;

    /* check parameters */
    if(intMask == NULL)
    {
        return GT_BAD_PARAM;
    }
    /* get the PCI interrupt vector */
    extDrvGetPciIntVec(devIdx, &intVec);

    *intMask = (GT_U32)intVec;

    return GT_OK;
}

#ifndef __AX_PLATFORM__
/*******************************************************************************
* extDrvGetPciIntVecForMultiGroupDev
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
*
* INPUTS:
*       devIdx  - Device Index.
*       GroupId - Device GroupId.
*
* OUTPUTS:
*       intVec - PCI interrupt vector.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       This function is special for multi-groups device,
*       such as Lion.                  luoxun@autelan.com
*
*******************************************************************************/
GT_STATUS extDrvGetPciIntVecForMultiGroupDev
(
	IN	unsigned int devIdx,
	IN  unsigned int GroupId,
	OUT void        **intVec
)
{
    GT_Intr2Vec int2vec 	= {0};
	GT_PCI_INFO *pciInfo 	= NULL;

    /* check parameters */
    if(intVec == NULL)
    {
        osPrintfErr("extDrvGetPciIntVecForMultiGroup parameter error: intVec == NULL\n");
        return GT_BAD_PARAM;
    }

    if (0 == appDemoPpConfigList[devIdx].valid)
    {
        osPrintfErr("extDrvGetPciIntVecForMultiGroup parameter error: devIdx %d is not exist!\n", devIdx);
        return GT_BAD_PARAM;
    }

    if (0 == appDemoPpConfigList[devIdx].numOfPortGroups)
    {
        osPrintfErr("extDrvGetPciIntVecForMultiGroup parameter error: devIdx %d has 0 group!\n", devIdx);
        return GT_BAD_PARAM;
    }

    if (GroupId >= appDemoPpConfigList[devIdx].numOfPortGroups)
    {
        osPrintfErr("extDrvGetPciIntVecForMultiGroup parameter error: GroupId %d is overflowing!\n", GroupId);
        return GT_BAD_PARAM;
    }

    pciInfo = &(appDemoPpConfigList[devIdx].portGroupsInfo[GroupId].portGroupPciInfo);
    
    
    /* get the PCI interrupt vector */
    int2vec.intrLine	= GT_PCI_INT_A;
    int2vec.busNo 		= pciInfo->pciBusNum;
    int2vec.deviceNo	= pciInfo->pciIdSel;
	int2vec.funcNo		= pciInfo->funcNo;
    int2vec.vector 		= 0;

    osPrintfDbg("extDrvGetPciIntVec\n");
    osPrintfDbg("   busNo    = %d\n", pciInfo->pciBusNum);
    osPrintfDbg("   deviceNo = %d\n", pciInfo->pciIdSel);
    osPrintfDbg("   funcNo   = %d\n", pciInfo->funcNo);
    
    if (ioctl(gtPpFd, PRESTERA_IOC_GETINTVEC, &int2vec) < 0)
    {
        return GT_FAIL;
    }
    *intVec = (void *) int2vec.vector;
    
    /* check whether a valid value */
    if((*intVec) == NULL)
    {
        DBG_INFO(("Failed in gtPciIntrToVecNum\n"));
        return GT_FAIL;
    }
    return GT_OK;
}

/*******************************************************************************
* extDrvGetIntMaskForMultiGroupDev
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
*
* INPUTS:
*       devIdx  - Device Index.
*       GroupId - Device GroupId.
*
* OUTPUTS:
*       intMask - PCI interrupt mask.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       PCI interrupt mask should be used for interrupt disable/enable.
*       This function is special for multi-groups device,
*       such as Lion.                  luoxun@autelan.com
*
*******************************************************************************/
GT_STATUS extDrvGetIntMaskForMultiGroupDev
(
	IN	unsigned int devIdx,
	IN  unsigned int GroupId,
    OUT GT_U32      *intMask
)
{
    void        *intVec;

    /* check parameters */
    if(intMask == NULL)
    {
        return GT_BAD_PARAM;
    }
    
    /* get the PCI interrupt vector */
    extDrvGetPciIntVecForMultiGroupDev(devIdx, GroupId, &intVec);
    *intMask = (GT_U32)intVec;

    return GT_OK;
}

#endif

/*******************************************************************************
* extDrvEnableCombinedWrites
*
* DESCRIPTION:
*       This function enables / disables the Pci writes / reads combining
*       feature.
*       Some system controllers support combining memory writes / reads. When a
*       long burst write / read is required and combining is enabled, the master
*       combines consecutive write / read transactions, if possible, and
*       performs one burst on the Pci instead of two. (see comments)
*
* INPUTS:
*       enWrCombine - GT_TRUE enables write requests combining.
*       enRdCombine - GT_TRUE enables read requests combining.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on sucess,
*       GT_NOT_SUPPORTED    - if the controller does not support this feature,
*       GT_FAIL             - otherwise.
*
* COMMENTS:
*       1.  Example for combined write scenario:
*           The controller is required to write a 32-bit data to address 0x8000,
*           while this transaction is still in progress, a request for a write
*           operation to address 0x8004 arrives, in this case the two writes are
*           combined into a single burst of 8-bytes.
*
*******************************************************************************/
GT_STATUS extDrvEnableCombinedPciAccess
(
    IN  GT_BOOL     enWrCombine,
    IN  GT_BOOL     enRdCombine
)
{
    return GT_OK;
}

/*******************************************************************************
* extDrvPciDoubleWrite
*
* DESCRIPTION:
*        This routine will write a 64-bit data  to given address
*
* INPUTS:
*        address - address to write to
*       word1 - the first half of double word to write (MSW)
*       word2 - the second half of double word to write (LSW)
*
* OUTPUTS:
*      none
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPciDoubleWrite
(
    IN  GT_U32 address,
    IN  GT_U32 word1, 
    IN  GT_U32 word2
)
{
	return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* extDrvPciDoubleRead
*
* DESCRIPTION:
*        This routine will read a 64-bit data  from given address
*
* INPUTS:
*        address - address to read from
*
* OUTPUTS:
*       data     -  pointer for the received data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvPciDoubleRead
(
    IN  GT_U32  address,
    OUT GT_U64  *dataPtr
)
{
	return GT_NOT_SUPPORTED;
}

#ifndef __AX_PLATFORM__
/*******************************************************************************
* extDrvGetDeviceDmaBase
*
* DESCRIPTION:
*       This routine return the dma base address for both CPU and device access.
*
* INPUTS:
*       busNo 	- The device bus number.
*       devSel  	- The device index.
*       funcNo 	- The device function index.
*
* OUTPUTS:
*       dma_base	- DMA base address remapped(access by  CPU).
*       device_base	- device's DMA base address(access by device),
*				    this address will be writen to device's config space(e.g. via RxDesc or TxDesc).
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvGetDeviceDmaBase
(
	IN 	GT_U32 	busNo,
	IN 	GT_U32 	devSel,
	IN	GT_U32	funcNo,
	OUT	GT_U32	*dma_base,
	OUT unsigned int *deviceDmaBase
)
{
	GT_Dma_Base_STC dmaBaseSTC = {0};

	dmaBaseSTC.busNo	= busNo;
	dmaBaseSTC.devSel	= devSel;
	dmaBaseSTC.funcNo	= funcNo;
	
    if (ioctl(gtPpFd, PRESTERA_IOC_GETDEVBASEADDR, &dmaBaseSTC) < 0)
    {
        return GT_FAIL;
    }

	*dma_base 			= dmaBaseSTC.dma_base;
	*deviceDmaBase 		= dmaBaseSTC.device_base;
	
    return GT_OK;
}


/* read destinated address from kernel space */
GT_STATUS kread
(
	IN	unsigned long address
)
{
	GT_RAWIOWR_STC pciRawIOWR = {0};

	pciRawIOWR.address = address;
	pciRawIOWR.isRead = 1;
	osPrintf("before kread at 0x%016lx\r\n",address);
    if (ioctl(gtPpFd, PRESTERA_IOC_RAWMEMRW, &pciRawIOWR) < 0)
    {
        return GT_FAIL;
    }
	osPrintf("after  kread at 0x%016lx got 0x%016lx\r\n",pciRawIOWR.address,pciRawIOWR.data);
    return GT_OK;
}

/* write data to destinated address in kernel space */
GT_STATUS kwrite
(
	IN	unsigned long address,
	IN  unsigned long data
)
{
	GT_RAWIOWR_STC pciRawIOWR = {0};

	pciRawIOWR.address 	= address;
	pciRawIOWR.data 	= data;
	pciRawIOWR.isRead 	= 0;
	osPrintf("before kwrite 0x%016lx to 0x%016lx\r\n",data,address);
    if (ioctl(gtPpFd, PRESTERA_IOC_RAWMEMRW, &pciRawIOWR) < 0)
    {
        return GT_FAIL;
    }
	osPrintf("after  kwrite 0x%016lx to 0x%016lx\r\n",pciRawIOWR.data,pciRawIOWR.address);
    return GT_OK;
}

/* read destinated address from user space */
GT_STATUS uread
(
	IN	unsigned long address
)
{
	unsigned long data;
	data = (unsigned long)(*(volatile unsigned long *)address);

	osPrintf("before uread at 0x%016lx\r\n",address);

	osPrintf("after  uread at 0x%016lx got 0x%016lx\r\n",address,data);
    return GT_OK;
}

/* write data to destinated address in user space */
GT_STATUS uwrite
(
	IN	unsigned long address,
	IN  unsigned long data
)
{
	*(volatile unsigned long*)address = data;
	osPrintf("before uwrite 0x%016lx to 0x%016lx\r\n",data,address);
	data = (unsigned long)(*(volatile unsigned long *)address);	
	osPrintf("after  write64 to 0x%016lx got 0x%016lx\r\n",address,data);
    return GT_OK;
}

GT_STATUS ugetpktstat
(
	IN GT_PKG_COUNT_STAT* stat
)
{
	int rc = 0;
	
	rc = ioctl(gtPpFd,PRESTERA_IOC_GETPKGSTACOUNT,stat);
	if(rc < 0) {
		osPrintf("prestera ioctl error\n");
		rc = GT_FAIL;
	}

	return rc;
}

GT_STATUS uclearpktstat
(
	IN unsigned char dev,
	IN unsigned char port
)
{
	GT_STATUS rc = GT_OK;
	GT_DEV_PORT dev_port_s = {0};
	
	dev_port_s.devNum = dev;
	dev_port_s.portNum = port;
	
	rc = ioctl(gtPpFd,PRESTERA_IOC_CLAERPKGCOUNT,&dev_port_s);
	if(rc < 0) {
		osPrintf("prestera ioctl error\n");
		rc = GT_FAIL;
	}
	
	return rc;
}

/*******************************************************************************
* extDrvPciConfigWriteReg
*
* DESCRIPTION:
*       This routine read register to the CPU configuration register.
*
* INPUTS:
*       regAddr  - Register offset of CPU Register set
*
* OUTPUTS:
*       data     - data read from.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
int extDrvBoardCpuReadReg
(
    IN  unsigned int  regAddr,
    IN  unsigned int  *data
)
{
    	GT_CpuRegReadWrite_STC boardCpuRegCfg;

	memset(&boardCpuRegCfg, 0, sizeof(GT_CpuRegReadWrite_STC));
	
    	boardCpuRegCfg.regAddr = regAddr;
    
	/* call driver function */
    	if (ioctl(gtPpFd, PRESTERA_IOC_CPUREGREAD, &boardCpuRegCfg) < 0)
   	{
        	return GT_FAIL;
    	}

	if(boardCpuRegCfg.opResult) {
		return GT_FAIL;
	}

	*data = boardCpuRegCfg.value;	
	osPrintfDbg("driver read cpu register %#x value %#x %s\n", boardCpuRegCfg.regAddr, boardCpuRegCfg.value, \
			boardCpuRegCfg.opResult ? "fail":"ok");
	
    	return GT_OK;
}

/*******************************************************************************
* extDrvBoardCpuWriteReg
*
* DESCRIPTION:
*       This routine write register from the CPU configuration register.
*
* INPUTS:
*       regAddr  - Register offset of CPU Register set
*       data     - data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
int extDrvBoardCpuWriteReg
(
    IN  unsigned int  regAddr,
    IN  unsigned int  data
)
{
    GT_CpuRegReadWrite_STC boardCpuRegCfg;

	memset(&boardCpuRegCfg, 0, sizeof(GT_CpuRegReadWrite_STC));
	
    boardCpuRegCfg.regAddr = regAddr;
	boardCpuRegCfg.value = data;
    
    /* call driver function */
    if (ioctl(gtPpFd, PRESTERA_IOC_CPUREGWRITE, &boardCpuRegCfg) < 0)
    {
        return GT_FAIL;
    }

	if(boardCpuRegCfg.opResult) {
		return GT_FAIL;
	}
    
	osPrintfDbg("driver write cpu register %#x data %#x %s\n", boardCpuRegCfg.regAddr, boardCpuRegCfg.value, \
			boardCpuRegCfg.opResult ? "fail":"ok");

    return GT_OK;
}

/*******************************************************************************
* extDrvBoardGpioResetPhy
*
* DESCRIPTION:
*       This routine configure the CPU GPIO 45 to control phy reset operation and
*       driven bit sequence 1 -> 0 -> 1 on GPIO 45 to reset phy  
*       It's done in the following sequence:
*           1.> config GPIO 45 to control phy reset
*	    2.> output GPIO 45 value 1
*	    3.> output GPIO 45 value 0
*	    4.> output GPIO 45 value 1
*
* INPUTS:
*	None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
void extDrvBoardGpioResetPhy
(
	void
)
{
    unsigned int data = 0, regAddr = 0;
    int result  = 0;

    /* get GPIO Data Out Enable Control Register */
    regAddr = GPP_DATA_OUT_EN_REG(45/GPP_COUNT_IN_GROUP);

    /* read current control setting */
    if(GT_OK != (result = extDrvBoardCpuReadReg(regAddr ,&data))) {
    	osPrintfErr("GPIO reset phy read DATA_OUT_EN register %#x error\n", regAddr);
	return;
    }
    /* set GPIO 45 enable in GPIO High Data Out Enable Control Register */
    data &= (~(1<<13));
    if(GT_OK != (result = extDrvBoardCpuWriteReg(regAddr, data))) {
    	osPrintfErr("GPIO reset phy write DATA_OUT_EN register %#x %#x error\n", regAddr, data);
	return;
    }

    /* get GPIO Data Out Register */
    regAddr = GPP_DATA_OUT_REG(45/GPP_COUNT_IN_GROUP);

    /* read current control setting */
    if(GT_OK != (result = extDrvBoardCpuReadReg(regAddr ,&data))) {
    	osPrintfErr("GPIO reset phy read DATA_OUT register %#x error\n", regAddr);
	return;
    }
    /* driven bit sequence 1 -> 0 -> 1 to reset phy */
    data |= (1<<13);
    if(GT_OK != (result = extDrvBoardCpuWriteReg(regAddr, data))) {
    	osPrintfErr("GPIO reset phy write DATA_OUT 1 register %#x %#x error\n", regAddr, data);
	return;
    }
    data &= (~(1<<13));
    if(GT_OK != (result = extDrvBoardCpuWriteReg(regAddr, data))) {
    	osPrintfErr("GPIO reset phy write DATA_OUT 1->0 register %#x %#x error\n", regAddr, data);
	return;
    }
    data |= (1<<13);
    if(GT_OK != (result = extDrvBoardCpuWriteReg(regAddr, data))) {
    	osPrintfErr("GPIO reset phy write DATA_OUT 1->0->1 register %#x %#x error\n", regAddr, data);
	return;
    }

    return ;
}


#endif
