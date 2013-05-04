
/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapAppDemoFx950_api_interrupts.h
*
* DESCRIPTION:
*       Galtis wrapper definitions for FX950 and FAP20M interrupts.
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#ifndef WRAP_APPDEMO_FX950_API_INTERRUPTS_H_INCLUDED
/* { */
#define WRAP_APPDEMO_FX950_API_INTERRUPTS_H_INCLUDED

#ifdef  __cplusplus
extern "C" {
#endif

#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
/*#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>*/

typedef GT_STATUS FX950_EVENT_COUNTER_INCREMENT_FUNC
(
    IN  unsigned int    device_id,
    IN  unsigned long   interruptIndex
);

extern GT_STATUS wrFx950EventCountersCbGet
(
	FX950_EVENT_COUNTER_INCREMENT_FUNC **fx950EventCounterBlockGetCB
);

typedef enum
{
    /* Global event (includes Fap20m global, Fx950 global and PEX events) */
    WR_FAP20M_EVENT_TYPE_GLOBAL,
    /* FAP20M FE port event */
    WR_FAP20M_EVENT_TYPE_FE_PORT,
    /* FX950 HGL port event */
    WR_FAP20M_EVENT_TYPE_HGL_PORT,
    /* FX950 HGL port lane event */
    WR_FAP20M_EVENT_TYPE_HGL_PORT_LANE

}WR_FAP20M_EVENT_TYPE_ENT;


typedef enum
{
    /* fap20m events */

    /***** Global *****/

    /*
    * event on received source routed data cell
    */
    WR_FAP20M_RECV_SR_DATA_CELL_EVENT, 
 
    /*
     * Parity error detected at QDR sram descriptors memory
     * (QDP interrupts). 
     */
    WR_FAP20M_SFT_ERR_QDR_EVENT, 

    
    /***** per FE port *****/
    
    /*
     * Active link mask was changed.
     *  
     */
    WR_FAP20M_ACTIVE_LINK_CHANGED_EVENT,
    
    
    /* FX950 events  */
    
    /***** Global *****/
    
    /*
     * TWSI timed out in Slave or Master mode. 
     */
    WR_FX950_GLOBAL_INT_MISC_TWSI_TIME_OUT, 
    
    /*
     * TWSI received an invalid status in Slave or Master mode. 
     */
    WR_FX950_GLOBAL_INT_MISC_TWSI_STATUS,
    
    /*
     * The Genxs got from the PEX a transaction with address which is mapped to 
     * BAR0 (PEX space), but not to the actually PEX registers. 
     */
    WR_FX950_GLOBAL_INT_MISC_PEX_ADDR_UNMAPPED, 
    
    /*
     * All of a packets buffers were aged out. 
     */
    WR_FX950_GLOBAL_INT_BM_AGED_PACKET, 
    
    /*
     * Triggered aging finished scanning all buffers.
     */
    WR_FX950_GLOBAL_INT_BM_TRIGGERED_AGING_DONE,
    
    /****** PEX *****/
    
    /*
     * New Tx Request while in Dl-Down.
     */
    WR_FX950_PEX_INT_DI_DWN_TX_ACC_ERR,

    /*
     * Attempt to generate a PCI transaction while master is disabled.
     */
    WR_FX950_PEX_INT_M_DIS,
    
    /*
     * Erroneous write attempt to internal register.
     * Set when an erroneous write request to PCI Express internal register is 
     * received, either from the PCI Express (EP set) or from the internal bus 
     * (bit[64] set).
     */
    WR_FX950_PEX_INT_ERR_WR_TO_REG,
    
    /*
     * Hit Default Window Error.
     */
    WR_FX950_PEX_INT_HIT_DFLT_WIN_ERR,
    
    /*
     * Correctable Error Detected Indicates status of correctable errors 
     * detected by 98FX950.
     */
    WR_FX950_PEX_INT_CORE_ERR_DET,
    
    /*
     * Non-Fatal Error Detected Indicates status of Non-Fatal errors 
     * detected by 98FX950.
     */
    WR_FX950_PEX_INT_NF_ERR_DET,
    
    /*
     * Fatal Error Detected Indicates status of Fatal errors 
     * detected by 98FX950.
     */
    WR_FX950_PEX_INT_F_ERR_DET,
    
    /*
     * Dstate Change Indication Any change in the Dstate asserts this interrupt.
     * NOTE: This interrupt is relevant only for Endpoint.
     */
    WR_FX950_PEX_INT_DSTATE_CHANGE,
    
    /*
     * PCI Express BIST activated BIST is not supported
     */
    WR_FX950_PEX_INT_BIST,
    
    /*
     * Received CRS completion status.
     * A downstream PCI Express device can respond to a configuration request 
     * with CRS (Configuration Request Retry Status) if it is not ready yet 
     * to serve the request. RcvCRS interrupt is set when such a completion status 
     * is received.
     */
     WR_FX950_PEX_INT_RCV_CRS,
    
     /*
      * Received Hot Reset Indication
      * This interrupt is set when a hot-reset indication is received from the opposite 
      * device on the PCI Express port.
      * NOTE: This interrupt is not initialized by reset.
      */
     WR_FX950_PEX_INT_PEX_SLV_HOT_RESET,
    
      /*
       * Slave Disable Link Indication.
       * This interrupt is set when the opposite device on the PCI Express port is 
       * acting as a disable link master, and link was disabled.
       * NOTE: This interrupt is not initialized by reset.
       */
      WR_FX950_PEX_INT_PEX_SLV_DIS_LINK,
    
      /*
       * Slave Loopback Indication.
       * This interrupt is set when the opposite device on the PCI Express port 
       * is acting as a loopback master, and loopback mode was entered.
       * NOTE: This interrupt is not initialized by reset.
       */
      WR_FX950_PEX_INT_PEX_SLV_LB,
    
      /*
       * Link Failure indication.
       * PCI Express link dropped from active state (L0, L0s or L1) to Detect 
       * state due to link errors.
       * NOTE: When dropping to Detect via Hot Reset, Disable Link or Loopback states, 
       * the interrupt is not asserted.
       * NOTE: This interrupt is not initialized by reset.
       */
      WR_FX950_PEX_INT_PEX_LINK_FAIL,
    
      /****** Port events ******/
    
      /*
       * There was a wraparound in one of the MIB counters.
       */
      WR_FX950_PORT_INT_CONVERTOR_MIB_COUNT_WA,
    
      /*
       * MIB counter capture action is done.
       */
      WR_FX950_PORT_INT_CONVERTOR_MIB_COPY_DONE,
    
    
      /* HyperGLink interrupts */
    
      /*
       * Indicates that a PING message has been received.
       */
      WR_FX950_PORT_INT_HGL_PING_RECEIVED,
    
      /*
       * Indicates that a PING message has been sent.
       */
      WR_FX950_PORT_INT_HGL_PING_SENT,
    
      /*
       * One of the following counters has reached its limit
       */
      WR_FX950_PORT_INT_HGL_RX_ERR_CNT_RL,
    
      /*
       * A cell with an error when reformatting cell data from PCS to MAC was received. 
       * This cell is dropped. The number of cells dropped due to an error when 
       * reformatting cell data from PCS to MAC is counted by the Mac Dropped Received 
       * Counters.
       */
      WR_FX950_PORT_INT_HGL_BAD_PCS_2_MAC_REFORM_RX,
    
      /*
       * A cell with an error in the header was received. This cell is dropped. 
       * The number of cells dropped due to Error in the header is counted by the 
       * Mac Dropped Received Counters.
       */
      WR_FX950_PORT_INT_HGL_BAD_HEAD_CELL_RX,
    
      /*
       * A cell with bad length was received. This cell is dropped. 
       * The number of cells dropped due to BadLength is counted by the 
       * Mac Dropped Received Cells Counters
       */
      WR_FX950_PORT_INT_HGL_BAD_LENGTH_CELL_RX,
    
      /*
       * A Flow Control Cell was received.
       */
      WR_FX950_PORT_INT_HGL_FC_CELL_RX,
    
      /*
       * A Flow Control Cell was sent.
       */
      WR_FX950_PORT_INT_HGL_FC_CELL_TX,
    
      /*
       * Set when Flow Control from PCS# to XBAR Port for Traffic Class 0 is at 
       * XOFF for an interval longer than specified in <FcDeadLockLimit> 
    
       */
      WR_FX950_PORT_INT_HGL_FC_XOFF_DEADLOCK,
    
      /*
       * Set when general access tries to access an invalid address
       */
      WR_FX950_PORT_INT_HGL_ADDR_OUT_OF_RANGE,
    
    
      /* HGS interrupts */
    
      /*
       * Asserted when a change in the link status is detected.
       */
      WR_FX950_PORT_INT_HGS_LINK_STATUS_CHANGE,
    
      /*
       * Asserted when the status of <PortRxPause> has changed in the 
       * Port<n> Status Register0 .
       */
      WR_FX950_PORT_INT_HGS_FC_STAT_CHANGE,
    
      /*
       * Asserted when the XGMII MAC has received unexpected sequence.
       */
      WR_FX950_PORT_INT_HGS_UNKNOWN_SEQUENCE,
    
      /*
       * Asserted when the status of <Remote Fault> or <LocalFault> has changed.
       */
      WR_FX950_PORT_INT_HGS_FAULT_CHANGE,
    
      /*
       * This bit is set when the CPU attempted to read from or write to an 
       * illegal address in one of the port registers.
       */
      WR_FX950_PORT_INT_HGS_ADDR_OUT_OF_RANGE,
    
      /*
       * Packet dropped because lack of buffers for this port
       */
      WR_FX950_PORT_INT_HGS_NO_BUFF_PKT_DROP,
    
    
      /* XPCS interrupts */
    
      /*
       * Asserted when a change in the link status is detected
       */
      WR_FX950_PORT_INT_XPCS_LINK_STATUS_CHANGE,
    
      /*
       * Asserted when Deskew is not acquired clock cycles after the identification 
       * of the first ||A||.
       */
      WR_FX950_PORT_INT_XPCS_DESCEW_TIME_OUT,
    
      /*
       * Asserted when a column of ||A|| is detected.
       */
      WR_FX950_PORT_INT_XPCS_DETECTED_COLUMN_A,
    
      /*
       * Deskew Error detected. Asserted after lane alignment has been acquired 
       * and not all ||A|| are in the same column
       */
      WR_FX950_PORT_INT_XPCS_DESCEW_ERR,
    
      /*
       * PPM FIFO underrun interrupt.
       */
      WR_FX950_PORT_INT_XPCS_PPM_FIFO_UNDERRUN,
    
      /*
       * PPM FIFO overrun interrupt.
       */
      WR_FX950_PORT_INT_XPCS_PPM_FIFO_OVERRUN,
    
    
      /***** Per port lane *****/
    
      /*
       * Asserted when the PRBS checker is locked and an error has been detected.
       */
      WR_FX950_PORT_LANE_INT_PRBS_ERR,
    
      /*
       * Asserted when a disparity error has occurred 
       */
      WR_FX950_PORT_LANE_INT_DISPARITY_ERR,
    
      /*
       * Asserted when a symbol error has occurred
       */
      WR_FX950_PORT_LANE_INT_SYMBOL_ERR,
    
      /*
       * Asserted when CRPAT checker or CJPAT checker has detected an error
       */
      WR_FX950_PORT_LANE_INT_CJR_PAT_ERR,
    
      /*
       * Asserted when the Signal Detect status of the lane has changed.
       */
      WR_FX950_PORT_LANE_INT_SIGNAL_DETECT_CHANGE,
    
      /*
       * Asserted when the sync status of the lane has changed
       */
      WR_FX950_PORT_LANE_INT_SYNC_STATUS_CHANGE,
    
      /*
       * Asserted when an ||A|| symbol is detected.
       */
      WR_FX950_PORT_LANE_INT_DETECTED_A,

      /* Total number of interrupts */

      WR_FX950_EVENTS_NUM
  
}WR_FAP20M_EVENTS_ENT;


#ifdef  __cplusplus
}
#endif

#endif
