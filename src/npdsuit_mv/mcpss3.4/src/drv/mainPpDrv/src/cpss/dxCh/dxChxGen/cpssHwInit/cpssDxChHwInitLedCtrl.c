/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChHwInitLedCtrl.c
*
* DESCRIPTION:
*       Includes Leds control functions implementations. -- Dx
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>

/* check LED interface number correctness: 0 for Lion or devices with up to 12
   ports (e.g. Dx106/107), otherwise 0 or 1 */
#define PRV_CPSS_DXCH_LED_INTERFACE_NUM_CHECK_MAC(_devNum,_ledInterfaceNum)     \
    if((((PRV_CPSS_PP_MAC(_devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E) || \
         (PRV_CPSS_PP_MAC(_devNum)->numOfPorts <= 11)) &&                       \
          (_ledInterfaceNum > 0)) ||                                            \
          (_ledInterfaceNum >= CPSS_DXCH_MAX_LED_INTERFACE_CNS) )               \
    {                                                                           \
              return GT_BAD_PARAM;                                              \
    }

/*******************************************************************************
* cpssDxChLedStreamClassAndGroupConfig
*
* DESCRIPTION:
*       This routine configures the control, the class manipulation and the
*       group configuration registers for both led interfaces in Dx.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number
*       ledControl      - The user interface control register value.
*       ledClassConfPtr- (pointer to) a list of class manipulation registers
*       ledClassNum     - the class list length
*       ledGroupConfPtr- (pointer to) a list of group configuration registers
*       ledGroupNum     - the group list length
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE - ledClassNum value or ledGroupNum value is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  See the Led Serial Interface in the device's data-sheet for full
*           detail of the leds configuration, classes and groups.
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamClassAndGroupConfig
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              ledControl,
    IN  CPSS_DXCH_LED_CLASS_CONF_STC        *ledClassConfPtr,
    IN  GT_U32                              ledClassNum,
    IN  CPSS_DXCH_LED_GROUP_CONF_STC        *ledGroupConfPtr,
    IN  GT_U32                              ledGroupNum
)
{
    GT_U32                  i,j;              /* loop iterators           */
    GT_U32                  fieldOffset;      /* written field offset     */
    GT_U32                  fieldLength;      /* written field length     */
    GT_U32                  classNum;         /* class indication number  */
    GT_U32                  groupNum;         /* group number             */
    GT_U32                  ledInterfacesNum; /* number of led interfaces */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ledClassConfPtr);
    CPSS_NULL_PTR_CHECK_MAC(ledGroupConfPtr);


    /* validate class indication data */
    if (ledClassNum > 0)
    {
        if (ledClassNum > CPSS_DXCH_MAX_LED_CLASS_CNS)
        {
            return GT_OUT_OF_RANGE;
        }
    }

    /* validate group data */
    if (ledGroupNum > 0)
    {
        if (ledGroupNum > CPSS_DXCH_MAX_LED_GROUP_CNS)
        {
            return GT_OUT_OF_RANGE;
        }
    }

    /* Set number of led interfaces.
       LED intrerface 0 - Giga Ports 0..11, CPU port and XG ports 26, 27
       LED intrerface 1 - Giga Ports 12..23, and XG ports 24, 24.
        */
    if (PRV_CPSS_PP_MAC(devNum)->numOfPorts <= 11 ||
        PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* for reduced devices like DX106 and DX107 only interface 0 is used*/
        ledInterfacesNum = 1;
    }
    else
    {
        ledInterfacesNum = CPSS_DXCH_MAX_LED_INTERFACE_CNS;
    }

    /* configure led interfaces */
    for (i = 0; i < ledInterfacesNum; i++)
    {
        /* set led control per interface */

        if (prvCpssDrvHwPpWriteRegister(devNum,
                                        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                        ledRegs.ledControl[i],
                                        ledControl) != GT_OK)
        {
            return GT_FAIL;
        }

        /* set class indication per interface */
        for (j = 0; j < ledClassNum ; j++)
        {
            classNum = ledClassConfPtr[j].classNum;

            switch (classNum)
            {
                case 0:
                case 2:
                    fieldOffset = 0;
                    fieldLength = 16;
                    break;

                case 1:
                case 3:
                    fieldOffset = 16;
                    fieldLength = 16;
                    break;

                case 4:
                case 5:
                    fieldOffset = 0;
                    fieldLength = 16;
                    break;

                case 6:
                    fieldOffset = 0;
                    fieldLength = 20;
                    break;

                default:
                    return GT_FAIL;
            }

            if (prvCpssDrvHwPpSetRegField(devNum,
                                          PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                          ledRegs.ledClass[i][classNum],
                                          fieldOffset, fieldLength,
                                          ledClassConfPtr[j].classConf) != GT_OK)
            {
                return GT_FAIL;
            }
        }

        /* set group logic per interface */
        for (j = 0; j < ledGroupNum; j++)
        {
            groupNum = ledGroupConfPtr[j].groupNum;

            switch (groupNum)
            {
                case 0:
                case 2:
                    fieldOffset = 0;
                    break;

                case 1:
                case 3:
                    fieldOffset = 16;
                    break;

                default:
                    return GT_FAIL;
            }

            if (prvCpssDrvHwPpSetRegField(devNum,
                                          PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                          ledRegs.ledGroup[i][groupNum],
                                          fieldOffset, 16,
                                          ledGroupConfPtr[j].groupConf) != GT_OK)
            {
                return GT_FAIL;
            }
        }
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChLedStreamHyperGStackTxQStatusEnableSet
*
* DESCRIPTION:
*       Enables the display to HyperGStack Ports Transmit Queue Status via LED
*       Stream.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion.
*
* INPUTS:
*       devNum   - physical device number
*       enable  - GT_TRUE - enable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           conveyed via the LED stream. Port<i>TxqNotFull indication for each of
*                           those ports is set to 0 when the number of buffers currently allocated in all
*                           of this ports transmit queues exceeds the limit configured for this port.
*                 GT_FALSE - disable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           not conveyed via the LED stream.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChLedStreamHyperGStackTxQStatusEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
)
{
    GT_U32      value;   /* register field value */
    GT_U32      regAddr; /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_LION_E);


    /* write enable value */
    value = (enable == GT_TRUE) ? 1 : 0;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 1, value);
}

