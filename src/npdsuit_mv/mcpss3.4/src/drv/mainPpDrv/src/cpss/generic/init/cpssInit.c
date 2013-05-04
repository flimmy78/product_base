/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssInit.c
*
* DESCRIPTION:
*       Initialization function for PP's , regardless to PP's types.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/generic/init/cpssInit.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>

#if ( ((defined EX_FAMILY) || (defined MX_FAMILY)) && !(defined CPSS_PRODUCT))
#   include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#endif /* (defined EX_FAMILY) || (defined MX_FAMILY) */

#define DEBUG_PRINT(x) cpssOsPrintf x

/*******************************************************************************
* cpssPpInit
*
* DESCRIPTION:
*       This function initialize the internal DB of the CPSS regarding PPs.
*       This function must be called before any call to a PP functions ,
*       i.e before calling cpssExMxHwPpPhase1Init.
*
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - on failure.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPpInit
(
    void
)
{
    GT_STATUS rc;

    /***********************/
    /* initialize the cpss */
    /***********************/
    rc = prvCpssSysConfigPhase1();
    if(rc != GT_OK)
    {
        return rc;
    }

    /*****************************/
    /* initialize the cpssDriver */
    /*****************************/
    rc = prvCpssDrvSysConfigPhase1();
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Disable the combining writes / read feature in the system controller */
    rc = cpssExtDrvEnableCombinedPciAccess(GT_FALSE,GT_FALSE);


    return rc;
}

extern GT_STATUS prvCpssDrvPpDump(GT_U8   devNum);

