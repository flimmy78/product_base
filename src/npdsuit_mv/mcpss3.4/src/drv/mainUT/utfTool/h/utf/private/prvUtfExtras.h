/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvUtfExtras.h
*
* DESCRIPTION:
*       Internal header which defines API for helpers functions
*       which are specific for cpss unit testing.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvUtfExtrash
#define __prvUtfExtrash

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <utf/private/prvUtfHelpers.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#ifdef IMPL_FA
#include <cpssFa/generic/gen/private/gtCoreFaStructs.h>
#endif /* IMPL_FA */

#ifdef IMPL_XBAR
#include <cpssXbar/generic/private/gtCoreXbarStructs.h>
#endif /* IMPL_XBAR */

/******************************************************************************\
 *                              Packet Processor defines                      *
\******************************************************************************/

/* pp families - corresponding bits     */
/* used by iterator through devices     */
#define UTF_CPSS_PP_FAMILY_TWISTC_CNS   BIT_0
#define UTF_CPSS_PP_FAMILY_TWISTD_CNS   BIT_1
#define UTF_CPSS_PP_FAMILY_SAMBA_CNS    BIT_2
#define UTF_CPSS_PP_FAMILY_SALSA_CNS    BIT_3
#define UTF_CPSS_PP_FAMILY_SOHO_CNS     BIT_4
#define UTF_CPSS_PP_FAMILY_TIGER_CNS    BIT_5
#define UTF_CPSS_PP_FAMILY_PUMA_CNS     BIT_6

/*
 * Typedef: enumeration UTF_PP_FAMILY_BIT_ENT
 *
 * Description: The PP family representing bit.
 *
 * Fields:
 *      UTF_TWISTC_E      - (EXMX)   Twist-C family devices
 *      UTF_TWISTD_E      - (EXMX)   Twist-D family devices
 *      UTF_SAMBA_E       - (EXMX)   Samba family devices
 *      UTF_TIGER_E       - (EXMX)   Tiger family devices
 *      UTF_SALSA_E       - (DXSAL)  Salsa family devices
 *      UTF_PUMA_E        - (EXMXPM) Puma family devices
 *      UTF_CH1_E         - (DXCH)   Cheetah family devices
 *      UTF_CH1_DIAMOND_E - (DXCH)   Cheetah Diamond family devices
 *      UTF_CH2_E         - (DXCH)   Cheetah 2 family devices
 *      UTF_CH3_E         - (DXCH)   Cheetah 3 family devices
 *      UTF_XCAT_E        - (DXCH)   xCat family devices
 *      UTF_LION_E        - (DXCH)   Lion family devices
 *      UTF_XCAT2_E       - (DXCH)   xCat2 family devices
 *
 */
typedef enum
{
    UTF_NONE_FAMILY_E = 0,
    UTF_TWISTC_E      = BIT_0,
    UTF_TWISTD_E      = BIT_1,
    UTF_SAMBA_E       = BIT_2,
    UTF_TIGER_E       = BIT_3,
    UTF_SALSA_E       = BIT_4,
    UTF_PUMA_E        = BIT_5,
    UTF_CH1_E         = BIT_6,
    UTF_CH1_DIAMOND_E = BIT_7, 
    UTF_CH2_E         = BIT_8,
    UTF_CH3_E         = BIT_9,
    UTF_XCAT_E        = BIT_10,
    UTF_LION_E        = BIT_11,
    UTF_XCAT2_E       = BIT_12,
    UTF_ALL_FAMILY_E  = BIT_13 - 1
} UTF_PP_FAMILY_BIT_ENT;

/* widely-used family sets                              */
/* used by UTs while call prvUtfDeviceReset function    */

#define UTF_ALL_FAMILIES_SET_CNS    (0xFFFFFFFF)

