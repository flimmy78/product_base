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
* cpssDxChBrgFdbHash.c
*
* DESCRIPTION:
*       Hash calculate for MAC address table implementation.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/bridge/private/prvCpssDxChBrgFdbHash.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdbHash.h>
#include <cpss/generic/bridge/private/prvCpssBrgVlanTypes.h>

#define PRV_GET_BIT_MAC(word,bitNum) ((word) >> (bitNum))

/*******************************************************************************
* macHashCalcXor
*
* DESCRIPTION:
*       This function calculates the hash index for the mac address table.
*       for specific mac address and VLAN id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       addrPtr     - (pointer to) the mac address.
*       vid         - the VLAN id.
*       numOfBitsToUse - number of bits to use in MAC XOR hash algorithm
*
* OUTPUTS:
*       hashPtr - (pointer to) the hash index.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS macHashCalcXor(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *addrPtr,
    IN  GT_U16          vid,
    IN  GT_U32          numOfBitsToUse,
    OUT GT_U32          *hashPtr
)
{
    GT_U32              transposedHash; /* contains the transposed hash value */
    GT_U32              u32Array[7];
    GT_U8               *arEther;/* (pointer to) mac addres bytes */
    GT_U32              swappedVidBits;/* swapped vid bits */
    GT_U32              ii;/* iterator */


    arEther = addrPtr->arEther;

    swappedVidBits = 0;

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.vlanMode == CPSS_IVL_E)
    {
        /* swap the vid bits */
        for(ii = 0 ; ii < 12; ii++ )
        {
            swappedVidBits |= (vid & (1 << ii)) ?
                                (1 << (11-ii)) :
                                0;
        }
    }

    if(numOfBitsToUse == 12)
    {
        /* the hash function is :  */
        /* for 16K entries :
           MAC[ 0:11] XOR
           MAC[12:23] XOR
           MAC[24:35] XOR
           MAC[36:47] XOR
           VID[11:0] */ /*  !!!! REVERSED VID !!!! */

        /* Note bits over bit 11 will be ignored in the transpose phase
            see use of numBits */

        /* mac address */
        u32Array[0] = (arEther[5]) | (arEther[4]  << 8);     /* mac 00..11 */
        u32Array[1] = (arEther[4] >> 4) | (arEther[3] << 4); /* mac 12..23 */
        u32Array[2] = (arEther[2]) | (arEther[1]  << 8);     /* mac 24..35 */
        u32Array[3] = (arEther[1] >> 4) | (arEther[0] << 4); /* mac 36..47 */
        u32Array[4] = 0;
        /* Vid */
        u32Array[5] = 0;
        u32Array[6] = swappedVidBits;               /* VID[10:0]  REVERSED !!!*/
    }
    else if (numOfBitsToUse == 11)
    {
        /* the hash function is :  */
        /*  for 8K entries :
           MAC[ 0:10] XOR
           MAC[11:21] XOR
           MAC[22:32] XOR
           MAC[33:43] XOR
           MAC[44:47] XOR
           (VID[11]<<10) XOR
           VID[10:0] */ /*  !!!! REVERSED VID !!!! */

        /* Note bits over bit 10 will be ignored in the transpose phase
            see use of numBits */

        /* mac address */
        u32Array[0] = arEther[5] | (arEther[4] << 8);           /* mac 00..10 */
        u32Array[1] = (arEther[4] >> 3) | (arEther[3] << 5);    /* mac 11..22 */
        u32Array[2] = (arEther[3] >> 6) | (arEther[2] << 2) |   /* mac 23..33 */
                      (arEther[1] << 10);
        u32Array[3] = (arEther[1] >> 1) | (arEther[0] << 7);    /* mac 34..44 */
        u32Array[4] = arEther[0] >> 4;                          /* mac 44..47 */

        /* Vid */
        u32Array[5] = (swappedVidBits & 1) << 10;   /* (VID[11]<<10) */
        u32Array[6] = swappedVidBits >> 1;          /* VID[10:0]  REVERSED !!!*/
    }
    else
    {/* numOfBitsToUse = 13 */

        /* the hash function is :  */
        /*  for 32K entries :
           MAC[ 0:12] XOR
           MAC[13:25] XOR
           MAC[26:38] XOR
           MAC[39:47] XOR
           VID[0]VID[11:0] */ /*  !!!! REVERSED VID !!!! */

        /* Note bits over bit 12 will be ignored in the transpose phase
            see use of numBits */

        /* mac address */
        u32Array[0] = (arEther[5]) | (arEther[4]  << 8);        /* mac 00..12 */
        u32Array[1] = (arEther[4] >> 5) | (arEther[3] << 3) |
                                            (arEther[2] << 11); /* mac 13..25 */
        u32Array[2] = (arEther[2] >> 2) | (arEther[1]  << 6);   /* mac 26..38 */
        u32Array[3] = (arEther[1] >> 7) | (arEther[0] << 1);    /* mac 39..47 */
        u32Array[4] = 0;
        /* Vid */
        /* vid of 13 bits is: 0 1 2 3 4 5 6 7 8 9 10 11 0 */
        swappedVidBits = ((swappedVidBits << 1) | (vid & 0x1));
        u32Array[5] = swappedVidBits;  /* VID[0]VID[11:0]  REVERSED !!!*/
        u32Array[6] = 0;
    }

    /* compute the transposed hash */
    transposedHash = 0;
    for (ii = 0; ii < 7; ii++)
    {
        transposedHash ^= u32Array[ii];
    }

    (*hashPtr) = 0;

    /* we need to transpose (reverse) the calculation so far */
    for(ii = 0 ;ii < numOfBitsToUse; ii++)
    {
        (*hashPtr) |= (transposedHash & (1 << ii)) ?
                   1 << (numOfBitsToUse - (ii + 1)) :
                   0;
    }

    return GT_OK;
}

