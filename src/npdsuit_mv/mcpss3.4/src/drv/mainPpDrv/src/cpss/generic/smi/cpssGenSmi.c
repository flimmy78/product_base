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
* cpssGenPpSmi.c
*
* DESCRIPTION:
*       API for read/write register of device, which connected to SMI master 
*           controller of packet processor
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/phy/private/prvCpssPhy.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>

/*************************** Private definitions ******************************/

#define  PRV_CPSS_SMI_WRITE_ADDRESS_MSB_REGISTER_CNS   (0x00)
#define  PRV_CPSS_SMI_WRITE_ADDRESS_LSB_REGISTER_CNS   (0x01)
#define  PRV_CPSS_SMI_WRITE_DATA_MSB_REGISTER_CNS      (0x02)
#define  PRV_CPSS_SMI_WRITE_DATA_LSB_REGISTER_CNS      (0x03)
#define  PRV_CPSS_SMI_READ_ADDRESS_MSB_REGISTER_CNS    (0x04)
#define  PRV_CPSS_SMI_READ_ADDRESS_LSB_REGISTER_CNS    (0x05)
#define  PRV_CPSS_SMI_READ_DATA_MSB_REGISTER_CNS       (0x06)
#define  PRV_CPSS_SMI_READ_DATA_LSB_REGISTER_CNS       (0x07)
#define  PRV_CPSS_SMI_STATUS_REGISTER_CNS              (0x1f)
#define  PRV_CPSS_SMI_STATUS_WRITE_DONE_CNS            (0x02)
#define  PRV_CPSS_SMI_STATUS_READ_READY_CNS            (0x01)

#define PRV_CPSS_SMI_MNG_CNTRL_OFFSET_CNS    0x1000000
#define PRV_CPSS_SMI_RETRY_COUNTER_CNS       1000

/*************************** Private functions ********************************/

/*******************************************************************************
* smiStatusCheck
*
* DESCRIPTION:
*      Check if SMI is busy or during read operation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev - device number
*       portGroup - port group number
*       read - set to TRUE if it read operation
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - HW failure
*       GT_NOT_INITIALIZED - smi ctrl register callback not registered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#ifndef ASIC_SIMULATION
static GT_STATUS smiStatusCheck
(
    IN GT_U8    dev,
    IN GT_U32   portGroup,
    IN CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN GT_BOOL  read
)
{
    GT_U32 status;
    GT_U32 desired_status = 0;
    GT_U32 bit = 28;
    volatile GT_U32 retryCnt = PRV_CPSS_SMI_RETRY_COUNTER_CNS;   /* retry counter for busy SMI reg   */

    /* check if for given device there is registered callback */
    if(PRV_CPSS_PP_MAC(dev)->phyInfo.genSmiBindFunc.cpssPhySmiCtrlRegRead == NULL)
        return GT_NOT_INITIALIZED;

    if (GT_TRUE == read)
    {
        desired_status = 1;
        bit = 27;
    }

    /* check if smi register is not busy or read operation done */
    do
    {
        if (PRV_CPSS_PP_MAC(dev)->phyInfo.genSmiBindFunc.cpssPhySmiCtrlRegRead(dev, 
                                            portGroup, smiInterface, &status) != GT_OK)
        {
            return GT_HW_ERROR;
        }
        status = U32_GET_FIELD_MAC(status, bit, 1);
        retryCnt--;
    }while ((status != desired_status) && (retryCnt != 0));

    if (0 == retryCnt)
        return GT_NOT_READY;

    return GT_OK;
}
#endif 
/*************************** Global functions ********************************/

