/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvPpHwApi.c
*
* DESCRIPTION:
*       Prestera driver Hardware API
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
* External usage environment parameters
*******************************************************************************/
GT_BOOL memoryAccessTraceOn = GT_FALSE;
/*******************************************************************************
* prvCpssDrvHwCntlInit
*
* DESCRIPTION:
*       This function initializes the Hw control structure of a given PP.
*
* INPUTS:
*       devNum          - The PP's device number to init the structure for.
*       portGroupId     - The port group id.
*                          relevant only to 'multi-port-group' devices.
*                          supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       baseAddr        - The PP base address on the host interface bus.
*       internalPciBase - Base address to which the internal pci registers
*                         are mapped to.
*       doByteSwap      - Should the register's data be byte swapped (GT_TRUE),
*                         or not (GT_FALSE).
*       isDiag          - Is this initialization is for diagnostics purposes
*                         (GT_TRUE), or is it a final initialization of the Hw
*                         Cntl unit (GT_FALSE)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       1.  In case isDiag == GT_TRUE, no semaphores are initialized.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwCntlInit
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   baseAddr,
    IN GT_U32   internalPciBase,
    IN GT_BOOL  doByteSwap,
    IN GT_BOOL  isDiag
)
{
    GT_STATUS rc;

    /* check if driver object is ready */
    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.isMultiPortGroupDevice == GT_FALSE)
    {
        portGroupId = CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS;
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.numOfPortGroups        = 1;
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.activePortGroupsBmp    = BIT_0;
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.firstActivePortGroup   = CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS;
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.lastActivePortGroup    = CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS;

        prvCpssDrvPpConfig[devNum]->intCtrl.
            portGroupInfo[CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS].
                isrCookieInfo.devNum = devNum;
        prvCpssDrvPpConfig[devNum]->intCtrl.
            portGroupInfo[CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS].
                isrCookieInfo.portGroupId = CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS;
    }
    else if(0 == PRV_CPSS_DRV_IS_IN_RANGE_PORT_GROUP_ID_MAC(devNum,portGroupId))
    {
        /* out of range parameter */
        return GT_BAD_PARAM;
    }
    else /* multi port groups device */
    {
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.numOfPortGroups++;
        if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.numOfPortGroups >= 32)
        {
            /* limit to 32 due to current GT_U32 as bitmap */
            /* for active port groupss                     */
            return GT_FULL;
        }

        prvCpssDrvPpConfig[devNum]->portGroupsInfo.activePortGroupsBmp    |= (1 << portGroupId);

        /* check if need to update the firstActivePortGroup */
        if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.firstActivePortGroup > portGroupId)
        {
            prvCpssDrvPpConfig[devNum]->portGroupsInfo.firstActivePortGroup = portGroupId;
        }

        /* check if need to update the lastActivePortGroup */
        if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.lastActivePortGroup < portGroupId)
        {
            prvCpssDrvPpConfig[devNum]->portGroupsInfo.lastActivePortGroup = portGroupId;
        }

        prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].isrCookieInfo.devNum = devNum;
        prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].isrCookieInfo.portGroupId = portGroupId;

        /* initializer the 'force' portGroupId to 'Not forcing' */
        prvCpssDrvPpConfig[devNum]->portGroupsInfo.debugForcedPortGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwCntlInit(devNum,portGroupId,
                                  baseAddr,internalPciBase,doByteSwap,isDiag);
    return rc;
}


/*******************************************************************************
* prvCpssDrvHwPpReadRegister
*
* DESCRIPTION:
*       Read a register value from the given PP.
*
* INPUTS:
*       devNum  - The PP to read from.
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       data - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
    return prvCpssDrvHwPpPortGroupReadRegister(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,data);
}


/*******************************************************************************
* prvCpssDrvHwPpWriteRegister
*
* DESCRIPTION:
*       Write to a PP's given register.
*
* INPUTS:
*       devNum  - The PP to write to.
*       regAddr - The register's address to write to.
*       data    - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteRegister
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    return prvCpssDrvHwPpPortGroupWriteRegister(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,value);
}

#ifndef __AX_PLATFORM__
GT_STATUS readreg
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr
)
{
    GT_STATUS rc;
	  unsigned int data;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);

    #if 0   /* luoxun -- cpss1.3 */
    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadReg(devNum,regAddr,&data);
    #endif
    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadReg(devNum,0,regAddr,&data);
	osPrintf("readreg::dev %d reg %#0x data %#0x\r\n",devNum,regAddr,data);
    return rc;
}

GT_STATUS writereg
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN unsigned int value
)

{
    GT_STATUS rc;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);

    #if 0   /* luoxun -- cpss1.3 */
    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteReg(devNum,regAddr,value);
    #endif
    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteReg(devNum,0,regAddr,value);

     return rc;
}
#endif

