/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxDxChBrgFdb.c
*
* DESCRIPTION:
*       FDB tables facility CPSS DxCh implementation.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
/* get the specific signaling for trigger action and AU message from CPU ended*/
#include <cpss/generic/bridge/private/prvCpssGenBrgFdbIsrSig.h>
/* get vid checking */
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
/* get Cheetah FDB Hash function */
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgFdbHash.h>

#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>

#include <cpss/generic/bridge/cpssGenBrgFdb.h>

/*#define CPSS_DXCH_FDB_DUMP*/

#ifdef CPSS_DXCH_FDB_DUMP
    #define debugPrint(_x)      cpssOsPrintf _x
#else
    #define debugPrint(_x)
#endif

/*
number of messages queues :
1. primary AU queue
2. (primary)FU queue
3. secondary AU queue
*/
#define NUM_OF_MESSAGES_QUEUES_CNS   3

/* by default do not allow the WA to start when AUQ is not full */
static GT_BOOL    prvCpssAuqDeadlockOnQueueNotFullWaEnable = GT_FALSE;

/*******************************************************************************
 * External usage variables
 ******************************************************************************/
#define PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index)                 \
    if(index >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb)   \
        return GT_OUT_OF_RANGE

/* Converts packet command to hardware value */
#define PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(_val, _cmd)      \
    switch (_cmd)                                                       \
    {                                                                   \
        case CPSS_MAC_TABLE_FRWRD_E:                                    \
            _val = 0;                                                   \
            break;                                                      \
        case CPSS_MAC_TABLE_MIRROR_TO_CPU_E:                            \
            _val = 1;                                                   \
            break;                                                      \
        case CPSS_MAC_TABLE_CNTL_E:                                     \
            _val = 2;                                                   \
            break;                                                      \
        case CPSS_MAC_TABLE_DROP_E:                                     \
            _val = 3;                                                   \
            break;                                                      \
        case CPSS_MAC_TABLE_SOFT_DROP_E:                                \
            _val = 4;                                                   \
            break;                                                      \
        default:                                                        \
            return GT_BAD_PARAM;                                        \
    }

/* Converts  hardware value to packet command */
#define PRV_CPSS_DXCH_FDB_CONVERT_HW_CMD_TO_SW_VAL_MAC(_cmd, _val)      \
    switch (_val)                                                       \
    {                                                                   \
        case 0:                                                         \
            _cmd = CPSS_MAC_TABLE_FRWRD_E;                              \
            break;                                                      \
        case 1:                                                         \
            _cmd = CPSS_MAC_TABLE_MIRROR_TO_CPU_E;                      \
            break;                                                      \
        case 2:                                                         \
            _cmd = CPSS_MAC_TABLE_CNTL_E;                               \
            break;                                                      \
        case 3:                                                         \
            _cmd = CPSS_MAC_TABLE_DROP_E;                               \
            break;                                                      \
        case 4:                                                         \
            _cmd = CPSS_MAC_TABLE_SOFT_DROP_E;                          \
            break;                                                      \
        default:                                                        \
            _cmd = (_val);                                              \
            break;                                                      \
    }

/* number of words in the mac entry */
#define CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS   4
/* number of bytes in the mac entry */
#define CPSS_DXCH_FDB_MAC_ENTRY_BYTES_SIZE_CNS   16

/* number of words in the Au (address update) message */
#define CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS   4
/* number of bytes in the Au (address update) message */
#define CPSS_DXCH_FDB_AU_BYTES_SIZE_CNS   16

/*******************************************************************************
* prvDxChFdbToAuConvert
*
* DESCRIPTION:
*       convert the FDB format of CPSS to DxCh Au (address update message)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to key data of the mac entry
*       macEntryPtr     - MAC format of entry
*       updMessageType  - Au message type
*       deleteEntry     - GT_TRUE - the entry should be deleted
*                         GT_FALSE - update entry
*
* OUTPUTS:
*       hwDataArr       - (pointer to) AU message format
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - on bad parameter error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvDxChFdbToAuConvert
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC  *macEntryKeyPtr,
    IN  CPSS_MAC_ENTRY_EXT_STC      *macEntryPtr,
    IN  CPSS_UPD_MSG_TYPE_ENT       updMessageType,
    IN  GT_BOOL                     deleteEntry,
    OUT GT_U32                      hwDataArr[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]
)
{
    GT_U32      hwValue;             /* hardware value of the field      */
    GT_U32      hwUpdMessageType;    /* hw value of Update message type */
    GT_U32      bit40MacAddrOrIsIpm; /* bit 40 from MAC Address for MAC entry
                                        or entry type is IPM          */
    GT_U32      portTrunk;            /* port number or trunk Id         */
    GT_U32      isTrunk;              /* trunk flag                      */
    GT_U32      devTmp;               /* entry device number             */
    GT_U32      multiple;             /* if "1", forward packet to all
                                         ports in VIDX/VLAN */
    GT_U16      vidx = 0;
    GT_U16      vlanId;               /* vlan ID */
    GT_U32      entryType;            /* the only type supported
                                       0 -- mac entry
                                       1 -- ipmcV4 entry
                                       2 -- ipmcV6 entry */
    GT_U32  macLow16;                /* two low bytes of MAC Address */
    GT_U32  macHi32;                 /* four high bytes of MAC Address */
    GT_U8   sip[4];                  /* Source IP */
    GT_U8   dip[4];                  /* Destination IP */

    /* reset all words in hwDataArr */
    cpssOsMemSet((char *) &hwDataArr[0], 0, CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS * sizeof(GT_U32));

    /* Set Word0 */

    /* set update address message type */
    switch (updMessageType)
    {
        case CPSS_NA_E:
            hwUpdMessageType = 0;
            break;
        case CPSS_QA_E:
            hwUpdMessageType = 1;
            break;
        default:
            /* not supported yet */
            return GT_FAIL;
    }

    /* check entry type */
    switch (macEntryKeyPtr->entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
            macLow16 = GT_HW_MAC_LOW16(&(macEntryKeyPtr->key.macVlan.macAddr));
            macHi32 = GT_HW_MAC_HIGH32(&(macEntryKeyPtr->key.macVlan.macAddr));

            /* get bit 40 from MAC Address */
            bit40MacAddrOrIsIpm = (macHi32 >> 24) & 0x1;

            vlanId = macEntryKeyPtr->key.macVlan.vlanId;
            entryType = 0;
            break;
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
            cpssOsMemCpy(dip, macEntryKeyPtr->key.ipMcast.dip, 4);
            cpssOsMemCpy(sip, macEntryKeyPtr->key.ipMcast.sip, 4);
            vlanId = macEntryKeyPtr->key.ipMcast.vlanId;

            if (macEntryKeyPtr->entryType ==
                CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E)
                entryType = 1;
            else
                entryType = 2;

            /* set flag that entry is IPM */
            bit40MacAddrOrIsIpm = 1;

            /* set variables to avoid compilation warnings */
            macLow16 = 0;
            macHi32 = 0;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if (vlanId >= BIT_12)
        return GT_OUT_OF_RANGE;

    /* set message ID field for Cheetah only */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        hwDataArr[0] = 0x2;

    /* message type */
    hwDataArr[0] |= (hwUpdMessageType << 4);

    if (entryType == 0)
    {
        /* set MAC Address */

        /* MacAddr[15..0] */
        hwDataArr[0] |= (macLow16 << 16);  /* Word0 */
        /* MacAddr[47..16] */
        hwDataArr[1] = macHi32;            /* Word1 */

    }
    else
    {
        /* set IP Addresses */

        /* DIP[15..0] */
        hwDataArr[0] |= ((dip[3] | (dip[2] << 8)) << 16);  /* Word0, bits 16-31 */
        /* DIP[16..31] */
        hwDataArr[1] |= (dip[1] | (dip[0] << 8));          /* Word1, bits 0-15 */
        /* SIP[15..0] */
        hwDataArr[1] |= ((sip[3] | (sip[2] << 8)) << 16);  /* Word1, bits 16-31 */
        /* SIP[27..16] */
        hwDataArr[3] |= (sip[1] | ((sip[0] & 0xF) << 8));  /* Word3, bits 0-11 */
        /* SIP[31..28] */
        hwDataArr[3] |= ((sip[0] >> 4) << 27);             /* Word3, bits 27-30 */
    }

    /* VID */
    hwDataArr[2] = vlanId;

    /* Entry Type */
    hwDataArr[3] |= (entryType << 19);


    if(deleteEntry == GT_TRUE)
    {
        /* skip bit */
        hwDataArr[2] |= (1 << 12);
        return GT_OK;
    }


    if(updMessageType != CPSS_QA_E)
    {
        /* check interface destination type */
        switch(macEntryPtr->dstInterface.type)
        {
            /* check that the type of the entry is MAC */
            case CPSS_INTERFACE_PORT_E:
                /* check that type of the entry is MAC and address
                   is not multicast */
                if (bit40MacAddrOrIsIpm == 0)
                {
                    portTrunk = macEntryPtr->dstInterface.devPort.portNum;
                    isTrunk = 0;
                    devTmp = macEntryPtr->dstInterface.devPort.devNum;
                    multiple = 0;
                }
                else
                {
                    /* For multicast MAC or IPM entries the VIDX or VLAN
                       targed must be used but not port */
                    return GT_BAD_PARAM;
                }
                break;

            case CPSS_INTERFACE_TRUNK_E:
                /* check that type of the entry is MAC and address
                   is not multicast */
                if (bit40MacAddrOrIsIpm == 0)
                {
                    portTrunk = macEntryPtr->dstInterface.trunkId;
                    if (portTrunk == 0)
                    {
                        /* trunk ID should be > 0 */
                        return GT_BAD_PARAM;
                    }
                    isTrunk = 1;
                    devTmp = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
                    multiple = 0;
                }
                else
                {
                    /* For multicast MAC or IPM entries the VIDX or VLAN
                       targed must be used but not trunk*/
                    return GT_BAD_PARAM;
                }

                break;

            case CPSS_INTERFACE_VIDX_E:
            case CPSS_INTERFACE_VID_E:
                devTmp = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
                if(macEntryPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
                {
                    vidx = 0xFFF;
                }
                else /*CPSS_INTERFACE_VIDX_E*/
                {
                    vidx = macEntryPtr->dstInterface.vidx;
                    /* check range */
                    if (vidx >= BIT_12)
                        return GT_OUT_OF_RANGE;
                }
                multiple = 1;

                /* set variables to avoid compilation warnings */
                isTrunk = 0;
                portTrunk = 0;
                break;
            default:
                return GT_BAD_PARAM;
        }

        /* check ranges */
        if ((devTmp >= BIT_5) ||
            (macEntryPtr->userDefined >= BIT_4) ||
            (macEntryPtr->saQosIndex >= BIT_3) ||
            (macEntryPtr->daQosIndex >= BIT_3) ||
            (portTrunk >= BIT_7))
        {
            return GT_OUT_OF_RANGE;
        }

        /* Multiple */
        hwDataArr[2] |= (multiple << 15);

        /* check entry type */
        if (entryType == 0)
        {
            /* MAC entry */

            if (multiple == 1)
            {
                /* VIDX */
                hwDataArr[2] |= (vidx << 17);
            }
            else
            {/* muliple=0 and bit40MacAddr=0 */

                /* is Trunk bit */
                hwDataArr[2] |= (isTrunk << 17);

                /* PortNum/TrunkNum */
                hwDataArr[2] |= (portTrunk << 18);

                /* UserDefined */
                hwDataArr[2] |= (macEntryPtr->userDefined << 25);
            }

            /* Src ID */
            /* check range */
            if (macEntryPtr->sourceID >= BIT_5)
            {
                return GT_OUT_OF_RANGE;
            }
            hwDataArr[3] |= (macEntryPtr->sourceID << 2);

            /* DevNum */
            hwDataArr[3] |= (devTmp << 7);

            /* SA cmd */
            PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(hwValue,
                                                           macEntryPtr->saCommand);
            hwDataArr[3] |= (hwValue << 24);
        }

        else
        {/* IP entry */

            /* VIDX */
            hwDataArr[2] |= (vidx << 17);
        }

        /* age bit */
        hwDataArr[2] |= (BOOL2BIT_MAC(macEntryPtr->age)  << 13);

        /* spUnknown - NA strorm prevent entry */
        hwDataArr[2] |= (BOOL2BIT_MAC(macEntryPtr->spUnknown) << 14);

        /* DA Route */
        hwDataArr[2] |= ((BOOL2BIT_MAC(macEntryPtr->daRoute)) << 30);

        /* SA QoS Profile Index */
        hwDataArr[3] |= (macEntryPtr->saQosIndex << 12);

        /* Da QoS Profile Index */
        hwDataArr[3] |= (macEntryPtr->daQosIndex << 15);

        /* Static */
        hwDataArr[3] |= ((BOOL2BIT_MAC(macEntryPtr->isStatic)) << 18);

        /* DA cmd */
        PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(hwValue,
                                                       macEntryPtr->daCommand);
        hwDataArr[3] |= (hwValue << 21);

        /* Mirror to Analyzer Port */
        hwValue = BOOL2BIT_MAC(macEntryPtr->mirrorToRxAnalyzerPortEn);
        hwDataArr[3] |= (hwValue << 31);

        /* set Cheetah2 fields */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* check ranges */
            if ((macEntryPtr->daSecurityLevel >= BIT_3) ||
                (macEntryPtr->saSecurityLevel >= BIT_3))
            {
                return GT_OUT_OF_RANGE;
            }

            /* MAC DA Access Level */
            hwDataArr[0] |= (macEntryPtr->daSecurityLevel << 1);
            /* MAC SA Access Level */
            hwDataArr[0] |= (macEntryPtr->saSecurityLevel << 12);
            /* Application Specific CPU Code */
            hwDataArr[2] |= ((BOOL2BIT_MAC(macEntryPtr->appSpecificCpuCode)) << 29);
        }


    }
    return GT_OK;
}

