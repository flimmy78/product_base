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
* cpssDxChTrunk.c
*
* DESCRIPTION:
*       API definitions for 802.3ad Link Aggregation (Trunk) facility
*       CPSS - DxCh
*
*       States and Modes :
*       A. Application can work in one of 2 modes:
*           1. use "high level" trunk manipulations set of APIs - Mode "HL"
*           2. use "low level" trunk HW tables/registers set of APIs - MODE "LL"
*
*       B. Using "high level" trunk manipulations set of APIs - "High level"
*           Mode.
*           In this mode the CPSS synchronize all the relevant trunk
*           tables/registers, implement some WA for trunks Errata.
*           1. Create trunk
*           2. Destroy trunk
*           3. Add/remove member to/from trunk
*           4. Enable/disable member in trunk
*           5. Add/Remove port to/from "non-trunk" entry
*           6. set/unset designated member for Multi-destination traffic and
*              traffic sent to Cascade Trunks
*        C. Using "low level" trunk HW tables/registers set of APIs- "Low level"
*           Mode.
*           In this mode the CPSS allow direct access to trunk tables/registers.
*           In this mode the Application required to implement the
*           synchronization between the trunk tables/registers and to implement
*           the WA to some of the trunk Errata.
*
*           1. Set per port the trunkId
*           2. Set per trunk the trunk members , number of members
*           3. Set per trunk the Non-trunk local ports bitmap
*           4. Set per Index the designated local ports bitmap
*
*         D. Application responsibility is not to use a mix of using API from
*            those 2 modes
*            The only time that the Application can shift between the 2 modes,
*            is only when there are no trunks in the device.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
/* get DxCh Trunk types */
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
/* get DxCh Trunk private types */
#include <cpss/dxCh/dxChxGen/trunk/private/prvCpssDxChTrunk.h>

#include <cpssCommon/cpssFormatConvert.h>

/* offset between entries in the non trunk table */
#define CHEETAH_TRUNK_NON_TRUNK_TABLE_OFFSET_CNS        0x00000010

/* offset between entries in the designated ports trunk table */
#define CHEETAH_TRUNK_DESIGNATED_PORTS_TABLE_OFFSET_CNS 0x00000010

/* offset between entries in the num of members in trunk table for cheetah */
#define CHEETAH_TRUNK_NUM_MEMBERS_TABLE_OFFSET_CNS      0x00001000

/* offset between entries in the num of members in trunk table for cheetah2 */
#define CHEETAH2_TRUNK_NUM_MEMBERS_TABLE_OFFSET_CNS     0x00000004

/* check the index in the designated ports table  */
#define HW_DESIGNATED_PORTS_ENTRY_INDEX_CHECK_MAC(devNum,index) \
    if((index) >= PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numOfDesignatedTrunkEntriesHw)   \
        return GT_BAD_PARAM

/* HW check that the trunkId is valid index to trunk members table */
#define HW_TRUNK_ID_AS_INDEX_IN_TRUNK_MEMBERS_CHECK_MAC(devNum,index) \
    if(((index) > PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numTrunksSupportedHw) || ((index) == 0))\
        return GT_BAD_PARAM;                                          \
    if(0 == XCAT_DEVICE_AND_TRUNK_ID_CHECK_WA_TRAFFIC_TO_CPU_MAC(devNum,index))\
        return GT_BAD_PARAM/* the device in WA mode but the trunkId not in according to rules */


/* HW check that the trunkId is valid index to non-trunk ports table */
/* no mistake -- check the trunkId against numTrunksSupportedHw and not against
                numNonTrunkEntriesHw
                value 0 is valid !!!
*/
#define HW_TRUNK_ID_AS_INDEX_IN_NON_TRUNK_PORTS_CHECK_MAC(devNum,index)    \
    if((index) > PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numTrunksSupportedHw)\
        return GT_BAD_PARAM;                                               \
    if(0 == XCAT_DEVICE_AND_TRUNK_ID_CHECK_WA_TRAFFIC_TO_CPU_MAC(devNum,index)) \
        return GT_BAD_PARAM /* the device in WA mode but the trunkId not in according to rules */

/* trunk id that use as index in the "non-trunk" table ,
    HW uses this entry when getting traffic from source trunk that had trunkId
    bigger than what HW support */
#define NULL_TRUNK_ID_CNS   0

#define REGISTER_ADDR_IN_NUM_MEMBERS_TRUNK_TABLE_MAC(devNum,trunkId,regAddrPtr) \
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)        \
    {                                                                         \
        /* cheetah */                                                         \
        *regAddrPtr =                                                         \
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.trunkTable +       \
            (((trunkId-1)>>3) * CHEETAH_TRUNK_NUM_MEMBERS_TABLE_OFFSET_CNS);  \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        /* cheetah2 and above */                                              \
        *regAddrPtr =                                                         \
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.trunkTable +       \
            (((trunkId-1)>>3) * CHEETAH2_TRUNK_NUM_MEMBERS_TABLE_OFFSET_CNS); \
    }

/* #define DUMP_TRUNK_UP_TO_MEMBER */

#ifdef ASIC_SIMULATION
    #define DUMP_TRUNK_UP_TO_MEMBER
#endif /*ASIC_SIMULATION*/


/* invalid trunk member */
#define INVALID_TRUNK_MEMBER_VALUE_CNS  0xff

/* indicate that a UP is in use , NOTE: the index in the array of upToUseArr[]
is not the UP , but the value of the element is the UP */
#define UP_IN_USE_CNS  0xff


/*******************************************************************************
*   typedef: struct EXTRA_TRUNK_ENTRY_STC
*
*   Description:
*      Structure represent the DB of the DXCH CPSS for single trunk
*      INFO "PER TRUNK"
*
*   Fields:
*      errata - info related to errata
*          info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU -
*              info related to erratum described in
*              PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E
*           upToMemberMap[] - array of mapping from UP (user priority) to
*               the trunk members array -->
*               index into this array is the UP (user priority) , and the value
*               is the trunk member .
*           upToUseArr - array of next UP to use --> for new members.
*               the array holds UP values to represent 'free UP' to use for new
*               trunk member , when the value is UP_IN_USE_CNS meaning that
*               the UP is in use.
*               when a member is 'removed' from the trunk we need to 'free' the
*               UP that is associated with it.(set the UP index with value UP_IN_USE_CNS)
*           nextUpIndexToUse - index in array upToUseArr[]
*               index to be used for new members , the index is
*               incremented by one , for every new member (in cyclic manner)
*               NOTE: this index is not changed when 'remove' a member.
*
*
*******************************************************************************/
typedef struct{
    struct{
        struct{
            CPSS_TRUNK_MEMBER_STC   upToMemberMap[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS];
            GT_U8  upToUseArr[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS];
            GT_U32 nextUpIndexToUse;
        }info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU;
    }errata;
}EXTRA_TRUNK_ENTRY_STC;

/* max number of trunk in dxch devices */
#define DXCH_MAX_TRUNKS_CNS         128

/* number of trunks needed for the WA for "wrong trunk id source port information of packet to CPU" */
/*
    /2 , because only even trunks are valid for the WA
*/

#define XCAT_NUM_TRUNKS_FOR_WA_WRONG_SOURCE_PORT_TO_CPU  \
    (1 + ((XCAT_LAST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU - \
     XCAT_FIRST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU)/2))

/* macro to check the trunkId needed WA for :
   "wrong trunk id source port information of packet to CPU"
   note: trunks 126,127 not need the WA since not traffic to CPU
*/
#ifndef __AX_PLATFORM__
#define XCAT_TRUNKID_CHECK_FOR_WA_WRONG_SOURCE_PORT_TO_CPU_MAC(_trunkId) (_trunkId) ? 1:1  /*slove bug AXSSZFI-186 */
#else
#define XCAT_TRUNKID_CHECK_FOR_WA_WRONG_SOURCE_PORT_TO_CPU_MAC(_trunkId) \
    (((_trunkId) & 1) ? 0 :/* only even value */                         \
     ((_trunkId) < /*minimum value */                                    \
        XCAT_FIRST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU) ? 0 :        \
     ((_trunkId) > /*maximum value*/                                     \
        XCAT_LAST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU) ? 0 :         \
        1)
#endif
/* check that device need the WA but the trunkId not according to the rules */
/* this check must be only after checking that trunkId <= numTrunksSupportedHw */
/*
    MACRO returns:
    0 - the index for that device is NOT valid
    1 - the index for that device is valid
*/
#define XCAT_DEVICE_AND_TRUNK_ID_CHECK_WA_TRAFFIC_TO_CPU_MAC(_devNum,index) \
    (((PRV_CPSS_DEV_TRUNK_INFO_MAC(_devNum)->initDone == GT_FALSE) ? 0 : \
      (0 == PRV_CPSS_DXCH_CHECK_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_MAC(_devNum))? 0 : \
      (1 == XCAT_TRUNKID_CHECK_FOR_WA_WRONG_SOURCE_PORT_TO_CPU_MAC(index)) ? 0 : \
      (((index) & 0x7e) == 0x7e) ? 0 : /* for devices with "real" 127 trunks support trunks 126,127 for cascading */ \
      ((index) == 0) ? 0 : /* index 0 is valid for all devices */ \
      1/* failed all checks */        \
     ) ?                                \
     0 : /* macro return failure */     \
     1)  /* macro return success */


/*******************************************************************************
*   typedef: struct DXCH_TRUNK_SPECIFIC_INFO_STC
*
*   Description:
*      Structure represent the DB of the DXCH CPSS for trunks in that device
*      that needed for specific extra info
*      INFO "PER DEVICE"
*
*   Fields:
*      extraTrunkInfoArray[] - info per trunk for the WA of "wrong trunk id source
*           port information of packet to CPU"
*
*
*******************************************************************************/
typedef struct{
    EXTRA_TRUNK_ENTRY_STC   extraTrunkInfoArray[
        XCAT_NUM_TRUNKS_FOR_WA_WRONG_SOURCE_PORT_TO_CPU];
}DXCH_TRUNK_SPECIFIC_INFO_STC;


/*******************************************************************************
*   typedef: struct FIELD_INFO_STC
*
*   Description:
*      Structure represent the DB of the fields of the CRC hash mask entry
*
*   Fields:
*      offsetOfSwFieldForStcStart - offset of SW field from start of CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC
*                 value is in bytes.
*      startBit - the start bit in the entry
*      length   - number of bits in field
*      offset   - offset from bit 0 in the value
*      needToSwap - is this field value need to be swapped
*
*******************************************************************************/
typedef struct{
    int  offsetOfSwFieldForStcStart;
    GT_U32  startBit;
    GT_U32  length;
    GT_U32  offset;
    GT_BOOL needToSwap;
}FIELD_INFO_STC;

/*offset of SW field from start of CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC value is in bytes. */
#define HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(_field)  \
    CPSS_FORMAT_CONVERT_STRUCT_MEMBER_OFFSET_MAC(CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC,_field)

/* the DB about the fields in the CRC hash mask entry.
    index to this table is according to fields in
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC */
static FIELD_INFO_STC hashMaskCrcFieldsInfo[]=
{
     {HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(l4DstPortMaskBmp   ),2, 2 ,0 ,GT_TRUE } /* l4DstPortMaskBmp;    */
    ,{HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(l4SrcPortMaskBmp   ),0, 2 ,0 ,GT_TRUE } /* l4SrcPortMaskBmp;    */
    ,{HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(ipv6FlowMaskBmp    ),4, 3 ,0 ,GT_TRUE } /* ipv6FlowMaskBmp;     */
    ,{HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(ipDipMaskBmp       ),7, 16,0 ,GT_TRUE } /* ipDipMaskBmp;        */
    ,{HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(ipSipMaskBmp       ),23,16,0 ,GT_TRUE } /* ipSipMaskBmp;        */
    ,{HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(macDaMaskBmp       ),39,6 ,0 ,GT_TRUE } /* macDaMaskBmp;        */
    ,{HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(macSaMaskBmp       ),45,6 ,0 ,GT_TRUE } /* macSaMaskBmp;        */
    ,{HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(mplsLabel0MaskBmp  ),51,3 ,0 ,GT_TRUE } /* mplsLabel0MaskBmp;   */
    ,{HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(mplsLabel1MaskBmp  ),54,3 ,0 ,GT_TRUE } /* mplsLabel1MaskBmp;   */
    ,{HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(mplsLabel2MaskBmp  ),57,3 ,0 ,GT_TRUE } /* mplsLabel2MaskBmp;   */
    ,{HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(localSrcPortMaskBmp),60,1 ,0 ,GT_FALSE} /* localSrcPortMaskBmp; */
    ,{HASH_MASK_CRC_FIELD_SW_DISTANCE_MAC(udbsMaskBmp        ),61,9 ,14,GT_FALSE} /* udbsMaskBmp;         */
};

/* number of words in the entry of : hash mask (for CRC hash) table */
#define HASH_MASK_CRC_ENTRY_NUM_WORDS_CNS       3

/* check index to the hash mask crc table of the device */
#define HASH_MASK_CRC_INDEX_CHECK_MAC(_devNum,_index)   \
    if((_index) >=                                      \
       PRV_CPSS_DXCH_PP_MAC(_devNum)->accessTableInfoPtr[PRV_CPSS_DXCH_LION_TABLE_TRUNK_HASH_MASK_CRC_E].maxNumOfEntries)\
        return GT_BAD_PARAM

/*max number of Designated Trunk Entries*/
#define MAX_DESIGNATED_TRUNK_ENTRIES_NUM_CNS    64