/*******************************************************************************
* cpssSmiRegisterReadShort
*
* DESCRIPTION:
*      The function reads register of a device, which connected to SMI master 
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       dataPtr - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*       GT_NOT_INITIALIZED - smi ctrl register callback not registered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterReadShort
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr, 
    IN  GT_U32  regAddr,
    OUT GT_U16  *dataPtr
)
{
#ifndef ASIC_SIMULATION
    GT_STATUS rc;
    GT_U32  regVal;
    GT_U32  readData = 0;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {   /* for non multi-port groups device the MACRO assign value */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    if(smiInterface > CPSS_PHY_SMI_INTERFACE_MAX_E)
        return GT_BAD_PARAM;
   
    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* check if smi register is not busy */
        if ((rc = smiStatusCheck(devNum, portGroupId, smiInterface, GT_FALSE)) != GT_OK)
        {
            return rc;
        }
    
        regVal = ((smiAddr & 0x1F) << 16) | ((regAddr & 0x1F) << 21) | (1 << 26) ;
        if (PRV_CPSS_PP_MAC(devNum)->phyInfo.genSmiBindFunc.cpssPhySmiCtrlRegWrite(devNum, 
                                                    portGroupId, smiInterface, regVal) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    
        /* check if smi read operation done */
        if ((rc = smiStatusCheck(devNum, portGroupId, smiInterface, GT_TRUE)) != GT_OK)
        {
            return rc;
        }
    
        /* Read from Lion SMI management register */
        if (PRV_CPSS_PP_MAC(devNum)->phyInfo.genSmiBindFunc.cpssPhySmiCtrlRegRead(devNum, 
                                            portGroupId, smiInterface, &readData) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* read register of first ports group and exit */
        break;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    *dataPtr = (GT_U16)(readData & 0xFFFF);
#else 
    devNum          = devNum;
    portGroupsBmp   = portGroupsBmp;
    smiInterface    = smiInterface;
    smiAddr         = smiAddr; 
    regAddr         = regAddr;      
    dataPtr         = dataPtr;      
#endif 
    return GT_OK;
}

/*******************************************************************************
* cpssSmiRegisterWriteShort
*
* DESCRIPTION:
*      The function writes register of a device, which connected to SMI master 
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - address of register of configurated device
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*       GT_NOT_INITIALIZED - smi ctrl register callback not registered
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterWriteShort
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr, 
    IN  GT_U32  regAddr,
    IN  GT_U16  data
)
{
#ifndef ASIC_SIMULATION
    GT_STATUS rc;
    GT_U32  regVal;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {   /* for non multi-port groups device the MACRO assign value */
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    if(smiInterface > CPSS_PHY_SMI_INTERFACE_MAX_E)
        return GT_BAD_PARAM;

    regVal = data & 0xFFFF;
    regVal |= ((smiAddr & 0x1F) << 16) | ((regAddr & 0x1F) << 21) | (0 << 26) ;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* check if smi register is not busy */
        if ((rc = smiStatusCheck(devNum, portGroupId, smiInterface, GT_FALSE)) != GT_OK)
        {
            return rc;
        }
    
        if (PRV_CPSS_PP_MAC(devNum)->phyInfo.genSmiBindFunc.cpssPhySmiCtrlRegWrite(devNum,
                                                portGroupId, smiInterface, regVal) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
#else 
    devNum          = devNum;
    portGroupsBmp   = portGroupsBmp;
    smiInterface    = smiInterface;
    smiAddr         = smiAddr; 
    regAddr         = regAddr;      
    data            = data;      
#endif 
    return GT_OK;
}

/*******************************************************************************
* cpssSmiRegisterRead
*
* DESCRIPTION:
*      The function reads register of a Marvell device, which connected to 
*           SMI master controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       dataPtr - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       Function specific for Marvell devices with 32-bit registers
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterRead
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr, 
    IN  GT_U32  regAddr,
    OUT GT_U32  *dataPtr
)
{
#ifndef ASIC_SIMULATION
    GT_U16              msb;
    GT_U16              lsb;
    GT_U32              value;
    GT_STATUS           rc;

    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    /* write addr to read */
    msb = (GT_U16)(regAddr >> 16);
    lsb = (GT_U16)(regAddr & 0xFFFF);

    if ((rc = cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr, 
                                        PRV_CPSS_SMI_READ_ADDRESS_MSB_REGISTER_CNS, msb)) != GT_OK)
    {
        return rc;
    }

    if ((rc = cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr, 
                                        PRV_CPSS_SMI_READ_ADDRESS_LSB_REGISTER_CNS, lsb)) != GT_OK)
    {
        return rc;
    }

    /* read data */
    if ((rc = cpssSmiRegisterReadShort(devNum, portGroupsBmp, smiInterface, smiAddr, 
                                        PRV_CPSS_SMI_READ_DATA_MSB_REGISTER_CNS, &msb)) != GT_OK)
    {
        return rc;
    }

    if ((rc = cpssSmiRegisterReadShort(devNum, portGroupsBmp, smiInterface, smiAddr, 
                                        PRV_CPSS_SMI_READ_DATA_LSB_REGISTER_CNS, &lsb)) != GT_OK)
    {
        return rc;
    }

    value = (GT_U32)msb;
    *dataPtr = (value << 16) | lsb;
#else

    devNum  =       devNum;
    portGroupsBmp = portGroupsBmp;
    smiInterface  = smiInterface;
    smiAddr =       smiAddr;
    regAddr =       regAddr;
    dataPtr =       dataPtr;

#endif

    return GT_OK;
}

/*******************************************************************************
* cpssSmiRegisterWrite
*
* DESCRIPTION:
*      The function writes register of a Marvell device, which connected to SMI master 
*           controller of packet processor
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - address of register of configurated device
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       Function specific for Marvell devices with 32-bit registers
*
*******************************************************************************/
GT_STATUS cpssSmiRegisterWrite
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr, 
    IN  GT_U32  regAddr,
    IN  GT_U32  data
)
{
#ifndef ASIC_SIMULATION
    GT_U16              msb;
    GT_U16              lsb;

    /* write addr to write */
    msb = (GT_U16)(regAddr >> 16);
    lsb = (GT_U16)(regAddr & 0xFFFF);

    if (cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr, 
                                PRV_CPSS_SMI_WRITE_ADDRESS_MSB_REGISTER_CNS, msb) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    if (cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr, 
                                PRV_CPSS_SMI_WRITE_ADDRESS_LSB_REGISTER_CNS, lsb) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* write data to write */
    msb = (GT_U16)(data >> 16);
    lsb = (GT_U16)(data & 0xFFFF);

    if (cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr, 
                                PRV_CPSS_SMI_WRITE_DATA_MSB_REGISTER_CNS, msb) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    if (cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, smiAddr, 
                                PRV_CPSS_SMI_WRITE_DATA_LSB_REGISTER_CNS, lsb) != GT_OK)
    {
        return GT_HW_ERROR;
    }
#else

    devNum  =       devNum;
    portGroupsBmp = portGroupsBmp;
    smiInterface  = smiInterface;
    smiAddr =       smiAddr;
    regAddr =       regAddr;
    data    =       data;

#endif

    return GT_OK;
}

