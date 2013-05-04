/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwRegisters.c
*
* DESCRIPTION:
*       Private API definition for register access of the DXCH devices.
*
*       Those APIs gives next benefit :
*       for Lion (multi-port group device) with TXQ ver 1 (or above)
*       - write of 'global config' into the TXQ may need to we written only in
*       some of the port groups (not all)
*       - write of 'per port' may need to we written to other port group then
*       specified according to 'dest port'
*
*       still SOME functionality MUST NOT use those APIs , like:
*       - interrupts handling --> needed per specific port group
*       - prvCpssDxChPortGroupsCounterSummary(...) , prvCpssDxChPortGroupBusyWait(...)
*          and maybe others...
*       - maybe others
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* indication of 'no change' in the port group Id */
#define PORT_GROUPS_NO_CHANGE_CNS   0

/* BMP for Lion , for TXQ SHT sub unit , that need to be duplicated to port
   groups 0,1*/
#define LION_TXQ_SHT_UNIT_PORT_GROUPS_BMP_CNS   0x3

/* BMP for Lion , for TXQ DQ sub unit -- 'no change'
   Each packet is served by one of these four sub-units,
   based on the target port , so info is unique per port group */
#define LION_TXQ_DQ_UNIT_PORT_GROUPS_BMP_CNS    PORT_GROUPS_NO_CHANGE_CNS

/*
   BMP for Lion , for TXQ EGR sub unit , must be duplicated to all port groups
   because serve traffic according to INGRESS port
*/
#define LION_TXQ_EGR_UNIT_PORT_GROUPS_BMP_CNS   CPSS_PORT_GROUP_UNAWARE_MODE_CNS


/*
   BMP for Lion , for TXQ queue sub unit , single port group 0
*/
#define LION_TXQ_QUEUE_UNIT_PORT_GROUPS_BMP_CNS     0x1

/*
   BMP for Lion , for TXQ LL sub unit , single port group 0
*/
#define LION_TXQ_LL_UNIT_PORT_GROUPS_BMP_CNS     0x1

/*******************************************************************************
* prvCpssDxChLionDuplicatedMultiPortGroupsGet
*
* DESCRIPTION:
*       Check if register address is duplicated in multi-port groups device
*       for Lion B device
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       regAddr - register address to access
*
* OUTPUTS:
*       portGroupsBmpPtr  - (pointer to) the updated port groups
*                       relevant only when function returns GT_TRUE
*
* RETURNS:
*       GT_TRUE     - the caller need to use the updated 'port groups bmp'
*       GT_FALSE    - the caller NOT need to use the updated 'port groups bmp'
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvCpssDxChLionDuplicatedMultiPortGroupsGet
(
    IN GT_U32                   regAddr,
    OUT  GT_PORT_GROUPS_BMP     *portGroupsBmpPtr
)
{
    switch(U32_GET_FIELD_MAC(regAddr , 23 , 6))
    {
        /* sub-unit index */
        case 0x03:      /* address 0x01800000 */   /* TXQ EGR */
            *portGroupsBmpPtr = LION_TXQ_EGR_UNIT_PORT_GROUPS_BMP_CNS;
            break;
        case 0x14:      /* address 0x0a000000 */   /* TXQ Queue */
            *portGroupsBmpPtr = LION_TXQ_QUEUE_UNIT_PORT_GROUPS_BMP_CNS;
            break;
        case 0x21:      /* address 0x10800000 */   /* TXQ LL */
            *portGroupsBmpPtr = LION_TXQ_LL_UNIT_PORT_GROUPS_BMP_CNS;
            break;
        case 0x22:      /* address 0x11000000 */   /* TXQ DQ */
            *portGroupsBmpPtr = LION_TXQ_DQ_UNIT_PORT_GROUPS_BMP_CNS;
            break;
        case 0x23:      /* address 0x11800000 */   /* TXQ SHT */
            *portGroupsBmpPtr = LION_TXQ_SHT_UNIT_PORT_GROUPS_BMP_CNS;
            break;
       default:
           return GT_FALSE;
    }

    if(*portGroupsBmpPtr == PORT_GROUPS_NO_CHANGE_CNS)
    {
        return GT_FALSE;
    }

    return GT_TRUE;
}

