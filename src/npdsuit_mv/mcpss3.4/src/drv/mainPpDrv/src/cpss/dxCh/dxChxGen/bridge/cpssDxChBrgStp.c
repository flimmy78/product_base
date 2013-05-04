/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgStp.c
*
* DESCRIPTION:
*       Spanning Tree protocol facility DxCh cpss implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgStp.h>

static GT_STATUS stgEntrySplitTableWrite
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    IN  GT_U32     stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
);

static GT_STATUS stgEntrySplitTableRead
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    OUT  GT_U32     stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
);


/*******************************************************************************
* cpssDxChBrgStpInit
*
* DESCRIPTION:
*       Initialize STP library.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpInit
(
    IN GT_U8    dev
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgStpStateSet
*
* DESCRIPTION:
*       Sets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*       port   - port number
*       stpId  - STG (Spanning Tree Group) index  (0..255)
*       state  - STP port state.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or port or stpId or state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpStateSet
(
    IN GT_U8                    dev,
    IN GT_U8                    port,
    IN GT_U16                   stpId,
    IN CPSS_STP_STATE_ENT       state
)
{
    GT_STATUS   rc;
    GT_U32      convState;          /* STP converted state */
    GT_U32      bitOffset;          /* bit offset in current word          */
    GT_U32      wordOffset;         /* current word offset                 */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,port);
    PRV_CPSS_DXCH_STG_RANGE_CHECK_MAC(dev,stpId);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        /* convert the 'Physical port' to local port -- supporting multi-port-groups device */
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);
    }
    else
    {
        /* use global port because each entry comprize STP state of all ports.
           portGroupId is not used in this case because 
           prvCpssDxChWriteTableEntryField writes to all relevant port groups.*/
        localPort = port;
    }

    /* convert STP port state to number */
    switch(state)
    {
        case CPSS_STP_DISABLED_E:
            convState = 0;
            break;
        case CPSS_STP_BLCK_LSTN_E:
            convState = 1;
            break;
        case CPSS_STP_LRN_E:
            convState = 2;
            break;
        case CPSS_STP_FRWRD_E:
            convState = 3;
            break;
        default:
            return GT_BAD_PARAM;
    }

    /* calculate port stp state field offset in bits */
    /* each word in the STP entry hold 16 STP ports */
    wordOffset = (2 * localPort) >> 5;   /* / 32 */
    bitOffset = (2 * localPort) & 0x1f;  /* % 32 */

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        /* convert the 'Physical port' to portGroupId -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);

        return prvCpssDxChPortGroupWriteTableEntryField(dev,portGroupId,
                                             PRV_CPSS_DXCH_TABLE_STG_E,
                                             stpId,
                                             wordOffset,
                                             bitOffset,
                                             2,
                                             convState);
    }
    else
    {
        /*write to ingress*/
        rc = prvCpssDxChWriteTableEntryField(dev,
                                             PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                                             stpId,
                                             wordOffset,
                                             bitOffset,
                                             2,
                                             convState);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*write to egress*/
        return prvCpssDxChWriteTableEntryField(dev,
                                             PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
                                             stpId,
                                             wordOffset,
                                             bitOffset,
                                             2,
                                             convState);
    }
}

/*******************************************************************************
* cpssDxChBrgStpStateGet
*
* DESCRIPTION:
*       Gets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       port        - port number
*       stpId       - STG (Spanning Tree Group) index  (0..255)
*
* OUTPUTS:
*       statePtr    - (pointer to) STP port state.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or port or stpId or state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpStateGet
(
    IN GT_U8                    dev,
    IN GT_U8                    port,
    IN GT_U16                   stpId,
    OUT CPSS_STP_STATE_ENT      *statePtr
)
{
    GT_STATUS   rc;                 /* return code  */
    GT_U32      convState;          /* STP converted state */
    GT_U32      bitOffset;          /* bit offset in current word          */
    GT_U32      wordOffset;         /* current word offset                 */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U8   localPort;/* local port - support multi-port-groups device */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_PHY_PORT_CHECK_MAC(dev,port);
    PRV_CPSS_DXCH_STG_RANGE_CHECK_MAC(dev,stpId);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        /* convert the 'Physical port' to local port -- supporting multi-port-groups device */
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);
    }
    else
    {
        /* use global port because each entry comprize STP state of all ports.
           portGroupId is not used in this case because entry is same for all 
           relevant port groups.*/
        localPort = port;
    }

    /* calculate port stp state field offset in bits */
    /* each word in the STP entry hold 16 STP ports */
    wordOffset = (2 * localPort) >> 5;   /* / 32 */
    bitOffset = (2 * localPort) & 0x1f;  /* % 32 */

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        /* convert the 'Physical port' to portGroupId -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port);
        rc = prvCpssDxChPortGroupReadTableEntryField(dev,portGroupId,
                                                     PRV_CPSS_DXCH_TABLE_STG_E,
                                                     stpId,
                                                     wordOffset,
                                                     bitOffset,
                                                     2,
                                                     &convState);
    }
    else
    {
        /* read field only from ingress table */
        rc = prvCpssDxChReadTableEntryField(dev,
                                            PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                                            stpId,
                                            wordOffset,
                                            bitOffset,
                                            2,
                                            &convState);
    }

    if( GT_OK != rc )
    {
        return rc;
    }

    /* convert number to STP port state */
    switch(convState)
    {
        case 0: *statePtr = CPSS_STP_DISABLED_E;
                break;
        case 1: *statePtr = CPSS_STP_BLCK_LSTN_E;
                break;
        case 2: *statePtr = CPSS_STP_LRN_E;
                break;
        case 3: *statePtr = CPSS_STP_FRWRD_E;
                break;
        default:
            return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgStpEntryGet
*
* DESCRIPTION:
*       Gets the RAW STP entry from HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*       stpId  - STG (Spanning Tree Group) index  (0..255)
*
* OUTPUTS:
*       stpEntryWordArr     - (Pointer to) The raw stp entry to get.
*                         memory allocated by the caller.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpEntryGet
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    OUT GT_U32      stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_STG_RANGE_CHECK_MAC(dev,stpId);
    CPSS_NULL_PTR_CHECK_MAC(stpEntryWordArr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        return stgEntrySplitTableRead(dev,stpId,stpEntryWordArr);
    }
    else
    {
        return prvCpssDxChReadTableEntry(dev,
                                         PRV_CPSS_DXCH_TABLE_STG_E,
                                         (GT_U32)stpId,
                                         stpEntryWordArr);
    }
}

