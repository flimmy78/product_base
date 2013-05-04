/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfPortGroupWraparound.c
*
* DESCRIPTION:
*       IPFIX Wraparound per port group test for IPFIX
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/pcl/cpssPcl.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfPclGen.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfPolicerGen.h>
#include <common/tgfIpfixGen.h>

#include <ipfix/prvTgfBasicIngressPclKeys.h>
#include <ipfix/prvTgfBasicTests.h>
#include <ipfix/prvTgfPortGroupEntry.h>

/*******************************************************************************
* prvTgfIpfixPortGroupWraparoundCheck
*
* DESCRIPTION:
*       Check that the 1st bit in a requested wraparound bitmap is the same as
*       an expected one.
*
* INPUTS:
*       enPortGroupsBmp - state if to uses port groups bmp.
*       portGroupsBmp   - bmp of port groups to use in test.
*       reset           - reset bmp value wraparound after read.
*       expectedBmp     - the expected value.
*       stepNum         - printing mark auxilery.
*       bookmark        - second printing mark auxilery.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvTgfIpfixPortGroupWraparoundCheck
(
    GT_BOOL     enPortGroupsBmp,
    GT_PORT_GROUPS_BMP  portGroupsBmp,
    GT_BOOL     reset,
    GT_U32      expectedBmp,
    GT_U32      stepNum,
    GT_U32      bookmark
)
{
    GT_STATUS rc;
    GT_U32    bmp[1];

    /* set PortGroupsBmp mode */
    usePortGroupsBmp  = enPortGroupsBmp;
    currPortGroupsBmp = portGroupsBmp;

    /* check Wraparound bit map */
    rc = prvTgfIpfixWraparoundStatusGet(prvTgfDevNum, testedStage,
                                        basicTestConf[0].ipfixIndex,basicTestConf[0].ipfixIndex,
                                        reset, bmp);
    UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, rc, "prvTgfIpfixWraparoundStatusGet: %d, %d, %d, %d, %d, %d, %d, %d",
                                 prvTgfDevNum, testedStage,
                                 basicTestConf[0].ipfixIndex,basicTestConf[2].ipfixIndex,
                                 reset, stepNum, bookmark);

    UTF_VERIFY_EQUAL2_STRING_MAC(expectedBmp, (bmp[0] & 0x1), "Wraparound indication expected: %d, %d",
                                 stepNum, bookmark);
}

/*******************************************************************************
* prvTgfIpfixPortGroupWraparoundTestTrafficGenerate
*
* DESCRIPTION:
*       Generate traffic
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
*
*******************************************************************************/
GT_VOID prvTgfIpfixPortGroupWraparoundTestTrafficGenerate
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;
    PRV_TGF_IPFIX_ENTRY_STC ipfixEntry;
    PRV_TGF_IPFIX_WRAPAROUND_CFG_STC wraparounfConf;
    GT_U32      portIter, portIterJ;
    GT_U32      bmp[1];
    GT_U32      stepNum;

    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);

    /* 64 bytes packet length */
    prvTgfPacketPayloadPart.dataLength = basicTestConf[0].payloadLen;
    prvTgfPacketInfo.totalLen =
            TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + prvTgfPacketPayloadPart.dataLength;

    /* Set ethertype to value 0x1000 */
    prvTgfPayloadDataArr[0] = 0x10;
    prvTgfPayloadDataArr[1] = 0x00;

    /* clear Wraparound bit map */
    rc = prvTgfIpfixWraparoundStatusGet(prvTgfDevNum, testedStage,
                                        basicTestConf[0].ipfixIndex,basicTestConf[0].ipfixIndex,
                                        GT_TRUE, bmp);
    UTF_VERIFY_EQUAL5_STRING_MAC(GT_OK, rc, "prvTgfIpfixWraparoundStatusGet: %d, %d, %d, %d, %d",
                                 prvTgfDevNum, testedStage,
                                 basicTestConf[0].ipfixIndex,basicTestConf[2].ipfixIndex,
                                 GT_TRUE);

    /* set wraparound treshold configuration */
    wraparounfConf.action = PRV_TGF_IPFIX_WRAPAROUND_ACTION_NONE_E;
    wraparounfConf.dropThreshold = 0x3FFFFFFF;
    wraparounfConf.packetThreshold = 10;
    wraparounfConf.byteThreshold.l[0] = 0xFFFFFFFF;
    wraparounfConf.byteThreshold.l[1] = 0xF;

    /* set wraparound configuration */
    rc = prvTgfIpfixWraparoundConfSet(prvTgfDevNum, testedStage, &wraparounfConf);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfIpfixWraparoundConfSet: %d, %d",
                                 prvTgfDevNum, testedStage);

    cpssOsMemSet(&ipfixEntry, 0, sizeof(ipfixEntry));
    ipfixEntry.packetCount = 9*4;
    rc = prvTgfIpfixEntrySet(prvTgfDevNum, testedStage,
                             basicTestConf[0].ipfixIndex, &ipfixEntry);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfIpfixEntrySet: %d, %d, %d",
                                 prvTgfDevNum, testedStage, basicTestConf[0].ipfixIndex);

    prvTgfIpfixPortGroupWraparoundCheck(GT_FALSE, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        GT_FALSE, 0, 0, /* bookmark */1);

    for( portIter = 0 ; portIter < prvTgfPortsNum ; portIter++ )
    {
        prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, (1 << portIter), GT_FALSE, 0, portIter, /* bookmark */2);
    }

    for( portIter = 0 ; portIter < prvTgfPortsNum ; portIter++ )
    {
        /* send packet */
        prvTgfIpfixTestPacketSend(prvTgfPortsArray[portIter], &prvTgfPacketInfo, 1);

        cpssOsTimerWkAfter(400);

        prvTgfIpfixPortGroupWraparoundCheck(GT_FALSE, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            GT_FALSE, 1, portIter, /* bookmark */3);

        for( portIterJ = 0 ; portIterJ < prvTgfPortsNum ; portIterJ++ )
        {
            /* use PortGroupsBmp API in the test */
            usePortGroupsBmp  = GT_TRUE;
            currPortGroupsBmp = (1 << portIterJ);

            /* check Wraparound bit map */
            rc = prvTgfIpfixWraparoundStatusGet(prvTgfDevNum, testedStage,
                                                basicTestConf[0].ipfixIndex,basicTestConf[0].ipfixIndex,
                                                GT_FALSE, bmp);
            UTF_VERIFY_EQUAL7_STRING_MAC(GT_OK, rc, "prvTgfIpfixWraparoundStatusGet: %d, %d, %d, %d, %d, %d, %d",
                                         prvTgfDevNum, testedStage,
                                         basicTestConf[0].ipfixIndex,basicTestConf[2].ipfixIndex,
                                         GT_FALSE, portIter, portIterJ);

            UTF_VERIFY_EQUAL2_STRING_MAC((( portIterJ == portIter ) ? 1 : 0),
                                         (bmp[0] & 0x1),
                                         "Wraparound indication expected: %d, %d", portIter, portIterJ);
        }

        prvTgfIpfixPortGroupWraparoundCheck(GT_FALSE, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                            GT_TRUE, 1, portIter, /* bookmark */4);

        prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, (1 << portIter), GT_FALSE, 0, portIter, /* bookmark */5);
    }

    usePortGroupsBmp  = GT_FALSE;
    currPortGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    rc = prvTgfIpfixEntrySet(prvTgfDevNum, testedStage,
                             basicTestConf[0].ipfixIndex, &ipfixEntry);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "prvTgfIpfixEntrySet: %d, %d, %d",
                                 prvTgfDevNum, testedStage, basicTestConf[0].ipfixIndex);

    /* setting the ipfix entry is causing the wraparound bit to be set again, therefore */
    /* it must be reset again. */