/*******************************************************************************
* prvCpssDxChDuplicatedMultiPortGroupsGet
*
* DESCRIPTION:
*       Check if register address is duplicated in multi-port groups device
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum  - the device number
*       regAddr - register address to access
*
* OUTPUTS:
*       portGroupsBmpPtr  - (pointer to) the updated port groups
*                       relevant only when function returns GT_TRUE
*
* RETURNS:
*       GT_TRUE     - the caller need to use the updated 'port groups bmp'
*       GT_FALSE    - the caller NOT need to use the updated 'port groups bmp'
*
* COMMENTS:
*
*
*******************************************************************************/
GT_BOOL prvCpssDxChDuplicatedMultiPortGroupsGet
(
    IN GT_U8                    devNum,
    IN GT_U32                   regAddr,
    OUT  GT_PORT_GROUPS_BMP     *portGroupsBmpPtr
)
{
    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) ||
       0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        return GT_FALSE;
    }

    return prvCpssDxChLionDuplicatedMultiPortGroupsGet(regAddr,portGroupsBmpPtr);
}

/*******************************************************************************
* prvCpssDxChHwPpReadRegister
*
* DESCRIPTION:
*       Read a register value from the given PP. - DXCH API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP to read from.
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       dataPtr - (pointer to) memory for the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    OUT GT_U32  *dataPtr
)
{
    return prvCpssDxChHwPpPortGroupGetRegField(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,
        0,32,
        dataPtr);
}



/*******************************************************************************
* prvCpssDxChHwPpWriteRegister
*
* DESCRIPTION:
*       Write to a PP's given register. - DXCH API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP to write to.
*       regAddr - The register's address to write to.
*       value   - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpWriteRegister
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    return prvCpssDxChHwPpPortGroupSetRegField(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,
        0,32,
        value);
}



/*******************************************************************************
* prvCpssDxChHwPpGetRegField
*
* DESCRIPTION:
*       Read a selected register field. - DXCH API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       regAddr - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*
* OUTPUTS:
*       fieldDataPtr   - (pointer to) Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldDataPtr
)
{
    return prvCpssDxChHwPpPortGroupGetRegField(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,
        fieldOffset,fieldLength,fieldDataPtr);
}



/*******************************************************************************
* prvCpssDxChHwPpSetRegField
*
* DESCRIPTION:
*       Write value to selected register field. - DXCH API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpSetRegField
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 fieldData
)
{
    return prvCpssDxChHwPpPortGroupSetRegField(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,
        fieldOffset,fieldLength,fieldData);
}



/*******************************************************************************
* prvCpssDxChHwPpReadRegBitMask
*
* DESCRIPTION:
*       Reads the unmasked bits of a register. - DXCH API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
* GalTis:
*       Command - hwPpReadRegBitMask
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
)
{
    return prvCpssDxChHwPpPortGroupReadRegBitMask(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,
        mask,dataPtr);
}




/*******************************************************************************
* prvCpssDxChHwPpWriteRegBitMask
*
* DESCRIPTION:
*       Writes the unmasked bits of a register. - DXCH API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
* GalTis:
*       Command - hwPpWriteRegBitMask
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpWriteRegBitMask
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 value
)
{
    return prvCpssDxChHwPpPortGroupWriteRegBitMask(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,regAddr,
        mask,value);
}



/*******************************************************************************
* prvCpssDxChHwPpReadRam
*
* DESCRIPTION:
*       Read from PP's RAM. - DXCH API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       addr    - Address offset to read from.
*       length  - Number of Words (4 byte) to read.
*
* OUTPUTS:
*       dataPtr    - (pointer to) An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       None.
*
* GalTis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpReadRam
(
    IN  GT_U8   devNum,
    IN  GT_U32  addr,
    IN  GT_U32  length,
    OUT GT_U32  *dataPtr
)
{
    return prvCpssDxChHwPpPortGroupReadRam(
        devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        addr,length,dataPtr);
}


/*******************************************************************************
* prvCpssDxChHwPpWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM. - DXCH API
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       addr    - Address offset to write to.
*       length  - Number of Words (4 byte) to write.
*       dataPtr - (pointer to) the array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       None.
*
* GalTis:
*       Table - PPWrite
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpWriteRam
(
    IN GT_U8   devNum,
    IN GT_U32  addr,
    IN GT_U32  length,
    IN GT_U32  *dataPtr
)
{
    return prvCpssDxChHwPpPortGroupWriteRam(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        addr,length,dataPtr);
}

/*******************************************************************************
* prvCpssDxChHwPpPortGroupReadRegister
*
* DESCRIPTION:
*       Read a register value from the given PP. - DXCH API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to read from.
*
* OUTPUTS:
*       dataPtr - (pointer to) memory for the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpPortGroupReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *dataPtr
)
{
    return prvCpssDxChHwPpPortGroupGetRegField(
        devNum, portGroupId,
        regAddr, 0, 32, dataPtr);
}

/*******************************************************************************
* prvCpssDxChHwPpPortGroupWriteRegister
*
* DESCRIPTION:
*       Write to a PP's given register. - DXCH API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to write to.
*       value       - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpPortGroupWriteRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
)
{
    return prvCpssDxChHwPpPortGroupSetRegField(devNum,portGroupId,
        regAddr,
        0,32,
        value);
}




/*******************************************************************************
* prvCpssDxChHwPpPortGroupGetRegField
*
* DESCRIPTION:
*       Read a selected register field. - DXCH API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       fieldDataPtr   - (pointer to) Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpPortGroupGetRegField
(
    IN  GT_U8    devNum,
    IN  GT_U32   portGroupId,
    IN  GT_U32   regAddr,
    IN  GT_U32   fieldOffset,
    IN  GT_U32   fieldLength,
    OUT GT_U32   *fieldDataPtr
)
{
    GT_STATUS       rc;
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*port groups bmp */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(fieldOffset == 0 && fieldLength == 32)
    {
        /* read register */

        if(GT_FALSE ==
            prvCpssDxChDuplicatedMultiPortGroupsGet(
                devNum,regAddr,&portGroupsBmp))
        {
            return prvCpssDrvHwPpPortGroupReadRegister(
                devNum,portGroupId,regAddr,fieldDataPtr);
        }

        /* get first port group in the BMP */
        PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
            devNum,portGroupsBmp,portGroupId);
        rc = prvCpssDrvHwPpPortGroupReadRegister(
            devNum,portGroupId,regAddr,fieldDataPtr);

        return rc;
    }

    if(GT_FALSE ==
        prvCpssDxChDuplicatedMultiPortGroupsGet(
            devNum,regAddr,&portGroupsBmp))
    {
        return prvCpssDrvHwPpPortGroupGetRegField(
            devNum,portGroupId,
            regAddr,fieldOffset,fieldLength,fieldDataPtr);
    }

    /* get first port group in the BMP */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum,portGroupsBmp,portGroupId);

    rc = prvCpssDrvHwPpPortGroupGetRegField(
        devNum,portGroupId,
        regAddr,fieldOffset,fieldLength,fieldDataPtr);

    return rc;
}