#define UTF_EXMX_FAMILY_SET_CNS     (UTF_CPSS_PP_FAMILY_TWISTC_CNS | \
                                    UTF_CPSS_PP_FAMILY_TWISTD_CNS |  \
                                    UTF_CPSS_PP_FAMILY_SAMBA_CNS |   \
                                    UTF_CPSS_PP_FAMILY_TIGER_CNS)

#define UTF_EXMXTG_FAMILY_SET_CNS   UTF_CPSS_PP_FAMILY_TIGER_CNS

#define UTF_DXSAL_FAMILY_SET_CNS    UTF_CPSS_PP_FAMILY_SALSA_CNS

#define UTF_EXMXPM_FAMILY_SET_CNS   UTF_CPSS_PP_FAMILY_PUMA_CNS

/*
 * Description: macro to check and reset device iterator
 *
 * Parameters:
 *      _devNum          - device number iterator
 *      _notAppFamilyBmp - bitmap of not applicable device's families
 *
 * Comments: this macro uses default error validation
 *
 */
#define PRV_TGF_NOT_APPLIC_DEV_RESET_MAC(_devNum, _notAppFamilyBmp)            \
    do                                                                         \
    {                                                                          \
        GT_STATUS   _rc = GT_OK;                                               \
                                                                               \
        _rc = prvUtfNextNotApplicableDeviceReset(_devNum, (_notAppFamilyBmp)); \
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, _rc);                               \
    } while(0)

/*
 * Description: macro to check and reset device iterator
 *
 * Parameters:
 *      _devNum          - device number iterator
 *      _rc              - return code that needs to be used
 *      _notAppFamilyBmp - bitmap of not applicable device's families
 *
 * Comments: this macro uses speciric return code and error validation
 *
 */
#define PRV_TGF_NOT_APPLIC_DEV_RC_RESET_MAC(_devNum, _rc, _notAppFamilyBmp)    \
    {                                                                          \
        _rc = prvUtfNextNotApplicableDeviceReset(_devNum, (_notAppFamilyBmp)); \
        if (GT_OK != _rc)                                                      \
        {                                                                      \
            PRV_UTF_LOG1_MAC("prvUtfNextNotApplicableDeviceReset FAILED, rc = [%d]", _rc);  \
                                                                               \
            return _rc;                                                        \
        }                                                                      \
    }

/*
 * Description: macro to check if the device bmp is not supported
 *
 * Parameters:
 *      _devNum          - device number
 *      _notAppFamilyBmp - bitmap of not applicable device's families
 *
 * Comments: this macro uses speciric return code and error validation
 *
 */
#define PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(_devNum, _notAppFamilyBmp)         \
    if(GT_FALSE == prvUtfDeviceTestNotSupport(_devNum, (_notAppFamilyBmp)))    \
    {                                                                          \
        /* set skip flag and update skip tests num */                          \
        prvUtfSkipTestsSet();                                                  \
                                                                               \
        return ;                                                               \
    }

/* Macro to get max number of virtual ports */
#define UTF_GET_MAX_VIRT_PORTS_NUM_MAC(devNum) \
    (PRV_CPSS_PP_MAC(devNum)->numOfVirtPorts)

/* Macro to get max number of physical ports */
#define UTF_GET_MAX_PHY_PORTS_NUM_MAC(devNum) \
    (PRV_CPSS_PP_MAC(devNum)->numOfPorts)

/* init values for CPSS_INTERFACE_INFO_STC
   use maximal values to catch bugs */
#define UTF_INTERFACE_INIT_VAL_CNS    {0x7FFFFFFF, {0xFF,0xFF}, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF, 0xFFFF, 0xFFFFFFFF}

/* Invalid enum array size */
extern GT_U32 utfInvalidEnumArrSize;

/* Array with invalid enum values to check.
   Using GT_U32 type instead of GT_32 can be helpful to find some bugs */
extern GT_U32 utfInvalidEnumArr[];