#ifndef ASIC_SIMULATION
    prvTgfIpfixPortGroupWraparoundCheck(GT_FALSE, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        GT_TRUE, 1, 0, /* bookmark */6);
#endif /* ASIC_SIMULATION */

    prvTgfIpfixPortGroupWraparoundCheck(GT_FALSE, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        GT_FALSE, 0, 0, /* bookmark */7);

    for( portIter = 0 ; portIter < prvTgfPortsNum ; portIter++ )
    {
        prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, (1 << portIter), GT_FALSE, 0, portIter, /* bookmark */8);
    }

    /* send packet on port 0 & 36 */
    prvTgfIpfixTestPacketSend(prvTgfPortsArray[0], &prvTgfPacketInfo, 1);
    cpssOsTimerWkAfter(400);
    prvTgfIpfixTestPacketSend(prvTgfPortsArray[2], &prvTgfPacketInfo, 1);
    cpssOsTimerWkAfter(400);

    /* step 1 */
    stepNum = 1;
    prvTgfIpfixPortGroupWraparoundCheck(GT_FALSE, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        GT_FALSE, 1, stepNum++, /* bookmark */0);

    /* step 2 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_0, GT_FALSE, 1, stepNum++, /* bookmark */0);

    /* step 3 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_1, GT_FALSE, 0, stepNum++, /* bookmark */0);

    /* step 4 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_2, GT_FALSE, 1, stepNum++, /* bookmark */0);

    /* step 5 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_3, GT_FALSE, 0, stepNum++, /* bookmark */0);

    /* step 6 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_0, GT_TRUE, 1, stepNum++, /* bookmark */0);

    /* step 7 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_1, GT_FALSE, 0, stepNum++, /* bookmark */0);

    /* step 8 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_2, GT_FALSE, 1, stepNum++, /* bookmark */0);

    /* step 9 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_3, GT_FALSE, 0, stepNum++, /* bookmark */0);

    /* step 10 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_FALSE, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        GT_FALSE, 1, stepNum++, /* bookmark */0);
    /* step 11 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_2, GT_TRUE, 1, stepNum++, /* bookmark */0);

    /* step 12 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_0, GT_FALSE, 0, stepNum++, /* bookmark */0);

    /* step 13 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_1, GT_FALSE, 0, stepNum++, /* bookmark */0);

    /* step 14 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_2, GT_FALSE, 0, stepNum++, /* bookmark */0);

    /* step 15 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_TRUE, BIT_3, GT_FALSE, 0, stepNum++, /* bookmark */0);

    /* step 16 */
    prvTgfIpfixPortGroupWraparoundCheck(GT_FALSE, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        GT_FALSE, 0, stepNum++, /* bookmark */0);

    /* restore PortGroupsBmp mode */
    usePortGroupsBmp  = GT_FALSE;
    currPortGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }
}