/*******************************************************************************
* prvDxChBrgFdbBuildMacEntryHwFormat
*
* DESCRIPTION:
*       convert the FDB format of CPSS to DxCh Mac hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       macEntryPtr - pointer to extended MAC entry format
*       skip        - skip bit
*
* OUTPUTS:
*       hwDataArr   - (pointer to) hw mac entry format
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - on bad parameter error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvDxChBrgFdbBuildMacEntryHwFormat(
    IN  GT_U8                   devNum,
    IN  CPSS_MAC_ENTRY_EXT_STC  *macEntryPtr,
    IN  GT_BOOL                 skip,
    OUT GT_U32                  hwDataArr[CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS]
)
{

    GT_U32      hwValue;             /* hardware value of the field      */
    GT_U32      bit40MacAddrOrIsIpm; /* bit 40 from MAC Address for MAC entry
                                        or entry type is IPM          */
    GT_U32      portTrunk;      /* port number or trunk Id         */
    GT_U32      isTrunk;        /* trunk flag                      */
    GT_U32      devTmp;         /* entry device number             */
    GT_U32      multiple;       /* if "1", forward packet to all
                                   ports in VIDX/VLAN */
    GT_U16      vidx = 0;
    GT_U16      vlanId;          /* vlan ID */
    GT_U32      entryType = 0;  /* the only type supported
                                   0 -- mac entry
                                   1 -- ipmcV4 entry
                                   2 -- ipmcV6 entry */
    GT_U32  macLow16;            /* two low bytes of MAC Address */
    GT_U32  macHi32;             /* four high bytes of MAC Address */
    GT_U8   sip[4];              /* Source IP */
    GT_U8   dip[4];              /* Destination IP */
    GT_U32  saCmd;               /* command when the entry matches source
                                    address lookup for MAC SA entry */

    /* reset all words in hwDataArr */
    cpssOsMemSet((char *) &hwDataArr[0], 0,
             CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS * sizeof(GT_U32));

    /* check entry type */
    switch (macEntryPtr->key.entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
            macLow16 = GT_HW_MAC_LOW16(&(macEntryPtr->key.key.macVlan.macAddr));
            macHi32 = GT_HW_MAC_HIGH32(&(macEntryPtr->key.key.macVlan.macAddr));

            /* get bit 40 from MAC Address */
            bit40MacAddrOrIsIpm = (macHi32 >> 24) & 0x1;

            vlanId = macEntryPtr->key.key.macVlan.vlanId;
            entryType = 0;
            break;
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
            cpssOsMemCpy(dip, macEntryPtr->key.key.ipMcast.dip, 4);
            cpssOsMemCpy(sip, macEntryPtr->key.key.ipMcast.sip, 4);
            vlanId = macEntryPtr->key.key.ipMcast.vlanId;

            if (macEntryPtr->key.entryType ==
                CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E)
                entryType = 1;
            else
                entryType = 2;

            /* set flag that entry is IPM*/
            bit40MacAddrOrIsIpm = 1;

            /* set variables to avoid compilation warnings */
            macLow16 = 0;
            macHi32 = 0;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* check interface destination type */
    switch(macEntryPtr->dstInterface.type)
    {
        case CPSS_INTERFACE_PORT_E:
            /* check that type of the entry is MAC and address
               is not multicast */
            if (bit40MacAddrOrIsIpm == 0)
            {
                portTrunk = macEntryPtr->dstInterface.devPort.portNum;
                isTrunk = 0;
                devTmp = macEntryPtr->dstInterface.devPort.devNum;
                multiple = 0;
            }
            else
            {
                /* For multicast MAC or IPM entries the VIDX or VLAN
                   targed must be used but not port */
                return GT_BAD_PARAM;
            }
            break;

        case CPSS_INTERFACE_TRUNK_E:
            /* check that type of the entry is MAC and address
               is not multicast */
            if (bit40MacAddrOrIsIpm == 0)
            {
                portTrunk = macEntryPtr->dstInterface.trunkId;
                if (portTrunk == 0)
                {
                    return GT_BAD_PARAM;
                }
                isTrunk = 1;
                devTmp = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
                multiple = 0;
            }
            else
            {
                /* For multicast MAC or IPM entries the VIDX or VLAN
                   targed must be used but not trunk */
                return GT_BAD_PARAM;
            }
            break;

        case CPSS_INTERFACE_VIDX_E:
        case CPSS_INTERFACE_VID_E:
            devTmp = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
            if(macEntryPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
            {
                vidx = 0xFFF;
            }
            else /*CPSS_INTERFACE_VIDX_E*/
            {
                vidx = macEntryPtr->dstInterface.vidx;
                /* check range */
                if (vidx >= BIT_12)
                    return GT_OUT_OF_RANGE;
            }
            multiple = 1;

            /* set variables to avoid compilation warnings */
            portTrunk = 0;
            isTrunk = 0;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* check ranges */
    if ((vlanId >= BIT_12) ||
        (devTmp >= BIT_5) ||
        (macEntryPtr->userDefined >= BIT_4) ||
        (macEntryPtr->saQosIndex >= BIT_3) ||
        (macEntryPtr->daQosIndex >= BIT_3) ||
        (portTrunk >= BIT_7))
    {
        return GT_OUT_OF_RANGE;
    }

    /* valid bit */
    hwDataArr[0] = 0x1;

    /* skip */
    hwDataArr[0] |= (BOOL2BIT_MAC(skip) << 1);

    /* age */
    hwDataArr[0] |= (BOOL2BIT_MAC(macEntryPtr->age) << 2);

    /* entry type */
    hwDataArr[0] |= (entryType << 3);

    /* VID */
    hwDataArr[0] |= (vlanId << 5);


    if (entryType == 0)
    {
        /* MAC entry */

        /* MacAddr[14..0] */
        hwDataArr[0] |= ((macLow16 & 0x7FFF) << 17); /* Word0, bits 17-31 */

        /* MacAddr[15..46] */
        hwDataArr[1] = (macLow16 >> 15) & 0x1; /* highest bit from mac second byte */
        hwDataArr[1] |= (macHi32 << 1);

        /* MacAddr[47] */
        hwDataArr[2] |= ((macHi32 >> 31) & 0x1); /* the highest bit fro Mac Address */

        /* devNum ID */
        hwDataArr[2] |= (devTmp << 1);

        /* Src ID */
        /* check range */
        if (macEntryPtr->sourceID >= BIT_5)
        {
            return GT_OUT_OF_RANGE;
        }
        hwDataArr[2] |= (macEntryPtr->sourceID << 6);

        /* check if bit40MacAddr==0 or entry is VIDX or VID */
        if (multiple == 1)
        {
            /* VIDX */
            hwDataArr[2] |= (vidx << 13);
        }
        else
        {/* bit40MacAddr=0 and entry is PORT or TRUNK */

            /* is Trunk bit */
            hwDataArr[2] |= (isTrunk << 13);

            /* PortNum/TrunkNum */
            hwDataArr[2] |= (portTrunk << 14);

            /* UserDefined */
            hwDataArr[2] |= (macEntryPtr->userDefined << 21);
        }

        /* multiple */
        hwDataArr[2] |= (multiple << 26);

        /* SA cmd[1..0] */
        PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(saCmd,
                                                       macEntryPtr->saCommand);
        hwDataArr[2] |= ((saCmd & 0x3) << 30);

        /* SA cmd[2] */
        hwDataArr[3] |= ((saCmd >> 2) & 0x1);
    }

    else
    {
        /* IPM entry */

        /* DIP[14..0] */
        /* Word0, bits 17-31 */
        hwDataArr[0] |= ((dip[3] | ((dip[2] & 0x7F) << 8)) << 17);

        /* DIP[31..15] */
        /* Word1, bits 0-16 */
        hwDataArr[1] = ((dip[2] >> 7) | (dip[1] << 1) | (dip[0] << 9));

        /* SIP[14..0] */
        /* Word1, bits 17-31 */
        hwDataArr[1] |= ((sip[3] | ((sip[2] & 0x7F) << 8)) << 17);

        /* SIP[27..15] */
        /* Word2, bits 0-12 */
        hwDataArr[2] |= ((sip[2] >> 7) | (sip[1] << 1) | ((sip[0] & 0xF) << 9));

        /* VIDX */
        hwDataArr[2] |= (vidx << 13);

        /* SIP[28] */
        /* Word2, bit 26 */
        hwDataArr[2] |= (((sip[0] >> 4) & 0x1) << 26);

        /* SIP[30..29] */
        /* Word2, bits 30-31 */
        hwDataArr[2] |= (((sip[0] >> 5) & 0x3) << 30);

        /* SIP[31] */
        /* Word0, bit 31 */
        hwDataArr[3] |= ((sip[0] >> 7) & 0x1);
    }

    /* static */
    hwDataArr[2] |= (BIT2BOOL_MAC(macEntryPtr->isStatic) << 25);

    /* DA cmd */
    PRV_CPSS_DXCH_FDB_CONVERT_SW_CMD_TO_HW_VAL_MAC(hwValue,
                                                   macEntryPtr->daCommand);
    hwDataArr[2] |= (hwValue << 27);

    /* DA Route */
    hwDataArr[3] |= (BOOL2BIT_MAC(macEntryPtr->daRoute) << 1);

    /* spUnknown - NA strorm prevent entry */
    hwDataArr[3] |= (BOOL2BIT_MAC(macEntryPtr->spUnknown) << 2);

    /* SaQosProfileIndex */
    hwDataArr[3] |= (macEntryPtr->saQosIndex << 3);

    /* DaQosProfileIndex */
    hwDataArr[3] |= (macEntryPtr->daQosIndex << 6);

    /* MirrorToAnalyzerPort*/
    hwDataArr[3] |= ((BOOL2BIT_MAC(macEntryPtr->mirrorToRxAnalyzerPortEn)) << 9);

    /* set Cheetah2 only fields */
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
    {
        if ((macEntryPtr->daSecurityLevel >= BIT_3) ||
            (macEntryPtr->saSecurityLevel >= BIT_3))
        {
            return GT_OUT_OF_RANGE;
        }

        /* AppSpecific CPU Code */
        hwDataArr[3] |= ((BOOL2BIT_MAC(macEntryPtr->appSpecificCpuCode)) << 10);

        /* DA Access Level */
        hwDataArr[3] |= (macEntryPtr->daSecurityLevel << 11);

        /* SA Access Level */
        hwDataArr[3] |= (macEntryPtr->saSecurityLevel << 14);
    }

    return GT_OK;
}


/*******************************************************************************
* prvDxChFdbSendAuMessage
*
* DESCRIPTION:
*       function send Au message to PP
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portGroupId - the portGroupId. to support multi-port-groups device
*       hwDataArr   - (pointer to) AU message format
*                            data to write to register :
*                            hwDataArr[0] -- FDB entry word 0
*                            hwDataArr[1] -- FDB entry word 1
*                            hwDataArr[2] -- FDB entry word 2
*                            hwDataArr[3] -- FDB entry word 3
*                            hwDataArr[4] -- control register
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error  ---
*            A fail can return due to any of the following:
*             1. The message type is NA and the hash-chain has reached its
*                maximum length
*             2. The message type is AA and the FDB entry does not exist
*             3. The message type is QA and the FDB entry does not exist
*
*
*       GT_HW_ERROR - on hardware error
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvDxChFdbSendAuMessage(
    IN  GT_U8           devNum,
    IN  GT_U32          portGroupId,
    IN  GT_U32          hwDataArr[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]
)
{
    GT_STATUS   rc;                     /* return error code */
    GT_U32      addrUpdateControlReg;   /* address of the control reg */
    GT_U32      addrUpdateReg;          /* hw memory address to write */

    /* non-direct table --- this is the address of the control reg */
    addrUpdateControlReg = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        globalRegs.addrUpdateControlReg ;
    /* non-direct table --- this is the address of the data reg */
    addrUpdateReg = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.addrUpdate[0];

    /* check that specific/all port groups are ready */
    rc = prvCpssDxChPortGroupBusyWait(devNum,portGroupId,
                addrUpdateControlReg,0,
                GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if (rc != GT_OK)
    {
        return rc;
    }

    /* non-direct table --- write to it as continues memory
       better performance then 4 registers in a roll  */
    rc = prvCpssDrvHwPpPortGroupWriteRam(devNum,portGroupId,
                                addrUpdateReg,
                                CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS ,/* 4 words */
                                &hwDataArr[0]);

    if(rc != GT_OK)
    {
        return rc;
    }

    /* Start FDB messaging */
    rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId, addrUpdateControlReg, 1);
    return rc;
}

/*******************************************************************************
* prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet
*
* DESCRIPTION:
*       Set the field mask for 'MAC entry does NOT reside on the local port group'
*       When Enabled - AU/FU messages are not sent to the CPU if the MAC entry
*                      does NOT reside on the local core, i.e.
*                      the entry port[5:4] != LocalPortGroup
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*                      --ONLY for multi-Port Group device
*
* INPUTS:
*       devNum      - device number
*       enable      - mask enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*    GT_OK           - on success
*    GT_HW_ERROR     - on HW error
*    GT_BAD_PARAM    - on bad device number.
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*       none
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;    /* The register's address of l2IngressCtrl reg. */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);

    /* save the state to DB */
    PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E.
            filterEnabled = enable;

    if(PRV_CPSS_FDB_AUQ_FROM_NON_SOURCE_PORT_GROUP_WA_E(devNum))
    {
        /* when application asked to do the filtering in the SW (and not in HW) */
        /* don't change HW mode (HW default is mask 'disabled') */

        return GT_OK;
    }

    if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
                  PRV_CPSS_DXCH_LION_PREVENT_REDUNDANT_AU_FU_MESSAGES_NOT_SUPPORTED_WA_E))
    {
        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 26, 1, BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableGet
*
* DESCRIPTION:
*       Get the field mask for 'MAC entry does NOT reside on the local port group'
*       When Enabled - AU/FU messages are not sent to the CPU if the MAC entry
*                      does NOT reside on the local core, i.e.
*                      the entry port[5:4] != LocalPortGroup
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*                      --ONLY for multi-Port Group device
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) mask enable/disable
*
* RETURNS:
*    GT_OK           - on success
*    GT_HW_ERROR     - on HW error
*    GT_BAD_PARAM    - on bad device number.
*    GT_BAD_PTR      - on NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*       none
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_STATUS   rc ;        /* return code */
    GT_U32      regAddr; /* Register address */
    GT_U32      hwValue; /* HW value of register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT2_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_FDB_AUQ_FROM_NON_SOURCE_PORT_GROUP_WA_E(devNum))
    {
        /* when application asked to do the filtering in the SW (and not in HW) */
        /* don't change HW mode (HW default is mask 'disabled') */

        /* get info from the DB */
        *enablePtr = PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
            info_PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E.
                filterEnabled;

        return GT_OK;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 26, 1, &hwValue);

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}


/*******************************************************************************
* multiPortGroupCheckAndMessageFilterResolve
*
* DESCRIPTION:
*       1. check that the action mode supported for by the FDB mode.
*       2. resolve the state of the AUQ/FUQ due to 'non source port group'
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*                      --ONLY for multi-Port Group device
*
* INPUTS:
*       devNum         - device number
*       daRefreshGiven - is the 'daRefresh' parameter supplied by called
*       daRefresh      - da refresh value
*       fdbUploadGiven - is the 'fdbUpload' parameter supplied by called
*       fdbUpload      - FDB upload value
*       actionModeGiven - is the 'actionMode' parameter supplied by called
*       actionMode      - action mode value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*    GT_OK           - on success
*    GT_HW_ERROR     - on HW error
*    GT_BAD_PARAM    - on bad device number.
*    GT_BAD_STATE    - the setting not allowed in current state of configuration.
*
* COMMENTS:
*       none
*
*******************************************************************************/
static GT_STATUS multiPortGroupCheckAndMessageFilterResolve
(
    IN GT_U8    devNum,
    IN GT_BOOL  daRefreshGiven,
    IN GT_BOOL  daRefresh,
    IN GT_BOOL  fdbUploadGiven,
    IN GT_BOOL  fdbUpload,
    IN GT_BOOL  actionModeGiven,
    IN CPSS_FDB_ACTION_MODE_ENT  actionMode
)
{
    GT_STATUS   rc;     /* return code*/
    GT_BOOL     filterEnable = GT_FALSE;/* filter enable/disable*/
    GT_BOOL     modifyFilter = GT_FALSE;/* does filter need modification */
    GT_BOOL     needToUseFdbUpload = GT_FALSE;/* do we need FdbUpload ? */
    GT_BOOL     needToUseDaRefresh = GT_FALSE;/* do we need DaRefresh ? */

    if(actionModeGiven == GT_FALSE)
    {
        /* read from HW the current state */
        rc = cpssDxChBrgFdbActionModeGet(devNum, &actionMode);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    switch (actionMode)
    {
        case CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E:
            switch(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode)
            {
                case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E:
                case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
                    /*
                    This type of operation is not supported , due to the control
                    learning being used, the risk of AA messages being dropped & the
                    need to avoid dropping MAC entries from portGroups which are not
                    the source of these entries (meaning they would never be refreshed)
                    */
                    return GT_BAD_STATE;
                default:
                    break;
            }

            needToUseDaRefresh = GT_TRUE;
            break;
        case CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E:
            needToUseFdbUpload = GT_TRUE;
            needToUseDaRefresh = GT_TRUE;
            break;
        case CPSS_FDB_ACTION_TRANSPLANTING_E:
            switch(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode)
            {
                case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
                case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E:
                    /* not supported.
                      because triggering this would possibly result with entries which
                      have their new 'source' information properly set - but these entries
                      would not be relocated to the new source portGroups, meaning that
                      packet received from new station location (which was supposed to
                      be transplanted), may still create NA message.
                      Computing new source portGroup is not always possible, especially
                      when station is relocated to trunk or it was previously residing
                      on multiple-portGroups.
                      Application is required to perform Flush operation instead.
                      This would clear previously learnt MAC
                    */
                    return GT_BAD_STATE;
                default:
                    break;
            }
            break;
        default:
            break;
    }/*switch (actionMode)*/

    if(needToUseFdbUpload == GT_TRUE && fdbUploadGiven == GT_FALSE)
    {
        /* read from HW the current state */
        rc = cpssDxChBrgFdbUploadEnableGet(devNum, &fdbUpload);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if(needToUseDaRefresh == GT_TRUE && daRefreshGiven == GT_FALSE)
    {
        /* read from HW the current state */
        rc = cpssDxChBrgFdbAgeBitDaRefreshEnableGet(devNum, &daRefresh);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /********************************************/
    /* resolve the state of the messages filter */
    /********************************************/


    switch (actionMode)
    {
        case CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E:
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode ==
               PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E)
            {
                modifyFilter = GT_TRUE;
                filterEnable = daRefresh == GT_TRUE ? GT_FALSE : GT_TRUE;
            }
            break;
        case CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E:
            modifyFilter = GT_TRUE;
            if(fdbUpload == GT_TRUE)
            {
                filterEnable = GT_TRUE;
            }
            else
            {
                filterEnable = daRefresh == GT_TRUE ? GT_FALSE : GT_TRUE;
            }
            break;
        case CPSS_FDB_ACTION_DELETING_E:
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode ==
                PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E)
            {
                modifyFilter = GT_TRUE;
                filterEnable = GT_TRUE;
            }
            break;
        case CPSS_FDB_ACTION_TRANSPLANTING_E:
            if(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode ==
                PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E)
            {
                modifyFilter = GT_TRUE;
                filterEnable = GT_TRUE;
            }
        default:
            break;
    }/*switch (actionMode)*/

    if(modifyFilter == GT_TRUE)
    {
        rc = prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet(devNum,filterEnable);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbPortLearnStatusSet
*
* DESCRIPTION:
*       Enable/disable learning of new source MAC addresses for packets received
*       on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       port   - port number
*       status - GT_TRUE for enable  or GT_FALSE otherwise
*       cmd    - how to forward packets with unknown/changed SA,
*                 if status is GT_FALSE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on unknown command
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*       1. Unified FDBs mode and Unified-Linked FDBs mode :
*           Automatic learning is not supported in this mode, due to the fact
*           that FDBs are supposed to be synced.
*           API returns GT_BAD_STATE if trying to enable auto learning
*           in this mode
*       2. Linked FDBs mode - no limitations
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortLearnStatusSet
(
    IN GT_U8                    devNum,
    IN GT_U8                    port,
    IN GT_BOOL                  status,
    IN CPSS_PORT_LOCK_CMD_ENT   cmd
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    /* invert value,
       1 - Automatic learning is disabled.
       0 - Automatic learning is enable. */
    value = (status == GT_TRUE) ? 0 : 1;

    if (status == GT_FALSE)
    {
        /* set command */
        switch (cmd)
        {
            case CPSS_LOCK_FRWRD_E:
                break;
            case CPSS_LOCK_DROP_E:
                value |= (3 << 1);
                break;
            case CPSS_LOCK_TRAP_E:
                value |= (2 << 1);
                break;
            case CPSS_LOCK_MIRROR_E:
                 value |= (1 << 1);
                break;
            case CPSS_LOCK_SOFT_DROP_E:
                 value |= (4 << 1);
                break;
            default:
                return GT_BAD_PARAM;
        }
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];
	/*modified the value of regAddr,or the value could not set in group0 and group3*/
    if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        regAddr = regAddr - (PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port))*0x10000;
    }

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
        regAddr, 15, 4, value);
}

