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
* cmdCpssDxChNetTransmit.h
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

#ifndef __cmdCpssDxChNetTransmith__
#define __cmdCpssDxChNetTransmith__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>



#define   MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN   64
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
 * Typedef: enum TX_CMD_ENT
 *
 * Description: Enumeration of Transmit command Types
 *
 * Enumerations:
 *      TX_BY_VIDX  - Send packets using "netSendPktByVid" function and
 *                    defines to use spesific parameters of this function.
 *      TX_BY_PORT  - Send packets using "netSendPktByLport" function and
 *                    defines to use spesific parameters of this function.
 */
typedef enum
{
    DXCH_TX_BY_VIDX = 0,
    DXCH_TX_BY_PORT
} DXCH_TX_CMD_ENT;


/*
 * typedef: enum DXCH_NET_DSA_TYPE_ENT
 *
 * Description: Enumeration of DSA tag types
 *
 * Enumerations:
 *    DXCH_NET_DSA_TYPE_REGULAR_E  - regular DSA tag(single word - 4 bytes)
 *    DXCH_NET_DSA_TYPE_EXTENDED_E - extended DSA tag(two words - 8 bytes)
 *
 */
typedef enum
{
    DXCH_NET_DSA_TYPE_REGULAR_E,
    DXCH_NET_DSA_TYPE_EXTENDED_E
}DXCH_NET_DSA_TYPE_ENT;


/*
 * typedef: structure DXCH_NET_DSA_COMMON_STC
 *
 * Description: structure of common parameters in all DSA tag formats
 *
 *  dsaTagType - DSA type regular/extended
 *
 *  vpt - the value vlan priority tag (0..7)
 *  cfiBit - CFI bit of the vlan tag  (0..1)
 *  vid - the value of vlan id (0..4095)
 *
 */
typedef struct{
    DXCH_NET_DSA_TYPE_ENT           dsaTagType;

    GT_U8                           vpt;
    GT_U8                           cfiBit;
    GT_U16                          vid;
}DXCH_NET_DSA_COMMON_STC;

/*
 * typedef: enum DXCH_NET_DSA_CMD_ENT
 *
 * Description: Enumeration of DSA tag commands
 *
 * Enumerations:
 *    DXCH_NET_DSA_CMD_TO_CPU_E  - DSA command is "To CPU"
 *    DXCH_NET_DSA_CMD_FROM_CPU_E  - DSA command is "FROM CPU"
 *    DXCH_NET_DSA_CMD_TO_ANALYZER_E - DSA command is "TO Analyzer"
 *    DXCH_NET_DSA_CMD_FORWARD_E  - DSA command is "FORWARD"
 *
 */
typedef enum
{
    DXCH_NET_DSA_CMD_TO_CPU_E = 0 ,
    DXCH_NET_DSA_CMD_FROM_CPU_E    ,
    DXCH_NET_DSA_CMD_TO_ANALYZER_E ,
    DXCH_NET_DSA_CMD_FORWARD_E
} DXCH_NET_DSA_CMD_ENT;


