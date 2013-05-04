/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvUtfExtras.c
*
* DESCRIPTION:
*       defines API for helpers functions
*       which are specific for cpss unit testing.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#if (defined EX_FAMILY) || (defined MX_FAMILY)
#define CALL_EX_MX_CODE_MAC
#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#endif /* (defined EX_FAMILY) || (defined MX_FAMILY) */

#ifdef CHX_FAMILY
    #ifndef CALL_EX_MX_CODE_MAC
        /*the include to H file of exmx clash with the h file of the dxch
            (about definition of _txPortRegs)*/
        #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
        #define CALL_DXCH_CODE_MAC
    #endif /*CALL_EX_MX_CODE_MAC*/
    #include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
    #include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#endif /*CHX_FAMILY*/

#ifdef EXMXPM_FAMILY
#include <cpss/exMxPm/exMxPmGen/cpssHwInit/cpssExMxPmHwInit.h>
#include <cpss/exMxPm/exMxPmGen/config/cpssExMxPmCfg.h>
#endif /*EXMXPM_FAMILY*/

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>
#include <utf/private/prvUtfExtras.h>

#include <appDemo/sysHwConfig/appDemoDb.h>

/******************************************************************************\
 *                        Packet Processor Definitions                       *
\******************************************************************************/

/* macro to check that device exists */
#define  DEV_CHECK_MAC(dev) if(!PRV_CPSS_IS_DEV_EXISTS_MAC(dev)) return GT_BAD_PARAM


/*
 * Typedef: struct EXTRAS_CTX_STC
 *
 * Description: Extras context storage.
 *              There is can be only one statically allocated
 *              instance of this storage. Used for saving
 *              devices, ports, and iterators run-time info.
 *
 * Fields:
 *          devArray            - array of active/non active devices:
 *              for non-active device = 0,
 *              for active devices = utf constant corresponding to device's family
                                    (UTF_CPSS_PP_FAMILY_XXX_CNS, where XXX - family name)
 *          phyNumPortsArray    - contains number of physical ports for each device
 *          virtNumPortsArray   - contains number of virtual ports for each device
 *          devForPhyPorts      - current active device id for iteration thru phy ports
 *          devForVirtPorts     - current active device id for iteration thru virt ports
 */
typedef struct
{
    GT_U32  devArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U8   phyNumPortsArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U8   virtNumPortsArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U8   devForPhyPorts;
    GT_U8   devForVirtPorts;
    GT_U32  currentApplicableFamiliesHandle;
} EXTRAS_CTX_STC;

/* single instance of extras context, used for iteration */
static EXTRAS_CTX_STC extCtx;

/* Extras internal functions forward declaration */
static GT_STATUS devCheck(IN GT_U8 dev);
static GT_STATUS phyPortCheck(IN GT_U8 dev, IN GT_U8 port);
static GT_STATUS virtPortCheck(IN GT_U8 dev, IN GT_U8 port);
static GT_VOID firstTimeTrunkConvertInit(GT_VOID);

/* flag to state that the DXCH (xcat) devices need the WA of trunk-Id conversions */
GT_BOOL prvUtfDxChTrunkWaNeeded = GT_FALSE;

/* indication the trunk convert done for first time */
static GT_BOOL firstTimeTrunkConvert = GT_TRUE;

/* number of skipped UT in suit */
static GT_U32 prvUtfSkipTestNum = 0;

/* flag to indicate is current test is skipped */
static GT_BOOL prvUtfIsTestSkip = GT_FALSE;

/* use 56 as the first trunk-ID */
#define WA_FIRST_TRUNK_CNS  56
#define WA_LAST_TRUNK_CNS  (124+1)

/* Array with invalid enum values to check.
   Using GT_U32 type instead of GT_32 can be helpful to find some bugs */
GT_U32 utfInvalidEnumArr[] = {0x5AAAAAA5, 0xFFFFFFFF};

/* Invalid enum array size */
GT_U32 utfInvalidEnumArrSize = sizeof(utfInvalidEnumArr) / sizeof(utfInvalidEnumArr[0]);


/******************************************************************************\
 *                          Fabric Adapter Definitions                        *
\******************************************************************************/

#ifdef IMPL_FA

/* External usage environment parameters */
extern CORE_FA_CONFIG  *coreFaDevs[PRV_CPSS_MAX_PP_DEVICES_CNS];

/*
 * Typedef: struct EXTRAS_FA_CTX_STC
 *
 * Description: Extras context storage.
 *              There is can be only one statically allocated
 *              instance of this storage. Used for saving
 *              devices fabric adapret, ports, and iterators run-time info.
 *
 * Fields:
 *          devArray      - array of active/non active device fabric adapters:
 *              for non-active device = 0,
 *              for active devices = utf constant corresponding to device's type
 *                                   (UTF_CPSS_FA_TYPE_XXX_CNS, where XXX - type name)
 *          devForFaPorts - contains number of Fabric ports for each device
 */
typedef struct
{
    GT_U8   devArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U8   faNumPortsArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U8   devForFaPorts;
    GT_U32  currentApplicableTypesHandle;
} EXTRAS_FA_CTX_STC;

/* single instance of extras context, used for iteration */
static EXTRAS_FA_CTX_STC extFaCtx;

/* Extras internal functions forward declaration */
static GT_STATUS devFaCheck(IN GT_U8 dev);

/* Returns GT_OK if fabric port is active */
static GT_STATUS faPortCheck(IN GT_U8 dev, IN GT_U8 port);

/* macro to check that FA device exists */
#define  FA_CHECK_MAC(dev) if (GT_OK != devFaCheck(dev)) return GT_BAD_PARAM

#endif /* IMPL_FA */


/******************************************************************************\
 *                                 Xbar Definitions                           *
\******************************************************************************/

#ifdef IMPL_XBAR

/* External usage environment parameters */
extern CORE_XBAR_CONFIG  coreXbarDevs[PRV_CPSS_MAX_PP_DEVICES_CNS];

/*
 * Typedef: struct EXTRAS_XBAR_CTX_STC
 *
 * Description: Extras context storage.
 *              There is can be only one statically allocated
 *              instance of this storage. Used for saving
 *              devices xbar adapret, ports, and iterators run-time info.
 *
 * Fields:
 *          devArray      - array of active/non active device xbar adapters:
 *              for non-active device = 0,
 *              for active devices = utf constant corresponding to device's type
 *                                   (UTF_CPSS_XBAR_TYPE_XXX_CNS, where XXX - type name)
 *          devForFaPorts - contains number of Fabric ports for each device
 */
typedef struct
{
    GT_U8   devArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U8   faNumPortsArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
    GT_U8   devForFaPorts;
    GT_U32  currentApplicableTypesHandle;
} EXTRAS_XBAR_CTX_STC;

/* single instance of extras context, used for iteration */
static EXTRAS_XBAR_CTX_STC extXbarCtx;

/* Extras internal functions forward declaration */
static GT_STATUS devXbarCheck(IN GT_U8 dev);

/* Returns GT_OK if fabric port is active */
static GT_STATUS xbarFaPortCheck(IN GT_U8 dev, IN GT_U8 port);

/* macro to check that Xbar device exists */
#define  XBAR_CHECK_MAC(dev) if (GT_OK != devXbarCheck(dev)) return GT_BAD_PARAM

#endif /* IMPL_XBAR */


/*******************************************************************************
* prvUtfExtrasInit
*
* DESCRIPTION:
*       Initialize extras for cpss unit testing.
*       This function inits available devices and ports per device.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfExtrasInit
(
    GT_VOID
)
{
    GT_STATUS st = GT_OK;
    GT_U8 dev;
    GT_BOOL devWasFound     = GT_FALSE;
    GT_BOOL devFaWasFound   = GT_FALSE;
    GT_BOOL devXbarWasFound = GT_FALSE;

    cpssOsBzero((GT_CHAR*)&extCtx, sizeof(extCtx));

#ifdef IMPL_FA
    cpssOsBzero((GT_CHAR*)&extFaCtx, sizeof(extFaCtx));
#endif /* IMPL_FA */

#ifdef IMPL_XBAR
    cpssOsBzero((GT_CHAR*)&extXbarCtx, sizeof(extXbarCtx));