/*******************************************************************************
* cpssDxChBrgFdbPortLearnStatusGet
*
* DESCRIPTION:
*       Get state of new source MAC addresses learning on packets received
*       on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       port   - port number
*
* OUTPUTS:
*       statusPtr - (pointer to) GT_TRUE for enable  or GT_FALSE otherwise
*       cmdPtr    - (pointer to) how to forward packets with unknown/changed SA,
*                   when (*statusPtr) is GT_FALSE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortLearnStatusGet
(
    IN  GT_U8   devNum,
    IN  GT_U8   port,
    OUT GT_BOOL *statusPtr,
    OUT CPSS_PORT_LOCK_CMD_ENT *cmdPtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write into register */
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(statusPtr);
    CPSS_NULL_PTR_CHECK_MAC(cmdPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
            regAddr, 15, 4, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* invert value,
       1 - Automatic learning is disabled.
       0 - Automatic learning is enable. */
    *statusPtr = (value & 0x01) ? GT_FALSE : GT_TRUE;

    value = value >> 1;

    /* translate hw cmd to sw cmd */
    switch (value)
    {
        case 0:
            *cmdPtr = CPSS_LOCK_FRWRD_E;
            break;
        case 1:
            *cmdPtr = CPSS_LOCK_MIRROR_E;
            break;
        case 2:
            *cmdPtr = CPSS_LOCK_TRAP_E;
            break;
        case 3:
            *cmdPtr = CPSS_LOCK_DROP_E;
            break;
        case 4:
            *cmdPtr = CPSS_LOCK_SOFT_DROP_E;
            break;
        default:
            *cmdPtr = value;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbNaToCpuPerPortSet
*
* DESCRIPTION:
*       Enable/disable forwarding a new mac address message to CPU --per port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       port    - port number, CPU port
*       enable  - If GT_TRUE, forward NA message to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaToCpuPerPortSet
(
    IN GT_U8    devNum,
    IN GT_U8    port,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);

    value = (enable == GT_TRUE) ? 1 : 0;

    if(port == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.cpuPortControl;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                               bridgeRegs.portControl[port];
    }
	/*modified the value of regAddr,or the value could not set in group0 and group3*/
	if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
	{
		regAddr = regAddr - (PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port))*0x10000;
	}
    return  prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
             regAddr,0, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbNaToCpuPerPortGet
*
* DESCRIPTION:
*       Get Enable/disable forwarding a new mac address message to CPU --
*       per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       port    - port number, CPU port
*
* OUTPUTS:
*       enablePtr  - If GT_TRUE, NA message is forwarded to CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaToCpuPerPortGet
(
    IN  GT_U8    devNum,
    IN  GT_U8    port,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      value;      /* value to write into register */
    GT_STATUS   rc;         /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(port == CPSS_CPU_PORT_NUM_CNS)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.cpuPortControl;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                               bridgeRegs.portControl[port];
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
            regAddr, 0, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbNaStormPreventSet
*
* DESCRIPTION:
*   Enable/Disable New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       port       - port number
*       enable     - GT_TRUE - enable NA Storm Prevention (SP),
*                    GT_FALSE - disable NA Storm Prevention
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/portNum/auMsgType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for multi-port groups device :
*       Enabling this mechanism in any of the ports, creates a situation where the
*       different portGroups populate different MACs at the same indexes,
*       causing FDBs to become unsynchronized, this leads to the situation that once
*       sending MAC update to different portGroups from CPU, it may succeed on some
*       of the portGroups, while failing on the others (due to SP entries already
*       occupying indexes)... So the only way to bypass these problems, assuming SP
*       is a must, is to add new MACs by exact index - this mode of operation
*       overwrites any SP entry currently there.
*       Application that enables the SP in any of the ports should not use the NA
*       sending mechanism (cpssDxChBrgFdbMacEntrySet / cpssDxChBrgFdbPortGroupMacEntrySet APIs)
*       to add new MAC addresses, especially to multiple portGroups -
*       as this may fail on some of the portGroups.
*       Removing or updating existing (non-SP) entries may still use the NA messages.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaStormPreventSet
(
    IN GT_U8                       devNum,
    IN GT_U8                       port,
    IN GT_BOOL                     enable
)
{
    GT_U32    portControl;       /* register address */
    GT_STATUS rc = GT_OK;        /* return code */
    GT_U32    data;              /* reg subfield data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);

    portControl = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    data = BOOL2BIT_MAC(enable);

    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
            portControl, 19, 1, data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbNaStormPreventGet
*
* DESCRIPTION:
*   Get status of New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
* INPUTS:
*       devNum     - device number
*       port       - port number
*
* OUTPUTS:
*       enablePtr     - pointer to the status of repeated NA CPU messages
*                       GT_TRUE - NA Storm Prevention (SP) is enabled,
*                       GT_FALSE - NA Storm Prevention is disabled
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/portNum/auMsgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaStormPreventGet
(
    IN  GT_U8       devNum,
    IN  GT_U8       port,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32    portControl;       /* register address */
    GT_STATUS rc = GT_OK;        /* return code */
    GT_U32    data;              /* reg subfield data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(devNum,port);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    portControl = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.portControl[port];

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port),
            portControl, 19, 1, &data);

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;

}


/*******************************************************************************
* cpssDxChBrgFdbDeviceTableSet
*
* DESCRIPTION:
*       This function sets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*       So for proper work of PP the application must set the relevant bits of
*       all devices in the system prior to inserting FDB entries associated with
*       them
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       devTableBmp - bmp of devices to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDeviceTableSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  devTableBmp
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* save the info to DB --- needed in the AU messages handling
       PART of WA for Erratum
    */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.devTable = devTableBmp;

    return prvCpssDrvHwPpWriteRegister(devNum,
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.deviceTable,
                      devTableBmp);
}

/*******************************************************************************
* cpssDxChBrgFdbDeviceTableGet
*
* DESCRIPTION:
*       This function gets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       devTableBmpPtr - pointer to bmp of devices to set.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDeviceTableGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *devTableBmpPtr
)
{
    GT_U32      regAddr;        /* register address */
    GT_U32      hwData;         /* hw data from register */
    GT_STATUS   rc;             /* return code      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(devTableBmpPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.deviceTable;

    rc = prvCpssDrvHwPpReadRegister(devNum, regAddr, &hwData);

    *devTableBmpPtr = hwData;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryPtr  - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number or portGroupsBmp.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete ,
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend and
*       cpssDxChBrgFdbPortGroupMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*       for multi-port groups device :
*           1. Application should not use this API to add new MAC addresses once
*              SP feature is enabled in any of the ports, due to the fact it may
*              fail on one or more of the portGroups - in this case need
*              to write by index.
*           2. Updating existing MAC addresses is not affected and
*              can be preformed with no problem
*           3. application should be aware that when configuring MAC addresses only
*              to portGroups which none of them is the real source portGroups,
*              these entries would be "unseen" from AU perspective
*              (HW is filtering any AA, TA, QR etc... messages these entries invokes).
*              And although these entries are "Unseen" there are still used for forwarding
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntrySet
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_U32 hwData[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]; /* words of Au hw entry */
    GT_STATUS  rc;                                  /* return status        */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(macEntryPtr);

    /* build entry hw format */
    rc = prvDxChFdbToAuConvert(devNum,
                               &(macEntryPtr->key),
                               macEntryPtr,
                               CPSS_NA_E,
                               GT_FALSE,
                               &hwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* write data to Hw */
        rc = prvDxChFdbSendAuMessage(devNum,portGroupId ,&hwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryPtr     - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete ,
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend and
*       cpssDxChBrgFdbPortGroupMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/

GT_STATUS cpssDxChBrgFdbMacEntrySet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    return cpssDxChBrgFdbPortGroupMacEntrySet(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,macEntryPtr);
}


/*******************************************************************************
* cpssDxChBrgFdbPortGroupQaSend
*
* DESCRIPTION:
*       The function Send Query Address (QA) message to the hardware MAC address
*       table.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that QA message was processed by PP.
*       The PP sends Query Response message after QA processing.
*       An application can get QR message by general AU message get API.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryKeyPtr  - pointer to mac entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/vlan or portGroupsBmp
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete ,
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend and
*       cpssDxChBrgFdbPortGroupMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the QA message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupQaSend
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC  *macEntryKeyPtr
)
{
    GT_STATUS  rc;                                  /* return status        */
    GT_U32 hwData[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]; /* words of Au hw entry */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(macEntryKeyPtr);

    /* build entry hw format */
    rc = prvDxChFdbToAuConvert(devNum,
                               macEntryKeyPtr,
                               NULL,
                               CPSS_QA_E,
                               GT_FALSE,
                               &hwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* write data to Hw */
        rc = prvDxChFdbSendAuMessage(devNum,portGroupId,&hwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbQaSend
*
* DESCRIPTION:
*       The function Send Query Address (QA) message to the hardware MAC address
*       table.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that QA message was processed by PP.
*       The PP sends Query Response message after QA processing.
*       An application can get QR message by general AU message get API.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to mac entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/vlan
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete ,
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend and
*       cpssDxChBrgFdbPortGroupMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the QA message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQaSend
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    return cpssDxChBrgFdbPortGroupQaSend(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,macEntryKeyPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table through Address Update
*       message.(AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number or portGroupsBmp.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete ,
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend and
*       cpssDxChBrgFdbPortGroupMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryDelete
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    GT_U32 hwData[CPSS_DXCH_FDB_AU_WORDS_SIZE_CNS]; /* words of Au hw entry */
    GT_STATUS  rc;                                  /* return status        */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(macEntryKeyPtr);

    /* build entry hw format */
    rc = prvDxChFdbToAuConvert(devNum,
                               macEntryKeyPtr,
                               NULL,
                               CPSS_NA_E,
                               GT_TRUE,
                               &hwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* write data to Hw */
        rc = prvDxChFdbSendAuMessage(devNum, portGroupId,&hwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table through Address Update
*       message.(AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete ,
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend and
*       cpssDxChBrgFdbPortGroupMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryDelete
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    return cpssDxChBrgFdbPortGroupMacEntryDelete(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,macEntryKeyPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
*
*       for multi-port groups device :
*           1. Unified FDBs mode :
*              API should return GT_BAD_PARAM if value other then CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*              was passed when working in this mode, due to the fact that all
*              FDBs are supposed to be synced.
*              Application should use this API to add new MAC addresses once SP
*              feature is enabled in any of the ports , due to the fact
*              cpssDxChBrgFdbMacEntrySet/cpssDxChBrgFdbPortGroupMacEntrySet API
*              may fail on one or more of the portGroups.
*           2. Unified-Linked FDB mode / Linked FDBs mode :
*              Setting either CPSS_PORT_GROUP_UNAWARE_MODE_CNS or any subset of
*              portGroup can be supported in this mode, still application should
*              note that in Linked FDBs when using multiple portGroups, setting
*              same index would potentially result affecting up to 4 different
*              MAC addresses (up to 2 different MAC addresses in Unified-Linked
*              FDB mode).
*           3. application should be aware that when configuring MAC addresses only
*              to portGroups which none of them is the real source portGroups,
*              these entries would be "unseen" from AU perspective
*              (HW is filtering any AA, TA, QR etc... messages these entries invokes).
*              And although these entries are "Unseen" there are still used for forwarding
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index           - hw mac entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       macEntryPtr     - pointer to MAC entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum,portGroupsBmp,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    GT_U32 hwData[CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS];
                                            /* words of mac hw entry    */
    GT_STATUS  rc;                          /* return code              */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(macEntryPtr);

    PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index);

    rc = prvDxChBrgFdbBuildMacEntryHwFormat(devNum,
                                            macEntryPtr,
                                            skip,
                                            hwData);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* write entry to specific index format */
        rc = prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,
                                        PRV_CPSS_DXCH_TABLE_FDB_E,
                                        index,
                                        &hwData[0]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       index           - hw mac entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       macEntryPtr     - pointer to MAC entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    return cpssDxChBrgFdbPortGroupMacEntryWrite(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
        index,skip,macEntryPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
)
{
    GT_STATUS  rc;
    GT_U32     portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_DXCH_TABLE_ENT tableType ;
    GT_U32     fieldWordNum = 0;/* The 1st word contains the Valid and Skip bits */
    GT_U32     fieldOffset  = 0;/* The offset in the word                        */
    GT_U32     fieldLength  = 32;/* The length to be read                        */
    GT_U32     fieldValue;        /* The 1st word content */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(skipPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);
    PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index);

    tableType = PRV_CPSS_DXCH_TABLE_FDB_E;

    rc =  prvCpssDxChPortGroupReadTableEntryField(devNum,
                                             portGroupId,
                                             tableType,
                                             index,
                                             fieldWordNum,
                                             fieldOffset,
                                             fieldLength,
                                             &fieldValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* valid bit */
    *validPtr = BIT2BOOL_MAC(U32_GET_FIELD_MAC(fieldValue,0,1));

    /* skip bit */
    *skipPtr = BIT2BOOL_MAC(U32_GET_FIELD_MAC(fieldValue,1,1));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
)
{
    return cpssDxChBrgFdbPortGroupMacEntryStatusGet(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index,validPtr,skipPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedDevNumPtr = (pointer to) is device number associated with the
*                     entry (even for vidx/trunk entries the field is used by
*                     PP for aging/flush/transplant purpose).
*                     Relevant only in case of Mac Address entry.
*       entryPtr    - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_U8                   *associatedDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
)
{
    GT_STATUS rc;                   /* return code  */
    GT_U32    macEntryType;         /* entry type   */
    GT_U16    vlanId;               /* vlan ID      */
    GT_U32    multiple;             /* multiple bit */
    GT_U32    macAddr40bit;         /* the 40th bit from Mac address */
    GT_BOOL   useVidx = GT_FALSE;   /* flag for using vidx */
    GT_U32    hwValue;              /* temporary hw value */
    GT_U32    hwData[CPSS_DXCH_FDB_MAC_ENTRY_WORDS_SIZE_CNS];
                                    /* hw data      */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(skipPtr);
    CPSS_NULL_PTR_CHECK_MAC(agedPtr);
    CPSS_NULL_PTR_CHECK_MAC(associatedDevNumPtr);
    CPSS_NULL_PTR_CHECK_MAC(entryPtr);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);
    PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index);

    rc = prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,
                                   PRV_CPSS_DXCH_TABLE_FDB_E,
                                   index,
                                   &hwData[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    cpssOsMemSet(entryPtr,0,sizeof(CPSS_MAC_ENTRY_EXT_STC));

    /* valid bit */
    *validPtr = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[0],0,1));

    /* skip bit */
    *skipPtr = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[0],1,1));

    /* age bit */
    *agedPtr = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[0],2,1));

    entryPtr->age = *agedPtr;


    /* get entry type */
    macEntryType = U32_GET_FIELD_MAC(hwData[0],3,2);

    /* get vlanId */
    vlanId = (GT_U16)U32_GET_FIELD_MAC(hwData[0],5,12);


    if (macEntryType == 0)
    {
        /* MAC Address entry */

        /* set MAC Address */
        entryPtr->key.key.macVlan.macAddr.arEther[5] =
                        (GT_U8)U32_GET_FIELD_MAC(hwData[0],17,8);

        entryPtr->key.key.macVlan.macAddr.arEther[4] =
                        (GT_U8)(U32_GET_FIELD_MAC(hwData[0],25,7) |
                                (U32_GET_FIELD_MAC(hwData[1],0,1) << 7));

        entryPtr->key.key.macVlan.macAddr.arEther[3] =
                         (GT_U8)U32_GET_FIELD_MAC(hwData[1],1,8);

        entryPtr->key.key.macVlan.macAddr.arEther[2] =
                         (GT_U8)U32_GET_FIELD_MAC(hwData[1],9,8);

        entryPtr->key.key.macVlan.macAddr.arEther[1] =
                         (GT_U8)U32_GET_FIELD_MAC(hwData[1],17,8);

        entryPtr->key.key.macVlan.macAddr.arEther[0] =
                         (GT_U8)(U32_GET_FIELD_MAC(hwData[1],25,7) |
                                 (U32_GET_FIELD_MAC(hwData[2],0,1) << 7));
        /* set vid */
        entryPtr->key.key.macVlan.vlanId = vlanId;

        /* get multiple bit */
        multiple = U32_GET_FIELD_MAC(hwData[2],26,1);

        /* get MacAddr[40] bit */
        macAddr40bit = (entryPtr->key.key.macVlan.macAddr.arEther[0] & 0x1);

        /* associated devNum number */
        *associatedDevNumPtr = (GT_U8)U32_GET_FIELD_MAC(hwData[2],1,5);

        /* set interface parameters */
        if((multiple) || (macAddr40bit))
        {
            /* multicast entry */
            useVidx = GT_TRUE;

        }
        else if(U32_GET_FIELD_MAC(hwData[2],13,1))/* trunk bit */
        {
            entryPtr->dstInterface.type = CPSS_INTERFACE_TRUNK_E;
            entryPtr->dstInterface.trunkId =
                            (GT_TRUNK_ID)U32_GET_FIELD_MAC(hwData[2],14,7);
            /* user defined */
            entryPtr->userDefined = U32_GET_FIELD_MAC(hwData[2],21,4);

        }
        else
        {
            entryPtr->dstInterface.type = CPSS_INTERFACE_PORT_E;
            entryPtr->dstInterface.devPort.portNum =
                            (GT_U8)U32_GET_FIELD_MAC(hwData[2],14,6);
            entryPtr->dstInterface.devPort.devNum = *associatedDevNumPtr;
            /* user defined */
            entryPtr->userDefined = U32_GET_FIELD_MAC(hwData[2],21,4);

        }

        /* SA cmd */
        hwValue = ((U32_GET_FIELD_MAC(hwData[2],30,2)) |
                   ((U32_GET_FIELD_MAC(hwData[3],0,1)) << 2));

        PRV_CPSS_DXCH_FDB_CONVERT_HW_CMD_TO_SW_VAL_MAC(entryPtr->saCommand,
                                                       hwValue);
        /* Source ID */
        entryPtr->sourceID = U32_GET_FIELD_MAC(hwData[2],6,5);
    }

    else
    {
        /* IP Multicast entry */

        /* set DIP Address */
        entryPtr->key.key.ipMcast.dip[3] =
                        (GT_U8)U32_GET_FIELD_MAC(hwData[0],17,8);

        entryPtr->key.key.ipMcast.dip[2] =
                        (GT_U8)(U32_GET_FIELD_MAC(hwData[0],25,7) |
                                (U32_GET_FIELD_MAC(hwData[1],0,1) << 7));

        entryPtr->key.key.ipMcast.dip[1] =
                        (GT_U8)U32_GET_FIELD_MAC(hwData[1],1,8);

        entryPtr->key.key.ipMcast.dip[0] =
                        (GT_U8)U32_GET_FIELD_MAC(hwData[1],9,8);

        /* set SIP Address */
        entryPtr->key.key.ipMcast.sip[3] =
                        (GT_U8)U32_GET_FIELD_MAC(hwData[1],17,8);

        entryPtr->key.key.ipMcast.sip[2] =
                        (GT_U8)(U32_GET_FIELD_MAC(hwData[1],25,7) |
                                (U32_GET_FIELD_MAC(hwData[2],0,1) << 7));

        entryPtr->key.key.ipMcast.sip[1] =
                        (GT_U8)U32_GET_FIELD_MAC(hwData[2],1,8);

        entryPtr->key.key.ipMcast.sip[0] =
                        (GT_U8)(U32_GET_FIELD_MAC(hwData[2],9,4)         |
                                (U32_GET_FIELD_MAC(hwData[2],26,1) << 4) |
                                (U32_GET_FIELD_MAC(hwData[2],30,2) << 5) |
                                (U32_GET_FIELD_MAC(hwData[3],0,1)  << 7));

        /* set VID */
        entryPtr->key.key.ipMcast.vlanId = vlanId;

        useVidx = GT_TRUE;
    }

    if (useVidx)
    {
        entryPtr->dstInterface.vidx = (GT_U16)U32_GET_FIELD_MAC(hwData[2],13,12);

        /* set destination interface type to VID or VIDX */
        if(entryPtr->dstInterface.vidx == 0xFFF)
        {
            entryPtr->dstInterface.type = CPSS_INTERFACE_VID_E;
            /* set VID */
            entryPtr->dstInterface.vlanId = vlanId;
        }
        else
        {
            /* set vidx */
            entryPtr->dstInterface.type = CPSS_INTERFACE_VIDX_E;
        }
    }

    /* set mac entry type */
    switch(macEntryType)
    {
        case 0:
            entryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
            break;
        case 1:
            entryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E;
            break;
        case 2:
            entryPtr->key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E;
            break;
        default:
            return GT_BAD_PARAM;
    }



    /* set DA cmd */
    hwValue = (GT_U8)U32_GET_FIELD_MAC(hwData[2],27,3);
    PRV_CPSS_DXCH_FDB_CONVERT_HW_CMD_TO_SW_VAL_MAC(entryPtr->daCommand,
                                                   hwValue);
    /* set static */
    entryPtr->isStatic = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[2],25,1));

    /* DA Route */
    entryPtr->daRoute = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[3],1,1));

    /* spUnknown - NA strorm prevent entry */
    entryPtr->spUnknown = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[3],2,1));

    /* SA QoS Profile index */
    entryPtr->saQosIndex = U32_GET_FIELD_MAC(hwData[3],3,3);

    /* DA QoS Profile index */
    entryPtr->daQosIndex = U32_GET_FIELD_MAC(hwData[3],6,3);

    /* Mirror to Analyzer Port */
    entryPtr->mirrorToRxAnalyzerPortEn = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[3],9,1));

    /* set Cheetah2 fields */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
    {
        /* Application Specific CPU Code */
        entryPtr->appSpecificCpuCode = BIT2BOOL_MAC(U32_GET_FIELD_MAC(hwData[3],10,1));

        /* DA Access Level */
        entryPtr->daSecurityLevel = U32_GET_FIELD_MAC(hwData[3],11,3);

        /* SA Access Level */
        entryPtr->saSecurityLevel = U32_GET_FIELD_MAC(hwData[3],14,3);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedDevNumPtr = (pointer to) is device number associated with the
*                     entry (even for vidx/trunk entries the field is used by
*                     PP for aging/flush/transplant purpose).
*                     Relevant only in case of Mac Address entry.
*       entryPtr    - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_U8                   *associatedDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
)
{
    return cpssDxChBrgFdbPortGroupMacEntryRead(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index,validPtr,skipPtr,
        agedPtr,associatedDevNumPtr,entryPtr);
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index       - hw mac entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN GT_U32        index
)
{
    GT_STATUS   rc;
    GT_U32      hwData = 0;/* value to write to first word of the FDB entry */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index);

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
    /* write 0 to first word */
        rc = prvCpssDxChPortGroupWriteTableEntryField(devNum,portGroupId,
                                         PRV_CPSS_DXCH_TABLE_FDB_E,
                                         index,
                                         0,
                                         0,
                                         32,
                                         hwData);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_U32        index
)
{
    return cpssDxChBrgFdbPortGroupMacEntryInvalidate(devNum,
        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index);
}
/*******************************************************************************
* cpssDxChBrgFdbMaxLookupLenSet
*
* DESCRIPTION:
*       Set the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       lookupLen   - The maximal length of MAC table lookup, this must be
*                     value divided by 4 with no left over.
*                     in range 4..32. (4,8,12..32)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_OUT_OF_RANGE          - lookupLen > 32 or lookupLen < 4
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Maximal Hash chain length in HW calculated as follows:
*       Maximal Hash chain length = (regVal + 1) * 4
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMaxLookupLenSet
(
    IN GT_U8    devNum,
    IN GT_U32   lookupLen
)
{
    GT_U32      regAddr;    /* The register's address of l2IngressCtrl reg. */
    GT_U32      value;      /* The value to be written to the register.     */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(lookupLen & 0x3) /* only divided by 4 with no left over */
    {
        return GT_BAD_PARAM;
    }

    value = (lookupLen >> 2) - 1;

    if(lookupLen < 4 || value > 7)/* 3 bits for the field */
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 3, value);
}