/*******************************************************************************
* prvCpssDxChTrunkTrafficToCpuWaCpuPortEnableSet
*
* DESCRIPTION:
*       enable/disable the needed for the WA for the CPU/cascade port that may
*       represent the pass to the CPU
*       WA of "wrong trunk id source port information of packet to CPU" erratum
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number of the device that we set member on.
*       portNum   - the CPU/cascade port that may represent the pass to the CPU
*       enable    - enable/disable the settings
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success , or device not need the WA
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTrunkTrafficToCpuWaCpuPortEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_U8                   portNum,
    IN  GT_BOOL                 enable
)
{
    GT_U32  value;/*value to write to register */
    GT_U32  portOffset = portNum * 4;/* offset that related to the port */
    GT_U32  bit; /* bit index in the register */
    GT_U32  additionalInfoBmp; /* bitmap for additional WA info */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(0 ==
        PRV_CPSS_DXCH_CHECK_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_MAC(devNum))
    {
        return  GT_OK;
    }

    if(enable == GT_TRUE)
    {
        value = 0x00001ff9;
    }
    else
    {
        value = 0;
    }

    additionalInfoBmp = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
        additionalInfoBmp;

    if (((additionalInfoBmp >> 1) & 0x1) == 0)
    {
        rc = prvCpssDrvHwPpWriteRegister(devNum,0x0f80c000 + portOffset,value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else if (((additionalInfoBmp >> 2) & 0x1) == 0)
    {
        rc = prvCpssDrvHwPpWriteRegister(devNum,
                                    0x0f808000 + PRV_CPSS_HW_DEV_NUM_MAC(devNum)*256 + portOffset,value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        rc = prvCpssDrvHwPpWriteRegister(devNum,
                                    0x0f80A000 + PRV_CPSS_HW_DEV_NUM_MAC(devNum)*256 + portOffset,value);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(portNum == CPSS_CPU_PORT_NUM_CNS)
    {
        bit = 31;
    }
    else
    {
        bit = portNum;
    }

    rc = prvCpssDrvHwPpSetRegField(devNum,0x0f800010,bit,1,BOOL2BIT_MAC(enable));
    if (rc != GT_OK)
    {
        return rc;
    }

    if(enable == GT_TRUE)
    {
        rc = prvCpssDrvHwPpSetRegField(devNum,0x0f800004,bit,1,0);/*set 0 regardless to enable parameter */
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChTrunkTrafficToCpuWaUpInit
*
* DESCRIPTION:
*       initialize the UP (user priority) to port settings,needed WA for :
*  "wrong trunk id source port information of packet to CPU"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number of the device that we set member on.
*       additionalInfoBmp   - bitmap for additional WA info
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTrunkTrafficToCpuWaUpInit
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  additionalInfoBmp
)
{
    GT_STATUS   rc;
    GT_U32      port;           /* port iterator */
    GT_U32      portOffset   ;  /* offset from register address */
    GT_U32      portOffset_1 ;  /* offset #1 from register address */

    rc = prvCpssDrvHwPpSetRegField(devNum,0x0e000208,1,4,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (((additionalInfoBmp >> 1) & 0x1) == 0)
    {
        rc = prvCpssDrvHwPpSetRegField(devNum,0x0f800000,7,1,1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else if (((additionalInfoBmp >> 2) & 0x1) == 0)
    {
        rc = prvCpssDrvHwPpSetRegField(devNum,0x0f800000,0,19,0x40082);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        rc = prvCpssDrvHwPpSetRegField(devNum,0x0f800000,0,19,0x50082);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = prvCpssDxChTrunkTrafficToCpuWaCpuPortEnableSet(devNum,
                CPSS_CPU_PORT_NUM_CNS,
                GT_TRUE);/*enable*/
    if(rc != GT_OK)
    {
        return rc;
    }

    for(port = 0 ; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts ; port++)
    {
        portOffset      = 0x40 * port;
        portOffset_1    = 0x10 * port;

        if (additionalInfoBmp & 0x1)
        {
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040020 + portOffset,(63-port) << 11);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040024 + portOffset,0x00080000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0b0008 + portOffset_1,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0b000c + portOffset_1,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040000 + portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040004 + portOffset,0x00030000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040020 + portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040024 + portOffset,0x00030000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0b0000 + portOffset_1,0x00000800);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0b0004 + portOffset_1,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040018 + portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e04001c + portOffset,0x00050000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040010 + portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040014 + portOffset,0x00050000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040008 + portOffset,0x00000000);/*devNum << 27*/
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e04000c + portOffset,0x00050000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040000 + portOffset,0x000007ff + (port << 11));
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040004 + portOffset,0x00050000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040038 + portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e04003c + portOffset,0x00083000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040030 + portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040034 + portOffset,0x00080000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e040028 + portOffset,0x00000000); /*(31-devNum) << 27*/
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e04002c + portOffset,0x00080000); /*srcTrg<<1*/
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffc0 - portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffc4 - portOffset,0x00030000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffe0 - portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffe4 - portOffset,0x00030000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0bbff0 - portOffset_1,0x00000800);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0bbff4 - portOffset_1,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0bbff8 - portOffset_1,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0bbffc - portOffset_1,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffd8 - portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffdc - portOffset,0x00050000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffd0 - portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffd4 - portOffset,0x00050000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffc8 - portOffset,0x00000000);/*devNum << 27*/
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffcc - portOffset,0x00050000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffc0 - portOffset,0x000007ff + (port << 11));
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffc4 - portOffset,0x00050000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06fff8 - portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06fffc - portOffset,0x00083000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06fff0 - portOffset,0x00000000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06fff4 - portOffset,0x00080000);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffe8 - portOffset,0x00000000);/*(31-devNum) << 27*/
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffec - portOffset,0x00080000);/*srcTrg<<1*/
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffe0 - portOffset,(63-port) << 11);
            if (rc != GT_OK)
            {
                return rc;
            }
            rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e06ffe4 - portOffset,0x00080000);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.trunksNum <
        XCAT_LAST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU)
    {
        /* actual number of trunks reduced to 30 !!! even though we 'set' 124 trunks */
        PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.trunksNum =
            XCAT_LAST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU;
            /* set 124 for the generic engine only */
    }

    return GT_OK;
}

/*******************************************************************************
* trunkTrafficToCpuWaUpForPortEnableSet
*
* DESCRIPTION:
*       bind/unbind the UP (user priority) to port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number of the device that we set member on.
*       enable - GT_TRUE - bind the port to new UP
*                GT_FALSE - unbind the port from the UP
*       up   - the user priority to bind to the port
*               (relevant only when enable == GT_TRUE)
*       memberPtr - (pointer to) the trunk member
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkTrafficToCpuWaUpForPortEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_BOOL                 enable,
    IN  GT_U8                   up,
    IN  CPSS_TRUNK_MEMBER_STC   *memberPtr
)
{
    GT_U32  portOffset = 0x10 * memberPtr->port;  /* offset that related to the port */
    GT_U32  value = 0x800;/* value to write to the register */
    GT_U32 additionalInfoBmp; /* bitmap for additional WA info */
    GT_STATUS   rc;             /* return code */

    if(memberPtr->device != PRV_CPSS_HW_DEV_NUM_MAC(devNum))
    {
        /* not relevant to this device */
        return GT_OK;
    }

    if(0 == PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, memberPtr->port))
    {
        /* not need treatment */
        return GT_OK;
    }

    if(enable)
    {
        value |= 1<<23;
        value |= (up&7) << 18;
    }

    additionalInfoBmp = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                        info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
                        additionalInfoBmp;

    if ((additionalInfoBmp & 0x1) == 0)
    {
        rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0bbff0 - portOffset,     value);
        if (rc != GT_OK)
        {
            return rc;
    }
        rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0bbff4 - portOffset,0x00000000);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0bbff8 - portOffset,0x00000000);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0bbffc - portOffset,0x00000000);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0b0000 + portOffset,     value);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0b0004 + portOffset,0x00000000);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0b0008 + portOffset,0x00000000);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = prvCpssDrvHwPpWriteRegister(devNum,0x0e0b000c + portOffset,0x00000000);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDxChTrunkTrafficToCpuWaUpToMemberConvert
*
* DESCRIPTION:
*       convert the UP (user priority) to trunk member {dev,port}
*       Needed for WA of "wrong trunk id source port information of packet to CPU" erratum
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number of the device that connected to CPU port/SDMA.
*       dsaTagInfoPtr - (pointer to) the DSA tag info parsed from the DSA tag.
*
* OUTPUTS:
*       dsaTagInfoPtr - (pointer to) the DSA tag info with fixed info regarding
*                       "wrong trunk id source port information of packet to CPU"
*
* RETURNS:
*       GT_OK   - on success , the DSA tag was 'fixed' (updated)
*       GT_NO_CHANGE - there is no need for 'convert'
*       GT_DSA_PARSING_ERROR - No valid mapping between the UP and trunk member.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTrunkTrafficToCpuWaUpToMemberConvert
(
    IN  GT_U8   devNum,
    INOUT CPSS_DXCH_NET_DSA_PARAMS_STC  *dsaTagInfoPtr
)
{
    GT_U8   dev;
    GT_TRUNK_ID   trunkId;/* trunkId to use for the WA */
    DXCH_TRUNK_SPECIFIC_INFO_STC *extraTrunksInfoPtr;/*(pointer to) specific trunk db of the device*/
    EXTRA_TRUNK_ENTRY_STC   *currWaTrunkInfoPtr;/*(pointer to)extra trunk info*/
    CPSS_DXCH_NET_DSA_TO_CPU_STC  *toCpuDsaTagInfoPtr;/*'to cpu' DSA tag format*/
    GT_U32  index;/*index in the specific info trunks */
    CPSS_TRUNK_MEMBER_STC   *memberPtr;/*(pointer to) the trunk member */

    if(dsaTagInfoPtr->dsaType != CPSS_DXCH_NET_DSA_CMD_TO_CPU_E)
    {
        return GT_NO_CHANGE;
    }

    toCpuDsaTagInfoPtr = &dsaTagInfoPtr->dsaInfo.toCpu;

    /* the trunkId is supposed to be (portNum * 2) */
    trunkId = (GT_TRUNK_ID)(toCpuDsaTagInfoPtr->interface.portNum * 2);

    /* check that the trunkId is in valid range */
    if(0 ==
        XCAT_TRUNKID_CHECK_FOR_WA_WRONG_SOURCE_PORT_TO_CPU_MAC(trunkId))
    {
        /* the WA not needed */
        return GT_NO_CHANGE;
    }

    if(toCpuDsaTagInfoPtr->srcIsTrunk == GT_TRUE)
    {
        return GT_NO_CHANGE;
    }

    if(toCpuDsaTagInfoPtr->isEgressPipe == GT_TRUE)
    {
        return GT_NO_CHANGE;
    }

    if(dsaTagInfoPtr->commonParams.vpt >= PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS)
    {
        return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(toCpuDsaTagInfoPtr->devNum))
    {
        /* the src device is unknown --> assume is same device,revision as this
            <devNum> */
        dev = devNum;
    }
    else
    {
        dev = toCpuDsaTagInfoPtr->devNum;
    }


    if(0 ==
        PRV_CPSS_DXCH_CHECK_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_MAC(dev))
    {
        return  GT_NO_CHANGE;
    }

    index = (trunkId - XCAT_FIRST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU)
            / 2;

    if (index >= XCAT_NUM_TRUNKS_FOR_WA_WRONG_SOURCE_PORT_TO_CPU)
    {
        /* the value of toCpuDsaTagInfoPtr->interface.portNum is too big */
        return GT_BAD_PARAM;
    }

    extraTrunksInfoPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
            extraTrunksInfoPtr;
    currWaTrunkInfoPtr = &extraTrunksInfoPtr->extraTrunkInfoArray[index];
    /* convert VPU to trunk member Index */
    memberPtr = &currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.upToMemberMap[
            dsaTagInfoPtr->commonParams.vpt];

    if(memberPtr->port == INVALID_TRUNK_MEMBER_VALUE_CNS)
    {
        /* DSA parsing error as result of trunk WA port mapping error */
        return GT_DSA_PARSING_ERROR;
    }

    if(memberPtr->device != toCpuDsaTagInfoPtr->devNum)
    {
        /* DSA parsing error as result of wrong trunk WA port mapping error */
        return GT_DSA_PARSING_ERROR;
    }

    /* replace the trunk member with the info in the DSA */
    toCpuDsaTagInfoPtr->interface.portNum =  memberPtr->port;
    /* no need to change toCpuDsaTagInfoPtr->devNum because it should be
       unchanged */

    return GT_OK;
}

/*******************************************************************************
* trunkTrafficToCpuWaUpToMemberAdd
*
*
* DESCRIPTION:
*       add new(or existing) member to the trunk --> needed for the DB of
*       map 'UP to trunk member'
*       Needed for WA of "wrong trunk id source port information of packet to CPU" erratum
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number of the device that we set member on.
*       trunkId   - the trunk Id to which we add member
*       memberPtr - (pointer to) the trunk member
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success or the WA not needed
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_STATE - unable to add a new member , because no free UP to use
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkTrafficToCpuWaUpToMemberAdd
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    IN  CPSS_TRUNK_MEMBER_STC   *memberPtr
)
{
    DXCH_TRUNK_SPECIFIC_INFO_STC *extraTrunksInfoPtr;/*(pointer to) specific trunk db of the device*/
    EXTRA_TRUNK_ENTRY_STC   *currWaTrunkInfoPtr;/*(pointer to)extra trunk info*/
    GT_U32  index;/*index in the specific info trunks */
    CPSS_TRUNK_MEMBER_STC   *currentMemberPtr;/* (pointer to)current trunk member in DB*/
    GT_U32  nextUpIndexToUse;/* next index to use for new member */
    GT_U8   up;/* UP (use priority) to use */

    index = (trunkId - XCAT_FIRST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU)
            / 2;
    extraTrunksInfoPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
            extraTrunksInfoPtr;
    currWaTrunkInfoPtr = &extraTrunksInfoPtr->extraTrunkInfoArray[index];

    currentMemberPtr = &currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.upToMemberMap[
            0];

    /* look for the existence of the member */
    for(index = 0 ; index < PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS;
        index++,currentMemberPtr++)
    {
        if(currentMemberPtr->port == memberPtr->port &&
           currentMemberPtr->device == memberPtr->device)
        {
            /* member already in DB --> nothing to do more */
            return GT_OK;
        }
    }

    nextUpIndexToUse = currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.nextUpIndexToUse;

    up = currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.upToUseArr[
        nextUpIndexToUse];

    if(up == UP_IN_USE_CNS)
    {
        return GT_BAD_STATE;
    }

    currentMemberPtr = &currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.upToMemberMap[
            up];

    if(currentMemberPtr->port != INVALID_TRUNK_MEMBER_VALUE_CNS)
    {
        /* not empty member ?! */
        return GT_BAD_STATE;
    }

    *currentMemberPtr = *memberPtr;

    currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.nextUpIndexToUse =
            (nextUpIndexToUse + 1) % PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS;

    currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.upToUseArr[
        nextUpIndexToUse] = UP_IN_USE_CNS;

    return trunkTrafficToCpuWaUpForPortEnableSet(devNum,GT_TRUE,up,memberPtr);
}

/*******************************************************************************
* trunkTrafficToCpuWaUpToMemberRemove
*
*
* DESCRIPTION:
*       remove existing member from the trunk --> needed for
*       the DB of map 'UP to trunk member'
*       Needed for WA of "wrong trunk id source port information of packet to CPU" erratum
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number of the device that we set member on.
*       trunkId   - the trunk Id to which we remove member
*       memberPtr - (pointer to) the trunk member
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_STATE             - member not found
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkTrafficToCpuWaUpToMemberRemove
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    IN  CPSS_TRUNK_MEMBER_STC   *memberPtr
)
{
    DXCH_TRUNK_SPECIFIC_INFO_STC *extraTrunksInfoPtr;/*(pointer to) specific trunk db of the device*/
    EXTRA_TRUNK_ENTRY_STC   *currWaTrunkInfoPtr;/*(pointer to)extra trunk info*/
    GT_U32  index;/*index in the specific info trunks */
    CPSS_TRUNK_MEMBER_STC   *currentMemberPtr;/* (pointer to)current trunk member in DB*/
    GT_U32  freeUpIndex;/* the UP index to get the free UP */
    GT_U32  ii;

    index = (trunkId - XCAT_FIRST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU)
            / 2;
    extraTrunksInfoPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
            extraTrunksInfoPtr;
    currWaTrunkInfoPtr = &extraTrunksInfoPtr->extraTrunkInfoArray[index];

    currentMemberPtr = &currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.upToMemberMap[
            0];

    /* look for the existence of the member */
    for(index = 0 ; index < PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS;
        index++,currentMemberPtr++)
    {
        if(currentMemberPtr->port == memberPtr->port &&
           currentMemberPtr->device == memberPtr->device)
        {
            /* member found in DB */
            break;
        }
    }

    if(index == PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS)
    {
        /* member not in the trunk  --> should not happen */
        return GT_BAD_STATE;
    }

    /* invalidate the member for that UP */
    currentMemberPtr->port = INVALID_TRUNK_MEMBER_VALUE_CNS;
    currentMemberPtr->device = INVALID_TRUNK_MEMBER_VALUE_CNS;

    freeUpIndex = (currWaTrunkInfoPtr->errata.
                    info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.
                    nextUpIndexToUse - 1) %
                    PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS;

    /* look for index to free the UP to it */
    for(ii = 0 ; ii < PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS ;
        ii++ , freeUpIndex = ((freeUpIndex - 1)%PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS))
    {
        if(currWaTrunkInfoPtr->errata.
            info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.upToUseArr[
            freeUpIndex] != UP_IN_USE_CNS)
        {
            freeUpIndex = ((freeUpIndex + 1)%PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS);
            break;
        }
    }

    if(ii == PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS)
    {
        /* all UPs are used */
        freeUpIndex = currWaTrunkInfoPtr->errata.
            info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.
            nextUpIndexToUse;
    }

    /* restore the 'free UP' to last to use */
    currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.upToUseArr[
        freeUpIndex] = (GT_U8)index;

    return trunkTrafficToCpuWaUpForPortEnableSet(devNum,GT_FALSE,0,memberPtr);
}