#endif /* IMPL_XBAR */

    for (dev = 0; dev < PRV_CPSS_MAX_PP_DEVICES_CNS; dev++)
    {
        if (GT_OK == devCheck(dev))
        {
            switch (PRV_CPSS_PP_MAC(dev)->devFamily)
            {
                case CPSS_PP_FAMILY_TWISTC_E:
                    extCtx.devArray[dev] = UTF_CPSS_PP_FAMILY_TWISTC_CNS;
                    break;
                case CPSS_PP_FAMILY_TWISTD_E:
                    extCtx.devArray[dev] = UTF_CPSS_PP_FAMILY_TWISTD_CNS;
                    break;
                case CPSS_PP_FAMILY_SAMBA_E:
                    extCtx.devArray[dev] = UTF_CPSS_PP_FAMILY_SAMBA_CNS;
                    break;
                case CPSS_PP_FAMILY_TIGER_E:
                    extCtx.devArray[dev] = UTF_CPSS_PP_FAMILY_TIGER_CNS;
                    break;    
                case CPSS_PP_FAMILY_SALSA_E:
                    extCtx.devArray[dev] = UTF_CPSS_PP_FAMILY_SALSA_CNS;
                    break;
                case CPSS_PP_FAMILY_PUMA_E:
                    extCtx.devArray[dev] = UTF_CPSS_PP_FAMILY_PUMA_CNS;
                    break;
                case CPSS_PP_FAMILY_CHEETAH_E:
                    extCtx.devArray[dev] = (CPSS_98DX106_CNS == PRV_CPSS_PP_MAC(dev)->devType) ? UTF_CH1_DIAMOND_E : UTF_CH1_E;
                    break;
                case CPSS_PP_FAMILY_CHEETAH2_E:
                    extCtx.devArray[dev] = UTF_CH2_E;
                    break;
                case CPSS_PP_FAMILY_CHEETAH3_E:
                    extCtx.devArray[dev] = UTF_CH3_E;
                    break;
                case CPSS_PP_FAMILY_DXCH_XCAT_E:
                    extCtx.devArray[dev] = (0 == PRV_CPSS_PP_MAC(dev)->revision) ? UTF_CH3_E : UTF_XCAT_E;
                    break;
                case CPSS_PP_FAMILY_DXCH_LION_E:
                    extCtx.devArray[dev] = UTF_LION_E;
                    break;
                case CPSS_PP_FAMILY_DXCH_XCAT2_E:
                    extCtx.devArray[dev] = UTF_XCAT2_E;
                    break;
                
                default:
                    extCtx.devArray[dev] = 0;
            }
            extCtx.phyNumPortsArray[dev] = PRV_CPSS_PP_MAC(dev)->numOfPorts;
            extCtx.virtNumPortsArray[dev] = PRV_CPSS_PP_MAC(dev)->numOfVirtPorts;
            extCtx.currentApplicableFamiliesHandle = UTF_ALL_FAMILY_E;
            devWasFound = GT_TRUE;

            PRV_UTF_LOG3_MAC("[UTF]: prvUtfExtrasInit: PP dev [%d], phy ports [%d], virt ports [%d] ",/* no \n */
                             dev,
                             extCtx.phyNumPortsArray[dev],
                             extCtx.virtNumPortsArray[dev]);

            /*add printing of existing ports bmp[0] */
            PRV_UTF_LOG1_MAC("ports bmp[0x%8.8x]", /* no \n */
                             PRV_CPSS_PP_MAC(dev)->existingPorts.ports[0]);
            if(PRV_CPSS_PP_MAC(dev)->existingPorts.ports[1])
            {
                /*add printing of existing ports bmp[1] */
                PRV_UTF_LOG1_MAC("[0x%8.8x]", /* no \n */
                                 PRV_CPSS_PP_MAC(dev)->existingPorts.ports[1]);
            }

            PRV_UTF_LOG0_MAC("\n");  /* here is the \n */
        }
        else
        {
            extCtx.devArray[dev] = 0;
            extCtx.phyNumPortsArray[dev] = 0;
            extCtx.virtNumPortsArray[dev] = 0;
        }

#ifdef IMPL_FA

        if (GT_OK == devFaCheck(dev))
        {
            switch (coreFaDevs[dev]->devType)
            {
                case GT_FA_98FX900A:
                    extFaCtx.devArray[dev] = UTF_CPSS_FA_TYPE_98FX900A_CNS;
                break;
                case GT_FA_98FX902A:
                    extFaCtx.devArray[dev] = UTF_CPSS_FA_TYPE_98FX902A_CNS;
                break;
                case GT_FA_98FX910A:
                    extFaCtx.devArray[dev] = UTF_CPSS_FA_TYPE_98FX910A_CNS;
                break;
                case GT_FA_98FX915A:
                    extFaCtx.devArray[dev] = UTF_CPSS_FA_TYPE_98FX915A_CNS;
                break;
                default:
                    extFaCtx.devArray[dev] = 0;
            }

            extFaCtx.faNumPortsArray[dev] = coreFaDevs[dev]->numOfFports;
            devFaWasFound = GT_TRUE;

            PRV_UTF_LOG2_MAC("[UTF]: prvUtfExtrasInit: FA dev [%d], fa ports [%d]\n",
                             dev,
                             extFaCtx.faNumPortsArray[dev]);
        }
        else
        {
            extFaCtx.devArray[dev] = 0;
        }
#endif /* IMPL_FA */

#ifdef IMPL_XBAR

        if (GT_OK == devXbarCheck(dev))
        {
            switch (coreXbarDevs[dev].devType)
            {
                case GT_XBAR_98FX900A:
                    extXbarCtx.devArray[dev] = UTF_CPSS_XBAR_TYPE_98FX900A_CNS;
                break;
                case GT_XBAR_98FX902A:
                    extXbarCtx.devArray[dev] = UTF_CPSS_XBAR_TYPE_98FX902A_CNS;
                break;
                case GT_XBAR_98FX910A:
                    extXbarCtx.devArray[dev] = UTF_CPSS_XBAR_TYPE_98FX910A_CNS;
                break;
                case GT_XBAR_98FX915A:
                    extXbarCtx.devArray[dev] = UTF_CPSS_XBAR_TYPE_98FX915A_CNS;
                break;
                case GT_XBAR_98FX9010A:
                    extXbarCtx.devArray[dev] = UTF_CPSS_XBAR_TYPE_98FX9010A_CNS;
                break;
                case GT_XBAR_98FX9110:
                    extXbarCtx.devArray[dev] = UTF_CPSS_XBAR_TYPE_98FX9110A_CNS;
                break;
                case GT_XBAR_98FX9210:
                    extXbarCtx.devArray[dev] = UTF_CPSS_XBAR_TYPE_98FX9210A_CNS;
                break;
                default:
                    extXbarCtx.devArray[dev] = 0;
            }

            extXbarCtx.faNumPortsArray[dev] = coreXbarDevs[dev].numOfFports;
            devXbarWasFound = GT_TRUE;

            PRV_UTF_LOG2_MAC("[UTF]: prvUtfExtrasInit: Xbar dev [%d], fa ports [%d]\n",
                             dev,
                             extXbarCtx.faNumPortsArray[dev]);
        }
        else
        {
            extXbarCtx.devArray[dev] = 0;
        }
#endif /* IMPL_XBAR */
    }

    if ((GT_FALSE == devWasFound) && (GT_FALSE == devFaWasFound) && (GT_FALSE == devXbarWasFound))
    {
        PRV_UTF_LOG0_MAC("[UTF]: prvUtfExtrasInit: no active device was found\n");
        st = GT_FAIL;
    }

    return st;
}


/******************************************************************************\
 *                        Packet Processor Implementation                     *
\******************************************************************************/

/*******************************************************************************
* prvUtfNextNotApplicableDeviceReset
*
* DESCRIPTION:
*       Reset device iterator must be called before go over all devices
*
* INPUTS:
*       devPtr                      - iterator thru devices
*       notApplicableFamiliesHandle - handle of non-applicable devices
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   -   iterator initialization OK
*       GT_FAIL -   general failure error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextNotApplicableDeviceReset
(
    INOUT   GT_U8   *devPtr,
    IN      GT_U32  notApplicableFamiliesHandle
)
{
    if (NULL == devPtr)
    {
        return GT_FAIL;
    }

    /* if applicable devices are not specified - error */
    if (UTF_ALL_FAMILY_E == notApplicableFamiliesHandle)
    {
        return GT_FAIL;
    }

    extCtx.currentApplicableFamiliesHandle = UTF_ALL_FAMILY_E & ~(notApplicableFamiliesHandle);
    *devPtr = 0xFF;

    return GT_OK;
}

/*******************************************************************************
* prvUtfNextDeviceReset
*
* DESCRIPTION:
*       Reset device iterator must be called before go over all devices
*
* INPUTS:
*       devPtr      -   iterator thru devices
*       applicableFamiliesHandle - handle of applicable devices
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           -   iterator initialization OK
*       GT_FAIL         -   general failure error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextDeviceReset
(
    INOUT   GT_U8   *devPtr,
    IN      GT_U32  applicableFamiliesHandle
)
{
    if (NULL == devPtr)
    {
        return GT_FAIL;
    }

    /* if applicable devices are not specified - error */
    if (0 == applicableFamiliesHandle)
    {
        return GT_FAIL;
    }

    extCtx.currentApplicableFamiliesHandle = applicableFamiliesHandle;
    *devPtr = 0xFF;

    return GT_OK;
}

/*******************************************************************************
* prvUtfNextDeviceGet
*
* DESCRIPTION:
*       This routine returns next active/non active device id.
*       Call the function until non GT_OK error. Note that
*       iterator must be initialized before.
*
* INPUTS:
*       devPtr      -   device id
*       activeIs    -   set to GT_TRUE for getting active devices
*
* OUTPUTS:
*       None.
*
*       GT_OK           -   Get next device was OK
*       GT_FAIL         -   There is no active devices
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextDeviceGet
(
    INOUT GT_U8     *devPtr,
    IN GT_BOOL      activeIs
)
{
    GT_STATUS st = GT_OK;

    if (NULL == devPtr)
    {
        return GT_FAIL;
    }

    firstTimeTrunkConvertInit();

    st = GT_FAIL;
    (*devPtr)++;


    while (*devPtr < PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        if ((extCtx.devArray[*devPtr] != 0) &&
            ((((extCtx.devArray[*devPtr] & extCtx.currentApplicableFamiliesHandle) == 0)
              ? GT_FALSE : GT_TRUE) == activeIs))
        {
            st = GT_OK;
            break;
        }
        (*devPtr)++;
    }

    return st;
}

/*******************************************************************************
* prvUtfNextPhyPortReset
*
* DESCRIPTION:
*       Reset phy ports iterator must be called before go over all ports
*
* INPUTS:
*       portPtr     -   iterator thru ports
*       dev         -   device id of ports
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           -   iterator initialization OK
*       GT_FAIL         -   general failure error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextPhyPortReset
(
    INOUT GT_U8     *portPtr,
    IN GT_U8        dev
)
{
    GT_STATUS st = GT_OK;

    if ((NULL == portPtr) || (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS))
    {
        return GT_FAIL;
    }

    /* init values for iteration thru phy ports */
    extCtx.devForPhyPorts = dev;
    *portPtr = 0xFF;

    /* device must be active */
    if (0 == extCtx.devArray[dev])
    {
        st = GT_FAIL;
    }
    return st;
}

