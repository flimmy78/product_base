/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTrunk.c
*
* DESCRIPTION:
*     CPSS trunk testing implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/port/cpssPortStat.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <common/tgfPortGen.h>

#include <utf/utfMain.h>
#include <utf/private/prvUtfExtras.h>
#include <utf/private/prvUtfHelpers.h>

#include <trafficEngine/tgfTrafficGenerator.h>
#include <trafficEngine/tgfTrafficTable.h>

#include <common/tgfCommon.h>
#include <common/tgfBridgeGen.h>
#include <common/tgfTrunkGen.h>
#include <common/tgfCscdGen.h>
#include <common/tgfPclGen.h>
#include <trunk/prvTgfTrunk.h>

/********remove all printings to the log from this file **************/
/********  due to large printings ************************************/
static GT_BOOL  trunkOpenLog = 0;
#define TRUNK_PRV_UTF_LOG0_MAC \
    if(trunkOpenLog) PRV_UTF_LOG0_MAC

#define TRUNK_PRV_UTF_LOG1_MAC \
    if(trunkOpenLog) PRV_UTF_LOG1_MAC

#define TRUNK_PRV_UTF_LOG2_MAC \
    if(trunkOpenLog) PRV_UTF_LOG2_MAC

#define TRUNK_PRV_UTF_LOG4_MAC \
    if(trunkOpenLog) PRV_UTF_LOG4_MAC
/*********************************************************************/



/******************************************************************************\
 *                       Test configuration section                           *
\******************************************************************************/
/* default Trunk Id */
#define PRV_TGF_TRUNK_ID_CNS  25

/* secondary Trunk Id */
#define PRV_TGF_TRUNK_ID_2_CNS  13

/* index of port in port-array for trunk member 0 */
#define PRV_TGF_TRUNK_PORT0_INDEX_CNS     0

/* index of port in port-array for trunk member 1 */
#define PRV_TGF_TRUNK_PORT1_INDEX_CNS     2

/* index of port in port-array for trunk member 2 */
#define PRV_TGF_TRUNK_PORT2_INDEX_CNS     1

/* index of port in port-array for trunk member 3 */
#define PRV_TGF_TRUNK_PORT3_INDEX_CNS     3

/* index of port in port-array to send traffic to */
#define PRV_TGF_SEND_PORT_INDEX_CNS     1

/* 8 ports in test */
#define TRUNK_8_PORTS_IN_TEST_CNS       8
/* 4 ports of test are not in trunk */
#define NOT_IN_TRUNK_PORTS_NUM_CNS      4

/* to avoid running test of about 40 minutes , run only subset of it in windows */
/* it means that in Linux_sim and on HW it will run full test */
#define FORCE_REDUCED_TEST_MAC

/* burst size of the CRC hash tests .
NOTE: bad LBH achieved when burst size was 16 or 32 !

Most fields where OK with 64 packets but some of them needed 128 for good LBH !
*/
#define CRC_HASH_BURST_COUNT_NEW_CNS             128
/* number of mac addresses for incremental DA */
#define FDB_MAC_COUNT_CNS             64

/* trunk '1' members */
static CPSS_TRUNK_MEMBER_STC    trunk_1_Members[2];
/* trunk '2' members */
static CPSS_TRUNK_MEMBER_STC    trunk_2_Members[2];

static CPSS_TRUNK_MEMBER_STC    designatedMember3;
static CPSS_TRUNK_MEMBER_STC    designatedMember4;

#define BURST_COUNT_CNS                 30
#define NO_TRAFFIC_EXPECTED_CNS         0
#define FULL_TRAFFIC_EXPECTED_CNS       (BURST_COUNT_CNS * 4)
#define HALF_TRAFFIC_EXPECTED_CNS       (FULL_TRAFFIC_EXPECTED_CNS / 2)

/* the specific members of a trunk that will get traffic when the LBH
   expected mode is : PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E.
   bmp of indexes in the arrays of :
       prvTgfDevsArray[] , prvTgfPortsArray[]
           the traffic should do LBH in those ports and not in other ports of
           the trunk.

       WARNING : this is NOT a good practice to set the system to get this behavior
*/
#define TRUNK_LBH_MEMBERS_INDEXES_ALL_BMP_CNS 0xFFFFFFFF
static GT_U32   trunkLbhMembersIndexesBmp = TRUNK_LBH_MEMBERS_INDEXES_ALL_BMP_CNS;
/* the limited number of members of a trunk that will get traffic when the LBH
   expected mode is : PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E.

       WARNING : this is NOT a good practice to set the system to get this behavior
*/
static GT_U32   trunkLbhNumOfLimitedMemebers = 0;

static GT_U8    testPortsFor60Ports[TRUNK_8_PORTS_IN_TEST_CNS] =
    {0,     16+1,       32+2,       48+3 , /* ports not in trunk */
     2 ,      19,         41,         57 };/* ports of trunk */

/* ports that used with force configuration */
static GT_U8    testPortsFor60PortsConfigForced[TRUNK_8_PORTS_IN_TEST_CNS] =
    {0,     16,       34,       48 , /* ports not in trunk */
     2 ,    18,       40,       58 };/* ports of trunk */

/* cascade trunk in Lion A must be with Id that match to the port group of the cascade ports */
#define PORT_GROUP_OF_CASCADE_TRUNK_LION_A_CNS  (U32_GET_FIELD_MAC(PRV_TGF_TRUNK_ID_CNS,4,2))

#define PORT_OF_CASCADE_TRUNK_LION_A_WITH_OFFSET_MAC(_offset)    \
    (PORT_GROUP_OF_CASCADE_TRUNK_LION_A_CNS << 4) + (_offset)

/*ports of cascade trunk - in the same port group -- limitation for Lion A */
static GT_U8    testPortsFor60Ports_cascadeTrunkPortsSamePortGroup[TRUNK_8_PORTS_IN_TEST_CNS] =
    {0,     16+1,       32+2,       48+3 , /* ports not in trunk */
     PORT_OF_CASCADE_TRUNK_LION_A_WITH_OFFSET_MAC(3), /* cascade trunk in Lion A must be with Id that match to the port group of the cascade ports */
     PORT_OF_CASCADE_TRUNK_LION_A_WITH_OFFSET_MAC(4),
     PORT_OF_CASCADE_TRUNK_LION_A_WITH_OFFSET_MAC(9),
     PORT_OF_CASCADE_TRUNK_LION_A_WITH_OFFSET_MAC(11)};/* ports of cascade trunk - in the same port group -- limitation for Lion A */

/* ports that used with force configuration */
static GT_U8    testPortsFor60PortsConfigForced_cascadeTrunkPortsSamePortGroup[TRUNK_8_PORTS_IN_TEST_CNS] =
    {0,     16,       32,       48 , /* ports not in trunk */
     PORT_OF_CASCADE_TRUNK_LION_A_WITH_OFFSET_MAC(3), /* cascade trunk in Lion A must be with Id that match to the port group of the cascade ports */
     PORT_OF_CASCADE_TRUNK_LION_A_WITH_OFFSET_MAC(4),
     PORT_OF_CASCADE_TRUNK_LION_A_WITH_OFFSET_MAC(9),
     PORT_OF_CASCADE_TRUNK_LION_A_WITH_OFFSET_MAC(11)};/* ports of cascade trunk - in the same port group -- limitation for Lion A */


static GT_U8    testPortsFor28Ports[TRUNK_8_PORTS_IN_TEST_CNS] =
    {0,        8,         18,       23 , /* ports not in trunk */
     2 ,      19,         21,       27 };/* ports of trunk */

static GT_U8    crcTestPortsFor28Ports[TRUNK_8_PORTS_IN_TEST_CNS] =
    {0,        8,         17 /*18*/,       23 , /* ports not in trunk, don't use port 18 because it get same (CRC_hash % 4) as port 0 */
     2 ,      19,         21,       27 };/* ports of trunk */

static GT_U8    first4PortsOrig[TRUNK_8_PORTS_IN_TEST_CNS];
/*
enum for type of tests
*/
typedef enum{
    TRUNK_TEST_TYPE_CRC_HASH_E,
    TRUNK_TEST_TYPE_CASCADE_TRUNK_E,
    TRUNK_TEST_TYPE_DESIGNATED_TABLE_MODES_E,
    TRUNK_TEST_TYPE_MC_LOCAL_SWITCHING_E

}TRUNK_TEST_TYPE_ENT;


/* number of seeds to test with CRC-16 */
#define SEED_CRC_16_NUM_CNS   6
/* number of seeds to test with CRC-6 */
#define SEED_CRC_6_NUM_CNS    4
/* seeds to test with CRC 6/16 , the last 2 seeds not needed for CRC-6
    since (seed & 0x3f) on those values will give same value as on the previous
    2 seeds */
static GT_U32 crcSeeds[SEED_CRC_16_NUM_CNS] = {0xFFFF , 0xAAAA , 0x5555 , 0 ,0xA5A5 , 0x5A5A};
/******************************* Test packet **********************************/

/******************************* common payload *******************************/

/* Data of packet (long version) */
static GT_U8 prvTgfPacketPayloadLongDataArr[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f
};

/* size of prvTgfPacketPayloadLongDataArr */
#define  PRV_TGF_PACKET_PAYLOAD_LONG_DATA_ARR_LEN_CNS  \
    sizeof(prvTgfPacketPayloadLongDataArr)

/* Payload part (long version) */
static TGF_PACKET_PAYLOAD_STC prvTgfPacketLongPayloadPart =
{
    PRV_TGF_PACKET_PAYLOAD_LONG_DATA_ARR_LEN_CNS, /* dataLength */
    prvTgfPacketPayloadLongDataArr          /* dataPtr */
};


/* copy of prvTgfPacketL2Part for restore purposes */
static TGF_PACKET_L2_STC prvTgfPacketL2PartOrig;


/* common L2 traffic for all packets of prvTgfTrunkTrafficTypes[] */
static TGF_PACKET_L2_STC prvTgfPacketL2Part =
{
    {0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0xee},                /* daMac */
    {0x00, 0xbb, 0xcc, 0xdd, 0xee, 0xff}                 /* saMac */
};

/* 'Remote device' number */
#define REMOTE_DEVICE_NUMBER_CNS       7

/* default VLAN0 Id */
#define PRV_TGF_VLANID_CNS  5

#define MAX_VLANS_CNS    5

/* vlans array */
static GT_U16   prvTgfVlansArr[MAX_VLANS_CNS]={1 , 3, 555, 1234, 4094};
static GT_U32   prvTgfVlansNum = 2;/* number of vlan used in test */

/* copy of prvTgfPacketVlanTagPart for restore purposes */
static TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPartOrig;

/* First VLAN_TAG part */
TGF_PACKET_VLAN_TAG_STC prvTgfPacketVlanTagPart = {
    TGF_ETHERTYPE_8100_VLAN_TAG_CNS,                    /* etherType */
    0, 0, PRV_TGF_VLANID_CNS                            /* pri, cfi, VlanId */
};

/******************************* MPLS packet **********************************/

/* MAC_DA[5:0] == MAC_SA[5:0] to cause         */
/* MACTrunkHash = MAC_SA[5:0]^MAC_DA[5:0] == 0 */
static TGF_PACKET_L2_STC prvTgfPacketMplsL2Part =
{
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x00},                /* daMac */
    {0x00, 0x66, 0x77, 0x88, 0x99, 0x00}                 /* saMac */
};

/* ethertype part of packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketMplsEtherTypePart =
{TGF_ETHERTYPE_8847_MPLS_TAG_CNS};

/* packet's MPLS */
static TGF_PACKET_MPLS_STC prvTgfPacketMplsPart =
{
    1,                  /* label */
    0,                  /* experimental use */
    1,                  /* stack --> end of stack (last bit) */
    0x40                /* timeToLive */
};

/* PARTS of packet MPLS */
static TGF_PACKET_PART_STC prvTgfMplsPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketMplsL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketMplsEtherTypePart},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMplsPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* packet's MPLS */
static TGF_PACKET_MPLS_STC prvTgfPacketMplsLable1Part =
{
    1,                  /* label */
    0,                  /* experimental use */
    0,                  /* stack --> NOT end of stack (last bit = 0) */
    0x15                /* timeToLive */
};
/* packet's MPLS */
static TGF_PACKET_MPLS_STC prvTgfPacketMplsLable2Part =
{
    2,                  /* label */
    0,                  /* experimental use */
    0,                  /* stack --> NOT end of stack (last bit = 0) */
    0x16                /* timeToLive */
};
/* packet's MPLS */
static TGF_PACKET_MPLS_STC prvTgfPacketMplsLable3Part =
{
    3,                  /* label */
    0,                  /* experimental use */
    1,                  /* stack --> end of stack (last bit) */
    0x17                /* timeToLive */
};

/* PARTS of packet MPLS -- 3 labels */
static TGF_PACKET_PART_STC prvTgfMpls3LabelsPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketMplsEtherTypePart},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMplsLable1Part},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMplsLable2Part},
    {TGF_PACKET_PART_MPLS_E,      &prvTgfPacketMplsLable3Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};


/* Length of MPLS packet */
#define PRV_TGF_MPLS_PACKET_LEN_CNS(numLables) \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS \
    + (numLables*TGF_MPLS_HEADER_SIZE_CNS) + PRV_TGF_PACKET_PAYLOAD_LONG_DATA_ARR_LEN_CNS + TGF_CRC_LEN_CNS

/* MPLS packet to send */
static TGF_PACKET_STC prvTgfMplsPacketInfo =
{
    PRV_TGF_MPLS_PACKET_LEN_CNS(1),                 /* totalLen */
    (sizeof(prvTgfMplsPacketPartArray)
        / sizeof(prvTgfMplsPacketPartArray[0])), /* numOfParts */
    prvTgfMplsPacketPartArray                    /* partsArray */
};

/* MPLS packet to send - 3 labels */
static TGF_PACKET_STC prvTgfMpls3LablesPacketInfo =
{
    PRV_TGF_MPLS_PACKET_LEN_CNS(3) + TGF_VLAN_TAG_SIZE_CNS,                 /* totalLen */
    (sizeof(prvTgfMpls3LabelsPacketPartArray)
        / sizeof(prvTgfMpls3LabelsPacketPartArray[0])), /* numOfParts */
    prvTgfMpls3LabelsPacketPartArray                    /* partsArray */
};

/******************************* IPv4 packet **********************************/

/* MACTrunkHash = MAC_SA[5:0]^MAC_DA[5:0] == 0 */
static TGF_PACKET_L2_STC prvTgfPacketIpShiftL2Part =
{
    {0x00, 0x11, 0x22, 0x33, 0x34, 0x00},                /* daMac */
    {0x00, 0x66, 0x77, 0x88, 0x34, 0x00}                 /* saMac */
};

/* ethertype part of IPV4 packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketIpv4EtherTypePart =
{TGF_ETHERTYPE_0800_IPV4_TAG_CNS};

/* packet's IPv4 */
/* SIP[5:0]==1, DIP[5:1]==0 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4OtherPart =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x2E,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    4,                  /* protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS, /* csum */
    { 7,  0,  8,   0},    /* srcAddr */
    { 1,  1,  1,   0}    /* dstAddr */
};

/* PARTS of packet IPv4 - IPV4 OTHER */
static TGF_PACKET_PART_STC prvTgfIpv4OtherPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketIpShiftL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4OtherPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* Length of IPv4 packet */
#define PRV_TGF_IPV4_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS \
    + TGF_IPV4_HEADER_SIZE_CNS + PRV_TGF_PACKET_PAYLOAD_LONG_DATA_ARR_LEN_CNS+ TGF_CRC_LEN_CNS

/* IPv4 Other packet to send */
static TGF_PACKET_STC prvTgfIpv4OtherPacketInfo =
{
    PRV_TGF_IPV4_PACKET_LEN_CNS,                      /* totalLen */
    (sizeof(prvTgfIpv4OtherPacketPartArray)
        / sizeof(prvTgfIpv4OtherPacketPartArray[0])), /* numOfParts */
    prvTgfIpv4OtherPacketPartArray                    /* partsArray */
};

/******************************* ETH_OTHER packet *****************************/
/* MACTrunkHash = MAC_SA[5:0]^MAC_DA[5:0] == 1 */
static TGF_PACKET_L2_STC prvTgfPacketMaskL2Part =
{
    {0x00, 0x11, 0x22, 0x33, 0x34, 0x00},                /* daMac */
    {0x00, 0x66, 0x77, 0x88, 0x34, 0x01}                 /* saMac */
};

/* ethertype part of packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEthOtherEtherTypePart = {0x1234};

/* PARTS of packet ETHERNET OTHER */
static TGF_PACKET_PART_STC prvTgfEthOtherPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketMaskL2Part},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEthOtherEtherTypePart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* Length of packet */
#define PRV_TGF_PACKET_ETH_OTHER_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS \
    + PRV_TGF_PACKET_PAYLOAD_LONG_DATA_ARR_LEN_CNS+ TGF_CRC_LEN_CNS

/* packet to send */
static TGF_PACKET_STC prvTgfPacketEthOtherInfo =
{
    PRV_TGF_PACKET_ETH_OTHER_LEN_CNS,                                             /* totalLen */
    sizeof(prvTgfEthOtherPacketPartArray) / sizeof(prvTgfEthOtherPacketPartArray[0]), /* numOfParts */
    prvTgfEthOtherPacketPartArray                                                     /* partsArray */
};

/*c.Send unknown unicast (00:09:99:99:99:99) with 100 incremental DA and static SA */
/*d.Send unknown unicast (00:09:99:99:99:99) with 100 static DA and incremental SA*/
static TGF_PACKET_L2_STC multiDestination_prvTgfPacketL2Part_unk_UC =
{
    {0x00, 0x09, 0x99, 0x99, 0x99, 0x99},                /* daMac */
    {0x00, 0x01, 0x11, 0x11, 0x11, 0x11}                 /* saMac */
};

/******************************* simple vlan tagged packet **********************************/
/* port bitmap VLAN members */
static CPSS_PORTS_BMP_STC localPortsVlanMembers = {{0, 0}};

/* PARTS of packet */
TGF_PACKET_PART_STC prvTgfPacketVlanTagPartArray[] = {
    {TGF_PACKET_PART_L2_E,       &multiDestination_prvTgfPacketL2Part_unk_UC},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E, &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_PAYLOAD_E,  &prvTgfPacketLongPayloadPart}
};
/* Length of MPLS packet */
#define PRV_TGF_VLAN_TAGGED_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + \
    PRV_TGF_PACKET_PAYLOAD_LONG_DATA_ARR_LEN_CNS+ TGF_CRC_LEN_CNS

/* PACKET to send */
TGF_PACKET_STC prvTgfPacketVlanTagInfo = {
    PRV_TGF_VLAN_TAGGED_PACKET_LEN_CNS,                                 /* totalLen */
    sizeof(prvTgfPacketVlanTagPartArray) / sizeof(TGF_PACKET_PART_STC), /* numOfParts */
    prvTgfPacketVlanTagPartArray                                        /* partsArray */
};


/***********************************************/
/* traffic types */

/* packet's ethertype */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketEtherTypePart_Ipv6 = {
    TGF_ETHERTYPE_86DD_IPV6_TAG_CNS
};
/* packet's IPv6 */
static TGF_PACKET_IPV6_STC prvTgfPacketIpv6Part = {
    6,                  /* version */
    0,                  /* trafficClass */
    0,                  /* flowLabel */
    0x02,               /* payloadLen */
    0x3b,               /* nextHeader */
    0x40,               /* hopLimit */
    {0x6545, 0, 0, 0, 0, 0, 0x0000, 0x3212}, /* TGF_IPV6_ADDR srcAddr */
    {0xff02, 0, 0, 0, 0, 0, 0xccdd, 0xeeff}  /* TGF_IPV6_ADDR dstAddr */
};

/* PARTS of packet */
static TGF_PACKET_PART_STC prvTgfIpv6PacketArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},  /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEtherTypePart_Ipv6},
    {TGF_PACKET_PART_IPV6_E,      &prvTgfPacketIpv6Part},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* LENGTH of ipv6 packet */
#define PRV_TGF_IPV6_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS \
    + TGF_IPV6_HEADER_SIZE_CNS + PRV_TGF_PACKET_PAYLOAD_LONG_DATA_ARR_LEN_CNS+ TGF_CRC_LEN_CNS

/* PACKET1 to send */
static TGF_PACKET_STC prvTgfPacketIpv6Info = {
    PRV_TGF_IPV6_PACKET_LEN_CNS,                                      /* totalLen */
    sizeof(prvTgfIpv6PacketArray) / sizeof(prvTgfIpv6PacketArray[0]), /* numOfParts */
    prvTgfIpv6PacketArray                                        /* partsArray */
};

/* packet's TCP part */
static TGF_PACKET_TCP_STC prvTgfPacketTcpPart =
{
    1,                  /* src port */
    2,                  /* dst port */
    1,                  /* sequence number */
    2,                  /* acknowledgment number */
    0,                  /* data offset */
    0,                  /* reserved */
    0x00,               /* flags */
    4096,               /* window */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS,/* csum */
    0                   /* urgent pointer */
};


/* packet's IPv4 part */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4TcpPart =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x36,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    TGF_PROTOCOL_TCP_E, /* TCP -- protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS,/* csum - need calc */
    {22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
};

