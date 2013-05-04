/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdFx950.c
*
* DESCRIPTION:
*       Init for Galtis Agent
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/


/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>


GT_STATUS cmdLibInitFap20m_api_auto_management(GT_VOID);
GT_STATUS cmdLibInitFap20m_api_cell(GT_VOID);
GT_STATUS cmdLibInitFap20m_api_configuration(GT_VOID);
GT_STATUS cmdLibInitFap20m_api_diagnostics(GT_VOID);
GT_STATUS cmdLibInitFap20m_api_egress_ports(GT_VOID);
GT_STATUS cmdLibInitFap20m_api_ingress_queue(GT_VOID);
GT_STATUS cmdLibInitFap20m_api_links(GT_VOID);
GT_STATUS cmdLibInitFap20m_api_routing(GT_VOID);
GT_STATUS cmdLibInitFap20m_api_scheduler(GT_VOID);
GT_STATUS cmdLibInitFap20m_api_spatial_multicast(GT_VOID);
GT_STATUS cmdLibInitFap20m_api_statistics(GT_VOID);
GT_STATUS cmdLibInitFap20m_interrupt_services(GT_VOID);
GT_STATUS cmdLibInitFap20m_api_init(GT_VOID);
GT_STATUS cmdLibInitFx950_api_chopping(GT_VOID);
GT_STATUS cmdLibInitFx950_api_chunk_context_manager(GT_VOID);
GT_STATUS cmdLibInitFx950_api_diagnostics(GT_VOID);
GT_STATUS cmdLibInitFx950_api_hgl_cell_reassembly(GT_VOID);
GT_STATUS cmdLibInitFx950_api_hgl_interface(GT_VOID);
GT_STATUS cmdLibInitFx950_api_hgs_interface(GT_VOID);
GT_STATUS cmdLibInitFx950_api_labeling(GT_VOID);
GT_STATUS cmdLibInitFx950_api_mh_interoperability(GT_VOID);
GT_STATUS cmdLibInitFx950_api_pcs(GT_VOID);
GT_STATUS cmdLibInitFx950_api_queue_selection(GT_VOID);
GT_STATUS cmdLibInitFx950_api_scheduler(GT_VOID);
GT_STATUS cmdLibInitFx950_api_serdes(GT_VOID);
GT_STATUS cmdLibInitFx950_api_statistics(GT_VOID);
GT_STATUS cmdLibInitFx950_api_statistics_interface(GT_VOID);
GT_STATUS cmdLibInitFx950_api_tail_drop(GT_VOID);
GT_STATUS cmdLibInitFx950_api_flow_control(GT_VOID);
GT_STATUS cmdLibInitFx950_api_mh_marking(GT_VOID);


/* global Fx950 database initialization routine */
GT_STATUS cmdFx950LibInit( void )
{
    /* Fap20m*/
    if (cmdLibInitFap20m_api_auto_management() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_api_cell() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_api_configuration() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_api_diagnostics() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_api_egress_ports() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_api_ingress_queue() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_api_links() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_api_routing() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_api_scheduler() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_api_spatial_multicast() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_api_statistics() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_interrupt_services() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFap20m_api_init() != GT_OK)
        return GT_FAIL;

    /* Fx950 */
    if (cmdLibInitFx950_api_chopping() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_chunk_context_manager() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_diagnostics() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_hgl_cell_reassembly() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_hgl_interface() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_hgs_interface() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_labeling() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_mh_interoperability() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_pcs() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_queue_selection() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_scheduler() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_serdes() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_statistics() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_statistics_interface() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_tail_drop() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_flow_control() != GT_OK)
        return GT_FAIL;
    if (cmdLibInitFx950_api_mh_marking() != GT_OK)
        return GT_FAIL;

    return GT_OK;
}