/*******************************************************************************
* prvUtfNextPhyPortGet
*
* DESCRIPTION:
*       This routine returns next active/non active physical port id.
*       Call the function until non GT_OK error. Note that
*       iterator must be initialized before.
*
* INPUTS:
*       portPtr      -   port id
*       activeIs    -   set to GT_TRUE for getting active ports
*
* OUTPUTS:
*       None.
*
*       GT_OK           -   Get next port was OK
*       GT_FAIL         -   There is no active ports
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextPhyPortGet
(
    INOUT GT_U8     *portPtr,
    IN GT_BOOL      activeIs
)
{
    GT_STATUS st = GT_OK;

    if (NULL == portPtr)
    {
        return GT_FAIL;
    }

    st = GT_FAIL;
    (*portPtr)++;

    while (*portPtr < extCtx.phyNumPortsArray[extCtx.devForPhyPorts])
    {
        GT_BOOL portValidIs = (GT_BOOL)(GT_OK == phyPortCheck(extCtx.devForPhyPorts, *portPtr));
        if (activeIs == portValidIs)
        {
            st = GT_OK;
            break;
        }
        (*portPtr)++;
    }
    return st;
}

/*******************************************************************************
* prvUtfNextVirtPortReset
*
* DESCRIPTION:
*       Reset virtual ports iterator must be called before go over all ports
*
* INPUTS:
*       portPtr     -   iterator thru ports
*       dev         -   device id of ports
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           -   iterator initialization OK
*       GT_FAIL         -   general failure error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextVirtPortReset
(
    INOUT GT_U8     *portPtr,
    IN GT_U8        dev
)
{
    GT_STATUS st = GT_OK;

    if ((NULL == portPtr) || (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS))
    {
        return GT_FAIL;
    }

    /* init values for iteration thru phy ports */
    extCtx.devForVirtPorts = dev;
    *portPtr = 0xFF;

    /* device must be active */
    if (0 == extCtx.devArray[dev])
    {
        st = GT_FAIL;
    }
    return st;
}

/*******************************************************************************
* prvUtfNextVirtPortGet
*
* DESCRIPTION:
*       This routine returns next active/non active virtual port id.
*       Call the function until non GT_OK error. Note that
*       iterator must be initialized before.
*
* INPUTS:
*       portPtr      -   port id
*       activeIs    -   set to GT_TRUE for getting active ports
*
* OUTPUTS:
*       None.
*
*       GT_OK           -   Get next port was OK
*       GT_FAIL         -   There is no active ports
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextVirtPortGet
(
    INOUT GT_U8     *portPtr,
    IN GT_BOOL      activeIs
)
{
    GT_STATUS st = GT_OK;

    if (NULL == portPtr)
    {
        return GT_FAIL;
    }

    st = GT_FAIL;
    (*portPtr)++;

    while (*portPtr < extCtx.virtNumPortsArray[extCtx.devForVirtPorts])
    {
        GT_BOOL portValidIs = (GT_BOOL)(GT_OK == virtPortCheck(extCtx.devForVirtPorts, *portPtr));
        if (activeIs == portValidIs)
        {
            st = GT_OK;
            break;
        }
        (*portPtr)++;
    }
    return st;

}

/*******************************************************************************
* prvUtfPortTypeGet
*
* DESCRIPTION:
*       This routine returns port type.
*
* INPUTS:
*       dev      -  device id
*       port     -  physical port
*
* OUTPUTS:
*       portTypePtr     -   type of port
*
*       GT_OK           -   Get port type was OK
*       GT_BAD_PARAM    -   Invalid port or device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfPortTypeGet
(
    IN GT_U8                    dev,
    IN GT_U8                    port,
    OUT PRV_CPSS_PORT_TYPE_ENT  *portTypePtr
)
{
    GT_STATUS st = GT_OK;

    PRV_CPSS_GEN_PP_CONFIG_STC  *devPtr;

    DEV_CHECK_MAC(dev);
    PRV_CPSS_PHY_PORT_CHECK_MAC(dev, port);
    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(portTypePtr);

    devPtr = PRV_CPSS_PP_MAC(dev);

    /* Get port type */
    *portTypePtr = devPtr->phyPortInfoArray[port].portType;
    return st;
}

/*******************************************************************************
* prvUtfDeviceTypeGet
*
* DESCRIPTION:
*       This routine returns device type. Some tests require device type
*       to make proper function behaviour validation.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       devTypePtr     -   type of device
*
*       GT_OK           -   Get device type was OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfDeviceTypeGet
(
    IN GT_U8                    dev,
    OUT CPSS_PP_DEVICE_TYPE     *devTypePtr
)
{
    GT_STATUS st = GT_OK;

    PRV_CPSS_GEN_PP_CONFIG_STC  *devPtr;

    DEV_CHECK_MAC(dev);

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(devTypePtr);

    devPtr = PRV_CPSS_PP_MAC(dev);

    /* Get device type */
    *devTypePtr = devPtr->devType;
    return st;
}

/*******************************************************************************
* prvUtfDeviceFamilyGet
*
* DESCRIPTION:
*       This routine returns device family. Some tests require device family
*       to make proper function behaviour validation.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       devFamilyPtr     -   family of device
*
*       GT_OK           -   Get device family was OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfDeviceFamilyGet
(
    IN GT_U8                        dev,
    OUT CPSS_PP_FAMILY_TYPE_ENT     *devFamilyPtr
)
{
    DEV_CHECK_MAC(dev);

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(devFamilyPtr);

    /* Get device family */
    *devFamilyPtr = PRV_CPSS_PP_MAC(dev)->devFamily;

    return GT_OK;
}

/*******************************************************************************
* prvUtfMaxMcGroupsGet
*
* DESCRIPTION:
*       This routine returns maximum value for MC groups per device.
*
* INPUTS:
*       dev      -  device id
*
* OUTPUTS:
*       maxMcGroupsPtr     -   maximum value for multicast groups
*
*       GT_OK           -   Get max MC groups was OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfMaxMcGroupsGet
(
    IN GT_U8    dev,
    OUT GT_U16  *maxMcGroupsPtr
)
{
    DEV_CHECK_MAC(dev);

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(maxMcGroupsPtr);

#ifdef CALL_EX_MX_CODE_MAC
    if(PRV_CPSS_EXMX_FAMILY_CHECK_MAC(dev))
    {
        /* ExMx devices */
        *maxMcGroupsPtr = PRV_CPSS_EXMX_DEV_MODULE_CFG_MAC(dev)->bridgeCfg.maxMcGroups;
    }
    else
#endif /* CALL_EX_MX_CODE_MAC */
    {
        *maxMcGroupsPtr = 0;
    }

    return GT_OK;
}

/*******************************************************************************
* prvUtfNumPortsGet
*
* DESCRIPTION:
*       get the number of physical ports in the device
*
* INPUTS:
*       dev         -   device id of ports
*
* OUTPUTS:
*       numOfPortsPtr - (pointer to)number of physical ports
*
* RETURNS:
*       GT_OK           -   get the number of physical ports in the device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNumPortsGet
(
    IN GT_U8        dev,
    OUT GT_U8       *numOfPortsPtr
)
{
    /* device must be active */
    if (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS ||  0 == extCtx.devArray[dev])
    {
        return GT_BAD_PARAM;
    }

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(numOfPortsPtr);

    *numOfPortsPtr = extCtx.phyNumPortsArray[dev];

    return GT_OK;
}

/*******************************************************************************
* prvUtfNumVirtualPortsGet
*
* DESCRIPTION:
*       get the number of Virtual ports in the device
*
* INPUTS:
*       dev         -   device id of ports
*
* OUTPUTS:
*       numOfPortsPtr - (pointer to)number of Virtual ports
*
* RETURNS:
*       GT_OK           -   get the number of virtual ports in the device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNumVirtualPortsGet
(
    IN GT_U8        dev,
    OUT GT_U8       *numOfVirtualPortsPtr
)
{
    /* device must be active */
    if (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS || 0 == extCtx.devArray[dev])
    {
        return GT_BAD_PARAM;
    }

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(numOfVirtualPortsPtr);

    *numOfVirtualPortsPtr = extCtx.virtNumPortsArray[dev];

    return GT_OK;
}

/*******************************************************************************
* prvUtfDeviceRevisionGet
*
* DESCRIPTION:
*       Get revision number of device
*
* INPUTS:
*       dev         -   device id of ports
*
* OUTPUTS:
*       revisionPtr - (pointer to)revision number
*
* RETURNS:
*       GT_OK           -   Get revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfDeviceRevisionGet
(
    IN GT_U8        dev,
    OUT GT_U8       *revisionPtr
)
{
    /* device must be active */
    DEV_CHECK_MAC(dev);

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(revisionPtr);

    *revisionPtr = PRV_CPSS_PP_MAC(dev)->revision;

    return GT_OK;
}