/*******************************************************************************
* cpssDxChBrgFdbMaxLookupLenGet
*
* DESCRIPTION:
*       Get the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       lookupLenPtr   - The maximal length of MAC table lookup, this must be
*                        value divided by 4 with no left over.
*                        in range 4..32. (4,8,12..32)
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Maximal Hash chain length in HW calculated as follows:
*       Maximal Hash chain length = (regVal + 1) * 4
*
*******************************************************************************/
 GT_STATUS cpssDxChBrgFdbMaxLookupLenGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *lookupLenPtr
)
{
    GT_U32  regAddr;        /* The register's address of l2IngressCtrl reg. */
    GT_U32  hwValue;        /* The value to be read from the register.     */
    GT_STATUS   rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(lookupLenPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 3, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *lookupLenPtr = ((hwValue + 1) << 2);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacVlanLookupModeSet
*
* DESCRIPTION:
*       Sets the VLAN Lookup mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad param
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacVlanLookupModeSet
(
    IN GT_U8                devNum,
    IN CPSS_MAC_VL_ENT      mode
)
{
    GT_U32      regAddr;    /* register address             */
    GT_U32      value;      /* value to write into register */
    GT_STATUS   rc;         /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch(mode)
    {
        case CPSS_IVL_E:
            value = 1;
            break;
        case CPSS_SVL_E:
            value = 0;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 1, value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Update FDB hash parameters */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.vlanMode = mode;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacVlanLookupModeGet
*
* DESCRIPTION:
*       Get VLAN Lookup mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad param
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacVlanLookupModeGet
(
    IN  GT_U8            devNum,
    OUT CPSS_MAC_VL_ENT  *modePtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 hwValue;     /* value to read from register  */
    GT_STATUS rc;       /* return status                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 3, 1, &hwValue);

    *modePtr = (hwValue == 1) ? CPSS_IVL_E : CPSS_SVL_E;

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbAuMsgRateLimitSet
*
* DESCRIPTION:
*     Set Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       msgRate    - maximal rate of AU messages per second
*       enable     - enable/disable message limit
*                    GT_TRUE - message rate is limited with msgRate
*                    GT_FALSE - there is no limitations, WA to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_OUT_OF_RANGE          - out of range.
*       GT_BAD_PARAM             - on bad parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when DxCh Clock 200 MHz(DxCh2 220 MHz Clock) ,
*          granularity and rate changed linearly depending on clock.
*
*     for multi-port groups device :
*       The rate-limiting is done individually in each one of the port Groups.
*       This means that potentially configuring this value would result a rate
*       which is up to n times bigger then expected (n is number of port groups)
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAuMsgRateLimitSet
(
    IN GT_U8                       devNum,
    IN GT_U32                      msgRate,
    IN GT_BOOL                     enable
)
{
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwValue;          /* value to write to register   */
    GT_U32    rate;             /* value of "rate" subfield     */
    GT_U32    granularity;      /* granularity                  */
    GT_U32    window;           /* window in uSec               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    /* convert specified rate to HW format */
    granularity = 2;

    window      = (10000 * PRV_CPSS_PP_MAC(devNum)->baseCoreClock) / PRV_CPSS_PP_MAC(devNum)->coreClock;

    rate = ((msgRate / granularity) * window) / 1000000;

    /* max rate in a 10ms window is 0xFF */
    if ((rate >= BIT_8) && enable)
    {
        return GT_OUT_OF_RANGE;
    }

    /* store enable bit and rate together */
    hwValue = (BOOL2BIT_MAC(enable) << 8) | rate;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 10, 9, hwValue);
}

/*******************************************************************************
* cpssDxChBrgFdbAuMsgRateLimitGet
*
* DESCRIPTION:
*     Get Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       msgRatePtr - (pointer to) maximal rate of AU messages per second
*       enablePtr  - (pointer to) GT_TRUE - message rate is limited with
*                                           msgRatePtr
*                                 GT_FALSE - there is no limitations, WA to CPU
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_OUT_OF_RANGE          - out of range.
*       GT_BAD_PARAM             - on bad parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when DxCh Clock 200 MHz(DxCh2 220 MHz Clock) ,
*          granularity and rate changed linearly depending on clock.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAuMsgRateLimitGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *msgRatePtr,
    OUT GT_BOOL     *enablePtr
)
{
    GT_U32    regAddr;          /* register address             */
    GT_U32    hwValue;          /* value to read from register  */
    GT_STATUS rc;               /* return code                  */
    GT_U32    rate;             /* value of "rate" subfield     */
    GT_U32    granularity;      /* granularity                  */
    GT_U32    window;           /* window in uSec               */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(msgRatePtr);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    granularity = 2;

    window      = (10000 * PRV_CPSS_PP_MAC(devNum)->baseCoreClock) / PRV_CPSS_PP_MAC(devNum)->coreClock;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 10, 9, &hwValue);
    if (rc != GT_OK)
    {
        return GT_HW_ERROR;
    }

    rate = (hwValue & 0xFF);

    /* convert HW value to SW rate */
    *msgRatePtr = ((rate * granularity * 1000000) / window) ;

    *enablePtr = BIT2BOOL_MAC(((hwValue >> 8) & 0x1));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbNaMsgOnChainTooLongSet
*
* DESCRIPTION:
*       Enable/Disable sending NA messages to the CPU indicating that the device
*       cannot learn a new SA . It has reached its max hop (bucket is full).
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgOnChainTooLongSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    data = (enable == GT_TRUE) ? 1 : 0;

    /* Enable/Disable NA msg to CPU if chain too long */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 4, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbNaMsgOnChainTooLongGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending NA messages to the CPU
*       indicating that the device cannot learn a new SA. It has reached its
*       max hop (bucket is full).
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the NA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgOnChainTooLongGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32    regAddr;           /* register address    */
    GT_U32    data;              /* reg sub field data  */
    GT_STATUS rc;                /* return code         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 4, 1, &data);

    /* Enable/Disable NA msg to CPU if chain too long */
    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbSpAaMsgToCpuSet
*
* DESCRIPTION:
*       Enabled/Disabled sending Aged Address (AA) messages to the CPU for the
*       Storm Prevention (SP) entries when those entries reach an aged out status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the SP AA message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*           Whenever applicable it's highly recommended disabling these messages,
*           as enabling this would stress the AUQ with excessive messages
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSpAaMsgToCpuSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    data = (enable == GT_TRUE) ? 1 : 0;

    /* Enable/Disable SP AA msg to CPU */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 5, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbSpAaMsgToCpuGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending Aged Address (AA) messages
*       to the CPU for the Storm Prevention (SP) entries when those entries reach
*       an aged out status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the SP AA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSpAaMsgToCpuGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32    regAddr;           /* register address    */
    GT_U32    data;              /* reg sub field data  */
    GT_STATUS rc;                /* return code         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 5, 1, &data);

    /* Enable/Disable status of SP AA msg to CPU */
    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbAAandTAToCpuSet
*
* DESCRIPTION:
*       Enable/Disable the PP to/from sending an AA and TA address
*       update messages to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable the message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or auMsgType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           Whenever applicable it's highly recommended disabling these messages,
*           as enabling this would stress the AUQ with excessive messages
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAAandTAToCpuSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
{
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to write into register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* Disable/Enable TA and AA */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;
    value = (enable == GT_TRUE) ? 1 : 0;

    /* Disable/Enable TA and AA */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 19, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbAAandTAToCpuGet
*
* DESCRIPTION:
*       Get state of sending an AA and TA address update messages to the CPU
*       as configured to PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - the messages to CPU enabled
*                                  GT_FALSE - the messages to CPU disabled
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or auMsgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAAandTAToCpuGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
{
    GT_STATUS rc;           /* HW write return value*/
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to read from register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* HW support only AA,TA */

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    /* Get Disable/Enable TA and AA */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 19, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbHashModeSet
*
* DESCRIPTION:
*       Sets the FDB hash function mode.
*       The CRC based hash function provides the best hash index destribution
*       for random addresses and vlans.
*       The XOR based hash function provides optimal hash index destribution
*       for controlled testing scenarios, where sequential addresses and vlans
*       are often used.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - hash function based mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_HASH_FUNC_MODE_ENT  mode
)
{
    GT_U32      regAddr;    /* register adress              */
    GT_U32      data;       /* data to write to register    */
    GT_STATUS   rc;         /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch (mode)
    {
        case CPSS_MAC_HASH_FUNC_XOR_E:
            data = 0x0;
            break;
        case CPSS_MAC_HASH_FUNC_CRC_E:
            data = 0x1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    /* Set the FDBHashMode in the FDB Global Configuration Register */
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 21, 1, data);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Update FDB hash parameters */
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode = mode;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbHashModeGet
*
* DESCRIPTION:
*       Gets the FDB hash function mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - hash function based mode:
*                 CPSS_FDB_HASH_FUNC_XOR_E - XOR based hash function mode
*                 CPSS_FDB_HASH_FUNC_CRC_E - CRC based hash function mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashModeGet
(
    IN  GT_U8                         devNum,
    OUT CPSS_MAC_HASH_FUNC_MODE_ENT   *modePtr
)
{
    GT_STATUS rc;           /* HW write return value*/
    GT_U32 regAddr;         /* register address */
    GT_U32 value;           /* value to read from register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 21, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *modePtr = (value == 1) ? CPSS_MAC_HASH_FUNC_CRC_E :
                              CPSS_MAC_HASH_FUNC_XOR_E;
    return GT_OK;
}

/******************************************************************************
* cpssDxChBrgFdbAgingTimeoutSet
*
* DESCRIPTION:
*       Sets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE :
*       For DxCh3 the device support aging time with granularity of 16 seconds
*       (16,32..1008) . So value of 226 will be round down to 224 , and value of
*       255 will be round up to 256.
*       For others the device support aging time with
*       granularity of 10 seconds (10,20..630) .
*       So value of 234 will be round down to 230 ,
*       and value of 255 will be round up to 260.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       timeout - aging time in seconds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   For the 270MHz range from 16..1008 (but due to rounding we support values 8..636)
*   HW supports steps 16
*   For the 220MHz range from 9..573 (but due to rounding we support values 5..577)
*   HW supports steps 9 (9.09)
*   For the 200MHz range from 10..630 (but due to rounding we support values 5..634)
*   HW supports steps 10
*   For the 144 MHz range from 14..875 (but due to rounding we support values 7..882)
*   HW supports steps 13.88
*   For the 167 MHz range from 14..886 (but due to rounding we support values 7..893)
*   HW supports steps 14 (14.07)
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgingTimeoutSet
(
    IN GT_U8  devNum,
    IN GT_U32 timeout
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      refClock;   /* reference clock for aging unit calculation */
    GT_STATUS   rc;         /* return status    */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    refClock = PRV_CPSS_PP_MAC(devNum)->baseCoreClock;
    /* xCat and xCat2 have reference clock different from base one (222 MHz). */
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            refClock = 235; /* aging_unit = 2350/core_clock [sec] */
            break;
        default: break;
    }

    /* adjust timeout value according to aging unit.
       aging unit depends on core clock.
       register_value = timeout / aging_unit
       where aging_unit = reference_clock / core_clock  */
    timeout = (timeout * PRV_CPSS_PP_MAC(devNum)->coreClock) / refClock;

    if((PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E))
    {
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_FDB_AGING_TIMEOUT_WA_E))
        {
           /* To round the number to nearest (*20) value,
              add 10 before divide by 20 .*/
            timeout = (timeout + 10) / 20;
        }
        else
        {
            /* To round the number to nearest (*10) value, add 5 before divide by 10 .
            So value of 234 will be round down to 230 , and value of 255 will be
            round up to 260.
            */
            timeout = (timeout + 5) / 10;
        }
    }
    /* DxCh3 devices */
    else
    {
        /* To round the number to nearest (*16) value, add 8 before divide by 16 .
        So value of 226 will be round down to 224 , and value of 255 will be
        round up to 256.
        */
        timeout = (timeout + 8) / 16;
    }
    /* check boundary */
    if (timeout == 0 || timeout > 63) /* values according to HW spec */
    {
        rc = GT_BAD_PARAM;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

        /* write ActionTimer to FDB Action0 Register */
        rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 5, 6, (timeout));
    }

    return rc;
}

/******************************************************************************
* cpssDxChBrgFdbAgingTimeoutGet
*
* DESCRIPTION:
*       Gets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE :
*       For DxCh3 the device support aging time with granularity of 16 seconds
*       (16,32..1008).
*       For others the device support aging time with
*       granularity of 10 seconds (10,20..630) .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       timeoutPtr - pointer to aging time in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For core clock of 270MHz The value ranges from 16 seconds to 1008
*       seconds in steps of 16 seconds.
*       For core clock of 220MHz The value ranges from 9 seconds to 573
*       seconds in steps of 9 seconds.
*       For core clock of 200 MHZ The value ranges from 10 seconds to 630
*       seconds in steps of 10 seconds.
*       For core clock of 144MHZ the aging timeout ranges from 14 (13.8) to 875
*       seconds in steps of 14 seconds.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgingTimeoutGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *timeoutPtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_STATUS   rc;         /* return status                */
    GT_U32      hwValue;    /* value to read from register  */
    GT_U32      refClock;   /* reference clock for aging unit calculation */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(timeoutPtr);

    refClock = PRV_CPSS_PP_MAC(devNum)->baseCoreClock;
    /* xCat and xCat2 have reference clock different from base one (222 MHz). */
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            refClock = 235; /* aging_unit = 2350/core_clock [sec] */
            break;
        default: break;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 5, 6, &hwValue);

    if((PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_CHEETAH2_E) ||
       (PRV_CPSS_PP_MAC(devNum)->devFamily >= CPSS_PP_FAMILY_DXCH_XCAT_E))
    {
        if(GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
            PRV_CPSS_DXCH_XCAT_FDB_AGING_TIMEOUT_WA_E))
        {
            *timeoutPtr = ((hwValue * 20 * refClock) /
                           PRV_CPSS_PP_MAC(devNum)->coreClock);
        }
        else
        {
            *timeoutPtr = ((hwValue * 10 * refClock) /
                            PRV_CPSS_PP_MAC(devNum)->coreClock);
        }
    }
    else
    {
        *timeoutPtr = ((hwValue * 16 * refClock) /
                       PRV_CPSS_PP_MAC(devNum)->coreClock);
    }
    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbSecureAgingSet
*
* DESCRIPTION:
*      Enable or disable secure aging. This mode is relevant to automatic or
*      triggered aging with removal of aged out FDB entries. This mode is
*      applicable together with secure auto learning mode see
*      cpssDxChBrgFdbSecureAutoLearnSet. In the secure aging mode the device
*      sets aged out unicast FDB entry with <Multiple> =  1 and VIDX = 0xfff but
*      not invalidates entry. This causes packets destined to this FDB entry MAC
*      address to be flooded to the VLAN.  But such packets are known ones.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable secure aging
*                 GT_FALSE - disable secure aging
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for multi-port groups device :
*       1. Unified FDBs mode and Unified-Linked FDBs mode :
*           Since Automatic learning is not supported in this mode nor secured automatic
*           learning - then there is no sense with supporting this feature either.
*           API returns GT_BAD_STATE if trying to enable it
*       2. Linked FDBs mode - no limitations
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAgingSet
(
    IN GT_U8          devNum,
    IN GT_BOOL        enable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    data = (enable == GT_TRUE) ? 1 : 0;

    /* Enable/Disable secure aging */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 29, 1, data);

}

/*******************************************************************************
* cpssDxChBrgFdbSecureAgingGet
*
* DESCRIPTION:
*      Get the status (enabled/disabled) of secure aging.
*      This mode is relevant to automatic or triggered aging with removal of
*      aged out FDB entries. This mode is applicable together with secure auto
*      learning mode see cpssDxChBrgFdbSecureAutoLearnGet. In the secure aging
*      mode  the device sets aged out unicast FDB entry with <Multiple> =  1 and
*      VIDX = 0xfff but not invalidates entry. This causes packets destined to
*      this FDB entry MAC address to be flooded to the VLAN.
*      But such packets are known ones.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable secure aging
*                    GT_FALSE - disable secure aging
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAgingGet
(
    IN  GT_U8          devNum,
    OUT GT_BOOL        *enablePtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_STATUS   rc;         /* return status                */
    GT_U32      hwValue;    /* value to read from register  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 29, 1, &hwValue);

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbSecureAutoLearnSet
*
* DESCRIPTION:
*        Configure secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       mode    - Enable/Disable secure automatic learning.
*                 The command assigned to packet with new SA not found in FDB
*                 when secure automatic learning enable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAutoLearnSet
(
    IN GT_U8                                devNum,
    IN CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  mode
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg subfield data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    /* enable security Automatic Learning */
    data = 1;

    switch(mode)
    {
        case CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E:
            /* disable security Automatic Learning, New source addresses
               received on Port/Vlan configured to Automatic learning are
               learned automatically */
            data = 0;
            break;
        case CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E:
            /* enable security Automatic Learning , Set security automatic learn
             unknown Source command: 0 - trap to CPU */
            break;
        case CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E:
            /* enable security Automatic Learning , Set security automatic learn
            unknown Source command:1 - hard drop */
            data |= (1 << 1);
            break;
        case CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E:
            /* enable security Automatic Learning , Set security automatic learn
            unknown Source command: 2 - soft drop */
            data |= (2 << 1);
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                            bridgeRegs.bridgeGlobalConfigRegArray[0];

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 18, 3, data);

}

/*******************************************************************************
* cpssDxChBrgFdbSecureAutoLearnGet
*
* DESCRIPTION:
*        Get secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr  - Enabled/Disabled secure automatic learning.
*                  The command assigned to packet with new SA not found in FDB
*                  when secure automatic learning enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAutoLearnGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  *modePtr
)
{
    GT_U32      regAddr;    /* register address             */
    GT_STATUS   rc;         /* return status                */
    GT_U32      hwValue;    /* value to read from register  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* Get Bridge Global Configuration Register0 address */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.bridgeGlobalConfigRegArray[0];

    /* get <Secure Auto Learn Enable> and
      <Secure Auto Learning Unknown Src Command> */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 18, 3, &hwValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    switch (hwValue)
    {
        case 0:
            *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_DISABLED_E;
            /* Security Automatic Learning is disabled. New source addresses
               received on Port/Vlan configured to Automatic learning are
               learned automatically */
            break;
        case 1:
            *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_UNK_TRAP_E;
            /* Security Automatic Learning enabled, Security automatic learn
             unknown Source command is trap to CPU */
            break;
        case 3:
            *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_UNK_HARD_DROP_E;
            /* Security Automatic Learning enabled, Security automatic learn
             unknown Source command is hard drop */
            break;
        case 5:
            *modePtr = CPSS_MAC_SECURE_AUTO_LEARN_UNK_SOFT_DROP_E;
            /* Security Automatic Learning enabled, Security automatic learn
             unknown Source command is soft drop */
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbStaticTransEnable
*
* DESCRIPTION:
*       This routine determines whether the transplanting operate on static
*       entries.
*       When the PP will do the transplanting , it will/won't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it will/won't Flush the FDB static entries
*       (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE transplanting is enabled on static entries,
*                GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticTransEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    value = (enable == GT_TRUE) ? 1 : 0;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 11, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbStaticTransEnableGet
*
* DESCRIPTION:
*       Get whether the transplanting enabled to operate on static entries.
*       when the PP do the transplanting , it do/don't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it do/don't Flush the FDB static entries
*       (entries that are not subject to aging).
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
*       enablePtr - GT_TRUE transplanting is enabled on static entries,
*                   GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticTransEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */
    GT_STATUS rc;       /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 11, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbStaticDelEnable
*
* DESCRIPTION:
*       This routine determines whether flush delete operates on static entries.
*       When the PP will do the Flush , it will/won't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE delete is enabled on static entries, GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticDelEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    value = (enable == GT_TRUE) ? 1 : 0;

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 12, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbStaticDelEnableGet
*
* DESCRIPTION:
*       Get whether flush delete operates on static entries.
*       When the PP do the Flush , it do/don't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE delete is enabled on static entries,
*                    GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticDelEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */
    GT_STATUS rc;       /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 12, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbActionsEnableSet
*
* DESCRIPTION:
*       Enables/Disables FDB actions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE -  Actions are enabled
*                 GT_FALSE -  Actions are disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    value = BOOL2BIT_MAC(enable);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbActionsEnableGet
*
* DESCRIPTION:
*       Get the status of FDB actions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - actions are enabled
*                    GT_FALSE - actions are disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionsEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */
    GT_STATUS rc;       /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 1, &value);

    *enablePtr = BIT2BOOL_MAC(value);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbTrigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       actFinishedPtr  - GT_TRUE  - triggered action completed
*                         GT_FALSE - triggered action is not completed yet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           'action finished' only when action finished on all port groups.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrigActionStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *actFinishedPtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_STATUS rc;   /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actFinishedPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    /* check that all port groups are ready */
    rc = prvCpssDxChPortGroupBusyWait(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                regAddr,1,
                GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if(rc == GT_BAD_STATE)
    {
        rc = GT_OK;
        /* not all port groups has the bit with value 0 */
        *actFinishedPtr = GT_FALSE;
    }
    else
    {
        *actFinishedPtr = GT_TRUE;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerModeSet
*
* DESCRIPTION:
*       Sets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ACTION_MODE_ENT     mode
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    switch (mode)
    {
        case CPSS_ACT_AUTO_E:
            value = 0;
            break;
        case CPSS_ACT_TRIG_E:
            value = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 2, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerModeGet
*
* DESCRIPTION:
*       Gets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_MAC_ACTION_MODE_ENT     *modePtr
)
{
    GT_U32 regAddr;     /* register address             */
    GT_U32 value;       /* value to read from register  */
    GT_STATUS rc;       /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 2, 1, &value);

    *modePtr = (value == 1) ? CPSS_ACT_TRIG_E : CPSS_ACT_AUTO_E;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbStaticOfNonExistDevRemove
*
* DESCRIPTION:
*       enable the ability to delete static mac entries when the aging daemon in
*       PP encounter with entry registered on non-exist device in the
*       "device table" the PP removes it ---> this flag regard the static
*       entries
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
* INPUTS:
*       devNum       - device number
*       deleteStatic -  GT_TRUE - Action will delete static entries as well as
*                                 dynamic
*                       GT_FALSE - only dynamic entries will be deleted.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It's application responsibility to check the status of Aging Trigger by
*       busy wait (use cpssDxChBrgFdbTrigActionStatusGet API), in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticOfNonExistDevRemove
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     deleteStatic
)
{
    GT_U32      regAddr; /* register address */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* set action 2 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 28, 1,
                                   ((deleteStatic == GT_TRUE) ? 1 : 0));
}

/*******************************************************************************
* cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
*
* DESCRIPTION:
*       Get whether enabled/disabled the ability to delete static mac entries
*       when the aging daemon in PP encounter with entry registered on non-exist
*       device in the "device table".
*       entries
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       deleteStaticPtr -  GT_TRUE - Action deletes entries if there associated
*                                    device number don't exist (static etries
*                                                                  as well)
*                          GT_FALSE - Action don't delete entries if there
*                                     associated device number don't exist
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *deleteStaticPtr
)
{
    GT_U32      value;      /* hw value from register   */
    GT_U32      regAddr;    /* register address         */
    GT_STATUS   rc;         /* return code              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(deleteStaticPtr);

    /* set action 2 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 28, 1, &value);

    *deleteStaticPtr = BIT2BOOL_MAC(value);

    return rc;

}

/*******************************************************************************
* cpssDxChBrgFdbDropAuEnableSet
*
* DESCRIPTION:
*       Enable/Disable dropping the Address Update messages when the queue is
*       full .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - Drop the address update messages
*                 GT_FALSE - Do not drop the address update messages
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDropAuEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      data;       /* data to be written to register   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* get address of Address Update Queue Control register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;

    data = BOOL2BIT_MAC(enable);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 31, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbDropAuEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of dropping the Address Update messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - The address update messages are dropped
*                    GT_FALSE - The address update messages are not dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDropAuEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32      regAddr;    /* register address                 */
    GT_U32      data;       /* data to be written to register   */
    GT_STATUS   rc;         /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* get address of Address Update Queue Control register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 31, 1, &data);

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with trunk,
*       regardless of the device to wich they are associated.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*       ageOutAllDevOnTrunkEnable   - GT_TRUE : All entries associated with a
*                                     trunk, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     trunk on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnTrunkEnable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    data = (ageOutAllDevOnTrunkEnable == GT_TRUE) ? 1 : 0;

    /* Enable/Disable aging out of all entries associated with trunk */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 16, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with trunk, regardless of