/*******************************************************************************
* cpssDxChLedStreamHyperGStackTxQStatusEnableGet
*
* DESCRIPTION:
*       Gets HyperGStack Ports Transmit Queue Status via LED Stream.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion.
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE - enable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           conveyed via the LED stream. Port<i>TxqNotFull indication for each of
*                           those ports is set to 0 when the number of buffers currently allocated in all
*                           of this ports transmit queues exceeds the limit configured for this port.
*                   GT_FALSE - disable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           not conveyed via the LED stream.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChLedStreamHyperGStackTxQStatusEnableGet
(
    IN  GT_U8       devNum,
    OUT  GT_BOOL    *enablePtr
)
{
    GT_U32      value;   /* register field value */
    GT_U32      regAddr; /* register address */
    GT_STATUS   rc;      /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 3, 1, &value);

    if(rc != GT_OK)
    {
        return rc;
    }

    /* write enable value */
    *enablePtr = (value == 0) ? GT_FALSE : GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLedStreamCpuOrPort27ModeSet
*
* DESCRIPTION:
*       This routine configures the whether LED stream interface 0 contains CPU
*       or port 27 indications.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion.
*
* INPUTS:
*       devNum          - physical device number
*       indicatedPort   - the selected port indication: CPU or port 27.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamCpuOrPort27ModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_LED_CPU_OR_PORT27_ENT indicatedPort
)
{
    GT_STATUS rc;           /* return status */
    GT_U32  regData;        /* register led control value */
    GT_U32  regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_LION_E);


    switch(indicatedPort)
    {
        case CPSS_DXCH_LED_CPU_E:       regData = 0 ;
                                        break;
        case CPSS_DXCH_LED_PORT27_E:    regData = 1 ;
                                        break;
        default: return GT_BAD_PARAM;
    }

    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledXgClass04Manipulation[0];

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChLedStreamCpuOrPort27ModeGet
*
* DESCRIPTION:
*       This routine gets the whether LED stream interface 0 contains CPU
*       or port 27 indications.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Lion.
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       indicatedPortPtr    - (pointer to) the selected port indication:
*                             CPU or port 27.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamCpuOrPort27ModeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_LED_CPU_OR_PORT27_ENT *indicatedPortPtr
)
{
    GT_STATUS rc;           /* return status */
    GT_U32  regData;        /* register led control value */
    GT_U32  regAddr;        /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(indicatedPortPtr);

    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledXgClass04Manipulation[0];

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &regData);
    if( rc != GT_OK )
    {
        return rc;
    }

    *indicatedPortPtr = (regData == 0) ? CPSS_DXCH_LED_CPU_E : CPSS_DXCH_LED_PORT27_E ;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLedStreamClassManipXgAux
*
* DESCRIPTION:
*       Auxilary function to calculate register address and offsets for class
*       manipulation configuration for XG (Flex) ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number.
*       classNum        - class number
*
* OUTPUTS:
*       regAddrPtr              - (pointer to) register address
*       invertEnableOffsetPtr   - (pointer to) invert enable offset in register.
*       blinkEnableOffsetPtr    - (pointer to) blink enable offset in register.
*       blinkSelectOffsetPtr    - (pointer to) blink select offset in register.
*       forceEnableOffsetPtr    - (pointer to) force enable offset in register.
*       forceDataOffsetPtr      - (pointer to) force data offset in register.
*       forceDataLengthPtr      - (pointer to) force data length in register.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLedStreamClassManipXgAux
(
    IN  GT_U8   devNum,
    IN  GT_U32  ledInterfaceNum,
    IN  GT_U32  classNum,
    OUT GT_U32  *regAddrPtr,
    OUT GT_U32  *invertEnableOffsetPtr,
    OUT GT_U32  *blinkEnableOffsetPtr,
    OUT GT_U32  *blinkSelectOffsetPtr,
    OUT GT_U32  *forceEnableOffsetPtr,
    OUT GT_U32  *forceDataOffsetPtr,
    OUT GT_U32  *forceDataLengthPtr
)
{
    switch(classNum)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4: *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledXgClass04Manipulation[ledInterfaceNum];
                break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9: *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledXgClass59Manipulation[ledInterfaceNum];
                break;
        case 10:
        case 11: *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledXgClass1011Manipulation[ledInterfaceNum];
                 break;
        default: return GT_BAD_PARAM;
    }

    switch(classNum)
    {
        case 0:
        case 5: *invertEnableOffsetPtr  = 7 ;
                *blinkEnableOffsetPtr   = 5 ;
                *blinkSelectOffsetPtr   = 6 ;
                *forceEnableOffsetPtr   = 4 ;
                *forceDataOffsetPtr     = 2 ;
                break;
        case 1:
        case 6: *invertEnableOffsetPtr  = 13 ;
                *blinkEnableOffsetPtr   = 11 ;
                *blinkSelectOffsetPtr   = 12 ;
                *forceEnableOffsetPtr   = 10 ;
                *forceDataOffsetPtr     = 8  ;
                break;
        case 2:
        case 7: *invertEnableOffsetPtr  = 19 ;
                *blinkEnableOffsetPtr   = 17 ;
                *blinkSelectOffsetPtr   = 18 ;
                *forceEnableOffsetPtr   = 16 ;
                *forceDataOffsetPtr     = 14 ;
                break;
        case 3:
        case 8: *invertEnableOffsetPtr  = 25 ;
                *blinkEnableOffsetPtr   = 23 ;
                *blinkSelectOffsetPtr   = 24 ;
                *forceEnableOffsetPtr   = 22 ;
                *forceDataOffsetPtr     = 20 ;
                break;
        case 4:
        case 9: *invertEnableOffsetPtr  = 31 ;
                *blinkEnableOffsetPtr   = 29 ;
                *blinkSelectOffsetPtr   = 30 ;
                *forceEnableOffsetPtr   = 28 ;
                *forceDataOffsetPtr     = 26 ;
                break;

        case 10:*invertEnableOffsetPtr  = 25 ;
                *blinkEnableOffsetPtr   = 23 ;
                *blinkSelectOffsetPtr   = 24 ;
                *forceEnableOffsetPtr   = 22 ;
                *forceDataOffsetPtr     = 20 ;
                break;

        case 11:*invertEnableOffsetPtr  = 31 ;
                *blinkEnableOffsetPtr   = 29 ;
                *blinkSelectOffsetPtr   = 30 ;
                *forceEnableOffsetPtr   = 28 ;
                *forceDataOffsetPtr     = 26 ;
                break;
        default: return GT_BAD_PARAM;
    }

    *forceDataLengthPtr = 2;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLedStreamClassManipulationSet
*
* DESCRIPTION:
*       This routine configures a LED stream class manipulation.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamClassManipulationSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT     portType,
    IN  GT_U32                          classNum,
    IN  CPSS_LED_CLASS_MANIPULATION_STC *classParamsPtr
)
{
    return cpssDxChLedStreamPortGroupClassManipulationSet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               ledInterfaceNum, portType, 
                                               classNum, classParamsPtr);
}