/*******************************************************************************
* prvUtfDeviceTestSupport
*
* DESCRIPTION:
*       check if the device supported for the test type
*
* INPUTS:
*       devNum         -   device id
*       utTestSupport  - bmp of test types support , see :
*                          UTF_CPSS_PP_FAMILY_TWISTC_CNS
*                          UTF_CPSS_PP_FAMILY_TWISTD_CNS
*                          UTF_CPSS_PP_FAMILY_SAMBA_CNS
*                          UTF_CPSS_PP_FAMILY_SALSA_CNS
*                          UTF_CPSS_PP_FAMILY_SOHO_CNS
*                          UTF_CPSS_PP_FAMILY_CHEETAH_CNS
*                          UTF_CPSS_PP_FAMILY_CHEETAH2_CNS
*                          UTF_CPSS_PP_FAMILY_CHEETAH3_CNS
*                          UTF_CPSS_PP_FAMILY_TIGER_CNS
*                          UTF_CPSS_PP_FAMILY_PUMA_CNS
*                          UTF_CPSS_PP_FAMILY_XCAT_CNS
*                          UTF_CPSS_PP_FAMILY_LION_A_CNS
*                          UTF_CPSS_PP_FAMILY_LION_CNS
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_TRUE - the device support the test type
*       GT_FALSE - the device not support the test type
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_BOOL prvUtfDeviceTestSupport
(
    IN GT_U8        devNum,
    IN GT_U32       utTestSupport
)
{
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    /* prepare device iterator */
    st = prvUtfNextDeviceReset(&dev, utTestSupport);
    if(st != GT_OK)
    {
        return GT_FALSE;
    }

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if(dev == devNum)
        {
            return GT_TRUE;
        }
    }

    return GT_FALSE;
}

/*******************************************************************************
* prvUtfDeviceTestNotSupport
*
* DESCRIPTION:
*       Check if the device is not supported for the test type
*
* INPUTS:
*       devNum                 - device id
*       notApplicableDeviceBmp - bmp of not applicable device types
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_TRUE - the device support the test type
*       GT_FALSE - the device not support the test type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL prvUtfDeviceTestNotSupport
(
    IN GT_U8        devNum,
    IN GT_U32       notApplicableDeviceBmp
)
{
    GT_STATUS   st = GT_OK;
    GT_U8       dev;

    /* prepare device iterator */
    st = prvUtfNextNotApplicableDeviceReset(&dev, notApplicableDeviceBmp);
    if(st != GT_OK)
    {
        return GT_FALSE;
    }

    /* 1. Go over all active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        if(dev == devNum)
        {
            return GT_TRUE;
        }
    }

    return GT_FALSE;
}

/*******************************************************************************
* prvUtfSkipTestsNumReset
*
* DESCRIPTION:
*       Reset skip test counter
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvUtfSkipTestsNumReset
(
    GT_VOID
)
{
    prvUtfSkipTestNum = 0;
}

/*******************************************************************************
* prvUtfSkipTestsNumGet
*
* DESCRIPTION:
*       Get total number of skipped tests
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Number of skipped tests
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 prvUtfSkipTestsNumGet
(
    GT_VOID
)
{
    return prvUtfSkipTestNum;
}

/*******************************************************************************
* prvUtfSkipTestsFlagReset
*
* DESCRIPTION:
*       Reset skip test flag
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvUtfSkipTestsFlagReset
(
    GT_VOID
)
{
    prvUtfIsTestSkip = GT_FALSE;
}

/*******************************************************************************
* prvUtfSkipTestsFlagGet
*
* DESCRIPTION:
*       Get current skip test flag
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Skip flag
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_BOOL prvUtfSkipTestsFlagGet
(
    GT_VOID
)
{
    return prvUtfIsTestSkip;
}

/*******************************************************************************
* prvUtfSkipTestsSet
*
* DESCRIPTION:
*       Set skip flag and update number of skipped tests
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvUtfSkipTestsSet
(
    GT_VOID
)
{
    /* mark current test as skipped */
    prvUtfIsTestSkip = GT_TRUE;

    /* increase number of skipped tests */
    prvUtfSkipTestNum++;
}

/*******************************************************************************
* firstTimeTrunkConvertInit
*
* DESCRIPTION:
*       initialize the DB for the trunk Id conversion
*
* INPUTS:
*       trunkIdPtr - (pointer to) the trunkId of the test
*
* OUTPUTS:
*       trunkIdPtr - (pointer to) the trunkId of the CPSS
*
* RETURNS:
*       none
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_VOID firstTimeTrunkConvertInit(GT_VOID)
{
    GT_U32  ii;

    if(firstTimeTrunkConvert == GT_FALSE)
    {
        /* initialization already done */
        return;
    }

    firstTimeTrunkConvert = GT_FALSE;

    /* Add element for each device in system    */
    for(ii = 0; ii < PRV_CPSS_MAX_PP_DEVICES_CNS; ii++)
    {
        if(PRV_CPSS_PP_MAC(ii) == NULL)
        {
            continue;
        }

        if(0 == CPSS_IS_DXCH_FAMILY_MAC(PRV_CPSS_PP_MAC(ii)->devFamily))
        {
            continue;
        }

#ifdef CALL_DXCH_CODE_MAC
        /* use the internal DxCh DB */
        if(PRV_CPSS_DXCH_PP_MAC(ii)->errata.
            info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
                enabled == GT_TRUE)
        {
            prvUtfDxChTrunkWaNeeded = GT_TRUE;
            break;
        }
#else /*CALL_DXCH_CODE_MAC*/
        break;
#endif /*CALL_DXCH_CODE_MAC*/
    }
}

/*******************************************************************************
* prvUtfTrunkConvertFromTestToCpss
*
* DESCRIPTION:
*       convert trunkId from test value
*       to value that the CPSS expect
*
* INPUTS:
*       trunkIdPtr - (pointer to) the trunkId of the test
*
* OUTPUTS:
*       trunkIdPtr - (pointer to) the trunkId of the CPSS
*
* RETURNS:
*       none
*
* COMMENTS:
*
*
*******************************************************************************/
GT_VOID prvUtfTrunkConvertFromTestToCpss
(
   INOUT GT_TRUNK_ID *trunkIdPtr
)
{
    GT_TRUNK_ID newTrunkId;/* new trunkId s*/

    firstTimeTrunkConvertInit();

    if(prvUtfDxChTrunkWaNeeded == GT_FALSE)
    {
        /* this must be after calling firstTimeTrunkConvertInit(...) */

        /* WA not needed */
        return;
    }

    /* WA for trunk Erratum : "wrong trunk id source port
       information of packet to CPU" */

    /* need to do the WA */
    /* convert trunk-Id from test (1..30) to trunkId that fit the WA  (66..124, even) */

    if((*trunkIdPtr) > WA_LAST_TRUNK_CNS)  /* bigger then last (not first) */
    {
        /*no change*/
        return;
    }
    else if((*trunkIdPtr) == 0)
    {
        /*no change*/
        return;
    }

    newTrunkId = (GT_TRUNK_ID)(WA_FIRST_TRUNK_CNS + ((*trunkIdPtr) * 2));
    if(newTrunkId >= 128)
    {
        /* can't convert */
        return;
    }

    (*trunkIdPtr) = newTrunkId;

    return;
}

/*******************************************************************************
* prvUtfTrunkConvertFromCpssToTest
*
* DESCRIPTION:
*       convert trunkId from CPSS value to value that the TEST expect (get)
*
* INPUTS:
*       trunkIdPtr - (pointer to) the trunkId of the CPSS
*
* OUTPUTS:
*       trunkIdPtr - (pointer to) the trunkId of the test
*
* RETURNS:
*       none
*
* COMMENTS:
*
*
*******************************************************************************/
GT_VOID prvUtfTrunkConvertFromCpssToTest
(
   INOUT GT_TRUNK_ID *trunkIdPtr
)
{
    firstTimeTrunkConvertInit();

    if(prvUtfDxChTrunkWaNeeded == GT_FALSE)
    {
        /* this must be after calling firstTimeTrunkConvertInit(...) */

        /* WA not needed */
        return;
    }

    if((*trunkIdPtr) > WA_LAST_TRUNK_CNS)
    {
        /*no change*/
        return;
    }
    else if((*trunkIdPtr) < WA_FIRST_TRUNK_CNS)
    {
        /* can't convert */
        return;
    }
    else if((*trunkIdPtr) == 0)
    {
        /*no change*/
        return;
    }

    (*trunkIdPtr) = (GT_TRUNK_ID)(((*trunkIdPtr) - WA_FIRST_TRUNK_CNS) / 2);

    return;
}

/*******************************************************************************
* prvUtfHwFromSwDeviceNumberGet
*
* DESCRIPTION:
*       Get HW device number from the SW device number
*
* INPUTS:
*       dev         - SW device number
*
* OUTPUTS:
*       hwDevPtr    - (pointer to)HW device number
*
* RETURNS:
*       GT_OK           -   Get revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfHwFromSwDeviceNumberGet
(
    IN GT_U8        dev,
    OUT GT_U8       *hwDevPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
    GT_STATUS rc;   /* return code */

    if(!PRV_CPSS_IS_DEV_EXISTS_MAC(dev))
    {
        /* all cases that use this function need to convert SW to HW value
           and since we not know the 'remote device' we will do no convert
           so HW devNum == SW devNum
        */
        *hwDevPtr = dev;
        return GT_OK;
    }

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(hwDevPtr);

#if (defined CHX_FAMILY)
    rc = cpssDxChCfgHwDevNumGet(dev, hwDevPtr);
#else
    rc = cpssExMxPmPpHwDevNumGet(dev, hwDevPtr);
#endif

    return rc;

#else
    dev = dev;
    hwDevPtr = hwDevPtr;

    return GT_BAD_STATE;