/* PARTS of packet IPv4 - TCP */
static TGF_PACKET_PART_STC prvTgfIpv4TcpPacketArray[] = {
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part}, /* type, partPtr */
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4TcpPart},
    {TGF_PACKET_PART_TCP_E,       &prvTgfPacketTcpPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* LENGTH of ipv4 TCP packet */
#define PRV_TGF_IPV4_TCP_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS \
    + TGF_IPV4_HEADER_SIZE_CNS + TGF_TCP_HEADER_SIZE_CNS + PRV_TGF_PACKET_PAYLOAD_LONG_DATA_ARR_LEN_CNS

/* ipv4 TCP to send */
static TGF_PACKET_STC prvTgfIpv4TcpPacketInfo = {
    PRV_TGF_IPV4_TCP_PACKET_LEN_CNS,                      /* totalLen */
    sizeof(prvTgfIpv4TcpPacketArray) / sizeof(prvTgfIpv4TcpPacketArray[0]), /* numOfParts */
    prvTgfIpv4TcpPacketArray                                        /* partsArray */
};


/* packet's UDP part */
static TGF_PACKET_UDP_STC prvTgfPacketUdpPart =
{
    8,                  /* src port */
    0,                  /* dst port */
    TGF_PACKET_AUTO_CALC_LENGTH_CNS,  /* length */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS /* csum */
};

/* packet's IPv4 part */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4UdpPart =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x36,               /* totalLen */
    0,                  /* id */
    0,                  /* flags */
    0,                  /* offset */
    0x40,               /* timeToLive */
    TGF_PROTOCOL_UDP_E, /* protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS, /* csum */
    {22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
};

/* PARTS of packet UDP */
static TGF_PACKET_PART_STC prvTgfIpv4UdpPacketArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4UdpPart},
    {TGF_PACKET_PART_UDP_E,       &prvTgfPacketUdpPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* LENGTH of ipv4 UDP packet */
#define PRV_TGF_IPV4_UDP_PACKET_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS \
    + TGF_IPV4_HEADER_SIZE_CNS + TGF_UDP_HEADER_SIZE_CNS + PRV_TGF_PACKET_PAYLOAD_LONG_DATA_ARR_LEN_CNS

/* ipv4 UDP to send */
static TGF_PACKET_STC prvTgfIpv4UdpPacketInfo = {
    PRV_TGF_IPV4_UDP_PACKET_LEN_CNS,                      /* totalLen */
    sizeof(prvTgfIpv4UdpPacketArray) / sizeof(prvTgfIpv4UdpPacketArray[0]), /* numOfParts */
    prvTgfIpv4UdpPacketArray                                        /* partsArray */
};

/* packet's IPv4 */
static TGF_PACKET_IPV4_STC prvTgfPacketIpv4FragmentPart =
{
    4,                  /* version */
    5,                  /* headerLen */
    0,                  /* typeOfService */
    0x2E,               /* totalLen */
    0,                  /* id */
    1,                  /* flags */    /* more fragments     */
    0x200,              /* offset */   /* not first fragment */
    0x40,               /* timeToLive */
    4,                  /* protocol */
    TGF_PACKET_AUTO_CALC_CHECKSUM_CNS, /* csum */
    {22, 22, 22, 22},   /* srcAddr */
    { 1,  1,  1,  3}    /* dstAddr */
};

/* PARTS of packet IPv4 - IPV4 FRAGMENT */
static TGF_PACKET_PART_STC prvTgfIpv4FragmentPacketPartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4FragmentPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* IPv4 Fragment packet to send */
static TGF_PACKET_STC prvTgfIpv4FragmentPacketInfo =
{
    PRV_TGF_IPV4_PACKET_LEN_CNS,                         /* totalLen */
    (sizeof(prvTgfIpv4FragmentPacketPartArray)
        / sizeof(prvTgfIpv4FragmentPacketPartArray[0])), /* numOfParts */
    prvTgfIpv4FragmentPacketPartArray                    /* partsArray */
};


/* PARTS of packet IPv4 - IPV4 OTHER */
static TGF_PACKET_PART_STC prvTgfIpv4OtherPacketPart1Array[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketIpv4EtherTypePart},
    {TGF_PACKET_PART_IPV4_E,      &prvTgfPacketIpv4OtherPart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* IPv4 Other packet to send */
static TGF_PACKET_STC prvTgfIpv4OtherPacket1Info =
{
    PRV_TGF_IPV4_PACKET_LEN_CNS,                      /* totalLen */
    (sizeof(prvTgfIpv4OtherPacketPart1Array)
        / sizeof(prvTgfIpv4OtherPacketPart1Array[0])), /* numOfParts */
    prvTgfIpv4OtherPacketPart1Array                    /* partsArray */
};




/* PARTS of packet ETHERNET OTHER */
static TGF_PACKET_PART_STC prvTgfEthOtherPacketPart1Array[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketEthOtherEtherTypePart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* packet to send */
static TGF_PACKET_STC prvTgfPacketEthOther1Info =
{
    PRV_TGF_PACKET_ETH_OTHER_LEN_CNS,                                             /* totalLen */
    sizeof(prvTgfEthOtherPacketPart1Array) / sizeof(prvTgfEthOtherPacketPart1Array[0]), /* numOfParts */
    prvTgfEthOtherPacketPart1Array                                                     /* partsArray */
};

/* number of UDEs */
#define NUM_UDE_CNS     5
/* use this ether type as base value for the 5 UDEs */
#define UDE_BASE_CNS    0x5678

/* ether type part of packet */
static TGF_PACKET_ETHERTYPE_STC prvTgfPacketUdeTypePart = {UDE_BASE_CNS};

/* PARTS of packet ETHERNET OTHER */
static TGF_PACKET_PART_STC prvTgfEthUdePartArray[] =
{
    {TGF_PACKET_PART_L2_E,        &prvTgfPacketL2Part},
    {TGF_PACKET_PART_VLAN_TAG_E,  &prvTgfPacketVlanTagPart},
    {TGF_PACKET_PART_ETHERTYPE_E, &prvTgfPacketUdeTypePart},
    {TGF_PACKET_PART_PAYLOAD_E,   &prvTgfPacketLongPayloadPart}
};

/* Length of packet */
#define PRV_TGF_PACKET_UDE_LEN_CNS \
    TGF_L2_HEADER_SIZE_CNS + TGF_ETHERTYPE_SIZE_CNS + TGF_VLAN_TAG_SIZE_CNS \
    + PRV_TGF_PACKET_PAYLOAD_LONG_DATA_ARR_LEN_CNS

/* packet to send */
static TGF_PACKET_STC prvTgfPacketUdeInfo =
{
    PRV_TGF_PACKET_UDE_LEN_CNS,                                             /* totalLen */
    sizeof(prvTgfEthUdePartArray) / sizeof(prvTgfEthUdePartArray[0]), /* numOfParts */
    prvTgfEthUdePartArray                                                     /* partsArray */
};

/* array of traffic types */
static TGF_PACKET_STC *prvTgfTrunkTrafficTypes[PRV_TGF_PCL_PACKET_TYPE_LAST_E] =
{
     &prvTgfIpv4TcpPacketInfo/* ipv4 - TCP */
    ,&prvTgfIpv4UdpPacketInfo/* ipv4 - UDP */
    ,&prvTgfMpls3LablesPacketInfo/* mpls */
    ,&prvTgfIpv4FragmentPacketInfo/* ipv4 - fragment */
    ,&prvTgfIpv4OtherPacket1Info/* ipv4 - other */
    ,&prvTgfPacketEthOther1Info/* ethernet */
    ,&prvTgfPacketUdeInfo/* ude 0 */
    ,&prvTgfPacketIpv6Info/* ipv6 */
    ,&prvTgfPacketUdeInfo/* ude 1 */
    ,&prvTgfPacketUdeInfo/* ude 2 */
    ,&prvTgfPacketUdeInfo/* ude 3 */
    ,&prvTgfPacketUdeInfo/* ude 4 */
};

/* Pearson hash modes for filling the table with values */
typedef enum{
    PEARSON_TEST_MODE_INDEX_E,          /*value = index (0..63)*/
    PEARSON_TEST_MODE_REVERSE_INDEX_E,  /*value = 63 - index (63..0)*/
    PEARSON_TEST_MODE_START_MID_INDEX_E,/*value = (32 + index) % 64 (32..63,0..31)*/
    PEARSON_TEST_MODE_EVEN_INDEX_E,     /*value = index & 0x3e (0,0,2,2..62,62)*/
    PEARSON_TEST_MODE_MODE_8_INDEX_E,   /*value = index & 0x7 (0..7,0..7,0..7,...,0..7)*/

    PEARSON_TEST_MODE_0_E,              /*value = 0 (all indexes) */

    PEARSON_TEST_MODE_LAST_E

}PEARSON_TEST_MODE_ENT;

/* get string for Pearson mode */
#define TO_STRING_PEARSONE_MODE_MAC(_pearsonMode)    \
        (                                        \
        _pearsonMode ==    PEARSON_TEST_MODE_INDEX_E               ? "INDEX"           :  \
        _pearsonMode ==    PEARSON_TEST_MODE_REVERSE_INDEX_E       ? "REVERSE_INDEX"   :  \
        _pearsonMode ==    PEARSON_TEST_MODE_START_MID_INDEX_E     ? "START_MID_INDEX" :  \
        _pearsonMode ==    PEARSON_TEST_MODE_EVEN_INDEX_E          ? "EVEN_INDEX"      :  \
        _pearsonMode ==    PEARSON_TEST_MODE_MODE_8_INDEX_E        ? "MODE_8_INDEX"    :  \
        _pearsonMode ==    PEARSON_TEST_MODE_0_E                   ? "MODE_0"          :  \
                                                                              "???")

/* number of traffic types sent by prvTgfCommonMultiDestinationTrafficTypeSend(...) */
#define MULTI_DEST_COUNT_MAC    4
/*
 * Typedef: enum PRV_TGF_TRUNK_HASH_TEST_PACKET_ENT
 *
 * Description: enumerator packets used in trunk hash tests
 *
 * Fields:
 *      PRV_TGF_TRUNK_HASH_TEST_PACKET_ETH_MAC_SA_E  - ETH_OTHER packet,
 *                     MAC_DA{5:0]==0, MAC_SA[5:0]==1
 *      PRV_TGF_TRUNK_HASH_TEST_PACKET_IPV4_SIP_E    - IPV4 packet,
 *                     DIP[5:0]==0, SIP[5:0]==1
 *      PRV_TGF_TRUNK_HASH_TEST_PACKET_MPLS_LAB0_E   - MPLS packet
 *                     MAC_DA{5:0]==0, MAC_SA[5:0]==0, LABEL0[5:0]==1
 */
typedef enum
{
    PRV_TGF_TRUNK_HASH_TEST_PACKET_ETH_MAC_SA_E,
    PRV_TGF_TRUNK_HASH_TEST_PACKET_IPV4_SIP_E,
    PRV_TGF_TRUNK_HASH_TEST_PACKET_MPLS_LAB0_E
} PRV_TGF_TRUNK_HASH_TEST_PACKET_ENT;

/* if flag don't match the tested field then we don't need to run traffic ... not relevant */
#define RETURN_IF_FIELD_NOT_RELEVANT_MAC(flag) \
             if(flag == GT_FALSE)              \
             {                                 \
                if(debugMode)                  \
                {                              \
                    cpssOsPrintf(" debug ERROR ---> value is 0 for [%s] --> field not relevant \n",#flag);\
                }                              \
                return GT_OK;                  \
             }

/* the ipv4 is 'Shifted 12 bytes in the Ipv6 field' */
#define IPV4_OFFSET_FROM_BYTE_12_CNS     12

/******************************************************************************\
 *                           Private declarations                             *
\******************************************************************************/

/* stored default hash mode */
static PRV_TGF_TRUNK_LBH_GLOBAL_MODE_ENT prvTgfDefHashMode = PRV_TGF_TRUNK_LBH_PACKETS_INFO_E;


typedef enum{
    PRV_TGF_TRUNK_DESIGNATED_PORTS_BASIC_E,
    PRV_TGF_TRUNK_DESIGNATED_PORTS_SET_DESIGNATED_AFTER_TRUNK_SETTING_ENT,
    PRV_TGF_TRUNK_DESIGNATED_PORTS_ADD_PORTS_INSTEAD_OF_SET_ENT
}PRV_TGF_TRUNK_DESIGNATED_PORTS_MUTATION_ENT;
static PRV_TGF_TRUNK_DESIGNATED_PORTS_MUTATION_ENT designatedPortsMutation = PRV_TGF_TRUNK_DESIGNATED_PORTS_BASIC_E;
static GT_BOOL designatedPort4AsDisabled = GT_FALSE;

static GT_U32   debug_burstNum = 0;

static GT_U32   debugMode = 0;
static GT_U32   debug_hashMaskTableIndex        = PRV_TGF_TRUNK_HASH_MASK_TABLE_TRAFFIC_TYPE_FIRST_INDEX_CNS;
static GT_U32   debug_hashMaskFieldType         = PRV_TGF_TRUNK_FIELD_MAC_ADDR_E;
static GT_U32   debug_hashMaskFieldSubType      = PRV_TGF_TRUNK_FIELD_DIRECTION_SOURCE_E;
static GT_U32   debug_hashMaskFieldByteIndex    = 5;
static GT_U32   debug_pearsonMode               = PEARSON_TEST_MODE_INDEX_E;
static GT_U32   debug_crcMode                   = PRV_TGF_TRUNK_LBH_CRC_6_MODE_E;
static GT_U32   debug_seedIndex                 = 0;

static GT_U32   debug_desigMode                 = PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E;
static GT_U32   debug_lbhGenerated              = 0;


/* if debug mode break */
#define IF_DEBUG_MODE_BREAK_MAC(debugMode) \
    if(debugMode) break


/******************************************************************************\
 *                            Private test functions                          *
\******************************************************************************/

/*******************************************************************************
* prvTgfTrunkForHashTestConfigure
*
* DESCRIPTION:
*       This function configures trunk of two ports
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkForHashTestConfigure
(
    GT_VOID
)
{
    CPSS_TRUNK_MEMBER_STC          enabledMembersArray[2];
    GT_STATUS                      rc;
    GT_TRUNK_ID             trunkId;

    enabledMembersArray[0].device = prvTgfDevsArray[PRV_TGF_TRUNK_PORT0_INDEX_CNS];
    enabledMembersArray[0].port   = prvTgfPortsArray[PRV_TGF_TRUNK_PORT0_INDEX_CNS];
    enabledMembersArray[1].device = prvTgfDevsArray[PRV_TGF_TRUNK_PORT1_INDEX_CNS];
    enabledMembersArray[1].port   = prvTgfPortsArray[PRV_TGF_TRUNK_PORT1_INDEX_CNS];

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    rc = prvTgfTrunkMembersSet(
        trunkId /*trunkId*/,
        2 /*numOfEnabledMembers*/,
        enabledMembersArray,
        0 /*numOfDisabledMembers*/,
        (CPSS_TRUNK_MEMBER_STC*)NULL  /*disabledMembersArray*/);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMembersSet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* save default hashMode */
    rc = prvTgfTrunkHashGlobalModeGet(prvTgfDevNum, &prvTgfDefHashMode);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkHashGlobalModeGet FAILED, rc = [%d]", rc);
        return rc;
    }

    rc = prvTgfTrunkHashGlobalModeSet(
        PRV_TGF_TRUNK_LBH_PACKETS_INFO_E);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkHashGlobalModeSet FAILED, rc = [%d]", rc);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrunkForHashTestConfigurationReset
*
* DESCRIPTION:
*       This function resets configuration of trunk
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkForHashTestConfigurationReset
(
    GT_VOID
)
{
    GT_STATUS   rc;
    GT_TRUNK_ID             trunkId;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    rc = prvTgfTrunkMembersSet(
        trunkId /*trunkId*/,
        0 /*numOfEnabledMembers*/,
        (CPSS_TRUNK_MEMBER_STC*)NULL /*enabledMembersArray*/,
        0 /*numOfDisabledMembers*/,
        (CPSS_TRUNK_MEMBER_STC*)NULL  /*disabledMembersArray*/);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMembersSet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* restore default hashMode */
    rc = prvTgfTrunkHashGlobalModeSet(prvTgfDefHashMode);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkHashGlobalModeSet FAILED, rc = [%d]", rc);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrunkForHashTestSendPacketAndCheck
*
* DESCRIPTION:
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets from both trunk ports
*
* INPUTS:
*       packetInfoPtr           - (pointer to) the packet info
*       burstCount              -  burst size
*       expectedEgressFromPort0 - amount of packets expected to egress from
*                                 thunk member 0
*       expectedEgressFromPort1 - amount of packets expected to egress from
*                                 thunk member 1
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
static GT_VOID prvTgfTrunkForHashTestSendPacketAndCheck
(
    IN TGF_PACKET_STC   *packetInfoPtr,
    IN GT_U32            burstCount,
    IN GT_U32            expectedEgressFromPort0,
    IN GT_U32            expectedEgressFromPort1
)
{
    GT_STATUS                       rc           = GT_OK;
    GT_U32                          portIter     = 0;
    GT_BOOL                         isEqualCntrs = GT_FALSE;
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   expectedCntrs;
    GT_U32                          portNums[3] =
        {
            PRV_TGF_SEND_PORT_INDEX_CNS,
            PRV_TGF_TRUNK_PORT0_INDEX_CNS,
            PRV_TGF_TRUNK_PORT1_INDEX_CNS
        };
    GT_U32                          expectedPackets[3];

    TRUNK_PRV_UTF_LOG0_MAC("======= Reset Counters =======\n");

    expectedPackets[0] = burstCount;
    expectedPackets[1] = expectedEgressFromPort0;
    expectedPackets[2] = expectedEgressFromPort1;

    for (portIter = 0; (portIter < 3); portIter++)
    {
        /* reset counters */
        rc = prvTgfResetCountersEth(
            prvTgfDevsArray[portNums[portIter]],
            prvTgfPortsArray[portNums[portIter]]);
        UTF_VERIFY_EQUAL2_STRING_MAC(
            GT_OK, rc, "prvTgfResetCountersEth: %d, %d\n",
            prvTgfDevsArray[portNums[portIter]],
            prvTgfPortsArray[portNums[portIter]]);
    }

    TRUNK_PRV_UTF_LOG0_MAC("======= Generating Traffic =======\n");

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(
        prvTgfDevsArray[PRV_TGF_SEND_PORT_INDEX_CNS],
        packetInfoPtr, burstCount, 0, NULL);
    UTF_VERIFY_EQUAL4_STRING_MAC(
        GT_OK, rc, "prvTgfSetTxSetupEth: %d, %d",
        prvTgfDevsArray[PRV_TGF_SEND_PORT_INDEX_CNS], burstCount, 0, 0);

    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* send Packet from port 8 */
    rc = prvTgfStartTransmitingEth(
        prvTgfDevsArray[PRV_TGF_SEND_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_SEND_PORT_INDEX_CNS]);
    UTF_VERIFY_EQUAL2_STRING_MAC(
        GT_OK, rc, "prvTgfStartTransmitingEth: %d %d",
        prvTgfDevsArray[PRV_TGF_SEND_PORT_INDEX_CNS],
        prvTgfPortsArray[PRV_TGF_SEND_PORT_INDEX_CNS]);

    /* Delay 100 milliseconds - give to sent packet to pass all pilelines in PP */
    cpssOsTimerWkAfter(100);


    /* read and check ethernet counters */
    for (portIter = 0; (portIter < 3); portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(
            prvTgfDevsArray[portNums[portIter]],
            prvTgfPortsArray[portNums[portIter]],
            GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(
            GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d\n",
            prvTgfDevsArray[portNums[portIter]],
            prvTgfPortsArray[portNums[portIter]]);

        /* copy read counters to  expected counters */
        cpssOsMemCpy(&expectedCntrs, &portCntrs, sizeof(expectedCntrs));

        /* calculate expected Tx counters */
        if (PRV_TGF_SEND_PORT_INDEX_CNS == portNums[portIter])
        {
            expectedCntrs.goodPktsRcv.l[0]   = expectedPackets[portIter];
        }
        else
        {
            /* calculate expected Rx counters */
            expectedCntrs.goodPktsSent.l[0]   = expectedPackets[portIter];
        }

        /* check Rx\Tx counters */
        PRV_TGF_VERIFY_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
        UTF_VERIFY_EQUAL0_STRING_MAC(isEqualCntrs, GT_TRUE, "get another counters values.");

        /* print expected values if not equal */
        PRV_TGF_PRINT_DIFF_COUNTERS_MAC(isEqualCntrs,expectedCntrs,portCntrs);
    }
}


/*******************************************************************************
* prvTgfTrunkForHashTestTrafficGenerateAndCheck
*
* DESCRIPTION:
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets from both trunk ports
*
* INPUTS:
*       testPacketType          - the packet type
*       burstCount              -  burst size
*       expectedEgressFromPort0 - amount of packets expected to egress from
*                                 thunk member 0
*       expectedEgressFromPort1 - amount of packets expected to egress from
*                                 thunk member 1
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
static GT_VOID prvTgfTrunkForHashTestTrafficGenerateAndCheck
(
    IN PRV_TGF_TRUNK_HASH_TEST_PACKET_ENT testPacketType,
    IN GT_U32                             burstCount,
    IN GT_U32                             expectedEgressFromPort0,
    IN GT_U32                             expectedEgressFromPort1
)
{
    TGF_PACKET_STC            *packetInfoPtr;
    GT_STATUS                 rc;
    PRV_TGF_BRG_MAC_ENTRY_STC brgMacEntry;
    GT_U32                    i;
    TGF_PACKET_L2_STC         *packetL2Ptr;

    switch (testPacketType)
    {
        case PRV_TGF_TRUNK_HASH_TEST_PACKET_ETH_MAC_SA_E:
            packetInfoPtr = &prvTgfPacketEthOtherInfo;
            break;
        case PRV_TGF_TRUNK_HASH_TEST_PACKET_IPV4_SIP_E:
            packetInfoPtr = &prvTgfIpv4OtherPacketInfo;
            break;
        case PRV_TGF_TRUNK_HASH_TEST_PACKET_MPLS_LAB0_E:
            packetInfoPtr = &prvTgfMplsPacketInfo;
            break;
        default:
            TRUNK_PRV_UTF_LOG0_MAC(
                "prvTgfTrunkForHashTestTrafficGenerateAndCheck bad pktType\n");
            return;
    }


    /* search MAC addresses */
    for (i = 0; (i < packetInfoPtr->numOfParts); i++)
    {
        if (packetInfoPtr->partsArray[i].type == TGF_PACKET_PART_L2_E)
        {
            break;
        }
    }
    UTF_VERIFY_NOT_EQUAL0_STRING_MAC(
        packetInfoPtr->numOfParts, i, "MAC_SA not found in packet info\n");

    packetL2Ptr = (TGF_PACKET_L2_STC*)packetInfoPtr->partsArray[i].partPtr;

    cpssOsMemSet(&brgMacEntry, 0, sizeof(brgMacEntry));
    brgMacEntry.key.entryType = PRV_TGF_FDB_ENTRY_TYPE_MAC_ADDR_E;
    for (i = 0; (i < 6); i++)
    {
        brgMacEntry.key.key.macVlan.macAddr.arEther[i] =
            packetL2Ptr->daMac[i];
    }
    brgMacEntry.key.key.macVlan.vlanId = 1;
    brgMacEntry.daCommand = PRV_TGF_PACKET_CMD_FORWARD_E;
    brgMacEntry.saCommand = PRV_TGF_PACKET_CMD_FORWARD_E;

    brgMacEntry.dstInterface.type    = CPSS_INTERFACE_TRUNK_E;
    brgMacEntry.dstInterface.trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(brgMacEntry.dstInterface.trunkId);
    brgMacEntry.dstOutlif.outlifType = PRV_TGF_OUTLIF_TYPE_LL_E;

    brgMacEntry.isStatic = GT_TRUE;

    rc = prvTgfBrgFdbMacEntrySet(&brgMacEntry);
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "prvTgfBrgFdbMacEntrySet: failed\n");

    prvTgfTrunkForHashTestSendPacketAndCheck(
        packetInfoPtr,
        burstCount,
        expectedEgressFromPort0,
        expectedEgressFromPort1);

    rc = prvTgfBrgFdbMacEntryDelete(&(brgMacEntry.key));
    UTF_VERIFY_EQUAL0_STRING_MAC(
        GT_OK, rc, "prvTgfBrgFdbMacEntryDelete: failed\n");

}


/*******************************************************************************
* prvTgfTrunkHashTestTrafficMplsModeEnableSet
*
* DESCRIPTION:
*       Test for prvTgfTrunkHashMplsModeEnableSet;
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets from both trunk ports
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
GT_VOID prvTgfTrunkHashTestTrafficMplsModeEnableSet
(
    GT_VOID
)
{
    prvTgfTrunkHashMplsModeEnableSet(GT_TRUE);

    /* Generate traffic */
    prvTgfTrunkForHashTestTrafficGenerateAndCheck(
        PRV_TGF_TRUNK_HASH_TEST_PACKET_MPLS_LAB0_E,
        2 /*burstCount*/,
        0 /*expectedEgressFromPort0*/,
        2 /*expectedEgressFromPort1*/);

    prvTgfTrunkHashMplsModeEnableSet(GT_FALSE);

    /* Generate traffic */
    prvTgfTrunkForHashTestTrafficGenerateAndCheck(
        PRV_TGF_TRUNK_HASH_TEST_PACKET_MPLS_LAB0_E,
        3 /*burstCount*/,
        3 /*expectedEgressFromPort0*/,
        0 /*expectedEgressFromPort1*/);
}


/*******************************************************************************
* prvTgfTrunkHashTestTrafficMaskSet
*
* DESCRIPTION:
*       Test for prvTgfTrunkHashMaskSet;
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets from both trunk ports
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
GT_VOID prvTgfTrunkHashTestTrafficMaskSet
(
    GT_VOID
)
{
    prvTgfTrunkHashMaskSet(
        PRV_TGF_TRUNK_LBH_MASK_MAC_SA_E, 0);

    /* Generate traffic */
    prvTgfTrunkForHashTestTrafficGenerateAndCheck(
        PRV_TGF_TRUNK_HASH_TEST_PACKET_ETH_MAC_SA_E,
        3 /*burstCount*/,
        3 /*expectedEgressFromPort0*/,
        0 /*expectedEgressFromPort1*/);

    prvTgfTrunkHashMaskSet(
        PRV_TGF_TRUNK_LBH_MASK_MAC_SA_E, 0x3F);

    /* Generate traffic */
    prvTgfTrunkForHashTestTrafficGenerateAndCheck(
        PRV_TGF_TRUNK_HASH_TEST_PACKET_ETH_MAC_SA_E,
        1 /*burstCount*/,
        0 /*expectedEgressFromPort0*/,
        1 /*expectedEgressFromPort1*/);

}

/*******************************************************************************
* prvTgfTrunkHashTestTrafficIpShiftSet
*
* DESCRIPTION:
*       Test for prvTgfTrunkHashIpShiftSet;
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets from both trunk ports
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
GT_VOID prvTgfTrunkHashTestTrafficIpShiftSet
(
    GT_VOID
)
{
    prvTgfTrunkHashIpModeSet(GT_TRUE);

    prvTgfTrunkHashIpShiftSet(
        CPSS_IP_PROTOCOL_IPV4_E,
        GT_TRUE /*isSrcIp*/, 1 /*shiftValue*/);

    /* Generate traffic */
    prvTgfTrunkForHashTestTrafficGenerateAndCheck(
        PRV_TGF_TRUNK_HASH_TEST_PACKET_IPV4_SIP_E,
        2 /*burstCount*/,
        2 /*expectedEgressFromPort0*/,
        0 /*expectedEgressFromPort1*/);

    prvTgfTrunkHashIpShiftSet(
        CPSS_IP_PROTOCOL_IPV4_E,
        GT_TRUE /*isSrcIp*/, 0 /*shiftValue*/);

    /* Generate traffic */
    prvTgfTrunkForHashTestTrafficGenerateAndCheck(
        PRV_TGF_TRUNK_HASH_TEST_PACKET_IPV4_SIP_E,
        3 /*burstCount*/,
        0 /*expectedEgressFromPort0*/,
        3 /*expectedEgressFromPort1*/);

    prvTgfTrunkHashIpModeSet(GT_FALSE);
}


/*******************************************************************************
* internalTrunkForDesignatedPortsTestConfigurationReset
*
* DESCRIPTION:
*       This function resets configuration of trunk.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internalTrunkForDesignatedPortsTestConfigurationReset
(
    IN  GT_TRUNK_ID             trunkId
)
{
    GT_STATUS   rc;

    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    rc = prvTgfTrunkMembersSet(
        trunkId /*trunkId*/,
        0 /*numOfEnabledMembers*/,
        (CPSS_TRUNK_MEMBER_STC*)NULL /*enabledMembersArray*/,
        0 /*numOfDisabledMembers*/,
        (CPSS_TRUNK_MEMBER_STC*)NULL  /*disabledMembersArray*/);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMembersSet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* restore default 'no designated' */
    rc = prvTgfTrunkDesignatedMemberSet(trunkId,GT_FALSE,NULL);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedMemberSet FAILED, rc = [%d]", rc);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrunkForDesignatedPortsTestConfigurationReset
