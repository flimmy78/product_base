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
* gtAppDemoDxSysConfig.c
*
* DESCRIPTION:
*       System configuration and initialization control for Dx devices.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*       $Revision: 1.5 $
*
*******************************************************************************/
#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
/* Cascading support */
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <cpss/dxCh/dxCh3/policer/cpssDxCh3Policer.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>

/*******************************************************************************
 * External variables
 ******************************************************************************/

/* appDemoPpConfigList - Holds the Pp's configuration parameters */
extern APP_DEMO_PP_CONFIG appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];

static CPSS_DXCH_PP_PORT_GROUP_INIT_INFO_STC portGroupsPhase1InfoArr[APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS];

#ifndef __AX_PLATFORM__
extern unsigned int auteware_product_id;
#endif
/*******************************************************************************
 * Local usage variables
 ******************************************************************************/

static FUNCP_CPSS_MODULE_INIT cpssInitList[20]; /* Defined at the end of the file. */


static GT_STATUS appDemoConvert2DxChPhase1Info
(
    IN  GT_U8                              dev,
    IN  CPSS_PP_PHASE1_INIT_PARAMS         *appCpssPpPhase1ParamsPtr,
    OUT CPSS_DXCH_PP_PHASE1_INIT_INFO_STC  *cpssPpPhase1InfoPtr
);

static GT_STATUS appDemoConvert2DxChPhase2Info
(
    IN  CPSS_PP_PHASE2_INIT_PARAMS          *appCpssPpPhase2ParamsPtr,
    OUT CPSS_DXCH_PP_PHASE2_INIT_INFO_STC   *cpssPpPhase2InfoPtr
);

static GT_VOID appDemoCoreToCpssDxChPpInfoConvert
(
    IN  CPSS_PP_CONFIG_INIT_STC       *sysConfPtr,
    OUT CPSS_DXCH_PP_CONFIG_INIT_STC  *ppLogicaslInfoPtr
);

/*static GT_VOID appDemoCoreToCpssPpSalInfoConvert      - For Salsa - not implemented for now..
(
    IN  CPSS_PP_CONFIG_INIT_STC         *sysConfPtr,
    OUT CPSS_EXMXTG_PP_CONFIG_INIT_STC  *tgPpLogicaslInfoPtr
);*/

static GT_STATUS prvCscdPortsInit
(
    IN  GT_U8   dev
);

static GT_STATUS prv10GPortsConfig
(
    IN  GT_U8   dev
);


/* User configured bitmap for SERDES power down */
GT_U32 appDemoDxChPowerDownBmp = 0;

/* offset used during HW device ID calculation formula */
extern GT_U8 appDemoHwDevNumOffset;

/* packet size */
#define APP_DEMO_PACKET_SIZE_CNS 68


/*******************************************************************************
 * Private functions implementation
 ******************************************************************************/
/*******************************************************************************
* appDemoDxChDeviceIdAndRevisionPrint
*
* DESCRIPTION:
*       The function prints out ID and revision of device
*
* APPLICABLE DEVICES: All.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS appDemoDxChDeviceIdAndRevisionPrint
(
    GT_U8   devNum
)
{
    CPSS_DXCH_CFG_DEV_INFO_STC devInfo; /* device info */
    char *lionRevName[]={"A0", "B0", "B0A", "B1 or above"}; /* revision names */
    char *revNamePtr = NULL;            /* name of revision */
    int  revision;                      /* revision */
    GT_STATUS   rc;                     /* return code */

    /* get device info */
    rc = cpssDxChCfgDevInfoGet(devNum,&devInfo);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgDevInfoGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    revision = devInfo.genDevInfo.revision;
    switch (devInfo.genDevInfo.devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_LION_E:
            switch (revision)
            {
                case 0:
                case 1:
                    /* A0 revisions */
                    revNamePtr = lionRevName[0];
                    break;
                case 2:
                    /* check shaper API disable flag to distinguish between
                       B0 and B0A.*/
                    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum, PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E))
                    {
                        /* B0 revisions */
                        revNamePtr = lionRevName[1];
                    }
                    else
                    {
                        /* B0A revisions */
                        revNamePtr = lionRevName[2];
                    }
                    break;
                default:
                    revNamePtr = lionRevName[3];
                    break;
            }
            break;
        default: break;
    }

    if (revNamePtr)
    {
        osPrintf("Device[%d] ID 0x%X revision %s\n",devNum, devInfo.genDevInfo.devType, revNamePtr);
    }
    else
    {
        osPrintf("Device[%d] ID 0x%X revision %d\n",devNum, devInfo.genDevInfo.devType, revision);
    }
    return GT_OK;
}

/*******************************************************************************
 * Public API implementation
 ******************************************************************************/

/*******************************************************************************
* appDemoDxChPowerDownBmpSet
*
* DESCRIPTION:
*       Sets AppDemo power down SERDES bitmask.
*
* INPUTS:
*       powerDownBmp  - the value to be written.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxChPowerDownBmpSet
(
    IN GT_U32   powerDownBmp
)
{
    appDemoDxChPowerDownBmp = powerDownBmp;
    return GT_OK;
}
/*******************************************************************************
* appDemoDxChFdbInit
*
* DESCRIPTION:
*        flush the FDB of DXCH device after init.
*
* INPUTS:
*       dev  - The Pp device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS appDemoDxChFdbInit(
    IN GT_U8 dev
)
{
    GT_STATUS rc;            /* return value */
    GT_U8     port;          /* port iterator */
    /* GT_TRUE  - triggered action completed
       GT_FALSE - triggered action is not completed yet */
    GT_BOOL   actFinished;
    GT_U8     hwDevNum;      /* HW device number */
    GT_U32    fdbBmp;        /* bmp of devices to set */
    GT_U32    i;             /* iterator */
    GT_U32              value;
    CPSS_FDB_ACTION_MODE_ENT    actionMode;

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

	#if 0
    if(appDemoDbEntryGet("ctrlMacLearn", &value) == GT_OK)
    {
        /* --> use same string as in PSS : values are:
            typedef enum
            {
                GT_AUTOMATIC        = 0,
                GT_CONTROLED_MSSG   = 1,
                GT_CONTROLED_TAG    = 2
            } GT_LEARNING_MODE;
        */

        /* allow to force auto learning , because by default we set ' Controlled learning'*/
        if(value == 0 /*GT_AUTOMATIC*/)
        {
            appDemoSysConfig.forceAutoLearn = GT_TRUE;
        }
    }
	#else
	/* for "delete fdb vlan 1" on board 2X12G12S, default set auto leran MAC addr */
	/* 2011-09-20 for distributed system */
	/*for CPU control learning  2012-05-07 zhangcl@autelan.com*/
	if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
		appDemoSysConfig.forceAutoLearn = GT_FALSE;
	else
		appDemoSysConfig.forceAutoLearn = GT_TRUE;  /*default vlan fdb delete problem*/
	#endif

    /*********************************************************/
    /* Set FDB learning mode                                 */
    /*********************************************************/
    rc = cpssDxChBrgFdbMacVlanLookupModeSet(dev, CPSS_IVL_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbMacVlanLookupModeSet", rc);
    if (rc != GT_OK)
       return rc;

    /* restore automatic action mode*/
    rc = cpssDxChBrgFdbMacTriggerModeSet(dev, CPSS_ACT_AUTO_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbMacTriggerModeSet", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* default for the 'appDemo' */
    actionMode = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;

    switch(PRV_CPSS_DXCH_PP_MAC(dev)->portGroupsExtraInfo.fdbMode)
    {
        case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E:
        case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
            actionMode = CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;
            break;
        case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_NON_MULTI_PORT_GROUP_DEVICE_E:
            if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
                actionMode = CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;
            }
            break;
        default:
            break;
    }

    /* restore age with/without removal */
    rc = cpssDxChBrgFdbTrigActionStart(dev, actionMode);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbTrigActionStart", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* wait for action to be over */
    actFinished = GT_FALSE;
    while(actFinished == GT_FALSE)
    {
        rc = cpssDxChBrgFdbTrigActionStatusGet(dev,&actFinished);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbTrigActionStatusGet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        osTimerWkAfter(1);
    }

    /*if LION enable the sending to CPU of AA/TA  else disable*/
	if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    	rc = cpssDxChBrgFdbAAandTAToCpuSet(dev, GT_TRUE);
	else
		rc = cpssDxChBrgFdbAAandTAToCpuSet(dev, GT_FALSE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAAandTAToCpuSet", rc);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* Setting aging timeout to default timeout (300 seconds) is needed because
       DX106 core clock is 144MHz and not 200MHz as in other PPs. */
    rc = cpssDxChBrgFdbAgingTimeoutSet(dev, 300);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAgingTimeoutSet", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfPorts; port++)
    {
        if (PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_NOT_EXISTS_E)
            continue;

        if(appDemoSysConfig.forceAutoLearn == GT_TRUE)
        {
            rc = cpssDxChBrgFdbPortLearnStatusSet(dev, port, GT_TRUE, CPSS_LOCK_FRWRD_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbPortLearnStatusSet - auto learning", rc);
        }
        else
        {
            rc = cpssDxChBrgFdbPortLearnStatusSet(dev, port, GT_FALSE, CPSS_LOCK_FRWRD_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbPortLearnStatusSet - controlled learning", rc);
        }
        if (GT_OK != rc)
        {
            return rc;
        }

        /* note the cascade init will make sure to disable the sending from
           the cascade ports */
        rc = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbNaToCpuPerPortSet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    fdbBmp = 0;
    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            rc = cpssDxChCfgHwDevNumGet(appDemoPpConfigList[i].devNum, &hwDevNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet", rc);
            if (GT_OK != rc)
            {
                return rc;
            }
            fdbBmp |= (1 << hwDevNum );
        }
    }

    if(fdbBmp != 0)
    {
        rc = cpssDxChBrgFdbDeviceTableSet(dev, fdbBmp);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbDeviceTableSet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    /* Deletes all addresses from FDB table */
    rc = appDemoDxChFdbFlush(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChFdbFlush", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}
/*******************************************************************************
* appDemoDxHwPpPhase1Init
*
* DESCRIPTION:
*       Implements HW phase 1 initialization.
*
* INPUTS:
*      dev                     -   temp device id, could be changed later
*      coreSysCfgPh1ParamsPtr  -   phase 1 core specific parameters
*      corePpPhase1ParamsPtr   -   PP phase 1 init parameters
*
* OUTPUTS:
*      ppDeviceTypePtr         -   output correct device type
*
* RETURNS:
*       GT_OK       - on success,
*       GT_BAD_PTR  -   on bad pointers
*       GT_FAIL     - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxHwPpPhase1Init
(
    IN  GT_U8                        dev,
    IN  CPSS_PP_PHASE1_INIT_PARAMS  *appCpssPpPhase1ParamsPtr,
    OUT CPSS_PP_DEVICE_TYPE         *ppDevTypePtr
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_DXCH_PP_PHASE1_INIT_INFO_STC   cpssPpPhase1Params;     /* CPSS phase 1 PP params */
    GT_U32  value;
    CPSS_DXCH_IMPLEMENT_WA_ENT   waFromCpss[CPSS_DXCH_IMPLEMENT_WA_LAST_E];
    GT_U32  waIndex=0;
    GT_U32  ii;
    GT_BOOL trunkToCpuWaEnabled = GT_FALSE;

    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxHwPpPhase1Init", rc);
    printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);/*xcat debug*/
    int *p=(int *)osCacheDmaMalloc(4);
    printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);/*xcat debug*/
    CPSS_ENABLER_DBG_TRACE_RC_MAC("osCacheDmaMalloc", p);
    p = malloc(4);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("malloc", p);
    p = cpssOsMalloc(4);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssOsMalloc", p);
    p = osMalloc(4);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("osMalloc", p);
    #if 0
        return GT_NOT_IMPLEMENTED;/*xcat debug*/
    #endif

    if ((NULL == appCpssPpPhase1ParamsPtr) ||
        (NULL == ppDevTypePtr))
    {
        return GT_BAD_PTR;
    }

    /* fill CPSS Phase1 init parameters */
    rc = appDemoConvert2DxChPhase1Info(dev,
                                       appCpssPpPhase1ParamsPtr,
                                       &cpssPpPhase1Params);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoConvert2DxChPhase1Info", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    /* use same string as for PSS */
    if(appDemoDbEntryGet("GT_CORE_SYS_WA_OPTION_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E", &value) == GT_OK)
    {
        if(value)
        {
            trunkToCpuWaEnabled = GT_TRUE;

            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E;

        }
    }

    if(appDemoDbEntryGet("GT_CORE_SYS_WA_OPTION_SDMA_PKTS_FROM_CPU_STACK_WA_E", &value) == GT_OK)
    {
        if(value)
        {
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_E;

        }
    }

    if(appDemoDbEntryGet("GT_CORE_SYS_WA_OPTION_SDMA_PKTS_FROM_CPU_STACK_PADDING_WA_E", &value) == GT_OK)
    {
        if(value)
        {
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_PADDING_E;

        }
    }

    if(appDemoDbEntryGet("GT_CORE_SYS_WA_RESTRICTED_ADDRESS_FILTERING_WA_E", &value) == GT_OK)
    {
        if(value)
        {
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E;

        }
    }

    

    rc = cpssDxChHwPpPhase1Init(&cpssPpPhase1Params, ppDevTypePtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpPhase1Init", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDxChDeviceIdAndRevisionPrint(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChDeviceIdAndRevisionPrint", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(waIndex)
    {
        rc = cpssDxChHwPpImplementWaInit(dev,waIndex,&waFromCpss[0],0);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpImplementWaInit", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        if (trunkToCpuWaEnabled == GT_TRUE)
        {
            for(ii = SYSTEM_DEV_NUM_MAC(0) ; ii < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount);ii++)
            {
                if(appDemoPpConfigList[ii].valid && appDemoPpConfigList[ii].devNum == dev)
                {
                    /* save to DB --> will be used by the galtis Wrappers */
                    appDemoPpConfigList[ii].wa.trunkToCpuPortNum = GT_TRUE;
                    break;
                }
            }
        }
    }

    return GT_OK;
} /* appDemoDxHwPpPhase1Init */

/*******************************************************************************
* appDemoDxHwPpStartInit
*
* DESCRIPTION:
*       Implements PP start initialization.
*
* INPUTS:
*      dev             -   device number
*      fReloadEeprom   -   reload Eeprom flag
*      initDataListPtr -   list of registers to init
*      initDataListLen -   length of registers' list
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
GT_STATUS appDemoDxHwPpStartInit
(
    IN  GT_U8                       dev,
    IN  GT_BOOL                     fReloadEeprom,
    IN  CPSS_REG_VALUE_INFO_STC     *initDataListPtr,
    IN  GT_U32                      initDataListLen
)
{
    GT_STATUS rc;

    rc = cpssDxChHwPpStartInit(dev,
                                 fReloadEeprom,
                                 initDataListPtr,
                                 initDataListLen);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpStartInit", rc);
    return rc;
} /* appDemoDxHwPpStartInit */

/*******************************************************************************
* appDemoDxHwPpPhase2Init
*
* DESCRIPTION:
*       Implements HW phase 2 initialization.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*      oldDev                  -   old device id, used for phase 1
*      coreSysCfgPh2ParamsPtr  -   phase 2 core specific parameters
*      corePpPhase2ParamsPtr   -   PP phase 2 init parameters
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  -   on bad pointer
*       GT_FAIL     - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxHwPpPhase2Init
(
    IN  GT_U8                       oldDev,
    IN  CPSS_PP_PHASE2_INIT_PARAMS  *appCpssPpPhase2ParamsPtr
)
{
    GT_STATUS                           rc = GT_OK;
    CPSS_DXCH_PP_PHASE2_INIT_INFO_STC   cpssPpPhase2Info;       /* Cpss phase 2 info */
    GT_U8                               hwDevNum;               /* HW device number */


    if (NULL == appCpssPpPhase2ParamsPtr)
    {
        return GT_BAD_PTR;
    }

    /* Fill CPSS phase2 init structure */
    rc = appDemoConvert2DxChPhase2Info(appCpssPpPhase2ParamsPtr,
                                       &cpssPpPhase2Info);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoConvert2DxChPhase2Info", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssDxChHwPpPhase2Init(oldDev, &cpssPpPhase2Info);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpPhase2Init", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

	osPrintfDbg("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);/*xcat debug*/	
    osPrintfDbg("******** Here we can change the device num of different SW !!!\n",oldDev);
    /* Now we need not the appDemoHwDevNumOffset, so jump the follow code!!  zhangdi */

#if 0
    /* In order to configure HW device ID different from SW device ID the following */
    /* logic is used: HW_device_ID = (SW_device_ID + appDemoHwDevNumOffset) modulo 32 */
    /* (this insures diffrent HW and SW device IDs since the HW device ID is 5 */
    /* bits length). */
    hwDevNum = (GT_U8)((cpssPpPhase2Info.newDevNum + appDemoHwDevNumOffset) & 0x1F);

    rc = cpssDxChCfgHwDevNumSet(cpssPpPhase2Info.newDevNum, hwDevNum);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumSet", rc);
#endif /* end */
    return rc;
} /* appDemoDxHwPpPhase2Init */

/*******************************************************************************
* appDemoDxPpLogicalInit
*
* DESCRIPTION:
*      Implements logical initialization for specific device.
*      Device number is a part of system configuration parameters.
*
* INPUTS:
*      dev               -   device number
*      logicalConfigParams  -   PP logical configuration parameters
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDxPpLogicalInit
(
    IN  GT_U8                        dev,
    IN  CPSS_PP_CONFIG_INIT_STC     *logicalConfigParams
)
{
    GT_STATUS                        rc = GT_OK;
    CPSS_DXCH_PP_CONFIG_INIT_STC     ppLogicaslInfo;     /* CPSS format - DxCh info  */


    switch (PRV_CPSS_PP_MAC(dev)->devFamily)
    {
         case CPSS_PP_FAMILY_SALSA_E:
             /* convert core format to specifc CPSS format -- Salsa */
             /*appDemoCoreToCpssDxSalPpInfoConvert(logicalConfigParams,
                                              &tgPpLogicaslInfo);*/

             /* call CPSS to do Pp logical init */
             /*rc = cpssDxSalCfgPpLogicalInit(dev, &tgPpLogicaslInfo);*/
             rc = GT_FAIL;
             break;
         default:
             /* convert core format to specific CPSS format -- Dx */
             appDemoCoreToCpssDxChPpInfoConvert(logicalConfigParams,
                                            &ppLogicaslInfo);

             /* call CPSS to do Pp logical init */
             rc = cpssDxChCfgPpLogicalInit(dev, &ppLogicaslInfo);
             CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgPpLogicalInit", rc);
             break;
     }
     return rc;
} /* appDemoDxPpLogicalInit */

/*******************************************************************************
* appDemoDxLibrariesInit
*
* DESCRIPTION:
*      Initialize all CPSS libraries for specific device.
*      It's entry point where all required CPSS libraries are initialized.
*
* INPUTS:
*      dev                 -   device number
*      libInitParamsPtr    -   initialization parameters
*      sysConfigParamPtr   -   system configuration parameters
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
GT_STATUS appDemoDxLibrariesInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParams,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParams
)
{
    GT_STATUS   rc = GT_OK;
    GT_U8       i;          /* Per function loop counter    */
    printf("cpssInitList[] = {\n", __FILE__, __FUNCTION__, __LINE__, rc);/*xcat debug*/
#ifdef IMPL_PORT
   printf("0   IMPL_PORT\n");/*xcat debug*/
#endif

#ifdef IMPL_PHY
   printf("1   IMPL_PHY\n");/*xcat debug*/
#endif

#ifdef IMPL_BRIDGE
   printf("2   IMPL_BRIDGE\n");/*xcat debug*/
#endif

#ifdef IMPL_NETIF
   printf("3   IMPL_NETIF\n");/*xcat debug*/
#endif

#ifdef IMPL_MIRROR
   printf("4   IMPL_MIRROR\n");/*xcat debug*/
#endif

#ifdef IMPL_PCL
   printf("5   IMPL_PCL\n");/*xcat debug*/
#endif

#ifdef IMPL_POLICER
   printf("6   IMPL_POLICER\n");/*xcat debug*/
#endif

#ifdef IMPL_TRUNK
    printf("7   IMPL_TRUNK\n");/*xcat debug*/
#endif

#ifdef IMPL_IP
    printf("8   IMPL_IP\n} \n");/*xcat debug*/
#endif


    /* for given device id init all cpss modules */
    i = 0;
    while ((cpssInitList[i] != 0) && (GT_OK == rc))
    {
        /* for given device id init cpss module */
        rc = cpssInitList[i](dev, libInitParams, sysConfigParams);
        printf("cpssInitList[%d] rc=%d\n", i, rc);/*xcat debug*/ 
        i++;
    }
    return rc;
} /* appDemoDxLibrariesInit */

/*******************************************************************************
* appDemoDxTrafficEnable
*
* DESCRIPTION:
*      Enable traffic for specific device.
*      Enable ports and device.
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDxTrafficEnable
(
    IN  GT_U8   dev
)
{
    GT_STATUS   rc = GT_OK;
    GT_U8       port;


    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    /* for given device id loop on ports */
    for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfPorts; port++)
    {
        if (PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_NOT_EXISTS_E)
            continue;

        rc = cpssDxChPortEnableSet(dev, port, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortEnableSet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }
    }

    /* enable device */
	#if 0
    rc = cpssDxChCfgDevEnable(dev, GT_TRUE);
	#endif
	rc = cpssDxChCfgDevEnable(dev, GT_FALSE);/*disable device  and after setting Switch Fabric , enable it  */
	CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgDevEnable", rc);
    return rc;
} /* appDemoDxTrafficEnable */