#endif
}

/*******************************************************************************
* prvUtfSwFromHwDeviceNumberGet
*
* DESCRIPTION:
*       Get SW device number from the HW device number
*
* INPUTS:
*       hwDev   - HW device number
*
* OUTPUTS:
*       devPtr  - (pointer to)SW device number
*
* RETURNS:
*       GT_OK           -   Get revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfSwFromHwDeviceNumberGet
(
    IN GT_U8        hwDev,
    OUT GT_U8       *devPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
    GT_STATUS rc;   /* return code */
    GT_U8   i;      /* loop index */
    GT_U8   localHwDev; /* in loop variable for getting HW IDs */

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(devPtr);

    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(PRV_CPSS_PP_MAC(i) == NULL)
        {
            continue;
        }

#if (defined CHX_FAMILY)
        rc = cpssDxChCfgHwDevNumGet(i, &localHwDev);
#else
        rc = cpssExMxPmPpHwDevNumGet(i, &localHwDev);
#endif

        if(rc != GT_OK)
        {
            return rc;
        }

        if( localHwDev == hwDev )
        {
            *devPtr = i;
            return GT_OK;
        }
    }

    *devPtr = hwDev; /* no conversion done */
    return GT_OK;

#else
    hwDev = hwDev;
    devPtr = devPtr;

    return GT_BAD_STATE;
#endif
}
/******************************************************************************/
/* extras internal functions implementation                                   */
/******************************************************************************/

/* Returns GT_OK if device is active */
static GT_STATUS devCheck(IN GT_U8 dev)
{
    return PRV_CPSS_IS_DEV_EXISTS_MAC(dev) ? GT_OK : GT_BAD_PARAM;
}

/* Returns GT_OK if physical port is active */
static GT_STATUS phyPortCheck(IN GT_U8 dev, IN GT_U8 port)
{
    PRV_CPSS_PHY_PORT_CHECK_MAC(dev, port);
    return GT_OK;
}

/* Returns GT_OK if virtual port is active */
static GT_STATUS virtPortCheck(IN GT_U8 dev, IN GT_U8 port)
{
    if(PRV_CPSS_PP_MAC(dev)->functionsSupportedBmp &
       PRV_CPSS_DXCH_FUNCTIONS_SUPPORT_CNS)
    {
        /* DX device ,no virtual ports , only physical ports */
        PRV_CPSS_PHY_PORT_CHECK_MAC(dev, port);
    }
    else
    {
        PRV_CPSS_VIRTUAL_PORT_CHECK_MAC(dev, port);
    }

    return GT_OK;
}

#if (defined EX_FAMILY) || (defined MX_FAMILY)

/*******************************************************************************
* prvUtfExMxMaxNumOfPolicerEntriesGet
*
* DESCRIPTION:
*       This routine returns maximum number of policer entries per device.
*
* INPUTS:
*       dev      -  ExMx device id
*
* OUTPUTS:
*       maxMcGroupsPtr     -   maximum number of policer entries
*
*       GT_OK           -   Get maxNumOfPolicerEntries was OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfExMxMaxNumOfPolicerEntriesGet
(
    IN  GT_U8    dev,
    OUT GT_U32   *maxNumOfPolicerEntriesPtr
)
{
    PRV_CPSS_EXMX_PP_CONFIG_STC  *exMxDevPtr;

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(maxNumOfPolicerEntriesPtr);

    /* check if dev active */
    PRV_CPSS_EXMX_DEV_CHECK_MAC(dev);

    exMxDevPtr = PRV_CPSS_EXMX_PP_MAC(dev);

    *maxNumOfPolicerEntriesPtr = exMxDevPtr->moduleCfg.policerCfg.maxNumOfPolicerEntries;

    return GT_OK;
}

#endif /* (defined EX_FAMILY) || (defined MX_FAMILY) */

/*******************************************************************************
* prvUtfCfgTableNumEntriesGet
*
*       the function return the number of entries of each individual table in
*       the HW.
*
* INPUTS:
*       dev     - physical device number
*       table   - type of the specific table
*
* OUTPUTS:
*       numEntriesPtr - (pointer to) number of entries
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong device number or table type
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfCfgTableNumEntriesGet
(
    IN GT_U8                 dev,
    IN PRV_TGF_CFG_TABLE_ENT table,
    OUT GT_U32               *numEntriesPtr
)
{
#if (defined CHX_FAMILY) || (defined EXMXPM_FAMILY)
    GT_STATUS rc;   /* return code */
#if (defined CHX_FAMILY)
    CPSS_DXCH_CFG_TABLES_ENT dxChTable;
#else
    CPSS_EXMXPM_CFG_TABLES_ENT exMxPmTable;
#endif

    /* device must be active */
    DEV_CHECK_MAC(dev);

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(numEntriesPtr);

#if (defined CHX_FAMILY)
    switch(table)
    {
        case PRV_TGF_CFG_TABLE_VLAN_E: dxChTable = CPSS_DXCH_CFG_TABLE_VLAN_E;
            break;
        case PRV_TGF_CFG_TABLE_VIDX_E: dxChTable = CPSS_DXCH_CFG_TABLE_VIDX_E;
            break;
        case PRV_TGF_CFG_TABLE_FDB_E: dxChTable = CPSS_DXCH_CFG_TABLE_FDB_E;
            break;
        case PRV_TGF_CFG_TABLE_ROUTER_NEXT_HOP_E: dxChTable = CPSS_DXCH_CFG_TABLE_ROUTER_NEXT_HOP_E;
            break;
        case PRV_TGF_CFG_TABLE_MAC_TO_ME_E: dxChTable = CPSS_DXCH_CFG_TABLE_MAC_TO_ME_E;
            break;
        case PRV_TGF_CFG_TABLE_ARP_E: dxChTable = CPSS_DXCH_CFG_TABLE_ARP_E;
            break;
        case PRV_TGF_CFG_TABLE_TUNNEL_START_E: dxChTable = CPSS_DXCH_CFG_TABLE_TUNNEL_START_E;
            break;
        case PRV_TGF_CFG_TABLE_STG_E: dxChTable = CPSS_DXCH_CFG_TABLE_STG_E;
            break;
        case PRV_TGF_CFG_TABLE_ACTION_E: dxChTable = CPSS_DXCH_CFG_TABLE_PCL_ACTION_E;
            break;
        case PRV_TGF_CFG_TABLE_TCAM_E: dxChTable = CPSS_DXCH_CFG_TABLE_PCL_TCAM_E;
            break;
        case PRV_TGF_CFG_TABLE_POLICER_METERS_E: dxChTable = CPSS_DXCH_CFG_TABLE_POLICER_METERS_E;
            break;
        case PRV_TGF_CFG_TABLE_POLICER_COUNTERS_E: dxChTable = CPSS_DXCH_CFG_TABLE_POLICER_BILLING_COUNTERS_E;
            break;
        case PRV_TGF_CFG_TABLE_MLL_DIT_E: dxChTable = CPSS_DXCH_CFG_TABLE_MLL_PAIR_E;
            break;
        case PRV_TGF_CFG_TABLE_ROUTER_LTT_E: dxChTable = CPSS_DXCH_CFG_TABLE_ROUTER_LTT_E;
            break;
        case PRV_TGF_CFG_TABLE_ROUTER_TCAM_E: dxChTable = CPSS_DXCH_CFG_TABLE_ROUTER_TCAM_E;
            break;
        case PRV_TGF_CFG_TABLE_QOS_PROFILE_E: dxChTable = CPSS_DXCH_CFG_TABLE_QOS_PROFILE_E;
            break;
        case PRV_TGF_CFG_TABLE_CNC_E: dxChTable = CPSS_DXCH_CFG_TABLE_CNC_E;
            break;
        case PRV_TGF_CFG_TABLE_CNC_BLOCK_E: dxChTable = CPSS_DXCH_CFG_TABLE_CNC_BLOCK_E;
            break;
        case PRV_TGF_CFG_TABLE_TRUNK_E: dxChTable = CPSS_DXCH_CFG_TABLE_TRUNK_E;
            break;
        default: return GT_BAD_PARAM;
    }
    rc = cpssDxChCfgTableNumEntriesGet(dev, dxChTable, numEntriesPtr);
#else
    switch(table)
    {
        case PRV_TGF_CFG_TABLE_VLAN_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_VLAN_E;
            break;
        case PRV_TGF_CFG_TABLE_VIDX_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_VIDX_E;
            break;
        case PRV_TGF_CFG_TABLE_FDB_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_FDB_E;
            break;
        case PRV_TGF_CFG_TABLE_ROUTER_NEXT_HOP_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_ROUTER_NEXT_HOP_E;
            break;
        case PRV_TGF_CFG_TABLE_MAC_TO_ME_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_MAC_TO_ME_E;
            break;
        case PRV_TGF_CFG_TABLE_ARP_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_ARP_TUNNEL_START_E;
            break;
        case PRV_TGF_CFG_TABLE_TUNNEL_START_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_ARP_TUNNEL_START_E;
            break;
        case PRV_TGF_CFG_TABLE_STG_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_STG_E;
            break;
        case PRV_TGF_CFG_TABLE_ACTION_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_INTERNAL_ACTION_E;
            break;
        case PRV_TGF_CFG_TABLE_TCAM_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_INTERNAL_TCAM_E;
            break;
        case PRV_TGF_CFG_TABLE_POLICER_METERS_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_POLICER_METERS_COUNTERS_E;
            break;
        case PRV_TGF_CFG_TABLE_POLICER_COUNTERS_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_POLICER_METERS_COUNTERS_E;
            break;
        case PRV_TGF_CFG_TABLE_MLL_DIT_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_DIT_E;
            break;
        case PRV_TGF_CFG_TABLE_FDB_LUT_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_FDB_LUT_E;
            break;
        case PRV_TGF_CFG_TABLE_FDB_TCAM_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_FDB_TCAM_E;
            break;
        case PRV_TGF_CFG_TABLE_MPLS_E: exMxPmTable = CPSS_EXMXPM_CFG_TABLE_MPLS_E;
            break;
        default: return GT_BAD_PARAM;
    }
    rc = cpssExMxPmCfgTableNumEntriesGet(dev, exMxPmTable, numEntriesPtr);
