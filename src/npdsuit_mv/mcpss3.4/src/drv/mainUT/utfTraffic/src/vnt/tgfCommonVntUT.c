/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* tgfCommonVntUT.c
*
* DESCRIPTION:
*       Enhanced UTs for CPSS Vnt
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <common/tgfCommon.h>

#include <vnt/prvTgfVnt.h>

/*----------------------------------------------------------------------------*/
/*
    tgfVntOamPortPduTrapEnable
*/
UTF_TEST_CASE_MAC(tgfVntOamPortPduTrapEnable)
{
/********************************************************************
    Test 7.1 - Oam Port Pdu Trap Enable

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E);

    /* Set configuration */
    prvTgfVntOamPortPduTrapSet();

    /* Generate traffic */
    prvTgfVntOamPortPduTrapTrafficGenerate();

    /* Restore configuration */
    prvTgfVntOamPortPduTrapRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVntLoopbackConfiguration
*/
UTF_TEST_CASE_MAC(tgfVntLoopbackConfiguration)
{
/********************************************************************
    Test 7.2 - Vnt Loopback Mode Configuration

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    /* Set configuration */
    prvTgfVntLoopbackModeConfSet();

    /* Generate traffic */
    prvTgfVntLoopbackTrafficConfGenerate();

    /* Restore configuration */
    prvTgfVntLoopbackConfRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVntCfmEtherTypeIdentification
*/
UTF_TEST_CASE_MAC(tgfVntCfmEtherTypeIdentification)
{
/********************************************************************
    Test 7.3 - Vnt Cfm EtherType Identification

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_XCAT2_E);
    CPSS_TBD_BOOKMARK/* UTF_XCAT2_E should be removed after PCL xCat2 update */

    /* Set configuration */
    prvTgfVntCfmEtherTypeIdentificationSet();

    /* Generate traffic */
    prvTgfVntCfmEtherTypeIdentificationTrafficGenerate();

    /* Restore configuration */
    prvTgfVntCfmEtherTypeIdentificationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVntCfmOpcodeIdentification
*/
UTF_TEST_CASE_MAC(tgfVntCfmOpcodeIdentification)
{

/********************************************************************
    Test 7.4 - Vnt Cfm Opcode Identification

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/
    PRV_TGF_NOT_SUPPORTED_DEV_CHECK_MAC(prvTgfDevNum, UTF_XCAT2_E);
    CPSS_TBD_BOOKMARK/* UTF_XCAT2_E should be removed after PCL xCat2 update */

    /* Set configuration */
    prvTgfVntCfmOpcodeIdentificationSet();

    /* Generate traffic */
    prvTgfVntCfmOpcodeIdentificationTrafficGenerate();

    /* Restore configuration */
    prvTgfVntCfmOpcodeIdentificationRestore();
}

/*----------------------------------------------------------------------------*/
/*
    tgfVntCfmOpcodeIdentification
*/
UTF_TEST_CASE_MAC(tgfVntOamPortUnidirectionalTransmitMode)
{

/********************************************************************
    Test 7.5 - Unidirectional transmit mode

    1. Set configuration
    2. Generate traffic
    3. Restore configuration
********************************************************************/

    /* Set configuration */
    prvTgfVntOamPortUnidirectionalTransmitModeSet();

    /* Generate traffic */
    prvTgfVntOamPortUnidirectionalTransmitModeTrafficGenerate();

    /* Restore configuration */
    prvTgfVntOamPortUnidirectionalTransmitModeRestore();
}


/*
 * Configuration of tgfVnt suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(tgfVnt)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVntOamPortPduTrapEnable)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVntLoopbackConfiguration)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVntCfmEtherTypeIdentification)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVntCfmOpcodeIdentification)
    UTF_SUIT_DECLARE_TEST_MAC(tgfVntOamPortUnidirectionalTransmitMode)
UTF_SUIT_END_TESTS_MAC(tgfVnt)

