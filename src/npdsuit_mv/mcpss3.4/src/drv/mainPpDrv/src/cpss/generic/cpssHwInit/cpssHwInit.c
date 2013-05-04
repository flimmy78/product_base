/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssHwInit.c
*
* DESCRIPTION:
*       Includes CPSS  level basic HW initialization functions.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwInit.h>
#include <cpss/generic/extMac/cpssExtMacDrv.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>


PRV_CPSS_GEN_GLOBAL_DATA_STC sysGenGlobalInfo = {{0}};

/* define the global DB about the per system info */
PRV_CPSS_SYS_CONFIG_INFO_STC   prvCpssSysCfgInfo;

PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_ENT prvCpssPrePhase1PpInitMode =
                                      PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_DEFAULT_INIT_E;
/* define array of pointers to the valid devices */
void* prvCpssPpConfig[PRV_CPSS_MAX_PP_DEVICES_CNS];

/* array of pointers to the valid families
(index to array is family type - one of CPSS_PP_FAMILY_TYPE_ENT) */
PRV_CPSS_FAMILY_INFO_STC* prvCpssFamilyInfoArray[CPSS_PP_FAMILY_LAST_E];



#define BASE_ADDR_NOT_USED_CNS 0xffffffff


/* define the HSU  global DB */
CPSS_HSU_INFO_STC   hsuInfo;
/*******************************************************************************
* prvCpssHwIfSet
*
* DESCRIPTION:
*       This function sets cpss HW interface parameters.
*       called during "phase1" initialization .
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       mngInterfaceType  - Management interface type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM             - on bad device number or interface type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssHwIfSet(
    IN GT_U8                            devNum,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT    mngInterfaceType
)
{
    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }

    if((GT_U32)mngInterfaceType > 31)
    {
        return GT_BAD_PARAM;
    }

    PRV_CPSS_PP_MAC(devNum)->hwIfSupportedBmp = PRV_CPSS_HW_IF_BMP_MAC(mngInterfaceType);

    if(mngInterfaceType == CPSS_CHANNEL_PEX_E)
    {
        /* in most cases we will treat the "PCI" and "PEX" the same */
        PRV_CPSS_PP_MAC(devNum)->hwIfSupportedBmp |= PRV_CPSS_HW_IF_BMP_MAC(CPSS_CHANNEL_PCI_E);
    }

    return GT_OK;

}

/*******************************************************************************
* prvCpssNonMultiPortGroupsDeviceSet
*
* DESCRIPTION:
*       This function sets cpss DB to be 'non-multi-port-groups' device.
*       It is called during "phase1" initialization.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM             - on bad device number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssNonMultiPortGroupsDeviceSet(
    IN GT_U8                            devNum
)
{
    GT_U32              firstActivePortGroupId;/* port group Id for the first active port group */
    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr;/* pointer to common device info */

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }

    devPtr = PRV_CPSS_PP_MAC(devNum);

    /* fill the 'multi port groups' info for the simplicity of the macros that
       needed to support loops on port group !!! */
    devPtr->portGroupsInfo.isMultiPortGroupDevice = GT_FALSE;
    devPtr->portGroupsInfo.numOfPortGroups = 1;
    firstActivePortGroupId = CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS;
    devPtr->portGroupsInfo.activePortGroupsBmp        = 1 << firstActivePortGroupId;
    devPtr->portGroupsInfo.firstActivePortGroup       = firstActivePortGroupId;
    devPtr->portGroupsInfo.lastActivePortGroup        = firstActivePortGroupId;
    devPtr->portGroupsInfo.cpuPortPortGroupId         = firstActivePortGroupId;
    devPtr->portGroupsInfo.auqPortGroupIdLastServed   = firstActivePortGroupId;
    devPtr->portGroupsInfo.fuqPortGroupIdLastServed   = firstActivePortGroupId;
    devPtr->portGroupsInfo.secureBreachNextPortGroupToServe = firstActivePortGroupId;
    devPtr->portGroupsInfo.ptpNextPortGroupToServe    = firstActivePortGroupId;

    /* from this point on we can start use the macros of:
       PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC,
       PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC
    */

    return GT_OK;
}



/*******************************************************************************
* prvCpssSysConfigPhase1
*
* DESCRIPTION:
*       This function sets cpss system level system configuration parameters
*       before any of the device's phase1 initialization .
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssSysConfigPhase1
(
    void
)
{
    GT_U32  ii;

    /* initializing the global DB */
    cpssOsMemSet(&prvCpssSysCfgInfo,0,sizeof(prvCpssSysCfgInfo));
    cpssOsMemSet(&sysGenGlobalInfo,0,sizeof(sysGenGlobalInfo));
    /* initialization of HSU global DB */
    cpssOsMemSet(&hsuInfo,0,sizeof(hsuInfo));
    hsuInfo.hsuState = CPSS_HSU_STATE_COMPLETED_E;
    /* initializing the per PP DB */
    cpssOsMemSet(prvCpssPpConfig,0,sizeof(prvCpssPpConfig));
    /* initializing the per FAMILY DB */
    cpssOsMemSet(&prvCpssFamilyInfoArray,0,sizeof(prvCpssFamilyInfoArray));
    sysGenGlobalInfo.prvMiiDevNum = PRV_CPSS_GEN_NETIF_MII_DEV_NOT_SET;
    sysGenGlobalInfo.prvCpssGenNetIfMiiCtrl.rxValid = GT_FALSE;
    sysGenGlobalInfo.prvCpssGenNetIfMiiCtrl.txValid = GT_FALSE;
    if(sysGenGlobalInfo.prvCpssAfterDbRelease == GT_FALSE)
    {
        /* initialize only once */
        for (ii = 0 ; ii < PRV_CPSS_MAX_PP_DEVICES_CNS; ii++)
        {
            sysGenGlobalInfo.reInitializationDbPtr[ii].busBaseAddr = BASE_ADDR_NOT_USED_CNS;
            sysGenGlobalInfo.reInitializationDbPtr[ii].deviceDidHwReset = GT_TRUE;
            sysGenGlobalInfo.reInitializationDbPtr[ii].deviceNeedNetIfInit = GT_TRUE;
            sysGenGlobalInfo.reInitializationDbPtr[ii].dmaInfoValid = GT_FALSE;
        }
    }


    return GT_OK;
}