/*******************************************************************************
* trunkTrafficToCpuWaTrunkTableEntrySet
*
* DESCRIPTION:
*       Set the trunk table entry --> needed for the DB of map 'UP to trunk member'
*       Needed for WA of "wrong trunk id source port information of packet to CPU" erratum
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id
*       numMembers - num of enabled members in the trunk
*       membersArray - array of enabled members of the trunk
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - successes or WA no needed
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS trunkTrafficToCpuWaTrunkTableEntrySet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    IN  GT_U32                  numMembers,
    IN  CPSS_TRUNK_MEMBER_STC   membersArray[]
)
{
    GT_STATUS   rc;
    DXCH_TRUNK_SPECIFIC_INFO_STC *extraTrunksInfoPtr;/*(pointer to) specific trunk db of the device*/
    EXTRA_TRUNK_ENTRY_STC   *currWaTrunkInfoPtr;/*(pointer to)extra trunk info*/
    GT_U32  index;/*index in the specific info trunks */
    CPSS_TRUNK_MEMBER_STC   *currentMemberPtr;/* (pointer to)current trunk member in DB*/
    GT_U32  ii;/* index in membersArray[] */
    CPSS_TRUNK_MEMBER_STC   tmpMember;/*tmp trunk member*/
    GT_U32                  numOfDisabledMembers = PRV_CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS; /* number of members as retrieved from trunk DB*/
    GT_U32                  numTotalMembers = 0;
    CPSS_TRUNK_MEMBER_STC   disabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS]; /* array of db disabledmembers of the trunk */
    CPSS_TRUNK_MEMBER_STC   totalMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS]; /* array of enabled & disabled members of the trunk */


    if(0 == PRV_CPSS_DXCH_CHECK_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_MAC(devNum) ||
       0 == XCAT_TRUNKID_CHECK_FOR_WA_WRONG_SOURCE_PORT_TO_CPU_MAC(trunkId))
    {
        /* the WA not needed */
        return GT_OK;
    }

    /* get disabled trunk members */
    rc = cpssDxChTrunkDbDisabledMembersGet(devNum,trunkId,
                                           &numOfDisabledMembers,
                                           &disabledMembersArray[0]);
    if(rc == GT_OK)
    {
       /* if first enable member is NULL port and there are disabled trunk's
       port members, then WA ignore the null port (the array will occupy
       only the disabled members)*/
       if (!((membersArray[0].port == PRV_CPSS_DXCH_NULL_PORT_NUM_CNS) &&
             (numOfDisabledMembers != 0)))
       {
           for(ii = 0; ii < numMembers ;ii++)
           {
               totalMembersArray[ii] = membersArray[ii];
               numTotalMembers++;
           }
       }

       for(ii = 0; ii < numOfDisabledMembers ;ii++)
       {
           totalMembersArray[numTotalMembers] = disabledMembersArray[ii];
           numTotalMembers++;
       }

       if( CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS < numTotalMembers)
       {
           return GT_OUT_OF_RANGE;
       }

    }
    else /* the DB not supported at this stage or at all */
    {
        /* copy the members array to local array */
        cpssOsMemCpy(totalMembersArray, membersArray ,
                     sizeof(CPSS_TRUNK_MEMBER_STC)*numMembers);
        numTotalMembers = numMembers;
    }
    index = (trunkId - XCAT_FIRST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU)
            / 2;
    extraTrunksInfoPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
            extraTrunksInfoPtr;
    currWaTrunkInfoPtr = &extraTrunksInfoPtr->extraTrunkInfoArray[index];

    currentMemberPtr = &currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.upToMemberMap[
            0];

    /* remove 'old members' that exists in DB but not in the new settings */
    for(index = 0 ; index < PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS;
        index++,currentMemberPtr++)
    {
        if(currentMemberPtr->port == INVALID_TRUNK_MEMBER_VALUE_CNS)
        {
            /* empty space */
            continue;
        }

        for(ii = 0 ; ii < numTotalMembers; ii++)
        {
            if(totalMembersArray[ii].port == currentMemberPtr->port &&
               totalMembersArray[ii].device == currentMemberPtr->device )
            {
                break;
            }
        }

        if(ii == numTotalMembers)
        {
            /* copy the member to remove to temporary variable */
            tmpMember = *currentMemberPtr; /* must not supply currentMemberPtr
                        because this will affect behavior of trunkTrafficToCpuWaUpToMemberRemove(...) */

            /* this member should be removed from the WA */
            rc = trunkTrafficToCpuWaUpToMemberRemove(devNum,trunkId,&tmpMember);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    for(ii = 0 ; ii < numTotalMembers; ii++)
    {
        /* add the members */
        rc = trunkTrafficToCpuWaUpToMemberAdd(devNum,trunkId,
                                              &totalMembersArray[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    return GT_OK;
}


#ifdef DUMP_TRUNK_UP_TO_MEMBER
/*******************************************************************************
* cpssDxChTrunkTrafficToCpuWaUpToMemberDump
*
*       Function Relevant mode : ALL modes
*
* DESCRIPTION:
*       a debug function to dump the DB of 'UP to trunk members' for
*       specific trunk id.
*       Needed for WA of "wrong trunk id source port information of packet to CPU" erratum
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number
*       trunkId   - the trunk Id to which we dump the info
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkTrafficToCpuWaUpToMemberDump
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId
)
{
    DXCH_TRUNK_SPECIFIC_INFO_STC *extraTrunksInfoPtr;/*(pointer to) specific trunk db of the device*/
    EXTRA_TRUNK_ENTRY_STC   *currWaTrunkInfoPtr;/*(pointer to)extra trunk info*/
    GT_U32  index;/*index in the specific info trunks */
    CPSS_TRUNK_MEMBER_STC   *currentMemberPtr;/* (pointer to)current trunk member in DB*/
    GT_U32  ii;/* iterator */
    GT_U8   *nextUpToUsePtr;/*(pointer to)next Up To Use in array upToUseArr[]*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(0 ==
        PRV_CPSS_DXCH_CHECK_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_MAC(devNum))
    {
        cpssOsPrintf(" this device not implementing the WA \n");

        return GT_OK;
    }

    /* check that the trunkId is in valid range */
    if(0 ==
        XCAT_TRUNKID_CHECK_FOR_WA_WRONG_SOURCE_PORT_TO_CPU_MAC(trunkId))
    {
        cpssOsPrintf(" trunk ID [%d] not in WA range [%d]-[%d] or NOT EVEN \n",
                    trunkId,
                    XCAT_FIRST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU,
                    XCAT_LAST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU);
        return GT_OK;
    }

    index = (trunkId - XCAT_FIRST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU)
            / 2;
    extraTrunksInfoPtr = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
            extraTrunksInfoPtr;
    currWaTrunkInfoPtr = &extraTrunksInfoPtr->extraTrunkInfoArray[index];

    /************************************************/
    currentMemberPtr = &currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.upToMemberMap[
            0];

    cpssOsPrintf(" upToMemberMap [] \n");
    cpssOsPrintf("\n");

    cpssOsPrintf(" UP       |   {dev}  , {port}   \n");
    cpssOsPrintf("================================\n");

    for(ii = 0 ; ii < PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS;
        ii++, currentMemberPtr++)
    {
        cpssOsPrintf(" %d       %d    , %d \n",
                        ii,
                        currentMemberPtr->device,
                        currentMemberPtr->port);
    }

    cpssOsPrintf("\n");

    /************************************************/

    cpssOsPrintf(" nextUpIndexToUse = [%d] \n",
            currWaTrunkInfoPtr->errata.
                info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.
                    nextUpIndexToUse);
    cpssOsPrintf("\n");

    /************************************************/
    nextUpToUsePtr = &currWaTrunkInfoPtr->errata.
        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.upToUseArr[
            0];

    cpssOsPrintf(" upToUseArr [] \n");
    cpssOsPrintf("\n");

    cpssOsPrintf(" index     |   next UP to use \n");
    cpssOsPrintf("================================\n");

    for(ii = 0 ; ii < PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS;
        ii++, nextUpToUsePtr++)
    {
        cpssOsPrintf(" %d       %d    \n",
                        ii,
                        (*nextUpToUsePtr));
    }

    cpssOsPrintf("\n");
    /************************************************/


    return GT_OK;
}
#endif /*DUMP_TRUNK_UP_TO_MEMBER*/

/*******************************************************************************
* trunkDbFlagsInit
*
* DESCRIPTION:
*       get info about the flags of the device about trunk
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkDbFlagsInit
(
    IN  GT_U8                       devNum
)
{
    PRV_CPSS_TRUNK_DB_INFO_STC *devTrunkInfoPtr = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum);

    devTrunkInfoPtr->nullPort = PRV_CPSS_DXCH_NULL_PORT_NUM_CNS;
    devTrunkInfoPtr->doDesignatedTableWorkAround  =  GT_FALSE;

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        devTrunkInfoPtr->numOfDesignatedTrunkEntriesHw = 8;
    }
    else
    {
        devTrunkInfoPtr->numOfDesignatedTrunkEntriesHw = 64;
    }

    devTrunkInfoPtr->initDone = GT_FALSE;
    devTrunkInfoPtr->validityMask.port   = 0x3f;/* 6 bits supported */
    devTrunkInfoPtr->validityMask.device = 0x1f;/* 5 bits supported */

    devTrunkInfoPtr->loadBalanceType =  PRV_CPSS_TRUNK_LOAD_BALANCE_NATIVE_E;

    devTrunkInfoPtr->numNonTrunkEntriesHw = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.trunksNum;
    devTrunkInfoPtr->numTrunksSupportedHw = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.trunksNum;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTrunkPortTrunkIdSet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portNum  - the port number.
*       memberOfTrunk - is the port associated with the trunk
*                 GT_FALSE - the port is set as "not member" in the trunk
*                 GT_TRUE  - the port is set with the trunkId
*
*       trunkId - the trunk to which the port associate with
*                 This field indicates the trunk group number (ID) to which the
*                 port is a member.
*                 1 through 127 = The port is a member of the trunk
*                 this value relevant only when memberOfTrunk = GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad port number , or
*                      bad trunkId number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkPortTrunkIdSet
(
    IN GT_U8            devNum,
    IN GT_U8            portNum,
    IN GT_BOOL          memberOfTrunk,
    IN GT_TRUNK_ID      trunkId
)
{
    GT_STATUS rc;    /* return error code */
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write to register */
    GT_U32 bitOffset;       /* offset of nit from start of register */
    GT_U32 length; /* length of field in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    if(memberOfTrunk == GT_TRUE)
    {
        HW_TRUNK_ID_AS_INDEX_IN_TRUNK_MEMBERS_CHECK_MAC(devNum,trunkId);

        value = trunkId;
    }
    else
    {
        /* the port is not member in any trunk */
        value = 0;
    }

    /* trunk id of port control it accessed via interface to :
       Port<n> VLAN and QoS Configuration Entry
       Word 0 , bits 0:6 (7 bits)
    */
    rc = prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        portNum,
                                        0,/* Word 0 */
                                        0,/* start bit 0 */
                                        7,/* 7 bits */
                                        value);
    if(rc != GT_OK)
    {
        return rc;
    }

    /*  NOTE: for multi-port groups devices
        those are 'HA' registers and must be set with 'global port' number and
        the same way to all port groups
    */

    /* base 0x0780002C , 4 ports info  in register , for each port info 7 bits*/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                trunkNumConfigReg[portNum >> 2];

    length = 7;/* 7 bits per port */
    bitOffset = (portNum & 0x3) * length;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, bitOffset, length, value);
}

/*******************************************************************************
* cpssDxChTrunkPortTrunkIdGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Set the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portNum  - the port number.
*
* OUTPUTS:
*       memberOfTrunkPtr - (pointer to) is the port associated with the trunk
*                 GT_FALSE - the port is set as "not member" in the trunk
*                 GT_TRUE  - the port is set with the trunkId
*       trunkIdPtr - (pointer to)the trunk to which the port associate with
*                 This field indicates the trunk group number (ID) to which the
*                 port is a member.
*                 1 through 127 = The port is a member of the trunk
*                 this value relevant only when (*memberOfTrunkPtr) = GT_TRUE
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad port number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_STATE - the trunkId value is not synchronized in the 2 registers
*                      that should hold the same value
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkPortTrunkIdGet
(
    IN GT_U8            devNum,
    IN GT_U8            portNum,
    OUT GT_BOOL         *memberOfTrunkPtr,
    OUT GT_TRUNK_ID     *trunkIdPtr
)
{
    GT_STATUS   rc;
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write to register */
    GT_U32 bitOffset;       /* offset of nit from start of register */
    GT_U32 length; /* length of field in the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(memberOfTrunkPtr);
    CPSS_NULL_PTR_CHECK_MAC(trunkIdPtr);

    /* trunk id of port control it accessed via interface to :
       Port<n> VLAN and QoS Configuration Entry
       Word 0 , bits 0:6 (7 bits)
    */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        portNum,
                                        0,/* Word 0 */
                                        0,/* start bit 0 */
                                        7,/* 7 bits */
                                        &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* build the trunk-id value */
    *trunkIdPtr = (GT_TRUNK_ID)value;

    /* the port is trunk member if the trunk-id is NOT ZERO */
    *memberOfTrunkPtr = (*trunkIdPtr) ? GT_TRUE : GT_FALSE;


    /************************************************************************/
    /* according to DR decision: 30-05-06                                   */
    /* check that the other register is also configured with the same value */
    /************************************************************************/

    /*  NOTE: for multi-port groups devices
        those are 'HA' registers and must be set with 'global port' number and
        the same way to all port groups
    */

    /* base 0x0780002C , 4 ports info  in register , for each port info 7 bits*/
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.
                trunkNumConfigReg[portNum >> 2];

    length = 7;/* 7 bits per port */
    bitOffset = (portNum & 0x3) * length;

    rc =  prvCpssDrvHwPpGetRegField(devNum, regAddr, bitOffset, length, &value);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(value != (GT_U32)(*trunkIdPtr))
    {
        return GT_BAD_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
* chTrunkMembersTableAccessWrite
*
* DESCRIPTION:
*       function write specific member of a specific trunk :
*       "Trunk table Trunk<n> Member<i> Entry (1<=n<128, 0<=i<8)"
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id 1<=trunkId<128
*       trunkMemberIdx - trunk member index  0<=trunkMemberIdx<8
*       memberPtr - (pointer to) the member to write to the table
*
* OUTPUTS:
*       memberPtr - (pointer to) the member to write to the table
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_BAD_PARAM - bad parameters (or null pointer)
*       GT_OUT_OF_RANGE - parameter not in valid range
*
* COMMENTS:
*
*       [1] C.14.2 Trunk table
*       The Trunk table is used to define the trunk members.
*       Up to 127 trunks (numbered 1 through 127) may be defined,with each trunk
*       having up to eight members.
*       see Table 315 outlines the content of each entry in the table.
*
*******************************************************************************/
static GT_STATUS chTrunkMembersTableAccessWrite
(
    IN GT_U8                            devNum,
    IN GT_TRUNK_ID                      trunkId,
    IN GT_U32                           trunkMemberIdx,
    INOUT CPSS_TRUNK_MEMBER_STC         *memberPtr
)
{
    GT_U32      valueArr[1];/* value to write to memory */
    GT_U32      entryIndex; /* entry index */

    entryIndex = ((trunkId) << 3) | /* trunk id     */
                  (trunkMemberIdx);  /* member index */

    valueArr[0] = memberPtr->device << 6 | memberPtr->port;

    return prvCpssDxChWriteTableEntry(devNum,
                                      PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E,
                                      entryIndex,
                                      valueArr);
}


/*******************************************************************************
* chTrunkMembersTableAccessRead
*
* DESCRIPTION:
*       function read specific member of a specific trunk :
*       "Trunk table Trunk<n> Member<i> Entry (1<=n<128, 0<=i<8)"
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id 1<=trunkId<128
*       trunkMemberIdx - trunk member index  0<=trunkMemberIdx<8
*       memberPtr - (pointer to) the member read from the table
*
* OUTPUTS:
*       memberPtr - (pointer to) the member read from the table
*
*
* RETURNS:
*       GT_OK        - on success.
*       GT_FAIL      - on failure.
*       GT_BAD_PARAM - bad parameters (or null pointer)
*       GT_OUT_OF_RANGE - parameter not in valid range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*
* COMMENTS:
*
*       [1] C.14.2 Trunk table
*       The Trunk table is used to define the trunk members.
*       Up to 127 trunks (numbered 1 through 127) may be defined,with each trunk
*       having up to eight members.
*       see Table 315 outlines the content of each entry in the table.
*
*******************************************************************************/
static GT_STATUS chTrunkMembersTableAccessRead
(
    IN GT_U8                            devNum,
    IN GT_TRUNK_ID                      trunkId,
    IN GT_U32                           trunkMemberIdx,
    INOUT CPSS_TRUNK_MEMBER_STC         *memberPtr
)
{

    GT_STATUS rc;    /* return error code */
    GT_U32      valueArr[1];/* value to read from memory */
    GT_U32      entryIndex; /* entry index */


    entryIndex = trunkId << 3 | /* trunk id     */
                 trunkMemberIdx ;  /* member index */

    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_TABLE_TRUNK_MEMBERS_E,
                                   entryIndex,
                                   valueArr);
    if (rc != GT_OK)
    {
        return rc;
    }

    memberPtr->device = (GT_U8)(U32_GET_FIELD_MAC(valueArr[0],6,5));
    memberPtr->port   = (GT_U8)(U32_GET_FIELD_MAC(valueArr[0],0,6));

    return GT_OK;

}