/*******************************************************************************
* hashCalcCrc
*
* DESCRIPTION:
*       This function calculates the hash index for both mac address table and
*       ip address for specific VLAN id. Mac Address and IP address hush
*       functions use same CRC algorith.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dataPtr     - (pointer to) the address combined with VID.
*                          for MAC: VID and Mac address
*                          for IP:  VID, dip and sip.
*       numOfBitsToUse  - number of bits to use for MAC CRC hash algorithm
*
* OUTPUTS:
*       hashBitPtr - (pointer to) the hash bit array.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS hashCalcCrc(
    IN  GT_U32          *dataPtr,
    IN  GT_U32          numOfBitsToUse,
    OUT GT_U32          *hashBitPtr
)
{
    if (numOfBitsToUse < 13)
    {/* number of hash bits is 12 or 11 */

        hashBitPtr[0] =
                      PRV_GET_BIT_MAC(dataPtr[1],31) ^ PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^
                      PRV_GET_BIT_MAC(dataPtr[1],27) ^ PRV_GET_BIT_MAC(dataPtr[1],26) ^ PRV_GET_BIT_MAC(dataPtr[1],22) ^
                      PRV_GET_BIT_MAC(dataPtr[1],20) ^ PRV_GET_BIT_MAC(dataPtr[1],19) ^ PRV_GET_BIT_MAC(dataPtr[1],18) ^
                      PRV_GET_BIT_MAC(dataPtr[1],17) ^ PRV_GET_BIT_MAC(dataPtr[1],16) ^ PRV_GET_BIT_MAC(dataPtr[1],15) ^
                      PRV_GET_BIT_MAC(dataPtr[1],12) ^ PRV_GET_BIT_MAC(dataPtr[1],3)  ^ PRV_GET_BIT_MAC(dataPtr[1],2)  ^
                      PRV_GET_BIT_MAC(dataPtr[1],1)  ^ PRV_GET_BIT_MAC(dataPtr[0],30) ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^
                      PRV_GET_BIT_MAC(dataPtr[0],26) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^ PRV_GET_BIT_MAC(dataPtr[0],24) ^
                      PRV_GET_BIT_MAC(dataPtr[0],23) ^ PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],17) ^
                      PRV_GET_BIT_MAC(dataPtr[0],16) ^ PRV_GET_BIT_MAC(dataPtr[0],15) ^ PRV_GET_BIT_MAC(dataPtr[0],14) ^
                      PRV_GET_BIT_MAC(dataPtr[0],13) ^ PRV_GET_BIT_MAC(dataPtr[0],12) ^ PRV_GET_BIT_MAC(dataPtr[0],11) ^
                      PRV_GET_BIT_MAC(dataPtr[0],8)  ^ PRV_GET_BIT_MAC(dataPtr[0],7)  ^ PRV_GET_BIT_MAC(dataPtr[0],6)  ^
                      PRV_GET_BIT_MAC(dataPtr[0],5)  ^ PRV_GET_BIT_MAC(dataPtr[0],4)  ^ PRV_GET_BIT_MAC(dataPtr[0],3)  ^
                      PRV_GET_BIT_MAC(dataPtr[0],2)  ^ PRV_GET_BIT_MAC(dataPtr[0],1)  ^ PRV_GET_BIT_MAC(dataPtr[0],0);

        hashBitPtr[1] =
                      PRV_GET_BIT_MAC(dataPtr[1],31) ^
                      PRV_GET_BIT_MAC(dataPtr[1],30) ^ PRV_GET_BIT_MAC(dataPtr[1],26) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^
                      PRV_GET_BIT_MAC(dataPtr[1],22) ^ PRV_GET_BIT_MAC(dataPtr[1],21) ^ PRV_GET_BIT_MAC(dataPtr[1],15) ^
                      PRV_GET_BIT_MAC(dataPtr[1],13) ^ PRV_GET_BIT_MAC(dataPtr[1],12) ^ PRV_GET_BIT_MAC(dataPtr[1],4)  ^
                      PRV_GET_BIT_MAC(dataPtr[1],1)  ^ PRV_GET_BIT_MAC(dataPtr[0],31) ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^
                      PRV_GET_BIT_MAC(dataPtr[0],27) ^ PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],18) ^
                      PRV_GET_BIT_MAC(dataPtr[0],11) ^ PRV_GET_BIT_MAC(dataPtr[0],9)  ^ PRV_GET_BIT_MAC(dataPtr[0],0);

        hashBitPtr[2] =
                      PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^ PRV_GET_BIT_MAC(dataPtr[1],26) ^
                      PRV_GET_BIT_MAC(dataPtr[1],24) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^ PRV_GET_BIT_MAC(dataPtr[1],20) ^
                      PRV_GET_BIT_MAC(dataPtr[1],19) ^ PRV_GET_BIT_MAC(dataPtr[1],18) ^ PRV_GET_BIT_MAC(dataPtr[1],17) ^
                      PRV_GET_BIT_MAC(dataPtr[1],15) ^ PRV_GET_BIT_MAC(dataPtr[1],14) ^ PRV_GET_BIT_MAC(dataPtr[1],13) ^
                      PRV_GET_BIT_MAC(dataPtr[1],12) ^ PRV_GET_BIT_MAC(dataPtr[1],5)  ^ PRV_GET_BIT_MAC(dataPtr[1],3)  ^
                      PRV_GET_BIT_MAC(dataPtr[1],1)  ^ PRV_GET_BIT_MAC(dataPtr[1],0)  ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^
                      PRV_GET_BIT_MAC(dataPtr[0],28) ^ PRV_GET_BIT_MAC(dataPtr[0],26) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^
                      PRV_GET_BIT_MAC(dataPtr[0],24) ^ PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],19) ^
                      PRV_GET_BIT_MAC(dataPtr[0],17) ^ PRV_GET_BIT_MAC(dataPtr[0],16) ^ PRV_GET_BIT_MAC(dataPtr[0],15) ^
                      PRV_GET_BIT_MAC(dataPtr[0],14) ^ PRV_GET_BIT_MAC(dataPtr[0],13) ^ PRV_GET_BIT_MAC(dataPtr[0],11) ^
                      PRV_GET_BIT_MAC(dataPtr[0],10) ^ PRV_GET_BIT_MAC(dataPtr[0],8)  ^ PRV_GET_BIT_MAC(dataPtr[0],7)  ^
                      PRV_GET_BIT_MAC(dataPtr[0],6)  ^ PRV_GET_BIT_MAC(dataPtr[0],5)  ^ PRV_GET_BIT_MAC(dataPtr[0],4)  ^
                      PRV_GET_BIT_MAC(dataPtr[0],3)  ^ PRV_GET_BIT_MAC(dataPtr[0],2)  ^ PRV_GET_BIT_MAC(dataPtr[0],0);

        hashBitPtr[3] =
                      PRV_GET_BIT_MAC(dataPtr[1],31) ^
                      PRV_GET_BIT_MAC(dataPtr[1],30) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^ PRV_GET_BIT_MAC(dataPtr[1],26) ^
                      PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],24) ^ PRV_GET_BIT_MAC(dataPtr[1],22) ^
                      PRV_GET_BIT_MAC(dataPtr[1],21) ^ PRV_GET_BIT_MAC(dataPtr[1],17) ^ PRV_GET_BIT_MAC(dataPtr[1],14) ^
                      PRV_GET_BIT_MAC(dataPtr[1],13) ^ PRV_GET_BIT_MAC(dataPtr[1],12) ^ PRV_GET_BIT_MAC(dataPtr[1],6)  ^
                      PRV_GET_BIT_MAC(dataPtr[1],4)  ^ PRV_GET_BIT_MAC(dataPtr[1],3)  ^ PRV_GET_BIT_MAC(dataPtr[0],27) ^
                      PRV_GET_BIT_MAC(dataPtr[0],24) ^ PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],20) ^
                      PRV_GET_BIT_MAC(dataPtr[0],18) ^ PRV_GET_BIT_MAC(dataPtr[0],13) ^ PRV_GET_BIT_MAC(dataPtr[0],9)  ^
                      PRV_GET_BIT_MAC(dataPtr[0],2)  ^ PRV_GET_BIT_MAC(dataPtr[0],0)  ^
                      1;

        hashBitPtr[4] =
                      PRV_GET_BIT_MAC(dataPtr[1],31) ^ PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],27) ^
                      PRV_GET_BIT_MAC(dataPtr[1],26) ^ PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^
                      PRV_GET_BIT_MAC(dataPtr[1],22) ^ PRV_GET_BIT_MAC(dataPtr[1],18) ^ PRV_GET_BIT_MAC(dataPtr[1],15) ^
                      PRV_GET_BIT_MAC(dataPtr[1],14) ^ PRV_GET_BIT_MAC(dataPtr[1],13) ^ PRV_GET_BIT_MAC(dataPtr[1],7)  ^
                      PRV_GET_BIT_MAC(dataPtr[1],5)  ^ PRV_GET_BIT_MAC(dataPtr[1],4)  ^ PRV_GET_BIT_MAC(dataPtr[0],28) ^
                      PRV_GET_BIT_MAC(dataPtr[0],25) ^ PRV_GET_BIT_MAC(dataPtr[0],23) ^ PRV_GET_BIT_MAC(dataPtr[0],21) ^
                      PRV_GET_BIT_MAC(dataPtr[0],19) ^ PRV_GET_BIT_MAC(dataPtr[0],14) ^ PRV_GET_BIT_MAC(dataPtr[0],10) ^
                      PRV_GET_BIT_MAC(dataPtr[0],3)  ^ PRV_GET_BIT_MAC(dataPtr[0],1);

        hashBitPtr[5] =
                      PRV_GET_BIT_MAC(dataPtr[1],30) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^ PRV_GET_BIT_MAC(dataPtr[1],27) ^
                      PRV_GET_BIT_MAC(dataPtr[1],26) ^ PRV_GET_BIT_MAC(dataPtr[1],24) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^
                      PRV_GET_BIT_MAC(dataPtr[1],19) ^ PRV_GET_BIT_MAC(dataPtr[1],16) ^ PRV_GET_BIT_MAC(dataPtr[1],15) ^
                      PRV_GET_BIT_MAC(dataPtr[1],14) ^ PRV_GET_BIT_MAC(dataPtr[1],8)  ^ PRV_GET_BIT_MAC(dataPtr[1],6)  ^
                      PRV_GET_BIT_MAC(dataPtr[1],5)  ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^ PRV_GET_BIT_MAC(dataPtr[0],26) ^
                      PRV_GET_BIT_MAC(dataPtr[0],24) ^ PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],20) ^
                      PRV_GET_BIT_MAC(dataPtr[0],15) ^ PRV_GET_BIT_MAC(dataPtr[0],11) ^ PRV_GET_BIT_MAC(dataPtr[0],4)  ^
                      PRV_GET_BIT_MAC(dataPtr[0],2);

        hashBitPtr[6] =
                      PRV_GET_BIT_MAC(dataPtr[1],31) ^ PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^
                      PRV_GET_BIT_MAC(dataPtr[1],27) ^ PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],24) ^
                      PRV_GET_BIT_MAC(dataPtr[1],20) ^ PRV_GET_BIT_MAC(dataPtr[1],17) ^ PRV_GET_BIT_MAC(dataPtr[1],16) ^
                      PRV_GET_BIT_MAC(dataPtr[1],15) ^ PRV_GET_BIT_MAC(dataPtr[1],9)  ^ PRV_GET_BIT_MAC(dataPtr[1],7)  ^
                      PRV_GET_BIT_MAC(dataPtr[1],6)  ^ PRV_GET_BIT_MAC(dataPtr[0],30) ^ PRV_GET_BIT_MAC(dataPtr[0],27) ^
                      PRV_GET_BIT_MAC(dataPtr[0],25) ^ PRV_GET_BIT_MAC(dataPtr[0],23) ^ PRV_GET_BIT_MAC(dataPtr[0],21) ^
                      PRV_GET_BIT_MAC(dataPtr[0],16) ^ PRV_GET_BIT_MAC(dataPtr[0],12) ^ PRV_GET_BIT_MAC(dataPtr[0],5)  ^
                      PRV_GET_BIT_MAC(dataPtr[0],3);

        hashBitPtr[7] =
                      PRV_GET_BIT_MAC(dataPtr[1],30) ^ PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^
                      PRV_GET_BIT_MAC(dataPtr[1],26) ^ PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],21) ^
                      PRV_GET_BIT_MAC(dataPtr[1],18) ^ PRV_GET_BIT_MAC(dataPtr[1],17) ^ PRV_GET_BIT_MAC(dataPtr[1],16) ^
                      PRV_GET_BIT_MAC(dataPtr[1],10) ^ PRV_GET_BIT_MAC(dataPtr[1],8)  ^ PRV_GET_BIT_MAC(dataPtr[1],7)  ^
                      PRV_GET_BIT_MAC(dataPtr[0],31) ^ PRV_GET_BIT_MAC(dataPtr[0],28) ^ PRV_GET_BIT_MAC(dataPtr[0],26) ^
                      PRV_GET_BIT_MAC(dataPtr[0],24) ^ PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],17) ^
                      PRV_GET_BIT_MAC(dataPtr[0],13) ^ PRV_GET_BIT_MAC(dataPtr[0],6)  ^ PRV_GET_BIT_MAC(dataPtr[0],4);

        hashBitPtr[8] =
                      PRV_GET_BIT_MAC(dataPtr[1],31) ^ PRV_GET_BIT_MAC(dataPtr[1],30) ^ PRV_GET_BIT_MAC(dataPtr[1],29) ^
                      PRV_GET_BIT_MAC(dataPtr[1],27) ^ PRV_GET_BIT_MAC(dataPtr[1],26) ^ PRV_GET_BIT_MAC(dataPtr[1],22) ^
                      PRV_GET_BIT_MAC(dataPtr[1],19) ^ PRV_GET_BIT_MAC(dataPtr[1],18) ^ PRV_GET_BIT_MAC(dataPtr[1],17) ^
                      PRV_GET_BIT_MAC(dataPtr[1],11) ^ PRV_GET_BIT_MAC(dataPtr[1],9)  ^ PRV_GET_BIT_MAC(dataPtr[1],8)  ^
                      PRV_GET_BIT_MAC(dataPtr[1],0)  ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^ PRV_GET_BIT_MAC(dataPtr[0],27) ^
                      PRV_GET_BIT_MAC(dataPtr[0],25) ^ PRV_GET_BIT_MAC(dataPtr[0],23) ^ PRV_GET_BIT_MAC(dataPtr[0],18) ^
                      PRV_GET_BIT_MAC(dataPtr[0],14) ^ PRV_GET_BIT_MAC(dataPtr[0],7)  ^ PRV_GET_BIT_MAC(dataPtr[0],5);

        hashBitPtr[9] =
                      PRV_GET_BIT_MAC(dataPtr[1],31) ^ PRV_GET_BIT_MAC(dataPtr[1],30) ^
                      PRV_GET_BIT_MAC(dataPtr[1],28) ^ PRV_GET_BIT_MAC(dataPtr[1],27) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^
                      PRV_GET_BIT_MAC(dataPtr[1],20) ^ PRV_GET_BIT_MAC(dataPtr[1],19) ^ PRV_GET_BIT_MAC(dataPtr[1],18) ^
                      PRV_GET_BIT_MAC(dataPtr[1],12) ^ PRV_GET_BIT_MAC(dataPtr[1],10) ^ PRV_GET_BIT_MAC(dataPtr[1],9)  ^
                      PRV_GET_BIT_MAC(dataPtr[1],1)  ^ PRV_GET_BIT_MAC(dataPtr[0],30) ^ PRV_GET_BIT_MAC(dataPtr[0],28) ^
                      PRV_GET_BIT_MAC(dataPtr[0],26) ^ PRV_GET_BIT_MAC(dataPtr[0],24) ^ PRV_GET_BIT_MAC(dataPtr[0],19) ^
                      PRV_GET_BIT_MAC(dataPtr[0],15) ^ PRV_GET_BIT_MAC(dataPtr[0],8)  ^ PRV_GET_BIT_MAC(dataPtr[0],6)  ^
                      1;

        hashBitPtr[10] =
                       PRV_GET_BIT_MAC(dataPtr[1],31) ^
                       PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^ PRV_GET_BIT_MAC(dataPtr[1],24) ^
                       PRV_GET_BIT_MAC(dataPtr[1],21) ^ PRV_GET_BIT_MAC(dataPtr[1],20) ^ PRV_GET_BIT_MAC(dataPtr[1],19) ^
                       PRV_GET_BIT_MAC(dataPtr[1],13) ^ PRV_GET_BIT_MAC(dataPtr[1],11) ^ PRV_GET_BIT_MAC(dataPtr[1],10) ^
                       PRV_GET_BIT_MAC(dataPtr[1],2)  ^ PRV_GET_BIT_MAC(dataPtr[0],31) ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^
                       PRV_GET_BIT_MAC(dataPtr[0],27) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^ PRV_GET_BIT_MAC(dataPtr[0],20) ^
                       PRV_GET_BIT_MAC(dataPtr[0],16) ^ PRV_GET_BIT_MAC(dataPtr[0],9)  ^ PRV_GET_BIT_MAC(dataPtr[0],7);

        hashBitPtr[12] = 0;

        if (numOfBitsToUse == 12)
        {
            hashBitPtr[11] =
                           PRV_GET_BIT_MAC(dataPtr[1],31) ^ PRV_GET_BIT_MAC(dataPtr[1],30) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^
                           PRV_GET_BIT_MAC(dataPtr[1],27) ^ PRV_GET_BIT_MAC(dataPtr[1],26) ^ PRV_GET_BIT_MAC(dataPtr[1],25) ^
                           PRV_GET_BIT_MAC(dataPtr[1],21) ^ PRV_GET_BIT_MAC(dataPtr[1],19) ^ PRV_GET_BIT_MAC(dataPtr[1],18) ^
                           PRV_GET_BIT_MAC(dataPtr[1],17) ^ PRV_GET_BIT_MAC(dataPtr[1],16) ^ PRV_GET_BIT_MAC(dataPtr[1],15) ^
                           PRV_GET_BIT_MAC(dataPtr[1],14) ^ PRV_GET_BIT_MAC(dataPtr[1],11) ^ PRV_GET_BIT_MAC(dataPtr[1],2)  ^
                           PRV_GET_BIT_MAC(dataPtr[1],1)  ^ PRV_GET_BIT_MAC(dataPtr[1],0)  ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^
                           PRV_GET_BIT_MAC(dataPtr[0],28) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^ PRV_GET_BIT_MAC(dataPtr[0],24) ^
                           PRV_GET_BIT_MAC(dataPtr[0],23) ^ PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],21) ^
                           PRV_GET_BIT_MAC(dataPtr[0],16) ^ PRV_GET_BIT_MAC(dataPtr[0],15) ^ PRV_GET_BIT_MAC(dataPtr[0],14) ^
                           PRV_GET_BIT_MAC(dataPtr[0],13) ^ PRV_GET_BIT_MAC(dataPtr[0],12) ^ PRV_GET_BIT_MAC(dataPtr[0],11) ^
                           PRV_GET_BIT_MAC(dataPtr[0],10) ^ PRV_GET_BIT_MAC(dataPtr[0],7)  ^ PRV_GET_BIT_MAC(dataPtr[0],6)  ^
                           PRV_GET_BIT_MAC(dataPtr[0],5)  ^ PRV_GET_BIT_MAC(dataPtr[0],4)  ^ PRV_GET_BIT_MAC(dataPtr[0],3)  ^
                           PRV_GET_BIT_MAC(dataPtr[0],2)  ^ PRV_GET_BIT_MAC(dataPtr[0],1)  ^ PRV_GET_BIT_MAC(dataPtr[0],0);
        }
        else
        {
            hashBitPtr[11] = 0;
        }
    }

    else
    {
        /* number of hash bits is 13 */

         hashBitPtr[0] =
                     PRV_GET_BIT_MAC(dataPtr[1],31) ^ PRV_GET_BIT_MAC(dataPtr[1],30) ^
                     PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^ PRV_GET_BIT_MAC(dataPtr[1],26) ^
                     PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],24) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^
                     PRV_GET_BIT_MAC(dataPtr[1],20) ^ PRV_GET_BIT_MAC(dataPtr[1],13) ^ PRV_GET_BIT_MAC(dataPtr[1],11) ^
                     PRV_GET_BIT_MAC(dataPtr[1],10) ^ PRV_GET_BIT_MAC(dataPtr[1],9)  ^ PRV_GET_BIT_MAC(dataPtr[1],7)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],5)  ^ PRV_GET_BIT_MAC(dataPtr[1],3)  ^ PRV_GET_BIT_MAC(dataPtr[1],2)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],1)  ^ PRV_GET_BIT_MAC(dataPtr[1],0)  ^ PRV_GET_BIT_MAC(dataPtr[0],31) ^
                     PRV_GET_BIT_MAC(dataPtr[0],28) ^ PRV_GET_BIT_MAC(dataPtr[0],26) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^
                     PRV_GET_BIT_MAC(dataPtr[0],21) ^ PRV_GET_BIT_MAC(dataPtr[0],19) ^ PRV_GET_BIT_MAC(dataPtr[0],18) ^
                     PRV_GET_BIT_MAC(dataPtr[0],17) ^ PRV_GET_BIT_MAC(dataPtr[0],14) ^ PRV_GET_BIT_MAC(dataPtr[0],13) ^
                     PRV_GET_BIT_MAC(dataPtr[0],12) ^ PRV_GET_BIT_MAC(dataPtr[0],11) ^ PRV_GET_BIT_MAC(dataPtr[0],9)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],7)  ^ PRV_GET_BIT_MAC(dataPtr[0],6)  ^ PRV_GET_BIT_MAC(dataPtr[0],4)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],3)  ^ PRV_GET_BIT_MAC(dataPtr[0],1)  ^ PRV_GET_BIT_MAC(dataPtr[0],0);

         hashBitPtr[1] =
                     PRV_GET_BIT_MAC(dataPtr[1],28) ^
                     PRV_GET_BIT_MAC(dataPtr[1],27) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^ PRV_GET_BIT_MAC(dataPtr[1],21) ^
                     PRV_GET_BIT_MAC(dataPtr[1],20) ^ PRV_GET_BIT_MAC(dataPtr[1],14) ^ PRV_GET_BIT_MAC(dataPtr[1],13) ^
                     PRV_GET_BIT_MAC(dataPtr[1],12) ^ PRV_GET_BIT_MAC(dataPtr[1],9)  ^ PRV_GET_BIT_MAC(dataPtr[1],8)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],7)  ^ PRV_GET_BIT_MAC(dataPtr[1],6)  ^ PRV_GET_BIT_MAC(dataPtr[1],5)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],4)  ^ PRV_GET_BIT_MAC(dataPtr[0],31) ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^
                     PRV_GET_BIT_MAC(dataPtr[0],28) ^ PRV_GET_BIT_MAC(dataPtr[0],27) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^
                     PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],21) ^ PRV_GET_BIT_MAC(dataPtr[0],20) ^
                     PRV_GET_BIT_MAC(dataPtr[0],17) ^ PRV_GET_BIT_MAC(dataPtr[0],15) ^ PRV_GET_BIT_MAC(dataPtr[0],11) ^
                     PRV_GET_BIT_MAC(dataPtr[0],10) ^ PRV_GET_BIT_MAC(dataPtr[0],9)  ^ PRV_GET_BIT_MAC(dataPtr[0],8)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],6)  ^ PRV_GET_BIT_MAC(dataPtr[0],5)  ^ PRV_GET_BIT_MAC(dataPtr[0],3)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],2)  ^ PRV_GET_BIT_MAC(dataPtr[0],0);

         hashBitPtr[2] =
                     PRV_GET_BIT_MAC(dataPtr[1],31) ^ PRV_GET_BIT_MAC(dataPtr[1],30) ^
                     PRV_GET_BIT_MAC(dataPtr[1],26) ^ PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^
                     PRV_GET_BIT_MAC(dataPtr[1],22) ^ PRV_GET_BIT_MAC(dataPtr[1],21) ^ PRV_GET_BIT_MAC(dataPtr[1],20) ^
                     PRV_GET_BIT_MAC(dataPtr[1],15) ^ PRV_GET_BIT_MAC(dataPtr[1],14) ^ PRV_GET_BIT_MAC(dataPtr[1],11) ^
                     PRV_GET_BIT_MAC(dataPtr[1],8)  ^ PRV_GET_BIT_MAC(dataPtr[1],6)  ^ PRV_GET_BIT_MAC(dataPtr[1],3)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],2)  ^ PRV_GET_BIT_MAC(dataPtr[1],1)  ^ PRV_GET_BIT_MAC(dataPtr[0],31) ^
                     PRV_GET_BIT_MAC(dataPtr[0],30) ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^
                     PRV_GET_BIT_MAC(dataPtr[0],23) ^ PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],19) ^
                     PRV_GET_BIT_MAC(dataPtr[0],17) ^ PRV_GET_BIT_MAC(dataPtr[0],16) ^ PRV_GET_BIT_MAC(dataPtr[0],14) ^
                     PRV_GET_BIT_MAC(dataPtr[0],13) ^ PRV_GET_BIT_MAC(dataPtr[0],10) ^ PRV_GET_BIT_MAC(dataPtr[0],0);

         hashBitPtr[3] =
                     PRV_GET_BIT_MAC(dataPtr[1],30) ^ PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^
                     PRV_GET_BIT_MAC(dataPtr[1],27) ^ PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],22) ^
                     PRV_GET_BIT_MAC(dataPtr[1],21) ^ PRV_GET_BIT_MAC(dataPtr[1],20) ^ PRV_GET_BIT_MAC(dataPtr[1],16) ^
                     PRV_GET_BIT_MAC(dataPtr[1],15) ^ PRV_GET_BIT_MAC(dataPtr[1],13) ^ PRV_GET_BIT_MAC(dataPtr[1],12) ^
                     PRV_GET_BIT_MAC(dataPtr[1],11) ^ PRV_GET_BIT_MAC(dataPtr[1],10) ^ PRV_GET_BIT_MAC(dataPtr[1],5)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],4)  ^ PRV_GET_BIT_MAC(dataPtr[1],1)  ^ PRV_GET_BIT_MAC(dataPtr[0],30) ^
                     PRV_GET_BIT_MAC(dataPtr[0],28) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^ PRV_GET_BIT_MAC(dataPtr[0],24) ^
                     PRV_GET_BIT_MAC(dataPtr[0],23) ^ PRV_GET_BIT_MAC(dataPtr[0],21) ^ PRV_GET_BIT_MAC(dataPtr[0],20) ^
                     PRV_GET_BIT_MAC(dataPtr[0],19) ^ PRV_GET_BIT_MAC(dataPtr[0],15) ^ PRV_GET_BIT_MAC(dataPtr[0],13) ^
                     PRV_GET_BIT_MAC(dataPtr[0],12) ^ PRV_GET_BIT_MAC(dataPtr[0],9)  ^ PRV_GET_BIT_MAC(dataPtr[0],7)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],6)  ^ PRV_GET_BIT_MAC(dataPtr[0],4)  ^ PRV_GET_BIT_MAC(dataPtr[0],3)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],0);

         hashBitPtr[4] =
                     PRV_GET_BIT_MAC(dataPtr[1],31) ^
                     PRV_GET_BIT_MAC(dataPtr[1],30) ^ PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^
                     PRV_GET_BIT_MAC(dataPtr[1],26) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^ PRV_GET_BIT_MAC(dataPtr[1],22) ^
                     PRV_GET_BIT_MAC(dataPtr[1],21) ^ PRV_GET_BIT_MAC(dataPtr[1],17) ^ PRV_GET_BIT_MAC(dataPtr[1],16) ^
                     PRV_GET_BIT_MAC(dataPtr[1],14) ^ PRV_GET_BIT_MAC(dataPtr[1],13) ^ PRV_GET_BIT_MAC(dataPtr[1],12) ^
                     PRV_GET_BIT_MAC(dataPtr[1],11) ^ PRV_GET_BIT_MAC(dataPtr[1],6)  ^ PRV_GET_BIT_MAC(dataPtr[1],5)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],2)  ^ PRV_GET_BIT_MAC(dataPtr[0],31) ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^
                     PRV_GET_BIT_MAC(dataPtr[0],26) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^ PRV_GET_BIT_MAC(dataPtr[0],24) ^
                     PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],21) ^ PRV_GET_BIT_MAC(dataPtr[0],20) ^
                     PRV_GET_BIT_MAC(dataPtr[0],16) ^ PRV_GET_BIT_MAC(dataPtr[0],14) ^ PRV_GET_BIT_MAC(dataPtr[0],13) ^
                     PRV_GET_BIT_MAC(dataPtr[0],10) ^ PRV_GET_BIT_MAC(dataPtr[0],8)  ^ PRV_GET_BIT_MAC(dataPtr[0],7)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],5)  ^ PRV_GET_BIT_MAC(dataPtr[0],4)  ^ PRV_GET_BIT_MAC(dataPtr[0],1);

         hashBitPtr[5] =
                     PRV_GET_BIT_MAC(dataPtr[1],31) ^
                     PRV_GET_BIT_MAC(dataPtr[1],30) ^ PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],27) ^
                     PRV_GET_BIT_MAC(dataPtr[1],24)  ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^ PRV_GET_BIT_MAC(dataPtr[1],22) ^
                     PRV_GET_BIT_MAC(dataPtr[1],18)  ^ PRV_GET_BIT_MAC(dataPtr[1],17) ^ PRV_GET_BIT_MAC(dataPtr[1],15) ^
                     PRV_GET_BIT_MAC(dataPtr[1],14)  ^ PRV_GET_BIT_MAC(dataPtr[1],13) ^ PRV_GET_BIT_MAC(dataPtr[1],12) ^
                     PRV_GET_BIT_MAC(dataPtr[1],7)   ^ PRV_GET_BIT_MAC(dataPtr[1],6)  ^ PRV_GET_BIT_MAC(dataPtr[1],3)^
                     PRV_GET_BIT_MAC(dataPtr[1],0)   ^ PRV_GET_BIT_MAC(dataPtr[0],30) ^ PRV_GET_BIT_MAC(dataPtr[0],27) ^
                     PRV_GET_BIT_MAC(dataPtr[0],26)  ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^ PRV_GET_BIT_MAC(dataPtr[0],23) ^
                     PRV_GET_BIT_MAC(dataPtr[0],22)  ^ PRV_GET_BIT_MAC(dataPtr[0],21) ^ PRV_GET_BIT_MAC(dataPtr[0],17) ^
                     PRV_GET_BIT_MAC(dataPtr[0],15)  ^ PRV_GET_BIT_MAC(dataPtr[0],14) ^ PRV_GET_BIT_MAC(dataPtr[0],11) ^
                     PRV_GET_BIT_MAC(dataPtr[0],9)   ^ PRV_GET_BIT_MAC(dataPtr[0],8)  ^ PRV_GET_BIT_MAC(dataPtr[0],6) ^
                     PRV_GET_BIT_MAC(dataPtr[0],5)   ^ PRV_GET_BIT_MAC(dataPtr[0],2);

         hashBitPtr[6] =
                     PRV_GET_BIT_MAC(dataPtr[1],31) ^ PRV_GET_BIT_MAC(dataPtr[1],30) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^
                     PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],24) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^
                     PRV_GET_BIT_MAC(dataPtr[1],19) ^ PRV_GET_BIT_MAC(dataPtr[1],18) ^ PRV_GET_BIT_MAC(dataPtr[1],16) ^
                     PRV_GET_BIT_MAC(dataPtr[1],15) ^ PRV_GET_BIT_MAC(dataPtr[1],14) ^ PRV_GET_BIT_MAC(dataPtr[1],13) ^
                     PRV_GET_BIT_MAC(dataPtr[1],8)  ^ PRV_GET_BIT_MAC(dataPtr[1],7)  ^ PRV_GET_BIT_MAC(dataPtr[1],4)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],1)  ^ PRV_GET_BIT_MAC(dataPtr[0],31) ^ PRV_GET_BIT_MAC(dataPtr[0],28) ^
                     PRV_GET_BIT_MAC(dataPtr[0],27) ^ PRV_GET_BIT_MAC(dataPtr[0],26) ^ PRV_GET_BIT_MAC(dataPtr[0],24) ^
                     PRV_GET_BIT_MAC(dataPtr[0],23) ^ PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],18) ^
                     PRV_GET_BIT_MAC(dataPtr[0],16) ^ PRV_GET_BIT_MAC(dataPtr[0],15) ^ PRV_GET_BIT_MAC(dataPtr[0],12) ^
                     PRV_GET_BIT_MAC(dataPtr[0],10) ^ PRV_GET_BIT_MAC(dataPtr[0],9)  ^ PRV_GET_BIT_MAC(dataPtr[0],7)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],6)  ^ PRV_GET_BIT_MAC(dataPtr[0],3)  ^ 1;

         hashBitPtr[7] =
                     PRV_GET_BIT_MAC(dataPtr[1],31) ^ PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],26) ^
                     PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],24) ^ PRV_GET_BIT_MAC(dataPtr[1],20) ^
                     PRV_GET_BIT_MAC(dataPtr[1],19) ^ PRV_GET_BIT_MAC(dataPtr[1],17) ^ PRV_GET_BIT_MAC(dataPtr[1],16) ^
                     PRV_GET_BIT_MAC(dataPtr[1],15) ^ PRV_GET_BIT_MAC(dataPtr[1],14) ^ PRV_GET_BIT_MAC(dataPtr[1],9)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],8)  ^ PRV_GET_BIT_MAC(dataPtr[1],5)  ^ PRV_GET_BIT_MAC(dataPtr[1],2)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],0)  ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^ PRV_GET_BIT_MAC(dataPtr[0],28) ^
                     PRV_GET_BIT_MAC(dataPtr[0],27) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^ PRV_GET_BIT_MAC(dataPtr[0],24) ^
                     PRV_GET_BIT_MAC(dataPtr[0],23) ^ PRV_GET_BIT_MAC(dataPtr[0],19) ^ PRV_GET_BIT_MAC(dataPtr[0],17) ^
                     PRV_GET_BIT_MAC(dataPtr[0],16) ^ PRV_GET_BIT_MAC(dataPtr[0],13) ^ PRV_GET_BIT_MAC(dataPtr[0],11) ^
                     PRV_GET_BIT_MAC(dataPtr[0],10) ^ PRV_GET_BIT_MAC(dataPtr[0],8)  ^ PRV_GET_BIT_MAC(dataPtr[0],7)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],4)  ^ 1;

         hashBitPtr[8] =
                     PRV_GET_BIT_MAC(dataPtr[1],30) ^ PRV_GET_BIT_MAC(dataPtr[1],27) ^
                     PRV_GET_BIT_MAC(dataPtr[1],26) ^ PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],21) ^
                     PRV_GET_BIT_MAC(dataPtr[1],20) ^ PRV_GET_BIT_MAC(dataPtr[1],18) ^ PRV_GET_BIT_MAC(dataPtr[1],17) ^
                     PRV_GET_BIT_MAC(dataPtr[1],16) ^ PRV_GET_BIT_MAC(dataPtr[1],15) ^ PRV_GET_BIT_MAC(dataPtr[1],10) ^
                     PRV_GET_BIT_MAC(dataPtr[1],9)  ^ PRV_GET_BIT_MAC(dataPtr[1],6)  ^ PRV_GET_BIT_MAC(dataPtr[1],3)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],1)  ^ PRV_GET_BIT_MAC(dataPtr[0],30) ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^
                     PRV_GET_BIT_MAC(dataPtr[0],28) ^ PRV_GET_BIT_MAC(dataPtr[0],26) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^
                     PRV_GET_BIT_MAC(dataPtr[0],24) ^ PRV_GET_BIT_MAC(dataPtr[0],20) ^ PRV_GET_BIT_MAC(dataPtr[0],18) ^
                     PRV_GET_BIT_MAC(dataPtr[0],17) ^ PRV_GET_BIT_MAC(dataPtr[0],14) ^ PRV_GET_BIT_MAC(dataPtr[0],12) ^
                     PRV_GET_BIT_MAC(dataPtr[0],11) ^ PRV_GET_BIT_MAC(dataPtr[0],9)  ^ PRV_GET_BIT_MAC(dataPtr[0],8)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],5)  ^ 1;

         hashBitPtr[9] =
                     PRV_GET_BIT_MAC(dataPtr[1],31)^ PRV_GET_BIT_MAC(dataPtr[1],28) ^ PRV_GET_BIT_MAC(dataPtr[1],27) ^
                     PRV_GET_BIT_MAC(dataPtr[1],26)^ PRV_GET_BIT_MAC(dataPtr[1],22) ^ PRV_GET_BIT_MAC(dataPtr[1],21) ^
                     PRV_GET_BIT_MAC(dataPtr[1],19)^ PRV_GET_BIT_MAC(dataPtr[1],18) ^ PRV_GET_BIT_MAC(dataPtr[1],17) ^
                     PRV_GET_BIT_MAC(dataPtr[1],16)^ PRV_GET_BIT_MAC(dataPtr[1],11) ^ PRV_GET_BIT_MAC(dataPtr[1],10) ^
                     PRV_GET_BIT_MAC(dataPtr[1],7) ^ PRV_GET_BIT_MAC(dataPtr[1],4)  ^ PRV_GET_BIT_MAC(dataPtr[1],2)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],31)^ PRV_GET_BIT_MAC(dataPtr[0],30) ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^
                     PRV_GET_BIT_MAC(dataPtr[0],27)^ PRV_GET_BIT_MAC(dataPtr[0],26) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^
                     PRV_GET_BIT_MAC(dataPtr[0],21)^ PRV_GET_BIT_MAC(dataPtr[0],19) ^ PRV_GET_BIT_MAC(dataPtr[0],18) ^
                     PRV_GET_BIT_MAC(dataPtr[0],15)^ PRV_GET_BIT_MAC(dataPtr[0],13) ^ PRV_GET_BIT_MAC(dataPtr[0],12) ^
                     PRV_GET_BIT_MAC(dataPtr[0],10)^ PRV_GET_BIT_MAC(dataPtr[0],9)  ^ PRV_GET_BIT_MAC(dataPtr[0],6)  ^
                     1;

         hashBitPtr[10] =
                     PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^
                     PRV_GET_BIT_MAC(dataPtr[1],27) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^ PRV_GET_BIT_MAC(dataPtr[1],22) ^
                     PRV_GET_BIT_MAC(dataPtr[1],20) ^ PRV_GET_BIT_MAC(dataPtr[1],19) ^ PRV_GET_BIT_MAC(dataPtr[1],18) ^
                     PRV_GET_BIT_MAC(dataPtr[1],17) ^ PRV_GET_BIT_MAC(dataPtr[1],12) ^ PRV_GET_BIT_MAC(dataPtr[1],11) ^
                     PRV_GET_BIT_MAC(dataPtr[1],8)  ^ PRV_GET_BIT_MAC(dataPtr[1],5)  ^ PRV_GET_BIT_MAC(dataPtr[1],3)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],0)  ^ PRV_GET_BIT_MAC(dataPtr[0],31) ^ PRV_GET_BIT_MAC(dataPtr[0],30) ^
                     PRV_GET_BIT_MAC(dataPtr[0],28) ^ PRV_GET_BIT_MAC(dataPtr[0],27) ^ PRV_GET_BIT_MAC(dataPtr[0],26) ^
                     PRV_GET_BIT_MAC(dataPtr[0],22) ^ PRV_GET_BIT_MAC(dataPtr[0],20) ^ PRV_GET_BIT_MAC(dataPtr[0],19) ^
                     PRV_GET_BIT_MAC(dataPtr[0],16) ^ PRV_GET_BIT_MAC(dataPtr[0],14) ^ PRV_GET_BIT_MAC(dataPtr[0],13) ^
                     PRV_GET_BIT_MAC(dataPtr[0],11) ^ PRV_GET_BIT_MAC(dataPtr[0],10) ^ PRV_GET_BIT_MAC(dataPtr[0],7)  ^
                     1;

         hashBitPtr[11] =
                     PRV_GET_BIT_MAC(dataPtr[1],31)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],26) ^ PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],21) ^
                     PRV_GET_BIT_MAC(dataPtr[1],19) ^ PRV_GET_BIT_MAC(dataPtr[1],18) ^ PRV_GET_BIT_MAC(dataPtr[1],12) ^
                     PRV_GET_BIT_MAC(dataPtr[1],11) ^ PRV_GET_BIT_MAC(dataPtr[1],10) ^ PRV_GET_BIT_MAC(dataPtr[1],7)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],6)  ^ PRV_GET_BIT_MAC(dataPtr[1],5)  ^ PRV_GET_BIT_MAC(dataPtr[1],4)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],3)  ^ PRV_GET_BIT_MAC(dataPtr[1],2)  ^ PRV_GET_BIT_MAC(dataPtr[0],29) ^
                     PRV_GET_BIT_MAC(dataPtr[0],27) ^ PRV_GET_BIT_MAC(dataPtr[0],26) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^
                     PRV_GET_BIT_MAC(dataPtr[0],23) ^ PRV_GET_BIT_MAC(dataPtr[0],20) ^ PRV_GET_BIT_MAC(dataPtr[0],19) ^
                     PRV_GET_BIT_MAC(dataPtr[0],18) ^ PRV_GET_BIT_MAC(dataPtr[0],15) ^ PRV_GET_BIT_MAC(dataPtr[0],13) ^
                     PRV_GET_BIT_MAC(dataPtr[0],9)  ^ PRV_GET_BIT_MAC(dataPtr[0],8)  ^ PRV_GET_BIT_MAC(dataPtr[0],7)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],6)  ^ PRV_GET_BIT_MAC(dataPtr[0],4)  ^ PRV_GET_BIT_MAC(dataPtr[0],3)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],1)  ^ PRV_GET_BIT_MAC(dataPtr[0],0)  ^ 1;

         hashBitPtr[12] =
                     PRV_GET_BIT_MAC(dataPtr[1],31) ^ PRV_GET_BIT_MAC(dataPtr[1],30) ^
                     PRV_GET_BIT_MAC(dataPtr[1],29) ^ PRV_GET_BIT_MAC(dataPtr[1],28) ^ PRV_GET_BIT_MAC(dataPtr[1],27) ^
                     PRV_GET_BIT_MAC(dataPtr[1],25) ^ PRV_GET_BIT_MAC(dataPtr[1],24) ^ PRV_GET_BIT_MAC(dataPtr[1],23) ^
                     PRV_GET_BIT_MAC(dataPtr[1],22) ^ PRV_GET_BIT_MAC(dataPtr[1],19) ^ PRV_GET_BIT_MAC(dataPtr[1],12) ^
                     PRV_GET_BIT_MAC(dataPtr[1],10) ^ PRV_GET_BIT_MAC(dataPtr[1],9)  ^ PRV_GET_BIT_MAC(dataPtr[1],8)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],6)  ^ PRV_GET_BIT_MAC(dataPtr[1],4)  ^ PRV_GET_BIT_MAC(dataPtr[1],2)  ^
                     PRV_GET_BIT_MAC(dataPtr[1],1)  ^ PRV_GET_BIT_MAC(dataPtr[1],0)  ^ PRV_GET_BIT_MAC(dataPtr[0],31) ^
                     PRV_GET_BIT_MAC(dataPtr[0],30) ^ PRV_GET_BIT_MAC(dataPtr[0],27) ^ PRV_GET_BIT_MAC(dataPtr[0],25) ^
                     PRV_GET_BIT_MAC(dataPtr[0],24) ^ PRV_GET_BIT_MAC(dataPtr[0],20) ^ PRV_GET_BIT_MAC(dataPtr[0],18) ^
                     PRV_GET_BIT_MAC(dataPtr[0],17) ^ PRV_GET_BIT_MAC(dataPtr[0],16) ^ PRV_GET_BIT_MAC(dataPtr[0],13) ^
                     PRV_GET_BIT_MAC(dataPtr[0],12) ^ PRV_GET_BIT_MAC(dataPtr[0],11) ^ PRV_GET_BIT_MAC(dataPtr[0],10) ^
                     PRV_GET_BIT_MAC(dataPtr[0],8)  ^ PRV_GET_BIT_MAC(dataPtr[0],6)  ^ PRV_GET_BIT_MAC(dataPtr[0],5)  ^
                     PRV_GET_BIT_MAC(dataPtr[0],3)  ^ PRV_GET_BIT_MAC(dataPtr[0],2)  ^ PRV_GET_BIT_MAC(dataPtr[0],0);
    }

    return GT_OK;
}