*       the device to wich they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                      trunk, regardless of the device they
*                                      are associated with are aged out.
*                                      GT_FALSE: Only entries associated with a
*                                      trunk on the local device are aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnTrunkEnablePtr
)
{
    GT_U32      regAddr;    /* register address         */
    GT_U32      data;       /* reg sub field data       */
    GT_STATUS   rc;         /* return code              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ageOutAllDevOnTrunkEnablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 16, 1, &data);

    *ageOutAllDevOnTrunkEnablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with port,
*       regardless of the device to wich they are associated.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                       - device number
*       ageOutAllDevOnNonTrunkEnable - GT_TRUE : All entries associated with a
*                                     port, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     port on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnNonTrunkEnable
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    data = (ageOutAllDevOnNonTrunkEnable == GT_TRUE) ? 1 : 0;

    /* Enable/Disable aging out of all entries associated with trunk */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 17, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with port, regardless of
*       the device to wich they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnNonTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                         port, regardless of the device they
*                                         are associated with are aged out.
*                                         GT_FALSE: Only entries associated with a
*                                         port on the local device are aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnNonTrunkEnablePtr
)
{
    GT_U32      regAddr;    /* register address         */
    GT_U32      data;       /* reg sub field data       */
    GT_STATUS   rc;         /* return code              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(ageOutAllDevOnNonTrunkEnablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 17, 1, &data);

    *ageOutAllDevOnNonTrunkEnablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbActionTransplantDataSet
*
* DESCRIPTION:
*       Prepares the entry for transplanting (old and new interface parameters).
*       VLAN and VLAN mask for transplanting is set by
*       cpssDxChBrgFdbActionActiveVlanSet. Execution of transplanting is done
*       by cpssDxChBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       oldInterfacePtr - pointer to old Interface parameters
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters
*                         (it may be only port or trunk).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionTransplantDataSet
(
    IN GT_U8                    devNum,
    IN CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    IN CPSS_INTERFACE_INFO_STC  *newInterfacePtr
)
{
    GT_U32      action0RegAddr;    /* register address of Action0 Register  */
    GT_U32      action1RegAddr;    /* register address of Action1 Register  */
    GT_U32      data;              /* reg sub field data       */
    GT_U16      newPortTrunk;
    GT_U16      oldPortTrunk;
    GT_U8       newIsTrunk;
    GT_U8       oldIsTrunk;
    GT_U8       newDevNum = 0;
    GT_U8       oldDevNum = 0;
    GT_STATUS   rc;                /* return code              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(oldInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(newInterfacePtr);

    /* check old interface type and set old interface parameters accordingly */
    switch (oldInterfacePtr->type)
    {
        case CPSS_INTERFACE_PORT_E:
            oldIsTrunk = 0;
            oldPortTrunk = oldInterfacePtr->devPort.portNum;
            oldDevNum = oldInterfacePtr->devPort.devNum;
            /* check range of device number */
            if (oldDevNum >= BIT_5)
            {
                return GT_OUT_OF_RANGE;
            }

            break;

        case CPSS_INTERFACE_TRUNK_E:
            oldIsTrunk = 1;
            oldDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
            oldPortTrunk = oldInterfacePtr->trunkId;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* check range of portTrunk */
    if (oldPortTrunk >= BIT_7)
    {
        return GT_OUT_OF_RANGE;
    }


    /* check new interface type and set new interface parameters accordingly */
    switch (newInterfacePtr->type)
    {
        case CPSS_INTERFACE_PORT_E:
            newIsTrunk = 0;
            newPortTrunk = newInterfacePtr->devPort.portNum;
            newDevNum = newInterfacePtr->devPort.devNum;
            /* check range of device number */
            if (newDevNum >= BIT_5)
            {
                return GT_OUT_OF_RANGE;
            }
            break;

        case CPSS_INTERFACE_TRUNK_E:
            newIsTrunk = 1;
            newDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
            newPortTrunk = newInterfacePtr->trunkId;
            break;

        default:
            return GT_BAD_PARAM;
    }

    /* check range of portTrunk */
    if (newPortTrunk >= BIT_7)
    {
        return GT_OUT_OF_RANGE;
    }


    /* get Action0 and Action1 registers addresses */
    action0RegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                bridgeRegs.macTblAction0;
    action1RegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                bridgeRegs.macTblAction1;
    /* set data for Action0 register */
    data = (oldPortTrunk | (oldDevNum << 7) | (newPortTrunk << 12));

    rc = prvCpssDrvHwPpSetRegField(devNum, action0RegAddr, 13, 19, data);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set data for Action1 register */
    data = (newDevNum | (newIsTrunk << 5) | (oldIsTrunk << 6));

    return prvCpssDrvHwPpSetRegField(devNum, action1RegAddr, 0, 7, data);
}

/*******************************************************************************
* cpssDxChBrgFdbActionTransplantDataGet
*
* DESCRIPTION:
*       Get transplant data: old interface parameters and the new ones.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*
*
* OUTPUTS:
*       oldInterfacePtr - pointer to old Interface parameters.
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters.
*                         (it may be only port or trunk).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionTransplantDataGet
(
    IN  GT_U8                    devNum,
    OUT CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    OUT CPSS_INTERFACE_INFO_STC  *newInterfacePtr
)
{
    GT_U32      action0RegAddr;    /* register address of Action0 Register  */
    GT_U32      action1RegAddr;    /* register address of Action1 Register  */
    GT_U32      data;              /* reg sub field data       */
    GT_U32      newPortTrunk;
    GT_U32      oldPortTrunk;
    GT_U32      newIsTrunk;
    GT_U32      oldIsTrunk;
    GT_U8       newDevNum = 0;
    GT_U8       oldDevNum = 0;
    GT_STATUS   rc;                /* return code              */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(oldInterfacePtr);
    CPSS_NULL_PTR_CHECK_MAC(newInterfacePtr);

    /* get Action0 and Action1 registers addresses */
    action0RegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                bridgeRegs.macTblAction0;
    action1RegAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                                bridgeRegs.macTblAction1;

    /* get data from Action0 register */
    rc = prvCpssDrvHwPpGetRegField(devNum, action0RegAddr, 13, 19, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* parse data from Action0 register */
    oldPortTrunk = (data & 0x7F);
    oldDevNum = (GT_U8)((data >> 7) & 0x1F);
    newPortTrunk = ((data >> 12) & 0x7F);

    /* get data from Action1 register */
    rc = prvCpssDrvHwPpGetRegField(devNum, action1RegAddr, 0, 7, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* parse data from Action0 register */
    newDevNum = (GT_U8)(data & 0x1F);
    newIsTrunk = ((data >> 5) & 0x1);
    oldIsTrunk = ((data >> 6) & 0x1);

    /* check new interface type */
    if (newIsTrunk)
    {
        /* Trunk */
        newInterfacePtr->trunkId = (GT_TRUNK_ID)newPortTrunk;
        newInterfacePtr->type = CPSS_INTERFACE_TRUNK_E;
    }
    else
    {
        /* Port */
        newInterfacePtr->devPort.devNum = newDevNum;
        newInterfacePtr->devPort.portNum = (GT_U8)newPortTrunk;
        newInterfacePtr->type = CPSS_INTERFACE_PORT_E;
    }

    /* check old interface type */
    if (oldIsTrunk)
    {
        /* Trunk */
        oldInterfacePtr->trunkId = (GT_TRUNK_ID)oldPortTrunk;
        oldInterfacePtr->type = CPSS_INTERFACE_TRUNK_E;
    }
    else
    {
        /* Port */
        oldInterfacePtr->devPort.devNum = oldDevNum;
        oldInterfacePtr->devPort.portNum = (GT_U8)oldPortTrunk;
        oldInterfacePtr->type = CPSS_INTERFACE_PORT_E;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function returns AU message processing completion and success status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       completedPtr - pointer to AU message processing completion.
*                      GT_TRUE - AU message was processed by PP.
*                      GT_FALSE - AU message processing is not completed yet by PP.
*       succeededPtr - pointer to a success of AU operation.
*                      GT_TRUE  - the AU action succeeded.
*                      GT_FALSE - the AU action has failed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DXCH devices the succeed status doesn't relevant for query
*       address messages (message type = CPSS_QA_E) -  FEr#93.
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbFromCpuAuMsgStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *completedPtr,
    OUT GT_BOOL  *succeededPtr
)
{
    GT_STATUS   rc;             /* return code                  */
    GT_U32      regAddr;        /* register address             */
    GT_U32      hwData;         /* data to read from register   */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(completedPtr);
    CPSS_NULL_PTR_CHECK_MAC(succeededPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    globalRegs.addrUpdateControlReg;

    /* check if the message action is completed, read Message from CPU
       Management register */
    /* check that all port groups are ready */
    rc = prvCpssDxChPortGroupBusyWait(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                regAddr,0,
                GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if(rc == GT_BAD_STATE)
    {
        /* not all port groups has the bit with value 0 */
        *completedPtr = GT_FALSE;

        return GT_OK;
    }
    else if (rc != GT_OK)
    {
        return rc;
    }

    /* the action is completed */
    *completedPtr = GT_TRUE;

    /* loop on all active port groups */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /* get the succeeded bit */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, 1, 1, &hwData);
        if(rc != GT_OK)
        {
            return rc;
        }

    /* check if the action has succeeded */
        if(hwData == 0)
        {
            /* at least one port group not succeeded */
            *succeededPtr = GT_FALSE;
            return GT_OK;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    /* all port groups succeeded */
    *succeededPtr = GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveVlanSet
*
* DESCRIPTION:
*       Set action active vlan and vlan mask.
*       All actions will be taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan Id
*       vlanMask    - vlan mask filter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveVlanSet
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   vlanMask
)
{
    GT_U32      regAddr;        /* register address             */
    GT_U32      hwData;         /* data to read from register   */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* check ranges */
    if ((vlanId >= BIT_12) || (vlanMask >= BIT_12))
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction1;

    hwData = (vlanId | (vlanMask << 12));

    /* set Active Vlan and Active Vlan Mask in FDB Action1 register */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 8, 24, hwData);
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveVlanGet
*
* DESCRIPTION:
*       Get action active vlan and vlan mask.
*       All actions are taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       vlanIdPtr      - pointer to Vlan Id
*       vlanMaskPtr    - pointer to vlan mask filter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveVlanGet
(
    IN  GT_U8    devNum,
    OUT GT_U16   *vlanIdPtr,
    OUT GT_U16   *vlanMaskPtr
)
{
    GT_U32      regAddr;        /* register address             */
    GT_U32      hwData;         /* data to read from register   */
    GT_STATUS   rc;             /* return code                  */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(vlanIdPtr);
    CPSS_NULL_PTR_CHECK_MAC(vlanMaskPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction1;

    /* get Active Vlan and Vlan Mask from FDB Action1 Register */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 8, 24, &hwData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *vlanIdPtr = (GT_U16)(hwData & 0xFFF);
    *vlanMaskPtr = (GT_U16)((hwData >> 12) & 0xFFF);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveDevSet
*
* DESCRIPTION:
*       Set the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       actDev      - Action active device (0..31)
*       actDevMask  - Action active mask (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - actDev > 31 or actDevMask > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveDevSet
(
    IN GT_U8    devNum,
    IN GT_U32   actDev,
    IN GT_U32   actDevMask
)
{
    GT_U32    regAddr;           /* register address */
    GT_U32    data;              /* reg sub field data */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    if(actDevMask >= BIT_5 || actDev >= BIT_5)
    {
        return GT_OUT_OF_RANGE;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    data = (actDevMask << 5) | actDev ;

    /* Set the Active device number and active device number mask */
    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 18, 10, data);
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveDevGet
*
* DESCRIPTION:
*       Get the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       actDevPtr      - pointer to action active device (0..31)
*       actDevMaskPtr  - pointer to action active mask (0..31)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - actDev > 31 or actDevMask > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveDevGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *actDevPtr,
    OUT GT_U32   *actDevMaskPtr
)
{
    GT_U32    regAddr;          /* register address     */
    GT_U32    data;             /* reg sub field data   */
    GT_STATUS rc;               /* return code          */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actDevPtr);
    CPSS_NULL_PTR_CHECK_MAC(actDevMaskPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    /* get Active Dev and Dev Mask from FDB Action1 Register */
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 18, 10, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *actDevPtr = (data & 0x1F);
    *actDevMaskPtr = ((data >> 5) & 0x1F);

    return GT_OK;

}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveInterfaceSet
*
* DESCRIPTION:
*       Set the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of : Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*       A FDB entry will be treated by the FDB action only if the following
*       cases are true:
*       1. The interface is trunk and "active trunk mask" is set and
*       "associated trunkId" masked by the "active trunk/port mask" equal to
*       "Active trunkId".
*       2. The interface is port and "associated portNumber" masked by the
*       "active trunk/port mask" equal to "Active portNumber".
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       actIsTrunk       - determines if the interface is port or trunk
*       actIsTrunkMask   - action active trunk mask.
*       actTrunkPort     - action active interface (port/trunk)
*       actTrunkPortMask - action active port/trunk mask.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To enable Trunk-ID filter set actIsTrunk and actIsTrunkMask to 0x1,
*       set actTrunkPort to trunkID and actTrunkPortMask to 0x7F. To disable
*       Trunk-ID filter set all those parameters to 0x0.
*       To enable Port/device filter set actIsTrunk to 0x0, actTrunkPort to
*       portNum, actTrunkPortMask to 0x7F, actDev to device number and
*       actDevMask to 0x1F (all ones) by cpssDxChBrgFdbActionActiveDevSet.
*       To disable port/device filter set all those parameters to 0x0.
*
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveInterfaceSet
(
    IN GT_U8   devNum,
    IN GT_U32  actIsTrunk,
    IN GT_U32  actIsTrunkMask,
    IN GT_U32  actTrunkPort,
    IN GT_U32  actTrunkPortMask
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* check ranges according to maximal range */
    if ((actTrunkPort >= BIT_7) ||
        (actTrunkPortMask >= BIT_7))
    {
        return GT_OUT_OF_RANGE;
    }
    /* set data */
    data = (actTrunkPort | (actTrunkPortMask << 7) |
            ((actIsTrunk & 0x1) << 14) | ((actIsTrunkMask & 0x1) << 15));

    /* Get address of FDB Action2 register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 0, 16, data);
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveInterfaceGet
*
* DESCRIPTION:
*       Get the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of: Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       actIsTrunkPtr       - determines if the interface is port or trunk
*       actIsTrunkMaskPtr   - action active trunk mask.
*       actTrunkPortPtr     - action active interface (port/trunk)
*       actTrunkPortMaskPtr - action active port/trunk mask.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveInterfaceGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *actIsTrunkPtr,
    OUT GT_U32  *actIsTrunkMaskPtr,
    OUT GT_U32  *actTrunkPortPtr,
    OUT GT_U32  *actTrunkPortMaskPtr
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */
    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(actIsTrunkPtr);
    CPSS_NULL_PTR_CHECK_MAC(actIsTrunkMaskPtr);
    CPSS_NULL_PTR_CHECK_MAC(actTrunkPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(actTrunkPortMaskPtr);


    /* Get address of FDB Action2 register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 0, 16, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    *actTrunkPortPtr = (data & 0x7F);
    *actTrunkPortMaskPtr = ((data >> 7) & 0x7F);
    *actIsTrunkPtr = ((data >> 14) & 0x1);
    *actIsTrunkMaskPtr = ((data >> 15) & 0x1);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbUploadEnableSet
*
* DESCRIPTION:
*       Enable/Disable reading FDB entries via AU messages to the CPU.
*       The API only configures mode of triggered action.
*       To execute upload use the cpssDxChBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE - enable FDB Upload
*                  GT_FALSE - disable FDB upload
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUploadEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS   rc;
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    /* the FDB upload occurs on all port groups */
    portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    /* Get address of FDB Action2 register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    data = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* resolve the 'Messages filter' state */
        rc = multiPortGroupCheckAndMessageFilterResolve(devNum,
            GT_FALSE,/* DA refresh NOT given */
            GT_FALSE,/* DA refresh value -- not relevant due to "DA refresh NOT given" */
            GT_TRUE,/* FDB upload given */
            enable,/* FDB upload value */
            GT_FALSE, /* action mode NOT given */
            0);      /* action mode value -- not relevant due to "action mode NOT given" */
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, 30, 1, data);
}

/*******************************************************************************
* cpssDxChBrgFdbUploadEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of uploading FDB entries via AU messages to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - FDB Upload is enabled
*                     GT_FALSE - FDB upload is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUploadEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */
    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* Get address of FDB Action2 register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction2;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 30, 1, &data);

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* fdbTrigAction
*
* DESCRIPTION:
*       trigger FDB action.
*       as part of WA for multi-port groups device , the trigger need to be done
*       only on specific port groups.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
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
*
*       caller must set bit 1 to value 1 (triggering the action)
*
*******************************************************************************/
static GT_STATUS fdbTrigAction
(
    IN GT_U8 devNum,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 fieldData
)
{
    GT_STATUS rc;                   /* return code                        */
    GT_U32    regAddr;              /* register address                   */
    GT_U32  portGroupId;/* the port group Id - support multi-port-groups device */
    GT_BOOL  multiPortGroupWaNeeded = GT_FALSE;/* indication that WA needed for multi port group device */

    /* Get address of FDB Action0 register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* loop on all port group */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needWaWaitForReTriggering == GT_TRUE)
            {
                /* at least one port group started the WA , and needs the 're-triggering' */
                multiPortGroupWaNeeded = GT_TRUE;
                break;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    }

    if(multiPortGroupWaNeeded == GT_FALSE)
    {
        /* non-multi port groups device , or this multi-port groups device not
           doing 're-trigger' of the action as part of 'WA for AUQ stuck'*/


        /* trigger the action on the device (for multi-port group device --> all port groups) */
        return prvCpssDrvHwPpSetRegField(devNum, regAddr, fieldOffset, fieldLength, fieldData);
    }

    /*************************************************************/
    /* we deal with multi-port groups device that not all port   */
    /* groups need to re-trigger the action                      */
    /*************************************************************/

    /* loop on all port group */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /* check if current port group need to be skipped */
        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needWaWaitForReTriggering == GT_FALSE)
        {
            /* skip this port group -- the action not need re-trigger */
            continue;
        }

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, fieldOffset, fieldLength, fieldData);
        if(rc != GT_OK)
        {
            return rc;
        }


        debugPrint(("fdbTrigAction : trigger on portGroupId[%d] \n",portGroupId));

        /* clear the flag , it can be set again only from function cpssDxChBrgFdbTriggerAuQueueWa(...) */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needWaWaitForReTriggering = GT_FALSE;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* trigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - the previous action not done
*
* COMMENTS:
*       for multi-port groups device :
*           'action finished' only when action finished on all port groups.
*
*******************************************************************************/
static GT_STATUS trigActionStatusGet
(
    IN  GT_U8    devNum
)
{
    GT_STATUS rc;   /* return code                  */
    GT_PORT_GROUPS_BMP  actFinishedPortGroupsBmp; /*bitmap of port groups on which 'trigger action' processing completed*/
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_BOOL startTrigger = GT_FALSE;

    /* replace call to cpssDxChBrgFdbTrigActionStatusGet ... to support WA
       for AUQ full issue ... see related API cpssDxChBrgFdbTriggerAuQueueWa
       check which of the port groups not finished the processing of the action */
    rc = cpssDxChBrgFdbPortGroupTrigActionStatusGet(devNum,
                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                &actFinishedPortGroupsBmp);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check that the previous action has completed */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(actFinishedPortGroupsBmp & (1<<(portGroupId)))
        {
            /* the port group is ready -- clear the flag of <notReady> */
            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].notReady = GT_FALSE;

            /* there is at least one port group that need the triggering */
            startTrigger = GT_TRUE;
            continue;
        }

        /* there maybe some port groups that the previous action not done, But
           we need to ignore the port groups that are skipped as part of the WA
           that may have started ---> so check <notReady> */
        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].notReady == GT_TRUE)
        {
            /* the WA started for the device but this port group was not ready so it was skipped */
            debugPrint(("trigActionStatusGet : 'not ready' portGroupId[%d] \n",portGroupId));
        }
        else
        {
            /*this port group just not finished the previous action*/
            debugPrint((" GT_BAD_STATE : file[%s] line[%d] \n",__FILE__,__LINE__));
            return GT_BAD_STATE;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    if(startTrigger == GT_FALSE)
    {
        debugPrint((" GT_BAD_STATE : file[%s] line[%d] \n",__FILE__,__LINE__));
        /* there are NO port groups that need the triggering */
        return GT_BAD_STATE;
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbTrigActionStart
*
* DESCRIPTION:
*       Enable actions, sets action type, action mode to CPSS_ACT_TRIG_E and
*       starts triggered action by setting Aging Trigger.
*       This API may be used to start one of triggered actions: Aging, Deleting,
*       Transplanting and FDB Upload.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE    - when previous FDB triggered action is not completed yet
*                         or CNC block upload not finished (or not all of it's
*                         results retrieved from the common used FU and CNC
*                         upload queue)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Deleting and Transplanting actions can be done only by CPU triggered
*       FDB action that activated by the function.
*       See below sequence of CPSS API calls for Deleting and
*       Transplanting actions.
*
*       Before making Delete or Transplant sequence the application need to
*       disable Actions to avoid wrong automatic aging.
*
*       It is Application responsibility to get and store all parameters
*       that are changed during those actions.
*       1. AA message to CPU status by cpssDxChBrgFdbAAandTAToCpuGet.
*       2. Action Active configuration by
*          cpssDxChBrgFdbActionActiveInterfaceGet,
*          cpssDxChBrgFdbActionActiveDevGet,
*          cpssDxChBrgFdbActionActiveVlanGet.
*       3. Action mode and trigger mode by cpssDxChBrgFdbActionModeGet
*          cpssDxChBrgFdbActionTriggerModeGet.
*
*       The AA and TA messages may be disabled before the FDB action
*       4. Disable AA and TA messages to CPU by cpssDxChBrgFdbAAandTAToCpuSet.
*
*       5. Set Active configuration by: cpssDxChBrgFdbActionActiveInterfaceSet,
*       cpssDxChBrgFdbActionActiveVlanSet and cpssDxChBrgFdbActionActiveDevSet.
*
*       6. Start triggered action by cpssDxChBrgFdbTrigActionStart
*
*       7. Wait that triggered action is completed by:
*           -  Busy-wait poling of status - cpssDxChBrgFdbTrigActionStatusGet
*           -  Wait of event CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E.
*              This wait may be done in context of dedicated task to restore
*              Active configuration and AA messages configuration.
*
*   for multi-port groups device :
*           the trigger is done on ALL port groups regardless to operation mode
*           (Trigger aging/transplant/delete/upload).
*           see also description of function cpssDxChBrgFdbActionModeSet about
*           'multi-port groups device'.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrigActionStart
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
)
{
    GT_STATUS rc;                   /* return code                        */
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */
    GT_BOOL   uploadEnable;         /* FU upload Enable                   */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* check if previous trigger action finished */
    rc = trigActionStatusGet(devNum);
    if (rc != GT_OK)
    {
        /*GT_BAD_STATE             - the previous action not done*/
        return rc;
    }

    if ((PRV_CPSS_DXCH3_FAMILY_CHECK_MAC(devNum))/* devices with CNC support */
        && (mode == CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E))
    {
        rc = cpssDxChBrgFdbUploadEnableGet(devNum, &uploadEnable);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(uploadEnable == GT_TRUE)
        {
            /* loop on all port groups to check that all CNC upload already read */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
            {
            /* there are CNC upload messages in the queue */
                if (PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId].unreadCncCounters != 0)
            {
                    debugPrint((" GT_BAD_STATE : file[%s] line[%d] \n",__FILE__,__LINE__));
                return GT_BAD_STATE;
            }
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

            /* Need not set SelCNCOrFU for xCat2 and above.
               The default value is OK and is not changed by CNC. */
            if (!PRV_CPSS_DXCH_XCAT2_FAMILY_CHECK_MAC(devNum))
            {
            /* set FDB to be the Queue owner */
            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.globalControl;
            rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 14, 1, 1);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }
    }

    /* let the 'action mode set' to set bits 4,5 and to extra logic the needed */
    rc  = cpssDxChBrgFdbActionModeSet(devNum,mode);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* enable actions, set triggerMode = TRIGGER and set AgingTriger bit */
    data = 0x7;

    return fdbTrigAction(devNum,
                               0,/*start bit 0 */
                               3,/* 3 bits */
                               data);/* value of the 3 bits */
}

/*******************************************************************************
* cpssDxChBrgFdbActionModeSet
*
* DESCRIPTION:
*       Sets FDB action mode without setting Action Trigger
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - FDB action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*       set configuration to all portGroups with same value.
*       1. Age with delete (may be Triggered or Auto)
*           a. Unified FDBs mode and Unified-Linked FDB mode :
*              This type of operation is not supported , due to the control
*              learning being used, the risk of AA messages being dropped & the
*              need to avoid dropping MAC entries from portGroups which are not
*              the source of these entries (meaning they would never be refreshed)
*              API returns GT_BAD_STATE if trying to use this action mode.
*           b. Linked FDBs mode :
*              Application should use this type of operation only in Automatic mode
*              (not in Controlled mode) + Static FDB entries .
*              In this mode automatic learnt addresses would be silently removed
*              by PP while application addresses would be removed only by specific
*              application API call.
*
*       2. Age without delete (may be Triggered or Auto)
*           The only type of Aging that can be used with Unified FDBs mode and
*           Unified-Linked FDB mode (in Linked FDBs mode it should be used when
*           using controlled mode), AA would be sent by PP to Application that
*           would remove entry from relevant portGroups.
*
*       3. Transplant (only Triggered)
*           a. Unified-Linked FDB mode and Linked FDB mode :
*               This action mode is not supported - API returns GT_BAD_STATE
*               because triggering this would possibly result with entries which
*               have their new 'source' information properly set - but these entries
*               would not be relocated to the new source portGroups, meaning that
*               packet received from new station location (which was supposed to
*               be transplanted), may still create NA message.
*               Computing new source portGroup is not always possible, especially
*               when station is relocated to trunk or it was previously residing
*               on multiple-portGroups.
*               Application is required to perform Flush operation instead.
*               This would clear previously learnt MAC
*
*       4. Delete (only Triggered)
*           supported in all FDB modes
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
)
{
    GT_STATUS   rc;
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* Get address of FDB Action0 register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    /* check mode */
    switch (mode)
    {
        case CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E:
            data = 0;
            break;
        case CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E:
            data = 1;
            break;
        case CPSS_FDB_ACTION_DELETING_E:
            data = 2;
            break;
        case CPSS_FDB_ACTION_TRANSPLANTING_E:
            data = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* check FBD state with limitations on action mode */
        /* resolve the 'Messages filter' state */
        rc = multiPortGroupCheckAndMessageFilterResolve(devNum,
            GT_FALSE,/* DA refresh NOT given */
            GT_FALSE,/* DA refresh value -- not relevant due to "DA refresh NOT given" */
            GT_FALSE,/* FDB upload NOT given */
            GT_FALSE,/* FDB upload value -- not relevant due to "FDB upload NOT given" */
            GT_TRUE, /* action mode given */
            mode);   /* action mode value */
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 2, data);
}

/*******************************************************************************
* cpssDxChBrgFdbActionModeGet
*
* DESCRIPTION:
*       Gets FDB action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr    - FDB action mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_FDB_ACTION_MODE_ENT     *modePtr
)
{
    GT_U32    regAddr;              /* register address                   */
    GT_U32    data;                 /* reg sub field data                 */
    GT_STATUS rc;                   /* return code                        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* Get address of FDB Action0 register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 3, 2, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* check mode */
    switch (data)
    {
        case 0:
            *modePtr = CPSS_FDB_ACTION_AGE_WITH_REMOVAL_E;
            break;
        case 1:
            *modePtr = CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E;
            break;
        case 2:
            *modePtr = CPSS_FDB_ACTION_DELETING_E;
            break;
        case 3:
            *modePtr = CPSS_FDB_ACTION_TRANSPLANTING_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerToggle
*
* DESCRIPTION:
*       Toggle Aging Trigger and cause the device to scan its MAC address table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - the previous action not done
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*       the trigger is done on ALL port groups regardless to operation mode
*       (Trigger aging/transplant/delete/upload).
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerToggle
(
    IN GT_U8  devNum
)
{
    GT_STATUS   rc;/*return code*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* check if previous trigger action finished */
    rc = trigActionStatusGet(devNum);
    if (rc != GT_OK)
    {
        /*GT_BAD_STATE             - the previous action not done*/
        return rc;
    }

    /* set self-clear trigger bit */
    return fdbTrigAction(devNum,
                               1,/*start bit 1 */
                               1,/* 1 bit */
                               1);/* value of the bit */
}

/*******************************************************************************
* cpssDxChBrgFdbTrunkAgingModeSet
*
* DESCRIPTION:
*       Sets bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       trunkAgingMode - FDB aging mode for trunk entries.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E:
*                 In Controlled Aging mode, the device ages Trunk entries
*                 whether it is the owner or not. When a Trunk entry is aged,
*                 it is not deleted from the MAC table, but an Aged
*                 Address (AA) message is sent to the CPU. This allows
*                 the CPU to track whether the MAC has been aged on all
*                 devices that have port members for the Trunk. Only then
*                 the CPU explicitly deletes the Trunk entry on all
*                 devices.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E:
*                 This is Automatic Trunk Entry Aging mode, which treats
*                 Trunk entries in the MAC table just like non-Trunk entries.
*                 That is, aging is performed only on entries owned by the
*                 device. The advantage of this mode is that it does not
*                 require any CPU intervention. This mode has the
*                 disadvantage of possibly aging a Trunk entry on all devices
*                 when there still may be active traffic for the MAC on a
*                 different device, where packets were sent over another
*                 link of the Trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or aging mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrunkAgingModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_AGE_TRUNK_MODE_ENT  trunkAgingMode
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

     switch (trunkAgingMode)
    {
        case CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E:
            value = 0;
            break;
        case CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E:
            value = 1;
            break;
        default:
            return GT_BAD_PARAM;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 8, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbTrunkAgingModeGet
*
* DESCRIPTION:
*       Get bridge FDB Aging Mode for trunk entries.
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
*       trunkAgingModePtr - (pointer to) FDB aging mode for trunk entries.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E:
*                 In Controlled Aging mode, the device ages Trunk entries
*                 whether it is the owner or not. When a Trunk entry is aged,
*                 it is not deleted from the MAC table, but an Aged
*                 Address (AA) message is sent to the CPU. This allows
*                 the CPU to track whether the MAC has been aged on all
*                 devices that have port members for the Trunk. Only then
*                 the CPU explicitly deletes the Trunk entry on all
*                 devices.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E:
*                 This is Automatic Trunk Entry Aging mode, which treats
*                 Trunk entries in the MAC table just like non-Trunk entries.
*                 That is, aging is performed only on entries owned by the
*                 device. The advantage of this mode is that it does not
*                 require any CPU intervention. This mode has the
*                 disadvantage of possibly aging a Trunk entry on all devices
*                 when there still may be active traffic for the MAC on a
*                 different device, where packets were sent over another
*                 link of the Trunk.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or aging mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrunkAgingModeGet
(
    IN GT_U8                        devNum,
    OUT CPSS_FDB_AGE_TRUNK_MODE_ENT *trunkAgingModePtr
)
{
    GT_U32      regAddr;    /* register address */
    GT_U32      value;      /* value to read from register */
    GT_STATUS   rc;         /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(trunkAgingModePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macControlReg;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 8, 1, &value);

    *trunkAgingModePtr = (value == 0)
            ? CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E
            : CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E;

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbInit
*
* DESCRIPTION:
*       Init FDB system facility for a device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - on not supported FDB table size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbInit
(
    IN  GT_U8   devNum
)
{
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    /* Init FDB hash parameters */
    switch(PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb)
    {
        case _8K:
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.size =
                    PRV_CPSS_DXCH_MAC_TBL_SIZE_8K_E;
            break;
        case _16K:
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.size =
                    PRV_CPSS_DXCH_MAC_TBL_SIZE_16K_E;
            break;
        case _32K:
            PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.size =
                    PRV_CPSS_DXCH_MAC_TBL_SIZE_32K_E;
            break;
        default:
            return GT_NOT_SUPPORTED;
    }

    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.vlanMode =
                                                        CPSS_SVL_E;
    PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode =
                                            CPSS_MAC_HASH_FUNC_CRC_E;

    PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode =
        PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_NON_MULTI_PORT_GROUP_DEVICE_E;

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /****************************/
        /* multi port groups device */
        /****************************/

        /* initialize the mask state --> enabled */
        rc = prvCpssDxChBrgFdbAuFuMessageToCpuOnNonLocalMaskEnableSet(devNum,GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }

        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode =
            PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbAgeBitDaRefreshEnableSet
*
* DESCRIPTION:
*       Enables/disables destination address-based aging. When this bit is
*       set, the aging process is done both on the source and the destination
*       address (i.e. the age bit will be refresh when MAC DA hit occurs, as
*       well as MAC SA hit).
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable refreshing
*                 GT_FALSE - disable refreshing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*       for multi-port groups device:
*           Once enabled, MAC address is eligibly to be aged-out if no packets were
*           received from it or sent to it from any other location during aging period.
*           In order to properly compute this application is required to keep a
*           'score board' for every regular entry (same as needed for Trunk entries),
*           each bit representing age bit from each underlying portGroup which this
*           entry is written to. Entry is aged-out if all relevant portGroups have
*           sent AA, indicating entry was aged-out (once DA refresh is disabled -
*           only the source portGroup indication interests us for aging calculations).
*           Note:
*           that it's highly recommended not to enable this feature, as this
*           feature enables transmissions of many AA messages to the CPU
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeBitDaRefreshEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
)
{
    GT_STATUS   rc;     /* return code*/
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[2];

    value = BOOL2BIT_MAC(enable);

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        /* resolve the 'Messages filter' state */
        rc = multiPortGroupCheckAndMessageFilterResolve(devNum,
            GT_TRUE,/* DA refresh given */
            enable,/* DA refresh value */
            GT_TRUE,/* FDB upload NOT given */
            GT_FALSE,/* FDB upload value --  not relevant due to "FDB upload NOT given"*/
            GT_FALSE, /* action mode NOT given */
            0);      /* action mode value -- not relevant due to "action mode NOT given" */
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 11, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbAgeBitDaRefreshEnableGet
*
* DESCRIPTION:
*       Gets status (enabled/disabled) of destination address-based aging bit.
*       When this bit is set, the aging process is done both on the source and
*       the destination address (i.e. the age bit will be refresh when MAC DA
*       hit occurs, as well as MAC SA hit).
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) status DA refreshing of aged bit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeBitDaRefreshEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to read from register */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[2];

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 11, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(value);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutedLearningEnableSet
*
* DESCRIPTION:
*       Enable or disable learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet's MAC SA to the router's MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers Source Addresses.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutedLearningEnableSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to write to register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);


    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.bridgeGlobalConfigRegArray[0];

    /* HW: 0 - enabled, 1 - disabled */
    value = (enable == GT_FALSE) ? 1 : 0 ;

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 3, 1, value);
}

/*******************************************************************************
* cpssDxChBrgFdbRoutedLearningEnableGet
*
* DESCRIPTION:
*       Get Enable or Disable state of learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet's MAC SA to the router's MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers Source Addresses.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutedLearningEnableGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
)
{
    GT_U32 regAddr;     /* register address */
    GT_U32 value;       /* value to read from register */
    GT_STATUS rc;       /* returned status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
        bridgeRegs.bridgeGlobalConfigRegArray[0];

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 3, 1, &value);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* HW: 0 - enabled, 1 - disabled */
    *enablePtr = (value == 0) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* fdbTriggerAuQueueWaCheckAndClear
*
* DESCRIPTION:
*       check if for current port group the WA needed , and clear the trigger
*       action bit for the needing port group :
*       Trigger the Address Update (AU) queue workaround for case when learning
*       from CPU is stopped due to full Address Update (AU) fifo queue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*       portGroupId - the portGroupId. to support multi-port-groups device
*
* OUTPUTS:
*       waNeededPtr - (pointer to) does the WA needed for this port group
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or  portGroupId
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS fdbTriggerAuQueueWaCheckAndClear
(
    IN   GT_U8 devNum,
    IN   GT_U32  portGroupId,
    OUT  GT_BOOL *waNeededPtr
)
{
    GT_U32                      regAddr;       /* register address */
    GT_STATUS                   rc;
    GT_U32                      regValue;

    *waNeededPtr = GT_FALSE;

    PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activated = GT_FALSE;

    /* get the address of FDB Scanning0 register */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    /* check if this port group requires the WA */
    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 1, 1,&regValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(regValue == 0)
    {
        /* the action is not 'stuck' on this port group */
        return GT_OK;
    }

    /* clear the trigger bit --> allow the Application to trigger that action
       again */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId , regAddr, 1, 1, 0);
    if(rc != GT_OK)
    {
        return rc;
    }

    *waNeededPtr = GT_TRUE;

    /* NOTE: the flag of needWaWaitForReTriggering is cleared only from fdbTrigAction */
    PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needWaWaitForReTriggering = GT_TRUE;

    PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activated = GT_TRUE;

    return GT_OK;
}