/*******************************************************************************
* cpssPpInterruptsDisable
*
* DESCRIPTION:
*       This API disable PP interrupts
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - The device number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssPpInterruptsDisable
(
    IN GT_U8     devNum
)
{
    GT_U32 value;
    GT_STATUS rc;
    GT_32 key = 0;
    GT_U32 portGroupId = 0;
    /* lock interrupts */
    cpssOsTaskLock();
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &key);
    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            /* mask all interrupts */
            value = 0x0;
            rc = prvCpssDrvExMxDxHwPpInterruptsMaskToDefault(devNum,portGroupId);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    /* unlock interrupts */
    cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &key);
    cpssOsTaskUnLock();

    return GT_OK;
}




/*******************************************************************************
* cpssPpConfigDevDataImport
*
* DESCRIPTION:
*       Imports data for device specific information.  Used in catch up process
*       during initialization as the first step in the catch up process for
*       configuring the standby CPU.
*
*       NOTE: for a device that will be initialized in "pp phase 1" with
*       High availability mode of "standby" , this function MUST be called prior
*       to the "pp phase 1"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                   - The device number.
*       configDevDataBufferPtr   - pointer to a pre allocated buffer for holding
*                                    information on devices in the system
*       configDevDataBufferSize  - size of the pre allocated buffer
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                  - on success.
*       GT_BAD_PARAM           - wrong device Number or configDevDataBufferSize
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssPpConfigDevDataImport
(
    IN GT_U8     devNum,
    IN void      *configDevDataBufferPtr,
    IN GT_U32    configDevDataBufferSize
)
{
    /* don't check if the device exists !!! */
    /* since it may be before "phase 1 init" */

    /* check only the range */
    if((devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS) ||
       (configDevDataBufferSize < sizeof(PRV_CPSS_PP_HA_CONFIG_INFO_STC)))
    {
        return GT_BAD_PARAM;
    }
    sysGenGlobalInfo.cpssPpHaDevData[devNum] = configDevDataBufferPtr;
    return GT_OK;
}