/*******************************************************************************
* macHashCalcCrc
*
* DESCRIPTION:
*       This function calculates the hash index for the mac address table.
*       for specific mac address and VLAN id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       addrPtr         - (pointer to) the mac address.
*       vid             - the VLAN id.
*       numOfBitsToUse  - number of bits to use for MAC CRC hash algorithm
*
* OUTPUTS:
*       hashPtr - (pointer to) the hash index.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS macHashCalcCrc(
    IN  GT_ETHERADDR    *addrPtr,
    IN  GT_U16          vid,
    IN  GT_U32          numOfBitsToUse,
    OUT GT_U32          *hashPtr
)
{
    GT_U32  dataPtr[3];
    GT_U32  hashBitPtr[13];
    GT_U32  lowWordMacAddress = GT_HW_MAC_LOW32(addrPtr);/* low 32 bits of mac
                                                         address */
    GT_U32  hiHalfWordMacAddress = GT_HW_MAC_HIGH16(addrPtr);/* hi 16 bits of mac
                                                         address*/
    GT_STATUS   rc;
    GT_U32      ii; /* iterator */

    dataPtr[0] = (lowWordMacAddress    << 12) | vid;
    dataPtr[1] = (hiHalfWordMacAddress << 12) | (lowWordMacAddress >> 20);
    dataPtr[2] = 0;

    rc = hashCalcCrc(dataPtr, numOfBitsToUse, hashBitPtr);
    if (rc != GT_OK)
    {
        return rc;
    }


    (*hashPtr) = 0;

    for(ii = 0 ; ii < 13; ii++)
    {
        (*hashPtr) |= (hashBitPtr[ii] & 0x01) << ii;
    }


    return GT_OK;
}

