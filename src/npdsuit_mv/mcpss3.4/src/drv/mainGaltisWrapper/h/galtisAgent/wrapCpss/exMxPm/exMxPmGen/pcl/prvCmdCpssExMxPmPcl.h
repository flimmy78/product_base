/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* prvCmdCpssExMxPmPcl.h
*
* DESCRIPTION:
*       This file implements PCL common definitions and data used in
*       both generic and customer specic Galtis wrappers.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
******************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/exMxPm/exMxPmGen/pcl/cpssExMxPmPcl.h>
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>

#ifndef __prvCmdCpssExMxPmPclh__
#define __prvCmdCpssExMxPmPclh__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    /****************************pcl action table*************************************/
/* --- cpssExMxPmPclRule Table--- */

/* --- cpssExMxPmPclAction Table*/

/* table cpssExMxPmPclAction global variables */

#define ACTION_TABLE_SIZE 128

#define EXT_TCAM_CFG_TABLE_ZISE 128

#define INVALID_RULE_INDEX 0x7FFFFFF0

typedef struct
{
    CPSS_EXMXPM_PCL_ACTION_UNT             actionEntry;
    CPSS_EXMXPM_PCL_ACTION_FORMAT_ENT      actionFormat;
    GT_U32                                 ruleIndex;
    GT_U8                                  devNum;
    GT_BOOL                                valid;
    GT_U32                                 nextFree;
} WR_PCL_ACTION_STC;

typedef struct
{   /*extTcam Rule params*/
    GT_U8                                     devNum;
    GT_U32                                    tcamId;
    GT_U32                                    ruleStartIndex;
    CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT   ruleSize;
    GT_BOOL                                   ageEnable;
    /*extTcam action params */
    GT_U32                                    actionStartIndex;
    CPSS_EXMXPM_CONTROL_MEMORY_ENT            memoryType;

    GT_BOOL                                   valid;
    GT_U32                                    nextFree;
} WR_CPSS_EXMXPM_PCL_EXT_TCAM_CFG_STC;


extern CPSS_EXMXPM_PCL_RULE_FORMAT_UNT      *wrCpssExMxPmPcl_maskDataPtr;
extern CPSS_EXMXPM_PCL_RULE_FORMAT_UNT      *wrCpssExMxPmPcl_patternDataPtr;
extern CPSS_EXMXPM_PCL_RULE_FORMAT_TYPE_ENT *wrCpssExMxPmPcl_ruleFormatPtr;
extern GT_BOOL *wrCpssExMxPmPcl_useExternalTcamPtr;
extern WR_PCL_ACTION_STC *wrCpssExMxPmPcl_actionIntTable;
extern WR_PCL_ACTION_STC *wrCpssExMxPmPcl_actionExtTable;
extern WR_CPSS_EXMXPM_PCL_EXT_TCAM_CFG_STC *wrCpssExMxPmPcl_extTcamConfigPclTablePtr;

/*******************************************************************************
* wrCpssExMxPmPclExtConfigIndexGet (table utilite)
*
* DESCRIPTION:
*  The function searches for external tcam config  table entry by rule index
*  and returns index.
*
*
*
* INPUTS:
*
*       ruleIndex       - Index of the rule external TCAM.
*
* OUTPUTS:
*       tableIndex     - external TCAM table entry
*
* RETURNS:
*         none
*
* COMMENTS:
*
*******************************************************************************/
void wrCpssExMxPmPclExtConfigIndexGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  ruleIndex,
    OUT GT_U32                  *tableIndex
);

/*******************************************************************************
* wrCpssExMxPmPclActionTableIndexGet (table command)
*
* DESCRIPTION:
*  The function searches for action table entry by rule index and device number
*  and returns the actionEntry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       ruleIndex       - Index of the rule in the internal or external TCAM.
*
* OUTPUTS:
*       tableIndex     - action table entry
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
void wrCpssExMxPmPclActionTableIndexGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  ruleIndex,
    IN  WR_PCL_ACTION_STC      *actionTable,
    OUT GT_U32                  *tableIndex
);

/* RULE BY FIELD SET */
#define PRV_PM_PCL_RULE_FMT_PATH_SIZE_CNS 64

#define PRV_PM_PCL_RULE_FMT_NAME_CNS "***RULE_FORMAT***"

/* the dummy descripror that holds format in "offset" variable */
#define PRV_PM_PCL_RULE_FMT_HOLDER_MAC(_format)          \
    {                                                    \
        PRV_PM_PCL_RULE_FMT_NAME_CNS,                    \
        (struct _stc_GT_UTIL_STC_ELEMENT_DSC_STC*)0,     \
        (GT_U32)_format                                  \
    }

extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrStd[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrStdPkt[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrStdIp[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrStandardNotIp[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrStandardIpL2Qos[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrStandardIpv4L4[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonEgr[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonEgrStd[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonEgrStdIp[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrStandardNotIp[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrStandardIpL2Qos[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrStandardIpv4L4[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrExt[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrExtPkt[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrExtendedNotIpv6[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrExtendedIpv6L2[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrExtendedIpv6L4[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonEgrExt[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrExtendedIpv6L2[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrExtendedIpv6L4[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_EgrExtendedNotIpv6[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_commonIngrUdb[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrStandardUdb[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrExtendedUdb[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_IngrExternalUdb80B[];
extern GT_UTIL_STC_ELEMENT_DSC_STC wrCpssExMxPmPclRuleFieldDsc_Union[];


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__prvCmdCpssExMxPmPclh__*/