*
* DESCRIPTION:
*       This function resets configuration of trunk.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkForDesignatedPortsTestConfigurationReset
(
    GT_VOID
)
{
    GT_STATUS   rc;
    rc = internalTrunkForDesignatedPortsTestConfigurationReset(PRV_TGF_TRUNK_ID_CNS);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = internalTrunkForDesignatedPortsTestConfigurationReset(PRV_TGF_TRUNK_ID_2_CNS);
    return rc;
}

/*******************************************************************************
* prvTgfTrunkDesignatedPorts_basicCommonSetting
*
* DESCRIPTION:
Important note for all next tests:
    Start test by setting Trunk2 with Ports p5,p6.
    Those ports not need to be 'link up' or connected to traffic checks.
*
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
static GT_VOID prvTgfTrunkDesignatedPorts_basicCommonSetting
(
    GT_VOID
)
{
    GT_STATUS   rc;
    GT_TRUNK_ID trunkId = PRV_TGF_TRUNK_ID_2_CNS;

    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    trunk_2_Members[0].port   =   16;
    trunk_2_Members[0].device =   prvTgfDevsArray[PRV_TGF_TRUNK_PORT1_INDEX_CNS];

    trunk_2_Members[1].port   =   20;
    trunk_2_Members[1].device =   prvTgfDevsArray[PRV_TGF_TRUNK_PORT1_INDEX_CNS];

    rc = prvTgfTrunkMembersSet(
        trunkId /*trunkId*/,
        2 /*numOfEnabledMembers*/,
        trunk_2_Members,
        0 /*numOfDisabledMembers*/,
        NULL  /*disabledMembersArray*/);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMembersSet FAILED, rc = [%d]", rc);
        return ;
    }


    /* define members in trunk 1 */
    trunk_1_Members[0].device = prvTgfDevsArray[2];
    trunk_1_Members[0].port   = prvTgfPortsArray[2];
    trunk_1_Members[1].device = prvTgfDevsArray[3];
    trunk_1_Members[1].port   = prvTgfPortsArray[3];

    /*define designated ports*/
    designatedMember3 = trunk_1_Members[0];
    designatedMember4 = trunk_1_Members[1];

}

/*******************************************************************************
* prvTgfTrunkDesignatedPorts_trunk2ConfigCheck
*
* DESCRIPTION:
Important note for all next tests:
--on trunk2 : before every traffic sending in those tests, check trunk tr2 ports in designated trunk table.
*
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
static GT_VOID prvTgfTrunkDesignatedPorts_trunk2ConfigCheck
(
    GT_VOID
)
{
    GT_STATUS   rc;
    GT_U8   devNum = trunk_2_Members[0].device;
    CPSS_PORTS_BMP_STC  designatedPorts_even_value;
    CPSS_PORTS_BMP_STC  designatedPorts_odd_value;
    CPSS_PORTS_BMP_STC  designatedPorts_mask;
    CPSS_PORTS_BMP_STC  designatedPorts;
#ifdef EXMXPM_FAMILY
    GT_U32  numDesig = 16;
#else
    GT_U32  numDesig = 8;
#endif
    GT_U32  ii;

    designatedPorts_mask.ports[0] = 0;
    designatedPorts_mask.ports[1] = 0;

    CPSS_PORTS_BMP_PORT_SET_MAC((&designatedPorts_mask),trunk_2_Members[0].port);
    CPSS_PORTS_BMP_PORT_SET_MAC((&designatedPorts_mask),trunk_2_Members[1].port);

    designatedPorts_even_value.ports[0] = 0;
    designatedPorts_even_value.ports[1] = 0;
    CPSS_PORTS_BMP_PORT_SET_MAC((&designatedPorts_even_value),trunk_2_Members[0].port);

    designatedPorts_odd_value.ports[0] = 0;
    designatedPorts_odd_value.ports[1] = 0;
    CPSS_PORTS_BMP_PORT_SET_MAC((&designatedPorts_odd_value),trunk_2_Members[0].port);

    for(ii = 0 ; ii < numDesig; ii++)
    {
        rc = prvTgfTrunkDesignatedPortsEntryGet(devNum,ii,&designatedPorts);
        if(rc != GT_OK)
        {
            TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedPortsEntryGet FAILED, rc = [%d]", rc);
            return;
        }

        if(ii & 1)
        {
            /* odd entry */
            if(((designatedPorts.ports[0] & designatedPorts_mask.ports[0]) !=
               designatedPorts_odd_value.ports[0]) &&
                ((designatedPorts.ports[1] & designatedPorts_mask.ports[1]) !=
               designatedPorts_odd_value.ports[1]))
            {
                TRUNK_PRV_UTF_LOG0_MAC("[TGF]: prvTgfTrunkDesignatedPorts_trunk2ConfigCheck: odd - designated ports not as expected");
            }
        }
        else
        {
            /* even entry */
            if(((designatedPorts.ports[0] & designatedPorts_mask.ports[0]) !=
               designatedPorts_even_value.ports[0]) &&
                ((designatedPorts.ports[1] & designatedPorts_mask.ports[1]) !=
               designatedPorts_even_value.ports[1]))
            {
                TRUNK_PRV_UTF_LOG0_MAC("[TGF]: prvTgfTrunkDesignatedPorts_trunk2ConfigCheck: even - designated ports not as expected");
            }
        }
    }
}


/*******************************************************************************
* prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic
*
* DESCRIPTION:
9.        send multi-destination traffic , meaning from port p1:
a.        Send BC (FF:FF:FF:FF:FF:FF) with 100 incremental SA.
b.        Send unregistered MC (01:02:03:04:05:06) with 100 incremental SA.
c.        Send unknown unicast (00:09:99:99:99:99) with 100 incremental DA and static SA
d.        Send unknown unicast (00:09:99:99:99:99) with 100 static DA and incremental SA
*
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
static void prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic
(
    GT_VOID
)
{
    GT_STATUS       rc;
    GT_U32          portIter;
    GT_U32          portsCount   = prvTgfPortsNum;

    /* check configuration of trunk'2' before sending traffic*/
    prvTgfTrunkDesignatedPorts_trunk2ConfigCheck();

    /* reset counters */
    for (portIter = 0; portIter < portsCount; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevNum, prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevNum, prvTgfPortsArray[portIter]);
    }

    /*send multi-destination traffic , meaning from port p1*/
    prvTgfCommonMultiDestinationTrafficSend(prvTgfDevNum,
                                            prvTgfPortsArray[0],
                                            BURST_COUNT_CNS,
                                            GT_FALSE);
}


/*******************************************************************************
* prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck
*
* DESCRIPTION:
*   check that traffic counter match the needed value
*
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
static void prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck
(
    IN GT_U8    devNum,
    IN GT_U8    portNum,
    IN GT_U32   expectedCounter
)
{
    prvTgfCommonMultiDestinationTrafficCounterCheck(devNum,portNum,expectedCounter);
}


/*******************************************************************************
* prvTgfTrunkDesignatedPorts_startTest
*
* DESCRIPTION:
1.        in trunk tr1 : Set p4 as designated
2.        in trunk tr1 : Set p3,p4 as enabled members
*
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
static GT_STATUS prvTgfTrunkDesignatedPorts_startTest
(
    GT_VOID
)
{
    GT_STATUS               rc;
    GT_TRUNK_ID             trunkId;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    if(designatedPortsMutation != PRV_TGF_TRUNK_DESIGNATED_PORTS_SET_DESIGNATED_AFTER_TRUNK_SETTING_ENT)
    {
        /*1.        in trunk tr1 : Set p4 as designated*/
        rc = prvTgfTrunkDesignatedMemberSet(trunkId,GT_TRUE,&designatedMember4);
        if (GT_OK != rc)
        {
            TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedMemberSet FAILED, rc = [%d]", rc);
            return rc;
        }
    }

    switch(designatedPortsMutation)
    {
        case PRV_TGF_TRUNK_DESIGNATED_PORTS_BASIC_E:
        /* fall through */
        case PRV_TGF_TRUNK_DESIGNATED_PORTS_SET_DESIGNATED_AFTER_TRUNK_SETTING_ENT:
            if(designatedPort4AsDisabled == GT_FALSE)
            {
                /*2.        in trunk tr1 : Set p3,p4 as enabled members*/
                rc = prvTgfTrunkMembersSet(
                    trunkId /*trunkId*/,
                    2 /*numOfEnabledMembers*/,
                    trunk_1_Members,
                    0 /*numOfDisabledMembers*/,
                    NULL  /*disabledMembersArray*/);
            }
            else
            {
                rc = prvTgfTrunkMembersSet(
                    trunkId /*trunkId*/,
                    1 /*numOfEnabledMembers*/,
                    &trunk_1_Members[0],
                    1 /*numOfDisabledMembers*/,
                    &trunk_1_Members[1]  /*disabledMembersArray*/);
            }
            if (GT_OK != rc)
            {
                TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMembersSet FAILED, rc = [%d]", rc);
                return rc;
            }

            if(designatedPortsMutation != PRV_TGF_TRUNK_DESIGNATED_PORTS_BASIC_E)
            {
                rc = prvTgfTrunkDesignatedMemberSet(trunkId,GT_TRUE,&designatedMember4);
                if (GT_OK != rc)
                {
                    TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedMemberSet FAILED, rc = [%d]", rc);
                    return rc;
                }
            }

            break;
        case PRV_TGF_TRUNK_DESIGNATED_PORTS_ADD_PORTS_INSTEAD_OF_SET_ENT:
            rc = prvTgfTrunkMemberAdd(
                trunkId /*trunkId*/,
                &trunk_1_Members[0]);
            if (GT_OK != rc)
            {
                TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMemberAdd FAILED, rc = [%d]", rc);
                return rc;
            }
            rc = prvTgfTrunkMemberAdd(
                trunkId /*trunkId*/,
                &trunk_1_Members[1]);
            if (GT_OK != rc)
            {
                TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMemberAdd FAILED, rc = [%d]", rc);
                return rc;
            }

            if(designatedPort4AsDisabled == GT_TRUE)
            {
                rc = prvTgfTrunkMemberDisable(
                    trunkId /*trunkId*/,
                    &trunk_1_Members[1]);
                if (GT_OK != rc)
                {
                    TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMemberDisable FAILED, rc = [%d]", rc);
                    return rc;
                }
            }

        default:
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrunkDesignatedPorts_test1
*
* DESCRIPTION:
1.3.1        Test 1 : Set designated before trunk members

1.        in trunk tr1 : Set p4 as designated
2.        in trunk tr1 : Set p3,p4 as enabled members
3.        send multi-destination traffic
4.        check that all traffic egress p4 (not egress port p3) , p2 get all traffic

Check 'unset designated'
5.        in trunk tr1 : UnSet the designated
6.        send multi-destination traffic
7.        check that load balance of traffic between trunk members (p3,p4) , p2 get all traffic
*
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
static GT_STATUS prvTgfTrunkDesignatedPorts_test1
(
    GT_VOID
)
{
    GT_STATUS               rc;
    GT_TRUNK_ID             trunkId;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

/*1.        in trunk tr1 : Set p4 as designated*/
/*2.        in trunk tr1 : Set p3,p4 as enabled members*/
    rc = prvTgfTrunkDesignatedPorts_startTest();
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedPorts_startTest FAILED, rc = [%d]", rc);
        return rc;
    }

/*3.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();

/*4.        check that all traffic egress p4 (not egress port p3) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        FULL_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);


/*Check 'unset designated'*/
    /*5.        in trunk tr1 : UnSet the designated*/
    rc = prvTgfTrunkDesignatedMemberSet(trunkId,GT_FALSE,NULL);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedMemberSet FAILED, rc = [%d]", rc);
        return rc;
    }
/*6.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();
/*7.        check that load balance of traffic between trunk members (p3,p4) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        HALF_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        HALF_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrunkDesignatedPorts_test2
*
* DESCRIPTION:
1.3.2        Test 2 : remove designated (and later add back)

1.        in trunk tr1 : Set p4 as designated
2.        in trunk tr1 : Set p3,p4 as enabled members
3.        remove p4 (designated) from the trunk tr1
4.        send multi-destination traffic
5.        check that all traffic not egress the trunk's port (not egress port p3) , p2 get all traffic , P4 is not in trunk and should get all traffic.
6.        ------------
7.        add p4 (designated) to trunk tr1
8.        send multi-destination traffic
9.        check that all traffic egress p4 (not egress port p3) , p2 get all traffic
*
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
static GT_STATUS prvTgfTrunkDesignatedPorts_test2
(
    GT_VOID
)
{
    GT_STATUS               rc;
    GT_TRUNK_ID             trunkId;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

/*1.        in trunk tr1 : Set p4 as designated*/
/*2.        in trunk tr1 : Set p3,p4 as enabled members*/
    rc = prvTgfTrunkDesignatedPorts_startTest();
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedPorts_startTest FAILED, rc = [%d]", rc);
        return rc;
    }

/*3.        remove p4 (designated) from the trunk tr1*/
    rc = prvTgfTrunkMemberRemove(trunkId,&designatedMember4);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMemberRemove FAILED, rc = [%d]", rc);
        return rc;
    }

/*4.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();

/*5.        check that all traffic not egress the trunk's port (not egress port p3) ,
        p2 get all traffic , P4 is not in trunk and should get all traffic.*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        FULL_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);


/*7.        add p4 (designated) to trunk tr1*/
    rc = prvTgfTrunkMemberAdd(trunkId,&designatedMember4);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMemberAdd FAILED, rc = [%d]", rc);
        return rc;
    }
/*8.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();
/*9.        check that all traffic egress p4 (not egress port p3) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        FULL_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);

    return GT_OK;
}


/*******************************************************************************
* prvTgfTrunkDesignatedPorts_test3
*
* DESCRIPTION:
1.3.3        Test 3 : replace designated

1.        in trunk tr1 : Set p4 as designated
2.        in trunk tr1 : Set p3,p4 as enabled members
3.        in trunk tr1 : Set p3 as designated (replace p4)
4.        send multi-destination traffic
5.        check that all traffic egress p3 (not egress port p4) , p2 get all traffic
6.        ------------
7.        in trunk tr1 : Set p4 as designated (replace p3)
8.        send multi-destination traffic
9.        check that all traffic egress p4 (not egress port p3) , p2 get all traffic
10.        ------------
11.        in trunk tr1 : Set p2 as designated (replace p4)
12.        send multi-destination traffic
13.        check that all traffic not egress any trunk port (not egress port p3,p4) , p2 get all traffic
*
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
static GT_STATUS prvTgfTrunkDesignatedPorts_test3
(
    GT_VOID
)
{
    GT_STATUS               rc;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_MEMBER_STC    designatedMember2;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

/*1.        in trunk tr1 : Set p4 as designated*/
/*2.        in trunk tr1 : Set p3,p4 as enabled members*/
    rc = prvTgfTrunkDesignatedPorts_startTest();
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedPorts_startTest FAILED, rc = [%d]", rc);
        return rc;
    }

/*3.        in trunk tr1 : Set p3 as designated (replace p4)*/
    rc = prvTgfTrunkDesignatedMemberSet(trunkId,GT_TRUE,&designatedMember3);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedMemberSet FAILED, rc = [%d]", rc);
        return rc;
    }

/*4.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();

/*5.        check that all traffic egress p3 (not egress port p4) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        FULL_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);


/*7.        in trunk tr1 : Set p4 as designated (replace p3)*/
    rc = prvTgfTrunkDesignatedMemberSet(trunkId,GT_TRUE,&designatedMember4);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedMemberSet FAILED, rc = [%d]", rc);
        return rc;
    }
/*8.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();
/*9.        check that all traffic egress p4 (not egress port p3) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        FULL_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);


/*11.        in trunk tr1 : Set p2 as designated (replace p4)*/
    designatedMember2.device = prvTgfDevsArray[1];
    designatedMember2.port = prvTgfPortsArray[1];
    rc = prvTgfTrunkDesignatedMemberSet(trunkId,GT_TRUE,&designatedMember2);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedMemberSet FAILED, rc = [%d]", rc);
        return rc;
    }
/*12.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();
/*13.        check that all traffic not egress any trunk port (not egress port p3,p4) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);


    return GT_OK;
}


/*******************************************************************************
* prvTgfTrunkDesignatedPorts_test4
*
* DESCRIPTION:
1.3.4        Test 4 : Disable designated member (and enable again)

1.        in trunk tr1 : Set p4 as designated
2.        in trunk tr1 : Set p3,p4 as enabled members
3.        in trunk tr1 : disable port p4 (designated)
4.        send multi-destination traffic
5.        check that all traffic not egress any trunk port (not egress port p3 , p4) , p2 get all traffic
6.        ------------
7.        in trunk tr1 : enable port p4 (designated)
8.        send multi-destination traffic
9.        check that all traffic egress p4 (not egress port p3) , p2 get all traffic
*
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
static GT_STATUS prvTgfTrunkDesignatedPorts_test4
(
    GT_VOID
)
{
    GT_STATUS               rc;
    GT_TRUNK_ID             trunkId;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

/*1.        in trunk tr1 : Set p4 as designated*/
/*2.        in trunk tr1 : Set p3,p4 as enabled members*/
    rc = prvTgfTrunkDesignatedPorts_startTest();
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedPorts_startTest FAILED, rc = [%d]", rc);
        return rc;
    }

/*3.        in trunk tr1 : disable port p4 (designated)*/
    rc = prvTgfTrunkMemberDisable(trunkId,&designatedMember4);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMemberDisable FAILED, rc = [%d]", rc);
        return rc;
    }

/*4.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();

/*5.        check that all traffic not egress any trunk port (not egress port p3 , p4) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);


/*7.        in trunk tr1 : enable port p4 (designated) */
    rc = prvTgfTrunkMemberEnable(trunkId,&designatedMember4);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMemberEnable FAILED, rc = [%d]", rc);
        return rc;
    }
/*8.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();
/*9.        check that all traffic egress p4 (not egress port p3) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        FULL_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrunkDesignatedPorts_test5
*
* DESCRIPTION:
1.3.5        Test 5 : Disable designated member and set new designated

1.        in trunk tr1 : Set p4 as designated
2.        in trunk tr1 : Set p3,p4 as enabled members
3.        in trunk tr1 : disable port p4 (designated)
4.        in trunk tr1 : Set p3 as designated
5.        send multi-destination traffic
6.        check that all traffic egress p3 (not egress port p4) , p2 get all traffic
7.        ------------
8.        in trunk tr1 : enable port p4
9.        send multi-destination traffic
10.        check that all traffic egress p3 (not egress port p4) , p2 get all traffic
*
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
static GT_STATUS prvTgfTrunkDesignatedPorts_test5
(
    GT_VOID
)
{
    GT_STATUS               rc;
    GT_TRUNK_ID             trunkId;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

/*1.        in trunk tr1 : Set p4 as designated*/
/*2.        in trunk tr1 : Set p3,p4 as enabled members*/
    rc = prvTgfTrunkDesignatedPorts_startTest();
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedPorts_startTest FAILED, rc = [%d]", rc);
        return rc;
    }

/*3.        in trunk tr1 : disable port p4 (designated)*/
    rc = prvTgfTrunkMemberDisable(trunkId,&designatedMember4);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMemberDisable FAILED, rc = [%d]", rc);
        return rc;
    }

/*4.        in trunk tr1 : Set p3 as designated*/
    rc = prvTgfTrunkDesignatedMemberSet(trunkId,GT_TRUE,&designatedMember3);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedMemberSet FAILED, rc = [%d]", rc);
        return rc;
    }

/*5.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();

/*6.        check that all traffic egress p3 (not egress port p4) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        FULL_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);


/*8.        in trunk tr1 : enable port p4*/
    rc = prvTgfTrunkMemberEnable(trunkId,&designatedMember4);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMemberEnable FAILED, rc = [%d]", rc);
        return rc;
    }
/*9.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();
/*10.        check that all traffic egress p3 (not egress port p4) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        FULL_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrunkDesignatedPorts_test6
*
* DESCRIPTION:
1.        in trunk tr1 : Set p4 as designated
2.        in trunk tr1 : Set p3 as enabled member , Set P4 (designated) as disabled member
3.        send multi-destination traffic
4.        check that all traffic not egress any trunk port (not egress port p3 , p4) , p2 get all traffic
5.        ------------
6.        in trunk tr1 : enable port p4
7.        send multi-destination traffic
8.        check that all traffic egress p4 (not egress port p3) , p2 get all traffic
*
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
static GT_STATUS prvTgfTrunkDesignatedPorts_test6
(
    GT_VOID
)
{
    GT_STATUS               rc;
    GT_TRUNK_ID             trunkId;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

/*1.        in trunk tr1 : Set p4 as designated*/
/*2.        in trunk tr1 : Set p3 as enabled member , Set P4 (designated) as disabled member*/
    rc = prvTgfTrunkDesignatedPorts_startTest();
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkDesignatedPorts_startTest FAILED, rc = [%d]", rc);
        return rc;
    }

/*3.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();

/*4.        check that all traffic not egress any trunk port (not egress port p3 , p4) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);


/*6.        in trunk tr1 : enable port p4*/
    rc = prvTgfTrunkMemberEnable(trunkId,&designatedMember4);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMemberEnable FAILED, rc = [%d]", rc);
        return rc;
    }
/*7.        send multi-destination traffic*/
    prvTgfTrunkDesignatedPorts_sendMultiDestinationTraffic();
/*8.        check that all traffic egress p4 (not egress port p3) , p2 get all traffic*/
    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[0].device,trunk_1_Members[0].port,
        NO_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        trunk_1_Members[1].device,trunk_1_Members[1].port,
        FULL_TRAFFIC_EXPECTED_CNS);

    prvTgfTrunkDesignatedPorts_MultiDestinationTrafficCheck(
        prvTgfDevsArray[1],prvTgfPortsArray[1],
        FULL_TRAFFIC_EXPECTED_CNS);

    return GT_OK;
}

/*******************************************************************************
* internalTrunkDesignatedPorts
*
* DESCRIPTION:
*       Test for designated member in trunk.
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
static GT_VOID internalTrunkDesignatedPorts
(
    GT_VOID
)
{
    prvTgfTrunkDesignatedPorts_basicCommonSetting();
    prvTgfTrunkDesignatedPorts_test1();
    prvTgfTrunkForDesignatedPortsTestConfigurationReset();

    prvTgfTrunkDesignatedPorts_basicCommonSetting();
    prvTgfTrunkDesignatedPorts_test2();
    prvTgfTrunkForDesignatedPortsTestConfigurationReset();

    prvTgfTrunkDesignatedPorts_basicCommonSetting();
    prvTgfTrunkDesignatedPorts_test3();
    prvTgfTrunkForDesignatedPortsTestConfigurationReset();

    prvTgfTrunkDesignatedPorts_basicCommonSetting();
    prvTgfTrunkDesignatedPorts_test4();
    prvTgfTrunkForDesignatedPortsTestConfigurationReset();

    prvTgfTrunkDesignatedPorts_basicCommonSetting();
    prvTgfTrunkDesignatedPorts_test5();
    prvTgfTrunkForDesignatedPortsTestConfigurationReset();

    designatedPort4AsDisabled = GT_TRUE;
    prvTgfTrunkDesignatedPorts_basicCommonSetting();
    prvTgfTrunkDesignatedPorts_test6();
    prvTgfTrunkForDesignatedPortsTestConfigurationReset();
    designatedPort4AsDisabled = GT_FALSE;
}

/*******************************************************************************
* prvTgfTrunkDesignatedPorts
*
* DESCRIPTION:
*       Test for designated member in trunk.
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
GT_VOID prvTgfTrunkDesignatedPorts
(
    GT_VOID
)
{
    /* set transmit time to 0 */

    /* mutation 0 */
    designatedPortsMutation = PRV_TGF_TRUNK_DESIGNATED_PORTS_BASIC_E;
    internalTrunkDesignatedPorts();
    /* mutation 1 - set designated after trunk setting */
    designatedPortsMutation = PRV_TGF_TRUNK_DESIGNATED_PORTS_SET_DESIGNATED_AFTER_TRUNK_SETTING_ENT;
    internalTrunkDesignatedPorts();
    /* mutation 2 - : 'add' ports instead of 'set'*/
    designatedPortsMutation = PRV_TGF_TRUNK_DESIGNATED_PORTS_ADD_PORTS_INSTEAD_OF_SET_ENT;
    internalTrunkDesignatedPorts();
}