/*******************************************************************************
* cpssDxChLedStreamClassManipulationGet
*
* DESCRIPTION:
*       This routine gets a LED stream class manipulation configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*
* OUTPUTS:
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamClassManipulationGet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT     portType,
    IN  GT_U32                          classNum,
    OUT CPSS_LED_CLASS_MANIPULATION_STC *classParamsPtr
)
{
    return cpssDxChLedStreamPortGroupClassManipulationGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               ledInterfaceNum, portType, 
                                               classNum, classParamsPtr);
}

/*******************************************************************************
* cpssDxChLedStreamGroupConfigSet
*
* DESCRIPTION:
*       This routine configures a LED stream group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       groupNum        - group number 
*                         For tri-speed ports: range 0..3
*                         For XG ports: range 0..1
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - on out of range class number 
*                                  (allowable class range o..15)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamGroupConfigSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT portType,
    IN  GT_U32                      groupNum,
    IN  CPSS_LED_GROUP_CONF_STC     *groupParamsPtr
)
{
    return cpssDxChLedStreamPortGroupGroupConfigSet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               ledInterfaceNum, portType, 
                                               groupNum, groupParamsPtr);
}


/*******************************************************************************
* cpssDxChLedStreamGroupConfigGet
*
* DESCRIPTION:
*       This routine gets a LED stream group configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       groupNum        - group number 
*                         For tri-speed ports: range 0..3
*                         For XG ports: range 0..1
*
* OUTPUTS:
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamGroupConfigGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT portType,
    IN  GT_U32                      groupNum,
    OUT CPSS_LED_GROUP_CONF_STC     *groupParamsPtr
)
{
    return cpssDxChLedStreamPortGroupGroupConfigGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               ledInterfaceNum, portType, 
                                               groupNum, groupParamsPtr);
}

/*******************************************************************************
* cpssDxChLedStreamConfigSet
*
* DESCRIPTION:
*       This routine configures the LED stream.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANG           - ledStart or ledEnd out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamConfigSet
(
    IN  GT_U8               devNum,
    IN  GT_U32              ledInterfaceNum,
    IN  CPSS_LED_CONF_STC   *ledConfPtr
)
{
    return cpssDxChLedStreamPortGroupConfigSet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                              ledInterfaceNum, ledConfPtr);
}

/*******************************************************************************
* cpssDxChLedStreamConfigGet
*
* DESCRIPTION:
*       This routine gets the LED stream configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*
* OUTPUTS:
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad hardware value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamConfigGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              ledInterfaceNum,
    OUT CPSS_LED_CONF_STC   *ledConfPtr
)
{
    return cpssDxChLedStreamPortGroupConfigGet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                              ledInterfaceNum, ledConfPtr);
}

/*******************************************************************************
* cpssDxChLedStreamClassIndicationSet
*
* DESCRIPTION:
*       This routine configures classes 9-11 indication (debug) for flex ports.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*       classNum        - class number, range 9..11
*       indication      - port (debug) indication data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamClassIndicationSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          ledInterfaceNum,
    IN  GT_U32                          classNum,
    IN  CPSS_DXCH_LED_INDICATION_ENT    indication
)
{
    GT_STATUS rc;               /* return status */
    GT_U32  regData;            /* register led control value */
    GT_U32  regAddr;            /* register address */
    GT_U32  offset;             /* offset in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);


    if( ledInterfaceNum >= CPSS_DXCH_MAX_LED_INTERFACE_CNS )
    {
        return GT_BAD_PARAM;
    }

    switch(classNum)
    {
        case 9:  offset = 0;
                 break;
        case 10: offset = 4;
                 break;
        case 11: offset = 8;
                 break;
        default: return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledHyperGStackDebugSelect[ledInterfaceNum];

    switch(indication)
    {
        case CPSS_DXCH_LED_INDICATION_PRIMARY_E:                       regData = 0;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_LINK_E:                          regData = 1;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_RX_ACT_E:                        regData = 2;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_TX_ACT_E:                        regData = 3;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_LOCAL_FAULT_OR_PCS_LINK_E:       regData = 4;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_REMOTE_FAULT_OR_RESERVED_E:      regData = 5;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_UKN_SEQ_OR_DUPLEX_E:             regData = 6;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_P_REJ_E:                         regData = 7;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_RX_ERROR_E:                      regData = 8;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_JABBER_E:                        regData = 9;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_FRAGMENT_E:                      regData = 10;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_CRC_ERROR_E:                     regData = 11;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_FC_RX_E:                         regData = 12;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_TX_BAD_CRC_OR_LATE_COL_E:        regData = 13;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_RX_BUFFER_FULL_OR_BACK_PR_E:     regData = 14;
                                                                       break;
        case CPSS_DXCH_LED_INDICATION_WATCHDOG_EXP_SAMPLE_E:           regData = 15;
                                                                       break;
        default: return GT_BAD_PARAM;
    }

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, offset, 4, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChLedStreamClassIndicationGet
*
* DESCRIPTION:
*       This routine gets classes 9-11 indication (debug) for flex ports.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*       classNum        - class number, range 9..11
*
* OUTPUTS:
*       indicationPtr   - (pointer to) port (debug) indication data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_STATE             - on bad hardware value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamClassIndicationGet
(
    IN  GT_U8                          devNum,
    IN  GT_U32                         ledInterfaceNum,
    IN  GT_U32                         classNum,
    OUT CPSS_DXCH_LED_INDICATION_ENT   *indicationPtr
)
{
    GT_STATUS rc;               /* return status */
    GT_U32  regData;            /* register led control value */
    GT_U32  regAddr;            /* register address */
    GT_U32  offset;             /* offset in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(indicationPtr);

    if( ledInterfaceNum >= CPSS_DXCH_MAX_LED_INTERFACE_CNS )
    {
        return GT_BAD_PARAM;
    }

    switch(classNum)
    {
        case 9:  offset = 0;
                 break;
        case 10: offset = 4;
                 break;
        case 11: offset = 8;
                 break;
        default: return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledHyperGStackDebugSelect[ledInterfaceNum];
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, offset, 4, &regData);
    if( rc != GT_OK )
    {
        return rc;
    }

    switch(regData)
    {
        case 0:  *indicationPtr = CPSS_DXCH_LED_INDICATION_PRIMARY_E;
                 break;
        case 1:  *indicationPtr = CPSS_DXCH_LED_INDICATION_LINK_E;
                 break;
        case 2:  *indicationPtr = CPSS_DXCH_LED_INDICATION_RX_ACT_E;
                 break;
        case 3:  *indicationPtr = CPSS_DXCH_LED_INDICATION_TX_ACT_E;
                 break;
        case 4:  *indicationPtr = CPSS_DXCH_LED_INDICATION_LOCAL_FAULT_OR_PCS_LINK_E;
                 break;
        case 5:  *indicationPtr = CPSS_DXCH_LED_INDICATION_REMOTE_FAULT_OR_RESERVED_E;
                 break;
        case 6:  *indicationPtr = CPSS_DXCH_LED_INDICATION_UKN_SEQ_OR_DUPLEX_E;
                 break;
        case 7:  *indicationPtr = CPSS_DXCH_LED_INDICATION_P_REJ_E;
                 break;
        case 8:  *indicationPtr = CPSS_DXCH_LED_INDICATION_RX_ERROR_E;
                 break;
        case 9:  *indicationPtr = CPSS_DXCH_LED_INDICATION_JABBER_E;
                 break;
        case 10: *indicationPtr = CPSS_DXCH_LED_INDICATION_FRAGMENT_E;
                 break;
        case 11: *indicationPtr = CPSS_DXCH_LED_INDICATION_CRC_ERROR_E;
                 break;
        case 12: *indicationPtr = CPSS_DXCH_LED_INDICATION_FC_RX_E;
                 break;
        case 13: *indicationPtr = CPSS_DXCH_LED_INDICATION_TX_BAD_CRC_OR_LATE_COL_E;
                 break;
        case 14: *indicationPtr = CPSS_DXCH_LED_INDICATION_RX_BUFFER_FULL_OR_BACK_PR_E;
                 break;
        case 15: *indicationPtr = CPSS_DXCH_LED_INDICATION_WATCHDOG_EXP_SAMPLE_E;
                 break;
        default: return GT_BAD_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLedStreamDirectModeEnableSet
*
* DESCRIPTION:
*       This routine enables\disables LED Direct working mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*       enable          - GT_TRUE: direct mode
*                         GT_FALSE: serial mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*           In Direct mode LED pin indicates a different Stack port status.
*           The external pins are mapped as follows:
*           LEDDATA1 — indicates stack port 24
*           LEDSTB1  — indicates stack port 25
*           LEDDATA0 — indicates stack port 26
*           LEDSTB0  — indicates stack port 27
*           The indication for each port is driven from the LED group0
*           configurations.
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamDirectModeEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  ledInterfaceNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;               /* return status */
    GT_U32  regData;            /* register led control value */
    GT_U32  regAddr;            /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);


    if( ledInterfaceNum >= CPSS_DXCH_MAX_LED_INTERFACE_CNS )
    {
        return GT_BAD_PARAM;
    }

    regData = (GT_TRUE == enable ) ? 1 : 0 ;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledHyperGStackDebugSelect1[ledInterfaceNum];

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, regData);

    return rc;
}