/*******************************************************************************
* cpssPpConfigDevDataExport
*
* DESCRIPTION:
*       Imports data for device specific information.  Used in catch up process
*       during initialization as the first step in the catch up process for
*       configuring the standby CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*       configDevDataBufferPtr      - pointer to a pre allocated buffer for
*                                     holding information on devices in the system
*       configDevDataBufferSizePtr  - size of the pre allocated buffer
*
* OUTPUTS:
*       configDevDataBufferPtr      - pointer to a pre allocated buffer for
*                                     holding information on devices in the system
*                                     filled by the function.
*       configDevDataBufferSizePtr  - size of the filled data in the pre
*                                     allocated buffer, in case of failure
*                                     (GT_BAD_SIZE), needed size of the block
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_SIZE              - block is not big enough to hold all the data
*       GT_BAD_PARAM             - wrong device Number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssPpConfigDevDataExport
(
    IN    GT_U8     devNum,
    INOUT void      *configDevDataBufferPtr,
    INOUT GT_U32    *configDevDataBufferSizePtr
)
{
    PRV_CPSS_PP_HA_CONFIG_INFO_STC *devData;

    if(!PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }

    if(*configDevDataBufferSizePtr < sizeof(PRV_CPSS_PP_HA_CONFIG_INFO_STC))
    {
        *configDevDataBufferSizePtr = sizeof(PRV_CPSS_PP_HA_CONFIG_INFO_STC);
        return GT_BAD_SIZE;
    }

    devData = configDevDataBufferPtr;

    /********************/
    /* export next info */
    /********************/
    devData->deviceId   = PRV_CPSS_PP_MAC(devNum)->devType;
    devData->revision   = PRV_CPSS_PP_MAC(devNum)->revision;

    *configDevDataBufferSizePtr = sizeof(PRV_CPSS_PP_HA_CONFIG_INFO_STC);

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigDevDbRelease
*
* DESCRIPTION:
*       private (internal) function to release all the DB of the device.
*       NOTE: function 'free' the allocated memory ,buffers, semaphores
*             and restore DB to 'pre-init' state
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbRelease
(
    IN    GT_U8     devNum
)
{
    GT_STATUS rc;
    GT_U32    ii;
    PRV_CPSS_GEN_PP_CONFIG_STC* devPtr = PRV_CPSS_PP_MAC(devNum);
    GT_U32  queue;/*rx/tx queue*/

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        /* device not found */
        return GT_BAD_PARAM;
    }

    /* save the state of the AUQ,FUQ,CNC HW positions */
    rc = prvCpssPpConfigDevDbPrvInfoSet(devNum,
        &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[0]),
        &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[0]),
        &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[0]),
        &(PRV_CPSS_PP_MAC(devNum)->intCtrl.rxDescList[0]));
    if(rc != GT_OK)
    {
        return rc;
    }


    /* UN-bind the old device from the DSMA Tx completed callback to the driver */
    rc = prvCpssDrvInterruptPpTxEndedCbBind(devNum,NULL);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check if we can detach from the DB of 'per family' */
    if(prvCpssFamilyInfoArray[devPtr->devFamily] && prvCpssFamilyInfoArray[devPtr->devFamily]->numReferences)
    {
        prvCpssFamilyInfoArray[devPtr->devFamily]->numReferences--;
        if(prvCpssFamilyInfoArray[devPtr->devFamily]->numReferences == 0)
        {
            /* no more references to this DB , so release it */
            FREE_PTR_MAC(prvCpssFamilyInfoArray[devPtr->devFamily]);
        }
    }

    for(queue = 0; queue < NUM_OF_TX_QUEUES; queue++)
    {
        FREE_PTR_MAC(devPtr->intCtrl.txDescList[queue].swTxDescBlock);
        FREE_PTR_MAC(devPtr->intCtrl.rxDescList[queue].swRxDescBlock);

        if(devPtr->intCtrl.txDescList[queue].poolId)
        {
            rc = cpssBmPoolDelete(devPtr->intCtrl.txDescList[queue].poolId);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    FREE_PTR_MAC(devPtr->phyPortInfoArray);
    FREE_PTR_MAC(devPtr->trunkInfo.trunksArray);
    FREE_PTR_MAC(devPtr->trunkInfo.allMembersArray);
    FREE_PTR_MAC(devPtr->trunkInfo.allMembersIsLocalArray);

    /* set the device info pointer as NULL --
       so we can do "phase 1" for a device with this devNum */
    devPtr = NULL;
    FREE_PTR_MAC(prvCpssPpConfig[devNum]);
    sysGenGlobalInfo.cpssPpHaDevData[devNum] = NULL;


    /* check if there are still existing families (quicker then loop on devices) */
    for(ii = 0 ; ii < CPSS_PP_FAMILY_LAST_E ; ii++)
    {
        if(prvCpssFamilyInfoArray[ii])
        {
            break;
        }
    }

    if(ii == CPSS_PP_FAMILY_LAST_E)
    {
        /* no more devices */


        /* release VCT DB */
        rc = prvCpssVctDbRelease();
        if(rc != GT_OK)
        {
            return rc;
        }

        cpssOsMemSet(&prvCpssSysCfgInfo,0,sizeof(prvCpssSysCfgInfo));

        /* no more existing devices */
        sysGenGlobalInfo.prvCpssAfterDbRelease = GT_TRUE;
    }


    /*  let the cpssDriver do the same "remove" of the device. */
    rc = prvCpssDrvHwPpDevRemove(devNum);

    return rc;
}

/*******************************************************************************
* prvCpssPpConfigDevDbBusBaseAddrAttach
*
* DESCRIPTION:
*       private (internal) function to attach the base address to devNum
*
*       this needed for the 'initialization after the DB release processing'
*       when the initialization for a device during 'phase 1' will be we will
*       know what 'old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*       busBaseAddr -  the address of the device on the interface bus (pci/pex/smi/twsi)
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbBusBaseAddrAttach
(
    IN GT_U8        devNum,
    IN GT_U32       busBaseAddr
)
{
    GT_U32      ii;

    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* look for this base address if already in the DB , because if already in DB
    we need to move info from that device to the 'new device' */
    for(ii = 0 ; ii < PRV_CPSS_MAX_PP_DEVICES_CNS; ii++)
    {
        if(sysGenGlobalInfo.reInitializationDbPtr[ii].busBaseAddr == busBaseAddr)
        {
            break;
        }
    }

    if(ii == PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        /* this is a new base address */
        /* just set it into DB , for the device */
        sysGenGlobalInfo.reInitializationDbPtr[devNum].busBaseAddr = busBaseAddr;
        sysGenGlobalInfo.reInitializationDbPtr[devNum].deviceDidHwReset = GT_TRUE;
        sysGenGlobalInfo.reInitializationDbPtr[devNum].deviceNeedNetIfInit = GT_TRUE;
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfoValid = GT_FALSE;
    }
    else if(ii != devNum)/* found the bus base address on 'old' device number */
    {
        /* move the info from the 'old device' to the new device */
        cpssOsMemCpy(&sysGenGlobalInfo.reInitializationDbPtr[devNum],
                     &sysGenGlobalInfo.reInitializationDbPtr[ii],
                     sizeof(sysGenGlobalInfo.reInitializationDbPtr[0]));

        /*restore old device to init values */
        sysGenGlobalInfo.reInitializationDbPtr[ii].busBaseAddr = BASE_ADDR_NOT_USED_CNS;
        sysGenGlobalInfo.reInitializationDbPtr[ii].deviceDidHwReset = GT_TRUE;
        sysGenGlobalInfo.reInitializationDbPtr[ii].deviceNeedNetIfInit = GT_TRUE;
        sysGenGlobalInfo.reInitializationDbPtr[ii].dmaInfoValid = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigDevDbInfoSet
*
* DESCRIPTION:
*       private (internal) function to :
*       1. first call for device or device did HW reset :
*           function set the info about the device regarding DMA allocations
*           given by the application.
*       2. else the function check that the info given now match the info given
*          of first time after the HW reset.
*
*       this needed for the 'Initialization after the DB release processing'
*       when the initialization for a device during 'Phase 1' will be we will
*       know what 'Old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*       netIfCfgPtr - (pointer to)Network interface configuration.
*       auqCfgPtr - (pointer to)AU queue info
*       fuqUseSeparate - do we have another Queue for FU message
*       fuqCfgPtr - (pointer to)FU queue info
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_STATE             - the new info it not the same info that was
*                                  set for this device (according to the bus
*                                  base address) before the 'shutdown'
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*       GT_BAD_PTR               - one of the pointers is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbInfoSet
(
    IN GT_U8                        devNum,
    IN CPSS_NET_IF_CFG_STC          *netIfCfgPtr,
    IN CPSS_AUQ_CFG_STC             *auqCfgPtr,
    IN GT_BOOL                      fuqUseSeparate,
    IN CPSS_AUQ_CFG_STC             *fuqCfgPtr
)
{
    /* validate the pointers */
    CPSS_NULL_PTR_CHECK_MAC(auqCfgPtr);
    if(fuqUseSeparate == GT_TRUE)
    {
        CPSS_NULL_PTR_CHECK_MAC(fuqCfgPtr);
    }

    if(! PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    else if(sysGenGlobalInfo.reInitializationDbPtr[devNum].busBaseAddr == BASE_ADDR_NOT_USED_CNS)
    {
        return GT_NOT_FOUND;
    }
    else if(! PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum))
    {
        /* all current info relate to DMA so currently not relevant to non
        PCI/PEX devices */
        return GT_OK;
    }

    if(sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfoValid == GT_FALSE ||
       sysGenGlobalInfo.reInitializationDbPtr[devNum].deviceDidHwReset == GT_TRUE)
    {
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.auqCfg         = (*auqCfgPtr);
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.fuqUseSeparate = fuqUseSeparate;
        if(fuqCfgPtr)
        {
            sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.fuqCfg         = (*fuqCfgPtr);
        }

        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfoValid = GT_TRUE;

        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.netIfCfg = (*netIfCfgPtr);
/*        reInitializationDbPtr[devNum].deviceDidHwReset = GT_FALSE; -->
         the setting to GT_FALSE need to come from prvCpssPpConfigDevDbHwResetSet */

        /* we are done with saving the info (for possible use after shutdown) */
        return GT_OK;
    }

    /* we need to check that the DMA sizes not changed from the previous
       initialization , because we can't modify HW values */

    if(cpssOsMemCmp(&sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.auqCfg,
                    auqCfgPtr,
                    sizeof(sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.auqCfg)))
    {
        return GT_BAD_STATE;
    }

    if(fuqUseSeparate == GT_TRUE)
    {
        if(GT_TRUE == sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.fuqUseSeparate)
        {
            if(cpssOsMemCmp(&sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.fuqCfg,
                            fuqCfgPtr,
                            sizeof(sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.fuqCfg)))
            {
                return GT_BAD_STATE;
            }
        }
        else
        {
            /* we now use the FU queue even we not used it before , it is valid
               state */
            /*so we need to update the initial DB now */
            sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.fuqUseSeparate = GT_TRUE;
            sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.fuqCfg         = (*fuqCfgPtr);
        }
    }
    else
    {
        /* we not care that there was DMA from FU queue and there non now */
        /* because the PP will not access this DMA any more */
        /* BUT we stay with the initial state of
           reInitializationDbPtr[devNum].dmaInfo.fuqUseSeparate
           because if there was before DMA for FU queue we need to remember that*/
        /* so no check need to be done */
    }

    /* check that the Network interface configuration did not changed from the */
    /* previous initialization */
    if(cpssOsMemCmp(&sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.netIfCfg,
                    netIfCfgPtr,
                    sizeof(sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.netIfCfg)))
    {
        return GT_BAD_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigDevDbPrvInfoSet
*
* DESCRIPTION:
*       private (internal) function to :
*       set the AUQ,FUQ private cpss info.
*
*       this needed for the 'Initialization after the DB release processing'
*       when the initialization for a device during 'Phase 1' will be we will
*       know what 'Old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*       auqInternalInfoPtr - (pointer to)AU queue private info , of all port groups
*       fuqInternalInfoPtr - (pointer to)FU queue private info , of all port groups
*       secondaryAuqInternalInfoPtr - (pointer to)secondary AU queue private info , of all port groups
*       rxDescListPtr      - (pointer to)Rx descriptors list private info
* OUTPUTS:
*       none
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*
* COMMENTS:
*      NOTE: not checking for NULL pointers to speed up performances
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbPrvInfoSet
(
    IN GT_U8                        devNum,
    IN PRV_CPSS_AU_DESC_CTRL_STC    *auqInternalInfoPtr,
    IN PRV_CPSS_AU_DESC_CTRL_STC    *fuqInternalInfoPtr,
    IN PRV_CPSS_AU_DESC_CTRL_STC    *secondaryAuqInternalInfoPtr,
    IN PRV_CPSS_RX_DESC_LIST_STC    *rxDescListPtr
)
{
    GT_U32 i;

    if(! PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    else if(sysGenGlobalInfo.reInitializationDbPtr[devNum].busBaseAddr == BASE_ADDR_NOT_USED_CNS)
    {
        /* Soft Reset was done. The PP DMAs states were changed. No need to store it. */
        return GT_OK;
    }
    else if (sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfoValid == GT_FALSE)
    {
        return GT_OK;
    }

    if(sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.fuqUseSeparate == GT_TRUE)
    {
        for( i = 0 ; i < CPSS_MAX_PORT_GROUPS_CNS ; i++ )
        {
            sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.fuqInternalInfo[i] = fuqInternalInfoPtr[i];
        }
    }

    for( i = 0 ; i < CPSS_MAX_PORT_GROUPS_CNS ; i++ )
    {
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.auqInternalInfo[i] = auqInternalInfoPtr[i];
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.secondaryAuqInternalInfo[i] = secondaryAuqInternalInfoPtr[i];
    }

    for( i = 0 ; i < NUM_OF_RX_QUEUES ; i++ )
    {
        /* "swRxDescBlock" is start of array of Rx descriptors, while "next2Return" and "next2Receive"   */
        /* are pointers to descriptors in this array. After the SW restart operation the array is not    */
        /* valid any more and we have to reallocate it. The new next2Return and next2Receive pointers    */
        /* will have to point to descriptors in the new allocated array with the same offsets from       */
        /* array start as were in the original array, therefore offsets are calculated here since the    */
        /* pointers value themselves will have no meaning after the array pointed now by "swRxDescBlock" */
        /* is released.                                                                                  */
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[i].next2ReturnOffset =
            rxDescListPtr[i].next2Return - (PRV_CPSS_SW_RX_DESC_STC *)rxDescListPtr[i].swRxDescBlock;
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[i].next2ReceiveOffset =
            rxDescListPtr[i].next2Receive - (PRV_CPSS_SW_RX_DESC_STC *)rxDescListPtr[i].swRxDescBlock;
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[i].freeDescNum = rxDescListPtr[i].freeDescNum;
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[i].headerOffset = rxDescListPtr[i].headerOffset;
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[i].forbidQEn = rxDescListPtr[i].forbidQEn;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigDevDbPrvInfoGet
*
* DESCRIPTION:
*       private (internal) function to :
*       get the AUQ,FUQ private cpss info.
*
*       this needed for the 'Initialization after the DB release processing'
*       when the initialization for a device during 'Phase 1' will be we will
*       know what 'Old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*       portGroupId - The port group Id , to support multi-port-groups device
* OUTPUTS:
*       auqInternalInfoPtr - (pointer to)AU queue private info
*       fuqInternalInfoPtr - (pointer to)FU queue private info (can be NULL when FUQ not used)
*       secondaryAuqInternalInfoPtr - (pointer to)secondary AU queue private info (can be NULL when queue not used)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*       GT_BAD_PTR               - one of the pointers is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbPrvInfoGet
(
    IN GT_U8                        devNum,
    IN GT_U32                       portGroupId,
    OUT PRV_CPSS_AU_DESC_CTRL_STC    *auqInternalInfoPtr,
    OUT PRV_CPSS_AU_DESC_CTRL_STC    *fuqInternalInfoPtr,
    OUT PRV_CPSS_AU_DESC_CTRL_STC    *secondaryAuqInternalInfoPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(auqInternalInfoPtr);

    if(! PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    else if(sysGenGlobalInfo.reInitializationDbPtr[devNum].busBaseAddr == BASE_ADDR_NOT_USED_CNS)
    {
        return GT_NOT_FOUND;
    }
    else if (sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfoValid == GT_FALSE)
    {
        /* no meaning in non- DMA device */
        return GT_OK;
    }
    else if(portGroupId >= CPSS_MAX_PORT_GROUPS_CNS)
    {
        return GT_BAD_PARAM;
    }

    if(sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.fuqUseSeparate == GT_TRUE)
    {
        CPSS_NULL_PTR_CHECK_MAC(fuqInternalInfoPtr);
        *fuqInternalInfoPtr = sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.fuqInternalInfo[portGroupId];
    }

    *auqInternalInfoPtr = sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.auqInternalInfo[portGroupId];

    if(secondaryAuqInternalInfoPtr)
    {
        *secondaryAuqInternalInfoPtr = sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.secondaryAuqInternalInfo[portGroupId];
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigDevDbPrvNetInfInfoSet
*
* DESCRIPTION:
*       private (internal) function to :
*       set the Rx descriptors list private cpss info.
*
*       this needed for the 'Initialization after the DB release processing'
*       when the initialization for a device during 'Phase 1' will be we will
*       know what 'Old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - The device number.
*       rxDescPtr          - (pointer to) the start of Rx descriptors block.
*       rxDescListPtr      - (pointer to)Rx descriptors list private info.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*       GT_BAD_PTR               - one of the pointers is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbPrvNetInfInfoSet
(
    IN GT_U8                        devNum,
    IN PRV_CPSS_RX_DESC_STC         *rxDescPtr,
    IN PRV_CPSS_RX_DESC_LIST_STC    *rxDescListPtr
)
{
    GT_U32 i;

    if(! PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    else if(sysGenGlobalInfo.reInitializationDbPtr[devNum].busBaseAddr == BASE_ADDR_NOT_USED_CNS)
    {
        return GT_NOT_FOUND;
    }
    else if (sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfoValid == GT_FALSE)
    {
        /* no meaning in non- DMA device */
        return GT_OK;
    }

    for( i = 0 ; i < NUM_OF_RX_QUEUES ; i++ )
    {
        /* The Rx descriptors reside in memory as an array, the start of which is at "rxDescPtr". */
        /* For each queue we save for later use after SW restart a pointer to where the block of  */
        /* descriptors belonging to that queue start. Since they all reside sequentially in the   */
        /* same array, we simply have to increment the start pointer between loop cycle in the    */
        /* amount of descriptors of each queue.                                                   */
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[i].rxDescBlock = rxDescPtr;
        rxDescPtr += rxDescListPtr[i].freeDescNum;
        /* At this stage the value of "freeDescNum" is equal to the total number of existing */
        /* descriptors, a value that is used later after SW restart to allocate the required */
        /* memory for the SW Rx descriptors.                                                 */
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[i].totalDescNum = rxDescListPtr[i].freeDescNum;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigDevDbPrvNetInfInfoGet
*
* DESCRIPTION:
*       private (internal) function to :
*       get the Rx descriptors list private cpss info.
*
*       this needed for the 'Initialization after the DB release processing'
*       when the initialization for a device during 'Phase 1' will be we will
*       know what 'Old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
* OUTPUTS:
*       rxDescListPtr      - (pointer to) Rx descriptors list private info
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbPrvNetInfInfoGet
(
    IN GT_U8                        devNum,
    OUT PRV_CPSS_RX_DESC_LIST_STC   *rxDescListPtr
)
{
    PRV_CPSS_SW_RX_DESC_STC *firstSwRxDesc; /* Points to the first Sw Rx desc  */
    PRV_CPSS_SW_RX_DESC_STC *swRxDesc;      /* Points to the Sw Rx desc to init. */
    GT_U32 sizeOfDesc;          /* The amount of memory (in bytes) that*/
                                /* a single desc. will occupy, including*/
                                /* the alignment.                       */
    GT_U8 i, rxQueue;                       /* Loop counters */

    if(! PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    else if(sysGenGlobalInfo.reInitializationDbPtr[devNum].busBaseAddr == BASE_ADDR_NOT_USED_CNS)
    {
        return GT_NOT_FOUND;
    }
    else if (sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfoValid == GT_FALSE)
    {
        /* no meaning in non- DMA device */
        return GT_OK;
    }

    for( rxQueue = 0 ; rxQueue < NUM_OF_RX_QUEUES ; rxQueue++ )
    {
        firstSwRxDesc = (PRV_CPSS_SW_RX_DESC_STC*)cpssOsMalloc(sizeof(PRV_CPSS_SW_RX_DESC_STC)*
                        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[rxQueue].totalDescNum);
        if(firstSwRxDesc == NULL)
        {
            return GT_FAIL;
        }

        /* save pointer to allocated Mem block on per queue structure */
        rxDescListPtr[rxQueue].swRxDescBlock = firstSwRxDesc;

        rxDescListPtr[rxQueue].next2Return =
            firstSwRxDesc + sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[rxQueue].next2ReturnOffset;
        rxDescListPtr[rxQueue].next2Receive =
            firstSwRxDesc + sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[rxQueue].next2ReceiveOffset;
        rxDescListPtr[rxQueue].freeDescNum = sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[rxQueue].freeDescNum;
        rxDescListPtr[rxQueue].headerOffset = sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[rxQueue].headerOffset;
        rxDescListPtr[rxQueue].forbidQEn = sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[rxQueue].forbidQEn;

        sizeOfDesc = sizeof(PRV_CPSS_RX_DESC_STC);
        if((sizeOfDesc % RX_DESC_ALIGN) != 0)
        {
            sizeOfDesc += (RX_DESC_ALIGN -(sizeof(PRV_CPSS_RX_DESC_STC) % RX_DESC_ALIGN));
        }

        swRxDesc = NULL;
        for(i = 0; i < sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[rxQueue].totalDescNum; i++)
        {
            swRxDesc = firstSwRxDesc + i;

            swRxDesc->rxDesc = (PRV_CPSS_RX_DESC_STC*)
                ((GT_U32)sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfo.rxDescInfo[rxQueue].rxDescBlock + i*sizeOfDesc) ;

            swRxDesc->swNextDesc  = (firstSwRxDesc + i + 1);
        }

        if( swRxDesc != NULL )
        {
            /* Close the cyclic desc. list. */
            swRxDesc->swNextDesc = firstSwRxDesc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigDevDbHwResetSet
*
* DESCRIPTION:
*       private (internal) function to state that the device did HW reset or that
*       the device finished the 'pp logic init' function
*
*       this needed for the 'initialization after the DB release processing'
*       when the initialization for a device during 'phase 1' will be we will
*       know what 'old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number
*       didHwReset - GT_TRUE - Did HW reset ,
*                    GT_FALSE -  finished pp logical init.
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbHwResetSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   didHwReset
)
{
    if(! PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    else if(sysGenGlobalInfo.reInitializationDbPtr[devNum].busBaseAddr == BASE_ADDR_NOT_USED_CNS)
    {
        return GT_NOT_FOUND;
    }

    if(didHwReset == GT_TRUE)
    {
        sysGenGlobalInfo.reInitializationDbPtr[devNum].busBaseAddr = BASE_ADDR_NOT_USED_CNS;
        sysGenGlobalInfo.reInitializationDbPtr[devNum].deviceDidHwReset = GT_TRUE;
        sysGenGlobalInfo.reInitializationDbPtr[devNum].deviceNeedNetIfInit = GT_TRUE;
        sysGenGlobalInfo.reInitializationDbPtr[devNum].dmaInfoValid = GT_FALSE;

    }
    else
    {
        sysGenGlobalInfo.reInitializationDbPtr[devNum].deviceDidHwReset = GT_FALSE;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigDevDbHwResetGet
*
* DESCRIPTION:
*       private (internal) function get the state that the device did HW reset
*       or that the device finished the 'pp logic init' function.
*
*       this needed for the 'initialization after the DB release processing'
*       when the initialization for a device during 'phase 1' will be we will
*       know what 'old devNum' it used and so accessing the special DB for this
*       device.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number
* OUTPUTS:
*       didHwResetPtr - GT_TRUE - Did HW reset ,
*                       GT_FALSE -  finished pp logical init.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbHwResetGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *didHwResetPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(didHwResetPtr);

    if(! PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    else if(sysGenGlobalInfo.reInitializationDbPtr[devNum].busBaseAddr == BASE_ADDR_NOT_USED_CNS)
    {
        return GT_NOT_FOUND;
    }

    *didHwResetPtr = sysGenGlobalInfo.reInitializationDbPtr[devNum].deviceDidHwReset;

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigDevDbNetIfInitSet
*
* DESCRIPTION:
*       Private (internal) function to set if network initialization is needed or
*       if configuration will be based on previously saved re-initialization parameters.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The device number
*       needNetIfInit   - GT_TRUE  - Need network interface init
*                         GT_FALSE - Network interface init using previously saved values.
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbNetIfInitSet
(
    IN GT_U8     devNum,
    IN GT_BOOL   needNetIfInit
)
{
    if(! PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    else if(sysGenGlobalInfo.reInitializationDbPtr[devNum].busBaseAddr == BASE_ADDR_NOT_USED_CNS)
    {
        return GT_NOT_FOUND;
    }

    sysGenGlobalInfo.reInitializationDbPtr[devNum].deviceNeedNetIfInit = needNetIfInit;

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigDevDbNetIfInitGet
*
* DESCRIPTION:
*       Private (internal) function to get whether network initialization is needed or
*       whether configuration will be based on previously saved re-initialization parameters.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number
* OUTPUTS:
*       needNetIfInitPtr    - (pointer to):
*                             GT_TRUE  - Need network interface init
*                             GT_FALSE - Network interface init using previously saved values.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device Number
*       GT_NOT_FOUND             - device not found in the DB
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbNetIfInitGet
(
    IN  GT_U8     devNum,
    OUT GT_BOOL   *needNetIfInitPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(needNetIfInitPtr);

    if(! PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    else if(sysGenGlobalInfo.reInitializationDbPtr[devNum].busBaseAddr == BASE_ADDR_NOT_USED_CNS)
    {
        return GT_NOT_FOUND;
    }

    *needNetIfInitPtr = sysGenGlobalInfo.reInitializationDbPtr[devNum].deviceNeedNetIfInit;

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigDevDbRenumber
*
* DESCRIPTION:
*       the function set parameters for the driver to renumber it's DB.
*       the function "renumber" the current device number of the device to a
*       new device number.
*       NOTE:
*       this function MUST be called under 'Interrupts are locked'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       oldDevNum  - The PP's "old" device number .
*       newDevNum  - The PP's "new" device number swap the DB to.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL     - on error
*       GT_ALREADY_EXIST - the new device number is already used
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevDbRenumber
(
    IN GT_U8    oldDevNum,
    IN GT_U8    newDevNum
)
{
    /* swap the cpss DB */
    if(sysGenGlobalInfo.reInitializationDbPtr[newDevNum].busBaseAddr != BASE_ADDR_NOT_USED_CNS)
    {
        return GT_ALREADY_EXIST;
    }

    cpssOsMemCpy(&sysGenGlobalInfo.reInitializationDbPtr[newDevNum],
                 &sysGenGlobalInfo.reInitializationDbPtr[oldDevNum],
                 sizeof(sysGenGlobalInfo.reInitializationDbPtr[0]));

    /* set old device with init values */
    sysGenGlobalInfo.reInitializationDbPtr[oldDevNum].busBaseAddr = BASE_ADDR_NOT_USED_CNS;
    sysGenGlobalInfo.reInitializationDbPtr[oldDevNum].deviceDidHwReset = GT_TRUE;
    sysGenGlobalInfo.reInitializationDbPtr[oldDevNum].deviceNeedNetIfInit = GT_TRUE;
    sysGenGlobalInfo.reInitializationDbPtr[oldDevNum].dmaInfoValid = GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* prvCpssPrePhase1PpInit
*
* DESCRIPTION:
*       private (internal) function to make phase1 pre-init
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       prePhase1InitMode  - pre phase 1 init mode.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPrePhase1PpInit
(
    IN    PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_ENT     prePhase1InitMode
)
{
    prvCpssPrePhase1PpInitMode = prePhase1InitMode;
    return GT_OK;
}

/*******************************************************************************
* prvCpssPrePhase1PpInitModeGet
*
* DESCRIPTION:
*       private (internal) function retrieving  pre phase1 init mode
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
* OUTPUTS:
*       prePhase1InitModePtr  - pointer to pre phase 1 init mode .
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPrePhase1PpInitModeGet
(
    OUT PRV_CPSS_PP_PRE_PHASE1_INIT_MODE_ENT     *prePhase1InitModePtr
)
{
    *prePhase1InitModePtr = prvCpssPrePhase1PpInitMode;
    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigPortGroupFirstActiveGet
*
* DESCRIPTION:
*       Get the first active port group from the given bitmap.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number .
*       portGroupsBmp    - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       portGroupIdPtr  - pointer to port Group Id.
*                         for non multi-port groups device
*                         gets the CPSS_PORT_GROUP_UNAWARE_MODE_CNS value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigPortGroupFirstActiveGet
(
    IN  GT_U8                  devNum,
    IN  GT_PORT_GROUPS_BMP     portGroupsBmp,
    OUT GT_U32                 *portGroupIdPtr
)
{
    GT_U32 bmp;       /* port group bitmap             */
    GT_U32 bmp4;      /* port group bitmap 4 LSBs      */
    GT_U32 i;         /* loop index                    */

    /* the "first non-zero bit table" for values 0-15       */
    /* the value for 0 is not relevant                      */
    /* 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 */
    /* 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1,  0,  2,  0,  1,  0  */
    /* packed in one 32-bit value                           */
    /* bits n,n+1 contain the "first non-zero bit" for n    */
    #define PRV_FIRST_NON_ZERO_BIT_CNS 0x12131210

    if (0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }
    CPSS_NULL_PTR_CHECK_MAC(portGroupIdPtr);

    if (! PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
         *portGroupIdPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
         return GT_OK;
    }

    /* merge the device port group bitmap with the given bitmap */
    bmp = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp;
    if (portGroupsBmp != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        bmp &= portGroupsBmp;
    }

    /* search the first non-zero bit in bitmap */
    /* loop on 8 4-bit in 32-bit bitmap        */
    for (i = 0; (i < 8); i++, bmp >>= 4)
    {
        bmp4 = bmp & 0x0F;

        if (bmp4 == 0)
        {
            /* non-zero bit not found */
            /* search in next 4 bits  */
            continue;
        }

        /* non-zero bit found                                            */
        /* the expression below is the fast version of                   */
        /* (i * 4) + ((PRV_FIRST_NON_ZERO_BIT_CNS >> (bmp4 * 2)) & 0x03) */
        *portGroupIdPtr =
            (i << 2) + ((PRV_FIRST_NON_ZERO_BIT_CNS >> (bmp4 << 1)) & 0x03);
        return GT_OK;
    }

    /* occurs only if bmp == 0 */
    return GT_BAD_PARAM;

    /* this definition is local for this function */
    #undef PRV_FIRST_NON_ZERO_BIT_CNS
}

/*******************************************************************************
* prvCpssPpConfigDevInfoGet
*
* DESCRIPTION:
*    Private (internal) function returns device static information
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*
* OUTPUTS:
*       devInfoPtr   - (pointer to) device information
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigDevInfoGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_GEN_CFG_DEV_INFO_STC   *devInfoPtr
)
{
    PRV_CPSS_GEN_PP_CONFIG_STC *ppConfigPtr; /* pointer to PP configuration DB */

    /* check devNum */
    if(! PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }

    ppConfigPtr = PRV_CPSS_PP_MAC(devNum);

    devInfoPtr->devType  = ppConfigPtr->devType;
    devInfoPtr->revision = ppConfigPtr->revision;
    devInfoPtr->devFamily = ppConfigPtr->devFamily;
    devInfoPtr->maxPortNum = (GT_U8)(ppConfigPtr->numOfPorts - 1);
    devInfoPtr->numOfVirtPorts = ppConfigPtr->numOfVirtPorts;
    devInfoPtr->existingPorts =ppConfigPtr->existingPorts;

    return GT_OK;
}

/*******************************************************************************
* prvCpssPpConfigPortToPhymacObjectBind
*
* DESCRIPTION:
*         The function binds port phymac pointer
*         to 1540M PHYMAC object
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*    devNum         - device number
*    portNum        - port number
*    macDrvObjPtr   - port phymac object pointer
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK       - success
*       GT_FAIL     - error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssPpConfigPortToPhymacObjectBind
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN CPSS_MACDRV_OBJ_STC * const macDrvObjPtr

)
{
    /* Get PHY MAC object ptr */

    PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portMacObjPtr = macDrvObjPtr;

    return GT_OK;
}