/*******************************************************************************
* prvCpssDrvHwPpGetRegField
*
* DESCRIPTION:
*       Read a selected register field.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       regAddr - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*
* OUTPUTS:
*       fieldData   - Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
)
{
    return prvCpssDrvHwPpPortGroupGetRegField(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr,fieldOffset,fieldLength,fieldData);
}


/*******************************************************************************
* prvCpssDrvHwPpSetRegField
*
* DESCRIPTION:
*       Write value to selected register field.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       regAddr - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       this function actually read the register modifies the requested field
*       and writes the new value back to the HW.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpSetRegField
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 fieldData

)
{
    return prvCpssDrvHwPpPortGroupSetRegField(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr,fieldOffset,fieldLength,fieldData);
}


/*******************************************************************************
* prvCpssDrvHwPpReadRegBitMask
*
* DESCRIPTION:
*       Reads the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to read from.
*       regAddr - Register address to read from.
*       mask    - Mask for selecting the read bits.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
)
{
    return  prvCpssDrvHwPpPortGroupReadRegBitMask(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr,mask,dataPtr);
}


/*******************************************************************************
* prvCpssDrvHwPpWriteRegBitMask
*
* DESCRIPTION:
*       Writes the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to write to.
*       regAddr - Register address to write to.
*       mask    - Mask for selecting the written bits.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteRegBitMask
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 value
)
{
    return prvCpssDrvHwPpPortGroupWriteRegBitMask(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        regAddr,mask,value);
}


/*******************************************************************************
* prvCpssDrvHwPpReadRam
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       addr    - Address offset to read from.
*       length  - Number of Words (4 byte) to read.
*
* OUTPUTS:
*       data    - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpReadRam
(
    IN GT_U8    devNum,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *data
)
{
    return prvCpssDrvHwPpPortGroupReadRam(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr,length,data);
}


/*******************************************************************************
* prvCpssDrvHwPpWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       addr    - Address offset to write to.
*       length  - Number of Words (4 byte) to write.
*       data    - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteRam
(
    IN GT_U8  devNum,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
)
{
    return prvCpssDrvHwPpPortGroupWriteRam(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr,length,data);
}


/*******************************************************************************
* prvCpssDrvHwPpReadVec
*
* DESCRIPTION:
*       Read from PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       addrArr - Address array to read from.
*       arrLen  - The size of addrArr/dataArr.
*
* OUTPUTS:
*       dataArr - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpReadVec
(
    IN GT_U8    devNum,
    IN GT_U32   addrArr[],
    OUT GT_U32  dataArr[],
    IN GT_U32   arrLen
)
{
    return prvCpssDrvHwPpPortGroupReadVec(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addrArr,dataArr,arrLen);
}


/*******************************************************************************
* prvCpssDrvHwPpWriteVec
*
* DESCRIPTION:
*       Writes to PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       addrArr - Address offset to write to.
*       dataArr - An array containing the data to be written.
*       arrLen  - The size of addrArr/dataArr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteVec
(
    IN GT_U8    devNum,
    IN GT_U32   addrArr[],
    IN GT_U32   dataArr[],
    IN GT_U32   arrLen
)
{
    return prvCpssDrvHwPpPortGroupWriteVec(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addrArr,dataArr,arrLen);
}


/*******************************************************************************
* prvCpssDrvHwPpWriteRamInReverse
*
* DESCRIPTION:
*       Writes to PP's RAM in reverse.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       addr    - Address offset to write to.
*       length  - Number of Words (4 byte) to write.
*       data    - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteRamInReverse
(
    IN GT_U8 devNum,
    IN GT_U32 addr,
    IN GT_U32 length,
    IN GT_U32 *data
)
{
    return prvCpssDrvHwPpPortGroupWriteRamInReverse(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,addr,length,data);
}

/*******************************************************************************
* prvCpssDrvHwPpIsrRead
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*
* INPUTS:
*       devNum  - The PP to read from.
*       regAddr - The register's address to read from.
*                 Note: regAddr should be < 0x1000000
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_INLINE GT_STATUS prvCpssDrvHwPpIsrRead
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
)
{
    return prvCpssDrvHwPpPortGroupIsrRead(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,dataPtr);
}
/*******************************************************************************
* prvCpssDrvHwPpIsrWrite
*
* DESCRIPTION:
*       Write a register value using special interrupt address completion region
*
* INPUTS:
*       devNum  - The PP to write to.
*       regAddr - The register's address to write to.
*       data    - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpIsrWrite
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    return prvCpssDrvHwPpPortGroupIsrWrite(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,value);
}
/*******************************************************************************
* prvCpssDrvHwPpReadInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       data    - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpReadInternalPciReg
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    return prvCpssDrvHwPpPortGroupReadInternalPciReg(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,data);
}


/*******************************************************************************
* prvCpssDrvHwPpWriteInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       regAddr - The register's address to read from.
*       data    - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpWriteInternalPciReg
(
    IN  GT_U8   devNum,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    return prvCpssDrvHwPpPortGroupWriteInternalPciReg(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,data);
}

/*******************************************************************************
* prvCpssDrvHwByteSwap
*
* DESCRIPTION:
*       This function performs a byte swap on a given word IF the PCI bus swap
*       the info from the PP - otherwise the function do nothing
*
* INPUTS:
*       devNum  - device on which the byte swap is performed.
*       data    - The data to be byte swapped.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The data after performing byte-swapping (if needed).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_INLINE GT_U32 prvCpssDrvHwByteSwap
(
    IN  GT_U8   devNum,
    IN  GT_U32  data
)
{
    return prvCpssDrvHwPpPortGroupByteSwap(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,data);
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupReadRegister
*
* DESCRIPTION:
*       Read a register value from the given PP.
*       in the specific port group in the device
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*
* OUTPUTS:
*       data - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
)
{
    GT_STATUS rc;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadReg(
        devNum, portGroupId, regAddr, data);
    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteRegister
*
* DESCRIPTION:
*       Write to a PP's given register.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to write to.
*       data        - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum,portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if (portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteReg(
                devNum,portGroupId,regAddr,value);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    }
    else
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteReg(
            devNum, portGroupId, regAddr, value);

		/* hwPpPciWriteRegister() */
    }

    return rc;
}