/*******************************************************************************
* cpssDxChTrunkTableEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunk table entry , and set the number of members in it.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id
*       numMembers - num of enabled members in the trunk
*                    Note : value 0 is not recommended.
*       membersArray - array of enabled members of the trunk
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE - numMembers exceed the number of maximum number
*                         of members in trunk (total of 0 - 8 members allowed)
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkTableEntrySet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    IN  GT_U32                  numMembers,
    IN  CPSS_TRUNK_MEMBER_STC   membersArray[]
)
{
    GT_STATUS               rc;              /* return error code */
    GT_U32                  regAddr;         /* hw memory address to write */
    GT_U32                  offset;          /* offset of field in the register */
    GT_U32                  length;          /* length of field in the register */
    GT_U32                  data;            /* data to write to register */
    GT_U32                  ii;              /* iterator */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    HW_TRUNK_ID_AS_INDEX_IN_TRUNK_MEMBERS_CHECK_MAC(devNum,trunkId);
    CPSS_NULL_PTR_CHECK_MAC(membersArray);

    if(numMembers > PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS)
    {
        return GT_BAD_PARAM;
    }

    /* NOTE : if (numMembers == 0) the PP will not work "as expected" with
    this configuration */

    for (ii = 0;ii < numMembers; ii ++)
    {
        if(membersArray[ii].port >= BIT_6 || membersArray[ii].device >= BIT_5)
        {
            /* not supported in HW */
            return GT_BAD_PARAM;
        }

        rc = chTrunkMembersTableAccessWrite(devNum,trunkId,ii,&membersArray[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* do the WA (if needed) for "wrong trunk id source port information of packet to CPU" erratum */
    rc = trunkTrafficToCpuWaTrunkTableEntrySet(devNum,trunkId,numMembers,membersArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH_TRUNK_CONFIG_FOR_TRUNK_NEXT_HOP_WA_E))
    {
        /* Title : Fer#137: Trunk configuration for Trunk Next Hop

        Relevant for:
        =============
        Relevant for: 98DX167/247-A1/A2 and 98DX253/263/273-A1/A298DX107-BCW-A2/B0
        and 98DX107-LKJB098DX163-A1/A2, 98DX163R-A2, and 98DX243-A1/A298DX262-A2

        Description:
        =============
        When the Next Hop is a trunk, the router sets the TrunkHash in the
        descriptor to PktHash[2:0]. This means that the value of the TrunkHash
        may be set to any value: 0 through 7.
        If the Next Hop is a trunk with less than 8 members, the device may
        fetch an invalid trunk member.

        Functional Impact:
        =================
        The application must configure 8 entries in the Trunks Members tables
        with valid members.

        Work Around:
        =============
        All of the entries in the Trunks Members table of trunks that are
        associated with Next Hop must be configured with trunk members.
        For example, for a trunk that has two members:
        -- Trunk<n>Member0 <= Member0
        -- Trunk<n>Member1 <= Member1
        -- Trunk<n>Member2 <= Member0
        -- Trunk<n>Member3 <= Member1
        -- Trunk<n>Member4 <= Member0
        -- Trunk<n>Member5 <= Member1
        -- Trunk<n>Member6 <= Member0
        -- Trunk<n>Member7 <= Member1

        */

        for(ii = numMembers ; ii < CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS; ii++)
        {
            rc = chTrunkMembersTableAccessWrite(devNum,trunkId,ii,
                    &membersArray[ii % numMembers]);/* set redundant members */
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /*
            NOTE : the fix for that Erratum is not changing the value
                "num of members in trunk" -- only set redundant members.
            -- this is in order not to effect all L2 trunk load balancing.
            -- that way we don't care it it is cheetah (not support L3)
        */
    }

    /* update trunk counter register */
    length = 4;/* 4 bits per trunk */
    data = numMembers;

    REGISTER_ADDR_IN_NUM_MEMBERS_TRUNK_TABLE_MAC(devNum,trunkId,(&regAddr));

    offset = length * ((trunkId-1) & 0x7);

    /* write the counter to the HW */
    return prvCpssDrvHwPpSetRegField(devNum,regAddr,offset,length,data);
}

/*******************************************************************************
* cpssDxChTrunkTableEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk table entry , and get the number of members in it.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id
*
* OUTPUTS:
*       numMembersPtr - (pointer to)num of members in the trunk
*       membersArray - array of enabled members of the trunk
*                      array is allocated by the caller , it is assumed that
*                      the array can hold CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS
*                      members
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkTableEntryGet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT GT_U32                  *numMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS]
)
{
    GT_STATUS       rc;                 /* return error code */
    GT_U32          regAddr;            /* hw memory address to write */
    GT_U32          offset;             /* offset of field in the register */
    GT_U32          length;             /* length of field in the register */
    GT_U32          data;               /* data to write to register */
    GT_U32          ii;                  /* iterator */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    HW_TRUNK_ID_AS_INDEX_IN_TRUNK_MEMBERS_CHECK_MAC(devNum,trunkId);
    CPSS_NULL_PTR_CHECK_MAC(numMembersPtr);
    CPSS_NULL_PTR_CHECK_MAC(membersArray);/* must always be valid and must be
            able to hold CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS of members */

    /* update trunk counter register */
    length = 4;/* 4 bits per trunk */

    REGISTER_ADDR_IN_NUM_MEMBERS_TRUNK_TABLE_MAC(devNum,trunkId,(&regAddr));

    offset = length * ((trunkId - 1) & 0x7);

    /* write the counter to the HW */
    prvCpssDrvHwPpGetRegField(devNum,regAddr,offset,length,&data);

    *numMembersPtr = data; /* let the caller get the actual number of members */

    for (ii = 0; ii < (*numMembersPtr); ii ++)
    {
        rc = chTrunkMembersTableAccessRead(devNum,trunkId,ii,&membersArray[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunk's non-trunk ports specific bitmap entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsPtr - (pointer to) non trunk port bitmap of the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkNonTrunkPortsEntrySet
(
    IN  GT_U8               devNum,
    IN  GT_TRUNK_ID         trunkId,
    IN  CPSS_PORTS_BMP_STC  *nonTrunkPortsPtr
)
{
    GT_U32  regAddr;   /* hw memory address to write */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    HW_TRUNK_ID_AS_INDEX_IN_NON_TRUNK_PORTS_CHECK_MAC(devNum,trunkId);
    CPSS_NULL_PTR_CHECK_MAC(nonTrunkPortsPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
                trunkTblCfgRegs.baseAddrNonTrunk +
                CHEETAH_TRUNK_NON_TRUNK_TABLE_OFFSET_CNS * trunkId;

        return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0,
                        PRV_CPSS_PP_MAC(devNum)->numOfPorts,
                        nonTrunkPortsPtr->ports[0]);
    }
    else
    {
        return prvCpssDxChWriteTableEntry(devNum,
                            PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E,
                            (GT_U32)trunkId,
                            nonTrunkPortsPtr->ports);
    }
}

/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk's non-trunk ports bitmap specific entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id - in this API trunkId can be ZERO !
*
* OUTPUTS:
*       nonTrunkPortsPtr - (pointer to) non trunk port bitmap of the trunk.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkNonTrunkPortsEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_TRUNK_ID         trunkId,
    OUT  CPSS_PORTS_BMP_STC  *nonTrunkPortsPtr
)
{
    GT_U32  regAddr;   /* hw memory address to write */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    HW_TRUNK_ID_AS_INDEX_IN_NON_TRUNK_PORTS_CHECK_MAC(devNum,trunkId);
    CPSS_NULL_PTR_CHECK_MAC(nonTrunkPortsPtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
                trunkTblCfgRegs.baseAddrNonTrunk +
                CHEETAH_TRUNK_NON_TRUNK_TABLE_OFFSET_CNS * trunkId;

        nonTrunkPortsPtr->ports[1] = 0;

        return prvCpssDrvHwPpGetRegField(devNum, regAddr, 0,
                        PRV_CPSS_PP_MAC(devNum)->numOfPorts,
                        &nonTrunkPortsPtr->ports[0]);
    }
    else
    {
        return prvCpssDxChReadTableEntry(devNum,
                            PRV_CPSS_DXCH_LION_TABLE_TXQ_NON_TRUNK_MEMBERS_E,
                            (GT_U32)trunkId,
                            nonTrunkPortsPtr->ports);
    }
}


/*******************************************************************************
* cpssDxChTrunkDesignatedPortsEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the designated trunk table specific entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE          - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDesignatedPortsEntrySet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    IN  CPSS_PORTS_BMP_STC  *designatedPortsPtr
)
{
    GT_U32  regAddr;   /* hw memory address to write */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(designatedPortsPtr);
    HW_DESIGNATED_PORTS_ENTRY_INDEX_CHECK_MAC(devNum,entryIndex);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /*write the low register*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            trunkTblCfgRegs.baseAddrTrunkDesig +
            CHEETAH_TRUNK_DESIGNATED_PORTS_TABLE_OFFSET_CNS * entryIndex;

        return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0,
                        PRV_CPSS_PP_MAC(devNum)->numOfPorts,
                        designatedPortsPtr->ports[0]);
    }
    else
    {
        return prvCpssDxChWriteTableEntry(devNum,
                            PRV_CPSS_DXCH_LION_TABLE_TXQ_DESIGNATED_PORT_E,
                            entryIndex,
                            designatedPortsPtr->ports);
    }
}

/*******************************************************************************
* cpssDxChTrunkDesignatedPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE          - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDesignatedPortsEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    OUT  CPSS_PORTS_BMP_STC  *designatedPortsPtr
)
{
    GT_U32  regAddr;   /* hw memory address to write */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(designatedPortsPtr);
    HW_DESIGNATED_PORTS_ENTRY_INDEX_CHECK_MAC(devNum,entryIndex);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /*read the low register*/
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            trunkTblCfgRegs.baseAddrTrunkDesig +
            CHEETAH_TRUNK_DESIGNATED_PORTS_TABLE_OFFSET_CNS * entryIndex;

        designatedPortsPtr->ports[1] = 0;

        return prvCpssDrvHwPpGetRegField(devNum, regAddr, 0,
                        PRV_CPSS_PP_MAC(devNum)->numOfPorts,
                        &designatedPortsPtr->ports[0]);
    }
    else
    {
        return prvCpssDxChReadTableEntry(devNum,
                            PRV_CPSS_DXCH_LION_TABLE_TXQ_DESIGNATED_PORT_E,
                            entryIndex,
                            designatedPortsPtr->ports);
    }
}

/*******************************************************************************
* cpssDxChTrunkHashIpModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the IP SIP/DIP information,
*       when calculation the trunk hashing index for a packet.
*       Relevant when the is IPv4 or IPv6 and <TrunkHash Mode> = 0.
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       enable - GT_FALSE - IP data is not added to the trunk load balancing
*                           hash.
*               GT_TRUE - The following function is added to the trunk load
*                         balancing hash, if the packet is IPv6.
*                         IPTrunkHash = according to setting of API
*                                       cpssDxChTrunkHashIpv6ModeSet(...)
*                         else packet is IPv4:
*                         IPTrunkHash = SIP[5:0]^DIP[5:0]^SIP[21:16]^DIP[21:16].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpModeSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* value to write to register               */
    GT_U32      startBit;        /* the word's bit at which the field starts */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, Lion & xCat2 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
        startBit  = 5;
    }
    else
    {
        /* DXCH1-3 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.l2IngressControl;
        startBit  = 3;
    }

    value = (enable == GT_TRUE) ? 1 : 0;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, startBit, 1, value);
}

/*******************************************************************************
* cpssDxChTrunkHashIpModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get the enable/disable of device from considering the IP SIP/DIP
*       information, when calculation the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr -(pointer to)
*               GT_FALSE - IP data is not added to the trunk load balancing
*                           hash.
*               GT_TRUE - The following function is added to the trunk load
*                         balancing hash, if the packet is IPv6.
*                         IPTrunkHash = according to setting of API
*                                       cpssDxChTrunkHashIpv6ModeSet(...)
*                         else packet is IPv4:
*                         IPTrunkHash = SIP[5:0]^DIP[5:0]^SIP[21:16]^DIP[21:16].
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpModeGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS   rc;              /* returned code                            */
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* value to read from register              */
    GT_U32      startBit;        /* the word's bit at which the field starts */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, Lion & xCat2 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
        startBit  = 5;
    }
    else
    {
        /* DXCH1-3 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.l2IngressControl;
        startBit  = 3;
    }


    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, startBit, 1, &value);

    *enablePtr = value ? GT_TRUE : GT_FALSE;

    return rc;
}

/*******************************************************************************
* cpssDxChTrunkHashL4ModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the L4 TCP/UDP
*       source/destination port information, when calculation the trunk hashing
*       index for a packet.
*
*       Note:
*       1. Not relevant to NON TCP/UDP packets.
*       2. The Ipv4 hash must also be enabled , otherwise the L4 hash mode
*          setting not considered.
*       3. Not relevant to multi-destination packets (include routed IPM).
*       4. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       hashMode - L4 hash mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashL4ModeSet
(
    IN GT_U8   devNum,
    IN CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT hashMode
)
{
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* HW data value                            */
    GT_U32      mask ;           /* HW data mask                             */
    GT_U32      l4HashBit;       /* l4 Hash Bit                              */
    GT_U32      l4HashLongBit;   /* l4 Hash Long Bit                              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, Lion & xCat2 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
        l4HashBit      = BIT_4;
        l4HashLongBit  = BIT_0;
    }
    else
    {
        /* DXCH1-3 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.l2IngressControl;
        l4HashBit      = BIT_4;
        l4HashLongBit  = BIT_14;
    }

    switch(hashMode)
    {
        case CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E:
            value = 0;
            break;
        case CPSS_DXCH_TRUNK_L4_LBH_LONG_E:
            value = l4HashBit | l4HashLongBit;
            break;
        case CPSS_DXCH_TRUNK_L4_LBH_SHORT_E:
            value = l4HashBit;
            break;
        default:
            return GT_BAD_PARAM;
    }

    mask = l4HashBit | l4HashLongBit;

    return prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, mask, value);
}

/*******************************************************************************
* cpssDxChTrunkHashL4ModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get the Enable/Disable of device from considering the L4 TCP/UDP
*       source/destination port information, when calculation the trunk hashing
*       index for a packet.
*
*       Note:
*       1. Not relevant to NON TCP/UDP packets.
*       2. The Ipv4 hash must also be enabled , otherwise the L4 hash mode
*          setting not considered.
*       3. Not relevant to multi-destination packets (include routed IPM).
*       4. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       hashModePtr - (pointer to)L4 hash mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashL4ModeGet
(
    IN GT_U8   devNum,
    OUT CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT *hashModePtr
)
{
    GT_STATUS   rc;              /* returned code                            */
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* HW data value                            */
    GT_U32      l4HashBit;       /* l4 Hash Bit                              */
    GT_U32      l4HashLongBit;   /* l4 Hash Long Bit                              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(hashModePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, Lion & xCat2 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
        l4HashBit      = BIT_4;
        l4HashLongBit  = BIT_0;
    }
    else
    {
        /* DXCH1-3 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.l2IngressControl;
        l4HashBit      = BIT_4;
        l4HashLongBit  = BIT_14;
    }

    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value);

    if(value & l4HashBit)
    {
        if(value & l4HashLongBit)
        {
            *hashModePtr = CPSS_DXCH_TRUNK_L4_LBH_LONG_E;
        }
        else
        {
            *hashModePtr = CPSS_DXCH_TRUNK_L4_LBH_SHORT_E;
        }
    }
    else
    {
        *hashModePtr = CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E;
    }

    return rc;
}


/*******************************************************************************
* cpssDxChTrunkHashIpv6ModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the hash generation function for Ipv6 packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number.
*       hashMode   - the Ipv6 hash mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or hash mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpv6ModeSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT   hashMode
)
{
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* value to read from register              */
    GT_U32      startBit;        /* the word's bit at which the field starts */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, Lion & xCat2 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
        startBit  = 1;
    }
    else
    {
        /* DXCH1-3 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.l2IngressControl;
        startBit  = 10;
    }

    /* setting <Ipv6HashConfig[1:0]> field data */
    switch(hashMode)
    {
        case CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_FLOW_E:
            value = 0;
            break;
        case CPSS_DXCH_TRUNK_IPV6_HASH_MSB_SIP_DIP_FLOW_E:
            value = 1;
            break;
        case CPSS_DXCH_TRUNK_IPV6_HASH_MSB_LSB_SIP_DIP_FLOW_E:
            value = 2;
            break;
        case CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_E:
            value = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, startBit, 2, value);
}