#endif

    return rc;

#else
    dev = dev;
    table = table;
    numEntriesPtr = numEntriesPtr;

    return GT_BAD_STATE;
#endif
}

/******************************************************************************\
 *                          Fabric Adapter Implementation                     *
\******************************************************************************/

#ifdef IMPL_FA

/*******************************************************************************
* prvUtfNextFaReset
*
* DESCRIPTION:
*       Reset device fabric adapter iterator. Must be called before go over all
*       device fabric adapters
*
* INPUTS:
*       devPtr                  -   iterator thru devices
*       applicableTypesHandle   - handle of applicable FA devices type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           -   iterator initialization OK
*       GT_FAIL         -   general failure error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextFaReset
(
    INOUT   GT_U8   *devPtr,
    IN      GT_U32  applicableTypesHandle
)
{
    if (NULL == devPtr)
    {
        return GT_FAIL;
    }

    /* if applicable fabric adapter are not specified - error */
    if (0 == applicableTypesHandle)
    {
        return GT_FAIL;
    }

    extFaCtx.currentApplicableTypesHandle = applicableTypesHandle;
    *devPtr = 0xFF;

    return GT_OK;
}

/*******************************************************************************
* prvUtfNextFaGet
*
* DESCRIPTION:
*       This routine returns next active/non active device fabric adapter id.
*       Call the function until non GT_OK error. Note that
*       iterator must be initialized before.
*
* INPUTS:
*       devPtr   - device id
*       activeIs - set to GT_TRUE for getting active FA devices
*
* OUTPUTS:
*       None.
*
*       GT_OK    - Get next device was OK
*       GT_FAIL  - There is no active devices
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextFaGet
(
    INOUT GT_U8     *devPtr,
    IN GT_BOOL      activeIs
)
{
    GT_STATUS st = GT_OK;

    if (NULL == devPtr)
    {
        return GT_FAIL;
    }

    st = GT_FAIL;
    (*devPtr)++;

    while (*devPtr < PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        if ((((extFaCtx.devArray[*devPtr] & extFaCtx.currentApplicableTypesHandle) == 0)
             ? GT_FALSE : GT_TRUE) == activeIs)
        {
            st = GT_OK;
            break;
        }
        (*devPtr)++;
    }

    return st;
}

/*******************************************************************************
* prvUtfNextFaPortReset
*
* DESCRIPTION:
*       Reset fabric ports iterator must be called before go over all ports
*
* INPUTS:
*       portPtr - iterator thru FA ports
*       dev     - FA device id of ports
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - iterator initialization OK
*       GT_FAIL - general failure error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextFaPortReset
(
    INOUT GT_U8     *portPtr,
    IN GT_U8        dev
)
{
    GT_STATUS st = GT_OK;

    if ((NULL == portPtr) || (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS))
    {
        return GT_FAIL;
    }

    /* init values for iteration thru phy ports */
    extFaCtx.devForFaPorts = dev;
    *portPtr = 0xFF;

    /* device must be active */
    if (0 == extFaCtx.devArray[dev])
    {
        st = GT_FAIL;
    }
    return st;
}

/*******************************************************************************
* prvUtfNextFaPortGet
*
* DESCRIPTION:
*       This routine returns next active/non active fabric port id.
*       Call the function until non GT_OK error. Note that
*       iterator must be initialized before.
*
* INPUTS:
*       portPtr  - FA port id
*       activeIs - set to GT_TRUE for getting active ports
*
* OUTPUTS:
*       None.
*
*       GT_OK    - Get next port was OK
*       GT_FAIL  - There is no active ports
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextFaPortGet
(
    INOUT GT_U8     *portPtr,
    IN GT_BOOL      activeIs
)
{
    GT_STATUS st = GT_OK;

    if (NULL == portPtr)
    {
        return GT_FAIL;
    }

    st = GT_FAIL;
    (*portPtr)++;

    while (*portPtr < extFaCtx.faNumPortsArray[extFaCtx.devForFaPorts])
    {
        GT_BOOL portValidIs = (GT_BOOL)(GT_OK == faPortCheck(extFaCtx.devForFaPorts, *portPtr));
        if (activeIs == portValidIs)
        {
            st = GT_OK;
            break;
        }
        (*portPtr)++;
    }
    return st;
}



/*******************************************************************************
* prvUtfFaDeviceTypeGet
*
* DESCRIPTION:
*       This routine returns device fabric adapter type. Some tests require
*       device type to make proper function behaviour validation.
*
* INPUTS:
*       dev          - FA device id
*
* OUTPUTS:
*       devTypePtr   - type of FA device
*
*       GT_OK        - Get device type was OK
*       GT_BAD_PARAM - Invalid device id
*       GT_BAD_PTR   - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfFaDeviceTypeGet
(
    IN GT_U8             dev,
    OUT GT_FA_DEVICE     *devTypePtr
)
{
    GT_STATUS st = GT_OK;

    FA_CHECK_MAC(dev);

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(devTypePtr);

    /* Get device type */
    *devTypePtr = coreFaDevs[dev]->devType;

    return st;
}

/*******************************************************************************
* prvUtfFaRevisionGet
*
* DESCRIPTION:
*       Get revision number of device fabric adapter
*
* INPUTS:
*       dev          - fabric adapter id
*
* OUTPUTS:
*       revisionPtr  - (pointer to)revision number
*
* RETURNS:
*       GT_OK        - Get revision of device OK
*       GT_BAD_PARAM - Invalid device id
*       GT_BAD_PTR   - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfFaDeviceRevisionGet
(
    IN GT_U8        dev,
    OUT GT_U32      *revisionPtr
)
{
    FA_CHECK_MAC(dev);

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(revisionPtr);

    *revisionPtr = coreFaDevs[dev]->devRev;

    return GT_OK;
}

/*******************************************************************************
* prvUtfNumFaPortsGet
*
* DESCRIPTION:
*       get the number of fabric ports in the device
*
* INPUTS:
*       dev           - FA device id of ports
*
* OUTPUTS:
*       numOfPortsPtr - (pointer to)number of fabric ports
*
* RETURNS:
*       GT_OK         - get the number of fabric ports in the device OK
*       GT_BAD_PARAM  - Invalid device id
*       GT_BAD_PTR    - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNumFaPortsGet
(
    IN GT_U8        dev,
    OUT GT_U8       *numOfFaPortsPtr
)
{
    /* device must be active */
    if (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS || 0 == extFaCtx.devArray[dev])
    {
        return GT_BAD_PARAM;
    }

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(numOfFaPortsPtr);

    *numOfFaPortsPtr = extFaCtx.faNumPortsArray[dev];

    return GT_OK;
}

/*******************************************************************************
* prvUtfFaMaxNumPpGet
*
* DESCRIPTION:
*       get number of packet processors in the device.
*
* INPUTS:
*       dev          - FA device id
*
* OUTPUTS:
*       maxNumDevPtr - (pointer to)number of packet processors
*
* RETURNS:
*       GT_OK         - get the number of packet processors in the device OK
*       GT_BAD_PARAM  - Invalid device id
*       GT_BAD_PTR    - Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfFaMaxNumPpGet
(
    IN  GT_U8              dev,
    OUT GT_FA_MAX_NUM_DEV  *maxNumDevPtr
)
{
    /* device must be active */
    if (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS || GT_FALSE == extFaCtx.devArray[dev])
    {
        return GT_BAD_PARAM;
    }

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(maxNumDevPtr);

    *maxNumDevPtr = coreFaDevs[dev]->maxNumDev;

    return GT_OK;
}

/******************************************************************************/
/* extras internal functions implementation                                   */
/******************************************************************************/

/* Returns GT_OK if FA device is active */
static GT_STATUS devFaCheck(IN GT_U8 dev)
{
    return ((dev >= PRV_CPSS_MAX_PP_DEVICES_CNS) || (NULL == coreFaDevs[dev]))
            ? GT_BAD_PARAM : GT_OK;
}

/* Returns GT_OK if fabric port is active */
static GT_STATUS faPortCheck(IN GT_U8 dev, IN GT_U8 port)
{
    if(port >= coreFaDevs[dev]->numOfFports)
    {
        return GT_BAD_PARAM;
    }

    return GT_OK;
}

#endif /* IMPL_FA */


/******************************************************************************\
 *                               Xbar  Implementation                         *
\******************************************************************************/

#ifdef IMPL_XBAR

/*******************************************************************************
* prvUtfNextXbarReset
*
* DESCRIPTION:
*       Reset Xbar device iterator. Must be called before go over all
*       Xbar device
*
* INPUTS:
*       devPtr                  - iterator thru devices
*       applicableTypesHandle   - handle of applicable Xbar devices type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           -   iterator initialization OK
*       GT_FAIL         -   general failure error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextXbarReset
(
    INOUT   GT_U8   *devPtr,
    IN      GT_U32  applicableTypesHandle
)
{
    if (NULL == devPtr)
    {
        return GT_FAIL;
    }

    /* if applicable fabric adapter are not specified - error */
    if (0 == applicableTypesHandle)
    {
        return GT_FAIL;
    }

    extXbarCtx.currentApplicableTypesHandle = applicableTypesHandle;
    *devPtr = 0xFF;

    return GT_OK;
}