/*******************************************************************************
* prvTgfTrunkVlanTestInit
*
* DESCRIPTION:
*       Initialize local vlan default settings
*
* INPUTS:
*       vlanId      - vlan id.
*       portMembers - bit map of ports belonging to the vlan.
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
static GT_VOID prvTgfTrunkVlanTestInit
(
    IN GT_U16                                  vlanId,
    CPSS_PORTS_BMP_STC                         portsMembers
)
{
    GT_STATUS                   rc = GT_OK;
    CPSS_PORTS_BMP_STC          portsTagging;
    PRV_TGF_BRG_VLAN_INFO_STC   vlanInfo;
    PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;

    /* clear entry */
    cpssOsMemSet(&vlanInfo, 0, sizeof(vlanInfo));
    cpssOsMemSet(&portsTaggingCmd, 0, sizeof(PRV_TGF_BRG_VLAN_PORTS_TAG_CMD_STC));
    portsTagging.ports[0] = 0;
    portsTagging.ports[1] = 0;

    /* set vlan entry */
    vlanInfo.unkSrcAddrSecBreach  = GT_FALSE;
    vlanInfo.unregNonIpMcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv4McastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregIpv6McastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unkUcastCmd          = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
    vlanInfo.unregIpv4BcastCmd    = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.unregNonIpv4BcastCmd = CPSS_PACKET_CMD_FORWARD_E;
    vlanInfo.ipv4IgmpToCpuEn      = GT_FALSE;
    vlanInfo.mirrToRxAnalyzerEn   = GT_FALSE;
    vlanInfo.ipv6IcmpToCpuEn      = GT_FALSE;
    vlanInfo.ipCtrlToCpuEn        = PRV_TGF_BRG_IP_CTRL_NONE_E;
    vlanInfo.ipv4IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv6IpmBrgMode       = CPSS_BRG_IPM_SGV_E;
    vlanInfo.ipv4IpmBrgEn         = GT_FALSE;
    vlanInfo.ipv6IpmBrgEn         = GT_FALSE;
    vlanInfo.ipv6SiteIdMode       = CPSS_IP_SITE_ID_INTERNAL_E;
    vlanInfo.ipv4UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv4McastRouteEn     = GT_FALSE;
    vlanInfo.ipv6UcastRouteEn     = GT_FALSE;
    vlanInfo.ipv6McastRouteEn     = GT_FALSE;
    vlanInfo.stgId                = 0;
    vlanInfo.autoLearnDisable     = GT_TRUE;/* working in controlled learning */
    vlanInfo.naMsgToCpuEn         = GT_TRUE;/* working in controlled learning */
    vlanInfo.mruIdx               = 0;
    vlanInfo.bcastUdpTrapMirrEn   = GT_FALSE;
    vlanInfo.floodVidx            = 0xFFF;
    vlanInfo.floodVidxMode        = PRV_TGF_BRG_VLAN_FLOOD_VIDX_MODE_ALL_FLOODED_TRAFFIC_E;
    vlanInfo.ucastLocalSwitchingEn = GT_FALSE;
    vlanInfo.mcastLocalSwitchingEn = GT_FALSE;

    /* set vlan entry */
    rc = prvTgfBrgVlanEntryWrite(prvTgfDevNum,
                                 vlanId,
                                 &portsMembers,
                                 &portsTagging,
                                 &vlanInfo,
                                 &portsTaggingCmd);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryWrite: %d", vlanId);
}

/*******************************************************************************
* prvTgfTrunkVlanRestore
*
* DESCRIPTION:
*       Restore\Cancel prvTgfTrunkToCpuTraffic settings
*
* INPUTS:
*       vlanId      - vlan id.
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
static GT_VOID prvTgfTrunkVlanRestore
(
    IN GT_U16   vlanId
)
{
    GT_STATUS   rc = GT_OK;

    /* set vlan entry */
    rc = prvTgfBrgVlanEntryInvalidate(vlanId);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgVlanEntryInvalidate: %d", vlanId);
}

/*******************************************************************************
* prvTgfTrunkAndVlanTestConfigure
*
* DESCRIPTION:
*       This function configures trunk and vlan
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkAndVlanTestConfigure
(
    GT_VOID
)
{
    CPSS_TRUNK_MEMBER_STC          enabledMembersArray[4];
    GT_STATUS                      rc;
    GT_TRUNK_ID             trunkId;

    enabledMembersArray[0].device = prvTgfDevsArray[PRV_TGF_TRUNK_PORT0_INDEX_CNS];
    enabledMembersArray[0].port   = prvTgfPortsArray[PRV_TGF_TRUNK_PORT0_INDEX_CNS];
    enabledMembersArray[1].device = prvTgfDevsArray[PRV_TGF_TRUNK_PORT1_INDEX_CNS];
    enabledMembersArray[1].port   = prvTgfPortsArray[PRV_TGF_TRUNK_PORT1_INDEX_CNS];
    enabledMembersArray[2].device = prvTgfDevsArray[PRV_TGF_TRUNK_PORT2_INDEX_CNS];
    enabledMembersArray[2].port   = prvTgfPortsArray[PRV_TGF_TRUNK_PORT2_INDEX_CNS];
    enabledMembersArray[3].device = prvTgfDevsArray[PRV_TGF_TRUNK_PORT3_INDEX_CNS];
    enabledMembersArray[3].port   = prvTgfPortsArray[PRV_TGF_TRUNK_PORT3_INDEX_CNS];

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    rc = prvTgfTrunkMembersSet(
        trunkId /*trunkId*/,
        4 /*numOfEnabledMembers*/,
        enabledMembersArray,
        0 /*numOfDisabledMembers*/,
        (CPSS_TRUNK_MEMBER_STC*)NULL  /*disabledMembersArray*/);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMembersSet FAILED, rc = [%d]", rc);
        return rc;
    }

    /* ports 0, 8, 18, 23 are VLAN Members */
    localPortsVlanMembers.ports[0] = BIT_0 | BIT_8 | BIT_18 | BIT_23;

    /* set VLAN entry */
     prvTgfTrunkVlanTestInit(PRV_TGF_VLANID_CNS, localPortsVlanMembers);

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrunkTestPacketSend
*
* DESCRIPTION:
*       Function sends packet, performs trace and check expected results.
*
* INPUTS:
*       devNum        - device number to send traffic from
*       portNum       - port number to send traffic from
*       packetInfoPtr - PACKET to send
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
static GT_VOID prvTgfTrunkTestPacketSend
(
    IN GT_U8           devNum,
    IN GT_U8           portNum,
    IN TGF_PACKET_STC *packetInfoPtr,
    IN GT_U32               burstCount,
    IN GT_U32               numVfd,
    IN TGF_VFD_INFO_STC     vfdArray[]
)
{
    GT_STATUS       rc           = GT_OK;

    /* use different 'etherType for vlan tagging' instead of 0x8100 */
    /* this will make our packet that has '0x8100' to be recognized as 'untagged'
      by the 'traffic generator' and also the CPSS will not override the ethertype
      of tag0 with the DSA tag */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_NON_VALID_TAG_CNS);

    /* setup Packet */
    rc = prvTgfSetTxSetupEth(devNum, packetInfoPtr, burstCount, numVfd, vfdArray);
    UTF_VERIFY_EQUAL3_STRING_MAC(GT_OK, rc, "ERROR of SetTxSetup: %d, %d , %d",
                                 devNum, burstCount, numVfd );

    utfPrintKeepAlive();

    /* send Packet from port portNum */
    rc = prvTgfStartTransmitingEth(devNum, portNum);
    if(rc == GT_HW_ERROR)
    {
        GT_U8   _devNum,_portNum;
        GT_U32  portIter;/* port iterator */

        cpssOsTimerWkAfter(20);

        /* give it another try , but first clear all the counters */
        for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
        {
            _devNum  = prvTgfDevsArray [portIter];
            _portNum = prvTgfPortsArray[portIter];
            TRUNK_PRV_UTF_LOG2_MAC("=======  : dev[%d]port[%d] reset counters  =======\n",
                _devNum, _portNum);

            /* reset counters */
            rc = prvTgfResetCountersEth(_devNum, _portNum);
            UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                         _devNum, _portNum);
        }

        /* send Packet from port portNum */
        rc = prvTgfStartTransmitingEth(devNum, portNum);
    }
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of StartTransmitting: %d, %d",
                                 devNum, portNum);

    /* restore default ethertype */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_8100_VLAN_TAG_CNS);
}

/*******************************************************************************
* prvTgfTrunkToCpuTrafficFromPortAndDevice
*
* DESCRIPTION:
*       Generate and test traffic from port on device
*
* INPUTS:
*       devNum  - device number to send traffic from
*       portNum - port number to send traffic from
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
static GT_VOID prvTgfTrunkToCpuTrafficFromPortAndDevice
(
    IN GT_U8           devNum,
    IN GT_U8           portNum
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      portIter = 0;
    CPSS_PORT_MAC_COUNTER_SET_STC portCntrs;
    GT_U8                trigPacketBuff[TGF_RX_BUFFER_MAX_SIZE_CNS] = {0};
    GT_U32               buffLen = TGF_RX_BUFFER_MAX_SIZE_CNS;
    GT_U32               packetActualLength = 0;
    GT_U8                queue;
    TGF_NET_DSA_STC      rxParam;
    CPSS_INTERFACE_INFO_STC portInterface;

    /* reset counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevsArray[portIter], prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfResetCountersEth: %d, %d",
                                     prvTgfDevsArray[portIter], prvTgfPortsArray[portIter]);
    }

    /* enable PCL ingress Policy */
    rc = prvTgfPclIngressPolicyEnable(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclIngressPolicyEnable: %d", GT_TRUE);

    /* setup portInterface for capturing */
    portInterface.type            = CPSS_INTERFACE_PORT_E;
    portInterface.devPort.devNum  = prvTgfDevNum;
    portInterface.devPort.portNum = portNum;

    /* enable capture on port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_PCL_E, GT_TRUE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, &d\n",
                                 prvTgfDevNum, portNum);

    /* clear table */
    rc = tgfTrafficTableRxPcktTblClear();
    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, rc, "tgfTrafficTableRxPcktTblClear");

    /* send packet */
    prvTgfTrunkTestPacketSend(devNum, portNum, &prvTgfPacketVlanTagInfo ,1 ,0 ,NULL);

    /* check counters */
    for (portIter = 0; portIter < prvTgfPortsNum; portIter++)
    {
        /* read counters */
        rc = prvTgfReadPortCountersEth(prvTgfDevsArray[portIter], prvTgfPortsArray[portIter], GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     prvTgfDevsArray[portIter], prvTgfPortsArray[portIter]);

        if ((devNum == prvTgfDevsArray[portIter]) &&
            (portNum == prvTgfPortsArray[portIter]))
        {
            /* check Rx counters */
            UTF_VERIFY_EQUAL0_STRING_MAC(1, portCntrs.goodPktsRcv.l[0],
                                         "get another goodPktsRcv counter than expected");
            continue;
        }


        /* check Tx counters */
        UTF_VERIFY_EQUAL0_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                                     "get another goodPktsSent counter than expected");
    }

    /* disable capture on nexthope port */
    rc = tgfTrafficGeneratorPortTxEthCaptureSet(&portInterface, TGF_CAPTURE_MODE_PCL_E, GT_FALSE);
    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "ERROR of tgfTrafficGeneratorPortTxEthCaptureSet: %d, &d\n",
                                 prvTgfDevNum, portNum);

    /* Get entry from captured packet's table */
    rc = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                  GT_TRUE, GT_TRUE, trigPacketBuff,
                                  &buffLen, &packetActualLength,
                                  &devNum, &queue,
                                  &rxParam);

    UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "tgfTrafficGeneratorRxInCpuGet: %d, %d",
                                 TGF_PACKET_TYPE_CAPTURE_E, GT_TRUE);

    UTF_VERIFY_EQUAL0_STRING_MAC(portNum,
                                 rxParam.portNum,
                                 "sampled packet port num different then expected");

    UTF_VERIFY_EQUAL0_STRING_MAC(devNum,
                                 rxParam.devNum,
                                 "sampled packet port num different then expected");

    /* Get next entry from captured packet's table - no entry expected*/
    rc = tgfTrafficGeneratorRxInCpuGet(TGF_PACKET_TYPE_CAPTURE_E,
                                  GT_FALSE, GT_TRUE, trigPacketBuff,
                                  &buffLen, &packetActualLength,
                                  &devNum, &queue,
                                  &rxParam);

    UTF_VERIFY_EQUAL2_STRING_MAC(GT_NO_MORE, rc, "tgfTrafficGeneratorRxInCpuGet: %d, %d",
                                 TGF_PACKET_TYPE_CAPTURE_E, GT_TRUE);

    /* disable PCL ingress Policy */
    rc = prvTgfPclIngressPolicyEnable(GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfPclIngressPolicyEnable: %d", GT_FALSE);
}

/*******************************************************************************
* prvTgfTrunkToCpuTraffic
*
* DESCRIPTION:
*       Generate and test traffic
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
GT_VOID prvTgfTrunkToCpuTraffic
(
    GT_VOID
)
{
    GT_STATUS   rc = GT_OK;
    GT_U32      i;

    /* enable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_TRUE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_TRUE);

    for( i = 0 ; i <= PRV_TGF_TRUNK_PORT3_INDEX_CNS ; i++ )
    {
        /* send packet to port and device */
        prvTgfTrunkToCpuTrafficFromPortAndDevice(prvTgfDevsArray[i], prvTgfPortsArray[i]);
    }

    /* disable packet trace */
    rc = tgfTrafficTracePacketByteSet(GT_FALSE);
    UTF_VERIFY_EQUAL1_STRING_MAC(rc, GT_OK, "Error in tgfTrafficTracePacketByteSet %d", GT_FALSE);
}

/*******************************************************************************
* prvTgfTrunkAndVlanTestConfigurationReset
*
* DESCRIPTION:
*       This function resets configuration of trunk and vlan
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkAndVlanTestConfigurationReset
(
    GT_VOID
)
{
    GT_STATUS   rc;
    GT_TRUNK_ID             trunkId;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    rc = prvTgfTrunkMembersSet(
        trunkId /*trunkId*/,
        0 /*numOfEnabledMembers*/,
        (CPSS_TRUNK_MEMBER_STC*)NULL /*enabledMembersArray*/,
        0 /*numOfDisabledMembers*/,
        (CPSS_TRUNK_MEMBER_STC*)NULL  /*disabledMembersArray*/);
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: prvTgfTrunkMembersSet FAILED, rc = [%d]", rc);
        return rc;
    }

    prvTgfTrunkVlanRestore(PRV_TGF_VLANID_CNS);

    /* clear capturing RxPcktTable */
    rc = tgfTrafficTableRxPcktTblClear();
    if (GT_OK != rc)
    {
        TRUNK_PRV_UTF_LOG1_MAC("[TGF]: tgfTrafficTableRxPcktTblClear FAILED, rc = [%d]", rc);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* prvTgfTrunkLoadBalanceCheck
*
* DESCRIPTION:
*   check that total summary of counters of ports that belong to trunk got value
*   of numPacketSent.
*
* INPUTS:
*       trunkId - trunk Id that his ports should received the traffic and on
*                 them we expect total of numPacketSent packets
*       mode    - the mode of expected load balance .
*                   all traffic expected to be received from single port or with
*                   even distribution on the trunk ports
*       numPacketSent - number of packet that we expect the trunk to receive
*       tolerance - relevant to PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E
*                   state the number of packet that may get to other port in the trunk.
*                   meaning that (numPacketSent - tolerance) should get to port A
*                   and tolerance should get to port B , but other ports (in trunk)
*                   should not get it.
*       trunkMemberSenderPtr - when not NULL , indicates that the traffic to check
*                   LBH that egress the trunk was originally INGRESSED from the trunk.
*                   but since in the 'enhanced UT' the CPU send traffic to a port
*                   due to loopback it returns to it, we need to ensure that the
*                   member mentioned here should get the traffic since it is the
*                   'original sender'
* OUTPUTS:
*       stormingDetectedPtr - when not NULL , used to check if storming detected.
*                   relevant only when trunkMemberSenderPtr != NULL
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfTrunkLoadBalanceCheck
(
    IN GT_TRUNK_ID  trunkId,
    IN PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT  mode,
    IN GT_U32       numPacketSent,
    IN GT_U32       tolerance,
    IN CPSS_TRUNK_MEMBER_STC *trunkMemberSenderPtr,
    OUT GT_BOOL     *stormingDetectedPtr
)
{
    GT_STATUS   rc;
    GT_U8   devNum = prvTgfDevNum;
    GT_U32  ii;
    GT_U32  portIter;/* port iterator */
    GT_U32                  numOfEnabledMembers = PRV_CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
    CPSS_TRUNK_MEMBER_STC   enabledMembersArray[PRV_CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    GT_U32  firstPortWithCounter = 0xFFFFFFFF;/*relevant only to mode 'Single port'*/
    GT_U32  expectedCountPerPort;/*relevant only to mode 'even'*/
    GT_U32  *expectedCountPtr = NULL;/*relevant to 'single port only'*/
    GT_U32  expectedCount = numPacketSent - tolerance;/*relevant to 'single port only'*/
    CPSS_TRUNK_TYPE_ENT trunkType;/* trunk type*/
    CPSS_TRUNK_MEMBER_STC currMember;/* currentMember*/
    CPSS_PORTS_BMP_STC cascadeTrunkPorts;/* cascade trunk ports */
    GT_U32  cascadePortIndex;/*cascade port index*/
    GT_U32  membersIndexesBmp = trunkLbhMembersIndexesBmp;/* members indexes that may get traffic */
    GT_U32  numMembersReceive = 0;
    GT_U32  originalNumPacketSent = numPacketSent;/* original number of packet sent to the trunk */
    GT_BOOL trafficReceivedOnSender = GT_FALSE;/* was traffic switched back to the sender */
    GT_U32  maxIterations;/* max iterations for break of loopback */
    GT_U32  sleepTime;/*time to sleep -- in milliseconds */

    TRUNK_PRV_UTF_LOG4_MAC("=======  prvTgfTrunkLoadBalanceCheck: trunkId[%d] mode[%d] numPacketSent[%d] burst tolerance[%d]=======\n",
            trunkId,mode,numPacketSent,tolerance);


    if(stormingDetectedPtr)
    {
        *stormingDetectedPtr = GT_FALSE;
    }

    switch(mode)
    {
        case PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E:
            break;
        case PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EVEN_E:
            break;
        case PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EMPTY_E:
            mode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EVEN_E;
            numPacketSent = 0;
            break;
        case PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E:
            break;
        case PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E:
            break;
        default:
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, GT_BAD_PARAM);
            return;
    }

    cascadeTrunkPorts.ports[0] = 0;
    cascadeTrunkPorts.ports[1] = 0;
    cascadePortIndex = 0;

    TRUNK_PRV_UTF_LOG0_MAC("=======  : trunkId type get =======\n");
    rc = prvCpssGenericTrunkDbTrunkTypeGet(devNum,trunkId,&trunkType);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    if(trunkType != CPSS_TRUNK_TYPE_CASCADE_E)
    {
        TRUNK_PRV_UTF_LOG0_MAC("=======  : trunkId members get =======\n");
        rc = prvCpssGenericTrunkDbEnabledMembersGet(devNum,trunkId,&numOfEnabledMembers,enabledMembersArray);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

        if(numOfEnabledMembers == 0)
        {
            /* empty trunk ??? */
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, GT_EMPTY);
            return;
        }

        /* convert HW devNum to SW devNum */
        for (ii = 0; ii < numOfEnabledMembers; ii++)
        {
            rc = prvUtfSwFromHwDeviceNumberGet(enabledMembersArray[ii].device,&enabledMembersArray[ii].device);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
        }
    }
    else
    {
        TRUNK_PRV_UTF_LOG0_MAC("=======  : cascade trunk ports get =======\n");
        /* cascade trunk */
        rc = prvCpssGenericTrunkCascadeTrunkPortsGet(devNum,trunkId,&cascadeTrunkPorts);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

        numOfEnabledMembers = 0;

        for (ii = 0; ii < 64; ii++)
        {
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&cascadeTrunkPorts,ii))
            {
                continue;
            }

            numOfEnabledMembers++;
        }

        if(numOfEnabledMembers == 0)
        {
            /* empty trunk ??? */
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, GT_EMPTY);
            return;
        }
    }


    /* relevant to modes 'even' and 'Specific members' */
    expectedCountPerPort = numPacketSent / numOfEnabledMembers;
    if (mode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E)
    {
        numMembersReceive = 0;
        for (ii = 0; ii < 32; ii++)
        {
            if((1 << ii) &  membersIndexesBmp)
            {
                numMembersReceive++;
            }
        }

        expectedCountPerPort = numPacketSent / numMembersReceive;
    }
    else if (mode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E)
    {
        expectedCountPerPort = numPacketSent / trunkLbhNumOfLimitedMemebers;
        numMembersReceive = 0;/* number of members that actually get traffic */
    }

    TRUNK_PRV_UTF_LOG0_MAC("=======  : check members ports counters =======\n");
    /* check counters */
    for (ii = 0; ii < numOfEnabledMembers; ii++)
    {

        if(trunkType != CPSS_TRUNK_TYPE_CASCADE_E)
        {
            currMember.device = enabledMembersArray[ii].device;
            currMember.port = enabledMembersArray[ii].port;
        }
        else
        {
            for(/* continue */;cascadePortIndex < 64 ; cascadePortIndex++)
            {
                if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&cascadeTrunkPorts,cascadePortIndex))
                {
                    break;
                }
            }

            currMember.port = (GT_U8)cascadePortIndex;
            currMember.device = devNum;
            cascadePortIndex++;
        }


        TRUNK_PRV_UTF_LOG2_MAC("=======  : dev[%d]port[%d] read counters  =======\n",
            currMember.device, currMember.port);
        /* read counters */
        rc = prvTgfReadPortCountersEth(currMember.device, currMember.port, GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     currMember.device, currMember.port);

        TRUNK_PRV_UTF_LOG2_MAC("=======  : dev[%d]port[%d] check that port should get this traffic  =======\n",
            currMember.device, currMember.port);


        if(trunkMemberSenderPtr)
        {
            if(trunkMemberSenderPtr->device == currMember.device &&
               trunkMemberSenderPtr->port   == currMember.port )
            {
                CPSS_PORT_MAC_COUNTER_SET_STC tmpPortCntrs;
                /* ignore the ORIGINAL number of packets sent to it (from the CPU). */
                portCntrs.goodPktsSent.l[0] -= originalNumPacketSent;

                /* since this port (member of the trunk) is the sender of traffic ,
                   and the traffic may be <multi-destination local switching Enable> == GT_TRUE
                   meaning that we may get storming of packets.

                   */

                if(portCntrs.goodPktsSent.l[0])
                {
                    /* we need to wait until all packets processed */

                    /* let the device time to terminate the storming */
#ifdef ASIC_SIMULATION
                    maxIterations = 50; /* 0.5 seconds -- tests show that only 2 loops are used !!! */
                    sleepTime = 10;
#else /*!ASIC_SIMULATION*/
                    maxIterations = 5;  /* also in BM some cases needed 2 loops instead of 1 */
                    sleepTime = 10;
#endif /*!ASIC_SIMULATION*/
                    do
                    {
                        cpssOsTimerWkAfter(sleepTime);
                        /* read the counters */
                        rc = prvTgfReadPortCountersEth(currMember.device, currMember.port, GT_TRUE, &tmpPortCntrs);
                        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                                     currMember.device, currMember.port);
                    }
                    while((--maxIterations) && (tmpPortCntrs.goodPktsSent.l[0] || tmpPortCntrs.goodPktsSent.l[1]));

                    if(maxIterations == 0 && (tmpPortCntrs.goodPktsSent.l[0] || tmpPortCntrs.goodPktsSent.l[1]))
                    {
                        /* error -- print it regardless to 'Printing mode' */
                        cpssOsPrintf("== ERROR == ERROR ==  : dev[%d]port[%d] port storming detected but not terminated !  =======\n",
                            currMember.device, currMember.port);
                    }

                    /*7. skip further processing of this port*/
                    trafficReceivedOnSender = GT_TRUE;
                    if(stormingDetectedPtr)
                    {
                        *stormingDetectedPtr = GT_TRUE;
                    }

/*                    cpssOsPrintf("$%d$",maxIterations);*/

                    continue;
                }

            }
        }

        if(mode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E)
        {
            if(portCntrs.goodPktsSent.l[0] != 0)
            {
                if(firstPortWithCounter == 0)
                {
                    /* this is the third port that got traffic ? */
                    UTF_VERIFY_EQUAL3_STRING_MAC(0, portCntrs.goodPktsSent.l[0],
                        "trunk load balance: this is the third port that got traffic ? : in trunk[%d] expected [%d] got [%d]",
                                                 trunkId,0, portCntrs.goodPktsSent.l[0]);
                }
                else if(firstPortWithCounter == 0xFFFFFFFF)
                {
                    expectedCountPtr = portCntrs.goodPktsSent.l[0] > tolerance ?
                                       &expectedCount : &tolerance;

                    firstPortWithCounter &= 0xffff;

                    if(portCntrs.goodPktsSent.l[0] != numPacketSent &&
                       portCntrs.goodPktsSent.l[0] != (numPacketSent - tolerance) &&
                       portCntrs.goodPktsSent.l[0] != tolerance)
                    {
                        UTF_VERIFY_EQUAL3_STRING_MAC((*expectedCountPtr), portCntrs.goodPktsSent.l[0],
                            "trunk load balance: in trunk[%d] expected [%d] got [%d]",
                                                     trunkId,(*expectedCountPtr), portCntrs.goodPktsSent.l[0]);
                    }
                }
                else if(tolerance)
                {
                    firstPortWithCounter &= 0xffff0000;

                    if(expectedCountPtr == (&tolerance))
                    {
                        UTF_VERIFY_EQUAL3_STRING_MAC(expectedCount, portCntrs.goodPktsSent.l[0],
                            "trunk load balance: in trunk[%d] expected [%d] got [%d]",
                                                     trunkId,expectedCount, portCntrs.goodPktsSent.l[0]);
                    }
                    else /*if(expectedCountPtr == (&expectedCount))*/
                    {
                        UTF_VERIFY_EQUAL3_STRING_MAC(tolerance, portCntrs.goodPktsSent.l[0],
                            "trunk load balance: in trunk[%d] expected [%d] got [%d]",
                                                     trunkId,tolerance, portCntrs.goodPktsSent.l[0]);
                    }
                }
                else
                {
                    UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, GT_FAIL,
                        "trunk load balance: got packet not on single port in trunk[%d] ",
                                                 trunkId);
                }
            }
            else if(expectedCount == 0)
            {
                /* we expect no counter on the trunk's ports */
                UTF_VERIFY_EQUAL3_STRING_MAC(expectedCount, portCntrs.goodPktsSent.l[0],
                    "trunk load balance: in trunk[%d] expected [%d] got [%d]",
                                             trunkId,expectedCount, portCntrs.goodPktsSent.l[0]);
            }
        }
        else if (mode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E)
        {
            /*  need to see that member that should get traffic will get it,
                and member that should not get it will not get it.
            */

            /* look for the member index in the test */

            for(portIter = 0 ; portIter < prvTgfPortsNum ; portIter++)
            {
                if(0 == ((1 << portIter) &  membersIndexesBmp))
                {
                    /* we expect no traffic from this trunk's port */
                    continue;
                }

                if(prvTgfDevsArray[portIter]  == currMember.device &&
                   prvTgfPortsArray[portIter] == currMember.port )
                {
                    /* this member should receive traffic */
                    break;
                }
            }

            if(portIter == prvTgfPortsNum)
            {
                /* we expect no traffic from this trunk's port */
                UTF_VERIFY_EQUAL5_STRING_MAC(0 , portCntrs.goodPktsSent.l[0],
                    "trunk load balance: expected[%d] received [%d] on dev[%d] on port[%d] in trunk[%d] ",
                                             0,portCntrs.goodPktsSent.l[0],
                                             currMember.device, currMember.port,
                                             trunkId);
            }
            else
            {
                if(portCntrs.goodPktsSent.l[0] != expectedCountPerPort &&
                   portCntrs.goodPktsSent.l[0] != (expectedCountPerPort + 1))
                {
                    UTF_VERIFY_EQUAL5_STRING_MAC(expectedCountPerPort , portCntrs.goodPktsSent.l[0],
                        "trunk load balance: expected[%d] received [%d] on dev[%d] on port[%d] in trunk[%d] ",
                                                 expectedCountPerPort,portCntrs.goodPktsSent.l[0],
                                                 currMember.device, currMember.port,
                                                 trunkId);
                }
            }


        }
        else if (mode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E)
        {
            if(portCntrs.goodPktsSent.l[0] != 0)
            {
                /* count that this port got traffic */
                numMembersReceive++;

                /* check that it got 'Expected value'*/
                if(portCntrs.goodPktsSent.l[0] != expectedCountPerPort &&
                   portCntrs.goodPktsSent.l[0] != (expectedCountPerPort + 1))
                {
                    UTF_VERIFY_EQUAL5_STRING_MAC(expectedCountPerPort , portCntrs.goodPktsSent.l[0],
                        "trunk load balance: expected[%d] received [%d] on dev[%d] on port[%d] in trunk[%d] ",
                                                 expectedCountPerPort,portCntrs.goodPktsSent.l[0],
                                                 currMember.device, currMember.port,
                                                 trunkId);
                }
            }
        }
        /*PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EVEN_E*/
        else if (GT_FALSE == prvTgfCommonIsDeviceForce(prvTgfDevNum))
        {
            if(portCntrs.goodPktsSent.l[0] != expectedCountPerPort &&
               portCntrs.goodPktsSent.l[0] != (expectedCountPerPort + 1))
            {
                UTF_VERIFY_EQUAL5_STRING_MAC(expectedCountPerPort , portCntrs.goodPktsSent.l[0],
                    "trunk load balance: expected[%d] received [%d] on dev[%d] on port[%d] in trunk[%d] ",
                                             expectedCountPerPort,portCntrs.goodPktsSent.l[0],
                                             currMember.device, currMember.port,
                                             trunkId);
            }
        }
    }

    if(mode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E)
    {
        if(firstPortWithCounter == 0xFFFFFFFF && numPacketSent != 0 &&
           trafficReceivedOnSender == GT_FALSE)/* single port that received was the sender */
        {
            /* no port received the traffic */
            UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, GT_FAIL,
                "trunk load balance: no port receive packets in trunk [%d] ",
                                         trunkId);
        }
    }
    else if (mode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E)
    {
        if(numMembersReceive != trunkLbhNumOfLimitedMemebers)
        {
            /* no port received the traffic */
            UTF_VERIFY_EQUAL3_STRING_MAC(numMembersReceive, trunkLbhNumOfLimitedMemebers,
                "trunk load balance: in trunk [%d] received number of ports [%d] , instead of [%d]",
                                         trunkId, numMembersReceive, trunkLbhNumOfLimitedMemebers);
        }
    }

    TRUNK_PRV_UTF_LOG0_MAC("======= prvTgfTrunkLoadBalanceCheck : ended  =======\n");

    return;
}