/*******************************************************************************
* cpssDxChTrunkHashIpv6ModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the hash generation function for Ipv6 packets.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number.
*
* OUTPUTS:
*       hashModePtr - (pointer to)the Ipv6 hash mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or hash mode
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpv6ModeGet
(
    IN GT_U8                                devNum,
    OUT CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT   *hashModePtr
)
{
    GT_STATUS   rc;              /* returned code                            */
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* value to read from register              */
    GT_U32      startBit;        /* the word's bit at which the field starts */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(hashModePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, Lion & xCat2 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
        startBit  = 1;
    }
    else
    {
        /* DXCH1-3 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.l2IngressControl;
        startBit  = 10;
    }


    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, startBit, 2, &value);

    switch(value)
    {
        case 0:
            *hashModePtr = CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_FLOW_E;
            break;
        case 1:
            *hashModePtr =  CPSS_DXCH_TRUNK_IPV6_HASH_MSB_SIP_DIP_FLOW_E;
            break;
        case 2:
            *hashModePtr =  CPSS_DXCH_TRUNK_IPV6_HASH_MSB_LSB_SIP_DIP_FLOW_E;
            break;
        case 3:
        default:
            *hashModePtr =  CPSS_DXCH_TRUNK_IPV6_HASH_LSB_SIP_DIP_E;
            break;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTrunkHashIpAddMacModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the use of mac address bits to trunk hash calculation when packet is
*       IP and the "Ip trunk hash mode enabled".
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       enable  - enable/disable feature
*                   GT_FALSE - If the packet is an IP packet MAC data is not
*                              added to the Trunk load balancing hash.
*                   GT_TRUE - The following function is added to the trunk load
*                             balancing hash:
*                             MACTrunkHash = MAC_SA[5:0]^MAC_DA[5:0].
*
*                             NOTE: When the packet is not an IP packet and
*                             <TrunkLBH Mode> = 0, the trunk load balancing
*                             hash = MACTrunkHash, regardless of this setting.
*                             If the packet is IPv4/6-over-X tunnel-terminated,
*                             the mode is always GT_FALSE (since there is no
*                              passenger packet MAC header).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpAddMacModeSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* value to read from register              */
    GT_U32      startBit;        /* the word's bit at which the field starts */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, Lion & xCat2 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
        startBit  = 7;
    }
    else
    {
        /* DXCH1-3 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.l2IngressControl;
        startBit  = 2;
    }

    value = (enable == GT_TRUE) ? 1 : 0;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, startBit, 1, value);
}



/*******************************************************************************
* cpssDxChTrunkHashIpAddMacModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the use of mac address bits to trunk hash calculation when packet is
*       IP and the "Ip trunk hash mode enabled".
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)enable/disable feature
*                   GT_FALSE - If the packet is an IP packet MAC data is not
*                              added to the Trunk load balancing hash.
*                   GT_TRUE - The following function is added to the trunk load
*                             balancing hash:
*                             MACTrunkHash = MAC_SA[5:0]^MAC_DA[5:0].
*
*                             NOTE: When the packet is not an IP packet and
*                             <TrunkLBH Mode> = 0, the trunk load balancing
*                             hash = MACTrunkHash, regardless of this setting.
*                             If the packet is IPv4/6-over-X tunnel-terminated,
*                             the mode is always GT_FALSE (since there is no
*                              passenger packet MAC header).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpAddMacModeGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS   rc;              /* returned code                            */
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* value to read from register              */
    GT_U32      startBit;        /* the word's bit at which the field starts */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, Lion & xCat2 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
        startBit  = 7;
    }
    else
    {
        /* DXCH1-3 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.l2IngressControl;
        startBit  = 2;
    }

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, startBit, 1, &value);

    *enablePtr = value ? GT_TRUE : GT_FALSE;

    return rc;
}



/*******************************************************************************
* cpssDxChTrunkHashGlobalModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the general hashing mode of trunk hash generation.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       hashMode   - hash mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or hash mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashGlobalModeSet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  hashMode
)
{
    GT_STATUS   rc;              /* returned code                            */
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* value to write to register 1             */
    GT_U32      value1;          /* value to write to register 2             */
    GT_U32      startBit;        /* the word's bit at which the field starts */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch(hashMode)
    {
        case CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E:
            value = 0;
            value1 = 0;/* disable CRC hash mode - relevant to trunkCrcHashSupported */
            break;
        case CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E:
            value = 1;
            value1 = 0;/* disable CRC hash mode - relevant to trunkCrcHashSupported */
            break;
        case CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E:
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.trunkCrcHashSupported == GT_FALSE)
            {
                /* the device not support the CRC hash feature */
                return GT_BAD_PARAM;
            }
            value = 0;/* don't care */
            value1 = 1;/* enable CRC hash mode */
            break;
        default:
            return GT_BAD_PARAM;
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion, xCat2 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
        startBit  = 3;
    }
    else
    {
        /* DXCH1-3 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.l2IngressControl;
        startBit  = 9;
    }

    rc =  prvCpssDrvHwPpSetRegField(devNum, regAddr, startBit, 1, value);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.trunkCrcHashSupported == GT_TRUE)
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.crcHashConfigurationReg;

        rc =  prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, value1);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashGlobalModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the general hashing mode of trunk hash generation.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       hashModePtr   - (pointer to)hash mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashGlobalModeGet
(
    IN GT_U8    devNum,
    OUT CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  *hashModePtr
)
{
    GT_STATUS   rc;              /* returned code                            */
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* value to read from register              */
    GT_U32      startBit;        /* the word's bit at which the field starts */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(hashModePtr);

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.trunkCrcHashSupported == GT_TRUE)
    {
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.crcHashConfigurationReg;

        rc =  prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &value);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(value == 1)
        {
            /* using the CRC hash mode */
            *hashModePtr = CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E;
            return GT_OK;
        }

        /* NOT using the CRC hash mode - so check the other register ... */
    }

    if (PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        /* xCat, lion, xCat2 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
        startBit  = 3;
    }
    else
    {
        /* DXCH1-3 */
        regAddr =
            PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.l2IngressControl;
        startBit  = 9;
    }

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, startBit, 1, &value);

    *hashModePtr =
        value
            ? CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E
            : CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E;

    return rc;
}

/*******************************************************************************
* cpssDxChTrunkHashDesignatedTableModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the hash mode of the designated trunk table .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       mode - The designated table hashing mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashDesignatedTableModeSet
(
    IN GT_U8   devNum,
    IN CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT    mode
)
{
    GT_STATUS   rc;      /* return code */
    GT_U32      regAddr; /*register addresses */
    GT_U32      value1,value2,value3,value4; /* value of registers */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch(mode)
    {
        case CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E:
            value1 = 0;
            value2 = 0;/* don't care */
            value3 = 1;
            value4 = 0;/* don't care */
            break;
        case CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E:
            value1 = 1;
            value2 = 0;
            value3 = 0;
            value4 = 0;
            break;
        case CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E:
            value1 = 1;
            value2 = 1;
            value3 = 2;
            value4 = 0;
            break;
        case CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E:
            if(0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
            {
                /* those devices not support this option */
                return GT_BAD_PARAM;
            }
            value1 = 1;/* not used for Lion, used for xCat2 */
            value2 = 0;/* not used for Lion, used for xCat2 */
            value3 = 3;/* not used for xCat2, used for Lion */
            value4 = 1;/* not used for Lion, used for xCat2 */
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.filterConfig.globalEnables;

        return prvCpssDxChHwPpSetRegField(devNum, regAddr, 5, 2, value3);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfig;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, value1);
        if(rc != GT_OK)
        {
            return rc;
        }

        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 24, 1, value2);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* xCat2 and above - configure McTrunkHashSrcInfoEn bit */
        if(PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
        {
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;
            rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 8, 1, value4);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        return GT_OK;
    }
}

