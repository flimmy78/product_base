/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpConGenInit.c
*
* DESCRIPTION:
*       Low level driver initialization of PPs, and declarations of global
*       variables
*
* FILE REVISION NUMBER:
*       $Revision: 1.2 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpssDriver/pp/interrupts/cpssDrvInterrupts.h>

#include <cpss/generic/events/private/prvCpssGenEvReq.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>

#ifdef  INCL_EXDXMX_DRIVER
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#endif

#define DEBUG_PRINT(x) cpssOsPrintf x

/*****************************************************************************
* Global
******************************************************************************/
/* array of pointers to the valid driver devices
   index to this array is devNum */
PRV_CPSS_DRIVER_PP_CONFIG_STC* prvCpssDrvPpConfig[PRV_CPSS_MAX_PP_DEVICES_CNS] =
    {NULL};

/* array including all driver management interface objects in Unit       */
PRV_CPSS_DRV_OBJ_STC* prvCpssDrvPpObjConfig[PRV_CPSS_MAX_PP_DEVICES_CNS] =
    {NULL};

/* driver global data that should be saving in HSU*/
PRV_CPSS_DRV_GLOBAL_DATA_STC drvGlobalInfo = {0};


/* DB to store device type id
   Note: The DB used for debug purpose only
*/
GT_U16   prvCpssDrvDebugDeviceId[PRV_CPSS_MAX_PP_DEVICES_CNS];

/*****************************************************************************
* Externals
******************************************************************************/

/*******************************************************************************
* drvEventMaskDevice
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* INPUTS:
*       devNum - PP's device number .
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                     queue number / other ) the event was received upon.
*                   may use value PRV_CPSS_DRV_EV_REQ_UNI_EV_EXTRA_DATA_ANY_CNS
*                   to indicate 'ALL interrupts' that relate to this unified
*                   event
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event or the evExtData within the unified
*                      event are not found in this device interrupts
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       This function called when OS interrupts and the ExtDrv interrupts are
*       locked !
*
*******************************************************************************/
static GT_STATUS drvEventMaskDevice
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_U32                   evExtData,
    IN CPSS_EVENT_MASK_SET_ENT  operation
)
{
    GT_STATUS   rc;
    GT_U32  portGroupId;/* port group Id */

    /* check that the device exists and properly initialized */
    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        rc = prvCpssDrvEvReqUniEvMaskSet(
                        prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intNodesPool,
                        prvCpssDrvPpConfig[devNum]->intCtrl.numOfIntBits,
                            (GT_U32)uniEvent, evExtData, operation);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* drvEventGppIsrConnect
*
* DESCRIPTION:
*       This function connects an Isr for a given Gpp interrupt.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR) with GPP
*
* INPUTS:
*       devNum      - The Pp device number at which the Gpp device is connected.
*       gppId       - The Gpp Id to be connected.
*       isrFuncPtr  - A pointer to the function to be called on Gpp interrupt
*                     reception.
*       cookie      - A cookie to be passed to the isrFuncPtr when its called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       To disconnect a Gpp Isr, call this function with a NULL parameter in
*       the isrFuncPtr param.
*
*******************************************************************************/
static GT_STATUS drvEventGppIsrConnect
(
    IN  GT_U8                   devNum,
    IN  CPSS_EVENT_GPP_ID_ENT   gppId,
    IN  CPSS_EVENT_ISR_FUNC     isrFuncPtr,
    IN  void                    *cookie
)
{
    /* check that the device exists and properly initialized */
    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    return prvCpssDrvDxExMxGppIsrConnect(devNum,gppId,isrFuncPtr,cookie);
}