/*******************************************************************************
* prvTgfTrunkLoadBalanceSpecificMembersSet
*
* DESCRIPTION:
*   set the specific members of a trunk that will get traffic when the LBH
*   expected mode is : PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E.
*
* INPUTS:
*       membersIndexesBmp - bmp of indexes in the arrays of :
*       prvTgfDevsArray[] , prvTgfPortsArray[]
*           the traffic should do LBH in those ports and not in other ports of
*           the trunk.
*
*       WARNING : this is NOT a good practice to set the system to get this behavior
*
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
void prvTgfTrunkLoadBalanceSpecificMembersSet
(
    IN GT_U32   membersIndexesBmp
)
{
    trunkLbhMembersIndexesBmp = membersIndexesBmp;

    return;
}

/*******************************************************************************
* prvTgfTrunkLoadBalanceLimitedNumSet
*
* DESCRIPTION:
*   set the limited number of members of a trunk that will get traffic when the LBH
*   expected mode is : PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E.
*
* INPUTS:
*       numOfLimitedMemebers - limited number of members of a trunk
*
*       WARNING : this is NOT a good practice to set the system to get this behavior
*
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
void prvTgfTrunkLoadBalanceLimitedNumSet
(
    IN GT_U32   numOfLimitedMemebers
)
{
    trunkLbhNumOfLimitedMemebers = numOfLimitedMemebers;

    return;
}

/*******************************************************************************
* trunkHashPearsonFill
*
* DESCRIPTION:
*   fill Pearson hash table according to mode .
*
*       applicable devices: Lion and above
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
static GT_STATUS trunkHashPearsonFill
(
    IN PEARSON_TEST_MODE_ENT   pearsonMode
)
{
    GT_STATUS   rc;
    GT_U32      index;
    GT_U32      value;

    for(index = 0 ; index < PRV_TGF_TRUNK_PEARSON_TABLE_SIZE_CNS ; index++)
    {
        switch(pearsonMode)
        {
            case PEARSON_TEST_MODE_0_E:              /*value = 0 (all indexes) */
                value = 0;
                break;
            case PEARSON_TEST_MODE_INDEX_E:          /*value = index (0..63)*/
                value = index;
                break;
            case PEARSON_TEST_MODE_REVERSE_INDEX_E:  /*value = 63 - index (63..0)*/
                value = (PRV_TGF_TRUNK_PEARSON_TABLE_SIZE_CNS - 1) - index;
                break;
            case PEARSON_TEST_MODE_START_MID_INDEX_E:/*value = (32 + index) % 64 (32..63,0..31)*/
                value = ((PRV_TGF_TRUNK_PEARSON_TABLE_SIZE_CNS / 2) + index) % PRV_TGF_TRUNK_PEARSON_TABLE_SIZE_CNS;
                break;
            case PEARSON_TEST_MODE_EVEN_INDEX_E:     /*value = index & 0x3e (0,0,2,2..62,62)*/
                value = index & 0xfffffffe;
                break;
            case PEARSON_TEST_MODE_MODE_8_INDEX_E:   /*value = index & 0x7 (0..7,0..7,0..7,...,0..7)*/
                value = index & 0x7;
                break;
            default:
                return GT_BAD_PARAM;
        }

        rc = prvTgfTrunkHashPearsonValueSet(index,value);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    return GT_OK;
}

/*******************************************************************************
* trunkHashMaskCrcTableEntryClear
*
* DESCRIPTION:
*   clear the single entry or entire table of trunk hash mask CRC
*
*       applicable devices: Lion and above
*
* INPUTS:
*       index - indecx in the table .
*               PRV_TGF_TRUNK_HASH_MASK_TABLE_INDEX_CLEAR_ALL_CNS means clear
*               all table
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkHashMaskCrcTableEntryClear
(
    IN GT_U32              index
)
{
    GT_STATUS   rc;
    PRV_TGF_TRUNK_LBH_CRC_MASK_ENTRY_STC    entry;
    GT_U32   ii,iiStart,iiLast;

    cpssOsMemSet(&entry,0,sizeof(entry));

    if(index == PRV_TGF_TRUNK_HASH_MASK_TABLE_INDEX_CLEAR_ALL_CNS)
    {
        iiStart = 0;
        iiLast = PRV_TGF_TRUNK_HASH_MASK_TABLE_SIZE_CNS;
    }
    else
    {
        iiStart = index;
        iiLast = index + 1;
    }

    for(ii = iiStart ; ii < iiLast; ii++)
    {
        rc = prvTgfTrunkHashMaskCrcEntrySet(ii,&entry);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}
/*******************************************************************************
* trunkHashMaskCrcFieldSet
*
* DESCRIPTION:
*   set single field in the trunk hash mask CRC table
*
*       applicable devices: Lion and above
*
* INPUTS:
*       index - index in the table
*       fieldType - field type
*       fieldSubType - field type sub index
*       fieldByteIndex - byte index in the field
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkHashMaskCrcFieldSet
(
    IN GT_U32              index,
    IN PRV_TGF_TRUNK_FIELD_TYPE_ENT fieldType,
    IN GT_U32              fieldSubType,
    IN GT_U32              fieldByteIndex
)
{
    PRV_TGF_TRUNK_LBH_CRC_MASK_ENTRY_STC    entry;
    GT_U32  *fieldPtr;

    cpssOsMemSet(&entry,0,sizeof(entry));

    switch(fieldType)
    {
        case PRV_TGF_TRUNK_FIELD_L4_PORT_E:
            if(fieldSubType == PRV_TGF_TRUNK_FIELD_DIRECTION_SOURCE_E)
            {
                fieldPtr = &entry.l4SrcPortMaskBmp;
            }
            else
            {
                fieldPtr = &entry.l4DstPortMaskBmp;
            }
            break;
        case PRV_TGF_TRUNK_FIELD_IP_ADDR_E:
            if(fieldSubType == PRV_TGF_TRUNK_FIELD_DIRECTION_SOURCE_E)
            {
                fieldPtr = &entry.ipSipMaskBmp;
            }
            else
            {
                fieldPtr = &entry.ipDipMaskBmp;
            }
            break;
        case PRV_TGF_TRUNK_FIELD_MAC_ADDR_E:
            if(fieldSubType == PRV_TGF_TRUNK_FIELD_DIRECTION_SOURCE_E)
            {
                fieldPtr = &entry.macSaMaskBmp;
            }
            else
            {
                fieldPtr = &entry.macDaMaskBmp;
            }
            break;
        case PRV_TGF_TRUNK_FIELD_MPLS_LABEL_E:
            switch(fieldSubType)
            {
                case 0:
                    fieldPtr = &entry.mplsLabel0MaskBmp;
                    break;
                case 1:
                    fieldPtr = &entry.mplsLabel1MaskBmp;
                    break;
                default:
                    fieldPtr = &entry.mplsLabel2MaskBmp;
                    break;
            }
            break;
        case PRV_TGF_TRUNK_FIELD_IPV6_FLOW_E:
            fieldPtr = &entry.ipv6FlowMaskBmp;
            break;
        case PRV_TGF_TRUNK_FIELD_LOCAL_SRC_PORT_E:
            fieldPtr = &entry.localSrcPortMaskBmp;
            break;
        case PRV_TGF_TRUNK_FIELD_UDBS_E:
            fieldPtr = &entry.udbsMaskBmp;
            break;
        default:
            return GT_BAD_PARAM;
    }

    *fieldPtr = 1 << fieldByteIndex;

    return prvTgfTrunkHashMaskCrcEntrySet(index,&entry);

}

/*******************************************************************************
* trunkSendCheckLbh1
*
* DESCRIPTION:
*       for single destination :
*       check the 'Single destination traffic' did LBH (according to mode)
*       on trunk ports , and check that other ports (out of the trunk) not
*       received flooding
*       for multi-destination :
*       check the 'Multi destination traffic' did LBH (according to mode)
*       on trunk ports , and check that other ports (out of the trunk) also
*       received flooding
*
*       applicable devices: All DxCh devices
*
* INPUTS:
*       singleDestination - is it single/multi destination traffic
*       numberOfPortsSending - number of ports that send bursts
*       burstCount - number of packets in burst
*       lbhMode - expected LBH mode
*       trunkMemberSenderPtr - when not NULL , indicates that the traffic to check
*                   LBH that egress the trunk was originally INGRESSED from the trunk.
*                   but since in the 'enhanced UT' the CPU send traffic to a port
*                   due to loopback it returns to it, we need to ensure that the
*                   member mentioned here should get the traffic since it is the
*                   'original sender'
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void trunkSendCheckLbh1
(
    IN GT_BOOL             singleDestination,
    IN GT_U32              numberOfPortsSending,
    IN GT_U32              burstCount,
    IN PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT  lbhMode,
    IN CPSS_TRUNK_MEMBER_STC *trunkMemberSenderPtr
)
{
    GT_STATUS   rc;
    GT_U8   devNum,portNum;/*device number to send packets , and port number */
    GT_U32  portIter;/* port iterator */
    GT_U32  lbhTolerance;/* LBH tolerance on port */
    GT_TRUNK_ID             trunkId;/* trunkId that should get the 'known UC' */
    CPSS_PORT_MAC_COUNTER_SET_STC   portCntrs;
    CPSS_PORT_MAC_COUNTER_SET_STC   expectedCntrs;
    GT_BOOL                         isEqualCntrs = GT_FALSE;
    GT_U32               totalCount;/* total packets sent count */
    CPSS_TRUNK_MEMBER_STC   trunkMember;/*trunk member*/
    GT_BOOL                 stormingDetected;/*used to check if storming detected
                                relevant only when trunkMemberSenderPtr != NULL */

    totalCount =  numberOfPortsSending * burstCount;
    lbhTolerance = 0;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    prvTgfTrunkLoadBalanceCheck(trunkId,lbhMode,totalCount,lbhTolerance,trunkMemberSenderPtr,&stormingDetected);

    /* check that other ports in vlan got/not the traffic */
    for(portIter = 0 ; portIter < prvTgfPortsNum; portIter++)
    {
        devNum  = prvTgfDevsArray [portIter];
        portNum = prvTgfPortsArray[portIter];

        trunkMember.device = devNum;
        trunkMember.port   = portNum;
        /* skip ports members in the trunk */
        if(GT_OK == prvTgfTrunkDbIsMemberOfTrunk(devNum,&trunkMember,NULL))
        {
            /* trunk member --> skip it */
            continue;
        }

        TRUNK_PRV_UTF_LOG2_MAC("=======  : dev[%d]port[%d] read counters  =======\n",
            devNum, portNum);
        /* read counters */
        rc = prvTgfReadPortCountersEth(devNum, portNum, GT_TRUE, &portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, rc, "prvTgfReadPortCountersEth: %d, %d",
                                     devNum, portNum);

        /* copy read counters to  expected counters */
        cpssOsMemCpy(&expectedCntrs, &portCntrs, sizeof(expectedCntrs));


        if(singleDestination == GT_TRUE)
        {
            if(portIter < numberOfPortsSending)
            {
                /* this port was egress traffic for the 'loopback' to ingress */
                expectedCntrs.goodPktsSent.l[0] = burstCount;
            }
            else
            {
                /* no packet should egress the ports that not sent traffic */
                expectedCntrs.goodPktsSent.l[0]   = 0;
            }
        }
        else
        {
            /* those ports need to be flooded for each packet sent from other ports */
            if(portIter < numberOfPortsSending)
            {
                /* this port will egress from other ports that send and also the 'loopback' to ingress*/
                expectedCntrs.goodPktsSent.l[0] = burstCount * numberOfPortsSending;
            }
            else
            {
                /* this port will egress from other ports that send */
                expectedCntrs.goodPktsSent.l[0] = burstCount * numberOfPortsSending;
            }
        }

        if(stormingDetected == GT_TRUE)
        {
            /* when storming detected , the ports get a lot of packets . */
            isEqualCntrs = GT_TRUE;

            if(expectedCntrs.goodPktsSent.l[0])
            {
                /* we expect a lot of packets */
                if(portCntrs.goodPktsSent.l[0] == 0)
                {
                    /* but we got nothing */
                    isEqualCntrs = GT_FALSE;
                }
            }
            else
            {
                /* we expected 0 , so even if storming still should be 0 */
                if(portCntrs.goodPktsSent.l[0] != 0)
                {
                    /* but got packets */
                    isEqualCntrs = GT_FALSE;
                }
            }



            UTF_VERIFY_EQUAL0_STRING_MAC(isEqualCntrs, GT_TRUE, "stormingDetected : get another counters values.");
            continue;
        }

        /* check Rx\Tx counters */
        PRV_TGF_VERIFY_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
        UTF_VERIFY_EQUAL2_STRING_MAC(isEqualCntrs, GT_TRUE, "get another counters values on dev[%d],port[%d]",devNum, portNum);

        /* print expected values if not equal */
        PRV_TGF_PRINT_DIFF_COUNTERS_MAC(isEqualCntrs, expectedCntrs, portCntrs);
    }
}
/*******************************************************************************
* trunkSendCheckLbh
*
* DESCRIPTION:
*       for single destination :
*       check the 'Single destination traffic' did LBH (according to mode)
*       on trunk ports , and check that other ports (out of the trunk) not
*       received flooding
*       for multi-destination :
*       check the 'Multi destination traffic' did LBH (according to mode)
*       on trunk ports , and check that other ports (out of the trunk) also
*       received flooding
*
*       applicable devices: All DxCh devices
*
* INPUTS:
*       singleDestination - is it single/multi destination traffic
*       numberOfPortsSending - number of ports that send bursts
*       burstCount - number of packets in burst
*       lbhMode - expected LBH mode
*       trunkMemberSenderPtr - when not NULL , indicates that the traffic to check
*                   LBH that egress the trunk was originally INGRESSED from the trunk.
*                   but since in the 'enhanced UT' the CPU send traffic to a port
*                   due to loopback it returns to it, we need to ensure that the
*                   member mentioned here should get the traffic since it is the
*                   'original sender'
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void trunkSendCheckLbh
(
    IN GT_BOOL             singleDestination,
    IN GT_U32              numberOfPortsSending,
    IN GT_U32              burstCount,
    IN PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT  lbhMode,
    IN CPSS_TRUNK_MEMBER_STC *trunkMemberSenderPtr
)
{
    GT_U32  portIter;/* port iterator */
    GT_U8   devNum,portNum;/*device number to send packets , and port number */

    trunkSendCheckLbh1(singleDestination,numberOfPortsSending,burstCount,lbhMode,trunkMemberSenderPtr);

    /* make sure to clean ALL the counters of all the ports , in case that the function
        trunkSendCheckLbh1 failed in the middle and the 'fContinue flag was GT_FALSE' */
    for(portIter = 0 ; portIter < prvTgfPortsNum; portIter++)
    {
        devNum  = prvTgfDevsArray [portIter];
        portNum = prvTgfPortsArray[portIter];

        TRUNK_PRV_UTF_LOG2_MAC("=======  : dev[%d]port[%d] reset counters  =======\n",
            devNum, portNum);
        /* reset counters */
        prvTgfResetCountersEth(devNum, portNum);
    }
}