/*******************************************************************************
* cpssDxChTrunkHashDesignatedTableModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the hash mode of the designated trunk table .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       modePtr - (pointer to) The designated table hashing mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashDesignatedTableModeGet
(
    IN GT_U8   devNum,
    OUT CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT *modePtr
)
{
    GT_STATUS   rc;      /* return code */
    GT_U32      regAddr; /*register addresses */
    GT_U32      value1,value2,value3,value4; /* value of registers */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.egr.filterConfig.globalEnables;
        rc = prvCpssDxChHwPpGetRegField(devNum, regAddr, 5, 2, &value3);
    }
    else
    {
        value1 = 0;
        /* first line mode check */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfig;
        rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &value1);
        if(rc != GT_OK)
        {
            return rc;
        }

        if( 0 == value1 )
        {
            value3 = 1;
        }
        else
        {
            value4 = 0;
            /* xCat2 and above - check McTrunkHashSrcInfoEn bit */
            if(PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueMiscCtrl;
                rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 8, 1, &value4);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            if( 1 == value4 )
            {
                value3 = 3;
            }
            else
            {
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfigExt;
                rc =  prvCpssDrvHwPpGetRegField(devNum, regAddr, 24, 1, &value2);
                if(rc != GT_OK)
                {
                    return rc;
                }

                if( 0 == value2 )
                {
                    value3 = 0;
                }
                else
                {
                    value3 = 2;
                }
            }

        }
    }

    switch(value3)
    {
        case 0:
            *modePtr = CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E;
            break;
        case 1:
            *modePtr = CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E;
            break;
        case 2:
            *modePtr = CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E;
            break;
        case 3:
        default:
            *modePtr = CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTrunkDbEnabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the enabled members of the trunk
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       numOfEnabledMembersPtr - (pointer to) max num of enabled members to
*                                retrieve - this value refer to the number of
*                                members that the array of enabledMembersArray[]
*                                can retrieve.
*
* OUTPUTS:
*       numOfEnabledMembersPtr - (pointer to) the actual num of enabled members
*                      in the trunk (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
*       enabledMembersArray - (array of) enabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbEnabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfEnabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   enabledMembersArray[]
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return prvCpssGenericTrunkDbEnabledMembersGet(devNum,trunkId,
                numOfEnabledMembersPtr,enabledMembersArray);
}

/*******************************************************************************
* cpssDxChTrunkDbDisabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the disabled members of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       numOfDisabledMembersPtr - (pointer to) max num of disabled members to
*                                retrieve - this value refer to the number of
*                                members that the array of enabledMembersArray[]
*                                can retrieve.
*
* OUTPUTS:
*       numOfDisabledMembersPtr -(pointer to) the actual num of disabled members
*                      in the trunk (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
*       disabledMembersArray - (array of) disabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbDisabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfDisabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   disabledMembersArray[]
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    return prvCpssGenericTrunkDbDisabledMembersGet(devNum,trunkId,
                numOfDisabledMembersPtr,disabledMembersArray);
}

/*******************************************************************************
* cpssDxChTrunkDbIsMemberOfTrunk
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Checks if a member {device,port} is a trunk member.
*       if it is trunk member the function retrieve the trunkId of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       memberPtr - (pointer to) the member to check if is trunk member
*
* OUTPUTS:
*       trunkIdPtr - (pointer to) trunk id of the port .
*                    this pointer allocated by the caller.
*                    this can be NULL pointer if the caller not require the
*                    trunkId(only wanted to know that the member belongs to a
*                    trunk)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_FOUND             - the pair {devNum,port} not a trunk member
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbIsMemberOfTrunk
(
    IN  GT_U8                   devNum,
    IN  CPSS_TRUNK_MEMBER_STC   *memberPtr,
    OUT GT_TRUNK_ID             *trunkIdPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    return prvCpssGenericTrunkDbIsMemberOfTrunk(devNum,memberPtr,trunkIdPtr);
}

/*******************************************************************************
* cpssDxChTrunkDbTrunkTypeGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Get the trunk type.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number.
*       trunkId     - the trunk id.
*
* OUTPUTS:
*       typePtr - (pointer to) the trunk type
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbTrunkTypeGet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT CPSS_TRUNK_TYPE_ENT     *typePtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    return prvCpssGenericTrunkDbTrunkTypeGet(devNum,trunkId,typePtr);
}


/*******************************************************************************
* cpssDxChTrunkDesignatedMemberSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function Configures per-trunk the designated member -
*       value should be stored (to DB) even designated member is not currently
*       a member of Trunk.
*       Setting value replace previously assigned designated member.
*
*       NOTE that:
*       under normal operation this should not be used with cascade Trunks,
*       due to the fact that in this case it affects all types of traffic -
*       not just Multi-destination as in regular Trunks.
*
*  Diagram 1 : Regular operation - Traffic distribution on all enabled
*              members (when no specific designated member defined)
*
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*  index \ member  %   M1  %   M2   %    M3  %  M4  %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 0       %   1   %   0    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 1       %   0   %   1    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 2       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 3       %   0   %   0    %    0   %  1   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 4       %   1   %   0    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 5       %   0   %   1    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 6       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 7       %   0   %   0    %    0   %  1   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*  Diagram 2: Traffic distribution once specific designated member defined
*             (M3 in this case - which is currently enabled member in trunk)
*
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*  index \ member  %   M1  %   M2   %    M3  %  M4  %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 0       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 1       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 2       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 3       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 4       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 5       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 6       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 7       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number
*       trunkId     - the trunk id.
*       enable      - enable/disable designated trunk member.
*                     GT_TRUE -
*                       1. Clears all current Trunk member's designated bits
*                       2. If input designated member, is currently an
*                          enabled-member at Trunk (in local device) enable its
*                          bits on all indexes
*                       3. Store designated member at the DB (new DB parameter
*                          should be created for that)
*
*                     GT_FALSE -
*                       1. Redistribute current Trunk members bits (on all enabled members)
*                       2. Clear designated member at  the DB
*
*       memberPtr   - (pointer to)the designated member we set to the trunk.
*                     relevant only when enable = GT_TRUE
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST         - this member already exists in another trunk.
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDesignatedMemberSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_BOOL                  enable,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return prvCpssGenericTrunkDesignatedMemberSet(devNum,trunkId,
            enable,memberPtr);
}

/*******************************************************************************
* cpssDxChTrunkDbDesignatedMemberGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function get Configuration per-trunk the designated member -
*       value should be stored (to DB) even designated member is not currently
*       a member of Trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number
*       trunkId     - the trunk id.
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable designated trunk member.
*                     GT_TRUE -
*                       1. Clears all current Trunk member's designated bits
*                       2. If input designated member, is currently an
*                          enabled-member at Trunk (in local device) enable its
*                          bits on all indexes
*                       3. Store designated member at the DB (new DB parameter
*                          should be created for that)
*
*                     GT_FALSE -
*                       1. Redistribute current Trunk members bits (on all enabled members)
*                       2. Clear designated member at  the DB
*
*       memberPtr   - (pointer to) the designated member of the trunk.
*                     relevant only when enable = GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbDesignatedMemberGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    OUT GT_BOOL                 *enablePtr,
    OUT CPSS_TRUNK_MEMBER_STC   *memberPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return prvCpssGenericTrunkDbDesignatedMemberGet(devNum,trunkId,
            enablePtr,memberPtr);
}


/*******************************************************************************
* cpssDxChTrunkMembersSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function set the trunk with the specified enable and disabled
*       members.
*       this setting override the previous setting of the trunk members.
*
*       the user can "invalidate/unset" trunk entry by setting :
*           numOfEnabledMembers = 0 and  numOfDisabledMembers = 0
*
*       This function support next "set entry" options :
*       1. "reset" the entry
*          function will remove the previous settings
*       2. set entry after the entry was empty
*          function will set new settings
*       3. set entry with the same members that it is already hold
*          function will rewrite the HW entries as it was
*       4. set entry with different setting then previous setting
*          a. function will remove the previous settings
*          b. function will set new settings
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           distribute MC/Cascade trunk traffic among the enabled members
*       else
*           1. Set all member ports bits with 0
*           2. If designated member is one of the enabled members, set its relevant
*           bits on all indexes to 1.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*       enabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfEnabledMembers = 0)
*       numOfEnabledMembers - number of enabled members in the array.
*       disabledMembersArray - (array of) members to set in this trunk as disabled
*                     members .
*                    (this parameter ignored if numOfDisabledMembers = 0)
*       numOfDisabledMembers - number of disabled members in the array.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE - when the sum of number of enabled members + number of
*                         disabled members exceed the number of maximum number
*                         of members in trunk (total of 0 - 8 members allowed)
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST - one of the members already exists in another trunk
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMembersSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    enabledMembersArray[],
    IN GT_U32                   numOfDisabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    disabledMembersArray[]
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return prvCpssGenericTrunkMembersSet(devNum,trunkId,
            numOfEnabledMembers,enabledMembersArray,
            numOfDisabledMembers,disabledMembersArray);
}

/*******************************************************************************
* cpssDxChTrunkCascadeTrunkPortsSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function sets the 'cascade' trunk with the specified 'Local ports'
*       overriding any previous setting.
*       The cascade trunk may be invalidated/unset by portsMembersPtr = NULL.
*       Local ports are ports of only configured device.
*       Cascade trunk is:
*           - members are ports of only configured device pointed by devNum
*           - trunk members table is empty (see cpssDxChTrunkTableEntrySet)
*             Therefore it cannot be used as target by ingress engines like FDB,
*             Router, TTI, Ingress PCL and so on.
*           - members ports trunk ID is not set (see cpssDxChTrunkPortTrunkIdSet).
*             Therefore packets those ingresses in member ports are not associated with trunk
*           - all members are enabled only and cannot be disabled.
*           - may be used for cascade traffic and pointed by the 'Device map table'
*             as the local target to reach to the 'Remote device'.
*             (For 'Device map table' refer to cpssDxChCscdDevMapTableSet(...))
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*       portsMembersPtr - (pointer to) local ports bitmap to be members of the
*                   cascade trunk.
*                   NULL - meaning that the trunk-id is 'invalidated' and
*                          trunk-type will be changed to : CPSS_TRUNK_TYPE_FREE_E
*                   not-NULL - meaning that the trunk-type will be : CPSS_TRUNK_TYPE_CASCADE_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - there are ports in the bitmap that not supported by
*                            the device.
*       GT_BAD_PARAM       - bad device number , or bad trunkId number , or number
*                            of ports (in the bitmap) larger then the number of
*                            entries in the 'Designated trunk table'
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_CASCADE_E
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk ,
*                            or this trunk hold members defined using cpssDxChTrunkMembersSet(...)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*   1. This function does not set ports are 'Cascade ports' (and also not
*       check that ports are 'cascade').
*   2. This function sets only next tables :
*       a. the designated trunk table:
*          distribute MC/Cascade trunk traffic among the members
*       b. the 'Non-trunk' table entry.
*   3. because this function not set the 'Trunk members' table entry , the
*       application should not point to this trunk from any ingress unit , like:
*       FDB , PCL action redirect , NH , TTI action redirect , PVE ...
*       (it should be pointed ONLY from the device map table)
*   4. Application can manipulate the 'Per port' trunk-id , for those ports ,
*       using the 'Low level API' of : cpssDxChTrunkPortTrunkIdSet(...)
*   5. this API supports only trunks with types : CPSS_TRUNK_TYPE_FREE_E or
*       CPSS_TRUNK_TYPE_CASCADE_E.
*   6. next APIs are not supported from trunk with type : CPSS_TRUNK_TYPE_CASCADE_E
*       cpssDxChTrunkMembersSet ,
*       cpssDxChTrunkMemberAdd , cpssDxChTrunkMemberRemove,
*       cpssDxChTrunkMemberEnable , cpssDxChTrunkMemberDisable
*       cpssDxChTrunkDbEnabledMembersGet , cpssDxChTrunkDbDisabledMembersGet
*       cpssDxChTrunkDesignatedMemberSet , cpssDxChTrunkDbDesignatedMemberGet
*
********************************************************************************
*   Comparing the 2 function :
*
*        cpssDxChTrunkCascadeTrunkPortsSet   |   cpssDxChTrunkMembersSet
*   ----------------------------------------------------------------------------
*   1. purpose 'Cascade trunk'               | 1. purpose 'Network trunk' , and
*                                            |    also 'Cascade trunk' with up to
*                                            |    8 members
*   ----------------------------------------------------------------------------
*   2. supported number of members depends   | 2. supports up to 8 members
*      on number of entries in the           |    (also in Lion).
*      'Designated trunk table'              |
*      -- Lion supports 64 entries (so up to |
*         64 ports in the 'Cascade trunk').  |
*      -- all other devices supports 8       |
*         entries (so up to 8 ports in the   |
*         'Cascade trunk').                  |
*   ----------------------------------------------------------------------------
*   3. manipulate only 'Non-trunk' table and | 3. manipulate all trunk tables :
*      'Designated trunk' table              |  'Per port' trunk-id , 'Trunk members',
*                                            |  'Non-trunk' , 'Designated trunk' tables.
*   ----------------------------------------------------------------------------
*   4. ingress unit must not point to this   | 4. no restriction on ingress/egress
*      trunk (because 'Trunk members' entry  |    units.
*       hold no ports)                       |
*   ----------------------------------------------------------------------------
*   5. not associated with trunk Id on       | 5. for cascade trunk : since 'Per port'
*      ingress                               | the trunk-Id is set , then load balance
*                                            | according to 'Local port' for traffic
*                                            | that ingress cascade trunk and
*                                            | egress next cascade trunk , will
*                                            | egress only from one of the egress
*                                            | trunk ports. (because all ports associated
*                                            | with the trunk-id)
*   ----------------------------------------------------------------------------
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkCascadeTrunkPortsSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *portsMembersPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return prvCpssGenericTrunkCascadeTrunkPortsSet(devNum,trunkId,portsMembersPtr);
}

/*******************************************************************************
* cpssDxChTrunkCascadeTrunkPortsGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       Gets the 'Local ports' of the 'cascade' trunk .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*
* OUTPUTS:
*       portsMembersPtr - (pointer to) local ports bitmap of the cascade trunk.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PARAM       - bad device number , or bad trunkId number
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_CASCADE_E
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkCascadeTrunkPortsGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    OUT CPSS_PORTS_BMP_STC       *portsMembersPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return prvCpssGenericTrunkCascadeTrunkPortsGet(devNum,trunkId,portsMembersPtr);
}

/*******************************************************************************
* cpssDxChTrunkMemberAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function add member to the trunk in the device.
*       If member is already in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members,
*           now taking into account also the added member
*       else
*           1. If added member is not the designated member - set its relevant bits to 0
*           2. If added member is the designated member & it's enabled,
*              set its relevant bits on all indexes to 1.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number on which to add member to the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to add to the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST         - this member already exists in another trunk.
*       GT_FULL - trunk already contains maximum supported members
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMemberAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return prvCpssGenericTrunkMemberAdd(devNum,trunkId,memberPtr);
}

/*******************************************************************************
* cpssDxChTrunkMemberRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function remove member from a trunk in the device.
*       If member not exists in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the removed member
*       else
*           1. If removed member is not the designated member - nothing to do
*           2. If removed member is the designated member set its relevant bits
*              on all indexes to 1.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number on which to remove member from the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to remove from the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMemberRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return prvCpssGenericTrunkMemberRemove(devNum,trunkId,memberPtr);
}

/*******************************************************************************
* cpssDxChTrunkMemberDisable
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function disable (enabled)existing member of trunk in the device.
*       If member is already disabled in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the disabled member
*       else
*           1. If disabled member is not the designated member - set its relevant bits to 0
*           2. If disabled member is the designated member set its relevant bits
*               on all indexes to 0.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number on which to disable member in the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to disable in the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMemberDisable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return prvCpssGenericTrunkMemberDisable(devNum,trunkId,memberPtr);
}

/*******************************************************************************
* cpssDxChTrunkMemberEnable
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function enable (disabled)existing member of trunk in the device.
*       If member is already enabled in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the enabled member
*       else
*           1. If enabled member is not the designated member - set its relevant bits to 0
*           2. If enabled member is the designated member set its relevant bits
*               on all indexes to 1.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number on which to enable member in the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to enable in the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMemberEnable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return prvCpssGenericTrunkMemberEnable(devNum,trunkId,memberPtr);
}


/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsAdd
*
* DESCRIPTION:
*       Function Relevant mode : Low Level mode
*
*       add the ports to the trunk's non-trunk entry .
*       NOTE : the ports are add to the "non trunk" table only and not effect
*              other trunk relate tables/registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsBmpPtr - (pointer to)bitmap of ports to add to
*                             "non-trunk members"
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkNonTrunkPortsAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(nonTrunkPortsBmpPtr);

    return prvCpssGenericTrunkNonTrunkPortsAdd(devNum,trunkId,nonTrunkPortsBmpPtr);
}


/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsRemove
*
* DESCRIPTION:
*       Function Relevant mode : Low Level mode
*
*      Removes the ports from the trunk's non-trunk entry .
*      NOTE : the ports are removed from the "non trunk" table only and not
*              effect other trunk relate tables/registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsBmpPtr - (pointer to)bitmap of ports to remove from
*                             "non-trunk members"
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkNonTrunkPortsRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(nonTrunkPortsBmpPtr);

    return prvCpssGenericTrunkNonTrunkPortsRemove(devNum,trunkId,nonTrunkPortsBmpPtr);
}

/*******************************************************************************
* cpssDxChTrunkInit
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS DxCh Trunk initialization of PP Tables/registers and
*       SW shadow data structures, all ports are set as non-trunk ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*       maxNumberOfTrunks  - maximum number of trunk groups.(0..127)
*                            when this number is 0 , there will be no shadow used
*                            Note:
*                            that means that API's that used shadow will FAIL.
*       trunkMembersMode   - type of how the CPSS SW will fill the HW with
*                            trunk members
*
* OUTPUTS:
*           None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_BAD_PARAM             - wrong devNum or bad trunkLbhMode
*       GT_OUT_OF_RANGE          - the numberOfTrunks > 127
*       GT_BAD_STATE             - if library already initialized with different
*                                  number of trunks than requested
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkInit
(
    IN  GT_U8       devNum,
    IN  GT_U8       maxNumberOfTrunks,
    IN  CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT trunkMembersMode
)
{
    GT_STATUS   rc;
    PRV_CPSS_TRUNK_DB_INFO_STC      *devTrunkInfoPtr;
    DXCH_TRUNK_SPECIFIC_INFO_STC    *extraTrunksInfoPtr;/*(pointer to)extra trunk info*/
    EXTRA_TRUNK_ENTRY_STC           *currWaTrunkInfoPtr;
    GT_U32  ii,jj;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(1 ==
        PRV_CPSS_DXCH_CHECK_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_MAC(devNum))
    {
        if(PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
            extraTrunksInfoPtr == NULL)
        {
            extraTrunksInfoPtr = cpssOsMalloc(sizeof(DXCH_TRUNK_SPECIFIC_INFO_STC));
            if(extraTrunksInfoPtr == NULL)
            {
                return GT_OUT_OF_CPU_MEM;
            }

            cpssOsMemSet(extraTrunksInfoPtr,0,sizeof(DXCH_TRUNK_SPECIFIC_INFO_STC));

            PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
                info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
                extraTrunksInfoPtr = extraTrunksInfoPtr;

            currWaTrunkInfoPtr = &extraTrunksInfoPtr->extraTrunkInfoArray[0];

            for(ii = 0;
                ii < XCAT_NUM_TRUNKS_FOR_WA_WRONG_SOURCE_PORT_TO_CPU;
                ii++ , currWaTrunkInfoPtr++)
            {
                for(jj = 0; jj < PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS; jj++)
                {
                    currWaTrunkInfoPtr->errata.
                        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.
                            upToMemberMap[jj].port = INVALID_TRUNK_MEMBER_VALUE_CNS;

                    currWaTrunkInfoPtr->errata.
                        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.
                            upToMemberMap[jj].device = INVALID_TRUNK_MEMBER_VALUE_CNS;

                    /* set the UPs to use */
                    currWaTrunkInfoPtr->errata.
                        info_WRONG_TRUNK_ID_SOURCE_PORT_INFORMATION_OF_PACKET_TO_CPU.
                            upToUseArr[jj] = (GT_U8)jj;
                }
            }
        }
    }

    rc = prvCpssGenericTrunkInit(devNum,maxNumberOfTrunks);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the trunk members filling mode */
    devTrunkInfoPtr = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum);
    switch(trunkMembersMode)
    {
        case CPSS_DXCH_TRUNK_MEMBERS_MODE_NATIVE_E:
            break;
        case CPSS_DXCH_TRUNK_MEMBERS_MODE_SOHO_EMULATION_E:
            devTrunkInfoPtr->loadBalanceType = PRV_CPSS_TRUNK_LOAD_BALANCE_SX_EMULATION_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChTrunkFamilyInit
*
* DESCRIPTION:
*       This function sets CPSS private DxCh family info about trunk behavior
*       that relate to the family and not to specific device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devFamily - device family.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - on failure.
*       GT_NOT_INITIALIZED       - the family was not initialized properly
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChTrunkFamilyInit(
    IN CPSS_PP_FAMILY_TYPE_ENT  devFamily
)
{
    PRV_CPSS_FAMILY_TRUNK_BIND_FUNC_STC *bindFuncPtr;

    if(prvCpssFamilyInfoArray[devFamily] == NULL)
    {
        return GT_NOT_INITIALIZED;
    }

    bindFuncPtr = &prvCpssFamilyInfoArray[devFamily]->trunkInfo.boundFunc;

    bindFuncPtr->dbFlagsInitFunc             = trunkDbFlagsInit;

    bindFuncPtr->portTrunkIdSetFunc          = cpssDxChTrunkPortTrunkIdSet;
    bindFuncPtr->membersSetFunc              = cpssDxChTrunkTableEntrySet;
    bindFuncPtr->nonMembersBmpSetFunc        = cpssDxChTrunkNonTrunkPortsEntrySet;
    bindFuncPtr->nonMembersBmpGetFunc        = cpssDxChTrunkNonTrunkPortsEntryGet;
    bindFuncPtr->designatedMembersBmpSetFunc = cpssDxChTrunkDesignatedPortsEntrySet;
    bindFuncPtr->designatedMembersBmpGetFunc = cpssDxChTrunkDesignatedPortsEntryGet;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashMplsModeEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the MPLS information,
*       when calculating the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant to NON-MPLS packets.
*       2. Relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E)
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - the device number.
*       enable - GT_FALSE - MPLS parameter are not used in trunk hash index
*                GT_TRUE  - The following function is added to the trunk load
*                   balancing hash:
*               MPLSTrunkHash = (mpls_label0[5:0] & mpls_trunk_lbl0_mask) ^
*                               (mpls_label1[5:0] & mpls_trunk_lbl1_mask) ^
*                               (mpls_label2[5:0] & mpls_trunk_lbl2_mask)
*               NOTE:
*                   If any of MPLS Labels 0:2 do not exist in the packet,
*                   the default value 0x0 is used for TrunkHash calculation
*                   instead.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMplsModeEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* value to write to register               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];

    value = (enable == GT_TRUE) ? 1 : 0;

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, 6, 1, value);
}