/*******************************************************************************
* fdbTriggerAuQueueWaCheck
*
* DESCRIPTION:
*       check if the device can start the WA .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       waNeededPortGroupsBmpPtr - (pointer to) bmp of port groups that need the WA.
*
*       waNotReadyPortGroupsBmpPtr - (pointer to) bmp of port groups that action
*                                   not finished but the AUQ not full so those port
*                                   groups NOT ready for the WA !
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS fdbTriggerAuQueueWaCheck
(
    IN   GT_U8 devNum,
    OUT  GT_PORT_GROUPS_BMP  *waNeededPortGroupsBmpPtr,
    OUT  GT_PORT_GROUPS_BMP  *waNotReadyPortGroupsBmpPtr

)
{
    GT_STATUS           rc;
    GT_PORT_GROUPS_BMP  actFinishedPortGroupsBmp; /*bitmap of port groups on which 'trigger action' processing completed*/
    GT_PORT_GROUPS_BMP  isFullPortGroupsBmp; /*bitmap of port groups on which 'the queue is full'*/
    GT_PORT_GROUPS_BMP  actNotFinishedPortGroupsBmp; /*bitmap of port groups on which 'trigger action' processing NOT completed*/

    *waNotReadyPortGroupsBmpPtr = 0;
    /* when the AUQ is not full and pp not finished process the action , it may
       still be due to 'task priority' issues inside the PP (heavy traffic processing) */

    /* check which of the port groups not finished the processing of the action */
    rc = cpssDxChBrgFdbPortGroupTrigActionStatusGet(devNum,
                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                &actFinishedPortGroupsBmp);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* get the port groups that not finished the trigger action */
    actNotFinishedPortGroupsBmp = (~actFinishedPortGroupsBmp) &
        PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp;

    *waNeededPortGroupsBmpPtr = actNotFinishedPortGroupsBmp;

    if(prvCpssAuqDeadlockOnQueueNotFullWaEnable == GT_TRUE)
    {
        /* for debug purposes only ! */
        return GT_OK;
    }

    /* check that the AUQ is FULL (part of CQ#124401) */
    /* check which of the port groups has 'queue full' */
    rc = cpssDxChBrgFdbPortGroupQueueFullGet(devNum,
                CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                CPSS_DXCH_FDB_QUEUE_TYPE_AU_E,
                &isFullPortGroupsBmp);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* WA needed only on port groups which reached 'AUQ full' (and not finished the trigger action) */
    *waNeededPortGroupsBmpPtr &= isFullPortGroupsBmp;

    /* from port groups on which 'trigger action' processing NOT completed remove those with AUQ full.
       which leave us with :
       port groups that action not finished but the AUQ not full so those port groups NOT ready for the WA !
    */
    *waNotReadyPortGroupsBmpPtr = actNotFinishedPortGroupsBmp & ~(isFullPortGroupsBmp);

    return GT_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbTriggerAuQueueWa
*
* DESCRIPTION:
*       Trigger the Address Update (AU) queue workaround for case when learning
*       from CPU is stopped due to full Address Update (AU) fifo queue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_STATE             - the secondary AUQ not reached the end of
*                                  queue , meaning no need to set the secondary
*                                  base address again.
*                                  this protect the SW from losing unread messages
*                                  in the secondary queue (otherwise the PP may
*                                  override them with new messages).
*                                  caller may use function
*                                  cpssDxChBrgFdbSecondaryAuMsgBlockGet(...)
*                                  to get messages from the secondary queue.
*       GT_NOT_SUPPORTED         - the device not need / not support the WA
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTriggerAuQueueWa
(
    IN  GT_U8 devNum
)
{
    PRV_CPSS_AU_DESC_CTRL_STC   *descCtrlWaPtr;  /* pointer to the WA descriptors DB
                                                      of the device */
    GT_U32                      phyAddr;       /* phy address */
    GT_U32                      regAddr;       /* register address */
    GT_STATUS                   rc;
    GT_U32                      regValue;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_BOOL waStarted = GT_FALSE;/* was the WA started */
    GT_BOOL waNeeded;/* do we need WA on the current port group */
    GT_PORT_GROUPS_BMP  waNeededPortGroupsBmp;/*bmp of port groups that need the WA*/
    GT_PORT_GROUPS_BMP  waNotReadyPortGroupsBmp;/*bmp of port groups that action not finished but the AUQ not full
                            so those port groups NOT ready for the WA ! */

    /* do sanity check on parameters */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
         PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E) != GT_TRUE)
    {
        debugPrint((" cpssDxChBrgFdbTriggerAuQueueWa : GT_NOT_SUPPORTED \n"));
        return GT_NOT_SUPPORTED;
    }

    /* get bmp of port groups which need the WA , and those that WA not ready for them */
    rc = fdbTriggerAuQueueWaCheck(devNum,&waNeededPortGroupsBmp,&waNotReadyPortGroupsBmp);
    if(rc != GT_OK)
    {
        debugPrint((" fdbTriggerAuQueueWaCheck : rc[%d] \n",rc));
        return rc;
    }

    if(waNeededPortGroupsBmp == 0)
    {
        debugPrint((" cpssDxChBrgFdbTriggerAuQueueWa : no reason to start the WA  : GT_BAD_STATE \n"));

        /* no reason to start the WA */
        return GT_BAD_STATE;
    }


    if((PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) &&
        PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                   enabled == GT_FALSE)
    {
        /* check if we already in the process of releasing the dead lock in the PP */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)
        {
            if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needToCheckSecondaryAuQueue == GT_TRUE)
            {
                /* application must first remove all messages from the secondary AUQ
                   before calling this function again

                   use function cpssDxChBrgFdbSecondaryAuMsgBlockGet(...) to get all
                   current messages in the queue.

                   use function cpssDxChBrgFdbQueueFullGet(...) to check that the PP
                   is stuck and need to 'release' it

                   */
                debugPrint((" cpssDxChBrgFdbTriggerAuQueueWa : portGroupId[%d] GT_BAD_STATE \n",portGroupId));
                return GT_BAD_STATE;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)

        /* check if we already in the process of releasing the dead lock in the PP */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)
        {
            /* check if wa needed for this port group */
            rc = fdbTriggerAuQueueWaCheckAndClear(devNum,portGroupId,&waNeeded);
            if(rc != GT_OK)
            {
                debugPrint((" fdbTriggerAuQueueWaCheckAndClear :  portGroupId[%d] , rc[%d] \n",portGroupId,rc));
                return rc;
            }

            if(waNeeded == GT_FALSE)
            {

                debugPrint((" fdbTriggerAuQueueWaCheckAndClear : WARNING should not happen : portGroupId[%d] not need WA,\n",portGroupId));

                /* the action is not 'stuck' on this port group */
                continue;
            }

            waStarted = GT_TRUE;

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;

            descCtrlWaPtr = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]);

            /* get a the value of the AUQ size */
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 0, 29,
                        &regValue);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(regValue != descCtrlWaPtr->blockSize)
            {
                /* set a new value to the AUQ size , to be with the size of the WA AUQ */
                rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, 0, 29,
                            descCtrlWaPtr->blockSize);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQBaseAddr;

            /* set the HW with 'secondary' AUQ base address , that will allow the PP to
               break the  deadlock that was done with the 'primary' AUQ */
            cpssOsVirt2Phy(descCtrlWaPtr->blockAddr,&phyAddr);
            rc =  prvCpssDrvHwPpPortGroupWriteRegister(devNum,portGroupId , regAddr,phyAddr);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* state that the mechanism should query the 'secondary' AUQ the
               when finish getting messages from primary AUQ */
            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needToCheckSecondaryAuQueue = GT_TRUE;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)

    }
    else
    {
        /* working without the SDMA AUQ (only with the 'on Chip Fifo') */

        /* take care of all port groups */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)
        {
            /* check if wa needed for this port group */
            rc = fdbTriggerAuQueueWaCheckAndClear(devNum,portGroupId,&waNeeded);
            if(rc != GT_OK)
            {
                debugPrint((" fdbTriggerAuQueueWaCheckAndClear :  portGroupId[%d] , rc[%d] \n",portGroupId,rc));
                return rc;
            }

            if(waNeeded == GT_FALSE)
            {
                debugPrint((" fdbTriggerAuQueueWaCheckAndClear : WARNING should not happen : portGroupId[%d] not need WA,\n",portGroupId));
                /* the action is not 'stuck' on this port group */
                continue;
            }

            waStarted = GT_TRUE;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,waNeededPortGroupsBmp,portGroupId)

    }

    if(waStarted == GT_FALSE)
    {
        debugPrint((" cpssDxChBrgFdbTriggerAuQueueWa : WARNING should not happen  : GT_BAD_STATE \n"));

        /* no reason to start the WA */
        return GT_BAD_STATE;
    }

    /* set/clear the flag of <notReady> */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(0 == (waNotReadyPortGroupsBmp & (1<<(portGroupId))))
        {
            /* clear the flag on this port group */
            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].notReady = GT_FALSE;
        }
        else
        {
            /* the WA started for the device but this port group was not ready so it was skipped */
            debugPrint(("cpssDxChBrgFdbTriggerAuQueueWa : 'not ready' portGroupId[%d] \n",portGroupId));
            /* set the flag on this port group */
            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].notReady = GT_TRUE;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)



    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbQueueFullGet
*
* DESCRIPTION:
*       The function returns the state that the AUQ/FUQ - is full/not full
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number
*       queueType - queue type AU/FU queue
*                   NOTE: when using shared queue for AU and FU messages, this
*                         parameter is ignored (and the AUQ is queried)
*                         meaning that this parameter always ignored for DxCh1
*                         devices.
*
*
* OUTPUTS:
*       isFullPtr - (pointer to) is the relevant queue full
*                   GT_TRUE  - the queue is full
*                   GT_FALSE - the queue is not full
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum , queueType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           'queue is full' when at least one of the port groups has queue full.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQueueFullGet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_BOOL *isFullPtr
)
{
    GT_U32                      regAddr;       /* register address */
    GT_STATUS                   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(isFullPtr);

    if((queueType == CPSS_DXCH_FDB_QUEUE_TYPE_AU_E) ||
       PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.fuqUseSeparate == GT_FALSE)
    {
        /*use Address Update Queue Control register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;
    }
    else if(queueType == CPSS_DXCH_FDB_QUEUE_TYPE_FU_E)
    {
        /*use FU Update Queue Control register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
    }
    else
    {
        return GT_BAD_PARAM;
    }

    /* check that at least one port group is full */
    rc = prvCpssDxChPortGroupBusyWait(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                regAddr,30,
                GT_TRUE);/* only to check the bit --> no 'busy wait' */
    if(rc == GT_BAD_STATE)
    {
        rc = GT_OK;
        /* at least one of the port groups has AUQ full */
        *isFullPtr = GT_TRUE;
    }
    else
    {
        /* All port groups not have AUQ full */
        *isFullPtr = GT_FALSE;
    }

    return rc;
}


/*******************************************************************************
* cpssDxChBrgFdbQueueRewindStatusGet
*
* DESCRIPTION:
*       function check if the specific AUQ was 'rewind' since the last time
*       this function was called for that AUQ
*       this information allow the application to know when to finish processing
*       of the WA relate to cpssDxChBrgFdbTriggerAuQueueWa(...)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       rewindPtr       - (pointer to) was the AUQ rewind since last call
*                           GT_TRUE - AUQ was rewind since last call
*                           GT_FALSE - AUQ wasn't rewind since last call
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQueueRewindStatusGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *rewindPtr
)
{
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  numPortGroupsForWa = 0;/* number of port groups that started the WA */
    GT_U32  numPortGroupsRewind = 0;/* number of port groups that rewind from the start of the WA */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(rewindPtr);

    *rewindPtr = GT_FALSE;

    if((0 == PRV_CPSS_HW_IF_PCI_COMPATIBLE_MAC(devNum)) ||
        PRV_CPSS_DXCH_PP_MAC(devNum)->errata.
        info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E.
                   enabled == GT_TRUE)
    {
        /* working without the SDMA AUQ (only with the 'on Chip Fifo') */

        /* there is no meaning for 'rewind' , so we return 'GT_OK' with 'rewind = GT_FALSE'*/

        return GT_OK;
    }


    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activated == GT_FALSE)
        {
            /* this port group was not activated for the WA */

            /* the rewind flag is not relevant for this port group */
            PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].auqRewind = GT_FALSE;

            continue;
        }

        numPortGroupsForWa ++;

        if(PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].auqRewind == GT_FALSE)
        {
            /* not rewind */
            continue;
        }

        /* this port group not need to be counted for next time , because already rewind */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activated = GT_FALSE;
        /* this port group already rewind and counted */
        PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].auqRewind = GT_FALSE;

        /* this port group was rewind */
        numPortGroupsRewind ++;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    if(numPortGroupsForWa == 0)
    {
        return GT_OK;
    }

    if(numPortGroupsForWa == numPortGroupsRewind)
    {
        /* all the relevant port groups rewind */
        *rewindPtr = GT_TRUE;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortGroupBrgFdbMessagesQueueManagerInfoGet
*
* DESCRIPTION:
*       debug tool - print info that the CPSS hold about the state of the :
*           primary AUQ,FUQ,secondary AU queue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portGroupId - the port group Id
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       relate to CQ#109715
*
*******************************************************************************/
static GT_STATUS cpssDxChPortGroupBrgFdbMessagesQueueManagerInfoGet
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId
)
{
    PRV_CPSS_AU_DESC_CTRL_STC *managerArray[NUM_OF_MESSAGES_QUEUES_CNS];
    char*                     namesArray[NUM_OF_MESSAGES_QUEUES_CNS];
    GT_U32                    managerArrayNum;
    GT_U32  ii;

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        debugPrint((" current portGroupId is : [%d]  \n", portGroupId));
    }


    managerArrayNum = 0;
    namesArray[managerArrayNum] = "auq_prim";
    managerArray[managerArrayNum] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]);
    managerArrayNum++;

    if(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId].blockSize)
    {
        namesArray[managerArrayNum] = "__fuq___";
        managerArray[managerArrayNum] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]);
        managerArrayNum++;
    }

    if(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId].blockSize)
    {
        namesArray[managerArrayNum] = "auq_seco";
        managerArray[managerArrayNum] = &(PRV_CPSS_PP_MAC(devNum)->intCtrl.secondaryAuDescCtrl[portGroupId]);
        managerArrayNum++;
    }


    debugPrint(("queue   blockAddr  blockSize   currDescIdx unreadCncCounters  \n"));
    debugPrint(("============================================================= \n"));

    for(ii = 0 ; ii < managerArrayNum ; ii++)
    {
        debugPrint(("%s    0x%8.8x  %d \t %d  \t %d \n",
                    namesArray[ii],
                    managerArray[ii]->blockAddr,
                    managerArray[ii]->blockSize,
                    managerArray[ii]->currDescIdx,
                    managerArray[ii]->unreadCncCounters
                    ));
    }

    debugPrint(("needToCheckSecondaryAuQueue = [%d],"
             "auqRewind = [%d],"
             "activated = [%d],"
             "needWaWaitForReTriggering = [%d],"
             "\n"
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needToCheckSecondaryAuQueue
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].auqRewind
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].activated
            ,PRV_CPSS_PP_MAC(devNum)->intCtrl.auqDeadLockWa[portGroupId].needWaWaitForReTriggering
            ));


    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMessagesQueueManagerInfoGet
*
* DESCRIPTION:
*       debug tool - print info that the CPSS hold about the state of the :
*           primary AUQ,FUQ,secondary AU queue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       relate to CQ#109715
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMessagesQueueManagerInfoGet
(
    IN  GT_U8   devNum
)
{
    GT_STATUS   rc;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        rc = cpssDxChPortGroupBrgFdbMessagesQueueManagerInfoGet(devNum,portGroupId);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index           - hw mac entry index
*       age            - Age flag that is used for the two-step Aging process.
*                        GT_FALSE - The entry will be aged out in the next pass.
*                        GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN GT_U32       index,
    IN GT_BOOL      age
)
{
    GT_STATUS   rc;
    GT_U32      fieldValue;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_CPSS_DXCH_FDB_INDEX_CHECK_MAC(devNum,index);

    fieldValue = BOOL2BIT_MAC(age);

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDxChPortGroupWriteTableEntryField(devNum,
                                             portGroupId,
                                             PRV_CPSS_DXCH_TABLE_FDB_E,
                                             index,
                                             0, /* word */
                                             2, /* field offset */
                                             1, /* field length */
                                             fieldValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* portGroupBitGet
*
* DESCRIPTION:
*       Read register from needed port groups , and return value of needed bit
*       from those port groups (in bitmap format)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       regAddr     - register address
*       bitIndex    - bit index to read from the register.
*
* OUTPUTS:
*       bitValuePortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      bit is set.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           bit is set , otherwise bit is not set
*                      for multi-port groups device :
*                           when bit is set (1) - the bit is set (in the
*                                            corresponding port group).
*                           when bit is not set (0) - the bit is not set. (in the corresponding
*                                            port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS portGroupBitGet
(
    IN  GT_U8    devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32  regAddr,
    IN  GT_U32  bitIndex,
    OUT GT_PORT_GROUPS_BMP  *bitValuePortGroupsBmpPtr
)
{
    GT_STATUS rc;   /* return code                  */
    GT_U32  hwData;/* hw data - single bit value - 0 or 1 */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(bitValuePortGroupsBmpPtr);

    *bitValuePortGroupsBmpPtr = 0;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* get the bit value on this port group */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, bitIndex, 1, &hwData);
        if (rc != GT_OK)
        {
            return rc;
        }

        *bitValuePortGroupsBmpPtr |= (hwData) << portGroupId;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    if(portGroupsBmp == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        /* in unaware mode convert bmps of 'all active port groups' to the
           'unaware value' CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        if((*bitValuePortGroupsBmpPtr) ==
            PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp)
        {
            /* all port groups have the bit set to 1 */
            *bitValuePortGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupTrigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       actFinishedPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      'trigger action' processing completion.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation completed , otherwise not completed
*                      for multi-port groups device :
*                           when bit is set (1) - 'trigger action' was completed by PP (in the
*                                            corresponding port group).
*                           when bit is not set (0) - 'trigger action' is not
*                                            completed yet by PP. (in the corresponding
*                                            port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupTrigActionStatusGet
(
    IN  GT_U8    devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_PORT_GROUPS_BMP  *actFinishedPortGroupsBmpPtr
)
{
    GT_STATUS rc;   /* return code                  */
    GT_U32 regAddr;     /* register address             */
    GT_PORT_GROUPS_BMP bitValuePortGroupsBmp;/*bitmap of port groups on which bit is set.*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(actFinishedPortGroupsBmpPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.macTblAction0;

    /* get value of bit 1 in the register from all port groups */
    rc = portGroupBitGet(devNum,portGroupsBmp,regAddr,1,&bitValuePortGroupsBmp);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* the 'trigger action done' is the opposite value to the bit */
    *actFinishedPortGroupsBmpPtr = ~bitValuePortGroupsBmp;
    /* return only relevant bits */
    *actFinishedPortGroupsBmpPtr &= portGroupsBmp;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupQueueFullGet
*
* DESCRIPTION:
*       The function returns the state that the AUQ/FUQ - is full/not full
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       queueType - queue type AU/FU queue
*                   NOTE: when using shared queue for AU and FU messages, this
*                         parameter is ignored (and the AUQ is queried)
*                         meaning that this parameter always ignored for DxCh1
*                         devices.
*
*
* OUTPUTS:
*       isFullPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      'the queue is full'.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           'queue is full' , otherwise 'queue is NOT full'
*                      for multi-port groups device :
*                           when bit is set (1) - 'queue is full' (in the
*                                            corresponding port group).
*                           when bit is not set (0) - 'queue is NOT full'. (in the corresponding
*                                            port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp or queueType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupQueueFullGet
(
    IN GT_U8    devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_PORT_GROUPS_BMP  *isFullPortGroupsBmpPtr
)
{
    GT_STATUS rc;   /* return code                  */
    GT_U32 regAddr;     /* register address             */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(isFullPortGroupsBmpPtr);

    if((queueType == CPSS_DXCH_FDB_QUEUE_TYPE_AU_E) ||
       PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.fuqUseSeparate == GT_FALSE)
    {
        /*use Address Update Queue Control register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.auQControl;
    }
    else if(queueType == CPSS_DXCH_FDB_QUEUE_TYPE_FU_E)
    {
        /*use FU Update Queue Control register */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->globalRegs.fuQControl;
    }
    else
    {
        return GT_BAD_PARAM;
    }

    /* get value of bit 30 in the register from all port groups */
    rc = portGroupBitGet(devNum,portGroupsBmp,regAddr,30,isFullPortGroupsBmpPtr);

    return rc;
}


/*******************************************************************************
* cpssDxChBrgFdbMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       index           - hw mac entry index
*       age            - Age flag that is used for the two-step Aging process.
*                        GT_FALSE - The entry will be aged out in the next pass.
*                        GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_BOOL      age
)
{
    return cpssDxChBrgFdbPortGroupMacEntryAgeBitSet(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,index,age);
}


/*******************************************************************************
* cpssDxChBrgFdbNaMsgVid1EnableSet
*
* DESCRIPTION:
*       Enable/Disable Tag1 VLAN Id assignment in vid1 field of the NA AU
*       message CPSS_MAC_UPDATE_MSG_EXT_STC.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE: vid1 field of the NA AU message is taken from
*                          Tag1 VLAN.
*                 GT_FALSE: vid1 field of the NA AU message is taken from
*                          Tag0 VLAN.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgVid1EnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
)
{
    GT_U32                      regAddr;        /* register address */
    GT_U32                      bitValue;       /* bit value        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.globalUnitConfig;

    bitValue = BOOL2BIT_MAC(enable);

    return prvCpssDrvHwPpSetRegField(devNum, regAddr, 2, 1, bitValue);
}

/*******************************************************************************
* cpssDxChBrgFdbNaMsgVid1EnableGet
*
* DESCRIPTION:
*       Get the status of Tag1 VLAN Id assignment in vid1 field of the NA AU
*       message CPSS_MAC_UPDATE_MSG_EXT_STC (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE: vid1 field of the NA AU message is taken from
*                          Tag1 VLAN.
*                    GT_FALSE: vid1 field of the NA AU message is taken from
*                          Tag0 VLAN.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgVid1EnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
)
{
    GT_U32                      regAddr;        /* register address */
    GT_U32                      bitValue;       /* bit value        */
    GT_STATUS                   rc;             /* return code      */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.globalUnitConfig;

    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 2, 1, &bitValue);
    if (rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(bitValue);

    return GT_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function returns AU message processing completion and success status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       completedPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      AU message processing completion.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation completed , otherwise not completed
*                      for multi-port groups device :
*                           when bit is set (1) - AU message was processed by PP (in the
*                                            corresponding port group).
*                           when bit is not set (0) - AU message processed is not
*                                            completed yet by PP. (in the corresponding
*                                            port group)
*       succeededPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      AU operation succeeded.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation succeeded , otherwise failed
*                      for multi-port groups device :
*                           when bit is set (1) - AU action succeeded (in the
*                                            corresponding port group).
*                           when bit is not set (0) - AU action has failed. (in the
*                                            corresponding port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DXCH devices the succeed status doesn't relevant for query
*       address messages (message type = CPSS_QA_E) -  FEr#93.
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_PORT_GROUPS_BMP  *completedPortGroupsBmpPtr,
    OUT GT_PORT_GROUPS_BMP  *succeededPortGroupsBmpPtr
)
{
    GT_U32      regAddr;        /* register address             */
    GT_U32      hwData;         /* data to read from register   */
    GT_STATUS   rc;             /* return code                  */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_PORT_GROUPS_BMP  completedPortGroupsBmp = 0;/* completed bmp */
    GT_PORT_GROUPS_BMP  succeededPortGroupsBmp = 0;/* succeeded bmp */


    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(completedPortGroupsBmpPtr);
    CPSS_NULL_PTR_CHECK_MAC(succeededPortGroupsBmpPtr);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                    globalRegs.addrUpdateControlReg;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* check if the message action is completed, read Message from CPU
           Management register */
        /* check that all port groups are ready */
        rc = prvCpssDxChPortGroupBusyWait(devNum,portGroupId,
                    regAddr,0,
                    GT_TRUE);/* only to check the bit --> no 'busy wait' */
        if(rc == GT_BAD_STATE)
        {
            /* this port group not completed the action */
            continue;
        }
        else if (rc != GT_OK)
        {
            return rc;
        }

        /* the action is completed on this port group */
        completedPortGroupsBmp |= 1 << portGroupId;

        /* get the succeeded bit */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 1, 1, &hwData);
        if (rc != GT_OK)
        {
            return rc;
        }

        if(hwData)
        {
            succeededPortGroupsBmp |= 1 << portGroupId;
        }

    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    *completedPortGroupsBmpPtr = completedPortGroupsBmp;
    *succeededPortGroupsBmpPtr = succeededPortGroupsBmp;

    if(portGroupsBmp == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        /* in unaware mode convert bmps of 'all active port groups' to the
           'unaware value' CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        if(completedPortGroupsBmp ==
            PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp)
        {
            /* all port groups completed the action */
            *completedPortGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

            if(completedPortGroupsBmp == succeededPortGroupsBmp)
            {
                *succeededPortGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChBrgFdbAuqDeadlockWaEnable
*
* DESCRIPTION:
*       debug tool - allow to enable/disable the use of the WA for the deadlock
*                   of the AUQ with other operations
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number.
*       _auqDeadlockWaEnable  - enable/disable the WA
*                           GT_TRUE - (default mode) allow the WA
*                           GT_FALSE - do not allow the WA
*       _auqDeadlockOnQueueNotFullWaEnable  - enable/disable the WA on 'AUQ not full'
*                           GT_TRUE - allow the WA on 'AUQ not full'
*                           GT_FALSE - (default mode) allow the WA on when 'AUQ full'
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK           - on success
* COMMENTS:
*       relate to CQ#109715
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgFdbAuqDeadlockWaEnable
(
    IN  GT_U8                                     devNum,
    IN  GT_BOOL                                   _auqDeadlockWaEnable,
    IN  GT_BOOL                                   _auqDeadlockOnQueueNotFullWaEnable
)
{
    devNum = devNum;/* avoid compiler warning */
    _auqDeadlockWaEnable = _auqDeadlockWaEnable;/* not used parameter , but to be aligned with the PSS */
    prvCpssAuqDeadlockOnQueueNotFullWaEnable = _auqDeadlockOnQueueNotFullWaEnable;

    return GT_OK;
}

