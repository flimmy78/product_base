/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxIpUT.c
*
* DESCRIPTION:
*       Unit Tests for the CPSS EXMX Ip HW structures APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/exMx/exMxGen/ip/cpssExMxIp.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>

#define UTF_EXMX_SAMBA_FAMILY_SET_CNS   UTF_CPSS_PP_FAMILY_SAMBA_CNS

#define UTF_EXMX_NON_TIGER_FAMILY_SET_CNS     (UTF_CPSS_PP_FAMILY_TWISTC_CNS | \
                                               UTF_CPSS_PP_FAMILY_TWISTD_CNS |  \
                                               UTF_CPSS_PP_FAMILY_SAMBA_CNS)

/* Invalid enumeration value used for testing */
#define IP_INVALID_ENUM_CNS       0x5AAAAAA5

/*  Internal function. Is used for filling CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC  */
/*  structure with default values which are used in many tests.             */
static void IpUcRouteEntryDefaultSet(IN  CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray);

/*  Internal function. Is used for filling CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC  */
/*  structure with default values which are used in many tests.             */
static void IpMcRouteEntryDefaultSet(IN  CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC *routeEntriesArray);

/*  Internal function. Is used for filling OUTLIF CONFIG structure  */
/*  with default values which are used for most of all tests.       */
static void IpOutlifConfigDefaultSet(IN  CPSS_EXMX_IP_OUTLIF_CONFIG_STC  *outlifPtr);

/*  Internal function. Is used for filling OUTLIF CONFIG structure  */
/*  with default values which are used for most of all tests.       */
static void IpMcLinkListDefaultSet(IN  CPSS_EXMX_IP_MC_LINK_LIST_STC  *mllEntryPtr);



/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpHwLpmNodeNextPointersWrite
(
    IN GT_U8                                    dev,
    IN GT_U32                                   memAddr,
    IN GT_U32                                   sramIndex,
    IN GT_U32                                   pointingToSramIndex,
    IN CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC   *nextPointerArrayPtr,
    IN GT_U32                                   numOfNextPointers
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpHwLpmNodeNextPointersWrite)
{
/*
ITERATE_DEVICES
1.1. Call function with memAddr [1000], sramIndex [0], pointingToSramIndex[0], nextPointerArrayPtr [0] [CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E, nextNodePointer [nextPointer = 1200, range5Index = 1] ], numOfNextPointers [1]. Expected: GT_OK.
1.2. Call function with memAddr [1000], sramIndex [0], pointingToSramIndex[0],  nextPointerArrayPtr [0] [CPSS_EXMX_IP_COMPRESSED_2_NODE_PTR_TYPE_E, nextNodePointer [nextPointer = 1200, range5Index = 1] ] , numOfNextPointers [1].  Expected: GT_OK.
1.3. Call function with memAddr [1000], sramIndex [0], pointingToSramIndex[0], nextPointerArrayPtr [0] [CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E, routeEntryPointer [routeEntryBaseMemAddr = 1200, blockSize = 1, CPSS_EXMX_IP_REG_ROUTE_ENTRY_E] ] , numOfNextPointers [1].  Expected: GT_OK.
1.4. Check out-of-range enum for pointerType. Call with nextPointerArrayPtr [0] [pointerType = 0x5AAAAAA5, …], other parameters same as in 1.1. Expected: GT_BAD_PARAM.
1.5. Check for NULL pointers. Call with nextPointerArrayPtr [NULL], other parameters same as in 1.1. Expected: GT_BAD_PTR.
1.6. Check out-of-range sramIndex. Call with sramIndex [1000], other parameters same as in 1.1. Expected: NON GT_OK.
1.7. Check out-of-range pointingToSramIndex. Call with pointingToSramIndex [1000], other parameters same as in 1.1. Expected: NON GT_OK.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                                   memAddr;
    GT_U32                                   sramIndex;
    GT_U32                                   pointingToSramIndex;
    CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC   nextPointerArray[1];
    GT_U32                                   numOfNextPointers;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with memAddr [1000], sramIndex [0],           */
        /* pointingToSramIndex[0], nextPointerArrayPtr [0]                  */
        /* [CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E, nextNodePointer           */
        /* [nextPointer = 1200, range5Index = 1] ], numOfNextPointers [1].  */
        /* Expected: GT_OK.                                                 */

        memAddr = 1000;
        sramIndex = 0;
        pointingToSramIndex = 0;
        nextPointerArray[0].pointerType = CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E;
        nextPointerArray[0].pointerData.nextNodePointer.nextPointer = 1200;
        nextPointerArray[0].pointerData.nextNodePointer.range5Index = 1;

        numOfNextPointers = 1;

        st = cpssExMxIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex,
                 pointingToSramIndex, nextPointerArray, numOfNextPointers);

        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "%d, %d, %d, %d, pointerType = %d",
             dev, memAddr, sramIndex, pointingToSramIndex, nextPointerArray[0].pointerType);

        /* 1.2. Call function with memAddr [1000], sramIndex [0],
           pointingToSramIndex[0],  nextPointerArrayPtr [0]
           [CPSS_EXMX_IP_COMPRESSED_2_NODE_PTR_TYPE_E, nextNodePointer
           [nextPointer = 1200, range5Index = 1] ] , numOfNextPointers [1].
           Expected: GT_OK.     */

        nextPointerArray[0].pointerType = CPSS_EXMX_IP_COMPRESSED_2_NODE_PTR_TYPE_E;

        st = cpssExMxIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex,
                 pointingToSramIndex, nextPointerArray, numOfNextPointers);

        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "%d, %d, %d, %d, pointerType = %d",
             dev, memAddr, sramIndex, pointingToSramIndex, nextPointerArray[0].pointerType);

        /* 1.3. Call function with memAddr [1000], sramIndex [0],       */
        /* pointingToSramIndex[0], nextPointerArrayPtr [0]              */
        /* [CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E, routeEntryPointer */
        /* [routeEntryBaseMemAddr = 1200, blockSize = 1,                */
        /* CPSS_EXMX_IP_REG_ROUTE_ENTRY_E] ] , numOfNextPointers [1].   */
        /* Expected: GT_OK.                                             */

        nextPointerArray[0].pointerType = CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryBaseMemAddr = 1200;
        nextPointerArray[0].pointerData.routeEntryPointer.blockSize = 1;
        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMX_IP_REG_ROUTE_ENTRY_E;

        st = cpssExMxIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex,
                 pointingToSramIndex, nextPointerArray, numOfNextPointers);

        UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, st, "%d, %d, %d, %d, pointerType = %d",
             dev, memAddr, sramIndex, pointingToSramIndex, nextPointerArray[0].pointerType);

        /* 1.4. Check out-of-range enum for pointerType. Call with  */
        /* nextPointerArrayPtr [0] [pointerType = 0x5AAAAAA5, …],   */
        /* other parameters same as in 1.1. Expected: GT_BAD_PARAM. */

        nextPointerArray[0].pointerType = IP_INVALID_ENUM_CNS;
        nextPointerArray[0].pointerData.nextNodePointer.nextPointer = 1200;
        nextPointerArray[0].pointerData.nextNodePointer.range5Index = 1;

        st = cpssExMxIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex,
                 pointingToSramIndex, nextPointerArray, numOfNextPointers);

        UTF_VERIFY_EQUAL5_STRING_MAC(GT_BAD_PARAM, st, "%d, %d, %d, %d, pointerType = %d",
             dev, memAddr, sramIndex, pointingToSramIndex, nextPointerArray[0].pointerType);

        nextPointerArray[0].pointerType = CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E;

        /* 1.5. Check for NULL pointers. Call with nextPointerArrayPtr      */
        /* [NULL], other parameters same as in 1.1. Expected: GT_BAD_PTR.   */

        st = cpssExMxIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex,
                 pointingToSramIndex, NULL, numOfNextPointers);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, nextPointerArrayPtr = NULL", dev);

        /* 1.6. Check out-of-range sramIndex. Call with sramIndex [1000],   */
        /* other parameters same as in 1.1. Expected: NON GT_OK.            */

        sramIndex = 1000;

        st = cpssExMxIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex,
                 pointingToSramIndex, nextPointerArray, numOfNextPointers);

        UTF_VERIFY_NOT_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, sramIndex = %d",
                                         dev, memAddr, sramIndex);

        sramIndex = 0;

        /* 1.7. Check out-of-range pointingToSramIndex. Call with       */
        /* pointingToSramIndex [1000], other parameters same as in 1.1. */
        /* Expected: NON GT_OK.                                         */

        pointingToSramIndex = 1000;

        st = cpssExMxIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex,
                 pointingToSramIndex, nextPointerArray, numOfNextPointers);

        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pointingToSramIndex = %d",
                                         dev, pointingToSramIndex);

        pointingToSramIndex = 0;
    }

    memAddr = 1000;
    sramIndex = 0;
    pointingToSramIndex = 0;
    nextPointerArray[0].pointerType = CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E;
    nextPointerArray[0].pointerData.nextNodePointer.nextPointer = 1200;
    nextPointerArray[0].pointerData.nextNodePointer.range5Index = 1;
    numOfNextPointers = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex,
                 pointingToSramIndex, nextPointerArray, numOfNextPointers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpHwLpmNodeNextPointersWrite(dev, memAddr, sramIndex,
             pointingToSramIndex, nextPointerArray, numOfNextPointers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpHwLpmNodeWrite
(
    IN GT_U8                                          dev,
    IN GT_U32                                         memAddr,
    IN GT_U32                                         sramIndex,
    IN GT_U32                                         pointingToSramIndex,
    IN CPSS_EXMX_IP_NEXT_POINTER_TYPE_ENT             nodeType,
    IN CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT  *rangeSelectSecPtr,
    IN CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC         *nextPointerArrayPtr,
    IN GT_U32                                         numOfNextPointers
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpHwLpmNodeWrite)
{
/*
ITERATE_DEVICES 
1.1. Call function with memAddr [1000], sramIndex [0], pointingToSramIndex[0],  nodeType [CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E], rangeSelectSecPtr - zeroed, nextPointerArrayPtr[0] [CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E, routeEntryPointer [routeEntryBaseMemAddr = 1000, blockSize = 1, CPSS_EXMX_IP_REG_ROUTE_ENTRY_E] ], numOfNextPointers [1]. Expected: GT_OK.
1.2. Call function cpssExMxIpHwLpmNodeRead with non-NULL rangeSelectSecPtr, nextPointerArrayPtr, numOfNextPointersPtr, other params same as in 1.1. Expected: GT_OK and the same values as written.
1.3. Check out-of-range enum for nodeType. Call with nodeType [0x5AAAAAA5], other parameters same as in 1.1. Expected: GT_BAD_PARAM.
1.4. Check for NULL pointer. Call with rangeSelectSecPtr [NULL], other parameters same as in 1.1. Expected: GT_BAD_PTR.
1.5. Check for NULL pointer. Call with nextPointerArrayPtr [NULL], other parameters same as in 1.1. Expected: GT_BAD_PTR.
1.6. Check out-of-range pointingToSramIndex. Call with pointingToSramIndex [100], other parameters same as in 1.1. Expected: non GT_OK.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                                          memAddr;
    GT_U32                                          sramIndex;
    GT_U32                                          pointingToSramIndex;
    CPSS_EXMX_IP_NEXT_POINTER_TYPE_ENT              nodeType;
    CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT   rangeSelectSec;
    CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC          nextPointerArray[1];
    GT_U32                                          numOfNextPointers;

    CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT   rangeSelectSecGet;
    CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC          nextPointerArrayGet[1];
    GT_U32                                          numOfNextPointersGet;
    GT_BOOL                                         failureWas;

    /* Fill with zero */
    cpssOsMemSet((GT_VOID*)&rangeSelectSec, 0, 
                 sizeof(CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT));

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with memAddr [1000], sramIndex [0],           */
        /* pointingToSramIndex[0],  nodeType                                */
        /* [CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E], rangeSelectSecPtr    */
        /* - zeroed, nextPointerArrayPtr[0]                                 */
        /* [CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E, routeEntryPointer     */
        /* [routeEntryBaseMemAddr = 1000, blockSize = 1,                    */
        /* CPSS_EXMX_IP_REG_ROUTE_ENTRY_E] ], numOfNextPointers [1].        */
        /* Expected: GT_OK.                                                 */

        memAddr = 1000;
        sramIndex = 0;
        pointingToSramIndex = 0;
        nodeType = CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
        nextPointerArray[0].pointerType = CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E;
        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryBaseMemAddr = 1000;
        nextPointerArray[0].pointerData.routeEntryPointer.blockSize = 1;
        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMX_IP_REG_ROUTE_ENTRY_E;
        numOfNextPointers = 1;

        st = cpssExMxIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                 nodeType, &rangeSelectSec, nextPointerArray, numOfNextPointers);

        UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st, "%d, %d, %d, %d, %d, pointerType = %d",
             dev, memAddr, sramIndex, pointingToSramIndex, nodeType, nextPointerArray[0].pointerType);

        /* 1.2. Call function cpssExMxIpHwLpmNodeRead with non-NULL
           rangeSelectSecPtr, nextPointerArrayPtr, numOfNextPointersPtr,
           other params same as in 1.1.
           Expected: GT_OK and the same values as written.  */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)&rangeSelectSecGet, 0, 
                     sizeof(CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT));


        cpssOsMemSet((GT_VOID*)nextPointerArrayGet, 0, 
                     numOfNextPointers*sizeof(CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC));
        numOfNextPointersGet = 0;

        st = cpssExMxIpHwLpmNodeRead(dev, memAddr, sramIndex, /* pointingToSramIndex,*/
                 nodeType, &rangeSelectSecGet, nextPointerArrayGet, &numOfNextPointersGet);

        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxIpHwLpmNodeRead: %d, %d, %d, %d",
                   dev, memAddr, sramIndex, nodeType);

        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&rangeSelectSec,
                                            (const GT_VOID*)&rangeSelectSecGet, sizeof(CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                "cpssExMxIpHwLpmNodeRead: get another rangeSelectSec than was set: %d, %d, %d, %d",
                dev, memAddr, sramIndex, nodeType);

            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)nextPointerArray,
                                            (const GT_VOID*)nextPointerArrayGet, numOfNextPointers*sizeof(CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                "cpssExMxIpHwLpmNodeRead: get another nextPointerArray than was set: %d, %d, %d, %d",
                dev, memAddr, sramIndex, nodeType);

            failureWas = (numOfNextPointers != numOfNextPointersGet);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                "cpssExMxIpHwLpmNodeRead: get another nextPointerArray than was set: %d, %d, %d, %d",
                dev, memAddr, sramIndex, nodeType);
        }

        /* 1.3. Check out-of-range enum for nodeType. Call with nodeType    */
        /* [0x5AAAAAA5], other parameters same as in 1.1.                   */
        /* Expected: GT_BAD_PARAM.                                          */

        nodeType = IP_INVALID_ENUM_CNS;

        st = cpssExMxIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                 nodeType, &rangeSelectSec, nextPointerArray, numOfNextPointers);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nodeType = %d",
             dev, nodeType);

        nodeType = CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;

        /* 1.4. Check for NULL pointer. Call with rangeSelectSecPtr [NULL], */
        /* other parameters same as in 1.1. Expected: GT_BAD_PTR.           */

        st = cpssExMxIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                 nodeType, NULL, nextPointerArray, numOfNextPointers);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, rangeSelectSecPtr = NULL", dev);

        /* 1.5. Check for NULL pointer. Call with nextPointerArrayPtr       */
        /* [NULL], other parameters same as in 1.1. Expected: GT_BAD_PTR.   */

        st = cpssExMxIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                 nodeType, &rangeSelectSec, NULL, numOfNextPointers);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, nextPointerArrayPtr = NULL", dev);

        /* 1.6. Check out-of-range pointingToSramIndex. Call with       */
        /* pointingToSramIndex [100], other parameters same as in 1.1.  */
        /* Expected: non GT_OK.                                         */

        pointingToSramIndex = 100;

        st = cpssExMxIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                 nodeType, &rangeSelectSec, nextPointerArray, numOfNextPointers);

        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pointingToSramIndex = %d",
                                         dev, pointingToSramIndex);

        pointingToSramIndex = 0;
    }

    memAddr = 1000;
    sramIndex = 0;
    pointingToSramIndex = 0;
    nodeType = CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
    nextPointerArray[0].pointerType = CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E;
    nextPointerArray[0].pointerData.routeEntryPointer.routeEntryBaseMemAddr = 1000;
    nextPointerArray[0].pointerData.routeEntryPointer.blockSize = 1;
    nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMX_IP_REG_ROUTE_ENTRY_E;
    numOfNextPointers = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                 nodeType, &rangeSelectSec, nextPointerArray, numOfNextPointers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex,
             nodeType, &rangeSelectSec, nextPointerArray, numOfNextPointers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpHwLpmNodeRead
(
    IN  GT_U8                                          dev,
    IN  GT_U32                                         memAddr,
    IN  GT_U32                                         sramIndex,
    IN  CPSS_EXMX_IP_NEXT_POINTER_TYPE_ENT             nodeType,
    OUT CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT  *rangeSelectSecPtr,
    OUT CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC         *nextPointerArrayPtr,
    OUT GT_U32                                         *numOfNextPointersPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpHwLpmNodeRead)
{
/*
ITERATE_DEVICES 
1.1. Write IpHwLpmNode for testing. Call cpssExMxIpHwLpmNodeWrite with memAddr [1000], sramIndex [0], pointingToSramIndex[0],  nodeType [CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E], rangeSelectSecPtr - zeroed, nextPointerArrayPtr[0] [CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E, routeEntryPointer [routeEntryBaseMemAddr = 1000, blockSize = 1, CPSS_EXMX_IP_REG_ROUTE_ENTRY_E] ], numOfNextPointers [1]. Expected: GT_OK.
1.2. Call function with non-NULL rangeSelectSecPtr, nextPointerArrayPtr, numOfNextPointersPtr, other params same as in 1.1. Expected: GT_OK and same values as written.
1.3. Check out-of-range enum for nodeType. Call with nodeType [0x5AAAAAA5], other parameters same as in 1.2. Expected: GT_BAD_PARAM.
1.4. Check for NULL pointer support. Call with rangeSelectSecPtr [NULL], other params same as in 1.2. Expected: GT_BAD_PTR.
1.5. Check for NULL pointer support. Call with nextPointerArrayPtr [NULL], other params same as in 1.2. Expected: GT_BAD_PTR.
1.6. Check for NULL pointer support. Call with numOfNextPointersPtr [NULL], other params same as in 1.2. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                                          memAddr;
    GT_U32                                          sramIndex;
    GT_U32                                          pointingToSramIndex;
    CPSS_EXMX_IP_NEXT_POINTER_TYPE_ENT              nodeType;
    CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT   rangeSelectSec;
    CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC          nextPointerArray[1];
    GT_U32                                          numOfNextPointers;

    CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT   rangeSelectSecGet;
    CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC          nextPointerArrayGet[1];
    GT_U32                                          numOfNextPointersGet;
    GT_BOOL                                         failureWas;

    /* Fill with zero */
    cpssOsMemSet((GT_VOID*)&rangeSelectSec, 0, 
                 sizeof(CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT));

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Write IpHwLpmNode for testing. Call cpssExMxIpHwLpmNodeWrite*/
        /* with memAddr [1000], sramIndex [0],                              */
        /* pointingToSramIndex[0],  nodeType                                */
        /* [CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E], rangeSelectSecPtr    */
        /* - zeroed, nextPointerArrayPtr[0]                                 */
        /* [CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E, routeEntryPointer     */
        /* [routeEntryBaseMemAddr = 1000, blockSize = 1,                    */
        /* CPSS_EXMX_IP_REG_ROUTE_ENTRY_E] ], numOfNextPointers [1].        */
        /* Expected: GT_OK.                                                 */

        memAddr = 1000;
        sramIndex = 0;
        pointingToSramIndex = 0;
        nodeType = CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
        nextPointerArray[0].pointerType = CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E;
        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryBaseMemAddr = 1000;
        nextPointerArray[0].pointerData.routeEntryPointer.blockSize = 1;
        nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMX_IP_REG_ROUTE_ENTRY_E;
        numOfNextPointers = 1;

        st = cpssExMxIpHwLpmNodeWrite(dev, memAddr, sramIndex, pointingToSramIndex,
                 nodeType, &rangeSelectSec, nextPointerArray, numOfNextPointers);

        UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st, "cpssExMxIpHwLpmNodeWrite: %d, %d, %d, %d, %d, pointerType = %d",
             dev, memAddr, sramIndex, pointingToSramIndex, nodeType, nextPointerArray[0].pointerType);

        /* 1.2. Call function with non-NULL rangeSelectSecPtr,
           nextPointerArrayPtr, numOfNextPointersPtr, other params
           same as in 1.1. Expected: GT_OK and same values as written.  */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)&rangeSelectSecGet, 0, 
                     sizeof(CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT));

        cpssOsMemSet((GT_VOID*)nextPointerArrayGet, 0, 
                     numOfNextPointers*sizeof(CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC));
        numOfNextPointersGet = 0;

        st = cpssExMxIpHwLpmNodeRead(dev, memAddr, sramIndex, /* pointingToSramIndex,*/
                 nodeType, &rangeSelectSecGet, nextPointerArrayGet, &numOfNextPointersGet);

        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxIpHwLpmNodeRead: %d, %d, %d, %d",
                   dev, memAddr, sramIndex, nodeType);

        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&rangeSelectSec,
                                            (const GT_VOID*)&rangeSelectSecGet, sizeof(CPSS_EXMX_IP_LPM_NODE_RANGE_SELECTION_SEC_UNT))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                "get another rangeSelectSec than was set: %d, %d, %d, %d",
                dev, memAddr, sramIndex, nodeType);

            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)nextPointerArray,
                                            (const GT_VOID*)nextPointerArrayGet, numOfNextPointers*sizeof(CPSS_EXMX_IP_LPM_NODE_NEXT_POINTER_STC))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                "get another nextPointerArray than was set: %d, %d, %d, %d",
                dev, memAddr, sramIndex, nodeType);

            failureWas = (numOfNextPointers != numOfNextPointersGet);
            UTF_VERIFY_EQUAL4_STRING_MAC(GT_FALSE, failureWas,
                "get another nextPointerArray than was set: %d, %d, %d, %d",
                dev, memAddr, sramIndex, nodeType);
        }

        /* 1.3. Check out-of-range enum for nodeType. Call with nodeType
        [0x5AAAAAA5], other parameters same as in 1.2. Expected: GT_BAD_PARAM.*/

        nodeType = IP_INVALID_ENUM_CNS;

        st = cpssExMxIpHwLpmNodeRead(dev, memAddr, sramIndex,
                 nodeType, &rangeSelectSecGet, nextPointerArrayGet, &numOfNextPointersGet);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st, "%d, nodeType = %d",
             dev, nodeType);

        nodeType = CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;

        /* 1.4. Check for NULL pointer. Call with rangeSelectSecPtr [NULL], */
        /* other parameters same as in 1.2. Expected: GT_BAD_PTR.           */

        st = cpssExMxIpHwLpmNodeRead(dev, memAddr, sramIndex,
                 nodeType, NULL, nextPointerArrayGet, &numOfNextPointersGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, rangeSelectSecPtr = NULL", dev);

        /* 1.5. Check for NULL pointer. Call with nextPointerArrayPtr       */
        /* [NULL], other parameters same as in 1.2. Expected: GT_BAD_PTR.   */

        st = cpssExMxIpHwLpmNodeRead(dev, memAddr, sramIndex,
                 nodeType, &rangeSelectSecGet, NULL, &numOfNextPointersGet);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, nextPointerArrayPtr = NULL", dev);


        /* 1.6. Check for NULL pointer. Call with numOfNextPointersPtr       */
        /* [NULL], other parameters same as in 1.2. Expected: GT_BAD_PTR.   */

        st = cpssExMxIpHwLpmNodeRead(dev, memAddr, sramIndex,
                 nodeType, &rangeSelectSecGet, nextPointerArrayGet, NULL);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, numOfNextPointersPtr = NULL", dev);

        /* 1.7. Check out-of-range pointingToSramIndex. Call with       */
        /* pointingToSramIndex [100], other parameters same as in 1.1.  */
        /* Expected: non GT_OK.                                         */

        /*
        pointingToSramIndex = 100;

        st = cpssExMxIpHwLpmNodeRead(dev, memAddr, sramIndex, pointingToSramIndex,
                 nodeType, &rangeSelectSecGet, nextPointerArrayGet, &numOfNextPointersGet);

        UTF_VERIFY_NOT_EQUAL2_STRING_MAC(GT_OK, st, "%d, pointingToSramIndex = %d",
                                         dev, pointingToSramIndex);

        pointingToSramIndex = 0;*/
    }

    memAddr = 1000;
    sramIndex = 0;
    pointingToSramIndex = 0;
    nodeType = CPSS_EXMX_IP_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
    nextPointerArray[0].pointerType = CPSS_EXMX_IP_REGULAR_NODE_PTR_TYPE_E;
    nextPointerArray[0].pointerData.routeEntryPointer.routeEntryBaseMemAddr = 1000;
    nextPointerArray[0].pointerData.routeEntryPointer.blockSize = 1;
    nextPointerArray[0].pointerData.routeEntryPointer.routeEntryMethod = CPSS_EXMX_IP_REG_ROUTE_ENTRY_E;
    numOfNextPointers = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpHwLpmNodeRead(dev, memAddr, sramIndex,
                 nodeType, &rangeSelectSecGet, nextPointerArrayGet, &numOfNextPointersGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpHwLpmNodeRead(dev, memAddr, sramIndex,
             nodeType, &rangeSelectSecGet, nextPointerArrayGet, &numOfNextPointersGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpMcTreeNodeWrite
(
    IN GT_U8                                          dev,
    IN GT_U32                                         memAddr,
    IN CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC          *nextPointerArrayPtr,
    IN GT_U32                                         numOfNextPointers
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpMcTreeNodeWrite)
{
/*
ITERATE_DEVICES 
1.1. Call function with memAddr [2048], nextPointerArrayPtr[0] [2048, 36, 40] , numOfNextPointers [1]. Expected: GT_BAD_PARAM  for Tiger devices and GT_OK for others (TWIST, TWIST-D , SAMBA/RUMBA).
1.2. For non-Tiger devices (TWIST, TWIST-D, SAMBA/RUMBA) call function cpssExMxIpMcTreeNodeRead with memAddr [2048], non-NULL nextPointerArrayPtr, numOfNextPointers [1]. Expected: GT_OK and the same values in nextPointerArrayPtr as was written.
1.3. Check for NULL pointer support. For non-Tiger devices call with memAddr [2048], nextPointerArrayPtr [NULL], numOfNextPointers [1]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                                 memAddr;
    CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC  nextPointerArray[1];
    CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC  nextPointerArrayGet[1];
    GT_U32                                 numOfNextPointers;
    GT_BOOL                                failureWas;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_NON_TIGER_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx (except Tiger) active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with memAddr [2048], nextPointerArrayPtr[0]
           [2048, 36, 40] , numOfNextPointers [1]. Expected: GT_BAD_PARAM for
           Tiger devices and GT_OK for others (TWIST, TWIST-D , SAMBA/RUMBA).*/

        memAddr = 2048;
        nextPointerArray[0].nextPointer = 2048;
        nextPointerArray[0].nextMcNodeMin = 36;
        nextPointerArray[0].nextMcNodeMax = 40;
        numOfNextPointers = 1;

        st = cpssExMxIpMcTreeNodeWrite(dev, memAddr, nextPointerArray,
                                       numOfNextPointers);

        UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st, "%d, %d, {%d, %d, %d}, %d",
             dev, memAddr, nextPointerArray[0].nextPointer,
             nextPointerArray[0].nextMcNodeMin, nextPointerArray[0].nextMcNodeMax,
             numOfNextPointers);

        /* 1.2. For non-Tiger devices (TWIST, TWIST-D, SAMBA/RUMBA) call
           function cpssExMxIpMcTreeNodeRead with memAddr [2048], non-NULL
           nextPointerArrayPtr, numOfNextPointers [1]. Expected: GT_OK and
           the same values in nextPointerArrayPtr as was written.   */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)nextPointerArrayGet, 0, 
                     numOfNextPointers*sizeof(CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC));

        st = cpssExMxIpMcTreeNodeRead(dev, memAddr, nextPointerArrayGet, numOfNextPointers);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpMcTreeNodeRead: %d, %d, numOfNextPointers = %d",
                   dev, memAddr, numOfNextPointers);

        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)nextPointerArray,
                                            (const GT_VOID*)nextPointerArrayGet, numOfNextPointers*sizeof(CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                "cpssExMxIpMcTreeNodeRead: get another nextPointerArray than was set: %d, %d, numOfNextPointers = %d",
                dev, memAddr, numOfNextPointers);
        }

        /* 1.3. Check for NULL pointer support. For non-Tiger devices call   */
        /* with memAddr [2048], nextPointerArrayPtr [NULL], numOfNextPointers*/
        /* [1]. Expected: GT_BAD_PTR.                                        */

        st = cpssExMxIpMcTreeNodeWrite(dev, memAddr, NULL, numOfNextPointers);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, nextPointerArrayPtr = NULL", dev);
    }

    memAddr = 2048;
    nextPointerArray[0].nextPointer = 2048;
    nextPointerArray[0].nextMcNodeMin = 36;
    nextPointerArray[0].nextMcNodeMax = 40;
    numOfNextPointers = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_NON_TIGER_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx/ Tiger devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpMcTreeNodeWrite(dev, memAddr, nextPointerArray,
                                       numOfNextPointers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpMcTreeNodeWrite(dev, memAddr, nextPointerArray,
                                   numOfNextPointers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpMcTreeNodeRead
(
    IN  GT_U8                                          dev,
    IN  GT_U32                                         memAddr,
    OUT CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC          *nextPointerArrayPtr,
    IN  GT_U32                                         numOfNextPointers
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpMcTreeNodeRead)
{
/*
ITERATE_DEVICES 
1.1. Write IpMcTreeNode for testing. For TWIST, TWIST-D, SAMBA/RUMBA devices call function cpssExMxIpMcTreeNodeWrite with memAddr [2048], nextPointerArrayPtr[0] [2048, 36, 40] , numOfNextPointers [1]. Expected: GT_OK.
1.2. Call with memAddr [2048], non-NULL nextPointerArrayPtr, numOfNextPointers [1]. Expected: GT_OK for TWIST, TWIST-D, SAMBA/RUMBA and GT_BAD_PARAM for others.
1.3. Check for NULL pointer support. For TWIST, TWIST-D, SAMBA/RUMBA devices call with memAddr [2048], nextPointerArrayPtr [NULL], numOfNextPointers [1]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                                 memAddr;
    CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC  nextPointerArray[1];
    CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC  nextPointerArrayGet[1];
    GT_U32                                 numOfNextPointers;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_NON_TIGER_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx (except Tiger) active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Write IpMcTreeNode for testing. For TWIST, TWIST-D, SAMBA/RUMBA
           devices call function cpssExMxIpMcTreeNodeWrite with memAddr [2048],
           nextPointerArrayPtr[0] [2048, 36, 40] , numOfNextPointers [1].
           Expected: GT_OK.     */

        memAddr = 2048;
        nextPointerArray[0].nextPointer = 2048;
        nextPointerArray[0].nextMcNodeMin = 36;
        nextPointerArray[0].nextMcNodeMax = 40;
        numOfNextPointers = 1;

        st = cpssExMxIpMcTreeNodeWrite(dev, memAddr, nextPointerArray,
                                       numOfNextPointers);

        UTF_VERIFY_EQUAL6_STRING_MAC(GT_OK, st, "cpssExMxIpMcTreeNodeWrite: %d, %d, {%d, %d, %d}, %d",
             dev, memAddr, nextPointerArray[0].nextPointer,
             nextPointerArray[0].nextMcNodeMin, nextPointerArray[0].nextMcNodeMax,
             numOfNextPointers);

        /* 1.2. Call with memAddr [2048], non-NULL nextPointerArrayPtr,
           numOfNextPointers [1]. Expected: GT_OK for TWIST, TWIST-D,
           SAMBA/RUMBA and GT_BAD_PARAM for others. */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)nextPointerArrayGet, 0, 
                     numOfNextPointers*sizeof(CPSS_EXMX_IP_MC_TREE_NEXT_POINTER_STC));

        st = cpssExMxIpMcTreeNodeRead(dev, memAddr, nextPointerArrayGet,
                                      numOfNextPointers);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, numOfNextPointers = %d",
                   dev, memAddr, numOfNextPointers);

        /* 1.3. Check for NULL pointer support. For TWIST, TWIST-D,         */
        /* SAMBA/RUMBA devices call with memAddr [2048], nextPointerArrayPtr*/
        /* [NULL], numOfNextPointers [1]. Expected: GT_BAD_PTR.             */

        st = cpssExMxIpMcTreeNodeRead(dev, memAddr, NULL, numOfNextPointers);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, nextPointerArrayPtr = NULL", dev);
    }

    memAddr = 2048;
    nextPointerArray[0].nextPointer = 2048;
    nextPointerArray[0].nextMcNodeMin = 36;
    nextPointerArray[0].nextMcNodeMax = 40;
    numOfNextPointers = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_NON_TIGER_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx/ Tiger devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxIpMcTreeNodeRead(dev, memAddr, nextPointerArrayGet,
                                      numOfNextPointers);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpMcTreeNodeRead(dev, memAddr, nextPointerArrayGet,
                                  numOfNextPointers);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpEcmpWARouteEntriesConvert
(
    IN  CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC routeEntriesArray[],
    IN  GT_U32                          numOfRouteEntries,
    OUT CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC ecmpWARouteEntriesArray[],
    OUT GT_U32                          *ecmpWANumOfRouteEntries
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpEcmpWARouteEntriesConvert)
{
/*
1. Call function with routeEntriesArrayPtr [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E, mirrorToAnalyzer=GT_TRUE, forceCos=GT_FALSE, trafficClass=0, dropPrecedence=CPSS_DP_GREEN_E, userPriority=0, ipCounterSet =CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE, enableDecTtlHopLimit=GT_TRUE, age=GT_TRUE, forceUserPriority=GT_FALSE, enableScopeCheck =GT_FALSE, destinationSiteId =CPSS_EXMX_IP_SITE_EXTERNAL_E, GT_FALSE, 128, nextHopInformation [outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo [ linkLayerOutLif [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[ tarInfo[tarIsTrunk=GT_FALSE, portInfo [0,0] ] ] ] ] ],arpPointer = 0 ], GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, 100,1,0, GT_FALSE ] , numOfRouteEntries [1], non-NULL ecmpWARouteEntriesArray and ecmpWANumOfRouteEntries. Expected: GT_OK.
2. Check just sizes convertion. Call with routeEntriesArray [NULL], numOfRouteEntries [6], ecmpWARouteEntriesArray[NULL], non-NULL ecmpWANumOfRouteEntries. Expected: GT_OK.
3. Check out-of-range numOfRouteEntries. Call with numOfRouteEntries [7], other params same as in 2. Expected: non GT_OK.
4. Check for NULL-pointer support. Call with ecmpWANumOfRouteEntries[NULL], other params same as in 1. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;

    CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC routeEntriesArray[1];
    GT_U32                          numOfRouteEntries;
    CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC ecmpWARouteEntriesArray[1];
    GT_U32                          ecmpWANumOfRouteEntries;

    /* No device parameter - no device iteration, no test pattern.. */
    
    /* 1. Call function with routeEntriesArrayPtr [0]
       [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E, mirrorToAnalyzer=GT_TRUE,
       forceCos=GT_FALSE, trafficClass=0, dropPrecedence=CPSS_DP_GREEN_E,
       userPriority=0, ipCounterSet =CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE,
       enableDecTtlHopLimit=GT_TRUE, age=GT_TRUE, forceUserPriority=GT_FALSE,
       enableScopeCheck =GT_FALSE, destinationSiteId =
       CPSS_EXMX_IP_SITE_EXTERNAL_E, GT_FALSE, 128, nextHopInformation
       [outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo [ linkLayerOutLif
       [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[ tarInfo[tarIsTrunk=
       GT_FALSE, portInfo [0,0] ] ] ] ] ],arpPointer = 0 ], GT_FALSE, GT_FALSE,
       GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, 100,1,0, GT_FALSE ] ,
       numOfRouteEntries [1], non-NULL ecmpWARouteEntriesArray and
       ecmpWANumOfRouteEntries. Expected: GT_OK.    */

    IpUcRouteEntryDefaultSet(routeEntriesArray);
    numOfRouteEntries = 1;

    st = cpssExMxIpEcmpWARouteEntriesConvert(routeEntriesArray,
            numOfRouteEntries, ecmpWARouteEntriesArray, &ecmpWANumOfRouteEntries);

    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "numOfRouteEntries = %d",
                                 numOfRouteEntries);

    /* 2. Check just sizes convertion. Call with routeEntriesArray [NULL],
       numOfRouteEntries [6], ecmpWARouteEntriesArray[NULL], non-NULL
       ecmpWANumOfRouteEntries. Expected: GT_OK.    */

    numOfRouteEntries = 6;
    st = cpssExMxIpEcmpWARouteEntriesConvert(NULL, numOfRouteEntries,
                                     NULL, &ecmpWANumOfRouteEntries);

    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, st, "routeEntriesArray = NULL, numOfRouteEntries = %d, ecmpWANumOfRouteEntries = NULL",
                                 numOfRouteEntries);

    /* 3. Check out-of-range numOfRouteEntries. Call with                    */
    /* numOfRouteEntries [7], other params same as in 2. Expected: non GT_OK.*/

    numOfRouteEntries = 7;
    st = cpssExMxIpEcmpWARouteEntriesConvert(NULL, numOfRouteEntries,
                                     NULL, &ecmpWANumOfRouteEntries);

    UTF_VERIFY_NOT_EQUAL1_STRING_MAC(GT_OK, st, "routeEntriesArray = NULL, numOfRouteEntries = %d, ecmpWANumOfRouteEntries = NULL",
                                 numOfRouteEntries);

    /* 4. Check for NULL-pointer support. Call with
          ecmpWANumOfRouteEntries[NULL], other params same as in 1.
          Expected: GT_BAD_PTR. */

    numOfRouteEntries = 1;

    st = cpssExMxIpEcmpWARouteEntriesConvert(routeEntriesArray,
            numOfRouteEntries, ecmpWARouteEntriesArray, NULL);

    UTF_VERIFY_EQUAL0_STRING_MAC(GT_BAD_PTR, st, "ecmpWANumOfRouteEntries = NULL");

}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpUcRouteEntriesWrite
(
    IN GT_U8                           dev,
    IN GT_U32                          baseMemAddr,
    IN CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC *routeEntriesArrayPtr,
    IN GT_U32                          numOfRouteEntries
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpUcRouteEntriesWrite)
{
/*
ITERATE_DEVICES 
1.1. Call function with baseMemAddr [1023], routeEntriesArrayPtr [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E, mirrorToAnalyzer=GT_TRUE, forceCos=GT_FALSE, trafficClass=0, dropPrecedence=CPSS_DP_GREEN_E, userPriority=0, ipCounterSet =CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE, enableDecTtlHopLimit=GT_TRUE, age=GT_TRUE, forceUserPriority=GT_FALSE, enableScopeCheck =GT_FALSE, destinationSiteId =CPSS_EXMX_IP_SITE_EXTERNAL_E, GT_FALSE, 128, nextHopInformation [outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo [ linkLayerOutLif [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[ tarInfo[tarIsTrunk=GT_FALSE, portInfo [0,0] ] ] ] ] ],arpPointer = 0 ], GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, 100,1,0, GT_FALSE ] , numOfRouteEntries [1]. Expected: GT_OK.
1.2. Call function cpssExMxIpUcRouteEntriesRead with baseMemAddr [1023], non-NULL routeEntriesArrayPtr, numOfRouteEntries [1]. Expected: GT_OK and the same values in routeEntriesArrayPtr as was written.
1.3. Check for NULL pointer support. Call with routeEntriesArrayPtr [NULL], other params same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                          baseMemAddr;
    CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC routeEntriesArray[1];
    CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC routeEntriesArrayGet[1];
    GT_U32                          numOfRouteEntries;

    GT_BOOL                         failureWas;

    baseMemAddr = 1023;
    IpUcRouteEntryDefaultSet(routeEntriesArray);
    numOfRouteEntries = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with baseMemAddr [1023], routeEntriesArrayPtr
          [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E, mirrorToAnalyzer=GT_TRUE,
          forceCos=GT_FALSE, trafficClass=0, dropPrecedence=CPSS_DP_GREEN_E,
          userPriority=0, ipCounterSet =CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE,
          enableDecTtlHopLimit=GT_TRUE, age=GT_TRUE, forceUserPriority=GT_FALSE,
          enableScopeCheck =GT_FALSE, destinationSiteId =
          CPSS_EXMX_IP_SITE_EXTERNAL_E, GT_FALSE, 128, nextHopInformation
          [outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo [ linkLayerOutLif
          [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[ tarInfo[tarIsTrunk=
          GT_FALSE, portInfo [0,0] ] ] ] ] ],arpPointer = 0 ], GT_FALSE,
          GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, 100,1,0, GT_FALSE ]
          , numOfRouteEntries [1]. Expected: GT_OK. */

        baseMemAddr = 1023;
        IpUcRouteEntryDefaultSet(routeEntriesArray);
        numOfRouteEntries = 1;

        st = cpssExMxIpUcRouteEntriesWrite(dev, baseMemAddr, routeEntriesArray,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, numOfRouteEntries = %d",
             dev, baseMemAddr, numOfRouteEntries);

        /* 1.2. Call function cpssExMxIpUcRouteEntriesRead with baseMemAddr
           [1023], non-NULL routeEntriesArrayPtr, numOfRouteEntries [1].
           Expected: GT_OK and the same values in routeEntriesArrayPtr
           as was written.  */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)routeEntriesArrayGet, 0, 
                     numOfRouteEntries*sizeof(CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC));

        st = cpssExMxIpUcRouteEntriesRead(dev, baseMemAddr, routeEntriesArrayGet,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpUcRouteEntriesRead: %d, %d, numOfRouteEntries = %d",
                   dev, baseMemAddr, numOfRouteEntries);

        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)routeEntriesArray,
                                            (const GT_VOID*)routeEntriesArrayGet, numOfRouteEntries*sizeof(CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                "cpssExMxIpUcRouteEntriesRead: get another routeEntriesArray than was set: %d, %d, numOfRouteEntries = %d",
                dev, baseMemAddr, numOfRouteEntries);
        }

        /* 1.3. Check for NULL pointer support. Call with routeEntriesArrayPtr
           [NULL], other params same as in 1.1. Expected: GT_BAD_PTR.   */

        st = cpssExMxIpUcRouteEntriesWrite(dev, baseMemAddr, NULL,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, routeEntriesArrayPtr = NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and     */
        /* valid parameters>.  Expected: GT_BAD_PARAM.                      */
        st = cpssExMxIpUcRouteEntriesWrite(dev, baseMemAddr, routeEntriesArray,
                                           numOfRouteEntries);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpUcRouteEntriesWrite(dev, baseMemAddr, routeEntriesArray,
                                       numOfRouteEntries);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpUcRouteEntriesRead
(
    IN GT_U8                           dev,
    IN GT_U32                          baseMemAddr,
    IN CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC *routeEntriesArrayPtr,
    IN GT_U32                          numOfRouteEntries
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpUcRouteEntriesRead)
{
/*
ITERATE_DEVICES 
1.1. Write IpUcRouteEntry for testing. Call function cpssExMxIpUcRouteEntriesWrite with baseMemAddr [1023], routeEntriesArrayPtr [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E, mirrorToAnalyzer=GT_TRUE, forceCos=GT_FALSE, trafficClass=0, dropPrecedence=CPSS_DP_GREEN_E, userPriority=0, ipCounterSet =CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE, enableDecTtlHopLimit=GT_TRUE, age=GT_TRUE, forceUserPriority=GT_FALSE, enableScopeCheck =GT_FALSE, destinationSiteId =CPSS_EXMX_IP_SITE_EXTERNAL_E, GT_FALSE, 128, nextHopInformation [outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo [ linkLayerOutLif [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[ tarInfo[tarIsTrunk=GT_FALSE, portInfo [0,0] ] ] ] ] ],arpPointer = 0 ], GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, 100,1,0, GT_FALSE ] , numOfRouteEntries [1]. Expected: GT_OK.
1.2. Call with baseMemAddr [1023], non-NULL routeEntriesArrayPtr, numOfRouteEntries [1]. Expected: GT_OK.
1.3. Check for NULL pointer support. Call with baseMemAddr [1023], routeEntriesArrayPtr [NULL], numOfRouteEntries [1]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                          baseMemAddr;
    CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC routeEntriesArray[1];
    CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC routeEntriesArrayGet[1];
    GT_U32                          numOfRouteEntries;

    baseMemAddr = 1023;
    IpUcRouteEntryDefaultSet(routeEntriesArray);
    numOfRouteEntries = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Write IpUcRouteEntry for testing. Call function
          cpssExMxIpUcRouteEntriesWrite with baseMemAddr [1023], routeEntriesArrayPtr
          [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E, mirrorToAnalyzer=GT_TRUE,
          forceCos=GT_FALSE, trafficClass=0, dropPrecedence=CPSS_DP_GREEN_E,
          userPriority=0, ipCounterSet =CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE,
          enableDecTtlHopLimit=GT_TRUE, age=GT_TRUE, forceUserPriority=GT_FALSE,
          enableScopeCheck =GT_FALSE, destinationSiteId =
          CPSS_EXMX_IP_SITE_EXTERNAL_E, GT_FALSE, 128, nextHopInformation
          [outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo [ linkLayerOutLif
          [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[ tarInfo[tarIsTrunk=
          GT_FALSE, portInfo [0,0] ] ] ] ] ],arpPointer = 0 ], GT_FALSE,
          GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, 100,1,0, GT_FALSE ]
          , numOfRouteEntries [1]. Expected: GT_OK. */

        /* baseMemAddr = 1023; */
        /* IpUcRouteEntryDefaultSet(routeEntriesArray); */
        /* numOfRouteEntries = 1; */

        st = cpssExMxIpUcRouteEntriesWrite(dev, baseMemAddr, routeEntriesArray,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpUcRouteEntriesWrite: %d, %d, numOfRouteEntries = %d",
             dev, baseMemAddr, numOfRouteEntries);

        /* 1.2. Call with baseMemAddr [1023], non-NULL routeEntriesArrayPtr,
           numOfRouteEntries [1]. Expected: GT_OK.  */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)routeEntriesArrayGet, 0, 
                     numOfRouteEntries*sizeof(CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC));

        st = cpssExMxIpUcRouteEntriesRead(dev, baseMemAddr, routeEntriesArrayGet,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, numOfRouteEntries = %d",
                   dev, baseMemAddr, numOfRouteEntries);

        /* 1.3. Check for NULL pointer support. Call with baseMemAddr [1023],
           routeEntriesArrayPtr [NULL], numOfRouteEntries [1].
           Expected: GT_BAD_PTR.    */

        st = cpssExMxIpUcRouteEntriesRead(dev, baseMemAddr, NULL,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, routeEntriesArrayPtr = NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and     */
        /* valid parameters>.  Expected: GT_BAD_PARAM.                      */
        st = cpssExMxIpUcRouteEntriesRead(dev, baseMemAddr, routeEntriesArrayGet,
                                           numOfRouteEntries);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpUcRouteEntriesRead(dev, baseMemAddr, routeEntriesArrayGet,
                                       numOfRouteEntries);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpRouteEntryActiveBitReadAndReset
(
    IN GT_U8                           dev,
    IN GT_U32                          memAddr,    
    IN GT_BOOL                         doReset,
    OUT GT_BOOL                         *activeBitPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpRouteEntryActiveBitReadAndReset)
{
/*
ITERATE_DEVICES 
1.1. Write IpUcRouteEntry for testing. Call function cpssExMxIpUcRouteEntriesWrite with baseMemAddr [2047], routeEntriesArrayPtr [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E, mirrorToAnalyzer=GT_TRUE, forceCos=GT_FALSE, trafficClass=0, dropPrecedence=CPSS_DP_GREEN_E, userPriority=0, ipCounterSet =CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE, enableDecTtlHopLimit=GT_TRUE, age=GT_TRUE, forceUserPriority=GT_FALSE, enableScopeCheck =GT_FALSE, destinationSiteId =CPSS_EXMX_IP_SITE_EXTERNAL_E, GT_FALSE, 128, nextHopInformation [outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo [ linkLayerOutLif [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[ tarInfo[tarIsTrunk=GT_FALSE, portInfo [0,0] ] ] ] ] ],arpPointer = 0 ], GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, 100,1,0, GT_FALSE ] , numOfRouteEntries [1]. Expected: GT_OK.
1.2. Write IpMcRouteEntry for testing. Call function cpssExMxIpMcRouteEntryWrite with baseMemAddr [4097], routeEntriesArrayPtr [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E, mirrorToAnalyzer=GT_TRUE, forceCos=GT_FALSE, trafficClass=0, dropPrecedence=CPSS_DP_GREEN_E, userPriority=0, ipCounterSet =CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE, enableDecTtlHopLimit=GT_TRUE, age=GT_TRUE, forceUserPriority=GT_FALSE, enableScopeCheck =GT_FALSE, destinationSiteId =CPSS_EXMX_IP_SITE_EXTERNAL_E, internalMllPointer=0, mtu=128, rpfCommand=CPSS_EXMX_IP_RPF_FAIL_NO_CHECK_E, rpfInlifVid=100, externalMllPointer=0, deviceVidx=100, pushMplsLabel=GT_FALSE, setExp=GT_FALSE, label=100, exp=1, ttlHopLimit=0, mcMirrorCpuCodeEnable = GT_FALSE, mirrorCmd[mirrorToCpu=GT_FALSE] ] , numOfRouteEntries [1]. Expected: GT_OK.
1.3. Check with uc (unicast) entry. Call with memAddr [2047], non-NULL activeBitPtr, doReset [GT_FALSE and GT_TRUE]. Expected: GT_OK.
1.4. Check with mc (multicast) entry. Call with memAddr [4097], non-NULL activeBitPtr, doReset [GT_FALSE and GT_TRUE]. Expected: GT_OK.
1.5. Check for NULL pointer support. Call with memAddr [2048], activeBitPtr [NULL], doReset [GT_FALSE]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                          memAddr;    
    GT_BOOL                         doReset;
    GT_BOOL                         activeBit;

    GT_U32                          baseMemAddr;
    CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC routeEntriesArrayUc[1];
    CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC routeEntriesArrayMc[1];
    GT_U32                          numOfRouteEntries;

    IpUcRouteEntryDefaultSet(routeEntriesArrayUc);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Write IpUcRouteEntry for testing. Call function
           cpssExMxIpUcRouteEntriesWrite with baseMemAddr [2047],
           routeEntriesArrayPtr [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E,
           mirrorToAnalyzer=GT_TRUE, forceCos=GT_FALSE, trafficClass=0,
           dropPrecedence=CPSS_DP_GREEN_E, userPriority=0, ipCounterSet =
           CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE, enableDecTtlHopLimit=GT_TRUE,
           age=GT_TRUE, forceUserPriority=GT_FALSE, enableScopeCheck =GT_FALSE,
           destinationSiteId =CPSS_EXMX_IP_SITE_EXTERNAL_E, GT_FALSE, 128,
           nextHopInformation [outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo
           [ linkLayerOutLif [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[ 
           tarInfo[tarIsTrunk=GT_FALSE, portInfo [0,0] ] ] ] ] ],arpPointer
            = 0 ], GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE,
           100,1,0, GT_FALSE ] , numOfRouteEntries [1]. Expected: GT_OK.    */

        baseMemAddr = 2047;
        numOfRouteEntries = 1;
        /* IpUcRouteEntryDefaultSet(routeEntriesArrayUc); */

        st = cpssExMxIpUcRouteEntriesWrite(dev, baseMemAddr, routeEntriesArrayUc,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpUcRouteEntriesWrite: %d, %d, numOfRouteEntries = %d",
             dev, baseMemAddr, numOfRouteEntries);

        /* 1.2. Write IpMcRouteEntry for testing. Call function
           cpssExMxIpMcRouteEntryWrite with baseMemAddr [4097],
           routeEntriesArrayPtr [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E,
           mirrorToAnalyzer=GT_TRUE, forceCos=GT_FALSE, trafficClass=0,
           dropPrecedence=CPSS_DP_GREEN_E, userPriority=0, ipCounterSet =
           CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE, enableDecTtlHopLimit=GT_TRUE,
           age=GT_TRUE, forceUserPriority=GT_FALSE, enableScopeCheck =GT_FALSE,
           destinationSiteId =CPSS_EXMX_IP_SITE_EXTERNAL_E, internalMllPointer
           =0, mtu=128, rpfCommand=CPSS_EXMX_IP_RPF_FAIL_NO_CHECK_E,
           rpfInlifVid=100, externalMllPointer=0, deviceVidx=100, pushMplsLabel
           =GT_FALSE, setExp=GT_FALSE, label=100, exp=1, ttlHopLimit=0,
           mcMirrorCpuCodeEnable = GT_FALSE, mirrorCmd[mirrorToCpu=GT_FALSE] ],
           numOfRouteEntries [1]. Expected: GT_OK.  */

        baseMemAddr = 4097;
        IpMcRouteEntryDefaultSet(routeEntriesArrayMc);
        numOfRouteEntries = 1;

        st = cpssExMxIpMcRouteEntryWrite(dev, baseMemAddr, routeEntriesArrayMc,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpMcRouteEntryWrite: %d, %d, numOfRouteEntries = %d",
             dev, baseMemAddr, numOfRouteEntries);

        /* 1.3. Check with uc (unicast) entry. Call with memAddr [2047],
           non-NULL activeBitPtr, doReset [GT_FALSE and GT_TRUE].
           Expected: GT_OK. */

        memAddr = 2047;
        doReset = GT_FALSE;

        cpssExMxIpRouteEntryActiveBitReadAndReset(dev, memAddr, doReset,
                                                  &activeBit);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, %d",
                                     dev, memAddr, doReset);

        doReset = GT_TRUE;

        cpssExMxIpRouteEntryActiveBitReadAndReset(dev, memAddr, doReset,
                                                  &activeBit);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, %d",
                                     dev, memAddr, doReset);

        /* 1.4. Check with mc (multicast) entry. Call with memAddr [4097],
           non-NULL activeBitPtr, doReset [GT_FALSE and GT_TRUE].
           Expected: GT_OK. */

        memAddr = 4097;
        doReset = GT_FALSE;

        cpssExMxIpRouteEntryActiveBitReadAndReset(dev, memAddr, doReset,
                                                  &activeBit);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, %d",
                                     dev, memAddr, doReset);

        doReset = GT_TRUE;

        cpssExMxIpRouteEntryActiveBitReadAndReset(dev, memAddr, doReset,
                                                  &activeBit);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, %d",
                                     dev, memAddr, doReset);

        /* 1.5. Check for NULL pointer support. Call with memAddr [2048],
           activeBitPtr [NULL], doReset [GT_FALSE]. Expected: GT_BAD_PTR.   */

        memAddr = 2048;
        doReset = GT_FALSE;
        cpssExMxIpRouteEntryActiveBitReadAndReset(dev, memAddr, doReset, NULL);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, activeBitPtr = NULL", dev);
    }

    memAddr = 2048;
    doReset = GT_FALSE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and     */
        /* valid parameters>.  Expected: GT_BAD_PARAM.                      */
        cpssExMxIpRouteEntryActiveBitReadAndReset(dev, memAddr, doReset,
                                                  &activeBit);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    cpssExMxIpRouteEntryActiveBitReadAndReset(dev, memAddr, doReset,
                                              &activeBit);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpRouteEntryFieldWrite
(
    IN GT_U8                                   dev,
    IN GT_U32                                  memAddr,
    IN CPSS_EXMX_IP_ROUTE_ENTRY_FIELD_TYPE_ENT fieldType,
    IN CPSS_EXMX_IP_ROUTE_ENTRY_FIELD_UNT      fieldValue
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpRouteEntryFieldWrite)
{
/*
ITERATE_DEVICES 
1.1. Call function with memAddr [1027], fieldType [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_FIELD_E], fieldValue [ cmd[CPSS_EXMX_IP_ROUTE_ENTRY_CMD_TRAP_2_CPU_E] ]. Expected: GT_OK.
1.2. Call function with memAddr [2048], fieldType [CPSS_EXMX_IP_ROUTE_ENTRY_MTU_FIELD_E], fieldValue [ mtu[128] ]. Expected: GT_OK.
1.3. Check out-of-range enum for fieldType. Call with memAddr [4096], fieldType [0x5AAAAAA5], fieldValue [ mtu[128] ]. Expected: GT_BAD_PARAM.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                                  memAddr;
    CPSS_EXMX_IP_ROUTE_ENTRY_FIELD_TYPE_ENT fieldType;
    CPSS_EXMX_IP_ROUTE_ENTRY_FIELD_UNT      fieldValue;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with memAddr [1027], fieldType
           [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_FIELD_E], fieldValue
           [ cmd[CPSS_EXMX_IP_ROUTE_ENTRY_CMD_TRAP_2_CPU_E] ].
           Expected: GT_OK. */

        memAddr = 1027;
        fieldType = CPSS_EXMX_IP_ROUTE_ENTRY_CMD_FIELD_E;
        fieldValue.cmd = CPSS_EXMX_IP_ROUTE_ENTRY_CMD_TRAP_2_CPU_E;

        st = cpssExMxIpRouteEntryFieldWrite(dev, memAddr, fieldType,
                                            fieldValue);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, %d",
                                     dev, memAddr, fieldType);

        /* 1.2. Call function with memAddr [2048], fieldType
          [CPSS_EXMX_IP_ROUTE_ENTRY_MTU_FIELD_E], fieldValue
          [ mtu[128] ]. Expected: GT_OK.    */

        memAddr = 2048;
        fieldType = CPSS_EXMX_IP_ROUTE_ENTRY_MTU_FIELD_E;
        fieldValue.mtu = 128;

        st = cpssExMxIpRouteEntryFieldWrite(dev, memAddr, fieldType,
                                            fieldValue);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, %d",
                                     dev, memAddr, fieldType);

        /* 1.3. Check out-of-range enum for fieldType. Call with memAddr
           [4096], fieldType [0x5AAAAAA5], fieldValue [ mtu[128] ].
           Expected: GT_BAD_PARAM.  */

        memAddr = 4096;
        fieldType = IP_INVALID_ENUM_CNS;

        st = cpssExMxIpRouteEntryFieldWrite(dev, memAddr, fieldType,
                                            fieldValue);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PARAM, st,
                                     "%d, fieldType = %d", dev, fieldType);
    }

    /* restore valid params */
    memAddr = 1024;
    fieldType = CPSS_EXMX_IP_ROUTE_ENTRY_CMD_FIELD_E;
    fieldValue.cmd = CPSS_EXMX_IP_ROUTE_ENTRY_CMD_TRAP_2_CPU_E;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and     */
        /* valid parameters>.  Expected: GT_BAD_PARAM.                      */
        st = cpssExMxIpRouteEntryFieldWrite(dev, memAddr, fieldType,
                                            fieldValue);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpRouteEntryFieldWrite(dev, memAddr, fieldType,
                                        fieldValue);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpRouteEntryMplsPushParamsWrite
(
    IN GT_U8                                      dev,
    IN GT_U32                                     memAddr,
    IN CPSS_EXMX_ROUTE_ENTRY_MPLS_PUSH_PARAMS_STC *mplsParamsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpRouteEntryMplsPushParamsWrite)
{
/*
ITERATE_DEVICES 
1.1. Call function with memAddr [1023], mplsParamsPtr  [pushMplsLabel=GT_FALSE, setExp=GT_FALSE, label=1000, exp=100, ttl=0]. Expected: GT_OK for SAMBA/RUMBA devices and GT_BAD_PARAM for others.
1.2. Check for NULL pointer support. For SAMBA/RUMBA devices call with memAddr [1024], mplsParamsPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                                     memAddr;
    CPSS_EXMX_ROUTE_ENTRY_MPLS_PUSH_PARAMS_STC mplsParams;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active Samba devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with memAddr [1023], mplsParamsPtr
          [pushMplsLabel=GT_FALSE, setExp=GT_FALSE, label=1000, exp=100,
          ttl=0]. Expected: GT_OK for SAMBA/RUMBA devices
          and GT_BAD_PARAM for others.  */

        memAddr = 1023;
        mplsParams.pushMplsLabel = GT_FALSE;
        mplsParams.setExp = GT_FALSE;
        mplsParams.label = 1000;
        mplsParams.exp = 100;
        mplsParams.ttl = 0;

        st = cpssExMxIpRouteEntryMplsPushParamsWrite(dev, memAddr, &mplsParams);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d", dev, memAddr);

        /* 1.2. Check for NULL pointer support. For SAMBA/RUMBA devices call
         with memAddr [1024], mplsParamsPtr [NULL]. Expected: GT_BAD_PTR.   */

        memAddr = 1024;

        st = cpssExMxIpRouteEntryMplsPushParamsWrite(dev, memAddr, NULL);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, mplsParamsPtr = NULL", dev);
    }

    memAddr = 1024;
    mplsParams.pushMplsLabel = GT_FALSE;
    mplsParams.setExp = GT_FALSE;
    mplsParams.label = 1000;
    mplsParams.exp = 100;
    mplsParams.ttl = 0;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_SAMBA_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non applicable devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and     */
        /* valid parameters>.  Expected: GT_BAD_PARAM.                      */
        st = cpssExMxIpRouteEntryMplsPushParamsWrite(dev, memAddr, &mplsParams);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpRouteEntryMplsPushParamsWrite(dev, memAddr, &mplsParams);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpUcRouteEntryTunnelTermWrite
(
    IN GT_U8                                           dev,
    IN GT_U32                                          memAddr,
    IN CPSS_EXMX_UC_ROUTE_ENTRY_TUNNEL_TERM_PARAMS_STC *tunnelTermInfoPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpUcRouteEntryTunnelTermWrite)
{
/*
ITERATE_DEVICES 
1.1. Call function with memAddr [1023], tunnelTermInfoPtr [greTunnelTerm=GT_FALSE, ipTunnelTerm=GT_TRUE, tunnelCopyDscpExp=GT_FALSE, tunnelCopyTtl=GT_TRUE]. Expected: GT_OK.
1.2. Check for NULL pointer support. Call with memAddr [1023], tunnelTermInfoPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                                          memAddr;
    CPSS_EXMX_UC_ROUTE_ENTRY_TUNNEL_TERM_PARAMS_STC tunnelTermInfo;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active ExMx devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with memAddr [1023], tunnelTermInfoPtr
          [greTunnelTerm=GT_FALSE, ipTunnelTerm=GT_TRUE, tunnelCopyDscpExp
          =GT_FALSE, tunnelCopyTtl=GT_TRUE]. Expected: GT_OK.   */

        memAddr = 1023;
        tunnelTermInfo.greTunnelTerm=GT_FALSE;
        tunnelTermInfo.ipTunnelTerm = GT_TRUE;
        tunnelTermInfo.tunnelCopyDscpExp = GT_FALSE;
        tunnelTermInfo.tunnelCopyTtl = GT_TRUE;

        st = cpssExMxIpUcRouteEntryTunnelTermWrite(dev, memAddr, &tunnelTermInfo);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d", dev, memAddr);

        /* 1.2. Check for NULL pointer support. Call with memAddr [1023],
           tunnelTermInfoPtr [NULL]. Expected: GT_BAD_PTR.  */

        st = cpssExMxIpUcRouteEntryTunnelTermWrite(dev, memAddr, NULL);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, tunnelTermInfoPtr = NULL", dev);
    }

    memAddr = 1024;
    tunnelTermInfo.greTunnelTerm=GT_FALSE;
    tunnelTermInfo.ipTunnelTerm = GT_TRUE;
    tunnelTermInfo.tunnelCopyDscpExp = GT_FALSE;
    tunnelTermInfo.tunnelCopyTtl = GT_TRUE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non applicable devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and     */
        /* valid parameters>.  Expected: GT_BAD_PARAM.                      */
        st = cpssExMxIpUcRouteEntryTunnelTermWrite(dev, memAddr, &tunnelTermInfo);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpUcRouteEntryTunnelTermWrite(dev, memAddr, &tunnelTermInfo);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpMcRouteEntryWrite
(
    IN GT_U8                           dev,
    IN GT_U32                          baseMemAddr,
    IN CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC *routeEntriesArrayPtr,
    IN GT_U32                          numOfRouteEntries
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpMcRouteEntryWrite)
{
/*
ITERATE_DEVICES 
1.1. Call function with baseMemAddr [1011], routeEntriesArrayPtr [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E, mirrorToAnalyzer=GT_TRUE, forceCos=GT_FALSE, trafficClass=0, dropPrecedence=CPSS_DP_GREEN_E, userPriority=0, ipCounterSet =CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE, enableDecTtlHopLimit=GT_TRUE, age=GT_TRUE, forceUserPriority=GT_FALSE, enableScopeCheck =GT_FALSE, destinationSiteId =CPSS_EXMX_IP_SITE_EXTERNAL_E, internalMllPointer=0, mtu=128, rpfCommand=CPSS_EXMX_IP_RPF_FAIL_NO_CHECK_E, rpfInlifVid=100, externalMllPointer=0, deviceVidx=100, pushMplsLabel=GT_FALSE, setExp=GT_FALSE, label=100, exp=1, ttlHopLimit=0, mcMirrorCpuCodeEnable = GT_FALSE,  mirrorCmd[mirrorToCpu=GT_FALSE] ] , numOfRouteEntries [1]. Expected: GT_OK.
1.2. Call function cpssExMxIpMcRouteEntryRead with baseMemAddr [1011], non-NULL routeEntriesArrayPtr, numOfRouteEntries [1]. Expected: GT_OK and the same values in routeEntriesArrayPtr as was written.
1.3. Check for NULL pointer support. Call with routeEntriesArrayPtr [NULL], other params same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                          baseMemAddr;
    CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC routeEntriesArray[1];
    CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC routeEntriesArrayGet[1];
    GT_U32                          numOfRouteEntries;

    GT_BOOL                         failureWas;

    baseMemAddr = 1011;
    IpMcRouteEntryDefaultSet(routeEntriesArray);
    numOfRouteEntries = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with baseMemAddr [1011], routeEntriesArrayPtr [0]
          [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E, mirrorToAnalyzer=GT_TRUE,
          forceCos=GT_FALSE, trafficClass=0, dropPrecedence=CPSS_DP_GREEN_E,
          userPriority=0, ipCounterSet =CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE,
          enableDecTtlHopLimit=GT_TRUE, age=GT_TRUE, forceUserPriority=GT_FALSE,
          enableScopeCheck =GT_FALSE, destinationSiteId =
          CPSS_EXMX_IP_SITE_EXTERNAL_E, internalMllPointer=0, mtu=128,
          rpfCommand=CPSS_EXMX_IP_RPF_FAIL_NO_CHECK_E, rpfInlifVid=100,
          externalMllPointer=0, deviceVidx=100, pushMplsLabel=GT_FALSE,
          setExp=GT_FALSE, label=100, exp=1, ttlHopLimit=0,
          mcMirrorCpuCodeEnable = GT_FALSE,  mirrorCmd[mirrorToCpu=GT_FALSE] ] ,
          numOfRouteEntries [1]. Expected: GT_OK.   */

        /* baseMemAddr = 1011; */
        /* IpMcRouteEntryDefaultSet(routeEntriesArray); */
        /* numOfRouteEntries = 1; */

        st = cpssExMxIpMcRouteEntryWrite(dev, baseMemAddr, routeEntriesArray,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, numOfRouteEntries = %d",
             dev, baseMemAddr, numOfRouteEntries);

        /* 1.2. Call function cpssExMxIpMcRouteEntryRead with baseMemAddr
           [1011], non-NULL routeEntriesArrayPtr, numOfRouteEntries [1].
           Expected: GT_OK and the same values in routeEntriesArrayPtr
           as was written.  */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)routeEntriesArrayGet, 0, 
                     numOfRouteEntries*sizeof(CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC));

        st = cpssExMxIpMcRouteEntryRead(dev, baseMemAddr, routeEntriesArrayGet,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpMcRouteEntryRead: %d, %d, numOfRouteEntries = %d",
                   dev, baseMemAddr, numOfRouteEntries);

        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)routeEntriesArray,
                                            (const GT_VOID*)routeEntriesArrayGet, numOfRouteEntries*sizeof(CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL3_STRING_MAC(GT_FALSE, failureWas,
                "cpssExMxIpMcRouteEntryRead: get another routeEntriesArray than was set: %d, %d, numOfRouteEntries = %d",
                dev, baseMemAddr, numOfRouteEntries);
        }

        /* 1.3. Check for NULL pointer support. Call with routeEntriesArrayPtr
           [NULL], other params same as in 1.1. Expected: GT_BAD_PTR.   */

        st = cpssExMxIpMcRouteEntryWrite(dev, baseMemAddr, NULL,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, routeEntriesArrayPtr = NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and     */
        /* valid parameters>.  Expected: GT_BAD_PARAM.                      */
        st = cpssExMxIpMcRouteEntryWrite(dev, baseMemAddr, routeEntriesArray,
                                           numOfRouteEntries);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpMcRouteEntryWrite(dev, baseMemAddr, routeEntriesArray,
                                       numOfRouteEntries);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpMcRouteEntryRead
(
    IN  GT_U8                           dev,
    IN  GT_U32                          baseMemAddr,
    OUT CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC *routeEntriesArrayPtr,
    IN  GT_U32                          numOfRouteEntries
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpMcRouteEntryRead)
{
/*
ITERATE_DEVICES 
1.1. Write IpMcRouteEntry for testing. Call function cpssExMxIpMcRouteEntryWrite with baseMemAddr [1024], routeEntriesArrayPtr [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E, mirrorToAnalyzer=GT_TRUE, forceCos=GT_FALSE, trafficClass=0, dropPrecedence=CPSS_DP_GREEN_E, userPriority=0, ipCounterSet =CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE, enableDecTtlHopLimit=GT_TRUE, age=GT_TRUE, forceUserPriority=GT_FALSE, enableScopeCheck =GT_FALSE, destinationSiteId =CPSS_EXMX_IP_SITE_EXTERNAL_E, internalMllPointer=0, mtu=128, rpfCommand=CPSS_EXMX_IP_RPF_FAIL_NO_CHECK_E, rpfInlifVid=100, externalMllPointer=0, deviceVidx=100, pushMplsLabel=GT_FALSE, setExp=GT_FALSE, label=100, exp=1, ttlHopLimit=0, mcMirrorCpuCodeEnable = GT_FALSE, mirrorCmd[mirrorToCpu=GT_FALSE] ] , numOfRouteEntries [1]. Expected: GT_OK.
1.2. Call with baseMemAddr [1024], non-NULL routeEntriesArrayPtr, numOfRouteEntries [1]. Expected: GT_OK.
1.3. Check for NULL pointer support. Call with baseMemAddr [1024], routeEntriesArrayPtr [NULL], numOfRouteEntries [1]. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                          baseMemAddr;
    CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC routeEntriesArray[1];
    CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC routeEntriesArrayGet[1];
    GT_U32                          numOfRouteEntries;

    baseMemAddr = 1024;
    IpMcRouteEntryDefaultSet(routeEntriesArray);
    numOfRouteEntries = 1;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Write IpMcRouteEntry for testing. Call function
           cpssExMxIpMcRouteEntryWrite with baseMemAddr [1024],
           routeEntriesArrayPtr [0] [CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E,
           mirrorToAnalyzer=GT_TRUE, forceCos=GT_FALSE, trafficClass=0,
           dropPrecedence=CPSS_DP_GREEN_E, userPriority=0, ipCounterSet =
           CPSS_EXMX_IP_CNT_SET0_E, GT_TRUE, enableDecTtlHopLimit=GT_TRUE,
           age=GT_TRUE, forceUserPriority=GT_FALSE, enableScopeCheck =GT_FALSE,
           destinationSiteId =CPSS_EXMX_IP_SITE_EXTERNAL_E, internalMllPointer
           =0, mtu=128, rpfCommand=CPSS_EXMX_IP_RPF_FAIL_NO_CHECK_E,
           rpfInlifVid=100, externalMllPointer=0, deviceVidx=100,
           pushMplsLabel=GT_FALSE, setExp=GT_FALSE, label=100, exp=1,
           ttlHopLimit=0, mcMirrorCpuCodeEnable = GT_FALSE,
           mirrorCmd[mirrorToCpu=GT_FALSE] ] ,
           numOfRouteEntries [1]. Expected: GT_OK.  */

        /* baseMemAddr = 1024; */
        /* IpMcRouteEntryDefaultSet(routeEntriesArray); */
        /* numOfRouteEntries = 1; */

        st = cpssExMxIpMcRouteEntryWrite(dev, baseMemAddr, routeEntriesArray,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "cpssExMxIpMcRouteEntryWrite: %d, %d, numOfRouteEntries = %d",
             dev, baseMemAddr, numOfRouteEntries);

        /* 1.2. Call with baseMemAddr [1024], non-NULL routeEntriesArrayPtr,
           numOfRouteEntries [1]. Expected: GT_OK.  */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)routeEntriesArrayGet, 0, 
                     numOfRouteEntries*sizeof(CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC));

        st = cpssExMxIpMcRouteEntryRead(dev, baseMemAddr, routeEntriesArrayGet,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, st, "%d, %d, numOfRouteEntries = %d",
                   dev, baseMemAddr, numOfRouteEntries);

        /* 1.3. Check for NULL pointer support. Call with baseMemAddr [1024],
           routeEntriesArrayPtr [NULL], numOfRouteEntries [1].
           Expected: GT_BAD_PTR.    */

        st = cpssExMxIpMcRouteEntryRead(dev, baseMemAddr, NULL,
                                           numOfRouteEntries);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, routeEntriesArrayPtr = NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and     */
        /* valid parameters>.  Expected: GT_BAD_PARAM.                      */
        st = cpssExMxIpMcRouteEntryRead(dev, baseMemAddr, routeEntriesArrayGet,
                                           numOfRouteEntries);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpMcRouteEntryRead(dev, baseMemAddr, routeEntriesArrayGet,
                                       numOfRouteEntries);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpMcLinkListWrite
(
    IN GT_U8                         dev,
    IN GT_U32                        memAddr,
    IN CPSS_EXMX_IP_MC_LINK_LIST_STC *mllEntry
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpMcLinkListWrite)
{
/*
ITERATE_DEVICES 
1.1. Call function with memAddr [1023], mllEntry [firstMllNode [last=GT_FALSE, outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo [ linkLayerOutLif [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[ tarInfo[tarIsTrunk=GT_FALSE, portInfo [0,0] ] ] ] ] ], hopLimitThreshold=5, excludeSrcVlan=GT_TRUE, rpfCmd= CPSS_EXMX_IP_MC_MLL_RPF_FAIL_BRIDGE_E ] , secondMllNode[..same as firstMllNode ..],  nextPointer=0]. Expected: GT_OK.
1.2. Call function cpssExMxIpMcLinkListRead with memAddr [1023], non-NULL mllEntry. Expected: GT_OK and the same values in mllEntry as was written.
1.3. Check for NULL pointer support. Call with mllEntry [NULL], other params same as in 1.1. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                          memAddr;
    CPSS_EXMX_IP_MC_LINK_LIST_STC   mllEntry;
    CPSS_EXMX_IP_MC_LINK_LIST_STC   mllEntryGet;

    GT_BOOL                         failureWas;

    memAddr = 1023;
    IpMcLinkListDefaultSet(&mllEntry);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call function with memAddr [1023], mllEntry [firstMllNode
           [last=GT_FALSE, outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo
           [ linkLayerOutLif [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[
           tarInfo[tarIsTrunk=GT_FALSE, portInfo [0,0] ] ] ] ] ],
           hopLimitThreshold=5, excludeSrcVlan=GT_TRUE, rpfCmd= 
           CPSS_EXMX_IP_MC_MLL_RPF_FAIL_BRIDGE_E ] , secondMllNode[..same
            as firstMllNode ..],  nextPointer=0]. Expected: GT_OK.  */

        /* memAddr = 1023; */
        /* IpMcLinkListDefaultSet(&mllEntry); */

        st = cpssExMxIpMcLinkListWrite(dev, memAddr, &mllEntry);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d", dev, memAddr);

        /* 1.2. Call function cpssExMxIpMcLinkListRead with memAddr [1023],
           non-NULL mllEntry. Expected: GT_OK and the same values in
           mllEntry as was written. */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)&mllEntryGet, 0, 
                     sizeof(CPSS_EXMX_IP_MC_LINK_LIST_STC));

        st = cpssExMxIpMcLinkListRead(dev, memAddr, &mllEntryGet);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxIpMcLinkListRead: %d, %d",
                                     dev, memAddr);

        if (GT_OK == st)
        {
            failureWas = (0 == cpssOsMemCmp((const GT_VOID*)&mllEntry,
                                            (const GT_VOID*)&mllEntryGet, sizeof(CPSS_EXMX_IP_MC_LINK_LIST_STC))) ? GT_FALSE : GT_TRUE;
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_FALSE, failureWas,
                "cpssExMxIpMcLinkListRead: get another mllEntry than was set: %d, %d",
                dev, memAddr);
        }

        /* 1.3. Check for NULL pointer support. Call with mllEntry [NULL],
           other params same as in 1.1. Expected: GT_BAD_PTR.   */

        st = cpssExMxIpMcLinkListWrite(dev, memAddr, NULL);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, mllEntryPtr = NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and     */
        /* valid parameters>.  Expected: GT_BAD_PARAM.                      */
        st = cpssExMxIpMcLinkListWrite(dev, memAddr, &mllEntry);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpMcLinkListWrite(dev, memAddr, &mllEntry);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpMcLinkListRead
(
    IN  GT_U8                         dev,
    IN  GT_U32                        memAddr,
    OUT CPSS_EXMX_IP_MC_LINK_LIST_STC *mllEntryPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpMcLinkListRead)
{
/*
ITERATE_DEVICES 
1.1. Write McLinkList entry for testing. Call function cpssExMxIpMcLinkListWrite with memAddr [1024], mllEntry [firstMllNode [last=GT_FALSE, outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo [ linkLayerOutLif [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[ tarInfo[tarIsTrunk=GT_FALSE, portInfo [0,0] ] ] ] ] ], hopLimitThreshold=5, excludeSrcVlan=GT_TRUE, rpfCmd= CPSS_EXMX_IP_MC_MLL_RPF_FAIL_BRIDGE_E ] , secondMllNode[..same as firstMllNode ..],  nextPointer=0]. Expected: GT_OK.
1.2. Call with memAddr [1024], non-NULL mllEntryPtr. Expected: GT_OK.
1.3. Check for NULL pointer support. Call with memAddr [1024], mllEntryPtr [NULL. Expected: GT_BAD_PTR.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                          memAddr;
    CPSS_EXMX_IP_MC_LINK_LIST_STC   mllEntry;
    CPSS_EXMX_IP_MC_LINK_LIST_STC   mllEntryGet;

    memAddr = 1024;
    IpMcLinkListDefaultSet(&mllEntry);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Write McLinkList entry for testing. Call function
           cpssExMxIpMcLinkListWrite with memAddr [1024], mllEntry [firstMllNode
           [last=GT_FALSE, outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo
           [ linkLayerOutLif [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[
           tarInfo[tarIsTrunk=GT_FALSE, portInfo [0,0] ] ] ] ] ],
           hopLimitThreshold=5, excludeSrcVlan=GT_TRUE, rpfCmd= 
           CPSS_EXMX_IP_MC_MLL_RPF_FAIL_BRIDGE_E ] , secondMllNode[..same
            as firstMllNode ..],  nextPointer=0]. Expected: GT_OK.  */

        /* memAddr = 1024; */
        /* IpMcLinkListDefaultSet(&mllEntry); */

        st = cpssExMxIpMcLinkListWrite(dev, memAddr, &mllEntry);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxIpMcLinkListWrite: %d, %d", dev, memAddr);

        /*1.2. Call with memAddr [1024], non-NULL mllEntryPtr.
               Expected: GT_OK. */

        /* Fill returned structures with zero */
        cpssOsMemSet((GT_VOID*)&mllEntryGet, 0, 
                     sizeof(CPSS_EXMX_IP_MC_LINK_LIST_STC));

        st = cpssExMxIpMcLinkListRead(dev, memAddr, &mllEntryGet);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "%d, %d",
                                     dev, memAddr);

        /* 1.3. Check for NULL pointer support. Call with mllEntry [NULL],
           other params same as in 1.1. Expected: GT_BAD_PTR.   */

        st = cpssExMxIpMcLinkListRead(dev, memAddr, NULL);

        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st,
                                     "%d, mllEntryPtr = NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and     */
        /* valid parameters>.  Expected: GT_BAD_PARAM.                      */
        st = cpssExMxIpMcLinkListRead(dev, memAddr, &mllEntryGet);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpMcLinkListRead(dev, memAddr, &mllEntryGet);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*