/*******************************************************************************
* cpssDxChLedStreamDirectModeEnableGet
*
* DESCRIPTION:
*       This routine gets LED pin indication direct mode enabling status.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE: direct mode
*                                      GT_FALSE: serial mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_STATE             - on bad hardware value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*           In Direct mode LED pin indicates a different Stack port status.
*           The external pins are mapped as follows:
*           LEDDATA1 — indicates stack port 24
*           LEDSTB1  — indicates stack port 25
*           LEDDATA0 — indicates stack port 26
*           LEDSTB0  — indicates stack port 27
*           The indication for each port is driven from the LED group0
*           configurations.
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamDirectModeEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  ledInterfaceNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS rc;               /* return status */
    GT_U32  regData;            /* register led control value */
    GT_U32  regAddr;            /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if( ledInterfaceNum >= CPSS_DXCH_MAX_LED_INTERFACE_CNS )
    {
        return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledHyperGStackDebugSelect1[ledInterfaceNum];

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &regData);
    if( rc != GT_OK )
    {
        return rc;
    }

    *enablePtr = (regData == 1) ? GT_TRUE : GT_FALSE ;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLedStreamGroupConfTriSpeedAux
*
* DESCRIPTION:
*       Auxilary function to calculate register address and offset for group
*       configuration for tri-speed (network) ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       interfaceNum    - LED stream interface number.
*       groupNum        - group number
*
* OUTPUTS:
*       regAddrPtr              - (pointer to) register address
*       groupDataOffsetPtr      - (pointer to) group data offset in register.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLedStreamGroupConfTriSpeedAux
(
    IN  GT_U8   devNum,
    IN  GT_U32  interfaceNum,
    IN  GT_U32  groupNum,
    OUT GT_U32  *regAddrPtr,
    OUT GT_U32  *groupDataOffsetPtr
)
{
    if( groupNum >= CPSS_DXCH_MAX_LED_GROUP_CNS )
    {
        return GT_BAD_PARAM;
    }

    *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledGroup[interfaceNum][groupNum];

    *groupDataOffsetPtr = ( groupNum & 0x1 ) ? 16 : 0 ;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupConfigSet
*
* DESCRIPTION:
*       This routine configures the LED stream.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANG           - ledStart or ledEnd out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupConfigSet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32              ledInterfaceNum,
    IN  CPSS_LED_CONF_STC   *ledConfPtr
)
{
    GT_STATUS rc;               /* return status */
    GT_U32  regData;            /* register led control value */
    GT_U32  regAddr;            /* register address */
    GT_U32  data;               /* local data */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ledConfPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_CPSS_DXCH_LED_INTERFACE_NUM_CHECK_MAC(devNum,ledInterfaceNum);

    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledControl[ledInterfaceNum];

    regData = 0;

    if( (ledConfPtr->ledStart > 255) || (ledConfPtr->ledEnd > 255) )
    {
        return GT_OUT_OF_RANGE;
    }

    U32_SET_FIELD_MAC(regData , 0, 8, ledConfPtr->ledStart);
    U32_SET_FIELD_MAC(regData , 8, 8, ledConfPtr->ledEnd);

    switch(ledConfPtr->pulseStretch)
    {
        case CPSS_LED_PULSE_STRETCH_0_NO_E: data = 0;
                                            break;
        case CPSS_LED_PULSE_STRETCH_1_E:    data = 1;
                                            break;
        case CPSS_LED_PULSE_STRETCH_2_E:    data = 2;
                                            break;
        case CPSS_LED_PULSE_STRETCH_3_E:    data = 3;
                                            break;
        case CPSS_LED_PULSE_STRETCH_4_E:    data = 4;
                                            break;
        case CPSS_LED_PULSE_STRETCH_5_E:    data = 5;
                                            break;
        default: return GT_BAD_PARAM;
    }
    U32_SET_FIELD_MAC(regData , 16, 3, data);


    switch(ledConfPtr->blink0Duration)
    {
        case CPSS_LED_BLINK_DURATION_0_E:   data = 0;
                                            break;
        case CPSS_LED_BLINK_DURATION_1_E:   data = 1;
                                            break;
        case CPSS_LED_BLINK_DURATION_2_E:   data = 2;
                                            break;
        case CPSS_LED_BLINK_DURATION_3_E:   data = 3;
                                            break;
        case CPSS_LED_BLINK_DURATION_4_E:   data = 4;
                                            break;
        case CPSS_LED_BLINK_DURATION_5_E:   data = 5;
                                            break;
        case CPSS_LED_BLINK_DURATION_6_E:   data = 6;
                                            break;
        default: return GT_BAD_PARAM;
    }
    U32_SET_FIELD_MAC(regData , 19, 3, data);

    switch(ledConfPtr->blink0DutyCycle)
    {
        case CPSS_LED_BLINK_DUTY_CYCLE_0_E: data = 0;
                                            break;
        case CPSS_LED_BLINK_DUTY_CYCLE_1_E: data = 1;
                                            break;
        case CPSS_LED_BLINK_DUTY_CYCLE_2_E: data = 2;
                                            break;
        case CPSS_LED_BLINK_DUTY_CYCLE_3_E: data = 3;
                                            break;
        default: return GT_BAD_PARAM;
    }
    U32_SET_FIELD_MAC(regData , 22, 2, data);

    switch(ledConfPtr->blink1Duration)
    {
        case CPSS_LED_BLINK_DURATION_0_E:   data = 0;
                                            break;
        case CPSS_LED_BLINK_DURATION_1_E:   data = 1;
                                            break;
        case CPSS_LED_BLINK_DURATION_2_E:   data = 2;
                                            break;
        case CPSS_LED_BLINK_DURATION_3_E:   data = 3;
                                            break;
        case CPSS_LED_BLINK_DURATION_4_E:   data = 4;
                                            break;
        case CPSS_LED_BLINK_DURATION_5_E:   data = 5;
                                            break;
        case CPSS_LED_BLINK_DURATION_6_E:   data = 6;
                                            break;
        default: return GT_BAD_PARAM;
    }
    U32_SET_FIELD_MAC(regData , 24, 3, data);

    switch(ledConfPtr->blink1DutyCycle)
    {
        case CPSS_LED_BLINK_DUTY_CYCLE_0_E: data = 0;
                                            break;
        case CPSS_LED_BLINK_DUTY_CYCLE_1_E: data = 1;
                                            break;
        case CPSS_LED_BLINK_DUTY_CYCLE_2_E: data = 2;
                                            break;
        case CPSS_LED_BLINK_DUTY_CYCLE_3_E: data = 3;
                                            break;
        default: return GT_BAD_PARAM;
    }
    U32_SET_FIELD_MAC(regData , 27, 2, data);

    U32_SET_FIELD_MAC(regData , 29, 1, ((ledConfPtr->disableOnLinkDown == GT_TRUE)?1:0));

    switch(ledConfPtr->ledOrganize)
    {
        case CPSS_LED_ORDER_MODE_BY_PORT_E:     data = 0 ;
                                                break;
        case CPSS_LED_ORDER_MODE_BY_CLASS_E:    data = 1 ;
                                                break;
        default: return GT_BAD_PARAM;
    }
    U32_SET_FIELD_MAC(regData , 30, 1, data);

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 0, 31, regData);
        if( rc != GT_OK )
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledClass[ledInterfaceNum][6];

    regData = 0;

    U32_SET_FIELD_MAC(regData , 16, 1, ((ledConfPtr->clkInvert == GT_TRUE)?1:0));

    switch(ledConfPtr->class5select)
    {
        case CPSS_LED_CLASS_5_SELECT_HALF_DUPLEX_E:     data = 0 ;
                                                        break;
        case CPSS_LED_CLASS_5_SELECT_FIBER_LINK_UP_E:   data = 1 ;
                                                        break;
        default: return GT_BAD_PARAM;
    }
    U32_SET_FIELD_MAC(regData , 18, 1, data);

    switch(ledConfPtr->class13select)
    {
        case CPSS_LED_CLASS_13_SELECT_LINK_DOWN_E:      data = 0 ;
                                                        break;
        case CPSS_LED_CLASS_13_SELECT_COPPER_LINK_UP_E: data = 1 ;
                                                        break;
        default: return GT_BAD_PARAM;
    }
    U32_SET_FIELD_MAC(regData , 19, 1, data);

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId, regAddr, 0xD0000, regData);
        if( rc != GT_OK )
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupConfigGet
*
* DESCRIPTION:
*       This routine gets the LED stream configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*
* OUTPUTS:
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad hardware value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupConfigGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32              ledInterfaceNum,
    OUT CPSS_LED_CONF_STC   *ledConfPtr
)
{
    GT_STATUS rc;           /* return status */
    GT_U32  regData;        /* register led control value */
    GT_U32  regAddr;        /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ledConfPtr);
    PRV_CPSS_DXCH_LED_INTERFACE_NUM_CHECK_MAC(devNum,ledInterfaceNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledControl[ledInterfaceNum];

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr, 0, 31, &regData);
    if( rc != GT_OK )
    {
        return rc;
    }

    ledConfPtr->ledStart = U32_GET_FIELD_MAC(regData, 0, 8);
    ledConfPtr->ledEnd   = U32_GET_FIELD_MAC(regData, 8, 8);

    switch(U32_GET_FIELD_MAC(regData, 16, 3))
    {
        case 0: ledConfPtr->pulseStretch = CPSS_LED_PULSE_STRETCH_0_NO_E;
                break;
        case 1: ledConfPtr->pulseStretch = CPSS_LED_PULSE_STRETCH_1_E;
                break;
        case 2: ledConfPtr->pulseStretch = CPSS_LED_PULSE_STRETCH_2_E;
                break;
        case 3: ledConfPtr->pulseStretch = CPSS_LED_PULSE_STRETCH_3_E;
                break;
        case 4: ledConfPtr->pulseStretch = CPSS_LED_PULSE_STRETCH_4_E;
                break;
        case 5: ledConfPtr->pulseStretch = CPSS_LED_PULSE_STRETCH_5_E;
                break;
        default: return GT_BAD_STATE;
    }

    switch(U32_GET_FIELD_MAC(regData, 19, 3))
    {
        case 0: ledConfPtr->blink0Duration = CPSS_LED_BLINK_DURATION_0_E;
                break;
        case 1: ledConfPtr->blink0Duration = CPSS_LED_BLINK_DURATION_1_E;
                break;
        case 2: ledConfPtr->blink0Duration = CPSS_LED_BLINK_DURATION_2_E;
                break;
        case 3: ledConfPtr->blink0Duration = CPSS_LED_BLINK_DURATION_3_E;
                break;
        case 4: ledConfPtr->blink0Duration = CPSS_LED_BLINK_DURATION_4_E;
                break;
        case 5: ledConfPtr->blink0Duration = CPSS_LED_BLINK_DURATION_5_E;
                break;
        case 6: ledConfPtr->blink0Duration = CPSS_LED_BLINK_DURATION_6_E;
                break;
        default: return GT_BAD_STATE;
    }

    switch(U32_GET_FIELD_MAC(regData, 22, 2))
    {
        case 0: ledConfPtr->blink0DutyCycle = CPSS_LED_BLINK_DUTY_CYCLE_0_E;
                break;
        case 1: ledConfPtr->blink0DutyCycle = CPSS_LED_BLINK_DUTY_CYCLE_1_E;
                break;
        case 2: ledConfPtr->blink0DutyCycle = CPSS_LED_BLINK_DUTY_CYCLE_2_E;
                break;
        case 3: ledConfPtr->blink0DutyCycle = CPSS_LED_BLINK_DUTY_CYCLE_3_E;
                break;
        default: return GT_BAD_STATE;
    }

    switch(U32_GET_FIELD_MAC(regData, 24, 3))
    {
        case 0: ledConfPtr->blink1Duration = CPSS_LED_BLINK_DURATION_0_E;
                break;
        case 1: ledConfPtr->blink1Duration = CPSS_LED_BLINK_DURATION_1_E;
                break;
        case 2: ledConfPtr->blink1Duration = CPSS_LED_BLINK_DURATION_2_E;
                break;
        case 3: ledConfPtr->blink1Duration = CPSS_LED_BLINK_DURATION_3_E;
                break;
        case 4: ledConfPtr->blink1Duration = CPSS_LED_BLINK_DURATION_4_E;
                break;
        case 5: ledConfPtr->blink1Duration = CPSS_LED_BLINK_DURATION_5_E;
                break;
        case 6: ledConfPtr->blink1Duration = CPSS_LED_BLINK_DURATION_6_E;
                break;
        default: return GT_BAD_STATE;
    }

    switch(U32_GET_FIELD_MAC(regData, 27, 2))
    {
        case 0: ledConfPtr->blink1DutyCycle = CPSS_LED_BLINK_DUTY_CYCLE_0_E;
                break;
        case 1: ledConfPtr->blink1DutyCycle = CPSS_LED_BLINK_DUTY_CYCLE_1_E;
                break;
        case 2: ledConfPtr->blink1DutyCycle = CPSS_LED_BLINK_DUTY_CYCLE_2_E;
                break;
        case 3: ledConfPtr->blink1DutyCycle = CPSS_LED_BLINK_DUTY_CYCLE_3_E;
                break;
        default: return GT_BAD_STATE;
    }

    ledConfPtr->disableOnLinkDown = U32_GET_FIELD_MAC(regData, 29, 1) == 1 ? GT_TRUE : GT_FALSE;

    ledConfPtr->ledOrganize = U32_GET_FIELD_MAC(regData, 30, 1) == 0 ?
                                            CPSS_LED_ORDER_MODE_BY_PORT_E :
                                            CPSS_LED_ORDER_MODE_BY_CLASS_E;

    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledClass[ledInterfaceNum][6];

    rc = prvCpssDrvHwPpPortGroupReadRegBitMask(devNum, portGroupId, regAddr, 0xD0000, &regData);
    if( rc != GT_OK )
    {
        return rc;
    }

    ledConfPtr->clkInvert = U32_GET_FIELD_MAC(regData, 16, 1) == 1 ?
                                                                    GT_TRUE : GT_FALSE;

    ledConfPtr->class5select = U32_GET_FIELD_MAC(regData, 18, 1) == 0 ?
                                                    CPSS_LED_CLASS_5_SELECT_HALF_DUPLEX_E:
                                                    CPSS_LED_CLASS_5_SELECT_FIBER_LINK_UP_E;

    ledConfPtr->class13select = U32_GET_FIELD_MAC(regData, 19, 1) == 0 ?
                                                    CPSS_LED_CLASS_13_SELECT_LINK_DOWN_E:
                                                    CPSS_LED_CLASS_13_SELECT_COPPER_LINK_UP_E;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLedStreamClassManipTriSpeedAux
*
* DESCRIPTION:
*       Auxilary function to calculate register address and offsets for class
*       manipulation configuration of tri-speed (network) ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number.
*       classNum        - class number
*
* OUTPUTS:
*       regAddrPtr              - (pointer to) register address
*       invertEnableOffsetPtr   - (pointer to) invert enable offset in register.
*       blinkEnableOffsetPtr    - (pointer to) blink enable offset in register.
*       blinkSelectOffsetPtr    - (pointer to) blink select offset in register.
*       forceEnableOffsetPtr    - (pointer to) force enable offset in register.
*       forceDataOffsetPtr      - (pointer to) force data offset in register.
*       forceDataLengthPtr      - (pointer to) force data length in register.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLedStreamClassManipTriSpeedAux
(
    IN  GT_U8   devNum,
    IN  GT_U32  ledInterfaceNum,
    IN  GT_U32  classNum,
    OUT GT_U32  *regAddrPtr,
    OUT GT_U32  *invertEnableOffsetPtr,
    OUT GT_U32  *blinkEnableOffsetPtr,
    OUT GT_U32  *blinkSelectOffsetPtr,
    OUT GT_U32  *forceEnableOffsetPtr,
    OUT GT_U32  *forceDataOffsetPtr,
    OUT GT_U32  *forceDataLengthPtr
)
{
    switch(classNum)
    {
        case 0:
        case 2:
        case 4:
        case 5:
        case 6: *invertEnableOffsetPtr  = 15 ;
                *blinkEnableOffsetPtr   = 13 ;
                *blinkSelectOffsetPtr   = 14 ;
                *forceEnableOffsetPtr   = 12 ;
                *forceDataOffsetPtr     = 0  ;
                break;
        case 1:
        case 3: *invertEnableOffsetPtr  = 31 ;
                *blinkEnableOffsetPtr   = 29 ;
                *blinkSelectOffsetPtr   = 30 ;
                *forceEnableOffsetPtr   = 28 ;
                *forceDataOffsetPtr     = 16 ;
                break;
        default: return GT_BAD_PARAM;
    }

    *forceDataLengthPtr = 12;

    *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledClass[ledInterfaceNum][classNum];

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupClassManipulationSet
*
* DESCRIPTION:
*       This routine configures a LED stream class manipulation.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number.
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupClassManipulationSet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  GT_U32                          ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT     portType,
    IN  GT_U32                          classNum,
    IN  CPSS_LED_CLASS_MANIPULATION_STC *classParamsPtr
)
{
    GT_STATUS rc;    /* return status */
    GT_U32  regData; /* register led control value */
    GT_U32  regMask; /* register led control mask */
    GT_U32  regAddr; /* register address */
    GT_U32  invertEnableOffset; /* field offset of invert enable */
    GT_U32  blinkEnableOffset;  /* field offset of blink enable */
    GT_U32  blinkSelectOffset;  /* field offset of blink select */
    GT_U32  forceEnableOffset;  /* field offset of force enable */
    GT_U32  forceDataOffset;    /* field offset of force data */
    GT_U32  forceDataLength;  /* field length of force data */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(classParamsPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_CPSS_DXCH_LED_INTERFACE_NUM_CHECK_MAC(devNum,ledInterfaceNum);

    switch(portType)
    {
        case CPSS_DXCH_LED_PORT_TYPE_TRI_SPEED_E:
            rc = prvCpssDxChLedStreamClassManipTriSpeedAux(devNum,
                                                           ledInterfaceNum,
                                                           classNum,
                                                           &regAddr,
                                                           &invertEnableOffset,
                                                           &blinkEnableOffset,
                                                           &blinkSelectOffset,
                                                           &forceEnableOffset,
                                                           &forceDataOffset,
                                                           &forceDataLength);
            break;
        case CPSS_DXCH_LED_PORT_TYPE_XG_E:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                return GT_BAD_PARAM;
            }
            rc = prvCpssDxChLedStreamClassManipXgAux(devNum,
                                                     ledInterfaceNum,
                                                     classNum,
                                                     &regAddr,
                                                     &invertEnableOffset,
                                                     &blinkEnableOffset,
                                                     &blinkSelectOffset,
                                                     &forceEnableOffset,
                                                     &forceDataOffset,
                                                     &forceDataLength);
            break;
        default: return GT_BAD_PARAM;
    }

    if( rc != GT_OK )
    {
        return rc;
    }

    if( classParamsPtr->forceData >= (GT_U32)(1 << forceDataLength) )
    {
        return GT_OUT_OF_RANGE;
    }

    regData = 0x0;
    regMask = 0x0;

    U32_SET_FIELD_MAC(regData , forceDataOffset, forceDataLength, classParamsPtr->forceData);
    U32_SET_FIELD_MAC(regMask , forceDataOffset, forceDataLength, ((1 << forceDataLength)-1));

    U32_SET_FIELD_MAC(regData , forceEnableOffset, 1, ((classParamsPtr->forceEnable == GT_TRUE)?1:0));
    U32_SET_FIELD_MAC(regMask , forceEnableOffset, 1, 1);

    U32_SET_FIELD_MAC(regData , blinkEnableOffset, 1, ((classParamsPtr->blinkEnable == GT_TRUE)?1:0));
    U32_SET_FIELD_MAC(regMask , blinkEnableOffset, 1, 1);

    switch(classParamsPtr->blinkSelect)
    {
        case CPSS_LED_BLINK_SELECT_0_E: U32_SET_FIELD_MAC(regData , blinkSelectOffset, 1, 0);
                                        break;
        case CPSS_LED_BLINK_SELECT_1_E: U32_SET_FIELD_MAC(regData , blinkSelectOffset, 1, 1);
                                        break;
        default: return GT_BAD_PARAM;
    }
    U32_SET_FIELD_MAC(regMask , blinkSelectOffset, 1, 1);

    U32_SET_FIELD_MAC(regData , invertEnableOffset, 1, ((classParamsPtr->invertEnable == GT_TRUE)?1:0));
    U32_SET_FIELD_MAC(regMask , invertEnableOffset, 1, 1);

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupWriteRegBitMask(devNum, portGroupId, regAddr, regMask, regData);
        if( rc != GT_OK )
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupClassManipulationGet
*
* DESCRIPTION:
*       This routine gets a LED stream class manipulation configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number.
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*
* OUTPUTS:
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupClassManipulationGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  GT_U32                          ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT     portType,
    IN  GT_U32                          classNum,
    OUT CPSS_LED_CLASS_MANIPULATION_STC *classParamsPtr
)
{
    GT_STATUS rc;    /* return status */
    GT_U32  regData; /* register led control value */
    GT_U32  regAddr; /* register address */
    GT_U32  invertEnableOffset; /* field offset of invert enable */
    GT_U32  blinkEnableOffset;  /* field offset of blink enable */
    GT_U32  blinkSelectOffset;  /* field offset of blink select */
    GT_U32  forceEnableOffset;  /* field offset of force enable */
    GT_U32  forceDataOffset;    /* field offset of force data */
    GT_U32  forceDataLength;    /* field length of force data */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(classParamsPtr);
    PRV_CPSS_DXCH_LED_INTERFACE_NUM_CHECK_MAC(devNum,ledInterfaceNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);
    switch(portType)
    {
        case CPSS_DXCH_LED_PORT_TYPE_TRI_SPEED_E:
            rc = prvCpssDxChLedStreamClassManipTriSpeedAux(devNum,
                                                           ledInterfaceNum,
                                                           classNum,
                                                           &regAddr,
                                                           &invertEnableOffset,
                                                           &blinkEnableOffset,
                                                           &blinkSelectOffset,
                                                           &forceEnableOffset,
                                                           &forceDataOffset,
                                                           &forceDataLength);
            break;
        case CPSS_DXCH_LED_PORT_TYPE_XG_E:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                return GT_BAD_PARAM;
            }
            rc = prvCpssDxChLedStreamClassManipXgAux(devNum,
                                                     ledInterfaceNum,
                                                     classNum,
                                                     &regAddr,
                                                     &invertEnableOffset,
                                                     &blinkEnableOffset,
                                                     &blinkSelectOffset,
                                                     &forceEnableOffset,
                                                     &forceDataOffset,
                                                     &forceDataLength);
            break;
        default: return GT_BAD_PARAM;
    }

    if( rc != GT_OK )
    {
        return rc;
    }

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regData);
    if( rc != GT_OK )
    {
        return rc;
    }

    classParamsPtr->forceData = U32_GET_FIELD_MAC(regData, forceDataOffset, forceDataLength);
    classParamsPtr->forceEnable = U32_GET_FIELD_MAC(regData, forceEnableOffset, 1) == 1 ?
                                    GT_TRUE : GT_FALSE;

    classParamsPtr->blinkEnable = U32_GET_FIELD_MAC(regData, blinkEnableOffset, 1) == 1 ?
                                    GT_TRUE : GT_FALSE;

    classParamsPtr->blinkSelect = U32_GET_FIELD_MAC(regData, blinkSelectOffset, 1) == 0 ?
                                    CPSS_LED_BLINK_SELECT_0_E : CPSS_LED_BLINK_SELECT_1_E;

    classParamsPtr->invertEnable = U32_GET_FIELD_MAC(regData, invertEnableOffset, 1) == 1 ?
                                    GT_TRUE : GT_FALSE;
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLedStreamGroupConfXgAux
*
* DESCRIPTION:
*       Auxilary function to calculate register address and offset for group
*       configuration for XG (Flex) ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       interfaceNum    - LED stream interface number.
*       groupNum        - group number
*
* OUTPUTS:
*       regAddrPtr              - (pointer to) register address
*       groupDataOffsetPtr      - (pointer to) group data offset in register.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLedStreamGroupConfXgAux
(
    IN  GT_U8   devNum,
    IN  GT_U32  interfaceNum,
    IN  GT_U32  groupNum,
    OUT GT_U32  *regAddrPtr,
    OUT GT_U32  *groupDataOffsetPtr
)
{
    if( groupNum >= 2 )
    {
        return GT_BAD_PARAM;
    }

    *regAddrPtr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ledRegs.ledXgGroup01Configuration[interfaceNum];

    *groupDataOffsetPtr = ( groupNum & 0x1 ) ? 16 : 0 ;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupGroupConfigSet
*
* DESCRIPTION:
*       This routine configures a LED stream group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       groupNum        - group number
*                         For tri-speed ports: range 0..3
*                         For XG ports: range 0..1
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - on out of range class number
*                                  (allowable class range o..15)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupGroupConfigSet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT portType,
    IN  GT_U32                      groupNum,
    IN  CPSS_LED_GROUP_CONF_STC     *groupParamsPtr
)
{
    GT_STATUS rc;               /* return status */
    GT_U32  regData;            /* register led control value */
    GT_U32  regAddr;            /* register address */
    GT_U32  groupDataOffset;    /* offset in register for group data */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(groupParamsPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_CPSS_DXCH_LED_INTERFACE_NUM_CHECK_MAC(devNum,ledInterfaceNum);

    if( (groupParamsPtr->classA > 0xF) ||
        (groupParamsPtr->classB > 0xF) ||
        (groupParamsPtr->classC > 0xF) ||
        (groupParamsPtr->classD > 0xF) )
    {
        return GT_OUT_OF_RANGE;
    }

    switch(portType)
    {
        case CPSS_DXCH_LED_PORT_TYPE_TRI_SPEED_E:
            rc = prvCpssDxChLedStreamGroupConfTriSpeedAux(devNum,
                                                          ledInterfaceNum,
                                                          groupNum,
                                                          &regAddr,
                                                          &groupDataOffset);
            break;
        case CPSS_DXCH_LED_PORT_TYPE_XG_E:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                return GT_BAD_PARAM;
            }
            rc = prvCpssDxChLedStreamGroupConfXgAux(devNum,
                                                    ledInterfaceNum,
                                                    groupNum,
                                                    &regAddr,
                                                    &groupDataOffset);
            break;
        default: return GT_BAD_PARAM;
    }

    if( rc != GT_OK )
    {
        return rc;
    }

    regData = 0;
    U32_SET_FIELD_MAC(regData , 0, 4, groupParamsPtr->classA);
    U32_SET_FIELD_MAC(regData , 4, 4, groupParamsPtr->classB);
    U32_SET_FIELD_MAC(regData , 8, 4, groupParamsPtr->classC);
    U32_SET_FIELD_MAC(regData , 12, 4, groupParamsPtr->classD);

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                groupDataOffset, 16, regData);
        if( rc != GT_OK )
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupGroupConfigGet
*
* DESCRIPTION:
*       This routine gets a LED stream group configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       groupNum        - group number
*                         For tri-speed ports: range 0..3
*                         For XG ports: range 0..1
*
* OUTPUTS:
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupGroupConfigGet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT portType,
    IN  GT_U32                      groupNum,
    OUT CPSS_LED_GROUP_CONF_STC     *groupParamsPtr
)
{
    GT_STATUS rc;               /* return status */
    GT_U32  regData;            /* register led control value */
    GT_U32  regAddr;            /* register address */
    GT_U32  groupDataOffset;    /* offset in register for group data */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(groupParamsPtr);
    PRV_CPSS_DXCH_LED_INTERFACE_NUM_CHECK_MAC(devNum,ledInterfaceNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    switch(portType)
    {
        case CPSS_DXCH_LED_PORT_TYPE_TRI_SPEED_E:
            rc = prvCpssDxChLedStreamGroupConfTriSpeedAux(devNum,
                                                          ledInterfaceNum,
                                                          groupNum,
                                                          &regAddr,
                                                          &groupDataOffset);
            break;
        case CPSS_DXCH_LED_PORT_TYPE_XG_E:
            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                return GT_BAD_PARAM;
            }
            rc = prvCpssDxChLedStreamGroupConfXgAux(devNum,
                                                    ledInterfaceNum,
                                                    groupNum,
                                                    &regAddr,
                                                    &groupDataOffset);
            break;
        default: return GT_BAD_PARAM;
    }

    if( rc != GT_OK )
    {
        return rc;
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                            groupDataOffset, 16, &regData);
    if( rc != GT_OK )
    {
        return rc;
    }

    groupParamsPtr->classA = U32_GET_FIELD_MAC(regData, 0, 4);
    groupParamsPtr->classB = U32_GET_FIELD_MAC(regData, 4, 4);
    groupParamsPtr->classC = U32_GET_FIELD_MAC(regData, 8, 4);
    groupParamsPtr->classD = U32_GET_FIELD_MAC(regData, 12, 4);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLedStreamSyncLedsEnableSet
*
* DESCRIPTION:
*       This routine enables\disables the global state in which the two LED 
*       streams are synchronized.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE: LED streams are synchronized
*                 GT_FALSE: LED streams are not synchronized
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamSyncLedsEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_BOOL enable
)
{
    GT_STATUS rc;       /* return status */
    GT_U32  regAddr;    /* register address */
    GT_U32  regData;    /* register value */
    
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);

    regData = (GT_TRUE == enable ) ? 1 : 0 ;
    
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[0];

    /* enable\disable synchronization bit */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 14, 1, regData);
    if( rc != GT_OK )
    {
        return rc;
    }

    /* Only for enabling operation Restarts LED is needed */
    if( GT_TRUE == enable )
    {
        return prvCpssDrvHwPpSetRegField(devNum, regAddr, 15, 1, 1);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChLedStreamSyncLedsEnableGet
*
* DESCRIPTION:
*       This routine gets the global state indicating if the two LED streams 
*       are synchronized.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE: LED streams are synchronized
*                                  GT_FALSE: LED streams are not synchronized
*       inSyncTrigPtr   - (pointer to) GT_TRUE: synchronization triggering
*                                               not finished yet.
*                                      GT_FALSE: synchronization triggering
*                                                finished.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamSyncLedsEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr,
    OUT GT_BOOL *inSyncTrigPtr
)
{
    GT_STATUS rc;               /* return status */
    GT_U32  regAddr;            /* register address */
    GT_U32  regData;            /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(inSyncTrigPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->macRegs.macCountersCtrl[0];

    /* get LED synchronization & Restarts LED states */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 14, 2, &regData);
    if( rc != GT_OK )
    {
        return rc;
    }

    *enablePtr = ((regData & 1) == 0) ? GT_FALSE : GT_TRUE ;

    *inSyncTrigPtr = ((regData & 2) == 0) ? GT_FALSE : GT_TRUE ;

    return GT_OK;
}