/*******************************************************************************
* prvCpssDrvSysConfigPhase1
*
* DESCRIPTION:
*       This function sets cpssDriver system level system configuration
*       parameters before any of the device's phase1 initialization .
*
*
*  APPLICABLE DEVICES: ALL systems must call this function
*
* INPUTS:
*       none
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvSysConfigPhase1
(
    void
)
{
    GT_STATUS   rc = GT_NOT_INITIALIZED;
    GT_U32      i;

    cpssOsMemSet(prvCpssDrvPpConfig,0,sizeof(prvCpssDrvPpConfig));
    cpssOsMemSet(prvCpssDrvPpObjConfig,0,sizeof(prvCpssDrvPpObjConfig));
    drvGlobalInfo.performReadAfterWrite = GT_TRUE;
    drvGlobalInfo.prvCpssDrvAfterDbRelease = GT_FALSE;

    /* bind a callback function for the mask/unmask of unify-events for ALL PP
        devices */
    rc = prvCpssGenEventMaskDeviceBind(PRV_CPSS_DEVICE_TYPE_PP_E,&drvEventMaskDevice);
    if(rc != GT_OK)
    {
        return rc;
    }

    /*  initialize debug DB */
    for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        prvCpssDrvDebugDeviceId[i] = 0;
    }

    /* build the needed bus objects */
#ifdef  INCL_EXDXMX_DRIVER
    rc = prvCpssGenEventGppIsrConnectBind(&drvEventGppIsrConnect);
    if(rc != GT_OK)
    {
        return rc;
    }
