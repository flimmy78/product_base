/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgMc.c
*
* DESCRIPTION:
*       CPSS DXCh Multicast Group facility implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgMc.h>

/* number of words in vidx entry */
#define VIDX_NUM_WORDS_CNS  3

/*******************************************************************************
* prvDxChBrgMcMemberBitSet
*
* DESCRIPTION:
*       Handles port member in the Multicast Group entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - PP's device number.
*       vidx            - multicast group index.
*                         valid range 0..(Layer-2 Multicast groups number - 1).
*                         see datasheet of specific device to get number of
*                         Layer-2 Multicast groups.
*                         The flood VIDX entry is not allowed for non CPU port 
*                         configuration. CPU port may be added or deleted from
*                         the flood VIDX entry.
*                         The flood VIDX is 0x3FF for xCat2 devices and 
*                         0xFFF for other devices.
*       portNum         - physical port number.
*                         Range of portNum is: [0-27] for DxCh2;
*                                              [0-26] for DxCh.
*                         CPU port supported
*       bitValue        - 0: Delete port from the Multicast Group entry.
*                         1: Add port to the Multicast Group entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum or 
*                             vidx is larger than allowed value.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvDxChBrgMcMemberBitSet
(
    IN GT_U8    devNum,
    IN GT_U16   vidx,
    IN GT_U8    portNum,
    IN GT_U32   bitValue
)
{
    GT_U32  bitOffset;  /* bit offset of port in the MC Entry */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;  /* local port - support multi-port-groups device */
    GT_U16  floodVidx;   /* reserved VIDX for flooding traffic */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {   /* xCat2 flood VIDX is 0x3FF */
        floodVidx = 0x3FF;
    }
    else
    {
        /* Non xCat2 flood VIDX is 0xFFF */
        floodVidx = 0xFFF;
    }

    /* allow CPU port member manipulation in the flood VIDX. 
       Other members cannot be changed in the flood VIDX. */
    if ((portNum != CPSS_CPU_PORT_NUM_CNS) || (vidx != floodVidx))
    {
        if (vidx == floodVidx)
        {
            return GT_BAD_PARAM;
        }

        PRV_CPSS_DXCH_VIDX_NUM_CHECK_MAC(devNum,vidx);
    }

    if (bitValue > 1)
    {
        return GT_BAD_PARAM;
    }

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if (portNum != CPSS_CPU_PORT_NUM_CNS)
        {
            /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
            localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

            if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
                if (localPort > 26)
                {
                    return GT_BAD_PARAM;
                }
            }
            else
            {
                if (localPort > 27)
                {
                    return GT_BAD_PARAM;
                }
            }

            /* Calculate HW bit offset for requested port number */
            bitOffset  = localPort + 1;
        }
        else
        {
            /* CPU port member bit */
            bitOffset = 0;
            portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }

        /* Set VIDX MC Entry bit is associated with the added port number */
        return prvCpssDxChPortGroupWriteTableEntryField(devNum,portGroupId,
                                               PRV_CPSS_DXCH_TABLE_MULTICAST_E,
                                               (GT_U32)vidx,  /* entry index */
                                               0,             /* word0 */
                                               bitOffset,     /* start bit */
                                               1,             /* field length */
                                               bitValue);     /* field value */
    }
    else
    {
        if(portNum > 63)
        {
            /* supports 64 ports */
            return GT_BAD_PARAM;
        }

        return prvCpssDxChWriteTableEntryField(devNum,
                                               PRV_CPSS_DXCH_TABLE_MULTICAST_E,
                                               (GT_U32)vidx,  /* entry index */
                                               PRV_CPSS_DXCH_TABLE_WORD_INDICATE_GLOBAL_BIT_CNS,
                                               portNum,       /* start bit */
                                               1,             /* field length */
                                               bitValue);     /* field value */
    }
}