/*******************************************************************************
* prvCpssDrvHwPpPortGroupGetRegField
*
* DESCRIPTION:
*       Read a selected register field.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*
* OUTPUTS:
*       fieldData   - Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
)
{
    GT_STATUS rc;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpGetRegField(
        devNum, portGroupId,
        regAddr,fieldOffset,fieldLength,fieldData);

     return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupSetRegField
*
* DESCRIPTION:
*       Write value to selected register field.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupSetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum,portGroupId);


    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpSetRegField(
                devNum, portGroupId,
                regAddr,fieldOffset,fieldLength,fieldData);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
    }
    else
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpSetRegField(
            devNum, portGroupId,
            regAddr,fieldOffset,fieldLength,fieldData);
    }

     return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupReadRegBitMask
*
* DESCRIPTION:
*       Reads the unmasked bits of a register.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - Register address to read from.
*       mask        - Mask for selecting the read bits.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
* GalTis:
*       Command - hwPpReadRegBitMask
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
)
{
    GT_STATUS rc;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadRegBitMask(
        devNum, portGroupId, regAddr, mask, dataPtr);
    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteRegBitMask
*
* DESCRIPTION:
*       Writes the unmasked bits of a register.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - Register address to write to.
*       mask        - Mask for selecting the written bits.
*       value       - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
* GalTis:
*       Command - hwPpWriteRegBitMask
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    IN GT_U32   value
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address */
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 regAddr,
                                                                 1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteRegBitMask(
                devNum, portGroupId, regAddr, mask, value);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
    }
    else
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteRegBitMask(
            devNum, portGroupId, regAddr, mask, value);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupReadRam
*
* DESCRIPTION:
*       Read from PP's RAM.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr        - Address offset to read from.
*       length      - Number of Words (4 byte) to read.
*
* OUTPUTS:
*       data    - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* GalTis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupReadRam
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *data
)
{
    GT_STATUS rc;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address range*/
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 addr,
                                                                 length);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadRam(
        devNum, portGroupId, addr, length, data);
    return rc;
}


/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM.
*       in the specific port group in the device
*
* INPUTS:
*       devNum     - The PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr       - Address offset to write to.
*       length     - Number of Words (4 byte) to write.
*       data       - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* GalTis:
*       Table - PPWrite
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteRam
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address range*/
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 addr,
                                                                 length);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteRam(
                devNum, portGroupId, addr, length, data);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
    }
    else
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteRam(
            devNum, portGroupId, addr, length, data);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteRamInReverse
*
* DESCRIPTION:
*       Writes to PP's RAM in reverse.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr        - Address offset to write to.
*       length      - Number of Words (4 byte) to write.
*       data        - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteRamInReverse
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address range*/
        rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                 portGroupId,
                                                                 addr,
                                                                 length);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if( portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteRamRev(
                devNum, portGroupId, addr, length, data);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
    }
    else
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteRamRev(
            devNum, portGroupId, addr, length, data);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupReadVec
*
* DESCRIPTION:
*       Read from PP's RAM a vector of addresses.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addrArr     - Address array to read from.
*       arrLen      - The size of addrArr/dataArr.
*
* OUTPUTS:
*       dataArr - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupReadVec
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    OUT GT_U32  dataArr[],
    IN GT_U32   arrLen
)
{
    GT_U32      i;
    GT_STATUS   rc;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);

    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address ranges */
        for (i = 0; i < arrLen; i++)
        {
            rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                     portGroupId,
                                                                     addrArr[i],
                                                                     1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadVec(
        devNum, portGroupId, addrArr, dataArr, arrLen);
    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteVec
*
* DESCRIPTION:
*       Writes to PP's RAM a vector of addresses.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addrArr     - Address offset to write to.
*       dataArr     - An array containing the data to be written.
*       arrLen      - The size of addrArr/dataArr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteVec
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    IN GT_U32   dataArr[],
    IN GT_U32   arrLen
)
{
    GT_U32      i;
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);


    if (prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr != NULL)
    {
        /* check address ranges */
        for (i = 0; i < arrLen; i++)
        {
            rc = prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr(devNum,
                                                                     portGroupId,
                                                                     addrArr[i],
                                                                     1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
        {

            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteVec(
                devNum, portGroupId, addrArr, dataArr, arrLen);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
    }
    else
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteVec(
            devNum, portGroupId, addrArr, dataArr, arrLen);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupIsrRead
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*                     Note - regAddr should be < 0x1000000
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* GalTis:
*       None.
*
*******************************************************************************/
GT_INLINE GT_STATUS prvCpssDrvHwPpPortGroupIsrRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   *dataPtr
)
{
    GT_STATUS rc;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);

    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpIsrRead(
        devNum, portGroupId, regAddr, dataPtr);

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupIsrWrite
*
* DESCRIPTION:
*       Write a register value using special interrupt address completion region
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to write to.
*       data        - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* GalTis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupIsrWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpIsrWrite(
                devNum, portGroupId, regAddr, value);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
    }
    else
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpIsrWrite(
            devNum, portGroupId, regAddr, value);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupByteSwap
*
* DESCRIPTION:
*       This function performs a byte swap on a given word IF the PCI bus swap
*       the info from the PP - otherwise the function do nothing
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - device on which the byte swap is performed.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       data        - The data to be byte swapped.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The data after performing byte-swapping (if needed).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_INLINE GT_U32 prvCpssDrvHwPpPortGroupByteSwap
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  data
)
{
    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);

    return
        (prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].doByteSwap
         == GT_TRUE)
        ? (BYTESWAP_MAC(data))
        : (data);
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupReadInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*       in the specific port group in the device
*
* INPUTS:
*       devNum     - The Pp's device numbers.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr    - The register's address to read from.
*
* OUTPUTS:
*       data    - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupReadInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
)
{
    GT_STATUS rc;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(
        devNum, portGroupId);

    rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpReadIntPciReg(
        devNum, portGroupId, regAddr, data);

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupWriteInternalPciReg
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The Pp's device numbers.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*       data        - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupWriteInternalPciReg
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
    GT_STATUS rc = GT_OK;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, portGroupId);

    if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
        {
            rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteIntPciReg(
                devNum, portGroupId, regAddr, data);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum, portGroupId)
    }
    else
    {
        rc = prvCpssDrvPpObjConfig[devNum]->busPtr->drvHwPpWriteIntPciReg(
            devNum, portGroupId, regAddr, data);
    }

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpPortGroupDebugForcePortGroupId
*
* DESCRIPTION:
*       debug tool --> force port group Id for all operations in the cpssDriver level
* INPUTS:
*       devNum             - The PP to read from.
*       forcedPortGroupId  - The port group Id to force.
*                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*                            --> remove the forcing.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - non-multi port groups device
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPortGroupDebugForcePortGroupId
(
    IN GT_U8    devNum,
    IN GT_U32   forcedPortGroupId
)
{
    /* must save the value before call PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(...) */
    GT_U32  copyOfPortGroupId = forcedPortGroupId;

    PRV_CPSS_DRV_CHECK_BUS_FUNC_OBJ_MAC(devNum);
    /* check the forcedPortGroupId value */
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum, copyOfPortGroupId);

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) == GT_FALSE)
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    /* set the new 'debugForcedPortGroupId' */
    prvCpssDrvPpConfig[devNum]->portGroupsInfo.debugForcedPortGroupId =
        forcedPortGroupId;

    return GT_OK;
}




/*******************************************************************************
* prvCpssDrvHwPpMemoryAccessTraceEnableSet
*
* DESCRIPTION:
*       debug tool --> make debug prints for all memory access in the cpssDriver level
* INPUTS:
*       enable      - GT_TRUE  - enable memory access debug prints
*                     GT_FALSE - disable memory access debug prints is off
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpMemoryAccessTraceEnableSet
(
    IN GT_BOOL    enable
)
{
    memoryAccessTraceOn = enable;
    return GT_OK;
}