/*******************************************************************************
* appDemoDxPpGeneralInit
*
* DESCRIPTION:
*      Implements general initialization for specific device.
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDxPpGeneralInit
(
    IN  GT_U8   dev
)
{
    GT_STATUS           rc = GT_OK;
    CPSS_PORTS_BMP_STC  portsMembers;
    CPSS_PORTS_BMP_STC  portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC  cpssVlanInfo;   /* cpss vlan info format    */
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd; /* ports tagging command */
    GT_U8               port;           /* current port number      */
    GT_U16              vid;            /* default VLAN id          */
    GT_U32              mruSize, cpuMruSize;
    GT_BOOL             flowControlEnable;

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    /***********************/
    /* FDB configuration   */
    /***********************/
    rc = appDemoDxChFdbInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChFdbInit", rc);
    /*********************************************************/
    /* Port Configuration                                    */
    /*********************************************************/
    /* Jambo frame support */
    mruSize = 1522; /* default */
    cpuMruSize = 16382; /* maximum */

#ifndef __AX_PLATFORM__
    if( (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT_E)
        && (PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E) ){
		rc = cpssDxChIpRouterMacSaLsbModeSet(dev,CPSS_SA_LSB_PER_PORT_E);
		if(rc != GT_OK) 
			return rc;
	    #ifdef AX_FULL_DEBUG
		osPrintf("appDemoDxPpGeneralInit::cpssDxChIpRouterMacSaLsbModeSet BY_PORT port %d ok\r\n",port);
	    #endif
    }

	rc = cpssDxChIpRoutingEnable(dev,GT_TRUE);
	if(rc != GT_OK) 
		return rc;
	
	
	rc = cpssDxChBrgGenArpBcastToCpuCmdSet(dev,CPSS_PACKET_CMD_MIRROR_TO_CPU_E);
	if(rc != GT_OK)
		return rc;
	rc = cpssDxChBrgGenRipV1MirrorToCpuEnable(dev,GT_TRUE);
	if(rc != GT_OK)
		return rc;
	rc = cpssDxChIpExceptionCommandSet(dev,CPSS_DXCH_IP_EXCP_UC_TTL_EXCEED_E,	\
					CPSS_IP_PROTOCOL_IPV4_E,CPSS_PACKET_CMD_TRAP_TO_CPU_E);
	if(rc != GT_OK)
		return rc;
	
	/*by zhubo@autelan.com 2008.9.19 IPv4 link local mirror set*/
	rc = cpssDxChIPv4LinkLocalMirrorEnable(dev,GT_TRUE);
	if(rc != GT_OK) {
		osPrintfErr("set link local multicast packet mirror error %d on device %d\n",rc, dev);
		return rc;
	}


    #if 1   /* zhangdi cancel for debug!@@@@@, here we dead */
	/*by yangxs@autelan.com 2010.08.05 set net intf multicast packet trap to cpu*/
	rc = cpssDxChNetIfSetMultiTrapToCpu(dev,GT_TRUE);
	if(rc != GT_OK) {
		osPrintfErr("set net intf multicast packet trap to cpu error %d on device %d\n",rc, dev);
		return rc;
	}
	#endif 
	
	/* by yinlm@aulan.com 2008.12.15 set vlan egress-filter enable */
	rc = cpssDxChBrgVlanEgressFilteringEnable(dev, GT_TRUE);
	if(rc != GT_OK) {
		osPrintfErr("set vlan egress-filter enable error %d on device %d\n",rc, dev);
		return rc;
	}