/*******************************************************************************
* cpssPpDump
*
* DESCRIPTION:
*       Dump function , to print the info on a specific PP.
*
*
* APPLICABLE DEVICES: 
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device Number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPpDump
(
    IN GT_U8   devNum
)
{
    PRV_CPSS_GEN_PP_CONFIG_STC      *devInfoPtr;/* pointer to device info */
    PRV_CPSS_PORT_INFO_ARRAY_STC    *portInfoPtr;/* pointer to current port info */
    GT_U8   port;/* port iterator*/

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        DEBUG_PRINT(("bad device number [%d] \n",devNum));
        return GT_BAD_PARAM;
    }

    DEBUG_PRINT(("\n"));
    DEBUG_PRINT(("start Info about device number [%d]: \n",devNum));

    devInfoPtr = PRV_CPSS_PP_MAC(devNum);

    DEBUG_PRINT((" devNum = [%d] \n",devInfoPtr->devNum));
    DEBUG_PRINT((" functionsSupportedBmp = [%d] \n",devInfoPtr->functionsSupportedBmp));

    DEBUG_PRINT((" devType = [0x%8.8x] \n",devInfoPtr->devType));
    DEBUG_PRINT((" revision = [%d] \n",devInfoPtr->revision));
    DEBUG_PRINT((" devFamily = [%s] \n",
        devInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTC_E    ? " CPSS_PP_FAMILY_TWISTC_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTD_E    ? " CPSS_PP_FAMILY_TWISTD_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_SAMBA_E     ? " CPSS_PP_FAMILY_SAMBA_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_SALSA_E     ? " CPSS_PP_FAMILY_SALSA_E ":

        devInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH_E   ? " CPSS_PP_FAMILY_CHEETAH_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH2_E  ? " CPSS_PP_FAMILY_CHEETAH2_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH3_E  ? " CPSS_PP_FAMILY_CHEETAH3_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E  ? " CPSS_PP_FAMILY_DXCH_XCAT_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION_E  ? " CPSS_PP_FAMILY_DXCH_LION_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_TIGER_E     ? " CPSS_PP_FAMILY_TIGER_E ":
        devInfoPtr->devFamily == CPSS_PP_FAMILY_PUMA_E     ? " CPSS_PP_FAMILY_PUMA_E ":

        " unknown "
    ));

    DEBUG_PRINT((" numOfPorts = [%d] \n",devInfoPtr->numOfPorts));
    DEBUG_PRINT((" numOfVirtPorts = [%d] \n",devInfoPtr->numOfVirtPorts));
    DEBUG_PRINT((" existingPorts[0] = [0x%8.8x] \n",devInfoPtr->existingPorts.ports[0]));
    DEBUG_PRINT((" existingPorts[1] = [0x%8.8x] \n",devInfoPtr->existingPorts.ports[1]));
    DEBUG_PRINT((" baseCoreClock = [%d] \n",devInfoPtr->baseCoreClock));
    DEBUG_PRINT((" coreClock = [%d] \n",devInfoPtr->coreClock));

    DEBUG_PRINT((" mngInterfaceType = [%s] \n",
        PRV_CPSS_HW_IF_PEX_COMPATIBLE_MAC(devNum) ? " CPSS_CHANNEL_PEX_E " :
        PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum) ? " CPSS_CHANNEL_PCI_E " :
        PRV_CPSS_HW_IF_SMI_COMPATIBLE_MAC(devNum)  ? " CPSS_CHANNEL_SMI_E " :
        PRV_CPSS_HW_IF_TWSI_COMPATIBLE_MAC(devNum) ? " CPSS_CHANNEL_TWSI_E " :
        " unknown "
    ));

    DEBUG_PRINT((" hwDevNum = [%d] \n",devInfoPtr->hwDevNum));

    DEBUG_PRINT((" configInitAfterHwResetDone = [%s] \n",
            devInfoPtr->configInitAfterHwResetDone == GT_TRUE   ? " GT_TRUE " :
            devInfoPtr->configInitAfterHwResetDone == GT_FALSE  ? " GT_FALSE " :
            " unknown "
    ));

    DEBUG_PRINT((" ppHAState = [%s] \n",
            devInfoPtr->ppHAState == CPSS_SYS_HA_MODE_ACTIVE_E  ? " CPSS_SYS_HA_MODE_ACTIVE_E " :
            devInfoPtr->ppHAState == CPSS_SYS_HA_MODE_STANDBY_E ? " CPSS_SYS_HA_MODE_STANDBY_E " :
            " unknown "
    ));


    if(devInfoPtr->phyPortInfoArray == NULL)
    {
        DEBUG_PRINT((" devInfoPtr->phyPortInfoArray ERROR -- NULL POINTER !!! \n"));
    }
    else
    {
        portInfoPtr = &devInfoPtr->phyPortInfoArray[0];
        for (port = 0 ; port < devInfoPtr->numOfPorts ; port++ , portInfoPtr++)
        {
            DEBUG_PRINT(("port [%d] \n",port));

            DEBUG_PRINT((" portInfoPtr->portFcParams.xonLimit = [%d] \n",portInfoPtr->portFcParams.xonLimit));
            DEBUG_PRINT((" portInfoPtr->portFcParams.xoffLimit = [%d] \n",portInfoPtr->portFcParams.xoffLimit));

            DEBUG_PRINT((" portInfoPtr->portType = [%s] \n",
                        portInfoPtr->portType == PRV_CPSS_PORT_NOT_EXISTS_E ? " PRV_CPSS_PORT_NOT_EXISTS_E " :
                        portInfoPtr->portType == PRV_CPSS_PORT_FE_E         ? " PRV_CPSS_PORT_FE_E " :
                        portInfoPtr->portType == PRV_CPSS_PORT_GE_E         ? " PRV_CPSS_PORT_GE_E " :
                        portInfoPtr->portType == PRV_CPSS_PORT_XG_E         ? " PRV_CPSS_PORT_XG_E " :
            " unknown "
            ));

            DEBUG_PRINT((" portInfoPtr->smiIfInfo.smiPortAddr = [0x%4.4x] \n",portInfoPtr->smiIfInfo.smiPortAddr));
            DEBUG_PRINT((" portInfoPtr->smiIfInfo.smiCtrlRegAddr = [0x%8.8x] \n",portInfoPtr->smiIfInfo.smiCtrlRegAddr));
        }
    }

    /* print more specific info */
#if ( ((defined EX_FAMILY) || (defined MX_FAMILY)) && !(defined CPSS_PRODUCT))
    {
        PRV_CPSS_EXMX_PP_CONFIG_STC  *exMxDevInfoPtr;

        DEBUG_PRINT(("\n"));

        DEBUG_PRINT((" This is ExMx device \n"));

        exMxDevInfoPtr = PRV_CPSS_EXMX_PP_MAC(devNum);

        DEBUG_PRINT((" exMxDevInfoPtr->uplinkCfg = [%s] \n",
            exMxDevInfoPtr->uplinkCfg == CPSS_PP_UPLINK_BACK_TO_BACK_E  ? " CPSS_PP_UPLINK_BACK_TO_BACK_E " :
            exMxDevInfoPtr->uplinkCfg == CPSS_PP_UPLINK_FA_E            ? " CPSS_PP_UPLINK_FA_E " :
            exMxDevInfoPtr->uplinkCfg == CPSS_PP_NO_UPLINK_E            ? " CPSS_PP_NO_UPLINK_E " :
            " unknown "
            ));
    }
#endif /* (defined EX_FAMILY) || (defined MX_FAMILY) */

    DEBUG_PRINT(("finished Info about device number [%d]: \n",devNum));
    DEBUG_PRINT(("\n"));
    DEBUG_PRINT(("\n"));

    /* call the driver to print it's info */
    prvCpssDrvPpDump(devNum);

    return GT_OK;

}