/*******************************************************************************
* prvUtfNextXbarGet
*
* DESCRIPTION:
*       This routine returns next active/non active Xbar device id.
*       Call the function until non GT_OK error. Note that
*       iterator must be initialized before.
*
* INPUTS:
*       devPtr   - device id
*       activeIs - set to GT_TRUE for getting active Xbar devices
*
* OUTPUTS:
*       None.
*
*       GT_OK    - Get next device was OK
*       GT_FAIL  - There is no active devices
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextXbarGet
(
    INOUT GT_U8     *devPtr,
    IN GT_BOOL      activeIs
)
{
    GT_STATUS st = GT_OK;

    if (NULL == devPtr)
    {
        return GT_FAIL;
    }

    st = GT_FAIL;
    (*devPtr)++;

    while (*devPtr < PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        if ((((extXbarCtx.devArray[*devPtr] & extXbarCtx.currentApplicableTypesHandle) == 0)
             ? GT_FALSE : GT_TRUE) == activeIs)
        {
            st = GT_OK;
            break;
        }
        (*devPtr)++;
    }

    return st;
}

/*******************************************************************************
* prvUtfNextXbarFaPortReset
*
* DESCRIPTION:
*       Reset fabric ports iterator must be called before go over all ports
*
* INPUTS:
*       portPtr - iterator thru FA ports
*       dev     - Xbar device id of ports
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - iterator initialization OK
*       GT_FAIL - general failure error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextXbarFaPortReset
(
    INOUT GT_U8     *portPtr,
    IN GT_U8        dev
)
{
    GT_STATUS st = GT_OK;

    if ((NULL == portPtr) || (dev >= PRV_CPSS_MAX_PP_DEVICES_CNS))
    {
        return GT_FAIL;
    }

    /* init values for iteration thru phy ports */
    extXbarCtx.devForFaPorts = dev;
    *portPtr = 0xFF;

    /* device must be active */
    if (0 == extXbarCtx.devArray[dev])
    {
        st = GT_FAIL;
    }
    return st;
}

/*******************************************************************************
* prvUtfNextXbarFaPortGet
*
* DESCRIPTION:
*       This routine returns next active/non active fabric port id.
*       Call the function until non GT_OK error. Note that
*       iterator must be initialized before.
*
* INPUTS:
*       portPtr  - FA port id
*       activeIs - set to GT_TRUE for getting active ports
*
* OUTPUTS:
*       None.
*
*       GT_OK    - Get next port was OK
*       GT_FAIL  - There is no active ports
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfNextXbarFaPortGet
(
    INOUT GT_U8     *portPtr,
    IN GT_BOOL      activeIs
)
{
    GT_STATUS st = GT_OK;

    if (NULL == portPtr)
    {
        return GT_FAIL;
    }

    st = GT_FAIL;
    (*portPtr)++;

    while (*portPtr < extXbarCtx.faNumPortsArray[extXbarCtx.devForFaPorts])
    {
        GT_BOOL portValidIs = (GT_BOOL)(GT_OK == xbarFaPortCheck(extXbarCtx.devForFaPorts, *portPtr));
        if (activeIs == portValidIs)
        {
            st = GT_OK;
            break;
        }
        (*portPtr)++;
    }
    return st;
}

/******************************************************************************/
/* extras internal functions implementation                                   */
/******************************************************************************/

/* Returns GT_OK if Xbar device is active */
static GT_STATUS devXbarCheck(IN GT_U8 dev)
{
    return ((dev >= PRV_CPSS_MAX_PP_DEVICES_CNS) || (coreXbarDevs[dev].isValid == GT_FALSE))
            ? GT_BAD_PARAM : GT_OK;
}

/* Returns GT_OK if fabric port for Xbar is active */
static GT_STATUS xbarFaPortCheck(IN GT_U8 dev, IN GT_U8 port)
{
    if(port >= coreXbarDevs[dev].numOfFports)
    {
        return GT_BAD_PARAM;
    }

    return GT_OK;
}

#endif /* IMPL_XBAR */

/*******************************************************************************
* prvUtfCounterRegisterSet
*
* DESCRIPTION:
*       Write counter value to given register
*       in the specific port group in the device
*
* INPUTS:
*       devNum      - The PP to write to.
*       portGroupId - The port group id. relevant only to 'multi-port-groups'
*                     devices. Supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr     - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
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
static GT_STATUS prvUtfCounterRegisterSet
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   value
)
{
    if(fieldLength == 32)
    {
        return prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                    regAddr, value);
    }
    else
    {
        return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                               fieldOffset, fieldLength, value);
    }
}

/*******************************************************************************
* prvUtfPrivatePerPortGroupPerRegisterCounterGet
*
* DESCRIPTION:
*       This Function checks counters in the simulation.
*
* INPUTS:
*       regAddr - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       getCounter - Pointer to get counter function.
*       setCounter - Pointer to set counter function.
*       getPerGroupCounter - Pointer to get per port group counter function.
*       setPerGroupCounter - Pointer to set per port group counter function.
*       getCounterPerReg - Pointer to get counter function.
*       setCounterPerReg - Pointer to set counter function.
*       getPerGroupCounterPerReg - Pointer to get per port group counter function.
*       setPerGroupCounterPerReg - Pointer to set per port group counter function.
*
* OUTPUTS:
*       None.
*
*       GT_OK    - Get next port was OK
*       GT_FAIL  - There is no active ports
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_VOID prvUtfPrivatePerPortGroupPerRegisterCounterGet
(
    IN GT_U32                                           regAddr,
    IN GT_U32                                           fieldOffset,
    IN GT_U32                                           fieldLength,
    IN PRV_UTF_CPSS_GET_COUNTER_FUN                     getCounter,
    IN PRV_UTF_CPSS_SET_COUNTER_FUN                     setCounter,
    IN PRV_UTF_CPSS_GET_PER_GROUP_COUNTER_FUN           getPerGroupCounter,
    IN PRV_UTF_CPSS_SET_PER_GROUP_COUNTER_FUN           setPerGroupCounter,
    IN PRV_UTF_CPSS_GET_COUNTER_PER_REG_FUN             getCounterPerReg,
    IN PRV_UTF_CPSS_SET_COUNTER_PER_REG_FUN             setCounterPerReg,
    IN PRV_UTF_CPSS_GET_PER_GROUP_COUNTER_PER_REG_FUN   getPerGroupCounterPerReg,
    IN PRV_UTF_CPSS_SET_PER_GROUP_COUNTER_PER_REG_FUN   setPerGroupCounterPerReg
)
{
    GT_STATUS               st = GT_OK;         /* return status */
    GT_U8                   devNum;             /* device number */
    GT_U32                  writeValueArr[3];   /* write counter value */
    GT_U32                  counterValue = 0;   /* read counter value */
    GT_U32                  i;                  /* iterator */
    GT_PORT_GROUPS_BMP      portGroupsBmp = 0;  /* port group bitmap */
    GT_U32                  portGroupId;        /* port group id */
    GT_U32                  countMaxValue;      /* counter maximal value */

    if(fieldLength == 32)
    {
        countMaxValue = 0xFFFFFFFF;
    }
    else
    {
        countMaxValue = (1 << fieldLength) - 1;
    }

    writeValueArr[0] = countMaxValue;
    writeValueArr[1] = countMaxValue / 2;
    writeValueArr[2] = 0;

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&devNum, UTF_NONE_FAMILY_E);
    
    /* 1. Go over all active devices. */
    /*   writeValue = countMaxValue , countMaxValue/2, 0
         Write countMaxValue to counter by prvCpssDrvHwPpPortGroupSetRegField /
         prvCpssDrvHwPpWriteRegister.
         Read counter by old (not per port group API) and
         compare value with countMaxValue */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        for(i = 0; i < 3; i++)
        {
            if((setCounter == NULL) && (setCounterPerReg == NULL))
            {
                st = prvUtfCounterRegisterSet(devNum, 0, regAddr, fieldOffset,
                                              fieldLength, writeValueArr[i]);
            }
            else
            {
                if(setCounter != NULL)
                {
                    st = setCounter(devNum, writeValueArr[i]);
                }
                else
                {
                    st = setCounterPerReg(devNum, regAddr, writeValueArr[i]);
                }
            }
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, devNum);

            if(getCounter != NULL)
            {
                st = getCounter(devNum, &counterValue);
            }
            else if(getCounterPerReg != NULL)
            {
                st = getCounterPerReg(devNum, regAddr, &counterValue);
            }
            else
            {
                st = GT_FAIL;
            }
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, devNum);
            UTF_VERIFY_EQUAL1_STRING_MAC(writeValueArr[i], counterValue,
               "get another enable than was set: %d", devNum);
        }

    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&devNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E);
    
    /* 1. Go over all active devices. */
    /* port_group = first_port_group... last_port_group
       writeValue = countMaxValue , countMaxValue/2, 0
       Write writeValue to counter by prvCpssDrvHwPpPortGroupSetRegField for port_group.
       Read counter by new per port group API for port_group and
       compare value with writeValue.
       Read counter by new per port group API for all other existing port groups
       and see that them was not changed.
       Read counter by new per port group API
       for CPSS_PORT_GROUP_UNAWARE_MODE_CNS port groups and
       see that it sum of all port groups. */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        for(portGroupId = 0; portGroupId < CPSS_MAX_PORT_GROUPS_CNS; portGroupId++)
        {
            portGroupsBmp = 1 << portGroupId;

            for(i = 0; i < 3; i++)
            {
                if((setPerGroupCounter == NULL) &&
                   (setPerGroupCounterPerReg == NULL))
                {
                    st = prvUtfCounterRegisterSet(devNum, portGroupId, regAddr,
                                                 fieldOffset,
                                                 fieldLength, writeValueArr[i]);
                }
                else
                {
                    if(setPerGroupCounter != NULL)
                    {
                        st = setPerGroupCounter(devNum, portGroupsBmp,
                                                writeValueArr[i]);
                    }
                    else
                    {
                        st = setPerGroupCounterPerReg(devNum, portGroupsBmp,
                                                     regAddr,
                                                     writeValueArr[i]);
                    }
                }
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, portGroupsBmp);

                if(getPerGroupCounter != NULL)
                {
                    st = getPerGroupCounter(devNum, portGroupsBmp,
                                                   &counterValue);
                }
                else if(getPerGroupCounterPerReg != NULL)
                {
                    st = getPerGroupCounterPerReg(devNum, portGroupsBmp,
                                                  regAddr, &counterValue);
                }
                else
                {
                    st = GT_FAIL;
                }

                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, portGroupsBmp);
                UTF_VERIFY_EQUAL2_STRING_MAC(writeValueArr[i], counterValue,
                "get another enable than was set: %d%d", devNum, portGroupsBmp);

                /* Read counter by new per port group API for all
                  other existing port groups and see that them was not changed. */
                portGroupsBmp = (~portGroupsBmp) & 0xF;
                if(getPerGroupCounter != NULL)
                {
                    st = getPerGroupCounter(devNum, portGroupsBmp, &counterValue);
                }
                else if(getPerGroupCounterPerReg != NULL)
                {
                    st = getPerGroupCounterPerReg(devNum, portGroupsBmp,
                                                   regAddr, &counterValue);
                }
                else
                {
                    st = GT_FAIL;
                }

                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, portGroupsBmp);
                UTF_VERIFY_EQUAL2_STRING_MAC(0, counterValue,
                "get another enable than was set: %d%d", devNum, portGroupsBmp);

                portGroupsBmp = (~portGroupsBmp) & 0xF;

                /* Read counter by new per port group API
                  for CPSS_PORT_GROUP_UNAWARE_MODE_CNS port groups and
                  see that it sum of all port groups. */
                if((setPerGroupCounter == NULL) &&
                   (setPerGroupCounterPerReg == NULL))
                {
                    st = prvUtfCounterRegisterSet(devNum, portGroupId, regAddr,
                                                 fieldOffset,
                                                 fieldLength, writeValueArr[i]);
                }
                else
                {
                    if(setPerGroupCounter != NULL)
                    {
                        st = setPerGroupCounter(devNum, portGroupsBmp,
                                                writeValueArr[i]);
                    }
                    else
                    {
                        st = setPerGroupCounterPerReg(devNum, portGroupsBmp,
                                                     regAddr,
                                                     writeValueArr[i]);
                    }
                }
                UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, devNum, portGroupsBmp);

                if(getPerGroupCounter != NULL)
                {
                    st = getPerGroupCounter(devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            &counterValue);
                }
                else if(getPerGroupCounterPerReg != NULL)
                {
                    st = getPerGroupCounterPerReg(devNum,
                                            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            regAddr, &counterValue);
                }
                else
                {
                    st = GT_FAIL;
                }

                UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, devNum);
                UTF_VERIFY_EQUAL1_STRING_MAC(writeValueArr[i], counterValue,
                "get another enable than was set: %d%d", devNum);

            }
        }
    }

    /* prepare device iterator */
    PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(&devNum, UTF_NONE_FAMILY_E);
    
    /* 1. Go over all active devices. */
    /*   writeValue = countMaxValue , countMaxValue/2, 0
         Write countMaxValue to counter by prvCpssDrvHwPpPortGroupSetRegField /
         prvCpssDrvHwPpWriteRegister.
         Read counter by old (not per port group API) and
         compare value with countMaxValue */
    while(GT_OK == prvUtfNextDeviceGet(&devNum, GT_TRUE))
    {
        for(i = 0; i < 3; i++)
        {
            st = prvUtfCounterRegisterSet(devNum, 0, regAddr, fieldOffset,
                                              fieldLength, writeValueArr[i]);
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, devNum);

            if(getCounter != NULL)
            {
                st = getCounter(devNum, &counterValue);
            }
            else if(getCounterPerReg != NULL)
            {
                st = getCounterPerReg(devNum, regAddr, &counterValue);
            }
            else
            {
                st = GT_FAIL;
            }
            UTF_VERIFY_EQUAL1_PARAM_MAC(GT_OK, st, devNum);
            UTF_VERIFY_EQUAL1_STRING_MAC(writeValueArr[i], counterValue,
               "get another enable than was set: %d", devNum);
        }

    }


}



