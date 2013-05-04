/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpLpmDbg.c
*
* DESCRIPTION:
*       the CPSS DXCH debug .
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>


/*******************************************************************************
* IpLttRead
*
* DESCRIPTION:
*     This func reads  LTT data .
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - the device number
*       lttTtiRow           - router tti TCAM row
*       lttTtiColumn        - router tti TCAM column
* OUTPUTS:
*     dataPtr              - pointer to LTT data
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/

static GT_STATUS IpLttRead
(
    IN  GT_U8    devNum,
    IN  GT_U32   lttTtiRow,
    IN  GT_U32   lttTtiColumn,
    OUT GT_U32   *dataPtr
)
{
    GT_STATUS   retVal = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    /* ltt size is the same as router TCAM size */
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    default:
        if (lttTtiColumn >= 4)
            return GT_BAD_PARAM;
        break;
    }
    CPSS_NULL_PTR_CHECK_MAC(dataPtr);


    /* now get the 22 bits in the hw according the location of the entry in the line */
    retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
                                            lttTtiRow,lttTtiColumn,0,22,dataPtr);
    return (retVal);
}

/*******************************************************************************
* readIpv4Prefix
*
* DESCRIPTION:
*     This func reads ipv4 prefixes from physical router tcam .
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum              - the device number
*       row                 - router tti TCAM row
*       column              - router tti TCAM column
* OUTPUTS:
*     word0Ptr              - pointer to first word of TCAM key (hw format) per specific column
*     word1Ptr              - pointer to second word  of TCAM key (hw format) per specific column
*     mask0Ptr              - pointer to first word of TCAM mask (hw format)
*     mask1Ptr              - pointer to second  word of TCAM mask (hw format)
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_INITIALIZED       - if the driver was not initialized
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - if pp family nor CHEETAH2 and nor CHEETAH3.
*
* COMMENTS:
*     none
*
*******************************************************************************/

static GT_STATUS readIpv4Prefix
(
       IN  GT_U8    devNum,
       IN  GT_U32   row,
       IN  GT_U32   column,
       OUT GT_U32  *word0Ptr,
       OUT GT_U32  *word1Ptr,
       OUT GT_U32  *mask0Ptr,
       OUT GT_U32  *mask1Ptr
)
{
       GT_BOOL                     valid;
       GT_U32  keyArr[6];           /* TCAM key in hw format            */
       GT_U32  maskArr[6];          /* TCAM mask in hw format           */
       GT_U32  actionArr[6];        /* TCAM action (rule) in hw format  */
       GT_U32  validArr[5];         /* TCAM line valid bits             */
       GT_U32  compareModeArr[5];   /* TCAM line compare mode           */

       GT_U32  wordOffset, word0, word1, mask0, mask1;
       GT_STATUS retVal;

       /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

       CPSS_NULL_PTR_CHECK_MAC(word0Ptr);
       CPSS_NULL_PTR_CHECK_MAC(word1Ptr);
       CPSS_NULL_PTR_CHECK_MAC(mask0Ptr);
       CPSS_NULL_PTR_CHECK_MAC(mask1Ptr);

        /* clear data */
       cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * 6);
       cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * 6);
       cpssOsMemSet(actionArr, 0, sizeof(GT_U32) * 5);
       cpssOsMemSet(validArr, 0, sizeof(GT_U32) * 5);

       /* read hw data */
       retVal = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                       row,&validArr[0],
                                                       &compareModeArr[0],
                                                       &keyArr[0],&maskArr[0],
                                                       &actionArr[0]);
       if (retVal != GT_OK)
               return retVal;


       /* entry holds valid IPv4 prefix if the followings applies:
            - the entry is valid
            - the compare mode of the entry is single compare mode */
       if ((validArr[column] == 1) && (compareModeArr[column] == 0))
           valid = GT_TRUE;
       else
           valid = GT_FALSE;
       if(valid != GT_TRUE)
               return GT_NO_SUCH;

       switch (column)
       {
       case 0:
       case 2:
               wordOffset = (column == 0) ? 0 : 3;
               word0 = keyArr[wordOffset];
               word1 = keyArr[wordOffset + 1] & 0xffff;
               mask0 = maskArr[wordOffset];
               mask1 = maskArr[wordOffset + 1] & 0xffff;
       break;

       case 1:
       case 3:
               wordOffset = (column == 1) ? 1 : 4;

               word0 = ((keyArr[wordOffset] & 0xffff0000)>>16) | ((keyArr[wordOffset+1] & 0xffff)<<16);
               word1 = (keyArr[wordOffset+1] & 0xffff0000)>>16;
               mask0 = ((maskArr[wordOffset] & 0xffff0000)>>16) | ((maskArr[wordOffset+1] & 0xffff)<<16);
               mask1 = (maskArr[wordOffset+1] & 0xffff0000)>>16;
       break;
       default:
               return GT_BAD_PARAM;
       }

       *word0Ptr = word0;
       *word1Ptr = word1;
       *mask0Ptr = mask0;
       *mask1Ptr = mask1;

       return GT_OK;
}

/*******************************************************************************
* dumpRouteTcam
*
* DESCRIPTION:
*     This func makes physical router tcam scanning and prints its contents.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*     dump   -  parameter for debugging purposes
*
* OUTPUTS:
*     None
*
* RETURNS:
*       GT_OK   - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_INITIALIZED       - if the driver was not initialized
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL - on failure.
*
* COMMENTS:
*     None
*
*******************************************************************************/


GT_STATUS dumpRouteTcam
(
    IN GT_BOOL dump
)
{
    GT_U8   devNum =0;
    GT_U32  routerTtiTcamRow;
    GT_U32  routerTtiTcamColumn;
    GT_U32  tcamData32 = 0, tcamData16 = 0, tcamMask32 = 0, tcamMask16 = 0, lttData;
    GT_U32  retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;
       if(dump == GT_FALSE)
               return GT_OK;

       for(routerTtiTcamColumn = 0; routerTtiTcamColumn < 4 ; routerTtiTcamColumn++)
       {
               for(routerTtiTcamRow = 0 ; routerTtiTcamRow < fineTuningPtr->tableSize.routerAndTunnelTermTcam ; routerTtiTcamRow ++)
               {
                       retVal = readIpv4Prefix(devNum,routerTtiTcamRow,routerTtiTcamColumn,
                                               &tcamData32,&tcamData16,&tcamMask32, &tcamMask16);
                       if( retVal == GT_NO_SUCH) /* Non valid entry */
                               continue;
                       if( retVal != GT_OK)
                               return retVal;

                       { /* Read LTT entry and print the information. */
                               retVal = IpLttRead(devNum,routerTtiTcamRow,routerTtiTcamColumn,&lttData);
                               if( retVal != GT_OK)
                                       return retVal;

                               cpssOsPrintf("R:%8d C:%8d    Key:0x%08X 0x%08X    Mask:0x%08X 0x%08X   LTT:0x%08X\n", routerTtiTcamRow,  routerTtiTcamColumn,
                                                 tcamData32, tcamData16,  tcamMask32,  tcamMask16,  lttData);
                       }

               }
       }
       return GT_OK;
}