/*******************************************************************************
* cpssDxChTrunkHashMplsModeEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get trunk MPLS hash mode
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr - (pointer to)the MPLS hash mode.
*               GT_FALSE - MPLS parameter are not used in trunk hash index
*               GT_TRUE  - The following function is added to the trunk load
*                   balancing hash:
*               MPLSTrunkHash = (mpls_label0[5:0] & mpls_trunk_lbl0_mask) ^
*                               (mpls_label1[5:0] & mpls_trunk_lbl1_mask) ^
*                               (mpls_label2[5:0] & mpls_trunk_lbl2_mask)
*               NOTE:
*                   If any of MPLS Labels 0:2 do not exist in the packet,
*                   the default value 0x0 is used for TrunkHash calculation
*                   instead.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMplsModeEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_STATUS   rc;              /* return code                              */
    GT_U32      regAddr;         /* register address                         */
    GT_U32      value;           /* value to write to register               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 6, 1, &value);

    *enablePtr = value ? GT_TRUE : GT_FALSE;

    return rc;
}

/*******************************************************************************
* prvCpssDxChTrunkHashMaskPositionGet
*
* DESCRIPTION:
*       Get trunk hash mask control register addres and bit position
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - the device number.
*       maskedField - field to apply the mask on
*
* OUTPUTS:
*       regAddrPtr     - (pointer to) address of hash control register
*       fieldOffsetPtr - (pointer to) offset of field in register in bits (0..31)
*       fieldSizePtr   - (pointer to) size of field in register in bits (0..31)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad regIndex
*
* COMMENTS:
*       None
*******************************************************************************/
static GT_STATUS prvCpssDxChTrunkHashMaskPositionGet
(
    IN  GT_U8                         devNum,
    IN CPSS_DXCH_TRUNK_LBH_MASK_ENT   maskedField,
    OUT GT_U32                        *regAddrPtr,
    OUT GT_U32                        *fieldOffsetPtr,
    OUT GT_U32                        *fieldSizePtr
)
{
    /* all mask are 6-bit sized */
    *fieldSizePtr   = 6;

    switch (maskedField)
    {
        case CPSS_DXCH_TRUNK_LBH_MASK_MAC_DA_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[1];
            *fieldOffsetPtr = 6;
            break;
        case CPSS_DXCH_TRUNK_LBH_MASK_MAC_SA_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[1];
            *fieldOffsetPtr = 0;
            break;
        case CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL0_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[2];
            *fieldOffsetPtr = 0;
            break;
        case CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL1_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[2];
            *fieldOffsetPtr = 6;
            break;
        case CPSS_DXCH_TRUNK_LBH_MASK_MPLS_LABEL2_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[2];
            *fieldOffsetPtr = 12;
            break;
        case CPSS_DXCH_TRUNK_LBH_MASK_IPV4_DIP_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[1];
            *fieldOffsetPtr = 18;
            break;
        case CPSS_DXCH_TRUNK_LBH_MASK_IPV4_SIP_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[1];
            *fieldOffsetPtr = 12;
            break;
        case CPSS_DXCH_TRUNK_LBH_MASK_IPV6_DIP_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
            *fieldOffsetPtr = 14;
            break;
        case CPSS_DXCH_TRUNK_LBH_MASK_IPV6_SIP_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];
            *fieldOffsetPtr = 8;
            break;
        case CPSS_DXCH_TRUNK_LBH_MASK_IPV6_FLOW_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[1];
            *fieldOffsetPtr = 24;
            break;
        case CPSS_DXCH_TRUNK_LBH_MASK_L4_DST_PORT_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[2];
            *fieldOffsetPtr = 24;
            break;
        case CPSS_DXCH_TRUNK_LBH_MASK_L4_SRC_PORT_E:
            *regAddrPtr =
                PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[2];
            *fieldOffsetPtr = 18;
            break;
        default : return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashMaskSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the masks for the various packet fields being used at the Trunk
*       hash calculations
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum      - the device number.
*       maskedField - field to apply the mask on
*       maskValue   - The mask value to be used (0..0x3F)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or maskedField
*       GT_OUT_OF_RANGE          - maskValue > 0x3F
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMaskSet
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_TRUNK_LBH_MASK_ENT   maskedField,
    IN GT_U8                          maskValue
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* address of hash control register    */
    GT_U32    fieldOffset;   /* offset of field in register in bits */
    GT_U32    fieldSize;     /* size of field in register in bits   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    rc = prvCpssDxChTrunkHashMaskPositionGet(
        devNum, maskedField, &regAddr, &fieldOffset, &fieldSize);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (maskValue >= (GT_U8)(1 << fieldSize))
    {
        return GT_OUT_OF_RANGE;
    }

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, fieldOffset, fieldSize, maskValue);
}

/*******************************************************************************
* cpssDxChTrunkHashMaskGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the masks for the various packet fields being used at the Trunk
*       hash calculations
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - the device number.
*       maskedField - field to apply the mask on
*
* OUTPUTS:
*       maskValuePtr - (pointer to)The mask value to be used (0..0x3F)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or maskedField
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMaskGet
(
    IN GT_U8                          devNum,
    IN CPSS_DXCH_TRUNK_LBH_MASK_ENT   maskedField,
    OUT GT_U8                         *maskValuePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* address of hash control register    */
    GT_U32    fieldOffset;   /* offset of field in register in bits */
    GT_U32    fieldSize;     /* size of field in register in bits   */
    GT_U32    value;         /* field value                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(maskValuePtr);

    rc = prvCpssDxChTrunkHashMaskPositionGet(
        devNum, maskedField, &regAddr, &fieldOffset, &fieldSize);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, fieldOffset, fieldSize, &value);

    *maskValuePtr = (GT_U8)value;

    return rc;
}

/*******************************************************************************
* prvCpssDxChTrunkHashShiftPositionGet
*
* DESCRIPTION:
*       Get trunk hash shift control register addres and bit position
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum        - the device number.
*       protocolStack - Set the shift to either IPv4 or IPv6 IP addresses.
*       isSrcIp       - GT_TRUE  = Set the shift to IPv4/6 source addresses.
*                       GT_FALSE = Set the shift to IPv4/6 destination addresses.
*
* OUTPUTS:
*       regAddrPtr     - (pointer to) address of hash control register
*       fieldOffsetPtr - (pointer to) offset of field in register in bits (0..31)
*       fieldSizePtr   - (pointer to) size of field in register in bits (0..31)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad regIndex
*
* COMMENTS:
*       None
*******************************************************************************/
static GT_STATUS prvCpssDxChTrunkHashShiftPositionGet
(
    IN  GT_U8                         devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT     protocolStack,
    IN GT_BOOL                        isSrcIp,
    OUT GT_U32                        *regAddrPtr,
    OUT GT_U32                        *fieldOffsetPtr,
    OUT GT_U32                        *fieldSizePtr
)
{
    /* all shiftes are in the same register */
    *regAddrPtr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.trunkHashCfg[0];

    switch (protocolStack)
    {
        case CPSS_IP_PROTOCOL_IPV6_E:
            *fieldSizePtr   = 4;
            *fieldOffsetPtr = (isSrcIp == GT_FALSE) ? 24 : 28;
            break;
        case CPSS_IP_PROTOCOL_IPV4_E:
            *fieldSizePtr   = 2;
            *fieldOffsetPtr = (isSrcIp == GT_FALSE) ? 20 : 22;
            break;
        default : return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashIpShiftSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the shift being done to IP addresses prior to hash calculations.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - the device number.
*       protocolStack - Set the shift to either IPv4 or IPv6 IP addresses.
*       isSrcIp       - GT_TRUE  = Set the shift to IPv4/6 source addresses.
*                       GT_FALSE = Set the shift to IPv4/6 destination addresses.
*       shiftValue    - The shift to be done.
*                       IPv4 valid shift: 0-3 bytes (Value = 0: no shift).
*                       IPv6 valid shift: 0-15 bytes (Value = 0: no shift).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or protocolStack
*       GT_OUT_OF_RANGE - shiftValue > 3 for IPv4 , shiftValue > 15 for IPv6
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpShiftSet
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      isSrcIp,
    IN GT_U32                       shiftValue
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* address of hash control register    */
    GT_U32    fieldOffset;   /* offset of field in register in bits */
    GT_U32    fieldSize;     /* size of field in register in bits   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    rc = prvCpssDxChTrunkHashShiftPositionGet(
        devNum, protocolStack, isSrcIp, &regAddr, &fieldOffset, &fieldSize);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (shiftValue >= (GT_U32)(1 << fieldSize))
    {
        return GT_OUT_OF_RANGE;
    }

    return prvCpssDrvHwPpSetRegField(
        devNum, regAddr, fieldOffset, fieldSize, shiftValue);
}

/*******************************************************************************
* cpssDxChTrunkHashIpShiftGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the shift being done to IP addresses prior to hash calculations.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - the device number.
*       protocolStack - Set the shift to either IPv4 or IPv6 IP addresses.
*       isSrcIp       - GT_TRUE  = Set the shift to IPv4/6 source addresses.
*                       GT_FALSE = Set the shift to IPv4/6 destination addresses.
*
* OUTPUTS:
*       shiftValuePtr    - (pointer to) The shift to be done.
*                       IPv4 valid shift: 0-3 bytes (Value = 0: no shift).
*                       IPv6 valid shift: 0-15 bytes (Value = 0: no shift).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or protocolStack
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashIpShiftGet
(
    IN GT_U8                        devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT   protocolStack,
    IN GT_BOOL                      isSrcIp,
    OUT GT_U32                      *shiftValuePtr
)
{
    GT_STATUS rc;            /* return code                         */
    GT_U32    regAddr;       /* address of hash control register    */
    GT_U32    fieldOffset;   /* offset of field in register in bits */
    GT_U32    fieldSize;     /* size of field in register in bits   */
    GT_U32    value;         /* field value                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(shiftValuePtr);

    rc = prvCpssDxChTrunkHashShiftPositionGet(
        devNum, protocolStack, isSrcIp, &regAddr, &fieldOffset, &fieldSize);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpGetRegField(
        devNum, regAddr, fieldOffset, fieldSize, &value);

    *shiftValuePtr = (GT_U8)value;

    return rc;
}

/*******************************************************************************
* prvCpssDxChTrunkHwDevNumSet
*
* DESCRIPTION:
*        Update trunk logic prior change of HW device number.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       hwDevNum - HW device number (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameters
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTrunkHwDevNumSet
(
    IN GT_U8    devNum,
    IN GT_U8    hwDevNum
)
{
    GT_U32  value;/*value to write to register */
    GT_U32  port;/* port index*/
    GT_U32  additionalInfoBmp; /* bitmap for additional WA info */
    GT_U32  regAddr; /* base address */
    GT_U32  oldHwDevNum;/* hw dev num before the change to 'hwDevNum'*/
    GT_STATUS rc;

    if(0 ==
        PRV_CPSS_DXCH_CHECK_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_MAC(devNum))
    {
        return  GT_OK;
    }

    oldHwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum);

    if(oldHwDevNum == hwDevNum)
    {
        return GT_OK;
    }

    additionalInfoBmp = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.
        additionalInfoBmp;

    if(((additionalInfoBmp >> 1) & 0x1) == 0)
    {
        return GT_OK;
    }

    if (((additionalInfoBmp >> 2) & 0x1) == 0)
    {
        regAddr = 0x0f808000;
    }
    else
    {
        regAddr = 0x0f80A000;
    }

    for(port = 0 ; port < 64 ; port++ , regAddr+=4)
    {

        rc = prvCpssDrvHwPpReadRegister(devNum,
                                    regAddr + oldHwDevNum*256 ,&value);
        if(rc !=GT_OK)
        {
            return rc;
        }
        rc = prvCpssDrvHwPpWriteRegister(devNum,
                                    regAddr + hwDevNum*256 ,value);
        if(rc !=GT_OK)
        {
            return rc;
        }
        /* invalidate old entries */
        rc = prvCpssDrvHwPpWriteRegister(devNum,
                                    regAddr + oldHwDevNum*256 ,0);
        if(rc !=GT_OK)
        {
            return rc;
        }

    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChTrunkHashCrcParametersSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the CRC hash parameter , relevant for CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E .
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The device number.
*       crcMode - The CRC mode .
*       crcSeed - The seed used by the CRC computation .
*                 when crcMode is CRC_6 mode : crcSeed is in range of 0..0x3f (6 bits value)
*                 when crcMode is CRC_16 mode : crcSeed is in range of 0..0xffff (16 bits value)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or crcMode
*       GT_OUT_OF_RANGE          - crcSeed out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashCrcParametersSet
(
    IN GT_U8                                devNum,
    IN CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT     crcMode,
    IN GT_U32                               crcSeed
)
{
    GT_U32    regAddr;       /* register address    */
    GT_U32    value;         /* value to write to the register */
    GT_U32    mask;          /* mask the bits to write to the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    switch(crcMode)
    {
        case CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E:
            if(crcSeed >= BIT_6)
            {
                return GT_OUT_OF_RANGE;
            }

            mask  = (GT_U32)(BIT_1 | (0x3F << 8));/* bit 1 and bits 8..13 */
            value = BIT_1 | crcSeed << 8;/* bit 1 with value 1 , bits 8..13 with value of seed */

            break;
        case CPSS_DXCH_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E:
            if(crcSeed >= BIT_16)
            {
                return GT_OUT_OF_RANGE;
            }

            mask  = (GT_U32)(BIT_1 | (0xFFFF << 16));/* bit 1 and bits 16..31 */
            value = crcSeed << 16; /* bit 1 with value 0 , bits 16..31 with value of seed */
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.crcHashConfigurationReg;

    return prvCpssDrvHwPpWriteRegBitMask(devNum, regAddr, mask, value);
}

/*******************************************************************************
* cpssDxChTrunkHashCrcParametersGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the CRC hash parameter , relevant for CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E .
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The device number.
*
* OUTPUTS:
*       crcModePtr - (pointer to) The CRC mode .
*       crcSeedPtr - (pointer to) The seed used by the CRC computation .
*                 when crcMode is CRC_6 mode : crcSeed is in range of 0..0x3f (6 bits value)
*                 when crcMode is CRC_16 mode : crcSeed is in range of 0..0xffff (16 bits value)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashCrcParametersGet
(
    IN GT_U8                                devNum,
    OUT CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT     *crcModePtr,
    OUT GT_U32                               *crcSeedPtr
)
{
    GT_STATUS rc;            /* return code         */
    GT_U32    regAddr;       /* register address    */
    GT_U32    value;         /* value read from the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(crcModePtr);
    CPSS_NULL_PTR_CHECK_MAC(crcSeedPtr);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.crcHashConfigurationReg;
    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &value);

    if(U32_GET_FIELD_MAC(value,1,1))  /* CRC Hash Sub-Mode */
    {
        *crcModePtr = CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E;
        *crcSeedPtr = U32_GET_FIELD_MAC(value,8,6);
    }
    else
    {
        *crcModePtr = CPSS_DXCH_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E;
        *crcSeedPtr = U32_GET_FIELD_MAC(value,16,16);
    }

    return rc;
}

/*******************************************************************************
* cpssDxChTrunkPortHashMaskInfoSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set port-based hash mask info.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The device number.
*       portNum - The port number.
*       overrideEnable - enable/disable the override
*       index - the index to use when 'Override enabled'.
*               (values 0..15) , relevant only when overrideEnable = GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or portNum
*       GT_OUT_OF_RANGE          - when overrideEnable is enabled and index out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkPortHashMaskInfoSet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    IN GT_BOOL   overrideEnable,
    IN GT_U32    index
)
{
    GT_U32    value;         /* value to write to the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);

    if(overrideEnable == GT_TRUE)
    {
        if(index >= BIT_4)
        {
            /* 4 bits in HW */
            return GT_OUT_OF_RANGE;
        }

        value = BIT_0 | index << 1;
    }
    else
    {
        value = 0;
    }

    /* trunk id of port control it accessed via interface to :
       Port<n> VLAN and QoS Configuration Entry
       bit 66 , 67..70
    */
    return prvCpssDxChWriteTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,/* global bit */
                                        66,/* start bit 66 , 67..70 */
                                        5,/* 5 bits */
                                        value);
}

/*******************************************************************************
* cpssDxChTrunkPortHashMaskInfoGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get port-based hash mask info.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - The device number.
*       portNum - The port number.
*
* OUTPUTS:
*       overrideEnablePtr - (pointer to)enable/disable the override
*       indexPtr - (pointer to)the index to use when 'Override enabled'.
*               (values 0..15) , relevant only when overrideEnable = GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or portNum
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkPortHashMaskInfoGet
(
    IN GT_U8     devNum,
    IN GT_U8     portNum,
    OUT GT_BOOL  *overrideEnablePtr,
    OUT GT_U32   *indexPtr
)
{
    GT_STATUS rc;            /* return code         */
    GT_U32    value;         /* value read from the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(overrideEnablePtr);
    CPSS_NULL_PTR_CHECK_MAC(indexPtr);

    /* trunk id of port control it accessed via interface to :
       Port<n> VLAN and QoS Configuration Entry
       bit 66 , 67..70
    */
    rc = prvCpssDxChReadTableEntryField(devNum,
                                        PRV_CPSS_DXCH_TABLE_PORT_VLAN_QOS_E,
                                        portNum,
                                        PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,/* global bit */
                                        66,/* start bit 66 , 67..70 */
                                        5,/* 5 bits */
                                        &value);

    if(value & 1)
    {
        *overrideEnablePtr = GT_TRUE;
    }
    else
    {
        *overrideEnablePtr = GT_FALSE;
    }

    *indexPtr = value >> 1;

    return rc;
}