/*******************************************************************************
* prvUtfPerPortGroupCounterGet
*
* DESCRIPTION:
*       This Function checks counters in the simulation.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       regAddr - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       getCounter - Pointer to get counter function.
*       setCounter - Pointer to set counter function.
*       getPerGroupCounter - Pointer to get per port group counter function.
*       setPerGroupCounter - Pointer to set per port group counter function.
*
* OUTPUTS:
*       None.
*
*       GT_OK    - Get next port was OK
*       GT_FAIL  - There is no active ports
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_VOID prvUtfPerPortGroupCounterGet
(
    IN GT_U32                                   regAddr,
    IN GT_U32                                   fieldOffset,
    IN GT_U32                                   fieldLength,
    IN PRV_UTF_CPSS_GET_COUNTER_FUN             getCounter,
    IN PRV_UTF_CPSS_SET_COUNTER_FUN             setCounter,
    IN PRV_UTF_CPSS_GET_PER_GROUP_COUNTER_FUN   getPerGroupCounter,
    IN PRV_UTF_CPSS_SET_PER_GROUP_COUNTER_FUN   setPerGroupCounter
)
{
    prvUtfPrivatePerPortGroupPerRegisterCounterGet(regAddr, fieldOffset,
                                                 fieldLength, getCounter,
                                                 setCounter,getPerGroupCounter,
                                                 setPerGroupCounter,
                                                 NULL, NULL, NULL, NULL);
}

/*******************************************************************************
* prvUtfPerPortGroupCounterPerRegGet
*
* DESCRIPTION:
*       This Function checks counters in the simulation for
*       specific register adddress.
*
* INPUTS:
*       regAddr - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       getCounterPerReg - Pointer to get counter function.
*       setCounterPerReg - Pointer to set counter function.
*       getPerGroupCounterPerReg - Pointer to get per port group counter function.
*       setPerGroupCounterPerReg - Pointer to set per port group counter function.
*
* OUTPUTS:
*       None.
*
*       GT_OK    - Get next port was OK
*       GT_FAIL  - There is no active ports
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_VOID prvUtfPerPortGroupCounterPerRegGet
(
    IN GT_U32                                           regAddr,
    IN GT_U32                                           fieldOffset,
    IN GT_U32                                           fieldLength,
    IN PRV_UTF_CPSS_GET_COUNTER_PER_REG_FUN             getCounterPerReg,
    IN PRV_UTF_CPSS_SET_COUNTER_PER_REG_FUN             setCounterPerReg,
    IN PRV_UTF_CPSS_GET_PER_GROUP_COUNTER_PER_REG_FUN   getPerGroupCounterPerReg,
    IN PRV_UTF_CPSS_SET_PER_GROUP_COUNTER_PER_REG_FUN   setPerGroupCounterPerReg
)
{
    prvUtfPrivatePerPortGroupPerRegisterCounterGet(regAddr, fieldOffset,
                                                   fieldLength,
                                                   NULL, NULL, NULL, NULL,
                                                   getCounterPerReg,
                                                   setCounterPerReg,
                                                   getPerGroupCounterPerReg,
                                                   setPerGroupCounterPerReg);
}

/*******************************************************************************
* prvUtfIsPbrModeUsed
*
* DESCRIPTION:
*       This routine returns GT_TRUE if PBR mode used. GT_FALSE otherwise.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       None
*
*       GT_TRUE  - PBR mode used
*       GT_FALSE - otherwise
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_BOOL prvUtfIsPbrModeUsed()
{
    GT_U32 value;

    return ((appDemoDbEntryGet("usePolicyBasedRouting", &value) == GT_OK)
            && (value == GT_TRUE)) ?  GT_TRUE : GT_FALSE;
}

/*******************************************************************************
* prvUtfDeviceCoreClockGet
*
* DESCRIPTION:
*       Get core clock in Herz of device
*
* INPUTS:
*       dev         -   device id of ports
*
* OUTPUTS:
*       coreClockPtr - (pointer to)core clock in Herz
*
* RETURNS:
*       GT_OK           -   Get revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfDeviceCoreClockGet
(
    IN GT_U8          dev,
    OUT GT_U32       *coreClockPtr
)
{
    /* device must be active */
    DEV_CHECK_MAC(dev);

    /* check for null pointer */
    CPSS_NULL_PTR_CHECK_MAC(coreClockPtr);

    switch (PRV_CPSS_PP_MAC(dev)->coreClock)
    {
        case 222: *coreClockPtr = 222222222; break;
        case 167: *coreClockPtr = 166666667; break;
        default: *coreClockPtr = PRV_CPSS_PP_MAC(dev)->coreClock * 1000000;
            break;
    }
    return GT_OK;
}