#ifdef GT_PCI
    prvCpssDrvMngInfPciPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    cpssOsMemSet(prvCpssDrvMngInfPciPtr,0,sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    /* pointer to the object that control PCI */
    rc = prvCpssDrvHwPciDriverObjectInit();
    if(rc != GT_OK)
    {
        return rc;
    }
#endif /*GT_PCI*/

#ifdef GT_SMI
    prvCpssDrvMngInfSmiPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    cpssOsMemSet(prvCpssDrvMngInfSmiPtr,0,sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    /* pointer to the object that control SMI */
    rc = prvCpssDrvHwSmiDriverObjectInit();
    if(rc != GT_OK)
    {
        return rc;
    }
#endif /*GT_SMI*/

#ifdef GT_I2C
    prvCpssDrvMngInfTwsiPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    cpssOsMemSet(prvCpssDrvMngInfTwsiPtr,0,sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    /* pointer to the object that control TWSI */
    rc = prvCpssDrvHwTwsiDriverObjectInit();
    if(rc != GT_OK)
    {
        return rc;
    }
#endif /*GT_I2C*/

/*#ifdef GT_PEX

    prvCpssDrvMngInfPexPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    cpssOsMemSet(prvCpssDrvMngInfPexPtr,0,sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    * pointer to the object that control PEX *
    rc = prvCpssDrvHwPexDriverObjectInit();

    if(rc != GT_OK)
    {
        return rc;
    }
#endif*/

#if defined GT_PCI  /*|| defined GT_PEX*/

    /* HA StandBy driver allocation for PCI and PEX management interfaces */
    prvCpssDrvMngInfPciHaStandbyPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    cpssOsMemSet(prvCpssDrvMngInfPciHaStandbyPtr,0,sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    /* pointer to the object that control PCI -- for HA standby */
    rc = prvCpssDrvHwPciStandByDriverObjectInit();
    if(rc != GT_OK)
    {
        return rc;
    }

#endif /*GT_PEX*/
#endif

#ifdef  INCL_SOHO_DRIVER
    /* the cpss is not implemented for SOHO - yet */
    rc = GT_NOT_IMPLEMENTED;
#endif

    return rc;
}


/*******************************************************************************
* prvCpssDrvHwPpPhase1Init
*
* DESCRIPTION:
*       This function is called by cpssExMxHwPpPhase1Init() or other
*       cpss "phase 1" family functions, in order to enable PP Hw access,
*       the device number provided to this function may
*       be changed when calling prvCpssDrvPpHwPhase2Init().
*
*  APPLICABLE DEVICES: ALL devices
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       ppOutInfoPtr  - (pointer to)the info that driver return to caller.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_HW_ERROR - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPhase1Init
(
    IN  GT_U8                devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr,
    OUT PRV_CPSS_DRV_PP_PHASE_1_OUTPUT_INFO_STC *ppOutInfoPtr
)
{
    GT_STATUS rc;

    if(prvCpssDrvPpObjConfig[devNum])
    {
        return GT_ALREADY_EXIST;
    }

    if(prvCpssDrvMngInfPciPtr == NULL &&
       prvCpssDrvMngInfPciHaStandbyPtr == NULL &&
       prvCpssDrvMngInfSmiPtr == NULL &&
       prvCpssDrvMngInfTwsiPtr == NULL)
    {
        /* this is adding of first device after all devices were removed */
        rc = prvCpssDrvSysConfigPhase1();
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* create the device object for the functions for this device */
    prvCpssDrvPpObjConfig[devNum] = cpssOsMalloc(sizeof(PRV_CPSS_DRV_OBJ_STC));
    if(prvCpssDrvPpObjConfig[devNum] == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

#ifdef INCL_EXDXMX_DRIVER
    /* Dx, Ex, Mx driver init */
    rc = prvCpssDrvDxExMxInitObject(devNum);

    if(rc != GT_OK)
        return rc;
#endif /* INCL_EXDXMX_DRIVER */

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    /* Call appropriate implementation by virtual function */
    rc = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpCfgPhase1Init(devNum,
                    ppInInfoPtr,ppOutInfoPtr);

    return rc;
}


/*******************************************************************************
* prvCpssDrvHwPpPhase2Init
*
* DESCRIPTION:
*       the function set parameters for the driver for the "init Phase 2".
*       the function "renumber" the current device number of the device to a
*       new device number.
*
*  APPLICABLE DEVICES: ALL devices
*
* INPUTS:
*       devNum     - The PP's current device number .
*       newDevNum  - The PP's "new" device number to register.
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
GT_STATUS prvCpssDrvHwPpPhase2Init
(
    IN GT_U8    devNum,
    IN GT_U8    newDevNum
)
{
    if(devNum == newDevNum)
    {
        /* nothing more to do */
        return GT_OK;
    }

    if(prvCpssDrvPpObjConfig[newDevNum] || prvCpssDrvPpConfig[newDevNum])
    {
        return GT_ALREADY_EXIST;
    }

    /* swap the function objects */
    prvCpssDrvPpObjConfig[newDevNum] = prvCpssDrvPpObjConfig[devNum];
    prvCpssDrvPpObjConfig[devNum] = NULL;

    /* swap the info DB */
    prvCpssDrvPpConfig[newDevNum] = prvCpssDrvPpConfig[devNum];
    prvCpssDrvPpConfig[devNum] = NULL;

    /* update the devNum */
    prvCpssDrvPpConfig[newDevNum]->devNum = newDevNum;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvHwPpRamBurstConfigSet
*
* DESCRIPTION:
*       Sets the Ram burst information for a given device.
*
* INPUTS:
*       devNum          - The Pp's device number.
*       ramBurstInfo    - A list of Ram burst information for this device.
*       burstInfoLen    - Number of valid entries in ramBurstInfo.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       Narrow SRAM burst is not supported under Tiger devices.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpRamBurstConfigSet
(
    IN  GT_U8               devNum,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfoPtr,
    IN  GT_U8               burstInfoLen
)
{
    GT_STATUS   rc;

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    /* Call appropriate implementation by virtual function */
    rc = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpSetRamBurstConfig(devNum,
                    ramBurstInfoPtr,burstInfoLen);
    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpInitInMasks

*
* DESCRIPTION:
*      Set the interrupt mask for a given device.
*
*  APPLICABLE DEVICES: ExMx devices
*
* INPUTS:
*       devNum      - The Pp's device number.
*       b4StartInit - Is the call to this function is done before / after start
*                     Init.
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*      None.
*
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpInitInMasks
(
    IN  GT_U8   devNum,
    IN  GT_BOOL b4StartInit
)
{
    GT_STATUS   rc;
    GT_U32  portGroupId;/* port group Id */

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /* Call appropriate implementation by virtual function */
        rc = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpInitInMasks(devNum,portGroupId,
                        b4StartInit,GT_FALSE);
		/* add GT_FALSE in the last, for we redefine the FUNC, zhandi 20110325*/
		
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvHwPpDevRemove
*
* DESCRIPTION:
*       Synchronize the driver/Config library after a hot removal operation, of
*       the given device.
*
* INPUTS:
*       devNum   - device Number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpDevRemove
(
    IN GT_U8    devNum
)
{
    GT_STATUS           rc;
    GT_U32  ii;
    GT_U32  portGroupId;/* port group Id */

    if(prvCpssDrvPpConfig[devNum] == NULL)
    {
        /* Unknown device  */
        return GT_BAD_PARAM;
    }

    /* Remove the device from the interrupts queues */
    rc = prvCpssDrvInterruptDeviceRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        prvCpssDrvInterruptScanFree(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intScanRoot);

        if(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem)
        {
            cpssOsMutexDelete(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
        }

        FREE_PTR_MAC(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo);

        FREE_PTR_MAC(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intNodesPool);
        FREE_PTR_MAC(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intMaskShadow);
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    FREE_PTR_MAC(prvCpssDrvPpConfig[devNum]->intCtrl.accessB4StartInit);

    /* clear the memory */
    FREE_PTR_MAC(prvCpssDrvPpConfig[devNum]);
    FREE_PTR_MAC(prvCpssDrvPpObjConfig[devNum]);

    /* check if there are still existing devices */
    for(ii = 0 ; ii < PRV_CPSS_MAX_PP_DEVICES_CNS ; ii++)
    {
        if(prvCpssDrvPpConfig[ii])
        {
            break;
        }
    }

    if(ii == PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        /* no more existing devices */


        /* release the management interface bus pointers */
        FREE_PTR_MAC(prvCpssDrvMngInfPciPtr);
        FREE_PTR_MAC(prvCpssDrvMngInfPciHaStandbyPtr);
        FREE_PTR_MAC(prvCpssDrvMngInfSmiPtr);
        FREE_PTR_MAC(prvCpssDrvMngInfTwsiPtr);
/*        FREE_PTR_MAC(prvCpssDrvMngInfPexPtr);*/

        /* release the DB of interrupt ISR (lines) manager */
        rc = prvCpssDrvComIntSvcRtnDbRelease();
        if(rc != GT_OK)
        {
            return rc;
        }

        /* set that we are done releaseing the DB */
        drvGlobalInfo.prvCpssDrvAfterDbRelease = GT_TRUE;
    }



    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvHwPpHaModeSet
*
* DESCRIPTION:
*       function to set CPU High Availability mode of operation.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       devNum       - the device number.
*       mode - active or standby
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or mode
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpHaModeSet
(
    IN  GT_U8   devNum,
    IN  CPSS_SYS_HA_MODE_ENT mode
)
{
    GT_STATUS   rc;

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    /* Call appropriate implementation by virtual function */
    rc = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpHaModeSet(devNum,mode);

    return rc;
}


/*******************************************************************************
* cpssDrvInterruptsTreeGet
*
* DESCRIPTION:
*       function return :
*       1. the root to the interrupts tree info of the specific device
*       2. the interrupt registers that can't be accesses before 'Start Init'
*
* APPLICABLE DEVICES: All devices --> Packet Processors (not Fa/Xbar)
*
* INPUTS:
*       devNum - the device number
*
* OUTPUTS:
*       numOfElementsPtr - (pointer to) number of elements in the tree.
*       treeRootPtrPtr - (pointer to) pointer to root of the interrupts tree info.
*       numOfInterruptRegistersNotAccessibleBeforeStartInitPtr - (pointer to)
*                           number of interrupt registers that can't be accessed
*                           before 'Start init'
*       notAccessibleBeforeStartInitPtrPtr (pointer to)pointer to the interrupt
*                           registers that can't be accessed before 'Start init'
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - the driver was not initialized for the device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   cpssDrvInterruptsTreeGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *numOfElementsPtr,
    OUT const CPSS_INTERRUPT_SCAN_STC        **treeRootPtrPtr,
    OUT GT_U32  *numOfInterruptRegistersNotAccessibleBeforeStartInitPtr,
    OUT GT_U32  **notAccessibleBeforeStartInitPtrPtr
)
{
    GT_STATUS   rc;
    GT_U32  portGroupId = CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS;/*tempo internal parameter and not getting it from application*/

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return GT_BAD_PARAM;
    }

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum,portGroupId);

    CPSS_NULL_PTR_CHECK_MAC(numOfElementsPtr);
    CPSS_NULL_PTR_CHECK_MAC(treeRootPtrPtr);
    CPSS_NULL_PTR_CHECK_MAC(numOfInterruptRegistersNotAccessibleBeforeStartInitPtr);
    CPSS_NULL_PTR_CHECK_MAC(notAccessibleBeforeStartInitPtrPtr);

    /* Call appropriate implementation by virtual function */
    rc = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpInterruptsTreeGet(devNum,portGroupId,numOfElementsPtr,treeRootPtrPtr,
        numOfInterruptRegistersNotAccessibleBeforeStartInitPtr,notAccessibleBeforeStartInitPtrPtr);

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpRenumber
*
* DESCRIPTION:
*       the function set parameters for the driver to renumber it's DB.
*       the function "renumber" the current device number of the device to a
*       new device number.
*       NOTE:
*       this function MUST be called under 'Interrupts are locked'
*
*  APPLICABLE DEVICES: ALL devices
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
GT_STATUS prvCpssDrvHwPpRenumber
(
    IN GT_U8    oldDevNum,
    IN GT_U8    newDevNum
)
{
    if(oldDevNum == newDevNum)
    {
        /* nothing more to do */
        return GT_OK;
    }

    if(prvCpssDrvPpObjConfig[newDevNum] || prvCpssDrvPpConfig[newDevNum])
    {
        return GT_ALREADY_EXIST;
    }

    /* swap the function objects */
    prvCpssDrvPpObjConfig[newDevNum] = prvCpssDrvPpObjConfig[oldDevNum];
    prvCpssDrvPpObjConfig[oldDevNum] = NULL;

    /* swap the info DB */
    prvCpssDrvPpConfig[newDevNum] = prvCpssDrvPpConfig[oldDevNum];
    prvCpssDrvPpConfig[oldDevNum] = NULL;

    /* update the devNum */
    prvCpssDrvPpConfig[newDevNum]->devNum = newDevNum;

    /*call the interrupts to renumber */
    return prvCpssDrvInterruptsRenumber(oldDevNum,newDevNum);
}


/*******************************************************************************
* prvCpssDrvDebugDeviceIdSet
*
* DESCRIPTION:
*       This is debug function.
*       The function overrides device ID by given value.
*       The function should be called before cpssDxChHwPpPhase1Init().
*
*  APPLICABLE DEVICES: All devices
*
* INPUTS:
*       devNum  - device number .
*       devType - device type to store.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL     - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvDebugDeviceIdSet
(
    IN GT_U8                    devNum,
    IN CPSS_PP_DEVICE_TYPE      devType
)
{
    if (devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_BAD_PARAM;
    }

    prvCpssDrvDebugDeviceId[devNum] = (GT_U16)(devType >> 16);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvAddrCheckWaBind
*
* DESCRIPTION:
*       Binds errata db with address check callback
*
* APPLICABLE DEVICES: All devices
*
* INPUTS:
*       devNum           - the device number
*       addrCheckFuncPtr - pointer to callback function that checks addresses.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_INITIALIZED       - the driver was not initialized for the device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDrvAddrCheckWaBind
(
    IN  GT_U8                                   devNum,
    IN  PRV_CPSS_DRV_ERRATA_ADDR_CHECK_FUNC     addrCheckFuncPtr
)
{
    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr = addrCheckFuncPtr;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvPpDump
*
* DESCRIPTION:
*       Dump function , to print the info on a specific PP -- CPSS driver.
*
*
*  APPLICABLE DEVICES: ALL devices
*
* INPUTS:
*       devNum - device Number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - on bad device number
*
* COMMENTS:
*       function also called from the "cpss pp dump" : cpssPpDump(devNum)
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpDump
(
    IN GT_U8   devNum
)
{
    PRV_CPSS_DRIVER_PP_CONFIG_STC   *drvPpInfoPtr;
    GT_U32  portGroupId;/* port group Id */

    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS ||
       prvCpssDrvPpConfig[devNum] == NULL)
    {
        DEBUG_PRINT(("bad device number [%d] \n",devNum));
        return GT_BAD_PARAM;
    }

    drvPpInfoPtr = prvCpssDrvPpConfig[devNum];

    portGroupId = drvPpInfoPtr->portGroupsInfo.firstActivePortGroup;

    DEBUG_PRINT(("\n"));
    DEBUG_PRINT(("start CPSS DRIVER Info about device number [%d]: \n",devNum));

    DEBUG_PRINT((" devNum = [%d] \n",drvPpInfoPtr->devNum));

    DEBUG_PRINT((" devType = [0x%8.8x] \n",drvPpInfoPtr->devType));
    DEBUG_PRINT((" devFamily = [%s] \n",
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTC_E    ? " CPSS_PP_FAMILY_TWISTC_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTD_E    ? " CPSS_PP_FAMILY_TWISTD_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_SAMBA_E     ? " CPSS_PP_FAMILY_SAMBA_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_SALSA_E     ? " CPSS_PP_FAMILY_SALSA_E ":

        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH_E   ? " CPSS_PP_FAMILY_CHEETAH_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH2_E  ? " CPSS_PP_FAMILY_CHEETAH2_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH3_E  ? " CPSS_PP_FAMILY_CHEETAH3_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E  ? " CPSS_PP_FAMILY_DXCH_XCAT_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION_E  ? " CPSS_PP_FAMILY_DXCH_LION_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_TIGER_E     ? " CPSS_PP_FAMILY_TIGER_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_PUMA_E      ? " CPSS_PP_FAMILY_PUMA_E ":
        " unknown "
    ));
    DEBUG_PRINT((" numOfPorts = [%d] \n",drvPpInfoPtr->numOfPorts));

    if(drvPpInfoPtr->portGroupsInfo.isMultiPortGroupDevice == GT_TRUE)
    {
        DEBUG_PRINT(("numOfPortGroups  = [%d] \n",
                     drvPpInfoPtr->portGroupsInfo.numOfPortGroups));
        DEBUG_PRINT(("activePortGroupsBmp   = [%d] \n",
                     drvPpInfoPtr->portGroupsInfo.activePortGroupsBmp));
        DEBUG_PRINT(("firstActivePortGroup  = [%d] \n",
                     drvPpInfoPtr->portGroupsInfo.firstActivePortGroup));
        DEBUG_PRINT(("lastActivePortGroup   = [%d] \n",
                     drvPpInfoPtr->portGroupsInfo.lastActivePortGroup));
    }
    else
    {
        if(drvPpInfoPtr->portGroupsInfo.numOfPortGroups != 1 ||
           drvPpInfoPtr->portGroupsInfo.activePortGroupsBmp != BIT_0 ||
           drvPpInfoPtr->portGroupsInfo.firstActivePortGroup != 0 ||
           drvPpInfoPtr->portGroupsInfo.lastActivePortGroup != 0)
        {
            DEBUG_PRINT(("Error : non 'multi port grops' device with bad configuration : \n"));
            DEBUG_PRINT(("numOfPortGroups --  = [%d] \n",
                         drvPpInfoPtr->portGroupsInfo.numOfPortGroups));
            DEBUG_PRINT(("activePortGroupsBmp --   = [%d] \n",
                         drvPpInfoPtr->portGroupsInfo.activePortGroupsBmp));
            DEBUG_PRINT(("firstActivePortGroup --  = [%d] \n",
                         drvPpInfoPtr->portGroupsInfo.firstActivePortGroup));
            DEBUG_PRINT(("lastActivePortGroup --   = [%d] \n",
                         drvPpInfoPtr->portGroupsInfo.lastActivePortGroup));
        }
    }

    DEBUG_PRINT((" doByteSwap = [%s] \n",
            drvPpInfoPtr->hwCtrl[portGroupId].doByteSwap == GT_TRUE   ? " GT_TRUE " :
            drvPpInfoPtr->hwCtrl[portGroupId].doByteSwap == GT_FALSE  ? " GT_FALSE " :
            " unknown "
    ));

    DEBUG_PRINT((" drvChannel = [%s] \n",
        PRV_CPSS_DRV_HW_IF_PEX_COMPATIBLE_MAC(devNum) ? " CPSS_CHANNEL_PEX_E " :
        PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum)  ? " CPSS_CHANNEL_PCI_E " :
        PRV_CPSS_DRV_HW_IF_SMI_COMPATIBLE_MAC(devNum)  ? " CPSS_CHANNEL_SMI_E " :
        PRV_CPSS_DRV_HW_IF_TWSI_COMPATIBLE_MAC(devNum) ? " CPSS_CHANNEL_TWSI_E " :
        " unknown "
    ));

    DEBUG_PRINT(("finished CPSS DRIVER Info about device number [%d]: \n",devNum));
    DEBUG_PRINT(("\n"));

    return GT_OK;
}

