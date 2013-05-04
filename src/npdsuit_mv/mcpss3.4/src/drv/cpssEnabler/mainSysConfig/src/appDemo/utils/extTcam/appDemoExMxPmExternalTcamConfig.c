/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
********************************************************************************
* appDemoExMxPmExternalTcamConfig.c
*
* DESCRIPTION:
*       App demo ExMxPm External TCAM configuration API.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <appDemo/utils/extTcam/appDemoExMxPmExternalTcamConfig.h>
#include <appDemo/utils/extTcam/appDemoExMxPmExternalTcamBinding.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <cpss/exMxPm/exMxPmGen/config/private/prvCpssExMxPmInfo.h>
#include <gtExtDrv/drivers/gtTwsiHwCtrl.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>

/* the array where the values to overwrite the defaults when external TCAM */
/* management initialize are saved.                                        */
static APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC overwrittenDefaultArr[APP_DEMO_EXTERNAL_TCAM_MAX_DEFALT_RULE_NUMS_OVERWRITTEN_MAC];


APP_DEMO_EXTERNAL_TCAM_NUM_OF_RULES_STC appDemoNumOfRulesTbl[APP_DEMO_EXTERNAL_TCAM_CLIENT_KEY_COMB_MAC*2+1] =
{
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E,            0, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E,            1, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E,            2, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E,            3, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     0, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     1, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     2, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     3, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     4, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E,  0 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     5, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     6, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     7, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E,  0 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  0, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  1, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  2, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  3, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  4, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  5, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  6, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  7, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  8, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  9, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 10, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 11, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 12, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 13, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 14, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 15, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  0, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  1, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  2, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  3, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  4, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  5, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  6, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  7, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  8, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  9, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 10, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 11, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 12, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 13, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 14, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {0, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 15, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E,            0, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E,            1, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E,            2, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E,            3, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     0, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     1, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     2, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     3, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     4, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E,  0 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     5, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     6, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E,     7, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E,  0 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  0, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  1, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  2, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  3, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  4, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  5, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  6, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  7, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  8, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E,  9, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 10, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 11, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 12, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 13, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 14, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E, 15, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  0, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  1, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  2, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  3, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  4, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  5, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  6, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  7, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  8, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E,  9, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 10, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 11, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 12, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 13, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 14, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 },
    {1, CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E, 15, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E, 16 },
    {APP_DEMO_EXTERNAL_TCAM_EOT_MAC,
        CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E,            0, CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E,  0 }
};

GT_U32 appDemoExternalTcamNumOflookups = 2;
APP_DEMO_EXTERNAL_TCAM_NUM_OF_RULES_STC *appDemoNumOfRulesTblPtr = appDemoNumOfRulesTbl;