/*
 * typedef: structure DXCH_NET_DSA_FROM_CPU_STC
 *
 * Description: structure of "from CPU" DSA tag parameters
 *
 * fields:
 *      dstInterface  - Packet's destination interface:
 *         type == CPSS_INTERFACE_TRUNK_E --> not supported !
 *         when type == CPSS_INTERFACE_PORT_E
 *              devPort.tgtDev - the target device that packet will be send to.
 *                               NOTE : HW device number !!
 *              devPort.tgtPort- the target port on device that packet will be
 *                             send to.
 *         when type == CPSS_INTERFACE_VIDX_E
 *              vidx     - the vidx group that packet will be sent to.
 *         when type == CPSS_INTERFACE_VID_E
 *              vlanId   - must be equal to commonParams.vid !!!
 *
 *      tc - the traffic class for the transmitted packet
 *
 *      dp  - the drop precedence for the transmitted packet
 *
 *      egrFilterEn - Egress filtering enable.
 *                  GT_FALSE - Packets from the CPU are not egress filtered.
 *                             Unicast packets are forwarded regardless of the
 *                             Egress port Span State or VLAN Membership.
 *                             Multi-destination packets are forwarded to the
 *                             Multicast group members specified in this tag,
 *                             regardless of the target port's Span state.
 *                  GT_TRUE -  Packets from CPU are Egress filtered.
 *
 *      cascadeControl - This field indicates which TC is assigned to the packet
 *                       when it is forwarded over a cascading/stacking port.
 *                  GT_FALSE - If the packet is queued on a port that is enabled
 *                             for Data QoS mapping (typically a cascade port),
 *                             the packet is queued according to the data
 *                             traffic {TC, DP} mapping table, which maps the
 *                             DSA tag TC and DP to a cascade port TC and DP.
 *                             On a port that is disabled for Data QoS mapping (
 *                             a non-cascade ports), the packet is queued
 *                             according to the DSA tag TC and DP.
 *                   GT_TRUE - If the packet is queued on a port that is enabled
 *                             for Control QoS mapping (typically a cascade
 *                             port), the packet is queued according to the
 *                             configured Control TC and DP. On a port that is
 *                             enabled for Control QoS mapping (non-cascade
 *                             ports), the packet is queued according to the DSA
 *                             tag TC and DP.
 *
 *      egrFilterRegistered -  Indicates that the packet is Egress filtered as a
 *                             registered packet.
 *                  GT_FALSE - Packet is egress filtered as an Unregistered
 *                             packet and is forwarded to an Egress port
 *                             according to its type (Unicast or Multicast) and
 *                             the configuration of the Egress port Port<n>
 *                             UnkFilterEn if packet is Unicast, and according
 *                             to the configuration of Port<n> UnregFilterEn if
 *                             the packet is Multicast.
 *                  GT_TRUE  - Packet is egress filtered as a registered packet
 *                             according to the members of the Multicast group.
 *                             NOTE:
 *                             When this field is 0, the type of the packet
 *                             Multicast or Unicast is set according to the
 *                             packet's MAC DA[40].
 *
 *       srcId              - Packet's Source ID
 *
 *       srcDev             - Packet's Source Device Number.
 *                            NOTE : HW device number !!
 *
 *       extDestInfo - extra destination information:
 *          multiDest - info about multi destination destination , used when:
 *                      type == CPSS_INTERFACE_VIDX_E or
 *                      type == CPSS_INTERFACE_VID_E
 *             excludeInterface - to exclude a "source" interface from the
 *                      destination flooding.
 *                      GT_FALSE - no "source" interface to exclude
 *                      GT_TRUE  - use "source" interface to exclude , see
 *                                 parameter excludedInterface.
 *             excludedInterface - the "source" interface to exclude from the
 *                          Multicast group.
 *                          NOTE: relevant only
 *                                when excludeInterface == GT_TRUE
 *                          when type == CPSS_INTERFACE_PORT_E
 *                              the {devPort.deviceNum,devPort.portNum} will be
 *                              excluded from the vid/vidx
 *                          when type == CPSS_INTERFACE_TRUNK_E
 *                              the trunkId will be excluded from the vid/vidx
 *                          type == CPSS_INTERFACE_VIDX_E not supported
 *                          type == CPSS_INTERFACE_VID_E  not supported
 *
 *             mirrorToAllCPUs - Mirror the packet to all CPUs
 *                          Enable a CPU to send a Multicast/Broadcast packet to
 *                          all CPUs in the system.
 *                          NOTE: relevant only :
 *                               when (excludeInterface == GT_FALSE) or
 *                               when ((excludeInterface == GT_TRUE) and
 *                                (excludedInterface.type == CPSS_INTERFACE_TRUNK_E))
 *                          GT_FALSE - The Multicast packet sent by the CPU is
 *                                     not mirrored to the CPU attached to the
 *                                     receiving device.
 *                          GT_TRUE - The Multicast packet sent by the CPU is
 *                                    mirrored to the CPU attached to the
 *                                    receiving device with a TO_CPU DSA Tag and
 *                                    MIRROR_TO_ALL_CPUS CPU Code.
 *       devPort - {device,port} info (single destination), used when:
 *                          type == CPSS_INTERFACE_PORT_E
 *             dstIsTagged - GT_FALSE - Packet is sent via network port untagged
 *                           GT_TRUE  - Packet is sent via network port tagged
 *             mailBoxToNeighborCPU - Mail box to Neighbor CPU, for CPU to CPU
 *                              mail box communication.
 *                           NOTE: As a Mail message is sent to a CPU with
 *                              unknown Device Number, the TrgDev
 *                              (dstInterface.devPort.tgtDev) must be set to
 *                              the local device number and TrgPort
 *                              (dstInterface.devPort.tgtPort) must be set to
 *                              the Cascading port number in the local device
 *                              through which this packet is to be transmitted.
 *
 */