#endif

    for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfPorts; port++)
    {

		
        if (PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_NOT_EXISTS_E)
            continue;

        /*  The next 2 calls are to set Port-Override-Tc-Enable */
        /* Set port trust mode */
        rc = cpssDxChCosPortQosTrustModeSet(dev, port, CPSS_QOS_PORT_TRUST_L2_L3_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCosPortQosTrustModeSet", rc);
        if (rc != GT_OK)
            return rc;

        /* set: Packet's DSCP is not remapped */
        rc = cpssDxChCosPortReMapDSCPSet(dev, port, GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCosPortReMapDSCPSet", rc);
        if (rc != GT_OK)
            return rc;

        rc = cpssDxChPortDuplexModeSet(dev, port, CPSS_PORT_FULL_DUPLEX_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortDuplexModeSet", rc);
        if (rc != GT_OK)
            return rc;
		
        if(PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType < PRV_CPSS_PORT_XG_E)
        {

            rc = cpssDxChPortDuplexAutoNegEnableSet(dev, port, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortDuplexAutoNegEnableSet", rc);
            if (rc != GT_OK)
                return rc;

            #ifdef __AX_PLATFORM__
			rc = cpssDxChPortFlowCntrlAutoNegEnableSet(dev, port, GT_FALSE, GT_TRUE);
            #else
			rc = cpssDxChPortFlowCntrlAutoNegEnableSet(dev, port, GT_TRUE, GT_TRUE);
            #endif
			
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortFlowCntrlAutoNegEnableSet", rc);
            if (rc != GT_OK)
                return rc;
            #ifdef AX_FULL_DEBUG
			osPrintf("appDemoDxPpGeneralInit::cpssDxChPortFlowCntrlAutoNegEnableSet port %d ok\r\n",port);
            #endif

            rc = cpssDxChPortSpeedAutoNegEnableSet(dev, port, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortSpeedAutoNegEnableSet", rc);
            if (rc != GT_OK)
                return rc;
            #ifdef AX_FULL_DEBUG
			osPrintf("appDemoDxPpGeneralInit::cpssDxChPortSpeedAutoNegEnableSet port %d ok\r\n",port);
            #endif
            #ifndef __AX_PLATFORM__
			
			#if 1  
			/* set Port type SGMII */
			rc = cpssDxChPortTypeSet(dev, port, PORT_TYPE_SGMII);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortTypeSet", rc);
            if (rc != GT_OK)
                return rc;
            #endif	/* Here the code dead !!!  xcat debug */	
            #ifdef AX_FULL_DEBUG
			osPrintf("appDemoDxPpGeneralInit::cpssDxChPortTypeSet port %d type SGMII ok\r\n",port);			
            #endif
		
			/* set Port In-band Auto-Negotiation disable */
			rc = cpssDxChPortInbandAutoNegEnableSet(dev,port,GT_FALSE);
			CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortInbandAutoNegEnableSet", rc);
            if (rc != GT_OK)
                return rc;			
            #ifdef AX_FULL_DEBUG
			osPrintf("appDemoDxPpGeneralInit::cpssDxChPortInbandAutoNegEnableSet port %d FALSE ok\r\n",port);			
            #endif
		
			#if 1 
			/* set Port In-band Auto-Negotiation mode SGMII */
			rc = cpssDxChPortInbandAutoNegModeSet(dev,port,PORT_INBAND_AN_SGMII_MODE);
			CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortInbandAutoNegModeSet", rc);
            if (rc != GT_OK)
                return rc;
            #endif 	/* Here the code dead !!!  xcat debug */	    	
            #ifdef AX_FULL_DEBUG
			osPrintf("appDemoDxPpGeneralInit::cpssDxChPortInbandAutoNegModeSet port %d SGMII ok\r\n",port);			
            #endif
	
			#if 1   
			/* set Port MAC not Reset */
			rc = cpssDxChPortGEMacResetEnableSet(dev,port,GT_FALSE);
			CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortGEMacResetEnableSet", rc);			
            if (rc != GT_OK)
                return rc;
            #ifdef AX_FULL_DEBUG
			osPrintf("appDemoDxPpGeneralInit::cpssDxChPortGEMacResetEnableSet port %d FALSE ok\r\n",port);			
            #endif  
			#endif 
			
			/* set Port IPG */
			rc = cpssDxChPortIPGSet(dev,port,0xc);
			CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortIPGSet", rc);
			if(rc != GT_OK) 
				return rc;
            #ifdef AX_FULL_DEBUG
			osPrintf("appDemoDxPpGeneralInit::cpssDxChPortIPGSet port %d ok\r\n",port);
            #endif
            #endif
        }	
		
        #if 1   /*luoxun --combination may be wrong*/
        /* XG Ports */
		else
		{
        osPrintfDbg("%s %s %d PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType %#x dev %#x port %#x\n", __FILE__, __FUNCTION__, __LINE__,
			PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType,dev,port);/*xcat debug*/
			/* set Port MAC not Reset */
			rc = cpssDxChPortXGMacResetEnableSet(dev,port,GT_FALSE);
			CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortXGMacResetEnableSet", rc);			
            if (rc != GT_OK)
                return rc;
			#ifdef AX_FULL_DEBUG
			osPrintf("appDemoDxPpGeneralInit::cpssDxChPortXGMacResetEnableSet port %d FALSE ok\r\n",port);			
			#endif
		}
        #endif

        /* Disable Flow Control for revision 3 in DxCh2, DxCh3, XCAT. */
        if(appDemoPpConfigList[dev].flowControlDisable)
        {
            flowControlEnable = GT_FALSE;
        }
        else
        {
            flowControlEnable = GT_TRUE;
        }
        rc = cpssDxChPortFlowControlEnableSet(dev, port, flowControlEnable);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortFlowControlEnableSet", rc);
        if (rc != GT_OK)
            return rc;

        rc = cpssDxChPortPeriodicFcEnableSet(dev, port, GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPeriodicFcEnableSet", rc);
        if (rc != GT_OK)
            return rc;

        rc = cpssDxChPortMruSet(dev, port, mruSize);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortMruSet", rc);
        if (rc != GT_OK)
            return rc;

        rc = cpssDxChPortCrcCheckEnableSet(dev, port, GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortCrcCheckEnableSet", rc);
        if (rc != GT_OK)
            return rc;

        /* tx Watchdog supported on DxCh1, DxCh2, DxCh3 */
        if((PRV_CPSS_PP_MAC(dev)->devFamily <= CPSS_PP_FAMILY_CHEETAH3_E))
        {
            rc = cpssDxChPortTxWatchdogEnableSet(dev, port, GT_TRUE, 500);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortTxWatchdogEnableSet", rc);
            if (rc != GT_OK)
                return rc;
        }

        /* call cpss api function with UP == 1 */
        rc = cpssDxChPortDefaultUPSet(dev, port, 1);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortDefaultUPSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        #ifndef __AX_PLATFORM__
		rc = cpssDxChIpRouterMacSaModifyEnable(dev,port,GT_TRUE);
		if(rc != GT_OK) 
			return rc;
        #ifdef AX_FULL_DEBUG
		osPrintf("appDemoDxPpGeneralInit::cpssDxChIpRouterMacSaModifyEnable port %d ok\r\n",port);
        #endif
        #endif
    }


    rc = cpssDxChPortMruSet(dev, CPSS_CPU_PORT_NUM_CNS, cpuMruSize);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortMruSet", rc);
    if (rc != GT_OK)
        return rc;
    #ifdef AX_FULL_DEBUG
	osPrintf("appDemoDxPpGeneralInit::cpssDxChPortMruSet cpu size %d ok\r\n",cpuMruSize);
    #endif

#ifndef __AX_PLATFORM__

	rc = cpssDxChPortInvalidCrcModeSet(dev,CPSS_CPU_PORT_NUM_CNS,CPSS_DXCH_PORT_NOT_CONTAIN_4B_CRC_MODE_E);
	if(rc != GT_OK) {
		osPrintf("appDemoDxPpGeneralInit::cpssDxChPortInvalidCrcModeSet cpu not contain 4B CRC err %d\n",rc);
		return rc;
	}
    #ifdef AX_FULL_DEBUG
	osPrintf("appDemoDxPpGeneralInit::cpssDxChPortInvalidCrcModeSet cpu not contain 4B CRC ok\n");
    #endif
	
	rc = cpssDxChIpExceptionCommandSet(dev,CPSS_DXCH_IP_EXCP_UC_RPF_FAIL_E,CPSS_IP_PROTOCOL_IPV4V6_E,CPSS_PACKET_CMD_DROP_HARD_E);
	if(rc != GT_OK) {
		osPrintf("appDemoDxPpGeneralInit::cpssDxChIpExceptionCommandSet dev %d RPF fail hard drop err %d\n",dev,rc);
		return rc;
	}
    #ifdef AX_FULL_DEBUG
	osPrintf("appDemoDxPpGeneralInit::cpssDxChIpExceptionCommandSet dev %d RPF fail hard drop ok\n",dev);
    #endif
	/* 
        * by qinhs@autelan.com on 11-12-2008
        *	Tx port shaper token bucket update rate in step of 260Kbps and 
        * 65kbps for slow rate. The range of shaper value as follows :
        *	<260Kbps - 1064Mbps> for normal rate mode and
        *	<65Kbps - 266Mbps> for slow rate mode
	 */
	if(PRV_CPSS_PP_MAC(dev)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E){
    	rc = cpssDxChPortTxShaperGlobalParamsSet(dev, 6, 6, 4);
    	if(rc != GT_OK) {
    		osPrintfErr("Dx device%d general init set Tx shaper global parameter error %d\n",dev,rc);
    		return rc;
    	}
    
    	rc = cpssDxChNetIfCpuRateLimiterInit(dev);
    	if(rc != GT_OK) {
    		osPrintf("appDemoDxPpGeneralInit::cpssDxChNetIfCpuRateLimiterInit dev %d err %d\n",dev,rc);	
    		return rc;
    	}
        
    	rc = cpssDxChNetIfCpuRxFcInit(dev);
    	if(rc != GT_OK) {
    		osPrintf("appDemoDxPpGeneralInit::cpssDxChNetIfCpuRxFcInit dev %d err %d\n",dev,rc);	
    		return rc;
    	}
        
    	
    	/*add by zhubo@autelan.com 2008.8.9*/
    	rc = cpssDxChPortTxTailDropUcEnableSet(dev,1);
    	if(rc != GT_OK) {
    		osPrintf("appDemoDxPpGeneralInit::cpssDxChPortTxTailDropUcEnableSet dev %d err %d\n",dev,rc);	
    		return rc;
    	}
    
    	rc = cpssDxChPortTxBuffersSharingMaxLimitSet(dev,0xeff);
    	if(rc != GT_OK) {
    		osPrintf("appDemoDxPpGeneralInit::cpssDxChPortTxBuffersSharingMaxLimitSet dev %d err %d\n",dev,rc);	
    		return rc;
    	}
	}
#endif
    /*****************************************************/
    /* Cascade ports configuration                       */
    /*****************************************************/
    rc = prvCscdPortsInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCscdPortsInit", rc);
    if (rc != GT_OK)
       return rc;


    /****************************************************/
    /* XG ports additional board-specific configuration */
    /****************************************************/
    rc = prv10GPortsConfig(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("prv10GPortsConfig", rc);
    if (rc != GT_OK)
       return rc;


    /*********************************************************/
    /* Default VLAN configuration: VLAN 1 contains all ports */
    /*********************************************************/

    /* default VLAN id is 1 */
    vid = 1;

    /* Fill Vlan info */
    osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));

    cpssVlanInfo.naMsgToCpuEn           = GT_TRUE;

    cpssVlanInfo.autoLearnDisable       = GT_FALSE; /* Enable auto learn on VLAN */
    cpssVlanInfo.unkUcastCmd            = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv6McastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv4McastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregNonIpMcastCmd     = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregNonIpv4BcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv4BcastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.floodVidx              = 0xFFF;

    /* Fill ports and tagging members */
    osMemSet(&portsMembers, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTagging, 0, sizeof(CPSS_PORTS_BMP_STC));
    osMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));

    /* set all ports as VLAN members */
    for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfPorts; port++)
    {
        if (PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_NOT_EXISTS_E)
            continue;
        /* set the port as member of vlan */
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembers, port);

        /* Set port pvid */
        rc = cpssDxChBrgVlanPortVidSet(dev, port, vid);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanPortVidSet", rc);
        if(rc != GT_OK)
             return rc;

        portsTaggingCmd.portsCmd[port] = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
    }

    /* Write default VLAN entry */
    rc = cpssDxChBrgVlanEntryWrite(dev, vid,
                                   &portsMembers,
                                   &portsTagging,
                                   &cpssVlanInfo,
                                   &portsTaggingCmd);

    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanEntryWrite", rc);
    if (rc != GT_OK)
       return rc;
	
	/* set NA msg to CPU enable for CPU control fdb learning */
	if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
		rc = cpssDxChBrgVlanNaToCpuEnable(dev, vid, GT_TRUE);
	else
		rc = cpssDxChBrgVlanNaToCpuEnable(dev, vid, GT_FALSE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanNaToCpuEnable", rc);
    if (rc != GT_OK)
       return rc;
	osPrintf("set NA msg to CPU disable dev %d OK.\n",dev);	
    
    /*********************************************************/
    /* Default TTI configuration (xCat A1 and above only):   */
    /*   - Set TTI PCL ID for IPV4 lookup to 1               */
    /*   - Set TTI PCL ID for MPLS lookup to 2               */
    /*   - Set TTI PCL ID for ETH  lookup to 3               */
    /*   - Set TTI PCL ID for MIM  lookup to 4               */
    /*********************************************************/
    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
    {
        rc = cpssDxChTtiPclIdSet(dev,CPSS_DXCH_TTI_KEY_IPV4_E,1);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPclIdSet", rc);
        if (rc != GT_OK)
           return rc;
        rc = cpssDxChTtiPclIdSet(dev,CPSS_DXCH_TTI_KEY_MPLS_E,2);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPclIdSet", rc);
        if (rc != GT_OK)
           return rc;
        rc = cpssDxChTtiPclIdSet(dev,CPSS_DXCH_TTI_KEY_ETH_E,3);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPclIdSet", rc);
        if (rc != GT_OK)
           return rc;
        rc = cpssDxChTtiPclIdSet(dev,CPSS_DXCH_TTI_KEY_MIM_E,4);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTtiPclIdSet", rc);
        if (rc != GT_OK)
           return rc;
    }

    if(PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(dev))
    {
        /* Mirror initialization -
          init Rx and Tx Global Analyzer indexes to be 0 for Ingress
          and 1 for Egress */
        rc = cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet(dev,
                                                             GT_TRUE, 1);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet(dev,
                                                             GT_TRUE, 0);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return rc;

} /* appDemoDxPpGeneralInit */




/*******************************************************************************
 * Private function implementation
 ******************************************************************************/

/*******************************************************************************
* prvCscdPortsInit
*
* DESCRIPTION:
*       Configure cascade ports (if they are present)
*
* INPUTS:
*       dev                      -   The PP's device number.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       This function is called from appDemoDxPpGeneralInit
*
*******************************************************************************/
static GT_STATUS prvCscdPortsInit
(
    IN  GT_U8   dev
)
{
    GT_STATUS rc = GT_OK;
    GT_U32                      ii,jj,mm;               /* Loop index.                  */
    GT_TRUNK_ID                 trunkId,trunkIds[3]={0,0,0};    /* the cascade trunkId */
    GT_U32                      trustDsaQosEnable = 0;
    CPSS_QOS_ENTRY_STC          portQosCfg;
    GT_U32                      policyEn;               /* Enable policy Engine for port */
    GT_U8                       targetDevNum;           /* the device to be reached via cascade port for analyzing */
    GT_U8                       targetHwDevNum;          /* the HW num for device to be reached via cascade port for analyzing */
    GT_U8                       portNum;
    CPSS_CSCD_PORT_TYPE_ENT     cscdPortType;           /* Enum of cascade port or network port */
    CPSS_CSCD_LINK_TYPE_STC     cpssCascadeLink;
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT  currentSrcPortTrunkHashEn;
    CPSS_PORTS_BMP_STC          cscdTrunkBmp;/* bmp of ports members in the cascade trunk */

    if (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_BAD_PARAM;
    }

    if (0 == appDemoPpConfigList[dev].numberOfCscdPorts)
    {
        return GT_OK;
    }

    if ((appDemoPpConfigList[dev].numberOfCscdPorts > PRV_CPSS_MAX_PP_PORTS_NUM_CNS) ||
        (appDemoPpConfigList[dev].numberOfCscdTargetDevs > CPSS_CSCD_MAX_DEV_IN_CSCD_CNS))
    {
        return GT_BAD_PARAM;
    }

    /* 1. Initialize the cascade library */

    /* Set Dsa ingress filter bit (only in Cheetah) */
    rc = cpssDxChCscdDsaSrcDevFilterSet(dev, GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdDsaSrcDevFilterSet", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    for(ii = 0; ii < appDemoPpConfigList[dev].numberOfCscdPorts; ++ii)
    {
        /*2.1. Configure ports to be cascade ports*/

        portNum = appDemoPpConfigList[dev].cscdPortsArr[ii].portNum;
        cscdPortType = appDemoPpConfigList[dev].cscdPortsArr[ii].cscdPortType;

        rc = cpssDxChCscdPortTypeSet(dev, portNum, cscdPortType);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdPortTypeSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        policyEn = 1; /* enable policy engine */
        if((CPSS_CSCD_PORT_DSA_MODE_REGULAR_E == cscdPortType)
           || (CPSS_CSCD_PORT_DSA_MODE_EXTEND_E == cscdPortType))
        {
            /* disable policy engine for cascading ports with extended DSA tag packets*/
            policyEn = (CPSS_CSCD_PORT_DSA_MODE_REGULAR_E == cscdPortType) ? 1 : 0;

            /* Enable Trust DSA Tag QoS profile for cascade port */
            trustDsaQosEnable = 1;
            /* Cascade port's QoS profile default settings */
            portQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_HARD_E;
            portQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            portQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            portQosCfg.qosProfileId     = 64; /*#define GT_CORE_CHEETAH_TC_FIRST_ENTRY 64*/
        }
        else if(CPSS_CSCD_PORT_NETWORK_E == cscdPortType)
        {
            /* Disable Trust DSA Tag QoS profile for network port */
            trustDsaQosEnable = 0;
            /* Network port's QoS profile default settings */
            portQosCfg.assignPrecedence = CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_SOFT_E;
            portQosCfg.enableModifyDscp = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            portQosCfg.enableModifyUp   = CPSS_PACKET_ATTRIBUTE_MODIFY_DISABLE_E;
            portQosCfg.qosProfileId     = 64; /*#define GT_CORE_CHEETAH_TC_FIRST_ENTRY 64*/
        }
        else
        {
            return GT_BAD_PARAM;
        }

        /* write data to Ports VLAN and QoS Configuration Table, word0*/
        rc = cpssDxChCosTrustDsaTagQosModeSet(dev, portNum, trustDsaQosEnable);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCosTrustDsaTagQosModeSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* enable/disable policy */
        rc = cpssDxChPclPortIngressPolicyEnable(dev, portNum, policyEn);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclPortIngressPolicyEnable", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChCosPortQosConfigSet(dev, portNum, &portQosCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCosPortQosConfigSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* set the "send to CPU AU" from this port -- fix bug #27827 */
        rc = cpssDxChBrgFdbNaToCpuPerPortSet(dev, portNum, GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbNaToCpuPerPortSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /*2.2.Board-specific cascade ports configuration */
        if(NULL != appDemoPpConfigList[dev].internalCscdPortConfigFuncPtr)
        {
            rc = appDemoPpConfigList[dev].internalCscdPortConfigFuncPtr(dev, portNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoPpConfigList[dev].internalCscdPortConfigFuncPtr", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }


    if(appDemoPpConfigList[dev].numberOfCscdTrunks)
    {
        if(appDemoPpConfigList[dev].numberOfCscdTrunks > 3)
        {
            rc = GT_FAIL;
            /* error -- more then 3 cascade trunks needed ? */
            CPSS_ENABLER_DBG_TRACE_RC_MAC("error 1 -- more then 3 cascade trunks needed ?", rc);
            return rc;
        }

        for(ii = 0; ii < appDemoPpConfigList[dev].numberOfCscdPorts; ++ii)
        {
            /*2.1. Configure ports to be cascade ports*/

            portNum = appDemoPpConfigList[dev].cscdPortsArr[ii].portNum;
            cscdPortType = appDemoPpConfigList[dev].cscdPortsArr[ii].cscdPortType;
            trunkId = appDemoPpConfigList[dev].cscdPortsArr[ii].trunkId;

            if(0xFFFF == trunkId || trunkId == 0)
            {
                continue;
            }

            /* check if this cascade trunk already built */
            for(jj = 0 ; jj < 3 ;jj++)
            {
                if(trunkId == trunkIds[jj])
                {
                    break;
                }

                if(0 == trunkIds[jj])
                {
                    break;
                }
            }/*jj*/

            if(jj == 3)
            {
                rc = GT_FAIL;
                /* error -- more then 3 cascade trunks needed ? */
                CPSS_ENABLER_DBG_TRACE_RC_MAC("error 2-- more then 3 cascade trunks needed ?", rc);
                return rc;
            }

            if(trunkId == trunkIds[jj])
            {
                /* this trunk already built */
                continue;
            }

            DBG_TRACE((" create on dev[%d] cascade trunk [%d] \n",dev,trunkId));

            trunkIds[jj] = trunkId;

            /* clear the local ports bmp for the new cascade trunk */
            cscdTrunkBmp.ports[0] = cscdTrunkBmp.ports[1] = 0;

            for(mm = 0; mm < appDemoPpConfigList[dev].numberOfCscdPorts; mm ++)
            {
                portNum = appDemoPpConfigList[dev].cscdPortsArr[mm].portNum;
                cscdPortType = appDemoPpConfigList[dev].cscdPortsArr[mm].cscdPortType;
                trunkId = appDemoPpConfigList[dev].cscdPortsArr[mm].trunkId;

                if(trunkId != trunkIds[jj])
                {
                    continue;
                }

                CPSS_PORTS_BMP_PORT_SET_MAC(&cscdTrunkBmp,portNum);
            }/*mm*/

            rc = cpssDxChTrunkCascadeTrunkPortsSet(dev,trunkIds[jj],&cscdTrunkBmp);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkCascadeTrunkPortsSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }/*ii*/
    }/* if cascade trunks */

    /* 4. Configure the cascade map table */
    for(ii = 0; ii < appDemoPpConfigList[dev].numberOfCscdTargetDevs; ++ii)
    {
        targetDevNum = appDemoPpConfigList[dev].cscdTargetDevsArr[ii].targetDevNum;

        if( NULL != prvCpssPpConfig[targetDevNum] )
        {
            rc = cpssDxChCfgHwDevNumGet(targetDevNum, &targetHwDevNum);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            targetHwDevNum = targetDevNum;
        }

        rc = cpssDxChCscdDevMapTableGet(dev,targetHwDevNum, 0, &cpssCascadeLink,
                                        &currentSrcPortTrunkHashEn);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdDevMapTableGet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        cpssCascadeLink.linkNum = appDemoPpConfigList[dev].cscdTargetDevsArr[ii].linkToTargetDev.linkNum;
        cpssCascadeLink.linkType = appDemoPpConfigList[dev].cscdTargetDevsArr[ii].linkToTargetDev.linkType;

        rc = cpssDxChCscdDevMapTableSet(dev,targetHwDevNum, 0, &cpssCascadeLink,
                                        currentSrcPortTrunkHashEn);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdDevMapTableSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* 5. Configure the port VLAN/VIDX membership */
    /*empty*/

    return rc;
}

/*******************************************************************************
* prv10GPortsConfig
*
* DESCRIPTION:
*       The function performs additional board-specific onfigure
*       for XG ports.
*
* INPUTS:
*       dev         - The PP's device number.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       This function is called from appDemoDxPpGeneralInit
*
*******************************************************************************/
static GT_STATUS prv10GPortsConfig
(
    IN  GT_U8   dev
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      ii;
    GT_U32      regAddr;  /* address of register */	
	unsigned int value;
    GT_U8       portNum;
    CPSS_CSCD_PORT_TYPE_ENT  portType;

    if (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        return GT_BAD_PARAM;
    }

    if((0 == appDemoPpConfigList[dev].numberOf10GPortsToConfigure) ||
       (NULL == appDemoPpConfigList[dev].internal10GPortConfigFuncPtr))
    {
        return GT_OK;
    }

    if (appDemoPpConfigList[dev].numberOf10GPortsToConfigure > PRV_CPSS_MAX_PP_PORTS_NUM_CNS)
    {
        return GT_BAD_PARAM;
    }

    for(ii = 0; ii < appDemoPpConfigList[dev].numberOf10GPortsToConfigure; ++ii)
    {
        rc = appDemoPpConfigList[dev].internal10GPortConfigFuncPtr(dev,
                                      appDemoPpConfigList[dev].ports10GToConfigureArr[ii]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoPpConfigList[dev].internal10GPortConfigFuncPtr", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* 
	  * Configure 98Dx804 XG port 26[connect to XAUI PHY mv82210] as Cascade port
	  */
    #ifndef __AX_PLATFORM__
	if(CPSS_98DX804_CNS == appDemoPpConfigList[dev].deviceId)
	{		
		portNum = 26;
		portType = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E;

        /*luoxun -- 'struct _addrSpace' has no member named 'cscdHeadrInsrtConf' in cpss3.4*/
		regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->haRegs.cscdHeadrInsrtConf[0];/* [0] or [1] is the same value 0x07800004*/
	//	regAddr = 0;
        /* The port is a cascading port used for inter-device connectivity and
           	    all packets received by it and transmitted by it must be DSA-tagged*/
        if (prvCpssDrvHwPpSetRegField(dev, regAddr, portNum, 1, 1) != GT_OK)
            return GT_HW_ERROR;

        /* set DSA support mode */
        value = (portType == CPSS_CSCD_PORT_DSA_MODE_REGULAR_E) ? 0 : 1;

        /*luoxun -- 'struct _addrSpace' has no member named 'cscdHeadrInsrtConf' in cpss3.4*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(dev)->haRegs.eggrDSATagTypeConf[0];/* 0x07800020*/
       // regAddr = 0;

        /* set relevant bit of Egress DSA tag type Configuration register */
        if (prvCpssDrvHwPpSetRegField(dev, regAddr, portNum, 1, value) != GT_OK)
            return GT_HW_ERROR;

		/* set the MRU of the cascade port to be big enough for DSA tag */
		if (cpssDxChPortMruSet(dev, portNum, 1536) != GT_OK)
		{
			return GT_FAIL;
		}
	}
    #endif

    return rc;
}


/*******************************************************************************
* appDemoConvert2DxChPhase1Info
*
* DESCRIPTION:
*       Set CPSS info from the core format.
*
* INPUTS:
*       dev                      -   The PP's device number to be initialized.
*       appCpssPpPhase1ParamsPtr    -   PP phase 1 configuration parameters.
*
* OUTPUTS:
*       cpssPpPhase1InfoPtr - CPSS PP phase 1 configuration parameters.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on bad pointers
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       This function converts HW data into CPSS phase 1 format.
*
*******************************************************************************/
static GT_STATUS appDemoConvert2DxChPhase1Info
(
    IN  GT_U8                               dev,
    IN  CPSS_PP_PHASE1_INIT_PARAMS          *appCpssPpPhase1ParamsPtr,
    OUT CPSS_DXCH_PP_PHASE1_INIT_INFO_STC   *cpssPpPhase1InfoPtr
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32  ii;

    if ((NULL == appCpssPpPhase1ParamsPtr) ||
        (NULL == cpssPpPhase1InfoPtr))
    {
        return GT_BAD_PTR;
    }

    osMemSet(cpssPpPhase1InfoPtr, 0, sizeof(*cpssPpPhase1InfoPtr));

    cpssPpPhase1InfoPtr->devNum          = dev;
    cpssPpPhase1InfoPtr->busBaseAddr     = appCpssPpPhase1ParamsPtr->baseAddr;
    cpssPpPhase1InfoPtr->internalPciBase = appCpssPpPhase1ParamsPtr->internalPciBase;
    cpssPpPhase1InfoPtr->intVecNum       = appCpssPpPhase1ParamsPtr->intVecNum;
    cpssPpPhase1InfoPtr->intMask         = appCpssPpPhase1ParamsPtr->intMask;
    cpssPpPhase1InfoPtr->coreClock =
        (appCpssPpPhase1ParamsPtr->coreClk == APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS) ?
         CPSS_DXCH_AUTO_DETECT_CORE_CLOCK_CNS : appCpssPpPhase1ParamsPtr->coreClk;
    cpssPpPhase1InfoPtr->mngInterfaceType   = appCpssPpPhase1ParamsPtr->mngInterfaceType;
    cpssPpPhase1InfoPtr->ppHAState      = appCpssPpPhase1ParamsPtr->sysHAState;
    cpssPpPhase1InfoPtr->powerDownPortsBmp = appCpssPpPhase1ParamsPtr->powerDownPortsBmp;
    cpssPpPhase1InfoPtr->serdesRefClock = appCpssPpPhase1ParamsPtr->serdesRefClock;
    cpssPpPhase1InfoPtr->initSerdesDefaults = appCpssPpPhase1ParamsPtr->initSerdesDefaults;

    if(appCpssPpPhase1ParamsPtr->numOfPortGroups)
    {
        cpssPpPhase1InfoPtr->busBaseAddr = CPSS_PARAM_NOT_USED_CNS;

        cpssPpPhase1InfoPtr->numOfPortGroups = appCpssPpPhase1ParamsPtr->numOfPortGroups;
        if(appCpssPpPhase1ParamsPtr->numOfPortGroups > APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS)
        {
            return GT_BAD_STATE;
        }

        cpssPpPhase1InfoPtr->multiPortGroupsInfoPtr = portGroupsPhase1InfoArr;

        for(ii = 0 ; ii < appCpssPpPhase1ParamsPtr->numOfPortGroups ; ii++)
        {
            portGroupsPhase1InfoArr[ii].busBaseAddr = appCpssPpPhase1ParamsPtr->portGroupsInfo[ii].busBaseAddr;
            portGroupsPhase1InfoArr[ii].internalPciBase = appCpssPpPhase1ParamsPtr->portGroupsInfo[ii].internalPciBase;
            portGroupsPhase1InfoArr[ii].intVecNum = appCpssPpPhase1ParamsPtr->portGroupsInfo[ii].intVecNum;
            portGroupsPhase1InfoArr[ii].intMask = appCpssPpPhase1ParamsPtr->portGroupsInfo[ii].intMask;
        }
    }

	osPrintfDbg("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);/*xcat debug*/	
    osPrintfDbg("******** Here we set isExternalCpuConnected = GT_TRUE\n");
    osPrintfDbg("******** And then the code will set the BAR of SW !!!\n");
	
	/* Because we use the external CPU zhangdi 2011-03-06 */
	/* cpssPpPhase1InfoPtr->isExternalCpuConnected = GT_FALSE; */
    cpssPpPhase1InfoPtr->isExternalCpuConnected = GT_TRUE;

    return rc;
} /* appDemoConvert2DxChPhase1Info */

/*******************************************************************************
* appDemoConvert2DxChPhase2Info
*
* DESCRIPTION:
*       Convert core phase 2 info to CPSS phase 2 info.
*
* INPUTS:
*       appCpssPpPhase2ParamsPtr    -   PP phase 2 configuration parameters.
*
* OUTPUTS:
*       cpssPpPhase2InfoPtr         - CPSS PP phase 2 configuration parameters.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on bad pointers
*       GT_FAIL     - otherwise
*
* COMMENTS:
*       This function converts HW data into CPSS phase 2 format.
*
*******************************************************************************/
static GT_STATUS appDemoConvert2DxChPhase2Info
(
    IN  CPSS_PP_PHASE2_INIT_PARAMS          *appCpssPpPhase2ParamsPtr,
    OUT CPSS_DXCH_PP_PHASE2_INIT_INFO_STC   *cpssPpPhase2InfoPtr
)
{
    GT_STATUS               rc = GT_OK;
    CPSS_RX_BUF_INFO_STC    *cpssRxBufInfoPtr;  /* pointer to cpss rx buf format    */
    CPSS_RX_BUF_INFO_STC    *pssRxBufInfoPtr;   /* pointer to cpss rx buf format     */


    if ((NULL == appCpssPpPhase2ParamsPtr) ||
        (NULL == cpssPpPhase2InfoPtr))
    {
        return GT_BAD_PTR;
    }

    osMemSet(cpssPpPhase2InfoPtr, 0, sizeof(*cpssPpPhase2InfoPtr));

    cpssPpPhase2InfoPtr->newDevNum                  = appCpssPpPhase2ParamsPtr->devNum;
    cpssPpPhase2InfoPtr->netIfCfg.txDescBlock       = appCpssPpPhase2ParamsPtr->netIfCfg.txDescBlock;
    cpssPpPhase2InfoPtr->netIfCfg.txDescBlockSize   = appCpssPpPhase2ParamsPtr->netIfCfg.txDescBlockSize;
    cpssPpPhase2InfoPtr->netIfCfg.rxDescBlock       = appCpssPpPhase2ParamsPtr->netIfCfg.rxDescBlock;
    cpssPpPhase2InfoPtr->netIfCfg.rxDescBlockSize   = appCpssPpPhase2ParamsPtr->netIfCfg.rxDescBlockSize;

    cpssRxBufInfoPtr    = &cpssPpPhase2InfoPtr->netIfCfg.rxBufInfo;
    pssRxBufInfoPtr     = &appCpssPpPhase2ParamsPtr->netIfCfg.rxBufInfo;

    cpssPpPhase2InfoPtr->netifSdmaPortGroupId       = appCpssPpPhase2ParamsPtr->netifSdmaPortGroupId;

    cpssRxBufInfoPtr->allocMethod = pssRxBufInfoPtr->allocMethod;
    osMemCpy(cpssRxBufInfoPtr->bufferPercentage,
             pssRxBufInfoPtr->bufferPercentage,
             sizeof(pssRxBufInfoPtr->bufferPercentage));

    cpssRxBufInfoPtr->rxBufSize     = appCpssPpPhase2ParamsPtr->rxBufSize;
    cpssRxBufInfoPtr->headerOffset  = appCpssPpPhase2ParamsPtr->headerOffset;

    if (CPSS_RX_BUFF_DYNAMIC_ALLOC_E == cpssRxBufInfoPtr->allocMethod)
    {
        cpssRxBufInfoPtr->buffData.dynamicAlloc.mallocFunc =
            (GT_VOID*)pssRxBufInfoPtr->buffData.dynamicAlloc.mallocFunc;

        cpssRxBufInfoPtr->buffData.dynamicAlloc.numOfRxBuffers =
            pssRxBufInfoPtr->buffData.dynamicAlloc.numOfRxBuffers;
    }
    else if (CPSS_RX_BUFF_STATIC_ALLOC_E == cpssRxBufInfoPtr->allocMethod)
    {
        cpssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockPtr =
            pssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockPtr;
        cpssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockSize =
            pssRxBufInfoPtr->buffData.staticAlloc.rxBufBlockSize;
    }

    cpssPpPhase2InfoPtr->auqCfg.auDescBlock     = appCpssPpPhase2ParamsPtr->auqCfg.auDescBlock;
    cpssPpPhase2InfoPtr->auqCfg.auDescBlockSize = appCpssPpPhase2ParamsPtr->auqCfg.auDescBlockSize;

    cpssPpPhase2InfoPtr->fuqUseSeparate         = appCpssPpPhase2ParamsPtr->fuqUseSeparate;
    cpssPpPhase2InfoPtr->useSecondaryAuq        = GT_FALSE;/* appdemo/galtisWrappers not implemented the WA*/

    osMemCpy(&cpssPpPhase2InfoPtr->fuqCfg,
             &appCpssPpPhase2ParamsPtr->fuqCfg,
             sizeof(appCpssPpPhase2ParamsPtr->fuqCfg));

    return rc;
} /* appDemoConvert2DxChPhase2Info */

/*******************************************************************************
* appDemoCoreToCpssDxChPpInfoConvert
*
* DESCRIPTION:
*       convert application format to CPSS format for DxCh "PP logical init"
*
* INPUTS:
*       ppConfPtr    - The PP logical configuration parameters
*
* OUTPUTS:
*      ppLogicaslInfoPtr - CPSS format for ExMX device.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID appDemoCoreToCpssDxChPpInfoConvert
(
    IN  CPSS_PP_CONFIG_INIT_STC       *ppConfigPtr,
    OUT CPSS_DXCH_PP_CONFIG_INIT_STC  *ppLogicaslInfoPtr
)
{
    /* IP Unicast */
    ppLogicaslInfoPtr->maxNumOfVirtualRouters = ppConfigPtr->maxNumOfVirtualRouters;
    ppLogicaslInfoPtr->maxNumOfIpNextHop = ppConfigPtr->maxNumOfIpNextHop;

    /* IPv4 Unicast */
    ppLogicaslInfoPtr->maxNumOfIpv4Prefixes = ppConfigPtr->maxNumOfIpv4Prefixes;

    /* IPv4/IPv6 Multicast */
    ppLogicaslInfoPtr->maxNumOfIpv4McEntries = ppConfigPtr->maxNumOfIpv4McEntries;
    ppLogicaslInfoPtr->maxNumOfMll = ppConfigPtr->maxNumOfMll;

    /* IPv6 Unicast */
    ppLogicaslInfoPtr->maxNumOfIpv6Prefixes = ppConfigPtr->maxNumOfIpv6Prefixes;

    /* Tunnels */
    ppLogicaslInfoPtr->maxNumOfTunnelEntries = ppConfigPtr->maxNumOfTunnelEntries;

    /* ipv4 tunnel termination */
    ppLogicaslInfoPtr->maxNumOfIpv4TunnelTerms = ppConfigPtr->maxNumOfIpv4TunnelTerms;

    /* ip routing mode*/
    ppLogicaslInfoPtr->routingMode = ppConfigPtr->routingMode;

} /* appDemoCoreToCpssDxChPpInfoConvert */


/*******************************************************************************
 * CPSS modules initialization routines
 ******************************************************************************/

#ifdef IMPL_PORT
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortBufMg.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>

#define CHEETAH_CPU_PORT_PROFILE                   CPSS_PORT_RX_FC_PROFILE_1_E
#define CHEETAH_NET_GE_PORT_PROFILE                CPSS_PORT_RX_FC_PROFILE_2_E
#define CHEETAH_NET_10GE_PORT_PROFILE              CPSS_PORT_RX_FC_PROFILE_3_E
#define CHEETAH_CASCADING_PORT_PROFILE             CPSS_PORT_RX_FC_PROFILE_4_E

#define CHEETAH_GE_PORT_XON_DEFAULT                14 /* 28 Xon buffs per port   */
#define CHEETAH_GE_PORT_XOFF_DEFAULT               35 /* 70 Xoff buffs per port  */
#define CHEETAH_GE_PORT_RX_BUFF_LIMIT_DEFAULT      25 /* 100 buffers per port    */

#define CHEETAH_CPU_PORT_XON_DEFAULT               14 /* 28 Xon buffs per port   */
#define CHEETAH_CPU_PORT_XOFF_DEFAULT              35 /* 70 Xoff buffs per port  */
#define CHEETAH_CPU_PORT_RX_BUFF_LIMIT_DEFAULT     25 /* 100 buffers for CPU port */

#define CHEETAH_XG_PORT_XON_DEFAULT                25 /* 50 Xon buffs per port   */
#define CHEETAH_XG_PORT_XOFF_DEFAULT               85 /* 170 Xoff buffs per port */
#define CHEETAH_XG_PORT_RX_BUFF_LIMIT_DEFAULT      56 /* 224 buffers per port    */

static GT_STATUS dxChPortBufMgInit
(
    IN  GT_U8    dev
)
{
    GT_U8   port;
    GT_STATUS   rc;        /* return code  */
    CPSS_PORT_RX_FC_PROFILE_SET_ENT profile;
    /*luoxun --maybe need combination*/
    GT_U32  buffLimit[4][3] = { /* 4 profiles : values for Xon,Xoff,rxBuff */
    /* Profile 0 - Set CPU ports profile */
        {CHEETAH_CPU_PORT_XON_DEFAULT,CHEETAH_CPU_PORT_XOFF_DEFAULT,CHEETAH_CPU_PORT_RX_BUFF_LIMIT_DEFAULT},
    /* Profile 1 - Set Giga ports profile */
        {CHEETAH_GE_PORT_XON_DEFAULT,CHEETAH_GE_PORT_XOFF_DEFAULT,CHEETAH_GE_PORT_RX_BUFF_LIMIT_DEFAULT},
    /* Profile 2 - Set XG and Cascade ports profile */
        {CHEETAH_XG_PORT_XON_DEFAULT,CHEETAH_XG_PORT_XOFF_DEFAULT,CHEETAH_XG_PORT_RX_BUFF_LIMIT_DEFAULT},
    /* Profile 3 - Set XG and Cascade ports profile */
        {CHEETAH_XG_PORT_XON_DEFAULT,CHEETAH_XG_PORT_XOFF_DEFAULT,CHEETAH_XG_PORT_RX_BUFF_LIMIT_DEFAULT},
    };

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

/* CPSS should config profile 0 and 1. */
    /* Set default settings for Flow Control Profiles: */
    for(profile = CPSS_PORT_RX_FC_PROFILE_1_E ; profile <= CPSS_PORT_RX_FC_PROFILE_4_E ; profile++)
    {
        rc = cpssDxChPortXonLimitSet(dev,profile,buffLimit[profile][0]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortXonLimitSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPortXoffLimitSet(dev,profile,buffLimit[profile][1]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortXoffLimitSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPortRxBufLimitSet(dev,profile,buffLimit[profile][2]);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortRxBufLimitSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    /* set the buffer limit profile association for network ports */
    for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfPorts; port++)
    {
        
        #ifndef __AX_PLATFORM__
    	if(PRV_CPSS_PORT_NOT_EXISTS_E == PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType)
    	{
			continue;
		}
        #endif
        if (PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_NOT_EXISTS_E)
            continue;

        profile = (PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType >= PRV_CPSS_PORT_XG_E) ?
                  CHEETAH_NET_10GE_PORT_PROFILE :
                  CHEETAH_NET_GE_PORT_PROFILE;

        rc = cpssDxChPortRxFcProfileSet(dev, port, profile);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortRxFcProfileSet", rc);
        if (rc != GT_OK)
            return rc;
    }

    /* set the buffer limit profile association for CPU port */
    profile = CHEETAH_CPU_PORT_PROFILE;
    rc = cpssDxChPortRxFcProfileSet(dev,(GT_U8) CPSS_CPU_PORT_NUM_CNS, profile);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortRxFcProfileSet", rc);
    if (rc != GT_OK)
        return rc;

    /* Enable HOL system mode for revision 3 in DxCh2, DxCh3, XCAT. */
    if (PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
    {   
        /* Special for xcat. luoxun */
        rc = cpssDxChPortFcHolSysModeSet(dev, CPSS_DXCH_PORT_HOL_E);
        printf("devFamily(XCAT): set port mode HOL.\n");
    }
    else 
    {
        printf("devFamily(not XCAT): ");
        if(appDemoPpConfigList[dev].flowControlDisable)
        {
            rc = cpssDxChPortFcHolSysModeSet(dev, CPSS_DXCH_PORT_HOL_E);
            printf("set port mode HOL.\n");
        }
        else
        {
            rc = cpssDxChPortFcHolSysModeSet(dev, CPSS_DXCH_PORT_FC_E);
            printf("set port mode FC.\n");
        }
    }
    
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortFcHolSysModeSet", rc);

    return rc;
}

static GT_STATUS prvPortLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initPort == GT_FALSE)
        return GT_OK;

    rc = cpssDxChPortStatInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortStatInit", rc);
    if( GT_OK != rc)
        return rc;

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    rc = dxChPortBufMgInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("dxChPortBufMgInit", rc);
    if( GT_OK != rc)
        return rc;

    rc = cpssDxChPortTxInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortTxInit", rc);
    if(rc != GT_OK)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_BRIDGE
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgStp.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgMc.h>

static GT_STATUS dxChBrgFdbInit
(
    IN  GT_U8                       dev
)
{
    GT_STATUS rc;       /* return code  */
    GT_U8     hwDev;    /* HW device number */

    cpssDxChBrgFdbInit(dev);

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    /* Set lookUp mode and lookup length. */
    rc = cpssDxChBrgFdbHashModeSet(dev, CPSS_MAC_HASH_FUNC_XOR_E);    /* use XOR hash function */
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbHashModeSet", rc);
    if( GT_OK != rc)
        return rc;

    rc = cpssDxChBrgFdbMaxLookupLenSet(dev, 4);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbMaxLookupLenSet", rc);
    if( GT_OK != rc)
        return rc;

    /******************************/
    /* do specific cheetah coding */
    /******************************/

    /* the trunk entries and the multicast entries are registered on device 31
       that is to support the "renumbering" feature , but the next configuration
       should not effect the behavior on other systems that not use a
       renumbering ..
    */
    /* age trunk entries on a device that registered from all device
       since we registered the trunk entries on device 31 (and auto learn set it
       on "own device" */
    /* Set Action Active Device Mask and Action Active Device. This is needed
       in order to enable aging only on own device.  */

    rc = cpssDxChCfgHwDevNumGet(dev, &hwDev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet", rc);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChBrgFdbActionActiveDevSet(dev, hwDev, 0x1F);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionActiveDevSet", rc);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable(dev, GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable", rc);
    if(rc != GT_OK)
        return rc;

    rc = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable(dev, GT_FALSE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable", rc);
    if(rc != GT_OK)
        return rc;

    return GT_OK;
}

static GT_STATUS prvBridgeLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;
    GT_U32      stpEntry[CPSS_DXCH_STG_ENTRY_SIZE_CNS];

    if(libInitParamsPtr->initBridge == GT_FALSE)
        return GT_OK;


    /* Init VLAN */
    rc = cpssDxChBrgVlanInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgVlanInit", rc);
    if( GT_OK != rc)
        return rc;

    /** STP **/
    rc = cpssDxChBrgStpInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgStpInit", rc);
    if( GT_OK != rc)
        return rc;

    rc = dxChBrgFdbInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("dxChBrgFdbInit", rc);
    if( GT_OK != rc)
        return rc;


    rc = cpssDxChBrgMcInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgMcInit", rc);
    if( GT_OK != rc)
        return rc;

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

#if 0 /* these actions are performed during CPSS under PSS Vlan init
 - should we perform them here?*/

    /* both vlan0,vlan1 in the same register */
    maxVlanRange = (GT_U16)corePpDevs[dev]->vlanData->maxNumOfVlans - 1;
    /* max num of VLAN's in Cheetah 0xFFE */
    if(maxVlanRange > GT_CORE_MAX_NUM_VLANS_CNS - 1)
        return GT_FAIL;

        /* config etherType0 */
    etherTypeMode = CPSS_VLAN_ETHERTYPE0_E;
    etherType = 0x8100;

    rc = cpssDxChBrgVlanEtherTypeSet(dev,
                                     etherTypeMode,
                                     etherType,
                                     maxVlanRange);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* config etherType1 */
    etherTypeMode = CPSS_VLAN_ETHERTYPE1_E;

    rc = cpssDxChBrgVlanEtherTypeSet(dev,
                                     etherTypeMode,
                                     etherType,
                                     maxVlanRange);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* This bit indicates that cheetah operates as a 802.1D
       Bridge or as a 802.1Q Bridge
    1 = 802.1D Bridge
    0 = 802.1Q Bridge */

    /* like Salsa */
    brgMode = CPSS_BRG_MODE_802_1Q_E;

    rc = cpssDxChBrgVlanBridgingModeSet(dev, brgMode);
    if(rc != GT_OK)
    {
        return rc;
    }

    /*
       reset word 0 in HW -- invalidate vlan 1
       since the HW start with reset value of valid vlan 1 with all ports
       members in it , we need to invalidate it
    */
    vlanId = 1;

    rc = cpssDxChBrgVlanEntryInvalidate(dev, vlanId);
    return rc;

#endif



    /* set first entry in STP like default entry */
    osMemSet(stpEntry, 0, sizeof(stpEntry));
    rc = cpssDxChBrgStpEntryWrite(dev, 0, stpEntry);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgStpEntryWrite", rc);
    if( GT_OK != rc)
    {
        /* the device not support STP !!! --> it's ok ,we have those ... */
        osPrintf("cpssDxChBrgStpEntryWrite : device[%d] not supported \n",dev);
        rc = GT_OK;
    }

    return GT_OK;
}
#endif


#ifdef IMPL_MIRROR
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>

static GT_STATUS prvMirrorLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;
    GT_U8     hwDev; /* HW device number */
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   interface;

    if(libInitParamsPtr->initMirror == GT_FALSE)
        return GT_OK;

    /*rc = cpssDxChMirrorInit(dev);
    if( GT_OK != rc)
        return rc;*/

    /* The following mirroring settings are needed because several RDE tests */
    /* assume that that default HW values of analyzers is port 0 SW device ID 0. */
    if( 0 == dev )
    {
        rc = cpssDxChCfgHwDevNumGet(0, &hwDev);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCfgHwDevNumGet", rc);
        if (GT_OK != rc)
        {
            return rc;
        }

        if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            rc = cpssDxChMirrorRxAnalyzerPortSet(dev, 0, hwDev);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorRxAnalyzerPortSet", rc);
            if (GT_OK != rc)
            {
                return rc;
            }

            rc = cpssDxChMirrorTxAnalyzerPortSet(dev, 0, hwDev);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorTxAnalyzerPortSet", rc);
        }
        else
        {
            interface.interface.type = CPSS_INTERFACE_PORT_E;
            interface.interface.devPort.devNum = hwDev;
            interface.interface.devPort.portNum = 0;

            rc = cpssDxChMirrorAnalyzerInterfaceSet(dev, 0, &interface);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorAnalyzerInterfaceSet", rc);
            if (GT_OK != rc)
            {
                return rc;
            }

            rc = cpssDxChMirrorAnalyzerInterfaceSet(dev, 1, &interface);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChMirrorAnalyzerInterfaceSet", rc);
        }

        if (GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
}
#endif


#ifdef IMPL_NETIF
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>

static GT_STATUS prvNetIfLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc = GT_OK;
    GT_U32 i;

    if(libInitParamsPtr->initNetworkIf == GT_FALSE)
        return GT_OK;

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }
    if (appDemoPpConfigList[dev].cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_SDMA_E)
    {
        rc = cpssDxChNetIfInit(dev);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfInit", rc);
    }
    else if (appDemoPpConfigList[dev].cpuPortMode == PRV_CPSS_GEN_NETIF_CPU_PORT_MODE_MII_E)
    {
        CPSS_DXCH_NETIF_MII_INIT_STC miiInit;
        miiInit.numOfTxDesc = sysConfigParamsPtr->miiNumOfTxDesc;
        miiInit.txInternalBufBlockSize = sysConfigParamsPtr->miiTxInternalBufBlockSize;
        miiInit.txInternalBufBlockPtr = cpssOsCacheDmaMalloc(miiInit.txInternalBufBlockSize);
        if (miiInit.txInternalBufBlockPtr == NULL)
        {
            return GT_FAIL;
        }
        for (i = 0; i < CPSS_MAX_RX_QUEUE_CNS; i++)
        {
            miiInit.bufferPercentage[i] = sysConfigParamsPtr->miiBufferPercentage[i];
        }
        miiInit.rxBufSize = sysConfigParamsPtr->miiRxBufSize;
        miiInit.headerOffset = sysConfigParamsPtr->miiHeaderOffset;
        miiInit.rxBufBlockSize = sysConfigParamsPtr->miiRxBufBlockSize;
        miiInit.rxBufBlockPtr = cpssOsCacheDmaMalloc(miiInit.rxBufBlockSize);
        if (miiInit.rxBufBlockPtr == NULL)
        {
            return GT_FAIL;
        }
        rc = cpssDxChNetIfMiiInit(dev,&miiInit);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChNetIfMiiInit", rc);
    }


    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif


#ifdef IMPL_PCL
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>

static GT_STATUS prvPclLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initPcl == GT_FALSE)
        return GT_OK;

    rc = cpssDxChPclInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclInit", rc);
    if( GT_OK != rc)
        return rc;

    rc = cpssDxChPclIngressPolicyEnable(dev,GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPclIngressPolicyEnable", rc);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_PHY
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>

static GT_STATUS prvPhyLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initPhy == GT_FALSE)
        return GT_OK;

    rc = cpssDxChPhyPortSmiInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiInit", rc);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif


#ifdef IMPL_POLICER
#include <cpss/dxCh/dxChxGen/policer/cpssDxChPolicer.h>

static GT_STATUS prvPolicerLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if(libInitParamsPtr->initPolicer == GT_FALSE)
    {
        /* check if Ingress stage #1 exists */
        if (PRV_CPSS_DXCH_PP_MAC(dev)->fineTuning.featureInfo.iplrSecondStageSupported
            == GT_TRUE)
        {

            /* Disable Policer Metering on Ingress stage #1 */
            rc = cpssDxCh3PolicerMeteringEnableSet(dev,
                                                   CPSS_DXCH_POLICER_STAGE_INGRESS_1_E,
                                                   GT_FALSE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxCh3PolicerMeteringEnableSet", rc);
            if (rc != GT_OK)
            {
                return rc;
            }

            /* Disable Policer Counting on Ingress stage #1 */
            rc = cpssDxChPolicerCountingModeSet(dev,
                                                CPSS_DXCH_POLICER_STAGE_INGRESS_1_E,
                                                CPSS_DXCH_POLICER_COUNTING_DISABLE_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerCountingModeSet", rc);
            return rc;
        }
        else
        {
            return GT_OK;
        }
    }

    rc = cpssDxChPolicerInit(dev);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPolicerInit", rc);
    if( GT_OK != rc)
        return rc;

    return GT_OK;
}
#endif

#ifdef IMPL_TRUNK
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>

static GT_STATUS prvTrunkLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;
    GT_U8      currMaxTrunks = (GT_U8)sysConfigParamsPtr->numOfTrunks;

    if(libInitParamsPtr->initTrunk == GT_FALSE)
        return GT_OK;

    rc = cpssDxChTrunkInit(dev, (GT_U8)sysConfigParamsPtr->numOfTrunks ,
        CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E);

    while(rc == GT_OUT_OF_RANGE && currMaxTrunks)
    {
        currMaxTrunks--;
        rc = cpssDxChTrunkInit(dev, currMaxTrunks ,
            CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E);
    }

    if((rc != GT_OK) &&
       (currMaxTrunks != (GT_U8)sysConfigParamsPtr->numOfTrunks))
    {
        osPrintf("cpssDxChTrunkInit : device[%d] support [%d] trunks \n",dev,currMaxTrunks);
    }

    return rc;
}
#endif

#ifdef IMPL_IP
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIp.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>

static GT_STATUS prvDxChIpLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS                                       rc;
    GT_U32                                          lpmDBId = 0;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC    indexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    tcamLpmManagerCapcityCfg;
    GT_U32                                          vrId = 0;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC                  vrConfigInfo;
    GT_U32                                          maxNumOfIpv4Prefixes;
    GT_U32                                          maxNumOfIpv6Prefixes;
    CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT               shadowType;
    GT_U32                                          i;
    GT_U32                                          pclIdArray[1];

    static GT_BOOL lpmDbInitialized = GT_FALSE;     /* keeps if LPM DB was created */
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT          *defUcNextHopInfoPtr = NULL;

    /********************************************************************/
    /* if lpm db is already created, all that is needed to do is to add */
    /* the device to the lpm db                                         */
    /********************************************************************/
    if (lpmDbInitialized == GT_TRUE)
    {
        rc = cpssDxChIpLpmDBDevListAdd(lpmDBId, &dev, 1);
        return  rc;
    }

    /*****************/
    /* create LPM DB */
    /*****************/

    /* set parameters */
    maxNumOfIpv4Prefixes = sysConfigParamsPtr->maxNumOfIpv4Prefixes;
    maxNumOfIpv6Prefixes = sysConfigParamsPtr->maxNumOfIpv6Prefixes;
    tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = maxNumOfIpv4Prefixes;
    tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = 0; /* Ch+ doesn't support MC */
    tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = maxNumOfIpv6Prefixes;

    /* The same init function is used for Ch+ and for xCat that is configured
       to work in policy based routing. Note that Ch+ PCL TCAM contains 2 columns
       and search order is column by column. xCat PCL TCAM contains 4 ip rules
       and search order is row by row */
    switch (appDemoPpConfigList[dev].devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH_E:
        shadowType = CPSS_DXCH_IP_TCAM_CHEETAH_SHADOW_E;
        indexesRange.firstIndex = 0;
        indexesRange.lastIndex = (maxNumOfIpv4Prefixes + maxNumOfIpv6Prefixes + 1) / 2 - 1;
        break;
    case CPSS_PP_FAMILY_DXCH_XCAT_E:
    case CPSS_PP_FAMILY_DXCH_XCAT2_E:
        shadowType = CPSS_DXCH_IP_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E;
        indexesRange.firstIndex = 0;
        indexesRange.lastIndex = (maxNumOfIpv4Prefixes + maxNumOfIpv6Prefixes + 3) / 4 - 1;
        break;
    default:
        return GT_NOT_IMPLEMENTED;
    }

    rc = cpssDxChIpLpmDBCreate(lpmDBId,shadowType,
                               CPSS_IP_PROTOCOL_IPV4V6_E,&indexesRange,
                               GT_TRUE,&tcamLpmManagerCapcityCfg,NULL);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmDBCreate", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* mark the lpm db as created */
    lpmDbInitialized = GT_TRUE;

    /* set default MC rules if required, for more detailes,
       refer to cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet */
    if (sysConfigParamsPtr->usePolicyBasedDefaultMc == GT_TRUE)
    {
        /* CPSS enabler has no knowledge about the possible virtual routers
           the LPM DB will support; Therefore CPSS enalber will add only PCL ID
           that correspond to virtual router 0 that is added by CPSS enalber */
        pclIdArray[0] = 0;
        rc = cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet(lpmDBId,1,pclIdArray);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet", rc);
        if (rc != GT_OK)
            return rc;
    }

    /*******************************/
    /* add active device to LPM DB */
    /*******************************/
    rc = cpssDxChIpLpmDBDevListAdd(lpmDBId, &dev, 1);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmDBDevListAdd", rc);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /**********************/
    /* add virtual router */
    /**********************/

    /* set next hop to all 0 and change only values other than 0 */
    /* note that Ch+ doesn't support MC so default MC route entry is not set */
    cpssOsMemSet(&vrConfigInfo,0,sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));
    vrConfigInfo.supportIpv4Mc = GT_FALSE;
    vrConfigInfo.supportIpv6Mc= GT_FALSE;
    vrConfigInfo.supportIpv4Uc = GT_TRUE;
    vrConfigInfo.supportIpv6Uc= GT_TRUE;
    /* ipv4 uc and ipv6 uc default route are the same */
    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            defUcNextHopInfoPtr = &vrConfigInfo.defIpv4UcNextHopInfo;
        }
        else
        {
            defUcNextHopInfoPtr = &vrConfigInfo.defIpv6UcNextHopInfo;
        }
        defUcNextHopInfoPtr->pclIpUcAction.pktCmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        defUcNextHopInfoPtr->pclIpUcAction.mirror.cpuCode = CPSS_NET_FIRST_USER_DEFINED_E;
        defUcNextHopInfoPtr->pclIpUcAction.redirect.redirectCmd = CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E;
        defUcNextHopInfoPtr->pclIpUcAction.vlan.modifyVlan = CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_ALL_E;
        defUcNextHopInfoPtr->pclIpUcAction.ipUcRoute.doIpUcRoute = GT_TRUE;

    }

    rc = cpssDxChIpLpmVirtualRouterAdd(lpmDBId,
                                       vrId,
                                       &vrConfigInfo);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmVirtualRouterAdd", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    return rc;
}

static GT_STATUS prvDxCh2Ch3IpLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS                                       rc = GT_OK;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    cpssLpmDbCapacity;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC    cpssLpmDbRange;
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC                 ucRouteEntry;
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC                 mcRouteEntry;
    CPSS_DXCH_IP_TCAM_SHADOW_TYPE_ENT               shadowType;
    CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT          defUcLttEntry;
    CPSS_DXCH_IP_LTT_ENTRY_STC                      defMcLttEntry;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC                  vrConfigInfo;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC pclTcamCfg;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC routerTcamCfg;
    GT_BOOL                                         isCh2VrSupported;
    GT_U32                                          lpmDbId = 0;
    GT_U32                                          tcamColumns;
    static GT_BOOL lpmDbInitialized = GT_FALSE;     /* traces after LPM DB creation */
    /* init default UC and MC entries */
    cpssOsMemSet(&defUcLttEntry,0,sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    cpssOsMemSet(&defMcLttEntry,0,sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));

    defUcLttEntry.ipLttEntry.ipv6MCGroupScopeLevel    = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
    defUcLttEntry.ipLttEntry.numOfPaths               = 0;
    defUcLttEntry.ipLttEntry.routeEntryBaseIndex      = 0;
    defUcLttEntry.ipLttEntry.routeType                = CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    defUcLttEntry.ipLttEntry.sipSaCheckMismatchEnable = GT_FALSE;
    defUcLttEntry.ipLttEntry.ucRPFCheckEnable         = GT_FALSE;

    defMcLttEntry.ipv6MCGroupScopeLevel    = CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E;
    defMcLttEntry.numOfPaths               = 0;
    defMcLttEntry.routeEntryBaseIndex      = 1;
    defMcLttEntry.routeType                = CPSS_DXCH_IP_ECMP_ROUTE_ENTRY_GROUP_E;
    defMcLttEntry.sipSaCheckMismatchEnable = GT_FALSE;
    defMcLttEntry.ucRPFCheckEnable         = GT_FALSE;


    cpssOsMemSet(&vrConfigInfo,0,sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));

    switch (appDemoPpConfigList[dev].devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        shadowType = CPSS_DXCH_IP_TCAM_CHEETAH2_SHADOW_E;
        tcamColumns = 5;
        break;
    case CPSS_PP_FAMILY_CHEETAH3_E:
        shadowType = CPSS_DXCH_IP_TCAM_CHEETAH3_SHADOW_E;
        tcamColumns = 4;
        break;
    case CPSS_PP_FAMILY_DXCH_XCAT_E:
    case CPSS_PP_FAMILY_DXCH_LION_E:
        shadowType = CPSS_DXCH_IP_TCAM_XCAT_SHADOW_E;
        tcamColumns = 4;
        break;
    default:
        return GT_NOT_IMPLEMENTED;
    }

    if ((shadowType == CPSS_DXCH_IP_TCAM_CHEETAH2_SHADOW_E) &&
        (sysConfigParamsPtr->maxNumOfVirtualRouters > 1))
    {
        isCh2VrSupported = GT_TRUE;
    }
    else
    {
        isCh2VrSupported = GT_FALSE;
    }

    /* define protocolStack */
    if ((sysConfigParamsPtr->lpmDbSupportIpv4 == GT_FALSE) && (sysConfigParamsPtr->lpmDbSupportIpv6 == GT_FALSE))
        return GT_BAD_PARAM;
     if ((sysConfigParamsPtr->lpmDbSupportIpv4 == GT_TRUE) && (sysConfigParamsPtr->lpmDbSupportIpv6 == GT_TRUE))
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
    else
        protocolStack = (sysConfigParamsPtr->lpmDbSupportIpv4 == GT_TRUE) ?
            CPSS_IP_PROTOCOL_IPV4_E : CPSS_IP_PROTOCOL_IPV6_E;

    /* fill vrConfigInfo structure */
    switch(protocolStack)
    {
    case CPSS_IP_PROTOCOL_IPV4_E:
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
        cpssOsMemCpy(&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry,
                     &defUcLttEntry.ipLttEntry,
                     sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        vrConfigInfo.supportIpv6Uc = GT_FALSE;
        vrConfigInfo.supportIpv6Mc = GT_FALSE;
        if(isCh2VrSupported == GT_TRUE)
        {
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
        }
        else
        {
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            cpssOsMemCpy(&vrConfigInfo.defIpv4McRouteLttEntry,
                         &defMcLttEntry,
                         sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }
        break;
    case CPSS_IP_PROTOCOL_IPV6_E:
        vrConfigInfo.supportIpv4Uc = GT_FALSE;
        vrConfigInfo.supportIpv6Uc = GT_TRUE;
        cpssOsMemCpy(&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry,
                     &defUcLttEntry.ipLttEntry,
                     sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        vrConfigInfo.supportIpv4Mc = GT_FALSE;
        if(isCh2VrSupported == GT_TRUE)
        {
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
        }
        else
        {
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            cpssOsMemCpy(&vrConfigInfo.defIpv6McRouteLttEntry,
                         &defMcLttEntry,
                         sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }

        break;
    case CPSS_IP_PROTOCOL_IPV4V6_E:
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
        cpssOsMemCpy(&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry,
                     &defUcLttEntry.ipLttEntry,
                     sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        vrConfigInfo.supportIpv6Uc = GT_TRUE;
        cpssOsMemCpy(&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry,
                     &defUcLttEntry.ipLttEntry,
                     sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        vrConfigInfo.supportIpv4Mc = (isCh2VrSupported == GT_TRUE)? GT_FALSE : GT_TRUE;
        vrConfigInfo.supportIpv6Mc = (isCh2VrSupported == GT_TRUE)? GT_FALSE : GT_TRUE;
        if(isCh2VrSupported == GT_TRUE)
        {
            vrConfigInfo.supportIpv4Mc = GT_FALSE;
            vrConfigInfo.supportIpv6Mc = GT_FALSE;
        }
        else
        {
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            cpssOsMemCpy(&vrConfigInfo.defIpv4McRouteLttEntry,
                         &defMcLttEntry,
                         sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
            cpssOsMemCpy(&vrConfigInfo.defIpv6McRouteLttEntry,
                         &defMcLttEntry,
                         sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }

        break;
    default:
        return GT_BAD_PARAM;
        break;
    }


    if( (vrConfigInfo.supportIpv4Uc == GT_TRUE) || (vrConfigInfo.supportIpv6Uc == GT_TRUE) )
    {
        /************************************************************/
        /* in this case ipv4/ipv6 unicast have common default route */
        /* set route entry 0 as default ipv4/6 unicast              */
        /************************************************************/
        cpssOsMemSet(&ucRouteEntry,0,sizeof(ucRouteEntry));
        ucRouteEntry.type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
        ucRouteEntry.entry.regularEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
        rc = cpssDxChIpUcRouteEntriesWrite(dev, 0, &ucRouteEntry, 1);
        if (rc != GT_OK)
        {
            if(rc == GT_OUT_OF_RANGE)
            {
                /* the device not support any IP (not router device)*/
                rc = GT_OK;

                osPrintf("cpssDxChIpUcRouteEntriesWrite : device[%d] not supported \n",dev);
            }

            return  rc;
        }
    }

    
#ifdef AX_FULL_DEBUG
	osPrintf("prvDxCh2IpLibInit::virtual route supported ? %s\n",isCh2VrSupported ? "true":"false");
#endif
#ifndef __AX_PLATFORM__ /* by qinh@autelan.com 2008-12-12 */
    /*********************************************/
    /* set route entry 0 as default route(0.0.0.0/0) action HARD_DROP */
    /*********************************************/
    cpssOsMemSet(&ucRouteEntry,0,sizeof(ucRouteEntry));
    
    #if 0   /* luoxun -- struct "ucRouteEntry" is different from cpss1.3 */
    ucRouteEntry.cmd = CPSS_PACKET_CMD_DROP_HARD_E;
    ucRouteEntry.appSpecificCpuCodeEnable = GT_TRUE;
    #endif
    ucRouteEntry.entry.regularEntry.cmd = CPSS_PACKET_CMD_DROP_HARD_E;
	ucRouteEntry.entry.regularEntry.appSpecificCpuCodeEnable = GT_TRUE;
    rc = cpssDxChIpUcRouteEntriesWrite(dev, 0, &ucRouteEntry, 1);
    if (rc != GT_OK)
    {
        return  rc;
    }

    /*********************************************/
    /* set route entry 2 as default ipv4 unicast action TRAP_CPU */
    /*********************************************/
    cpssOsMemSet(&ucRouteEntry,0,sizeof(ucRouteEntry));
    
    #if 0   /* luoxun -- struct "ucRouteEntry" is different from cpss1.3 */
    ucRouteEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
	ucRouteEntry.appSpecificCpuCodeEnable = GT_TRUE;
    #endif
    ucRouteEntry.entry.regularEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
	ucRouteEntry.entry.regularEntry.appSpecificCpuCodeEnable = GT_TRUE;
    rc = cpssDxChIpUcRouteEntriesWrite(dev, 2, &ucRouteEntry, 1);
    if (rc != GT_OK)
    {
        return  rc;
    }
#else
    /*********************************************/
    /* set route entry 0 as default ipv4 unicast */
    /*********************************************/
    cpssOsMemSet(&ucRouteEntry,0,sizeof(ucRouteEntry));
    ucRouteEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
	ucRouteEntry.appSpecificCpuCodeEnable = GT_TRUE;
    rc = cpssDxChIpUcRouteEntriesWrite(dev, 0, &ucRouteEntry, 1);
    if (rc != GT_OK)
    {
        return  rc;
    }
#endif
#ifdef AX_FULL_DEBUG
	osPrintf("prvDxCh2IpLibInit::cpssDxChIpUcRouteEntriesWrite ok\n");
#endif

    if (GT_FALSE == isCh2VrSupported)
    {
        /* Ch2 without Vr support case                 */
        if( (vrConfigInfo.supportIpv4Mc == GT_TRUE) || (vrConfigInfo.supportIpv6Mc == GT_TRUE) )
        {
            /*******************************************************/
            /* in this case ipv4/ipv6 mc have common default route */
            /* set route entry 1 as default ipv4/6 multicast       */
            /*******************************************************/
            cpssOsMemSet(&mcRouteEntry,0,sizeof(mcRouteEntry));
            mcRouteEntry.cmd = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            mcRouteEntry.RPFFailCommand = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            rc = cpssDxChIpMcRouteEntriesWrite(dev, 1, &mcRouteEntry);
            if (rc != GT_OK)
            {
                return  rc;
            }
        }
    }
    else
    {
        /* in case of Ch2 with Vr support - no mc support               */
        /* instead we will create another Vr with uc default            */

        /**************************************************************/
        /* in this case ipv4/ipv6 unicast have common default route   */
        /* set route entry 1 as default ipv4/6 unicast for this router*/
        /**************************************************************/
        rc = cpssDxChIpUcRouteEntriesWrite(dev, 1, &ucRouteEntry, 1);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpUcRouteEntriesWrite", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }
    }

    /********************************************************************/
    /* if lpm db is already created, all that is needed to do is to add */
    /* the device to the lpm db                                         */
    /********************************************************************/
    if (lpmDbInitialized == GT_TRUE)
    {
        rc = cpssDxChIpLpmDBDevListAdd(lpmDbId, &dev, 1);
        if(rc == GT_BAD_PARAM)
        {
            osPrintf("cpssDxChIpLpmDBDevListAdd : device[%d] not supported \n",dev);
            /* the device not support the router tcam */
            rc = GT_OK;
        }

        return  rc;
    }

    /*****************/
    /* create LPM DB */
    /*****************/

    /* set parameters */

    cpssLpmDbCapacity.numOfIpv4Prefixes         = sysConfigParamsPtr->maxNumOfIpv4Prefixes;
    cpssLpmDbCapacity.numOfIpv6Prefixes         = sysConfigParamsPtr->maxNumOfIpv6Prefixes;
    cpssLpmDbCapacity.numOfIpv4McSourcePrefixes = sysConfigParamsPtr->maxNumOfIpv4McEntries;
    cpssLpmDbRange.firstIndex                   = sysConfigParamsPtr->lpmDbFirstTcamLine;
    cpssLpmDbRange.lastIndex                    = sysConfigParamsPtr->lpmDbLastTcamLine;

    if ((appDemoPpConfigList[dev].devFamily == CPSS_PP_FAMILY_CHEETAH2_E) && (cpssLpmDbRange.lastIndex > 1023))
    {
        cpssLpmDbRange.lastIndex = 1023;
    }

    /* now create the lpm db */
    if (GT_TRUE == isCh2VrSupported)
    {
        GT_U32 maxNumOfPclEntriesForIpPrefixes = sysConfigParamsPtr->maxNumOfPceForIpPrefixes;

        if (maxNumOfPclEntriesForIpPrefixes < 1)
        {
            maxNumOfPclEntriesForIpPrefixes = 1;
        }

        routerTcamCfg.tcamRange.firstIndex = cpssLpmDbRange.firstIndex;
        routerTcamCfg.tcamRange.lastIndex = cpssLpmDbRange.lastIndex;
        routerTcamCfg.prefixesCfg.numOfIpv4Prefixes = sysConfigParamsPtr->maxNumOfIpv4Prefixes;
        routerTcamCfg.prefixesCfg.numOfIpv6Prefixes = sysConfigParamsPtr->maxNumOfIpv6Prefixes;

        /* take PCL entries from bottom of the table */
        pclTcamCfg.tcamRange.lastIndex = 511;
        pclTcamCfg.tcamRange.firstIndex = pclTcamCfg.tcamRange.lastIndex + 1 -
                                          (maxNumOfPclEntriesForIpPrefixes + 1)/2;

        /* take all the reserved entries */
        pclTcamCfg.prefixesCfg.numOfIpv4Prefixes = maxNumOfPclEntriesForIpPrefixes/2;
        pclTcamCfg.prefixesCfg.numOfIpv6Prefixes = maxNumOfPclEntriesForIpPrefixes -
                                                   pclTcamCfg.prefixesCfg.numOfIpv4Prefixes;

        rc = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDbId,
                                                    protocolStack,
                                                    &pclTcamCfg,
                                                    &routerTcamCfg);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmDBCheetah2VrSupportCreate", rc);
    }
    else
    {
        rc = cpssDxChIpLpmDBCreate(lpmDbId, shadowType,
                                   protocolStack, &cpssLpmDbRange,
                                   sysConfigParamsPtr->lpmDbPartitionEnable,
                                   &cpssLpmDbCapacity,NULL);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmDBCreate", rc);
    }
    if (rc != GT_OK)
    {
        return rc;
    }

    /* mark the lpm db as created */
    lpmDbInitialized = GT_TRUE;

    /*******************************/
    /* add active device to LPM DB */
    /*******************************/
    rc = cpssDxChIpLpmDBDevListAdd(lpmDbId, &dev, 1);
    if (rc != GT_OK)
    {
        if(rc == GT_BAD_PARAM)
        {
            /* the device not support the router tcam */
            osPrintf("cpssDxChIpLpmDBDevListAdd : device[%d] not supported \n",dev);
            rc = GT_OK;
        }

        return  rc;
    }

    /*************************/
    /* create virtual router */
    /*************************/
    if (GT_TRUE == isCh2VrSupported)
    {
        rc = cpssDxChIpLpmVirtualRouterAdd(lpmDbId,
                                           0,
                                           &vrConfigInfo);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmVirtualRouterAdd", rc);
        if (rc != GT_OK)
        {
            return  rc;
        }
        vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex = 1;
        vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex = 1;

        /*****************************************/
        /* This the Ch2 with Vr support case, so */
        /* create another virtual router in PCL  */
        /*****************************************/
        rc = cpssDxChIpLpmVirtualRouterAdd(lpmDbId,
                                           1,
                                           &vrConfigInfo);
    }
    else
    {
        rc = cpssDxChIpLpmVirtualRouterAdd(lpmDbId,
                                           0,
                                           &vrConfigInfo);
    }
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChIpLpmVirtualRouterAdd", rc);
    return rc;
}


static GT_STATUS prvIpLibInit
(
    IN  GT_U8                       dev,
    IN  APP_DEMO_LIB_INIT_PARAMS    *libInitParamsPtr,
    IN  CPSS_PP_CONFIG_INIT_STC     *sysConfigParamsPtr
)
{
    GT_STATUS rc;

    if ((libInitParamsPtr->initIpv4 == GT_FALSE) && (libInitParamsPtr->initIpv6 == GT_FALSE))
    {
        return GT_OK;
    }

    if(appDemoOnDistributedSimAsicSide)
    {
        return GT_OK;
    }

    switch (appDemoPpConfigList[dev].devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH_E:
    case CPSS_PP_FAMILY_DXCH_XCAT2_E:
        rc = prvDxChIpLibInit(dev,libInitParamsPtr,sysConfigParamsPtr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvDxChIpLibInit", rc);
        break;

    case CPSS_PP_FAMILY_CHEETAH2_E:
    case CPSS_PP_FAMILY_CHEETAH3_E:
        rc = prvDxCh2Ch3IpLibInit(dev,libInitParamsPtr,sysConfigParamsPtr);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvDxCh2Ch3IpLibInit", rc);
        break;

    case CPSS_PP_FAMILY_DXCH_XCAT_E:
    case CPSS_PP_FAMILY_DXCH_LION_E:
        /* if policy based routing, use same initialization as Ch+,
           if not use same initialization as Ch2,Ch3 */
        if (sysConfigParamsPtr->usePolicyBasedRouting == GT_TRUE)
        {
            rc = prvDxChIpLibInit(dev,libInitParamsPtr,sysConfigParamsPtr);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvDxChIpLibInit", rc);
        }

        else
        {
            rc = prvDxCh2Ch3IpLibInit(dev,libInitParamsPtr,sysConfigParamsPtr);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("prvDxCh2Ch3IpLibInit", rc);
        }
        break;

    default:
        rc = GT_NOT_IMPLEMENTED;
    }

    return rc;
}

#endif


/*******************************************************************************
* prvFdbActionDelete
*
* DESCRIPTION:
*       Deletes all addresses from FDB table.
*
*
* INPUTS:
*       dev            - physical device number
*
* OUTPUTS:
*       actDevPtr      - pointer to old action device number
*       actDevMaskPtr  - pointer to old action device mask
*       triggerModePtr - pointer to old trigger mode
*       modePtr        - pointer to old action mode
*
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvFdbActionDelete
(
    IN  GT_U8                       dev,
    OUT GT_U32                      *actDevPtr,
    OUT GT_U32                      *actDevMaskPtr,
    OUT CPSS_MAC_ACTION_MODE_ENT    *triggerModePtr,
    OUT CPSS_FDB_ACTION_MODE_ENT    *modePtr
)
{
    GT_STATUS rc;


    /* save Mac Action action mode */
    rc = cpssDxChBrgFdbActionModeGet(dev, modePtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionModeGet", rc);
    if (rc != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* save Mac Action trigger mode */
    rc = cpssDxChBrgFdbMacTriggerModeGet(dev, triggerModePtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbMacTriggerModeGet", rc);
    if (rc != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* save mac action device parameters */
    rc = cpssDxChBrgFdbActionActiveDevGet(dev,actDevPtr,actDevMaskPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionActiveDevGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* disable actions to be done on the entire MAC table
       before change active configuration */
    rc = cpssDxChBrgFdbActionsEnableSet(dev, GT_FALSE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionsEnableSet", rc);
    if (rc != GT_OK)
    {
        return GT_HW_ERROR;
    }


    /* delete all entries regardless of device number */
    rc = cpssDxChBrgFdbActionActiveDevSet(dev,0, 0);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionActiveDevSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* enable actions, action mode, triggered mode, trigger the action */
    rc = cpssDxChBrgFdbTrigActionStart(dev, CPSS_FDB_ACTION_DELETING_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbTrigActionStart", rc);

    return rc;
}


/*******************************************************************************
* appDemoDxChFdbFlush
*
* DESCRIPTION:
*       Deletes all addresses from FDB table.
*
*
* INPUTS:
*       dev            - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDxChFdbFlush
(
    IN GT_U8   dev
)
{
    GT_STATUS                 rc;
    GT_BOOL                   actionCompleted;      /* Action Trigger Flag         */
    GT_U32                    loopCounter;          /* Counter for busy wait loops */
    /* old value of AA TA messages to CPU enabler */
    GT_BOOL                   aaTaMessegesToCpuEnable;
    GT_U32                    actDev;               /* old action device number */
    GT_U32                    actDevMask;           /* old action device mask */
    CPSS_MAC_ACTION_MODE_ENT  triggerMode;          /* old trigger mode */
    CPSS_FDB_ACTION_MODE_ENT  mode;                 /* old action mode */


    /* enable delete of static entries */
    rc = cpssDxChBrgFdbStaticDelEnable(dev, GT_TRUE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbStaticDelEnable", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* save old value of AA TA messages to CPU enabler */
    rc = cpssDxChBrgFdbAAandTAToCpuGet(dev, &aaTaMessegesToCpuEnable);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAAandTAToCpuGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(aaTaMessegesToCpuEnable == GT_TRUE)
    {


        /* disable AA and TA messages to CPU */
        rc = cpssDxChBrgFdbAAandTAToCpuSet(dev, GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAAandTAToCpuSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

    }

    rc = prvFdbActionDelete(dev, &actDev, &actDevMask, &triggerMode, &mode);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("prvFdbActionDelete", rc);

    if(rc != GT_OK)
    {
        return rc;
    }

    actionCompleted = GT_FALSE;
    loopCounter = 0;

    /* busy wait for Action Trigger Status */
    while (!actionCompleted)
    {
        rc = cpssDxChBrgFdbTrigActionStatusGet(dev, &actionCompleted);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbTrigActionStatusGet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        loopCounter ++;

        /* check if loop counter has reached 100000000 */
        if (loopCounter >= 100000000)
        {
            return GT_TIMEOUT;
        }

    }

    if(aaTaMessegesToCpuEnable == GT_TRUE)
    {
        /* restore AA and TA messages to CPU */
        rc = cpssDxChBrgFdbAAandTAToCpuSet(dev, aaTaMessegesToCpuEnable);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbAAandTAToCpuSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

    }


    /* restore active device parameters */
    rc = cpssDxChBrgFdbActionActiveDevSet(dev,
                                          actDev,
                                          actDevMask);

    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbActionActiveDevSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* disable delete of static entries */
    rc = cpssDxChBrgFdbStaticDelEnable(dev, GT_FALSE);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbStaticDelEnable", rc);
    if (rc != GT_OK)
    {
        return rc;
    }


    /* restore Trigger mode - it should be last operation because
       it may start auto aging. All active config should be restored
       before start of auto aging */
    rc = cpssDxChBrgFdbMacTriggerModeSet(dev, triggerMode);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbMacTriggerModeSet", rc);

    return rc;

}


/*******************************************************************************
* cpuTx
*
* DESCRIPTION:
*       Local function used by workaround for SGMII 2.5GB to send packet from CPU
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum     - physical device number
*       portsNum   - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_NO_RESOURCE  - memory resource not available.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS cpuTx(GT_U8 port, GT_BOOL sync)
{
    CPSS_DXCH_NET_TX_PARAMS_STC pcktParams;
    GT_U8                       *buffList[1];
    GT_U32                      buffLenList[1];
    GT_U32                      numOfBufs;
    CPSS_UNI_EV_CAUSE_ENT       uniEventArr[1];
    GT_U32                      txEventHndl;
    GT_U32                      evBitmapArr[8];
    GT_STATUS                   rc;

    uniEventArr[0] = CPSS_PP_TX_BUFFER_QUEUE_E;
    cpssEventBind(uniEventArr, 1, &txEventHndl);

    osMemSet(&pcktParams, 0, sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
    pcktParams.sdmaInfo.txQueue = 0;
    pcktParams.sdmaInfo.recalcCrc = 1;
    pcktParams.sdmaInfo.invokeTxBufferQueueEvent = GT_FALSE;
    pcktParams.sdmaInfo.evReqHndl = txEventHndl;
    pcktParams.packetIsTagged = GT_FALSE;
    pcktParams.dsaParam.dsaType = CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E;
    /* use here HW device number! */
    pcktParams.dsaParam.dsaInfo.fromCpu.srcDev = PRV_CPSS_HW_DEV_NUM_MAC(0);
    pcktParams.dsaParam.dsaInfo.fromCpu.cascadeControl = GT_FALSE;
    pcktParams.dsaParam.dsaInfo.fromCpu.tc = 0;
    pcktParams.dsaParam.dsaInfo.fromCpu.dp = 0;
    pcktParams.dsaParam.dsaInfo.fromCpu.egrFilterEn = GT_FALSE;
    pcktParams.dsaParam.dsaInfo.fromCpu.egrFilterRegistered = GT_FALSE;
    pcktParams.dsaParam.dsaInfo.fromCpu.srcId = 0;
    pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_PORT_E;
    pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = PRV_CPSS_HW_DEV_NUM_MAC(0);
    pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = port;
    pcktParams.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = 1;
    pcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged = GT_FALSE;
    pcktParams.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.mailBoxToNeighborCPU = GT_FALSE;
    pcktParams.dsaParam.commonParams.cfiBit = 0;
    pcktParams.dsaParam.commonParams.vid = 1;
    pcktParams.dsaParam.commonParams.vpt = 0;
    pcktParams.dsaParam.commonParams.dsaTagType = CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E;
    pcktParams.dsaParam.commonParams.dropOnSource = GT_FALSE;
    pcktParams.dsaParam.commonParams.packetIsLooped = GT_FALSE;

    numOfBufs = 1;
    buffLenList[0] = 64;
    buffList[0] = osMalloc(buffLenList[0]*sizeof(GT_U8));
    buffList[0][0] = 0;
    buffList[0][1] = 0x0;
    buffList[0][2] = 0x22;
    buffList[0][3] = 0x22;
    buffList[0][4] = 0x22;
    buffList[0][5] = 0x22;
    buffList[0][6] = 0;
    buffList[0][7] = 0x0;
    buffList[0][8] = 0x0;
    buffList[0][9] = 0x0;
    buffList[0][10] = 0x0;
    buffList[0][11] = 0x2;
    rc = cpssDxChNetIfSdmaTxQueueEnable(0, 0, GT_TRUE);

    if(sync == GT_TRUE)
    {
        rc = cpssDxChNetIfSdmaSyncTxPacketSend(0,&pcktParams,buffList, buffLenList, numOfBufs);
    }
    else
    {
        rc = cpssDxChNetIfSdmaTxPacketSend(0,&pcktParams,buffList, buffLenList, numOfBufs);
        if(rc != GT_OK)
        {
            osFree(buffList[0]);
            return rc;
        }

        rc = cpssEventSelect(txEventHndl, NULL, evBitmapArr, 8);
        if(rc != GT_OK)
        {
            osFree(buffList[0]);
            return rc;
        }
    }

    osFree(buffList[0]);

    return rc;
}

/*******************************************************************************
* appDemoDxChXcatSgmii2_5GbNetworkPortWa
*
* DESCRIPTION:
*       The workaround for SGMII 2.5GB
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum     - physical device number
*       portsBmp   - physical ports bitmap
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_NO_RESOURCE  - memory resource not available.
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS appDemoDxChXcatSgmii2_5GbNetworkPortWa
(
    GT_U8   devNum,
    GT_U32  portsBmp
)
{
    GT_STATUS   rc = GT_OK;               /* return status */
    GT_U8       port;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    /* configure SGMII 1G */
    for (port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
    {
        if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_NOT_EXISTS_E)
            continue;

        if((portsBmp & (1<<port)) == 0)
            continue;

        rc = cpssDxChPortInterfaceModeSet(devNum, port, CPSS_PORT_INTERFACE_MODE_SGMII_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortSpeedAutoNegEnableSet(devNum, port, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortSpeedSet(devNum, port, CPSS_PORT_SPEED_1000_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortDuplexAutoNegEnableSet(devNum, port, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortDuplexModeSet(devNum, port, CPSS_PORT_FULL_DUPLEX_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortFlowCntrlAutoNegEnableSet(devNum, port, GT_FALSE, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortFlowControlEnableSet(devNum, port, CPSS_PORT_FLOW_CONTROL_DISABLE_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortSerdesPowerStatusSet(devNum, port,
                                              CPSS_PORT_DIRECTION_BOTH_E,
                                              0x1, GT_TRUE);
        if (rc!= GT_OK)
        {
            return rc;
        }

        /* force link up to cause packet to pass whole eggress pipe */
        if((rc = cpssDxChPortForceLinkPassEnableSet(devNum,port,GT_TRUE)) != GT_OK)
            return rc;

        rc = cpuTx(port, GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

    }

    /* configure 2.5G */
    for (port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
    {
        if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[port].portType ==
             PRV_CPSS_PORT_NOT_EXISTS_E)
            continue;

        if((portsBmp & (1<<port)) == 0)
            continue;

        rc = cpssDxChPortInterfaceModeSet(devNum, port, CPSS_PORT_INTERFACE_MODE_SGMII_E);
        if(rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortSpeedSet(devNum, port, CPSS_PORT_SPEED_2500_E);
        if (rc!= GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortSerdesPowerStatusSet(devNum, port,
                                              CPSS_PORT_DIRECTION_BOTH_E,
                                              0x1, GT_TRUE);
        if (rc!= GT_OK)
        {
            return rc;
        }

        if((rc = cpssDxChPortForceLinkPassEnableSet(devNum,port,GT_TRUE)) != GT_OK)
            return rc;
    }

    return rc;
}


/* Definition of lib init functions. */
static FUNCP_CPSS_MODULE_INIT cpssInitList[] = {

#ifdef IMPL_PORT
   prvPortLibInit,
#endif

#ifdef IMPL_PHY
   prvPhyLibInit,
#endif

#ifdef IMPL_BRIDGE
   prvBridgeLibInit,
#endif

#if 1   /*xcat debug, for DMA failure. 20110119, zhangdi change to 1*/
#ifdef IMPL_NETIF
   prvNetIfLibInit,
#endif
#endif

#ifdef IMPL_MIRROR
   prvMirrorLibInit,
#endif

#ifdef IMPL_PCL
   prvPclLibInit,
#endif

#ifdef IMPL_POLICER
   prvPolicerLibInit,
#endif

#ifdef IMPL_TRUNK
    prvTrunkLibInit,
#endif

#ifdef IMPL_IP
    prvIpLibInit,
#endif

    0
};