/*******************************************************************************
* prvCpssDxChHwPpPortGroupSetRegField
*
* DESCRIPTION:
*       Write value to selected register field. - DXCH API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpPortGroupSetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData
)
{
    GT_STATUS       rc = GT_FAIL;
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*port groups bmp */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(fieldOffset == 0 && fieldLength == 32)
    {
        /* write register */

        if(GT_FALSE ==
            prvCpssDxChDuplicatedMultiPortGroupsGet(devNum,regAddr,&portGroupsBmp))
        {
            return prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,fieldData);
        }

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,fieldData);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

        return rc;
    }


    if(GT_FALSE ==
        prvCpssDxChDuplicatedMultiPortGroupsGet(devNum,regAddr,&portGroupsBmp))
    {
        return prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,fieldOffset,fieldLength,fieldData);
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,fieldOffset,fieldLength,fieldData);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return rc;
}



/*******************************************************************************
* prvCpssDxChHwPpPortGroupReadRegBitMask
*
* DESCRIPTION:
*       Reads the unmasked bits of a register. - DXCH API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
* GalTis:
*       Command - hwPpReadRegBitMask
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpPortGroupReadRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
)
{
    GT_STATUS       rc;
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*port groups bmp */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(GT_FALSE ==
        prvCpssDxChDuplicatedMultiPortGroupsGet(devNum,regAddr,&portGroupsBmp))
    {
        return prvCpssDrvHwPpPortGroupReadRegBitMask(devNum,portGroupId,regAddr,mask,dataPtr);
    }

    /* get first port group in the BMP */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(devNum,portGroupsBmp,portGroupId);

    rc = prvCpssDrvHwPpPortGroupReadRegBitMask(devNum,portGroupId,regAddr,mask,dataPtr);

    return rc;
}




