/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvDuneUtfSuitsConfig.h
*
* DESCRIPTION:
*       Internal header which is included only by UTF code
*       and provides configuration for DUNE suits.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
/* includes */
#include "prvDuneUtfSuitsStart.h"

/* defines */
/* Provides more flexible switching between different configuration of suites */
#define PRV_DUNE_UTF_SUITES_CONFIG_FILENAME_CNS "prvDuneUtfSuitsConfig.h"

/* Uses macros to declare test-suite functions
 * and then implements utfAllDuneSuitsDeclare function
 * that calls test-suite functions.
 * Functions that are defined as arguments of UTF_MAIN_DECLARE_SUIT_MAC macros
 * will be declared and called.
 *
 * If you what add a Suit to the image please paste
 * UTF_MAIN_DECLARE_SUIT_MAC(name of Suit) here.
 */
UTF_MAIN_BEGIN_SUITS_MAC()
    /* Fap20m UTs */
    UTF_MAIN_DECLARE_SUIT_MAC(fap20m_api_auto_management)
    UTF_MAIN_DECLARE_SUIT_MAC(fap20m_api_cell)
    UTF_MAIN_DECLARE_SUIT_MAC(fap20m_api_configuration)
    UTF_MAIN_DECLARE_SUIT_MAC(fap20m_api_diagnostics)
    UTF_MAIN_DECLARE_SUIT_MAC(fap20m_api_egress_ports)
    UTF_MAIN_DECLARE_SUIT_MAC(fap20m_api_ingress_queue)
    UTF_MAIN_DECLARE_SUIT_MAC(fap20m_api_links)
    UTF_MAIN_DECLARE_SUIT_MAC(fap20m_api_routing)
    UTF_MAIN_DECLARE_SUIT_MAC(fap20m_api_scheduler)
    UTF_MAIN_DECLARE_SUIT_MAC(fap20m_api_spatial_multicast)
    UTF_MAIN_DECLARE_SUIT_MAC(fap20m_api_statistics)

    /* Fx950 UTs */
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_chopping)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_chunk_context_manager)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_diagnostics)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_hgl_cell_reassembly)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_hgl_interface)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_hgs_interface)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_labeling)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_mh_interoperability)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_pcs)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_queue_selection)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_scheduler)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_serdes)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_statistics)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_statistics_interface)
    UTF_MAIN_DECLARE_SUIT_MAC(fx950_api_tail_drop)
UTF_MAIN_END_SUITS_MAC()

/* includes */
#include "prvDuneUtfSuitsEnd.h"