/* Macro to mask ports bmp with existing physical ports */
#define UTF_MASK_PORTS_BMP_WITH_EXISTING_MAC(devNum,currPortsBmp)             \
    currPortsBmp.ports[0] &= PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[0]; \
    currPortsBmp.ports[1] &= PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[1]

/*
 * typedef: enum PRV_TGF_CFG_TABLE_ENT
 *
 * Description: Enumeration for the type of tables that the devices hold..
 *
 * Enumerations:
 *      PRV_TGF_CFG_TABLE_VLAN_E - table type represent the VLAN table.
 *      PRV_TGF_CFG_TABLE_VIDX_E - table type represent the VIDX (multicast groups) table.
 *      PRV_TGF_CFG_TABLE_FDB_E - table type represent the FDB table.
 *      PRV_TGF_CFG_TABLE_ROUTER_NEXT_HOP_E - table type represent the router next hop table.
 *      PRV_TGF_CFG_TABLE_MAC_TO_ME_E - table type represent the Mac to Me table.
 *      PRV_TGF_CFG_TABLE_ARP_E - table type represent the ARP/Tunnel Start table (ARP entries).
 *      PRV_TGF_CFG_TABLE_TUNNEL_START_E - table type represent the ARP/Tunnel Start table (tunnel start entries).
 *      PRV_TGF_CFG_TABLE_STG_E - table type represent the STG (spanning tree groups) table.
 *      PRV_TGF_CFG_TABLE_ACTION_E - table type represent the action table.
 *      PRV_TGF_CFG_TABLE_TCAM_E - table type represent the TCAM table.
 *      PRV_TGF_CFG_TABLE_POLICER_METERS_E - table type represent the policer metering table.
 *      PRV_TGF_CFG_TABLE_POLICER_COUNTERS_E table type represent the policer counters table.
 *      PRV_TGF_CFG_TABLE_ROUTER_LTT_E - table type represent the router lookup translation table (LTT).
 *      PRV_TGF_CFG_TABLE_ROUTER_TCAM_E - table type represent the Router Tcam table.
 *      PRV_TGF_CFG_TABLE_MLL_DIT_E - table type represent the MLL Pair\Downstream Interface Table table.
 *      PRV_TGF_CFG_TABLE_QOS_PROFILE_E - table type represent the QOS profile table.
 *      PRV_TGF_CFG_TABLE_CNC_E t - table type represent the centralized counters (CNC) table.
 *      PRV_TGF_CFG_TABLE_CNC_BLOCK_E - table type represent CNC block (the number of conters per CNC block).
 *      PRV_TGF_CFG_TABLE_TRUNK_E - table type represent trunk table.
 *      PRV_TGF_CFG_TABLE_FDB_LUT_E - table type represent the FDB lookup table.
 *      PRV_TGF_CFG_TABLE_FDB_TCAM_E - table type represent the FDB TCAM table.
 *      PRV_TGF_CFG_TABLE_MPLS_E - table type represent the MPLS Table.
 *
 */
typedef enum
{
    PRV_TGF_CFG_TABLE_VLAN_E,
    PRV_TGF_CFG_TABLE_VIDX_E,
    PRV_TGF_CFG_TABLE_FDB_E,
    PRV_TGF_CFG_TABLE_ROUTER_NEXT_HOP_E,
    PRV_TGF_CFG_TABLE_MAC_TO_ME_E,
    PRV_TGF_CFG_TABLE_ARP_E,
    PRV_TGF_CFG_TABLE_TUNNEL_START_E,
    PRV_TGF_CFG_TABLE_STG_E,
    PRV_TGF_CFG_TABLE_ACTION_E,
    PRV_TGF_CFG_TABLE_TCAM_E,
    PRV_TGF_CFG_TABLE_POLICER_METERS_E,
    PRV_TGF_CFG_TABLE_POLICER_COUNTERS_E,
    PRV_TGF_CFG_TABLE_MLL_DIT_E,

    PRV_TGF_CFG_TABLE_ROUTER_LTT_E,
    PRV_TGF_CFG_TABLE_ROUTER_TCAM_E,
    PRV_TGF_CFG_TABLE_QOS_PROFILE_E,
    PRV_TGF_CFG_TABLE_CNC_E,
    PRV_TGF_CFG_TABLE_CNC_BLOCK_E,
    PRV_TGF_CFG_TABLE_TRUNK_E,

    PRV_TGF_CFG_TABLE_FDB_LUT_E,
    PRV_TGF_CFG_TABLE_FDB_TCAM_E,
    PRV_TGF_CFG_TABLE_DIT_E,
    PRV_TGF_CFG_TABLE_MPLS_E

} PRV_TGF_CFG_TABLE_ENT;