typedef struct{
    CPSS_INTERFACE_INFO_STC         dstInterface; /* vid/vidx/port */
    GT_U8                           tc;
    CPSS_DP_LEVEL_ENT               dp;
    GT_BOOL                         egrFilterEn;
    GT_BOOL                         cascadeControl;
    GT_BOOL                         egrFilterRegistered;

    GT_U32                          srcId;
    GT_U8                           srcDev;

    union{
        struct{
            GT_BOOL                         excludeInterface;
            CPSS_INTERFACE_INFO_STC         excludedInterface; /* port/trunk */
            GT_BOOL                         mirrorToAllCPUs;
        }multiDest;

        struct{
            GT_BOOL                     dstIsTagged;
            GT_BOOL                     mailBoxToNeighborCPU;
        }devPort;
    }extDestInfo;

}DXCH_NET_DSA_FROM_CPU_STC;

/*
 * typedef: structure DXCH_NET_DSA_FORWARD_STC
 *
 * Description: structure of "forward" DSA tag parameters
 *
 * fields:
 *
 *  srcIsTagged - source packet received tagged
 *
 *  srcDev - Packet's Source Device Number.
 *           NOTE : HW device number !!
 *
 *  srcIsTrunk - the packet was received from trunk
 *     source.trunkId - - Packet's Source trunk id
 *                  NOTE : relevant when srcIsTrunk = GT_TRUE
 *     source.portNum - - Packet's Source port num
 *                  NOTE : relevant when srcIsTrunk = GT_FALSE
 *
 *  srcId              - Packet's Source ID
 *
 *  srcDev             - Packet's Source Device Number.
 *                       NOTE : HW device number !!
 *
 *  egrFilterRegistered -  Indicates that the packet is Egress filtered as a
 *                         registered packet.
 *              GT_FALSE - Packet is egress filtered as an Unregistered
 *                         packet and is forwarded to an Egress port
 *                         according to its type (Unicast or Multicast) and
 *                         the configuration of the Egress port Port<n>
 *                         UnkFilterEn if packet is Unicast, and according
 *                         to the configuration of Port<n> UnregFilterEn if
 *                         the packet is Multicast.
 *              GT_TRUE  - Packet is egress filtered as a registered packet
 *                         according to the members of the Multicast group.
 *                         NOTE:
 *                         When this field is 0, the type of the packet
 *                         Multicast or Unicast is set according to the
 *                         packet's MAC DA[40].
 *
 *  wasRouted - Indicates whether the packet is routed.
 *              GT_FALSE - Packet has not be Layer 3 routed.
 *              GT_TRUE  - Packet has been Layer 3 routed.
 *
 * qosProfileIndex - Packet's QoS Profile.
 *
 * dstInterface  - Packet's destination interface:
 *         type == CPSS_INTERFACE_TRUNK_E --> not supported !
 *         when type == CPSS_INTERFACE_PORT_E
 *              devPort.tgtDev - the target device that packet will be send to.
 *                               NOTE : HW device number !!
 *              devPort.tgtPort- the target port on device that packet will be
 *                             send to.
 *         when type == CPSS_INTERFACE_VIDX_E
 *              vidx     - the vidx group that packet will be sent to.
 *         when type == CPSS_INTERFACE_VID_E
 *              vlanId   - must be equal to commonParams.vid !!!
 *
 */
typedef struct{
    GT_BOOL                         srcIsTagged;
    GT_U8                           srcDev;
    GT_BOOL                         srcIsTrunk;
    union
    {
        GT_TRUNK_ID                 trunkId;
        GT_U8                       portNum;
    }source;

    GT_U32                          srcId;

    GT_BOOL                         egrFilterRegistered;
    GT_BOOL                         wasRouted;
    GT_U32                          qosProfileIndex;

    CPSS_INTERFACE_INFO_STC         dstInterface; /* vid/vidx/port */

}DXCH_NET_DSA_FORWARD_STC;

/*
 * typedef: structure DXCH_NET_DSA_TO_ANALYZER_STC
 *
 * Description: structure of "to analyzer" DSA tag parameters
 *
 * fields:
 *
 *      rxSniffer - is this packet destined to the Rx or to the Tx Analyzer port
 *          GT_FALSE - Packet was Tx sniffed and is forwarded to Target Tx
 *                     sniffer (Analyzer).
 *          GT_TRUE  - Packet was Rx sniffed and is forwarded to Target Rx
 *                     sniffer (Analyzer).
 *
 *      isTagged -  When rxSniffer == GT_TRUE:
 *                      This field is srcTagged.
 *                      srcTagged -
 *                          GT_FALSE - Packet was received from a regular
 *                                     network port untagged and is forwarded
 *                                     to the Ingress analyzer untagged.
 *                          GT_TRUE - Packet was received from a regular network
 *                                    port tagged and is forwarded to the Ingress
 *                                    analyzer tagged, with the same VID and UP
 *                                     with which it was received.
 *                      When rxSniffer == GT_FALSE:
 *                      This field is trgTagged.
 *                      trgTagged -
 *                          GT_FALSE - Packet was sent via a regular network
 *                                     port untagged and is forwarded to the Egress
 *                                     analyzer untagged.
 *                          GT_TRUE - Packet was sent via a regular network port
 *                                    tagged and is forwarded to the Egress
 *                                    analyzer tagged.
 *
 *      devPort - src/dst {dev,port} info :
 *              devPort.devNum -
 *                      SrcDev:
 *                      When rx_sniff = GT_TRUE or when tag is not extended:
 *                      SrcDev indicates the packet's original Ingress port.
 *                      When rx_sniff = GT_FALSE and the tag is extended:
 *                      TrgDev:
 *                      TrgDev indicates the packet's Egress port
 *                      NOTE: When the tag is not extended, this field always
 *                      indicates SrcDev.
 *              devPort.portNum -
 *                      When rx_sniff = GT_TRUE or when tag is not extended:
 *                      SrcPort:
 *                      indicates the packet's original Ingress port.
 *                      When rx_sniff = GT_FALSE and the tag is extended:
 *                      TrgPort:
 *                      indicates the packet's Egress port.
 *                      NOTE: When this tag is not extended, port is a 5-bit
 *                      field.
 *                      When this tag is not extended, this field always
 *                      indicates SrcPort.
 *
 *
 */
typedef struct{
    GT_BOOL                         rxSniffer;

    GT_BOOL                         isTagged;
    struct
    {
        GT_U8                       devNum;
        GT_U8                       portNum;
    }devPort;

}DXCH_NET_DSA_TO_ANALYZER_STC;