/*******************************************************************************
* prvCpssDxChHwPpPortGroupWriteRegBitMask
*
* DESCRIPTION:
*       Writes the unmasked bits of a register. - DXCH API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
* GalTis:
*       Command - hwPpWriteRegBitMask
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpPortGroupWriteRegBitMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    IN GT_U32   value
)
{
    GT_STATUS       rc = GT_FAIL;
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*port groups bmp */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(GT_FALSE ==
        prvCpssDxChDuplicatedMultiPortGroupsGet(devNum,regAddr,&portGroupsBmp))
    {
        return prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum,portGroupId,regAddr,mask,value);
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum,portGroupId,regAddr,mask,value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return rc;
}

/*******************************************************************************
* prvCpssDxChHwPpPortGroupReadRam
*
* DESCRIPTION:
*       Read from PP's RAM. - DXCH API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP device number to read from.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr        - Address offset to read from.
*       length      - Number of Words (4 byte) to read.
*
* OUTPUTS:
*       dataPtr     - (pointer to) An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       None.
*
* GalTis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpPortGroupReadRam
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *dataPtr
)
{
    GT_STATUS       rc;
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*port groups bmp */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(GT_FALSE ==
        prvCpssDxChDuplicatedMultiPortGroupsGet(
            devNum,addr,&portGroupsBmp))
    {
        return prvCpssDrvHwPpPortGroupReadRam(
            devNum,portGroupId,addr,length,dataPtr);
    }

    /* get first port group in the BMP */
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum,portGroupsBmp,portGroupId);

    rc = prvCpssDrvHwPpPortGroupReadRam(
        devNum,portGroupId,addr,length,dataPtr);

    return rc;
}

/*******************************************************************************
* prvCpssDxChHwPpPortGroupWriteRam
*
* DESCRIPTION:
*       Writes to PP's RAM. - DXCH API
*       in the specific port group in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The PP device number to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr        - Address offset to write to.
*       length      - Number of Words (4 byte) to write.
*       dataPtr     - (pointer to) An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device*
* COMMENTS:
*       None.
*
* GalTis:
*       Table - PPWrite
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpPortGroupWriteRam
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *dataPtr
)
{
    GT_STATUS       rc = GT_FAIL;
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*port groups bmp */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(GT_FALSE ==
        prvCpssDxChDuplicatedMultiPortGroupsGet(devNum,addr,&portGroupsBmp))
    {
        return prvCpssDrvHwPpPortGroupWriteRam(
            devNum,portGroupId,addr,length,dataPtr);
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupWriteRam(
            devNum,portGroupId,addr,length,dataPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum,portGroupsBmp,portGroupId)

    return rc;
}