/*******************************************************************************
* appDemoExMxPmExternalTcamMngInit
*
* DESCRIPTION:
*       Initialize the external TCAM management structure.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       logicalConfigParamsPtr  -   (pointer to) PP logical configuration parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function allocate the memory for the external TCAM management
*       database and start to initialize it.
*       It calculates the number of lines needed to be allocated from the
*       control memory and used for actions.
*       This function must be called prior to call to appDemoDxPpLogicalInit
*       since it initializes external TCAM relevant parameters in
*       logicalConfigParamsPtr pointed structure.
*       The function does not configure HW -> only the output structure.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamMngInit
(
    IN  GT_U8                        devNum,
    OUT  CPSS_PP_CONFIG_INIT_STC     *logicalConfigParamsPtr
)
{
    GT_STATUS   rc;     /* return code         */
    GT_U32 i;           /* table index         */
    GT_U32 numOfRules;  /* requested num rules */
    GT_BOOL validNum;   /* validate the requested num */
    GT_U32 ruleIndexInMngDb;  /* rule index in external TCAM management database */

    /* binding the external TCAM glue layer functions to the CPSS callbacks */
    logicalConfigParamsPtr->externalTcamInfo.extTcamRuleWriteFuncPtr =
        (void *)appDemoExMxPmExternalTcamRuleWrite;
    logicalConfigParamsPtr->externalTcamInfo.extTcamRuleReadFuncPtr =
        (void *)appDemoExMxPmExternalTcamRuleRead;
    logicalConfigParamsPtr->externalTcamInfo.extTcamActionWriteFuncPtr =
        (void *)appDemoExMxPmExternalTcamActionWrite;
    logicalConfigParamsPtr->externalTcamInfo.extTcamActionReadFuncPtr =
        (void *)appDemoExMxPmExternalTcamActionRead;
    logicalConfigParamsPtr->externalTcamInfo.actionLinesInMemoryControl0 = 0;
    logicalConfigParamsPtr->externalTcamInfo.actionLinesInMemoryControl1 = 0;

    /* allocating the external TCAM management database */
    appDemoPpConfigList[devNum].extTcamMngPtr =
        osMalloc(sizeof(APP_DEMO_EXTERNAL_TCAM_KEY_LOOKUP_STC)*
                 appDemoExternalTcamNumOflookups * APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC);

    if(appDemoPpConfigList[devNum].extTcamMngPtr == NULL)
        return GT_OUT_OF_CPU_MEM;

    osMemSet(appDemoPpConfigList[devNum].extTcamMngPtr, 0,
                    sizeof(APP_DEMO_EXTERNAL_TCAM_KEY_LOOKUP_STC)*
                    appDemoExternalTcamNumOflookups * APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC);

    /* initializing the external TCAM management database */

    i = 0;
    /* initializing maxRuleNum fields */
    while(appDemoNumOfRulesTblPtr[i].lookupIndex != APP_DEMO_EXTERNAL_TCAM_EOT_MAC)
    {
         rc = appDemoExMxPmExternalTcamClientKeyLookupGet(devNum,
                                   appDemoNumOfRulesTblPtr[i].tcamClient,
                                   appDemoNumOfRulesTblPtr[i].clientKeyType,
                                   appDemoNumOfRulesTblPtr[i].lookupIndex,
                                   &numOfRules, &validNum);
         CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoExMxPmExternalTcamClientKeyLookupGet", rc);
         if(rc != GT_OK)
             return rc;

         if(validNum == GT_FALSE)
             numOfRules = appDemoNumOfRulesTblPtr[i].numOfRules;

         APP_DEMO_EXTERNAL_TCAM_GET_RULE_SIZE_INDEX_MAC(appDemoNumOfRulesTblPtr[i].ruleSize,
                                                        ruleIndexInMngDb);

         appDemoPpConfigList[devNum].extTcamMngPtr[ruleIndexInMngDb+
             appDemoNumOfRulesTblPtr[i].lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC].maxRuleNum
               += numOfRules;

         i++;
    }

    /* initializing actionStartIndex fields */
    appDemoPpConfigList[devNum].extTcamMngPtr[0].actionStartIndex = 0;
    for( i = 1;
         i < appDemoExternalTcamNumOflookups * APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC;
         i++)
    {
         appDemoPpConfigList[devNum].extTcamMngPtr[i].actionStartIndex =
             appDemoPpConfigList[devNum].extTcamMngPtr[i-1].actionStartIndex +
             appDemoPpConfigList[devNum].extTcamMngPtr[i-1].maxRuleNum *
                                APP_DEMO_EXTERNAL_TACM_LINES_PER_ACTION_MAC;
    }
    /* computing the required number of line in control memory for actions */
    logicalConfigParamsPtr->externalTcamInfo.actionLinesInMemoryControl0 =
             appDemoPpConfigList[devNum].extTcamMngPtr[i-1].actionStartIndex +
             appDemoPpConfigList[devNum].extTcamMngPtr[i-1].maxRuleNum *
                                APP_DEMO_EXTERNAL_TACM_LINES_PER_ACTION_MAC;

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamRuleIndexByLenAndLookupGet
*
* DESCRIPTION:
*       The function gets the rule start index and the maximum number of
*       rules that describe the external TCAM rule area appropriate to
*       a rule size and lookup number combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       ruleSize            - size of rule entry.
*       lookupIndex         - the number of lookup.
*
* OUTPUTS:
*       ruleStartIndexPtr   - (pointer to) rule start index number of the area.
*       maxRuleNumPtr       - (pointer to) the maximal number of rules that can
*                             be inserted into the area.
*       linesPerRulePtr     - (pointer to) how many external TCAM lines a rule
*                             of ruleSize type consumes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamRuleIndexByLenAndLookupGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      lookupIndex,
    OUT GT_U32                                      *ruleStartIndexPtr,
    OUT GT_U32                                      *maxRuleNumPtr,
    OUT GT_U32                                      *linesPerRulePtr
)
{
    GT_U32  ruleMngIndex;  /* rule index used in the management database */

    CPSS_NULL_PTR_CHECK_MAC(ruleStartIndexPtr);
    CPSS_NULL_PTR_CHECK_MAC(maxRuleNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(linesPerRulePtr);

    APP_DEMO_EXTERNAL_TCAM_GET_RULE_SIZE_INDEX_MAC(ruleSize, ruleMngIndex);

    if(lookupIndex >= appDemoExternalTcamNumOflookups)
        return GT_BAD_PARAM;

    *ruleStartIndexPtr =
       appDemoPpConfigList[devNum].extTcamMngPtr
          [lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC+ruleMngIndex].ruleStartIndex;

    *maxRuleNumPtr =
       appDemoPpConfigList[devNum].extTcamMngPtr
          [lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC+ruleMngIndex].maxRuleNum;

    switch(ruleSize)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E: *linesPerRulePtr = 2;
                                                        break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E: *linesPerRulePtr = 4;
                                                        break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E: *linesPerRulePtr = 8;
                                                        break;
        default: return GT_BAD_PARAM;
                 break;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamActionIndexGet
*
* DESCRIPTION:
*       The function gets the line in control memory area allocated for external
*       TCAM actions for a defined rule line in external TCAM.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       ruleSize            - size of rule entry.
*       lookupIndex         - the number of lookup.
*       ruleStartIndex      - rule start index.
*
* OUTPUTS:
*       actionStartIndexPtr - (pointer to) action start index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamActionIndexGet
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      lookupIndex,
    IN  GT_U32                                      ruleStartIndex,
    OUT GT_U32                                      *actionStartIndexPtr
)
{
    GT_U32  ruleMngIndex;       /* rule index used in the management database */
    GT_U32  ruleStartAreaIndex; /* the start index of these rules area        */
    GT_U32  maxRulesInArea;     /* the maximal number of rules in the area    */
    GT_U32  linesPerRule;       /* the number of TCAM lines per rule          */

    CPSS_NULL_PTR_CHECK_MAC(actionStartIndexPtr);

    APP_DEMO_EXTERNAL_TCAM_GET_RULE_SIZE_INDEX_MAC(ruleSize, ruleMngIndex);

    if(lookupIndex >= appDemoExternalTcamNumOflookups)
        return GT_BAD_PARAM;

    ruleStartAreaIndex =
       appDemoPpConfigList[devNum].extTcamMngPtr
         [lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC+ruleMngIndex].ruleStartIndex;

    maxRulesInArea =
       appDemoPpConfigList[devNum].extTcamMngPtr
         [lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC+ruleMngIndex].maxRuleNum;

    switch(ruleSize)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E: linesPerRule = 2;
                                                        break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E: linesPerRule = 4;
                                                        break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E: linesPerRule = 8;
                                                        break;
        default: return GT_BAD_PARAM;
                 break;
    }

    if(ruleStartIndex < ruleStartAreaIndex ||
       ruleStartIndex >= ruleStartAreaIndex + maxRulesInArea*linesPerRule)
        return GT_BAD_PARAM;

    *actionStartIndexPtr =
       appDemoPpConfigList[devNum].extTcamMngPtr
        [lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC+ruleMngIndex].actionStartIndex
        + (ruleStartIndex - ruleStartAreaIndex) *
            APP_DEMO_EXTERNAL_TACM_LINES_PER_ACTION_MAC/linesPerRule;

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupSet
*
* DESCRIPTION:
*       This function sets the value of a number of rules requested for a
*       device, client, key and lookup combination later to be used and
*       overwrite the default system value.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       tcamClient          - EXMXPM device External TCAM unit client.
*       clientKeyType       - The selected key type enumeration per client:
*                             please refer to Functional Spec for key types number.
*       lookupIndex         - The number of lookup.
*       requestedRulesNum   - the number of rules requested (will overwrite the
*                             default value).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place to save the request
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to effect the system external TCAM configuration this
*       function must be called before cpssInitSystem!!!
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamClientKeyLookupSet
(
  IN GT_U8                                 devNum,
  IN CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT  tcamClient,
  IN GT_U32                                clientKeyType,
  IN GT_U32                                lookupIndex,
  IN GT_U32                                requestedRulesNum
)
{
    GT_U32 i;

    switch(tcamClient)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E:
        case CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E:
        case CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E:
        case CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E:
                                                                break;
        default: return GT_BAD_PARAM;
                 break;
    }

    if(clientKeyType > 15 || lookupIndex >= appDemoExternalTcamNumOflookups)
        return GT_BAD_PARAM;

    for( i = 0 ; i < APP_DEMO_EXTERNAL_TCAM_MAX_DEFALT_RULE_NUMS_OVERWRITTEN_MAC ; i++ )
    {
        if(overwrittenDefaultArr[i].inUse == GT_FALSE)
        {
            overwrittenDefaultArr[i].devNum             = devNum;
            overwrittenDefaultArr[i].tcamClient         = tcamClient;
            overwrittenDefaultArr[i].clientKeyType      = clientKeyType;
            overwrittenDefaultArr[i].lookupIndex        = lookupIndex;
            overwrittenDefaultArr[i].requestedRulesNum  = requestedRulesNum;
            overwrittenDefaultArr[i].inUse              = GT_TRUE;
            return GT_OK;
        }
    }

    return GT_NO_RESOURCE;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupGet
*
* DESCRIPTION:
*       This function gets the value of a number of rules requested for a
*       device, client, key and lookup combination later to be used and
*       overwrite the default system value.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum                  - device number
*       tcamClient              - EXMXPM device External TCAM unit client.
*       clientKeyType           - The selected key type enumeration per client:
*                                 please refer to Functional Spec for key types number.
*       lookupIndex             - The number of lookup.
*
* OUTPUTS:
*       requestedRulesNumPtr    - (pointer to) the number of rules requested.
*       validPtr                - (pointer to) indication whether a match to the
*                                 device, client, key and lookup was found.
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamClientKeyLookupGet
(
    IN GT_U8                                 devNum,
    IN CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_ENT  tcamClient,
    IN GT_U32                                clientKeyType,
    IN GT_U32                                lookupIndex,
    OUT GT_U32                               *requestedRulesNumPtr,
    OUT GT_BOOL                              *validPtr
)
{
    GT_U32 i;

    CPSS_NULL_PTR_CHECK_MAC(requestedRulesNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);

    *validPtr = GT_FALSE;

    switch(tcamClient)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_TTI_E:
        case CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_EGRESS_PCL_E:
        case CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_1_E:
        case CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_INGRESS_PCL_2_E:
                                                                break;
        default: return GT_BAD_PARAM;
                 break;
    }

    if(clientKeyType > 15 || lookupIndex >= appDemoExternalTcamNumOflookups)
        return GT_BAD_PARAM;

    for( i = 0 ; i < APP_DEMO_EXTERNAL_TCAM_MAX_DEFALT_RULE_NUMS_OVERWRITTEN_MAC ; i++ )
    {
        if(overwrittenDefaultArr[i].inUse == GT_TRUE)
        {
            if (overwrittenDefaultArr[i].devNum == devNum &&
                overwrittenDefaultArr[i].tcamClient == tcamClient &&
                overwrittenDefaultArr[i].clientKeyType == clientKeyType &&
                overwrittenDefaultArr[i].lookupIndex == lookupIndex)
            {
                *requestedRulesNumPtr = overwrittenDefaultArr[i].requestedRulesNum;
                *validPtr = GT_TRUE;

                return GT_OK;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupSetByIndex
*
* DESCRIPTION:
*       This function sets an entry (specified by index) in the database
*       of values to overwrite the default of maximum number of rules for
*       device, client, key and lookup combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       entryIndex  - entry index in database.
*       entryPtr    - (pointer to) database entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamClientKeyLookupSetByIndex
(
    IN GT_U32                                           entryIndex,
    IN APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC   *entryPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    if(entryIndex >= APP_DEMO_EXTERNAL_TCAM_MAX_DEFALT_RULE_NUMS_OVERWRITTEN_MAC)
        return GT_BAD_PARAM;

    overwrittenDefaultArr[entryIndex] = *entryPtr;
    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamClientKeyLookupGetByIndex
*
* DESCRIPTION:
*       This function gets an entry (specified by index) from the database
*       of values to overwrite the default of maximum number of rules for
*       device, client, key and lookup combination.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       entryIndex  - entry index in database.
*
* OUTPUTS:
*       entryPtr    - (pointer to) database entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamClientKeyLookupGetByIndex
(
    IN GT_U32                                           entryIndex,
    OUT APP_DEMO_EXTERNAL_TCAM_OVERWRITTEN_DEFAULT_STC  *entryPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    if(entryIndex >= APP_DEMO_EXTERNAL_TCAM_MAX_DEFALT_RULE_NUMS_OVERWRITTEN_MAC)
        return GT_BAD_PARAM;

    *entryPtr = overwrittenDefaultArr[entryIndex];
    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamNlreset
*
* DESCRIPTION:
*       This function implements the Netlogic TCAM workaroud.
*       It includes TWSI init and usage, NL 100 NOPs requirements and
*       several ETC configurations due to TCAM.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place for all rules in external TCAM
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS appDemoExMxPmExternalTcamNlReset
(
    IN  GT_U8                        devNum
)
{
    GT_STATUS rc;
    GT_U32 regAddr;
    GT_U8   twsiWriteBuffer[2];
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOP_PARAMS_STC   loopParams;

    rc = hwIfTwsiInitDriver();
    if (rc != GT_OK)
    {
        return rc;
    }

    /* non valid transaction to stabilize DBUS */
    regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->ETC.MGClient.TCAMMGClientCommand;
    rc = prvCpssDrvHwPpWriteRegister(devNum, regAddr, 0x100403);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* Reset the TCAM via the TWSI */
    twsiWriteBuffer[0] = 0x4;
    twsiWriteBuffer[1] = 0x9d;
    rc = hwIfTwsiWriteData(0x10, twsiWriteBuffer, 2);
    if (rc != GT_OK)
    {
        return rc;
    }

    osTimerWkAfter(3000);

    /* Reset the Pup.*/
    regAddr = PRV_CPSS_EXMXPM_PP_MAC(devNum)->regsAddr.ETC.externalTCAMCtrl;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 27, 1, 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Extended TCAM control1 - Remove Mask From Hit */
    regAddr = PRV_CPSS_EXMXPM_PP_MAC(devNum)->regsAddr.ETC.externalTCAMCtrl1;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 2, 1, 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* TCAM control - Reload Configuration */
    regAddr = PRV_CPSS_EXMXPM_PP_MAC(devNum)->regsAddr.ETC.externalTCAMCtrl;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 31, 1, 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* 100 loops configuration */
    loopParams.loopCount = 100;
    loopParams.loopInfinitely = GT_FALSE;
    loopParams.incrementData = GT_FALSE;
    loopParams.incrementRuleIndexOrRegAddr = GT_FALSE;
    rc = cpssExMxPmExternalTcamIndirectCmdLoopParamsSet(devNum, &loopParams);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* NOP at coomand3 */
    rc = cpssExMxPmExternalTcamIndirectCmdParamsSet(devNum, 0,
                                                    CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_NOP_E,
                                                    0, NULL);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* execute NOP command */
    rc = cpssExMxPmExternalTcamIndirectCmdStart(devNum);

    return rc;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamNlConfig
*
* DESCRIPTION:
*       Configure Netlogic TCAM
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NO_RESOURCE           - no place for all rules in external TCAM
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Content of blockUse array entry:
*       bits[7:0] - lookup index
*       bits[15:8] - rule size: 1/2/3 for 160/320/640 bits.
*
*       LTR and Blocks configuration:
*       1. All BMR0 & BMR1 are set to all 0's.
*       2. LTR0 - 160 bit, lookup 0
*          LTR1 - 320 bit, lookup 0
*          LTR2 - 640 bit, lookup 0
*          LTR3 - 160 bit, lookups 0 & 1
*          LTR4 - 320 bit, lookups 0 & 1
*          LTR5 - 640 bit, lookups 0 & 1
*
*******************************************************************************/
static GT_STATUS appDemoExMxPmExternalTcamNlConfig
(
    IN  GT_U8                        devNum
)
{
    GT_U32 rc;              /* return status */
    GT_U32 i, j, k;         /* loop indexes  */
    GT_U32 blockUse[32];    /* local blocks data */
    GT_U32 blockIndex;      /* block number */
    GT_U32 numOfLines;      /* number of lines needed in external TCAM */
    CPSS_EXMXPM_EXTERNAL_TCAM_REG tcamRegVal;  /* TCAM register data to write */
    CPSS_EXMXPM_EXTERNAL_TCAM_REG tcamRegVal1; /* another TCAM register data to write */

    rc = appDemoExMxPmExternalTcamNlReset(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Device Configuration Register (register 1) value setting:     */
    /* bit 10 - enable output impedence update                       */
    /* bit 24 - Ssingle RBUS modes                                   */
    tcamRegVal[0] = 0x1000400;
    tcamRegVal[1] = 0x0;
    tcamRegVal[2] = 0x0;
    rc = cpssExMxPmExternalTcamRegSet(devNum, 0, 0x1, tcamRegVal);
    if( rc != GT_OK )
        return rc;

    osMemSet(blockUse, 0, sizeof(blockUse));
    blockIndex = 0;

    /* loop over lookups */
    for( i = 0 ; i < appDemoExternalTcamNumOflookups ; i++)
    {
        /* loop over rule size in a lookup */
        for ( j = 0 ; j < APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC ; j++)
        {
            appDemoPpConfigList[devNum].extTcamMngPtr[j +
                            i*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC].ruleStartIndex =
                blockIndex*APP_DEMO_NL_TCAM_LINES_IN_BLOCK_MAC;

            /* j = 0, 160 bits, 2 lines per rule */
            /* j = 1, 320 bits, 4 lines per rule */
            /* j = 2, 640 bits, 8 lines per rule */
            numOfLines =
                (appDemoPpConfigList[devNum].extTcamMngPtr[j +
                       i*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC].maxRuleNum) << (j+1) ;
            while (numOfLines > 0)
            {
                if(blockIndex >= APP_DEMO_NL_TCAM_NUM_OF_BLOCKS_MAC)
                    return GT_NO_RESOURCE;
                blockUse[blockIndex] = (j<<8) + i;
                blockIndex++;
                if(numOfLines <= APP_DEMO_NL_TCAM_LINES_IN_BLOCK_MAC)
                    break;
                numOfLines -= APP_DEMO_NL_TCAM_LINES_IN_BLOCK_MAC;
            }
        }
    }

    /* block registers configuration */
    for( i = 0 ; i < blockIndex ; i++ )
    {
        tcamRegVal[0] = 0x0;
        tcamRegVal[1] = 0x0;
        tcamRegVal[2] = 0x0;

        /* BMR0 & BMR1 - Address in TCAM 0x800+block#*0x10 to 0x807+block#*0x10 */
        for( j = 0 ; j < 8 ; j++ )
        {
            rc = cpssExMxPmExternalTcamRegSet(devNum, 0, 0x800+j+i*0x10,
                                                            tcamRegVal);
            if( rc != GT_OK )
                return rc;
        }

        /* Block Configuration register - Address in TCAM 0x400+block#*0x01 */
        switch((blockUse[i] & 0xFF00) >> 8)
        {
            case 0: /* 160 bit */
                tcamRegVal[0] = 0x1 << 1 | 0x1;
                break;
            case 1: /* 320 bit */
                tcamRegVal[0] = 0x2 << 1 | 0x1;
                break;
            case 2: /* 640 bit */
                tcamRegVal[0] = 0x3 << 1 | 0x1;
                break;
            default:
                break;
        }

        rc = cpssExMxPmExternalTcamRegSet(devNum, 0, 0x400+i*0x1, tcamRegVal);
        if( rc != GT_OK )
            return rc;
    }

    /* LTR registers configuration */
    {
        /* loop over lookups */
        for( i = 0; i < appDemoExternalTcamNumOflookups ; i++ )
        {
            /* loop over rule size */
            for ( j = 0 ; j < APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC ; j++)
            {
                tcamRegVal[1] = 0x0;
                tcamRegVal[2] = 0x0;

                /* set key construction map for KPU#0 field in Key Construction Register */
                switch(j)
                {
                    case 0: /* 160 bit */
                        tcamRegVal[0] = 0xF;
                        break;
                    case 1: /* 320 bit */
                        tcamRegVal[0] = 0xFF;
                        break;
                    case 2: /* 640 bit */
                        tcamRegVal[0] = 0xFFFF;
                        break;
                    default:
                        break;
                }

                /* Key Constriction register - Address in TCAM 0x206+LTR#*0x10 */
                rc = cpssExMxPmExternalTcamRegSet(devNum, 0,
                         0x206+(j+i*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC)*0x10,
                         tcamRegVal);
                if( rc != GT_OK )
                    return rc;

                tcamRegVal[0] = 0x0;
                tcamRegVal1[0] = 0x0;
                tcamRegVal1[1] = 0x0;
                tcamRegVal1[2] = 0x0;

                for( k = 0 ; k < blockIndex ; k++ )
                {
                    /* the same rule size and the block lookup fits the LTR lookup */
                    if(((blockUse[k] & 0xFF00) >> 8) == j &&
                       (blockUse[k] & 0xFF) <= i )
                    {
                        /* for Block Select register */
                        tcamRegVal[0] |= (1<<k);

                        /* for Parallel Search register */
                        tcamRegVal1[k/16] |=
                            ( (blockUse[k] & 0xFF) << ((k-(k/16)*16)*2) );
                    }
                }

                /* Block Select register - Address in TCAM 0x200+LTR#*0x10 */
                rc = cpssExMxPmExternalTcamRegSet(devNum, 0,
                         0x200+(j+i*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC)*0x10,
                         tcamRegVal);
                if( rc != GT_OK )
                    return rc;

                /* Parallel Search register - Address in TCAM 0x202+LTR#*0x10 */
                rc = cpssExMxPmExternalTcamRegSet(devNum, 0,
                         0x202+(j+i*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC)*0x10,
                         tcamRegVal1);
                if( rc != GT_OK )
                    return rc;

            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamIdtConfig
*
* DESCRIPTION:
*       Configure IDT TCAM
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        Use the same TCAM division as NL.
*
*        SSSR, DAR and GMR configuration:
*       1. All GMRs are set to all 1's (recommended by MSIL)
*       2. 160 bit, lookup 0       - SSSR0, DAR160_DB0
*          320 bit, lookup 0       - SSSR1, DAR320_DB0
*          640 bit, lookup 0       - SSSR2, DAR640_DB0
*          160 bit, lookups 0 & 1  - SSSR3, DAR160_DB0, DAR160_DB1
*          320 bit, lookups 0 & 1  - SSSR4, DAR320_DB0, DAR320_DB1
*          640 bit, lookups 0 & 1  - SSSR5, DAR640_DB0, DAR640_DB1
*
*******************************************************************************/
static GT_STATUS appDemoExMxPmExternalTcamIdtConfig
(
    IN  GT_U8                        devNum
)
{
    GT_U32 rc;              /* return status */
    GT_U32 i, j, k;         /* loop indexes  */
    GT_U32 blockUse[32];    /* local blocks data */
    GT_U32 blockIndex;      /* block number */
    GT_U32 segmentSize;     /* segment size value for SegSize field in SSSR */
    GT_U32 numOfLines;      /* number of lines needed in external TCAM */
    CPSS_EXMXPM_EXTERNAL_TCAM_REG tcamRegVal;  /* TCAM register data to write */

    /* Configuration Register (register 1) value setting:                       */
    /* bit 1 - Last or Single device                                            */
    /* bit 5 - Search Enable: All insturctions are enabled for normal operation */
    /* bit 7 - Invalidate Array Entry on Parity Error                           */
    /* bit 9 - Input Impedence Control Update Enable                            */
    /* bit 10 - Output Impedence Control Update Enable                          */
    tcamRegVal[0] = 0x6A2;
    tcamRegVal[1] = 0x0;
    tcamRegVal[2] = 0x0;
    rc = cpssExMxPmExternalTcamRegSet(devNum, 0, 0x1, tcamRegVal);
    if( rc != GT_OK )
        return rc;

    /* configure global write mode to data/mask in cpss DB */
    rc = cpssExMxPmExternalTcamGlobalWriteModeSet(devNum, 0,
                 CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_DATA_MASK_AND_LOGIC_E);
    if( rc != GT_OK )
        return rc;

    /* configure TCAM write mode to data/mask */
    /* Feature Configuration Register, bit 8: 0 - data/mask, 1 - X/Y */
    rc = cpssExMxPmExternalTcamRegGet(devNum, 0, 0x1B0, tcamRegVal);
    if( rc != GT_OK )
        return rc;

    U32_SET_FIELD_MAC(tcamRegVal[0] , 8, 1, 0);

    rc = cpssExMxPmExternalTcamRegSet(devNum, 0, 0x1B0, tcamRegVal);
    if( rc != GT_OK )
        return rc;

    osMemSet(blockUse, 0, sizeof(blockUse));

    blockIndex = 0;
    /* loop over lookups */
    for( i = 0 ; i < appDemoExternalTcamNumOflookups ; i++)
    {
        /* loop over rule size in a lookup */
        for ( j = 0 ; j < APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC ; j++)
        {
            appDemoPpConfigList[devNum].extTcamMngPtr[j +
                            i*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC].ruleStartIndex =
                blockIndex*APP_DEMO_NL_TCAM_LINES_IN_BLOCK_MAC;

            /* j = 0, 160 bits, 2 lines per rule */
            /* j = 1, 320 bits, 4 lines per rule */
            /* j = 2, 640 bits, 8 lines per rule */
            numOfLines =
                (appDemoPpConfigList[devNum].extTcamMngPtr[j +
                       i*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC].maxRuleNum) << (j+1) ;
            while (numOfLines > 0)
            {
                if(blockIndex >= APP_DEMO_NL_TCAM_NUM_OF_BLOCKS_MAC)
                    return GT_NO_RESOURCE;
                blockUse[blockIndex] = (j<<8) + i;
                blockIndex++;
                if(numOfLines <= APP_DEMO_NL_TCAM_LINES_IN_BLOCK_MAC)
                    break;
                numOfLines -= APP_DEMO_NL_TCAM_LINES_IN_BLOCK_MAC;
            }
        }
    }

    /* DARs configuration according to blocks */
    tcamRegVal[1] = 0x0;
    tcamRegVal[2] = 0x0;

    /* loop over lookups */
    for( i = 0 ; i < appDemoExternalTcamNumOflookups ; i++)
    {
        /* loop over rule size in a lookup */
        for ( j = 0 ; j < APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC ; j++)
        {
            /* DAR160_DB#n address id 0x208+n */
            /* DAR320_DB#n address id 0x20C+n */
            /* DAR640_DB#n address id 0x214+n */
            GT_U32 darAddress[] =
            { 0x208, 0x20C, 0x214,
              0x209, 0x20D, 0x215,
              0x20A, 0x20E, 0x216,
              0x20B, 0x20F, 0x217
            };

            tcamRegVal[0] = 0x0;

            for( k = 0 ; k < blockIndex ; k++ )
            {
                /* the same as the lookup and rule size */
                if(((blockUse[k] & 0xFF00) >> 8) == j &&
                   (blockUse[k] & 0xFF) == i )
                {
                    tcamRegVal[0] |= (1<<k);
                }
            }

            rc = cpssExMxPmExternalTcamRegSet(devNum, 0,
                         darAddress[j+i*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC],
                         tcamRegVal);
            if( rc != GT_OK )
                return rc;
        }
    }

    /* loop over lookups */
    for( i = 0 ; i < appDemoExternalTcamNumOflookups ; i++)
    {
        /* loop over rule size in a lookup */
        for ( j = 0 ; j < APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC ; j++)
        {
            segmentSize = 0;
            switch(j)
            {
                case 0: segmentSize = 0x2;
                    break;
                case 1: segmentSize = 0x3;
                    break;
                case 2: segmentSize = 0x5;
                    break;
                default:
                    break;
            }
            /* set SSj_SegSize and SSj_DB fields in SSSR */

            /* SSj_SegSize_0 = SSj_SegSize_1 = segmentSize */
            /* SSj_DB_0 = 0, SSj_DB_1 = 1                  */
            tcamRegVal[0] = (segmentSize << 3) | (segmentSize << 23) | (1<<20) ;

            /* SSj_SegSize_2 = SSj_SegSize_3 = segmentSize */
            /* SSj_DB_2 = 2, SSj_DB_3 = 3                  */
            tcamRegVal[1] = (2<<8) | (segmentSize << 11) | (3<<28) |
                                (segmentSize << 31);

            /* SSj_SegSize_3 = segmentSize */
            tcamRegVal[2] = (segmentSize >> 1);

            /* set EN_* fields in SSSR */
            switch(i)
            {
                case 3: tcamRegVal[2] |= (1<<15);
                case 2: tcamRegVal[1] |= (1<<27);
                case 1: tcamRegVal[1] |= (1<<7);
                case 0: tcamRegVal[0] |= (1<<19);
                    break;
                default:
                    break;
            }

            /* SSSR#n address id 0x20+n */
            rc = cpssExMxPmExternalTcamRegSet(devNum, 0,
                                0x20+j+i*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC,
                                tcamRegVal);
            if( rc != GT_OK )
                return rc;

        }
    }

    /* configure all GMRs to all 1's */
    tcamRegVal[0] = 0xFFFFFFFF;
    tcamRegVal[1] = 0xFFFFFFFF;
    tcamRegVal[2] = 0x0000FFFF;

    /* GMRs address 0x300 - 0x3BF */
    for (i = 0x300 ; i <= 0x3BF ; i++)
    {
        rc = cpssExMxPmExternalTcamRegSet(devNum, 0, i, tcamRegVal);
        if( rc != GT_OK )
            return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamConfig
*
* DESCRIPTION:
*       Configure external TCAM device (by setting appropriate values to external
*       TCAM registers, not ETC registers) based on the external TCAM management
*       database content.
*       This function configure the ruleStartIndex fields in the external TCAM
*       management database.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Since the external TCAM type used is required this function is called
*       after CPSS database is initialized with external TCAM type.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamConfig
(
    IN  GT_U8                        devNum
)
{
    GT_STATUS   rc;                                 /* return code         */
    CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_ENT tcamType;    /* external TCAM type */

    rc = cpssExMxPmExternalTcamTypeGet(devNum, 0, &tcamType);
    if( rc != GT_OK)
        return rc;

    if(tcamType == CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_NL_E)
        rc = appDemoExMxPmExternalTcamNlConfig(devNum);
    else /* tcamType == CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_IDT_E */
        rc = appDemoExMxPmExternalTcamIdtConfig(devNum);

    return rc;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamCalcIndexConvertParams
*
* DESCRIPTION:
*       Calculate the hit index conversion params for a specified lookup and
*       key type.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       lookupIndex     - the lookup number
*       ruleSize        - rule size in external TCAM
*
* OUTPUTS:
*       indexConvertPtr - (pointer to) hit index conversion params.

* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamCalcIndexConvertParams
(
    IN  GT_U8                                            devNum,
    IN  GT_U32                                           lookupIndex,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT          ruleSize,
    OUT  CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC *indexConvertPtr
)
{
    GT_U32  ruleIndexInMngDb;  /* rule index in external TCAM management database */
    GT_U32  ruleStartIndexAfterShift;  /* the start index after the shift operation */

    APP_DEMO_EXTERNAL_TCAM_GET_RULE_SIZE_INDEX_MAC(ruleSize, ruleIndexInMngDb);

    switch(ruleSize)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E:
                indexConvertPtr->shiftOperation = CPSS_SHIFT_OPERATION_SHIFT_LEFT_E;
                indexConvertPtr->shiftValue = 1;
                ruleStartIndexAfterShift =
                  appDemoPpConfigList[devNum].extTcamMngPtr[ruleIndexInMngDb +
                    lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC].ruleStartIndex << 1;
                break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E:
                indexConvertPtr->shiftOperation = CPSS_SHIFT_OPERATION_SHIFT_LEFT_E;
                indexConvertPtr->shiftValue = 0;
                ruleStartIndexAfterShift =
                  appDemoPpConfigList[devNum].extTcamMngPtr[ruleIndexInMngDb +
                    lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC].ruleStartIndex;
                break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E:
                indexConvertPtr->shiftOperation = CPSS_SHIFT_OPERATION_SHIFT_RIGHT_E;
                indexConvertPtr->shiftValue = 1;
                ruleStartIndexAfterShift =
                  appDemoPpConfigList[devNum].extTcamMngPtr[ruleIndexInMngDb +
                    lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC].ruleStartIndex >> 1;
                break;
        default: return GT_BAD_PARAM;
                 break;
    }

    if(ruleStartIndexAfterShift >
                    appDemoPpConfigList[devNum].extTcamMngPtr[ruleIndexInMngDb +
                     lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC].actionStartIndex)
    {
         indexConvertPtr->baseAddrOperation = CPSS_ADJUST_OPERATION_SUBSTRUCT_E;
         indexConvertPtr->baseAddrOffset = ruleStartIndexAfterShift -
              appDemoPpConfigList[devNum].extTcamMngPtr[ruleIndexInMngDb +
                     lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC].actionStartIndex;
    }
    else
    {
         indexConvertPtr->baseAddrOperation = CPSS_ADJUST_OPERATION_ADD_E;
         indexConvertPtr->baseAddrOffset =
             appDemoPpConfigList[devNum].extTcamMngPtr[ruleIndexInMngDb +
                   lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC].actionStartIndex -
                                 ruleStartIndexAfterShift;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamControllerConfig
*
* DESCRIPTION:
*       Configure external TCAM controller (ETC). ETC configuration involves
*       setting the Client Search and Base Address Conversion tables.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.

* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function is called after the external TCAM was configured
*       (and therefore the ruleStartIndex fields in the external TCAM
*       management database were configured).
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamControllerConfig
(
    IN  GT_U8                        devNum
)
{
    GT_STATUS   rc;                                 /* return code        */
    GT_U32  lookupIndex;                            /* the lookup index   */
    GT_U32  ruleIndexInMngDb;  /* rule index in external TCAM management database */
    GT_U32  i;                  /* loop index counter */
    CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC clientConfigParams; /* ETC client params */
    CPSS_EXMXPM_EXTERNAL_TCAM_HIT_INDEX_CONVERT_STC indexConvertParams; /* ETC hit index convert params */

    clientConfigParams.tcamDeviceSelectBmp = 0x1;
    clientConfigParams.globalMask = 0;

    i = 0;
    while(appDemoNumOfRulesTblPtr[i].lookupIndex != APP_DEMO_EXTERNAL_TCAM_EOT_MAC)
    {
         APP_DEMO_EXTERNAL_TCAM_GET_RULE_SIZE_INDEX_MAC(
                                appDemoNumOfRulesTblPtr[i].ruleSize, ruleIndexInMngDb);

         clientConfigParams.lookupSchemeIndex = ruleIndexInMngDb +
             appDemoNumOfRulesTblPtr[i].lookupIndex*APP_DEMO_EXTERNAL_TCAM_RULE_LEN_NUM_MAC;

         /* Client Search table entry set */
         rc = cpssExMxPmExternalTcamClientConfigSet(devNum,
                        appDemoNumOfRulesTblPtr[i].tcamClient,
                        appDemoNumOfRulesTblPtr[i].clientKeyType,
                        appDemoNumOfRulesTblPtr[i].lookupIndex,
                        &clientConfigParams);
         CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmExternalTcamClientConfigSet", rc);
         if(rc != GT_OK)
             return rc;



         /* Address Hit Index Conversion table entry set */
         for(lookupIndex = 0 ; lookupIndex <= appDemoNumOfRulesTblPtr[i].lookupIndex ;
                                                                            lookupIndex++)
         {
             APP_DEMO_EXTERNAL_TCAM_GET_RULE_SIZE_INDEX_MAC(
                                appDemoNumOfRulesTblPtr[i].ruleSize, ruleIndexInMngDb);

             rc = appDemoExMxPmExternalTcamCalcIndexConvertParams(devNum,
                        lookupIndex,
                        appDemoNumOfRulesTblPtr[i].ruleSize,
                        &indexConvertParams);
             CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoExMxPmExternalTcamCalcIndexConvertParams", rc);
             if(rc != GT_OK)
                 return rc;

             rc = cpssExMxPmExternalTcamHitIndexConvertSet(devNum,
                        appDemoNumOfRulesTblPtr[i].tcamClient,
                        appDemoNumOfRulesTblPtr[i].clientKeyType,
                        appDemoNumOfRulesTblPtr[i].lookupIndex,
                        lookupIndex,
                        &indexConvertParams);
             CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmExternalTcamHitIndexConvertSet", rc);
             if(rc != GT_OK)
                 return rc;
         }
         i++;
    }

    return GT_OK;
}