/*
 * typedef: structure DXCH_NET_DSA_TO_CPU_STC
 *
 * Description: structure of "To CPU" DSA tag parameters
 *
 * fields:
 *
 *      isEgressPipe - indicates the type of data forwarded to the CPU.
 *                     the data came from egress/ingress pipe (in PP).
 *          GT_FALSE - The packet was forwarded to the CPU by the Ingress pipe
 *                     and this tag contains the packet's source information.
 *          GT_TRUE  - The packet was forwarded to the CPU by the Egress pipe
 *                     and this tag contains the packet's destination
 *                     information.
 *          NOTE: When this tag is not extended, <isEgressPipe> should be
 *                     considered like GT_FALSE.
 *
 *      isTagged -  When isEgressPipe == GT_FALSE:
 *                      This field is srcTagged.
 *                      srcTagged - This tag contains Source Port information
 *                                  and this bit indicates the VLAN Tag format,
 *                                  in which the packet was received on the
 *                                  network port:
 *                          GT_FALSE - Packet was received from a network port
 *                                     untagged.
 *                          GT_TRUE - Packet was received from a network port
 *                                     tagged.
 *                      When isEgressPipe == GT_TRUE:
 *                      This field is trgTagged.
 *                      trgTagged - This tag contains Target Port information
 *                                  and this bit indicates the VLAN Tag format,
 *                                  in which the packet was transmitted via the
 *                                  network port:
 *                          GT_FALSE - Packet was transmitted to a regular
 *                                     network port untagged.
 *                          GT_TRUE - Packet was transmitted to a regular
 *                                     network port tagged.
 *
 *               NOTE: When isEgressPipe == GT_FALSE and the packet
 *                     forwarded to the CPU is received on a customer port on
 *                     which Nested VLAN is implemented, srcTagged is set to
 *                     GT_FALSE, regardless of the packet's VLAN tag format.
 *                     As the packet is considered untagged, when the packet is
 *                     forwarded to the CPU, the customer's VLAN tag (if any)
 *                     resides after the DSA tag.
 *
 *      devNum - When isEgressPipe == GT_FALSE:
 *                  NOTE : HW device number !!
 *                  This field is srcDev.
 *                      This tag contains Source Port information and this field
 *                      indicates the number of the Source Device on which the
 *                      packet was received.
 *                  When isEgressPipe == GT_TRUE:
 *                      This field is trgDev.
 *                      This tag contains Target Port information and this field
 *                      indicates the number of the Destination Device through
 *                      which the packet was transmitted.
 *
 *      srcIsTrunk -  When a packet's DSA Tag is replaced from FORWARD to
 *                      TO_CPU and FORWARD_DSA<SrcIsTrunk> = GT_TRUE and
 *                      isEgressPipe = GT_FALSE, this field is set to
 *                      GT_TRUE and This Tag hold interface.trunkId which is the
 *                      Trunk number parsed from the FORWARD DSA Tag
 *                  When isEgressPipe == GT_FALSE:
 *                      GT_FALSE - the field of <interface.port> indicates the source
 *                                 port number
 *                      GT_TRUE - the field of <interface.trunkId> indicates the source
 *                                 trunk number
 *                  When isEgressPipe == GT_TRUE:
 *                      this field should be GT_FALSE.
 *                      GT_FALSE - the field of <interface.port> indicates the
 *                                 destination Port through which the packet was
 *                                 transmitted.
 *                      GT_TRUE  - not supported !
 *      interface - interface info
 *             trunkId - see comments about srcIsTrunk field
 *             port    - see comments about srcIsTrunk field
 *
 *      cpuCode - CPU codes that indicate the reason for sending a packet to
 *                the CPU.
 *                Note : this is unified CPU code format (and not HW format))
 *
 *      wasTruncated - Was packet sent to CPU truncated?
 *                GT_FALSE - packet was not truncated.
 *                GT_TRUE - Indicates that only the first 128 bytes of the
 *                          packet are sent to the CPU. The packet's original
 *                          byte count is in <originByteCount> field.
 *
 *      originByteCount - The packet's original byte count.
 *
 *
 */
typedef struct{
   GT_BOOL                          isEgressPipe;

   GT_BOOL                          isTagged;
   GT_U8                            devNum;
   GT_BOOL                          srcIsTrunk;
   union
   {
       GT_TRUNK_ID                  srcTrunkId;
       GT_U8                        portNum;
   }interface;

   CPSS_NET_RX_CPU_CODE_ENT         cpuCode;
   GT_BOOL                          wasTruncated;
   GT_U32                           originByteCount;

}DXCH_NET_DSA_TO_CPU_STC;



/*
 * typedef: structure DXCH_NET_DSA_PARAMS_STC
 *
 * Description: structure of DSA parameters , that contain full information on
 *              DSA for RX,Tx packets to/from CPU from/to PP.
 *
 * fields:
 *      commonParams - common parameters in all DSA tag formats
 *
 *      dsaType - the type of DSA tag.(FORWARD/TO_CPU/FROM_CPU/TO_ANALYZER)
 *
 *      dsaInfo - the DSA tag information:
 *          toCpu - information needed when sending "TO_CPU" packets.
 *              relevant when dsaInfo = TO_CPU
 *          fromCpu - information needed when sending "FROM_CPU" packets.
 *              relevant when dsaInfo = FROM_CPU
 *          toAnalyzer - information needed when sending "TO_ANALYZER" packets.
 *              relevant when dsaInfo = TO_ANALYZER
 *          forward - information needed when sending "FORWARD" packets.
 *              relevant when dsaInfo = FORWARD
 *
*/
typedef struct{
    DXCH_NET_DSA_COMMON_STC  commonParams;

    DXCH_NET_DSA_CMD_ENT           dsaType;
    union{
        DXCH_NET_DSA_TO_CPU_STC         toCpu;
        DXCH_NET_DSA_FROM_CPU_STC       fromCpu;
        DXCH_NET_DSA_TO_ANALYZER_STC    toAnalyzer;
        DXCH_NET_DSA_FORWARD_STC        forward;
    }dsaInfo;
}DXCH_NET_DSA_PARAMS_STC;


/*
 * typedef: structure DXCH_NET_SDMA_TX_PARAMS_STC
 *
 * Description: structure of Tx parameters , that should be set to PP when using
 *              the SDMA channel
 *
 * fields:
 *      recalcCrc - GT_TRUE  - the PP should add CRC to the transmitted packet,
 *                  GT_FALSE - leave packet unchanged.
 *                  NOTE : The DXCH device always add 4 bytes of CRC when need
 *                         to recalcCrc = GT_TRUE
 *
 *
 *      txQueue - the queue that packet should be sent to CPU port. (0..7)
 *
 *      evReqHndl - The application handle got from cpssEventBind for
 *                  CPSS_PP_TX_BUFFER_QUEUE_E events.
 *
 *     invokeTxBufferQueueEvent - invoke Tx buffer Queue event.
 *              when the SDMA copies the buffers of the packet from the CPU ,
 *              the PP may invoke the event of CPSS_PP_TX_BUFFER_QUEUE_E.
 *              this event notify the CPU that the Tx buffers of the packet can
 *              be reused by CPU for other packets. (this in not event of
 *              CPSS_PP_TX_END_E that notify that packet left the SDMA)
 *              The invokeTxBufferQueueEvent parameter control the invoking of
 *              that event for this Packet.
 *              GT_TRUE - PP will invoke the event when buffers are copied (for
 *                        this packet's buffers).
 *              GT_FALSE - PP will NOT invoke the event when buffers are copied.
 *                        (for this packet's buffers).
 *              NOTE :
 *                  when the sent is done as "Tx synchronic" this parameter
 *                  IGNORED (the behavior will be as GT_FALSE)
 *
 *
 *
 */
typedef struct{
    GT_BOOL                         recalcCrc;
    GT_U8                           txQueue;
    /*GT_U32                          evReqHndl;*/
    GT_BOOL                         invokeTxBufferQueueEvent;
}DXCH_NET_SDMA_TX_PARAMS_STC;

/*
 * typedef: structure DXCH_PKT_DESC_STC
 *
 * Description: structure of Tx parameters , that contain full information on
 *              how to send TX packet from CPU to PP.
 *
 * fields:
 *      cmdType         - Defines type of transmition (VLAN, Port).
 *      valid           - This flag indicates that the entry is valid (or not)
 *      entryID         - Entry ID number for Delete/Get/ChangeAllign operations
 *      txSyncMode      - should the packet be send in a sync mode or not
 *      packetIsTagged - the packet (buffers) contain the vlan TAG inside.
 *                  GT_TRUE  - the packet contain the vlan TAG inside.
 *                  GT_FALSE - the packet NOT contain the vlan TAG inside.
 *
 *      cookie  - The user's data to be returned to the Application when in
 *                the Application's buffers can be re-used .
 *
 *      sdmaInfo - the info needed when send packets using the SDMA.
 *
 *      dsaParam - the DSA parameters
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
typedef struct{
    DXCH_TX_CMD_ENT                 cmdType;
    GT_BOOL                         valid;
    GT_U32                          entryId;
    GT_BOOL                         txSyncMode;

    GT_BOOL                         packetIsTagged;
    /*GT_PTR                        cookie;*/
    DXCH_NET_SDMA_TX_PARAMS_STC     sdmaInfo;
    DXCH_NET_DSA_PARAMS_STC         dsaParam;

    GT_U32                          pcktsNum;
    GT_U32                          gap;
    GT_U32                          waitTime;
    GT_U8                           *pcktData[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN];
    GT_U32                          pcktDataLen[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN];
    GT_U32                          numOfBuffers;
    GT_U32                          numSentPackets;

}DXCH_PKT_DESC_STC; /*CPSS_NET_TX_PARAMS_STC;*/





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
} DXCH_MODE_SETINGS_STC;


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
}DXCH_MAC_TYPE;

