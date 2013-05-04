/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* trafficEngineUt.c
*
* DESCRIPTION:
*       -- packet generator , parser.
*    -- with trace capabilities
*    -- each packet can be built from :
*        -- L2 - mac da,sa
*        -- vlan tag (optional) -- single/double tag(Q-in-Q)
*        -- Ethernet encapsulation (optional) -- LLC/SNAP/etherII/Novell
*        -- ether type
*        -- L3 (optional) - ipv4/6 / mpls / arp / ..
*        -- L4 (optional) - tcp / udp / icmp / IGMPv2,3,6 / MLD / RIPv1,2 ..
*        -- payload
*        -- CRC32
*    -- support tunneling (tunnel + passenger)
*        -- each passenger is like a new packet
*    -- each packet has - total length
*    -- other specific info:
*        -- table index , num packets , waitTime
*        -- Tx parameters (CPSS_DXCH_NET_TX_PARAMS_STC):
*            - DSA tag info , txSynch , invokeTxBufferQueueEvent , txQueue ...
*        -- for Rx traffic : CPU code (for 'to_cpu')
*            -- the engine need CB function to get the CPU code of this frame.
*
*-- special CPU codes:
*    list of CPU codes that can be attached to CB for advanced purposes:
*        distinguish between the 'rx in cpu'(any CPU code) and the 'Tx port capture' (tx_analyzer cpu code)
*
*
*-- basic init test capabilities:
*    -- set port in loopback mode
*    -- set port with force link up
*-- emulate SMB port counters:
*    -- read/clear port counter : all/rx/tx all/packets/bytes
*    -- 'capture' - set port as 'Egress mirror port' / use STC Sampling to cpu
*                 - set the CPU as the 'Tx analyzer port'
*                 - CPU will get those frames with specific CPU code.
*
*-- emulate SMB send to port:
*    -- fill table of frames to 'Ingress the device' from the port
*       (port should be in: loopback mode and in link up)
*    -- send all / specific table index
*    -- done in specific task.
*        -- this also allow use to break the sending if we want from terminal
*        -- set FLAG 'Break Sending' from terminal so task will stop sending
*
*
*
*-- support for multi devices types (Dx/ExMxPm/ExMx):
*    -- the engine of build/parse should not care about it.
*    -- the send/receive of packet (a 'table')in CPU will call CB with specific
*        Tx parameters (CPSS_DXCH_NET_TX_PARAMS_STC) /
*        Rx parameter (CPSS_DXCH_NET_RX_PARAMS_STC)
*        and attach those parameter to the frame -->
*            each entry in table point to ' cookie + CB' that hold the specific Rx/Tx info
*
*
*    -- SMP port emulation:
*        -- read/clear counters - attach CB for read counters (per device , per port)
*        -- 'capture' - attach CB for set CPU as analyzer , CB for set port as tx mirrored port
*            CB for set Tx port with Sampling to CPU
*
*
*
*-- NOTE : all traffic that received in the CPU saved in the same 'table'
*          'test' can distinguish between : rx and ' capture of tx' by the
*          mechanism of : 'Special CPU codes'
*
*   -SMB trigger emulation - TBD
*   -Expected results  - TBD
*   -Predefined traffic flows  - TBD
*
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsRand.h>
#include <cpssCommon/cpssFormatConvert.h>
#include <trafficEngine/trafficEngineUt.h>
#include <trafficEngine/trafficGeneratorUt.h>

/* flag that state we need to trace field */
#define UT_TRACE_FIELD_FLAG_CNS     BIT_0
/* flag that state we need to trace bytes */
#define UT_TRACE_BYTES_FLAG_CNS     BIT_1

/* global trace controller --> when traceGlobal == 0 there is no trace
   each bit state other type of trace reason */
static GT_U32  traceGlobal = UT_TRACE_FIELD_FLAG_CNS ;

/* offset of the member of structure or union from the origin in bytes */
#define OFFSET_IN_STRUCT_MAC(stcType,field)    \
    CPSS_FORMAT_CONVERT_STRUCT_MEMBER_OFFSET_MAC(stcType, field)

typedef struct{
    GT_U32  offsetInStruct;
    GT_U8   doTrace;
    char*   nameString;
}UT_TRACE_FIELD_INFO_STC;

typedef struct{
    UT_PACKET_PART_ENT          type;
    UT_TRACE_FIELD_INFO_STC     *fieldPtr;
}UT_TRACE_FIELD_INFO_DB_STC;

/* max number of fields supported for trace */
#define UT_TRACE_MAX_FIELDS  1024

static GT_U32   currIndexInTraceFieldsInfoArray = 0;
/* DB of fields info about their trace */
static UT_TRACE_FIELD_INFO_DB_STC  traceFieldsInfoArray[UT_TRACE_MAX_FIELDS]=
    {  {0,0} };

/* the indexes in the array of traceFieldsInfoArray , that the packet parts start in */
static GT_U32   traceFieldsInfoIndexes[UT_PACKET_PART_MAX_E+1] = {0};

#define UT_TRACE_FIELD_NAME_ADD_DB_MAC(part,field)   \
    if(currIndexInTraceFieldsInfoArray >= UT_TRACE_MAX_FIELDS)        \
        return GT_INIT_ERROR;                                         \
    traceFieldsInfoArray[currIndexInTraceFieldsInfoArray].type = part; \
    traceFieldsInfoArray[currIndexInTraceFieldsInfoArray].fieldPtr = &UT_TRACE_FIELD_GET_MAC(part,field);\
    currIndexInTraceFieldsInfoArray++