/******************************************************************************\
 *                            Fabric Adapters defines                         *
\******************************************************************************/

#ifdef IMPL_FA

/* fa types - corresponding bits            */
/* used by iterator through fabric adapters */
#define UTF_CPSS_FA_TYPE_98FX900A_CNS   BIT_0
#define UTF_CPSS_FA_TYPE_98FX902A_CNS   BIT_1
#define UTF_CPSS_FA_TYPE_98FX910A_CNS   BIT_2
#define UTF_CPSS_FA_TYPE_98FX915A_CNS   BIT_3

/* widely-used type sets                            */
/* used by UTs while call prvUtfFaReset function    */
#define UTF_ALL_FA_TYPES_SET_CNS    (UTF_CPSS_FA_TYPE_98FX900A_CNS | \
                                    UTF_CPSS_FA_TYPE_98FX902A_CNS | \
                                    UTF_CPSS_FA_TYPE_98FX910A_CNS | \
                                    UTF_CPSS_FA_TYPE_98FX915A_CNS)
#endif /* IMPL_FA */


/******************************************************************************\
 *                                 Xbar defines                               *
\******************************************************************************/

#ifdef IMPL_XBAR

/* xbar types - corresponding bits          */
/* used by iterator through xbar            */
#define UTF_CPSS_XBAR_TYPE_98FX900A_CNS   BIT_0
#define UTF_CPSS_XBAR_TYPE_98FX902A_CNS   BIT_1
#define UTF_CPSS_XBAR_TYPE_98FX910A_CNS   BIT_2
#define UTF_CPSS_XBAR_TYPE_98FX915A_CNS   BIT_3
#define UTF_CPSS_XBAR_TYPE_98FX9010A_CNS  BIT_4
#define UTF_CPSS_XBAR_TYPE_98FX9110A_CNS  BIT_5
#define UTF_CPSS_XBAR_TYPE_98FX9210A_CNS  BIT_6

/* widely-used type sets                            */
/* used by UTs while call prvUtfFaReset function    */
#define UTF_ALL_XBAR_TYPES_SET_CNS  (UTF_CPSS_XBAR_TYPE_98FX900A_CNS | \
                                    UTF_CPSS_XBAR_TYPE_98FX902A_CNS  | \
                                    UTF_CPSS_XBAR_TYPE_98FX910A_CNS  | \
                                    UTF_CPSS_XBAR_TYPE_98FX915A_CNS  | \
                                    UTF_CPSS_XBAR_TYPE_98FX9010A_CNS | \
                                    UTF_CPSS_XBAR_TYPE_98FX9110A_CNS | \
                                    UTF_CPSS_XBAR_TYPE_98FX9210A_CNS)

/* External usage environment parameters */
extern CORE_XBAR_CONFIG  coreXbarDevs[PRV_CPSS_MAX_PP_DEVICES_CNS];

/* Macro to get max number of fports */
#define UTF_GET_MAX_FPORTS_NUM_MAC(devNum) \
    (coreXbarDevs[devNum].numOfFports)

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
);