/*******************************************************************************
* cmdCpssDxChTxSetPacketDesc
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
GT_STATUS cmdCpssDxChTxSetPacketDesc
(
    IN  GT_U32                  entryId,
    IN  DXCH_PKT_DESC_STC       *packetDesc,
    IN  GT_BOOL                 extended,
    OUT GT_U32                  *newEntryIndex
);

/*******************************************************************************
* cmdCpssDxChTxGetPacketDesc
*
* DESCRIPTION:
*       Returns specific packet descriptor entry from the table.
*
* INPUTS:
*     entryIndex   - Entry index in packet descriptor table.
*     dsaCmd       - DSA tag commands
*
* OUTPUTS:
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
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGetPacketDesc
(
    INOUT GT_U32      *entryIndex,
    IN DXCH_NET_DSA_CMD_ENT  dsaCmd,
    OUT DXCH_PKT_DESC_STC *packetDesc
);

/*******************************************************************************
* cmdCpssDxChTxBeginGetPacketDesc
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
GT_STATUS cmdCpssDxChTxBeginGetPacketDesc
(
void
);

/*******************************************************************************
* cmdCpssDxChTxEndGetPacketDesc
*
* DESCRIPTION:
*       Finish operation began by cmdCpssDxChTxBeginGetPacketDesc API function
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
*******************************************************************************/
GT_STATUS cmdCpssDxChTxEndGetPacketDesc
(
    void
);

/*******************************************************************************
* cmdCpssDxChTxClearPacketDesc
*
* DESCRIPTION:
*       Clear all packet descriptors entries from the table.
*
* INPUTS:
*     dsaCmd       - DSA tag commands
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
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxClearPacketDesc
(
    IN DXCH_NET_DSA_CMD_ENT  dsaCmd
);

/*******************************************************************************
* cmdCpssDxChTxDelPacketDesc
*
* DESCRIPTION:
*       Delete packet descriptor entry from the table.
*
* INPUTS:
*       entryID     - Entry ID of packet descriptor to delete.
*       dsaCmd      - DSA tag commands
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
GT_STATUS cmdCpssDxChTxDelPacketDesc
(
    IN  GT_U32      entryID,
    IN DXCH_NET_DSA_CMD_ENT  dsaCmd
);

/*******************************************************************************
* cmdCpssDxChTxStart
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
*       Command   - cmdCpssDxChTxStart
*
* Toolkit:
*       Interface - <prestera/tapi/networkif/commands.api>
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxStart
(
void
);

/*******************************************************************************
* cmdCpssDxChTxStop
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
*       Command   - cmdCpssDxChTxStop
*
* Toolkit:
*       Interface - <prestera/tapi/networkif/commands.api>
*       Using     - <netSendPktByLport, netSendPktByIf, netSendPktByVid,
*                    netFreeTxGtBuf, gtBufCreate>
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxStop
(
void
);

/*******************************************************************************
* cmdCpssDxChTxSetMode
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
GT_STATUS cmdCpssDxChTxSetMode
(
    IN GT_U32   cyclesNum,
    IN GT_U32   gap
);

/*******************************************************************************
* cmdCpssDxChTxGetMode
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
GT_STATUS cmdCpssDxChTxGetMode
(
    OUT GT_U32 *cyclesNum,
    OUT GT_U32 *gap
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__cmdCpssDxChNetTransmith__*/