GT_STATUS cpssExMxIpMcLinkListLastBitWrite
(
    IN GT_U8                         dev,
    IN GT_U32                        memAddr,
    IN GT_BOOL                       lastBit,
    IN GT_BOOL                       writeToFirst
)
*/
UTF_TEST_CASE_MAC(cpssExMxIpMcLinkListLastBitWrite)
{
/*
ITERATE_DEVICES 
1.1. Write McLinkList entry for testing. Call function cpssExMxIpMcLinkListWrite with memAddr [1023], mllEntry [firstMllNode [last=GT_FALSE, outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo [ linkLayerOutLif [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[ tarInfo[tarIsTrunk=GT_FALSE, portInfo [0,0] ] ] ] ] ], hopLimitThreshold=5, excludeSrcVlan=GT_TRUE, rpfCmd= CPSS_EXMX_IP_MC_MLL_RPF_FAIL_BRIDGE_E ] , secondMllNode[..same as firstMllNode ..],  nextPointer=0]. Expected: GT_OK.
1.2. Call with memAddr [1023], lastBit [GT_TRUE], writeToFirst[GT_TRUE and GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS                   st = GT_OK;
    GT_U8                       dev;

    GT_U32                          memAddr;
    CPSS_EXMX_IP_MC_LINK_LIST_STC   mllEntry;

    GT_BOOL                         lastBit;
    GT_BOOL                         writeToFirst;

    memAddr = 1023;
    IpMcLinkListDefaultSet(&mllEntry);

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all ExMx active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Write McLinkList entry for testing. Call function
           cpssExMxIpMcLinkListWrite with memAddr [1023], mllEntry [firstMllNode
           [last=GT_FALSE, outlif [CPSS_EXMX_IP_OUTLIF_LL_E, outlifInfo
           [ linkLayerOutLif [vid=100, useVidx = GT_FALSE, vidxOrTrunkPort[
           tarInfo[tarIsTrunk=GT_FALSE, portInfo [0,0] ] ] ] ] ],
           hopLimitThreshold=5, excludeSrcVlan=GT_TRUE, rpfCmd= 
           CPSS_EXMX_IP_MC_MLL_RPF_FAIL_BRIDGE_E ] , secondMllNode[..same
            as firstMllNode ..],  nextPointer=0]. Expected: GT_OK.  */

        /* memAddr = 1023; */
        /* IpMcLinkListDefaultSet(&mllEntry); */

        st = cpssExMxIpMcLinkListWrite(dev, memAddr, &mllEntry);

        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxIpMcLinkListWrite: %d, %d", dev, memAddr);

        /* 1.2. Call with memAddr [1023], lastBit [GT_TRUE],
           writeToFirst[GT_TRUE and GT_FALSE]. Expected: GT_OK. */

        lastBit = GT_TRUE;
        writeToFirst = GT_TRUE;

        st = cpssExMxIpMcLinkListLastBitWrite(dev, memAddr, lastBit, writeToFirst);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, memAddr, lastBit, writeToFirst);

        writeToFirst = GT_FALSE;

        st = cpssExMxIpMcLinkListLastBitWrite(dev, memAddr, lastBit, writeToFirst);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, memAddr, lastBit, writeToFirst);
    }

    lastBit = GT_TRUE;
    writeToFirst = GT_TRUE;

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /*2. Go over all non active/non ExMx devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active/non applicable device and     */
        /* valid parameters>.  Expected: GT_BAD_PARAM.                      */
        st = cpssExMxIpMcLinkListLastBitWrite(dev, memAddr, lastBit, writeToFirst);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxIpMcLinkListLastBitWrite(dev, memAddr, lastBit, writeToFirst);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxIp suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxIp)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpHwLpmNodeNextPointersWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpHwLpmNodeWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpHwLpmNodeRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpMcTreeNodeWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpMcTreeNodeRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpEcmpWARouteEntriesConvert)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpUcRouteEntriesWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpUcRouteEntriesRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpRouteEntryActiveBitReadAndReset)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpRouteEntryFieldWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpRouteEntryMplsPushParamsWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpUcRouteEntryTunnelTermWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpMcRouteEntryWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpMcRouteEntryRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpMcLinkListWrite)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpMcLinkListRead)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxIpMcLinkListLastBitWrite)
UTF_SUIT_END_TESTS_MAC(cpssExMxIp)

/*  Internal function. Is used for filling CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC  */
/*  structure with default values which are used in many tests.             */
static void IpUcRouteEntryDefaultSet
(
    IN  CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC *routeEntriesArray
)
{
    cpssOsBzero((GT_VOID*)routeEntriesArray, sizeof(CPSS_EXMX_IP_UC_ROUTE_ENTRY_STC));
    routeEntriesArray[0].cmd = CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E;
    routeEntriesArray[0].mirrorToAnalyzer = GT_TRUE;
    routeEntriesArray[0].forceCos = GT_FALSE;
    routeEntriesArray[0].trafficClass = 0;
    routeEntriesArray[0].dropPrecedence = CPSS_DP_GREEN_E;
    routeEntriesArray[0].userPriority = 0;
    routeEntriesArray[0].ipCounterSet = CPSS_EXMX_IP_CNT_SET0_E;
    routeEntriesArray[0].enableDecTtlHopLimit = GT_TRUE;
    routeEntriesArray[0].age = GT_TRUE;
    routeEntriesArray[0].forceUserPriority = GT_FALSE;
    routeEntriesArray[0].enableScopeCheck = GT_FALSE;
    routeEntriesArray[0].destinationSiteId = CPSS_EXMX_IP_SITE_EXTERNAL_E;
    routeEntriesArray[0].enableICMPRedirectHook = GT_FALSE;
    routeEntriesArray[0].mtu = 128;
    IpOutlifConfigDefaultSet(&routeEntriesArray[0].nextHopInformation.outlif);
    routeEntriesArray[0].nextHopInformation.arpPointer = 0;
    routeEntriesArray[0].greIPv4TunnelTerminate = GT_FALSE;
    routeEntriesArray[0].ipIPv4TunnelTerminate = GT_FALSE;
    routeEntriesArray[0].tunnelCopyDscp = GT_FALSE;
    routeEntriesArray[0].tunnelCopyTtl = GT_FALSE;
    routeEntriesArray[0].pushMplsLabel = GT_FALSE;
    routeEntriesArray[0].setExp = GT_FALSE;
    routeEntriesArray[0].label = 100;
    routeEntriesArray[0].exp = 1;
    routeEntriesArray[0].ttlHopLimit = 0;
    routeEntriesArray[0].mirrorToCpu = GT_FALSE;
}


/*  Internal function. Is used for filling CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC  */
/*  structure with default values which are used in many tests.             */
static void IpMcRouteEntryDefaultSet
(
    IN  CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC *routeEntriesArray
)
{
    cpssOsBzero((GT_VOID*)routeEntriesArray, sizeof(CPSS_EXMX_IP_MC_ROUTE_ENTRY_STC));

    /* ====  similar to UC  */
    routeEntriesArray[0].cmd = CPSS_EXMX_IP_ROUTE_ENTRY_CMD_ROUTE_E;
    routeEntriesArray[0].mirrorToAnalyzer = GT_TRUE;
    routeEntriesArray[0].forceCos = GT_FALSE;
    routeEntriesArray[0].trafficClass = 0;
    routeEntriesArray[0].dropPrecedence = CPSS_DP_GREEN_E;
    routeEntriesArray[0].userPriority = 0;
    routeEntriesArray[0].ipCounterSet = CPSS_EXMX_IP_CNT_SET0_E;
    routeEntriesArray[0].enableDecTtlHopLimit = GT_TRUE;
    routeEntriesArray[0].age = GT_TRUE;
    routeEntriesArray[0].forceUserPriority = GT_FALSE;
    routeEntriesArray[0].enableScopeCheck = GT_FALSE;
    routeEntriesArray[0].destinationSiteId = CPSS_EXMX_IP_SITE_EXTERNAL_E;

     /* =====  not similar to UC..  */

    routeEntriesArray[0].internalMllPointer = 0;
    routeEntriesArray[0].mtu = 128;
    routeEntriesArray[0].rpfCommand = CPSS_EXMX_IP_RPF_FAIL_NO_CHECK_E;
    routeEntriesArray[0].rpfInlifVid = 100;
    routeEntriesArray[0].externalMllPointer = 0;
    routeEntriesArray[0].deviceVidx = 100;

    routeEntriesArray[0].pushMplsLabel = GT_FALSE;
    routeEntriesArray[0].setExp = GT_FALSE;
    routeEntriesArray[0].label = 100;
    routeEntriesArray[0].exp = 1;
    
    routeEntriesArray[0].ttlHopLimit = 0;
    routeEntriesArray[0].mcMirrorCpuCodeEnable = GT_FALSE;
    routeEntriesArray[0].mirrorCmd.mirrorToCpu = GT_FALSE;
}

/*  Internal function. Is used for filling OUTLIF CONFIG structure  */
/*  with default values which are used for most of all tests.       */
static void IpOutlifConfigDefaultSet
(
    IN CPSS_EXMX_IP_OUTLIF_CONFIG_STC  *outlifPtr
)
{
    cpssOsBzero((GT_VOID*)outlifPtr, sizeof(CPSS_EXMX_IP_OUTLIF_CONFIG_STC));

    outlifPtr->outlifType = CPSS_EXMX_IP_OUTLIF_LL_E;  /*  (Link layer)    */

    outlifPtr->outlifInfo.linkLayerOutLif.vid = 100;
    outlifPtr->outlifInfo.linkLayerOutLif.useVidx = GT_FALSE;

    outlifPtr->outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.tarIsTrunk = GT_FALSE;

    outlifPtr->outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarPort = 0;
    outlifPtr->outlifInfo.linkLayerOutLif.vidxOrTrunkPort.tarInfo.trunkOrPort.portInfo.tarDev = 0;
}

/*  Internal function. Is used for filling OUTLIF CONFIG structure  */
/*  with default values which are used for most of all tests.       */
static void IpMcLinkListDefaultSet
(
    IN CPSS_EXMX_IP_MC_LINK_LIST_STC  *mllEntryPtr
)
{
    cpssOsBzero((GT_VOID*)mllEntryPtr, sizeof(CPSS_EXMX_IP_MC_LINK_LIST_STC));

    mllEntryPtr->firstMllNode.last = GT_FALSE;
    IpOutlifConfigDefaultSet(&mllEntryPtr->firstMllNode.outlif);
    mllEntryPtr->firstMllNode.hopLimitThreshold = 5;
    mllEntryPtr->firstMllNode.excludeSrcVlan = GT_TRUE;
    mllEntryPtr->firstMllNode.rpfCmd = CPSS_EXMX_IP_MC_MLL_RPF_FAIL_BRIDGE_E;

    mllEntryPtr->secondMllNode.last = GT_FALSE;
    IpOutlifConfigDefaultSet(&mllEntryPtr->firstMllNode.outlif);
    mllEntryPtr->secondMllNode.hopLimitThreshold = 5;
    mllEntryPtr->secondMllNode.excludeSrcVlan = GT_TRUE;
    mllEntryPtr->secondMllNode.rpfCmd = CPSS_EXMX_IP_MC_MLL_RPF_FAIL_BRIDGE_E;

    mllEntryPtr->nextPointer = 0;
}