/******************************************************************************\
 *                            Packet Processor UT API                         *
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
);

/*******************************************************************************
* prvUtfNextDeviceReset
*
* DESCRIPTION:
*       Reset device iterator must be called before go over all devices
*
* INPUTS:
*       devPtr      -   iterator thru devices
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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
    IN  GT_U8                        dev,
    OUT CPSS_PP_FAMILY_TYPE_ENT      *devFamilyPtr
);

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
);

/*******************************************************************************
* prvUtfNumPortsGet
*
* DESCRIPTION:
*       Get number of physical ports in the device
*
* INPUTS:
*       dev         -   device id of ports
*
* OUTPUTS:
*       numOfPortsPtr - (pointer to)number of physical ports
*
* RETURNS:
*       GT_OK           -   Get the number of physical ports in the device OK
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
);

/*******************************************************************************
* prvUtfNumVirtualPortsGet
*
* DESCRIPTION:
*       Get number of Virtual ports in the device
*
* INPUTS:
*       dev         -   device id of ports
*
* OUTPUTS:
*       numOfPortsPtr - (pointer to)number of Virtual ports
*
* RETURNS:
*       GT_OK           -   Get the number of virtual ports in the device OK
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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
void prvUtfTrunkConvertFromTestToCpss
(
   INOUT GT_TRUNK_ID *trunkIdPtr
);

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
void prvUtfTrunkConvertFromCpssToTest
(
   INOUT GT_TRUNK_ID *trunkIdPtr
);

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
);

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
);

/* Don't do casting to GT_TRUNK_ID for trunkId. it must be of this type already */
#define CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(_trunkId) \
   prvUtfTrunkConvertFromTestToCpss(&_trunkId)

#define CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(_trunkId) \
   prvUtfTrunkConvertFromCpssToTest(&_trunkId)

/* flag to state that the DXCH (xcat) devices need the WA of trunk-Id conversions */
extern GT_BOOL prvUtfDxChTrunkWaNeeded;


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
);

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
);

/******************************************************************************\
 *                            Fabric Adapter UT API                           *
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
);

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
);

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
);

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
);

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
);

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
    IN GT_U8       dev,
    OUT GT_U32     *revisionPtr
);

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
);

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
);

#endif /* IMPL_FA */


/******************************************************************************\
 *                                Xbar UT API                                 *
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
*       devPtr                  -   iterator thru devices
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
);

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
);

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
);

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
);

#endif /* IMPL_XBAR */


typedef GT_STATUS (*PRV_UTF_CPSS_GET_COUNTER_FUN)
(
    IN  GT_U8                   devNum,
    OUT GT_U32                  *counterValue
);

typedef GT_STATUS (*PRV_UTF_CPSS_SET_COUNTER_FUN)
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  counterValue
);

typedef GT_STATUS (*PRV_UTF_CPSS_GET_PER_GROUP_COUNTER_FUN)
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    OUT GT_U32                  *counterValue
);

typedef GT_STATUS (*PRV_UTF_CPSS_SET_PER_GROUP_COUNTER_FUN)
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  counterValue
);

typedef GT_STATUS (*PRV_UTF_CPSS_GET_COUNTER_PER_REG_FUN)
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  regAddr,
    OUT GT_U32                  *counterValue
);

typedef GT_STATUS (*PRV_UTF_CPSS_SET_COUNTER_PER_REG_FUN)
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  regAddr,
    IN  GT_U32                  counterValue
);

typedef GT_STATUS (*PRV_UTF_CPSS_GET_PER_GROUP_COUNTER_PER_REG_FUN)
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  regAddr,
    OUT GT_U32                  *counterValue
);

typedef GT_STATUS (*PRV_UTF_CPSS_SET_PER_GROUP_COUNTER_PER_REG_FUN)
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  regAddr,
    IN  GT_U32                  counterValue
);

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
);

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
);



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
GT_BOOL prvUtfIsPbrModeUsed
(
    GT_VOID
);

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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvUtfExtrash */

