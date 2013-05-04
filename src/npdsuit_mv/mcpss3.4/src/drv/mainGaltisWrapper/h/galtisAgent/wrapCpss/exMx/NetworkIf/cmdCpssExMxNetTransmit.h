/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* cmdCpssExMxNetTransmit.h
*
* DESCRIPTION:
*       This file implements Receive/Transmit functionaly user exit functions
*       for Galtis.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
******************************************************************************/

#ifndef __cmdNetTransmitCpss__
#define __cmdNetTransmitCpss__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/exMx/exMxGen/networkIf/cpssExMxGenNetIfTypes.h>



#define MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN   64
/*number of buffer at the pool*/
#define   MAX_NUM_OF_BUFFERS_AT_POOL 124
#define   TX_BUFFER_SIZE 500

/* Size of block to aligned to. Must be power of 2, 2 is minimum */
#define ALIGN_ADDR_CHUNK 64
/* This macro used to get alighnment address from allocated */
#define ALIGN_ADR_FROM_MEM_BLOCK(adr,align)\
                   (( ((GT_U32)( ((GT_U8*)(adr)) + (ALIGN_ADDR_CHUNK) - 1)) &\
                   ((0xFFFFFFFF - (ALIGN_ADDR_CHUNK) + 1) )) + (align))


/*
 * Typedef: struct VLAN_DATA_STC
 *
 * Description: Unique fields necessary for "netSendPktByVid" function
 *
 *       devNum      - physical device number, used to find the vidx
 *                     according to the mac and vid
 *       mac         - Multicast Ehter address to send by.
 *       vid         - The Vid egress port list.
 *
 */
typedef struct
{
    GT_U8           devNum;
    GT_U16          vid;
    GT_U16          vidx;
} VLAN_DATA_STC;

/*
 * Typedef: struct PORT_DATA_STC
 *
 * Description: Unique fields necessary for "netSendPktByLport" function
 *
 *       isTrunk    - is the interface a trunk or a port
 *       devNum     - device number
 *       portTrunk  - can be a port number or a trunk id.
 *       vid        - The packet vlan id.
 *       addDsa     - when the packet is send from ExMx device (that support it)
 *                    a packet that should exist from a port of a MUX device,
 *                    need to be send with descriptor that notify the VB device
 *                    that connected to the MUX device to add DSA tag.
 *                    NOTE : even if the device that the CPU is sending the
 *                    packet from is not VB device and even not supporting VB
 *                    the TX descriptor is still fill with this info , so the VB
 *                    device that will get the TX descriptor will act
 *                    accordingly
 *       dsaInfo     - info about DSA tag
 *                    NOTE: relevant only when addDsa = GT_TRUE and useVidx = GT_FALSE
 *          dsaInfo.physicalMuxDev - HW device number of the MUX device that the
 *                    packet should be egress the system
 *                    (see also field dsaInfo.physicalMuxPort).
 *                    NOTE: relevant only when addDsa = GT_TRUE and useVidx = GT_FALSE
 *                          and packetEncap = CPSS_EXMX_NET_CONTROL_PCKT_E
 *          dsaInfo.physicalMuxPort - HW port number of the MUX device that the
 *                    packet should be egress the system
 *                    (see also field dsaInfo.physicalMuxDev).
 *                    NOTE: relevant only when addDsa = GT_TRUE and useVidx = GT_FALSE
 *                          and packetEncap = CPSS_EXMX_NET_CONTROL_PCKT_E
 *          dsaInfo.tcOnMuxDevice - traffic class to be used by the mux device
 *                    NOTE: relevant only when addDsa = GT_TRUE
 *                          and packetEncap = CPSS_EXMX_NET_CONTROL_PCKT_E

 *
 */
typedef struct
{
    GT_BOOL         isTrunk;
    GT_U8           devNum;
    GT_U8           portTrunk;
    GT_U16          vid;

    GT_BOOL         addDsa;
    struct{
        GT_U8       physicalMuxDev;
        GT_U8       physicalMuxPort;
        GT_U8       tcOnMuxDevice;
    }dsaInfo;
} PORT_DATA_STC;


/*
 * Typedef: enum TX_CMD_ENT
 *
 * Description: Enumeration of Transmit command Types
 *
 * Enumerations:
 *      TX_BY_VIDX  - Send packets using "netSendPktByVid" function and
 *                    defines to use spesific parameters of this function.
 *      TX_BY_PORT_TRUNK - Send packets using "netSendPktByLport" function and
 *                    defines to use spesific parameters of this function.
 */
typedef enum
{
    TX_BY_VIDX = 0,
    TX_BY_PORT_TRUNK
} TX_CMD_ENT;

/*
 * Typedef: enum TX_PARAMS_UNT
 *
 * Description: Enumeration of Transmit command Types
 *
 * Enumerations:
 *      byVlan    - defines parameters to send packets via "netSendPktByVid"
 *      byIf      - defines parameters to send packets via "netSendPktByIf"
 *      byLPort   - defines parameters to send packets via "netSendPktByLport"
 *
 */
typedef union
{
    VLAN_DATA_STC    byVlan;
    PORT_DATA_STC    byPort;
} TX_PARAMS_UNT;

/*
 * Typedef: struct
 *
 * Description: Packet Description structure
 *
 *      cmdType         - Defines type of transmition (VLAN, Port).
 *      valid           - This flag indicates that the entry is valid (or not)
 *      entryID         - Entry ID number for Delete/Get/ChangeAllign operations
 *      txSyncMode      - should the packet be send in a sync mode or not
 *      userPrioTag - User priority attached to the tag header.
 *      tagged          - does the packet already include tag.
 *      dropPrecedence - The packet's drop precedence.
 *      txQueue     - The tx Queue number to transmit the packet on.
 *      encap           - Defines the packet encapsulation.
 *      appendCrc       - (GT_TRUE) Add Crc to the transmitted packet,
 *                        (GT_FALSE) Leave the packet as is.
 *      macDaType       - Type of MAC-DA (valid only if packetEncap = ETHERNET).
 *      cmdParams       - Defines fields for transmition type (VLAN, If, LPort)
 *      invokeTxBufferQueueEvent - indication to configure the PP to invoke Tx buffer
 *                    Queue event after copying packet's buffers to PP memory.
 *                    This will allow the user to send more then one packets between
 *                    invokations of TX_BUFFER_QUEUE. The buffers are returned to
 *                    application afetr the invocation of the interrupt.
 *                    NOTE1: This parameter ignored in cpssExMxNetIfSyncTxPacketSend.
 *      pcktsNum        - Number of packets to send.
 *      gap             - The time is calculated in multiples of 64 clock cycles
 *                        Valid values are 0 (no delay between packets to
 *                        CPU), through 0x3FFE (1,048,544 Clk cycles).
 *                        0x3FFF - has same effect as 0.
 *      waitTime        - The wait time before moving to the next entry.
 *      pcktData        - Array of pointers to packet binary data.
 *      pcktDataLen     - Array of lengths of pcktData pointers.
 *      numOfBuffers    - number of buffers used by pcktData.
 *      numSentPackets  - Number of sent packets.
 *
 */
typedef struct
{
    TX_CMD_ENT                      cmdType;
    GT_BOOL                         valid;
    GT_U32                          entryId;
    GT_BOOL                         txSyncMode;
    GT_U8                           userPrioTag;
    GT_BOOL                         tagged;
    GT_U8                           dropPrecedence;
    GT_U8                           txQueue;
    CPSS_EXMX_NET_PCKT_ENCAP_ENT    encap;
    GT_BOOL                         appendCrc;
    CPSS_EXMX_NET_MAC_TYPE_ENT      macDaType;
    TX_PARAMS_UNT                   cmdParams;
    GT_BOOL                         invokeTxBufferQueueEvent;
    GT_U32                          pcktsNum;
    GT_U32                          gap;
    GT_U32                          waitTime;
    GT_U8                           *pcktData[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN];
    GT_U32                          pcktDataLen[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN];
    GT_U32                          numOfBuffers;
    GT_U32                          numSentPackets;

} PKT_DESC_STC;


/*
 * Typedef: struct MODE_SETINGS_STC
 *
 * Description: Packet Description structure
 *
 *      cyclesNum - The maximum number of loop cycles (-1 = forever)
 *      gap       - The time to wit between two cycles.
 *
 */

typedef struct
{
    GT_U32  cyclesNum;
    GT_U32  gap;
} MODE_SETINGS_STC;


/*
 * Typedef: enum GT_MAC_TYPE
 *
 * Description: Defines the different Mac-Da types of a transmitted packet.
 *
 * Fields:
 *      UNICAST_MAC         - MAC_DA[0] = 1'b0
 *      MULTICAST_MAC       - MAC_DA[0] = 1'b1
 *      BROADCAST_MAC       - MAC_DA = 0xFFFFFFFF
 *
 */
typedef enum
{
    UNICAST_MAC,
    MULTICAST_MAC,
    BROADCAST_MAC
}GT_MAC_TYPE;

/*******************************************************************************
* cmdCpssTxSetPacketDesc
*
* DESCRIPTION:
*       Creates new transmit parameters description entry in the table.
*
* INPUTS:
*       entryID      - Entry ID of packet descriptor to return.
*       packetDesc   - Pointer to Packet descriptor new entry in the table
*       extended     - flag indicates if this is the extended table
*
* OUTPUTS:
*       newEntryIndex   - The index of the new entry in the table.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPcktDescs
*
* Toolkit:
*       Using - <netSendPktByLport, netSendPktByIf, netSendPktByVid,
*                netFreeTxGtBuf, gtBufCreate>
*
*******************************************************************************/
GT_STATUS cmdCpssTxSetPacketDesc
(
    IN  GT_U32          entryId,
    IN  PKT_DESC_STC    *packetDesc,
    IN  GT_BOOL         extended,
    OUT GT_U32          *newEntryIndex
);

/*******************************************************************************
* cmdCpssTxGetPacketDesc
*
* DESCRIPTION:
*       Retruns specific packet descriptor entry from the table.
*
* INPUTS:
*     entryIndex   - Entry index in packet descriptor table.
*
* OUTPUTS:
*   entryIndex     - The returned entry index.
*     packetDesc   - Packet descriptor from the table.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*       GT_EMPTY - on invalid index
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPcktDescs
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS cmdCpssTxGetPacketDesc
(
    INOUT GT_U32      *entryIndex,
    OUT PKT_DESC_STC *packetDesc
);

/*******************************************************************************
* cmdCpssTxBeginGetPacketDesc
*
* DESCRIPTION:
*       Indicates for cmdTxGetPacketDesc function that user go to get many
*       records in one session. This API make exact copy of table so that
*       user will get table without changes that can be made during get
*       operation. This mechanism is best suitable for Refresh table
*       operation.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS cmdCpssTxBeginGetPacketDesc
(
    void
);

/*******************************************************************************
* cmdCpssTxEndGetPacketDesc
*
* DESCRIPTION:
*       Finish operation began by cmdTxBeginGetPacketDesc API function
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS cmdCpssTxEndGetPacketDesc
(
    void
);


/*******************************************************************************
* cmdCpssTxDelPacketDesc
*
* DESCRIPTION:
*       Delete packet descriptor entry from the table.
*
* INPUTS:
*       entryID     - Entry ID of packet descriptor to delete.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       After delete operation all record will be shifted
*       so the table will be not fragmented.
*
* GalTis:
*       Table - cmdTxPcktDescs
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS cmdCpssTxDelPacketDesc
(
    IN  GT_U32      entryID
);

/*******************************************************************************
* cmdCpssTxClearPacketDesc
*
* DESCRIPTION:
*       Clear all packet descriptors entries from the table.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPcktDescs
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS cmdCpssTxClearPacketDesc
(
    void
);


/*******************************************************************************
* cmdCpssTxStart
*
* DESCRIPTION:
*       Starts transmition of packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command   - cmdCpssTxStart
*
* Toolkit:
*       Interface - <prestera/tapi/networkif/commands.api>
*
*******************************************************************************/
GT_STATUS cmdCpssTxStart
(
    void
);

/*******************************************************************************
* cmdCpssTxStop
*
* DESCRIPTION:
*       Stop transmition of packets.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command   - cmdCpssTxStop
*
* Toolkit:
*       Interface - <prestera/tapi/networkif/commands.api>
*       Using     - <netSendPktByLport, netSendPktByIf, netSendPktByVid,
*                    netFreeTxGtBuf, gtBufCreate>
*
*******************************************************************************/
GT_STATUS cmdCpssTxStop
(
    void
);

/*******************************************************************************
* cmdCpssTxSetMode
*
* DESCRIPTION:
*       This command will set the the transmit parameters.
*
* INPUTS:
*       cyclesNum - The maximum number of loop cycles (-1 = forever)
*       gap       - The time to wit between two cycles
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command     - cmdTxSetMode
*
* Toolkit:
*       Interface   - <prestera/tapi/networkif/commands.api>
*
*******************************************************************************/
GT_STATUS cmdCpssTxSetMode
(
    IN GT_U32   cyclesNum,
    IN GT_U32   gap
);

/*******************************************************************************
* cmdCpssTxGetMode
*
* DESCRIPTION:
*       This command will get the the transmit parameters.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       cyclesNum - The maximum number of loop cycles (-1 = forever)
*       gap       - The time to wit between two cycles.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command - cmdTxGetMode
*
* Toolkit:
*       Interface - <prestera/tapi/networkif/commands.api>
*
*******************************************************************************/
GT_STATUS cmdCpssTxGetMode
(
    OUT GT_U32 *cyclesNum,
    OUT GT_U32 *gap
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__cmdNetTransmitCpss__*/