/*******************************************************************************
* cpssDxChBrgMcInit
*
* DESCRIPTION:
*       Initializes Bridge engine Multicast Library.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgMcInit
(
    IN GT_U8    devNum
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgMcIpv6BytesSelectSet
*
* DESCRIPTION:
*
*       Sets selected 4 bytes from the IPv6 SIP and 4 bytes from the IPv6 DIP
*       are used upon IPv6 MC bridging, based on the packet's IP addresses (due
*       to the limitation of Cheetah FDB Entry, which contains only 32 bits for
*       DIP and 32 bits for SIP, in spite of 128 bits length of IPv6 address).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - PP's device number.
*       dipBytesSelectMapArr- array, which contains numbers of 4 bytes from the
*                             DIP IPv6 address are used upon IPv6 MC bridging.
*                             The first array element contains DIP byte 0,
*                             respectively the fourth array element contains
*                             DIP byte 3.
*
*                             The numbers of IPv6 address bytes are in the
*                             Network format and in the range [0:15], where 0
*                             is MSB of IP address and 15 is LSB of IP address.
*
*       sipBytesSelectMapArr- array, which contains numbers of 4 bytes from the
*                             SIP IPv6 address are used upon IPv6 MC bridging.
*                             The first array element contains SIP byte 0,
*                             respectively the fourth array element contains
*                             SIP byte 3.
*
*                             The numbers of IPv6 address bytes are in the
*                             Network format and in the range [0:15], where 0
*                             is MSB of IP address and 15 is LSB of IP address.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointers.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgMcIpv6BytesSelectSet
(
    IN GT_U8  devNum,
    IN GT_U8  dipBytesSelectMapArr[4],
    IN GT_U8  sipBytesSelectMapArr[4]
)
{
    GT_U32      i;              /* iterator */
    GT_U32      regAddr;        /* register address */
    GT_U32      regValue;       /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(dipBytesSelectMapArr);
    CPSS_NULL_PTR_CHECK_MAC(sipBytesSelectMapArr);

    /* Verify validity of the passed parameters */
    for (i = 0; i < 4; i++)
    {
        if ((sipBytesSelectMapArr[i] > 0xF) || (dipBytesSelectMapArr[i] > 0xF))
        {
            return GT_BAD_PARAM;
        }
    }

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeIpv6BytesSelection;

    /* Convert DIP and SIP Big-Endian Network    */
    /* format to the Little-Endian PP HW format. */
    regValue = 0;
    for (i = 0; i < 4; i++)
    {
        regValue |= ((0xF - dipBytesSelectMapArr[i]) << (4*i)) |
                    ((0xF - sipBytesSelectMapArr[i]) << (4*i+16));
    }

    /* Set DIP and SIP values to the HW register */
    return prvCpssDrvHwPpWriteRegister(devNum, regAddr, regValue);
}

/*******************************************************************************
* cpssDxChBrgMcIpv6BytesSelectGet
*
* DESCRIPTION:
*
*       Gets selected 4 bytes from the IPv6 SIP and 4 bytes from the IPv6 DIP
*       are used upon IPv6 MC bridging, based on the packet's IP addresses (due
*       to the limitation of Cheetah FDB Entry, which contains only 32 bits for
*       DIP and 32 bits for SIP, in spite of 128 bits length of IPv6 address).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - PP's device number.
*
* OUTPUTS:
*       dipBytesSelectMapArr- array, which contains numbers of 4 bytes from the
*                             DIP IPv6 address are used upon IPv6 MC bridging.
*                             The first array element contains DIP byte 0,
*                             respectively the fourth array element contains
*                             DIP byte 3.
*
*                             The numbers of IPv6 address bytes are in the
*                             Network format and in the range [0:15], where 0
*                             is MSB of IP address and 15 is LSB of IP address.
*
*       sipBytesSelectMapArr- array, which contains numbers of 4 bytes from the
*                             SIP IPv6 address are used upon IPv6 MC bridging.
*                             The first array element contains SIP byte 0,
*                             respectively the fourth array element contains
*                             SIP byte 3.
*
*                             The numbers of IPv6 address bytes are in the
*                             Network format and in the range [0:15], where 0
*                             is MSB of IP address and 15 is LSB of IP address.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointers.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgMcIpv6BytesSelectGet
(
    IN  GT_U8  devNum,
    OUT GT_U8  dipBytesSelectMapArr[4],
    OUT GT_U8  sipBytesSelectMapArr[4]
)
{
    GT_U32      i;              /* iterator */
    GT_U32      regAddr;        /* register address */
    GT_STATUS   retStatus;      /* generic return status code */
    GT_U32      regValue;       /* register value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(dipBytesSelectMapArr);
    CPSS_NULL_PTR_CHECK_MAC(sipBytesSelectMapArr);

    regAddr =
        PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeIpv6BytesSelection;

    /* Read DIP and SIP values from HW */
    retStatus = prvCpssDrvHwPpReadRegister(devNum, regAddr, &regValue);
    if (GT_OK != retStatus)
    {
        return retStatus;
    }

    /* Convert DIP and SIP Little-Endian PP HW  */
    /* format to the Big-Endian Network format. */
    for (i = 0; i < 4; i++)
    {
        dipBytesSelectMapArr[i] =
            (GT_U8)(0xF - ((regValue >> (4*i)) & 0xF));
        sipBytesSelectMapArr[i] =
            (GT_U8)(0xF - ((regValue >> (4*i+16)) & 0xF));
    }

    return retStatus;
}

/*******************************************************************************
* prvCpssDxChBrgMcEntryWrite
*
* DESCRIPTION:
*       Writes Multicast Group entry to the HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum          - PP's device number.
*       vidx            - multicast group index.
*                         valid range 0..(Layer-2 Multicast groups number - 1).
*                         see datasheet of specific device to get number of
*                         Layer-2 Multicast groups.
*                         The flood VIDX entry is not allowed for configuration.
*                         The flood VIDX is 0x3FF for xCat2 devices and 
*                         0xFFF for other devices.
*       portBitmapPtr   - pointer to the bitmap of ports are belonged to the
*                         vidx MC group of specified PP device. Only first 27
*                         bits for DxCh and 28 bits for DxCh2 and above can be set.
*                         CPU port supported
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum, ports bitmap value or
*                                  vidx is larger than the allowed value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function doesn't allow rewrite the VIDX 0xFFF Multicast Table Entry.
*       It should allow the Multicast packets to be flooded to all VLAN ports.
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgMcEntryWrite
(
    IN GT_U8                devNum,
    IN GT_U16               vidx,
    IN CPSS_PORTS_BMP_STC   *portBitmapPtr
)
{
    GT_U32      vidxHwEntry[VIDX_NUM_WORDS_CNS];
    GT_STATUS   rc;  /* generic return status code */
    GT_U32      hwPortBitmap;     /* bmp of ports in HW entry   */
    GT_U32      hwPortBitmapSize; /* bmp of ports in HW entry size in bits */
                                  /* DxCh2 - 28 ports ; DxCh - 27 ports    */

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* bit0 - CPU port and 27 ports */
            hwPortBitmapSize = 28;
        }
        else
        {
            /* bit0 - CPU port and 28 ports */
            hwPortBitmapSize = 29;
        }

        if (portBitmapPtr->ports[0] >= (GT_U32)(1 << (hwPortBitmapSize - 1)))
        {
            return GT_BAD_PARAM;
        }

        hwPortBitmap = (portBitmapPtr->ports[0] << 1);
        if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(
            portBitmapPtr, CPSS_CPU_PORT_NUM_CNS))
        {
            /* bit0 related to CPU port membership */
            hwPortBitmap |= 1;
        }

        /* Write bitmap of ports are belonged to the vidx MC group */
        rc = prvCpssDxChWriteTableEntryField(devNum,
                                                 PRV_CPSS_DXCH_TABLE_MULTICAST_E,
                                             (GT_U32)vidx,  /* entry index  */
                                             0,             /* word0        */
                                             0,                   /* start bit    */
                                             hwPortBitmapSize,    /* field length */
                                             hwPortBitmap);

        return rc;
    }
    else
    {
        vidxHwEntry[0] =  portBitmapPtr->ports[0];
        vidxHwEntry[1] =  portBitmapPtr->ports[1];
        vidxHwEntry[2] =  0;

        rc =  prvCpssDxChWriteTableEntry(devNum,
                                         PRV_CPSS_DXCH_TABLE_MULTICAST_E,
                                         (GT_U32)vidx,  /* entry index */
                                         &vidxHwEntry[0]);/* field value */
        return rc;
    }
}

/*******************************************************************************
* cpssDxChBrgMcEntryWrite
*
* DESCRIPTION:
*       Writes Multicast Group entry to the HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum          - PP's device number.
*       vidx            - multicast group index.
*                         valid range 0..(Layer-2 Multicast groups number - 1).
*                         see datasheet of specific device to get number of
*                         Layer-2 Multicast groups.
*                         The flood VIDX entry is not allowed for configuration.
*                         The flood VIDX is 0x3FF for xCat2 devices and 
*                         0xFFF for other devices.
*       portBitmapPtr   - pointer to the bitmap of ports are belonged to the
*                         vidx MC group of specified PP device. Only first 27
*                         bits for DxCh and 28 bits for DxCh2 and above can be set.
*                         CPU port supported
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum, ports bitmap value or
*                                  vidx is larger than the allowed value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function doesn't allow rewrite the VIDX 0xFFF Multicast Table Entry.
*       It should allow the Multicast packets to be flooded to all VLAN ports.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgMcEntryWrite
(
    IN GT_U8                devNum,
    IN GT_U16               vidx,
    IN CPSS_PORTS_BMP_STC   *portBitmapPtr
)
{
    GT_STATUS   rc;          /* generic return status code */
    GT_U16      floodVidx;   /* reserved VIDX for flooding traffic */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_VIDX_NUM_CHECK_MAC(devNum,vidx);
    CPSS_NULL_PTR_CHECK_MAC(portBitmapPtr);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {   /* xCat2 flood VIDX is 0x3FF */
        floodVidx = 0x3FF;
    }
    else
    {
        /* Non xCat2 flood VIDX is 0xFFF */
        floodVidx = 0xFFF;
    }

    /* The flood VIDX entry cannot be changed */
    if (vidx == floodVidx)
    {
        return GT_BAD_PARAM;
    }

    rc = prvCpssDxChBrgMcEntryWrite(devNum, vidx, portBitmapPtr);
    return rc;
}
/*******************************************************************************
* cpssDxChBrgMcEntryRead
*
* DESCRIPTION:
*       Reads the Multicast Group entry from the HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - PP's device number.
*       vidx            - multicast group index.
*                         valid range 0..(Layer-2 Multicast groups number - 1).
*                         see datasheet of specific device to get number of
*                         Layer-2 Multicast groups.
*                         The flood VIDX entry is allowed for reading.
*                         The flood VIDX is 0x3FF for xCat2 devices and 
*                         0xFFF for other devices.
*
* OUTPUTS:
*       portBitmapPtr   - pointer to the bitmap of ports are belonged to the
*                         vidx MC group of specified PP device.
*                         CPU port supported
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or 
*                                  vidx is larger than the allowed value.
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgMcEntryRead
(
    IN  GT_U8                devNum,
    IN  GT_U16               vidx,
    OUT CPSS_PORTS_BMP_STC   *portBitmapPtr
)
{
    GT_U32      vidxHwEntry[VIDX_NUM_WORDS_CNS];
    GT_STATUS   rc;               /* return code                */
    GT_U32      hwPortBitmap;     /* bmp of ports in HW entry   */
    GT_U32      hwPortBitmapSize; /* bmp of ports in HW entry size in bits */
                                  /* DxCh2 - 28 ports ; DxCh - 27 ports    */
    GT_U16  floodVidx;   /* reserved VIDX for flooding traffic */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(portBitmapPtr);

    if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
    {   /* xCat2 flood VIDX is 0x3FF */
        floodVidx = 0x3FF;
    }
    else
    {
        /* Non xCat2 flood VIDX is 0xFFF */
        floodVidx = 0xFFF;
    }

    /* The flood VIDX entry can be read */
    if (vidx != floodVidx)
    {
        /* Check Multicast Group entry index (VIDX) */
        PRV_CPSS_DXCH_VIDX_NUM_CHECK_MAC(devNum,vidx);
    }

    cpssOsMemSet(portBitmapPtr, 0, sizeof(CPSS_PORTS_BMP_STC));

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
        /* Gather the bitmap of ports from the VIDX Multicast Table Entry */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
        {
            hwPortBitmapSize = 28;
        }
        else
        {
            hwPortBitmapSize = 29;
        }

        /* Read the VIDX Multicast Table Entry from the HW */
        rc = prvCpssDxChReadTableEntryField(
                                          devNum,
                                          PRV_CPSS_DXCH_TABLE_MULTICAST_E,
                                          (GT_U32)vidx, /* entry index  */
                                          0,            /* word0        */
                                          0,                  /* start bit    */
                                          hwPortBitmapSize,   /* field length */
                                          &hwPortBitmap);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* HW format to SW format */
        portBitmapPtr->ports[0] = (hwPortBitmap >> 1);
        portBitmapPtr->ports[1] = 0;

        if ((hwPortBitmap & 1) != 0)
        {
            /* CPU port is a member of MC group */
            CPSS_PORTS_BMP_PORT_SET_MAC(
                portBitmapPtr, CPSS_CPU_PORT_NUM_CNS);
        }

        return GT_OK;
    }
    else
    {
        rc =  prvCpssDxChReadTableEntry(devNum,
                                       PRV_CPSS_DXCH_TABLE_MULTICAST_E,
                                       (GT_U32)vidx,  /* entry index */
                                       &vidxHwEntry[0]);/* field value */
        portBitmapPtr->ports[0] = vidxHwEntry[0];
        portBitmapPtr->ports[1] = vidxHwEntry[1];

        return rc;
    }
}

/*******************************************************************************
* cpssDxChBrgMcGroupDelete
*
* DESCRIPTION:
*       Deletes the Multicast Group entry from the HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - PP's device number.
*       vidx            - multicast group index.
*                         valid range 0..(Layer-2 Multicast groups number - 1).
*                         see datasheet of specific device to get number of
*                         Layer-2 Multicast groups.
*                         The flood VIDX entry is not allowed for configuration.
*                         The flood VIDX is 0x3FF for xCat2 devices and 
*                         0xFFF for other devices.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or 
*                                  vidx is larger than the allowed value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgMcGroupDelete
(
    IN GT_U8    devNum,
    IN GT_U16   vidx
)
{
    CPSS_PORTS_BMP_STC  portBitmap;  /* MC group ports bitmap */

    /* Nullify MC group ports bitmap */
    portBitmap.ports[0] = 0;
    portBitmap.ports[1] = 0;

    /* Write nullified MC group ports bitmap to the MC Table Entry */
    return cpssDxChBrgMcEntryWrite(devNum, vidx, &portBitmap);
}

/*******************************************************************************
* cpssDxChBrgMcMemberAdd
*
* DESCRIPTION:
*       Add new port member to the Multicast Group entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - PP's device number.
*       vidx            - multicast group index.
*                         valid range 0..(Layer-2 Multicast groups number - 1).
*                         see datasheet of specific device to get number of
*                         Layer-2 Multicast groups.
*                         The flood VIDX entry is not allowed for non CPU port 
*                         configuration. CPU port may be added to the flood 
*                         VIDX entry.
*                         The flood VIDX is 0x3FF for xCat2 devices and 
*                         0xFFF for other devices.
*       portNum         - physical port number.
*                         Range of portNum is: [0-27] for DXCh2 and above;
*                                              [0-26] for DXCh.
*                         CPU port supported
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum, portNum or 
*                                  vidx is larger than the allowed value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgMcMemberAdd
(
    IN GT_U8   devNum,
    IN GT_U16  vidx,
    IN GT_U8   portNum
)
{
    return prvDxChBrgMcMemberBitSet(devNum, vidx, portNum, 1);
}

/*******************************************************************************
* cpssDxChBrgMcMemberDelete
*
* DESCRIPTION:
*       Delete port member from the Multicast Group entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - PP's device number.
*       vidx            - multicast group index.
*                         valid range 0..(Layer-2 Multicast groups number - 1).
*                         see datasheet of specific device to get number of
*                         Layer-2 Multicast groups.
*                         The flood VIDX entry is not allowed for non CPU port 
*                         configuration. CPU port may be deleted from the flood 
*                         VIDX entry.
*                         The flood VIDX is 0x3FF for xCat2 devices and 
*                         0xFFF for other devices.
*       portNum         - physical port number.
*                         Range of portNum is: [0-27] for DXCh2 and above;
*                                              [0-26] for DxCh.
*                         CPU port supported
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum, portNum or 
*                                  vidx is larger than the allowed value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgMcMemberDelete
(
    IN GT_U8          devNum,
    IN GT_U16         vidx,
    IN GT_U8          portNum
)
{
    return prvDxChBrgMcMemberBitSet(devNum, vidx, portNum, 0);
}