#define UT_TRACE_FIELD_NAME_DECLARE_MAC(part,field,offsetInStruct)   \
    static UT_TRACE_FIELD_INFO_STC utTrace_##part##_##field = {offsetInStruct,0,#field}

#define UT_TRACE_FIELD_GET_MAC(part,field)   \
    utTrace_##part##_##field

#define UT_TRACE_FIELD_NAME_INFO_GET_MAC(part,field)   \
    utTrace_##part##_##field.nameString

#define UT_TRACE_FIELD_NAME_TRACE_GET_MAC(part,field)   \
    utTrace_##part##_##field.doTrace

#define UT_TRACE_FIELD_NAME_OFFSET_GET_MAC(part,field)   \
    utTrace_##part##_##field.offsetInStruct

/* flag that state we need to trace field */
#define UT_TRACE_L2_MAC_DA_FLAG_CNS     BIT_0
/* flag that state we need to trace bytes */
#define UT_TRACE_L2_MAC_SA_FLAG_CNS     BIT_1

/* offset of the member from the origin in bytes -- L2 structure*/
#define L2_OFFSET_IN_STRUCT_MAC(field)    \
    OFFSET_IN_STRUCT_MAC(UT_TRAFFIC_ENGINE_PACKET_L2_STC,field)

/* macro to declare L2 fields */
#define UT_TRACE_L2_FIELD_NAME_DECLARE_MAC(field)   \
    UT_TRACE_FIELD_NAME_DECLARE_MAC(UT_PACKET_PART_L2_E,field,L2_OFFSET_IN_STRUCT_MAC(field))

/* macro to add L2 fields to DB */
#define UT_TRACE_L2_FIELD_NAME_ADD_DB_MAC(field)   \
    UT_TRACE_FIELD_NAME_ADD_DB_MAC(UT_PACKET_PART_L2_E,field)


#define UT_TRACE_L2_FIELD_NAME_TRACE_GET_MAC(field)   \
    UT_TRACE_FIELD_NAME_TRACE_GET_MAC(UT_PACKET_PART_L2_E,field)

/* macro to trace mac address */
#define UT_TRACE_MAC_ADDR_MAC(name,value)    \
    osPrintf("%s [%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x]\n",                  \
            #name, value[0],value[1],value[2],value[3],value[4],value[5])

/* macro to trace IPv4 */
#define UT_TRACE_IPV4_ADDR_MAC(name,value)    \
    osPrintf("%s [%d:%d:%d:%d]\n",                  \
            #name, (value>>24)&0xff,(value>>16)&0xff,(value>>8)&0xff,(value>>0)&0xff)

/* macro to trace numeric field in decimal way --> GT_U32,GT_U16,GT_U8 */
#define UT_TRACE_NUMERIC_DECIMAL_MAC(name,value)    \
    osPrintf("%s [%ld]\n", #name, (GT_U32)value)

/* macro to trace numeric field in HEX way --> GT_U32,GT_U16,GT_U8 */
#define UT_TRACE_NUMERIC_HEX_MAC(name,value)    \
    osPrintf("%s [0x%lx]\n", #name, (GT_U32)value)

/* macro to trace the name of field and the value of it -- L2 fields */
#define UT_TRACE_FIELD_IN_L2_MAC(name,value)  \
    if(UT_TRACE_L2_FIELD_NAME_TRACE_GET_MAC(name))  \
        UT_TRACE_MAC_ADDR_MAC(name,value)

/* offset of the member from the origin in bytes -- vlan tag structure*/
#define VLAN_TAG_OFFSET_IN_STRUCT_MAC(field)    \
    OFFSET_IN_STRUCT_MAC(UT_TRAFFIC_ENGINE_PACKET_VLAN_TAG_STC,field)

    /* macro to declare vlan tag fields */
#define UT_TRACE_VLAN_TAG_FIELD_NAME_DECLARE_MAC(field)   \
    UT_TRACE_FIELD_NAME_DECLARE_MAC(UT_PACKET_PART_VLAN_TAG_E,field,VLAN_TAG_OFFSET_IN_STRUCT_MAC(field))

/* macro to add VLAN_TAG fields to DB */
#define UT_TRACE_VLAN_TAG_FIELD_NAME_ADD_DB_MAC(field)   \
    UT_TRACE_FIELD_NAME_ADD_DB_MAC(UT_PACKET_PART_VLAN_TAG_E,field)

#define UT_TRACE_VLAN_TAG_FIELD_NAME_TRACE_GET_MAC(field)   \
    UT_TRACE_FIELD_NAME_TRACE_GET_MAC(UT_PACKET_PART_VLAN_TAG_E,field)

#define UT_TRACE_VLAN_TAG_FIELD_NAME_OFFSET_GET_MAC(field)   \
    UT_TRACE_FIELD_NAME_OFFSET_GET_MAC(UT_PACKET_PART_VLAN_TAG_E,field)


/* macro to trace the name of field and the value of it -- vlan tag fields */
#define UT_TRACE_FIELD_IN_VLAN_TAG_MAC(name,value)  \
    if(UT_TRACE_VLAN_TAG_FIELD_NAME_TRACE_GET_MAC(name))  \
    {                                                                             \
        GT_U32 tmp = UT_TRACE_VLAN_TAG_FIELD_NAME_OFFSET_GET_MAC(name);           \
        if(tmp == UT_TRACE_VLAN_TAG_FIELD_NAME_OFFSET_GET_MAC(etherType) ||       \
           tmp == UT_TRACE_VLAN_TAG_FIELD_NAME_OFFSET_GET_MAC(vid) )              \
        {                                                                         \
                UT_TRACE_NUMERIC_HEX_MAC(name,value);                             \
        }                                                                         \
        else                                                                      \
        {                                                                         \
                UT_TRACE_NUMERIC_DECIMAL_MAC(name,value);                         \
        }                                                                         \
    }


/* offset of the member from the origin in bytes -- ethertype structure*/
#define ETHERTYPE_OFFSET_IN_STRUCT_MAC(field)    \
    OFFSET_IN_STRUCT_MAC(UT_TRAFFIC_ENGINE_PACKET_ETHERTYPE_STC,field)

/* macro to declare etherType fields */
#define UT_TRACE_ETHERTYPE_FIELD_NAME_DECLARE_MAC(field)   \
    UT_TRACE_FIELD_NAME_DECLARE_MAC(UT_PACKET_PART_ETHERTYPE_E,field,ETHERTYPE_OFFSET_IN_STRUCT_MAC(field))

/* macro to add ETHERTYPE fields to DB */
#define UT_TRACE_ETHERTYPE_FIELD_NAME_ADD_DB_MAC(field)   \
    UT_TRACE_FIELD_NAME_ADD_DB_MAC(UT_PACKET_PART_ETHERTYPE_E,field)

#define UT_TRACE_ETHERTYPE_FIELD_NAME_TRACE_GET_MAC(field)   \
    UT_TRACE_FIELD_NAME_TRACE_GET_MAC(UT_PACKET_PART_ETHERTYPE_E,field)


/* macro to trace the name of field and the value of it -- ethertype fields */
#define UT_TRACE_FIELD_IN_ETHERTYPE_MAC(name,value)  \
    if(UT_TRACE_ETHERTYPE_FIELD_NAME_TRACE_GET_MAC(name))  \
        UT_TRACE_NUMERIC_HEX_MAC(name,value)


/* macro to trace the name of field and the value of it -- L2 part */
#define UT_TRACE_L2_FIELD_IN_PART_MAC(name,value)  \
        if(traceGlobal & UT_TRACE_FIELD_FLAG_CNS)  \
            UT_TRACE_FIELD_IN_L2_MAC(name,value)

/* macro to trace the name of field and the value of it -- vlan tag part */
#define UT_TRACE_VLAN_TAG_FIELD_IN_PART_MAC(name,value)  \
        if(traceGlobal & UT_TRACE_FIELD_FLAG_CNS)  \
            UT_TRACE_FIELD_IN_VLAN_TAG_MAC(name,value)

/* macro to trace the name of field and the value of it -- ethertype part */
#define UT_TRACE_ETHERTYPE_FIELD_IN_PART_MAC(name,value)  \
        if(traceGlobal & UT_TRACE_FIELD_FLAG_CNS)  \
            UT_TRACE_FIELD_IN_ETHERTYPE_MAC(name,value)

/* macro to trace the name of fields and the value of it -- in vlan tag */
#define UT_TRACE_VLAN_TAG_FIELDS_IN_PART_MAC(infoPtr)  \
    UT_TRACE_VLAN_TAG_FIELD_IN_PART_MAC(etherType,infoPtr->etherType); \
    UT_TRACE_VLAN_TAG_FIELD_IN_PART_MAC(vid,infoPtr->vpt);             \
    UT_TRACE_VLAN_TAG_FIELD_IN_PART_MAC(cfi,infoPtr->cfi);             \
    UT_TRACE_VLAN_TAG_FIELD_IN_PART_MAC(vid,infoPtr->vid)

/* macro to trace the name of fields and the value of it -- in ethretype */
#define UT_TRACE_ETHERTYPE_FIELDS_IN_PART_MAC(infoPtr)  \
    UT_TRACE_ETHERTYPE_FIELD_IN_PART_MAC(etherType,infoPtr->etherType)

/* macro to trace the name of fields and the value of it -- in L2 */
#define UT_TRACE_L2_FIELDS_IN_PART_MAC(infoPtr)  \
    UT_TRACE_L2_FIELD_IN_PART_MAC(daMac,infoPtr->daMac);\
    UT_TRACE_L2_FIELD_IN_PART_MAC(saMac,infoPtr->saMac)
/************************/
/* declare trace fields */
/************************/
/* L2 */
UT_TRACE_L2_FIELD_NAME_DECLARE_MAC(daMac);
UT_TRACE_L2_FIELD_NAME_DECLARE_MAC(saMac);
/* vlan tag */
UT_TRACE_VLAN_TAG_FIELD_NAME_DECLARE_MAC(etherType);
UT_TRACE_VLAN_TAG_FIELD_NAME_DECLARE_MAC(vpt);
UT_TRACE_VLAN_TAG_FIELD_NAME_DECLARE_MAC(cfi);
UT_TRACE_VLAN_TAG_FIELD_NAME_DECLARE_MAC(vid);
/* ethertype */
UT_TRACE_ETHERTYPE_FIELD_NAME_DECLARE_MAC(etherType);


extern GT_STATUS utTrafficDemoInit(
    void
);


/*******************************************************************************
* partCopy
*
* DESCRIPTION:
*       copy part of packet from source to destination
*
* INPUTS:
*       destPtr   - to where to copy the data from the source
*       sourcePtr - from where to copy the data to the destination
*       numOfBytes - (max) number of bytes to write
*       currentBytePtr - (pointer to) the first byte index in bufferPtr to write to
*       maxByteIndexToUpdate - the max index allowed to be modified in bufferPtr
* OUTPUTS:
*       currentBytePtr - (pointer to) the first byte index in bufferPtr for the
*                       next builder to write to
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void partCopy(
    IN  GT_U8                 *destPtr,
    OUT const GT_U8           *sourcePtr,
    IN  GT_U32                 numOfBytes,
    INOUT   GT_U32            *currentBytePtr,
    IN  GT_U32                 maxByteIndexToUpdate
)
{
    GT_U32  ii;

    for (ii = 0 ;
         ii < numOfBytes && (*currentBytePtr) <= maxByteIndexToUpdate ;
         ii ++ , (*currentBytePtr)++)
    {
        destPtr[*currentBytePtr] = sourcePtr[ii];
    }

    return;
}

/*******************************************************************************
* partBuild
*
* DESCRIPTION:
*       build part of packet from specific part input fields
*
* INPUTS:
*       partPtr   - part fields info
*       currentBytePtr - (pointer to) the first byte index in bufferPtr to write to
*       maxByteIndexToUpdate - the max index allowed to be modified in bufferPtr
* OUTPUTS:
*       currentBytePtr - (pointer to) the first byte index in bufferPtr for the
*                       next builder to write to
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void partBuildL2(
    IN  void                  *partPtr,
    INOUT   GT_U32            *currentBytePtr,
    IN  GT_U32                 maxByteIndexToUpdate,
    OUT GT_U8                 *bufferPtr
)
{
    UT_TRAFFIC_ENGINE_PACKET_L2_STC*    l2Ptr = partPtr;

    UT_TRACE_L2_FIELDS_IN_PART_MAC(l2Ptr);

    partCopy(bufferPtr,
             &l2Ptr->daMac[0], sizeof(l2Ptr->daMac) ,
             currentBytePtr , maxByteIndexToUpdate);

    partCopy(bufferPtr,
             &l2Ptr->saMac[0], sizeof(l2Ptr->saMac) ,
             currentBytePtr , maxByteIndexToUpdate);

    return;
}

/*******************************************************************************
* partBuildEtherType
*
* DESCRIPTION:
*       build part of packet from specific part input fields
*
* INPUTS:
*       partPtr   - part fields info
*       currentBytePtr - (pointer to) the first byte index in bufferPtr to write to
*       maxByteIndexToUpdate - the max index allowed to be modified in bufferPtr
*       calledFromVlan - called from vlan
* OUTPUTS:
*       currentBytePtr - (pointer to) the first byte index in bufferPtr for the
*                       next builder to write to
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void partBuildEtherType(
    IN  void                  *partPtr,
    INOUT   GT_U32            *currentBytePtr,
    IN  GT_U32                 maxByteIndexToUpdate,
    OUT GT_U8                 *bufferPtr,
    IN GT_BOOL                 calledFromVlan
)
{
    UT_TRAFFIC_ENGINE_PACKET_ETHERTYPE_STC*    etherTypePtr = partPtr;
    GT_U8   etherType[2];

    etherType[1] = (GT_U8)(etherTypePtr->etherType & 0xff) ;
    etherType[0] = (GT_U8)((etherTypePtr->etherType >> 8) & 0xff);

    if(calledFromVlan == GT_FALSE)
    {
        UT_TRACE_ETHERTYPE_FIELDS_IN_PART_MAC(etherTypePtr);
    }

    partCopy(bufferPtr,
             &etherType[0], sizeof(etherType) ,
             currentBytePtr , maxByteIndexToUpdate);

    return;
}

/*******************************************************************************
* partBuild
*
* DESCRIPTION:
*       build part of packet from specific part input fields
*
* INPUTS:
*       partPtr   - part fields info
*       currentBytePtr - (pointer to) the first byte index in bufferPtr to write to
*       maxByteIndexToUpdate - the max index allowed to be modified in bufferPtr
* OUTPUTS:
*       currentBytePtr - (pointer to) the first byte index in bufferPtr for the
*                       next builder to write to
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void partBuildVlanTag(
    IN  void                  *partPtr,
    INOUT   GT_U32            *currentBytePtr,
    IN  GT_U32                 maxByteIndexToUpdate,
    OUT GT_U8                 *bufferPtr
)
{
    UT_TRAFFIC_ENGINE_PACKET_VLAN_TAG_STC*    vlanTagPtr = partPtr;
    GT_U8   vtag[2];

    /* the vlan tag start with ethertype field */
    partBuildEtherType(partPtr,currentBytePtr,maxByteIndexToUpdate,bufferPtr,GT_TRUE);

    vtag[1] = (GT_U8)(vlanTagPtr->vid & 0xff) ;
    vtag[0] = (GT_U8)(((vlanTagPtr->vid >> 8) & 0x0f)  |
                      ((vlanTagPtr->cfi & 1) << 4)     |
                      ((vlanTagPtr->vpt & 7) << 5) );

    UT_TRACE_VLAN_TAG_FIELDS_IN_PART_MAC(vlanTagPtr);

    partCopy(bufferPtr,
             &vtag[0], sizeof(vtag) ,
             currentBytePtr , maxByteIndexToUpdate);

    return;
}


/*******************************************************************************
* partBuild
*
* DESCRIPTION:
*       build part of packet from specific part input fields
*
* INPUTS:
*       partPtr   - part fields info
*       currentBytePtr - (pointer to) the first byte index in bufferPtr to write to
*       maxByteIndexToUpdate - the max index allowed to be modified in bufferPtr
* OUTPUTS:
*       currentBytePtr - (pointer to) the first byte index in bufferPtr for the
*                       next builder to write to
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void partBuildWildCard(
    IN  void                  *partPtr,
    INOUT   GT_U32            *currentBytePtr,
    IN  GT_U32                 maxByteIndexToUpdate,
    OUT GT_U8                 *bufferPtr
)
{
    UT_TRAFFIC_ENGINE_PACKET_WILDCARD_STC*    wildCardPtr = partPtr;

    partCopy(bufferPtr,
             &wildCardPtr->bytesPtr[0], wildCardPtr->numOfBytes ,
             currentBytePtr , maxByteIndexToUpdate);

    return;
}

/*******************************************************************************
* partBuild
*
* DESCRIPTION:
*       build part of packet from specific part input fields
*
* INPUTS:
*       partPtr   - part fields info
*       currentBytePtr - (pointer to) the first byte index in bufferPtr to write to
*       maxByteIndexToUpdate - the max index allowed to be modified in bufferPtr
* OUTPUTS:
*       currentBytePtr - (pointer to) the first byte index in bufferPtr for the
*                       next builder to write to
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void partBuildPayLoad(
    IN  void                  *partPtr,
    INOUT   GT_U32            *currentBytePtr,
    IN  GT_U32                 maxByteIndexToUpdate,
    OUT GT_U8                 *bufferPtr
)
{
    partBuildWildCard(partPtr,currentBytePtr,maxByteIndexToUpdate,bufferPtr);

    return;
}

/*******************************************************************************
* partBuild
*
* DESCRIPTION:
*       build part of packet from specific part input fields
*
* INPUTS:
*       partPtr   - part fields info
*       currentBytePtr - (pointer to) the first byte index in bufferPtr to write to
*       maxByteIndexToUpdate - the max index allowed to be modified in bufferPtr
* OUTPUTS:
*       currentBytePtr - (pointer to) the first byte index in bufferPtr for the
*                       next builder to write to
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong parameter.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_BAD_STATE                - parser caused bad state that should not
*                                     happen
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS partBuild(
    IN  UT_PACKET_PART_STC    *partPtr,
    INOUT   GT_U32            *currentBytePtr,
    IN  GT_U32                 maxByteIndexToUpdate,
    OUT GT_U8                 *bufferPtr
)
{
    switch(partPtr->type)
    {
        case UT_PACKET_PART_L2_E:
            partBuildL2(partPtr->partPtr,currentBytePtr,maxByteIndexToUpdate,bufferPtr);
            break;
        case UT_PACKET_PART_VLAN_TAG_E:
            partBuildVlanTag(partPtr->partPtr,currentBytePtr,maxByteIndexToUpdate,bufferPtr);
            break;
        case UT_PACKET_PART_ETHERTYPE_E:
            partBuildEtherType(partPtr->partPtr,currentBytePtr,maxByteIndexToUpdate,bufferPtr,GT_FALSE);
            break;
        case UT_PACKET_PART_WILDCARD_E:
            partBuildWildCard(partPtr->partPtr,currentBytePtr,maxByteIndexToUpdate,bufferPtr);
            break;
        case UT_PACKET_PART_PAYLOAD_E:
            partBuildPayLoad(partPtr->partPtr,currentBytePtr,maxByteIndexToUpdate,bufferPtr);
            break;
        default:
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* utTrafficEnginePacketBuild
*
* DESCRIPTION:
*       build packet from input fields
*
* INPUTS:
*       packetInfoPtr   - packet fields info
*
* OUTPUTS:
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong parameter.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_BAD_STATE                - parser caused bad state that should not
*                                     happen
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficEnginePacketBuild(
    IN  UT_TRAFFIC_ENGINE_PACKET_STC    *packetInfoPtr,
    OUT GT_U8                           *bufferPtr
)
{
    GT_STATUS   rc;
    GT_U32  ii;
    GT_U32  currentByte;/* the current byte index in the buffer to write to */

    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    /* start writing to byte 0 in the buffer */
    currentByte = 0;

    /* loop on all parts and add each part after the other */
    for(ii = 0 ; ii < packetInfoPtr->numOfParts ; ii++)
    {
        rc = partBuild(&packetInfoPtr->partsArray[ii],
                  &currentByte, /* function will update the current byte for the
                                   next builder */
                  (packetInfoPtr->totalLen - 1),/* max index to write to (the buffer not allocated
                                from more then it) -- NOTE : buffer must be allocated
                                for CRC (4 bytes) */
                  bufferPtr);

        if(rc != GT_OK)
        {
            return rc;
        }

        if(currentByte == packetInfoPtr->totalLen)
        {
            /* no need to put any extra info , since total packet length is
               reached */
            break;
        }
        else if(currentByte > packetInfoPtr->totalLen)
        {
            /* we caused overflow by writing to out of buffer */
            /* should not happen */
            return GT_BAD_STATE;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* utTrafficEnginePacketParse
*
* DESCRIPTION:
*       parse packet from bytes to parts and fields
*
* INPUTS:
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*       bufferLength-length of the buffer
*
* OUTPUTS:
*       packetInfoPtrPtr   - (pointer to) pointer to packet fields info
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong parameter.
*       GT_BAD_PTR                  - on NULL pointer
*       GT_BAD_STATE                - parser caused bad state that should not
*                                     happen
*
* COMMENTS:
*
*       the (*packetInfoPtrPtr) is pointer to static memory that is 'reused' for
*       every new packet that need 'parse' , so use this packet info before
*       calling to new 'parse'
*
*
*******************************************************************************/
GT_STATUS utTrafficEnginePacketParse(
    IN GT_U8                             *bufferPtr,
    IN GT_U32                            bufferLength,
    OUT  UT_TRAFFIC_ENGINE_PACKET_STC    **packetInfoPtrPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetInfoPtrPtr);
    UT_TRAFFIC_GENERATOR_PARAM_NUT_USED(bufferLength);

    return GT_NOT_IMPLEMENTED;
}


/*******************************************************************************
* utTrafficEnginePacketVfdApply
*
* DESCRIPTION:
*       apply VFD info on the buffer
*
* INPUTS:
*       vfdPtr - (pointer to) VFD info
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*                   before modification
*       bufferLength - the number of bytes in the buffer (not include CRC bytes)
* OUTPUTS:
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*                   after modification
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong parameter.
*       GT_BAD_PTR                  - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficEnginePacketVfdApply(
    IN    UT_TRAFFIC_ENGINE_VFD_STC       *vfdPtr,
    INOUT GT_U8                           *bufferPtr,
    IN    GT_U32                           bufferLength
)
{
    GT_U32  ii;
    CPSS_NULL_PTR_CHECK_MAC(vfdPtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    if(vfdPtr->mode == UT_TRAFFIC_ENGINE_VFD_MODE_OFF_E)
    {
        return GT_OK;
    }
    else
    {
        if(vfdPtr->cycleCount > UT_TRAFFIC_ENGINE_VFD_PATTERN_BYTES_NUM_CNS)
        {
            return GT_BAD_PARAM;
        }

        if(vfdPtr->offset + vfdPtr->cycleCount >= bufferLength)
        {
            return GT_BAD_PARAM;
        }
    }

    switch(vfdPtr->mode)
    {
        case UT_TRAFFIC_ENGINE_VFD_MODE_RANDOM_E:
            for(ii = 0 ; ii < vfdPtr->cycleCount; ii++)
            {
                bufferPtr[vfdPtr->offset+ii] = (GT_U8)osRand();
            }
            break;
        case UT_TRAFFIC_ENGINE_VFD_MODE_INCREMENT_E:
            if(vfdPtr->modeExtraInfo == 0)
            {
                /* for the first time not arithmetic needed , copy the bytes and return */
                for(ii = 0 ; ii < vfdPtr->cycleCount; ii++)
                {
                    bufferPtr[vfdPtr->offset+ii] = vfdPtr->patternPtr[ii];
                }
                vfdPtr->modeExtraInfo++;
                break;
            }

            /* for the other times arithmetic needed, increment/decrement */
            vfdPtr->modeExtraInfo++;

            ii = vfdPtr->cycleCount-1;
            bufferPtr[vfdPtr->offset+ii]++;
            while(1)
            {
                if((bufferPtr[vfdPtr->offset+ii] != 0) ||
                    ((vfdPtr->offset+ii)==0) || (ii == 0))
                {
                    break;
                }

                ii--;
                bufferPtr[vfdPtr->offset+ii]++;
            }
            break;
        case UT_TRAFFIC_ENGINE_VFD_MODE_DECREMENT_E:
            if(vfdPtr->modeExtraInfo == 0)
            {
                /* for the first time not arithmetic needed , copy the bytes and return */
                for(ii = 0 ; ii < vfdPtr->cycleCount; ii++)
                {
                    bufferPtr[vfdPtr->offset+ii] = vfdPtr->patternPtr[ii];
                }
                vfdPtr->modeExtraInfo++;
                break;
            }

            /* for the other times arithmetic needed, increment/decrement */
            vfdPtr->modeExtraInfo++;

            ii = vfdPtr->cycleCount-1;
            bufferPtr[vfdPtr->offset+ii]--;
            while(1)
            {
                if((bufferPtr[vfdPtr->offset+ii] != 0xff) ||
                    ((vfdPtr->offset+ii)==0) || (ii == 0))
                {
                    break;
                }

                ii--;
                bufferPtr[vfdPtr->offset+ii]--;
            }
            break;
        case UT_TRAFFIC_ENGINE_VFD_MODE_STATIC_E:
            if(vfdPtr->modeExtraInfo == 0)
            {
                /* for the first time copy the bytes and return */
                for(ii = 0 ; ii < vfdPtr->cycleCount; ii++)
                {
                    bufferPtr[vfdPtr->offset+ii] = vfdPtr->patternPtr[ii];
                }
            }
            /* for the next times , do nothing */
            vfdPtr->modeExtraInfo++;
            break;
        default:
            return GT_BAD_PARAM;

    }


    return GT_OK;
}


/*******************************************************************************
* utTrafficEnginePacketVfdCheck
*
* DESCRIPTION:
*       Check if the VFD parameters match the buffer
*
* INPUTS:
*       vfdPtr - (pointer to) VFD info (must be  vfdPtr->mode ==
*                                       UT_TRAFFIC_ENGINE_VFD_MODE_STATIC_E)
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*       bufferLength - the number of bytes in the buffer (not include CRC bytes)
* OUTPUTS:
*       matchPtr    - (pointer to) do we have a match
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong parameter.
*       GT_BAD_PTR                  - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficEnginePacketVfdCheck(
    IN    UT_TRAFFIC_ENGINE_VFD_STC       *vfdPtr,
    IN    GT_U8                           *bufferPtr,
    IN    GT_U32                           bufferLength,
    OUT   GT_BOOL                          *matchPtr
)
{
    GT_U32  ii;
    CPSS_NULL_PTR_CHECK_MAC(vfdPtr);
    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);
    CPSS_NULL_PTR_CHECK_MAC(matchPtr);

    switch(vfdPtr->mode)
    {
        case UT_TRAFFIC_ENGINE_VFD_MODE_STATIC_E:
            if(vfdPtr->cycleCount > UT_TRAFFIC_ENGINE_VFD_PATTERN_BYTES_NUM_CNS)
            {
                return GT_BAD_PARAM;
            }

            if(vfdPtr->offset + vfdPtr->cycleCount >= bufferLength)
            {
                return GT_BAD_PARAM;
            }

            for(ii = 0 ; ii < vfdPtr->cycleCount; ii++)
            {
                if(bufferPtr[vfdPtr->offset+ii] != vfdPtr->patternPtr[ii])
                {
                    *matchPtr = GT_FALSE;
                    return GT_OK;
                }
            }
            *matchPtr = GT_TRUE;
            return GT_OK;
        default:
            /* not supported */
            return GT_BAD_PARAM;
    }
}

/*******************************************************************************
* utTrafficEnginePacketCheckSum16BitsCalc
*
* DESCRIPTION:
*       calculate checksum of 16 bits
*
* INPUTS:
*       bytesPtr   - (pointer) to start of section in packet need to be calculated
*       numBytes   - number of bytes need to be included in the calculation
* OUTPUTS:
*       checkSumArray - array of 2 bytes with the calculated check sum
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong parameter.
*       GT_BAD_PTR                  - on NULL pointer
*
* COMMENTS:
*        If there's a field CHECKSUM within the input-buffer
*           it supposed to be zero before calling this function.
*
*
*******************************************************************************/
GT_STATUS utTrafficEnginePacketCheckSum16BitsCalc(
    IN GT_U8                *bytesPtr,
    IN GT_U32               numBytes,
    OUT GT_U8               checkSumArray[2]
)
{
    GT_U32     vResult;
    union
    {
        GT_U32     OneUlong;      /* 32 bit integer */
        struct
        {
            GT_U16 word1;     /* 1st word */
            GT_U16 word2;     /* 2st word */
        } TwoUshorts;
    } sum;
    GT_U32 i;
    GT_U32 nUShorts;
    GT_U16 *pInt;
    GT_U16 vPseudoWord;
    GT_U16 *tmpU16Ptr;
    GT_U8   *bytePtr;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/

    CPSS_NULL_PTR_CHECK_MAC(bytesPtr);
    CPSS_NULL_PTR_CHECK_MAC(checkSumArray);

    sum.OneUlong = vResult = 0;
    CPSS_TBD_BOOKMARK;/* Endian issue ?! */
    pInt = (GT_U16*) bytesPtr;

    /* number of 16bit quantities in header */
    nUShorts = numBytes / 2;

    /* add up all of the 16 bit quantities */
    for (i = 0; i < nUShorts;  i++)
    {
        if (i != 5)
        {
            sum.OneUlong += *pInt;
        }
        pInt++;
    }


    if ( numBytes > (2 * nUShorts) )
    {
        /* an odd number of bytes */
        /* the last byte is appended by a zero at the right of it */
        vPseudoWord = (GT_U16)((*((GT_U8*)pInt)) << 8);

        pInt = &vPseudoWord;
        /* add the last word to the sum */
        sum.OneUlong +=  *pInt ;
    }

    /* sum together the two words of the double-word */
    vResult = (GT_U32)sum.TwoUshorts.word1 + (GT_U32)sum.TwoUshorts.word2 ;

    /* vResult can be bigger then a word (example : 0003 +
    *  fffe = 0001 0001), so sum its two words again.
    */
    tmpU16Ptr = (GT_U16*)((void *)&vResult);
    vResult = *( tmpU16Ptr + 1) + *(tmpU16Ptr) ;

    /*invert the bits*/
    vResult = ~vResult;

    bytePtr = (GT_U8*)&vResult;

    checkSumArray[0] = bytePtr[0];
    checkSumArray[1] = bytePtr[1];

    return GT_OK;
}

/*******************************************************************************
* utTrafficTracePacketBytes
*
* DESCRIPTION:
*       trace the packet bytes . will trace only if was set to 'GT_TRUE' in
*       function utTrafficTracePacketSet
*
* INPUTS:
*       bufferPtr   - (pointer to) the buffer that hold the packet bytes
*       length      - length of packet
* OUTPUTS:
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PTR                  - on NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS utTrafficTracePacket(
    IN GT_U8                           *bufferPtr,
    IN GT_U32                           length
)
{
    GT_U32  ii;

    CPSS_NULL_PTR_CHECK_MAC(bufferPtr);

    if(traceGlobal & UT_TRACE_BYTES_FLAG_CNS)
    {
        osPrintf("utTrafficTracePacket : packet trace , length [%d](include CRC) \n",length);
        for(ii = 0; ii < length; ii++)
        {
            if((ii & 0x0f) == 0)
            {
                osPrintf("0x%4.4x :",ii);
            }

            osPrintf(" %2.2x",bufferPtr[ii]);

            if((ii & 0x0f) == 0x0f)
            {
                osPrintf("\n");
            }
        }
        osPrintf("\n");
    }

    return GT_OK;
}

/*******************************************************************************
* utTrafficTraceFieldInPartInPacketSet
*
* DESCRIPTION:
*       open/close the tracing for a field
*
* INPUTS:
*       packetPartType  - the part that represent the 'context' of field
*       trace           - trace enable/disable
*       fieldOffset     - offset in bytes of field from start of structure that
*                         represent the packetPartType
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_BAD_PARAM                - on wrong packetPartType parameter.
*       GT_NO_SUCH                  - the field is not member of the part
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   utTrafficTraceFieldInPartInPacketSet
(
    IN UT_PACKET_PART_ENT   packetPartType,
    IN GT_BOOL              trace,
    IN GT_U32               fieldOffset
)
{
    GT_U32  ii;
    switch(packetPartType)
    {
        case UT_PACKET_PART_L2_E:
        case UT_PACKET_PART_VLAN_TAG_E:
        case UT_PACKET_PART_ETHERTYPE_E:
        case UT_PACKET_PART_WILDCARD_E:
        case UT_PACKET_PART_PAYLOAD_E:
            break;
        default:
            return GT_BAD_PARAM;
    }

    for(ii = traceFieldsInfoIndexes[packetPartType] ;
        ii < traceFieldsInfoIndexes[packetPartType+1];
        ii++)
    {
        if (fieldOffset < traceFieldsInfoArray[ii].fieldPtr->offsetInStruct)
        {
            /* no need to continue .
              the fields suppose to be in sorted within the part */
            break;
        }
        else if(fieldOffset > traceFieldsInfoArray[ii].fieldPtr->offsetInStruct)
        {
            continue;
        }

        traceFieldsInfoArray[ii].fieldPtr->doTrace = (GT_U8)(BOOL2BIT_MAC(trace));

        return GT_OK;
    }

    return GT_NO_SUCH;

}

/*******************************************************************************
* utTrafficTraceAllFieldsInPartSet
*
* DESCRIPTION:
*       open/close the tracing for all fields in specific packet
*
* INPUTS:
*       packetPartType  - the part that represent the 'context' of field
*       trace           - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   utTrafficTraceAllFieldsInPartSet
(
    IN UT_PACKET_PART_ENT   packetPartType,
    IN GT_BOOL              trace
)
{
    GT_U32  fieldIndex = 0;

    while(traceFieldsInfoArray[fieldIndex].type != packetPartType)
    {
        fieldIndex++;

        if(fieldIndex >= currIndexInTraceFieldsInfoArray)
        {
            return GT_NOT_FOUND;
        }
    }

    while(traceFieldsInfoArray[fieldIndex].type == packetPartType)
    {
        if(GT_OK != utTrafficTraceFieldInPartInPacketSet(packetPartType,trace,
            traceFieldsInfoArray[fieldIndex].fieldPtr->offsetInStruct))
        {
            break;
        }

        fieldIndex++;

        if(fieldIndex >= currIndexInTraceFieldsInfoArray)
        {
            break;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* utTrafficTraceAllFieldsInPartsInPacketSet
*
* DESCRIPTION:
*       open/close the tracing for all fields in packet (all parts)
*
* INPUTS:
*       trace           - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   utTrafficTraceAllFieldsInPartsInPacketSet
(
    IN GT_BOOL              trace
)
{
    GT_U32  packetPartType;

    for(packetPartType = 0;
        packetPartType < UT_PACKET_PART_MAX_E;
        packetPartType++)
    {
        if(GT_OK != utTrafficTraceAllFieldsInPartSet(packetPartType,trace))
        {
            break;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* utTrafficTracePacketByteSet
*
* DESCRIPTION:
*       open/close the tracing on bytes of packets
*
* INPUTS:
*       trace           - trace enable/disable
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   utTrafficTracePacketByteSet
(
    IN GT_BOOL              trace
)
{
    if(trace == GT_TRUE)
    {
        traceGlobal |= UT_TRACE_BYTES_FLAG_CNS;
    }
    else
    {
        traceGlobal &= ~UT_TRACE_BYTES_FLAG_CNS;
    }

    return GT_OK;
}


/*******************************************************************************
* utTrafficTraceInit
*
* DESCRIPTION:
*       initialize the DB of the trace utility
*
* INPUTS:
*       None
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_INIT_ERROR   - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   utTrafficTraceInit
(
    void
)
{
    GT_U32      ii = 0;


    utTrafficDemoInit();


    /* add l2 fields to DB */
    traceFieldsInfoIndexes[ii++] = currIndexInTraceFieldsInfoArray;
    UT_TRACE_L2_FIELD_NAME_ADD_DB_MAC(daMac);
    UT_TRACE_L2_FIELD_NAME_ADD_DB_MAC(saMac);

    /* add vlan tag to DB */
    traceFieldsInfoIndexes[ii++] = currIndexInTraceFieldsInfoArray;
    UT_TRACE_VLAN_TAG_FIELD_NAME_ADD_DB_MAC(etherType);
    UT_TRACE_VLAN_TAG_FIELD_NAME_ADD_DB_MAC(vpt);
    UT_TRACE_VLAN_TAG_FIELD_NAME_ADD_DB_MAC(cfi);
    UT_TRACE_VLAN_TAG_FIELD_NAME_ADD_DB_MAC(vid);

    /* add etherType to DB */
    traceFieldsInfoIndexes[ii++] = currIndexInTraceFieldsInfoArray;
    UT_TRACE_ETHERTYPE_FIELD_NAME_ADD_DB_MAC(etherType);


    /* MUST be last line */
    traceFieldsInfoIndexes[ii] = currIndexInTraceFieldsInfoArray;
    return GT_OK;
}