/*******************************************************************************
* macHashCalc
*
* DESCRIPTION:
*       This function calculates the hash index for the mac address table.
*       for specific mac address and VLAN id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       addrPtr     - (pointer to) the mac address.
*       vid         - the VLAN id.
*
* OUTPUTS:
*       hashPtr - (pointer to) the hash index.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_STATE - Bad value of mac hash info parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS macHashCalc
(
    IN  GT_U8           devNum,
    IN  GT_ETHERADDR    *addrPtr,
    IN  GT_U16          vid,
    OUT GT_U32          *hashPtr
)
{
    GT_STATUS           rc;
    GT_U32              numOfBitsToUse;

    switch (PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.size)
    {
    case PRV_CPSS_DXCH_MAC_TBL_SIZE_8K_E:
        numOfBitsToUse = 11;
        break;

    case PRV_CPSS_DXCH_MAC_TBL_SIZE_16K_E:
        numOfBitsToUse = 12;
        break;

    case PRV_CPSS_DXCH_MAC_TBL_SIZE_32K_E:
        numOfBitsToUse = 13;
        break;

    default:
        return GT_BAD_STATE;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.vlanMode ==
        CPSS_SVL_E)
    {
        vid = 0;
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode ==
                                                CPSS_MAC_HASH_FUNC_XOR_E)
    {
        /* XOR hash function */
        rc = macHashCalcXor(devNum, addrPtr,vid,numOfBitsToUse,hashPtr);
    }
    else if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode ==
                                                CPSS_MAC_HASH_FUNC_CRC_E)
    {
        /* CRC hash function */
        rc = macHashCalcCrc(addrPtr,vid,numOfBitsToUse,hashPtr);
    }
    else
    {
        return GT_BAD_PARAM;
    }

    /* the hash START index DOES NOT depend on the macChainLen !!! */
    (*hashPtr) *= 4;/* 4 is HARD CODED !!!!! - do not change */

    return rc;
}

/*******************************************************************************
* ipHashCalcCrc
*
* DESCRIPTION:
*       !!! hashing in Cheetah devices --- CRC hashing !!!
*       This function calculates the hash index for the mac address table.
*       for specific DIP, SIP and VLAN id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vlanId          - the VLAN id.
*       dip             - destination IP
*       sip             - sources IP
*       numOfBitsToUse  - Number of bits to use for IP Hash CRC algorithm
*
* OUTPUTS:
*       hashPtr - (pointer to) the hash index.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipHashCalcCrc
(
    IN GT_U16   vlanId,
    IN GT_U32   dip,
    IN GT_U32   sip,
    IN GT_U32   numOfBitsToUse,
    OUT GT_U32  *hashPtr
)
{

    GT_U32  hashBitPtr[13];
    GT_U32  dataPtr[3];
    /* GT_U32  C = 0xfff; => Even number of PRV_GET_BIT_MAC from variable C can be ommited
                             and odd number can be replace with 0x1 */

    GT_U32      ii;/* iterator */
    GT_STATUS   rc;

    dataPtr[0] = (vlanId & 0xFFF) | ((dip & 0xFFFFF) << 12); /* VID[11:0] | (dip[19:0] << 12) */
    dataPtr[1] = ((dip >> 20) & 0xFFF) | ((sip & 0xFFFFF) << 12); /* dip[31:20] | (sip[19:0] << 12) */
    dataPtr[2] = (sip >> 20) & 0xFFF; /* sip[31:20] */

    rc = hashCalcCrc(dataPtr, numOfBitsToUse, hashBitPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (numOfBitsToUse < 13)
    {/* number of hash bits is 12 or 11 */

        hashBitPtr[0] ^= PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],8) ^ PRV_GET_BIT_MAC(dataPtr[2],6)  ^
                       PRV_GET_BIT_MAC(dataPtr[2],5)  ^ PRV_GET_BIT_MAC(dataPtr[2],2)  ^ PRV_GET_BIT_MAC(dataPtr[2],0);
                       /* 0 = PRV_GET_BIT_MAC(C,0)     ^ PRV_GET_BIT_MAC(C,2)     ^ PRV_GET_BIT_MAC(C,5)     ^ PRV_GET_BIT_MAC(C,6)     ^
                       PRV_GET_BIT_MAC(C,8)     ^ PRV_GET_BIT_MAC(C,11); */

        hashBitPtr[1] ^= PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],9) ^ PRV_GET_BIT_MAC(dataPtr[2],8) ^
                       PRV_GET_BIT_MAC(dataPtr[2],7) ^ PRV_GET_BIT_MAC(dataPtr[2],5) ^ PRV_GET_BIT_MAC(dataPtr[2],3) ^ PRV_GET_BIT_MAC(dataPtr[2],2) ^
                       PRV_GET_BIT_MAC(dataPtr[2],1);
                       /* 0 = PRV_GET_BIT_MAC(C,1) ^
                       PRV_GET_BIT_MAC(C,2) ^ PRV_GET_BIT_MAC(C,3) ^ PRV_GET_BIT_MAC(C,5) ^ PRV_GET_BIT_MAC(C,7) ^
                       PRV_GET_BIT_MAC(C,8) ^ PRV_GET_BIT_MAC(C,9) ^ PRV_GET_BIT_MAC(C,11); */

        hashBitPtr[2] ^=  PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],10) ^ PRV_GET_BIT_MAC(dataPtr[2],9) ^
                        PRV_GET_BIT_MAC(dataPtr[2],5) ^ PRV_GET_BIT_MAC(dataPtr[2],4) ^ PRV_GET_BIT_MAC(dataPtr[2],3);
                        /* 0 = PRV_GET_BIT_MAC(C,3) ^
                        PRV_GET_BIT_MAC(C,4) ^ PRV_GET_BIT_MAC(C,5) ^ PRV_GET_BIT_MAC(C,9) ^ PRV_GET_BIT_MAC(C,10) ^
                        PRV_GET_BIT_MAC(C,11); */

        hashBitPtr[3] ^=  PRV_GET_BIT_MAC(dataPtr[2],10) ^ PRV_GET_BIT_MAC(dataPtr[2],8) ^ PRV_GET_BIT_MAC(dataPtr[2],4) ^
                        PRV_GET_BIT_MAC(dataPtr[2],2) ^ PRV_GET_BIT_MAC(dataPtr[2],0);
                        /* 0x1 = PRV_GET_BIT_MAC(C,0) ^ PRV_GET_BIT_MAC(C,2) ^ PRV_GET_BIT_MAC(C,4) ^
                        PRV_GET_BIT_MAC(C,8) ^ PRV_GET_BIT_MAC(C,10); */

        hashBitPtr[4] ^=  PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],9) ^ PRV_GET_BIT_MAC(dataPtr[2],5) ^
                        PRV_GET_BIT_MAC(dataPtr[2],3) ^ PRV_GET_BIT_MAC(dataPtr[2],1) ^ PRV_GET_BIT_MAC(dataPtr[2],0);
                        /*  0 = PRV_GET_BIT_MAC(C,0) ^ PRV_GET_BIT_MAC(C,1) ^ PRV_GET_BIT_MAC(C,3) ^
                        PRV_GET_BIT_MAC(C,5) ^ PRV_GET_BIT_MAC(C,9) ^ PRV_GET_BIT_MAC(C,11); */

        hashBitPtr[5] ^= PRV_GET_BIT_MAC(dataPtr[2],10) ^ PRV_GET_BIT_MAC(dataPtr[2],6) ^ PRV_GET_BIT_MAC(dataPtr[2],4) ^
                    PRV_GET_BIT_MAC(dataPtr[2],2) ^ PRV_GET_BIT_MAC(dataPtr[2],1) ^ PRV_GET_BIT_MAC(dataPtr[2],0);
                    /* 0 = PRV_GET_BIT_MAC(C,0) ^ PRV_GET_BIT_MAC(C,1) ^ PRV_GET_BIT_MAC(C,2) ^ PRV_GET_BIT_MAC(C,4) ^
                    PRV_GET_BIT_MAC(C,6) ^ PRV_GET_BIT_MAC(C,10); */

        hashBitPtr[6] ^=  PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],7) ^ PRV_GET_BIT_MAC(dataPtr[2],5) ^
                        PRV_GET_BIT_MAC(dataPtr[2],3) ^ PRV_GET_BIT_MAC(dataPtr[2],2) ^ PRV_GET_BIT_MAC(dataPtr[2],1);
                        /* 0 = PRV_GET_BIT_MAC(C,1) ^ PRV_GET_BIT_MAC(C,2) ^ PRV_GET_BIT_MAC(C,3) ^ PRV_GET_BIT_MAC(C,5) ^
                        PRV_GET_BIT_MAC(C,7) ^ PRV_GET_BIT_MAC(C,11); */

        hashBitPtr[7] ^=  PRV_GET_BIT_MAC(dataPtr[2],8) ^ PRV_GET_BIT_MAC(dataPtr[2],6) ^ PRV_GET_BIT_MAC(dataPtr[2],4) ^
                        PRV_GET_BIT_MAC(dataPtr[2],3) ^ PRV_GET_BIT_MAC(dataPtr[2],2) ^ PRV_GET_BIT_MAC(dataPtr[2],0);
                        /* 0 = PRV_GET_BIT_MAC(C,0) ^
                        PRV_GET_BIT_MAC(C,2) ^ PRV_GET_BIT_MAC(C,3) ^ PRV_GET_BIT_MAC(C,4) ^ PRV_GET_BIT_MAC(C,6) ^ PRV_GET_BIT_MAC(C,8); */

        hashBitPtr[8] ^=  PRV_GET_BIT_MAC(dataPtr[2],9) ^ PRV_GET_BIT_MAC(dataPtr[2],7) ^ PRV_GET_BIT_MAC(dataPtr[2],5) ^
                        PRV_GET_BIT_MAC(dataPtr[2],4) ^ PRV_GET_BIT_MAC(dataPtr[2],3) ^ PRV_GET_BIT_MAC(dataPtr[2],1);
                        /* 0x1 = PRV_GET_BIT_MAC(C,1) ^
                        PRV_GET_BIT_MAC(C,3) ^ PRV_GET_BIT_MAC(C,4) ^ PRV_GET_BIT_MAC(C,5) ^ PRV_GET_BIT_MAC(C,7) ^ PRV_GET_BIT_MAC(C,9); */

        hashBitPtr[9] ^=  PRV_GET_BIT_MAC(dataPtr[2],10) ^ PRV_GET_BIT_MAC(dataPtr[2],8) ^ PRV_GET_BIT_MAC(dataPtr[2],6) ^
                        PRV_GET_BIT_MAC(dataPtr[2],5) ^ PRV_GET_BIT_MAC(dataPtr[2],4) ^ PRV_GET_BIT_MAC(dataPtr[2],2) ^ PRV_GET_BIT_MAC(dataPtr[2],0);
                        /* 0x1 = PRV_GET_BIT_MAC(C,0) ^
                        PRV_GET_BIT_MAC(C,2) ^ PRV_GET_BIT_MAC(C,4) ^ PRV_GET_BIT_MAC(C,5) ^ PRV_GET_BIT_MAC(C,6) ^
                        PRV_GET_BIT_MAC(C,8) ^ PRV_GET_BIT_MAC(C,10); */

        hashBitPtr[10] ^= PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],9) ^ PRV_GET_BIT_MAC(dataPtr[2],7) ^
                        PRV_GET_BIT_MAC(dataPtr[2],6) ^ PRV_GET_BIT_MAC(dataPtr[2],5) ^ PRV_GET_BIT_MAC(dataPtr[2],3) ^ PRV_GET_BIT_MAC(dataPtr[2],1) ^
                        PRV_GET_BIT_MAC(dataPtr[2],0);
                        /* 0 = PRV_GET_BIT_MAC(C,0) ^
                        PRV_GET_BIT_MAC(C,1) ^ PRV_GET_BIT_MAC(C,3) ^ PRV_GET_BIT_MAC(C,5) ^ PRV_GET_BIT_MAC(C,6) ^
                        PRV_GET_BIT_MAC(C,7) ^ PRV_GET_BIT_MAC(C,9) ^ PRV_GET_BIT_MAC(C,11); */

        hashBitPtr[12] = 0;

        if (numOfBitsToUse == 12)
        {
            hashBitPtr[11] ^=  PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],10) ^ PRV_GET_BIT_MAC(dataPtr[2],7) ^
                             PRV_GET_BIT_MAC(dataPtr[2],5) ^ PRV_GET_BIT_MAC(dataPtr[2],4) ^ PRV_GET_BIT_MAC(dataPtr[2],1);
                            /* 0 = PRV_GET_BIT_MAC(C,1) ^
                            PRV_GET_BIT_MAC(C,4) ^ PRV_GET_BIT_MAC(C,5) ^ PRV_GET_BIT_MAC(C,7) ^ PRV_GET_BIT_MAC(C,10) ^
                            PRV_GET_BIT_MAC(C,11); */
        }
        else
            hashBitPtr[11] = 0;
    }
    else
    {
        /* number of hash bits is 13 */

        hashBitPtr[0] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],10) ^ PRV_GET_BIT_MAC(dataPtr[2],9)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],7)  ^ PRV_GET_BIT_MAC(dataPtr[2],4)  ^ PRV_GET_BIT_MAC(dataPtr[2],2)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],0);

        hashBitPtr[1] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],9)  ^ PRV_GET_BIT_MAC(dataPtr[2],8)  ^ PRV_GET_BIT_MAC(dataPtr[2],7)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],5)  ^ PRV_GET_BIT_MAC(dataPtr[2],4)  ^ PRV_GET_BIT_MAC(dataPtr[2],3)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],2)  ^ PRV_GET_BIT_MAC(dataPtr[2],1);

        hashBitPtr[2] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],8)  ^ PRV_GET_BIT_MAC(dataPtr[2],7)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],6)  ^ PRV_GET_BIT_MAC(dataPtr[2],5)  ^ PRV_GET_BIT_MAC(dataPtr[2],3)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],0);

        hashBitPtr[3] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],10) ^ PRV_GET_BIT_MAC(dataPtr[2],8)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],6)  ^ PRV_GET_BIT_MAC(dataPtr[2],2)  ^ PRV_GET_BIT_MAC(dataPtr[2],1);

        hashBitPtr[4] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],9)  ^ PRV_GET_BIT_MAC(dataPtr[2],7)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],3)  ^ PRV_GET_BIT_MAC(dataPtr[2],2);

        hashBitPtr[5] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],10)  ^ PRV_GET_BIT_MAC(dataPtr[2],8)  ^ PRV_GET_BIT_MAC(dataPtr[2],4)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],3)   ^ PRV_GET_BIT_MAC(dataPtr[2],0);

        hashBitPtr[6] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],9)  ^ PRV_GET_BIT_MAC(dataPtr[2],5)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],4)  ^ PRV_GET_BIT_MAC(dataPtr[2],1)  ^ PRV_GET_BIT_MAC(dataPtr[2],0);

        hashBitPtr[7] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],10) ^ PRV_GET_BIT_MAC(dataPtr[2],6)  ^ PRV_GET_BIT_MAC(dataPtr[2],5)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],2)  ^ PRV_GET_BIT_MAC(dataPtr[2],1)  ^ PRV_GET_BIT_MAC(dataPtr[2],0);

        hashBitPtr[8] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],7)  ^ PRV_GET_BIT_MAC(dataPtr[2],6)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],3)  ^ PRV_GET_BIT_MAC(dataPtr[2],2)  ^ PRV_GET_BIT_MAC(dataPtr[2],1)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],0);

        hashBitPtr[9] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],8)  ^ PRV_GET_BIT_MAC(dataPtr[2],7)  ^ PRV_GET_BIT_MAC(dataPtr[2],4)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],3)  ^ PRV_GET_BIT_MAC(dataPtr[2],2)  ^ PRV_GET_BIT_MAC(dataPtr[2],1);

        hashBitPtr[10] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],9)  ^ PRV_GET_BIT_MAC(dataPtr[2],8)  ^ PRV_GET_BIT_MAC(dataPtr[2],5)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],4)  ^ PRV_GET_BIT_MAC(dataPtr[2],3)  ^ PRV_GET_BIT_MAC(dataPtr[2],2)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],0);

        hashBitPtr[11] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],7)  ^ PRV_GET_BIT_MAC(dataPtr[2],6)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],5)  ^ PRV_GET_BIT_MAC(dataPtr[2],3)  ^ PRV_GET_BIT_MAC(dataPtr[2],2)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],1)  ^ PRV_GET_BIT_MAC(dataPtr[2],0);

        hashBitPtr[12] ^=
                    PRV_GET_BIT_MAC(dataPtr[2],11) ^ PRV_GET_BIT_MAC(dataPtr[2],10) ^ PRV_GET_BIT_MAC(dataPtr[2],9)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],8)  ^ PRV_GET_BIT_MAC(dataPtr[2],6)  ^ PRV_GET_BIT_MAC(dataPtr[2],3)  ^
                    PRV_GET_BIT_MAC(dataPtr[2],1);
    }

    (*hashPtr) = 0;

    for(ii = 0 ; ii < numOfBitsToUse; ii++)
    {
        (*hashPtr) |= (hashBitPtr[ii] & 0x01) << ii;
    }

    return GT_OK;

}


/*******************************************************************************
* ipHashCalcXor
*
* DESCRIPTION:
*       !!! hashing in Cheetah devices --- XOR hashing !!!
*       This function calculates the hash index for the mac address table.
*       for specific DIP, SIP and VLAN id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       vid             - the VLAN id.
*       dip             - destination IP
*       sip             - sources IP
*       numOfBitsToUse  - number of bits to use for XOR algorithm: 11,12 or 13
*
* OUTPUTS:
*       hashPtr         - (pointer to) the hash index.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ipHashCalcXor
(
    IN GT_U16   vid,
    IN GT_U32   dip,
    IN GT_U32   sip,
    IN GT_U32   numOfBitsToUse,
    OUT GT_U32 *hashPtr
)
{
    GT_U32              transposedHash; /* contains the transposed hash value */
    GT_U32              u32Array[7];
    GT_U32              swappedVidBits;/* swapped vid bits */
    GT_U32              ii;/* iterator */

    swappedVidBits = 0;

    /* swap the vid bits */
    for(ii = 0 ; ii < 12; ii++ )
    {
        swappedVidBits |= (vid & (1 << ii)) ?
            (1 << (11-ii)) :
            0;
    }

    if(numOfBitsToUse == 12)
    {
        u32Array[0] = dip & 0xFFF;              /* dip[11:0] */
        u32Array[1] = sip & 0xFFF;              /* sip[11:0] */
        u32Array[2] = (dip >> 12) & 0xFFF;      /* dip[23:12] */
        u32Array[3] = (sip >> 12) & 0xFFF;      /* sip[23:12] */
        u32Array[4] = (dip >> 24) & 0xFFF;      /* dip[31:24] */
        u32Array[5] = (sip >> 24) & 0xFF;       /* sip[31:24] */
        u32Array[6] = swappedVidBits;           /* VID[11:0]  REVERSED !!!*/
    }
    else if (numOfBitsToUse == 11)
    {
        u32Array[0] = dip & 0x7FF;              /* dip[10:0] */
        u32Array[0] ^= ((swappedVidBits & 1) << 10); /* VID[11] should be
                                                    xored with dip[10], dip[21]
                                                    sip[10], sip[21], vid[0]*/

        u32Array[1] = sip & 0x7FF;              /* sip[10:0] */
        u32Array[2] = (dip >> 11) & 0x7FF;      /* dip[21:11] */
        u32Array[3] = (sip >> 11) & 0x7FF;      /* sip[21:11] */
        u32Array[4] = (dip >> 22) & 0x3FF;      /* dip[31:22] */
        u32Array[5] = (sip >> 22) & 0x3FF;      /* sip[31:22] */
        u32Array[6] = swappedVidBits >> 1;      /* VID[10:0]  REVERSED !!!*/

    }
    else
    {/* number of bits to use is 13 */

        /* swapped vid of 13 bits is: 0 1 2 3 4 5 6 7 8 9 10 11 0 */
        swappedVidBits = ((swappedVidBits << 1) | (vid & 0x1));

        u32Array[0] = dip & 0x1FFF;             /* dip[12:0] */
        u32Array[1] = sip & 0x1FFF;             /* sip[12:0] */
        u32Array[2] = (dip >> 13) & 0x1FFF;     /* dip[25:13] */
        u32Array[3] = (sip >> 13) & 0x1FFF;     /* sip[25:13] */
        u32Array[4] = (dip >> 26) & 0x3F;       /* dip[31:26] */
        u32Array[5] = (sip >> 26) & 0x3F;       /* sip[31:26] */
        u32Array[6] = swappedVidBits;           /* VID[0]VID[11:0]  REVERSED !!!*/
    }

    /* compute the transposed hash */
    transposedHash = 0;
    for (ii = 0; ii < 7; ii++)
    {
        transposedHash ^= u32Array[ii] ;
    }

    (*hashPtr) = 0;

    /* we need to transpose (reverse) the calculation so far */
    for(ii = 0 ;ii < numOfBitsToUse; ii++)
    {
        (*hashPtr) |= (transposedHash & (1 << ii)) ?
                   1 << (numOfBitsToUse - (ii + 1)) :
                   0;
    }

    return GT_OK;


}


/*******************************************************************************
* ipHashCalc
*
* DESCRIPTION:
*       This function prepares IPMC[S/dataPtr] + VID data for calculating 12-bit hash
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       vid             - Vlan id
*       sip             - source IP
*       dip             - destination IP
* OUTPUTS:
*       hashPtr         - (pointer to) the hash index
*
* RETURNS:
*       GT_OK   - success
*       GT_FAIL - failure
*
* COMMENTS:
*       for CRC used polinom X^12+X^11+X^3+X^2+X+1 i.e 0xF01.
*
*******************************************************************************/
static GT_STATUS ipHashCalc
(
    IN  GT_U8   devNum,
    IN  GT_U16  vid,
    IN  GT_U32  sip,
    IN  GT_U32  dip,
    OUT GT_U32  *hashPtr
)
{
    GT_STATUS           rc;
    GT_U32              numOfBitsToUse;

    switch (PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.size)
    {
        case PRV_CPSS_DXCH_MAC_TBL_SIZE_8K_E:
            numOfBitsToUse = 11;
            break;

        case PRV_CPSS_DXCH_MAC_TBL_SIZE_16K_E:
            numOfBitsToUse = 12;
            break;

        case PRV_CPSS_DXCH_MAC_TBL_SIZE_32K_E:
            numOfBitsToUse = 13;
            break;

        default:
            return GT_BAD_STATE;
    }

    if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode ==
                                            CPSS_MAC_HASH_FUNC_XOR_E)
    {
        /* XOR hash function */
        rc = ipHashCalcXor(vid, dip, sip, numOfBitsToUse,hashPtr);
    }
    else if(PRV_CPSS_DXCH_PP_MAC(devNum)->bridge.fdbHashParams.hashMode ==
                                            CPSS_MAC_HASH_FUNC_CRC_E)
    {
        /* CRC hash function */
        rc = ipHashCalcCrc(vid, dip, sip, numOfBitsToUse,hashPtr);
    }
    else
    {
        return GT_BAD_PARAM;
    }

    /* the hash START index DOES NOT depend on the macChainLen !!! */
    (*hashPtr) *= 4;/* 4 is HARD CODED !!!!! - do not change */

    return rc;
}

/*******************************************************************************
* cpssDxChBrgFdbHashCalc
*
* DESCRIPTION:
*       This function calculates the hash index for the FDB table.
*       The FDB table hold 2 types of entries :
*       1. for specific mac address and VLAN id.
*       2. for specific src IP , dst IP and VLAN id.
*
*       for more details see CPSS_MAC_ENTRY_EXT_KEY_STC description.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       hashPtr - (pointer to) the hash index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum, bad vlan-Id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashCalc
(
    IN  GT_U8                           devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC      *macEntryKeyPtr,
    OUT GT_U32                          *hashPtr
)
{
    GT_U32 sip;
    GT_U32 dip;
    GT_U32 ii;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(macEntryKeyPtr);
    CPSS_NULL_PTR_CHECK_MAC(hashPtr);

    /* check key type */
    switch (macEntryKeyPtr->entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:

            /* check vlan-id parameter */
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(macEntryKeyPtr->key.macVlan.vlanId);

            return macHashCalc(devNum, &macEntryKeyPtr->key.macVlan.macAddr,
                        macEntryKeyPtr->key.macVlan.vlanId,
                        hashPtr);

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:

            /* check vlan-id parameter */
            PRV_CPSS_VLAN_VALUE_CHECK_MAC(macEntryKeyPtr->key.ipMcast.vlanId);

            sip = 0;
            dip = 0;
            ii = 0;
            /* set the sip field */
            U32_SET_FIELD_NO_CLEAR_MAC(sip,24,8,macEntryKeyPtr->key.ipMcast.sip[ii++]);
            U32_SET_FIELD_NO_CLEAR_MAC(sip,16,8,macEntryKeyPtr->key.ipMcast.sip[ii++]);
            U32_SET_FIELD_NO_CLEAR_MAC(sip, 8,8,macEntryKeyPtr->key.ipMcast.sip[ii++]);
            U32_SET_FIELD_NO_CLEAR_MAC(sip, 0,8,macEntryKeyPtr->key.ipMcast.sip[ii++]);

            ii = 0;
            /* set the dip field */
            U32_SET_FIELD_NO_CLEAR_MAC(dip,24,8,macEntryKeyPtr->key.ipMcast.dip[ii++]);
            U32_SET_FIELD_NO_CLEAR_MAC(dip,16,8,macEntryKeyPtr->key.ipMcast.dip[ii++]);
            U32_SET_FIELD_NO_CLEAR_MAC(dip, 8,8,macEntryKeyPtr->key.ipMcast.dip[ii++]);
            U32_SET_FIELD_NO_CLEAR_MAC(dip, 0,8,macEntryKeyPtr->key.ipMcast.dip[ii++]);

            return ipHashCalc(devNum, macEntryKeyPtr->key.ipMcast.vlanId,
                              sip,dip,hashPtr);

        default:
            return GT_BAD_PARAM;
    }
}