/*******************************************************************************
* cpssDxChBrgStpEntryWrite
*
* DESCRIPTION:
*       Writes a raw STP entry to HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - device number
*       stpId           - STG (Spanning Tree Group) index  (0..255)
*       stpEntryWordArr     - (Pointer to) The raw stp entry to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpEntryWrite
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    IN  GT_U32      stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
)
{
    PRV_CPSS_DXCH_DEV_CHECK_MAC(dev);
    PRV_CPSS_DXCH_STG_RANGE_CHECK_MAC(dev,stpId);
    CPSS_NULL_PTR_CHECK_MAC(stpEntryWordArr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(dev))
    {
        return stgEntrySplitTableWrite(dev,stpId,stpEntryWordArr);
    }
    else
    {
        return prvCpssDxChWriteTableEntry(dev,
                                          PRV_CPSS_DXCH_TABLE_STG_E,
                                          (GT_U32)stpId,
                                          stpEntryWordArr);
    }
}


/*******************************************************************************
* stgEntrySplitTableRead
*
* DESCRIPTION:
*       Gets the RAW STP entry from HW. - for split table (ingress,egress)
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       dev    - device number
*       stpId  - STG (Spanning Tree Group) index  (0..255)
*
* OUTPUTS:
*       stpEntryWordArr     - (Pointer to) The raw stp entry to get.
*                         memory allocated by the caller.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_BAD_STATE    - the ingress table and egress table not match
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS stgEntrySplitTableRead
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    OUT  GT_U32     stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
)
{
    GT_STATUS rc;
    GT_U32  tmpStpEntry[CPSS_DXCH_STG_ENTRY_SIZE_CNS];

    /* read from ingress table */
    rc =  prvCpssDxChReadTableEntry(dev,
                                     PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                                     (GT_U32)stpId,
                                     stpEntryWordArr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* read from egress table */
    rc =  prvCpssDxChReadTableEntry(dev,
                                     PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
                                     (GT_U32)stpId,
                                     tmpStpEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(0 != cpssOsMemCmp(tmpStpEntry,stpEntryWordArr,sizeof(tmpStpEntry)))
    {
        /* the ingress STG and the egress STG not synchronized ?! */
        /* we have not API to allow this situation */
        return GT_BAD_STATE;
    }

    return GT_OK;
}

/*******************************************************************************
* stgEntrySplitTableWrite
*
* DESCRIPTION:
*       Sets the RAW STP entry from HW. - for split table (ingress,egress)
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       dev    - device number
*       stpId  - STG (Spanning Tree Group) index  (0..255)
*       stpEntryWordArr     - (Pointer to) The raw stp entry to get.
*                         memory allocated by the caller.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS stgEntrySplitTableWrite
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    IN  GT_U32     stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
)
{
    GT_STATUS rc;

    /* write to ingress table */
    rc =  prvCpssDxChWriteTableEntry(dev,
                                     PRV_CPSS_DXCH_LION_TABLE_STG_INGRESS_E,
                                     (GT_U32)stpId,
                                     stpEntryWordArr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* write to egress table */
    rc =  prvCpssDxChWriteTableEntry(dev,
                                     PRV_CPSS_DXCH_LION_TABLE_STG_EGRESS_E,
                                     (GT_U32)stpId,
                                     stpEntryWordArr);
    return rc;
}