/*******************************************************************************
* swap32BitsAndShift
*
* DESCRIPTION:
*       swap length bits value and right shift it back to bit 0
*
* INPUTS:
*       valueToSwap  - the value to swap
*       numOfBits    - number of bits in the value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       the swapped value after right shifted back to start at bit 0.
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32 swap32BitsAndShift(
    IN GT_U32   valueToSwap,
    IN GT_U32   numOfBits
)
{
    GT_U32  tmp; /* temp value*/
    GT_U32  swapped;/* the swapped value */

    tmp = valueToSwap >> 24;
    swapped  = (U8_SWAP_BITS(tmp)) << 0;
    tmp = valueToSwap >> 16;
    swapped |= (U8_SWAP_BITS(tmp)) << 8;
    tmp = valueToSwap >> 8;
    swapped |= (U8_SWAP_BITS(tmp)) << 16;
    tmp = valueToSwap >> 0;
    swapped |= (U8_SWAP_BITS(tmp)) << 24;

    return swapped >> (32-numOfBits);
}

/*******************************************************************************
* hashMaskFieldDestinationGet
*
* DESCRIPTION:
*       get fields distance and number info and pointer to first field
*
* INPUTS:
*       entryPtr - entry of type CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC.
*
* OUTPUTS:
*       numOfFieldsPtr   - (pointer to) the number of GT_U32 fields in entryPtr
*       fieldsDestinationPtr - (pointer to) the distance between 2 fields in entryPtr (value is in GT_U32 steps)
*       fieldPtrPtr -(pointer to) pointer to first GT_U32 field in  entryPtr
*
* RETURNS:
*       none
*
* COMMENTS:
*
*******************************************************************************/
static void hashMaskFieldDestinationGet
(
    IN CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC *entryPtr,
    OUT GT_U32  *numOfFieldsPtr,
    OUT GT_U32  *fieldsDestinationPtr,
    OUT GT_U32  **fieldPtrPtr
)
{
    *fieldsDestinationPtr =
        (hashMaskCrcFieldsInfo[1].offsetOfSwFieldForStcStart -
         hashMaskCrcFieldsInfo[0].offsetOfSwFieldForStcStart) / sizeof(GT_U32);

    /* pointer to first field */
    *fieldPtrPtr = (GT_U32*)(((char*)entryPtr) + hashMaskCrcFieldsInfo[0].offsetOfSwFieldForStcStart);

    *numOfFieldsPtr = sizeof(hashMaskCrcFieldsInfo) / sizeof(hashMaskCrcFieldsInfo[0]);

    return ;
}

/*******************************************************************************
* cpssDxChTrunkHashMaskCrcEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the entry of masks in the specified index in 'CRC hash mask table'.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - the device number.
*       index   - the table index (0..27)
*       entryPtr - (pointer to) The entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_OUT_OF_RANGE          - one of the fields in entryPtr are out of range
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMaskCrcEntrySet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    IN CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC *entryPtr
)
{
    GT_U32  mask;
    GT_U32  length;/*field length*/
    GT_U32  fieldMaskBmp;/* field mask bmp */
    GT_U32  fieldType;/*field type*/
    GT_U32  *fieldPtr;/* pointer to the current field in the entry */
    GT_U32  hwEntry[HASH_MASK_CRC_ENTRY_NUM_WORDS_CNS];/* values read from HW and need to combined to write back to HW */
    GT_U32  numOfFields;/* number of fields in the entry */
    GT_U32  fieldsDestination;/* 2 fields destination (value is in GT_U32 steps)*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    HASH_MASK_CRC_INDEX_CHECK_MAC(devNum,index);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    /* NOTE: the index is checked in prvCpssDxChWriteTableEntry() */

    /* get fields distance and number info and pointer to first field */
    hashMaskFieldDestinationGet(entryPtr,&numOfFields,&fieldsDestination,&fieldPtr);

    cpssOsMemSet(hwEntry,0,sizeof(hwEntry));

    for(fieldType = 0 ; fieldType < numOfFields ; fieldType++ , fieldPtr += fieldsDestination)
    {
        length = hashMaskCrcFieldsInfo[fieldType].length;
        /* mask according to length of field */
        mask = BIT_MASK_MAC(length);

        if(hashMaskCrcFieldsInfo[fieldType].offset)
        {
            /* the value is shifted , so need to shift the mask */
            mask <<= hashMaskCrcFieldsInfo[fieldType].offset;
        }

        fieldMaskBmp = *fieldPtr;

        if((fieldMaskBmp & (~mask)))
        {
            /* there are bits in the bitmap that are not supported by the
               device : 'out of range' */
            return GT_OUT_OF_RANGE;
        }

        if(hashMaskCrcFieldsInfo[fieldType].offset)
        {
            /* the value is shifted and we already shifted the mask to ,
               so we need to 'restore' the values to start at bit 0 */
            fieldMaskBmp >>= hashMaskCrcFieldsInfo[fieldType].offset;
            mask         >>= hashMaskCrcFieldsInfo[fieldType].offset;
        }

        if(hashMaskCrcFieldsInfo[fieldType].needToSwap == GT_TRUE)
        {
            /* swap the values that the caller set */
            /* the swap will swap bits of the value and shift it to start from bit 0 */
            fieldMaskBmp = swap32BitsAndShift(fieldMaskBmp,length);
        }

        /* set values from caller */
        U32_SET_FIELD_IN_ENTRY_MAC(hwEntry,hashMaskCrcFieldsInfo[fieldType].startBit,length,
            fieldMaskBmp);
    }

    /* write the table entry */
    return prvCpssDxChWriteTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION_TABLE_TRUNK_HASH_MASK_CRC_E,
                                   index,
                                   hwEntry);
}

/*******************************************************************************
* cpssDxChTrunkHashMaskCrcEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the entry of masks in the specified index in 'CRC hash mask table'.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - the device number.
*       index   - the table index (0..27)
*
* OUTPUTS:
*       entryPtr - (pointer to) The entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashMaskCrcEntryGet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    OUT CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC *entryPtr
)
{
    GT_STATUS rc;            /* return code         */
    GT_U32  length;/*field length*/
    GT_U32  fieldMaskBmp;/* field mask bmp */
    GT_U32  fieldType;/*field type*/
    GT_U32  *fieldPtr;/* pointer to the current field in the entry */
    GT_U32  hwEntry[HASH_MASK_CRC_ENTRY_NUM_WORDS_CNS];/* values read from HW and need to combined to write back to HW */
    GT_U32  numOfFields;/* number of fields in the entry */
    GT_U32  fieldsDestination;/* 2 fields destination (value is in GT_U32 steps)*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    HASH_MASK_CRC_INDEX_CHECK_MAC(devNum,index);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    /* get the table entry */
    rc = prvCpssDxChReadTableEntry(devNum,
                                   PRV_CPSS_DXCH_LION_TABLE_TRUNK_HASH_MASK_CRC_E,
                                   index,
                                   hwEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* get fields distance and number info and pointer to first field */
    hashMaskFieldDestinationGet(entryPtr,&numOfFields,&fieldsDestination,&fieldPtr);

    for(fieldType = 0 ; fieldType < numOfFields ; fieldType++ , fieldPtr += fieldsDestination)
    {
        length = hashMaskCrcFieldsInfo[fieldType].length;

        fieldMaskBmp = 0;

        /* get values from HW */
        U32_GET_FIELD_IN_ENTRY_MAC(hwEntry,hashMaskCrcFieldsInfo[fieldType].startBit,length,
            fieldMaskBmp);

        if(hashMaskCrcFieldsInfo[fieldType].needToSwap == GT_TRUE)
        {
            /* swap the values that the HW returned */
            /* the swap will swap bits of the value and shift it to start from bit 0 */
            fieldMaskBmp = swap32BitsAndShift(fieldMaskBmp,length);
        }

        if(hashMaskCrcFieldsInfo[fieldType].offset)
        {
            /* the value need to be shifted because it not start at bit 0 */
            fieldMaskBmp <<= hashMaskCrcFieldsInfo[fieldType].offset;
        }

        *fieldPtr = fieldMaskBmp;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashPearsonValueSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the Pearson hash value for the specific index.
*
*       NOTE: the Pearson hash used when CRC-16 mode is used.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - the device number.
*       index   - the table index (0..63)
*       value   - the Pearson hash value (0..63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_OUT_OF_RANGE          - value > 63
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashPearsonValueSet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    IN GT_U32                         value
)
{
    GT_U32    regAddr;       /* register address    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    if(index > 63)
    {
        return GT_BAD_PARAM;
    }

    if(value >= BIT_6)
    {
        /* 6 bits in HW */
        return GT_OUT_OF_RANGE;
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pearsonHashTableReg[index / 4];
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, (index % 4)* 8 , 6 ,value);
}

/*******************************************************************************
* cpssDxChTrunkHashPearsonValueGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the Pearson hash value for the specific index.
*
*       NOTE: the Pearson hash used when CRC-16 mode is used.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum  - the device number.
*       index   - the table index (0..63)
*
* OUTPUTS:
*       valuePtr   - (pointer to) the Pearson hash value (0..63)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
GT_STATUS cpssDxChTrunkHashPearsonValueGet
(
    IN GT_U8                          devNum,
    IN GT_U32                         index,
    OUT GT_U32                        *valuePtr
)
{
    GT_U32    regAddr;       /* register address    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E);

    CPSS_NULL_PTR_CHECK_MAC(valuePtr);

    if(index > 63)
    {
        return GT_BAD_PARAM;
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.pearsonHashTableReg[index / 4];

    return prvCpssDrvHwPpGetRegField(devNum, regAddr, (index % 4) * 8 , 6 ,valuePtr);
}

/*******************************************************************************
* prvCpssDxChTrunkDesignatedTableForSrcPortHashMappingSet
*
* DESCRIPTION:
*
*       the function sets the designated device table with the portsArr[].trunkPort
*       in indexes that match hash (%8 or %64(modulo)) on the ports in portsArr[].srcPort
*       this to allow 'Src port' trunk hash for traffic sent to the specified
*       trunk.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       numOfPorts  - number of pairs in array portsArr[].
*       portsArr[] - (array of) pairs of 'source ports' ,'trunk ports'
*                     for the source port hash.
*       mode - hash mode (%8 or %64 (modulo))
*
* OUTPUTS:
*       None
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or trunkId or port in portsArr[].srcPort or
*                   port in portsArr[].trunkPort or mode
*                   or map two Source ports that falls into same Source hash index
*                   into different trunk member ports
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTrunkDesignatedTableForSrcPortHashMappingSet
(
    IN GT_U8             devNum,
    IN GT_U32            numOfPorts,
    IN CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC  portsArr[],
    IN CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT  mode
)
{
    GT_STATUS rc;/* return code */
    GT_U32  ii,jj;/* iterators*/
    CPSS_PORTS_BMP_STC  designatedPorts; /* bmp of current ports in designated entry */
    GT_U32  portsToBeDesignatedArr[MAX_DESIGNATED_TRUNK_ENTRIES_NUM_CNS];/* list of trunk ports that need to be
                    designated for the trunk in the designated trunk table in
                    the same index as they appear in array of portsToBeDesignatedArr[]*/
    GT_U32  hashMode;/* hash mode %8 or %64 */
    GT_U32  numOfDesignatedTrunkEntriesHw;/* number of entries in trunk designated port table in HW */

    numOfDesignatedTrunkEntriesHw = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numOfDesignatedTrunkEntriesHw;

    switch(mode)
    {
        case CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_MODULO_8_E:
            hashMode = 8;
            break;
        case CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_MODULO_64_E:
            hashMode = 64;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(hashMode > numOfDesignatedTrunkEntriesHw)
    {
        return GT_BAD_PARAM;
    }

    if(hashMode > MAX_DESIGNATED_TRUNK_ENTRIES_NUM_CNS)
    {
        /* should update MAX_DESIGNATED_TRUNK_ENTRIES_NUM_CNS */
        return GT_FAIL;
    }

    /* check ports numbers validity */
    for(ii = 0 ; ii < numOfPorts; ii++)
    {
        if(portsArr[ii].srcPort >= BIT_6)
        {
            return GT_BAD_PARAM;
        }

        if(portsArr[ii].trunkPort >= BIT_6)
        {
            return GT_BAD_PARAM;
        }
    }

    /* NOTE :
        when hashMode == 8 and numOfDesignatedTrunkEntriesHw == 64
        application ask for hash of %8 , although HW do %64 .
        so we duplicate the first 8 (0..7) entries 7 more times to indexes :
        8..15 , 16..23 , 24..31 , 32..39 , 40..47 , 48..55 , 56..63
    */

    /* need to override the trunk designated table */
    /* to set 8/64 entries to be appropriate for 'src port' hashing */

    /* start with 'not set yet' values */
    for(jj = 0 ; jj < hashMode; jj++)
    {
        portsToBeDesignatedArr[jj] = 0xFF;
    }

    /* fill in index of %8/64 of the 'source ports' with value of a trunk member */
    for(jj = 0 ; jj < numOfPorts; jj++)
    {
        if(portsToBeDesignatedArr[portsArr[jj].srcPort % hashMode] != 0xFF &&
           portsToBeDesignatedArr[portsArr[jj].srcPort % hashMode] != portsArr[jj].trunkPort)
        {
            return GT_BAD_PARAM;
        }

        /* actual balance done according to 'global port' and not 'local port'*/
        portsToBeDesignatedArr[portsArr[jj].srcPort % hashMode] =
            portsArr[jj].trunkPort;
    }

    /* now fill all the 'not yet set' entries */
    ii = 0;
    for(jj = 0 ; jj < hashMode; jj++)
    {
        if(portsToBeDesignatedArr[jj] == 0xFF)
        {
            portsToBeDesignatedArr[jj] = portsArr[ii++].trunkPort;

            if(ii == numOfPorts)
            {
                /* rewind the index in portsArr[] */
                ii = 0;
            }
        }
    }

    /* now update the actual HW table */
    for(jj = 0 ; jj < numOfDesignatedTrunkEntriesHw; jj++)
    {
        rc = cpssDxChTrunkDesignatedPortsEntryGet(devNum, jj, &designatedPorts);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* clear the ports set by CPSS */
        for(ii = 0 ; ii < numOfPorts ; ii++)
        {
            CPSS_PORTS_BMP_PORT_CLEAR_MAC((&designatedPorts),portsArr[ii].trunkPort);
        }

        /* set proper designated port */
        CPSS_PORTS_BMP_PORT_SET_MAC((&designatedPorts),portsToBeDesignatedArr[jj % hashMode]);


        rc = cpssDxChTrunkDesignatedPortsEntrySet(devNum, jj, &designatedPorts);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTrunkMcLocalSwitchingEnableSet
*
*       Function Relevant mode : High Level mode
*
* DESCRIPTION:
*       Enable/Disable sending multi-destination packets back to its source
*       trunk on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       enable      - Boolean value:
*                     GT_TRUE  - multi-destination packets may be sent back to
*                                their source trunk on the local device.
*                     GT_FALSE - multi-destination packets are not sent back to
*                                their source trunk on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or bad trunkId number
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. the behavior of multi-destination traffic ingress from trunk is
*       not-affected by setting of cpssDxChBrgVlanLocalSwitchingEnableSet
*       and not-affected by setting of cpssDxChBrgPortEgressMcastLocalEnable
*       2. the functionality manipulates the 'non-trunk' table entry of the trunkId
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkMcLocalSwitchingEnableSet
(
    IN GT_U8            devNum,
    IN GT_TRUNK_ID      trunkId,
    IN GT_BOOL          enable
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* call the generic API */
    return prvCpssGenericTrunkMcLocalSwitchingEnableSet(devNum,trunkId,enable);
}

/*******************************************************************************
* cpssDxChTrunkDbMcLocalSwitchingEnableGet
*
*       Function Relevant mode : High Level mode
*
* DESCRIPTION:
*       Get the current status of Enable/Disable sending multi-destination packets
*       back to its source trunk on the local device.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*
* OUTPUTS:
*       enablePtr   - (pointer to) Boolean value:
*                     GT_TRUE  - multi-destination packets may be sent back to
*                                their source trunk on the local device.
*                     GT_FALSE - multi-destination packets are not sent back to
*                                their source trunk on the local device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_PARAM             - bad device number , or bad trunkId number
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDbMcLocalSwitchingEnableGet
(
    IN GT_U8            devNum,
    IN GT_TRUNK_ID      trunkId,
    OUT GT_BOOL          *enablePtr
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* call the generic API */
    return prvCpssGenericTrunkDbMcLocalSwitchingEnableGet(devNum,trunkId,enablePtr);
}