/*******************************************************************************
* trunkHashCrcLbhTrafficSend
*
* DESCRIPTION:
*   send traffic according to field checking
*
*       applicable devices: Lion and above
*
* INPUTS:
*       trafficPtr  - (pointer to) traffic to send
*       trafficType - traffic type
*       maskCrcTableFieldType - field type
*       maskCrcTableFieldSubType - field type sub index
*       maskCrcTableFieldByteIndex - byte index in the field
*       lbhMode - LBH expected
* OUTPUTS:
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkHashCrcLbhTrafficSend
(
    IN TGF_PACKET_STC      *trafficPtr,
    IN PRV_TGF_PCL_PACKET_TYPE_ENT     trafficType,
    IN PRV_TGF_TRUNK_FIELD_TYPE_ENT fieldType,
    IN GT_U32              fieldSubType,
    IN GT_U32              fieldByteIndex,
    IN PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT  lbhMode
)
{
    GT_STATUS   rc;
    GT_U8   devNum,portNum;/*device number to send packets , and port number */
    GT_BOOL isL4,isMpls,isIpv4,isIpv6,isUde;/* flags */
    GT_BOOL srcPortTested;/* is src port tested */
    TGF_PACKET_PAYLOAD_STC *payloadPtr;/* pointer to payload info of packet */
    GT_U32  offset;/* offset of byte from start of packet */
    GT_U32  bitOffset;/*offset of bit from start of byte */
    GT_U32  numBits;/*number of bits in the tested field */
    GT_U32  numBitsInLastByte;/*number of bits in the last byte of tested field */
    GT_U32               burstCount;/* burst count */
    GT_U32               numVfd = 1;/* number of VFDs in vfdArray */
    TGF_VFD_INFO_STC     vfdArray[1];/* vfd Array -- used for increment the tested bytes , and for vlan tag changing */
    TGF_VFD_INFO_STC     *vrfPtr = &vfdArray[0];
    GT_U32  ii;
    GT_U32  portIter;/* port iterator */
    GT_U32  numberOfPortsSending;/* number of ports sending traffic */
    GT_U32  numVlans;/* number of vlans to test */

    isL4 = GT_FALSE;
    isMpls = GT_FALSE;
    isIpv4 = GT_FALSE;
    isIpv6 = GT_FALSE;
    isUde = GT_FALSE;
    srcPortTested = GT_FALSE;
    bitOffset = 0;
    numVlans = 1;

    switch(trafficType)
    {
        case PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E      :
        case PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E      :
            isL4 = GT_TRUE;
            isIpv4 = GT_TRUE;
            break;
        case PRV_TGF_PCL_PACKET_TYPE_MPLS_E          :
            isMpls = GT_TRUE;
            break;
        case PRV_TGF_PCL_PACKET_TYPE_IPV4_FRAGMENT_E :
        case PRV_TGF_PCL_PACKET_TYPE_IPV4_OTHER_E    :
            isIpv4 = GT_TRUE;
            break;
        case PRV_TGF_PCL_PACKET_TYPE_ETHERNET_OTHER_E :
            break;
        case PRV_TGF_PCL_PACKET_TYPE_IPV6_E          :
            isIpv6 = GT_TRUE;
            break;
        case PRV_TGF_PCL_PACKET_TYPE_UDE_E           :
        case PRV_TGF_PCL_PACKET_TYPE_UDE_1_E         :
        case PRV_TGF_PCL_PACKET_TYPE_UDE_2_E         :
        case PRV_TGF_PCL_PACKET_TYPE_UDE_3_E         :
        case PRV_TGF_PCL_PACKET_TYPE_UDE_4_E         :
            isUde = GT_TRUE;

            /* update the ether type field */
            if(trafficType != PRV_TGF_PCL_PACKET_TYPE_UDE_E)
            {
                prvTgfPacketUdeTypePart.etherType = (TGF_ETHER_TYPE)(UDE_BASE_CNS +
                    (trafficType - PRV_TGF_PCL_PACKET_TYPE_UDE_1_E) + 1);
            }
            else
            {
                prvTgfPacketUdeTypePart.etherType = UDE_BASE_CNS;
            }
            break;
        default:
            return GT_BAD_PARAM;
    }

    switch(fieldType)
    {
        case PRV_TGF_TRUNK_FIELD_L4_PORT_E:
            RETURN_IF_FIELD_NOT_RELEVANT_MAC(isL4);
            RETURN_IF_FIELD_NOT_RELEVANT_MAC((fieldByteIndex <= 2));

            numBits = 16;

            offset = TGF_L2_HEADER_SIZE_CNS +/*l2*/
                     TGF_VLAN_TAG_SIZE_CNS  +/*vlan tag*/
                     TGF_ETHERTYPE_SIZE_CNS +/*ethertype*/
                     TGF_IPV4_HEADER_SIZE_CNS;/*ipv4 header*/

            if(fieldSubType == PRV_TGF_TRUNK_FIELD_DIRECTION_DESTINATION_E)
            {
                offset += 2;
            }

            offset += fieldByteIndex;

            break;
        case PRV_TGF_TRUNK_FIELD_IPV6_FLOW_E:
            RETURN_IF_FIELD_NOT_RELEVANT_MAC(isIpv6);
            RETURN_IF_FIELD_NOT_RELEVANT_MAC((fieldByteIndex <= 2));

            numBits = 20;

            offset = TGF_L2_HEADER_SIZE_CNS +/*l2*/
                     TGF_VLAN_TAG_SIZE_CNS  +/*vlan tag*/
                     TGF_ETHERTYPE_SIZE_CNS +/*ethertype*/
                     1;/*start in mid of second byte in ipv6 header */

            offset += fieldByteIndex;
            break;
        case PRV_TGF_TRUNK_FIELD_IP_ADDR_E:
            RETURN_IF_FIELD_NOT_RELEVANT_MAC((isIpv6 || isIpv4));
            RETURN_IF_FIELD_NOT_RELEVANT_MAC((fieldByteIndex < 16));

            offset = TGF_L2_HEADER_SIZE_CNS +/*l2*/
                     TGF_VLAN_TAG_SIZE_CNS  +/*vlan tag*/
                     TGF_ETHERTYPE_SIZE_CNS ;/*ethertype*/

            if(isIpv4 == GT_TRUE)
            {
                /* the Ipv4 bytes start from 12..15 , where : 12 is MSB for Ipv4 */
                RETURN_IF_FIELD_NOT_RELEVANT_MAC((fieldByteIndex >= IPV4_OFFSET_FROM_BYTE_12_CNS));

                numBits = 32;

                offset += 12;
                if(fieldSubType == PRV_TGF_TRUNK_FIELD_DIRECTION_DESTINATION_E)
                {
                    offset += 4;
                }

                offset += fieldByteIndex - IPV4_OFFSET_FROM_BYTE_12_CNS;
            }
            else /* ipv6 */
            {
                numBits = 128;

                offset += 8;
                if(fieldSubType == PRV_TGF_TRUNK_FIELD_DIRECTION_DESTINATION_E)
                {
                    offset += 16;
                }

                offset += fieldByteIndex;
            }
            break;
        case PRV_TGF_TRUNK_FIELD_MAC_ADDR_E:

            numBits = 48;

            offset = 0;
            if(fieldSubType == PRV_TGF_TRUNK_FIELD_DIRECTION_SOURCE_E)
            {
                offset += 6;
            }
            offset += fieldByteIndex;

            break;
        case PRV_TGF_TRUNK_FIELD_MPLS_LABEL_E:
            RETURN_IF_FIELD_NOT_RELEVANT_MAC(isMpls);
            RETURN_IF_FIELD_NOT_RELEVANT_MAC((fieldByteIndex <= 2));

            numBits = 20;/* field of 20 bits , 4 bits in last byte */
            bitOffset = 4;

            offset = TGF_L2_HEADER_SIZE_CNS +/*l2*/
                     TGF_VLAN_TAG_SIZE_CNS  +/*vlan tag*/
                     TGF_ETHERTYPE_SIZE_CNS ;/*ethertype*/

            switch(fieldSubType)
            {
                case 0:
                case 1:
                case 2:
                    break;
                default:
                    return GT_BAD_PARAM;

            }

            offset += 4 * fieldSubType; /* 4 bytes offset for each label */

            offset += fieldByteIndex;

            break;
        case PRV_TGF_TRUNK_FIELD_LOCAL_SRC_PORT_E:
            srcPortTested = GT_TRUE;
            numBits = 6;
            offset = 0;/* use the mac DA field */
            break;

        case PRV_TGF_TRUNK_FIELD_UDBS_E:

            if(trafficPtr->numOfParts < 1)
            {
                /*prevent access violation*/
                return GT_BAD_PARAM;
            }

            if(TGF_PACKET_PART_PAYLOAD_E != trafficPtr->partsArray[trafficPtr->numOfParts - 1].type)
            {
                /* assume payload is the last part of the packet */
                return GT_BAD_PARAM;
            }

            numBits = 8;

            /* pointer to the payload of the packet */
            payloadPtr = trafficPtr->partsArray[trafficPtr->numOfParts - 1].partPtr;
            /* calculate the number of bytes from the start of the packet */
            offset = trafficPtr->totalLen - payloadPtr->dataLength;

            rc = prvTgfPclUserDefinedByteSet(0/* not relevant */ ,
                trafficType , fieldByteIndex  , PRV_TGF_PCL_OFFSET_L2_E ,
                (GT_U8)offset);
            if(rc != GT_OK)
            {
                if(debugMode)
                {
                    cpssOsPrintf(" debug ERROR ---> prvTgfPclUserDefinedByteSet failed \n");
                }
                return rc;
            }

            break;

        default:
            return GT_BAD_PARAM;
    }

    numBitsInLastByte = numBits & 0x7;

    if(fieldType == PRV_TGF_TRUNK_FIELD_MAC_ADDR_E &&
       fieldSubType == PRV_TGF_TRUNK_FIELD_DIRECTION_DESTINATION_E)
    {
        /* according to the FDB learned MACs */
        burstCount = FDB_MAC_COUNT_CNS;
    }
    else
    {
        burstCount = CRC_HASH_BURST_COUNT_NEW_CNS;
    }

    vrfPtr->modeExtraInfo = 0;
    vrfPtr->offset = offset;
    cpssOsMemSet(vrfPtr->patternPtr,0,sizeof(TGF_MAC_ADDR));
    vrfPtr->patternPtr[0] = 0;
    vrfPtr->cycleCount = 1;/*single byte*/
    if(bitOffset == 0)
    {
        vrfPtr->mode = TGF_VFD_MODE_INCREMENT_E;

        if(numBitsInLastByte && (((numBits - 1) / 8) == fieldByteIndex))
        {
            /* we do last byte */
            burstCount = 1 << numBitsInLastByte;
        }
    }
    else
    {
        vrfPtr->mode = TGF_VFD_MODE_INCREMENT_VALUE_E;
        vrfPtr->incValue = (1 << bitOffset);

        if(fieldByteIndex != 0 && vrfPtr->offset != 0)
        {
            /* due to usage of 2 bytes set offset as previous byte */
            vrfPtr->offset--;
            vrfPtr->cycleCount = 2;/* 2 bytes */
        }
        else
        {
            burstCount = 1 << (8 - bitOffset) ;
        }
    }

    if(burstCount > CRC_HASH_BURST_COUNT_NEW_CNS)
    {
        burstCount = CRC_HASH_BURST_COUNT_NEW_CNS;
    }

    if(debug_burstNum)
    {
        burstCount = debug_burstNum;
    }

    if(srcPortTested == GT_TRUE)
    {
        /* 4 ports will send the traffic */
        numberOfPortsSending = NOT_IN_TRUNK_PORTS_NUM_CNS;
        burstCount = 1;
    }
    else
    {
        /* single port will sent the traffic */
        numberOfPortsSending = 1;
    }



    for(portIter = 0 ; portIter < numberOfPortsSending; portIter++)
    {
        devNum  = prvTgfDevsArray [portIter];
        portNum = prvTgfPortsArray[portIter];

        for(ii = 0 ; ii < numVlans ; ii++)
        {
            /* modify the vid part */
            prvTgfPacketVlanTagPart.vid = (TGF_VLAN_ID)prvTgfVlansArr[ii];

            vrfPtr->modeExtraInfo = 0;
            /* send the burst of packets with incremental byte */
            prvTgfTrunkTestPacketSend(devNum, portNum, trafficPtr ,burstCount ,numVfd ,vfdArray);
        }
    }

    /* check the LBH and no flooding */
    trunkSendCheckLbh(GT_TRUE,numberOfPortsSending,burstCount * numVlans ,lbhMode,NULL);

    return GT_OK;
}


/*******************************************************************************
* prvTgfTrunkCrcHashTest
*
* DESCRIPTION:
*   check CRC hash feature.
*
*       applicable devices: Lion and above
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
void prvTgfTrunkCrcHashTest
(
    void
)
{
    GT_STATUS   rc;
    PRV_TGF_TRUNK_LBH_CRC_MODE_ENT  crcMode;
    GT_U32              seedIndex,numSeeds,seedMask,currentSeed;
    PEARSON_TEST_MODE_ENT   pearsonMode,pearsonModeLast;
    GT_U32              hashMaskTableIndex;/* index of entry in hash mask table - 0..27 */
    PRV_TGF_TRUNK_FIELD_TYPE_ENT hashMaskFieldType;/* index of field in the 70 bytes for hash - 0..69 */
    GT_U32              hashMaskFieldSubType,hashMaskFieldSubTypeStart,hashMaskFieldSubTypeEnd;
    GT_U32              hashMaskFieldByteIndex,hashMaskFieldByteIndexStart,hashMaskFieldByteIndexEnd;
    PRV_TGF_PCL_PACKET_TYPE_ENT     trafficType;/*traffic type*/
    PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT  lbhMode;/* LBH mode expected */

    if(debugMode)
    {
        cpssOsPrintf("debug_hashMaskTableIndex = %d \n",debug_hashMaskTableIndex);
        cpssOsPrintf("debug_hashMaskFieldType = %d %s \n",debug_hashMaskFieldType,
            TO_STRING_HASH_MASK_FIELD_TYPE_MAC(debug_hashMaskFieldType));
        switch(debug_hashMaskFieldType)
        {
            case PRV_TGF_TRUNK_FIELD_L4_PORT_E:
            case PRV_TGF_TRUNK_FIELD_IP_ADDR_E:
            case PRV_TGF_TRUNK_FIELD_MAC_ADDR_E:
                cpssOsPrintf("debug_hashMaskFieldSubType = %d %s \n",debug_hashMaskFieldSubType,
                TO_STRING_HASH_MASK_FIELD_SUB_TYPE_MAC(debug_hashMaskFieldSubType));
                break;
            default:
                cpssOsPrintf("debug_hashMaskFieldSubType = %d \n",debug_hashMaskFieldSubType);
                break;
        }
        cpssOsPrintf("debug_hashMaskFieldByteIndex = %d \n",debug_hashMaskFieldByteIndex);
        cpssOsPrintf("debug_pearsonMode = %d %s\n",debug_pearsonMode,
            TO_STRING_PEARSONE_MODE_MAC(debug_pearsonMode));

        cpssOsPrintf("debug_crcMode = %d , %s \n",debug_crcMode,
            TO_STRING_CRC_MODE_MAC(debug_crcMode));

        cpssOsPrintf("debug_seedIndex = %d \n",debug_seedIndex);
    }

    for(crcMode = PRV_TGF_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E ;
        crcMode >= PRV_TGF_TRUNK_LBH_CRC_6_MODE_E;
        crcMode--)
    {/* level 1 */
        if(debugMode)
        {
            crcMode = debug_crcMode;
        }

        if(crcMode == PRV_TGF_TRUNK_LBH_CRC_6_MODE_E)
        {
            numSeeds = SEED_CRC_6_NUM_CNS;/* the last 2 values are redundant for crc_6 */
            seedMask = BIT_6 - 1;/* 6 bits mask */
            pearsonModeLast = 1;
        }
        else
        {
            numSeeds = SEED_CRC_16_NUM_CNS;
            seedMask = BIT_16 - 1;/* 16 bits mask */
            pearsonModeLast = PEARSON_TEST_MODE_LAST_E;
        }

#ifdef FORCE_REDUCED_TEST_MAC
        {
            /* for debug */
            CPSS_TBD_BOOKMARK   /*--> to be removed when stable */
            numSeeds = 1;
        }
#endif /*FORCE_REDUCED_TEST_MAC*/

        for(seedIndex = 0 ;
            seedIndex < numSeeds ;
            seedIndex++)
        {/* level 2 */
            if(debugMode)
            {
                seedIndex = debug_seedIndex;
            }

            currentSeed = crcSeeds[seedIndex] & seedMask;

            /* set new mode and seed */
            TRUNK_PRV_UTF_LOG0_MAC("=======  : set CRC sub-mode and seed =======\n");
            rc = prvTgfTrunkHashCrcParametersSet(crcMode,currentSeed);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

            TRUNK_PRV_UTF_LOG0_MAC("=======  : clear mask hash CRC table =======\n");
            /* clear full table */
            rc = trunkHashMaskCrcTableEntryClear(PRV_TGF_TRUNK_HASH_MASK_TABLE_INDEX_CLEAR_ALL_CNS);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

            for(hashMaskTableIndex = PRV_TGF_TRUNK_HASH_MASK_TABLE_TRAFFIC_TYPE_FIRST_INDEX_CNS,
                trafficType = 0;
                hashMaskTableIndex < PRV_TGF_TRUNK_HASH_MASK_TABLE_SIZE_CNS;
                hashMaskTableIndex++,trafficType++)
            {/* level 3 */
                for(hashMaskFieldType = 0 ;
                    hashMaskFieldType < PRV_TGF_TRUNK_FIELD_LAST_E;
                    hashMaskFieldType++)
                {/* level 4 */
                    hashMaskFieldSubTypeStart = 0;
                    hashMaskFieldSubTypeEnd   = 0;

                    hashMaskFieldByteIndexStart = 0;
                    hashMaskFieldByteIndexEnd = 1;

                    switch(hashMaskFieldType)
                    {
                        case PRV_TGF_TRUNK_FIELD_L4_PORT_E:
                            hashMaskFieldSubTypeStart = PRV_TGF_TRUNK_FIELD_DIRECTION_SOURCE_E;
                            hashMaskFieldSubTypeEnd   = PRV_TGF_TRUNK_FIELD_DIRECTION_DESTINATION_E;
                            hashMaskFieldByteIndexEnd = 2; /* 2 bytes */
                            break;
                        case PRV_TGF_TRUNK_FIELD_IP_ADDR_E:
                            hashMaskFieldSubTypeStart = PRV_TGF_TRUNK_FIELD_DIRECTION_SOURCE_E;
                            hashMaskFieldSubTypeEnd   = PRV_TGF_TRUNK_FIELD_DIRECTION_DESTINATION_E;
                            hashMaskFieldByteIndexEnd = 16;/* 16 bytes */
                            break;
                        case PRV_TGF_TRUNK_FIELD_MAC_ADDR_E:
                            hashMaskFieldSubTypeStart = PRV_TGF_TRUNK_FIELD_DIRECTION_SOURCE_E;
                            hashMaskFieldSubTypeEnd   = PRV_TGF_TRUNK_FIELD_DIRECTION_DESTINATION_E;
                            hashMaskFieldByteIndexEnd = 6;/* 6 bytes */
                            break;
                        case PRV_TGF_TRUNK_FIELD_MPLS_LABEL_E:
                            hashMaskFieldSubTypeEnd = 2;  /* labels 0,1,2 */
                            hashMaskFieldByteIndexEnd = 3;/* 3 bytes */
                            break;
                        case PRV_TGF_TRUNK_FIELD_UDBS_E:
                            hashMaskFieldByteIndexStart = 14; /* start UDB 14 */
                            hashMaskFieldByteIndexEnd = 23;  /* end UDB 22 */
                            break;
                        default:
                            break;
                    }

                    for(hashMaskFieldSubType = hashMaskFieldSubTypeStart;
                        hashMaskFieldSubType <= hashMaskFieldSubTypeEnd;
                        hashMaskFieldSubType++)
                    {/* level 5 */

                        for(hashMaskFieldByteIndex = hashMaskFieldByteIndexStart;
                            hashMaskFieldByteIndex < hashMaskFieldByteIndexEnd;
                            hashMaskFieldByteIndex++)
                        {/* level 6 */

                            if(hashMaskFieldType == PRV_TGF_TRUNK_FIELD_MAC_ADDR_E &&
                               hashMaskFieldByteIndex == 0)
                            {
                                /*For DA :  this is the MSB of the DA , and we not set
                                   learning on this bytes because half of them
                                   are 'MCAST' */

                                /* For SA : this is the MSB of the SA , and
                                   the device not bridge SA which is 'MCAST' */

                                continue;
                            }

                            if(hashMaskFieldType == PRV_TGF_TRUNK_FIELD_IP_ADDR_E &&
                               hashMaskFieldByteIndex == 0)
                            {
                                /*For DIP :  this is the MSB of the DIP
                                    value >= 224 is IPMC (but the DA is not matching) */

                                /* For SIP : this is the MSB of the SIP,
                                    value >= 224 is IPMC (not allowed) */

                                continue;
                            }

                            if(debugMode)
                            {
                                hashMaskTableIndex      = debug_hashMaskTableIndex;
                                hashMaskFieldType       = debug_hashMaskFieldType;
                                hashMaskFieldSubType    = debug_hashMaskFieldSubType;
                                hashMaskFieldByteIndex  = debug_hashMaskFieldByteIndex;
                                if(debug_hashMaskTableIndex >= PRV_TGF_TRUNK_HASH_MASK_TABLE_TRAFFIC_TYPE_FIRST_INDEX_CNS)
                                {
                                    trafficType = debug_hashMaskTableIndex -
                                                PRV_TGF_TRUNK_HASH_MASK_TABLE_TRAFFIC_TYPE_FIRST_INDEX_CNS;
                                }
                                else
                                {
                                    trafficType = 0;
                                }
                            }

                            rc = trunkHashMaskCrcFieldSet(
                                    hashMaskTableIndex,
                                    hashMaskFieldType,
                                    hashMaskFieldSubType,
                                    hashMaskFieldByteIndex);
                            if(rc != GT_OK)
                            {
                                UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
                                break;
                            }

                            for(pearsonMode = 0 ;
                                pearsonMode < pearsonModeLast ;
                                pearsonMode++)
                            {/* level 7 */

                                if(debugMode)
                                {
                                    pearsonMode = debug_pearsonMode;
                                }

                                /* expect good LBH */
                                lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EVEN_E;

                                if(crcMode == PRV_TGF_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E)
                                {
                                    switch(pearsonMode)
                                    {
                                        case PEARSON_TEST_MODE_0_E:
                                            /* no LBH expected */
                                            lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E;
                                            break;
                                        case PEARSON_TEST_MODE_EVEN_INDEX_E:
                                            /* LBH only on %<numTrunkMemebers> = 0 or 2 members of the trunk */
                                            /* because we use 4 members in the trunk then
                                               only member in index 0,2 will get traffic while
                                               members in index 1,3 will not get it */
                                            lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E;
                                            break;
                                        default:
                                            break;
                                    }

                                    /*****************/
                                    /* special cases */
                                    /*****************/
                                    if(lbhMode == PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E)
                                    {
                                        /* forced to single port */
                                    }
                                    else if(hashMaskFieldType == PRV_TGF_TRUNK_FIELD_LOCAL_SRC_PORT_E)
                                    {
                                        /* it seems that the Pearson hash will
                                           result ONLY 2 values and both of them
                                           with same %4

                                           so because our trunk with 4 members only
                                           single port receive the traffic !
                                        */
                                        lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E;
                                        prvTgfTrunkLoadBalanceLimitedNumSet(1);
                                    }
                                    else if (hashMaskFieldType == PRV_TGF_TRUNK_FIELD_UDBS_E &&
                                             hashMaskFieldByteIndex == 15)
                                    {
                                        /* it seems that the hash goes to only
                                           2 ports !

                                           NOTE: other UDBs (14,16..22) are ok !
                                        */

                                        lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E;
                                        prvTgfTrunkLoadBalanceLimitedNumSet(2);
                                    }
                                    else if(hashMaskFieldType == PRV_TGF_TRUNK_FIELD_MPLS_LABEL_E &&
                                       hashMaskFieldSubType == 0 && /* label 0 */
                                       hashMaskFieldByteIndex == 0)/* byte 0 */
                                    {
                                        /* it seems that the hash goes to only
                                           2 ports !

                                           NOTE: other labels (0,2) are ok !
                                        */
                                        lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E;
                                        prvTgfTrunkLoadBalanceLimitedNumSet(2);

                                        if(pearsonMode == PEARSON_TEST_MODE_EVEN_INDEX_E)
                                        {
                                            /* it seems that the 'even values in the Pearson'
                                               combined with the fact that only 2 ports will
                                               gets be reduced to only 1 port in the trunk !!!
                                            */
                                            prvTgfTrunkLoadBalanceLimitedNumSet(1);
                                        }

                                    }
                                }

                                {
                                    utfGeneralStateMessageSave(0,"hashMaskTableIndex = %d",hashMaskTableIndex);
                                    utfGeneralStateMessageSave(1,"hashMaskFieldType = %d %s ",hashMaskFieldType,
                                        TO_STRING_HASH_MASK_FIELD_TYPE_MAC(hashMaskFieldType));
                                    switch(hashMaskFieldType)
                                    {
                                        case PRV_TGF_TRUNK_FIELD_L4_PORT_E:
                                        case PRV_TGF_TRUNK_FIELD_IP_ADDR_E:
                                        case PRV_TGF_TRUNK_FIELD_MAC_ADDR_E:
                                            utfGeneralStateMessageSave(2,"hashMaskFieldSubType = %d %s ",hashMaskFieldSubType,
                                                TO_STRING_HASH_MASK_FIELD_SUB_TYPE_MAC(hashMaskFieldSubType));
                                        break;
                                        default:
                                            utfGeneralStateMessageSave(2,"hashMaskFieldSubType = %d ",hashMaskFieldSubType);
                                        break;
                                    }
                                    utfGeneralStateMessageSave(3,"hashMaskFieldByteIndex = %d ",hashMaskFieldByteIndex);
                                    utfGeneralStateMessageSave(4,"pearsonMode = %d %s ",pearsonMode,
                                        TO_STRING_PEARSONE_MODE_MAC(pearsonMode));

                                    utfGeneralStateMessageSave(5,"crcMode = %d , %s ",crcMode,
                                        TO_STRING_CRC_MODE_MAC(crcMode));

                                    utfGeneralStateMessageSave(6,"seedIndex = %d ",seedIndex);
                                    utfGeneralStateMessageSave(7,"lbhMode = %d %s",lbhMode,
                                        TO_STRING_LBH_MODE_MAC(lbhMode)
                                        );
                                }

                                /* set Pearson table */
                                rc = trunkHashPearsonFill(pearsonMode);
                                UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
                                if(rc != GT_OK)
                                {
                                    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
                                    break;
                                }

                                /* send traffic according to field checking */
                                rc = trunkHashCrcLbhTrafficSend(
                                        prvTgfTrunkTrafficTypes[trafficType],
                                        trafficType,
                                        hashMaskFieldType,
                                        hashMaskFieldSubType,
                                        hashMaskFieldByteIndex,
                                        lbhMode);
                                if(rc != GT_OK)
                                {
                                    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
                                    break;
                                }


#ifdef FORCE_REDUCED_TEST_MAC
                                if(crcMode == PRV_TGF_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E)
                                {
                                    /* for debug */
                                    CPSS_TBD_BOOKMARK   /*--> to be removed when stable */
                                    break;
                                }
#endif /*FORCE_REDUCED_TEST_MAC*/

                                IF_DEBUG_MODE_BREAK_MAC(debugMode);
                            }/* level 7 */
                            IF_DEBUG_MODE_BREAK_MAC(debugMode);
                        }/* level 6 */
                        IF_DEBUG_MODE_BREAK_MAC(debugMode);
                    }/* level 5 */
                    IF_DEBUG_MODE_BREAK_MAC(debugMode);
                }/* level 4 */
                IF_DEBUG_MODE_BREAK_MAC(debugMode);
            }/* level 3 */
            IF_DEBUG_MODE_BREAK_MAC(debugMode);
        }/* level 2 */
        IF_DEBUG_MODE_BREAK_MAC(debugMode);
    }/* level 1 */
}

/*******************************************************************************
* prvTgfTrunkGenericInit
*
* DESCRIPTION:
*   generic init for trunk testss
*
*       applicable devices: All DxCh devices
*
* INPUTS:
*       testType - test type
*       cascadeTrunk - do we create cascade trunk / regular trunk
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void prvTgfTrunkGenericInit
(
    IN TRUNK_TEST_TYPE_ENT testType,
    IN GT_BOOL cascadeTrunk
)
{
    GT_STATUS   rc;
    GT_U32  ii,jj,kk;
    GT_U32  portIter;/* port iterator */
    CPSS_INTERFACE_INFO_STC portInterface;
    CPSS_TRUNK_MEMBER_STC member;
    GT_TRUNK_ID             trunkId;
    PRV_TGF_BRG_MAC_ENTRY_STC      macEntry = PRV_TGF_BRG_FDB_MAC_ENTRY_DEFAULT_MAC;
    CPSS_PORTS_BMP_STC  portsBmp;
    GT_U32              membersIndexesBmp = 0;
    GT_BOOL             isPortConfigForced = GT_FALSE;


    /* check if the device has ports that used with force configuration */
    isPortConfigForced = prvTgfCommonIsDeviceForce(prvTgfDevNum);

    /* save first 4 ports of test */
    for(portIter = 0 ; portIter < TRUNK_8_PORTS_IN_TEST_CNS ; portIter++)
    {
        first4PortsOrig[portIter]  = prvTgfPortsArray[portIter];
        if(GT_TRUE == prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E | UTF_XCAT2_E))
        {
            if(0 == PRV_CPSS_PP_MAC(prvTgfDevNum)->revision &&
               testType == TRUNK_TEST_TYPE_CASCADE_TRUNK_E)
            {
                /* lion A */
                /* ports of cascade trunk - in the same port group -- limitation for Lion A */
                prvTgfPortsArray[portIter] = (GT_U8) ((GT_TRUE == isPortConfigForced) ? testPortsFor60PortsConfigForced_cascadeTrunkPortsSamePortGroup[portIter]
                                                                                      : testPortsFor60Ports_cascadeTrunkPortsSamePortGroup[portIter]);
            }
            else
            {
                prvTgfPortsArray[portIter] = (GT_U8) ((GT_TRUE == isPortConfigForced) ? testPortsFor60PortsConfigForced[portIter]
                                                                                      : testPortsFor60Ports[portIter]);
            }
        }
        else
        {
            if (testType == TRUNK_TEST_TYPE_CRC_HASH_E)
            {
                prvTgfPortsArray[portIter] = crcTestPortsFor28Ports[portIter];
            }
            else
            {
                prvTgfPortsArray[portIter] = testPortsFor28Ports[portIter];
            }
        }
    }

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* save original L2 header */
    prvTgfPacketL2PartOrig = prvTgfPacketL2Part;
    /* save original Vlan tag header */
    prvTgfPacketVlanTagPartOrig = prvTgfPacketVlanTagPart;

    /* map input params */
    portInterface.type            = CPSS_INTERFACE_PORT_E;

    /* update the number of ports in test */
    prvTgfPortsNum = TRUNK_8_PORTS_IN_TEST_CNS;

    TRUNK_PRV_UTF_LOG0_MAC("======= clear counters =======\n");
    /* clear also counters at end of test */
    for(portIter = 0; portIter < prvTgfPortsNum ; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevsArray[portIter],
                                    prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
    }

    TRUNK_PRV_UTF_LOG0_MAC("======= force links up =======\n");

    /* force linkup on all ports involved */
    for(portIter = 0; portIter < prvTgfPortsNum ; portIter++)
    {
        portInterface.devPort.devNum  = prvTgfDevsArray[portIter];
        portInterface.devPort.portNum = prvTgfPortsArray[portIter];

        rc = tgfTrafficGeneratorPortForceLinkUpEnableSet(&portInterface,GT_TRUE);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
    }

    if(cascadeTrunk == GT_FALSE)
    {
        rc = prvTgfTrunkMembersSet(
            trunkId /*trunkId*/,
            0 /*numOfEnabledMembers*/,
            (CPSS_TRUNK_MEMBER_STC*)NULL /*enabledMembersArray*/,
            0 /*numOfDisabledMembers*/,
            (CPSS_TRUNK_MEMBER_STC*)NULL  /*disabledMembersArray*/);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

        for(portIter = NOT_IN_TRUNK_PORTS_NUM_CNS; portIter < prvTgfPortsNum ; portIter++)
        {
            member.device = prvTgfDevsArray [portIter];
            member.port   = prvTgfPortsArray[portIter];

            rc = prvTgfTrunkMemberAdd(trunkId,&member);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

            if(((portIter - NOT_IN_TRUNK_PORTS_NUM_CNS) & 1) == 0)
            {
                /* the first and third .. members */
                /* used when mode is PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E */
                U32_SET_FIELD_MAC(membersIndexesBmp,portIter,1,1);
            }
        }

        prvTgfTrunkLoadBalanceSpecificMembersSet(membersIndexesBmp);

        macEntry.dstInterface.type = CPSS_INTERFACE_TRUNK_E;
        macEntry.dstInterface.trunkId = trunkId;

        /* make sure that ALL the mac DA that will be sent during the test will
           be known in the FDB on the needed vlans */
        for(ii = 0 ; ii < prvTgfVlansNum ; ii++)
        {
            macEntry.key.key.macVlan.vlanId = prvTgfVlansArr[ii];
            for(jj = 0; jj < 6; jj++)
            {
                macEntry.key.key.macVlan.macAddr.arEther[jj] =
                    prvTgfPacketL2Part.daMac[jj];
            }

            rc = prvTgfBrgFdbMacEntrySet(&macEntry);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

            for(jj = 0; jj < 6; jj++)
            {
                if(jj == 0)
                {
                    /* the MAC address MSB will cause 'MCAST address' so do not
                       check those */
                    continue;
                }

                for(kk = 0 ; kk < FDB_MAC_COUNT_CNS; kk++)
                {
                    macEntry.key.key.macVlan.macAddr.arEther[jj] = (GT_U8)(kk);
/*                        prvTgfPacketL2Part.daMac[jj] + kk);*/

                    rc = prvTgfBrgFdbMacEntrySet(&macEntry);
                    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
                }

                macEntry.key.key.macVlan.macAddr.arEther[jj] =
                    prvTgfPacketL2Part.daMac[jj];
            }
        }
    }
    else
    {
        portsBmp.ports[0] = 0;
        portsBmp.ports[1] = 0;

        for(portIter = NOT_IN_TRUNK_PORTS_NUM_CNS; portIter < prvTgfPortsNum ; portIter++)
        {
            CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,prvTgfPortsArray[portIter]);
        }

        prvTgfTrunkCascadeTrunkPortsSet(prvTgfDevsArray[0],trunkId,&portsBmp);

        jj = 0;
        for(ii = 0; ii < 64 ; ii++)
        {
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp,ii))
            {
                continue;
            }

            if((jj & 1) == 0)
            {
                /* the first and third .. members */
                /* used when mode is PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E */

                /*find the port Iterator that match this port*/
                for(portIter = NOT_IN_TRUNK_PORTS_NUM_CNS; portIter < prvTgfPortsNum ; portIter++)
                {
                    if(prvTgfPortsArray[portIter] == ii)
                    {
                        break;
                    }
                }

                if(portIter == prvTgfPortsNum)
                {
                    UTF_VERIFY_EQUAL0_STRING_MAC(GT_OK, GT_BAD_STATE, "bad state");
                    return;
                }


                U32_SET_FIELD_MAC(membersIndexesBmp,portIter,1,1);
            }

            jj++;
        }

        prvTgfTrunkLoadBalanceSpecificMembersSet(membersIndexesBmp);
    }

    for(ii = 0 ; ii < prvTgfVlansNum ; ii++)
    {
        if(prvTgfVlansArr[ii] == 1)
        {
            /* default VLAN entry -- don't touch it */
            continue;
        }

        /* set VLAN entry -- with ports in test */
        rc = prvTgfBrgDefVlanEntryWrite(prvTgfVlansArr[ii]);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryWrite: %d",
                                     prvTgfVlansArr[ii]);
    }



    return;
}

/*******************************************************************************
* prvTgfTrunkCrcHashTestInit
*
* DESCRIPTION:
*   init for CRC hash feature test
*
*       applicable devices: Lion and above
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
void prvTgfTrunkCrcHashTestInit
(
    void
)
{
    GT_STATUS   rc;
    GT_U32  ii;

    prvTgfVlansNum = 2;

    prvTgfTrunkGenericInit(TRUNK_TEST_TYPE_CRC_HASH_E,GT_FALSE);
    /* set UDE registers */
    for(ii = 0 ; ii < NUM_UDE_CNS ; ii++)
    {
        rc = prvTgfPclUdeEtherTypeSet(ii,UDE_BASE_CNS + ii);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
    }

    rc = prvTgfTrunkHashGlobalModeSet(PRV_TGF_TRUNK_LBH_PACKETS_INFO_CRC_E);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    return;
}

/*******************************************************************************
* prvTgfTrunkFdBFlush
*
* DESCRIPTION:
*   flush the FDB
*
*       applicable devices: all DxCh devices
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
static void prvTgfTrunkFdBFlush
(
    void
)
{
    GT_STATUS   rc;
    GT_U32  numOfValid = 0 ;/* number of valid entries in the FDB */
    GT_U32  numOfSkip = 0; /* number of skipped entries in the FDB */
    GT_U32  numOfValidMinusSkip=0;/* number of (valid - skip) entries in the FDB*/
    GT_U32  maxFlush;/* up to 10 times flush FDB , due to still processing of AppDemo or Asic simulation */
#ifdef ASIC_SIMULATION
    maxFlush = 10;
#else
    maxFlush = 1;
#endif /*ASIC_SIMULATION*/

    TRUNK_PRV_UTF_LOG0_MAC("=======  : flush FDB =======\n");

    currPortGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    do{
        /* delete mac addresses , include static */
        rc = prvTgfBrgFdbFlush(GT_TRUE);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

        /* Check FDB capacity (FDB should be empty) */
        /* get FDB counters */
        rc = prvTgfBrgFdbCount(&numOfValid,&numOfSkip,NULL,NULL,NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

        numOfValidMinusSkip = numOfValid - numOfSkip;

        maxFlush--;

        if(numOfValidMinusSkip && maxFlush)
        {
            cpssOsTimerWkAfter(1000);
            utfPrintKeepAlive();
        }
    }while(numOfValidMinusSkip && maxFlush);

    UTF_VERIFY_EQUAL1_STRING_MAC(0, numOfValidMinusSkip,
                    "FDB not empty , got [%d] entries ",
                    numOfValidMinusSkip);

}

/*******************************************************************************
* prvTgfTrunkGenericRestore
*
* DESCRIPTION:
*   restore setting after trunk test
*
*       applicable devices: All DxCh devices
*
* INPUTS:
*       cascadeTrunk - do we destroy cascade trunk / regular trunk
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
static void prvTgfTrunkGenericRestore
(
    IN GT_BOOL cascadeTrunk
)
{
    GT_STATUS   rc;
    GT_U32  portIter;/* port iterator */
    GT_TRUNK_ID             trunkId;
    GT_U32  ii;


    /* restore first 4 ports of test */
    for(portIter = 0 ; portIter < TRUNK_8_PORTS_IN_TEST_CNS ; portIter++)
    {
        prvTgfPortsArray[portIter] = first4PortsOrig[portIter];
    }

    /* restore default number of ports */
    prvTgfPortsNum = PRV_TGF_DEFAULT_NUM_PORTS_IN_TEST_CNS;

    /* restore L2 header from original */
    prvTgfPacketL2Part = prvTgfPacketL2PartOrig;
    /* restore Vlan tag header from original */
    prvTgfPacketVlanTagPart = prvTgfPacketVlanTagPartOrig;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    TRUNK_PRV_UTF_LOG0_MAC("======= clear counters =======\n");
    /* clear also counters at end of test */
    for(portIter = 0; portIter < prvTgfPortsNum ; portIter++)
    {
        rc = prvTgfResetCountersEth(prvTgfDevsArray[portIter],
                                    prvTgfPortsArray[portIter]);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
    }

    rc = prvTgfTrunkHashGlobalModeSet(PRV_TGF_TRUNK_LBH_PACKETS_INFO_E);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    if(cascadeTrunk == GT_FALSE)
    {
        rc = prvTgfTrunkMembersSet(
            trunkId /*trunkId*/,
            0 /*numOfEnabledMembers*/,
            (CPSS_TRUNK_MEMBER_STC*)NULL /*enabledMembersArray*/,
            0 /*numOfDisabledMembers*/,
            (CPSS_TRUNK_MEMBER_STC*)NULL  /*disabledMembersArray*/);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
    }
    else
    {
        rc = prvTgfTrunkCascadeTrunkPortsSet(prvTgfDevNum,trunkId,NULL);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
    }

    prvTgfTrunkFdBFlush();

    for(ii = 0 ; ii < prvTgfVlansNum ; ii++)
    {
        if(prvTgfVlansArr[ii] == 1)
        {
            /* default VLAN entry -- don't touch it */
            continue;
        }

        /* invalidate the other vlans */
        rc = prvTgfBrgDefVlanEntryInvalidate(prvTgfVlansArr[ii]);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_OK, rc, "prvTgfBrgDefVlanEntryInvalidate: %d",
                                     prvTgfVlansArr[ii]);
    }

    /* restore default ethertype */
    tgfTrafficGeneratorEtherTypeForVlanTagSet(TGF_ETHERTYPE_8100_VLAN_TAG_CNS);

    return;
}
/*******************************************************************************
* prvTgfTrunkCrcHashTestRestore
*
* DESCRIPTION:
*   restore for CRC hash feature test
*
*       applicable devices: Lion and above
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
void prvTgfTrunkCrcHashTestRestore
(
    void
)
{
    GT_STATUS   rc;
    GT_U32  ii;

    prvTgfTrunkGenericRestore(GT_FALSE);

    /* restore UDE registers */
    for(ii = 0 ; ii < NUM_UDE_CNS ; ii++)
    {
        rc = prvTgfPclUdeEtherTypeSet(ii,0xFFFF);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
    }
}
/*******************************************************************************
* tgfTrunkDebugBurstNumSet
*
* DESCRIPTION:
*       debug function to run test of traffic with smaller burst -->
*       less number of entries to the FDB.
*       to to allow fast debug of error in test and not wait till end of traffic
*       learn...
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
GT_STATUS tgfTrunkDebugBurstNumSet
(
    IN GT_U32   burstNum
)
{
    debug_burstNum = burstNum;
    return GT_OK;
}

/*******************************************************************************
* tgfTrunkDebugCrcHash
*
* DESCRIPTION:
*       debug function to run test of CRC hash for specific
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
GT_STATUS tgfTrunkDebugCrcHash
(
    IN GT_U32   hashMaskTableIndex,
    IN GT_U32   hashMaskFieldType,
    IN GT_U32   hashMaskFieldSubType,
    IN GT_U32   hashMaskFieldByteIndex,
    IN GT_U32   pearsonMode,
    IN GT_U32   crcMode,
    IN GT_U32   seedIndex
)
{
    debugMode = 1;
    debug_hashMaskTableIndex     = hashMaskTableIndex      ;
    debug_hashMaskFieldType      = hashMaskFieldType       ;
    debug_hashMaskFieldSubType   = hashMaskFieldSubType    ;
    debug_hashMaskFieldByteIndex = hashMaskFieldByteIndex  ;
    debug_pearsonMode            = pearsonMode             ;
    debug_crcMode                = crcMode                 ;
    debug_seedIndex              = seedIndex               ;

    return GT_OK;
}


/*******************************************************************************
* prvTgfTrunkCrcHashMaskPriorityTest
*
* DESCRIPTION:
*   check CRC hash mask priority:
*       default of accessing the CRC mask hash table is :
*           'Traffic type' --> index 16..27
*       higher priority for the 'per port' --> index 0..15
*       highest priority to the TTI action --> index 1..15 (without index 0)
*
*       applicable devices: Lion and above
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
void prvTgfTrunkCrcHashMaskPriorityTest
(
    void
)
{
    GT_STATUS   rc;
    PRV_TGF_TRUNK_LBH_CRC_MODE_ENT  crcMode;
    GT_U32              seedIndex;
    PEARSON_TEST_MODE_ENT   pearsonMode;
    GT_U32              hashMaskTableIndex;/* index of entry in hash mask table - 0..27 */
    PRV_TGF_TRUNK_FIELD_TYPE_ENT hashMaskFieldType;/* index of field in the 70 bytes for hash - 0..69 */
    GT_U32              hashMaskFieldSubType;
    GT_U32              hashMaskFieldByteIndex;
    PRV_TGF_PCL_PACKET_TYPE_ENT     trafficType;/*traffic type*/
    GT_U8   devNum,portNum;/*device number to send packets , and port number */
    GT_U32  portIter;/* port iterator */
    GT_U32  portOverrideMaskEntryIndex;/* the 'per port' override index */
    PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT  lbhMode;/* LBH mode expected */

    crcMode = PRV_TGF_TRUNK_LBH_CRC_16_AND_PEARSON_MODE_E;
    seedIndex = 1;
    pearsonMode = PEARSON_TEST_MODE_INDEX_E;
    trafficType = PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E;
    hashMaskFieldType = PRV_TGF_TRUNK_FIELD_L4_PORT_E;
    hashMaskFieldSubType = PRV_TGF_TRUNK_FIELD_DIRECTION_SOURCE_E;
    hashMaskFieldByteIndex = 0;/*MSB of TCP port*/

    /* set new mode and seed */
    rc = prvTgfTrunkHashCrcParametersSet(crcMode,crcSeeds[seedIndex]);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    /* set Pearson table */
    rc = trunkHashPearsonFill(pearsonMode);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    /* clear full table */
    rc = trunkHashMaskCrcTableEntryClear(PRV_TGF_TRUNK_HASH_MASK_TABLE_INDEX_CLEAR_ALL_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    hashMaskTableIndex = trafficType + PRV_TGF_TRUNK_HASH_MASK_TABLE_TRAFFIC_TYPE_FIRST_INDEX_CNS;

    /* set the CRC hash mask table according to 'Traffic type' */
    rc = trunkHashMaskCrcFieldSet(
            hashMaskTableIndex,
            hashMaskFieldType,
            hashMaskFieldSubType,
            hashMaskFieldByteIndex);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EVEN_E;
    /* send traffic according to field checking */
    rc = trunkHashCrcLbhTrafficSend(
            prvTgfTrunkTrafficTypes[trafficType],
            trafficType,
            hashMaskFieldType,
            hashMaskFieldSubType,
            hashMaskFieldByteIndex,
            lbhMode);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    portIter = 0;
    devNum  = prvTgfDevsArray [portIter];
    portNum = prvTgfPortsArray[portIter];

    for(portOverrideMaskEntryIndex = 0 ;
        portOverrideMaskEntryIndex < PRV_TGF_TRUNK_HASH_MASK_TABLE_TRAFFIC_TYPE_FIRST_INDEX_CNS ;
        portOverrideMaskEntryIndex++)
    {
        /* set the 'src port' to be with 'Override mask index' */
        rc = prvTgfTrunkPortHashMaskInfoSet(devNum,portNum,GT_TRUE,portOverrideMaskEntryIndex);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

        /* clear full table */
        rc = trunkHashMaskCrcTableEntryClear(PRV_TGF_TRUNK_HASH_MASK_TABLE_INDEX_CLEAR_ALL_CNS);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

        hashMaskTableIndex = portOverrideMaskEntryIndex;

        /* set the CRC hash mask table according to 'Override index' */
        rc = trunkHashMaskCrcFieldSet(
                hashMaskTableIndex,
                hashMaskFieldType,
                hashMaskFieldSubType,
                hashMaskFieldByteIndex);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

        /* send traffic according to field checking */
        rc = trunkHashCrcLbhTrafficSend(
                prvTgfTrunkTrafficTypes[trafficType],
                trafficType,
                hashMaskFieldType,
                hashMaskFieldSubType,
                hashMaskFieldByteIndex,
                lbhMode);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
    }

    portOverrideMaskEntryIndex = 5;
    /* set the 'src port' to be with 'Override mask index' */
    rc = prvTgfTrunkPortHashMaskInfoSet(devNum,portNum,GT_FALSE,portOverrideMaskEntryIndex);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    /* clear full table */
    rc = trunkHashMaskCrcTableEntryClear(PRV_TGF_TRUNK_HASH_MASK_TABLE_INDEX_CLEAR_ALL_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    hashMaskTableIndex = portOverrideMaskEntryIndex;

    /* set the CRC hash mask table according to 'Traffic type' */
    rc = trunkHashMaskCrcFieldSet(
            hashMaskTableIndex,
            hashMaskFieldType,
            hashMaskFieldSubType,
            hashMaskFieldByteIndex);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    /* no LBH because traffic need to go to index by traffic type */
    lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E;

    /* send traffic according to field checking */
    rc = trunkHashCrcLbhTrafficSend(
            prvTgfTrunkTrafficTypes[trafficType],
            trafficType,
            hashMaskFieldType,
            hashMaskFieldSubType,
            hashMaskFieldByteIndex,
            lbhMode);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);


    CPSS_TBD_BOOKMARK/* add here the TTI action to check override of index 1..15 */
}

/*******************************************************************************
* tgfTrunkCascadeTrunkTestInit
*
* DESCRIPTION:
*   init for cascade trunks test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkCascadeTrunkTestInit
(
    void
)
{
    GT_STATUS   rc;
    GT_TRUNK_ID             trunkId;
    PRV_TGF_BRG_MAC_ENTRY_STC      macEntry = PRV_TGF_BRG_FDB_MAC_ENTRY_DEFAULT_MAC;
    GT_U32      ii,kk;
    GT_U8                        targetPortNum;
    CPSS_CSCD_LINK_TYPE_STC      cascadeLink;
    GT_U32  devTableArr[PRV_TGF_FDB_DEV_TABLE_SIZE_CNS];
    TGF_VFD_INFO_STC    vfd;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    prvTgfVlansNum = 2;

    prvTgfTrunkGenericInit(TRUNK_TEST_TYPE_CASCADE_TRUNK_E,GT_TRUE);

    rc = prvTgfTrunkHashGlobalModeSet(PRV_TGF_TRUNK_LBH_PACKETS_INFO_E);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);


    /* add the remote device to the FDB device table , so FDB entries on current
       device which are registered on the remote device will not be deleted by
       the aging daemon */
    rc = prvTgfBrgFdbDeviceTableGet(devTableArr);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    devTableArr[0] |= (1 << REMOTE_DEVICE_NUMBER_CNS);

    rc = prvTgfBrgFdbDeviceTableSet(devTableArr);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);


    /* set mac entries on 'Remote device' so we can point this device via the
       cascade trunk */
    macEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    macEntry.dstInterface.devPort.devNum = REMOTE_DEVICE_NUMBER_CNS;
    macEntry.dstInterface.devPort.portNum = 7;/* dummy port number */

    /* set the info for the increment of the mac */
    vfd.cycleCount = 6;
    vfd.mode = TGF_VFD_MODE_INCREMENT_E;
    vfd.modeExtraInfo = 0;
    vfd.offset = 0;
    cpssOsMemCpy(vfd.patternPtr, prvTgfPacketL2Part.daMac, sizeof(TGF_MAC_ADDR));

    /* make sure that ALL the mac DA that will be sent during the test will
       be known in the FDB on the needed vlans */
    for(ii = 0 ; ii < prvTgfVlansNum ; ii++)
    {
        cpssOsMemCpy(vfd.patternPtr, prvTgfPacketL2Part.daMac, sizeof(TGF_MAC_ADDR));
        macEntry.key.key.macVlan.vlanId = prvTgfVlansArr[ii];

        for(kk = 0 ; kk < FDB_MAC_COUNT_CNS; kk++)
        {
            /* increment the mac address */
            vfd.modeExtraInfo = kk;

            /* over ride the buffer with VFD info */
            rc = tgfTrafficEnginePacketVfdApply(&vfd,
                    macEntry.key.key.macVlan.macAddr.arEther,
                    sizeof(TGF_MAC_ADDR));
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

            rc = prvTgfBrgFdbMacEntrySet(&macEntry);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
        }
    }

    /* cascade link info */
    cascadeLink.linkNum = trunkId;
    cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;

    /* set this 'Remote device' to point to the cascade trunk */
    for(targetPortNum = 0 ;targetPortNum < 64;targetPortNum++)
    {
        rc = prvTgfCscdDevMapTableSet(prvTgfDevNum,REMOTE_DEVICE_NUMBER_CNS,
                targetPortNum,&cascadeLink,
                PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
    }

    return;
}

/*******************************************************************************
* tgfTrunkCascadeTrunkTestRestore
*
* DESCRIPTION:
*   restore values after cascade trunks test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkCascadeTrunkTestRestore
(
    void
)
{
    GT_STATUS   rc;
    GT_U8                        targetPortNum;
    CPSS_CSCD_LINK_TYPE_STC      cascadeLink;
    GT_U32  devTableArr[PRV_TGF_FDB_DEV_TABLE_SIZE_CNS];

    /* remove the remote device from the FDB device table */
    /* do it before FDB flush that is done in prvTgfTrunkGenericRestore(...)
       otherwise the 'Remote device entries' will not be deleted */
    rc = prvTgfBrgFdbDeviceTableGet(devTableArr);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    devTableArr[0] &= ~(1 << REMOTE_DEVICE_NUMBER_CNS);

    rc = prvTgfBrgFdbDeviceTableSet(devTableArr);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    prvTgfTrunkGenericRestore(GT_TRUE);

    /* cascade link info */
    cascadeLink.linkNum = CPSS_NULL_PORT_NUM_CNS;
    cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_PORT_E;

    /* set this 'Remote device' to point 'NULL port' */
    for(targetPortNum = 0 ;targetPortNum < 64;targetPortNum++)
    {
        rc = prvTgfCscdDevMapTableSet(prvTgfDevNum,REMOTE_DEVICE_NUMBER_CNS,
                targetPortNum,&cascadeLink,
                PRV_TGF_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
    }

    /* set the desigMode table */
    rc = prvTgfTrunkHashDesignatedTableModeSet(PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);


}
/*******************************************************************************
* tgfTrunkCascadeTrunkTest
*
* DESCRIPTION:
*   The cascade trunks test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkCascadeTrunkTest
(
    void
)
{
    GT_STATUS   rc;
    TGF_PACKET_STC      *trafficPtr;/* pointer to traffic to sent */
    GT_U8   devNum,portNum;/*device number to send packets , and port number */
    GT_U32  portIter;/* port iterator */
    GT_U32  numberOfPortsSending;/* number of ports sending traffic */
    GT_U32  numVlans;/* number of vlans to test */
    GT_U32               burstCount;/* burst count */
    GT_U32               numVfd = 1;/* number of VFDs in vfdArray */
    TGF_VFD_INFO_STC     vfdArray[1];/* vfd Array -- used for increment the tested bytes , and for vlan tag changing */
    TGF_VFD_INFO_STC     *vrfPtr = &vfdArray[0];
    PRV_TGF_PCL_PACKET_TYPE_ENT     trafficType;/*traffic type*/
    PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT  lbhMode;/* LBH mode expected */
    GT_U32  ii;
    PRV_TGF_TRUNK_DESIGNATED_TABLE_MODE_ENT desigMode;/* designated table mode */


    /*****************/
    /* test known UC */
    /*****************/
    trafficType = PRV_TGF_PCL_PACKET_TYPE_IPV4_TCP_E;
    trafficPtr = prvTgfTrunkTrafficTypes[trafficType];

    utfGeneralStateMessageSave(0,"trafficType = %d",trafficType);

    vrfPtr->mode = TGF_VFD_MODE_INCREMENT_E;
    vrfPtr->modeExtraInfo = 0;
    vrfPtr->offset = 0;/* mac DA field */
    cpssOsMemCpy(vrfPtr->patternPtr,prvTgfPacketL2Part.daMac,sizeof(TGF_MAC_ADDR));
    vrfPtr->cycleCount = 6;/*6 bytes*/

    burstCount = FDB_MAC_COUNT_CNS;

    if(debug_burstNum)
    {
        burstCount = debug_burstNum;
    }

    for(desigMode = 0 ;
        desigMode <= PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E ;
        desigMode++)
    {
        utfGeneralStateMessageSave(1,"desigMode = %d %s",desigMode,
            TO_STRING_DESIGNATED_TABLE_MODE_MAC(desigMode)
            );

        /* known UC is not influenced by those designated table modes */
        lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EVEN_E;
        numberOfPortsSending = NOT_IN_TRUNK_PORTS_NUM_CNS;
        numVlans = prvTgfVlansNum;

        switch(desigMode)
        {
            case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E:
                /* known UC also get impact by this mode */
                lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E;
                break;
            case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E:
                /* send from single vlan , so we will not have LBH due to vlan changing*/
                numVlans = 1;
                break;
            case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E:
                if(GT_FALSE == prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E))
                {
                    /* the device not support this mode */
                    continue;
                }
                /* send from single port , so we will not have LBH due to port changing*/
                numberOfPortsSending = 1;

                break;
            default:
                break;
        }

        utfGeneralStateMessageSave(2,"lbhMode = %d %s",lbhMode,
            TO_STRING_LBH_MODE_MAC(lbhMode)
            );

        /* set the desigMode table */
        rc = prvTgfTrunkHashDesignatedTableModeSet(desigMode);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);


        for(portIter = 0 ; portIter < numberOfPortsSending; portIter++)
        {
            devNum  = prvTgfDevsArray [portIter];
            portNum = prvTgfPortsArray[portIter];

            for(ii = 0 ; ii < numVlans ; ii++)
            {
                /* modify the vid part */
                prvTgfPacketVlanTagPart.vid = (TGF_VLAN_ID)prvTgfVlansArr[ii];

                vrfPtr->modeExtraInfo = 0;
                /* send the burst of packets with incremental byte */
                prvTgfTrunkTestPacketSend(devNum, portNum, trafficPtr ,burstCount ,numVfd ,vfdArray);
            }
        }

        /* check the LBH and no flooding */
        trunkSendCheckLbh(GT_TRUE,numberOfPortsSending,burstCount * numVlans ,lbhMode,NULL);
    }

}

/*******************************************************************************
* tgfTrunkDesignatedTableModesTestInit
*
* DESCRIPTION:
*   init for designated table modes test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkDesignatedTableModesTestInit
(
    void
)
{
    prvTgfTrunkGenericInit(TRUNK_TEST_TYPE_DESIGNATED_TABLE_MODES_E,GT_FALSE);

    /* flush the FDB because we test 'Multi-destination traffic' */
    prvTgfTrunkFdBFlush();

}


/*******************************************************************************
* tgfTrunkDesignatedTableModesTestRestore
*
* DESCRIPTION:
*   restore values after designated table modes test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkDesignatedTableModesTestRestore
(
    void
)
{
    GT_STATUS   rc;

    prvTgfTrunkGenericRestore(GT_FALSE);

    /* set the desigMode table */
    rc = prvTgfTrunkHashDesignatedTableModeSet(PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
}


/*******************************************************************************
* tgfTrunkDesignatedTableModesTest
*
* DESCRIPTION:
*   The designated table modes test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkDesignatedTableModesTest
(
    void
)
{
    GT_STATUS   rc;
    TGF_PACKET_STC      *trafficPtr;/* pointer to traffic to sent */
    GT_U8   devNum,portNum;/*device number to send packets , and port number */
    GT_U32  portIter;/* port iterator */
    GT_U32  numberOfPortsSending;/* number of ports sending traffic */
    GT_U32  numVlans;/* number of vlans to test */
    GT_U32               burstCount;/* burst count */
    PRV_TGF_PCL_PACKET_TYPE_ENT     trafficType;/*traffic type*/
    PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT  lbhMode;/* LBH mode expected */
    GT_U32  ii;
    PRV_TGF_TRUNK_DESIGNATED_TABLE_MODE_ENT desigMode;/* designated table mode */
    PRV_TGF_MULTI_DESTINATION_TYPE_ENT  multiDestType;/* multi destination traffic type */
    GT_U32  multiDestCount;/* number of destination traffics */
    enum {LBH_FALSE_E,LBH_TRUE_E}   lbhGenerated;

    /*******************/
    /* test unknown UC */
    /*******************/

    trafficType = PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E;
    trafficPtr = prvTgfTrunkTrafficTypes[trafficType];

    utfGeneralStateMessageSave(0,"trafficType = %d",trafficType);

    for(desigMode = 0 ;
        desigMode <= PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E ;
        desigMode++)
    {
        if(debugMode)
        {
            desigMode = debug_desigMode;
        }

        utfGeneralStateMessageSave(1,"desigMode = %d %s",desigMode,
            TO_STRING_DESIGNATED_TABLE_MODE_MAC(desigMode)
            );

        if(desigMode == PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E)
        {
            if(GT_FALSE == prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E))
            {
                /* the device not support this mode */
                continue;
            }
        }

        /* set the desigMode table */
        rc = prvTgfTrunkHashDesignatedTableModeSet(desigMode);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

        for(lbhGenerated = LBH_FALSE_E ;
            lbhGenerated <= LBH_TRUE_E;
            lbhGenerated++)
        {
            if(debugMode)
            {
                lbhGenerated = debug_lbhGenerated;
            }

            utfGeneralStateMessageSave(2,"lbhGenerated = %d %s",lbhGenerated,
                lbhGenerated == LBH_FALSE_E ? "LBH_FALSE_E" : "LBH_TRUE_E"
                );


            lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EVEN_E;
            numberOfPortsSending = NOT_IN_TRUNK_PORTS_NUM_CNS;
            numVlans = prvTgfVlansNum;
            burstCount = CRC_HASH_BURST_COUNT_NEW_CNS / MULTI_DEST_COUNT_MAC;

            if(debug_burstNum)
            {
                burstCount = debug_burstNum;
            }

            if(lbhGenerated == LBH_FALSE_E)
            {
                /* we want to send traffic that will go to single port in trunk */

                switch(desigMode)
                {
                    case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E:
                        lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E;
                        break;
                    case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E:
                        burstCount = 1;
                        break;
                    case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E:
                        burstCount = 1;
                        numVlans = 1;
                        break;
                    case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E:
                        if(GT_FALSE == prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E))
                        {
                            /* the device not support this mode */
                            continue;
                        }
                        numberOfPortsSending = 1;
                        numVlans = 1;
                        break;
                    default:
                        break;
                }

                lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E;

                multiDestType  = PRV_TGF_MULTI_DESTINATION_TYPE_BC_E;
                multiDestCount = 1;
            }
            else
            {
                switch(desigMode)
                {
                    CPSS_COVERITY_NON_ISSUE_BOOKMARK
                    /* coverity[dead_error_begin] */
                    case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_FIRST_INDEX_E:
                        /* already tested in lbhGenerated == LBH_FALSE_E */
                        continue;
                    case PRV_TGF_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E:
                        if(GT_FALSE == prvUtfDeviceTestNotSupport(prvTgfDevNum, UTF_CH1_E | UTF_CH1_DIAMOND_E | UTF_CH2_E | UTF_CH3_E | UTF_XCAT_E))
                        {
                            /* the device not support this mode */
                            continue;
                        }
                        break;
                    default:
                        break;
                }

                multiDestType = PRV_TGF_MULTI_DESTINATION_TYPE_ALL_E;
                multiDestCount =  MULTI_DEST_COUNT_MAC;
            }

            utfGeneralStateMessageSave(3,"lbhMode = %d %s",lbhMode,
                TO_STRING_LBH_MODE_MAC(lbhMode)
                );

            for(portIter = 0 ; portIter < numberOfPortsSending; portIter++)
            {
                devNum  = prvTgfDevsArray [portIter];
                portNum = prvTgfPortsArray[portIter];

                for(ii = 0 ; ii < numVlans ; ii++)
                {
                    /* modify the vid part */
                    prvTgfPacketVlanTagPart.vid = (TGF_VLAN_ID)prvTgfVlansArr[ii];

                    /* send the multi-destination traffic */
                    /* NOTE: this function sent 4 types of traffic with the burst count !!! */
                    prvTgfCommonMultiDestinationTrafficTypeSend(devNum, portNum, burstCount ,
                                                                GT_FALSE, trafficPtr , multiDestType);

                }
            }

            /* check the LBH and flooding */
            trunkSendCheckLbh(GT_FALSE,numberOfPortsSending,burstCount * numVlans * multiDestCount,lbhMode,NULL);

            IF_DEBUG_MODE_BREAK_MAC(debugMode);
        }
        IF_DEBUG_MODE_BREAK_MAC(debugMode);
    }

}

/*******************************************************************************
* tgfTrunkDebugDesignatedTable
*
* DESCRIPTION:
*       debug function to run test of the designated table for specific input
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
GT_STATUS tgfTrunkDebugDesignatedTable
(
    IN GT_U32   desigMode,
    IN GT_U32   lbhGenerated
)
{
    debugMode = 1;
    debug_desigMode       = desigMode       ;
    debug_lbhGenerated    = lbhGenerated    ;

    return GT_OK;
}

/*******************************************************************************
* tgfTrunkMcLocalSwitchingTestInit
*
* DESCRIPTION:
*   init for trunk multi-destination local switching test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkMcLocalSwitchingTestInit
(
    void
)
{
    prvTgfVlansNum = 2;

    prvTgfTrunkGenericInit(TRUNK_TEST_TYPE_MC_LOCAL_SWITCHING_E,GT_FALSE);

    /* flush the FDB because we test 'Multi-destination traffic' */
    prvTgfTrunkFdBFlush();
}


/*******************************************************************************
* tgfTrunkMcLocalSwitchingTestRestore
*
* DESCRIPTION:
*   restore values after trunk multi-destination local switching test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkMcLocalSwitchingTestRestore
(
    void
)
{
    GT_U32  portIter;/* port iterator */
    GT_TRUNK_ID             trunkId;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    prvTgfTrunkGenericRestore(GT_FALSE);

    /* DO NOT allow multi-destination to flood back - on the trunk */
    prvTgfTrunkMcLocalSwitchingEnableSet(trunkId,GT_FALSE);

    /* DO NOT allow multi-destination to flood back - on the port */
    portIter = NOT_IN_TRUNK_PORTS_NUM_CNS;
    prvTgfBrgPortEgressMcLocalEnable(prvTgfDevsArray [portIter],
                                      prvTgfPortsArray[portIter],
                                      GT_FALSE);


}

/*******************************************************************************
* mcLocalSwitchingSendAndCheck
*
* DESCRIPTION:
*   the function sends multi-destination packets from all port of the trunk ,
*   and check that the trunk get/not get packets flooded back to the trunk.
*   the function also check that all other ports that are not in trunk will get
*   the flooding
*
*       applicable devices: ALL DxCh devices
*
* INPUTS:
*       lastPortIndex - index of last port in trunk
*       lbhMode  - the mode of expected load balance .
*                   all traffic expected to be received from single port or
*                   not received at all.
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void mcLocalSwitchingSendAndCheck
(
    IN GT_U32  lastPortIndex,
    IN PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT  lbhMode
)
{
    GT_STATUS   rc;
    PRV_TGF_PCL_PACKET_TYPE_ENT     trafficType;/*traffic type*/
    GT_U8   devNum,portNum;/*device number to send packets , and port number */
    GT_U32  portIter;/* port iterator */
    GT_U32               burstCount;/* burst count */
    TGF_PACKET_STC      *trafficPtr;/* pointer to traffic to sent */
    GT_U32  multiDestCount;/* number of destination traffics */
    GT_U32      ii,jj;
    GT_U32      maxPackets;
    GT_TRUNK_ID             trunkId;
    PRV_TGF_MULTI_DESTINATION_TYPE_ENT  multiDestType;/*multi destination traffic type*/
    CPSS_TRUNK_MEMBER_STC trunkMemberSender;/*indicates that the traffic to check
                   LBH that egress the trunk was originally INGRESSED from the trunk.
                   but since in the 'enhanced UT' the CPU send traffic to a port
                   due to loopback it returns to it, we need to ensure that the
                   member mentioned here should get the traffic since it is the
                   'original sender'*/

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    trafficType = PRV_TGF_PCL_PACKET_TYPE_IPV4_UDP_E;
    trafficPtr = prvTgfTrunkTrafficTypes[trafficType];

    burstCount = 1;/* single packet -- so there is no LBH back into the trunk */
    maxPackets = 3;/* number of times to send the same packet */

    for(jj = 0 ; jj < prvTgfVlansNum ; jj++)
    {
        /* modify the vid part */
        prvTgfPacketVlanTagPart.vid = (TGF_VLAN_ID)prvTgfVlansArr[jj];
        /* send all types of multi-destination */
        for(multiDestType = 0 ; multiDestType < PRV_TGF_MULTI_DESTINATION_TYPE_ALL_E ;
            multiDestType +=2)/* reduce time of test*/
        {
            for(portIter = NOT_IN_TRUNK_PORTS_NUM_CNS; portIter <= lastPortIndex ;
                portIter+=2)/* reduce time of test*/
            {
                devNum  = prvTgfDevsArray [portIter];
                portNum = prvTgfPortsArray[portIter];

                /* clear all counters before sending traffic */
                for(ii = 0; ii < prvTgfPortsNum ; ii++)
                {
                    rc = prvTgfResetCountersEth(prvTgfDevsArray[ii],
                                                prvTgfPortsArray[ii]);
                    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
                }

                for(ii = 0 ; ii < maxPackets ; ii++)
                {
                   /* send the multi-destination traffic */
                    prvTgfCommonMultiDestinationTrafficTypeSend(devNum, portNum, burstCount ,
                                                                GT_FALSE, trafficPtr , multiDestType);
                }

                multiDestCount = 1;

                /* state that the traffic ingress from the trunk (but for that we send
                   it from CPU , to egress from this port) */
                trunkMemberSender.port = portNum;
                trunkMemberSender.device = devNum;

                /* check the LBH and flooding / no flooding */
                trunkSendCheckLbh(GT_FALSE,/* Multi-destination */
                        1,
                        burstCount * maxPackets * multiDestCount,
                        lbhMode,
                        &trunkMemberSender);
            }
        }
    }
}

/*******************************************************************************
* basicTrunkMcLocalSwitchingTest
*
* DESCRIPTION:
*   The trunk multi-destination local switching test
*
*       applicable devices: ALL DxCh devices
*
*       The test:
*
* ========== section 1 ===========
*    a. set trunk with 2 ports
*    b. send flooding from it   --> check no flood back
*    c. 'disable filter' on the trunk
* ========== section 2 ===========
*    d. send flooding from it   --> check flood back to only one of the 2 ports of the trunk
*    e. add port to the trunk
*    f. send flooding from it   --> check flood back to only one of the 3 ports of the trunk
*    g. 'enable filter' on the trunk
*    h. send flooding from it   --> check no flood back
*    i. 'disable filter' on the trunk
*    j. send flooding from it   --> check flood back to only one of the 3 ports of the trunk
*    k. remove port from the trunk
*    l. send flooding from it   --> check flood back to only one of the 2 ports of the trunk, check that the removed port get flood.
*
* INPUTS:
*       sectionNumber - section number:
*                    section 1 : a..c
*                    section 2 : d..l
*       clearTheTrunk - do we need to clear the trunk.
*                       note : on section 1 we clear the trunk anyway.
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void basicTrunkMcLocalSwitchingTest
(
    GT_U32      sectionNumber,
    GT_BOOL     clearTheTrunk
)
{
    GT_STATUS rc;
    GT_U32  portIter;/* port iterator */
    GT_U32  lastPortIndex;/* index of last port in trunk */
    PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT  lbhMode;
    GT_U32  numMembers;/* number of current trunk members */
    CPSS_TRUNK_MEMBER_STC member;/*trunk member*/
    GT_TRUNK_ID             trunkId;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* work with 3 members in the trunk */
    numMembers = 3;
    lastPortIndex = NOT_IN_TRUNK_PORTS_NUM_CNS + numMembers - 1;

    if(clearTheTrunk == GT_TRUE || sectionNumber == 1)
    {
        /* clear the trunk */
        rc = prvTgfTrunkMembersSet(
            trunkId /*trunkId*/,
            0 /*numOfEnabledMembers*/,
            (CPSS_TRUNK_MEMBER_STC*)NULL /*enabledMembersArray*/,
            0 /*numOfDisabledMembers*/,
            (CPSS_TRUNK_MEMBER_STC*)NULL  /*disabledMembersArray*/);
        UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

        /* a. add 3 members to the trunk */
        numMembers = 3;

        for(portIter = NOT_IN_TRUNK_PORTS_NUM_CNS;
            portIter < (NOT_IN_TRUNK_PORTS_NUM_CNS + numMembers) ;
            portIter++)
        {
            member.device = prvTgfDevsArray [portIter];
            member.port   = prvTgfPortsArray[portIter];

            rc = prvTgfTrunkMemberAdd(trunkId,&member);
            UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);
        }

        if(sectionNumber == 1)
        {
            lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EMPTY_E;

            /* b. send traffic from the trunk --> check no flood back to the trunk */
            mcLocalSwitchingSendAndCheck(lastPortIndex,lbhMode);

            /* c. allow multi-destination to flood back */
            prvTgfTrunkMcLocalSwitchingEnableSet(trunkId,GT_TRUE);

            /* section 1 -- done*/
            return;
        }
    }

    /* section 2 */
    portIter = (NOT_IN_TRUNK_PORTS_NUM_CNS + numMembers);

    lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E;
    /* d. send traffic from the trunk --> check flood back to the trunk --> single port */
    mcLocalSwitchingSendAndCheck(lastPortIndex,lbhMode);

    /* e. add port to trunk */
    member.device = prvTgfDevsArray [portIter];
    member.port   = prvTgfPortsArray[portIter];

    rc = prvTgfTrunkMemberAdd(trunkId,&member);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    numMembers ++;
    lastPortIndex = NOT_IN_TRUNK_PORTS_NUM_CNS + numMembers - 1;

    lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E;
    /* f. send traffic from the trunk --> check flood back to the trunk --> single port */
    mcLocalSwitchingSendAndCheck(lastPortIndex,lbhMode);

    /* g. don't allow multi-destination to flood back */
    prvTgfTrunkMcLocalSwitchingEnableSet(trunkId,GT_FALSE);

    lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EMPTY_E;
    /* h. send traffic from the trunk --> check no flood back to the trunk */
    mcLocalSwitchingSendAndCheck(lastPortIndex,lbhMode);

    /* i. allow multi-destination to flood back */
    prvTgfTrunkMcLocalSwitchingEnableSet(trunkId,GT_TRUE);

    lbhMode = PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E;
    /* j. send traffic from the trunk --> check flood back to the trunk --> single port */
    mcLocalSwitchingSendAndCheck(lastPortIndex,lbhMode);

    /* k. remove the last port from the trunk*/
    rc = prvTgfTrunkMemberRemove(trunkId,&member);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    numMembers --;
    lastPortIndex = NOT_IN_TRUNK_PORTS_NUM_CNS + numMembers - 1;

    /* l. send traffic from the trunk --> check flood back to the trunk --> single port */
    mcLocalSwitchingSendAndCheck(lastPortIndex,lbhMode);


}
/*******************************************************************************
* tgfTrunkMcLocalSwitchingTest
*
* DESCRIPTION:
*   The trunk multi-destination local switching test
*
*       applicable devices: ALL DxCh devices
*
*       The test:
* ========== section 1 ===========
*    a. set trunk with 2 ports
*    b. send flooding from it   --> check no flood back
*    c. 'disable filter' on the trunk
* ========== section 2 ===========
    *    d. send flooding from it   --> check flood back to only one of the 2 ports of the trunk
    *    e. add port to the trunk
    *    f. send flooding from it   --> check flood back to only one of the 3 ports of the trunk
    *    g. 'enable filter' on the trunk
    *    h. send flooding from it   --> check no flood back
    *    i. 'disable filter' on the trunk
    *    j. send flooding from it   --> check flood back to only one of the 3 ports of the trunk
    *    k. remove port from the trunk
    *    l. send flooding from it   --> check flood back to only one of the 2 ports of the trunk, check that the removed port get flood.
* ========== permutations ===========
*    m. remove all ports from trunk and than add 2 or 3 ports, back to existing test phases from <d>
*    n. Add manipulation of cpssDxChBrgPortEgressMcastLocalEnable() ***> prvTgfBrgPortEgressMcLocalEnable()
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
void tgfTrunkMcLocalSwitchingTest
(
    void
)
{
    GT_STATUS rc;
    GT_U32  portIter;/* port iterator */
    GT_TRUNK_ID             trunkId;

    trunkId = PRV_TGF_TRUNK_ID_CNS;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* run section 1 (a..c) */
    basicTrunkMcLocalSwitchingTest(1,GT_TRUE);
    /* run section 2 (d..l) */
    basicTrunkMcLocalSwitchingTest(2,GT_FALSE);

    /* m. remove all ports from trunk and than add 2 or 3 ports, run section 2 again . */
    /* motivation of this section is to check that if we remove all ports from
       the trunk , still the trunk keeps the settings of 'multi-destination local switching enable' settings

       and at this point the trunk is set as <multi-destination local switching enable> = GT_TRUE
    */

    /* run section 2 (d..l) starting with clearing the trunk and add members */
    basicTrunkMcLocalSwitchingTest(2,GT_TRUE);

    /* n. Add manipulation of cpssDxChBrgPortEgressMcastLocalEnable() ***> prvTgfBrgPortEgressMcLocalEnable()*/
    /* enable switch back on PORT that is member of the trunk  */
    portIter = NOT_IN_TRUNK_PORTS_NUM_CNS;
    rc = prvTgfBrgPortEgressMcLocalEnable(prvTgfDevsArray [portIter],
                                          prvTgfPortsArray[portIter],
                                          GT_TRUE);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, rc);

    /* o. DONT allow multi-destination to flood back */
    prvTgfTrunkMcLocalSwitchingEnableSet(trunkId,GT_FALSE);

    /* p. run section 1 (a..c) --> check that no flood back to the trunk */
    basicTrunkMcLocalSwitchingTest(1,GT_TRUE);


}

/* debug function to change port numbers for trunk UTs */
GT_STATUS prvUtfTrunkTestPortSet(GT_BOOL is64Ports, GT_BOOL isCrcOrForcePorts, GT_U32 index, GT_U8 portNum)
{
    if (index >= TRUNK_8_PORTS_IN_TEST_CNS)
    {
        return GT_FAIL;
    }

    if (is64Ports == 0)
    {
        if (isCrcOrForcePorts == 0)
        {
            testPortsFor28Ports[index] = portNum;
        }
        else
        {
            crcTestPortsFor28Ports[index] = portNum;
        }
    }
    else
    {
        if (isCrcOrForcePorts == 0)
        {
            testPortsFor60Ports[index] = portNum;
        }
        else
        {
            testPortsFor60PortsConfigForced[index] = portNum;
        }
    }

    return GT_OK;
}

